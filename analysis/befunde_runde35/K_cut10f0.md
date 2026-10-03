# Runde 35 — Spur K "cut10f0": Neue Szene ROOM10F0 (Ada/Leon/Marvin), Karte 11C0+1150, MAIN01

Baum: `.claude/worktrees/r35_cut10f0`, Zweig `r35/cut10f0`, Basis master 154a73c1. Datum 2026-10-03.
**Lesehinweis:** §0-§7 = erster Durchgang (bis zum Sitzungslimit 12:00), §8 = Fortsetzung mit dem Gegenlesen gegen den
Wortlaut — drei Befunde, ihre Belege, Umsetzung und Messung nachher. Wo §8 etwas aendert (Programmgroesse, Gestentabelle
§3.1, Bildnummern §4, Tests §5), gilt §8; die betroffenen Stellen sind markiert.
**§9 = NACHBESSERUNG 1** nach der unabhaengigen Abnahme (K_abnahme_0.md). Wo §9 etwas aendert, gilt §9: MAIN01 beginnt
erst nach der 1150-Montage ((9,73)) statt am Ende der 10F0-Szene und endet an (4,64) statt am Besucht-Bit der Zone;
Skript-Befehle an den MAIN-Slot gelten im Fenster nicht; Leon dreht sich schon vor Zeile 6 zu Ada (Programm 1350 B /
288 Opcodes); Hinweiskette und Gestenblock-Leihe liegen nicht mehr in menu_common.c / main.c. Die OFFEN-Punkte und
"Fuer den Nutzer" stehen aktuell in §9.7/§9.8.
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
| 11 Hey Marvin, glad you made it! | Leon | **dreht zu Marvin (Plc_dest 9)**, 15 + 23 | wie 6; Drehung ROOM11C0 sub02 @0x0185E (§8.2) |
| 12 Allow me to introduce you. This is... | Leon | **dreht zu Ada (Plc_dest 9)**, 15 = Arm Richtung Ada, Kopf bei Marvin, + 23 | wie 6; Kopf = Plc_neck Modus 1 (§8.2) |
| 13 ... Ada, Ada Wong | Ada | 18 Hand zur Brust + 23 | ROOM11C0 @0x018F0 (Ada Clip 18) |
| 14 Ada Wong. | Leon | Plc_neck Modus 3 Nicken | ROOM11B0 @0x016B4 |
| 15 Hello, glad to meet another Survivor! I'm Marvin. | Marvin | **Kopf zu Ada (Plc_neck 1)**, 15, 18 Hand zur Brust, 23, Kopf zurueck zu Leon | ROOM11B0 @0x014F6; ROOM11C0 @0x018F0, @0x01886 |
| 16 Anyway... looks like we can't contact ... | Leon | **dreht zu Marvin (Plc_dest 9)**, Modus 2 + Modus 4 (Kopf gebeugt schuetteln) | ROOM10D0 sub21 @0x01BB4/@0x01BC2 (Leon) |
| 17 Ohh... what do we do then?... | Marvin | 20 Unterarm nach vorn + 23 | ROOM11B0 @0x015A0 (Marvin Clip 20) |
| 18 ... + Pause | Leon | keine (Sleep 70 + 60) | Nutzer: "etwas pause" |
| 19 I know! The patrol car! ... | Leon | 21 Hand hoch, 17 Arm-Schwung, 23 | ROOM1050 rec0 Clip 21 (1x gerufen), ROOM1170 sub02 @0x015F0 (17) |
| 20 Yeah, you're right! ... | Marvin | 15, **16**, 23 | Original-Paar ROOM11B0 sub06 @0x014F6/@0x014FE (§8.3) |
| 21 Okay, Marvin, you go with Ada ... | Leon | 15 (zu Marvin), **16**, 23 | wie 20 |
| 22 I'm going to get Chief Irons ... | Leon | 17 + 23 | ROOM1170 sub02 @0x015F0 |
| 23 Alright! Sounds like a plan. Take care Leon! | Marvin | 15, **16**, 23 | wie 20 |

(Stand nach der Fortsetzung §8: **fett** = geaendert. Takt JEDER Zeile jetzt Sleep 40 + 50 + 20 = 110 Bilder wie ROOM11B0
sub06 @0x014FA/@0x01502/@0x0150A; im ersten Durchgang hatten Ein-Gesten-Zeilen nur 50 + 20 = 70.)

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
- Suite: siehe §8.8 (Abschluss der Fortsetzung): `=== LOCAL-BUILD-OK (all) — Tests 486/486`.

## 6. OFFEN
- Keine Sprachaufnahmen: die 18 Zeilen laufen stumm mit Untertitel (99 Bilder Standzeit + Lesezeit), bis der Nutzer synchro/STAGE1/room10F0/main06..main23.wav liefert
  (dann haelt der Stimmen-Riegel die naechste Zeile bis zum Ende der Aufnahme).
- PSX-Ziel: die BGM-Weiche sitzt in platform/pc/src/audio_pc.c (audio_psx.c unveraendert); die Gestenblock-Leihe in platform/pc/main.c — fuer den PSX-Port waeren beide
  Haken in asset_psx.c/audio_psx.c nachzuziehen (nicht Teil dieser Runde, PSX-Build-Luecke laut Memory).
- Dialogkamera Cut 2 zeigt die drei Figuren klein (Abstand ~9500 Einheiten zur Monitorbank, Nutzer-Vorgabe "hinten rechts ... bei CUT2"); die Kopfgesten sind in dieser
  Entfernung nur wenige Bildpunkte gross — ausgefuehrt (NECK_LOG), aber auf dem Bild kaum sichtbar. Falls gewuenscht: Dialog naeher an der Kamera (z ~6000) = Positions-
  Konstanten in re15_cut10f0.h + szene_bauen.py.
- (Fortsetzung) MAIN01-BEGINN — Lesart, vom Nutzer zu bestaetigen: der Satz "Bis Leon dann den Parking Lot erreicht hat, soll durchweg
  MAIN01 ... gespielt werden" steht in AUFTRAG.md Z.92 HINTER der 1150-Montage (Spur L). Umgesetzt ist der Beginn am Ende der
  10F0-Szene ((9,71), Zuteilung VERTRAG §0 Spur K) — MAIN01 laeuft damit schon auf dem Weg zu Irons und waehrend dessen
  Todesszene, sofern Spur L dort nichts anderes schaltet. Soll es erst mit der Montage beginnen: EINE Stelle,
  `re15_cut10f0_bgm_eintrag` (cut_10f0.c) zusaetzlich auf (9,73) "Irons-Todesszene gesehen" (VERTRAG §1.1 Spur L) gaten und L
  die Raummusik am Montage-Ende einmal anstossen lassen (wie re15_cut10f0_tick). Naechster Messweg: Lauf D mit
  RE15_SET_FLAG=9:73 nach dem Zusammenfuehren mit L.
