# Spur B — Gegenpruefung des Bauplans (Hebetisch ROOM1150/1151, Cursor statt Direktoeffnung)

Stand: FERTIG (Gegenpruefung vor dem Bau, kein Port-Code geaendert). Pruefer: Skeptiker, nicht Autor von
`B_hebetisch.md` (Stand a98aa263). Alle Zahlen unten selbst gelesen: RDT-Bytes per Python aus
`re15_port/shared_assets/PSX/…`, SCD per `re15_port/tools/scd_dump_room.py`, EXE per
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf `info/Re1.5/PSX.EXE` (Auslieferungsstand geprueft:
`@0x80026e4c` = `08 00 e0 03`, nicht der Patch-Sprung), RE2 aus `info/re2leon/PL0/RDT/ROOM2130.RDT`,
Port-Code dieses Baums.

## Urteil

**haltbar_mit_auflagen.**

Der RE-Kern haelt vollstaendig: jede tragende Adresse/Byte-Folge des Dossiers stimmt (Tabelle (a)), die Lesart
"Cut 4, Kuppel unten rechts, Halt vor `For` @0x0FC0" passt zu Wortlaut und Bild, und der Halt-Weg ist gegen den
Vorschalt-Weg richtig begruendet (Cut_chg merkt den ANGEZEIGTEN Cut). Die Cursor-Wahl ist sogar staerker belegt als
das Dossier sagt (derselbe Cursor in acht Raeumen, (b)/(f)).

Der Plan hat aber drei echte Defekte im Einbau, die das Dossier nicht sieht, und zwei Punkte, an denen es den
Auftrag verlaesst:

1. **Der Cursor-Tick liefe unter dem offenen Inventar** (Plan §5.2/§5.3: Tick am Kopf von `re15_game_step`, Schranke
   nur "Text offen / RE15_PAUSE_SCD"). Das Port-Menue setzt `g_re15_pauseflags` nicht, `re15_game_step` laeuft jedes
   Bild, die Pad-Woerter werden unmaskiert veroeffentlicht -> D-Pad im Inventar schiebt den Cursor, Quadrat im
   Inventar loest "Nothing happened." oder den Kuppel-Klick aus. (c)1.
2. **Install nur am Tuerweg** (Plan §5.2: nur `scd_room_setup.c`). Der Boot-/CONTINUE-Weg ruft die Hebetisch-Installs
   separat in `main.c` (4658/4672/4685) — genau die Luecke, die in Runde 30 die Sicherung nach dem Laden
   verschwinden liess. §4.1 "Modus wird beim Raumaufbau zurueckgesetzt" stimmt so nur fuer den Tuerweg. (c)2.
3. **Sprachdatei-Behauptung falsch** (§3.5/§8.3): `re15_dialog_open_mask` spielt keine `main20.wav`; nur der
   `Message_on`-Opcode reiht Sprache ein. (a)/(e).
4. **Abbruch weggelassen und dem schlafenden Nutzer als Frage hingelegt** (§8.1), obwohl der Auftrag "Abbruchtaste ->
   Cursor-Modus verlassen (Vorbild 11F0/RE2)" verlangt und VERTRAG §3 Nr. 2 Fragen an den Nutzer verbietet. Das genannte
   Vorbild 11F0 HAT einen Ausgang (EXIT-Zelle -> sub17 @0x015FA). (b)/(f).
5. **§3.4 "Kein Raum zeigt einen Text bei Fehldruck" ist widerlegt:** ROOM4020 sub12 zeigt im Cursor-Modus
   "The button doesn't respond..." und kehrt in den Cursor zurueck — das uebersehene RE1.5-Vorbild fuer
   "Nothing happened". (f).

Mit den Auflagen unten ist der Plan baubar; keine davon verlangt einen neuen RE-Grundsatz, alle Belege stehen hier.

## (a) Tragende Konstanten/Adressen selbst nachgeprueft

