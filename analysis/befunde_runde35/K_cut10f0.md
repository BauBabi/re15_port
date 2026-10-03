# Runde 35 — Spur K "cut10f0": Neue Szene ROOM10F0 (Ada/Leon/Marvin), Karte 11C0+1150, MAIN01

Baum: `.claude/worktrees/r35_cut10f0`, Zweig `r35/cut10f0`, Basis master 154a73c1. Datum 2026-10-03.
Vertrag (VERTRAG.md): Bank-9-Bits 71 (Szene gesehen) + 72 (Reserve); Nachrichten-IDs ROOM10F0 6..30;
Ereignis 20 in 10F0; AOT-Slots nach Zensus; neue Logik in `engine/src/cut_10f0.c` + `include/re15_cut10f0.h`.

## 0. Auftrag (woertlich, AUFTRAG.md Z.40-67, 92)
1. Erstes Betreten ROOM10F0: Ada hinten rechts an den Monitoren bei CUT2; Kamera CUT2 (Ada) -> CUT0 (Leon);
   Leon Arm strecken "Hey - how did you came in here?"; Leon laeuft zu ihr, steht links von ihr;
   Woman: "Did you really think there was only one staff card for the Communication Room?" (180-Grad-Arm-Geste);
   Woman: "Anyway... the communication system is completely destroyed. We won't reach anyone with it anymore..." (Kopfschuetteln, Kopf gesenkt);
   Marvin durch die Tuer (Tuerknall-SE, Marvin laden, Cut0); Marvin "Leon! You already made it!" (Arm);
   Marvin laeuft Richtung Cut2, steht schraeg links; Leon "Hey Marvin, glad you made it!" (Arm);
   Leon "Allow me to introduce you. This is..." (Arm Richtung Ada); Ada "... Ada, Ada Wong"; Leon "Ada Wong.";
   Marvin "Hello, glad to meet another Survivor! I'm Marvin."; Leon "Anyway... looks like we can't contact anyone with this thing anymore." (Kopfschuetteln);
   Marvin "Ohh... what do we do then?..."; Leon "..."; Pause; Leon "I know! The patrol car! We can use it to get out of here!";
   Marvin "Yeah, you're right! That could be our way out!"; Leon "Okay, Marvin, you go with Ada to the parking lot and wait there. I'm going to get Chief Irons, and I'll be right behind you!";
   Marvin "Alright! Sounds like a plan. Take care Leon!"; Marvin und Ada rennen hintereinander zur Tuer; kurz weiter Balken; Ende.
2. Danach Karte auf: erst kurz ROOM11C0 markiert (wie 1150 davor), dann kurz ROOM1150 blinkend; beide blinken bis besucht.
3. Bis Leon ROOM11C0 erreicht: durchweg MAIN01 als BGM.
4. Animationen flexibel waehlen wie bei vergleichbaren Dialogen.

## 1. Arbeitsprotokoll (fortlaufend)

### 1.0 Start
- `git status --porcelain` leer, HEAD 154a73c1, Zweig r35/cut10f0. OK.
- Pflichtlektuere AUFTRAG.md + VERTRAG.md vollstaendig gelesen. Vorbild adaruf_1050.c / re15_adaruf.h, map_hint_common.c, D_adaruf.md gelesen.

### 1.1 Zensus ROOM10F0 (RDT selbst gelesen: tools/scd_dump_room.py, rdt_msgdump.py, r34n_d/kameras.py, r34n_d/rbj_zensus.py)
- RDT 225092 B, 12 Kameras, 4 Props; main_scd @0xF30, sub_scd @0x10D4 (2 Subs), msg @0x1168 (6 Nachrichten Id 0..5).
- main00 @0x00F32..0x010D0: Door_aot_set Slot 0 (Tuer -> ROOM10D0, Rechteck (8800,-1300,1000,2000)), Slot 1 (Tuer -> ROOM1090, floor 1),
  Text-Plaetze Slot 2..9 (sce 1): Slot 2 @0x00F72 msg 0 "A large communications device" Rechteck x 7000..7800 z 2600..12300 (Ostwand);
  Slot 3 @0x00F86 msg 3 "There are various devices" Rechteck x 900..6900 z 12200..13000 (Nordwand). Item-Plaetze Slot 11..14, Props obj 1..3.
- sub00 @0x010D8 (Kiste/Flag (3,102)), sub01 @0x01140 (Member_cmp). KEINE Szene, KEIN Sce_em_set, KEIN RBJ (RDT+0x5C = 0; rbj_zensus.py "ROOM10F0: kein RBJ", ROOM10F1 ebenso).
- AOT-Slots belegt 0..9, 11..14 (10 in sub00). Port-Installer fuer 10F0 bisher: keiner. Spur K braucht keinen AOT-Slot (Szene startet automatisch wie ROOM1050 sub03 ueber sub00 @0x00C8A).
- Eintritt von ROOM10D0: ROOM10D0 main00 @0x01174 Door_aot_set Slot 0 `3b 00 02 31 00 00 2c 01 8e e0 e8 03 d0 07 d0 20 00 00 a2 fe 00 08 00 0f 00 ...`
  = Spawn (8400, 0, -350), Gierung 2048, Ziel Raum 0x0F, Cut 0.
