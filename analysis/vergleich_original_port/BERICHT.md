# Vergleich Original ↔ Port, Stage für Stage (2026-10-30)

Auftrag: „Vergleiche mir Stück für Stück noch mal Original mit unserem Port … fehlt noch etwas?“ — linear, Stage 1 → 6.

## Methode (was gemessen wurde und was nicht)

| Raster | Werkzeug | Ergebnis |
|---|---|---|
| Gegner-Registrierung je Stage | `dispatch_scan.py` (Schreibzugriffe der Overlays auf die Typ-Tabelle @0x80072bac) | Typ → Wurzel je Stage, z. B. STAGE1 @0x8011e87c–e96c |
| Overlay-Funktionen je Stage | `vergleich.py` (jal-Ziele, Prolog-Zeiger, `jalr`-Tabellen, echte Decompile-Starts) | S1 378 · S2 341 · S3 311 · S4 266 · S5 317 · S6 41 |
| Zwillinge über Stages | Byte-Signatur (Overlay-Ziele maskiert, EXE-Ziele exakt) | 54 eindeutige Klassen ohne Start-Zitat im Port |
| Port-Zitat | Startadresse (oder die eines byte-gleichen Zwillings) steht in `re15_port/{engine,platform,include,tools}` | streng; „nicht zitiert“ = Prüfauftrag, kein Urteil |
| Erreichbarkeit | `wer_setzt.py` (wer schreibt +0x4/+0x5 = n), `xref.py`, `kontext.py` | toter Code wird als solcher markiert |
| SCD-Opcodes | alle 240 RDTs gegen `s_op_table` | vollständig (s. u.) |
| Gegnertypen je Raum | alle `Sce_em_set` gegen die Typ-Weiche `re15_enemy_ai_run_all` | jeder gespawnte Typ hat einen Handler |
| Laufprobe | `tests/unit/probe_raum_sweep.c`: jeder Raum 600 Takte `re15_game_step` | 206/206 echte Räume ohne Absturz/Hänger |

Nicht gemessen: Bildvergleich gegen das Original je Raum (dafür bräuchte es DuckStation-Läufe), Byte-Treue
innerhalb portierter Funktionen. „Zitiert“ heißt: jemand hat dort disassembliert und es im Port belegt.

## Ergebnis in einem Satz

Die Spiel-Logik ist bis auf **eine echte, sichtbare Lücke (brennende Zombies)** und vier kleinere,
selbst als OFFEN markierte Teilmechanismen portiert; alles Übrige ohne Start-Zitat ist entweder über
Tabellen-Index portiert, ein Spielerhaken mit Innen-Zitaten oder im Auslieferungsstand unerreichbar.

## Lücken (nach Gewicht)

### 1. Brennende Zombies — FEHLT (Stage 5, alle Stages tragen den Code)
- **Original:** Ist Flag Zone 1 Index 31 gesetzt (`DAT_800aca3c & 1`, Test @0x801008f4–904), bewaffnet
  der Zombie-INIT sich mit `+0x1d8 |= 0x100` (@0x80100920–28), Zeitzähler `+0x1da = 600 + rand + rand`
  (@0x8010094c–7c) und hängt Effekt 8 an Körperteile aus @0x8011f7a4 (`FUN_80019700` @0x801009c0 /
  @0x80100a14 …, `a0 = 0x08031800 / 0x08032000 / 0x08031000` @0x80100954–58, @0x801009c8–cc, @0x80100a1c–20;
  Effekt-Id 8 ist in den Raumbänken der Flammen-Slot, vgl. unit_r36_granate).
  Bei `+0x1da == 300` werden 8 Körperteile (Tabelle @0x8011f7a4) mit Routine 0x16 zu Treffer-Hitboxen
  (`FUN_80019d50(8,3,0x16,…)` → @0x80071d98 = 0x80017eb0 → 0x17/0x18, Hitbox FUN_80017fa4).
  Bei 0: `+0x4 = 0x1503` → DEATH[21][0] = **0x80108d74** (Verbrennen: 8× erneut Hitboxen, SE 8/5,
  Kill-Flag 0x8004ef90, Leiche). Zwillinge: S2 0x80108c08, S3 0x80108e60, S4 0x80108d28, S5 0x80108ea8.
- **Wer setzt das Flag:** ROOM5120/5121 und ROOM5140/5141 (`Set 1,31,1` jeweils ganz vorn im Skript);
  dort stehen Typ-0x18-Zombies.
