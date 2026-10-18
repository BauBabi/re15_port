# Runde 39 — Spur J "dining1141" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r39_dining1141` (Zweig `r39/dining1141`, Basis 1038dc15). Baut auf Runde 38 Spur F
(`analysis/befunde_runde38/F_dining1141.md`, `F_abnahme_0.md`, `F_abnahme_1.md`; Modul `engine/src/dining_r38.c`,
`gen/dining_r38_programme.inc`, Werkzeug `tools/r38_dining/programme_bauen.py`).

Punkte (Wortlaut AUFTRAG.md, Runde 39, "Mit Room 1141 bin ich noch überhaupt nicht zufrieden"):
1. Zombie-Geraeusche im Hintergrund weg ("Es gibt ja keine Zombies").
2. John kommt durch die TUER statt durch das Tor; waehrend des Tueroeffnens Kamera vorne auf Elza, Tuer-zu-Ton, dann
   Kamera auf die Tuer, wo John bereits da ist.
3. Johns "shout"-Voiceover (Port-Ton Runde 38 F) weg.
4. Statt dessen help01-05 im Mix mit Abstaenden, dazwischen mehrfaches Klopfen gegen eine Tuer.
5. Elza reagiert beim ersten Hoeren nach dem Betreten: "Oh, no, this is John! He is in Trouble, I have to help him!"
   (Nachtrag 2026-10-18: JOHN, nicht Roy).
6. Glasbruch-Ton vor Elzas "What was that?".
7. John schaut ab "It's some Kind of Gorilla...." nicht mehr zu Elza — fixen.
8. Nach dem Tod der Gorillas: "John: Puuuh, that was close! I thought we end up as Gorilla meal…." und "John: Elza,
   follow me, I think I might have found a way out of here!" mit passenden Animationen, statt einfach rauszurennen.
9. Beim Verlassen 1141 -> 1171 kurz Panik-Musik.

Ressourcen (VERTRAG 1.1/1.2): Bank-9-Bits 94, 95; Nachrichten-Ids 1141 50-59.

## 0. Stand / Fortschritt
- 2026-10-18 14:20 Baum sauber auf 1038dc15, Dossier angelegt.
- 14:27 Basis-Bau `local_build.sh configure` + `build` gruen (LOCAL-BUILD-OK).
- 14:55 Messung vorher (Lauf v0 = echter Tuerweg 1131 -> 1141 -> 1171, Wegplan wie integration_r38_dining1141_d, plus
  400 Bilder in 1171) ausgewertet; Ton-Quellen vermessen (Abschnitt 1, 2).
- **Fortsetzung 1 (ab 17:10, nach Sitzungslimit):** Baum = 56ffa0f3 (Merge master Welle 1+2). Plan:
  neues Modul `engine/src/dining_r39.c` + `include/re15_dining_r39.h` + `platform/pc/src/dining_r39_pc.c`, Programme
  `engine/src/gen/dining_r39_programme.inc` aus `tools/r39_dining/programme_bauen.py` (baut auf den Helfern des
  Runde-38-Werkzeugs auf); Haken in dining_r38.c (Phase H: Evt_exec/Ereignis 6 -> Programme der Runde 39), ein
  Stamm-Haken in audio_pc.c fuer help01..05 (Sprach-Pfad wie main42). Ablauf neu (Nutzer-Punkte 1-8):
  John ist beim Betreten NICHT im Raum (auf dem Dach, Punkt 2); Schleife = help01..05 gemischt + mehrfaches Klopfen
  (Punkt 1/3/4); nach dem ersten Ruf Elzas Satz Id 50 (Punkt 5, Bank 9 Bit 94 = einmal); Gemaelde -> Rolladen hoch ->
  Kamera auf Elza, Tuer auf/zu, Kamera auf die Dachtuer mit John (Punkt 2); Glasbruch vor msg 6 (Punkt 6); Blick
  Johns ab msg 9 (Punkt 7, RE Plc_dest Mode 9 / Turn-Sub); nach dem Tod der Gorillas Johns Zeilen Id 51/52 mit
  Gesten, dann Abgang (Punkt 8, Bank 9 Bit 95 = gesagt); Musik beim Wechsel 1141 -> 1171 (Punkt 9).

## 1. Messung vorher

