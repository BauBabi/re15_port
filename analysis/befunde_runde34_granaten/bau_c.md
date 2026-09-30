# Runde 34 (Granaten) — Bau Spur C (Plattform)

Stand: 2026-09-30, Zweig `r34g/c-plattform` (auf `r34g/c0-vertrag` 8d8651e4), Arbeitsbaum
`.claude/worktrees/r34g_c`, Bauverzeichnis `re15_port/build_r34_c`, Laufzeit-Ausgaben `build/r34g_c/` (unversioniert).
Auftrag: BAUPLAN §3.3 C1-C4, C7, C8 mit der Orchestrator-Teilung C (Plattform) / D (RE2-FX-Maschine, `re2fx_pc.c`).
Dateibesitz: `platform/pc/main.c`, `platform/pc/src/*.c` ausser `re2fx_pc.c/.h`, `tests/unit/probe_r34_plattform*.c`,
`tests/unit/probes/r34_plattform.cmake`.

STATUS: FERTIG + NACHBESSERUNG (Gegenpruefung M1/M2, Abschnitt NACHBESSERUNG unten) — C1, C2, C3, C4, C7, C8 gebaut und
gemessen; Sonden + zwei exe-Pins gruen, Mutationsproben rot/gruen (§9, NACHBESSERUNG); volle Suite §10 / NACHBESSERUNG N6.
Was erst mit Spur A/B/D messbar ist, steht unter OFFEN.

Neue Datei `platform/pc/src/fx_plattform_pc.c/.h`: die fensterlos pruefbaren Teile (Takt, Ton-Weiche, Haken-Bindung,
Licht-Latch, TEX.TIM-Seiten, Zeichen-Helfer, Harness-Parse, Part-Tinte, Zeichenkamera fuer Spur D). Jede Konstante traegt
dort ihre Adresse bzw. ihren Datei-Offset; `main.c` ruft die Funktionen an den Stellen des Original-Hauptlaufs.

---

## C1 — ESP-Takt hinter dem Spielschritt (E10)

### Beleg (selbst disassembliert, `re15_disasm.py dis 0x8001cdd0 60`)
```
8001cdec: jal 0x8003f038
8001cdf4: jal 0x8004f090
8001cdfc: jal 0x8001500c
8001ce04: jal 0x8001a50c        Gegner
8001ce0c: jal 0x80031c44        Spieler inkl. Waffen-FSM (Spawn Muendung/Huelse/Granate)
8001ce14: jal 0x8002bd44
8001ce1c: jal 0x800436a8
8001ce24: jal 0x8004f0b0
8001ce2c: jal 0x80019e20        ESP-Tick
8001ce34: jal 0x8001db28        Item-Modal
8001ce54: jal 0x80039590        (gegatet 0x800aca38 & 0x100000)
8001ce60: lbu v0,21336(v0)      Licht-Latch 0x800b5358 (C3)
```
RE2: Gegner-Schleife 0x800267c0 .. `bne s2,v0,0x800267c0` @0x80026930, danach `jal 0x8001d300` @0x80026980 (Pumpe).

### Gebaut
| Datei:Zeile | Inhalt |
|---|---|
| `platform/pc/src/fx_plattform_pc.c` `re15_pc_fx_takt_setzen/_frei/_takt` | Freigabe + Takt: `re15_esp_fx_tick(re15_esp_room_bank())`, dann `re2fx_tick()` |
| `platform/pc/main.c:5439` (vor der Zweig-Kette `re15_discard_frozen`) | `re15_pc_fx_takt_setzen(0)` — eingefrorene Bilder (Discard/Item-Modal/Menue) ticken nicht, wie bisher |
| `platform/pc/main.c:5522` (SCD-30-Hz-Zweig, alte Tick-Stelle) | `re15_pc_fx_takt_setzen(1)` statt `re15_esp_fx_tick(...)` |
| `platform/pc/main.c:7516` hinter `re15_game_step(&gctx)` | `re15_pc_fx_takt()` — vor dem Item-Modal-Tick (@0x8001ce2c < @0x8001ce34) |
| `platform/pc/main.c:10703` hinter dem `md1_ok`-Block | Rueckfall `re15_pc_fx_takt()` (Bild ohne Spielschritt; sonst wirkungslos) |

### Messung (echte exe, ROOM1140, Pistole `RE15_GIVE=3:15 RE15_EQUIP=3`, Schuss im Bild 360, `RE15_FX_LOG`)
Vorher-exe = Stand C0 (`re15_pc_vorher.exe`), nachher = C1. Erste FX-Zeilen im Spawnbild F360:

| Platz | vorher F360 | nachher F360 |
|---|---|---|
| Muendung id 2 sub 0 | `frame=0 fl=03` (ungetickt) | `frame=1 fl=93` (Routine 8 lief: Flags := row[0x0e] = 0x93) |
| Rauch id 3 sub 0 | `frame=0 fl=03` | `frame=8 fl=13` |
| Huelse id 4 sub 0 | `frame=0 fl=03` | `frame=0 fl=63` (Routine 38 -> 16, Flags 0x63) |
| Blut id 0 (Treffer) | `frame=0` | `frame=1` |
| Zweitblitz id 2 sub 4 (Kind der Routine 8) | erst F361 | schon F360 `frame=6 fl=13` |

Der Zustand, den das alte Programm erst im Bild 361 zeigte, steht jetzt im Spawnbild 360 — genau die Verschiebung um
ein Bild, die @0x8001ce0c < @0x8001ce2c verlangt.

### Gebrochene Pins durch die Verschiebung
Keine. (Gegenpruefung M2: das hiess auch — es GAB keinen Pin auf die Lage des Takts in main.c. Seit der Nachbesserung
haelt ihn `integration_r34_plattform_takt` fest, s. NACHBESSERUNG N2.) Die volle Suite nach C1 (`build/r34g_c/suite_c1.log`) ergab 426/428; beide Roten sind Laufzeit-Flattern ohne
Bezug zum Takt, einzeln nachgeprueft:
* `integration_relatch_pin` (Timeout 30 s im Verbund) -> einzeln `Passed 30.61 sec`.
* `integration_r30_irons_tisch_bild`: exe endet bzw. haengt NACH `[pad] kein Controller gefunden` und VOR `[fps] target=`
  (`debug.log` der Laeufe `lauf_bd268cc1_P` und einzeln `lauf_6f198bd0_S`: 5 Zeilen) — also im Vorspann/Titel, bevor die
  Spielschleife (und damit der ESP-Takt) ueberhaupt laeuft. Dieselbe Klasse hatte C0 (Timeout im Verbund). Unter der
  Last der Parallel-Spuren haengen auch fremde exe-Laeufe (gesehen: zwei `build_r34_a`-Prozesse der Spur A).
Keine Unit-Sonde laeuft ueber `main.c`; die Unit-Sonden mit eigener Schleife (z.B. `probe_r30b_muendung`) sind vom Umbau
nicht beruehrt.

---

## C2 — ESP-Zeichnen

### Beleg (selbst disassembliert: `dis 0x80053240 100`, `dis 0x800533bc 70`, `dis 0x800534c4 170`, `dis 0x80053768 40`)
```
800532f0: addiu s0,s0,-132        Platz 95 -> 0 (s0 startet bei Pool + 96*0x84 + 0x6c)
800532fc: andi v0,v1,0x1 / beq    aktiv?
80053308: andi v0,v1,0x2 / beq    sichtbar?
80053314: lh v0,-68(s0)           slot+0x28  -> Regions-Test FUN_80014368 @0x80053334
80053324: lh v0,-66(s0)           slot+0x2a
80053330: lh v0,-64(s0)           slot+0x2c
80053344: lbu v0,2(s0) / lw v1,12(s0)   Satz = [slot+0x78] + slot+0x6e*8
80053354: lbu v0,1(s3)            Zellen je Satz = Byte1
800534f4: lbu v0,108(a1) / 80053500 srl v0,v0,3 / 80053504 andi v0,v0,0x2   ABE = Flags-Bit 4 -> Code 0x2c|2
8005350c: addiu v0,a1,40          RTPS auf slot+0x28 (lwc2 @0x80053514/18, RTPS 0x4a180001 @0x80053534)
80053538: lhu v0,50(a1)           CLUT = slot+0x32 (Wort 3 des POLY_FT4)
8005353c: lhu v1,48(a1)           TPAGE = slot+0x30 (Wort 5)
800535d0: lhu v0,4(a1)            defW = slot+0x04 (Zeilenkopie), mult mit step16
800535e0: lhu v0,6(a1)            defH = slot+0x06
80053620: sra v0,a0,6             OT-Eimer SZ3>>6
80053778..800537b0                die n Quads eines Platzes als KETTE in Reihenfolge (OT -> Quad0 -> .. -> Quad n-1 ->
                                  alter Eimerkopf); der spaeter gebaute Platz (kleinerer Index) liegt vorn = unten
80053394: andi v1,v1,0x8          Flags-Bit 3 -> zweites Zeichnen mit derselben Matrix 0x800b5288 (@0x800533bc-0x80053470)
```

