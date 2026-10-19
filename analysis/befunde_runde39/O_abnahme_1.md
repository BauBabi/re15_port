# Runde 39 Spur O "parkplatz11c0" — Abnahme 1 (unabhaengig)

Datum: 2026-10-19. Baum `.claude/worktrees/r39_parkplatz11c0` (Zweig `r39/parkplatz11c0`, HEAD 2c7498cd, Basis fa2c0e7e).
Pruefer: unabhaengige Abnahme 1 (kein Code geaendert). Massstab: AUFTRAG.md Nachtrag 9 (Wortlaut), VERTRAG.md,
Abnahme 0 (`O_abnahme_0.md`, Mangel M1). Scratchpad (nicht eingecheckt, im Folgenden SP):
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\abn1`.

Wortlaut des Nutzers:
> "Ah, eine Sache fehlt mir noch - bei room11C0, während der Kampf gegen die Gorillas noch läuft, soll bei Leon der Text
> kommen "I cannot leave Ada alone here with those monsters!". Wenn Leon versucht durch die Tür aus dem Raum raus zu gehen."

## Stand der Abnahme (fortlaufend)
- [x] Diff gelesen (`git diff master...HEAD`, 37 Dateien; Code seit Abnahme 0: nur parkplatz_r39.c +10 Zeilen, Kommentare
      in re15_parkplatz_r39.h / programme_bauen.py; Rest Riegel + Dossier/Belege)
- [x] Bau: `local_build.sh configure` + `build` -> LOCAL-BUILD-OK, "ninja: no work to do"; exe 01:33:34 juenger als
      parkplatz_r39.c (01:31:06); exe und Riegel-exe enthalten den neuen Logtext "bei (3,67)=1 -> keine Sperre";
      `programme_bauen.py --pruefen` -> ok; Arbeitsbaum sauber
- [x] Riegel der Spur selbst gefahren (unit 10/10, integration K/W/N/E/I + integration_r39_stimmen_11b0 6/6)
- [ ] Eigene Messungen an der exe
- [x] RE-Gate (6 Stellen selbst: RDT-Freigabezweig + Roh-Zensus, @0x8006ee44, @0x8003f2a0/@0x8003ee3c/@0x8003f0a0, STAGE1 @0x8011b8f4, @0x8004082c)
- [x] Vertrag / Pfade / Nebenwirkungen (Modulgruppen 136/136; Probe-Merge: 1 trivialer Konflikt mit Spur K)
- [ ] Urteil

## 1. Riegel der Spur (selbst gefahren, Spurbaum, gezielt)
- `ctest --test-dir re15_port/build -R "^unit_r39_parkplatz11c0_" --timeout 600 --output-on-failure`
  -> `100% tests passed, 0 tests failed out of 10` (0,56 s). Ausgaben der neuen Teile selbst gelesen
  (`test_r39_parkplatz11c0.exe <teil>`):
  - `ende_gesetzt`: "erstes Bild: KEIN Programm SZENE", "Bild 0..59 der Szene: Slot 0 durchgehend die Original-Tuer",
    "Aot_on(0) in Bild 60: Raumwechsel ROOM11B0, kein Text msg 40", "Steuerung zurueck ... in Bild 1047", "Gorillas tot
    (Kill-Bits ...): kein Programm ENDE", "Druck an der Tuer: Raumwechsel ROOM11B0, kein Text" -> `0 Fehler`.
  - `kills_gesetzt`: "Bild 0: sub01 startet SZENE (@0x01824) und ENDE (@0x01842) im selben Durchlauf", "am Bildende ist
    Slot 0 die Tuer", "Ende-Szene: msg 7, kein Text msg 40, Aot_on(0) @0x01C08 -> ROOM11B0" -> `0 Fehler`.
  - `echt`: "Treffer bis Tod: 31 / 31 (Tod Bild 150 / 150), Kill-Bits Bild 221, ENDE Bild 222, Tuer Bild 222, Wechsel
    Bild 932" -> `0 Fehler` (dieselben Bilder wie der Pruefstand der Abnahme 0, 2.3).
  - `programme`: zusaetzlich "Evt_exec-Haken bei (3,67)=1: sub02 bleibt das Original (keine Sperre; Abnahme 0 M1)".
- `ctest --test-dir re15_port/build -R "^integration_r39_parkplatz11c0_|^integration_r39_stimmen_11b0$" --timeout 1800 -j 2
  --output-on-failure` -> `100% tests passed, 0 tests failed out of 6` (i 91,46 s, stimmen_11b0 104,26 s, k 123,66 s,
  n 121,02 s, w 47,60 s, e 37,69 s; Rechner unter Last zweier anderer Spur-Abnahmen). Mitschrift SP `ctest_integration_spur.txt`.
  - **`integration_r39_stimmen_11b0` (Abnahme 0: 2x `***Failed 600 s`) jetzt `Passed 104,26 s`.** debug.log
    `re15_port/build/r39_stimmen_wd/lauf_11b0_95d1bb1c/debug.log`: `[save] CONTINUE: resumed in room 11c0`,
    `[parkplatz] F0 ROOM11C0 sub01 @0x01824 Evt_exec sub02 bei (3,67)=1 -> keine Sperre (...), Original-sub02`,
    `[fire-aot] slot=0 at F60`, `[aot] DOOR FIRE slot=0 rect=(-25100,17250,hw=2000,hh=1350) target_cut=1
    spawn=(-25552,0,-25331)`, `loaded room11b0.rdt`, `[msg] room=11b0 id=20`, `id=21`, `DOOR FIRE slot=5`,
    `loaded room1260.rdt`, `EXIT_AT: Bild 30 in Raum 1260`.
  - K (`.../r39_parkplatz11c0_k_wd/lauf_00674e64_k`): `[parkplatz] F6 ... Programm SZENE`, msg 0..6, `[scd F1107] letterbox
    closed -> gameplay`, F1120 `AOT slot 0 typ 5 ev 40`, `F1741 ROOM11C0 Tuer im Kampf -> msg 40`, `[voice] ROOM11C0 msg 40:
    keine Aufnahme (synchro/STAGE1/room11c0/main40.wav)`, kein `DOOR FIRE slot=0` bis `F2207 MESSHAKEN ...GORILLAS_TOT`,
    F2207 `Programm ENDE`, msg 7/8/9, `DOOR FIRE slot=0 ... spawn=(-25552,0,-25331)`, `loaded room11b0.rdt`.
    Framedump F1865 (SP `k_fd_001865.png`, 320x240): Leon an der Parkplatz-Tuer, Textfenster
    "I cannot leave Ada alone here / with those monsters!".
- Generator: `python re15_port/tools/r39_parkplatz/programme_bauen.py --pruefen` -> `...parkplatz_r39_programme.inc ok`.

### 1.1 Messen die Riegel? (Gegenprobe, selbst gelinkt, Baum unveraendert)
Dasselbe Objekt `test_r39_parkplatz11c0.c.obj` des Baums, einmal vor der Bibliothek gelinkt mit dem ALTEN Modul
(`git show 72c76540:re15_port/engine/src/parkplatz_r39.c` = Stand der Abnahme 0, SP `vorher/test_vorher.exe`), einmal mit
reinen Durchreichern fuer alle `re15_parkplatz_r39_*`-Symbole (= ohne Spur O, SP `vorher/test_ohne_spur_o.exe`); die
Bibliothek zieht ihr parkplatz_r39-Objekt dann nicht (der neue Logtext "keine Sperre (Freigabe" steht nicht in der exe).
| Teil | altes Modul (Abnahme 0) | ohne Spur O | Stand HEAD |
|---|---|---|---|
| ende_gesetzt | **7 Fehler**, exit 1 ("erstes Bild: KEIN Programm SZENE", "Aot_on(0) in Bild 60: Raumwechsel ROOM0000", "Druck an der Tuer: Raumwechsel ROOM0000, kein Text", ...) | - | 0 Fehler, exit 0 |
| programme | **1 Fehler** ("Evt_exec-Haken bei (3,67)=1: sub02 bleibt das Original") | - | 0 Fehler |
| kampf | 0 Fehler | **Fehler**: "Tuer im Kampf: msg -1 ..., kein Raumwechsel", "Slot 0 = Text msg 40", "Stimme msg 40 einmal eingereiht", ... | 0 Fehler |
| wiedereintritt | - | **3 Fehler** ("VOR dem ersten Bild: Slot 0 = Text msg 40 (Installer)", "Tuer: msg -1") | 0 Fehler |
| echt | 0 Fehler | **3 Fehler** ("Kampf (Gorillas frei): Tuer -> msg -1") | 0 Fehler |
| kills_gesetzt | 0 Fehler | - | 0 Fehler |
=> `ende_gesetzt`/`programme` messen genau M1 (rot am alten Stand, gruen am neuen); `kampf`/`wiedereintritt`/`echt`
messen den Nutzer-Punkt (rot ohne Spur O). `kills_gesetzt` und `echt` sind Schutz-Riegel fuer Randzustand/H2 (gruen in
beiden Staenden — kein M1-Nachweis, aber auch so beschrieben: Dossier "(Pruefstand: dasselbe)").

## 2. Eigene Messungen an der exe (Stand HEAD 2c7498cd, exe 01:33:34)
Eigener Treiber SP `lauf.cmake` (Muster des Riegels: Messkopie `re15_pc_r39o_abn1_<lauf>_<id>.exe` neben der exe, eigener
Arbeitsordner SP `laeufe/<lauf>`, Umgebung des Aufrufers geloescht; RE15_WINDOW_SCALE=1, RE15_MSG_LOG, RE15_EVT_TRACE,
RE15_STATE_LOG, RE15_WEGPLAN). Kein Riegel faehrt diese Laeufe.

### 2.1 Lauf C1 — CONTINUE direkt in ROOM11C0 beim ERSTEN Besuch (Boot-/CONTINUE-Weg + vorbestehender F900-Haken)
Frage: Der Bau-Agent meldet als Nebenbefund, dass am CONTINUE-Weg der "door-3 self-reentry FALLBACK" (main.c:5631-5642)
in Bild 900 main00+sub00 mitten in der Szene neu laeuft. main00 setzt dabei Slot 0 neu als Door_aot_set (@0x01712) —
bleibt die Sperre bestehen? Karte `11C0 -25100 15000 1024 flag:4:0xF3 flag:3:0x82 flag:9:93` ((4,64)=0, (3,67)=0), Env
`RE15_R39O_GORILLAS_STARR=1 RE15_EXIT_AT=2200`, Wegplan SP `plan_c1.txt` (RAUM 11C0, AOTS, FLAG, GEHE (-25100,16600),
BLICK 3072, AOTS, 3x AKTION, AOTS). Ergebnis (85 s, debug.log/wegplan_log.txt SP `laeufe/c1`):
```
[save] CONTINUE: resumed in room 11c0 (hp=100)
[parkplatz] F0 ROOM11C0 sub01 @0x01824 Evt_exec sub02 -> Programm SZENE (Tuer Slot 0 -> Text msg 40, Gosub sub02)
[msg] room=11c0 id=0 .. id=5
[parkplatz] F900 ROOM11C0: Kampf laeuft beim Eintritt ((4,64)=1, (3,67)=0) -> Tuer Slot 0 = Text msg 40
[scd] door-3 self-reentry FALLBACK: re-ran main00+sub00 at frame 900
[scd F914] letterbox closed -> gameplay (player_mode=0)
F931  AOT slot 0 typ 5 ev 40 flags 0x31 band 0 mitte (-25100,17250)    FLAG (4,64) = 1, FLAG (3,67) = 0
F1514 AOT slot 0 typ 5 ev 40 ...   F1515 AKTION
[parkplatz] F1515 ROOM11C0 Tuer im Kampf -> msg 40 "I cannot leave Ada alone here with those monsters!" (Stimme main40.wav)
F1704 AOT slot 0 typ 5 ev 40 ...   (kein DOOR FIRE)   [flow] EXIT_AT: Bild 2200 in Raum 11c0
```
=> Die Sperre ueberlebt den F900-Haken: der Installer steht am Ende von scd_room_reenter (scd_room_setup.c), den der
Haken aufruft, und setzt Slot 0 im selben Bild wieder auf Text. Der Haken selbst (Szene bricht nach msg 5 ab, msg 6
"Ada: O-okay..." fehlt, Steuerung F914 statt ~F1107) ist vorbestehend und nur ueber einen Spielstand IN 11C0 erreichbar
(11C0 hat keine Schreibmaschine) — Nebenbefund des Bau-Agenten bestaetigt, nicht Spur O.

## 3. RE-Gate (selbst disassembliert / selbst gelesen)
Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (Baum) auf `info/Re1.5/PSX.EXE` (md5 b55fdaa5...) und
`info/Re1.5/PSX/BIN/STAGE1.BIN`; RDT `re15_port/shared_assets/PSX/STAGE1/ROOM11C0.RDT` (md5 5f303459..., bitgleich mit
`info/Re1.5/PSX/STAGE1/ROOM11C0.RDT`). Schwerpunkt: die NEUEN Belege der Nachbesserung 1.

| Zitat (Nachbesserung 1) | selbst gelesen | Ergebnis |
|---|---|---|
| sub01 Freigabe @0x0182E/@0x01836/@0x0183E/@0x01842, Beginn @0x01820/@0x01824; sub02 @0x0184E, sub03 @0x01B02; sub00 @0x017C8/@0x017D0 | Sub-Tabelle @0x1758: sub01 @0x181C, sub02 @0x184E, sub03 @0x1B02. Bytes: @0x0181C `06 00 0a 00`, @0x01820 `21 04 40 00`, @0x01824 `04 0a 18 02`, @0x01828 `08 00`, @0x0182A `06 00 1e 00`, @0x0182E `21 03 43 00`, @0x01832 `06 00 14 00`, @0x01836 `21 07 60 01`, @0x0183A `06 00 0a 00`, @0x0183E `21 07 61 01`, @0x01842 `04 ff 18 03`; @0x0184E `22 04 40 01`; @0x01B02 `22 03 43 01`; @0x017C8 `07 00 52 00`, @0x017CC `06 00 48 00`, @0x017D0 `21 03 43 00`; main00 @0x01712 `3b 00 02 31 00 00 24 96 1c 3e a0 0f 8c 0a 30 9c 00 00 0d 9d fb 0b 00 1b 01` | bestaetigt: Ck(3,67)==0 umschliesst den ganzen Freigabe-Zweig (If-Block @0x0182A, Laenge 0x1e) -> bei (3,67)=1 erreicht sub01 @0x01842 nie |
| Zensus "(3,67) setzt nur sub03 @0x01B02, (4,64) nur sub02 @0x0184E, kein Loescher" | eigener ROH-Byte-Scan aller 240 RDTs (kann keine Stelle uebersehen, zaehlt eher zu viel): `22 03 43` genau 1 Treffer = ROOM11C0 @0x01B02 `22 03 43 01`; `22 04 40` mit Wert 00/01 genau 1 Treffer = ROOM11C0 @0x0184E `22 04 40 01` (16 weitere Treffer mit Werten 0xbe/0x33 an Offsets >= 0xF363 = keine Set-Opcodes); grep `flag_set( 3, 67/0x43 )`/`( 4, 64/0x40 )` in engine/platform/include: 0 Treffer | bestaetigt: Zustand (4,64)=0/(3,67)=1 nur per Spielstand/Debug-Flag |
| Waffe 3 gegen 0x27 = 12 @0x8006ee44 (Riegel `echt`) | Leser @0x800124b0 `lbu v1,8(s1)` (Typ), @0x800124b8 `addiu a0,a0,-7984` = 0x8006e0d0, @0x800124c0-d0 Typ*0x58 (`sll/addu/sll/subu/sll`), @0x800124d8-e0 Waffe*4, @0x800124ec `lhu v1,0(v1)`, @0x800124f4 `subu a0,a0,v1` (hp +0x9A), @0x800124f0/f8 +0x93 \|= 1; Daten @0x8006ee38 = `00 00 00 00 07 00 00 00 1e 00 00 00 0c 00 ..` -> [0,7,30,12,...], Index 3 = **12**; @0x80012528 `bgez v1` / @0x8001252c `sb v0,4(s1)` (2) bzw. @0x80012538 (3) | bestaetigt (Zeile 0x27 = 0x8006e0d0 + 0x27*0x58 = 0x8006ee38) |
| Slot-Reihenfolge Evt_exec @0x8003f2a0 / scd_execute_scripts @0x8003f0a0 (Riegel `kills_gesetzt`) | @0x8003f2a0: a0 = pc[1], a1 = pc[3], pc += 4, `jal 0x8003ee3c`; @0x8003ee3c: Slot < 10 direkt, sonst erster freier Slot ab 2 (@0x8003ee60-a0, Aktiv-Byte 0x800b2b4d + n*0x170); @0x8003f0a0: Schleife s2 = 0..9 (@0x8003f180 `sltiu v0,s2,0xa`), je Slot das Aktiv-Byte (@0x8003f0dc `lbu v0,1(s0)`) ZUM ZEITPUNKT der Iteration | bestaetigt: ein im selben Durchlauf gestarteter Thread auf hoeherem Slot laeuft noch im selben Bild, in Startreihenfolge (Port: Slots ab 10 statt ab 2, Reihenfolge gleich) |
| Kill-Bit im ersten Todesbild STAGE1 @0x8011b7c8/@0x8011b8e8-f4 (Hinweis H1) | @0x8011b7c8 `lbu v1,7(a1)`, @0x8011b7e0 `beq v1,zero,0x8011b804`; @0x8011b8e8 `lbu a1,454(v0)` (+0x1C6 = pc[7]), @0x8011b8f0 `addiu a0,a0,4152` = 0x800b1038, @0x8011b8f4 `jal 0x8004ef90`; @0x8004ef90-b4 `srl/sll/addu/andi/srlv/or/sw` (Bit setzen); Bank-Tabelle @0x80074664[7] = 0x800b1038 | bestaetigt |
| Aot_on @0x8004082c (Ruecknahme vor sub03 @0x01C08) | @0x8004080c `lbu v0,0(v1)` rec[0], @0x8004081c Tabelle 0x8007469c, @0x8004082c `jalr v0`; RDT @0x01C08 `47 00` | bestaetigt |

- Konstanten: die Nachbesserung bringt KEINE neue Konstante (nur `RE15_R39O_ENDE_BANK/_BIT`, belegt sub03 @0x01B02 /
  sub01 @0x0182E). Neue Test-Konstanten `ECHT_WAFFE 3` / `ECHT_SCHADEN 12` tragen @0x8006ee44 (oben selbst gelesen).
  Alle Konstanten in `include/re15_parkplatz_r39.h` tragen @0x/RDT-Offset oder VERTRAG-Bezug (Id 40 = VERTRAG 1.2);
  Text = NUTZER-VORGABE, Stimme = PORT-WAHL (gekennzeichnet).
- Commit 2c7498cd nennt die Adressen in der Commit-Message (@0x0182E, @0x01842, @0x01B02, @0x0184E, @0x8003f2a0,
  @0x8003f0a0, @0x8006ee44).
- Rate-Wort-Scan ueber alle `+`-Zeilen von `git diff master...HEAD -- re15_port` (deferred, tunable, interim, for now,
  faithful, plausib, TODO, FIXME, ungefaehr, geschaetzt, vermutlich, approx, guess, "sieht richtig"): **0 Treffer**.
  Env-Schalter nur als Messhaken (RE15_R39O_GORILLAS_STARR/_TOT, kein Spielverhalten), nicht als Abschluss.
- Erklaert der Fix den Befund M1? Ja, selbst gemessen: (a) Logik — die Sperre hat genau eine Freigabe, und die steht hinter
  Ck(3,67)==0 (@0x0182A/@0x0182E); SZENE sperrt jetzt nur, wenn diese Freigabe noch erreichbar ist. (b) Gegenprobe 1.1:
  `ende_gesetzt` rot (7) am alten, gruen am neuen Modul. (c) `integration_r39_stimmen_11b0` gruen (Abschnitt 1) mit genau
  der neuen Logzeile "bei (3,67)=1 -> keine Sperre".

## 4. Vertrag, Pfade, Nebenwirkungen
- Pfade: `git diff master...HEAD --name-only` (Merge-Basis fa2c0e7e; `git diff master` zeigt zusaetzlich nur den
  master-Fortschritt nach der Basis, z. B. Spur K, als Loeschungen): 37 Dateien — analysis/befunde_runde39/O_* (Dossier,
  Abnahme 0, Belege), engine/src/parkplatz_r39.c, engine/src/gen/parkplatz_r39_programme.inc, include/re15_parkplatz_r39.h,
  tools/r39_parkplatz/programme_bauen.py, tests/unit/test_r39_parkplatz11c0.c, tests/unit/probes/r39_parkplatz11c0.cmake
  (Sonden-Muster je Thema, keine Test-CMakeLists), tests/integration/test_r39_parkplatz11c0.cmake und drei Haken-Dateien.
  KEIN release/, platform/android/, shared_assets/PSX/, synchro/, keine Test-CMakeLists. Arbeitsbaum sauber.
- Gemeinsame Dateien nur Haken mit Kommentar "Runde 39 Spur O": scd_vm.c +4 (Include, op_evt_exec hinter dem Runde-38-F-Haken,
  re15_scd_show_message vor re15_dialog_open_mask), scd_room_setup.c +2 (Include, Installer am Blockende), platform/pc/main.c
  +3 (Include, Boot-/CONTINUE-Installer, Messhaken). Seit Abnahme 0 an den Haken nichts geaendert. Jeder Haken prueft zuerst
  Raum 0x11C0 (evt_exec, install, stimme) bzw. die Messhaken-Umgebung — andere Raeume laufen unveraendert durch.
- Bank-9: keines belegt (Bit 96 bleibt frei; Original-Flags (4,64)/(3,67)/(7,0x60)/(7,0x61)); k_port[] unberuehrt.
- Nachrichten-Id: 11C0 Id 40 (VERTRAG 1.2). In master belegt 11C0 nur 0..9 (RDT) und 10..14 (irons_tod_1150.c:699-701,
  `git grep` auf master) — keine Kollision; stimmen_r39.c (Spur A) hat keine 11C0-Zeile.
- Stimme: `synchro/STAGE1/room11C0/main40.wav` fehlt (Baum und Hauptbaum: nur main00..main14) — erwartet, die Aufnahme macht
  der Nutzer; der Lader (audio_pc.c:2242-2269) sucht zuerst `room11C0/` (%04X), dann `room11c0/` und meldet den letzten Versuch.
- Tests: unit verlinkt `m` ausser WIN32 (sqrt in test_r39_parkplatz11c0.c:270); Integrationsskript mit
  cmake_minimum_required, exe-Kopie je Teil (`re15_pc_r39o_<TEIL>_<id>`, LAST_EXT), eigener Arbeitsordner je Teil, Umgebung
  des Aufrufers geloescht; kein popen, keine .exe-/Backslash-Pfade.
- Modulgruppen (Regex der Abnahme 0 inkl. 11B0-Nachbarn, `-E "^integration_" -j 4`): **`100% tests passed, 0 tests failed
  out of 136`** (13,94 s; darin unit_scd_*, unit_aot_*, unit_msg_dialog, unit_r35_affen_* (Szenen-Gang 11C0), unit_r39_gegner_*,
  unit_r38_dining1141_*, alle 10 unit_r39_parkplatz11c0_*). Mitschrift SP `ctest_modulgruppen.txt`.
- Welche fremden Riegel setzen (3,0x43)? `grep` in tests/: nur test_r36_szenenleon / test_r38_werkstatt11b0 (beide bleiben in
  ROOM11B0) und test_r39_stimmen (11c0-Karte, jetzt gruen). Die Aenderung der Nachbesserung wirkt also an der exe nur in
  `integration_r39_stimmen_11b0`.
- Probe-Merge gegen den heutigen master 2963579d (`git merge-tree --write-tree master HEAD`, schreibt keinen Arbeitsbaum):
  EIN Konflikt, `re15_port/engine/src/scd_room_setup.c`, zwei Stellen — Spur K (in master) und Spur O haengen je eine
  Include-Zeile (:48) und eine Installer-Zeile am Blockende (:509) an dieselbe Stelle. Aufloesung: beide Zeilen behalten
  (unabhaengig: re15_r39k_install loescht NPC-INIT-Merker, re15_parkplatz_r39_install wirkt nur in 0x11C0). scd_vm.c und
  main.c mischen ohne Konflikt. Integrationsaufgabe, kein Mangel der Spur.
