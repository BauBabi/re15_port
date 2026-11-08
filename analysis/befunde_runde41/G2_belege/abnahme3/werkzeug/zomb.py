# Abnahme G2-3: Zombies je 2080/2081-Besuch aus state.log (eigene Rechnung, nicht der Bau-Auswerter)
import re, sys, math
LX, LZ = -9950, -25700
lines = open(sys.argv[1] + "/state.log", errors="replace").read().splitlines()
secs = []; cur = None
for l in lines:
    if l.startswith("F1 "): cur = []; secs.append(cur)
    if cur is not None: cur.append(l)
rz = re.compile(r"\[(\d+) t=16 st=(\d+)[^\]]*?@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
for si, sec in enumerate(secs):
    Z = {}
    pl = None
    for l in sec:
        m = re.match(r"^F(\d+) pad=\S+ PL\((-?\d+),(-?\d+),", l)
        if not m: continue
        f = int(m[1])
        if pl is None: pl = (int(m[2]), int(m[3]))
        for g in rz.finditer(l):
            s, st, x, z, r, hp = (int(v) for v in g.groups())
            d = Z.setdefault(s, dict(f0=f, p0=(x, z), r0=r, last=(x, z), weg=0.0, tot=None, rt=r, fmove=None, flast=f))
            if d["tot"] is None:
                dw = math.hypot(x - d["last"][0], z - d["last"][1])
                if dw > 0 and d["fmove"] is None: d["fmove"] = f
                d["weg"] += dw; d["last"] = (x, z); d["rt"] = r; d["flast"] = f
                if st == 3 or hp < 0: d["tot"] = f
    if not Z: continue
    print("Abschnitt %d (Spieler-Start %s, %d Bilder): %d Zombies (t=16)" % (si, pl, len(sec), len(Z)))
    for s, d in sorted(Z.items()):
        (x0, z0), (x1, z1) = d["p0"], d["last"]
        a0 = math.hypot(x0 - LX, z0 - LZ); a1 = math.hypot(x1 - LX, z1 - LZ)
        mv = math.degrees(math.atan2(-(z1 - z0), x1 - x0)) if (x1, z1) != (x0, z0) else float('nan')
        zl = math.degrees(math.atan2(-(LZ - z0), LX - x0))
        blick = d["rt"] * 360 / 4096; blick0 = d["r0"] * 360 / 4096
        print("  Slot %2d: Start (%d,%d) Leiter %5d -> Ende (%d,%d) Leiter %5d (naeher %5d), Bahn %5d, Richtung Bewegung %6.1f Grad / zur Leiter %6.1f Grad, Gier Start %d Ende %d, erstes Bewegen F%s, Tod F%s" % (
            s, x0, z0, a0, x1, z1, a1, a0 - a1, d["weg"], mv, zl, d["r0"], d["rt"], d["fmove"], d["tot"]))
