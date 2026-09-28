#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_tuer_hintergrund.py - die RE2-Tuer im vorgerenderten Hintergrund zeigen, beide Seiten eines Paares
nebeneinander, beschriftet mit Archiv und Variante. Zum ANSEHEN: steht der gemalte Griff links oder rechts,
und passt das zur Variante (V0: Griff links / Angel rechts, V1: Griff rechts / Angel links - Abschnitt 1)?

Runde 31, Teil T2 (analysis/befunde_runde31/tueren_02_re2.md, Abschnitt 2).
Liest NUR: build/r31_tueren/t2/re2_tueren.json (re2_tuer_zensus.py), info/re2leon/PL0/RDT/ROOM*.RDT,
           info/re2leon/COMMON/BSS/ROOMsrr/ROOMsrrNN.bmp.
Schreibt NUR: build/r31_tueren/t2/hg/.

Kameramodell 1:1 wie tools/re2_sicherung/re2_aot_on_bg.py (dort gegen re15_port/engine/src/camera_common.c
bzw. FUN_80053ca4 belegt; RE2 benutzt dieselbe Blickmatrix-Routine FUN_80076cb0):
  RID-Satz 32 B ab RDT-Kopfwort 0x24: +0 flag u16, +2 fov u16, +4 Auge xyz s32, +0x10 Ziel xyz s32, +0x1C pri
  H = fov >> 7; Bildpunkt 160 + H*x/z, 120 + H*y/z.  Anzahl Kameras = RDT-Byte 1.
Die Bodenhoehe der Tuer = y der Zielposition der GEGENSEITE (dort erscheint der Spieler in diesem Raum).

Aufruf:
  python re15_port/tools/tueren/re2_tuer_hintergrund.py [--alle] [--paar N ...]
