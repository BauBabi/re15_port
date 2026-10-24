# Runde 41 — Spur A "wasser20b0": ABNAHME 1 (unabhaengig, 2026-10-24)

Baum `.claude/worktrees/r41_wasser20b0`, Zweig `r41/wasser20b0`, HEAD fe9d44d4 (= Code-Endstand 7db5a6b2 + Dossier 0636c300 +
Orchestrator-Entscheide A-E3..A-E5 fe9d44d4), Merge-Basis master 116e8b0f. Massstab: AUFTRAG.md (Wortlaut Punkt 1/2, Nachtraege 2/3,
Entscheide E1/E2, **A-E3/A-E4/A-E5 vom 2026-10-24 04:01**), VERTRAG.md §2, Maengel M1..M7 aus `A_abnahme_0.md`.
Der Bau-Agent wird nicht geglaubt; nur selbst Gemessenes zaehlt. Bericht FORTLAUFEND geschrieben (Stand je Abschnitt).
Scratch: `%TEMP%/claude/.../scratchpad/A41abn1/` (Logs, Bilder, Sonden; Pfade unten als `A41abn1/...`).

## 0. Baustand (selbst gebaut)
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)` (04:08). exe `re15_port/build/platform/pc/re15_pc.exe` mtime 2026-10-24 03:40:31 ist juenger als jede
  Quelle des Diffs (juengste: wassersprung_r41.c 03:40:29, re15_wassersprung_r41.h 03:34:38, enemy_common.c 02:58:42);
  `git status` sauber -> die exe ist der Code-Stand 7db5a6b2 (0636c300/fe9d44d4 aendern keinen Code).
- Diff gegen die Merge-Basis (`git diff master...HEAD`, ohne Belegbilder): aot_common.c (+7/-1), climb_common.c (+5/-1),
  enemy_ai_common.c (+15/-3), enemy_common.c (+4/-1), npckante_r38.c (+2/-1), scd_vm.c (+2), szenenleon_r36.c (+2),
  wasser_r38.c (+3), re15_actor.h (RE15_ACTOR_MAX 21 -> 36), re15_wasser_r38.h (+2), main.c (+12/-3), neu wasser20b0_r41.c/.h,
  wassersprung_r41.c/.h; Tests: test_r41_wasser20b0.c (9 Teile), probes/r41_wasser20b0.cmake, integration
  test_r41_wasser20b0(.cmake|_npc.cmake), test_r36_gegnerzahl.c/.cmake.

## 1. Gezielte Tests (selbst gefahren)
- `ctest -R "^unit_r41_wasser20b0_" --timeout 600` -> `100% tests passed, 0 tests failed out of 9`; Einzellaeufe des Binaers:
  bytes 49/0, modell 4/0, sequencer 16/0, raum 18/0, landung 6/0, npc 90/0, npc_wasser 30/0, stempel 7/0, npc_subs 9/0
  (Pruefungen/Fehler) = 229 Pruefungen, 0 Fehler.
- (weitere Laeufe folgen unten)
- `ctest -R "^integration_r41_wasser20b0(_npc)?$" --timeout 1800 -V` (04:09-04:13, sequentiell) -> `100% tests passed, 0 tests failed
  out of 2`: integration_r41_wasser20b0_npc Passed 115.73 s (`npc2000: Spawns 0x42=1 0x40=1; Schritte Slot 1 (0x42, Clip 5) = 8,
  Slot 2 (0x40, Clip 5) = 8, Toene >= 0: 1`; `landung_leon: Landungen 1, fx-Zeilen id=6 sub=3: 104`; `landung_elza: Landungen 1,
  fx-Zeilen id=6 sub=3: 104`), integration_r41_wasser20b0 Passed 97.45 s (Leon UND Elza: `EM024-Spawns=35 fx-Zeilen id=15: 2408
  Cut7=1 Cut6=1 frei(mo=1)=1 zeichnet0x24=0`).
- **Alle** Nicht-Integrations-Tests (RE15_ACTOR_MAX 36 wirkt auf jede Aktor-Schleife; neue Haken in den NPC-Subs aller Typen):
  `ctest -R "^(unit|probe|prop)_" --timeout 600 -j 4` (04:24-04:35) -> `100% tests passed, 0 tests failed out of 925`
  (633.66 s). Darin u.a. unit_r39_heliport1171_binder (RBJ-Haken), dog_*/re2_dog_* (64-Bit-Maske), r36_gegnerzahl, r38_wasser*,
  re2_spider_water_*, r38_npckante, r36_szenenleon, npc_*.

## 2. RE-Gate (Stichproben selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, PSX.EXE)
- Entity-Takt (Stempel als Pro-Bild-Wert, N1.10b): `jalr v0` @0x8001a588, `lw v0,-14460(v0)` (g_entity) @0x8001a594,
  `sh zero,136(v0)` @0x8001a59c -> +0x88 wird je Entity NACH dem Handler geloescht. BELEGT.
- RBJ-Binder FUN_8001b3f8 (N1.10c): `addu a3,zero,zero` @0x8001b410 (einmalig, Verzoegerungsplatz), `lw a0,0(v1)` @0x8001b438
  (u32-Markerwort), Spieler `andi v0,a0,0x1`/`srl a0,a0,1` @0x8001b440/48, `sw v0,0(t3)` @0x8001b450, Gegner-Schleife
  `andi`/`srl` @0x8001b470/78, Ziel +0x180/+0x184 @0x8001b4a0/@0x8001b4bc, `bne a0,zero,0x8001b470` @0x8001b4c0 + `addiu a3,a3,1`
  @0x8001b4c4. BELEGT. Anmerkung: a3 laeuft ueber die Records weiter (kumulativ) - im Original ist damit ein Gegner-Index >= 31
  ueber einen spaeteren Record erreichbar; der Port-Haken `mslot < 32` beseitigt das undefinierte `1u << 32..35` im (abweichenden)
  Port-Modell, ist aber nicht das Original-Modell. Das Original-Modell ist per Entscheid A-E5 an Spur J vergeben -> kein Mangel A.
- NPC-Sub-Tabelle @0x80076ca0 (`table`): [4]=0x80051148 [5]=0x80051484 [7]=0x80051908 [8]=0x80051b00 [9]=0x80051cf8
  [18]=[22]=0x80052508 [19]=0x80052864. BELEGT.
- Sub 9: `jal 0x8001aac4` (Kurs) @0x80051e44, Bildwort `lw v0,360(v0)`/`lw v1,0(v0)`/`andi v0,v1,0x4000` @0x80051e58-68,
  `addu a0,zero,zero` @0x80051e70, Fuss 7-3*bit @0x80051e74-8c, `jal 0x80045630` @0x80051e88. BELEGT (Haken nach dem Kurs, a0 = 0).
- Sub 4 Phase 2: `lw v0,360(a0)` @0x800512d4, `andi v0,v1,0x4000` @0x800512e4, `addu a0,zero,zero` @0x800512f0, `jal 0x80045630`
  @0x80051304, DANACH Kurs `jal 0x8001aac4` @0x8005133c und `jal 0x800245d8` @0x80051344 - der Port-Haken steht vor Kurs/Vorschub. BELEGT.
- Kante 0x12/0x16: `andi v0,v1,0x4000` @0x800525b4, `addu a0,zero,zero` @0x800525bc, `jal 0x80045630` @0x800525d4, danach anim_set
  mit +0x170/+0x174 `lw a0,368(v0)`/`lw a1,372(v0)` @0x800525e8-ec. BELEGT.
- Sub 7: `jal 0x800245d8` (pos_advance, a0 = 0x800) @0x800519c8 VOR dem Test @0x800519dc-ec, `jal 0x80045630` @0x80051a0c, danach
  LOCO-Paar `lw a0,132(v0)`/`lw a1,364(v0)` @0x80051a20-24, `jal 0x8001f314` @0x80051a28; `sb v1,148(v0)` (+0x94) @0x8005196c. BELEGT.
- FUN_80045630: Boden `jal 0x800437d4` @0x8004566c (Lage +0x34/+0x3c), Part = *(+0x188) + fuss*0xac @0x8004567c-a8; Zweig Bit 0x80
  `lui a0,0x605`/`ori a0,a0,0xd00` @0x800456b8-bc, a2+0x40, Versatz (0,100,0), Gier +0x6a; Zweig Stempel `lh v0,136(a1)` @0x800456dc,
  `addiu s0,a0,-3` @0x800456ec, `0x06032000` @0x800456f0-f4, `addiu a2,a2,-108` (= Part[fuss-1]+0x40) @0x800456f8, Gier 0x800;
  `jal 0x80019700` @0x80045710; Ton: `andi v0,s0,0x7f`/`addu a0,s3,v0` @0x80045718-24, `sltiu v0,v0,0x40` @0x80045730, NPC
  `j 0x80045794`/`addiu s3,a0,-8` @0x80045758-5c, sonst Override DAT_800aca3c&0x4000 -> 9 @0x80045780-90. **Lautstaerke/Richtung:**
  `jal 0x80045a64` mit a0 = Entity+0x34 @0x80045830-34 (FUN_80045a64: Abstand Kamera(+4/+8/+12 des Kamerasatzes DAT_800b0fe4*32)
  -> Entity, `SquareRoot0` x3, `0x10624dd3`-Multiplikation >> 4 = /250 @0x80045b3c-4c, Deckel `slti v0,s4,128` -> 0x7f
  @0x80045b50-5c, Seite ueber FUN_80045d6c @0x80045b70/88/a0). BELEGT.
- Letterbox (M4): `read 0x80072ec0 --w 2`: ab 0x80072ecc POLY_F4 (0,0)-(320,0)-(0,24)-(320,24), zweiter Satz +0x18 gleich. BELEGT.
- Guess-Tell-Suche im Diff (`git diff master...HEAD -- re15_port`, ohne Belegbilder): "deferred"/"for now"/"TODO" nur in
  Kontextzeilen des Altbestands (aot_common.c Include-Kommentar "deferred grant", scd_vm.c "deferred until"); "plausibel",
  "tunable", "interim", "faithful": keine Treffer. Neue Konstanten ohne @0x: keine gefunden; PORT-WAHL gekennzeichnet
  (Aufsetzer-Regel 0x4b / 0x47-CDEMD1, Zehntel-/Halbe-Spanne-Schwelle - Form an den markierten Typen belegt, s. 4.3).

## 3. Punkt 1 — 20B0-Wasserschwall (eigene Messung)
### 3.1 Weg
Tuer-Zensus aller 240 RDTs (`scd_dump_room.py`, 653 Door_aot_set-Saetze, Ziel-Bytes [22]/[23]): KEIN Satz fuehrt nach Stage 1 Raum
0x0B (20B0/20B1). Im Auslieferungsstand ist 20B0 nur ueber das Debug-Menue ("L TUNNEL FLOOD", so auch die Original-Messung der
Spur) erreichbar; der Tuerweg 2030 -> 20B0 entsteht erst in Spur C. Der Debug-Sprung ist hier also der Original-Zugang.
### 3.2 Lauf p20b0_voll (Leon, Messkopie re15_pc_a41abn1_p20b0_voll.exe, env -i)
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=20b0@250
RE15_FX_LOG=fx.log RE15_ENEMY_DBG=1 RE15_SPAWN_DIAG=1 RE15_FRAMEDUMP=0-330/1:f RE15_EXIT_AT=340#20b0` -> rc 0; debug.log
`Cut_chg(11)` ... `[scd F105] Cut_chg(7)`, `[scd F146] Cut_chg(15)`, `[scd F228] Cut_chg(6)`, `[scd F242] letterbox closed ->
gameplay (player_mode=0)`; fx.log 3548 Zeilen. Die Szene laeuft durch, der Spieler wird frei (vorher Port: haengt, keine Partikel).
Elza 20B1: integration_r41_wasser20b0 (1.) gleich.
### 3.3 Partikel-Pool Original gegen Port (eigene Lesung, `A41abn1/pool_vgl.py`, Platz-Stride 0x84 ab 0x800a73b8, Felder +0x6c/
+0x70/+0x71/+0x72/+0x6e/+0x40 - dieselben Offsets wie re15_esp.c)
- tf_8.sav (Takt 1): 8 aktive 0x0f-Plaetze = 2 x sub 0 Skala 0x2000 a1 -200 Versatz (200,1800,0) + 6 x sub 7 (0x4000 x5, 0x3000 x1)
  mit den Versaetzen (-1000,2700,-1200) (-2000,2700,1200) (-1000,2700,0) (-2000,1300,-1200) (-1000,1300,1200) (-2000,1300,0), Anim 4.
  Port F7: 8 Plaetze, 2 x sub 0 bei z -7350/-5550 (= Saetze 0x08/0x1A), 6 x sub 7, frame=4. GLEICH.
