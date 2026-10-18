# Runde 39 — Spur E "gegner": UNABHAENGIGE ABNAHME 0 (2026-10-18)

Pruefer: Abnahme-Agent (unabhaengig vom Bau-Agenten). Baum `.claude/worktrees/r39_gegner`, Zweig `r39/gegner`,
HEAD c3823fa4, Merge-Basis c71fa144 (master ist inzwischen weiter: Spur A Nachbesserung — der Diff gegen `master`
zeigt deshalb Fremdaenderungen; gewertet wird `git diff c71fa144`, 17 Dateien, +1616/-3).

Gebaut: `bash re15_port/tools/local_build.sh configure && ... build` im Spurbaum -> `LOCAL-BUILD-OK`, `ninja: no work to
do` (exe 12:44 = Stand 8deb5f38; c3823fa4 aendert nur das Dossier, f0e123d4 nur Dossier/Bilder). Alle Messungen unten an
Messkopien `re15_pc_abn0_p1.exe` / `re15_pc_abn0_p3.exe` neben der exe (Asset-Wurzel = exe-Ordner), Arbeitsordner im
Scratchpad `.../scratchpad/p1_11c0`, `p3a_1010`, `p3b_1220`, `p3c_1221`, `p3d_1011`.

## 0. Kurzurteil

| Punkt | Urteil | Kern des Belegs |
|---|---|---|
| 1 Parking-Lot-Gorillas HP x2, Flinch erst nach 3 Pistolentreffern | **erfuellt** | eigener Exe-Lauf 11C0: hp 360 -> 348/336 ohne Clip 7 -> 324 FLINCH (Clip 7 F440-452) -> 312 ohne; Tabelle 180 @0x8011f514, Pistole 12 @0x8006ee44 selbst gelesen |
| 2 kein paralleler Hundebiss | **erfuellt** | Mechanismus PL+0x1D3&0x80 (@0x801051f8/@0x80105200, Tore @0x80104e5c-68 / @0x80104d84-94, Freigabe @0x8003f8b4) selbst disassembliert; Unit-Riegel vorher -40 / nachher -20; eigener 1190-Lauf 4 Bisse, Riegel je 23 Bilder |
| 3 liegende Zombies kriechen (1010/1011, 1220/1221) | **erfuellt** | Block B @0x80104718/2c-30/38/48 selbst disassembliert; eigene Exe-Laeufe: Leon 1010 ueber die Tuer (Wecker dist=2996, Kriechen, Griff), 1220 (2126/1999); Elza 1221 (CONTINUE) und 1011 ueber die Tuer — bildgleich |
| 4 Gorilla-Schatten beim Sprung am Boden | **erfuellt** (mit Mangel M1 am Beleg des Root-Skip) | `lh a1,442(v0)` @0x80116f30 / `jal 0x8001b064` @0x80116f34 selbst gelesen; eigener Lauf: 24 Sprungbilder quad_y == boden_y, Framedump F1370 Blob auf dem Asphalt |

Gates: Vertrag OK (§5). RE-Gate: **M1** — die zitierte Root-Skip-Stelle @0x80116df4-f8 tut NICHT, was behauptet wird
(Details §4.2). Daher `bestanden = false` bis M1 behoben ist; die vier Nutzer-Punkte selbst messen wie behauptet.

## 1. Punkt 1 — Parking-Lot-Gorillas (ROOM11C0)

### 1.1 RE-Gate (selbst disassembliert, `re15_disasm.py`)
- HP-Saat INIT FUN_80116f50: `jal 0x8001af20` (rng) @0x801170cc, Basis 0x8011f034 @0x801170d4-d8, `andi v0,v0,0xf`
  @0x801170e4, `lbu v1,8(a1)` (Typ) @0x801170e8, `lhu v0,0(v0)` @0x801170fc, **`sh v0,154(a1)` @0x80117104** — stimmt.
  Zeile 0x27 = 0x8011f034 + 0x27*0x20 = 0x8011f514: `read 0x8011f514 16 --w 2` -> **16 x 180** — stimmt.
- Schadenszeile 0x27 @0x8006ee38 (`read 0x8006ee38 22 --w 2 --stride 4`): `[0,7,30,12,12,28,28,50,38,40,40,70,8,50,8,40,40,
  70,400,20,0,100]` -> Waffe 3 (Pistole, `DAT_8006f430 = [3,3,9,10,...]`) = **12 @0x8006ee44** — stimmt. Messer 7, Schrot 38.
- Doppelte HP (Faktor 2) und Schwelle 3 x 12 = 36 sind als **NUTZER-VORGABE** gekennzeichnet (re15_gegner_r39.h) — zulaessig.
  Das Original hat keine Schadens-Schwelle (Resolver `+0x4 = 2` bei jedem Treffer) — vom Bau-Agenten korrekt benannt.

