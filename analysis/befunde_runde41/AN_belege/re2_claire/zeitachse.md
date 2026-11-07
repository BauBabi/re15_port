# RE2 Claire (PAL): "Annette trifft den mutierten William" - echte Aufnahme, Zeitachse, RAM

Spur AN, Runde 41. Quelle: echte Disc `C:/Users/mjoedicke/Downloads/ePSXe2018/Resident Evil 2 Claire.cue` (SLES-10972, EXE
`SLES_109.72`, europaeische PAL-Fassung), DuckStation 0.1-11580-ge7f2f101c. Szene = `PL1/RDT/ROOM6141.RDT` MAIN sub02
@0x00E32..0x01192. Alle Zeiten sind Videozeit von **Lauf 1** (`re2c_lauf1_voll.mp4`, t = 0 beim Start der Aufnahme).
Clip `re2c_szene_320.mp4` = Lauf 1 ab 1,00 s, also **Clipzeit = Videozeit - 1,00 s**.

## 0. Kurzfassung

- **Birkin kommt von oben.** Er steht ab Raumeintritt geparkt ueber der Decke (Slot 1, Typ 0x33, (-6600, -9000, -14800),
  Richtung 2048). `Member_set 0x17 = 1` @0x010CA setzt ihn in Gang: y -9000 -> -5250 (58,81 s) -> 0 (59,61 s), x/z bleiben.
  Er faellt in Schnitt 2 senkrecht zwischen Kamera und Annette, Ruecken zur Kamera, Blick nach Westen auf Annette
  (`re2c_09b_birkin_faellt_dicht.jpg`, sichtbar 59,05-59,5 s). Landung Bewegung 0x0E, danach Stand Bewegung 0x00.
- **Angriff = Hieb, kein Griff, kein Wurf.** `Member_set 0x17 = 2` @0x01142 (Schnitt 0x0A, Kamera tief vor Birkin, also von
  Annettes Seite): Birkin holt mit dem grossen Klauenarm aus (70,2-70,6 s), schlaegt quer durch (70,7-71,1 s), Blut spritzt
  links und rechts ins Bild (ab 71,09 s), er stoesst dabei nach vorn auf die Kamera zu (`re2c_12b_hieb_dicht.jpg`).
  RAM: Bewegung 0x07, x -6600 -> -6508 (Ausholen) -> -6802 -> **-7997** (Vorstoss 1397 Einheiten nach Westen) -> -7753 ->
  -7544 (zurueck, Routine 3 = Stand). Annette steht dabei bei (-11097, 0, -15001) und spielt synchron ihre Bewegung 0x02
  (`Plc_motion(2,2,0x44)` @0x01138); die Bildzaehler beider laufen gleich (5/6, 25/26, 45/46).
- **Annette liegt** danach per `Pos_set` @0x01162 bei **(-12300, 900, -15300), Richtung 3300 (0x0CE4), Bewegung 0x18**
  (Schleife), also 1203 Einheiten weiter westlich als sie stand, y +900. Dieselbe Lage setzt MAIN sub00 im else-Zweig
  @0x00D16 fuer den spaeteren Besuch (`44 00 00 49 00 09 .. f4 cf 84 03 3c c4 e4 0c ..`). **Sie ist in keinem Bild dieser
  Szene liegend zu sehen.**
- **Schnitt 6 zeigt Claire, nicht Annette:** Claire allein vor der Tuer "P-4 LABORATORY" (feste Kamera), dazu Annettes
  Schrei (Xa 0x32) aus dem Off; 73,79-74,08 s gehen die Kinobalken auf = Steuerung zurueck (`re2c_13_...jpg`).
- Bildtakt: DuckStation lief als **PAL-Konsole, 49,74682 Bilder/s**; RE2 rechnet **1 Spielbild = 2 Bildwechsel = 40,2 ms
  (25 Spielbilder/s)** (Abschnitt 3). Sleeps im Skript zaehlen Spielbilder; an Schnittkanten kommt die Ladezeit dazu.

## 1. Weg

### 1.1 Disc, Emulator, Einstellungen

- `SYSTEM.CNF` `BOOT = cdrom:SLES_109.72;1`; DuckStation-Log: `Game: Resident Evil 2 (Disc 2) | SLES-10972 |
  HASH-FDE121340177D3BC`, `Auto-detected console PAL region`, `Console Region: PAL`. Es gibt kein PAL-BIOS auf dem Rechner:
  `BIOS region NTSC-U does not match requested region PAL`, benutzt `SCPH-1001 (v2.2 12-04-95 A)`. Die Konsole (Bildtakt)
  ist trotzdem PAL. Media-Capture: `Video: 640x480 FPS=49.74682, Codec=libx264`, `Audio: SampleRate=44100` (AAC).
