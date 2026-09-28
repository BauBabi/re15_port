#!/usr/bin/env python3
"""r30_yavg_period.py - Runde 30 / Thema D.
Liest die ffmpeg-Ausgabe (metadata=print, YAVG je Bild mit pts_time) einer Zeilen-Region
und bestimmt die Periodendauer des Helligkeitspulses: Zeitpunkte der ABFALLENDEN
Spruenge (Ruecksetzen 0x86 -> 0x80 ist kein Sprung nach unten, das Maximum 0xBE liegt in
der Mitte) -> hier ueber die lokalen MAXIMA und ueber die Autokorrelation.
"""
import sys, re, statistics
def main():
    txt = open(sys.argv[1]).read()
    ts = [float(x) for x in re.findall(r"pts_time:([0-9.]+)", txt)]
    ys = [float(x) for x in re.findall(r"YAVG=([0-9.]+)", txt)]
    n = min(len(ts), len(ys)); ts, ys = ts[:n], ys[:n]
    print("Bilder: %d   Dauer %.3f s   YAVG min %.3f max %.3f  Spanne %.3f" % (
        n, ts[-1] - ts[0], min(ys), max(ys), max(ys) - min(ys)))
    if max(ys) - min(ys) < 0.01:
        print("KEIN Puls in dieser Region (konstant)."); return
    mid = (max(ys) + min(ys)) / 2
    # steigende Durchgaenge durch die Mitte
    cross = []
    for i in range(1, n):
        if ys[i - 1] < mid <= ys[i]:
            f = (mid - ys[i - 1]) / (ys[i] - ys[i - 1])
            cross.append(ts[i - 1] + f * (ts[i] - ts[i - 1]))
    per = [(b - a) * 1000 for a, b in zip(cross, cross[1:])]
    print("steigende Mitteldurchgaenge: %d" % len(cross))
    if per:
        print("PERIODE aus Durchgaengen: Median %.1f ms  Mittel %.1f ms  min %.1f  max %.1f" % (
            statistics.median(per), statistics.mean(per), min(per), max(per)))
if __name__ == "__main__":
    main()
