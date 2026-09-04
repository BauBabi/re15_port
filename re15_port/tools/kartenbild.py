# -*- coding: utf-8 -*-
"""KARTENBILD: rastert ein Kartenblatt so, wie der Spieler es sieht.

Anlass: Nutzer-Befunde werden als SCREENSHOT gemeldet ("diese Tuer hier ist falsch").
Wer die gemeldete Stelle aus Tabellenzeilen erraet, trifft daneben - deshalb wird hier
gezeichnet, was der Zeichner zeichnet, und jede Marke mit ihrem Tabellen-Index
beschriftet. Kartenpixel == Bildschirmpixel (die Rechtecke liegen bei x120..216,
y80..166 im 320x240-Bild), ein Screenshot ist also nur um den Fensterfaktor skaliert.

Aufruf:
    python re15_port/tools/kartenbild.py <blatt> [--rot <zid>] [--skala 3]
"""
import io, os, re, sys

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, '..', '..'))
DATA = os.path.join(WURZEL, 're15_port', 'shared_assets', 'PSX', 'DATA')
KOPF = os.path.join(WURZEL, 're15_port', 'engine', 'src', 're15_map_zones.h')

sys.path.insert(0, HIER)
_s = io.open(os.path.join(HIER, 'gen_map_zones.py'), encoding='utf-8').read()
_ns = {'__name__': 'kb', '__file__': os.path.join(HIER, 'gen_map_zones.py')}
exec(compile(_s[:_s.index('def main(')], 'gen', 'exec'), _ns)
rects, rect_uv = _ns['rects'], _ns['rect_uv']

KIND = {0: 'Nord', 1: 'Ost', 2: 'Sued', 3: 'West', 4: 'Treppe-waag', 5: 'Treppe-senkr'}


def _tabelle(name):
    s = io.open(KOPF, encoding='utf-8').read()
    i = s.index('s_map_%s[] = {' % name); j = s.index('};', i)
    aus = []
    for z in s[i:j].split('\n'):
        if not z.strip().startswith('{'):
            continue
        # ⛔ HEX ZUERST: '0x1140' zerfaellt sonst in '0' und '1140' und verschiebt
        # JEDE Spalte der Zeile um eins - die Zonen-Zuordnung war dadurch leer.
        m = re.findall(r'0x[0-9A-Fa-f]+|-?\d+', z.split('/*')[0])
        if m:
            aus.append([int(x, 0) for x in m])
    return aus


def kachel(pg):
    d = open(os.path.join(DATA, 'MAP%02X.PIX' % (pg + 1)), 'rb').read()
    return [[((d[y * 128 + (u >> 1)] >> 4) if (u & 1) else (d[y * 128 + (u >> 1)] & 0xF))
             for u in range(256)] for y in range(256)]


def main(argv):
    from PIL import Image, ImageDraw
    pg = int(argv[0])
    rot = None
    skala = 3
    if '--rot' in argv:
        rot = int(argv[argv.index('--rot') + 1], 0)
    if '--skala' in argv:
        skala = int(argv[argv.index('--skala') + 1])

    px = kachel(pg)
    rr = rects(pg)
    zonen = [z for z in _tabelle('zones') if z[5] == pg]
    marken = [(k, m) for k, m in enumerate(_tabelle('marks')) if m[0] == pg]
    # rect -> zid (erste Zone, die dieses Rechteck traegt)
    rect2zid = {}
    for z in zonen:
        rect2zid.setdefault(z[6], z[8])

    W = H = 256
    im = Image.new('RGB', (W * skala, H * skala), (0, 0, 48))
    d = ImageDraw.Draw(im)

    def blit(rx, ry, rw, rh, u, v, ton):
        for j in range(rh):
            for i in range(rw):
                sx, sy = u + i, v + j
                if sx > 255 or sy > 255:
                    continue
                idx = px[sy][sx]
                if idx == 0:
                    continue
                c = ton if idx == 4 else tuple(max(0, q - 60) for q in ton)
                d.rectangle([(rx + i) * skala, (ry + j) * skala,
                             (rx + i) * skala + skala - 1, (ry + j) * skala + skala - 1],
                            fill=c)

    print('== Blatt %d: %d Rechtecke, %d Marken ==' % (pg, len(rr), len(marken)))
    for i, r in enumerate(rr):
        zid = rect2zid.get(i)
        ton = (192, 24, 24) if (rot is not None and zid == rot) else (40, 176, 40)
        u, v = rect_uv(pg, i)
        blit(r[0], r[1], r[2], r[3], u, v, ton)
        print('   rect %d  x=%3d y=%3d %2dx%-2d  zid %s%s'
              % (i, r[0], r[1], r[2], r[3], zid, '  <- ROT' if ton[0] == 192 else ''))

    for k, m in marken:
        _, mrect, mx, my, kind, za, zb = m[:7]
        if kind <= 3:
            lx = (kind == 0 or kind == 2)
            x0, y0 = (mx - 2, my - 1) if lx else (mx - 1, my - 2)
            w, h = (5, 2) if lx else (2, 5)
        else:
            x0, y0, w, h = mx - 2, my - 2, 5, 5
        d.rectangle([x0 * skala, y0 * skala, (x0 + w) * skala - 1, (y0 + h) * skala - 1],
                    fill=(255, 216, 0))
        d.rectangle([(x0 - 1) * skala, (y0 - 1) * skala,
                     (x0 + w + 1) * skala - 1, (y0 + h + 1) * skala - 1],
                    outline=(0, 255, 255))
        d.text(((x0 + w + 2) * skala, (y0 - 2) * skala), '#%d' % k, fill=(255, 255, 255))
        print('   Marke #%-3d rect %d  (%3d,%3d) kind %d=%-12s zid %3d/%3d'
              '   Bildkasten x%d..%d y%d..%d'
              % (k, mrect, mx, my, kind, KIND.get(kind, '?'), za, zb,
                 x0, x0 + w - 1, y0, y0 + h - 1))

    ziel = os.path.join(WURZEL, 'shots', 'kartenbild_blatt%02d.png' % pg)
    if not os.path.isdir(os.path.dirname(ziel)):
        os.makedirs(os.path.dirname(ziel))
    im.crop((100 * skala, 60 * skala, 240 * skala, 190 * skala)).save(ziel)
    print('== geschrieben: %s (Ausschnitt x100..240 y60..190) ==' % ziel)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