- `settings.ini` vorher gesichert (`%LOCALAPPDATA%/DuckStation/settings.ini.vor_annette_claire_2026-11-02`), fuer die Laeufe
  geaendert: `SaveSelectedSaveState = SDL-0/RightShoulder` (statt `Keyboard/F2`), fuer Lauf 2/3 zusaetzlich
  `R1 = Keyboard/E` (RB vom Spiel-R1 geloest, sonst zielt Claire bei jedem Savestate), zur Fehlersuche `LogToFile = true`.
  `ToggleMediaCapture = SDL-0/LeftShoulder` war schon gebunden. Danach Sicherung zurueckkopiert, `cmp` = bytegleich
  (md5 32b482961684406328c37307a2ae52b4, gleich dem Stand vor der G5-Abnahme). Die Claire-Savestate-Dateien
  `SLES-10972_1.sav/.bak/_resume.sav` aus `savestates/` in den Scratchpad verschoben. Eine Claire-Speicherkarte wurde nicht
  angelegt (das Spiel hat nie gespeichert). Nebenwirkung: `duckstation.log` ist durch `LogToFile` neu geschrieben
  (Kopie: Scratchpad `capture/duckstation_letzter_lauf.log`).
- Steuerung: virtuelles Pad (vgamepad), `werkzeug/re2c_drv.py` (Befehlsdatei, `snap` = RB + Kopie + Vorschaubild aus dem
  Savestate), `werkzeug/cmd.py` (Befehle anhaengen, auf Marke warten).

### 1.2 Bis zum ersten steuerbaren Raum

Titel -> NEW GAME -> NORMAL (Kreuz) -> Vorspann (~3 min, ganz durchlaufen lassen) -> Strasse, Claire steuerbar
(`capture/boot/b41.png`, `b42.png`). Fehlversuche: START auf dem Titel waehlt LOAD GAME ("There are no files", zurueck ueber
"Don't use memory card"); START waehrend des ersten Vorspanns fuehrte zurueck zum Titel (nicht weiter untersucht).
Savestate `capture/boot/b42.sav` = Startraum (Stage 1, Raum 0x00), Claire bei (18327, 0, -2167), sechs Zombies (Typ 0x1F).

### 1.3 RAM-Adressen der PAL-EXE (gefunden ueber maskierten Code-Vergleich mit Leon DS SLUS_007.48, `werkzeug/exe_map.py`)

Instruktionsworte wie sie disassembliert werden (im Speicher little-endian).

| Zweck | Claire PAL SLES_109.72 | Beleg (Adresse: Wort Befehl) | Leon DS SLUS_007.48 |
|---|---|---|---|
| AOT-Tabelle (Eintrag = Befehl+2) | **0x800c6130** | Aot_set-Handler @0x80051080 `3c03800c` lui v1,0x800c; @0x80051084 `24636130` addiu v1,0x6130; @0x800510c0 `24420002` (+2), @0x800510c4 `ac620000` sw | 0x800ce558 |
| Flag-Zeigertabelle (Ck/Set) | **0x8009f8e4** | @0x8004e164 `3c01800a`; @0x8004e16c `8c23f8e4` lw v1,-0x71c(at); Wort: @0x8004e160 `000510c2` srl a1,3, @0x8004e170 `3042001c` andi 0x1c; Maske: @0x8004e180 `3c038000`, @0x8004e188 `00431806` srlv | 0x800a78c8 |
| Schnitt (u16) | **0x800cc3b4** | Cut-Kern @0x80050e08 `3c02800d`; @0x80050e0c `2442c3b4`; @0x80050e18 `a4440000` sh a0,0(v0) | 0x800d4820 |
| Spieler-Zeiger | **0x800c79a8** | Work_set Fall 1 @0x80051cc0 `3c02800c`; @0x80051cc4 `8c4279a8`; @0x80051ccc `ac820154` | 0x800cfe14 |
| Gegner-Zeiger [i] | **0x800c79b0 + 4i** | Work_set Fall 3 @0x80051ce4 `3c01800c`; @0x80051cec `8c2279b0` | 0x800cfe1c |
| Work_set-Sprungtabelle | 0x80010ec8 | @0x80051ca8 `3c018001`; @0x80051cb0 `8c220ec8` | 0x800111f0 |
| Member_set-Tabelle | 0x80010f00 | @0x80052020 `3c018001`; @0x80052028 `8c220f00`; Eintraege [0x0B..0x0D] `sw a2,0x38/0x3c/0x40(a0)`, [0x0F] `sh a2,0x76(a0)`, [0x17] @0x800520f0 `sh a2,0x1d4(a0)` (Offsets gleich Leon) | 0x80011228 |
| SCD-Befehlstabelle | 0x8009f4f8 | Interpreter @0x80013b88 `3c13800a`; @0x80013b8c `2673f4f8`; Eintrag 0x3F @0x8009f5f4 = 0x80052ef0 | 0x800a74c8 |
| Plc_motion | 0x80052ef0 | `90450002` lbu a1,2; `a060014d` sb 0,0x14d; `a46001d4` sh 0,0x1d4; `34420004` ori 4 -> `ac620004` sw +4; `a065014c` sb a1,0x14c; `a46601cc` sh a2,0x1cc | 0x80056b8c |
| SCD-Aufgabe 2 (sub02) | Feld 0x800cf340, 0x174 je Aufgabe; **Aufgabe 2 = 0x800cf628**, Zeiger +0x1C = **0x800cf644**, Ruecksprung 0x800cf76c | Interpreter @0x80013bb4 `3c02800d`/@0x80013bb8 `2442f340`; @0x80013bd0 `8e02001c` lw v0,0x1c(s0); @0x80013bd8 `90420000` lbu Befehl; @0x80013bf0 `0040f809` jalr; @0x80013c70 `26310174` (+0x174); gosub-Stapel +0x140 (@0x80013c2c `8e030140`) | - |
| Spielbild-Takt | VSync(u8 **0x800d7662**) | @0x8002ade4 `3c04800d`; @0x8002ade8 `90847662` lbu a0,0x7662(a0); @0x8002adec `0c0204d4` jal 0x80081350 (libetc VSync; Vcount 0x800a3b50, @0x800813b8 `8c423b50`) | - |
| Xa-Tabelle STAGE6 | 0x8011A4D4 (Zeiger nach 0x800c611c, STAGE6.BIN @0x34 `0c 80 01 3c 1c 61 22 ac`) | Vorgabe des Auftrags | 0x800ce544 |

