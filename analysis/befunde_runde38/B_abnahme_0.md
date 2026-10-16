# Runde 38 — Spur B "szenenschutz": UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme-Agent 0 (2026-10-16). Baum `.claude/worktrees/r38_szenenschutz`, Zweig `r38/szenenschutz`,
HEAD 8d778c67, Basis e2cc7673 (= Code-Stand v0.8.25; `git log 2077bb8e..e2cc7673 -- re15_port/engine re15_port/platform
re15_port/include` ist leer). Alles hier Gemessene ist SELBST gemessen; Aussagen des Bau-Agenten sind als solche
gekennzeichnet. Endstand des Berichts: 2026-10-16 ~23:30.

## 0. Stand / Bau

- `git diff master...HEAD --name-only` (nur Spur-Aenderungen; master ist seit e2cc7673 nur um Doku weitergelaufen):
  15 Dateien — Dossier + 3 Belege, `engine/src/szenenschutz_r38.c` (neu), `include/re15_szenenschutz_r38.h` (neu),
  Haken in `enemy_ai_common.c`, `enemy_ai_re2_crow.c`, `enemy_ai_re2_dog.c`, Tests
  `tests/unit/test_r38_szenenschutz{,_alle,_arm}.c`, `tests/unit/probes/r38_szenenschutz.cmake`,
  `tests/integration/test_r38_szenenschutz.cmake`, Anpassung des fremden Riegels
  `tests/integration/test_r36_kleidung_hundeweg.cmake`.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (exe
  21:36:10, juenger als alle Engine-Quellen; letzte Engine-Aenderung 2db050bf, 019cbff9 nur Kommentar im Kopf).
- Messkopien (eigener Name, neben der exe): `re15_pc_abn0b_neu.exe` (= re15_pc.exe, md5 a4d031d1...) und
  `re15_pc_abn0b_v0825.exe` (= `release/pkg-win/re15_port_v0.8.25/re15_pc.exe`, Vergleich "vorher").
  Laeufe unter `scratchpad/abn/<name>` (Werkzeuge `lauf.sh`, `auswerten.py`, `blatt.py` im Scratchpad).

## 1. Tests der Spur (selbst gefahren)

`ctest --test-dir re15_port/build -R "r38_szenenschutz" --timeout 1200 --output-on-failure -V`:
`100% tests passed, 0 tests failed out of 4` (unit_r38_szenenschutz 0.18 s, _alle 0.53 s, _arm 0.09 s,
integration_r38_szenenschutz 393.6 s). Statuszeilen der Integration (identisch mit dem Dossier):
`[1190] Szene F521..F738 hp-Verlust=0 Angriffs-Bilder=0 erster Biss F805; [szenenschutz] F739 aus (94 geschuetzte
Gegner-Ticks)`, `[11D0] Szene F331..F524 hp-Verlust=0 Angriffs-Bilder=0 erster Biss F550`, `[1171] Szenen-Kraehe greift
bei F101 (hp 92, pm=2)`, `[1171s]: kein geschuetzter Akteur in ROOM1171`.

## 2. Nutzer-Punkt, selbst an der exe gemessen — UEBER DEN ECHTEN WEG

Die Riegel der Spur loesen die Szenen per Flag (1190: `RE15_SET_FLAG_AT=4:243,5:4,5:5`) bzw. per Teleport + AOT-Zuendung
(11D0: `RE15_PLAYER_POS` + `RE15_FIRE_AOT`) aus. Ich habe beide Szenen ueber ECHTE Eingaben ausgeloest: hinlaufen
(RE15_INPUT_SCRIPT, Tankstrecke kalibriert: 96 Rot-Einheiten/Bild, Gehen 74 Einheiten/Bild), Knopf/Gegenstand mit
Quadrat bedienen, Abfrage "Yes" bzw. Aufnahme-Modal bestaetigen. 11D0 zusaetzlich mit echtem TUER-Eintritt aus ROOM1180
(2.3). ROOM1190 betrete ich per Original-Debug-Menue-Sprung (`[debug-menu] JUMP -> 119 FIRING-ARMORY (ROOM1190)
spawn=(3050,0,-11650) cut=0`, ~480 Einheiten neben der Tuer-Ankunft (3174,-11182) aus ROOM1180 Tuer 2 @0x00A54); die
Zugangstuer mit Strom liegt in ROOM1230 rund 25000 Einheiten hinter zwei freien Hunden. Die Szene selbst ist in 1190
echt ausgeloest (zwei Knoepfe). Gleiches Skript mit der v0.8.25-exe als "vorher".

