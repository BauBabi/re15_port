"""G9: was die Seitenfaktoren aus der G4-eigenen Seitenstatistik (seiten_g4.json) fuer Huefte/Ruecken (Teile 0/7) bedeuten wuerden - L* je Zugriff
= knie(f_s x L_G4) wie die Hautzeile (unter dem Knie exakt multiplikativ), Haut-Band P25-P75 der uebrigen Hautflaechen wie Riegel S1."""
import sys, os, json
sys.dont_write_bytecode = True
G = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, G)
import numpy as np
import kf_k5g2_tex as TX, kf_k5e as E, kf_k5g3_tex as T3
X = E.eingaenge(); Z = TX.lade_zonen()
import riegel_k5g2 as R2
S = R2.Stand(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/K5e_f1')
plan = T3.materialplan(S.form, X, Z)
cl = [[int(x) for x in z] for z in X['cl_g4']]
lab = [TX.lab_zeile(z) for z in cl]
sg = json.load(open('seiten_g4.json'))
f = 1.86
varianten = {'Abgabe (K5c-Hautflaechen je Seite)': {0: 2.4402, 1: 1.0, 2: 1.3705},
             'G4-eigene Seitenstatistik (Hautklasse)': {s: sg[str(s)]['g_haut'] for s in range(3)},
             'ohne Angleich': {0: 1.0, 1: 1.0, 2: 1.0}}
px0 = X['T']['pixel']
def L_je(keys, g):
    out = []
    for k in keys:
        s = plan[k][1]
        ys, xs = np.nonzero(TX.flaeche_maske(S.fl[k], px0.shape))
        i = px0[ys, xs]
        out.append(TX.knie(lab[s][i, 0] * f * g[s], 75.0))
    return np.concatenate(out)
haut = [k for k, v in plan.items() if v[0] == 'haut']
sattel = [k for k in haut if k[0] in (0, 7)]
rest = [k for k in haut if k[0] not in (0, 7)]
for nm, g in varianten.items():
    a = L_je(sattel, g); b = L_je(rest, g)
    print('%-42s f_s %s  Teile 0/7 Median L* %.1f   uebrige Haut P25/Median/P75 %.1f/%.1f/%.1f' % (nm, {s: round(f * g[s], 3) for s in g}, np.median(a), *np.percentile(b, [25, 50, 75])))
