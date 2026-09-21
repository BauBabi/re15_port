# -*- coding: utf-8 -*-
"""Fuer jedes Geometrie-Doppel in ROOM10F0 C4/C5: die ATLAS-KACHELN vergleichen.

Frage 4 des Auftrags: Aendern die Doppel das BILD, oder kosten sie nur Budget?
Gemessen wird pro Paar:
  - Deckung (Palettenindex != 0) beider Kacheln: Punktzahl und Symmetrische Differenz
  - Farbunterschied auf den gemeinsam gedeckten Punkten
"""
import os
import struct
import sys
import numpy as np
sys.path.insert(0, "re15_port/tools/maske")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from zensus import container

MASKS = "re15_port/shared_assets/PSX/MASKS"


def lies_tim_bytes(b):
    magic, flag = struct.unpack_from("<II", b, 0)
    assert magic == 0x10
    o = 8
    clut = None
    if flag & 8:
        blen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, o)
        clut = np.frombuffer(b, np.uint16, cw * ch, o + 12)
        o += blen
    blen, px, py, pw, ph = struct.unpack_from("<IHHHH", b, o)
    idx = np.frombuffer(b, np.uint8, pw * 2 * ph, o + 12).reshape(ph, pw * 2)
    return idx, clut


def rgb(clut):
    c = np.asarray(clut, np.uint32)
    r = (c & 31) << 3
    g = ((c >> 5) & 31) << 3
    b = ((c >> 10) & 31) << 3
    return np.stack([r, g, b], 1).astype(np.int32)


if __name__ == "__main__":
    for cut in (4, 5):
        res, _ = container(os.path.join(MASKS, "ROOM10F0.MSK"))
        masks, decl, gc, build = res[cut]
        idx, clut = lies_tim_bytes(open(os.path.join(MASKS, "ROOM10F0_PRI%02d.TIM" % cut), "rb").read())
        pal = rgb(clut)
        print("=== ROOM10F0 C%d  Atlas %dx%d  Masken %d" % (cut, idx.shape[1], idx.shape[0], len(masks)))
        geo = {}
        paare = []
        for i, m in enumerate(masks):
            k = m[2:]
            if k in geo:
                paare.append((geo[k], i))
            else:
                geo[k] = i
        print("  Geometrie-Doppel: %d" % len(paare))
        ges_a = ges_b = ges_nur_a = ges_nur_b = ges_farbdiff = ges_beide = 0
        print("  %-11s %-16s  %5s %5s %5s %5s %6s" %
              ("Paar", "dst / Groesse / t", "|A|", "|B|", "nurA", "nurB", "Farbe!=") )
        for (a, b) in paare:
            ma, mb = masks[a], masks[b]
            w, h = ma[5], ma[6]
            A = idx[ma[1]:ma[1] + h, ma[0]:ma[0] + w]
            B = idx[mb[1]:mb[1] + h, mb[0]:mb[0] + w]
            ca, cb = A != 0, B != 0
            beide = ca & cb
            farbdiff = int((beide & (A != B)).sum())
            print("  %3d==%-6d (%3d,%3d) %2dx%-2d t=%-3d %5d %5d %5d %5d %6d" % (
                a, b, ma[2], ma[3], w, h, ma[4],
                int(ca.sum()), int(cb.sum()), int((ca & ~cb).sum()), int((cb & ~ca).sum()), farbdiff))
            ges_a += int(ca.sum()); ges_b += int(cb.sum())
            ges_nur_a += int((ca & ~cb).sum()); ges_nur_b += int((cb & ~ca).sum())
            ges_beide += int(beide.sum()); ges_farbdiff += farbdiff
        print("  SUMME  A=%d  B=%d  nurA=%d  nurB=%d  beide=%d  davon Farbe!=%d"
              % (ges_a, ges_b, ges_nur_a, ges_nur_b, ges_beide, ges_farbdiff))
        print()
