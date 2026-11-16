"""Messung PV in festen Abstaenden ab dem koerpernahen Ende (Verfahren des Pruefers pr8 a_mass.profil_abs: Mittellinie = laengster Skelettpfad
der Maske, Breite = 2 x Abstandstransformation): Port = sichtbare Schwanz-Bildpunkte der ganzen Figur (f_rast, Knochen 16-22), Video = Nutzer-Umriss
h90 (Areas.png - Videobild n1439)."""
import os, sys, json
import numpy as np
PR = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr8'
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V
import f_rast as FR
cwd = os.getcwd(); os.chdir(PR); sys.path.insert(0, PR)
import a_mass as A
um, _r, _g = A.nutzer_maske(90)
os.chdir(cwd)
AB = (10, 30, 50, 70, 90, 110, 130, 150, 170, 190, 210)
v = A.profil_abs(um, (366, 213), AB)
ys, xs = np.nonzero(um)
print('Video h90 (Nutzer-Umriss): L %.1f, tiefster Punkt y %d' % (v['L'], ys.max()), v['breite'])
for p in sys.argv[1:]:
    mo = V.modell(open(p, 'rb').read())
    A_ = FR.mit_bg(mo, V.PV, licht=False)
    m = (A_['bone'][:480] >= 16) & (A_['bone'][:480] <= 22)
    r = A.profil_abs(m, (376, 220), AB)
    pr = A.profil(m, (376, 220))
    ys, xs = np.nonzero(m)
    print(os.path.basename(p), 'L %.1f, tiefster sichtbarer Punkt y %d (x %d), Spitze der Mittellinie %s' % (r['L'], ys.max(), xs[np.argmax(ys)], pr['spitze']), r['breite'])
