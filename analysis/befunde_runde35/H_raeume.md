# Runde 35 Spur H "raeume" — Dossier (fortlaufend)

Baum `.claude/worktrees/r35_raeume`, Zweig `r35/raeume`, Basis master 154a73c1.
Zuteilung (VERTRAG.md): Nachrichten-IDs ROOM1190/1191 = 6..11, Ereignis 27 (1190). Keine Bank-9-Bits.

## Punkte (Wortlaut AUFTRAG.md)
1. ROOM 1190: Hunde haben einen Schatten waehrend sie durch die Luke springen (in der Luft) -> muss raus.
2. ROOM 1190: Zielscheibe ganz links + 3. von links: "This target has a surprisingly large number of bullet holes."; die anderen beiden: "This target does not have many bullet holes".
3. ROOM 1200: nach Minidisc-Player steht der Trage-Zombie auf und laeuft durch die Luft, statt auf die Spieler-Ebene herunterzukommen.
4. ROOM 1210: Zombie-Arme beim Greifen nicht synchron zu Leon beim Schuetteln (Clipping).

Messwerkzeug: Kopie der exe unter eigenem Namen (`re15_pc_r35h_mess.exe` neben der exe), Lauf aus einem
Arbeitsordner im Scratchpad. Bilder per `RE15_FRAMEDUMP` (gdigrab liefert in dieser Sitzung weisse Bilder,
s. Auftrag). Bau: `local_build.sh configure` + `build` am Basisstand 154a73c1 = OK.

---

## Punkt 1 — Hunde-Schatten beim Sprung durch die Luke (ROOM1190)

### 1.1 Messung vorher (Basisstand, gebaute exe)
Lauf: `RE15_DEBUG_JUMP=1190@250 RE15_SUBSTART=10@60#1190 RE15_STATE_LOG RE15_FRAMEDUMP=100-520/4`
(sub10 = das Hunde-Ereignis des Raums, s. 1.2). State-Log Hund Slot 1:
F181 st4 ss2=1 Clip 0x14 (Absprung) -> F197 ss2=2 z=-27480 -> F205 ss2=3 Clip 0x15 (Flug) z=-25368 ->
F221 st1 (gelandet) z=-22401. Hund 2 springt F213..F253, Hund 3 F245..F285. Kamera = Cut 11.
Bilder `H_raeume/p1_vorher_F184-216.png` (8 Bilder) und `p1_vorher_F200_F204.png` (vergroessert):
**unter dem Hund haengt ein dunkler Schatten-Quad mitten an der Wand, in Hoehe der Pfoten** — genau der
Nutzerbefund. Ursache im Port: `main.c` NPC-Schatten `int32_t nsh_y = npc->y;` — fuer Typ 0x20 wird der
Schatten auf die KOERPER-Hoehe gelegt (nur die Kraehe 0x21 hat einen Boden-Zweig).

### 1.2 RE-Belege
ROOM1190 SCD (scd_dump_room.py): sub13 `Sce_em_set 44 00 20 40 02 ... b2 02 f0 f1 ac 90` = Hund Typ 0x20,
grid 0x40, floor 2, (690,-3600,-28500); sub10 @0x027E6 `Member_set 0x0C=0x43` (grid 0x43 = Absprung-Freigabe)
je Hund, Hund 1/2 zusaetzlich `Member_set 0x01=-3600` (y). sub11 dreht die Luke (obj 4).

RE1.5 — Sprung-Maschine FUN_80111398 (Zustand 4 Sub 0, STAGE1_full/FUN_80111398.c):
`case 1: if (0xc < +0x95) { +0x8c += 6; 245d8(0); +0x38 -= 0x14; +0x1ba = 0; }` — ab Bild 0xD steigt der
Koerper (+0x38 = y), die BODEN-Referenz +0x1ba wird 0 (Raumboden). `case 3: 0x8001c1a4(+0x8c,0,-0x1e,+0x1ba)`
ballistisch bis +0x1ba.
RE1.5 — Hunde-Root FUN_8010d7f8 (Tabelle @0x80120f74), letzte Zeile, IMMER (auch im Pausen-Zweig):
`func_0x8001b064(_DAT_800ac784 + 0xb0, (int)*(short *)(_DAT_800ac784 + 0x1ba));`
FUN_8001b064 (RE_15_Quellcode_V2): `local_88.t[1] = (long)param_2;` — die Quad-Hoehe IST der 2. Parameter =
**+0x1ba (Boden), nicht +0x38 (Koerper)**. X/Z = +0x34/+0x3c + gedrehter Versatz (+0xb8/+0xba). Gezeichnet nur,
wenn `FUN_80014368(+0x34, DAT_800ac790)` (Punkt im Cut-Viereck) — der Port hat das als `npc_region_culled`.

RE2 (Ziel, wo RE1.5 unfertig ist — hier nur Gegenprobe, RE1.5 ist vollstaendig):
* Allokator 0x80016480 (PSX.EXE RE2): `sw a1,20(t0)` @0x800164DC = rec+0x14 Zeiger auf die Position,
  `sb 5,14(t0)` @0x800164D4 (belegt), `sw a2,4(t0)` @0x80016530 (Halbmasse). Hund ruft ihn mit &+0x38
  @0x80100238-64 (EMD0G_MOD0.BIN).
* Entity-Schleife: @0x80026868 `lw a0,20(s1)` / @0x8002686c `jal 0x8004fba0` (Boden-Sonde an der Position) /
  @0x80026874 `sh v0,450(s0)` = **+0x1C2 = Boden-Y**, dann @0x800268c8 `lh a1,450(s0)` / @0x800268d0
  `lh a2,118(s0)` / @0x800268d4 `andi a3,a3,0x400` / @0x800268d8 `jal 0x800168b4`.
* 0x800168b4 (RE2_Quellcode_V2/FUN_800168b4.c): `param_1[5] = param_2` = rec+0x0A = **Quad-Y = +0x1C2**;
  rec+8/rec+0xC = Position X/Z aus dem Zeiger rec+0x14; Bit 4 von rec+0x0E an, bei word0 & 0x400 aus.
* Zeichner 0x8001699c: `lbu v1,8(s1)` @0x80016A04 / `andi v0,v1,0x4` @0x80016A0C / `andi v0,v1,0x8`
  @0x80016A14, Hoehe `lh v0,4(s1)` @0x80016A4C = rec+0x0A.
* Fenster-Sprung des RE2-Hundes 0x80102C78: setzt nur +0x1C0 |= 2 (@0x80102CE8) und +0x1D3 |= 0x80, KEIN
  Schatten-Ausblenden (Suche `ori ...,0x400` im ganzen EMD0G_MOD0.BIN: kein Treffer) — der Schatten bleibt
  auch in RE2 beim Sprung AM BODEN (+0x1C2).

**Befund:** RE1.5 und RE2 legen den Hunde-Schatten auf die Boden-Referenz (+0x1ba bzw. +0x1C2). Der Port legt
ihn auf den Koerper — deshalb "fliegt" der Schatten mit. Kein Original blendet ihn beim Sprung aus; er liegt
dort, wo der Hund landen wird (Raumboden y=0, +0x1ba := 0 @FUN_80111398 case 1), d.h. unter Cut 11
(Kamera blickt zur Luke hoch) ausserhalb des Bildes bzw. am Boden — NICHT in der Luft.

### 1.3 Umsetzung
* NEU `include/re15_hundeschatten.h` + `engine/src/hundeschatten_1190.c`: `re15_hundeschatten_y(e)` liefert
  fuer Typ 0x20 `e->dog_floor_y` (= +0x1ba, RE2 +0x1C2-Analog), sonst `e->y`. Messschiene
  `RE15_HUNDESCHATTEN_LOG=<datei>` (env-gegatet): je gezeichnetem Hunde-Schatten Koerper-Y / Boden-Y.
