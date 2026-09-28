#!/usr/bin/env python3
"""zensus_lib.py - gemeinsame Grundlage des Tuer-Zensus (Runde 31, Teil T1).

Nur LESEND gegenueber allen Originaldaten. Keine Konstante hier steuert Spielverhalten;
alles ist Messwerkzeug. Herkunft jeder Formatannahme steht am Feld.

RE1.5 Door_aot_set (Opcode 0x3B), Laenge 32 bzw. 40 Byte:
  Handler @0x800405bc: speichert NUR den Zeiger pc+2 in die AOT-Tabelle 0x800ac9b0[pc[1]]
  (@0x800405c4 lbu v0,1(v0) / @0x80040600..0c sw v0=pc+2). Vorschub @0x80040618..34:
  lbu v0,3(v1); andi 0x80; +40 wenn gesetzt, sonst +32.
  Satz s0 = pc+2. Scan @0x80042c50..:
    s0[0] = pc[2] sce          (Sprung ueber Tabelle, @0x80042f74 lbu v0,0(s0))
    s0[1] = pc[3] sat          (Bit 0x80 = Viereck statt Rechteck, @0x80042f04/0x80042f68)
    s0[2] = pc[4] Band         (@0x80042cac lbu v0,2(s0): Bit 0x80 = jedes Band, sonst ==
                                Spielerband obj+0x82, @0x80042cc0..cc)
  Rechteckform (sat & 0x80 == 0): Trefftest FUN_80042b64, pc+6 s16 x, +8 s16 z, +10 u16 w,
    +12 u16 d; Nutzlast a0 = s0+12 = pc+14 (@0x80042fb8 addiu a0,s0,12).
  Viereckform (sat & 0x80): Trefftest FUN_80014368 liest 4 Punkte (x,z) s16 ab s0+4 = pc+6
    (@0x80014368 lh t5,4(a1) ... @0x800143a0 lh v1,18(a1)); Nutzlast a0 = s0+20 = pc+22
    (@0x80042f90 addiu a0,s0,20).
  Nutzlast (relativ n): +0/+2/+4 s16 Ziel x/y/z, +6 s16 Ziel-Richtung, +8 Stage, +9 Raum,
    +10 Cut, +11 Ziel-Band, +12 Archiv (RE1.5-Lader @0x8001720c lbu v0,12(v0)),
    +13 Variante (@0x800164a8 lbu v0,13(v0)), +14..+17 Rest.
RE2 Door_aot_set 0x3B (32 B) / Door_aot_set_4p 0x68 (40 B): dieselbe Anordnung
  (Laengen aus re2_scd_lens.TABLE, Archiv = Nutzlast +12 = pc+26 bzw. pc+34).
"""
import glob
import math
import os
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
for _p in (os.path.join(REPO, "re15_port", "tools"), os.path.join(REPO, "re15_port", "tools", "tor"),
           os.path.join(REPO, "analysis", "nutzer_batch_2026-08-27", "tools")):
    if _p not in sys.path:
        sys.path.insert(0, _p)

import tor_kamera as K          # noqa: E402  (nur lesend)

AUS = os.path.join(REPO, "build", "r31_tueren", "t1")
BAND = 1800                     # Boden y = -Band*1800 (06_massstab §2, bodenzensus 324/326)
BLATT_H = 3549                  # Blatt im Raum (06_massstab §1: 6602 / 1,86)
BLATT_B = 1950                  # RE1.5-Blattbreite (06_massstab §1: 1953 +- 167)
RE2_BLATT = (128, 219)          # Blattbereich der RE2-Tuertextur: v 0..218 (04_tuerkatalog §1.2)


def rel(p):
    return os.path.relpath(p, REPO).replace("\\", "/")


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


