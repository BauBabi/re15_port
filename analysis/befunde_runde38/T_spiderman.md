# Runde 38 — Spur T "spiderman" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_spiderman`, Zweig `r38/spiderman`, Basis e2cc7673. Angelegt 2026-10-16 16:22.

## Punkte (Wortlaut des Nutzers, AUFTRAG.md)
1. "In Stage 4 die Spiderman kommen nicht richtig aus den Schächten raus wie im Original, obwohl die Schächte korrekt hoch gehen"
2. "In Stage 4 die Spiderman führen - mal wieder - dazu das Leon teilweise komplett von der Karte verschwindet, wenn sie einen Angreifen. Sowas darf GARNICHT passieren, bei keinen Gegner."

## Stand
- 16:22 Dossier angelegt; Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.

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
| 12 | enemy_ai_re2_dog.c:606/1260/1270 | RE2-Hund | re2d_grab_anchor | ok |
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
  hund15, gorilla (11C0 Kampf-Layout), spiderman_vorn/_hinten. Invariante je Griffbild: Raumhuelle + kein Wanddurchgang
  (eigener Slab-Test, nicht der Pruefling). Mutationsprobe Wandwaechter aus: kriecher2_1220 132 Bilder hinter der
  Wand -> FAIL.
- Alt-Riegel angepasst: `integration_r37_beretta` Fall t2b_std_w5 schoss in den Schacht (Spiderman grid 0x41) — jetzt
  der schon draussen stehende Spiderman (Ck(4,0xD6)=1, Sce_em_set @0x017D4, Leon (-23943,-16785)); Absicht (drei
  Kugeln je -10) unveraendert, gruen.

## OFFEN

## Fuer den Nutzer
