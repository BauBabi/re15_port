"""G2-RE2 Mass der Fensterlaeufe (exe), gleich fuer die Abnahme-4-Laeufe (Stand NB5 bc5fb7c9c) und die neuen Laeufe.
Eingaben je Laufordner: gator_trag.log (TRAG je Bild mit dem Kanister), state.log (cam= je Bild), gator_boss.log (RAMMEN/UEBERQUERUNG/G2RE2), gator_zeit.log
(nur neu: Faden-Zyklen je Boss-Takt, f = Modus Rueckwaerts/Tauchen).
Masse (Kopfpunkt 3236 vor der Wurzel und Kanister-Vorderkante 4630 vor, 237 rechts .. 2694 links = Mass der Abnahme 4 kopfblock.py/kamnah.py):
 - Haengen: 2-s-Fenster (60 Bilder, Wurzel nie 300 vom Fensteranfang, ausser Biss ph 3 mit Wurzel < 3200 von der Figur und Lauern = >= 10 Bilder Takt (0,0) in ph 2);
   laengste Folge solcher Fenster in s; ohne Fortschritt zur Figur (Kopf nicht 300 naeher als sein Bestwert, Kopf > 4500, Figur < 1500 bewegt)
 - Drehen: 120-Bilder-Fenster mit >= 360 Grad Gier und Wurzel-Kasten < 3000, Kopf > 4500 von der Figur
 - rueckwaerts: ph 2 mo 10 (Rueckwaerts setzen), ph 7 mo 10 (Rueckzug nach Vorstoss)
 - Kopf/Kanister in Plattform/Rampe (nicht in der Ueberquerung ph 7: dort liegt er ueber dem Deck), Bildtiefe < 2500 / <= 0 in der Kamera der Figur (Cut aus state.log)
"""
import re, sys, math, statistics
run = sys.argv[1]
CUTS = {0: ((2754, -3132, -26190), (-7992, -18, -21564)), 1: ((-7740, -3132, -25956), (5724, -18, -21654)), 2: ((2484, -3132, -25956), (5724, -18, -7380)),
        3: ((2484, -3132, -6516), (5724, -18, -25398)), 4: ((-7740, -3132, -6462), (5724, -18, -12492)), 5: ((2754, -3132, -6408), (-7992, -18, -10782)),
        6: ((-7740, -3132, -6462), (-4464, -18, -25308)), 7: ((-7740, -3132, -25830), (-4410, -18, -7452))}
def fw(c):
    p, t = CUTS[c]; d = [t[i] - p[i] for i in range(3)]; l = math.sqrt(sum(v * v for v in d)); return p, [v / l for v in d]
FW = {c: fw(c) for c in CUTS}
rx = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\) takt=\((-?\d+),(-?\d+)\).*spieler=\((-?\d+),(-?\d+)\)")
rows = [tuple(int(v) for v in m.groups()) for m in (rx.search(l) for l in open(run + "/gator_trag.log", errors="replace")) if m]
cam = {}
for l in open(run + "/state.log", errors="replace"):
    m = re.match(r"F(\d+) .*cam=(\d+)", l)
    if m: cam[int(m.group(1))] = int(m.group(2))