### 2.1 ROOM1190 (Leon, Standard-KI), echter Hebel-Weg
Befehl (Lauf e8 = neu, v8 = v0.8.25): `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=1190@240
RE15_SET_FLAG=4:243` (Strom an, Vorbedingung von sub01 @0x0237C), `RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT=L0.7,U2.05,L0.36667,U5.8,R0.36667,U1.0,W0.3,A0.1,W2,A0.1,W5,R0.7,U0.9,
L0.36667,U3.25,L0.33333,U1.0,W0.3,A0.1,W2,A0.1,W12`, `RE15_STATE_LOG=state.log RE15_EVT_TRACE=1 RE15_FLAG_TRACE=1`.
debug.log (neu): `[evt] F618 room=1190 Evt_exec sub=2` (Knopf 0, "There's a switch here. Push it?") /
`[evt] F1040 ... sub=4` (Knopf 2) / `[evt] F1214 ... sub=10` (Szene, aus sub01 Kombination A @0x02438-0x02468) /
`[szenenschutz] F1214 an` / `[szenenschutz] F1432 aus (94 geschuetzte Gegner-Ticks)`.

| | Szene pm=2 | HP-Verlust IN der Szene | Hund st1/ss1=3 IN der Szene | erster Biss |
|---|---|---|---|---|
| v0.8.25 (v8) | F1214..F1431 | **F1401 hp 100->80 (pm=2)** | Slot1 F1387-F1421 (35 B.), Slot2 F1419-F1431 (13 B.) | F1401 (in der Szene); danach F1445/1510/1579/1589, tot F1669 |
| neu (e8) | F1214..F1431 | **0** | **0** | F1562 = 131 Bilder nach Szenenende (Commits Slot1 F1530 = Landung F1368+162, Slot3 F1553 = Landung F1432+121) |

Bild: `scratchpad/abn/blatt_v0825_1190.png` / `blatt_neu_1190.png` (Framedump F1340..F1570, RE15_WINDOW_SCALE=1): die
Szenen-Kamera zeigt den Lueftungsschacht (Spruenge F1390/F1420), Leon ist NICHT im Bild — in v0.8.25 liegt beim
Schnitt auf Cut 2 (F1440) schon Blut unter Leon; neu steht er unverletzt, die Hunde umkreisen ihn bis F1562.
=> Der Nutzer-Befund "in der Szene angegriffen" ist am echten Weg mit v0.8.25 reproduziert (Biss F1401 bei pm=2) und
mit dem neuen Stand weg.