- tf_7.sav (Takt 48): 22 Plaetze, (sub,anim) = (0,5)x4 (0,8)x6 (0,11)x4 (0,14)x2 (7,7)x6. Port F54: exakt dieselbe Verteilung
  (22 Plaetze). GLEICH (Versatz F = Takt + 6, wie im Dossier).
### 3.4 Wolke am exakten Framebuffer (Abnahme-0-Mangel M1 nachgeprueft)
VRAM von tf_7.sav (`vram_dump.py`, GPU-VRAM-Marke), oberer Puffer = Cut 12; Port-Lauf p20b0_cut12 (wie 3.2, dazu RE15_FORCE_CUT=12,
FRAMEDUMP 40-62). CLUT des Effekts @VRAM (288,492): `0000 e39a d737 ced4 c272 ba0f adac a56a 9d28 9907 90c5 ...`. Region x 60..200,
y 70..170, exakte Treffer je CLUT-Index 3..11:
```
Original tf_7 oben :  5  21  40 236 308 393 826 673 906   (Summe 3414)
Port F52 (Cut 12)  :  5  22  57 236 322 447 835 709 632   (Summe 3268)
```
Bild `A41abn1/vgl_tf7_F52_zoom.png` (links Original-FB, rechts Port): dieselbe Wolke, gleiche Lage, gleiche Textur/Loecher.
"Gruenlich" (G>=100, G>R+8, G>B+8) in derselben Region: Original-FB 21, **Original-VIDEO o052 (bester Abgleich aller Videobilder o030..o069 mit tf_7:
"Zeilen 8..232 gestreckt" mittl. |Diff| 2.64, Identitaet 8.83 -> derselbe Takt) 220**, Port-FB F52 19. Dasselbe Original zeigt im Video also die
10-fache Zahl "gruenlicher" Pixel wie in seinem eigenen Framebuffer - der Massstab der Abnahme 0 (Pixelzahl im Video) mass die
Aufnahmekette. Eigene Nachbildung der Kette (crop 8..232, bilinear 640x480, VP9 6000k yuv420p, area 320x240; `port_20b0_vp9.mp4`)
erreicht die DuckStation-Glaettung nicht (tf_7 durch meine Kette: 54-60 statt 220) - ein Video-gegen-Video-Vergleich ist mit
den vorhandenen Mitteln nicht kalibrierbar. Massgeblich bleibt damit der Framebuffer: dort sind Pool und Bild gleich.
=> M1 (Abnahme 0) als Zeichner-Mangel WIDERLEGT (unabhaengig bestaetigt). M4 (Letterbox): 24 px sind die Original-Primitive
(@0x80072ecc, 2.), das Video ist der Overscan-Ausschnitt (Abgleich gestreckt 2.64 vs Identitaet 8.83); die Port-Balken F5-F14
nach dem Sprung stammen aus dem Startraum des Harness (ROOM1240.RDT @0x0055e = `22 01 1b 01` = Set(1,0x1b,1), selbst gelesen) -
bestaetigt.
Restabweichungen OFFEN 1-3 (erstes Bild nach Kamerawechsel ohne Sprites, RVD 12->13 2-3 Bilder, EM024-INIT +1 Takt) sind laut
Orchestrator-Entscheid (AUFTRAG 2026-10-24 04:01, letzter Punkt) engine-weit und "bleiben fuer die naechste Runde" -> kein Mangel A.
### 3.5 Urteil Punkt 1: ERFUELLT

