"""haeng3.py <laufordner> [von_F] [bis_F]: Haenger-Mass eines exe-Laufs (Messblatt NB3, Vorgabe B) aus gator_trag.log (TRAG je Bild: Phase, Clip, Bild, Gier, Wurzel,
Figur), state.log (cam=, hp=), climb.log (Figur y), gator_boss.log (TICK alle 30 Takte: rt= zw= ziel=, Zuordnung ueber pos=(x,z)).
 - Haenge-Fenster wie gator_rw_r41.c re15_gr2_rw_nachtakt (b): 150 Bilder in Phase 2 (CHASE) ohne Biss/Treffer, Wurzel-Kasten < 2500 x 2500, Kopf (3236 vor der Wurzel,
   Clip 0 NB5 M2a; Mass wie mass_v4.py) > 4500 von der Figur, netto < 600 in den letzten 60 Bildern -> Fensterende F, Kasten, Figur, Phase, TICK-Kontext.
 - laengste Zeit ohne Fortschritt zur Figur (Mass des Riegels re2 / mass_v4.py).
 - Treffer (hp < 100 in state.log) mit Phase."""
import re, sys, math
run = sys.argv[1]; f0 = int(sys.argv[2]) if len(sys.argv) > 2 else 0; f1 = int(sys.argv[3]) if len(sys.argv) > 3 else 1 << 30
rx = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\) takt=\((-?\d+),(-?\d+)\).*spieler=\((-?\d+),(-?\d+)\)")
rows = [tuple(int(v) for v in m.groups()) for m in (rx.search(l) for l in open(run + "/gator_trag.log", errors="replace")) if m]
rows = [r for r in rows if f0 <= r[0] <= f1]
cam = {}; hp = {}; gst = {}; ply = {}
# state.log und climb.log haben je Spielbild eine Zeile (gleiche Zeilenzahl); der LETZTE F-Abschnitt (F faellt zurueck) = Rueckkehr-Besuch 2090/2091
sl = [l for l in open(run + "/state.log", errors="replace") if l.startswith("F")]
cl = [l for l in open(run + "/climb.log", errors="replace") if l.startswith("T")] if __import__("os").path.exists(run + "/climb.log") else []
start = 0
for i, l in enumerate(sl):
    m = re.match(r"F(\d+) ", l)
    if m and int(m.group(1)) == 0: start = i
for i in range(start, len(sl)):
    l = sl[i]
    m = re.match(r"F(\d+) .*PL\((-?\d+),(-?\d+),rot=(\d+),hp=(-?\d+)\).*cam=(\d+)", l)
    if not m: continue
    F = int(m.group(1))
    cam[F] = int(m.group(6)); hp[F] = int(m.group(5))
    g = re.search(r"@\((-?\d+),(-?\d+),r(\d+)\)", l)
    if g: gst[F] = (int(g.group(1)), int(g.group(2)), int(g.group(3)))
    if i < len(cl):
        c = re.search(r"pos=\((-?\d+),(-?\d+),(-?\d+)\)", cl[i])
        if c: ply[F] = (int(c.group(1)), int(c.group(2)), int(c.group(3)))
# T (climb.log, global) -> F (state.log, Rueckkehr): Anker = erstes Bild der Rueckkehr: F0 der TRAG/ state.log ~ erste T-Zeile room=2090/2091 nach einer 2080/2081-Zeile
ticks = []
for l in open(run + "/gator_boss.log", errors="replace"):
    m = re.match(r"TICK ph=(\d+) pos=\((-?\d+),(-?\d+)\) y=(-?\d+) dist=(\d+) mo=(\d+) cd=(\d+) rt=(-?\d+) zw=(-?\d+) ziel=\((-?\d+),(-?\d+)\)", l)
    if m: ticks.append(tuple(int(v) for v in m.groups()))
tick_bei = {}
for t in ticks: tick_bei.setdefault((t[1], t[2]), []).append(t)
def kopf(r):
    a = r[4] * 2 * math.pi / 4096; return (r[5] + math.cos(a) * 3236, r[6] - math.sin(a) * 3236)