- Kameras (RDT @0x60, 0x20 je Satz): Cut 0 @0x060 pos (6535,-3754,6105) Blick -84 Grad (Sued, die Tuerseite); Cut 1 @0x080 pos (5806,-3627,10200) Blick -84;
  Cut 2 @0x0A0 pos (5874,-3618,1851) Blick +88 Grad (Nord: Konsole rechts, Geraetewand hinten); Cut 3..11 Westteil/Buero.
- RVD @0x200 (28 Saetze, 20 B): 0->1 Band z 2900..3900 (x 3300..10300), 1->2 Band z 7000..8000, 2->1 z 6000..7000, 1->0 z 1900..2900, 2->3 x 3600..4100 (z 7600..15600).
- Gierung (actor_locomotion.c:341-344 `x += cos(hdg)*v; z -= sin(hdg)*v`): 0 = +X, 1024 = -Z, 2048 = -X, 3072 = +Z. Tuer-Eintritt 2048 = Blick nach Westen in den Raum.
- BGM-Tabelle UNK_80074828 (audio_pc.c SS_BGMTBL, Index = Raum-Byte + Stage-Versatz): 10F0 (0x0F) 0xff20 = MAIN20; 10D0 0xff1f; 1100 0xff1f; 1110 0xff1b; 1120/1130 0xff17;
  1150 0xff1e; 1040 0xff00; 1030 0x4041 (MAIN01+SUB00, Manuell-Start-Flags); 11B0 0xff1d; 11C0 (0x1C) 0xff56. MAIN01 = Slot 1 (Memory reai-v2-musik-main01).

