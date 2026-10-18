# Runde 39 — Spur E "gegner": UNABHAENGIGE ABNAHME 1 (2026-10-18, nach Nachbesserung 1)

Pruefer: Abnahme-Agent (unabhaengig vom Bau-Agenten). Baum `.claude/worktrees/r39_gegner`, Zweig `r39/gegner`,
HEAD f20734fa, Merge-Basis c71fa144 (`git diff c71fa144`: 18 Dateien, +2109/-3; der Diff gegen `master` zeigt
Fremdaenderungen der Spuren A/D, gewertet wird die Merge-Basis). Delta seit Abnahme 0 (`git diff 8338a13e HEAD -- re15_port`):
gegner_r39.c (Root-Skip-Haken `re15_r39_schatten_aus` entfernt, `re15_r39_schatten_y` ohne grid&0x20-Ausstieg, Kommentar
neu), re15_gegner_r39.h (4), main.c (-1 Hakenzeile), test_r39_gegner.c / test_r39_gegner_schatten.cmake / r39_gegner.cmake
(Riegel umgeschrieben). **Der Code der Punkte 1-3 ist seit Abnahme 0 unveraendert** (enemy_ai_common.c, enemy_ai_re2_dog.c,
enemy_ai_re2_zombie.c, Teile (1)-(3) von gegner_r39.c: kein Diff).

Gebaut: `bash re15_port/tools/local_build.sh configure && ... build` im Spurbaum -> `LOCAL-BUILD-OK`, `ninja: no work to
do` (exe 13:43 > letzte Quelle 13:4x = Stand f20734fa; test_r39_gegner.exe 13:46). Alle Messungen an Messkopien
`re15_pc_abn1_<tag>.exe` neben der exe (Asset-Wurzel = exe-Ordner), Arbeitsordner `scratchpad/abn1/<tag>`; kein
`taskkill /IM`, keine volle Suite.

## 0. Kurzurteil

| Punkt | Urteil | Kern des Belegs (alles selbst gemessen) |
|---|---|---|
| 1 Parking-Lot-Gorillas HP x2, Flinch erst nach 3 Pistolentreffern | **erfuellt** | exe 11C0: hp 360 -> 348 / 336 ohne Clip 7 -> 324 FLINCH (Clip 7 F440-F452) -> 312 ohne; 6 Schuesse, 4 Treffer; Code unveraendert seit Abnahme 0 (180 @0x8011f514, 12 @0x8006ee44 dort selbst gelesen) |
| 2 kein paralleler Hundebiss | **erfuellt** | integration hunde1190 selbst: 4 Bisse, je EIN Hund in Sub 3/3, Riegel 23 Bilder; NEU Szenen-Vergleich 11C1 (Elza, 5 Hunde): v0.8.26-exe beisst F1008 **und F1019** (11 Bilder, Tod in der Reaktion) — f20734fa-exe bildgleich bis F1008, Riegel FREI F1031, naechster Biss erst F1040 (32 Bilder) |
| 3 liegende Zombies kriechen (1010/1011, 1220/1221) | **erfuellt** | vier Exe-Laeufe (Leon 1010 ueber die Tuer, Leon 1220, Elza 1221 CONTINUE, Elza 1011 ueber die Tuer): Wecker bei dist 2996 / 2126 / 1999 im Kegel, Kriechen, Griff, HP-Abzug; der Kriecher ausserhalb des Kegels bleibt liegen |
| 4 Gorilla-Schatten beim Sprung am Boden (+ M1) | **erfuellt, M1 behoben** | Wurzel-Schwanz @0x80116f20-38 auf jedem Pfad selbst disassembliert (Sprungziele 0x80116e88 / STAGE3 0x80110bd0 liegen VOR dem Schwanz); Region-Cull @0x8001b180/@0x8001b224/@0x8001b22c + FUN_80014368 selbst gelesen; Original-RAM-Zonen selbst aus den Savestates gelesen (identisch mit der Tabelle); exe: 404 Szenen-Quads auf -3600, 24 Sprung-Bilder quad_y == boden_y, Framedumps gegen s020/s021/s024/s026 ohne Fremd-Blob |

Gates: Vertrag OK, Tests OK (85/85 Unit + 9/9 Exe-Integration der beruehrten Gruppen), RE-Gate OK (M1 der Abnahme 0 ist
am Byte behoben, s. §4.1). **bestanden = true.** Maengel: keine. Hinweise H1-H7 (§6), davon keiner gegen den Wortlaut.

## 1. Punkt 1 — Parking-Lot-Gorillas (ROOM11C0)

