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
- [x] Eigene Messungen an der exe (C1 CONTINUE+F900, R1 freie Gorillas, R2 echter Kampfausgang)
- [x] RE-Gate (6 Stellen selbst: RDT-Freigabezweig + Roh-Zensus, @0x8006ee44, @0x8003f2a0/@0x8003ee3c/@0x8003f0a0, STAGE1 @0x8011b8f4, @0x8004082c)
- [x] Vertrag / Pfade / Nebenwirkungen (Modulgruppen 136/136; Probe-Merge: 1 trivialer Konflikt mit Spur K)
- [x] Urteil: Nutzer-Punkt erfuellt, alle Gates halten -> BESTANDEN

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
messen den Nutzer-Punkt (rot ohne Spur O). `kills_gesetzt` (Randzustand) und `echt` (H2) sind am alten wie am neuen Modul
gruen — kein M1-Nachweis, so auch im Dossier beschrieben ("(Pruefstand: dasselbe)").

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

### 2.2 Lauf R1 — Gorillas LEBEND und FREI, Leon geht zu Fuss zur Tuer (Tuer ueber den echten Weg 11B0 -> 11C0)
Aufbau ohne Speicherkarte: Titel -> NEW GAME (RE15_TITLE_CONFIRM_MS=1500, RE15_PSELECT_AUTO=1 = Leon), Debug-Menue-Sprung
`RE15_DEBUG_JUMP=11B0@gp` vor die Parkplatz-Tuer (`RE15_PLAYER_POS=-25480,-26000,1024`), Flags wie die Riegel-Karte
(`RE15_SET_FLAG=4:0xF3,3:0x82,9:93`), Browning 0x03 x99 ausgeruestet (RE15_GIVE/RE15_EQUIP), Spur-E-Messhaken
`RE15_R39_MESS_SPIELER_HP=100` (Leon ueberlebt die Griffe), KEIN GORILLAS_STARR/_TOT. Wegplan SP `plan_r1.txt`: 11B0 AKTION
(echte Tuer) -> RAUM 11C0 -> Plc_dest-Punkte der Szene -> 6x GEHE (-25100,16600) -> BLICK 3072 -> 3x AKTION -> FLAG; danach
ein stehender Skript-Schuetze (RE15_INPUT_SCRIPT ab Spielbild 1300, Bloecke "W0.1,M0.6,6x(MA0.1,M0.3)").
Ergebnis (SP `laeufe/r1_teil/`):
```
[debug-menu] JUMP -> 11b GARAGE (ROOM11B0) spawn=(-25480,0,-26000)
[aot] DOOR FIRE slot=1 rect=(-25480,-28190,hw=1250,hh=1750) ... spawn=(-25279,0,17268)   <- echte Tuer 11B0 -> 11C0
[parkplatz] F6 ROOM11C0 sub01 @0x01824 Evt_exec sub02 -> Programm SZENE ...
[scd F1107] letterbox closed -> gameplay
wegplan: F1364..F2002 sechsmal "Klemme" (Gorilla-Griffe, Messhaken fuellt 6/12 HP auf), F2040 an der Tuer angekommen
F2164 AOT slot 0 typ 5 ev 40 flags 0x31
[parkplatz] F2165 ROOM11C0 Tuer im Kampf -> msg 40 "I cannot leave Ada alone here with those monsters!"
state F2165 pad=8000 PL(-24093,17277) msg(a=1 fsm=1 id=40) pf=FFFF0007
F2355..F2358 FLAG (4,64)=1, (3,67)=0, (7,96)=0, (7,97)=0
[parkplatz] F2359 ROOM11C0 Tuer im Kampf -> msg 40 ...   (erster Skript-Druck noch an der Tuer)
kein DOOR FIRE slot=0; Kampf danach: 18 Treffer bis F4660 (gegner_r39.log "affen treffer ... hp=..")
```
=> Mit lebenden, freien Gorillas zeigt die Tuer im Kampf zweimal den Text, kein Raumwechsel. Der Schuetze war mit der Pistole
zu langsam fuer 2 x 31 Treffer (Leon wird staendig gegriffen) — Lauf nach F4660 abgebrochen (nur meine Messkopie beendet);
der Kampfausgang wird in Lauf R2 mit einer staerkeren Waffe gemessen.