Gleiche EXE im Savestate: `werkzeug/re2c_ss.py` findet `SLES_109.72[0x800:0x830]` im RAM (RAM-Basis im Zustandsblock 0x1a62).

### 1.4 Einstieg (nur RAM-Bytes, dann echt durch die Tuer)

`werkzeug/mk_6141.py capture/boot/b42.sav capture/p6141.sav 0 --weg`:

- AOT 0 (Tabelle 0x800c6130[0] = 0x8011b7c0 -> Befehl @0x8011b7be) ist die Tuer der Strasse
  `3b 00 01 31 00 00 75 a9 44 b1 ea 06 e8 08 1c c4 00 00 e9 d1 28 07 00 01 00 00 0b 00 00 00 00 00` (Ziel Stage 1 Raum 1).
  Neu: `3b 00 01 31 00 00 df 3b d1 eb 70 17 70 17 1b b0 00 00 84 a2 14 f8 05 14 06 00 26 00 00 00 00 00` =
  Rechteck (15327, -5167, 6000 x 6000) um Claire; ab +14 Byte fuer Byte die echte Tuer ROOM6151 -> ROOM6141
  (`PL1/RDT/ROOM6151.RDT` INIT sub00 @0x02B96 `... 1b b0 00 00 84 a2 14 f8 05 14 06 00 26 00 ...`): Lage (-20453, 0, -23932),
  Richtung 0xF814, Stage-Byte 5 (= Stage 6), Raum 0x14, Schnitt 6, Tuerbild 0x26.
- Flag (0x22, 0x2B): Tabelle[0x22] = 0x800cc470 (EXE-Daten) -> Wort 0x800cc474 `00000000 -> 00100000`.
  (1,1) = 0 (Wort 0x800c776c = 0x80000040, Maske 0x40000000), (3,0x42) = 0 (Wort 0x800cc3d0 = 0). Damit laeuft MAIN sub00
  @0x00C58/@0x00C60/@0x00C68 in den Zweig mit den beiden Sce_em_set und `Evt_exec sub02` @0x00D0E.
- `--weg`: die sechs Zombies des Startraums (Gegner-Zeiger 0x800c79b0[0..5]) auf x = -15000 gesetzt. Grund: erster Versuch
  ohne das -> Kreuz kam vor dem fertigen Laden des Savestates, Claire wurde gepackt (Game Over, `capture/test/t02..t04.png`).
  Betrifft nur den Startraum.
- Im Emulator: Savestate laden, 10 s warten, Kreuz -> Tuerbild 0x26 (Labortuer "TYPE-P") -> ROOM6141.
- Befund am RAM nach dem Eintritt: (2,7) und (1,0x1B) werden von sub02 @0x00E32/@0x00E36 gesetzt, am Ende (5,0x0C) und
  (3,0x42) gesetzt @0x01186/@0x0118A, sub08 loescht (2,7)/(1,0x1B) (Staende s10, e24, e25). Ab dem Raumeintritt laeuft
  nur RE2s eigener Code; geaendert sind nur Startraum-Bytes und das eine Vorbedingungs-Flag.