Code seit Abnahme 0 unveraendert (RE-Gate dort: HP-Saat `sh v0,154(a1)` @0x80117104, Zeile 0x27 @0x8011f514 = 16 x 180;
Pistole 12 @0x8006ee44; Faktor 2 und Schwelle 3 x 12 = 36 als NUTZER-VORGABE gekennzeichnet). Erneut an der neu gebauten
exe gemessen (Messkopie `re15_pc_abn1_p1b.exe`, Arbeitsordner `abn1/p1b_11c0`):

`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_SET_FLAG=4:0x40 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-6729,-12800,1486 RE15_EQUIP=3
RE15_INPUT_SCRIPT="M1.5,MA0.1,M0.6,MA0.1,M0.6,...(14 x)" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=290
RE15_STATE_LOG=state.log RE15_R39_LOG=1 RE15_EXIT_AT=900` -> EXIT 0.

- gegner_r39.log: `affen init slot=2/3 raum=11c0 hp=360 (tabelle 180 @0x8011f514)`; `treffer slot=2 hp=348 akku=12 -> OHNE
  ZUCKEN (zurueck in 1/5/1/0)`, `hp=336 akku=24 -> OHNE ZUCKEN (1/3/1/0)`, `hp=324 akku=36 -> FLINCH (Schwelle 36)`, `hp=312
  akku=12 -> OHNE ZUCKEN (1/7/1/0)`.
- state.log (eigene Auswertung): Schuesse (Magazin faellt) F335/379/440/462/484/524; Treffer F335 360->348 `st=1/5/1/0 mo=18`
  (kein Clip 7), F379 348->336 `st=1/3/1/0 mo=5` (kein Clip 7), **F440 336->324 `st=2/3/1/1 mo=7` = Flinch-Clip 7 F440-F452,
  Exit F453 -> `st=1/7/1/0 mo=20`** (Vergeltungs-Sprung der Runde 35), F462 324->312 `st=1/7/1/0 mo=20` ohne Clip 7. Genau ein
  Clip-7-Start im Lauf. Bildgleich mit Abnahme 0 (deterministisch).
- Bedienhinweis (kein Befund): mein erster Lauf benutzte die Kurzform `(MA0.1,M0.6)x12` aus dem Abnahme-0-Bericht — der
  Skript-Parser liest `x12` als Kreuz-Taste 12 s (426 Ticks, `Tick 66 -> 0x4000`), es fiel nur EIN Schuss. Die Kurzform war
  im Abnahme-0-Bericht eine Abkuerzung der ausgeschriebenen Folge (debug.log des damaligen Laufs: 25 Glieder, 297 Ticks).
- Tests selbst: unit_r39_gegner_affen_hp / _affen_zucken / _affen_schuss gruen; unit_r35_affen_* 20/20 (Original-Riegel mit
  `re15_r39_gegner_enable(0)`).
- Urteil: **erfuellt** (Wortlaut "beim Parking Lot" = ROOM11C0; ROOM1141 bleibt 180 — Entscheidungsfrage an den Nutzer, H5).

## 2. Punkt 2 — kein paralleler Hundebiss (RE2-Hundemodul)

Code seit Abnahme 0 unveraendert (RE-Gate dort selbst disassembliert: Riegel `ori v1,v1,0x80` @0x801051f8 / `sb v1,467(a0)`
@0x80105200, Kontakt-Tor @0x80104e5c-68, Flug-Tor @0x80104d84-94, Freigabe `sb zero,467(s0)` @0x8003f8b4).

### 2.1 integration_r39_gegner_hunde1190 (echte exe ROOM1190, selbst gefahren)
Passed 42,24 s, Lauf `r39_gegner_hunde1190_wd/lauf_9c14878c`: Bisse F685 (100->80), F720 (->60), F753 (->40), F833 (->20);
im Biss-Bild je genau EIN Hund in Sub 3/3 (F685 Hund 1 d=949, F720 Hund 2 d=2039, F753 Hund 3 d=949, F833 Hund 3 d=1286),
die anderen in 2/1; gegner_r39.log `GESETZT F685 / FREI F708`, `F720 / F743`, `F753 / F776`, `F833 / F856` = 23 Bilder je
Riegel; Abstaende 35/33/80. Bildgleich mit Abnahme 0.