### 1.2 Eigener Exe-Lauf (Messprotokoll)
Befehl (Arbeitsordner `scratchpad/p1_11c0`, Messkopie `re15_pc_abn0_p1.exe`):
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_SET_FLAG=4:0x40 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-6729,-12800,1486 RE15_EQUIP=3
RE15_INPUT_SCRIPT="M1.5,(MA0.1,M0.6)x12" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=290 RE15_STATE_LOG=state.log
RE15_R39_LOG=1 RE15_EXIT_AT=800` -> EXIT 0.
- gegner_r39.log: `affen init slot=2/3 raum=11c0 hp=360 (tabelle 180 @0x8011f514)`; `treffer slot=2 hp=348 akku=12 -> OHNE
  ZUCKEN (zurueck in 1/5/1/0)`, `hp=336 akku=24 -> OHNE ZUCKEN (1/3/1/0)`, `hp=324 akku=36 -> FLINCH (Schwelle 36)`,
  `hp=312 akku=12 -> OHNE ZUCKEN (1/7/1/0)`.
- state.log (eigene Auswertung): Schuesse (Magazin faellt) F335/379/440/462/484/524; Treffer F335 360->348 `st=1/5 mo=18`,
  F379 348->336 `st=1/3 mo=5`, **F440 336->324 `st=2/3 mo=7` = Flinch-Clip 7, Ende F453 -> `st=1/7 mo=20`** (Vergeltungs-
  Sprung), F462 324->312 `st=1/7 mo=20` ohne Clip 7. Genau EIN Clip-7-Start im Lauf (F440). Der Gorilla laeuft zwischen den
  Treffern weiter (Lage -8386 -> -8058 -> -7727), fx-Zaehler steigt beim Treffer (Blut).
- Abzug je Treffer 12 = Pistole (@0x8006ee44). ROOM1141 bleibt 180 (unit_r39_gegner_affen_hp, selbst gefahren, gruen).
- Urteil: **erfuellt**. Wortlaut "beim Parking Lot" = ROOM11C0 (debug_jump_table "PARKING LOT"); die Beschraenkung auf
  11C0 ist Wortlaut-treu (ROOM1141-Gorillas = Speisesaal-Szene, Spur J) — Entscheidungsfrage des Bau-Agenten bleibt an den
  Nutzer.

## 2. Punkt 2 — kein paralleler Hundebiss (RE2-Hundemodul, ROOM1190 / 11C1)

### 2.1 RE-Gate (selbst disassembliert, `re2_disasm.py --bin EMD0G_MOD0.BIN` bzw. RE2 PSX.EXE Leon)
- Boden-Biss-Spielerhaken 0x801051bc: `lbu v1,467(a0)` @0x801051dc, `addiu v0,zero,2 / sb v0,4(a0)` @0x801051e0-e4,
  `addiu v0,a1,2 / sb v0,5(a0)` @0x801051e8-ec, `sb zero,6/7` @0x801051f0-f4, **`ori v1,v1,0x80` @0x801051f8 / `jr ra` /
  `sb v1,467(a0)` @0x80105200** — stimmt.
- Kontakt-Tor: `lbu v0,467(s1)` @0x80104e5c / `andi v0,v0,0x80` @0x80104e64 / `bne v0,zero,0x80104fcc` @0x80104e68 — stimmt.
- Flug-Tor: `lbu v0,467(s1)` @0x80104d84 / `andi 0x80` @0x80104d8c / `bne ...,0x80104dd4` @0x80104d90 / Delay-Slot
  `addiu v0,zero,-4` @0x80104d94 — stimmt.
- Freigabe Spieler (RE2 PSX.EXE): HURT [2]/[3] Phase 2 `sw a0,4(s0)` @0x8003f8b0 / **`sb zero,467(s0)` @0x8003f8b4** /
  `andi 0xed` + `ori 0x2` @0x8003f8b8-c4 — stimmt; Phase 0 `lbu v1,467(s0)` @0x8003f830 / `ori 0x80` @0x8003f83c /
  `sb v1,467(s0)` @0x8003f844 — stimmt; Spieler-Wurzel `lbu/andi 0x7f/beq/addiu -1/sb` @0x8003bff4-c008 — stimmt.
- Port-Zwilling der Freigabe (`re15_r39_hund_riegel_tick`): frei, sobald +0x93 Bit 1 geloescht ist und weder Flinch (cmd 2)
  noch Niederschlag laeuft. Das Loeschen von +0x93 ist VORBESTEHENDER Port-Code (game_step_common.c:1626, zitiert
  @0x80031964 `sb zero,DAT_800acae7`; ghidra-Dump Z. 126888 — die Stelle liegt im Block 0x80031944-0x800319d0, der auch den
  Spieler-Schatten-Record baut, also eher ein Spieler-Reset-/Init-Handler als ein Bild-Prolog; in dieser Abnahme nicht neu
  geprueft, nicht Teil der Spur). Fuer die Spur zaehlt: der Riegel steht genau so lange wie die Spieler-Reaktion (gemessen
  23 Bilder je Biss), das entspricht dem RE2-Mechanismus (Freigabe am HURT-Exit @0x8003f8b4) als PORT-Zwilling — so im
  Kopf von re15_gegner_r39.h (2) benannt. Zulaessig.

### 2.2 Eigene Messungen
- `ctest -R integration_r39_gegner_hunde1190` selbst gefahren: **Passed 42,25 s**. Lauf `r39_gegner_hunde1190_wd/lauf_e973891b`:
  Bisse F685 (100->80), F720 (->60), F753 (->40), F833 (->20); eigene Auswertung der Hunde-Zustaende im Biss-Bild: F685
  Hund 1 `st=1/3/3 d=949`, Hund 2 `2/1 d=6869`, Hund 3 `2/1 d=951`; F720 Hund 2 `3/3`, die anderen `2/1`; F753 Hund 3 `3/3`;
  F833 Hund 3 `3/3` — bei jedem Biss genau EIN Hund im Biss (Sub 3/3). gegner_r39.log: `GESETZT F685 / FREI F708`, `F720 /
  F743`, `F753 / F776`, `F833 / F856` = 23 Bilder je Riegel (22 Flinch-Bilder + Exit-Bild).
- `unit_r39_gegner_hund_riegel` (selbst gefahren, gruen): zwei RE2-Hunde im Flug im selben Bild: vorher hp 100->80->60
  (Doppelbiss = Nutzer-Befund), nachher 100->80->80, PL+0x1D3 = 0x80, Hund 2 +0x21E = 0; nach dem Ende der Reaktion frei,
  fremder Riegel (Zombie-Griff) bleibt.
- Einschraenkung (vom Bau-Agenten offen benannt, von mir bestaetigt): am skriptierten 1190-Weg entsteht der Doppelbiss weder
  vorher noch nachher als Spielszene (die Hunde kommen nacheinander); der Nutzer-Fall ist ueber den Mechanismus (Unit-Riegel)
  belegt, nicht als Szene nachgestellt. Der Mechanismus ist das RE2-Original (Beta -> Retail), also die richtige Form.
- Urteil: **erfuellt**.

## 3. Punkt 3 — liegende Zombies kriechen (RE2-WAIT + RE1.5-Wecker)

### 3.1 RE-Gate (selbst disassembliert, STAGE1.BIN)
- DECIDE[5] FUN_8010466C Block B: `lw v0,464(v0)` (+0x1d0 = dist) @0x80104710, **`sltiu v0,v0,0xbb8` @0x80104718**,
  `beq ...,0x8010474c` @0x8010471c, `jal 0x8001a9cc` (arc_test) @0x8010472c / **`ori a1,zero,0x200` @0x80104730**,
  `bne` @0x80104734, **`ori v0,zero,0x1` @0x80104738**, **`sw v0,4(v1)` @0x80104748** — stimmt (Wort 1 = Kriech-Lokomotion).
  Bau-Agent zitiert "arc 0x200 @0x8010472C-34" — korrekt.
- RE2-Seite (Deskriptor 1 -> 0x201 @0x80100B74, EXEC[2] @0x80103B48 ohne Ausgang, DECIDE[2] @0x80103A70 nur Griff) ist im
  Port-Code enemy_ai_re2_zombie.c:8680ff (Runde 16) schon mit Bytes belegt; der neue Haken sitzt in re2z_crawl zwischen
  DECIDE und EXEC (`+0x5` wird vom EXEC frisch gelesen @0x80101254) — NUTZER-VORGABE "kriechen wie im Original" in
  RE1.5-Form, so gekennzeichnet. Zulaessig.
- Nebenwirkung geprueft (Zensus aller RDTs, grid 0x81): 1010/1011 (0x10), 1220/1221 (0x16), **4050/4051 (0x18), 5060/5061
  (0x18)** — der Wecker wirkt auch dort (im RE1.5-Original ebenso Block B fuer die Zombie-Wurzel). Niedergeschossene Zombies
  (RE2 P8 -> 0x101 im Steh-Root) sind NICHT betroffen (der Haken liegt nur in der Kriecher-Wurzel, Sub 2).

### 3.2 Eigene Messungen (Messkopie `re15_pc_abn0_p3.exe`, Standard-Flavor RE2)
- **ROOM1010 ueber die ECHTE Tuer** (`scratchpad/p3a_1010`): `RE15_DEBUG_JUMP=1020@240 RE15_FIRE_AOT=0@60#1020` (Door_aot
  0 von ROOM1020 -> 1010 Cut 4, debug.log `loaded room1020.rdt` ... `loaded room1010.rdt`), `RE15_INPUT_SCRIPT=R0.25,U2
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=420`, EXIT 720. state.log (eigene Auswertung, Typ 0x10 g=81):
  nach der Tuer beide Kriecher `st=1/2 mo=23` (liegend) bei (950,-1700) d=3510 und (-50,-3800) d=3700, Leon am Tuer-Ziel
  (3650,-3950) r2048. Leon dreht/laeuft; **F435 Kriecher A `st=1/0/1 mo=5` d=2996** (gegner_r39.log `kriecher weckt slot=3
  typ=10 dist=2996 -> Lokomotion`), kriecht (950,-1700) -> (1655,-1898) F453 -> (2303,-1914) F463 = Griff `ss1=1 mo=14/15`,
  Leon hp 100->95 (F464) ->90 (F494) ->85 (F524), F580 Kriecher A `st=7` (Griffausgang). Kriecher B (d=3700, ausserhalb des
  0x200-Kegels) bleibt bis zum Ende liegen (`1/2 mo=23`). = Block-B-Verhalten.