### 2.2 ROOM1191 (Elza, Standard-KI), echter Hebel-Weg (Kombination B: Knopf 3 + Knopf 1)
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_SET_FLAG=4:243,3:112
RE15_DEBUG_JUMP=1190@250`, Skript `L0.7,U2.05,L0.36667,U0.87,R0.36667,U1.0,W0.3,A0.1,W2,A0.1,W5,R0.7,U0.9,R0.36667,
U3.33,R0.33333,U1.0,W0.3,A0.1,W2,A0.1,W12` (Lauf el1, neu): `[pl] Spieler-Familie PL04`, `loaded room1191.rdt`,
`[evt] F490 sub=5`, `[evt] F914 sub=3`, `[evt] F1088 sub=10`, Szene F1088..F1305: HP-Verlust 0, Angriffs-Bilder 0;
erster Biss F1436 (131 Bilder nach Szenenende).

### 2.3 ROOM11D0 (Leon, Standard-KI), echter Aufnahme-Weg
`RE15_DEBUG_JUMP=11d0@240`, Skript `R0.36667,U1.6,R0.33333,U2.8,R0.36667,U3.95,L0.36667,U2.0,W0.3,L0.36667,U0.4,W0.3,
A0.1,W1.6` + 8x `A0.1,W0.23333` (Modal bestaetigen) `,W12`: Leon laeuft durch die offene Tuer in den Gegenstands-Kaefig
(-10982,-12181), Quadrat -> `[szenenschutz] F696 an` (sub03), `[evt] F797 sub=2` (Kaefig-Freigabe), `[szenenschutz]
F871 aus (365 geschuetzte Gegner-Ticks)`.

| | Szene pm=2 | HP-Verlust IN der Szene | Hund st1/ss1=3 IN der Szene | erster Biss |
|---|---|---|---|---|
| v0.8.25 (v12) | F696..F870 | 0 | **Slot4 F865-F870 (6 Bilder, Ansprung in der Szene)** | F919 |
| neu (e12) | F696..F870 | 0 | **0** | F890 (Commit Slot4 F881 = 11 Bilder nach Szenenende) |

**Mit echtem Tuer-Eintritt** (Lauf t1 = neu, t1v = v0.8.25): `RE15_DEBUG_JUMP=1180@30 RE15_SET_FLAG=4:243` (Strom an:
in ROOM1180 fuehrt Tuer 0 dann nach 11D0, main00 @0x00A12), im Gang `RE15_PRESS=right@40..50,square@53,square@54`
(zur Tuer drehen, Tuer oeffnen) -> `[tuer] Sequenz Archiv 2 DOOR1A ...` -> `loaded room11d0.rdt`, Ankunft
`F1 PL(-300,-17400,rot=1024)`; in 11D0 Skript `L0.36667,U1.6,R0.36667,U2.8,R0.36667,U3.7667,L0.36667,U2.6,W0.3,
L0.36667,U0.4,W0.3,A0.1,W1.6` + 8x `A0.1,W0.23333` `,W12` (Ankunft `F700 PL(-10983,-12182)` im Kaefig):

| | Szene pm=2 | HP-Verlust IN der Szene | Hund st1/ss1=3 IN der Szene | erster Biss |
|---|---|---|---|---|
| v0.8.25 (t1v) | F709..F883 | 0 | **Slot4 F878-F883 (6 Bilder)** | F932 |
| neu (t1) | F709..F883 | 0 | **0** | F903 (Commit Slot4 F894) |

Bild 11D0 (Debug-Sprung-Lauf, gleiche Bahn): `scratchpad/abn/blatt_v0825_11d0.png` zeigt bei F880 den Hund im Ansprung
auf Leon (in der Szene begonnen); `blatt_neu_11d0.png` den laufenden Hund, Blut ab F900.

Leon ist ab F871 frei (pst=1, mo=200, kein Plc-Zustand mehr). Hinweis (kein Mangel, s. 6): am echten Weg kommt der
erste Biss NACH der Szene mit dem neuen Stand frueher (20 Bilder nach Szenenende) als mit v0.8.25 (49 Bilder) — der in
der Szene begonnene Ansprung von v0.8.25 ging ins Leere, jetzt laeuft der Hund weiter und setzt nach der Freigabe an.
11D0 hat im Original KEINE Sperrzeit (Freigabe FUN_80111658 -> ACT[12], Dossier 2.2).

### 2.4 Ausnahme "Teil der Cutscene" — Elza ROOM1171
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_SET_FLAG=3:194 RE15_DEBUG_JUMP=1170@250
RE15_EXIT_AT=300` je KI (Standard / `RE15_AI_FLAVOR=re15`), neu gegen v0.8.25: state.log Segment 1 (300 Bilder)
**bytegleich** in beiden KI-Modi (`cmp` ohne Abweichung; md5 094e1bff... Standard, ad4600f9... RE1.5).
RE1.5-KI: Szenen-Kraehe greift Elza `F101 ... hp=92) pst=4 ... pm=2 gr=1` (200 Bilder gr=1) — der Szenen-Angriff
bleibt. Standard-KI: 0 Bilder gr=1, wie v0.8.25 (das ist der Haenger der Spur I). Spur I (r38_heliport1171, nur
gelesen) fuehrt ihre Ereignis-Kraehe auch unter der Standard-KI in FLIGHT-2 (+0x4=4, grid 0x40,
`re15_r38i_ereigniskraehe`) — vom Szenenschutz ausgenommen (`e->state != 1`, `grid&0x40`).

