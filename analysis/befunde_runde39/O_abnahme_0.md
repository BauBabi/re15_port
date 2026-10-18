# Runde 39 Spur O "parkplatz11c0" — Abnahme 0 (unabhaengig)

Datum: 2026-10-19. Baum `.claude/worktrees/r39_parkplatz11c0` (Zweig `r39/parkplatz11c0`, HEAD 27d587e1, Basis fa2c0e7e).
Pruefer: unabhaengige Abnahme 0 (kein Code geaendert). Massstab: AUFTRAG.md Nachtrag 9 (Wortlaut), VERTRAG.md.

Wortlaut des Nutzers:
> "Ah, eine Sache fehlt mir noch - bei room11C0, während der Kampf gegen die Gorillas noch läuft, soll bei Leon der Text
> kommen "I cannot leave Ada alone here with those monsters!". Wenn Leon versucht durch die Tür aus dem Raum raus zu gehen."

## Stand der Abnahme
- [x] Diff gelesen (31 Dateien seit der Basis fa2c0e7e; Code: parkplatz_r39.c/.h, gen/parkplatz_r39_programme.inc, 4 Haken)
- [x] Bau: `local_build.sh configure` + `build` -> LOCAL-BUILD-OK, "ninja: no work to do" (exe 23:19:53 juenger als alle
      Spur-Quellen; letzte Code-Aenderung 6bbc46d0 23:19:36); Generator `programme_bauen.py --pruefen` -> ok
- [x] Riegel der Spur: unit 7/7, integration 4/4 gruen (selbst gefahren)
- [x] Eigene Messungen an der exe: K ausgewertet, A2 (Gorillas frei/lebend), C (CONTINUE im Kampf), E (Elza); Pruefstaende
      "echt" (echter Kampfausgang) und "vorher/nachher" (Szene + Kampf identisch)
- [x] RE-Gate (6 Adressen selbst disassembliert, alle RDT-Zitate selbst gelesen)
- [x] Vertrag / Pfade / Nebenwirkungen
- [x] Modulgruppen 133/133, 11C0/11B0-Nachbarn an der exe (9 gruen, integration_r39_stimmen_11b0 2x rot -> M1)
- [x] Urteil: Nutzer-Punkt erfuellt; NICHT bestanden wegen M1 (Gate Tests/Nebenwirkungen)

## 1. Riegel der Spur (selbst gefahren, Spurbaum, gezielt)
- `ctest -R "^unit_r39_parkplatz11c0_"` -> `100% tests passed, 0 tests failed out of 7` (0,63 s).
  Ausgabe `kampf` gelesen: Bild 1 = SZENE, Slot 0 Text; Rueckgabe der Steuerung Bild 1043; Tuer -> msg 40, Stimme 1x,
  kein Raumwechsel; Leon nach 30 Bildern UP 2022 Einheiten weg; zweiter Versuch wieder msg 40; Kill-Bits -> ENDE im
  selben Bild, Slot 0 -> 0x1B (-25552,0,-25331), msg 7, Raumwechsel ROOM11B0.
- `ctest -R "^integration_r39_parkplatz11c0_" --timeout 1800` -> `100% tests passed, 0 tests failed out of 4`
  (K 139,6 s, W 56,6 s, N 143,2 s, E 45,4 s; Rechner unter Last anderer Spuren).

## 2. Eigene Messung an der exe (Nutzer-Punkt)
Scratchpad (nicht eingecheckt): `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad`
(im Folgenden SP).