| Behauptung (Dossier) | selbst gelesen | Ergebnis |
|---|---|---|
| Ausloeser ROOM1150/1151 main00 @0x00D7E | beide RDT: `2c 01 00 31 00 00 d8 aa e0 b1 dc 05 e4 0c ff 00 18 04 00 00`; `ff 00 18 04` je Raum genau 1x (@0x00D8C) | bestaetigt |
| Schild-Text Slot 4 @0x00DBA | `2c 04 01 31 00 00 74 aa f8 ad 68 10 50 14 00 00 ff ff 00 00` (Rechteck umschliesst Slot 1 vollstaendig) | bestaetigt |
| sub04 @0x0F96..0x10B6 (Opcode-Folge §3.2) | eigener `scd_dump_room.py`-Lauf: Set(2,0,1) @0x0F96, Set(2,2,1) @0x0F9A, Se_on(2,0x0A) @0x0FA2, Sleep 5 @0x0FAE, `29 04` @0x0FB2, `32 00 24 af cf fe cc bb` @0x0FB4, Sleep 5 @0x0FBC, `0d 00 18 00 0f 00` @0x0FC0 … Pos_set Park @0x109E, Set(5,0,0) @0x10A6, Set(2,0,0) @0x10AA, Set(2,2,0) @0x10AE, `2a` @0x10B2, Evt_end @0x10B4 | bestaetigt |
| 1151 = 1150 um -0x22 | Bytevergleich 1150[0xF96:0x10B6] == 1151[0xF74:0x1094]: True | bestaetigt |
| Halte-Signatur 2/240 | eigene Suche ueber 240 RDTs: ROOM1150 @0xFB2 (Halt 0xFC0), ROOM1151 @0xF90 (Halt 0xF9E), sonst keine | bestaetigt |
| Cut 4 @0x00E0 | flag 0, fov 32946 (H 257), pos (-21942,-2160,-18378), tgt (-19980,-1566,-18396), in 1150 und 1151 gleich. Zusatz: 1150 hat 9 Cuts; die anderen Nahkameras 5/6/7 blicken vom Modell weg (tgt z -26676 / x -25002 / z -27126) -> Cut 4 ist die einzige Modell-Nahansicht | bestaetigt |
| Cut_chg merkt ANGEZEIGTEN Cut | `800402c0 lbu a1,4068(a1)` (0x800b0fe4) … `800402e4 sb a1,16251(at)` (0x800b3f7b) … `800402fc sh a0,4068(at)` | bestaetigt |
| Cut_old stellt ihn her | `8004033c lbu a0,16251(a0)` … `80040364 sh a0,4068(at)`, `80040378/7c/84` Bit 0x100 aus | bestaetigt |
| Sce_key_ck 0x51 = GEHALTEN | `8004293c lw v0,-14488(v0)` = 0x800ac768 | bestaetigt |
| Opcode 0x52 = FLANKE | `80042978 lw v0,-14484(v0)` = 0x800ac76c, sonst Instruktion fuer Instruktion gleich | bestaetigt |
| Add_speed ohne Grenze | `80040f40..80040f7c` nur `lw/lh/addu/sw` auf +52/+56/+60 | bestaetigt |
| Typ-4-Anhebung -900 | `8002c234 lbu v1,8(s1)` / `8002c23c bne v1,v0(=4)` / `8002c24c addiu v0,v0,-900` / `8002c250 sw v0,96(s1)` | bestaetigt |
| Spieler-Dispatcher-Gate | `80031c54 lw a0,-13760(a0)` (g_pauseflags) / `80031c78 bltz a0,0x80031da8` | bestaetigt |
| Preset-Tabelle @0x80073dbc | `read … 16 --w 2`: [4096,8192,16384,32768,4096,16384,128,128,8,64,8,4,32768,8192,128,64] -> virt 0x0040/0x4000 <- roh 0x80, virt 0x8000 <- roh 0x40 | bestaetigt |
| sce-1-Pausemaske | `80043098 lhu a3,2(v0)` / `800430a4 sll a3,a3,16` -> FUN_80027e68 `80027ecc or` / `80027ed0 sw` in g_pauseflags | bestaetigt |
| 11F0-Cursor | Obj_model_set @0x00E54 `2d 00 04 00 00 00 00 01 00 00 9e b3 00 00 9c 58 …` (Typ 4, (-19554,0,22684)); Prop-Tabelle @0x0240: obj 0 = TIM 0x18DAC / MD1 0x1928; MD1 5556 B (naechstes MD1 0x2EDC); TIM 33312 B, flag 9, CLUT (0,480) 256x1, Bild 64x256 Halbworte = 128x256 8bpp | bestaetigt |
| 11F0-Bewegung | sub01 `51 01 01 00`@0x01098->sub02, `…04…`@0x010B0->sub03, `…02…`@0x010C8->sub04, `…08…`@0x010E0->sub05; sub02..05 `2f 02 c8 00` / `2f 02 38 ff` / `2f 00 c8 00` / `2f 00 38 ff` | bestaetigt |
| Cut 10 von 11F0 @0x01A0 | fov 26684, pos (-19628,-17442,22616), tgt (-19628,15928,22617) | bestaetigt |
| RE2-Klick | ROOM2130.RDT @0x01192 `36 02 0a 01 00 00 9b a0 00 fc f4 d3`; `RE15_PANEL_SE_KLICK 0x0A` (re15_audio.h:206); Lader `load_re2_panel_se_pc` (audio_pc.c:999) ohne Raumbedingung, Dateien in `shared_assets/RE2/` vorhanden | bestaetigt |
| Glyphen | ROOM1000 @0x00D22 `04 02 2a 4b 50 44 45 4a 43 00 51 4a 51 4f 51 3d 48 57 01 00` ("Nothing unusual."); ROOM3001 @0x0199D `00 44 3d 4c 4c 41 4a 41 40` (" happened") | bestaetigt |
| Texte je Raum | 1150: 15 (Sektion @0x12F8), 1151: 4 (@0x10EC), Id 20 frei; `re15_msg_install_text` schreibt in die raumlokale Tabelle, `re15_msg_clear_room_block` raeumt sie beim Abbau | bestaetigt |
| EDT-Laute | 1150 @0x145E8: [0x0A] `00 00 45 11` [0x0B] `00 00 55 11` [0x0C] `00 00 66 16` [0x0D] `00 00 77 16`; 11F0 @0x3794 [0x0A..0x0D] leer | bestaetigt |
| Mess-Sonde | `probe_r34n_b_messung.exe` liegt gebaut vor, Ausgabe `B_belege/probe_r34n_b_messung.txt` deckt §3.2/§3.6/§3.7 | bestaetigt |
| **Sprachdatei main20.wav wird abgespielt, wenn der Nutzer sie ablegt** (§3.5, §8.3) | Sprache reiht nur `scd_queue_voice` (scd_vm.c:1582) aus `Message_on` ein; der Untersuchungsweg `re15_scd_show_message` sagt woertlich "No voiceover" (scd_vm.c:1513), `re15_dialog_open_mask` (msg_common.c:356) fasst Audio nicht an | **widerlegt** |
| **"Kein Raum zeigt einen Text bei Fehldruck"** (§3.4) | ROOM4020: Zelle Slot 9 (`2c 09 05 44 …` @0x00750) -> sub01 Member_cmp(15==9) @0x00888 + `51 01 40 00` @0x00892 -> `04 ff 18 0c` @0x00896 -> sub12 @0x00B14: Bank 5 aus, Pause aus, `2b 01 ff ff` @0x00B44 (msg 1 @0x0BAA "The button doesn't respond..."), Evt_next, Bank 5 wieder an, `09 0a 02 00` @0x00B6E, Set(5,9,1) @0x00B72 | **widerlegt** (gilt nur fuer "ausserhalb jeder Zelle") |
| **§5.3 "tick … dieselbe Schranke wie scd_vm_tick"** | scd_vm_tick hat ZWEI Schranken: innen `RE15_PAUSE_SCD` (scd_vm.c:656) und aussen in main.c 5337/5343/5355 (Wegwerf-Abfrage, Item-Modal, Menue -> kein VM-Tick). Der Plan uebernimmt nur die innere | **widerlegt** |

