"""Schnellansicht Szene (keine Nutzer-Tafel): Original | Variante roh (Spiellicht) | Variante gefiltert (Saat 15), Ausschnitt.
python szene_blick.py <Variante> <bild 1|2> <x0> <y0> <x1> <y1> <faktor> <aus.png>"""
import sys, os, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import riegel_farbe_v3 as RV
import p_zonen as PZ
v, n = sys.argv[1], int(sys.argv[2]); x0, y0, x1, y1, f = map(int, sys.argv[3:8]); aus = sys.argv[8]
k = PZ.P.Kandidat(os.path.join(v, 'form.json'))
s, alb, m, kk = k.rendern(n)
vid = np.asarray(PZ.FV.video_filter(s, bild=n, entzerren=False, rauschen=True, saat=15), np.uint8)
o = np.asarray(PZ.P.original(n), np.uint8)
ks = [cv2.resize(np.asarray(b, np.uint8)[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST) for b in (o, s, vid)]
cv2.imwrite(aus, cv2.cvtColor(np.hstack(ks), cv2.COLOR_RGB2BGR))