- Fehlversuch: DuckStation `-statefile` mit Vorwaertsstrich-Pfad laedt nichts und beendet sich still (Log ohne
  "Loading state"); mit Backslash-Pfad geht es (`re2c_drv.py` macht `os.path.abspath`).

### 1.5 Laeufe

| Lauf | Inhalt | Rohdaten (Scratchpad `.../scratchpad/an/capture/`) |
|---|---|---|
| 1 | Video ungestoert, Aufnahme 10 s nach Laden gestartet, Kreuz nach weiteren 2 s, 80,89 s | `lauf1/re2c_lauf1_voll.mp4`, `lauf1/log.txt` |
| 2 | Video + 67 Savestates im Abstand ~1,0 s (s00 vor der Tuer .. s66 = Schnitt 0x0B) | `lauf2/re2c_lauf2_voll.mp4`, `lauf2/s00..s66.sav/.png`, `ram.txt`, `zeit.txt` |
| 3 | Video + 30 Savestates ~0,8 s ab 52 s nach der Tuer (Birkin-Fall bis Steuerung) | `lauf3/re2c_lauf3_voll.mp4`, `lauf3/e01..e30.sav/.png`, `ram.txt`, `zeit.txt` |

**Deterministisch:** Lauf 3 hat alle 12 Schnittkanten auf demselben Videobild wie Lauf 1 (594 ... 3593); Lauf 2 hat alle
10 erfassten Kanten genau 44 Bilder spaeter (Aufnahme startete 44 Bilder frueher). Die Savestates verschieben die
emulierte Zeitachse also nicht; RAM-Staende aus Lauf 2/3 gelten fuer das Video von Lauf 1. Videozeit je Stand:
`werkzeug/snap_zeit.py` (Vorschaubild gegen Laufvideo, Versatz Videozeit-Treiberlog Median, Streuung 0,031 s / 0,040 s).

## 2. Zeitachse (Lauf 1)

Schnittkanten per Bilddifferenz (`werkzeug/schnitte.py`, 160x120 grau, mittlere Absolutdifferenz > 17). Xa-Einsaetze per
Kreuzkorrelation der Tonspur mit den aus VOICE16.XAS dekodierten Eintraegen (`werkzeug/xa_einsatz.py`, r = Korrelation).
"Spielbild" = (Videobild - 594) / 2 = Spielbilder seit der Einblendung, Ladestillstand eingeschlossen.

