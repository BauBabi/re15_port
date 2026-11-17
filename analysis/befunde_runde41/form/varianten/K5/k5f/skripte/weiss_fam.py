import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import numpy as np, cv2
import rl, fam
PL = rl.PL; K5 = rl.K5
S = sys.argv[1]
def r15(t):
    mo = PL.re15typ(t); mo.T = rl._Tex(mo); return mo
g4 = PL.re2(0x34); g4.T = rl._Tex(g4)
g5 = PL.re2(0x36); g5.T = rl._Tex(g5)
mods = [('G4', g4, 0, 'RE2-G4 (Typ 0x34) Original'), ('EM30', rl.fl.lade_em30(), 0, 'EM30_HANGAR ganze Figur'),
        ('K5c', rl.fl.lade_var(rl.BASIS), 2, 'K5c (basis_k5c)'), ('K5e_f1', rl.fl.lade_var(K5 + '/ruhe/K5e_f1'), 2, 'K5e_f1 Spiel-Asset'),
        ('Zombie', r15(0x10), 0, 'RE1.5 EM10'), ('Gorilla', r15(0x27), 0, 'RE1.5 EM27'), ('G1', r15(0x30), 0, 'RE1.5 EM30 (G1)'),
        ('Tyrant', r15(0x2B), 0, 'RE1.5 EM2B'), ('Hund15', r15(0x20), 0, 'RE1.5 EM20'), ('G5', g5, 0, 'RE2 EM36 (nur Knochen 0/1)')]
aus = {}; kach = []
for key, mo, clip, txt in mods:
    werte = []; zeile = []
    for wn, az, el in rl.WINKEL4:
        img, K = rl.weiss_id(mo, clip, az, el, 380)
        m = K['maske'].copy()
        if key == 'EM30_haut':
            m &= ~np.isin(K['mesh'], rl.EM30_KLEIDUNG)
        werte.append(fam.alles(img, m)); zeile.append(img)
    mit = {k: round(float(np.mean([w[k] for w in werte])), 3) for k in werte[0] if isinstance(werte[0][k], (int, float))}
    aus[key] = dict(text=txt, mittel=mit, je_winkel=werte)
    kach.append(np.hstack(zeile))
    print(key, {k: mit[k] for k in ('L_mittel', 'L_std', 'L_cv', 'lokal5', 'lokal5_rel', 'C_mittel', 'C_std', 'dunkel_pct', 'hell_pct', 'S5', 'F_fremd', 'spanne90', 'block_ton_streu')})
json.dump(aus, open(S + '/weiss_fam.json', 'w'), indent=1)
cv2.imwrite(S + '/weiss_fam.png', cv2.cvtColor(np.vstack(kach), cv2.COLOR_RGB2BGR))
