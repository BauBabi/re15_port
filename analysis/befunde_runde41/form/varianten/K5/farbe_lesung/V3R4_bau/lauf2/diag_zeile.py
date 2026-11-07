"""Je Gruppenzeile: genutzte Eintraege mit a* > 15 (P1-Fleischklasse): G4-Lab -> neues Lab, Texelzahl."""
import sys, os, json
K5 = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5'
sys.path.insert(0, K5 + '/generator')
os.chdir(K5 + '/generator')
import numpy as np
import riegel_farbe_v3 as R
KG = R.KG
vdir = sys.argv[1]
zeilen = [int(x) for x in sys.argv[2].split(',')]
v = R.Var(vdir)
g4, g4tim, _ = KG.g4_modell()
px_g4, cl_g4 = KG.g4_cluts(g4tim)
b = json.load(open(vdir + '/bau.json', encoding='utf-8'))
basis = {int(k): x for k, x in b['zeilen']['basis'].items()}
for z in zeilen:
    cnt = np.zeros(256)
    for key, mi, zz, ecken, a3 in v.flaechen:
        if zz != z:
            continue
        xy = v.texel(ecken)
        if len(xy):
            np.add.at(cnt, v.px[xy[:, 1], xy[:, 0]], 1)
    Ln = R.lab(v.rgb_zeilen[z])
    Lg = R.lab(KG.wort_rgb(cl_g4[basis[z]]))
    _L, _C, _h, w = KG.r4_eintraege(cl_g4[basis[z]])
    sel = np.where((cnt > 0))[0]
    print('== Zeile', z, b['zeilen']['gruppe'].get(str(z)), 'Texel', int(cnt.sum()))
    o = sel[np.argsort(Lg[sel, 0])]
    for i in o:
        print('  %3d n%5d w%.2f G4 %5.1f %6.1f %6.1f -> %5.1f %6.1f %6.1f  rgb %s %s' % (i, cnt[i], w[i], *Lg[i], *Ln[i], tuple(int(x) for x in v.rgb_zeilen[z][i]), 'FLEISCH' if Ln[i, 1] > 15 else ''))
