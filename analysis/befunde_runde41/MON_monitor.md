# Runde 41 Spur Monitor — Hangar-Szene als Monitorbild im Monitor Room (Runde 1 + 2)

Zweig `r41/sichel_szene`, Basis 23f9da5c9 (abgenommener Stand der Sichel-Szene). Code `engine/src/monitor_r41.c`,
`include/re15_monitor_r41.h`; Haken in `birkin_formen.c`, `room_common.c`, `re15_map_zones.c`, `platform/pc/src/render_pc.c`.

Nutzer 2026-11-15 (Wortlaut): "Das einzige ist - diese Szene wo der Birkin den Spiderman schmeisst, ist 100%ig kein Boss
Kampf. Sie wirkt nicht so. Ich wuerde eher sagen, das Leon oder Elza Birkin auf dem Monitor sehen mit der Szene, wo auch
immer er danach hin laeuft."

## 1. Original-Bausteine (Belege)

### 1.1 Monitor-Bild-Effekt (Zone 1 Bit 0x1E)

PSX.EXE (info/Re1.5/PSX.EXE, Datei-Offset = Adresse - 0x8000F800), selbst gelesen (ghidra1_V2.txt):

    8001d1d8  jal  FUN_80020610                       je Bild aus der Hauptschleife
    80020614  lw   v1,DAT_800aca3c                    Zone 1 (Bit 0x1E = 0x80000000 >> 30 = 2)
    8002061c  andi v0,v1,0x8   -> FUN_80020674(1)     Bit 0x1C (Wackeln, Tabelle @0x80072e70)
    80020638  andi v0,v1,0x4   -> FUN_80020674(2)     Bit 0x1D (Tabelle @0x80072e90)
    80020640  andi v0,v1,0x2   -> 8002065c jal FUN_80020794 (a0 = 0, a1 = 0)
    FUN_80020794:
    800207cc  lbu  v1,DAT_800b5576 ; 800207d4 lbu a2,DAT_80072eb8 (=2) ; 800207e0 sb    Phase += 2 je Bild
    800207e8  jal  FUN_800209ec(0,7)                  OT 0 Eimer 7 (Letterbox: Eimer 4 @0x800aa6a8, FUN_80021a0c)
    800207f4  lw   v0,DAT_80072eb4 (=2) ; 800207fc addu ; 80020800 andi 0xff         s1 += 2 je Zeile
    80020810  lb   a2,DAT_80072d70(s1)               Sinus 256 x int8 (Datei 0x63570: 3,6,9,...,127,...,-127,...,0)
    80020818  lw   v0,DAT_80072eb0 (=0x40) ; 80020820 div ; 80020848 mflo              Versatz = Sinus/64
    80020858  jal  FUN_80020894(zeile, 0, versatz)  0x18-Byte-Kopie (DR_MOVE) Quell-Zeile zeile+versatz,
                                                     versatz 0 -> keine Kopie; < 0 an den Kettenkopf, sonst ans Ende

Zensus aller RDTs (`analysis/story_leon_elza/census.json`): Set(1,0x1E) gibt es genau zweimal, ROOM5031 sub03 @0x011A8 = 1
(vor Cut 11, dem Sherry-Monitorbild) und @0x011E6 = 0 (danach). Bit 0x1E ist damit der Monitor-Effekt des Spiels. Der
Port hatte FUN_80020610 nicht (auch Elzas Sherry-Bild lief bisher ohne ihn). Jetzt: `re15_monitor_r41_zeilen` (Tabelle,
Reihenfolge der Kopien) + `pc_monitor_zeilen` in render_pc.c nach Hintergrund + Figuren, vor Blenden/Letterbox/Text.
Nicht gebaut (unveraendert offen): die Zweige 0x1C/0x1D (FUN_80020674, Wackeln in 1080/2040/20B0/3080/4020/5090/6030).
Der Raumlader loescht die unteren 16 Bit von Zone 1 (FUN_800396fc @0x80039710-30, scd_room_setup.c) - das Bit wird
deshalb erst nach dem Laden des Hangars gesetzt.

### 1.2 Kamerafuehrung (ROOM5031 sub03, scd_annot/ROOM5031.txt)

