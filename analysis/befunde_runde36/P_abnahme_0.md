# Runde 36 — Spur P "story" — ABNAHME 0 (unabhaengig)

Baum `.claude/worktrees/r36_story`, Zweig `r36/story` @ f46b1a80, Basis master 78a4a748 (merge-base geprueft).
Gemessen am selbst gebauten Stand (local_build.sh configure + build: `LOCAL-BUILD-OK (build)`, ninja "no work to do",
exe 2026-10-11 00:40:46 juenger als alle Engine-Quellen der Spur; Arbeitsbaum sauber).
Messwerkzeug: eigenes Skript `lauf.sh` (Scratch, Muster test_r36_story.cmake): Karte per probe_r36_story_karte,
exe-Kopie `re15_pc_Pabn0_<name>.exe`, RE15_CONTINUE_TEST + Karte, RE15_INPUT_SCRIPT (Basis spiel, Start 60),
RE15_MSG_LOG, RE15_STATE_LOG (Position/Gierung/msg je Bild), RE15_EXIT_AT, Bilder RE15_FRAMEDUMP mit
RE15_WINDOW_SCALE=1 (beschleunigter Renderer, KEIN Software-Render).

Stand dieses Berichts: IN ARBEIT (wird fortlaufend geschrieben).

## 0. Gelesen
* AUFTRAG.md (Nachtrag 14:00 woertlich), VERTRAG.md (Zeile P, 1.1-1.4), Dossier P_story.md.
* `git diff master...HEAD` (16 Dateien; der 2-Punkt-Diff zeigt zusaetzlich synchro/room1011.. als "geloescht" —
  das ist master 565da62b NACH der Basis, kein Eingriff der Spur).
* Code-Diff vollstaendig gelesen: story_r36.c, re15_story_r36.h, gen/story_r36_texte.inc, Haken msg_common.c (2),
  scd_room_setup.c (1), platform/pc/main.c (1), Tests test_r36_story.c, test_r36_story.cmake, probe_r36_story_karte.c,
  probes/r36_story.cmake, Werkzeug tools/r36_p/texte_bauen.py.

## 1. Tests (selbst gefahren)
* `ctest -R "^unit_r36_story_"` -> 100% tests passed, 0 tests failed out of 6 (103 Pruefungen im Binary, 0 Fehler).
* `ctest -R "^integration_r36_story_"`: integration_r36_story_leon Passed 367.46 s; _elza laeuft (s.u.).

## 2. RE-Gate (selbst nachgelesen, nicht aus dem Dossier uebernommen)
SCD-Dumps mit `re15_port/tools/scd_dump_room.py` (alle 206 RDTs, 0 Walk-Abbrueche "ABBRUCH"), MIPS mit
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf info/Re1.5/PSX.EXE.

