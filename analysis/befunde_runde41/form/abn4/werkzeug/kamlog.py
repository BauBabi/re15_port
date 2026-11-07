# Abnahme 4 (eigenes Werkzeug): kam.log der Szenenwahl auswerten - je Bild gewaehlter Schnitt, ob er nach dem Bildmass "gut" (sz=1) war,
# Rueckfall-Bilder (gewaehlt ohne gute Kamera), Schnittwechsel mit Begruendung (Zonenkamera der Form / aktueller Schnitt / Zonenkamera des
# Spielers / groesstes Formbild / Rueckfall).   python kamlog.py <laufdir>
import sys, os, re
d = sys.argv[1]
def zone(x):
    return 11 if x < 2750 else 12 if x < 14000 else 13 if x < 20250 else 14
rows = []
for line in open(os.path.join(d, 'kam.log')):
    m = re.match(r'F(\d+) (\w+) wahl=(\d+) form=\((-?\d+),(-?\d+)\) pl=\((-?\d+),(-?\d+)\)(.*)', line)
    if not m: continue
    f, wer, wahl, fx, fz, px, pz, rest = int(m.group(1)), m.group(2), int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)), int(m.group(7)), m.group(8)
    kams = {}
    for k in re.finditer(r'\| c(\d+) ok=(\d) nah=(\d) hi=(\d+) f=(\d+) in=(\d+) mi=(\d) p=(\d+) pi=(\d) df=(\d+) dp=(\d+) pn=(\d) sz=(\d)', rest):
        kams[int(k.group(1))] = dict(ok=int(k.group(2)), nah=int(k.group(3)), hi=int(k.group(4)), f=int(k.group(5)), inn=int(k.group(6)), mi=int(k.group(7)), p=int(k.group(8)), pi=int(k.group(9)), df=int(k.group(10)), dp=int(k.group(11)), pn=int(k.group(12)), sz=int(k.group(13)))
    rows.append((f, wer, wahl, fx, px, kams))
name = os.path.basename(d.rstrip('/'))
if not rows:
    print(name + ': kein kam.log'); sys.exit(0)
rueck = [r for r in rows if r[2] in r[5] and r[5][r[2]]['sz'] == 0]
ohne = [r for r in rows if r[2] not in r[5]]
print('%s: %d Szenenbilder F%d-F%d, Kandidaten je Bild %s; Rueckfall (gewaehlt, aber nicht gut): %d%s; gewaehlt nicht unter den Kandidaten: %d' % (
    name, len(rows), rows[0][0], rows[-1][0], sorted({tuple(sorted(r[5])) for r in rows}), len(rueck),
    (' (F%d..F%d)' % (rueck[0][0], rueck[-1][0])) if rueck else '', len(ohne)))
prev = None
for r in rows:
    f, wer, wahl, fx, px, k = r
    if wahl != prev:
        zf, zp = zone(fx), zone(px)
        grund = 'Zonenkamera der Form' if wahl == zf else 'Zonenkamera des Spielers' if wahl == zp else 'groesstes gutes Formbild'
        gut = ' '.join('c%d%s' % (c, '' if v['sz'] else '-') for c, v in sorted(k.items()))
        print('  F%d wahl=%d (%s; Form x %d -> Zone %d, Spieler x %d -> Zone %d) | gut/nicht: %s | c%d: f=%d p=%d df=%d nah=%d pn=%d' % (
            f, wahl, grund, fx, zf, px, zp, gut, wahl, k[wahl]['f'], k[wahl]['p'], k[wahl]['df'], k[wahl]['nah'], k[wahl]['pn']))
        prev = wahl
