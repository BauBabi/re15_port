# Runde 39 — Spur J "dining1141": Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r39_dining1141`, Zweig `r39/dining1141`, Stand b2a2f267 (Merge-Basis mit master 22abaed9;
master = 3640d6c1). Gepruefter Diff: `git diff master...HEAD` (17 Dateien, nur Zweig-Aenderungen; die 3 weiteren Dateien
in `git diff master` — UEBERGABE.md, args_welle3_forts.json, test_r38_itemorte.c — sind master-Commits nach dem Abzweig).
Messordner (nicht committet): scratchpad `jabn/` (Laeufe `l_a`, `l_b`, `l_e`, ..., Kontaktbogen `a_*.png`, Werkzeuge
`lauf.sh` (eigene exe-Kopie `re15_pc_jabn_<lauf>.exe`, danach geloescht), `cap.py` (Pegel je Takt aus cap.raw),
`bogen.py`/`crop.py` (Bilder)).

## 0. Stand der Abnahme (fortlaufend)
- 19:03 Bericht angelegt; Bau (configure + build) gruen; Unit-Gruppen 60/60 gruen; RE-Stichproben selbst gelesen.
- 19:06-19:13 Lauf A (echter Weg 1131 -> 1141 -> 1171 mit Elza, 1300 freie Bilder Warten in 1141) ausgewertet.
- 19:13-19:27 Lauf B (dichte Bilder), Laeufe E/F (Gemaelde vor/nach dem ersten Ruf), Gegenprobe-Bau V (ohne
  Punkt-9-Haken), Laeufe V/W, Lauf G (Wiederbetreten) ausgewertet.
- ab 19:21 gezielte Integrationsriegel (13 Tests, -j 2) — Abschnitt 5.

## 1. Bau und gezielte Tests
- `bash re15_port/tools/local_build.sh configure` + `build`: `LOCAL-BUILD-OK (build)`, `ninja: no work to do` — die exe
  (18:18) entspricht dem Code-Stand 2d7ff566; alle spaeteren Commits aendern nur das Dossier (`git show --stat`).
- Generator: `python re15_port/tools/r39_dining/programme_bauen.py --pruefen` -> `dining_r39_programme.inc ok`
  (Programme/Texte/Klopf-Tonteil = Werkzeugausgabe; Listing scratchpad `jabn/listing.txt`).
- Unit (gezielt, Module des Diffs: dining_r38/39, Kartenbits, audio_pc.c BGM/Stimme/SE, scd_vm.c op_message_on,
  Nachrichten-FSM, Heliport 1171): `ctest -R "unit_r39_dining1141|unit_r38_dining1141|unit_r36_kartenbits|
  unit_rotor_bgm_pin|unit_r35_cut10f0|unit_synchro_stimme|unit_voice_ausreden|unit_r39_stimmen|unit_r38_rekorder|
  unit_r38_elzaintro_stimme|unit_se_bank_routing|unit_se_voice_machine|unit_r30_tuer_stimme|unit_audio|
  unit_r34n_f_leiche_stimme|unit_r38_tagebuch|unit_r38_elzatoene|unit_msg_dialog|unit_action_msg_gate|
  unit_r38_heliport1171|unit_scd_flag_fade"` -> **100% tests passed, 0 tests failed out of 60**.
  `test_r39_dining1141 phase_h` direkt: 0 Fehler (Ausgabe: Reaktion Bild 170 genau einmal, Cut 7 ab 1307, John da 1312,
  Tuer zu 1354, Cut 6 ab 1382, Glas 1970 vor msg 6 1991, Blick msg 9..11 0 von 297 daneben, msg 51 3220 / msg 52 3350).
- Integration: siehe Abschnitt 5 (nachgetragen).