# ----------------------------------------------------------------------------
# Tuersatz lesen (beide Spiele)
# ----------------------------------------------------------------------------
def tuersatz(d, pc):
    """-> dict des Door_aot_set bei Datei-Offset pc (RE1.5 0x3B oder RE2 0x3B/0x68)."""
    op = d[pc]
    vier = bool(d[pc + 3] & 0x80) if op == 0x3B else (op == 0x68)
    if vier:
        pts = [(s16(struct.unpack_from("<H", d, pc + 6 + 4 * i)[0]),
                s16(struct.unpack_from("<H", d, pc + 8 + 4 * i)[0])) for i in range(4)]
        n = pc + 22
        laenge = 40
        rect = None
    else:
        x, z, w, dp = struct.unpack_from("<hhHH", d, pc + 6)
        rect = (x, z, w, dp)
        pts = [(x, z), (x + w, z), (x + w, z + dp), (x, z + dp)]
        n = pc + 14
        laenge = 32
    nx, ny, nz, nd = struct.unpack_from("<hhhh", d, n)
    return dict(pc=pc, op=op, laenge=laenge, roh=d[pc:pc + laenge].hex(" "),
                slot=d[pc + 1], sce=d[pc + 2], sat=d[pc + 3], band=d[pc + 4], folge=d[pc + 5],
                form="viereck" if vier else "rechteck", rect=rect, pts=pts,
                ziel=(nx, ny, nz), ziel_dir=nd & 0xFFFF,
                ziel_stage=d[n + 8], ziel_raum=d[n + 9], ziel_cut=d[n + 10], ziel_band=d[n + 11],
                archiv=d[n + 12], variante=d[n + 13], rest=d[n + 14:n + 18].hex(" "),
                nutzlast_off=n)


def raumname(stage_byte, raum_byte):
    """Stage-Byte 0 = STAGE1: 'SRR' (ohne Variante)."""
    return "%X%02X" % (stage_byte + 1, raum_byte)