| Video | Videobild | Spielbild | Schnitt | Skript | Inhalt |
|---|---|---|---|---|---|
| 1,43-1,93 s | 71-96 | | Strasse | Tuer AOT | Bild schliesst sich (Tuer ausgeloest) |
| 2,13-7,90 s | 106-393 | | Tuerbild | | Labortuer "TYPE-P" (Tuerbild 0x26) oeffnet, Kamera faehrt hinein |
| 7,92-11,56 s | | | schwarz | | Laden ROOM6141 (RAM s01..s10: Schnitt 6 = Tuerschnitt, sub02 steht ab 11,20 s @0x00E44) |
| **11,94 s** | 594 | 0 | **7** | cut_chg @0x00E44 | Einblendung: Claire links, Annette rechts (Totale an der Labortuer) |
| 13,05 s | 649 | 28 | 7 | Xa 2A @0x00E94 | r = 0,987, 0,75 s |
| **14,96 s** | 744 | 75 | **8** | @0x00EB4 | ueber Claires Schulter, Annette zielt mit der Pistole |
| 14,94 s | 743 | 75 | 8 | Xa 2B @0x00EBA | r = 0,939, 3,31 s |
| **19,72 s** | 981 | 194 | **9** | @0x00EEA | Claire von vorn, Annettes Arm mit Pistole |
| 19,70 s | 980 | 193 | 9 | Xa 2C @0x00EF0 | r = 0,938, 0,75 s |
| **22,62 s** | 1125 | 266 | **8** | @0x00F0C | |
| 22,58-34,95 s | 1123 | 265 | 8 | Xa 2D @0x00F16 | r = 0,978, 12,37 s |
| **36,18 s** | 1800 | 603 | **9** | @0x00F9A | |
| 36,16-46,72 s | 1799 | 602 | 9 | Xa 2E @0x00FA0 | r = 0,984, 10,56 s |
| **48,37 s** | 2406 | 906 | **8** | @0x0100A | Bild steht 48,37-50,09 s still (Bilddifferenz hoechstens 0,08 von 255) |
| 48,33 s | 2404 | 905 | 8 | Xa 2F @0x01010 | r = 0,458 (kurz, 0,64 s) |
| 49,40 s | 2457 | 932 | (8 im Bild) | Se_on @0x01040 | **Krach**; RAM s49 (49,33 s): Zeiger steht auf cut_chg 7 @0x01024 |
| **50,09 s** | 2492 | 949 | **7** | @0x01024 | Claire (Plc_neck @0x0105A) und Annette (Plc_neck @0x01068) wenden den Kopf zu (-6600, 0, -14800) = Birkins Stelle; Annette zielt weiter |
| 52,93 s | 2633 | 1020 | 7 | Xa 30 @0x0107A | "William." r = 0,850, 1,17 s |
| ~54,3-55,4 s | | | 7 | Plc_dest @0x010A2 | Annette geht rechts aus dem Bild (Ziel (-20000,-15400)) |
| **57,47 s** | 2859 | 1133 | **2** | @0x010B2 | Gang, Kamera im Osten nach Westen: Annette kommt den Gang herauf (Ziele (-15000,-15000), (-11000,-15000)) |
| 58,70 s | 2920 | 1163 | 2 | Member_set 0x17=1 @0x010CA, Se_on @0x010CE | lauter Ton; Birkin faellt (RAM e06 58,81 s y -5250) |
| 59,05-59,5 s | | | 2 | | Birkin faellt von oben ins Bild, landet mit dem Ruecken zur Kamera zwischen Kamera und Annette |
| **61,17 s** | 3043 | 1225 | **0x0A** | @0x01102 | Birkin von vorn unten (Annettes Seite), steht; Sleep 60 @0x01110 |
| 60,85-61,25 s | | | | Se_on @0x01104 | Ton steigt ab 60,85 s, Spitze 61,25 s |
| **64,23 s** | 3195 | 1301 | **0x0B** | @0x01114 | Annette von oben auf dem Gitterboden, Pistole in der Hand, Blick nach oben (Plc_neck @0x01128 auf (-6600,-4500,-14800)) |
| 64,20-68,14 s | 3194 | 1300 | 0x0B | Xa 31 @0x0111A | "W-William! You're alive!" r = 0,999, 3,95 s |
| **69,77 s** | 3471 | 1439 | **0x0A** | @0x0113C, Member_set 0x17=2 @0x01142 | Birkin holt aus 70,2-70,6 s, Hieb 70,7-71,1 s, Blut ab 71,09 s, stoesst zur Kamera vor |
| **72,23 s** | 3593 | 1500 | **6** | @0x0114A | Claire allein vor "P-4 LABORATORY" |
| 72,20-73,59 s | 3592 | 1499 | 6 | Xa 32 @0x01150 | Schrei (Annette, aus dem Off) r = 0,955, 1,39 s |
| 73,79-74,08 s | 3671-3685 | 1539-1546 | 6 | gosub sub08 @0x0118E | Kinobalken oeffnen sich = Steuerung zurueck (RAM e25 73,92 s: Claire Routine 1, Zeiger @0x01190) |

Clip: Videozeit - 1,00 s. Bildleisten: `re2c_01_tuer_und_eintritt.jpg` .. `re2c_13_schnitt06_schrei_steuerung.jpg` (je Schnitt
4-8 Bilder, beschriftet mit Videozeit und Videobild), dazu dicht `re2c_09b_birkin_faellt_dicht.jpg` (58,6-61,1 s, 12 Bilder)
und `re2c_12b_hieb_dicht.jpg` (69,7-72,3 s, 16 Bilder).

Beobachtungen zur Kopplung Skript/Bild/Ton (gemessen, fuer den Port wichtig):
- Die RAM-Schnittvariable wechselt **vor** dem Bild (gemessene Vorlaeufe mindestens 0,15 s (s35) bis mindestens 0,91 s (s47);
  e12 63,43 s schon Schnitt 0x0B, Bild erst 64,23 s; e22 71,54 s schon Schnitt 6, Bild 72,23 s). Dazwischen steht das alte
  Bild still (Hintergrund wird geladen).
- Jeder Xa-Einsatz direkt nach `cut_chg; Sleep 1` liegt 0,020-0,037 s **vor** der sichtbaren Kante (alle 7 Faelle: 2B, 2C,
  2D, 2E, 2F, 31, 32). Der
  SCD-Zeiger steht in dieser Wartezeit auf dem Xa_on-Befehl selbst (s13 @0x00EBA, s47 @0x01010, e12 @0x0111A, e22 @0x01150):
  die Stimme startet erst, wenn das neue Bild da ist.
- Der Krach (Se_on direkt hinter cut_chg 7, ohne Sleep) kommt 0,69 s **vor** dem Bild von Schnitt 7.