### 2.1 Riegel-Lauf K selbst gefahren und ausgewertet (`re15_port/build/tests/integration/r39_parkplatz11c0_k_wd/lauf_15dfd837_k`)
Echter Weg: Speicherkarte 11B0 vor der Parkplatz-Tuer -> CONTINUE -> AKTION -> `[aot] DOOR FIRE slot=1 ... spawn=(-25279,0,17268)`
-> `[room] PC loaded room11c0.rdt` -> F6 `[parkplatz] ... Evt_exec sub02 -> Programm SZENE` -> msg 0..6 -> F1107 `letterbox closed
-> gameplay`. Zustands-Log (state.txt, nur Wechsel):
```
F1741 pad=8000 PL(-25016,16556 rot=3095 hp=100) msg(a=1 fsm=1 id=40 q=0) pf=FFFF0007   <- Druck an der Tuer: Text offen, Welt steht
F1804 pad=8000 ... msg(a=1 fsm=1 id=40) pf=FFFF0007                                    <- zweiter Druck: Text bleibt
F1867 pad=8000 ... msg(a=1 fsm=1 id=40) / F1868 msg(a=0 fsm=0 id=40) pf=00000007       <- dritter Druck schliesst, kein Wieder-Oeffnen
F1948..F2046 pad=0010 PL(-24958,16510) -> (-20083,12132)                              <- Leon laeuft weg = steuerbar
```
debug.log: F1741 `ROOM11C0 Tuer im Kampf -> msg 40 ...`, `[voice] ROOM11C0 msg 40: keine Aufnahme (synchro/STAGE1/room11c0/main40.wav)`;
KEIN `DOOR FIRE slot=0` bis F2207 `MESSHAKEN RE15_R39O_GORILLAS_TOT` -> F2207 `Programm ENDE` -> msg 7/8/9 -> `DOOR FIRE slot=0
rect=(-25100,17250,hw=2000,hh=1350) target_cut=1 spawn=(-25552,0,-25331)` -> `loaded room11b0.rdt`.
Framedump (RE15_WINDOW_SCALE=1, 320x240) F1865: Leon an der Parkplatz-Tuer (EXIT-Schild), Textfenster
"I cannot leave Ada alone here / with those monsters!" in RE1.5-Schrift, ohne Sprecherkopf; F1745 Schreibmaschine "I cann"
(SP `o_bilder/k_001865.png`, `o_bilder/k_001745.png`).

### 2.2 Eigener Lauf A2 — Gorillas LEBEND und FREI (kein RE15_R39O_GORILLAS_STARR, kein Kill-Haken)
Eigener Treiber (SP `abn0/lauf.cmake`, exe-Kopie `re15_pc_r39o_abn0_*`, Muster des Riegels), Karte wie K, Env
`RE15_R39_MESS_SPIELER_HP=100` (Spur-E-Messhaken: Leon-HP >= 100, sonst stirbt er unterwegs), `RE15_EXIT_AT=4200#11c0`,
Wegplan SP `abn0/plan_a2.txt` (Plc_dest-Punkte der Szene, dann 12x GEHE zur Tuer, BLICK 3072, 3x AKTION, Wiederholung).
(Vorlauf A ohne GEHE-Wiederholung: Leon kam wegen der Gorillas nicht bis zur Tuer, Slot 0 dabei durchgehend Text, kein DOOR FIRE.)
- Leon wird unterwegs mehrfach von den Gorillas festgehalten (`KLEMME` F1600..F1726), erreicht die Tuer F2101.
- `AOT slot 0 typ 5 ev 40 flags 0x31` bei jeder Liste (F1120..F2604); `FLAG (4,64)=1`, `(3,67)=0`, `(7,96)=0`, `(7,97)=0`.
- debug.log: F2161 `ROOM11C0 Tuer im Kampf -> msg 40` + Stimme angefragt; F2355 zweiter Versuch -> wieder msg 40; **kein
  `DOOR FIRE slot=0`**, Lauf endet `EXIT_AT: Bild 4200 in Raum 11c0`.
- state.txt: F2161 msg(a=1 id=40) pf=FFFF0007, Gorillas (-23302,14752)/(-23664,17935) stehen bis F2287 still (Welt friert wie bei
  jedem Untersuchungstext), F2288 Text zu, Gorillas laufen weiter; F2355..F2481 dasselbe.
- Framedump F2280 (SP `o_bilder/a2_002280.png`): ein Gorilla direkt vor der Tuer, Textfenster "I cannot leave Ada alone here /
  with those monsters!".

### 2.3 Eigener Pruefstand "echt" — echter Kampfausgang ohne Kill-Bit-Abkuerzung (Engine-Bibliothek des Baums)
Die Riegel der Spur beenden den Kampf nur per Abkuerzung (Kill-Bits direkt gesetzt). Ein ECHTER Abschuss muss die Tuer auch
freigeben, sonst saesse Leon nach dem Kampf fest. Pruefstand SP `abn0/pruef/abn0_echt.c` (bindet das Spur-Harness
test_r39_parkplatz11c0.c ein, gelinkt gegen `build/engine/libre15_engine.a`): echte Szene ab dem Tuer-Spawn, Tuer im Kampf
mit FREIEN Gorillas, dann Pistolen-Treffer mit dem Schreibsatz des Resolvers (wie test_r39_gegner_1141.c `treffer()`:
Waffe 3, Schaden 12) bis zum Tod — die Kill-Bits setzt nur der Todes-Commit der Engine. Ausgabe:
```
ok: Szene vorbei (Bild 1043), Slot 0 = Text msg 40
ok: vor dem Kampf: Kill-Bits (7,0x60)/(7,0x61) = 0
ok: Kampf (Gorillas frei): Tuer -> msg 40, kein Raumwechsel
    Gorillas: HP 360 / 360, em_flag 0x60 / 0x61
    Treffer bis Tod: 31 / 31 (Tod Bild 150 / 150), Kill-Bits Bild 221, ENDE Bild 222, Tuer Bild 222, Wechsel Bild 932 -> 11B0
ok: Kill-Bits (7,0x60)/(7,0x61) vom TODES-COMMIT der Engine gesetzt (Bild 221)
ok: Programm ENDE startet (Bild 222), Slot 0 im selben Bild wieder Tuer
ok: msg 7 (Ada: Okay, it is over now.)
ok: Ende-Szene geht durch die Tuer -> ROOM11B0 (Bild 932)
echt: 0 Fehler
```
=> Nach einem echten Kampf wird die Tuer frei, die Original-Ende-Szene laeuft und geht nach ROOM11B0. Kein Softlock.