- **Port:** Flag wird nur als „combat latch“ gelöscht (scd_room_setup.c); der Arm-Zweig ist „DEFERRED
  (dormant)“ (enemy_ai_common.c `re15_enemy_ai_live_active`), DEATH[21] fehlt; im RE2-Gehirn gar nicht.

### 2. Spinne 0x25 klettert/hüpft nicht (Stage 2)
- ACT-Tabelle @0x80118e64[+0x6]: 9 = 0x80112af8, 10 = 0x80112e58, 12 = 0x801130a4 (420 Befehle),
  13 = 0x80113734, 14/15. Port: `default` „CLIMB/HOP/CEILING … OPEN“, Rückfall auf den Fahrer.
- **Erreichbar:** Sub 10 schreibt das DECIDE bei Feststecken ≥ 31 Takte (@0x80111440–70, @0x801111c4–f4).

### 3. Tyrant 0x2b: Wende B[11] nur vereinfacht (Stage 4/5)
- B[11] = **0x80113438** (S5 0x801135b8), 6 Phasen über @0x801002ac, rund 400 Befehle. Port: Fünfzeiler
  mit falscher Startadresse (`@0x80113538`).
- B[12]/A[12] (0x80113b9c/0x80113a78): kein Schreiber für +0x5 = 12 → tot.

### 4. Zombie-Spawn-Modus Grid-Nibble 3 (RE1.5-Gehirn) — deferred
- 2 Records (ROOM3010/3011). Dispatcher @0x8011f80c[3] = 0x80101800; zugehöriges Aufstehen
  +0x5 = 5 (@0x80106a04) → Zeilen 0x80104b38/40. Port: `default` „deferred“. Im RE2-Gehirn (Standard)
  laufen diese Spawns über die RE2-Deskriptoren.

### 5. Partikel-Emitter 0x24 (Stage 2, 20B0/20B1) — Stöße fehlen, nur Atmosphäre
- Gitter 0/1 → 0x8010f130 / 0x8010f3fc und Untertabellen @0x80118d88/@0x80118d90 (fünf Funktionen à
  ~130 Befehle). Port: OFFEN, Drift portiert. Keine Spielwirkung.

### 6. Zombie-Zustand 4 (`Plc_motion` auf einem Zombie) — nicht belegt
- `Plc_motion` @0x80041bb0 setzt +0x4 = 4; Zombie-Wurzel [4] = 0x8010919c (+0x5 → @0x8012018c). Port: No-op.
- Die Skripte mit `Work_set(2,n)` + `Plc_motion` (1021, 1090, 10D0, 2001 …) teilen den Slot mit NPC-
  Spawns anderer Zweige; ob je ein Zombie das Ziel ist, entscheidet der Laufzeitzweig → **messen**, nicht
  annehmen.

## Geprüft und in Ordnung (kein Handlungsbedarf)

| Fund | Begründung |
|---|---|
| Krähe 0x21: 10 Funktionen ohne Start-Zitat (S1) | Bewegungstabelle @0x80121184 Fälle 0–17 im Port (`re15_crow_move`) |
| Typ 0x27 Unterzustände 9–14 (S1, inkl. 0x8011a44c) | einziger Außen-Einstieg @0x8011b1d4 hängt an +0x1e3; +0x1e3 nur `sb zero` @0x801170ac → tot |
| Typ 0x27 0x8011c158 | Spielerhaken 0x8011c118 (Gorilla-Wurf), im Port (affen_11c0.c, enemy_ai_common.c) |
| Alligator 0x23 (8 Funktionen, S2) | kein `Sce_em_set` mit Typ 0x23 in 240 RDTs → im Original unerreichbar; Port: Bosskampf-Design |
| Birkin 0x30 ACT[2]/[12]/[13] (S3/S5) | kein Byte- oder Wort-Store auf +0x5 = 2/12/13 → tot |
| Tyrant 0x80115cc0/0x80115d00 | Spielerhaken (aca59-Varianten), im Port mit Innen-Zitaten (spiderman_r38.c) |
| Tyrant HURT-Spuren 0x80114850 | im Port mit Innen-Zitaten (HURT 0x80114770) |
| Zombie-Mädchen 0x8010c088 (alle Stages) | nur aus Modus-1-Animate 0x8010be50; kein Raum spawnt Modus 1 (Port-Vermerk, Zensus) |
| NPC 0x49 ACTIVE 0x8011df40 (alle Stages) | ruft 0x800509e4/0x8005070c = gemeinsame NPC-Maschine, im Port |
| 0x45 0x8011d468, 0x26 0x801169c4 | Band-Stempel 0x80012aa4 bzw. Netz-Sprite-Präsentation (Port-Vermerk) |
| Zombie 0x801019f8 u. a. | falsche Decompile-Starts (Delay-Slots) — durch die Start-Prüfung aussortiert |
| SCD-Opcodes | 86 Handler; einziger nicht registrierter in Räumen ist `Break_point` 0x1c (1030/1031/1040/1041): Original @0x8003f1c0 = `pc+1`, Port `op_unknown` Größe 1 = gleich |
| Gegnertypen | jeder in `Sce_em_set` vorkommende Typ (0x10–0x4d) hat eine Port-Weiche |
| 34 „Räume“ mit Parse-Fehler | 4-Byte-Platzhalter `00 00 00 00` auch auf der Original-Disc (z. B. ROOM1270) |
| EXE: 54 Funktionen ohne Zitat | alle mit Aufrufern, fast nur Render/Kollision/Bibliothek (0x80026–0x8002b, 0x8005–0x8006), Aufrufer meist zitiert; der PC-Renderer löst das anders |