### 2.5 ROOM11D1 (Elza, Standard-KI), echter Aufnahme-Weg
Gleiches Skript wie 2.3 mit Elza-Wahl (Lauf el2): `loaded room11d1.rdt`, `[szenenschutz] F716 an`, `[evt] F817 sub=2`,
`[szenenschutz] F891 aus (365 geschuetzte Gegner-Ticks)`; Szene F716..F890: HP-Verlust 0, Angriffs-Bilder 0; erster
Biss F910 (20 Bilder nach Szenenende).

## 3. RE-Gate (selbst disassembliert, re15_disasm.py / re2_disasm.py, richtige Binaerdatei)

| Behauptung (Code/Dossier) | Binaerdatei @Adresse | gelesen |
|---|---|---|
| RE1.5-Hund ATTACK-RANGE: Sperrzeit-Tor | STAGE1.BIN @0x8010e578/80 | `lh v0,470(v0)` (+0x1d6) / `bne v0,zero,0x8010e628` — stimmt |
| ... Spieler-Tor +0x93 | STAGE1.BIN @0x8010e5c4/cc | `lbu v0,-13593(v0)` (0x800acae7) / `beq v0,zero,0x8010e6c4`, sonst +5=6 (Knurren), hp<50 -> 5 — stimmt |
| 1190-Freigabe schreibt 0x78 | STAGE1.BIN @0x801115e8/ec | `ori v0,zero,0x78` / `sh v0,470(v1)`; @0x8011161c `sb zero,9(v0)`; @0x80111628/2c `ori 0x201`/`sw v0,4(v1)` — stimmt |
| Abzug im ACTIVE-Schwanz | STAGE1.BIN @0x8010dd5c-70 | `lh`/`beq`/`addiu -1`/`sh 470(a0)`; liegt in FUN_8010dbcc (Prolog @0x8010dbcc, Dispatch-jalr @0x8010dcbc/@0x8010dcf0, kein `jr ra` davor); Zustands-Tabelle @0x80120f74 [1] = 0x8010dbcc, [4..6] = 0x80111350 — stimmt |
| Gegner-Wurzel prueft nur 0x20000000 | STAGE1.BIN @0x8010d80c-18 | `lw v0,0(s0)` (0x800aca40) / `lui v1,0x2000` / `and` / `bne` — stimmt |
| RE1.5-Kraehe Commit ohne Spieler-Tor | STAGE1.BIN @0x8011330c-70 | +0x1d2/+0x1db/+0x1dc<0x2710/+0x1d6<3/grid<0x80 -> a0 0xc/0xa, `jal 0x80115d74` — kein Zugriff auf 0x800acae7 — stimmt |
| Griff-/Strike-Kontakt | STAGE1.BIN @0x80113dcc-dd4, @0x80114454-64 | `lbu 464` / `beq` ; `jal 0x80115d74` a0=0x10, danach pos_advance @0x8011446c — stimmt |
| Kraehen-INIT grid&0x40 -> +0x4=4 | STAGE1.BIN @0x80112400-0c | `andi 0x40` / `beq` / `ori v0,zero,0x4` / `sw v0,4(v1)` — stimmt (die 0x40 steht @0x80112400) |
| RE2-Kraehe Arbiter nur HP-Tor | EMOVL21_S0.BIN @0x801041f0-04 | `lh v0,-690(v0)` (0x800cfd4e) / `bltz` ; Grab-Join `and 0xa0000` -> `jal 0x80104088` a1=13 — stimmt |
| RE2-Kraehe Abbruch-Listener | EMOVL21_S0.BIN @0x80104400-64 | +0x22A&0x10 -> Vol 350, rand&0x9d -> +0x220, Sub 4, +0x21C=30, +0x22A&=0xffa7 — stimmt |
| RE2-Hund Entscheid/Biss-Kontakt PL+0x1D3 | EMD0G_MOD0.BIN @0x80104d84-90, @0x80104e5c-68 | `lbu 467(s1)` / `andi 0x80` / `bne` -> -4 bzw. 0 — stimmt |
| Set(2,7) liest die Pad-Aufbereitung | PSX.EXE @0x800304f4-14 | `lw aca40` / `lui 0x100` / `and` / `beq` / `andi 0xf000` — stimmt |
| Plc_motion / Plc_dest / Plc_ret | PSX.EXE @0x80041ba4/bb0, @0x80041c10/14, @0x80041f8c/90 | `ori 4`/`sb 4(v0)`; `ori 4`/`sb 4(a1)`; `ori 1`/`sb 4(v0)` — stimmt |
| Schadens-Aufloeser +0x93 Bit 0 | PSX.EXE @0x80012e24-30 | `lbu hit_react` / `andi 0x1` / `bne` — stimmt |