### 1.2 Messung vorher (gebauter Stand master 154a73c1; exe-Kopie re15_pc_r35k.exe neben der exe; RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=10f0@250)
- Lauf A (Cut 0, RE15_FRAMEDUMP=400:cut0.ppm, RE15_FLOOR_DUMP=1, RE15_EXIT_AT=402#10f0): Leon allein an der Tuer. debug.log:
  "[debug-menu] AUTO-JUMP -> ROOM10F0 (Frame 250)", "[rbj] room 10F0 has no RBJ (RBJ/ROOM10F0.RBJ) — Leon auf PL00-Basis zurueckgesetzt (Arena-Reset @0x80039738)".
  Kein NPC, keine Nachricht, kein Szenenfaden: die Szene existiert nicht. BGM: "[bgm] stage=0 room=0F entry=FF20".
- Lauf B/C (RE15_FORCE_CUT=2 / =1, Framedump Bild 400): Cut 2 zeigt die lange Konsole mit Monitoren RECHTS, hinten die grosse Monitorbank in der Nordost-Ecke —
  dort gehoert Ada hin ("hinten rechts an den Monitoren bei CUT2"). Cut 1 = Gegenschuss nach Sueden.
- Floor-Dump (re15_collision_on_floor, 23474 Punkte, Gitter 100): markiert NUR Zellen-Boden, nicht die Begehbarkeit (der Tuer-Spawn (8400,-350) selbst gilt als "nicht auf Boden",
  obwohl Leon dort steht) -> Standorte werden am gebauten Stand per Framedump/RE15_STATE_LOG eingemessen, nicht aus dem Dump abgeleitet.
- Hinweis (Werkzeug): gdigrab liefert in dieser Sitzung keine brauchbaren Bilder; alle Bilder = RE15_FRAMEDUMP (komponierter Frame vor dem Present).

## 2. RE-Belege (Original-Formen, aus denen die Szene gebaut ist)
Datei-Offsets in re15_port/shared_assets/PSX/STAGE1/ROOM####.RDT; EXE-Adressen = PSX.EXE, Overlay 0x8010xxxx = STAGE1.BIN.
- Szenen-Rahmen: Set(2,7)=1 + Set(1,27)=1 am Anfang, Set(2,7)=0 + Set(1,27)=0 + Plc_ret + Cut_auto(1) am Ende — ROOM1090 sub02 @0x02414/@0x02418 bzw. @0x024BE/@0x024C2;
  ROOM1050 sub03 Schwanz @0x00DF2 `42 00 3c 01 22 02 07 00 22 01 1b 00 01 00`. Letterbox FUN_80021a0c `andi v0,v0,0x10` @0x80021a24, 15 Bilder auf/zu (@0x80021a54/@0x80021a7c).
- Einmal-Riegel als ERSTES Opcode: ROOM11B0 sub06 @0x01478 `22 03 83 01`. Hier Set(9,71) `22 09 47 01` (VERTRAG §1.1 Spur K).
- Auto-Start beim Raumstart: ROOM1050 sub00 @0x00C8A Ck(3,110)==1 -> Sce_em_set Ada + Evt_exec sub03 (die Szene laeuft ab dem Init-Lauf). Port: scd_event_fire(20) im Installer NACH dem Init-Lauf.
- NPC-Spawn: ROOM11B0 main00 @0x01080 `44 00 40 40 00 00 00 ff d0 8a 00 00 d0 8a 00 00 f8 9b 00 00` (Marvin 0x40, geparkt -30000/-30000), @0x01094 `44 01 42 40 ...` (Ada 0x42).
  Handler Sce_em_set FUN_800420a0: Kill-Flag 0xff = kein Gate (@0x80042128), `sh zero,452(s0)` +0x1c4 = 0 (@0x8004216c), grid 0x40 = stehend. NPC-INIT setzt Clip 2 = Ruhe
  (`ori v0,zero,0x2` @0x8011cd8c, `sb v0,148(v1)` @0x8011cd90; Memory reai-v2-platzhalter-als-verhalten).
- NPC "laden" = Pos_set + Dir_set aus der Parklage: ROOM10D0 sub21 @0x01ABC `32 00 fb 1d 00 00 91 50` / @0x01AC4 `33 00 00 00 ea 06 00 00` (Marvin an die Tuer);
  stehen: Plc_dest Modus 6 ROOM11B0 @0x014C0 `40 00 06 3f 00 00 00 00` (0x800517f0: Clip 1 einmal -> Clip-2-Ruhe, `sb v1,148(v0)` @0x80051854, Folgeclip @0x800518c4/@0x800518c8).
- Gehen/Rennen/Drehen: Plc_dest Modus 4 (Spieler ROOM10D0 sub21 @0x01AAE `40 00 04 21 fb 1d 91 50`, NPC ROOM11B0 @0x0175C), Modus 5 (Spieler ROOM11C0 sub02 @0x01868,
  NPC ROOM11B0 @0x015EA), Modus 9 (Spieler ROOM11C0 @0x0185E, NPC @0x018CA). Handler @0x80041be4: `sb 4,+0x4; sb mode,+0x5` @0x80041c14-18, Ankunftsbit +0x1c3 @0x80041c24,
  `sh zero,+0x1c4` @0x80041c4c. NPC-Walk-Subs @0x80076ca0 [4]=0x80051148 [5]=0x80051484; Tempo Sub4 @0x80076c00 = 75, Sub5 @0x80076c80 = 200 (Memory reai-v2-cutscene-npc-system).
  Warteschleife: ROOM1050 sub03 @0x00DCA `11 00 08 00 02 00 12 04 21 05 20 00` (Do / Evt_next+Nop / Edwhile / Ck(5,bit)==0); Bits 0/1/2 = ROOM11C0 sub05 @0x01C3E, ROOM11B0 sub10 @0x018AC, sub11 @0x018BE.
- Gesten (Plc_motion @0x80041b90: `sb a1,148(v0)` @0x80041ba8 = Clip, +0x1c4 @0x80041bc8; Plc_flg Unterbefehl 0 `or v0,v0,a2` @0x80041ffc = 0x80 rueckwaerts):
  Dialogzeile = Message_on, Geste A, Sleep 40, Geste B, Sleep 50, Clip 23 (Hand an die Huefte), Sleep 20 — ROOM11C0 sub02 @0x018A0..@0x018B8; ROOM11B0 sub06 @0x014EE..@0x0150A (Marvin, Clips 15/16/23).
  Bibliothek (Bildbeleg D_adaruf.md §3.5 + D_belege/gesten_katalog_vorn.png, angesehen): 15 = Arm nach vorn ("Hey!"-Griff, 83 Aufrufe) = "Arm strecken";
  19 vor + rueckwaerts = Arm seitlich hinaus, Hand dreht auf = die vom Nutzer in Runde 34 so genannte "180-Grad-Armgeste"; 17 = Arm-Schwung; 18 = Hand zur Brust; 20 = Unterarm nach vorn;
  21 = Hand hoch; 23 = Abschluss. Ada spielt 19 vor+rueck in ROOM11C0 sub02 @0x018D8/@0x018E0/@0x018E4 ("He turned around all of a sudden") -> dieselbe Geste fuer "Did you really think ...".
- Kopfschuetteln mit leicht gesenktem Kopf (exakt die Nutzerbeschreibung, Original-Paar): ROOM11B0 sub06 @0x0154E `41 02 00 00 00 00 2c 01 00 0a` (Plc_neck Modus 2 relativ, Pitch +300)
  + Sleep 30 + @0x0155C `41 04 03 00 00 00 00 00 64 00` (Modus 4 Yaw-Sweep = Kopfschuetteln, 3 Schwuenge) + Sleep 60; identisch ROOM10D0 sub21 @0x01BB4/@0x01BC2 (Leon) und
  @0x01BEE/@0x01BFC (MARVIN), ROOM1170 @0x01766/@0x01774. Handler Plc_neck @0x80041e98 (Ziel = Work-Entity `lw v1,0x154(a0)` @0x80041e9c; Modus-Bits caseD @0x80041ed8-f10:
  1 = 0x04 Weltpunkt, 2 = 0x08 relativ, 3 = 0x2a Nick-Sweep, 4 = 0x58 Yaw-Sweep, 0 = 0x12 loslassen). Nicken ROOM11B0 @0x016B4 `41 03 01 00 00 00 00 00 00 3c`.
  Blick auf Weltpunkt ROOM11C0 @0x01886 `41 01 fb dc 00 00 f5 c7 64 00`. Loslassen @0x01646 `41 00 00 00 00 00 00 00 64 00`.
- Tuerknall = RE2 (VERTRAG §2.2 "Sound ist RE2"): Door_exit-Satz (se 1) des RE2-Tuerarchivs DOOR13, das der Port fuer GENAU diese Tuer spielt
  (gen/tuer_zuordnung.inc "S047 T027 ROOM10F0 -> ROOM10D0 | DOOR13 V0"); Tonteil = DOOR13.DO2[0 .. 0x3DA8) nach RE2-Archivtabelle @0x8009A604 `a8 3d 04 95 08 00 00 00 96 c1 00 00`
  (Tonlader FUN_80014cd0 @0x80014d94), Port-Lader re15_audio_re2_tuer_laden (audio_pc.c:3825, EDT 0xC38 + VB), Abspielen re15_audio_re2_tuer_se(1) (:3863).
  Ausgeloest per Original-Opcode Se_on (Form ROOM10D0 sub21 @0x01A02 `36 02 0c 00 01 00 00 00 00 00 00 00` — dort das RE1.5-Tuergeraeusch vor Marvins "Freeze!") mit der
  Port-Bank 0x0E (RE1.5 FUN_80045024 kennt Bank 0..5 ueber DAT_800b21ec[bank]; 0x0E ist in allen 240 RDTs unbelegt). Beim echten Eintritt liegt der DOOR13-Tonteil schon
  in der Tuerbank (sie ueberlebt re15_audio_load_room_banks, RE2 @0x800597a4..@0x80059818), die eingebackene Kopie (gen/cut10f0_tuerton.inc, sha1 5ec7fd64...) deckt Sprung/CONTINUE.
- Nachrichten-Form: Kopf `04 00 05 cc <Name> 16 05 00 00`, Ende `04 01 01 63` (99 Bilder); Farben Leon 01 (ROOM1090 msg 1 @0x279C), Woman/Ada 02 (ROOM1090 msg 0 @0x275C / ROOM11C0 msg 1 @0x1CD0),
  Marvin 07 (ROOM10D0 msg 14 @0x211E `04 00 05 07 29 3d 4e 52 45 4a 16 05 00 00`); "..." = `57 57 57` (ROOM11C0 msg 8 @0x1E14). Breiteste ausgelieferte Dialogzeile 285 px (ROOM30E0 msg 13 @0x13FD,
  tools/r35_k/texte_bauen.py misst den ganzen Korpus) -> alle 18 Port-Zeilen darunter (max 275 px).
- Gestenbank: 10F0 hat keinen RBJ -> Leihe des ROOM11B0-Blocks (RDT @0x1CB0, 48168 B): rec0 Marker 0x1 (Spieler) 25 Clips, rec1 Marker 0x2 (Gegner 0) 25 Clips, Bildzahlen
  15:20 16:30 17:30 18:20 19:30 20:25 21:35 22:50 23:24 24:30 (rbj_zensus.py liste). Clips 15..23 von 1050/1090/1170 bytegleich (D_adaruf.md §3.4). Binder FUN_8001b3f8 @0x80039a08
  (Marker-Bit 1+i -> Gegner i, Kanal +0x180 = Executor-Sub 0); Port enemy_common.c rbj_resolve_slot (Marker-Bit == Aktor-Slot) + Alias Aktor 2 -> Record 1 (zwei NPCs, ein Record).
- Kartenhinweis: RE2 Opcode 0x84 @0x800591C4 (Modus 4 @0x800591DC, Phase @0x800591E8, Bit 0x8000 @0x800591EC, Nummer @0x80059210), Schirm schliesst auf 0x6000 @0x8006F884;
  Blinkzaehler @0x8006F20C-0x8006F284, Periode 78 Schritte je VBlank (re15_map_hint_periode). Port-Kette K1 (ROOM11C0, Blatt 0 Rechteck 4) -> K2 (ROOM1150, Blatt 4 Rechteck 2);
  "kurz eine Weile" = 3 Perioden = 234 VBlanks ~ 3,9 s = PORT-WAHL (NUTZER-VORGABE ohne Zahl), START/Abbruch springt sofort weiter.
- BGM: FUN_80044210 @0x80044210 vergleicht MAIN/SUB-Slot mit dem Cache (@0x800443B8/@0x800443D0); gleicher MAIN = laeuft durch (@0x80044280 -> LAB_800443b0). FUN_800444b0 spielt
  nur Flag == 0 (Manuell-Start-Flag main_b>>6 @0x800442D0) -> erzwungener Eintrag 0xFF01 (MAIN01 ohne Flag, kein SUB). Weiche in audio_pc.c ss_bgm_entry, Bedingung (9,71)=1 und Zone ROOM11C0 unbesucht.

## 3. Umsetzung (Dateien, Konstanten)
- NEU `include/re15_cut10f0.h` (alle Konstanten mit Beleg/Kennzeichnung), `engine/src/cut_10f0.c` (Programm, 18 Nachrichten, Installer, Weiche, Tick, RBJ-Leihe/Alias, BGM-Weiche),
  `engine/src/gen/cut10f0_szene.inc` (1162 B, 245 Opcodes; Generator `tools/r35_k/szene_bauen.py` mit Vorbild je Opcode-Form),
  `engine/src/gen/cut10f0_tuerton.inc` (DOOR13-Tonteil 15784 B; `tools/r35_k/tuerton_bauen.py`), `tools/r35_k/texte_bauen.py` (Nachrichten + Breitenbudget).
- Haken (je 1-5 Zeilen, Kommentar "Runde 35 Spur K"): scd_room_setup.c (Installer am Ende des Blocks), scd_vm.c scd_event_fire (Weiche nach adaruf), game_step_common.c (Tick vor dem
  Hinweis-Poll), enemy_common.c rbj_resolve_slot (Record-Alias), platform/pc/main.c (Gestenblock-Leihe `pc_rbj_leihen` im Raumwechsel-Pfad), platform/pc/src/audio_pc.c (BGM-Weiche in
  ss_bgm_entry; Port-Bank 0x0E -> RE2-Tuerbank vor der Bank-Weiche), map_hint_common.c (Eintraege K1/K2 mit Folge/Zeitsteuerung, re15_map_hint_request/_eintrag_fuer/_folge/_zeitgesteuert,
  re15_map_ziel_aktiv_n), re15_map_hint.h, menu_common.c (Hinweiskette im offenen Schirm: Zeit oder START -> Folge-Ziel; letztes Ziel schliesst nach derselben Zeit; zweites Kartenziel),
  re15_inv_screen.h (ziel2_* am Ende), re15_inv_screen.c (zweites Ziel in der Kachelschleife, gleiche Phase).
  Boot-/CONTINUE-Weg (main.c geht dort nicht durch scd_room_reenter, Muster Sicherung/Granate/Hebetisch-Cursor/Dokumente): Leihe des Gestenblocks + re15_cut10f0_install
  hinter dem Dokumente-Installer — gemessen (integration Lauf C): ohne diese Zeile lief bei CONTINUE im Raum zwar die Leihe, aber keine Szene ("Szene -1, Leihe 2724").
  (ROOM10F0 hat keinen Speicherpunkt — re15_savepoint.c: 1070/1120/1150/... — der Fall ist nur mit Fremd-Spielstaenden/Werkzeugen erreichbar, aber jetzt abgedeckt.)
  tests/test_support.c: Spion fuer re15_audio_re2_tuer_laden/_se (Unit-Riegel linken ohne SDL-Audio).
- NUTZER-VORGABE / PORT-WAHL (in re15_cut10f0.h gekennzeichnet): Standorte Ada (6000,11500) Gierung 3072, Leon (4800,11500), Wegpunkt (5500,2500), Marvin (3800,9900),
  Adas Abgangs-Wegpunkt (5500,3200); Texte und Reihenfolge woertlich; Zeilenumbrueche unter 285 px; Gestenwahl je Zeile (Tabelle oben); Hinweisdauer 3 Blinkperioden;
  MAIN01 ab Szenenende (Flag (9,71)) bis zur besuchten Zone ROOM11C0, im Raum-Byte 0x1C selbst nie.

### 3.1 Gestenwahl je Zeile (Nutzer: "Sei bei den Animationen ein wenig flexibel ... wie bei vergleichbaren Dialogen")
Form jeder Zeile = ROOM11C0 sub02 @0x018A4 (Geste A, Sleep 40, Geste B, Sleep 50, Clip 23, Sleep 20). Clips = Bibliothek 15..23 (D_adaruf.md §3.5, Bild gesten_katalog_vorn.png):

| Zeile (msg) | Sprecher | Geste | Beleg der Geste |
|---|---|---|---|
| 6 Hey - how did you came in here? | Leon | 15 Arm nach vorn ("Arm strecken") + 23 | ROOM11C0 @0x018A4 "Ada! Where's Marvin?" |
| 7 Did you really think ... | Woman | 19 vor + 19 rueckwaerts ("180-Grad-Geste") + 23 | ROOM11C0 @0x018D8/@0x018E0/@0x018E4 (Ada) |
| 8/9 Anyway... destroyed / We won't reach ... | Woman | Plc_neck Modus 2 (Kopf gesenkt) + Modus 4 (Schuetteln), zurueck Modus 1 | ROOM11B0 @0x0154E/@0x0155C/@0x0156A |
| 10 Leon! You already made it! | Marvin | 15 + 23, davor Plc_dest 9 zu Leon | ROOM11B0 @0x014F6 (Marvin Clip 15), @0x018CA |
| 11 Hey Marvin, glad you made it! | Leon | 15 + 23 | wie 6 |
| 12 Allow me to introduce you. This is... | Leon | 15 (zu Ada gewandt, Kopf zu Marvin) + 23 | wie 6; Kopf = Plc_neck Modus 1 |
| 13 ... Ada, Ada Wong | Ada | 18 Hand zur Brust + 23 | ROOM11C0 @0x018F0 (Ada Clip 18) |
| 14 Ada Wong. | Leon | Plc_neck Modus 3 Nicken | ROOM11B0 @0x016B4 |
| 15 Hello, glad to meet another Survivor! I'm Marvin. | Marvin | 15, 18 Hand zur Brust, 23 | ROOM11B0 @0x014F6; ROOM11C0 @0x018F0 |
| 16 Anyway... looks like we can't contact ... | Leon | Modus 2 + Modus 4 (Kopf gebeugt schuetteln) | ROOM10D0 sub21 @0x01BB4/@0x01BC2 (Leon) |
| 17 Ohh... what do we do then?... | Marvin | 20 Unterarm nach vorn + 23 | ROOM11B0 @0x015A0 (Marvin Clip 20) |
| 18 ... + Pause | Leon | keine (Sleep 70 + 60) | Nutzer: "etwas pause" |
| 19 I know! The patrol car! ... | Leon | 21 Hand hoch, 17 Arm-Schwung, 23 | ROOM1050 rec0 Clip 21 (1x gerufen), ROOM1170 sub02 @0x015F0 (17) |
| 20 Yeah, you're right! ... | Marvin | 15 + 23 | wie 10 |
| 21 Okay, Marvin, you go with Ada ... | Leon | 15 + 23 | wie 6 |
| 22 I'm going to get Chief Irons ... | Leon | 17 + 23 | ROOM1170 sub02 @0x015F0 |
| 23 Alright! Sounds like a plan. Take care Leon! | Marvin | 15 + 23 | wie 10 |

Schnittstelle zu Spur L: (9,71) = "10F0-Szene gesehen" (VERTRAG §0), gesetzt als erstes Opcode; L gatet seine 1150-Szene auf (9,71)=1 und (3,94)=1.

## 4. Messung nachher (gebauter Stand mit cut_10f0.c; exe-Kopie re15_pc_r35k.exe; Bilder = RE15_FRAMEDUMP, Scratch r35k/szene1..4)
Bildnummern = g_engine.frame_count, das beim Raumstart auf 0 springt (= RE15_STATE_LOG "F" und RE15_CAM_TRACE "F").
- Start: debug.log "[cut10f0] ROOM10F0: Szene gestartet (Ereignis 20, Faden 10, Flag (9,71)=0)", "[rbj] Animationsblock von ROOM11B0 geliehen (48168 B)",
  "[rbj] room 10F0 cinematic overlay: 25 clips, 284 kf". Spawns (RE15_SPAWN_DIAG): Ada 0x42 Slot 0 (6000,0,11500) dir 3072; Marvin 0x40 Slot 1 (-30000,0,-30000);
  "[enemy] EM42 loaded ... EM40 loaded" (CDEMD0.EMS). Balken voll ab F15.
- Kamera (RE15_CAM_TRACE / "[scd Fn] Cut_chg"): F6 Cut 2 (Ada an der Monitorbank, Leon hinter der Kamera), F56 Cut 0 (Leon an der Tuer), Leons "Hey - how did you
  came in here?" F71..F160 mit Clip 15 (Arm nach vorn) + 23; Leon geht ab F164 (Modus 4), Kamera folgt den RVD-Baendern: Cut 1 ab ~F260 (Band 0->1 z 2900..3900),
  Cut 2 ab ~F300 (Band 1->2 z 7000..8000), Ankunft F357 bei (4796,11356) Gierung 4019 (= Blick +X zu Ada; Ziel (4800,11500), Modus-4-Ankunft < 100 bzw. Konsole);
  F356 Cut_chg 2 (Dialogkamera). Leon steht LINKS von Ada (x 4796 < 6000), beide vor der Monitorbank (Bilder h_000360..h_000700).
- Nachrichten (RE15_MSG_LOG): id 6, 7 (F396, Ada Clip 19 vor+rueck), 8/9 (Kopf gesenkt + Schuetteln: RE15_NECK_LOG "slot=1 mode=2 tgt=(0,0,300)" dann "mode=4 tgt=(3,0,0)"),
  Tuerknall F704 ("[se] SCD Se_on: bank=14 id=1"), Marvin per Pos_set an der Tuer (8400,-350) F716, F732 Cut 0, id 10 "Leon! You already made it!" (Marvin Clip 15 + 23,
  Blick zu Leon: neck slot=2 mode=1 tgt=(4800,0,11500)), F831 Cut 2, Marvin geht (Modus 4) ueber (5500,2500) nach (3814,9831) = schraeg links vor Leon/Ada (F987),
  danach id 11..23 in dieser Reihenfolge (Leons Kopfschuetteln bei id 16: neck slot=0 mode=2 + mode=4; Nicken bei id 14: mode=3; "I know!" Clip 21 + 17; Pause nach id 18 = 130 Bilder).
- Abgang: Marvin rennt (Modus 5, 200/Takt) ueber (5500,2500) zur Tuer, Ada 25 Bilder spaeter ueber (5500,3200); F2199 Cut 0 (beide laufen durch das Bild zur Tuer,
  h_002200/h_002220), Parken beider per Pos_set (-30000,-30000), zweiter Tuerknall F2217, F2249 Cut 2 (Leon allein, Balken noch 45 Bilder), Faden zu Ende F2272
  ("[cut10f0] Szene zu Ende: Kartenhinweis ROOM11C0 -> ROOM1150 angefordert, MAIN01 bis ROOM11C0"), Balken weg F2286, Flags danach (9,71)=1 (2,7)=0 (1,27)=0,
  player_mode 0, Cut-Automatik an; Kamera bleibt Cut 2 (Leon in Zone 2, kein 2->x-Band greift) — gemessen: cam-trace F2400..F2640 shown=2.
  ⛔ Erste Fassung endete mit Cut 0 + Cut_auto 1: Leon (z 11356) lag ausserhalb jedes 0->x-Bands, die Kamera blieb auf der Tuer stehen (Bilder f_002540ff ohne Leon).
  Behoben durch Cut_chg 2 vor dem Schlussbalken (szene_bauen.py); danach gemessen cam=2 (h_002260ff, Leon im Bild).
- Karte: "[hint] F2294 begin (Zaehler 10, Richtung 1)", Blatt B1 mit ROOM11C0 blinkend (h_002320..h_002400 "POLICE STATION B1", Kachel rot/Umriss im Wechsel, Ton Se(2,0x2B)
  alle 39 Schritte), nach 3 Perioden "[hint] F2402 Folge-Hinweis 1 -> 2 (Zeit)" -> Blatt 3F mit ROOM1150 blinkend (h_002420..h_002540 "POLICE STATION 3F"), nach weiteren
  3 Perioden "[hint] F2517 schliessen (Abbruch/Zeit)" (t = 3,95 s je Ziel, Wanduhr). Normale Karte danach: re15_map_ziel_aktiv_n(0) = Blatt 0/Rechteck 4 (11C0),
  (1) = Blatt 4/Rechteck 2 (1150), bis die jeweilige Zone besucht ist (unit_r35_cut10f0_karte).
- BGM: dieser Rechner hat keinen Audio-Endpunkt ("[audio] SDL_OpenAudioDevice failed: WASAPI"), re15_audio_start_room_bgm kehrt dann vor der Tabelle zurueck ->
  Messung mit SDL_AUDIODRIVER=dummy: "[bgm] stage=0 room=0F entry=FF20 -> MAIN20" beim Eintritt, nach der Szene "entry=FF01 -> MAIN01" (integration_r35_cut10f0, Lauf A).
  Tabellenweiche (unit_r35_cut10f0_bgm): -1 vor der Szene; 0xFF01 fuer 0x0F/0x0D/0x10/0x11/0x12/0x13/0x15/0x04/0x06/0x03/0x1B/0x1F; -1 fuer Raum-Byte 0x1C, andere Stages,
  und sobald die Zone ROOM11C0 besucht ist. Gleicher MAIN-Slot ueber Raumwechsel = "laeuft durch" (FUN_80044210 @0x80044280), d.h. MAIN01 wird zwischen den Raeumen nicht neu gestartet.
- Genau einmal: zweiter Raumaufbau mit (9,71)=1 -> kein Faden, kein Spawn, keine Leihe (unit_r35_cut10f0_einmal; integration Lauf B mit Karte "gesehen").
- Bilder (K_belege/): vorher_cut0_leon_an_der_tuer.png, vorher_cut2_konsole_monitore.png, grundriss_10f0_aots_kameras.png (Floor-Dump + AOT-Rechtecke + Kameras),
  nachher_kontaktbogen_alle_20_bilder.png (F0..F2700 je 20 Bilder), nachher_F0080_cut0_leon_hey.png, nachher_F0360_leon_links_von_ada.png,
  nachher_F0760_cut0_marvin_an_der_tuer.png, nachher_F1100_drei_im_dialog.png, nachher_F2220_abgang_zur_tuer.png, nachher_F2260_cut2_leon_balken.png,
  nachher_F2360_karte_B1_room11c0.png, nachher_F2480_karte_3F_room1150.png, nachher_F2600_cut2_leon_frei.png.
- Echter Weg (integration_r35_cut10f0 Lauf A, Spielstand ROOM10D0 + CONTINUE + Aktionstaste): "[save] CONTINUE: resumed in room 10d0", "[tuer] Sequenz Archiv 2 DOOR13
  Variante 1 ... Schliesston 1, Ton geladen", "[esp] room 10F0", "[cut10f0] ROOM10F0: Szene gestartet (Ereignis 20, Faden 10, Flag (9,71)=0)", "[rbj] room 10F0 cinematic
  overlay: 25 clips, 284 kf", Nachrichten 6..23, 2x "[se] SCD Se_on: bank=14 id=1", "[bgm] stage=0 room=0F entry=FF01 -> MAIN01(flag 0) SUB--", "[hint] F2411 Folge-Hinweis
  1 -> 2 (Zeit)", "[hint] F2529 schliessen (Abbruch/Zeit)", cam-trace bis F2700 shown=2 (Leon (4792,11359)). Lauf B ((9,71)=1): keine der Zeilen. Lauf C (Spielstand IM Raum,
  Boot-Weg main.c): Szene + Leihe wie A.

