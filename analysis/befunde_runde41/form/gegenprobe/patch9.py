#!/usr/bin/env python3
"""Neunter Umbau: bericht.py fuellt Lage, Gier, Wurzel aus gerippe.json und liest Profile."""
import os
H = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gerippe')


def ers(t, a, b):
    assert a in t, a[:60]
    return t.replace(a, b)


p = os.path.join(H, 'bericht.py')
t = open(p, encoding='utf-8').read()
t = ers(t, """           '{{MERKMALE}}': t_merkmale(), '{{PRUEF}}': t_pruef()}
""", """           '{{MERKMALE}}': t_merkmale(), '{{PRUEF}}': t_pruef()}
    for nr in (1, 2):
        p = g['posen']['bild%d' % nr]
        ers['{{LAGE%d}}' % nr] = '(%d, %d, %d)' % tuple(p['lage'])
        ers['{{GIER%d}}' % nr] = '%d (%s Grad)' % (p['gier'], str(p['gier_grad']).replace('.', ','))
        ers['{{WURZEL%d}}' % nr] = '%d' % -p['wurzel'][1]
""")
open(p, 'w', encoding='utf-8', newline='\n').write(t)
print('ok')