* `platform/pc/main.c` (2 Zeilen + Include): im NPC-Schatten nach `int32_t nsh_y = npc->y;`
  `if (npc->type == RE15_HUNDESCHATTEN_TYP) nsh_y = re15_hundeschatten_y(npc);`.
* Konstanten: keine neuen Zahlen; Quelle der Hoehe = Feld +0x1ba (`lh a1,442(v0)` @0x8010d91c), das die
  Sprungmaschine bereits byte-true fuehrt (`sh zero,442(v0)` @0x801114f0, Port enemy_ai_common.c).
* Wirkung ueber den Luken-Sprung hinaus: auch bei den RE2-Angriffsspruengen (Sub 3, y bis -1120) liegt der
  Schatten jetzt am Boden statt am Koerper — das ist RE2 (@0x800268c8 Quad-Y = +0x1C2), Bild
  `p1_angriffssprung_F316_vorher_nachher.png` (links alt: Quad halb in der Luft; rechts: am Boden unter dem Hund).

### 1.4 Messung nachher (gleicher Lauf, neue exe)
`hs.log`: 117 Luft-Bilder (Koerper ueber Boden), in ALLEN schatten_y = boden_y = 0; Luken-Spruenge
F195-213 / F227-245 / F259-277 (Koerper bis y=-3680). Bilder `p1_nachher_F184-216.png`,
`p1_nachher_F200_F204.png`: **kein Schatten mehr an der Wand**. Pixel-Differenz vorher/nachher
(Graustufe, Schwelle 12) in den drei Luken-Spruengen: nachher nur HELLER (F200 1102 px, F204 4762 px je Hund),
**0 Pixel dunkler** = kein neuer Schatten irgendwo im Bild (Raumboden liegt unter Cut 11 ausserhalb).

### 1.5 Test
`unit_r35_raeume_hundeschatten` (tests/unit/test_r35_raeume_hundeschatten.c): echte ROOM1190.RDT, sub13-Spawns,
Freigabe wie sub10, Sprungmaschine Bild fuer Bild, beide KI-Geschmaecker. Ergebnis: je Hund 21 Luft-Bilder
(y bis -3680), in jedem Schatten-Y = Boden (0), 0 Bilder mit Schatten auf Koerperhoehe, Landung y == Boden.

---

## Punkt 2 — Zielscheiben-Texte (ROOM1190/1191)

### 2.1 Messung vorher
ROOM1190 hat 6 Nachrichten (rdt_msgdump): 0 "There's a switch here. Push it?" (Ja/Nein), 1 "There's a driver
here. Take it?", 2 "I have nothing else to do / here.", 3 "No response...[02 00]The power is not supplied.",
4/5 Weste. An den vier Scheiben-Plaetzen gab es nur msg 0 / 3 / 2 — keinen Scheiben-Text (Nutzerbefund).
Raumskript (scd_dump_room.py): main00 @0x02226.. Slots 0..3 sce 5 flags 0x31, Rechteck x=-4400 w=600,
z=-25700/-22000/-18500/-14900 d=2400 (nach dem Hunde-Ereignis (4,234)=1: sce 1 msg 2 @0x0227A..);
sub01: Strom aus ((4,243)=0) -> Aot_reset Slots 0..3 sce 3 -> sub12 (msg 0, Ck(12,31) -> msg 3);
Strom an -> Switch work_vars[0] @0x023C4 -> sub02..05 (Slot 0..3: msg 0, Ja -> Scheibe faehrt, sub06/sub08);
sub10 (Hunde) Aot_reset Slots 0..3 sce 1 msg 2 @0x027B4..

### 2.2 Welche Scheibe ist "ganz links" / "3. von links" — aus der Kamera gemessen
Scheiben = Obj 0..3 (sub13 Obj_model_set @0x0297E/0x029A0/0x029C2/0x029E4, x=-21340,
z=-24516/-20916/-17316/-13716); Slot n bewegt Obj n (`Work_set 2e 03 0n` in sub02..05 @0x024C8/0x02540/
0x025B8/0x02630). Projektion der Scheibenmitte (y=-3186 = Obj-y -4446 + Box-Mitte 1260) durch alle 15 Kameras
(tools/maske/geom.py cut_view = FUN_80053ca4-Nachbau, Kamerablock RDT+0x24) und Framedump mit RE15_FORCE_CUT
5/6/7/9: Bild `H_raeume/p2_kameras_cut5_6_7_9.png` (rote Kreise = projizierte Scheibenmitten S0..S3 liegen
exakt auf den gerenderten Scheiben). In JEDEM Cut, der die Scheiben zeigt (5: Blick den Stand hinunter, 6:
Kabinen 3/4, 7: Kabinen 1/2, 9: Nahsicht), liegen sie von links nach rechts als S0, S1, S2, S3; die Kabinen-
Nummern 1..4 an den Trennwaenden laufen gleich (Cut 7 "1" links, Cut 6 "3"/"4").
=> **ganz links = Slot 0, 3. von links = Slot 2** (viele Einschuesse); Slot 1 und 3 = wenige.
Gegenprobe Raetsel: sub01 @0x0244C-0x02464 feuert sub10 (Hunde) bei (5,4)=1 (5,5)=1 (5,6)=0 (5,7)=0; die Bits
setzt sub02 (Slot 0) `22 05 04 01` @0x024F4, sub04 (Slot 2) `22 05 05 01` @0x025E4, sub03 (Slot 1) Bit 6
@0x0256C, sub05 (Slot 3) Bit 7 @0x0265C. Die Loesung des Leon-Raetsels ((3,111)=0) ist also Scheibe 0 und 2
vorn — genau die beiden "vielen Einschuesse" des Nutzers. Zensus (3,111)/(3,112) ueber alle RDTs: gesetzt nur
in ROOM1190/1191 sub10 und ROOM1241 @0x055A/0x055E (zweite Variante 1+3, nicht Leons Weg).

### 2.3 Umsetzung
* NEU `include/re15_ziel1190.h` + `engine/src/ziel_1190.c`. Port-Nachrichten **6 = viele, 7 = wenige**
  (VERTRAG 1.2: 1190 -> 6..11). Form: Kopf der Original-Nachricht `04 02`, Seite 1 = Nutzer-Satz (zwei Zeilen,
  Umbruch 0x08), Seitenumbruch `02 00`, Seite 2 = die Original-Nachricht des Platzes Byte fuer Byte (msg 0 mit
  Ja/Nein bzw. msg 2). Belegte Form: ROOM1190 msg 4 @0x2EAF hat genau "Text 02 00 Frage 03 02 01".
  Glyphen re15_msg_glyph; Zeilenbreiten font_width.h: 202/195 px bzw. 174/120 px (<= 271 px).
* Slot = work_vars[0], vom Aktions-Scan gestempelt (FORWARD @0x80042f3c, Port aot_common.c:1507), gelesen im
  selben VM-Takt (sub01 -> Evt_exec sub02..05 laufen im selben Tick; der VM-Schwanz wischt [0..3] danach
  auf -1, FUN_8003ebf4 / scd_vm.c:811). Pruefung zusaetzlich: aktiver Slot mit linker Kante -4400 und Z-Kante
  des Slots. Die Park-Phase der Ja/Nein-Frage haelt ein eigener Latch (der Stempel ist dann schon weg).
* Haken (scd_vm.c, 6 Zeilen + Include): op_message_on VOR dem Ja/Nein-Zweig
  `re15_ziel1190_message_on(t->pc, pause_mask)` (1 = pc+=4, 2 = parken — dieselbe Semantik wie der Zweig);
  re15_scd_show_message (sce-1-Platz) `if (re15_ziel1190_show(index, pause_mask)) return;`.
* Stimme: je Satz EINE Datei (Text wird bei jedem Oeffnen neu unter 6/7 abgelegt, Seite 2 wechselt):
  synchro/STAGE1/room1190/main06.wav, main07.wav (Muster leiche_1110_1230.c: SCD_AUDIO_VOICE_ON).
