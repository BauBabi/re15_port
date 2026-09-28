#!/usr/bin/env python3
"""tor_massstab.py - Massstab zwischen RAUMWELT und TUERSZENE (RE2 Retail / RE1.5).

Der Faktor k = Einheiten der Tuerszene je Raumwelt-Einheit steht in keinem Code
(die Tuerszene ist eine eigene Welt mit eigener Kamera). Er wird hier GEMESSEN:
an Tueren, die im Raumhintergrund zu sehen sind und deren Door_aot_set ein
Archiv waehlt, dessen mesh0 das Standardblatt ist (x -145..143, y -6600..2,
z -3599..0 -> 288 x 6602 x 3599).

Nur LESEND gegenueber allen Originaldaten. Schreibt ausschliesslich nach
build/tor_1170/massstab.json und build/tor_1170/massstab_*.png.

Befehle:
  zensus                      Standardblatt-Archive + Tuersaetze, die sie waehlen
  kandidaten                  RE2-Tueren/Cuts, nach Naehe sortiert
  bodenzensus                 Ziel-y gegen Ziel-Band aller Tuersaetze (Boden = -Band*1800)
  lupe SPIEL RAUM CUT x0 y0 x1 y1 [F]   vergroesserter Ausschnitt mit Pixelraster
  messen                      alle Tueren der Tabelle TUEREN vermessen (JSON + Bilder)
  gegenprobe [k]              Spielerhoehe gegen Blatt-/Knaufhoehe
  wiederholprobe              dieselbe Tuer (RE2 ROOM5050) aus zwei Kameras
  alles                       zensus + messen + auswerten + gegenprobe, schreibt massstab.json

Verfahren je Tuer (Verfahren A, traegt das Ergebnis):
  1. Vier Blattecken am vergroesserten Hintergrund naehern (Tabelle TUEREN).
  2. Jede Kante verfeinern (kante_verfeinern): Gradientenmaximum quer zur Kante an jedem
     Pixel der mittleren 70 %, Gerade durch die Maxima; Ecken = Geradenschnitte.
  3. Rechteck in einer senkrechten Ebene, Unterkante auf Bodenhoehe y = -Band*1800, mit
     5 Groessen (x0, z0, Richtung, Breite, Hoehe) an die 8 Eckkoordinaten anpassen.
     Der Restfehler (rms) prueft, ob das Viereck im Bild ueberhaupt ein Rechteck in der
     Wand sein KANN; den Massstab selbst traegt allein die Bodenhoehe.
  4. k_Hoehe = 6602 / Hoehe, k_Breite = 3599 / Breite.
Verfahren B (nur Gegenprobe): Ecken auf die naechste achsparallele AOT- bzw. SCA-Kante.
Die Kollisionszellen liegen bis ~1000 Einheiten VOR der sichtbaren Wand und taugen deshalb
nicht als Wandebene.

Herkunft der Formeln:
  H = fov >> 7      RE1.5 @0x80021e68 lhu a0,2(v0) / @0x80021e70 srl a0,a0,7 (Delay-Slot
                    von jal 0x80066c30 = SetGeomScreen, dort ctc2 a0,$26)
                    RE2   @0x8002c138 lhu a0,2(v0) / @0x8002c140 srl a0,a0,7 (jal 0x8008de24)
  Kamerasatz 32 B   u16 flag, u16 fov, s32 ort[3], s32 ziel[3], u32 pri; Tabelle =
                    RDT-Zeiger +36 (beide Spiele: lw v1,36(v1); sll v0,v0,5)
  Blickmatrix       tor_kamera.Kamera (= re15_port/engine/src/camera_common.c:65-115)
  Boden             y = -Band * 1800 (RE2-Zensus: Ziel-y gegen Ziel-Band in 572 Tuersaetzen)
  Door_aot_set      0x3B, 32 B: +1 aot, +4 Band, +6 x, +8 z, +10 w, +12 d,
                    +14/16/18 Ziel x/y/z, +20 Richtung, +22 Stage, +23 Raum, +24 Cut,
                    +25 Ziel-Band, +26 Archiv (Tuertextur-Typ), +27 Variante
"""
import glob
import json
import math
import os
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.join(REPO, "analysis", "nutzer_batch_2026-08-27", "tools"))

import tor_kamera as K          # noqa: E402  (nur lesend)
import do2_format as F          # noqa: E402  (nur lesend)

AUS = os.path.join(REPO, "build", "tor_1170")
JSON_PFAD = os.path.join(AUS, "massstab.json")

BLATT_X = (-145, 143)      # Dicke   (DOOR00 mesh0, MD1 @Datei 0x5224 in RE2 DOOR00)
BLATT_Y = (-6600, 2)       # Hoehe
BLATT_Z = (-3599, 0)       # Breite
BLATT_H = BLATT_Y[1] - BLATT_Y[0]    # 6602
BLATT_B = BLATT_Z[1] - BLATT_Z[0]    # 3599
KNAUF_H = 3224             # Knauf (mesh1) haengt bei y = -3224 am Blatt
BAND = 1800                # 0x708


def rel(p):
    return os.path.relpath(p, REPO).replace("\\", "/")


# ----------------------------------------------------------------------------
# Raumzugriff fuer beide Spiele
# ----------------------------------------------------------------------------
class Raum(object):
    def __init__(self, spiel, name):
        """spiel 're2' | 're15'; name z.B. '7030' (Stage, Raum zweistellig hex, Spieler)."""
        self.spiel = spiel
        self.name = name.upper()
        st, rr = self.name[0], self.name[1:3]
        if spiel == "re2":
            self.pfad = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM%s.RDT" % self.name)
            self.bg = os.path.join(REPO, "info", "re2leon", "COMMON", "BSS",
                                   "ROOM%s%s" % (st, rr), "ROOM%s%s%%02d.bmp" % (st, rr))
        else:
            self.pfad = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE%s" % st,
                                     "ROOM%s.RDT" % self.name)
            self.bg = os.path.join(REPO, "extracted", "PSX", "STAGE%s" % st,
                                   "ROOM%s%s" % (st, rr), "ROOM%s%s%%02d.bmp" % (st, rr))
        self.d = open(self.pfad, "rb").read()
        self.ncut = self.d[1]
        if spiel == "re2":
            self.offs = struct.unpack_from("<23I", self.d, 8)
            self.cam_off = self.offs[7]
            self.sca_off = self.offs[6]
        else:
            self.cam_off = struct.unpack_from("<I", self.d, 0x24)[0]
            self.sca_off = struct.unpack_from("<I", self.d, 0x20)[0]

    def kamera(self, cut, exakt=True):
        o = self.cam_off + 32 * cut
        return K.Kamera(cut, o, self.d[o:o + 32], exakt=exakt)

    def hintergrund(self, cut):
        from PIL import Image
        return np.asarray(Image.open(self.bg % cut).convert("RGB"), np.uint8)

    def sca(self):
        """-> [dict(x,z,w,d,form,off)] (Hintergrund-Hilfslinien, nicht Messgrundlage)."""
        d, s = self.d, self.sca_off
        aus = []
        if self.spiel == "re2":
            # Kopf 16 B: s16 cx, s16 cz, u32 anzahl(+1), s32 decke, u32 0xc5c5c5c5
            n = struct.unpack_from("<I", d, s + 4)[0]
            for i in range(max(0, n - 1)):
                o = s + 16 + 16 * i
                x, z, w, dd, idf, typ, fl = struct.unpack_from("<hhHHHHI", d, o)
                aus.append(dict(off=o, x=x, z=z, w=w, d=dd, form=idf & 0xF, roh=d[o:o + 16].hex(" ")))
        else:
            kopf, zellen = K.sca_eindeutig(d)
            for c in zellen:
                aus.append(dict(off=c["off"], x=c["x"], z=c["z"], w=c["w"], d=c["d"],
                                form=c["typ"], roh=c["roh"]))
        return aus

    def tueren(self):
        """Alle Door_aot_set (0x3B) des Raums, je Datei-Offset einmal."""
        d = self.d
        aus, gesehen = [], set()
        if self.spiel == "re2":
            import re2_scd_walk as W
            for nm, base, subs in W.scd_blocks(d):
                for i in range(len(subs)):
                    s = base + subs[i]
                    e = W.block_end(d, base, subs, i, self.offs)
                    if e <= s or e > len(d):
                        continue
                    ops, st = W.walk(d, s, e, i + 1 == len(subs))
                    for (pc, op, ln) in ops:
                        if op == 0x3B and pc not in gesehen:
                            gesehen.add(pc)
                            aus.append(_tuersatz(d, pc))
        else:
            import re15_scd_walk as W
            tbl = struct.unpack_from("<24I", d, 0x20)
            for nm, base, subs in W.blocks(d):
                for i, o in enumerate(subs):
                    if i + 1 < len(subs):
                        e = base + subs[i + 1]
                    else:
                        cand = [x for x in tbl if base + o < x <= len(d)]
                        e = min(cand) if cand else len(d)
                    pc = base + o
                    while pc < e:
                        n = W.oplen(d, pc)
                        if n == 0 or pc + n > e:
                            break
                        if d[pc] == 0x3B and pc not in gesehen:
                            gesehen.add(pc)
                            aus.append(_tuersatz(d, pc))
                        pc += n
        return aus