## 4. Punkt 2 — Wassereffekt beim Sprung von der Kante ins Wasser; NPCs teilen die Effekte (+ Nachtrag 3, E1, A-E4)
### 4.1 Punkt 2a Landung (eigener Lauf landung_leon, echter Weg Tuer 20A0 -> 2090)
`RE15_DEBUG_JUMP=20a0@240 RE15_PLAYER_POS=4950,5000,2048,2 RE15_INPUT_SCRIPT=W9,A0.2,W4,R0.37,W0.3,U0.7,W0.2,A0.2,W0.3,A0.2,W0.3,
A0.2,W0.3,A0.2,W4 RE15_INPUT_SCRIPT_START=0 RE15_CLIMB_TRACE=climb.log RE15_FX_LOG=fx.log RE15_KEIN_GATOR=1 RE15_FRAMEDUMP=180-215/1:f
RE15_EXIT_AT=360#2090` (+ NO_INTRO/NOAUDIO/WINDOW_SCALE=1/TITLE_SHOT): `[aot] DOOR FIRE slot=0 ... spawn=(6300,-1800,-16250)`,
`PC loaded room2090.rdt`, climb.log `T459 room=2090 ... pos=(6363,-640,-14032) rot=3072 band=0 sub=10 kind=0x02 ph=4`; fx.log 104
Zeilen `id=6 sub=3` ab F192 bei w(6363,-640,-14032), `-> sx=96 sy=148 S=8` (im Bild). Bilder `A41abn1/landung_leon_zoom.png`,
`landung_f198_hell.png`: kleine weiss-gruenliche Spritzer um Leons Fuesse F196-F204. Elza 2091: integration_r41_wasser20b0_npc
(1.) `landung_elza: Landungen 1, fx-Zeilen id=6 sub=3: 104`. Mechanik @0x80038730-d0 von Abnahme 0 selbst disassembliert (8 x
0x06033000, Gate `slt` @0x8003874c). NPC-Gegenstueck: die NPC-Kante 0x80052508.. und der Schub 0x80052864.. rufen KEIN
`jal 0x80019700` (eigener Scan), also auch im Original keine NPC-Landungsspritzer. **2a: erfuellt** (ohne Original-Bild der Landung;
Mechanismus belegt).
### 4.2 Punkt 2b NPC-Wasser-EFFEKTE an der exe
- Lauf npc2000_lang (Leon, `RE15_SET_FLAG=4:244 RE15_DEBUG_JUMP=2000@240 RE15_PLAYER_POS=4350,-20500,0,1 RE15_NPCSCHRITT_LOG=npc.log
  RE15_FOOT_DEBUG=1 RE15_FX_LOG=fx.log RE15_AUDIO_CAP_SYNC=cap.raw RE15_FRAMEDUMP=0-3600/20:f RE15_EXIT_AT=3600#2000`, Audio AN):
  64 NPC-Schritte (Marvin 0x42 Slot 1 / Ada 0x40 Slot 2; Gehen Clip 5 und Laufen Clip 0 der zweiten Szenenhaelfte zu (-26100,...)),
  davon 7 im Kanal y=0 unter dem WATER-Satz (`stempel=-900 zweig=stempel ton=18 plaetze=1`, z.B. `F1637 slot=1 typ=0x42 clip=5
  bild=8 fuss=7 ... pos=(-5866,0,-22509)`, `F1762 slot=2 typ=0x40 ...`). fx.log zeichnet den Platz `id=6 sub=3 ... F=1640 -> sx=159
  sy=143`; Bild `A41abn1/npc2000_wasser_bogen.png` (F1640/F1660 Ada, F1780 Marvin im Kanal, gruenliche Spritzer an den Fuessen).
  Diese Stempel entstehen waehrend der Szene (player_mode 2) - die M3-Aufhebung wirkt.
