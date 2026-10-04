# Runde 35 Spur C — "Zombie Maedchen" (Typ 0x13, EM013)

Auftrag (woertlich, AUFTRAG.md Z.8): "Was sollen Zombie Mädchen sein? Wenn es das gibt, muss es
natürlich mit portiert werden"

Baum: .claude/worktrees/r35_zgirl, Zweig r35/zgirl, Basis 154a73c1 (+ e7e7131c Orchestrator-Hinweis).

## Fuer den Nutzer (5 Zeilen)
1. Das "Zombie-Maedchen" ist Gegnertyp 0x13 = Modell EM013, eine weibliche Zombie-Variante (braunes Haar,
   Kleid); das Original fuehrt sie mit eigener KI-Wurzel, die die normale Zombie-Kampfmaschine mitbenutzt.
2. Sie kommt im ganzen Spiel nur in STAGE 4 vor: ROOM4050 (Labor-Schlaftrakt "Private Rooms", 15 Kameras),
   in zwei Zimmern — hinter Tuer 6 (Kamera 9) und hinter Tuer 7 (Kamera 14), jeweils mit einem Gegenstand.
3. Vorher: KI, Griff, Treffer, Tod waren schon portiert, aber sie erschien NIE — der Port lud bei Tueren, die
   in denselben Raum fuehren, nur in Stage 1 neu; ROOM4050 waehlt seine Gegner gerade beim Neuladen.
4. Jetzt: jede solche Tuer laedt wie im Original neu (alle Stages); sie erscheint an beiden Stellen, laeuft
   an, packt (RE2-KI: 2x -20; RE1.5-KI: -10 dann -5 und Fressen), stirbt, bleibt tot (Kill-Flag 0xA0/0x7D).
5. Keine neuen Sprachdateien, keine neuen Assets (Modell und Toene kommen aus den Original-Daten).
6. Nachbesserung: dieselbe Tuer-Regel brach den Endkampf (ROOM5090) — Birkin starb nach den zwei Wagentueren
   ohne Treffer. Jetzt baut jeder Neuspawn den Boss wie im Original neu auf (600 HP), der Kampf laeuft an.

## Messung vorher

### M1 Zensus aller Sce_em_set (Opcode 0x44) — alle 6 Stages, main + alle subs (2026-10-04)
Werkzeug: `re15_port/tools/r35_zgirl/em_zensus.py` (opcode-exakter Walk ueber `tools/scd_walk_lib.py`,
dieselbe Laengentabelle wie `scd_vm.c`; KEINE Rohbyte-Suche). Layout laut `op_sce_em_set`
(scd_vm.c): pc[1]=Slot, pc[2]=Typ, pc[3]=Verhalten, pc[7]=Kill-Flag, pc[8..13]=x,y,z, pc[16]=dir.
36 RDTs sind 4-Byte-Platzhalter (1270/1271, 20C0-20F1, 30F0/1, 4060/1, 40C0-40F1, 5150-5171,
6050-6071) und tragen kein SCD.

**Typ 0x13 kommt in GENAU EINEM Raum vor: ROOM4050 (+ Zwilling ROOM4051), main00, zwei Records:**
```
ROOM4050 main00 @0x01eb4  44 00 13 00 00 01 00 a0 54 d9 00 00 7e 04 00 00 00 02 00 00
         slot=0 type=0x13 beh=0x00 killflag=0xa0 pos=(-9900,0,1150)  dir=512
ROOM4050 main00 @0x01f5c  44 00 13 00 00 01 00 7d 40 06 00 00 5c 12 00 00 00 04 00 00
         slot=0 type=0x13 beh=0x00 killflag=0x7d pos=(1600,0,4700)   dir=1024
(ROOM4051 byte-gleich an denselben Offsets)
```
Kontext (scd_dump_room.py ROOM4050 main00): beide Records stehen in einem
`Switch(work_vars[0x0A])` @0x01E56 `13 0a d2 01`. work_vars[0x0A] = DAT_800b0fe4 = der
ANGEZEIGTE Cut, vom Raumlader aus dem Tuer-Payload Byte 10 gestempelt (@0x8001d948
`sh v1,4068(at)`, zitiert in scd_room_setup.c:403-410). D.h. main00 waehlt den Gegner nach dem
EINTRITTS-CUT:
| Case (Cut) | Offset | Gegner | Item_aot_set |
|---|---|---|---|
| 6  | 0x1E5A | 0x18 (Zombie-Variante) beh 0x81 kf 0x9f | slot 30 Item 0x24 |
| 9  | 0x1EAE | **0x13** kf 0xa0 (-9900,0,1150) | slot 30 Item 0x26 |
| 11 | 0x1F02 | 0x18 beh 0x0d kf 0x7c | slot 30 Item 0x23 |
| 14 | 0x1F56 | **0x13** kf 0x7d (1600,0,4700) | slot 30 Item 0x25 |
| 4, 5 | 0x1FAA/0x1FEA | kein Gegner | Items 0x23/0x22 |
ROOM4050 ist ein Labyrinth aus 15 Cuts, dessen 14 Door_aot_set (sce 2) fast alle wieder in
ROOM4050 fuehren (Payload `03 05 <cut>` = Stage-Index 3, Raum 5, Ziel-Cut). Zum Zombie-Maedchen
fuehren: Tuer-Slot 6 (rect (-10300,-23200), Ziel-Cut 9) und Tuer-Slot 7 (rect (-17950,-25750),
Ziel-Cut 0x0e).

Typen-Histogramm (Auszug): 0x10 35 Raeume, 0x11 21, 0x16 11, 0x18 13, 0x13 **2 (4050/4051)**.

### M2 Registrierung Typ 0x13 in den Stage-Overlays (re15_reg_scan.py)
0x13 ist in STAGE1..5 registriert (je eine relozierte Kopie), in STAGE6 nicht:
STAGE1 0x8010a8c8, STAGE2 0x8010a75c, STAGE3 0x8010a9b4, **STAGE4 0x8010a87c** (der Raum, in dem
sie vorkommt), STAGE5 0x8010a9fc. Die bisherige Portierung (RE15_ZOMBIEGIRL_AI.md) wurde an der
STAGE1-Kopie (0x8010a8c8) gelesen.