- **ROOM1220 Leon** (`p3b_1220`, Debug-Sprung 1220@240, `RE15_PLAYER_POS=-23500,-7700,2048`): F6 beide 0x16 g=81 `st=1/2
  mo=23` bei (-25398,-6739) d=2126 / (-23500,-9700) d=1999; **F7 beide `st=1/0/1 mo=5`** (`kriecher weckt ... dist=2126` /
  `1999`), Kriecher 2 kriecht (-23500,-9700) -> (-23500,-8986) F24 = Griff, Leon hp 100->95->90->85, F151 `st=7`; danach
  Kriecher 1 Griff F167, hp 85->80->75->70, F294 `st=7`.
- **Elza ROOM1221 / ROOM1011**: siehe §3.3 (zweiter Lauf, weil der erste Versuch durch `RE15_TITLE_SHOT_AF=2` vor dem
  CONTINUE-Vorlauf in NEW GAME lief — Bedienfehler des Pruefers, kein Befund).
- Urteil Leon-Raeume: wie behauptet; die liegenden Zombies kriechen, sobald der Spieler naeher als 3000 im Sichtkegel steht.

### 3.3 Elza-Laeufe (Messkopie `re15_pc_abn0_p3e.exe`, Spielstand per `probe_r36_elza_karte` = Figur 4, CONTINUE)
- **ROOM1221 Elza** (`p3c2_1221`): Karte `re15_card.mcr: Raum 0x1221, Figur 4 (Elza=4), Lage (-23500,-7700)` rot 2048;
  `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_EXIT_AT=400#1221`; debug.log `[pl] Spieler-Familie PL04
  (character=4, Elza-Bit=1)`, `[save] CONTINUE: resumed in room 1221 (hp=100)`, `[elzaems] Typ 0x16 aus EMD/CDEMD1.EMS`.
  state.log: F0 beide 0x16 g=81 `st=1/2 mo=23` (liegend, d=2126 / 1999); **F1 beide `st=1/0/1 mo=5`** (`kriecher weckt
  slot=1 dist=2126`, `slot=2 dist=1999`); Kriecher 2 kriecht (-23500,-9700) -> (-23500,-8986) F18 = Griff `ss1=1`, Elza hp
  100->95 (F29) ->90 ->85, F145 `st=7`; Kriecher 1 Griff F161, hp ->80 ->75 ->70, F288 `st=7`. Bildgleich mit Leons 1220-Lauf.