* NUTZER-VORGABE: die beiden Saetze. PORT-WAHL: (a) Satz als Seite VOR dem Original (in allen drei
  Raumzustaenden, "ergaenzt" = das Original bleibt); (b) Satzende "." auch beim zweiten Satz (Gleichlauf,
  leiche L8); (c) Ereignis 27 wird NICHT gebraucht (kein Ereignisprogramm, nur Textaustausch).

### 2.4 Messung nachher
Echte exe (DEBUG_JUMP 1190@gp, Spieler 620 vor Slot 0, (4,243)=1, Viereck per RE15_PAD_AT): debug.log
`[ziel1190] ROOM1190 Slot 0 -> Port-Nachricht 6 (viele Einschuesse) + Original msg 0`; Bild
`H_raeume/p2_exe_slot0_seite1_seite2.png`: F150 Seite 1 tippt, F240 "This target has a surprisingly / large
number of bullet holes." mit Weiter-Pfeil, F330 Seite 2 tippt, F420 "There's a switch here. Push it?  Yes No".
⛔ Messfalle (nur Harness): Einzelbild-Flanken aus RE15_PAD_AT gehen unter RE15_FRAMEDUMP verloren, wenn
mehrere Spielschritte zwischen zwei Text-Takten liegen (gemessen mit temporaerer Sonde: im Seiten-Warten kam
die Flanke an, sobald sie 4 Bilder lang gedrueckt wurde; ein voriger Lauf mit einer Einzelflanke blieb stehen).
Der STATE_LOG-Wert fsm= ist dafuer kein Mass (zeigte durchgehend 1, waehrend das Bild Seite 2 und die Frage
zeigte). Temporaere Sonde wieder entfernt (git checkout).

### 2.5 Test
`unit_r35_raeume_ziel` (tests/unit/test_r35_raeume_ziel.c): echte ROOM1190/1191, Raumaufbau wie im Spiel,
Aktion per re15_aot_scan, VM + Text-FSM Bild fuer Bild. 3 Zustaende x 4 Slots (+1191): Slot 0/2 -> msg 6,
Slot 1/3 -> msg 7; Text = Nutzer-Satz + Original-Seite; Strom aus: Frage bleibt Frage, nach Ja folgt msg 3;
Strom an: nach Ja kippt (4,n) (Scheibe faehrt, Original-Mechanik intakt); nach Hunden: keine Frage.
Gegenprobe ohne Stempel (re15_aot_fire_slot): Original-Nachricht bzw. nichts (sce 5 = NOP @0x8004318C).
Aufbau-Bytes: Seite 2 == msg 0 Byte fuer Byte, `02 00` an Stelle 62, Laenge <= 128 (PSX MSG_RAW_LEN).

---

## Punkt 3 — ROOM1200: Zombie von der Bahre laeuft durch die Luft

### 3.1 Messung vorher (Basisstand, echte exe)
Messschiene NEU `RE15_GEGNER_Y_LOG` (trage_1200.c, je Bild x/y/z/Band je Gegner). Echter Weg: DEBUG_JUMP
1200@gp, Spieler vor dem Minidisc-Platz (-25880,-16450), Aktion (RE15_PAD_AT) -> Item-Modal -> Ja -> sub03.
ROOM1200 SCD: main00 Sce_em_set id 0 @0x00856 (grid 0x88, Band 0), **id 1 @0x0086A (Typ 0x10, grid 0x87,
pc[4]=1 -> Band 1, y=-1800, (-24249,-18579))**, id 2 @0x0087E (grid 0xA1 = Schubladen-Kriecher, Band 1, y
-1800), id 3 @0x00892 (0x87, Band 0). sub03 @0x009F4: nach Ja ((9,104)=1) Evt_exec sub02 (id 2 -> 0x81,
Schublade obj 1 faehrt) und Member_set id 0 -> 0x8A, id 1 -> 0x89 @0x00A26, id 3 -> 0x89.
Gemessen, beide Geschmaecker (`H_raeume/p3_vorher_re15_luft_F200-700.png`, `p3_vorher_re2_luft_F200-700.png`,
Cut 2 per RE15_FORCE_CUT): Slot 2 (= id 1) wacht bei F210 auf (grid 0x89), steht auf und laeuft danach
600+ Bilder lang auf **y=-1800 / Band 1** durch den Raum (RE1.5: bis (-25622,-1800,-17706), RE2: bis
(-22368,-1800,-18297)) — im Bild schwebt er oben links ueber dem Schrankblock. Slot 3 (Schubladen-
Kriecher) kriecht unter RE1.5 ebenso auf -1800 (unter RE2 steht er still, s. OFFEN).

### 3.2 RE-Belege — der Original-Mechanismus ist die Engine-Schwerkraft FUN_8001bd60
* Zombie-Wurzel FUN_80100424 (STAGE1.BIN, selbst disassembliert), VOR Steer (@0x80100538) und Dispatch
  (@0x80100588), jedes Bild nach den Gates:
  `801004dc addiu a0,zero,-10` (Delay-Slot) / `80100514 jal 0x8001bd60` / `80100518 ori a1,zero,0x14`.
  Der Port fuehrte den Aufruf als "func_0x8001bd60(-10,20) setup helper — deferred" (enemy_ai_common.c:213)
  bzw. "look aux ... unmodeled port-wide" (:10690) — er fehlte komplett.
* FUN_8001bd60 (PSX.EXE): `8001bd64-80` DAT_800aca3c & 0x4000 -> Erkennung ueberspringen;
  `8001bd94-a8` r = FUN_8003b7f0(&+0x34, -(*(+0x78)+6) [`subu a1,zero,a1`], +0x82);
  `8001bdb0` andi r,2 -> sonst nichts; `8001bdc8-ec` nur wenn y == -(+0x82*1800);
  `8001bdf0-be14` +0x1c0 = 0x8000 | ((r&0xc)>>2)<<13; `8001be18-54` do { +0x1ba += 1800; +0x82 -= 1 }
  while (n-- != 0); Fall-Teil `8001be64-a4`: wenn +0x1c0 & 0x8000: y += a0 + a1*(+0x1c0 & 0x1fff),
  +0x1c0 += 1; `8001beb4-e8` wenn +0x1ba < y: y = +0x1ba, +0x1c0 &= 0x7fff.
  Weitere Aufrufer: 0x8010a9b8 (Zombie-Maedchen), 0x8010c288, 0x8011c5ec, 0x8011cbbc, 0x8011d1e4,
  0x8011d778, 0x8011dcc4, 0x8011e29c (s. OFFEN).
* +0x1ba-Seed: Sce_em_set FUN_800420a0 `lbu v1,2(s2)` (pc[4]) @0x800421d4, Faktorfolge @0x800421f8-0x8004220c
  = -(pc[4]*1800), `sh v0,442(s0)` @0x80042210. Radius: +0x78 = 0x8011f778 (`lw v0,-2160` @0x80100770 /
  `sw v0,120` @0x80100778), Wort +6 = 0x0190 = 400.
* Zellen ROOM1200 (SCA-Dump, 12-B-Zellen ab RDT+0x20): Band-1-Zellen 11/12/13/17 tragen u1 = 0x02 (Wort
  0x1302: Absturzkante, n = 0 -> ein Band): 13 = x -26400..-22570 z -17351..-16251, 12 = x -23300..-22000
  z -26300..-16240, 11 und 17 analog; die Bloecke 8/9/10/14/15/16 sind Band-1-Waende (u0 0xFF). Der Liege-
  platz (-24249,-18579) liegt in KEINER Band-1-Zelle (dort ruht er), jeder Weg hinunter kreuzt eine Kante.
  Zensus aller 240 RDTs: Absturzkanten nur auf Band >= 1 (34 Raeume), keine auf Band 0 -> kein Unterlauf.

