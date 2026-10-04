# Runde 35 Spur F "inhalt" — Dossier (fortlaufend)

Baum: `.claude/worktrees/r35_inhalt`, Zweig `r35/inhalt`, Basis master 154a73c1 (geprueft: status leer).
Zuteilung (VERTRAG.md): Bank-9-Bits 80 (Memory Card 1010), 81 (Schrot 1090); Nachrichten-IDs 1010 ab 3
(max 5), 1090 ab 10 (max 12); Ereignisse 28 (1010), 29 (1090).

## Punkte (Wortlaut AUFTRAG.md)
1. Doppeltueren unsymmetrisch (Griffe)
2. Codes in den selbsterstellten Dokumenten gruen
3. ROOM1010 / ROOM1220 stehende Zombies weiter von der Tuer
4. Memory Card im Regal ROOM1010 (add_card.bmp, Marke (272,157))
5. Schrot-Munition auf dem rechten Aussenluefter ROOM1090 (Shotgun.bmp, Marke (107,133))
6. RE2-Karten-Weltmodelle (World Items) extrahieren und ablegen

## Protokoll

### Punkt 2 — Codes in den eigenen Dokumenten gruen

**Messung vorher.** Die eigenen Dokumente sind GERASTERTE 4bpp-Seiten im RE2-FILE-Format
(`shared_assets/RE2/FILES/FILE25..29_*.TIM`, gesetzt mit `tools/re2_doc_satz.py` +
`tools/r34n_e/doc_satz_brief.py`); der Port zeigt sie mit ihrer EIGENEN CLUT
(`engine/src/re2doc_common.c` re15_re2doc_pixel: CLUT-Eintrag des Seiten-TIMs).
Codes stehen nur in zwei Texten (grep Ziffern in `analysis/befunde_runde34_nacht/E_texte/*.txt`
und `analysis/befunde_runde30/irons_diary_en.txt`): dok3 Marvin "4312" (FILE28), dok4 Armory
"5632" (FILE29). Irons Diary = nur Datumsangaben, Elliot "Only 3 more hours" = kein Code.
Index-Zensus ueber alle 191 RE2-Textseiten (Skript scratchpad clut_zensus.py): Pixel nutzen nur
Index 0 (Papier), 1..6 (Kern-Grauverlauf), 8 (Kontur), Index 15 genau 1 Pixel; **9..14 = 0 Pixel**.
RE2 selbst faerbt in seinen Dokumentseiten also nichts — "diesem Gruen" ist das Gruen der
Spieltexte, in denen das Original die Codes faerbt.

**RE-Beleg (Original-Codes sind gruen).** Die zwei Original-Nachrichten mit den Codes:
- ROOM1110.RDT @0x0DB0: `61 05 01 10 0f 0d 0e 05 00` = ":" Farbe 1, "4312", Farbe 0
- ROOM1230.RDT @0x172F: `61 05 01 11 12 0f 0e 05 00` = ":" Farbe 1, "5632", Farbe 0
Steuerbyte 0x05 im Textmaler FUN_80028868 (PSX.EXE, selbst disassembliert):
```
8002896c  lbu  v0,0(a2)        ; Argument N
80028974  andi v1,v0,0x4
80028978  sltu v1,zero,v1      ; (N&4)!=0
8002897c  andi v0,v0,0x3
80028980  sll  v0,v0,1         ; (N&3)*2
80028984  addiu v1,v1,480
80028988  addu v0,v0,v1
8002898c  sll  v0,v0,6
80028994  ori  s5,v0,0x10      ; CLUT = (x 256, y 480+(N&3)*2+((N&4)!=0))
```
N=1 -> CLUT-Zeile y 482 = Zeile 2 des CLUT-Blocks von DATA/TEX.TIM (Kopf: CLUT x 256 y 480,
32x24). Rohbytes TEX.TIM @0x94 (Zeile 2, Eintraege 0..6):
`00 00 e0 16 80 12 20 0e e0 09 80 09 40 05` -> 1..6 = (0,184,40) (0,160,32) (0,136,24)
(0,120,16) (0,96,16) (0,80,8).
Zeile 0 (weiss, @0x14) Eintraege 1..6 = `7b 67 39 5f b5 4e 31 42 ce 35 4a 29` =
(216,216,200)..(80,80,80) — **RGB-gleich** dem Kern-Grauverlauf 1..6 der RE2-Dokumentseiten
(Zensus oben, CLUT[1..6] auf allen 191 Seiten gleich). Dieselbe Rampe, also bildet Index i des
Dokument-Kerns 1:1 auf Eintrag i der gruenen Zeile ab — keine Schaetzung noetig.

**Entscheidung (Umsetzung).** Satz-Werkzeug `doc_satz_brief.py` bekommt `--gruen TOKEN`:
die Kernpixel der Glyphen des Tokens bekommen Index v+8 (9..14), die CLUT-Eintraege 9..14 der
Seite = TEX.TIM Zeile 2 Eintraege 1..6 (gelesen aus der Datei, kein Zahlenwert im Werkzeug).
Kontur (Index 8) bleibt die der RE2-Seite (PORT-WAHL: die Dokumentkontur (24,24,32) ist nicht die
des Textmalers (56,48,72), es gibt kein Original-Gegenstueck fuer eine gruene Dokumentkontur).

**Umsetzung Punkt 2.**
- `re15_port/tools/r34n_e/doc_satz_brief.py`: `--gruen TOKEN` (Optional 5 im Kopf): Kernpixel
  1..6 der Token-Glyphen -> 9..14; CLUT 9..14 = TEX.TIM Zeile 2 Eintraege 1..6 (`tex_zeile(1)`,
  Formel @0x80028974-94), Pruefung Vorlage-CLUT[1..6] == TEX.TIM Zeile 0 [1..6] (sonst Abbruch);
  Kontur mit Kern 1..6+9..14; nur Seiten MIT Token bekommen die erweiterte CLUT.
- `re15_port/tools/r34n_e/satz_bauen.sh`: FILE28 `--gruen 4312`, FILE29 `--gruen 5632`;
  Soll-Liste jetzt `analysis/befunde_runde35/F_belege/satz_md5.txt` (FILE26/27-Zeilen unveraendert
  aus der r34-Liste). Lauf: alle vier "gleich der md5-Liste" (reproduzierbar).
- Neue Assets (Paket-/Android-Gate, GROESSENGLEICH -> Gate-Pin noetig!):
  `re15_port/shared_assets/RE2/FILES/FILE28_p01_page.TIM` (md5 ad9d0f6f..., vorher 69268c01),
  `re15_port/shared_assets/RE2/FILES/FILE29_p01_page.TIM` (md5 f39ce419..., vorher e9c2c39d).
- Pixel-Differenz alt/neu (gemessen): FILE28_p01 82 Pixel, nur x 147..179 y 100..107, Paare
  (1,9)..(6,14); FILE29_p01 104 Pixel, x 5..36 y 148..155; CLUT 0..8 und 15 unveraendert.
  Bericht: FILE28 "p01 Zeile 6 (y 96..111): '4312' Kern-x 147..179 gruen", FILE29 "p01 Zeile 9
  (y 144..159): '5632' Kern-x 5..36 gruen".

### Punkt 6 — RE2-Welt-Item-Modelle extrahiert und abgelegt

**Wortlaut** "die Karten Modelle in der Welt zum Einsammeln, also die World items, aus Resident
Evil 2 extrahierst und irgendwo ablegst wo ich es sehen kann". Abgelegt sind ALLE RE2-Welt-Items
(Leon, info/re2leon/PL0/RDT, 495 RDTs) — "die World items" — mit eigenem Karten-Bogen fuer die
einsammelbaren Karten (Card Keys).

**RE-Beleg (Mechanismus, RE2 PSX.EXE, Belegkette aus re2_doc_worldmodels.py, Commit 2b95b592):**
Item_aot_set Op 0x4E -> LAB_80054CD4 (Tabelleneintrag @0x800A7600); Record 22 Byte, +18 flag
(`lhu a1,0x12(s0)` @0x80054CF4), +20 md1 (`lbu s2,0x14(s0)` @0x80054CF8); md1 < 32
(`sltiu v0,s2,0x20` @0x80054D98) = Slot der Raum-Modelltabelle (Lader FUN_80052D14: nOmodel
`lbu s2,0x2(v0)` @0x80052D70, Tabelle `lw s4,0x30(v0)` @0x80052D74, Schritt 8 @0x80052DF4);
md1 = 255 = kein Weltmodell. Das Modell haengt an der PLATZIERUNG, nicht an der Item-Id.

