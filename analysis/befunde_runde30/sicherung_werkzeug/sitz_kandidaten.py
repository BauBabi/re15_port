#!/usr/bin/env python3
"""Rechnet SITZ-KANDIDATEN der Sicherung auf dem Hebetisch in Cut 4 durch (Z-Puffer).

Je Kandidat und je Plattform-Hoehe: Groesse der Sicherung im Bild (320x240) und wie viel
davon nach WAHRER Verdeckung durch Plattform und Deckelhaelften uebrig bleibt.
Das ist die VORAUSWAHL — die Abnahme ist der Framedump aus der Mess-Variante
(re15_pc_r30_sicherung, RE15_R30_SITZ), denn der Port sortiert Dreiecke nach mittlerer
Tiefe und nicht je Pixel.

    python analysis/befunde_runde30/sicherung_werkzeug/sitz_kandidaten.py
"""
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_lib import (REPO, cuts, inc_bytes, md1_lesen, prop_rot, props, rdt_laden,  # noqa: E402
                     tim_lesen, verketten, view_bauen, view_x_welt)
from raster import Bild, flaechen, tim_als_rgba  # noqa: E402

ZIEL = os.path.join(REPO, "build", "r30_sicherung")
PLATTFORM_XZ = (-20700, -17460)      # Pos_set @0x0FB4 / Obj_model_set @0x0E00
PLATTFORM_ROT = (0, 2048, 0)         # Obj_model_set @0x0E00 pc[18..19] = 00 08
DECKEL_WEG = 240                     # For 24 @0x0FC0 x Speed_set +-10 @0x0FCA/@0x0FD4
F = 3

# name, (x,y,z), (rx,ry,rz), Herleitung
KANDIDATEN = [
    ("BESTAND Deck, laengs",  (-628, -927, 784),  (0, 0, 0),
     "include/re15_sicherung.h:57-59"),
    ("K1 Kuppelfach, quer",   (-280, -1062, 1260), (0, 1024, 0),
     "Tafel y=-1036 (q79/q80/q81) x[-485..-74], Naht z=1260; y = -1036 - 26"),
    ("K2a Regalfach A, quer", (-150, -116, 478),  (0, 1024, 0),
     "Boden q89 y=-90, Waende q86 z=96 / q88 z=861 -> Mitte z=478; y = -90 - 26"),
    ("K2b Regalfach B, quer", (-150, -117, 1332), (0, 1024, 0),
     "Boden q93 y=-91, Waende q90 z=950 / q92 z=1715 -> Mitte z=1332; y = -91 - 26"),
]
HOEHEN = [(-305, False, "Start, Deckel zu"), (-305, True, "Deckel offen"),
          (-705, True, "halbe Fahrt"), (-1105, True, "Modal-Schranke"),
          (-1205, True, "Hochpunkt")]


def zeichne(b, view, mesh, tex, wrot, wpos, kennung):
    cr, ct = view_x_welt(view, wrot, wpos)
    for (p, uv, po) in flaechen(mesh):
        q = []
        for v in p:
            vx = ((v[0] * cr[0] + v[1] * cr[1] + v[2] * cr[2]) >> 12) + ct[0]
            vy = ((v[0] * cr[3] + v[1] * cr[4] + v[2] * cr[5]) >> 12) + ct[1]
            vz = ((v[0] * cr[6] + v[1] * cr[7] + v[2] * cr[8]) >> 12) + ct[2]
            if vz < 64:
                q = None
                break
            q.append(((160 + view["H"] * vx / vz) * F, (120 + view["H"] * vy / vz) * F, vz))
        if q:
            b.dreieck(q, uv, tex, kennung)


def main():
    rdt = rdt_laden("ROOM1150.RDT")
    pr = props(rdt)
    m = [md1_lesen(p["md1"])["meshes"][0] for p in pr]
    tex = [tim_als_rgba(tim_lesen(p["tim"])) for p in pr]
    sm = md1_lesen(inc_bytes("re15_sicherung_md1"))["meshes"][0]
    stex = tim_als_rgba(tim_lesen(inc_bytes("re15_sicherung_tim")))
    view = view_bauen(cuts(rdt)[4])
    prot = prop_rot(*PLATTFORM_ROT)

    bilder = []
    for name, sitz, rot, woher in KANDIDATEN:
        print("\n== %s  Sitz %s  rot %s ==\n   (%s)" % (name, sitz, rot, woher))
        for py, offen, wann in HOEHEN:
            ppos = [PLATTFORM_XZ[0], py, PLATTFORM_XZ[1]]
            b = Bild(320 * F, 240 * F)
            zeichne(b, view, m[0], tex[0], prot, ppos, 0)
            dz = DECKEL_WEG if offen else 0
            for k, s in ((1, dz), (2, -dz)):
                r, t = verketten(prot, ppos, prop_rot(0, 0, 0), [0, 0, s])
                zeichne(b, view, m[k], tex[k], r, t, k)
            r, t = verketten(prot, ppos, prop_rot(*rot), list(sitz))
            allein = Bild(320 * F, 240 * F)
            zeichne(allein, view, sm, stex, r, t, 4)
            zeichne(b, view, sm, stex, r, t, 4)
            ya, xa = np.nonzero(allein.wer == 4)
            ys, xs = np.nonzero(b.wer == 4)
            im_bild = ((xa >= 0) & (xa < 320 * F)).sum()
            if len(xa) == 0:
                print("   y=%5d %-16s AUSSERHALB des Bildes" % (py, wann))
                continue
            print("   y=%5d %-16s unverdeckt %5.0f px (320x240), bbox x%5.1f..%5.1f y%5.1f..%5.1f "
                  "(%4.1f x %4.1f) | sichtbar %5.0f px = %3.0f %%"
                  % (py, wann, len(xa) / (F * F), xa.min() / F, xa.max() / F, ya.min() / F,
                     ya.max() / F, (xa.max() - xa.min() + 1) / F, (ya.max() - ya.min() + 1) / F,
                     len(xs) / (F * F), 100.0 * len(xs) / len(xa)))
            if py == -1205 or (py == -305 and offen):
                img = Image.fromarray(b.rgb)
                d = ImageDraw.Draw(img)
                d.rectangle((xa.min() - 4, ya.min() - 4, xa.max() + 4, ya.max() + 4),
                            outline=(255, 0, 255))
                d.text((4, 4), "%s | Plattform y=%d (%s)" % (name, py, wann), fill=(255, 255, 0))
                bilder.append(img.resize((480, 360)))
    blatt = Image.new("RGB", (480 * 2 + 10, (360 + 10) * len(KANDIDATEN)), (255, 0, 255))
    for i, im in enumerate(bilder):
        blatt.paste(im, ((i % 2) * 490, (i // 2) * 370))
    blatt.save(os.path.join(ZIEL, "sitz_kandidaten_zpuffer.png"))
    print("\ngeschrieben: build/r30_sicherung/sitz_kandidaten_zpuffer.png")


if __name__ == "__main__":
    main()
