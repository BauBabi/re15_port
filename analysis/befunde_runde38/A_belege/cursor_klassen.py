"""cursor_klassen.py - Runde 38 Spur A: Cursor-Pixel eines Bildes nach den fuenf Paletten-Klassen des
11F0-Cursor-TIMs (ROOM11F0.RDT TIM @0x018DAC, CLUT 256x1: 1 = (0,88,0) dunkelgruen [Schatten der Ecken],
2 = (64,56,0) dunkelgelb [Schatten des Kreuzes], 3 = (8,248,0) gruen, 4 = (216,208,0) gelb, 5 = (240,248,136) weiss).
Die Klasse eines Bildpunkts = naechster Paletteneintrag nach Farbton (Licht moduliert die Helligkeit), sonst '.'.
Aufruf: python cursor_klassen.py <bild> [x0 y0 x1 y1 in LOGISCHEN Pixeln] [--skala N] [--ascii]
Mit --skala N wird jeder logische Pixel als NxN-Block gelesen (Fensterskalierung des Framedumps) und
jeder physische Bildpunkt einzeln klassifiziert; Ausgabe = Zaehlung je Klasse (physisch)."""
import sys
from PIL import Image

KL = {1: (0, 88, 0), 2: (64, 56, 0), 3: (8, 248, 0), 4: (216, 208, 0), 5: (240, 248, 136)}
ZEICHEN = {1: 'g', 2: 'y', 3: 'G', 4: 'Y', 5: 'W', 0: '.'}


def klasse(p):
    r, g, b = p
    # Cursor-Farben: Blau ~0 (ausser Weiss), Gruen oder Rot+Gruen dominieren.
    if b > 60 and not (r > 150 and g > 150):
        return 0
    if r > 150 and g > 150 and b > 90:
        return 5
    if g > 140 and r < 90 and b < 60:
        return 3
    if r > 140 and g > 130 and b < 60:
        return 4
    if g >= 50 and r <= 20 and b <= 20 and g - r >= 40:
        return 1
    if r >= 40 and g >= 32 and b <= 16 and r - b >= 30 and g - b >= 25 and abs(r - g) <= 24:
        return 2
    return 0


def main():
    a = [x for x in sys.argv[1:] if not x.startswith('--')]
    skala = 1
    if '--skala' in sys.argv:
        skala = int(sys.argv[sys.argv.index('--skala') + 1])
        a = [x for x in a if x != str(skala)] if a.count(str(skala)) == 1 and len(a) > 5 else a
    im = Image.open(a[0]).convert('RGB')
    x0, y0, x1, y1 = (int(v) for v in a[1:5]) if len(a) >= 5 else (140, 100, 180, 138)
    px = im.load()
    zaehl = {k: 0 for k in ZEICHEN}
    zeilen = []
    for y in range(y0 * skala, y1 * skala):
        z = ''
        for x in range(x0 * skala, x1 * skala):
            k = klasse(px[x, y])
            zaehl[k] += 1
            z += ZEICHEN[k]
        zeilen.append('%4d %s' % (y, z))
    if '--ascii' in sys.argv:
        print('\n'.join(zeilen))
    print('Klassen (physische Pixel, Skala %d): G=%d Y=%d W=%d | Schatten g=%d y=%d | Schattenanteil %.1f %%' % (
        skala, zaehl[3], zaehl[4], zaehl[5], zaehl[1], zaehl[2],
        100.0 * (zaehl[1] + zaehl[2]) / max(1, zaehl[1] + zaehl[2] + zaehl[3] + zaehl[4] + zaehl[5])))


if __name__ == '__main__':
    main()