- Lauf elza2001 (Elza: RE15_TITLE_SHOT_CHAR=1, `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `RE15_SET_FLAG=3:65
  RE15_DEBUG_JUMP=2001@240`): John 0x49 spawnt (`Sce_em_set type=0x49 ... (-26003,-1800,-26062)`), laeuft per Plc_dest (Sub 9/5/4/5/9):
  23 Schritte, davon 5 im Kanal `zweig=stempel plaetze=1` (F310..F374). Sichtbar nicht pruefbar: Kamera Cut 0 zeigt Elza an der
  Leiter, John ausser Bild (Bogen `A41abn1/elza2001_bogen.png`) - die Spawns sind im Log.
- Unit npc_wasser (1.): alle Typen ausser 0x47 durch den FLR-0x97-Fleck (Zweig boden 0x06050D00).
- Kein Doppelschritt (eigene Auswertung der drei npc.log): npc2000_lang 64 Schritte, npc11c0 8, elza2001 23 - kein Abstand <= 2
  Bilder je Slot; Takt 10-16 Bilder (Laufen ~11, Gehen ~15).
- => NPC-Wasser-EFFEKTE: erfuellt (beide Szenarien, Szene + Riegel).
### 4.3 Punkt 2b NPC-SCHRITT-TOENE an der exe (Nachtrag 3: "fussschritt sounds wenn sie laufen ... muss auch fuer die NPCs gelten")
- npc2000_lang, debug.log (RE15_FOOT_DEBUG, audio_pc.c re15_audio_footstep): **`[foot] SE unterdrueckt: type=21 -> vag=-1
  (dekodiert=0)` x54 und `type=18 -> vag=-1` x7** = 61 der 64 NPC-Schritte spielen KEINEN Ton; hoerbar nur die 3 Schritte mit
  Index 22 (a0 = 1, Sub 5 Laufbeginn: `[foot] SE foot=7 type=22 vag=5` x2, `foot=4 type=22 vag=5` x1). Spieler-Schritte (type 29,
  vag 1) spielen.
- npc11c0 (Leon, `RE15_DEBUG_JUMP=11c0@240`, Marvin per Plc_dest): 8 NPC-Schritte, **alle 8 `SE unterdrueckt: type=21 -> vag=-1`**.
- elza2001: 23 NPC-Schritte (John), **alle 23 unterdrueckt** (`type=21` x18, `type=18` x5).
- Ursache (Daten, eigene Lesung): der NPC-Zweig indiziert dieselbe snd0-EDT mit Boden - 8 (`addiu s3,a0,-8` @0x8004575c, Tabelle
  *(DAT_800ac778+8) @0x80045748-54 = RDT-Kopf +0x08). ROOM2000/2001 EDT @0x161c4/@0x165dc: Eintraege 15..21 LEER (00000000),
  22..31 belegt; ROOM11C0: nur 0,1,2,5,6,26..31 belegt. Original-RAM tf_7.sav (ROOM20B0): DAT_800ac778 = 0x80119330,
  *(+8) = 0x8011e33c, 128 B dort == Datei-EDT (RDT +0x08 = 0x500c), Eintraege 15..21 = 00000000, 22 = 0000b111 - der NPC-Zweig
  liest im Original genau diese Tabelle. Zensus aller 206 RDTs mit snd0-EDT: Index 21 (= NPC auf Boden 0x1d) in nur
  26 Raeumen belegt, Index 29 (Spieler) in 206. Im Original ist ein leerer Eintrag ebenfalls stumm (FUN_80045630: Stimme =
  (e[3]&0x1f)-16+(fuss!=4) = -16/-15, Satz bei 0x800b2420+Stimme*18 = ausserhalb der 8 Saetze, die FUN_800458d4 abarbeitet - `addiu
  s1,s1,9392`/`addiu s1,s1,-18`/`bne s4,zero` @0x800458f0..0x800459e4). Der Port ist an dieser Stelle also original-gleich STUMM -
  der Nutzer-Wortlaut verlangt aber hoerbare NPC-Schritte (Beta-Luecke: nach der Projektregel Beta -> Retail RE2 pruefen bzw. eine
  gekennzeichnete NUTZER-VORGABE-Abbildung; nicht gebaut). Der Riegel prueft nur den Index (`ton >= 0` in npc.log,
  unit "Schritt-Ton 0x17-8 = 15"), nicht ob ein VAG spielt - Index 15 ist in ROOM2000 ebenfalls leer.
- Zensus der 52 Raeume mit NPC-Sce_em_set (Typ 0x40..0x4f; FLR @RDT+0x34, 12-B-Saetze, Typbyte +8; EDT @RDT+0x08;
  `A41abn1/npc_ton_zensus.txt`): der Hauptboden 0x1d hat in KEINEM dieser Raeume einen Eintrag bei Index 21 (a0 = 0); nur 11 von
  70 (Raum, Bodentyp)-Paaren haben einen belegten NPC-Eintrag (z.B. 0x17/0x1a in 4000/6030). Ohne Abbildung bleiben NPC-Schritte
  damit praktisch im ganzen Spiel stumm (hoerbar nur der Laufbeginn a0 = 1 auf 0x1d in 1260/1261/2000/2001).
- Das Dossier (N1.14 "Hoer-/sichtbar: alle NPCs ... machen beim Gehen, Laufen ... Schrittgeraeusche") ist damit an der exe WIDERLEGT.
- => NPC-Schritt-TOENE: **nicht erfuellt**.
### 4.4 Entscheid A-E4 (Abnahmepunkt der Spur A): NPC-Schritt leiser/seitlich nach Abstand und Lage zur Kamera wie FUN_80045a64
- Original (2.): `jal 0x80045a64` mit Entity+0x34 @0x80045830 - Abstand zur Kamera /250 (@0x80045b3c-4c), Deckel 0x7f, Seite via
  FUN_80045d6c, Ergebnis DAT_800b2824/26 in den Stimmsatz (+14/+16 @0x80045888-a4).
- Port: `re15_audio_footstep(int foot, int sound_type)` (audio_pc.c:3809) hat keinen Lage-Parameter; Lautstaerke = nur
  Ton-Lautstaerke `vol = (tvol * 0x4000 / 127) >> 1`, `vol_r_q15 = 0` ("legacy mono"); wassersprung_r41.c:356 ruft
  `re15_audio_footstep(fuss, idx)` ohne Lage. Kein Haken in audio_pc.c im Diff (`git diff master...HEAD --stat`: audio_pc.c fehlt).
  Messzeile je Schritt mit Lautstaerke/Seite: keine. An der exe in 2000 ist der Punkt zudem gar nicht hoerbar (4.3: 61/64 stumm).
- => A-E4: **nicht erfuellt** (Entscheid 04:01 nach dem Bau-Ende 03:57; im Dossier als OFFEN 8 gefuehrt).

## 5. Regression durch RE15_ACTOR_MAX 36 (selbst gefunden)
- Integrationskette der beruehrten Module (Wasser-/Rampen-Raeume, NPC-Szenen, Klettern, Schatten, ESP, Gegnerzahl), `ctest -R
  "^integration_(r35_granate|r36_gegnerzahl|r36_kartenitems_bild|r36_szenenleon|r36_tueren|r37_itemband_lift|r38_rolltor20a0|
  r38_rolltor20a0_abe|r38_wasser|r38_wasserzombie|r39_johnruf1041_tuer|r39_stimmen|r36_klettern|r39_gegner_schatten|r36_pri_schatten|
  r34_plattform_esp_eintritt|r37_roy_leon1170|r37_szene11c0|r38_werkstatt11b0)$" --timeout 1800 -j 2`: **rot
  integration_r35_granate** (49.37 s), woertlich: `r35_granate [gator]: Boss-Slot 20 fehlt im Zustandslog (Bild 59/60)`.