"""
import argparse
import json
import math
import os
import struct
import sys

from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
T2 = os.path.join(REPO, "build", "r31_tueren", "t2")
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")
BSS = os.path.join(REPO, "info", "re2leon", "COMMON", "BSS")
TUER_HOEHE = 2600   # nur Zeichenhilfe (Kasten ueber dem AOT-Rechteck), keine Spielgroesse


def isqrt(x):
    return int(math.isqrt(max(0, int(x))))


def build_view(cut):
    flag, fov = cut[0], cut[1]
    px, py, pz, tx, ty, tz = cut[2:8]
    dx, dy, dz = tx - px, ty - py, tz - pz
    dist = isqrt(dx * dx + dy * dy + dz * dz)
    if dist == 0:
        return None
    horiz = isqrt(dx * dx + dz * dz)
    sp = int((-dy) * 4096 / dist)
    cp = int(horiz * 4096 / dist)
    if horiz:
        sy = int(dx * 4096 / horiz)
        cy = int(dz * 4096 / horiz)
        r = [cy, 0, -sy, (sp * sy) >> 12, cp, (sp * cy) >> 12, (cp * sy) >> 12, -sp, (cp * cy) >> 12]
    else:
        r = [4096, 0, 0, 0, cp, sp, 0, -sp, cp]
    t = [(r[0] * -px + r[1] * -py + r[2] * -pz) >> 12, (r[3] * -px + r[4] * -py + r[5] * -pz) >> 12,
         (r[6] * -px + r[7] * -py + r[8] * -pz) >> 12]
    return r, t, fov >> 7


def project(view, x, y, z):
    r, t, H = view
    vx = ((r[0] * x + r[1] * y + r[2] * z) >> 12) + t[0]
    vy = ((r[3] * x + r[4] * y + r[5] * z) >> 12) + t[1]
    vz = ((r[6] * x + r[7] * y + r[8] * z) >> 12) + t[2]
    if vz <= 1:
        return None
    return (160 + H * vx / vz, 120 + H * vy / vz, vz)


def kameras(datei):
    d = open(os.path.join(RDT, datei), "rb").read()
    n = d[1]
    off = struct.unpack_from("<I", d, 0x24)[0]
    return [struct.unpack_from("<HHiiiiiiI", d, off + i * 32) for i in range(n)]


def bss_bild(datei, cam):
    stufe = datei[4]
    if not stufe.isdigit():
        stufe = str(ord(stufe) - ord("A") + 1)
    raum = datei[5:7]
    p = os.path.join(BSS, "ROOM%s%s" % (stufe, raum), "ROOM%s%s%02d.bmp" % (stufe, raum, cam))
    return p if os.path.isfile(p) else None


def beste_kamera(seite, boden_y, blick=None):
    """blick = Blickrichtung des Spielers, der durch die Gegenseite hereinkommt (weg von der Tuer):
    (cos d, -sin d) wie der Laeufer FUN_800245d8 (re15_port/engine/src/actor_locomotion.c:340-344).
    Bevorzugt werden Kameras, die GEGEN diese Richtung, also auf die Tuer, blicken."""
    cams = kameras(seite["datei"])
    best = None
    for i, c in enumerate(cams):
        v = build_view(c)
        if v is None:
            continue
        lo = [project(v, x, boden_y, z) for x, z in seite["punkte"]]
        hi = [project(v, x, boden_y - TUER_HOEHE, z) for x, z in seite["punkte"]]
        if any(p is None for p in lo + hi):
            continue
        xs = [p[0] for p in lo + hi]
        ys = [p[1] for p in lo + hi]
        cx0, cx1 = max(min(xs), 0), min(max(xs), 320)
        cy0, cy1 = max(min(ys), 0), min(max(ys), 240)
        if cx1 <= cx0 or cy1 <= cy0:
            continue
        sichtbar = (cx1 - cx0) * (cy1 - cy0)
        ganz = (max(xs) - min(xs)) * (max(ys) - min(ys))
        anteil = sichtbar / ganz if ganz else 0
        sc = sichtbar * (0.3 + anteil)
        if blick is not None:
            vx, vz = c[5] - c[2], c[7] - c[4]
            n = math.hypot(vx, vz) or 1.0
            gegen = -(vx * blick[0] + vz * blick[1]) / n
            sc *= 0.15 + max(0.0, gegen)
        if best is None or sc > best[0]:
            best = (sc, i, lo, hi)
    return best


def bild_seite(seite, boden_y, text, blick=None):
    b = beste_kamera(seite, boden_y, blick)
    if b is None:
        return None, None
    _, cam, lo, hi = b
    p = bss_bild(seite["datei"], cam)
    if p is None:
        return None, cam
    im = Image.open(p).convert("RGB")
    # Ausschnitt: Kasten ueber dem Rechteck mit Rand, auf 360 Zeilen vergroessert (zum Ansehen des Griffs)
    xs = [q[0] for q in lo + hi]
    ys = [q[1] for q in lo + hi]
    x0, x1 = max(int(min(xs)) - 30, 0), min(int(max(xs)) + 30, 320)
    y0, y1 = max(int(min(ys)) - 40, 0), min(int(max(ys)) + 10, 240)
    aus = None
    if x1 - x0 > 4 and y1 - y0 > 4:
        aus = im.crop((x0, y0, x1, y1))
        k = 360.0 / aus.height
        aus = aus.resize((max(1, int(aus.width * k)), 360), Image.LANCZOS)
        if aus.width > 640:
            aus = aus.resize((640, int(360 * 640 / aus.width)), Image.LANCZOS)
    S = 2
    im = im.resize((320 * S, 240 * S), Image.NEAREST)
    dr = ImageDraw.Draw(im)
    dr.polygon([(q[0] * S, q[1] * S) for q in lo], outline=(255, 220, 60))
    for a, c in zip(lo, hi):
        dr.line([(a[0] * S, a[1] * S), (c[0] * S, c[1] * S)], fill=(255, 150, 60))
    dr.rectangle([0, 0, 640, 14], fill=(0, 0, 0))
    dr.text((4, 1), "%s cam%02d  %s" % (seite["datei"], cam, text), fill=(255, 255, 0))
    if aus is not None:
        ges = Image.new("RGB", (640, 480 + 364), (30, 30, 30))
        ges.paste(im, (0, 0))
        ges.paste(aus, (0, 484))
        return ges, cam
    return im, cam


# [BILD] selbst angesehene Stichprobe (analysis/befunde_runde31/tueren_02_re2.md 2.3): Paar-Nr (index.json), Seite
# ("a"/"b"), Kamera, gesehener Griff (vor der Tuer stehend) und Bemerkung.
STICHPROBE = [
    (91, "a", 6, "rechts", "frontal"), (54, "b", 0, "links", "frontal"), (54, "a", 4, "rechts", "rechte Wand, nahe Kante"),
    (154, "a", 8, "links", "frontal"), (154, "b", 2, "rechts", "von oben"), (159, "b", 0, "rechts", "frontal"),
    (26, "b", 0, "links", "frontal"), (87, "a", 4, "rechts", "leicht schraeg"), (138, "b", 0, "links", "frontal"),
    (185, "a", 3, "rechts", "frontal"), (185, "b", 0, "links", "frontal"), (117, "a", 9, "links", "frontal"),
    (50, "a", 5, "links", "linke Wand, nahe Kante"), (50, "b", 0, "rechts", "frontal"), (204, "a", 8, "links", "frontal"),
    (204, "b", 5, "rechts", "frontal"), (22, "b", 4, "rechts", "frontal, Druecker (Bit 7)"),
    (213, "a", 6, "Handrad links", "Schott zweiteilig"), (213, "b", 0, "Handrad links", "Schott zweiteilig"),
    (181, "a", 9, "Angel links", "Schott einteilig"), (134, "a", 0, "links", "schraeg"), (134, "b", 2, "rechts", "frontal"),
    (175, "a", 3, "links", "frontal"), (175, "b", 3, "rechts", "frontal"),
    (22, "a", 0, "rechts?", "linke Wand, sehr flach: Griff an der FERNEN Kante"),
]


def stichprobe(J, out):
    S = J["seiten"]
    kacheln = []
    for (k, ab, cam, gesehen, bem) in STICHPROBE:
        p = J["paare"][k]
        A, B = S[p["a"]], S[p["b"]]
        seite, gegen = (A, B) if ab == "a" else (B, A)
        v = seite["archiv_varianten"]
        boden = gegen["ziel_pos"][1]
        c = kameras(seite["datei"])[cam]
        vw = build_view(c)
        lo = [project(vw, x, boden, z) for x, z in seite["punkte"]]
        hi = [project(vw, x, boden - TUER_HOEHE, z) for x, z in seite["punkte"]]
        xs = [q[0] for q in lo + hi]
        ys = [q[1] for q in lo + hi]
        x0, x1 = max(int(min(xs)) - 30, 0), min(int(max(xs)) + 30, 320)
        y0, y1 = max(int(min(ys)) - 40, 0), min(int(max(ys)) + 10, 240)
        im = Image.open(bss_bild(seite["datei"], cam)).convert("RGB").crop((x0, y0, x1, y1))
        f = 150.0 / im.height
        im = im.resize((max(1, int(im.width * f)), 150), Image.LANCZOS)
        if im.width > 200:
            im = im.resize((200, int(150 * 200 / im.width)), Image.LANCZOS)
        t = Image.new("RGB", (200, 190), (24, 24, 28))
        t.paste(im, ((200 - im.width) // 2, 0))
        dr = ImageDraw.Draw(t)
        dr.text((2, 152), "%s c%d DOOR%02X V%s" % (seite["datei"][4:8], cam, v[0][0], "/".join(
            "%d%s" % (x[1], "b7" if x[2] else "") for x in v)), fill=(255, 255, 0))
        dr.text((2, 164), "Griff: %s" % gesehen, fill=(120, 255, 120) if "?" not in gesehen else (255, 120, 120))
        dr.text((2, 176), bem[:34], fill=(200, 200, 200))
        kacheln.append(t)
    cols = 6
    rows = (len(kacheln) + cols - 1) // cols
    sh = Image.new("RGB", (cols * 202, rows * 192), (40, 40, 40))
    for i, t in enumerate(kacheln):
        sh.paste(t, ((i % cols) * 202, (i // cols) * 192))
    ziel = os.path.join(REPO, "analysis", "befunde_runde31", "tueren_belege", "t2_stichprobe_griff.jpg")
    sh.save(ziel, quality=82, optimize=True)
    print("Stichprobe:", ziel, sh.size)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--alle", action="store_true")
    ap.add_argument("--paar", type=int, nargs="*")
    ap.add_argument("--stichprobe", action="store_true")
    a = ap.parse_args()
    J = json.load(open(os.path.join(T2, "re2_tueren.json")))
    if a.stichprobe:
        stichprobe(J, None)
        return
    S = J["seiten"]
    out = os.path.join(T2, "hg")
    os.makedirs(out, exist_ok=True)
    liste = []
    for k, p in enumerate(J["paare"]):
        if a.paar and k not in a.paar:
            continue
        A, B = S[p["a"]], S[p["b"]]
        if not A["datei"][4].isdigit():
            continue          # Reihe B hat dieselben Hintergruende
        # Boden der Seite A = y der Zielposition von B (B fuehrt in A's Raum)
        ya, yb = B["ziel_pos"][1], A["ziel_pos"][1]
        va = A["archiv_varianten"]
        vb = B["archiv_varianten"]
        def richtung(d):
            w = d * 2 * math.pi / 4096.0
            return (math.cos(w), -math.sin(w))
        ia, ca = bild_seite(A, ya, "DOOR%02X V%s" % (va[0][0], "/".join(str(x[1]) for x in va)), richtung(B["ziel_dir"]))
        ib, cb = bild_seite(B, yb, "DOOR%02X V%s" % (vb[0][0], "/".join(str(x[1]) for x in vb)), richtung(A["ziel_dir"]))
        rec = {"paar": k, "a": A["datei"], "b": B["datei"], "arch": va[0][0], "va": [x[1] for x in va],
               "vb": [x[1] for x in vb], "cam_a": ca, "cam_b": cb}
        if ia is None and ib is None:
            rec["bild"] = None
            liste.append(rec)
            continue
        W = 1284
        sh = Image.new("RGB", (W, 480 + 364), (30, 30, 30))
        if ia:
            sh.paste(ia, (0, 0))
        if ib:
            sh.paste(ib, (644, 0))
        fn = "paar%03d_DOOR%02X_%s_%s.png" % (k, va[0][0], A["datei"][4:8], B["datei"][4:8])
        sh.save(os.path.join(out, fn))
        rec["bild"] = fn
        liste.append(rec)
    json.dump(liste, open(os.path.join(out, "index.json"), "w"), indent=1)
    print("Paare mit Bild: %d von %d (Reihe A)" % (sum(1 for r in liste if r["bild"]), len(liste)))


if __name__ == "__main__":
    main()