# ----------------------------------------------------------------------------
# RE1.5
# ----------------------------------------------------------------------------
def re15_rdts():
    return sorted(glob.glob(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE*", "ROOM*.RDT")))


def re15_tueren(d):
    """Alle Door_aot_set einer RE1.5-RDT, je Datei-Offset einmal, mit Fundstelle."""
    import scd_walk_lib as W
    aus, gesehen = [], set()
    for (tag, idx), ops in sorted(W.regionen(d).items()):
        for pc, op, sz in ops:
            if op == 0x3B and pc not in gesehen:
                gesehen.add(pc)
                t = tuersatz(d, pc)
                assert t["laenge"] == sz, (pc, sz)
                t["skript"] = "%s%02d" % (tag, idx)
                aus.append(t)
    return aus


def re2_tueren(d):
    """Alle Door_aot_set (0x3B) und Door_aot_set_4p (0x68) einer RE2-RDT."""
    import re2_scd_walk as W2
    offs = struct.unpack_from("<23I", d, 8)
    aus, gesehen = [], set()
    for nm, base, subs in W2.scd_blocks(d):
        for i in range(len(subs)):
            s = base + subs[i]
            e = W2.block_end(d, base, subs, i, offs)
            if e <= s or e > len(d):
                continue
            ops, st = W2.walk(d, s, e, i + 1 == len(subs))
            for (pc, op, ln) in ops:
                if op in (0x3B, 0x68) and pc not in gesehen:
                    gesehen.add(pc)
                    t = tuersatz(d, pc)
                    assert t["laenge"] == ln, (pc, ln)
                    t["skript"] = "%s%02d" % (nm, i)
                    aus.append(t)
    return aus


class Raum(object):
    """Ein Raum eines Spiels mit Kameras, Hintergruenden, SCA und RVD-Kamerazonen."""

    def __init__(self, spiel, name):
        self.spiel = spiel
        self.name = name.upper()                 # z.B. '1000' (mit Variante)
        st, rr = self.name[0], self.name[1:3]
        if spiel == "re2":
            self.pfad = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM%s.RDT" % self.name)
            self.bg = os.path.join(REPO, "info", "re2leon", "COMMON", "BSS", "ROOM%s%s" % (st, rr),
                                   "ROOM%s%s%%02d.bmp" % (st, rr))
        else:
            self.pfad = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE%s" % st,
                                     "ROOM%s.RDT" % self.name)
            self.bg = os.path.join(REPO, "extracted", "PSX", "STAGE%s" % st, "ROOM%s%s" % (st, rr),
                                   "ROOM%s%s%%02d.bmp" % (st, rr))
        self.d = open(self.pfad, "rb").read()
        self.ncut = self.d[1]
        if spiel == "re2":
            self.offs = struct.unpack_from("<23I", self.d, 8)
            self.cam_off = self.offs[7]
            self.rvd_off = self.offs[8]
        else:
            self.cam_off = struct.unpack_from("<I", self.d, 0x24)[0]
            self.rvd_off = struct.unpack_from("<I", self.d, 0x28)[0]
        self._bg = {}

    def kamera(self, cut, exakt=True):
        o = self.cam_off + 32 * cut
        return K.Kamera(cut, o, self.d[o:o + 32], exakt=exakt)

    def hat_bild(self, cut):
        return os.path.exists(self.bg % cut)

    def hintergrund(self, cut):
        if cut not in self._bg:
            from PIL import Image
            self._bg[cut] = np.asarray(Image.open(self.bg % cut).convert("RGB"), np.uint8)
        return self._bg[cut]

    def zonen(self):
        """RVD (20 B je Zone: u16 Marke, u8 von, u8 nach, 4 x (s16 x, s16 z)); Ende 0xFFFF/FF/FF.
        Port: rdt_common.c parse_zones; erste Zone je 'von' = Bildbereich des Cuts
        (re15_rdt_get_region_quad, FUN_80014324)."""
        d, p, aus = self.d, self.rvd_off, []
        if not p or p >= len(d):
            return aus
        while p + 20 <= len(d):
            m, a, b = struct.unpack_from("<HBB", d, p)
            if m == 0xFFFF and a == 0xFF and b == 0xFF:
                break
            pts = [struct.unpack_from("<hh", d, p + 4 + 4 * i) for i in range(4)]
            aus.append(dict(marke=m, von=a, nach=b, pts=pts))
            p += 20
            if len(aus) > 400:
                break
        return aus

    def bereiche(self):
        """{cut: Viereck} - erste RVD-Zone je 'von'."""
        aus = {}
        for z in self.zonen():
            if z["von"] not in aus:
                aus[z["von"]] = z["pts"]
        return aus

    def gruppen(self):
        """{cut: Gruppennummer} - Zusammenhangskomponenten des Umschaltgraphen (alle RVD-Zonen
        ausser der ersten je 'von', die der Bildbereich ist). Cuts einer Gruppe zeigen
        denselben zusammenhaengenden Bereich des Raums (z.B. ROOM1000: {0,1,2} Ostraum,
        {3,4,5} und {6,7,8} die beiden Toiletten)."""
        eltern = list(range(max(self.ncut, 1)))

        def f(x):
            while eltern[x] != x:
                eltern[x] = eltern[eltern[x]]
                x = eltern[x]
            return x
        erst = set()
        for z in self.zonen():
            a, b = z["von"], z["nach"]
            if a not in erst:
                erst.add(a)
                continue
            if a < len(eltern) and b < len(eltern):
                eltern[f(a)] = f(b)
        return {c: f(c) for c in range(len(eltern))}


def im_viereck(pts, x, z):
    """Punkt in konvexem Viereck (Umlaufsinn egal)."""
    s = []
    for i in range(4):
        ax, az = pts[i]
        bx, bz = pts[(i + 1) % 4]
        s.append((bx - ax) * (z - az) - (bz - az) * (x - ax))
    return all(v >= 0 for v in s) or all(v <= 0 for v in s)


def abstand_viereck(pts, x, z):
    """0 innen, sonst Abstand zur naechsten Kante."""
    if im_viereck(pts, x, z):
        return 0.0
    best = 1e18
    for i in range(4):
        ax, az = pts[i]
        bx, bz = pts[(i + 1) % 4]
        vx, vz = bx - ax, bz - az
        L2 = vx * vx + vz * vz
        t = 0.0 if L2 == 0 else max(0.0, min(1.0, ((x - ax) * vx + (z - az) * vz) / L2))
        best = min(best, math.hypot(ax + t * vx - x, az + t * vz - z))
    return best


# ----------------------------------------------------------------------------
# Raumnamen aus dem Debug-Menue (DEBUG.BIN @0x2642 + 0x4FA*Stage + 0x1A*Index; Name ab +0,
# NUL-terminiert. Beleg: debug_menu_common.c re15_debug_menu_room_name, analysis/
# bug_save_room_name.md M5 "ROOM1070 @0x26f8: LOBBY OFFICE").
# ----------------------------------------------------------------------------
_NAMEN = None


def raumnamen():
    global _NAMEN
    if _NAMEN is None:
        d = open(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "BIN", "DEBUG.BIN"), "rb").read()
        _NAMEN = {}
        for st in range(6):
            for i in range(0x31):
                o = 0x2642 + 0x4FA * st + 0x1A * i
                nm = d[o:o + 0x1A].split(b"\0")[0].decode("latin1").strip()
                if nm:
                    _NAMEN["%X%02X" % (st + 1, i)] = nm
    return _NAMEN