### Gebaut
| Datei:Zeile | Inhalt | Beleg |
|---|---|---|
| `main.c:259` | Sichtbarkeit `re15_pc_esp_sichtbar` = Flags Bit0 UND Bit1 (vorher nur Bit1) | @0x800532fc-0c |
| `main.c:263`, `:287`, `:330-332` | Weltlage `re15_pc_esp_weltlage`: `wpos` (slot+0x28), Rueckfall `x+xlat` solange wpos 0 — fuer Regions-Test UND Projektion | @0x80053314-30, @0x8005350c |
| `main.c:296-320` | GLOBAL-Bank: Seite nach TPAGE (`re15_pc_esp_tpage` & 0x1f), Palette nach CLUT (`re15_pc_esp_clut`), Slot 50/51; sonst Altweg (Ein-Paletten-Blaetter 20..23/44) | @0x80053538/@0x8005353c |
| `main.c:418-424` | defW/defH `re15_pc_esp_defwh` = Zeilenkopie +0x04/+0x06 fuer JEDEN Row-VM-Platz (vorher nur Routine 17/18) | @0x800535d0/@0x800535e0 |
| `main.c` Tri-Aufrufe | `clut_wort` an `re15_render_textured_tri` (render_pc.c waehlt damit den Paletten-Stapel) | — |
| `main.c:3788-3816` | Boot: `DATA/TEX.TIM` -> Seiten 0x1e/0x1f (je 16 Paletten 480..495) -> Slots 50/51 | TEX.TIM-Offsets unten |
| `main.c:140-141` | `RE15_TIM_SLOT_FX_SEITE_1E 50`, `_1F 51` | — |
| `render_pc.c:204` | `RE15_TIM_SLOT_MAX` 50 -> 56 (50/51 Effektseiten, 52..55 frei fuer Spur D) | — |
| `fx_plattform_pc.c` `re15_pc_fx_seite_bauen` | schneidet die Seite byte-true aus TEX.TIM, NUR im gemessenen Rechteck (Spalte >= 192) | s.u. |

TEX.TIM (Datei gelesen): Kopf `10 00 00 00 08 00 00 00` (4 bpp + CLUT); CLUT-Block @0x08 `0c 06 00 00 00 01 e0 01 20 00 18 00`
= VRAM (256,480) 32x24, Eintraege ab @0x14; Bild-Block @0x614 `0c 80 02 00 00 00 00 00 40 01 00 01` = 320 hw x 256, Pixel ab
@0x620. Spalte 192 = VRAM x 896: gemessen bitgleich gegen die ShowVRAM-Grundwahrheit (`tools/tex_tim_effect_slice.py
--verify-vram`, 0/32768 Abweichungen), derselbe Weg liefert seit 2026-08-21 `effect8_fire.tim` (gepinnt in
`test_1090_fire_pin`). TPAGE -> VRAM nach psx-spx (x = (tp & 0xf)*64, y = (tp>>4 & 1)*256). EFF-Kopfworte CORE00.ESP
(gelesen): Id 3 `11 78 1e 00` @0x00C, Id 8 `11 79 1e 00` @0x62C, Id 0 `51 79 1f 00` @0x828, Id 2 `51 7a 1f 00` @0xF04,
Id 4 `d1 7a 1f 00` @0x172C.

CLUT/TPAGE-Saat des Spawners (selbst nachgelesen, `dis 0x80019870 12`): `lhu v0,4(t5)` / `addu v0,v0,s0` / `sh v0,50(t0)`
@0x8001987c-88 (CLUT = Kopf +4 + (sub>>3)*0x40), `lhu v1,6(t5)` / `sh v1,48(t0)` @0x8001988c/98 (TPAGE = Kopf +6); Routine 10
(`dis 0x800176b0 22`): `lhu a0,30(v1)` @0x800176e0, `sll a0,a0,6` @0x800176e8, `lhu v0,50` / `addu` / `sh v0,50(v1)`
@0x800176f0-fc (CLUT += row[0x1e] << 6), TPAGE |= row[0x16] @0x800176d8-ec.
Genutzte Paletten (Zeile = CLUT-Wort >> 6, Spalte 16 = x 272): 480 Rauch, 481 Rauch-Kind 0x0B, 483 Feuerball 0x19,
484 Feuer, 485 Blut, 489 Muendung, 490 Zweitblitz (Routine 10 `CLUT += row[0x1e]<<6` @0x800176e0-fc, id 2 sub 4
row[0x1e] = 1), 491 Huelse, 492 Granate 0x0D. Hochgeladen 480..495 (16 Paletten -> 256x4096 je Seite, unter der
4096-Grenze aelterer GPUs); CLUT ausserhalb -> Altweg.

### Row-Bytes +0x04/+0x06 in CORE00.ESP (nachgeprueft, Werkzeug `esp_rows.py`)
* Blut id 0 sub 0/1 Stroeme 0-2: 0x1000; sub 1 Strom 3 und sub 2 Stroeme 1-5: Halte-Zeile w/h 1 (Flags 0x61, unsichtbar),
  danach 0x0e10 / 0x1068 / 0x12c0 / 0x1518 / 0x1770; sub 2 Strom 0 0x0e10.
* Muendung id 2 sub 0/1/4/5/7: 0x1000; sub 3/6: 0x1320; sub 2: w/h 1 (Routine 15, unsichtbar).
* Huelse id 4 sub 0/7 Zeile 0: w/h 1 bei Flags 0x63 (SICHTBAR) -> Quad von HOECHSTENS 1 Pixel (Korrektur der Nachbesserung:
  die Ecke x0 = (sx<<16 + dx*stepX) >> 16 rundet eine negative Zellen-Ecke auf -1 ab, x1 = (... + w16) >> 16 auf 0 —
  Original `srl a0,a0,16` / `srl t1,t1,16` @0x800536e0-e8 mit s6 = SX<<16 @0x80053558, der Port rechnet dieselbe
  Abrundung; also auch im Original ein 1-Pixel-Punkt, kein unsichtbarer Platz); Zeile 1: 0x1000.
* Granate id 4 sub 0x0D (Zeile @0x1AB8), Feuerball/Rauch id 3 sub 1/3: 0x1000.

### Reihenfolge
Die Kettenverknuepfung @0x80053778-b0 legt die Quads eines Platzes IN Reihenfolge vor den alten Eimerkopf; die Schleife
laeuft 95 -> 0 (@0x800532f0). Im selben Eimer liegt also Platz 0 unten, Platz 95 oben; im Platz Quad 0 unten. Der Port
reiht Plaetze 0 -> 95 und Quads 0 -> n-1 in die STABIL nach Tiefe sortierte Liste (render_pc.c Einfuegesortierung, bewegt
nur bei `<`) — bei gleichem Schluessel exakt diese Reihenfolge. Keine Aenderung noetig. Unterschied bleibt nur ZWISCHEN
Plaetzen mit verschiedener Tiefe im selben 64er-Eimer (Port: exaktes vz, Original: Eimer + Kette) — portweite Konvention
(`main.c` OT-Skala-Kommentar), OFFEN O-C3.

### Messung Vorher/Nachher (Pflicht: Muendung, Huelse, Rauch, Blut)
ROOM1140, Pistole, Schuss F360, `RE15_FRAMEDUMP=356-400/1`, Fenster 1x (320x240), vorher = C1-exe (`re15_pc_c1.exe`,
gebaut aus 589b1dc7), nachher = C2. 45 Bilder, `ppmdiff.py`:

| Bilder | Pixel | Quads (FX-Log-Rueckrechnung `attrib.py`) |
|---|---|---|
| 356-359, 364-366, 375-400 | 0 | — |
| 360 | 7 | Muendung/Zweitblitz/Rauch-Ueberlappung; 1 Pixel Huelse (153,78) |
| 361 / 362 / 363 | 16 / 11 / 1 | Muendung id 2.0, Zweitblitz id 2.4, Rauch id 3.0 |
| 367-374 | 4-11 je Bild (48 gesamt) | NUR Rauch id 3.0 |
| **Summe** | **91** | 0 Pixel ausserhalb eines Effekt-Quads |

Blut (25 Partikel in 360-370): **pixelgleich** (Seite 0x1f Palette 485 == `effect0_blood.tim`, Sonde S48/S49; Zeilen
0x1000). Feuer id 8: Seite 0x1e Palette 484 == `effect8_fire.tim` (S47/S49).

Zuordnung per Mutationsprobe (Seiten-Upload aus, `re15_pc_mutT.exe`): C1 vs mutT = **1 Pixel** (F360 (153,78) = die
Huelse in ihrer Halte-Zeile w/h 1, Flags 0x63 -> Quad von hoechstens 1 Pixel, @0x800535d0/@0x800536e0-e8); mutT vs C2 = **90 Pixel** = der
Texturwechsel. Dessen Ursachen, belegt:
1. Die alten Blaetter 21/22/23 sind ShowVRAM-Abzuege OHNE Bit 15 (README extracted_fx). Texelvergleich alte Blaetter
   gegen TEX.TIM-Seite + Palette (`sheet_cmp.py`): Muendung 1269, Rauch 4162, Huelse 1269 abweichende Texel — **alle**
   an Halbworten mit Bit 15 im Nibble 3 (Index 8..15 an jeder 4. Spalte), **0 andere**; Blut 0, Feuer 0.
2. Zweitblitz id 2 sub 4: jetzt Palette 490 (Routine 10 @0x800176e0-fc), vorher auf dem Muendungsblatt (Palette 489).
Granate im Flug (id 4 sub 0x0D, Stand vor Spur A): vorher Slot 23 = Huelsenblatt Palette 491, jetzt Seite 0x1f Palette 492
(`0000 b631 b1ef adce`, K3 CLUT_GRANATE) — gemessen F383-F415 je 3-8 Pixel anders, z.B. F399 (152,113) (60,60,48) ->
(28,28,24) (`build/r34g_c/c2_granate_alt_oben_neu_unten.png`).

---

## C3 — Licht-Latch-Leser (E11)

