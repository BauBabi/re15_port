"""Pruefer V3: Vorhersage der Bild-1/2-Ketten fuer Leser As (b2)-Machbarkeitsprobe (G4-Texel 1:1, ohne Feuerstellen, ohne Kopfloesung).
Gleicher Weg wie riegel_farbe.kette_v2 / rotton.py: Szenenrender (Kandidat-Licht 'mess') -> video_filter Saat 15 -> p_zonen.rueck -> Klassen.
Nur Vorhersage, kein Riegel-Ergebnis (die Probe ist kein Bau)."""
import os, sys, pickle, json, math
A = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/A'
HIER = os.path.dirname(os.path.abspath(__file__))
os.chdir(A); sys.path.insert(0, A)
from uvprojekt import *
from probe import mo, tg, k5, t5, k5_rgba
import numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/P_skripte')
import p_zonen as PZ
P = PZ.P; FV = PZ.FV
alle = pickle.load(open('voll2_uv.pkl', 'rb'))
m2 = modell_mit(k5, alle); tex = RGBTextur(kombi_atlas(k5_rgba(), mo, tg))
K = P.Kandidat(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/K5c/form.json')


def lab(x):
    return cv2.cvtColor(np.asarray(x, np.float32).reshape(1, -1, 3) / 255.0, cv2.COLOR_RGB2LAB)[0]


def rotlab(T, m):
    L = lab(T[m]); C = np.hypot(L[:, 1], L[:, 2])
    r = (L[:, 1] >= 12) & (C >= 15) & (L[:, 0] >= 20)
    Pp = L[r]
    if not len(Pp):
        return dict(n=0)
    return dict(n=int(r.sum()), anteil=round(100 * float(r.mean()), 1), lab=[round(float(x), 1) for x in Pp.mean(0)],
                h=round(math.degrees(math.atan2(Pp[:, 2].mean(), Pp[:, 1].mean())), 1), dunkel=round(100 * float((L[:, 0] < 20).mean()), 1),
                lab_alles=[round(float(x), 1) for x in L.mean(0)])


R = {}
for n in (1, 2):
    lage, gier, clip, bild = K.stand(n)
    kam = P.kamera(n); licht, kk = P.licht_fuer(n, K.licht)
    hg = P.lade_rgb(P.daten('hintergrund_bild%d.png' % n))
    for nm, mod, tx in (('K5c_heute', K.mo, K.tex), ('b2_probe_A', m2, tex)):
        f0, kc = mod.clips[clip if clip < len(mod.clips) else 0]
        kf = mod.frames[f0 + min(bild, kc - 1)] & 0xFFF
        sau = hg.copy()
        m = P.geschlossen(FB.rastern(FB.dreiecke(mod, kf, lage, gier, kam, licht), tx, sau, kam.abbildung))
        vid = FV.video_filter(sau, bild=n, entzerren=False, rauschen=True, saat=15)
        T, c = PZ.rueck(vid, n)
        regs = dict(PZ.regionen(n))
        fig = np.zeros((480, 640), bool)
        for g in regs.values():
            fig |= g
        R['bild%d|%s' % (n, nm)] = {z: rotlab(T, g & m) for z, g in list(regs.items()) + [('Figur', fig)]}
        cv2.imwrite(os.path.join(HIER, 'kette_b2_bild%d_%s.png' % (n, nm)), cv2.cvtColor(vid, cv2.COLOR_RGB2BGR))
    vo = P.original(n); To, co = PZ.rueck(vo, n)
    regs = dict(PZ.regionen(n)); fig = np.zeros((480, 640), bool)
    for g in regs.values():
        fig |= g
    R['bild%d|Original' % n] = {z: rotlab(To, g) for z, g in list(regs.items()) + [('Figur', fig)]}
json.dump(R, open(os.path.join(HIER, 'kette_b2.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
for k, v in R.items():
    print(k)
    for z, x in v.items():
        print('   ', z, x)
