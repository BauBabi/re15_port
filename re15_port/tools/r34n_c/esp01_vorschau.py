#!/usr/bin/env python3
"""Spur C (Runde 34 Nacht, Auflage 2): den RE1.5-Kandidaten ESP 0x01 gruen (ROOM5060) zeigen —
(1) so, wie RE1.5 ihn selbst setzt (ROOM5060 sub06 @0x03124 `3a 00 01 08 00 01 00 0d 9c ff 6a fa c8 9c 00 00`
    = Id 0x01, Unterindex 0x08 -> Strom-Gruppe 0 / CLUT-Zeile 1 (gruen), Eigner-Kategorie 0 (absolut),
    scale16 0x0D00, Lage (-100,-1430,-25400), Cut 0x0B aus @0x030F2 `29 0b`);
    Groesse w = S*scale16*H/(SZ*256) wie platform/pc/main.c:337-360 (FUN_800534c4), S = Satz-Byte 3 = 0x18,
    Zelle 0 (Satz 0 @0x04928 `00 01 01 18`), additiv (ABR 1 wie Row-Routine 10 @0x800176d8-ec);
(2) auf den Generator-Lampen ROOM11F10.bmp — Groesse nach DERSELBEN Herleitung wie die RE2-Variante:
    Sprite(Bild) x (Lampenoeffnung 11F0 / bedeckte Leuchte im Herkunftsbild), dazu Mitte = Glasmitte.
Aufruf: esp01_vorschau.py <ausgabe-praefix> [kante_11f0]"""
import os, sys, struct, math
from PIL import Image
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools", "tor"))
import tor_kamera as tk
HAUPT = "C:/workspace/git/reAi_v2"
RDT = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE5", "ROOM5060.RDT")
BG5060 = HAUPT + "/extracted/PSX/STAGE5/ROOM506/ROOM50611.bmp"
BG11F0 = HAUPT + "/extracted/PSX/STAGE1/ROOM11F/ROOM11F10.bmp"
out = sys.argv[1]
d = open(RDT, "rb").read()
idh, pe, tb, te = struct.unpack_from("<4I", d, 0x4C)
k = list(d[idh:idh + 8]).index(0x01)
tim = tb + struct.unpack_from("<I", d, te - 4 * (k + 1))[0]
bl, cx_, cy_, cw, ch = struct.unpack_from("<IHHHH", d, tim + 8)
cl = [[struct.unpack_from("<H", d, tim + 20 + (p * cw + i) * 2)[0] for i in range(cw)] for p in range(ch)]
o = tim + 8 + bl
bl2, pxx, pyy, pw, ph = struct.unpack_from("<IHHHH", d, o)
pix = d[o + 12:o + bl2]
def texel(u, v): return (pix[v * pw * 2 + u // 2] >> (4 * (u & 1))) & 15
S = 24; ZELLE_U = [0, 24, 48, 72, 96]
def zeichne(im, mx, my, kante, zelle, zeile):
    p = im.load(); k = int(round(kante))
    x0 = int(math.floor(mx - k / 2.0 + 0.5)); y0 = int(math.floor(my - k / 2.0 + 0.5))
    for j in range(k):
        for i in range(k):
            u = i * S // k; v = j * S // k
            t = cl[zeile][texel(ZELLE_U[zelle] + u, v)]
            if t == 0: continue
            if not (0 <= x0 + i < im.width and 0 <= y0 + j < im.height): continue
            r5, g5, b5 = t & 31, (t >> 5) & 31, (t >> 10) & 31
            R, G, B = p[x0 + i, y0 + j]
            p[x0 + i, y0 + j] = (min(255, R + (r5 << 3)), min(255, G + (g5 << 3)), min(255, B + (b5 << 3)))
    return x0, y0, k
# (1) ROOM5060 Cut 11
cam = tk.kamera(d, 11)
sx, sy, vz = cam.bild([-100, -1430, -25400])
w = S * 0x0D00 * cam.H / (float(vz) * 256.0)
print("ROOM5060 Cut 11: H=%d  Leuchte (-100,-1430,-25400) -> Bild (%.1f, %.1f) SZ=%.0f  Kante %.1f px" % (cam.H, sx, sy, vz, w))
im = Image.open(BG5060).convert("RGB")
x0, y0, kk = zeichne(im, float(sx), float(sy), w, 0, 1)
print("  gezeichnet: Ecke (%d,%d) Kante %d" % (x0, y0, kk))
im.save(out + "_room5060_cut11.png")
cx0, cy0 = int(sx) - 40, int(sy) - 40
im.crop((cx0, cy0, cx0 + 80, cy0 + 80)).resize((320, 320), Image.NEAREST).save(out + "_room5060_zoom.png")
Image.open(BG5060).convert("RGB").crop((cx0, cy0, cx0 + 80, cy0 + 80)).resize((320, 320), Image.NEAREST).save(out + "_room5060_zoom_ohne.png")
# (2) ROOM11F0 Cut 10, beide Lampen
kante = float(sys.argv[2]) if len(sys.argv) > 2 else w
im = Image.open(BG11F0).convert("RGB")
for (mx, my) in ((222.5, 75.5), (222.5, 135.5)):
    print("  11F0 Lampe Mitte (%.1f,%.1f): Ecke/Kante %s" % ((mx, my) + (zeichne(im, mx, my, kante, 0, 1),)))
im.save(out + "_11f0.png")
im.crop((190, 55, 250, 155)).resize((360, 600), Image.NEAREST).save(out + "_11f0_zoom.png")
print("ok")
