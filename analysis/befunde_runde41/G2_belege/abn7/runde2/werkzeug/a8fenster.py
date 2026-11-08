# Abnahme 8 (eigenes Werkzeug): Lage des liegenden Kanisters relativ zum Gator je Bild (letzter Besuch): laengs = Projektion auf die Schwimmrichtung
# (cos th, -sin th) wie kanister2080_r41.c 'vor', quer = senkrecht dazu; dazu Gator-Clip (0/1 Verfolgung, 4 Biss-Anlauf). Zeigt, warum keine Stufe 1 kam.
# Aufruf: python a8fenster.py <lauf> [max_abstand]
import sys, re, os, math
d = sys.argv[1]; dmax = int(sys.argv[2]) if len(sys.argv) > 2 else 2500
st = open(os.path.join(d, 'state.log'), encoding='latin-1').read().splitlines()
rs = re.compile(r'^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+)')
rg = re.compile(r' mo=(\d+) af=(\d+) stun=(\d+) d=\S+ @\((-?\d+),(-?\d+),r(-?\d+)\)')
fr = []
for z in st:
    m = rs.match(z); g = rg.search(z)
    if m and g: fr.append((int(m.group(1)), int(g.group(1)), int(g.group(2)), int(g.group(4)), int(g.group(5)), int(g.group(6)), int(m.group(2)), int(m.group(3))))
lv = []; cur = []
for f in fr:
    if cur and f[0] <= cur[-1][0]: lv = cur; cur = []
    cur.append(f)
lv = cur
kan = None; auf = None
for z in open(os.path.join(d, 'debug.log'), encoding='latin-1', errors='replace'):
    m = re.search(r'Kanister im Becken \((-?\d+),(-?\d+),(-?\d+)\)', z)
    if m: kan = (int(m.group(1)), int(m.group(3)))
for z in open(os.path.join(d, 'gator_boss.log'), encoding='latin-1', errors='replace'):
    m = re.search(r'KANISTER Stufe 1 .* F(\d+)', z)
    if m and auf is None: auf = int(m.group(1))
n = 0; fenster = 0; lage = {}
for F, mo, af, gx, gz, r, px, pz in lv:
    if auf and F >= auf: break
    dx, dz = kan[0] - gx, kan[1] - gz
    if math.hypot(dx, dz) > dmax: continue
    th = r * 2 * math.pi / 4096
    la = math.cos(th) * dx - math.sin(th) * dz
    qu = math.sin(th) * dx + math.cos(th) * dz
    n += 1
    k = 'Verfolgung' if mo in (0, 1) else ('Biss-Anlauf' if mo == 4 else 'Clip %d' % mo)
    inw = (0 <= la < 9000 * 2731 / 4096 * 1.0) and abs(qu) < 1733
    lage.setdefault(k, [0, 0]); lage[k][0] += 1; lage[k][1] += 1 if inw else 0
    if inw: fenster += 1
print('%s: Kanister %s, Stufe 1 %s; Bilder mit Gator-Wurzel <= %d: %d; davon Kanister im Fenster laengs [0,%d) |quer| < 1733: %d; nach Gator-Clip: %s' % (
      os.path.basename(os.path.normpath(d)), kan, ('F%d' % auf) if auf else 'keine', dmax, n, int(9000 * 2731 / 4096), fenster,
      ', '.join('%s %d (im Fenster %d)' % (k, v[0], v[1]) for k, v in lage.items())))