- **ROOM1011 Elza ueber die ECHTE Tuer** (`p3d2_1011`): Karte 1021 (-2750,-7250) rot 0, `RE15_FIRE_AOT=0@60#1021` (Door_aot 0
  -> 1011 Cut 4), `RE15_INPUT_SCRIPT=R0.25,U2 ... START=300`, `RE15_EXIT_AT=360#1011`; debug.log `CONTINUE: resumed in room
  1021`, `loaded room1011.rdt`. state.log: nach der Tuer beide 0x10 g=81 `st=1/2 mo=23` bei (950,-1700) d=3510 / (-50,-3800)
  d=3700; **F315 Kriecher A `st=1/0/1 mo=5` d=2996** (`kriecher weckt slot=4 typ=10 dist=2996`), kriecht (950,-1700) ->
  (1655,-1898) F333 -> (2303,-1914) F343 = Griff, Elza hp 100->95 (F344). Kriecher B bleibt liegen. Bildgleich mit Leon 1010.
- Urteil Punkt 3: **erfuellt** (alle vier genannten Raeume, beide Figuren, Tuer-Weg fuer 1010/1011). Die Form (RE1.5-Block-B-
  Wecker als zweiter Ausgang des RE2-WAIT) ist als NUTZER-VORGABE gekennzeichnet und mit den Original-Bytes belegt.

