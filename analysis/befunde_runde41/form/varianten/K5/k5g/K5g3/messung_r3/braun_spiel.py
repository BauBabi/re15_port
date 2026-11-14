"""Runde 3 (G6) - braeunliche Musterung im ECHTEN Spiel (Wiederholung der Messung des Zahlen-Pruefers r2, z5080 mit Braun-Marker, mit eigenem Code).

Laeufe: generator/kf_k5g3_zlauf.sh (eigene exe-Kopie, echter Renderer, RE15_FRAMEDUMP = SDL_RenderReadPixels des eigenen Fensters, kein
AUTOSHOT/SOFTWARE_RENDER), 5080 Bild-1-Lage BB_POSE -20711,-15984,756,0,11,1, Spieler 500,-9175,2048,0, Bilder 360-440/20.
Je Variante: normal, Schluessel (alle CLUT-Worte weiss, kf_k5f_spielmass.tim_key) und Marker (die Braun-Eintraege der Variante reines Gruen).
Braun-Pixel = Gruen im Markerbild (G > R + 25 und G > B + 25). Q = normal / Schluessel x 255 (Albedo-Lesart wie Messblatt K5F 1.1).
Varianten: K5g3 Runde 3 (Abgabe), K5g3 Runde 2 (Commit f48d37df1), K5e_f1 (Spiel-Asset). Die Marker-Maske kommt je Variante aus ihrem eigenen
Marker (K5e_f1: die Maske von Runde 3; Beine sind in allen drei Staenden geometrisch gleich).
    python braun_spiel.py            -> braun_spiel.json, belege/braun_z5080.png
"""
import json
import os
import shutil
import subprocess
import sys

import cv2
import numpy as np

sys.dont_write_bytecode = True
HIER = os.path.dirname(os.path.abspath(__file__))
G = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, G)
import kf_k5e as E  # noqa: E402
import kf_k5g2_tex as TX  # noqa: E402
import kf_k5g3_tex as T3  # noqa: E402
import kf_k5f_spielmass as KS  # noqa: E402
import riegel_k5g3_tex as RT  # noqa: E402
import riegel_k5g2 as R2  # noqa: E402

FE = E.FE
SP = os.path.join(HIER, 'spiel').replace(chr(92), '/')
VAR = os.path.join(HIER, 'var')


def braun_eintraege(v):
    """(Zeile, Index) der Braun-Material-Texel in der Variante v (Pruef-Stand, Kopien aus bau.json)."""
    bau = json.load(open(os.path.join(v, 'bau.json'), encoding='utf-8'))
    S = RT.stand_mit_namen(os.path.join(v, 'pruef'), bau['textur']['zeilen'])
    X = E.eingaenge()
    plan = T3.materialplan(S.form, X, TX.lade_zonen())
    je = RT.je_mit_kern(S, plan)
    kopien = {c['flaeche']: c for c in bau['textur']['kopien']}
    aus = set()
    for k in sorted(je):
        c = kopien.get('%d|%d|%d' % k)
        if c is None:
            continue
        dx, dy = c['nach'][0] - c['von'][0], c['nach'][1] - c['von'][1]
        for (x, y), nr in je[k].items():
            if nr in ('16', '19'):
                aus.add((S.zeile[k], int(S.T['pixel'][y + dy, x + dx])))
    return sorted(aus)


def marker(v, aus, eintraege):
    shutil.rmtree(aus, ignore_errors=True)
    os.makedirs(aus)
    shutil.copy(os.path.join(v, 'BAHNSTEIG_FORM.EMD'), aus)
    t = FE.tim_lesen(open(os.path.join(v, 'BAHNSTEIG_FORM.TIM'), 'rb').read())
    cl = [[int(x) for x in z] for z in t['cluts']]
    for z, i in eintraege:
        cl[z][i] = 0x03E0 | (cl[z][i] & 0x8000)
    open(os.path.join(aus, 'BAHNSTEIG_FORM.TIM'), 'wb').write(FE.tim_schreiben(t['pixel'], cl, t['clut_x'], t['clut_y'], t['bild_x'], t['bild_y']))


def lauf(name, v):
    env = dict(os.environ, K5G3_SPIEL=HIER.replace(chr(92), '/'))
    subprocess.run([shutil.which('bash') or 'bash', os.path.join(G, 'kf_k5g3_zlauf.sh'), name, '5080', '360-440/20', 'RE15_BIRKIN_FORM=bahnsteig',
                    'RE15_BB_POSE=-20711,-15984,756,0,11,1', 'RE15_FORCE_CUT=1', 'RE15_PLAYER_POS=500,-9175,2048,0',
                    'RE15_RE2_ASSET_ROOT=%s' % v.replace(chr(92), '/')], check=False, env=env, stdout=subprocess.DEVNULL)


