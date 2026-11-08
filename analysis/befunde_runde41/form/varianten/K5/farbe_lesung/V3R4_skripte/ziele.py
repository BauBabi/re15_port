"""Nachtrag Rot (Messer): Zielwerte je Region aus mess.json.
Ziel = Vorlage O (Original, rueckgerechnet; derselbe Messraum wie die Variante nach der Kette).
Toleranz (rot, rot_eng, h_rot, C_rot, dL) = 2 x sigma (Block-Bootstrap 12 px der Vorlage) + |Rundlauf-Mittel - O| (Verschiebung durch eine
zweite Kette, Mittel aus 3 Saaten).
Groesste Rotflaeche (raeumlich, kein Bootstrap): Fenster [0,6 x O, max(1,5 x O, O + 3)] wie MESSBLATT_FARBE_V1 K4 (0,6-1,5 x Original) und
MESSBLATT_FARBE_V3 K12 (max(1,5 x Original, Original + 3 Punkte)), beidseitig erweitert um |R - O| + Spannweite der 3 Rundlauf-Saaten.
Bindend nur Regionen mit >= 20 Bloecken; C* der Rotklasse nur Info (haengt in der Kette an der Aderbreite, Probe probe.py); Bild 1 Ruecken groesste Rotflaeche ist der Hof der Leuchtstelle F1 (4-10 px neben F1) und nur Info."""
import json
import numpy as np

E = json.load(open('mess.json', encoding='utf-8'))
S = E['saaten']
METRIK = ('rot', 'rot_eng', 'h_rot', 'C_rot', 'dL', 'gross_p', 'gross_eng_p')
MIN_BLOECKE = 20
INFO = {('1', 'Ruecken', 'gross_eng_p'), ('1', 'Ruecken', 'gross_p')}


def mittel(z, pre, m):
    v = [z['%s%d' % (pre, s)][m] for s in S]
    v = [x for x in v if x is not None]
    return (float(np.mean(v)), float(max(v) - min(v))) if v else (None, None)


Z = {}
for n in ('1', '2'):
    B = E['bilder'][n]
    for rn, z in B['zonen'].items():
        if 'O_T' not in z:
            continue
        for raum in ('T', 'V'):
            o = z['O_' + raum]; bo = z['O_%s_boot' % raum]
            e = {}
            for m in METRIK:
                O = o.get(m)
                Rm, Rsp = mittel(z, 'R_' + raum, m)
                Vm, Vsp = mittel(z, 'V3_' + raum, m)
                if O is None or Rm is None:
                    e[m] = dict(O=O, R=Rm, V3=Vm, ziel=None)
                    continue
                if m in ('gross_p', 'gross_eng_p'):
                    w = abs(Rm - O) + Rsp
                    lo, hi = max(0.6 * O - w, 0.0), max(1.5 * O, O + 3.0) + w
                    sig, tol = None, round(w, 2)
                else:
                    sig = bo.get(m)
                    tol = (2 * sig if sig is not None else 0) + abs(Rm - O)
                    lo, hi = O - tol, O + tol
                    if m in ('rot', 'rot_eng'):
                        lo = max(lo, 0.0)
                if m in ('gross_p', 'gross_eng_p'):
                    ok = None if Vm is None else bool(Vm <= hi)          # bindend nur die Obergrenze (Nutzer: zu viel Rot); Untergrenze Info
                    unten_ok = None if Vm is None else bool(Vm >= lo)
                else:
                    ok = None if Vm is None else bool(lo <= Vm <= hi)
                    unten_ok = None
                bindend = (z['bloecke'] >= MIN_BLOECKE or rn == 'Figur') and (n, rn, m) not in INFO and m != 'C_rot'
                e[m] = dict(O=round(O, 2), R=round(Rm, 2), R_spann=round(Rsp, 2), V3=None if Vm is None else round(Vm, 2),
                            V3_spann=None if Vsp is None else round(Vsp, 2), sigma=sig, tol=round(tol, 2), ziel=[round(lo, 2), round(hi, 2)], ok=ok,
                            bindend=bool(bindend), unten_ok=unten_ok, bloecke=z['bloecke'], alb=z['V3_alb'].get(m) if raum == 'T' else None)
            Z['bild%s|%s|%s' % (n, rn, raum)] = e
json.dump(Z, open('ziele.json', 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
