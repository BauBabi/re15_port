# Leser A V4 / L4: Regionsfarben Original (n1484, n1434) und Port V3 r2 (Leisten v3, in 640er-Koordinaten zurueckgerechnet)
# Aufruf: python a_farben.py <scratch-dir> <beleg-dir>
import sys, json, numpy as np, cv2
from PIL import Image, ImageDraw
from skimage.color import rgb2lab, deltaE_ciede2000
O = sys.argv[1]; BD = sys.argv[2]; V = 'C:/workspace/git/reAi_v2/Version_80%/'
SW = 17.0
I1 = np.array(Image.open(V + 'Beschreibung1_Vorlage.png').convert('RGB')).astype(float)
I2 = np.array(Image.open(V + 'Beschreibung2_Vorlage.png').convert('RGB')).astype(float)
M1 = json.load(open(O + '/a_messung.json', encoding='utf8'))
kk = np.array(M1['abblendung_1484']['k_rgb'])
bg = np.array(Image.open(O + '/bg_median_1350_1473.png')).astype(float)
bg1 = SW + (bg - SW) * kk
SCH = json.load(open(O + '/schwanz.json')); SM = np.load(O + '/schwanz_maske.npy')
yy, xx = np.mgrid[0:480, 0:640]


def lab1(rgb):
    return rgb2lab(np.clip(np.array(rgb, float) / 255.0, 0, 1).reshape(1, 1, 3)).reshape(3)


def erode(m, e=2):
    return cv2.erode(m.astype(np.uint8), np.ones((2 * e + 1, 2 * e + 1), np.uint8)) > 0


def box(b):
    m = np.zeros((480, 640), bool); x0, y0, x1, y1 = b; m[y0:y1, x0:x1] = True; return m


def stats(img, m, fade=False):
    px = img[m]
    if len(px) == 0:
        return None
    labs = rgb2lab(np.clip(px / 255.0, 0, 1).reshape(-1, 1, 3)).reshape(-1, 3)
    sub = np.clip((px - SW) * 255.0 / (255.0 - SW), 0, 255)
    d = dict(n=int(len(px)), rgb=px.mean(0).round(1).tolist(), rgb_std=px.std(0).round(1).tolist(), lab=lab1(px.mean(0)).round(1).tolist(),
             lab_std=labs.std(0).round(1).tolist(), rgb_sw=sub.mean(0).round(1).tolist(), lab_sw=lab1(sub.mean(0)).round(1).tolist())
    if fade:
        fc = SW + (px - SW) / kk; fcs = np.clip((fc - SW) * 255.0 / (255.0 - SW), 0, 255)
        d.update(rgb_entblendet=fc.mean(0).round(1).tolist(), lab_entblendet=lab1(fc.mean(0)).round(1).tolist(),
                 rgb_entblendet_sw=fcs.mean(0).round(1).tolist(), lab_entblendet_sw=lab1(fcs.mean(0)).round(1).tolist())
    return d


