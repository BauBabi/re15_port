# Runde 38 — Spur B "szenenschutz": UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme-Agent 0 (2026-10-16). Baum `.claude/worktrees/r38_szenenschutz`, Zweig `r38/szenenschutz`,
HEAD 8d778c67, Basis e2cc7673 (= Code-Stand v0.8.25; `git log 2077bb8e..e2cc7673 -- re15_port/engine re15_port/platform
re15_port/include` ist leer). Alles hier Gemessene ist SELBST gemessen; Aussagen des Bau-Agenten sind als solche
gekennzeichnet. (Bericht wird fortlaufend geschrieben.)

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
(11D0: `RE15_PLAYER_POS` + `RE15_FIRE_AOT`) aus. Ich habe beide Szenen ueber ECHTE Eingaben ausgeloest: Raum betreten
(Debug-Sprung = Ankunft am Tuer-Spawn der Raumtabelle, main.c `re15_room_spawns[]`), hinlaufen (RE15_INPUT_SCRIPT,
Tankstrecke kalibriert: 96 Rot-Einheiten/Bild, Gehen 74 Einheiten/Bild), Knopf/Gegenstand mit Quadrat bedienen,
Abfrage "Yes" bestaetigen. Gleiches Skript mit der v0.8.25-exe als "vorher".

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
Ursache (Code gelesen): `live_run` (probe_sca_wall_hit.c:286-330) setzt `pl->state = 0` und ruft dann
`scd_room_reenter` — das ROOM11C0-Skript fuehrt den Spieler per `Plc_dest(slot=0 mode=0x09 ...)` (Log-Zeile), Spieler+0x4
wird 4, `re15_szenenschutz_haelt()` = 1, der Gorilla/Maggot 0x27 sieht den Spieler waehrend aller 240+240 Bilder als
"gerade getroffen" -> keine CHASE-/REAR-UP-Kette mehr. Die Positiv-Kontrolle des fremden Riegels ist damit tot -> M1.
Lauf B: die uebrigen 405 Nicht-Integrationstests: 403/403 bestanden (Ergebnis der zwei Android-Skript-Riegel s. unten).

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

### 4.3 1171 unveraendert (s. 2.4), Elza-Varianten 1191/11D1 wie Leon (s. 2.2/2.5).

(weitere Abschnitte folgen)
