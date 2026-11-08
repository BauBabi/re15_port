# Leser A V4: Belegbilder (A_*.png) aus den Messdateien
# Aufruf: python a_belege.py <scratch-dir> <beleg-dir>
import sys, json, numpy as np, cv2
from PIL import Image, ImageDraw, ImageFont
from skimage.morphology import skeletonize
O = sys.argv[1]; BD = sys.argv[2]; V = 'C:/workspace/git/reAi_v2/Version_80%/'
try:
    FONT = ImageFont.truetype('C:/Windows/Fonts/arial.ttf', 13); FB = ImageFont.truetype('C:/Windows/Fonts/arialbd.ttf', 15)
except Exception:
    FONT = FB = ImageFont.load_default()
L1 = json.load(open(O + '/a_l1.json', encoding='utf8'))
M = json.load(open(O + '/a_messung.json', encoding='utf8'))
F = json.load(open(O + '/a_farben.json', encoding='utf8'))
S = json.load(open(O + '/schwanz.json'))
FARB = [(0, 220, 255), (60, 255, 60), (255, 0, 255), (255, 200, 0), (0, 120, 255)]
out = []
# ---------- L1: Linien nachgezeichnet ----------
for f, tag in [('Beschreibung1', 'bild1'), ('Beschreibung2', 'bild2')]:
    B = Image.open(V + f + '.png').convert('RGB'); W, Hh = B.size
    g = (np.array(B).astype(float) * 0.45 + 140).clip(0, 255).astype(np.uint8)
    im = Image.fromarray(g); im = im.resize((W * 2, Hh * 2), Image.NEAREST); d = ImageDraw.Draw(im)
    for i, z in enumerate(L1[f]['zuordnung']):
        c = FARB[i % 5]
        pts = [z['linien_start']] + ([z['geodaetisch']['knick']] if z['geodaetisch'] and 'belegung_polylinie' in z['geodaetisch'] else []) + [z['endpunkt_spitze']]
        d.line([(p[0] * 2 + 1, p[1] * 2 + 1) for p in pts], fill=c, width=3)
        e = z['endpunkt_spitze']; d.ellipse([e[0] * 2 - 6, e[1] * 2 - 6, e[0] * 2 + 8, e[1] * 2 + 8], outline=c, width=3)
        bx = z['box']; d.rectangle([bx[0] * 2 - 3, bx[1] * 2 - 3, bx[2] * 2 + 3, bx[3] * 2 + 3], outline=c, width=2)
        lab = '%d %s -> (%d,%d)  Belegung %.3f%s | beste Alternative (%d,%d) %.3f%s' % (
            i + 1, z['text'][:34], e[0], e[1], z['belegung'],
            (' (Polylinie %.3f)' % z['belegung_poly']) if z.get('belegung_poly') else '', z['beste_alternative'][0], z['beste_alternative'][1],
            z['belegung_alternative'], (' (Poly %.3f)' % z['belegung_alternative_poly']) if z.get('belegung_alternative_poly') else '')
        d.rectangle([4, Hh * 2 - 22 * (len(L1[f]['zuordnung']) - i) - 6, 1100, Hh * 2 - 22 * (len(L1[f]['zuordnung']) - i) + 14], fill=(0, 0, 0))
        d.text((8, Hh * 2 - 22 * (len(L1[f]['zuordnung']) - i) - 4), lab, fill=c, font=FONT)
    d.rectangle([0, 0, W * 2, 22], fill=(0, 0, 0))
    d.text((6, 3), 'Leser A V4 / L1 %s: Linien nach Pixelbelegung (Maske |RGB-(237,28,36)|<=40), Anzeige x2, Endpunkt = Kreis, Textblock = Rahmen' % f, fill=(255, 255, 255), font=FB)
    p = BD + '/A_L1_%s_linien.png' % tag; im.save(p); out.append(p)
