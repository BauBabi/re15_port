# Runde 38 — Spur F "dining1141" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r38_dining1141` (Zweig `r38/dining1141`, Basis e2cc7673).
Punkte (Wortlaut AUFTRAG.md Runde 38 + Nachtrag 14 Runde 37):
1. Hinweis an Roys Leiche (1021): "There's an empty key tag on his belt. / It reads "Dining Room"." (NUTZER-VORGABE);
   "Help" + Klopfen sind TON, kein Text.
2. Ablauf Nachtrag 14: Schluessel in 1011 an Roys Liegestelle, Tuer 1131 -> 1141 zu bis zum Schluessel,
   John auf dem Dach (Haemmern + Help + panische Musik bis zum Gemaelde-Schalter), Szene ohne "John, wait!"/"sorry",
   Tuersperre "I cannot leave him alone with those monsters." solange die Gorillas leben.
3. ROOM1140 (Leon): Rolltor ohne Ton beim Hochfahren.

## 0. Stand / Fortschritt
- 16:20 Baum sauber auf e2cc7673, Dossier angelegt (ca756a09).
- 16:28 Basis-Bau `local_build.sh configure` + `build` gruen (LOCAL-BUILD-OK).
- 16:48 Messung vorher Punkt 3 (1140), 17:10 Messung vorher 1141-Szene, Zensus Item-Id/Bank 9/Nachrichten,
  Ton-Quellen gemessen (Whisper-Transkription, VAB-Zensus). Naechster Schritt: Umsetzung Punkt 3.
- 17:36 Modul dining_r38 (6ae7118b) — ~18:18 Sitzungslimit; 21:12 Fortsetzung, 21:35 vom Orchestrator gestoppt
  (Nutzer-Nachtrag 2). Beide Reste (2fd2342d, 32754a59) ungeprueft committet.
- **Fortsetzung 3 (ab ~22:00):** Bau des Ist-Stands gruen (`configure` + `build`: "ninja: no work to do", LOCAL-BUILD-OK —
  die Vorgaenger hatten diesen Stand schon gebaut). Umbau 1141 auf Nachtrag 2, Plan Abschnitt 3.0 (6f1890c5); Lauf m1;
  Riegel unit (c391d0cd) und integration (587a882c ff.); Hilferuf-Pegel gemessen und auf den Sprach-Pfad gelegt
  (7fa09458); Laeufe int_d2/int_d3 (echter Tuerweg 1131 -> 1141 -> 1171), int_wzl (W/Z), int_l3 (Leon 1140), v1011.
- **Fortsetzung 4 (2026-10-17, nach Sitzungslimit 23:45):** Endstand 18e09882 gebaut (`configure` + `build`,
  LOCAL-BUILD-OK, 730/730). Offen laut Auftrag: Framedump-Pruefung "John erscheint in keinem Bild ohne vorherige
  Bewegung ins Bild" als Riegel (Abschnitt 4.3), gezielte Tests am Endstand, Abschluss.

## 1. Messung vorher (Stand e2cc7673, echte exe)

### 1.1 Punkt 3 — Leon ROOM1140 Rolltor (Lauf `v1140`)
`RE15_GOTO_ROOM=1140 RE15_SET_FLAG=2:0 RE15_FORCE_EVENT=3@60 RE15_PRESS=square@200..350 RE15_SE_DEBUG=1
RE15_AUDIO_CAP_SYNC=cap.raw` (Leon, Strom (2,0) an — Port-Stromtor scd_vm.c s_power_gates). Auszug
`F_belege/v1140_vorher_auszug.txt`:
- F60 `scd_event_fire(3)` -> sub03, F60 msg 1 "A beautiful painting.There's a switch below. Will you push it?"
- F293 `Cut_chg(5)`, das Prop obj 0 (Rolladen, (-7124,0,-6976)) faehrt hoch (prop-render y -6 ...),
  F487 Letterbox zu.
- **0 Zeilen `[se] SCD Se_on` im ganzen Lauf** — der Rolladen faehrt stumm. Befund bestaetigt.

### 1.2 Punkt 1/2 — Elza ROOM1141 mit (4,0xF0)=1 (Lauf `v1141`, Bilder F100..F1800)
`RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_GOTO_ROOM=1141 RE15_SET_FLAG=4:0xF0 RE15_MSG_LOG=1`:
- F30 `Evt_exec sub=2` sofort beim Betreten (sub00 @0x00B02), Cut 7, msg 0 "Elza: John, wait!", msg 1
  "John: Elza! sorry, I'm glad to see you're still alive...", msg 2 "Elza: Same here... are you okay? what's
  happened?", ... msg 11, F1476 Letterbox zu; danach Gorilla-Kampf, John steht im Suedwesten (-7550,-21500).
- Die Szene startet also beim ersten Betreten — der Nutzer-Befund (Logikloch) ist der Ist-Zustand.
- 1131 -> 1141: Door_aot_set ROOM1131 main00 @0x0088E `3b 00 02 31 ...` ohne jede Bedingung -> offen.
- 1021 Roys Leiche: sub06 @0x02202 `2b 09 ff ff` = nur msg 9 "He's dead.[Seite]Poor Roy..." (kein Hinweis).

### 1.3 Nachtrag 2 — der alte Ablauf liess John auftauchen (Stand 6ae7118b/32754a59, aus dem Programm-Listing)
Das Programm RIEGEL (tools/r38_dining/programme_bauen.py vor 6f1890c5, `git show 32754a59:re15_port/tools/r38_dining/
programme_bauen.py`) lief beim Riegel (Ereignis 5) und enthielt nacheinander `Cut_chg 6` (Kamera im Dachgang, Blick auf
die Dachtuer — Bild cut1141_6 in `F_belege/sheet_1141_cuts.png`) und `Sce_em_set John ... Lage in der Dachtuer
(-7350,-400)` — John wurde also IM BILD von Cut 6 an der Dachtuer erzeugt, ohne vorher irgendwo gewesen zu sein: genau
das "plötzlich rein poppende" Auftauchen aus dem Nutzer-Nachtrag. Ebenso der Abgang: `Cut_chg 6`, John RUN an die
Dachtuer, `Pos_set` geparkt — Verschwinden im Bild von Cut 6. Beides ist ersetzt (Abschnitt 3.0).

## 2. RE-Belege

### 2.1 Punkt 3 — Rolladen-Ton: Form aus ROOM1131 sub02, Wellen aus der 1131-snd0-Bank
Werkzeug: `re15_port/tools/scd_dump_room.py` (eine Laengentabelle), `tools/r38_dining/zensus.py`.
- ROOM1140 sub03 @0x00CA6..0x00D51 (172 B): Message_on 1, Ifel_ck `06 00 a0 00` @0x00CAC, Set(3,0x6A),
  Aot_reset 5, Set(2,7), Set(1,0x1B), Cut_chg 5 @0x00CCA, Bewegungsblock @0x00CCC..0x00D15 (Work_set Prop 0,
  Speed_set, 4 For-Schleifen), Sca_id_set ..., **kein Opcode 0x36**.