## Zahlen

`vergleich.py` (streng, Start-Zitat): S1 355/378 · S2 313/341 · S3 299/311 · S4 253/266 · S5 299/317 · S6 40/41.
Alle 54 Restklassen sind oben einsortiert. EXE: 332/386 Funktionen zitiert (`RE15_ADDRESS_COVERAGE.md`, neu erzeugt).

## Werkzeuge (in diesem Ordner)

`dispatch_scan.py` · `vergleich.py` (→ `vergleich.json`) · `kontext.py` · `xref.py` · `wer_setzt.py` ·
`tests/unit/probe_raum_sweep.c` (Treiber: je Raum ein Aufruf).

## Nachtrag — Umsetzung und Korrekturen (2026-10-30, zweiter Durchgang)

| Punkt | Stand |
|---|---|
| 1. Brennende Zombies | **umgesetzt** (brandzombie_r15.c). Korrektur: Takt 300 ist KEIN Biss — FUN_80019d50 schaltet die angehaengten Flammen auf Routine 22 (@0x80017eb0) -> 23 -> 18; die Hitbox-Routine 0x18 wird nie erreicht. Original-Fehler s0 = 0x800aca88 @0x80101290 -> nur Teil 0 flammt bei 300 auf. Effekt 8 liegt in CORE00.ESP (Raumbank 5120: 5/7/9/16). RE2-Gehirn: Gestaltung (Brand-Zeile 10). |
| 2. Spinne 0x25 Klettern | in Arbeit (eigener Zweig); Einstieg ist die Hilfsroutine 0x801118a8 (aus WALK/CHASE/CREEP/Sub13), braucht +0x90/+0x1b4 der Wandklemme. |
| 3. Tyrant B[11] | **umgesetzt** (Wand-/Deckenwechsel, Grid 0/8/0x10/9, Boxen @0x8011a094/@0x8011a0a0, Fusshalt 0x80115bec). **Korrektur:** B[12]/A[12] sind ERREICHBAR (Phase 5 schreibt +0x5 ueber v1 @0x80113a40-54, mein Schreiber-Scan sah nur v0) — Wand-Modus, jetzt portiert; B[13] laeuft rueckwaerts ohne Lenkung. |
| 4. Zombie-Nibble 3 | **umgesetzt** (Kriech-Maschine wie Nibble 1, DECIDE[5] = 0x80104760 ohne Loskriechen, DECIDE[6] = 0x80103980). |
| 5. Emitter 0x24 | **war schon portiert** (Runde 41, wasser20b0_r41.c); veralteter OFFEN-Vermerk entfernt. Die fuenf 130-Befehl-Phasen gehoeren zu Gittern 2..4, die kein RDT spawnt -> tot. |
| 6. Zombie-Zustand 4 | **unerreichbar, belegt**: alle 1359 Plc_motion-Befehle der 240 RDTs haben a (-> +0x5) = 0/1/2 (1264/78/17); die Zombie-Tabelle @0x8012018c[0..3] ist NULL -> ein Zombie-Ziel waere im Original `jalr 0`. |
| Alligator | **gefunden**: CDEMD0.EMS Block 25 (Sektor 1922) ist ein texturiertes Alligator-Modell ohne Eintrag in der Modell-Ladetabelle (@0x80072f38 Typ 0x23 = 0/0). Render: analysis/alligator_re15/. |