Set(2,7) @0x01142, Cut_chg 7 @0x01194, Sleep 0x14 @0x01196, Cut_chg 11 + Set(1,0x1E)=1 @0x011A6/@0x011A8, Set(1,0x1E)=0
@0x011E6, Cut_chg 9 @0x011EA, Sleep 0x32 @0x011F8, Cut_chg 1 + Cut_auto 1 @0x01208/@0x0120A, Set(2,7)=0 @0x0120C.
Bilder der Schnitte (Framedump 23f9da5c9, RE15_FORCE_CUT): Cut 7 = ein Monitor formatfuellend, Cut 9 = Monitorwand mit
Pult, Cut 11 = entfernter Lagerraum (Sherry + Affe bei (14100..18200, 17050)).

### 1.3 Wege (Tuerkarte aus den RDTs, Door_aot_set)

| Tuer | Ziel | Bedingung |
|---|---|---|
| ROOM5000 slot 0 @0x007FE | 5030 (-13846,-5474) Gier 798 Cut 2 | Ck(3,0x4C)==0 (sonst @0x00822 -> 5110) |
| ROOM5030 slot 3 @0x00EC6 | 50A0 (-12117,0,4472) Gier 1024 Cut 0 | - |
| ROOM4070 slot 1 @0x011FE | 50A0 (-23878,-16268) Cut 11 | Ck(3,0x4C)==0 |
| ROOM50A0 slot 3 @0x0156A | 5030 (-15020,-25030) Cut 5 | - |

(3,0x4C) setzen Leon 4010 sub04 @0x00DAC (nach der T-Vakzine, L31) und Elza 5011 @0x0094C (E39). Danach sind Monitor
Room und Hangar nicht mehr erreichbar. Der Hangar ist vorher auch ueber 4070 erreichbar (West), dann ohne Birkin.

## 2. Bau (PORT-SZENE / PORT-WAHL)

- Ausloeser: erstes freies Bild in 5030/5031 (Zustand 1, nicht gegriffen, keine Szene, Pause-Wort & 0xFF000000 == 0,
  keine Blende, kein Raumwechsel offen), Bank-9-Bit 38 == 0, (3,0x4C) == 0.
- Ablauf: Set(2,7) + Cut 7 (Zonen aus) 20 Takte -> Bit 38 = 1, Raumwechsel in 50A0/50A1 (Lage der 5030-Tuer, Gier 1024,
  Eintritts-Schnitt 12) -> nach dem Laden Zonen aus + Set(1,0x1E) -> erster KI-Takt: Raumfiguren (Affen 0x29) aus, Set(2,7)
  -> zweiter Takt: Szene startet wie bisher (bf_hangar_starten) -> h135: Set(1,0x1E)=0, Raumwechsel zurueck an die
  gespeicherte Stelle, Eintritts-Schnitt 9 -> 50 Takte -> Schnitt des Spielers, Cut_auto 1, Set(2,7)=0.
- PORT-WAHL Letterbox: in 5030/5031 nur die Pad-Sperre (2,7), nicht (1,0x1B) wie @0x01146 - die Szene blendet ihre eigene
  Letterbox ab dem Start ein; mit (1,0x1B) waere der Balkenzaehler beim Start 0xF0 statt 0 und h0..h6 nicht bitgleich.
- PORT-WAHL Rueckkehr: Schnitt des Spielers statt "Cut 1" (Elzas Platz vor den Monitoren), der Spieler wird nicht bewegt.
- Hangar ausserhalb des Monitorbilds: `re15_bf_hangar_aktiv` = Monitorbild || Messhaken RE15_HANGAR_IM_RAUM (alter
  Ausloeser, nur fuer die Inhalts-Riegel schwanz_zusammen / sichel_form).
- Karte: waehrend des Monitorbilds markiert der Hangar keinen Besuch (`re15_map_zone_update` uebersprungen).
- Spider-Man-Leiche: entfaellt - er flieht in der Szene aus dem Bild (h80-h86), es bleibt nichts liegen.
- Bank-9-Bit 38: VERTRAG Runde 41 1.1 nachgetragen, k_port[] (test_r36_kartenbits.c, Puffer 128 wie r42/story).

## 3. Messungen

