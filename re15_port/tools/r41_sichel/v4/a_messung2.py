# Leser A V4 / L2 + L5: Endpunkt-Strukturen, Kopfboxen, Abstaende. Alle Zahlen in 640x480-Bildpixeln.
# Aufruf: python a_messung2.py <scratch-dir>   (braucht frames1300.npy, a_l1.json, a_messung_teil1.json)
import sys, json, numpy as np, cv2
from PIL import Image
from skimage.color import rgb2lab
O = sys.argv[1]; V = 'C:/workspace/git/reAi_v2/Version_80%/'
SW = 17.0
I1 = np.array(Image.open(V + 'Beschreibung1_Vorlage.png').convert('RGB')).astype(float)  # = n1484
I2 = np.array(Image.open(V + 'Beschreibung2_Vorlage.png').convert('RGB')).astype(float)  # = n1434
L1 = json.load(open(O + '/a_l1.json', encoding='utf8'))
out = json.load(open(O + '/a_messung_teil1.json'))
kk = np.array(out['abblendung_1484']['k_rgb'])


def lab(rgb):
    return rgb2lab(np.clip(np.array(rgb, float) / 255.0, 0, 1).reshape(1, 1, 3)).reshape(3)


def stats(img, mask, fade=False):
    px = img[mask]; n = len(px)
    if n == 0:
        return None
    raw = px.mean(0); sd = px.std(0)
    sub = np.clip((px - SW) * 255.0 / (255.0 - SW), 0, 255)
    labs = rgb2lab(np.clip(px / 255.0, 0, 1).reshape(-1, 1, 3)).reshape(-1, 3)
    d = dict(n=int(n), rgb=raw.round(1).tolist(), rgb_std=sd.round(1).tolist(), lab=lab(raw).round(1).tolist(),
             lab_std=labs.std(0).round(1).tolist(), rgb_sw=sub.mean(0).round(1).tolist(), lab_sw=lab(sub.mean(0)).round(1).tolist())
    if fade:
        fc = SW + (px - SW) / kk; fcs = np.clip((fc - SW) * 255.0 / (255.0 - SW), 0, 255)
        d.update(rgb_entblendet=fc.mean(0).round(1).tolist(), lab_entblendet=lab(fc.mean(0)).round(1).tolist(),
                 rgb_entblendet_sw=fcs.mean(0).round(1).tolist(), lab_entblendet_sw=lab(fcs.mean(0)).round(1).tolist())
    return d


def erode(m, e=2):
    return cv2.erode(m.astype(np.uint8), np.ones((2 * e + 1, 2 * e + 1), np.uint8)) > 0


H = {'bild1': dict(n=1484, oben=138, sohle=442, H=442 - 138,
                   quelle_oben='Spalten x388-394: erste Zeile mit |n1484 - Median(n1350..1473)*k| > 45 (Summe RGB); Minimum y138 bei x390/392',
                   quelle_sohle='Beinspalte x372-384 hebt sich bis y=442 vom Schlagschatten (x400-410) ab (Lum 23-25 gegen 22); darunter nicht aufloesbar (Unsicherheit +6/-3)'),
     'bild2': dict(n=1434, oben=186, sohle=439, H=439 - 186,
                   quelle_oben='Spalten x380-392: erste Zeile mit |n1434 - n1360| > 60 (Summe RGB); Minimum y186',
                   quelle_sohle='Sohle in n1434 verdeckt (gruener Vordergrund); Standfuss rot (R-G>=10, x420-451) Unterkante y=439 in n1440/1442/1444 (n1438: 437), gleiche Kamera; Oberkante n1440 = 185 (n1434: 186)')}
out['H'] = H
IMG = {'bild1': I1, 'bild2': I2}


