# Abnahme 8 (eigenes Werkzeug): Bahnen von Figur und Gator (letzter Besuch, state.log) mit Kanister-Liegestelle; Kennzahlen zur Kanister-Naehe.
# Aufruf: python a8bahn.py <lauf> <aus.png> <titel>
import sys, re, os, math
import matplotlib; matplotlib.use('Agg')
import matplotlib.pyplot as plt
d, aus, titel = sys.argv[1], sys.argv[2], sys.argv[3]
st = open(os.path.join(d, 'state.log'), encoding='latin-1').read().splitlines()
rs = re.compile(r'^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+)')
rg = re.compile(r' mo=(\d+) af=(\d+) stun=(\d+) d=\S+ @\((-?\d+),(-?\d+),r(-?\d+)\)')
fr = []
for z in st:
    m = rs.match(z); g = rg.search(z)
    if m: fr.append((int(m.group(1)), int(m.group(2)), int(m.group(3)), int(g.group(4)) if g else None, int(g.group(5)) if g else None, int(m.group(5)), int(m.group(6))))
lv = []; cur = []
for f in fr:
    if cur and f[0] <= cur[-1][0]: lv = cur; cur = []
    cur.append(f)
lv = cur
kan = None; auf = None; ang = None
for z in open(os.path.join(d, 'debug.log'), encoding='latin-1', errors='replace'):
    m = re.search(r'Kanister im Becken \((-?\d+),(-?\d+),(-?\d+)\)', z)
    if m: kan = (int(m.group(1)), int(m.group(3)))
for z in open(os.path.join(d, 'gator_boss.log'), encoding='latin-1', errors='replace'):
    m = re.search(r'KANISTER Stufe 1 laengs=(-?\d+) quer=(-?\d+) .* F(\d+)', z)
    if m and auf is None: auf = int(m.group(3))
    m = re.search(r'KANISTER angehaengt .* F(\d+)', z)
    if m and ang is None: ang = int(m.group(1))
fig, ax = plt.subplots(figsize=(7, 9))
px = [f[1] for f in lv]; pz = [f[2] for f in lv]
gx = [f[3] for f in lv if f[3] is not None]; gz = [f[4] for f in lv if f[3] is not None]
ax.plot(gx, gz, '-', color='#999999', lw=0.6, label='Gator (Wurzel)')
ax.plot(px, pz, '-', color='#1f77b4', lw=0.9, label='Figur')
ax.add_patch(plt.Rectangle((2318, -17650), 6732 - 2318, 17650 - 14032, fill=False, ec='#2ca02c', lw=1.2, label='Rampen-Deck (y -1800)'))
if kan: ax.plot([kan[0]], [kan[1]], 's', color='#d62728', ms=8, label='Kanister-Liegestelle')
if auf:
    a = [f for f in lv if f[0] == auf]
    if a: ax.plot([a[0][3]], [a[0][4]], '*', color='#ff7f0e', ms=14, label='Gator bei Stufe 1 F%d' % auf)
ax.set_aspect('equal'); ax.invert_yaxis(); ax.set_xlabel('x'); ax.set_ylabel('z'); ax.set_title(titel, fontsize=9); ax.legend(fontsize=7, loc='lower left')
fig.savefig(aus, dpi=110, bbox_inches='tight')
# Kennzahlen: Gator-Wurzel naeher als 4000 / 2500 an der Liegestelle, solange der Kanister liegt (bis Stufe 1)
if kan:
    bis = auf if auf else lv[-1][0]
    nah = [(f[0], int(math.hypot(f[3] - kan[0], f[4] - kan[1]))) for f in lv if f[3] is not None and f[0] < bis]
    dmin = min(nah, key=lambda t: t[1]) if nah else None
    fig_nah = [f[0] for f in lv if f[0] < bis and math.hypot(f[1] - kan[0], f[2] - kan[1]) < 4000]
    print('%s: Kanister %s; Stufe 1 %s, angehaengt %s; bis dahin Gator-Wurzel kleinster Abstand %s (F%s); Bilder Gator < 4000: %d, < 2500: %d; Figur < 4000 an der Liegestelle: %d Bilder (erstes F%s)' % (
          os.path.basename(os.path.normpath(d)), kan, ('F%d (%.1f s)' % (auf, auf / 30.0)) if auf else 'keine', ('F%d' % ang) if ang else '-', dmin[1] if dmin else '-', dmin[0] if dmin else '-',
          sum(1 for t in nah if t[1] < 4000), sum(1 for t in nah if t[1] < 2500), len(fig_nah), fig_nah[0] if fig_nah else '-'))
