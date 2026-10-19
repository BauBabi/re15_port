"""B1-Lageplan aus Kollision + Tueren: jeder Ort = begehbare Flaeche (probe_r39_karten flut, echter
Kollisionsloeser, r 0) in SEINEM Raumrahmen; je Tuerpaar wird der Nachbar so verschoben, dass seine
Wandflaeche an der Tuer AUF der Wandflaeche des Vorgaengers liegt (Nutzer-Modell: "der naechste Raum
schliesst EXAKT an die Wand an") und die Tuermitten (Trigger) laengs der Wand zusammenfallen.
Drehung: aus den Ankunftsrichtungen (Spawn -> Trigger zeigt im Raum zur Tuer); gegenlaeufig = keine.
Aufruf: b1_layout.py <pgm-ordner> [bild.png]"""
import sys, os, re
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 're15_port', 'tools'))
import gen_map_zones as GZ
D = sys.argv[1]
def lade(n):
    d = open(os.path.join(D, n + '.pgm'), 'rb').read()
    m = re.match(rb'P5\n# x0 (-?\d+) z0 (-?\d+) schritt (\d+)\n(\d+) (\d+)\n255\n', d)
    x0, z0, st, w, h = map(int, m.groups())
    return dict(x0=x0, z0=z0, st=st, a=np.frombuffer(d[m.end():], np.uint8).reshape(h, w) > 0)
def frei(g, x, z):
    i = int(round((x - g['x0']) / g['st'])); k = int(round((z - g['z0']) / g['st']))
    return 0 <= k < g['a'].shape[0] and 0 <= i < g['a'].shape[1] and g['a'][k, i]
def tuer(rid, slot, ziel):
    for d in GZ.read_rdt(rid)[1]:
        if d['slot'] == slot and d['dest'] == ziel: return d
    raise KeyError((hex(rid), slot, hex(ziel)))
def flaeche(g, sx, sz, dx, dz):
    """vom Ankunftspunkt (sx,sz) in Richtung (dx,dz) bis zum Ende der begehbaren Flaeche: Wandkoordinate."""
    x, z = sx, sz
    while frei(g, x + dx * g['st'], z + dz * g['st']): x += dx * g['st']; z += dz * g['st']
    return (x + dx * g['st'] / 2) if dx else (z + dz * g['st'] / 2)
ORTE = {'1180': (0x1180,), '1230': (0x1230,), '1190H': (0x1190,), '1190O': (0x1190,), '11B0': (0x11B0,),
        '11D0': (0x11D0,), '1160': (0x1160,), '10A0': (0x10A0,), '11C0': (0x11C0,)}
# (Ort A, Raum A, Slot A, Ort B, Raum B, Slot B): Tuer A->B und Gegentuer B->A
PAARE = [('1180', 0x1180, 2, '1190H', 0x1190, 4), ('1230', 0x1230, 6, '1190O', 0x1190, 6),
         ('1180', 0x1180, 3, '11B0', 0x11B0, 0), ('1180', 0x1180, 1, '10A0', 0x10A0, 1),
         ('1180', 0x1180, 0, '11D0', 0x11D0, 0), ('1180', 0x1180, 0, '1160', 0x1160, 0),
         ('11B0', 0x11B0, 1, '11C0', 0x11C0, 0)]
G = {n: lade(n) for n in ORTE}
lage = {'1180': (0, 0), '1230': (0, 0)}
erg = []
for (a, ra, sa, b, rb, sb) in PAARE:
    tab = tuer(ra, sa, rb); tba = tuer(rb, sb, ra)
    # Ankunft in B (tab Spawn) liegt in B, Trigger tba in B; Ankunft in A (tba Spawn) in A, Trigger tab in A
    SA = (tba['nx'], tba['nz']); TA = (tab['lx'], tab['lz'])
    SB = (tab['nx'], tab['nz']); TB = (tba['lx'], tba['lz'])
    va = (TA[0] - SA[0], TA[1] - SA[1]); vb = (TB[0] - SB[0], TB[1] - SB[1])
    ax = 0 if abs(va[0]) > abs(va[1]) else 1
    da = (np.sign(va[0]), 0) if ax == 0 else (0, np.sign(va[1]))
    db = (np.sign(vb[0]), 0) if abs(vb[0]) > abs(vb[1]) else (0, np.sign(vb[1]))
    if abs(vb[0]) + abs(vb[1]) < 500:
        # Ankunft liegt IM Trigger (ROOM11C0: Spawn (-25279,17268), Trigger-Mitte (-25100,17250)) - die
        # Richtung kommt dann aus der naechsten Wand des Triggers: Nordwand z 17736 (ROOM11C0.RDT @0x01434)
        db = (-da[0], -da[1])
    gegen = (da[0] == -db[0] and da[1] == -db[1])
    fa = flaeche(G[a], SA[0], SA[1], *da); fb = flaeche(G[b], SB[0], SB[1], *db)
    ta = lage[a]
    if ax == 0: t = (fa - fb + ta[0], TA[1] - TB[1] + ta[1])
    else:       t = (TA[0] - TB[0] + ta[0], fa - fb + ta[1])
    if b not in lage: lage[b] = (int(t[0]), int(t[1]))
    erg.append((a, b, ax, da, db, gegen, fa, fb, t, tab['yaw'], tba['yaw']))
    print(f"{a} slot {sa} <-> {b} slot {sb}: Achse {'xz'[ax]}, Richtung A {da} B {db} {'GEGENLAEUFIG' if gegen else 'GEDREHT?'}, "
          f"Wand A {fa:.0f} Wand B {fb:.0f}, Tuermitte A {TA} B {TB}, Lage {b} = {tuple(int(v) for v in t)}, Yaw A->B {tab['yaw']} B->A {tba['yaw']}")
print('LAGEN', lage)
if len(sys.argv) > 2:
    from PIL import Image
    K = 100
    xs = []; zs = []
    for n, g in G.items():
        if n not in lage: continue
        h, w = g['a'].shape
        xs += [g['x0'] + lage[n][0], g['x0'] + lage[n][0] + w * g['st']]; zs += [g['z0'] + lage[n][1], g['z0'] + lage[n][1] + h * g['st']]
    X0, X1, Z0, Z1 = min(xs), max(xs), min(zs), max(zs)
    W, H = (X1 - X0) // K + 1, (Z1 - Z0) // K + 1
    img = np.zeros((H, W, 3), np.uint8); cnt = np.zeros((H, W), np.int32)
    farben = {'1180': (255, 0, 255), '1230': (255, 0, 255), '1190H': (0, 200, 0), '1190O': (0, 255, 255), '11B0': (255, 255, 0),
              '11D0': (255, 128, 0), '1160': (255, 128, 0), '10A0': (255, 80, 80), '11C0': (128, 128, 255)}
    for n, g in G.items():
        if n not in lage or n in ('1230', '1160'): continue
        k, i = np.nonzero(g['a'])
        X = g['x0'] + i * g['st'] + lage[n][0]; Z = g['z0'] + k * g['st'] + lage[n][1]
        px = (X - X0) // K; py = (Z1 - Z) // K
        img[py, px] = farben[n]; cnt[py, px] += 1
    img[cnt > 1] = (255, 255, 255)
    Image.fromarray(img).save(sys.argv[2]); print('Bild', sys.argv[2], W, H, 'x', X0, X1, 'z', Z0, Z1)