- Ursache (gemessen, Laufordner re15_port/build/tests/integration/r35_granate_wd/lauf_e1788512/gator): debug.log `[enemy] ROOM2090:
  Alligator (0x23) in Wasserzone gesetzt, Slot 35 (Port-Ergaenzung, Nutzer-Auftrag)`, state.log `[35 t=23 st=0 ...]`. main.c setzt
  den Alligator auf den hoechsten freien Platz ab RE15_ACTOR_MAX-1 (Kopf test_r35_granate.cmake Z.36-39: "Slot 20 = RE15_ACTOR_MAX-1");
  mit 36 liegt er auf Slot 35, der Riegel erwartet fest 20 (`r35_boss(gator 2090 20 23 60 1000)` Z.403). Kein Flattern
  (deterministisch, Ursache im Log). Der Diff dieser Spur verschiebt den Boss-Platz, passt den Riegel aber nicht an; die
  Riegelauswahl des Bau-Agenten (N1.12: "keiner dieser Riegel ... hat mehr als 31 Aktoren") hat r35_granate nicht gefahren.
  Zugleich Hinweis fuer B/F/G: der Alligator-Platz ist jetzt 35 (Aktor-Masken mit 32 Bit beachten; eigener grep: die verbliebenen
  `1u << idx`-Masken sind auf < 16 bzw. `ai < 32` begrenzt, s_dog_noclamp_mask ist 64 Bit, RBJ-Haken `mslot < 32`).