### 2.4 Eigener Lauf C — CONTINUE mitten im Kampf IN ROOM11C0 (Boot-/CONTINUE-Installer in main.c, kein Riegel faehrt ihn)
Karte `11C0 -25100 16600 3072 flag:4:0xF3 flag:3:0x82 flag:9:93 flag:4:64`, Env `RE15_R39O_GORILLAS_STARR=1`,
`RE15_EXIT_AT=420#11c0`, Wegplan SP `abn0/plan_c.txt` (AOTS, FLAG, BLICK 3072, 3x AKTION, AOTS):
`[save] CONTINUE: resumed in room 11c0` -> `[parkplatz] F0 ROOM11C0: Kampf laeuft beim Eintritt ((4,64)=1, (3,67)=0) -> Tuer
Slot 0 = Text msg 40` (genau einmal) -> F31 `AOT slot 0 typ 5 ev 40 flags 0x31` -> F36 `Tuer im Kampf -> msg 40` + Stimme
angefragt -> kein `DOOR FIRE`, `EXIT_AT: Bild 420 in Raum 11c0`.

### 2.5 Elza (Riegel E selbst gefahren, `r39_parkplatz11c0_e_wd/lauf_7f721e5f_e`)
`[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, CONTINUE 11B1 -> `DOOR FIRE slot=1` -> `loaded room11c1.rdt` (5 Hunde 0x20),
`AOT slot 0 typ 1 ev 0 flags 0x31` (Tuer), kein `[parkplatz]`, `DOOR FIRE slot=0 ... spawn=(-25552,0,-25331)` -> `loaded room11b1.rdt`.

### 2.6 Eigener Vergleich vorher/nachher (Szene und Kampf unveraendert?)
Die Behauptung "state.txt vorher/nachher bitgleich (1738/1738, 2746/2746)" stuetzt sich auf eine Basis-exe, die im Spurbaum
nicht liegt. Eigener Nachweis auf Engine-Ebene: derselbe Pruefstand (SP `abn0/pruef/abn0_vergleich.c`: echte Szene ab dem
Tuer-Spawn, dann Kampf mit FREIEN Gorillas, 3000 Bilder, Zustand je Bild = Spieler x/y/z/Blick/HP/Clip/Bild, Nachricht,
Flags (4,64)/(3,67)/(7,0x60)/(7,0x61)/(2,7), Kamera, jeder Aktor mit Typ/Lage/Zustaenden/HP/Clip/Bild/grid) zweimal gelinkt:
"nachher" normal, "vorher" mit Durchreichern fuer alle `re15_parkplatz_r39_*`-Symbole (SP `abn0/pruef/abn0_stummel.c`; das
Bibliotheksobjekt parkplatz_r39.o wird dann nicht gezogen — die "vorher"-exe enthaelt seinen Log-Text nicht).
Ergebnis: `cmp z_vorher.txt z_nachher.txt` -> **identisch** (3000/3000 Zeilen); am Ende Slot 0 vorher `typ 1 ev 0` (Tuer),
nachher `typ 5 ev 40` (Text). Die Gosub-Huelle aendert Szene und Kampf also nicht; der einzige Unterschied ist die Tuer.

### 2.7 Querbezug: `integration_r39_stimmen_11b0` (Spur A, in master; A_stimmen.md:260 `Passed 102,39 s`) haengt mit Spur O in ROOM11C0
Der Riegel der Spur A (tests/integration/test_r39_stimmen.cmake:96-98) startet per Karte IN ROOM11C0 mit den Flags
`3:0x43 3:0x82 4:0xf3` — also (3,67)=1, aber (4,64)=0 — und feuert die Tuer mit `RE15_FIRE_AOT=0@60#11c0`, um zur
11B0-Schlussszene zu kommen. Der Bau-Agent hat ihn nicht gefahren (seine 11C0-Liste: 6 Tests ohne diesen).
Selbst gefahren (Spurbaum, `-R "^integration_r39_stimmen_11b0$"` innerhalb des Nachbar-Laufs), debug.log
`re15_port/build/r39_stimmen_wd/lauf_11b0_c855dff5/debug.log`:
```
[save] CONTINUE: resumed in room 11c0 (hp=100)
[parkplatz] F0 ROOM11C0 sub01 @0x01824 Evt_exec sub02 -> Programm SZENE (Tuer Slot 0 -> Text msg 40, Gosub sub02)
[parkplatz] F60 ROOM11C0 Tuer im Kampf -> msg 40 "I cannot leave Ada alone here with those monsters!" (Stimme main40.wav)
...  (kein DOOR FIRE, kein loaded room11b0; 6 min spaeter steht der Lauf noch in 11C0: [walk] F9120 pl pos=(-16072,0,12478))
```
ctest: `8/8 Test #1009: integration_r39_stimmen_11b0 ......***Failed  600.39 sec` — `r39_stimmen_11b0: Text msg 20 fehlt im Log:
Marvin: But, hold on\.\.\.` (test_r39_stimmen.cmake:179); Summe des Nachbar-Laufs `88% tests passed, 1 tests failed out of 8`.
Einzeln nachgefahren (`-R "^integration_r39_stimmen_11b0$"`, ohne Parallel-Last aus diesem Baum): identisch
`1/1 Test #1009: integration_r39_stimmen_11b0 .....***Failed  600.38 sec`, dieselbe Meldung; debug.log
`re15_port/build/r39_stimmen_wd/lauf_11b0_328f3a09/debug.log` wieder `[parkplatz] F0 ... Programm SZENE` und
`F60 ... Tuer im Kampf -> msg 40` — deterministisch, keine Last-Folge.
Ursache (selbst gemessen, Pruefstand SP `abn0/pruef/abn0_inkons.c`, gleiche Flags, einmal mit, einmal ohne Spur O gelinkt):
```
== nachher (mit Spur O)
Start: (4,64)=0 (3,67)=1, Slot 0 typ 1 ev 0
F60 (wie RE15_FIRE_AOT=0@60): Slot 0 typ 5 ev 40, (4,64)=1
Szene vorbei (Bild 1048): Slot 0 typ 5 ev 40
nach 6000 Kampfbildern mit Treffern: Kill-Bits (7,0x60)=1 (7,0x61)=1, ENDE-Zaehler 0, Slot 0 typ 5 ev 40
Druck an der Tuer: msg 40, Raumwechsel 0
== vorher (ohne Spur O)
... Slot 0 typ 1 ev 0 ... Druck an der Tuer: msg -1, Raumwechsel 1 -> 11B0
```
Das Programm SZENE sperrt beim Evt_exec sub02 OHNE Blick auf (3,67). Ist (3,67) schon 1, kann die einzige Freigabe — sub01
@0x0182E `21 03 43 00` Ck(3,67)==0 -> @0x01842 Evt_exec sub03 -> Programm ENDE — nie mehr greifen: die Tuer bleibt fuer den
ganzen Besuch Text, auch nachdem beide Gorillas tot sind (die einzige Tuer des Raums; Leon sitzt fest). Die eigene
Definition der Spur ("Kampf laeuft" = (4,64)=1 && (3,67)=0, `re15_parkplatz_r39_kampf`, Installer) gilt fuer SZENE nicht.
Im echten Spielablauf ist (4,64)=0 bei (3,67)=1 nicht erreichbar (nur ROOM11C0 setzt beide Bits, sub02 @0x0184E vor sub03
@0x01B02; kein RDT-Skript und kein Port-Code loescht sie — RDT-Zensus und grep selbst gemacht); erreichbar ist er ueber
Spielstaende/Debug-Flags wie im Spur-A-Riegel. Folge fuer die Runde: ein gruener master-Riegel wird mit Spur O rot.