def _tuersatz(d, pc):
    x, z, w, dp = struct.unpack_from("<hhHH", d, pc + 6)
    nx, ny, nz, nd = struct.unpack_from("<hhhh", d, pc + 14)
    return dict(pc=pc, roh=d[pc:pc + 32].hex(" "), aot=d[pc + 1], band=d[pc + 4],
                rect=(x, z, w, dp), ziel=(nx, ny, nz), ziel_dir=nd,
                ziel_stage=d[pc + 22], ziel_raum=d[pc + 23], ziel_cut=d[pc + 24],
                ziel_band=d[pc + 25], archiv=d[pc + 26], variante=d[pc + 27] & 0x7F)


# ----------------------------------------------------------------------------
# Standardblatt-Archive
# ----------------------------------------------------------------------------
_STD = None


def standard_archive():
    """-> {archivnummer: dict(meshes, name)} fuer RE2-Archive, deren mesh0 das Standardblatt ist."""
    global _STD
    if _STD is not None:
        return _STD
    ref = None
    aus = {}
    for name, p in F.alle_archive():
        do2 = F.Do2.lesen(open(p, "rb").read())
        md = F.Md1.lesen(do2.md1)
        ecken = tuple(sorted(v[:3] for v in md.meshes[0].vertices))
        if ref is None:
            ref = ecken            # RE1.5 DOOR00 mesh0
            xs = [(min(v[k] for v in ecken), max(v[k] for v in ecken)) for k in range(3)]
            assert xs == [BLATT_X, BLATT_Y, BLATT_Z], xs
        if name.startswith("RE2/") and ecken == ref:
            nr = int(name[-2:], 16)
            aus[nr] = dict(name=name, meshes=len(md.meshes))
    _STD = aus
    return aus


def cmd_zensus():
    std = standard_archive()
    einzel = sorted(n for n, v in std.items() if v["meshes"] <= 2)
    print("Standardblatt als mesh0: %d von 55 RE2-Archiven" % len(std))
    print("  alle      :", " ".join("%02X" % n for n in sorted(std)))
    print("  <=2 Meshes:", " ".join("%02X" % n for n in einzel))
    import re2_scd_walk as W
    n_alle = n_std = 0
    gesehen = set()
    for fn, d, name, base, i, s, e, last in W.all_subs():
        ops, st = W.walk(d, s, e, last)
        for (pc, op, ln) in ops:
            if op != 0x3B or (fn, pc) in gesehen:
                continue
            gesehen.add((fn, pc))
            n_alle += 1
            if d[pc + 26] in std:
                n_std += 1
    print("Door_aot_set (0x3B) in %d RDT-Saetzen, davon %d mit Standardblatt-Archiv" % (n_alle, n_std))
    return dict(standard_archive=sorted(std), einzelblatt=einzel, saetze=n_alle, saetze_standard=n_std)


def cmd_bodenzensus():
    """Ziel-y gegen Ziel-Band ueber alle Tuersaetze: belegt Boden y = -Band * 1800."""
    from collections import Counter
    aus = {}
    for spiel in ("re2", "re15"):
        c = Counter()
        for nm, t in alle_tueren(spiel):
            c[(t["ziel_band"], t["ziel"][1])] += 1
        passt = sum(v for (b, y), v in c.items() if y == -b * BAND)
        aus[spiel] = dict(saetze=sum(c.values()), passt=passt,
                          abweichend={"%d/%d" % k: v for k, v in sorted(c.items())
                                      if k[1] != -k[0] * BAND})
        print("%-4s %d Tuersaetze, Ziel-y == -Band*1800 in %d; abweichend (Band/y): %s" % (
            spiel, aus[spiel]["saetze"], passt, aus[spiel]["abweichend"]))
    return aus


