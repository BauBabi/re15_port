# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: Lage, Groesse und DREHUNG des gemalten Klemmbretts.

Jeder helle Pixel des Blocks in Cut 6 (Nahaufnahme, ~3x mehr Pixel als Cut 2) wird ueber
seinen Sehstrahl auf die Tischebene y=-1520 gelegt. Die Hauptachsen der Punktwolke in der
Welt-XZ-Ebene geben Mitte, Kantenlaengen und Drehung. Die Schwelle ist durchgefahren.

Yaw-Konvention des Ports (platform/pc/main.c pc_prop_rot_q12, reines rot_y):
    Modell (x,0,z) -> Welt (c*x + s*z, 0, -s*x + c*z),  c=cos, s=sin, 4096 = 360 Grad
    also zeigt die Modell-Z-Achse (lange Buchkante) in Weltrichtung (s, c).
"""
import os, sys, math
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_idw_geom import lade_kameras, auf_ebene, AUS

H = -1520.0


def main():
    cams = lade_kameras()
    A6 = np.array(Image.open(os.path.join(AUS, 'bg', 'ROOM11506.png')).convert('RGB')).astype(float)
    L = A6.sum(axis=2) / 3.0
    box = (170, 225, 95, 150)
    for s in (110, 120, 130, 140):
        pts = []
        for y in range(box[2], box[3]):
            for x in range(box[0], box[1]):
                if L[y, x] >= s:
                    p, _ = auf_ebene(cams[6], x + 0.5, y + 0.5, H)
                    pts.append((p[0], p[2]))
        P = np.array(pts)
        m = P.mean(axis=0)
        C = np.cov((P - m).T)
        w, v = np.linalg.eigh(C)
        lang = v[:, 1]; kurz = v[:, 0]
        a = (P - m) @ lang; b = (P - m) @ kurz
        # Modell-Z (lang) zeigt nach Welt (sin, cos): ry = atan2(x, z); Vorzeichen so, dass die
        # Achse von der Kamera weg zeigt (Welt -x)
        if lang[0] > 0:
            lang = -lang
        ry = math.atan2(lang[0], lang[1]) / (2 * math.pi) * 4096.0
        if ry < 0:
            ry += 4096.0
        print('Schwelle %d: n=%d Mitte (%.0f, %.0f) | lange Kante %.0f (5..95 %%: %.0f), kurze Kante %.0f (5..95 %%: %.0f) | '
              'lange Achse Welt (%.3f, %.3f) -> rot_y = %.0f (Abweichung von 3072: %+.0f = %+.1f Grad)'
              % (s, len(P), m[0], m[1], a.max() - a.min(), np.percentile(a, 95) - np.percentile(a, 5),
                 b.max() - b.min(), np.percentile(b, 95) - np.percentile(b, 5),
                 lang[0], lang[1], ry, ry - 3072, (ry - 3072) * 360 / 4096.0))


if __name__ == '__main__':
    main()