### 2.3 Lauf R2 — ECHTER Kampfausgang an der exe (kein Kill-Haken): Tuer im Kampf gesperrt, nach dem Kampf frei
Wie R1 (bis F2359 bildgleich: dieselben Klemmen, dieselben Drucke), aber Super Redhawk 0x07 x99 (RE15_GIVE=0x07:99,0x17:60,
RE15_EQUIP=7; Schaden gegen 0x27 = Zeile @0x8006ee38 + 7*4 = @0x8006ee54 = `32 00` = 50, selbst gelesen), damit der stehende
Skript-Schuetze beide Gorillas in der Laufzeit toetet. KEIN RE15_R39O_GORILLAS_TOT/_STARR (debug.log: 0 Zeilen "MESSHAKEN").
Ergebnis (335 s, exit 0, SP `laeufe/r2`):
```
[pl] Spieler-Familie PL00 (character=0, Elza-Bit=0)          [equip] W-bank -> W07
[parkplatz] F6 ... Programm SZENE                            [scd F1107] letterbox closed -> gameplay
F1120 AOT slot 0 typ 5 ev 40; FLAG (4,64)=1, (3,67)=0
[parkplatz] F2165 ROOM11C0 Tuer im Kampf -> msg 40 ...       state: msg(a=1 fsm=1 id=40) pf=FFFF0007
[parkplatz] F2359 ROOM11C0 Tuer im Kampf -> msg 40 ...       F2354 AOT slot 0 typ 5 ev 40; (7,96)=(7,97)=0
state.txt Gorilla Slot 2: hp 360 -> 310 (F2593) -> ... -> 10 (F3568) -> -40 Zustand 3 (F3616) -> Zustand 7 (F3656)
state.txt Gorilla Slot 3: hp 360 -> ... -> 10 (F4219) -> -40 Zustand 3 (F4312) -> Zustand 7 (F4352)   (Magazin 99 -> 67)
[parkplatz] F4354 ROOM11C0 sub01 @0x01842 Evt_exec sub03 -> Programm ENDE (Tuer Slot 0 zurueck -> ROOM11B0, Gosub sub03)
[msg] room=11c0 id=7 / id=8 / id=9
[aot] DOOR FIRE slot=0 rect=(-25100,17250,hw=2000,hh=1350) target_cut=1 spawn=(-25552,0,-25331)
[room] PC loaded room11b0.rdt          [flow] EXIT_AT: Bild 200 in Raum 11b0
```
=> Am gebauten Spiel, mit echten Treffern: waehrend des Kampfes Text statt Tuer; die Kill-Bits setzt allein der Todes-Commit
der Engine, sub01 startet daraufhin die Original-Ende-Szene (Programm ENDE gibt die Tuer im selben Bild frei), die Szene geht
durch die Tuer nach ROOM11B0 — kein Festsitzen. Zeitfenster Hinweis H1 hier gemessen: zweiter Gorilla tot (Zustand 3) F4312,
ENDE F4354 = 42 Bilder (1,4 s), in denen die Tuer noch den Text zeigt (Kill-Bit-Commit erst im Leichen-Zustand 7, F4352).
Die Spieler-Drehung nach der Ruecknahme stimmt: Lauf K, ROOM11B0 F1 `PL(-25552,-25331,rot=3067)` = main00 @0x01712+20
`fb 0b` (der Aot_reset der Sperre beruehrt nur sce/Flags/x,y,z; Raum, Cut und Blick bleiben im Satz).

