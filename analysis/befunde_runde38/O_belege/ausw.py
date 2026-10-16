# ausw.py <debug.log> [raum ...] : je Montage-Raum und Kamera-Schnitt (aus den [irons-tod-mess]-Zeilen) je Zombie
# Abstand zur Kamera-Bodenprojektion (RDT RID, Datei-Offsets s. cuts.py) am ersten/letzten Takt des Schnitts, Weg,
# Tiefe vor der Kamera (Skalarprodukt mit der Blickrichtung xz) und ob er sich bewegt.
import re, sys, math
CAM = {  # (raum, cut): (pos_x, pos_z, target_x, target_z)  — RDT camera_start + cut*32, +4/+0xC/+0x10/+0x18
    ('1130', 0): (-2386, -7451, 876, -18973),
    ('1040', 1): (-26262, -10692, -25020, -1818),
    ('1030', 7): (-26820, -2574, -15858, -5040),
    ('1030', 6): (-11790, -15282, -8640, -26478),
    ('1030', 12): (-11790, -15282, -8640, -26478),
}
rx = re.compile(r'\[irons-tod-mess\] ROOM(\w+) T(\d+) cam=(\d+):(.*) PL\((-?\d+),(-?\d+)\) hp=(-?\d+)')
ra = re.compile(r' (\d+):(\w\w)\((-?\d+),(-?\d+)\)->\((-?\d+),(-?\d+)\) g=(\w\w) s=(\d+) st=(\d+) fl=(\w+) mo=(\d+)')
log = sys.argv[1]; raeume = sys.argv[2:]
rows = []
besuch = {}
for line in open(log, errors='replace'):
    m = rx.search(line)
    if not m: continue
    raum, t, cam = m.group(1), int(m.group(2)), int(m.group(3))
    if raeume and raum not in raeume: continue
    akt = {int(a.group(1)): (a.group(2), int(a.group(3)), int(a.group(4)), a.group(7), int(a.group(8)), a.group(10), int(a.group(11))) for a in ra.finditer(m.group(4))}
    rows.append((raum, t, cam, akt, (int(m.group(5)), int(m.group(6))), int(m.group(7))))
# Abschnitte = (raum, cam) zusammenhaengend
abschn = []
for r in rows:
    if abschn and abschn[-1][0] == r[0] and abschn[-1][1] == r[2] and r[1] >= abschn[-1][2][-1][1]:
        abschn[-1][2].append(r)
    else:
        abschn.append((r[0], r[2], [r]))
for raum, cam, rs in abschn:
    c = CAM.get((raum, cam))
    t0, t1 = rs[0][1], rs[-1][1]
    print('ROOM%s Cut %d  T%d..T%d (%d Messzeilen), Spieler %s hp %d..%d' % (raum, cam, t0, t1, len(rs), rs[0][4], max(x[5] for x in rs), min(x[5] for x in rs)))
    if not c:
        print('   (keine Kamera hinterlegt)'); continue
    fx, fz = c[2] - c[0], c[3] - c[1]; n = math.hypot(fx, fz); fx /= n; fz /= n
    ids = sorted(set().union(*[set(x[3]) for x in rs]))
    for i in ids:
        seq = [(x[1], x[3][i]) for x in rs if i in x[3]]
        (ta, a), (tb, b) = seq[0], seq[-1]
        da = math.hypot(a[1] - c[0], a[2] - c[1]); db = math.hypot(b[1] - c[0], b[2] - c[1])
        tiefe_a = (a[1] - c[0]) * fx + (a[2] - c[1]) * fz; tiefe_b = (b[1] - c[0]) * fx + (b[2] - c[1]) * fz
        weg = sum(math.hypot(seq[k+1][1][1] - seq[k][1][1], seq[k+1][1][2] - seq[k][1][2]) for k in range(len(seq) - 1))
        print('   %2d typ %s g=%s: T%d (%d,%d) d=%5.0f tiefe=%6.0f -> T%d (%d,%d) d=%5.0f tiefe=%6.0f  Weg %5.0f  Abstand %+6.0f  s=%d fl=%s mo=%d' % (
            i, a[0], a[3], ta, a[1], a[2], da, tiefe_a, tb, b[1], b[2], db, tiefe_b, weg, db - da, b[4], b[5], b[6]))
