# Abnahme 8 (eigenes Werkzeug): Schub waehrend der RE2-Reaktion - je Reaktionsbild Bildschritt der Figur (state.log) minus RE2-Hakenschritt (GTR "schritt");
# Summe/Max je Reaktion: "Schub-Weg" = was Koerper-/Boss-Schub und Klemme zusaetzlich bewegen. Letzter Besuch.
# Aufruf: python a8schub.py <lauf>
import sys, re, os, math
d = sys.argv[1]
st = open(os.path.join(d, 'state.log'), encoding='latin-1').read().splitlines()
rs = re.compile(r'^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) ps1=(\d+)')
fr = []
for z in st:
    m = rs.match(z)
    if m: fr.append(tuple(int(x) for x in m.groups()))
lv = []; cur = []
for f in fr:
    if cur and f[0] <= cur[-1][0]: lv = cur; cur = []
    cur.append(f)
lv = cur if cur else lv
pos = {f[0]: (f[1], f[2]) for f in lv}
gb = open(os.path.join(d, 'gator_boss.log'), encoding='latin-1').read().splitlines()
# letzter Besuch in gator_boss.log: ab der letzten "F2 R209" Zeile
start = max([i for i, z in enumerate(gb) if z.startswith('F2 R209')] or [0])
rg = re.compile(r'^GTR F(\d+) sub (\d) ph \d+ bild (\d+) schritt (-?\d+) ')
reak = []; cur = None
for z in gb[start:]:
    if z.startswith('GTR F') and ' START ' in z:
        if cur: reak.append(cur)
        cur = dict(F=int(z.split()[1][1:]), schub=0.0, smax=0, bilder=0, schub_liegend=0.0)
    m = rg.match(z)
    if m and cur is not None:
        F = int(m.group(1)); s = abs(int(m.group(4))); b = int(m.group(3))
        if F in pos and F - 1 in pos:
            w = math.hypot(pos[F][0] - pos[F-1][0], pos[F][1] - pos[F-1][1])
            sch = max(0.0, w - s)
            cur['schub'] += sch; cur['smax'] = max(cur['smax'], int(sch)); cur['bilder'] += 1
            if int(m.group(2)) == 3 and b >= 7: cur['schub_liegend'] += sch
if cur: reak.append(cur)
if not reak: print('%s: keine Reaktionen' % os.path.basename(os.path.normpath(d))); sys.exit()
reak.sort(key=lambda r: -r['schub'])
gross = [r for r in reak if r['schub'] > 1500]
print('%s: %d Reaktionen; Schub-Weg je Reaktion max %d (F%d), Mittel %d; Reaktionen mit Schub-Weg > 1500: %d; groesster Einzelschub je Bild %d; '
      'Schub im Sub 3 ab Bild 7 (Figur am Boden/steht auf) max %d' % (os.path.basename(os.path.normpath(d)), len(reak), int(reak[0]['schub']), reak[0]['F'],
      int(sum(r['schub'] for r in reak) / len(reak)), len(gross), max(r['smax'] for r in reak), int(max(r['schub_liegend'] for r in reak))))