### 2.2 NEU: Szenen-Vergleich vorher/nachher in ROOM11C1 (Elza-Parking-Lot, fuenf Hunde 0x20)
Spielstand `probe_r36_elza_karte re15_card.mcr 11C1 -22604 14455 0` (Figur 4 = Elza), beide Laeufe mit derselben Karte und
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
RE15_CARD_SLOT=0 RE15_STATE_LOG=state.log RE15_EXIT_AT=1030` (debug.log `[save] CONTINUE: resumed in room 11c1`,
`Spieler-Familie PL04`):
- **vorher** = `release/pkg-win/re15_port_v0.8.26/re15_pc.exe` (v0.8.26-Paket, in seinem Ordner gestartet, keine r39-Strings
  in der exe; Arbeitsordner `abn1/p2_11c1_v`): Bisse F779 (100->80, Hund 5 Sub 3/3), F862 (Hund 2), F906 (Hund 5), F947 (Hund 1;
  Hund 2 gleichzeitig in 3/2), **F1008 (20->0, Hund 4 Sub 3/3; Hund 5 in 2/2 d=964) und F1019 (0->-20, Hund 5 Sub 3/3 d=951 UND
  Hund 4 Sub 3/3)** — der zweite Biss faellt 11 Bilder nach dem ersten, mitten in Elzas 22-Bilder-Reaktion, und toetet sie =
  der Nutzer-Befund als Spielszene.
- **nachher** = exe f20734fa (Messkopie `re15_pc_abn1_p2n.exe`, `abn1/p2_11c1_n`): bildgleich bis F1008 (F779/F862/F906/F947/
  F1008, Abstaende 83/44/41/61), **kein weiterer Biss bis zum Laufende F1030**; gegner_r39.log `GESETZT F779 / FREI F802`,
  `F862 / F885`, `F906 / F929`, `F947 / F970`, `GESETZT F1008` (Riegel steht am Laufende noch). Bei F947 war Hund 2 im Flug
  (Sub 3/2, d=1644) und biss nicht; bei F1008 waren Hund 2 und 5 in Sub 8/1 (abgewiesen) statt im Biss.
- Verlaengerter Nachher-Lauf bis F1150 (`abn1/p2_11c1_n2`): siehe §2.3.
- Der Riegel macht den Spieler nicht unsterblich: ein Hund beisst wieder, sobald die Reaktion vorbei ist (Freigabe wie
  @0x8003f8b4).

### 2.3 Verlaengerter Nachher-Lauf (`abn1/p2_11c1_n2`, dieselbe Karte, `RE15_EXIT_AT=1150`)
Bildgleich mit §2.2 nachher bis F1008 (Bisse F779/F862/F906/F947/F1008, Riegel GESETZT/FREI je 23 Bilder); `GESETZT F1008 /
FREI F1031`; **naechster Biss F1040** (Hund 1 Sub 3/1, hp 0 -> -20 = Tod) — 32 Bilder nach F1008, also erst NACH dem Ende
der Reaktion. Vorher stirbt Elza bei F1019 durch den parallelen zweiten Biss in der Reaktion, nachher bei F1040 durch den
naechsten regulaeren Biss danach: der Riegel sperrt genau die Reaktion, nicht mehr (Abstaende 83/44/41/61/32, min 32 >= 24).

- Tests selbst: unit_r39_gegner_hund_riegel (vorher -40 im selben Bild / nachher -20, Riegel 0x80, Freigabe, fremder Riegel
  bleibt) und unit_r39_gegner_hund_re15 (RE1.5-Flavor: acae7-Tor @0x8010f294 nativ) gruen; unit_re2_dog_* / unit_dog_* /
  r30_hund_* / r36_kleidung_hund* / integration_r36_kleidung_hundeweg gruen (keine Nebenwirkung).
- Urteil: **erfuellt** — Mechanismus (RE2-Byte PL+0x1D3) am Unit-Riegel, am echten 1190-Weg und jetzt auch als Spielszene
  (11C1 vorher/nachher) belegt. ROOM1190 selbst liefert am skriptierten Weg keinen Doppelbiss (Hunde kommen nacheinander),
  wie von Bau-Agent und Abnahme 0 benannt.

## 3. Punkt 3 — liegende Zombies kriechen (RE2-WAIT + RE1.5-Wecker Block B)

Code seit Abnahme 0 unveraendert (RE-Gate dort: `sltiu v0,v0,0xbb8` @0x80104718, arc 0x200 @0x8010472c-34, Wort 1
@0x80104738/@0x80104748 selbst disassembliert). Vier eigene Exe-Laeufe an der neuen exe (Messkopien `re15_pc_abn1_p3*.exe`),
Standard-Flavor RE2:

| Lauf | Aufbau | Messung (state.log / gegner_r39.log, eigene Auswertung) |
|---|---|---|
| `p3a_1010` Leon, ECHTE Tuer | `RE15_DEBUG_JUMP=1020@240 RE15_FIRE_AOT=0@60#1020` (ROOM1020 Door_aot 0 -> 1010 Cut 4), `RE15_INPUT_SCRIPT=R0.25,U2` ab 420, EXIT 720 | nach der Tuer beide 0x10 g=81 `1/2/1 mo=23` bei (950,-1700) d=3510 / (-50,-3800) d=3700; **F435 `kriecher weckt slot=3 dist=2996`** -> `1/0/1 mo=5`, kriecht (950,-1700) -> (1655,-1898) F453 -> (2303,-1914) F462 Griff `1/1/x mo=14-16`, Leon hp 100->95 F464 ->90 F494 ->85 F524, F580 `st=7`; Kriecher B bleibt `1/2/1` bis F720 (d=4344, ausserhalb des Kegels) |
| `p3b_1220` Leon, Debug-Sprung | `RE15_DEBUG_JUMP=1220@240 RE15_PLAYER_POS=-23500,-7700,2048`, EXIT 600 | F6 beide 0x16 g=81 `1/2/0 mo=23` d=2126/1999; **F7 beide `1/0/1 mo=5`** (`weckt slot=1 dist=2126`, `slot=2 dist=1999`); Kriecher 2 (-23500,-9700) -> (-23500,-8986) F24 Griff, hp 95 F35 / 90 F65 / 85 F95, F151 `st=7`; Kriecher 1 Griff F167, hp 80/75/70 (F178/208/238), F294 `st=7` |
| `p3c_1221` ELZA, CONTINUE | Karte Abnahme 0 (Raum 0x1221, Figur 4, (-23500,-7700) rot 2048), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_EXIT_AT=400#1221`; debug.log `CONTINUE: resumed in room 1221`, `PL04`, `[elzaems] Typ 0x16` | F0 beide `1/2/0 mo=23` (liegend); **F1 beide `1/0/1 mo=5`** (dist 2126 / 1999); Kriecher 2 Griff F18, Elza hp 95 F29 / 90 F59 / 85 F89, F145 `st=7`; Kriecher 1 Griff F161, hp 80 F172 ... — bildgleich mit Leons 1220-Lauf (Versatz 6 Bilder = Debug-Sprung-Vorlauf) |
| `p3d_1011` ELZA, ECHTE Tuer | Karte Abnahme 0 (Raum 0x1021, (-2750,-7250) rot 0), `RE15_FIRE_AOT=0@60#1021` (-> 1011 Cut 4), `RE15_INPUT_SCRIPT=R0.25,U2` ab 300, `RE15_EXIT_AT=360#1011`; debug.log `resumed in room 1021`, `loaded room1011.rdt`, `[elzaems] Typ 0x10` | nach der Tuer beide 0x10 g=81 `1/2/1 mo=23` (950,-1700) / (-50,-3800); **F315 `weckt slot=4 dist=2996`** -> `1/0/1 mo=5`, (1655,-1898) F333 -> (2303,-1914) F342 Griff `1/1/x`, Elza hp 100->95 F344; Kriecher B liegt (d=3873) — bildgleich mit Leon 1010 |

