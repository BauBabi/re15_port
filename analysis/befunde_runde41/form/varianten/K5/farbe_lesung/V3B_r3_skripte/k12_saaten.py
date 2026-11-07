"""K12 Schwanz (und Zonen) fuer Saaten 15/12345/7 und K1v2. python k12_saaten.py <Variante>"""
import sys, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import riegel_farbe_v3 as RV
M = RV.masken_daten(); M['nur_k11_13'] = True
for s in (15, 12345, 7):
    RV.SAAT = s
    r = RV.ketten_v3(sys.argv[1], M)
    print('Saat', s, {k[:4]: v[0] for k, v in r.items() if k.startswith(('K11', 'K12'))})