## (b) Passung zum Nutzerwortlaut (AUFTRAG.md) und zu den Bildern

* **"beim Modell … nicht direkt aufgeht"**: heutiger Ablauf am Kontaktbogen `m1_cut4_start_bogen.png` angesehen: F226/F236
  Raumkamera (Westwand mit Tuer, Modell nicht im Bild), ab F238 Cut 4, ab F242 teilt sich die Kuppel. Die
  Nahkamera, in der der Nutzer die Kuppel aufgehen sieht, ist Cut 4. Lesart passt.
* **"unten rechts auf die Kuppel"**: `cut4_cursorzustand_F240_320.png` angesehen: die lavendelfarbene Achteck-Kuppel liegt
  rechts unter der Bildmitte. Das Nutzerbild `irons items.png` zeigt die RAUMkamera auf Irons' Schreibtisch; dort liegt
  keine Kuppel unten rechts (vorn Mitte-links ein gemalter Rundbau, rechts ein gruener Turm) — es stuetzt also die
  Cut-4-Lesart, es widerspricht ihr nicht. Passt.
* **"die Kuppel"**: Lupe `kuppel_trefferflaeche_lupe.png` angesehen: Deckel (rot) und Podest (gelb) ueberlagern sich auf der
  Oberseite (orange) und bilden fuer das Auge EIN Achteck. Deckel+Podest als Trefferflaeche ist damit die Lesart
  "zu seinen Bildern"; §8.2 ist keine offene Nutzerfrage, sondern entschieden (Auflage 5).