## 2. RE-Gate: Stichproben selbst gelesen (richtige Binaerdatei, Sprungziel)
| # | Zitat im Diff | selbst gelesen | Befund |
|---|---|---|---|
| 1 | Punkt 9: PSX.EXE FUN_80044210 @0x8004429c-@0x800442b0 Warten, @0x800442b8 Laden; FUN_800396fc @0x800399b0 vor @0x80039a00 | `re15_disasm.py dis 0x80044270`: `8004428c lbu v0,0x5218(0x800b)` / `80044294 beq v0,zero,0x800442b8` / `8004429c jal 0x80029ac8` (`800442a0 ori a0,zero,1`) / `800442a8 lbu v0,0x5218` / `800442b0 bne v0,zero,0x8004429c` / `800442b8 jal 0x80044564`; Sprungziel `0x80029ac8`: `sh a0,2(v1)`, `sh 1,0(v1)`, `jal 0x8006e3c8` (Task-Warten, a0=1 Bild); `dis 0x800399a8`: `800399b0 jal 0x80044210` ... `80039a00 jal 0x8003ef6c` | stimmt: erst wartet der Musikwechsel die Ausblendung ab und laedt, dann startet der SCD-Init |
| 2 | Punkt 7: ROOM1141 @0x00E94 / @0x00CFE / @0x00D06 / sub03 @0x00FA4 / sub04 @0x00FB6 / @0x00EC4 | Datei `shared_assets/PSX/STAGE1/ROOM1141.RDT`: @0x00E94 `40 00 09 01 e0 e3 74 c3`, @0x00CFE `40 00 09 01 e0 e3 74 c3`, @0x00D06 `18 04`, @0x00FA4 `22 05 00 00 0f 04 08 00 21 05 00 00 02 00`, @0x00FB6 `22 05 01 00 0f 04 08 00 21 05 01 00 02 00`, @0x00EBA `18 03`, @0x00EBC `2b 09 00 00`, @0x00EC4 `3f 00 0f 00` | stimmt (Johns Drehung Modus 9 Bit 1; Original wartet an @0x00EBA nur auf Elza/Bit 0) |
| 3 | Punkt 6: RE2 ROOM1090 @0x02BB0/@0x02BBC/@0x02BC0 | Datei `info/re2leon/PL0/RDT/ROOM1090.RDT`: @0x02BB0 `36 02 21 01 00 00 9c f0 00 00 50 c9`, @0x02BBC `09 0a 05 00`, @0x02BC0 `36 02 21 01 ...` | stimmt (Se_on Satz 0x21, Sleep 5, Se_on) |
| 4 | Punkt 4: RE2 @0x8009A5D4 `88 2a cc 8b 06 00 00 00 db 36 00 00` = DOOR0F | `info/re2leon/PSX.EXE` (t_addr 0x80010000, Datei 0x800 + 0x8A5D4): genau diese 12 Byte; Tabellenbasis 0x8009A520 + 15*12 = 0x8009A5D4 = Eintrag 0x0F; DOOR0F.DO2 = 48076 B wie Tabelle | stimmt |
| 5 | Punkt 2: ROOM1171 @0x0140C `29 04` / @0x0140E `09 0a 05 00` (Sleep 5 nach Cut_chg); Punkt 8: ROOM1171 @0x014C2, ROOM1141 @0x00C78/@0x00C94 `3f 02 06 00`, @0x00CB8 `3f 00 0f 00`, @0x00CCC `3f 00 17 00` | ROOM1171.RDT / ROOM1141.RDT: alle Bytes wie zitiert | stimmt (Formen) |
| 6 | Punkt 4: Pausen Sleep 30/90/90 = RE2 ROOM1060 sub05 +0x2E/+0x58/+0x5E | `re2_scd_dump.py ROOM1060 MAIN 5`: sub05 @0x00F26; @0x00F54 `09 0a 1e 00`, @0x00F7E `09 0a 5a 00`, @0x00F84 `09 0a 5a 00` (= +0x2E/+0x58/+0x5E) | stimmt (Form der RE2-Einbruch-Schleife) |
Suche im Diff nach "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorerst|geschaetzt|approx": **0 Treffer**.
Neue getenv im Code-Diff: nur `RE15_R39J_LOG` (Messschiene) und `RE15_SE_DEBUG` (Log) — kein Env-Schalter als Abschluss.
Konstanten ohne Original-Adresse sind gekennzeichnet: MIX (3,1,4,2,5) = PORT-WAHL; Lagen (Elza vorn / nach dem Kampf,
John an der Dachtuer, Park) = PORT-WAHL mit Projektions-/SCA-Beleg (Dossier 2.5); Texte 50..52 = NUTZER-VORGABE;
Sleep 43/27 = Laengen DOOR1A Ton 0/1 (tueren_02_re2.md 4); Sleep 30/90/90 = Form RE2 ROOM1060 sub05.

## 3. Messlauf A (echte exe, echter Weg mit Elza)
Befehl: `jabn/lauf.sh a plan_a.txt "600#1171" "0-9000/10:fd_" RE15_R38F_GORILLAS_TOT=600 RE15_R38F_GORILLAS_STARR=1`
(Umgebung wie integration_r39_dining1141_d: RE15_PSELECT_AUTO(_SWITCH)=1, RE15_SET_FLAG Nach-1021-Flags, RE15_GIVE=0x43:1,
RE15_DEBUG_JUMP=1131@gp, RE15_AUDIO_CAP_SYNC, RE15_R39J_LOG, RE15_SE_DEBUG, RE15_MSG_LOG, RE15_BGM_CTL_DEBUG; Wegplan =
der des Riegels, aber **WARTE 1300 statt 420** in 1141, damit die ganze Ruf-Schleife laeuft; 600 Bilder in 1171).
Ergebnis: rv=0, 358 s; 1131 -> (Tuer) -> 1141 -> (Dachtuer) -> 1171. Bildnummern = Bilder seit Raumeintritt.

### Punkt 1 — keine Zombie-Geraeusche
- debug.log Abschnitt 1141 (Zeilen 1155..2594), alle Ton-Ereignisse gezaehlt: 21x Klopfen (0x3E/0), 7 Hilferufe
  (0x3E/0x31..0x35), 2x Glas (0x3E/0x21), 2x2 Tuer DOOR1A (Bank 14), 3x Rolladen (0x3B). **0 Zeilen** mit
  `bank=0x3C`, `bank=0x3D`, `bank=60`, `bank=61`, `main42`. Keine Gegner-/Zombie-Tonbank geladen (nur Modelle EM27/EM49);
  die Gorillas sind in Phase H herausgenommen (`[dining] ... Gorillas heraus`).
- Gegenbeleg Ist-Stand (Dossier 1.1, Lauf v0 des Bau-Agenten): 0x3C/0x0B = RE2-1060-Stoehnen je Runde zweimal.
  RE2-Quelle selbst gelesen: ROOM1060 MAIN sub05 @0x00F48/@0x00F58/@0x00F72 `36 02 0a ..`/`36 02 0b ..` = die
  Einbruch-Schleife, aus der Runde 38 die Toene nahm.
- **Urteil: erfuellt.**