def bild(name, n='dump_000400.ppm'):
    p = os.path.join(SP, name, n)
    return cv2.imread(p) if os.path.exists(p) else None


def main():
    os.makedirs(VAR, exist_ok=True)
    stände = dict(k5g3r3=RT.K3.AUS, k5g3r2=RT.r2_stand(), k5e=RT.K5E)
    eintr = {}
    for nm, v in stände.items():
        KS.schluessel(v, os.path.join(VAR, nm))         # -> var/<nm>_normal, var/<nm>_key
        if nm != 'k5e':
            eintr[nm] = braun_eintraege(v)
            marker(v, os.path.join(VAR, nm + '_braun'), eintr[nm])
    for nm in stände:
        for art in ('normal', 'key') + (('braun',) if nm != 'k5e' else ()):
            d = os.path.join(VAR, nm + '_' + art)
            if art == 'braun':
                d = os.path.join(VAR, nm + '_braun')
            lauf('%s_%s' % (nm, art), d)
    erg = dict(eintraege={k: [list(t) for t in v] for k, v in eintr.items()})
    bilder = sorted(os.listdir(os.path.join(SP, 'k5g3r3_braun')))
    bilder = [b for b in bilder if b.endswith('.ppm')]
    for nm in stände:
        mk = 'k5g3r3' if nm == 'k5e' else nm
        werte = []
        for b in bilder:
            M = bild(mk + '_braun', b)
            N, K = bild(nm + '_normal', b), bild(nm + '_key', b)
            if M is None or N is None or K is None:
                continue
            Mi = M.astype(int)
            m = (Mi[..., 1] > Mi[..., 2] + 25) & (Mi[..., 1] > Mi[..., 0] + 25)
            if not m.any():
                continue
            q = np.clip(N.astype(np.float32) / np.maximum(K.astype(np.float32), 1) * 255, 0, 255).astype(np.uint8)
            d = dict(bild=b, px=int(m.sum()))
            for art, img in (('P', N), ('Q', q)):
                L = cv2.cvtColor(img.astype(np.float32) / 255., cv2.COLOR_BGR2Lab)
                l_ = L[..., 0][m]
                C = np.hypot(L[..., 1], L[..., 2])[m]
                h = np.degrees(np.arctan2(np.median(L[..., 2][m]), np.median(L[..., 1][m])))
                d[art] = dict(L_median=round(float(np.median(l_)), 2), cv=round(float(l_.std() / max(l_.mean(), 1e-6)), 3),
                              C_median=round(float(np.median(C)), 2), ton=round(float(h), 1),
                              farben=len(set(map(tuple, img[m].reshape(-1, 3).tolist()))))
            werte.append(d)
        erg[nm] = werte
        if werte:
            print(nm, 'px', werte[0]['px'], 'P', werte[0]['P'], 'Q', werte[0]['Q'], flush=True)
    json.dump(erg, open(os.path.join(HIER, 'braun_spiel.json'), 'w', encoding='utf-8'), indent=1)
    # Beleg (Messbild): Ausschnitt um die Braun-Stelle, K5e_f1 | K5g3 r2 | K5g3 r3 | Marker r3, 5-fach
    b = 'dump_000400.ppm'
    M = bild('k5g3r3_braun', b).astype(int)
    m = (M[..., 1] > M[..., 2] + 25) & (M[..., 1] > M[..., 0] + 25)
    ys, xs = np.nonzero(m)
    y0, y1, x0, x1 = max(ys.min() - 25, 0), ys.max() + 25, max(xs.min() - 25, 0), xs.max() + 25
    z = lambda x: cv2.resize(x[y0:y1, x0:x1], None, fx=5, fy=5, interpolation=cv2.INTER_NEAREST)  # noqa: E731
    reihe = np.hstack([z(bild('k5e_normal', b)), z(bild('k5g3r2_normal', b)), z(bild('k5g3r3_normal', b)), z(bild('k5g3r3_braun', b))])
    kopf = np.zeros((36, reihe.shape[1], 3), np.uint8)
    cv2.putText(kopf, 'Messbild - kein Ergebnis | K5e_f1 | K5g3 Runde 2 | K5g3 Runde 3 | Marker R3 (5080, Bild 400)', (6, 24),
                cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 1)
    os.makedirs(os.path.join(HIER, 'belege'), exist_ok=True)
    cv2.imwrite(os.path.join(HIER, 'belege', 'braun_z5080.png'), np.vstack([kopf, reihe]))


if __name__ == '__main__':
    main()