## 3. Bild- und Spielrate

- Videobilder: 49,74682/s (DuckStation PAL, Log `FPS=49.74682`; 4025 Bilder in 80,890 s).
- Spielbild = 2 Videobilder: (a) Code: Bildwechsel-Warten `VSync(u8 [0x800d7662])` @0x8002ade8/@0x8002adec, der Wert ist in
  allen gelesenen Staenden **2** (s00, s12, s30, s60, e15, e22, e28); (b) Video: bei Bewegung (z. B. 59,1-60,3 s) ist jede
  zweite Bilddifferenz exakt 0; (c) RAM: Bewegungsbildzaehler Birkin +0x14D 5 -> 45 von e20 (69,87 s) nach e22 (71,54 s)
  = 40 Bilder in 1,67 s = 41,8 ms (+-0,05 s Standzeit-Streuung); zugleich steht der Skriptzeiger in e20/e21 im Sleep 45 @0x01146
  und bei e22 nach dessen Ende (Schnitt 6), d. h. 45 Sleep-Bilder ~ 45 Bewegungsbilder.
- Also: **1 Spielbild = 2 / 49,74682 s = 40,20 ms; Sleep n dauert n x 40,2 ms**, zuzueglich Ladestillstand an
  Schnittkanten (an den Kanten 0x0A->0x0B: Sleep 60 = 2,41 s, Kantenabstand 3,06 s; 0x0A->6: Sleep 45 = 1,81 s,
  Kantenabstand 2,45 s).
- Ein eigener 25-Hz-Zaehler fand sich im RAM nicht (Suche u16/u32 ueber s12..s57); gefunden nur VSync-Zaehler mit 49,8/s
  (0x800a3b50 = libetc Vcount, 0x800a4c68, 0x800aa780, 0x800bbc00).

## 4. RAM je Stand (Auswahl; alle 97 Staende in `ram_reihe_voll.md`)

Felder: Lage +0x38/+0x3C/+0x40, Richtung +0x76 (R, 0..4095), Routine +4..+7 (r, Plc_motion schreibt `(p1<<8)|4` nach +4),
Bewegung +0x14C / Bildzaehler +0x14D (M), Birkin +0x1D4 = Member 0x17. SCD = Zeiger der Aufgabe 2 (Datei-Offset in
ROOM6141.RDT) / gosub-Ruecksprung. Zeit = Lauf-1-Videozeit.

