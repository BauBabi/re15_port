#!/usr/bin/env python3
"""r30_pulse_log_stats.py - Runde 30 / Thema D.

Wertet die Logdatei der instrumentierten Kopie (R30_PULSE_LOG) aus:
Zeile = "<zeit_us> <ctr> <val>", eine Zeile je Aufruf von re15_render_pc_title_menu.
Ausgabe: Aufrufrate (Hz), Periodendauer des Pulses in ms (Abstand der Ruecksetzungen
ctr==0), Verteilung der Aufrufabstaende.
"""
import sys, statistics
def main():
    rows = [tuple(int(x) for x in l.split()) for l in open(sys.argv[1]) if l.strip()]
    skip = int(sys.argv[2]) if len(sys.argv) > 2 else 0
    rows = rows[skip:]
    t = [r[0] for r in rows]
    dt = [(b - a) / 1000.0 for a, b in zip(t, t[1:])]
    print("Aufrufe               : %d  (erste %d uebersprungen)" % (len(rows), skip))
    print("Messdauer             : %.3f s" % ((t[-1] - t[0]) / 1e6))
    print("Aufrufabstand  Median : %.3f ms   Mittel %.3f ms   min %.3f   max %.3f" % (
        statistics.median(dt), statistics.mean(dt), min(dt), max(dt)))
    print("Aufrufrate (Mittel)   : %.2f Hz" % (1000.0 / statistics.mean(dt)))
    resets = [r[0] for r in rows if r[1] == 0]
    per = [(b - a) / 1000.0 for a, b in zip(resets, resets[1:])]
    print("Ruecksetzungen ctr==0 : %d" % len(resets))
    if per:
        print("PULSPERIODE           : Median %.1f ms   Mittel %.1f ms   min %.1f   max %.1f   (n=%d)" % (
            statistics.median(per), statistics.mean(per), min(per), max(per), len(per)))
    vals = sorted(set(r[2] for r in rows))
    print("Pulswerte             : min 0x%02x max 0x%02x, %d verschiedene" % (vals[0], vals[-1], len(vals)))
    # Aufrufe je Periode
    idx = [i for i, r in enumerate(rows) if r[1] == 0]
    n = [b - a for a, b in zip(idx, idx[1:])]
    if n: print("Aufrufe je Periode    : %s" % sorted(set(n)))
    # Histogramm der Abstaende
    hist = {}
    for d in dt:
        k = round(d)
        hist[k] = hist.get(k, 0) + 1
    print("Abstands-Histogramm (ms gerundet): " + ", ".join("%d ms x%d" % (k, hist[k]) for k in sorted(hist)))
if __name__ == "__main__":
    main()
