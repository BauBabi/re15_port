#!/usr/bin/env python3
"""Spur E (Runde 37): Vorschau der Code-Panel-Lampen auf den RE1.5-Panel-Cuts (Python-Nachbau des Zeichners).

Kunst  = RE2 ESP 0x16 (shared_assets/RE2/LAMPE2130.TIM, bytegleich info/re2leon/PL0/RDT/room2110/esp16.tim).
Zellen = effect.esp ESP-0x16-Block @+0x70 (u, dx/dy): 0 (0,-16) 1 (32,-16) 2 (64,-16) 3 (96,-16) 4 (128,-16)
         5 (160,-12, S 24) 6 (184,-16) 7 (216,-16); Anim-Saetze @+0x10.
Mischung = ABR 1 (B+F, gesaettigt), Index 0 durchsichtig (wie platform/pc/src/panel_lampen_pc.c).
Aufruf: lampen_vorschau.py <bg-verzeichnis mit ROOMxxxcc.ppm> <ausgabe.png> <modell: einzel|paar> <kante_rund> <kante_quadrat>
"""
import os, sys, struct
from PIL import Image
HERE = os.path.dirname(os.path.abspath(__file__))
TIM = os.path.join(HERE, "..", "..", "shared_assets", "RE2", "LAMPE2130.TIM")
d = open(TIM, "rb").read()
bl = struct.unpack_from("<I", d, 8)[0]; o = 8 + bl
pw, ph = struct.unpack_from("<HH", d, o + 8); pix = d[o + 12:]
cl = [[struct.unpack_from("<H", d, 20 + (r * 16 + k) * 2)[0] for k in range(16)] for r in range(4)]
ZELLE = {0: (0, 16, 32), 3: (96, 16, 32), 5: (160, 12, 24), 6: (184, 16, 32), 7: (216, 16, 32)}
def texel(u, v): return (pix[v * pw * 2 + u // 2] >> (4 * (u & 1))) & 15

def lampe(im, mx, my, zelle, zeile, kante_s32):
    u0, mitte, S = ZELLE[zelle]
    k = kante_s32 * S / 32.0           # Kante dieser Zelle (S 24 -> 3/4)
    x0 = mx - mitte * k / S; y0 = my - mitte * k / S
    p = im.load()
    for yy in range(int(y0), int(y0 + k) + 1):
        for xx in range(int(x0), int(x0 + k) + 1):
            if not (0 <= xx < im.size[0] and 0 <= yy < im.size[1]): continue
            u = int((xx + 0.5 - x0) * S / k); v = int((yy + 0.5 - y0) * S / k)
            if not (0 <= u < S and 0 <= v < S): continue
            t = cl[zeile][texel(u0 + u, v)]
            if t == 0: continue
            r, g, b = p[xx, yy]
            p[xx, yy] = (min(255, r + ((t & 31) << 3)), min(255, g + (((t >> 5) & 31) << 3)),
                         min(255, b + (((t >> 10) & 31) << 3)))

def main():
  bgdir, aus, modell = sys.argv[1], sys.argv[2], sys.argv[3]
  k_rund, k_quad = float(sys.argv[4]), float(sys.argv[5])
  BLAU, GRUEN = 1, 2
  # gemalte Lampen (Mitte), gemessen an ROOM10D11 / ROOM11E15 (Farbmasken, s. Dossier)
  D10_ROT, D10_BLAU = (121.5, 76.5), (168.5, 76.5)
  SG_ROT, SG_GELB, SG_GRUEN = (128.5, 174.5), (155.0, 174.5), (180.5, 174.5)
  reihen = []
  for n, rund in (("ROOM10D11", True), ("ROOM11E15", False)):
      for zustand in ("blau", "gruen"):
          im = Image.open(os.path.join(bgdir, n + ".ppm")).convert("RGB")
          if rund:
              pos = D10_BLAU if (zustand == "blau" or modell == "einzel") else D10_ROT
              lampe(im, pos[0], pos[1], 7, BLAU if zustand == "blau" else GRUEN, k_rund)
          else:
              pos = SG_GRUEN if (zustand == "gruen" or modell == "einzel") else SG_ROT
              lampe(im, pos[0], pos[1], 3, BLAU if zustand == "blau" else GRUEN, k_quad)
          box = (70, 30, 230, 120) if rund else (90, 120, 230, 220)
          reihen.append(im.crop(box).resize(((box[2]-box[0])*3, (box[3]-box[1])*3), Image.NEAREST))
  W = Image.new("RGB", (max(r.size[0] for r in reihen) * 2, sum(r.size[1] for r in reihen[::2])), (255, 255, 255))
  y = 0
  for i in range(0, len(reihen), 2):
      W.paste(reihen[i], (0, y)); W.paste(reihen[i + 1], (reihen[i].size[0], y)); y += reihen[i].size[1]
  W.save(aus); print(aus, W.size)

if __name__ == "__main__":
  main()