## 4. Punkt 4 — Gorilla-Schatten beim Sprung

### 4.1 RE-Gate Hoehe (selbst disassembliert)
- Gorilla-Wurzel FUN_80116db8 Schwanz: `lw v0,-14460(v0)` (g_entity) @0x80116f24, **`lh a1,442(v0)` @0x80116f30** (+0x1ba),
  **`jal 0x8001b064` @0x80116f34**, `addiu a0,a0,176` @0x80116f38 (+0xb0 Record), dann Epilog @0x80116f3c-4c — stimmt.
- FUN_8001b064 (Decompile RE_15_Quellcode_V2 + eigener Disasm): `local_88.t[1] = (long)param_2` = Quad-Hoehe = a1; liest
  rec+8/+10 (`lhu v0,8(s2)` @0x8001b0f8, `lhu v0,10(s2)` @0x8001b108) und die Ecken rec+0xc/+0xe (`lhu v1,12(s2)`
  @0x8001b184, `lhu v0,14(s2)` @0x8001b19c/@0x8001b1f0) — stimmt.
- STAGE3 kleiner Affe FUN_80110b10: `lh a1,442(v0)` @0x80110c78 / `jal 0x8001b064` @0x80110c7c / `addiu a0,a0,176`
  @0x80110c80 — stimmt.

### 4.2 RE-Gate Root-Skip — **MANGEL M1 (zitierte Stelle tut nicht das Behauptete)**
Behauptung (Dossier Befund B, gegner_r39.c Kopf von re15_r39_schatten_aus, Kommentar main.c, Integrationsriegel (d)):
"`andi v0,v0,0x20` @0x80116df4 / `bne v0,zero,0x80116e88` @0x80116df8 -> mit grid-Bit 0x20 wird die GANZE Wurzel
uebersprungen, also auch der Schatten-Schwanz `lh a1,442(v0)` @0x80116f30 / `jal 0x8001b064`."

Selbst disassembliert (STAGE1.BIN, `dis 0x80116db8 100`):
```
80116df4: andi v0,v0,0x20
80116df8: bne  v0,zero,0x80116e88      ; Sprungziel liegt IN der Funktion, VOR dem Schatten-Schwanz
80116e00..80116e84: Gehirn-Dispatch jalr @0x801213c8[+0x4], 0x8002b498, 0x8002aec4 (Push-out), 0x8002b544, 0x8003b0a4
80116e88: lui a1,0x800b / lw a1,g_entity            ; <- hierher springt der "Root-Skip"
80116e94: lbu v0,9(a1) / 80116e9c: andi v0,v0,0x40 / 80116ea0: bne v0,zero,0x80116f20
80116ea8..80116f1c: +0x1d8 = hitbox+6 + 100 + ((+0x38 - +0x1ba) >> 3), Klemme >= 100 (0x64),
                    sh +0x1d8 -> +0xb4 (rec+4) @0x80116f00, +0xb4+100 -> +0xb6 (rec+6) @0x80116f1c
80116f20..80116f38: lh a1,442(v0) / jal 0x8001b064 / addiu a0,a0,176   ; SCHATTEN-SCHWANZ — wird IMMER erreicht
80116f3c: lw ra,20(sp) ... 80116f48: jr ra
```
Jeder Pfad (Pause-Bit @0x80116dd8, grid&0x20 @0x80116df8, grid&0x40 @0x80116ea0) endet am Schatten-Schwanz @0x80116f20.
Der Root-Skip ueberspringt NUR den Gehirn-Dispatch und die Push-out-/Kollisionsaufrufe (@0x80116e00-e84). STAGE3 Affe
ebenso: `bne ...,0x80110bd0` @0x80110b40, und 0x80110bd0 liegt vor dem Schwanz @0x80110c78 (durchlaufend, keine Abzweigung
zum Epilog). Die Aussage "Wurzel samt Schatten-Schwanz laeuft nicht" ist damit am Byte **falsch**.

Was im Original fuer die grid&0x20-Szenen-Gorillas stattdessen gilt — **im Original-RAM gemessen** (Runde-35-Aufnahme
`orig_scene/r3`, 110 DuckStation-Savestates der 11C0-Szene, gelesen mit `re15_ss.Ram`, Entity-Pool 0x800acc2c Stride 0x1f4):

