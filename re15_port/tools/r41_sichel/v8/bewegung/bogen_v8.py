"""Bogen h76-h102 neu: Ablesung mit EINER Methode (Augen-Agent, Pixelauswertung der Rasterblaetter gitter_bogen_h84_h96.png und
gitter_bogen_h76_h82_h98_h102.png, +-3 px Spitze/Knolle/Bogenrand, Ansatz +-8 px) statt der Linienablesung video_linien.py (h76-h100 lag
20-40 px weiter links; dort lagen h88/h92 25-30 px links vom Nutzer-Umriss h90 -> Zucken). Eichung an h90: Nutzer-Umriss Areas.png Spitze
(286,349) gegen Ablesung (283,322) -> Nadelende im Bodenschatten, Versatz (+3,+27) auf die Spitzen, deren Ablesung an der
Schattenoberkante endet (h88-h94, h98); bei h76/h80/h84/h86/h100 reicht der sichtbare Teil bis zur Bodenlinie, h78 endet ueber dem Schatten.
Pfeil = groesster Abstand von (Ansatz, linkester Bogenpunkt, Knollenende) zur Sehne Ansatz -> Spitze (+ links der Sehne im Bild).

    python bogen_v8.py  -> bogen_v8.json und Tabellenzeilen
"""
import json, numpy as np
# h: (Spitze aussen, Knollenende, linkester Bogenpunkt (mit Lichtkante, wo abgelesen), Ansatz, Ende im Bodenschatten?)
# h82: Spitze/Knolle verdeckt (dunkles Objekt im Vordergrund), h96: unsicher, Form weicht ab, h102: nicht erkennbar -> nicht in der Tabelle
A = {
    76: ((280, 341), (276, 311), (275, 258), (326, 226), False),
    78: ((278, 323), (265, 304), (260, 272), (340, 226), False),
    80: ((300, 343), (276, 315), (270, 275), (350, 222), False),
    84: ((292, 337), (281, 322), (276, 283), (350, 222), False),
    86: ((291, 336), (279, 316), (277, 282), (350, 222), False),
    88: ((284, 325), (278, 309), (277, 282), (350, 222), True),
    90: ((283, 322), (278, 303), (280, 278), (350, 222), True),
    92: ((282, 317), (279, 301), (280, 278), (350, 222), True),
    94: ((280, 317), (279, 302), (285, 282), (350, 222), True),
    98: ((269, 327), (279, 313), (298, 262), (346, 225), True),
    100: ((292, 351), (298, 325), (300, 266), (338, 222), False),
}
NUTZER90 = (286, 349)
d = (NUTZER90[0] - A[90][0][0], NUTZER90[1] - A[90][0][1])
aus = {}
for h, (sp, kn, li, an, schatten) in sorted(A.items()):
    t = (sp[0] + d[0], sp[1] + d[1]) if schatten else sp
    a = np.array(an, float); e = np.array(t, float); v = e - a
    n = np.array([v[1], -v[0]]) / np.linalg.norm(v)
    off = [float((np.array(q, float) - a) @ n) for q in (li, kn)]
    pf = max(off, key=abs)
    aus[h] = dict(spitze=[int(t[0]), int(t[1])], austritt=list(an), pfeil=round(pf, 1), schatten=schatten, ablesung=dict(spitze=sp, knolle=kn, links=li))
    print(h, aus[h])
json.dump(dict(versatz=d, je_h=aus), open('bogen_v8.json', 'w'), indent=1)