Diff-Suche nach "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|vermutlich|ungemessen": 0 Treffer in
Code und Dossier; kein neuer `getenv`. Konstanten im Code: 0x78 (@0x801115e8), Zustand 1/4 (@0x80041bb0/@0x80041c14,
Wurzeltabelle @0x80120f74), Bit 0x10 (@0x80104418), Sub 11/13 (@0x80101F2C/@0x80102444), Bit 0x80 (@0x80104D8C), grid
0x40 (@0x80112400-08) — belegt. **Ausnahme:** die Typgrenze `e->type >= 0x40` in `re15_szenenschutz_gegen`
(szenenschutz_r38.c) traegt KEIN @-Zitat (Kommentar "NPC-Typen (0x40..0x4d) greifen den Spieler nie an"); der Beleg
existiert im Original (`sltiu v0,v0,0x40` @0x80050a78, selbst disassembliert; im Port zitiert enemy_ai_common.c:10140),
wird an der neuen Stelle aber nicht zitiert -> Mangel M3 (formal).

Erklaert der Fix den Befund? Ja: (a) vorher liefen die freien Gegner-Gehirne waehrend pm=2 ungebremst (gemessen: Hund
st1/3 in der Szene, Biss F1401 bei pm=2), jetzt sehen sie in pm=2 die Original-Tore geschlossen; (b) der RE2-Hund las
die 1190-Sperrzeit nie (Port schrieb dog_atk_cd=0x78 in enemy_ai_common.c:7595, RE2-Modul las es nicht) — jetzt erste
Commits >= 120 Bilder nach der Landung (gemessen e8: +162/+121).

## 4. Nebenwirkungen (selbst gemessen)

### 4.1 Unit-Tests: ROT — `unit_sca_wall_hit` (Riegel einer fremden Spur) faellt durch Spur B
Lauf A: die 219er-Liste des Bau-Agenten (`B_belege/unit_regex.txt`) + 48 weitere KI-/Treffer-/Raum-Riegel
(1090-Feuer, 1210-Arme, unit_hit_reaction, r31_trefferboxen, unit_sca_wall_hit, Elza-Riegel …) = 267 Tests, `-j 4`:
`99% tests passed, 1 tests failed out of 267` — `209 - unit_sca_wall_hit (Failed)`. Einzeln nachgefahren: reproduzierbar
(0.15 s, kein Fenster-Haken, keine Last-Flatterei):
`FAIL: C1/FREI: REAR-UP feuerte NIE — der Positiv-Zweig ist unerreichbar, der WAND-Nullwert waere dann kein Beweis`
(`[C1] ROOM11C0 Maggot 0x27 ... FREI ... Verbraucher=0 (max 0)`), im selben Lauf
`[szenenschutz] F0 an (Szene haelt den Spieler)` ... `[szenenschutz] F0 aus (960 geschuetzte Gegner-Ticks)`.
Gegenprobe OHNE Spur B (dieselbe Test-exe aus Baeumen auf derselben Basis, nur gelesen/ausgefuehrt, Arbeitsordner im
Scratchpad): `r38_cursor/re15_port/build/tests/unit/probe_sca_wall_hit.exe` -> `OK (0 Fehler)`, C1 FREI
`Verbraucher=237`; `r38_integration/.../probe_sca_wall_hit.exe` -> `OK (0 Fehler)`.
Ursache (Code gelesen): `live_run` setzt `pl->state = 0` (probe_sca_wall_hit.c:297) und ruft dann
`scd_room_reenter` (:306) — das ROOM11C0-Skript startet seine Eingangsszene (sub01 @0x01824 Evt_exec sub2) und fuehrt
den Spieler per `Plc_dest(slot=0 mode=0x09 ...)` (Log-Zeile); ohne main.c bleibt player_mode 0, aber Spieler+0x4 wird
4, `re15_szenenschutz_haelt()` = 1, der Gorilla/Maggot 0x27 sieht den Spieler waehrend aller 240+240 Bilder als
"gerade getroffen" -> keine CHASE-/REAR-UP-Kette mehr. Die Positiv-Kontrolle des fremden Riegels ist damit tot -> M1.
Lauf B: die uebrigen 405 Nicht-Integrationstests (`-j 4`, Begruendung: der Haken sitzt im globalen Gegner-Durchlauf,
jeder Engine-Riegel mit Gegner-Tick ist betroffen): `99% tests passed, 1 tests failed out of 405` — einzig
`unit_r35_android_bash_mutanten` (Android-Bash-Skripte, nicht der Spur zugerechnet, s. H6); alle Engine-Riegel gruen.

