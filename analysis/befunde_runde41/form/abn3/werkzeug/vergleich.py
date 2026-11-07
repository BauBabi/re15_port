# Abnahme 3 (eigenes Werkzeug): zwei Laeufe je Bild vergleichen (Spieler x/z/hp, Form x/z/mo/af/hp/st) - Spiel gleich, nur Kamera anders?
#   python vergleich.py <lauf_a> <lauf_b> <von> <bis>
import sys, os, re
RX = re.compile(r'^F(\d+) pad=([0-9A-Fa-f]+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? cam=(\d+)')
RXF = re.compile(r'\[(\d+) t=30 st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) af=(\d+) stun=(\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')


def lese(d, von, bis):
    z = {}
    for line in open(os.path.join(d, 'state.log'), errors='replace'):
        m = RX.match(line)
        if not m:
            continue
        f = int(m.group(1))
        if f in z or not (von <= f <= bis):
            continue
        e = RXF.search(line)
        z[f] = ((int(m.group(3)), int(m.group(4)), int(m.group(6))),
                (int(e.group(11)), int(e.group(12)), int(e.group(7)), int(e.group(8)), int(e.group(14)), int(e.group(2))) if e else None,
                int(m.group(7)))
    return z


a, b, von, bis = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
za, zb = lese(a, von, bis), lese(b, von, bis)
gl = ungl = camgl = 0
erste = []
for f in sorted(set(za) & set(zb)):
    if za[f][:2] == zb[f][:2]:
        gl += 1
    else:
        ungl += 1
        if len(erste) < 5:
            erste.append((f, za[f][:2], zb[f][:2]))
    if za[f][2] == zb[f][2]:
        camgl += 1
print('%s vs %s F%d-F%d: %d Bilder gemeinsam, Spieler+Form gleich %d, ungleich %d; Kamera gleich %d' % (
    os.path.basename(a.rstrip('/')), os.path.basename(b.rstrip('/')), von, bis, len(set(za) & set(zb)), gl, ungl, camgl))
for e in erste:
    print('  ungleich F%d: %s | %s' % e)