RG1 = I1[..., 0] - I1[..., 1]; RG2 = I2[..., 0] - I2[..., 1]; L1m = I1.mean(2); L2m = I2.mean(2)
blob = lambda x, y: np.load(O + '/blob_%d_%d.npy' % (x, y))
lin = {l['text']: l for l in M1['linien']}
# Schwanzabschnitte (Bild 2) ueber naechsten Pfadpunkt
path = np.array(SCH['pfad'], float)  # jeder 3. Pfadpunkt
seg = np.r_[0, np.cumsum(np.hypot(*np.diff(path, axis=0).T))]
sk = SCH['s_knick'] * seg[-1] / SCH['pfad_len_px']
ys, xs = np.where(SM)
d2 = (xs[:, None] - path[None, :, 0]) ** 2 + (ys[:, None] - path[None, :, 1]) ** 2
s_of = np.zeros((480, 640)); s_of[ys, xs] = seg[np.argmin(d2, axis=1)]
SMe = erode(SM, 2); SMe1 = erode(SM, 1)
sl = seg[-1]
schwanz = {
    'Schwanz nahe Ruecken (s 0-15 % Beginn->Knick), Maske ero2': SMe & (s_of <= 0.15 * sk) & SM,
    'Schwanz breiter Teil (s 35-65 % Beginn->Knick), Maske ero2': SMe & (s_of >= 0.35 * sk) & (s_of <= 0.65 * sk),
    'Schwanz am Knick (|s-s_Knick|<=10 px), Maske ero1': SMe1 & (np.abs(s_of - sk) <= 10),
    'Schwanz Spitze (letzte 20 % Knick->Ende), Maske ero1': SMe1 & (s_of >= sk + 0.8 * (sl - sk)),
}
# Schwanz vorn (Bild 1): dunkles Band links neben dem Koerper; Pixel mit |I1-bg1|>=30 und Lum<60 in x295..352, y300..392
sv = box((295, 300, 352, 392)) & (np.abs(I1 - bg1).sum(2) >= 30) & (L1m < 60)
n_, lab_, st_, _ = cv2.connectedComponentsWithStats(sv.astype(np.uint8), connectivity=8)
big = np.argmax(st_[1:, 4]) + 1 if n_ > 1 else 0
sv = lab_ == big
R = {}
R['bild1'] = {
    'Kopf-Kuppe (Oberkopf B-links der roten Zone) box 372-388 x 147-157': box((372, 147, 389, 158)),
    'Kopf rote Zone (oben) box 389-407 x 140-156, R-G>=15': box((389, 140, 408, 157)) & (RG1 >= 15),
    'Augenumfeld box 371-391 x 157-177': box((371, 157, 392, 178)),
    'Auge (heller Fleck, Blob-Schwelle 72,1)': blob(*lin['Auge']['ende']),
    'Haare (dunkles Feld, Blob-Schwelle 31,2)': blob(*lin['Haare']['ende']),
    'Nase box 387-397 x 173-183': box((387, 173, 398, 184)),
    'unter dem Kopf/Kinn box 378-411 x 183-194': box((378, 183, 412, 195)),
    'Brust rot box 372-424 x 195-234, R-G>=15': box((372, 195, 425, 235)) & (RG1 >= 15),
    'Brust hell box 372-424 x 195-234, R-G<15 und Lum>=45': box((372, 195, 425, 235)) & (RG1 < 15) & (L1m >= 45),
    'Oberarm/Schulter B-links box 305-349 x 165-214': box((305, 165, 350, 215)),
    'Oberarm B-rechts box 435-479 x 180-229': box((435, 180, 480, 230)),
    'Unterarm B-rechts box 462-494 x 265-319': box((462, 265, 495, 320)),
    'Klaue B-rechts box 482-507 x 330-356, R-G>=15': box((482, 330, 508, 357)) & (RG1 >= 15),
    'Hand/Klaue B-links box 375-409 x 320-354': box((375, 320, 410, 355)),
    'Jeans box 372-439 x 262-299': box((372, 262, 440, 300)),
    'Schwanz vorn (Band links, |I-bg*k|>=30, Lum<60)': sv,
    'Bezug Wand links box 40-119 x 150-299': box((40, 150, 120, 300)),
    'Bezug Wand rechts box 560-629 x 230-329': box((560, 230, 630, 330)),
    'Bezug Boden box 30-199 x 380-439': box((30, 380, 200, 440)),
}
R['bild2'] = {
    'Kopf rote Zone (Kuppe oben) box 418-441 x 193-211, R-G>=25': box((418, 193, 442, 212)) & (RG2 >= 25),
    'Buckel/Ruecken hell box 372-414 x 195-227, Lum>=80': box((372, 195, 415, 228)) & (L2m >= 80),
    'Augenumfeld box 440-461 x 224-249': box((440, 224, 462, 250)),
    'Auge (heller Fleck, Blob-Schwelle 77,8)': blob(*lin['Auge von der Seite etwa hier']['ende']),
    'Haare (dunkle Vorderkante, Blob-Schwelle 56,8)': blob(*lin['Haare von der Seite']['ende']),
    'Kopf unterhalb Auge box 436-461 x 250-262': box((436, 250, 462, 263)),
    'Oberarm (nah) box 345-394 x 250-284': box((345, 250, 395, 285)),
    'Unterarm/Klaue box 340-371 x 285-321': box((340, 285, 372, 322)),
    'Jeans box 395-434 x 296-329': box((395, 296, 435, 330)),
    'Bezug Wand links box 40-119 x 150-299': box((40, 150, 120, 300)),
    'Bezug Boden box 30-199 x 380-439': box((30, 380, 200, 440)),
}
R['bild2'].update(schwanz)
ERO_NONE = ('Auge (', 'Haare (', 'Schwanz')
farben = []
for bild, regs in R.items():
    img = I1 if bild == 'bild1' else I2
    for name, m in regs.items():
        me = m if name.startswith(ERO_NONE) else erode(m, 2) & m
        if name.startswith('Schwanz vorn'):
            me = erode(m, 1) & m
        s = stats(img, me, bild == 'bild1')
        ys_, xs_ = np.where(me)
        farben.append(dict(bild=bild, region=name, fenster='Maske %d px, bbox x%d-%d y%d-%d%s' % (len(xs_), xs_.min(), xs_.max(), ys_.min(), ys_.max(),
                      '' if name.startswith(ERO_NONE) and not name.startswith('Schwanz vorn') else (', 2 px erodiert' if not name.startswith('Schwanz vorn') else ', 1 px erodiert')), **s))
        np.save(O + '/reg_%s_%d.npy' % (bild, len(farben)), me)