| Stand | Zeit | Schnitt | SCD (Zeiger / Ruecksprung) | Claire Lage, Richtung | Claire Routine, Bewegung | Annette (Slot 0) Lage, Richtung | Annette Routine, Bewegung | Birkin (Slot 1) Lage, Richtung | Birkin Routine, Bewegung, +0x1D4 |
|---|---|---|---|---|---|---|---|---|---|
| s10 | 11.20 s | 6 | @0x00E44(sub02) / - | (-20453, 0, -23932) R2068 | r01000000 M02/1 | (-22710, 0, -22970) R150 | r04060000 M00/1 | (-6600, -9000, -14800) R2048 | r04000000 M00/1 =0 |
| s11 | 12.17 s | 7 | @0x00E87(sub02) / - | (-21060, 0, -23320) R2048 | r01000100 M08/11 | (-22710, 0, -22970) R150 | r04060100 M02/11 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| s13 | 14.20 s | 8 | @0x00EBA(sub02) / - | (-21060, 0, -23320) R2048 | r04140000 M01/10 | (-22710, 0, -22970) R150 | r04060100 M02/41 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| s18 | 19.17 s | 9 | @0x00EF0(sub02) / - | (-21060, 0, -23320) R2048 | r04140000 M01/10 | (-22710, 0, -22970) R150 | r04140000 M13/17 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| s21 | 22.08 s | 8 | @0x00F16(sub02) / - | (-21060, 0, -23320) R2048 | r04000100 M0C/1 | (-22710, 0, -22970) R150 | r04140000 M13/17 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| s35 | 36.03 s | 9 | @0x00FB3(sub02) / - | (-21060, 0, -23320) R2048 | r04000100 M0C/1 | (-22710, 0, -22970) R150 | r04140000 M13/5 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| s47 | 47.45 s | 8 | @0x01010(sub02) / - | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-22710, 0, -22970) R150 | r04140000 M13/5 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| s49 | 49.33 s | 8 | @0x01024(sub02) / - | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-22710, 0, -22970) R150 | r04140000 M13/5 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| s50 | 50.25 s | 7 | @0x0104D(sub02) / - | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-22710, 0, -22970) R150 | r04140000 M13/5 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| s52 | 52.14 s | 7 | @0x01073(sub02) / - | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-22710, 0, -22970) R150 | r04140000 M13/5 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| s55 | 54.92 s | 7 | @0x01571(sub10) / @0x010B0(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-22207, 0, -21910) R3287 | r04050200 M01/7 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| s56 | 55.88 s | 7 | @0x01571(sub10) / @0x010B0(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-20951, 0, -18221) R3287 | r04050200 M01/10 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| e04 | 57.29 s | 2 | @0x01571(sub10) / @0x010C6(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-19904, 0, -15173) R3355 | r04050200 M01/9 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| e05 | 58.04 s | 2 | @0x01571(sub10) / @0x010C6(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-17177, 0, -14595) R119 | r04050200 M01/7 | (-6600, -9000, -14800) R2048 | r04000100 M10/19 =0 |
| e06 | 58.81 s | 2 | @0x01571(sub10) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-13987, 0, -15012) R4093 | r04050200 M01/6 | (-6600, -5250, -14800) R2048 | r04010100 M10/19 =1 |
| e07 | 59.61 s | 2 | @0x010FF(sub02) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-11097, 0, -15001) R4087 | r04000100 M0C/2 | (-6600, 0, -14800) R2048 | r04010200 M0E/13 =1 |
| e08 | 60.36 s | 2 | @0x010FF(sub02) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-11097, 0, -15001) R4087 | r04000200 M0C/0 | (-6600, 0, -14800) R2048 | r04010300 M00/8 =1 |
| e09 | 61.15 s | 10 | @0x01111(sub02) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-11097, 0, -15001) R4087 | r04000200 M0C/0 | (-6600, 0, -14800) R2048 | r04010300 M00/20 =1 |
| e12 | 63.43 s | 11 | @0x0111A(sub02) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-11097, 0, -15001) R4087 | r04000200 M0C/0 | (-6600, 0, -14800) R2048 | r04010300 M00/7 =1 |
| e19 | 69.06 s | 11 | @0x01138(sub02) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-11097, 0, -15001) R4087 | r04000200 M0C/0 | (-6600, 0, -14800) R2048 | r04010300 M00/24 =1 |
| e20 | 69.87 s | 10 | @0x01147(sub02) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-11097, 0, -15001) R4087 | r04020100 M02/6 | (-6508, 0, -14768) R2048 | r04020100 M07/5 =2 |
| e21 | 70.65 s | 10 | @0x01147(sub02) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-11097, 0, -15001) R4087 | r04020100 M02/26 | (-6802, 0, -14766) R2048 | r04020100 M07/25 =2 |
| e22 | 71.54 s | 6 | @0x01150(sub02) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-11097, 0, -15001) R4087 | r04020100 M02/46 | (-7997, 0, -14786) R2048 | r04020101 M07/45 =2 |
| e23 | 72.36 s | 6 | @0x01185(sub02) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-12300, 900, -15300) R3300 | r04000100 M18/6 | (-7753, 0, -14789) R2048 | r04020101 M07/51 =2 |
| e24 | 73.17 s | 6 | @0x01185(sub02) / @0x010E8(sub02) | (-21060, 0, -23320) R2048 | r04000200 M0A/0 | (-12300, 900, -15300) R3300 | r04000100 M18/26 | (-7544, 0, -14800) R2048 | r04030100 M00/11 =2 |
| e25 | 73.92 s | 6 | @0x01190(sub02) / @0x01190(sub02) | (-21060, 0, -23320) R2048 | r01000100 M08/13 | (-12300, 900, -15300) R3300 | r04000100 M18/6 | (-7544, 0, -14800) R2048 | r04030100 M00/31 =2 |
| e30 | 77.78 s | 6 | @0x01190(sub02) / @0x01190(sub02) | (-21060, 0, -23320) R2048 | r01000100 M08/19 | (-12300, 900, -15300) R3300 | r04000100 M18/22 | (-7544, 0, -14800) R2048 | r04030100 M00/22 =2 |

Abgleich mit dem Skript: Annette-Start (-22710,0,-22970) R150 = Sce_em_set @0x00C6C; Birkin (-6600,-9000,-14800) R2048 =
Sce_em_set @0x00C82; Claire (-21060,0,-23320) R2048 = Pos_set/Dir_set @0x00E76/@0x00E7E; Annettes Weg ueber die
Plc_dest-Ziele (-20000,-15400) @0x010A2, (-15000,-15000) @0x010B8, (-11000,-15000) @0x010DE (sie bleibt bei -11097,
-15001 stehen); Annette liegend = Pos_set @0x01162 `32 00 f4 cf 84 03 3c c4`, Dir_set @0x0116A `.. e4 0c ..`,
Plc_motion @0x01172 `3f 00 18 04` (RAM M18, +0x1CC = 4); Annette vor dem Hieb Plc_motion @0x01138 `3f 02 02 44`
(RAM r0402.., M02, +0x1CC = 0x44); Birkin +0x1D4 = 1 ab e06, = 2 ab e20 (Member_set @0x010CA / @0x01142).

