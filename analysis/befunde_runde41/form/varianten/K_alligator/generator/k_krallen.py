#!/usr/bin/env python3
"""Variante K, Schritt 2: Krallen in Screenshot 2 lesen (Nutzer-Teile right_arm_front ohne Unterkiefer, left_arm_front) und gegen J vergleichen.
Spitzen wie form_pruefung.spitzen (Schartenhoehe >= 12 px aus dem Sattel), Farbe aus bild2.png (Video) und farbe_bild2_echt.png (Rueckrechnung, farbe.md).
Laenge in Einheiten ueber die Tiefe der Hand von J (Tiefenpuffer) und die Brennweite fx/fy aus kamera_bild2.json. Kein RE-Wert."""
import json
import math
import os
import sys

import cv2
import numpy as np

import k_kern as KK
import form_pruefung as P

form, mo, fj = KK.lade()
kam = KK.kamera(2)
fx = kam['brennweite_bild_px']['fx']
orig = cv2.imread(os.path.join(KK.FORM, 'bild2.png'))[:, :, ::-1].astype(float)
echt = cv2.imread(os.path.join(KK.FORM, 'farbe_bild2_echt.png'))[:, :, ::-1].astype(float)
logo = P.logo_maske()
tief = np.load(os.path.join(KK.K_DIR, '_lauf', '_tiefe_J_bild2.npy'))
idb = np.load(os.path.join(KK.K_DIR, '_lauf', '_id_J_bild2.npy'))
uk = KK.maske('unterkiefer')
Y = orig @ P.YK


def lese(name, m, wurzel_oben=True):
    q = P.groesste(m)
    e = P.enden(q)
    _q, oben, unten, _l = e
    wurzel = oben if wurzel_oben else unten
    sp = P.spitzen(q, wurzel)
    dt = cv2.distanceTransform(q.astype(np.uint8), cv2.DIST_L2, 5)
    aus = []
    for (y, x, ue) in sp:
        if (y, x) == wurzel:
            continue
        # Zacke: Pixel im Umkreis ue um die Spitze innerhalb der Maske, Achse = Spitze -> Schwerpunkt
        yy, xx = np.mgrid[0:480, 0:640]
        r = min(ue if math.isfinite(ue) else 30, 30)
        zone = q & ((yy - y) ** 2 + (xx - x) ** 2 <= r * r)
        cy, cx = yy[zone].mean(), xx[zone].mean()
        rich = math.degrees(math.atan2(y - cy, x - cx))
        breite = 2 * float(dt[zone].max())
        t = tief[max(y - 6, 0):y + 7, max(x - 6, 0):x + 7]
        t = t[np.isfinite(t)]
        d = float(np.median(t)) if len(t) else float(np.median(tief[q & np.isfinite(tief)]))
        kern = zone & ~logo
        farbe = orig[kern].mean(0) if kern.any() else None
        farbe_e = echt[kern].mean(0) if kern.any() else None
        hand = q & ~zone & ~logo & (dt > 4)
        aus.append(dict(spitze=[x, y], ueberragt_px=round(ue, 1) if math.isfinite(ue) else None, richtung_grad_bild=round(rich, 1),
                        breite_px=round(breite, 1), tiefe=round(d), laenge_einheiten=round((ue if math.isfinite(ue) else r) * d / fx),
                        unter_logo=bool(logo[max(y - 2, 0):y + 3, max(x - 2, 0):x + 3].any()),
                        Y_spitze=round(float(Y[kern].mean()), 1) if kern.any() else None, Y_hand=round(float(Y[hand].mean()), 1) if hand.any() else None,
                        rgb_video=[round(float(c)) for c in farbe] if farbe is not None else None,
                        rgb_echt=[round(float(c)) for c in farbe_e] if farbe_e is not None else None))
    return dict(teil=name, px=int(q.sum()), wurzel=[wurzel[1], wurzel[0]], spitzen=aus,
                rgb_hand_video=[round(float(c)) for c in orig[q & ~logo].mean(0)], rgb_hand_echt=[round(float(c)) for c in echt[q & ~logo].mean(0)])


R = {}
R['right_arm_front_ohne_unterkiefer'] = lese('right_arm_front', P.teil_roh('right_arm_front') & ~uk)
R['left_arm_front'] = lese('left_arm_front', P.teil_roh('left_arm_front'))
# J: Hand + Klauen als Maske (nur sichtbare Pixel) und dieselbe Lesung
for nm, kn in (('J_rechts', (18, 19, 20, 29, 31)), ('J_links', (13, 14, 15, 28, 30))):
    m = np.isin(idb, kn)
    R[nm] = lese(nm, m)
# J: Textur der Krallen (mittlere Farbe der Klauen-Texel im Atlas, CLUT-Zeile 1), zum Vergleich mit dem Video
import form_buehne as FB
tx = FB.Textur(open(os.path.join(KK.J_DIR, 'FORM.TIM'), 'rb').read())
bau = json.load(open(os.path.join(KK.J_DIR, 'bau.json')))
farben = {}
for a in bau['atlas']:
    if a['name'].startswith(('klaue', 'kralle', 'hand')):
        x, y, w, h = a['rechteck']
        px = tx.px[y:y + h, x:x + w]
        rgb = tx.lut[a['zeile']][px][..., :3].reshape(-1, 3).astype(float)
        farben[a['name']] = dict(mittel=[round(float(c)) for c in rgb.mean(0)], Y=round(float((rgb @ P.YK).mean()), 1),
                                 Y_p90=round(float(np.percentile(rgb @ P.YK, 90)), 1))
R['J_textur'] = farben
json.dump(R, open(os.path.join(KK.K_DIR, 'krallen_lesung.json'), 'w', encoding='utf-8'), indent=1)
for k, v in R.items():
    if k == 'J_textur':
        for a, b in v.items():
            print('  Textur J %-10s %s Y %.0f p90 %.0f' % (a, b['mittel'], b['Y'], b['Y_p90']))
        continue
    print(k, 'px', v['px'], 'Wurzel', v['wurzel'], 'Hand Video', v['rgb_hand_video'], 'echt', v['rgb_hand_echt'])
    for s in v['spitzen']:
        print('   ', s)
