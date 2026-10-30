# Runde 41 Spur G, Nachbesserung 1 (M2): Nachrechnung des Spur-B-Aufnahmefensters ueber die Gator-Lagen eines state.log.
# Offline-Replay des B-Aufnahmefensters (gator_re2_r41.c gr2_kan_takt Stufe 0: laengs in [8601, 9000) x Skala 2731/4096,
# |quer| < hit_radius_min + 267; gr2_kan_vor: laengs = (c*dx - s*dz)>>12, quer = (s*dx + c*dz)>>12) ueber die Gator-Lagen
# (Typ 0x23) eines state.log: fuer jede Kandidaten-Liegestelle das erste Bild mit Fenstertreffer.
import sys, re, math
LOG = sys.argv[1]; F0 = int(sys.argv[2]) if len(sys.argv) > 2 else 0
S = 2731/4096.0; AB = 8601*S; LEN = 399*S; QMAX = 1466 + 267
pat = re.compile(r'\[(\d+) t=23 st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=(\w+) mo=(\d+) af=(\d+) stun=(\d+) d=(\d+) @\((-?\d+),(-?\d+),r(\d+)\)\]')
fr = []
seg = 0; last = -1
for line in open(LOG, errors='replace'):
    m0 = re.match(r'F(\d+) ', line)
    if not m0: continue
    f = int(m0.group(1))
    if f < last: seg += 1
    last = f
    m = pat.search(line)
    if not m: continue
    x, z, r = int(m.group(11)), int(m.group(12)), int(m.group(13)) & 4095
    mo = int(m.group(7)); st = int(m.group(2))
    pm = re.search(r'PL\((-?\d+),(-?\d+),rot=(\d+)', line)
    fr.append((seg, f, x, z, r, mo, st, int(pm.group(1)) if pm else 0, int(pm.group(2)) if pm else 0))
print('Bilder mit Gator:', len(fr), 'Segmente:', sorted(set(a[0] for a in fr)))
segs = sorted(set(a[0] for a in fr))
want_seg = segs[-1] if len(sys.argv) <= 3 else int(sys.argv[3])
fr = [a for a in fr if a[0] == want_seg and a[1] >= F0]
print('Segment', want_seg, 'Bilder', len(fr), 'von F', fr[0][1] if fr else -1, 'bis', fr[-1][1] if fr else -1)
xs = [a[2] for a in fr]; zs = [a[3] for a in fr]
if fr: print('Gator x %d..%d z %d..%d' % (min(xs), max(xs), min(zs), max(zs)))
def treffer(kx, kz):
    out = []
    for (sg, f, x, z, r, mo, st, px, pz) in fr:
        a = r * 2 * math.pi / 4096.0
        c, s = math.cos(a), math.sin(a)
        dx, dz = kx - x, kz - z
        l = c*dx - s*dz; q = s*dx + c*dz
        if AB <= l < AB + LEN and -QMAX < q < QMAX:
            out.append((f, x, z, r, mo))
    return out
cands = [(-2278,-17267,'Bau-Landepunkt'), (-5300,-16300,'RDT-Gator-Sce_em_set'), (-4300,-16300,'West-Route x'), (-3500,-16300,'Plattform-Marge')]
for kx in range(-8000, -1500, 500):
    for kz in range(-24000, -9000, 1000):
        cands.append((kx, kz, ''))
best = []
for (kx, kz, nm) in cands:
    t = treffer(kx, kz)
    if nm: print('%-22s (%6d,%6d): %4d Treffer-Bilder, erstes %s' % (nm, kx, kz, len(t), t[0] if t else '-'))
    best.append((len(t), kx, kz, t[0][0] if t else None))
best.sort(reverse=True)
print('Raster bestes:', best[:12])
