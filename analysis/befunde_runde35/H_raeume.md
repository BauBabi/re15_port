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

## OFFEN
- (laufend)
