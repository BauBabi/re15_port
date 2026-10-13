# Runde 37 — Spur Q kistenregel — UNABHAENGIGE ABNAHME 0 (2026-10-13)

Baum `.claude/worktrees/r37_kistenregel`, Zweig `r37/kistenregel`, Stand bae33f2d (Basis 6f360fb3).
Pruefer: Abnahme-Agent. Gewertet wird nur, was hier selbst am gebauten Stand gemessen oder selbst disassembliert wurde.
Messordner (Scratchpad dieser Sitzung): `.../scratchpad/abnQ0/<lauf>/debug.log` (unten nur `<lauf>`).

## Ergebnis
**BESTANDEN.** Beide Punkte erfuellt, alle Gates halten. Keine Maengel, die den Auftrag betreffen. Drei Hinweise fuer
die Integration (H1..H3), keiner davon ist eine Abweichung vom Wortlaut.

## 0. Gelesen
* AUFTRAG.md "Orchestrator-Auftrag Q" (Z. 255-270), VERTRAG.md Zeile Q (Z. 28) und 1.4 (Z. 65), Dossier Q_kistenregel.md.
* `git log 6f360fb3..HEAD`: 9 Commits (8754d28f .. bae33f2d). `git diff 6f360fb3 --stat`: 16 Dateien, +3132/-7.
* Den Code-Diff habe ich ganz gelesen: neu `engine/src/kistenregel_r37.c` (Tabelle `k_faelle[20]` + Regel
  `re15_kistenregel_r37_ck`), neu `include/re15_kistenregel_r37.h`, `scd_vm.c` op_ck +5 Zeilen (Include + Haken vor
  der Bedingung). Tests: neu test_r37_kistenregel.c, probes/r37_kistenregel.cmake, integration/test_r37_kistenregel.cmake.
  Nachgezogen: test_1090_flame_out_pin.c, test_r36_elza.c, test_r36_schluessel.c, integration/test_r36_elza.cmake,
  probe_r36_elza_karte.c (give:). Dazu Werkzeuge tools/r37_kistenregel/*.py und die Belege Q_belege/*.txt.

## 1. Bau
`local_build.sh configure` + `build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. Die exe ist von
2026-10-13 21:44:18. scd_vm.c hat 21:44:16 (Rueckbau nach der Gegenprobe), scd_vm.c.obj 21:44:18. Der Baum ist
sauber (`git status --short` leer). In der exe steht die Messzeile (`grep -c "kistenregel] ROOM" re15_pc.exe` = 3).
Messkopien unter eigenem Namen: `re15_pc_r37q_abn0.exe` (Stand Q) und `re15_pc_r37q_abn0_basis.exe` (Basis-exe aus
r37_feuerloescher/build, 2026-10-12 14:35 = Code von Spur I ohne Q). Beide wurden nach der Abnahme geloescht.

## 2. Punkt 1 — Benutzung nur mit dem Gegenstand im INVENTAR (Auftrag Q), Zensus, Entscheidung je Fall

### 2.1 Vorher (Gegenprobe an der Basis-exe, derselbe Weg wie 2.2 Lauf q1)
`q1basis`: Spielstand ROOM1050 vor der Notausgangstuer (19850,-22300, Blick 1024). Leon traegt den Loescher (`give:0x31:1`),
die Flags sind (9,0x86)/(3,0x85)/(3,0x80). RE15_BOX_TEST=1 oeffnet den Box-Schirm, dort wird der Loescher in Platz 2
getauscht. Dann geht Leon durch die TUER nach 1090, und RE15_FIRE_AOT=2@300#1090 loest den Feuerplatz aus. debug.log
woertlich:
```
[room] PC loaded room1090.rdt (336440 bytes)
[msgtext] room=1090 id=7 "I must hurry up and get something to put out this fire to save that woman!"
[msgtext] room=1090 id=8 "Will you use the Fire Extinguisher?"
[feuerloescher] ROOM1090: kein Loescher (Leon hat ihn in die Item-Box gelegt (Item 0x31 in der Kiste, Platz 2))
[msgtext] room=1090 id=9 "You've used the Fire Extinguisher."
[msgtext] room=1090 id=3 "Leon: There! Are you okay?"
```
Damit ist der Befund des Auftrags reproduziert: Leon loescht, obwohl der Loescher in der Kiste liegt.

### 2.2 Nachher — 1090 (K1), der erste Fall des Auftrags
* `q1` (Stand Q, gleiche Eingabe wie q1basis, ECHTER Tuerweg 1050 -> 1090, Box-Schirm):
  ```
  [room] PC loaded room1090.rdt (336440 bytes)
  [kistenregel] ROOM1090 @0x0230A Ck(3,0x85) K1: Item 0x31 NICHT im Inventar (Item-Box Platz 2) -> Besitz 0, Zweig ohne Gegenstand
  [fire-aot] slot=2 at F300 (Raum 1090)
  [msgtext] room=1090 id=7 "I must hurry up and get something to put out this fire to save that woman!"
  [flow] EXIT_AT: Bild 1000 in Raum 1090 erreicht -> exit
  ```
  Es kommt keine Frage (id=8), kein Loeschen (id=9) und keine Rettung (id=3). Bild: `q1fd/fd_000330.png` zeigt
  "I must hurry up" auf dem Schirm.
* `q2` (Gegenprobe ohne Box-Schirm, Loescher im Inventar, sonst gleich): `K1: Item 0x31 im Inventar -> wie Original`,
  dann id=7, id=8, id=9, `Loescher weggeworfen -> liegt in ROOM1090`, `liegt leer am Boden obj 5 bei (2500,-1800,-1300)`,
  id=3. Das ist das Verhalten von vorher.
* ctest `integration_r37_kistenregel` (selbst gefahren, `lauf_15b0ccba_*`): A (Kiste, Leon drueckt am Feuer selbst die
  Aktionstaste): id=7 zweimal, kein id=8/9. B (Inventar): 7/8/9. I (ECHTE Box ROOM1150, `room=1150 id=3 ITEMBOX`,
  Sprung nach 1090): `K1 ... NICHT im Inventar (Item-Box Platz 2)`, nur id=7, `EXIT_AT: Bild 500 in Raum 1090`.

### 2.3 Nachher — die uebrigen Faelle an der exe (selbst gefahren; Kiste gegen Inventar)
Karte mit `kiste:`/`give:` -> CONTINUE -> RE15_FIRE_AOT=<Platz>@70 (derselbe Pfad wie der Aktionsdruck,
re15_aot_fire_slot), danach A je 2 s.
| Fall | Lauf | Kiste (debug.log) | Inventar (debug.log) |
|---|---|---|---|
| K2 10D0 Leon | ctest C/D | `K2 ... NICHT im Inventar`, id=8 "You have not the Blue Keycard to operate it." | id=9 "You've used the Blue Keycard." |
| K2 10D1 Elza | `k2eb` | id=7 (Leser-Frage), `K2: Item 0x38 NICHT im Inventar`, id=8 "You have not the Blue Keycard..." | (Unit) |
| K3 1230 | `k3b`/`k3i` | id=7, `K3: Item 0x37 NICHT im Inventar`, id=8 "You have not the Red Keycard..." | `K3 ... im Inventar`, id=9 "You've used the Red Keycard." |
| K4 11E1 Elza | ctest E | id=8 "You have not the Yellow Keycard..." | (Unit) |
| K5 11E1 Elza | `k5b`/`k5i` | `K5: Item 0x30 NICHT im Inventar`, nur id=10 "If I had some kind of pliers..." | id=10, id=11 "Will you use the Pliers?" |
| K6 1100 | `k6b`/`k6i` | `K6 ... NICHT`, nur id=1 "It's a control panel. A type of voice device is required..." | id=1, id=3 "Will you use the Minidisc Player w/ Disc?" |
| K7 3010 / 3011 Elza | ctest G / `k7eb`/`k7ei` | id=0 "It's locked.An ID card is required to open it." | 3011: id=1 "You've used the Green Keycard." |
| K8 3050 | `k8b`/`k8i` | `K8 ... NICHT`, id=3 (Raumtext), es startet KEIN Ereignis-Faden | `K8 ... im Inventar`, Ereignis-Faden `thread start slot=10 first_op=0x2B`, id=3 |
| K9 4000 | `k9b` | beide Stellen `@0x01128` und `@0x01374`: `NICHT im Inventar`, id=0 "The door is locked..." | `k9i`: beide `im Inventar -> wie Original` |
| K10 4030 / 4031 Elza | ctest H / `k10eb` | id=0 "Main Fuse ..." | (Unit) |
| K11 1211 Elza | ctest F + `e1`/`e2`/`e3` (ECHTER Tuerweg 11E1 -> 1211) | `e1`: Box-Schirm, Tuer, `K11: Item 0x42 NICHT im Inventar (Item-Box Platz 2)`, id=10 "It's locked.It seems that there is someone inside..."; `e2` (Kiste, Platz 6 gefeuert): KEIN Plc_dest, keine Befreiung | `e3`: `K11 ... im Inventar`, Platz 6 -> `thread start slot=10 first_op=0x22`, Elza laeuft zur Zelle (Plc_dest -20650,-25450) = Befreiung beginnt |

Zu `e2`: RE15_FIRE_AOT feuert auch einen INAKTIVEN Platz (hier Platz 6 mit Ereignis 0 -> sub00, `first_op=0x06`). Das
liegt am Messhaken und nicht am Spiel. Im Spiel kann man einen inaktiven Platz nicht ausloesen.

### 2.4 Zensus und Entscheidung je Fall — selbst nachgeprueft
* **20 Tabellenzeilen gegen die ausgelieferten RDTs** (eigenes Python, nicht der Riegel): an jedem Offset steht
  `21 bank bit wert`, und das gilt fuer alle 20 Zeilen (z.B. ROOM1090 @0x0230A `06 00 24 00 | 21 03 85 00 | 2c 02 01 b1`).
  Die "fertig"-Saetze an den zitierten Offsets stimmen ebenfalls, alle 12 mit OK: 1090 @0x0271E `22 03 81 01`,
  10D0 @0x0152A `22 03 32 01`, 11E0 @0x01A8A `22 03 8b 01` / @0x01FC6 `22 04 24 01`, 1100 @0x00C68 `22 04 e8 01`,
  3010 @0x02302 `22 03 3c 01`, 3050 @0x0260E `22 03 b3 01`, 4000 @0x01450 `22 03 20 01`, 4030 @0x006D2 `22 04 19 01`,
  1211 @0x01F76 `22 04 fc 01`, 1000 @0x00D0C `22 03 85 01`, 1240 @0x0055A `22 03 8b 01` (K4 bei Leon tot).
* **Item-Ids je Fall** gegen Zensus Abschnitt 1/2 (Item_aot_set-Platzierung + Taken-Bit + Ableitung): K1 0x31 tk(9,0x86)
  -> (3,0x85); K2 0x38 tk(9,0x34); K3 0x37 tk(9,0x88); K4 0x39 tk(9,0x8A); K5 0x30 tk(9,0x0A) -> (3,0x7F); K6 0x44
  tk(9,0x68) -> (3,0x67); K7 0x36 tk(9,0x55) -> (3,0x3B); K8 0x46 tk(9,0xB4) -> (3,0xB2); K9 0x47 tk(9,0x06) -> (3,0x65);
  K10 0x41 tk(9,0x56) -> (3,0x57). Das sind dieselben Ids wie in der Tabelle. Fuer K11 ist 0x42 die PORT-WAHL aus Runde 36
  (schluessel_r36.c setzt (4,0xEC) nur in Zustand 7 nach einem erfolgreichen `re15_inv_grant_stapeln_nutzer`,
  item_modal_common.c Z. 336-343; Roy 1011 gibt ihn im Port nicht mehr).
* **Elza-Gegenstuecke ohne Fall**: ROOM1091 und ROOM4001 enthalten weder `21 03 85` noch `21 03 65` (Byte-Suche:
  0 Treffer). K1 und K9 gibt es also nur bei Leon.
* **Ck ist der einzige Bit-Abfrage-Weg der Raumskripte**: 0x58 -> 0x8003fd54 (Tabelle 0x80074608) ist ein indirekter
  Bit-Test ueber das Feld 0x800b0fd0 (selbst disassembliert, `sltu` @0x8003fdc4 / `xor` @0x8003fdcc). Der eigene Walk
  ueber alle 240 RDTs mit scd_dump_room.py ergibt: 0x58 kommt 0 Mal vor, 0x59 2 Mal (ROOM1030 @0x02822 / ROOM1031
  @0x0290E `59 05 04 01`, Bank 5). Das stimmt mit Dossier 2.3 ueberein.
* **Text-Seite** (Zensus Abschnitt 5, gelesen): Jedes "use/used" mit einem Inventar-Gegenstand gehoert zu K1..K11. Crank,
  Time Bomb, Fuse (STAGE2) und Latch Key sind reine Skript-Bits ohne Inventar-Gegenstand. Der Port gibt Gegenstaende nur
  ueber das Item-Modal (grep `re15_inv_grant` in engine/src und platform/pc/src: einzige Aufrufstelle
  item_modal_common.c Z. 336). Kein Port-Modul liest eines der elf Besitz-Bits fuer eine Benutzung (grep `flag_get` auf
  alle elf Bits: 0 Treffer).
* **Box-Raeume und Benutzungs-Raeume sind getrennt** (re15_itembox.c `s_boxpoints` Z. 62-71: 1150/1151, 2010/2011,
  30A0/30A1, 30B0/30B1, 4010/4011, 5010/5011, 6020/6021, 6030/6031). Keiner dieser Raeume ist einer der 17
  Benutzungs-Raeume. Darum genuegt die Auswertung beim Raumstart.
* **K9 sub01 je Bild** (@0x01370 `06 00 1e 00` Ifel bis 0x01392): Die Regel sperrt nur `Evt_exec sub02`
  (Benutzungs-Ablauf). Der Rest von sub01 ab @0x01392 (Bank 5 / sub04) liegt ausserhalb dieses Blocks und bleibt
  unberuehrt.

**Urteil Punkt 1: erfuellt.** Die Regel sitzt an EINER Stelle (op_ck), gesteuert ueber eine Tabelle je Fall. Die
Benutzung geht nur mit dem Gegenstand im Inventar, sonst zeigt der Raum seinen eigenen Text, ohne Frage und ohne
Benutzung. So reagiert auch RE2 (2.5). Der Zensus umfasst alle Stages, Leon und Elza, und jede Entscheidung hat einen
Beleg. Der Befund 1090 ist vorher reproduziert und nachher behoben, auch ueber den echten Tuerweg und die echte Box.

### 2.5 RE-Gate (selbst disassembliert, richtige Binaerdatei)
1. **RE2 Keep_Item_ck** (info/re2leon/PSX.EXE): `re2_disasm.py table 0x800a7640` -> `0x800584f0`. Die Opcode-Tabelle
   beginnt bei 0x800a74c8 ([0] -> 0x800537e4); 0x800a74c8 + 0x5E*4 = 0x800a7640. @0x80058500 `lbu v1,1(v0)`,
   @0x8005850c `jal 0x800696cc`, @0x80058514 `nor` / @0x8005851c `srl v0,v0,31`. Bestaetigt.
2. **RE2 FUN_800696CC**: @0x800696d4 `addiu a2,a2,18092` (0x800d46ac, Zaehler), @0x800696e4 `lbu v0,19004(at)`
   (0x800d4a3c + i*4), @0x800696ec `beq v0,a0`, @0x80069708 `addiu v0,zero,-1`. Die Schleife zaehlt nur bis zum
   Zaehler und erreicht das Box-Feld nicht. Bestaetigt.
3. **RE1.5 Ck LAB_8003fcf4** (Tabelle 0x800744a8 + 0x21*4 = 0x8007452c -> 0x8003fcf4): @0x8003fd48 `sltu v0,zero,v0`,
   @0x8003fd50 `xor v0,v0,a1`. Der Haken in scd_vm.c sitzt vor `cond = (got!=0) ^ (value==0)` und wirkt damit auf beide
   Formen ==0 und ==1. Bestaetigt.
4. **RE1.5 0x5E** (0x800744a8 + 0x5E*4 = 0x80074620 -> 0x80042b04): @0x80042b44 `ori v0,zero,0x1`, liefert konstant 1.
   Bestaetigt.
5. **RE1.5 FUN_8004dfec** (Inventar-Suche, @0x8004e014 `addiu at,at,4268` = 0x800b10ac): ghidra1_V2.txt nennt XREF[2]
   @0x8004ebbc / @0x8004ec34. Die eigene Byte-Suche nach `fb 37 01 0c` (`jal 0x8004dfec`) findet in PSX.EXE nur die
   Datei-Offsets 0x3f3bc und 0x3f434. In DEBUG.BIN, TITLE.BIN und STAGE1..6.BIN gibt es 0 Treffer. Bestaetigt.
6. **RE2-Texte**: ROOM2080 @0x01724 `21 04 af 00 06 00 54 00 5e 53 2b 00 02 ...`, ROOM6150 @0x02D3C `21 04 9f 01`, und
   `5e 3f` steht bei @0x02D64. Die msg-Dateien room2080/msg/sub01 ("It's locked. A special kind of key is required.") und
   room6150/msg/sub00 ("I could open it if I had the key.") sind im Zeichensatz-Versatz lesbar. Bestaetigt.
* Im ganzen Code-Diff kommen "deferred/tunable/interim/for now/faithful/plausibel/TODO" nicht vor (grep: 0). Es gibt
  keinen getenv-Schalter im Engine-Code. Jede Tabellenzeile traegt ihren Datei-Offset, K11 0x42 ist als Port-Aufnahme
  (Runde 36) ausgewiesen.
* Der Fix erklaert den Befund. Ursache war `21 03 85 00` @0x0230A, das nur das Bit liest. Der Haken setzt genau an
  diesem Ck an, und die Gegenprobe q1basis gegen q1 zeigt bei gleicher Eingabe vorher 7/8/9/3 und nachher nur 7.

## 3. Punkt 2 — Zusammenhang Item-Box (RE2) / Feuerloescher Nachtrag 4/4b/4c (leer am Boden nur nach 'Yes', bei 'No' "Empty Fire Extinguisher")
* 'Yes': Lauf `q2` (Stand Q, Tuerweg) zeigt `[feuerloescher] Loescher weggeworfen -> liegt in ROOM1090` und
  `ROOM1090: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250, Zone Slot 6 Untersuchen msg 29`.
* 'No': ctest `integration_r37_feuerloescher` (selbst gefahren, 315,64 s Passed), Lauf `lauf_dbd69463_i`:
  `K1: Item 0x31 im Inventar -> wie Original`, 7/8/9, Rettung id=3/4, KEIN "Loescher weggeworfen", KEIN "liegt leer am
  Boden". Lauf `_j`: `[feuerloescher] Name Item 0x31: Empty Fire Extinguisher`.
* Box nach dem Loeschen: Laeufe M/N/O desselben Riegels sind gruen (Loescher in der Kiste -> nichts liegt in 1090). Der
  Unit-Riegel `raumstart` "K1 1090 FERTIG ((3,0x81)=1) + KISTE" ist gruen: das fertig-Bit hat Vorrang, die Regel sperrt
  nicht nachtraeglich.
* Item-Box selbst: `unit_room1150_itembox`, `unit_itembox_transfer`, `unit_itembox_screen` und
  `unit_r35_inventar1050_kiste` sind gruen.
**Urteil Punkt 2: erfuellt.** Die Q-Regel aendert am Verhalten der Spur I nichts, und die Item-Box arbeitet weiter wie RE2.

## 4. Tests (gezielt, keine volle Suite)
* `ctest -R "r37_kistenregel" --timeout 1800 -V`: **100% tests passed, 0 tests failed out of 5**. Die Teile:
  tabelle 54/0, raumstart 58/0, ablauf 23/0, spielstand 4/0, integration 267 s, alle 9 Laeufe gruen.
* Regression ueber die beruehrten Module und Raeume (`-j 3 --timeout 1800`; Ausdruck = der erste Teil des Bau-Ausdrucks
  + unit/integration_r36_elza_ + integration_keypad/keydoor + unit_r34n_d_adaruf_ + unit_r21_discard +
  unit_r26_panel_11f0 + unit_r34n_a_rolltor + unit_room1050_sicherung + unit_r36_kartenbits/kartenitems):
  **100% tests passed, 0 tests failed out of 106** (462 s). Darin u.a. integration_r37_feuerloescher,
  integration_r36_schluessel(_befreiung), integration_r36_elza_unlock, integration_r36_story_leon/elza,
  integration_flag_census, unit_1090_flame_out_pin, unit_r36_schluessel_zelle/befreiung, unit_r36_elza_unlock.
* Gegenprobe "vorher" an der Basis-exe (2.1). Die Riegel messen die Regel selbst, denn A und I waeren mit dem alten
  Verhalten rot.

## 5. Vertrag
* Pfade (`git diff 6f360fb3 --name-only`): analysis/befunde_runde37/Q_*, engine/src/kistenregel_r37.c (neu),
  include/re15_kistenregel_r37.h (neu), engine/src/scd_vm.c (+5 Zeilen Haken, gemeinsame Datei = kleiner Haken wie in
  VERTRAG 1.4 "scd_vm.c Ck-Pfad"), Tests unter tests/unit (+ probes/r37_kistenregel.cmake per GLOB) und
  tests/integration/*.cmake (neue Datei mit `cmake_minimum_required(VERSION 3.16)`), tools/r37_kistenregel/. Nicht
  beruehrt: release/, platform/android/, shared_assets/PSX/ und die Test-CMakeLists.
* Es gibt keine neuen Bank-9-Bits, keine Nachrichten-Ids, keine AOT-Plaetze und keine Ereignisnummern (der Code setzt
  nichts, er aendert nur den Ck-Lesewert). feuerloescher_r37.c ist unveraendert.
* Riegel anderer Spuren wurden nachgezogen (5 Stueck, Dossier 4.4). Jede Aenderung legt den Gegenstand zusaetzlich ins
  Inventar, was dem Spielzustand mit Item-Box entspricht. Pruefung O (unit_r36_schluessel_befreiung) erwartete bisher die
  Befreiung OHNE den Schluessel im Inventar. Das verbietet Auftrag Q jetzt ausdruecklich, deshalb ist die Umstellung
  folgerichtig (siehe H1).

## 6. Hinweise (keine Maengel)
* **H1 (Integration):** unit_r36_schluessel_befreiung Pruefung O ist absichtlich von "Szene bis zum Ende ohne Abfrage"
  auf "keine Befreiung, Platz 8 = msg 10" umgestellt (test_r36_schluessel.c Z. 621-631). Wer Runde 36 gegen Runde 37
  vergleicht, sieht dort eine gewollte Aenderung.
* **H2 (Grenze):** Die Regel wertet K1 und K5..K11 beim Raumstart aus, also dort, wo das Original auswertet. Das ist
  richtig, solange kein Box-Raum zugleich Benutzungs-Raum ist (heute getrennt, 2.4). Eine spaetere Spur, die eine Item-Box
  in einen der 17 Raeume stellt, muss das neu pruefen.
* **H3 (PSX):** Einen PSX-Bau gibt es nicht (kein PSn00bSDK). kistenregel_r37.c braucht stdio nur unter
  RE15_PLATFORM_PC. Das ist nicht gemessen, nur gelesen.
* Testluecke (gering, kein Mangel): integration_r37_kistenregel misst E/F/G/H nur mit der Kiste, ohne Gegenprobe
  "Inventar" an der exe; der Unit-Riegel deckt sie ab. K3/K5/K6/K8/K9 misst der Riegel nur auf Unit-Ebene; an der exe
  habe ich sie hier nachgemessen (2.3).

## 7. Maengel
Keine.
