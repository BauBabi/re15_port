#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""tuerkatalog.py - Katalog der 55 RE2-Tuer-Archive (DO2) und Skript-Entschluesselung.

Zweck: Vorbildsuche fuer das Gelaendertor in RE1.5 ROOM1170 (analysis/tor_1170/04_tuerkatalog.md).

Liest NUR (schreibt ausschliesslich nach build/tor_1170/):
  info/re2leon/PSX.EXE                 RE2-Leon-EXE (Tabellen + Handler-Belege)
  info/re2leon/COMMON/DOOR/DOORxx.DO2  die 55 Archive (roh, NICHT die entpackten Ordner)

Alle Format-/Verhaltensannahmen tragen ihre Herkunft (RE2 Leon PSX.EXE, t_addr 0x80010000):

  Container   Tabelle @0x8009a520, 12 B je Tuer: u16 Groesse Klangteil, u16 Groesse Modellteil,
              u32 Sektoren Klangteil (gelesen in FUN_80014cd0: uVar14 = *(u16*)(0x8009a520+id*0xc)).
              Modellteil liegt in der Datei bei Sektoren*0x800 und wird nach 0x801a1000 geladen:
              +0 u32 MD1-Offset, +4 u32 TIM-Offset (beide relativ zum Modellteil; @0x80013c1c ff.
              DAT_801a1000/DAT_801a1004 += 0x801a1000), +8 SCD-Offsettabelle (u16, relativ zu +8;
              @0x80014060 ori v1,v1,0x1008 -> DAT_800d8cbc).
  MD1         Kopf 12 B, danach Mesh-Tabelle; Mesh-Schritt 56 B (@0x80014b60..68: a1*8-a1 <<3),
              Dreiecksteil @+0, Vierecksteil @+0x1c (@0x80014b74 addiu v1,v1,28).
              Der Tuer-Renderer FUN_8001468c zeichnet NUR Dreiecke (@0x80014710 lw fp,16(a2),
              @0x80014718 lw t0,20(a2), Schleifenende @0x80014b04 bne t0,zero; kein Vierecks-Durchlauf).
  SCD-VM      Dispatch-Tabelle @0x800a74c8 (0x8f Eintraege), Tuer-Laeufer FUN_80014058 faehrt die
              Ereignisplaetze 10..13 (@0x80014068 addiu s2,zero,10; Schritt 0x174).
  Opcodes     Laengen/Felder: siehe OPS unten, jede Zeile mit der Adresse des PC-Vorschubs.
  Kamera      FUN_80013c1c laedt @0x8001082c..: Auge (10000,0,0), Ziel (0,0,0); FUN_80076cb0 baut
              daraus die Blickmatrix (x_cam = z_welt, y_cam = y_welt, z_cam = 10000 - x_welt).
              Projektionsabstand H = 0x122 = 290 (FUN_8008de24 = ctc2 a0,26).
  RotMatrix   @0x8008e1f4: M = Rx*Ry*Rz, m[0][2] = +sin(y) (@0x8008e2c8 sh t6,4(a1)).

Aufruf:
  python re15_port/tools/tor/tuerkatalog.py            # alles: JSON + Bilder
  python re15_port/tools/tor/tuerkatalog.py --dis 10   # Skripte von DOOR10 zeilenweise
  python re15_port/tools/tor/tuerkatalog.py --trace 10 1 [--obj 1]   # Bild-Zeitachse Variante 1
  python re15_port/tools/tor/tuerkatalog.py --mesh 10  # Mesh-Masse
  python re15_port/tools/tor/tuerkatalog.py --rang     # Rangliste der Vorbilder
  python re15_port/tools/tor/tuerkatalog.py --vergleich 0A 2E   # Byte-Vergleich zweier Skriptbloecke
  python re15_port/tools/tor/tuerkatalog.py --platte   # Tueren mit gleicher Mesh-0-Geometrie
  python re15_port/tools/tor/tuerkatalog.py --gelb     # Gelbanteil der Texturen (Warnschraffur)
  python re15_port/tools/tor/tuerkatalog.py --sheets 2E,0A,10   # zusaetzlich Einzelblaetter + Zeitachse im JSON
