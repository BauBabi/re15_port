# Zusammenfassung Stufe 3 Runde 2 aus auswertung.json: je Stage/Figur, Mitlaufen (tN vs wN), byte-true-Abgleich.
import json, sys, collections
erg = json.load(open(sys.argv[1]))
by = {r['tag']: r for r in erg}
z = collections.defaultdict(collections.Counter)
for r in erg:
    z[(r['tag'][0], r['rid'][0])][r['urteil']] += 1
for k in sorted(z): print('Figur %s STAGE%s' % k, dict(z[k]))
# Mitlaufen: Lauf wN gegen Ankunft tN desselben Raums
n = bew = still = 0; liste = []
for r in erg:
    if not r['geh'] or r['urteil'] == 'KEIN ABZUG': continue
    t = by.get(r['tag'].replace('_w', '_t'))
    if not t or not t.get('marker') or not r.get('marker') or not t.get('lage') or not r.get('lage'): continue
    dw = max(abs(r['lage'][0] - t['lage'][0]), abs(r['lage'][1] - t['lage'][1]))
    dm = max(abs(r['marker'][0] - t['marker'][0]), abs(r['marker'][1] - t['marker'][1]))
    if dw < 1000: continue                     # kaum gelaufen (Wand direkt vor der Tuer)
    n += 1
    if dm >= 2: bew += 1
    else: still += 1; liste.append((r['tag'], dw, dm))
print('Mitlaufen: %d Laeufe >= 1000 Einheiten, Punkt bewegt (>= 2 px) %d, steht %d %s' % (n, bew, still, liste))
# byte-true
b = [r for r in erg if 'formel_px' in r]
c = collections.Counter(min(r['formel_px'], 9) for r in b)
print('Ausgelieferte Zeile: %d Abzuege, Abstand Punkt - Original-Formel (px): %s' % (len(b), dict(sorted(c.items()))))
for r in b:
    if r['formel_px'] > 1: print('   ', r['tag'], r['marker'], r['formel'], r['formel_va'], r['formel_px'])