### M3 STAGE4-Kopie == STAGE1-Kopie (relokationsbewusster Wortvergleich, ovl_reloc_diff.py)
* Zombie-Maedchen-Code STAGE1 [0x8010a8c8, +0x1924) gegen STAGE4 [0x8010a87c, +0x1924):
  1609 Woerter: 1551 gleich, 13 j/jal (alle Zieldelta -0x4c), 45 nur-Immediate (44x addiu, 1x lw,
  alle Delta -27352 = -0x6ad8 = Datenrelokation), **0 echte Unterschiede**.
* Geteilte Standard-Zombie-Phasen-Handler STAGE1 [0x80100424, +0xa4a4) gegen STAGE4 [0x801003d8]:
  10537 Woerter, 163 j/jal (-0x4c), 127 Immediate (Daten -0x6ad8, ein Zeiger -0x6a94 = die HP-Tabelle
  0x8011f034, die das Maedchen nicht liest), **0 echte Unterschiede**.
* Tabellen: Zustands-/Modus-/Phasen-Tabellen STAGE1 [0x80120208, +0x4f8) gegen STAGE4 (-0x6ad8):
  196 gleich, 122 Code-Zeiger mit -0x4c, 0 echte; Daten [0x8011f760, +0xa0): 28 gleich, 11 Code -0x4c,
  1 Datenzeiger, 0 echte.
* => Jede STAGE1-Adresse der Portierung gilt fuer STAGE4 mit Code -0x4c / Daten -0x6ad8
  (z.B. Wurzel 0x8010a8c8 -> 0x8010a87c, Zustands-Tabelle 0x80120208 -> 0x80119730).

### M4 Warum sie im Port nicht erscheint (gemessen, Lauf build/r35_zgirl_mess/vorher_tuer6)
`RE15_DEBUG_JUMP=4050@gp` (Sprung kommt mit Cut 0 -> kein Case) + `RE15_FIRE_AOT=6@60#4050`:
```
[aot] DOOR FIRE slot=6 rect=(-9300,-22700,hw=1000,hh=500) target_cut=9 spawn=(-9350,0,-2600)
F400 PL(-9350,-2600,...) cam=9   — 3590 Zeilen state.log, 0 Zeilen mit t=13, kein [spawn-diag]
```
Tuer feuert, Leon wird versetzt, Kamera Cut 9 — aber main00 laeuft NICHT neu, also kein Switch,
kein Spawn. Ursache im Port: aot_common.c (vor dem Fix Z.797-800) setzte
`g_scd_pending_scenario` nur, wenn `0x1000|room<<4|var == Raum` — d.h. NUR fuer Stage 1
("BEWUSST NICHT verallgemeinert: Stage >= 2 ..."). Zensus `selbsttuer_zensus.py`: 67 Selbst-Tueren,
der Port stieg bei 21 neu ein (alle 21 in Stage 1), bei 0 von 46 in Stage 2..6.
Zum Zombie-Maedchen fuehren AUSSCHLIESSLICH Selbst-Tueren: Tueren von aussen nach ROOM4050 kommen
nur aus ROOM4040 (Slot 3 -> Cut 2, Slot 4 -> Cut 0). Deckt sich mit Runde 34 (mess_geg.md M-H3).

## RE-Belege

### R1 Tuer-Warp laedt JEDES Ziel neu, auch den eigenen Raum (PSX.EXE FUN_8001d600, selbst disassembliert)
```
8001d930  lbu  v1,10(a0)          ; Tuer-Payload Byte 10 = Ziel-Cut
8001d940  sb   v1,-1099(at)       ; 0x800afbb5 angeforderter Cut
8001d948  sh   v1,4068(at)        ; 0x800b0fe4 = work_vars[0x0A] = Eintritts-Cut  <- Switch-Variable ROOM4050
8001d94c  lbu  v0,9(a0)           ; Raum
8001d95c  sh   v0,4066(at)        ; 0x800b0fe2 aktueller Raum
8001d960  lbu  v0,8(a0)           ; Stage
8001d968  beq  v1,v0,0x8001d988   ; NUR Stage verglichen (gleich -> kein Overlay-Wechsel 0x80039a30)
8001d988  jal  0x800396fc         ; Raumlader UNBEDINGT
  80039a00  jal 0x8003ef6c        ; darin SCD-Raum-Init (main00 + sub00 neu)
```

### R2 Die STAGE4-KI (dort, wo sie vorkommt) — Kernadressen selbst disassembliert (STAGE4.BIN)
Registrierung 0x80072bac[0x13] = 0x8010a87c (re15_reg_scan.py). Zustands-Tabelle @0x80119730:
[0]=0x8010aae0 INIT, [1]=0x8010b228 ACTIVE, [2]=0x8010bf34 HURT, [3]=0x8010bfc8 DEATH,
[4]=0x80109150, [5]/[6]=0, [7]=0x80109508 (geteilte Leichen-Ruhe). (= STAGE1 0x80120208 -0x4c.)
```
8010a96c  jal  0x8001bd60          ; Engine-Schwerkraft (Spur H), a1 = ori 0x14 im Delay-Slot
8010ab58  ori  v0,zero,0x1 / 8010ab5c sb v0,4(v1)   ; INIT -> +0x4 = 1
8010ab74  sh   v0,444(v1) / 8010ab8c sh v0,446(v1)  ; Steuerziel = Spieler x/z
8010abb4  ori  v0,zero,0x14 / 8010abbc sh v0,156(v1); +0x9c = 20
8010abb8  jal  0x8001af20 (rng) / 8010abc0 andi v0,v0,0x1f / 8010abcc addiu v0,v0,50
8010abd0  sh   v0,154(v1)          ; HP = (rng & 0x1f) + 50 = 50..81
8010ac9c  lw   v0,-13764(v0)       ; DAT_800aca3c
8010aca4  andi v0,v0,0x1 / 8010aca8 beq v0,zero,0x8010afc0   ; Ansprung-Lauerzeit nur bei Bit 0
8010b648  lbu  v0,9(v0) / 8010b650 andi 0xf / 8010b65c addiu at,at,-26792 (=0x80119758) / jalr
                                    ; MODUS-Dispatch ueber +0x9 & 0xf, 16 Eintraege
8010b688  Modus 0: lhu v0,448(a0) / ori v1,0x8001 / andi 0x9fff / bne  (+0x1c0-Unterbrechung)
          danach DECIDE @0x8011978c[+0x5] (8010b6dc) und ANIMATE @0x801197d0[+0x5] (8010b710)
```
Modus-Tabelle @0x80119758: [0]=0x8010b688 (Phasen-FSM des Standard-Zombies), [1]=0x8010b738
(Tabellen 0x80119814/0x80119830), [2]=0x8010b7b4, [3]=0x8010b830, [4]=0x8010b8ac, [5]/[6]=0x8010b928,
[7]/[8]=0x8010b9a4, [9]/[10]=0x8010ba20, [11]=0x8010ba9c, [12]=0x8010bb18, [13..15] zeigen in
geteilten Code (0x80101b18/0x80101d98/0x8010200c; ohne Spawn-Weg).