## 5. Tests
- `tests/unit/probes/r35_cut10f0.cmake` (GLOB): unit_r35_cut10f0_programm / _texte / _tuerton / _szene / _einmal / _karte / _bgm (test_r35_cut10f0.c, echte VM + Spielschritt
  in der Reihenfolge von main.c) — alle 7 gruen (ctest -R "^unit_r35_cut10f0_").
- integration_r35_cut10f0 (test_r35_cut10f0.cmake, Karte aus probe_r35_cut10f0_karte: ROOM10D0 (1900,0,-7000) Gierung 2048 vor der Tuer, Flags (3,50)=1 Schloss offen /
  (4,247)=1 Freeze-Szene gesehen): Lauf A CONTINUE -> Aktionstaste -> DOOR13-Sequenz -> ROOM10F0 -> Szene (Nachrichten 6..23 in Reihenfolge, 2x Se_on Bank 14 Satz 1,
  Szenen-Ende, Folge-Hinweis 1 -> 2, schliessen, entry=FF01, Kamera Cut 2); Lauf B mit (9,71)=1: nichts davon; Lauf C (Karte "in10f0", CONTINUE im Raum = Boot-Weg
  main.c): Szene + Leihe. Ergebnis: 8/8 gruen (ctest -R r35_cut10f0: 7 Unit-Teile + integration_r35_cut10f0 146 s).