## 6. Vertrag (VERTRAG.md) und Riegel
- Pfade (`git diff master...HEAD --name-only`): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine
  CMakeLists.txt (0 Treffer). Bank-9-Bits / Nachrichten-Ids: keine neuen (grep `flag_set|game_flag|MSG_ID|re15_msg` auf die
  hinzugefuegten Zeilen: 0). OK.
- Gemeinsame Dateien, alle Haken markiert "Runde 41 Spur A": aot_common.c (+7/-1; Datei laut §1.3 der Spur H), scd_vm.c (+2; C/F/G),
  npckante_r38.c / szenenleon_r36.c (je 1 Aufruf + Include; NPC-Code = J/K), enemy_common.c (1 Bedingung; ohne Eigentuemer),
  climb_common.c (1 Aufruf + 2 Includes), wasser_r38.c (Getter + 1 Aufruf), enemy_ai_common.c (EM024 + 4 NPC-Sub-Haken + 64-Bit-
  Maske), main.c (2 Haken, per A-E3 angenommen), re15_actor.h (RE15_ACTOR_MAX, E2). Klein und markiert; fuer die Integration:
  Konfliktflaechen mit H (aot_common.c), J/K (NPC-Subs), C/F/G (scd_vm.c). Kein Mangel.
- RE15_PLAYER_POS 4. Wert (main.c, A-E3): 17 Riegel uebergeben schon 4 Werte (z.B. test_r37_itemband `-3596,1523,1024,4`); bisher
  wirkte der 4. Wert nur im Sprung-Spawn (main.c ~Z.7553, Runde 33), jetzt auch beim Boot-Teleport (y/floor/Band im Startraum vor dem
  Sprung). Alle 17 nutzen RE15_DEBUG_JUMP (der Sprung setzt die Lage neu) - Nebenwirkung siehe 7.
