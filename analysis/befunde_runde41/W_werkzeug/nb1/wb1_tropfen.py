"""wb1_tropfen.py - Spur W, Nachbesserung 1: Deckentropfen Effekt 6 Sub 0 in ROOM2000 Cut 2.
Liest aus dem RDT: die acht Sce_espr_on der Schleife (Repo sub07 @0x01B06..), die Kamera RID Cut 2 (@0x000A0),
das Regions-Viereck von Cut 2 (erster RVD-Satz mit rec[2] == 2, wie FUN_80014324; Test wie FUN_80014368 @0x80053334)
und die sprite.pri-Masken von Cut 2. Projiziert jeden Punkt bei y -10000 / -1800 / -900 / 0 (LookAt wie
camera_common.c re15_camera_build_view, H = fov >> 7) und prueft, ob eine Maske den Fallweg deckt.
Optional: Bild mit den Fallwegen (Unterlage = 960x720-Bild der Marke).
Aufruf: python wb1_tropfen.py <ROOM2000.RDT> [<unterlage.png> <ausgabe.png>]"""
import math, struct, sys
d = open(sys.argv[1], "rb").read()
CUT = 2
rid = struct.unpack_from("<I", d, 0x08 + 4 * 7)[0]          # Kopf-Tabelle Eintrag 7 = RID
cam = rid + CUT * 0x20
fl, fov, px, py, pz, tx, ty, tz, pri = struct.unpack_from("<HHiiiiiiI", d, cam)
H = fov >> 7
dx, dy, dz = tx - px, ty - py, tz - pz
dist = math.isqrt(dx * dx + dy * dy + dz * dz); hz = math.isqrt(dx * dx + dz * dz)
sp = int(-dy * 4096 / dist); cp = int(hz * 4096 / dist); sy = int(dx * 4096 / hz); cy = int(dz * 4096 / hz)
R = [[cy, 0, -sy], [(sp * sy) >> 12, cp, (sp * cy) >> 12], [(cp * sy) >> 12, -sp, (cp * cy) >> 12]]
def proj(x, y, z):
    v = [x - px, y - py, z - pz]; c = [sum(R[r][k] * v[k] for k in range(3)) / 4096 for r in range(3)]
    return None if c[2] <= 0 else (160 + H * c[0] / c[2], 120 + H * c[1] / c[2], c[2])
print("RID Cut %d @0x%05X fov 0x%04X H %d Lage (%d,%d,%d) Ziel (%d,%d,%d) pri @0x%X" % (CUT, cam, fov, H, px, py, pz, tx, ty, tz, pri))
print("Gegenprobe Leon (-10552,-1800,-26043) ->", tuple(round(v) for v in proj(-10552, -1800, -26043)[:2]), "(befund.log: 84,108)")
def lo32(a, b):
    v = (a * b) & 0xffffffff
    return v - (1 << 32) if v & 0x80000000 else v
def innen(px_, pz_, xs, zs):                                 # FUN_80014368 Befehl fuer Befehl (aot_common.c)
    x0, z0, x1, z1, x2, z2, x3, z3 = xs[0], zs[0], xs[1], zs[1], xs[2], zs[2], xs[3], zs[3]
    t1 = pz_ - z0; t2 = x1 - x0; a2 = px_ - x0; a3 = z1 - z0; t3 = x3 - x0; t4 = z3 - z0
    if lo32(a3, a2) < lo32(t2, t1): return 0
    if lo32(t3, t1) < lo32(t4, a2): return 0
    ddz = z2 - z0; ddx = x2 - x0
    t1 -= ddz; t2 -= ddx; a2 -= ddx; a3 -= ddz; t3 -= ddx; t4 -= ddz
    if lo32(t2, t1) < lo32(a3, a2): return 0
    if lo32(t4, a2) < lo32(t3, t1): return 0
    return 1
rvd = struct.unpack_from("<I", d, 0x28)[0]; p = rvd
while d[p + 2] != CUT: p += 20
v = struct.unpack_from("<8h", d, p + 4); xs, zs = v[0::2], v[1::2]
print("Region Cut %d = RVD-Satz @0x%05X %s  x %s  z %s" % (CUT, p, d[p:p + 4].hex(" "), xs, zs))
gc, mc = struct.unpack_from("<HH", d, pri); gs = [struct.unpack_from("<HHhh", d, pri + 4 + 8 * i) for i in range(gc)]
mp = pri + 4 + 8 * gc; masken = []
for n, _, gx, gy in gs:
    for _ in range(n):
        sx_, sy_, mx, my, dep, _u, sz = struct.unpack_from("<BBBBHBB", d, mp); mp += 8
        if (sz & 0xf0) == 0: w, h = struct.unpack_from("<HH", d, mp); mp += 4
        else: w = h = (sz >> 4) * 8
        masken.append((mx + gx, my + gy, w, h, dep))
print("Masken Cut %d: %d, x %d..%d" % (CUT, len(masken), min(m[0] for m in masken), max(m[0] + m[2] - 1 for m in masken)))
# Schleife: alle Sce_espr_on Effekt 6 Sub 0 Skala 0x1000 hinter dem Evt_exec-Ziel (Bytesuche, Laenge 16)
punkte = []; i = 0
while True:
    i = d.find(b"\x3a\x00\x06\x00\x00\x00\x00\x10", i)
    if i < 0 or i > 0x4000: break
    x, y, z = struct.unpack_from("<hhh", d, i + 8); punkte.append((i, x, y, z)); i += 16
bilder = []
for k, (o, x, y0, z) in enumerate(punkte):
    ein = innen(x, z, xs, zs)
    lagen = [proj(x, y, z) for y in (y0, -1800, -900, 0)]
    txt = " -> ".join("-" if l is None else "(%d,%d)" % (round(l[0]), round(l[1])) for l in lagen)
    weg = [proj(x, y, z) for y in range(y0, 1, 100)]
    gedeckt = any(m[0] <= l[0] < m[0] + m[2] and m[1] <= l[1] < m[1] + m[3] and m[4] < (int(l[2]) >> 6)
                  for l in weg if l for m in masken)
    print("Punkt %d @0x%05X (%d,%d,%d) Region %s  y %d/-1800/-900/0: %s  Maske deckt: %s" % (
        k, o, x, y0, z, "innen" if ein else "aussen", y0, txt, "ja" if gedeckt else "nein"))
    if ein and all(lagen): bilder.append((k, weg, lagen))
if len(sys.argv) >= 4:
    from PIL import Image, ImageDraw
    im = Image.open(sys.argv[2]).convert("RGB"); S = im.size[0] / 320; dr = ImageDraw.Draw(im)
    farbe = {3: (0, 230, 230), 5: (255, 230, 0), 7: (255, 40, 40)}
    for k, weg, lagen in bilder:
        c = farbe.get(k, (255, 255, 255))
        dr.line([(l[0] * S, l[1] * S) for l in weg], fill=c, width=2)
        for l in lagen[1:]:
            dr.ellipse([l[0] * S - 5, l[1] * S - 5, l[0] * S + 5, l[1] * S + 5], outline=c, width=2)
        l = lagen[-1]; dr.text((l[0] * S + 8, l[1] * S - 6), "Punkt %d" % k, fill=c)
    dr.text((6, 6), "Effekt 6 Sub 0 (Repo sub07), Fallweg y -10000 bis 0, Ringe y -1800/-900/0; Regions-Test Cut 2 innen: 3, 5, 7",
            fill=(255, 255, 255))
    im.save(sys.argv[3])
