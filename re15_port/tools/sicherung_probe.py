#!/usr/bin/env python3
"""SICHTPROBE: das Modell in den leeren Sockel von Cut 7 rendern und neben das
Original-Cut-8 stellen — links die Vorlage, rechts der Nachbau am selben Ort.

Die Zahlen liefert sicherung_abnahme.py; dieses Bild ist die Gegenprobe fuer das
Auge, kein Ersatz dafuer. Texturiert wird AFFIN und ohne Zusatzlicht: die Textur
traegt bereits die Originalbeleuchtung (sie stammt aus BG08), und affines Mapping
ist genau das, was die PSX-GPU tut.

    python re15_port/tools/sicherung_probe.py [--ziel build/sicherung/probe.png]
"""
import argparse
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sicherung_modell import bg_laden, modell_bauen, textur_bauen           # noqa: E402
from sicherung_abnahme import (CAM, H, EBENE_X, basis, projiziere,          # noqa: E402
                               rueckprojekt, silhouette_original)


def rendern(bg, v, vt, faces, tex):
    """Affiner Textur-Rasterizer mit Z-Puffer (Painter waere bei einem konvexen
    Rotationskoerper auch genug, der Z-Puffer macht es unabhaengig davon)."""
    bild = np.array(bg, np.uint8).copy()
    zbuf = np.full((240, 320), 1e9, np.float32)
    T = np.asarray(tex, np.uint8)
    th, tw = T.shape[:2]
    proj = [projiziere(p) for p in v]

    for fc in faces:
        if any(proj[a - 1] is None for a, _ in fc):
            continue
        idx = [(proj[a - 1], vt[b - 1]) for a, b in fc]
        tri = [(0, 1, 2), (0, 2, 3)] if len(idx) == 4 else [(0, 1, 2)]
        for t in tri:
            (p0, u0), (p1, u1), (p2, u2) = idx[t[0]], idx[t[1]], idx[t[2]]
            x0, y0, z0 = p0; x1, y1, z1 = p1; x2, y2, z2 = p2
            flaeche = (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0)
            if flaeche >= 0:                       # Rueckseite wegwerfen
                continue
            xmin = max(0, int(np.floor(min(x0, x1, x2))))
            xmax = min(319, int(np.ceil(max(x0, x1, x2))))
            ymin = max(0, int(np.floor(min(y0, y1, y2))))
            ymax = min(239, int(np.ceil(max(y0, y1, y2))))
            for py in range(ymin, ymax + 1):
                for px in range(xmin, xmax + 1):
                    cx, cy = px + 0.5, py + 0.5
                    w0 = ((x1 - cx) * (y2 - cy) - (x2 - cx) * (y1 - cy)) / flaeche
                    w1 = ((x2 - cx) * (y0 - cy) - (x0 - cx) * (y2 - cy)) / flaeche
                    w2 = 1.0 - w0 - w1
                    if w0 < 0 or w1 < 0 or w2 < 0:
                        continue
                    z = w0 * z0 + w1 * z1 + w2 * z2
                    if z >= zbuf[py, px]:
                        continue
                    uu = w0 * u0[0] + w1 * u1[0] + w2 * u2[0]
                    vv = w0 * u0[1] + w1 * u1[1] + w2 * u2[1]
                    tx = min(tw - 1, max(0, int(uu * tw)))
                    ty = min(th - 1, max(0, int((1.0 - vv) * th)))
                    zbuf[py, px] = z
                    bild[py, px] = T[ty, tx]
    return Image.fromarray(bild)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ziel", default=os.path.join("build", "sicherung", "probe.png"))
    a = ap.parse_args()

    tex, _ = textur_bauen()
    v, vt, faces = modell_bauen()

    # Zielort = der LEERE Sockel aus Cut 7, also dieselbe Stelle, an der Cut 8 die
    # Sicherung zeigt. Fusspunkt aus der Original-Silhouette zurueckprojiziert.
    orig = silhouette_original()
    oy = sorted(orig)
    y_unten = max(oy)
    x_mitte = (orig[y_unten][0] + orig[y_unten][1]) / 2.0
    fuss = rueckprojekt(x_mitte, y_unten, EBENE_X)
    welt = [np.array([fuss[0] + p[2], fuss[1] - p[1], fuss[2] + p[0]]) for p in v]

    cut7 = Image.fromarray(bg_laden("ROOM10507.ppm"))
    cut8 = Image.fromarray(bg_laden("ROOM10508.ppm"))
    gerendert = rendern(cut7, welt, vt, faces, tex)

    def hell(im, f=2.6):
        return Image.fromarray(np.clip(np.asarray(im, np.float32) * f, 0, 255).astype(np.uint8))

    kasten = (120, 40, 250, 190)
    S = 4
    w = (kasten[2] - kasten[0]) * S
    h = (kasten[3] - kasten[1]) * S
    blatt = Image.new("RGB", (w * 3 + 40, h), (40, 40, 40))
    for i, (im, _lbl) in enumerate(((cut7, "Cut 7 original (leer)"),
                                    (cut8, "Cut 8 original (eingesetzt)"),
                                    (gerendert, "Cut 7 + Modell"))):
        blatt.paste(hell(im.crop(kasten)).resize((w, h), Image.NEAREST), (i * (w + 20), 0))
    os.makedirs(os.path.dirname(a.ziel) or ".", exist_ok=True)
    blatt.save(a.ziel)
    gerendert.save(os.path.splitext(a.ziel)[0] + "_voll.png")
    print("links Cut7 (leer) | Mitte Cut8 (Original eingesetzt) | rechts Cut7 + Modell")
    print("geschrieben:", a.ziel)


if __name__ == "__main__":
    main()