n = len(rows); kd = [math.hypot(kopf(r)[0] - r[9], kopf(r)[1] - r[10]) for r in rows]
# Treffer
treffer = [F for F in sorted(hp) if hp[F] < 100 and (F - 1 not in hp or hp[F - 1] >= 100)]
ph_von = {r[0]: r[1] for r in rows}
print("%s: %d TRAG-Bilder F%d..F%d; Treffer (hp<100) %d: %s" % (run.replace("\\", "/").split("/")[-1], n, rows[0][0] if rows else -1, rows[-1][0] if rows else -1, len(treffer),
      ", ".join("F%d ph%s Figur(%d,%s,%d)" % (F, ph_von.get(F, "?"), ply.get(F, (0, "?", 0))[0], ply.get(F, (0, "?", 0))[1], ply.get(F, (0, "?", 0))[2]) for F in treffer[:24])))
oben = sum(1 for F in ply if ply[F][1] < -900 and ((1850 < ply[F][0] < 7200 and -18100 < ply[F][2] < -14500) or (-1700 < ply[F][0] < 1850 and -20000 < ply[F][2] < -12450)))
print("  Figur oben (y < -900 auf Rampe/Plattform): %d Bilder; Kameras: %s" % (oben, ", ".join("%d:%d" % (c, sum(1 for F in cam if cam[F] == c)) for c in sorted(set(cam.values())))))
# ohne Fortschritt (mass_v4.py)
best = None; lauf = 0; ohne_max = 0; ohne_bis = -1; p0 = None
for i, r in enumerate(rows):
    px, pz = r[9], r[10]
    if p0 is None or kd[i] <= 4500 or math.hypot(px - p0[0], pz - p0[1]) > 1500 or kd[i] < best - 300: lauf = 0; best = kd[i]; p0 = (px, pz)
    else:
        lauf += 1
        if lauf > ohne_max: ohne_max = lauf; ohne_bis = r[0]
print("  laengste Zeit ohne Fortschritt zur Figur: %d Bilder = %.1f s (bis F%d)" % (ohne_max, ohne_max / 30.0, ohne_bis))
# Haenge-Fenster (RW-Mass)
seit_biss = 1 << 20; hist = []; fenster = []
for i, r in enumerate(rows):
    F, ph = r[0], r[1]
    if ph in (3, 8) or hp.get(F, 100) < 100: seit_biss = 0
    else: seit_biss += 1
    if ph != 2: hist = []; continue
    hist.append((r[5], r[6], F, i))
    if len(hist) >= 150 and seit_biss >= 150:
        w = hist[-150:]
        xs = [h[0] for h in w]; zs = [h[1] for h in w]
        nx, nz = r[5] - w[-60][0], r[6] - w[-60][1]
        if max(xs) - min(xs) < 2500 and max(zs) - min(zs) < 2500 and kd[i] > 4500 and math.hypot(nx, nz) < 600:
            fenster.append((F, w[0][2], min(xs), max(xs), min(zs), max(zs), r, kd[i]))
            hist = []
print("  Haenge-Fenster (RW-Mass, 150 Bilder Kasten < 2500, Kopf > 4500, netto < 600/60 B.): %d" % len(fenster))
for (F, Fa, x0, x1, z0, z1, r, k) in fenster:
    y = ply.get(F, ("?", "?", "?"))[1]
    print("    F%d (ab F%d): Wurzel (%d..%d, %d..%d) jetzt (%d,%d) Gier %d Clip %d Bild %d | Figur (%d,%s,%d) cam %s hp %s | Kopf %d von der Figur" % (F, Fa, x0, x1, z0, z1, r[5], r[6], r[4], r[2], r[3], r[9], y, r[10], cam.get(F, "?"), hp.get(F, "?"), k))
    # TICK-Kontext: alle TICKs, deren pos in diesem Fenster vorkommt (Zuordnung ueber die Wurzel-Lage)
    pos_im_fenster = set((rr[5], rr[6]) for rr in rows if Fa <= rr[0] <= F)
    gesehen = []
    for p in pos_im_fenster:
        for t in tick_bei.get(p, []): gesehen.append(t)
    gesehen = sorted(set(gesehen), key=lambda t: (t[1], t[2]))
    for t in gesehen[:12]:
        print("      TICK ph=%d pos=(%d,%d) dist=%d mo=%d cd=%d rt=%d zw=%d ziel=(%d,%d)" % (t[0], t[1], t[2], t[4], t[5], t[6], t[7], t[8], t[9], t[10]))
# Phasen-Anteile und Figur oben
ph_n = {}
for r in rows: ph_n[r[1]] = ph_n.get(r[1], 0) + 1
print("  Phasen (Bilder): %s" % ", ".join("%d:%d" % (k, v) for k, v in sorted(ph_n.items())))