### Punkt 2 — John kommt durch die Tuer; Kamera vorne auf Elza, Tuer-zu-Ton, dann Kamera auf die Tuer mit John
- r39.txt Lauf A: John `(-30000,-30000)` (geparkt) von F1 bis F2148; `[scd F2143] Cut_chg(7)`, angezeigt ab F2144
  (Elza (-7806,-12768) rot 3101 = Blick zur Kamera); `Se_on bank=14 id=0` (DOOR1A auf) im Bild F2148; John ab F2149 bei
  (-7350,-1300) (innen vor der Dachtuer, hinter der Kamera von Cut 7); `Se_on bank=14 id=1` (zu) F~2191;
  `[scd F2218] Cut_chg(6)`, angezeigt ab F2219: John steht schon an der Tuer; F2238 Cut 5, John laeuft aus dem Gang.
- Lauf B (deterministisch gleich zu A: r39.txt bis Zeile 4967 bytegleich, `cmp`) mit Bild je 2 Takte:
  `jabn/b_tuer.png` — F2142 Cut 5: Gang und Dachtuer durch die Rolladen-Oeffnung sichtbar und **leer**; F2144..F2218
  Cut 7: Elza frontal mit Balken, Tuer-Toene laufen; F2220..F2238 Cut 6: John vor der geschlossenen Dachtuer, der Kamera
  zugewandt; F2240 Cut 5: John im Gang Richtung Raum. Kein Bild zeigt John vor F2220 (dazu Unit phase_h: "John vor Cut 6
  in keinem angezeigten Bild (0 Bilder)").
- Mitschnitt: Tuer-auf Takt ~5823..5832, Tuer-zu Takt 5865..5872 (RMS bis 3036, Spitze 10278) ueber MAIN16, das bei
  F2218 (Takt 5891, `op=2`) endet.
- Laengen Sleep 43/27 (= DOOR1A Ton 0/1, 1,43 s/0,89 s) am Mitschnitt nachgemessen: in der Tuerfahrt 1141 -> 1171
  (dieselbe Bank DOOR1A, `pitch=0x4f3 (13641 Hz)` wie die Skript-Se_on) klingt der Auf-Ton Takt 10248..10288 (40 Bilder
  ueber RMS 5, Ausklang darunter), der Zu-Ton Takt 10364..10390 (26 Bilder) — passt zu 43/27.
- **Urteil: erfuellt** (Reihenfolge Kamera Elza -> Tuer auf -> Tuer zu -> Kamera Tuer mit John wie im Wortlaut).

### Punkt 3 — kein "shout" fuer John
- 0 Zeilen `Se_on bank=0x3D` / `clip loaded: main42.wav` im Abschnitt 1141 (Lauf A). Die einzigen Stimmen in Phase H sind
  die Nutzer-Dateien help01..05 (`[voice] clip loaded: help03.wav` usw.).
- **Urteil: erfuellt.**

### Punkt 4 — help01..05 im Mix mit Abstaenden, dazwischen mehrfaches Klopfen
- Folge (debug.log, Bild/Takt): Klopfen F35/F40/F45 (Takt 3708/3713/3718) -> help03 F75 -> [Reaktion] -> Klopfen
  F355/360/365 -> help01 F395 -> Klopfen F575/580/585 -> help04 F615 -> Klopfen F795/800/805 -> help02 F835 -> Klopfen
  F1015/1020/1025 -> help05 F1055 -> Klopfen F1235/1240/1245 -> help03 F1275 -> Klopfen F1455/1460/1465 -> help01 F1495
  -> Gemaelde. Alle fuenf Dateien, gemischte Reihenfolge 3,1,4,2,5 (PORT-WAHL, je Durchlauf gleich), Abstand je Runde
  220 Bilder (7,3 s), vor JEDEM Ruf eine Dreiergruppe Klopfen (Abstand 5 Bilder).
- Mitschnitt (cap.raw, `cap.py`): Schlaege Takt 3709/3714/3719 RMS 2480/2484/2502, Spitze 8116/7895/7774 (Grund: alte
  Musik in der Ausblendung RMS ~250); spaetere Gruppen ueber MAIN16: Takt 4029/4034/4039 RMS 3093/2419/2692, Spitze
  10110/8146/8679 gegen Musik-Bilder RMS 700-1900, Spitze 2000-5400. Hilferuf help03 Takt 3752..3783 RMS 4000-7000,
  Spitze bis 16358. Hoerbar ueber der Musik; der Klang selbst (Holzschlag RE2 DOOR0F, "Treppe Holz") ist eine Wahl des
  Bau-Agenten, weil RE2 keinen Tuer-Klopfton hat (Dossier 2.4) — Hinweis an den Nutzer ist im Dossier 7.
- **Urteil: erfuellt** (Klangwahl offen gelegt; Abnahme des Klangs durch Hoeren steht dem Nutzer zu).

### Punkt 5 — Elzas Reaktion beim ersten Hoeren
- `[msgtext] room=1141 id=50 "Elza: Oh, no, this is John! He is in Trouble, I have to help him!"` — Wortlaut mit dem
  Nachtrag (John statt Roy). Offen F176..F274 (r39.txt), 101 Bilder nach dem ersten Ruf help03 (F75). In 1495 Bildern
  Phase H **genau einmal**; danach `b94=1`. Bild: `jabn/a_reakt.png` (F180..F270: Text im Spielbild, Elza an der
  Doppeltuer, Kopf zur Dachtuer).
- Wiederbetreten (Lauf G, Abschnitt 4): beim zweiten Betreten keine zweite Reaktion (`b94=1`) — richtig.

- **Wettlauf gemessen (Lauf F, eigener Lauf):** `lauf.sh f plan_f.txt "1400#1141" ... RE15_DEBUG_JUMP=1141@gp
  RE15_PLAYER_POS=-8150,-10250,2048` (Elza vor dem Gemaelde, Phase-H-Flags wie oben; Wegplan: WARTE 100, dann AKTION
  alle 20 Bilder = Gemaelde-Frage, "Yes"). r39.txt: Klopfen F36/41/46, help03 F76, Gemaelde-Frage msg 13 offen
  F133..F247, (3,0x6A)=1 und Cut 5 ab F250 (Rolladen faehrt hoch), **msg 50 "Oh, no, this is John! He is in Trouble, I
  have to help him!" offen F291..F389 — NACH dem "Yes", mitten in der Rolladen-Szene** (Bild `jabn/f_race.png`:
  F290..F385 Text ueber dem hochfahrenden Rolladen). Ursache im Programm (Listing HAEMMERN39 +0x007A): die Reaktion
  steht nur unter `Ck(9,94)==0`, nicht unter `Ck(3,0x6A)==0` wie jede Klopf-/Ruf-Zeile; die Gemaelde-Frage
  (`2b 0d 80 ff`, Pausemaske 0xff80) haelt den HAEMMERN-Faden im `Sleep 90` nach dem ersten Ruf an, nach dem "Yes" laeuft
  er weiter in die Reaktion. Fenster = Gemaelde-Frage geoeffnet zwischen erstem Ruf (F75) und Reaktion (F165). Wegzeit
  von der Doppeltuer bis zur offenen Frage in Lauf A: 158 Bilder im Gehen (F1512 -> F1670: 98 + 47 Bilder Weg, 8 Drehung,
  2 Taste); Steuerung hat Elza ab F1..F6 -> wer direkt hingeht, oeffnet die Frage um F164 (Fensterkante), wer rennt,
  deutlich frueher (mitten im Fenster). Gegenprobe Lauf E (Frage schon bei F33, vor dem ersten Ruf): kein Klopfen, kein Ruf, keine Reaktion
  (Schleife wird nach dem "Yes" gar nicht betreten) — dort richtig. Pad-Sperre bleibt in Lauf F erhalten (state.txt
  pm=2 durchgehend ab F249, Balken (1,0x1B)), es ist "nur" die Zeile zur falschen Zeit.
