"""NB4 N4: 6030 Abgang - Leon tritt zur Seite, Annette geht zur Tuer. Ueberdeckung im Cut 0 (ID-Puffer, Annette-Netz als Koerper
fuer beide). Waende 6030: x -4341 (Tuerwand) .. -1322 (Pult), z -26410 .. -20430."""
import sys, math, numpy as np
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/analysis/befunde_runde41/AN_belege/werkzeug/n3')
import szmass as SZ
import form_buehne as FB
R6 = FB.Raum('6030')
def kam(c):
    s = R6.schnitt(c); return FB.Kamera.aus_rid(s['pos'], s['ziel'], s['fov'])
SZ.kam = kam
FRAU = SZ.FRAU
PULT = (-1790, -22272); LEON0 = (-3873, -23487)
def mass(A, L, cut=0, ga=None, gl=None):
    ga = SZ.blick(*A, -4800, A[1]) if ga is None else ga        # Blick zur Tuer (Westen)
    gl = SZ.blick(*L, *A) if gl is None else gl
    buf, al = SZ.idpuffer(cut, [(FRAU, 1, 0, (A[0], 0, A[1]), ga, 2), (FRAU, 1, 0, (L[0], 0, L[1]), gl, 3)])
    a, l = al[2], al[3]
    fa = (buf == 2).sum() / max(a.sum(), 1); fl = (buf == 3).sum() / max(l.sum(), 1)
    return round(fa, 2), round(fl, 2), (a.sum(), l.sum())
def frei(x, z, r=450):
    return -4341 + r <= x <= -1322 - r and -26410 + r <= z <= -20430 - r
print('heute: A (-3873,-22550), Leon bleibt', mass((-3873, -22550), LEON0))
for A in ((-3873, -22550), (-3873, -23000), (-3873, -23350)):
    for lx in range(-3600, -1700, 300):
        for lz in range(-25800, -22000, 300):
            if not frei(lx, lz) or math.hypot(lx - A[0], lz - A[1]) < 1000: continue
            fa, fl, n = mass(A, (lx, lz))
            if fa >= 0.98 and fl >= 0.9:
                print('A', A, 'Leon', (lx, lz), 'Weg Leon', int(math.hypot(lx - LEON0[0], lz - LEON0[1])), 'frei A/L', fa, fl, n)