### R3 Welcher Modus ist erreichbar? (Spawn-Byte + Laufzeit-Schreiber von +0x9)
* Beide Spawns tragen Verhalten pc[3] = 0x00 -> +0x9 = 0 -> **Modus 0**.
* Laufzeit-Schreiber von +0x9 in [0x801003d8, 0x8010c1a0) (STAGE4, eigener Scan): nur 0, &0x7f, |0x80
  und **0x81** an 0x80104ee0 / 0x80105088 / 0x80108904 / 0x8010a140 (STAGE1 +0x4c). Zugeordnet:
  0x80105088 = Kriechtor-Uebergabe FUN_80104f80 (Zeile [0x10], nur per ROOM1030-Skript);
  0x80108904 = Todeszeile (Waffe 8, Richtung 1) FUN_80107ee0 = **Beine ab -> Kriecher** (Fall 2:
  +0x9=0x81, +0x4=1, HP=0x1e; Decompilat STAGE1_full/FUN_80107ee0.c); 0x8010a140 = Zeile [0xf]
  FUN_80109e4c; 0x80104ee0 = Modus-12-ANIMATE FUN_80104b40. Damit wird das Maedchen im Original nur
  ueber den Kriecher-Tod (Waffe 8 aus Richtung 1) zu Modus 1. Der Port fuehrt den RE1.5-Tod
  zombieweit ueber das vereinfachte Steh-/Liege-Modell (re15_enemy_ai_live_death, keine Todeszeilen-
  Tabelle) — siehe OFFEN O2.
* Todes-/Treffer-Haupttabellen: Maedchen @0x8012063c/@0x8012039c == Standard @0x8011feac/@0x8011fb90
  Wort fuer Wort, AUSSER Waffe 1 (Messer) Richtung 0/1 = 0x00000000 bei beiden Tabellen des Maedchens
  (eigener Dump). Messer gegen das Maedchen = `jalr 0` im Original (unfertig) -> Beta->Retail: RE2
  behandelt sie als gewoehnlichen Zombie (Lader klemmt 0x10..0x1F auf eine Gruppe), der Port nimmt die
  Standard-Zeile (Zurueckzucken). Gemessen in T5 (Tests).

### R4 Ansprung ("Lunge") ist im Original fuer sie AUS
* Gate @0x8010aca4 (oben): nur bei DAT_800aca3c & 1.
* Der Raumlader loescht die unteren 16 Bit bei jedem Laden (PSX.EXE): `80039710 lui v1,0xffff` /
  `80039728 and v0,v0,v1` / `80039730 sw v0,-13764(at)` (0x800aca3c).
* Kein `ori ...,1`-Schreiber auf 0x800aca3c in EXE und STAGE4 (eigener Scan, 38 + 11 Stores, alle
  ori 0x40/0x80/0xc0/0x2000/0x4000/0x8000 oder and). Gesetzt wird Bit 0 nur per SCD `Set(1,31,1)`
  (`22 01 1f 01`); Zensus aller RDTs: NUR ROOM5120/5121 main00 @0x01402 und ROOM5140/5141 main00
  @0x01a5a — nicht ROOM4050. => Im einzigen Raum des Maedchens ist der Ansprung nie scharf. Der Port
  traegt die Spur (enemy_ai_common.c ACTIVE, `ai_flags & 0x100`) und schaltet sie nie scharf
  (s_live_combat_active = 0) — deckungsgleich. Gepinnt in T4.

### R5 Schwerkraft FUN_8001bd60 in ROOM4050 ohne Wirkung (gemessen am RDT)
Das Maedchen ruft sie @0x8010a96c (STAGE4). Sie faellt nur in Zellen mit Attribut-Bit 0x2 (Absturzkante,
Spur H: `r & 2` @0x8001bdb0-b4). ROOM4050-SCA (RDT+0x20): 5 x 82 = 410 Zellen, Attributwort
(u1 | floor<<8) bei ALLEN 410 = 0x0300 -> Bit 0x2 nirgends gesetzt, Band 0 ueberall. Die Routine kann
dort weder einen Fall starten noch +0x1c0 setzen (also auch die Modus-0-Unterbrechung
`(+0x1c0 & 0x9fff) == 0x8001` @0x8010b698-a4 nie ausloesen). Die Port-Funktion baut Spur H
(re15_schwerkraft_8001bd60, Zweig r35/raeume, Feld e->fall_1c0 in re15_actor.h) — siehe
"Zusammenfuehrung".

## Umsetzung

### U1 Selbst-Tueren in allen Stages (aot_common.c, 1 Zeile + Kommentar)
`g_scd_pending_scenario = (int)d->target_cut;` jetzt unbedingt. An dieser Stelle kommt nur an, wessen
`dest_id = ((dest_stage+1)<<12)|(room<<4)|var` == aktueller Raum ist (Kreuz-Raum-Tueren kehren vorher
zurueck) — das ist exakt der Original-Zweig @0x8001d968/@0x8001d988. Stage 1 unveraendert (alle 21
Selbst-Tueren erfuellten die alte Bedingung), neu: 46 Selbst-Tueren in Stage 2..6 (u.a. ROOM4050
Slots 1-3, 5-13). Commit 8ec202f9.

