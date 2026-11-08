# Leser B: Zusatz-Belege - Schwanzfarben ohne Chroma-Auswahl (Kreis r5 um Achsstationen), Nachbarbilder mit Markierungen, H-Beleg.
import sys, json, numpy as np
from PIL import Image, ImageDraw
import b_common as C
V, S, BEL = sys.argv[1:4]
L1 = json.load(open(f'{S}/B_L1.json')); L3 = json.load(open(f'{S}/B_L3.json')); HJ = json.load(open(f'{S}/B_H.json'))
I2 = np.asarray(Image.open(f'{V}/Beschreibung2_Vorlage.png').convert('RGB')).astype(float)
yy, xx = np.mgrid[:480, :640]
out = {}
for nm, key in (('knick_nach_beginn', 'stationen_knick_nach_beginn'), ('knick_nach_ende', 'stationen_knick_nach_ende')):
    for stn in L3['maske'][key]:
        m = (xx - stn['x']) ** 2 + (yy - stn['y']) ** 2 <= 25
        s = C.stats(I2, m); s2 = C.stats(C.schwarz_ab(I2), m)
        out[f"{nm} f{stn['f']} ({stn['x']},{stn['y']})"] = dict(roh=s, schwarz_ab=s2)
        print(nm, stn['f'], (stn['x'], stn['y']), s['rgb'], s['lab'], '| -17', s2['rgb'], s2['lab'])
json.dump(out, open(f'{S}/B_L4_schwanz_kreise.json', 'w'), indent=1)
def crop(n, box, lo, hi, sc, marks, titel):
    a = C.bild(S, n); a = np.clip((a - lo) / (hi - lo) * 255, 0, 255).astype(np.uint8)
    im = Image.fromarray(a).crop(box); im = im.resize((im.width * sc, im.height * sc), Image.NEAREST); d = ImageDraw.Draw(im)
    for (x, y, c) in marks:
        X, Y = (x - box[0] + .5) * sc, (y - box[1] + .5) * sc; d.ellipse((X - 5, Y - 5, X + 5, Y + 5), outline=c, width=2)
    d.text((3, 3), titel, fill=(255, 255, 0)); return im
cols = [(0, 255, 255), (255, 255, 0), (0, 255, 0), (255, 0, 255), (60, 140, 255)]
m1 = [(z['ende'][0], z['ende'][1], cols[i]) for i, z in enumerate(L1['1']['zuordnung'])]
m2 = [(z['ende'][0], z['ende'][1], cols[i]) for i, z in enumerate(L1['2']['zuordnung'])]
ns1 = list(range(1480, 1491)); ns2 = list(range(1430, 1441))
W = Image.new('RGB', (100 * 4 * 6, 80 * 4 * 2))
for i, n in enumerate(ns1[:12]):
    W.paste(crop(n, (345, 130, 445, 210), 8, 70, 4, m1, f'n{n}' + (' = Bild 1' if n == 1485 else '')), ((i % 6) * 400, (i // 6) * 320))
W.save(f'{BEL}/B_L2_nachbarn_bild1_kopf_x4.png')
W = Image.new('RGB', (230 * 2 * 4, 190 * 2 * 3))
for i, n in enumerate(ns2):
    W.paste(crop(n, (260, 180, 490, 370), 10, 130, 2, m2, f'n{n}' + (' = Bild 2' if n == 1435 else '')), ((i % 4) * 460, (i // 4) * 380))
W.save(f'{BEL}/B_L2_nachbarn_bild2_x2.png')
# H-Beleg: Oberkante/Sohle-Linien
W = Image.new('RGB', (640 * 2, 480 * 2))
for i, (n, key) in enumerate(((1485, 'bild1'), (1476, 'bild1'), (1435, 'bild2'), (1441, 'bild2'))):
    a = np.clip(C.bild(S, n) * 1.4, 0, 255).astype(np.uint8); im = Image.fromarray(a); d = ImageDraw.Draw(im)
    h = HJ[key]; d.line([(250, h['oberkante_y']), (520, h['oberkante_y'])], fill=(0, 255, 255)); d.line([(250, h['sohle_y_median']), (520, h['sohle_y_median'])], fill=(255, 0, 255))
    for m, bl in h['fussbloecke_nachbarbilder'].items():
        if int(m) == n:
            for b in bl: d.rectangle((b['x'] - 8, b['y0'], b['x'] + 8, b['y1']), outline=(255, 255, 0))
    d.text((5, 5), f"n{n}: Oberkante y {h['oberkante_y']} (cyan, gemessen in n{h['n']}), Sohle y {h['sohle_y_median']} (magenta, Median Fussbloecke Nachbarn), H {h['H_px']} px", fill=(255, 255, 0))
    W.paste(im, ((i % 2) * 640, (i // 2) * 480))
W.save(f'{BEL}/B_H_oberkante_sohle.png')