### Beleg (selbst disassembliert: `dis 0x8001ce5c 150`, `dis 0x8001d088 90`, `dis 0x8004f008 36`, `dis 0x800661c0 30`,
`dis 0x80068098 100`, `dis 0x8001e8c8 60`)
```
8001ce60: lbu v0,21336(v0)        Latch 0x800b5358; 8001ce68 beq -> 0x8001d088
8001ce7c: lh v1,4068(v1)          aktiver Cut 0x800b0fe4; Satz = [RDT+0x2c] + Cut*40 (sll 2 / addu / sll 3)
8001cea0: jal 0x8004ee38          Byte-Kopie 40 B (a2 = 0x28 im Delay @0x8001ce6c) nach [0x800ac77c]
8001cebc: ori v0,zero,0x4b0 / 8001cec0 sh v0,0(s0)   Vektor x = 1200 (s0 = 0x1f80002c), z := 0 @0x8001cec8
8001cecc: jal 0x8004f008          a0 = Spieler +0x6a (lh 0x800acabe)
          -> 8004f038 jal 0x80068098 (RotMatrix {0, a0, 0}, Tabelle 0x800794c4)
          -> 8004f048 jal 0x800661c0 (ApplyMatrix: ctc2 RT, 0x4a486012 = MVMVA sf 1 V0 ohne T, swc2 MAC1..3)
8001cef8: sb zero,3(v0)           Licht 2 Typ := 0
8001cf20..34: lbu 10 / sltiu 0xd2 / sb 0xd2      R := max(R, 0xd2)
8001cf5c..70: lbu 11 / sltiu 0x8c / sb 0x8c      G := max(G, 0x8c)
8001cf98..ac: lbu 12 / sltiu 0x50 / sb 0x50      B := max(B, 0x50)
8001cfe4: sh v1,28(v0)            x := lhu 0x800aca88 + gedrehtes x
8001d018: addiu v1,v1,-800 / 8001d01c sh v1,30(v0)   y := lhu 0x800aca8c - 800
8001d058: sh v1,32(v0)            z := lhu 0x800aca90 + gedrehtes z
8001d080: ori v1,zero,0x1770 / 8001d084 sh v1,38(v0)   Helligkeit Licht 2
8001d09c: jal 0x8001e8c8          Spieler (darin 8001e94c jal 0x80053fc0 = Lichtrechnung je Figur)
8001d0e8..164: Entity-Schleife    je aktive Entity jal 0x8001e8c8
8001d1ac: jal 0x8004ee38          Satz aus der Kopie zurueck
8001d1b4: sb zero,0(s0)           Latch := 0
8001d1c0: jal 0x8002c18c          Objekte/Props ERST DANACH -> ohne das Licht
```
Satz-Layout = `re15_light_cut_t` (`re15_light.h:55-62`): +3 type_flags[2], +10..12 colors[2], +0x1c..0x20 positions[2],
+0x26 brightness[2].

### Gebaut
| Datei:Zeile | Inhalt |
|---|---|
| `fx_plattform_pc.c` `re15_pc_licht_latch_rechnen` | Licht 2 nach obiger Liste; Drehung ueber `re15_skel_euler_matrix(0, rot_y, 0)` (= RotMatrix 0x80068098), MAC = (m*1200)>>12; 16-Bit-Summen wie lhu/sh |
| `fx_plattform_pc.c` `re15_pc_licht_latch_anwenden/_zurueck` | Kopie + Umstellen nur bei Latch; Zurueckschreiben + Latch 0, wenn er stand |
| `main.c:8683` (vor dem Licht-Kontext des Spielers) | anwenden (+ Zeile `[licht] F.. Latch -> Cut ..` in debug.log) |
| `main.c:10377` (hinter der Figuren-Schleife, VOR den Props) | zurueck |
| `main.c:10707` (hinter dem md1_ok-Block) | zurueck als Rueckfall (Latch-Loeschung haengt im Original nicht am Zeichnen) |
| `main.c:7494-7513` | Mess-Haken `RE15_FORCE_LICHT=<bild>[,...]` (setzt den Latch wie Routine 31 @0x8001857c / Routine 9 @0x80017694; kein Spielverhalten) |

### Messung (ROOM1140, `RE15_FORCE_LICHT=330`, Framedumps F327-F334, Gegenlauf ohne Haken)
* debug.log: `[licht] F330 Latch -> Cut 0 Licht2 Typ 0 Farbe (210,140,80) Lage (-6400,-800,-17600) Hell 6000` bei Leon
  `PL(-7600,-17600,rot=0)` = Leon + (1200, -800, 0).
* Pixeldifferenz zum Lauf ohne Haken: F327/328/329 **0**, **F330 1828 Pixel** (bbox x 7..180, y 71..169: Leon + Zombies),
  F331-F334 **0** -> genau EIN Bild, danach bitgleich zurueck. Mittlere Aenderung R +13.2 / G +12.5 / B +8.9.
  Tisch-Props (Glas, Tasse, Aschenbecher) unveraendert (`build/r34g_c/c3_licht_F330_lupe.png`: ohne | mit | F331).

---

## C4 — Ton, Haken-Bindung, RE2-FX-Anbindung

### Beleg RE1.5 FUN_80045024 (`dis 0x80045024 80`, `table 0x80010e70 6`)
```
80045028: srl v1,a0,24            Bank
80045058..6c: lb 0x800b21ec[Bank] == -1 -> Rueckkehr (Bank nicht geladen)
80045078: srl v0,a0,16 / 8004507c andi s4,v0,0xff   Satz
80045080: andi a0,a0,0xff         Byte0 = Lage-Flag (Byte1 wird nicht gelesen)
80045094: sltiu v0,v1,0x6         Bank-Tor
0x80010e70: [0] 0x800450bc (0x801fdd00) [1] 0x800450d0 ARMS 0x801fcd00 [2] 0x800450e4 snd0 [3] 0x800450f8 snd1
            [4] 0x8004511c CORE 0x801fbd00 [5] 0x80045130 snd0; Satz-Tor sltiu 0x21 (@0x800450bc/d0/e4/11c), snd1 0x19 (@0x800450f8)
```
### Beleg RE2 (`re2_disasm.py dis 0x80020fcc 26`, `dis 0x80021670 18`, `dis 0x8005ba28 50`)
```
80020fd4: lui a0,0x112 / 80021028 ori a0,a0,0x1 / 8002102c jal 0x8005ba28 (a1 = Platz+0x60)   Op 48 Brand
80021678: lui a0,0x113 / 8002167c ori a0,a0,0x1 / 800216ac jal 0x8005ba28 (a1 = sp+16)         Op 49 Saeure
8005ba30: srl t1,a0,24 (Bank) / 8005ba7c-80 Satz = (a0>>16)&0xff / 8005ba8c-94 [0x800dbb78+Bank*4]+Satz*4
```
RE2 ARMS0B Satz 19 / ARMS0A Satz 18 = `00 00 33 20` (Saeure-GP §12); RE1.5 `ARMS10.EDH`/`ARMS11.EDH` @0x28 (Satz 10) =
`00 00 33 20` (Datei gelesen, Sonde W35), VB bytegleich RE2 ARMS0B/0A (md5 39cec979 / 46833b5e).

### Gebaut
| Datei:Zeile | Inhalt |
|---|---|
| `fx_plattform_pc.c` `re15_pc_esp_se_weiche` / `re15_pc_esp_se` | Bank ueber `re15_audio_se_bank_kind` (include/re15_audio.h), Satz-Tore, Aufruf weapon/snd0/snd1/core; Log `SE  esp code=..` im Waffen-Log |
| `fx_plattform_pc.c` `re15_pc_re2fx_se_weiche` / `re15_pc_re2fx_se` | 0x01130001 -> ARMS10 Satz 10, 0x01120001 -> ARMS11 Satz 10, sonst stumm |
| `fx_plattform_pc.c` `re15_pc_r34_haken_binden` | `re15_esp_se_hook`, `re15_esp_aufschlag_hook = re2fx_aufschlag`, `re2fx_se_hook`, `re2fx_applier = re15_re2_gl_apply` |
| `audio_pc.c` `re15_audio_arms_zusatz_se` (hinter `re15_audio_prime_weapon`) | zwei einmal geladene Zusatzbaenke (ARMS10/ARMS11), neben der Waffenbank; `se_play_layers` wie ARMS |
| `main.c:3826` (nach `re15_audio_init`) | Haken binden; `re2fx_register_core` mit `shared_assets/RE2/CORE00.ESP` (Puffer lebt bis Prozessende) |
| `main.c:10685-10694` | `re15_pc_fx_kamera_setzen` (Ansicht dieses Bilds fuer Spur D), `pc_draw_effects`, `re2fx_pc_draw()`, Kamera ungueltig |
| `room_pc.c:130-135` | `re2fx_reset()` im Raum-Teardown neben `re15_esp_fx_reset()` (Port-Zuordnung: RE1.5 nullt die 96 ESP-Plaetze @0x80019378; der RE2-Pool teilt im Port diese Lebensdauer) |

Messung exe: `[re2fx] RE2 CORE00.ESP 8572 B -> re2fx_register_core rc=-1` (C0-Stub, Spur D). Die ESP-Haken ruft erst Spur A
(Routinen 29/31) — der exe-Zaehlwert (8 Abprall-SEs + 1 CORE-8 je MITTE-Wurf) ist nach dem Merge von A zu messen (OFFEN).
Stattdessen `unit_r34_plattform_ton` mit dem ECHTEN audio_pc.c (§9): ARMS10/ARMS11 Satz 10, ARMS09 Satz 0x0A ueber den
ESP-Haken und CORE Satz 8 KLINGEN (PCM-Energie 156008740 / 228798044 / 1354640 / 11661880), leere Saetze und Bank 0 stumm.

---

## C7 — RE2-Part-Farbwort (+0x70), O-VB3