- Erster Lauf der ganzen Suite: 485/486 — rot NUR unit_cam_selfheal (test_cam_selfheal.c M6: "ROOM10F0: die Kamera wechselt nie"), 3/3 deterministisch rot, kein Flattern.
  Ursache = Folge dieser Spur: die Sonde betritt ROOM10F0 mit frischen Flags und erwartet den szenenfreien Raum (freie RVD-Kette); jetzt startet beim ERSTEN Betreten die Szene
  (Cut_chg sperrt die Kamera-Automatik, der Spieler ist gehalten). Behoben in der Sonde selbst (enter_room: (9,71)=1 fuer ROOM10F0 NACH scd_vm_init, das die Flags nullt,
  VOR scd_room_reenter — 6 Zeilen mit Kommentar "Runde 35 Spur K"); danach 3/3 gruen. Kein Spielcode geaendert: die Sonde misst die Kamera-Selbstheilung, nicht die Szene.
- Suite: (siehe Abschluss unten)

## 6. OFFEN
- Keine Sprachaufnahmen: die 18 Zeilen laufen stumm mit Untertitel (99 Bilder Standzeit + Lesezeit), bis der Nutzer synchro/STAGE1/room10F0/main06..main23.wav liefert
  (dann haelt der Stimmen-Riegel die naechste Zeile bis zum Ende der Aufnahme).