* **"unseren Cursor"**: staerker belegt als im Dossier. Nicht nur 11F0 — MD1 und TIM des Cursors sind in acht
  RE1.5-Raeumen BYTEGLEICH (SHA1 MD1 f1f6f0d8…, TIM ed8d82b1…: 11F0, 4020, 30E0, 1080, 2040, 5050, 1100, 1230), der Start
  (-19554,22684) Typ 4 steht in allen gleich, und alle diese Raeume zeigen ihn ueber dieselbe Nadir-"Cursor-Buehne"
  pos (-19628,-17442,22616) -> tgt (-19628,15928,22617) (11F0 Cut 10, 1080 Cut 1, 4020 Cut 4, 30E0 Cut 2, 2040 Cut 14,
  5050 Cut 4, 1100 Cut 12, 3050 Cut 15; 10D0/1230 dieselbe Kamera bei z -23184, 11E0 bei x 19222). RE1.5 projiziert den
  Cursor also IMMER mit einer eigenen Buehnenkamera ueber ein gemaltes Nahbild — genau das, was §5.1 Nr. 2 tut. Die
  Projektion ueber Cut 10 statt ueber Cut 4 ist damit nicht nur bequem, sondern die RE1.5-Bauart.
* **"Nothing happened"**: Lesart ok; das direkte RE1.5-Vorbild (4020 sub12, (f)) bestaetigt Ablauf und Stummheit.
* **"Dafuer soll es das Klick-Geraeusch geben, wie … 11F0"**: Klick nur auf der Kuppel — passt zu 11F0 (op_sce_key_ck
  toent nur, wenn Arbeits-Objekt `member_0b != 0`, scd_vm.c:4932-4950) und zur Nutzerentscheidung 2026-09-26.
* **Abbruch (bequemere Lesart gewaehlt):** der Auftrag verlangt "Abbruchtaste -> Cursor-Modus verlassen (Vorbild
  11F0/RE2)". Das Dossier widerlegt die TASTE (kein RE1.5-Cursorraum fragt eine andere Maske ab; RE2 ROOM2130 hat gar
  keinen freien Cursor, sondern fuenf Ja/Nein-Fragen, `analysis/befunde_2026-09-26/re2-schalterraetsel-2130.md` §2.1),
  zieht daraus aber "gar kein Ausgang" und legt die Frage dem Nutzer hin. Das genannte Vorbild 11F0 laesst den Spieler
  heraus (EXIT-Zelle Slot 12 @0x00E40 -> Member_cmp(15==12) @0x0128C -> sub17 @0x015FA: Cursor zurueck @0x01602,
  Slot 1 wieder scharf @0x01682, Bank 5 aus, `22 02 00 00`@0x016E8 / `22 02 02 00`@0x016EC, Cut_old @0x016F0, Cut_auto
  @0x016F2); 10D0/11E0/1230 haben ebenfalls EXIT-Zellen. Kein Ausgang haben nur Raeume mit vorgeschalteter Ja-Frage
  (2040/5050/3050) oder Zwangsloesung (30E0: Einstieg sub09 @0x01038 ohne Frage, einziger Ausgang sub04 @0x00FC0 nach
  zwei Zellen). Die Hebetisch-Szene hat weder Frage noch EXIT-Kunst. -> Auflage 4.
* **Hinweis (keine Auflage):** der Motor-Laut ROOM1150-0x0A (@0x0FA2) kommt beim Tischdruck, 11 Bilder vor dem
  Cursor; nach dem Kuppeldruck oeffnet der Deckel nur mit dem RE2-Klick. Das ist die Folge von "sub04 unveraendert"
  (Dossier §8.4) und im Uebergabetext offen zu nennen, damit der Nutzer es beim Hoeren einordnen kann.

## (c) Softlocks, Regressionen im Original-Ablauf, Laden/Speichern, Raumvarianten