## 3. RE-Gate (selbst disassembliert / selbst gelesen)
Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf `info/Re1.5/PSX.EXE` und `PSX/BIN/STAGE1.BIN` des Baums.

| Zitat | selbst gelesen | Ergebnis |
|---|---|---|
| Aot_on LAB_800407bc, `jalr` @0x8004082c | @0x800407ec rec = DAT_800ac9b0[pc[1]], @0x8004080c `lbu v0,0(v1)` rec[0], @0x8004081c Tabelle 0x8007469c, @0x8004082c `jalr v0` | bestaetigt: ruft den Handler, den der Slot JETZT hat -> die Ruecknahme muss vor sub03 @0x01C08 stehen |
| Aot_reset LAB_80040738 | @0x80040764 `sb v0,0(a1)` (sce = pc[2]), @0x8004076c-78 (rec[1] & 0x80) + pc[3], @0x80040788-a8 drei Halbworte, @0x800407ac pc += 10 | bestaetigt |
| sce-Tabelle PTR_8007469c | [1] = 0x80043084 (@0x8004309c msg, @0x80043098 Maske, @0x800430a4 `sll a3,a3,16`, @0x800430a0 `jal 0x80027e68`); [2] = 0x800430bc (@0x800430c4 Nutzlast -> 0x800ac9a8, @0x800430d4 0x800b5359 = 1, g_pauseflags OR 0xFF000000) | bestaetigt |
| Kill-Bits | Sce_em_set: @0x800420f4 `addiu s2,a1,2`, @0x80042120 `lbu a1,7(a1)` (pc[7]), @0x80042170 `sb v0,454(s0)` (s2[5] = pc[7] -> +0x1C6); Bank a3 = 0x800ac77c + 18620 = **0x800b1038** (die Werkzeug-Annotation "0x800b48bc" ist falsch gerechnet); Gorilla-Tod STAGE1 @0x8011b8e8 `lbu a1,454(v0)`, @0x8011b8f0 a0 = 0x800b1038, @0x8011b8f4 `jal 0x8004ef90` (@0x8004ef90-b4 Bit setzen); Flag-Tabelle @0x80074664[7] = 0x800b1038 | bestaetigt: (7,0x60)/(7,0x61) = Gorilla 1/2 tot |
| Raum-Variante @0x800397e4 | `srl a0,a0,31` + @0x800397ec `addu a0,a0,v0` | passt zu "Elza = +1" |
| ROOM11C0.RDT | main00 @0x01712 (einzige Tuer, 25 B wie zitiert); sub00 @0x01770/@0x01784/@0x01798/@0x017C8/@0x017D0; sub01 @0x01820..@0x01842; sub02 @0x0184E/@0x01856/@0x01ACE/@0x01ABA/@0x01A58; sub03 @0x01B02/@0x01B68/@0x01C08; Sub-Tabelle @0x1758 (8 Subs, sub02 @0x184E, sub03 @0x1B02); kompletter Dump `tools/scd_dump_room.py`: genau 1 Door_aot_set, genau 1 Aot_on (sub03), kein Aot_reset/Aot_set auf Slot 0 | alle Bytes wie zitiert |
| Form-Belege | ROOM11B0 @0x01216 `46 01 01 31 0f 00 ff ff 00 00`, @0x00FAA Tuer -> 11C0 Spawn (-25279,0,17268) Blick 0x11FB; ROOM11B1 @0x011C0 `46 01 01 31 0e 00 ff ff 00 00` / @0x011A0 `21 04 f3 00`; ROOM1141 @0x00FDC `46 01 02 31 4f aa 00 00 8f 9b`; ROOM1090 msg 7 @0x28E6 `04 02 25 00 49 51 ...`; RE2 ROOM7040 @0x0185A `46 00 04 31 01 00 00 00 ff ff`, @0x01ACC `46 00 01 31 f4 98 00 00 c9 a5`, @0x010FE Door_aot_set Nutzlast `f4 98 00 00 c9 a5`, @0x01ADF `22 02 07 00`, msg @0x01D1A `fa 02 25 00 3f 3d 4a 3a 50 ..` (I can t ...); RE2 ROOM6010 @0x0068A, @0x015C9 | alle Bytes wie zitiert |