### 4.2 "Spieler+0x4 == 4" haelt auch Bilder, in denen Leon sich im Port BEWEGT
Dossier 2.4 begruendet die zweite Halte-Bedingung mit "auch ohne (2,7) kann sich Leon nicht bewegen" (Beispiel ROOM1230
sub21). Gemessen an der exe (Lauf e14): `RE15_DEBUG_JUMP=1230@240 RE15_SET_FLAG=4:243 RE15_PLAYER_POS=3900,25000,3072
RE15_INPUT_SCRIPT=A0.1,U4,W2` (Quadrat an AOT 18 = sub21, dann Steuerkreuz hoch): `[szenenschutz] F301 an`, state.log
`F304 ... PL(3900,25150,...) pst=4` / `F312 PL(3900,25750) pst=4` / `F320 PL(3900,26350) pst=4` / `F336 PL(3900,27100)
pst=4` — Leon LAEUFT ~35 Bilder lang 2100 Einheiten (bis zur Wand), waehrend er im Plc-Zustand 4 steht und der
Szenenschutz an ist (pm=0; das Port-Bewegungstor player_common.c:946-947 prueft nur player_mode/msg_block, nicht +0x4).
Erst die Nachricht (msg_a=1 ab ~F336) sperrt die Bewegung (Kontrolllauf e15 ohne Steuerkreuz: z bleibt 25000).
Folge: in diesen Fenstern ist Leon beweglich UND fuer freie Gegner unangreifbar — das widerspricht der Halte-Definition
("wo er sich noch nicht mal bewegen kann") und der Dossier-Aussage -> M2.
Gegenprobe v0.8.25 (Lauf v14, gleiches Skript): dieselbe Bahn (F304 z=25150 ... F336 z=27100) — das Laufen in pst=4 ist
eine VORBESTEHENDE Port-Abweichung (das Original dispatcht bei +0x4=4 den Executor @0x80073f90[4] statt des Pad-Zweigs,
Dossier 2.4); Spur B legt den Schutz darueber, ohne das zu pruefen.

### 4.3 1171 unveraendert (s. 2.4), Elza-Varianten 1191/11D1 wie Leon (s. 2.2/2.5).

### 4.4 Integration (exe) der beruehrten Szenen/Gegner — selbst gefahren
`ctest -R '^(integration_r35_zgirl|integration_r35_zgirl_5090|integration_r36_kleidung_hundeweg|integration_r36_story_leon|
integration_r36_story_elza|integration_r36_szene1150|integration_r36_szenenleon|integration_r37_bissarm_echtlauf|
integration_r37_elzaems_leon5080|integration_r37_elzaems_leon1100|integration_r37_elzaems_elza10c1kraehe|
integration_r37_elzaems_leon10c0kraehe|integration_r37_elzaems_elza1141|integration_r37_feuerloescher|
integration_r37_roy_szene1011|integration_r37_roy_szene1021|integration_r37_roy_leon1170|integration_r37_szene11c0|
integration_r37_zombieaktiv|integration_r35_fenster|integration_r36_elza_fenster|integration_r36_ziel1190_scheiben)$'
-j 3 --timeout 1500` (Auswahl: Szenen mit Gegnern in beiden Szenarien, Kraehen 1120/1170/10C0, Hunde 1190, Gitterarm,
Feuer 1090, Montage-Zombies, Endkampf 5090) — Ergebnis: `100% tests passed, 0 tests failed out of 22` (u.a.
integration_r36_kleidung_hundeweg 83 s, integration_r37_roy_leon1170 93 s, integration_r37_elzaems_*kraehe 60/82 s,
integration_r37_feuerloescher 393 s, integration_r35_zgirl_5090 250 s, integration_r37_zombieaktiv 43 s), unter
Volllast der Maschine (mehrere Spur-Agenten parallel), ohne Wiederholung.