## Messung nachher

### N1 ROOM4050 Tuer 6 -> Cut 9 (Lauf build/r35_zgirl_mess/nachher_tuer6, Default-KI = RE2)
```
[aot] DOOR FIRE slot=6 ... target_cut=9 spawn=(-9350,0,-2600)
[spawn-diag] Sce_em_set type=0x13 behavior=0x00 slot=0 pos=(-9900,0,1150) dir=512
F61  [1 t=13 st=1 ss1=0 ... @(-9900,1150,r512)] hp=98
F231 [1 t=13 st=1 ss1=3 ss2=3 mo=12 d=834]  (Griff)     Spieler-HP 100 -> 80 (F246) -> 60 (F291)
F336 [1 t=13 st=1 ss1=5 g=80 mo=2]           (zurueckgestossen/liegt), F484 wieder ss1=1 Anlauf
```
540 Bilder mit dem Zombie-Maedchen in state.log. Sie erscheint jetzt.

### N2 ECHTER Weg: Aktionstaste an der Tuer (kein FIRE_AOT), beide Spawn-Orte
Werkzeug `tools/r35_zgirl/lauf.sh` (exe-Kopie re15_pc_zgirl.exe). Leon per `RE15_PLAYER_POS` vor die
Tuer gestellt, Blick so, dass der Vorwaertspunkt (620 vor dem Spieler, `ori 0x26c` @0x80042bd0,
aot_common.c) in der Tuerflaeche liegt; `RE15_PRESS=square@60,square@62`:
* Tuer 6: POS (-9300,-23320) rot 3072 ->
  `[aot] DOOR FIRE slot=6 ... target_cut=9` -> `[spawn-diag] Sce_em_set type=0x13 pos=(-9900,0,1150)`,
  241 Bilder mit t=13 (Lauf echt_tuer6).
* Tuer 7: POS (-16830,-24750) rot 2048 ->
  `[aot] DOOR FIRE slot=7 rect=(-17450,-24750,hw=500,hh=1000) target_cut=14` ->
  `[spawn-diag] Sce_em_set type=0x13 pos=(1600,0,4700) dir=1024`, 241 Bilder (Lauf echt_tuer7).

### N3 Modell / Bild (RE15_FRAMEDUMP, nicht gdigrab)
debug.log: `[enemy] RE2 EM013 loaded: 17 meshes, 15 bones, 31 clips` und
`Hybrid EM13: RE1.5-Geometrie (15 Meshes) unter RE2-Rig (15 Bones, 31 Clips), 0 Kanten ohne
Zuordnung` (Default-KI RE2 = Hybrid-Rig; enemy_dbg.log `ZEICHNE Typ 0x13: 15 Teile`).
Framedump Bild 150/210/240/300 (Lauf bild_tuer6): Cut 9 = Schlafraum mit Etagenbett und
Umbrella-Schild; die Zombie-Frau (braunes Haar, gemustertes Kleid) laeuft auf Leon zu und packt ihn.
⚠ Messumgebung: mit dem Standard-Renderer (Direct3D) lieferte SDL_RenderReadPixels in dieser
Sitzung `GetRenderTargetData(): DEVICELOST` (nur die Vorspann-Bilder kamen an); mit
`SDL_RENDER_DRIVER=opengl` (weiterhin beschleunigt, kein SOFTWARE_RENDER) kommen alle Bilder.

### N4 KI in beiden Geschmaeckern am echten Tuerweg (exe, Tuer 6 -> Cut 9)
* RE2-KI (Spiel-Default), Lauf nachher_tuer6: Anlauf ab F61 (d 3788), Griff F231 (ss1=3), Spieler-HP
  100 -> 80 (F246) -> 60 (F291), danach weggestossen (ss1=5, g=0x80) und neuer Anlauf ab F484.
* RE1.5-KI (`RE15_AI_FLAVOR=re15`), Lauf re15_tuer6: Spawn-HP 80 (50..81), ss1 0 -> 2 (engage, F70)
  -> 3 (Griff, F198) -> Spieler-HP 100 -> 90 (Aufprall -10, F212) -> 75 (Bisse) -> ss1=5 (Fressen,
  F313) -> Spieler-HP -1 (F378). Das ist der geteilte Zombie-Griff FUN_80102548 (ungemasht -> Fressen).

### N5 Tonereignisse (SDL_AUDIODRIVER=dummy, RE15_SE_DEBUG, Tuer 6; diese Sitzung hat kein Audiogeraet)
ROOM4050 bringt eine eigene Raum-Tonbank mit (RDT snd1: VH @0x5430 `pBAV`, 2 Programme, 22 Toene,
15 VAGs; ROOM1140 zum Vergleich 11 VAGs). Die Gegner-Toene laufen ueber re15_audio_room_se(N) ->
Raumbank (FUN_800453d0) und die Clip-Bild-Flags (FUN_8001b38c, Zombie-Wurzel-Schwanz @0x8010aad8
STAGE1 / 0x8010aa8c STAGE4). Gemessen im Raum (nach dem Sprung):
* RE2-KI: se 0 x4, 5 x2, 3 x2, 8 x1, 1 x1.
* RE1.5-KI: se 1 x8 (Schritte, Bit 1 der Bildflags), 0 x3, 4 x2, 3 x1.
Datengetrieben aus dem Original-RDT; kein eigener Ton noetig.


## Tests

Neue Dateien (nur aus `tests/unit/probes/r35_zgirl.cmake` registriert):
`tests/unit/test_r35_zgirl.c`, `tests/integration/test_r35_zgirl.cmake`.

