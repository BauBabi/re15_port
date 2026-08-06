# ROOM1060-Treppenabstieg — Hardware-Referenz (ORIGINAL-EXE)

Zehn DuckStation-Savestates, vom Nutzer am 2026-08-06 aufgenommen: kompletter Abstieg durch das
vierstöckige Treppenhaus ROOM1060, Spieler jeweils **in Ruhe** am Absatz.

**Quelle:** `Biohazard 1.5 (MZD Mod) Update 25-01-2025` — deren PSX.EXE ist byte-identisch zu
`info/Re1.5/PSX.EXE` (md5 `b55fdaa5b9ec0f84bdd8a6b1107ea0b0`, selbst verifiziert). Alle zehn
tragen an `0x80026e4c` den Auslieferungs-Stub `08 00 e0 03` — also **kein** Save-Mod.

## Messwerte

| Slot | Position (x, y, z) | Band |
|---|---|---|
| 1 | (25178, −14400, 24568) | 8 |
| 2 | (25181, −10800, 19532) | 6 |
| 3 | (21585, −10800, 20332) | 6 |
| 4 | (21454, −7200, 25368) | 4 |
| 5 | (25283, −7200, 24568) | 4 |
| 6 | (25287, −3600, 19532) | 2 |
| 7 | (21588, −3600, 20332) | 2 |
| 8 | (21457, 0, 25368) | 0 |
| 9 | (21457, 0, 25368) | 0 |
| 10 | (21384, −3600, 20332) | 2 |

## Ergebnis

| Treppe | Start z | Ende z | **Δz** | Δy |
|---|---|---|---|---|
| 8→6 (Ost) | 24568 | 19532 | **−5036** | +3600 |
| 6→4 (West) | 20332 | 25368 | **+5036** | +3600 |
| 4→2 (Ost) | 24568 | 19532 | **−5036** | +3600 |
| 2→0 (West) | 20332 | 25368 | **+5036** | +3600 |

**Das Original trägt exakt 5036 Einheiten pro Treppe** — viermal identisch, in beide Richtungen,
bei jeweils 3600 Einheiten Höhenunterschied (= 2 Bänder à 0x708).

Das bestätigt die geometrische Herleitung (Schachtzelle 4100 + 2×(Body-Reach 450 + Skin 18) =
5036) samt der vorhergesagten Start-/Landepunkte auf die Einheit genau.

## Port-Stand: trifft exakt

Gegen diese Referenz gemessen (`re15_port/tests/unit/probe_stair_hwtruth_1060.c`, ctest
`unit_stair_hwtruth_1060`): der Port liefert **−5036, +5036, −5036, +5036 — Differenz 0 bei allen
vier Treppen**. Gegenprobe mit temporär zurückgenommenem Cursor-Fix (`77db4118`): ebenfalls exakt
5036, die Strecke war also schon vorher richtig.

**Damit widerlegt:** die frühere Annahme, der Port laufe ~700–900 Einheiten zu kurz. Die Werte
3761 und 4134/4355 stammten aus Messaufbauten **ohne PL00-Skelett** (dann trägt der FK-Fuß-Lock
`FUN_800390e0` nichts bei und nur die Vorwärts-10 wirkt) bzw. aus umgeschriebenen Agenten-Sonden.

Die zwei weiterhin offenen, byte-true belegten Divergenzen (Fußreferenz **pro Fuß** statt
gemeinsam @0x80039148-58; **unbedingter** Zug @0x80038d50 statt Skip beim Fußwechsel) verursachen
nachweislich **keinen** Streckenfehler. Sie einzubauen würde eine Strecke gefährden, die aktuell
auf die Einheit stimmt — deshalb bleiben sie liegen.