| Savestate | Gorilla | +0x4..7 | grid +0x9 | pos (x,y,z) | +0x1ba | rec+4/+6 (+0xb4/b6) | **rec+0xc/+0xe (+0xbc/be)** | +0x1d8 |
|---|---|---|---|---|---|---|---|---|
| s001 t=6.11 | slot1 | 1/0/0/0 | 0x30 | (-1220,-20000,-21568) | -3600 | 100/200 | **1700/1800** | 100 |
| s001 t=6.11 | slot2 | 1/0/0/0 | 0x30 | (-554,-20000,-25423) | -3600 | 100/200 | **1700/1800** | 100 |
| s020 t=34.89 | slot1/2 | 1/0/0/0 | 0x30 | (…,-2500,…) Wagen | -3600 | 1837/1937 | **1700/1800** | 1837 |
| s021 t=36.41 | slot1 | 1/0/1/0 | 0x10 | (-3617,0,-17798) | 0 | 1700/1800 | 1700/1800 | 1700 |
| s040 t=65.17 | slot1/2 | 1/5/1/0 | 0x10 | Boden y=0 | 0 | 1700/1800 | 1700/1800 | 1700 |
(Spieler zum Vergleich: +0xbc/+0xbe = 500/600, wie Memory reai-v2-char-shadow.)

Drei Byte-Tatsachen daraus: (a) die Szenen-Gorillas stehen schon bei t=6 in **Zustand 1** mit hp=180 — der INIT lief im
Spawn-Bild (generischer Wurzelaufruf aus Sce_em_set @0x8004256c-0x80042608, J_affen.md Z. 172), der Root-Skip verhindert
ihn also nicht; (b) die Ecken rec+0xc/+0xe sind **1700/1800** und von Anfang an gesetzt — der Schatten-Aufruf
`jal 0x8001b064(a0=+0xb0, a1=+0x1ba=-3600)` laeuft fuer die geparkten (y=-20000) und die Wagen-Gorillas (y=-2500) jedes Bild
mit vollem Quad (ob er auf dem Bildschirm sichtbar ist, entscheidet nur der Quad-Cull FUN_80014368 @0x8001b224 bzw. die
Masken des Cuts — nicht der Root-Skip); (c) rec+4/+6 (+0x1d8-Formel der Wurzel) laufen GETRENNT von den Ecken rec+0xc/+0xe,
die FUN_8001b064 liest — die Groesse des gezeichneten Quads haengt also NICHT an +0x1d8 (das relativiert H1 unten, s. dort).

Folge fuer die Abnahme: der Haken `re15_r39_schatten_aus` (kein Schatten bei grid&0x20) ist als Original-Mechanismus
widerlegt; Kommentare (gegner_r39.c, re15_gegner_r39.h (4), main.c), Dossier Befund B / Punkt 4 und Integrationsriegel (d)
tragen eine falsche Byte-Aussage. Das ist der Fall "zitierte Adresse ist kein Beleg" (Memory
reai-v2-zitierte-adresse-ist-kein-beleg). Als **PORT-WAHL** (der Port hat keine Original-Masken-Verdeckung fuer einen Quad auf
-3600 im Wagen/auf dem Dach; sichtbar waeren zwei neue Fehler, Befund B des Bau-Agenten) ist der Haken vertretbar — dann
aber so zu kennzeichnen, mit der wirklichen Original-Lage (Quad laeuft, Ecken 1700/1800, Hoehe -3600) daneben. Der
Nutzer-Punkt selbst (Sprung) ist davon unberuehrt.

### 4.3 Hinweis H1 (kein Nutzer-Punkt dieser Spur, vorbestehende byte-true-Luecke): Gorilla-Schattengroesse
Im Original-RAM (Tabelle oben) tragen die Gorillas die Schatten-Ecken rec+0xc/+0xe = **1700/1800** (Halb-Extents), der Port
zeichnet den Gorilla-Quad mit den Spieler-Werten **500/600** (main.c `nhx = 500, nhz = 600`, nur Kraehe/Hund/Leichen eigene
Groessen). Original-Bild s040_t065.17.png (Gorilla am Boden): der dunkle Blob unter dem Gorilla ist deutlich breiter als der
Blob des Ports im Framedump F1370. Die Wurzel-Formel +0x1d8 (rec+4/+6, @0x80116ec8-f1c) laeuft daneben her und wird von
FUN_8001b064 nicht gelesen (es liest rec+0xc/+0xe) — wer rec+4/+6 konsumiert, ist offen. Vorbestehend (nicht durch diese
Spur verursacht), nicht Teil des Wortlauts; fuer den Orchestrator als NACHLAUF-Kandidat notiert.

