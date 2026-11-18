#!/usr/bin/env python3
"""Runde 42 Strang "gegenstaende": Item-Bilder (ITPS, 112x72) der drei Kapseln 0x1E/0x1F/0x20 -> engine/src/gen/gegenstaende_r42_bilder.inc

Nacharbeit (gelb G5/G7): die .inc traegt nur noch die drei Inventar-Kacheln (je 1200 B) und die CLUT; das 112x72-Bild baut
engine/src/gegenstaende_r42.c (kachel_zu_bild) zur Laufzeit nach GENAU der Regel von bild() unten. So leitet ULTIMATE 1.5 dasselbe
Bild aus der Disc-Kachel ab (eine Bildquelle je Modus), und der PC-Bau traegt 4 KB statt 24 KB (die PSX zeichnet kein ITPS-Bild).

    python re15_port/tools/r42_gegenstaende/kapselbilder.py [--check] [--messbild <png>]

Warum: Im Mod-Baum tragen die ITPS-Bloecke 0x1E..0x20 nur Capcoms Platzhalter (仮キャラ/"BOSS"), auf der 40-%-Disc ebenso
(GEGENSTAENDE.md [B7]). Die INVENTAR-Kacheln derselben Ids (DATA/ITEMALL.PIX, 40x30, Kachel = Id) zeigen dagegen je eine Kapsel.
Vorgabe (Auftrag Strang): "Neue Bilder (Kapseln) nur aus Original-Material ableiten" -> das Bild ist die Inventar-Kachel selbst,
2-fach vergroessert (naechster Nachbar, keine neuen Farben), mittig auf die Hintergrundfarbe der Kachel gesetzt.
Formate (re15_ultimate15.h, selbst gemessen dort): ITEMALL-Kachel i = Bytes i*1200..+1200, 8bpp, Farben = ST_00.TIM CLUT-Zeile 0;
ITPS-Block = Id*0x3000, CLUT-Daten +0x14 (256 x RGB555), Bild 112x72 8bpp ab +0x220.
"""
import os, struct, sys
HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
PSX = os.path.join(PORT, 'shared_assets', 'PSX')
ZIEL = os.path.join(PORT, 'engine', 'src', 'gen', 'gegenstaende_r42_bilder.inc')
IDS = (0x1E, 0x1F, 0x20)
W, H, TW, TH, S = 112, 72, 40, 30, 2


def clut_st00():
    d = open(os.path.join(PSX, 'DATA', 'ST_00.TIM'), 'rb').read()
    assert struct.unpack_from('<I', d, 0)[0] == 0x10 and struct.unpack_from('<I', d, 4)[0] & 8
    n, x, y, w, h = struct.unpack_from('<IHHHH', d, 8)
    assert w == 256, w
    return d[20:20 + 512]


def bild(pix, i):
    t = pix[i * 1200:(i + 1) * 1200]
    hg = t[0]                                   # Ecke oben links = Hintergrund der Kachel
    out = bytearray([hg]) * (W * H)
    ox, oy = (W - TW * S) // 2, (H - TH * S) // 2
    for y in range(TH * S):
        for x in range(TW * S):
            out[(oy + y) * W + ox + x] = t[(y // S) * TW + x // S]
    return bytes(out), hg


def main():
    pix = open(os.path.join(PSX, 'DATA', 'ITEMALL.PIX'), 'rb').read()
    clut = clut_st00()
    out = ['/* Generiert von re15_port/tools/r42_gegenstaende/kapselbilder.py - nicht von Hand aendern. Item-Bilder der Kapseln',
           ' * 0x1E/0x1F/0x20 = Inventar-Kachel (DATA/ITEMALL.PIX Kachel = Id, 40x30) 2-fach, mittig (zur Laufzeit, gegenstaende_r42.c);',
           ' * CLUT = ST_00.TIM Zeile 0. Nur PC (#ifdef RE15_PLATFORM_PC in gegenstaende_r42.c). */',
           'static const uint8_t k_g42_kapsel_clut[512] = {']
    for i in range(0, 512, 16):
        out.append('    ' + ' '.join('0x%02x,' % b for b in clut[i:i + 16]))
    out.append('};')
    bilder = {}
    for i in IDS:
        b, hg = bild(pix, i)
        bilder[i] = b
        t = pix[i * 1200:(i + 1) * 1200]
        out.append('/* Item 0x%02X: Kachel @ITEMALL.PIX 0x%05X (40x30), Hintergrund-Index %d; Bild = kachel_zu_bild() */' % (i, i * 1200, hg))
        out.append('static const uint8_t k_g42_kachel_%02x[%d] = {' % (i, len(t)))
        for k in range(0, len(t), 24):
            out.append('    ' + ','.join('%d' % v for v in t[k:k + 24]) + ',')
        out.append('};')
    out.append('/* Mass der Laufzeit-Regel (= bild() in kapselbilder.py) */')
    out.append('#define K_G42_BILD_W %d' % W)
    out.append('#define K_G42_BILD_H %d' % H)
    out.append('#define K_G42_KACHEL_W %d' % TW)
    out.append('#define K_G42_KACHEL_H %d' % TH)
    out.append('#define K_G42_FAKTOR %d' % S)
    text = chr(10).join(out) + chr(10)
    if '--messbild' in sys.argv:
        from PIL import Image, ImageDraw
        pal = [((v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3) for v in struct.unpack('<256H', clut)]
        m = Image.new('RGB', (3 * (W * 3 + 10) + 10, H * 3 + 40), (40, 40, 40)); dr = ImageDraw.Draw(m)
        dr.rectangle([0, 0, m.width, 20], fill=(160, 0, 0)); dr.text((6, 5), 'Messbild - kein Ergebnis  (Kapsel-Bilder 112x72 x3)', fill=(255, 255, 255))
        for k, i in enumerate(IDS):
            im = Image.new('RGB', (W, H)); im.putdata([pal[v] for v in bilder[i]])
            m.paste(im.resize((W * 3, H * 3), Image.NEAREST), (10 + k * (W * 3 + 10), 30))
        m.save(sys.argv[sys.argv.index('--messbild') + 1])
    if '--check' in sys.argv:
        alt = open(ZIEL, encoding='utf-8').read() if os.path.exists(ZIEL) else ''
        print('gleich' if alt == text else 'VERALTET'); sys.exit(0 if alt == text else 1)
    open(ZIEL, 'w', newline=chr(10)).write(text)
    print('geschrieben', ZIEL)


if __name__ == '__main__':
    main()
