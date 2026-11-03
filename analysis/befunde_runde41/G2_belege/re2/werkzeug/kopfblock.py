# Abnahme 4: liegt der Kopf / das Kanister-Vorderende der Belagerungsbahn in Plattform oder Rampe (Bloecke GR2_PLAT_*/GR2_RAMP_*)?
# Vorwaerts der Gier g: (cos g, -sin g) (gator_re2_r41.c gr2_runde_punkt_v); Kopf 3236 vor der Wurzel, Kanister-Enden 4589..4666 vor, 237 rechts .. 2694 links
# (Dossier NB5 M2a, re15_gr2_huelle_ausdehnung). "links" = +z im Gator-Rahmen (Gier 0: x vorn, z links) -> Welt: links = (sin g, cos g)?? wird unten aus der Drehung bestimmt.
import re, sys, math
run = sys.argv[1]; f0 = int(sys.argv[2]) if len(sys.argv) > 2 else 0
rx = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\).*spieler=\((-?\d+),(-?\d+)\)")
rows = [tuple(int(v) for v in m.groups()) for m in (rx.search(l) for l in open(run + "/gator_trag.log", errors="replace")) if m]
PLAT = (-1700, 1850, -20000, -12450); RAMP = (1850, 7200, -18100, -14500)
def drin(x, z, r): return r[0] <= x <= r[1] and r[2] <= z <= r[3]
def tief(x, z, r): return min(x - r[0], r[1] - x, z - r[2], r[3] - z)
n = 0; kopf_in = 0; kan_in = 0; maxk = 0; maxc = 0; bsp = []
for (F, ph, mo, af, g, x, z, px, pz) in rows:
    if F < f0: continue
    n += 1
    a = g * 2 * math.pi / 4096
    fx, fz = math.cos(a), -math.sin(a)
    lx, lz = -fz, fx      # links = Vorwaerts um +90 Grad in (x,z) gedreht (Gier-Rahmen x vorn, z links: Welt = Ry(g) * (0,0,1))
    # Welt-Drehung Ry(g) (Port, gr2_tr_kasten: dx = c l0 + s l1, dz = -s l0 + c l1): lokal (vorn, links) = (l0, l1)
    c, s = math.cos(a), math.sin(a)
    def welt(l0, l1): return (x + c * l0 + s * l1, z - s * l0 + c * l1)
    k = welt(3236, 0)
    ks = [welt(4630, q) for q in (-237, 600, 1300, 2000, 2694)]
    ki = any(drin(*k, r) for r in (PLAT, RAMP))
    ci = [max(tief(*p, r) for r in (PLAT, RAMP) if drin(*p, r)) for p in ks if any(drin(*p, r) for r in (PLAT, RAMP))]
    if ki: kopf_in += 1; maxk = max(maxk, max(tief(*k, r) for r in (PLAT, RAMP) if drin(*k, r)))
    if ci:
        kan_in += 1; maxc = max(maxc, max(ci))
        if len(bsp) < 6 or F % 300 == 0: bsp.append((F, x, z, g, round(max(ci))))
print("%s ab F%d: %d Bilder; Kopf (3236 vor) in Plattform/Rampe %d Bilder (tiefste %d); Kanister-Vorderkante in Plattform/Rampe %d Bilder (tiefste %d)" % (run.split('/')[-1], f0, n, kopf_in, maxk, kan_in, maxc))
print("  Beispiele (F, Wurzel x, z, Gier, Tiefe):", bsp[:10])