1. **DEFEKT — Inventar-Leck.** Belege: `re15_game_step(&gctx)` wird in main.c:7358 jedes Bild gerufen (umschliessende
   Bloecke: `while (running)` 4790 -> `if (md1_ok)` 5622 -> 6528, keine Menue-Schranke). Im Schritt liegt der geplante
   Haken neben `re15_granate_tick()` (game_step_common.c:1050), die Pad-Woerter werden bei :1103/:1115 veroeffentlicht,
   das Menue-Gate `if (re15_menu_gameplay_frozen()) { re15_menu_fsm_tick(…); return; }` steht erst bei :1239. Die
   Maskierung auf 0xf000 (:1147-1150) greift nur bei `RE15_PAUSE_PAD`, und das Port-Menue setzt `g_re15_pauseflags`
   NICHT (menu_common.c: kein Schreibzugriff; das Original tut es @0x8001cdd8-e8, der Port modelliert es ueber
   `re15_menu_gameplay_frozen`). START ist im Cursor-Modus erlaubt (Gate :1230 prueft PAD/panel_sperre/in_cinematic,
   (2,0) ist keins davon — das Dossier sagt es selbst in §7 Nr. 4). Folge: D-Pad im Inventar bewegt den Cursor im
   Hintergrund, Quadrat auf einem Item oeffnet "Nothing happened." oder loest Klick + FREI aus. Das 11F0-SCD-Vorbild hat
   das Problem nicht, weil der VM-Tick unter dem Menue gar nicht laeuft (main.c:5355). Gleiches gilt fuer Item-Modal
   (5343) und Wegwerf-Abfrage (5337). -> Auflage 1.
