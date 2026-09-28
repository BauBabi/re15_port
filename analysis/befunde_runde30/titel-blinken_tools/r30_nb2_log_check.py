#!/usr/bin/env python3
"""r30_nb2_log_check.py - Runde 30 / Thema D, zweite Nachbesserung.

Schnellauswertung eines Protokolls der Messschiene RE15_TITLE_PULSE_LOG (12 Spalten, s. main.c
pc_title_pulse_log): je Pulswert der Engine die gezeichneten Werte und die Zeilen-Hashes, dazu
der Vergleich mit den Original-Bildpuffern (r30_nb2_orig_zeilen_hash.py) und die Perioden aus
den BEOBACHTETEN Bildwechseln. Der Test selbst ist tests/integration/test_r30_titel_puls.cmake;
dieses Werkzeug dient nur der Diagnose.

Aufruf: python r30_nb2_log_check.py <puls.txt>
"""
import sys
from collections import defaultdict

ORIG = {  # Pulswert -> (FNV-1a, Summe) der aktiven Zeile NEW GAME im Original-Bildpuffer
    0x88: (0x452e4901, 66958), 0x8a: (0xc2713d44, 67059), 0x9c: (0x74c7e050, 69027),
    0x9e: (0x5018e0cb, 69432), 0xa2: (0xb7245ff0, 69713), 0xa4: (0x0b2c163f, 70168),
    0xac: (0x1a2d63c7, 70902), 0xae: (0x0ee16d0d, 70980), 0xba: (0xcb4d35aa, 72397),
    0xbc: (0x7841f327, 72586), 0xbe: (0x409b0bdb, 72666),
}
SOLL_P = 2005817

def main():
    rows = []
    for ln in open(sys.argv[1]):
        f = ln.split()
        if len(f) != 12: continue
        t, ctr, val, due, ph, tick, b, shown, drawn, row = map(int, f[:10])
        rows.append(dict(t=t, ctr=ctr, val=val, due=due, ph=ph, tick=tick, b=b, shown=shown,
                         drawn=drawn, row=row, h=int(f[10], 16), s=int(f[11])))
    print("Zeilen: %d (Titel %d, Fade %d, nicht gezeigt %d)" % (
        len(rows), sum(r["ph"] == 0 for r in rows), sum(r["ph"] == 1 for r in rows),
        sum(r["shown"] == 0 for r in rows)))
    by_val = defaultdict(set); drawn_bad = 0
    for r in rows:
        if not r["shown"]: continue
        by_val[r["val"]].add((r["h"], r["s"]))
        if r["drawn"] != r["val"]: drawn_bad += 1
    print("gezeichnet != Engine: %d" % drawn_bad)
    multi = {v: hs for v, hs in by_val.items() if len(hs) > 1}
    print("Pulswerte mit mehr als einem Zeilenbild: %d" % len(multi))
    for v in sorted(by_val):
        hs = sorted(by_val[v])
        o = ORIG.get(v)
        tag = ""
        if o: tag = "  Original 0x%08x -> %s" % (o[0], "GLEICH" if hs == [o] else "ABWEICHEND")
        print("  0x%02x: %s%s" % (v, ", ".join("0x%08x/%d" % x for x in hs), tag))
    # Perioden aus beobachteten Bildwechseln (nur Titel, gezeigt)
    tit = [r for r in rows if r["ph"] == 0 and r["shown"]]
    smin = min(r["s"] for r in tit)
    ev = []
    for a, c in zip(tit, tit[1:]):
        if c["s"] == smin and a["s"] != smin:
            ev.append((c["t"], c["t"] - a["t"]))
    print("Abfall auf das Minimum (Summe %d): %d Ereignisse" % (smin, len(ev)))
    for (t0, d0), (t1, d1) in zip(ev, ev[1:]):
        p = t1 - t0
        print("  Periode %d us  Abw %d  Toleranz %d" % (p, p - SOLL_P, max(d0, d1) + 1))

if __name__ == "__main__":
    main()