- Konstanten in `include/re15_parkplatz_r39.h`: jede traegt @0x-Adresse/RDT-Offset oder VERTRAG-Bezug (Id 40 = VERTRAG 1.2);
  Text = NUTZER-VORGABE (woertlich), Form belegt (ROOM1090 msg 7, 1141 msg 41). Stimme = PORT-WAHL (gekennzeichnet).
- Rate-Wort-Scan ueber `git diff master...HEAD -- re15_port` (deferred, tunable, interim, for now, faithful, plausib, TODO,
  FIXME, ungefaehr, geschaetzt, vermutlich, approx, guess): **0 Treffer**. Env-Schalter nur als Messhaken (RE15_R39O_*), nicht
  als Abschluss; Spielverhalten ohne Env.
- Erklaert der Fix den Befund? Ja: vorher stand in Slot 0 waehrend des Kampfes die Original-Tuer (main00 @0x01712, keine Sub
  aendert sie — selbst im Dump geprueft), der Fix tauscht sie per Aot_reset fuer genau die Original-Kampfspanne
  ((4,64)=1 und (3,67)=0) gegen MESSAGE msg 40 und vor sub03 @0x01C08 zurueck.
- Generator: `programme_bauen.py --pruefen` -> `...parkplatz_r39_programme.inc ok` (prueft 17 RDT-Stellen + Tuer-Zensus).