- **Urteil: teilweise** — im normalen Ablauf erfuellt, aber die Reaktion kommt nach dem Befreien, wenn der Spieler im
  Fenster F75..F165 die Gemaelde-Frage oeffnet (Mangel M1).

### Punkt 6 — Glasbruch vor "What was that?!"
- debug.log: msg 5 (John, main05.wav 6,64 s) offen F2703..F2902; `Se_on bank=0x3E id=0x21 ... GLAS1090 Glasbruch` F2903
  (Takt 6576) und F2908 (Takt 6581); `[msgtext] room=1141 id=6 "Elza: What was that?!"` offen ab F2929.
- Mitschnitt: Takt 6569..6575 RMS < 40 (Stille), Takt 6576..6590 RMS 8000-10200, Spitze 28673 (Glas), msg-6-Stimme
  (main06.wav) ab Takt ~6602. Der Glasbruch steht also allein, direkt vor Elzas Zeile.
- Nebenbefund (kein Mangel): der zweite Se_on 0x21 nach 5 Bildern startet dieselbe Welle neu (Takt 6581.. = Takt 6576..
  bitgleiche Pegelfolge), es klingt also ein Glasbruch mit Neuanschlag, nicht zwei ueberlagerte — wie die RE2-Form.
- **Urteil: erfuellt.**

### Punkt 7 — John schaut ab "...some kind of gorilla..." zu Elza
- msg 9 = `"John: Looks like some kind of gorilla, that would explain how they managed to enter through the windows!"`
  (F3296..F3464), msg 10 (F3466..F3564), msg 11 (F3567..F3713). r39.txt je 10 Bilder: John (-5822,-10144) rot=2017,
  Elza (-7996,-10326), `blick=-22` (= -1,9 Grad) in **jedem** Bild von F3290 bis F3700; ab F3710 laeuft er (Original
  sub02 @0x00F8E) in die Ecke. Vorher (Dossier 1.3, Lauf v0): rot 3341 = +117 Grad.
- Bild `jabn/a_blick.png` (Cut 5, F3300..F3700): John steht rechts und ist Elza (links) zugewandt, Gesten zu ihr hin.
- **Urteil: erfuellt.**

### Punkt 8 — nach dem Tod der Gorillas zwei Zeilen Johns mit passenden Animationen
- debug.log Lauf A: `F4312 MESSHAKEN RE15_R38F_GORILLAS_TOT: 2 Gorillas heraus` (Kill-Flags, keine Schusszahl),
  `[scd F4342] Cut_chg(8)`; msg 51 `"John: Puuuh, that was close! I thought we end up as Gorilla meal...."` offen
  F4375..F4473, msg 52 `"John: Elza, follow me, I think I might have found a way out of here!"` offen F4505..F4603;
  danach (F4584 Cut 7) laeuft John zur Dachtuer, F4715 geparkt, (9,67)=1. Wortlaut = NUTZER-VORGABE ("...." statt "…."
  weil die Schrift kein Auslassungszeichen hat).