# Matrix-Bild
im = Image.new('RGB', (1500, 430), (255, 255, 255)); d = ImageDraw.Draw(im); y = 8
for f in ['Beschreibung1', 'Beschreibung2']:
    m = L1[f]['matrix']; d.text((8, y), '%s: Belegung der Strecke Linienstart (am Text) -> Kandidat-Endpunkt (Anteil Abtastpunkte auf Kernrot, 3x3 aufgeweitet)' % f, fill=(0, 0, 0), font=FB); y += 22
    d.text((330, y), '   '.join('(%d,%d)' % tuple(s) for s in m['spalten']), fill=(0, 0, 0), font=FONT); y += 18
    for zl, row, z in zip(m['zeilen'], m['belegung'], L1[f]['zuordnung']):
        best = int(np.argmax(row))
        d.text((8, y), zl[:44], fill=(0, 0, 0), font=FONT)
        for j, v in enumerate(row):
            d.text((330 + j * 92, y), '%.3f' % v, fill=(200, 0, 0) if j == best else (60, 60, 60), font=FB if j == best else FONT)
        y += 18
    d.text((8, y), 'Kreuzungen der gewaehlten Strecken: %s' % (L1[f]['kreuzungen'] or 'keine'), fill=(0, 0, 0), font=FONT); y += 30
d.text((8, y), 'Bild 1 "Auge": Linie mit Knick (Polylinie (660,112)->(386,146)->(382,165)); gerade Strecke 0,387, Polylinie 1,000; eigene 8er-Zusammenhangskomponente (keine Beruehrung mit Haare/Nase).', fill=(0, 0, 0), font=FONT)
p = BD + '/A_L1_matrix.png'; im.save(p); out.append(p)
# ---------- Kopf-Ausschnitte x6 ----------
I1 = np.array(Image.open(V + 'Beschreibung1_Vorlage.png').convert('RGB')); I2 = np.array(Image.open(V + 'Beschreibung2_Vorlage.png').convert('RGB'))
IM = {'bild1': I1, 'bild2': I2}


def kopfbild(l, name, s=10, r=20):
    x, y = l['ende']; img = IM[l['bild']]
    c = img[y - r:y + r + 1, x - r:x + r + 1].astype(float)
    c = np.clip((c - 17) * 255 / 238 * 1.8, 0, 255).astype(np.uint8)   # Schwarzwert ab, x1,8 nur zur Anzeige
    t = Image.fromarray(c).resize(((2 * r + 1) * s, (2 * r + 1) * s), Image.NEAREST); d = ImageDraw.Draw(t)
    P = lambda px, py: ((px - (x - r)) * s, (py - (y - r)) * s)
    for w, col in ((9, (255, 255, 0)), (15, (0, 255, 255))):
        h = w // 2; d.rectangle([P(x - h, y - h), P(x + h + 1, y + h + 1)], outline=col, width=2)
    hull = l['ort']['blob']['huelle']; d.polygon([P(px + .5, py + .5) for px, py in hull], outline=(255, 0, 255), width=3)
    d.ellipse([P(x + .5, y + .5)[0] - 6, P(x + .5, y + .5)[1] - 6, P(x + .5, y + .5)[0] + 6, P(x + .5, y + .5)[1] + 6], outline=(255, 40, 40), width=3)
    kb = M['kopf']['vorn' if l['bild'] == 'bild1' else 'seite']['box']
    d.rectangle([P(kb[0], kb[1]), P(kb[2], kb[3])], outline=(255, 140, 0), width=2)
    o = l['ort']; b = o['blob']
    txt = ['%s  Ende (%d,%d)  %s' % (l['text'][:30], x, y, o['hell_dunkel']),
           'Kern3 %.1f / Ring15 %.1f; Blob %dx%d px, %d px, Huelle %.0f px2, Schwerpunkt (%.1f,%.1f)' % (o['kern3_lum'], o['ring15_lum'], b['breite'], b['hoehe'], b['pixel'], b['huelle_flaeche'], *b['schwerpunkt']),
           'gelb 9x9, cyan 15x15, magenta Blob-Huelle, orange Kopfbox; Anzeige: Schwarzwert ab x1,8, %dx' % s]
    d.rectangle([0, 0, t.size[0], 54], fill=(0, 0, 0))
    for i, tx in enumerate(txt): d.text((4, 2 + 17 * i), tx, fill=(255, 255, 255), font=FONT)
    p = BD + '/A_kopf_%s.png' % name; t.save(p); out.append(p)