Messordner (nicht committet): scratchpad `j39/l_v0` (Lauf v0, exe-Kopie `re15_pc_j39m_v0.exe`, Elza, `RE15_DEBUG_JUMP=1131@gp`,
Flags nach der 1021-Szene, `RE15_GIVE=0x43:1`, `RE15_R38F_GORILLAS_TOT=600`/`_STARR=1`, Wegplan = integration_r38_dining1141_d
+ `WARTE 400` in 1171; `RE15_SE_DEBUG`, `RE15_BGM_CTL_DEBUG`, `RE15_STATE_LOG`, `RE15_NECK_LOG`, `RE15_AUDIO_CAP_SYNC`).
Werkzeuge: scratchpad `j39/lauf.sh`, `vab_analyse.py`, `re2_scan.py`, `zoom.py`, `bogen.py`.

### 1.1 Punkt 1 (Zombie-Geraeusche) und Punkt 4 (kein Klopfen) — was in 1141 klingt
debug.log v0 ab `loaded room1141.rdt`: die Klopf-Schleife (Programm HAEMMERN, Runde 38 F) spielt je Runde
`Se_on bank=0x3C id=10` (F1, F245, F485, F725), `bank=0x3C id=11` zweimal (F35/F65, F275/F305, ...) und `bank=0x3D id=0`
= main42.wav (F155, F395, F635). Port-Bank 0x3C = KLOPF1060 = RE2 ROOM1060 snd0 ("ROOM106.SAP - Cabin sounds - Zombies
breaking in", RE2_SOUND_LISTE.md). Vermessen (dekodiert mit der Abspielrate des Tones, `[se] Stimme`-Zeilen bestaetigen sie):
| Satz | Tone/VAG | Rate | Dauer | Befund |
|---|---|---|---|---|
| 0x0A `00 00 85 20` | Tone 8+9 (1 Zusatzlage) / VAG 5 | 11025 Hz | 1,39 s | breitbandiges Rauschen, kein Grundton (Autokorrelation 0,11-0,25, Schwerpunkt ~2200 Hz) = Krachen/Splittern, KEIN Klopfen |
| 0x0B `00 00 ad 00` | Tone 10 / VAG 6 | 8193 Hz | 2,73 s | durchgehender Grundton 85-110 Hz in JEDEM 40-ms-Rahmen 0,0-2,6 s, Formantbaender 0,5/1/2/3 kHz im Spektrogramm = STIMME (Zombie-Stoehnen) |
Satz 0x0B laeuft zweimal je Schleifenrunde: das sind die "komischen Zombie Geraeusche im Hintergrund". Ein Klopfen gegen
eine Tuer gibt es im Ist-Stand nicht (0x0A ist ein Krachen) — der Nutzer hat recht ("den du aktuell noch garnicht hier drin hast").
Bild: scratchpad `j39/zoom1.png` (Zeilen 1060/0A, 1060/0B).

### 1.2 Punkt 3 (Johns "shout") 
`Se_on bank=0x3D id=0 -> Runde 38 Spur F (Stimme main42.wav)` je Runde, `[voice] clip loaded: main42.wav` (0,95 s, Schnitt
"Help me out." aus room1171/main01.wav, Runde 38 F 2.5) = der "komische shout voiceover".

### 1.3 Punkt 7 (John schaut ab msg 9 nicht zu Elza) — gemessen am Zustands-Log v0
Johns Blick (rot) gegen die Richtung John -> Elza (Port-Yaw: pos_advance @0x800245d8 x += cos, z -= sin):
| Nachricht | Elza | John | rot John | Soll | Fehler |
|---|---|---|---|---|---|
| 40, 3, 4, 5 | (-8069,-10258) | (-5840,-10127) | 2011 | 2010 | 0 Grad |
| 6, 7, 8 (Fensterblick) | dito | dito | 13 | 2010 | -175 Grad (gewollt: Blick zu den Fenstern) |
| **9, 10, 11** | dito | dito | **3341** | 2010 | **+117 Grad** |
| 11 (Ende) | dito | dito | 3917 | 2010 | +168 Grad |
Ab msg 9 steht John 117 Grad neben Elza — Befund bestaetigt.

### 1.4 Punkt 9 (Panik-Musik beim Wechsel 1141 -> 1171)
debug.log v0: `[bgm] stage=0 room=14 entry=FF56 -> MAIN16(flag 1)`; Sce_bgm_control slot 0 op=1 capTick 3674 (Klopf-Schleife),
op=2 capTick 4939 (John frei), op=1 capTick 5792 (Gorilla-Einbruch @0x00E5C), op=2 capTick 6920 (Gorillas tot, sub01 @0x00C06);
1171: `[bgm] stage=0 room=17 entry=5572 -> MAIN32(flag 1) SUB(flags 1/0)`, `Sce_bgm_control slot=0 op=1 capTick=8139` (main00
@0x011D6). Mitschnitt-RMS (30-Takt-Fenster): 6880 1553, 6910 881, 6940 3,6, danach 0 (MAIN16 aus) bis zur Tuer, ab 8110 MAIN32
(RMS 300-1300). In DIESEM Weg spielt beim Eintritt in 1171 also MAIN32 (der Titel des Raums), nicht MAIN16 — Abschnitt 4.9 klaert,
was der Nutzer als "kurz Panik Musik" hoert.
### 1.5 Punkt 9 nachgemessen (Lauf v0, Mitschnitt je Bild): der Hall der Panik-Musik ist MAIN16, entmutet von 1171
Mitschnitt-RMS je Bild (Takt = 1470 Abtastwerte): 8128..8136 = 0, **8138 = 735, 8140 = 2263**, 8142 2119, 8144 1352 ...
abklingend bis 8196..8202 = 61..81, ab **8204** steigt MAIN32 (279, 302, 429 ...). `[bgm] Sce_bgm_control slot=0 op=1 capTick=8139`
= ROOM1171 main00 @0x011D6 `54 00 01 00 00 00`. Der Ton setzt also im Bild des Skript-Befehls ein, nicht beim Raumladen,
und klingt ueber die 60-Bilder-Ausblendung ab (Lautstaerke 2263 = Pegel von MAIN16, gerendert 1300-1700 je 0,5 s;
MAIN32 gerendert 233-1104). Gegenprobe Kreuzkorrelation (scratchpad `j39b/xkorr.py`, Renderings `RE15_BGM_DUMP_MAIN`):
Fenster 8260..8290 = MAIN32 (rho 0,659), Fenster 8137..8160 keine MAIN32-Lage (0,128).
Mechanismus im Port (audio_pc.c): Sce_bgm_control op=2 (sub01 @0x00C06, Gorillas tot) setzt nur die Lautstaerke von
Slot 0 auf 0 (`case 2: s->mvol = 0`) - die Sequenz MAIN16 laeuft stumm weiter. Beim Wechsel 1141 -> 1171 (MAIN 0x16 ->
0x32) stoesst re15_bgm_play_room die Ausblendung an und laedt MAIN32 erst nach 60 Bildern (s_bgm_pending); das
Raum-Skript von 1171 laeuft aber sofort, sein op=1 setzt `s->mvol = base` auf den NOCH geladenen Track = MAIN16 ->
2 s Panik-Musik im neuen Raum. Wortlaut des Nutzers "am anfang noch kurz die Panik Musik" = genau diese 2 s.

## 2. RE-Belege

### 2.1 Punkt 9 - Reihenfolge im Original: erst Musikwechsel (wartend), DANN Raum-Skript
`re15_disasm.py dis 0x80044210` (FUN_80044210 = Musik des neuen Raums):
    80044278 andi a0,s0,0x3f / 8004427c andi v1,v1,0x3f / 80044280 beq a0,v1,0x800442f4   (MAIN gleich -> nur SUB)
    80044288 lbu v0,0x5218(0x800b)          ; Ausblend-Zustand DAT_800b5218
    80044294 beq v0,zero,0x800442b8
    8004429c jal 0x80029ac8 / 800442a0 ori a0,zero,0x1   ; ein Bild warten
    800442a8 lbu v0,0x5218(0x800b) / 800442b0 bne v0,zero,0x8004429c   ; WARTEN bis die Ausblendung fertig ist
    800442b8 jal 0x80044564                  ; erst dann MAIN laden (+ 800442d8 jal 0x80044774 SUB)
Aufrufer (jal-Suche 0x0C011084 im PSX.EXE): einzig FUN_800396fc @0x800399b0 `jal 0x80044210`; im selben Raumlader erst
danach @0x80039a00 `jal 0x8003ef6c` = SCD-INIT-Lauf (main00/sub00, scd_vm.c "INIT FUN_8003ef6c ... @0x8003f018").
=> Im Original wirkt ein Sce_bgm_control des neuen Raums IMMER auf den NEUEN Track; der alte (hier: gestoppter MAIN16)
kann nicht wieder hoerbar werden. Der Port blendet nicht wartend aus (Raum laeuft weiter) und latcht den Skript-Status
schon richtig fuer den aufgeschobenen Load (re15_bgm_load_main: `s_bgm_slot_status[0] == 1` -> hoerbar) - nur die
sofortige Lautstaerke-Aenderung am ALTEN Track waehrend `s_bgm_pending` ist falsch. Fix: waehrend des aufgeschobenen
Wechsels nur latchen (Abschnitt 3).

### 2.2 Punkt 7 - warum John ab msg 9 nicht zu Elza schaut
ROOM1141 sub02: @0x00E86 John Plc_neck -> Elza, @0x00E94 `40 00 09 01 ..` Plc_dest Modus 9 (Drehen) Flag-Bit 1, @0x00E9C
Sleep 5, Elza @0x00EA4/@0x00EB2 Plc_dest Modus 9 Flag-Bit 0, @0x00EBA `18 03` Gosub sub03 = wartet NUR auf Bit 0 (Elza;
sub03 @0x00FA4 `22 05 00 00` + While Ck(5,0)==0), dann @0x00EBC msg 9 und @0x00EC4 John `3f 00 0f 00` Plc_motion -> setzt
+0x5 = pc[1] = 0 (@0x80041bc4, scd_vm.c op_plc_motion) = Geste statt Dreh-Sub 9: Johns Drehung wird abgebrochen, wo sie
gerade steht. Im Original ist Johns Drehung kurz: vom Fensterblick (-850,-13350) (= Platz + (5900,0), Blick 0) zu Elza
(-7200,-15500) = Richtung (-450,-2150) = 102 Grad. Runde 38 F hat die Plaetze auf Elza (-8000,-10250) / John (-5800,-10250)
gelegt: Elza liegt genau WESTLICH von John -> 180 Grad; die Drehung ist bei msg 9 noch nicht fertig (gemessen v0: 3341
statt 2010). Die Original-Form fuer "John dreht, Skript wartet auf ihn" steht im selben Sub: @0x00CFE Plc_dest John Modus 9
Bit 1 + @0x00D06 `18 04` Gosub sub04 (sub04 @0x00FB6 `22 05 01 00` + While Ck(5,1)==0). Bit 1 setzt der Dreh-Sub beim
Ausrichten (enemy_ai_common.c re15_npc_sub_turn: `lbu a1,451(v0)` @0x80051dcc / `jal 0x8004ef90` @0x80051dd8).

### 2.3 Punkt 6 - Glasbruch: RE2-Form und RE2-Welle
RE2 ROOM1090 MAIN sub15 (Kraehen brechen durchs Fenster, re2_scd_dump.py):
    @0x02BB0 `36 02 21 01 00 00 9c f0 00 00 50 c9` Se_on(2,0x21)   @0x02BBC `09 0a 05 00` Sleep 5
    @0x02BC0 `36 02 21 01 00 00 9c f0 00 00 50 c9` Se_on(2,0x21)
Die Welle liegt im Port schon bytegleich als shared_assets/RE2/GLAS1090.EDT/.VH/.VB (Runde 35 Spur M, audio_pc.c
re15_audio_re2_glas_se, EDT @0x84 Satz 0x21). Im Original-1141 klingt vor msg 6 nichts (sub02 @0x00DC2 Plc_motion 0x17,
@0x00DC6 Sleep 20, @0x00DCA Message_on 6 "What was that?!"); msg 9 nennt die Ursache ("...enter through the windows!").

### 2.4 Punkt 4 - Klopfen: Zensus der RE2-Toene, Wahl
- Ist (Abschnitt 1.1): RE2 ROOM1060 snd0 0x0A = Krachen (Rauschen 1,39 s), 0x0B = Stoehnen (Grundton 85-110 Hz) - kein Klopfen.
- Zensus aller RE2-Raumbaenke snd0+snd1 (scratchpad `j39b/klopfscan.py`: Schlaege mit Anstieg >= 4x in 30 ms, Abklingen
  auf -20 dB <= 150 ms, Tieftonanteil < 1 kHz >= 50 %): KEIN mehrfacher Tuer-Klopfton in RE2. Mehrschlag-Kandidaten sind
  Mechanik: 1010/0A = Kendo schliesst die Ladentuer ab (sub09 @0x01C20), 2000/11 dieselbe Welle, 1120/0C..10 Regal,
  1110/10 Eisenplatte (sub08 @0x02C06), 2070/16 Schloss-Rasseln (Satz 0x16 = "verschlossen", lock_se), 2-Schlag-Paare
  0x1A..0x1F = Schritte. RE1.5 11A0..20A1 snd1/06 = Gegnerbank (Spinnen-Raeume), kein Klopfen.
- Tuerarchive (Tonteil je DOOR##.DO2, tueren_02_re2.md Abschnitt 4): DOOR0F = "Treppe Holz", Satz 0 = VAG 2, 0,31 s,
  44100 Hz, EIN trockener Holzschlag: Abklingen -20 dB in 40 ms, 100 % unter 1 kHz (`j39b/tuer_ton.py`, Bild
  `j39b/tuer1.png`). RE2 spielt ihn je Treppe dreimal (V4 Bild 65/108/151). Archiv-Tabelle RE2 @0x8009A5D4
  `88 2a cc 8b 06 00 00 00 db 36 00 00` = Tonteil 0x2A88 B (gen/re2_tuer_tabelle.inc DOOR0F).
- WAHL (NUTZER-VORGABE "mehrfachen Klopf gegen Tuer", Quelle RE2): Klopfen = DOOR0F Satz 0 dreimal im Abstand 5 Bilder
  (Form RE2 ROOM1090 @0x02BB0/@0x02BBC/@0x02BC0: Se_on, Sleep 5, Se_on). Tonteil bytegleich als gen/*.inc.


### 2.5 Punkt 2/8 - Kameras und Lagen (RID ROOM1141, SCA, Projektion H = persp>>7 = 208)
RID-Saetze (RDT +0x24, 0x20 B je Cut): Cut 5 (-7266,-3806,-17278) -> Norden (Rolladen, Gemaelde, durch die Rolladen-Oeffnung
der Gang); Cut 6 (-8292,-838,-6606) -> Norden = Dachgang mit Dachtuer; Cut 7 (-7465,-3024,-9613) -> Sueden (vom Gangende
in den Raum: der Gang und die Dachtuer liegen HINTER der Kamera); Cut 8 (-2824,-1295,-18232) -> Suedwesten (Ecke).
"Kamera vorne auf Elza" = Cut 7: das einzige nach Sueden blickende Bild am Gang; Elza muss dafuer in seinem Kegel stehen
und nach Norden blicken. Bildlage (scratchpad `j39b/proj.py`, Fuss y=0 / Kopf y=-1700): Elza (-7800,-12800) -> (208,234) /
(215,147) = ganz im Bild; am Gemaelde (-8000,-10250) waere sie 50 Grad neben der Achse (nicht im Bild). Cut 5 zeigt die
Dachtuer (-7350,-1300) durch die Rolladen-Oeffnung (118,106) in 16000 Tiefe - deshalb NICHT Cut 5 waehrend die Tuer
aufgeht. John innen vor der Dachtuer (-7350,-1300) in Cut 6 -> Fuss (170,188), Kopf (170,121). Nach dem Kampf: John in
seiner Ecke (-7550,-21500) (Ziel sub02 @0x00F8E) und Elza (-6000,-20000) beide im Bild von Cut 8 ((194,200) / (217,232)).
SCA (RDT +0x20, 5 x 19 Saetze zu 12 B, re15_rdt.h re15_sca_entry_t; `j39b/sca.py`): alle vier Lagen frei
(naechste Zelle 800 / 1050 / 2171 / 600). Tuerton-Laengen DOOR1A (tueren_02_re2.md 4): Ton 0 = 1,43 s, Ton 1 = 0,89 s.

## 3. Umsetzung

### 3.1 Dateien (Commit 46916d79)
- NEU `re15_port/include/re15_dining_r39.h` (alle Konstanten mit Beleg), `re15_port/engine/src/dining_r39.c` (Programme,
  Texte 50..52, Installer: John in Phase H geparkt, Port-Bank 0x3E, Sprach-Stamm help01..05, Messschiene RE15_R39J_LOG),
  `re15_port/platform/pc/src/dining_r39_pc.c` (Bank 0x3E: Klopfen DOOR0F, Glas GLAS1090 0x21, Hilferufe ueber den
  Sprach-Pfad), `re15_port/tools/r39_dining/programme_bauen.py` -> `re15_port/engine/src/gen/dining_r39_programme.inc`
  (HAEMMERN39 620 B, BEFREIUNG39 1496 B, Texte, DOOR0F-Tonteil 10888 B bytegleich; Werkzeug nutzt die Helfer der Runde 38).
- HAKEN (Kommentar "Runde 39 Spur J"): dining_r38.c (Include; Evt_exec -> HAEMMERN39; Ereignis 6 -> BEFREIUNG39; Se_on
  -> Bank 0x3E; Installer-Ende -> re15_dining_r39_install; Messhaken), dining_r38_pc.c (Init 1 Zeile), audio_pc.c
  (help-Stamm 2 Zeilen in re15_voice_load_clip; Punkt 9: `if (!s_bgm_pending)` vor dem Lautstaerke-Switch in
  ss_seq_ctl_ex), tests/unit/test_r36_kartenbits.c k_port[] (Bit 94). KEIN Asset-Patch, keine neue Datei unter
  shared_assets/ (Klopf-Tonteil als gen/*.inc).
- Ressourcen: Bank 9 Bit 94 (Reaktion); Bit 95 nicht gebraucht. Nachrichten 50/51/52 (Text), 53..57 = Sprach-Ids der
  help-Dateien (ohne Text). Port-Bank 0x3E (frei in master: 0x0E/0x3A/0x3B/0x3C/0x3D vergeben; Spur B nutzt TOENE39.VBS).

### 3.2 Ablauf neu (je Nutzer-Punkt)
| Punkt | Mechanik | Beleg / Form |
|---|---|---|
| 1 | HAEMMERN39 spielt kein Se_on(0x3C,0x0B) (Stoehnen) und kein (0x3C,0x0A) mehr | Abschnitt 1.1 |
| 2 | Phase H: John geparkt (-30000,-30000) statt im Gang; BEFREIUNG39 nach dem Rolladen: Elza WALK (-7800,-12800) + Gosub sub03, TURN zur Tuer + sub03, Cut 7, DOOR1A Satz 0, John Pos_set (-7350,-1300) + Dir_set 1024, Sleep 43, DOOR1A Satz 1, Sleep 27, MAIN Stop, Cut 6, Sleep 20, Cut 5, John RUN an seinen Platz | Formen sub02 @0x00CD8/@0x00CE0/@0x00CE2/@0x00CEA, @0x00C24, ROOM10D0 sub21 @0x01A02/@0x01D9E, ROOM10D0 @0x01AC4, sub01 @0x00C06, sub02 @0x00DC6; Laengen Abschnitt 2.5 |
| 3 | kein Se_on(0x3D,..) = main42 "Help me out." mehr | Abschnitt 1.2 |
| 4 | je Runde: 3 x Se_on(0x3E,0) (Sleep 5), Sleep 30, Se_on(0x3E,0x30+k) mit k = 3,1,4,2,5 (PORT-WAHL Mix), Sleep 90, Sleep 90; jede Ton-Zeile in Ifel Ck(3,0x6A)==0 | RE2 ROOM1090 @0x02BB0..@0x02BC0, RE2 ROOM1060 sub05 +0x2E/+0x58/+0x5E, Abschnitt 2.4 |
| 5 | nach dem ersten Ruf: Ifel Ck(9,94)==0 { Set(2,7)=1, Plc_neck Elza -> Dachtuer, Sleep 10, msg 50, Sleep 90, Set(9,94)=1, Plc_neck aus, Set(2,7)=0 } | sub02 @0x00C26/@0x00C42/@0x00C4C/@0x00D0A/@0x00F30/@0x00F4E; RE2 1060 +0x58 |
| 6 | vor sub02 @0x00DC6 Sleep 20 / @0x00DCA msg 6: Se_on(0x3E,0x21), Sleep 5, Se_on(0x3E,0x21) | Abschnitt 2.3 |
| 7 | nach @0x00E94 (John TURN zu Elza, Bit 1) Gosub sub04 | Abschnitt 2.2 |
| 8 | nach dem Tod der Gorillas + Sleep 30: Cut 8, Sperre, Elza Pos_set (-6000,-20000) + Blick 1547 zu John, Kopf/Drehung zueinander, msg 51 + Johns Gesten (0,15)/(0,15 rueckw.)/(0,23), Plc_motion(2,6), msg 52, (2,6) rueckwaerts; dann Abgang Runde 38 | sub02 @0x00C24..@0x00C76, @0x00CB0..@0x00CD0, @0x00C78..@0x00C9C; ROOM1171 @0x014C2 |
| 9 | audio_pc.c ss_seq_ctl_ex: waehrend `s_bgm_pending` wird Sce_bgm_control nur gelatcht | Abschnitt 2.1 |

## 4. Messung nachher

## 5. Tests

## 6. OFFEN

## 7. Fuer den Nutzer