- Tests selbst: unit_r39_gegner_kriecher_1010 / _1220 (RE2: vorher Weg 0, nachher Wort 1 + Weg > 1600) und _1010_re15 /
  _1220_re15 (Block B nativ) gruen; unit_re2_crawler, unit_re2z_crawl_hurt_reentry, unit_re2z_crawler_spawn_wait,
  unit_1030_crawl_live, unit_crawl_toggle, unit_r36_blut_pin_kriecher*, unit_r38_griff_kriecher2_1220 gruen.
- Urteil: **erfuellt** (alle vier Raeume, beide Figuren, Tuer-Weg fuer 1010/1011). Form (RE1.5-Block-B-Wecker als zweiter
  Ausgang des RE2-WAIT) als NUTZER-VORGABE gekennzeichnet und mit den Original-Bytes belegt.

## 4. Punkt 4 — Gorilla-Schatten (Nachbesserung 1 = Mangel M1 der Abnahme 0)

### 4.1 RE-Gate — die neuen Byte-Aussagen selbst disassembliert (`re15_disasm.py`)
**(a) Gorilla-Wurzel FUN_80116db8, STAGE1.BIN (`dis 0x80116db8 104`):**
```
80116dd8: bne v0,zero,0x80116e88      ; Pause-Bit -> 0x80116e88
80116dec: lbu v0,9(v1) / 80116df4: andi v0,v0,0x20 / 80116df8: bne v0,zero,0x80116e88   ; Root-Skip -> 0x80116e88
80116e00-80116e24: Gehirn-Dispatch (Tabelle 0x801213c8, jalr @0x80116e20)
80116e28-80116e84: jal 0x8002b498 / 0x8002aec4 (sh +0x1d2) / 0x8002b544 / 0x8003b0a4 (sh +0x1d6)
80116e88: lui a1,0x800b / lw a1,g_entity                       ; <- beide Sprungziele landen HIER
80116e94-ea0: lbu 9(a1) / andi 0x40 / bne -> 0x80116f20
80116ea8-f1c: +0x1d8 = hitbox+6 + 100 + ((+0x38 - +0x1ba) >> 3), Klemme 0x64, sh +0xb4 @0x80116f00, +0xb6 @0x80116f1c
80116f20-f38: lw v0,g_entity / lw a0,g_entity / lh a1,442(v0) @0x80116f30 / jal 0x8001b064 @0x80116f34 / addiu a0,a0,176 @0x80116f38
80116f3c-f4c: lw ra / lw s0 / addiu sp / jr ra                 ; einziger Epilog
```
Jeder Pfad endet im Schatten-Schwanz. Die Behauptung in gegner_r39.c / re15_gegner_r39.h (4) / Dossier / Riegel (d) stimmt
jetzt mit den Bytes ueberein; die falsche Root-Skip-Aussage des Stands c3823fa4 ist aus Code, Header, main.c und Riegel
entfernt (`git diff 8338a13e HEAD`). **M1 behoben.**
**(b) Kleiner Affe FUN_80110b10, STAGE3.BIN (`--bin STAGE3.BIN dis 0x80110b10 100`):** `andi 0x20` @0x80110b3c / `bne
v0,zero,0x80110bd0` @0x80110b40; 0x80110bd0 = `lui a1 / lw a1,g_entity`, grid&0x40 @0x80110be4-e8 -> 0x80110c68, +0x1d8-Formel
@0x80110bf0-c64, Schwanz `lh a1,442(v0)` @0x80110c78 / `jal 0x8001b064` @0x80110c7c / `addiu a0,a0,176` @0x80110c80, Epilog
@0x80110c84-94 — stimmt.
**(c) Region-Cull FUN_8001b064 (PSX.EXE, `dis 0x8001b170 52`):** `lw a1,-14448(a1)` = DAT_800ac790 @0x8001b180; a0 = g_entity
+ 52 (+0x34 = Position) @0x8001b1ec-f4; `jal 0x80014368` @0x8001b224; `beq v0,zero,0x8001b364` @0x8001b22c — stimmt.
**FUN_80014368 (`dis 0x80014368 30` + `dis 0x800143d4 30`):** liest `lw v0,8(a0)` (z) @0x80014370 und `lw v0,0(a0)` (x)
@0x80014384, Ecken `lh 4/6/8/10/16/18/12/14(a1)`, nie +4 (Hoehe); vier mult/slt-Paare @0x80014380-b0, @0x800143b8-cc,
@0x800143ec-40c, @0x80014414-28, Rueckgabe 1 (`ori v0,zero,0x1` @0x8001442c) / 0 (@0x80014430) — die vier Halbebenen-Bedingungen
im Riegel bzw. im zone_read.py entsprechen genau diesen Paaren (selbst nachgerechnet).
**(d) Zonen-Lookup FUN_80014324 (`dis 0x80014324 17`):** `lw v0,DAT_800ac778` / `lw v1,40(v0)` (RDT+0x28) @0x80014330, `lbu
v0,2(v1)` == a0 (Cut) @0x80014338-54, Schritt 20 — stimmt. **Cut-Apply FUN_80021bbc (`dis 0x80021bf4 10`):** `jal 0x80014324`
@0x80021c00 / `sw v0,-14448(at)` = DAT_800ac790 @0x80021c0c — stimmt.
**(e) INIT im Spawn-Bild, Sce_em_set (`dis 0x80042550 50`):** `lbu v1,9(s0)` @0x80042564 / `andi v0,v1,0xdf` @0x8004256c /
`sb v0,9(s0)` @0x80042570 / Dispatch 0x80072bac / `jalr v0` @0x8004259c mit Delay-Slot `andi s4,v1,0x20` @0x800425a0 / `lbu
v0,9(s0)` @0x800425fc / `or v0,v0,s4` @0x80042604 / `sb v0,9(s0)` @0x80042608 — stimmt (Port: enemy_ai_common.c
re15_enemy_spawn_root:14455ff, `s4 = grid & 0x20` @0x800425a0).
**(f) Ecken 1700/1800 (`dis 0x801171d8 12`):** `lhu a3,6(v1)` (hitbox+6 = 1600) @0x801171d8, `addiu a2,a3,100` @0x801171e8,
`jal 0x8001af5c` @0x801171ec, `addiu a3,a3,200` @0x801171f0 -> 1700/1800 — stimmt (RAM unten).