def ort(img, p, fade):
    x, y = p; r = {}
    Lm = img.mean(2)
    for w in (9, 15):
        h = w // 2
        m = np.zeros((480, 640), bool); m[y - h:y + h + 1, x - h:x + h + 1] = True
        me = np.zeros((480, 640), bool); me[y - h + 2:y + h - 1, x - h + 2:x + h - 1] = True
        r['f%d' % w] = dict(fenster=[x - h, y - h, x + h, y + h], voll=stats(img, m, fade),
                            erodiert2=dict(fenster=[x - h + 2, y - h + 2, x + h - 2, y + h - 2], **stats(img, me, fade)))
    core = Lm[y - 1:y + 2, x - 1:x + 2].mean()
    ring = np.ones((15, 15), bool); ring[4:11, 4:11] = False
    ringv = Lm[y - 7:y + 8, x - 7:x + 8][ring].mean()
    r['kern3_lum'] = round(float(core), 1); r['ring15_lum'] = round(float(ringv), 1)
    r['hell_dunkel'] = 'heller als Umgebung' if core > ringv + 3 else ('dunkler als Umgebung' if core < ringv - 3 else 'wie Umgebung (|d|<=3)')
    thr = (core + ringv) / 2; sub = Lm[y - 15:y + 16, x - 15:x + 16]
    bm = (sub > thr) if core > ringv else (sub < thr)
    n, lab_, st, cen = cv2.connectedComponentsWithStats(bm.astype(np.uint8), connectivity=4)
    c = lab_[15, 15]
    if c == 0:
        ys, xs = np.where(lab_ > 0); i = np.argmin((ys - 15) ** 2 + (xs - 15) ** 2); c = lab_[ys[i], xs[i]]
    ys, xs = np.where(lab_ == c); pts = np.c_[xs, ys].astype(np.int32)
    hull = cv2.convexHull(pts); ha = cv2.contourArea(hull)
    r['blob'] = dict(schwelle_lum=round(float(thr), 1), pixel=int(len(xs)),
                     bbox=[int(xs.min() + x - 15), int(ys.min() + y - 15), int(xs.max() + x - 15), int(ys.max() + y - 15)],
                     breite=int(xs.max() - xs.min() + 1), hoehe=int(ys.max() - ys.min() + 1), huelle_flaeche=round(float(ha), 1),
                     schwerpunkt=[round(float(xs.mean() + x - 15), 1), round(float(ys.mean() + y - 15), 1)],
                     randberuehrung_31=bool(xs.min() == 0 or ys.min() == 0 or xs.max() == 30 or ys.max() == 30),
                     huelle=(hull.reshape(-1, 2) + [x - 15, y - 15]).tolist())
    bmask = np.zeros((480, 640), bool); bmask[ys + y - 15, xs + x - 15] = True
    np.save(O + '/blob_%d_%d.npy' % (x, y), bmask)
    r['blob_farbe'] = stats(img, bmask, fade); r['blob_farbe_erodiert1'] = stats(img, erode(bmask, 1), fade)
    return r


lin = []
for f, bild in [('Beschreibung1', 'bild1'), ('Beschreibung2', 'bild2')]:
    for z in L1[f]['zuordnung']:
        p = tuple(z['endpunkt_spitze'])
        o = ort(IMG[bild], p, bild == 'bild1')
        lin.append(dict(bild=bild, text=z['text'], ende=list(p), ende_skelett=z['endpunkt_skelett'], linien_start=z['linien_start'],
                        belegung=z['belegung'], belegung_poly=z.get('belegung_poly'), beste_alternative=z['beste_alternative'],
                        belegung_alternative=z['belegung_alternative'], belegung_alternative_poly=z.get('belegung_alternative_poly'), ort=o))
out['linien'] = lin

# ---------- Kopf (L5) ----------
f = I1.mean(2)
row = f[165, 355:436]; left = 355 + int(np.argmin(row[:20]))
col = f[178:200, 381]; chin = 178 + int(np.argmin(col))
nase_x = L1['Beschreibung1']['zuordnung'][2]['endpunkt_spitze'][0]
right = 2 * nase_x - left
kopf1 = dict(box=[left, 138, right, chin],
             regel='links = Lum-Minimum Zeile y165 in x355..374; oben = H1-Oberkante y138; unten = Lum-Minimum Spalte x381 in y178..199 (dunkler Spalt unter dem Kopf); rechts = linke Kante an der Nase (x=%d) gespiegelt, weil die rechte Kopfkante (B) im dunklen Haarfeld nicht aufloest' % nase_x,
             zeile165_x355=[int(v) for v in row], spalte381_y178=[int(v) for v in col])