### 3.3 Umsetzung
* NEU in `engine/src/trage_1200.c` / `include/re15_trage1200.h`: `re15_schwerkraft_8001bd60(e, a0, a1, radius)`
  Zeile fuer Zeile wie oben; Zellwort ueber das vorhandene `re15_collision_floor_typeword` (= FUN_8003b7f0
  mit Band/Radius), Standobjekt-Gate = `re15_climb_dbg_standing() >= 0` (= DAT_800aca3c & 0x4000,
  climb_common.c @0x80031d24). Konstanten: -10 @0x801004dc, 0x14 @0x80100518, 1800 @0x8001be2c,
  0x8000 @0x8001bdf0, 0x1fff @0x8001be7c, 0x7fff @0x8001bee4.
* `include/re15_actor.h`: Feld `fall_1c0` (+0x1c0) hinter dog_floor_y (+0x1ba, der Port-Platz dieses Felds).
* `engine/src/scd_vm.c` Sce_em_set (1 Zeile): `a->dog_floor_y = -(pc[4]*1800)` @0x80042210.
* `engine/src/enemy_ai_common.c` re15_enemy_ai_live_tick (1 Aufruf): hinter dem Abstand, VOR Steer und
  Dispatch = die Original-Reihenfolge @0x80100514. Gilt fuer die Wurzel FUN_80100424 (Typen 0x10/0x11/0x12/
  0x16/0x18) in BEIDEN KI-Geschmaeckern — PORT-WAHL fuer RE2: die Raumdaten (SCA-Baender) sind RE1.5, die
  Schwerkraft ist Engine (EXE), nicht KI; genauso faehrt der Port die RE1.5-SCA-Klemme in beiden Geschmaeckern.

### 3.4 Messung nachher (echte exe, gleicher Weg)
RE1.5 (`RE15_AI_FLAVOR=re15`, Spieler geht nach dem Aufnehmen weg, Bild `H_raeume/p3_nachher_re15_sturz_F374-410.png`):
Slot 2 laeuft bis F380 auf Band 1, betritt die Kante (Zelle 13) -> F380 y -1810, F381 -1800, F382 -1770,
-1720, -1650, -1560, -1450, -1320, -1170, -1000, -810, -600, -370, -120, F394/395 **y=0 Band 0**, Fallwort
0x000F = exakt -10 + 20*t; danach geht er Leon auf der Spieler-Ebene an (Sub 3/5 Griff). Im Bild sieht man
ihn von der Schrankoberkante herunterfallen und neben Leon landen.
RE2 (Default-Geschmack): Slot 2 erreicht die Kante spaeter (der RE2-Zombie steht zuerst am Rand und beisst
nach unten, Sub 14), faellt bei F~680 und geht auf y=0 weiter (`p3_nachher_re2_F300-650.png`, Log) — das galt
nur, wenn Leon sich entfernt; bleibt er direkt unter der Kante, stand der Zombie bis Nachbesserung 1 oben (M1).

### 3.5 Test
`unit_r35_raeume_trage` (tests/unit/test_r35_raeume_trage.c): (A) Funktion an den echten ROOM1200-Zellen —
Kante: 15 Bilder Sturz -1800 -> 0 mit -10+20t, +0x82 1->0, +0x1ba -1800->0, Fallwort 0x8001 nach Bild 0
und 0x000F nach der Landung; Liegeplatz / Rand innerhalb des Radius / y != -(Band*1800): kein Sturz;
n=1 (synthetisches Wort 0x1306) -> zwei Baender. (B) echter Weg (main00-Spawns, Weckwert 0x89 wie sub03,
EM10- bzw. RE2-EM010-Bank geladen, KI + Animation je Bild): RE1.5 Sturz Bild 152-166, RE2 132-146, beide
landen auf y=0 / Band 0, Fallfolge exakt. (C) Seed +0x1ba = -(Band*1800) fuer alle Raumgegner.

---

## Punkt 4 — ROOM1210 Gitterarme: Griff nicht synchron zu Leons Schuetteln (Clipping)

### 4.1 Messung vorher (Basisstand, echte exe, KI-Default RE2 -> EM2D-Arme, Leon aus der EM2D-Opferbank)
Lauf DEBUG_JUMP 1210@gp, Leon (-20622,-14600) rot 1024, RE15_FORCE_CUT=4, RE15_ANIM_TRACE, RE15_RE2_TRACE
(re2_ki.log). Arm Slot 5 (Westreihe, z -15747) greift (Sub 4 = HALTEN, Clip 5, 19 Bilder), Leon im
Opfer-Clip 0 (19 Bilder). ANIM_TRACE Bild fuer Bild: Leon zeigt durchgehend **Arm-Bild + 1** (Arm 0..18 /
Leon 1..18,0). Bilder `H_raeume/p4_vorher_cut4_F34-56.png` (Cut 4) und obere Reihe von
`p4_griff_vorher_oben_nachher_unten_F56-71.png`: die Zombiehaende fahren Leon durch Kopf/Oberkoerper.
PIN-Messschiene (neu, RE15_RE2_TRACE, B4 P0): `PIN slot 5 yaw 3664: Parts-Pose Clip 3 Bild 4 -> (-20588,
-15148); Clip 5 Bild 0 (bisher) -> (-20728,-15056)` — der Port stellte Leon **168 Einheiten** neben den
Punkt, den das Original nimmt.

### 4.2 RE-Belege (RE2 EM2D-Overlay CDEMD0_EM2D_ai1.BIN @0x80100000, Volltext
`analysis/befunde_2026-09-19/arme-1210-re2_em2d_ai1.dis`; RE2 PSX.EXE)
* B4 P0 @0x80100BC4: `ori v0,v0,0x5 / sw v0,332(s0)` (Clip 5) @0x80100BC8-CC, Hand-Part = Tabelle
  `lbu v1,5142(at)` @0x80100BEC (0x80101416 + var*7), Part-Stride 0xAC (@0x80100BF4-C10), `lw v1,408(s0)`
  (+0x198 Parts) @0x80100C0C, **`lw v0,92(v1)` -> `sw v0,0x800CFC30` (PL.x) @0x80100C18-20, `lw v0,100(v1)`
  -> `sw v0,0x800CFC38` (PL.z) @0x80100C24-38**. P0 ruft KEINEN Advance: Ende `j 0x80100D6C` @0x80100CAC.
* Die Parts-Matrizen baut nur 0x8002959C -> 0x80029614 (`jal 0x80029614` @0x800295FC); 0x8002959C setzt
  +0x178 auf das Bild **+0x14D vor dem Zaehlen** (`lbu v0,333(a0)` @0x800295E8, `sw a2,376(a0)` @0x800295F8),
  gezaehlt wird erst @0x80029B30. Also liest P0 die Hand der Pose, die B3 P1 (`jal 0x8002959C` @0x80100B24)
  im VORTAKT gebaut hat (Clip 3, Startbild des Vortakts). Ebenso A1 (Hand < 900 @0x801007C4) und A3
  (Hand < 600 @0x801009BC): FUN_800157D4(&PL, part[Hand]+0x5C, r).
* Spieler-Hook P0 @0x801012A8: Clipwort 0x000F0000 (`lui v0,0xf / sw v0,332(s1)`), FUN_80015910(PL, Greifer)
  @0x801012E0 = `(Greifer+0x76 - PL+0x76 + 0x400) & 0xFFF < 0x800` (RE2 PSX.EXE @0x80015910-2C), danach
  FUN_80015558(PL, Greifer.x, Greifer.z, 2048) @0x801012F8, bei 1: PL+0x76 += 2048 @0x80101304-18, Advance
  @0x80101328. Spieler-Routine 5 laeuft nach allen Entities (@0x80026620) — daher Leon = Arm + 1 (Arm zaehlt
  erst ab P1 @0x80100D18). Port-Takt war darin bereits byte-true.

