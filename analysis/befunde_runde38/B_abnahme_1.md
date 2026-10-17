# Runde 38 — Spur B "szenenschutz": UNABHAENGIGE ABNAHME 1

Pruefer: Abnahme-Agent 1 (2026-10-17). Baum `.claude/worktrees/r38_szenenschutz`, Zweig `r38/szenenschutz`,
HEAD c2aeab86, Basis e2cc7673. Alles hier Gemessene ist SELBST gemessen; Aussagen des Bau-Agenten sind als solche
gekennzeichnet. Der Bericht wird fortlaufend geschrieben (Sitzungslimit).

## 0. Stand / Bau

- `git diff master...HEAD --name-only` (nur Spur-Aenderungen; `git diff master` zeigt zusaetzlich Doku-Dateien, die
  master seit e2cc7673 neu hat — keine Spur-Aenderung): 22 Dateien — Dossier, Abnahme 0, 8 Belege unter `B_belege/`,
  `engine/src/szenenschutz_r38.c` (neu), `include/re15_szenenschutz_r38.h` (neu), Haken in `enemy_ai_common.c`,
  `enemy_ai_re2_crow.c`, `enemy_ai_re2_dog.c`, `player_common.c`, Tests `tests/unit/test_r38_szenenschutz{,_alle,_arm}.c`,
  `tests/unit/probes/r38_szenenschutz.cmake`, `tests/integration/test_r38_szenenschutz.cmake`, Anpassung
  `tests/integration/test_r36_kleidung_hundeweg.cmake` (schon in Abnahme 0 bewertet).
