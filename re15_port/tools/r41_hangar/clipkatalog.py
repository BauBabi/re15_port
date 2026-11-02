#!/usr/bin/env python3
"""Runde 41 Spur H: Clip-Katalog einer EMD-Bank (Bank 0) als Bildbogen, zum Abgleich mit dem 80%-Video.

    python re15_port/tools/r41_hangar/clipkatalog.py --typ 0x2B --von 0 --bis 9 --out <png>

Je Zeile ein Clip, je Spalte ein Bild (gleichmaessig ueber die Clip-Laenge). Geometrie, Skelett, Pose und Textur
liest emd_ansichtsblatt.py (Port der Engine-Leser); Ansicht = Kamera schraeg von vorn-links (nur Lesbarkeit).
Die Wurzelverschiebung je Bild (kf_pos) bleibt erhalten: wandert der Clip, wandert die Figur in der Kachel.
"""
import argparse, os, sys
import numpy as np
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import emd_ansichtsblatt as ea

def ansicht(gier_grad):
    import math
    g = math.radians(gier_grad)
    # Figur-vorn = +X, unten = +Y. Kamera kreist um Y: rechts/tiefe in der XZ-Ebene.
    depth = np.array([-math.cos(g), 0.0, -math.sin(g)])      # Blick zur Figur
    down = np.array([0.0, 1.0, 0.0])
    right = np.cross(down, depth); right = right / np.linalg.norm(right)
    return right, down, depth

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--typ', default='0x2B'); ap.add_argument('--von', type=int, default=0)
    ap.add_argument('--bis', type=int, default=9); ap.add_argument('--spalten', type=int, default=6)
    ap.add_argument('--gier', type=float, default=35.0); ap.add_argument('--kachel', type=int, default=150)
    ap.add_argument("--hell", type=float, default=2.4); ap.add_argument("--out", required=True)
    a = ap.parse_args()
    mo = ea.Modell(int(a.typ, 0))
    right, down, depth = ansicht(a.gier)
    K = a.kachel; zeilen = list(range(a.von, min(a.bis, len(mo.clips) - 1) + 1))
    S = Image.new('RGB', (60 + K * a.spalten, K * len(zeilen)), (18, 19, 22))
    d = ImageDraw.Draw(S)
    for r, c in enumerate(zeilen):
        first, n = mo.clips[c]
        d.text((4, r * K + 4), 'clip\n0x%02X\n%d B.' % (c, n), fill=(230, 230, 230))
        for s in range(a.spalten):
            fi = 0 if a.spalten == 1 else round(s * (n - 1) / (a.spalten - 1))
            kf = mo.frames[first + fi] & 0xfff
            V, N, F = mo.geometrie(kf)
            col, alp = ea.render(mo, V, N, F, right, down, depth, K, K, K / 5600.0, K / 2, K * 0.80)
            col = col * a.hell
            img = Image.fromarray((np.clip(col, 0, 1) * 255).astype(np.uint8))
            bg = Image.new('RGB', (K, K), (28, 30, 35)); bg.paste(img, (0, 0), Image.fromarray((alp * 255).astype(np.uint8)))
            dd = ImageDraw.Draw(bg); dd.line([(0, K * 0.80), (K, K * 0.80)], fill=(60, 64, 72))
            dd.text((3, K - 12), 'B%d' % fi, fill=(214, 138, 74))
            S.paste(bg, (60 + s * K, r * K))
    S.save(a.out); print(a.out, S.size)

if __name__ == '__main__':
    main()