**Umsetzung.** Neues Werkzeug `tools/re2_sicherung/re2_item_worldmodels.py` (verallgemeinert
re2_doc_worldmodels.py von Dokumenten auf alle Ids; Op 0x69 4-Punkt-Zonen mit erstem Punkt als
Lage). `tools/re2_sicherung/re2_scd_walk.py`: `RE15_INFO_DIR`-Ausweichpfad fuer
information293.txt (schlanker Baum; nur gelesen aus dem Hauptbaum).
Aufruf: `RE15_INFO_DIR=C:/workspace/git/reAi_v2/info/Resident_Evil_und_Playstation_Information
C:/Python310/python.exe tools/re2_sicherung/re2_item_worldmodels.py` (18 s).

**Ergebnis (gemessen, `extracted_re2_items/_bericht.txt`):** 269 Item_aot_set-Platzierungen,
206 mit Weltmodell, 63 ohne (md1 255); **81 verschiedene Meshes** (md5 der MD1-Bytes), 63 Item-Ids
mit Weltmodell. SCD 2553 Bloecke, 45 desynchron (94,55 % der Bytes gewalkt — dieselbe Dunkelziffer
wie re2_doc_worldmodels). Karten: mesh043 Blue Card Key (room10B0 main00 @0x01714), mesh066/067 Lab
Card Key (room60A0 sub18 @0x014B4, room6150 sub00 @0x02CFE); Red Card Key (room2150 @0x012A8) hat
md1 255 = kein Weltobjekt. 8 Platzierungen mit md1 >= nOmodel (im Bericht einzeln, z.B. room1000
sub01 Id 6 md1 7 bei nOmodel 0) — der Pool-Slot kommt dort nicht aus der Raumtabelle.
**Fuer den Nutzer:** `extracted_re2_items/uebersicht.html` (Galerie, zwei Ansichten je Mesh, Namen,
Ids, Raeume), `kontaktbogen.png` (alle 81), `karten.png` (die 3 Card-Key-Meshes), `katalog.csv`
(Item-Id; Name; Karte; Meshes; Bilddateien; Platzierungen), `platzierungen.csv`,
`ohne_modell.csv`, `modelle/meshNNN_<md5>.{md1,tim,obj,_a.png,_b.png}`.
Lageplaene ("map") sind in RE2 KEINE Items: die Namentabelle 0x00..0x8B fuehrt 0 Maps (3 Card
Keys); die 7 Lageplan-Funde ("police station map" ROOM20B0 @0x3BA6, "police B1 map" ROOM2130
@0x1B41, "sewage disposal map" ROOM3060 @0x2B4E, "sewer map" ROOM4040 @0x1BAE, "factory map"
ROOM5040 @0x2357 / ROOM5060 @0x110C, "laboratory map" ROOM6120 @0x18B1) laufen ueber
Nachricht+Ja/Nein-Ereignis, nicht ueber Item_aot_set (OFFEN unten, falls der Nutzer DIESE meinte).

**Messung nachher Punkt 2 (echte exe, beschleunigter Renderer, RE15_FRAMEDUMP; gdigrab liefert in
dieser Sitzung weisse Bilder).** Kopie `re15_pc_r35f_doc.exe` neben der exe, Arbeitsverzeichnis mit
`analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr`, `RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1 RE15_DOC=28|29 RE15_PAD_AT=400:S,520:M,580:A,600:A,700:R` (Muster
tools/r34n_e/leser_lauf.sh, jetzt ohne Umbenennung, die Tabelle kennt FILE28/29), Framedump F780 =
Seite 2/3: `F_belege/p2_exe_FILE28_p01.png` ("The new code is: 4312", 4312 gruen),
`F_belege/p2_exe_FILE29_p01.png` ("...with the code: 5632", 5632 gruen). Restlicher Text weiss.
**Test:** `unit_r35_inhalt_doku` 19/19 (A Gruen = TEX.TIM Zeile 2 aus der Datei; B je Code 82 bzw.
104 gruene Pixel, alle im Code-Kasten, ganze Breite, 0 weisse Kernpixel im Kasten; C FILE25..29
sonst 0 gruene Pixel). Gegenprobe: mit den alten Seiten faellt B (0 gruene Pixel).

### Punkt 3 — Zombies ROOM1010 / ROOM1220 weiter von der Eintrittstuer

**Original-Daten (selbst gelesen, scd_dump_room.py).** Spawns = Sce_em_set (Op 0x44, 20 Byte, x/y/z
pc+8/10/12, Richtung pc+16, Kill-Flag pc+7) in sub00, Switch auf work_vars[0x0A] = Eintritts-Cut
(Tuer-Payload Byte 10, FUN_8001d600 @0x8001d948).
- ROOM1010 sub00 Case 0: @0x009E2 Typ 0x10 beh 0x00 (3750,5000) Richtung 3072; @0x009F6 (1200,7150)
  Richtung 0 (= die "stehenden"); Case 4: @0x00A12/@0x00A26 beh 0x81 (Kriecher) (950,-1700)/(-50,-3800).
- ROOM1220 sub00 (5 Zellen, je Cut 2 Zombies Typ 0x16): Case 0 @0x00F2A/@0x00F3E, Case 2 @0x00F5A
  (-15850,-11100)/@0x00F6E (-14900,-8050), Case 4 @0x00F8A/@0x00F9E, Case 6 @0x00FBA/@0x00FCE,
  Case 8 @0x00FEA/@0x00FFE. ROOM1221 bytegleich; ROOM1011 dieselben Saetze mit Slot+1 (Slot 0 = Typ 0x47).
- Eintrittspunkte = Ziele der Tuer-Saetze im Nachbarraum (Door_aot_set +14/+18/+20/+24):
  ROOM1020 @0x01CA2 -> 1010 (3650,6900) yaw 2048 Cut 0; @0x01C82 -> 1010 (3650,-3950) Cut 4;
  ROOM1210 @0x01CE6/@0x01D06/@0x01D26/@0x01D46/@0x01D66 -> 1220 Cut 0/2/4/6/8.

**RE-Belege (Mechanik, die "Chance" begrenzt):**
- Drehen auf der Stelle 96/Bild: TURN-IN-PLACE-Paar @0x80073ee4 = [0,96] (player_common.c) ->
  180 Grad = 22 Bilder.
- RE2-Zombie (Auslieferungs-KI): DECISION[0] @0x80101294: `dist<0x1388` -> Gang 0x101
  (@0x80101308-1C); `dist<0xBB8` + Kegel 800 + Sicht + rand -> Sprung-Biss 0x0C01 (@0x80101374-78);
  DECISION[2] @0x80101F7C: `sltiu 0xbb8` @0x801020A8 -> 0x0C01; Griff 0x0301 bei `sltiu 0x4b0`
  (1200) @0x80102114. EXEC[12] @0x80104748 = Sprung-Biss mit Schub-Clip 0x1B (82/141/212 je Bild).
  => unter 3000 (0xBB8) kann der Zombie jederzeit in den Schub-Sprung gehen.

**Messung vorher (echte Spielschleife, Riegel `test_r35_inhalt_zombies mess`, RE2-KI):**
```
Raum Cut Eintritt         naechster STEHEN-Griff  FLUCHT (sofort drehen + VIERECK)  Flucht-min
1010  0  (3650,6900)        1902        48        raus Bild 23                        1546
1010  4  (3650,-3950)       3514         0        raus Bild 22                        3514
1220  0  (-22400,-6500)     2607         0        raus Bild 22                        2607
1220  2  (-16600,-9900)     1415        12        GEGRIFFEN Bild 12                    998
1220  4  (-22400,-14000)    3567       347        raus                                3567
1220  6  (-16600,-17900)    2683        86        raus                                2327
1220  8  (-16600,-25050)    2912       168        raus                                2912
```
Echte exe (Karte probe_r35_inhalt_karte in ROOM1020 (-4000,-18000) rot 0, VIERECK -> Tuer -> 1010):
ohne Eingabe nach dem Eintritt Griff in Bild 53 (state.log gr=1); mit sofortigem Drehen ab Bild 1
zurueck in ROOM1020 in Bild 26 — also nur mit sofortiger, fehlerfreier Reaktion; 1220 Cut 2 ist
gar nicht zu entkommen (Griff Bild 12 waehrend des Drehens).

