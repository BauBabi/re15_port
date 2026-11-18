# Bildtafeln je Blatt und Figur aus den FB_SHOT-Abzuegen (echtes Spiel). Je Raum ein Abzug, in Durchlauf-Reihenfolge.
# Aufruf: tafel.py <auswertung.json> <ausordner> <messbild 0/1> [schussordner]
import sys, os, json, collections
from PIL import Image, ImageDraw, ImageFont
S = os.path.dirname(os.path.abspath(__file__))
erg = json.load(open(sys.argv[1])); AUS = sys.argv[2]; MESS = sys.argv[3] == '1'
SCH = sys.argv[4] if len(sys.argv) > 4 else os.path.join(S, 'schuss')
os.makedirs(AUS, exist_ok=True)
NAME = {0: 'Police Station B1', 1: 'Police Station B2', 2: 'Police Station 1F', 3: 'Police Station 2F', 4: 'Police Station 3F',
        5: 'Police Station Roof', 6: 'Drains B2', 7: 'Factory', 8: 'Laboratory B1', 9: 'Laboratory B2', 10: 'Laboratory B3',
        11: 'Laboratory B4', 12: 'Subway'}
STAGE = {0: 1, 1: 1, 2: 1, 3: 1, 4: 1, 5: 1, 6: 2, 7: 3, 8: 4, 9: 5, 10: 5, 11: 5, 12: 6}
FIG = {'L': 'Leon', 'E': 'Elza'}
f = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 17)
fb = ImageFont.truetype('C:/Windows/Fonts/consolab.ttf', 22)
grp = collections.OrderedDict()
for r in erg:
    if r['urteil'] == 'KEIN ABZUG' or not r.get('erw'): continue
    pg = r['blatt'] if (r['urteil'] != 'ok' and r.get('blatt') is not None) else r['erw'][0]
    if pg not in NAME: continue
    k = (r['tag'][0], pg)
    g = grp.setdefault(k, collections.OrderedDict())
    if r['rid'] not in g or (g[r['rid']]['urteil'] != 'ok' and r['urteil'] == 'ok'): g[r['rid']] = r   # je Raum der erste gute Abzug
SK, SP, MAXT = 2, 4, 16
X0, Y0, X1, Y1 = 0, 0, 320, 240
tw, th = (X1 - X0) * SK, (Y1 - Y0) * SK
liste = []
for (fig, pg), g in sorted(grp.items(), key=lambda kv: (kv[0][0] != 'L', kv[0][1])):
    rs = list(g.values())
    teile = [rs[i:i + MAXT] for i in range(0, len(rs), MAXT)]
    for ti, teil in enumerate(teile):
        zeilen = (len(teil) + SP - 1) // SP
        W, H = SP * (tw + 12) + 12, 92 + zeilen * (th + 36)
        im = Image.new('RGB', (W, H), (24, 24, 32)); d = ImageDraw.Draw(im)
        for i, r in enumerate(teil):
            cx, cy = 12 + (i % SP) * (tw + 12), 92 + (i // SP) * (th + 36)
            src = Image.open(os.path.join(SCH, r['tag'], 'karte.bmp')).convert('RGB')
            im.paste(src.crop((X0, Y0, X1, Y1)).resize((tw, th), Image.NEAREST), (cx, cy))
            if r['marker']:
                mx, my = r['marker']
                hx, hy = cx + (mx - X0) * SK, cy + (my - Y0) * SK
                d.ellipse([hx - 18, hy - 18, hx + 18, hy + 18], outline=(255, 0, 255), width=3)
            HINW = {'KEIN MARKER': 'FEHLT: kein Punkt', 'KEIN ROT': 'FEHLT: Raum nicht rot'}
            zus = '' if r['urteil'] == 'ok' else '  ' + HINW.get(r['urteil'], r['urteil'])
            d.text((cx + 2, cy + th + 6), 'STAGE%d %s  Raum %s  %s%s' % (STAGE[pg], NAME[pg], r['rid'], FIG[fig], zus),
                   fill=(255, 255, 200) if not zus else (255, 110, 110), font=f)
        titel = 'Karte STAGE%d - %s (Blatt %d) - %s%s' % (STAGE[pg], NAME[pg], pg, FIG[fig],
                                                         ' - Teil %d/%d' % (ti + 1, len(teile)) if len(teile) > 1 else '')
        unter = 'Durchlauf: Raeume in Reihenfolge, vorher betretene sind aufgedeckt. Rot = aktueller Raum. Pinker Kreis = nur Suchhilfe, nicht im Spiel.'
        if MESS:
            d.rectangle([0, 0, W, 50], fill=(160, 0, 0)); d.text((10, 12), 'Messbild - kein Ergebnis   ' + titel, fill=(255, 255, 255), font=fb)
        else:
            d.rectangle([0, 0, W, 50], fill=(40, 40, 70)); d.text((10, 12), titel, fill=(255, 255, 255), font=fb)
        d.text((10, 60), unter, fill=(200, 200, 200), font=f)
        name = 'tafel_%02d_%s%s.png' % (pg, FIG[fig].lower(), '_%d' % (ti + 1) if len(teile) > 1 else '')
        im.save(os.path.join(AUS, name), optimize=True)
        liste.append((name, len(teil)))
for n, k in liste: print(n, k)
