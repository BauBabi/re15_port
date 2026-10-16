# Runde 38 — Spur T "spiderman" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_spiderman`, Zweig `r38/spiderman`, Basis e2cc7673. Angelegt 2026-10-16 16:22.

## Punkte (Wortlaut des Nutzers, AUFTRAG.md)
1. "In Stage 4 die Spiderman kommen nicht richtig aus den Schächten raus wie im Original, obwohl die Schächte korrekt hoch gehen"
2. "In Stage 4 die Spiderman führen - mal wieder - dazu das Leon teilweise komplett von der Karte verschwindet, wenn sie einen Angreifen. Sowas darf GARNICHT passieren, bei keinen Gegner."

## Stand
- 16:22 Dossier angelegt; Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.
- 16:3x Identifikation (0x2B), Messung vorher (m1/m2), RE (EMERGE, Griffe, Hooks).
- 17:0x Umsetzung spiderman_r38.c + Haken, Messung nachher (m3-m6), Riegel unit_r38_spiderman.
- 17:2x Schritt 3: Zensus, Wandwaechter, Riegel unit_r38_griff_* (9 Faelle), Alt-Riegel r37_beretta angepasst.
- 17:48 gezielte Tests gruen (141/141), Dossier fertig.
- 18:03 Nachtrag (III): Bezugspunkt der Wandregel beim Kollaps ohne Griff (RE2-Hund) kalibriert, Riegelfall hund2 +
  vergifteter Standpunkt, Mutationsprobe; gezielte Tests 142/142.

## Kurzfassung
- Punkt 1: der Port liess den Spiderman (Typ 0x2B, grid 0x41) sofort beim Betreten aus dem Schacht laufen. Original
  (STAGE4.BIN Untertabelle @0x8011A270 [1] 0x801152EC): versteckt (Teile aus, word0 |= 2|0x40, kein Schatten), WARTEN
  bis das Raumskript grid 0x42 setzt (sub05 @0x015BA, derselbe Lauf, der den Deckel hochfahren laesst), dann Clip 0x1D:
  sichtbar ab Bild 6, greifbar/grid 0 ab Bild 0x21, Ende -> ACTIVE +0x5 = 1. Jetzt so gebaut; im Schacht kein Ziel.
- Punkt 2: Griff 1 (B[5]/B[9]) kopierte den gemeinsamen Anker nicht (FUN_8001ac38 @0x80112B08/@0x80113000) -> der
  Spieler wurde vom Anker eines frueheren Griffs aus platziert (gemessen 14 600 Einheiten Sprung, ROOM4001). Griff 2
  (B[15]) war ein erfundenes Schleppen; Original setzt den Spieler EINMAL (@0x80114204/14). Beides behoben.
  GENERELL: Zensus aller Lageschreiber (Tabelle 2), alle anderen Greifer kopieren ihren Anker; neue Regel fuer ALLE
  Greifer: kein Griff setzt den Spieler hinter eine Wand (ROOM1220 RE2-Kriecher vorher 132 Bilder hinter Zelle 7).
  Die Regel misst die Strecke ab dem letzten begehbaren Standpunkt; beim Kehlbiss des RE2-Hunds (Kollaps OHNE Griff)
  stammte der noch aus einem frueheren Griff -> jetzt im Kollaps-Eintritt neu gesetzt wie RE2 +0x118/+0x11A
  (`sh` @0x800356f4-f8). Ohne das: Spieler 302/302 Bilder ausserhalb des Raums (Mutationsprobe hund2).

## 0. Identifikation "Spiderman" (Zensus Stage-4-RDTs)
- Typ **0x2B** (Port: `re15_tyrant_ai_tick`, enemy_ai_common.c "TYRANT"). Namensbeleg: RE15Editor
  `info/.../RE15Editor/src/main/java/baubabi/reeditor/core/rdt/scd/ScdUtilities.java:230`
  `enemyMapForSelection.put("2B", "2B - SPIDER-MAN - Stage 4,5")`; Zeile 258: Zustand 65 (=grid 0x41)
  "not there (SPIDER-MAN)".
- Zensus (Werkzeug `T_belege/t_emset_zensus.py`, Sce_em_set 0x44 aller RDTs): Typ 0x2B steht in STAGE4
  NUR in **ROOM4001** (Elza; Leon-ROOM4000 hat an denselben Slots 0x29 "MONKEY"), sub00:
  - @0x0122C `44 02 2b 41 ...` slot 2 grid 0x41 (-8280,0,-16110) [Ck(4,20)==1 && Ck(3,0x91)==0], sonst
    @0x0126A grid 0x00 (-13880,0,-12560);
  - @0x012BE `44 03 2b 41 ...` slot 3 grid 0x41 (-20160,0,-11070) [Ck(3,0xAF)==0], sonst @0x012FC grid 0x00.
  - Schachtdeckel = Obj_model_set obj 1 @0x01240 (-8280,-630,-16110) / obj 2 @0x012D2 (-20160,-630,-11070).
  - Ausloeser: Aot_set slot 4/5 sce 3 (@0x01218/@0x012AA) -> sub05/sub07: `Set(3,0x91/0xAF)=1`,
    `Work_set(2,2|3) Member_set 0C=0x42` (@0x015BA/@0x016F0: grid := 0x42), Deckel obj 1/2 fliegt hoch
    (Speed_set/Add_speed ab @0x015CE/@0x01704) = "die Schaechte gehen korrekt hoch".
  - STAGE5: 0x2B in 5040/5041 + 5070/5071 (grid 0x41 bzw. 0), 5110/5111 + 5130/5131 (grid 0) — Leon UND Elza.
    (Das Debug-Menue zaehlt Stages ab 0: "Stage 4" dort = Spiel-STAGE5, debug_jump_table.h:79.)
- Registrierung STAGE4.BIN (Werkzeug `T_belege/t_hook_scan.py`): `sw` @0x801183F8 -> 0x80072C58 = Typ 0x2B
  Root 0x801118D0; Spieler-Hook A (cmd 5) @0x801184E8 -> 0x800AC804 = **0x80115CC0**; Hook B (cmd 6)
  @0x80118574 -> 0x800AC904 = **0x801161DC**. STAGE5: Root 0x80111A50, Hook A 0x80115E40, Hook B 0x8011635C
  (Kopie +0x180).