- Engine-Aenderung der Nachbesserung 1: nur 432487fa (`player_common.c` +4/-1, `szenenschutz_r38.c`, Kopf); 4db0b69f,
  e325f566, 9fe60f20, c2aeab86 aendern nur Doku/Belege/Integrations-Skript (`git show --stat`).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`;
  exe `re15_port/build/platform/pc/re15_pc.exe` 2026-10-16 23:35:47 (juenger als `szenenschutz_r38.c` 23:34:36 und
  `player_common.c` 23:34:16), md5 `81fa7d42ed08bd5a94a6d614500c09f2` (= Messkopie des Bau-Agenten).

- Messkopien neben der Bau-exe: `re15_pc_abn1b_neu.exe` (md5 81fa7d42..., = re15_pc.exe) und `re15_pc_abn1b_v0825.exe`
  (= `release/pkg-win/re15_port_v0.8.25/re15_pc.exe`, md5 7aa892fc...; Code-Gleichheit Basis/Release:
  `git log 2077bb8e..e2cc7673 -- re15_port/engine re15_port/platform re15_port/include` leer). Laeufe unter
  `<scratchpad>/B_abn1/runs/<name>` (Werkzeuge `B_abn1/lauf.sh`, `B_abn1/auswerten.py`; beide Messkopien nach der
  Abnahme wieder geloescht). Hinweis: beim Anlegen habe ich
  versehentlich die Datei `<scratchpad>/lauf.sh` (Werkzeug der abgeschlossenen Abnahme 0) ueberschrieben; Repo und
  Spurbaum sind davon nicht beruehrt.

## 1. Tests der Spur + Nachbar-Riegel (selbst gefahren)

- `ctest --test-dir re15_port/build -R "^(unit_sca_wall_hit|unit_r38_szenenschutz|unit_r38_szenenschutz_alle|unit_r38_szenenschutz_arm)$" --timeout 600 --output-on-failure -V`
  -> `100% tests passed, 0 tests failed out of 4`. unit_sca_wall_hit (= M1 der Abnahme 0): `[C1] ... FREI: 240 Frames
  neu=0 alt=0 ref=0 ... Verbraucher=237 (max 0)`, `OK (0 Fehler)`; im Lauf des Riegels 0 `[szenenschutz]`-Zeilen
  (`grep -c "^209: \[szenenschutz\]"` = 0), obwohl der Aufbau die 11C0-Eingangsszene mit `Plc_dest(slot=0 mode=0x09 ...)`
  startet — die engere Halte-Bedingung greift ohne Nachricht nicht.
- Auswahl des Bau-Agenten selbst geprueft: `B_belege/unit_regex_nb1.txt` = 438 Namen. Eigene Gegenprobe (Werkzeug
  `B_abn1/abdeckung.py`: alle Test-Quellen, die `re15_re2dog_tick|dog_atk_cd|re15_crow_ai_tick|re15_re2crow|
  re15_player_is_grabbed|re15_enemy_ai_tick|re15_dog_ai_tick` direkt nennen, ueber build.ninja/`ctest -N -V` auf
  Testnamen abgebildet): 4 Tests fehlten in der Auswahl — `unit_r35_fenster_{glas,ereignis,knall,satzform}`
  (test_r35_cut11c0_fenster.c tickt die RE2-Kraehe direkt; der neue Arbiter-Haken liest `re15_szenenschutz_sicht()`, das
  ausserhalb von run_all 0 ist). Ich habe sie dazugenommen:
  `ctest -R "<438 + 4 Namen>" -j 4 --timeout 600 --output-on-failure` -> `100% tests passed, 0 tests failed out of 442`
  (Total Test time (real) = 37.49 sec, Maschine unter Volllast).

## 2. Nutzer-Punkt, selbst an der exe gemessen — UEBER DEN ECHTEN WEG

### 2.1 ROOM1190 (Leon, Standard-KI): echter TUER-Eintritt + echtes Hebelraetsel
Weg: Neues Spiel (`RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`), Debug-Sprung nach ROOM1180 (`RE15_DEBUG_JUMP=1180@30`,
Spawn vor Tuer 2 `RE15_PLAYER_POS=-8133,10226,1024` = Ankunftspunkt der Gegentuer ROOM1190 sub13 @0x028B2, Blick -z),
Strom an als Vorbedingung (`RE15_SET_FLAG=4:243`, Ck @0x0237C), Tuer per Quadrat (`RE15_PRESS=square@60,square@61`) ->
`[tuer] Sequenz Archiv 2 DOOR13 ... Tuer 19` -> `[room] PC loaded room1190.rdt`, Ankunft `PL(3174,-11182,rot=1024)`
(= Door_aot_set ROOM1180 slot 2 @0x00A54). In ROOM1190 nur Steuerkreuz/Quadrat
(`RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT=R0.36667,U2.1,L0.36667,U6.1,R0.36667,U1.0,
W0.3,A0.1,W2,A0.1,W5,R0.7,U0.9,L0.36667,U3.25,L0.33333,U1.0,W0.3,A0.1,W2,A0.1,W12`): Schalter 0 und 2 (Kombination A,
sub01 @0x0244C/@0x02454 -> Evt_exec sub10 @0x02468). Kein Flag-Setzen der Szene, kein Teleport im Raum.

debug.log (neu, Lauf e1190n): `[evt] F419 room=1190 Evt_exec sub=2` / `[evt] F841 ... sub=4` / `[evt] F1015 ... sub=10` /
`[szenenschutz] F1015 an (Szene haelt den Spieler)` / `[szenenschutz] F1233 aus (94 geschuetzte Gegner-Ticks)`.
v0.8.25 (Lauf e1190v): dieselben drei Evt_exec-Zeilen in denselben Bildern (F419/F841/F1015).

| Stand | Szene pm=2 | HP-Verlust IN der Szene | Hund st1/ss1=3 IN der Szene | erster HP-Verlust nach der Szene | Hund 1 Landung -> erster Angriff |
|---|---|---|---|---|---|
| v0.8.25 (e1190v) | F1015..F1232 | **20: F1201 hp 100->80 bei pm=2** | **49 Bilder (Slots 1+2, erstes F1187)** | F1246 (+14) | F1169 -> F1187 (+18) |
| neu (e1190n) | F1015..F1232 | **0** | **0** | F1299 (+67) | F1169 -> F1290 (+121) |

(Auswertung `python B_abn1/auswerten.py runs/<lauf>/state.log <seg> 20 -v`; v0.8.25 stirbt vor F1800 und startet neu,
darum dort Segment 2.) Die +121 entsprechen der Sperrzeit 0x78 = 120 Bilder ab der Landung (@0x801115e8-ec, Abzug
@0x8010dd5c-70, s. 4.). => Am echten Weg ist der Befund des Nutzers mit v0.8.25 reproduziert (Biss in der Szene) und
mit dem neuen Stand weg; nach der Szene greifen die Hunde wieder an (5 Bisse bis F1575).
Wiederholung mit Bildserie (Laeufe b1190neu/b1190v0825, `RE15_FRAMEDUMP=1160-1320/8:fd_`, `RE15_WINDOW_SCALE=1`,
EXIT 1320): Werte identisch (v0.8.25 F1201 100->80 bei pm=2, 49 Angriffsbilder; neu 0/0, Biss F1299). Bildblatt
`<scratchpad>/B_abn1/blatt_1190.png` (links v0.8.25, rechts neu; Ausschnitte `blatt_1190_ausschnitt.png`,
`blatt_1190_nach.png`): in der Szene zeigt die Kamera den Lueftungsschacht (Leon nicht im Bild), die Beschriftung
traegt `hp=80 pm=2` (v0.8.25) bzw. `hp=100 pm=2` (neu); beim Schnitt auf Leon (F1240) steht er in v0.8.25 schon
blutend mit Hund im Angriff (1/3), neu unverletzt, die Hunde laufen (1/2).

### 2.2 ROOM11D0 (Leon, Standard-KI): echter TUER-Eintritt + echte Aufnahme
Weg: Neues Spiel, Debug-Sprung ROOM1180, Spawn vor Tuer 0 (`RE15_PLAYER_POS=-4729,-16018,1024` = Ankunft der Gegentuer
ROOM11D0 @0x011E2), Strom an (`RE15_SET_FLAG=4:243`: ROOM1180 main00 Else-Zweig @0x00A12 -> Tuer 0 nach ROOM11D0),
Quadrat F60/F61 -> `[tuer] Sequenz Archiv 2 DOOR1A ... Tuer 26` -> `[room] PC loaded room11d0.rdt`, Ankunft
`PL(-300,-17400,rot=1024)`; in ROOM11D0 nur Eingaben (`RE15_INPUT_SCRIPT_START=300`,
`U1.6,R0.36667,U2.8,R0.36667,U3.7667,L0.36667,U2.6,W0.3,L0.36667,U0.4,W0.3,A0.1,W1.6` + 8x `A0.1,W0.23333` + `W12`):
Leon laeuft durch die Kaefigtuer zu (-10983,-12182), Quadrat an AOT 9 -> sub03 (Set(2,7) @0x01892) -> Aufnahme-Modal
bestaetigt -> sub02 (Freigabe Member_set(12,0x42) @0x0172C.., Set(2,7)/(1,27) @0x01754/@0x01758 ... @0x01888/@0x0188C).
(Der in Abnahme 0 notierte Weg `L0.36667,U1.6,...` lief bei mir an der Kaefigwand fest: deren RE15_PRESS-Rechts-Druecke
F40..F50 feuerten wegen des je Raum neu beginnenden Bildzaehlers auch in ROOM11D0 und drehten Leon vorab um +1056 —
die tatsaechlich gelaufene Bahn ist die obige ohne das erste L; Abnahme-0-Lauf t1 state.log F299 rot=2080.)

| Stand | Szene pm=2 | HP-Verlust IN der Szene | Hund st1/ss1=3 IN der Szene | erster HP-Verlust nach der Szene |
|---|---|---|---|---|
| v0.8.25 (e11d0v0825) | F698..F872 | 0 | **6 Bilder (Slot 4 ab F867 = Lunge in der Szene)** | F921 (+49) |
| neu (e11d0neu) | F698..F872 | 0 | **0** | F892 (+20); Hund 4 Commit ss1=3 erst F885 |

debug.log neu: `[szenenschutz] F698 an` / `[evt] F799 room=11d0 Evt_exec sub=2` / `[scd F873] letterbox closed ->
gameplay (player_mode=0)` / `[szenenschutz] F873 aus (365 geschuetzte Gegner-Ticks)`. Hund 4 (neu) laeuft in der Szene
weiter (ss1=2, d 7865 -> 3328), erreicht Leon im Kaefig nach der Szene, Commit F885, Treffer F892-F894 (hp 80).
=> In der Szene greift kein Hund mehr an (v0.8.25: Lunge ab F867 waehrend pm=2). Wie in Abnahme 0 (H1) kommt der
erste Biss nach der Szene FRUEHER als in v0.8.25 (20 statt 49 Bilder) — der in der Szene begonnene Ansprung von
v0.8.25 ging ins Leere. Leon ist ab F873 beweglich (pm=0, pst=1); eine Schonfrist nach der Szene hat 11D0 im Original
nicht (Freigabe FUN_80111658 ohne +0x1d6-Schreiber, Dossier 2.2). Wortlaut ("in Cutscenes ... wo er sich noch nicht
mal bewegen kann") damit erfuellt; s. Hinweis H1.

### 2.2b Elza-Varianten (beide Szenarien), echter TUER-Eintritt aus ROOM1181
Elza-Wahl `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, Sprung `1180@250` -> `ROOM1181`,
Tuer-Druck F300/F301, Skript ab F400 (Bildzaehler je Raum).
- ROOM1191 (Kombination B, sub01 @0x0247C-0x024A8: (4,243)=1, (3,112)=1, Bank-5-Bits 6+7; `RE15_SET_FLAG=4:243,3:112`,
  Tuer 2 -> `[tuer] Sequenz ... DOOR13` -> `loaded room1191.rdt`; Skript `R0.36667,U2.1,L0.36667,U1.2,R0.36667,U1.0,
  W0.3,A0.1,W2,A0.1,W5,R0.7,U0.9,R0.36667,U3.2167,R0.33333,U1.0,W0.3,A0.1,W2,A0.1,W12` = Schalter 3 (`sub=5` F572),
  Schalter 1 (`sub=3` F993), Szene `sub=10` F1167):