**Regel (PORT-WAHL zur NUTZER-VORGABE, Form belegt).** Ein Zombie ist zu nah, wenn er waehrend der
schnellstmoeglichen Flucht unter die Sprungschwelle 0xBB8 = 3000 kommt. Er wird um die KLEINSTE
Strecke versetzt (Ringe 100, 200, ..., 64 Richtungen um die Original-Lage), bei der er die ganze
Flucht ueber >= 3000 bleibt und die Flucht "raus" endet; auf dem Ring gewinnt der Ort mit dem
groessten Abstand zum Eintritt (= "weiter zurueck"); nur Orte mit freiem Weg von der Original-Lage
(Zellen-Strahl der Gegner-Wandklemme, Band 0, Maske 4) und freier Grundflaeche
(re15_collision_box_blocked = FUN_8003b558-Port, Radius hit_radius_min). Zwei Durchgaenge (einzeln,
dann gemeinsam). Werkzeug: `test_r35_inhalt_zombies suche`.

**Umsetzung.** `engine/src/zombie_abstand_r35.c` + `include/re15_zombie_abstand.h` (Tabelle,
Schluessel = Raum-Basis + Typ + ORIGINAL-x/z = Satz-Waechter), Haken 3 Zeilen in
`engine/src/scd_vm.c` op_sce_em_set nach der 5090-Umtypung (+1 include). KEIN RDT-Patch; Typ,
Verhalten, Richtung, Kill-Flag bleiben.
```
1010 @0x009E2 (3750,5000)    -> (3750,3500)     Versatz 1500
1010 @0x009F6 (1200,7150)    -> (205,7248)      Versatz 1000
1220 @0x00F2A (-25000,-6700) -> (-25398,-6739)  Versatz  400
1220 @0x00F5A (-15850,-11100)-> (-13362,-10345) Versatz 2600
1220 @0x00F6E (-14900,-8050) -> (-14264,-7414)  Versatz  900
1220 @0x00FBA (-15400,-15500)-> (-15070,-14883) Versatz  700
1220 @0x00FFE (-14350,-23200)-> (-14267,-23144) Versatz  100
```
Unveraendert (schon >= 3000): 1010 Kriecher, 1220 @0x00F3E, Cut 4 beide, @0x00FCE, @0x00FEA.

**Messung nachher (Riegel `unit_r35_inhalt_zombies` 22/22):** alle Eintritte FLUCHT raus (Bild
22/23), Flucht-Mindestabstand 3044/3514/3007/3005/3567/3083/3012/3005 >= 3000; STEHEN-Griff 1010 Cut 0
48 -> 102, 1220 Cut 2 12 -> 96 (1221 ebenso); Spawn: 9 Saetze auf neuer Lage, 7 unveraendert,
0 falsch. Gegenprobe A: ohne Tabelle 1220 Cut 2 gegriffen (Bild 12), 1010 Cut 0 Flucht-min 1546.
Echte exe nachher (derselbe Tuerweg 1020 -> 1010, ohne Eingabe nach dem Eintritt): Spawn
`[1 t=10 @(3750,3500,r3072)] [2 t=10 @(205,7248,r0)]` (state.log F1), Griff erst in Bild 107
(vorher 53). Framedumps F20/F50/F80 nach dem Eintritt: `F_belege/p3_exe_1010_cut0_F20_F50_F80.png`
(beide Zombies im Raum, laufen an; keiner in Wand/Moebel).

### Punkte 4 und 5 — Memory Card im Regal ROOM1010, Shotgun Shells auf dem Luefter ROOM1090

**Messung vorher.** Kein Item an beiden Stellen (Sonde `aots 1090`: Slots 0..3 + Kamerazonen ab 39;
1010: Slots 0..7, Dokument 4 Slot 9). Marken (re15_port/tools/r34n_e/marken.py gegen alle
Hintergruende, probe_bg_dump):
- add_card.bmp -> ROOM1010 **Cut 7** (Abweichung 3,11 gegen 63,46), 130 rote Pixel x 269..278
  y 158..170, Mitte (274,0 ; 164,5) (AUFTRAG nennt (272,157) — gemessen ist die Huelle).
- Shotgun.bmp -> ROOM1090 **Cut 2** (7,01; Cut 10 = dieselbe Kamera, Matrix/pos/tgt gleich, 8,41),
  400 rote Pixel x 98..117 y 127..146, Mitte (108,0 ; 137,0).

**RE-Belege (FORM).** Obj_model_set LAB_80040914 (obj = Pool-Index, Typ @0x8004095c, Band
@0x80040974, Flags | 1 @0x80040998); Item_aot_set @0x80040644 (+18 Bit @0x80040680, +20 Prop
@0x80040684, Bit gesetzt -> still + Modell weg @0x800406dc-718); Aufnahme-Modal FUN_8001db28 (Ja ->
Zone 0 @0x8001e090, einfuegen @0x8001e0c4, Bank-9-Bit @0x8001e0d0/@0x8001e0d4); Aktionspunkt 620
voraus @0x80042bd0; Etagen-Tor des Scans (rec[2] == Spieler-Etage ausser Bit 0x80,
aot_common.c @0x80042cb4-ccc). Item-Ids aus DEBUG.BIN (Offsettabelle @0x495C, Blob @0x4A28):
0x21 "Memory Card", 0x16 "Shotgun Shells".
Zensus Shotgun Shells (re15_port/tools/r35_inhalt/schrot_export.py, alle 240 RDTs): 28 Saetze,
Menge 7 in 22 (sonst 14); Meshes md5 11f139fa.. (12 Saetze, STAGE1: 1010/1011/1110/1111/1190/1191/
11D0/11D1) und e2baafec.. (10); 6 ohne Modell. Gewaehlt: haeufigstes = ROOM1010.RDT Prop 2
(Item_aot_set @0x009C2), MD1 @0x0015DC 484 B, TIM @0x024F48 17440 B -> `gen/r35_schrot_prop.inc`
(UNVERAENDERT eingebacken). Memory Card: RE1.5 platziert 0x21 nirgends -> dasselbe Modell wie die
Hebetisch-Karte (Keycard-MD1 ROOM1110.RDT @0x0013D8 + Karten-TIM, re15_irons_tisch_md1_bytes),
Menge RE15_IRONS_KARTE_MENGE = 3.

**Lage (PORT-WAHL aus Messung, Werkzeuge: probe_r34n_e_dokumente `kamera`/`sicht`, geom.py,
scratchpad ray_box.py):**
- Karte: Strahl der Markenmitte (Cut 7) tritt bei (3600,-2055,-1708) in SCA 4 (x 3600..4600,
  z -2050..1450, Regal) = Fach zwischen zwei Brettern. Bretthoehen gemessen in Cut 4 (frontal):
  mittlere Helligkeit je Hoehe auf der Regalfront x 3600 ueber z -1900..1350: Brett A -2475..-2400,
  Brett B Vorderkante -1725..-1650 (Abfall bei -1625), Brett C -1150..-850. Strahl trifft
  Brett B (y -1725) bei (3796,-1725,-1033) = Kartenmitte; Ursprung (Ecke) (3877,-1725,-1168), rot 0.
  Vorwaerts Cut 7 (273,96 ; 164,46); Cut 7 hat keine Masken (sicht: 0 ueber dem Pixel).
