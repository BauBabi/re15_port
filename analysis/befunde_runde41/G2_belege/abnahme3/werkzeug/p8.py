# Abnahme G2-3, Punkt 8: nach dem Aufschlag des Kanisters (Rueckkehr-Besuch 2090) bis zur Aufnahme:
# je 1 s Gator-Wurzel, Abstand zum Spieler / zum Kanister, Annaeherungsrate an beide (je s), Gier-Abweichung zur Richtung Spieler/Kanister.
import re, sys, math
run = sys.argv[1]
dbg = open(run + "/debug.log", errors="replace").read()
m = re.search(r"F(\d+) ROOM209\d Kanister im Becken \((-?\d+),(-?\d+),(-?\d+)\)", dbg); f_land, KX, KZ = int(m[1]), int(m[2]), int(m[4])
gb = open(run + "/gator_boss.log", errors="replace").read()
m = re.search(r"KANISTER angehaengt.*?F(\d+)", gb); f_auf = int(m[1]) if m else None
m = re.search(r"F(\d+) RUNDE zum Kanister an", dbg); f_runde = int(m[1]) if m else None
anschw = len(re.findall(r"ANSCHWIMMEN an", dbg))
L = open(run + "/state.log", errors="replace").read().splitlines()
st = [i for i, l in enumerate(L) if l.startswith("F1 ")]
# Abschnitt: der Besuch, in dem F_land..F_auf liegt = erster 2090-Besuch nach der Szene (3. Abschnitt)
idx = next(k for k in range(1, len(st)) if L[st[k-1]].split()[2].startswith('PL(-9950,-25700'))
sec = L[st[idx]:st[idx + 1]] if idx + 1 < len(st) else L[st[idx]:]
rp = re.compile(r"^F(\d+) pad=\S+ PL\((-?\d+),(-?\d+),.*?\[35 t=23 st=\d+[^\]]*?mo=(\d+) af=\d+[^\]]*@\((-?\d+),(-?\d+),r(-?\d+)\)")
R = {}
for l in sec:
    g = rp.match(l)
    if g: R[int(g[1])] = tuple(int(v) for v in g.groups()[1:])
ende = f_auf if f_auf else max(R)
print("%s: Aufschlag F%d Kanister (%d,%d), RUNDE-zum-Kanister F%s, Aufnahme F%s (%.1f s nach Aufschlag), ANSCHWIMMEN an: %d" % (
    run.split('/')[-1], f_land, KX, KZ, f_runde, f_auf, (f_auf - f_land) / 30 if f_auf else -1, anschw))
def ang(dx, dz): return math.atan2(-dz, dx)
sum_k = sum_p = 0; n = 0
for f in range(f_land, ende + 1, 30):
    if f not in R or f + 30 not in R: continue
    px, pz, mo, gx, gz, r = R[f]; px2, pz2, _, gx2, gz2, _ = R[f + 30]
    dk0 = math.hypot(gx - KX, gz - KZ); dk1 = math.hypot(gx2 - KX, gz2 - KZ)
    dp0 = math.hypot(gx - px, gz - pz); dp1 = math.hypot(gx2 - px2, gz2 - pz2)
    th = r * 2 * math.pi / 4096
    ak = math.degrees((ang(KX - gx, KZ - gz) - th + math.pi) % (2 * math.pi) - math.pi)
    ap = math.degrees((ang(px - gx, pz - gz) - th + math.pi) % (2 * math.pi) - math.pi)
    print("  F%4d Gator (%6d,%6d) Clip %d | Spieler %5d (%+5d/s, Blick-Abw %+4.0f) | Kanister %5d (%+5d/s, Blick-Abw %+4.0f)" % (f, gx, gz, mo, dp0, dp1 - dp0, ap, dk0, dk1 - dk0, ak))