- (Fortsetzung) War Leon VOR der Szene schon im Parkplatz (nur mit Strom (4,243) moeglich, ROOM11B0 sub01 @0x011F6/@0x01216), steht
  das Besucht-Bit der Zone ROOM11C0 bereits: die Parkplatz-Kachel blinkt dann nicht und MAIN01 beginnt nicht (Wortlaut "nicht
  besucht" / "erreicht hat" woertlich genommen). Ein eigener Latch wie (9,72) fuer ROOM1150 braeuchte ein weiteres Bank-9-Bit —
  Spur K hat keines mehr frei (71, 72 belegt); frei laut VERTRAG §1.1: 84, 64, 66-70. Messweg: unit_r35_cut10f0_bgm /
  _karte mit `re15_map_zone_update(0x11C0, ...)` VOR dem Setzen von (9,71).
- (Fortsetzung) Android/PSX: der CONTINUE-Haken (§8.5) sitzt in platform/pc/main.c; ob platform/android denselben Lade-Weg nimmt
  (TABU fuer diese Spur), ist nicht gemessen — Messweg: Lauf D auf dem Geraet, Zeile `[bgm] ... entry=FF01` nach dem Laden.

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
- (Fortsetzung) ROOM1150 blinkt auf Blatt 3F, obwohl man vorher schon dort war — bis Leon den Raum NACH der Szene wieder betritt
  (Bild K_belege/nachher2_karte_3F_room1150_blinkt_trotz_besuch.png). MAIN01 laeuft auch nach dem Laden eines Spielstands weiter.
  Die Szene dauert jetzt ~91 s (2720 Bilder; jede Zeile steht mindestens 3 s wie in den Original-Dialogen); mit Sprachdateien
  richtet sich jede Zeile wie bisher nach der Laenge der Aufnahme.
- (Fortsetzung) Fuer die Zusammenfuehrung: Spur K belegt jetzt BEIDE zugeteilten Bank-9-Bits — 71 "Szene gesehen", 72 "ROOM1150 nach
  der Szene betreten" (vorher Reserve). Spur L: (9,71) wie vereinbart; die MAIN01-Weiche gilt auch in ROOM1150 (Raum-Byte 0x15)
  und waehrend der Montage (siehe OFFEN, MAIN01-Beginn). Gemeinsame Dateien mit Haken dieser Spur: scd_room_setup.c (1 Zeile),
  scd_vm.c (1), game_step_common.c (1), enemy_common.c (1), menu_common.c (Hinweiskette), re15_inv_screen.c/.h (zweites Ziel),
  map_hint_common.c/.h (Eintraege K1/K2, Latch-Feld), platform/pc/main.c (Leihe 2x, Installer am Boot-Weg, CONTINUE-BGM),
  platform/pc/src/audio_pc.c (Weiche, Port-Bank 0x0E), tests/test_support.c (Spion), tests/unit/test_cam_selfheal.c (Flag).

## 8. FORTSETZUNG (2026-10-03 nachmittags, nach dem Sitzungslimit 12:00) — Gegenlesen gegen den Wortlaut
Stand beim Einstieg: Baum sauber, HEAD 9cc0d253 (3 wip-Commits). Gelesen: Dossier §0-§7, `git log --stat master..HEAD`,
Diff gegen master, AUFTRAG.md, VERTRAG.md. Keine vorliegende Messung wiederholt. Gegengelesen wurde JEDE Zeile des
Nutzers (AUFTRAG.md Z.41-67, Z.92) gegen Programm (tools/r35_k/szene_bauen.py), Texte (texte_bauen.py), Karte
(map_hint_common.c) und BGM (cut_10f0.c). Ergebnis: Texte/Sprecher/Reihenfolge/Kamera/Tuerknall stimmen woertlich
(18 Zeilen, "Woman:" bei 7/8/9, "Ada:" ab 13) — DREI Abweichungen vom Wortlaut bzw. von "vergleichbaren Dialogen":

### 8.1 BEFUND 1 — ROOM1150 blinkt im echten Spiel NIE ("So lange der Raum nicht besucht ist, blinken beide weiter")
- Mechanik bisher: `re15_map_ziel_aktiv_n` (map_hint_common.c) blendet ein Ziel aus, sobald `re15_map_zone_visited(zone)`
  steht. Fuer K2 (ROOM1150) steht dieses Bit im echten Spiel LAENGST: der Weg nach ROOM10F0 fuehrt ueber die erste
  Irons-Szene in ROOM1150 (Flag (3,94), ROOM1150 sub08 @0x01110; erst deren Hinweis — Tabelleneintrag 0 `{0x1150, ...,
  0x10F0, 0, 3, 94}` — schickt nach ROOM10F0). Der vorhandene Riegel unit_r35_cut10f0_karte pruefte nur den frischen
  Zustand (1150 nie besucht) und war deshalb gruen.
- MESSUNG VORHER (gebauter Stand 9cc0d253 + nur neue Pruefzeilen, `test_r35_cut10f0.exe karte`): Zone 1150 besucht,
  (3,94)=1, Zone 10F0 besucht, dann (9,71)=1 -> ROOM1150 nicht aktiv ("aktiv 0"):
  `FEHLER: echter Weg: nach der Szene blinken ROOM11C0 (0/4) UND das schon frueher besuchte ROOM1150 (0/4, aktiv 0)`.
  ⛔ Berichtigung zu genau dieser Zeile: in diesem ersten Lauf war auch ROOM11C0 nicht aktiv (die gedruckten "0/4" sind
  stehengebliebene Werte) — ein TESTARTEFAKT: die Besucht-Bits liegen nicht in g_game, `re15_game_state_init` loescht
  sie nicht, der erste Testteil hatte 11C0 schon "besucht". Mit `re15_map_visited_reset()` davor bleibt genau der eine
  Befund. Der saubere Vorher-Beleg ist der ALTE Riegel selbst, im selben Lauf gruen:
  `ok: ROOM1150 besucht -> nur noch ROOM11C0 blinkt (Blatt 0)` — Besucht-Bit der Zone => Ziel aus; und im echten Spiel
  steht dieses Bit vor der Szene.
  Der Hinweis-SCHIRM am Szenenende zeigt 1150 trotzdem blinkend (re15_map_hint_ziel fragt das Besucht-Bit nicht,
  RE2 @0x8006F4E8-0x8006F608 — gemessen §4), nur die normale Karte danach nicht.
- Deutung des Nutzer-Satzes: "besucht" = NACH der Szene betreten (Leon soll Irons holen) -> eigener Latch.
  VERTRAG §1.1: Bit (9,72) = Reserve der Spur K. Zensus (alle 240 RDTs, Rohbytes `21 09 48` / `22 09 48`):
  0 Treffer; Port-Code (engine/src, include, platform/pc/main.c): kein Nutzer. Gesetzt beim Raumaufbau von
  ROOM1150 mit (9,71)=1 (re15_cut10f0_install laeuft in scd_room_reenter UND am Boot-Weg fuer JEDEN Raum).
- ROOM11C0 behaelt das Besucht-Bit der Zone: vor der Szene ist der Parkplatz nur mit Strom erreichbar (ROOM11B0 sub01
  @0x011F2 `06 00 40 00` / @0x011F6 Ck(4,243)==0 -> @0x01216 `46 01 01 31 0f 00 ff ff 00 00` Aot_reset Slot 1 = die
  Tuer nach 11C0 wird zum Text-Platz msg 15 "It's too dark to see anything..."); war Leon trotzdem schon dort, gilt
  der Wortlaut woertlich ("nicht besucht" ist dann falsch -> kein Blinken, MAIN01 endet sofort). Kein zweiter Latch
  (Spur K hat genau ein Reserve-Bit).

### 8.2 BEFUND 2 — Leon begruesst Marvin mit dem Ruecken zu ihm ("Hey Marvin, glad you made it! wieder arm strecken")
- MESSUNG VORHER (integration Lauf A, debug.log lauf_8c69c670_a, RE15_CAM_TRACE): von F360 bis F2040 unveraendert
  `pl=(4792,11359) rot=4019` — Leon steht die GANZE Szene nach +X zu Ada gewandt. Marvin steht bei (3814,9831):
  Richtung von Leon = (-978,-1528) = Gierung ~1390 (0 = +X, 1024 = -Z, 2048 = -X). Differenz zu 4019: ~134 Grad —
  Marvin steht schraeg HINTER Leon; Clip 15 ("Arm strecken") zeigt bei Zeile 11 auf Ada, nicht auf Marvin. Damit ist
  auch "This is... -> arm strecken Richtung Ada" (Zeile 12) von Zeile 11 nicht zu unterscheiden, und Leon redet
  bei 16/19/21/22 ("Okay, Marvin, you go with Ada ...") an Marvin vorbei.
- Form der Abhilfe (belegt, schon im Programm benutzt): Plc_dest Modus 9 (drehen) Spieler ROOM11C0 sub02 @0x0185E +
  Warteschleife Ck(5,0) ROOM1050 sub03 @0x00DCA. Choreografie (PORT-WAHL nach Wortlaut): vor Zeile 11 dreht Leon zu
  Marvin, vor Zeile 12 zurueck zu Ada (Arm Richtung Ada, Kopf bleibt per Plc_neck bei Marvin), nach Marvins
  Vorstellung (15) wieder zu Marvin — den Rest des Gespraechs (16..22) fuehrt er mit Marvin. Marvin sieht bei
  "glad to meet another Survivor" Ada an (Plc_neck Modus 1, Form ROOM11C0 sub02 @0x01886), danach wieder Leon.

### 8.3 BEFUND 3 — Ein-Gesten-Zeilen sind kuerzer als JEDE Dialogzeile des Originals
- MESSUNG VORHER (Listing szene_bauen.py; debug.log Lauf A): Zeilen mit einer Geste bekamen Message_on, Geste,
  Sleep 50, Clip 23, Sleep 20 = 70 Bilder bis zur naechsten Zeile (11 -> 12, 12 -> 13, 13 -> 14, 17 -> 18, 20 -> 21,
  21 -> 22, 22 -> 23). Ohne Sprachdatei ersetzt die naechste Message_on die laufende (Standzeit sonst 99 Bilder,
  `04 01 01 63`; msg_common.c re15_msg_compute_duration zaehlt Glyphen nicht) -> z.B. die zweizeilige Zeile 21
  (65 Zeichen) stand nur 70 Bilder = 2,3 s.
- ORIGINAL (ROOM11B0 sub06, Datei-Offsets): JEDE Zeile = Message_on, Geste A, `09 0a 28 00` Sleep 40, Geste B,
  `09 0a 32 00` Sleep 50, Clip 23, `09 0a 14 00` Sleep 20 = 110 Bilder: msg 1 @0x014EE..@0x0150A (Marvin 15/16/23,
  58 Zeichen), msg 2 @0x0150E..@0x0152A (Leon 19/17/23, 70 Zeichen), msg 8 @0x0166E..@0x01686 (95 Zeichen, 3 Reihen).
  Kuerzeste Original-Zeile: 90 Bilder (msg 4 @0x01574: Sleep 40 @0x01580 + Sleep 50 @0x0158C), msg 10/11/12 je 100
  (40+40+20 @0x0177A/@0x01782/@0x0178A). Keine Zeile unter 90.
- Abhilfe: jede Zeile im vollen Original-Takt 40 + 50 + 20 = 110. Zeilen mit Nutzer-Geste halten die Geste
  (Sleep 40 + Sleep 50 ohne Geste B); die langen zweizeiligen Zeilen 20/21/23 bekommen das Original-Paar
  15 -> 16 -> 23 (Clip 16 = "kleine Handflaechen-Geste", ROOM11B0 sub06 @0x014FE `3f 00 10 00`; Clip-Inhalt im
  geliehenen Block: rec1 Clip 15..23 hash-gleich mit rec0 und ROOM11C0 rec0 — rbj_zensus.py suche, z.B. Clip 16
  fec1ec970163, Clip 19 fde929aa6e6b). Mit Sprachdatei haelt weiterhin der Stimmen-Riegel die naechste Zeile.

### 8.4 Gelesen und in Ordnung befunden (keine Aenderung)
- "hinten rechts an den Monitoren bei CUT2": Ada (6000,11500) = die NO-Tasche zwischen Wandvorsprung (x 4300..5700,
  z >= 11800) und Konsole (x >= 6900) — Bild nachher_F1100: sie ist die rechte der drei Figuren vor der Monitorwand.
- "Kamera muss immer wechseln ..., solange Leon noch nicht bei ihr steht": Cut 2 (F6) -> Cut 0 (F56) -> RVD-Kette
  Cut 1 (~F260) -> Cut 2 (~F300) waehrend Leon laeuft (Cut_auto 1), erst bei Ankunft Cut_chg 2 fest.
- Reihenfolge Tuerknall (F704) -> Marvin laden (Pos_set F716) -> Cut 0 (F732); Abgang Modus 5 (rennen), Balken +45.
- MAIN01-START: der Nutzer-Satz steht in AUFTRAG.md HINTER der 1150-Montage ("Bis Leon DANN den Parking Lot erreicht
  hat"). VERTRAG §0 teilt ihn Spur K zu ("danach Karte ...; MAIN01 ... bis zum Parkplatz"). ENTSCHEIDUNG: Beginn =
  Ende der 10F0-Szene ((9,71)), Ende = Parkplatz. Das erfuellt den Satz in jeder Lesart fuer die Strecke nach der
  1150-Montage ("durchweg") und haengt nicht an Spur L. Folge fuer die Integration: auf dem Weg 10F0 -> 1150 und
  WAEHREND der Irons-Todesszene (Spur L) laeuft bereits MAIN01, sofern L dort nichts anderes schaltet; soll MAIN01
  erst mit der Montage beginnen, ist die eine Stelle `re15_cut10f0_bgm_eintrag` (zusaetzlich (9,73) lesen,
  VERTRAG §1.1 Spur L) — siehe OFFEN.
- Skript-BGM auf dem Weg (Zensus Sce_bgm_control 0x54 / Sce_bgmtbl_set 0x57, alle ROOM1xx0): ROOM1030 main00
  @0x01C5E/@0x01C64 (Start MAIN/SUB), ROOM1090 sub00/02/03, ROOM1170, ROOM11C0 sub00/02/03, ROOM11D0, ROOM11F0,
  ROOM1200 — kein Raum auf dem Weg STOPPT den MAIN-Kanal ausserhalb bereits gelaufener Szenen.

### 8.5 BEFUND 4 (beim Messen gefunden) — nach dem LADEN eines Spielstands im MAIN01-Fenster spielt die Tabellenmusik
- MESSUNG VORHER (echte exe, Lauf D: Spielstand IN ROOM10F0 mit (9,71)=1, CONTINUE): debug.log
  `[bgm] stage=0 room=0F entry=FF20 -> MAIN20(flag 0) SUB--` und erst DANACH `[save] CONTINUE: resumed in room 10f0`;
  ebenso Lauf F in ROOM1150: `room=15 entry=FF1E -> MAIN1E`. Der Boot-BGM-Aufruf (platform/pc/main.c:4631,
  `re15_audio_start_room_bgm(boot_room)`) laeuft VOR dem Restore der Flags — die Weiche sieht (9,71)=0. Erst die
  naechste Tuer brachte MAIN01 (`room=0D entry=FF01`). "durchweg MAIN01" war nach jedem Laden bis zum ersten
  Raumwechsel verletzt.
- ORIGINAL (selbst disassembliert, re15_disasm.py auf info/Re1.5/PSX.EXE): der LOAD ist ein Block-memcpy
  `80026290 lui a0,0x800b / 80026294 addiu a0,a0,3516 (=0x800b0dbc) / 80026298 lw a1,504(sp) / 8002629c jal 0x8004ee38 /
  800262a0 ori a2,zero,0x1430`; DANACH laeuft der Raumlader FUN_800396fc mit der Musikwahl FUN_800443ec/FUN_80044210,
  deren Cache-Vergleich `80044278 andi a0,s0,0x3f / 8004427c andi v1,v1,0x3f / 80044280 beq a0,v1,0x800442f4` nur die
  Tabelle UNK_80074828 liest — die Wahl haengt dort nie an Flags, die Reihenfolge Boot-BGM/Restore war im Port deshalb
  bisher gleichgueltig. Die MAIN01-Weiche ist die erste flag-abhaengige Raummusik.
- UMSETZUNG: Haken in platform/pc/main.c direkt hinter `[save] CONTINUE: resumed ...` (6 Zeilen, "Runde 35 Spur K"):
  gilt die Weiche (`re15_cut10f0_bgm_eintrag(stage, room) >= 0`), wird `re15_audio_start_room_bgm` noch einmal gerufen;
  der Cache-Vergleich (FUN_80044210 @0x80044278-@0x80044280) ersetzt das noch nicht geladene MAIN20 durch MAIN01.
- MESSUNG NACHHER (Lauf D): `[bgm] ... room=0F entry=FF20`, `[save] CONTINUE: resumed in room 10f0`,
  `[bgm] stage=0 room=0F entry=FF01 -> MAIN01(flag 0) SUB--`, Tuer (DOOR13), dann
  `[bgm] stage=0 room=0D entry=FF01 -> MAIN01(flag 0) SUB--(flags 1/1)  [unveraendert, laeuft durch]`.

### 8.6 Umsetzung der Fortsetzung (Dateien, Konstanten)
- `include/re15_cut10f0.h`: RE15_CUT10F0_ZIEL2_BESUCHT_BANK/_BIT = (9,72) (VERTRAG §1.1 Reserve Spur K; Zensus §8.1),
  Ablauf-/Choreografie-Kommentar, Haken-Beschreibung.
- `engine/src/map_hint_common.c`: Tabellenfelder `erreicht_bank/erreicht_bit` (0 = Besucht-Bit der Zone, Runde-33-Regel
  unveraendert); Eintrag K2 `{0x10F0, {0}, 0, 0x1150, 0, 9, 71, -1, 1, 9, 72}`; `re15_map_ziel_aktiv_n` fragt den Latch.
- `engine/src/cut_10f0.c` `re15_cut10f0_install`: Raumaufbau ROOM1150 (nur Leons Variante 0x1150) mit (9,71)=1 ->
  Set (9,72)=1 + Logzeile `[cut10f0] ROOM1150 nach der Szene betreten: (9,72)=1, Kartenziel ROOM1150 erreicht`.
- `tools/r35_k/szene_bauen.py` -> `engine/src/gen/cut10f0_szene.inc` (jetzt 1326 Bytes, 282 Opcodes, 18 Message_on;
  vorher 1162/245): zeile() im Takt Sleep 40 `09 0a 28 00` + Sleep 50 `09 0a 32 00` + Clip 23 + Sleep 20 `09 0a 14 00`
  (ROOM11B0 sub06 @0x014FA/@0x01502/@0x01506/@0x0150A); drei Leon-Drehungen `40 00 09 00 <x> <z>` + Warteschleife
  (ROOM11C0 sub02 @0x0185E, ROOM1050 sub03 @0x00DCA) zu Marvin (3800,9900) / Ada (6000,11500) / Marvin; Marvins Blick
  `41 01 <x> 00 00 <z> 64 00` zu Ada und zurueck (ROOM11C0 sub02 @0x01886); Clip 16 `3f 00 10 00` (ROOM11B0 @0x014FE)
  als Geste B der Zeilen 20/21/23. Positionen/Texte unveraendert (NUTZER-VORGABE / PORT-WAHL, re15_cut10f0.h).
- `platform/pc/main.c`: CONTINUE-Haken §8.5.
- Tests: `tests/unit/test_r35_cut10f0.c` (karte: echter Weg; szene: Leons Gierung je Zeile, Zeilenabstand),
  `tests/unit/probe_r35_cut10f0_karte.c` (Staende raus / vor11c0 / in1150), `tests/integration/test_r35_cut10f0.cmake`
  (Laeufe D/E/F, Endbild Lauf A 3200), `tests/unit/probes/r35_cut10f0.cmake` (Beschreibung).

### 8.7 Messung nachher (gebauter Stand der Fortsetzung)
Unit, echte VM + Spielschritt (`test_r35_cut10f0.exe szene`):
- `Faden 1 (max 1) | msg 6..23 ab B71..B2455 | Ada (6000,11500) dir 3072 | Marvin geparkt 1, an der Tuer B756
  (8400,-350), bei Leon B1083 (3814,9831) | Leon los B204 an B414 (4792,11359) rot 1413 | Knall B744/B2665 |
  Cuts: 2 0 1 2 0 2 0 2 | Ende B2720 Balken voll B15 weg B2734`
- Leons Gierung beim Aufgehen der Zeile (Soll = atan2 zum Ziel, 4096 = 360 Grad): Zeile 7 -> Ada 4019 (Soll 4020);
  Zeile 11 "Hey Marvin" -> Marvin 1413 (Soll 1413); Zeile 12 "This is..." -> Ada 4019 (Soll 4020); Zeilen 16, 19, 21, 22
  -> Marvin 1413; am Ende 1413. Marvin steht bei Zeile 11 auf (3814,9831).
- Zeilenabstaende (Bilder bis zur naechsten Zeile): 6:343 7:110 8:110 9:151 10:298 11:126 12:110 13:110 14:90 15:126
  16:130 17:110 18:130 19:110 20:110 21:110 22:110 — Minimum 90 (Zeile 14 "Ada Wong." mit Nicken; = kuerzeste
  Original-Zeile), vorher 70. 126 = 110 + 16 Bilder Drehung (Modus 9).
Echte exe, echter Weg (Spielstand ROOM10D0 + CONTINUE + Aktionstaste, beschleunigter Renderer, RE15_FRAMEDUMP
1040-3180/20, Scratch r35k/mess1; gdigrab unbrauchbar wie in §1.2):
- cam-trace: F1080 `rot=1075` (Leon dreht), F1200 `rot=1413` (zu Marvin, Zeile 11), F1320 `rot=4019` (zu Ada, Zeile 12),
  F1740..F2460 `rot=1413`; Position durchgehend (4792,11359).
- Bild K_belege/nachher2_gesten_zeilen11_bis_22.png (angesehen): F1120/F1180 Leon Marvin zugewandt, Arm zu Marvin;
  F1260 Leon zu Ada gedreht, Arm waagerecht auf Ada; F1340 Ada Hand zur Brust; F1600 Marvin gestikuliert, Leon bei Ada;
  F1700 Leon zu Marvin gedreht, Kopf gesenkt; F2040 "I know!" Hand hoch; F2260 Arm zu Marvin. Alle drei im Bild.
- Ende: `[cut10f0] Szene zu Ende ...` , `[bgm] stage=0 room=0F entry=FF01 -> MAIN01`, `[hint] F2741 begin`,
  `[hint] F2859 Folge-Hinweis 1 -> 2 (Zeit)`, `[hint] F2977 schliessen (Abbruch/Zeit)` (3,96 s je Ziel, Wanduhr).
  Bild K_belege/nachher2_abgang_balken_karte.png (angesehen): F2600 Marvin/Ada laufen los, F2660 Cut 0 Ada vor der Tuer,
  F2700/F2720 Leon allein mit Balken, F2780 Blatt B1 (Parkplatz-Kachel), F2900 Blatt 3F (ROOM1150 rot), F3000 frei.
Karte danach, normale Karte der echten exe (Stand in ROOM10F0 mit (9,71)=1; RE15_INV_OPEN_AT=30#10f0,
RE15_MAP_SHOT_PAGE=4 deckt Blatt 3F auf = ALLE Zonen des Blatts besucht, auch ROOM1150; FRAMEDUMP 90-230/10):
- Bild K_belege/nachher2_karte_3F_room1150_blinkt_trotz_besuch.png (angesehen): alle Raeume gruen (besucht), die
  Kachel ROOM1150 wechselt F120 rot -> F140 Umriss -> F160 rot -> F180 Umriss. Vorher (Besucht-Bit) waere sie gruen.
- Unit `karte`: `ok: echter Weg: nach der Szene blinken ROOM11C0 (0/4) UND das schon frueher besuchte ROOM1150 (4/2,
  aktiv 1)`, `ok: ... ROOM1150 blinkt in anderen Raeumen weiter`, `ok: ... Betreten von ROOM1150 nach der Szene setzt
  (9,72) -> nur noch ROOM11C0 blinkt`, `ok: ... ROOM11C0 besucht -> kein Ziel mehr`, Elzas ROOM1151: kein Latch.
- Lauf F (Stand in ROOM1150, (9,71)=1): `[cut10f0] ROOM1150 nach der Szene betreten: (9,72)=1, Kartenziel ROOM1150
  erreicht`, `[bgm] stage=0 room=15 entry=FF01 -> MAIN01`.
MAIN01 von Raum zu Raum (echte Tueren, SDL_AUDIODRIVER=dummy weil der Rechner keinen Audio-Endpunkt hat):
- Lauf D (10F0 -> 10D0): siehe §8.5 — `room=0D entry=FF01 ... [unveraendert, laeuft durch]` (kein Neustart des Stuecks).
- Lauf E (Stand in ROOM11B0 vor der Tuer zum Parkplatz, (4,243)=1 Strom, (3,130)=1): `[save] CONTINUE: resumed in room
  11b0`, `[bgm] stage=0 room=1B entry=FF01 -> MAIN01(flag 0) SUB--`, `[tuer] Sequenz Archiv 2 DOOR1A ...`,
  `[bgm] stage=0 room=1C entry=FF56 -> MAIN16(flag 1) SUB--` = die eigene Musik des Parkplatzes (UNK_80074828[0x1C]);
  danach ist die Zone ROOM11C0 besucht -> Weiche -1 fuer jeden Raum (unit_r35_cut10f0_bgm "Parkplatz besucht -> Tabelle
  wieder normal").
- Tabellenweiche fuer die uebrigen Raeume des Wegs (unit `bgm`): 0xFF01 fuer Raum-Bytes 0x0F 0x0D 0x10 0x11 0x12 0x13
  0x15 0x04 0x06 0x03 0x1B 0x1F; -1 fuer 0x1C, andere Stages und nach dem Parkplatz.

### 8.8 Tests der Fortsetzung
- `ctest -R r35_cut10f0`: 8/8 gruen — unit_r35_cut10f0_programm/_texte/_tuerton/_szene/_einmal/_karte/_bgm und
  integration_r35_cut10f0 (208 s; Laeufe A Szene am echten Weg, B genau einmal, C Boot-Weg, D MAIN01 nach dem Laden und
  durch die Tuer, E Ende am Parkplatz, F Latch (9,72)).
- Je Nutzer-Punkt: (1) Szene: _programm, _texte, _szene, _einmal, integration A/B/C; (2) Karte: _karte (Kette, beide
  Ziele, echter Weg), integration A (Hinweiskette am Schirm) + F; (3) MAIN01: _bgm, integration A/D/E/F;
  (4) Animationen: _szene (Gierung je Zeile, Zeilentakt) + Bildbelege.
- GANZE SUITE (`bash re15_port/tools/local_build.sh all`, Stand f2ac79e0 = Code dieses Abschlusses, 2026-10-03 16:01-16:25,
  parallel bauende Nachbarbaeume): `100% tests passed, 0 tests failed out of 486`, `Total Test time (real) = 1427.08 sec`,
  woertliche Schlusszeile:
  `=== LOCAL-BUILD-OK (all) — Tests 486/486`
  (Schranke RE15_MIN_TESTS=478; 478 Bestand + 8 dieser Spur). Kein Fenster-Haken geflattert, nichts nachgefahren.
  unit_cam_selfheal (im ersten Durchgang angepasst, §5) gruen.

### 8.9 Abschluss — Stand je Nutzer-Punkt
| Punkt (AUFTRAG.md) | Stand | Beleg |
|---|---|---|
| 1 Szene beim ERSTEN Betreten von ROOM10F0, Reihenfolge/Sprecher/Kamera/Tuerknall/Abgang/Balken | erfuellt | §4, §8.7; unit _programm/_texte/_szene/_einmal, integration A/B/C; Bilder nachher_*, nachher2_* |
| 2 Karte: erst ROOM11C0 markiert, dann ROOM1150 blinkend, beide bis besucht | erfuellt (ROOM1150: Besuch NACH der Szene, Latch (9,72)) | §8.1, §8.7; unit _karte, integration A/F; Bild nachher2_karte_3F_* |
| 3 MAIN01 durchweg bis zum Parkplatz | erfuellt ab Szenenende, auch nach dem Laden; Beginn-Lesart siehe §6 OFFEN | §8.5, §8.7; unit _bgm, integration A/D/E/F |
| 4 Animationen passend wie bei vergleichbaren Dialogen | erfuellt (Original-Zeilentakt 110, Leon dem Angesprochenen zugewandt) | §3.1, §8.2, §8.3, §8.7; unit _szene; Bild nachher2_gesten_* |

Commits des Zweigs r35/cut10f0 ueber master 154a73c1: 0908b3df, d017beaa, 9cc0d253 (erster Durchgang), 3d366e0e, 779c45bc,
4ea3c410, f2ac79e0 (Fortsetzung, wip) und der Abschluss-Commit `feat(r35-cut10f0): ...`.

## 9. NACHBESSERUNG 1 (2026-10-03 abends, nach der unabhaengigen Abnahme 0 = K_abnahme_0.md)
Stand beim Einstieg: Baum sauber, HEAD 702bca9e (`doc(r35-cut10f0): Abnahme 0`). Gelesen: K_abnahme_0.md ganz,
Dossier §0-§8, VERTRAG.md, AUFTRAG.md Z.36-110, `git diff master..HEAD` der gemeinsamen Dateien. Vier Maengel,
jeder mit Ursache / Messung vorher / Beleg / Aenderung / Messung nachher. "Messung vorher" = die Messlaeufe der
Abnahme am selben Code (HEAD 7b20a561; K_abnahme_0.md §2/§8) — nicht wiederholt.

### 9.0 RE-Belege, die fuer die Maengel 1 und 2 neu gezogen wurden (selbst disassembliert, `re15_disasm.py`, info/Re1.5/PSX.EXE)
- **Sce_bgm_control (0x54) -> FUN_80044da4**, Sprungtabelle @0x80010e58 = `80044f28 80044e00 80044e50 80044e88 80044ee8
  80044f20` (op 0 = nur Nutzlast, 1 = SetVol+Play, 2 = Stop, 3 = SetVol+Replay, 4 = Pause, 5 = Ausblendung):
  op 2 @0x80044e50: `lb a0,0x800b52ae[slot*8]` @0x80044e60, `jal 0x800603dc` (SsSeqStop) @0x80044e64, Status
  `sb 2,0x800b52ac[slot*8]` @0x80044e7c.
  Nutzlast @0x80044f2c ff.: `beq s1,zero` @0x80044f34 -> Slot 0 nimmt `lw v1,0x800b3f88` @0x80044f50 (Kopf der
  GELADENEN MAIN-Bank), sonst `0x800b3f8c` (SUB); `sb v1,-15(v0)` @0x80044f6c (ProgAtr[part-1].mvol) bzw.
  `sb v0,24(v1)` @0x80044f74 (Master). Die Schreiber treffen also immer die Bank, die gerade geladen ist.
- **SsSeqStop** 0x800603dc -> _SsSndStop @0x80060270: loescht die Bits 1/2/8 (`and ... -2/-3/-9` @0x800602c8/e4/300)
  und SETZT Bit 4 `ori v0,v0,0x4` @0x8006031c im Zustandswort +0x90 (144) der Sequenz.
- **SsSeqReplay** 0x8005acec -> _SsSndReplay @0x8005ac48: `lw v1,144(a2)` @0x8005ac88, `andi v0,v1,0x204` @0x8005ac90,
  `bne v0,zero,0x8005ace4` @0x8005ac94 -> eine GESTOPPTE Sequenz (Bit 4) wird von Replay NICHT wieder gestartet.
- **Auto-Replay beim Raumstart FUN_800444b0** (Aufrufer nur @0x8001d5d4 und @0x8001daf0, je NACH dem Raumlader
  FUN_800396fc @0x8001d5ac/@0x8001d988): `lbu v1,0x800b52ad` @0x800444b8, `bne v1,zero,0x800444ec` @0x800444c8,
  `jal 0x8005acec` (SsSeqReplay) @0x800444d8, Status = 1 @0x800444e8. Wegen @0x8005ac94 holt das einen vom Skript
  gestoppten MAIN NICHT zurueck.
- **FUN_80044210** (einziger Aufrufer @0x800399b0): gleicher MAIN (`beq a0,v1,0x800442f4` @0x80044280) -> kein Laden,
  kein Play; das Flag 0x800b52ad wird NUR im MAIN-Wechsel-Zweig geschrieben (@0x800442D0).
  => Im Original bleibt ein per op 2 gestoppter MAIN stumm, bis ein Skript op 1 schickt oder der MAIN wechselt.
  Der Port bildet genau das ab ("[unveraendert, laeuft durch]"); der Engine-Pfad ist byte-true, kein Engine-Fehler.
- **Tabelle UNK_80074828** (STAGE1, Index = Raum-Byte): [0x1D] ROOM11D0 = 0xFF7B (MAIN3B, Flag 1 = Handstart),
  [0x18] ROOM1180 = 0xFF1D, [0x16] 0xFF1D, [0x1B] 0xFF1D, [0x09] ROOM1090 = 0x0355 (MAIN15 Flag 1 + SUB03),
  [0x15] ROOM1150 = 0xFF1E, [0x0F] 0xFF20, [0x0D] 0xFF1F, [0x03] 0x4041, [0x1C] 0xFF56.
  Im Original haben ROOM11D0 und ROOM1180 VERSCHIEDENE MAINs — der Stop des Zwingers (@0x01710, gilt MAIN3B) endet
  dort mit dem Raumwechsel (MAIN1D wird neu geladen). Erst die Weiche der Spur K (ueberall MAIN01) macht aus dem
  Raum-Stop ein dauerhaftes Verstummen.
- RDT-Bytes: ROOM11D0 sub01 @0x016E4 `21 07 d1 01 06 00 38 00 / 21 07 d2 01 .. / 21 07 dd 01 .. / 21 07 2c 01 .. /
  21 07 2d 01 .. / 21 05 00 00`, @0x01710 `54 00 02 00 00 00`; ROOM1090 sub00 @0x022EE `54 00 00 01 78 33`
  (Programm 0 der MAIN-Bank: Lautstaerke 0x77, Pan 0x32), sub03 @0x024DA `54 00 00 01 01 41` (Programm 0 stumm),
  @0x024E0 `54 00 02 00 00 00`.
- Vergleichsdialog fuer Mangel 3: ROOM11C0 sub02 @0x01886 `41 01 fb dc 00 00 f5 c7 64 00` (Plc_neck Modus 1),
  @0x01890 `40 00 09 00 fb dc f5 c7` (Plc_dest Modus 9) + `18 05`, `29 0d` (Cut_chg), `09 0a 14 00` (Sleep 20),
  @0x018A0 `2b 00 00 00` + `3f 00 0f 00` (Zeile mit Clip 15): Blick, Drehung, Schnitt, 20 Bilder, DANN die Zeile.
- "Szene laeuft" im Port = `re15_cine_active()` (game_state.c: flag(1,27) || flag(2,7)) — die beiden Rahmen-Flags
  jeder Original-Szene (ROOM1090 sub02 @0x02414/@0x02418, Ende @0x024BE/@0x024C2; Balken FUN_80021a0c @0x80021a24).

### 9.1 Plan je Mangel (Umsetzung und Messung nachher folgen unten)
1. MAIN verstummt: Skript-Befehle an Slot 0 (MAIN) gelten der Tabellen-Musik des Raums (ROOM11D0: MAIN3B), die im
   Fenster gar nicht geladen ist. Im Fenster werden sie nicht auf MAIN01 angewandt (Haken audio_pc.c, SEQ_CTL-Zweig).
2. MAIN01 zu frueh: Weiche zusaetzlich an (9,73) (VERTRAG §1.1 Spur L "Irons-Todesszene gesehen", nur gelesen) und an
   "keine Szene laeuft" beim OEFFNEN; den Anstoss macht re15_cut10f0_tick selbst (kein Haken bei Spur L noetig),
   ebenso nach dem Laden (der CONTINUE-Haken in main.c entfaellt).
3. Zeile 6: Blick + Drehung zu Ada VOR der Zeile in der Form von ROOM11C0 sub02.
4. Haken: Hinweiskette/zweites Ziel aus menu_common.c nach cut_10f0.c, pc_rbj_leihen aus main.c nach
   platform/pc/src/cut10f0_pc.c; in beiden Dateien bleiben Haken von 1-5 Zeilen.

### 9.2 Mangel 3 — Zeile 6: Leon streckt den Arm an Ada vorbei
- **Ursache:** das Programm liess Leon nach dem Tuer-Eintritt mit der Eintritts-Gierung 2048 (Blick -X, Westwand) stehen
  und spielte Clip 15 sofort; Blick (Plc_neck) und Drehung zu Ada kamen erst NACH der Zeile.
- **Messung vorher** (Abnahme Lauf A, state.log F76-F166): `PL(8400,-350,rot=2048)`, `mo=15`, Nachricht 6; Soll-Gierung
  zu Ada (6000,11500) = 2942 -> 894/4096 = 78,6 Grad daneben; neck.log Zeile 2 erst `PLC_NECK slot=0 mode=1`.
- **Beleg (Form):** ROOM11C0 sub02 @0x01886 `41 01 fb dc 00 00 f5 c7 64 00` (Plc_neck Modus 1), @0x01890
  `40 00 09 00 fb dc f5 c7` (Plc_dest Modus 9) + `18 05` (warten), `29 0d` (Cut_chg), @0x0189C `09 0a 14 00` (Sleep 20),
  @0x018A0 `2b 00 00 00` + `3f 00 0f 00` (Zeile + Clip 15). Handler Plc_dest @0x80041be4, Plc_neck @0x80041e98 (§2).
- **Aenderung:** tools/r35_k/szene_bauen.py -> gen/cut10f0_szene.inc (jetzt 1350 Bytes, 288 Opcodes; vorher 1326/282):
  nach `Cut_chg 2` kommen `2e 01 00 00` Work_set Spieler, `41 01 70 17 00 00 ec 2c 64 00` Plc_neck Modus 1 auf Ada
  (6000,11500), `40 00 09 00 70 17 ec 2c` Plc_dest Modus 9 + Warteschleife Ck(5,0) (ROOM1050 sub03 @0x00DCA), dann wie
  bisher Sleep 50, `Cut_chg 0`, Sleep 20, Message_on 6 + Clip 15. Der spaetere doppelte Plc_neck vor dem Losgehen entfaellt.
  Positionen/Text unveraendert (NUTZER-VORGABE). Reihenfolge wie im Vorbild: Blick, Drehung, Schnitt, 20 Bilder, Zeile.
- **Messung nachher:**
  - Unit (`test_r35_cut10f0.exe szene`, echte VM): `ok: Zeile 6 'Hey - how did you came in here?' (Leon an der Tuer):
    Leon blickt zu Ada (Gierung 2941, Soll 2942)`, `ok: Zeile 6: Leon steht dabei noch am Tuer-Spawn (8400,-350)`;
    Ablauf `msg 6..23 ab B81..B2464 | Leon los B213 an B423 (4797,11359) rot 1415 | Ende B2729`; Zeilenabstaende
    `6:342 7:110 8:110 9:151 10:298 11:126 12:110 13:110 14:90 15:126 16:130 17:110 18:130 19:110 20:110 21:110 22:110`
    (Minimum weiter 90).
  - Echte exe, echter Weg (Spielstand ROOM10D0 + CONTINUE + Aktionstaste, beschleunigter Renderer, RE15_STATE_LOG,
    RE15_NECK_LOG, RE15_FRAMEDUMP 60-240/20; Scratch r35k_nb1/m3): `[scd F6] Cut_chg(2)`, `[scd F66] Cut_chg(0)`,
    `[msg] room=10f0 id=6`; state.log F69..F199 `PL(8400,-350,rot=2941,...)`, `mo=15` F86-F176, danach `mo=23`;
    neck.log Zeile 1 `PLC_NECK slot=0 mode=1 tgt=(6000,0,11500)` (jetzt VOR der Zeile).
  - Bilder (angesehen): K_belege/nachbesserung1_zeile6_cut0_F080_bis_F180.png (Cut 0, Untertitel "Leon: Hey - how did
    you came in here?"), K_belege/nachbesserung1_zeile6_leon_arm_zu_ada_F080_F100_F120_F140.png (Ausschnitt 3-fach): Leon
    steht der Kamera — und damit Ada, die hinter der Kamera im Raum steht — zugewandt, ab F100 ist der linke Arm nach vorn
    gestreckt. Vorher stand er im Profil zur Westwand.
- **Test:** unit_r35_cut10f0_szene prueft jetzt auch Zeile 6 (Tabelle w[], k = 0, Toleranz 160/4096).

### 9.3 Mangel 1 — MAIN01 verstummt, wenn ein Raumskript den MAIN-Kanal stoppt
- **Ursache:** im Fenster liefert die Weiche in jedem Raum MAIN01; der Befehl des Raumskripts an Slot 0 meint aber die
  TABELLEN-Musik des Raums (Zwinger: MAIN3B mit Handstart-Flag, 0xFF7B). Er traf die geladene MAIN01-Sequenz
  (FUN_80044da4 op 2 @0x80044e50 = SsSeqStop), und weil danach jeder Raum denselben MAIN traegt, startet nichts sie neu
  (FUN_80044210 @0x80044280; FUN_800444b0 -> SsSeqReplay kehrt bei gestoppter Sequenz um, @0x8005ac94). Das Original
  verhaelt sich im selben Fall genauso — der Engine-Pfad ist richtig, der Fehler liegt in der Weiche der Spur K.
  Dieselbe Bauart trifft die Nutzlast-Schreiber: ROOM1090 sub00 @0x022EE setzt Programm 0 der MAIN-Bank (Lautstaerke/Pan)
  und sub03 @0x024DA macht es stumm — im Fenster waere das Programm 0 von MAIN01 gewesen (@0x80044f50/@0x80044f6c).
- **Messung vorher** (Abnahme J4/J5): `[bgm] stage=0 room=1D entry=FF01 ... [unveraendert, laeuft durch]`,
  `[bgm] Sce_bgm_control slot=0 op=2 ... capTick=519`, Pegel 580 -> 0; nach der Tuer in ROOM1180 `room=18 entry=FF01 ...
  [unveraendert, laeuft durch]` bei Pegel 0 ueber 450 Ticks.
- **Aenderung:**
  - cut_10f0.c: `s_bgm_stand` = die letzte Auskunft der Weiche an die Audio-Schicht (1 = 0xFF01 geliefert);
    `re15_cut10f0_bgm_haelt_main()` gibt sie heraus.
  - audio_pc.c, SCD_AUDIO_SEQ_CTL: Befehl an Slot 0 bei `re15_cut10f0_bgm_haelt_main()` -> nicht anwenden (weder
    Play/Stop/Pause noch die Nutzlast), Logzeile unter RE15_BGM_CTL_DEBUG. Slots 1/2 (SUB) und der direkte Aufruf
    `re15_audio_seq_ctl` (Game Over, main.c) bleiben unberuehrt. In ROOM11C0 (Raum-Byte 0x1C) liefert die Weiche die
    Tabelle -> dort gelten die Skript-Befehle wieder (sub00 @0x01810 `54 00 01 00 00 00` startet MAIN16).
  - Beim OEFFNEN des Fensters leert der Tick die Skript-Latches (`re15_audio_bgm_status_reset`, wie room_common.c vor
    jedem Raumwechsel): was das Raumskript vorher der Tabellen-Musik mitgab, wird beim Laden von MAIN01 nicht mehr
    nachgezogen (re15_bgm_load_main -> re15_bgm_vabw_apply).
- **Messung nachher** (echte exe, echter Weg: Stand im Flur ROOM1180 mit Strom (4,243), die fuenf Zwinger-Gegner tot,
  (9,71)=1, (9,73)=1; Aktionstaste -> Tuer DOOR1A -> ROOM11D0; RE15_BGM_CTL_DEBUG, RE15_AUDIO_CAP_SYNC; Scratch
  r35k_nb1/m1):
  `[bgm] stage=0 room=18 entry=FF1D`, `[save] CONTINUE: resumed in room 1180`, `[cut10f0] MAIN01-Fenster auf in ROOM1180:
  (9,71)=1 (9,73)=1, Parkplatz erreicht (4,64)=0`, `[cut10f0] Raummusik-Anstoss in ROOM1180: Soll MAIN01`,
  `[bgm] stage=0 room=18 entry=FF01 -> MAIN01(flag 0) SUB--`, `[bgm] loaded: 9 VAGs, SEQ 7788B`, Tuersequenz,
  `[bgm] stage=0 room=1D entry=FF01 -> MAIN01 ... [unveraendert, laeuft durch]`,
  `[bgm] Sce_bgm_control slot=0 op=2 im MAIN01-Fenster NICHT angewandt (Runde 35 Spur K) capTick=736`.
  Tonpegel (mittlerer Betrag je 30 Ticks = 1 s, 1470 Stereo-Frames je Tick) ab dem Stop-Befehl bei Tick 736, 15 s im
  Zwinger: 691, 444, 432, 795, 827, 804, 852, 820, 1182, 1517, 1428, 1513, 1786, 1703, 1322, 1603 — kein Abfall auf 0
  (vorher 580 -> 0).
- **Test:** integration_r35_cut10f0 Lauf G (derselbe Weg; prueft "laeuft durch" in den Zwinger, die Zeile "NICHT
  angewandt" und dass danach KEIN Slot-0-Befehl mehr angewandt wird); unit_r35_cut10f0_bgm ("Zwinger ROOM11D0 im
  Fenster: MAIN gehalten", "im Parkplatz ... gelten wieder").

### 9.4 Mangel 2 — MAIN01 beginnt eine Szene zu frueh
- **Ursache:** die Weiche hing nur an (9,71) "10F0-Szene gesehen" und wurde am Ende der 10F0-Szene angestossen. Der
  Nutzer-Satz steht in AUFTRAG.md Z.92 HINTER der 1150-Montage ("Bis Leon DANN den Parking Lot erreicht hat") — der Weg
  zu Irons und dessen Todesszene (Spur L) liefen sonst schon unter MAIN01.
- **Messung vorher** (Abnahme): Lauf A F2731 `entry=FF01` am Ende der 10F0-Szene; J1 `room=15 entry=FF01` statt
  Tabelle 0xFF1E.
- **Beleg:** VERTRAG §1.1 — Bit (9,73) = "Irons-Todesszene gesehen" (Spur L; K liest es nur). "Szene laeuft" =
  `re15_cine_active()` = flag(1,27) || flag(2,7), die Rahmen-Flags jeder Original-Szene (ROOM1090 sub02
  @0x02414/@0x02418 gesetzt, @0x024BE/@0x024C2 geloescht; Balken FUN_80021a0c @0x80021a24). Musikwahl beim Raumaufbau
  FUN_80044210 (@0x800399b0), LOAD-Reihenfolge @0x8002629c (§8.5).
- **Aenderung** (cut_10f0.c, re15_cut10f0.h RE15_CUT10F0_BGM_START_BANK/_BIT = (9,73)):
  - Fenster-Bedingung `fenster_flags()` = (9,71) UND (9,73) UND "Parkplatz noch nicht erreicht".
  - `s_bgm_offen` (fluechtig): oeffnet erst, wenn die Bedingung steht UND keine Szene laeuft. Setzt Spur L (9,73) am
    Anfang ihrer Montage, beginnt MAIN01 trotzdem erst mit deren Ende (Rahmen-Flags geloescht); setzt sie es am Ende,
    genauso. Einmal offen, bleibt es ueber spaetere Szenen offen ("durchweg").
  - Der Anstoss gehoert jetzt dem Tick (`bgm_fenster_tick`, je Spielbild): weicht die letzte Auskunft an die
    Audio-Schicht (`s_bgm_stand`) vom Soll des laufenden Raums ab, ruft er `re15_audio_start_room_bgm`. Das deckt das
    Montage-Ende (kein Haken bei Spur L noetig), das LADEN (der Boot-BGM-Aufruf laeuft vor dem Restore) und das Laden
    eines Stands ausserhalb des Fensters. Der CONTINUE-Haken in platform/pc/main.c (§8.5) ist dadurch entfallen.
  - re15_cut10f0_tick stoesst am Ende der 10F0-Szene KEINE Musik mehr an (nur noch der Kartenhinweis).
- **"Parkplatz erreicht" neu = Original-Flag (4,64)** (RE15_CUT10F0_ZIEL1_ERREICHT_*), fuer das Fenster-Ende UND fuer
  die Kachel ROOM11C0 (map_hint_common.c Eintrag K1, `erreicht` 4/64). Beleg: ROOM11C0 sub01 @0x01820 `21 04 40 00`
  Ck(4,64)==0 -> @0x01824 `04 0a 18 02` Evt_exec sub02; sub02 @0x0184E `22 04 40 01` als ERSTES Opcode (die
  Ankunftsszene "Ada! Where's Marvin?" @0x018A0); sub00 @0x0176C waehlt damit den Spawn. Zensus 240 RDTs (Rohbytes
  `22 04 40`, `21 04 40`): nur diese drei Stellen; kein Port-Nutzer.
  Grund (K_abnahme_0.md §9 "Zusammenfuehrung K <-> L", jetzt mit Beleg): das Besucht-Bit der Zone setzt JEDER
  Raumaufbau — scd_room_setup.c ruft nach main00/sub00 `re15_map_zone_update(g_current_room_id, Spieler)` —, also auch
  der Montage-Schnitt der Spur L nach ROOM11C0 Cut 13, bei dem Leon gar nicht dort ist. Mit dem Zonen-Bit haette die
  Montage die Kachel geloescht und das Fenster geschlossen, bevor es aufging. (4,64) setzt nur die Ankunftsszene selbst,
  die laut Auftrag spaeter noch kommt ("wo Ada in der spaeteren cutscene schon steht").
- **Messung nachher** (echte exe, integration_r35_cut10f0, debug.log der Laeufe):
  - Lauf A (echte Tuer ROOM10D0 -> ROOM10F0, (9,73) per RE15_SET_FLAG_AT mitten in die laufende Szene gesetzt —
    Stellvertreter fuer die Montage): `[bgm] stage=0 room=0F entry=FF20 -> MAIN20`, `[setflag-at] Frame 300:
    flag(9,73/0x49) = 1`, ... (kein `entry=FF01` waehrend der Szene) ..., `[cut10f0] Szene zu Ende`,
    `[cut10f0] MAIN01-Fenster auf in ROOM10F0: (9,71)=1 (9,73)=1, Parkplatz erreicht (4,64)=0`,
    `[cut10f0] Raummusik-Anstoss in ROOM10F0: Soll MAIN01 (letzte Auskunft an die Audio-Schicht: Tabelle)`,
    `[bgm] stage=0 room=0F entry=FF01 -> MAIN01(flag 0) SUB--`.
  - Lauf C (10F0-Szene ohne (9,73)): nur `room=0F entry=FF20`, kein FF01.
  - Lauf F (Stand in ROOM1150, (9,71)=1, (9,73)=0): `[bgm] stage=0 room=15 entry=FF1E -> MAIN1E` (Tabelle, vorher
    FF01), `[cut10f0] ROOM1150 nach der Szene betreten: (9,72)=1`, bei Bild 60 `[setflag-at] ... flag(9,73)`,
    `[cut10f0] MAIN01-Fenster auf in ROOM1150`, `[bgm] stage=0 room=15 entry=FF01 -> MAIN01`.
  - Lauf D (Laden im Fenster): `room=0F entry=FF20`, `[save] CONTINUE: resumed in room 10f0`, `MAIN01-Fenster auf in
    ROOM10F0`, `Raummusik-Anstoss ... Soll MAIN01`, `room=0F entry=FF01`, Tuer, `room=0D entry=FF01 ... [unveraendert,
    laeuft durch]`.
  - Lauf E (Ende): `room=1B entry=FF01`, Tuer DOOR1A, `[bgm] stage=0 room=1C entry=FF56 -> MAIN16(flag 1)`,
    `[cut10f0] MAIN01-Fenster zu in ROOM11C0: (9,71)=1 (9,73)=1, Parkplatz erreicht (4,64)=1` — die Ankunftsszene
    setzt das Flag am echten Weg.
  - Unit `bgm`: "nach der 10F0-Szene allein ... weiter Tabelle", "waehrend der Montage ... kein MAIN01, auch nicht bei
    einem Raumaufbau, kein Anstoss", "Montage-Ende: Fenster offen, Raummusik EINMAL angestossen (Raum 0x15, 1x)",
    "Besucht-Bit der Zone ROOM11C0 (Raumaufbau ohne Leon) beendet das Fenster nicht", "Parkplatz erreicht ((4,64)=1):
    Fenster zu", "nach dem Laden im Fenster: das erste Spielbild stoesst die Raummusik an", "Stand ausserhalb des
    Fensters geladen: ... zurueck auf die Tabelle". Unit `karte`: "Raumaufbau ROOM11C0 allein (Montage-Schnitt, Zone
    besucht): die Kachel blinkt weiter", "Ankunftsszene ROOM11C0 gestartet ((4,64)=1) -> kein Ziel mehr".
- **Folge im Baum der Spur K allein:** (9,73) setzt hier niemand — MAIN01 erklingt im eigenen Baum nur in den
  Messlaeufen. Erst mit Spur L laeuft es im Spiel (siehe "Fuer die Zusammenfuehrung", §9.7).

### 9.5 Mangel 4 — VERTRAG §1.4 "nur kleine Haken (1-5 Zeilen)"
- **Messung vorher** (`git diff master --numstat`, HEAD 7b20a561): menu_common.c 41/2 (Block in map_mode ~20 Zeilen,
  Funktion hint_wechsel 12, menu_task_step 6); platform/pc/main.c 43/0 (Funktion pc_rbj_leihen 20, Haken 6/6/3/6).
- **Aenderung:**
  - Hinweiskette und Folge-Ziel (vorher map_mode-Block + hint_wechsel) -> cut_10f0.c `re15_cut10f0_hinweis_kette`;
    zweites Kartenziel -> cut_10f0.c `re15_cut10f0_ziel2_setzen`. Verhalten unveraendert (dieselben Logzeilen
    `[hint] F.. Folge-Hinweis 1 -> 2 (Zeit)`; neu `[hint] F.. Zeit um (Hinweis 2, 3 Blinkperioden)` vor dem
    unveraenderten `[hint] F.. schliessen`).
  - pc_rbj_leihen -> NEUE Datei platform/pc/src/cut10f0_pc.c `re15_cut10f0_pc_rbj_leihen` (liest ueber
    re15_pc_read_any, dieselbe Wurzelliste wie pc_read_shared). CONTINUE-BGM-Haken entfernt (§9.4).
- **Messung nachher** (`git diff master --numstat` / `-U0`, Stand dieses Abschlusses):
  - engine/src/menu_common.c **8/1**: include (1), map_mode @1595 (4 neu + die `if`-Zeile), menu_task_step @2429 (1)
    und @2432 (1).
  - platform/pc/main.c **12/0**: include (1), Boot-Leihe @4234 (4: 2 Kommentar + 2 Code), Boot-Installer @4900 (3),
    Raumwechsel-Leihe @8157 (4: 2 + 2).
  - unveraendert klein: scd_room_setup.c 5 (include + 4), scd_vm.c 3, game_step_common.c 2, enemy_common.c 4.
  Kein Haken ueber 5 Zeilen. Nicht auf der Vertragsliste, aber mit Haken dieser Spur: audio_pc.c 23 (Weiche 3,
  Port-Bank 0x0E 10, MAIN-Sperre 9, include 1), map_hint_common.c, re15_inv_screen.c/.h, tests/test_support.c.

### 9.6 Fortsetzung der Nachbesserung 1 (dritte Sitzung, 2026-10-03 ab 17:49, nach dem Guthaben-Limit 17:39)
- **Stand beim Einstieg:** HEAD 7cd02507 (Dossier-Rest = die 5 Kopfzeilen "§9 = NACHBESSERUNG 1", gegengelesen, stimmen
  mit §9.2-§9.5 ueberein). Baum sauber. Ein vom Vorgaenger um 17:35:48 gestarteter `local_build.sh all` lief noch
  (PID 28204/40988, ctest bei Test 406/486, exe 17:35:55 = Stand fc841604 = Code des HEAD); nicht abgebrochen,
  Ergebnis unten (§9.8). Bis dahin ein Fehlschlag im Log: `integration_r32_tor_hell` (5,16 s, Fenster-Haken unter Last).
- **Je Mangel festgestellt (Diff 702bca9e..HEAD gelesen):** M1 (Stop im Fenster) umgesetzt + Lauf G gemessen (§9.3);
  M2 (Beginn (9,73)) umgesetzt + Laeufe A/C/D/E/F gemessen (§9.4); M3 (Zeile 6) umgesetzt + gemessen (§9.2);
  M4: menu_common.c 8/1, main.c 12/0 erledigt (§9.5). Offen waren: die Suite am Endstand, der Abschnitt "Fuer die
  Zusammenfuehrung" (§9.7, in §9.4 angekuendigt), OFFEN/Fuer den Nutzer und der Abschluss-Commit.
- **M4 Rest — audio_pc.c** (nicht auf der Vertragsliste, aber dieselbe Zusammenfuehrungs-Gefahr: Spuren A/B fassen den
  Ton an): die Haken waren 10 bzw. 9 Zeilen. Rumpf nach platform/pc/src/cut10f0_pc.c verlegt
  (`re15_cut10f0_pc_se_on`, `re15_cut10f0_pc_main_gesperrt`, gleiche Logzeilen), in audio_pc.c bleiben je 2 Kommentar-
  + 1 Codezeile. Rumpf zuerst in cut10f0_pc.c -> Linkfehler `test_rotor_bgm_pin` (bindet audio_pc.c ohne platform-
  Dateien, nur re15_engine); deshalb in engine/src/cut_10f0.c (`re15_cut10f0_se_on`, `re15_cut10f0_main_gesperrt`).
  `git diff master --numstat` audio_pc.c: **10/0** (include 1, ss_bgm_entry 3, Se_on 3, SEQ_CTL 3). Bau: LOCAL-BUILD-OK (build).