## 4. Vertrag, Pfade, Nebenwirkungen
- Pfade (`git diff master...HEAD --name-only`, 31 Dateien): analysis/befunde_runde39/O_*, engine/src/parkplatz_r39.c,
  engine/src/gen/parkplatz_r39_programme.inc, include/re15_parkplatz_r39.h, tools/r39_parkplatz/programme_bauen.py,
  tests/unit/test_r39_parkplatz11c0.c, tests/unit/probes/r39_parkplatz11c0.cmake (Sonden-Muster je Thema),
  tests/integration/test_r39_parkplatz11c0.cmake + drei Haken-Dateien. KEIN release/, platform/android/, shared_assets/PSX/,
  keine Test-CMakeLists, kein synchro/. (Die Loeschungen in `git diff master` — AUFTRAG.md, UEBERGABE.md, r39.js,
  synchro/STAGE1/room1041/main40.wav — sind nur master-Fortschritt nach der Basis, nicht Teil der Spur.)
- Gemeinsame Dateien nur kleine Haken mit Kommentar "Runde 39 Spur O": scd_vm.c +4 (Include, op_evt_exec hinter dem
  Runde-38-F-Haken, re15_scd_show_message vor re15_dialog_open_mask), scd_room_setup.c +2 (Include, Installer am Blockende),
  platform/pc/main.c +3 (Include, Boot-/CONTINUE-Installer, Messhaken). Haken-Kette in op_evt_exec: nur 1141 (Runde 38 F)
  und 11C0 (Spur O) — kein anderes Modul ersetzt sub02/sub03 von 11C0.
- Bank-9: keines belegt (Bit 96 bleibt frei; Original-Flags (4,64)/(3,67)/(7,0x60)/(7,0x61) reichen) — k_port[] unberuehrt.
- Nachrichten-Id: 11C0 Id 40 (VERTRAG 1.2); sonst belegt in 11C0 nur 0..9 (RDT) und 10..14 (irons_tod_1150.c:699-701) — frei.
- Stimme: Haken nur fuer ROOM11C0 msg 40. Lader (audio_pc.c) versucht zuerst `synchro/STAGE1/room11C0/` (Grossschreibung, wie der
  Ordner der Nutzer-Aufnahmen main00..09), dann `room11c0/`; der Log nennt nur den letzten Versuch. Eine kuenftige
  `synchro/STAGE1/room11C0/main40.wav` wird damit auch auf Linux gefunden.
- Kein Doppelstart: Evt_exec-Spur K = je 1x sub=2 (F6, SZENE) und sub=3 (F2207, ENDE); A2 = 1x sub=2.
- Elza: Riegel E (ROOM11B1 -> 11C1: `AOT slot 0 typ 1`, kein `[parkplatz]`, Tuer -> 11B1) und unit elza gruen.
- Testqualitaet: unit verlinkt `m` ausser WIN32 (sqrt); Integrationsskript mit cmake_minimum_required, exe-Kopie je Teil
  (`re15_pc_r39o_<TEIL>_<id>`, LAST_EXT), eigener Arbeitsordner je Teil, Env des Aufrufers geloescht; Stimmen-Regex
  `room11[cC]0` (Linux-fest).

## 5. Gezielte Laeufe der beruehrten Module (Spurbaum, KEINE volle Suite)
- Modulgruppen (Regex des Bau-Agenten + 11B0-Nachbarn `^unit_r38_werkstatt11b0_|^unit_marvin_|^unit_sherry_wall_pin$`,
  `-E "^integration_" -j 4`): **`100% tests passed, 0 tests failed out of 133`** (darin unit_scd_*, unit_aot_*, unit_msg_dialog,
  unit_r35_affen_szene, unit_r39_gegner_1141_ablauf1141 = echter Gorilla-Tod -> Kill-Bits in 1141, unit_r38_dining1141_*).
