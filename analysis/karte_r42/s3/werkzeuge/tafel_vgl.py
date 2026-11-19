# Vorher/nachher-Tafeln je Blatt (Leon, STAGE2-6): links aaea5f8c2, rechts dieser Stand; gleiche Lage, nur der aktuelle Raum besucht.
import sys, os, json, collections
from PIL import Image, ImageDraw, ImageFont
S = os.path.dirname(os.path.abspath(__file__))
erg = json.load(open(os.path.join(S, 'vergleich.json'))); AUS = sys.argv[1]; MESS = sys.argv[2] == '1'
os.makedirs(AUS, exist_ok=True)
NAME = {6: 'Drains B2', 7: 'Factory', 8: 'Laboratory B1', 9: 'Laboratory B2', 10: 'Laboratory B3', 11: 'Laboratory B4', 12: 'Subway'}
STAGE = {6: 2, 7: 3, 8: 4, 9: 5, 10: 5, 11: 5, 12: 6}
f = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 17); fb = ImageFont.truetype('C:/Windows/Fonts/consolab.ttf', 22)
X0, Y0, X1, Y1, SK = 24, 24, 296, 216, 2
tw, th = (X1 - X0) * SK, (Y1 - Y0) * SK
grp = collections.defaultdict(list)
for e in erg:
    if e['vorher'] and e['nachher'] and e['page'] in NAME: grp[e['page']].append(e)
for pg, es in sorted(grp.items()):
    PAARE = 2
    zeilen = (len(es) + PAARE - 1) // PAARE
    W = PAARE * (2 * tw + 40) + 10; H = 92 + zeilen * (th + 36)
    im = Image.new('RGB', (W, H), (24, 24, 32)); d = ImageDraw.Draw(im)
    for i, e in enumerate(es):
        cx, cy = 10 + (i % PAARE) * (2 * tw + 40), 92 + (i // PAARE) * (th + 36)
        for k, (wo, ordner) in enumerate((('vorher', 'schuss_basis'), ('nachher', 'schuss_nachher'))):
            src = Image.open(os.path.join(S, ordner, e['tag'], 'karte.bmp')).convert('RGB')
            x = cx + k * (tw + 6)
            im.paste(src.crop((X0, Y0, X1, Y1)).resize((tw, th), Image.NEAREST), (x, cy))
            m = e[wo]['marker']
            if m:
                hx, hy = x + (m[0] - X0) * SK, cy + (m[1] - Y0) * SK
                d.ellipse([hx - 18, hy - 18, hx + 18, hy + 18], outline=(255, 0, 255), width=3)
            d.text((x + 2, cy + th + 6), ('VORHER' if k == 0 else 'NACHHER') + '  Raum %s  Leon%s' % (e['rid'], '' if m else '  (kein Punkt)'),
                   fill=(255, 200, 120) if k == 0 else (160, 255, 160), font=f)
    titel = 'Karte STAGE%d - %s (Blatt %d) - Leon - vorher (v0.8.30-Stand) / nachher (r42/karte)' % (STAGE[pg], NAME[pg], pg)
    if MESS: d.rectangle([0, 0, W, 50], fill=(160, 0, 0)); d.text((10, 12), 'Messbild - kein Ergebnis   ' + titel, fill=(255, 255, 255), font=fb)
    else: d.rectangle([0, 0, W, 50], fill=(40, 40, 70)); d.text((10, 12), titel, fill=(255, 255, 255), font=fb)
    d.text((10, 60), 'Gleiche Stelle im Raum, nur dieser Raum betreten. Rot = aktueller Raum. Pinker Kreis = nur Suchhilfe, nicht im Spiel.', fill=(200, 200, 200), font=f)
    n = 'vergleich_%02d_leon.png' % pg
    im.save(os.path.join(AUS, n), optimize=True); print(n, len(es), im.size)
