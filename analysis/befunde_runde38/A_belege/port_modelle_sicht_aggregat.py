"""port_modelle_sicht_aggregat.py - Runde 38 Spur A: fasst die Laeufe von port_modelle_sicht.py je Raum und Port-Platz zusammen
(Cuts ganz vorne / gemischt / nur Flaeche 0 / KEIN MAC0>0 = mit dem Verwurf unsichtbar). Aufruf: python port_modelle_sicht_aggregat.py <ausgabeordner> [alt|neu]"""
import os, sys, glob, importlib.util
s = importlib.util.spec_from_file_location('m', r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_cursor/analysis/befunde_runde38/A_belege/port_modelle_sicht.py')
m = importlib.util.module_from_spec(s); s.loader.exec_module(m)
B = sys.argv[1]; art = sys.argv[2] if len(sys.argv) > 2 else 'alt'
from collections import defaultdict
je = defaultdict(list)
for f in sorted(glob.glob(B + '/*/c*/slots_%s.txt' % art)):
    teile = os.path.normpath(f).split(os.sep)
    raumdir = teile[-3]; cut = int(teile[-2][1:])
    elza = raumdir.endswith('e'); raum = raumdir[:4]
    z = open(f).read().split('\n')
    ist = z[0].split()[1] if z and z[0] else '?'
    nom = m.rdt_kopf(raum, elza)[1]
    for l in z[1:]:
        if not l.strip(): continue
        slot, neg, null, pos = map(int, l.split())
        if slot not in m.PORT_PLATZ or m.platz_obj(slot) < nom: continue
        je[(raum + ('e' if elza else ''), slot)].append((cut, neg, null, pos, ist))
for (r, slot), L in sorted(je.items()):
    weg = [c for c, n, z, p, i in L if n > 0 and p == 0]
    teil = [c for c, n, z, p, i in L if n > 0 and p > 0]
    ganz = [c for c, n, z, p, i in L if n == 0 and p > 0]
    nul = [c for c, n, z, p, i in L if n == 0 and p == 0]
    print('%-6s Platz %2d (obj %2d): Cuts mit Modell %2d | ganz vorne %s | gemischt %s | nur Flaeche 0 %s | KEIN MAC0>0 (mit <0) %s' % (r, slot, m.platz_obj(slot), len(L), ganz, teil, nul, weg))
