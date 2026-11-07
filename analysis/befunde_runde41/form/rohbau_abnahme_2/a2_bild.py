#!/usr/bin/env python3
"""Abnahme 2: Original und gefiltertes Render nebeneinander, nichts ins Original gemischt. Kein RE-Wert.
Erzeugt ganz_bild*.png, nah_*.png, nah_*_hell.png (aufgehellt) und Umriss-Blaetter."""
import os, sys
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
import form_video as FV
K = os.path.join(FORM, 'rohbau_final', 'form.json')
AUS = os.path.join(HIER, 'bild')
os.makedirs(AUS, exist_ok=True)
F = cv2.FONT_HERSHEY_SIMPLEX
def sch(a, t, xy=(6, 18), g=.52, f=(255, 255, 90)):
    a = a.copy()
    cv2.putText(a, t, xy, F, g, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(a, t, xy, F, g, f, 1, cv2.LINE_AA)
    return a
def cut(a, box, s, ton=None):
    x0, y0, x1, y1 = box
    c = a[y0:y1, x0:x1].astype(np.float32)
    if ton:
        c = 255 * np.power(np.clip(c / 255 * ton[0], 0, 1), ton[1])
    return cv2.resize(np.clip(c, 0, 255).astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)
def save(ts, name):
    h = max(t.shape[0] for t in ts)
    ts = [cv2.copyMakeBorder(t, 0, h - t.shape[0], 2, 2, cv2.BORDER_CONSTANT, value=(18, 18, 22)) for t in ts]
    cv2.imwrite(os.path.join(AUS, name), np.hstack(ts)[:, :, ::-1])
BOX = {1: [('figur', (175, 170, 450, 480), 3), ('oben', (235, 175, 445, 335), 4), ('unten', (185, 310, 350, 480), 4), ('arm', (365, 225, 445, 390), 4)],
       2: [('figur', (130, 135, 485, 480), 3), ('kopf', (200, 140, 430, 345), 4), ('hand', (130, 310, 345, 480), 4), ('arm', (355, 250, 480, 425), 4)]}
k = P.Kandidat(K)
for n in (1, 2):
    sauber, alb, m, _ = k.rendern(n)
    gef = FV.video_filter(sauber, bild=n, entzerren=False)
    o = P.original(n)
    cv2.imwrite(os.path.join(AUS, 'gef_bild%d.png' % n), gef[:, :, ::-1])
    cv2.imwrite(os.path.join(AUS, 'sauber_bild%d.png' % n), sauber[:, :, ::-1])
    np.save(os.path.join(AUS, 'maske_bild%d.npy' % n), m)
    save([sch(o, 'Original Bild %d' % n), sch(gef, 'Render gefiltert')], 'ganz_bild%d.png' % n)
    for t, box, s in BOX[n]:
        save([sch(cut(o, box, s), 'Original'), sch(cut(gef, box, s), 'Render gefiltert')], 'nah_b%d_%s.png' % (n, t))
        save([sch(cut(o, box, s, (2.4, .7)), 'Original hell'), sch(cut(gef, box, s, (2.4, .7)), 'Render gefiltert hell')], 'nah_b%d_%s_hell.png' % (n, t))
    hell = np.clip(255 * np.power(np.clip(o.astype(np.float32) / 255 * 2.2, 0, 1), .7), 0, 255).astype(np.uint8)
    hell = cv2.resize(hell, None, fx=2, fy=2, interpolation=cv2.INTER_CUBIC)
    mm = cv2.resize(m.astype(np.uint8), None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    ko, _h = cv2.findContours(mm, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(hell, ko, -1, (255, 255, 255), 1)
    cv2.imwrite(os.path.join(AUS, 'umriss_bild%d.png' % n), hell[:, :, ::-1])
print('fertig')