### O-VB3 geklaert (RE2 PSX.EXE, selbst disassembliert: `dis 0x80026840 40`, `dis 0x80027380 20`, `dis 0x800278f8 6`,
`dis 0x80027ac0 16`, `dis 0x80027bd0 40`; Aufrufer-Suche `jal 0x80027160` / `jal 0x80027434`)
```
80026894: lw a1,408(s0)           Part-Feld der Entity (+0x198)
8002689c: jal 0x80027160          Part-Walk — in der Entity-Hauptschleife 0x800267c0-0x80026930 = JEDER Gegnertyp
8002738c / 800273d4: jal 0x80027434   je Part (a1 = Part-Record, Schritt 172)
80027900: lw fp,112(s1)           Part +0x70 = Farbwort
80027ae0: addu a2,fp,zero / 80027aec jal 0x80027bec
80027c08: sw a2,16(sp) / 80027c2c lwc2 $6,0(v0) (0xc8460000 = RGBC)  -> NCCT: out = RGBC * (BK + LCM*LLM*N)
```
Weitere Aufrufer von 0x80027160: 0x8002ec08, 0x80033af0, 0x80033bd4. Die Tinte SKALIERT das Beleuchtungsergebnis
(neutral 0x808080, FUN_80028368.c:55). Das stand fuer Zombies mit aktiver Gore-Bruecke schon im Port
(`main.c` `RE2_GORE_TINT`, `re15_re2z_gore_resolve`) — die Synthese "rendert es nirgends" (P29) war zu eng gegrept
(`gore_tint` statt `part_tint`). Neu: dieselbe Modulation fuer alle UEBRIGEN RE2-KI-Aktoren (Hund, Spinne, ... V4).

### Gebaut
| Datei:Zeile | Inhalt |
|---|---|
| `fx_plattform_pc.c` `re15_pc_re2_part_tint` | nur `re15_ai_re2_for_type(type)`; Bone i = Part i (<= 16, V4); Wort 0 = nie geseedet -> neutral; Rueckgabe 1 nur bei nicht-neutralem Part |
| `main.c:9874-9882` | `tint_on = gore_on || re15_pc_re2_part_tint(...)`; `RE2_GORE_TINT` gated auf `tint_on` (+ Indexgrenze) |
Bei neutralen Tinten bleibt der Renderpfad bitgleich (prim * 0x80 >> 7 = prim; und der Pfad wird ohne nicht-neutralen Part
gar nicht betreten). Sichtbar wird es, sobald Spur B die V4-Tinten fuer Hund/Spinne schreibt.

---

## C8 — Harness `RE15_FORCE_AUFSCHLAG="<re2_art>@<bild>[,...]"`

`main.c:7471-7492`: im Spielbild <bild> mit Takt, VOR `re15_pc_fx_takt()` (= dort, wo Routine 31 den Haken im echten
Ablauf ruft), `re2fx_aufschlag(art, q, gier)` mit q = 1500 vor Leon auf Bodenhoehe (RotMatrix(0, rot_y, 0) wie K1 ROTY),
gier = Leons rot_y; Zeile in debug.log und Waffen-Log. Die 1500 ist eine HARNESS-WAHL (Abstand), keine Original-
Konstante. Parse/Rechnung in `fx_plattform_pc.c` `re15_pc_force_aufschlag_eintrag` (Sonde A100-A103). Sichtabnahme
erst mit Spur D (heute Stub).

---

## 9. Sonden und Mutationsproben

### unit_r34_plattform (`tests/unit/probe_r34_plattform.c`, 77 Pruefungen, gruen)
Teile T (C1 Takt), H (Haken), W (Weiche + EDH-Bytes), S (TEX.TIM-Seiten), Z (Zeichen-Helfer), L (Licht), A (Harness),
F (Part-Tinte). Sollwerte als LITERALE aus Disasm/Datei. Negativ-Kontrollen: Takt ohne Freigabe / unter
RE15_PAUSE_ACTION (T2/T6), Haken vor der Bindung NULL (H10), Bank 0/6 und Satz-Tore (W23-W27, W29), unbekannte RE2-Codes
(W32), tpage 0x0e / 0x1d / falsche Magic (S50-S52), CLUT ausserhalb (S54), Flags 0x0a/0x61 (Z62/Z63), Latch 0 und
ungueltiger Cut (L87/L90), Harness Art 3/Unsinn/NULL (A103), RE1.5-Hund und neutrale Tinten (F110/F112/F113).

| Mutation (kurz verstellt, gebaut, gelaufen, zurueckgesetzt) | Ergebnis |
|---|---|
| `RE15_PC_LICHT_R_MIN` 0xd2 -> 0xd1 | rot 81 (+88) |
| `RE15_PC_LICHT_VOR` 0x4b0 -> 0x4a0 | rot 82 (+85) |
| `RE15_PC_LICHT_HOEHE` 800 -> 700 | rot 82 (+85) |
| `RE15_PC_LICHT_HELL` 0x1770 -> 0x1771 | rot 83 |
| Satz-Tor 0x21 -> 0x22 | rot 25 |
| `TEX_QUELL_SPALTE0` 192 -> 196 | rot 41 (+42, 47 ...) |
| `RE15_PC_FX_CLUT_Y0` 480 -> 481 | rot 42 (+43-45 ...) |
| Sichtbarkeit nur Bit1 | rot 62 |
| defW fest 0x1000 | rot 69 (+70) |
| ESP-Tick im Takt entfernt | rot 4 |
| `RE15_PC_ARMS_SAEURE` 0x10 -> 0x11 | rot 30 (+33) |
| Flavor-Tor der Part-Tinte entfernt | rot 110 |
Nach jeder Probe `git checkout` der Datei, Neubau, `GRUEN: 77 bestanden` (Logs `build/r34g_c/mut_*.log`).

### unit_r34_plattform_ton (`tests/unit/probe_r34_plattform_ton.c`, echtes audio_pc.c, CAP_SYNC, 8 Pruefungen, gruen)
| Mutation | Ergebnis |
|---|---|
| Zusatzbank spielt Satz+1 | rot 2 (+4, 8) |
| ESP-Satz aus (code>>8) statt (code>>16) | rot 5 |

---

## 10. Volle Suite

Werkzeuge und Belegbilder dieses Dossiers: `analysis/befunde_runde34_granaten/bau_c_werkzeug/` (`lauf.sh` Messlauf,
`ppmdiff.py` Pixelvergleich, `attrib.py` Pixel -> Effekt-Quad, `sheet_cmp.py` Texelvergleich alte Blaetter gegen
TEX.TIM, `rows_wh.py` Zeilen-Bytes, `mutation.sh` / `mutation_ton.sh` Mutationsproben, drei PNG).

* **Lauf nach C1** (`local_build.sh test`, 428 Tests): 426/428 — `integration_relatch_pin` (Timeout im Verbund, einzeln
  `Passed 30.61 sec`) und `integration_r30_irons_tisch_bild` (Haenger im Titel vor der Spielschleife, §C1).
* **Endlauf 1** (`local_build.sh` = all, Stand e993828a, 430 Tests = 428 + `unit_r34_plattform` +
  `unit_r34_plattform_ton`): `99% tests passed, 2 tests failed out of 430`, 1296.93 s. Rot:
  `integration_r30_granate_laden` (Lauf b, SOFTWARE_RENDER, Abriss bei F120 mit exit=1) und
  `integration_r30_irons_tisch_bild` (Lauf P0: exe endet nach `[pad] kein Controller gefunden`, 5 Log-Zeilen, vor der
  Spielschleife). **Einzeln wiederholt: beide gruen** (`Passed 111.42 sec` / `Passed 59.33 sec`) -> Last-Flattern
  (parallel laufen die Bauten/exe-Laeufe der Spuren A, B, D und ein Android-Bau), kein reproduzierbares Rot.
* **Endlauf 2** (all, Stand 6b8e6f16): `99% tests passed, 2 tests failed out of 430`, 1033.73 s. Rot diesmal
  `integration_r30_cut_blitz` (Lauf A: exe endet bei F553 mit exit=1, das Telefon-Bild F330 ... nicht erreicht) und
  `integration_elza_vollstart` (exe endet nach 61 s mit exit=1, Log bricht bei F445 mitten im Raumaufbau ab; der Test
  erwartet einen Zeitablauf nach 100 s). **Einzeln wiederholt: beide gruen** (`Passed 107.97 sec` / `Passed 100.19 sec`);
  die Roten aus Lauf 1 liefen in Lauf 2 gruen (granate_laden 135.84 s, irons_tisch_bild gruen).
* Befund zum Muster "exit=1, Log bricht ohne Meldung ab": der Port hat ausser den `exit(1)` der Renderer-Initialisierung
  (render_pc.c:587/622/635/643) keinen Weg zu Code 1 und keinen Absturz-Haken; ein Absturz haette 0xc0000005. Code 1 ohne
  Logzeile ist das Bild eines von aussen beendeten Prozesses (`taskkill /F` setzt Exit-Code 1) — die Parallel-Sitzungen
  starten und beenden `re15_pc.exe`-Prozesse (Memory "taskkill-Falle"). Jedes der vier verschiedenen Roten lief einzeln
  gruen; kein Rot war reproduzierbar.
* **Endlauf 3** (all, Stand wie Endlauf 2 + Dossier): `99% tests passed, 5 tests failed out of 430`, 1225.16 s:
  `integration_elza_vollstart`, `integration_r30_irons_tisch_bild` (P0 exit=1), `integration_r30_irons_tisch_licht`
  (P60 exit=1), `integration_r30_titel_puls` (Bilddauer im Titel bis 393 ms statt ~33 ms -> Periode verfehlt, Last),
  `integration_r33_speichern` (b exit=1). Einzeln wiederholt: irons_tisch_bild `Passed 105.49`, irons_tisch_licht
  `Passed 52.96`, titel_puls `Passed 27.64`, r33_speichern `Passed 76.28`; elza_vollstart zweimal mit exit=1 nach 35 s
  bzw. 90 s abgerissen, dann `Passed 102.57 sec`.
