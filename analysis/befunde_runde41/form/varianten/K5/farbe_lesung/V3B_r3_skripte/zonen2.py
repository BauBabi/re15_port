"""Bild-2-Zonen: Lab je Zone, Variante gegen Original, und dieselbe Kette je Knochen in der Figur. python zonen2.py <Variante> [n]"""
import sys, os, collections, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import riegel_farbe_v3 as RV
import p_zonen as PZ
v = sys.argv[1]; n = int(sys.argv[2]) if len(sys.argv) > 2 else 2
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3'
k = PZ.P.Kandidat(os.path.join(v, 'form.json'))
s, alb, m, kk = k.rendern(n)
T, c = PZ.rueck(PZ.FV.video_filter(s, bild=n, entzerren=False, rauschen=True, saat=15), n)
To, co = PZ.rueck(PZ.P.original(n), n)
kn = np.load(S + '/leserb/fid%d.npz' % n)['kn']
def lb(x): return [round(float(t), 1) for t in RV.lab(x.reshape(-1, 3).mean(0))[0]] if len(x) else None
regs = dict(PZ.regionen(n))
fig = np.zeros((480, 640), bool)
for nm, g in regs.items():
    fig |= g
    print('%-20s px %5d V %s O %s' % (nm, g.sum(), lb(T[g]), lb(To[g])))
print('%-20s px %5d V %s O %s' % ('Figur', fig.sum(), lb(T[fig]), lb(To[fig])))
for b, cnt in collections.Counter(kn[fig].tolist()).most_common(12):
    s2 = fig & (kn == b)
    print('   Knochen %3d px %5d V %s O %s' % (b, cnt, lb(T[s2]), lb(To[s2])))