### 4.3 Umsetzung
* `engine/src/enemy_ai_re2_zellenarm.c`: Pose-Merker `pose_clip/pose_frame` je Arm = was die Parts tragen;
  `arm_adv()` merkt (Clip, Bild) VOR dem Zaehlen und ruft re15_re2_advance_959c (alle sieben Advance-Stellen
  @0x80100370/6D0/740/950/B24/D18/E08/F00); `arm_hand_pose()` rechnet part[Hand] aus dieser Pose. Genutzt in
  A1 (900), A3 (600) und B4 P0 (Pin @0x80100C18-38). Messschiene `[re2arm] PIN` (RE15_RE2_TRACE).
* `tests/unit/test_p2_1210_arme_re2.c` (4d): Erwartung von "Hand der aktuellen Pose" auf "Hand der Parts-Pose"
  umgestellt (Startbild-Historie, Kommentar Runde 35 Spur H) — die alte Pruefung hielt den Fehler fest.

### 4.4 Messung nachher
* PIN (exe, gleicher Lauf): Leon steht auf der Parts-Pose-Hand (-20588,-15148) statt (-20728,-15056).
* Clipping-Mass (probe_r35_raeume_arme + Riegel; waagerechter Abstand Arm-Hand zu Leons Brustachse,
  PL00-Knochen 8 im Opfer-Renderpfad, Leon = Arm + 1, Ueberblendung abgewartet):
  | Fall | vorher (Clip 5 Bild 0) | nachher (Parts-Pose) |
  |---|---|---|
  | Leon dem Arm zugewandt (kein Flip) | Hand < 120 in 9 von 19 Bildern, min 33 | **0 von 19 (Riegel: 0 von 44), min 169-170** |
  | Leon mit dem Ruecken zum Arm (Flip +2048) | 2 von 19 (vorzeichenbehaftet: 17 von 19 nicht klar hinter der Brust) | 11 von 19 (Riegel 24 von 44; vorzeichenbehaftet 14 von 19) |
* Bild `p4_griff_vorher_oben_nachher_unten_F56-71.png` (Cut 4, Gesicht-Fall): die Hand liegt nachher seitlich
  an Kopf/Schulter statt durch das Gesicht.
* Phasen-Gegenprobe (Sonde): im Gesicht-Fall waere der Gleichlauf bei Leon = Arm + 6..10 am saubersten (0
  falsche Bilder), byte-true ist + 1 (5 von 19 "Hand nicht klar vor der Brust") — der Port bleibt beim Original.

### 4.5 Tests
`unit_r35_raeume_arme` (tests/unit/test_r35_raeume_arme.c, echter Weg game_step, ROOM1210 + RE2-EM2D + PL00):
(1) Pin = Parts-Pose-Hand (Abstand 0) und nicht Clip-5-Hand (167 daneben), beide Blickfaelle; (2) jedes
Halte-Bild Leon = Arm + 1; (3) Gesicht-Griff: 0 von 44 Halte-Bildern mit Hand < 120 an der Brustachse
(Minimum 170). Ruecken-Griff gemessen und protokolliert (24 von 44, s. OFFEN).
`unit_1210_arme_re2` angepasst und gruen. Mess-Sonde `probe_r35_raeume_arme` (kein add_test).

---

---

## Suite
`bash re15_port/tools/local_build.sh all` am Endstand (nach ae4162c8, Seed nur Zombie-Wurzel-Typen):
`=== LOCAL-BUILD-OK (all) — Tests 482/482` (Schranke 478; neu: unit_r35_raeume_hundeschatten,
unit_r35_raeume_ziel, unit_r35_raeume_trage, unit_r35_raeume_arme; angepasst: unit_1210_arme_re2 (4d)).
Ein Vorlauf mit globalem +0x1ba-Seed war ebenfalls 482/482; der Seed wurde trotzdem auf die Zombie-Wurzel-Typen
begrenzt, weil dog_floor_y bei RE2-Spinne (0x25, ROOM2030/2050/2060/20A0 Band 2-3) und RE2-Kraehe +0x1C2 ist.

## Fuer den Nutzer
* **Sprachdateien (neu, optional — ohne Datei laeuft der Text stumm mit Untertitel):**
  * `synchro/STAGE1/room1190/main06.wav` — Leon: "This target has a surprisingly large number of bullet holes."
  * `synchro/STAGE1/room1190/main07.wav` — Leon: "This target does not have many bullet holes."
  (Seite 2 der Nachricht ist der unveraenderte Originaltext des Platzes, ohne Stimme.)
* **Neue Assets fuer das Paket-/Android-Gate:** keine (alles im Code, keine Datei unter shared_assets/).
* **Bedienung / was man sieht:**
  * ROOM1190: an den vier Scheiben-Schaltern (Kabinen 1..4, von links) kommt zuerst der Scheiben-Satz, dann wie
    bisher "There's a switch here. Push it?" (bzw. nach den Hunden "I have nothing else to do here.").
    Scheibe 1 und 3 von links = viele Einschuesse = genau die beiden Scheiben, die fuer Leons Raetsel vorn
    stehen muessen.
  * ROOM1190 Hunde: beim Sprung durch die Luke haengt kein Schatten mehr in der Luft (er liegt am Raumboden).
  * ROOM1200: der Zombie von der Bahre faellt beim Herunterlaufen an der Kante auf den Boden und kommt auf
    Leons Ebene (in beiden KI-Einstellungen). Bleibt Leon nach dem Aufnehmen direkt unter der Kante stehen,
    schnappt der Zombie (RE2-KI) einmal von oben nach ihm, geht dann los und faellt herunter (Nachbesserung 1).
  * ROOM1210: Leon wird beim Griff an die Stelle gestellt, die das RE2-Original nimmt; von vorn gegriffen
    steckt keine Zombiehand mehr in seinem Oberkoerper. Von hinten gegriffen (Leon schaut beim Zupacken vom
    Fenster weg) liegt die Hand an bzw. in Leons Brust — das ist im RE2-Original genauso gebaut: dieselbe
    Opfer-Animation, nur Leon um 180 Grad gedreht (Nachbesserung 1, M2). Unter der KI-Einstellung RE1.5 steht
    Leon beim Griff ein Stueck vom Arm entfernt (kein Kontakt, OFFEN 6).
* **Messschienen (env, kein Spielverhalten):** `RE15_HUNDESCHATTEN_LOG=<datei>`, `RE15_GEGNER_Y_LOG=<datei>`,
  `[re2arm] PIN` in re2_ki.log bei `RE15_RE2_TRACE=1`.

## OFFEN
1. **ROOM1210 Ruecken-Griff (Punkt 4) — Sichtpruefung am RE2-Original.** Statisch belegt (Nachbesserung 1,
   M2): RE2 hat fuer den Griff von hinten keinen eigenen Pfad (eine Opferbank @0x80100C3C-5C, ein Clip
   @0x801012A8-AC, nur der Flip @0x8010130C-18; der Arm liest PL+0x76 nie); der Riegel prueft diese
   Konstruktion (4a-c). Ein BILD eines RE2-Ruecken-Griffs fehlt. Naechster Messweg: pcsx-redux mit
   `C:/Users/mjoedicke/Downloads/ePSXe2018/re2leon.cue`, Raum der EM2D-Gitterarme, Leon so an einem Arm
   vorbei, dass `((Arm.yaw - PL.yaw + 0x400) & 0xFFF) < 0x800` (FUN_80015910), PL+0x38/+0x40/+0x76 + Bild im
   Halten. (Ein Versuch, den Ruecken-Griff in der Port-exe per Eingabeskript zu provozieren — Laeufe
   m2_back_a/b, `U3.6,R1.4,D2.5/3.5` — erreichte den Arm nicht; der Riegel deckt den Fall ab.)
2. **RE1.5-Flavor der Gitterarme** (EM01A + Opferbank-Leihgabe vom Zombie 0x10): gemessen in Nachbesserung 1
   (M4) — kein Clipping, s. OFFEN 6.
3. **Schubladen-Kriecher ROOM1200 (Slot 3, grid 0x81) unter RE2** steht nach dem Wecken still (Sub 2, Clip 23)
   — nicht Teil des Nutzerbefunds (der laufende Bahren-Zombie ist id 1), unter RE1.5 kriecht er und faellt
   jetzt ebenfalls an der Kante (Zelle 17). Naechster Weg: RE2-Kriecher-Wurzel 0x80101210 gegen grid 0x81.
