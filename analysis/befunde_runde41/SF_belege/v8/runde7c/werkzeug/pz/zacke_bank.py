"""Zacke-Regel (zacke2.stuecke) auf Klassen der Bank (marker_p5.klassen, FIGUR_DIFF 40, cv2-Verkleinerung): 9 -> 3 (Kopf), 8 -> 2 (Ruecken).
python zacke_bank.py N F0 F1   (bLM_N = Bank-LM-Lauf, N = plain-Lauf)"""
import sys, numpy as np
sys.path.insert(0, 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/bank')
import os
here = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, here)
import marker_p5 as MK, voll_p5 as VP
import zacke2 as Z, szene as SZ
n, f0, f1 = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
for f in range(f0, f1 + 1):
    M = VP.ppm(SZ.L_ + 'bLM_%s/fd/f%06d.ppm' % (n, f)); P = VP.ppm(SZ.L_ + '%s/fd/f%06d.ppm' % (n, f))
    k = MK.klassen(M); k[np.abs(M - P).sum(-1) <= VP.FIGUR_DIFF] = -1
    b = VP.klein(k).astype(np.int16); c = np.full(b.shape, -1, np.int8)
    c[b == 0] = 0; c[b == 3] = 0; c[b == 8] = 2; c[b == 9] = 3
    st = Z.stuecke(c, VP.klein(P.astype(np.int16)).astype(float))
    r = [(s['px'], s['tief'], s['maxtiefe'], s['seite'], s['L_wand'], 'UEBER' if s['ueber'] else '') for s in st if s['seite'] == 'r']
    if r: print('F%d' % f, r)
