"""Spur SF Schwanz V2, Variante V2 (Formblatt V2 K3-K7, Ae6/Ae8/Ae9): Koordinatensuche der Koerperparameter gegen die
Vorgaben in der Vorderansicht h118/h125 (Posen anpassen_v2.py, Spielkamera): Bund 0,42, Saum 0,57, Breite 0,61 (0,57-0,65),
Krallen 0,33 (0,30-0,36), Sichel 0,22 (0,17-0,27) - je in H. GESTALTUNG nach der Bildmessung, keine RE-Werte.
Parameter: Oberschenkel-/Schienbeinlaenge (skal y 1/4, 2/5), Rumpflaenge (skal y 7/8), Beckenhoehe (ducken becken),
Schulterbreite (skal z 8), Oberarm-Dicke (skal x/z 10/13), Arme zur Mitte (ducken arm_innen), Klauen-Ellbogen (ducken ellbogen_links).
python koerper_suche.py <posen_vorn.json> <aus.json>
"""
import sys, os, json, copy
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import b2
import koerper_mass as KM
import birkin_formen as BF

ZIEL = dict(bund=0.42, saum=0.57, breite=0.61, krallen=0.33, sichel=0.22)
GEWICHT = dict(bund=3.0, saum=3.0, breite=1.5, krallen=1.0, sichel=0.7)
START = dict(bein_o=1.0, bein_u=1.0, rumpf=1.0, becken=110.0, schulter=1.15, arm=1.0, arm_innen=20.0, ellbogen=60.0)
SCHRITT = dict(bein_o=0.06, bein_u=0.08, rumpf=0.05, becken=80.0, schulter=0.06, arm=0.06, arm_innen=6.0, ellbogen=12.0)


def form_aus(p, mit_schwanz=False):
    f = copy.deepcopy(BF.FORMEN['hangar'])
    if not mit_schwanz:
        f.pop('schwanz_kette', None); f.pop('schwanz_textur', None)
    sk = dict(f['skal'])
    sk[1] = sk[4] = (1.0, p['bein_o'], 1.0)
    sk[2] = sk[5] = (1.0, p['bein_u'], 1.0)
    a7, a8 = sk[7], sk[8]
    sk[7] = (a7[0], a7[1] * p['rumpf'], a7[2])
    sk[8] = (a8[0], a8[1] * p['rumpf'], p['schulter'])
    for b in (10, 13):
        x, y, z = sk[b]
        sk[b] = (x * p['arm'], y, z * p['arm'])
    f['skal'] = sk
    f['ducken'] = dict(f['ducken'], becken=p['becken'], arm_innen=p['arm_innen'], ellbogen_links=p['ellbogen'])
    return f


def bewerte(p, posen, basis):
    mo = b2.modell(BF.baue(basis, form_aus(p)))
    ms = [KM.koerper_mass(mo, pose) for pose in posen]
    fehler = 0.0
    for k, z in ZIEL.items():
        for m in ms:
            v = m[k] if m[k] is not None else 0.0
            fehler += GEWICHT[k] * (v - z) ** 2
    return fehler, ms


def main():
    posen = list(json.load(open(sys.argv[1])).values())
    basis = BF.lade_em30()
    p = dict(START)
    best, ms = bewerte(p, posen, basis)
    print('Start', round(best, 5), [KM.text(m) for m in ms], flush=True)
    for runde in range(3):
        for k in p:
            for vz in (1, -1):
                while True:
                    q = dict(p); q[k] = p[k] + vz * SCHRITT[k] / (runde + 1)
                    f, m2 = bewerte(q, posen, basis)
                    if f < best - 1e-6:
                        best, p, ms = f, q, m2
                    else:
                        break
        print('Runde', runde, round(best, 5), {k: round(v, 3) for k, v in p.items()}, flush=True)
        for m in ms:
            print('   ', ' | '.join(KM.text(m)), flush=True)
    json.dump(dict(parameter=p, fehler=best, masse=[{k: v for k, v in m.items()} for m in ms]), open(sys.argv[2], 'w'),
              indent=1, default=float)


if __name__ == '__main__':
    main()
