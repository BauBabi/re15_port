#!/usr/bin/env python3
"""Spur G (Runde 34 Nacht, Stufe BAU, Auflage 2) - Port-Framedumps gegen die BSS-Dekodierung.

Wertet eine RE15_FRAMEDUMP-Serie der ECHTEN exe aus ("<a>-<b>/<s>:<praefix>" schreibt
<praefix>NNNNNN.ppm, main.c ~10762) zusammen mit RE15_CUT_SYNC_LOG (je Bild
"F<n> room=<raum> bg=<raum>#<cut> view=<cut> ...", main.c pc_cut_sync_log):

  * Bildspeicher: Framedump bei RE15_WINDOW_SCALE=3 = 960x720; logisches Pixel = Mitte des 3x3-Blocks.
  * Cut je Bild = Hintergrund-Herkunft bg=<raum>#<cut> (der Hintergrund, der in DIESEM Bild in den
    Bildspeicher ging); fehlt sie (-1), gilt view=<cut>.
  * je Bild mit Schild im Cut (Rechtecke wie ss_zensus_1170.SIGN): max |Port - BSS| im Rechteck,
    Blau-Pixel, mittlere Helligkeit.
  * Lebendnachweis: Zahl der Bildpunkte, die sich gegen den vorigen Dump AUSSERHALB des
    Schild-Rechtecks aendern.
  * Zusammenfassung je zusammenhaengendem Cut-Abschnitt.

⛔ frame_count beginnt je Raum bei 0 (main.c): Dumps eines frueheren Raums mit gleicher Bildnummer
werden ueberschrieben. Mit RE15_EXIT_AT="<n>#1170" und n groesser als die Montage (1421 Bilder)
stammen am Ende ALLE Dumps aus ROOM1170; ausgewertet werden nur Bilder, die cutsync.log in
room=1170 fuehrt.

  C:/Python310/python.exe re15_port/tools/r34n_g/port_schild_eval.py <dump-praefix> <cutsync.log>
      <bss-praefix> [--raum 1170] [--scale 3]
"""
import argparse, glob, os, re, sys
import numpy as np

SIGN = {2: (283, 48, 314, 65), 3: (290, 4, 319, 23), 4: (315, 10, 319, 23), 10: (260, 0, 289, 12)}


def load_ppm(path):
    data = open(path, "rb").read()
    parts = data.split(b"\n", 3)
    w, h = [int(v) for v in parts[1].split()]
    return np.frombuffer(parts[3][:w * h * 3], dtype=np.uint8).reshape(h, w, 3).astype(np.int16)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dump")
    ap.add_argument("log")
    ap.add_argument("bss")
    ap.add_argument("--raum", default="1170")
    ap.add_argument("--scale", type=int, default=3)
    a = ap.parse_args()
    cut_of = {}
    for line in open(a.log, encoding="utf-8", errors="replace"):
        m = re.match(r"F(\d+) room=([0-9a-fA-F]+) bg=(\S+) view=(-?\d+)", line)
        if not m or m.group(2).lower() != a.raum.lower():
            continue
        f = int(m.group(1))
        bg = m.group(3)
        mb = re.match(r"([0-9a-fA-F]+)#(\d+)", bg)
        cut_of[f] = int(mb.group(2)) if mb and mb.group(1).lower() == a.raum.lower() else int(m.group(4))
    files = sorted(glob.glob(a.dump + "*.ppm"))
    bss = {}
    rows = []
    prev = None
    k = a.scale
    for p in files:
        mf = re.search(r"(\d+)\.ppm$", p)
        if not mf:
            continue
        f = int(mf.group(1))
        if f not in cut_of:
            continue
        cut = cut_of[f]
        big = load_ppm(p)
        img = big[k // 2::k, k // 2::k][:240, :320]
        live = -1
        if prev is not None:
            d = np.any(img != prev[1], axis=2)
            box = SIGN.get(cut)
            if box:
                d[box[1]:box[3] + 1, box[0]:box[2] + 1] = False
            live = int(d.sum())
        prev = (f, img)
        box = SIGN.get(cut)
        if box is None:
            rows.append((f, cut, None, None, None, live))
            continue
        if cut not in bss:
            bss[cut] = load_ppm("%s_cut%02d.ppm" % (a.bss, cut))
        x0, y0, x1, y1 = box
        c = img[y0:y1 + 1, x0:x1 + 1]
        ref = bss[cut][y0:y1 + 1, x0:x1 + 1]
        dmax = int(np.abs(c - ref).max())
        blue = int(((c[..., 2] > 150) & (c[..., 2] > c[..., 0] + 60) & (c[..., 2] > c[..., 1] + 40)).sum())
        lum = float(c.mean())
        rows.append((f, cut, dmax, blue, lum, live))
    print("Dumps %d, davon in room=%s mit Cut aus cutsync: %d" % (len(files), a.raum, len(rows)))
    for f, cut, dmax, blue, lum, live in rows:
        if dmax is None:
            print("F%05d cut %2d  (Schild nicht im Bild)  Lebend: %s px geaendert" % (f, cut, live))
        else:
            print("F%05d cut %2d  max|Port-BSS| %3d  blau %3d  L %6.1f  Lebend: %s px geaendert ausserhalb" % (
                f, cut, dmax, blue, lum, live))
    print()
    print("Abschnitte (zusammenhaengend gleicher Cut):")
    i = 0
    while i < len(rows):
        j = i
        while j + 1 < len(rows) and rows[j + 1][1] == rows[i][1]:
            j += 1
        seg = rows[i:j + 1]
        cut = rows[i][1]
        lives = [r[5] for r in seg if r[5] is not None and r[5] >= 0]
        nlive = sum(1 for v in lives if v > 0)
        if SIGN.get(cut):
            ds = [r[2] for r in seg]
            bl = [r[3] for r in seg]
            ls = [r[4] for r in seg]
            zero = sum(1 for v in ds if v == 0)
            print("  Cut %2d F%d..F%d (%d Dumps): max|Port-BSS| %d..%d (=0 in %d), blau %d..%d, L %.1f..%.1f; "
                  "Lebend: %d/%d Dumps mit Aenderung ausserhalb des Schilds" % (
                      cut, seg[0][0], seg[-1][0], len(seg), min(ds), max(ds), zero, min(bl), max(bl),
                      min(ls), max(ls), nlive, len(lives)))
        else:
            print("  Cut %2d F%d..F%d (%d Dumps): Schild nicht im Bild; Lebend: %d/%d" % (
                cut, seg[0][0], seg[-1][0], len(seg), nlive, len(lives)))
        i = j + 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