- Riegel je Punkt: Punkt 1 unit bytes/modell/sequencer/raum/stempel + integration_r41_wasser20b0 (Leon+Elza) - messend. Punkt 2a
  unit landung + integration_r41_wasser20b0_npc (Tuerweg Leon/Elza) - messend. Punkt 2b Effekte: unit npc/npc_wasser/npc_subs +
  integration npc2000 - messend; Typ 0x47 (Elliot/Roy) ohne Wasser-Lauf-Pin (nur Marken-Zensus). Punkt 2b Toene: die Riegel pruefen
  nur den Tonindex (`ton=` im Log bzw. "Schritt-Ton 0x17-8 = 15"), nicht ob ein VAG spielt -> nicht messend fuer den Nutzer-Punkt
  (Index 15 und 21 sind in ROOM2000 leere EDT-Eintraege). Kein Riegel fuer A-E4.
- Parallel-/Linux-Festigkeit der neuen Riegel: eigene Laufordner + exe-Kopie mit Praefix (Muster test_r34n_b_cursor.cmake),
  cmake_minimum_required, probes-cmake linkt `m` ausser WIN32. OK.

## 7. Nebenwirkungen: weitere Integrationslaeufe (selbst gefahren, 04:36-05:xx, Last durch parallele Agenten)
- Kette 1 (Abschnitt 5, `-j 2`): Passed r38_rolltor20a0 282.95 s, r38_rolltor20a0_abe 194.94 s, r36_klettern 535.28 s,
  r36_szenenleon 161.71 s, r38_werkstatt11b0 154.36 s, r38_wasserzombie 125.97 s, r38_wasser 175.52 s, r37_szene11c0 120.61 s,
  r37_roy_leon1170 58.69 s (an d77bf72a rot, jetzt gruen), r36_gegnerzahl 88.06 s, r36_pri_schatten 27.81 s,
  r34_plattform_esp_eintritt 9.17 s, r39_gegner_schatten 58.66 s, r36_tueren 37.93 s, r36_kartenitems_bild 444.66 s;
  **Failed r35_granate 49.37 s** (Abschnitt 5).
- Kette 2 (Riegel mit 4-Werte-RE15_PLAYER_POS, `-j 2`): Passed r35_inventar1050 71.54 s, r35_zgirl_5090 143.21 s,
  r37_fahrstuhl 164.08 s, r37_fahrstuhl_n1 132.26 s (weitere unten).

## 8. Urteile je Nutzer-Punkt (gegen den Wortlaut)
### Punkt 1 — "Runde 41 startet mit dem Wasserschwall in 20B0 im Original ... in unserem Port noch kaputt" -> ERFUELLT
Original gemessen (Spur: Savestates + Video; Abnahme: eigene Pool- und VRAM-Lesung tf_8/tf_7), Port repariert: 35 EM024 unsichtbar,
Sequencer byte-gleich (Pool Takt 1 und Takt 48 gleich, 3.3), Wolke am exakten Framebuffer gleich (CLUT-Histogramm, Bild, 3.4), Szene
laeuft durch (Cut 7/15/6, Spieler frei F242), Leon und Elza. Der Abnahme-0-Mangel M1 (Dichte im Video) ist die Aufnahmekette
(Original-FB 21 vs Original-Video 220 am selben Takt), M4 der Overscan-Ausschnitt. Die Rest-Zeitabweichungen OFFEN 1-3 sind
per Orchestrator-Entscheid (04:01) in die naechste Runde verschoben.
### Punkt 2 — "Wassereffekt vom rein springen von der Kante in das Wasser. Auch die NPCs teilen die gleichen Effekte ..."
### (+ Nachtrag 3 "Effekte wenn sie sich im Wasser bewegen, fussschritt sounds wenn sie laufen ... auch fuer die NPCs") -> TEILWEISE
- 2a Landung: erfuellt (4.1; Leon 2090 + Elza 2091 ueber den Tuerweg, 8 x 0x06033000, im Bild).
- 2b NPC-Wasser-Effekte: erfuellt (4.2; Ada/Marvin im 2000-Kanal waehrend der Szene, John 2001 im Elza-Szenario, Unit je Typ).
- 2b NPC-Schritt-Toene: NICHT erfuellt (4.3; an der exe 61/64 in 2000, 8/8 in 11C0, 23/23 in 2001 stumm - leere EDT-Eintraege
  Boden-8; das Dossier behauptet das Gegenteil).
- A-E4 Lautstaerke/Richtung nach FUN_80045a64: NICHT erfuellt (4.4; nicht gebaut, kein Haken in audio_pc.c).

## 9. Gates
- RE-Gate: HAELT (2.; alle Stichproben enthalten das Behauptete; PORT-WAHL gekennzeichnet; keine Guess-Tells).
- Vertrag: HAELT (6.; Pfade, Bits, Ids; Haken klein und markiert; main.c per A-E3).
- Tests: 9/9 Spur-Units, 925/925 Unit/Probe/Prop, Spur-Integration 2/2 gruen; **eine Regression: integration_r35_granate rot**
  (5., Ursache RE15_ACTOR_MAX 36 dieser Spur, deterministisch). Riegel fuer die Schritt-Toene messen nur den Index (6.).