### 4.2 Original-RAM selbst gelesen (Runde-35-Savestates `orig_scene/r3`, `zone_read.py` des Bau-Agenten gelesen und selbst
ausgefuehrt, `re15_ss.Ram`, Pool 0x800acc2c Stride 0x1f4, Cut 0x800b0fe4, Zone = DAT_800ac790 -> 20 Byte, Ecken @+4..+0x12)

| Savestate | Cut | DAT_800ac790 | Zone (x,z) x4 | G1 (slot1) | G2 (slot2) | +0x1ba | +0xbc/+0xbe | in_quad G1/G2 |
|---|---|---|---|---|---|---|---|---|
| s001 t=6.11 | 0 | 0x80121b80 (STALE, Kopf 0x1950) | (1530,0)(17586,0)(10768,-32750)(0,-32590) | 1/0/0/0 grid 30 (-1220,-20000,-21568) | 1/0/0/0 grid 30 (-554,-20000,-25423) | -3600 | 1700/1800 | 0/0 |
| s002 t=7.64 | 1 | 0x80121c00 | (-30301,4702)(-28200,27002)(1800,29703)(2603,4502) | dito | dito | -3600 | 1700/1800 | 0/0 |
| s004 t=10.69 | 3 | 0x80121c8c | (-29901,-20799)(-29300,19505)(-2098,19505)(-3693,-20298) | dito | dito | -3600 | 1700/1800 | 0/0 |
| s005 t=12.19 | 4 | 0x80121cdc | (-30500,-28901)(-29700,19803)(-2897,20704)(-2899,-29900) | dito | dito | -3600 | 1700/1800 | 0/0 |
| s010 t=19.78 | 13 | 0x80121ef8 | (-24200,-18102)(-24502,5700)(12901,5200)(13004,-17901) | dito | dito | -3600 | 1700/1800 | 0/0 |
| s020 t=34.89 | 12 | 0x80121ed0 | (-23700,-21102)(-24502,-1300)(5401,-1300)(5504,-25401) | (-1220,-2500,-21568) Wagen, rec+4/6 1837/1937 | (-554,-2500,-25423), 1837/1937 | -3600 | 1700/1800 | **1/0** |
| s021 t=36.41 | 12 | 0x80121ed0 | dito | (-3617,0,-17798) grid 10, +0x1ba 0, rec+4/6 1700/1800 | (-1220,-2500,-21568) Wagen | 0 / -3600 | 1700/1800 | 1/1 |
| s022 t=37.92, s026 t=44.00 | 5 | 0x80121d18 | (-21767,-31201)(-18463,-3298)(18004,-3300)(20301,-32700) | (-3617,0,-17798) | (-1220,-2500,-21568) Wagen | 0 / -3600 | 1700/1800 | 1/1 |

