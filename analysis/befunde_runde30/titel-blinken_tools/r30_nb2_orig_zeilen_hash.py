#!/usr/bin/env python3
"""r30_nb2_orig_zeilen_hash.py - Runde 30 / Thema D, zweite Nachbesserung.

Liefert die SOLL-Werte fuer den Bild-Teil von integration_r30_titel_puls: je Bildpuffer der
Original-Savestates im Titelmenue die aktive Zeile NEW GAME als
  FNV-1a-32 ueber die Bytes (r5, g5, b5) je Pixel, Zeilen y0 .. y0+16, Spalten 0x20 .. 0x11f
  (256 x 17 = 4352 Pixel: das Rechteck bei y und der subtraktive Schatten bei y+1,
   Zeichner FUN_801027a0: +0x10000 auf das xy-Wort @0x80102810-14)
  und die Summe r5+g5+b5 ueber dieselben Pixel.
Dieselbe Rechnung macht render_pc.c (Messschiene RE15_TITLE_PULSE_LOG) ueber die
zurueckgelesenen Pixel des Ports (Kanal >> 3).

Der Pulswert, den ein Puffer zeigt, wird wie in r30_title_row_sim.py bestimmt: der
Zeichner-Nachbau (Modulation min(31, texel*wert >> 7), subtraktiv dann additiv) wird fuer
alle 32 Pulswerte gerechnet; gilt fuer genau einen Wert "0 von 4352 Pixeln weichen ab"
(bzw. fuer die Gruppe 0x80/0x82/0x84, die dasselbe Bild ergibt), ist der Puffer diesem Wert
zugeordnet. Die Hash-Werte kommen aus dem ORIGINAL-VRAM, nicht aus dem Nachbau.

Nur lesend. Ausgabe: Tabelle auf stdout.
"""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
import re15_ss
from r30_title_row_sim import load_bg, sim_row, split, ROWY

ROOT = r"C:\workspace\git\reAi_v2\stage_saves"
TITLE = open(r"C:\workspace\git\reAi_v2\info\Re1.5\PSX\BIN\TITLE.BIN", "rb").read()

def title_loaded(ram):
    a0, a1 = 0x801027a0, 0x80102944
    return ram.bytes(a0, a1 - a0) == TITLE[a0 - 0x80100000:a1 - 0x80100000]

def patched(ram):
    return ram.bytes(0x80026e4c, 4) == bytes([0x24, 0xc2, 0x01, 0x08])

def fnv_region(get5, y0):
    h = 2166136261
    s = 0
    for y in range(y0, y0 + 17):
        for x in range(0x20, 0x120):
            for c in get5(x, y):
                h ^= c
                h = (h * 16777619) & 0xffffffff
                s += c
    return h, s

def main():
    bg = load_bg()
    names = sys.argv[1:] or ["boot_40.sav", "boot_44.sav", "boot_48.sav", "boot_52.sav",
                             "mzd_title.sav", "nav_down1.sav"]
    vals = [0x80 + 2 * k for k in range(32)]
    print("%-14s %-6s %-6s %-6s %-8s %-7s %-10s %-8s %s" % (
        "savestate", "patch", "cursor", "Puffer", "RAM-val", "Bild=", "fnv1a", "summe", "Deckung"))
    rows = []
    for n in names:
        r = re15_ss.Ram(os.path.join(ROOT, n))
        if not title_loaded(r):
            print("%-14s TITLE.BIN nicht geladen" % n); continue
        cur = r.u8(0x801026ca)
        val = r.u16(0x80102944)
        y0 = ROWY[cur]
        for oy in (0, 240):
            get5 = lambda x, y, oy=oy: split(r.vpix(x, oy + y))
            h, s = fnv_region(get5, y0)
            hits = []
            for v in vals:
                sim = sim_row(r, bg, cur, True, v, "blend")
                bad = sum(1 for (x, y), c in sim.items() if split(r.vpix(x, oy + y)) != c)
                if bad == 0: hits.append(v)
            deck = ",".join("0x%02x" % v for v in hits) if hits else "KEINE"
            print("%-14s %-6s %-6d %-6d 0x%02x    %-7s 0x%08x %-8d 0 Abw. fuer %s" % (
                n, "JA" if patched(r) else "nein", cur, oy, val,
                ("0x%02x" % hits[-1]) if len(hits) == 1 else "?", h, s, deck))
            rows.append((n, cur, oy, hits, h, s))
    print()
    print("Tabelle fuer test_r30_titel_puls.cmake (nur eindeutige Zuordnung, Zeile NEW GAME):")
    seen = {}
    for n, cur, oy, hits, h, s in rows:
        if cur != 0 or len(hits) != 1: continue
        v = hits[0]
        if v in seen and seen[v][0] != h:
            print("  WIDERSPRUCH 0x%02x: %s 0x%08x gegen %s 0x%08x" % (v, n, h, seen[v][1], seen[v][0]))
        seen.setdefault(v, (h, s, "%s/y%d" % (n, oy)))
    for v in sorted(seen):
        h, s, src = seen[v]
        print("  0x%02x 0x%08x %d   # %s" % (v, h, s, src))

if __name__ == "__main__":
    main()