## 1. Punkt 1 — Austritt aus dem Schacht
### Messung vorher (Port e2cc7673, Lauf m1/m2)
Aufbau: `T_belege/mess/lauf.sh` (Messkopie der exe), Elza (RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1),
RE15_DEBUG_JUMP=4000@250 (-> ROOM4001, debug.log "JUMP -> 400 LAB ENTRANCE (ROOM4001)"), RE15_SET_FLAG=4:20
(Ck(4,20)==1 = Spiderman-Zweig von sub00), RE15_STATE_LOG.
- F1 nach dem Sprung: Slot 3/4 Typ 0x2B grid 0x41 st=0 an (-8280,-16110) / (-20160,-11070).
- F61: BEIDE schon st=1 (ACTIVE) ss1=0, grid weiter 0x41 — der Port-EMERGE (enemy_ai_common.c case 4) spielt
  Clip 0x1D sofort ab und schaltet nach dem Clip auf ACTIVE, OHNE auf den Ausloeser zu warten; sie laufen
  aus der Wand los, bevor der Spieler den Schacht-Bereich (AOT 4/5) je betreten hat (grid wird nie 0x42).
- Sie sind waehrenddessen gezeichnet (kein Verstecken), zielbar und schiebbar.

### RE-Belege (STAGE4.BIN, RAW @0x80100000; Werkzeug re15_disasm.py)
- INIT 0x80111A64: grid 0x40 -> `sw 4,4(a0)` @0x80111B5C (Zustand 4, +0x5=0); grid 0x41 -> `ori v0,zero,0x104`
  @0x80111B78 / `sw` @0x80111B7C (Zustand 4, +0x5=1); grid 0x43 -> 0x204 @0x80111B9C-A0. word0 &= 0x1FFFFFFF
  @0x80111C14-20 (Hoehenband geloescht).
- EMERGE = Zustandstabelle @0x8011A0B4 [4..6] = 0x80114FE4 -> Untertabelle @0x8011A270 [+0x5]:
  [0] 0x8011502C, [1] 0x801152EC, [2] 0x801155AC (grid 0x43, in keinem RDT), [3] 0x80115AE8.
- [1] 0x801152EC (grid 0x41 = ROOM4001/5040/5070), Phase +0x6:
  - P0 @0x80115338: word0 |= 2 (@0x80115340) |= 0x40 (@0x8011535C); Schatten +0xBC/+0xBE = 0 (@0x80115370-74);
    Clip 0x1C, bei grid 0x41 Clip 0x1D (@0x80115380-A8); +0x95 = 0, +0x8F = 0 (@0x801153B8/C8); +0x6 = 1;
    anim_set; ALLE 26 Teile word0 &= ~1 (Schleife @0x801153F8-48, Stride 0xAC) = nicht zeichnen.
  - P1 @0x8011544C: WARTEN bis grid == 0x42 (`ori v0,zero,0x42 / bne` @0x8011545C-60) — kein anim_set;
    dann +0x95 = 0, +0x8F = 7, +0x6 = 2 (@0x80115468-88).
  - P2 @0x8011548C: bei Bild 6 alle Teile |= 1 (sichtbar, @0x801154A0-F4); bei Bild 0x21 (@0x80115508)
    word0 &= ~2 / &= ~0x40 (@0x80115514-3C), Schatten 0x3E8/0x44C (@0x80115548-54), grid := 0 (@0x80115564);
    anim_set, fertig -> +0x4-Wort = 0x101 (@0x80115588-98) = ACTIVE, +0x5 = 1 (B[1] Laufen).
  - [0] 0x8011502C ist dieselbe Maschine mit dem Freigabe-Bild 0x32 (@0x80115248) statt 0x21.
- Ausloeser im RDT: ROOM4001 sub05 @0x015BA / sub07 @0x016F0 `Member_set 0C=0x42` (Member 0x0C -> +0x9 grid,
  FUN_8004116c) im selben Skript, das den Deckel (obj 1/2) hochfahren laesst.
- Zielbarkeit: Aim-Latch FUN_8003703c ueberspringt grid & 0x60 (`lbu a0,-145(s0)` = +0x09, `andi v0,a0,0x60 / bne`
  @0x80037104-10); der Treffer-Resolver FUN_80011f50 braucht ein Hoehenband in word0 & 0xE0000000
  (@0x800120d0-ec), das 0x2B nur im ACTIVE-Schwanz stempelt (@0x80111DCC-E24) — INIT loescht es. Im Schacht
  ist der Spiderman also weder anvisierbar noch treffbar.
- Koerperschub: word0 & 2 -> FUN_8002aec4 ueberspringt das Paar (Port re15_body_push, @0x8002af04-af30).
- Schatten-Pflege im Root-Schwanz nur ohne grid & 0x40 (`andi v0,v0,0x40` @0x801119CC).

## 2. Punkt 2 — Spieler verschwindet von der Karte
### Messung vorher (Port e2cc7673, Lauf m2 = m1 bis Bild 3000)
- F1225 Elza (-15518,-2468) hp 50, Spiderman Slot 4 (-15172,-2818) ATTACK1 (ss1=4).
- **F1228: GRIFF (ss1=5, Clip 14 = 0xE) -> Spieler springt in EINEM Bild nach (-1448,1503) = 14 600 Einheiten**,
  bleibt den ganzen Griff/Kollaps bei ~(-450,-400)..(-100,30) (= Welturprung + Opfer-Clip-Versatz), stirbt dort
  (F1315 hp -1, pst 7). Genau "Leon verschwindet komplett von der Karte".
- Ursache (Code): enemy_ai_common.c Typ 0x2B B[5]/B[9] rief nur `re15_player_victim_latch` — KEIN Anker. Die
  generische Opfer-Maschine platziert den Spieler jedes Bild ABSOLUT = pl->anchor + RotY * Wurzelversatz
  (re15_victim_place -> re15_clip_root_motion_abs, = FUN_8001ad68). pl->anchor stammt aus einem frueheren Griff
  (oder 0,0). Dasselbe Muster wie "mal wieder": 585c239a (Hund, "verschwinde komplett", Anker (0,0)) und
  9661e369 (ROOM1210-Arme, 16 508 Einheiten) — jedes Mal ein Greifer ohne Anker-Kopie.
- Kein Regress, sondern seit dem Tyrant-Ausbau (audit wf_efd92a2c) nie verdrahtet: B[5]/B[9] hatten nie einen
  Anker, B[15] setzte den Spieler jedes Bild auf die Gegnerposition und schob den Gegner 40/Bild ohne Wand.