- ROOM1141 sub06 @0x00FE8: derselbe Bewegungsblock @0x01032..0x0107B, ebenfalls **kein 0x36**.
- ROOM1131 sub02 @0x00A22 (Elzas Flur-Rolladen, "It's a shutter switch"): Ifel_ck `06 00 d0 00` @0x00A28 und
    @0x00A48 `36 02 0c 00 01 00 00 00 00 00 00 00`  Se_on(2,0x0C)
    @0x00A54 `09 0a 0f 00`                          Sleep 15
    @0x00A58 `36 02 0a 00 03 00 00 00 00 00 00 00`  Se_on(2,0x0A)
    @0x00A64 `09 0a 0f 00`                          Sleep 15
    @0x00A68..0x00AB1                               Bewegungsblock (74 B, bytegleich 1140/1141)
    @0x00AB2 `36 02 0b 00 03 00 00 00 00 00 00 00`  Se_on(2,0x0B)
    @0x00ABE `09 0a 0a 00`                          Sleep 10
  Ifel-Laenge 0xd0 - 0xa0 = 0x30 = 48 B = genau die 3 Se_on (je 12 B) + 3 Sleep (je 4 B).
- Bank 2 = RDT-snd0 (FUN_80045024 Bankweiche, re15_audio.h). snd0-EDT-Saetze (rolltor_sound.md 4.4, nachgemessen):
    1130/1131 Satz 0x0A `00 00 76 16` -> VAG 4 (5920 B, sha1 f240d33c), 0x0B `00 00 87 16` -> VAG 5 (2240 B,
    ce8ed301), 0x0C `00 00 95 11` -> VAG 8 (336 B, 7819c42b)
    1140/1141 Satz 0x0A -> VAG 4 (912 B, 7f67e256), 0x0B -> VAG 5 (3168 B, 487c19e0), 0x0C -> VAG 7 (7440 B,
    0f0c7871) — ANDERE Wellen; keine der 9 VAGs von 1140/1141 ist eine der drei Rolladen-Wellen (sha1-Vergleich).
  => Die Rolladen-Wellen gibt es nur in der 1130/1131-Bank. Der Port spielt sie aus ROOM1131.RDT snd0
     (EDT @0x1450, VH @0x14D0, VB @0x2F70 — RDT-Kopf +0x08/+0x0C/+0x10), nicht aus der Bank des Raums.

### 2.2 Item-Id fuer den Dining Room Key (Zensus `zensus.py`, VERTRAG 1.3)
- Platzierungen (Item_aot_set 0x50 + Aot_reset sce 9) in allen RDTs: nie platziert und Eigenschaftssatz
  @0x80074DA8+Id*12 = `01 00 00 00 88 4c 07 80 00 00 00 00` (wie alle Schluesselkarten): 0x2A-0x2F, 0x33-0x35,
  0x3A-0x3E, 0x40 (Port: Sicherung), 0x42 (Port: Prison Key), **0x43 "Communications Card"**, 0x45 (Port: Parking
  Lot Key). Keep_Item_ck (0x5E) kommt in keinem RDT vor. Port-Code nennt 0x43 nirgends.
- **Gewaehlt 0x43**: einzige freie Id der Karten-/Schluessel-Familie 0x36..0x47 (0x36-0x39 Keycards, 0x41/0x44/
  0x46/0x47 platziert). Ihr ausgelieferter Name traegt kein "Key" -> der Wegwerf-Text (item_prompt_common.c
  re15_prompt_name_ist_schluessel, prueft den AUSGELIEFERTEN Namen) braucht einen Haken, sonst sagte die Abfrage
  "item" statt "key".

### 2.3 Bank 9 (VERTRAG 1.1: F = 64, 66, 67)
`zensus.py`: (9,60..70) in keinem Item_aot_set (pc[18]) und keinem Ck/Set der Bank 9; Port-Code: kein Bank-9-Bit 64/66/67
(einziges "64" ist (4,64) in re15_cut10f0.h). Vergabe: 64 = Schluessel genommen, 66 = Tuer 1131->1141 aufgeschlossen,
67 = John hat 1141 durch die Dachtuer verlassen.

### 2.4 Nachrichten (VERTRAG 1.2)
RDT-Nachrichten: 1011 = 24 (0..23), 1021 = 15, 1131 = 6, 1141 = 15. Port-Ids F: 1011 45-49, 1021 40-44,
1131 40-44, 1141 40-49 — alle hinter den Originalen.