### 4.4 Eigene Messung
- `ctest -R integration_r39_gegner_schatten` selbst gefahren: **Passed 58,67 s** (`lauf_e1730d75`). gegner_r39.log: 24 Zeilen
  `schatten F1362..F1385 slot=3 typ=27 grid=10 st=1/7 koerper_y=-720..-4680 boden_y=0 quad_y=0` (alle quad_y == boden_y),
  595 `schatten_aus`-Zeilen (Szenen-Gorillas grid 0x30).
- Framedumps meines Laufs (PPM -> PNG, scratchpad `fd_001370.png`): der springende Gorilla (links oben, in der Luft) hat
  seinen Blob auf dem Asphalt; der stehende Gorilla in der Mitte ebenso. Gegenbild des Bau-Agenten `E_gegner_bilder/
  vorher_sprung_F1365.png`: Blob in Koerperhoehe in der Luft. Der Nutzer-Befund ist behoben.
- Urteil: **erfuellt** (Punkt); Beleg-Mangel M1 am zusaetzlichen Root-Skip-Haken.

## 5. Vertrag / Tests / Pfade
- Pfade (`git diff c71fa144 --name-only`): analysis/befunde_runde39/E_gegner.md (+5 Bilder), engine/src/{enemy_ai_common.c
  (+4 Zeilen Haken), enemy_ai_re2_dog.c (+3), enemy_ai_re2_zombie.c (+3), gegner_r39.c (neu)}, include/re15_gegner_r39.h
  (neu), platform/pc/main.c (+2 Haken), tests/integration/test_r39_gegner_{hunde1190,schatten}.cmake (neu),
  tests/unit/probes/r39_gegner.cmake (neu, Sonden-je-Thema), tests/unit/test_r35_affen.c (+2 Zeilen), test_r39_gegner.c (neu).
  Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists. Gemeinsame Dateien nur kleine Haken mit
  Kommentar "Runde 39 Spur E". Bank-9-Bits / Nachrichten-Ids: keine (keine zugeteilt, keine benutzt).
- Tabu-Woerter im Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO): keine. `getenv` nur fuer die
  Mess-Schiene RE15_R39_LOG (kein Verhalten).
- `re15_r39_gegner_enable(0)` in test_r35_affen.c room_boot: die drei r35-Riegel (sprung/wagen/schrot) messen den
  ORIGINAL-Mechanismus in 11C0 gegen DuckStation-Savestates; mit der NUTZER-VORGABE waren sie rot (Regression dieser Spur,
  vom Bau-Agenten als Befund A belegt). Die Durchsichtigkeit ist ein Test-Schalter, kein Spielverhalten — zulaessig, aber
  die r35-Riegel messen jetzt nicht mehr den AUSGELIEFERTEN Stand von 11C0 (siehe Hinweis H2).
- Tests selbst gefahren: `ctest -R "unit_.*(r39_gegner|r35_affen|dog|hund|kriech|crawl|affen3|schatten|maggot|griff)"`
  -> **100% passed, 85 Tests**; `integration_r39_gegner_schatten` Passed 58,7 s; `integration_r39_gegner_hunde1190` Passed
  42,3 s. Je Nutzer-Punkt gibt es messende Riegel (vorher/nachher im selben Lauf).
- Hinweis H2: `unit_r35_affen_wagen` prueft "HP 360 / 360 = 180 (Original)" nur noch mit Haken aus; einen Riegel, der den
  ausgelieferten 11C0-Stand (360) am ROOM-Boot misst, gibt es (unit_r39_gegner_affen_hp). OK.

## 6. Zensus-Gegenprobe (eigene Dumps, `scd_dump_room.py`)
- ROOM1090: Sce_em_set-Typen 0x10 x2, 0x11 x2, 0x26 x7, 0x42 x1 — **keine Hunde**; ROOM1190: 0x20 x6 (3 Szene `..20 40..`
  + 3 Kampf `..20 00..`); ROOM11C1: 0x20 x5. Die Zuordnung "Room 1090 mit Hunden" = ROOM1190 des Bau-Agenten (wie Spur B)
  ist damit bestaetigt.
- ROOM11C0: 0x27 x2 Szene (grid 0x30, pc[4]=2, y=-20000 @0x01784/@0x01798) + 0x27 x2 Kampf (grid 0x10 @0x017E8/@0x017FC);
  grid-0x81-Liegespawns in 1010/1011/1220/1221 (+ 4050/4051, 5060/5061, Typ 0x18) wie in §3.1.