### 2.4 Lauf F1 — eigenes Bild des Textes (Framedump RE15_WINDOW_SCALE=1)
Karte `11B0 -25480 -26000 1024 flag:4:0xF3 flag:3:0x82 flag:9:93 flag:4:64` (Kampf laeuft), Wegplan SP `plan_f1.txt`
(11B0 AKTION -> RAUM 11C0 -> BLICK 3072 -> AKTION -> WARTE 200), Env `RE15_R39O_GORILLAS_STARR=1 RE15_EXIT_AT=330#11c0
RE15_FRAMEDUMP=90-300/30:fd_` (33 s): `DOOR FIRE slot=1 ...` -> `loaded room11c0.rdt` -> `[parkplatz] F51 ROOM11C0: Kampf
laeuft beim Eintritt ... -> Tuer Slot 0 = Text msg 40` -> `F52 ROOM11C0 Tuer im Kampf -> msg 40`; state F52..F300
`msg(a=1 fsm=1 id=40) pf=FFFF0007`, Leon steht an der Tuer (-25279,17268). Bild SP `laeufe/f1/fd_000210.png` (320x240):
Leon vor der Parkplatz-Tuer (EXIT-Schild), Textfenster **"I cannot leave Ada alone here / with those monsters!"**.
Text-Bytes selbst dekodiert (gen/parkplatz_r39_programme.inc, Zeichen 0x1d='A', 0x3d='a', 0x00=' ', 0x08=Umbruch,
0x1a='!'): `25`=I, `3f 3d 4a 4a 4b 50`=cannot, `48 41 3d 52 41`=leave, `1d 40 3d`=Ada, `3d 48 4b 4a 41`=alone,
`44 41 4e 41`=here, `08`, `53 45 50 44`=with, `50 44 4b 4f 41`=those, `49 4b 4a 4f 50 41 4e 4f`=monsters, `1a`=! — woertlich
der Nutzer-Text.

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

## 5. Gezielte Laeufe der beruehrten Raeume an der exe (Spurbaum, KEINE volle Suite)
`ctest --test-dir re15_port/build -R "^integration_r37_szene11c0$|^integration_r36_schluessel$|^integration_r39_gegner_schatten$|^integration_r35_cut1150$|^integration_r36_szene1150$|^integration_r38_montage$|^integration_r38_werkstatt11b0$|^integration_r35_granate$|^integration_r35_cut10f0$" -j 2 --timeout 1800`
-> **`100% tests passed, 0 tests failed out of 9`** (797,77 s: cut10f0 248,33, granate 173,42, cut1150 525,36,
werkstatt11b0 152,83, szene11c0 114,41, szene1150 113,59, montage 99,33, schluessel 107,73, gegner_schatten 58,64 s; Mitschrift
SP `ctest_nachbarn.txt`). Zusammen mit Abschnitt 1 (Spur 10 + 6) und 4 (Modulgruppen 136): alle gezielten Laeufe gruen,
kein Fenster-Haken musste nachgefahren werden. Nicht selbst gefahren: die 847 unit_/prop_/probe_ (laut Bau-Agent gruen) —
seit jenem Lauf keine Code-Aenderung.