### RE-Belege
- B[5] GRIFF vorn 0x80112A64, Phasen @0x8010026C: P0 @0x80112AA8 Clip 0xE, +0x8F=7, `jal 0x8001AC38`
  @0x80112B08 mit a0 = 0x800ACA54 (Spieler): FUN_8001ac38 setzt den Anker des GREIFERS = pos - RotY*Wurzel(Clip,
  Bild 0) aus +0x84/+0x16C (@0x8001acfc/@0x8001ad18) und KOPIERT ihn auf den Spieler (@0x8001ad28-48); DANACH
  Yaw-Snap a8f8(Spieler, 0x800) @0x80112B8C. Jede Phase platziert den Spiderman: `jal 0x8001AD68` @0x80112BF4
  / @0x80112D5C (Anker + RotY * Wurzel). B[9] GRIFF hinten 0x80112F58: dasselbe, Clip 0x12, Anker @0x80113000.
- B[8] KOLLAPS 0x80112E20: aca58=6, aca59=0, Platzierung `jal 0x8001AD68` @0x80112F00 (Anker des Griffs).
- Spieler-Hook A [0x2B] = 0x80115CC0 (Tabelle @0x8011A298 nach aca59): [0]/[1] 0x80115D00 (Phasen
  @0x8010037C): Clip = aca59*4 (@0x80115D78-84), Halten aca59*4+1 (@0x80115E58-64), Loesen aca59*4+2
  (@0x80115ED4-E0); Platzierung FUN_8001ad68(Spieler, Opferbank) @0x80115E00/@0x80115E80/@0x80115F38.
  [2]/[3] 0x80115FF0 (Phasen @0x80100394): Clip 8 (@0x80116044), 9 (@0x801160C8), 0xA (@0x80116108) — KEINE
  Platzierung (kein jal 0x8001ad68 in 0x80115FF0-0x801161D8), kein Yaw-Snap.
- Spieler-Hook B [0x2B] = 0x801161DC -> [0] 0x8011621C: Kollaps-Clip aca59*4+3 (@0x80116288-98), Platzierung
  @0x801162BC.
- B[15] GRIFF 2 0x8011411C (Phasen @0x80100324): P0 @0x80114160 Clip 0x18, Anker +0xA0/+0xA2 = eigene Lage
  (@0x801141B4-D4), +0x8C = 0x66C (@0x801141E0-E8), pos_advance(0x800) @0x801141E4, **Spieler X/Z = vorgeschobene
  Lage** (@0x80114204/@0x80114214), eigene Lage zurueck (@0x80114218-38), aca58=5, aca59 = a780(Spieler)+2
  (@0x8011426C-84), EINMALIG. P2 @0x801142C8 Clip 0x19, +0x9C=100; P3 @0x801142F8 hp<0 -> hp=0 (@0x8011432C)
  + Loesen, sonst +0x9C -= 1 + 8*Tasten (`sll v0,v0,3` @0x80114358); P4 @0x80114398 Clip 0x1A; P6 @0x80114414
  +0x1DE=0x1E, +0x5=0xD. Kein Spieler-Schreiben pro Bild, kein Schieben des Spiderman.
- EM2B-Baenke (gemessen, CDEMD1.EMS Sektor 1104): Hauptbank 30 Clips (0xE fc 15 Wurzel x 354..-363, 0x12 fc 34),
  Opferbank 11 Clips (0 fc 15 Wurzel x 2089..1070, 3 fc 91 Kollaps, 8/9/0xA fc 6/30/30).
### Zensus: alle Stellen, die die Spielerposition schreiben (Schritt 3 GENERELL)
Gesucht (engine/src, Stand e2cc7673): jede Schreibstelle `pl/player->x|z`, jede absolute Platzierung (re15_victim_place
= FUN_8001ad68 / RE2 0x80015CB8) und jeder Opfer-Latch/-Kollaps (re15_player_victim_latch*/devour/re2z_victim_begin).
Die Fehlerklasse "Leon verschwindet" hat ZWEI Formen: (I) Platzierung vom Anker eines frueheren Griffs (Greifer kopiert
seinen Anker nicht = FUN_8001ac38-Kopie fehlt), (II) eine einzelne Versetzung ueberquert eine ganze Wand (beide
Original-Klemmen sind Punkt-Tests).

| # | Stelle | Greifer | Anker-/Lagequelle | Befund |
|---|---|---|---|---|
| 1 | enemy_ai_common.c re15_enemy_ai_live_grab [0] | RE1.5-Zombie Stehen | re15_clip_anchor_set(Lauf-Bank) + Kopie (@0x801025f0, @0x8001ad28-48) | ok |
| 2 | re15_zcrawl_grab_animate [0] | RE1.5-Kriecher | anchor_set + Kopie (@0x80103c04) | ok |
| 3 | re15_dog_grabhold LATCH | RE1.5-Hund | Hund- und Spieler-Anker = Spielerlage (@0x8010f8ec/f4); 0x0b nur aus grabhold | ok |
| 4 | re15_dog_state456 Fall 4/5/10/11 | RE1.5-Hund Maschine A/B | Latch OHNE Anker | unerreichbar: Zustand 4 setzt nur INIT mit +0x5 0/1 (@0x8010db88-b8) |
| 5 | re15_crow_move / enemy_ai_re2_crow.c:1123 | Kraehe | keine Platzierung (0x21 ausgenommen, 0x801159bc ohne ad68) | ok |
| 6 | maggot sub 15 PIN | Gorilla 0x27 | re15_affen_pin_anker (FUN_8001ac38 @0x8011ac18) | ok |
| 7 | 0x1A RE1.5-Zweig (~12880) | Gitterarme | Anker = Greiferlage + Kopie (9661e369) | ok |
| 8 | **B[5]/B[9] 0x2B** | **Spiderman Griff 1** | **kein Anker** | **DEFEKT -> behoben (FUN_8001ac38 @0x80112B08/@0x80113000)** |
| 9 | **B[15] 0x2B** | **Spiderman Griff 2** | **Spieler = Gegnerlage jedes Bild, Gegner +40/Bild ohne Wand, Opfer-Platzierung mit altem Anker** | **DEFEKT -> behoben (einmalig @0x80114204/14)** |
| 10 | B[8]/B[10] 0x2B | Spiderman Kollaps | Anker aus Griff 1 | ok (B[10] fehlte -> ergaenzt) |
| 11 | enemy_ai_re2_zombie.c:1866/1870, :2578 | RE2-Zombie Stehen/Kriecher | re15_re2z_grab_anchor (0x80015b94) | ok |
| 12 | enemy_ai_re2_dog.c:606/1260/1270 | RE2-Hund | re2d_grab_anchor; Latch geht DIREKT in den Kollaps (re15_re2dog_victim_latch -> devour) | Anker ok; Standpunkt der Wandklemme nicht neu gesetzt -> (III) |
| 13 | enemy_ai_re2_zellenarm.c:735-736 | RE2-Gitterarm | Spieler = Hand (@0x80100C18-38) | eigene Lage, kein Anker |
| 14 | enemy_ai_boss_gator.c:2097/2118 | Gator-Fress | Anker-Kopie + Lage am Maul | ok (eigene Maschine) |
| 15 | g5/tentakel/zellenarm/gator Schub | Bosse | Koerperschub (FUN_80034f..), Zug -100 | danach Spieler-Wandklemme |
| 16 | re15_victim_place (alle generischen Griffe) | alle | Klemme ab letztem Standpunkt (Runde 37 J) | **(II) Wanddurchgang moeglich: ROOM1220 RE2-Kriecher 132 Griffbilder hinter Zelle 7** |
| 17 | Rueckstoss cmd 2 / Koerperschub | alle | <= 200/Bild bzw. Ueberdeckung, danach Klemme @0x80031d70 | ok |