Identisch mit der Tabelle des Bau-Agenten (E_gegner.md Nachbesserung 1) und mit den Zonen im Riegel unit_r39_gegner_schatten
(Cut 0 dort = Anker @0x80121bd8 statt des stalen Zeigers; beide schliessen die geparkten Gorillas aus). Nebenbefund selbst
bestaetigt: rec+4/+6 am Boden ebenfalls 1700/1800 (+0x1d8-Formel mit +0x38 == +0x1ba), im Wagen 1837/1937.

### 4.3 Messung an der exe (Messkopie `re15_pc_abn1_p4_11c0.exe`, Arbeitsordner `abn1/p4_11c0`)
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-22604,14455,0 RE15_STATE_LOG=state.log RE15_R39_LOG=1
RE15_FRAMEDUMP=760-1400/10:fd_ RE15_EXIT_AT=1500#11C0` -> EXIT 0 (Aufbau = integration_r39_gegner_schatten).
- debug.log: Cut_chg(13) F213, Cut_chg(12) F763, Cut_chg(5) F880; state.log cam 0 -> 1 -> 2 -> 3 -> 4 -> 5 (F184) -> 13 (F213)
  -> 12 (F763) -> 5 (F880).
- gegner_r39.log, eigene Auswertung (`schatten`-Zeilen = Bilder mit gezeichnetem Quad und koerper_y != boden_y): grid 30
  slot 2/3 geparkt (-20000, Quad -3600) F185-F213 je 29 Zeilen (cam 5 nach dem Debug-Sprung + Cut_chg(13)-Bild; Original-
  Reihe 0 -> 1 -> 3 -> 4 -> 13 kennt vor Cut 13 kein Cut 5 — Debug-Sprung-Artefakt, H6); slot 2 im Wagen (-2500) F764-F798
  (35); slot 3 im Wagen F800-F1110 (311: Cut 12 bis F879, Cut 5 ab F880); **Cut-13-Fenster F214-F762: keine Zeile**; **Sprung
  slot 3 grid 10 st 1/7 F1362-F1385 (24 Zeilen): koerper_y -720 .. -4680 .. -720, boden_y 0, quad_y 0**. Zeilen mit quad_y !=
  boden_y: **0** von 404 + 24. Identisch mit lauf_59e277d7 des Bau-Agenten.