| Beweis | Lauf | Ergebnis |
|---|---|---|
| (a) Szenenbilder bitgleich | 23f9da5c9 (alter Ausloeser, Weg wie integration_r41_schwanz_zusammen) gegen Monitorbild mit RE15_MONITOR_OHNE_ZEILEN=1, Framedump je Videobild h (Messzeile [hangar-takt]) | h0..h134: 135 von 135 Bildern bitgleich (320x240). h135 ist im Monitorbild nicht gezeichnet (Raumwechsel im selben Takt) |
| (a') Determinismus der Referenz | 23f9da5c9 zweimal, Szenenstart F243 bzw. F255 | 136/136 bitgleich |
| (a'') Effekt = reine Nachbearbeitung | Monitorbild mit gegen ohne Effekt | 32400 Zeilen: 14742 gleich, 17525 um genau 1 Zeile versetzt, 133 anders - nur Zeilen 23/24/215 (Balkenkante; Letterbox nach dem Effekt, OT-Eimer 4 nach 7) |
| (b) Hangar leer | unit_r41_monitor "hangar": vor dem Monitor Room, nach dem Monitorbild, Wiederbetreten, nach Speichern/Laden, je alle Schnitte 0..11 | 0 Birkin, 0 Spider-Man, Leon und Elza |
| (c) genau einmal | unit + integration: 1 Monitor-Start, 1 Szenenstart je Lauf; kein zweiter im selben Besuch, beim Wiederbetreten, nach dem Laden | gruen; Gegenprobe RE15_KEIN_MONITORBILD / alte exe rot |
| (d) Sherry | integration Elza: Monitorbild beim Betreten, danach Gang zu den Monitoren -> sub03 (Cut 10/8/7/11/9/1), "Elza: Sherry!" msg 3 | gruen; Cut 11 jetzt mit dem Original-Effekt |
| (e) Stelle/Steuerung/Musik | Leon (-24936,0,-13000) / (-13846,0,-5474) Gier gleich vor und nach; Pad-Sperre 0, Zonen an, Spieler laeuft auf Eingabe; [bgm] MAIN29 "unveraendert, laeuft durch" bei beiden Wechseln | gruen |

Riegel: `unit_r41_monitor` (+ 4 Gegenproben), `integration_r41_monitor` (Leon, Elza, Gegenprobe), umgestellt
`unit_birkin_szenen` (Abschnitte 2/3/5/6), Messhaken in `integration_r41_schwanz_zusammen` / `integration_r41_sichel_form`.

Lauf der Gruppe `ctest -R "birkin|schwanz|spinn|sichel|hangar|monitor|kartenbits"` (Commit a56226458, eigener Bauordner):
37/37 gruen. `integration_r41_monitor` mit der exe von 23f9da5c9: rot (0 Monitor-Starts).

Videos (gdigrab, echtes Fenster): Leon 5000 -> Tuer -> 5030 -> Monitorbild -> Suedtuer -> Hangar leer (Affen fuer das Video per
Kill-Flag 8:42/8:43 aus, wie Spur H); Elza 5001 -> 5031 -> Monitorbild -> Sherry-Ereignis.

## 3b. Runde 2 (Pruefer SICHT: R1 Aufploppen, R2 kein sichtbarer Monitor-Look)

Befund der Pruefer (Runde 1): nach Schwarz + Einblende stand der Hangar 8 Bilder leer, dann ploppten Birkin und Spider-Man auf
(monitor_r41.c schaltete die Raumfiguren im 1. KI-Takt ab, bf_hangar_starten setzte die Szene erst im 2.); der Original-Effekt
(+-1 Zeile) ist mit blossem Auge nicht zu sehen, die Szene lief bildschirmfuellend wie vorher.

Aenderungen (monitor_r41.c Kopf Punkt 5, render_pc.c pc_monitor_look, room_common.c):
- Harter Schnitt wie ROOM5031 sub03 (Cut_chg 7 -> 11 @0x011A6, 11 -> 9 @0x011EA, keine Blende dazwischen): der Raumwechsel des
  Monitorbilds laeuft ohne FUN_800217b0/FUN_800216ec-Kick und ohne State-4/5-Freeze (re15_monitor_r41_harter_schnitt).
- Kein leerer Hangar: bis zum Szenenstart zeigt der Zeichner das gemerkte Bild von Cut 7 (Modus STANDBILD, 2 Bilder), das erste
  Bild danach hat Birkin und Spider-Man.
- PORT-WAHL Monitor-Rahmen: die Szene (fertiges Bild mit Original-Zeileneffekt, vor Letterbox/Text) laeuft verkleinert IN der
  weissen Bildschirmflaeche des Monitors aus Cut 7 - Bildzeilen (jede zweite 320x240-Zeile 72 %), 35 % entsaettigt, zum
  Schirmrand dunkler (Ecke 55 %). Kein Text, kein Logo. Flaeche gemessen am Framedump F25 ROOM5030/5031 Cut 7
  (Ecken (73.5,53.5) (233.5,53.5) (236.5,168.5) (70.5,168.5), Helligkeitsprofile im Kopf von k_schirm).
- Kein Vorbild in RE2: RE2 hat dieselbe Zeilen-Funktion (FUN_8002ab84, Sinus-Tabelle @0x8009d9f4 = 256 Bytes gleich RE1.5
  @0x80072d70, Datei-Vergleich), einziger Aufrufer FUN_8002aa6c @0x8002ab04 (Wackel-Zweig), keinen Monitor-Rahmen.
- Letterbox jetzt wie das Original: (1,0x1B) ab Cut 7 (@0x01146) bis nach Cut_auto (@0x01210); nach jedem Raumwechsel neu
  gesetzt (Raumlader loescht Zone 1 untere 16 Bit @0x80039710-30), am Szenenende im selben Takt wieder (kein Absacken). Die
  Pad-Sperre (2,7) hat keine Luecke mehr (Runde 1: zwischen Laden und erstem Monitor-Takt 0).
- h135 wird gezeigt: Rueckweg einen Takt nach bf_hangar_beenden (Phase ENDE).
- Sicherung (PORT-WAHL): Szene startet nicht binnen 60 Takten / Monitorbild laenger als 900 Takte -> zurueck ohne Szene.
- Messhaken RE15_MONITOR_OHNE_LOOK=1 (Alias RE15_MONITOR_OHNE_ZEILEN): kein Zeileneffekt, kein Rahmen, keine Letterbox vor
  der Szene.

| Beweis | Lauf | Ergebnis |
|---|---|---|
| (a) Szenenbilder bitgleich | 23f9da5c9 (git archive, eigener Build, alter Ausloeser im Raum) gegen HEAD mit RE15_MONITOR_OHNE_LOOK=1, Framedump je Videobild h, 320x240 | Leon 50A0 h-8..h135: 144/144 bitgleich; Elza 50A1: 144/144 (jetzt mit Vorlauf h-8..h-1 und h135). Referenz zweimal: h-8..h110 119/119 |
| R1 kein Aufploppen | Messzeile [monitor-bild] je Bild; gdigrab 30 Bilder/s | Hangar F0/F1 Standbild Cut 7, F3 erstes Szenenbild mit Birkin + Spider-Man (F2 gehaltenes Bild); im Video: Cut 7 (Schirm 230) -> naechstes Bild Szene im Schirm (49), kein Schwarz |
| R2 Monitor-Look | Framedumps + gdigrab 960x720 | Szene im Monitor sichtbar, Zeilen sichtbar (Messbilder mb_mo_l.png, mb_video_l.png im Arbeitsordner) |
| Rueckkehr | Framedump 5030 F0..F5 | F0 gehaltenes Monitorbild, F1 Cut 9 mit Letterbox, kein Schwarz |

Riegel neu: unit_r41_monitor "ablauf" (harte Schnitte, kein gezeigtes Hangar-Bild ohne Figuren, Rueckweg = Szenenende + 1 Takt,
Letterbox/Pad-Sperre ohne Luecke, Ladetakt HALTEN), Gegenprobe unit_r41_monitor_gegen_pop (RE15_MONITOR_OHNE_LOOK=1 -> rot);
integration_r41_monitor pruefe_pop (Leon + Elza) mit Gegenprobe P (RE15_MONITOR_OHNE_LOOK=1 -> rot); unit_birkin_szenen
Szenenende-Pruefung auf Phase ENDE umgestellt. Gruppe birkin|schwanz|spinn|sichel|hangar|monitor|kartenbits: 38/38 gruen.

## 4. Offen

1. PSX-Plattform: der Zeilen-Effekt ist nur im PC-Zeichner gebaut (wie die Region-Cull-Ausnahme der Szene).
2. FUN_80020674 (Bits 0x1C/0x1D) bleibt ungebaut (eigene Spur, betrifft 1080/2040/20B0/3080/4020/5090/6030).
3. (Runde 2 erledigt: Monitor-Rahmen als PORT-WAHL.) Elzas Original-Sherry-Bild (Cut 11) bleibt bildschirmfuellend wie im
   Original - der Rahmen gilt nur fuer das Port-Monitorbild.
4. Zweig r41/monitore (bdddfe887) animiert die Monitore in 5030/5031: das gemerkte Standbild von Cut 7 friert diese Animation
   waehrend des Monitorbilds ein; beim Zusammenfuehren ansehen (keine gemeinsame Datei).