### Umsetzung (alle Greifer)
- (I) Spiderman behoben (s. o.); alle uebrigen Latch-Stellen setzen den Anker (Tabelle).
- (II) `re15_r38_griff_durch_wand` (spiderman_r38.c (3)): die Strecke letzter begehbarer Standpunkt -> neue Opferlage
  darf das Innere keiner soliden Typ-1-Zelle (u0 & 1 = Spieler-Maske, Klemmband) schneiden, sonst bleibt der
  Standpunkt. NUTZER-VORGABE "Sowas darf GARNICHT passieren, bei keinen Gegner." Form: kein neuer Wert; Original-Klemmen
  sind Punkt-Tests (RE1.5 FUN_8003b0a4 / Port push_rect; RE2 FUN_8004c1bc `subu v1,s4,v1 / addu v0,v0,a0 / sltu`
  @0x8004c370-80 und @0x8004c38c-9c = Lage+Radius gegen x/z/w/d), die einen Sprung UEBER eine Wand nicht sehen; im
  Original kommt er nicht vor, im Port setzen grosse Wurzelversaetze (RE2-Kriecher ~2000 in ROOM1220) ihn hinter
  duenne Waende. Haken: re15_victim_place (1 Zeile, enemy_ai_common.c) und die Griff-2-Lage.
- (III) Bezugspunkt der Regel (II) = `s_victim_ok` (Runde 37 J), bisher nur im Griff-Latch gesetzt
  (re15_player_victim_latch_ex / re15_re2z_victim_begin, Eintritt aus Zustand 0/3). Der RE2-Hund-Latch
  (re15_re2dog_victim_latch, Spieler-Haken 0x80104ACC) und jeder andere Kollaps ohne vorherigen Griff gehen DIREKT
  in re15_player_victim_devour -> der Bezugspunkt stammte aus einem frueheren Griff (oder (0,0) beim ersten Griff des
  Spiels). Fuer den alten Punkttest war das gleichgueltig, fuer den Streckentest (II) nicht: die Strecke vom alten
  Standpunkt kreuzt Waende -> Spieler bleibt auf dem alten Standpunkt = ausserhalb. Behoben: devour setzt den
  Bezugspunkt auf die Lage JETZT, wenn kein Griff laeuft (`g_player_victim` != 1/2). Beleg: RE2 FUN_8003567c
  (Spieler-Kollision, gerufen @0x80026634) schreibt nach dem Punkttest `jal 0x8004c1bc` @0x800356d0 in JEDEM Bild
  die Vorbildlage `lw v0,0x84(s0) / lw v1,0x8c(s0) / sh v0,0x118(s0) / sh v1,0x11a(s0)` @0x800356ec-f8 — der
  Bezug ist also nie aelter als ein Bild. Gefunden durch den neuen Riegelfall hund2 (vergifteter Standpunkt):
  MUTATIONSPROBE (Zeile entfernt): `302 ausserhalb`, Spieler (36700,21850) = vergifteter Standpunkt, FAIL;
  mit Zeile: 0 ausserhalb, 0 hinter einer Wand.

### Umsetzung (Spiderman)
- `re15_r38_sm_griff1` (B[5]/B[9]): Anker = Lage - RotY(Blick VOR dem Schnapp) * Wurzel(Clip 0xE/0x12, Bild 0) aus
  der Hauptbank, Kopie auf den Spieler, dann Schnapp, Platzierung ad68 je Phase, Halten 0xF/0x13 mit +0x9C=100,
  +0x9E=0x32, Tasten *100, Loesen 0x10/0x14 (SE 0 bei Bild 0x18), Ende +0x1DE=0x3C, +0x5=3. Ablauf: vorn -> B[8],
  hinten -> B[10] (fehlte im Port: hinten lief auf B[8]).
- `re15_r38_sm_kollaps` (B[8]/B[10]): Clip 0x11/0x15, aca58=6, aca59 0/1, SE 8, Platzierung vom Griff-Anker.
- `re15_r38_sm_griff2` (B[15]): einmalige Spielerlage (+0x66C hinter dem Blick, Wandklemme wie @0x80031d70),
  keine Schleppbewegung, Tasten *8, hp<0 -> 0.
- Opfer-Maschine: Clips 4v+{0,1,2,3} bzw. 8/9/0xA; Griff 2 ohne Platzierung/ohne Yaw.
- Haken in enemy_ai_common.c: Fall 5/9/8/10/15 rufen das Modul (vorher ~60 Zeilen ohne Anker), Clip-Belegung,
  Platzierungs-/Yaw-Ausnahme Griff 2, 5 schmale Zugaenge (Pin, Latch mit Variante, Variante, Clip-Vorschub/-Laenge).

### Messung nachher
- m5 (ROOM4001 Elza, Spiderman draussen, Spieler steht): F464 GRIFF vorn, Spieler bleibt bei (-15518,-2468)
  und schwankt mit dem Opfer-Clip innerhalb (-15534..-15286, -2468..-2671); Kollaps F529, Tod F589 bei
  (-14971,-3060). Groesster Bild-zu-Bild-Sprung des ganzen Laufs 476 (ATTACK1-Knockback), vorher 14 620.
