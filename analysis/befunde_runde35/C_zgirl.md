# Runde 35 Spur C — "Zombie Maedchen" (Typ 0x13, EM013)

Auftrag (woertlich, AUFTRAG.md Z.8): "Was sollen Zombie Mädchen sein? Wenn es das gibt, muss es
natürlich mit portiert werden"

Baum: .claude/worktrees/r35_zgirl, Zweig r35/zgirl, Basis 154a73c1 (+ e7e7131c Orchestrator-Hinweis).

## Fuer den Nutzer (5 Zeilen)
(wird am Ende gefuellt)

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

## Tests
(laufend)

## OFFEN
(laufend)