* **Gegenprobe "von aussen beendet"**: derselbe elza-Riegel (`cmake -P test_elza_vollstart.cmake`) mit einer
  UMBENANNTEN Kopie derselben exe (`re15_pc_r34c_probe.exe`, bytegleich) -> `Process terminated due to timeout`
  (= der erwartete Ausgang) und `elza_vollstart OK`, waehrend die Laeufe unter dem Namen `re15_pc.exe` in derselben
  Viertelstunde mit exit=1 abrissen. Im Repo beendet `scripts/green.sh:20` noch `taskkill //F //IM re15_pc.exe`
  maschinenweit (fremde Datei, nicht angefasst); `tools/local_build.sh:290-296` ist schon auf das eigene Bauverzeichnis
  begrenzt.
* **Endlauf 4** (all): `99% tests passed, 4 tests failed out of 430`, 951.76 s: `integration_r30_cut_blitz` (Teile A/B
  gruen, Teil C exe exit=1 ohne cutsync.log), `integration_elza_vollstart` (exit=1 nach 63 s),
  `integration_r30_irons_tisch_laden` (exit=1 vor Bild 120), `integration_r30_titel_puls` (Bilddauer im Titel bis
  79783 us). Einzeln: cut_blitz `Passed 91.34`, irons_tisch_laden `Passed 45.12`; elza und titel_puls im Einzellauf
  diesmal rot — bei gemessener CPU-Last 94-100 % durch die Parallel-Sitzungen (elza kam bis F790, der Wandzeit-Rahmen
  LAUFZEIT 100 s reicht bei der gesunkenen Bildrate nicht bis zum Szenenende; titel_puls misst die Bilddauer). Beide
  liefen mit DERSELBEN exe zuvor einzeln gruen (elza `Passed 102.57 sec`, titel_puls `Passed 27.64 sec`).
* Ergebnis: kein reproduzierbares Rot. Jeder der sieben verschiedenen roten exe-Tests aus vier Volllaeufen lief
  mindestens einmal einzeln gruen; alle Unit-Sonden (auch die zwei neuen) waren in jedem Lauf gruen. Die Zielzeile
  `LOCAL-BUILD-OK (all)` war unter der Dauer-Volllast nicht erreichbar.
* **Endlauf 5** (all): `99% tests passed, 3 tests failed out of 430`, 889.40 s, Abschlusszeile
  `!!! [local_build] FEHLER: ctest fehlgeschlagen (exit=8)`. Rot: `integration_elza_vollstart` (exit=1 nach 27 s, ROOM1031
  nie erreicht), `integration_r30_irons_tisch_licht` (P2 exit=1 vor dem Laden), `integration_relatch_pin` (exit=1 nach
  59 Log-Zeilen) — alle drei das Abbruchbild "von aussen beendet", alle drei mit derselben exe zuvor einzeln gruen
  (elza 102.57 s, irons_tisch_licht 52.96 s, relatch_pin 30.61 s). CPU-Last beim Ende 93 %.
* Fazit: fuenf Volllaeufe (426-428/430 gruen), 7 verschiedene rote exe-Tests, jeder einzeln gruen, keiner in zwei
  Volllaeufen hintereinander mit derselben Ursache rot ausser den exit=1-Abbruechen; kein Rot beruehrt einen Code-Pfad
  dieser Spur vor dem Abbruch (Titel/Laden). Fuer die Integration: Suite auf einer ruhigen Maschine wiederholen.

---

## NACHBESSERUNG (Gegenpruefung `bau_c.gegenpruefung.md`, Maengel M1/M2)

Stand: 2026-09-30, Commits `810a5ff2` (M2), `48e7ac16` (M1 + H4/H7), Dossier-Commit dieses Abschnitts. Laufzeit-Ausgaben
`build/r34g_c/nb*` (unversioniert), Werkzeuge `bau_c_werkzeug/nb_*`.

### N1 — M1: defW/defH aus der Zeile fuer Raum-Bank-Effekte mit nicht portierten Routinen

**Befund bestaetigt, Tragweite an der exe nachgemessen und korrigiert.** Der Zeichenpfad bleibt (byte-true, @0x800535d0/e0);
die Folgen stehen jetzt hier, die Ursache (Routinen) als INTEGRATIONSWUNSCH an Spur A, und ein Altbestand-Defekt, der den
Befund an der exe verdeckte, ist behoben (N1.4).

**N1.1 Original, selbst disassembliert** (`re15_disasm.py table 0x80071d40 48`, `dis` je Adresse; Slot-Felder aus dem
Spawner FUN_80019700: +0x70 Effekt-Id `sb t8,112(t0)` @0x800197d4, +0x72 scale `sh s1,114(t0)` @0x800197dc, +0x78
Satz-Basis `sw v0,120(t0)` @0x80019894, +0x80 Strom-Anfang `sw t0,128(t1)` @0x80019900):

| Routine | Adresse | Wirkung (Instruktionen) | schreibt +0x04/+0x06? |
|---|---|---|---|
| 41 | 0x80018ef4 | Flags := row[0x0e] (`lbu 14` @0x80018f04, `sb 108` @0x80018f0c); row[0x16] != 0 -> row[0x16]-- (@0x80018f1c-3c), sonst Flags := row[0x1e] (@0x80018f40-48), Satzindex +0x6e := row[0x26] (`lbu 38` @0x80018f58, `sb 110` im Delay-Slot @0x80018f60), Vorschub `jal 0x800174e4` @0x80018f5c, dann +0x0a (Beschl. y der NEUEN Zeile) += rng & 3 (`jal 0x8001af20` @0x80018f64, `andi 0x3` @0x80018f7c, `sh` @0x80018f84) | nein (die Zeilenkopie beim Vorschub) |
| 42 | 0x80018f98 | u16 +0x0e < u16 +0x26 (@0x80018fa8-b8): +0x0e++ (@0x80018fc0-cc); nur fuer Effekt-Id 0x0b (`lbu 112` @0x80018fc4, `bne` @0x80018fd4): Treffer = FUN_8002b7e8({+0x28,+0x2a,+0x2c}, 0x2d) (@0x80018fdc-ffc, a1 im Delay-Slot @0x80018fd8; alle aktiven Entitaeten 0x800acc2c Stride 0x1F4 + Spieler 0x800aca54 ueber FUN_8002b5d0) -> Flags \|= 0x20 sonst Flags := 0x13 (@0x80019004-30); ab +0x0e >= 0x10 **defH += 768** (`addiu v0,v0,768` @0x8001905c, `sh v0,6(v1)` @0x80019064). Sonst SCHLEIFE: 40 Byte Zeile 0 aus [+0x80] (@0x80019068-11c), Cursor +0x6f := 0 (@0x8001912c), row[0x16] := 0 (@0x80019138), xlat := 0 (@0x8001913c-44), Satzindex := 1 (@0x8001914c), Timer +0x6d := Satz[1].Byte2 (@0x8001915c-74) | defH |
| 24 | 0x80017f50 | row[0x0e] != 0 -> -- (@0x80017f60-80), sonst Vorschub (@0x80017f84); dann `jal 0x80017fa4` (Routine 25) @0x80017f8c | ueber 25 |
| 25 | 0x80017fa4 | **defW += 30, defH += 30** (@0x80017fbc-c8 / @0x80017fcc-d8); room_coll auf +0x28 (`jal 0x8001c6e8` @0x80017ff4); FUN_80012d60(0x1f4, Punkt, 0) @0x80018008; Treffer oder Boden -> Kind FUN_800199d4(0x030C0000 \| (+0x72 + 500), +0x2e, 0x80072d4c) @0x80018030-54, SE 0x03030001 @0x8001805c-64, A/B/Beschl./Geschw. := 0 @0x80018078-94, Satzindex := 0x0d @0x80018098 | defW, defH |
| 13 | 0x80017990 | room_coll auf +0x28 (@0x800179c4); gelandet -> Flags := row[0x0e] (@0x800179f0-f8), xlat_y -= Ueberstand (@0x800179fc-0c), row[0x16] != 0 -> Satzindex := row[0x16] (@0x80017a10-28), row[0x26] != 0 -> Vorschub (@0x80017a38-48) | nein |
| 19 | 0x80017d08 | row[0x0e] != 0 -> -- (@0x80017d1c-3c); sonst Kind FUN_800199d4(((row[0x16]>>8)<<24) \| ((row[0x16]&0xff)<<16) \| (rng+3072), rng*682, +0x74, +0x40) @0x80017d40-a0, row[0x26] != 0 -> Vorschub @0x80017db4-c4 | nein |

Zeilen (Gegenpruefer-Werkzeug `esp_zeilen_room2000.py`, ROOM2000.RDT Effekt 0x0b ab 0x15110): sub 0 = 6 Stroeme, Zeile 0
Routine 41 w/h 1, Flags 0x13 (Strom 0) bzw. 0x61 (Stroeme 1-5 = UNSICHTBAR, Bit 1 fehlt), Halten 0/5/10/15/20/25; Zeile 1
Routine 42 w 0x1000 h 0x19c4, Grenze +0x26 = 25. Original also: ein Dauerstrahl — Strom 0 sofort, die anderen gestaffelt,
jede Schleife ohne Halten, ab Zaehler 16 waechst die Hoehe um 768 je Takt.