"""
import argparse
import json
import math
import struct
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
DOOR_DIR = REPO / "info" / "re2leon" / "COMMON" / "DOOR"
EXE_PATH = REPO / "info" / "re2leon" / "PSX.EXE"
OUT_DIR = REPO / "build" / "tor_1170"

EXE_BASE = 0x80010000
EXE_HDR = 0x800
DOOR_TABLE = 0x8009A520      # 12 B je Tuer
OP_TABLE = 0x800A74C8        # SCD-Dispatch
MEMBER_SET_TABLE = 0x80011228  # 44 Eintraege, Member_set (0x34/0x35) -> Feld
MEMBER_GET_TABLE = 0x800112F8  # 44 Eintraege, Member_copy (0x3d)
N_DOORS = 55

CAM_EYE_X = 10000   # @0x80010830: 10 27 00 00
GEOM_H = 290        # @0x80013c1c..: FUN_8008de24(0x122)
SCREEN_CX, SCREEN_CY = 160, 120   # Bildmitte 320x240 (nur fuer die Uebersichtsbilder)

_exe = None


def exe():
    global _exe
    if _exe is None:
        _exe = EXE_PATH.read_bytes()
        t_addr = struct.unpack_from("<I", _exe, 0x18)[0]
        assert t_addr == EXE_BASE, hex(t_addr)
    return _exe


def exe_rd(addr, n):
    o = addr - EXE_BASE + EXE_HDR
    return exe()[o:o + n]


def exe_u32(addr):
    return struct.unpack("<I", exe_rd(addr, 4))[0]


# ----------------------------------------------------------------------------------------
# Opcode-Tabelle. (Name, Laenge, Beleg fuer den PC-Vorschub bzw. Sprung)
# ----------------------------------------------------------------------------------------
OPS = {
    0x00: ("Nop", 1, "@0x800537ec addiu v0,v0,1"),
    0x01: ("Evt_end", 2, "@0x80053808 Ebene 0: sb zero,1(a3) -> Ereignis aus, sonst Ruecksprung @0x80053850"),
    0x02: ("Evt_next", 1, "@0x80053868 addiu v0,v0,1; Rueckgabe 2 @0x80053874"),
    0x03: ("Evt_chain", 4, "@0x80053888 lbu a1,3(v0); jal 0x800530ec (Neustart, kein Vorschub)"),
    0x04: ("Evt_exec", 4, "@0x800538bc addiu v0,v0,4"),
    0x05: ("Evt_kill", 2, "@0x80053914 addiu v0,v0,2"),
    0x06: ("Ifel_ck", 4, "@0x8005392c addiu a1,a2,4"),
    0x07: ("Else_ck", 4, "@0x8005397c addu v1,v1,v0 (pc += u16@2)"),
    0x08: ("Endif", 2, "@0x800539b8 addiu v0,v0,2"),
    0x09: ("Sleep", 1, "@0x800539e4 addiu v0,a2,1; Zaehler = u16@2 @0x80053a10"),
    0x0A: ("Sleeping", 3, "@0x80053a6c addiu v0,v0,3"),
    0x0D: ("For", 6, "@0x80053b58 addiu t0,t0,6"),
    0x0E: ("Next", 2, "@0x80053d1c addiu v0,v0,2"),
    0x0F: ("While", 4, "@0x80053da0 addiu a1,a1,4"),
    0x10: ("Ewhile", 2, "@0x80053e2c lw v0,32(v1) (Sprung zum While)"),
    0x13: ("Switch", 4, "@0x80054040 addiu a3,a3,4"),
    0x14: ("Case", 6, "@0x80054100 addiu v0,v0,6"),
    0x15: ("Default", 2, "@0x80054118 addiu v0,v0,2"),
    0x16: ("Eswitch", 2, "@0x8005414c addiu v0,v0,2"),
    0x17: ("Goto", 6, "@0x80054190 addu a1,a1,t0 (pc += s16@4)"),
    0x18: ("Gosub", 2, "@0x800541b4 addiu v1,v1,2"),
    0x1A: ("Break", 2, "@0x80054290 lw v1,96(v1) (Sprung ans Blockende)"),
    0x1D: ("Work_copy", 4, "@0x800542c8 addiu a1,a1,4"),
    0x23: ("Cmp", 6, "@0x80054484 addiu v0,v0,6"),
    0x24: ("Save", 4, "@0x8005452c addiu v0,v0,4"),
    0x25: ("Copy", 3, "@0x8005455c addiu v0,v0,3"),
    0x26: ("Calc", 6, "@0x800545a4 addiu v0,v0,6"),
    0x2E: ("Work_set", 3, "@0x80055928 addiu v0,v0,3"),
    0x2F: ("Speed_set", 4, "@0x80055a94 addiu v0,v0,4"),
    0x30: ("Add_speed", 1, "@0x80055b1c addiu v0,v0,1"),
    0x31: ("Add_aspeed", 1, "@0x80055b8c addiu v1,v1,1"),
    0x34: ("Member_set", 4, "@0x80055c30 addiu v0,v0,4"),
    0x35: ("Member_set2", 3, "@0x80055c90 addiu v0,v0,3"),
    0x36: ("Se_on", 12, "@0x8005653c addiu v1,s0,12"),
    0x3D: ("Member_copy", 3, "@0x80055e54 addiu v0,v0,3"),
    0x4D: ("Door_model_set", 22, "@0x80014cac addiu v0,a1,22"),
    0x53: ("Sce_fade_set", 6, "@0x80057fa8 addiu v0,s2,6"),
    0x74: ("Sce_fade_adjust", 4, "@0x80058004 addiu s0,s0,4"),
    0x8A: ("Op8A", 6, "@0x80059378 addiu v1,v1,6"),
    0x8B: ("Op8B", 6, "@0x800593c8 addiu v1,v1,6"),
    0x8C: ("Op8C", 8, "@0x8005941c addiu v1,v1,8"),
}

CMP_OPS = {0: "==", 1: ">", 2: ">=", 3: "<", 4: "<=", 5: "!=", 6: "&"}   # Sprungtabelle @0x800111a0
CALC_OPS = {0: "+", 1: "-", 2: "*", 3: "/", 4: "%", 5: "|", 6: "&", 7: "^", 8: "~", 9: "<<", 10: ">>u", 11: ">>s"}  # @0x800111c0

VAR_NAMES = {
    0x0C: "var0C(Tuer-Variante = door_type & 0x7f, @FUN_80013c1c DAT_800d4804)",
    0x0D: "var0D(1 solange Klang laedt, @FUN_80013eb4 DAT_800d4806)",
    0x0E: "var0E(door_type & 0x80, DAT_800d4808)",
    0x0F: "var0F(Tuernummer, DAT_800d480a)",
}


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s8(v):
    v &= 0xFF
    return v - 0x100 if v & 0x80 else v


# ----------------------------------------------------------------------------------------
# Member-Tabellen aus der EXE lesen (Sprungziel -> Speicherbefehl)
# ----------------------------------------------------------------------------------------
def _decode_store(word):
    op = word >> 26
    off = s16(word & 0xFFFF)
    width = {0x28: 1, 0x29: 2, 0x2B: 4, 0x20: 1, 0x24: 1, 0x21: 2, 0x25: 2, 0x23: 4}.get(op)
    signed = op in (0x20, 0x21)
    return width, off, signed


def member_table(base, first_is_target):
    """Liefert {member: (breite, offset)}; first_is_target: Befehl steht AM Sprungziel (Getter)
    statt im Delay-Slot des folgenden j (Setter)."""
    out = {}
    for i in range(44):
        tgt = exe_u32(base + i * 4)
        w0 = exe_u32(tgt)
        w1 = exe_u32(tgt + 4)
        cand = [w0, w1] if first_is_target else [w1, w0]
        for w in cand:
            width, off, _ = _decode_store(w)
            if width:
                out[i] = (width, off, tgt)
                break
    return out


# ----------------------------------------------------------------------------------------
# Container / MD1 / TIM
# ----------------------------------------------------------------------------------------
class Door:
    def __init__(self, idx):
        self.idx = idx
        self.name = "DOOR%02X" % idx
        self.path = DOOR_DIR / (self.name + ".DO2")
        self.data = self.path.read_bytes()
        ent = exe_rd(DOOR_TABLE + idx * 12, 12)
        self.snd_size, self.mdl_size, self.snd_sectors = struct.unpack_from("<HHI", ent, 0)
        self.mo = self.snd_sectors * 0x800
        d = self.data
        self.md1_rel, self.tim_rel = struct.unpack_from("<II", d, self.mo)
        self.scd_tab = self.mo + 8
        first = struct.unpack_from("<H", d, self.scd_tab)[0]
        self.n_scripts = first // 2
        self.scd_offs = list(struct.unpack_from("<%dH" % self.n_scripts, d, self.scd_tab))
        self.md1_off = self.mo + self.md1_rel
        self.tim_off = self.mo + self.tim_rel
        self.scd_end_rel = self.md1_rel - 8       # relativ zur Offsettabelle
        self.valid = (self.mo + self.mdl_size == len(d)
                      and struct.unpack_from("<I", d, self.tim_off)[0] == 0x10
                      and (self.snd_size + 0x7FF) & ~0x7FF == self.mo)
        self._parse_md1()
        self._parse_tim()

    # --- SCD ---
    def scd_block(self):
        """Skriptblock ab der Offsettabelle (veraenderbar: Work_copy schreibt hinein)."""
        return bytearray(self.data[self.scd_tab:self.scd_tab + self.scd_end_rel])

    def script_range(self, k):
        a = self.scd_offs[k]
        b = self.scd_offs[k + 1] if k + 1 < self.n_scripts else self.scd_end_rel
        return a, b

    # --- MD1 ---
    def _parse_md1(self):
        d = self.data
        o = self.md1_off
        self.md1_len, self.md1_unk, self.md1_objs = struct.unpack_from("<III", d, o)
        tab = o + 12
        self.meshes = []
        for m in range(self.md1_objs // 2):
            e = tab + m * 56
            vo, vc, no, nc, to, tc, tt = struct.unpack_from("<7I", d, e)
            qvo, qvc, qno, qnc, qo, qc, qt = struct.unpack_from("<7I", d, e + 28)
            verts = [struct.unpack_from("<hhh", d, tab + vo + i * 8) for i in range(vc)]
            tris = []
            for i in range(tc):
                n0, v0, n1, v1, n2, v2 = struct.unpack_from("<6H", d, tab + to + i * 12)
                u0, w0, clut, u1, w1, page, u2, w2, _ = struct.unpack_from("<BBHBBHBBH", d, tab + tt + i * 12)
                tris.append({"v": (v0, v1, v2), "n": (n0, n1, n2),
                             "uv": ((u0, w0), (u1, w1), (u2, w2)), "clut": clut, "page": page})
            used = sorted({i for t in tris for i in t["v"]})
            pts = [verts[i] for i in used if i < len(verts)] or verts
            if pts:
                mn = [min(p[a] for p in pts) for a in range(3)]
                mx = [max(p[a] for p in pts) for a in range(3)]
            else:
                mn = mx = [0, 0, 0]
            uvs = [uv for t in tris for uv in t["uv"]]
            self.meshes.append({
                "uv_min": [min(u[0] for u in uvs), min(u[1] for u in uvs)] if uvs else [0, 0],
                "uv_max": [max(u[0] for u in uvs), max(u[1] for u in uvs)] if uvs else [0, 0],
                "cluts": sorted({t["clut"] for t in tris}), "pages": sorted({t["page"] for t in tris}),
                "index": m, "entry_file_off": e, "verts": verts, "tris": tris,
                "n_verts": vc, "n_normals": nc, "n_tris": tc, "n_quads": qc,
                "bbox_min": mn, "bbox_max": mx,
                "size": [mx[a] - mn[a] for a in range(3)],
            })

    # --- TIM ---
    def _parse_tim(self):
        d = self.data
        o = self.tim_off
        magic, flags = struct.unpack_from("<II", d, o)
        self.tim_flags = flags
        p = o + 8
        self.cluts = []
        if flags & 8:
            bl, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, p)
            raw = struct.unpack_from("<%dH" % (cw * ch), d, p + 12)
            for r in range(ch):
                self.cluts.append(raw[r * cw:(r + 1) * cw])
            self.clut_rect = (cx, cy, cw, ch)
            p += bl
        bl, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, p)
        self.tim_img_rect = (ix, iy, iw, ih)
        bpp = {0: 4, 1: 8, 2: 16, 3: 24}[flags & 3]
        self.tim_bpp = bpp
        self.tex_w = iw * 16 // bpp if bpp <= 16 else iw * 2 // 3
        self.tex_h = ih
        self.tim_pix_off = p + 12
        self.tim_len = (p + bl) - o

    def texture(self, clut=0):
        from PIL import Image
        d = self.data
        w, h = self.tex_w, self.tex_h
        img = Image.new("RGB", (w, h))
        px = img.load()
        pal = self.cluts[clut] if self.cluts else None

        def rgb(c):
            return ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)
        p = self.tim_pix_off
        if self.tim_bpp == 8:
            for y in range(h):
                row = d[p + y * w:p + (y + 1) * w]
                for x in range(w):
                    px[x, y] = rgb(pal[row[x]])
        elif self.tim_bpp == 4:
            for y in range(h):
                for x in range(w):
                    b = d[p + y * (w // 2) + x // 2]
                    i = (b >> 4) if (x & 1) else (b & 15)
                    px[x, y] = rgb(pal[i])
        else:
            for y in range(h):
                for x in range(w):
                    px[x, y] = rgb(struct.unpack_from("<H", d, p + (y * w + x) * 2)[0])
        return img


def mesh_components(m):
    """Zusammenhangskomponenten eines Meshes (gemeinsame Vertex-Indizes ODER gleiche Position)."""
    par = list(range(len(m["verts"])))

    def f(a):
        while par[a] != a:
            par[a] = par[par[a]]
            a = par[a]
        return a
    pos = {}
    for i, v in enumerate(m["verts"]):
        if v in pos:
            par[f(i)] = f(pos[v])
        else:
            pos[v] = i
    for tr in m["tris"]:
        a, b, c = tr["v"]
        if max(a, b, c) >= len(par):
            continue
        par[f(a)] = f(b)
        par[f(b)] = f(c)
    g = {}
    for ti, tr in enumerate(m["tris"]):
        if max(tr["v"]) < len(par):
            g.setdefault(f(tr["v"][0]), []).append(ti)
    out = []
    for tl in g.values():
        vs = {i for ti in tl for i in m["tris"][ti]["v"]}
        pts = [m["verts"][i] for i in vs]
        mn = [min(p[a] for p in pts) for a in range(3)]
        mx = [max(p[a] for p in pts) for a in range(3)]
        size = [mx[a] - mn[a] for a in range(3)]
        srt = sorted(size)
        # Stab: zwei kleine Masse, ein langes (>= 5x), Mantel ohne Deckel = 2 Dreiecke je Seite
        rod = srt[1] > 0 and srt[2] >= 5 * srt[1] and len(tl) in (6, 8, 10, 12, 16) and len(vs) == len(tl)
        out.append({"tris": len(tl), "verts": len(vs), "min": mn, "max": mx, "size": size,
                    "stab": bool(rod), "seiten": len(tl) // 2 if rod else 0})
    out.sort(key=lambda c: (-c["tris"], c["min"][2], c["min"][1]))
    return out


def mesh_front_stats(m, unit=20):
    """Frontansicht orthografisch entlang x (z waagrecht, y senkrecht): Deckungsgrad der
    Dreiecke im Huellrechteck und Eckenzahl der konvexen Huelle (4 = Rechteck, 8 = gefast)."""
    from PIL import Image, ImageDraw
    mn, mx = m["bbox_min"], m["bbox_max"]
    w = max(1, (mx[2] - mn[2]) // unit + 1)
    h = max(1, (mx[1] - mn[1]) // unit + 1)
    img = Image.new("L", (w, h), 0)
    dr = ImageDraw.Draw(img)
    pts2 = set()
    for tr in m["tris"]:
        if max(tr["v"]) >= len(m["verts"]):
            continue
        ps = [((m["verts"][i][2] - mn[2]) / unit, (m["verts"][i][1] - mn[1]) / unit) for i in tr["v"]]
        dr.polygon(ps, fill=255)
        for i in tr["v"]:
            pts2.add((m["verts"][i][2], m["verts"][i][1]))
    hist = img.histogram()
    cover = hist[255] / float(w * h)
    P = sorted(pts2)

    def cross(o, a, b):
        return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0])
    hull = []
    if len(P) >= 3:
        lo = []
        for p in P:
            while len(lo) >= 2 and cross(lo[-2], lo[-1], p) <= 0:
                lo.pop()
            lo.append(p)
        up = []
        for p in reversed(P):
            while len(up) >= 2 and cross(up[-2], up[-1], p) <= 0:
                up.pop()
            up.append(p)
        hull = lo[:-1] + up[:-1]
    return {"deckung": round(cover, 4), "huelle_ecken": len(hull), "huelle": hull, "raster_einheit": unit}


# ----------------------------------------------------------------------------------------
# Disassembler
# ----------------------------------------------------------------------------------------
def decode_op(blk, pc):
    """Eine Anweisung -> dict(op, name, len, raw, text, fields)."""
    op = blk[pc]
    if op not in OPS:
        return {"off": pc, "op": op, "name": "??", "len": 1, "raw": bytes(blk[pc:pc + 1]).hex(" "),
                "text": "UNBEKANNT 0x%02x" % op, "f": {}}
    name, ln, _ = OPS[op]
    b = blk[pc:pc + ln]
    f = {}

    def u16(o):
        return struct.unpack_from("<H", b, o)[0]

    def i16(o):
        return struct.unpack_from("<h", b, o)[0]
    t = name
    if op == 0x4D:
        f = {"id": b[1], "b2": b[2], "frame": b[3], "on": b[4], "mesh": b[5], "flags": u16(6),
             "w8": i16(8), "x": i16(10), "y": i16(12), "z": i16(14),
             "rx": u16(16), "ry": u16(18), "rz": u16(20)}
        par = ("Eltern=Obj%d" % (f["flags"] & 0xF)) if f["flags"] & 0x10 else "Eltern=Kamera"
        t = ("Door_model_set obj=%d mesh=%d an=%d flags=0x%04x(%s) b2=%d bild=%d w8=%d pos=(%d,%d,%d) rot=(%d,%d,%d)"
             % (f["id"], f["mesh"], f["on"], f["flags"], par, f["b2"], f["frame"], f["w8"],
                f["x"], f["y"], f["z"], f["rx"], f["ry"], f["rz"]))
    elif op == 0x2E:
        f = {"type": b[1], "id": s8(b[2])}
        t = "Work_set typ=%d id=%d" % (f["type"], f["id"])
    elif op == 0x2F:
        f = {"idx": b[1], "val": i16(2)}
        nm = ["vx", "vy", "vz", "wx", "wy", "wz", "ax", "ay", "az", "awx", "awy", "awz"]
        t = "Speed_set [%d=%s] = %d" % (f["idx"], nm[f["idx"]] if f["idx"] < 12 else "?", f["val"])
    elif op == 0x0D:
        f = {"blk": i16(2), "n": u16(4)}
        t = "For n=%d (Block %d B)" % (f["n"], f["blk"])
    elif op == 0x09:
        f = {"n": struct.unpack_from("<H", blk, pc + 2)[0]}
        t = "Sleep %d" % f["n"]
    elif op == 0x0A:
        f = {"n": u16(1)}
        t = "Sleeping (Zaehler %d)" % f["n"]
    elif op == 0x04:
        f = {"slot": b[1], "b2": b[2], "script": b[3]}
        t = "Evt_exec platz=%d skript=%d" % (f["slot"], f["script"])
    elif op == 0x03:
        f = {"script": b[3]}
        t = "Evt_chain skript=%d" % f["script"]
    elif op == 0x05:
        f = {"slot": b[1]}
        t = "Evt_kill platz=%d" % f["slot"]
    elif op == 0x18:
        f = {"script": b[1]}
        t = "Gosub %d" % f["script"]
    elif op == 0x13:
        f = {"var": b[1], "blk": u16(2)}
        t = "Switch var0x%02x (Block %d B)" % (f["var"], f["blk"])
    elif op == 0x14:
        f = {"blk": u16(2), "val": i16(4)}
        t = "Case %d (Block %d B)" % (f["val"], f["blk"])
    elif op == 0x17:
        f = {"ifn": s8(b[1]), "lvl": s8(b[2]), "rel": i16(4)}
        t = "Goto %+d -> 0x%x" % (f["rel"], pc + f["rel"])
    elif op == 0x06:
        f = {"blk": u16(2)}
        t = "Ifel_ck (sonst -> 0x%x)" % (pc + 4 + f["blk"])
    elif op == 0x07:
        f = {"blk": u16(2)}
        t = "Else_ck (-> 0x%x)" % (pc + f["blk"])
    elif op == 0x0F:
        f = {"cond": b[1], "blk": i16(2)}
        t = "While (Bedingung %d B, Ende -> 0x%x)" % (f["cond"], pc + 4 + f["blk"])
    elif op == 0x23:
        f = {"var": b[2], "cmp": b[3], "val": i16(4)}
        t = "Cmp var0x%02x %s %d" % (f["var"], CMP_OPS.get(f["cmp"], "?"), f["val"])
    elif op == 0x24:
        f = {"var": b[1], "val": i16(2)}
        t = "Save var0x%02x = %d" % (f["var"], f["val"])
    elif op == 0x25:
        f = {"dst": b[1], "src": b[2]}
        t = "Copy var0x%02x = var0x%02x" % (f["dst"], f["src"])
    elif op == 0x26:
        f = {"calc": b[2], "var": b[3], "val": i16(4)}
        t = "Calc var0x%02x %s= %d" % (f["var"], CALC_OPS.get(f["calc"], "?"), f["val"])
    elif op == 0x1D:
        f = {"var": b[1], "dst": b[2], "word": b[3]}
        t = "Work_copy var0x%02x -> Skriptbyte 0x%x (%s)" % (f["var"], pc + 4 + f["dst"], "16 Bit" if f["word"] else "8 Bit")
    elif op == 0x34:
        f = {"member": b[1], "val": i16(2)}
        t = "Member_set [%d] = %d" % (f["member"], f["val"])
    elif op == 0x35:
        f = {"member": b[1], "var": b[2]}
        t = "Member_set2 [%d] = var0x%02x" % (f["member"], f["var"])
    elif op == 0x3D:
        f = {"var": s8(b[1]), "member": s8(b[2])}
        t = "Member_copy var0x%02x = [%d]" % (f["var"], f["member"])
    elif op == 0x36:
        f = {"vab": b[1], "se": i16(2), "work": i16(4), "x": i16(6), "y": i16(8), "z": i16(10)}
        t = "Se_on vab=%d se=0x%04x work=0x%04x pos=(%d,%d,%d)" % (f["vab"], f["se"] & 0xFFFF, f["work"] & 0xFFFF, f["x"], f["y"], f["z"])
    elif op == 0x53:
        f = {"b1": b[1], "b2": b[2], "b3": b[3], "w4": i16(4)}
        t = "Sce_fade_set %d,%d,%d,%d" % (f["b1"], f["b2"], f["b3"], f["w4"])
    elif op == 0x74:
        f = {"b1": b[1], "w2": i16(2)}
        t = "Sce_fade_adjust %d,%d" % (f["b1"], f["w2"])
    elif ln > 1:
        t = "%s %s" % (name, bytes(b[1:]).hex(" "))
    return {"off": pc, "op": op, "name": name, "len": ln, "raw": bytes(b).hex(" "), "text": t, "f": f}


def disasm_script(door, k, blk=None):
    blk = blk if blk is not None else door.scd_block()
    a, e = door.script_range(k)
    out = []
    pc = a
    while pc < e:
        ins = decode_op(blk, pc)
        out.append(ins)
        if ins["name"] == "??":
            break
        pc += ins["len"]
    return out, (pc == e)


# ----------------------------------------------------------------------------------------
# VM
# ----------------------------------------------------------------------------------------
class Obj:
    __slots__ = ("on", "b8", "frame", "mesh", "flags", "w16", "pos", "rot", "parent", "mem", "set_tick")

    def __init__(self):
        self.on = 0
        self.b8 = 0
        self.frame = 0
        self.mesh = 0
        self.flags = 0
        self.w16 = 0
        self.pos = [0, 0, 0]
        self.rot = [0, 0, 0]
        self.parent = -1
        self.mem = {}
        self.set_tick = -1


class Event:
    def __init__(self):
        self.active = False
        self.pc = 0
        self.sub = 0
        self.ifn = [-1] * 4
        self.lvl = [-1] * 4
        self.ifstk = [[0] * 8 for _ in range(4)]
        self.ifsp = 0            # Index in ifstk[sub] (Zeiger event+320)
        self.ifsp_sub = 0
        self.cnt = [[0] * 4 for _ in range(4)]
        self.lstart = [[0] * 4 for _ in range(4)]
        self.lend = [[0] * 4 for _ in range(4)]
        self.lifn = [[0] * 4 for _ in range(4)]
        self.ret = [0] * 4
        self.work = None         # (typ, id)
        self.speed = [0] * 12


class VM:
    def __init__(self, door, variant=0, var0e=0, sound_ready_tick=0, max_ticks=3000):
        self.door = door
        self.blk = door.scd_block()
        self.vars = [0] * 256
        self.vars[0x0C] = variant
        self.vars[0x0D] = 1
        self.vars[0x0E] = var0e
        self.vars[0x0F] = door.idx
        self.sound_ready_tick = sound_ready_tick
        self.objs = [Obj() for _ in range(10)]
        self.ev = {i: Event() for i in range(10, 14)}
        self.tick = 0
        self.frame_counter = 0
        self.max_ticks = max_ticks
        self.log = []            # (tick, slot, off, text)
        self.frames = []         # je Bild: Liste der Objektzustaende
        self.sounds = []
        self.fades = []
        self.notes = set()
        self.vars_read = set()
        self.global248 = 0
        self.mset = member_table(MEMBER_SET_TABLE, False)
        self.mget = member_table(MEMBER_GET_TABLE, True)
        self.start_event(self.ev[10], 0)

    # FUN_800530ec
    def start_event(self, e, script):
        e.pc = self.door.scd_offs[script]
        e.ifsp_sub = e.sub
        e.ifsp = 0
        e.active = True
        e.ifn[0] = -1
        e.lvl[0] = -1

    def rdvar(self, i):
        self.vars_read.add(i)
        return s16(self.vars[i])

    # --- Feldzugriff Tuerobjekt ---
    def obj_store(self, o, width, off, val):
        if off == 56:
            o.pos[0] = val
        elif off == 60:
            o.pos[1] = val
        elif off == 64:
            o.pos[2] = val
        elif off == 116:
            o.rot[0] = val & 0xFFFF
        elif off == 118:
            o.rot[1] = val & 0xFFFF
        elif off == 120:
            o.rot[2] = val & 0xFFFF
        elif off == 324:
            if width == 1:
                o.flags = (o.flags & 0xFF00) | (val & 0xFF)
            else:
                o.flags = val & 0xFFFF
            self._reparent(o)
        elif off == 326:
            o.mesh = val & 0xFFFF
        elif off == 270:
            o.frame = val & 0xFFFF
        elif off == 0:
            o.on = val & 0xFFFF
        elif off == 8:
            o.b8 = val & 0xFF
        elif off == 16:
            o.w16 = val
        else:
            o.mem[off] = val

    def obj_load(self, o, width, off):
        m = {56: o.pos[0], 60: o.pos[1], 64: o.pos[2], 116: o.rot[0], 118: o.rot[1], 120: o.rot[2],
             324: o.flags, 326: o.mesh, 270: o.frame, 0: o.on, 8: o.b8, 16: o.w16}
        if off in m:
            return m[off]
        return o.mem.get(off, 0)

    def _reparent(self, o):
        # Elternzeiger wird NUR in Door_model_set gesetzt (@0x80014c64..8c); Member_set auf die
        # Flags aendert ihn nicht. Deshalb hier nichts tun.
        pass

    def cur_obj(self, e):
        if e.work is None:
            self.notes.add("Zugriff ohne Work_set")
            return None
        typ, idx = e.work
        if typ != 5:
            self.notes.add("Work_set typ=%d (kein Tuerobjekt)" % typ)
            return None
        if not 0 <= idx < 10:
            self.notes.add("Work_set id=%d ausserhalb 0..9" % idx)
            return None
        return self.objs[idx]

    # --- eine Anweisung; Rueckgabe wie die Handler: 1 weiter, 2 Bildende, 0 Bedingung falsch ---
    def step(self, slot, e):
        blk = self.blk
        pc = e.pc
        ins = decode_op(blk, pc)
        op = ins["op"]
        f = ins["f"]
        self.log.append((self.tick, slot, pc, ins["text"], ins["raw"]))
        if ins["name"] == "??":
            self.notes.add("unbekannter Opcode 0x%02x @0x%x" % (op, pc))
            e.active = False
            return 2
        s = e.sub
        if op in (0x00,):
            e.pc += 1
            return 1
        if op == 0x01:
            if e.sub == 0:
                e.active = False
                return 2
            e.sub -= 1
            e.pc = e.ret[e.sub]
            e.ifsp_sub = e.sub
            e.ifsp = e.ifn[e.sub] + 1
            return 1
        if op == 0x02:
            e.pc += 1
            return 2
        if op == 0x03:
            self.start_event(e, f["script"])
            return 1
        if op == 0x04:
            e.pc += 4
            slot_id = f["slot"]
            if slot_id >= 14:           # @0x800531ac sltiu v0,a0,0xe -> freien Platz 10..13 suchen
                slot_id = None
                for c in range(10, 14):
                    if not self.ev[c].active:
                        slot_id = c
                        break
                if slot_id is None:
                    self.notes.add("Evt_exec: kein freier Platz")
                    return 1
            if slot_id < 10:
                self.notes.add("Evt_exec auf Platz %d (<10)" % slot_id)
                return 1
            n = self.ev[slot_id]
            n.sub = 0
            n.speed = [0] * 12      # @0x80053214..28: sechs Worte ab +344 geloescht
            self.start_event(n, f["script"])
            return 1
        if op == 0x05:
            if f["slot"] in self.ev:
                self.ev[f["slot"]].active = False
            e.pc += 2
            return 1
        if op == 0x06:
            e.pc += 4
            e.ifn[s] += 1
            e.ifstk[e.ifsp_sub][e.ifsp] = e.pc + f["blk"]
            e.ifsp += 1
            return 1
        if op == 0x07:
            e.ifsp -= 1
            e.pc = pc + f["blk"]
            e.ifn[s] -= 1
            return 1
        if op == 0x08:
            e.ifsp -= 1
            e.pc += 2
            e.ifn[s] -= 1
            return 1
        if op == 0x09:
            e.pc += 1
            e.lvl[s] += 1
            e.cnt[s][e.lvl[s]] = f["n"]
            return 1
        if op == 0x0A:
            l = e.lvl[s]
            e.cnt[s][l] = (e.cnt[s][l] - 1) & 0xFFFF
            if e.cnt[s][l] == 0:
                e.pc += 3
                e.lvl[s] -= 1
            return 2
        if op == 0x0D:
            if f["n"] == 0:
                e.pc = pc + f["blk"] + 6
                return 1
            e.lvl[s] += 1
            l = e.lvl[s]
            e.cnt[s][l] = f["n"]
            e.lstart[s][l] = pc + 6
            e.lend[s][l] = pc + 6 + f["blk"]
            e.lifn[s][l] = e.ifn[s]
            e.pc = pc + 6
            return 1
        if op == 0x0E:
            l = e.lvl[s]
            e.cnt[s][l] = (e.cnt[s][l] - 1) & 0xFFFF
            if e.cnt[s][l] != 0:
                e.pc = e.lstart[s][l]
            else:
                e.pc += 2
                e.lvl[s] -= 1
            return 1
        if op == 0x0F:
            e.lvl[s] += 1
            l = e.lvl[s]
            e.lstart[s][l] = pc
            e.lend[s][l] = pc + 4 + f["blk"]
            e.lifn[s][l] = e.ifn[s]
            e.pc = pc + 4
            if not self.eval_cond(slot, e, f["cond"]):
                e.pc = e.lend[s][l]
                e.lvl[s] -= 1
            return 1
        if op == 0x10:
            l = e.lvl[s]
            e.pc = e.lstart[s][l]
            e.lvl[s] -= 1
            return 1
        if op == 0x13:
            e.lvl[s] += 1
            l = e.lvl[s]
            a3 = pc + 4
            e.lend[s][l] = a3 + f["blk"]
            e.lifn[s][l] = e.ifn[s]
            v = self.rdvar(f["var"])
            while True:
                o2 = blk[a3]
                if o2 == 0x15:
                    a3 += 2
                    break
                if o2 == 0x16:
                    e.lvl[s] -= 1
                    a3 += 2
                    break
                cb = struct.unpack_from("<H", blk, a3 + 2)[0]
                cv = struct.unpack_from("<h", blk, a3 + 4)[0]
                a3 += 6
                if cv == v:
                    break
                a3 += cb
            e.pc = a3
            return 1
        if op == 0x14:
            e.pc += 6
            return 1
        if op in (0x15,):
            e.pc += 2
            return 1
        if op == 0x16:
            e.lvl[s] -= 1
            e.pc += 2
            return 1
        if op == 0x17:
            e.ifn[s] = f["ifn"]
            e.lvl[s] = f["lvl"]
            e.ifsp_sub = s
            e.ifsp = f["ifn"] + 1
            e.pc = pc + f["rel"]
            return 1
        if op == 0x18:
            e.ret[s] = pc + 2
            e.sub += 1
            e.ifn[e.sub] = -1
            e.lvl[e.sub] = -1
            e.ifsp_sub = e.sub
            e.ifsp = 0
            e.pc = self.door.scd_offs[f["script"]]
            return 1
        if op == 0x1A:
            l = e.lvl[s]
            e.pc = e.lend[s][l]
            e.ifn[s] = e.lifn[s][l]
            e.lvl[s] -= 1
            return 1
        if op == 0x1D:
            e.pc += 4
            v = self.vars[f["var"]] & 0xFFFF
            self.vars_read.add(f["var"])
            dst = e.pc + f["dst"]
            if f["word"]:
                struct.pack_into("<H", blk, dst, v)
            else:
                blk[dst] = v & 0xFF
            return 1
        if op == 0x23:
            e.pc += 6
            a = self.rdvar(f["var"])
            b = f["val"]
            c = f["cmp"]
            r = {0: a == b, 1: a > b, 2: a >= b, 3: a < b, 4: a <= b, 5: a != b, 6: (a & b) != 0}.get(c)
            if r is None:          # @0x80054498 sltiu v1,a2,7: sonst Rueckgabe = altes v0 (= pc)
                r = True
                self.notes.add("Cmp mit Operator %d" % c)
            return 1 if r else 0
        if op == 0x24:
            e.pc += 4
            self.vars[f["var"]] = f["val"] & 0xFFFF
            return 1
        if op == 0x25:
            e.pc += 3
            self.vars[f["dst"]] = self.vars[f["src"]]
            return 1
        if op == 0x26:
            e.pc += 6
            a = s16(self.vars[f["var"]])
            b = f["val"]
            c = f["calc"]
            ua = a & 0xFFFF
            if c == 0:
                r = ua + b
            elif c == 1:
                r = ua - b
            elif c == 2:
                r = a * b
            elif c == 3:
                r = int(a / b) if b else 0
            elif c == 4:
                r = int(math.fmod(a, b)) if b else 0
            elif c == 5:
                r = ua | b
            elif c == 6:
                r = ua & b
            elif c == 7:
                r = ua ^ b
            elif c == 8:
                r = ~ua
            elif c == 9:
                r = ua << (b & 31)
            elif c == 10:
                r = ua >> (b & 31)
            else:
                r = a >> (b & 31)
            self.vars[f["var"]] = r & 0xFFFF
            return 1
        if op == 0x2E:
            e.pc += 3
            e.speed = [0] * 12
            e.work = (f["type"], f["id"])
            return 1
        if op == 0x2F:
            e.pc += 4
            if f["idx"] < 12:
                e.speed[f["idx"]] = f["val"]
            else:
                self.notes.add("Speed_set Index %d" % f["idx"])
            return 1
        if op == 0x30:
            e.pc += 1
            o = self.cur_obj(e)
            if o is not None:
                for a in range(3):
                    o.pos[a] += s16(e.speed[a])
                    o.rot[a] = (o.rot[a] + e.speed[3 + a]) & 0xFFFF
            return 1
        if op == 0x31:
            e.pc += 1
            for a in range(6):
                e.speed[a] = s16(e.speed[a] + e.speed[6 + a])
            return 1
        if op in (0x34, 0x35):
            val = f["val"] if op == 0x34 else self.rdvar(f["var"])
            e.pc += 4 if op == 0x34 else 3
            o = self.cur_obj(e)
            ent = self.mset.get(f["member"])
            if o is not None and ent:
                self.obj_store(o, ent[0], ent[1], val)
            return 1
        if op == 0x3D:
            e.pc += 3
            o = self.cur_obj(e)
            ent = self.mget.get(f["member"])
            if o is not None and ent:
                self.vars[f["var"] & 0xFF] = self.obj_load(o, ent[0], ent[1]) & 0xFFFF
            return 1
        if op == 0x36:
            e.pc += 12
            self.sounds.append({"tick": self.tick, "slot": slot, "off": pc, **f})
            return 1
        if op == 0x4D:
            e.pc += 22
            if f["id"] < 10:
                o = self.objs[f["id"]]
                o.b8 = f["b2"]
                o.frame = f["frame"]
                o.on = f["on"]
                o.mesh = f["mesh"]
                o.flags = f["flags"]
                o.w16 = f["w8"]
                o.pos = [f["x"], f["y"], f["z"]]
                o.rot = [f["rx"], f["ry"], f["rz"]]
                o.parent = (f["flags"] & 0xF) if (f["flags"] & 0x10) else -1
                o.set_tick = self.tick
                if f["flags"] & 0x800:
                    self.global248 = 1
            else:
                self.notes.add("Door_model_set id=%d" % f["id"])
            return 1
        if op == 0x53:
            e.pc += 6
            self.fades.append({"tick": self.tick, "op": "set", **f})
            return 1
        if op == 0x74:
            e.pc += 4
            self.fades.append({"tick": self.tick, "op": "adjust", **f})
            return 1
        # uebrige: nur Vorschub
        e.pc += ins["len"]
        return 1

    # FUN_80053f50
    def eval_cond(self, slot, e, nbytes):
        end = e.pc + nbytes
        r = self.step(slot, e)
        guard = 0
        while e.pc != end and guard < 32:
            guard += 1
            conn = struct.unpack_from("<H", self.blk, e.pc)[0]
            e.pc += 2
            v = self.step(slot, e)
            r = (r | v) if conn else (r & v)
        return r

    def snapshot(self):
        st = []
        for i, o in enumerate(self.objs):
            if o.on:
                st.append({"obj": i, "mesh": o.mesh, "flags": o.flags, "parent": o.parent,
                           "pos": list(o.pos), "rot": [s16(r) for r in o.rot]})
        return st

    def run(self):
        while self.ev[10].active and self.tick < self.max_ticks:
            if self.tick >= self.sound_ready_tick:
                self.vars[0x0D] = 0
            for slot in range(10, 14):
                e = self.ev[slot]
                if not e.active:
                    continue
                guard = 0
                while True:
                    guard += 1
                    if guard > 20000:
                        self.notes.add("Endlosschleife Platz %d @0x%x" % (slot, e.pc))
                        e.active = False
                        break
                    r = self.step(slot, e)
                    if r == 1:
                        continue
                    if r == 2:
                        break
                    if e.ifn[e.sub] < 0:
                        break
                    e.ifsp -= 1
                    e.pc = e.ifstk[e.ifsp_sub][e.ifsp]
                    e.ifn[e.sub] -= 1
            # Render (FUN_80014234): Flag 0x400 schaltet bei Bildzaehler == frame das OT-Bit 0x80
            for o in self.objs:
                if o.on and (o.flags & 0x400) and self.frame_counter == o.frame:
                    if (o.flags & 0xC0) == 0xC0:
                        o.flags &= 0xFF7F
                    else:
                        o.flags |= 0x80
            self.frames.append(self.snapshot())
            self.frame_counter += 1
            self.tick += 1
        if self.tick >= self.max_ticks:
            self.notes.add("Abbruch nach %d Bildern" % self.max_ticks)
        return self


# ----------------------------------------------------------------------------------------
# Geometrie
# ----------------------------------------------------------------------------------------
def rot_matrix(rx, ry, rz):
    """PsyQ RotMatrix @0x8008e1f4: M = Rx*Ry*Rz (Winkel 4096 = 360 Grad)."""
    ax, ay, az = (a * 2 * math.pi / 4096.0 for a in (rx, ry, rz))
    sx, cx = math.sin(ax), math.cos(ax)
    sy, cy = math.sin(ay), math.cos(ay)
    sz, cz = math.sin(az), math.cos(az)
    return [[cy * cz, -cy * sz, sy],
            [sx * sy * cz + cx * sz, cx * cz - sx * sy * sz, -sx * cy],
            [sx * sz - cx * sy * cz, cx * sy * sz + sx * cz, cx * cy]]


def mat_mul(a, b):
    return [[sum(a[i][k] * b[k][j] for k in range(3)) for j in range(3)] for i in range(3)]


def mat_vec(a, v):
    return [sum(a[i][k] * v[k] for k in range(3)) for i in range(3)]


IDENT = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]


def world_transforms(state):
    """Welt-Transform (R, t) je Objekt; Eltern zuerst (Index aufsteigend wie FUN_80014234)."""
    res = {}
    for s in state:
        R = rot_matrix(*s["rot"])
        t = list(s["pos"])
        p = s["parent"]
        if p >= 0 and p in res:
            PR, Pt = res[p]
            t = [a + b for a, b in zip(mat_vec(PR, t), Pt)]
            R = mat_mul(PR, R)
        res[s["obj"]] = (R, t)
    return res


def project(pw):
    """Welt -> Bildschirm mit der Tuerkamera."""
    zc = CAM_EYE_X - pw[0]
    if zc <= 1:
        return None
    return (SCREEN_CX + GEOM_H * pw[2] / zc, SCREEN_CY + GEOM_H * pw[1] / zc, zc)


def world_points(door, state):
    """Alle Dreiecke in Weltkoordinaten: Liste (obj, mesh, [p0,p1,p2])."""
    tr = world_transforms(state)
    out = []
    for s in state:
        if s["mesh"] >= len(door.meshes):
            continue
        m = door.meshes[s["mesh"]]
        R, t = tr[s["obj"]]
        vw = [[a + b for a, b in zip(mat_vec(R, v), t)] for v in m["verts"]]
        for tri in m["tris"]:
            if max(tri["v"]) < len(vw):
                out.append((s["obj"], s["mesh"], [vw[i] for i in tri["v"]]))
    return out


# ----------------------------------------------------------------------------------------
# Auswertung
# ----------------------------------------------------------------------------------------
def analyse_variant(door, variant, var0e=0):
    vm = VM(door, variant, var0e).run()
    frames = vm.frames
    n = len(frames)
    objs = {}
    for t, st in enumerate(frames):
        for s in st:
            o = objs.setdefault(s["obj"], {"obj": s["obj"], "mesh": s["mesh"], "parent": s["parent"],
                                           "flags0": s["flags"], "first": t, "pos0": s["pos"], "rot0": s["rot"],
                                           "track": []})
            o["track"].append((t, s["pos"], s["rot"], s["flags"]))
    summary = []
    for i in sorted(objs):
        o = objs[i]
        tr = o["track"]
        # bei Neu-Setzen (Door_model_set mitten im Ablauf) zaehlt die Bewegung je Abschnitt
        drot = [0, 0, 0]
        dpos = [0, 0, 0]
        ext_rot = [0, 0, 0]
        ext_pos = [0, 0, 0]
        first_move = last_move = None
        first_rot = last_rot = None
        acc_r = [0, 0, 0]
        acc_p = [0, 0, 0]
        alone = [0, 0, 0]      # Weg in Bildern OHNE x-Aenderung (= Tuerbewegung, nicht Kamerafahrt)
        alone_ext = [0, 0, 0]
        first_x = last_x = None
        prev = tr[0]
        for cur in tr[1:]:
            moved = False
            turned = False
            dxs = cur[1][0] - prev[1][0]
            if dxs:
                if first_x is None:
                    first_x = cur[0]
                last_x = cur[0]
            for a in range(3):
                dr = s16(cur[2][a] - prev[2][a])
                dp = cur[1][a] - prev[1][a]
                if dr or dp:
                    moved = True
                if dr:
                    turned = True
                acc_r[a] += dr
                acc_p[a] += dp
                if a and not dxs:
                    alone[a] += dp
                    if abs(alone[a]) > abs(alone_ext[a]):
                        alone_ext[a] = alone[a]
                if abs(acc_r[a]) > abs(ext_rot[a]):
                    ext_rot[a] = acc_r[a]
                if abs(acc_p[a]) > abs(ext_pos[a]):
                    ext_pos[a] = acc_p[a]
            if moved:
                if first_move is None:
                    first_move = cur[0]
                last_move = cur[0]
            if turned:
                if first_rot is None:
                    first_rot = cur[0]
                last_rot = cur[0]
            prev = cur
        drot = acc_r
        dpos = acc_p
        summary.append({
            "obj": i, "mesh": o["mesh"], "parent": o["parent"], "flags": o["flags0"],
            "first_frame": o["first"], "pos0": o["pos0"], "rot0": o["rot0"],
            "rot_extreme": ext_rot, "rot_end": drot, "pos_extreme": ext_pos, "pos_end": dpos,
            "first_move": first_move, "last_move": last_move,
            "first_rot": first_rot, "last_rot": last_rot,
            "pos_ohne_fahrt_extrem": alone_ext, "fahrt_x_von": first_x, "fahrt_x_bis": last_x,
            "frames_moving": (last_move - first_move + 1) if first_move is not None else 0,
            "frames_turning": (last_rot - first_rot + 1) if first_rot is not None else 0,
        })
    return vm, summary


def deg(u):
    return round(u * 360.0 / 4096.0, 2)


def classify(door, summary):
    """Bewegungsart aus den Bahnen. Wurzel = Objekt ohne Eltern (haengt an der Kamera),
    Kind = an ein anderes Objekt gebunden. Schwellen: 64 Winkeleinheiten (5,6 Grad), 200 Laengen.
    Fluegel = dreht um y UND (Wurzel ODER Mesh-Hoehe >= 3000)."""
    movers = [s for s in summary if s["first_move"] is not None]
    kinds = []
    detail = {"fluegel_dreh_y": [], "kind_dreh": [], "heben_y": [], "schieben_z": [], "fahrt_x": []}

    def big(s):
        return s["mesh"] < len(door.meshes) and door.meshes[s["mesh"]]["size"][1] >= 3000
    for s in movers:
        rx, ry, rz = s["rot_extreme"]
        px, py, pz = s["pos_extreme"]
        if abs(ry) >= 64 and (s["parent"] < 0 or big(s)):
            detail["fluegel_dreh_y"].append({"obj": s["obj"], "winkel": ry, "grad": deg(ry),
                                             "bild_von": s["first_rot"], "bild_bis": s["last_rot"]})
        elif abs(rx) >= 64 or abs(ry) >= 64 or abs(rz) >= 64:
            detail["kind_dreh"].append({"obj": s["obj"], "rot": [rx, ry, rz]})
        # Hub/Schub zaehlt nur in Bildern, in denen x ruht; sonst ist es die Kamerabahn
        _, ay, az = s["pos_ohne_fahrt_extrem"]
        if abs(ay) >= 200:
            detail["heben_y"].append({"obj": s["obj"], "weg": ay})
        if abs(az) >= 200:
            detail["schieben_z"].append({"obj": s["obj"], "weg": az})
        if abs(px) >= 200:
            detail["fahrt_x"].append({"obj": s["obj"], "weg": px, "y_gesamt": py, "z_gesamt": pz,
                                      "bild_von": s["fahrt_x_von"], "bild_bis": s["fahrt_x_bis"]})
    sw = detail["fluegel_dreh_y"]
    if sw:
        signs = {1 if x["winkel"] > 0 else -1 for x in sw}
        if len(sw) >= 2 and len(signs) == 2:
            kinds.append("Drehen Hochachse zweifluegelig (%s Grad)" % "/".join(str(x["grad"]) for x in sw))
        elif len(sw) >= 2:
            kinds.append("Drehen Hochachse, %d Objekte gleichsinnig (%s Grad)" % (
                len(sw), "/".join(str(x["grad"]) for x in sw)))
        else:
            kinds.append("Drehen Hochachse einfluegelig (%s Grad)" % sw[0]["grad"])
    if detail["heben_y"]:
        kinds.append("Heben/Senken y (%s)" % "/".join(str(x["weg"]) for x in detail["heben_y"][:3]))
    if detail["schieben_z"]:
        kinds.append("Schieben z (%s)" % "/".join(str(x["weg"]) for x in detail["schieben_z"][:3]))
    if detail["kind_dreh"]:
        kinds.append("Klinke/Anbauteil dreht (%d Obj.)" % len(detail["kind_dreh"]))
    if detail["fahrt_x"]:
        kinds.append("Kamerafahrt x (%s)" % "/".join(str(x["weg"]) for x in detail["fahrt_x"][:2]))
    if not kinds:
        kinds.append("keine Bewegung")
    return kinds, detail


def door_record(door):
    rec = {
        "name": door.name, "file": str(door.path.relative_to(REPO)).replace("\\", "/"),
        "file_size": len(door.data), "valid": door.valid,
        "table_entry_addr": "0x%08x" % (DOOR_TABLE + door.idx * 12),
        "snd_size": door.snd_size, "model_block_off": door.mo, "model_block_size": door.mdl_size,
        "md1_off": door.md1_off, "tim_off": door.tim_off, "scd_table_off": door.scd_tab,
        "n_scripts": door.n_scripts, "scd_offsets": door.scd_offs,
        "md1": {"len": door.md1_len, "objs": door.md1_objs, "n_meshes": len(door.meshes),
                "tris_total": sum(m["n_tris"] for m in door.meshes),
                "quads_total": sum(m["n_quads"] for m in door.meshes),
                "meshes": [dict({k: m[k] for k in ("index", "entry_file_off", "n_verts", "n_normals", "n_tris",
                                                   "n_quads", "bbox_min", "bbox_max", "size",
                                                   "uv_min", "uv_max", "cluts", "pages")},
                                komponenten=mesh_components(m), front=mesh_front_stats(m))
                           for m in door.meshes]},
        "tim": {"bpp": door.tim_bpp, "w": door.tex_w, "h": door.tex_h, "cluts": len(door.cluts),
                "img_rect_vram": door.tim_img_rect, "len": door.tim_len},
        "scripts": [], "variants": [],
    }
    clean = True
    for k in range(door.n_scripts):
        ins, ok = disasm_script(door, k)
        clean &= ok
        a, e = door.script_range(k)
        rec["scripts"].append({
            "index": k, "off_rel": a, "file_off": door.scd_tab + a, "len": e - a, "clean": ok,
            "n_ops": len(ins), "n_door_model_set": sum(1 for i in ins if i["op"] == 0x4D),
            "ops": sorted({i["name"] for i in ins}),
        })
    rec["scd_clean"] = clean
    # Varianten: Case-Werte des Switch in Skript 0
    ins0, _ = disasm_script(door, 0)
    cases = []
    sw = [i for i in ins0 if i["op"] == 0x13]
    rec["switch_var"] = sw[0]["f"]["var"] if sw else None
    for idx, i in enumerate(ins0):
        if i["op"] == 0x14:
            tgt = None
            for j in ins0[idx + 1:]:
                if j["op"] == 0x18:
                    tgt = j["f"]["script"]
                    break
                if j["op"] == 0x14:        # Durchfall: Case direkt ausgefuehrt = nur Vorschub (@0x80054100)
                    continue
                if j["op"] in (0x16, 0x1A):
                    break
            cases.append((i["f"]["val"], tgt))
    if not cases:
        cases = [(0, None)]
    rec["cases"] = [{"value": v, "gosub": t} for v, t in cases]
    for v, tgt in cases:
        vm, summary = analyse_variant(door, v)
        kinds, detail = classify(door, summary)
        tri_on_screen = 0
        if vm.frames:
            last_full = max(vm.frames, key=len)
            for s in last_full:
                if s["mesh"] < len(door.meshes):
                    tri_on_screen += door.meshes[s["mesh"]]["n_tris"]
        rec["variants"].append({
            "variant": v, "setup_script": tgt, "frames_total": len(vm.frames),
            "objects": summary, "n_objects": len(summary), "kinds": kinds, "detail": detail,
            "tris_instanced": tri_on_screen,
            "sounds": vm.sounds, "fades": vm.fades, "notes": sorted(vm.notes),
            "vars_read": sorted(vm.vars_read),
        })
    rec["rang"] = rank_record(rec)
    return rec


def rank_record(rec):
    """Punktwertung 'Vorbild fuer ein einfluegeliges Schwenktor aus Rohren' - nur Messwerte.
      B1 Bewegung : einfluegelige Drehung um die Hochachse 3 | zweifluegelig 1 | sonst 0
      B2 Fluegel durchbrochen (Deckungsgrad der Frontansicht < 0,90)            2
      B3 Staebe als deckellose Prismen im Fluegel-Mesh (>= 1)                    1
      B4 Aussenkontur nicht rechteckig (Ecken der konvexen Huelle > 4)           1
      B5 Pfosten als eigenes, selbst unbewegtes Wurzelobjekt mit bewegten Kindern 1
    Gleichstand: mehr Huell-Ecken zuerst (gerundete Ecken), dann duennere Staebe."""
    v = rec["variants"][0]
    det = v["detail"]
    sw = det["fluegel_dreh_y"]
    meshes = {m["index"]: m for m in rec["md1"]["meshes"]}
    objs = {o["obj"]: o for o in v["objects"]}
    if sw:
        leaf_mesh = objs[sw[0]["obj"]]["mesh"]
    else:
        inst = [o["mesh"] for o in v["objects"] if o["mesh"] in meshes]
        leaf_mesh = max(inst, key=lambda i: meshes[i]["size"][1] * max(1, meshes[i]["size"][2])) if inst else None
    if leaf_mesh is None or leaf_mesh not in meshes:
        return {"punkte": 0, "fluegel_mesh": None, "B": [0, 0, 0, 0, 0]}
    m = meshes[leaf_mesh]
    signs = {1 if x["winkel"] > 0 else -1 for x in sw}
    b1 = 3 if len(sw) == 1 else (1 if len(sw) >= 2 and len(signs) == 2 else 0)
    b2 = 2 if m["front"]["deckung"] < 0.90 else 0
    rods = [c for c in m["komponenten"] if c["stab"]]
    b3 = 1 if rods else 0
    b4 = 1 if m["front"]["huelle_ecken"] > 4 else 0
    b5 = 0
    for o in v["objects"]:
        if o["parent"] < 0 and o["mesh"] in meshes and o["first_rot"] is None:
            sz = sorted(meshes[o["mesh"]]["size"])
            kids = [k for k in v["objects"] if k["parent"] == o["obj"] and k["first_move"] is not None
                    and meshes.get(k["mesh"], {"size": [0, 0, 0]})["size"][1] >= 3000]
            if sz[1] <= 200 and sz[2] >= 3000 and kids:
                b5 = 1
    rod_d = min((sorted(c["size"])[1] for c in rods), default=0)
    return {"punkte": b1 + b2 + b3 + b4 + b5, "B": [b1, b2, b3, b4, b5], "fluegel_mesh": leaf_mesh,
            "fluegel_groesse": m["size"], "fluegel_tris": m["n_tris"], "deckung": m["front"]["deckung"],
            "huelle_ecken": m["front"]["huelle_ecken"], "staebe": len(rods),
            "stab_seiten": sorted({c["seiten"] for c in rods}), "stab_dicke_min": rod_d,
            "dreh_grad": [x["grad"] for x in sw]}


def cmd_rang():
    recs = [door_record(Door(i)) for i in range(N_DOORS)]
    rows = [(r["rang"]["punkte"], r["rang"].get("huelle_ecken", 0), -r["rang"].get("stab_dicke_min", 0), r)
            for r in recs]
    rows.sort(key=lambda x: (-x[0], -x[1], -x[2], x[3]["name"]))
    print("Platz Tuer    Pkt  B1 B2 B3 B4 B5  Fluegel(x,y,z)        Tris  Deckung Ecken Staebe Seiten Dicke Grad")
    for n, (p, _, _, r) in enumerate(rows, 1):
        g = r["rang"]
        if g["fluegel_mesh"] is None:
            print("%4d  %s  %2d   (kein Objekt instanziert)" % (n, r["name"], p))
            continue
        print("%4d  %s  %2d   %d  %d  %d  %d  %d  %-20s %4d  %.3f   %3d   %3d   %-5s %4d  %s" % (
            n, r["name"], p, g["B"][0], g["B"][1], g["B"][2], g["B"][3], g["B"][4], g["fluegel_groesse"],
            g["fluegel_tris"], g["deckung"], g["huelle_ecken"], g["staebe"], g["stab_seiten"],
            g["stab_dicke_min"], g["dreh_grad"]))


# ----------------------------------------------------------------------------------------
# Bilder
# ----------------------------------------------------------------------------------------
PALETTE = [(255, 220, 60), (80, 220, 255), (255, 120, 120), (140, 255, 140), (255, 160, 255),
           (255, 170, 60), (170, 170, 255), (200, 255, 220), (255, 255, 255), (150, 200, 120)]


def draw_wire(door, state, size=(320, 240), scale=1, by="obj"):
    from PIL import Image, ImageDraw
    img = Image.new("RGB", (size[0] * scale, size[1] * scale), (16, 18, 24))
    dr = ImageDraw.Draw(img)
    for obj, mesh, tri in world_points(door, state):
        ps = [project(p) for p in tri]
        if any(p is None for p in ps):
            continue
        col = PALETTE[(obj if by == "obj" else mesh) % len(PALETTE)]
        pts = [(p[0] * scale, p[1] * scale) for p in ps]
        dr.line(pts + [pts[0]], fill=col, width=1)
    return img


def draw_top(door, states, size=(480, 480), dist_max=11000.0):
    """Draufsicht: z nach rechts (wie auf dem Bildschirm), Kamera unten, Entfernung nach oben.
    states = Liste (Zustand, Farbe oder None)."""
    from PIL import Image, ImageDraw
    img = Image.new("RGB", size, (16, 18, 24))
    dr = ImageDraw.Draw(img)
    k = (size[1] - 30) / dist_max
    cx = size[0] / 2.0

    def P(p):
        return (cx + p[2] * k, size[1] - 15 - (CAM_EYE_X - p[0]) * k)
    dr.line([P((CAM_EYE_X, 0, -150)), P((CAM_EYE_X, 0, 150))], fill=(255, 255, 255), width=3)
    dr.text((cx + 8, size[1] - 14), "Kamera (10000,0,0)", fill=(200, 200, 200))
    for state, col in states:
        for obj, mesh, tri in world_points(door, state):
            pts = [P(p) for p in tri]
            dr.line(pts + [pts[0]], fill=col or PALETTE[obj % len(PALETTE)], width=1)
    return img


def key_frames(door, vm, summary):
    """t0 = erstes Bild mit voller Objektzahl; t1 = Bild, in dem die letzte Fluegelbewegung
    (Drehung oder Hub/Schub) endet, bevorzugt VOR Beginn der Kamerafahrt gemessen."""
    frames = vm.frames
    full = max(len(s) for s in frames) if frames else 0
    t0 = 0
    for t, st in enumerate(frames):
        if len(st) == full:
            t0 = t
            break
    cand = [s["last_rot"] for s in summary if s["last_rot"] is not None and
            (s["parent"] < 0 or (s["mesh"] < len(door.meshes) and door.meshes[s["mesh"]]["size"][1] >= 3000))]
    if cand:
        t1 = max(cand)
    else:
        # Schiebe-/Hubtueren: letztes Bild, in dem sich y oder z eines Objekts aendert
        t1 = t0
        prev = None
        for t, st in enumerate(frames):
            cur = [(s["obj"], s["pos"][1], s["pos"][2]) for s in st]
            if prev is not None and cur != prev:
                t1 = t
            prev = cur
    return t0, min(t1, len(frames) - 1)


def undolly(state, ref):
    """Kamerafahrt herausrechnen: Wurzelobjekte auf die x-Lage des Bezugsbildes setzen."""
    rx = {s["obj"]: s["pos"][0] for s in ref}
    out = []
    for s in state:
        c = dict(s)
        if s["parent"] < 0 and s["obj"] in rx:
            c["pos"] = [rx[s["obj"]], s["pos"][1], s["pos"][2]]
        out.append(c)
    return out


def sheet_for(door, rec, variant_idx=0, scale=2):
    from PIL import Image, ImageDraw
    v = rec["variants"][variant_idx]
    vm, summary = analyse_variant(door, v["variant"])
    frames = vm.frames
    t0, t1 = key_frames(door, vm, summary)
    open_state = undolly(frames[t1], frames[t0])
    tex = door.texture(0).resize((door.tex_w * scale, door.tex_h * scale), Image.NEAREST)
    w0 = draw_wire(door, frames[t0], scale=scale)
    w1 = draw_wire(door, open_state, scale=scale)
    top = draw_top(door, [(frames[t0], (120, 120, 130)), (open_state, None)])
    W = tex.width + w0.width + w1.width + top.width + 50
    H = max(tex.height, w0.height) + 60
    img = Image.new("RGB", (W, H), (40, 42, 50))
    dr = ImageDraw.Draw(img)
    dr.text((8, 4), "%s  Variante %d (Aufbau-Skript %s)  Bilder=%d  Objekte=%d  MD1-Dreiecke=%s  im Bild=%d" % (
        door.name, v["variant"], v["setup_script"], v["frames_total"], v["n_objects"],
        [m["n_tris"] for m in rec["md1"]["meshes"]], v["tris_instanced"]), fill=(255, 255, 255))
    dr.text((8, 20), "; ".join(v["kinds"]), fill=(255, 230, 120))
    dr.text((8, 36), "Textur %dx%d CLUT0 | Tuerkamera Bild %d (zu) | Bild %d (Fluegel am Anschlag, Kamerafahrt herausgerechnet) | "
            "Draufsicht grau=zu, farbig=offen" % (door.tex_w, door.tex_h, t0, t1), fill=(200, 200, 200))
    y = 56
    img.paste(tex, (8, y))
    img.paste(w0, (tex.width + 20, y))
    img.paste(w1, (tex.width + w0.width + 30, y))
    img.paste(top, (tex.width + w0.width + w1.width + 40, y))
    return img


def candidates_sheet(doors, recs, picks):
    """Ein Blatt: je Kandidat Textur + Frontansicht zu + offen + Draufsicht (Variante mit kleinstem Wert)."""
    from PIL import Image
    rows = [sheet_for(doors[i], recs[i], 0, scale=1) for i in picks]
    W = max(r.width for r in rows)
    H = sum(r.height for r in rows)
    img = Image.new("RGB", (W, H), (40, 42, 50))
    y = 0
    for r in rows:
        img.paste(r, (0, y))
        y += r.height
    return img


def contact_textures(doors, cols=11, scale=1):
    from PIL import Image, ImageDraw
    cw, ch = 128 * scale + 8, 256 * scale + 20
    rows = (len(doors) + cols - 1) // cols
    img = Image.new("RGB", (cols * cw, rows * ch), (40, 42, 50))
    dr = ImageDraw.Draw(img)
    for i, d in enumerate(doors):
        x, y = (i % cols) * cw, (i // cols) * ch
        t = d.texture(0)
        if scale != 1:
            t = t.resize((t.width * scale, t.height * scale), Image.NEAREST)
        img.paste(t, (x + 4, y + 16))
        dr.text((x + 4, y + 2), "%s %dx%d" % (d.name, d.tex_w, d.tex_h), fill=(255, 255, 255))
    return img


def contact_wires(doors, recs, cols=8):
    from PIL import Image, ImageDraw
    cw, ch = 328, 260
    rows = (len(doors) + cols - 1) // cols
    img = Image.new("RGB", (cols * cw, rows * ch), (40, 42, 50))
    dr = ImageDraw.Draw(img)
    for i, (d, r) in enumerate(zip(doors, recs)):
        x, y = (i % cols) * cw, (i // cols) * ch
        v = r["variants"][0]
        vm, _ = analyse_variant(d, v["variant"])
        st = max(vm.frames, key=len) if vm.frames else []
        # erstes Bild mit voller Objektzahl
        for fr in vm.frames:
            if len(fr) == len(st):
                st = fr
                break
        img.paste(draw_wire(d, st), (x + 4, y + 16))
        dr.text((x + 4, y + 2), "%s V%d %s" % (d.name, v["variant"], "; ".join(v["kinds"])[:52]), fill=(255, 255, 255))
    return img


# ----------------------------------------------------------------------------------------
# Kommandozeile
# ----------------------------------------------------------------------------------------
def cmd_dis(idx, only=None):
    d = Door(idx)
    print("%s  Datei %s  Modellteil @0x%x  SCD-Tabelle @0x%x  Skripte %d" % (
        d.name, d.path.name, d.mo, d.scd_tab, d.n_scripts))
    for k in range(d.n_scripts):
        if only is not None and k != only:
            continue
        ins, ok = disasm_script(d, k)
        a, e = d.script_range(k)
        print("\n-- Skript %d: rel 0x%03x..0x%03x  Datei 0x%05x  %d B  %s" % (
            k, a, e, d.scd_tab + a, e - a, "sauber" if ok else "BRUCH"))
        for i in ins:
            print("  +0x%03x  @0x%05x  %-66s  %s" % (i["off"] - a, d.scd_tab + i["off"], i["raw"], i["text"]))


def cmd_trace(idx, variant, only_obj=None, every=1):
    d = Door(idx)
    vm, summary = analyse_variant(d, variant)
    print("%s Variante %d: %d Bilder; Hinweise: %s" % (d.name, variant, len(vm.frames), sorted(vm.notes)))
    for s in summary:
        print("  Obj %d mesh %d Eltern %d: pos0=%s rot0=%s  rot_extrem=%s rot_ende=%s pos_extrem=%s pos_ende=%s  bewegt Bild %s..%s" % (
            s["obj"], s["mesh"], s["parent"], s["pos0"], s["rot0"], s["rot_extreme"], s["rot_end"],
            s["pos_extreme"], s["pos_end"], s["first_move"], s["last_move"]))
    print("Klang:", vm.sounds)
    print("Blende:", vm.fades)
    prev = None
    for t, st in enumerate(vm.frames):
        row = [(s["obj"], tuple(s["pos"]), tuple(s["rot"]), s["flags"]) for s in st
               if only_obj is None or s["obj"] == only_obj]
        if row != prev or t == len(vm.frames) - 1:
            if t % every == 0 or row != prev:
                print("  Bild %4d: %s" % (t, "  ".join("o%d p=%s r=%s f=%04x" % r for r in row)))
        prev = row


def cmd_mesh(idx):
    d = Door(idx)
    print("%s MD1 @0x%x len=0x%x objs=%d  TIM @0x%x %dx%d %dbpp CLUTs=%d" % (
        d.name, d.md1_off, d.md1_len, d.md1_objs, d.tim_off, d.tex_w, d.tex_h, d.tim_bpp, len(d.cluts)))
    for m in d.meshes:
        print("  Mesh %d @0x%x: %d Vertices, %d Dreiecke, %d Vierecke; bbox min=%s max=%s Groesse=%s" % (
            m["index"], m["entry_file_off"], m["n_verts"], m["n_tris"], m["n_quads"],
            m["bbox_min"], m["bbox_max"], m["size"]))


def cmd_vergleich(a, b):
    """Byte-Vergleich zweier Skriptbloecke (ab SCD-Offsettabelle)."""
    da, db = Door(a), Door(b)
    ba, bb = da.scd_block(), db.scd_block()
    print("%s %d B, Offsets %s" % (da.name, len(ba), da.scd_offs))
    print("%s %d B, Offsets %s" % (db.name, len(bb), db.scd_offs))
    diff = [i for i in range(min(len(ba), len(bb))) if ba[i] != bb[i]]
    print("abweichende Bytes: %d" % (len(diff) + abs(len(ba) - len(bb))))
    for i in diff:
        k = max(j for j in range(da.n_scripts) if da.scd_offs[j] <= i)
        print("  Block 0x%03x  Skript %d +0x%02x  Datei %s 0x%05x: %02x   %s 0x%05x: %02x" % (
            i, k, i - da.scd_offs[k], da.name, da.scd_tab + i, ba[i], db.name, db.scd_tab + i, bb[i]))


def cmd_platte():
    """Gruppen gleicher Mesh-0-Geometrie (Vertex- und Indexlisten) ueber alle Archive."""
    import hashlib
    g = {}
    for i in range(N_DOORS):
        d = Door(i)
        m = d.meshes[0]
        h = hashlib.sha1(repr((m["verts"], [tr["v"] for tr in m["tris"]])).encode()).hexdigest()[:8]
        g.setdefault(h, []).append((d.name, m["n_verts"], m["n_tris"], m["size"]))
    for h, v in sorted(g.items(), key=lambda x: -len(x[1])):
        print("%s  %2d Tueren  %d Vertices %d Dreiecke %s: %s" % (
            h, len(v), v[0][1], v[0][2], v[0][3], " ".join(n[4:] for n, _, _, _ in v)))


def cmd_gelb():
    """Anteil kraeftig gelber Pixel im Blattbereich (Zeilen 0..219) je Textur."""
    for i in range(N_DOORS):
        d = Door(i)
        px = d.texture(0).load()
        n = tot = 0
        for y in range(220):
            for x in range(d.tex_w):
                r, g, b = px[x, y]
                tot += 1
                if r >= 150 and g >= 110 and b <= 70 and r - b >= 100:
                    n += 1
        print("%s  gelb %.4f" % (d.name, n / float(tot)))


def cmd_all(sheets):
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    doors = [Door(i) for i in range(N_DOORS)]
    recs = [door_record(d) for d in doors]
    ops_json = {"0x%02x" % k: {"name": v[0], "len": v[1], "beleg": v[2],
                               "handler": "0x%08x" % exe_u32(OP_TABLE + k * 4)} for k, v in OPS.items()}
    mset = member_table(MEMBER_SET_TABLE, False)
    out = {
        "quelle": {"exe": "info/re2leon/PSX.EXE", "door_table": "0x%08x" % DOOR_TABLE,
                   "op_table": "0x%08x" % OP_TABLE, "kamera_auge": [CAM_EYE_X, 0, 0], "geom_h": GEOM_H},
        "opcodes": ops_json,
        "member_set": {str(k): {"breite": v[0], "offset": v[1], "ziel": "0x%08x" % v[2]} for k, v in mset.items()},
        "doors": recs,
    }
    (OUT_DIR / "tuerkatalog.json").write_text(json.dumps(out, indent=1), encoding="utf-8")
    contact_textures(doors).save(OUT_DIR / "katalog_texturen.png")
    for r in range((N_DOORS + 10) // 11):      # vergroessert (NEAREST x2), je 11 Tueren
        contact_textures(doors[r * 11:(r + 1) * 11], cols=11, scale=2).save(
            OUT_DIR / ("katalog_texturen_reihe%d.png" % r))
    contact_wires(doors, recs).save(OUT_DIR / "katalog_drahtgitter.png")
    # volle Bild-Zeitachse der Kandidaten (nur Bilder, in denen sich etwas aendert)
    for idx in sheets:
        for v in recs[idx]["variants"]:
            vm, _ = analyse_variant(doors[idx], v["variant"])
            za = []
            prev = None
            for t, st in enumerate(vm.frames):
                row = [[s["obj"], s["mesh"], s["parent"]] + s["pos"] + s["rot"] + [s["flags"]] for s in st]
                if row != prev:
                    za.append({"bild": t, "objekte": row})
                prev = row
            v["zeitachse_spalten"] = ["obj", "mesh", "eltern", "x", "y", "z", "rx", "ry", "rz", "flags"]
            v["zeitachse"] = za
            v["ablauf"] = [{"bild": a, "platz": b, "rel": c, "text": d, "bytes": e}
                           for (a, b, c, d, e) in vm.log
                           if not d.startswith(("Add_", "Evt_next", "Next", "Sleeping", "Nop"))]
    (OUT_DIR / "tuerkatalog.json").write_text(json.dumps(out, indent=1), encoding="utf-8")
    for idx in sheets:
        d = doors[idx]
        for vi in range(len(recs[idx]["variants"])):
            sheet_for(d, recs[idx], vi).save(OUT_DIR / ("katalog_%s_v%d.png" % (d.name, recs[idx]["variants"][vi]["variant"])))
    if sheets:
        candidates_sheet(doors, recs, sheets).save(OUT_DIR / "katalog_kandidaten.png")
    # Kurzuebersicht
    for r in recs:
        for v in r["variants"]:
            print("%s V%d skr=%s meshes=%d tris=%s tex=%dx%d skripte=%d obj=%d bilder=%d  %s  %s" % (
                r["name"], v["variant"], v["setup_script"], r["md1"]["n_meshes"],
                [m["n_tris"] for m in r["md1"]["meshes"]], r["tim"]["w"], r["tim"]["h"], r["n_scripts"],
                v["n_objects"], v["frames_total"], "; ".join(v["kinds"]), v["notes"] or ""))
    bad = [r["name"] for r in recs if not (r["valid"] and r["scd_clean"])]
    print("Container/Skripte mit Bruch:", bad)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dis", type=lambda s: int(s, 16))
    ap.add_argument("--script", type=int)
    ap.add_argument("--trace", nargs=2)
    ap.add_argument("--obj", type=int)
    ap.add_argument("--mesh", type=lambda s: int(s, 16))
    ap.add_argument("--sheets", default="")
    ap.add_argument("--rang", action="store_true")
    ap.add_argument("--vergleich", nargs=2)
    ap.add_argument("--platte", action="store_true")
    ap.add_argument("--gelb", action="store_true")
    a = ap.parse_args()
    if a.rang:
        cmd_rang()
    elif a.vergleich:
        cmd_vergleich(int(a.vergleich[0], 16), int(a.vergleich[1], 16))
    elif a.platte:
        cmd_platte()
    elif a.gelb:
        cmd_gelb()
    elif a.dis is not None:
        cmd_dis(a.dis, a.script)
    elif a.trace:
        cmd_trace(int(a.trace[0], 16), int(a.trace[1]), a.obj)
    elif a.mesh is not None:
        cmd_mesh(a.mesh)
    else:
        sheets = [int(s, 16) for s in a.sheets.split(",") if s]
        cmd_all(sheets)


if __name__ == "__main__":
    main()