- m6 (Leon ROOM5040): Griff hinten + Kollaps hinten, Spieler im Griff (-25886,-25198)..(-25423,-24613).
- Riegel unit_r38_spiderman (s. Tests) mit VERGIFTETEM Spieler-Anker (30000,30000): Latch kopiert den Anker,
  402 Griffbilder je Lauf in der Raumhuelle, kein Bild hinter einer Wand, Abstand max 1731 <= 6665.
  MUTATIONSPROBE (Anker-Kopie entfernt): 10 FAIL — Spieler 58 690 Einheiten in einem Bild, 402/402 Bilder
  ausserhalb des Raums. Der Riegel faengt also genau den gemeldeten Fehler.

## 3. Tests
- `unit_r38_spiderman` (test_r38_spiderman.c, ROOM4001 + echter game_step): austritt (600 Bilder versteckt, kein Ziel/
  Schatten/Angriff; Ausloeser Aot 4 -> grid 0x42 durch sub05; sichtbar +6, greifbar +0x21, ACTIVE +34; Gegenprobe
  Aot 5), griff1 zwei Laeufe mit vergiftetem Anker, griff2 (einmalige Lage, keine Bewegung, Clips 8/9, kein Yaw).
  Mutationsprobe Anker-Kopie entfernt: 10 FAIL.
- `unit_r38_griff_<fall>` (test_r38_griff_alle.c), je Fall echter Griff mit vergiftetem Anker ausserhalb des Raums:
  zombie15_vorn/_hinten (RE1.5-KI), zombie2_vorn/_hinten (RE2-KI + Hybrid wie die exe), kriecher2_1220 (duenne Wand),
  hund15, hund2 (RE2-Kehlbiss = Kollaps ohne Griff), gorilla (11C0 Kampf-Layout), spiderman_vorn/_hinten. Vergiftet
  sind Spieler-Anker UND Standpunkt der Wandklemme (bis zum Latch jedes Bild). Invariante je Griffbild: Raumhuelle +
  kein Wanddurchgang (eigener Slab-Test, nicht der Pruefling). Mutationsproben: Wandwaechter aus -> kriecher2_1220
  132 Bilder hinter der Wand, FAIL; Kollaps-Kalibrierung (III) aus -> hund2 302/302 ausserhalb, FAIL.
- Alt-Riegel angepasst: `integration_r37_beretta` Fall t2b_std_w5 schoss in den Schacht (Spiderman grid 0x41) — jetzt
  der schon draussen stehende Spiderman (Ck(4,0xD6)=1, Sce_em_set @0x017D4, Leon (-23943,-16785)); Absicht (drei
  Kugeln je -10) unveraendert, gruen.

### Gezielte Tests (kein volles Suite-Lauf im Spurbaum)
`ctest --test-dir re15_port/build -R "r38_|tyrant|t2b|5040|beretta|victim|grab|griff|bissarm|1210|dog|hund|re2z|crawl|kriech|affen|gorilla|maggot|devour|finisher|pushoff|1220|11c0|aim|ivy|enemy_ai|roster|stage4|stage5|spider|spinne|zombie" --timeout 900 -j 6`
-> `100% tests passed, 0 tests failed out of 142` (Total Test time (real) = 221.35 sec), Stand nach (III).
(Vorher 141/141 in 217.08 sec ohne hund2. Erster Lauf der Auswahl: 140/141, rot nur integration_r37_beretta t2b —
der Alt-Riegel schoss in den Schacht, angepasst s. o.; danach einzeln `ctest -R "^integration_r37_beretta$"`: 1/1.)

## OFFEN
1. **Deckenlauf des Spiderman nicht portiert.** A[3]-Ende @0x80112544-5C8: Wandkontakt (Zellattribut *(+0x1B4)+0xA
   & 0xC, grid & 0x19 == 0, Kontaktrichtung (+0x90 & 0xF0)<<4 im Fenster +-0x200 zum Blick) -> +0x9F = Kontakt, +0x5 =
   0xB (TURN, Clip 0x16) -> Regime 0xD (Clips 0x17-0x1A mit Wurzel-y -5400 = an der Decke). Im Port fehlt der
   Uebergang; Griff 2 ist deshalb im Spiel unerreichbar (byte-true vorbereitet, im Riegel erzwungen geprueft).
   Nicht Teil der zwei Nutzer-Punkte -> NACHLAUF.
2. **Spiderman-Kollaps-Darstellung** (Hook B [0] 0x8011621C / [1] 0x801165F4): Arm ab bei Spieler-Bild 0x1D, Kopf ab
   bei 0x3C (Teile-Records 12-14 bzw. 8, `xori 0x1062/0x200` @0x80116348-84/@0x80116450), Wundstempel @0x80116560-78,
   cmd 7 am Clip-Ende — der Port faehrt die Zombie-Zeitachse (hp -1 bei Bild 0x23 + Blut 0x37).
3. EMERGE [2] (grid 0x43, 0x801155AC) bleibt der bisherige Port-Pfad — kein RDT setzt grid 0x43 (Zensus).
4. Die Aim-Latch-Regel grid & 0x60 (`andi v0,a0,0x60` @0x8003710c) gilt im Original fuer ALLE Typen; hier nur fuer
   0x2B angewandt (Fensterhund grid 0x41, Gorillas grid 0x30 im 11C0-Szenenlayout usw. unveraendert — eigene Pruefung).
5. Schattengroesse des Spiderman draussen: Original 0x3E8/0x44C (INIT `0x8001af5c(..,0x3e8,0x44c)` @0x80111C44-50,
   Root-Schwanz @0x801119D4-F4), Port-Standard 500x600 — nicht gemeldet, nicht geaendert.
6. Leons Archiv CDEMD0 traegt in der EM2B-Opferbank nur 8 Clips (0..7); Griff 2 braucht 8/9/0xA (nur Elzas CDEMD1).
   Datenluecke des Prototyps; der Port faengt sie ueber die Clip-Grenzpruefung ab.
7. Kommentar beim Efeu (enemy_ai_common.c, IVY): "0x801165F4 ist verwaiste Efeu-Griff-VM" ist falsch — es ist der
   Spieler-Kollaps hinten des Spiderman (Hook-B-Tabelle @0x8011A2B8[1], erreicht ueber B[10] aca59 = 1 @0x8011338C).
   Nur Kommentar, kein Verhalten -> NACHLAUF.
8. Der Riegel unit_r38_griff_* misst 10 Greifer-Faelle. Gator/G5/Tentakel/RE2-Gitterarm platzieren mit eigener Mechanik
   (Tabelle 13-15) und sind nicht darin; die Gitterarme haben ihren eigenen Anker-Riegel (unit_1210_gitterhaende).
