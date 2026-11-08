"""Leser A: Zahlenvergleich RE2-Koepfe (G1/G2/G3) und Ref2 gegen KI-Bilder und Beschreibung1/2.
    python re15_port/tools/r41_sichel/re2kopf/mess.py OUTDIR      (nach re2_kopf.py und vorlagen_kopf.py)
Masse:
  dE00 je Zone (Kuppe / links-Mitte / rechts-Mitte / Maul / gesamt), Kandidat flach (Texelfarbe) und mit Licht, gegen
      KI-Mittel (3 Front-Koepfe), KI-Seite (3), Beschreibung1, Beschreibung2.
  Zaehne: Anteil heller, ungesaettigter Pixel (L* > 60, Chroma < 18) in der Maulzone.
  Haar: Anteil brauner Pixel (Farbwinkel 30-75 Grad in Lab, Chroma > 12, L* 25-65) im ganzen Kopf.
  Augen KI: helle Flecken (L* > 70) oberhalb y 0,6 der Kopfbox, Schwerpunkt in Kopfbox-Anteilen.
  Ref2-Figurhoehe (Pose Clip 0 Bild 0) -> Zielkopfhoehe K10 (0,187 H) in Einheiten.
"""
import os, sys, json
import numpy as np, cv2
from PIL import Image
from skimage.color import rgb2lab, deltaE_ciede2000
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import re2_leser as RL, emd_ansichtsblatt as EA
sys.path.insert(0, os.path.dirname(HIER)); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import birkin_buehne as BB

OUT = sys.argv[1]
K = json.load(open(os.path.join(OUT, 'koepfe.json')))
V = json.load(open(os.path.join(OUT, 'vorlagen.json')))
ZONEN = ['kuppe', 'links_mitte', 'rechts_mitte', 'maul', 'gesamt']


def de(a, b):
    return float(deltaE_ciede2000(np.array(a[:3], float).reshape(1, 1, 3), np.array(b[:3], float).reshape(1, 1, 3))[0, 0])


def lab_img(rgb):
    return rgb2lab(np.asarray(rgb, float) / 255.0)


def zaehne(rgb, box, maske=None):
    x0, y0, x1, y1 = box; h = y1 - y0 + 1
    L = lab_img(rgb)
    sub = L[int(y0 + .65 * h):y1 + 1, x0:x1 + 1]
    m = np.ones(sub.shape[:2], bool) if maske is None else maske[int(y0 + .65 * h):y1 + 1, x0:x1 + 1]
    c = np.hypot(sub[..., 1], sub[..., 2])
    z = (sub[..., 0] > 60) & (c < 18) & m
    return round(float(z.sum() / max(1, m.sum())), 4), int(z.sum())


def haar(rgb, box, maske=None):
    x0, y0, x1, y1 = box
    L = lab_img(rgb)[y0:y1 + 1, x0:x1 + 1]
    m = np.ones(L.shape[:2], bool) if maske is None else maske[y0:y1 + 1, x0:x1 + 1]
    c = np.hypot(L[..., 1], L[..., 2]); hue = np.degrees(np.arctan2(L[..., 2], L[..., 1])) % 360
    hb = (hue > 30) & (hue < 75) & (c > 12) & (L[..., 0] > 25) & (L[..., 0] < 65) & m
    return round(float(hb.sum() / max(1, m.sum())), 4)


def augen_hell(rgb, box):
    x0, y0, x1, y1 = box; w, h = x1 - x0 + 1, y1 - y0 + 1
    L = lab_img(rgb)[y0:y1 + 1, x0:x1 + 1]
    m = (L[..., 0] > 70).astype(np.uint8); m[int(.6 * h):] = 0
    n, lab_, st, cen = cv2.connectedComponentsWithStats(m, connectivity=8)
    fl = [dict(px=int(st[i, 4]), x_anteil=round(cen[i][0] / w, 3), y_anteil=round(cen[i][1] / h, 3)) for i in range(1, n) if st[i, 4] >= 3]
    return sorted(fl, key=lambda d: -d['px'])[:4]