PLAT = (-1700, 1850, -20000, -12450); RAMP = (1850, 7200, -18100, -14500)
def drin(x, z, r): return r[0] < x < r[1] and r[2] < z < r[3]
n = len(rows)
fen = []; lauer = []
f2 = 0; folge = 0; folge_max = 0
best = None; lauf = 0; ohne_max = 0; p0 = None
dreh = set(); rueck = rueckzug = 0; blk = 0; k2500 = 0; k0 = 0
for i, (F, ph, mo, af, g, x, z, tx, tz, px, pz) in enumerate(rows):
    a = g * 2 * math.pi / 4096; c, s = math.cos(a), math.sin(a)
    def welt(l0, l1): return (x + c * l0 + s * l1, z - s * l0 + c * l1)
    kx, kz = welt(3236, 0)
    kd = math.hypot(kx - px, kz - pz)
    # ohne Fortschritt
    if p0 is None or kd <= 4500 or math.hypot(px - p0[0], pz - p0[1]) > 1500 or kd < best - 300: lauf = 0; best = kd; p0 = (px, pz)
    else: lauf += 1; ohne_max = max(ohne_max, lauf)
    lauer.append(1 if (ph == 2 and tx == 0 and tz == 0 and mo == 0) else 0)
    biss = 1 if (ph == 3 and math.hypot(x - px, z - pz) < 3200) else 0
    fen.append(biss)
    in_fenster = 0
    if i >= 60:
        x0, z0 = rows[i - 60][5], rows[i - 60][6]
        weit = max(math.hypot(r[5] - x0, r[6] - z0) for r in rows[i - 60:i + 1])
        if weit < 300 and not any(fen[i - 60:i + 1]) and sum(lauer[i - 60:i + 1]) < 10: f2 += 1; in_fenster = 1
    folge = folge + 1 if in_fenster else 0; folge_max = max(folge_max, folge)
    # Drehen
    if i >= 120 and kd > 4500:
        w = rows[i - 120:i + 1]
        tot = sum(abs(((w[k + 1][4] - w[k][4] + 2048) % 4096) - 2048) for k in range(len(w) - 1))
        if tot >= 4096:
            xs = [r[5] for r in w]; zs = [r[6] for r in w]
            if max(xs) - min(xs) < 3000 and max(zs) - min(zs) < 3000: dreh.update(range(i - 120, i + 1))
    if mo == 10 and ph == 2: rueck += 1
    if mo == 10 and ph == 7: rueckzug += 1
    pts = [welt(3236, 0)] + [welt(4630, q) for q in (-237, 600, 1300, 2000, 2694)]
    if ph != 7 and any(drin(px_, pz_, r) for (px_, pz_) in pts for r in (PLAT, RAMP)): blk += 1
    cc = cam.get(F)
    if cc in FW:
        cp, f = FW[cc]
        vz = min(min((p[0] - cp[0]) * f[0] + (y - cp[1]) * f[1] + (p[1] - cp[2]) * f[2] for y in (-1200, -2993)) for p in pts)
        if vz < 2500: k2500 += 1
        if vz <= 0: k0 += 1
boss = open(run + "/gator_boss.log", errors="replace").read() if True else ""
rammen = boss.count("RAMMEN F"); quer = boss.count("G2RE2 UEBERQUERUNG")
tauch = boss.count("G2RE2 TAUCHEN an")
zeit = []; zj = None; taucht = 0
try:
    for l in open(run + "/gator_zeit.log", errors="replace"):
        if l.startswith("K zyk_je_us="): zj = float(l.split("=")[1])
        m = re.match(r"Z F(\d+) ph=(-?\d+) f=(\d+) .*takt_zyk=(\d+)", l)
        if m:
            zeit.append(int(m.group(4)))
            if int(m.group(3)) >= 2: taucht += 1
except FileNotFoundError: pass
mi = n / 1800.0
z = ""
if zeit and zj:
    us = sorted(v / zj for v in zeit)
    z = "Takt %.0f/%.0f/%.0f us (Mittel/95 %%/max, %d Takte)" % (statistics.mean(us), us[len(us) * 95 // 100], us[-1], len(us))
print("%s: %d Bilder mit Kanister (%.1f min) | 2-s-Fenster %d, laengste Folge %.1f s | ohne Fortschritt zur Figur %.1f s | Drehen %.1f %% | rueckwaerts %.1f %% (+Rueckzug %.1f %%) | "
      "Tauch %d (%.2f/min) unter %.1f %% | Kopf/Kanister in Plattform/Rampe %.1f %% | < 2500 vor der Kamera %.1f %%, hinter %.1f %% | Rammen %d (Querungen %d) | %s"
      % (run.replace("\\", "/").split("/")[-1], n, mi, f2, (folge_max + 60) / 30.0 if folge_max else 0.0, ohne_max / 30.0, 100.0 * len(dreh) / max(n, 1),
         100.0 * rueck / max(n, 1), 100.0 * rueckzug / max(n, 1), tauch, tauch / mi if mi else 0, 100.0 * taucht / max(n, 1), 100.0 * blk / max(n, 1),
         100.0 * k2500 / max(n, 1), 100.0 * k0 / max(n, 1), rammen, quer, z))