4. **Weitere Aufrufer von FUN_8001bd60** (Zombie-Maedchen @0x8010a9b8, 0x8010c288, NPC-/Typ-0x47-Wurzeln
   0x8011c5ec/0x8011cbbc/0x8011d1e4/0x8011d778/0x8011dcc4/0x8011e29c) sind weiter ohne Schwerkraft — gleiche
   Funktion, je eine Zeile in der jeweiligen Wurzel; nicht Teil dieser Spur.
5. **Zielscheiben, zweite Raetsel-Variante** ((3,112)=1, nur aus ROOM1241 @0x055E): dort waeren Scheibe 1+3
   die Loesung; die Texte bleiben nach Nutzer-Vorgabe fest auf 0+2 = viele.
6. **ROOM1210 Gitterarme unter RE1.5-KI: Griff ohne Kontakt** (Nachbesserung 1, M4, Bild
   `H_raeume/nb1_m4_re15ki_griff_F276-320.png`): Leon steht im Halten 1300-1560 Einheiten neben der Hand und
   spielt die geliehene Zombie-Opfer-Animation. Ursache: der RE1.5-Writher hat im Original keinen Griff
   (@0x8010c8cc-f4: nach der Lunge +0x5 := 2/3); der Port-Griff dieses Geschmacks ist eine Nachruestung mit dem
   Zombie-Wurzelversatz relativ zum Hand-Anker. Das RE2-Ziel (EM2D: Pin auf die Hand @0x80100C18-38, Opferbank
   des Arms @0x80100C3C-5C) faehrt der Default-Geschmack. Naechster Schritt (Nutzer-Entscheid noetig, ob der
   RE1.5-Geschmack ueberhaupt greifen soll): entweder ohne Griff wie das RE1.5-Original (Greifen-Schleife und
   Zurueck), oder den RE2-EM2D-Griff auch fuer EM01A (Pin auf die EM01A-Hand, Leon zur Arm-Wurzel gedreht).
7. **RE2-Sichtstrahl ohne Hoehenteil:** RE2 0x80050858 prueft bei a3=1 (Zombie-Navigator) zusaetzlich die
   Hoehe der Saetze (@0x80050b00-74 und die YZ-/YX-Projektionen danach). Der Port-Stand-in kennt nur das Band
   des Gegners (Zell-Strahl) bzw. +0x82 (Region-Ray) und hat jetzt den Maskenfilter (@0x800508bc-c8). Folge:
   eine Band-1-Wand blockt die Sicht eines Band-1-Zombies auch dort, wo der Strahl im Original unter ihr zu
   Leon hinab laeuft (Sonde: (-24300,-12500) durch Zelle 9). Fuer ROOM1200 nicht noetig (Abstieg gemessen);
   naechster Schritt: Band -> Hoehenbereich [-(b+1)*1800, -b*1800) und Teilhoehen der Strahl-Enden.

---

## Nachbesserung 1 (nach Abnahme 0, 2026-10-04)

Maengel aus `H_abnahme_0.md`: M1 (P3 Bahren-Zombie bleibt unter RE2-KI in der Nutzerlage oben stehen),
M2 (P4 Ruecken-Griff clippt), M3 (veraltete Kommentare enemy_ai_common.c:214/:10695), M4 (P4 RE1.5-KI der
Gitterarme ungeprueft). Je Mangel: Ursache / Messung vorher / Beleg / Aenderung / Messung nachher.

**Stand je Mangel:**
| Mangel | Ergebnis |
|---|---|
| M1 | **behoben** — Ursache war der Port-Sichtstrahl (Region-Ray ohne Maskenfilter sah die begehbare Absturzkante als Wand, los=0 in jedem Bild); Filter nach RE2 @0x800508bc-c8. Nutzerlage: Biss F2062, Gang F2095, Sturz F2118-2132 (vorher 1011 Bilder oben). Riegel um "Spieler unter der Kante" + Sichtpruefung erweitert, Gegenprobe rot. |
| M2 | **widerlegt per Adresse** — RE2 hat fuer den Griff von hinten keinen eigenen Pfad (eine Opferbank @0x80100C3C-5C, ein Clip @0x801012A8-AC, nur Flip @0x8010130C-18, Arm liest PL+0x76 nie); Riegel prueft die Konstruktion (4a-c, Hand-Bahn max 0, Spiegel max 2). Sichtpruefung am RE2-Abbild bleibt OFFEN 1. |
| M3 | **behoben** — Kommentare enemy_ai_common.c:214/:10695/:14669. |
| M4 | **gemessen** — RE1.5-KI: kein Clipping; Leon steht im Halten 1300-1560 neben der Hand (Griff ohne Kontakt, Nachruestung ohne Original, @0x8010c8cc-f4) -> OFFEN 6. |

### M3 (Doku) — erledigt
enemy_ai_common.c:214 ("func_0x8001bd60(-10,20) setup helper — deferred") -> jetzt: Engine-Schwerkraft,
portiert als re15_schwerkraft_8001bd60, aufgerufen in re15_enemy_ai_live_tick (`jal` @0x80100514).
:10695 (Zombie-Maedchen, "unmodeled port-wide") -> "laeuft bisher nur in der Zombie-Wurzel, hier noch nicht
— OFFEN". :14669 (NPC-Wurzel "look helper 0x8001bd60") -> "Schwerkraft 0x8001bd60 [hier noch nicht portiert]".
Commit e83b0741.

### M1 (P3, RE2-KI, Spieler direkt unter der Kante)
**Messung vorher** (Lauf `m1_D_vorher` = Abnahme-Lauf p3c_re2_D: DEBUG_JUMP 1200@gp, Spieler (-25880,-16450)
Blick 2048, Aktion+Ja per RE15_PRESS F1800-2003 (Bilder zaehlen ab Raumladen), danach RE15_INPUT_SCRIPT=D3
ab F2100; `RE15_GEGNER_Y_LOG` jetzt mit `rot/los/t158/t15a/cd`):
* Slot 2 (= id 1) F2010 Sub 1 (Gang) auf Band 1, F2070 Sub 14 (Schnappbiss, Clip 0x11), ab F2100 Sub 0 P1
  (`st=1/0/1`, Clip 0) bei **(-24049,-1800,-17373)**, cd 60 -> 0, t158 -> 0 — und **los=0 in JEDEM Bild**
  des Raums (auch vor dem Wecken und nach der Landung).
* Sub 0 kommt nur ueber das Sicht-Bit weiter: DECISION[0] Block 1 `dist<0x1388 && a1024==0 && +0x154&0x800
  -> 0x101` (@0x80101308-1C) und der Selbst-Wecker P1 `t158==0 && dist<0x1D4C && +0x154&0x800 -> 0x101`
  (@0x80101544-7C). Ohne Sicht bleibt nur der Wander-Wurf (50 % alle 300..555 Bilder, @0x8010151C-40) —
  daher "kommt nie" (Abnahme, 1011 Bilder) bzw. "nach ~470 Bildern" (LU/RU) bzw. hier F2370 per Sub 9.
* Sonde `probe_r35_raeume_trage_los` (neu, kein add_test; zerlegt re15_re2_los_clear an den echten
  ROOM1200-Zellen, Zombie (-24049,-17373) Band 1, Maske 4): der Zell-Strahl (u0 & Maske, Gegnerband) ist fuer
  alle Spielerplaetze vor der Kante **frei**; geblockt wird vom zweiten Teil, dem RE1.5-Region-Ray
  (FUN_8003dcc4-Stand-in), und zwar an **Zelle 13** (Spieler (-25788,-16450), (-25880,-16450), (-24049,-16000),
  (-25000,-16800)) bzw. **Zelle 12** ((-22685,-17314)) — beide `u0 01 u1 02` = die Absturzkanten, fuer den
  Zombie (Maske 4) begehbar. Er "sieht" also die Kante, auf der er gleich hinunterlaufen soll, als Wand.