9. RE2-Hund-Kehlbiss (gemessen im Fall hund2, ROOM1140, Hund 1500 vor dem Spieler, hp 5): der Spieler springt am
   Kontakt-Latch 2554 Einheiten (auf den Flug-Anker hinter dem Hund) und beim Wechsel Sub 3 -> Sub 7 nochmals 3255
   (neuer Anker Clip 23 @0x80101DA0) — immer in der Raumhuelle, nie hinter einer Wand. Die ERSTE Versetzung liegt im
   Kontakt-Latch, fuer den Runde 30 (Kommentar enemy_ai_re2_dog.c am Kontakt-Latch) eine PORT-ABWEICHUNG belegt: der
   Block @0x80104F20-74 schreibt im Original KEIN Spieler-Kommando und KEINEN Anker, Kommando 6 + Anker fallen erst in
   Sub 7 P0 (@0x80101DC4 / @0x80101DA0); der Port startet die Opfer-Animation schon am Kontakt. Ob das Original damit
   genau EINE Versetzung (die zweite) hat, ist NICHT gemessen. Nicht Spiderman, nicht "von der Karte" -> NACHLAUF.

## Fuer den Nutzer
- **Spiderman kommt jetzt wie im Original aus dem Schacht:** er steckt unsichtbar im Wandschacht, bis man den Bereich
  davor betritt; dann faehrt das Gitter hoch und er klettert heraus (sichtbar ab dem 7. Bild, nach gut einer Sekunde
  greifbar), danach bruellt er und greift an. Vorher lief er schon beim Betreten des Raums los. Im Schacht kann man ihn
  nicht anvisieren (Original). Betrifft ROOM4001 (Elza, nach der Kendo-Szene) und die STAGE5-Gaenge 5040/5070 (beide
  Figuren; im Debug-Menue heisst STAGE5 "Stage 4").
- **Leon/Elza verschwinden beim Spiderman-Griff nicht mehr.** Der Griff stellte den Spieler vom Platz eines frueheren
  Griffs aus hin (bis 14 600 Einheiten weit, also ausserhalb des Raums). Jetzt uebernimmt der Spieler wie im Original
  den Ankerpunkt des Spiderman.
- **Fuer alle Gegner:** kein Griff setzt den Spieler mehr hinter eine Wand (gemessen beim Kriecher in ROOM1220, der Leon
  durch eine duenne Wand zog). Ein automatischer Test prueft das fuer Zombies (RE1.5/RE2), Kriecher, Hund, Gorilla und
  Spiderman mit absichtlich falschem Ankerpunkt.
- Keine neuen Spieldateien, keine Sprachdateien, nichts am Paket-Gate.
- Selbst pruefen: als Elza ROOM4001 nach der Kendo-Szene betreten und auf den rechten Wandschacht zugehen; oder als Leon
  ROOM5040 (Debug-Menue "Stage 4" -> B-2 CORRIDOR) und in den Gang laufen; sich greifen lassen.

## Nachbesserung 1 (nach Abnahme 0, 2026-10-16, fortlaufend)
Auftrag: Maengel M1 (Ein-Bild-Ruecksprung am Ende des Austritts, urteilsrelevant) und M2 (derselbe Ruecksprung an den
Phasenwechseln der neuen Griffe) aus `T_abnahme_0.md` beheben. Punkt 2 (erfuellt) nicht verschlechtern, Riegel gruen halten.
Der relayte Nutzer-Nachtrag ROOM20A0 (Rolladen vor der Kamera) gehoert zu Spur W (Baum r38_rolltor20a0), nicht zu T.

### Stand
- Lektuere: AUFTRAG/VERTRAG, Dossier, Abnahmebericht T_abnahme_0.md, spiderman_r38.c, re15_birkin_anim (enemy_ai_common.c),
  Render-Pfad der Gegner-Pose (platform/pc/main.c, re15_skel_compute_pose).

### M1 — Messung vorher (Stand 5db78deb = HEAD-Code, eigene Wiederholung der Abnahme-Messung 2.3 d)
`lauf.sh vor5070 ... RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 "RE15_DEBUG_JUMP=5070@250"
"RE15_PLAYER_POS=1700,-5700,2048" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=U9.5,W30"
RE15_STATE_LOG=state.log RE15_ANIM_TRACE=anim.log "RE15_EXIT_AT=420#5070"` (Leon, ROOM5070). RE15_ANIM_TRACE ist die Zeile,
die re15_compute_actor_kf beim ZEICHNEN schreibt (Spalten: Bild, Slot, Typ, motion, Clip, fc, anim_frame, gezeichnetes Bild):
```
375 1 43 29 29 35 34 34      state.log F375 [1 t=2b st=4 ss1=1 ss2=2 g=00 mo=29 af=34]
376 1 43 29 29 35 0 0        state.log F376 [1 t=2b st=1 ss1=1 ss2=0 g=00 mo=29 af=0]   <- M1: Clip 0x1D Bild 0 gezeichnet
377 1 43 2 2 50 1 1          state.log F377 [1 t=2b st=1 ss1=2 ss2=1 g=00 mo=2 af=1]    (B[2] Bruellen, Ueberblendung)
```
Framedump F374-F377 (RE15_WINDOW_SCALE=1): F374/F375 kauert der Spiderman ueber dem Bodengitter hinter Leon, F376 ist er
weg (Startpose 1278 unter dem Boden), F377 steht er versetzt unten im Bild (Ueberblendung AUS der falschen Startpose:
die Port-Ueberblendung re15_skel_compute_pose mischt mit 6*0x200 die zuletzt GEZEICHNETE Pose = Bild 0). Bild:
`T_bilder/nb1_vor_5070_F374-377.png`.

### RE-Belege Nachbesserung 1 (selbst disassembliert, re15_disasm.py)
- **anim_set FUN_8001f314 (PSX.EXE)**: Bildsatz = Clip-Tabelle + `lbu v0,149(t0)` (+0x95) @0x8001f35c -> `sll 2` / `addu`
  @0x8001f364-68 -> `sw a2,360(t0)` (+0x168) @0x8001f36c; `jal 0x8001f3bc` @0x8001f38c mit a3 = fc (`addu a3,t1,zero`
  @0x8001f390); Rueckgabe v0 von FUN_8001f3bc unveraendert (`j 0x8001f3ac` @0x8001f394).
