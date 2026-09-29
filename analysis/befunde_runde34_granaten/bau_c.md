# Runde 34 (Granaten) — Bau Spur C (Plattform)

Stand: 2026-09-29, Zweig `r34g/c-plattform` (auf `r34g/c0-vertrag` 8d8651e4), Arbeitsbaum
`.claude/worktrees/r34g_c`, Bauverzeichnis `re15_port/build_r34_c`, Laufzeit-Ausgaben `build/r34g_c/` (unversioniert).
Auftrag: BAUPLAN §3.3 C1-C4, C7, C8 mit der Orchestrator-Teilung C (Plattform) / D (RE2-FX-Maschine, re2fx_pc.c).
Dateibesitz: `platform/pc/main.c`, `platform/pc/src/*.c` ausser `re2fx_pc.c/.h`, `tests/unit/probe_r34_plattform*.c`,
`tests/unit/probes/r34_plattform.cmake`.

STATUS: IN ARBEIT (fortlaufend geschrieben).

Neue Datei `platform/pc/src/fx_plattform_pc.c/.h`: die fensterlos pruefbaren Teile (Takt, Ton-Weiche, Haken-Bindung,
Licht-Latch, TEX.TIM-Seiten), damit die Unit-Sonde sie ohne SDL linken kann.

---

## C1 — ESP-Takt hinter dem Spielschritt (E10)

### Beleg (selbst disassembliert, `re15_disasm.py dis 0x8001cdd0 60`)
```
8001cdec: jal 0x8003f038
8001cdf4: jal 0x8004f090
8001cdfc: jal 0x8001500c
8001ce04: jal 0x8001a50c        Gegner
8001ce0c: jal 0x80031c44        Spieler inkl. Waffen-FSM (Spawn Muendung/Huelse/Granate)
8001ce14: jal 0x8002bd44
8001ce1c: jal 0x800436a8
8001ce24: jal 0x8004f0b0
8001ce2c: jal 0x80019e20        ESP-Tick
8001ce34: jal 0x8001db28        Item-Modal
8001ce54: jal 0x80039590        (gegatet 0x800aca38 & 0x100000)
8001ce60: lbu v0,21336(v0)      Licht-Latch 0x800b5358 (C3)
```

### Gebaut
| Datei:Zeile | Inhalt |
|---|---|
| `platform/pc/src/fx_plattform_pc.c` `re15_pc_fx_takt_setzen` / `re15_pc_fx_takt` | Freigabe + Takt: `re15_esp_fx_tick(re15_esp_room_bank())`, dann `re2fx_tick()` (RE2 `jal 0x8001d300` @0x80026980 hinter der Gegner-Schleife 0x800267c0-0x80026930) |
| `platform/pc/main.c` Zweig-Kette (vor `re15_discard_frozen`) | `re15_pc_fx_takt_setzen(0)` — eingefrorene Bilder (Discard/Item-Modal/Menue) ticken nicht, wie bisher |
| `platform/pc/main.c` SCD-30-Hz-Zweig (alte Tick-Stelle) | `re15_pc_fx_takt_setzen(1)` statt `re15_esp_fx_tick(...)` |
| `platform/pc/main.c` hinter `re15_game_step(&gctx)` | `re15_pc_fx_takt()` — vor dem Item-Modal-Tick (@0x8001ce2c < @0x8001ce34) |
| `platform/pc/main.c` hinter dem `md1_ok`-Block | Rueckfall `re15_pc_fx_takt()` (Bild ohne Spielschritt; sonst wirkungslos) |

### Messung (echte exe, ROOM1140, Pistole `RE15_GIVE=3:15 RE15_EQUIP=3`, Schuss im Bild 360, `RE15_FX_LOG`)
Vorher-exe = Stand C0 (`re15_pc_vorher.exe`), nachher = C1. Erste FX-Zeilen im Spawnbild F360:

| Platz | vorher F360 | nachher F360 |
|---|---|---|
| Muendung id 2 sub 0 | `frame=0 fl=03` (ungetickt) | `frame=1 fl=93` (Routine 8 lief: Flags := row[0x0e] = 0x93) |
| Rauch id 3 sub 0 | `frame=0 fl=03` | `frame=8 fl=13` |
| Huelse id 4 sub 0 | `frame=0 fl=03` | `frame=0 fl=63` (Routine 38 -> 16, Flags 0x63) |
| Blut id 0 (Treffer) | `frame=0` | `frame=1` |
| Zweitblitz id 2 sub 4 (Kind der Routine 8) | erst F361 | schon F360 `frame=6 fl=13` |

Der Zustand, den das alte Programm erst im Bild 361 zeigte, steht jetzt im Spawnbild 360 — genau die Verschiebung um
ein Bild, die @0x8001ce0c < @0x8001ce2c verlangt.
