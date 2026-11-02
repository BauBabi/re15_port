# Abnahme G2-3, Punkt 7/8: eigene Auswertung (gator_trag.log + gator_boss.log + state.log), nicht der Bau-Auswerter.
# Masse: (A) 30-Bilder-Fenster netto < 300 (Mass Abnahme 1/2); (B) 2-s-Fenster: in 60 Bildern kommt die Wurzel nie 300 vom Fensteranfang weg
# (Ausnahme nur: Biss am Spieler laeuft = ph 3/8 und (Wurzel < 3200 oder Kopf < 2400)); (C) Umlauf um die Beckenmitte (Plattform (0,-16200)):
# Summe |d Winkel| / 360 und Netto-Winkel / 360; (D) Bahnlaenge; (E) Episoden >= 1 s mit Ort, Wand, Spieler-Abstand, Phase/Clip, Klemmen.
import re, sys, math, collections
run = sys.argv[1]
rx = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\) takt=\((-?\d+),(-?\d+)\) nach_klemmen=\((-?\d+),(-?\d+)\) spieler=\((-?\d+),(-?\d+)\)")
rows = [tuple(int(v) for v in m.groups()) for m in (rx.match(l) for l in open(run + "/gator_trag.log")) if m]
if not rows: print(run, "keine TRAG-Zeilen"); sys.exit()
kk = []; vorbei = []
for l in open(run + "/gator_boss.log", errors="replace"):
    m = re.search(r"KOPF-KLEMME an.*?F(\d+)", l)
    if m: kk.append(int(m.group(1)))
    m = re.search(r"TRAG VORBEI.*?F(\d+)", l)
    if m: vorbei.append(int(m.group(1)))
def kopf(r):
    th = r[4] * 2 * math.pi / 4096
    return math.hypot(r[5] + 4200 * math.cos(th) - r[11], r[6] - 4200 * math.sin(th) - r[12])
def wurzel(r): return math.hypot(r[5] - r[11], r[6] - r[12])
def ort(x, z):
    if -1700 <= x <= 1850 and -20000 <= z <= -12450: return "Plattform"
    if 1850 <= x <= 7200 and -18100 <= z <= -14500: return "Rampe"
    return "Wasser"
def wand(x, z):
    d = {"W": x + 8900, "O": 7200 - x, "S": z + 27000, "N": -5400 - z}
    k = min(d, key=d.get); return "%s%d" % (k, d[k])
n = len(rows)
# Abschnitte zusammenhaengender Bilder (Trag-Zeit kann unterbrochen sein)
def cont(i, j): return rows[j][0] - rows[i][0] == j - i
# (A)
stillA = [False] * n
for i in range(n - 30):
    if not cont(i, i + 30): continue
    if math.hypot(rows[i + 30][5] - rows[i][5], rows[i + 30][6] - rows[i][6]) < 300:
        for k in range(i, i + 31): stillA[k] = True
# (B)
fenB = 0; fenB_bilder = set(); fenB_list = []
for i in range(n - 60):
    if not cont(i, i + 60): continue
    mx = max(math.hypot(rows[k][5] - rows[i][5], rows[k][6] - rows[i][6]) for k in range(i, i + 61))
    if mx >= 300: continue
    biss = any(rows[k][1] in (3, 8) and (wurzel(rows[k]) < 3200 or kopf(rows[k]) < 2400) for k in range(i, i + 61))
    if biss: continue
    fenB += 1; fenB_list.append(rows[i][0]); fenB_bilder.update(range(rows[i][0], rows[i][0] + 61))
# (C)/(D)
cx, cz = 0, -16200
tot_abs = 0.0; tot_net = 0.0; weg = 0.0
for i in range(1, n):
    if rows[i][0] != rows[i - 1][0] + 1: continue
    a0 = math.atan2(rows[i - 1][6] - cz, rows[i - 1][5] - cx); a1 = math.atan2(rows[i][6] - cz, rows[i][5] - cx)
    d = (a1 - a0 + math.pi) % (2 * math.pi) - math.pi
    tot_abs += abs(d); tot_net += d
    weg += math.hypot(rows[i][5] - rows[i - 1][5], rows[i][6] - rows[i - 1][6])
# Episoden (A)
eps = []; i = 0
while i < n:
    if stillA[i]:
        j = i
        while j + 1 < n and stillA[j + 1] and rows[j + 1][0] == rows[j][0] + 1: j += 1
        eps.append((i, j)); i = j + 1
    else: i += 1
anteil = sum(stillA) / n
amSp = sum(1 for k in range(n) if stillA[k] and kopf(rows[k]) <= 2500)
dash = sum(1 for k in range(n) if stillA[k] and rows[k][2] in (2, 3) and kopf(rows[k]) > 2500)
fern = sum(stillA) - amSp - dash
lang = max(((j - i + 1) for i, j in eps), default=0)
lang_fern = 0
for i, j in eps:
    seg = rows[i:j + 1]
    if sum(1 for r in seg if kopf(r) > 2500) > len(seg) / 2: lang_fern = max(lang_fern, len(seg))
ortc = collections.Counter(ort(r[11], r[12]) for r in rows)
print("%s: Trag %d Bilder (%.0f s) F%d..F%d, Bahn %.0f (%.0f je s), Umlauf |Winkel| %.1f / netto %.1f Runden um (0,-16200)" % (
    run.split('/')[-1], n, n / 30, rows[0][0], rows[-1][0], weg, weg / (n / 30), tot_abs / (2 * math.pi), tot_net / (2 * math.pi)))
print("  Spieler-Ort: %s" % dict(ortc))
print("  (A) 30-B.-Fenster netto<300: %.1f %% (am Spieler %.1f %%, Dash %.1f %%, fern %.1f %%), laengste Folge %d B. (%.1f s), laengste ferne Folge %d B." % (
    100 * anteil, 100 * amSp / n, 100 * dash / n, 100 * fern / n, lang, lang / 30, lang_fern))
print("  (B) 2-s-Fenster ohne Fortschritt (ohne Biss am Spieler): %d Fenster, %d Bilder betroffen; Anfaenge %s" % (fenB, len(fenB_bilder), fenB_list[:12]))
print("  KOPF-KLEMME an: %d, TRAG VORBEI: %d" % (sum(1 for f in kk if rows[0][0] <= f <= rows[-1][0]), sum(1 for f in vorbei if rows[0][0] <= f <= rows[-1][0])))
for i, j in sorted(eps, key=lambda e: -(e[1] - e[0]))[:8]:
    seg = rows[i:j + 1]; m = len(seg)
    gx = sum(r[5] for r in seg) // m; gz = sum(r[6] for r in seg) // m
    k_ = sum(kopf(r) for r in seg) / m
    ph = collections.Counter(r[1] for r in seg).most_common(1)[0][0]; mo = collections.Counter(r[2] for r in seg).most_common(2)
    nkk = sum(1 for f in kk if seg[0][0] <= f <= seg[-1][0])
    print("   Folge F%d-F%d %d B. (%.1f s) Gator (%d,%d) %s, Kopf-Spieler %d, Spieler %s (%d,%d), ph %d, Clips %s, Klemmen %d" % (
        seg[0][0], seg[-1][0], m, m / 30, gx, gz, wand(gx, gz), k_, ort(seg[m // 2][11], seg[m // 2][12]), seg[m // 2][11], seg[m // 2][12], ph, mo, nkk))
