import sys, json, math
import numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/tools')
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/B_skripte')
import form_video as FV
import fl, zonen as Z
FORM = fl.FORM
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/leserb/'
LICHT = Z.LICHT
def lade(n):
    V = cv2.imread(FORM + '/bild%d.png' % n)[:, :, ::-1].copy()
    e, T, clip = Z.rueckrechnen(V, n)
    return V, e, T, clip
def lab(a):
    return fl.lab(np.clip(np.round(np.asarray(a, float)), 0, 255).astype(np.uint8).reshape(-1, 3))
def lab1(c):
    return fl.lab1([int(round(x)) for x in np.clip(c, 0, 255)])
def ziel(n):
    return cv2.imread(FORM + '/ziel2_bild%d.png' % n, 0)
def hue(L):
    return np.degrees(np.arctan2(L[..., 2], L[..., 1]))