## 5. Vertrag

- Pfade: nur eigene Dateien + kleine Haken (enemy_ai_common.c +11/-4 in 5 Hunks, je 1-2 Zeilen mit "Runde 38 Spur B";
  enemy_ai_re2_crow.c +3; enemy_ai_re2_dog.c +7/-2). Keine `release/`, `platform/android/`, `shared_assets/PSX/`,
  keine Test-CMakeLists (Registrierung ueber `tests/unit/probes/r38_szenenschutz.cmake`). Keine Bank-9-Bits,
  Nachrichten-Ids, Items, Sprachdateien.
- Fremder Riegel `test_r36_kleidung_hundeweg.cmake` angepasst (Save-Bild 600->700, L4->R4, Kasten/Schwelle 13->25 bei
  gemessenen 50 Pixeln), im Dossier 3 und im Kopf des Riegels begruendet; der Riegel laeuft gruen (4.4).
- Ein weiterer fremder Riegel (`unit_sca_wall_hit`) wurde von der Aenderung getroffen und NICHT angepasst/gefahren
  (M1); die 219er-Liste des Bau-Agenten ("jede Testgruppe von enemy_ai_common.c") enthielt ihn nicht, obwohl er Gegner
  ueber `frame_step` -> `re15_enemy_ai_run_all` tickt.
- Riegel je Punkt: vorhanden und messend (Negativkontrollen im Code und im Dossier dokumentiert).

## 6. Hinweise (keine Maengel)

- H1 11D0/11D1: Am echten Weg (auch mit Tuer-Eintritt, t1/t1v) setzt der erste Hund 11 Bilder nach der Freigabe an
  (Biss 20 Bilder danach; v0.8.25: Ansprung schon in der Szene, Biss erst 49 Bilder danach). Der Wortlaut ("in Cutscenes ... wo er sich noch nicht mal
  bewegen kann") ist erfuellt; eine Schonfrist NACH der Szene gibt es in 11D0 im Original nicht (Freigabe FUN_80111658
  ohne Sperrzeit, Dossier 2.2). Wuenscht der Nutzer eine Schonfrist, waere das eine neue Vorgabe.
- H2 RE1.5-KI (nicht Standard): Die Sicht "+0x93 gesetzt" fuehrt den RE1.5-Hund in seine Knurr-/Aushol-Reaktion
  (Dossier 6) — dokumentiert, Treffer gibt es nicht.
- H3 Feuer 0x26: Das Sicherheitsnetz gibt den Feuer-Abzug zurueck, solange die Szene haelt; das Feuer ist kein Angriff
  — bewusste Erweiterung (Dossier 6), in den gefahrenen Szenen nicht beobachtbar.
- H4 Integration mit Spur I: beide Spuren aendern enemy_ai_common.c in verschiedenen Hunks; die Ereignis-Kraehe der
  Spur I bleibt durch `state != 1`/`grid&0x40` ausgenommen (gelesen in r38_heliport1171).
- H5 ROOM11C0 (Gorilla-Raum, Gegenprobe zu M1 im Spiel): Debug-Sprung, 1100 Bilder, neu und v0.8.25 identische
  Zeitleiste `F6 pst=4 pm=2` (Szene sub02) -> `F1071 pst=1 pm=1` -> `F1085 pm=0`, kein HP-Verlust; neu
  `[szenenschutz] F1071 aus (2130 geschuetzte Gegner-Ticks)` = genau das Szenenfenster. Im Spiel ist M1 also die
  gewollte Wirkung waehrend der Eingangsszene — rot ist der fremde Riegel, dessen Aufbau die Szene nicht beendet.
- H6 `unit_r35_android_bash_mutanten` lief in Lauf B rot (`2 veraltete Eintraege`, 1471 s unter Volllast). Spur B
  aendert nichts unter `release/`, `platform/android/`, `tools/` (`git diff e2cc7673 --stat` leer) — Testgegenstand sind
  Android-Bash-Skripte; nicht der Spur zugerechnet.