| Test | misst | Ergebnis |
|---|---|---|
| unit_r35_zgirl_zensus | echtes ROOM4050 UND ROOM4051, main00 je Eintritts-Cut 0..14: Cut 9 -> 0x13 @(-9900,1150) r512 Kill-Flag 0xa0, Cut 14 -> @(1600,4700) r1024 0x7d, Cut 6/11 -> 0x18, sonst nichts | gruen (30/30) |
| unit_r35_zgirl_killflag | Zone 8 (Stage-Index 3) und Kill-Flags 0xa0/0x7d unterdruecken den Spawn | gruen |
| unit_r35_zgirl_tuer | Slot 6/7 -> g_scd_pending_scenario 9/14, Leon am Tuer-Ziel, Verbraucher scd_room_reenter spawnt sie; Slot 0 -> ROOM4040 ohne Szenario; Stage-1-Gegenprobe ROOM1110 Slot 1 -> 7 | gruen |
| unit_r35_zgirl_ki_re15 | echter Spielschritt (scd_vm_tick + re15_game_step, RE1.5-Bank 0x13 aus EMD/CDEMD0.EMS): INIT-HP 80 in 50..81, Abstand 3788 -> 337, Griff ab Bild 173, Abzuege 10 dann 5, Ansprung nie scharf | gruen |
| unit_r35_zgirl_ki_re2 | dito RE2-KI (RE2-Bank aus RE2/CDEMD0.EMS): Abstand 3788 -> 386, Griff ab Bild 155, Spieler-HP 100 -> 60 (2 x 20) | gruen |
| unit_r35_zgirl_tod | toedlich -> Zustand 3 -> Leiche 7 -> Flag (8,0xa0)=1 -> Wiedereintritt Cut 9 ohne Maedchen | gruen |
| unit_r35_zgirl_messer | re15_player_weapon_fire(1): Zustand 2, +0x5 = 1, HP 60 -> 54, Rueckkehr in Zustand 1 | gruen |
| unit_r35_zgirl_selbsttueren | alle Raeume: 60 Selbst-Tueren aus main00 (Stage 1: 14, Stage 2..6: 46) -> Szenario = Ziel-Cut, Wiedereintritt + 60 SCD-Bilder | gruen (60/60) |
| integration_r35_zgirl | echte exe, Aktionstaste an Tuer 6 und Tuer 7: DOOR FIRE, `Sce_em_set type=0x13` an beiden Lagen, >= 300 Bilder mit ihr, Annaeherung >= 1500, Tuer 6 zusaetzlich Spieler-HP < 100 | gruen (253 s) |
| unit_r35_zgirl_wiedereintritt (N1) | echter Spielschritt durch die Selbst-Tuer: ROOM5090 Boss nach Tuer 2 HP 600, Kampfstart ohne Routine 3; Spawn-Zaehler je Eintritt (ROOM5090 2, ROOM4050 Tuer 6 sechsmal je 1) | gruen; Gegenprobe ohne Fixes rot (4 Fehler) |
| integration_r35_zgirl_5090 (N1) | echte exe, Aktionstaste an Tuer 2 in ROOM5090, dann geradeaus: zweiter Sce_em_set 0x36, Boss-HP 600 am Ende, nie < 0, Boss laeuft an (x max >= -7500) | gruen (143 s) |

Der bestehende unit_zgirl_ai (synthetische FSM-Teile 1-6) bleibt unveraendert gruen.
Suite vor T8: `=== LOCAL-BUILD-OK (all) — Tests 486/486` (1370 s).
**Endstand (mit T8): `=== LOCAL-BUILD-OK (all) — Tests 487/487`** (Schranke 478; keine Flatterer).
Gegenprobe ohne Fix: der exe-Lauf vorher_tuer6 (M4) — dieselbe Tuer, kein Spawn; die alte Bedingung
`0x1000|room<<4|var == Raum` kann fuer 0x4050 nie wahr werden, T3/T8 waeren dort rot.


## OFFEN

* **O1 Schwerkraft-Aufruf (Zusammenfuehrung mit Spur H).** Das Original ruft in der Maedchen-Wurzel
  FUN_8001bd60(-10, 0x14) (STAGE4: `addiu a0,zero,-10` @0x8010a934, `jal 0x8001bd60` @0x8010a96c,
  `ori a1,zero,0x14` @0x8010a970; STAGE1 @0x8010a9b8). Die Port-Funktion baut Spur H
  (re15_schwerkraft_8001bd60 in trage_1200.c, Feld e->fall_1c0 in re15_actor.h, Zweig r35/raeume) — hier
  NICHT doppelt gebaut. Wirkung in ROOM4050 gemessen = keine (R5: alle 410 SCA-Woerter 0x0300, kein
  Absturzkanten-Bit). Nach dem Zusammenfuehren in re15_zgirl_ai_tick (enemy_ai_common.c, vor
  re15_nav_update_steer) eine Zeile: `re15_schwerkraft_8001bd60(e, -10, 0x14, (int32_t)e->hit_radius_min);`
  und in FUN_8010b688-Nachbau (Modus 0) die Unterbrechung `(fall_1c0 & 0x9fff) == 0x8001 -> +0x5=9,+0x6=0`
  (@0x8010b698-bc STAGE4) scharf schalten. Naechster Messweg: unit_r35_zgirl_ki_re15 erneut (darf sich in
  ROOM4050 nicht aendern).
* **O2 Todeszeilen-Tabelle (zombieweit, nicht maedchenspezifisch).** Der RE1.5-Tod des Ports ist das
  vereinfachte Steh-/Liege-Modell (re15_enemy_ai_live_death); die Haupttabelle @0x8012063c (STAGE4
  0x80119b64) [Waffe*0x20 + Richtung*4] mit Kopf-ab/Beine-ab (z.B. Waffe 8 Richtung 1 = FUN_80107ee0,
  Fall 2: +0x9=0x81 @0x80108950, HP 0x1e) ist fuer ALLE Zombies nicht portiert. Erst damit wird der
  Maedchen-Modus 1 (Kriecher, FUN_8010b738 STAGE4, Tabellen 0x80119814/0x80119830) erreichbar. Im
  Default-KI-Modus (RE2) uebernimmt der RE2-Zerleger das Beine-ab/Kriechen (Familie re15_re2z_owns_type
  inkl. 0x13). Eigene Runde (betrifft Spur A "abplatzende Beine/Arme").
