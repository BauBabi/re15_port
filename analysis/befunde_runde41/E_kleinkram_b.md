# Runde 41 — Spur E "kleinkram_b" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_kleinkram_b` (Zweig `r41/kleinkram_b`, Basis master 116e8b0f). Begonnen 2026-10-23.

## Punkte (Wortlaut AUFTRAG.md)
1. Leon hat eine komische herumfliegende Animation im ROOM 2020 am Anfang wenn er die Kurbel nutzt.
2. Die Map B1 ist jetzt beinahe perfekt, aber bevor man in den vorderen Teil der Waffen Armory - room 1090 - geht -
   also die vordere Tuer - macht der Spieler auf der Map einen Sprung zum hinteren Teil, obwohl man garnicht da ist.
3. Die Tuer der Werkstatt in ROOM 11B1 bei Elza, wenn sie ueber ROOM 2000 rein kommt und die Cutscene abspielt wo sie
   Elza sucht, ist immer noch offen. Ich wollte das sie dauerhaft geschlossen bleibt in Elzas Szenario.

Ressourcen (VERTRAG §1): Bank-9-Bit 120 (nur wenn kein Original-Flag reicht), keine neuen Nachrichten-Ids
(nur vorhandene). Gemeinsame Dateien: scd_room_setup.c (Installer am Blockende), karte_r39.c / re15_map_zones.c,
elzazombies_r39.c, test_r36_kartenbits.c k_port[] (nur falls Bit 120 belegt).

## Status
- [ ] Punkt 1 Kurbel 2020
- [ ] Punkt 2 Karte B1 / 1090-Vordertuer
- [ ] Punkt 3 Werkstatt-Tuer 11B1 (Elza)

## Protokoll
- (Start) Baum sauber auf 116e8b0f, Dossier angelegt.

## Messung vorher (Stand 116e8b0f, exe-Kopie re15_pc_r41e.exe, 2026-10-23 22:16)
Alle Laeufe: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1`,
eigener Laufordner je Lauf (debug.log liegt im cwd der exe). Ergebnisse unter scratchpad/r41e/<lauf>/.

### Punkt 3 (11B1 Werkstatt-Tuer, Elza) — REPRODUZIERT
Lauf w6: `RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=11b1@240 RE15_SET_FLAG=3:0x83,3:0x46,4:0xf3
RE15_PLAYER_POS=-195,-21821,1024,0 RE15_SCD_TRACE=1` (= Ankunft aus ROOM1261 Slot 0 @0x0112A, Zustand nach
10B1 sub04 @0x0195E `22 03 83 01` "Schacht offen" und 2001 @0x01958 `22 03 46 01`).
- sub00 nimmt den Zweig (3,0x83)==1: Trace `slot=1 @0x10FA op=0x21 … @0x1108 op=0x29 (Cut_chg 7) @0x110A op=0x3C
  @0x110E op=0x4B @0x1112 op=0x4B (Cut_replace 0<->7, 2<->8 = Tuerblatt OFFEN gemalt) @0x1116 op=0x37 @0x111A op=0x39
  (Sca_id_set/Sca_floor_set (3,0x0C) = Durchgangszelle AUF) @0x111E Work_set @0x1122 Member_set @0x1126 Else`.
  Der Text-Platz 8 "The door is locked." (@0x112A) wird NICHT angelegt.
- sub03 ("Sherry!? / Where has she gone!?") laeuft F6..F473: @0x1438 Set(3,0x46)=0, @0x1450 Pos_set, msg 10 @0x1480
  (F84), msg 11 @0x14CE (F372), @0x14E6 Plc_ret (F473). Bilder w6_sheet.png.
=> Nach der Szene ist die Werkstatt-Tuer offen (Zelle passierbar, Bild offen) — genau der Nutzer-Befund.

### Punkt 2 (Karte B1, ROOM1190 = "Waffen Armory 1090" des Nutzers, Runde 36 M R0)
- Numerisch (probe_r39_karten marker): Gang 1230 an der Ostkammer-Tuer (96,12161) -> Marker (146,109) = Ostkammer-
  Marke; an der Hallen-Tuer (-8118,9355) -> (127,115) = Hallen-Marke. Der Tuer-Zug (tuer_anziehen) greift in der
  Sonde NICHT (RE15_ZUGRAND=0 aendert nichts; betrete() tickt die VM nicht, main00 legt keine Tuer-AOTs an).
- Echte exe, Kartenschirm im Gang (w2/w3/w4, RE15_INV_OPEN_AT=30#1230 + RE15_INV_FB_SHOT): Marker an der
  Ostkammer-Tuer (146,108) ueber dem Ostkammer-Kasten, an der Hallen-Tuer (127,115) — KEIN Sprung im Gang.
- Code-Befund (noch nicht gemessen): re15_inv_screen.c tuer_anziehen() sucht die Marke, deren ANDERE Zone zu
  ZIELRAUM (ziel = Stage/Raum aus door_params) gehoert — fuer ROOM1190 passen Halle (zid 26) UND Ostkammer (zid 110),
  die Tabelle s_map_marks (re15_map_zones.h:986/987) fuehrt die Hallen-Marke zuerst. Offen: ob/wann dieser Zug im
  echten Tuerweg greift (Laeufe w7/w10: Tuer per Aktionstaste, Kartenschirm im Zielraum).
- Tuer-Daten: 1180/1230 Slot 2 @0x00A54/@0x00B54 rect (-9266,8888,2297,934) -> 1190 (3174,-11182) [Halle];
  1230 Slot 4 @0x00CE6 rect (-1052,11694,2297,934) -> 1190 (19515,-12109) [Ostkammer]; in 1180 Slot 4 sce 0 (zu).
  Blickrichtung gemessen: yaw 0 = +x, 2048 = -x (w7a/w7b [walk]).

### Punkt 1 (Kurbel 2020) — noch NICHT reproduziert
Lauf w1/w5: `RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_PLAYER_POS=2600,-2800,2048,1 RE15_FIRE_AOT=3@300#2020
RE15_PRESS=square@340,square@380,square@420 RE15_ANIM_TRACE RE15_FRAMEDUMP`: sub02 (slot 3 sce 3 @0x009AE) laeuft wie
die RDT: msg 0 F301, msg 1 "Will you use the Crank?" F342, Ja F422 -> @0x0A7E Cut_chg 3, @0x0A8C Pos_set (3650,-1800,
-2925), Sleep 20, @0x0A9C Plc_motion(0,0,0) F443 -> Spieler Clip 0 der Raum-RBJ (ROOM2020 RBJ @0xBB0, rec0 Marker 1,
15 Knochen, 1 Clip, 40 Bilder) spielt F443..F482 einmal und HAELT bei Bild 39 (Plc_flg 0x04 wird nicht geschleift —
Nebenbefund, nicht der Nutzer-Punkt). Bilder w1_sheet.png / w5_sheet.png: Leon steht an der Kurbel und kurbelt,
kein "Fliegen". Offen: echter Weg mit Aktionstaste (w9a/w9b), Waffe (w9c, RE15_EQUIP=3), Szenenende.
