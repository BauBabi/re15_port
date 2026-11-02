#!/usr/bin/env python3
"""Abnahme 2: Karte Original gegen gefiltertes Render, ohne jede Nutzermaske. Rot: Original dunkel, Render hell (fehlt). Cyan: Original hell, Render dunkel (zu viel). Kein RE-Wert."""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(os.path.dirname(HIER))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
def Y(a):
    return (.299 * a[..., 0] + .587 * a[..., 1] + .114 * a[..., 2]).astype(np.float32)
for n, box, schw in ((1, (175, 170, 450, 480), 55), (2, (130, 135, 485, 480), 55)):
    o = P.original(n)
    g = cv2.imread(os.path.join(HIER, 'bild', 'gef_bild%d.png' % n))[:, :, ::-1]
    m = np.load(os.path.join(HIER, 'bild', 'maske_bild%d.npy' % n))
    yo, yg = cv2.GaussianBlur(Y(o), (0, 0), 1.2), cv2.GaussianBlur(Y(g), (0, 0), 1.2)
    fehlt = (yg - yo) > schw
    zuviel = (yo - yg) > schw
    k = np.ones((3, 3), np.uint8)
    fehlt = cv2.morphologyEx(fehlt.astype(np.uint8), cv2.MORPH_OPEN, k).astype(bool)
    zuviel = cv2.morphologyEx(zuviel.astype(np.uint8), cv2.MORPH_OPEN, k).astype(bool)
    x0, y0, x1, y1 = box
    fe = np.zeros(o.shape[:2], bool); fe[y0:y1, x0:x1] = True
    print('=== Bild', n)
    for name, b in (('fehlt (Original dunkel, Render hell)', fehlt & fe), ('zu viel (Original hell, Render dunkel)', zuviel & fe)):
        nl, lab, st, ce = cv2.connectedComponentsWithStats(b.astype(np.uint8), connectivity=8)
        z = sorted([(st[i, 4], st[i, 0], st[i, 1], st[i, 0] + st[i, 2], st[i, 1] + st[i, 3], (m & (lab == i)).sum()) for i in range(1, nl)], reverse=True)
        print(name, 'gesamt', int(b.sum()))
        for t in z[:10]:
            print('   %5d px  x %d-%d  y %d-%d  im Modell %d' % (t[0], t[1], t[3], t[2], t[4], t[5]))
    s = 3
    a = o[y0:y1, x0:x1].astype(np.float32)
    a = (a * .45 + 60).astype(np.uint8)
    a[fehlt[y0:y1, x0:x1]] = (255, 40, 40)
    a[zuviel[y0:y1, x0:x1]] = (40, 230, 255)
    a = cv2.resize(a, None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
    ko, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    for c in ko:
        c = ((c - np.array([[x0, y0]])) * s + s // 2).astype(np.int32)
        cv2.polylines(a, [c], True, (255, 255, 255), 1)
    # Zeilenraster alle 20 px
    for yy in range(y0 - y0 % 20 + 20, y1, 20):
        cv2.putText(a, str(yy), (2, (yy - y0) * s), cv2.FONT_HERSHEY_SIMPLEX, .33, (255, 255, 0), 1, cv2.LINE_AA)
    for xx in range(x0 - x0 % 20 + 20, x1, 20):
        cv2.putText(a, str(xx), ((xx - x0) * s, 10), cv2.FONT_HERSHEY_SIMPLEX, .33, (255, 255, 0), 1, cv2.LINE_AA)
    cv2.imwrite(os.path.join(HIER, 'bild', 'flecken_bild%d.png' % n), a[:, :, ::-1])