**Beleg (RE2 PSX.EXE, selbst disassembliert, re2_disasm.py):** der Strahl, den der Port hier nachbildet, ist
0x80050858 (aus dem Navigator FUN_8004A808, Maske 0x2000, a3=1). Satzschleife:
```
800508b0: addiu t1,t1,16          ; naechster Satz (16 B)
800508b4: beq   t1,s6,0x80050f50  ; Ende -> 0 (frei)
800508bc: lhu   v1,8(t1)          ; Attribut des Satzes
800508c4: and   v0,v1,fp          ; fp = a2 = Maske des Aufrufers
800508c8: beq   v0,zero,0x800508b0; KEIN Maskentreffer -> Satz zaehlt nicht
800508d0: andi  v0,v1,0xf / 800508dc: lbu v0,29620(at) ; Form-Skip-Tabelle 0x800A73B4
...
80050b00: lw    s6,0(sp)          ; a3
80050b08: beq   s6,zero,0x80050f50 / 80050b0c: addiu v0,zero,1   ; a3=0: XZ-Treffer = blockiert
80050b10-30: Unterkante t5 = -1800 * (nachlaufende Nullen von Satz+0x0C)
80050b34-74: Oberkante t4 = -(w>>11)*100 - ((w>>6)&0x1f)*1800 (w = Satz+0x0A); danach YZ-/YX-Projektion
```
Die Maske ist Teil des Originals (wer nicht kollidiert, verdeckt nicht). Der Port-Zell-Strahl bildet das als
`u0 & Maske` ab (re15_re2_los_cells_blocked); der zusaetzlich UND-verknuepfte Region-Ray hatte den Filter
nicht — das ist der Defekt (Port-Mapping, kein RE2-Verhalten).

**Aenderung:** enemy_ai_common.c re15_los_ray_blocked bekommt einen Parameter `re2_maske`; nur der RE2-Aufruf
(re15_re2_los_clear, Zombie + Kraehe — beide laufen im Original durch dieselbe Satzschleife) uebergibt die
Maske des Aktors (+0x1D7, Default 4) und ueberspringt Zellen ohne `u0 & Maske` (@0x800508bc-c8). Der
RE1.5-Sensor FUN_8001bc08 ruft mit 0 = unveraendert FUN_8003dcc4. Commit d0684bcc.

**Messung nachher** (gleicher Lauf, neue exe; `m1_D_nach`, `m1_still_nach` (keine Eingabe), `m1_LU_nach`
(`L0.7,U4`) — alle drei Bild fuer Bild gleich, weil Slot 2 Leon packt, bevor die Eingabe ab F2100 wirkt):
* Sonde: alle Plaetze unter der Kante `los_clear=1`; nur die Band-1-Wand Zelle 9 (u0 FF) blockt weiter
  ((-24049,-15000), (-24300,-12500)).
* Lauf: `los=1` durchgehend. F2062-2094 Sub 14 (Schnappbiss, cd 60), **F2095 Sub 1** (Gang, Sicht -> 0x101),
  F2101 betritt Zelle 13, **F2118 Absturzkante** `@(-24487,-1810,-16917) b0 f1ba=0 f1c0=8001`, Folge -1810,
  -1800, -1770, -1720, -1650, -1560, -1450, -1320, -1170, -1000, -810, -600, -370, -120, **F2132 y=0
  f1c0=000f** (= -10+20t, 15 Bilder), danach Sub 3 (Griff) auf Leon. Vorher: 1011 Bilder oben (Abnahme) bzw.
  F2370 nur per Partner-Stoss Sub 9 (m1_D_vorher).
* Beobachtung (kein Eingriff): ab F2127 (y=-1000, noch im Fall) waehlt die RE2-Leiter Block G (Griff,
  `e->floor == pl->floor` @0x80102140) — weil FUN_8001bd60 +0x82 schon an der Kante senkt (@0x8001be4c-54),
  bevor der Koerper unten ist. Das ist die Original-Reihenfolge beider Funktionen; der Griff greift in der
  Landephase (Sub 3 P1/P2 bis F2132, dann P3 am Boden).

**Test:** `unit_r35_raeume_trage` erweitert — (D) Sicht von der Kante (-24049,-17373) Band 1 zu 5 Plaetzen
unter der Kante = frei, Band-1-Wand Zelle 9 blockt; (B') "Spieler unter der Kante" (steht am Minidisc-Platz
(-25880,-16450), Blick 2048) fuer RE2 und RE1.5: Sturz < Bild 600 + Landung y=0 Band 0 + Folge -10+20t.
Ergebnis: RE2 Sturz Bild 218, Landung 232; RE1.5 Sturz 168, Landung 182; (D) 5/5. **Gegenprobe** (Filter im
RE2-Aufruf abgeschaltet, Maske 0): 8 FAIL — (D) 5x geblockt, RE2 unter der Kante "Sturz ab Bild -1" (in 1500
Bildern nie) = genau der Abnahme-Befund. Commit 1f3e68f4.

### M2 (P4, Griff von hinten) — Widerlegung per Adresse: im RE2-Original ist der Ruecken-Griff der um 180 Grad gedrehte Gesicht-Griff
**Ursache/Messung vorher** (Abnahme 0, `probe_r35_raeume_arme`): Leon-Blick 3713 (Ruecken zum Arm, d=1) -> Hand
< 120 an der Brustachse in 11 von 19 Halte-Bildern, Minimum 4; bei jedem Phasenversatz d=0..18 6..16 von 19.
Der Riegel protokollierte das nur.

**Beleg — der Ruecken-Griff hat im RE2-Original keinen eigenen Pfad** (EM2D-Overlay CDEMD0_EM2D_ai1.BIN
@0x80100000, Volltext `analysis/befunde_2026-09-19/arme-1210-re2_em2d_ai1.dis`; RE2 PSX.EXE selbst disassembliert):
* **EINE Opferbank.** B4 P0 (Arm) nach dem Pin: `80100c3c lw v0,392(s0)` (+0x188) / `80100c44 sw v0,-640(at)`
  = 0x800CFD80 = PL+0x188; `80100c48 lw v1,396(s0)` (+0x18C) / `80100c5c sw v1,-636(at)` = PL+0x18C;
  `80100c4c addiu v0,zero,5` / `80100c54 sw v0,-1028(at)` = PL+0x4 (Spieler-Routine 5); `80100c30 sw s0,-596(at)`
  = PL+0x1B4 (Greifer). (PL-Basis 0x800CFBF8: PL.x = 0x800CFC30 = +0x38.) Leon bekommt die Bank des Arms —
  dieselbe fuer jeden Griff.
* **EIN Opfer-Clip.** Spieler-Hook 0x80101258 (Sprungtabelle @0x80100004: P0 0x801012A8, P1 0x8010131C,
  P2 0x80101338, P3 0x8010134C, P4 0x80101374): P0 `801012a8 lui v0,0xf` / `801012ac sw v0,332(s1)` = Clipwort
  0x000F0000 = Clip 0; P1 (Halten) nur `jal 0x8002959c` (@0x80101328) — kein Clip-Wechsel, keine Weiche.
* **Der einzige Unterschied ist der Blick.** `801012e0 jal 0x80015910` (PL, Greifer) -> s0;
  `801012f8 jal 0x80015558` (PL, Greifer.x @0x801012F0, Greifer.z @0x801012F4, a3 = 2048 @0x801012EC);
  `80101304 beq s0,zero,0x80101320` / `8010130c lhu v0,118(s1)` / `80101314 addiu v0,v0,2048` /
  `80101318 sh v0,118(s1)`. FUN_80015558 (RE2 PSX.EXE): Zielwinkel `jal 0x800154ac` @0x8001558C, Differenz
  `(Ziel - yaw + 2048) & 0xfff` @0x800155B8-C0 < `2048<<1` @0x800155C4-CC -> immer `sh v1,118(s1)` @0x800155DC
  = Leon blickt exakt zur Arm-Wurzel; danach +2048 im Ruecken-Fall. s0 ist ein Register des Hooks und wird
  nirgends gespeichert.