## 7. Maengel (nummeriert)
- **M1 (RE-Gate, Beleg falsch):** gegner_r39.c (Kopf re15_r39_schatten_aus), re15_gegner_r39.h (4), main.c-Haken-Kommentar,
  Dossier Befund B / Punkt 4 und test_r39_gegner_schatten.cmake (d) behaupten, `andi 0x20 / bne` @0x80116df4-f8 ueberspringe
  die Wurzel samt Schatten-Schwanz. Das Sprungziel 0x80116e88 liegt vor dem Schwanz @0x80116f20, der immer laeuft (eigener
  Disasm oben; STAGE3 @0x80110b40 -> 0x80110bd0 ebenso). Nachbesserung: entweder den wirklichen Mechanismus belegen, der
  im Original fuer grid&0x20-Gorillas keinen sichtbaren Quad erzeugt (Ecken +0xbc/+0xbe vor dem INIT = 0? Sichtbarkeits-
  Test FUN_80014368 @0x8001b224? -> Savestates orig_scene/r3 lesen), und die Kommentare/Riegel darauf umschreiben — oder den
  Haken als PORT-WAHL kennzeichnen (Begruendung: der Port kennt keinen degenerierten Record) und die falsche Byte-Aussage
  streichen. Ohne das steht eine falsche Disasm-Behauptung im Code.

## 8. Hinweise (keine Maengel gegen den Wortlaut)
- H1 Gorilla-Schattengroesse 1700/1800 (Original-RAM) gegen 500/600 (Port) — vorbestehend, §4.3.
- H2 Die r35-Affen-Riegel messen 11C0 nur noch mit `re15_r39_gegner_enable(0)` (Original-Mechanismus); der ausgelieferte
  11C0-Stand (360, Schwelle 36) wird von test_r39_gegner gemessen. OK, aber bei Aenderungen an gegner_r39.c beide Gruppen fahren.
- H3 Flavor: Hund und Kriecher wurden an der exe nur im RE2-Flavor (Nutzer-Standard) gemessen; RE1.5-Flavor nur in den
  Unit-Riegeln hund_re15 / kriecher_*_re15 (dort ist der Haken unbeteiligt: acae7-Tor @0x8010f294 bzw. Block B nativ).
- H4 Entscheidungsfrage des Bau-Agenten (3-Treffer-Schwelle / HP x2 auch fuer ROOM1141?) bleibt beim Nutzer; die Umsetzung
  folgt dem Wortlaut "beim Parking Lot" (= 11C0).
- H5 Nebenwirkung des Kriech-Weckers in 4050/4051 und 5060/5061 (Typ 0x18, grid 0x81): im RE1.5-Original ebenso Block B;
  nicht an der exe gemessen (kein Nutzer-Raum dieser Spur).

## 9. Ergebnis
- Punkte 1-4: **erfuellt** (jeweils selbst an der exe gemessen, Original-Bytes selbst gelesen).
- Gates: Vertrag/Pfade/Tests OK; **RE-Gate verletzt (M1)** — eine verhaltensrelevante Port-Entscheidung (kein Schatten bei
  grid&0x20) traegt eine am Byte falsche Mechanismus-Behauptung, im Original-RAM widerlegt.
- **bestanden = false** bis M1 behoben ist (Nachbesserung ist rein dokumentarisch/kennzeichnend, sofern der Haken als
  PORT-WAHL bleibt; der Riegel test_r39_gegner_schatten (d) ist dann als Port-Wahl-Riegel zu beschriften, nicht als
  Original-Beleg).

## 10. Pruefprotokoll (Werkzeuge)
- Disasm: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (STAGE1.BIN/STAGE3.BIN/PSX.EXE), `re2_disasm.py`
  (`--bin EMD0G_MOD0.BIN`, RE2 PSX.EXE Leon). Savestates: `re15-savestate-ghidra/scripts/re15_ss.py` auf
  `scratchpad/orig_scene/r3/s0xx_*.sav` (Runde 35, DuckStation). Zensus: `re15_port/tools/scd_dump_room.py`.
- Exe-Laeufe: Messkopien `re15_pc_abn0_p1.exe`, `re15_pc_abn0_p3.exe`, `re15_pc_abn0_p3e.exe` neben der exe (nach der
  Abnahme geloescht); ein haengender eigener Lauf (p3c, NEW GAME statt CONTINUE wegen `RE15_TITLE_SHOT_AF=2`) wurde per PID
  der eigenen Kopie beendet — kein `taskkill /IM re15_pc.exe`.
- ctest: `-R "unit_.*(r39_gegner|r35_affen|dog|hund|kriech|crawl|affen3|schatten|maggot|griff)"` 85/85;
  `integration_r39_gegner_schatten` 58,7 s; `integration_r39_gegner_hunde1190` 42,3 s (je einmal, gruen; keine volle Suite).
