#!/usr/bin/env python3
"""Variante K (E20), Schritt 1: Nutzer-Teile Oberkiefer und Unterkiefer (Version_80%, Alpha-Ausschnitte) bildpunktgenau in Screenshot 2 finden
(cv2.matchTemplate TM_SQDIFF mit Alpha-Maske), als Leinwand-Masken 640x480 nach nutzer_teile/ schreiben (wie die anderen: Alpha > 200).
Lage in nutzer_teile/lage_E20.json, NICHT in lage.json: form_pruefung.py (re15_port/tools, nicht anzufassen) liest lage.json und wuerde sonst die
Berichte aller Varianten und der laufenden Stufe-3-Abnahme veraendern. Kein RE-Wert: Nutzerlesung zweier Videostandbilder."""
import json
import os
import shutil
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.normpath(os.path.join(HIER, '..', '..', '..'))
NT = os.path.join(FORM, 'nutzer_teile')


def version80():
    p = HIER
    for _ in range(12):
        q = os.path.join(p, 'Version_80%')
        if os.path.isdir(q):
            return q
        p = os.path.dirname(p)
    raise SystemExit('Version_80% nicht gefunden')


V80 = version80()
SCHWELLE = 200          # wie die vorhandenen Masken: maske_left_arm_front.png = Alpha >= 200 an der Lage (gemessen, 5607 von 5607 px)
TEILE = {'oberkiefer': ('Oberkiefer.png', 2), 'unterkiefer': ('Unterkiefer.png', 2)}
SCREEN = {2: 'vlcsnap-2026-10-31-00h29m02s830.png', 1: 'vlcsnap-2026-10-31-00h28m45s774.png'}


def finde(bild, teil):
    a = teil[:, :, 3].astype(np.float32) / 255.0
    tpl = teil[:, :, :3].astype(np.float32)
    m = np.repeat(a[:, :, None], 3, 2)
    r = cv2.matchTemplate(bild.astype(np.float32), tpl, cv2.TM_SQDIFF, mask=m)
    mn, _mx, lo, _ = cv2.minMaxLoc(r)
    return lo, mn / max(float(m.sum()), 1.0)


def main():
    lage = {}
    ov = None
    for name, (datei, n) in TEILE.items():
        bild = cv2.imread(os.path.join(V80, SCREEN[n]))
        teil = cv2.imread(os.path.join(V80, datei), cv2.IMREAD_UNCHANGED)
        (x, y), mse = finde(bild, teil)
        h, w = teil.shape[:2]
        lage[name] = dict(bild=n, skala=1.0, x=int(x), y=int(y), w=int(w), h=int(h), mse=float(mse), quelle='Version_80%/' + datei,
                          screenshot='Version_80%/' + SCREEN[n], schwelle_alpha=SCHWELLE)
        m = np.zeros((480, 640), np.uint8)
        m[y:y + h, x:x + w] = np.where(teil[:, :, 3] >= SCHWELLE, 255, 0)
        cv2.imwrite(os.path.join(NT, 'maske_%s.png' % name), m)
        shutil.copyfile(os.path.join(V80, datei), os.path.join(NT, '%s.png' % name))
        lage[name]['px'] = int((m > 0).sum())
        print('%-12s Bild %d  x %d y %d  %dx%d  Rest %.6f  %d px' % (name, n, x, y, w, h, mse, lage[name]['px']))
    # Ueberlappung mit den vorhandenen Teilen von Bild 2
    alt = json.load(open(os.path.join(NT, 'lage.json'), encoding='utf-8'))
    for name in TEILE:
        m = cv2.imread(os.path.join(NT, 'maske_%s.png' % name), 0) > 0
        ue = {}
        for t, l in alt.items():
            if l['bild'] != 2:
                continue
            q = cv2.imread(os.path.join(NT, 'maske_%s.png' % t), 0) > 0
            if (q & m).any():
                ue[t] = int((q & m).sum())
        lage[name]['ueberlappt'] = ue
        print('  %s ueberlappt %s (von %d px)' % (name, ue, int(m.sum())))
    json.dump(lage, open(os.path.join(NT, 'lage_E20.json'), 'w', encoding='utf-8'), indent=1)


if __name__ == '__main__':
    main()