* **O3 Selbst-Tueren Stage 2..6 im Spiel.** 46 Tueren steigen jetzt neu ein (byte-true @0x8001d988).
  T8 (unit_r35_zgirl_selbsttueren) faehrt ALLE 60 Selbst-Tueren, die main00 beim Cut-0-Eintritt setzt
  (Stage 1: 14, Stage 2..6: alle 46 aus dem Zensus), durch: Szenario = Ziel-Cut, kein Raumwechsel,
  Wiedereintritt + 60 SCD-Bilder ohne Absturz. Am Bildschirm einzeln gesehen sind nur die zwei
  ROOM4050-Tueren 6/7; die uebrigen 44 Stage-2..6-Tueren sind nicht am Bild abgenommen (Stage 2+ ist nicht
  der aktuelle Spielweg). Naechster Messweg bei einem Befund: RE15_DEBUG_JUMP + RE15_FIRE_AOT wie lauf.sh.
  Nachbesserung 1: T8 tickte nur das SCD und war damit blind fuer den Endkampf-Bruch (M1). Der Zensus
  M2a (Abschnitt Nachbesserung 1) prueft jetzt alle Port-Zustaende der 46 Raeume: ausser dem G5-Modul und
  dem Spawn-Zaehler (beide behoben) uebersteht alles eine Selbst-Tuer wie einen Raumwechsel; T9 faehrt den
  echten Spielschritt (ROOM5090, ROOM4050), integration_r35_zgirl_5090 die echte exe.

## Zusammenfuehrung
* aot_common.c: eine Zeile (+4 Kommentarzeilen) im Selbst-Tuer-Zweig von aot_fire_door, Kommentar
  "Runde 35 Spur C".
* Abhaengigkeit Spur H (O1): nur ein Aufruf nach dem Merge, keine Code-Kopie.
* Nachbesserung 1: enemy_ai_boss_g5.c neue Funktion re15_g5_boss_spawn (vor re15_g5_boss_tick);
  enemy_ai_common.c re15_enemy_spawn_root G5-Zweig +4 Zeilen; scd_room_setup.c scd_room_reenter +5 Zeilen
  (Spawn-Zaehler-Reset hinter re15_re2z_rng_reset, Kommentar "Runde 35 Spur C"); aot_common.c nur
  Kommentar. Beruehrung mit Spur I (entladen): beide aendern Raumlade-Pfade — der Zaehler-Reset sitzt in
  scd_room_reenter (gilt fuer alle drei Ladewege), nicht in room_common.c.