- Blick (r39.txt): Elza (-5800,-21500) rot 2047, John (-7550,-21500) rot 4095, `blick=0` von F4375 bis F4580.
- Gesten (r39.txt + Lauf B je 2 Bilder): msg 51 `s1=0 mo=15` (F4375..F4470) dann `mo=23` — Bild `jabn/b_puh.png`: John
  dreht sich zu Elza, Hand vor der Brust, Hand an die Huefte (Gesten des Originals zu msg 1 "I'm glad to see you're still
  alive", sub02 @0x00CB8/@0x00CCC). msg 52 `s1=2 mo=6` (F4495..F4580) — Bild `jabn/b_wink1.png`/`b_wink2.png`: John reisst
  den rechten Arm in 6 Bildern waagerecht hoch und haelt ihn **42 Bilder (1,4 s) starr auf Elzas Kopfhoehe** — in der
  Hand die Pistole, Lauf an Elzas Kopf (Ausschnitt `jabn/b_waffe.png` F4510/F4540) —, dann faellt der Arm in 6 Bildern
  zurueck. Kein Heranwinken. Der Nutzer hat genau das inzwischen selbst bemaengelt (AUFTRAG.md Nachtrag 4, 19:25: "John
  sollte am Ende der Cutscene ... nicht die Waffe auf Elza richten, sondern das heranwinken/Folge Animation abspielen").
- Diese Geste ist Plc_motion(2,6) — dieselbe Port-Darstellung, die das Original in ROOM1171 zu "Elza, hurry up!" spielt
  (@0x014BA `2b 00 00 00`, @0x014C2 `3f 02 06 00`, @0x014C6 `43 00 80 00`, danach @0x014CE `3f 00 02 00` = (0,2),
  selbst gelesen); an genau dieser Stelle bemaengelt der Nutzer in 1171 eine "komische Kletter Animation statt der ran
  wink Animation" (Spur K; ob (2,6) oder (0,2) gemeint ist, ist offen). Der
  Bau-Agent fuehrt das selbst als OFFEN. Solange nicht geklaert ist, ob der Port Johns Bank 2 / Clip 6 richtig zuordnet,
  ist "passende Animationen" fuer msg 52 nicht belegt; die gemessene Darstellung (starrer Zeigearm auf Elzas Kopf) passt
  nicht zu "follow me".
- Sichtbarer Sprung (Lauf B, r39.txt + `jabn/b_sprung.png`): Cut 8 ab F4343 angezeigt, Elza wird im selben Bild
  umgesetzt; John steht bis F4358 bei (-7535,-21362) und springt in F4359 auf (-7550,-21500) (138 Einheiten), **16 Bilder
  nach dem Schnitt, im Bild** (`Pos_set John an seinen Platz im Kampf ... - beim Kamerawechsel`, Listing +0x04BA, steht
  hinter Sleep 1/10/5 der Elza-Zeilen). Mit eingefrorenen Gorillas klein; steht John im echten Kampf weiter weg, wird
  der Sprung groesser (Mangel M3).
- Nebenbefund: msg 52 steht noch 20 Bilder (F4584..F4603), waehrend die Kamera schon auf Cut 7 schneidet und John
  losrennt.
- **Urteil: teilweise** (Zeilen, Reihenfolge, Blick, Abgang danach: erfuellt; Animation zu msg 52: Mangel M2).

### Punkt 9 — keine Panik-Musik beim Wechsel 1141 -> 1171
- debug.log 1171: `[bgm] stage=0 room=17 entry=5572 -> MAIN32`, `Sce_bgm_control slot=0 op=1 ... capTick=10365`
  (1171 main00 waehrend der aufgeschobenen Ausblendung -> nur gelatcht), `Ausblendung fertig`, MAIN32 geladen.
- Mitschnitt: Takt 10292..10362 Stille (Tuerfahrt), Takt 10364..10390 RMS 1079/2082/1643/1237/664/476/359/.../30 =
  der Tuer-Schliesston (`[tuer] Sequenz fertig ... Schliesston 1`; dieselbe Abklingform wie Johns DOOR1A-Schliesston in
  1141, Takt 8385..8411), Takt 10392..10422 RMS 0,7 (Stille), ab Takt 10424 MAIN32 ansteigend (RMS 28 -> 1080).
  Kein 60-Bilder-Nachhall wie im Ist-Stand (Dossier 1.5, v0: Takte 8138..8202 Pegel bis 2263 = MAIN16).
- Mechanismus selbst gelesen (Stichprobe 1): Original laedt die neue Musik vor dem SCD-Init; der Haken in
  ss_seq_ctl_ex latcht waehrend `s_bgm_pending` nur — der Befund (op=1 von 1171 trifft den noch geladenen MAIN16)
  wird damit erklaert. Gegenprobe ohne den Haken: Abschnitt 4.
- **Urteil: erfuellt** (Gegenprobe Abschnitt 4: ohne den Haken kehrt MAIN16 zurueck, mit ihm Stille).

## 4. Gegenproben (eigene Laeufe)
- **Lauf E** (Gemaelde vor dem ersten Ruf, `RE15_DEBUG_JUMP=1141@gp`, Elza vor dem Gemaelde, AKTION ab F31): Frage
  F33..F148, "Yes" -> kein Klopfen, kein Ruf, keine Reaktion (r39.txt `se=0/0/0`, `b94=0`), Szene laeuft normal
  (Cut 7 F432, Cut 6 F507, msg 40..). Richtig: Elza hat noch nichts gehoert.
- **Lauf F** (Gemaelde nach dem ersten Ruf, vor der Reaktion): Reaktion nach dem Befreien — siehe Punkt 5 / Mangel M1.
- **Lauf G** (Wiederbetreten in Phase H nach der Reaktion; `RE15_DEBUG_JUMP=1141@gp`, Spawn (-7600,-17600), Flags wie
  oben + (9,66) fuer die offene Tuer 1131): 1. Besuch msg 50 einmal (debug.log Z. 900), WARTE 400, Doppeltuer -> 1131,
  zurueck -> 1141; 2. Besuch 800 Bilder: Klopfen + help03 F75 / help01 F295 / help04 F515 / help02 F735, **keine msg 50**
  (`b94=1`, kein `[msgtext] room=1141 id=50` nach Z. 1059). Reaktion genau einmal: bestaetigt.
- **Laeufe V/W (Punkt 9, Fix erklaert Befund?):** V = exe aus einer Quellkopie des Stands b2a2f267, in der NUR der
  Punkt-9-Haken (`if (!s_bgm_pending)` in ss_seq_ctl_ex, 3 Zeilen) entfernt ist (scratchpad `jabn/v_src`, eigener
  Bau `jabn/v_build`, Kopie `re15_pc_jabn_v.exe` neben der Baum-exe); W = die gebaute exe; beide mit demselben Wegplan
  (`plan_v.txt` = Riegel-Wegplan, WARTE 420). Spielzustand V == W (r39.txt bytegleich bis Zeile 3561, `cmp`);
  1141-Eintritt: Mitschnitt V == W (Takt fuer Takt gleich, der alte 1131-Titel lief dort schon mit voller Lautstaerke).
  1171-Eintritt (Werkzeug des Riegels `test_r39_dining1141 pegel <lauf>` + `cap.py`):
  | Lauf | op=1 1171 | Takte op+30..op+55 | Takte op+70..op+100 | Verlauf |
  |---|---|---|---|---|
  | W (mit Haken) | Takt 9476 | **Pegel 0** | 559 (MAIN32) | Tuer-zu-Ton 9474..9500, Stille 9504..9531, MAIN32 ab 9534 |
  | V (ohne Haken) | Takt 9478 | **Pegel 496 -> FEHLER** | 559 (MAIN32) | nach dem Tuer-zu-Ton bleibt RMS 700..1100 bis 9512, klingt bis 9536 aus (= MAIN16, ~60 Bilder), dann MAIN32 (Werte ab 9540 wie W) |
  | A (mit Haken, WARTE 1300) | Takt 10365 | Pegel 0 | 559 | wie W |
  r39.txt V == W komplett (5644 Zeilen, `cmp`). Damit ist belegt: der Haken ist die einzige Ursache des Unterschieds,
  und ohne ihn kommen die ~2 s Panik-Musik (MAIN16) beim Betreten von 1171 zurueck — der Fix erklaert den Befund.

## 5a. Nachtrag 4 des Nutzers (AUFTRAG.md, 2026-10-18 19:25 — NACH der Abgabe 19:01, waehrend dieser Abnahme in den
Zweig committet: 4d7c7b34, 2f889c9f, 18f4306a, f60715a8)
Nicht Teil der geprueften Abgabe; Stand der gebauten exe dazu (fuer die Nachbesserung):
| Nachtrag-4-Punkt (Wortlaut gekuerzt) | Stand b2a2f267 | Beleg |
|---|---|---|
| N4.1 "Elza soll das Rolltor oeffnen und dann noch ganz normal laufen koennen" | nicht erfuellt | Lauf A state.txt: nach dem "Yes" (msg 13 zu F1858) `pm=2` von F1860 bis F3698 ohne Unterbrechung — Rolladen, Tuer-Auftritt und Szene laufen am Stueck |
| N4.2 Szene erst nach dem Entriegeln der Tuer; Elza rennt zur anderen Kamera, dreht sich zur Tuer, hebt die Waffe (ohne Schusswaffe im Inventar das Messer) | nicht erfuellt | BEFREIUNG39 +0x00C8..+0x00DE: `Plc_dest WALK` (Modus 4) + TURN, kein Entriegeln, kein Waffe-Heben; Bild `jabn/b_tuer.png` (Elza steht mit gesenkten Armen) |
| N4.3 John schiesst ab und an auf die Gorillas und zieht ihre Aufmerksamkeit | nicht erfuellt | BEFREIUNG39: John RUN in die Ecke + Gosub sub04, danach nur die Warteschleife `Do / Evt_next / Edwhile Ck(4,0xF2)==1` (+0x046E); kein Schuss-Befehl in dining_r38/r39 (`grep -i schiess|schuss|plc_gun|shoot` leer) |
| N4.4 John soll am Ende nicht die Waffe auf Elza richten, sondern heranwinken/Folge-Animation | nicht erfuellt | = Mangel M2 (`jabn/b_waffe.png`) |

## 6. Vertrag (VERTRAG.md Runde 39)
- Pfade (`git diff master...HEAD --name-only`): nur `analysis/befunde_runde39/{AUFTRAG.md (Orchestrator-Hinweis),
  J_dining1141.md}` und `re15_port/{engine/src,include,platform/pc/src,tests/unit,tests/integration,tools/r39_dining}`.
  Kein `release/`, `platform/android/`, `shared_assets/PSX/`, keine Test-CMakeLists (neue Riegel ueber
  `tests/unit/probes/r39_dining1141.cmake`, GLOB `tests/unit/CMakeLists.txt:4060`).
- Bank-9: nur Bit 94 (zugeteilt 94/95), in `test_r36_kartenbits.c` k_port[] eingetragen (+2 Zeilen), Riegel gruen.
- Nachrichten-Ids: 50/51/52 (Text), 53..57 (Sprach-Ids der help-Dateien, ohne Text), 58 (Warte-Zeile) — alle im
  zugeteilten Bereich 50..59; `synchro/STAGE1/room1141/` hat keine main50..58.wav (nichts umgewidmet), Nutzer-Dateien
  help01..05.wav unveraendert (Zuordnung im Code).
- Gemeinsame Dateien (Kommentar "Runde 39 Spur J" ueberall): scd_vm.c +2, dining_r38.c +6/-2 (J-eigen), dining_r38_pc.c +1,
  test_r36_kartenbits.c +2, **audio_pc.c +5** (help-Stamm 2 Zeilen + Punkt-9-Haken 3 Zeilen). audio_pc.c ist im VERTRAG
  1.3 den Spuren A und B zugeteilt, nicht J; Punkt 9 (Musik beim Wechsel, VERTRAG 1.4 "J aendert nur den Uebergang/die
  Musik") braucht ihn aber, und die Haken sind klein — bei der Integration mit den A/B-Haken in re15_voice_load_clip
  zusammenfuehren (Hinweis H2, kein Mangel).
- Port-Bank 0x3E (Se_on pc[1]): im Baum frei (0x3A SZL, 0x3B/0x3C/0x3D Runde 38 F); global abgefangen wie die R38-Baenke.
  Mit Spur I (10F1) abgleichen (Hinweis des Bau-Agenten).
- Runde-38-Riegel: `unit_r38_dining1141_phase_h`/`_abgang` und `integration_r38_dining1141` Teile D/A sind jetzt
  Platzhalter, die "ok: ersetzt" melden (`if (1) return;` bzw. `elseif(FALSE)`). Die Pruefungen sind in
  `unit_r39_dining1141_phase_h` uebernommen (gegen die master-Fassung verglichen: Rolladen-Ton, msg 0/1/2 nicht, msg 40..11,
  Ecke, Sperre msg 41 an beiden Tueren, Abgang im Cut 7 ohne Schub/Sichtsprung, Riegel (4,0xC3), Ziele 0x17/0x13,
  Wiederbetreten) — **nicht** uebernommen ist die R38-Pruefung "John springt nie (groesster Schritt je Bild)"; der neue
  Ablauf setzt John absichtlich hinter der Kamera um (Tuer-Auftritt, geprueft), aber auch sichtbar nach Cut 8 (Mangel M3).

## 7. Maengel (nummeriert, nachpruefbar)
- **M1 (Punkt 5, mittel):** Elzas Reaktion msg 50 erscheint NACH dem Befreien, mitten in der Rolladen-Szene, wenn der
  Spieler die Gemaelde-Frage zwischen dem ersten Ruf (F75) und dem Reaktionsbeginn (F165) oeffnet und "Yes" waehlt.
  Nachpruefen: `jabn/lauf.sh f plan_f.txt "1400#1141" "0-1400/5:fd_" RE15_DEBUG_JUMP=1141@gp
  RE15_PLAYER_POS=-8150,-10250,2048` -> r39.txt `F250 6a=1`, `F291 msg=1/50`; Bild `jabn/f_race.png`. Zu Fuss von
  der Doppeltuer oeffnet man die Frage um F164 (Lauf-A-Wegzeit 158 Bilder), rennend deutlich frueher — im Fenster.
  Ursache: HAEMMERN39 +0x007A `06 00 36 00 / 21 09 5e 00` prueft nur (9,94), nicht (3,0x6A) wie jede Klopf-/Ruf-Zeile;
  die Frage (`2b 0d 80 ff`, Maske 0xff80) haelt den Faden im Sleep 90 an, nach "Yes" laeuft er in die Reaktion.
  Abhilfe-Richtung: Reaktion zusaetzlich unter Ck(3,0x6A)==0 (oder Bit 94 beim "Yes" setzen) + Riegel "Gemaelde im
  Fenster F75..F165" in unit_r39_dining1141_phase_h.
- **M2 (Punkt 8, "passende Animationen"):** Johns Geste zu msg 52 "Elza, follow me, ..." ist Plc_motion(2,6): der rechte
  Arm schnellt waagerecht hoch und richtet 42 Bilder (1,4 s) die Pistole starr auf Elzas Kopf (`jabn/b_waffe.png`),
  vom Nutzer im Nachtrag 4 (19:25) woertlich zurueckgewiesen ("nicht die Waffe auf Elza richten, sondern das
  heranwinken/Folge Animation"),
  kein Heranwinken, keine Geste Richtung Ausgang (`jabn/b_wink1.png` F4502..F4544, `b_wink2.png`). Es ist dieselbe
  Port-Darstellung, die das Original in 1171 zu "Elza, hurry up!" abruft (@0x014C2 `3f 02 06 00` + @0x014C6 `43 00 80 00`)
  und an der der Nutzer in diesem Auftrag eine "komische Kletter Animation statt der ran wink Animation" bemaengelt
  (Original 1171 sub02: @0x014C2 (2,6), @0x014C6 rueckwaerts, Sleep 20, @0x014CE `3f 00 02 00` (0,2), Sleep 60 — ob der
  Nutzer (2,6) oder das folgende (0,2) meint, ist offen); der Bau-Agent fuehrt die Zuordnung von Johns Bank 2 selbst
  als offen (Spur K). Fuer msg 52 ist damit keine passende Animation belegt, und die gemessene Darstellung (Zeigearm auf
  Elzas Kopf) ist keine Folgen-Geste. Abhilfe-Richtung: Ergebnis der Spur K abwarten/uebernehmen und die Geste am Bild neu abnehmen, sonst
  eine belegte Folgen-/Wink-Geste waehlen.
- **M3 (Punkt 8, klein):** John springt 16 Bilder nach dem Schnitt auf Cut 8 sichtbar um 138 Einheiten (Lauf B r39.txt
  F4358 (-7535,-21362) -> F4359 (-7550,-21500), Cut 8 angezeigt seit F4343; Bild `jabn/b_sprung.png`): `Pos_set John`
  (Listing BEFREIUNG39 +0x04BA) steht hinter Sleep 1/10/5 der Elza-Zeilen, nicht im Schnittbild. Die Runde-38-Pruefung
  "John springt nie" ist beim Ersetzen von unit_r38_dining1141_phase_h nicht uebernommen worden. Abhilfe-Richtung: John
  im selben Bild wie Elza setzen (vor den Sleeps) + Sprung-Riegel im angezeigten Bild.

## 8. Hinweise (keine Maengel)
- H1 Klopf-Klang: RE2 DOOR0F-Holzschlag (Treppe Holz), dreifach — Wahl des Bau-Agenten mangels RE2-Tuerklopfton; am
  Mitschnitt kurz und tief (Abklingen -20 dB nach 34..35 ms, 94..99 % Energie unter 1 kHz), ueber MAIN16 hoerbar.
  Klangabnahme durch den Nutzer (RE2_SOUND_LISTE: ROOM205 "Zombies smashing through wood", ROOM207/209/20A "Door sounds"
  waeren Alternativen zum Anhoeren).
- H2 audio_pc.c ist VERTRAG-seitig A/B zugeteilt; J-Haken (5 Zeilen) bei der Integration mit A/B zusammenfuehren.
- H3 msg 52 steht noch 20 Bilder, waehrend die Kamera auf Cut 7 schneidet und John losrennt (F4584..F4603).
- H4 Runde-38-Riegel D/A und phase_h/abgang sind Platzhalter ("ok: ersetzt"); Pruefungen in R39 uebernommen ausser
  "John springt nie" (M3).
- H5 Glasbruch: zweiter Se_on startet dieselbe Welle neu (keine Ueberlagerung) — Form wie RE2.
- H6 (nicht gemessen): help01..05 laufen ueber den Sprach-Kanal; ein Untersuchungstext waehrend eines Rufs wartet im
  Stimmen-Riegel bis zu dessen Ende (help01 = 3,42 s). In Runde 38 galt dasselbe fuer main42 (0,95 s).
- H7 Port-Bank 0x3E mit Spur I abgleichen.

## 9. Urteil
| Punkt | Urteil | Kurzbeleg |
|---|---|---|
| 1 Zombie-Geraeusche weg | erfuellt | Lauf A: 0x Bank 0x3C/0x3D im Abschnitt 1141; Gorillas in Phase H heraus |
| 2 John durch die Tuer, Kamera Elza -> Tuer-Toene -> Tuer mit John | erfuellt | Lauf A/B: Cut 7 F2144, Tuer auf F2148, John F2149 hinter der Kamera, Tuer zu ~F2191, Cut 6 F2219 mit John; `b_tuer.png` |
| 3 kein "shout" | erfuellt | 0x main42 / 0x3D |
| 4 help01..05 im Mix + mehrfaches Klopfen | erfuellt | Lauf A: 3,1,4,2,5 (+ Wiederholung), je 3 Schlaege Abstand 5, hoerbar im Mitschnitt |
| 5 Elzas Reaktion beim ersten Hoeren | teilweise | normal erfuellt (einmal, Lauf A/G); Lauf F: Reaktion nach dem "Yes" mitten in der Rolladen-Szene (M1) |
| 6 Glasbruch vor "What was that?!" | erfuellt | Lauf A: Glas F2903/F2908, msg 6 F2929, Stille davor |
| 7 John schaut ab msg 9 zu Elza | erfuellt | Lauf A: blick -22 (1,9 Grad) msg 9..11; `a_blick.png` |
| 8 zwei Zeilen Johns mit passenden Animationen | teilweise | Zeilen/Blick/Ablauf ja; Geste zu msg 52 = John richtet 1,4 s die Pistole auf Elzas Kopf, vom Nutzer im Nachtrag 4 woertlich zurueckgewiesen (M2); sichtbarer Sprung Johns nach Cut 8 (M3) |
| 9 keine Panik-Musik 1141 -> 1171 | erfuellt | Laeufe A/W Pegel 0, Gegenprobe V ohne Haken Pegel 496 (MAIN16) |
| N4.1-N4.4 (Nachtrag 4, 19:25, nach der Abgabe) | nicht erfuellt | Abschnitt 5a — offen fuer die Nachbesserung; N4.4 = M2 |

Gates: RE-Gate gehalten (6 Stichproben stimmen, keine Rate-Woerter, Kennzeichnung NUTZER-VORGABE/PORT-WAHL vorhanden);
Fix-erklaert-Befund fuer Punkt 9 per Gegenprobe V/W belegt; Vertrag gehalten (Hinweis H2 audio_pc.c). Tests: siehe
Abschnitt 5.
**bestanden = false** (Punkte 5 und 8 nur teilweise: M1, M2, M3).