| Zitat im Diff | selbst gelesen | Ergebnis |
|---|---|---|
| ROOM1020 main00 @0x01DEE `2d 06 00 00 01 00 0a 00 00 00 78 ee 00 00 44 d6 00 00 00 0c ..` | Obj_model_set obj 6 (-4488,0,-10684) rot y 3072; davor in main00 KEIN Ifel_ck (erster @0x01E10 `06 00 6c 00` Ck(3,0xC8)) | stimmt (unbedingt) |
| ROOM1020 main00 @0x01DDA `2c 05 03 31 00 00 0e ed 8c d8 e8 03 e8 03 ff 00 18 02 00 00` | Aot_set Slot 5 sce 3 Flags 0x31 Rechteck (-4850,-10100,1000,1000) -> sub02 | stimmt |
| ROOM1020 sub02 @0x01F70 `46 05 00 00 ..` / @0x01F86 `2b 00 ff ff` | Aot_reset Slot 5 sce 0; Message_on 0; Ende sub02 @0x01F9A Aot_reset Slot 5 sce 3 (Platz wieder scharf) | stimmt |
| ROOM11F0/11F1 sub18 @0x016F6 `22 04 f3 01`, "einziger Setzer" | Set(4,0xF3)=1; Suche ueber ALLE 206 SCD-Dumps: genau diese zwei Treffer; Port-Code: kein Schreiber von (4,0xF3) (nur Leser panel_zeiger_common.c:224) | stimmt |
| ROOM1211 sub00 @0x01DEE `2c 08 01 31 00 00 5c aa 00 a8 0e 03 d4 06 0a 00 ff ff 00 00` (unter @0x01DEA Ck(4,0xEC)==0) | Aot_set Slot 8 sce 1 Flags 0x31 Rechteck (-21924,-22528,782,1748) msg 10; k_zelle_satz = dieselben 20 Bytes bis auf pc[1] 08->09 und pc[14] 0a->14 | stimmt |
| ROOM1090 sub00 @0x0230E (28 B, Flags 0xB1) unter @0x0230A Ck(3,0x85)==0 | Aot_set Slot 2 sce 1 Band 1 Viereck (100,1300)(1850,2150)(3550,-2000)(1800,-2800) msg 7; k_schuppen_satz = dieselben 28 Bytes bis auf pc[1] 02->05 und pc[22] 07->14 | stimmt |
| ROOM3060: msg 1-8 nie angezeigt, einziges Message_on sub02 @0x01034 `2b 09 80 ff` | SCD 3060 komplett: Message_on nur msg 9; Text-Platz main00 @0x00EBE msg 10; dazu (im Dossier nicht genannt) sub00 @0x00F2C Aot_reset Slot 1 sce 1 msg 0 — msg 1-8 nirgends referenziert | stimmt (Aussage gilt) |
| PSX.EXE @0x800397e4 `srl a0,a0,31` / @0x800397ec `addu a0,a0,v0` (Variante Elza = Leon+1) | `800397e4: srl a0,a0,31`, `800397e8: jal 0x80013b60`, `800397ec: addu a0,a0,v0` (Delay-Slot) | stimmt |
| @0x80021fc8 (Verstecken wie die Aufnahme) | `80021fc8: sw zero,0(at)` mit at = 0x800b3f98 + idx*0x94 (erstes Wort des Objekt-Eintrags) | stimmt (Form) |

Suchlauf im Diff nach "deferred|tunable|interim|for now|faithful|plausibel|TODO": siehe Abschnitt 6.

## 3. P1 — Roy ROOM1020 erst nach dem Strom

Nutzer: "Ja, wenn es mehr sinn macht, das Roy erst da liegt, wenn der Strom eingeschaltet wurde, dann machen wir das so."

Messung (eigene Laeufe, ECHTER Tuer-Weg ROOM1010 -> ROOM1020, Leon zu Fuss vor Roy, echte Aktionstaste):
`lauf.sh p1_mit|p1_ohne "vor1020 [strom]" "A0.3,W2,U0.25,W0.5,L0.35,W0.5,U0.57,W0.5,A0.3,W6" "480#1020"`
(Tuer-Spawn aus ROOM1010 Door_aot_set Slot 1 @0x008CE: (-3785,0,-7412) Gierung 2048 — im state.log bestaetigt).
* ohne Strom: debug.log `[story36] ROOM1020: Strom (4,0xF3)=0 -> Roy noch nicht da (obj 6 versteckt, Slot 5 sce 0)`;
  state.log F216-218 `pad=8000 PL(-4334,-8670,rot=992)` cam=10 -> kein `[msg]`/`[msgtext]` bis Bild 480.
* mit Strom: `[story36] ROOM1020: Strom (4,0xF3)=1 -> Roy liegt da`; GLEICHER Standort/Druck F216-218 ->
  `[msg] room=1020 id=0`, `[msgtext] room=1020 id=0 "It's the RPD officer, Roy.He appears to have died recently."`,
  state.log F241 `pst=4 .. msg(a=1 fsm=1 id=0)`.
* Bild (Scratch p1_250.png, Bild 250, Cut 10, links ohne / rechts mit Strom): links Gang leer, rechts liegt Roy vor
  Leon, Text "It's" laeuft an.
* Strom-Bit = Generator: (4,0xF3) einziger Setzer sub18 (Abschnitt 2); kein Port-Code schreibt es.
* Elzas ROOM1021 unberuehrt (story_r36.c prueft nur 0x1020; unit_r36_story_roy "ROOM1021 mit (3,0x5F): kein Eingriff").

