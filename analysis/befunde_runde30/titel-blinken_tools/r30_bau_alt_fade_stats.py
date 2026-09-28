#!/usr/bin/env python3
"""r30_bau_alt_fade_stats.py - Runde 30 / Thema D, Bau-Agent.
ALTER STAND (instrumentierte Kopie, eine Zeile "<zeit_us> <zaehler> <wert>" je Aufruf des
Zeichners): trennt Titel-Schleife und Bestaetigungs-Fade am Bildabstand (der Fade hat eine
Untergrenze von 16 ms je Bild, main.c) und zaehlt die Aenderungen des Pulswerts im Fade.
Aufruf: r30_bau_alt_fade_stats.py <log> [grenze_ms=12]
"""
import sys, statistics
rows = [tuple(int(x) for x in l.split()[:3]) for l in open(sys.argv[1]) if l.strip()]
lim = float(sys.argv[2]) if len(sys.argv) > 2 else 12.0
dt = [(b[0] - a[0]) / 1000.0 for a, b in zip(rows, rows[1:])]
# erster zusammenhaengender Lauf von >= 20 Bildern mit Abstand > lim = der Fade
start = None
for i in range(len(dt) - 20):
    if all(d > lim for d in dt[i:i + 20]):
        start = i + 1; break
if start is None:
    print("kein Fade im Log gefunden"); sys.exit(2)
end = start
while end < len(dt) and dt[end] > lim: end += 1
fade = rows[start:end + 1]
title = rows[:start]
tdt = dt[:start - 1]
print("Titel-Schleife : %d Aufrufe, Abstand Median %.2f ms" % (len(title), statistics.median(tdt)))
print("Fade-Schleife  : %d Aufrufe, Abstand Median %.2f ms, Dauer %.2f s" % (
    len(fade), statistics.median(dt[start:end]), (fade[-1][0] - fade[0][0]) / 1e6))
chg = sum(1 for a, b in zip([rows[start - 1]] + fade, fade) if a[2] != b[2])
print("Pulswert aendert sich WAEHREND des Fades: %d mal (Original: 0)" % chg)
resets = [r[0] for r in fade if r[1] == 0]
per = [(b - a) / 1000.0 for a, b in zip(resets, resets[1:])]
if per:
    print("Pulsperiode waehrend des Fades: Median %.1f ms (n=%d)" % (statistics.median(per), len(per)))
