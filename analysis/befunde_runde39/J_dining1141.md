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
  Gesten, dann Abgang (Punkt 8; Bit 95 wurde nicht gebraucht); Musik beim Wechsel 1141 -> 1171 (Punkt 9).
- Fortsetzung 1 Ende (~20:15): Modul dining_r39 gebaut und gemessen (Laeufe n1, p5, Riegel-Laeufe), Befund Gespraechslage
  behoben (Elza neben John), Glasbruch an Johns Satzende (Warte-Zeile msg 58), Runde-38-Riegel an den neuen Ablauf angepasst.
  Gezielt gruen: Unit 50/50, Integration 14/14 + integration_r39_dining1141_d.
- **Nachbesserung 1 (2026-10-18 20:05..21:42, nach `J_abnahme_0.md`):** M1/M2/M3 + Nutzer-Nachtrag 4 (N4.1-N4.4) +
  Nachtrag 6 (N6) umgesetzt, Abschnitt 8 (Plan/Belege 8.0, Umsetzung 8.1, Messung 8.2, Tests 8.3/8.7, je Mangel 8.4,
  OFFEN 8.5, Fuer den Nutzer 8.6). Gezielt gruen: Unit 122/122, Integration 15/15 (D/S/elzaems am Endstand).

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
### 1.6 Punkte 2, 5, 6, 8 im selben Lauf v0 (debug.log, Abschnitt 1141)
- Punkt 2: `[dining] F283 ROOM1141 Phase H: John im Dachgang hinter dem Rolladen (-7124,0,-6150) Blick 1024` - John steht ab
  dem ersten Bild zwischen Rolladen und Dachtuer und kommt beim Hochfahren durch das Tor heraus.
