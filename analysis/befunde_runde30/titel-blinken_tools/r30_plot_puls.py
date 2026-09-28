#!/usr/bin/env python3
"""r30_plot_puls.py - Runde 30 / Thema D.  Zeichnet Pulswert ueber Zeit: Original gegen Port.
Original: Folge aus FUN_801028ec, 1 Schritt je 2 VBlanks (gemessen: 33.435 ms je Stufe,
          build/r30_titel-blinken/orig_capture_pairs.txt)
Port:     gemessene Zeitstempel der instrumentierten Kopie (R30_PULSE_LOG)
Aufruf: r30_plot_puls.py <port_pulse_log.txt> <ausgabe.png>
"""
import sys
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

SURFACE = "#fcfcfb"; INK = "#0b0b0b"; INK2 = "#52514e"; GRID = "#e4e3df"
C_ORIG = "#2a78d6"; C_PORT = "#eb6834"
TICK_MS = 33.435
SPAN_S = 4.2

def orig_series():
    t = []; v = []; val = 0x80; ctr = 0; n = 0
    t.append(0.0); v.append(val)
    while n * TICK_MS / 1000.0 < SPAN_S:
        val = val + 2 if ctr < 0x1f else val - 2
        ctr += 1
        if ctr == 0x3c: ctr = 0; val = 0x80
        n += 1
        t.append(n * TICK_MS / 1000.0); v.append(val)
    return t, v

def port_series(path):
    rows = [tuple(int(x) for x in l.split()) for l in open(path) if l.strip()]
    rows = rows[120:]
    # auf eine Ruecksetzung (ctr==0) ausrichten, damit beide Kurven bei 0x80 beginnen
    i0 = next(i for i, r in enumerate(rows) if r[1] == 0)
    rows = rows[i0:]
    t0 = rows[0][0]
    t = [(r[0] - t0) / 1e6 for r in rows]; v = [r[2] for r in rows]
    keep = [i for i, x in enumerate(t) if x <= SPAN_S]
    return [t[i] for i in keep], [v[i] for i in keep]

def main():
    to, vo = orig_series(); tp, vp = port_series(sys.argv[1])
    fig, ax = plt.subplots(figsize=(10, 4.2), dpi=130)
    fig.patch.set_facecolor(SURFACE); ax.set_facecolor(SURFACE)
    ax.step(tp, vp, where="post", color=C_PORT, lw=2, label="Port v0.8.15 (gemessen, 144-Hz-Anzeige): Periode 416 ms")
    ax.step(to, vo, where="post", color=C_ORIG, lw=2, label="Original (1 Schritt je 2 VBlanks): Periode 2006 ms")
    ax.set_xlim(0, SPAN_S); ax.set_ylim(0x7c, 0xc4)
    ax.set_yticks([0x80, 0x90, 0xa0, 0xb0, 0xbe])
    ax.set_yticklabels(["0x80", "0x90", "0xA0", "0xB0", "0xBE"], color=INK2)
    ax.set_xlabel("Zeit seit Ruecksetzen des Pulszaehlers (s)", color=INK2)
    ax.set_ylabel("Pulswert (Farbbyte der aktiven Zeile)", color=INK2)
    ax.tick_params(colors=INK2, length=0)
    ax.grid(True, color=GRID, lw=0.8); ax.set_axisbelow(True)
    for s in ax.spines.values(): s.set_visible(False)
    ax.set_title("Titelmenue: Helligkeitspuls der gewaehlten Zeile", color=INK, loc="left", fontsize=12, pad=26)
    leg = ax.legend(loc="lower left", bbox_to_anchor=(0.0, 1.0), ncol=2, frameon=False, fontsize=9, handlelength=1.6,
                    borderaxespad=0.2)
    for tx in leg.get_texts(): tx.set_color(INK)
    fig.tight_layout()
    fig.savefig(sys.argv[2], facecolor=SURFACE)
    print("geschrieben:", sys.argv[2])

if __name__ == "__main__":
    main()