def cmd_kandidaten(grenze=60):
    std = standard_archive()
    zeilen = []
    for p in sorted(glob.glob(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM*.RDT"))):
        nm = os.path.basename(p)[4:8]
        if nm[0] not in "1234567" or nm[3] != "0":
            continue
        r = Raum("re2", nm)
        rects = set()
        for t in r.tueren():
            if t["archiv"] not in std or std[t["archiv"]]["meshes"] > 2 or t["rect"] in rects:
                continue
            rects.add(t["rect"])
            x, z, w, dp = t["rect"]
            for c in range(r.ncut):
                cam = r.kamera(c)
                sx, sy, vz = cam.bild(np.array([x + w / 2.0, -t["band"] * BAND, z + dp / 2.0]))
                if 3000 < vz < 8500 and 50 < sx < 270 and 110 < sy < 236:
                    zeilen.append((float(vz), nm, c, t["pc"], t["archiv"], t["variante"], t["rect"]))
    zeilen.sort()
    for z in zeilen[:grenze]:
        print("vz=%5.0f ROOM%s cut %2d satz@0x%05X archiv %02X variante %d rect %s" % z)
    return zeilen


# ----------------------------------------------------------------------------
# Bildhilfen
# ----------------------------------------------------------------------------
def helligkeit(rgb):
    return rgb[..., 0] * 0.299 + rgb[..., 1] * 0.587 + rgb[..., 2] * 0.114


def abtasten(lum, x, y):
    """Bilinear; Koordinaten kontinuierlich, Pixel i deckt [i, i+1), Mitte bei i+0.5."""
    h, w = lum.shape
    fx = np.clip(np.asarray(x, float) - 0.5, 0, w - 1.001)
    fy = np.clip(np.asarray(y, float) - 0.5, 0, h - 1.001)
    x0 = np.floor(fx).astype(int)
    y0 = np.floor(fy).astype(int)
    ax, ay = fx - x0, fy - y0
    return (lum[y0, x0] * (1 - ax) * (1 - ay) + lum[y0, x0 + 1] * ax * (1 - ay) +
            lum[y0 + 1, x0] * (1 - ax) * ay + lum[y0 + 1, x0 + 1] * ax * ay)


def aufhellen(teil, hell):
    """NUR fuer die Ansicht: Spreizung auf das 1..99,5-Perzentil und Gamma. Gemessen wird
    immer am unveraenderten Bild."""
    if not hell:
        return teil
    t = teil.astype(float)
    lo, hi = np.percentile(t, 1), np.percentile(t, 99.5)
    t = np.clip((t - lo) / max(1.0, hi - lo), 0, 1) ** 0.6
    return (t * 255).astype(np.uint8)


def cmd_lupe(spiel, raum, cut, x0, y0, x1, y1, f=6, ziel=None, marken=None, linien=None,
             hell=False):
    from PIL import Image, ImageDraw
    r = Raum(spiel, raum)
    bg = r.hintergrund(cut)
    x0, y0 = max(0, x0), max(0, y0)
    x1, y1 = min(bg.shape[1], x1), min(bg.shape[0], y1)
    aus = Image.fromarray(aufhellen(bg[y0:y1, x0:x1], hell)).resize(
        ((x1 - x0) * f, (y1 - y0) * f), Image.NEAREST)
    rand = 22
    bild = Image.new("RGB", (aus.size[0] + rand, aus.size[1] + rand), (30, 30, 30))
    bild.paste(aus, (rand, rand))
    dr = ImageDraw.Draw(bild)
    for x in range(x0, x1 + 1):
        if x % 5 == 0:
            px = rand + (x - x0) * f
            dr.line([(px, rand - 5), (px, bild.size[1])], fill=(70, 70, 90) if x % 10 else (110, 110, 150))
            if x % 10 == 0:
                dr.text((px + 1, 2), "%d" % x, fill=(255, 255, 0))
    for y in range(y0, y1 + 1):
        if y % 5 == 0:
            py = rand + (y - y0) * f
            dr.line([(rand - 5, py), (bild.size[0], py)], fill=(70, 70, 90) if y % 10 else (110, 110, 150))
            if y % 10 == 0:
                dr.text((0, py + 1), "%d" % y, fill=(255, 255, 0))

    def P(p):
        return (rand + (p[0] - x0) * f, rand + (p[1] - y0) * f)
    for (a, b, farbe) in (linien or []):
        dr.line([P(a), P(b)], fill=farbe)
    for (p, farbe) in (marken or []):
        q = P(p)
        dr.line([(q[0] - 4, q[1]), (q[0] + 4, q[1])], fill=farbe)
        dr.line([(q[0], q[1] - 4), (q[0], q[1] + 4)], fill=farbe)
    os.makedirs(AUS, exist_ok=True)
    ziel = ziel or os.path.join(AUS, "massstab_lupe_%s_%s_c%02d.png" % (spiel, raum, cut))
    bild.save(ziel)
    print(rel(ziel), bild.size)
    return ziel


# ----------------------------------------------------------------------------
# Kanten verfeinern: Gradientenmaximum quer zur Kante, Gerade durch die Maxima
# ----------------------------------------------------------------------------
def kante_verfeinern(lum, a, b, R=3.5, schritt=0.25, rand=0.15, fenster=1.0, art="kante",
                     suche=1.5):
    """a, b = Naeherung der Kante (kontinuierliche Bildkoordinaten, Umlauf TL-TR-BR-BL:
    die Normale n zeigt dann immer ins Blatt).
    art 'kante': Ort des groessten Helligkeitssprungs quer zur Kante (Vorzeichen frei).
    art 'fuge' : die Naeherung liegt auf einer dunklen Linie (Fuge zwischen Blatt und Zarge).
                 Erst die Mitte der Linie suchen (Helligkeitsminimum), dann die blattseitige
                 Flanke: groesster Anstieg in Richtung Blatt, hoechstens 2 Pixel hinter der
                 Linienmitte. Blattgrenze = diese Flanke.
    Gesucht wird nur innerhalb +-suche Pixel um die Naeherung, je Abtastpunkt dann innerhalb
    +-fenster um das Maximum des gemittelten Profils.
    -> dict(a, b, n, rest, staerke, vorzeichen, verschiebung)  a/b = verfeinerte Endpunkte."""
    a = np.asarray(a, float)
    b = np.asarray(b, float)
    L = float(np.linalg.norm(b - a))
    t = (b - a) / L
    n = np.array([-t[1], t[0]])
    m = max(8, int(round(L * (1 - 2 * rand))))
    ss = np.linspace(rand, 1 - rand, m)
    offs = np.arange(-R, R + 1e-9, schritt)
    P = np.empty((m, len(offs)))
    for i, s in enumerate(ss):
        c = a + s * L * t
        P[i] = abtasten(lum, c[0] + offs * n[0], c[1] + offs * n[1])
    k = int(round(0.5 / schritt))
    D = P[:, 2 * k:] - P[:, :-2 * k]          # Differenz ueber 1 Pixel, mittig
    od = offs[k:-k]
    mittel = D.mean(axis=0)
    linie = None
    if art == "fuge":
        F_ = 0.5 * (P[:, 4 * k:] + P[:, :-4 * k]) - P[:, 2 * k:-2 * k]
        of = offs[2 * k:-2 * k]
        fm = F_.mean(axis=0)
        jl = int(np.argmax(np.where(np.abs(of) <= suche, fm, -1e9)))
        linie = float(of[jl])
        bereich = (od >= linie) & (od <= linie + 2.0)
        j0 = int(np.argmax(np.where(bereich, mittel, -1e9)))
        vz = 1.0
    else:
        nah = np.abs(od) <= suche
        j0 = int(np.argmax(np.where(nah, np.abs(mittel), -1e9)))
        vz = 1.0 if mittel[j0] >= 0 else -1.0
    o_i = np.full(m, np.nan)
    maske = np.abs(od - od[j0]) <= fenster
    for i in range(m):
        z = np.where(maske, vz * D[i], -1e9)
        j = int(np.argmax(z))
        if z[j] <= 0 or j == 0 or j == len(od) - 1:
            continue
        y0, y1, y2 = vz * D[i, j - 1], vz * D[i, j], vz * D[i, j + 1]
        nen = (y0 - 2 * y1 + y2)
        sub = 0.0 if abs(nen) < 1e-9 else 0.5 * (y0 - y2) / nen
        o_i[i] = od[j] + float(np.clip(sub, -1, 1)) * schritt
    gut = ~np.isnan(o_i)
    if gut.sum() < 4:
        return dict(a=a, b=b, n=0, von=int(m), rest=float("nan"), staerke=float(abs(mittel[j0])),
                    vorzeichen=vz, verschiebung=0.0, ok=False, linie=linie)
    for _ in range(4):
        A = np.vstack([np.ones(gut.sum()), ss[gut] - 0.5]).T
        p, *_ = np.linalg.lstsq(A, o_i[gut], rcond=None)
        res = o_i - (p[0] + p[1] * (ss - 0.5))
        neu_gut = (~np.isnan(o_i)) & (np.abs(res) <= 1.0)
        if neu_gut.sum() < 4 or (neu_gut == gut).all():
            break
        gut = neu_gut
    rest = float(np.sqrt(np.mean(res[gut] ** 2)))
    a2 = a + (p[0] - 0.5 * p[1]) * n
    b2 = b + (p[0] + 0.5 * p[1]) * n
    return dict(a=a2, b=b2, n=int(gut.sum()), von=int(m), rest=rest, linie=linie,
                staerke=float(abs(mittel[j0])), vorzeichen=vz, verschiebung=float(p[0]), ok=True)


def schnitt(g1, g2):
    """Schnittpunkt zweier Geraden (je zwei Punkte)."""
    p, r = np.asarray(g1[0], float), np.asarray(g1[1], float) - np.asarray(g1[0], float)
    q, s = np.asarray(g2[0], float), np.asarray(g2[1], float) - np.asarray(g2[0], float)
    den = r[0] * s[1] - r[1] * s[0]
    u = ((q[0] - p[0]) * s[1] - (q[1] - p[1]) * s[0]) / den
    return p + u * r


def ecken_verfeinern(lum, ecken, fest=(), fuge=()):
    """ecken = [TL, TR, BR, BL] Naeherung. fest = Namen der Kanten, die NICHT verfeinert
    werden ('oben','rechts','unten','links'); fuge = Kanten, an denen die Mitte der dunklen
    Fuge die Blattgrenze ist. -> (ecken_neu, kanteninfo)"""
    TL, TR, BR, BL = [np.asarray(e, float) for e in ecken]
    kanten = dict(oben=(TL, TR), rechts=(TR, BR), unten=(BR, BL), links=(BL, TL))
    info, ger = {}, {}
    for name, (a, b) in kanten.items():
        if name in fest:
            info[name] = dict(ok=False, fest=True)
            ger[name] = (a, b)
            continue
        r = kante_verfeinern(lum, a, b, art=("fuge" if name in fuge else "kante"))
        info[name] = dict(ok=bool(r["ok"]), art=("fuge" if name in fuge else "kante"),
                          n=r.get("n"), von=r.get("von"), rest=r.get("rest"),
                          staerke=r.get("staerke"), verschiebung=r.get("verschiebung"))
        ger[name] = (r["a"], r["b"]) if r["ok"] else (a, b)
    neu = [schnitt(ger["links"], ger["oben"]), schnitt(ger["oben"], ger["rechts"]),
           schnitt(ger["rechts"], ger["unten"]), schnitt(ger["unten"], ger["links"])]
    return neu, info


# ----------------------------------------------------------------------------
# Rueckprojektion
# ----------------------------------------------------------------------------
def rechteck_welt(p, yb):
    x0, z0, th, W, Hh = p
    t = np.array([math.cos(th), 0.0, math.sin(th)])
    BL = np.array([x0, yb, z0])
    BR = BL + W * t
    TL = BL + np.array([0.0, -Hh, 0.0])
    TR = BR + np.array([0.0, -Hh, 0.0])
    return [TL, TR, BR, BL]


def _proj(cam, pts):
    aus = []
    for w in pts:
        sx, sy, vz = cam.bild(np.asarray(w, float))
        aus.extend([float(sx), float(sy)])
    return np.array(aus)


def verfahren_a(cam, ecken, yb):
    """Blatt = Rechteck in einer senkrechten Ebene, Unterkante auf dem Boden y = yb.
    -> dict mit W, H (Raumwelt), Ebene, Restfehler in Pixeln, Direktwerten."""
    from scipy.optimize import least_squares
    TL, TR, BR, BL = [np.asarray(e, float) for e in ecken]
    b_l = cam.auf_ebene(BL[0], BL[1], (0, 1, 0), yb)
    b_r = cam.auf_ebene(BR[0], BR[1], (0, 1, 0), yb)
    if b_l is None or b_r is None:
        return None
    PBL, PBR = b_l[0], b_r[0]
    d = PBR - PBL
    Wd = float(np.hypot(d[0], d[2]))
    th = math.atan2(d[2], d[0])
    nrm = np.array([-math.sin(th), 0.0, math.cos(th)])
    c = float(nrm.dot(PBL))
    t_l = cam.auf_ebene(TL[0], TL[1], nrm, c)
    t_r = cam.auf_ebene(TR[0], TR[1], nrm, c)
    HL = float(yb - t_l[0][1])
    HR = float(yb - t_r[0][1])
    tv = np.array([math.cos(th), 0.0, math.sin(th)])
    lot_l = float((t_l[0] - PBL).dot(tv))       # seitlicher Versatz Oberkante gegen Unterkante
    lot_r = float((t_r[0] - PBR).dot(tv))
    ziel = np.array([TL[0], TL[1], TR[0], TR[1], BR[0], BR[1], BL[0], BL[1]])

    def f(p):
        return _proj(cam, rechteck_welt(p, yb)) - ziel
    r = least_squares(f, [PBL[0], PBL[2], th, Wd, 0.5 * (HL + HR)], x_scale=[1e3, 1e3, 0.1, 1e3, 1e3])
    p = r.x
    rms = float(np.sqrt(np.mean(r.fun ** 2)))
    return dict(W=float(p[3]), H=float(p[4]), theta_grad=float(np.degrees(p[2])),
                x0=float(p[0]), z0=float(p[1]), rms_px=rms, p=[float(v) for v in p],
                direkt=dict(W=Wd, H_links=HL, H_rechts=HR, lot_links=lot_l, lot_rechts=lot_r,
                            BL=[float(v) for v in PBL], BR=[float(v) for v in PBR],
                            vz_BL=float(b_l[1]), vz_BR=float(b_r[1])))


def verfahren_a_achse(cam, ecken, yb, start):
    """Wie Verfahren A, aber die Blattrichtung ist auf die naechste Achse festgelegt
    (Tuer-Rechtecke des Opcodes 0x3B sind achsparallel). 4 Groessen an 8 Koordinaten."""
    from scipy.optimize import least_squares
    th0 = start[2]
    th = round(th0 / (math.pi / 2)) * (math.pi / 2)
    ziel = np.array([c for e in ecken for c in (float(e[0]), float(e[1]))])

    def f(p):
        return _proj(cam, rechteck_welt([p[0], p[1], th, p[2], p[3]], yb)) - ziel
    r = least_squares(f, [start[0], start[1], start[3], start[4]], x_scale=[1e3, 1e3, 1e3, 1e3])
    p = r.x
    return dict(W=float(p[2]), H=float(p[3]), theta_grad=float(np.degrees(th)),
                abweichung_grad=float(np.degrees(th0 - th)), x0=float(p[0]), z0=float(p[1]),
                rms_px=float(np.sqrt(np.mean(r.fun ** 2))),
                p=[float(p[0]), float(p[1]), float(th), float(p[2]), float(p[3])])


def verfahren_b(cam, ecken, achse, c):
    """Wandebene achsparallel: achse 'x' (Ebene x = c) oder 'z' (Ebene z = c).
    Alle vier Ecken auf die Ebene; Unterkante frei."""
    nrm = (1, 0, 0) if achse == "x" else (0, 0, 1)
    P = []
    for e in ecken:
        r = cam.auf_ebene(e[0], e[1], nrm, c)
        if r is None:
            return None
        P.append(r[0])
    TL, TR, BR, BL = P
    k = 2 if achse == "x" else 0            # Laufrichtung der Wand
    W_u = abs(float(BR[k] - BL[k]))
    W_o = abs(float(TR[k] - TL[k]))
    H_l = float(BL[1] - TL[1])
    H_r = float(BR[1] - TR[1])
    return dict(achse=achse, c=float(c), W_unten=W_u, W_oben=W_o, H_links=H_l, H_rechts=H_r,
                W=0.5 * (W_u + W_o), H=0.5 * (H_l + H_r),
                y_unten_links=float(BL[1]), y_unten_rechts=float(BR[1]))


def streuung(cam, ecken, yb, sigma=1.0):
    """Fehlerfortpflanzung: jede der 8 Eckkoordinaten unabhaengig mit sigma Pixel.
    sigma = 1 Pixel: so weit liegen die beiden Flanken einer Fuge hoechstens auseinander."""
    e0 = np.array(ecken, float)
    basis = verfahren_a(cam, e0, yb)
    gW = gH = 0.0
    for i in range(4):
        for j in range(2):
            e = e0.copy()
            e[i, j] += 0.5
            r = verfahren_a(cam, e, yb)
            gW += ((r["W"] - basis["W"]) / 0.5) ** 2
            gH += ((r["H"] - basis["H"]) / 0.5) ** 2
    return sigma * math.sqrt(gW), sigma * math.sqrt(gH)


# ----------------------------------------------------------------------------
# Kontrollbilder
# ----------------------------------------------------------------------------
def _linie_welt(dr, cam, p, q, farbe, f, teile=24):
    """Weltstrecke als Polygonzug ins f-fach vergroesserte Vollbild (hinter der Kamera: aus)."""
    vor = None
    for i in range(teile + 1):
        w = np.asarray(p, float) + (np.asarray(q, float) - np.asarray(p, float)) * i / teile
        sx, sy, vz = cam.bild(w)
        if vz < 200:
            vor = None
            continue
        jetzt = (float(sx) * f, float(sy) * f)
        if vor is not None and max(abs(jetzt[0]), abs(jetzt[1]), abs(vor[0]), abs(vor[1])) < 4000:
            dr.line([vor, jetzt], fill=farbe)
        vor = jetzt


def bild_ecken(bg, ecken, hand, ziel, halb=13, f=10):
    """Die vier Blattecken 10-fach (NEAREST, nur Spreizung, kein Gamma): TL TR / BL BR.
    Rot = verfeinerte Kanten, gelbes Kreuz = Naeherung."""
    from PIL import Image, ImageDraw
    n = 2 * halb
    blatt = Image.new("RGB", (2 * n * f + 6, 2 * n * f + 6), (0, 0, 0))
    ordnung = [(0, 0, 0), (1, 1, 0), (3, 0, 1), (2, 1, 1)]     # (Ecke, Spalte, Zeile)
    H, W = bg.shape[:2]
    for (i, sp, ze) in ordnung:
        cx, cy = int(round(ecken[i][0])), int(round(ecken[i][1]))
        x0, y0 = cx - halb, cy - halb
        teil = np.zeros((n, n, 3), np.uint8)
        xa, xb = max(0, x0), min(W, x0 + n)
        ya, yb2 = max(0, y0), min(H, y0 + n)
        if xa < xb and ya < yb2:
            teil[ya - y0:yb2 - y0, xa - x0:xb - x0] = bg[ya:yb2, xa:xb]
        t = teil.astype(float)
        lo, hi = np.percentile(t, 1), np.percentile(t, 99.5)
        t = np.clip((t - lo) / max(1.0, hi - lo), 0, 1)
        im = Image.fromarray((t * 255).astype(np.uint8)).resize((n * f, n * f), Image.NEAREST)
        dr = ImageDraw.Draw(im)

        def P(p):
            return ((p[0] - x0) * f, (p[1] - y0) * f)
        for k in range(4):
            dr.line([P(ecken[k]), P(ecken[(k + 1) % 4])], fill=(255, 40, 40))
        q = P(hand[i])
        dr.line([(q[0] - 5, q[1]), (q[0] + 5, q[1])], fill=(255, 255, 0))
        dr.line([(q[0], q[1] - 5), (q[0], q[1] + 5)], fill=(255, 255, 0))
        for xx in range(x0, x0 + n + 1):
            if xx % 5 == 0:
                dr.text(((xx - x0) * f + 1, 1), "%d" % xx, fill=(0, 255, 255))
                dr.line([((xx - x0) * f, 0), ((xx - x0) * f, 8)], fill=(0, 255, 255))
        for yy in range(y0, y0 + n + 1):
            if yy % 5 == 0:
                dr.text((1, (yy - y0) * f + 1), "%d" % yy, fill=(0, 255, 255))
                dr.line([(0, (yy - y0) * f), (8, (yy - y0) * f)], fill=(0, 255, 255))
        blatt.paste(im, (sp * (n * f + 6), ze * (n * f + 6)))
    blatt.save(ziel)
    return ziel


def bild_boden(raum, cut, yb, rect, ziel, f=3, blatt=None, extra=None, radius=9000):
    """Vollbild f-fach: AOT-Rechteck (gruen) und SCA-Umrisse (blau, Form 0 kraeftig) auf
    Bodenhoehe yb; Blattrechteck (rot) aus Verfahren A. Dient der Pruefung der Kamera am Bild."""
    from PIL import Image, ImageDraw
    cam = raum.kamera(cut)
    bg = raum.hintergrund(cut)
    im = Image.fromarray(bg).resize((bg.shape[1] * f, bg.shape[0] * f), Image.NEAREST)
    dr = ImageDraw.Draw(im)
    x, z, w, dp = rect
    mitte = np.array([x + w / 2.0, z + dp / 2.0])
    for c in raum.sca():
        cx = min(max(mitte[0], c["x"]), c["x"] + c["w"])
        cz = min(max(mitte[1], c["z"]), c["z"] + c["d"])
        if math.hypot(cx - mitte[0], cz - mitte[1]) > radius:
            continue
        farbe = (80, 160, 255) if c["form"] == 0 else (60, 90, 150)
        e = [(c["x"], c["z"]), (c["x"] + c["w"], c["z"]), (c["x"] + c["w"], c["z"] + c["d"]),
             (c["x"], c["z"] + c["d"])]
        for i in range(4):
            _linie_welt(dr, cam, (e[i][0], yb, e[i][1]), (e[(i + 1) % 4][0], yb, e[(i + 1) % 4][1]),
                        farbe, f)
    e = [(x, z), (x + w, z), (x + w, z + dp), (x, z + dp)]
    for i in range(4):
        _linie_welt(dr, cam, (e[i][0], yb, e[i][1]), (e[(i + 1) % 4][0], yb, e[(i + 1) % 4][1]),
                    (0, 255, 0), f)
    if blatt is not None:
        for i in range(4):
            _linie_welt(dr, cam, blatt[i], blatt[(i + 1) % 4], (255, 60, 60), f, teile=4)
    for (p, q, farbe) in (extra or []):
        _linie_welt(dr, cam, p, q, farbe, f)
    im.save(ziel)
    return ziel


# ----------------------------------------------------------------------------
# Gegentuer: die Tuer im Nachbarraum, die HIER ankommt -> bestaetigt die Bodenhoehe
# ----------------------------------------------------------------------------
_ALLE = {}


def alle_tueren(spiel):
    if spiel in _ALLE:
        return _ALLE[spiel]
    aus = []
    if spiel == "re2":
        pfade = sorted(glob.glob(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM*.RDT")))
    else:
        pfade = sorted(glob.glob(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE*",
                                              "ROOM*.RDT")))
    for p in pfade:
        nm = os.path.basename(p)[4:8]
        if nm[3] != "0" or nm[0] not in "1234567":
            continue
        try:
            r = Raum(spiel, nm)
            for t in r.tueren():
                aus.append((nm, t))
        except Exception:                       # unvollstaendige Raeume (RE1.5) ueberspringen
            continue
    _ALLE[spiel] = aus
    return aus


def gegentueren(spiel, raum, rect, radius=4000):
    x, z, w, dp = rect
    mx, mz = x + w / 2.0, z + dp / 2.0
    aus = []
    for nm, t in alle_tueren(spiel):
        if "%X%02X" % (t["ziel_stage"] + 1, t["ziel_raum"]) != raum[:3].upper():
            continue
        e = math.hypot(t["ziel"][0] - mx, t["ziel"][2] - mz)
        if e <= radius:
            aus.append(dict(aus_raum=nm, satz="0x%05X" % t["pc"], ziel=list(t["ziel"]),
                            ziel_band=t["ziel_band"], abstand=round(e)))
    return aus


# ----------------------------------------------------------------------------
# Messtabelle. ecken = [TL, TR, BR, BL] = Naeherung des BLATTS (ohne Zarge), am
# vergroesserten Bild (Befehl 'lupe') abgelesen; die Messwerte entstehen erst durch
# kante_verfeinern. guete: A = vier klare Kanten, B = mindestens eine Kante unsicher.
# ----------------------------------------------------------------------------
TUEREN = [
    # ---------------- RE2 Retail ----------------
    dict(spiel="re2", raum="60B0", cut=4, rect=(-6011, -26269, 1000, 2100),
         ecken=[(126, 31), (186, 31.5), (182, 148.5), (130.5, 152)], guete="A",
         bem="Holzblatt in breiter dunkler Zarge, frontal"),
    dict(spiel="re2", raum="3060", cut=0, rect=(-17187, -28473, 1720, 1530),
         ecken=[(120.5, 40.3), (166.5, 40.3), (166.5, 143.8), (125.3, 141)], guete="A",
         fuge=("links", "rechts", "oben"),
         bem="Stahlblatt in heller Zarge, frontal"),
    dict(spiel="re2", raum="5050", cut=3, rect=(-11806, -26045, 1300, 1500),
         ecken=[(79.5, 31), (133.5, 36.5), (131.5, 167), (80, 170)], guete="A",
         bem="Stahlblatt mit runden Ecken (Bahnwagen)"),
    dict(spiel="re2", raum="20E0", cut=3, rect=None,
         ecken=[(93, 75.5), (133, 76), (134.5, 162.5), (96.5, 164)], guete="A",
         bem="Zellentuer mit Gitterfenster"),
    dict(spiel="re2", raum="2040", cut=0, rect=(4967, -17787, 1930, 2270),
         ecken=[(214.5, 10), (280.5, 13), (258.5, 123), (206.5, 121)], guete="B",
         fuge=("links", "rechts", "oben"),
         bem="Sechsfelder-Holztuer, Kamera hoch; Unterkante im Schatten"),
    dict(spiel="re2", raum="40A0", cut=8, rect=(-8522, -2619, 1800, 3400),
         ecken=[(116, 14), (170, 14), (167, 113), (119, 113)], guete="B",
         bem="Zellentuer mit Gitterfenster; linke Kante und Unterkante schwach"),
    dict(spiel="re2", raum="5050", cut=0, rect=(-11806, -26045, 1300, 1500),
         ecken=[(95.5, 62.5), (129, 63.5), (131, 145.5), (99, 149.5)], guete="C",
         bem="Wiederholprobe: dieselbe Tuer wie 5050 c3 aus einer zweiten Kamera"),
    dict(spiel="re2", raum="21A0", cut=0, rect=None,
         ecken=[(175, 44), (231, 44), (226.5, 134), (177, 134)], guete="C",
         bem="Stahltuer, mehrere Zargenlinien: Blattgrenze nicht eindeutig"),
    dict(spiel="re2", raum="20C0", cut=0, rect=(-11225, -28122, 2100, 1800),
         ecken=[(118, 102), (162.5, 104.5), (162.5, 201), (112.5, 201)], guete="C",
         fuge=("links", "oben"),
         bem="Sechsfelder-Holztuer frontal; Kamera nur 486 ueber dem Boden"),
    dict(spiel="re2", raum="6140", cut=0, rect=(2110, -16072, 1800, 2400),
         ecken=[(141.5, 85), (193, 85), (192, 207), (142.5, 207)], guete="C",
         bem="Metalltuer mit Seitenstreifen; Kamera nur 450 ueber dem Boden"),
    dict(spiel="re2", raum="2020", cut=0, rect=(-6075, -11550, 1800, 2200),
         ecken=[(137, 88.5), (178.5, 81.5), (177.5, 196), (137, 197)], guete="C",
         bem="Holztuer, schraeg; Kamera nur 522 ueber dem Boden"),
    # ---------------- RE1.5 ----------------
    dict(spiel="re15", raum="1000", cut=3, rect=(1200, -900, 1000, 2000),
         ecken=[(135.5, 57.5), (195.5, 51.5), (196.5, 172), (135, 171.5)], guete="A",
         fuge=("rechts", "oben"),
         bem="glattes Blatt in heller Zarge, frontal"),
    dict(spiel="re15", raum="1000", cut=6, rect=(1200, 2200, 1000, 2000),
         ecken=[(153.5, 36), (227.5, 36), (220.5, 153), (154.5, 153)], guete="A",
         fuge=("links", "rechts", "oben"),
         bem="glattes Blatt in heller Zarge, frontal (zweite Tuer des Raums)"),
    dict(spiel="re15", raum="1010", cut=0, rect=(4150, 5900, 1000, 2000),
         ecken=[(62.5, 55), (131, 55), (131.5, 163.5), (70.5, 164.5)], guete="A",
         fuge=("links", "rechts", "oben"),
         bem="glattes Blatt, dunkle Fuge ringsum"),
    dict(spiel="re15", raum="1010", cut=4, rect=(4150, -4950, 1000, 2000),
         ecken=[(166.5, 49), (238.5, 49), (229.5, 161), (165.5, 156)], guete="A",
         fuge=("links", "rechts", "oben"),
         bem="glattes Blatt, dunkle Fuge ringsum (zweite Tuer des Raums)"),
    dict(spiel="re15", raum="2060", cut=0, rect=(7950, 2950, 1000, 2000),
         ecken=[(140.5, 66), (186.5, 58.5), (187.5, 176), (140.5, 174)], guete="A",
         bem="orangefarbenes Blatt in grauer Zarge"),
    dict(spiel="re15", raum="1130", cut=6, rect=(-7150, 15350, 1000, 2000),
         ecken=[(122.5, 47), (209, 49), (201.5, 180), (129.5, 166)], guete="C",
         bem="Holztuer 'Chief Office': gemessen ist der AUSSENRAND der Zarge, nicht das Blatt"),
    dict(spiel="re15", raum="1140", cut=6, rect=(-8250, -750, 2000, 1000),
         ecken=[(141.5, 72.5), (190, 72.5), (190, 184), (141.5, 184)], guete="C",
         bem="Stahltuer frontal; Kamera nur 838 ueber dem Boden, Unterkante dunkel"),
    dict(spiel="re15", raum="3090", cut=0, rect=None,
         ecken=[(120.5, 31.5), (177.5, 31.5), (178, 126.5), (131, 128)], guete="C",
         bem="helle Tuer an der Stirnwand: Viereck passt nicht zu einem Rechteck in der Wand"),
]


def _wandprobe(raum, satz, a):
    """Achsparallele Wandlinien in der Naehe der Blattebene aus Verfahren A."""
    th = math.radians(a["theta_grad"])
    cx, sz = abs(math.cos(th)), abs(math.sin(th))
    if max(cx, sz) < math.cos(math.radians(15)):
        return dict(achse=None, grund="Blatt steht %.1f Grad schraeg zu den Achsen" %
                    math.degrees(math.acos(max(cx, sz))))
    achse = "z" if cx > sz else "x"            # Ebene z = c bzw. x = c
    BL, BR = np.array(a["direkt"]["BL"]), np.array(a["direkt"]["BR"])
    k, l = (2, 0) if achse == "z" else (0, 2)  # k = Koordinate der Ebene, l = Laufrichtung
    cA = 0.5 * (BL[k] + BR[k])
    lo, hi = sorted((BL[l], BR[l]))
    x, z, w, dp = satz["rect"]
    aot = [z, z + dp] if achse == "z" else [x, x + w]
    sca = []
    for c in raum.sca():
        c0, c1 = (c["z"], c["z"] + c["d"]) if achse == "z" else (c["x"], c["x"] + c["w"])
        l0, l1 = (c["x"], c["x"] + c["w"]) if achse == "z" else (c["z"], c["z"] + c["d"])
        if l1 < lo - 300 or l0 > hi + 300:
            continue
        for v in (c0, c1):
            sca.append((abs(v - cA), v, c["off"], c["form"]))
    sca.sort()
    aot_n = min(aot, key=lambda v: abs(v - cA))
    aus = dict(achse=achse, ebene_A=float(cA), aot_kanten=aot, aot_naechste=aot_n,
               aot_abstand=float(aot_n - cA))
    if sca:
        aus.update(sca_naechste=sca[0][1], sca_abstand=float(sca[0][1] - cA),
                   sca_zelle="0x%X" % sca[0][2], sca_form=sca[0][3])
    return aus


def messe(t, bilder=True):
    from PIL import Image, ImageDraw
    r = Raum(t["spiel"], t["raum"])
    if t.get("rect") is None:
        # Tuer ueber die Naehe zum Bodenpunkt unter der Blattmitte waehlen (Naeherung)
        camx = r.kamera(t["cut"])
        um = np.mean([t["ecken"][2], t["ecken"][3]], axis=0)
        best = None
        for s in r.tueren():
            w = camx.auf_ebene(um[0], um[1], (0, 1, 0), -s["band"] * BAND)
            if w is None:
                continue
            x, z, bw, bd = s["rect"]
            e = math.hypot(w[0][0] - (x + bw / 2.0), w[0][2] - (z + bd / 2.0))
            if best is None or e < best[0]:
                best = (e, s)
        satz = best[1]
    else:
        satz = [s for s in r.tueren() if tuple(s["rect"]) == tuple(t["rect"])][0]
    yb = float(t.get("boden", -satz["band"] * BAND))
    cam = r.kamera(t["cut"], exakt=True)
    camp = r.kamera(t["cut"], exakt=False)
    bg = r.hintergrund(t["cut"])
    lum = helligkeit(bg.astype(float))
    hand = [np.asarray(e, float) for e in t["ecken"]]
    ecken, info = ecken_verfeinern(lum, hand, fest=t.get("fest", ()), fuge=t.get("fuge", ()))
    a = verfahren_a(cam, ecken, yb)
    ap = verfahren_a(camp, ecken, yb)
    ah = verfahren_a(cam, hand, yb)
    sW, sH = streuung(cam, ecken, yb)
    aa = verfahren_a_achse(cam, ecken, yb, a["p"])
    wand = _wandprobe(r, satz, a)
    b_aot = b_sca = None
    if wand.get("achse"):
        b_aot = verfahren_b(cam, ecken, wand["achse"], wand["aot_naechste"])
        if "sca_naechste" in wand:
            b_sca = verfahren_b(cam, ecken, wand["achse"], wand["sca_naechste"])
    px_h = 0.5 * (np.linalg.norm(ecken[3] - ecken[0]) + np.linalg.norm(ecken[2] - ecken[1]))
    px_b = 0.5 * (np.linalg.norm(ecken[1] - ecken[0]) + np.linalg.norm(ecken[2] - ecken[3]))
    erg = dict(
        spiel=t["spiel"], raum="ROOM" + t["raum"], cut=t["cut"], guete=t["guete"], bem=t["bem"],
        rdt=rel(r.pfad), hintergrund=rel(r.bg % t["cut"]),
        satz="0x%05X" % satz["pc"], satz_bytes=satz["roh"], archiv="%02X" % satz["archiv"],
        variante=satz["variante"], rect=list(satz["rect"]), band=satz["band"], boden_y=yb,
        kamera=dict(off="0x%X" % cam.off, bytes=cam.roh.hex(" "), fov=cam.fov, H=cam.H,
                    ort=list(cam.pos), ziel=list(cam.ziel), hoehe_ueber_boden=yb - cam.pos[1]),
        ecken_hand=[[float(v) for v in e] for e in hand],
        ecken=[[round(float(v), 2) for v in e] for e in ecken],
        kanten=info, px_hoehe=float(px_h), px_breite=float(px_b),
        A=a, A_achse=aa, A_port_matrix=dict(W=ap["W"], H=ap["H"]),
        A_handecken=dict(W=ah["W"], H=ah["H"]),
        sigma_W=sW, sigma_H=sH, wand=wand, B_aot=b_aot, B_sca=b_sca,
        gegentueren=gegentueren(t["spiel"], t["raum"], satz["rect"]),
        k_hoehe=BLATT_H / a["H"], k_breite=BLATT_B / a["W"],
        k_hoehe_sigma=BLATT_H / a["H"] * sH / a["H"], k_breite_sigma=BLATT_B / a["W"] * sW / a["W"],
        verhaeltnis_H_zu_W=a["H"] / a["W"],
    )
    if bilder:
        os.makedirs(AUS, exist_ok=True)
        stamm = os.path.join(AUS, "massstab_%s_%s_c%02d" % (t["spiel"], t["raum"], t["cut"]))
        xs = [e[0] for e in ecken]
        ys = [e[1] for e in ecken]
        x0, x1 = int(min(xs)) - 12, int(max(xs)) + 13
        y0, y1 = int(min(ys)) - 12, int(max(ys)) + 13
        rot, gelb, gruen = (255, 40, 40), (255, 255, 0), (0, 255, 0)
        linien = [(ecken[i], ecken[(i + 1) % 4], rot) for i in range(4)]
        fit = _proj(cam, rechteck_welt(a["p"], yb)).reshape(4, 2)
        linien += [(fit[i], fit[(i + 1) % 4], gruen) for i in range(4)]
        marken = [(e, gelb) for e in hand]
        cmd_lupe(t["spiel"], t["raum"], t["cut"], x0, y0, x1, y1, 5, ziel=stamm + "_blatt.png",
                 marken=marken, linien=linien, hell=True)
        bild_boden(r, t["cut"], yb, satz["rect"], stamm + "_boden.png", f=3,
                   blatt=rechteck_welt(a["p"], yb))
        bild_ecken(bg, ecken, hand, stamm + "_ecken.png")
        erg["bilder"] = [rel(stamm + "_blatt.png"), rel(stamm + "_ecken.png"),
                         rel(stamm + "_boden.png")]
    return erg


def zeile(e):
    return ("%-4s ROOM%-4s c%-2d %s  px %5.1f x %5.1f  H %6.0f +-%3.0f  W %6.0f +-%3.0f  H/W %.3f  "
            "k_H %.3f  k_W %.3f  rms %.2f px  vz %5.0f" % (
                e["spiel"], e["raum"][4:], e["cut"], e["guete"], e["px_hoehe"], e["px_breite"],
                e["A"]["H"], e["sigma_H"], e["A"]["W"], e["sigma_W"], e["verhaeltnis_H_zu_W"],
                e["k_hoehe"], e["k_breite"], e["A"]["rms_px"],
                0.5 * (e["A"]["direkt"]["vz_BL"] + e["A"]["direkt"]["vz_BR"])))


def cmd_messen(nur=None, bilder=True):
    aus = []
    for t in TUEREN:
        if nur and (t["raum"] not in nur):
            continue
        e = messe(t, bilder=bilder)
        aus.append(e)
        print(zeile(e))
    return aus


# ----------------------------------------------------------------------------
# Auswertung
# ----------------------------------------------------------------------------
def _stat(werte):
    v = np.array(werte, float)
    if len(v) == 0:
        return None
    return dict(n=int(len(v)), mittel=float(v.mean()),
                streuung=float(v.std(ddof=1)) if len(v) > 1 else 0.0,
                median=float(np.median(v)), kleinst=float(v.min()), groesst=float(v.max()),
                fehler_des_mittels=float(v.std(ddof=1) / math.sqrt(len(v))) if len(v) > 1 else 0.0)


def einstufen(e):
    """-> (wertung, grund). 'zaehlt' nur, wenn der Bodenanker traegt und das Viereck zu einem
    Rechteck in einer senkrechten Ebene passt."""
    if e["guete"] == "C":
        return "aus", e["bem"]
    if e["kamera"]["hoehe_ueber_boden"] < 1000:
        return "aus", "Kamera nur %.0f ueber dem Boden" % e["kamera"]["hoehe_ueber_boden"]
    if e["A"]["rms_px"] > 1.0:
        return "aus", "Restfehler %.2f Pixel" % e["A"]["rms_px"]
    return "zaehlt", ""


def auswerten(mess):
    gruppen = {}
    for e in mess:
        w, grund = einstufen(e)
        e["wertung"], e["ausschlussgrund"] = w, grund
        if w != "zaehlt":
            continue
        for g in (e["spiel"] + "_" + e["guete"], e["spiel"] + "_AB", "alle_" + e["guete"], "alle_AB"):
            gruppen.setdefault(g, []).append(e)
    aus = {}
    for g, liste in sorted(gruppen.items()):
        aus[g] = dict(
            tueren=["%s c%d" % (e["raum"], e["cut"]) for e in liste],
            H=_stat([e["A"]["H"] for e in liste]), W=_stat([e["A"]["W"] for e in liste]),
            H_zu_W=_stat([e["verhaeltnis_H_zu_W"] for e in liste]),
            k_hoehe=_stat([e["k_hoehe"] for e in liste]),
            k_breite=_stat([e["k_breite"] for e in liste]))
    return aus


# ----------------------------------------------------------------------------
# Gegenprobe ohne Bild: Spielerhoehe
# ----------------------------------------------------------------------------
def spielermodell(pfad):
    """PLD: Verzeichnis am Dateiende (u32 Offset @0, u32 Anzahl @4): EDD, EMR, MD1, TIM.
    Nullpose (alle Winkel 0, Wurzel im Ursprung) und Pose des ersten Bilds von Clip 0."""
    sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))
    import emd_ansichtsblatt as E      # nur die Leser (md1_parse, emr_parse, edd_parse, mat_euler)
    d = open(pfad, "rb").read()
    o, n = struct.unpack_from("<II", d, 0)
    v = struct.unpack_from("<%dI" % n, d, o)
    edd, emr, md1 = d[v[0]:v[1]], d[v[1]:v[2]], d[v[2]:v[3]]
    meshes = E.md1_parse(md1)
    sk = E.emr_parse(emr)
    clips, frames = E.edd_parse(edd)

    def ausdehnung(kf, null):
        nb = sk.bone_count
        R, T = [None] * nb, [None] * nb
        wurzel = (0, 0, 0) if null else E.kf_pos(sk, kf)
        for b in range(nb):
            loc = E.mat_euler(*((0, 0, 0) if null else E.kf_angles(sk, kf, b)))
            p = sk.parent[b]
            if p < 0 or p >= b:
                R[b], T[b] = loc, np.array(wurzel, dtype=np.int64)
            else:
                R[b] = (R[p] @ loc) >> 12
                T[b] = ((R[p] @ np.array(sk.rel[b], dtype=np.int64)) >> 12) + T[p]
        pts = []
        for b in range(min(nb, len(meshes))):
            for vs in (meshes[b].tv, meshes[b].qv):
                if vs:
                    a = np.array([[q[0], q[1], q[2]] for q in vs], float)
                    pts.append(a @ (R[b].astype(float) / 4096.0).T + T[b])
        P = np.vstack(pts)
        return float(P[:, 1].min()), float(P[:, 1].max())
    n0 = ausdehnung(0, True)
    kf0 = frames[clips[0][0]] & 0xFFF
    p0 = ausdehnung(kf0, False)
    return dict(datei=rel(pfad), verzeichnis=["0x%X" % x for x in v],
                emr_bytes_wurzel="@Datei 0x%X: %s" % (v[1] + 8, d[v[1] + 8:v[1] + 14].hex(" ")),
                wurzel_rel=list(sk.rel[0]), knochen=sk.bone_count, meshes=len(meshes),
                nullpose_y=list(n0), nullpose_hoehe=n0[1] - n0[0],
                clip0_bild0=dict(keyframe=int(kf0), wurzel=list(E.kf_pos(sk, kf0)), y=list(p0),
                                 hoehe=p0[1] - p0[0]))


def cmd_gegenprobe(k_h=None, k_w=None):
    exe = open(os.path.join(REPO, "info", "Re1.5", "PSX.EXE"), "rb").read()
    t_addr = struct.unpack_from("<I", exe, 0x18)[0]
    o = 0x800 + 0x80073E94 - t_addr
    roh = exe[o:o + 12]
    ofs = struct.unpack_from("<hhh", roh, 0)
    rmin, hh, rmax = struct.unpack_from("<HHH", roh, 6)
    aus = dict(
        trefferkasten=dict(adresse="0x80073e94", datei_offset="0x%X" % o, bytes=roh.hex(" "),
                           versatz=list(ofs), radius=[rmin, rmax], halbe_hoehe=hh,
                           y_bereich=[ofs[1] - hh, ofs[1] + hh], hoehe=2 * hh,
                           port=["re15_port/engine/src/re15_damage.c:3448-3449",
                                 "re15_port/engine/src/re15_damage.c:3460",
                                 "re15_port/include/re15_actor.h:105"]),
        re15=spielermodell(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "PLD", "PL00.PLD")),
        re2=spielermodell(os.path.join(REPO, "info", "re2leon", "PL0", "PLD", "PL00.PLD")),
        blatt=dict(hoehe=BLATT_H, breite=BLATT_B, knauf=KNAUF_H),
    )
    sp = aus["re15"]["nullpose_hoehe"]       # aufrechte Grundhaltung; Clip 0 Bild 0 ist ein Schritt
    aus["spielerhoehe"] = sp
    aus["ohne_massstab"] = dict(blatt_zu_spieler=BLATT_H / sp, knauf_zu_spieler=KNAUF_H / sp)
    if k_h:
        aus["mit_k_hoehe"] = dict(k=k_h, blatt_raum=BLATT_H / k_h, knauf_raum=KNAUF_H / k_h,
                                  blatt_zu_spieler=BLATT_H / k_h / sp,
                                  knauf_zu_spieler=KNAUF_H / k_h / sp)
    print("Trefferkasten Spieler @0x80073e94 (Datei 0x%X): %s -> y %d..%d, Hoehe %d" %
          (o, roh.hex(" "), ofs[1] - hh, ofs[1] + hh, 2 * hh))
    for g in ("re15", "re2"):
        m = aus[g]
        print("%-5s %s: Wurzel %s, Nullpose y %.0f..%.0f = %.0f, Clip 0 Bild 0 y %.0f..%.0f = %.0f" % (
            g, m["datei"], m["wurzel_rel"], m["nullpose_y"][0], m["nullpose_y"][1],
            m["nullpose_hoehe"], m["clip0_bild0"]["y"][0], m["clip0_bild0"]["y"][1],
            m["clip0_bild0"]["hoehe"]))
    print("Blatt %d / Spieler %.0f = %.3f ; Knauf %d / Spieler = %.3f (ohne Massstab)" % (
        BLATT_H, sp, BLATT_H / sp, KNAUF_H, KNAUF_H / sp))
    if k_h:
        print("mit k_H = %.3f: Blatt %.0f Raumeinheiten = %.3f Spielerhoehen, Knauf %.0f = %.3f" % (
            k_h, BLATT_H / k_h, BLATT_H / k_h / sp, KNAUF_H / k_h, KNAUF_H / k_h / sp))
    return aus


