# Runde 39 Spur P "johnruf1041" - Dossier (fortlaufend)

Baum: .claude/worktrees/r39_johnruf1041, Zweig r39/johnruf1041, Basis b1309110
(= r39/dining1141 c7253b42 + master 02dc7917, Nachtrag 10, Stimme room1041/main40.wav).

Nutzer-Wortlaut: AUFTRAG.md, Nachtrag 10 (2026-10-19). Lesart des Orchestrators (verbindlich):
"ROOM 1040" = ROOM1041 (Elza), "nicht erst in ROOM 1041" meint 1141. Bit 97 setzt NUR Spur P.

Punkte:
- P1: 1041 nach Johns Verschwinden: leise Hilferufe (help01..05) -> Elza msg 40 (main40.wav) -> Karte mit 1141-Marker
  -> Panikphase (Bit 97 + Leons Schlussmusik nach Irons' Tod) -> Flur 1131 lauterer "please help" -> 1141 wie bisher ohne msg 50.
- P2: Panikmusik endet bei Johns Rettung (Gorilla-Kampf 1141 vorbei), Bit 97 aus, Rest normal.

## 0. Stand / Fortschritt
- 2026-10-19 00:5x Baum sauber auf b1309110, Dossier angelegt (9251be13); Pflichtlektuere AUFTRAG/VERTRAG, J_dining1141, H_elza,
  H_elzatoene (r38), G_tagebuch (r38), K cut10f0 (r35) gelesen.
- Bau (configure + build LOCAL-BUILD-OK), Messung vorher v1/v2 (2d109eea), RE-Belege (3c360565).
- Umsetzung 02469270 (Modul + Haken), Messung n1a/n1b/n2, Unit-Riegel, 1131 auf Xa_vol 100 (gemessen, 4699f778),
  Integrations-Riegel a/b, Riegel der Spur G (Tabellenzahl) nachgezogen (3e772251).
- Endstand gezielt gruen: P 10/10, Unit/Prop/Sonden 849/850 + Tagebuch 4/4, Integration 19/19 (Abschnitt 5).
- 2026-10-19 08:3x-10:xx Nachbesserung 1 (Nutzer-Nachtrag 12): Merge master 57e5f920, Messung vorher, RE-Belege, Modul
  tuersperre_r39 (1041/1051 Panikphase, Kanaldeckel 10B1 immer zu), Riegel unit + integration, Abschnitt 8.

## 1. Messung vorher

Bau des Baums b1309110 + Dossier (local_build.sh configure + build = LOCAL-BUILD-OK). Messordner scratchpad `p39/`
(nicht committet), Laufskript `p39/lauf.sh` (Elza-Spielstand per probe_r36_elza_karte, CONTINUE, Messkopie
re15_pc_p39_<lauf>.exe neben der exe, RE15_WEGPLAN = echte Tueren).

### 1.1 Lauf v1: ECHTER Weg 1021 -> 1041 nach der John-Szene (Elza)
Spielstand ROOM1021 (-25500,-8200) Blick 2048 mit den Flags nach der 1021-Szene wie integration_r39_dining1141_d
(3:0x5F 4:0xF0 4:0xF1 4:0xEC 3:0x78; (4,0x60)=0 -> kein John), Wegplan: zur Tuer slot 2, Aktion, 1041, 500 Bilder warten.
debug.log: `[save] CONTINUE: resumed in room 1021`, `[room] PC loaded room1041.rdt`, `[bgm] stage=0 room=04 entry=FF00 ->
MAIN00(flag 0) ... [unveraendert, laeuft durch]`; in 1041 0 Zeilen `[msg]`, 0 `[voice]`, kein `[hint]` -> kein Hilferuf,
kein Satz Elzas, keine Karte, keine andere Musik. Elza steht bei (-24194,16500) in Cut 6 (Bild `l_v1/v1_f60.png`: Gang
nach Norden, Kamera bei (-25668,-3114,8442) blickt nach Sueden auf Elza). Im Gang laufen Zombies (Typ 0x16, RE2 EM016,
Raum-Records main00 @0x01224..): Aktor 1 Abstand 15290 (F28) -> 9384 (F568), ~11 Einheiten/Bild.

### 1.2 Lauf v2: ECHTER Weg 1121 -> 1131 mit Bit 97 = 1 (Spielstand, Flags wie v1 + 9:97)
`[bgm] stage=0 room=12 entry=FF17 -> MAIN17`, `room=13 entry=FF17 -> MAIN17 [unveraendert, laeuft durch]`; in 1131 0
`[voice]`, 0 `[msg]` in 300 Bildern -> Bit 97 hat keine Wirkung, kein Ruf in 1131, Musik = Tabelle (MAIN17).

### 1.3 Leon (Vergleich, Form der Panikmusik)
integration_r35_cut10f0 (Runde 35 K) haelt Leons MAIN01-Fenster fest: "MAIN01-Fenster auf" erst nach dem Szenen-Ende,
`entry=FF01 -> MAIN01` von Raum zu Raum, Ende am Parkplatz (11C0 = Gorillas). Gezielt nachgefahren in Abschnitt 5.

### 1.4 ROOM1141 heute (Spur J, Dossier J_dining1141.md 1.4/4.1)
Phase H: HAEMMERN39 startet MAIN16 (`54 00 01` @+0000 = sub00 @0x00B4E), Klopfen + help01..05 im Mix (help03 zuerst),
Elzas Reaktion msg 50 nach dem ersten Ruf (Spur J entfernt sie, Nachtrag 10); Kampfende sub01 @0x00C06 MAIN16 Stop,
@0x00C0C Set(4,0xF2)=0. Die Hilferufe spielen mit Standard-Pegel (help03 RMS 3348 im Mitschnitt, J 4.1).

## 2. RE-Belege

### 2.1 Wo/wann "verschwindet" John? (ROOM1021/1041/1141, selbst gelesen, tools/scd_dump_room.py + Flag-Zensus)
Flag-Zensus aller STAGE1-RDTs (Skript scratchpad p39/flagsuche.py, Opcode-exakter Walk scd_walk_lib):
    1011 sub02 @0x00AFE Set(4,0xF1)=1 / @0x00B02 Set(4,0xFA)=1      Roys Dialog (verwundet)
    1211 sub02 @0x01F82 Set(4,0x60)=1 / @0x01F86 Set(3,0xC9)=1      Prison: schaltet die 1021-Szene scharf
    1021 main00 @0x01DDE Ck(4,0x60)==1 -> @0x01DF6 Sce_em_set Typ 0x49 (John) + @0x01E0A Typ 0x47 (Roy) + Evt_exec sub03
    1021 sub02 @0x01FD8 Set(4,0x60)=0  -> beim naechsten Laden kein John mehr (Spawn haengt an (4,0x60))
    1021 sub02 @0x01FE0 Set(4,0xF0)=1  -> "John-Szene 1021 gelaufen"; letzte Zeile msg 8 "John: Please... I... I'd like
                                          to be / alone now..." (ROOM1021 msg_sec 0x22A4, id 8 @0x24C1)
    1141 sub00 @0x00AFE Ck(4,0xF0)==1  -> John in 1141 (Phase H); sub02 @0x00C18 Set(4,0xF0)=0, @0x00C1C Set(4,0xF2)=1
    1141 sub01 @0x00C06 `54 00 02 00 00 00` MAIN Stop + @0x00C0C Set(4,0xF2)=0, sobald beide Gorillas tot sind
    1041 main00 @0x0116C Ck(4,0xF1)==0 / @0x01174 Ck(3,0xAD)==0 -> AOT 8 (Schuss-Szene sub11) nur VOR Roys Dialog
=> In keinem Original-Skript tritt John in 1041 auf (1041: 0 Sce_em_set Typ 0x49). John "verschwindet" mit dem Ende der
1021-Szene: (4,0x60) wird geloescht (kein Spawn mehr), (4,0xF0) steht bis 1141 sub02. Tueren (Door_aot_set, Skript
scratchpad p39/tueren.py): 1011 hat nur Tueren nach 1021 (@0x008AE/@0x008CE); 1021 slot 2 @0x01CC2 -> 1041 (Raum 04, Cut 6,
(-24000,0,16500)); 1041 slot 2 @0x010D8 -> 1061; 1061 -> 1121 (Raum 0x12, @0x00D12); 1121 slot 2 @0x00CD8 -> 1131 (Cut 2);
1131 slot 0 @0x0088E -> 1141. Jeder Weg aus 1021/1011 nach oben fuehrt also durch 1041 -> Ausloeser = ERSTER Eintritt in
ROOM1041 mit (4,0xF0)=1 und Bit 97 = 0 ("nachdem John verschwunden ist").

### 2.2 Form der 1041-Szene: ROOM1041 sub11 @0x021EA..@0x0223C (das Original "Elza hoert etwas" in DIESEM Raum)
    @0x021EA 22 03 ad 01 Set(3,0xAD)=1 | @0x021EE 46 08 .. Aot_reset 8 | @0x021F8 22 02 07 01 Set(2,7)=1 | @0x021FC 22 01 1b 01
    @0x02200 2b 03 00 00 Message_on 3 "gunshot" (Runde 38 H: Ton) | @0x02204 2e 01 00 + 00 Work_set(1,0) | @0x02208 09 0a 19 00
    Sleep 25 | @0x0220C 40 00 09 20 3c ab 04 42 Plc_dest Modus 9 (Drehen zum Punkt) | @0x02214..@0x0221C Do/Evt_next/Edwhile
    Ck(5,0x20)==0 | @0x02220 Sleep 25 | @0x02224 2b 02 00 00 msg 2 "Elza: What was that!?" | @0x02228 09 0a 64 00 Sleep 100 |
    @0x0222C/@0x02230 Set(2,7)=0 Set(1,0x1b)=0 | @0x02234 3c 01 Cut_auto 1 | @0x02236 Work_set(1,0) | @0x0223A 42 Plc_ret |
    @0x0223C 01 00 Evt_end
Text: ROOM1041 msg_sec 0x2248, msg 2 @0x022DC `04 00 05 01 ...` (Elza = Farbe 1, ctrl 04:00 05:01 05:00 04:01 01) - Form
fuer msg 40.

### 2.3 "leise" / "etwas lauter": RE2-Form Xa_vol (Sound ist RE2; RE1.5-Tabelle endet bei 0x5E, kein Xa_vol)
RE2-SCD-Verteiler @0x800a74c8 (143 Handler); Eintrag 0x5F @0x800a7644 -> 0x80058090:
    80058090 1c 00 82 8c  lw v0,28(a0) | 80058098 01 00 43 90 lbu v1,1(v0) (Skriptbyte) | 800580a0 80 00 62 2c sltiu v0,v1,0x80 |
    800580a4 beq -> | 800580b0 50 5b 23 ac sw v1,0x5b50(at) = XA-Lautstaerke 0x800d5b50 | 800580bc addiu v0,v0,2 (pc += 2)
Standard 0x78 = 120: FUN_800128c4 (CD-Init) @0x80012958 `li v0,0x78` / @0x8001296c `sw v0,0x850(s0)` = 0x800d5b50, und bei
JEDEM Raum-Skript-Init FUN_80052f3c (gerufen aus FUN_80053528 @0x800535b8, das die Verteilertabelle nach 0x1f800000 kopiert)
@0x80053014 `addiu v0,zero,120` / @0x80053038 `sw v0,0x5b50(at)` -> Xa_vol gilt bis zum naechsten Xa_vol oder Raumwechsel.
Anwendung FUN_80013ac8 (aus der XA-Wiedergabe FUN_800129b4 @0x80012b44 ueber FUN_80013b4c): Stereo (0x800d5b5b==1,
@0x80013ad0/@0x80013ad8) L=R=vol @0x80013ae4; Mono @0x80013b08 `srl v0,a0,1` = vol>>1 je Weg; DsMix @0x80013b34. Relativ
zum Standard also Faktor vol/120 (Mono (vol>>1)/60, gleich).
RE2 benutzt Xa_vol fuer Stimmen aus dem Off (Bytes aus den RDTs, info/re2leon/PL0/RDT):
    ROOM11D0.RDT sub09 @0x01F7A `5f 46` Xa_vol(70) + `59 00 0b 00` Xa_on(0,0x0B) | Else @0x01F84 `5f 32` Xa_vol(50) + `59 00 09 00`
    ROOM1020.RDT sub02 @0x01960 `5f 64` Xa_vol(100) + `59 00 0b 00`   (beide: Se_on mit Lage, Sleep 10, dann die Stimme)
    ROOM1010 sub22 Xa_vol(120) vor einem Dialog = zurueck auf Standard (decompiliert info/re2leon/PL0/RDT/room1010/scd/sub22.c)
=> Form fuer die Nutzer-Vorgabe: 1041 "leise" = Xa_vol 50 (kleinster RE2-Wert, 11D0 Else-Zweig), 1131 "schon etwas lauter"
= Xa_vol 100 (ROOM1020 @0x01960: dieselbe XA-Stimme 0x0B, die 11D0 mit 70 spielt; 70 nach Messung zugunsten von 100 verworfen,
Abschnitt 4.2), 1141 unveraendert Standard 120; vor Elzas Zeile Xa_vol(120) (Form ROOM1010 sub22). Messung der Wirkung:
Abschnitt 4.

### 2.4 Leons "panische finale Musik" (nach Irons' Tod, auf dem Weg zu den Gorillas) = MAIN01-Fenster der Runde 35 K
include/re15_cut10f0.h / engine/src/cut_10f0.c (Nutzer Runde 35: "Bis Leon dann den Parking Lot erreicht hat, soll
durchweg MAIN01 als Background Musik gespielt werden"): Fenster auf, sobald (9,71) UND (9,73) "Irons-Todesszene gesehen"
stehen und keine Szene laeuft (= Ende der 1150-Montage), zu mit (4,64) (ROOM11C0 sub02 @0x0184E Ankunft am Parkplatz =
bei den Gorillas). Weiche audio_pc.c ss_bgm_entry -> Tabellen-Eintrag 0xFF01 (MAIN01 = Slot 1, ROOM1030 traegt ihn
@0x8007482e als 0x4041 mit Manuell-Start-Flag; FUN_800444b0 spielt nur Flag 0 @0x800444c8), im Zielraum 11C0 dessen eigene
Musik; Sce_bgm_control an Slot 0 im Fenster nicht angewandt (FUN_80044da4 op 2 @0x80044e50 stoppte sonst MAIN01).
Abgenommen von integration_r35_cut10f0 (A/D/E/F). Form fuer Elza: Fenster = Bit 97, Zielraum 1141 mit eigener Musik
(MAIN16 = Raum-Titel 1141, Runde 38 F, gestartet von HAEMMERN39/@0x00E5C, gestoppt sub01 @0x00C06 = "John gerettet").

### 2.5 Karte mit Marker: RE2-Kartenhinweis (Opcode 0x84 @0x800591C4) in der Port-Form der Runde 35 K / 38 G
engine/src/map_hint_common.c s_hints[]: Eintraege ohne Anker (pc_versatz 0), portseitig angefordert
(re15_map_hint_request), "gezeigt" = Spiel-Flag, "erreicht" = eigener Latch oder Besucht-Bit der Zielzone; Zielzone
ROOM1141 = engine/src/re15_map_zones.h Zeile `{ 0x1141, -10600, -25600, 16700, 1850, 4, 6, ...}` (Blatt 4, Rechteck 6).
Zeitpunkt wie Spur H (Roys Tagebuch): Faden parkt, bis der Hinweis zu ist, dann erst weiter.

## 3. Umsetzung

### 3.1 Dateien
- NEU `re15_port/include/re15_johnruf_r39.h` (alle Konstanten mit Beleg), `re15_port/engine/src/johnruf_r39.c` (Installer,
  Opcode 0x5F, Message_on-Haken, Stimmen-Ordner, Musik-Weiche/-Sperre/-Tick, Rettung), `re15_port/tools/r39_johnruf/
  programme_bauen.py` -> `re15_port/engine/src/gen/johnruf_r39_programme.inc` (RUF1041 94 B, RUF1131 18 B, msg 40; Listing
  je Zeile mit Vorbild; `--pruefen` vergleicht).
- HAKEN (je 1-2 Zeilen, Kommentar "Runde 39 Spur P"): scd_room_setup.c (Include + Installer am ENDE des Blocks nach Spur I),
  platform/pc/main.c (Boot-/CONTINUE-Weg nach Spur C), scd_vm.c (Include; `s_op_table[0x5F]` mit PC-Schranke wie 0x62 der
  Runde 34; op_message_on hinter Spur J), game_step_common.c (Tick nach re15_tagebuch_r38_tick, vor der Hinweis-Annahme),
  audio_pc.c (ss_bgm_entry nach der Weiche der Runde 35 K; SEQ_CTL-Sperre nach K; re15_voice_load_clip Ordner/Stamm nach
  Spur J; re15_voice_play CD_VOL aus Xa_vol), map_hint_common.c (Include + Eintrag 5), tests/unit/test_r36_kartenbits.c
  (Include + k_port[] Bit 97). KEIN Asset-Patch, keine neue Datei unter shared_assets/ oder synchro/.
- Ressourcen: Bank 9 Bit 97 (VERTRAG 1.1), ROOM1041 Id 40 (Text + Stimme main40.wav), Sprach-Ids 53..57 (ohne Text, in
  1041 und 1131; dieselbe Zuordnung help01..05 wie Spur J in 1141), Warte-Zeile 59 (1041, ohne Text).

### 3.2 Welcher Ruf ist "das please help"? (Spracherkennung, Windows System.Speech en-US, Zwangswahl mit Schwelle 0)
help01 3,42 s: "hello" 0,38 + "please help" 0,42 (zwei Segmente) | help02 1,96 s: "please help" 0,35 bzw. "please" 0,30 +
"help" | help03 1,57 s: "help" 0,23 | help04 1,36 s: - | help05 1,18 s: "help" 0,02. Huellkurven (20/s): help02 = zwei
Woerter, help01 = drei Segmente ("Hello? ... Please help!"). => "das please help" = help02 ("Please help!"); help01 traegt
dasselbe am Ende. PORT-WAHL: 1041 = help03 ("Help!") + help02, 1131 = help02 noch einmal ("auch noch mal", lauter).
Skripte scratchpad p39/wahl2.ps1, huelle.py.

### 3.3 Ablauf (je Nutzer-Satz)
| Nutzer | Mechanik | Beleg / Form |
|---|---|---|
| "Hilferufe ... erstmalig leise im ROOM 1040 ..., nachdem John verschwunden ist" | Installer 1041: (4,0xF0)=1 und Bit 97=0 -> RUF1041; Set(2,7)/(1,0x1B), Sleep 30, Xa_vol 50, help03, Work_set, Sleep 25, Plc_dest Modus 9 zur Treppentuer, Warten (5,0x20), Sleep 25, help02, Sleep 90 | 2.1 (F0 = John weg), 2.2 sub11 @0x021F8..@0x02220, 2.3 RE2 Xa_vol 50 @0x01F84, Treppentuer main00 @0x010D8 |
| "dann soll bereits von Elza kommen: Oh, no, this is John! ..." + "Kopiere mir auch das voicefile" | Xa_vol 120, Message_on 40 (Text k_r39p_msg40, Stimme synchro/STAGE1/room1041/main40.wav = Kopie main50, liegt seit 02dc7917), Sleep 100 | sub11 @0x02224/@0x02228; RE2 ROOM1010 sub22 Xa_vol(120) |
| "Dann soll die Karte aufgehen und den ROOM 1141 markieren" | Warte-Zeile 59: Stimmen-Riegel laesst main40 ausreden, dann Kartenhinweis Eintrag 5 (ROOM1041 -> ROOM1141, zeitgesteuert), Faden parkt bis zu | RE2 0x84 @0x800591C4, Form Spur H / Runde 35 K |
| "Danach soll wieder die gleiche Panische finale Musik kommen wie bei Leon" | Set(9,97)=1, Steuerung zurueck; Tick: Fenster auf (keine Szene) -> Weiche 0xFF01 = MAIN01 in allen STAGE1-Raeumen ausser 1141, Sce_bgm_control Slot 0 gesperrt, Raummusik-Anstoss | 2.4 (cut_10f0.c, Eintrag 0xFF01, 11C0 ausgenommen) |
| "Wenn wir uns den Flur 1131 naehern, ... please help ... etwas lauter" | Installer 1131: Bit 97=1, (4,0xF0)=1, Ruf in dieser Phase noch nicht -> RUF1131: Sleep 30, Xa_vol 100, help02, Sleep 90, Xa_vol 120 (keine Szene) | 2.3 RE2 Xa_vol 100 ROOM1020 @0x01960, Wahl 4.2 |
| "Im ROOM 1141 soll alles so bleiben wie bisher" | 1141 von der Weiche ausgenommen (eigene Musik MAIN16 und Sce_bgm_control der Spur J wie bisher); Rufe in 1141 Standard 120 | Leon-Form 11C0; Runde 38 F |
| "Die panische Musik hoert auf, sobald man John gerettet hat. Der Rest bleibt normal." | Tick: Bit 97 und (4,0xF0)=0 und (4,0xF2)=0 -> Bit 97 = 0, Fenster zu, Tabellen-Musik | 2.1 sub02 @0x00C18 / sub01 @0x00C0C (+ @0x00C06 MAIN16 Stop) |
| (3) Wiederbetreten 1041 / Laden in der Phase | Bit 97 sperrt RUF1041; Bit 97 im Spielstand (Bank 9 mit g_game.flags), Tick stoesst MAIN01 nach dem Laden an | Form cut_10f0.c bgm_fenster_tick |


## 4. Messung nachher

### 4.1 Laeufe n1a/n1b: ECHTER Weg 1021 -> 1041 -> 1061 (Spielstand wie v1 + 4:5 4:4 = Rolladen offen; Bau 02469270)
n1a (Framedump je 10 Bilder in 1041, Bogen `p39/l_n1a/n1a_bogen.png`), n1b (Wegplan bis 1061, Mitschnitt):
| Bild (1041) | debug.log |
|---|---|
| F91 (Raumaufbau) | `[johnruf] F91 ROOM1041: John verschwunden ((4,0xF0)=1), Bit 97=0 -> RUF1041 gestartet (Faden 10)`, `[szenenschutz] F6 an` |
| F36 | `Xa_vol(50) ROOM1041 -> CD_VOL 6826`, `Ruf help03 (Sprach-Id 55 ...)`, `[voice] clip loaded: help03.wav` |
| F98 | `Ruf help02 (Sprach-Id 54, Xa_vol 50 -> CD_VOL 6826)`, `clip loaded: help02.wav` |
| F188 | `Xa_vol(120) -> CD_VOL 16383`, `[msgtext] room=1041 id=40 "Elza: Oh, no, this is John! He is in trouble, I have to help him!"`, `clip loaded: main40.wav` |
| F370 | `Kartenhinweis angefordert (Eintrag 5: ROOM1041 -> ROOM1141), Faden parkt` (= F188 + 180 Bilder main40) |
| F380..F498 | `[hint] F380 begin`, Blinken rot/umriss, `F498 Zeit um (Hinweis 5, 3 Blinkperioden)`, `schliessen` |
| F512 | `Karte zu: Faden laeuft weiter (Set(9,97)=1 ...)`, `Panik-Musikfenster auf in ROOM1041`, `[bgm] stage=0 room=04 entry=FF01 -> MAIN01` |
| 1061 | `[bgm] stage=0 room=06 entry=FF01 -> MAIN01 ... [unveraendert, laeuft durch]` |
Bilder: F40/F130 Elza an der Tuer, dreht sich nach Norden (Gier 923 = Soll 924 zur Treppentuer); F200 Text msg 40; F390/F450
Karte "POLICE STATION 3F" mit ROOM1141 rot / Umriss; F530 Steuerung zurueck. Szenenschutz (Runde 38 B) haelt die Zombies im
Gang ab, solange die Szene laeuft (Aktor 1: Abstand 5145 -> 2246, Biss erst F604 nach der Szene, ohne Eingaben).

Pegel (Mitschnitt n1b, `p39/pegel.py`: normierte Kreuzkorrelation + kleinste Quadrate, 44,1 kHz):
| Zeile | Takt | rho | g (Mitschnitt/Datei) | erwartet = Lader x Xa | RMS Fenster | RMS 1 s davor (Raum) |
|---|---|---|---|---|---|---|
| help03 (1041, Xa 50) | 649 | 0,953 | 0,488 | 1,156 x 0,417 = 0,482 | 1333 | 608 |
| help02 (1041, Xa 50) | 711 | 0,556 | 0,482 | 1,133 x 0,417 = 0,472 | 1770 | 938 |
| main40 (Elza, Xa 120) | 801 | 0,932 | 1,030 | 1,020 x 1,000 = 1,020 | 2547 | 289 |
(Lader = Spitzen-Boden audio_pc.c 14745/Spitze; Stufe 2 greift bei keiner der drei Dateien.) => die Rufe in 1041 klingen mit
0,417 = -7,6 dB ihres Normalpegels, deutlich ueber dem Raumklang (RMS 1333 gegen 608), leiser als Elzas Zeile (2547).
Messfehler der ersten Auswertung (int32-Ueberlauf in np.arange auf Windows) behoben, alle Werte oben neu.


### 4.2 Lauf n2: Spielstand 1121 MITTEN in der Panikphase -> 1131 -> 1141 (Spur J unveraendert) -> Rettung -> 1171
Spielstand ROOM1121 (-7400,-2900) Blick 2048, Flags wie v1 + 9:97, Dining Room Key im Inventar (give:0x43), Wegplan
1121 -> Tuer -> 1131 -> Tuer (Schluessel) -> 1141 (Wegplan der Spur J) -> Dachtuer -> 1171, RE15_R38F_GORILLAS_TOT=600
(Kill-Flags, keine Schusszahl). debug.log (Bau 02469270, 1131 noch mit Xa_vol 70):
- Laden: `[bgm] stage=0 room=12 entry=FF17 -> MAIN17` (Boot-BGM vor dem Restore), dann `[johnruf] F0 Panik-Musikfenster
  auf in ROOM1121 (Bit 97 = 1)`, `Raummusik-Anstoss ... Soll MAIN01`, `entry=FF01 -> MAIN01` -> Laden mitten in der Phase
  setzt die Musik fort.
- 1131: `RUF1131 gestartet`, `F36 Xa_vol(70) -> CD_VOL 9556`, `Ruf help02 (Sprach-Id 54 ...)`, `F126 Xa_vol(120)`,
  `room=13 entry=FF01 -> MAIN01 [unveraendert, laeuft durch]`.
- 1141: `entry=FF56 -> MAIN16(flag 1)`, HAEMMERN39 `Sce_bgm_control slot=0 op=1` angewandt (1141 von der Weiche
  ausgenommen), Klopfen + help01..05 der Spur J wie bisher (Standard 120), msg 50 der Spur J noch da (Spur J entfernt sie in
  ihrem Baum, Nachtrag 10). Kampfende: `Sce_bgm_control slot=0 op=2 ... capTick=5430` (sub01 @0x00C06) und im selben
  Bild `[johnruf] F4039 ROOM1141 John gerettet ((4,0xF0)=0, (4,0xF2)=0): Bit 97 = 0, Panikmusik aus`, `Fenster zu`.
- 1171: `entry=5572 -> MAIN32` (Tabelle), kein FF01 -> "Der Rest bleibt normal".
Pegel (Mitschnitt n2, `p39/pegel.py`):
| Zeile | Takt | rho | g | erwartet | RMS Fenster | RMS 1 s davor |
|---|---|---|---|---|---|---|
| help02 1131 (Xa 70) | 649 | 0,951 | 0,670 | 1,133 x 0,583 = 0,661 | 1438 | 979 (MAIN01) |
| help02 1141 (Spur J, 120) | 2507 | 0,870 | 1,146 | 1,133 | 2688 | 2238 |
| help03 1141 (Spur J, 120) | 1466 | 0,908 | 1,169 | 1,156 | 3348 | 1299 |
**Wahl 1131 = Xa_vol 100 statt 70 (gemessen):** die Stimme help02 hat im Mix RMS g x 2041: 1041 (50) 984, 1131 (70) 1367,
1131 (100) 1927. Der Hintergrund ist in 1131 aber MAIN01 (RMS 979), in 1041 MAIN00 + Raum (608). Abstand Stimme - Hintergrund:
1041 +4,2 dB, 1131 mit 70 nur +2,9 dB (der "lautere" Ruf stuende WENIGER heraus als der leise), mit 100 +5,9 dB. 100 ist der
RE2-Wert derselben XA-Stimme 0x0B in ROOM1020 (@0x01960 `5f 64`), die ROOM11D0 mit 70 spielt - RE2 regelt dieselbe Stimme je
Raum. Damit: 1041 0,417 (-7,6 dB) < 1131 0,833 (-1,6 dB) < 1141 1,0. Nachmessung mit 100: Abschnitt 4.3.

### 4.3 Riegel-Laeufe am Endstand (Bau nach 4699f778, Xa_vol 1131 = 100): integration_r39_johnruf1041_a/_b
Werkzeug `test_r39_johnruf1041 pegel` (Referenz = Datei genau wie der Port sie laedt: Resampling Q16, Spitzen-Boden,
Stufe 2; g am besten Versatz +-3 Takte um die Startzeile `[voice] Start ROOM.. Id .. CD_VOL .. capTick N`):
| Lauf | Zeile | Takt | rho | g gemessen | Soll Xa/120 | Verhaeltnis |
|---|---|---|---|---|---|---|
| a (lauf_a_88b2bda7) | 1041 help03 (Id 55) | 649 | 0,953 | 0,4179 | 0,4167 | 1,003 |
| a | 1041 help02 (Id 54) | 711 | 0,556 | 0,4201 | 0,4167 | 1,008 |
| a | 1041 main40 (Id 40) | 801 | 0,932 | 1,0021 | 1,0000 | 1,002 |
| b (lauf_b_3ca86312) | 1131 help02 (Id 54) | 649 | 0,975 | 0,8331 | 0,8333 | 1,000 |
| b | 1141 help03 (Spur J, Id 55) | 1466 | 0,908 | 1,0007 | 1,0000 | 1,001 |
1131 mit 100: Musik MAIN01 davor RMS 979, Ruf-Fenster RMS 2001 (+6,2 dB), danach 522. Lauf a: Folge help03 -> help02 ->
msg 40 + main40 -> Karte (Eintrag 5) -> Schirm -> zu -> Fenster -> FF01, Gier bei msg 40 = 923 (Soll 924), Abzug des
Hinweis-Schirms `karte.bmp` (scratchpad `p39/karte_a.png`): "POLICE STATION 3F", ROOM1141 rot gefuellt (Blinkphase),
1061 `entry=FF01 ... [unveraendert, laeuft durch]`. Lauf b: MAIN01 nach dem Laden in 1121, genau ein Ruf in 1131 mit
CD_VOL 13652, 1141 `entry=FF56 -> MAIN16` und kein "NICHT angewandt", "John gerettet" direkt hinter dem MAIN16-Stopp
(sub01 @0x00C06), 1171 `entry=5572 -> MAIN32`, keine `[johnruf]`-Zeile und kein FF01 mehr.

## 5. Tests
Neu (tests/unit/probes/r39_johnruf1041.cmake, tests/unit/test_r39_johnruf1041.c, tests/integration/test_r39_johnruf1041.cmake):
| Test | misst | Nutzer-Punkt |
|---|---|---|
| unit_r39_johnruf1041_texte | msg 40 dekodiert = "Elza: Oh, no, this is John! He is in trouble, I have to help him!", Form ROOM1041 msg 2 | Satz |
| unit_r39_johnruf1041_programme | RUF1041: Set(2,7) zuerst, help03/help02 mit Xa_vol 50, Plc_dest Modus 9 zur Treppe, Xa_vol 120 vor msg 40, msg 40 -> 59 -> Set(9,97) -> frei; Pause >= 59 Bilder; 54 B Original-Zeilen bytegleich mit ROOM1041.RDT sub11; RUF1131: Xa_vol 100 > 50, keine Pad-Sperre | leise, Satz, Karte, Musik, 1131 |
| unit_r39_johnruf1041_stimme | (1041/1131, 53..57) -> room1141/help01..05; 40 -> room1041/main40; 1141/Leon unveraendert | Rufe, Stimme |
| unit_r39_johnruf1041_xa | 0x5F nur in den Programmen (pc+2), sonst op_unknown (pc+1); CD_VOL 0x3FFF*vol/120; Raumaufbau 120 | leise/lauter |
| unit_r39_johnruf1041_ablauf | 1041 am VM mit allen Installern: nur mit (4,0xF0)=1/Bit 97=0; Sperre ab Bild 1, kein freies Bild bis Bit 97; Stimmen 55/54 mit 50, 40 mit 120; Blick 923 (Soll 924); Hinweis Eintrag 5 nach msg 40, Schirm Blatt 4 Rechteck 6; Bit 97 nach dem Schliessen, frei im selben Bild; Musik-Anstoss Raum 0x04; Wiederbetreten ohne Wiederholung | alle 1041-Saetze |
| unit_r39_johnruf1041_musik | Weiche 0xFF01 fuer 1041/1061/1121/1131/1031, -1 fuer 1141 und STAGE2; Sperre nur Slot 0; Kartenziel 1141 aktiv; Kampf (F2=1) haelt Bit 97, Rettung loescht es + Tabellen-Musik einmal; Laden in der Phase (1061) | Musik, Rettung, Laden |
| unit_r39_johnruf1041_ruf1131 | Flanke Bit 97 setzt den Ruf zurueck; ohne Phase / F0=0 kein Ruf; ein Ruf help02 mit 100, keine Sperre; Wiederbetreten kein zweiter | 1131 |
| unit_r39_johnruf1041_leon | Leon ROOM1040 mit denselben Flags: nichts, Weiche -1 | Leon unveraendert |
| integration_r39_johnruf1041_a | echte exe, Spielstand 1021 + CONTINUE, Tueren 1021 -> 1041 -> 1061: Folge, Blick, FB_SHOT, FF01 in 1041 und 1061, Pegel 3 Zeilen | 1041-Saetze, Musik |
| integration_r39_johnruf1041_b | echte exe, Spielstand 1121 mit Bit 97: MAIN01 nach dem Laden, 1131 ein Ruf (Pegel 100), 1141 MAIN16 + Rufe 120, Rettung hinter dem MAIN16-Stopp, 1171 MAIN32 | 1131, Rettung, Laden |
Angepasst: tests/unit/test_r36_kartenbits.c k_port[] + Bit 97; tests/unit/test_r38_tagebuch.c teil_karte: Tabelle hat 6
Eintraege (+ ROOM1041 -> ROOM1141 hinter den Eintraegen der Spur G) - vorher pinnte er 5.

Laeufe (Bau am Endstand, gezielt, keine volle Suite):
- `ctest --test-dir re15_port/build -R "r39_johnruf1041" --timeout 3600 --output-on-failure -j 2` ->
  "100% tests passed, 0 tests failed out of 10" (integration a 90,20 s, b 326,89 s).
- ALLE Unit-/Prop-/Sonden-Riegel `ctest --test-dir re15_port/build -R "^(unit_|prop_|probe_)" --timeout 600 -j 4` ->
  "99% tests passed, 1 tests failed out of 850" (1370 s); rot nur unit_r38_tagebuch_karte = Pin der Hinweis-Tabelle auf 5
  Eintraege (mein Eintrag 5 ist neu) -> Riegel nachgezogen (3e772251), danach
  `ctest --test-dir re15_port/build -R "unit_r38_tagebuch" --timeout 600` -> "100% tests passed, 0 tests failed out of 4".
- Integration der beruehrten Module (Pflicht: integration_r39_dining1141_d + integration_r38_dining1141*; dazu Leons
  MAIN01-Fenster, Sprachpfad, 1041-Schuss, Kartenketten), ctest -R woertlich:
  `ctest --test-dir re15_port/build -R "integration_r39_dining1141_d|integration_r38_dining1141|integration_r35_cut10f0|integration_r39_stimmen|integration_r38_elzatoene|integration_r38_tagebuch|integration_r39_elza_karte" --timeout 3600 --output-on-failure -j 2`
  -> "100% tests passed, 0 tests failed out of 19" (darin integration_r35_cut10f0 268,87 s = Leon unveraendert,
  integration_r38_dining1141_s 667,65 s, integration_r39_dining1141_d 359,26 s, integration_r38_elzatoene_gunshot1041 79,73 s).

## 6. OFFEN
- Spur J (parallel, Zweig r39/dining1141): entfernt Elzas Reaktion msg 50 in 1141 (Nachtrag 10). In diesem Baum laeuft sie
  noch (Lauf n2: `[msgtext] room=1141 id=50`). Bei der Integration gilt J; dieser Baum aendert in 1141 nichts (Weiche nimmt
  1141 aus, Rufe dort Standard 120, keine Zeile in dining_r39.c).
- Spuren Q/R lesen Bank 9 Bit 97 (RE15_R39P_BIT_BANK/RE15_R39P_BIT_PANIK in include/re15_johnruf_r39.h). Startet Q eine
  Szene (Montage) an der steigenden Flanke, oeffnet das Musikfenster nach der Form der Runde 35 K erst, wenn keine Szene mehr
  laeuft ((1,27)/(2,7) aus); faellt Qs erstes Szenenbild NACH dem Tick dieses Bildes, beginnt MAIN01 ein Bild vor der Montage -
  bei der Integration am gemeinsamen Lauf ansehen.
- Der lautere Ruf in 1131 kommt einmal je Panikphase und Sitzung (Merker im Speicher, Bit 97 steht im Spielstand): wer in der
  Phase speichert und spaeter in 1131 laedt bzw. hineingeht, hoert ihn nach dem Laden ein weiteres Mal. Ein dauerhafter Merker
  braeuchte ein zweites Bank-9-Bit (VERTRAG 1.1: nur 97 an P).
- Gemeinsame Dateien ausserhalb der VERTRAG-1.3-Liste fuer P: game_step_common.c (1 Zeile), map_hint_common.c (Include +
  Eintrag), main.c (2 Zeilen). Alle mit Kommentar "Runde 39 Spur P".

## 7. Fuer den Nutzer
- Ablauf jetzt (Elza): nach der Szene mit John in 1021 (John bleibt zurueck und ist danach weg) -> beim ersten Betreten des
  Gangs 1041 ruft John zweimal LEISE um Hilfe ("Help!", dann "Please help!") -> Elza dreht sich zur Treppe -> "Oh, no, this is
  John! He is in trouble, I have to help him!" (deine Aufnahme room1041/main40.wav) -> die Karte geht auf und markiert ROOM1141
  (3F) -> danach laeuft Leons Panikmusik nach Irons' Tod (MAIN01) in allen Raeumen bis zu John; im Flur 1131 ruft John noch
  einmal "Please help!", deutlich lauter -> in 1141 alles wie bisher (eigene Musik des Raums) -> sind beide Gorillas tot, hoert
  die Panikmusik auf, danach normale Raummusik.
- Lautstaerken (RE2-Werte, gemessen im Mitschnitt): 1041 leise = 42 % (-7,6 dB) des normalen Pegels, 1131 = 83 % (-1,6 dB),
  1141 = 100 %. Neue Sprachdateien: keine noetig. Elzas Satz in 1041 = `synchro/STAGE1/room1041/main40.wav` (liegt). Die Rufe
  sind deine Dateien `synchro/STAGE1/room1141/help02.wav` / `help03.wav` (unveraendert, Zuordnung im Code). Moechtest du in 1041
  oder 1131 andere Rufe (z.B. help01 "Hello? ... Please help!"), genuegt ein Wort.
- Paket-Gate: KEINE neue Datei unter shared_assets/ oder synchro/ (Programme als engine/src/gen/johnruf_r39_programme.inc).
- Bedienhinweis: die Karte schliesst nach ~4 s von selbst oder mit START; danach hast du die Steuerung.
- Belegte Sprach-/Nachrichten-Nummern: ROOM1041 Id 40 (Elzas Satz, main40.wav), 53..57 (Rufe = room1141/help01..05,
  ohne Text), 59 (Warte-Zeile Karte); ROOM1131 53..57 (Rufe). Eigene Aufnahmen unter room1041/main53..59.wav bzw.
  room1131/main53..57.wav wuerden dort NICHT gespielt.

## 8. Nachbesserung 1 (Abnahme 0 bestanden; Nutzer-Nachtrag 12, 2026-10-19 08:15-08:22)
Auftrag (Orchestrator, Maengel 1-3): (3) zuerst `git merge master` (a0d7d749: K heliport1171, G karten, O parkplatz11c0,
R hund1121); (1) Elza-Panikphase (Bit 97): Tuer ROOM1041 -> ROOM1031 und Tuer ROOM1051 -> ROOM10A1 zeigen statt des
Raumwechsels "I have to rescue John! He is in danger!" (Port-Ids 1041 = 41, 1051 = 40); (2) ROOM10B1: die Tuer ist fuer
Elza IMMER zu, auch bei der Suche nach Sherry.

### 8.0 Merge (Mangel 3)
- `git merge master` -> 57e5f920. Konflikte nur an den Blockenden: AUFTRAG.md (Nachtrag 11 aus master; der Hinweis an Spur J
  aus J bleibt), Include-/Installer-Zeilen in scd_room_setup.c (2), scd_vm.c, platform/pc/main.c, map_hint_common.c - beide
  Seiten behalten, die Zeile der Spur P jeweils am Ende des Blocks.

### 8.1 Messung vorher (Bau nach dem Merge 57e5f920, echte exe mit Elza, Spielstand + CONTINUE, echte Tueren)
Werkzeug `P_belege/mess.sh` (Elza-Karte probe_r36_elza_karte, Messkopie `re15_pc_p39n_<lauf>.exe` neben der exe, eigener
Laufordner, RE15_*-Umgebung nicht geerbt, RE15_WEGPLAN). Auszuege `P_belege/nb1_vorher_<lauf>.txt`.
| Lauf | Zustand | Eingabe | debug.log | Befund |
|---|---|---|---|---|
| v3 | ROOM1041, Bit 97 = 1 (Panikphase), Rollo offen (4,5)/(4,4) | GEHE (-20800,-3800), BLICK 0, AKTION | `[aot] DOOR FIRE slot=1 rect=(-19650,-3800,hw=750,hh=1100) target_cut=8` -> `loaded room1031.rdt` | Tuer 1041 -> 1031 geht in der Phase (Mangel 1) |
| v4 | ROOM1051, Bit 97 = 1, Rollo 1051 offen (3,0x79) | GEHE (16450,-13700), BLICK 0, AKTION | `[aot] DOOR FIRE slot=4 rect=(17200,-13700,hw=500,hh=1000) target_cut=0` -> `loaded room10a1.rdt` | Tuer 1051 -> 10A1 geht in der Phase (Mangel 1) |
| v6a | ROOM10B1 (ueber die echte Tuer 1261 Slot 2 -> 10B1 Cut 7), keine Szenen-Flags = vor der Szene | BLICK 3072, AKTION | `[aot] DOOR FIRE slot=1 rect=(-21370,4610,hw=750,hh=1050) target_cut=15` -> `loaded room1261.rdt` | Kanaldeckel offen (Mangel 2) |
| v6b | wie v6a + (3,0x82)=1 = Szene "John, wait!" steht an (ROOM1171 sub02 @0x013A4) | keine | msg 0..5 mit main00..05.wav, dann `[aot] DOOR FIRE slot=1` (= sub04 @0x01A8E `47 01`) -> `loaded room1261.rdt` | die Szene der Sherry-Suche oeffnet den Deckel (Mangel 2) |
| v6c | wie v6a + (3,0x83) (3,0x41) (3,0x46) = nach der Szene, Sherry-Suche (ROOM2001 @0x01958) | BLICK 3072, AKTION | `[aot] DOOR FIRE slot=1` -> `loaded room1261.rdt` | offen (Mangel 2) |

v5a-c (Erkundung 10B1): Cut 0 zeigt die obere Terrasse mit der Leiter zum Dach (Slot 0 -> ROOM1171), Cut 7 (Bild
`P_belege/nb1_10b1_cut7_kanaldeckel.png`, RE15_FORCE_CUT=7) die Gasse mit dem KANALDECKEL vor Elzas Fuessen (Slot 1).

### 8.2 RE-Belege
Selbst gelesen mit tools/scd_dump_room.py, tools/rdt_msgdump.py und den Skripten P_belege/tueren.py, flagsuche.py,
msggrep.py; RE2 mit analysis/nutzer_batch_2026-08-27/tools/re2_scd_walk.py und O_belege/re2_msgs.py.

Tuer-Saetze (Door_aot_set, 32 B, Ziel = Byte +22 Stage / +23 Raum / +24 Cut):
- ROOM1041 main00 @0x010B8 `3b 01 02 31 00 00 50 b0 dc ec dc 05 98 08 9a 99 00 00 eb f0 00 00 00 03 08 00 ..` = Slot 1,
  sce 2, sat 0x31, Rechteck x -20400 B 1500 / z -4900 T 2200, Ziel (-26214,0,-3861) Stage 0 Raum 0x03 (= ROOM1031) Cut 8.
  Steht in main00 ohne Bedingung. Gegenstueck ROOM1031 main00 @0x01D18 Slot 2 -> Raum 0x04.
- ROOM1051 main00 @0x00B5A `3b 04 02 31 00 00 3c 41 94 c6 e8 03 d0 07 58 66 c0 c7 e0 60 00 08 00 0a 00 08 ..` = Slot 4,
  Rechteck x 16700 B 1000 / z -14700 T 2000, Ziel (26200,-14400,24800) Raum 0x0A (= ROOM10A1) Cut 0. Ohne Bedingung; kein
  sub setzt Slot 4 um (sub01 @0x00CBA Aot_reset Slot 11, sub02 @0x00CDA Slot 7). Gegenstueck ROOM10A1 main00 @0x00DB2.
- ROOM10B1 main00 @0x016F2 `3b 01 02 31 00 00 98 a9 e8 0d dc 05 34 08 e6 b0 f8 f8 d4 e5 00 04 00 26 0f 01 ..` = Slot 1,
  Rechteck x -22120 B 1500 / z 3560 T 2100, Ziel (-20250,-1800,-6700) Raum 0x26 (= ROOM1261 "Sewer Passage") Cut 15;
  Port-Uebergang re15_tuer_eigen.inc:190 `P16M (Basis DOOR16) V5 (hinab)` = Leiter hinunter, im Bild ein Kanaldeckel.
  Der zweite Uebergang @0x016D2 Slot 0 -> Raum 0x17 (ROOM1171) Cut 3, Band 5, ist die Leiter zum Dach
  (tuer_zuordnung.inc:37 `DOOR16 V4 (hinauf)`, Runde 31 Kategorie "leiter"). 10B1 hat keine weiteren Uebergaenge.

"Die Tuer, die sich bei der Sherry-Suche oeffnet" (ROOM10B1; Flag-Zensus aller RDTs Bank 3 Bit 0x80..0x83):
- ROOM1171 sub02 @0x0139C `22 03 80 01` / @0x013A4 `22 03 82 01` (Abstieg vom Dach mit John) -> ROOM10B1 sub00 @0x017F6
  Ck(3,0x82)==1: John wartet am Deckel (Pos_set @0x017FE (-21422,0,3307)); sub01 @0x01816 Ck(3,0x82)==1 + @0x0181E Cmp
  (Cut == 7) -> @0x01824 Evt_exec sub04 = die Szene msg 0..5 ("There's still a little girl trapped in there! ... We can't
  just leave her behind!" / "...we can go through the sewer tunnels. Follow me."); sub04 @0x0195A Set(3,0x82)=0,
  @0x0195E Set(3,0x83)=1, @0x01962 Set(3,0x41)=1, @0x01966 Set(2,7)=1, @0x0196A Set(1,0x1B)=1, ..., @0x01A84 Plc_dest John
  zum Deckel, @0x01A8E `47 01` Aot_on Slot 1 = der Uebergang durch den Deckel, @0x01A90 Evt_end. KEIN Set(2,7)=0 in sub04:
  Pad-Sperre und Balken loest im Original erst der Raumwechsel.
  Danach ROOM1261 sub03 (3,0x41) John je Cut, ROOM2001 "Elza: Okay, I'll go for her." @0x01958 (3,0x46)=1
  (analysis/story_leon_elza/ablauf_elza.md E18-E22).
- => Die einzige Stelle in 10B1, an der sich etwas oeffnet, ist Slot 1 (Aot_on @0x01A8E) in der Szene, in der Elza nach
  Sherry suchen will; Slot 1 ist zugleich der einzige Uebergang, der in 10B1 als Tuer dient (Slot 0 ist die Leiter).
  "Die Tuer im ROOM 10B1" = Slot 1 (Kanaldeckel -> ROOM1261).

Form "Tuer ist immer zu" im Original:
- RE1.5 (Elza) ROOM11A1 main00 @0x00F9C Door_aot_set Slot 2, unmittelbar danach @0x00FBC `46 02 01 31 01 00 ff ff 00 00`
  = Aot_reset Slot 2 -> sce 1 MESSAGE msg 1 "It's locked." (@0x01430 `04 02 25 50 3a 4f 00 48 4b 3f 47 41 40 57 01 00`),
  Maske 0xffff, OHNE Bedingung = eine Tuer, die im Raumaufbau jedes Mal zur Meldung wird.
- RE1.5 ROOM1041 selbst main00 @0x01076..@0x01096: Ifel Ck(4,0xF3)==0 -> @0x0107E Aot_set Slot 0 sce 1 MESSAGE msg 1
  (Aufzug ohne Strom) mit dem Rechteck der Tuer, sonst Door_aot_set Slot 0 = Tuer als Meldung, solange ein Flag steht.
- Aot_on auf einen MESSAGE-Satz ruft den Text: LAB_800407bc @0x8004082c `jalr` PTR_8007469c[rec[0]], [1] = 0x80043084
  (Port aot_common.c re15_aot_fire_slot case MESSAGE -> re15_scd_show_message). Damit zeigt die Szene den Text statt des
  Raumwechsels, ohne dass ein Byte der Szene geaendert wird.
- Gosub-Huelle um eine Original-Szene: Spur O Programm SZENE/ENDE (Gosub @0x8003fc04, Runde 38 F SZENE1021); ein
  Evt_end im per Gosub gerufenen sub kehrt zum Rufer zurueck.
- Szenen-Ende in RE1.5-Form (Elza-Raum ROOM1051 sub02 @0x00D90..@0x00D9E): `22 02 07 00` Set(2,7)=0, `22 01 1b 00`
  Set(1,0x1B)=0, `3c 01` Cut_auto 1, `2e 01 00 00` Work_set(1,0) + Nop, `42 00` Plc_ret + Nop (dieselbe Folge in ROOM1041
  sub11 @0x0222C..@0x0223B).

Sperrtext 10B1: RE1.5 hat KEINEN Text fuer einen geschlossenen Deckel (msggrep "lid|manhole|cover|hatch|won't open":
nur Tueren "It's locked."/"The door won't open!" und ROOM3000/3001 msg 0 "A ladder leads down the manhole. / Descend it?").
Beta -> Retail: RE2 hat genau diesen Fall - ROOM21A0 Slot 2 = Deckel mit Leiter (msg 3 "Will you go down the ladder?"),
MAIN sub03 @0x01EA4 `46 02 04 31 01 00 00 00 ff ff` -> msg 1 " / The lid is firmly closed.<fd> I might be able to open it /
with the right tool..." und im Else-Zweig @0x01EC6 `46 02 04 31 04 00 00 00 ff ff` -> msg 4 @0x02963 " / The lid is firmly
closed.<fd> It's impossible for me / to open it." (die Figur, die ihn nie oeffnen kann). Fuer "ausnahmslos IMMER" = msg 4.
Umsetzung in RE1.5-Kodierung: `04 02` Erzaehlform, `08` leere erste Zeile (wie RE2 `fc` und RE1.5 ROOM3020 msg 6 @0xF35
`04 02 08 25 50 3a 4f ..` " / It's locked."), `02 00` Seitenwechsel fuer RE2 `fd` (RE1.5 ROOM3091 msg 6 @0x1FC6 "The door
is latched shut and won't open." `02 00` "I'd need a key ..."), `01 00` Ende. Kein Tuer-Ton: RE2 spielt Se 0x0216 nur im
TUER-Handler @0x800516a4 (key_id-Schloss, include/re15_lock_se.h); der Deckel ist dort ein MESSAGE-Satz.

Satz an der Tuer 1041 -> 1031 / 1051 -> 10A1 (NUTZER-VORGABE "I have to rescue John! He is in danger!"):
- Form wie die Tuersperren 1141 (Runde 38 F msg 41) und 11C0 (Spur O msg 40): Erzaehlform `04 02 <Zeile> 08 <Zeile> 01 00`
  ohne Sprecherkopf. Original: RE2-Zensus Spur O (O_belege/re2_tuer_msg_zensus.txt, 54 Stellen): JEDE Tuer, die RE2 durch
  einen Text ersetzt, ist ein MESSAGE-Satz in Erzaehlform - das naechste Vorbild ist ROOM4040 MAIN sub03 @0x00E9E/@0x00EB0
  (Tueren 1/0 -> msg 9 "I have to catch / that woman!": Leons Ziel, das ihn an der Tuer zurueckhaelt), RE1.5 ROOM1090 Slot 2
  @0x0230E Aot_set sce 1 -> msg 7 @0x28E6 `04 02 ..` "I must hurry up and get something to / put out this fire to save that
  woman!". Die Sprecherform "Elza: ..." (msg 40 in 1041, Form ROOM1041 msg 2) gehoert den Szenen-Dialogen (Message_on);
  an einem MESSAGE-Satz hat weder RE1.5 noch RE2 einen Sprecherkopf.
- Zeitpunkt: in der Panikphase. Beginn = Set(9,97)=1 in RUF1041 (Elza steht dabei in 1041) -> dort in derselben Szene vor der
  Rueckgabe der Steuerung, wie RE2 ROOM7040 sub06 @0x0185A `46 00 04 31 01 00 00 00 ff ff` (Spur O, Programm SZENE);
  spaeteres Betreten von 1041/1051 in der Phase oder Laden = Raumaufbau (Form RE1.5 ROOM1041 main00 @0x01076 /
  RE2 ROOM4040 sub03: Bedingung -> Tuer als Meldung). Ende der Phase = Johns Rettung in 1141 (Bit 97 = 0); 1041/1051 werden
  danach neu aufgebaut (main00 Door_aot_set) -> wieder Tueren.
- Stimme: MESSAGE-Saetze reihen im Port keine Stimme ein (scd_vm.c re15_scd_show_message); fuer die Nutzer-Zeilen gilt die
  Ordnerregel ueber denselben Haken wie Spur O (11C0 msg 40): synchro/STAGE1/room1041/main41.wav, synchro/STAGE1/room1051/
  main40.wav, sobald sie liegen.

### 8.3 Umsetzung
Dateien (NEU): `re15_port/include/re15_tuersperre_r39.h` (alle Konstanten mit Beleg), `re15_port/engine/src/tuersperre_r39.c`,
`re15_port/tools/r39_johnruf/tuersperre_bauen.py` -> `re15_port/engine/src/gen/tuersperre_r39.inc` (Texte, Programm
SZENE10B1, FNV-Anker; `--pruefen` vergleicht). Haken: `engine/src/johnruf_r39.c` (eigene Datei: Installer ruft
`re15_tuersperre_r39_install` vor allen Raum-Weichen, der Spielbild-Tick zuerst `re15_tuersperre_r39_tick`),
`engine/src/scd_vm.c` je 1 Zeile (Include; op_evt_exec hinter Spur O; re15_scd_show_message hinter Spur O), alle mit
"Runde 39 Spur P". KEIN Asset-Patch, keine neue Datei unter shared_assets/ oder synchro/.
| Sperre | Raum / Slot (Beleg) | Bedingung (Form) | Text |
|---|---|---|---|
| 1041 -> 1031 | ROOM1041 Slot 1, main00 @0x010B8, Ziel 0x03 (@0x010CF) | Bank 9 Bit 97, je Spielbild VOR dem AOT-Scan (ROOM11B1 sub01 @0x0119C/@0x011C0) | msg 41 "I have to rescue John! / He is in danger!" |
| 1051 -> 10A1 | ROOM1051 Slot 4, main00 @0x00B5A, Ziel 0x0A | Bank 9 Bit 97, je Spielbild | msg 40, derselbe Satz |
| 10B1 Kanaldeckel -> 1261 | ROOM10B1 Slot 1, main00 @0x016F2, Ziel 0x26 | immer (ROOM11A1 main00 @0x00FBC ohne Bedingung) | msg 40 = RE2 ROOM21A0 msg 4 " / The lid is firmly closed. [Seite] It's impossible for me / to open it." |
Satzform jeder Sperre = Aot_reset(slot, sce 1, sat 0x31, msg, Maske 0xffff, 0) (`re15_aot_retype`, LAB_80040738); Ziel-
Raum/Cut bleiben im Satz (nur sce + 3 Halbworte). Gesperrt wird nur, wenn der Slot noch die ausgelieferte Tuer ist und der
FNV-Anker des Door_aot_set (bei 10B1 zusaetzlich sub01..sub04 @0x01812..@0x01A92) stimmt. Leon (1040/1050/10B0) wird nie
angefasst (Tabelle nur 0x1041/0x1051/0x10B1). Stimme: 1041/41 und 1051/40 reihen im MESSAGE-Weg `SCD_AUDIO_VOICE_ON` mit der
Id ein (Ordnerregel, Muster Spur O); der Deckel-Text bleibt stumm wie in RE2.
Programm SZENE10B1 (22 B, Listing in gen/tuersperre_r39.inc): `18 04` Gosub sub04 (Szene unveraendert, ihr Aot_on @0x01A8E
trifft den gesperrten Slot -> Text) | `02 00` Evt_next+Nop (ROOM1051 sub02 @0x00CCC; die VM parkt unter RE15_PAUSE_SCD, bis der
Text zu ist, FUN_8003f038 @0x8003f040-4c) | `22 02 07 00` `22 01 1b 00` `3c 01` `2e 01 00 00` `42 00` (ROOM1051 sub02
@0x00D90..@0x00D9F: Pad frei, Balken weg, Cut_auto, Work_set(1,0), Plc_ret) | `01 00` Evt_end (@0x00DA2). Noetig, weil sub04
@0x01966 Set(2,7)=1 / @0x0196A Set(1,0x1B)=1 setzt und nie loest (gemessen: unit tuerszene "ohne Huelle").
Dateizahl: 3 neue Quelldateien + Werkzeug + gen-Datei; Haken scd_vm.c +5 Zeilen (3 Stellen), johnruf_r39.c +3.

### 8.4 Messung nachher (echte exe, Elza, Spielstand + CONTINUE, echte Tueren; Auszuege P_belege/nb1_nachher_*.txt)
| Lauf | Zustand | debug.log | Befund |
|---|---|---|---|
| n3 | 1041, Bit 97 = 1 | `[tuersperre] F0 ROOM1041: Panikphase (Bit 97) -> Tuer Slot 1 -> Text msg 41`, an der Tuer `[msgtext] room=1041 id=41 "I have to rescue John! He is in danger!"`, `[voice] ROOM1041 msg 41: keine Aufnahme (synchro/STAGE1/room1041/main41.wav)`; kein DOOR FIRE, zweite Aktion schliesst den Text, Elza frei (Wegplan bis Befehl 12) | Satz statt Raumwechsel |
| n3o | 1041, Bit 97 = 0, (4,0xF0) = 0 (wie nach der Rettung) | `[aot] DOOR FIRE slot=1 ... target_cut=8` -> `loaded room1031.rdt`, keine [tuersperre]-Zeile | Tuer normal |
| n4 | 1051, Bit 97 = 1 | `ROOM1051: Panikphase ... Slot 4 -> Text msg 40`, `[msgtext] room=1051 id=40 "I have to rescue John! He is in danger!"`, `[voice] ROOM1051 msg 40: keine Aufnahme (synchro/STAGE1/room1051/main40.wav)` | Satz statt Raumwechsel |
| n4o | 1051, Bit 97 = 0 | `DOOR FIRE slot=4` -> `loaded room10a1.rdt` | Tuer normal |
| n6a | 10B1 vor der Szene | `ROOM10B1: Kanaldeckel Slot 1 -> Text msg 40 (immer zu)`, `[msgtext] room=10b1 id=40 " The lid is firmly closed.It's impossible for me to open it."`, kein Raumwechsel | zu |
| n6b | 10B1 mit (3,0x82) (Sherry-Szene) | `Evt_exec sub04 -> Programm SZENE10B1`, msg 0..5 mit main00..05.wav, Text msg 40 in Bild 733, nach dem Schliessen (RE15_PRESS) Wegplan: `FLAG (2,7) = 0`, `FLAG (1,27) = 0`, pm=0, Elza laeuft (-20303,1579) -> (-20683,2792); kein `loaded room1261.rdt` | zu, Steuerung zurueck |
| integration_r39_johnruf1041_a (echter Weg 1021 -> 1041) | Beginn der Phase in 1041 | `F511 Karte zu ... Set(9,97)=1` -> im SELBEN Bild `F511 [tuersperre] ROOM1041: Panikphase (Bit 97) -> Tuer Slot 1 -> Text msg 41` und `Panik-Musikfenster auf` + `entry=FF01 -> MAIN01`; Elza geht zur Tuer nach 1031: `F968 ... msg 41 "I have to rescue John! He is in danger!"`, kein `loaded room1031.rdt`; danach Treppe -> 1061 (MAIN01 laeuft durch) | "beim Musikwechsel" belegt |
Bilder: `P_belege/nb1_nachher_tuertexte_1041_1051.png` (Textkasten an beiden Tueren, Erzaehlform ohne Sprecherkopf),
`P_belege/nb1_nachher_n6b_deckeltext.png` (Ende der Sherry-Szene: John am Deckel, Text Seite 1/2, danach Balken weg).

### 8.5 Tests (Nachbesserung 1)
Neu in `tests/unit/test_r39_johnruf1041.c` (registriert in probes/r39_johnruf1041.cmake; `room_boot` setzt jetzt
`g_room_rdt` wie der Spielweg, damit die Anker greifen):
| Test | misst |
|---|---|
| unit_r39_johnruf1041_tuertexte | Satz = Nutzer-Wortlaut, 1041 = 1051, Erzaehlform ohne Sprecherkopf; 10B1-Text = RE2 ROOM21A0 msg 4 zur Laufzeit aus der RE2-Datei umkodiert (Tabelle Eintrag 4 -> @0x02963); Ids 41/40/40 hinter den Original-Ids (1041: 4, 1051: 6, 10B1: 6); Stimme nur 1041/41 und 1051/40 |
| unit_r39_johnruf1041_tuer1041 | ohne Phase Aktion -> Raumwechsel ROOM1031; in der Phase Slot 1 = MESSAGE 41 (sat 0x31, Maske 0xffff, Ziel bleibt), Aktion -> Text + Stimme, kein Raumwechsel; RUF1041 setzt Bit 97 -> Tuer im selben Spielbild Text (Bild 543 = 543), 0 freie Bilder mit offener Tuer; Leon 1040 Tuer |
| unit_r39_johnruf1041_tuer1051 | dasselbe fuer Slot 4 / ROOM10A1; Leon 1050 (Ada gerettet, Bit 97 = 1) Tuer |
| unit_r39_johnruf1041_tuer10b1 | vor der Szene / Szene steht an / nach der Szene (Sherry-Suche) / Sherry-Flags + Bit 97: Deckel = Text 40, Leiter Slot 0 -> 0x17 bleibt Tuer, Aktion -> Text, kein Raumwechsel, keine Stimme; bei (3,0x82) Evt_exec sub04 -> Huelle; Leon 10B0 -> ROOM1260 |
| unit_r39_johnruf1041_tuerszene | Programm bytegleich (Gosub sub04 + ROOM1051 sub02 @0x00CCC/@0x00D90..@0x00D9F/@0x00DA2); MESSUNG ohne Huelle: Text kommt, aber (2,7)=1/(1,0x1B)=1 bleiben, Elza eingefroren; mit Huelle: Text statt Raumwechsel, danach (2,7)=0, (1,0x1B)=0, frei, Programm zu Ende, (3,0x83)/(3,0x41) gesetzt |
Neu `tests/integration/test_r39_johnruf1041_tuer.cmake` (TEIL t1041, t1041o, t1051, t1051o, t10b1v, t10b1s, t10b1n; exe-Kopie
`re15_pc_r39pt_<teil>_<zufall>`, eigener Laufordner, RE15_*-Umgebung nicht geerbt, Stimmen-Pruefung haelt auch, wenn die
Aufnahme spaeter liegt). `integration_r39_johnruf1041_a` macht jetzt den Umweg ueber die Tuer nach 1031 (Text nach dem
Musikwechsel, kein Raumwechsel), Rest unveraendert.
- `ctest --test-dir re15_port/build -R "integration_r39_johnruf1041_tuer|integration_r39_johnruf1041_a$" --timeout 1800 --output-on-failure -j 4`
  -> "100% tests passed, 0 tests failed out of 8" (t1041 ~30 s, t10b1s 67,75 s, a 82,93 s).
- Unit-Teile direkt: tuertexte 9/9, tuer1041 9/9, tuer1051 7/7, tuer10b1 6/6, tuerszene 6/6; die alten Teile texte 2, programme 13,
  stimme 9, xa 6, ablauf 16, musik 16, ruf1131 7, leon 2 - alle 0 Fehler.

### 8.6 Je Mangel
- **Mangel 1 (Tuer 1041 -> 1031 / 1051 -> 10A1 in der Panikphase).** Ursache: neue Nutzer-Vorgabe (Nachtrag 12), bisher keine
  Sperre. Vorher v3/v4: Raumwechsel in der Phase. Beleg: Slots @0x010B8/@0x00B5A, Form ROOM11B1 sub01 @0x011C0 (je Bild unter
  Flag, Elza-Raum), Text-Form RE1.5 ROOM1090 msg 7 / RE2 ROOM4040 msg 9 (Erzaehlform, kein Sprecherkopf an einem MESSAGE-Satz).
  Aenderung: tuersperre_r39.c (Tick + Installer), Texte 41/40. Nachher: n3/n4 + integration t1041/t1051 Text, kein Raumwechsel;
  n3o/n4o + t1041o/t1051o ohne Phase Raumwechsel; integration a: Sperre im Bild des Musikwechsels (F511), Text an der Tuer F968.
- **Mangel 2 (ROOM10B1 immer zu).** Ursache: neue Nutzer-Vorgabe. Vorher v6a/v6b/v6c: der Kanaldeckel fuehrt in jedem Zustand
  nach ROOM1261, die Sherry-Szene oeffnet ihn selbst (Aot_on @0x01A8E). Welche Tuer: Slot 1 (einziger Uebergang, der als Tuer
  dient und sich in der Sherry-Szene oeffnet; Slot 0 = Leiter zum Dach, bleibt). Beleg: Form ROOM11A1 main00 @0x00FBC, Text RE2
  ROOM21A0 msg 4 @0x02963 (geschlossener Deckel), Aot_on -> MESSAGE @0x8004082c/@0x80043084, Szenen-Ende ROOM1051 sub02.
  Aenderung: Sperre im Raumaufbau + Huelle SZENE10B1. Nachher: n6a/n6b + integration t10b1v/s/n: Text, kein Raumwechsel, nach
  der Szene Steuerung zurueck; Leon 10B0 unveraendert (unit).
- **Mangel 3 (Merge + Bau + Riegel).** 57e5f920, configure/build LOCAL-BUILD-OK; gezielte Laeufe Abschnitt 8.10.

### 8.8 OFFEN (Nachbesserung 1)
- Spur Q (elzazombies, parallel) arbeitet laut Nachtrag 11/12 auch in ROOM1041 (Durchbruch durchs Rolltor) und liest Bit 97.
  Die Tuersperre 1041 -> 1031 greift nur, solange Slot 1 die ausgelieferte Tuer ist (FNV-Anker @0x010B8 + Typ DOOR); setzt Q
  Slot 1 selbst um, bleibt Qs Fassung. Bei der Integration im gemeinsamen Lauf pruefen (integration_r39_johnruf1041_a und
  _tuer_t1041).
- 10B1: Nach der Sherry-Szene steht John am Deckel (Plc_dest sub04 @0x01A84 (-21422,4607)) und bleibt dort, bis Elza den Raum
  verlaesst; beim naechsten Betreten ist er weg ((3,0x82) = 0, Original-Flags). Der Weg weiter zu ROOM1261/ROOM2001 (John:
  "Elza, over here!", Aufnahmen room2001/main00..05) fuehrt nicht mehr durch 10B1, sondern ueber den Keller (ROOM11B1 Slot 5
  bzw. ROOM11D1 Slot 1 -> ROOM1261); (3,0x41) setzt die Szene vor dem Deckel selbst (sub04 @0x01962), ROOM2001 sub00
  @0x0179A `21 03 41 01` Ck(3,0x41)==1 -> @0x0179E Sce_em_set John. Diesen Umweg habe ich nicht gemessen.
- Fremde Riegel: siehe 8.10 (integration_r38_wasser Lauf elza10b1_tuer entfaellt; integration_r39_hund1121 Spielstaende mit
  4:0xF0). Spur Q: Spielstaende mit 9:97 brauchen ebenfalls 4:0xF0.
- Die Sprachdateien fuer die Tuer-Saetze liegen noch nicht; ohne Datei steht im debug.log
  "[voice] ROOM1041 msg 41: keine Aufnahme ..." und die Zeile bleibt stumm (Riegel halten auch mit Datei).

### 8.9 Fuer den Nutzer (Nachbesserung 1)
- In der Panikphase (ab Johns Hilferuf in 1041 und dem Musikwechsel, bis John in 1141 gerettet ist) zeigt die Tuer von ROOM1041
  nach ROOM1031 und die Tuer von ROOM1051 nach ROOM10A1 den Satz "I have to rescue John! He is in danger!" (als Gedanke wie
  die Sperren in 1141/11C0, ohne "Elza:"), statt Elza durchzulassen. Ab dem Bild, in dem MAIN01 beginnt. Nach Johns Rettung
  gehen beide Tueren wieder normal.
- Sprachdateien fuer diesen Satz (werden automatisch gespielt, sobald sie liegen):
  `synchro/STAGE1/room1041/main41.wav` und `synchro/STAGE1/room1051/main40.wav`.
- ROOM10B1: "Die Tuer" ist dort der Kanaldeckel in der schmalen Gasse (der zweite Ausgang ist die Leiter zum Dach, die bleibt
  offen). Der Deckel ist fuer Elza jetzt immer zu: "The lid is firmly closed. / It's impossible for me to open it." (der Text,
  den Resident Evil 2 an einem Deckel zeigt, der sich nie oeffnen laesst). Das gilt auch in der Szene "John, wait! ... Follow
  me.": John geht noch zum Deckel, dann kommt dieser Text, danach hast du Elza wieder. Leon (ROOM10B0) ist unveraendert.
  Folge: der Weg in die Kanaele (ROOM1261 und weiter nach ROOM2001 zu deinen Aufnahmen room2001/main00..05) geht nicht mehr
  ueber 10B1, sondern nur noch ueber den Keller (Garage 11B1 oder 11D1 -> 1261).
- Neue Assets fuer das Paket-Gate: keine (Texte und Programm stehen im Code: engine/src/gen/tuersperre_r39.inc).

### 8.10 Gezielte Laeufe am Endstand (keine volle Suite im Spurbaum)
Bau: `local_build.sh configure` + `build` -> LOCAL-BUILD-OK (nach jeder neuen Datei neu konfiguriert).
- ALLE Unit-/Prop-/Sonden-Riegel:
  `ctest --test-dir re15_port/build -R "^(unit_|prop_|probe_)" --timeout 600 -j 4`
  -> "100% tests passed, 0 tests failed out of 889" (Total Test time 1339,76 s).
- Fremder Riegel integration_r38_wasser (Runde 38 Spur N): VORHER am neuen Stand rot - Lauf elza10b1_tuer
  `re15_pc exit=Process terminated due to timeout`; im Lauf-debug.log `[tuersperre] F250 ROOM10B1: Kanaldeckel Slot 1 -> Text
  msg 40 (immer zu)`, kein DOOR FIRE, kein `loaded room1261.rdt` (= genau die neue Nutzer-Vorgabe; der Lauf wartete auf
  EXIT_AT 150#1261). Die 15 Pruefungen der anderen Laeufe waren gruen. Aenderung: Lauf elza10b1_tuer entfaellt (Kommentar mit
  Grund und Verweis auf integration_r39_johnruf1041_tuer_t10b1v/_s/_n; Elzas Wasserschritt bleibt in elza1261, der Tuerweg
  ins Wasser in leon10b0_tuer), Kopf- und Registrierungskommentar nachgezogen. NACHHER:
  `ctest --test-dir re15_port/build -R "^integration_r38_wasser$" --timeout 1800 --output-on-failure`
  -> "100% tests passed, 0 tests failed out of 1".
- Integration der beruehrten Module (Leons MAIN01-Fenster, 1141 J/F, Stimmen A, Elza-Toene, Tagebuch/Karte, Funk 10F1,
  Heliport 1171 -> 10B1 K/I, Parkplatz O, Hund R, alle P-Riegel):
  `ctest --test-dir re15_port/build -R "integration_r39_johnruf1041|integration_r39_hund1121|integration_r39_parkplatz11c0_|integration_r39_heliport1171|integration_r38_heliport1171|integration_r35_cut10f0|integration_r39_dining1141|integration_r38_dining1141|integration_r38_elzatoene|integration_r38_tagebuch|integration_r39_elza_karte|integration_r39_stimmen|integration_r39_funk10f1" --timeout 3600 --output-on-failure -j 4`
  -> "98% tests passed, 1 tests failed out of 43" (1249,50 s). Rot nur integration_r39_hund1121 Teil E - ein
  INTEGRATIONSBEFUND P+R (nicht aus dieser Nachbesserung): debug.log `[johnruf] F0 ROOM1061 John gerettet ((4,0xF0)=0,
  (4,0xF2)=0): Bit 97 = 0`, hund.log `F40 [hund1121] ROOM1121 aus: (9,97)=0` (P_belege/nb1_hund1121_E_vorher_*). Die
  Spielstaende der Spur R setzen Bit 97 OHNE (4,0xF0)=1; im Spiel setzt nur RUF1041 Bit 97, und das laeuft nur mit
  (4,0xF0)=1 (ROOM1021 sub02 @0x01FE0); (4,0xF0)=0 + (4,0xF2)=0 ist Johns Rettung (ROOM1141 sub02 @0x00C18, sub01
  @0x00C0C) - die Regel der Abnahme 0 ist richtig, der Spielstand war es nicht. Teil A/C bestanden nur, weil der Hund-Installer
  im Raumaufbau vor dem ersten Tick scharf schaltete. Aenderung (12f9f493): Laeufe A, C, E der Spur R tragen 4:0xF0 (Kommentar
  im Test). Danach `ctest --test-dir re15_port/build -R "^integration_r39_hund1121$" --timeout 1800 --output-on-failure`
  -> "100% tests passed, 0 tests failed out of 1"; die anderen 42 waren gruen (darunter integration_r38_dining1141_s 751,61 s,
  integration_r39_dining1141_d 421,19 s, integration_r35_cut10f0 298,95 s, integration_r39_heliport1171_weg 76,51 s).
- Pflichtgruppe am Endstand (nach beiden Riegel-Anpassungen), ein Lauf:
  `ctest --test-dir re15_port/build -R "integration_r39_johnruf1041|integration_r39_hund1121|integration_r39_parkplatz11c0_|integration_r38_wasser" --timeout 3600 --output-on-failure -j 4`
  -> "100% tests passed, 0 tests failed out of 17" (548,83 s; enthaelt integration_r38_wasserzombie ueber den Ausdruck).
- Hinweis an die Integration (Spur Q, parallel): Riegel, die die Panikphase per Spielstand herstellen, brauchen 9:97 UND
  4:0xF0 - sonst loescht die Rettungs-Regel Bit 97 im ersten Spielbild (genau der Befund von R Teil E).

### 8.11 Endstand Nachbesserung 1
Alle drei Maengel umgesetzt und gemessen: Merge master a0d7d749 (57e5f920); Tuer 1041 -> 1031 und 1051 -> 10A1 in der
Panikphase = "I have to rescue John! He is in danger!" (msg 41 / 40), ausserhalb der Phase normale Tueren; Kanaldeckel
ROOM10B1 fuer Elza immer zu (auch in der Sherry-Szene, danach Steuerung zurueck), Leon unveraendert. Bau LOCAL-BUILD-OK
("ninja: no work to do" am Endstand), Unit/Prop/Sonden 889/889, Integration 42/43 + hund1121 nach Testdaten-Fix 1/1,
Pflichtgruppe 17/17.

### 8.12 Nachbesserung 2 (Orchestrator selbst, 2026-10-19 ab 12:05; Sparmodus, keine weitere Abnahme)
Grund: Abnahme 1 (P_abnahme_1.md) M1 und M2.
- M2 (Nachtrag 13: der Tuersatz ist ein einfacher Raumtext wie Leons Tuertexte): der Stimmen-Haken entfaellt ganz
  (re15_tuersperre_r39_stimme und sein Aufruf in scd_vm.c re15_scd_show_message). 1041 msg 41 und 1051 msg 40 zeigen nur
  den Text. Die Bitte in 8.9 um synchro/STAGE1/room1041/main41.wav und room1051/main40.wav ist HINFAELLIG - es gibt
  keine Aufnahme fuer diese Saetze. Riegel: unit tuer1041/tuer1051 (Stimmen mit Id 41/40 = 0 beim echten Tuerdruck),
  integration t1041/t1051 (kein "[voice] ROOM1041 msg 41:" bzw. "[voice] ROOM1051 msg 40:" im Protokoll).
- M1 (Softlock): der Kanaldeckel ROOM10B1 ist ZURUECKGESTELLT - kein Eintrag mehr in k_sperren, 10B1 laeuft wie
  ausgeliefert (die Sherry-Szene oeffnet den Deckel nach ROOM1261, sub04 @0x01A8E 47 01). Begruendung im Code-Kommentar
  (einseitiger Abstieg ROOM10B1 SCA @0x00EE4, Aufstiegsregel @0x8002d544/@0x8002d548). Die Tests tuer10b1/tuerszene und
  t10b1v/t10b1s/t10b1n sind nicht mehr registriert (Quelle bleibt fuer die Entscheidung des Nutzers); der Lauf
  elza10b1_tuer in integration_r38_wasser ist wieder da (Revert von 01e7fb4b). Entscheidung beim Nutzer: welche Tuer
  in 10B1 gemeint ist bzw. wie Elza nach der Szene weiterkommt.