- **FUN_8001f3bc**: zuerst die POSE des Bildes +0x168 (= +0x95 beim Eintritt): Keyframe laden `jal 0x8001f664` @0x8001f534,
  Ueberblendung `lbu/addiu -1/sb 143` (+0x8F) @0x8001f5a8-b4 + `jal 0x80020510` @0x8001f5d0, Matrizen `jal 0x80068098`
  (RotMatrix) @0x8001f574 / @0x8001f5f0 in die Teile-Saetze (+0x188). ERST DANACH `lbu v0,149(v1)` / `addiu v0,v0,1` /
  `sb v0,149(v1)` @0x8001f610-1c, `sltu v0,v0,s4` @0x8001f624 (s4 = fc), bei >= fc `ori v0,zero,0x1` @0x8001f638 und
  `sb zero,149(v1)` @0x8001f63c. => Im Bild, in dem anim_set "fertig" meldet, steht die Pose von Bild fc-1 auf dem
  Schirm; +0x95 ist danach 0. Ohne weiteren anim_set-Aufruf bleibt diese Pose stehen (die Matrizen werden nur hier
  geschrieben).
- **Austritt P2 (STAGE4.BIN)**: anim_set `jal 0x8001f314` @0x8011557C (a3 = 0x200), `beq v0,zero` @0x80115584, sonst
  `ori v0,zero,0x101` / `sw v0,4(v1)` @0x80115588-98. Im Wechselbild also Clip 0x1D Bild 34 (fc 35).
- **Erstes ACTIVE-Bild**: B[1] 0x80112124 P0: +0x6 = 1 @0x80112144, +0x94 = 1 @0x80112154, +0x95 = 0 @0x80112164,
  +0x96 = 0 @0x80112174, +0x8F = 7 @0x80112184, anim_set @0x801121F0. B[2] 0x80112294 P0: +0x94 = 2 @0x801122F0, +0x95 = 0
  @0x80112300, +0x8F = 7 @0x80112320, anim_set @0x80112360. => neuer Clip ab Bild 0, Ueberblendung aus der Pose von
  Bild 34 (kein Bild (0x1D, 0) dazwischen).
- **Griff 1 (M2)**, Phasentabellen `table 0x8010026C` = {2aa8, 2d44, 2b9c, 2bdc, 2cd0, 2d20, 2da0} (vorn) und
  `0x8010028C` = {2fa0, 3220, 3094, 30d4, 31ac, 31fc, 327c} (hinten):
  - P1 = Schwanz von P5: ad68 `jal 0x8001ad68` @0x80112D5C / @0x80113238, anim_set @0x80112D78 / @0x80113254,
    `lbu v1,6(a0)` / `addu v1,v1,v0` / `sb v1,6(a0)` @0x80112D8C-9C / @0x80113268-78 (+0x6 += fertig).
  - P2 @0x80112B9C / @0x80113094: NUR +0x94 = 0xF / 0x13 (@0x80112BA8 / @0x801130A0), +0x6 = 3, +0x9C = 0x64, +0x9E = 0x32
    (@0x80112BB8-D8 / @0x801130B0-D0) — KEIN +0x95-Schreiben. Der Halte-Clip beginnt also bei dem +0x95, das das
    Clip-Ende hinterliess: 0 (`sb zero,149` @0x8001f63c). P3 platziert mit (Halte-Clip, 0) und rechnet Bild 0.
  - P5 @0x80112D20 / @0x801131FC: SE 0 bei +0x95 == 0x18 (@0x80112D30-3C), dann derselbe Schwanz (P1) -> +0x6 = 6.
  - P6 @0x80112DA0 / @0x8011327C: +0x1DE = 0x3C, +0x5 = 3, +0x6 = 0, word0 &= ~0x1000, Spieler +0x93 &= 0xFE, `jr ra`
    @0x80112E10 / @0x801132F0 — KEIN anim_set: im P6-Bild bleibt die Pose von Bild fc-1 des Loese-Clips stehen.
    Danach B[3] P0: +0x94 = 3 @0x8011264C, +0x95 = 0 @0x8011265C.
- **Griff 2 (M2)**, Phasentabelle `0x80100324` = {4160, 43d8, 42c8, 42f8, 4398, 43d8, 4414}: P1 = P5 = 0x801143D8
  (anim_set @0x801143EC, +0x6 += fertig @0x80114400-10); P2 @0x801142C8: +0x94 = 0x19 @0x801142D4, +0x6 = 3 @0x801142E4,
  +0x9C = 0x64 @0x801142F4, kein +0x95; P6 @0x80114414: kein `jal` bis `jr ra` @0x80114484. Danach B[13] P0 0x80113D3C:
  +0x95 = 0 @0x80113D7C.
- **Kollaps** B[8]/B[10] P1 -> P >= 2 ruft kein anim_set (@0x80112F40 / @0x80113420) — dort haelt der Port schon das letzte
  Bild (sm_anim_halten, unveraendert).

### Ursache M1/M2 (Port)
re15_birkin_anim (enemy_ai_common.c) zaehlt anim_frame VOR dem Zeichnen hoch und springt am Clip-Ende auf 0; der Renderer
zeichnet anim_frame NACH dem KI-Tick (main.c: `ncur = npc->anim_frame` -> re15_compute_actor_kf). Wechselt die Phase im
Bild des Clip-Endes und setzt erst das NAECHSTE Bild einen neuen Clip, steht im Wechselbild (Clip, 0) auf dem Schirm —
das Original zeigt dort (Clip, fc-1) (FUN_8001f3bc rechnet die Pose vor dem Hochzaehlen). Betroffen in spiderman_r38.c:
Austritt P2 -> ACTIVE (M1), Griff 1 P1 -> P2 und P5 -> P6, Griff 2 P1 -> P2 und P5 -> P6 (M2). Nicht betroffen: Schleifen-
Clips (Halten P3, Gehen) — dort ist (fc-1, 0, 1, ...) auch im Original die Folge; der Kollaps (haelt schon).

### Umsetzung Nachbesserung 1 (Commit 2029729b)
- `spiderman_r38.c` `sm_anim_halten` (an den Dateianfang verlegt, Kopf mit den FUN_8001f3bc-Adressen): Clip-Ende mit
  Phasenwechsel haelt anim_frame = fc-1 fuer das Wechselbild. Eingesetzt an:
  - M1: Austritt P2 -> ACTIVE (anim_set @0x8011557C, 0x101 @0x80115588-98). Das naechste Bild setzt B[1]/B[2] P0
    +0x95 = 0 / +0x8F = 7 (@0x80112164/@0x80112184 bzw. @0x80112300/@0x80112320) — Ueberblendung aus Bild 34.
  - M2 Griff 1: P0/P1 (`+0x6 += fertig` @0x80112D8C-9C / @0x80113268-78) und P5 -> P6 (P6 ohne anim_set
    @0x80112DA0 / @0x8011327C). P2 setzt `anim_frame = 0`: das Original schreibt in P2 kein +0x95
    (@0x80112B9C-D8 / @0x80113094-D0), es steht auf dem Wrap-Wert 0 (`sb zero,149` @0x8001f63c); der Port haelt fc-1
    nur fuer das Wechselbild. Ohne diese Zeile begaenne der Halte-Clip bei Bild 15 (Mutationsprobe 2).
  - M2 Griff 2: P0/P1 (@0x80114400-10), P2 `anim_frame = 0` (kein +0x95 @0x801142C8-F4), P5 -> P6 (`jr ra` @0x80114484
    ohne `jal`).
  - Kollaps B[8]/B[10]: hielt schon (unveraendert).