**N1.2 Wo der Port abweicht (gemessen, echte exe).** Routine 41 laeuft im Port nicht (`re15_esp.c` `default: break`), der
Platz bleibt in Zeile 0 mit den Spawner-Flags 0x03 (sichtbar) und w/h 1, bis die Anim-Saetze enden (27-32 Takte):
* **Ladeweg** (Karte -> Continue, `nb_lauf_laden.sh`, `RE15_FORCE_CUT=9`, Lauf `nb_2000_laden_cut9`): 18 Plaetze, FX-Log
  486 Zeilen F0..F26, `A=41 fl=03`, `w16=180`. Mit derselben Abrundung wie das Original (`srl` @0x800536e0-e8, s. §C2)
  wird daraus ein Quad von hoechstens 1 Pixel. master (defW 0x1000): w16 = 4096 x 180 = 737280 -> **11-Pixel-Kleckse**,
  27 Bilder lang. Beides nicht das Original; C2 ist im Zeichnen byte-true, die Luecke ist Routine 41/42.
* **Tuer-/Sprungweg** (vor N1.4): auf master UND C2 IDENTISCH — der Effekt wurde nie gezeichnet, weil sein Spawn scheiterte
  (N1.4). Lauf `nb_2000_tuer` (Sprung-Spawn = Tuer-Ankunft aus ROOM2050 (-26450,10250), Tuertabelle
  `re15_port/tools/engine_tueren.txt` Zeile `2050 2000 9100 5700 -26450 10250 0`): Waffen-Log
  `SPAWN id=11 sub=0 scale=0x1900 streams=-1` x3 + `SPAWN id=6 ... streams=-1` x3, FX-Zaehler 6 -> 0 im Folgebild, kein
  FX-Log in 45 Bildern. Der Gegenpruefer-Vorschlag "Sichtabnahme bei Ankunft ueber die Tuer" war damit vor N1.4 gar nicht
  moeglich.
* Sichtbar ist die Lage (-23900..-22700, -5300, 11200) nur in ROOM2000-Cuts 5..10 und 13 (Werkzeug `nb_cut_regionen.c`,
  Region-Test = derselbe `re15_esp_fx_culled`); Cut 0 (Debug-Sprung) cullt sie.

**N1.3 Umfang (Kollateral der C2-Regel, vollstaendig).**
| Effekt (Raumbank) | Routinen | Spawner im Port | Wirkung von C2 |
|---|---|---|---|
| 0x0b sub 0 | 41/42 | ROOM2000/2001 main00 @0x174C/5C/6C (Eintrittstakt), ROOM20B0/20B1 sub00 @0x1E96/A6/B6 (Else-Zweig von Ck(3,235), also nur bei Flag (3,235)=1) | Zeile 0 w/h 1 statt 0x1000: <= 1-Pixel-Punkte statt 11-Pixel-Kleckse, 27-32 Takte |
| 0x0d sub 0/1 | 10/24/25 | keiner (kein Sce_espr_on in 206 RDTs mit Flags 0, kein Engine-Aufruf mit Id 0x0d: `grep` der 40 Spawn-Aufrufe, variable Ids nur Gore 0/5/7/8) | keine |
| 0x06 sub 1/2 | 10/0 | keiner der Eintritts-Spawns (ROOM2000 sub07 = sub 0, ROOM20A0 sub02 = sub 6, beide w/h 0x1000) | keine; die Zeilengroessen 0x0f9c/0x1064 waeren ohnehin byte-true (10/0 schreiben +4/+6 nicht) |
| 0x05 sub 0 (Gore-Bruecken) | 19 | Hund/Zombie (Ids 5/7 ueber die Raumbank) | keine (w/h 0x1000) |

**N1.4 Altbestand behoben: die Raum-Effektbank stand beim SCD-Eintrittstakt noch nicht.**
* Ursache: `pc_load_room_esp` lief erst NACH `re15_room_apply_pending`, dessen `scd_room_reenter` den Eintrittstakt
  faehrt; der Teardown `re15_room_reset_render_pc` hatte die Bank vorher auf NULL gesetzt. Jeder Sce_espr_on des
  Eintrittsbilds loeste gegen die leere Bank auf -> `streams=-1` -> row-loser Ersatzplatz `re15_esp_fx_spawn_ex(NULL,..)`
  -> im naechsten Takt verworfen (`eff_idx < 0`). Auf master identisch (`git show master:.../main.c` Zeile 7726,
  `room_pc.c` Zeile 131).
* Original: Raumlader FUN_800396fc `jal 0x80019354` @0x8003996c (einziger Aufrufer, Voll-Scan im Kommentar von
  `scd_room_reenter`), darin Parse `jal 0x8001945c` @0x80019428 (RDT+0x4c @0x80019404, +0x50 @0x80019424) und
  TIM-Installer @0x8001943c-48 — VOR der SCD-Raum-Init `jal 0x8003ef6c` @0x80039a00.
* Fix (Dateien Spur C): `re15_pc_room_esp_laden()` (`platform/pc/main.c` hinter `pc_load_room_esp`) wird aus
  `re15_room_reset_render_pc` (`platform/pc/src/room_pc.c`, Teardown (4)) direkt nach dem Clear gerufen; der spaete Aufruf
  nach `apply_pending` entfaellt (Kommentar an der alten Stelle). Boot-/Ladeweg (`main.c` Boot-Block) war schon richtig.
* Zensus (Werkzeug `nb_eintritt_zensus.c`, 206 RDTs, alle Flags 0, Tuerweg = Bank NULL gegen Ladeweg = Bank vorher):
  ```
  ROOM11E0/11E1  Tuerweg  1 Platz ohne Zeilen/Bank | Ladeweg  1: id11 x1 (A0=10)            sub27 Schleife, Sleep 20
  ROOM2000/2001  Tuerweg  6 ohne Zeilen/Bank       | Ladeweg 21: id06 x3 (A0=10) id0b x18 (A0=41)   main00 einmalig
  ROOM20A0/20A1  Tuerweg  5 ohne Zeilen/Bank       | Ladeweg  5: id06 x5 (A0=10)            sub02 Schleife, Sleep 5
  ROOM5090..5141 Tuerweg 13..27 Plaetze, 0 ohne    | id08 (A0=17) aus CORE00 (Raumbaenke ohne 0x08) — unveraendert
  Raeume 206, mit Eintritts-Effekt 16, auf dem Tuerweg ohne Zeilen/Bank: 24 Plaetze
  ```
  (+ ROOM20B0/20B1 bei Flag (3,235)=1.) Schleifen-Effekte erschienen vorher eine Schleifenperiode spaeter, die Einmal-Spawns
  in ROOM2000/2001/20B0/20B1 nie.
* Messung vorher (Mutationsbau = alte Reihenfolge) / nachher, echte exe, Framedumps F0-F40, `ppmdiff.py`:
  | Lauf | Waffen-Log vorher | nachher | Pixel |
  |---|---|---|---|
  | ROOM20A0 Sprung | 5x `streams=-1`, 39x `=1`; FX id 6 ab F10 | 44x `streams=1`; FX id 6 ab F0 | 0 (Near-Gate: im Sprung-Cut hinter der Kamera, keine `->`-Zeile) |
  | ROOM11E0 Sprung | 1x `-1`, 1x `1` | 2x `1` | 0 (gecullt) |
  | ROOM2000 Sprung, Cut 0 | 6x `-1` | 3x `6`, 3x `1` | 0 (gecullt) |
  | ROOM2000 Tuer-Lage, `RE15_FORCE_CUT=9` | 6x `-1`, kein FX-Log | 3x `6`, 3x `1`; FX id 11 F1..32 (w16 177..225 je Lage), id 6 | **18** = 3 Punkte je 1 Pixel in F18-20/F24-26 bei (138..154, 71) |
  Die 18 Pixel sind die drei Spawn-Lagen von 0x0b in Zeile 0 (w/h 1, Flags 0x03, Routine 41 fehlt) — dasselbe Bild wie auf
  dem Ladeweg seit C2; das Original zeigt dort (Strom 0) den Strahl bzw. (Stroeme 1-5, Flags 0x61) nichts. Weg damit erst
  mit Routine 41/42 (INTEGRATIONSWUNSCH 6).
* Pin `integration_r34_plattform_esp_eintritt` (`tests/unit/probes/r34_plattform.cmake`, Skriptmodus `R34_PIN=esp_eintritt`):
  Debug-Sprung ROOM2000 (derselbe `re15_room_apply_pending`-Weg wie eine Tuer), Waffen-Log muss genau 3x
  `SPAWN id=11 sub=0 scale=0x1900 streams=6` und 3x `SPAWN id=6 sub=0 scale=0x1000 streams=1` tragen, kein `-1`.
  Negativ-Kontrolle des Auswerters vor dem Lauf (Zeile mit `streams=-1` zaehlt nicht). Gruen 9.2 s.
  **Mutationsprobe** (`nb_mut_oc7.py`: Aufruf im Teardown entfernt + alter Aufruf nach apply_pending): ROT
  `Effekt 0x0b beim Eintritt 3x gespawnt, davon 0x mit 6 Stroemen`, der Takt-Pin blieb gruen; `git checkout`, Neubau,
  beide gruen.

### N2 — M2: exe-Pin fuer die Lage des ESP-Takts