| Stand | Szene pm=2 | HP-Verlust IN der Szene | Hund st1/ss1=3 IN der Szene | erster HP-Verlust nach der Szene |
|---|---|---|---|---|
| v0.8.25 (e1191v0825) | F1167..F1384 | 0 | **35 Bilder (Slot 1 ab F1332)** | F1398 (+14, pm=1) |
| neu (e1191neu) | F1167..F1384 | 0 | **0** | F1515 (+131) |

  debug.log neu: `[szenenschutz] F1167 an` / `F1385 aus (94 geschuetzte Gegner-Ticks)`.
- ROOM11D1 (Tuer 0, Strom an, Skript wie 2.2): `[tuer] Sequenz ... DOOR1A` -> `loaded room11d1.rdt`,
  `[szenenschutz] F798 an` / `[evt] F899 room=11d1 Evt_exec sub=2` / `F973 aus (365 geschuetzte Gegner-Ticks)`:

| Stand | Szene pm=2 | HP-Verlust IN der Szene | Hund st1/ss1=3 IN der Szene | erster HP-Verlust nach der Szene |
|---|---|---|---|---|
| v0.8.25 (e11d1v0825) | F798..F972 | 0 | **6 Bilder (Slot 4 ab F967)** | F1021 (+49) |
| neu (e11d1neu) | F798..F972 | 0 | **0** | F992 (+20) |

