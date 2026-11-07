"""Schnellriegel fuer Bau-Iterationen (keine Abnahme): Ketten K3-K15, K1v2/K2v2/K4b, F1-Flaeche. python schnell.py <Variante>"""
import sys, os, time
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import riegel_farbe_v3 as RV
import f1_flaeche as FF
import p_zonen as PZ
v = sys.argv[1]
t = time.time()
res = {}
for k, (val, ok) in RV.PR.kette_riegel(v).items():
    if not k.startswith(('K1 ', 'K2 ')): res[k] = (val, ok)
for k, (val, ok) in RV.RF.kette_v2(v).items(): res[k] = (val, ok)
for k, (val, ok) in RV.ketten_v3(v, RV.masken_daten()).items(): res[k] = (val, ok)
for k, (val, ok) in res.items():
    print('  %-55s %-80s %s' % (k[:55], str(val)[:120], 'GRUEN' if ok else 'ROT'))
k = PZ.P.Kandidat(os.path.join(v, 'form.json'))
s, alb, m, kk = k.rendern(1)
c, d = FF.flaeche(PZ.FV.video_filter(s, bild=1, entzerren=False, rauschen=True, saat=15))
print('  F1-Flaeche', d, 'zeit', round(time.time() - t))