def cmd_wiederholprobe(spiel="re2", raum="5050", von=3, nach=0):
    """Dieselbe Tuer aus zwei Kameras: das aus Cut `von` gewonnene Rechteck in Cut `nach`
    projizieren und mit den dort gemessenen Ecken vergleichen."""
    ts = [t for t in TUEREN if t["spiel"] == spiel and t["raum"] == raum]
    a = messe([t for t in ts if t["cut"] == von][0], bilder=False)
    b = messe([t for t in ts if t["cut"] == nach][0], bilder=False)
    r = Raum(spiel, raum)
    soll = _proj(r.kamera(nach), rechteck_welt(a["A"]["p"], a["boden_y"])).reshape(4, 2)
    ist = np.array(b["ecken"], float)
    aus = dict(tuer="%s ROOM%s" % (spiel, raum), von=von, nach=nach,
               H=[a["A"]["H"], b["A"]["H"]], W=[a["A"]["W"], b["A"]["W"]],
               ebene=[a["wand"].get("ebene_A"), b["wand"].get("ebene_A")],
               projiziert=[[round(float(v), 2) for v in p] for p in soll],
               gemessen=[[round(float(v), 2) for v in p] for p in ist],
               differenz=[[round(float(v), 2) for v in p] for p in (ist - soll)])
    print("Tuer %s: Cut %d H %.0f W %.0f Ebene %.0f | Cut %d H %.0f W %.0f Ebene %.0f" % (
        aus["tuer"], von, a["A"]["H"], a["A"]["W"], aus["ebene"][0],
        nach, b["A"]["H"], b["A"]["W"], aus["ebene"][1]))
    for name, s, i in zip(("TL", "TR", "BR", "BL"), soll, ist):
        print("  %s aus Cut %d projiziert (%.1f, %.1f)  in Cut %d gemessen (%.1f, %.1f)  "
              "Differenz (%+.1f, %+.1f)" % (name, von, s[0], s[1], nach, i[0], i[1],
                                            i[0] - s[0], i[1] - s[1]))
    return aus