* **Die Hand-Bahn haengt nicht an Leons Blick.** Im ganzen Overlay greifen nur @0x8010130C und @0x80101318 auf
  Offset 118 (+0x76) zu (beide im Hook, auf PL). Der Arm liest PL.x/z (0x800CFC30/0x800CFC38) nur vor dem Griff
  (0x80100520-54, 0x801007B8, 0x801008DC-E4, 0x801009B0, 0x80100B04-0C) und schreibt sie im Pin
  (@0x80100C20/38). B4 P1 (Halten, @0x80100CB4-0x80100D68) = Vibration, SE, `jal 0x8002959c` @0x80100D18,
  `jal 0x8001598c` (Schuettel-Zaehler) — kein Bezug auf Leons Lage. (Die Wurzel @0x80100018-38 uebergibt &PL.x
  jedes Bild an den Navigator `jal 0x8004a808` — der fuehrt Sicht-/Wegfelder, die B4 P1 nicht liest.)
=> Im RE2-Original ist der Ruecken-Griff zwingend der Gesicht-Griff mit Leon um seine Wurzel (= Pin-Punkt =
Hand) um 180 Grad gedreht: gleiche Hand-Bahn (Arm-Clip 5), gleiche Leon-Pose relativ zu seinem Blick (Opfer-
Clip 0), Brust an der Wurzel gespiegelt. Die Abnahme-Zahl (11 von 19 < 120) ist damit die geometrische Folge
der RE2-Daten, kein Port-Defekt; eine "Korrektur" haette kein Original-Vorbild (kein zweiter Clip, keine
Lage-Weiche, keine Versatz-Konstante im Overlay). Der Port bildet alle vier Glieder schon byte-true ab
(enemy_ai_re2_zellenarm.c:594-596, Pin @0x80100C18-38, Leon = Arm + 1).

**Aenderung:** keine am Spielcode. Der Riegel prueft den Ruecken-Fall jetzt als Konstruktion statt ihn nur zu
protokollieren (unit_r35_raeume_arme (4), gleicher Arm, gleiche Startlage, nur Leons Blick vor dem Griff
verschieden): (4a) Ruecken-Blick = Gesicht-Blick + 2048; (4b) Wurzel, Bildtakt (Leon-/Arm-Bild) und Hand-Bahn
Bild fuer Bild gleich; (4c) Brust(Ruecken) - Wurzel = -(Brust(Gesicht) - Wurzel).

**OFFEN (Sichtpruefung am RE2-Original):** ein Bild eines RE2-Ruecken-Griffs liegt nicht vor. Fundort fuer den
naechsten Messweg: RE2-Leon-Abbild `C:/Users/mjoedicke/Downloads/ePSXe2018/re2leon.cue` (+ .bin), Emulator
pcsx-redux (Skill re15-pcsx-watchpoint); Arme = EM2D im RE2-Raum der Gitterhaende, Leon parallel zur Wand an
einem Arm vorbei so laufen, dass `((Arm.yaw - PL.yaw + 0x400) & 0xFFF) < 0x800` (FUN_80015910 @0x80015910-2C),
dann PL+0x38/+0x40/+0x76 und den Bildschirm im Halten vergleichen. Nach der Konstruktion oben muss das Bild
dieselbe Ueberschneidung zeigen.

**Messung nachher (Riegel, gebaut):** `unit_r35_raeume_arme` gruen — Gesicht: 44 Halte-Bilder, 0 mit Hand < 120,
Minimum 170; Ruecken: 24 von 44 < 120, Minimum 5; **(4) 44 Bilder verglichen, Blick 57 / 2105 (= +2048),
Hand-Abweichung max 0, Spiegel-Abweichung max 2** -> (4a) (4b) (4c) ok. Die Ueberschneidung im Ruecken-Griff ist
also exakt die an Leons Wurzel gespiegelte Brust des sauberen Gesicht-Griffs bei unveraenderter Hand-Bahn —
das, was die RE2-Konstruktion vorschreibt.

### M4 (P4, Gitterarme unter RE1.5-KI) — gemessen
**Messung** (echte exe, Tuer ROOM1220 -> ROOM1210 wie die Abnahme: `RE15_DEBUG_JUMP=1220@gp
RE15_PLAYER_POS=-21750,-6400,0 RE15_PAD_AT=45:A,46:A`, dann `RE15_INPUT_SCRIPT_BASIS=spiel START=80`):
* Lauf `m4_re15` (`RE15_AI_FLAVOR=re15`, Abnahme-Eingabe `R0.5,W0.3,U14,R0.7,U8`): im Vorbeilaufen KEIN Griff —
  Arm 5 (-25000,-15747) faehrt F255 aus (Sub 1, Lunge bis x -22600), Arm 6 F301, Leon laeuft durch.
* Lauf `m4_re15_stand` (`R0.5,W0.3,U4.8`, Leon bleibt bei (-20622,-15200) vor Arm 5 stehen): **Griff F277**
  (Arm 5 Sub 4 bei (-22580,-15747), Clip 1 Schleife, 76 Bilder bis Abwurf F353). Leon springt im Griff-Bild auf
  (-19659,-15477) und pendelt waehrend des Haltens zwischen x -19064 und -19317 (z -15311..-15382) — die Hand
  (Anker = Arm + MESH_REACH 1671 entlang +0x6a, auf die begehbare Flaeche geklemmt, also an die Flurkante
  x ≈ -20622) liegt **rund 1300-1560 Einheiten** neben Leons Wurzel. Bild
  `H_raeume/nb1_m4_re15ki_griff_F276-320.png` (Framedump, 3x): der bleiche EM01A-Arm haengt im Gitterfenster,
  Leon steht mitten im Flur und spielt die geliehene Zombie-Opfer-Animation (Haende nach vorn) **ohne Kontakt**
  — keine Hand im Koerper (kein Clipping), aber auch kein Griff, den man sieht; Gleichlauf gibt es nicht
  (EM01A Clip 1 = 39 Bilder Schleife gegen den geliehenen Opfer-Clip).
* Gegenlauf `m4_re2_stand` (Default RE2, gleiche Eingabe): Griff F244, Pin (-20588,-15148) = Parts-Pose-Hand,
  Hand an Kopf/Schulter (Bild `H_raeume/nb1_m4_re15ki_oben_re2ki_unten.png`, untere Haelfte).

**Einordnung (belegt, nicht geaendert):** der RE1.5-Writher hat im Original KEINEN Griff — selbst
disassembliert (STAGE1.BIN): `8010c8cc jal 0x8001af20` (rng) / `8010c8d4 andi v0,v0,0x1` / `8010c8e0 addiu v0,v0,2`
/ `8010c8e4 sb v0,5(v1)` / `8010c8f4 sb zero,6(v0)` = nach der Lunge +0x5 := 2 oder 3 (Greifen-Schleife bzw.
Zurueck), kein Spieler-Kommando; der ganze RE1.5-KI-Griff ist eine
gekennzeichnete Nachruestung (Anker-Kommentar enemy_ai_common.c:12770ff) mit geliehener Zombie-Opferbank,
deren Wurzelversatz Leon relativ zum Anker setzt (re15_clip_root_motion_abs). Das RE2-Ziel fuer einen Griff
durch Gitterstaebe ist der EM2D-Arm (Pin auf die Hand @0x80100C18-38, Opferbank des Arms @0x80100C3C-5C,
Clip 0 @0x801012A8) — genau das faehrt der Default-Geschmack RE2. Der Nutzerbefund "Hand clippt durch Leon"
besteht unter RE1.5-KI nicht; dort ist der Befund ein anderer (kein Kontakt). -> OFFEN 6.

(in Arbeit)
