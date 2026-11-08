"""G2-RE2 NB2 (Vorschlag 4): Mass der Fensterlaeufe (exe). mass_v4.py <lauf> [<lauf_ohne>]
Eingaben je Laufordner: gator_trag.log (TRAG je Bild: mit dem Kanister bzw. nach dem Messhaken RE15_G2_OHNE_KANISTER), state.log (cam= je Bild),
gator_boss.log (RAMMEN, G2RE2 RUECKWAERTS/TAUCHEN), debug.log ([gator-re2] Kanister-Sitz ... Abweichung max, Kanister AUSSERHALB des Regions-Vierecks).
Masse wie G2_belege/re2/werkzeug/mass_exe.py (Abnahme 4), dazu Vorschlag 4:
 - Kopf-Riegel nur mit dem Kopfpunkt (3236 vor der Wurzel, Clip 0, NB5 M2a), Kopf + Kanister (4630 vor, 237 rechts .. 2694 links) nur dokumentiert;
 - Drehen "ganz fern": 120-Bilder-Fenster mit >= 360 Grad Gier, Wurzel-Kasten < 3000 und Kopf IM GANZEN FENSTER > 4500 von der Figur (Riegel re2);
 - Querungen = Eintritte in Phase 7; Halterung = groesste "Kanister-Sitz"-Abweichung (Lage + Drehung, gator_re2_pc.c kanister_sitz) ueber den Lauf;
 - mit <lauf_ohne>: Bild fuer Bild (F) Lage, Gier, Phase, Clip, Bild gegen den Lauf ohne Kanister (gleiche Eingabe)."""
import re, sys, math
run = sys.argv[1]
ohne = sys.argv[2] if len(sys.argv) > 2 else None
CUTS = {0: ((2754, -3132, -26190), (-7992, -18, -21564)), 1: ((-7740, -3132, -25956), (5724, -18, -21654)), 2: ((2484, -3132, -25956), (5724, -18, -7380)),
        3: ((2484, -3132, -6516), (5724, -18, -25398)), 4: ((-7740, -3132, -6462), (5724, -18, -12492)), 5: ((2754, -3132, -6408), (-7992, -18, -10782)),
        6: ((-7740, -3132, -6462), (-4464, -18, -25308)), 7: ((-7740, -3132, -25830), (-4410, -18, -7452))}
def fw(c):
    p, t = CUTS[c]; d = [t[i] - p[i] for i in range(3)]; l = math.sqrt(sum(v * v for v in d)); return p, [v / l for v in d]
FW = {c: fw(c) for c in CUTS}
rx = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\) takt=\((-?\d+),(-?\d+)\).*spieler=\((-?\d+),(-?\d+)\)")
def lesen(r):
    return [tuple(int(v) for v in m.groups()) for m in (rx.search(l) for l in open(r + "/gator_trag.log", errors="replace")) if m]
rows = lesen(run)
cam = {}
for l in open(run + "/state.log", errors="replace"):
    m = re.match(r"F(\d+) .*cam=(\d+)", l)
    if m: cam[int(m.group(1))] = int(m.group(2))