- Kiste: Luefter = SCA 32 (x -3400..3400, z -17400..-15397, Band 5 = Boden -9000). Bild-rechts =
  Welt -x. Oberkante des rechten Luefters: Helligkeitssprung Wand->Luefter in Zeile 146/147 fuer alle
  Spalten 85..135 -> Strahl (108;146,5) auf der Front z -15397: y -10761. Mesh-Huelle x +-270,
  y -342..0, z +-103 -> Ursprung (-2408,-10761,-15500) rot 0; Huelle vorwaerts x 99,7..116,2
  y 136,7..146,6 (Marke x 98..117). Cut 2/10 ohne Masken.
- Zonen 1000 x 1000 mittig (haeufigste Groesse 69/162): Karte Slot 10 x 3296..4296 z -1533..-533
  Etage 0; Kiste Slot 4 x -2908..-1908 z -16000..-15000 Etage 5. obj 4 in beiden Raeumen
  (TIM-Slot 8). Bits (9,80) / (9,81) (VERTRAG).

**Umsetzung.** `engine/src/inhalt_r35.c` + `include/re15_inhalt_r35.h` (alle Konstanten mit Beleg),
`engine/src/gen/r35_schrot_prop.inc` (Generator `tools/r35_inhalt/schrot_export.py --schreiben`),
Haken: `scd_room_setup.c` +2 Zeilen (include + Installer am Blockende "Runde 35 Spur F"),
`platform/pc/main.c` +1 include, +1 Boot-/CONTINUE-Installer, +6 Zeilen Modell-Lader (Muster
Dokumente). Keine neuen Nachrichten (das normale Aufnahme-Modal zeigt Name + Frage) -> keine
Sprachdateien. Nachrichten-IDs/Ereignisse der Spur F unbenutzt.

**Messung nachher.** `unit_r35_inhalt_items` 25/25: Prop/Zone in 1010/1011/1090/1091, Bit -> weg,
Projektion Cut 7 (274,00;164,51) in der Marke, Cut 2 (108,01;141,52) in der Marke, echter
Aktionsdruck (re15_aot_scan) von (3300,-1033) yaw 0 bzw. (-2408,-9000,-14900) yaw 1024 (Etage 5)
-> Aufnahme-Modal 0x21 / 0x16 -> Ja -> Bit gesetzt, Item im Inventar (Memory Card 3, Shells 3 =
Nutzer-Halbierung re15_pickup_menge_nutzer im Modal). Echte exe (Karte probe_r35_inhalt_karte,
CONTINUE, RE15_FORCE_CUT=7 bzw. 2, RE15_FRAMEDUMP F100): `F_belege/p45_exe_marke_gegen_framedump.png`
(links Nutzerbild, rechts Port) und `F_belege/p45_exe_zoom.png`: die Karte liegt als kleines graues
Viereck im Regalfach unter der Marke, die Kiste steht auf dem rechten Luefter unter der Marke.
Hinweis: die grosse Kiste rechts im 1090-Bild ist das Original-Prop obj 1 (Obj_model_set @0x02190,
Typ 4, (-5960,-9000,-15482)) — nicht von dieser Spur.

### Punkt 1 — unsymmetrische Griffe an Doppeltueren der Tuersequenzen

**Messung vorher** (neue Sonde `probe_r35_inhalt_tueren`, Tuer-Maschine re15_door_seq_start /
re15_door_seq_bild = Door_init FUN_80013c1c / Door_move FUN_80013eb4; jede Wahl der Tuer-Tabelle
(Archiv, Variante, Port-Archiv, Spender), Bild 2; je Griff-Objekt die Weltmatrix, mit der der Laeufer
zeichnet — ohne Tausch o->welt, mit Tausch die Formel aus door_scene_pc.c griff_zeichnen; Mass:
Spitze (fernste Ecke) bzw. Huellen-Mitte relativ zum Anhaengepunkt; Spiegelebene = Mittelebene
zwischen den SCHWERPUNKTEN der beiden Fluegel-Meshes; nur Paare gleicher Griffe):
```
                      Spitze   Huellen-Mitte
RE2 DOOR1B V2          2,7      9,0      (Original, Referenz)
RE2 DOOR1D V3          0,0      0,0      (Original, Referenz)
P04B DOOR04 V2         0,2     18,4      (RE2-Geometrie, Mitte-Mass streut bei kurzen Vektoren)
P1DG/P1DK/P1DL 1D V3  49,7     55,7      <- UNSYMMETRISCH (Griff-Tausch <- DOOR07)
P1BD DOOR1B V2        37,1     47,6      <- UNSYMMETRISCH (Griff-Tausch <- DOOR23)
P0CD DOOR0C V0       180,0    179,9      <- UNSYMMETRISCH (Archiv-Griff, Stangen oben/unten)
```
Alle drei Befunde sind PORT-Archive der Runde 33 ("die wir erstellt haben"); kein RE2-Original ist
betroffen. (DOOR26/DOOR31 V0 sind RE2-Archive mit Rad + Hebel = verschiedene Meshes, kein Paar.)

**RE-Beleg (Ursache).** Die Archive spiegeln den Griff des zweiten Fluegels ueber die z-Drehung
ihres Door_model_set (pc+16..21 -> rot0, Port-Feld re15_door_obj_t.rot0): DOOR1D V3 obj 2
(62708,2048,0) / obj 3 (62708,2048,2048); DOOR1B obj 4 (63488,2048,0) / obj 5 (63488,2048,2048);
DOOR0C obj 2 (0,0,0) / obj 3 (0,0,2048), obj 4 (0,2048,0) / obj 5 (0,2048,2048). Gemessen: jeder
Griff mit rot0[2] = 2048 haengt am zweiten Fluegel (obj 1); alle Eintueren und alle Griffe am ersten
Fluegel tragen rot0[2] = 0 (Liste der Sonde, 27 getauschte Griffe).
1. Griff-Tausch (PORT-WAHL der Runden 31/33, door_scene_pc.c griff_zeichnen): Drehung =
   Grund-Drehung des Spenders (rot_vorn/rot_hinten) + Ausschlag — die Spiegel-Drehung rot0[2] des
   Archiv-Objekts wurde verworfen -> beide Fluegel trugen denselben Griff.
2. P0CD (ohne Tausch): DOOR0C-Stangen liegen nicht um den Anhaengepunkt (Huellen-Mitte 344 daneben);
   die z-Spiegeldrehung klappt die Stange des zweiten Fluegels auf die andere Seite (oben statt unten).