* `integration_r34_plattform_takt` (`tests/unit/probes/r34_plattform.cmake`, Skriptmodus `R34_PIN=takt`; Dateibesitz C —
  kein neues File unter `tests/integration/`): echte exe, `RE15_DEBUG_JUMP=1140@250`, `RE15_GIVE=3:15 RE15_EQUIP=3`, Skript
  `W1,M1,MA0.2,M2.5,W4` ab Spielbild 300 (Schuss F360), `RE15_FX_LOG` + `RE15_WAFFEN_LOG`, Ende F400.
  Pruefung: Spawnbild = F-Zeile des Waffen-Logs vor `SPAWN id=2 sub=0`; erste FX-Zeile `id=2 sub=0` muss im Spawnbild stehen
  mit `frame=1` und `fl=93`; keine Muendungszeile des Spawnbilds mit `fl=03`. Belege: @0x8001ce0c < @0x8001ce2c; Routine 8
  (0x80071d40[8] = 0x800175dc) `lbu v0,14(v1)` @0x800175ec / `sb v0,108(v1)` @0x800175f4; CORE00.ESP id 2 sub 0 Zeile 0
  @0x0FE0 (A=8, +0x0e = 0x93), Zeile 1 @0x1008 (A=9). Negativ-Kontrolle des Auswerters vor jedem Lauf (alte Zeile
  `frame=0 fl=03`, Zeile aus dem Folgebild -> verworfen; neue Zeile -> gilt). Gruen 22.6-33.0 s.
* **Mutationsproben** (`nb_mut_takt_exe.sh`, je Bau + Pin + `git checkout`):
  | Mutation (main.c) | Ergebnis |
  |---|---|
  | `re15_pc_fx_takt()` VOR `re15_game_step` (Aufruf dahinter entfernt) | ROT `Spawnbild 360: frame=0 fl=03, erwartet frame=1 fl=93` |
  | Aufruf hinter `re15_game_step` entfernt (nur der Rueckfall nach dem Zeichenblock tickt) | ROT dito |
* **Befund beim ersten Lauf** (fremde Datei, INTEGRATIONSWUNSCH 7): exit=1 bei Spielbild 17 nach dem Sprung, Waffen-Log
  mitten in der Zeile abgerissen. `tools/local_build.sh` setzt `CLEAN_PATH` ohne `WindowsPowerShell`
  (`command -v powershell` im Bau-PATH leer, gemessen), faellt deshalb in `do_build` auf `taskkill //F //IM re15_pc.exe`
  zurueck und beendet bei JEDEM `build` irgendeiner Sitzung alle `re15_pc.exe` der Maschine (auch die des Nutzers). Das
  erklaert die "exit=1 ohne Logmeldung"-Roten aller Suiten dieser Runde. Der Pin wiederholt deshalb genau einmal bei
  exit=1 (Signatur "von aussen beendet") oder Startfehler, nie bei einem falschen Ergebnis.

### N3 — Hinweise der Gegenpruefung
* **H4 behoben** (`main.c` vor `re15_re2z_gore_resolve`): `gore_draw/gore_tint/gore_mesh` werden fuer alle
  RE15_EMD_MAX_BONES Eintraege mit den Werten vorbelegt, die der Resolver selbst einem Slot ohne RE2-Part gibt (zeichnen,
  Tinte 0x808080, eigenes Mesh); 0x808080 = RE2 Part-Init `lui v0,0x80 / ori v0,v0,0x8080` @0x80028450-54,
  `sw v0,-36(s1)` @0x80028468. Vorher las die Zeichenschleife ab min(npc_bones,16) uninitialisierten Stapel (auch
  `gore_mesh`, Zeile `if (gore_mesh[nbi] < ...)`). Neutral = Identitaet, Renderpfad sonst bitgleich; `unit_re2_gore*`,
  `unit_re2z_*` (16 Tests) gruen.
* **H7 behoben** (Kommentare `fx_plattform_pc.h` C3-Block, `main.c` Licht-Latch zwei Stellen): nach der Latch-Loeschung
  laeuft zuerst `jal 0x80039ca0` @0x8001d1b8 (Tabelle 0x800af33c), dann `jal 0x8002c18c` @0x8001d1c0 (selbst gelesen,
  `dis 0x8001d1a4 10`). Welche Port-Objekte FUN_80039ca0 entsprechen, bleibt OFFEN (O-C9).
* **H1 unveraendert** (OFFEN O-C8): Bank 5 springt im Original ohne Satz-Tor auf die snd0-Tabelle (`table 0x80010e70 6`:
  [5] = 0x80045130 = das Ziel von Bank 2 hinter dessen Tor @0x800450e4-f0). Ein Satz >= 0x21 liest im Original jenseits
  der 0x21 Eintraege im RAM weiter. Im Port wuerde dasselbe ueber das Pufferende lesen: `re15_edt_decode`
  (`engine/src/vab_common.c:252-265`) rechnet `edt + se_id*4` ohne Obergrenze — das Tor schuetzt den Port vor einem
  Lesezugriff ausserhalb des Puffers. Deshalb unveraendert. Fuer die Granate (Bank 1/4) ohne Folge; ein Aufrufer mit
  Bank 5 ist nicht belegt.
* **H2 unveraendert**: die FX-Log-Zeile hinter die Gates zu legen aendert die V5-Zeilenmenge, auf der Auswerter der Spuren
  A/D verankern — Zuender-Phasen per Unit-Sonde abnehmen (Spur A).
* **H3**: INTEGRATIONSWUNSCH 8 (Part->Bone-Permutation exportieren), heute ohne Wirkung (keine Tinten-Schreiber).
* **H5/H6**: fremde Dateien bzw. Integration (INTEGRATIONSWUNSCH 7, BAUPLAN §4).

### N4 — Werkzeuge der Nachbesserung (`bau_c_werkzeug/`)
`nb_karte_raum.c` (Speicherkarte fuer den Ladeweg in einen beliebigen Raum), `nb_lauf_laden.sh` (Messlauf ueber den
Ladeweg), `nb_cut_regionen.c` (welche Cuts einen Weltpunkt zeigen), `nb_eintritt_zensus.c` (Eintritts-Spawns aller Raeume,
Tuerweg gegen Ladeweg), `nb_mut_takt_exe.sh`, `nb_mut_oc7.py` (Mutationsproben). Uebersetzungszeile im Kopf der .c-Dateien.

### N5 — Sonden-Stand
`unit_r34_plattform` (77), `unit_r34_plattform_ton` (8), `integration_r34_plattform_takt`,
`integration_r34_plattform_esp_eintritt` — alle gruen; Suite 430 -> 432 Tests.

### N6 — Volle Suite
* **Lauf N1** (`local_build.sh` = all, Stand `41523b8f`, 432 Tests, Log `build/r34g_c/nb/suite1_ctest.log`):
  `98% tests passed, 9 tests failed out of 432`, ctest 670.50 s, gesamt 11m19s, Abschluss
  `!!! [local_build] FEHLER: ctest fehlgeschlagen (exit=8)`. Die vier Tests dieser Spur gruen (`unit_r34_plattform`,
  `unit_r34_plattform_ton`, `integration_r34_plattform_takt` 27.5 s, `integration_r34_plattform_esp_eintritt` 14.5 s).
  Rot NUR exe-Tests: `integration_r30_cut_blitz` (Lauf A exit=1 nach 1.7 s), `integration_elza_vollstart` (ROOM1031 nie
  geladen), `integration_r30_granate_laden` (b exit=1), `integration_r30_irons_tisch_bild` (P0 exit=1),
  `integration_r30_titel_puls` (Periode (D) am Bild verfehlt — Bilddauer unter Last), `integration_r33_speichern`
  (a exit=1), `integration_boot_bg_pin` (exit=1), `integration_relatch_pin` (exit=1 nach 106 Log-Zeilen),
  `integration_save_counter_pin` (Lauf 3 exit=1). Parallel liefen die exe-Tests der Spuren A und B und eines
  r34n-Baums; zwei Sitzungen hatten ihre exe schon umbenannt (`re15_pc_nb.exe`, `re15_pc_r34n_a.exe`) — gegen den
  `taskkill /IM re15_pc.exe` aus INTEGRATIONSWUNSCH 7.
* **Einzel-Wiederholung** (`build/r34g_c/nb/suite1_rote_einzeln.log`, seriell): alle neun gruen — cut_blitz 75.50 s,
  elza_vollstart 100.70 s, granate_laden 152.26 s, irons_tisch_bild 63.80 s, titel_puls 28.82 s, r33_speichern 83.02 s,
  boot_bg_pin 3.69 s, relatch_pin 26.25 s, save_counter_pin 49.35 s. Kein reproduzierbares Rot.
* **Lauf N2** (`local_build.sh test` — ohne Bau-Schritt, damit dessen `taskkill` die Laeufe der Parallel-Sitzungen nicht
  trifft; derselbe Bau): (folgt)

---

## HINWEISE AN DIE SPUREN

* **A**: `wpos` wird vom Zeichner gelesen, sobald nicht alle drei 0 sind (Regions-Test + Projektion). Der Latch-Leser
  loescht `g_re15_licht_latch` nach den Figuren; Routine 9/31 muessen ihn nur setzen. `re15_esp_se_hook` ist gebunden
  (Bank 1 -> geladene ARMS-Bank, Bank 4 -> CORE; Byte1 egal). Kind-Plaetze mit Flags 0x0a sind unsichtbar, bis die
  Kind-Init Bit 0 setzt (Zeichner prueft Bit0 UND Bit1).
  Nachbesserung: der Zeichner nimmt defW/defH (+0x04/+0x06) fuer JEDEN Row-VM-Platz aus der Zeilenkopie — Routinen, die
  sie schreiben (41/42, 24/25) oder die Flags fuer die Halte-Phase setzen (41: 0x61 = unsichtbar), fehlen im Port
  (INTEGRATIONSWUNSCH 6). Seit N1.4 steht die Raum-Effektbank schon im Eintrittstakt; Effekt 0x0b in ROOM2000/2001
  (und 20B0/20B1) wird damit auch auf dem Tuerweg als Row-VM-Platz gespawnt und zeigt nach eurer Portierung den Strahl.