- integration_r39_gegner_schatten selbst gefahren: Passed 58,64 s (a-d gruen).
- Framedumps (65 PPM -> PNG, Kontaktbogen `abn1/p4_11c0/sheet_cut12.png`, `sheet_hatch_zoom.png`, `sheet_cut5.png`,
  `sheet_sprung.png`, Original 320x240 daneben): Cut 12 F770/F790 (Gorilla 1 in der Klappenoeffnung, Heckklappe davor) gegen
  s020: kein Blob auf dem weissen Klappenrahmen, Innenraum schwarz wie im Original (4x-Zoom des Bands 120..214 x 107..130);
  F800/F860 (Gorilla 1 draussen vorn, Gorilla 2 im Wagen) gegen s021: gleich; Cut 5 F900/F1000 (Leon/Ada am Wagen, Gorilla
  links) gegen s024/s026: links kein Fremd-Quad, F1100 Gorilla 2 draussen; Sprung F1360 (Absprung), **F1370 (Gorilla oben am
  Bildrand in der Luft, Blob am Boden)**, F1380 (in der Luft ueber dem stehenden Gorilla, Blob am Boden), F1390 gelandet.
  Einschraenkung: Framedump = Software-Renderer (Memory: Farben/Texturen nur per gdigrab belastbar) — fuer die LAGE des Quads
  (Nutzer-Punkt) hinreichend; der Mechanismus ist ueber +0x1ba @0x80116f30 belegt, nicht ueber das Bild.
- Urteil: **erfuellt**; M1 behoben (Original-Lage statt Port-Sonderweg, Byte-Belege korrekt).