kopf1['breite'] = right - left; kopf1['hoehe'] = chin - 138; kopf1['b_zu_h'] = round(kopf1['breite'] / kopf1['hoehe'], 3)
f2 = I2.mean(2)
row2 = f2[244, 400:471]
front = 400 + 55 + int(np.argmin(row2[55:70]))
colr = (I2[..., 0] - I2[..., 1])[180:230, 428]; top2 = 180 + int(np.argmax(colr >= 12))
band = f2[236:292, 438:460].mean(1); plateau = float(np.median(band[34:])); unten2 = 236 + 12 + int(np.argmax(band[12:] <= plateau + 1))
row3 = f2[236, 400:430]; back = 405 + int(np.argmin(row3[5:20]))
kopf2 = dict(box=[back, top2, front, unten2],
             regel='vorn = Lum-Minimum Zeile y244 in x455..469 (dunkle Vorderkante; Hintergrund dort in n1360/n1452 56-60, also Wesen); oben = erste Zeile R-G>=12 in Spalte x428 (rote Zone); hinten = Lum-Minimum Zeile y236 in x405..419; unten = erste Zeile ab y248, in der das Zeilenmittel x438..459 das Plateau (Median y270..291) + 1 erreicht; keine Kinnkante, nur Helligkeitsabfall -> unscharf',
             zeile244_x400=[int(v) for v in row2], spalte450_y185=[int(v) for v in f2[185:291, 450]])
kopf2['breite'] = front - back; kopf2['hoehe'] = unten2 - top2; kopf2['b_zu_h'] = round(kopf2['breite'] / kopf2['hoehe'], 3)
out['kopf'] = dict(vorn=kopf1, seite=kopf2)


def rel(p, box, Hpx):
    x0, y0, x1, y1 = box
    return dict(x_rel=round((p[0] - x0) / (x1 - x0), 3), y_rel=round((p[1] - y0) / (y1 - y0), 3),
                dx_mitte_px=round(p[0] - (x0 + x1) / 2, 1), dy_mitte_px=round(p[1] - (y0 + y1) / 2, 1),
                dx_mitte_H=round((p[0] - (x0 + x1) / 2) / Hpx, 4), dy_mitte_H=round((p[1] - (y0 + y1) / 2) / Hpx, 4))


for l in lin:
    if l['bild'] == 'bild1' or l['text'].startswith(('Auge', 'Haare')):
        b = kopf1['box'] if l['bild'] == 'bild1' else kopf2['box']; Hpx = H[l['bild']]['H']
        l['kopf_rel_ende'] = rel(l['ende'], b, Hpx); l['kopf_rel_blob'] = rel(l['ort']['blob']['schwerpunkt'], b, Hpx)
P = {l['text']: np.array(l['ende'], float) for l in lin if l['bild'] == 'bild1'}
paare = {}
for a in P:
    for b_ in P:
        if a < b_:
            d = P[b_] - P[a]
            paare['%s->%s' % (a, b_)] = dict(dx=float(d[0]), dy=float(d[1]), abstand_px=round(float(np.hypot(*d)), 1), abstand_H=round(float(np.hypot(*d)) / H['bild1']['H'], 4))
P2 = {l['text']: np.array(l['ende'], float) for l in lin if l['bild'] == 'bild2'}
a, b_ = 'Auge von der Seite etwa hier', 'Haare von der Seite'; d = P2[b_] - P2[a]
paare['seite Auge->Haare'] = dict(dx=float(d[0]), dy=float(d[1]), abstand_px=round(float(np.hypot(*d)), 1), abstand_H=round(float(np.hypot(*d)) / H['bild2']['H'], 4))
out['paare'] = paare
json.dump(out, open(O + '/a_messung.json', 'w'), indent=1, ensure_ascii=False)
for l in lin:
    o = l['ort']
    print(l['bild'], l['text'], l['ende'], o['hell_dunkel'], o['kern3_lum'], o['ring15_lum'])
    print('   blob', {k: v for k, v in o['blob'].items() if k != 'huelle'})
    print('   f9e', o['f9']['erodiert2']['rgb'], o['f9']['erodiert2']['lab'], 'f15e', o['f15']['erodiert2']['rgb'], o['f15']['erodiert2']['lab'])
    print('   blob rgb', o['blob_farbe']['rgb'], o['blob_farbe']['lab'], 'sw', o['blob_farbe']['lab_sw'], 'entbl', o['blob_farbe'].get('rgb_entblendet'))
    print('   rel', l.get('kopf_rel_ende'), l.get('kopf_rel_blob'))
print('kopf', {k: {kk_: v for kk_, v in d.items() if kk_ in ('box', 'breite', 'hoehe', 'b_zu_h')} for k, d in out['kopf'].items()})
print(json.dumps(paare))
