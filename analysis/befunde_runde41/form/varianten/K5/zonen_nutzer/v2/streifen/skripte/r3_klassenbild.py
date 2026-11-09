"""Runde 3, Befund 11: Kopf + Unterkiefer (+ Hals) im Knochenraum des Kopfes, eingefaerbt nach der Lage-Klasse aus r3_kugel.py
(Rueckseiten weggelassen wie im Spiel, Tiefenpuffer). Fuer die Tafel. Aufruf: python r3_klassenbild.py <name der r3_kugel-Messung> <clip> <bild>"""
import json
import sys
import cv2
from s_basis import *  # noqa: F401,F403
import r3_lage as RL
import r3_kopfbild as KB

FARBE = {'sechs': (215, 25, 35), 'z30_aussen': (245, 150, 0), 'z30_innen': (40, 110, 230), 'z30_verdeckt': (40, 110, 230),
         'maulhoehle': (196, 160, 235), 'zahn': (250, 248, 225), 'kopf': (175, 190, 175), 'kiefer': (160, 178, 200), 'hals': (222, 222, 212)}


def faerber(kl):
    def gruppe(f, b):
        if f in RL.SECHS:
            return 'sechs'
        if f in RL.Z30:
            return 'z30_aussen' if kl[f] == 'aussen' else 'z30_innen'
        if f in KB.ZAEHNE:
            return 'zahn'
        if b in (22, 23) and kl.get(f) == 'Maulhoehle':
            return 'maulhoehle'
        return {22: 'kopf', 23: 'kiefer', 21: 'hals'}[b]
    return gruppe


if __name__ == '__main__':
    name, clip, bild = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
    kl = {f: e['klasse'] for f, e in json.load(open(ZIEL + '/messung/r3_kugel_%s.json' % name, encoding='utf-8'))['je_flaeche'].items()}
    KB.gruppe = faerber(kl); KB.FARBE = FARBE
    k = kand(ASSET); kf = k.kf(clip, bild)
    for teile, tn in (((22, 23), 'kopf_kiefer'), ((21, 22, 23), 'mit_hals')):
        K, teil, schl = KB.kopfraum(k.mo, kf, teile)
        for an, (bl, ob) in KB.ANSICHTEN.items():
            img, idb, _xy, _a = KB.ansicht(K, teil, schl, bl, ob, B=600, rand=30)
            cv2.imwrite(AUS + '/r3_klasse_%s_%s_%s.png' % (name, tn, an.split(' ')[1]), img[..., ::-1])
    print('ok')
