"""K15-Bandmaske je Knochen: Ketten-T (Variante) gegen Original-T. python band_knochen.py <Variante> [maske=band|F1] [bild]"""
import sys, os, collections, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import riegel_farbe_v3 as RV
import p_zonen as PZ
v = sys.argv[1]; mn = sys.argv[2] if len(sys.argv) > 2 else 'band'; n = int(sys.argv[3]) if len(sys.argv) > 3 else (2 if mn == 'band' else 1)
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3'
k = PZ.P.Kandidat(os.path.join(v, 'form.json'))
sauber, alb, m, kk = k.rendern(n)
vid = PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=15)
T, c = PZ.rueck(vid, n)
To, co = PZ.rueck(PZ.P.original(n), n)
e = np.clip(To * PZ.LICHT[n], 0, 255).astype(np.uint8)
Tr, _c = PZ.rueck(PZ.FV.video_filter(e, bild=n, entzerren=False, rauschen=True, saat=15), n)
mk = RV.masken_daten()[mn]
kn = np.load(S + '/leserb/fid%d.npz' % n)['kn']
f = m & (alb.max(2) > 0)
def lb(x): return [round(float(t), 1) for t in RV.lab(x.reshape(-1, 3).mean(0))[0]] if len(x) else None
print('maske', mn, 'px', mk.sum(), 'Variante', lb(T[mk]), 'Original', lb(To[mk]), 'Rundlauf', lb(Tr[mk]), 'K5c-gedeckt', (mk & f).sum())
alb_m = alb[mk & f]
for b, cnt in collections.Counter(kn[mk].tolist()).most_common():
    s2 = mk & (kn == b)
    print('  Knochen %3d px %5d  V %s  O %s  R %s gedeckt %d' % (b, cnt, lb(T[s2]), lb(To[s2]), lb(Tr[s2]), (s2 & f).sum()))
s3 = mk & ~f
print('  ungedeckt px', s3.sum(), 'V', lb(T[s3]), 'O', lb(To[s3]), 'R', lb(Tr[s3]))
np.savez_compressed(os.path.join(S, 'r3', 'kette_%s.npz' % mn), T=T, To=To, Tr=Tr, f=f, kn=kn, alb=alb)