- Gleiche Fehlerklasse direkt nach dem Austritt — **B[2] Bruellen** (0x80112294, erste Handlung nach dem Austritt, A[1]
  0x80112024 -> +0x5 = 2; A[2] 0x8011228C = `jr ra`): der alte Port-Fall (enemy_ai_common.c case 2) schaltete schon im
  Fertig-Bild auf +0x5 = 3 und zeigte Clip 2 Bild 0 (Riegel: Wurzel 729 Einheiten, Knochenwinkel bis 1175/4096 = 103 Grad
  gegen Bild 49). Original: P1 @0x80112324 SE 1 bei +0x95 == 0x14 (@0x80112330-44), anim_set @0x80112360 (a2 = +0x7),
  `+0x6 += fertig` @0x80112374-84; P2 @0x80112390 im NAECHSTEN Bild ohne anim_set: +0x7 == 0 -> +0x5 = 3 @0x801123C4,
  +0x6 = 0 @0x801123D4, sonst +0x5 = +0x6 = +0x7 = 0 (@0x801123A0-C0). Neu `re15_r38_sm_bruellen` (spiderman_r38.c),
  Haken in enemy_ai_common.c case 2 (eine Zeile statt vier, "Runde 38 Spur T").
- Keine neue Konstante: fc kommt aus der Bank (re15_birkin_fc), 0 ist der Wrap-Wert @0x8001f63c, alle Clip-/Phasen-
  Werte waren schon belegt.

### Messung nachher (exe, Stand 2029729b, gleiche Laeufe wie vorher)
`wechsel.py` (Scratchpad) listet je Zustandswechsel des Spiderman-Slots den gezeichneten Clip/Bild aus RE15_ANIM_TRACE.
- **ROOM5070 Leon** (Lauf wie "vorher"): anim.log `375 ... 29 35 34 34`, **`376 ... 29 35 34 34`** (vorher `0 0`),
  `377 ... 2 50 1 1`. state.log F376 `[1 t=2b st=1 ss1=1 ss2=0 mo=29 af=34]`. Framedump F374-F377: der Spiderman bleibt
  in allen vier Bildern ueber dem Gitter sichtbar (`T_bilder/nb1_nach_5070_F374-377.png`, vorher F376 leer).
- **ROOM4001 Elza** (Abnahme-Lauf 3.1: Aot 5 betreten, stehen bleiben, `RE15_EXIT_AT=900#4001`), Slot 4:
  F253 `[4/1/2]->[1/1/0]` gezeichnet (0x1D, 34) [vorher F253 Bild 0]; F303 Bruellen fertig (2, 49), F304 P2 -> +0x5 = 3
  weiter (2, 49); F712 Griff hinten P1 -> P2 gezeichnet (0x12, 33) [vorher F711 `mo=18 af=0`]; F713 Halte-Clip
  (0x13, 1); F848 Kollaps-Ende (0x15, 84). 43 Zustandswechsel, im Modul-Code kein Ruecksprung.
- **ROOM5040 Leon**: F320 (0x1D, 34) [vorher `mo=29 af=0`], F370/F371 Bruellen (2, 49), F787 Griff hinten (0x12, 33),
  F788 (0x13, 1).
- Einziger verbliebener Fund des Werkzeugs: F314 (4001) / F389 (5040) `[1/7/1]->[1/3/0]` Clip 4 Bild 0 = Ende von
  B[7] (Alt-Pfad, NICHT Teil von M1/M2) — s. OFFEN 10: B[7] spielt Clip 4 im Original RUECKWAERTS, dort ist Slot 0
  das letzte gezeichnete Bild; die Abweichung des Ports ist die Laufrichtung, nicht das Endbild.

### Riegel und Mutationsproben (unit_r38_spiderman, direkt gestartet)
Der Test rechnet je Bild die gezeichnete Pose nach (`gezeichnet()`: Bank wie re15_actor_anim_select/
re15_actor_uses_loco_bank, re15_compute_actor_kf, re15_skel_compute_pose mit g_anim_pose_actor = Aktor) und prueft:
austritt (Wechselbild Clip 0x1D Bild 34 + Wurzel (0,-940,0) statt Startpose (-1683,1278,0); erstes ACTIVE-Bild mit
Ueberblendung, Abstand der gezeichneten Wurzel zur Austritts-Endpose 6 <= 21; Bruellen Fertig-/P2-Bild Clip 2 Bild 49),
griff1 Lauf A/B (Zugriff-Ende fc-1, Halte-Start Bild 1, Kollaps-Ende fc-1), NEU griff1 Losreissen (Tasten: Loese-Ende
P5 -> P6 und P6-Bild fc-1), griff2 (alle vier Wechselbilder), dazu ein allgemeiner Ruecksprung-Zaehler ueber alle
Modul-Phasen. Ergebnis: `test_r38_spiderman: 0 FAIL`.
- MUTATIONSPROBE 1 (sm_anim_halten ohne Halten): **16 FAIL** — u. a. `Wechselbild ... gezeichnet Clip 0x1D Bild 0`,
  `Ueberblendung startet an der Austritts-Endpose (Abstand 2084 <= 21)`, Ruecksprung-Zeilen mit Groesse: Austritt
  Wurzel 2784 / Winkel 1616/4096, Zugriff 0xE Bild 14 -> 0 Winkel 1892/4096, Kollaps 0x11 Bild 90 -> 0 Winkel 2022/4096,
  Loese-Clip 0x10 Bild 60 -> 0 Winkel 2022/4096, Bruellen Wurzel 729 / Winkel 1175/4096.
- MUTATIONSPROBE 2 (P2 ohne `anim_frame = 0`): **4 FAIL** — `erstes Halte-Bild Clip 0xF Bild 15 (soll 1)`,
  `Griff 2: erstes Halte-Bild Clip 0x19 Bild 5`.