Birkin nach der Szene: bleibt mit Routine 3 bei (-7544, 0, -14800) stehen (e24..e30, nach Steuerungsrueckgabe noch da;
im Bild von Schnitt 6 nicht sichtbar).

## 5. Belegdateien

Hier (`analysis/befunde_runde41/AN_belege/re2_claire/`):
- `zeitachse.md` (diese Datei), `ram_reihe_voll.md` (alle 97 Staende)
- `re2c_szene_320.mp4` (79,86 s, 320x240, mit Ton; Clipzeit = Videozeit - 1,00 s)
- `re2c_01_tuer_und_eintritt.jpg`, `re2c_02_schnitt07_xa2A.jpg`, `re2c_03_schnitt08_xa2B.jpg`, `re2c_04_schnitt09_xa2C.jpg`,
  `re2c_05_schnitt08_xa2D.jpg`, `re2c_06_schnitt09_xa2E.jpg`, `re2c_07_schnitt08_xa2F.jpg`, `re2c_08_schnitt07_krach_william.jpg`,
  `re2c_09_schnitt02_birkin_faellt.jpg`, `re2c_09b_birkin_faellt_dicht.jpg`, `re2c_10_schnitt0A_birkin.jpg`,
  `re2c_11_schnitt0B_annette_xa31.jpg`, `re2c_12_schnitt0A_hieb.jpg`, `re2c_12b_hieb_dicht.jpg`,
  `re2c_13_schnitt06_schrei_steuerung.jpg`
- `werkzeug/`: `re2c_drv.py` (DuckStation + Pad), `cmd.py`, `re2c_ss.py` (Savestate lesen/patchen, Vorschaubild),
  `mk_6141.py` (Tuer/Flag/Zombies), `exe_map.py` (Leon-DS/Claire-PAL Code-Vergleich, Disassembler), `ram_reihe.py`,
  `tabelle.py`, `snap_zeit.py`, `schnitte.py`, `xa_einsatz.py`, `leiste.py`, `sheet.py`

Rohdaten im Scratchpad `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/an/capture/`:
`lauf1/re2c_lauf1_voll.mp4` (volle Aufnahme 640x480, 80,89 s), `lauf2/`, `lauf3/` (Videos, Savestates, Vorschaubilder, Logs),
`p6141.sav` (gepatchter Einstieg), `boot/b42.sav` (ungepatchter Startraum) und `boot/b01..b42` (Weg durch Titel/Vorspann),
`test/` (Erkundungslauf, R1-Taste noch an RB gebunden), `duck_savestates/` (aus DuckStation verschobene Slot-Dateien),
`duckstation_letzter_lauf.log`. Dekodierte Xa: `.../scratchpad/an/xa16/xa_2A.wav .. xa_32.wav`.

## 6. Offene Punkte

- MAIN sub03 (Sterbegespraech) nicht aufgenommen: es braucht, dass Claire spaeter zu Annette geht (MAIN sub01 prueft u. a.
  (3,0x42)=1, (3,0x44)=0, (5,0x0C)=1 und ein `Cmp`); nicht ohne weiteres Laufen/Schalten erreichbar.
- Die Bedeutung der Flags (1,1) und (0x22,0x2B) im Spielablauf ist nicht hergeleitet; fuer die Szene genuegt, dass MAIN sub00
  sie prueft. Der Einstieg ist kein natuerlicher Spielstand (Startraum-Zustand, Inventar des Spielanfangs).
- Ladestillstand an den Schnittkanten (gemessen mindestens 0,15 s bis mindestens 0,91 s Vorlauf der Schnittvariable) haengt am CD-Zugriff; im Port gibt es sie nicht. Zu entscheiden
  ist, ob der Port die sichtbaren Kantenzeiten (Video) oder die reinen Sleep-Zeiten nachbildet.
- Die Herkunft der Toene 58,70 s und 60,85-61,25 s ist nur ueber die Lage im Skript (Se_on @0x010CE / @0x01104) und die
  RAM-Staende zugeordnet, nicht ueber die Tonbank-Nummern.
- Was genau Birkin in Bewegung 0x07 im Bild trifft (Annette ist im Schnitt 0x0A nicht im Bild), zeigt nur die Bildfolge
  `re2c_12b_hieb_dicht.jpg`; der Treffer liegt bei Bewegungsbild ~30-36 (70,9-71,1 s ab Beginn ~69,67 s).
