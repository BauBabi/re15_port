#!/usr/bin/env python3
"""Spur G2 (Runde 34 Nacht) - Port-Framedumps ROOM1150/1151 Cut 2: Zustand der Leuchtschrift je Bild.

Wertet eine RE15_FRAMEDUMP-Serie der ECHTEN exe aus ("<a>-<b>/<s>:<praefix>" -> <praefix>NNNNNN.ppm)
zusammen mit RE15_CUT_SYNC_LOG ("F<n> room=<raum> bg=<raum>#<cut> view=<cut> ...", main.c
pc_cut_sync_log). Je Bild in Cut 2 wird das Schrift-Rechteck x139..216 y25..40 (Vereinigung der
Masken-Rechtecke der Gruppen 6..11, sprite.pri Cut 2 @0x0066C) verglichen mit
  AUS-Referenz = BSS-Dekodierung (Masken 6..11 nicht gezeichnet = rote Schrift sichtbar) und
  AN-Referenz  = BSS + Masken 6..11 aus dem SLD-Atlas (so zeichnet FUN_80039590 bei Byte0 & 1).
Beide Referenzen kommen aus schrift1150_masken.py (gleiche Regeln wie der Port-Atlaslader:
Kanal << 3, Index 0 durchsichtig). Ausgabe je Bild: max|d| zu beiden, Zustand, Lebendnachweis
(Pixel, die sich gegen den vorigen Dump AUSSERHALB des Rechtecks aendern), dazu die Laeufe
gleichen Zustands (Takt).

  C:/Python310/python.exe re15_port/tools/r34n_g/port_schrift1150_eval.py <dump-praefix> <cutsync.log>
      <bss-ppm-praefix> [--raum 1150] [--scale 3]
"""
import argparse
import glob
import os
import re
import struct
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import schrift1150_masken as SM  # noqa: E402

BOX = (139, 25, 216, 40)   # inklusive; Vereinigung dst der Gruppen 6..11 (schrift1150_masken.py)


def load_ppm(path):
    data = open(path, "rb").read()
    parts = data.split(b"\n", 3)
    w, h = [int(v) for v in parts[1].split()]
    return np.frombuffer(parts[3][:w * h * 3], dtype=np.uint8).reshape(h, w, 3).astype(np.int16)


def referenzen(room, bss_praefix):
    rdt, _ = SM.original.load_rdt(SM.CD, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    _, recs = SM.records(rdt, cam, 2)
    idx, clut = SM.lade_atlas(room, 2)
    bg = load_ppm("%s_cut02.ppm" % bss_praefix).astype(np.uint8)
    an = SM.blit(bg, recs, idx, clut)
    aus = SM.blit(bg, [m for m in recs if not (6 <= m["gruppe"] <= 11)], idx, clut)
    return an.astype(np.int16), aus.astype(np.int16)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dump")
    ap.add_argument("log")
    ap.add_argument("bss")
    ap.add_argument("--raum", default="1150")
    ap.add_argument("--scale", type=int, default=3)
    a = ap.parse_args()
    room = "ROOM%s" % a.raum.upper()
    an, aus = referenzen(room, a.bss)
    x0, y0, x1, y1 = BOX
    ref_an = an[y0:y1 + 1, x0:x1 + 1]
    ref_aus = aus[y0:y1 + 1, x0:x1 + 1]
    cut_of = {}
    for line in open(a.log, encoding="utf-8", errors="replace"):
        m = re.match(r"F(\d+) room=([0-9a-fA-F]+) bg=(\S+) view=(-?\d+)", line)
        if not m or m.group(2).lower() != a.raum.lower():
            continue
        mb = re.match(r"([0-9a-fA-F]+)#(\d+)", m.group(3))
        cut_of[int(m.group(1))] = (int(mb.group(2)) if mb and mb.group(1).lower() == a.raum.lower()
                                   else int(m.group(4)))
    rows = []
    prev = None
    k = a.scale
    for p in sorted(glob.glob(a.dump + "*.ppm")):
        mf = re.search(r"(\d+)\.ppm$", p)
        if not mf:
            continue
        f = int(mf.group(1))
        if f not in cut_of:
            continue
        img = load_ppm(p)[k // 2::k, k // 2::k][:240, :320]
        live = -1
        if prev is not None:
            d = np.any(img != prev, axis=2)
            d[y0:y1 + 1, x0:x1 + 1] = False
            live = int(d.sum())
        prev = img
        if cut_of[f] != 2:
            rows.append((f, cut_of[f], None, None, "-", live))
            continue
        c = img[y0:y1 + 1, x0:x1 + 1]
        d_an = int(np.abs(c - ref_an).max())
        d_aus = int(np.abs(c - ref_aus).max())
        st = "AN(Masken)" if d_an == 0 else ("AUS(BSS)" if d_aus == 0 else "anders")
        rows.append((f, 2, d_an, d_aus, st, live))
    print("Referenzen: AN vs AUS unterscheiden sich im Rechteck in %d Pixeln"
          % int(np.any(ref_an != ref_aus, axis=2).sum()))
    for f, cut, d_an, d_aus, st, live in rows:
        if d_an is None:
            print("F%05d cut %2d  (nicht Cut 2)  Lebend %d" % (f, cut, live))
        else:
            print("F%05d cut  2  max|d| zu AN %3d, zu AUS %3d  -> %-10s  Lebend %d px" % (f, d_an, d_aus, st, live))
    print("\nLaeufe gleichen Zustands (nur Cut 2):")
    i = 0
    c2 = [r for r in rows if r[1] == 2]
    while i < len(c2):
        j = i
        while j + 1 < len(c2) and c2[j + 1][4] == c2[i][4]:
            j += 1
        lives = [r[5] for r in c2[i:j + 1] if r[5] >= 0]
        print("  %-10s F%d..F%d (%d Dumps), Lebend: %d Dumps mit Aenderung ausserhalb"
              % (c2[i][4], c2[i][0], c2[j][0], j - i + 1, sum(1 for v in lives if v > 0)))
        i = j + 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
