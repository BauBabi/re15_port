#!/usr/bin/env python3
"""K5g3 Runde 2 - Augen im ECHTEN Spielfenster messen (Sicht-Pruefer K5g3 r1: "Die zwei Augenpunkte muessen im Spiel als Augen erkennbar sein.
Bisher prueft das kein Riegel; noetig ist eine Sichtpruefung im Fenster").

    python kf_k5g3_augen_spiel.py [<Variante>]     -> <Variante>/spiel_augen.json (+ Fensterbilder unter $K5G3_SCR/augen_spiel/)

Je Variante zwei Laeufe mit generator/kf_k5g3_spiel.sh (eigene exe-Kopie, Variante ueber RE15_RE2_ASSET_ROOT, gdigrab nur auf das eigene Fenster
ueber die eigene PID, kein AUTOSHOT/SOFTWARE_RENDER), 5080 von vorn (BB_POSE -20711,-15984,2095,12,0,1 = Clip 12 Bild 0, Schnitt 1; wie
kf_k5g3_fenster.sh 'vorn_r5080'): (1) die Variante, (2) dieselbe Variante mit Markierung (die Augen-Eintraege der Zeile 'auge' auf reines Gruen).
Augen-Pixel = Pixel, die sich zwischen (1) und (2) um mehr als 20 unterscheiden; Messung L* im Bild (1), Ring = 15x15 ohne 5x5 um das Auge.
Gegenprobe: dieselbe Variante mit f_auge der Runde 1 (1,9844), gleiche Laeufe.
"""
import hashlib
import json
import os
import shutil
import subprocess
import sys

import cv2
import numpy as np

sys.dont_write_bytecode = True
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5e as E  # noqa: E402
import kf_k5g3 as K3  # noqa: E402

K5 = E.K5
POSE = '-20711,-15984,2095,12,0,1'


def _lab(rgb):
    sys.path.insert(0, os.path.join(K5, 'farbe_lesung', 'V4F_skripte'))
    import v4f
    return v4f.lab(rgb)


def markieren(v, aus):
    shutil.rmtree(aus, ignore_errors=True)
    shutil.copytree(v, aus)
    a = json.load(open(os.path.join(v, 'bau.json'), encoding='utf-8'))['textur']['augen_dekal']
    for n in ('BAHNSTEIG_FORM.TIM', 'FORM.TIM'):
        t = E.FE.tim_lesen(open(os.path.join(aus, n), 'rb').read())
        cl = [[int(x) for x in z] for z in t['cluts']]
        for i in a['augen_eintraege']:
            cl[a['zeile']][i] = 0x03E0
        open(os.path.join(aus, n), 'wb').write(E.FE.tim_schreiben(t['pixel'], cl, t['clut_x'], t['clut_y'], t['bild_x'], t['bild_y']))


def lauf(v, aus):
    shutil.rmtree(aus, ignore_errors=True)
    subprocess.run([shutil.which('bash') or 'bash', os.path.join(HIER, 'kf_k5g3_spiel.sh'), v.replace(chr(92), '/'), aus.replace(chr(92), '/'), '5080',
                    POSE, '500,-9175,2048,0', '900', '1', 'RE15_FORCE_CUT=1', 'RE15_INPUT_SCRIPT=W1'], check=False, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL, timeout=900)
    b = os.path.join(aus, 'fenster_1.png')
    if not os.path.exists(b):
        raise SystemExit('kein Fensterbild: %s' % aus)
    return cv2.cvtColor(cv2.imread(b), cv2.COLOR_BGR2RGB)


def messen(normal, mark):
    d = np.abs(mark.astype(int) - normal.astype(int)).max(2) > 20
    L = _lab(normal)[..., 0]
    n, lbl = cv2.connectedComponents(cv2.dilate(d.astype(np.uint8), np.ones((5, 5), np.uint8)))
    out = []
    for i in range(1, n):
        e = d & (lbl == i)
        if not e.any():
            continue
        ring = cv2.dilate(e.astype(np.uint8), np.ones((15, 15), np.uint8)).astype(bool) & \
            ~cv2.dilate(e.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool) & ~d
        ys, xs = np.nonzero(e)
        out.append(dict(px=int(e.sum()), mitte=[int(round(xs.mean())), int(round(ys.mean()))], L_median=round(float(np.median(L[e])), 2),
                        L_max=round(float(L[e].max()), 2), ring_median=round(float(np.median(L[ring])), 2)))
    return sorted(out, key=lambda t: t['mitte'][0])


def main(v):
    scr = os.path.join(K3.SCR, 'augen_spiel')
    os.makedirs(scr, exist_ok=True)
    par = json.load(open(os.path.join(v, 'bau.json'), encoding='utf-8'))['par']
    p2 = json.loads(json.dumps(par))
    p2['textur']['auge']['f'] = 1.9844
    g = os.path.join(scr, 'gegen_f19844')
    shutil.rmtree(g, ignore_errors=True)
    K3.bauen(p2, g, log=lambda *a: None)
    erg = {}
    for name, var in (('augen', v), ('gegen', g)):
        markieren(var, var.rstrip('/\\') + '_markiert')
        n = lauf(var, os.path.join(scr, name + '_normal'))
        m = lauf(var.rstrip('/\\') + '_markiert', os.path.join(scr, name + '_markiert'))
        erg[name] = messen(n, m)
        print(name, erg[name], flush=True)
    r = dict(tim_sha256=hashlib.sha256(open(os.path.join(v, 'BAHNSTEIG_FORM.TIM'), 'rb').read()).hexdigest(),
             emd_sha256=hashlib.sha256(open(os.path.join(v, 'BAHNSTEIG_FORM.EMD'), 'rb').read()).hexdigest(),
             verfahren='kf_k5g3_augen_spiel.py: kf_k5g3_spiel.sh 5080 BB_POSE %s, RE15_FORCE_CUT=1, Normal- und Markierungslauf (Augen-Eintraege gruen), '
                       'Differenz > 20 = Augen-Pixel, L* im Normalbild, Ring = 15x15 ohne 5x5 um das Auge' % POSE,
             bilder=dict(normal=os.path.join(scr, 'augen_normal', 'fenster_1.png').replace(chr(92), '/'),
                         markiert=os.path.join(scr, 'augen_markiert', 'fenster_1.png').replace(chr(92), '/')), **erg)
    json.dump(r, open(os.path.join(v, 'spiel_augen.json'), 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    return r


if __name__ == '__main__':
    main(os.path.abspath(sys.argv[1]) if len(sys.argv) > 1 else K3.AUS)