ln = {l['text']: l for l in M['linien']}
kopfbild(ln['Auge'], 'vorn_auge'); kopfbild(ln['Haare'], 'vorn_haare'); kopfbild(ln['Nase'], 'vorn_nase')
kopfbild(ln['Auge von der Seite etwa hier'], 'seite_auge'); kopfbild(ln['Haare von der Seite'], 'seite_haare')
# Kopfboxen beide
tiles = []
for bild, k, pts in [('bild1', 'vorn', ['Auge', 'Haare', 'Nase']), ('bild2', 'seite', ['Auge von der Seite etwa hier', 'Haare von der Seite'])]:
    kb = M['kopf'][k]['box']; x0, y0, x1, y1 = kb[0] - 25, kb[1] - 20, kb[2] + 25, kb[3] + 20
    c = np.clip((IM[bild][y0:y1, x0:x1].astype(float) - 17) * 255 / 238 * 1.8, 0, 255).astype(np.uint8); s = 6
    t = Image.fromarray(c).resize(((x1 - x0) * s, (y1 - y0) * s), Image.NEAREST); d = ImageDraw.Draw(t)
    d.rectangle([(kb[0] - x0) * s, (kb[1] - y0) * s, (kb[2] - x0) * s, (kb[3] - y0) * s], outline=(255, 140, 0), width=3)
    for i, n in enumerate(pts):
        e = ln[n]['ende']; cc = FARB[i]
        d.ellipse([(e[0] - x0 + .5) * s - 7, (e[1] - y0 + .5) * s - 7, (e[0] - x0 + .5) * s + 7, (e[1] - y0 + .5) * s + 7], outline=cc, width=3)
        d.text(((e[0] - x0) * s + 10, (e[1] - y0) * s - 6), n.split(' ')[0], fill=cc, font=FB)
    d.rectangle([0, 0, t.size[0], 20], fill=(0, 0, 0))
    d.text((4, 2), 'Kopfbox %s %s: %d x %d px, B/H %.3f' % (k, kb, M['kopf'][k]['breite'], M['kopf'][k]['hoehe'], M['kopf'][k]['b_zu_h']), fill=(255, 255, 255), font=FONT)
    tiles.append(t)
