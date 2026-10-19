"""B1 "wie man sich bewegt": der Spieler laeuft geradlinig durch die Gang-Abschnitte (probe_r39_karten weg = echter
Kartenpfad je 100 Einheiten) und an jeder B1-Tuer von der einen auf die andere Seite (probe_r39_karten marker).
Gemessen: Kartenpixel je 1000 Einheiten LAENGS der Laufrichtung (gleichmaessig waere 1000/450 = 2,22 bei RE2-Massstab),
Querversatz, groesster Einzelsprung, Zonenwechsel; Tuer: Sprung des Markers beim Durchgang.
Aufruf: b1_weg.py <probe.exe>"""
import sys, subprocess, re
P = sys.argv[1]
# Wege im Rahmen von ROOM1180 (Mitten der Gang-Abschnitte aus der Flut, Spielerradius 450 vor den Waenden)
WEGE = [
    ('Suedbein nach Norden', 0x1180, -4142, -15900, -4142, -7800),
    ('Versatz nach Osten',   0x1180, -5100, -5777, 5000, -5777),
    ('Laengsgang n. Norden', 0x1180, 3858, -3800, 3858, 30500),
    ('Querarm nach Westen',  0x1180, 5000, 13723, -9000, 13723),
    ('Westbein n. Sueden',   0x1180, -7942, 13700, -7942, 9950),
]
def weg(raum, x0, z0, x1, z1):
    n = max(1, int(max(abs(x1 - x0), abs(z1 - z0)) / 100))
    out = subprocess.run([P, 'weg', '%04X' % raum, '0', '0', str(x0), str(z0), str(x1), str(z1), str(n)],
                         capture_output=True, text=True).stdout
    pts = []
    for l in out.splitlines():
        if l.startswith('weg '):
            k, wx, wz, page, idx, zid, mx, my = map(int, l.split()[1:])
            pts.append((wx, wz, page, idx, zid, mx, my))
    return pts
print('%-22s %8s %8s %8s %8s %6s %s' % ('Weg', 'Welt', 'px laengs', 'px/1000', 'quer px', 'max', 'Zonen'))
for (name, raum, x0, z0, x1, z1) in WEGE:
    pts = weg(raum, x0, z0, x1, z1)
    if len(pts) < 2: print(name, 'KEINE PUNKTE'); continue
    dx, dz = x1 - x0, z1 - z0
    lang = abs(dx) + abs(dz)
    # Kartenrichtung: x -> mx, z -> -my
    if abs(dx) > abs(dz): laengs = [p[5] for p in pts]; quer = [p[6] for p in pts]; s = 1 if dx > 0 else -1
    else: laengs = [-p[6] for p in pts]; quer = [p[5] for p in pts]; s = 1 if dz > 0 else -1
    weg_px = (laengs[-1] - laengs[0]) * s
    maxsprung = max(abs(pts[i][5] - pts[i - 1][5]) + abs(pts[i][6] - pts[i - 1][6]) for i in range(1, len(pts)))
    zonen = sorted(set(p[4] for p in pts))
    print('%-22s %8d %8d %8.2f %8d %6d %s' % (name, lang, weg_px, 1000.0 * weg_px / lang, max(quer) - min(quer),
                                               maxsprung, zonen))
# Tueren: Marker im Tuer-Trigger (Seite A) gegen Marker an der Ankunft (Seite B)
TUEREN = [  # (Name, Raum A, Trigger-Mitte A, Band A, Raum B, Ankunft B, Band B)
    ('Gang -> 1190 Halle', 0x1180, (-8118, 9355), 0, 0x1190, (3174, -11182), 0),
    ('Gang -> 1190 Ostk.', 0x1230, (96, 12161), 0, 0x1190, (19515, -12109), 0),
    ('Gang -> 11B0',       0x1180, (5706, 28874), 0, 0x11B0, (-26483, -18094), 0),
    ('Gang -> 10A0 B1',    0x1180, (-177, -7772), 0, 0x10A0, (21200, 25500), 4),
    ('Gang -> 11D0',       0x1180, (-4837, -16839), 0, 0x11D0, (-300, -17400), 0),
    ('11B0 -> 11C0',       0x11B0, (-25480, -28190), 0, 0x11C0, (-25279, 17268), 0),
    ('1190 Halle -> Gang', 0x1190, (3000, -9650), 0, 0x1180, (-8133, 10226), 0),
    ('11C0 -> 11B0',       0x11C0, (-25100, 17250), 0, 0x11B0, (-25552, -25331), 0),
]
def marker(raum, p, band):
    y = -7200 if band == 4 else 0
    out = subprocess.run([P, 'marker', '%04X' % raum, str(p[0]), str(p[1]), str(band), str(y)],
                         capture_output=True, text=True).stdout
    m = re.search(r'Blatt (\d+).*Marker \((-?\d+),(-?\d+)\)', out)
    return (int(m.group(1)), int(m.group(2)), int(m.group(3))) if m else None
print()
print('%-20s %16s %16s %8s' % ('Tuer', 'Marker A', 'Marker B', 'Sprung'))
for (name, ra, pa, ba, rb, pb, bb) in TUEREN:
    a = marker(ra, pa, ba); b = marker(rb, pb, bb)
    if not a or not b: print(name, 'FEHLT', a, b); continue
    print('%-20s %16s %16s %8d' % (name, a, b, abs(a[1] - b[1]) + abs(a[2] - b[2]) if a[0] == b[0] else -1))