- Punkt 5: Nachrichten in 1141 in Folge 13, 40, 3, 4 ... 11 - keine Reaktion Elzas auf Klopfen/Hilferuf.
- Punkt 6: 0 Glas-Toene vor msg 6.
- Punkt 8: F3247 Messhaken (Gorillas tot), F3277 `Cut_chg(7)` + `Plc_dest(slot=1 mode=0x05 dest=(-7350,-7600))` - John
  rennt 30 Bilder nach dem Kampf ohne ein Wort aus dem Raum.

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
- HAKEN (Kommentar "Runde 39 Spur J"): scd_vm.c op_message_on (Warte-Zeile msg 58, 2 Zeilen); dining_r38.c (Include; Evt_exec -> HAEMMERN39; Ereignis 6 -> BEFREIUNG39; Se_on
  -> Bank 0x3E; Installer-Ende -> re15_dining_r39_install; Messhaken), dining_r38_pc.c (Init 1 Zeile), audio_pc.c
  (help-Stamm 2 Zeilen in re15_voice_load_clip; Punkt 9: `if (!s_bgm_pending)` vor dem Lautstaerke-Switch in
  ss_seq_ctl_ex), tests/unit/test_r36_kartenbits.c k_port[] (Bit 94). KEIN Asset-Patch, keine neue Datei unter
  shared_assets/ (Klopf-Tonteil als gen/*.inc).
- Ressourcen: Bank 9 Bit 94 (Reaktion); Bit 95 nicht gebraucht. Nachrichten 50/51/52 (Text), 53..57 = Sprach-Ids der
  help-Dateien (ohne Text), 58 = Warte-Zeile (ohne Text, Punkt 6). Port-Bank 0x3E (frei in master: 0x0E/0x3A/0x3B/0x3C/0x3D vergeben; Spur B nutzt TOENE39.VBS).

### 3.2 Ablauf neu (je Nutzer-Punkt)
| Punkt | Mechanik | Beleg / Form |
|---|---|---|
| 1 | HAEMMERN39 spielt kein Se_on(0x3C,0x0B) (Stoehnen) und kein (0x3C,0x0A) mehr | Abschnitt 1.1 |
| 2 | Phase H: John geparkt (-30000,-30000) statt im Gang; BEFREIUNG39 nach dem Rolladen: Elza WALK (-7800,-12800) + Gosub sub03, TURN zur Tuer + sub03, Cut 7, DOOR1A Satz 0, John Pos_set (-7350,-1300) + Dir_set 1024, Sleep 43, DOOR1A Satz 1, Sleep 27, MAIN Stop, Cut 6, Sleep 20, Cut 5, John RUN an seinen Platz | Formen sub02 @0x00CD8/@0x00CE0/@0x00CE2/@0x00CEA, @0x00C24, ROOM10D0 sub21 @0x01A02/@0x01D9E, ROOM10D0 @0x01AC4, sub01 @0x00C06, sub02 @0x00DC6; Laengen Abschnitt 2.5 |
| 3 | kein Se_on(0x3D,..) = main42 "Help me out." mehr | Abschnitt 1.2 |
| 4 | je Runde: 3 x Se_on(0x3E,0) (Sleep 5), Sleep 30, Se_on(0x3E,0x30+k) mit k = 3,1,4,2,5 (PORT-WAHL Mix), Sleep 90, Sleep 90; jede Ton-Zeile in Ifel Ck(3,0x6A)==0 | RE2 ROOM1090 @0x02BB0..@0x02BC0, RE2 ROOM1060 sub05 +0x2E/+0x58/+0x5E, Abschnitt 2.4 |
| 5 | nach dem ersten Ruf: Ifel Ck(9,94)==0 { Set(2,7)=1, Plc_neck Elza -> Dachtuer, Sleep 10, msg 50, Sleep 90, Set(9,94)=1, Plc_neck aus, Set(2,7)=0 } | sub02 @0x00C26/@0x00C42/@0x00C4C/@0x00D0A/@0x00F30/@0x00F4E; RE2 1060 +0x58 |
| 6 | vor sub02 @0x00DC6 Sleep 20 / @0x00DCA msg 6: Message_on 58 (Warte-Zeile ohne Text, s.u.), Se_on(0x3E,0x21), Sleep 5, Se_on(0x3E,0x21) | Abschnitt 2.3; Warte-Zeile: Lauf n1 zeigte den Glasbruch F1917 mitten in Johns Sprachzeile zu msg 5 (main05.wav 6,7 s, msg 5 offen F1818..F2017, msg 6 erst F2019 - der Port oeffnet die naechste Nachricht erst nach der Sprachzeile, scd_vm.c Stimmen-Riegel). msg 58 parkt im selben Riegel und wird danach vom Haken re15_dining_r39_message_on verbraucht (scd_vm.c 1 Zeile) - der Glasbruch klingt am Ende von Johns Satz, unmittelbar vor "What was that?!"; ohne Sprachdatei laeuft die Zeile sofort durch |
| 7 | nach @0x00E94 (John TURN zu Elza, Bit 1) Gosub sub04 | Abschnitt 2.2 |
| 8 | nach dem Tod der Gorillas + Sleep 30: Cut 8, Sperre, Elza Pos_set (-6000,-20000) + Blick 1547 zu John, Kopf/Drehung zueinander, msg 51 + Johns Gesten (0,15)/(0,15 rueckw.)/(0,23), Plc_motion(2,6), msg 52, (2,6) rueckwaerts; dann Abgang Runde 38 | sub02 @0x00C24..@0x00C76, @0x00CB0..@0x00CD0, @0x00C78..@0x00C9C; ROOM1171 @0x014C2 |
| 9 | audio_pc.c ss_seq_ctl_ex: waehrend `s_bgm_pending` wird Sce_bgm_control nur gelatcht | Abschnitt 2.1 |

## 4. Messung nachher

### 4.1 Lauf n1 (echte exe Stand 46916d79, echter Weg mit Elza 1131 -> 1141 -> 1171, Wegplan `j39b/weg_n1.txt`)
`lauf.sh n1` mit RE15_SET_FLAG (Nach-1021-Flags), RE15_GIVE=0x43:1, RE15_DEBUG_JUMP=1131@gp, RE15_R38F_GORILLAS_TOT=600
(Kill-Flags, keine Schusszahl), RE15_R39J_LOG, RE15_AUDIO_CAP_SYNC. Auswertung debug.log (Abschnitt 1141) + r39.txt + Mitschnitt:
| Punkt | Messung | Vorher (v0) |
|---|---|---|
| 1, 3 | 0 Zeilen `Se_on bank=0x3C` / `0x3D`, kein `clip loaded: main42.wav` | 0x3C 0A/0B/0B + 0x3D je Runde |
| 4 | Klopfen F35/F40/F45, F355/F360/F365 ... (je 5 Bilder), help03 F75, help01 F395, help04 F615; `[voice] clip loaded: help03.wav (32000 Hz ...)`; Pegel je Schlag RMS 2475 im Schlagbild, Spitze ~7900 (Musik dort RMS ~200, Spitze ~1000), Hilferuf RMS 3348 | kein Klopfen |
| 5 | Nachrichten 1141 in Folge: **50**, 13, 40, 3..11, 51, 52; msg 50 offen F176..F274 (nach help03 F75), (9,94) danach 1 | erste Nachricht 13 |
| 2 | r39.txt: F1264 angezeigt Cut 7 (Elza (-7806,-12768)), John springt im selben Bild von (-30000,-30000) nach (-7350,-1300) = hinter der Kamera; F1334 Cut 6 mit John an der Tuer (70 Bilder = 43 + 27); F1354 Cut 5, John laeuft heraus | John im Dachgang ab Bild 0 |
| 6 | `Se_on bank=0x3E id=0x21 ... GLAS1090` F1917 und F1922, msg 6 danach | kein Ton |
| 7 | Blickfehler Johns zu Elza (Port-Yaw, atan2(-dz,dx)): msg 9 F2386..F2554 und msg 10 F2556..F2654 je **+23** (2 Grad); msg 11 +23 bis F2790, ab F2791 laeuft er (Original sub02 @0x00F8E RUN in die Ecke) | msg 9..11 +117 Grad |
| 8 | F3433 Cut 8, Elza (-6000,-20000) Blick 1549; msg 51 F3470 Blickfehler +2, Johns Clip (0,15); msg 52 F3600 mit Sub 2 Clip 6; F3680 Cut 7, F3802 John geparkt (vorher (-7350,-914) hinter der Kamera), F3817 Cut 8 | John lief ohne Wort hinaus |
| 9 | Mitschnitt: Tuer-zu-Ton F9442..9468 (DOOR1A Ton 1 = 0,89 s), **Takte 9473..9498 Pegel 0**, MAIN32 ab 9502 (Werkzeug `test_r39_dining1141 pegel`) | v0 Takte 8169..8194 Pegel **345** (MAIN16), Werkzeug meldet FEHLER |
Runde-38-Regressionen im selben Lauf: `[msgtext] room=1141 id=41 "I cannot leave him alone with those monsters."` an der
Doppeltuer im Kampf, Rolladen-Ton 0x3B, Tuer-Toene 0x0E 0/1, Dachtuer -> 1171.
Hinweis Messtechnik: RE15_FRAMEDUMP nummeriert je Raum (g_engine.frame_count beginnt beim Eintritt neu) - der lange Lauf in
1171 hat die 1141-Bilder bis F1500 ueberschrieben; der Riegel-Lauf (4.2) endet deshalb 300 Bilder nach dem Eintritt in 1171.

### 4.2 Riegel-Lauf integration_r39_dining1141_d (Stand d74fc937, Lauf lauf_beaaee4e, 320 s)
Alle Pruefungen gruen (Auswertung mit dem korrigierten Skript per -DAUSWERTEN, siehe 5). Bilder je 10 Takte:
- Tuer-Auftritt (Bogen `j39b/int_tuer.png`): F1240/F1260 Cut 5, Elza geht in den Raum und dreht sich zum Gang (der Gang und
  die Dachtuer sind durch die Rolladen-Oeffnung zu sehen, John ist NICHT dort); F1270..F1330 Cut 7 = Elza von vorn mit
  Balken; F1340/F1350 Cut 6 = John steht vor der Dachtuer; F1360..F1420 Cut 5, John laeuft aus dem Gang zu Elza.
- Gespraech nach dem Kampf (Bogen `j39b/int_dialog.png`): Text "John: Puuuh, that was close! I thought / we end up as
  Gorilla meal...." und "John: Elza, follow me, I think I might / have found a way out of here!" im Bild; BEFUND: mit Elza
  bei (-6000,-20000) stand sie in Cut 8 VOR John (Bildlage 217 vs 194, Tiefe 3302 vs 5437) und verdeckte seine Gesten.
  Behoben: Elza (-5800,-21500), 1750 oestlich von John: Bildlage Elza x 145, John x 194, nebeneinander; ausserhalb von
  Johns Bahn (x > BAHN_X_MAX -6450), SCA frei (naechste Zelle 850). Nachmessung 4.3.
- Riegel-Skript-Fehler gefunden und behoben: CMake kennt nur CMAKE_MATCH_1..9; mit 11 Gruppen waren Blick/b67 leer und die
  Blick-Pruefung lief ins Leere (0 von 0). Jetzt Hauptteil 8 Gruppen + b67/blick einzeln; Auswertung desselben Laufs:
  msg 9/10 0 von 268 Bildern ueber 160 Einheiten, msg 51 Blick 0, (9,67) = 1, Cut 7 verlassen.

### 4.3 Riegel-Lauf integration_r39_dining1141_d am Endstand der Programme (Lauf lauf_3f843286, Passed 325 s)
- Punkt 2: "John erstmals im Raum: F1269 anz=7 (-7350,-1300); erstes Bild Cut 6 danach: F1339 (-7350,-1300); Cut 7 ab F1264"
  (Sleep 5 nach Cut_chg 7: John kommt 5 Bilder NACH dem Bildwechsel, hinter der Kamera).
- Punkt 7: msg 9/10 0 von 268 Bildern ueber 160 Einheiten Blickfehler.
- Punkt 8 (Bogen `j39b/int2_dialog.png`): Cut 8, Elza links / John rechts im Bild, beide einander zugewandt; zu msg 51 Johns
  Gesten (Hand zur Brust/hoch), zu msg 52 streckt John den Arm nach Elza aus (Plc_motion(2,6) = Heranwinken/Zeigen, KEIN
  Klettern - anders als der Nutzer-Befund in 1171, Spur K); Blick msg 51/52 179 Bilder = 0, danach dreht er zum Abgang;
  F3680 Cut 7, John laeuft auf die Kamera zu.
- Punkt 9: Werkzeug pegel "Takte 9426..9451 Pegel 0, Takte 9466..9496 Pegel 559".
- Punkt 5 (eigener Kurzlauf p5, Bogen `j39b/p5.png`): F176..F274 "Elza: Oh, no, this is John! / He is in Trouble, I have to
  help him!" im Bild von Cut 0 (Elza an der Doppeltuer), danach geht das Spiel weiter.
- Generator-Gegenprobe: `programme_bauen.py --pruefen` fuer dining_r39_programme.inc und dining_r38_programme.inc: ok.

### 4.4 Endstand mit Warte-Zeile (Commit 2d7ff566, Lauf lauf_32c2298f, integration_r39_dining1141_d Passed 340 s)
- Punkt 6: msg 5 (John, Sprachzeile main05.wav) offen bis F2022; `[msg] room=1141 id=58` parkt bis dahin; Glasbruch F2023 und
  F2028 (Se_on 0x3E/0x21, Takte 5546/5551); msg 6 "What was that?!" ab F2049 (= Sleep 5 + Sleep 20 nach dem ersten Glas).
  Vorher (n1): Glas F1917 mitten in Johns Satz, msg 6 erst F2019.
- Unveraendert: Punkt 2 John erstmals F1269 bei angezeigtem Cut 7, Cut 6 ab F1339 mit John an der Tuer; Punkt 7 0 von 268;
  Punkt 9 Pegel Takte 9365..9390 = 0, danach MAIN32 559.

## 5. Tests
Neu (tests/unit/probes/r39_dining1141.cmake, tests/unit/test_r39_dining1141.c, tests/integration/test_r39_dining1141.cmake):
| Test | misst | Punkte |
|---|---|---|
| unit_r39_dining1141_texte | msg 50/51/52 dekodiert = Wortlaut, Dialogform msg 2 | 5, 8 |
| unit_r39_dining1141_programme | Bytes: kein 0x3C/0x3D, 15 Schlaege mit Sleep 5, help01..05 je einmal im Mix, Ck(9,94) -> msg 50 -> Set(9,94) nach dem ersten Ruf; Cut 7 -> Tuer auf -> Pos_set/Dir_set John -> Tuer zu -> Cut 6; Glas x2 direkt vor msg 6; Gosub sub04 nach Johns TURN vor msg 9; Cut 8 -> 51 + Gesten -> (2,6) -> 52 -> RUN | 1-8 |
| unit_r39_dining1141_stamm | Id 53..57 -> help01..05 nur in 1141 | 4 |
| unit_r39_dining1141_phase_h | VM-Lauf: John geparkt und in keinem Cut-Viereck (nie gezeichnet), Reaktion einmal mit Pad-Sperre, Klopfen/Rufe, Tuer-Auftritt (John erst bei angezeigtem Cut 7, Tuer-zu VOR Cut 6, John im ersten Bild von Cut 6 sichtbar, vorher nie), Glas vor msg 6, Blick msg 9..11 (re15_ai_arc_test Kegel 160), Ecke, Sperre beide Tueren, Gespraech (Cut 8, Elza-Lage, Blick, Clips 15/23, Sub 2 Clip 6), Abgang (kein Schub, > 900 im Bild, Parken in Cut 7), Dachtuer -> 1171, Wiederbetreten | 1-8 + Runde 38 |
| integration_r39_dining1141_d | echte exe, echter Weg mit Elza, debug.log + RE15_R39J_LOG + Mitschnitt (Werkzeug pegel) | 1-9 |
Angepasst (vom Nutzer geaenderter Ablauf, Begruendung im Code): unit_r38_dining1141_phase_h und _abgang -> Verweis auf
unit_r39_dining1141_phase_h (die weiter gueltigen Pruefungen sind dorthin uebernommen); unit_r38_dining1141_weg1131 prueft den
Rueckfall tuer_1131 am Zustand statt am (nicht mehr moeglichen) Hinausgehen waehrend des Wartens; integration_r38_dining1141
D und A -> Verweis auf integration_r39_dining1141_d (Pruefungen uebernommen), S mit Wegplan WARTE 900 und Ende 5800#1141;
test_r36_kartenbits k_port[] + Bit 94.

Gezielter Lauf Unit (Endstand nach der Warte-Zeile, ctest -R woertlich; Gruppen je beruehrtem Modul: dining_r38/39,
Kartenbits, audio_pc.c BGM/Stimme/SE, scd_vm.c op_message_on-Haken):
`unit_r39_dining1141|unit_r38_dining1141|unit_r36_kartenbits|unit_rotor_bgm_pin|unit_r35_cut10f0|unit_synchro_stimme|unit_voice_ausreden|unit_r39_stimmen|unit_r38_rekorder|unit_r38_elzaintro_stimme|unit_se_bank_routing|unit_se_voice_machine|unit_r30_tuer_stimme|unit_audio|unit_r34n_f_leiche_stimme|unit_r38_tagebuch|unit_r38_elzatoene`
-> "100% tests passed, 0 tests failed out of 50".
Integration integration_r39_dining1141_d (Endstand): "Passed 340.14 sec" (Abschnitt 4.4).
Gezielter Lauf Integration (Endstand, ctest -R woertlich; Module dining_r38 (alle Teile), audio_pc.c BGM-Wechsel
(r35_cut10f0) und Sprach-Pfad (elzaintro, rekorder, r39_stimmen)):
`integration_r38_dining1141|integration_r35_cut10f0|integration_r39_stimmen|integration_r38_rekorder|integration_r38_elzaintro_stimme`
-> "100% tests passed, 0 tests failed out of 14" (darin integration_r38_dining1141_s Passed 663 s: zwei Laeufe mit
Rueckleser je Bild, John erscheint in keinem Bild von 1141 ohne Bewegung oder Kameraschnitt - der Tuer-Auftritt bleibt
im Rahmen der Regeln R0-R3; D/A Verweis-Stubs).

## 6. OFFEN
- Spur K (Welle 3, parallel): Johns Plc_motion(2,6) (eigene Bank, Clip 6) ist im Original die Geste zu "Elza, hurry up!"
  (ROOM1171 sub02 @0x014C2); der Nutzer sah sie im Port in 1171 als "komische Kletter Animation". Punkt 8 benutzt dieselbe
  Geste zu "Elza, follow me" (Form ROOM1141 sub02 @0x00C78/@0x00C94). Aendert Spur K die Kanal-/Bankzuordnung von Johns
  eigener Bank, wirkt das auch hier - bei der Integration gemeinsam ansehen (VERTRAG 1.4).
- Port-Bank 0x3E: in master frei; falls Spur I (10F1) in derselben Welle eine Port-Bank anlegt, bei der Integration abgleichen.
- synchro/STAGE1/room1141/main42.wav (Runde-38-Schnitt "Help me out.") wird nicht mehr gespielt; Datei und Runde-38-Code
  bleiben unangetastet (kein Nutzer-Asset, aber im Runde-38-Werkzeug referenziert).
- Bank 9 Bit 95 (VERTRAG J) nicht gebraucht - frei fuer die naechste Runde.

## 7. Fuer den Nutzer
- Neue Texte ohne Aufnahme (Sprachdateien nach synchro/README.md, der Port spielt sie automatisch, sobald sie liegen):
  `synchro/STAGE1/room1141/main50.wav` = "Elza: Oh, no, this is John! He is in Trouble, I have to help him!",
  `main51.wav` = "John: Puuuh, that was close! I thought we end up as Gorilla meal....",
  `main52.wav` = "John: Elza, follow me, I think I might have found a way out of here!".
- Die Hilferufe sind deine Dateien help01..05.wav (unveraendert, Zuordnung im Code). Reihenfolge je Durchlauf
  help03, help01, help04, help02, help05; zwischen zwei Rufen dreimal Klopfen.
- Klopfen: RE2 hat keinen eigenen Klopf-Ton an einer Tuer (alle Raumbaenke und Tuerarchive vermessen, Dossier 2.4). Genommen
  ist RE2s trockener Holzschlag aus dem Tuerarchiv DOOR0F (Holztreppe), dreimal kurz hintereinander. Gefaellt der Klang nicht,
  bitte sagen (oder eine eigene Klopf-Datei nennen).
- Glasbruch vor "What was that?!" = RE2s Fensterscheibe aus der Kraehen-Szene (derselbe Klang wie das Fenster in 1120).
- Ablauf jetzt: Eintritt -> Klopfen + Hilferuf -> Elzas Satz (einmal) -> Gemaelde -> Rolladen hoch -> Elza tritt zurueck,
  Kamera von vorn auf Elza, Tuer auf und zu -> Kamera auf die Dachtuer, John steht dort -> er laeuft zu Elza -> Szene wie
  bisher (Glasbruch vor msg 6) -> Kampf -> John: "Puuuh..." (Gesten wie zu "I'm glad to see you're still alive") ->
  "Elza, follow me..." (Heranwinken wie zu "Elza, hurry up!" in 1171) -> John geht durch den Gang aufs Dach.
- Paket-Gate: KEINE neue Datei unter shared_assets/ (Klopf-Ton als gen/dining_r39_programme.inc eingebettet).


## 8. Nachbesserung 1 (nach Abnahme 0, `J_abnahme_0.md`; Start 2026-10-18 ~20:10)

Maengel: M1 (Reaktion msg 50 im Fenster Gemaelde-Frage F75..F165 -> erscheint in der Rolladen-Szene), M2 (msg 52: John
richtet per Plc_motion(2,6) die Pistole auf Elza statt heranzuwinken), M3 (John springt 16 Bilder nach dem Schnitt auf
Cut 8 sichtbar 138 Einheiten), dazu Nutzer-Nachtrag 4 (N4.1 Rolltor oeffnen und danach frei laufen, N4.2 Szene erst nach
dem Entriegeln der Dachtuer: Elza rennt zur anderen Kamera, dreht sich zur Tuer, hebt die Waffe - Schusswaffe aus dem
Inventar, sonst das Messer; N4.3 John schiesst ab und an auf die Gorillas und zieht ihre Aufmerksamkeit; N4.4 = M2) und
Nachtrag 6 (N6: "He is in trouble", "gorilla meal" klein).

### 8.0 Plan und RE-Belege vor dem Code (diese Sitzung gelesen)
- **N4.1/N4.2 Mechanik "Tuer entriegeln" = Original sub05.** ROOM1141 main00 @0x00A72 `06 00 3c 00` Ifel Ck(4,0xC3)==0
  @0x00A76 `21 04 c3 00`: Dachtuer Slot 1 sce 0 (@0x00A7A `3b 01 00 31 ..`, nicht begehbar) + @0x00A9A `2c 02 03 31 ..
  ff 00 18 05 ..` Slot 2 = Ereignis 5 im Tuer-Rechteck; sub05 @0x00FC8 `46 02 00 00 ..` Aot_reset Slot 2, @0x00FD2
  `22 04 c3 01` Set(4,0xC3)=1, @0x00FD6 `2b 0c ff ff` msg 12 "You've opened the lock.", @0x00FDA Evt_next, @0x00FDC
  `46 01 02 31 ..` Slot 1 -> Tuer. = das "Entriegeln" des Nutzers. Neuer Ablauf: Gemaelde (Ereignis 6) = nur Rolladen
  mit Ton, danach Steuerung zurueck (Form sub06 @0x010C8..@0x010D6 Set(2,7)=0, Set(1,0x1B)=0, Cut_auto 1, Plc_ret);
  Klopfen/Rufe laufen weiter, solange die Dachtuer verriegelt ist (While Ck(4,0xC3)==0 statt (3,0x6A); Form der
  Bedingung main00 @0x00A76); Ereignis 5 in Phase H = Programm ENTRIEGELN39 (sub05-Zeilen, dann die Szene).
  Phase H = (4,0xF0)=1 und (4,0xC3)=0 (vorher (3,0x6A)=0): beim Wiederbetreten mit offenem Rolladen laeuft wieder die
  Klopf-Schleife (sub00 @0x00B02 Evt_exec sub02), der Rolladen bleibt offen (sub00 Else-Zweig @0x00BC2..).
- **N4.2 "hebt die Waffe" = Plc_motion(2,6) auf dem SPIELER.** Original-Form ROOM3060 sub03 @0x010BA `2e 01 00 00` +
  @0x010BE `3f 02 06 00` (Leon hebt vor den Hunden die Waffe), Sleep 40 @0x010C2; vorher @0x0108E `40 00 09 00 ..`
  Plc_dest TURN zum Ziel. Handler @0x80041b90 schreibt +0x5 = pc[1] = 2; Spieler-Executor 0x80030660 waehlt den Sub ueber
  `lbu v0,-13735(v0)` = 0x800aca59 @0x8003068c, Tabelle `addiu at,at,15920` = 0x80073e30 @0x8003069c: [2] = 0x80050f00
  (dieselbe Play-once/Hold-FSM wie bei den NPCs) auf dem Kanal +0x170/+0x174 = Spieler 0x800acbc4/0x800acbc8 = die
  W-Bank (PLW): der Waffen-Lader schreibt genau diese Zeiger (@0x80036be4 `sw v0,-13368(at)` = 0x800acbc8,
  @0x80036c04 `sw v0,-13372(at)` = 0x800acbc4). W-Bank Clip 6 = RAISE (player_common.c Gun-FSM sub0 / Melee @0x80034ee8),
  rueckwaerts (Plc_flg 0x80, Form ROOM1021 @0x020EA/@0x020EE) = Senken. Der Port spielt fuer pc[1] != 0 bisher die
  PL00-Bank (scd_vm.c op_plc_motion `anim_use_pl00`), nicht die W-Bank -> Port-Maschine noetig (nur ROOM1141, Haken).
  Waffe in der Hand: FUN_80036b68 @0x80036c08 haengt die Waffe der Id aca5d immer an (main.c "WEAPON-IN-HAND"), Messer =
  Id 1 (Rueckfall bei Ausruest-Platz 0x80, @0x8004666c/@0x80046688). NUTZER-VORGABE (Nachtrag 4): Schusswaffe aus dem
  Inventar (die ausgeruestete, sonst eine aus dem Inventar), sonst das Messer.
- **M2/N4.4 Heranwinken = RBJ-Geste Clip 17 (Sub 0).** (2,6) ist Johns Ziel-Clip: ROOM1021 sub02 @0x020C6 `3f 02 06 00`
  (zielen) / @0x020CE, @0x020D6 `3f 02 07 00` (Feuer-Clip, Schuss auf Roy) / @0x020EA+@0x020EE (2,6) rueckwaerts
  (senken); ROOM1141 @0x00C78 (2,6) vor msg 0 "Elza: John, wait!" / msg 1 "...sorry, ..." = John zielt auf die
  Eintretende. Die Dringlichkeits-/Folge-Geste des Originals ist Clip 17 der Gestenbibliothek (Armschwung bis ganz
  seitlich, D_adaruf.md 3.5): ROOM3070 sub03 @0x03724 `3f 00 11 00` "Leon: Hurry up! They're coming!", ROOM4000 sub09
  @0x0191A "Leon: Go on ahead! / I'll take care of this!"; John selbst spielt Clip 17 in 1141 (@0x00DBA zu msg 5) - der
  Clip liegt also in seinem 1141-RBJ-Satz.
- **N4.3 John schiesst = Original-Form ROOM1021 sub02** (oben) + Schuss-Ton RE15_ELZA_TON_SCHUSS (RE2 ROOM4040 Satz 0x0A,
  Runde 36 L / Runde 38 H, Verzug 1 = RE2 ROOM1010 sub30) ; Original-Zeile msg 11 "John: Elza, I'll try to distract
  them, ..." = genau das Ablenken. Aufmerksamkeit/Rhythmus = NUTZER-VORGABE, Form im Abschnitt Umsetzung.
- **M1:** Reaktion nur, solange die Dachtuer verriegelt ist und keine Szene laeuft (Gemaelde-Programm setzt beim
  Ausloesen synchron einen Merker, die Reaktion wartet darauf) - Riegel "Gemaelde im Fenster".
- **M3:** John im Schnittbild von Cut 8 setzen (Pos_set neben Elzas Pos_set vor Sleep 1) + Sprung-Riegel.
- **N6:** msg 50 "He is in trouble, ...", msg 51 "... gorilla meal....".

### 8.1 Umsetzung (Commits 37329e77, 3263b432, 0e3a7bcc)
| Mangel | Aenderung | Beleg / Form |
|---|---|---|
| N4.1 | Ereignis 6 in Phase H = ROLLTOR39 (224 B): sub06 @0x00FE8..@0x0100E, Rolladen-Ton (Runde 38), Bewegung @0x01032..@0x0107C, Sca @0x0107C..@0x010A4, Schluss @0x010C8..@0x010D8 (Pad/Balken aus, Cut_auto 1, Plc_ret = Steuerung zurueck), Merker aus, Evt_end. Phase H und HAEMMERN39 laufen bis Ck(4,0xC3) (statt (3,0x6A)); beim Wiederbetreten mit offenem Rolladen klopft John weiter | main00 @0x00A76, sub00 Else @0x00BC2 |
| N4.2 | Ereignis 5 (Riegel) in Phase H = ENTRIEGELN39: sub05 @0x00FC8..@0x00FDA (msg 12), Pad/Balken, Elza RUN (-7800,-12800), Cut 7, TURN zur Tuer, **Plc_motion(2,6) + Sleep 40** (Waffe heben), Tuer auf/John/Tuer zu (wie bisher), Cut 6, Cut 5, **(2,6) + Plc_flg 0x80** (senken); Waffe beim Ausloesen gewaehlt (re15_dining_r39_waffe_waehlen): Schusswaffe im Inventar (ausgeruestet oder erste), sonst Messer Id 1 | ROOM3060 sub03 @0x010BA/@0x010BE/@0x010C2; Spieler-Sub 0x80073e30[2] = 0x80050f00 (@0x8003068c/@0x8003069c), Kanal +0x170/+0x174 = W-Bank (@0x80036be4/@0x80036c04); ROOM1021 @0x020EA/@0x020EE; Messer-Rueckfall @0x8004666c |
| N4.2 Port | Spieler-Plc_motion(2,c) in 1141 = W-Bank-Clip c einmal + halten (dining_r39.c re15_dining_r39_w_*; Haken scd_vm.c op_plc_motion, game_step_common.c Takt, main.c Zeichner wie der Modus-6-Override). Der Port spielte fuer pc[1] != 0 die PL00-Bank | wie oben |
| N4.3 | Faden SCHUETZE39 (116 B, Evt_exec sub 30): Salve = TURN zum naechsten Gorilla (Anker ZIEL), (2,6) Zielen, Sleep 110, (2,7) zweimal im Abstand 30, (2,6) rueckwaerts, Sleep 20; Pause Sleep 90+90. Schuss: Ton RE15_ELZA_TON_SCHUSS, Treffer im Kegel +-0xC8 mit Handfeuerwaffen-Schaden 12, Gorilla kriecht 180 Bilder auf John zu | ROOM1021 sub02 @0x020C6..@0x020F2; Schaden @0x8006ee44; Treffer-Schreiber @0x80012428/@0x80012520/@0x800124bc; Kegel @0x80032fe0; Gorilla-Lenkziel @0x80117d4c |
| M1 | Reaktion: While Ck(5,0x10)==1 (Merker, synchron beim Ausloesen von Ereignis 6 gesetzt, ROLLTOR39 loescht ihn) + While Ck(2,7)==1, dann Ifel Ck(4,0xC3)==0 { Set(2,7)=1 ... } im selben Takt | Formen sub03 @0x00FA8, sub02 @0x00C26 |
| M2/N4.4 | msg 52 + John Plc_motion(0,17) (Armschwung), Sleep 50, (0,23), Sleep 20, Sleep 30 (Rest der Standzeit, Abnahme H3: 99 Bilder); kein (2,6) mehr im Gespraech. **Ersetzt durch Nutzer-Nachtrag 7 (8.8): Plc_motion(0,2) = ROOM1171 RBJ Record 1 Clip 2, Sleep 60 + 40** | 1141 sub02 @0x00DBA..@0x00DC6 (John), ROOM3070 sub03 @0x03724 "Hurry up! They're coming!" |
| M3 | John Pos_set (-7550,-21500) direkt nach Cut_chg 8, vor Sleep 1 | sub02 @0x00C24..@0x00C3E |
| N6 | msg 50 "...He is in trouble, ...", msg 51 "...gorilla meal...." | Nutzer-Nachtrag 6 |
Gemeinsame Dateien (je 1-3 Zeilen, Kommentar "Runde 39 Spur J"): scd_vm.c (op_plc_motion, VM-Anker), game_step_common.c,
platform/pc/main.c, enemy_ai_common.c (Gorilla-Lenkziel B[3]), re15_damage.c (re15_enemy_waffen_schaden). NICHT im
VERTRAG-1.3-Verzeichnis fuer J: enemy_ai_common.c (D/E), game_step_common.c, main.c, re15_damage.c - bei der Integration
mit Spur E (Nachbesserung 3, Gorilla in 1141) abgleichen.
Riegel: unit_r39_dining1141_fenster (neu), _phase_h/_programme erweitert, test_r38_dining1141 bis_zur_ecke (Riegel druecken).

### 8.2 Messung nachher (echte exe; Messordner scratchpad `j39n1/`, nicht committet)
- **Lauf n1** (Stand 37329e77, echter Weg 1131 -> 1141 -> 1171 mit Elza, nur Schluessel = ohne Schusswaffe; Wegplan
  `plan_n1.txt` = Riegel-Weg): 1141-Nachrichten in Folge 50 (einmal), 13 (Gemaelde), **12** ("You've opened the lock."),
  40, 3..11, 41, 51, 52; Wortlaut msg 50 "He is in trouble", msg 51 "gorilla meal" (N6). Gemaelde F788 -> ROLLTOR39,
  danach frei (Wegplan geht durch die Rolladen-Oeffnung zur Dachtuer), Klopfen weiter (18 Schlaege im Raum), Riegel
  F1599 -> ENTRIEGELN39. Elza dreht an der Tuer (F1644..F1664), rennt (Cut 6 -> Cut 5 Kamera-Automatik, F1668..F1716),
  Cut 7 ab F1724 (Cut 0 hoechstens ein Bild an der Zonengrenze F1720), Drehung zur Tuer F1724..F1744, Waffe heben F1743,
  John hinter der Kamera F1784, Cut 6 mit John F1856, Senken F1873, Cut 5 John laeuft heraus (Bogen `n1_lauf.png`).
  Befund: mit dem Messer zeigte Clip 6 die leere Hand (`n1_hand.png` F1756) -> Messer kalt = DRAW 0x0D (35fa62f6).
- **Lauf pist** (Kurzweg ab Phase H2: Debug-Sprung 1141, Rolladen offen, Reaktion gesehen, Elza im Gang, RE15_GIVE
  Pistole 0x03 + Munition; Riegel, Kampf mit eingefrorenen Gorillas, Messhaken 900): `Dachtuer entriegelt: Elza hebt
  Waffe 3 (vorher 1, Schusswaffe)` (aus dem Inventar ausgeruestet), Plc_motion(2,6) F340: Pistole beidhaendig hoch,
  gehalten bis Cut 6 (`pist_heben.png` F340..F440). John: TURN F2240, Zielen (2,6) F2250, Feuer (2,7) F2360 + F2388,
  `John schiesst (Schuss 1/2) auf Gorilla Slot 2: Treffer, Aufmerksamkeit`, zweite Salve F2729/F2759; Kamera folgt Elza
  (Cut 0), John ist dabei nicht im Bild (`pist_schuss.png`). Gespraech: Cut 8 ab F2873, John im selben Bild von
  (-7535,-21362) auf (-7550,-21500) (M3: kein sichtbarer Sprung mehr), msg 51 F2895 (Clip 15/23), msg 52 F3015..F3114 mit
  **Clip 17** F3015..F3113 und Clip 23, John laeuft F3115 los (msg 52 weg: H3 behoben), geparkt F3250.
- **Lauf mess** (wie pist, ohne Schusswaffe): `Elza hebt Waffe 1 (... Messer)`, DRAW 0x0D F340..F356 (Messer ueber den
  Koerper gezogen), dann Messer-Bereitschaft gehalten (`mess_heben.png`).
- **Lauf f** (M1-Fenster der Abnahme nachgestellt: `jabn/plan_f.txt`, Debug-Sprung 1141, Elza vor dem Gemaelde): erster Ruf,
  Frage msg 13 F133..F248, "ja" -> (3,0x6A) F250, Rolladen-Szene pm=2 F250..F470, **msg 50 erst F481** (vorher F291 mitten
  im Hochfahren), Wortlaut mit "trouble" (`f_neu.png`: F290/F380 Rolladen faehrt, F465 oben, F485 Text bei offenem Rolladen).
- **Lauf auf / auf2** (wie pist, Gorillas NICHT eingefroren): Treffer F2368/F2398 auf Gorilla Slot 2 (Energie 180 -> 168
  -> 156). auf (nur Lenkziel in B[3]): Abstand zu John 6545 -> 6848 in 180 Bildern = keine Wirkung, weil die
  A-Entscheidungen bei Elzas Naehe (2000) Angriffe waehlen. auf2 (Jagd B[3] ohne A-Entscheidungen, 25e1e2d5): der Gorilla
  dreht zu John (rot 2655 -> 1079 = Richtung John), kriecht, bleibt aber am zweiten Gorilla/Tisch haengen (x -6982 fest,
  Abstand 6600 +-80, `auf2.png`/`auf2_zoom.png`); danach (Aufmerksamkeit aus) Biss/Aufbaeumen gegen Elza. Folge: Jagd endet,
  wenn der Gorilla in 30 Bildern nicht 300 naeher an John ist (kein Kriechen auf der Stelle, Commit danach).

### 8.3 Tests (gezielt, keine volle Suite)
- Unit (Module des Diffs: dining_r38/39, Kartenbits, Gorilla-KI enemy_ai_common.c, Schaden re15_damage.c, Spieler-Ziel-/
  Plc-Pfad, Ton/Stimme/BGM audio_pc.c, Nachrichten-FSM, Elza-Anker r36, Heliport 1171), Regex woertlich:
  `unit_r39_dining1141|unit_r38_dining1141|unit_r36_kartenbits|unit_maggot|unit_r35_affen|unit_r38_affen3|unit_r36_zombieki_affen|unit_r38_griff_gorilla|unit_rotor_bgm_pin|unit_r35_cut10f0|unit_synchro_stimme|unit_voice_ausreden|unit_r39_stimmen|unit_r38_rekorder|unit_r38_elzaintro_stimme|unit_se_bank_routing|unit_se_voice_machine|unit_r30_tuer_stimme|unit_audio|unit_r34n_f_leiche_stimme|unit_r38_tagebuch|unit_r38_elzatoene|unit_msg_dialog|unit_action_msg_gate|unit_r38_heliport1171|unit_scd_flag_fade|unit_damage|unit_weapon_select|unit_aim_all_around|unit_aim_lower_exit|unit_plc_back_yaw_1090|unit_r30_android_n3_plc_aim|unit_r36_elza|unit_r39_gegner`
  -> "100% tests passed, 0 tests failed out of 122".

### 8.4 Je Mangel: Ursache / vorher / Beleg / Aenderung / nachher (Kurzfassung)
| Mangel | Ursache (vorher gemessen) | Beleg | Aenderung | nachher gemessen |
|---|---|---|---|---|
| M1 | Reaktion nur unter Ck(9,94); Gemaelde-Frage friert die VM (Pausemaske 0xff80 -> RE15_PAUSE_SCD, scd_vm.c @0x8003f040), nach "ja" lief HAEMMERN in die Reaktion waehrend der Rolladen fuhr (Abnahme Lauf F: msg 50 F291) | Pausemaske `2b 0d 80 ff` sub06 @0x00FE8; Frame-Runner @0x8003f040-4c | Merker (5,0x10) synchron bei Ereignis 6, Reaktion wartet auf Merker + (2,7), nur bei Ck(4,0xC3)==0 | Lauf f: Rolladen F250..F470, msg 50 F481; unit fenster |
| M2/N4.4 | msg 52 mit Plc_motion(2,6) = Johns Zielen (eigene Bank Clip 6) | ROOM1021 @0x020C6 (Zielen vor dem Schuss), ROOM1141 @0x00C78 (zielt auf die Eintretende, msg 1 "sorry"); Nachtrag 7: ROOM1171 @0x014CE + RBJ rec1 Clip 2 | erst Clip 17, nach Nachtrag 7 das Heranwinken aus 1171 (geliehen, 8.8) | Lauf wink: msg 52 F3015 mo=2 (96 Bilder), kein (2,6) im Gespraech |
| M3 | Pos_set John hinter Sleep 1/10/5 (16 Bilder nach dem Schnitt) | Abnahme Lauf B F4358 -> F4359 | Pos_set direkt nach Cut_chg 8 | Lauf pist: Schnitt und Pos_set im selben Bild F2873; Riegel "kein Sprung" (Unit + Integration) |
| N4.1 | Gemaelde startete die ganze Szene | AUFTRAG Nachtrag 4 | ROLLTOR39, Phase H bis (4,0xC3) | Lauf n1: frei laufen F~1000..F1599, 18 Schlaege |
| N4.2 | Szene ohne Riegel/Rennen/Waffe | sub05 @0x00FC8..@0x00FDC; ROOM3060 @0x010BE; 0x80073e30[2]; W-Bank @0x80036be4/@0x80036c04; Messer-DRAW @0x80035538 | ENTRIEGELN39 + W-Clip-Maschine | Laeufe n1/pist/mess: msg 12, Rennen, Cut 7, Pistole hoch / Messer gezogen, Senken nach Cut 6 |
| N4.3 | John schoss nie | ROOM1021 @0x020C6..@0x020F2; Schuss-Ton Runde 36 L/38 H; Schaden @0x8006ee44 | SCHUETZE39 + Treffer + Aufmerksamkeit | Laeufe pist/auf2: Salven alle ~370 Bilder, Treffer, Gorilla dreht zu John |
| N6 | Grossschreibung im Text | Nachtrag 6 | "trouble", "gorilla" | Laeufe n1/f: [msgtext] klein |

### 8.5 OFFEN (Nachbesserung 1)
- N4.3 Aufmerksamkeit: der getroffene Gorilla dreht zu John und kriecht zu ihm, solange er in 30 Bildern 300 naeher kommt;
  der Weg wird oft vom zweiten Gorilla oder vom Tisch versperrt (Lauf auf2) - dann endet die Aufmerksamkeit nach 30 Bildern
  und der Gorilla nimmt wieder Elza. Gorillas greifen John nicht an (die Angriffe des Originals wirken nur auf den Spieler).
  Takt der Salven (Pause 90+90), Trefferkegel +-0xC8 und Dauer 180 sind PORT-WAHL (kein RE1.5/RE2-Vorbild fuer einen
  schiessenden Begleiter).
- Gemeinsame Dateien ausserhalb von VERTRAG 1.3 fuer J: enemy_ai_common.c (Gorilla-Hirn: 2 Haken, Spur E Nachbesserung 3
  arbeitet am Gorilla in 1141 - bei der Integration zusammen ansehen), game_step_common.c (1 Zeile), platform/pc/main.c
  (2 Zeilen), re15_damage.c (Zugriff auf die Schadenszeile, 5 Zeilen), scd_vm.c (2 weitere Zeilen). Alle mit Kommentar
  "Runde 39 Spur J", alle auf ROOM1141 begrenzt.
- Spieler-Plc_motion(2,c) = W-Bank-Clip gilt im Port nur in ROOM1141. ROOM3060 sub03 @0x010BE und ROOM3061 sub02 @0x011DE
  tragen dieselbe Form (Leon/Elza heben die Waffe) und zeigen im Port weiter PL00-Clip 6 - eigener Punkt fuer eine
  spaetere Runde.
- Spur K (1171 "Elza, hurry up!"): Johns (2,6) ist Zielen (ROOM1021-Beleg oben); die Folge-Geste des Originals ist RBJ-Clip
  17. Falls Spur K in 1171 dieselbe Geste waehlt, passt 1141 dazu.

### 8.6 Fuer den Nutzer (Nachbesserung 1)
- Ablauf jetzt: Eintritt -> Klopfen + Hilferufe -> Elzas Satz (einmal, nie waehrend einer Szene) -> Gemaelde -> Rolladen
  hoch -> **frei laufen** (John klopft weiter) -> am Riegel der Dachtuer "You've opened the lock." -> Elza **rennt** in den
  Raum, die Kamera schneidet auf die Ansicht vom Gangende, Elza **dreht sich zur Tuer und hebt die Waffe** (eine Schusswaffe
  aus dem Inventar, die dabei ausgeruestet wird; ohne Schusswaffe zieht sie das Messer) -> Tuer auf/zu -> Kamera auf John
  an der Tuer -> Elza senkt die Waffe -> Szene wie bisher -> im Kampf **schiesst John ab und an** (zwei Schuesse je Salve,
  Pistolenschaden; der getroffene Gorilla wendet sich John zu) -> John: "Puuuh..." -> "Elza, follow me..." mit dem
  **Heranwinken aus 1171** (dieselbe Geste wie dort zu "Elza, hurry up!", Nachtrag 7) statt Zielen -> Abgang.
- Neue Sprachdateien wie bisher moeglich: main50/51/52.wav (Texte jetzt "trouble" / "gorilla" klein).
- Entscheidungsfragen: (1) Soll ein Gorilla John auch angreifen koennen (dann braucht John Treffer-/Todesverhalten, das
  RE1.5 nicht hat)? Jetzt: nur Zuwenden/Hinkriechen. (2) Takt "ab und an": jetzt alle ~12 s eine Salve - passt das?
- Integration erster Durchgang (Stand 25e1e2d5 + Lenkziel-Pruefung, `ctest -R "integration_r39_dining1141_d|integration_r38_dining1141|integration_r35_cut10f0|integration_r38_heliport1171_weg_re15|integration_r39_stimmen_1171|integration_r38_rekorder|integration_r38_elzaintro_stimme|integration_r37_elzaems_elza1141|integration_r36_elza_schuss" -j 2`):
  "87% tests passed, 2 tests failed out of 15" - rot NUR r39 D und r38 S: beim Umbau der Wegplaene war der vierte
  Druck am Gemaelde ("ja") verloren gegangen (Lauf r39 D: msg 13 1869 Bilder offen, (3,0x6A) blieb 0). Behoben 4aaece87;
  alle 13 anderen gruen (cut10f0 302 s, rekorder 304 s, r38 R/W/Z/L/A/D, elzaintro, heliport_weg, stimmen_1171,
  elzaems_elza1141, elza_schuss).
- Nach dem Cut-7-Schnitt (fe0c7a97) Unit erneut (Regex oben): "100% tests passed, 0 tests failed out of 122".
- Port-Bank 0x3E (Hinweis H7 der Abnahme): master enthaelt Spur I (68bfe620); ausser dining_r39 belegt keine Datei 0x3E
  (`git grep` auf master: nur 0x3A Spur 36 J) - kein Konflikt.

### 8.7 Endstand (Bau 6b8c7f69, gezielt)
- Integration zweiter Durchgang am Endstand (nach fe0c7a97 Cut-7-Schnitt, 4aaece87 Wegplan, 6b8c7f69 Sprung-Riegel):
  `ctest -R "integration_r39_dining1141_d|integration_r38_dining1141_s|integration_r37_elzaems_elza1141" -j 2` ->
  r38 S "Passed 717.58 sec" (zwei Laeufe mit Sichtpruefung je Bild: John taucht in keinem Bild ohne Bewegung oder Schnitt
  auf - auch nicht beim neuen Tuer-Auftritt und beim Schiessen), elzaems_elza1141 "Passed 13.84 sec", r39 D rot nur am
  Sprung-Riegel (F4296 = RUN-Schritt im Ankunftsbild, Sub 5 -> 6; Riegel berichtigt 6b8c7f69, derselbe Lauf per
  -DAUSWERTEN gruen). Danach `ctest -R "integration_r39_dining1141_d"` -> "100% tests passed, 0 tests failed out of 1"
  (Passed 351.08 sec, Lauf lauf_5ecb300d): John erstmals F1784 bei Cut 7 hinter der Kamera, Cut 6 mit John F1854, Cut 7
  ab F1708 (Elza laeuft ins Bild; Kamerafolge vom Riegel bis zum Heben 6 -> 5 -> 7, kein Cut 0), N4.1 386 freie Bilder
  nach dem Rolladen, entriegelt bei z -1060, Messer gezogen F1744 (w=13) bei z -12652, N4.3 2 Schuesse / 2 Treffer
  (F3857/F3887), M2 Clip 17 und 0 Bilder (2,6) im Gespraech, M3 groesster Schritt 0, Punkt 7 0 von 268, Punkt 9 Takte
  9889..9914 Pegel 0 / 9929..9959 Pegel 559 (Bogen `j39n1/d_final_lauf.png`: F1700 Cut 5, F1710 Cut 7 Elza laeuft ins
  Bild, F1750 Messer gezogen, F1860 John an der Tuer).
- Unit am Endstand, Regex wie 8.3 -> "100% tests passed, 0 tests failed out of 122".
- Gesamt Integration (erster + zweiter Durchgang, gleicher Code fuer alle Nicht-1141-Riegel): 15 von 15 gruen.


### 8.8 Nutzer-Nachtrag 7 (20:28, waehrend dieser Nachbesserung in den Baum committet: 363626f4)
Wortlaut: "Nein, die Animation beim follow me in room 1141 von John ist schon wieder falsch... die soll so aussehen wie in
room 1171, die du jetzt korrigiert hast..." Orchestrator (Belege Spur K): das korrigierte Heranwinken in 1171 = Original
Plc_motion(0,2) @0x014CE = RBJ-Record 1 (Marker 0x2 = John) Clip 2 von ROOM1171, 96 Bilder.
- Beleg selbst nachgelesen: ROOM1171 sub02 @0x014BA `2b 00 00 00` msg 0 "Elza, hurry up!", @0x014BE `2e 02 00 00`,
  @0x014CE `3f 00 02 00`, @0x014D2 `09 0a 3c 00`; RBJ von ROOM1171: RDT+0x5C = 0x188C, Kopf total 0x408C, 2 Records:
  rec 0 Prefix 0x8 Marker 0x1 (Spieler), rec 1 Prefix 0x1088 EDD 0x3E34 Marker 0x2 (Gegner 0 = John); der Port-Parser
  re15_emd_parse_rbj_record liest rec 1 mit 3 Clips, Clip 2 = 96 Bilder (Riegel phase_h, Logzeile im Lauf).
- Umsetzung (751e3f4d): msg 52 + Work_set(2,0) + Plc_motion(0,2) + Sleep 60 (Form ROOM1171 @0x014BA..@0x014D2) + Sleep 40
  (100 >= 99 Bilder msg 52 / 96 Clip). ROOM1141 hat diesen Clip nicht (seine RBJ-Records tragen die Gesten 15..24): Leihe
  fuer die Dauer der Geste - Anker WINK (ENTRIEGELN39 +0x048C) parst Record 1 der ROOM1171-RBJ gegen Johns Skelett (wie
  enemy_common.c rbj_resolve_slot), Haken in re15_actor_rbj_anim/_skel (enemy_common.c, je 2 Zeilen) geben ihn nur bei
  John Zustand 4/Sub 0/Clip 2 heraus; Plattform liest STAGE1/ROOM1171.RDT einmal (dining_r39_pc.c, Muster cut10f0_pc.c).
  Der Armschwung Clip 17 (vorheriger Stand dieser Nachbesserung) ist damit ersetzt.
- Gemessen (Lauf wink, Kurzweg wie pist): `[rbj] ROOM1141: Animationsblock von ROOM1171 fuer Johns Heranwinken gelesen
  (16540 B)`, `F3014 John winkt heran: RBJ ROOM1171 Record 1 Clip 2 geliehen (96 Bilder, 3 Clips)`, msg 52 F3015..F3113
  mit s1=0 mo=2, John laeuft F3116 los; Bild `j39n1/wink.png`/`wink_zoom.png`: John winkt mit dem linken Arm mehrmals heran
  (Arm vor - Hand zurueck, F3036..F3092), in der rechten Hand die Pistole gesenkt, keine Waffe auf Elza.
- Spur K hat in 1171 den RBJ-Kanal-Cache berichtigt (18940e95, Slot ohne Record belegt keinen Platz); die Leihe hier geht
  am Cache vorbei - bei der Integration zusammen ansehen.

- **Endstand nach Nachtrag 7 (751e3f4d, Bau 2026-10-18 21:47 - exe 21:47:36, Commit 751e3f4d 21:48:38, die Riegel-Laeufe unten vor f744e5ad 22:13:31; berichtigt in Nachbesserung 2 nach Hinweis H6 der Abnahme 1, vorher hier "~22:45"):**
  `ctest -R "integration_r39_dining1141_d|integration_r38_dining1141_s" -j 2` -> "100% tests passed, 0 tests failed out of 2"
  (D Passed 643.52 sec, S Passed 1103.82 sec unter Parallellast); Unit, Regex wie 8.3 -> "100% tests passed, 0 tests failed
  out of 122". Damit gesamt am Endstand gruen: Unit 122/122, Integration 15/15 (die 13 Nicht-D/S-Riegel aus dem ersten
  Durchgang beruehren den Nachtrag-7-Code nicht: Leihe nur in ROOM1141 bei John Zustand 4/Sub 0/Clip 2).


## 9. Nachbesserung 2 (nach Abnahme 1, `J_abnahme_1.md`; Start 2026-10-18 23:00, Uhrzeiten aus `date`)

Auftrag: (0) `git merge master` (8286bcb6: Welle 1+2, Spur E Nachbesserung 2/3 = Gorilla HP 360 / Flinch ab 36 fuer alle
0x27 inkl. 1141 + Querungs-Wache in 1141, I funk10f1, M schluessel2, Nutzer-Nachtraege 8 + 9), Konflikte beidseitig
aufloesen, bauen. (1) Nachtrag 8: Nutzer-Stimmen synchro/STAGE1/room1141/main50/51/52.wav am echten Weg messen
(Ordnerregel, keine Tabellenzeile), Original-Zeilen 12/13/14 bleiben stumm. (2) Mangel M1 (N4.3) nach dem Merge neu
messen und beheben: (a) Treffer auch bei gepacktem Elza, (b) Richtung waehrend der Zielbilder nachfuehren, (c) klemmender
Gorilla: sichtbare Zuwendung oder nachvollziehbares Ende; John beim Schiessen wenigstens einmal im Bild; Regel im
Abschnitt "Fuer den Nutzer" einfach beschreiben (keine Entscheidungsfrage). (3) H6: Uhrzeit in 8.8 berichtigen.

### 9.0 Stand / Fortschritt
- 23:00 Baum sauber auf 0aa7b194 (Abnahme 1), master = 8286bcb6. Dossier-Abschnitt angelegt.
- 23:02 Merge master (8286bcb6 + Orchestrator-Doku 28dad473 UEBERGABE/args_j_nach2) -> 25611933 (Konflikte AUFTRAG.md: Fassung master + Orchestrator-Hinweis 14:27 aus dem Zweig;
  test_r36_kartenbits.c: beide Zeilen Bit 94 J / Bit 70 I; enemy_ai_common.c, game_step_common.c, scd_vm.c automatisch, an
  verschiedenen Stellen: E-Haken HURT/Querung, I Ereignis 27). configure + build LOCAL-BUILD-OK.
- 23:06-23:13 Messung vorher am gemergten Stand (Laeufe v_a, v_l, v_l2, v_l3, gestartet 23:06:41; 9.1).
- 23:23 Umsetzung dccd5f8c (9.3, Commit 23:23:21); Laeufe n_a, n_l, n_l2, n_l3 gestartet 23:23:32 (Uhrzeiten aus `git log` / Dateizeiten).
- 23:32 Riegel (05bc9a4d), 23:34 Riegel `kampf` (ee3cd7c2), 23:35 Dossier 9.4-9.8 (ec02857e); 23:34 gezielte Suite gestartet (9.5).
- 23:38-23:49 Integration gezielt (13 von 14, rot nur der E-Riegel, Merge-Folge, 9.9); 23:57 E-Riegel nachgezogen (b5a42cca),
  gruen; 23:58-00:10 alle Unit-Riegel 842/842 (9.10).
- 00:11 Nutzer-Nachtrag 10 vom Orchestrator in den Baum committet (3f0ef678); 00:14 umgesetzt (11d366a9, 9.11), 00:21 Riegel
  (35ac16f1); 00:34 Johns Schuss vor die Gegner-KI (6161ac63, 9.12); 00:34-01:15 Unit 842/842 und Integration 14/14 am
  Endstand; 01:16 Riegel `kampf` mit Messhaken (cc2715b8), Lauf bis 01:21: 1/1 (9.13). Uhrzeiten aus `git log`.
- H6 erledigt: Uhrzeit in 8.8 aus den Commit-Zeiten berichtigt (751e3f4d 21:48:38, exe 21:47:36, f744e5ad 22:13:31).

### 9.1 Messung vorher (gemergter Stand 25611933, echte exe; Messordner scratchpad `j39n2/`, nicht committet)
Werkzeug `j39n2/lauf.sh` (eigene exe-Kopie `re15_pc_j39n2_<name>.exe`, Umgebung wie Abnahme 1 `J39abn1/lauf.sh`, dazu
RE15_R39_GEGNER_LOG fuer Spur E), Auswertung `salven.py` (je Schuss: Ziel, Elzas Griff gr=, Zustand des Ziels, Peilung rel in
der Konvention von re15_ai_arc_test mit atan2_q12 aus actor_locomotion.c, Kamera, John im Bild per Projektion),
`aufmerk.py` (Aufmerksamkeits-Episoden), `stimme_xkorr.py` (Kreuzkorrelation Sprachdatei gegen Mitschnitt).
Laeufe: v_a = Abnahme-Lauf A (echter Weg 1131 -> 1141 -> 1171, Messer, Gorillas starr + Messhaken 600), v_l/v_l2/v_l3 =
Abnahme-Laeufe L/L2/L3 (Pistole im Inventar, lebende Gorillas, RE15_R38F_GORILLAS_TOT=2400).

**Stimmen (Nachtrag 8), Lauf v_a** (debug.log + Mitschnitt, 1141: Takt = F + 3659 aus `Se_on ... F35 tick=3694`):

| Zeile | `[voice] clip loaded` | offen (r39.txt) | Kreuzkorrelation im Zeilenfenster |
|---|---|---|---|
| msg 50 Elza | main50.wav (32000 Hz x1) | F176..F356 (181 Bilder = Laenge der Aufnahme 180,1 Takte) | rho 0,903 bei Takt 3834 (Haelften 0,920/0,886; Median 0,004) |
| msg 51 John | main51.wav | F4000..F4174 (175) | rho 1,000 bei Takt 7658 (Haelften 1,000/1,000) |
| msg 52 John | main52.wav | F4176..F4304 (129) | rho 1,000 bei Takt 7834 (Haelften 1,000/1,000) |
| Gegenproben | main51 im Fenster von msg 50: rho 0,043; main50 im Fenster von msg 51: 0,033 | | |

Die Original-Zeilen bleiben stumm: `[voice] ROOM1141 msg 13: keine Aufnahme (synchro/STAGE1/room1141/main13.wav) - Zeile
bleibt stumm` (Gemaelde) und dasselbe fuer msg 12 (Schloss); msg 14 (Tisch) kommt im Weg nicht vor, `synchro/STAGE1/room1141/`
hat nach der Umbenennung keine main12/13/14.wav mehr, die alten Takes liegen unter `synchro/unused/`.
Befunde: (S1) die Pad-Sperre der Reaktion endet nach `Sleep 90` (F266), Elza spricht bis F356 weiter, waehrend der Spieler
schon laeuft; (S2) msg 52 wartet richtig auf das Ende von main51 (Stimmen-Riegel), aber John rennt nach `Sleep 60 + 40` los
(`[scd F4275] Cut_chg(7)`, r39.txt F4276 s1=5), seine Zeile laeuft bis F4304 - er rennt mitten im Satz weg.

**Kamera nach Johns Abgang (neu gefunden, Lauf v_a und Abnahme-Lauf A):** r39.txt `anz=8` von F4418 bis F6200, waehrend Elza
von (-5800,-21500) ueber (-6000,-12000) zur Dachtuer (-7227,-1054) geht; die Bilder `J39abn1/l_a/fd_005900..006100` zeigen nur
die leere Suedwest-Ecke (`j39n2/abn_a_ende.png`). Ursache: Cut_chg(7) fuer Johns Abgang merkt den ANGEZEIGTEN Cut (scd_vm.c
op_cut_chg @0x800402c0/@0x800402e4) = Cut 8 des Gespraechs; das Programmende `2a 00 3c 01` (Cut_old, Cut_auto 1) kehrt zu Cut 8
zurueck. RVD ROOM1141 (RDT+0x28 -> @0x01A8, 20 B je Satz, selbst geparst): Gruppe Cut 8 = nur Satz 22 `01 ff 08 00 fc d6 64 9c ..`
= der Gruppenkopf, den der Zonen-Scan ueberspringt (rdt_common.c re15_rdt_apply_zones_as_aots: erster Satz je cam_from) - aus
Cut 8 fuehrt kein Wechsel. Eingefuehrt mit dem Gespraech in Cut 8 (b2a2f267, Punkt 8); vorher (Runde 38) lag vor Cut 7 der
Kampf-Cut. Die Abnahmen bewerteten nur das Gespraech, nicht die Bilder danach.

**M1 (N4.3) am gemergten Stand** (`salven.py`; Spur E: 360 Energie, Treffer unter 36 Energie ohne Zucken, gegner_r39.log
`affen treffer ... OHNE ZUCKEN (zurueck in 1/3/1/0)`):

| Lauf | Salven | ohne Treffer | Treffer ohne sichtbare Zuwendung | mit Zuwendung |
|---|---|---|---|---|
| v_l | 5 (F3771..F5255) | 4: alle gr=1 (Elza im Griff), Ziel Sub 2/3 (kein Angriff), rel -45..-1 (im Kegel 0xC8) | 1 (F5255: 2 x 29 Bilder, Lage (-6982,-9488) fest) | 0 |
| v_l2 | 6 | 1 (F4137: rel -597/-612, Ziel lief waehrend der 110 Zielbilder aus dem Kegel) | 4 (Gorilla 3 bei x -6982: je 29 Bilder, Drehung 3157 -> 3123) | 1 (F3765: Gorilla 2 6513 -> 3276 in 149 Bildern) |
| v_l3 | 5 | 2 (F3771 Ziel im Griff-Sub 15 = Greifer; F4142 gr=1, Ziel Sub 2) | 2 (F4513, F5262 je 29 Bilder) | 1 (F4890: 89 Bilder, 10355 -> 9241; F5292 dazu 179 Bilder, 13356 -> 3125) |

Zusammen 16 Salven: 7 ohne Treffer (5 x gr=1 auf ein Ziel, das NICHT angriff = Mangel (a); 1 x Kegel = (b); 1 x Greifer),
6 Treffer ohne Zuwendung, 3 mit Zuwendung. Nach dem Merge zuckt kein Gorilla mehr bei Johns ersten zwei Treffern (H3 bestaetigt);
die Aufmerksamkeit haengt nicht am Zucken - sie endet aber am ersten Prueftakt nach 29 Bildern.
Ursache (c), gemessen am Zustands-Log v_l2 F4515..F4545 (Gorilla 3, `state.txt`): nach dem Treffer B[3] (ss1=3, Clip 5), der
Gorilla rueckt 1876 nach Sueden und wieder zurueck, die Drehung bleibt bei 3157 -> 3123 (Peilung zu John 2005, 1120 daneben)
- er dreht nicht zu John, obwohl der B[3]-Lenkschritt 6..37 je Bild (@0x80117d1c-34) in 30 Bildern 180..1110 erlaubt. Das
Lenkziel kommt aus `re15_r36_affen_padsuche_ziel` (Runde 36 F): ist dessen Umweg aktiv (`s_r36_padsuche_aktiv`), steuert B[3]
den UMWEG-Punkt an statt John; der Umweg wird mit der Lage des SPIELERS geplant (`r36_uw_route(e, pl, slot)`) und waehrend der
Aufmerksamkeit nicht neu bewertet (A[3] laeuft nicht, `ab_nur_b`). x -6982 = Westwand x -8600 (SCA `x -10600..-8600`) + Radius
1618 des Gorillas (`sca.py`: naechste Zelle 1618) - der Gorilla kommt in x nicht naeher als 553 an John (x -7535) heran.
Dazu der Prueftakt: nach 30 Bildern ohne 300 Fortschritt war die Aufmerksamkeit vorbei - auch wenn er sich noch drehte.
John im Bild: Cut 0 zeigt Johns Ecke (Projektion (122,90..117), Tiefe ~13000), Cut 1/2/5 nicht (hinter der Kamera). In v_l und
v_l3 fielen die ersten Schuesse in Cut 0, John stand aber hinter den Gorillas, die Elza packten (`j39n2/v_l_schuss1.png`).

### 9.2 RE-Belege (diese Sitzung selbst gelesen)
- **Nachziehen der Waffe (b):** `re15_disasm.py dis 0x80033eec`: Gun-FSM Sub 5 (Wechsel aufs naechste Ziel, Schreiber der 5
  in HOLD @0x800332f8): Phase 0 `ori v0,zero,0x7` / `sb v0,-13597(at)` (+0x8f = 7), `jal 0x80037250` mit `ori a0,zero,0x7530`
  (Zielsuche 30000 @0x80033f50/@0x80033f54); Phase >= 1 @0x80033f84: `ori a1,zero,0xc8` @0x80033f90, `jal 0x8001a8f8`
  @0x80033f94 (a0 = Ziel+0x34 @0x80033f98) = Drehung zum Ziel je Bild um hoechstens 0xC8; @0x80033fa8 `andi v0,v0,0x100`
  (Taste noch gehalten, sonst aca5a = 3 Senken @0x80033fbc), `jal 0x8001a9cc` @0x80033fcc mit `ori a1,zero,0x64` @0x80033fb0
  (ausgerichtet -> aca5a = 1 HOLD @0x80033fe4), `jal 0x8001f314` @0x80033ff8 (Haltebild der W-Bank). Dieselbe Drehung beim
  Zielbeginn: Sub 0 `ori a1,zero,0xc8` @0x80032fe0 / `jal 0x8001a8f8` @0x80032fec. Aufrufer von FUN_8001a8f8 im ganzen PSX.EXE
  (jal-Suche 0x0C006A3E): 0x80032fec, 0x80033f94, 0x80034134, 0x80034fbc, 0x80035620.
  Port: re15_enemy_steer_point (enemy_ai_common.c) = FUN_8001a8f8-Kern (`8001a958..8001a9b0`, Kommentar in player_common.c).
- **Zugewandt (c):** Gorilla A[3] `a804(0xbb8,0x180)` @0x80117a60-74 (enemy_ai_common.c re15_dog_arc(e, pl, 3000, 384)) =
  Biss-Kegel +-0x180; B[3]-Lenkschritt `(rng & 0x1f) + 6` @0x80117d1c-34, `jal 0x8001a8f8` @0x80117d50.
- **Kein Abbruch des Griffs durch Dritte (a):** der Wurf laeuft im SPIELER-Takt: Opfer-Handler 0x8011c118 als Kommando 5
  (`jalr v0` @0x80031cb4, re15_affen.h (6d)), Phasen P0..P7, Freigabe nur in P7 0x8011c384 (aca58 = 1, +0x93 = 0); der Greifer
  haelt Bit 0x1000 von Phase 3 bis Phase 4 (@0x8011ac34-38 setzen, @0x8011ad8c-94 loeschen). Ein Treffer auf den Greifer wuerde
  Sub 15 abbrechen, ohne dass Bit 0x1000 je geloescht wird - das Original hat dafuer keinen Weg (der Spieler kann im Griff nicht
  schiessen). Deshalb bleibt der Greifer ausgenommen; der ANDERE Gorilla ist frei treffbar.
- **Kamera am Ende:** RVD @0x01A8 Satz 0 `01 ff 00 00 fc d6 64 9c fc d6 cc de b8 0b cc de b8 0b 64 9c` = Gruppe Cut 0, Viereck
  (-10500,-25500)..(3000,-8500), enthaelt Elzas Gespraechslage (-5800,-21500) (Kampf davor: Lauf v_a F3977 `anz=0` bei
  (-5636,-18958)); Satz 22 `01 ff 08 00 fc d6 64 9c fc d6 8c bf 24 fa 8c bf 24 fa 64 9c` einziger Satz der Gruppe Cut 8.
  Original-Form fuer das Szenenende: ROOM1141 sub02 @0x00E80 `29 00` (Cut_chg in die Kamera von Elzas Lage) vor @0x00F56
  `3c 01` Cut_auto 1; scd_vm.c op_cut_old: Cut_old stellt DAT_800b3f7b her (@0x8004033c), den beim letzten Cut_chg angezeigten Cut.
- **Stimmen ausreden lassen:** der Port oeffnet eine Zeile erst nach dem Ende der laufenden Sprachzeile (scd_vm.c op_message_on,
  `g_re15_voice_laeuft`/`g_re15_voice_restbilder` aus audio_pc.c re15_audio_tick); dieselbe Warte-Zeile msg 58 traegt schon den
  Glasbruch (Punkt 6). Ohne Aufnahme laeuft sie sofort durch (Riegel `fenster`/`phase_h`).

### 9.3 Umsetzung (Commit dccd5f8c)

| Mangel | Aenderung | Datei / Konstante |
|---|---|---|
| M1 (a) | `schuss()` ohne `re15_player_victim_state() == 0`; Grund je Fehlschuss im Log (tot / im Angriff / nicht im Kegel). Ziel = naechster lebender Gorilla, der nicht angreift (`ziel_waehlen`), sonst der naechste lebende; im Zielen neu waehlen, wenn das Ziel tot ist oder angreift | dining_r39.c |
| M1 (b) | Anker ZIELEN (Plc_motion(2,6) Zielen) .. SENKEN (Plc_motion(2,6) rueckwaerts) in SCHUETZE39; dazwischen dreht John je Bild um hoechstens `RE15_R39J_NACHFUEHREN` 0xC8 zum Ziel (re15_enemy_steer_point) | programme_bauen.py, gen/*.inc (OFF_ZIELEN 0x002E, OFF_SENKEN 0x005E), re15_dining_r39.h @0x80033f90/@0x80033f94 |
| M1 (c) | Gorilla-B[3]-Haken: `re15_dining_r39_affe_ziel` liefert 1 = John, dann Lenkziel John OHNE Pad-Umweg; Fortschritts-Takt (30 Bilder / 300) erst ab Zuwendung `RE15_R39J_ZUGEWANDT` 0x180, je Treffer neu; Log "zugewandt" / "vorbei - festgeklemmt" | dining_r39.c, enemy_ai_common.c (Haken, 3 Zeilen umgestellt), re15_dining_r39.h @0x80117a60-74 |
| Stimmen | HAEMMERN39: nach msg 50 + Sleep 90 `2b 3a 00 00` (msg 58) vor Set(9,94)=1 / Pad frei; ENTRIEGELN39: nach msg 52 + Sleep 60 + 40 `2b 3a 00 00` vor Plc_neck aus / Abgang | programme_bauen.py |
| Kamera | Programmende `29 00 3c 01` (Cut_chg 0 + Cut_auto 1) statt `2a 00 3c 01` | programme_bauen.py, `RE15_R39J_CUT_ENDE` 0 (RVD @0x01A8 Satz 0 / Satz 22) |
| Messschiene | r39.txt `zl=<zielt>/<Ziel-Slot>` vor `blick=` (die Riegel-Regex endet weiter auf `blick=`) | dining_r39.c |

### 9.4 Messung nachher (Stand dccd5f8c, echte exe, gleiche Laeufe/Wegplaene wie 9.1: n_a, n_l, n_l2, n_l3)
**Stimmen und Ablauf, Lauf n_a** (1141: Takt = F + 3668 aus `F35 tick=3703`):

| Zeile | `[voice] clip loaded` | offen | Kreuzkorrelation | danach |
|---|---|---|---|---|
| msg 50 | main50.wav | F176..F356 | rho 0,903 bei Takt 3843 (Haelften 0,920/0,886) | Pad-Sperre F166..F371, frei ab F372 = NACH der Zeile (vorher: frei ab F266 mitten im Satz, 77 freie Bilder mit msg 50) |
| msg 51 | main51.wav | F4092..F4266 | rho 1,000 bei Takt 7759 | msg 52 wartet auf das Ende (Stimmen-Riegel) |
| msg 52 | main52.wav | F4268..F4396 | rho 1,000 bei Takt 7935 | `[scd F4397] Cut_chg(7)` + RUN = NACH der Zeile (vorher F4275, 29 Laufbilder mit msg 52); Mitschnitt Takt 8058 RMS 46, ab 8063 Stille |

msg 12 und 13: `[voice] ROOM1141 msg 12/13: keine Aufnahme ... - Zeile bleibt stumm` (unveraendert).
Kamera nach Johns Abgang: `[scd F4539] Cut_chg(0)`, r39.txt anz=0 ab F4540, anz=5 ab F6130, anz=6 ab F6293 (Elza an der
Dachtuer). Bilder `j39n2/n_a_ende.png`: F4540 Elza in Cut 0 am Fenster, F5900..F6100 Elza geht durch den Raum (Cut 0), F6200
Cut 5 mit Elza im Gang. Punkt 9 unveraendert: Werkzeug `pegel` auf n_a "Takte 10188..10213 Pegel 0, Takte 10228..10258
Pegel 559", 0 Fehler. Gegenprobe Riegel: `integration_r39_dining1141_d` per -DAUSWERTEN auf v_a (vor dem Fix) 3 Fehler
(Cut 8; 77 freie Bilder mit msg 50; 29 Laufbilder mit msg 52), auf n_a gruen.

**M1 (N4.3), Laeufe n_l / n_l2 / n_l3** (`salven.py`, `aufmerk.py`, debug.log `zugewandt`/`vorbei`):

| Lauf | Salven | Schuesse / Treffer | Treffer mit Elza im Griff (gr=1) | Zuwendung (Log "John zugewandt") | Annaeherung je Episode (Abstand zu John) | Schuesse bei Cut 0 |
|---|---|---|---|---|---|---|
| n_l (Elza stirbt F5130, wehrt sich nicht) | 4 | 8 / 8 | 2 | 4 von 4 | 5871->2808, 4264->2529, 7994->2582, 4933->2528 | 6 |
| n_l2 (Elza im Gang) | 6 | 12 / 12 | 0 (nie gepackt) | 5 von 6 (letzte Salve F5712 am Laufende) | 6513->2798, 9525->2528, 9394->2535, 9402->2529, 9406->2523 | 0 (Cut 5) |
| n_l3 (Elza neben John) | 5 | 10 / 10 | 6 | 5 von 5 | 3743->3366, 5612->2860, 5239->2790, 5503->3122, 4779->3109 | 8 |
| vorher v_l/v_l2/v_l3 | 16 | 32 / 17 | 0 | 3 von 16 Salven | 6513->3276, 10355->9241, 13356->3125; sonst 29 Bilder ohne Weg | 4 (John verdeckt) |

- (a) 8 Treffer, waehrend Elza im Griff war, alle auf den Gorilla, der sie nicht hielt; kein Fehlschuss mehr wegen des
  Opferzustands (Log-Grund `im Angriff`/`tot`/`nicht im Kegel`: in n_l/n_l2/n_l3 keiner).
- (b) 30 von 30 Schuessen im Kegel (rel -4..0); kein `nicht im Kegel`. Unit: nach 0x400 Abweichung im Zielen wieder im
  Kegel nach 5 Bildern, groesster Schritt 200 = 0xC8.
- (c) Der Gorilla am Westrand x -6982 (vorher 8 Treffer ohne Drehung) dreht sich jetzt zu John und laeuft am Rand entlang
  nach Sueden: n_l2 Gorilla 3 viermal (-6982,-12521) -> (-6985,-19384), je ~200 Bilder, Abstand 9400 -> 2530. Jede Episode
  endet am Ende mit "festgeklemmt" bei Abstand ~2520..3120 (|dx|+|dz|; euklidisch ~2050 = Gorilla-Radius 1618 + Johns
  Koerper): der Gorilla steht an John an, dann nimmt er wieder Elza. Bild `j39n2/n_l2_zuwendung.png` (Cut 5: der Gorilla
  kommt aus dem Gang auf die Kamera zu und laeuft unter ihr durch nach Sueden).
- John im Bild: n_l3 `j39n2/n_l3_john_zoom.png` F4990..F5025 und F5360..F5400 (Cut 0): John an seinem Platz neben der Pflanze,
  die Pistole auf den Gorilla gerichtet, der Gorilla dreht sich zu ihm (F5395/F5400). Vorher war John in Cut 0 hinter dem
  Gorilla verdeckt, der Elza hielt (`j39n2/v_l_schuss1.png`).
- Querungs-Wache (Spur E): 0 Ereignisse `querung` in n_l/n_l2/n_l3 (vorher v_l 2 x Elza-Wurf in die Westwand, abgefangen);
  Riegel `kampf`: niemand jenseits der Waende (Elza x >= -8600, Gorillas x >= -7100 / z >= -22100). Zucken nach Spur E: Johns
  erste zwei Treffer ohne Zucken, der dritte (Akku 36) zuckt (gegner_r39.log n_l2: 3 x FLINCH, 9 x OHNE ZUCKEN).

### 9.5 Tests (Nachbesserung 2)
- Unit `unit_r39_dining1141_programme`: + HAEMMERN39 msg 50 -> Sleep 90 -> msg 58 -> Set(9,94); SCHUETZE39 Anker ZIEL < ZIELEN
  < FEUER1/2 < SENKEN; ENTRIEGELN39 Heranwinken -> Sleep 60 + 40 -> msg 58 -> Abgang; Ende `29 00 3c 01`, kein Cut_old nach Cut 8.
- Unit `unit_r39_dining1141_phase_h`: + (a) Trefferbedingung `re15_dining_r39_trifft` bei Opferzustand 4 (freier Gorilla 0,
  Greifer Sub 15 = 3, ausser Kegel = 4); (b) Nachfuehren je Bild <= 0xC8, nach 0x400 Abweichung in 5 Bildern im Kegel; Kamera
  nach dem Abgang Cut 0 und Zonenwechsel nach Cut 5 (RVD Satz 2).
- Integration `integration_r39_dining1141_d`: + main50/51/52 je Zeile geladen (kein Hash/Dauer-Pin), msg 12/13 stumm, Ausreden
  msg 50/52, Kamera 0 -> 5/6.
- NEU `integration_r39_dining1141_kampf` (tests/integration/test_r39_dining1141_kampf.cmake, registriert in
  probes/r39_dining1141.cmake): echter Weg, Pistole, lebende Gorillas, Elza neben John (L3) - (a) Treffer bei gr=1, (b) kein
  Schuss ausser Kegel, (c) Mehrheit der Treffer-Salven mit Zuwendung, John beim Schiessen in Cut 0, Querungs-Wache.
  Gegenprobe -DAUSWERTEN: v_l3 (vor dem Fix) 4 Fehler, n_l3 und n_l gruen.

### 9.6 Je Mangel / Auftrag (Kurzfassung)

| Punkt | Ursache (gemessen) | Beleg | Aenderung | nachher |
|---|---|---|---|---|
| Merge | - | - | 25611933, Konflikte beidseitig | Bau gruen |
| Nachtrag 8 Stimmen | spielen bereits (Ordnerregel); aber Pad frei mitten in msg 50, John rennt mitten in msg 52 | Stimmen-Riegel op_message_on | Warte-Zeile msg 58 an beiden Stellen | n_a: rho 0,903/1,000/1,000, 0 freie Bilder / 0 Laufbilder mit offener Zeile |
| M1 (a) | Sperre `re15_player_victim_state() == 0` traf auch den freien Gorilla | Wurf im Spieler-Takt 0x8011c118/@0x80031cb4, Ende P7 0x8011c384 | Sperre weg, Greifer ausgenommen, Zielwahl freier Gorilla | 8 Treffer bei gr=1 |
| M1 (b) | Richtung nur vor den 110 Zielbildern | gun sub5 @0x80033f90/@0x80033f94 | Nachfuehren <= 0xC8 je Bild | 30/30 im Kegel |
| M1 (c) | Lenkziel = Pad-Umweg zu Elza (Runde 36 F), Prueftakt vor der Drehung | A[3]-Kegel @0x80117a60-74, B[3]-Lenkschritt @0x80117d1c-34 | Lenkziel John ohne Umweg, Takt ab Zuwendung | 14 von 15 Salven mit Zuwendung, Weg bis an John |
| John im Bild | Cut 0 zeigt Johns Ecke, John verdeckt | Projektion RID Cut 0 | (Folge von (a)/(c)) | n_l3 F4990..F5400 sichtbar beim Schiessen |
| Kamera (neu) | Cut_old -> Cut 8, Gruppe Cut 8 nur Kopf | RVD @0x01A8 Satz 0/22, sub02 @0x00E80 | Cut_chg 0 | n_a anz 0 -> 5 -> 6 |
| H6 | Uhrzeit geschaetzt | Commit-Zeiten | 8.8 berichtigt | - |

### 9.7 OFFEN (Nachbesserung 2)
- Gemeinsame Dateien ausserhalb VERTRAG 1.3 wie in 8.5 (enemy_ai_common.c: der B[3]-Haken bleibt 3 Zeilen, nur auf den
  Rueckgabewert umgestellt; er wirkt weiter nur bei Aufmerksamkeit > 0 = nur ROOM1141 nach Johns Treffer). Bei der Integration mit Spur E
  (Gorilla-Hirn) und K (RBJ-Cache) wie in 8.5 zusammen ansehen.
- Die Aufmerksamkeit endet am Ende mit dem Log "festgeklemmt", auch wenn der Gorilla an John ansteht (Koerperkontakt bei ~2050
  euklidisch, NAH-Schwelle 2000): sichtbar ist dasselbe (er steht bei John, dann wendet er sich wieder Elza zu).
- Spieler-Plc_motion(2,c) als W-Bank-Clip nur in ROOM1141 (8.5, unveraendert offen fuer eine spaetere Runde).

### 9.8 Fuer den Nutzer (Nachbesserung 2)
- **Johns Schuesse im Kampf (so funktioniert es jetzt, in einfachen Worten):** John schiesst etwa alle 12 Sekunden zweimal
  auf einen Gorilla. Er nimmt den naechsten Gorilla, der Elza gerade NICHT angreift - haelt ein Gorilla Elza fest, schiesst John
  auf den anderen. Waehrend er zielt, dreht er die Pistole mit, wenn der Gorilla sich bewegt (wie Leon/Elza beim Nachzielen).
  Ein getroffener Gorilla dreht sich zu John um und geht auf ihn zu, bis zu 7 Sekunden lang; kommt er eine Sekunde lang nicht naeher
  (Tisch, anderer Gorilla, oder er steht schon bei John), wendet er sich wieder Elza zu. Den Gorilla, der Elza gerade festhaelt, trifft
  John nicht (das Original kennt keinen Dritten, der so einen Griff unterbricht). Gorillas greifen John nicht an.
  Gemessen in drei Kampflaeufen: 30 von 30 Schuessen treffen, nach 14 von 15 Salven dreht sich der getroffene Gorilla zu John
  und geht auf ihn zu; steht Elza im suedlichen Teil des Raums nahe bei John (Kamera mit Blick auf die Doppeltuer),
  sieht man John schiessen.
- **Deine Aufnahmen main51/52** laufen an ihren Zeilen (John "Puuuh ..." und "Elza, follow me ..."), jede ganz; John rennt
  erst los, wenn er "... a way out of here!" fertig gesagt hat. main50 (Elza "Oh, no, this is John! ...") lief bis Nachtrag 10
  in 1141 ebenfalls ganz (Elza sprach aus, bevor du wieder steuerst); seit Nachtrag 10 kommt dieser Satz nicht mehr in 1141,
  sondern in ROOM1041 (Spur P, 9.11). Die alten Zeilen 12/13/14 (Schloss, Gemaelde, Tisch) bleiben stumm.
- **Kamera nach Johns Abgang:** vorher blieb die Kamera auf der leeren Suedwest-Ecke stehen, waehrend Elza zur Dachtuer ging;
  jetzt zeigt sie Elza und folgt ihr wie gewohnt.
- Keine Entscheidungsfrage offen. Paket-Gate: keine neue Datei unter shared_assets/ (Programme in gen/dining_r39_programme.inc).

### 9.9 Merge-Folge bei Spur E und Testlaeufe am Endstand (gezielt, keine volle Suite)
- Erster Lauf aller Unit-Riegel (Stand ee3cd7c2): `ctest -R "^(unit_|prop_|probe_)" -j 8 --timeout 600` ->
  "99% tests passed, 1 tests failed out of 842" - rot nur `unit_r39_gegner_1141_ablauf1141` (Spur E): der Riegel fuhr 1141 noch
  ueber "Gemaelde -> Szene" ("FEHLER: [vorher] Phase H -> Gemaelde -> BEFREIUNG: Kampf laeuft"); seit J Nachbesserung 1
  (Nutzer-Nachtrag 4) startet die Szene erst am Riegel der Dachtuer. Kein Fehler der Nachbesserung 2, eine Folge des Merges
  (master traegt E, aber noch nicht J).
- Integration (Stand ee3cd7c2): `ctest -R "integration_r39_dining1141|integration_r38_dining1141|integration_r39_gegner_1141|integration_r37_elzaems_elza1141|integration_r38_affen3" -j 4 --timeout 3600`
  -> "93% tests passed, 1 tests failed out of 14" (integration_r39_dining1141_d Passed 358.65 sec, integration_r39_dining1141_kampf
  Passed 303.52 sec, integration_r38_dining1141_s Passed 646.76 sec, w/r/z/l/a/d, affen3 (3), elzaems_elza1141 gruen); rot nur
  `integration_r39_gegner_1141` (Spur E): 0 Treffer-Zeilen - derselbe Wegplan "bis zum Gemaelde", die Gorillas brechen nie ein.
  Der Kopf dieses Riegels sagt selbst: "aendert eine spaetere Spur die Szene, ist der Wegplan bei der Integration nachzuziehen".
- Nachgezogen (b5a42cca, Dateien der Spur E, je Kommentar "Runde 39 Spur J"): Unit `bis_zum_kampf` wartet den Rolladen ab,
  drueckt den Riegel und schliesst msg 12 (Muster test_r38_dining1141.c bis_zur_ecke); Integration: Wegplan wie
  integration_r39_dining1141_d bis zum Riegel; Szenenende gemessen (scratchpad `j39n2/e_mess`, pm 0 ab F3655, vorher ~F2700),
  Schuetze und Ende um +960 verschoben (RE15_INPUT_SCRIPT_START 2760 -> 3720, RE15_EXIT_AT 4500 -> 5460, Kampfdauer gleich).
  Danach `ctest -R "unit_r39_gegner|unit_r39_dining1141|integration_r39_gegner_1141" -j 6 --timeout 1200` ->
  "100% tests passed, 0 tests failed out of 24" (ablauf1141: [nachher] 31/31 Treffer bis zum Tod, 10/10 Flinches;
  integration_r39_gegner_1141 Passed 303.05 sec: "OK - INIT 360 x 4, 33 Treffer-Zeilen (FLINCH 10 nur bei akku >= 36, OHNE
  ZUCKEN 23), Flinch-Beginne im state.txt 10"; Johns Treffer laufen dort durch dieselbe Regel).
- Alle Unit-Riegel am Endstand: siehe 9.10.
- OFFEN fuer die Integration: die beiden E-Riegel tragen jetzt den J-Ablauf; zieht Spur E sie in ihrem Zweig weiter, die
  Fassung aus b5a42cca uebernehmen (sonst sind sie nach dem Merge wieder rot).
- Fuer den Nutzer (Ergaenzung zu 9.8): die Entscheidungsfragen aus 8.6 entfallen - der Nutzer-Nachtrag 4 ist eindeutig ("ab und
  an schiessen und ihre Aufmerksamkeit erhalten"), die Regel steht oben in einfachen Worten; Gorillas greifen John weiterhin
  nicht an (die Angriffe des Originals wirken nur auf den Spieler, 8.5).

### 9.10 Zwischenstand vor Nachtrag 10 (Code b5a42cca, gezielt gruen; Endstand siehe 9.13)
- Alle Unit-Riegel am Endstand (Lauf 23:58:02..00:10:24): `ctest --test-dir re15_port/build -R "^(unit_|prop_|probe_)" -j 8 --timeout 600`
  -> "100% tests passed, 0 tests failed out of 842".
- Integration am Endstand: `ctest -R "integration_r39_dining1141|integration_r38_dining1141|integration_r39_gegner_1141|integration_r37_elzaems_elza1141|integration_r38_affen3" -j 4 --timeout 3600`
  -> 13 von 14 gruen (9.9), der eine rote E-Riegel nach dem Nachziehen gruen: `ctest -R "unit_r39_gegner|unit_r39_dining1141|integration_r39_gegner_1141" -j 6 --timeout 1200`
  -> "100% tests passed, 0 tests failed out of 24". Der D-Riegel mit dem Endstand-Skript (nur Variablenname H7 geaendert) per
  -DAUSWERTEN auf seinem ctest-Lauf lauf_7fd8ea61: "alle Pruefungen gruen". Zwischen den Laeufen nur Kommentar (re15_dining_r39.h
  `29 00`) und Riegel-Dateien geaendert, kein Verhalten.
- Volle Suite: nicht im Spurbaum (VERTRAG), faehrt der Orchestrator in der Integration.

### 9.11 Nutzer-Nachtrag 10 (waehrend dieser Nachbesserung in den Baum committet: 3f0ef678, 00:11)
Wortlaut (Ausschnitt, AUFTRAG.md): "Im ROOM 1141 soll alles so bleiben wie bisher, nur eben ohne die "Oh, no, this is John! He is
in Trouble, I have to help him!" Cutscene." Orchestrator: "Spur J: 1141 bleibt, nur OHNE Elzas Reaktion (msg 50 entfaellt in
1141)"; die Zeile, die leisen Rufe in 1041, Karte und Panikmusik baut Spur P "johnruf1041" (Port-Id 40 in 1041, Kopie der
Aufnahme synchro/STAGE1/room1041/main40.wav).
- Messung vorher (Lauf n_a, Stand dccd5f8c): msg 50 offen F176..F356 mit main50.wav, Pad-Sperre F166..F371, (9,94) = 1.
- Umsetzung (11d366a9): HAEMMERN39 ohne den Reaktions-Block (Ifel Ck(9,94)==0, Warten auf Merker (5,0x10) und Pad-Sperre,
  Set(2,7)=1, Plc_neck, msg 50, Sleep 90, Warte-Zeile msg 58, Set(9,94)=1, Plc_neck aus, Set(2,7)=0) - Klopfen und Rufe
  unveraendert; msg 50 wird in 1141 nicht mehr installiert (Text bleibt fuer den Riegel `texte` und zur Weiterverwendung,
  re15_dining_r39_text(0)); Bit 94 wird nicht mehr gesetzt, bleibt in k_port[] reserviert (aeltere Spielstaende); der Merker
  (5,0x10) bleibt (ROLLTOR39 setzt/loescht ihn, ohne Leser). synchro/STAGE1/room1141/main50.wav bleibt unangetastet
  (Nutzer-Datei, in 1141 ungenutzt).
- Messung nachher: Lauf n10_a0 (EXIT 700#1141) - keine Nachricht bis zum Gemaelde (erste Nachricht msg 13 F676), pm = 0 von F1
  bis F676, (9,94) = 0, Rufe help03 F75 / help01 F295 / help04 F515 mit je drei Schlaegen davor (Abstand jetzt 220 Bilder ohne
  die Reaktion); Bilder `j39n2/n10_frueh.png` F100..F600 ohne Textfenster, F680 Gemaelde-Text. Lauf n10_a (echter Weg bis 1171):
  `integration_r39_dining1141_d` per -DAUSWERTEN gruen; msg 51 F3886..F4060 / msg 52 F4062..F4190, Kreuzkorrelation main51
  rho 1,000 (Takt 7531), main52 rho 1,000 (Takt 7707); main50 im Fenster F150..F700: rho 0,063 (nicht da); Kamera Cut 0 ab
  F4333; Punkt 9 Pegel 0 / 559.
- Folge fuer den E-Riegel `integration_r39_gegner_1141`: die Szene endet ohne die Reaktion bei F3438 (Messlauf `j39n2/e_mess2`,
  vorher F3655) - START 3500, EXIT 5240 (gleicher Abstand 65 nach dem Szenenende, gleiche Kampfdauer 1740 Bilder).
- Riegel: `unit_r39_dining1141_programme` (HAEMMERN39 ohne msg 50, ohne Ck/Set(9,94), ohne Pad-Sperre), `_phase_h` (900 Bilder
  ohne Nachricht und ohne Pad-Sperre, (9,94) = 0), `_fenster` (Gemaelde gleich nach dem ersten Ruf: Rolladen, frei, keine msg 50;
  Wiederbetreten ohne msg 50), `integration_r39_dining1141_d` (erste Nachricht in 1141 = msg 13, kein main50.wav, kein
  "Oh, no, this is John!" im Abschnitt 1141).
- Fuer den Nutzer: In ROOM1141 kommt Elzas Satz nicht mehr; beim Betreten klopft John und ruft wie bisher, alles Weitere ist
  unveraendert. Der Satz kommt kuenftig in ROOM1041 (Spur P).

### 9.12 Johns Schuss vor der Gegner-KI (6161ac63)
- Gemessen (integration_r39_gegner_1141, Lauf lauf_eb59fc78 am Stand nach Nachtrag 10): rot an Spur Es Pruefung (c) - state.txt
  10 Flinch-Beginne (Zustand 2 + Clip 7) gegen 9 FLINCH-Eintraege; der zusaetzliche Beginn F3904 Platz 2 = Johns Schuss 2
  (`[dining39] F3904 John schiesst (Schuss 2) auf Gorilla Slot 2: Treffer`), state.txt F3904 `[2 ... st=2 ... mo=7 ...]` (Clip 7
  noch vom Flinch F3869), F3905 `st=1` mit "OHNE ZUCKEN" im gegner_r39.log.
- Ursache: re15_dining_r39_tick (Schuss, Treffer-Schreiber +0x4 = 2) lief NACH re15_enemy_ai_run_all (game_step_common.c); der
  Gorilla stand am Bildende in Zustand 2 und wurde erst im Folgebild von der HURT-Wurzel verarbeitet. Der Spieler-Resolver
  (re15_player_weapon_fire im Spieler-Takt, game_step_common.c vor dem KI-Sammelaufruf) trifft VOR der KI, sein Treffer wird im
  selben Bild verarbeitet (Spuren Es Pruefung (c) beruht darauf: ein Treffer unter der Schwelle zeigt nie Zustand 2 am Bildende).
  Johns Schuss folgt jetzt derselben Reihenfolge wie der Spieler-Treffer im Port.
- Aenderung: der Haken `re15_dining_r39_tick()` steht jetzt unmittelbar vor dem KI-Sammelaufruf (neben re15_fenster1120_tick,
  ebenfalls "vor der KI"); weiterhin 1 Zeile. W-Clip-Halten (Spieler-Takt lief davor) und Johns Nachzielen (der NPC-Sub 2 dreht
  nicht) unveraendert: Unit `phase_h` (b) wieder "groesster Schritt 200, im Kegel nach 5 Bildern".

### 9.13 Endstand nach Nachtrag 10 und Haken-Umzug (Code 6161ac63, Riegel cc2715b8; gezielt, keine volle Suite)
- M1 am Endstand (Laeufe f_l / f_l2 / f_l3, Wegplaene wie 9.1, `salven.py`/`aufmerk.py`): 15 Salven, 30 von 30 Schuessen
  getroffen (8 davon mit Elza im Griff: f_l 2, f_l3 6), 15 von 15 Salven mit "John zugewandt"; Annaeherungen f_l 5878->2797,
  4086->2522, 9268->2525, 8146->2528; f_l2 6576->2793, 9542->2533, 9394->2579, 9387->2531, 9388->2529, 9393->6368 (Laufende);
  f_l3: der Gorilla steht schon bei ~3400 am Tisch/zweiten Gorilla fest, dreht sich zu John und gibt nach 30 Bildern auf
  ("festgeklemmt", 59-84 Bilder je Episode) - die Zuwendung ist sichtbar, das Ende nachvollziehbar. John im Bild (Cut 0): f_l
  Salven 1-2, f_l3 alle 10 Schuesse.
- Alle Unit-Riegel (Lauf 00:34:28..00:53:28): `ctest --test-dir re15_port/build -R "^(unit_|prop_|probe_)" -j 6 --timeout 600`
  -> "100% tests passed, 0 tests failed out of 842".
- Integration (Lauf 00:34:23..01:15:17): `ctest --test-dir re15_port/build -R "integration_r39_dining1141|integration_r38_dining1141|integration_r39_gegner_1141|integration_r37_elzaems_elza1141|integration_r38_affen3" -j 4 --timeout 3600`
  -> "100% tests passed, 0 tests failed out of 14" (integration_r39_gegner_1141 Passed 355.77 sec, integration_r39_dining1141_d
  Passed 417.47 sec, integration_r38_dining1141_s Passed 756.34 sec, r/w/z/l/a/d, affen3 (3), elzaems_elza1141; der Riegel
  `kampf` Passed 2453.21 sec, weil Elza in diesem Lauf bei F5389 starb und die exe im Todesbild bis zur Zeitgrenze stand - die
  Pruefungen liefen auf den Daten bis dahin gruen).
- Danach Riegel `kampf` mit dem Messhaken der Spur E fuer Elzas Energie und EXIT_AT-Pflicht (cc2715b8):
  `ctest --test-dir re15_port/build -R "^integration_r39_dining1141_kampf$" --timeout 1800` -> "100% tests passed, 0 tests failed
  out of 1" (Passed 300.61 sec; "10 Schuesse, 10 Treffer", 6 Treffer bei gr=1, 5 von 5 Treffer-Salven mit Zuwendung, 10 Schuesse
  bei Cut 0, Querungs-Wache 0 Bilder; EXIT_AT Bild 5500 erreicht).

## 10. Nachbesserung 3 (nach Abnahme 2, `J_abnahme_2.md`; Start 2026-10-19 04:33, Uhrzeiten aus `date`)

Auftrag: (M1) Punkt 8 - zielt Elza noch, wenn ENTRIEGELN39 nach dem Tod der Gorillas auf Cut 8 schneidet, richtet sie
11 Bilder lang die Waffe auf John (Lauf R/R2 der Abnahme, state.txt F6296..F6306 pst=1 mo=215 rot 2048, F6307 pst=4).
Abhilfe: Elza vor dem ersten Bild von Cut 8 in einen neutralen Zustand (Original-Form belegen) + Riegel mit zielender
Elza beim Schnitt. Orchestrator: (a) H4 - die J-Integrationsriegel (integration_r39_dining1141_d/_kampf,
integration_r38_dining1141_*) erben keine RE15_*-Variablen des Aufrufers (VERTRAG 2); (b) H7 - Kommentar in
re15_dining_r39_install an den Code; (c) `git merge master` (2963579d, Spur K: RBJ-Kanal-Cache-Fix rbj_resolve_slot)
gegen Js RBJ-Leihe (re15_actor_rbj_anim/_skel) pruefen - Heranwinken in 1141 weiter RBJ ROOM1171 rec1 Clip 2 (96 Bilder);
danach ALLE unit_/prop_/probe_ + integration_r39_dining1141* + integration_r38_dining1141* + integration_r39_gegner_1141 +
integration_r39_heliport1171_weg gruen.

### 10.0 Stand / Fortschritt
- 04:33 Baum sauber auf 1a6be145 (Abnahme 2), master = 2963579d. Abschnitt angelegt.
- 04:35 Merge master 2963579d -> 0e486aa6 (Konflikt nur AUFTRAG.md, HEAD-Seite leer -> Fassung master = Nachtrag 11);
  enemy_common.c / enemy_ai_common.c / game_step_common.c automatisch. configure + build LOCAL-BUILD-OK (exe 04:34).
- 04:40-04:52 Messung vorher am gemergten Stand (Laeufe v_r2 = Abnahme-Lauf R2, v_m = schneller Messhaken-Lauf; 10.1).
- 04:55 RE-Belege (10.2), Umsetzung M1/H4/H7 (10.3) geschrieben, Bau folgt.
- 04:58-05:01 Bau gruen, Unit senken/programme gruen, Commit 8649df54 (M1, H4, H7); 05:01-05:07 Riegel zielen + Lauf n_r2
  (10.4); 05:06-05:16 alle Unit-Riegel 855/855; 05:09-05:14 zielen mit feindlicher Umgebung (H4); 05:16 db51683c
  (Kommentar Kommando 1, R1-Fenster des Riegels); 05:16-05:27 Integration gezielt 12/12 (10.5).

### 10.1 Messung vorher (gemergter Stand 0e486aa6, echte exe; Messordner scratchpad `j39n3/`, nicht committet)
Werkzeug `j39n3/lauf.sh` (= `J39abn2/lauf.sh` der Abnahme, eigene exe-Kopie `re15_pc_j39n3_<name>.exe`), Auszug `zust.py`.
- **Lauf v_r2** (= Abnahme-Lauf R2 bytegleich: Wegplan `plan_r.txt`, P228 RE15_GIVE=0x43:1,0x04:99,0x15:60 RE15_EQUIP=4,
  RE15_R39_MESS_SPIELER_HP=100, RE15_INPUT_SCRIPT = Schuetze `skript_r.txt` ab Bild 3500 (R1 halten + Feuer), echter Tod beider
  Gorillas mit 360 Energie, EXIT 6700#1141), rv=0, 345 s. M1 reproduziert, bildgleich zur Abnahme:
  `[scd F6296] Cut_chg(8)`; state.txt F6287 Schuss (mg 42 -> 41, ac 7 = Rueckstoss-Clip), F6296..F6306 `pst=1 mo=215 ac=7 cam=8 pm=2`
  bei (-5800,-21500) rot 2048 (= Blick auf John) - 11 Bilder Waffe auf John; F6307 `pst=4 mo=105` (Plc_dest TURN).
  Vorher (Spielerbilder F6267..F6295, nach dem Kampf-Ende (4,0xF2)=0 F6267 und vor dem Schnitt): Elza frei, zielt und schiesst
  weiter (Schuesse F6263/F6275/F6287).
- **Ursache (Code gelesen, Messung oben):** der Schnitt-Block von ENTRIEGELN39 setzte Cut_chg 8, Set(2,7)=1, Set(1,0x1B)=1,
  Pos_set + Member_set(4) Elza im selben Takt; unter (1,27)/(2,7) setzt main.c (cine_active) player_mode 2, und re15_player_tick
  ueberspringt im Skript-Modus den ganzen Pad-Zweig samt Ziel-FSM (player_common.c `if (g_scd.player_mode != 2 && !msg_block &&
  !pushing)`): die Waffe blieb im letzten Ziel-/Rueckstoss-Bild stehen, gedreht auf John, bis Plc_dest TURN (+0x0438) das
  Kommandowort ersetzte. Kein Regressionsbefund (seit dem Gespraech in Cut 8, b2a2f267, so; die Abnahmen 0/1 fuhren den Schnitt
  nie mit gehaltenem R1).
- **Lauf v_m2** (schneller Lauf fuer den Riegel, gemergter Stand: Wegplan `plan_r.txt`, P228, Messhaken
  RE15_R38F_GORILLAS_TOT=600 + _STARR=1, RE15_INPUT_SCRIPT `M8` ab Bild 3950 = R1 halten, EXIT 4210#1141), rv=0, 273 s:
  Szenenende F3456, Messhaken F4056, `[scd F4086] Cut_chg(8)`; F4086..F4096 `pst=1 mo=215 ac=8 cam=8 pm=2` (Halten, 11 Bilder),
  F4097 TURN. Werkzeug `test_r39_dining1141 zielen l_v_m2` (Riegel-Auswertung, 10.5): **3 Fehler** (Sperre 0 Bilder vor dem
  Schnitt, 0 Bilder gesenkt, 11 Bilder Cut 8 mit Waffe, Schnittbild mo 215).

### 10.2 RE-Belege (diese Sitzung selbst disassembliert/gelesen; `re15_disasm.py`, info/Re1.5/PSX.EXE)
- **Set(2,7) = Pausen-Wort Bit 0x01000000:** Set-Tabelle `read 0x80074664 4` = [0x800aca38, 0x800aca3c, **0x800aca40**,
  0x800b0ff8] (Zone 2 = g_pauseflags); 0x80000000 >> 7 = 0x01000000 (game_state.c, Set-Handler @0x8003fdd8..@0x8003fe6c).
- **Pad-Umsetzer FUN_80030444 kappt das virtuelle Held-Wort:** `800304f4 lw v0,-13760(v0)` (g_pauseflags), `800304f8 lui
  v1,0x100`, `800304fc and`, `80030500 beq v0,zero,0x80030520`, `8003050c lw v0,-14488(v0)` (0x800ac768), `80030514 andi
  v0,v0,0xf000`, `8003051c sw v0,-14488(at)`. Aufrufer (jal-Suche 0x0C00C111 im ganzen PSX.EXE): 0x80020c88, **0x80029dd8,
  0x80029e14** (Task-Schleife, vor der Spiel-Task) - das Skript setzt das Bit IN der Spiel-Task (SCD-Laeufer `jal 0x8003f038`
  @0x8001cdec vor dem Spieler `jal 0x80031c44` @0x8001ce0c): wirksam ab dem naechsten Bild.
- **Der Spieler-Dispatcher laeuft unter (2,7) weiter:** `80031c54 lw a0,-13760(a0)` (g_pauseflags), `80031c78 bltz
  a0,0x80031da8` - nur Bit 31 (= Set(2,0)) haelt ihn.
- **Die Ziel-FSM liest das gekappte Wort und senkt:**
  - Schusswaffe HOLD: `800331e4 lw v1,-14488(v1)` (0x800ac768), `800331ec andi v0,v1,0x100`, `800331f0 bne v0,zero,
    0x8003320c`, `800331f8 ori v0,zero,0x3`, `80033200 sh v0,-13734(at)` (aca5a = 3 = SENKEN).
  - Messer HOLD: `80035128 lw v1,-14488(v1)`, `80035130 andi v0,v1,0x100`, `8003513c ori v0,zero,0x3`, `80035144 sh
    v0,-13734(at)`.
  - Rueckstoss: `80033560 lw a0,-14488(a0)`, `80033600 andi v0,a0,0x100` / `80033604 bne`, Schwelle Byte 2 der Waffenzeile
    `8003362c addiu at,at,16530` (0x80074092 + (w-1)*5) / `8003363c sltu`, `8003364c sh v0,-13734(at)` (aca5a = 3).
  - Der Port bildet SENKEN bereits ab (player_common.c: HOLD + !R1 @0x80033200 / Messer @0x8003513c, Rueckstoss-Bruch
    @0x8003364c, Clip 6 rueckwaerts, Ausgang -> motion 200), aber im Skript-Modus (player_mode 2) laeuft der ganze Pad-Zweig
    nicht.
- **Original-Form "Szene nach einem Kampf" = ROOM11C0 sub03** (Parkplatz, Leon + Ada, Gorillas Typ 0x27 mit Kill-Flags
  pc[7] 0x60/0x61, sub00 @0x01784/@0x01798; sub01 @0x01836 Ck(7,0x60) / @0x0183E Ck(7,0x61) -> @0x01842 `04 ff 18 03` Evt_exec
  sub03). `xxd -s 0x1B02 -l 0x18 ROOM11C0.RDT` (shared_assets == info/Re1.5, `cmp` bytegleich):
  `22 03 43 01 | 22 01 1b 01 | 22 02 07 01 | 54 00 02 00 00 00 | 09 0a 1e 00 | 29 04` = Set(3,0x43)=1, **Set(1,0x1B)=1
  @0x01B06, Set(2,7)=1 @0x01B0A**, Sce_bgm_control Stop @0x01B0E, **Sleep 30 @0x01B14, Cut_chg 4 @0x01B18**, danach
  Member_set x/z + Plc_dest. Balken und Pad-Sperre stehen also 30 Bilder VOR dem Schnitt; in diesen 30 Bildern senkt die
  Ziel-FSM eine gehobene Waffe. (1141 hat dieselbe Lage: Gorillas 0x27 mit Kill-Flags 0xCB/0xCC, sub01 @0x00BFA/@0x00C02 -> BGM
  Stop @0x00C06 + (4,0xF2)=0 @0x00C0C; der BGM-Stopp kommt dort schon aus sub01.)
- **Laengen (PLD/PL04W03.PLW = Elzas Browning, Verzeichnis dir[0] = EDD):** Clips {22,16,52,1,105,30,**10**,23,1,24,1,24,1,
  **32**}: Clip 6 (Heben/Senken) 10 Bilder, Clip 0x0D (Nachladen) 32 Bilder; PL04W04 gleich, PL04W01 (Messer) Clip 6 = 10.
  Halten -> neutral in 10 Bildern, Rueckstoss -> SENKEN ab Bild 8 (Schwelle 7 fuer Waffe 3) + 10 = 17 < 30.

### 10.3 Umsetzung (Commit 8649df54)
| Teil | Aenderung | Datei / Konstante |
|---|---|---|
| Form | ENTRIEGELN39 nach `Ck(4,0xF2)`/`Ck(5,0x11)`: Set(1,0x1B)=1, Set(2,7)=1, Sleep 30, DANN Cut_chg 8 (vorher Sleep 30 frei, dann Cut_chg 8 + Sperre im selben Takt). Programm gleich lang (1376 B), nur +0x03FC..+0x0409 umgestellt, alle Anker unveraendert | tools/r39_dining/programme_bauen.py (baue_entriegeln39, nach_dem_kampf), gen/dining_r39_programme.inc; Form ROOM11C0 sub03 @0x01B06/@0x01B0A/@0x01B14/@0x01B18 |
| Mechanismus | `re15_dining_r39_ziel_senken()` = ROOM1141 Phase H und die Pad-Sperre stand im letzten Spielschritt (Merker am Anfang von re15_dining_r39_tick, der nach dem Spieler-Takt laeuft -> wirksam ab dem naechsten Bild wie @0x80029dd8/@0x80029e14) | engine/src/dining_r39.c, include/re15_dining_r39.h |
| Haken | re15_player_tick: zielt der Spieler im Kommando 1 (pst 1, Ziel-Phase != NONE), steht kein Text und gilt der Merker, laeuft der Pad-Zweig trotz player_mode 2 mit Pad 0 (= gekapptes Held-Wort: kein Bit 0x100, keine Richtung) -> HOLD/Messer/Rueckstoss -> SENKEN -> neutral. 7 Zeilen (3 Kommentar), Kommentar "Runde 39 Spur J" | engine/src/player_common.c (gemeinsame Datei ausserhalb VERTRAG 1.3, offengelegt; wirkt nur in ROOM1141 Phase H) |
| H4 | `r39j_umgebung_leeren()`: jede RE15_*-Variable aus `cmake -E environment` wird vor dem Spielstart entfernt | tests/integration/r39_dining1141_umgebung.cmake (neu), eingebunden in test_r39_dining1141.cmake, _kampf.cmake, _zielen.cmake, test_r38_dining1141.cmake |
| H7 | Kommentar: s_leihe_ok faellt je Raum (leihe_starten parst neu) | engine/src/dining_r39.c re15_dining_r39_install |

### 10.4 Messung nachher (Stand 8649df54, echte exe)
- **Unit `unit_r39_dining1141_senken`** (Engine-Weg re15_game_step, ROOM1141 Phase H, R1 die ganze Zeit gehalten, Cliplaengen
  aus PL04W03/PL04W01): Pistole im Halten - Setz-Bild noch Phase 2 (Halten), SENKEN ab Bild 1 danach, neutral ab Bild 10
  (motion 200), bleibt unten; Messer im Halten - gleich (SENKEN ab 1, neutral ab 10); Pistole im Rueckstoss (Schuss, Phase
  0x12) - SENKEN ab Bild 7 (Rueckstoss-Bruch, Schwelle 7), neutral ab 16; Gegenprobe ROOM1141 ohne Phase H - Waffe bleibt oben
  (Geltungsbereich). `senken: 0 Fehler`.
- **Riegel-Lauf `integration_r39_dining1141_zielen`** (lauf_a06aff7b, Passed 254.55 sec; Werkzeug `zielen`): Messhaken F4052,
  Pad-Sperre ab F4052 (pm 2), F4052 noch Halten (ac 8, Umsetzer-Bild), F4053..F4061 SENKEN (mo 215 ac 6, 9 Bilder), ab F4062
  neutral (mo 200), `[scd F4082] Cut_chg(8)` = 30 Bilder nach der Sperre, 40 Bilder Cut 8 ohne erhobene Waffe, Schnittbild
  pst 1 mo 200, F4094 TURN (pst 4). Vorher (v_m2, gleiche Lage): 11 Bilder Cut 8 mit Waffe.
- Bilder (committet): `J_belege/nb3_m1_vorher_v_m2_F4052-4100.png` (vorher: F4052..F4085 zielt Elza ohne Balken, F4087..F4096
  in Cut 8 der ausgestreckte Arm mit der Pistole auf John) und `J_belege/nb3_m1_nachher_zielen_F4048-4096.png` (nachher: F4048/
  F4052 zielt sie, ab F4054 Balken und Senken, ab F4062 steht sie neutral, F4086/F4096 Cut 8 Elza und John einander zugewandt,
  Arme unten).
- **Lauf n_r2** (= Abnahme-Lauf R2 / v_r2 bytegleich: echter Tod beider Gorillas mit 360 Energie, Schuetze haelt R1 und feuert;
  Stand 8649df54), rv=0, 337 s: Schuss F6263 (mg 43 -> 42, Rueckstoss ac 7), **Pad-Sperre ab F6266**, Rueckstoss laeuft bis
  zum Bruch, **F6271..F6279 SENKEN** (ac 6), **ab F6280 neutral** (mo 200); die Abzuege F6275..F6277 und F6287..F6289 (pad 8800)
  feuern nicht mehr (mg bleibt 42; Original: gekapptes Wort, kein Bit 0x100 im HOLD). `[scd F6296] Cut_chg(8)` (30 Bilder nach
  der Sperre, gleiches Bild wie vorher), **0 Bilder in Cut 8 mit mo 215** (vorher F6296..F6306 = 11), F6307 TURN wie vorher.
  Heranwinken unveraendert: `F6504 John winkt heran: RBJ ROOM1171 Record 1 Clip 2 geliehen (96 Bilder, 3 Clips)` (nach dem
  Merge mit Ks rbj_resolve_slot). Bilder `J_belege/nb3_m1_vorher_r2_F6280-6308.png` (vorher: F6297..F6306 Arm mit Pistole auf
  John) und `J_belege/nb3_m1_nachher_r2_F6264-6308.png` (nachher: F6268 Balken, F6271..F6280 Senken, F6297..F6308 Elza neutral
  John zugewandt).

### 10.5 Tests (gezielt, keine volle Suite im Spurbaum)
- Neu: `unit_r39_dining1141_senken` (Teil senken, 10.4), `integration_r39_dining1141_zielen` (tests/integration/
  test_r39_dining1141_zielen.cmake, registriert in probes/r39_dining1141.cmake; Werkzeug `test_r39_dining1141 zielen <lauf>`:
  Kampfende/Schnitt aus debug.log, Zustand je Bild aus state.txt (Abschnitt ab der letzten Zeile "F1 ")). Erweitert:
  `unit_r39_dining1141_programme` (NB3: `22 01 1b 01 22 02 07 01 09 0a 1e 00 29 08` hinter beiden Wartezeilen, keine zweite Sperre
  vor msg 51).
- Gegenprobe: `test_r39_dining1141 zielen l_v_m2` (Lauf am Stand vor dem Fix) -> **3 Fehler** (Sperre 0 Bilder vor dem Schnitt,
  0 Bilder gesenkt, 11 Bilder Cut 8 mit Waffe); `senken` Fall 4 (ohne Phase H) zeigt den alten Port-Stand (Waffe bleibt oben).
- H4 am Lauf: `env RE15_EXIT_AT=200#1131 RE15_FRAMEDUMP=0-50/1:feind_ RE15_GIVE=0x02:1 RE15_R38F_GORILLAS_TOT=5
  RE15_INPUT_SCRIPT=U30 ctest -R "^integration_r39_dining1141_zielen$"` -> `r39j: 5 RE15_*-Variablen des Aufrufers entfernt:
  RE15_EXIT_AT;RE15_FRAMEDUMP;RE15_GIVE;RE15_INPUT_SCRIPT;RE15_R38F_GORILLAS_TOT`, Lauf lauf_2e593b55 bis 4210#1141 (kein Abbruch
  in 1131, kein `feind_`-Bild), `zielen: 0 Fehler`, "100% tests passed, 0 tests failed out of 1". Rauchprobe D/kampf-Skript
  (RE15_DUMMY_X=1): "r39j: 1 RE15_*-Variablen des Aufrufers entfernt: RE15_DUMMY_X".
- Messkopien/Laufordner: zielen nutzt `re15_pc_r39j_z_<id>` und `lauf_<id>` (eigene Praefixe, nur eigene alte Kopien geloescht),
  `cmake_minimum_required(VERSION 3.16)`, keine .exe-/Backslash-Pfade (Endung aus `get_filename_component(... EXT)`).
- Alle Unit-Riegel am Stand 1264c240 (Code = 8649df54; Lauf 05:06:02..05:15:35):
  `ctest --test-dir re15_port/build -R "^(unit_|prop_|probe_)" -j 6 --timeout 600`
  -> **"100% tests passed, 0 tests failed out of 855"** (572.19 sec). Danach nur ein Kommentar in player_common.c (db51683c):
  `objdump -d player_common.c.obj` vor/nach bytegleich ("CODE_GLEICH", 2974 Zeilen).
- Hinweis zum Riegel zielen: ohne RE15_AUDIO_CAP_SYNC laufen die Stimmen-Zeilen nach der Wanduhr - das Szenenende schwankte
  in drei Laeufen 3421..3456 (Kampfende 4021..4056). Das R1-Fenster (Bild 3000 + 45 s) deckt Szenenenden ~2420..~3690 ab.
- Integration gezielt am Endstand db51683c (Lauf 05:16:08..05:27:14):
  `ctest --test-dir re15_port/build -R "^(integration_r39_dining1141_d|integration_r39_dining1141_kampf|integration_r39_dining1141_zielen|integration_r38_dining1141_[wzdlsra]|integration_r39_gegner_1141|integration_r39_heliport1171_weg)$" -j 4 --timeout 3600 --output-on-failure`
  -> **"100% tests passed, 0 tests failed out of 12"** (r38 r 191.97 s, r39_gegner_1141 299.10 s, r39_dining1141_d 354.16 s,
  r38 w 188.43 s, r38 z 117.20 s, r38 l 45.86 s, r38 a 0.17 s und r38 d 0.16 s (beide seit Nachbesserung 1 durch die
  r39-Riegel ersetzt: "ersetzt durch unit_r39_dining1141_phase_h / integration_r39_dining1141_d"), r39_dining1141_zielen 269.68 s,
  r39_heliport1171_weg 59.23 s, r39_dining1141_kampf 308.38 s, r38 s 665.97 s).
  - D (lauf_0b122b1a, ohne R1): Messhaken F3833, Sperre ab F3833 (pm 2), `[scd F3863] Cut_chg(8)` = 30 Bilder danach; nach dem Merge mit Spur K
    `F4061 John winkt heran: RBJ ROOM1171 Record 1 Clip 2 geliehen (96 Bilder, 3 Clips)` (Pruefung `pruef(_r "John winkt heran:
    RBJ ROOM1171 Record 1 Clip 2 geliehen (96 Bilder")` gruen); Unit phase_h (`wink`: re15_actor_rbj_anim(John) Clip 2 = 96 Bilder)
    in 855/855.
  - zielen (lauf_8042d2e3, R1-Fenster 3000..4350): Kampfende F4051, Sperre ab F4051, letztes Zielbild F4060 (9 Bilder SENKEN),
    Schnitt F4081, 40 Bilder Cut 8, 0 mit Waffe, Schnittbild pst 1 mo 200 - `zielen: 0 Fehler`.

### 10.6 Je Mangel / Auftrag: Ursache - Messung vorher - Beleg - Aenderung - Messung nachher
| Punkt | Ursache (gemessen) | vorher | Beleg | Aenderung | nachher |
|---|---|---|---|---|---|
| M1 (Punkt 8) | ENTRIEGELN39 setzte Cut_chg 8, Pad-Sperre und Pos_set/Member_set(4) Elzas im selben Takt; der Port haelt im Skript-Modus (player_mode 2) die ganze Ziel-FSM an, die Waffe blieb oben und zeigte nach Member_set auf John | v_r2 F6296..F6306, v_m2 F4086..F4096: je 11 Bilder `pst=1 mo=215 cam=8` | ROOM11C0 sub03 @0x01B06/@0x01B0A/@0x01B14/@0x01B18; FUN_80030444 @0x800304f4..@0x8003051c; Dispatcher `bltz` @0x80031c78; HOLD @0x800331e4/@0x800331ec/@0x80033200, Messer @0x80035128/@0x80035130/@0x80035144, Rueckstoss @0x80033560/@0x80033600/@0x8003364c; Umsetzer im Task-Takt @0x80029dd8/@0x80029e14 | Form: Balken + Sperre + Sleep 30 VOR dem Schnitt; Mechanismus: Ziel-FSM unter der Sperre mit Pad 0 (ROOM1141 Phase H, ab dem Bild nach dem Setzen) | zielen: Sperre 30 Bilder vor dem Schnitt, 9 Bilder SENKEN, 0 Bilder Cut 8 mit Waffe, Schnittbild neutral; n_r2 (echter Tod): 0 statt 11 |
| H4 | die J-Integrationsriegel setzten ihre Umgebung nur ueber re15_start_spiel; fremde RE15_*-Variablen des Aufrufers liefen mit in die exe | Abnahme 2 H4 (Code gelesen) | VERTRAG Abschnitt 2 | `r39j_umgebung_leeren()` (alle RE15_* aus `cmake -E environment`) in allen vier J-Skripten | 10.5 (Lauf mit feindlicher Umgebung) |
| H7 | Kommentar "s_leihe_ok bleibt" gegen `s_leihe_ok = 0` | Abnahme 2 H7 | - | Kommentar an den Code | - |
| Merge | master 2963579d (Spur K): rbj_resolve_slot belegt fuer Slots ohne Record keinen Cache-Platz | - | Js Leihe steht in re15_actor_rbj_anim/_skel VOR rbj_resolve_slot (enemy_common.c), wirkt also unabhaengig vom Cache | Merge 0e486aa6 | n_r2 F6504 "RBJ ROOM1171 Record 1 Clip 2 geliehen (96 Bilder, 3 Clips)"; Unit phase_h prueft re15_actor_rbj_anim(John) Clip 2 = 96 Bilder (10.5) |

### 10.7 OFFEN (Nachbesserung 3)
- **Geltungsbereich der Ziel-FSM unter der Sperre:** der Port haelt in ALLEN anderen Raeumen einen zielenden Spieler unter
  (2,7) weiter an (player_common.c Pad-Zweig nur bei player_mode != 2); das Original senkt ueberall nach demselben Mechanismus
  (FUN_80030444 + HOLD, 10.2) - z. B. Leon nach dem Tod der Parkplatz-Gorillas (ROOM11C0 sub03). Diese Nachbesserung beschraenkt
  sich auf ROOM1141 Phase H (Riegel `senken` Fall 4 haelt den Geltungsbereich fest). Fuer eine spaetere Runde: den Haken ohne die
  Raum-Bedingung (re15_dining_r39_ziel_senken) allgemein machen - braucht die volle Suite (alle Szenen, die nach einem Kampf
  beginnen).
- **Randfall Nachladen/Hieb im letzten Augenblick:** Nachladen (Clip 0x0D, 32 Bilder) und Messerhieb (Clip 7, 25 Bilder) testen
  R1 nicht und spielen erst aus, dann senkt die FSM (10 Bilder). Beginnt eines davon in den letzten ~12 (Nachladen) bzw. ~5 (Hieb)
  Bildern vor der Sperre, ist zu Beginn von Cut 8 noch das Ende des Senkens zu sehen (bis Plc_dest TURN 11 Bilder nach dem
  Schnitt). Das ist die Original-Form (ROOM11C0 sub03 wartet ebenfalls fest 30 Bilder); ein Warten auf "Spieler neutral" gibt es
  im Skript-Satz des Originals nicht. Gemessen nur der Halten-, Messer- und Rueckstoss-Fall.
- **Ein Bild Unterschied am Sperr-Beginn:** im Setz-Bild laeuft im Original der Dispatcher noch mit ungekapptem Wort (Drehen im
  Halten moeglich), der Port haelt den Pad-Zweig ab demselben Bild an (vorbestehende Port-Regel player_mode 2); das Senken beginnt
  in beiden im Bild danach.
- Abnahme 2 H5 (kein Auftrag dieser Nachbesserung, unveraendert): re15_dining_r39_tick laeuft ausserhalb des KI-Tors
  (game_step_common.c, vor `!(g_re15_pauseflags & RE15_PAUSE_AI)`); unter einer die Welt einfrierenden Nachricht laufen Johns
  Nachfuehren, ein angestossener Schuss und die Aufmerksamkeits-Zaehler weiter. Der neue Sperr-Merker gehoert dagegen bewusst
  VOR jedes Tor (er bildet den Pad-Umsetzer nach, der im Original im Task-Takt immer laeuft).
- Weiter offen aus 9.7: gemeinsame Dateien ausserhalb VERTRAG 1.3 - jetzt zusaetzlich engine/src/player_common.c (+7, nur
  ROOM1141 Phase H); E-Riegel (unit_r39_gegner_1141_ablauf1141, integration_r39_gegner_1141) in der Fassung dieses Zweigs
  uebernehmen (H3); Spieler-Plc_motion(2,c) als W-Bank-Clip nur in ROOM1141.

### 10.8 Fuer den Nutzer (Nachbesserung 3)
- **Elza zielt noch, wenn die Gorillas sterben:** Die Szene beginnt jetzt wie im Original nach einem Kampf (dieselbe Form wie bei
  Leon und Ada auf dem Parkplatz): zuerst kommen die schwarzen Balken und Elza kann sich nicht mehr bewegen; haelt man die
  Zieltaste noch, senkt sie dabei ihre Waffe. Eine Sekunde spaeter schneidet die Kamera auf Elza und John - Elza steht ruhig da und
  richtet die Waffe nicht mehr auf John. Wer die Taste nicht haelt, merkt nur, dass die Balken etwas frueher kommen.
- Keine neuen Dateien unter shared_assets/, keine Sprachdateien, keine Entscheidungsfrage. Neue Belegbilder unter
  analysis/befunde_runde39/J_belege/.

## 11. Nachbesserung 4 (nach Abnahme 3, `J_abnahme_3.md`; Start 2026-10-19, Uhrzeiten aus `date`)

Auftrag: (M1-Rest, Punkt 8) Beginnt Elza kurz vor der Pad-Sperre nachzuladen, zielt sie zu Beginn des Gespraechs noch auf
John (Abnahme 3 Lauf RL2: Nachladen ab F4053, Sperre F4056, Schnitt F4086; F4086..F4093 = 8 Bilder Cut 8 mit mo=215 ac=6,
fd_004087/fd_004088 Pistole auf Johns Oberkoerper; Ende erst durch Plc_dest TURN F4097). Abhilfe-Richtung der Abnahme:
Elzas Kommandowort im Schnittbild ersetzen wie ROOM11C0 sub03 @0x01B1A..@0x01B26 (Plc-Befehl fuer Elza im selben Takt wie
Cut_chg 8 / Pos_set / Member_set) und den Riegel zielen um den Fall "Nachladen kurz vor der Sperre" erweitern (kein Bild in
Cut 8 mit mo 215). Alles, was die Abnahme 3 als erfuellt bewertet hat, bleibt (Riegel gruen).

### 11.0 Stand / Fortschritt
- 08:16 Baum sauber auf ece3a8f9 (Abnahme 3), Abschnitt angelegt. Messordner scratchpad `J39n4/` (nicht committet).
- 08:17-08:22 Messung vorher (Lauf v_rl2 = Abnahme-Lauf RL2 bytegleich, 11.1); waehrenddessen RE-Belege selbst disassembliert (11.2), Commit 0a7d8264 08:23.

### 11.1 Messung vorher (Stand ece3a8f9 = Code e9f23f50, exe 05:15; Messordner scratchpad `J39n4/`, nicht committet)
Werkzeug `J39n4/lauf.sh` (= `J39abn3/lauf.sh`, eigene exe-Kopie `re15_pc_j39n4_<name>.exe`, mit RE15_AUDIO_CAP_SYNC).
- **Lauf v_rl2** (= Abnahme-3-Lauf RL2: `plan_r.txt`, RE15_GIVE=0x43:1,0x03:1,0x15:50 RE15_EQUIP=3 (Browning, 1 Patrone),
  RE15_R38F_GORILLAS_TOT=600 + _STARR=1, RE15_INPUT_SCRIPT=`M33.3333,MA0.1,M1.6667,MA0.1,M10` mit BASIS=spiel ab Bild 3000,
  EXIT 4250#1141, FD 4040-4110/1), rv=0, 306 s. Befund bildgleich zur Abnahme:
  `[dining] F4056 MESSHAKEN`, `[scd F4086] Cut_chg(8)`; state.txt F4053..F4083 `mo=215 ac=13` (Nachladen, Abzug mit leerem Magazin
  pad 8800 F4053), **F4056 pm=2** (Sperre, Nachladen laeuft weiter), F4084 `ac=8 mg=15` (Auffuellen, Halten), F4085..F4093
  `mo=215 ac=6` (SENKEN), **F4086..F4093 = 8 Bilder cam=8 mit mo=215**, F4094 `mo=200`, **F4097 `pst=4 mo=105`** (Elzas
  Plc_dest TURN, 11 Bilder nach dem Schnitt), F4099 `mo=1` (Sub 6). Bild `J_belege/nb4_m1rest_vorher_rl2_F4086-4098.png`
  (F4086 zeigt noch Cut 5; **F4087/F4088 Arme ausgestreckt, Pistole auf Johns Oberkoerper**, F4089/F4090 Waffe sinkt, ab F4092 unten).
- **Ursache (gemessen + Programm gelesen):** ENTRIEGELN39 gab Elza im Schnitt-Takt (+0x0408 Cut_chg 8 .. +0x0422 Member_set(4))
  keinen Plc-Befehl; ihr erster (Plc_dest TURN, Form ROOM1141 sub02 @0x00C50) stand hinter Sleep 1 + Plc_neck + Sleep 10
  (+0x0426..+0x0438). Bis dahin blieb sie im Kommando 1, die Ziel-FSM spielte Nachladen (31 Bilder) und Senken (10) zu Ende - das
  passt nicht in die 30 Bilder zwischen Sperre und Schnitt (NB3 deckt nur Halten 9-10 / Rueckstoss bis 16 Bilder). Kein
  Regressionsbefund: dieselbe Lage seit dem Gespraech in Cut 8; die Form "Sperre vor dem Schnitt" der Nachbesserung 3 verkuerzte
  ihn nur.

### 11.2 RE-Belege (diese Sitzung selbst disassembliert; `re15_disasm.py` auf info/Re1.5/PSX.EXE, `scd_dump_room.py`)
- **Original-Form der Szene nach einem Kampf, ROOM11C0 sub03** (`scd_dump_room.py ROOM11C0.RDT`, sub03 [0x1B02..0x1C0C]):
  `0x01B14 09 0a 1e 00` Sleep 30, `0x01B18 29 04` Cut_chg 4, **`0x01B1A 2e 01 00` Work_set(1,0)** + Nop, `0x01B1E 34 00 9d e0`
  Member_set(0,-8035), `0x01B22 34 02 ff c1` Member_set(2,-15873), **`0x01B26 40 00 04 00 38 c8 a1 de` Plc_dest** (Modus 4, Ziel
  (-14280,-8543), Bit 0), `0x01B2E 18 05` Gosub sub05 (= `22 05 00 00` / While Ck(5,0)==0, wartet auf die Ankunft), `0x01B30 40 00
  09 00 ..` Plc_dest TURN. Kein Sleep zwischen Cut_chg und Plc_dest: der Spieler bekommt seinen Plc-Befehl im SCHNITT-TAKT.
- **Opcode-Tabelle** 0x800744a8 (die einzige Fundstelle des Worts 0x80041be4 im PSX.EXE ist 0x800745a8 = 0x800744a8 + 0x40*4):
  [0x2E] = 0x80040d2c (Work_set), [0x3F] = 0x80041b90 (Plc_motion), [0x40] = 0x80041be4 (Plc_dest). SCD-Laeufer 0x8003f038 ->
  0x8003f0a0: `8003f0b4 addiu s4,s4,17576` (= 0x800744a8), `8003f10c jalr v0`.
- **Work_set Art 1 = Spieler 0x800aca54:** `80040da4 lui v0,0x800b` / `80040da8 addiu v0,v0,-13740` / `80040db0 sw v0,340(a0)`
  (Thread+0x154 = Arbeitsfigur). Damit +4 = 0x800aca58 (Kommandowort), +5 = 0x800aca59, +6 = 0x800aca5a, +7 = 0x800aca5b.
- **Plc_dest-Handler 0x80041be4 schreibt das Kommandowort sofort:** `80041be4 lw a1,340(a0)`, `80041bec lhu v0,452(a1)`, `80041bf0
  lbu a2,2(v1)` (Modus), `80041bf8 andi v0,v0,0x4` / `80041bfc beq v0,zero,0x80041c14` (Wiederholungs-Waechter: nur bei +0x1c4 Bit 4
  und gleichem Modus uebersprungen), **`80041c14 sb v0,4(a1)` (v0 = 4 aus `80041c00 ori v0,zero,0x4`), `80041c18 sb a2,5(a1)`,
  `80041c1c sb zero,6(a1)`, `80041c20 sb zero,7(a1)`**; fuer Spieler-Typen (< 0x10, `80041c50 sltiu v0,v0,0x10`) Modus 4..9 ueber
  Tabelle 0x80010dcc ([5] = 0x80041d1c fuer Modus 9: +0x1c8 aus 0x80073ee5[Typ*2]).
- **0x800aca5a ist die Phase der Ziel-FSM:** Pistolen-FSM 0x80032e9c `80032ea0 lbu v0,-13734(v0)` (0x800aca5a), Tabelle 0x800740f4:
  [0] 0x80032f18, [1] 0x80033180 (HOLD), [2] 0x80033460 (Rueckstoss), **[3] 0x80033c74 (SENKEN), [4] 0x80033d7c (NACHLADEN)**,
  [5] 0x80033eec. Nachladen: Init bei +7 = 0 (`80033d90 lbu v0,0(v1)` 0x800aca5b, `80033da4 ori v0,zero,0xd` / `80033dac sb
  0x800acae8` Clip 0x0D), Ende erst am Clip-Ende (`80033ea8 jal 0x8001f314` anim_set, `80033eb0 beq v0,zero,..`, `80033ebc sh
  v0(=1),-13734(at)` -> HOLD, `80033ec0 jal 0x8004ebdc`, `80033ed0 jal 0x80045024` Ton 0x01030001). Plc_dest loescht +6/+7: ein
  laufendes Nachladen oder Senken ist danach weg.
- **SCD vor dem Spieler im selben Bild:** Spiel-Task `8001cdec jal 0x8003f038` (SCD-Laeufer) vor `8001ce0c jal 0x80031c44`
  (Spieler). Der Spieler-Verteiler haelt nur bei Bit 31 (`80031c54 lw a0,-13760(a0)` g_pauseflags, `80031c78 bltz a0,..`), liest
  dann `80031c8c lbu v1,-13736(v1)` (0x800aca58) und springt ueber 0x80073f90 (`80031ca4 addiu at,at,16272`, `80031cb4 jalr v0`).
  Kommando 4 = 0x80030660 (Plc-Executor): `8003068c lbu v0,-13735(v0)` (0x800aca59), Tabelle 0x80073e30 (`8003069c`), `800306ac
  jalr v0`; [9] = **0x80031360 TURN**: `80031374 lbu v1,0(a0)` (0x800aca5a = 0 nach Plc_dest) -> Init `80031398 sb 1,aca5a`,
  `800313a4 sb 5,0x800acae8` (Clip 5), `800313b0 sb 0,0x800acae9`, `800313b8 sb 7,0x800acae3`; Ankunftstest `800313d0 jal
  0x8001ab9c` (Kegel 0x60), Ankunft `800313ec sb 6,0x800aca59` / `800313f4 sb 0,0x800aca5a`; `80031494 jal 0x8001f314` anim_set
  auf +0x170/+0x174 (0x800acbc4/0x800acbc8). **Im Schnittbild selbst laeuft also schon der TURN-Clip, kein Waffen-Clip.**
- Port: `op_plc_dest` (scd_vm.c) bildet das ab - Spieler-Walk-Modus: `re15_player_aim_interrupt()` (Phase NONE = +6 = 0) und
  `a->state = 4` (@0x80041c14); main.c ruft scd_vm_tick vor re15_game_step (wie @0x8001cdec vor @0x8001ce0c). Gemessen in v_rl2:
  der TURN F4097 beendet den Waffen-Clip im selben Bild (`pst=4 mo=105`).
- **Folgerung:** Elzas erster Plc-Befehl gehoert in den Schnitt-Takt (Form 11C0 sub03 @0x01B1A..@0x01B26). In ENTRIEGELN39 ist
  das der TURN zu John (Form 1141 sub02 @0x00C50, Bit 0); er rueckt von +0x0438 nach +0x0426 (direkt hinter Member_set(4)),
  Sleep 1 / Plc_neck / Sleep 10 folgen unveraendert. Laenge 1376 B und alle Anker bleiben; Johns Befehle liegen alle hinter dem
  alten Platz (Sleep 5 @+0x0440 beginnt im selben Takt wie vorher) - sein Ablauf ist bildgleich. Elza steht durch Member_set(4)
  schon zu John gedreht: der TURN kommt im ersten Bild an (Kegel 0x60) und geht wie bisher in Sub 6 ueber, nur 11 Bilder frueher.
- Messer/Hieb: dieselbe Phase +6 (Messer-FSM, Hieb Clip 7); der Handler unterscheidet keine Waffenart - ein Hieb-Rest endet ebenso.
- 08:24 Generator + Bau gruen, 08:24-08:28 Lauf n_rl2 (11.4), 08:27-08:33 Riegel (Unit schnitt/programme, zielen nachladen), Bau 08:32, Unit schnitt gruen 08:33, wip 82320b2a 08:33 (Uhrzeiten aus Dateizeiten/git log).

### 11.3 Umsetzung (Commit 82320b2a)
| Teil | Aenderung | Datei / Beleg |
|---|---|---|
| Programm | ENTRIEGELN39: Elzas `40 00 09 00 82 e2 04 ac` Plc_dest TURN -> John (Form 1141 sub02 @0x00C50) von +0x0438 nach +0x0426, direkt hinter `2e 01 00 00` Work_set(1,0) (+0x0416), Pos_set (+0x041A) und `34 04 00 08` Member_set(4) (+0x0422), also im Takt von `29 08` Cut_chg 8 (+0x0408); dahinter unveraendert Sleep 1, Plc_neck, Sleep 10, Sleep 5. Laenge 1376 B, Anker WINK 0x048C unveraendert | tools/r39_dining/programme_bauen.py (nach_dem_kampf), gen/dining_r39_programme.inc; Form ROOM11C0 sub03 @0x01B18/@0x01B1A/@0x01B1E/@0x01B22/@0x01B26 |
| Engine | keine Aenderung: op_plc_dest bildet @0x80041c14..@0x80041c20 schon ab (re15_player_aim_interrupt + state 4), main.c ruft scd_vm_tick vor re15_game_step (@0x8001cdec vor @0x8001ce0c) | engine/src/scd_vm.c (unveraendert) |
| Riegel Unit | neuer Teil `schnitt` (siehe 11.5) und in `programme` die Pruefung "TURN im Schnitt-Takt, kein zweiter bis msg 51" | tests/unit/test_r39_dining1141.c, probes/r39_dining1141.cmake |
| Riegel Integration | Werkzeug `zielen`: Schnittbild = Kommando 4 (vorher pst 1 mo 200); Fall `nachladen` (Vorbedingung: Nachladen im Sperr-Bild, 1..11 Bilder vorher begonnen); neuer Riegel `integration_r39_dining1141_zielen_nachladen` = Lauf RL2 (eigene exe-Kopie `re15_pc_r39j_zn_<id>`, eigener Laufordner, RE15_AUDIO_CAP_SYNC fuer ein bildgenaues Szenenende, cap.raw danach geloescht) | tests/integration/test_r39_dining1141_zielen.cmake (FALL=nachladen) |
Keine neue Konstante: die Umstellung verschiebt eine vorhandene Original-Zeile (Belegstellen im Listing der .inc).

### 11.4 Messung nachher (Stand 82320b2a, echte exe)
- **Lauf n_rl2** (= v_rl2 bytegleich, neue exe), rv=0, 256 s: bis F4085 bildgleich (Nachladen F4053..F4083, Sperre F4056, F4084
  `ac=8 mg=15`, F4085 `ac=6` SENKEN); `[scd F4086] Cut_chg(8)` und **im selben Takt** `[scd] Plc_dest(slot=0 mode=0x09
  dest=(-7550,-21500) flag=0) walk_active=1`; state.txt **F4086 `pst=4 mo=105` cam=8**, F4087 `pst=4 mo=105`, ab F4088 `mo=1`
  (Sub 6 Clip 1). **Cut 8 (F4086..F4125): 0 Bilder mit mo 215** (vorher F4086..F4093 = 8). Elzas Blick rot 2047 (vorher nach
  dem TURN F4097 ebenso 2047).
- Bild `J_belege/nb4_m1rest_nachher_rl2_F4086-4098.png` (Ausschnitt) und `..._bogen_F4080-4105.png` (ganze Bilder): F4086 zeigt
  noch Cut 5 (die Kamera wird ein Bild spaeter sichtbar, wie Cut 7 in 1.2 der Abnahme 3). F4087/F4088: der Waffenarm sinkt
  noch aus der Halte-Lage (Hand auf Hoehe von Johns Guertel, vorher F4087/F4088 beide Arme auf Brusthoehe), F4089 tiefer,
  F4090 an der Huefte, ab F4092 unten. **Das ist die Ueberblendung des Originals**, kein Waffen-Clip: der TURN-Init setzt
  +0x8f = 7 (`800313b8 sb v0(=7),-13597(at)` = 0x800acae3), Sub 6 beim Uebergang erneut (`80051874 sb v0(=7),143(v1)`), beide
  mit Schrittweite `ori a3,zero,0x200` (@0x80031498 / @0x80051890); FUN_8001f3bc liest +0x8f (`8001f408 lbu s0,143(v1)`,
  `8001f41c bne a3,zero,..` = Ueberblend-Zweig), rechnet `8001f474 mult v0,a3` (Schrittweite x frac), `8001f4a8 ori v0,zero,
  0x1000` / `8001f4b0 subu v0,v0,v1` (Gewicht des neuen Clips = 0x1000 - 0x200*frac) und mischt per GTE die zuletzt gezeigte
  Lage (Entity+0x188 -> +56/+58/+60) ein: Bild 1 nach dem Wechsel 87,5 % alte Lage, dann je Bild 12,5 % weniger. Weder
  Cut_chg (0x800402a0: setzt nur das Kamera-Bit 0x100 in 0x800aca3c und ruft 0x800142f4) noch Pos_set (0x80041048: nur +0x34/
  +0x38/+0x3C) setzen +0x8f zurueck - im Original endet ein Nachlade-/Senken-Rest in der Form 11C0 sub03 also ebenfalls mit
  dieser Ueberblendung in den ersten Bildern nach dem Schnitt, nicht mit einem harten Wechsel. Der Port war hier schon bytegleich
  (re15_actor.h anim_frac/anim_blend_rate, "byte-true FUN_8001f3bc").

### 11.6 Je Mangel: Ursache - Messung vorher - Beleg - Aenderung - Messung nachher
| Mangel | Ursache (gemessen) | vorher | Beleg | Aenderung | nachher |
|---|---|---|---|---|---|
| M1-Rest (Punkt 8): Nachladen kurz vor der Sperre -> Elza zielt zu Beginn von Cut 8 auf John | ENTRIEGELN39 gab Elza im Schnitt-Takt keinen Plc-Befehl; ihr erster (TURN zu John, Form 1141 sub02 @0x00C50) kam 11 Bilder nach dem Schnitt. Bis dahin blieb sie im Kommando 1, die Ziel-FSM spielte Nachladen (31 Bilder) + Senken (10) aus - laenger als die 30 Bilder zwischen Sperre und Schnitt | v_rl2 (= RL2): F4086..F4093 8 Bilder cam 8 mit mo 215, F4087/F4088 Pistole auf Johns Oberkoerper (`nb4_m1rest_vorher_rl2_F4086-4098.png`) | ROOM11C0 sub03 @0x01B18 Cut_chg / @0x01B1A Work_set(1,0) / @0x01B1E/@0x01B22 Member_set / @0x01B26 Plc_dest im selben Takt; Plc_dest 0x80041be4: +4 = 4 @0x80041c14, +5 @0x80041c18, +6/+7 = 0 @0x80041c1c/@0x80041c20; Work_set Art 1 = 0x800aca54 @0x80040da4..@0x80040db0; Ziel-FSM 0x80032e9c liest 0x800aca5a (@0x80032ea0), Tabelle 0x800740f4 [3] SENKEN 0x80033c74 / [4] NACHLADEN 0x80033d7c; SCD-Laeufer @0x8001cdec vor Spieler @0x8001ce0c; Kommando 4 -> 0x80030660 -> 0x80073e30[9] TURN 0x80031360 | Elzas TURN von +0x0438 nach +0x0426 (Schnitt-Takt, direkt hinter Member_set(4)); Laenge/Anker unveraendert; Riegel: Unit `schnitt` (mit Gegenprobe), `programme` (Lage), Integration `zielen_nachladen` (= RL2), `zielen` (Schnittbild Kommando 4) | n_rl2: F4086 `pst=4 mo=105` im Schnittbild, 0 Bilder Cut 8 mit mo 215; Unit `schnitt`: Nachladen 3/8/1 Bilder vorher, Hieb, Messer ziehen, Halten je 0 Bilder mit erhobener Waffe (Gegenprobe Fassung NB3: 8/3/10/1/0/0 - Messer ziehen passt mit 15 + 10 Bildern auch dort in die Sperre) |
| Dossier 10.7 "Randfall nach der Original-Form ... nur das Ende des Senkens" | Begruendung falsch (Abnahme 3, 1.8): die Original-Form wechselt das Kommandowort im Schnittbild | - | wie oben | 10.7-Punkt durch 11.7 ersetzt | - |

### 11.7 OFFEN (Nachbesserung 4)
- **Ueberblendung nach dem Schnitt (Original-Mechanismus, kein Waffen-Clip):** Bei einem Nachlade-/Senken-/Hieb-Rest im
  Schnittbild beginnt der TURN mit der Ueberblendung FUN_8001f3bc (+0x8f = 7 @0x800313b8, beim Uebergang in Sub 6 erneut
  @0x80051874; Gewicht des neuen Clips 0x1000 - 0x200*frac @0x8001f474/@0x8001f4a8/@0x8001f4b0). Sichtbar: in n_rl2 sinkt der
  Waffenarm in F4087/F4088 noch aus der Halte-Lage (Hand etwa auf Hoehe von Johns Guertel), F4090 an der Huefte, ab F4092
  unten. Dieselbe Ueberblendung hat die Original-Form ROOM11C0 sub03 (Cut_chg 0x800402a0 und Pos_set 0x80041048 setzen +0x8f
  nicht zurueck) - der Port bildet hier das Original ab und erfindet nichts dazu.
- **Nachladen bis in den Schnitt-Takt** (Abzug im letzten Bild vor der Sperre, Unit `schnitt` Fall "1 Bild"): Plc_dest bricht es
  ab wie im Original (+6/+7 = 0 @0x80041c1c/@0x80041c20; Auffuellen nur am Clip-Ende @0x80033ebc/@0x80033ec0) - das Magazin
  bleibt leer, der Vorrat 0x15 unveraendert; beim naechsten Abzug laedt Elza nach.
- **Riegel `integration_r39_dining1141_zielen_nachladen`** braucht ein bildgenaues Szenenende (RE15_AUDIO_CAP_SYNC; gemessen
  v_rl2/n_rl2 und Abnahme RL/RL2/K: Messhaken F4056). Verschiebt sich die Dauer bis zum Kampfende um mehr als etwa -2/+8 Bilder,
  meldet das Werkzeug "Vorbedingung Fall nachladen" (statt still zu bestehen); dann den Abzug im Skript (Bild 3000 + 1000 + 3 +
  50) nachziehen. Den Mechanismus haelt unabhaengig vom Bildtakt der Unit-Teil `schnitt` fest.
- Weiter offen aus 10.7 (unveraendert): Geltungsbereich der Ziel-FSM unter der Sperre (nur ROOM1141 Phase H; Original senkt
  ueberall); ein Bild Unterschied am Sperrbeginn; Abnahme 2 H5 (re15_dining_r39_tick ausserhalb des KI-Tors); gemeinsame Dateien
  ausserhalb VERTRAG 1.3 (audio_pc.c, enemy_ai_common.c, enemy_common.c, game_step_common.c, re15_damage.c, player_common.c);
  E-Riegel in der Fassung dieses Zweigs uebernehmen (H3); Spieler-Plc_motion(2,c) als W-Bank-Clip nur in ROOM1141. Diese
  Nachbesserung aendert keine gemeinsame Datei (nur Programm-Bytes, Generator, eigene Riegel).

### 11.8 Fuer den Nutzer (Nachbesserung 4)
- **Elza laedt nach, wenn die Gorillas sterben:** Wer kurz vor dem Ende des Kampfs noch nachlud (oder mit dem Messer zuschlug
  oder es zog), sah Elza zu Beginn des Gespraechs noch mit der Waffe auf John. Jetzt bekommt sie ihren ersten Befehl der Szene
  (sich zu John drehen) genau im Bild des Kamerawechsels - so wie Leon nach den Parkplatz-Gorillas im Original. Die Waffen-
  Bewegung endet mit dem Schnitt; in den ersten drei, vier Bildern sieht man nur noch, wie ihre Arme weich nach unten gleiten
  (die Ueberblendung des Originals). Ohne diesen Sonderfall aendert sich nichts: Elza steht beim Schnitt wie bisher ruhig da.
- Wer im allerletzten Moment nachlaedt, bricht das Nachladen ab (wie im Original): das Magazin ist danach leer, die Munition
  bleibt im Vorrat, beim naechsten Schuss laedt Elza nach.
- Keine neuen Dateien unter shared_assets/, keine Sprachdateien, keine Entscheidungsfrage. Neue Belegbilder unter
  analysis/befunde_runde39/J_belege/ (nb4_*).