- 11C0/11B0-Nachbarn an der exe (`-j 2`): integration_r37_szene11c0 Passed 135,8 s, integration_r38_montage 115,1 s,
  integration_r36_szene1150 125,4 s, integration_r36_schluessel 124,0 s, integration_r39_gegner_schatten 67,5 s,
  integration_r35_cut1150 575,4 s, integration_r38_werkstatt11b0 175,0 s, dazu einzeln integration_r35_granate (Lauf
  "raute" in 11C0) 165,1 s und integration_r35_cut10f0 (Teil E endet in 11C0) 242,9 s — gruen; **integration_r39_stimmen_11b0 haengt in
  ROOM11C0 (Abschnitt 2.7, Mangel M1)**.
- Weitere Nebenwirkung geprueft: Kartenmarker-Zug (re15_inv_screen.c `tuer_anziehen`, nur DOOR-Slots auf Grundriss-Blaettern)
  greift waehrend der Sperre an der 11C0-Tuer nicht — kosmetisch, wie bei jeder Original-Text-Tuer (z. B. 11B0 Slot 1 ohne
  Strom); kein Befund.

## 6. Urteil je Nutzer-Punkt
| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| ROOM11C0, waehrend der Gorilla-Kampf laeuft, bei Leon der Text "I cannot leave Ada alone here with those monsters!", wenn Leon versucht, durch die Tuer hinauszugehen | **erfuellt** | K (selbst gefahren): F1741 Druck -> msg 40, kein DOOR FIRE bis zum Kampfende, Leon laeuft danach weg; A2 (eigener Lauf, Gorillas LEBEND und FREI): F2161/F2355 Druck -> msg 40, kein DOOR FIRE bis EXIT_AT F4200; C (CONTINUE mitten im Kampf in 11C0): F36 -> msg 40; Bild F1865/F2280: Textfenster "I cannot leave Ada alone here / with those monsters!" an der Parkplatz-Tuer; Text dekodiert woertlich (unit texte); nach dem Kampf geht die Tuer wieder (Riegel N, K-Ende, eigener Pruefstand "echt" mit echten Pistolen-Treffern: Kill-Bits vom Todes-Commit -> ENDE -> msg 7 -> ROOM11B0); Elza 11C1 unberuehrt (E); Szene und Kampf vorher/nachher identisch (2.6). Nebenwirkung M1 betrifft einen im Spielablauf nicht erreichbaren Flag-Zustand und einen fremden Riegel, nicht den Wortlaut des Punkts |

## 7. Gates
- RE-Gate: **haelt** (Abschnitt 3; 6 Adressen + alle RDT-Zitate selbst gelesen, 0 Rate-Woerter, NUTZER-VORGABE/PORT-WAHL
  gekennzeichnet, Fix erklaert den gemessenen Befund).
- Vertrag (Ressourcen/Pfade): **haelt** (Abschnitt 4; Id 40, kein Bank-9-Bit, nur erlaubte Pfade, Haken 2-4 Zeilen).
- Tests/Nebenwirkungen: **haelt NICHT** — Spur-Riegel 7 + 4 gruen, Modulgruppen 133/133, neun 11C0/11B0-Nachbarn gruen, aber
  `integration_r39_stimmen_11b0` (Spur A; dort laut A_stimmen.md:260 `Passed 102,39 s`) haengt mit Spur O in ROOM11C0 (M1). Der Querbezug-Lauf des
  Bau-Agenten umfasste nur unit_/prop_/probe_ und sechs 11C0-Integrationen ohne diesen Riegel.

