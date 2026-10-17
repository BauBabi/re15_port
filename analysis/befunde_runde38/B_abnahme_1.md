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
  `<scratchpad>/B_abn1/runs/<name>` (Werkzeuge `B_abn1/lauf.sh`, `B_abn1/auswerten.py`). Hinweis: beim Anlegen habe ich
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

(Fortsetzung folgt.)