Wt = sum(t.size[0] for t in tiles) + 10; Ht = max(t.size[1] for t in tiles)
im = Image.new('RGB', (Wt, Ht), (255, 255, 255)); xo = 0
for t in tiles: im.paste(t, (xo, 0)); xo += t.size[0] + 10
p = BD + '/A_kopf_boxen.png'; im.save(p); out.append(p)
# ---------- Schwanz-Polygon ----------
SM = np.load(O + '/schwanz_maske.npy')
x0, y0, x1, y1 = 250, 175, 375, 365; s = 5
c = np.clip((I2[y0:y1, x0:x1].astype(float) - 17) * 255 / 238 * 1.6, 0, 255)
mm = SM[y0:y1, x0:x1]
edge = mm & ~(cv2.erode(mm.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0)
c[edge] = [255, 0, 255]
t = Image.fromarray(c.astype(np.uint8)).resize(((x1 - x0) * s, (y1 - y0) * s), Image.NEAREST); d = ImageDraw.Draw(t)
P = lambda px, py: ((px - x0 + .5) * s, (py - y0 + .5) * s)
path = S['pfad']; d.line([P(*q) for q in path], fill=(255, 255, 0), width=2)
tp = S['text_polygon']['punkte']
d.line([P(*tp['beginn']), P(*tp['knick']), P(*tp['ende'])], fill=(255, 60, 60), width=4)
for k, q in tp.items():
    d.ellipse([P(*q)[0] - 7, P(*q)[1] - 7, P(*q)[0] + 7, P(*q)[1] + 7], outline=(255, 60, 60), width=3)
    d.text((P(*q)[0] + 9, P(*q)[1] - 8), k, fill=(255, 80, 80), font=FB)
for key, col in (('knick_zu_beginn', (0, 255, 0)), ('knick_zu_ende', (0, 200, 255))):
    for r in S[key]:
        d.line([P(*r['rand1']), P(*r['rand2'])], fill=col, width=2)
        d.text((P(*r['rand2'])[0] + 4, P(*r['rand2'])[1]), '%.1f' % r['breite_px'], fill=col, font=FONT)
for lu in S['luecken']:
    d.line([P(*lu['von']), P(*lu['bis'])], fill=(255, 255, 255), width=2)
d.rectangle([0, 0, t.size[0], 56], fill=(0, 0, 0))
for i, tx in enumerate(['Bild 2 (n1434) Schwanz: rot = Polygon aus den Nutzerlinien (Beginn -> Knick -> Ende),',
                        'magenta = Maskenrand (R-G>=5 & ~Platte | |I-Median|>=40, x<318), gelb = Maskenachse, weiss = Maskenluecke,',
                        'gruen = Breiten Knick->Beginn (0,1..0,9), cyan = Breiten Knick->Ende; Anzeige Schwarzwert ab x1,6, 5x']):
    d.text((4, 2 + 17 * i), tx, fill=(255, 255, 255), font=FONT)
p = BD + '/A_schwanz_polygon.png'; t.save(p); out.append(p)
# ---------- Farbfelder ----------
rows = [f for f in F['farben']]
im = Image.new('RGB', (1500, 26 * len(rows) + 60 + 26 * len(F['deltaE']) + 40), (255, 255, 255)); d = ImageDraw.Draw(im)
d.text((6, 4), 'Farbfelder: [roh] [Schwarzwert ab] [Bild 1: entblendet+Schwarzwert ab] - Region - RGB roh / Lab roh / Lab bereinigt / Std', fill=(0, 0, 0), font=FB)
y = 28
for f in rows:
    sw = [f['rgb'], f['rgb_sw'], f.get('rgb_entblendet_sw') or f['rgb_sw']]
    for j, cc in enumerate(sw): d.rectangle([6 + j * 44, y, 46 + j * 44, y + 22], fill=tuple(int(round(v)) for v in cc), outline=(0, 0, 0))
    lb = f.get('lab_entblendet_sw') or f['lab_sw']
    d.text((146, y + 4), '%s | %s' % (f['bild'], f['region'][:62]), fill=(0, 0, 0), font=FONT)
    d.text((700, y + 4), 'RGB %s  Lab %s  ber. %s  std %s  n=%d' % (f['rgb'], f['lab'], lb, f['rgb_std'], f['n']), fill=(0, 0, 0), font=FONT)
    y += 26
y += 10; d.text((6, y), 'Port V3 r2 (Leisten SF_belege/v3, in 640er-Koordinaten) gegen Original bereinigt: [Original bereinigt] [Port]  dE00', fill=(0, 0, 0), font=FB); y += 24
from skimage.color import lab2rgb
for e in F['deltaE']:
    for j, lb in enumerate([e['lab_original_bezug'], e['lab_port']]):
        rgb = (lab2rgb(np.array(lb, float).reshape(1, 1, 3)).reshape(3) * 255).round().astype(int)
        d.rectangle([6 + j * 44, y, 46 + j * 44, y + 22], fill=tuple(rgb.tolist()), outline=(0, 0, 0))
    d.text((100, y + 4), '%s  <->  %s   dE00 %.1f (roh %.1f), dL %.1f, C %.1f/%.1f, h %.0f/%.0f' % (e['original'][:55], e['port'][:40], e['dE00_gegen_original_bereinigt'], e['dE00_gegen_original_roh'], e['L_port_minus_original'], e['C_original'], e['C_port'], e['h_original'], e['h_port']), fill=(0, 0, 0), font=FONT)
    y += 26
p = BD + '/A_farbfelder.png'; im.save(p); out.append(p)
# Regionen auf beiden Bildern
tiles = []
for bild, img in (('bild1', I1), ('bild2', I2)):
    g = np.clip((img.astype(float) - 17) * 255 / 238 * 1.5, 0, 255).astype(np.uint8).copy()
    idx = [i for i, f in enumerate(F['farben']) if f['bild'] == bild]
    for k, i in enumerate(idx):
        m = np.load(O + '/reg_%s_%d.npy' % (bild, i + 1))
        e = m & ~(cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0)
        g[e] = [(53 * k) % 255, (255 - 37 * k) % 255, (97 * k + 80) % 255]
    t = Image.fromarray(g).resize((1280, 960), Image.NEAREST); d = ImageDraw.Draw(t)
    for k, i in enumerate(idx):
        m = np.load(O + '/reg_%s_%d.npy' % (bild, i + 1)); ys, xs = np.where(m)
        d.text((xs.min() * 2, ys.min() * 2 - 12), str(k + 1), fill=((53 * k) % 255, (255 - 37 * k) % 255, (97 * k + 80) % 255), font=FB)
        d.text((8, 8 + 16 * k), '%d %s' % (k + 1, F['farben'][i]['region'][:70]), fill=(255, 255, 255), font=FONT)
    tiles.append(t)
im = Image.new('RGB', (2570, 960), (255, 255, 255)); im.paste(tiles[0], (0, 0)); im.paste(tiles[1], (1290, 0))
p = BD + '/A_farbregionen.png'; im.save(p); out.append(p)
print('\n'.join(out))