* **B**: `re2fx_applier` zeigt auf `re15_re2_gl_apply`. V4-Tinten fuer Hund/Spinne werden gezeichnet, sobald sie in
  `re2z_part_tint[i]` (i = Part = Bone) stehen und `re15_ai_re2_for_type(type)` gilt; ein Wort 0 gilt als ungesetzt.
* **D**: `re2fx_pc_draw()` wird im Effekt-Zeichenpass direkt nach `pc_draw_effects` gerufen; die Ansicht DIESES Bilds
  (cam_view, Bildmitte, Regions-Viereck, camf) liefert `re15_pc_fx_kamera()` (`platform/pc/src/fx_plattform_pc.h`,
  `gueltig` nur waehrend des Passes). TIM-Slots 52..55 sind frei (render_pc.c `RE15_TIM_SLOT_MAX` 56); ein TIM mit
  mehreren CLUT-Zeilen wird als Paletten-Stapel hochgeladen, `re15_render_textured_tri(..., clut, ...)` waehlt die Zeile
  ueber `clut >> 6` (Muster: Slots 50/51). `re2fx_register_core` wird beim Start mit `shared_assets/RE2/CORE00.ESP`
  gerufen (Puffer lebt), `re2fx_reset` im Raum-Teardown (`room_pc.c`), `re2fx_tick` hinter dem RE1.5-ESP-Tick,
  `re2fx_se_hook` -> ARMS10/ARMS11 Satz 10. Harness: `RE15_FORCE_AUFSCHLAG=2@<bild>` / `1@<bild>`.

## INTEGRATIONSWUNSCH (fremde Dateien — NICHT geaendert)

1. `engine/src/scd_room_setup.c:199` — neben `re15_esp_fx_reset();` auch `re2fx_reset();` (+ `#include "re2_fx.h"`):
   der zweite Raum-Reset-Weg (SCD-Raumaufbau) muss den RE2-FX-Pool ebenso leeren wie `room_pc.c` (Port-Zuordnung,
   RE1.5-Reset @0x80019378).
2. `include/re15_actor.h:519` (Spur B, V4) — `re2z_part_tint[16]` reicht nicht fuer alle Parts: Hund-Tod Brand faerbt
   17 Parts (`sltiu v0,s0,0x11` @0x80104774-800, EMD0G_MOD0), Spinne 20 Parts (`sltiu v0,a2,0x14` FUN_8010609C @0x8010609C-C4).
   Entweder das Feld auf 20 erweitern (dann `re15_pc_re2_part_tint` Grenze 16 -> 20, `fx_plattform_pc.c`) oder die
   Parts 16..19 bleiben neutral.
3. `release/make_package.sh:186-189` und `platform/android/app/build.gradle:113-115` — Existenz-Gate um
   `shared_assets/RE2/CORE00.ESP` (und `TEX.TIM`, sobald Spur D sie liest) ergaenzen (aus C0 uebernommen, weiter offen).
4. Android-Bau (andere Sitzung): `platform/pc/src/fx_plattform_pc.c` ist NEU (GLOB-Cache `app/.cxx` verwerfen; beim
   Release-Bau tut `release/build_android.sh:198` das selbst).
5. Hauptbaum/Integration: `tools/local_build.sh` `RE15_MIN_TESTS` unveraendert (428); Spur C bringt +4 Tests
   (`unit_r34_plattform`, `unit_r34_plattform_ton`, `integration_r34_plattform_takt`,
   `integration_r34_plattform_esp_eintritt`) — Suite dieses Baums 432.
6. **Spur A**, `engine/src/re15_esp.c:710` (`default: break;` in `esp_fx_dispatch`, Zeile 523): die ESP-Routinen
   **41** (0x80018ef4) und **42** (0x80018f98) portieren, danach **24** (0x80017f50) / **25** (0x80017fa4) und
   **13** (0x80017990) / **19** (0x80017d08) — Instruktionen je Routine in N1.1 (Tabelle). Braucht: +0x70 = Effekt-Id,
   +0x72 = scale, +0x78 Satzbasis, +0x80 Strom-Anfang (Spawner @0x800197d4/dc/@0x80019894/@0x80019900; im Port
   `effect_id`/`scale16`/`rows_base`), fuer 42 den Treffer-Test FUN_8002b7e8 (alle aktiven Entitaeten + Spieler,
   FUN_8002b5d0, a1 = 0x2d), fuer 25 room_coll + FUN_80012d60 + Kind-Spawn 0x800199d4. Abnahme danach: ROOM2000 ueber die
   Tuer aus ROOM2050 (Ankunft (-26450,0,10250)) in einem Cut 5..10/13 — Strom 0 sofort sichtbar mit h 0x19c4, wachsend
   um 768 je Takt ab Zaehler 16, Schleife alle 25 Takte; Sonde: `nb_eintritt_zensus.c`-Aufbau + Takte, Zeilen-Cursor und
   Flags gegen N1.1.
7. `tools/local_build.sh:147/293-300` (alle Baeume): der Bau-PATH `CLEAN_PATH` enthaelt kein
   `/c/Windows/System32/WindowsPowerShell/v1.0`, `command -v powershell` ist dort leer (gemessen) -> Rueckfall
   `taskkill //F //IM re15_pc.exe` (Zeile 299) beendet bei jedem `build` ALLE `re15_pc.exe` der Maschine (fremde
   Testlaeufe, das Spiel des Nutzers). Vorschlag: PowerShell mit absolutem Pfad rufen und den `taskkill /IM`-Zweig
   streichen. (Deckt sich mit dem Selbstbefund der Gegenpruefung Spur A.)
8. `engine/src/enemy_ai_re2_zombie.c:1032/4306` (Spur B, Hinweis H3): `re2z_part_to_bone` ist `static`;
   `re15_pc_re2_part_tint` (`fx_plattform_pc.c`) setzt Part i = Bone i voraus. Sobald Spur B Tinten fuer Aktoren schreibt,
   deren Bank nicht das RE2-EMD ist (RE1.5-Rueckfall in `pc_enemy_load_ex`), die Permutation exportieren
   (`include/re15_enemy_ai.h`) — dann bildet C7 Part -> Bone darueber ab.

## OFFEN

* O-C1 exe-Messung der Granaten-Toene (8x `SE  esp code=0x010A..` + 1x CORE 8 je MITTE-Wurf, `RE15_NOAUDIO`-Waffen-Log),
  der Wurf-Zeichnung ueber `wpos`, des Explosions-Lichts und der RE2-Aufschlaege: erst nach dem Merge von Spur A
  (Routinen 29/30/31, wpos, Latch-Setzer) bzw. Spur D (Maschine, Zeichner). Vorbereitete Haken/Harness stehen.
* O-C2 Flags-Bit 3 (zweites Zeichnen mit der Matrix 0x800b5288, @0x80053394-0x80053470) nicht portiert: kein heutiger
  Effekt traegt Bit 3 zusammen mit Bit 0+1 beim Zeichnen (Kinder 0x0a sind bis zur Init unsichtbar).
* O-C3 Reihenfolge verschiedener Plaetze innerhalb EINES 64er-OT-Eimers (Original: Kette, Port: exaktes vz) — portweite
  Konvention des Renderers, nicht granatenspezifisch.
* O-C4 Raum-Bank-Effekte (RDT-TIMs, Slots 36..43) zeichnen weiter mit Palette 0 ihres Blatts; Paletten-Versatz
  (sub>>3, Routine 10) fuer Raum-TIMs ist nicht umgesetzt (vor Runde 34 ebenso; ausserhalb des Granaten-Umfangs).
* O-C5 Lage-Flag (Byte0) der SEs ohne Wirkung (positionaler Zweig FUN_80045a64, BAUPLAN O5).
* O-C6 Laufzeit-Flattern: exe-Laeufe haengen unter der Last der Parallel-Spuren gelegentlich im Titel (vor der
  Spielschleife) bzw. der msys-Prozess wird nach `_exit` nicht abgeraeumt (exe fertig, Bilder/Logs vollstaendig,
  `timeout` beendet); betrifft fremde Spuren ebenso (gesehen: Spur-A-exe). Nachbesserung: die "exit=1 ohne Logmeldung"-Abrisse haben
  eine gemessene Ursache — INTEGRATIONSWUNSCH 7 (`local_build.sh` beendet bei jedem Bau alle `re15_pc.exe`).
* O-C7 GESCHLOSSEN (Nachbesserung N1.4): Raum-Effektbank vor dem SCD-Eintrittstakt, Pin `integration_r34_plattform_esp_eintritt`.
* O-C8 Bank-5-Satztor (H1): bleibt, weil `re15_edt_decode` keine Obergrenze kennt (N3).
* O-C9 Welche Port-Objekte der Tabelle 0x800af33c (`jal 0x80039ca0` @0x8001d1b8, hinter der Latch-Loeschung, also OHNE das
  Ein-Bild-Licht) entsprechen, ist nicht zugeordnet — laufen sie im Port in der Figuren-Schleife, bekaemen sie das Licht
  faelschlich (H7).
* O-C10 Sichtabnahme Effekt 0x0b (ROOM2000/2001/20B0/20B1) erst nach INTEGRATIONSWUNSCH 6; bis dahin zeigt der Port dort in
  den Cuts 5..10/13 fuer 27-32 Takte drei 1-Pixel-Punkte (Zeile 0 w/h 1, Flags 0x03) statt des Strahls (N1.4, Messung).