## 7. Urteil

| Punkt | Urteil | Beleg |
|---|---|---|
| "Leon wird in Cutscenes, in denen er sich nicht bewegen kann, nicht angegriffen (1190 Hunde, 11D0 Hunde); Ausnahme: Teil der Cutscene wie Elza 1170" | erfuellt | Echter Weg 1190: v0.8.25 Biss F1401 bei pm=2 -> neu 0 HP-Verlust, 0 Angriffs-Bilder in F1214..F1431 (e8/v8, Bildblaetter). 11D0 mit Tuer-Eintritt aus 1180: v0.8.25 Ansprung F878-F883 in der Szene -> neu 0 (t1/t1v; ebenso e12/v12). Elza 1191/11D1 ebenso (el1/el2). 1171: state.log bytegleich zu v0.8.25 in beiden KI-Modi, Szenen-Kraehe greift Elza F101 (pm=2). Spur-Riegel 4/4 gruen. |

Gates: Spur-Riegel gruen; RE-Gate haelt bis auf M3 (formal); Vertrag haelt; **Testsuite NICHT gruen (M1)**; Halte-
Definition ueberdeckt bewegliche Bilder (M2).

## 8. Maengel (nummeriert, nachpruefbar)

- **M1 (hoch) — fremder Riegel `unit_sca_wall_hit` rot durch Spur B.** `ctest --test-dir re15_port/build -R
  "^unit_sca_wall_hit$" --output-on-failure` -> `FAIL: C1/FREI: REAR-UP feuerte NIE` (C1 FREI `Verbraucher=0`), dazu
  `[szenenschutz] F0 an` / `F0 aus (960 geschuetzte Gegner-Ticks)`. Ohne Spur B (probe_sca_wall_hit.exe aus
  r38_cursor bzw. r38_integration): `OK (0 Fehler)`, C1 FREI `Verbraucher=237`. Ursache: der Aufbau
  (`probe_sca_wall_hit.c` live_run: `pl->state = 0`, dann `scd_room_reenter`) startet die ROOM11C0-Eingangsszene
  (sub01 @0x01824 Evt_exec sub2, Plc_dest -> Spieler+0x4 = 4) und tickt den Maggot 0x27 waehrend des Halte-Fensters.
  Noetig: den fremden Riegel begruendet anpassen (wie bei test_r36_kleidung_hundeweg) und die Riegel-Auswahl auf ALLE
  Tests erweitern, die Gegner ueber `frame_step`/`re15_enemy_ai_run_all` ticken (die 219er-Liste traf ihn nicht).
- **M2 (mittel) — zweite Halte-Bedingung "Spieler+0x4 == 4" schuetzt auch, waehrend Leon sich im Port bewegt.**
  Lauf e14 (ROOM1230 sub21, Quadrat an AOT 18 + Steuerkreuz): `[szenenschutz] F301 an`, pst=4, Leon laeuft F303-F336
  von z=25000 nach z=27100 (pm=0; Bewegungstor player_common.c:946-947 prueft +0x4 nicht; in v0.8.25 gleiche Bahn =
  vorbestehend). Dossier 2.4 ("auch dort kann sich Leon nicht bewegen") trifft fuer den Port nicht zu; der Schutz macht
  Leon in diesen Fenstern beweglich UND unangreifbar. Noetig: Halte-Bedingung auf Bilder beschraenken, in denen der Port
  das Pad wirklich ignoriert (oder das Bewegungstor byte-true an +0x4 binden: Dispatcher @0x80031c8c `lbu aca58` ->
  Tabelle @0x80073f90, [1] = 0x80031de8 Pad-Zweig, [4] = 0x80030660 Executor — selbst disassembliert) — mit Messung.
- **M3 (gering, formal) — Konstante ohne @-Beleg:** `if (e->type >= 0x40) return 0;` in `re15_szenenschutz_gegen`
  (engine/src/szenenschutz_r38.c) zitiert keine Adresse; Beleg vorhanden: PSX.EXE @0x80050a78 `sltiu v0,v0,0x40`
  (selbst disassembliert; im Port bei enemy_ai_common.c:10140 zitiert). Zitat ergaenzen.

bestanden = false (M1, M2, M3).
