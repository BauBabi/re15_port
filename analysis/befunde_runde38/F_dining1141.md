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
  die Vorgaenger hatten diesen Stand schon gebaut). Umbau 1141 auf Nachtrag 2, Plan Abschnitt 3.0.

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
  Irons waere ein fremder Sprecher). Datei synchro/STAGE1/room1141/john_help.wav (Nutzer kann sie ersetzen).
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

## 4. Messung nachher

## 5. Tests

## 6. OFFEN

## 7. Fuer den Nutzer