* Neue Dateien: tools/r35_zgirl/{em_zensus.py, selbsttuer_zensus.py, ovl_reloc_diff.py, lauf.sh},
  tests/unit/test_r35_zgirl.c, tests/unit/probes/r35_zgirl.cmake, tests/integration/test_r35_zgirl.cmake,
  analysis/befunde_runde35/C_zgirl_bilder/*.png. Keine Assets fuer das Paket-/Android-Gate.

## Nachbesserung 1 (2026-10-04, nach Abnahme 0 = C_abnahme_0.md: M1 blockierend, M2, M3)

Stand vor der Nachbesserung: HEAD 43396082 (Code = 0ab1e6f5). Der Fix U1 (aot_common.c) bleibt —
er ist byte-true (@0x8001d988) und traegt das Zombie-Maedchen. Nachgebessert wird, was er in
ROOM5090 freilegt (M1), die Absicherung der uebrigen neu einsteigenden Raeume (M2) und der
Kommentar (M3).

Betroffene Raeume (selbsttuer_zensus.py, `port_neu=0` vor U1): ROOM2040/2041 (Slots 2,3),
20A0/20A1 (1), 30E0 (2), 4000 (7), 4050/4051 (1-3, 5-13), 40A0/40A1 (10), 5090/5091 (0,2,3,4),
6030/6031 (0,1).

### M1 — Endkampf ROOM5090: Boss-HP 0 nach Selbst-Tuer (behoben)

**Ursache (gelesen):** op_sce_em_set setzt `a->hp = 0` (scd_vm.c, Sce_em_set traegt keine HP). Den
G5 baut das Port-Modul enemy_ai_boss_g5.c; sein Konstruktor (HP 600) lief nur bei
`s_g5_slot != slot || !g->aktiv`. Der zweite Spawn nach einer Selbst-Tuer landet im selben Slot 2
bei aktivem Modul -> kein Konstruktor -> HP 0 -> Todes-Trigger `e->hp <= 0` beim Kampfstart.
Das Modul fuehrt +0x4 nicht im Aktor: state.log zeigt fuer den Boss durchgehend `st=0`.

**Messung vorher (Abnahme-Laeufe, scratchpad/c_mess, eigene Auswertung g5_log.py):**
```
o3_5090_kampf (U1):        F101 hp=600 -> F201 (nach FIRE_AOT 2) hp=0 -> F408 cam=15 mo=6 hp=-1, mo 7/8/10, x bleibt -9000
basis_o3_5090_kampf:       F201 hp=600 -> F408 mo=1 hp=600, F587 x=-3891, F679 mo=3 x=-1986 (Boss laeuft an)
o3_5090_echt (U1):         Boss hp=0 im ganzen Lauf;  basis_o3_5090_echt: hp=600 ab F100
```

**RE-Beleg (selbst disassembliert):**
```
PSX.EXE  Sce_em_set 800421e0  sw   zero,4(s0)          04 00 00 ae   ; +0x4..+0x7 = 0 am neuen Entity
RE2 EM36 (CDEMD0.EMS @0x6C0000, 23476 B, md5 b57f8315dc8a = Dossier Runde 6)
         Main       80100164  lbu  v0,4(s3)                         ; Routine +0x4
                    80100178  lw   v0,21964(at)                     ; Tabelle @0x801055CC
         Tabelle    801055cc  {801003cc, 80100784, 801025bc, 80102bbc, 80103834, 0, 0, 80103878}
         Ctor       801003d8  addiu v0,zero,1 / 801003e8 sw v0,4(s0)   04 00 02 ae ; +0x4 = 1
                    801003fc  addiu v0,zero,600            58 02 02 24
                    80100400  sh   v0,342(s0)              56 01 02 a6   ; HP 600 (easy-Bit -> 400 @0x80100414)
```
=> Jeder Sce_em_set des G5 fuehrt im naechsten Tick durch den Konstruktor (Routine 0), auch der
zweite im selben Raum. Das Original laedt bei der Selbst-Tuer neu (@0x8001d988), sub00 @0x0124A spawnt
ihn erneut -> volle HP.

**Aenderung:** enemy_ai_boss_g5.c neue Funktion `re15_g5_boss_spawn(slot)` setzt `s_g5.aktiv = 0`
(= +0x4 = 0, Konstruktor faellig); gerufen aus dem vorhandenen Spawn-Haken
`re15_enemy_spawn_root` (enemy_ai_common.c, G5-Zweig, +4 Zeilen), der nach jedem Sce_em_set laeuft.
Der Konstruktor selbst ist unveraendert (Tentakel-Reset, HP 600, Intro-Zustand sub 2).
Nebenwirkung, gleiche Regel: auch Tod + CONTINUE in ROOM5090 (gleicher Slot, Modul aktiv) baut den
Boss jetzt neu auf.

**Messung nachher (dieselben zwei Laeufe, identische Eingabe, eigene exe-Kopie
`scratchpad/c_nb1_lauf.sh`, Laufordner `scratchpad/c_mess/nb1_echt` / `nb1_kampf`):**
```
nb1_kampf: [fire-aot] slot=2 at F200 -> DOOR FIRE slot=2 target_cut=14 -> 2. Sce_em_set type=0x36
           F201 hp=600; F408 cam=15 mo=1 x=-9000 hp=600; F587 x=-3891; F679 mo=3 x=-1986;
           F1001 x=856; F1060 x=1960 hp=600 (Boss laeuft an, kein Todesclip)
nb1_echt:  DOOR FIRE slot=0 target_cut=9 -> 2. Sce_em_set type=0x36, Boss hp=600 ab F100 bis F1700
Boss-Eintrag (Zustand, Clip, Lage, HP) Bild fuer Bild gegen die Basis-exe ohne U1:
           nb1_kampf vs basis_o3_5090_kampf: 1000 Bilder, 0 verschieden
           nb1_echt  vs basis_o3_5090_echt:   400 Bilder, 0 verschieden
```
Bild (RE15_FRAMEDUMP, opengl, kein SOFTWARE_RENDER): `C_zgirl_bilder/nb1_5090_kampf_F500-1100.png`
— der G5 kommt im Suedwagen auf Leon zu (F500 hinter ihm, F700 an ihm, F900/F1100 im Gang).
Gegenprobe (Fixes 1 und 2 aus dem Arbeitsbaum genommen, gebaut, T9 gefahren): `Boss HP 0`,
`Routine 3 gesehen 1, HP -1`, Spawn-Zaehler 4 bzw. 5..10 -> T9 rot; mit den Fixes gruen.

### M2 — Zensus der neu einsteigenden Raeume (a) und exe-Pin ROOM5090 (b)

**(a) Methode.** Der Raumwechsel-Pfad (room_common.c re15_room_apply_pending) und der
Selbst-Tuer-Pfad (game_step_common.c, Same-Room-Reenter) rufen beide scd_room_reenter
(= SCD-Raum-Init FUN_8003ef6c). Was NUR der Raumwechsel-Pfad zuruecksetzt, ueberlebt eine Selbst-Tuer —
das ist die Kandidatenmenge. Dazu je Raum die gespawnten Typen (neues Werkzeug
`re15_port/tools/r35_zgirl/reentry_zensus.py`, opcode-exakt) und die raumgebundenen Port-Haken
(grep auf die Raum-Ids im Engine-Code).

Gespawnte Typen (reentry_zensus.py): 2040/2041 keine; 20A0/20A1 0x25 (Adult-Spinne, 4 Records);
30E0 0x10, 0x11 (sub00), NPC 0x40/0x42 (sub11); 4000 0x29 (Kakerlake), NPC 0x40/0x42; 4050/4051 0x13,
0x18; 40A0/40A1 0x18; 5090 0x30 (-> 0x36 G5, sub00 @0x0124a) + NPC 0x4d; 5091 0x30; 6030/6031 NPC
0x40/0x49/0x4b/0x4d. Kein Raum hat Obj_model_set.

Nur im Raumwechsel-Pfad (room_common.c) zurueckgesetzt — Bewertung fuer die Selbst-Tuer:
| Zustand | Bewertung |
|---|---|
| G5-Modul s_g5/s_g5_slot + Tentakel s_tent_bereit (5090/5091) | **brach (M1)**, jetzt ueber re15_g5_boss_spawn je Spawn |
| Spawn-Zaehler DAT_800aca4e (re15_enemy_reset) | **brach**: Original nullt ihn in FUN_8003ef6c @0x8003f014 `sb zero,-13746(at)` (an @0x80039a00, also auch nach Selbst-Tuer). Leser: geteilter Zombie-Engage @0x801022c4 und Treffer-Rueckkehr @0x80105ea4 (STAGE1; `lbu v0,-13746(v0)` / `sltiu v0,v0,0x5`; STAGE4 -0x4c). In ROOM4050 spawnt jeder Eintritt <= 1 Gegner; der Port zaehlte ueber Selbst-Tueren weiter und waehlte ab 5 die andere Verhaltenstabelle. Jetzt in scd_room_reenter (alle Ladewege). |
| Kraehen-Schwarm 0x800aca50 (re15_crow_flock_reset) | in den 46 Raeumen kein Typ 0x21 -> ohne Wirkung (Stage-1 ROOM1170 unveraendert) |
| Opfer-Zustand des Spielers (re15_player_victim_reset), Treppe, Klettern | nicht erreichbar: Griff/Treppe/Klettern/Tod setzen `g_aot_action_pressed = 0` (game_step_common.c), Tueren feuern nur auf Aktion |
| savepoint/itembox-Anforderung | wird am Anfang des NAECHSTEN Bilds vor dem Spielschritt verbraucht (main.c `re15_savepoint_pending()` / `re15_itembox_pending()` -> set_pending(0)); eine Selbst-Tuer kann sie nicht ueberspannen |
| g_re15_pauseflags | auch im Selbst-Tuer-Pfad geloescht: re15_room_transition_present (room_common.c, `re15_pauseflags_clear()` = @0x8001ca44/@0x8001caec) |
| Modelle/RBJ/Licht/Nachrichten/Bank/BGM | gleicher Raum = gleiche Daten |
| Kollisionsband | setzt aot_fire_door selbst aus spawn_y (re15_collision_set_band) |

Raumgebundene Port-Haken in den 46 Raeumen (grep auf die Raum-Ids, engine/src + main.c):
| Raum | Haken | Zustand | Wiedereintritt |
|---|---|---|---|
| 5090/5091 | enemy_ai_boss_g5.c + enemy_ai_tentakel_g5.c (Dispatch enemy_ai_common.c `t == 0x36 && (room & 0xFFFE) == 0x5090`) | s_g5, s_g5_slot, s_tent*, s_devour_ph | war kaputt (M1), jetzt Konstruktor je Spawn |
| 5090/5091 | scd_vm.c Umtypung 0x30 -> 0x36, sub02-Riegel; re15_damage.c Waffen-/Granatenzeile, RE2-Trefferbox; main.c RE2-Modell-Lader | keiner (Funktion von Raum + Typ) | unveraendert |
| 20A0/20A1 | scd_room_setup.c Ereignis 2 (Effektschleife sub02) | Thread in g_scd | scd_room_reenter wischt g_scd und feuert neu — wie beim Betreten |
| 6030/6031 | re15_itembox.c Raum-Tabelle | keiner | unveraendert |
| 2040, 30E0, 4000, 4050, 40A0 | keine raumgebundenen Haken | — | — |

Typgebundene KI der gespawnten Typen: Zombies 0x10/0x11/0x13/0x18, Adult-Spinne 0x25, Kakerlake 0x29,
NPC 0x40..0x4d setzen ihren Zustand im INIT des Aktors (`e->state == 0` aus Sce_em_set, scd_vm.c
`a->state = 0`); scd_room_reenter nullt die Aktoren vorher. Die Modul-Woerter der RE2-KI
(g_re2_room_gflags inkl. Spinnen-Mutex 0x20, PRNG, One-Save-Latch) setzt re15_re2z_rng_reset in
scd_room_reenter zurueck (auch Selbst-Tuer). Die per-Slot-Tabellen in enemy_ai_common.c (s_wander_*,
s_gait_variant, s_zfoot_*, s_los_*) verhalten sich in beiden Pfaden gleich (auch der Raumwechsel nullt
sie nicht; der INIT/Engage schreibt sie). Ergebnis: ausser G5 und Spawn-Zaehler kein Zustand, der eine
Selbst-Tuer anders uebersteht als einen Raumwechsel.

**(b) Pins.**
* `unit_r35_zgirl_wiedereintritt` (T9, test_r35_zgirl.c): echter Spielschritt (scd_vm_tick +
  re15_game_step mit KI) durch die Selbst-Tuer. ROOM5090: Erstbetritt Boss HP 600, Spawns 2; Tuer 2 ->
  Szenario 14, Verbraucher in game_step_common.c steigt neu ein, Spawns dieses Eintritts 2; Boss HP 600;
  Kampfstart (grid 0x13 wie sub04 Member_set @0x130A) 120 Bilder ohne Routine 3, HP 600. ROOM4050 Tuer 6
  sechsmal: Spawn-Zaehler je 1. Gegenprobe ohne Fixes rot (s. M1).
* `integration_r35_zgirl_5090` (tests/integration/test_r35_zgirl_5090.cmake, echte exe, eigener
  exe-Name): Leon vor Tuer 2 (500,-9175) rot 2048, Aktionstaste per Eingabeskript `A0.1,W1,U12` ab
  Bild 100, dann geradeaus bis zum Kampfstart; prueft DOOR FIRE slot=2 -> Cut 14, zweiten
  Sce_em_set type=0x36 nach der Tuer, Boss-HP am Ende 600, nie < 0, Boss x max >= -7500 (Start -9000).
  Gemessen im Suite-Lauf (143 s, gruen; build/tests/integration/r35_zgirl_5090_wd): `DOOR FIRE slot=2
  ... target_cut=14` per Aktionstaste -> 2. `Sce_em_set type=0x36`; F100 hp=600 (cam 14), F281 Kampfstart
  (cam 15, mo=1, x=-9000), F451 x=-4046, F552 mo=3 x=-1986, hp=600 bis F900.

### M3 — Kommentar aot_common.c (behoben)
Der Satz "BEWUSST NICHT verallgemeinert ... eigene Runde mit eigener Messung" ist ersetzt: Runde 30
liess Stage >= 2 aus und verlangte eine Messung; Runde 35 Spur C hat sie gemacht, jede Selbst-Tuer
steigt neu ein, und die Messung fand die zwei Zustaende (G5-Konstruktor, Spawn-Zaehler), die jetzt auch
der Wiedereintritt zuruecksetzt.

### Nachbesserung 1 — Suite
`=== LOCAL-BUILD-OK (all) — Tests 489/489` (1519 s; 487 + unit_r35_zgirl_wiedereintritt +
integration_r35_zgirl_5090; keine Flatterer, Schranke 478). Die Spawn-Zaehler-Regel aendert auch die
21 Stage-1-Selbst-Tueren (byte-true @0x8003f014); kein bestehender Pin hat sich bewegt.

### Nachbesserung 1 — OFFEN
* O1/O2 wie oben (Schwerkraft-Aufruf nach Merge mit Spur H; Todeszeilen-Tabelle zombieweit).
* Kraehen-Schwarm 0x800aca50: das Original nullt ihn ebenfalls in der Raum-Init (FUN_8003ecec
  @0x8003ed84 `sh zero,-13744(at)`), der Port nur im Raumwechsel (re15_enemy_reset). In den 46 Raeumen
  ohne Wirkung (kein Typ 0x21); betrifft nur den Stage-1-Wiedereintritt ROOM1170 (Intro-Kraehen), der
  vor Runde 35 schon so lief. Naechster Messweg: probe/Integration des 1170-Intros mit Reset in
  scd_room_reenter vergleichen, bevor er dorthin wandert.
