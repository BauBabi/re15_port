# Runde 37 — Spur C "fahrstuhl": Abnahme 1 (unabhaengig)

Stand: FERTIG (fortlaufend geschrieben). Baum `.claude/worktrees/r37_fahrstuhl`, Zweig `r37/fahrstuhl`, Bau-Stand
a0326689 (Nachbesserung 1 = Commits f45539f8..a0326689 nach Abnahme 0 06bf9b12), merge-base mit master f1e019ca.

## 0. Gelesen
* AUFTRAG.md (#6/#7 + Nachtrag 5: "ja, dann mache den noch. Mir ist auch aufgefallen in den Stage2 kleineres aufzügen
  fehlt noch das sound. Den sollte man auch aus Resident Evil 2 extrahieren."), VERTRAG.md, Dossier C_fahrstuhl.md
  (Abschnitt "Nachbesserung 1"), Abnahme 0 (#6/#7 erfuellt, Maengel 1/2 = Nachtrag 5a/5b).
* `git log --oneline master..HEAD` (23 Commits), `git diff master...HEAD --stat` (60 Dateien),
  `git diff 06bf9b12..HEAD` (N1: 24 Dateien). Code-Diff N1 gelesen: aot_common.c (+1 Include, +3 Haken-Zeilen hinter dem
  Kabinen-Haken), scd_vm.c (+1 Include, +2 Scan, +2 Takt), platform/pc/main.c (+1), NEU lastenaufzug_r37.c/.h,
  lift2030_r37.c/.h, platform/pc/src/lift2030_se_pc.c, gen/re2_lift6030_bank.inc, gen/re15_tuer_eigen.inc (P2DS FNV),
  P2DS.DO2 (geaendert), RE2/LIFT6030.VBS (neu), tools/tueren/tuer_rezepte.py + tuer_rest_plan.py (Rezept P2DS),
  Runde-33-plan/archive.json, Riegel probe_r37_fahrstuhl_n1.c + test_r37_fahrstuhl_n1.cmake + probes/r37_fahrstuhl.cmake.

## 1. Bau + Tests (selbst gefahren)
* `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do`,
  `LOCAL-BUILD-OK (build)`. Geprueft, dass die exe der Stand HEAD ist: re15_pc.exe 13:12:17 ist juenger als jede
  geaenderte Quelle (zuletzt gen/re15_tuer_eigen.inc 13:11:48, aot_common.c/scd_vm.c 13:04:06), probe_r37_fahrstuhl_n1.exe
  13:49:44 > Quelle 13:49:33; `git status` sauber (nur `?? build/`); exe-Strings `[lift2030] Anker @0x%05X`, `LIFT6030.VBS`.
* Assets selbst nachgerechnet: P2DS.DO2 78244 B FNV 0x4A900C54 sha1 8daaadc1... (master: FNV 0xC9CE31CD sha1 520e0052...),
  LIFT6030.VBS 61192 B FNV 0x0C73F970 sha1 944a6194... = Dossier/Tabelle.
* `ctest -R "unit_r37_fahrstuhl_(lastenaufzug|lift2030|durchlauf)" -V`: 3/3 gruen, Ausgaben wie im Dossier
  (u. a. `P2DS V5: 451 Bilder, Schacht obj 2 y: 286 Bilder fallend / 0 steigend`, `P2DS V4: 301 ... 148 steigend / 0
  fallend`, vier Fahrten je `Laeufer 1 x: DOOR2D V5 P2DS Tuer 112, player_mode beim Feuern 2`, `Gegenprobe S218 ... V4`;
  Lift A/B hinauf/hinab in 2030 + 2031 `Toene 0@Takt35/Bild1 2|1@Takt35/Bild1`, Merker nach 90 bzw. 180 Bildern;
  ohne Strom `Toene` leer; `Stopp-Schlag FAHRT Bild 200 (Fahrt 180 + 20), FAHRT_KURZ Bild 110 (Fahrt 90 + 20)`).
* Gezielte Gruppen: siehe Abschnitt 5 (101/101 gruen).

## 2. Messungen an der exe (Messkopie `re15_pc_c_abn1_mess.exe`, Arbeitsordner im Scratchpad `abn1/`)

Helfer: Spielstand-Werkzeug `probe_r35_karte karte` (wie Abnahme 0), Start `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
RE15_CARD_SLOT=0`, Eingabe NUR `RE15_INPUT_SCRIPT` (BASIS spiel, START 60; Messfalle virtueller Controller, s. Dossier),
Bilder `RE15_TUER_SERIE`, Ton `RE15_AUDIO_CAP_SYNC` (1470 Stereo-Bilder je Takt).

### M1 Nachtrag 5a - Story-Weg ueber Spielstand (ROOM3070 -> Szene -> ROOM3080 Fahrt -> Ausstieg -> ROOM4000)
ROOM3070 sub00 (selbst dekodiert, scd_dump_room.py): `@0x033CA Ck(4,215)==0` / `@0x03408 Ck(4,236)==0` / `@0x03438
Ck(3,180)==1` -> `@0x03464 Aot_set Platz 3 sce 3 sat 0x41 rect (-20150,-6710,1200,4500)` (Mitte-Test, Szene sub03; sub03
`@0x035C6 Set(3,180)=0` = einmalig). Spielstand `karte re15_card.mcr 3070 -20479 -5000 0 flag:4:215 flag:4:236 flag:3:180`
(Spieler WESTLICH ausserhalb des Platzes, Blick +x), Skript `U0.6` = er GEHT in den Platz (erster Versuch mit Stand
(-19550,-7400) Blick +z lief an der Westkante x -20479 entlang am Platz vorbei, 8 min ohne Szene - kein Fehler der Spur).
```
[save] CONTINUE: resumed in room 3070 (hp=100)
[scd] thread start slot=10 first_op=0x22                        (sub03 = Szene, nach dem Hineingehen)
[aot] DOOR FIRE slot=0 rect=(-26800,-11960,hw=600,hh=1700) target_cut=0 spawn=(-25540,0,-22440)
[aot] Tuer S195 in einer Zwischensequenz gefeuert -> RE1.5-Uebergang ohne Sequenz      (Runde-31-Sperre, unveraendert)
[room] PC loaded room3080.rdt (240472 bytes)
[scd] thread start slot=10 first_op=0x22                        (sub02 erste Fahrt mit Ada)
[aot] DOOR FIRE slot=0 rect=(0,0,hw=0,hh=0) target_cut=1 spawn=(-7600,0,-6760)
[tuer] Sequenz Archiv 2 DOOR2D Port-Archiv P2DS Variante 5 Bit7 0 Tuer 45 Seite S000 T112 Spender FF (10 Skripte)
[tuer] Bild 20 Se_on Tonkopf-Eintrag 0
[tuer] Bild 175 Se_on Tonkopf-Eintrag 1
[tuer] Sequenz fertig: 451 Bilder + 1 Warten, 2 Se_on, Schliesston 0, Ton geladen, Notizen 0x0
[room] PC loaded room4000.rdt (288632 bytes)
[flow] EXIT_AT: Bild 150 in Raum 4000 erreicht -> exit
```
* Bilder (`RE15_TUER_SERIE`, 32 Abdunkel- + 451 Tuer-Bilder, Bogen jedes 30. Bild selbst gesichtet): Streckmetall-
  Plattform mit Gelaender vor Schwarz; das rot beleuchtete Fachwerk erscheint an der Gelaenderkante (Bild 60/150/240/
  390) und steht 30 Bilder spaeter gross ueber Kopf (Bild 90/180/420) = es zieht nach OBEN weg = Buehne faehrt HINAB;
  keine Bedientafel. Danach ROOM4000 Cut 1 mit der RE1.5-Ankunft (Spieler y -1680 -> 0, `[scd F912] Cut_chg(1)`).
* Ton: Einzelsequenz `RE15_TUER_ARCHIV=P2DS:5` aufgenommen (202 laute Takte, Spitze RMS 2935) und in der Story-Aufnahme
  gesucht: Treffer Takt 2617..2818, **593880 Abtastwerte identisch (max. Abweichung 0)**, davor/danach still -> beide
  DOOR2D-Toene (Anfahrt + Halt) laufen im echten Weg.
* Gemalt (probe_bg_dump STAGE3/4): ROOM30800..03 = rot-orange Gitterfachwerk der Kabine, ROOM40001 = Schacht mit Gitter
  + gelb-schwarzer Kante + "Level-1" - die Sequenz (rotes Fachwerk, Warnrand) passt dazu; keine Bedientafel gemalt.

### M2 Nachtrag 5a - Gegenprobe am Stand VOR der Spur (gleicher Spielstand, gleiches Skript)
exe des Hauptbaums (09:02, master hat seit f1e019ca keine re15_port-Aenderung; Strings `P27C`/`lift2030` fehlen) als
`re15_pc_c_abn1_vorher.exe` (danach geloescht):
```
[save] CONTINUE: resumed in room 3070 (hp=100)
[aot] Tuer S195 in einer Zwischensequenz gefeuert -> RE1.5-Uebergang ohne Sequenz
[room] PC loaded room3080.rdt (240472 bytes)
[aot] DOOR FIRE slot=0 rect=(0,0,hw=0,hh=0) target_cut=1 spawn=(-7600,0,-6760)
[room] PC loaded room4000.rdt (288632 bytes)        <- KEINE [tuer]-Zeile: Befund 5a am alten Stand reproduziert
```

### M3 Nachtrag 5a - weitere Wege (neuer Stand)
| Lauf | Weg | Ergebnis (debug.log) |
|---|---|---|
| m2a | Elza (`RE15_PSELECT_AUTO=1 ..._SWITCH=1`, `Elza-Bit=1`), `RE15_GOTO_ROOM=3081`, (4,0xFD)=0 -> sub02 | `DOOR FIRE slot=0 rect=(0,0,hw=0,hh=0)` -> `Port-Archiv P2DS Variante 5 ... Seite S000 T112` -> `room4001.rdt` |
| m2b | dasselbe mit `RE15_SET_FLAG=4:253` -> sub03 | dieselbe Sequenz -> `room4001.rdt` |
| m3 | Leon `RE15_SET_FLAG=4:96 RE15_DEBUG_JUMP=3080@120` -> sub09 (zweite Fahrt, Tuer feuert nach ~280 Bildern statt 907) | `P2DS Variante 5` -> `room4000.rdt` |
Alle mit `Sequenz fertig: 451 Bilder + 1 Warten`; nach der Ankunft laeuft die RE1.5-Ankunft in ROOM4000 weiter.

### M4 Nachtrag 5b - Lift-Ton ROOM2030 (Differenz zweier Aufnahmen mit/ohne Aktionstaste, eigener ADPCM-Dekoder)
Startlage per `RE15_DEBUG_JUMP=2030@120 RE15_PLAYER_POS=<620 vor der Plattform-Mitte>,0,<Band>` (der Raum ist ein
mehrstoeckiger Rundbau; ein Weg von der Tuer-Ankunft (6550,-5400,-100) aus ROOM2070 - selbst per Spielstand + Tuer
gefahren, `P1BD` -> `room2030` - zur Plattform wird bei z ~3920 von 3 Gegnern auf dem Steg zurueckgedraengt, darum
hier Teleport; der Weg ueber die Tuer zu Fuss folgt in M8). Der AUSLOESER ist echt:
Strom (3,126)=1, Aktionstaste auf dem Plattform-Platz (sce 3), Eingabe nur `RE15_INPUT_SCRIPT=A0.1` ab Bild 200.
```
Lift A hinauf:  [lift2030] Anker @0x01E30 -> Satz 0 (Takt 197)
                [se] Stimme: se=0 ... note=69 fine=0 center=93 vol=127 pitch=0x400 (11025 Hz)
                [lift2030] Anker @0x01E54 -> Satz 2 (Takt 197)
                [se] Stimme: se=2 ... note=60 fine=42 center=91 vol=127 pitch=0x2c4 (7622 Hz)
                [walk] F180 y -5400 / F210 -6000 / ... / F300 -10800  (60 je Bild = For 45 x 120 / 2 Bilder)
Lift A hinab:   Anker @0x01E30 -> Satz 0, Anker @0x01E98 -> Satz 2 (Takt 197); y -10800 -> -5400
Lift B hinauf:  Anker @0x01F0A -> Satz 0, Anker @0x01F2E -> Satz 1 (Takt 197); y -5400 -> -16200 bei F~377
Elza ROOM2031:  `Elza-Bit=1`, `RE15_GOTO_ROOM=2031`, `RE15_FORCE_EVENT=4@150` (sub04 = Lift B):
                Anker @0x01F0A -> Satz 0, Anker @0x01F2E -> Satz 1 (Takt 147), beide [se] Stimme
```
Unabhaengige Tonpruefung (`abn1/liftton.py`): EIGENER PSX-ADPCM-Dekoder auf die RE2-Bytes ROOM6030.RDT @0x22F08 (2496 B)
und @0x2F818 (34320 B; fuer Lift A ohne die Bloecke [794,1611)), Huelle je Spieltakt bei 11025 / 7622,8 Hz gegen die
Differenz der exe-Aufnahmen:
| Fall | Differenz | Korrelation Huelle | Massstab | Stopp-Schlag Aufnahme / Dekoder | Ende |
|---|---|---|---|---|---|
| Lift A hinauf | Takt 324..470 | 1.0000 (mittl. rel. Fehler 0.002) | 0.250 | +110 / +110 (Fahrt 90 + 20) | +146 = Welle 146 |
| Lift A hinab  | Takt 324..470 | 1.0000 (0.002) | 0.250 | +110 / +110 | +146 |
| Lift B hinauf | Takt 324..560 | 1.0000 (0.002) | 0.250 | +200 / +200 (Fahrt 180 + 20) | +236 = Welle 236 |
L = R in der Differenz (Port spielt nicht positional; RE2-Vorlage `36 02 11 00 00 00 ..`: Byte +3 FLAGS = 0, Lage 0 -
nach der Deutung des RE1.5-Handlers LAB_80041624 (scd_vm.c Z. 2009ff.: FLAGS != 0 -> Positional-Pfad) kein
Positional-Pfad; den RE2-Handler selbst habe ich nicht disassembliert).
Die Plattform kommt laut `[walk]`-Hoehe 90 bzw. 180 Bilder nach dem Fahrt-Ton an -> Stopp-Schlag 20 Bilder nach der
Ankunft wie RE2 (For 180 @0x02FDC endet T180, Schlag T200).

### M5 Nachtrag 5b - Gegenproben
* Ohne Strom (kein (3,126)), Aktionstaste auf Platz 5: keine `[lift2030]`-/`[se] Stimme`-Zeile; die Differenz besteht
  nur aus 7-Takt-Stoessen, die in BEIDEN Aufnahmen mit gleicher Form, aber an verschobenen Takten liegen (352/388,
  404/416, 421/496; im Log `[re2se] ENEMSE Bank 11`-Zeilen) = derselbe Ton zeitversetzt, kein Lift-Ton (Welle 146 Takte).
* Stand VOR der Spur (Hauptbaum-exe), Lift A hinauf mit Strom: die Plattform faehrt (`[scd F291] Cut_chg(10)`), Differenz
  der Aufnahmen mit/ohne Taste = **0 Takte** -> Befund 5b ("fehlt noch das sound") am alten Stand reproduziert.

### M6 Nebenwirkung P2DS auf S218 (ROOM4000 -> ROOM3070, Runde-33-Bestand)
* Selbst nachgerechnet (eigenes Skript `abn1/p2ds_mesh2.py`, do2_format-Leser, UV-Dreiecke je Mesh gegen das TIM):

  | Archiv | Mesh 0 (Buehne) Texel / davon Index 0 | Mesh 1 (Bedientafel) | Mesh 2 (Fachwerk) |
  |---|---|---|---|
  | P2DS master (Runde 33) | 18132 / 0 | 5580 / 5580 | **1737 / 1737** (ganz unsichtbar) |
  | P2DS neu (N1) | 18132 / 0 | 5580 / 5580 | 1737 / 0 |
  | RE2 DOOR2D | 18132 / 0 | 5580 / 0 | 1737 / 0 |

  Farben Mesh 2 neu = DOOR2D (max. RGB-Abweichung 0), Mesh 0 neu/alt je max. 8 (eine 5-Bit-Stufe, Runde-33-Rezept).
  DOOR2D-Skripte (tuerskript_dump.py dis, selbst gelesen): Skript 1/2 stellen obj 2..5 mit mesh 2 auf (obj 3/4/5
  Kinder von obj 2), Skript 4 @0x6300 / 5 @0x6344 bewegen obj 2 -> ohne Mesh-2-Texel bewegte sich in der alten P2DS
  nichts Sichtbares. Befund "Rezept nullte ganz Mesh 2" bestaetigt.
* `RE15_TUER_SEITE=S218` neu: `DOOR2D V4 ... Port-Archiv P2DS`, 301 Bilder; Bogen: Fachwerk erscheint oben (Bild 120/210)
  und steht 30 Bilder spaeter an der Gelaenderkante (150/240) = zieht nach unten = Buehne faehrt HINAUF. Runde 31
  stapel_5.pruefer.md Z. 459 (gemalt c01): "offener Aufzugschacht mit Fachwerk" - die Korrektur widerspricht dem Gemalten
  nicht. Die Aenderung an S218 ist sichtbar (Nutzer-Hinweis, kein Mangel: vorher stehendes Bild 301 Bilder lang).

### M7 #6/#7 nach der Nachbesserung (Regressionsprobe, echter Weg wie Abnahme 0 M1/M2)
* Einstieg 2F: `karte ... 10C0 1500 7000 1024 flag:4:243`, Skript `W1,A0.3,W1,A0.3` -> `DOOR FIRE slot=1 rect=(1500,6400,
  hw=1000,hh=500)` -> `Port-Archiv P27F Variante 0 ... Seite S040 T017` -> `room1080.rdt`.
* Ausstieg per Kabinen-Panel: `karte ... 1080 -12050 -670 3072 flag:4:243`, Skript `W1,A0.1,W1,U0.7,W0.5,A0.1` ->
  `DOOR FIRE slot=2 rect=(0,0,hw=0,hh=0)` -> `Port-Archiv P27C Variante 0 ... Seite S029 T018` -> `room1120.rdt`
  (die doppelte DOOR-FIRE-Zeile ist die in Abnahme 0 Abschnitt 8 notierte Beobachtung, eine Sequenz, ein Raumwechsel).
* Messfalle: in allen meinen Laeufen meldet debug.log `[pad] Controller: "Xbox 360 Controller" (Slot 0)` (fremde
  Agenten); alle Eingaben liefen ueber `RE15_INPUT_SCRIPT`, das die Tasten-Bits ganz ersetzt (input_pc.c Z. 440-445).

### M8 Nachtrag 5b - Weg ueber die Tuer (Spielstand ROOM2070, Tuer -> ROOM2030, zu Fuss zur Plattform)
* Spielstand `karte ... 2070 5500 -6400 0 flag:3:126` (Generator an), Skript `W1,A0.3,W1,A0.3,W1,A0.3` -> `DOOR FIRE slot=1
  rect=(5500,-6400,hw=500,hh=1500)` -> `Port-Archiv P1BD ... Seite S156` -> `room2030.rdt` (Ankunft (6550,-5400,-100)).
* Zu Fuss: der Steg (SCA-Zelle #38 x 5900..7250 z -10350..12800, Band 2/3, selbst aus RDT+0x20 gelesen) fuehrt gerade zur
  Plattform A (5850..7350 / 11350..12850). Im Port stehen dort 3 Gegner (Typ 25 bei (6918,5250), STATE_LOG) - ohne Kampf
  stoesst der Spieler bei z ~3920 an und wird zurueckgedraengt (zweimal gemessen). Deshalb Messtreiber
  `RE15_WEGPLAN` (dachkiste_r36.c, reine Pad-Steuerung) mit `GEGNERWEG` (Gegner freigegeben = Ersatz fuer das
  Erschiessen), `GEHE 6575 11900`, `BLICK 3072`, `AKTION`:
```
F31  GEGNERWEG -> 3 Gegner weg          F218 angekommen (6603,11800)       F233 AKTION
[lift2030] Anker @0x01E30 -> Satz 0 (Takt 230)      [se] Stimme: se=0 ... pitch=0x400 (11025 Hz)
[lift2030] Anker @0x01E54 -> Satz 2 (Takt 230)      [se] Stimme: se=2 ... pitch=0x2c4 (7622 Hz)
[walk] F300 y -9480 / F330 y -10800             F576 FLAG (3,222) = 1   (Lift A oben)
```
  -> der Ton kommt auch auf dem Weg Tuer -> Steg -> Plattform -> Aktionstaste.

## 3. RE-Gate (Stichproben SELBST gelesen/disassembliert)
1. **RE2 ROOM6170.RDT @0x035A2** (xxd): `3b 01 01 41 00 00 57 bc b4 ed 14 1e 80 0c f4 17 00 00 b5 6d 00 04 06 00 00 00 2d 05`
   -> Byte 22/23 = 06/00 (ROOM7000), Byte 26/27 = **2d 05** (DOOR2D V5). Eigener Rohscan aller PL0/PL1-RDTs (3b, sce 1,
   Byte 26 = 0x2D): nur ROOM6170 @0x035A2 und ROOMF170 @0x3572 (B-Fassung desselben Raums, gleiche Bytes) + ein
   Datenfund in ROOM6150 @0x158b3 (kein SCD-Satz) -> "einzige DOOR2D-Tuer" haelt.
2. **DOOR2D.DO2** (tuerskript_dump.py dis): Skript 0 @0x601c `13 0c 2e 00` Switch(var 12), Case 0/2/4 -> `18 01`,
   Case 1/3/5 -> `18 02`; Skript 2 @0x6130: Sleep 20, Se_on 0, Skript 3, Sleep 35, Skript 5, Sleep 120, Se_on 1,
   Sleep 245, Blende, Sleep 30 (= 451 Bilder); Skript 5 @0x6344 `34 0c 1c 0c` y 3100, `34 0b e8 ea` x -5400, For 60
   { `26 00 01 10 7d 00` y -= 125, `26 00 00 10 c8 00` x += 200, `02` Evt_next } x4; Skript 4 @0x6300 y -4400 / +125.
   **RE2 PSX.EXE Member-Tabelle** @0x80011228 (re2_disasm table): [11] -> 0x80055d2c `j 0x80055e30` mit Delay-Slot
   @0x80055d30 `sw a2,56(a0)`, [12] -> Delay-Slot @0x80055d38 `sw a2,60(a0)`, [13] -> @0x80055d40 `sw a2,64(a0)` =
   Member 11/12/13 = x/y/z -> Skript 5 = y faellt = Fachwerk steigt = Buehne HINAB. Haelt (Variante 5 richtig).
3. **RE2 ROOM6030.RDT** (xxd + eigener VH-Parser): @0x02F8C `22 02 07 01`, @0x02F90 `36 02 11 00 00..` (Se_on 0x11,
   Lage 0), @0x02F9C `09 0a 0a 00` Sleep 10, @0x02FBA `36 02 0b 00 00..` (Se_on 0x0B), @0x02FDC `0d 00 60 00 b4 00`
   For 180; Schleifenkoerper 0x2FE2..0x3041 mit genau EINEM `02` Evt_next (@0x03022) -> 180 Bilder, y += 30 (`26 00 00
   10 1e 00`). sub12 @0x030B6/@0x030BA/@0x030E4/@0x03106 und ROOMF030 @0x02E24/@0x02E28/@0x02E52/@0x02E74 gleich.
   Kopf +0x08/0C/10 = 0xE128/0xE1E8/0xF008; EDT 0x0B @0xE154 `00 01 13 02`, 0x11 @0xE16C `00 01 a3 00`; VH: Prog 1 Ton 1
   @0xEC28 (vol 127, center 91, shift 42, note 60) -> **VAG 22 @0x2F818 34320 B**, Ton 10 @0xED48 (center 93, shift 0,
   note 69) -> **VAG 15 @0x22F08 2496 B**. LIFT6030.VBS: KLICK und FAHRT bytegleich diesen Bytes, FAHRT_KURZ = FAHRT ohne
   Bloecke [794,1611) bytegleich. Werkzeug `re2_lift6030_cut.py` in einer Scratch-Kopie neu laufen lassen: VBS und
   `gen/re2_lift6030_bank.inc` **bytegleich** reproduziert.
4. **RE1.5-RDT-Bytes** (xxd): ROOM3080 = ROOM3081 @0x0072A `3b 00 02 31 00 00 | 00x8 | 50 e2 00 00 98 e5 80 07 | 03 00
   01 00 00` (Null-Rechteck, Stage 3 Raum 00 Cut 1, Archiv-Byte 0); 3080 sub00 @0x007A8 `21 04 60 00` -> @0x007E8
   `04 ff 18 02` / @0x007F0 `04 ff 18 09`; sub02 @0x007FA `22 04 60 01`, @0x007FE `22 02 07 01`, @0x00802 `22 01 1b 01`,
   @0x00A1A `2b 01 00 00`, @0x00A1E `09 0a 64 00`, @0x00A22 `47 00`; sub09 @0x00BBA/@0x00BBE, @0x00BFA `47 00`; 3081
   @0x0081E/@0x00862 `47 00`. ROOM2030 = ROOM2031 (0x1D00..0x2100 bytegleich): @0x01E30 `22 02 07 01`, @0x01E50
   `2f 01 88 ff`, @0x01E54 `0d 00 06 00 2d 00` + Koerper `30 02 02 00 0e 00` (Add_speed, 2x Evt_next), @0x01E98 dito
   (`2f 01 78 00`), @0x01F0A `22 02 07 01`, @0x01F2E/@0x01F72 `0d 00 06 00 5a 00`; kein Se_on im ROOM2030-SCD
   (scd_dump_room: 0 Treffer). **RE1.5 PSX.EXE Aot_on @0x800407bc** (re15_disasm): Platz aus `lbu v0,1(v0)`, Satz aus
   0x800ac9b0[Platz], Nutzlast +12/+20, Handler aus 0x8007469c[sce], `jalr v0` @0x8004082c = Tuer-Handler. Alle
   Offsets im Code (lastenaufzug_r37.c, lift2030_r37.c, Riegel) stimmen.

* Konstanten im N1-Diff: jede mit @0x/Byte-Offset (lastenaufzug_r37.c: Stage/Raum @0x0072A +22/+23, Satz 0x0072A,
  Archiv 0x2D @0x035A2 Byte 26, Variante 5 Byte 27 + @0x601c, Tuer 112 = Runde-31-Nummer nur Protokoll; lift2030_r37.c:
  sechs Anker mit Datei-Offset + 8-Byte-Signatur, Satzwahl nach RE2 @0x02F90/@0x02FBA; gen-Datei mit Herkunft) bzw.
  als PORT-WAHL gekennzeichnet (FAHRT_KURZ, Herleitung im Werkzeug, reproduzierbar). Suche im N1-Diff nach
  deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/geschaetzt/ungefaehr/vermutlich/approx: **kein Treffer**;
  **kein neuer getenv-Schalter**.
* Fix erklaert den Befund: (a) alter Stand = Null-Rechteck-Satz ohne Tabellenzeile + player_mode 2 -> Runde-31-Sperre
  -> RE1.5-Uebergang ohne Sequenz (M2); der Haken stellt die Anfrage vor der Sperre (M1). (b) alter Stand = RE1.5 ohne
  Se_on, Port ohne Anker -> Differenz 0 (M5); Anker im VM-Takt spielen die RE2-Saetze (M4).
* Kleiner Doku-Widerspruch (kein Verhalten): Kopfkommentar von `tools/fahrstuhl_r37/re2_lift6030_cut.py` Z. 17/33 nennt
  "Stopp-Schlag ab T199 = 19 Bilder", das Werkzeug selbst druckt `T200 (= 20 Bilder nach Fahrt-Ende T180)`; gen-Datei,
  Riegel und meine Messung sagen 20.

## 4. Vertrag / Pfade / Nebenwirkungen
* Keine Bank-9-Bits, keine Nachrichten-Ids, keine Ereignis-Nummern/AOT-Slots belegt (nur gelesen: (3,126), (3,220),
  (3,222), (4,96), (4,0xFD)).
* Pfade (`git diff master...HEAD --name-only` gegen `^(release/|re15_port/platform/android/|re15_port/shared_assets/PSX/)|
  CMakeLists`): **kein Treffer**. Neue Logik in neuen Dateien (lastenaufzug_r37.c/.h, lift2030_r37.c/.h,
  platform/pc/src/lift2030_se_pc.c); gemeinsame Dateien nur kleine Haken mit Kommentar "Runde 37 Spur C N1"
  (aot_common.c +1 Include +3, scd_vm.c +1 Include +2 +2, main.c +1). Tests nur ueber probes/r37_fahrstuhl.cmake;
  test_r37_fahrstuhl_n1.cmake hat `cmake_minimum_required(VERSION 3.16)`, Messkopie der exe unter eigenem Namen,
  `m` unter NOT WIN32 gelinkt, kein popen.
* Assets: NEU shared_assets/RE2/LIFT6030.VBS (Paragraf 1.6 erlaubt, Paket-Gate-Eintrag im Dossier N1-7 genannt);
  GEAENDERT shared_assets/RE15DOOR/P2DS.DO2 (Port-Archiv der Runde 33, nicht PSX/*; begruendet, s. M6).
* Probe-Merge (`git merge-tree`) r37/fahrstuhl gegen r37/itemband, beretta, pri1100: sauber; gegen r37/integration,
  r37/codepanel, r37/zombieaktiv: Textkonflikt nur in scd_vm.c an den Haken-Zeilen (Include-Liste, Raum-Scan, Takt-
  Haken direkt hinter dem Elza-Anker - beide Seiten behalten; die Haken sind raumgetrennt: Lift nur 2030/2031,
  Code-Panel nur 10D0/11E0/1230 -> die break/continue-Logik des Code-Panel-Hakens beruehrt den Lift nicht);
  sonst nur AUFTRAG.md (Doku).
* Nebenwirkungen: Lastenaufzug-Haken greift nur bei Raum 0x308x (x<=1) + Null-Rechteck + Ziel Stage 3 Raum 00; Lift-
  Haken nur bei armierten Ankern (Wachposten 0 ausserhalb 2030/2031, Signatur-Pruefung "lieber stumm als falsch").
  Die Kabinen-Ausstiege (#7) kommen im Haken VOR dem Lastenaufzug (Reihenfolge unveraendert). P2DS -> S218 s. M6.

## 5. Tests (gezielt, KEINE volle Suite)
Regex = Spur + Gruppen aller beruehrten Module (Tuersequenz/P2DS/Zuordnung, AOT-Feuerweg, SCD-VM-Anker inkl. Elza/
Hebetisch/Kartenhinweis, Ton/SE/Audio, Fahrstuhl, Kopflauf) - wie der Bau-Agent plus `lift|stage2|2030|3080|lastenaufzug`:
```
ctest --test-dir re15_port/build -R "r37_fahrstuhl|r36_tueren|r33_tuer|unit_r31_|r32_unterteilung|unit_door_seq|
  r35_inhalt_tueren|fahrstuhl|elevator|elev_se|r30_tuer|door_|keydoor|unit_aot_|unit_scd_|prop_scd|unit_tor_1170|
  r35_cut10f0_tuerton|elza|hebetisch|map_hint|integration_headless_run|r34_plattform_ton|unit_se_|unit_audio|r35_spiel|
  tuer|lift|stage2|2030|3080|lastenaufzug" --timeout 900 -j 2 --output-on-failure
100% tests passed, 0 tests failed out of 101          Total Test time (real) = 638.16 sec   (unter Fremdlast)
```
u. a. integration_r37_fahrstuhl_n1 135,8 s, integration_r37_fahrstuhl 146,4 s, integration_r35_karte_fahrstuhl 179,9 s,
integration_r36_story_elza 220,1 s, integration_r36_tueren 86,6 s, integration_elza_vollstart, integration_r36_elza_*,
integration_headless_run, unit_r37_fahrstuhl_* (7), unit_r33_tueren_archive/_zuordnung/_maschine (P2DS neu),
unit_r36_tueren_fahrstuhl, unit_elev_se, unit_fahrstuhl_1080/4020_etagen, unit_se_*, unit_audio_*. Kein roter Haken,
nichts nachzufahren. Riegel messen: lastenaufzug/durchlauf verlangen GENAU eine Anfrage P2DS V5 (sonst "Laeufer 0 x"),
lift2030 verlangt KLICK + FAHRT[_KURZ] im selben Takt und 90/180 Bilder bis zum Etagen-Merker, Gegenprobe ohne Strom
0 Toene; die exe-Gegenproben am alten Stand (M2, M5) zeigen genau die Befunde, die die Riegel ROT machen wuerden.

## 6. Urteile

| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| #6 Fahrstuhltuer in der Tueranimation korrekt (sonst neu bauen) | **erfuellt** | Seit Abnahme 0 unveraendert (P27F/P27C, fahrstuhl_r37.c, Riegel: `git diff 06bf9b12..HEAD` leer); echter Einstieg 2F per Spielstand -> `Port-Archiv P27F ... S040` (M7); integration_r37_fahrstuhl + unit_r37_fahrstuhl_archiv/_einstieg gruen. |
| #7 Tuersequenz beim Aussteigen | **erfuellt** | Echter Panel-Weg 1080 -> 1120 -> `Port-Archiv P27C ... S029` (M7); Kabinen-Haken steht weiter VOR dem neuen Haken; unit_r37_fahrstuhl_ausstieg gruen. |
| Nachtrag 5a "ja, dann mache den noch" (Ausstieg Lastenaufzug 3080 -> 4000 mit Tuersequenz) | **erfuellt** | Story-Weg per Spielstand 3070 (in den Szenen-Platz gegangen) -> Szene -> 3080-Fahrt -> `Port-Archiv P2DS Variante 5 ... 451 Bilder, 2 Se_on` -> room4000, Ton der Sequenz abtastwertgleich (M1); alter Stand ohne Sequenz (M2); Elza sub02/sub03 und Leon sub09 ebenso (M3); RE2-Herleitung DOOR2D V5 = hinab selbst belegt (Abschnitt 3, 1-2); P2DS zeigt das Fachwerk (M6). |
| Nachtrag 5b "in den Stage2 kleineres aufzuegen fehlt noch das sound ... aus Resident Evil 2 extrahieren" | **erfuellt** | Einzige STAGE2-Aufzuege = zwei Plattformen ROOM2030/2031 (eigener Nachrichten-Zensus: nur 2030 msg 1 @0x2038 "The elevator..."); RE2-Ton ROOM6030 bytegleich (eigener VH-Parser + eigener ADPCM-Dekoder, Huellen-Korrelation 1.0000), Lift A/B hinauf/hinab + Elza 2031 (M4), Weg ueber die Tuer (M8), ohne Strom stumm, alter Stand stumm (M5); RE2-Platz (Klick bei Set(2,7), Fahrt vor der Fahrschleife, Halt 20 Bilder nach Ankunft) belegt (Abschnitt 3, 3). |

Gates: RE-Gate haelt (6 Stellen selbst gelesen/disassembliert, inkl. RE2 PSX.EXE @0x80011228/@0x80055d30 und RE1.5
PSX.EXE @0x800407bc; Laenge der RE1.5-Schleife zusaetzlich gegen die Opcode-Tabelle @0x800744A8 geprueft: [0x02] =
0x8003F258 Evt_next `addiu v0,v0,1` @0x8003f260 + Rueckgabe 2, [0x30] = 0x80040F40 Add_speed `addiu v0,v0,1` @0x80040fc4
-> Koerper `30 02 02 00 0e 00` = 2 Bilder je Runde -> Lift A 90 / Lift B 180 Bilder, Grundlage von FAHRT_KURZ haelt);
Vertrag haelt; gezielte Tests 101/101 gruen; Tests je Punkt vorhanden und messend.

## 7. Maengel (nummeriert)
Keine. (Zu #6/#7/5a/5b kein Befund, der gegen den Wortlaut steht.)

## 8. Hinweise (kein Mangel; fuer Nutzer/Orchestrator)
1. **Abwaerts jetzt drei Fahrt-Abschnitte**: RE1.5-Fahrt in ROOM3080 (erste Fahrt mit Ada ~907 Bilder, weitere ~280),
   dann die RE2-Hubbuehne 451 Bilder (15 s, RE2-Laenge von DOOR2D Skript 2 @0x6130), dann die RE1.5-Ankunft in ROOM4000
   (Spieler y -1680 -> 0). So beauftragt ("DOOR2D-Variante abwaerts"), aber laenger als vorher - Nutzer ggf. fragen.
2. **S218 (ROOM4000 -> Lager, aufwaerts) sieht jetzt anders aus**: das rote Schacht-Fachwerk zieht vorbei statt eines
   301 Bilder stehenden Bildes (Korrektur am Runde-33-Rezept, M6).
3. Lift A: Motor-Mittelteil der RE2-Welle um 817 ADPCM-Bloecke gekuerzt (PORT-WAHL, reproduzierbar); Klick und Motor
   starten im selben Bild (RE2: 10 Bilder Abstand @0x02F9C; RE1.5 hat dort kein Sleep - RE1.5-Zeitachse behalten).
4. Paket: `release/make_package.sh` kopiert shared_assets/RE2 und RE15DOOR ganz (Z. 663/665) - LIFT6030.VBS kommt also
   mit; der Pflicht-Eintrag in check_tree (Muster ELZASE.VBS Z. 269-271, `-s` + `cmp`) fehlt noch (Orchestrator,
   release/ tabu). Android: drei neue Quelldateien -> GLOB-Cache (app/.cxx) neu konfigurieren.
5. Mischen: scd_vm.c kollidiert textlich mit r37/integration (Code-Panel, Spur E) und r37/zombieaktiv (nur Include) -
   beide Seiten behalten, die Haken sind raumgetrennt.
6. Doku-Kleinigkeit: Kopfkommentar `tools/fahrstuhl_r37/re2_lift6030_cut.py` sagt "T199 = 19 Bilder", Werkzeug-Ausgabe,
   gen-Datei und Riegel sagen 20 (kein Verhalten).
7. ROOM2030 im Port: 3 Gegner (Typ 25) auf dem Steg zu Lift A draengen den Spieler bei z ~3920 zurueck (M8, nur
   beobachtet, nicht Spur C, Ursache nicht untersucht).
8. Einstieg in den Lastenaufzug: das Gitter S195 feuert IN der ROOM3070-Szene und bleibt wegen der Runde-31-Sperre ohne
   Sequenz (M1, unveraendert, nicht Teil des Nachtrags).

Stand: FERTIG (Abnahme 1). Messkopien der exe (`re15_pc_c_abn1_mess.exe`, `re15_pc_c_abn1_vorher.exe`) danach geloescht;
Messordner nur im Scratchpad.