PLAT = (-1700, 1850, -20000, -12450); RAMP = (1850, 7200, -18100, -14500)
def drin(x, z, r): return r[0] < x < r[1] and r[2] < z < r[3]
n = len(rows)
best = None; lauf = 0; ohne_max = 0; p0 = None
dreh = set(); dreh_f = set(); rueck = 0; blk_k = 0; blk_kan = 0; k2500 = 0; k0 = 0; kk2500 = 0; quer = 0; kds = []
for i, (F, ph, mo, af, g, x, z, tx, tz, px, pz) in enumerate(rows):
    a = g * 2 * math.pi / 4096; c, s = math.cos(a), math.sin(a)
    def welt(l0, l1): return (x + c * l0 + s * l1, z - s * l0 + c * l1)
    kx, kz = welt(3236, 0)
    kd = math.hypot(kx - px, kz - pz); kds.append(kd)
    if p0 is None or kd <= 4500 or math.hypot(px - p0[0], pz - p0[1]) > 1500 or kd < best - 300: lauf = 0; best = kd; p0 = (px, pz)
    else: lauf += 1; ohne_max = max(ohne_max, lauf)
    if i >= 120:
        w = rows[i - 120:i + 1]
        tot = sum(abs(((w[k + 1][4] - w[k][4] + 2048) % 4096) - 2048) for k in range(len(w) - 1))
        if tot >= 4096:
            xs = [r[5] for r in w]; zs = [r[6] for r in w]
            if max(xs) - min(xs) < 3000 and max(zs) - min(zs) < 3000:
                if kd > 4500: dreh.update(range(i - 120, i + 1))
                if min(kds[i - 120:i + 1]) > 4500: dreh_f.update(range(i - 120, i + 1))
    if mo == 10 and ph == 2: rueck += 1
    if ph == 7 and (i == 0 or rows[i - 1][1] != 7): quer += 1
    kopf = [welt(3236, 0)]
    kan = [welt(4630, q) for q in (-237, 600, 1300, 2000, 2694)]
    if ph != 7 and any(drin(px_, pz_, r) for (px_, pz_) in kopf for r in (PLAT, RAMP)): blk_k += 1
    if ph != 7 and any(drin(px_, pz_, r) for (px_, pz_) in kopf + kan for r in (PLAT, RAMP)): blk_kan += 1
    cc = cam.get(F)
    if cc in FW:
        cp, f = FW[cc]
        def vz(pts): return min(min((p[0] - cp[0]) * f[0] + (y - cp[1]) * f[1] + (p[1] - cp[2]) * f[2] for y in (-1200, -2993)) for p in pts)
        v = vz(kopf)
        if v < 2500: k2500 += 1
        if v <= 0: k0 += 1
        if vz(kopf + kan) < 2500: kk2500 += 1
boss = open(run + "/gator_boss.log", errors="replace").read()
rammen = boss.count("RAMMEN F"); rw = boss.count("G2RE2 RUECKWAERTS an"); tauch = boss.count("G2RE2 TAUCHEN an"); haengt = boss.count("G2RE2 HAENGT")
dbg = open(run + "/debug.log", errors="replace").read()
sitz = re.findall(r"Kanister-Sitz F(\d+): angehaengt (\d+) Bilder, Abweichung max (\d+)", dbg)
aus = len(re.findall(r"Kanister F\d+ AUSSERHALB", dbg))
mi = n / 1800.0
pc = lambda v: 100.0 * v / max(n, 1)
print("%s: %d Bilder (%.1f min) | ohne Fortschritt zur Figur %.1f s | Drehen %.1f %% (ganz fern %.1f %%) | rueckwaerts %.1f %% (%d), Tauch %d, Haenger %d | "
      "Kopf in Plattform/Rampe %.1f %% (+Kanister %.1f %%) | Kopf < 2500 vor der Kamera %.1f %%, hinter %.1f %% (+Kanister < 2500 %.1f %%) | Rammen %d, Querungen %d | "
      "Kanister-Sitz %s | Kanister ausserhalb Regions-Viereck %d"
      % (run.replace("\\", "/").split("/")[-1], n, mi, ohne_max / 30.0, pc(len(dreh)), pc(len(dreh_f)), pc(rueck), rw, tauch, haengt, pc(blk_k), pc(blk_kan),
         pc(k2500), pc(k0), pc(kk2500), rammen, quer,
         ("%s Bilder, Abweichung max %s (letzte Zeile F%s)" % (sitz[-1][1], max(int(s_[2]) for s_ in sitz), sitz[-1][0])) if sitz else "keine Zeile", aus))
if ohne:
    ro = {r[0]: r for r in lesen(ohne)}
    rm = {r[0]: r for r in rows}
    gem = sorted(set(ro) & set(rm))
    d = [F for F in gem if rm[F][1:7] != ro[F][1:7]]
    erst = d[0] if d else None
    ph_e = (rm[erst][1], ro[erst][1]) if erst else None
    print("  gegen %s: %d gemeinsame Bilder (mit %d, ohne %d), abweichend %d, erstes F%s (Phase mit/ohne %s)%s"
          % (ohne.replace("\\", "/").split("/")[-1], len(gem), len(rm), len(ro), len(d), erst, ph_e,
             "" if not d else ", davon vor der ersten Querung mit Kanister: %d" % len([F for F in d if F < min([r[0] for r in rows if r[1] == 7] or [1 << 30])])))