# ---------- Port V3 r2 ----------
def portimg(k):
    a = np.load(O + '/port_%s_V3.npy' % k); ok = np.load(O + '/port_%s_V3_ok.npy' % k)
    ov = ((a[..., 0] > 150) & (a[..., 2] > 150) & (a[..., 1] < 110)) | ((a[..., 0] > 150) & (a[..., 1] > 150) & (a[..., 2] < 90)) | ((a[..., 1] > 140) & (a[..., 0] < 110) & (a[..., 2] < 110)) | ((a[..., 1] > 150) & (a[..., 2] > 150) & (a[..., 0] < 110))
    ov = cv2.dilate(ov.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
    return a, ok & ~ov


PORT = {
    'vorn_h118': [('Kopf-Kuppe', (370, 160, 396, 172)), ('Augenumfeld', (370, 170, 390, 185)), ('Kopf B-rechts neben Auge (Haare-Lage)', (386, 168, 398, 184)),
                  ('Maul', (373, 188, 395, 203)), ('Brust rot (R-G>=30)', (395, 208, 430, 240)), ('Brust hell', (380, 250, 420, 280)),
                  ('Oberarm/Schulter B-links', (300, 195, 330, 235)), ('Unterarm B-links', (300, 265, 326, 300)), ('Jeans', (362, 300, 395, 330)),
                  ('Schwanz vorn', (322, 350, 341, 372))],
    'kopf_h85': [('Kopf oben grau (seitlich)', (442, 188, 463, 204)), ('Maul seitlich', (442, 206, 462, 216)), ('Schulterwunde rot', (408, 215, 440, 250))],
    'seite_h83': [('Buckel/Ruecken hell', (372, 190, 405, 212)), ('Oberarm (nah)', (355, 240, 385, 262)), ('Jeans', (350, 292, 372, 318)),
                  ('Schwanz nahe Ruecken', (322, 200, 345, 214)), ('Schwanz breiter Teil', (282, 215, 302, 233)), ('Schwanz Spitze', (228, 282, 241, 296))],
}
port = []
for k, regs in PORT.items():
    a, ok = portimg(k)
    for name, b in regs:
        m = box(b) & ok
        if 'R-G>=30' in name:
            m &= (a[..., 0] - a[..., 1]) >= 30
        me = erode(m, 2) & m
        px = a[me]
        if len(px) == 0:
            continue
        port.append(dict(quelle=k, region=name, box=list(b), n=int(len(px)), rgb=px.mean(0).round(1).tolist(), rgb_std=px.std(0).round(1).tolist(), lab=lab1(px.mean(0)).round(1).tolist()))
# Paare Original <-> Port fuer Delta E 2000
PAAR = [('bild1', 'Kopf-Kuppe', 'vorn_h118', 'Kopf-Kuppe'), ('bild1', 'Augenumfeld', 'vorn_h118', 'Augenumfeld'),
        ('bild1', 'Auge (heller', 'vorn_h118', 'Augenumfeld'), ('bild1', 'Haare (dunkles', 'vorn_h118', 'Kopf B-rechts'),
        ('bild1', 'unter dem Kopf', 'vorn_h118', 'Maul'), ('bild1', 'Brust rot', 'vorn_h118', 'Brust rot'), ('bild1', 'Brust hell', 'vorn_h118', 'Brust hell'),
        ('bild1', 'Oberarm/Schulter B-links', 'vorn_h118', 'Oberarm/Schulter B-links'), ('bild1', 'Jeans', 'vorn_h118', 'Jeans'),
        ('bild1', 'Schwanz vorn', 'vorn_h118', 'Schwanz vorn'),
        ('bild2', 'Kopf rote Zone', 'kopf_h85', 'Kopf oben grau'), ('bild2', 'Auge (heller', 'kopf_h85', 'Kopf oben grau'),
        ('bild2', 'Buckel/Ruecken hell', 'seite_h83', 'Buckel/Ruecken hell'), ('bild2', 'Oberarm (nah)', 'seite_h83', 'Oberarm (nah)'),
        ('bild2', 'Jeans', 'seite_h83', 'Jeans'), ('bild2', 'Schwanz nahe Ruecken', 'seite_h83', 'Schwanz nahe Ruecken'),
        ('bild2', 'Schwanz breiter Teil', 'seite_h83', 'Schwanz breiter Teil'), ('bild2', 'Schwanz Spitze', 'seite_h83', 'Schwanz Spitze')]
de = []
for b, rn, pk, pn in PAAR:
    o = next(f for f in farben if f['bild'] == b and f['region'].startswith(rn))
    p = next(f for f in port if f['quelle'] == pk and f['region'].startswith(pn))
    lo = np.array(o['lab_entblendet_sw'] if b == 'bild1' else o['lab_sw']); lr = np.array(o['lab']); lp = np.array(p['lab'])
    de.append(dict(original=b + ': ' + o['region'], port=pk + ': ' + p['region'], lab_original_bezug=lo.round(1).tolist(), lab_port=lp.round(1).tolist(),
                   dE00_gegen_original_bereinigt=round(float(deltaE_ciede2000(lo.reshape(1, 3), lp.reshape(1, 3))[0]), 1),
                   dE00_gegen_original_roh=round(float(deltaE_ciede2000(lr.reshape(1, 3), lp.reshape(1, 3))[0]), 1),
                   L_port_minus_original=round(float(lp[0] - lo[0]), 1), C_original=round(float(np.hypot(lo[1], lo[2])), 1), C_port=round(float(np.hypot(lp[1], lp[2])), 1),
                   h_original=round(float(np.degrees(np.arctan2(lo[2], lo[1])) % 360), 1), h_port=round(float(np.degrees(np.arctan2(lp[2], lp[1])) % 360), 1)))
json.dump(dict(farben=farben, port=port, deltaE=de, bezug='Bild 1 bereinigt = entblendet (k) + Schwarzwert 17 ab; Bild 2 bereinigt = Schwarzwert 17 ab'),
          open(O + '/a_farben.json', 'w'), indent=1, ensure_ascii=False)
for f in farben:
    print(f['bild'], f['region'][:60].ljust(60), f['n'], 'rgb', f['rgb'], 'lab', f['lab'], 'sw', f['lab_sw'], 'entbl_sw', f.get('lab_entblendet_sw'), 'std', f['rgb_std'])
for p in port:
    print('PORT', p['quelle'], p['region'].ljust(40), p['n'], p['rgb'], p['lab'])
for d in de:
    print('dE', d['original'][:50].ljust(50), '|', d['port'][:35].ljust(35), d['dE00_gegen_original_bereinigt'], d['dE00_gegen_original_roh'], 'dL', d['L_port_minus_original'], 'C', d['C_original'], d['C_port'], 'h', d['h_original'], d['h_port'])
