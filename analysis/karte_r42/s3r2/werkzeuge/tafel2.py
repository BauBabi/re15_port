# Bildtafeln Stufe 3 Runde 2: je Blatt und Figur ALLE Abzuege (jede Lage je Raum), Raeume in Durchlauf-Reihenfolge.
# Beschriftung: Bereich (STAGE aus der RAUMNUMMER), Blattname, Raum, Figur, Art der Lage. Abweichung = rote Schrift.
# Aufruf: tafel2.py <auswertung.json> <ausordner> <messbild 0/1> <schussordner>
import sys, os, json, collections
from PIL import Image, ImageDraw, ImageFont
erg = json.load(open(sys.argv[1])); AUS = sys.argv[2]; MESS = sys.argv[3] == '1'; SCH = sys.argv[4]
os.makedirs(AUS, exist_ok=True)
NAME = {0: 'Police Station B1', 1: 'Police Station B2', 2: 'Police Station 1F', 3: 'Police Station 2F', 4: 'Police Station 3F',
        5: 'Police Station Roof', 6: 'Drains B2', 7: 'Factory', 8: 'Laboratory B1', 9: 'Laboratory B2', 10: 'Laboratory B3',
        11: 'Laboratory B4', 12: 'Subway'}
FIG = {'L': 'Leon', 'E': 'Elza'}
HINW = {'NEBEN DEM ROTEN RAUM': 'Punkt NEBEN dem roten Raum', 'RAUM NICHT ROT': 'Raum nicht rot', 'KEIN PUNKT': 'kein Punkt'}
f = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 16)
fb = ImageFont.truetype('C:/Windows/Fonts/consolab.ttf', 22)
grp = collections.OrderedDict()
for r in erg:
    if r['urteil'] == 'KEIN ABZUG' or r.get('blatt') is None: continue
    grp.setdefault((r['tag'][0], r['blatt']), []).append(r)
SK, SP, MAXT = 2, 4, 16
tw, th = 320 * SK, 240 * SK
liste = []
for (fig, pg), rs in sorted(grp.items(), key=lambda kv: (kv[0][0] != 'L', kv[0][1])):
    teile = [rs[i:i + MAXT] for i in range(0, len(rs), MAXT)]
    for ti, teil in enumerate(teile):
        zeilen = (len(teil) + SP - 1) // SP
        W, H = SP * (tw + 12) + 12, 92 + zeilen * (th + 54)
        im = Image.new('RGB', (W, H), (24, 24, 32)); d = ImageDraw.Draw(im)
        for i, r in enumerate(teil):
            cx, cy = 12 + (i % SP) * (tw + 12), 92 + (i // SP) * (th + 54)
            src = Image.open(os.path.join(SCH, r['tag'], 'karte.bmp')).convert('RGB')
            im.paste(src.resize((tw, th), Image.NEAREST), (cx, cy))
            if r.get('marker'):
                hx, hy = cx + r['marker'][0] * SK, cy + r['marker'][1] * SK
                d.ellipse([hx - 18, hy - 18, hx + 18, hy + 18], outline=(255, 0, 255), width=3)
            st = 'STAGE%s' % r['rid'][0]
            art = ('gelaufen ab Tuer aus %s' if r['geh'] else 'Ankunft aus %s') % r['aus']
            ok = r['urteil'] == 'ok'
            z1 = '%s %s  Raum %s  %s' % (st, NAME[pg], r['rid'], FIG[fig])
            z2 = '%s, Lage %s' % (art, tuple(r['lage']) if r.get('lage') else '?')
            if not ok: z2 += '  ' + HINW.get(r['urteil'], r['urteil'])
            farbe = (255, 255, 200) if ok else (255, 110, 110)
            d.text((cx + 2, cy + th + 4), z1, fill=farbe, font=f)
            d.text((cx + 2, cy + th + 24), z2, fill=farbe, font=f)
        titel = 'Karte - %s (Blatt %d) - %s%s' % (NAME[pg], pg, FIG[fig], ' - Teil %d/%d' % (ti + 1, len(teile)) if len(teile) > 1 else '')
        unter = ('Echte Lagen: Ankunft durch eine Tuer, oder von dort 2 s gelaufen. Rot = Raum, in dem die Figur steht; '
                 'gruen = schon besucht. Pinker Kreis = Suchhilfe, nicht im Spiel.')
        if MESS:
            d.rectangle([0, 0, W, 50], fill=(160, 0, 0)); d.text((10, 12), 'Messbild - kein Ergebnis   ' + titel, fill=(255, 255, 255), font=fb)
        else:
            d.rectangle([0, 0, W, 50], fill=(40, 40, 70)); d.text((10, 12), titel, fill=(255, 255, 255), font=fb)
        d.text((10, 60), unter, fill=(200, 200, 200), font=f)
        name = 'tafel_%02d_%s%s.png' % (pg, FIG[fig].lower(), '_%d' % (ti + 1) if len(teile) > 1 else '')
        im.save(os.path.join(AUS, name), optimize=True)
        liste.append((name, len(teil), sum(1 for r in teil if r['urteil'] != 'ok')))
for n, k, b in liste: print(n, k, 'abweichend', b)
