#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht): Vorschau der gruenen Generator-Lampen auf dem RE1.5-Original-Hintergrund
ROOM11F10.bmp (Cut 10) — zum Vergleichen von Groesse/Lage VOR dem Bau. Die Kunst ist RE2 ROOM2130
ESP 0x16 (esp16.tim), Zelle 3/4 (u 96/128, 32x32), Palette 2 (gruen), additiv (ABR1, B+F, 5-Bit-
saettigend wie die PSX-GPU). Zeichnet die Zelle als achsparalleles Viereck (POLY_FT4, UV 0..S-1,
wie FUN_80077ed0 bei Vergroesserung < 2: kein Kantenabzug) mit naechstem Texel.
Aufruf: re15_lampen_vorschau.py <out.png> <kante_px> [zelle 3|4] [oben|unten|beide|keine]"""
import os, sys, struct, math
from PIL import Image
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
TIM = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room2130", "esp16.tim")
BGP = sys.argv[5] if len(sys.argv) > 5 else "C:/workspace/git/reAi_v2/extracted/PSX/STAGE1/ROOM11F/ROOM11F10.bmp"
out = sys.argv[1]; kante = float(sys.argv[2]); zelle = int(sys.argv[3]) if len(sys.argv) > 3 else 3
welche = sys.argv[4] if len(sys.argv) > 4 else "beide"
# Mittelpunkte = Mitte der gruenlichen Glasflaeche, am Original-Hintergrund vermessen (Dossier 3.6):
# oben (222.5, 75.5), unten (222.5, 135.5). Gezeichnet wird GENAU nach der Regel des Bauplans
# (Dossier 5.1 Schritt 3): ganzzahlige Ecke x0 = round(mitte - kante/2 + 0.5) (22 px -> 212/65/125),
# Texel u = i*32/kante, v = j*32/kante (naechstes Texel), Texel 0 aus, B+F je Kanal gesaettigt.
MITTEN = {"oben": (222.5, 75.5), "unten": (222.5, 135.5)}
d = open(TIM, "rb").read()
bl, cx_, cy_, cw, ch = struct.unpack_from("<IHHHH", d, 8)
cluts = [[struct.unpack_from("<H", d, 8 + 12 + (p * cw + k) * 2)[0] for k in range(cw)] for p in range(ch)]
o = 8 + bl
bl2, pxx, pyy, pw, ph = struct.unpack_from("<IHHHH", d, o)
pix = d[o + 12:o + bl2]
def texel(u, v): return (pix[v * pw * 2 + u // 2] >> (4 * (u & 1))) & 15
U0 = {3: 96, 4: 128}[zelle]; S = 32
im = Image.open(BGP).convert("RGB"); p = im.load()
namen = ["oben", "unten"] if welche == "beide" else ([] if welche == "keine" else [welche])
for n in namen:
    mx, my = MITTEN[n]
    k = int(round(kante))
    x0 = int(math.floor(mx - k / 2.0 + 0.5)); y0 = int(math.floor(my - k / 2.0 + 0.5))
    print("  Lampe %s: Ecke (%d,%d) Kante %d -> Pixel x %d..%d, y %d..%d" % (n, x0, y0, k, x0, x0 + k - 1, y0, y0 + k - 1))
    for j in range(k):
        for i in range(k):
            u = i * S // k; v = j * S // k
            t = cluts[2][texel(U0 + u, v)]
            if t == 0: continue          # 0x0000 = durchsichtig (GPU)
            r5, g5, b5 = t & 31, (t >> 5) & 31, (t >> 10) & 31
            R, G, B = p[x0 + i, y0 + j]
            # B+F je Kanal, bei 255 gesaettigt (Port-Framebuffer 8 Bit, Texel-Kanal << 3)
            p[x0 + i, y0 + j] = (min(255, R + (r5 << 3)), min(255, G + (g5 << 3)), min(255, B + (b5 << 3)))
im.save(out)
im.crop((190, 55, 250, 155)).resize((360, 600), Image.NEAREST).save(out.replace(".png", "_zoom.png"))
print("ok", out)
