"""Versuch Stachel (Messung): Profil / Spitze variieren, je PV in festen Abstaenden (n7-Verfahren), tiefster sichtbarer Punkt, Anzahl angehobener
Keyframes, groesste Anhebung."""
import os, sys, json
import numpy as np
R8 = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PR = os.path.join(os.path.dirname(R8), 'pr8')
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V, f_rast as FR, schwanz_r8 as Q
cwd = os.getcwd(); os.chdir(PR); sys.path.insert(0, PR)
import a_mass as A
os.chdir(cwd)
basis = open(os.path.join(R8, 'emd', 'ALT_07b6703f.EMD'), 'rb').read()
AB = (110, 130, 150, 170, 190)
VAR = {
 'K1 Knolle 190 / Spitze 1300': (((200, 160), (330, 190), (450, 185), (560, 150), (660, 110), (760, 75), (920, 50), (1100, 28)), 1300, 760),
 'K2 Knolle 190 bei 450 / 1300': (((200, 150), (330, 175), (450, 190), (560, 175), (660, 125), (760, 80), (920, 52), (1100, 28)), 1300, 760),
 'K3 Knolle 210 / 1250': (((200, 165), (330, 205), (450, 210), (560, 170), (660, 120), (760, 80), (920, 50), (1080, 28)), 1250, 760),
}
for nm, (st, sp, en) in VAR.items():
    Q.STACHEL, Q.SPITZE_Y, Q.ENDE_Y = st, float(sp), float(en)
    prot = {}
    neu = Q.stufe(basis, 'b', prot)
    mo = V.modell(neu)
    A_ = FR.mit_bg(mo, V.PV, licht=False)
    m = (A_['bone'][:480] >= 16) & (A_['bone'][:480] <= 22)
    r = A.profil_abs(m, (376, 220), AB)
    ys, xs = np.nonzero(m)
    g = prot['heben']['grad']
    print('%s: L %.1f tief y %d, Breite %s | gehoben %d, groesste Drehung %d Grad, PV gehoben %s' % (nm, r['L'], ys.max(), r['breite'], len(g),
          max(abs(v) for v in g.values()) if g else 0, prot['heben']['pv_gehoben']), flush=True)