## 5. Vertrag / Tests / Pfade
- Pfade (`git diff c71fa144 --name-only`): analysis/befunde_runde39/{E_abnahme_0.md, E_gegner.md, E_gegner_bilder/*},
  engine/src/{enemy_ai_common.c (+7: include, 2-Zeilen-Blut-Zwilling, 3 Haken, Riegel-Tick), enemy_ai_re2_dog.c (+4),
  enemy_ai_re2_zombie.c (+4), gegner_r39.c (neu)}, include/re15_gegner_r39.h (neu), platform/pc/main.c (+2), tests/integration/
  test_r39_gegner_{hunde1190,schatten}.cmake (neu), tests/unit/probes/r39_gegner.cmake (neu, Sonden-je-Thema), tests/unit/
  test_r35_affen.c (+2), test_r39_gegner.c (neu). Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists.
  Gemeinsame Dateien nur kleine Haken mit Kommentar "Runde 39 Spur E". Bank-9-Bits / Nachrichten-Ids / Flags: keine (grep auf
  flag_set/flag_get/MSG_ID im Code-Diff leer). `git status` sauber (nur dieser Bericht neu).
- Tabu-Woerter (deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/hack) im Diff: keine. `getenv` nur
  RE15_R39_LOG (Mess-Schiene, kein Verhalten). `re15_r39_gegner_enable(0)` nur in Riegeln (vorher/nachher im selben Lauf).
- Jede Konstante im Diff traegt @0x... oder NUTZER-VORGABE (Faktor 2, 3 Treffer, Form des Weckers): RE15_R39_AFFEN_HP_TABELLE 180
  @0x8011f514, RE15_R39_PISTOLE_SCHADEN 12 @0x8006ee44, RE15_R39_KRIECH_WECK_DIST 0xbb8 @0x80104718, RE15_R39_KRIECH_WECK_ARC
  0x200 @0x8010472c-34, Quad-Y +0x1ba @0x80116f30 / @0x80110c78, Riegel-Byte @0x801051f8-0x80105200 / @0x80104e5c-68.
- Tests selbst gefahren (Spurbaum, keine volle Suite): `ctest -j4 -R "^unit_.*(r39_gegner|r35_affen|affen3|schatten|dog|hund|
  kriech|crawl|maggot|griff)"` -> **100% passed, 85 Tests**; `ctest -j1 -R "^(integration|r30).*(...)"` (r30_hund_hitbox_
  stauchung, integration_r36_kleidung_hundeweg 57 s, integration_r36_pri_schatten 28 s, integration_r37_elzaems_elza1181griff
  13 s, integration_r38_affen3 84 s, _decke 91 s, _tor 91 s, integration_r39_gegner_schatten 59 s, integration_r39_gegner_
  hunde1190 42 s) -> **100% passed, 9 Tests**. Je Nutzer-Punkt messende Riegel (vorher/nachher) vorhanden. Neue Tests
  parallel-fest (eigene Laufordner, exe-Kopien mit Praefix `re15_pc_r39e_<test>_`, Umgebungsvariablen des Aufrufers geloescht)
  und Linux-fest (`cmake_minimum_required` in beiden -P-Skripten, `m` fuer Nicht-WIN32, keine .exe-/Backslash-Pfade).
- Nebenwirkungen: Hund/Kriecher-Haken nur im RE2-Modul (RE1.5-Flavor nativ, Riegel hund_re15 / kriecher_*_re15 gruen); der
  Kriech-Wecker wirkt auch auf 0x81-Spawns in 4050/4051, 5060/5061 (Typ 0x18 — im RE1.5-Original ebenso Block B; nicht an der
  exe gemessen, H4); Schatten-Haken 0x29: +0x1ba bleibt im Port die Boden-Saat (affen3_r38.c:891, Decke/Sprung relativ dazu
  @0x80112f10-28 / @0x80114318-1c) = Original-Schwanz @0x80110c78 (H3).

## 6. Hinweise (keine Maengel gegen den Wortlaut)
- H1 (vorbestehend, Abnahme 0): Gorilla-Schattengroesse Original 1700/1800 (+0xbc/+0xbe, hier erneut aus dem RAM gelesen)
  gegen Port 500/600 (main.c nhx/nhz) — NACHLAUF-Kandidat, kein Punkt dieser Spur.
- H2 (Bau-Agent, selbst bestaetigt): DAT_800ac790 ist beim Raumeintritt bis zum ersten Cut-Apply stale (s001: 0x80121b80,
  misaligned, Kopf 0x1950); der Port nimmt die echte Cut-0-Zone @0x80121bd8. Fuer die Gorillas ergebnisgleich (0/0).
- H3: Kleiner Affe 0x29 mit grid&0x20 traegt jetzt den Quad auf +0x1ba wie der STAGE3-Schwanz; 0x29-Szenen mit geparkten
  Affen nicht an der exe gemessen (kein Nutzer-Raum dieser Spur; integration_r38_affen3* gruen).
- H4: RE1.5-KI-Flavor fuer Hund/Kriecher nur in Unit-Riegeln gemessen; Exe-Laeufe im RE2-Flavor (Nutzer-Standard).
- H5: Entscheidungsfrage des Bau-Agenten (HP x2 / 3-Treffer-Schwelle auch fuer ROOM1141?) bleibt beim Nutzer; Umsetzung folgt
  dem Wortlaut "beim Parking Lot" (= 11C0; Elzas 11C1 hat Hunde, keine Gorillas).
- H6 (nicht diese Spur, vorbestehender Debug-Sprung-Harness): nach `RE15_DEBUG_JUMP=11C0` laeuft die Kamera 0 -> 1 -> 2 -> 3
  -> 4 -> 5 -> 13, das Original (Savestates) 0 -> 1 -> 3 -> 4 -> 13; im Cut-5-Fenster F184-F212 liegen die geparkten Quads
  links ausserhalb des Bildes (Messung des Bau-Agenten x = -692..-128), nichts sichtbar. Punkte 1 und 4 wurden ueber den
  Debug-Sprung gemessen (Szene bzw. Kampf-Layout per Flag 4/0x40), nicht ueber die Tuer 11B0 -> 11C0; Mechanismen (Treffer-
  Schwelle, Quad-Hoehe) sind vom Eintrittsweg unabhaengig. Punkt 3 ueber die echte Tuer (1020 -> 1010, 1021 -> 1011).
- H7: Skript-Kurzform `(…)xN` ist keine Wiederholung (Parser liest `x` = Kreuz); in Berichten die Folge ausschreiben.

## 7. Maengel
Keine.

## 8. Ergebnis
- Punkte 1-4: **erfuellt** (jeweils selbst an der neu gebauten exe gemessen; Punkt 2 zusaetzlich als Spielszene vorher/nachher).
- Gates: Vertrag/Pfade/Tests OK; RE-Gate OK — die in Nachbesserung 1 neu zitierten Stellen (Wurzel-Schwanz auf jedem Pfad,
  Region-Cull, Punkt-im-Viereck, Zonen-Lookup, Cut-Apply, Sce_em_set, INIT-Ecken) enthalten das Behauptete; M1 ist behoben.
- **bestanden = true.**

## 9. Pruefprotokoll (Werkzeuge)
- Disasm: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (STAGE1.BIN / STAGE3.BIN / PSX.EXE). Savestates:
  `scratchpad/e_nb1/zone_read.py` (gelesen, dann selbst ausgefuehrt) mit `re15-savestate-ghidra/scripts/re15_ss.py` auf
  `scratchpad/orig_scene/r3/s0xx_*.sav`.
- Exe-Laeufe: Messkopien `re15_pc_abn1_{p4_11c0,p1_11c0,p1b,p3a_1010,p3b_1220,p3c_1221,p3d_1011,p2n,p2n2}.exe` (nach der
  Abnahme geloescht); vorher-Referenz `release/pkg-win/re15_port_v0.8.26/re15_pc.exe` nur gelesen/gestartet (Logs im Scratchpad).
  Arbeitsordner `scratchpad/abn1/*`, Kontaktbogen `abn1/p4_11c0/sheet_*.png`.
- ctest: Unit-Gruppen -j4 (85/85), Exe-Integrationen -j1 (9/9); keine volle Suite im Spurbaum.
