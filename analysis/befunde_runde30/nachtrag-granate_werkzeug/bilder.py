# -*- coding: utf-8 -*-
"""Nachtrag K: die ausgelieferten Bilder der Granaten-Ids 0x09/0x0A/0x0B.

  Item-Bild  ITEM/ITPS.ITP @id*0x3000 (Lader LAB_8001e404: @0x8001e414 ori 0x3000,
             @0x8001e450-58 id*6 Sektoren), TIM 8bpp 112x72, eingebettete Rechtecke
  Icon       DATA/ITEMALL.PIX Tile id @id*1200, 40x30 8bpp, CLUT DATA/ST_00.TIM Zeile 0
Ausgabe: build/r30_n_granate/bilder_*.png (4x)
"""
import os, sys, struct
from PIL import Image
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'sicherung_werkzeug'))
from r30_lib import PSX, REPO  # noqa
import item_bilder as IB  # noqa

ZIEL = os.path.join(REPO, 'build', 'r30_n_granate')
os.makedirs(ZIEL, exist_ok=True)
itps = open(os.path.join(PSX, 'ITEM', 'ITPS.ITP'), 'rb').read()
pix = open(os.path.join(PSX, 'DATA', 'ITEMALL.PIX'), 'rb').read()
st00 = open(os.path.join(PSX, 'DATA', 'ST_00.TIM'), 'rb').read()
re2 = open(os.path.join(REPO, 'info', 're2leon', 'COMMON', 'DATA', 'ITPS.ITP'), 'rb').read()
re2pix_p = os.path.join(REPO, 'info', 're2leon', 'COMMON', 'DATA', 'ITEMALL.PIX')
re2pix = open(re2pix_p, 'rb').read() if os.path.exists(re2pix_p) else b''
for iid in (0x09, 0x0A, 0x0B):
    blk = itps[iid * 0x3000:(iid + 1) * 0x3000]
    clen, cx, cy, cw, ch = struct.unpack_from('<IHHHH', blk, 8)
    plen, px_, py_, pw, ph = struct.unpack_from('<IHHHH', blk, 8 + clen)
    img, kopf = IB.itps_bild(itps, iid)
    IB.auf_grau(img, 4).save(os.path.join(ZIEL, 'bilder_itps_%02x_x4.png' % iid))
    ic = IB.icon_bild(pix, st00, iid)
    IB.auf_grau(ic, 8).save(os.path.join(ZIEL, 'bilder_icon_%02x_x8.png' % iid))
    tile = pix[iid * 1200:(iid + 1) * 1200]
    gleich_re2 = [k for k in range(len(re2) // 0x3000) if re2[k * 0x3000:(k + 1) * 0x3000] == blk]
    gleich_re2i = [k for k in range(len(re2pix) // 1200) if re2pix[k * 1200:(k + 1) * 1200] == tile]
    print('Id 0x%02X  ITPS @0x%05X crect (%d,%d) %dx%d prect (%d,%d) %dhw x %d   bytegleich RE2-Block: %s   Icon @0x%05X  !=0: %d/1200  bytegleich RE2-Tile: %s'
          % (iid, iid * 0x3000, cx, cy, cw, ch, px_, py_, pw, ph, gleich_re2 or '-', iid * 1200,
             sum(1 for b in tile if b), gleich_re2i or '-'))
    IB.statistik('itps %02x' % iid, img)
    IB.statistik('icon %02x' % iid, ic)
print('Blocks 0x09/0x0A gleich:', itps[9 * 0x3000:10 * 0x3000] == itps[10 * 0x3000:11 * 0x3000])
print('Tiles 0x09/0x0A gleich:', pix[9 * 1200:10 * 1200] == pix[10 * 1200:11 * 1200])
