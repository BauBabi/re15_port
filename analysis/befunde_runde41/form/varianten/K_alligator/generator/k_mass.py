#!/usr/bin/env python3
"""Variante K, Zusatzmasse zu form_pruefung (Bild 2): Ober-/Unterkiefer und Krallen gegen die Nutzer-Teile.

    python k_mass.py AUS.json "J=../../J_dicker" "K1=../K1_lang" ...

Je Variante (Tiefenpuffer aus k_kern, Pose Bild 2 des Kandidaten):
 - Oberkiefer: IoU sichtbarer Kopf (Knochen 22) gegen maske_oberkiefer; dazu IoU ohne den uebrigen Oberkoerper (face_and_upper_body ohne die Kiefer-Teile
   zaehlt dort auf keiner Seite: dunkel auf dunkel); Schwerpunkt- und Achsabweichung (form_pruefung.achse)
 - Unterkiefer: IoU sichtbarer Kiefer (23) gegen maske_unterkiefer, Schwerpunkt, Achse
 - rechter Arm ohne Unterkiefer: IoU Knochen 17,18,19,20,29,31 gegen right_arm_front ohne Unterkiefer
 - Krallen: Spitzen (form_pruefung.spitzen, >= 12 px) der sichtbaren Hand mit Fingern gegen die Spitzen der Handschnitte; Paare nach Abstand,
   mittlerer Abstand, Zahl ohne Partner. Kein RE-Wert."""
import json
import math
import sys

import numpy as np

import k_kern as KK
import form_pruefung as P

HAND_R = (17, 18, 19, 20, 29, 31)
HAND_L = (12, 13, 14, 15, 28, 30)


def id2(d):
    form, mo, fj = KK.lade(d)
    s = fj['bild2']
    R, T = mo.pose(KK.kf_von(mo, s['clip'], s['bild']))
    return KK.raster(KK.kamera(2), *KK.tris_welt(form, *KK.welt_pose(R, T, s['lage'], s['gier'])))[1]


def achs_vergleich(ist, soll):
    a, b = P.achse(ist), P.achse(soll)
    if a is None or b is None:
        return None
    w = abs(a['grad'] - b['grad']) % 180
    return dict(schwerpunkt_abstand_px=round(float(np.linalg.norm(a['mitte'] - b['mitte'])), 1), achse_ist_grad=round(a['grad'], 1), achse_soll_grad=round(b['grad'], 1),
                achse_abweichung_grad=round(min(w, 180 - w), 1), laenge_ist_soll_px=[round(a['laenge']), round(b['laenge'])], breite_ist_soll_px=[round(a['breite']), round(b['breite'])])


def spitzen_von(m, wurzel_oben=True):
    q = P.groesste(m)
    if q is None:
        return []
    e = P.enden(q)
    w = e[1] if wurzel_oben else e[2]
    return [(y, x) for (y, x, ue) in P.spitzen(q, w)] + [w]


def krallen_vergleich(ist_m, soll_m):
    si, ss = spitzen_von(ist_m), spitzen_von(soll_m)
    paare, frei = [], list(range(len(si)))
    for d, a, b in sorted((math.dist(s, t), a, b) for a, s in enumerate(ss) for b, t in enumerate(si)):
        if b in frei and a not in [p[0] for p in paare] and d <= 40:
            paare.append((a, b, d))
            frei.remove(b)
    return dict(spitzen_soll=len(ss), spitzen_ist=len(si), paare=len(paare), abstand_mittel_px=round(float(np.mean([p[2] for p in paare])), 1) if paare else None,
                soll_ohne_partner=len(ss) - len(paare), ist_ohne_partner=len(frei), soll=[[x, y] for y, x in ss], ist=[[x, y] for y, x in si])


def messen(d):
    idb = id2(d)
    ok, uk = KK.maske('oberkiefer'), KK.maske('unterkiefer')
    rest = KK.maske('face_and_upper_body') & ~ok & ~uk
    logo = P.logo_maske()
    k22, k23 = idb == 22, idb == 23
    g = ~rest
    arm_soll = P.teil_roh('right_arm_front') & ~uk & ~logo
    arm_ist = np.isin(idb, HAND_R) & ~logo
    return dict(oberkiefer=dict(iou=round(KK.iou(k22, ok), 3), iou_ohne_rumpf=round(KK.iou(k22 & g, ok & g), 3), deckung=round(float((k22 & ok).sum()) / ok.sum(), 3),
                                lage=achs_vergleich(k22, ok)),
                unterkiefer=dict(iou=round(KK.iou(k23, uk), 3), deckung=round(float((k23 & uk).sum()) / uk.sum(), 3), lage=achs_vergleich(k23, uk)),
                rechter_arm_ohne_unterkiefer=dict(iou=round(KK.iou(arm_ist, arm_soll), 3)),
                krallen_rechts=krallen_vergleich(np.isin(idb, HAND_R) & ~uk, P.teil_roh('right_arm_front') & ~uk),
                krallen_links=krallen_vergleich(np.isin(idb, HAND_L), P.teil_roh('left_arm_front')))


if __name__ == '__main__':
    R = {}
    for sp in sys.argv[2:]:
        nm, d = sp.split('=', 1)
        R[nm] = m = messen(d)
        o, u = m['oberkiefer'], m['unterkiefer']
        print('%-4s Oberkiefer IoU %.3f (ohne Rumpf %.3f) Lage %s | Unterkiefer IoU %.3f Lage %s | Arm r ohne UK %.3f | Krallen r %s l %s' % (
            nm, o['iou'], o['iou_ohne_rumpf'], o['lage'] and (o['lage']['schwerpunkt_abstand_px'], o['lage']['achse_abweichung_grad']),
            u['iou'], u['lage'] and (u['lage']['schwerpunkt_abstand_px'], u['lage']['achse_abweichung_grad']), m['rechter_arm_ohne_unterkiefer']['iou'],
            (m['krallen_rechts']['paare'], m['krallen_rechts']['spitzen_soll'], m['krallen_rechts']['abstand_mittel_px']),
            (m['krallen_links']['paare'], m['krallen_links']['spitzen_soll'], m['krallen_links']['abstand_mittel_px'])))
    json.dump(R, open(sys.argv[1], 'w', encoding='utf-8'), indent=1)
