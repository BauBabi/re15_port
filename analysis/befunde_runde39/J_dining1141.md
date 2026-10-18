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