## 8. Maengel
- **M1 Programm SZENE sperrt auch bei bereits gesetztem (3,67) -> Tuer dauerhaft gesperrt; master-Riegel
  `integration_r39_stimmen_11b0` haengt.** Ort: `re15_port/engine/src/parkplatz_r39.c` `re15_parkplatz_r39_evt_exec` (Zweig
  `sub_id == RE15_R39O_SUB_SZENE`, gibt k_r39o_p_szene ohne Flag-Pruefung zurueck). Nachpruefbar: (a) `ctest --test-dir
  re15_port/build -R "^integration_r39_stimmen_11b0$"` -> debug.log `[parkplatz] F0 ... Programm SZENE`, `F60 ... Tuer im
  Kampf -> msg 40`, kein `loaded room11b0`; (b) Pruefstand SP `abn0/pruef/abn0_inkons.c` (Abschnitt 2.7): mit (4,64)=0/(3,67)=1
  bleibt Slot 0 nach dem Tod beider Gorillas `typ 5 ev 40`, ENDE-Zaehler 0, Druck -> msg 40 ohne Raumwechsel.
  Beleg der Freigabe-Bedingung: sub01 @0x0182E `21 03 43 00` (Ck(3,67)==0) ist der einzige Weg zu sub03/ENDE; sub00 @0x017D0
  definiert den laufenden Kampf mit derselben Bedingung. Vorschlag: SZENE nur, wenn (3,67)==0 (wie
  `re15_parkplatz_r39_kampf`/Installer), sonst `orig` zurueckgeben; Riegel fuer (4,64)=0/(3,67)=1 (keine Sperre, Tuer ->
  ROOM11B0) und `integration_r39_stimmen_11b0` danach gruen messen. Unabhaengig davon kann der Orchestrator die Karte des
  Spur-A-Riegels in den widerspruchsfreien Nach-Kampf-Zustand bringen (zusaetzlich `4:64`) — das ist Spur A's Datei.
  Vorschlag selbst gegengeprueft (nur Scratchpad, Baum unveraendert): Kopie von parkplatz_r39.c mit der einen Zeile
  `if (sub_id == RE15_R39O_SUB_SZENE && re15_game_flag_get(RE15_R39O_ENDE_BANK, RE15_R39O_ENDE_BIT)) return orig;`
  (SP `abn0/pruef/parkplatz_r39_vorschlag.c`) vor die Bibliothek gelinkt: Zustand (4,64)=0/(3,67)=1 -> Slot 0 bleibt
  `typ 1`, Druck -> Raumwechsel ROOM11B0; normaler Kampf (Pruefstand "echt") -> Text im Kampf, ENDE, msg 7, ROOM11B0,
  `echt: 0 Fehler`.

## 9. Hinweise (ausserhalb der Spur O, kein Abnahme-Mangel)
- **H1 Kill-Bit-Zeitpunkt im Port spaeter als im Original.** Original: der Gorilla-Bodentod setzt das Kill-Bit im ERSTEN
  Todesbild — STAGE1.BIN @0x8011b7c8 `lbu v1,7(a1)` (sub_state_3), @0x8011b7e0 `beq v1,zero,0x8011b804` (Fall 0),
  @0x8011b804..@0x8011b8f4 (Clip 0xe, Gore, Se(0), `jal 0x8004ef90`), Fall-Durchlauf in Fall 1 @0x8011b8fc. Port: Commit erst im
  Leichen-Zustand 7 (enemy_ai_common.c run_all `e->state == RE15_AI_STATE_CORPSE` -> `re15_game_flag_set(re15_em_status_zone(),
  e->em_flag_id, 1)`, seit b5ef9204 vom 2026-08-25). Gemessen im Pruefstand "echt": Tod Bild 150, Kill-Bits Bild 221 = 71 Bilder
  spaeter. Folge in 11C0: die Original-Ende-Szene (sub03) beginnt ~2,4 s spaeter als im Original, und so lange zeigt die Tuer
  nach dem Tod des zweiten Gorillas noch msg 40. Fuer Typ 0x27 hat der Port keinen frueheren Commit (der Todesfall-Code
  enemy_ai_common.c `case 3` verweist nur per Kommentar auf den run_all-Commit); derselbe Weg gilt damit auch fuer die
  Gorillas in 1141 (dort nicht nachgemessen). Zustaendigkeit Gegner-KI (Spur E/Orchestrator), nicht Spur O.
- **H2 Riegel ohne echten Abschuss.** Alle Spur-Riegel beenden den Kampf per Abkuerzung (Kill-Bits direkt). Ginge der
  Todes-Commit fuer Typ 0x27 einmal verloren, saesse Leon nach dem Kampf in 11C0 fest (Tuer bliebe Text) — kein Spur-O-Riegel
  wuerde das melden. Heute deckt das unit_r39_gegner_1141_ablauf1141 (derselbe Commit-Weg, Typ 0x27, Stage 1) und mein
  Pruefstand (SP `abn0/pruef/abn0_echt.c`). Empfehlung: bei der Integration einen Teil "echt" in unit_r39_parkplatz11c0
  (echte Treffer bis zum Tod, Kill-Bits NICHT setzen, Tuer frei + ROOM11B0 pruefen).
- H3 Beobachtung (nicht gegen das Original geprueft): ein Druck waehrend der Schreibmaschine (K: F1804) beschleunigt den
  Text nicht (F1805 noch "...with thos"); geschlossen wird er erst mit dem Druck F1867. msg 40 laeuft ueber denselben
  MESSAGE-AOT-Weg wie jede Port-Untersuchungsmeldung (re15_scd_show_message -> re15_dialog_open_mask) — allgemeines
  Verhalten der Text-FSM, nicht Spur O.