**Umsetzung (keine DO2-Aenderung — Tonteil/SCD/MD1 bleiben bytegleich dem Basis-Archiv, wie
probe_r33_tueren `archive` pinnt; geaendert wird nur, WIE der Laeufer die Griffe zeichnet):**
- `engine/src/tuer_spiegel_r35.c` + `include/re15_tuer_spiegel.h`: `re15_tuer_griff_tausch_rot`
  (rot[2] = Grund-Drehung[2] + rot0[2] des Archiv-Objekts) und `re15_tuer_spiegel_dz` (Tabelle:
  P0CD, Fluegel 1, dz 2048 = Spiegeldrehung aufgehoben; PORT-WAHL nach NUTZER-VORGABE "Das ist so
  im allgemeinen nicht", nur fuer Port-Archive, das RE2-Archiv DOOR0C selbst bleibt unveraendert).
- `platform/pc/src/door_scene_pc.c`: griff_zeichnen nutzt re15_tuer_griff_tausch_rot (1 Zeile),
  objekt_zeichnen zeichnet Griffe am Fluegel 1 mit +dz, wenn das Archiv in der Tabelle steht.
  Gefunden wurde dz 2048 mit den Versuchsdrehungen der Sonde (dz:2048 -> 0,0/0,1 Grad;
  dy:2048 -> 146,1/2,8; dy+dz -> 33,9/177,2).

**Messung nachher.** Riegel `unit_r35_inhalt_tueren` (= Sonde mit "test") 6/6: A alter Stand 7 (Spitze)
bzw. 8 (Mitte) unsymmetrische Port-Paare; B neuer Stand alle 8 Port-Paare < 10 Grad (max 0,2 Spitze;
Mitte max 18,4 nur P04B unveraendert = RE2-Geometrie); C RE2-Originale Wert fuer Wert unveraendert.
Echte exe (`RE15_TUER_SEITE=S044,S049,S155,S030 RE15_TUER_BOGEN`, beschleunigter Renderer,
alter Stand = door_scene_pc.c aus 154a73c1 kurz eingebaut): `F_belege/p1_tuergriffe_vorher_nachher.png`
— S044 P0CD Stangen vorher auf verschiedener Hoehe, nachher gleich; S049 P1DG / S030 P1DK rechter
Druecker vorher verdreht, nachher spiegelgleich; S155 P1BD Riegelstangen nachher spiegelgleich.
Neue DO2-Dateien: KEINE (bewusst, s.o.).

## Tests (Riegel dieser Spur, alle aus `re15_port/tests/unit/probes/r35_inhalt.cmake`)
| Test | Punkt | misst | Ergebnis |
|---|---|---|---|
| unit_r35_inhalt_doku | 2 | gruene Pixel FILE28/29 ueber re15_re2doc_pixel, Soll = TEX.TIM Zeile 2 (@0x80028974-94) | 19/19 |
| unit_r35_inhalt_zombies | 3 | echte Spielschleife je Eintritt: STEHEN-Griffbild, FLUCHT raus/gegriffen, Flucht-Mindestabstand >= 0xBB8, Spawnlagen | 22/22 |
| unit_r35_inhalt_items | 4/5 | Prop/Zone nach scd_room_reenter, Bit -> weg, Projektion in die Marke, echter Aktionsdruck -> Modal -> Ja | 25/25 |
| unit_r35_inhalt_tueren | 1 | Griff-Symmetrie aller Port-Doppeltueren alt/neu, RE2-Originale unveraendert | 6/6 |
Mess-Werkzeuge (kein add_test): probe_r35_inhalt_karte (Speicherkarte an beliebiger Stelle,
`bit:<n>`, `y:<n>`), probe_r35_inhalt_tueren (ohne "test": Tabelle; `alt`, `mitte`, `dy:`/`dz:`),
test_r35_inhalt_zombies `mess` | `suche` | `tempo <i> [0/1]` | `karte` | `sca <rdt> <name>`.

## OFFEN (mit naechstem Messweg)
1. Punkt 6 — falls der Nutzer mit "Karten" die LAGEPLAENE meinte: RE2 fuehrt sie nicht als Item
   (Namentabelle 0x00..0x8B: 0 Maps), sondern als Nachricht+Ja/Nein-Ereignis (7 Fundstellen oben).
   Naechster Weg: SCD der 7 Raeume (info/re2leon/PL0/RDT/room20B0/scd/*.c usw.) nach dem Block mit
   Message_on des Lageplans durchsuchen und pruefen, ob dort ein Obj_model_set/Bit das Weltobjekt
   ausblendet; wenn ja, dessen md1-Slot wie in re2_item_worldmodels.py schneiden.
2. Punkt 6 — 8 Platzierungen mit md1 >= nOmodel (Liste in extracted_re2_items/_bericht.txt) sind
   nicht geschnitten: der Pool-Slot kommt dort nicht aus der Raumtabelle. Naechster Weg: im selben
   SCD-Block das Obj_model_set mit diesem obj suchen (Pool 0x800D0324 + md1*0x1F8).
3. Punkt 4 — die Karte ist in Cut 7 klein und dunkel (Keycard 161x270 in ~4100 Sichtweite = etwa
   6x4 Pixel, Lichtsatz Cut 7). Optische Nutzer-Abnahme; eine Aufhellung waere PORT-WAHL wie der
   Lichtsatz am Irons-Tisch (re15_irons_tisch.h) — nicht gemacht, weil kein Befund dazu vorliegt.
   Kamera-Weg: Cut 7 ist ueber keine RVD-Zone von 1010 erreichbar, die die Sonde `zonen` findet
   (17 Zonen, Rasterabfrage). ~~das Nutzerbild beweist aber, dass Cut 7 im Spiel erscheint~~ —
   FALSCH (Abnahme 0, M2): add_card.bmp ist ein Hintergrundbild ohne Spielerfigur. ERLEDIGT in
   Nachbesserung 1 (M2): die Karte steht jetzt und ist in Cut 4 sichtbar.
4. Punkt 3 — ROOM1011 (Elza): sub00 spawnt die Zombies dort nur hinter einer Bedingung; der
   Tabellen-Schluessel (Typ + Original-Lage) deckt die Saetze, gemessen ist nur ROOM1010/1220/1221.
5. Punkt 1 — P1B3 (DOOR1B V3) hat je Seite nur einen Griff (kein Paar) -> nicht messbar; dieselbe
   Regel (rot0[2]) gilt dort.
6. Paket/Android-Gate (Orchestrator): FILE28_p01_page.TIM / FILE29_p01_page.TIM aendern sich
   GROESSENGLEICH (Memory r34a: gleich grosse Aenderung erreicht Geraete nur mit Gate-Pin).

## Fuer den Nutzer
- Sprachdateien: KEINE — keine neue Nachricht (Memory Card / Shotgun Shells laufen ueber das normale
  Aufnahme-Modal mit Item-Name und Frage). Nachrichten-IDs/Ereignisse der Spur F bleiben frei.
- Geaenderte Assets fuer das Paket-/Android-Gate: `re15_port/shared_assets/RE2/FILES/FILE28_p01_page.TIM`
  (md5 ad9d0f6f...), `re15_port/shared_assets/RE2/FILES/FILE29_p01_page.TIM` (md5 f39ce419...).
  Neue Laufzeit-Assets: keine (Schrot-Modell eingebacken in `engine/src/gen/r35_schrot_prop.inc`).
- RE2-Welt-Items ansehen: `extracted_re2_items/uebersicht.html` im Browser oeffnen (alle 81 Modelle,
  Karten oben), oder `kontaktbogen.png` / `karten.png`; Liste `katalog.csv`.
- Im Spiel: ROOM1010 rechtes Regal (Kamera mit Schreibtisch/Monitoren) -> Memory Card x3;
  ROOM1090 Dach-Hinterhof, rechter Aussenluefter -> Shotgun Shells (7, nach Halbierung 3);
  Marvin's Notes / Armory Notice: Code 4312 / 5632 gruen; Doppeltueren P0CD/P1DG/P1DK/P1DL/P1BD
  mit spiegelgleichen Griffen; ROOM1010/1220: Zombies starten ausserhalb der Sprungweite, man kann
  sofort umdrehen und wieder hinaus.

## Suite
Lauf 1 (`local_build.sh all`): 480/482 — rot: `unit_r34n_e_dokumente` (Pin "Pool = Raum-Props +
Dokument hinten angehaengt": meine Karte stand hinter dem Dokument-Prop) und
`integration_r30_irons_tisch_bild` (Fenster-Haken, ROOM1150, einzeln nachgefahren: gruen 37 s).
Behebung: `re15_inhalt_r35_install` steht in scd_room_setup.c und main.c jetzt direkt VOR
`re15_dokumente_install` (statt am Blockende) — ⛔ Abweichung vom VERTRAG 1.4 "am Ende des Blocks",
begruendet durch den Riegel der Runde 34 (Dokument bleibt das zuletzt angehaengte Prop); eigene Zeile
mit Kommentar "Runde 35 Spur F", keine fremde Zeile veraendert. Danach `unit_r34n_e_dokumente` gruen.
Lauf 2 (nach der Umstellung, Stand dieses Commits): `=== LOCAL-BUILD-OK (all) — Tests 482/482`
(478 + 4 Riegel dieser Spur).

## Nachbesserung 1 (2026-10-04, nach Abnahme 0 — Maengel M1/M2/M3)

Grundlage: `analysis/befunde_runde35/F_abnahme_0.md` (bestanden = NEIN: P4 teilweise, P6 teilweise,
M3 gering). Stand bei Beginn: HEAD ce1354a8.

### M1 (P6) — RE2-Lageplan-Weltmodelle

**Ursache (gemessen).** Zwei Fehler im Extraktor, nicht nur die Auslegung von "Karten":
1. Die Blockdateien `info/re2leon/PL0/RDT/room*/scd/*.scd` sind an **268 von 2568 Bloecken kuerzer
   als der Block im RDT** (341 673 Byte fehlen; 42 Bloecke der RDTs ROOMG000..G040 fehlen ganz;
   Werkzeug scratchpad `scd_bounds.py`). Beispiel room20B0 sub00: Datei 14 Byte, RDT 0x030F4..0x0322A
   = 310 Byte — genau dort stehen Obj_model_set 8 (Lageplan) und sein Aot_set, ausserdem zwei
   Item_aot_set (Id 0x15 md1 5, Id 0x32 md1 7), die in der ersten Ablage FEHLEN.
2. Der Walker hatte fuer Switch (0x13) Laenge 2 statt 4 -> Desync in jedem Block mit Switch.

**RE-Belege (RE2 PSX.EXE, selbst disassembliert).**
- Bloecke aus dem RDT: @0x800535c4 `lw v0,-7388(v0)` (RDT-Zeiger 0x800ce324), @0x800535d4
  `lw v0,72(v0)` = **RDT+0x48 Tabelle SCD Main**, @0x800535dc `sw v0,-29508(at)` (0x800d8cbc);
  @0x800535f4 `lw v0,76(v0)` = **RDT+0x4C Tabelle SCD Sub**. Gosub @0x800541a8: @0x800541ec
  `lw v1,-29508(v1)`, @0x800541f8 `lhu v0,0(a2)` (a2 = Tabelle + 2n), @0x80054200 `addu v0,v1,v0` ->
  Block n = Tabelle + u16[n]. (Deckt sich mit information294.txt Z. 33/34.)
- Switch @0x80054020: `lhu t0,2(a3)`, `lbu a2,1(a3)`, @0x80054040 `addiu a3,a3,4` -> 4 Byte;
  Case `addiu a3,a3,6` @0x800540cc -> 6.
- Obj_model_set (0x2D) @0x80055260: @0x80055290 `lbu t1,1(s2)` Objekt-Index; Lage +14/+16/+18
  (`lh a2,14(s2)` @0x80055310, `lh a3,16(s2)` @0x80055314, `lh v0,18(s2)` @0x80055308);
  Modell: @0x8005541c `lw a0,8508(t2)` (RDT-Zeiger 0x800d213c), @0x80055424 `lw v1,48(a0)`
  (RDT+0x30 Modelltabelle), @0x80055428 `sll v0,t1,3`, @0x80055430 `lw a1,4(v0)` (MD1 des Slots)
  -> **Objekt-Index = Modell-Slot** (TIM/MD1-Paar t1).
- Ja/Nein-Antwort: `Ck(0x0B,0x1F,0)` (Bank 11 Bit 31) nach Message_on mit Frage — in allen 7
  Bloecken identisch (Messung unten). Message_on-Id = Datei msg/subNN.msg (MSG Sub, RDT+0x40).

**Messung: die 7 Lageplan-Aufnahmen aller 495 Leon-RDTs** (alle msg-Dateien nach "map" ohne
Steuercodes; room2060 "It looks like an operation map..." ist nur eine Beschreibung). Walk ueber
die RDT-Bloecke (scratchpad `mapwalk.py`), RDT-Byte-Offsets:
```
Raum      Plan                 Objekt  Platzierung (Obj_model_set)                Aufnahme-Block (Ja-Zweig)                         ausgeblendet durch
room20B0  police station map   obj 8   sub00 @0x03158 (geparkt 20000) + sub04    sub10 @0x037BA: Message_on 0 / Ck(11,31,0) /       Work_set(4,8) @0x037F2
                                       @0x03406 Ck(8,0x53,0) -> Pos_set @0x0340E  Set(8,0x53,1) @0x037D6 / Message_on 17 @0x03800  Pos_set(20000,20000,20000) @0x037F6
                                       (-11527,-50,-11526) in Nahaufnahme Cut 14; Aot_set 11 (Gosub sub10) sub00 @0x0317E, ausgeloest per Aot_on(11) sub04 @0x0346C
room2130  police B1 map        obj 0   sub03 @0x00FDC (-16900,-1200,-10400),     sub05 @0x01820: Message_on 0 / Ck(11,31,0) /       Work_set(4,0) @0x01836
                                       Tor Ck(4,0x7A,0) @0x00FC4; Aot_set 5       Set(4,0x7A,1) @0x0184A / Message_on 12 @0x01858    Pos_set(0,-21024,0) @0x0183A
                                       (Gosub sub05) @0x00FC8
room3060  sewage disposal map  obj 7   sub00 @0x01F92 (-13800,-7200,-25250),     sub22 @0x0283E: Message_on 1 / Ck(11,31,0) /       Work_set(4,7) @0x02856
                                       Tor Ck(8,0x89,0) @0x01F8E; Aot_set 3       Set(8,0x89,1) @0x0284E / Message_on 2 @0x0286C     Pos_set(-13800,-32000,-25250) @0x0285A
                                       (Gosub sub22) @0x01FB8
room4040  sewer map            obj 2   sub05 @0x00F7E (-22814,-2350,-11768),     sub06 @0x00FA8: Message_on 10 / Ck(11,31,0) /      Work_set(4,2) @0x00FC4
                                       Tor Ck(8,0xA2,0) @0x00F66; Aot_set 4       Set(8,0xA2,1) @0x00FB8 / Message_on 11 @0x00FDA    Pos_set(0,0,0) @0x00FC8
                                       (Gosub sub06) @0x00F6A
room5040  factory map          obj 1   sub00 @0x01D54 (-11599,-2000,-21188),     sub13 @0x02164: Message_on 3 / Ck(11,31,0) /       Work_set(4,1) @0x02174
                                       Tor Ck(0x22,0x19,0) @0x01D50; Aot_set 3    Set(0x22,0x19,1) @0x02188 / Message_on 4 @0x02196  Member_set(0x0C,-32000) @0x02178
                                       (Gosub sub13) @0x01D7A
room5060  factory map          obj 2   sub00 @0x00E20 (-10449,-21800,-20988),    sub04 @0x0101A: Message_on 1 / Ck(11,31,0) /       Work_set(4,2) @0x0102A
                                       Tor Ck(0x22,0x19,0) @0x00E1C; Aot_set 3    Set(0x22,0x19,1) @0x0103E / Message_on 2 @0x0104C  Member_set(0x0C,-32000) @0x0102E
                                       (Gosub sub04) @0x00E46
room6120  laboratory map       KEINS   — Aot_set_4p 6 (Gosub sub03) sub00 @0x012B0, Flaeche x -28790..-22600 z -10800..-5900;
                                       sub03 @0x014DA: Message_on 0 "A map of the lab is available here. Will you file ..." / Ck(11,31,0) /
                                       Set(4,0xA5,1) @0x01538 / Aot_reset 6 @0x0153C / Message_on 14 — KEIN Work_set(4,n), nichts wird ausgeblendet
```
Damit ist jede der sechs Weltmodell-Zuordnungen der Abnahme belegt (Platzierung + Ausblenden im
Ja-Zweig + Merkbit, das die Platzierung beim naechsten Betreten unterdrueckt), und der Kandidat
**room6120 model11 ist WIDERLEGT**: der Laborplan wird an einer Wandflaeche "abgelegt" (file), der
Block blendet nichts aus; obj 11 steht bei (-22622,-1459,-18360) (sub00 @0x0148A), 7560 ausserhalb
der Aufnahmeflaeche (z -10800..-5900). Meshes: room2130 obj 0 = room20B0 obj 8 (gleiches Mesh),
room5040 obj 1 = room5060 obj 2, room3060 obj 7 / room4040 obj 2 (md5 werden beim Schneiden
gemessen, siehe Umsetzung).

**Umsetzung M1** (Commits 280b4251, 5914d130):
- `tools/re2_sicherung/re2_scd_walk.py`: OVERRIDE Switch 0x13 = 4 (@0x80054040); neue Funktion
  `rdt_blocks(raw)` (RDT+0x48/+0x4C, Belegkette im Docstring).
- `tools/re2_sicherung/re2_item_worldmodels.py`: `scan_rdt()` liest die Bloecke aus dem RDT (letzter
  Block der Tabelle endet am letzten Evt_end — dahinter liegen andere RDT-Daten, z.B. ROOM1020 sub19
  ein MD1-Kopf; Gegenprobe: 87 dieser Enden = Laenge der scd-Datei, 3 Dateien sind noch kuerzer);
  `lageplaene()` findet die Aufnahmen nach dem Muster oben (Frage-Nachricht mit "map" und "take/file",
  Ja-Zweig Ck(0x0B,0x1F,0), Work_set(4,n), Pos_set/Member_set, Merkbits), sucht alle Obj_model_set(n)
  und schneidet Modell-Slot n; `md1_trim()` kuerzt das MD1 auf seine eigene Laenge (rdt_props schnitt
  room5060 Slot 2 mit 14188 statt 236 Byte -> falsche md5; Gegenprobe gegen die obj/modelNN.md1:
  596 von 597 gleich lang); Weltmodell nur bei md1 < 0x20 (@0x80054D98) — room2130 sub00 md1 0xE1
  stand vorher als "Fehler" im Bericht, ist aber "kein Weltmodell".
- Ablage `extracted_re2_items/`: `modelle/karte00_735097e1.*` (zusammengerollter Plan, police
  station map + police B1 map), `karte01_99dae133.*` (Planbogen, sewage disposal map + sewer map),
  `karte02_5cdae635.*` (Planbogen, factory map x2) je .md1/.tim/.obj/_a.png/_b.png; `karten.png`
  = 3 Card Keys + 3 Lageplaene; `uebersicht.html` Abschnitt "Karten (Card Keys + Lageplaene)";
  `katalog.csv` 3 neue Zeilen (karte = ja, Platzierung + Aufnahme-Block mit RDT-Offset);
  neu `lageplaene.csv` (alle 7 Aufnahmen inkl. room6120 "KEIN Weltobjekt", Merkbits, Weg-Record).

**Messung nachher M1** (`_bericht.txt`): 7 Lageplan-Aufnahmen, 6 mit Weltobjekt, 3 Meshes —
die md5 sind genau die der Abnahme (735097e1 / 99dae133 / 5cdae635). Nebenbefund durch die
RDT-Bloecke: **312 Item_aot_set-Platzierungen statt 269** (242 mit Weltmodell statt 206), 87 Item-
Meshes, SCD 2568 Bloecke ohne Desync (vorher 45 desynchron bei 94,55 % der Datei-Bytes). Verbleibend
3 Platzierungen mit md1 >= nOmodel (room2080 sub00 Id 13 md1 4 bei nOmodel 4, roomG040 sub00 2x) —
dort kommt der Pool-Slot nicht aus der Raumtabelle (OFFEN, wie zuvor; vorher 8 Faelle).
`karten.png` angesehen: Rolle (28 Dreiecke), zwei flache Planboegen mit Grundriss-Textur.

### M2 (P4) — Memory Card in der erreichbaren Kamera (Cut 4) sichtbar
Korrektur einer falschen Dossier-Aussage: OFFEN 3 oben sagt "das Nutzerbild beweist aber, dass Cut 7
im Spiel erscheint". Das ist NICHT gemessen und falsch begruendet: add_card.bmp (wie Shotgun.bmp)
enthaelt keine Spielerfigur, ist also ein Hintergrundbild (Cut-7-BSS), kein Spielbild.

**Ursache / Messung vorher.** Cut 7 ist im Spiel nicht erreichbar — selbst nachgeprueft:
`probe_r34n_e_dokumente zonenliste 1010`: 17 RVD-Saetze; Zone 14 "Cut 7 -> 0" ist der
Bereichskopf (Quad = ganze Suedhaelfte x -4200..6800 z -7600..1400, wie Zone 8 fuer Cut 4 und
Zone 10 fuer Cut 5); Uebergaenge in der Suedhaelfte nur 9 (4->5) und 11 (5->4); KEIN Satz mit Ziel
Cut 7. Tueren nach 1010 nur Cut 0/4 (Abnahme), kein Cut_chg im SCD. Am Regal ist Cut 4 aktiv.
Dort lag die Karte FLACH auf Brett B; Cut 4 sieht das Brett nur 14,3 Grad von oben (Sichtseite
gegen die Richtung zur Kamera: 0,247) -> 10x2-Pixel-Strich. Modell (scratchpad
`f_n1/karte_opt.py`, Kamera = VIEW-Matrix + H 208 wie `projiziere()`, Drehung = `pc_prop_rot_q12`,
Welt = m * v wie `re15_camera_compose_view_bone` out_rot = view_rot * bone_rot) gegen die
Abnahme-Messung: flach rot 0 -> Cut 4 bbox 95,0..105,2 x 105,7..107,8 (gemessen 95..105 x 105..107),
11,2 px^2; Cut 7 54,1 px^2 (gemessen 56 Diff-Pixel) -> Modell stimmt. Die Karte (Quad 161 x 270)
kann in Cut 4 hoechstens ~48 px^2 gross werden (Abstand 6400, H 208: 5 x 8,5 px).

**Beleg (FORM).** Prop = Obj_model_set-Felder; die drei Euler-Winkel liegen im Pool bei
+0x68/+0x6A/+0x6C (scd_vm.c Member 3..5, `case 3/5`), der Port zeichnet sie ueber
`pc_prop_rot_q12` (main.c:670-694). Werte = PORT-WAHL (Nutzer-Marke + Sichtbarkeit).

**Aenderung.** Suche ueber alle Drehungen (Raster 64): Kartenmitte bleibt GENAU auf dem
Markenstrahl von Cut 7, tiefste Kante auf Brett B (y -1725), alle Ecken im Regal (x 3630..4570,
z -2020..1420, oberhalb Brett A -2370), Stand auf einer KANTE (zwei Ecken auf dem Brett), Cut-7-
Flaeche >= alter; Ziel = groesste Cut-4-Flaeche. Ergebnis rot (896, 128, 3584): die Karte steht auf
ihrer kurzen Kante, 15,9 Grad nach hinten geneigt, Sichtseite zur Raumecke zwischen Cut 4 und Cut 7
(Normale (-0,694; -0,273; -0,667)); Ursprung (3748,-1725,-1383), Mitte (3719,-1855,-1299), Ecken
(3748,-1725,-1383) (3801,-1985,-1331) (3689,-1984,-1215) (3636,-1725,-1267). Aufhebe-Rechteck mittig
auf die neue Mitte: x 3219..4219 z -1799..-799 (PORT-WAHL wie vorher 1000 x 1000). Masken ueber den
Ecken in Cut 4/7: 0 (`probe_r34n_e_dokumente sicht`). Dateien: `include/re15_inhalt_r35.h`
(RE15_R35I_KARTE_X/Y/Z, _ROT_X/_ROT_Y/_ROT_Z, _RECT_X/_RECT_Z), `engine/src/inhalt_r35.c`
(Felder rot_x/rot_z). Commit 39930a8c.

**Messung nachher (echte exe, Kopie re15_pc_r35f_n1.exe, RE15_FRAMEDUMP, beschleunigter Renderer).**
- Framedump F120 mit/ohne Bit (9,80) (`RE15_FORCE_CUT=4` bzw. `7`, Karte (3650,6900) rot 2048):
  **Cut 4: 53 Diff-Pixel, bbox 101..107 x 99..107** (vorher 11 Pixel, 95..105 x 105..107; Modell
  102,0..108,4 x 99,5..108,7). **Cut 7: 114 Diff-Pixel, bbox 268..279 x 156..172**, Mitte auf der
  Marke (Huelle 269..278 x 158..170; vorher 56 Pixel). Bild: `F_belege/n1_m2_karte_cut4_cut7.png`.
- Echter Tuerweg (Karte ROOM1020 (-4000,-7250) rot 0, START 6, Skript der Abnahme
  `A0.2,W0.6,U0.25,R0.35,U1.3,R0.35,W0.3,A0.2,W4,A0.2,W1.5,A0.2,W2,S0.2,W3`): `cam=4` auf dem
  ganzen Weg; F100 zeigt die stehende Karte im Regal neben Leon (Ausschnitt im Belegbild); Leon steht
  bei (3147,-1064) rot 64 -> F140..F200 "Will you take the **Memory Card**?" -> Ja -> F360ff Inventar
  mit der Memory Card x3 (scratchpad `f_n1/m2_tuer_sheet.png`).
- Riegel `unit_r35_inhalt_items` 27/27 (neu: P mit rot (896,128,3584); V Cut 4 47,6 px^2 >= 40,
  Sichtseite zugewandt 0,99; V Cut 7 126,0 px^2 >= 100, zugewandt 0,87; M Mitte (274,01;164,51) in
  der Marke; D echter Aktionsdruck von (3300,-1033) yaw 0 -> Modal -> Ja -> Menge 3).

### M3 (P1) — Riegel-Luecke Seitenzuordnung + V3-Restbefund 1 px

**Ursache (gemessen, Sonde mit Ausgabe der Fluegel-Achsen).** "welt" der Tuer-Maschine ist bereits
der KAMERARAUM der Tuerszene (Kamera im Ursprung, z = Tiefe, Blaetter bei z 8000, Angeln bei
x +-3588). Fluegel 1 ist gegen Fluegel 0 um 2048 um die Senkrechte gedreht (S022: Fluegel 0
x-Achse (0,0,-4096), Fluegel 1 (0,0,+4096); bei V3 umgekehrt). Lokales pos[0] = +130 heisst also
an Fluegel 0 "vorn", an Fluegel 1 "hinten". Die bisherige Paarung nach dem Vorzeichen von pos[0]
verglich deshalb an vielen Seiten Vorderseiten- mit Rueckseiten-Griffen — und Punkt 1 hatte an
genau diesen falschen Paaren seine Korrekturen gesucht:
- P1B3/P1BD (DOOR1B <- Spender DOOR23): die Spiegel-Drehung rot0[2] beim Tausch machte die ECHTEN
  Kameraseiten-Paare schief (S136/S157 37,1 Grad, 3,27 px Breitenunterschied; die Riegelstange des
  zweiten Fluegels stand ins Blatt). Ohne sie: 0,1 Grad / 0,14 px.
- P0CD (DOOR0C): dz 2048 allein ergab auf den echten Paaren 33,9 Grad / 3,20 px (S041).
- DOOR1D V3 (S030/S049/S092): obj 3 sitzt im Archiv an der RUECKSEITE von Fluegel 1 (Anhaengepunkt
  z 8130) und ragt mit der Spiegel-Drehung durch das Blatt nach vorn (Huelle z 7918..8130) — im
  RE2-Original S192 genauso. Partner obj 2 sitzt vorn (z 7658..7870). 260 Einheiten mehr Tiefe ->
  in der Projektion 1,1..1,3 px tiefer/kleiner. Rechnung mit H 290 (@0x80013e34 `addiu a0,zero,290`)
  und OFX/OFY 160/120 (@0x80068e80/@0x80068e88): obj 2 Huellen-Mitte y 990 bei z 7764 -> sy 156,98;
  obj 3 y 990 bei z 8024 -> sy 155,78 = **genau der Abnahme-Befund L 157,0 / R 156,0**. Die
  "Punktspiegelung des Spender-Meshes" war es nicht: DOOR07 Mesh 1 ist in y symmetrisch
  (y -52..52, Mittel 0,0; x 0..212, z -54..455, gemessen in DOOR07.DO2 Sektor 10).

**Aenderung.**
- Sonde `probe_r35_inhalt_tueren`: Seite = KAMERASEITE: Griffe, deren Welt-Huelle vor die Blattmitte
  ragt (Seite 0) bzw. dahinter (Seite 1), je Fluegel der am weitesten ragende Griff; alter Stand mit
  Argument `lokal`. Neues Bildmass: Bild-Huellen beider Griffe (H 290, 160/120), Unterschied von
  Ober-/Unterkante, Hoehe, Breite in Pixeln. Verworfen und im Code vermerkt: absolute Huellen-Mitte
  gegen die Tuermitte, Huelle gegen Anhaengepunkt, Blatt-x-Achse (paarten V3 falsch bzw. verloren
  S041/S136/S157). Zusatzausgabe Fluegel-Achsen, Blatt-Tiefe, Anhaengepunkte.
- `engine/src/tuer_spiegel_r35.c` / `include/re15_tuer_spiegel.h`:
  `re15_tuer_griff_tausch_rot(..., spender, ...)` uebernimmt rot0[2] nur fuer Spender DOOR07;
  P0CD Fluegel 1: dy 2048 + dz 2048 (`re15_tuer_spiegel_dy` neu) — dy hebt die Drehung des Fluegels
  um die Senkrechte fuer den Griff auf, dz die Spiegeldrehung des Archivs; neu
  `re15_tuer_griff_tausch_dx`: DOOR07-Tausch, Fluegel 1, rot0[2] 2048, pos[0] < 0 -> Anhaengepunkt
  auf die Vorderseite (pos[0] gespiegelt). Alles ⛔ PORT-WAHL zur NUTZER-VORGABE, nur Port-Archive;
  RE2-Archive und DO2-Dateien unveraendert.
- `platform/pc/src/door_scene_pc.c`: 5 Zeilen (Spender an tausch_rot, +dx, s_spiegel_dy).
- Riegel `unit_r35_inhalt_tueren` (Sonde `test`): A alter Stand Befund (Spitze, P0CD 146,1 Grad);
  B neuer Stand JEDES Port-Paar beider Seiten < 10 Grad UND Bild-Fehler <= 1,0 px; C RE2-Originale
  Wert fuer Wert unveraendert; **D S041 gepinnt** (alt 146,1 -> neu 0,0 Grad, 0,30 px); E alle 12
  Port-Doppeltuer-Seiten mit Griffpaar haben ein Kameraseiten-Paar (S218 = DOOR2D-Rolltor ohne Paar).

**Messung vorher -> nachher (Sonde, Bild 2, Kameraseite).**
```
Seite  Archiv        vorher (Abnahme-0-Stand)              nachher
S022   P1DK V2       0,1 Grad 0,17 px                      0,1 Grad 0,17 px
S030   P1DK V3       0,1 Grad 1,28 px                      0,1 Grad 0,00 px
S041   P0CD V1      33,9 Grad 3,20 px                      0,0 Grad 0,30 px (Rest: Archiv-Hoehen -3000/-3008)
S044   P0CD V0       0,0 Grad 2,75 px (Paar 2/5)           0,0 Grad 0,91 px / abgewandt 0,83 px (-2976/-3000)
S045   P1DG V2       0,1 Grad 0,17 px                      0,1 Grad 0,17 px
S049   P1DG V3       0,1 Grad 1,28 px                      0,1 Grad 0,00 px
S059   P04B V2       0,1 Grad 0,00 px                      0,1 Grad 0,00 px
S080   P1DL V2       0,1 Grad 0,17 px                      0,1 Grad 0,17 px
S092   P1DL V3       0,1 Grad 1,28 px                      0,1 Grad 0,00 px
S136   P1B3 V3      37,1 Grad 3,27 px                      0,1 Grad 0,14 px
S155   P1BD V2       0,1 Grad 1,30 px                      0,1 Grad 0,14 px / abgewandt 0,12 px
S157   P1BD V3      37,1 Grad 3,27 px                      0,1 Grad 0,14 px
RE2    DOOR1D V3 S192 0,0 Grad 1,75 px (Original, unveraendert), DOOR1B V2/V3 18,0 Grad 0,68 px (Original)
```
**Echte exe** (`RE15_TUER_SEITE=S022,...,S157,S218,S192,S046 RE15_TUER_BOGEN RE15_TUER_SCHNELL=1`,
Kopie re15_pc_r35f_n1.exe, 15 Sequenzen gespielt): Balken-Schwerpunkt mit dem Mass der Abnahme
(helle Pixel, Spalten 132-142 / 178-186, Zeilen 148-166): S030 L 157,00 R **157,25** (vorher 156,00),
S049 157,00 / **157,00**, S092 157,20 / **157,22**; V2 unveraendert 155,00 / 155,00. Bild
`F_belege/n1_m3_tueren_vorher_nachher.png` (je Seite links Abnahme-0-Stand, rechts jetzt): S041/S044
Stangen jetzt gleich breit, S136/S157 Riegelstangen jetzt gleich lang, S030/S049/S092 Druecker auf
gleicher Hoehe. Damit ist M3b BEHOBEN (nicht nur gefuehrt).