Urteil P1: **erfuellt**.

## 4. P2 — Leon ROOM1210, Text an Elzas Zelle

Nutzer: "bei Leon ein Text an Elzas Zelle („The cell door is open…") – die leere Zelle als Spur. - kannst du machen."

Messung:
* ECHTER Tuer-Weg ROOM11E0 -> ROOM1210 und zu Fuss den Zellengang hinunter (Spawn aus 11E0 Slot 3 @0x016C4:
  (-26400,0,-2200) Gierung 0), echte Aktionstaste vor der Zelle:
  `lauf.sh p2_tuer "vor1210" "A0.3,W2,U2.6,W0.3,R0.35,W0.3,U8.6,W0.3,R0.33,W0.3,A0.3,W8" "900#1210" RE15_SET_FLAG=3:44`
  (3:44 = die Gitterhaende-Szene ROOM1210 sub00 @0x01EAA Ck(3,0x2C)/sub02 @0x01EC8 Set(3,0x2C) gilt als gelaufen —
  ohne das greifen die Arme Leon im Gang bei z -15000, gemessen im Kalibrierlauf kal1210, Messtechnik, kein Befund).
  debug.log: `[story36] ROOM1210: Text-Platz Slot 9 (Rechteck ROOM1211 @0x01DEE) -> msg 20`,
  state.log F522 `pad=8000 PL(-20631,-21292,rot=2016) cam=5 msg(a=1 fsm=1 id=20)`,
  `[msgtext] room=1210 id=20 "The cell door is open...Whoever was locked up here is gone."`.
  Bild (Scratch p2_tuer.png, Bild 600, Cut 5): Leon vor der OFFENEN Zellentuer, "The cell door is open..." + Weiter-Pfeil.
* CONTINUE in 1210 (Boot-Weg), zwei Aktionsdruecke: `lauf.sh p2_cont "zelle" "A0.3,W4,A0.3,W5" "400#1210"` ->
  Seite 1 "The cell door is open..." (Bild 160), Seite 2 "Whoever was locked up here / is gone." (Bild 240/380),
  Umbruch sauber im Rahmen (Scratch p2_cont.png).
* Platz = Elzas Zellentuer-Platz: k_zelle_satz bytegleich ROOM1211 @0x01DEE bis auf Slot/msg (Abschnitt 2);
  text_platz() legt die Felder genau wie op_aot_set (scd_vm.c:2903-2925, :3043-3055, :3113-3123) ab;
  unit_r36_story_zelle vergleicht Slot 9 (1210) mit dem vom echten op_aot_set gebauten Slot 8 (1211): alle Felder gleich.
* Slot 9 frei: ROOM1210-SCD belegt 0-5 (Tueren), 6 (sub00), 7/8 (Items); kein Sub setzt Slot 9 (SCD-Dump).
  Id 20: ROOM1210 hat keinen Nachrichtenblock; kein anderer Port-Text nutzt Id 20 dort (Savepoint 0xFE).
* Elzas 1211 unberuehrt (story_r36.c prueft nur 0x1210; unit_r36_story_zelle "ROOM1211: kein Port-Platz").

Urteil P2: **erfuellt**.

## 5. P3 — Elza ROOM1091, Spur am ausgebrannten Schuppen