- Erklaert der Fix den Befund? Punkt 1 ja (Mechanik + Pool + FB). Punkt 2b-Toene nein: der Index wird gerechnet, der Ton spielt nicht.
- **bestanden = NEIN.**

## 10. Maengel (nummeriert, nachpruefbar)
M1 (Punkt 2b, NUTZER-VORGABE Nachtrag 3 "fussschritt sounds wenn sie laufen ... auch fuer die NPCs"): NPC-Schritt-Toene sind an
   der exe stumm. Nachweis: `RE15_FOOT_DEBUG=1` (Audio an) -> debug.log `[foot] SE unterdrueckt: type=21 -> vag=-1` / `type=18 ->
   vag=-1`: ROOM2000 (Leon, `RE15_SET_FLAG=4:244`, Marvin/Ada) 61 von 64 Schritten, ROOM11C0 (Marvin) 8/8, ROOM2001 (Elza,
   `RE15_SET_FLAG=3:65`, John) 23/23. Ursache: NPC-Index Boden-8 (@0x8004575c) trifft leere snd0-EDT-Eintraege (ROOM2000/2001 15..21
   leer; in KEINEM der 52 NPC-Raeume ist Index 21 fuer den Hauptboden 0x1d belegt). Das Original ist dort ebenfalls stumm (Stimme
   -16/-15 ausserhalb der 8 Saetze @0x800b2420; FUN_800458d4) - die Nutzer-Vorgabe verlangt aber hoerbare NPC-Schritte. Noetig:
   Entscheid/Belegung des Weges (Beta -> Retail: RE2-NPC-Schrittton pruefen und uebernehmen, sonst gekennzeichnete NUTZER-VORGABE-
   Abbildung, z.B. Rueckfall auf den Spieler-Eintrag Boden+a0 bei leerem NPC-Eintrag) und ein Riegel, der das ABSPIELEN misst
   (VAG >= 0 / Mischer-Stimme je NPC-Schritt, je Typ, beide Szenarien), nicht nur den Index.
M2 (Entscheid A-E4, Abnahmepunkt der Spur A): Lautstaerke/Richtung der (NPC-)Schritte nach FUN_80045a64 (`jal 0x80045a64` @0x80045830;
   Abstand Kamera->Entity+0x34 /250 @0x80045b3c-4c, Deckel 0x7f @0x80045b50-5c, Seite FUN_80045d6c, Stimmsatz +14/+16 @0x80045888-a4)
   nicht gebaut: re15_audio_footstep(foot, type) ohne Lage, `vol_r_q15 = 0` (mono), kein Haken in audio_pc.c, keine Messzeile je Schritt,
   kein Pin in 2000. (Haengt an M1: in 2000 spielt fast kein NPC-Schritt.)
M3 (Regression dieser Spur): integration_r35_granate rot - `r35_granate [gator]: Boss-Slot 20 fehlt im Zustandslog (Bild 59/60)`;
   main.c setzt den Alligator auf den hoechsten freien Platz ab RE15_ACTOR_MAX-1, seit RE15_ACTOR_MAX 36 = Slot 35 (debug.log
   `Alligator (0x23) in Wasserzone gesetzt, Slot 35`, state.log `[35 t=23 ...]`), der Riegel erwartet fest 20 (test_r35_granate.cmake
   Z.403). Riegel an RE15_ACTOR_MAX-1 koppeln (bzw. den Platz aus dem Log lesen) und die uebrigen Annahmen "letzter Platz = 20"
   pruefen; Spuren B/F/G auf den neuen Alligator-Platz hinweisen.
M4 (Riegel): Typ 0x47 (Elliot CDEMD0 / Roy CDEMD1) ohne Wasser-Lauf-Pin (unit npc_wasser: 6 Typen); die Ton-Pruefungen in unit npc /
   npc_wasser / npc_subs ("Schritt-Ton 0x17-8 = 15") und integration npc2000 (`ton >= 0`) pruefen nur den Index - sie bleiben gruen,
   obwohl der Ton unterdrueckt wird (Index 15 und 21 sind in ROOM2000 leer).
M5 (Dossier): N1.14 "alle NPCs ... machen beim Gehen, Laufen, Rueckwaertsgehen und Drehen ... Schrittgeraeusche" ist an der exe
   widerlegt (M1) - richtigstellen.

Hinweise (keine Maengel dieser Spur): H1 RBJ-Haken `mslot < 32` passt zum (abweichenden) Port-Modell; das Original zaehlt a3 ueber
die Records weiter (@0x8001b410/@0x8001b4c4) -> Spur J (A-E5). H2 portwahl-Cache (wassersprung_r41.c, Ring 32) ist nach Zeiger/
Clip/Bildzahl verschluesselt - nach einem Bankwechsel auf dieselbe Adresse koennte ein alter Eintrag greifen (nur 0x4b/0x47-CDEMD1).
H3 OFFEN 1-4 (Schnittbild ohne Sprites, RVD-Zeitpunkt, Gegner-INIT-Takt, allgemeine Szenensperre) laut Orchestrator naechste Runde.
