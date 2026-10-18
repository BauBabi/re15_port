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
- (laufend)

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
= Xa_vol 70 (derselbe RE2-Satz, anderer Zweig), 1141 unveraendert Standard 120; vor Elzas Zeile Xa_vol(120) (Form ROOM1010
sub22). Messung der Wirkung: Abschnitt 4.

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
| "Wenn wir uns den Flur 1131 naehern, ... please help ... etwas lauter" | Installer 1131: Bit 97=1, (4,0xF0)=1, Ruf in dieser Phase noch nicht -> RUF1131: Sleep 30, Xa_vol 70, help02, Sleep 90, Xa_vol 120 (keine Szene) | 2.3 RE2 Xa_vol 70 @0x01F7A |
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


## 5. Tests

## 6. OFFEN

## 7. Fuer den Nutzer