2. **DEFEKT — Laden in ROOM1150/1151.** Die drei Hebetisch-Module werden an ZWEI Stellen installiert:
   scd_room_setup.c:413/418/423 (Tuer, `scd_room_reenter`) und main.c:4658/4672/4685 (Boot/CONTINUE). Der Kommentar dort
   (main.c:4641-4657) dokumentiert, dass die Sicherung genau an dieser Luecke nach dem Laden fehlte. Der Plan (§5.2 Zeile
   scd_room_setup.c, §5.5) nennt nur die erste Stelle. Liegt die Signatursuche in `install()` (so §5.2: "Zustand
   zuruecksetzen, Signatur suchen"), kommt nach CONTINUE in 1150/1151 kein Halt und damit kein Cursor — der alte
   Direktablauf. §5.3 legt die Suche dagegen in `haelt()`; der Plan widerspricht sich. -> Auflage 2.
3. **Softlock:** keiner gefunden. Die Kuppel ist immer erreichbar (Cursor ohne Grenze, Add_speed rein additiv; Huelle
   116 x 62 px), der Halt wird nur von `FREI` geloest, und `install()` setzt bei Raumaufbau zurueck. Einziger denkbarer
   Haenger waere ein falscher Treffertest — Riegel R2/R3 decken ihn.
4. **Kein zweites sub04 / kein Schild-Text durch den Cursordruck:** belegt. game_step_common.c:1348-1373: unter
   `RE15_PAUSE_PLAYER` wird `g_aot_action_pressed = 0` gesetzt, bevor `re15_aot_scan` laeuft (Original @0x80031c78).
5. **Original-Ablauf nach dem Halt:** unveraendert. Der Halt liegt vor jedem Schleifenrahmen (op_for, scd_vm.c:926-950,
   legt den Rahmen erst beim Ausfuehren an; Rueckgabe `SCD_R_YIELD` laesst den PC stehen, die VM liest ihn im naechsten
   Takt erneut, scd_vm.c:704-722). Sicherung/Granate haengen am Fenster [@0x101A,@0x1042) bzw. an der Plattformhoehe
   (Fenster, Memory reai-v2-einseitige-schranke-parkposition) — waehrend des Halts y=-305, keines davon trifft.
6. **Mess-Haken:** `RE15_FIRE_AOT` (main.c:7591 -> `re15_aot_fire_slot` -> scd_event_fire, aot_common.c:880) und die
   Sonden (probe_r31_hebetisch:129, probe_r30_granate:304, probe_r30_sicherung_nein:157, probe_r30_sicherung_fahrt:94,
   probe_irons_mittelmodell:93/297 — alle rufen scd_event_fire direkt) gehen an der GENERIC-Ausgabe
   game_step_common.c:2191-2198 vorbei -> kein Halt, alle bestehenden Riegel bleiben gruen. Kehrseite: sie pruefen
   danach einen Weg, den der Spieler nicht mehr hat. -> Auflage 8.
7. **CROSS-Doppelwirkung (nur wenn ein Abbruch gebaut wird):** der Text-Dismiss liest `(g_scd_pad_edge & 0xC000)`
   (msg_common.c:473) und verbraucht die Flanke nicht; 0x8000 ueberlebt die 0xf000-Maske. Ein Tick am Kopf von
   `re15_game_step` saehe im Schliessbild dieselbe CROSS-Flanke -> Text zu UND Cursor-Abbruch. Im VM-Takt (Auflage 1)
   tritt das nicht auf, weil die VM im Schliessbild noch unter `RE15_PAUSE_SCD` steht.
8. **60-Bilder-Modus** (`RE15_FPS=60`, main.c:3593-3600): VM nur jedes 2. Bild (5337ff.), `re15_game_step` jedes Bild ->
   der geplante Tick schoebe den Cursor doppelt so schnell wie 11F0. Mit Auflage 1 erledigt.
9. **Raumvariante 1151:** Datensatz, sub04, Signatur, Cut 4, Huelle gleich (selbst verglichen). Nur der Ladeweg ist
   betroffen (Punkt 2).

## (d) Ressourcen-Kollisionen (VERTRAG.md, andere Spuren)

* Nachrichten-Id 20: VERTRAG §1.3 Spur B; in 1150 (15 Texte) und 1151 (4) frei; raumlokal, beim Abbau geloescht — keine
  Kollision mit Id 20 der Spuren A/E/F in anderen Raeumen.
* TIM-Slot 28 = `RE15_TIM_SLOT_PROP(8)` (main.c:162-163): VERTRAG §1.5 obj_id 8 = B. In 1150/1151 unbelegt (nOmodel 4,
  Port-Props 4..7 -> 8/9/26/27). Kein anderer Hochlader nutzt 28 (Tuerszene 24/25, Waffe 24/25, Gore 46-49, Raum-ESP 36-43).
  11F0 laedt beim eigenen Raumaufbau obj 8 dorthin — unschaedlich, weil B je Cursor-Sitzung neu laedt.
* Kein AOT-Slot, kein Bank-9-Bit — der Vertrag laesst 11/12 bzw. 67/68 ungenutzt. Richtig so.
* Gemeinsame Dateien: scd_vm.c (1 Zeile op_for), game_step_common.c (Ausgabe-Haken), scd_room_setup.c (Install),
  main.c (Zeichnen) — dazu kommt durch Auflage 2 EINE Install-Zeile in main.c (Boot-Weg). panel_zeiger_common.c /
  re15_panel_zeiger.h (Spur C) und op_sce_key_ck bleiben unberuehrt; die 11F0-Panelsperre (`re15_panel_zeiger_sperrt`)
  wirkt nur in 11F0/11F1. Keine Ueberschneidung mit A/D/E (1050), F (1110/1230), G (1170).

## (e) Abnahmeplan: misst er das Nutzer-Symptom am Artefakt?

Ueberwiegend ja: echte exe, Tuerweg mit Aktionstaste, Framedumps (Cursor sichtbar ab Haltbild, Pixel gegen 11F0,
Schritt 8 px je Bild bei 960), Log-Zeilen fuer Text/Klick (`se=10`)/Deckelfahrt/Cut_old, Lade-Weg 1151. Es fehlen die
Faelle, an denen die Defekte oben sichtbar wuerden:

* Inventar im Cursor-Modus (START, D-Pad, Quadrat auf Item, schliessen) -> Cursorlage, Text, Klick unveraendert.
* CONTINUE in 1150 UND 1151 -> Cursor erscheint nach dem Tischdruck (der geplante 1151-Lauf deckt das nur, wenn er den
  Cursor tatsaechlich im Bild verlangt).
* Abbruch (Auflage 4) inkl. "Text mit CROSS schliessen bricht NICHT ab".
* Der Nutzerweg bis zu Ende: Kuppeldruck -> Items im Ruhefenster wirklich aufgenommen (Modal, Flags 53/56) -> Raumkamera.
* Die Sprachdatei-Zeile (`main20.wav`) ist keine Abnahme, solange der Weg keine Sprache abspielt.

## (f) Uebersehenes besseres Vorbild (Original/RE2)?

1. **ROOM4020 sub12 = RE1.5-Vorbild fuer "Nothing happened"** (Belege in (a)): Druck auf eine Zelle ohne Funktion ->
   Text "The button doesn't respond..." mit Maske 0xFFFF (`2b 01 ff ff` @0x00B44), OHNE Laut (kein Se_on im Sub),
   danach ist der Cursor wieder aktiv; Wiederholschutz fuer die noch gehaltene Taste = `Sleep 2` @0x00B6E vor dem
   Wiedereinschalten der Zelle @0x00B72. Der Plan trifft dasselbe Verhalten (stumm, 0xFFFF0000, Cursor danach frei,
   Flanke statt Sleep 2) — er sollte es mit diesem Vorbild belegen statt "Lesart ohne Vorbild". Textform: 4020 msg 1
   (@0x0BAA `04 02 08 30 44 41 … 57 57 57 01 00`) beginnt mit `08` und endet auf "..."; die Zensus-Zahlen des Dossiers
   koennen "." und "..." nicht trennen (beide enden auf 0x57). Eigener Zensus: 45 von 758 Untersuchungstexten beginnen
   mit `08`. Die Form "04 02 + Text + ." bleibt vertretbar, muss aber gegen dieses naechste Vorbild bewusst gewaehlt sein.
2. **Standard-Cursor + Buehnenkamera** (siehe (b)) — staerkt §5.1 Nr. 2.
3. **Ausgang:** 11F0 sub17 (EXIT) als Vorbild fuer "Cursor-Modus verlassen"; mechanisch der eigene Schluss von sub04
   (@0x109A..@0x10B4), siehe Auflage 4.
4. RE2 bietet fuer den freien Cursor kein Vorbild (ROOM2130 = Ja/Nein-Kette); Klick-Beleg aus RE2 bleibt richtig.

## Auflagen (nummeriert, umsetzbar)

1. **Cursor-Tick im VM-Takt, nicht am Kopf von `re15_game_step`.** Bewegen, Druck, Treffertest, Text, Klick und Abbruch
   im Halte-Aufruf aus `op_for` ausfuehren (er laeuft genau dann, wenn die VM laeuft: nicht unter Menue/Item-Modal/
   Wegwerf-Abfrage main.c:5337/5343/5355, nicht unter `RE15_PAUSE_SCD` scd_vm.c:656, im 30-Hz-Takt) — dieselben Schranken
   und dieselbe Pad-Sicht wie das SCD-getriebene 11F0/4020-Vorbild. Die Halte-API braucht dafuer den Thread (neuer PC
   bzw. Rueckgabecode). Falls doch im Schritt: explizit `re15_menu_gameplay_frozen()`, `re15_item_modal_active()`,
   `re15_discard_frozen()`, Todesablauf abfragen UND die Schliess-Flanke eines Textes verwerfen. Riegel: im Cursor-Modus
   Menue auf, 20 Bilder D-Pad + Quadrat, Menue zu -> Cursorlage, `g_re15_hebetisch_klick_zaehler`, Text-Id unveraendert.
2. **Install an BEIDEN Raumaufbau-Stellen:** neben `re15_granate_install` in scd_room_setup.c:423 UND in main.c:4685
   (Boot/CONTINUE), wie Sicherung/Irons-Tisch/Granate. Signatur-Cache an (Raum-Id, raw-Zeiger, raw_size) binden und in
   `install()` leeren. Defensiv: `sicht()`/Tick nur, solange wirklich ein aktiver Thread mit pc == Halt-PC existiert,
   sonst Zustand AUS. Abnahme: CONTINUE in 1150 und in 1151, dann Tischdruck -> Cursor im Framedump.
3. **Sprachdatei-Aussage berichtigen:** `re15_dialog_open_mask` spielt keine Aufnahme (Sprache nur ueber
   `scd_queue_voice` aus `Message_on`, scd_vm.c:1582; Untersuchungsweg "No voiceover", scd_vm.c:1513). "Nothing happened."
   bleibt unvertont wie jeder RE1.5-Untersuchungstext; KEINE main20.wav beim Nutzer bestellen (§3.5/§8.3 streichen).
4. **Ausgang ohne Kuppel bauen** (Auftrag: "Abbruchtaste -> Cursor-Modus verlassen"; Vorbild 11F0 sub17 @0x015FA; keine
   Frage an den Nutzer, VERTRAG §3 Nr. 2). Taste: virtuell 0x8000 = CROSS (Preset @0x80073dbc[15], dieselbe Taste, die
   Texte/Menues abbricht) als PORT-WAHL mit Grund "kein gemaltes EXIT in Cut 4". Wirkung: den haltenden sub04-Thread per
   PC auf **@0x109A** setzen und die Original-Bytes laufen lassen (`2e 03 00` @0x109A, Pos_set Parklage @0x109E,
   Set(5,0,0) @0x10A6, Set(2,0,0) @0x10AA, Set(2,2,0) @0x10AE, Cut_old @0x10B2, Evt_end @0x10B4) statt eigener
   Aufraeumzeilen; am Halt liegt noch kein For-/Block-Rahmen, der Sprung ist sauber. Kein Klick, kein Text. Im VM-Takt
   (Auflage 1), damit das Schliessen von "Nothing happened." mit CROSS nicht zugleich abbricht ((c)7). Riegel: nach
   Abbruch Kamera = gemerkte Raumkamera, Plattform y=-20224, (2,0)/(2,2)=0, Deckel lokal 0, Items unberuehrt, erneuter
   Tischdruck bringt den Cursor wieder.
5. **Offene Nutzerfragen §8.1/§8.2 schliessen:** Trefferflaeche = Deckel + Podest (ein sichtbares Achteck, Lupe), Ausgang
   nach Auflage 4. Keine Wahlfragen im Uebergabetext.
6. **Vorbild ROOM4020 sub12 im Kopf `re15_hebetisch_cursor.h` und im Dossier zitieren** (Adressen (a)/(f)), §3.4-Satz
   "Kein Raum zeigt einen Text bei Fehldruck" berichtigen, Textform (`04 02` ohne `08`, Schluss "." statt "...")
   ausdruecklich gegen 4020 msg 1 @0x0BAA begruenden.
7. **Halte-Bedingung am Anker pruefen, nicht nur am Offset** (wie `re15_panel_zeiger_abnahme_haelt`,
   panel_zeiger_common.c:210-219): pc-raw == Halt-Offset UND die 20 Signaturbytes ab Halt-14 stimmen, sonst kein Halt.
8. **Abnahme-Luecken schliessen:** Integrationshaken `test_r34n_b_cursor.cmake` faehrt den Nutzerweg BIS ZUM ENDE
   (Tischdruck per Eingabeskript -> Cursor -> Fehldruck-Text -> Kuppeldruck -> Items aufgenommen -> Raumkamera ohne Loch),
   dazu Faelle aus (e): Inventar, CONTINUE 1150+1151, Abbruch, CROSS-Schliessen. In den Koepfen der alten
   `RE15_FIRE_AOT`-Haken vermerken, dass sie den Harness-Weg OHNE Cursor pruefen.
9. **Uebergabetext:** den verschobenen Motor-Laut 0x0A (@0x0FA2 beim Tischdruck, Deckel oeffnet danach nur mit Klick)
   als bewusste Folge von "sub04 unveraendert" nennen (Hinweis, keine Frage).

## Widerlegt (Behauptungen des Dossiers, die nicht halten)

* §3.4 "Kein Raum zeigt einen Text bei Fehldruck." — ROOM4020 sub12 @0x00B14 zeigt msg 1 @0x0BAA im Cursor-Modus und kehrt
  in ihn zurueck. Haltbar nur als "kein Text bei Druck ausserhalb jeder Zelle".
* §3.5/§8.3 Sprachdatei `synchro/STAGE1/room1150/main20.wav` wuerde gespielt — der geplante Oeffnungsweg reiht keine
  Sprache ein (scd_vm.c:1513/1581).
* §5.3 "tick … dieselbe Schranke wie scd_vm_tick" — nur die innere Schranke; die aeusseren (main.c:5337/5343/5355) fehlen,
  deshalb das Inventar-Leck.
* §4.1 "Nach dem Laden in ROOM1150/1151 ist der Modus aus (Modul-Zustand wird beim Raumaufbau zurueckgesetzt)" — mit dem
  Install nur in scd_room_setup.c gilt das nicht fuer den Boot-/CONTINUE-Raumaufbau (main.c:4658-4685).
* §1.3 Nr. 3 / §8.1 als Begruendung fuer "kein Ausgang": das genannte Vorbild 11F0 hat einen (EXIT -> sub17 @0x015FA);
  "kein Ausgang" ist nur fuer Raeume mit Ja-Frage oder Zwangsloesung (30E0) belegt.
