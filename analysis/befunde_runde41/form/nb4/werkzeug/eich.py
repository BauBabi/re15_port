# Eichung Kugelmodell (kam.log, Promille Rahmen) gegen gezeichnete Bildpunkte (deck.log, Slot der Form / Spieler-Slot 0).
#   python eich.py <laufdir> [formslot]
import sys, os, re
d = sys.argv[1]
fs = int(sys.argv[2]) if len(sys.argv) > 2 else 12
kam = {}
for line in open(os.path.join(d, 'kam.log')):
    m = re.match(r'F(\d+) (\w+) wahl=(\d+)', line)
    if not m: continue
    f = int(m.group(1))
    cams = {}
    for c in re.finditer(r'c(\d+) ok=\d nah=(\d) hi=(\d+) f=(\d+) in=(\d+) mi=(\d) p=(\d+) pi=(\d) df=(\d+) dp=(\d+)', line):
        cams[int(c.group(1))] = dict(nah=int(c.group(2)), f=int(c.group(4)), inn=int(c.group(5)), p=int(c.group(7)), df=int(c.group(9)), dp=int(c.group(10)))
    kam[f] = (m.group(2), int(m.group(3)), cams)
rows = []
for line in open(os.path.join(d, 'deck.log')):
    m = re.match(r'F(\d+) cam=(-?\d+)(.*)', line)
    if not m: continue
    f, cam = int(m.group(1)), int(m.group(2))
    sl = {int(a): (int(b), int(c)) for a, b, c in re.findall(r' s(\d+)=(\d+)/(\d+)', m.group(3))}
    # das Bild F zeigt die Wahl aus F-1 oder F (Kamerawechsel wird im naechsten Bild angewandt)
    for g in (f, f - 1):
        if g in kam and kam[g][1] == cam and cam in kam[g][2]:
            c = kam[g][2][cam]
            fv, fb = sl.get(fs, (0, 0)); pv, pb = sl.get(0, (0, 0))
            rows.append((f, kam[g][0], cam, c['f'], fb * 1000 // 76800, fv * 1000 // 76800, c['p'], pb * 1000 // 76800, c['df'],
                         (fb - fv) * 1000 // fb if fb else -1))
            break
print('F wer cam | f_modell f_bedeckt f_sicht | p_modell p_bedeckt | df_modell f_verdeckt')
for r in rows:
    print('F%d %s c%d | %d %d %d | %d %d | %d %d' % r)