res = dict(dE00={}, zaehne={}, haar={}, augen_ki={}, zonen_ki_mittel={})
# KI-Mittel je Zone (Lab-Mittel der drei Bilder)
for art, keys in (('front', ['ki1_front', 'ki2_front', 'ki3_front']), ('rechts', ['ki1_rechts', 'ki2_rechts', 'ki3_rechts'])):
    res['zonen_ki_mittel'][art] = {z: [round(float(np.mean([V[k]['zonen'][z][i] for k in keys])), 1) for i in range(3)] for z in ZONEN}
ziele = dict(ki_front=res['zonen_ki_mittel']['front'], ki_rechts=res['zonen_ki_mittel']['rechts'],
             beschreibung1=V['beschreibung1']['zonen'], beschreibung2=V['beschreibung2']['zonen'])
for kand in ('em30', 'em31', 'em33', 'ref2'):
    res['dE00'][kand] = {}
    for zielname, zz in ziele.items():
        ansicht = 'rechts' if zielname in ('ki_rechts', 'beschreibung2') else 'vorn'
        for art in ('zonen_flach', 'zonen_licht'):
            res['dE00'][kand]['%s|%s' % (zielname, art)] = {z: round(de(K[kand][ansicht][art][z], zz[z]), 1) for z in ZONEN}
# Zaehne / Haar der Kandidaten (flaches Vorn-Render + Maske) und der Vorlagen
for kand in ('em30', 'em31', 'em33', 'ref2'):
    img = np.asarray(Image.open(os.path.join(OUT, kand + '_kopf_vorn_flach.png')).convert('RGB'))
    mk = np.load(os.path.join(OUT, kand + '_kopf_vorn_maske.npy'))
    box = tuple(K[kand]['vorn']['kopfbox_px'])
    res['zaehne'][kand] = zaehne(img, box, mk); res['haar'][kand] = haar(img, box, mk)
for nm in list(V):
    img = np.asarray(Image.open(os.path.join(OUT, (nm + '.png') if nm.startswith('ki') else ('beschreibung%s_kopf.png' % nm[-1]))).convert('RGB'))
    box = (0, 0, img.shape[1] - 1, img.shape[0] - 1)
    res['zaehne'][nm] = zaehne(img, box); res['haar'][nm] = haar(img, box)
    if nm.startswith('ki'):
        res['augen_ki'][nm] = augen_hell(img, box)
# Ref2 Figurhoehe und Zielkopf
ref = BB.modell(open(os.path.join(RL.BAUM, 're15_port', 'shared_assets', 'PSX', 'EMD', 'EM30_HANGAR.EMD'), 'rb').read())
kf0 = ref.frames[ref.clips[0][0]] & 0xfff
Vv, _, F = ref.geometrie(kf0)
koerper = sorted({v for f in F for v in f['v'] if f['bone'] not in range(16, 23)})
H = float(Vv[koerper][:, 1].max() - Vv[koerper][:, 1].min())
res['ref2'] = dict(H_einheiten_ohne_schwanz=round(H), pal_zeilen=int(ref.pal.shape[0]), clut_y=int(ref.clut_y),
                   kopf_ist=dict(hoehe=round(K['ref2']['vorn']['hoehe_einh']), breite=round(K['ref2']['vorn']['breite_einh']), tiefe=round(K['ref2']['rechts']['breite_einh'])),
                   kopf_ziel_K10=dict(hoehe=round(0.187 * H), breite=round(0.154 * H)),
                   kandidaten_skala_auf_K10={k: round(0.187 * H / K[k]['vorn']['hoehe_einh'], 3) for k in ('em30', 'em31', 'em33')})
json.dump(res, open(os.path.join(OUT, 'mess.json'), 'w'), indent=1)
print(json.dumps(res['zonen_ki_mittel'], indent=0))
for kand in res['dE00']:
    for k2, v in res['dE00'][kand].items():
        print(kand, k2, v)
print('zaehne', res['zaehne']); print('haar', res['haar']); print('augen_ki', json.dumps(res['augen_ki'])); print('ref2', res['ref2'])