Nutzer: "ROOM1090. Elzas 1091 ist bytegleich Leons Zustand nach der Rettung – Leon war zuerst dort. Hinnehmen und eine
Spur ergänzen („burnt-out shed") << kannst du machen" (+ "ich weiß nicht was du mit den empty extinguisher meinst" ->
KEIN Feuerloescher).

Messung:
* Tuer-Weg ROOM1051 -> ROOM1091 (Elza), Platz gefeuert (eigener integration-Lauf lauf_acfaced8_p3b):
  `[story36] ROOM1091: Text-Platz Slot 5 (Viereck ROOM1090 @0x0230E) -> msg 20`, `[fire-aot] slot=5 at F200 (Raum 1091)`,
  `[msgtext] room=1091 id=20 "It's a burnt-out shed.Someone must have put out the fire."` — belegt: Platz + Id 20
  ueberleben den Tuer-Weg (Aufbau Schritt 7 vor msg-Laden Schritt 11, room_common.c:355/:397).
* ECHTER Fussweg im Hof: Kalibrierlauf kal1091 (Spawn aus 1051 Slot 5 @0x00B7A: (-10100,-1800,4200) Gierung 3072)
  blieb nach 400 Einheiten an der Hofgeometrie haengen (x -9486); ein Fussweg bis zum Schuppen ist ohne Karte des
  Hofs nicht skriptbar (dazu 4 wache Zombies @0x0216E..). Ersatz: CONTINUE am Schuppen + echter Druck:
  `lauf.sh p3_cont "schuppen" "A0.3,W4,A0.3,W5" "400#1091"` -> F60 `PL(0,250,rot=0) msg(a=1 fsm=1 id=20)`,
  `[msgtext] room=1091 id=20 "It's a burnt-out shed.Someone must have put out the fire."`; Seite 1 "It's a burnt-out
  shed." (Bild 160), Seite 2 "Someone must have put out / the fire." (Bild 240/380), Scratch p3_cont.png.
* Platz = Leons Feuer-/Loesch-Viereck (VERTRAG Zeile P "Position = Leons Feuer-/Loesch-AOT in ROOM1090"):
  k_schuppen_satz bytegleich ROOM1090 @0x0230E bis auf Slot/msg; die 7 Feuer (Typ 0x26, ROOM1090 sub00
  @0x02214..@0x0228C) und das Nach-Rettungs-Objekt obj 2 (2410,-2653,-210) liegen im Viereck. (Die Analyse-Idee G1
  "Leons Slot 3 Rechteck @0x02352" waere unbrauchbar gewesen: Slot 3 ist ein Door_aot_set mit Rechteck (0,0,0,0).)
* Wortlaut traegt "burnt-out shed", KEIN Feuerloescher (unit_r36_story_schuppen, Text oben).
* Slot 5 frei (1091: 0/1 Tueren, 2 AUTO sub02, 4 = r35 Schrot); Id 20: kein Nachrichtenblock in 1091.
* Leon 1090 unberuehrt: nach dem Loeschen legt der Port dort KEINEN Platz an (wie Original, unit_r36_story_schuppen).

Urteil P3: **erfuellt** (Fussweg zum Schuppen nicht gemessen — Ersatz Tuer-Weg + CONTINUE-Druck, s.o.).

## 6. P4 — Text-Sweep "nie begegnet"

Nutzer: "Die dürfen sich niemals begegnet sein, das heißt - alle Textstellen, die darauf hindeuten das sie sich begegnet
wären, müssen wir umschreiben."

### 6.1 Eigener, unabhaengiger Sweep (nicht texte_alle.json des Bau-Agenten)
Eigener Dekoder (Scratch sweep_texte.py: RDT+0x3C-Lader wie re15_msg_load_room_block, Glyphen = re15_msg_glyph,
Steuercodes wie re15_msg_decode_text) ueber ALLE 206 ladbaren RDTs (34 weitere sind 4-Byte-Platzhalter):
1229 Nachrichten, davon 469 Dialogzeilen (Modus 00), 758 Untersuchungstexte (Modus 02).
* ALLE 469 Dialogzeilen gelesen (Stage 1-6, beide Varianten), dazu Schlagwortsuche ueber die Modus-02-Texte
  (who|someone|whoever|motorc|bike|cop|police|already|footprint|empty|gone ...).
* Kein Raum hat Leon UND Elza als Sprecher (Zaehlung der Sprecherkoepfe).
* Treffer mit Begegnungs-/Bekanntschafts-Hinweis (Original): 30C1 msg 3 ("Leon?! How is he? / We got separated"),
  30C1 msg 4 ("He's all right" = Antwort darauf), 30C1 msg 7 ("tell him ... we're all right"), 6031 msg 0
  ("Where's Leon?"), 6031 msg 1 ("He wants you to start the train"), 6030 msg 1 ("Where's Elza?"), 6030 msg 12
  ("Keeping the monsters at bay" als Antwort auf "Where's that biker girl?"), 50B0 msg 1 ("Your other friends"),
  6000 msg 0/3 ("Your friends are already here" / "join your friends") — GENAU die 11 Zeilen der Tabelle des
  Bau-Agenten (6030 msg 2 wurde zusaetzlich angepasst, damit Sherry Elza als Fremde vorstellt).
* Geprueft und KEIN Hinweis: 1090 msg 0-6 ("Woman" = Ada, vgl. 1050 msg 6/7 "Ada: ... I need a break" / "I'll take
  you to the medical room"); 2000 msg 2-6 (Leon sah ein kleines Maedchen = Sherry, nicht Elza); 3000 msg 5 ("that
  woman" = Ada, msg 6); 5090/5091 msg 1 ("my friends" — Leons Begleiter am Zug); 6030 msg 3-7 (John/Sherry ueber
  Elza in 3. Person); 1030/1031 msg 8 (anonymes Motorrad); 1021 msg 4/7 ("couldn't call for help", "We're the only
  ones left"); Port-Texte im Baum (adaruf_1050, cut_10f0, irons_tod_1150, leiche_1110_1230, rolltor_1050,
  tuer1060/1120, ziel_1190, hebetisch_cursor_1150): kein Elza-/Leon-Bezug.
* 3060 msg 1-8 (Elza/Sherry/John im Leon-RDT): im SCD von ROOM3060 nirgends referenziert (Abschnitt 2), Elza laedt
  ROOM3061 (eigene Texte, 10x Message_on) -> nie angezeigt; keine Aenderung noetig.

### 6.2 Messung am Stand (angezeigter Wortlaut, `[msgtext]`)
Eigene integration-Laeufe (lauf_a336af60 Leon / lauf_acfaced8 Elza) + eigene lauf.sh-Laeufe:
* 6030 Tuer-Weg 6000 -> 6030 + Marke (P4a): `id=1 "Leon: Is anyone else with you?"`, `id=2 "Sherry: Elza is out there
  fighting the monsters!"`, id 3-8 Original.
* 6030 CONTINUE Cut 0 = Annette zuerst (P4b): `id=11 "Annette: Where's that biker girl?"`, `id=12 "Leon: I've never met
  her. Can you start this thing?"`. Reihenfolge selbst nachgelesen: ROOM6030 sub00 @0x00D28 Switch(var 0A) Case 0 ->
  Gosub 7 @0x00D32; Tuer Slot 1 @0x00C52 ist eine Selbst-Tuer 6030 -> 6030 Cut 0 bei x[-9300..-8400]; die Wagen-Szene
  startet nur ueber sub01 @0x00D6A Cmp(var1==5) (Marke Slot 5 @0x00CC6 x[-19730..-18130]); Spawn aus 6000 Slot 3
  @0x00DDA = (-17970,-20380) Gierung 1100 liegt oestlich der Marke -> "Annette zuerst" ist erreichbar. Bei der
  Selbst-Tuer laedt der Port KEINE Nachrichten neu (game_step_common.c:2163-2186, "No asset reload") — die Overrides
  bleiben stehen.
* 6000 (P4c): `id=0 "Annette: You must be that cop. Your friend is already here."`, `id=3 "... Let's join the others
  and get out of here."`; 50B0 (P4d): `id=1 "Ada: Some other survivors are down there with her too."`.
* Elza 6031 Tuer-Weg 6001 -> 6031 (eigener Lauf p4e, Bilder 560/690): `id=0 "Elza: Sherry! Is anyone else here?"`,
  `id=1 "Sherry: A policeman! He's fighting off monsters. He wants us to start the train."`, `id=2 "Elza: I'll do
  it!"`; Bild: Sprecherfarben (Elza gruen, Sherry rot) erhalten, beide Zeilen im Rahmen (Scratch p4e.png).
* Elza 30C1 CONTINUE mit (3,0x39) (eigener Lauf p4f, Bilder 350/470/590/950): id 2 Original, `id=3 "Elza: Leon? I don't
  know him. I never ran into him at the RPD."`, `id=4 "Marvin: He's a rookie, and he's helping me and Ada escape the
  city."`, `id=7 "Elza: Will you tell them that me and John and Sherry are here and we're all right?"`; die 272-px-Zeile
  von msg 7 ist die bytegleiche Original-Zeile und steht im Rahmen (Scratch p4f_a.png/p4f_b.png).
* Byte-Form: unit_r36_story_texte (selbst gefahren): je Zeile Original an der Id = alt (Offset aus RDT+0x3C), Kopf
  (Sprecher+Farbe) und Schwanz `04 01 01 63` bytegleich, Dauer 99 Bilder = Original, alle anderen Nachrichten des
  Raums bytegleich. gen/story_r36_texte.inc aus tools/r36_p/texte_bauen.py selbst neu erzeugt (Kopie mit
  Scratch-Ausgabe): **bytegleich** zur eingecheckten .inc; `--check` ohne Fehler.
* Einbauort (Abweichung vom VERTRAG 1.4, im Dossier begruendet) selbst geprueft: room_common.c:355
  `scd_room_reenter(...)` vor :397 `re15_msg_load_room_block(...)` — ein Override aus scd_room_reenter waere im
  Tuer-Weg ueberschrieben worden; der Haken am Ende des Laders erreicht Tuer-, Boot-/CONTINUE- (main.c:4484) und
  PSX-Weg (asset_psx.c:701). Begruendung traegt.

Urteil P4: **erfuellt** (Wortlaut = PORT-WAHL, Freigabe durch den Nutzer steht aus — Hinweise H1-H3 unten).

## 7. P5 — Funkraum / Prison Key

Nutzer: "kann genauso gut sein, das sie es versucht hat, aber es eben nicht ging, weil die Kommunikationsanlage ja
zerstört war. In die Richtung würde ich auch gehen."

Pruefung (eigener Dekoder):
* ROOM10F0 msg 0 @0x01174 = ROOM10F1 msg 0 @0x01174: "A large communications device. | It's out of commission. | I
  can't send any messages from / here." (alle 6 Nachrichten beider Varianten gleich).
* Elza ROOM1021 msg 4 @0x023AC "John, I couldn't call for help / using the radio." + msg 5 "The building's electric
  power / is starting to fail..." — "versucht, ging nicht" passt; keine Reihenfolge-Aussage.
* Leon: 10D0 msg 22/23, 1150 msg 7-9 ("the system is damaged", "some officers were trying to repair it"), Port-Szene
  cut_10f0.c msg 8/9/16 ("the communication system is completely destroyed" — gesprochen von Ada, nicht Elza).
* KEIN Text (RDT oder Port im Baum) behauptet, Elza sei vor Leon im Funkraum gewesen.
* Kein leerer Haken fuer Leon: der Diff enthaelt fuer P5 keinen Code (nur Dossier-Abschnitt "P5").

Urteil P5: **erfuellt**.

## 8. Gates

### 8.1 RE-Gate
* Jede verhaltensrelevante Konstante traegt ihren Beleg: RE15_S36_ROY_OBJ 6 (@0x01DEE pc[1]), ROY_SLOT 5 (@0x01DDA),
  STROM_BANK/BIT (4,0xF3) (@0x016F6), Stilllegen = Aot_reset(5, sce 0) (@0x01F70), Verstecken (@0x80021fc8),
  Platz-Saetze = Original-Bytes ROOM1211 @0x01DEE / ROOM1090 @0x0230E; Slots 9/5 = gemessen freie Slots (PORT-WAHL,
  begruendet), Id 20 = VERTRAG 1.2; Textform (Kopf/Schwanz/Umbruch/Seitenwechsel) aus Original-Nachrichten,
  Wortlaut als PORT-WAHL gekennzeichnet (Header, .inc, Dossier, Commits). Commit-Messages tragen die Adressen.
* Stichproben selbst disassembliert/gelesen: 9 Zitate (Tabelle Abschnitt 2), alle stimmen; das Sprungziel des
  Raumladers (@0x800397e8 `jal 0x80013b60`, a0 im Delay-Slot @0x800397ec) gelesen, richtige Binaerdatei (PSX.EXE).
* Suchlauf im Diff nach deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorlaeufig|geschaetzt|approx:
  0 echte Treffer (nur "xxxx"/"xxx0" in Kommentaren). Einziger getenv = Messhaken RE15_MSG_LOG (nur Log, kein
  Verhalten, kein Schalter als Abschluss).
* Der Fix erklaert den Befund: P1 vorher gemessen (Dossier: Roy liegt ohne Strom, (4,0xF3)=0) — nachher selbst
  gemessen: ohne Strom weg, mit Strom da (Abschnitt 3).

### 8.2 Vertrag
* Bank 9: keine Bits benutzt (Bit 92 bleibt Reserve) — story_r36.c liest nur (4,0xF3).
* Nachrichten-Ids: 1210 Id 20, 1091 Id 20 (zugeteilt 20..21); Overrides nur bestehender Ids in 30C1/6030/6031 und den
  Sweep-Treffern 50B0/6000 (zugeteilt "+ Sweep-Treffer"); 1020 keine neue Id. Ereignis 29: nicht benutzt.
* Dateien (`git diff master...HEAD --name-only`, 16): neu story_r36.c, re15_story_r36.h, gen/story_r36_texte.inc,
  tools/r36_p/texte_bauen.py, Tests nur in probes/r36_story.cmake + eigene test_/probe_-Dateien; KEINE Aenderung an
  release/, platform/android/, shared_assets/PSX/, tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt.
* Gemeinsame Dateien nur mit kleinen Haken: scd_room_setup.c +2 (Include + Installer-Zeile am Block-Ende, Kommentar
  "Runde 36 Spur P"), msg_common.c +6 (2 Includes, 3 Zeilen am Ende des Laders, 1 Zeile Messhaken), main.c +3.
* Abweichung vom VERTRAG 1.4 (Overrides im Lader statt als Installer-Zeile): begruendet und selbst nachgeprueft (6.2).
* Tests je Punkt vorhanden und messend: P1 unit_r36_story_roy + integration P1a/P1b; P2 _zelle + P2a/P2b; P3
  _schuppen + P3a/P3b; P4 _texte/_sweep/_reihenfolge + P4a-P4f; P5 ohne Code (kein Test noetig). Arbeitsordner der
  Haken r36_story_<teil>_wd (kein Target-Name), exe-Kopie re15_pc_r36p_<teil>_<id>.exe, RE15_WINDOW_SCALE=1.
* RE15_MIN_TESTS: im Spurbaum 600 Tests registriert (592 + 8); Anhebung in local_build.sh bewusst dem Orchestrator
  ueberlassen (gemeinsame Datei) — korrekt.

### 8.3 Nebenwirkungen
* `ctest -R "^unit_(msg|text|savepoint|action_msg|1090|1210|r31_generator|r34n_c_generator|r34n_e|r34n_f|
  r34n_b_kreuztext|r35_inhalt|r35_cut10f0|r35_cut1150|r33_tuer1120|r34n_d|r34n_a|r35_karte_r1210|r35_raeume|
  re2_room1090|map_1090|plc_back_yaw_1090|room1050|r36_story|scd|room|weste)" -j 3` -> 100% tests passed, 0 tests
  failed out of 99 (u.a. unit_r31_generator: "[OK  ] sub18 laeuft im selben Bild (4:243 @0x016F6)",
  unit_1210_gitterhaende/_arme/_arme_re2, unit_1090_*).
* Installer wirkt nur in 0x1020/0x1210/0x1091; Override-Tabelle nur in 30C1/6030/6031/50B0/6000; Elzas 1021/1211 und
  Leons 1090 per Unit-Riegel unberuehrt. Id-20-Kollision: keine (Savepoint 0xFE, andere Port-Texte raumgebunden).
* Same-Room-Tuer (6030 Slot 1) und Raumwechsel ohne Nachrichtenblock (1210/1091: Lader kehrt frueh zurueck, Id 20
  bleibt) beachtet; unit_r36_story_reihenfolge prueft beide Ladereihenfolgen.