def cmd_alles():
    os.makedirs(AUS, exist_ok=True)
    zen = cmd_zensus()
    zen["boden"] = cmd_bodenzensus()
    mess = cmd_messen()
    ausw = auswerten(mess)
    print()
    for g, a in ausw.items():
        print("%-8s n=%d  H %.0f +-%.0f  W %.0f +-%.0f  H/W %.3f +-%.3f  k_H %.3f +-%.3f  k_W %.3f +-%.3f" % (
            g, a["H"]["n"], a["H"]["mittel"], a["H"]["streuung"], a["W"]["mittel"],
            a["W"]["streuung"], a["H_zu_W"]["mittel"], a["H_zu_W"]["streuung"],
            a["k_hoehe"]["mittel"], a["k_hoehe"]["streuung"],
            a["k_breite"]["mittel"], a["k_breite"]["streuung"]))
    for e in mess:
        if e["wertung"] != "zaehlt":
            print("ausgeschlossen: %s %s c%d - %s" % (e["spiel"], e["raum"], e["cut"],
                                                       e["ausschlussgrund"]))
    print()
    k_h = ausw["re15_A"]["k_hoehe"]["mittel"]
    k_w = ausw["re15_A"]["k_breite"]["mittel"]
    geg = cmd_gegenprobe(k_h, k_w)
    print()
    wdh = cmd_wiederholprobe()
    k_alle = ausw["alle_AB"]["k_hoehe"]
    tabelle = [dict(raum=r, tuerszene_k_hoehe=round(r * k_alle["mittel"]),
                    von=round(r * (k_alle["mittel"] - k_alle["streuung"])),
                    bis=round(r * (k_alle["mittel"] + k_alle["streuung"])))
               for r in (500, 1000, 1500, 2000)]
    print()
    print("k_H (alle, A+B) = %.3f +- %.3f (Streuung je Tuer), +- %.3f (Fehler des Mittels)" % (
        k_alle["mittel"], k_alle["streuung"], k_alle["fehler_des_mittels"]))
    for z in tabelle:
        print("  Raum %5d -> Tuerszene %5d  (%d .. %d)" % (z["raum"], z["tuerszene_k_hoehe"],
                                                           z["von"], z["bis"]))
    json.dump(dict(blatt=dict(x=BLATT_X, y=BLATT_Y, z=BLATT_Z, hoehe=BLATT_H, breite=BLATT_B,
                              knauf=KNAUF_H),
                   zensus=zen, tueren=mess, auswertung=ausw, gegenprobe=geg,
                   wiederholprobe=wdh, tabelle=tabelle),
              open(JSON_PFAD, "w", encoding="utf-8"), indent=1, default=float)
    print(rel(JSON_PFAD))


if __name__ == "__main__":
    a = sys.argv[1:]
    if not a:
        print(__doc__)
        sys.exit(0)
    if a[0] == "zensus":
        cmd_zensus()
    elif a[0] == "kandidaten":
        cmd_kandidaten()
    elif a[0] == "bodenzensus":
        cmd_bodenzensus()
    elif a[0] == "messen":
        cmd_messen(nur=a[1:] or None)
    elif a[0] == "gegenprobe":
        cmd_gegenprobe(float(a[1]) if len(a) > 1 else None)
    elif a[0] == "wiederholprobe":
        cmd_wiederholprobe()
    elif a[0] == "alles":
        cmd_alles()
    elif a[0] == "lupe":
        cmd_lupe(a[1], a[2], int(a[3]), int(a[4]), int(a[5]), int(a[6]), int(a[7]),
                 int(a[8]) if len(a) > 8 else 6, hell=(len(a) > 9 and a[9] == "hell"))
    else:
        print(__doc__)
        sys.exit(2)