## 6. Urteil je Nutzer-Punkt
| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| ROOM11C0, waehrend der Gorilla-Kampf laeuft, bei Leon der Text "I cannot leave Ada alone here with those monsters!", wenn Leon versucht, durch die Tuer hinauszugehen | **erfuellt** | Am gebauten Spiel (HEAD 2c7498cd) selbst gemessen: R1/R2 (Gorillas lebend und frei, echte Tuer 11B0 -> 11C0): F2165/F2359 Druck an der einzigen Tuer -> msg 40, kein Raumwechsel; R2 mit echtem Kampfausgang: Tod beider Gorillas durch Treffer -> Kill-Bits vom Todes-Commit -> F4354 Programm ENDE -> msg 7/8/9 -> `DOOR FIRE slot=0` -> ROOM11B0 (kein Festsitzen); K (Riegel, selbst gefahren und ausgewertet): F1741 msg 40, Bild F1865; F1 (eigenes Bild F210): Textfenster woertlich; W/C1: Wiedereintritt bzw. CONTINUE im Kampf (auch ueber den F900-Haken) -> Sperre steht; N/i1/i2/stimmen_11b0: ausserhalb des Kampfes bzw. bei (3,67)=1 bleibt die Tuer die Original-Tuer (M1 behoben); Elza ROOM11C1 unberuehrt (E). Einzige Tuer des Raums = Slot 0 (SCD-Dump: 1 Door_aot_set, 1 Aot_on, kein Aot_set/Aot_reset) |

## 7. Gates
- RE-Gate: **haelt** (Abschnitt 3; 6 Stellen selbst disassembliert/gelesen, darunter alle neuen Belege der Nachbesserung;
  keine neue Konstante; Test-Konstanten mit @0x; 0 Rate-Woerter; der Fix erklaert M1 und ist durch die Gegenprobe 1.1
  belegt).
- Vertrag (Ressourcen/Pfade): **haelt** (Abschnitt 4; Id 40, kein Bank-9-Bit, nur erlaubte Pfade, Haken 2-4 Zeilen mit
  Spur-Kommentar).
- Tests/Nebenwirkungen: **haelt** (Spur unit 10/10 + integration 5/5 + `integration_r39_stimmen_11b0` gruen — der in
  Abnahme 0 haengende master-Riegel; Modulgruppen 136/136; 11C0/11B0-Nachbarn 9/9; neue Riegel messen nachweislich
  (Gegenprobe 1.1); Elza unberuehrt).

## 8. Maengel
Keine. (M1 der Abnahme 0 ist behoben: Abschnitte 1, 1.1, 3.)

## 9. Hinweise (ausserhalb der Spur O, kein Abnahme-Mangel)
- **H1 (bestaetigt, offen bei Gegner-KI/Orchestrator):** der Port setzt die Kill-Bits erst im Leichen-Zustand 7, das Original
  im ersten Todesbild (STAGE1.BIN @0x8011b7c8 `lbu v1,7(a1)` / @0x8011b7e0 `beq v1,zero` -> @0x8011b804..@0x8011b8c8
  ohne Verzweigung -> @0x8011b8f4 `jal 0x8004ef90`, selbst gelesen). An der exe (R2) gemessen: zweiter Gorilla Zustand 3 F4312, Zustand 7 F4352, Programm ENDE F4354 —
  42 Bilder (1,4 s; Riegel `echt`: 71 Bilder), in denen die Tuer nach dem Tod des letzten Gorillas noch msg 40 zeigt. Wirkt
  nur, wenn Leon genau dann an der Tuer drueckt; Ursache in enemy_ai_common.c (geteilte Datei der Spuren D/E).
- **Nebenbefund F900 (bestaetigt, vorbestehend):** am CONTINUE-Weg direkt in ROOM11C0 laeuft der "door-3 self-reentry
  FALLBACK" in Bild 900 mitten in der Ada-Szene; Lauf C1: msg 6 "Ada: O-okay, I'll do that..." faellt weg, Steuerung F914.
  Die Sperre bleibt dabei bestehen (Installer am Ende von scd_room_reenter). Nur ueber einen Spielstand IN 11C0 erreichbar.
- **Integration:** `git merge-tree` gegen master 2963579d: ein trivialer Konflikt in scd_room_setup.c mit Spur K (Include
  :48, Installer :509) — beide Zeilen behalten.
- Stimme: `synchro/STAGE1/room11C0/main40.wav` liegt noch nicht vor; der Port fragt genau diese Datei an (K:
  `[voice] ROOM11C0 msg 40: keine Aufnahme (...)`) — Aufnahme durch den Nutzer.
