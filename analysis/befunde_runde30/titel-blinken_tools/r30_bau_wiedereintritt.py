#!/usr/bin/env python3
"""r30_bau_wiedereintritt.py - Runde 30 / Thema D, Bau-Agent.
Wiedereintritt in den Titel nach dem Tod: jeder Eintritt laedt im Original TITLE.BIN neu
(FUN_80029a28(0) @0x80021318 / @0x8001d208 -> jal 0x80013b60 @0x80029a64), der Puls beginnt
also wieder bei 0x80 / 0 und traegt im ersten Bild 0x82 (Schritt vor dem Zeichnen,
@0x80102ba0).
Liest die Messschiene RE15_TITLE_PULSE_LOG eines Laufs mit mehreren Titel-Eintritten
(z.B. RE15_TITLE_SHOT + RE15_KILL_AT + RE15_BOOT_EXIT_AT) und zeigt je Eintritt das erste
und das letzte Bild. Ein Eintritt beginnt, wo zwischen zwei Titel-Bildern mehr als
<luecke_ms> liegen.
Aufruf: r30_bau_wiedereintritt.py <log> [luecke_ms=500]
Rueckgabe 0 = jeder Eintritt beginnt mit (Zaehler 1, Wert 0x82, Einblende-Tick 0, B 255).
"""
import sys
rows = [tuple(int(x) for x in l.split()[:7]) for l in open(sys.argv[1]) if len(l.split()) >= 7]
gap = float(sys.argv[2]) if len(sys.argv) > 2 else 500.0
seg = [[rows[0]]]
for a, b in zip(rows, rows[1:]):
    if (b[0] - a[0]) / 1000.0 > gap: seg.append([])
    seg[-1].append(b)
fail = 0
print("Titel-Eintritte: %d" % len(seg))
for k, s in enumerate(seg):
    f, l = s[0], s[-1]
    ok = (f[1], f[2], f[5], f[6]) == (1, 0x82, 0, 255)
    print("  Eintritt %d: %4d Bilder, %.2f s; erstes Bild (Zaehler %d, Wert 0x%02x, Tick %d, B %d), "
          "letztes Bild (Zaehler %d, Wert 0x%02x)   %s" % (
              k, len(s), (l[0] - f[0]) / 1e6, f[1], f[2], f[5], f[6], l[1], l[2],
              "beginnt neu" if ok else "BEGINNT NICHT NEU"))
    if not ok: fail = 1
if len(seg) < 2:
    print("nur ein Eintritt im Log - Wiedereintritt NICHT gemessen"); fail = 1
print("ERGEBNIS: %s" % ("ABWEICHUNG" if fail else "jeder Eintritt beginnt bei 0x80 / 0"))
sys.exit(fail)