### 2.5 Ton-Quellen "Help" und Klopfen (Wahl begruendet)
- Transkription (Whisper base.en, Messwerkzeug, `F_belege/asr_voice_kendo.txt`, `asr_voice_alle.txt`): voice/Kendo
  (22 RE2-Kendo-Schnipsel) enthaelt **kein "help"** — nur "Hold on", "Freeze", "Who are you?", Schreie.
  Alle voice/*-Ordner: "Help me!" nur bei Irons (VOICE13.XAS[15]_04, 2,04 s) und Sherry.
- Johns eigene Stimme (Nutzer-Produktion synchro/): ROOM1171 main01.wav = "We need to get this thing off of here.
  Help me out." (32 kHz, 2,90 s). Huellkurve (10-ms-RMS, Schwelle 0,02): Sprache 0,28-0,80 / 0,90-1,87 /
  **2,04-2,59 s**, Pause 1,87-2,04 s. Schnitt ab 1,95 s -> "Help me out." (0,95 s; Whisper-Gegenprobe "Help me out.").
  => **Wahl "Help": Johns eigene Stimme** (gleicher Sprecher wie in der Szene direkt danach; Kendo hat kein Help,
  Irons waere ein fremder Sprecher). Datei synchro/STAGE1/room1141/john_help.wav, seit 7fa09458 main42.wav (Nutzer kann sie ersetzen).
- Klopfen/Haemmern: RE2 ROOM1060 ("An ominous wind can be heard...", "Zombies are inside...!!", "It's too dangerous
  to go back outside...!!" — die Huette, in der die Zombies gegen die Tuer schlagen; RE2_SOUND_LISTE "ROOM106.SAP -
  Cabin sounds - Zombies breaking in"). Dort laeuft das Schlagen als SCHLEIFE, sub05 @Datei 0x0F26 (106 B):
    +0x22 `36 02 0a 01 00 00 dc 88 f8 f8 3d 05` Se_on(2,0x0A)   +0x2E `09 0a 1e 00` Sleep 30
    +0x32 `36 02 0b 01 00 00 dc 88 f8 f8 3d 05` Se_on(2,0x0B)   +0x3E `09 0a 1e 00` Sleep 30
    +0x42 `06 00 18 00` Ifel  +0x46 `23 00 1d 02 00 7d` Cmp  +0x4C Se_on(2,0x0B)  +0x58 `09 0a 5a 00` Sleep 90  +0x5C Endif
    +0x5E `09 0a 5a 00` Sleep 90   +0x62 `17 ff ff 00 e0 ff` Goto -0x20 (-> +0x42)
  snd0 (RDT +0x08/+0x0C/+0x10 = EDT 0x3F58 / VH 0x4018 / VB 0x4C38): Satz 0x0A `00 00 85 20` -> VAG 5 (8768 B),
  0x0B `00 00 ad 00` -> VAG 6 (12480 B).
  => **Klopfen = RE2 ROOM1060 snd0 Satz 0x0A/0x0B, Takt = dieselbe Schleifenform** (Schlag, 30, Schlag, 30, ...,
  90 Pause) — Beta->Retail, RE2 ist die Vorlage fuer "in regelmaessigen Abstaenden".
- **Musikwahl "panische Musik" (Fortsetzung 4 nachgetragen):** der Raum-Titel von 1141 selbst. BGM-Tabelle Raum 0x14
  (Stage 1) = Eintrag 0xFF56 -> MAIN16 mit Handstart-Flag (debug.log int_d3 `[bgm] stage=0 room=14 entry=FF56 ->
  MAIN16(flag 1)`): der Titel laeuft nur, wenn ein Skript ihn startet — im Original ausschliesslich in der John-/
  Gorilla-Lage: sub00 @0x00B4E `54 00 01 00 00 00` (Start beim Betreten mit der Szene), sub02 @0x00E5C (Gorillas
  brechen ein), Stopp sub01 @0x00C06 `54 00 02 ..`, sobald beide Gorillas tot sind. Das Original koppelt diesen Titel
  also genau an die Bedrohung in diesem Raum. Tempo gemessen: `[bgm] loaded: 11 VAGs, SEQ 5916B, PPQN=48
  tempo=394736us` = 152 Viertel/min (zum Vergleich der Flur 1131 MAIN17: tempo=1250000us = 48/min). Die
  Klopf-Schleife startet ihn mit derselben Zeile (HAEMMERN +0000 = @0x00B4E), BEFREIUNG stoppt ihn mit der Zeile von
  @0x00C06, wenn John frei ist; der Gorilla-Einbruch startet ihn wieder (@0x00E5C, Original).
- "Are you okay? What's happened?" (1141 msg 2 ohne "Same here..."): synchro/STAGE1/room1141/main02.wav = "Same here.
  Are you okay? What's happened?" (Sprache 0,16-0,98 / 1,50-2,33 / 2,82-3,54 s). Schnitt ab 1,24 s ->
  main40.wav (Whisper: "Are you okay? What's happened?"). Id 2 bleibt unangetastet (VERTRAG 1.2).

### 2.6 ROOM1141 Geometrie (RDT, fuer die neue Szene)
- Tueren: Slot 0 -> 1131 Rechteck x -9100..-8100, z -19750..-15750 (Westwand, Doppeltuer); Slot 1 -> 1171
  x -8250..-6250, z -750..250 (Dachtuer am Nordende des Dachgangs), Riegel Slot 2 sce 3 -> sub05 im selben Rechteck.
- SCA: Westwand x -10600..-8600; Dachgang x -8600..-5750, z -6700..-250; Rolladen-Zelle r[13] x -9000..-5300,
  z -7200..-6700 (faellt per Sca_id_set 0x0d); Tisch x -4650..7450, z -17450..-11350.
- Kameras: Cut 6 (-8292,-838,-6606) -> (-6375,-3295,8211) = Dachgang mit Dachtuer; Cut 7 (-7465,-3024,-9613) ->
  Blick nach Sueden; Cut 0/2 (893,-3929,-11697) Richtung Doppeltuer / Fenster; Cut 5 (-7266,-3806,-17278) -> Norden
  (Rolladen + Gemaelde). Bilder `F_belege` (sheet_1141).
- Original-Szene sub02: Elza Pos_set (-7600,0,-17600) @0x00C32, John Spawn (-6750,0,-13350) sub00 @0x00B06,
  Dialogplaetze Elza (-7200,-15500) / John (-6750,-13350), Fensterblick (-1300,-15500)/(-850,-13350),
  Gorillas Pos_set (9500,-11000)/(10150,-17700) @0x00E38/@0x00E4C, John am Ende -> (-7550,-21500) @0x00F8E.

## 3. Umsetzung

### 3.0 Nachtrag 2 (Nutzer 2026-10-16 21:33) — John taucht nirgends auf
Wortlaut: "In Room 1141 mit dem reinkommenden John musst du anders lösen. Ich möchte keine plötzlich rein poppenden
Charaktere haben..." Der alte Ablauf (6ae7118b: John wird beim Riegel in der Dachtuer gespawnt und "bricht herein") ist
genau ein solches Auftauchen und ist ersetzt (6f1890c5):

| Schritt | Mechanik | Beleg / Form |
|---|---|---|
| Phase H | (4,0xF0)==1 UND Notrolladen zu (3,0x6A)==0 (statt Riegel (4,0xC3)) | (3,0x6A): sub06 @0x00FF6 `22 03 6a 01` setzt, sub00 @0x00B84 `21 03 6a 00` liest |
| John schon da | Installer (nach dem Init-Lauf, VOR dem ersten Bild) setzt den Spawn sub00 @0x00B06 `44 00 49 40 .. a2 e5 00 00 da cb` = (-6750,0,-13350) auf (-7124,0,-6150) Blick 1024 — im Dachgang HINTER dem Rolladen; Gorillas heraus | x = Prop obj 0 x sub00 @0x00B9C+10 `2c e4`; Dachgang SCA x -8600..-5750, z -6700..-250; Rolladen-Zelle r[13] z -7200..-6700; Blick: pos_advance @0x800245d8 (x += cos, z -= sin) -> 1024 = -Z (zum Rolladen). PORT-WAHL der Tiefe: 550 hinter der Zelle |
| Klopfen/Hilferuf/Musik | HAEMMERN wie bisher (Klopfen RE2 ROOM1060 sub05, "Help me out.", MAIN Play @0x00B4E), jetzt als While Ck(3,0x6A)==0, jede Tonzeile in Ifel_ck Ck(3,0x6A)==0 | While-Form sub03 @0x00FA8 `0f 04 08 00` (Laenge ab hinter dem Kopf bis hinter Ewhile, scd_vm.c op_while); Ifel-Form sub01 @0x00BFE `06 00 10 00` (Laenge bis hinter Endif @0x00C10). Das Original-Set(3,0x6A)=1 beim "ja" am Gemaelde beendet die Schleife — kein C-Eingriff mehr |
| Gemaelde (Ereignis 6) in Phase H | Programm BEFREIUNG: sub06 @0x00FE8..@0x0100D (Frage, Ja-Zweig, Cut 5) + Rolladen-Ton (ROOM1131 @0x00A48/@0x00A58, @0x00AB2) + Bewegung @0x01032..@0x0107B + Sca @0x0107C..@0x010A3; die Gorilla-Bloecke @0x0100E/@0x010A4 entfallen (in Phase H keine Gorillas) | John steht beim Hochfahren hinter dem Rolladen und wird im Bild von Cut 5 sichtbar, ohne sich zu bewegen |
| John kommt heraus | Musik aus (Form sub01 @0x00C06), Flags sub02 @0x00C18/@0x00C1C/@0x00C20, Gorillas geparkt (sub00 @0x00B1A..@0x00B41), John RUN an seinen Platz + Gosub sub04 (Form sub02 @0x00F8E/@0x00F96) | |
| Szene | sub02 @0x00CD4..@0x00FA1 (Elza an ihren Platz mode 4, Drehungen, msg 2 -> Port-Nachricht 40 ... msg 11, Gorillas durch die Fenster, John in die Ecke) — Cut_chg 0 @0x00D08/@0x00E80 -> 5; 12 Ziele umgesetzt: Elza (-7200,-15500) -> (-8000,-10250), John (-6750,-13350) -> (-5800,-10250), Blickpunkte (-1300,-15500)/(-850,-13350) -> Platz + (5900,0) | Cut 0 (893,-3929,-11697) -> (-17202,3679,-26058) blickt nach WSW auf die Doppeltuer; Cut 5 (-7266,-3806,-17278) -> (-3224,2605,2675) blickt nach NNO auf Rolladen + Gemaelde. Plaetze PORT-WAHL: Elza = Ostkante/z-Mitte der Gemaelde-Zone (Aot slot 3 @0x00B88: x -9000..-8000, z -11000..-9500), John 2200 oestlich = Original-Abstand (2196). Blickpunkt-Regel aus dem Original: (-1300,-15500) = (-7200,-15500)+(5900,0), (-850,-13350) = (-6750,-13350)+(5900,0) |
| Tuersperre | VOR der Rueckgabe der Steuerung (@0x00F4E): Aot_reset Slot 0 und Slot 2 (Riegel im Rechteck der Dachtuer) -> msg 41; warten, bis sub01 (4,0xF2) loescht | Form ROOM11B0 sub01 @0x01216 `46 01 01 31 0f 00 ff ff 00 00`; Slot 1 ist bei zu Riegel sce 0 (main00 @0x00A7A) |
| Johns Abgang | Pad-Sperre + Balken, Cut 7 (Blick nach Sueden), John RUN zum Gang-Ausgang und vor die Dachtuer (er laeuft auf die Kamera zu und verlaesst das Bild am unteren Rand), DOOR1A auf/zu im Off, geparkt, (9,67)=1; Riegel-Zustand wie nach sub05 (@0x00FC8..@0x00FD5 ohne Nachricht, @0x00FDC), Slot 0 wieder Tuer | Cut 7 (-7465,-3024,-9613) -> (-4265,2209,-27792); DOOR1A = tuer_zuordnung.inc S064 ROOM1141 Slot 1 |
| Riegel | Ereignis 5 hat KEIN Port-Programm mehr (Original sub05). John oeffnet ihn beim Abgang; danach ist die Dachtuer offen | Begruendung: John hat ihn auf der Gangseite vorgelegt; geht er hinaus, kann er ihn nicht wieder vorlegen — ein "You've opened the lock." danach waere ein neues Logikloch |

### 3.1 Dateien
- NEU `re15_port/engine/src/dining_r38.c`, `re15_port/include/re15_dining_r38.h` (Installer, Ereignis-/Evt_exec-Haken,
  Tuer-Handler 1131, Gegenstand 0x43, Messhaken), `re15_port/platform/pc/src/dining_r38_pc.c` (Port-Baenke 0x3B/0x3C/0x3D).
- GENERIERT `re15_port/engine/src/gen/dining_r38_programme.inc` (tools/r38_dining/programme_bauen.py: ROLL1140,
  ROLL1141, LEICHE1021, HAEMMERN, BEFREIUNG mit Listing je Zeile + Texte + Anker),
  `re15_port/engine/src/gen/dining_r38_schluessel.inc` (tools/r38_dining/re2_export.py: RE2 Herz-Schluessel 0x5B).
- NEUE DATEN `re15_port/shared_assets/RE2/KLOPF1060.EDT/.VH/.VB` (RE2 ROOM1060 snd0 bytegleich),
  `synchro/STAGE1/room1141/main42.wav` ("Help me out.", Schnitt aus room1171/main01.wav; bis 7fa09458 john_help.wav), `synchro/STAGE1/room1141/
  main40.wav` ("Are you okay? What's happened?", Schnitt aus main02.wav) — Werkzeug tools/r38_dining/sprache_schneiden.py.
- HAKEN (je 1-5 Zeilen, Kommentar "Runde 38 Spur F"): scd_room_setup.c (Installer am Blockende), scd_vm.c
  (scd_event_fire-Kette + op_evt_exec), platform/pc/main.c (Boot-/CONTINUE-Installer, Prop-Lader 1011, Icon/Bild an
  zwei Ladestellen, Port-Bank-Init, Messhaken), audio_pc.c (Se_on der Port-Baenke vor der Bankweiche + Mitschnitt-Takt),
  item_discard_common.c / item_icon_common.c / item_prompt_common.c / itps_common.c / re15_inv_screen.c /
  inv_render_pc.c (Gegenstand 0x43 wie Runde 36 Spur Q), tests/unit/test_r36_kartenbits.c k_port[] (Bits 64/66/67).
- Tests: `re15_port/tests/unit/test_r38_dining1141.c`, `re15_port/tests/unit/probes/r38_dining1141.cmake`,
  `re15_port/tests/integration/test_r38_dining1141.cmake`.

### 3.2 Konstanten (alle in include/re15_dining_r38.h mit Beleg)
| Konstante | Wert | Beleg |
|---|---|---|
| Item | 0x43 | Zensus 2.2: nie platziert, Satz @0x80074DA8+0x43*12 wie alle Karten/Schluessel |
| Bank-9-Bits | 64 / 66 / 67 | VERTRAG 1.1, k_port[] |
| 1011 Schluessel | obj 6, Zone Slot 14, (-2000,0,7450) | Roy-Spawn sub00 @0x009F6; Slot/obj nach Port-Zensus (obj 3/4, Slot 9/10 belegt) |
| 1021 Leiche | Ereignis 6 -> msg 40 | sub06 @0x02202 `2b 09 ff ff` -> `2b 28 ff ff` |
| 1131 Tuer | Slot 0 -> Ereignis 29, msg 40/41 | Door_aot_set @0x0088E; RE2 LAB_80051514 (@0x80051628 Besitz, @0x80051654 Ton, @0x80051690 Bit) |
| Phase H | (4,0xF0) && !(3,0x6A) | @0x01FE0 / @0x00FF6 / @0x00B84 |
| John im Gang | (-7124,0,-6150) Blick 1024 | Prop obj 0 x @0x00B9C+10; pos_advance @0x800245d8; PORT-WAHL Tiefe |
| Plaetze | Elza (-8000,-10250), John (-5800,-10250) | Gemaelde-Zone @0x00B88; Abstand 2196 = @0x00CD8/@0x00B06 |
| Klopf-Takt | 30/30/90/90 Bilder | RE2 ROOM1060 sub05 +0x2E/+0x3E/+0x58/+0x5E |
| Port-Baenke | 0x3B Rolladen (ROOM1131 snd0), 0x3C Klopfen (KLOPF1060), 0x3D Hilferuf | Abschnitt 2.1/2.5 |

## 4. Messung nachher

### 4.1 Lauf m1 (echte exe, Elza, Sprung 1141 + Gemaelde per RE15_FIRE_AOT; scratchpad r38f_dining/l_m1)
`RE15_PSELECT_AUTO(_SWITCH)=1 RE15_SET_FLAG=4:0xF0 RE15_DEBUG_JUMP=1141@gp RE15_PLAYER_POS=-8300,-10250,2048
RE15_FIRE_AOT=3@700#1141 RE15_PRESS=square@760,square@820,square@880 RE15_AUDIO_CAP_SYNC RE15_STATE_LOG RE15_FRAMEDUMP`
- debug.log: `[dining] F1005 ROOM1141 Phase H: John im Dachgang hinter dem Rolladen (-7124,0,-6150) Blick 1024, Gorillas
  heraus` im Ladebild; Zustands-Log ab F0: `[1 t=49 ... @(-7124,-6150,r1024)]` — John ist ab dem ersten Bild da.
- Ton: `[se] Se_on bank=0x3C id=10` F1, `0x3C id=11` F35, `0x3C id=11` F65, `0x3D id=0` F155, `0x3C id=10` F245 ...
  (Takt 30/30/90/90), Gemaelde-Frage ab F705 (waehrend der Frage pausiert die Schleife wie jeder SCD-Faden), "ja"
  F883: `0x3B id=12` F883, `0x3B id=10` F898, `0x3B id=11` F1093; danach kein 0x3C/0x3D mehr.
- Nachrichten in 1141: 13, 40, 3, 4, 5, 6, 7, 8, 9, 10, 11 — kein 0, 1, 2. Kamera: F883 Cut 5, F1956 Cut 2 (Gorillas),
  F2058 Cut 5, F2484 Steuerung frei; John am Ende (-7531,-21363) = Ecke @0x00F8E.
- Bilder `F_belege/n2_rolladen_john.png` (F870..F1290): F900/F930 Cut 5, Rolladen zu, John NICHT im Bild; F960
  Rolladen faehrt hoch, Johns Beine erscheinen UNTER dem Rolladen (er stand die ganze Zeit dort); F1020 John ganz zu
  sehen, Dachgang mit Dachtuer dahinter; F1110/F1140 John laeuft heraus; F1170..F1260 "Elza: Are you okay? what's
  happened?" mit beiden im Bild; F1290 "John: Somehow... Elza, listen to me!".

### 4.1a Hilferuf zu leise — gemessen und behoben (7fa09458)
Ton-Mitschnitt m1 gegen die Se_on-Takte (`tools/r38_dining/ton_mitschnitt.py`, Kreuzkorrelation mit der Quelldatei,
Least-Squares-Verstaerkung im Mix):
| Quelle | Korrelation am Takt | Kontrolle 3 s vorher | Verstaerkung im Mix |
|---|---|---|---|
| Hilferuf (alte SE-Bank 0x3D) Takt 3294 / 3534 / 3774 | 0,295 / 0,258 / 0,236 | 0,042 / 0,033 / 0,054 | 0,234 / 0,247 |
| Dialogzeile main40 (Sprach-Pfad), Takt 4300 | 0,844 | — | 1,043 |
Der Hilferuf lag also im Mix, aber ~12 dB unter jeder Sprachzeile (se_play_layers: vol = (127*0x4000/127)>>1 = 0x2000 =
0,25) und unter der Raummusik (RMS 1100-3300 gegen ~330) — praktisch unhoerbar. Weil "Help" eine Sprachzeile Johns aus
der Nutzer-Produktion ist ("Teil der Audio files"), laeuft sie jetzt ueber den Sprach-Pfad wie jede synchro-Zeile
(re15_voice_play, Pegel-Boden 45 % Vollaussteuerung): Datei `synchro/STAGE1/room1141/main42.wav` (umbenannt aus
john_help.wav; Port-Id 42 ohne Text, VERTRAG 1.2), Haken audio_pc.c `re15_audio_stimme` (1 Zeile). Die SCD-Zeile bleibt
Se_on(0x3D,0) im Takt der Schleife. Klopfen (0x3C) bleibt SE: RMS ~5000-5500 im Mix, deutlich ueber der Musik.

### 4.1b Lauf int_d2 (echter Tuerweg 1131 -> 1141, integration_r38_dining1141 Teil D, vor dem Fix 95fc8af6)
- Tuer 1131 mit Schluessel: `[dining] ROOM1131: Dining Room Key benutzt -> Tuer offen, Flag (9,66) = 1`, Tuer -> 1141.
- 1141 ab Bild 1: John (t=49) bei (-7124,-6150); 12x Klopfen 0x3C, 3x Hilferuf 0x3D; Musik `Sce_bgm_control slot=0 op=1`
  bei Phase-H-Beginn (Takt 3470), `op=2` nach dem Rolladen (Takt 4735, John frei), `op=1` beim Gorilla-Einbruch (Takt
  5588 = Original @0x00E5C).
- Nachrichten 1141: 13, 40, 3, 4, 5, 6, 7, 8, 9, 10, 11. Kameras: Cut 0 am Eingang, Cut 5 ab Bild 800 (Elza auf dem Weg
  zum Gemaelde), Cut 2 Bild 2119 (Gorillas), Cut 5 Bild 2221.
- Bildbogen `F_belege/n2_tuerweg_rolladen.png` (Bild 795..1395): Cut 5 mit geschlossenem Rolladen, Elza am Gemaelde,
  John NICHT im Bild (Bild 825..1095, Frage "Will you push it? Yes"); Bild 1125 Johns Beine unter dem hochfahrenden
  Rolladen, 1185 John im Gang, 1275/1305 John laeuft heraus, 1335.. "Elza: Are you okay? what's happened?".
- Abbruch: die Doppeltuer-Nachricht msg 41 (Bild 2930) blieb offen — der zweite Druck nach WARTE 45 beschleunigte nur
  die Schrift; Wegplan auf WARTE 90 korrigiert (95fc8af6), Lauf int_d3.

### 4.1c Lauf int_d3 (echter Tuerweg 1131 -> 1141 -> 1171, Teil D, Stand 7fa09458 + 95fc8af6) — alle Pruefungen gruen
Auswertung `cmake -DTEIL=D -DAUSWERTEN=<lauf> -P tests/integration/test_r38_dining1141.cmake`: "alle Pruefungen gruen
(TEIL D)". Messwerte:
- `[give] item 67 x1`; `ROOM1131: Dining Room Key benutzt -> Tuer offen, Flag (9,66) = 1`; msg 41 im Zustands-Log;
  `loaded room1141.rdt`; `ROOM1141 Phase H: John im Dachgang hinter dem Rolladen (-7124,0,-6150) Blick 1024`; Zustands-
  Log Bild F1: `[1 t=49 ... @(-7124,-6150,r1024)]`.
- Hilferuf jetzt Sprachzeile: `Se_on bank=0x3D id=0 -> Runde 38 Spur F (Stimme main42.wav) F155 tick=3696` (F395/F635
  folgen), `[voice] clip loaded: main42.wav`; Pegel im Mitschnitt: Korrelation 0,863, Verstaerkung 1,288 (Quell-RMS 1393
  -> im Mix 1794 bei Mix-RMS 2080) — vorher 0,23 (Abschnitt 4.1a).
- Erste Nachricht in 1141 = msg 13 (Gemaelde) — in Phase H erscheint kein Text; danach 40, 3 .. 11, nie 0/1/2.
- Rolladen-Ton 0x3B 12/10/11, `ROOM1141 Gemaelde (Ereignis 6) in Phase H -> Programm BEFREIUNG`.
- Doppeltuer im Kampf: msg 41 offen, waehrend John im Raum ist (Zustands-Log).
- `MESSHAKEN RE15_R38F_GORILLAS_TOT` (nach 600 Kampfbildern) -> Abgang: Cut 7 ab Bild 3277; John (Zustands-Log, je 15
  Bilder) F3270 (-7531,-21363) -> F3300 (-7399,-20375) -> F3330 (-7371,-14375) -> F3360 (-7353,-8375) -> F3390
  (-7351,-2575) -> F3405 (-7351,-975) -> F3414 geparkt (-30000,-30000) bei Cut 7, letzte Lage z -975 = hinter der
  Kamera von Cut 7 (z -9613, Blick nach Sueden); 0 Spruenge. `Se_on bank=14 id=0/1 -> RE2-Tuerbank` (DOOR1A auf/zu).
  Bildbogen `F_belege/n2_abgang_cut7.png`: F3285 John hinten in der Ecke, F3300..F3330 laeuft auf die Kamera zu, F3345
  dicht an der Kamera, ab F3360 aus dem Bild (unterer Rand) — kein Verschwinden im Bild.
- Dachtuer -> `loaded room1171.rdt`.

### 4.1d Laeufe int_wzl (W, Z) und int_l3 (L) — gruen
- W (Elza, Sprung in 1021, danach nur Tueren): Leiche -> `[msg] room=1021 id=40`; Tuer Slot 1 -> ROOM1011 (Spawn
  (3650,0,6900)); `[dining] ROOM1011: Dining Room Key (Item 0x43, Bit (9,64)) Prop obj 6 bei (-2000,0,7450), Zone Slot
  14`; Elza geht zur Liegestelle (-1372,7415), Aufnahme-Modal "Will you take the Dining Room Key?" (Bild
  `F_belege/n2_schluessel_aufnahme.png`: RE2-Herz-Schluessel im Item-Bild, Icon im Modal), `FLAG (9,64) = 1`; Tuer Slot 0
  -> ROOM1021 (zweites Laden), dort kein `type=0x49` (John weg, Original main00 @0x01DF6). (Die Pruefung "zweimal
  geladen" schlug im Lauf selbst am Makro-Fehler fehl — Auswertung mit 36a17826 gruen.)
- Sichtbarkeit des Schluessels (Lauf v1011c, Cut 3 erzwungen, Elza ausserhalb des Bildes): `F_belege/
  n2_schluessel_liegestelle_cut3.png` — der Schluessel liegt auf Roys Blutlache (Liegestelle) als kleiner heller Punkt.
  Das RE2-Modell 0x5B ist eine flache 152x76-Flaeche am Boden (MD1-Ecken (-76,0,+-38)/(76,0,+-38)); aus den Raumkameras
  von ROOM1011 ist die Stelle weit weg: Cut 3 4419 Einheiten (Flaeche ~18 px^2), Cut 1 6028 (~8 px^2), Cut 2 knapp unter
  dem Bildrand (y 245). Gross und deutlich ist der Schluessel im Aufnahme-Modal (Item-Bild, `n2_schluessel_aufnahme.png`).
- Z (Elza 1131 ohne Schluessel): `ROOM1131 Slot 0 (Tuer -> ROOM1141) gesperrt: Ereignis 29`, `Tuer zum
  Besprechungsraum verschlossen`, msg 40 im Zustands-Log, kein Raumwechsel; Bild `F_belege/n2_tuer1131_zu.png` ("It's
  locked." / "The door is marked "Dining Room".").
- L (Leon 1140): Leons Gemaelde-Zone (Slot 5) feuert im Port Ereignis 5 = sub05 "The switch is not responding..." —
  sub00 @0x00B14 setzt Slot 5 -> sub03, aber sub01 @0x00C7A `46 05 03 31 ff 00 18 05` (laeuft jedes Bild) stellt ihn
  auf sub05 (gemessen: `test_r38_dining1141 takt` "1140 Slot 5 ... Ereignis 5"; Lauf int_l2 msg 1 -> msg 5, Strom (2,0)
  lag nachweislich an: Wegplan-FLAG (2,0) = 1 bis vor das Gemaelde, Lauf l4). Der Rolladen hebt sich bei Leon nur ueber
  Ereignis 3 = sub03 — Lauf int_l3 per RE15_FORCE_EVENT=3 (wie der Vorher-Lauf v1140): `Se_on bank=0x3B id=12` F243,
  `id=10` F258 (Sleep 15), `id=11` F453, `Port-Bank 0x3B geladen`; Mitschnitt: Pegel x2,10 nach 0x0A (Motor), x1,91 nach
  0x0B (Anschlag). Elza 1141 (int_d3, echter Weg ueber das Gemaelde): 0x3B 12/10/11, Pegel x1,44/x1,56.

### 4.2 Riegel unit_r38_dining1141_* (Engine-Wege, Messwerte aus den Ausgaben)
- phase_h: Bild 0 John bei (-7124,-6150) Blick 1024, keine Gorillas; 600 Bilder ohne Nachricht, John 0 Bilder
  abgewichen; Toene Bild 0/29/59/149/239/269/299/389/479/509/539 = 0x3C 0A/0B/0B, 0x3D 00 im Wechsel; "ja" Bild 725;
  Nachrichten 40 3 4 5 6 7 8 9 10 11; groesster Schritt Johns 252 je Bild (Lauf), kein Sprung; Doppeltuer und Dachtuer
  im Kampf -> msg 41; Gorillas tot -> John in Bild 3146 geparkt, Cut 7 aktiv, im Bild davor NICHT im Bild von Cut 7,
  vorher lief er sichtbar durch Cut 7; DOOR1A auf/zu im Off; (9,67)=1, (4,0xC3)=1, Slot 1 -> 0x17, Slot 0 -> 0x13;
  Dachtuer -> ROOM1171; Wiederbetreten: kein John.
- tuer1131: ohne Schluessel msg 40, kein Raumwechsel; mit Schluessel msg 41, (9,66)=1, Wegwerf-Abfrage gestellt und
  beantwortet, Schluessel weg; danach Raumwechsel nach ROOM1141 (eine Tuersequenz).
- schluessel1011: vor der 1021-Szene kein Schluessel; nach der Szene (Flags wie im Spiel: (4,0xF1) (4,0xEC) (3,0x78)
  (3,0x5F) (4,0xF0)) kein Roy, Prop obj 6 bei (-2000,7450), Zone Slot 14 Item; Druck von Osten -> Aufnahme-Modal
  Item 0x43 -> Inventar + (9,64); Wiederbetreten: weg.
- leiche1021: Anker bytegleich, Ereignis 6 -> msg 40 "He's dead.Poor Roy...There's an empty key tag on his belt. It
  reads Dining Room"." (Log-Dekoder ohne Seitenleerzeichen und ohne die Font-Glyphe 0x61 = oeffnendes Anfuehrungszeichen;
  Rohbytes `61 20 45 4a 45 4a 43 00 2e 4b 4b 49 19` geprueft).
- leon1140: 0x3B 0x0C Bild 143, 0x0A Bild 158 (Abstand 15 = Sleep @0x00A54), 0x0B Bild 353; Rolladen-Prop faehrt hoch.

### 4.3 Fortsetzung 4 — Befund: Kamera blieb nach Johns Abgang auf Cut 7 (Elza lief aus dem Bild)
**Messung vorher** (Lauf int_d3 = `build/tests/integration/r38_dining1141_d_wd/lauf_2879b87c_d`, Stand 7fa09458/95fc8af6,
Abgangs-Programm unveraendert bis 18e09882): Zustands-Log 1141 `cam=7` ab F3277 (Cut_chg 7 des Abgangs) bis zum
Raumwechsel; `[scd F3443] letterbox closed -> gameplay` (Programm-Ende), danach geht Elza (Wegplan) von (-5605,-18938)
nach Norden: F4065 (-5696,-14003), F4140 (-6810,-9719), F4215 (-7429,-5345), F4290 (-7264,-1071) — immer `cam=7`.
Cut 7 steht bei (-7465,-3024,-9613) und blickt nach Sueden; ab z > -9613 ist Elza HINTER der Kamera. Framedumps
fd_004065/004140/004215/004290: nur in F4065 ist Elza im Bild, danach der leere Raum
(`F_belege/n4_cut7_haengt.png`). Der Spieler steuerte also eine unsichtbare Elza bis zur Dachtuer — der alte Riegel
pruefte nur "loaded room1171.rdt".
**RE-Beleg:**
- RVD ROOM1141 (RDT-Kopf +0x28, 20-B-Saetze, rdt_common.c parse_zones): Satz 21 `from 7 to 0` ist der EINZIGE Satz
  der Gruppe 7 = der Anker, den FUN_80014230 nie testet (Gruppensuche FUN_80014324, Scan ab dem zweiten Satz;
  rdt_common.c re15_rdt_apply_zones_as_aots). Cut 7 ist eine reine Szenen-Kamera (Original nur sub02 @0x00C24
  `29 07`): aus Cut 7 fuehrt keine Zone heraus, Cut_auto 1 allein laesst die Kamera dort stehen.
- Das Original beendet eine solche Szenen-Kamera mit `2a 00 3c 01` = Cut_old, Nop, Cut_auto 1, z.B. ROOM1150
  @0x010F2..0x010F5 (nach `29 06` Cut_chg 6 @0x010EA + Message_on 1 "It's a phone..."), ebenso ROOM10D0 @0x018D0,
  ROOM11F0 @0x016F0 (16 RDTs in STAGE1 tragen die Folge). Cut_old = FUN_8004032c: @0x8004033c `lbu a0,DAT_800b3f7b`
  = der ANGEZEIGTE Cut, den Cut_chg (LAB_800402a0) @0x800402c0/@0x800402e4 vor dem Wechsel gemerkt hat
  (scd_vm.c op_cut_chg/op_cut_old); dazu loescht FUN_8004032c Bit 0x100 von DAT_800aca3c = Kamera-Automatik an.
**Umsetzung:** Abgangs-Programm (tools/r38_dining/programme_bauen.py, BEFREIUNG-Schluss) endet jetzt mit
`2a 00` Cut_old + Nop vor `3c 01` Cut_auto 1 (Form ROOM1150 @0x010F2) — die Kamera kehrt auf den Cut zurueck, der vor
Cut 7 angezeigt war (Elzas Standort, eine Lauf-Kamera mit Zonen). Ifel-Laenge des Ja-Zweigs 0x474 -> 0x476 (vom
Werkzeug nachgerechnet).
**Messung nachher** (Lauf `integration_r38_dining1141_d` = lauf_425d9435_d, Stand 9736579e, alle Pruefungen gruen):
Kamerafolge 1141 laut Zustands-Log F1 cam 0 -> F800 5 -> F2119 2 -> F2221 5 -> F2788 0 -> F3277 7 (Abgang) ->
**F3429 0** (Cut_old, Elza bei (-5605,-18938)) -> F4108 5 (Elza (-6010,-11959) auf dem Weg nach Norden) -> F4267 6
(Elza (-7391,-1499) vor der Dachtuer) -> ROOM1171. Bildbogen `F_belege/n4_kamera_nach_abgang.png` (F3420 Cut 7,
F3435 Cut 0, F4125/F4200 Cut 5, F4275 Cut 6): Elza ist in jedem Bild zu sehen. Neue Pruefungen:
`unit_r38_dining1141_phase_h` "Kamera nach dem Abgang zurueck auf Cut 6 (vor Cut 7 angezeigt, jetzt Cut 6),
Kamera-Automatik an (1)" und `integration_r38_dining1141_d` "nach dem Abgang verlaesst die Kamera Cut 7 (Cut_old):
F3429 ... cam=0".

## 5. Tests
Registriert NUR aus `re15_port/tests/unit/probes/r38_dining1141.cmake`:
| Test | misst | Nutzer-Punkt |
|---|---|---|
| unit_r38_dining1141_texte | fuenf Port-Texte dekodiert = Wortlaut, Rohbytes der Anfuehrungszeichen 61..19, nirgends "Help" | 1 |
| unit_r38_dining1141_programme | HAEMMERN ohne Message_on, BEFREIUNG ohne msg 0/1/2, mit 40, Lagen = Kopf, Sperre Slot 0/2 | 1, 2, 4 |
| unit_r38_dining1141_leiche1021 | Ereignis 6 -> msg 40 mit Hinweis (Anker bytegleich) | 1, 2 |
| unit_r38_dining1141_schluessel1011 | Schluessel nur nach der 1021-Szene, Aufnahme-Modal -> Item 0x43 + (9,64), danach weg | 2 |
| unit_r38_dining1141_tuer1131 | zu ohne Schluessel, msg 41 + (9,66) + Wegwerf-Abfrage, dann Raumwechsel 1141 | 2 |
| unit_r38_dining1141_phase_h | John ab Bild 0 hinter dem Rolladen, in keinem Cut ausser 5/6 im Bild, Takt 30/30/90/90, keine Nachricht, "ja", Rolladen-Ton, Szene, keine Spruenge, Sperre msg 41, Abgang in Cut 7 ausserhalb des Bildes, Kamera danach zurueck auf den vorigen Cut + Automatik (Fortsetzung 4), Riegel/Tueren, 1171, Wiederbetreten | 2, 4 |
| unit_r38_dining1141_leon1140 | Leon: 0x3B 0x0C/0x0A/0x0B in der 1131-Folge, Abstand Sleep 15, Rolladen faehrt | 3 |
| integration_r38_dining1141_w/z/d/l | echte exe: W 1021->1011->1021, Z 1131 zu, D 1131->1141->1171 (Zustands-Log: John ab Bild 0, kein Sprung, Parken hinter Cut 7, Kamera verlaesst Cut 7 danach), L Leon 1140 | 1-4 |
| integration_r38_dining1141_s | Nachtrag 2 per Framedump (Fortsetzung 4): zwei Laeufe des Weges D (mit / ohne Johns Bild), JEDES Bild von 1141 zurueckgelesen und verglichen, Regeln R0-R3 (Abschnitt 4.4) | 4 |
Dazu `unit_r36_kartenbits` (k_port[] traegt 64/66/67). Messwerkzeug (kein Riegel): `test_r38_dining1141 takt`
(Druckzahlen je Abfrage fuer die Wegplaene).

## 6. OFFEN
- Keine offenen Mechanik-Punkte der Spur. Zur Integration (VERTRAG 1.5): ROOM1011 — Spur G (Tagebuch beim liegenden Roy,
  Phase (4,0xFA)=1) und Spur F (Schluessel nach der 1021-Szene, (3,0x5F)=1, (4,0xFA)=0 seit ROOM1211 sub02 @0x01F7A)
  liegen in verschiedenen Phasen; F belegt Prop obj 6 / Zone Slot 14, obj 5 bleibt fuer G. ROOM1171 (Spur I) betritt
  Elza bei F ueber die Dachtuer mit (3,0xC2)=1 wie im Original.
- Befund fuer den Orchestrator (nicht Teil der Nutzer-Punkte dieser Spur): Leons ROOM1140-Gemaelde feuert im Port
  immer sub05 "The switch is not responding..." — sub01 @0x00C7A `46 05 03 31 ff 00 18 05` setzt Slot 5 unbedingt auf
  sub05. Das ist ORIGINAL-Verhalten: das Original seedet Thread-Slot 1 jedes Gameplay-Bild neu auf sub01 (FUN_8003f038
  @0x8003f064-84, im Port seit e53fb043), und schon vorher lief sub01 im Port einmal beim Eintritt NACH sub00 (seit
  aa346af1, `git show aa346af1:re15_port/engine/src/scd_room_setup.c` Zeile 162-163) — die Korrektur "seit e53fb043"
  aus Fortsetzung 3 ist damit zurueckgenommen (nur Code-Lesung, kein Bisect-Lauf). Die nutzer-gelenkte
  Strom-Rekonstruktion scd_vm.c s_power_gates (2026-06-14: Strom aus -> Ereignis 3 auf 5) bekommt Ereignis 3 vom
  Gemaelde deshalb nie zu sehen; bei Leon faehrt der Rolladen am echten Weg nicht. Der Rolladen-Ton haengt an sub03 und
  spielt, sobald sub03 laeuft (Lauf int_l3, RE15_FORCE_EVENT=3); bei Elza (1141) laeuft er am echten Weg (Lauf int_d3).
- Lesart Nachtrag 2 (Orchestrator, AUFTRAG.md Runde 38) enthaelt zwei Saetze, die sich ausschliessen: "Der Riegel der
  Dachtuer bleibt fuer Elza wie im Original ("You've opened the lock." danach)" und "Johns Weg aufs Dach danach: durch
  den Gang und die Dachtuer" (ebenso Nachtrag 14 Schritt 6 "kurzer Abgang durch die Dachtuer"). Der Riegel sitzt innen
  (Riegel-Zone Slot 2 im Rechteck der Dachtuer x -8250..-6250, z -750..250, Elza loest sie auf der Gangseite aus); geht
  John hinaus, kann er ihn von aussen nicht wieder vorlegen. Umgesetzt ist der Weg durch die Dachtuer (beide Texte
  verlangen ihn; ein danach wieder verriegeltes "You've opened the lock." waere ein neues Logikloch): John oeffnet den
  Riegel beim Hinausgehen (sub05-Zeilen @0x00FC8..@0x00FD5 ohne Nachricht + @0x00FDC), Elza findet die Tuer offen.
- Beobachtung aus Lauf int_wzl/Z (`F_belege/n2_tuer1131_zu.png`): ueber der Tuer in ROOM1131 haengt im Hintergrundbild
  das Original-Schild "Briefing Room"; die Texte folgen dem Nutzer-Wortlaut "Dining Room" (Hinweis und Schluesselname).

## 7. Fuer den Nutzer
- **Ablauf Elza (neu):** Roys Leiche in ROOM1021 zeigt nach "He's dead. / Poor Roy..." den Hinweis "There's an empty key
  tag on his belt. / It reads "Dining Room"." — in ROOM1011 liegt an Roys alter Liegestelle der **Dining Room Key**
  (RE2-Herz-Schluessel) — die Tuer ROOM1131 -> Besprechungsraum ist bis dahin verschlossen ("It's locked. / The door is
  marked "Dining Room".") — im Besprechungsraum hoert man hinter dem geschlossenen Notrolladen Klopfen und Johns "Help me
  out." mit der Kampfmusik des Raums — das Gemaelde ("There's a switch below.") hebt den Rolladen, John steht dahinter
  und kommt heraus, dann direkt "Are you okay? What's happened?" — solange die Gorillas leben, sagt jede Tuer "I cannot
  leave him alone with those monsters." — danach laeuft John (Kamera Blick nach Sueden) aus dem Bild zur Dachtuer, man
  hoert sie auf- und zugehen, danach springt die Kamera auf Elzas Ansicht zurueck (Fortsetzung 4: vorher blieb sie auf
  der Szenen-Kamera stehen und Elza lief aus dem Bild); die Dachtuer ist dann offen und fuehrt zu John aufs Dach
  (ROOM1171).
- **Riegel der Dachtuer:** John hat ihn auf der Gangseite vorgelegt und oeffnet ihn beim Hinausgehen. "You've opened
  the lock." kommt in diesem Ablauf deshalb nicht mehr (sonst oeffnete Elza einen Riegel, durch den John eben gegangen
  ist).
- **Rolltor-Ton ROOM1140/1141:** jedes Hochfahren des Notrolladens spielt jetzt die drei Rolladen-Toene aus ROOM1131
  (Klick, Motor, Anschlag) — Elza 1141 beim Gemaelde-Schalter (gemessen am echten Weg), Leon 1140 bei sub03. Hinweis:
  Leons Gemaelde meldet im Port "The switch is not responding..." (Original sub01 @0x00C7A stellt den Schalter jedes Bild
  darauf); hochfahren sieht man den Rolladen dort nur ueber sub03. Falls der Befund in ROOM1140 aus Elzas Raum (1141)
  stammt: dort ist er jetzt mit Ton.
- **Neue Assets (Paket-Gate):** `re15_port/shared_assets/RE2/KLOPF1060.EDT`, `.VH`, `.VB` (RE2 ROOM1060 snd0,
  bytegleich). Gegenstand/Modell/Icon/Bild des Schluessels sind eingebacken (gen/dining_r38_schluessel.inc).
- **Sprachdateien (synchro, ersetzbar):** `synchro/STAGE1/room1141/main40.wav` = "Are you okay? What's happened?"
  (Schnitt aus main02.wav ab 1,24 s), `synchro/STAGE1/room1141/main42.wav` = "Help me out." (Schnitt aus
  room1171/main01.wav ab 1,95 s; Id 42 ist eine reine Sprach-Id ohne Text). Eine eigene Aufnahme unter demselben Namen
  ersetzt sie ohne weiteren Schritt.

## 8. Nachpruefen (Befehle)
- Riegel: `ctest --test-dir re15_port/build -R "r38_dining1141" --timeout 600 --output-on-failure` (Integration je Teil
  `integration_r38_dining1141_w|z|d|l`, unter Last je 8-40 min).
- Einzelteil direkt: `cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=D -P re15_port/tests/integration/test_r38_dining1141.cmake`;
  Auswertung eines vorhandenen Laufs ohne neues Spiel: zusaetzlich `-DAUSWERTEN=<dir>/lauf_<id>`.
- Ton-Mitschnitt gegen die Se_on-Takte: `python re15_port/tools/r38_dining/ton_mitschnitt.py <arbeitsordner des Laufs>`.
- Druckzahlen je Abfrage: `re15_port/build/tests/unit/test_r38_dining1141.exe takt`.
- Programme neu erzeugen/pruefen: `python re15_port/tools/r38_dining/programme_bauen.py [--pruefen]`.