- PSX-Ziel: die BGM-Weiche sitzt in platform/pc/src/audio_pc.c (audio_psx.c unveraendert); die Gestenblock-Leihe in platform/pc/main.c — fuer den PSX-Port waeren beide
  Haken in asset_psx.c/audio_psx.c nachzuziehen (nicht Teil dieser Runde, PSX-Build-Luecke laut Memory).
- Dialogkamera Cut 2 zeigt die drei Figuren klein (Abstand ~9500 Einheiten zur Monitorbank, Nutzer-Vorgabe "hinten rechts ... bei CUT2"); die Kopfgesten sind in dieser
  Entfernung nur wenige Bildpunkte gross — ausgefuehrt (NECK_LOG), aber auf dem Bild kaum sichtbar. Falls gewuenscht: Dialog naeher an der Kamera (z ~6000) = Positions-
  Konstanten in re15_cut10f0.h + szene_bauen.py.

## 7. Fuer den Nutzer
- Sprachdateien (Sprecher: Text), synchro/STAGE1/room10F0/:
  main06.wav Leon: Hey - how did you came in here?
  main07.wav Woman: Did you really think there was only one staff card for the Communication Room?
  main08.wav Woman: Anyway... the communication system is completely destroyed.
  main09.wav Woman: We won't reach anyone with it anymore...
  main10.wav Marvin: Leon! You already made it!
  main11.wav Leon: Hey Marvin, glad you made it!
  main12.wav Leon: Allow me to introduce you. This is...
  main13.wav Ada: ... Ada, Ada Wong
  main14.wav Leon: Ada Wong.
  main15.wav Marvin: Hello, glad to meet another Survivor! I'm Marvin.
  main16.wav Leon: Anyway... looks like we can't contact anyone with this thing anymore.
  main17.wav Marvin: Ohh... what do we do then?...
  main18.wav Leon: ...
  main19.wav Leon: I know! The patrol car! We can use it to get out of here!
  main20.wav Marvin: Yeah, you're right! That could be our way out!
  main21.wav Leon: Okay, Marvin, you go with Ada to the parking lot and wait there.
  main22.wav Leon: I'm going to get Chief Irons, and I'll be right behind you!
  main23.wav Marvin: Alright! Sounds like a plan. Take care Leon!
- Neue Assets fuer das Paket-/Android-Gate: KEINE Dateien unter shared_assets/ (Tonteil und Szene liegen eingebacken in engine/src/gen/cut10f0_*.inc;
  DOOR13.DO2 und ROOM11B0.RDT sind bereits im Gate).
- Bedienung: Szene startet beim ersten Betreten von ROOM10F0 (Leon) automatisch; die Karte danach schaltet nach ~4 s von B1 (Parkplatz) auf 3F (Irons' Buero) und schliesst
  nach weiteren ~4 s, START springt sofort weiter/schliesst. Beide Raeume blinken in der normalen Karte, bis man sie betreten hat. MAIN01 laeuft ab Szenenende in jedem
  Raum, bis man den Parkplatz ROOM11C0 betritt.