### 2.3 Ausnahme "Teil der Cutscene" — Elza ROOM1171, echter TUER-Eintritt
Die 1171-Szene startet nur bei Eintritts-Schnitt 0 oder 3: sub00 @0x01374 `Switch` / @0x01378 `Case 0` / @0x01388
`Case 3` -> `Gosub 07`; sub07 @0x016C4 `Ck(3,194)==1` -> John 0x49 + 2 Kraehen 0x21 grid 0x40 (@0x016C8/@0x016DC/
@0x016F0) -> `Evt_exec sub2` @0x01704. Schnitt 3 hat genau die Tuer aus ROOM10B1 (Door_aot_set @0x016D2 `... 00 17 03`).
(Gegenprobe: Tuer aus ROOM1141 @0x00AB2 hat Schnitt 9 -> Lauf t1141a: Raum geladen, keine Szene — darum dieser Weg.)
Weg: Elza (`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, `[pl] Spieler-Familie PL04`),
Story-Vorbedingung `RE15_SET_FLAG=3:194`, Debug-Sprung ROOM10B0 (-> `ROOM10B1`) mit Spawn in der Tuerflaeche
(`RE15_PLAYER_POS=14335,-6840,0`), Quadrat F300/F301 -> `[tuer] Sequenz Archiv 2 DOOR16 ... Tuer 22` ->
`[room] PC loaded room1171.rdt` -> `[evt] ... room=1171 Evt_exec sub=2`; EXIT F400 in 1171. Je Stand x KI-Modus:

| Lauf | Stand / KI | Szene pm=2 | Griff/HP in der Szene | state.log 1171 (400 Bilder) |
|---|---|---|---|---|
| t10b1a | neu / RE1.5 (`RE15_AI_FLAVOR=re15`) | F1..F400 | Kraehe Slot 2 (st=4 FLIGHT-2) greift F101: hp 100->92, gr=1 (300 Bilder) | md5 d1cd525dbdbe |
| t10b1v | v0.8.25 / RE1.5 | F1..F400 | identisch | md5 d1cd525dbdbe — **bytegleich** |
| t10b1sn | neu / Standard | F1..F400 | kein Griff (Kraehen ohne Angriff = Haenger der Spur I) | md5 53f551bdab1e |
| t10b1sv | v0.8.25 / Standard | F1..F400 | identisch | md5 53f551bdab1e — **bytegleich** |

debug.log neu in 1171: `[szenenschutz] F6 an (Szene haelt den Spieler)` — der Schutz ist aktiv, fasst aber keinen
Akteur an (John Typ 0x49 >= 0x40, Kraehen +0x4 = 4 und grid 0x40). => Der Szenen-Angriff der Ausnahme bleibt
unveraendert. Integration mit Spur I (in `r38/integration` gemergt, hier nur gelesen: `git show
r38/heliport1171:re15_port/engine/src/heliport1171_r38.c`, Diff von enemy_ai_common.c): die Ereignis-Kraehe
(`re15_r38i_ereigniskraehe` = Typ 0x21 && grid&0x40) laeuft auch unter der Standard-KI im RE1.5-Gehirn (INIT grid&0x40
-> +0x4 = 4, @0x80112400-0c) und ihr Rueckgriff schreibt keine HP — vom Szenenschutz ausgenommen (`state != 1`,
`grid&0x40` in `re15_szenenschutz_gegen`). Am vereinigten Stand selbst gemessen habe ich das nicht (Integrationsbaum
der Orchestrierung); der Riegel `integration_r38_heliport1171` deckt es dort ab.

## 3. RE-Gate (selbst disassembliert, richtige Binaerdatei)

| Behauptung (Code/Dossier/Bau-Agent) | Binaerdatei @Adresse | gelesen (re15_disasm.py / re2_disasm.py) |
|---|---|---|
| M3: Typgrenze 0x40 (szenenschutz_r38.c) | PSX.EXE @0x80050a70-7c | `lbu v0,-146(s0)` / `nop` / `sltiu v0,v0,0x40` / `beq v0,zero,0x80050ae4`; `bytes 0x80050a70 12` = `6e ff 02 92 00 00 00 00 40 00 42 2c`. Kontext @0x80050a08-48: `lui a0,0x800b`/`addiu a0,a0,-13746` = 0x800aca4e, `addiu s1,a0,478` = 0x800acc2c (Gegnerfeld), `addiu s0,a0,632` = Gegner+0x9A -> `-146(s0)` = Gegner+0x8 (Typ); Stride 500 @0x80050a64/6c — **stimmt** |
| M2: Spieler-Dispatcher bindet +0x4 | PSX.EXE @0x80031c8c-cb4, Tabelle @0x80073f90 | `lbu v1,-13736(v1)` (0x800aca58) / `sll 2` / `addiu at,at,16272` (0x80073f90) / `lw v0,0(at)` / `jalr v0`; Tabelle [1] = 0x80031de8, [4] = 0x80030660; der Executor 0x80030660 dispatcht nur ueber 0x80073e30[aca59] (@0x8003069c-ac), kein Pad-Zugriff im Rumpf bis `jr ra` @0x80030708 — **stimmt** |
| (B) 1190-Sperrzeit 0x78 | STAGE1.BIN @0x801115e8/ec | `ori v0,zero,0x78` / `sh v0,470(v1)` (+0x1d6, v1 = g_entity(cur)) — **stimmt** |
| (B) Abzug je ACTIVE-Bild | STAGE1.BIN @0x8010dd5c-70 | `lh v0,470(a0)` / `beq v0,zero` / `addiu v0,v1,-1` / `sh v0,470(a0)` — **stimmt** |
| (B) Tor ATTACK-RANGE | STAGE1.BIN @0x8010e578/80 | `lh v0,470(v0)` / `bne v0,zero,0x8010e628` — **stimmt** |
| 11D0-Freigabe ohne Sperrzeit | STAGE1.BIN FUN_80111658 | `ori v0,zero,0x42` @0x80111710, `ori v0,zero,0xc01` @0x80111718, `sw v0,4(v1)` @0x8011172c; kein Store auf `470(` bis `jr ra` @0x8011175c — **stimmt** |
| (C) RE2-Kraehe Abbruch-Bit 0x10 | EMOVL21_S0.BIN @0x80104400-64 | `lhu v0,554(s0)` / `andi v0,v0,0x10` / `beq` / `jal 0x8010472c` (a1=350) / `jal 0x80015fe8` / `andi 0x9d` -> `sb 544(s0)` / a1=4 -> `jal 0x80104088` / `sb 30,540(s0)` / `andi 0xffa7` -> `sh 554(s0)` — **stimmt** |

Diff-Suche (`git diff master...HEAD -- re15_port` bzw. Dossier, nur `+`-Zeilen) nach
`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|vermutlich|ungemessen|getenv|geschaetzt`:
**0 Treffer**. Konstanten im Engine-Diff: 0x40 (@0x80050a78), Zustand 1/4 (Wurzeltabellen [1]; @0x80041bb0/@0x80041c14),
grid 0x40 (@0x80112408), +0x93 Bit 0 (@0x8010e5c4/@0x80012e24), Sub 11/13 (@0x80101F2C/@0x80102444), Bit 0x10
(@0x80104418), 0x78 (@0x801115e8-ec) — alle mit Adresse. `player_mode == 2` und `re15_player_msg_sperre()` sind die
Port-Bewegungstore (Port-Wahl, im Kopf von szenenschutz_r38.c begruendet; NUTZER-VORGABE "wo er sich noch nicht mal
bewegen kann"). Erklaert der Fix den Befund? Ja — gemessen in 2.1/2.2: vorher liefen die freien Gehirne in pm=2
ungebremst (Biss F1201 bei pm=2, Lunge F867 bei pm=2), jetzt sehen sie in pm=2 die Original-Tore geschlossen; der
RE2-Hund ehrt die Sperrzeit (Landung F1169 -> Commit F1290).

## 4. Maengel der Abnahme 0 — selbst nachgeprueft

### M1 (fremder Riegel unit_sca_wall_hit) — behoben
`ctest -R "^(unit_sca_wall_hit|...)$"` -> Passed; `[C1] ... FREI: 240 Frames neu=0 alt=0 ref=0 ... Verbraucher=237`
(= Wert ohne Spur B laut Abnahme 0), `OK (0 Fehler)`, keine `[szenenschutz]`-Zeile im Lauf. Ursache war die zu weite
Halte-Bedingung (M2); der fremde Riegel ist unveraendert (`git diff master...HEAD --name-only` nennt ihn nicht).
Riegel-Auswahl: 438 + 4 selbst ergaenzte = 442 Tests gruen (Abschnitt 1).

### M2 (Halten bei Spieler+0x4 == 4, obwohl Leon im Port laeuft) — behoben
Code (szenenschutz_r38.c): `if (g_scd.player_mode == 2) return 1; return g_actors[PLAYER].state == 4 &&
re15_player_msg_sperre();` — `re15_player_msg_sperre()` ist woertlich der bisherige msg_block-Ausdruck des
Bewegungs-Gates (player_common.c, `message_display_frames > 0 || message_query != 0`), das Gate liest jetzt dieselbe
Funktion (reine Ausfaktorisierung, gleicher Ausdruck). Damit gilt: geschuetzt <=> das Pad-Gate des Ports
(`player_mode != 2 && !msg_block`) ist zu (Fall pm=2) bzw. Executor + Nachricht.
Selbst gemessen (Laeufe m1230neu/m1230v0825, Rezept der Abnahme 0 e14: `RE15_DEBUG_JUMP=1230@240 RE15_SET_FLAG=4:243
RE15_PLAYER_POS=3900,25000,3072 RE15_INPUT_SCRIPT=A0.1,U4,W2 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=300
RE15_EXIT_AT=560#1230`): 30 Bilder `pst=4 msg a=0 pm=0` (F301..F330), Leon z 25000 -> 27100; debug.log nach
`loaded room1230` KEIN `[szenenschutz] F.. an` (Abnahme 0 mass hier `F301 an`); ab F331 `msg(a=1) pf=FFFF0007` (Maske
0xFFFF0000 aus Message_on `2b 0a ff ff` @0x014BC traegt 0x20000000 -> Hund steht, af bleibt 2). state.log Segment
ROOM1230 neu == v0.8.25 **bytegleich** (md5 ee97d96140cd, 560 Bilder) — die Spur aendert 1230 nicht mehr.
Riegel: `integration_r38_szenenschutz` (selbst gefahren, 174 s, Passed) meldet `[1230] Executor-Bilder ohne
(2,7)/Nachricht mit Hund: 30, Leon-Weg z 2100; Szenenschutz-Fenster in ROOM1230: ''`. Negativkontrolle der Auswertung
selbst gefahren (Werkzeug `B_abn1/neg1230/`: der 1230-Block des Skripts gegen mein m1230neu-Log mit eingefuegter
Altzeile `[szenenschutz] F301 an`): `CMake Error ... r38_szenenschutz[1230]: Szenenschutz haelt, obwohl Leon sich
bewegen kann`, rc=1; ohne Einfuegung rc=0. Der Stolperdraht (Weg < 1000 -> Abbruch "Bewegungs-Gate bindet jetzt an
Spieler+0x4") ist im Skript-Kopf und im Dossier N1.OFFEN begruendet.

### M3 (Typgrenze 0x40 ohne Zitat) — behoben
Zitat steht am Code (`re15_szenenschutz_gegen`), Bytes selbst gelesen (Abschnitt 3).

## 5. Riegel der Spur — Messkraft selbst geprueft
- `integration_r38_szenenschutz` gegen die **v0.8.25-exe** (Negativkontrolle, `cmake -DRE15_PC_EXE=<re15_pc_abn1b_v0825.exe>
  -DWORKDIR=<scratchpad>/B_abn1/neg_v0825 -P tests/integration/test_r38_szenenschutz.cmake`): rc=1,
  `r38_szenenschutz[1190] Szene F521..F738 hp-Verlust=0 Angriffs-Bilder=33 erster Biss F752` ->
  `CMake Error ... 33 Bilder Hund im Angriff (st1/3) waehrend der Szene`. Mit dem neuen Stand gruen (Abschnitt 4 M2).
  (Die dabei liegengebliebene Messkopie `re15_pc_r38b_szene_ee042d4a.exe` habe ich wieder geloescht.)
- 1230-Teil: Negativkontrolle mit eingefuegter Altzeile (Abschnitt 4 M2) — schlaegt an.
- Unit-Riegel T1/T4b (Diff 432487fa gelesen): Executor + Nachricht -> kein Angriff; Executor ohne Nachricht -> Biss;
  nach Plc_ret -> Biss; T1 prueft die Halte-Bedingung mit/ohne Nachricht bzw. Ja/Nein-Abfrage und Spieler+0x4 == 1.

## 6. Nebenwirkungen (selbst gemessen)

### 6.1 Unit + Integration
- Unit: 442/442 gruen (Abschnitt 1).
- Integration (eigene Auswahl: Szenen mit Gegnern in beiden Szenarien, Hunde 1190, Kraehen 1170/10C0/10C1, Gorilla 11C0,
  Feuer 1090, Birkin 3071/5080/5090, Codepanel 1230, Elza-Start 1031, Montage 1150, Kisten/Fahrstuhl):
  `ctest -R '^(integration_r36_kleidung_hundeweg|integration_r36_ziel1190_scheiben|integration_r37_roy_leon1170|
  integration_r37_roy_szene1011|integration_r37_roy_szene1021|integration_r37_szene11c0|integration_r35_zgirl_5090|
  integration_r36_story_leon|integration_r36_story_elza|integration_r37_codepanel|integration_r37_elzaems_elza10c1kraehe|
  integration_r37_elzaems_leon10c0kraehe|integration_r37_elzaems_elza3071|integration_r37_elzaems_leon5080|
  integration_r37_fahrstuhl_n1|integration_r35_granate|integration_r37_kistenregel|integration_r36_szenenleon|
  integration_r36_szene1150|integration_elza_vollstart|integration_r37_feuerloescher)$' -j 3 --timeout 1500`
  -> `100% tests passed, 0 tests failed out of 21` (Total Test time (real) = 911.60 sec, unter Volllast, ohne
  Wiederholung). Dazu `integration_r38_szenenschutz` (Abschnitt 4).
- Sicherheitsnetz: 0 Zeilen `[szenenschutz] ... Treffer abgefangen|WARNUNG` in meinen 29 Laeufen mit Szenenschutz-Zeilen
  und in allen Arbeitsordnern unter `re15_port/build/tests/integration` (auch die 79er-Laeufe des Bau-Agenten).

### 6.2 Szenen ohne beabsichtigte Aenderung: neu == v0.8.25 bytegleich (state.log)
| Lauf | Szene | Schutz aktiv? | Ergebnis |
|---|---|---|---|
| m1230neu/v0825 | ROOM1230 sub21 Hocke + Nachricht (Rezept Abnahme 0 e14) | nein | bytegleich (560 Bilder) |
| e1031neu/v0825 | Elzas Start ROOM1031 (Rezept integration_elza_vollstart) | ja, F451..F999, 3288 geschuetzte Ticks | bytegleich (1400 Bilder) |
| t10b1a/v, t10b1sn/sv | Elza ROOM1171 ueber die Tuer aus 10B1, RE1.5- und Standard-KI | ja, ab F6 | je bytegleich (400 Bilder) |
| e3071neu/v0825 | Elza ROOM3071 Birkin-Szene sub02 (Debug-Sprung, AOT 2 per Quadrat) | ja, ab F301 | bytegleich (1200 Bilder) |
| m1150neu/v0825 | Irons' Tod ROOM1150 + Montage 1130/1040/1030 -> 11C0 (Rezept integration_r36_szene1150: Karte `probe_r35_cut1150_karte re15_card.mcr 1130 nach10f0 ersteszene` + CONTINUE + Tuer) | ja, ab F6 bis Ende | bytegleich (alle Segmente, 3042 Zeilen) |
| r1190neu/v0825 | ROOM1190 echter Weg, RE1.5-KI | ja, F1015..F1232 | bytegleich (1600 Bilder) |
| g11c0neu/v0825 | ROOM11C0 Gorilla-Eingangsszene (Debug-Sprung, Rezept Abnahme 0 H5) | ja, bis F1071, 2130 geschuetzte Ticks | bytegleich (1300 Bilder) |

### 6.3 Gibt es Szenen, deren Angriff "Teil der Cutscene" ist und jetzt unterdrueckt wuerde? (Zensus, selbst)
- Gegner-Code, der Szenen-Flags setzt (alle `jal 0x8004ef90` in STAGE1-6.BIN, Argumente disassembliert): STAGE1 = 17
  Stellen, davon 15 mit a1 = `lbu a1,454(v0)` = Gegner+0x1C6 (Status-/Todes-Flag des Gegners, verschiedene Banken);
  die zwei anderen sind der 1171-Kraehengriff
  @0x801152c4 ((5,0x1C), FLIGHT-2, ausgenommen) und der Feuer-Emitter @0x80116ac8 ((5,0x1D+n), ROOM1090-Skripte lesen
  (5,29..31) nicht). STAGE3/STAGE5: Birkin-Sub 10 (Spawn-Auftritt 0x80119524) setzt (5,0x1F) @0x80119658 und (5,0x1E)
  @0x801197b4, auf die ROOM3071 sub02 wartet (`While` @0x0364D/@0x03665). Der Auftritt greift den Spieler im Original
  NICHT an (0x80119524-0x80119b50: nur anim_set, 0x8011b5c4, pos_advance, 0x800453d0, 0x8001aac4 Gier-Drehung,
  0x8004ef90; kein Zugriff auf 0x800acae7/0x800acaee/0x800aca58). Gemessen bytegleich (6.2).
- `Member_set` auf Gegner in Halte-Subs (Werkzeug `B_abn1/member_zensus.py`, alle 240 RDTs, `Work_set` Art 2 = Akteur):
  70 Treffer, ausschliesslich Member 12 (grid +0x9): Freigaben (1190/1191 0x43, 11D0/11D1 0x42, 3060/3061 0x42),
  Montage-Zombies (1100/1101, 11F0/11F1, 1200/1201 0x89/0x8A), Gorilla 1141/11C0 (0x10/0x30), Birkin 3070/3071/
  5090/5091 (0x13/0x1/0x12), 3080 (0x4), Kakerlaken 5031/50B0 (0x41/0x61). Kein Zustands-/Sub-Schreiber (Member 8/9/10).
  Die gemessenen Vertreter (1190/1191, 11D0/11D1 = beabsichtigte Aenderung; 3071, Montage 1150, 11C0 = bytegleich,
  6.2) zeigen keinen unterdrueckten Szenen-Angriff.

## 7. Vertrag
- Pfade (`git diff master...HEAD --name-only`): nur Dossier/Abnahme/Belege, eigene Dateien
  (`szenenschutz_r38.c`, `re15_szenenschutz_r38.h`, 3 Unit-Tests, Probe-Registrierung `tests/unit/probes/
  r38_szenenschutz.cmake`, Integrations-Skript) und kleine Haken mit Kommentar "Runde 38 Spur B": enemy_ai_common.c
  (Include, 1 Zeile is_grabbed, 3 Bedingungen Kraehe, 3 Aufrufe run_all), enemy_ai_re2_crow.c (+3), enemy_ai_re2_dog.c
  (+9/-2), player_common.c (+4/-1: `re15_player_msg_sperre()` = woertlich der bisherige msg_block-Ausdruck, das Gate ruft
  sie; verhaltensneutral). Keine `release/`, `platform/android/`, `shared_assets/PSX/`, keine Test-CMakeLists. Keine
  Bank-9-Bits, Nachrichten-Ids, Items, Sprachdateien. Fremder Riegel test_r36_kleidung_hundeweg.cmake (schon in Abnahme 0
  bewertet) laeuft gruen.
- Kein anderer Runde-38-Zweig beruehrt player_common.c / enemy_ai_re2_dog.c / enemy_ai_re2_crow.c
  (`git diff --numstat e2cc7673...r38/<x>` je Zweig leer). Probe-Merge in den Integrationsstand ohne Arbeitsbaum
  (`git merge-tree --write-tree r38/integration r38/szenenschutz`): EIN Konflikt, nur der Include-Block in
  enemy_ai_common.c (Spur I, Spur T und Spur B fuegen an derselben Stelle je eine `#include`-Zeile ein) — alle drei
  Zeilen behalten.

## 8. Hinweise (keine Maengel)
- **H1 11D0/11D1:** In der Szene greift kein Hund mehr an; der erste Biss nach der Szene kommt aber FRUEHER als in v0.8.25
  (echter Weg: Leon F892 = +20 statt F921 = +49; Elza F992 = +20 statt F1021 = +49) — in v0.8.25 ging der in der Szene
  begonnene Ansprung ins Leere. Leon ist ab dem ersten freien Bild beweglich (pm=0, pst=1). Das Original hat in 11D0
  keine Sperrzeit nach der Freigabe (FUN_80111658 schreibt +0x1d6 nicht, Abschnitt 3). Eine Schonfrist NACH der Szene
  waere eine neue Nutzer-Vorgabe.
- **H2 Feuer 0x26:** das Sicherheitsnetz gibt Feuer-Abzug zurueck, solange eine Szene haelt (bewusste Erweiterung, Dossier
  6/N1.OFFEN); in keinem Lauf eingetreten (0 `Treffer abgefangen`).
- **H3 Integration:** (a) Spur T (in `r38/integration`) bringt neue Griffe fuer 0x2B (`re15_r38_sm_griff1/2`, setzen +0x93
  selbst); nach dem Merge `unit_r38_szenenschutz_alle` (Zensus inkl. 0x2B) und `integration_r38_szenenschutz` im
  Integrationsbaum fahren — der Zensus-Riegel schlaegt an, wenn ein gemergter Angriff das Spieler-Tor umgeht. (b) Spur O
  (Montage-Zombies Richtung Kamera, noch nicht gemergt): das RE2-Zombie-Gehirn liest `re15_player_is_grabbed()` auch in
  der Ausstiegsleiter @0x80102564-74 (-> Zustandswort 0x101), und die Montage laeuft im Halte-Fenster (m1150: Schutz an
  ab F6). Heute bytegleich zu v0.8.25 (6.2); nach dem Merge von O dessen Riegel gegen den Szenenschutz pruefen.
  (c) Spur I: Ereignis-Kraehe ausgenommen (2.3).
- **H4 Riegel-Auswahl:** die systematische Auswahl des Bau-Agenten liess 4 Tests aus, die die RE2-Kraehe direkt ticken
  (`unit_r35_fenster_{glas,ereignis,knall,satzform}`); sie koennen vom Haken nicht betroffen sein (Sicht nur in run_all)
  und laufen gruen (Abschnitt 1).
- **H5 vorbestehend, nicht Spur B:** ROOM3071 sub02 haengt im Port in beiden Staenden (Birkin-Sub 10 ist kompakt
  portiert und setzt (5,0x1F)/(5,0x1E) nicht, enemy_ai_common.c `case 10` "Faithful-compact"); state.log bytegleich.
  Ebenso vorbestehend: das Bewegungs-Gate bindet nicht an Spieler+0x4 (Dossier N1.OFFEN, gemessen 1230).

## 9. Urteil

| Punkt (Wortlaut AUFTRAG.md) | Urteil | Beleg |
|---|---|---|
| "Das Leon schon in Cutscenes angegriffen werden kann, wo er sich noch nicht mal bewegen kann ist mist. So wie bei Leon zum Beispiel bei ROOM 1190 bei den Hunden oder 11D0 bei den Hunden. Es sei den es ist Teil der Cutscene wie bei Elza Room 1170." | erfuellt | Echte Wege (Tuer + Hebel/Aufnahme), neu gegen v0.8.25: 1190 v0.8.25 Biss F1201 bei pm=2 + 49 Angriffsbilder -> neu 0/0 (e1190n/v, b1190 mit Bildblatt); 11D0 v0.8.25 Lunge F867-F872 in der Szene -> neu 0 (e11d0n/v); Elza 1191 (35 -> 0) und 11D1 (6 -> 0); Ausnahme Elza 1171 ueber die Tuer aus 10B1: Kraehengriff F101 bei pm=2 bleibt, state.log bytegleich in beiden KI-Modi. Riegel gruen und messend (Negativkontrolle gegen v0.8.25 schlaegt an). |

Gates: Bau ok; Spur-Riegel 4/4 + exe-Riegel gruen; RE-Gate haelt (7 Stellen selbst disassembliert, 0 Rate-Tells);
Vertrag haelt; Unit 442/442, Integration 21/21 gruen; Maengel M1-M3 der Abnahme 0 behoben und selbst nachgemessen.

## 10. Maengel
Keine.

bestanden = true.
