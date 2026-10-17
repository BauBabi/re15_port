# Runde 38 — Spur K "itemorte": unabhaengige Abnahme 1

Pruefer: Abnahme-Agent 1 (2026-10-17). Baum `.claude/worktrees/r38_itemorte`, Zweig `r38/itemorte`, HEAD 099879be,
Merge-Basis 1bcbd0ff. Gegenstand: Nachbesserung 1 (Mangel M1 der Abnahme 0, Prison Key nicht gut sichtbar) und
Gegenprobe beider Nutzer-Punkte am gebauten Stand. Bericht wird FORTLAUFEND geschrieben (Sitzungslimit).

**Ergebnis: BESTANDEN.** Punkt 1 (Feuerloescher ROOM1030) und Punkt 2 (Parking Lot Key / Prison Key an der Pinnwand-Marke,
gut sichtbar) sind an der exe gemessen erfuellt; Mangel M1 der Abnahme 0 ist behoben (Prison Key = RE2 Small Key, im
Spielfenster 960x720 ein heller Bartschluessel in der Marke). RE-Gate und Vertrag halten; gezielter ctest-Lauf 130/130 gruen.

## 0. Werkzeug und Bau

* Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` ->
  `ninja: no work to do.`, `LOCAL-BUILD-OK (build)`. exe `re15_port/build/platform/pc/re15_pc.exe` 05:09 (Baum 0a2f02bc);
  `git diff 0a2f02bc HEAD --stat` = nur Dossier + 3 Belegbilder; `find engine include platform/pc tests -newer exe` leer;
  `git status --short` leer.
* Gelesen: AUFTRAG.md (Wortlaut; die master-Nachtraege 1/2 betreffen W/F, nicht K), VERTRAG.md, Dossier K_itemorte.md
  (ganz, besonders 9.1-9.9), K_abnahme_0.md, `git log master..HEAD`, `git diff master...HEAD` (Code ganz gelesen;
  `git diff master` ohne Punkte zeigt zusaetzlich die seit 1bcbd0ff auf master gewachsenen Orchestrator-Dateien als
  Loeschung — nicht die Spur).
* Gebaute exe = HEAD-Stand nachgeprueft: die Bytes von `k_r38k_prison_md1` (236 B, gen/itemorte_r38_md1.inc) und von
  `re15_r36q_prison_tim/_icon/_itps` (gen/schluessel_r36.inc) stehen in der exe (Python-Suche, Offsets 4819648 /
  4953184 / 4989248 / 5002752).
* Messkopie `re15_port/build/platform/pc/re15_pc_r38k_abn1.exe` (eigener Name neben der exe, Asset-Wurzel =
  exe-Verzeichnis; nie `taskkill /IM`; nach der Abnahme geloescht). Eigener Lauf-Treiber `lauf.sh` (Kartenwerkzeug
  `probe_r37_feuerloescher_karte` -> CONTINUE mit `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
  RE15_NO_INTRO=1 RE15_NOAUDIO=1`, Eingabe `RE15_INPUT_SCRIPT` Basis Spielbild ab 60, Ende `RE15_EXIT_AT`, Bilder
  `RE15_FRAMEDUMP`) und EIGENES Differenzwerkzeug `diff.py` (Python/PIL: |dR|+|dG|+|dB| > 30, Sum|dL| Rec. 601, Huelle,
  Pixel ausserhalb) im Scratchpad der Abnahme (`k1/`), NICHT die C-Auswerter des Riegels.
* Fenstergroesse des Nutzers auf DIESER Maschine gemessen: Lauf ohne `RE15_WINDOW_SCALE` -> debug.log
  `[window] windowed 960x720`, `[framedump] 960x720`; das Bild ist **bitgleich** mit dem Lauf `RE15_WINDOW_SCALE=3`
  (`cmp` ohne Ausgabe). => Skala 3 ist das Bild, das der Nutzer sieht; Skala 1 ist die Messgroesse der Riegel.

## 1. RE-Gate (Stichprobe der NEUEN Belege der Nachbesserung 1, selbst gelesen/disassembliert)

| Zitat im Diff | selbst geprueft (Werkzeug, Binaerdatei) | Befund |
|---|---|---|
| RE2 `ROOM10F0.RDT @0x006F0` Item_aot_set Item 0x1F md1 4, `@0x00706 2d 04 .. 0a 00 10 00` | `xxd -s 0x6E0` info/re2leon/PL0/RDT/ROOM10F0.RDT (md5 fc6964b8... = Wert im Werkzeug-Kopf) | `4e05 0231 0000 05b0 c59c 3606 1207 1f00 0100 2600 04a0` = Opcode 0x4E, Slot 5, sat 0x31, Item **0x001F**, Menge 1, Bit 0x26, **md1 4**; direkt dahinter `2d04 0000 0000 0000 0000 0a00 1000` = Obj_model_set Objekt 4, pc[12..13] = **0x0010** — wie zitiert |
| MD1 @0x001650 / TIM @0x0215BC = Modell 4 | eigene Python-Zerlegung des RDT-Kopfs: Offset-Tabelle Eintrag 10 (Modellliste) = 0xE4, Eintrag 4 bei 0x104 = (TIM **0x215BC**, MD1 **0x1650**); md5 der Bytes 236 B = c5d49ef6..., 34848 B = 326b822b... | wie zitiert — das Weltmodell IST das Objekt 4 des Small-Key-Satzes |
| RE2 Obj_model_set 0x80055260 @0x8005530c / @0x80055320 | `re2_disasm.py table 0x800a757c` -> `[0] 0x80055260` (= Tabelle 0x800a74c8 + 0x2D*4); `dis 0x80055300` | `8005530c lh a1,12(s2)`, `80055320 sw a1,16(s1)` — Attribut-Wort -> Pool+0x10 |
| RE2 Zeichner @0x800365e8 | `re2_disasm.py dis 0x800365d8` / `dis 0x80036650` | `800365e0 lw v0,16(s1)`, `800365e8 andi v0,v0,0x10`, `800365ec beq v0,zero,0x80036650`; gesetzt -> `80036610 jal 0x8002d0e8`, sonst `8003666c jal 0x80027bec`. Sprungziele selbst nach GTE-Befehlen durchsucht (Python, cop2-Woerter): 0x80027bec enthaelt **NCCT** (@0x80027d18/@0x80027f4c) und **NCCS** (@0x80027eec) = Licht; 0x8002d0e8 nur RTPT/RTPS/NCLIP/AVSZ/MVMVA, **kein NCC*** = unbeleuchtet — Bit 0x10 = flach belegt |
| RE1.5-Gegenstueck @0x8002c2f4 -> @0x8002c320 | `re15_disasm.py dis 0x8002c2e8` info/Re1.5/PSX.EXE | `8002c2f4 andi v0,v0,0x10`, `8002c2f8 beq v0,zero,0x8002c360`, `8002c320 jal 0x80022f0c` — wortgleich; Port: main.c:10913/10986 `(attr & 0x12) == 0x10` (vorbestehend, Runde 36 N) |
| RE2-Text @0x800A11EF (Satzform des Untersuchen-Texts) | Python: PSX.EXE t_addr 0x80010000, Datei 0x919EF, Zeichensatz (A=0x1D, a=0x3D, 0x00 Leer, 0x01 '.', 0xFC Umbruch, 0xFE Ende) | `00 25 00 3f 3d 4a ...` = " I can open a\|simple lock \|with this." (davor `... 01 fe` = Ende des Vorgaengers); Nachbarn @0x800A11C9 "I can type in my\|progress with this." (Ink Ribbon 0x1E) und @0x800A1215 "They look like\|parts for a gun." (0x20) — Eintrag 0x1F = Small Key belegt |
| Icon ITEMALL.PIX Tile 31 @0x9150, Item-Bild ITPS Block 31 @0x5D000 | 31 x 1200 = 37200 = 0x9150; 31 x 0x3000 = 0x5D000; Tile-Bytes stehen in `ITEMALL/ITEMALL_032.TIM` @544; Index 0 kommt 0-mal vor; Bilder `ITEMALL_032_0412.bmp` / `ITPS3100_0549.bmp` angesehen | Icon und Item-Bild zeigen denselben silbernen Bartschluessel mit Ringreide wie das Weltmodell |

* Erzeugte Dateien: `schluessel_export.py` + `itemorte_export.py --schreiben` mit umgelenktem Ziel (eigenes
  `k1/regen.py`, Module per importlib geladen, nur ZIEL/ZIEL_MD1 umgebogen) -> `gen/schluessel_r36.inc`,
  `gen/itemorte_r38_lage.inc`, `gen/itemorte_r38_md1.inc` **bitgleich** (`cmp` ohne Ausgabe). Werkzeug-Ausgabe Prison Key:
  `2596 sichtbare Texel | s 5.3032 -> sichtbar 736 x 345 | ... gezeichnete Huelle x 132.53..144.49 y 78.35..84.28,
  10 Verkleinerungsschritte` (Marke x 133..143 -> 132,5..144,5 = Marke mit +-0,5 px) — kein Handeingriff.

## 2. Punkt 2 — Parking Lot Key / Prison Key "gut sichtbar" an der Stelle aus key_location.bmp (Mangel M1 der Abnahme 0)

Marke (Abnahme 0, Nutzerbild selbst ausgewertet): x 133..143 y 77..85 auf der Pinnwand, ROOM10F0 Cut 5. Messfenster =
Marke +-2 px (x 131..145 y 75..87; Skala 3: x 393..437 y 225..263). Alle Laeufe: Karte `10F1 0 4000 1024 cut:5 elza`
[+ `flag:9:83`] bzw. `10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94` [+ `flag:9:82`], Skript `W3`, Ende 120, Bild 100.

| Lauf (lauf.sh-Name) | Log (debug.log) | diff.py (Schwelle 30) |
|---|---|---|
| p2e_mit/ohne, Skala 1 (Elza) | `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `[itemorte] ROOM10F1: Slot 8 (msg 4) -> Slot 15`, `[schluessel] ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83)) Prop obj 4 bei (1358,-2495,-3400) rot (1024,0,0), Zone Slot 8`, `[prop-render] pi=4 oid=0x04 ... rot=(1024,0,0)` | **27 Pixel, Sum\|dL\| 1627**, \|dL\|/px 60,3, Huelle x 132..143 y 79..83, **aussen 0** (Schwelle 10: 30 / 1636) |
| p2l_mit/ohne, Skala 1 (Leon) | `[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45, Bit (9,82)) ... rot (3072,0,2048), Zone Slot 8` | 55 Pixel, Sum\|dL\| 3428, \|dL\|/px 62,3, Huelle x 132..143 y 78..84, aussen 0 |
| p2e3 / p2l3, Skala 3 | `[framedump] 960x720` | Elza **217 Pixel, Sum\|dL\| 11899**, Huelle x 397..432 y 236..250, aussen 0; Leon 491 / 28852, x 397..432 y 233..255, aussen 0 |
| p2eD, OHNE RE15_WINDOW_SCALE (= Fenster des Nutzers 960x720) | `[window] windowed 960x720` | bitgleich mit Skala 3 -> 217 / 11899 |
| p2e4 / p2l4, Skala 4 (1280x960, Standard auf groesseren Schirmen), Fenster x 524..583 y 300..351 | — | Elza **345 Pixel / 18015**, Huelle x 530..576 y 315..334; Leon 865 / 48354, x 529..576 y 311..340; aussen 0 |
| c6e / c6l, RE15_FORCE_CUT=6, Skala 3 (zweite Kamera auf die Pinnwand) | `[prop-render] ... oid=0x04` | Elza 480 Pixel / 24727, Huelle x 244..296 y 206..228 (auf der Pinnwand); Leon 979 / 52877; aussen 0 |
| c7e / c7l, RE15_FORCE_CUT=7, Skala 3 (Buecherregal vor der Pinnwand) | — | 0 Pixel im ganzen Bild (Regel `re15_itemorte_r38_verdeckt`, unveraendert) |

=> Die Zahlen des Bau-Agenten (9.6: 27/1627, 55/3428, 217/11899, 491/28852) sind mit meinem eigenen Werkzeug **genau**
reproduziert. Prison/Parking: Skala 1 49 % Pixel / 47 % Sum|dL|, Skala 3 44 % / 41 %, Cut 6 49 % / 47 %; |dL| je Pixel
97 % / 93 %. Gegen den Cabin Key der Abnahme 0 (14 / ~500 bzw. 112 / 3193): x 1,9 Pixel (beide Skalen), Sum|dL| x 3,2 (Skala 1) / x 3,7 (Skala 3).

**Bild (selbst angesehen):**
* Skala 3 1:1 (`k1/s3_1zu1.png`, so sieht es der Nutzer): links Leons Parking Lot Key als heller Klingen-Fleck mit
  dunklem Kopf, rechts Elzas Prison Key als **weisser Bartschluessel mit Ring links, Schaft und zwei Barten rechts**,
  ganz in der Marke, unten rechts auf der Pinnwand — auf den ersten Blick als Schluessel zu erkennen (Augenschein: die
  Silhouette ist die klassische Schluesselform; die Zahlen oben sind das Mass, das Bild die Gegenprobe).
* Skala 3 x3 vergroessert (`k1/s3_zoom3.png`, Nutzerbild / Leon / Elza / Elza genommen): Ring, Schaft und Bart des Small
  Key silbern auf dem Kork, in der roten Marke des Nutzers.
* Skala 1 x8 (`k1/s1_zoom8.png`): bei 320x240 ein heller Ring (x 132..134) und ein dunkler 1-Pixel-Schaft nach rechts —
  im 320x240-Bild schwaecher (L-Mittel der geaenderten Pixel 57,3 gegen 73,3 Untergrund: der Schaft trifft seine dunkle
  Unterkante), so wie es der Bau-Agent als Grenze benennt. Das 320x240-Bild ist die Riegel-Messgroesse, nicht das
  Spielfenster (gemessen: Fenster 960x720 auf dieser Maschine; render_pc.c:620-630 waehlt die groesste ganze Skala <= 90 %
  der Arbeitsflaeche, Standard 4).
* Cut 6 Skala 3 (`k1/cut6_s3.png`, Leon / Elza / Elza genommen): der silberne Bartschluessel klar auf der Pinnwand.

**Gehweg (ECHTE Bewegung, Kamera durchgehend Cut 5)** — Skript `W1,U3.2,W0.3,L0.36,W0.3,U0.45,W0.3,R0.36,W0.3,U1.6,W3`,
Skala 3, Serie `RE15_FRAMEDUMP=60-400/20`, je mit/ohne Bit; State-Log: (0,4000) -> (0,1675) F120 -> (0,-432) F180 ->
(1036,-390) F240 -> (1036,-2932) ab F300 vor der Pinnwand:

| Bild | 60..220 | 260 | 300..400 (Figur steht vor der Pinnwand) |
|---|---|---|---|
| Prison Key (Elza) Pixel / Sum\|dL\| | 217 / 11899 | 174 / 9302 | 171 / 9022 |
| Parking Lot Key (Leon) | 491 / 28852 | 316 / 18474 | 232 / 15413 |

Bild `k1/geh_s3.png` (F100/F180/F260/F340, oben Elza, unten Leon, halbe Groesse): der weisse Schluessel ist in jedem
Bild an der Pinnwand zu sehen, bis die Figur davor steht und den rechten Teil verdeckt (Huelle dann x 397..426).

**Echter Tuer-Weg Elza** (Karte `10D1 1900 -7000 2048 cut:0 elza flag:3:50 flag:4:247`, Skript `W1,A0.5,W12`):
`[tuer] Sequenz Archiv 2 DOOR13 ...`, `[room] PC loaded room10f1.rdt`, `[itemorte] ROOM10F1: Slot 8 (msg 4) -> Slot 15`,
`[schluessel] ROOM10F1: PRISON KEY ... rot (1024,0,0), Zone Slot 8`; Ankunft (8400,-350) rot 2048 Cut 0 (dort liegt die
Pinnwand nicht im Bild, Anker-Viereck). Derselbe Tuer-Weg mit `RE15_FORCE_CUT=5`, Bild 250, mit/ohne (9,83):
**217 Pixel / 11899, Huelle x 397..432 y 236..250, aussen 0** = bitgleich mit dem CONTINUE-Weg. (Ein Gehversuch von der
Tuer quer durch den Raum blieb bei x 5368 am Mobiliar haengen — `stand 10F1`: Ost- und Westteil sind nur ueber z 8700..11100
verbunden; fuer das Urteil nicht noetig, die Pinnwand-Kameras sind oben gemessen.)

**Aufnahme Elza** (Karte `10F1 1300 -2900 1024 cut:5 elza`, Skript `W1,A0.1,W3.5,A0.1,W4,S0.1,W6`, Skala 3, Serie 100..520/20):
`[schluessel] PRISON KEY genommen: Item 0x42, Bit (9,83)`, `[schluessel] Prison Key genommen -> Flag (4,0xEC) = 1 (Zelle
ROOM1211)`. Bild F140/F180: Modal "Will you take the **Prison Key**?" mit dem RE2-Bild des silbernen Bartschluessels
(ITPS 31); F260 Pinnwand ohne Schluessel; F400..F520 Statusschirm, Item-Liste: das Icon ist derselbe Bartschluessel
(Tile 31) — Modell, Item-Bild und Icon passen zusammen (`k1/auf_e.png`). (Nebenbefund ausserhalb der Spur, nicht bewertet: der Statusschirm
zeigt in diesem Kartenwerkzeug-Spielstand auch fuer Elza Leons Ausweis — K fasst keinen Statusschirm-Code an
(`git diff master...HEAD --name-only`: kein re15_inv_screen.c/inventory_common.c).)

Urteil Punkt 2: **erfuellt.** Beide Schluessel haengen je Szenario an der Nutzer-Marke (Huelle in der Marke +-1 px, aussen
0), auf dem CONTINUE- und dem Tuer-Weg, in Cut 5 und Cut 6, beim Hingehen sichtbar, aufnehmbar. Der Mangel M1 ist
behoben: der Prison Key ist im Spielfenster (960x720) ein heller, als Schluessel erkennbarer Bartschluessel (217 Pixel,
\|dL\| je Pixel 93 % des Parking Lot Key) statt des duennen dunklen Rings (112 Pixel, 49 %); Modell, Item-Bild und Icon sind
derselbe RE2-Gegenstand. Grenze (gemessen, kein Mangel): halbe Flaeche des Parking Lot Key bei voller Markenbreite; im
320x240-Bild ist der Schaft dunkel (siehe oben) — der Wortlaut verlangt "gut sichtbar an die Location", die Marke ist
voll ausgefuellt, groesser ginge nur ueber die Marke hinaus.

## 3. Punkt 1 — "fire extinguisher ... in room 1030 haengen an der stelle von fire_extinguisher.bmp, anstatt in room 1050"

Nachbesserung 1 aendert am Loescher nichts (`git diff f63a9634..HEAD -- re15_port`: schluessel_export.py,
itemorte_export.py, gen/schluessel_r36.inc, in gen/itemorte_r38_lage.inc/_md1.inc NUR die PRISON-Zeilen, Kommentare,
Riegel, probe_r38_itemorte_kontrast.c); am gebauten Stand neu gemessen. Marke (Abnahme 0): x 38..48 y 111..122, ROOM1030 Cut 0.

| Lauf | Befehl | Ergebnis |
|---|---|---|
| p1_mit/ohne, Skala 1 | `1030 -11500 -19500 2048 cut:0` [+ `flag:9:0x86 flag:3:0x85`], W3, Bild 100 | `[itemorte] ROOM1030: Slot 14 (msg 14) -> Slot 20`, `[feuerloescher] ROOM1030: Loescher an der Wand obj 5 bei (-2813,-2122,-17904) rot_y 3072, Zone Slot 14 Aufnahme Item 0x31 Bit (9,0x86)`, `[prop-render] pi=1 oid=0x05`; ohne: `kein Loescher (schon genommen, (9,0x86)=1)`. **59 Pixel im Fenster (x 36..50 y 109..124), 50 in der Marke, aussen 0, Huelle x 40..45 y 110..123** |
| p1s3_mit/ohne, Skala 3 (Spielfenster) | dasselbe mit RE15_WINDOW_SCALE=3 | 537 Pixel, Huelle x 121..138 y 331..372 (= Marke x3 x 114..146 y 333..368 +-1), aussen 0; Bild `k1/p1_s3.png` (Nutzerbild / Port / genommen): roter Loescher am Pfeiler unter dem Monitor, genau auf der roten Marke |
| p1t_mit/ohne, ECHTER TUER-WEG 1050 -> 1030 | `1050 20900 12350 0 cut:0`, `W1,A0.5,W8`, Ende `150#1030`, Bild 140 | `[tuer] Sequenz Archiv 2 DOOR13 Variante 0 ...`, `[room] PC loaded room1030.rdt`, Installer + `oid=0x05` auch hier; Ankunft (1172,-12558) Cut 2: **175 Pixel, Huelle x 271..280 y 87..110, aussen 0** (Loescher am Pfeiler) |
| p1_auf, Aufnahme | `1030 -4300 -17904 0 cut:0`, `W1,A0.1,W3.5,A0.1,W4,A0.1,W6`, Serie 100..500/50 | `[feuerloescher] Aufnahme Item 0x31 (9,0x86) -> Set(3,0x85)=1 wie ROOM1000 sub01 @0x00D0C`; dritter Druck `[msgtext] room=1030 id=14 "Seems like even the emergency broadcast has gone offline..."`; Bilder (`k1/p1_auf.png`): F100 Item-Bild, F150 "Will you take the", ab F200 Pfeiler leer, F400/F500 Monitor-Satz |
| p1_1050_mit/ohne (alter Ort) | `1050 15500 -21000 0 cut:6` [+ Bits] | keine `[feuerloescher]`-/`oid=0x05`-Zeile; **0 Pixel im ganzen Bild** |
| p1_1031 (Elza) | `1031 -11500 -19500 2048 cut:0 elza flag:3:0xC1 flag:3:0x7D` | `Spieler-Familie PL04`, kein `[feuerloescher]`/`[itemorte]`, `[prop-render]` nur obj 0 und obj 15 |

Urteil Punkt 1: **erfuellt** (wie Abnahme 0; Zahlen bitgleich: 59/50/0, 175, Aufnahme, Monitor-Satz, 1050 leer, Elza ohne).

## 4. RE-Gate (Fortsetzung) — Konstanten, Wortsuche, erklaert der Fix den Befund?

* Weitere Belege der Nachbesserung selbst gelesen: RE2 `ROOM20E0.RDT @0x018F8` `2d02 ... 0a00 1000` (Objekt 2, flach) mit
  `@0x019C0 4e04 0231 ... 1f00 0100 3c00 02a0` (Item 0x1F, md1 2), `@0x01A96` `2d02 ... 0a00 1000`; `ROOM2020.RDT @0x01C1E`
  `2d06 ... 0a00 1000` mit `@0x01C44 ... 1f00 ... 06a0` (Item 0x1F, md1 6) — RE2 stellt den Small Key an allen drei
  Fundstellen flach auf. ROOM20E0-Modell 2 = MD1 @0x2A98, md5 c5d49ef6... = bytegleich mit ROOM10F0 @0x1650 (wie zitiert).
  Das Small-Key-MD1 unterscheidet sich vom P.-Room-Key-MD1 nur in 24 Bytes (UV am Ende) — dieselbe flache 5-Ecken-
  Geometrie, deshalb dieselbe Wicklung/Sichtseite (+1y) in der Werkzeug-Ausgabe.
* Icon-CLUT: `ITEMALL_032/089/096.TIM` CLUT-Block je 512 B, md5 15a5a175 (gleich) — das Icon ist wie in Runde 36 ohne
  Farbumsetzung einsetzbar (Index 0 kommt in Tile 31 nicht vor, Abschnitt 1).
* Neue/geaenderte Konstanten der Nachbesserung: `RE15_R38K_PRISON_ROT_X/Y/Z` (1024,0,0) und `RE15_R38K_PRISON_S10000`
  53032 stehen in der ERZEUGTEN gen/itemorte_r38_lage.inc, Herleitung im Werkzeug-Kopf (NUTZER-VORGABE Marke / "gut
  sichtbar": groesster Massstab mit gezeichneter Huelle in der Marke; PORT-WAHL Lage in der Ebene: Sichtseite =
  Wicklungs-Vorderseite, Textur-u nach rechts) — reproduziert (Abschnitt 1). Die Quell-Offsets in schluessel_export.py
  tragen Datei-Offset + md5 (selbst geprueft). C-Quellen der Nachbesserung: NUR Kommentarzeilen
  (`git diff f63a9634..HEAD -- schluessel_r36.c re15_itemorte_r38.h`, Nicht-Kommentar-Zeilen: keine).
* Wortsuche im Code-Diff (`git diff master...HEAD -- re15_port`, nur +Zeilen) nach deferred/tunable/interim/for now/
  faithful/plausib/TODO/FIXME/ungefaehr/geschaetzt/schaetz/vermut/"sieht richtig"/getenv: **0 Treffer** — kein
  Env-Schalter als Abschluss.
* Erklaert der Fix den Befund? Ja. M1 war "Modell = duenner dunkler Ring" (Abnahme 0: 14/112 Pixel, L der Pixel nahe
  am Kork). Die Nachbesserung tauscht NUR das Modell (MD1/TIM/Icon/Bild aus demselben RE2-Gegenstand) und rechnet den
  Massstab nach derselben Regel neu; gemessen aendert sich genau die Groesse, die M1 benannt hat (Pixel x 1,9, Kontrast
  x 3,2/3,7, |dL| je Pixel 49 % -> 93..97 % des Parking Lot Key), alles andere (Ort, Zone, Aufnahme, Sichtregel) ist
  gleich wie in Abnahme 0 (B1/B2/V1..V4 dieselben Zahlen).

Gate: **haelt**.

## 5. Vertrag / Pfade / Tests / Nebenwirkungen

* Pfade (`git diff master...HEAD --name-only`, ohne Dossier/Belege): engine/src/{feuerloescher_r37.c, itemorte_r38.c,
  kistenregel_r37.c (1 Kommentarzeile), schluessel_r36.c, gen/itemorte_r38_lage.inc, gen/itemorte_r38_md1.inc,
  gen/schluessel_r36.inc}, include/{re15_feuerloescher_r37.h, re15_itemorte_r38.h, re15_schluessel_r36.h},
  platform/pc/main.c (+4: Include + 3-Zeilen-Haken), tests/{integration/test_r36_schluessel.cmake,
  test_r37_feuerloescher.cmake, test_r38_itemorte.cmake, unit/probe_r38_itemorte_kontrast.c, unit/probes/r38_itemorte.cmake,
  unit/test_r36_schluessel.c, test_r37_feuerloescher.c, test_r38_itemorte.c}, tools/{r36_schluessel/schluessel_export.py,
  r38_itemorte/itemorte_export.py}. **Kein** `release/`, `platform/android/`, `shared_assets/PSX/`, keine CMakeLists.txt.
  master hat seit der Basis 1bcbd0ff keinen Code geaendert (`git diff 1bcbd0ff master --stat -- re15_port` leer).
* Ressourcen: keine neuen Bank-9-Bits (Loescher (9,0x86)/(3,0x85), Schluessel (9,82)/(9,83)/(4,0xEC) aus Runde 36/37),
  keine Nachrichten-Ids, keine Assets, keine Texte. Die Nachbesserung bringt keine neue Engine-Quelldatei (nur das
  Test-Werkzeug probe_r38_itemorte_kontrast.c, registriert in probes/r38_itemorte.cmake ohne add_test).
* Riegel-Aenderungen der Nachbesserung gelesen: `test_r36_schluessel.c` nur auf das neue Modell nachgezogen (MD1 236 B /
  TIM 34848 B, Tile 31, Block 31, Drehung (1024,0,0)) plus zwei NEUE Pruefungen (Icon 0x42 ohne CLUT-Index 0, Modal-Leser
  0x42 > 4000 Bildpunkte); keine Pruefung entfallen. `test_r38_itemorte.cmake`: B3/B4 angehoben, B3K/B3V/B4K/B5K/B4V neu,
  V5 >= 45 — der Cabin Key faellt durch jede Schwelle (Zahlen der Abnahme 0 gegen die Schwellen nachgerechnet:
  B3 14 < 22, B3K 502 < 1300, B3V 15 % < 38 %, B4 112 < 180, B4K 3193 < 9500, B4V 11 % < 33 %, V5 29 < 45).
* `test_r38_itemorte` direkt: **50 Pruefungen, 0 Fehler**; Teil `lage`: `ROOM10F1 Cut 5: 1565 sichtbare Abtastpunkte,
  Huelle x 132.57..144.49 y 78.35..84.23, 0 ausserhalb`, "11.9 von 11 Spalten", `16 Pruefungen, 0 Fehler`.
* **Gezielter ctest-Lauf am gebauten Stand** (eigene Auswahl `k1/regex.txt` = die 119er-Auswahl des Bau-Agenten +
  `unit_1090_` (alle) + `probe_1090_` + `unit_prop_texel_key_11f0|unit_map_1090|unit_pri_inventar_riegel` = alle Gruppen
  der beruehrten Module (feuerloescher_r37.c, schluessel_r36.c, Prop-Zeichner main.c, kistenregel_r37.c) und Raeume
  (1030/1031, 1050/1051, 1090/1091, 10F0/10F1, 11B0-Tuer, 1211-Zelle, Inventar/Itembox/Censusse); sauberer PATH wie
  local_build.sh, `-j 3 --timeout 600 --output-on-failure`, 05:32-05:49):
  **`100% tests passed, 0 tests failed out of 130`** (Total Test time (real) = 1045.50 sec), darin
  integration_r38_itemorte 370.21 s, integration_r37_feuerloescher 326.32 s, integration_r36_story_leon 370.31 s,
  integration_r36_kartenitems_bild 434.01 s, integration_r37_kistenregel 275.83 s, integration_r35_cut10f0 249.93 s.
  Keine roten Fenster-Haken (nichts nachzufahren).
* Framedumps MEINES ctest-Laufs (`build/tests/integration/r38_itemorte_wd/lauf_e22775f4_*`, 05:39) mit diff.py
  nachgerechnet: B1 59, B2 55/3428, B3 27/1627, B4 217/11899, B5 491/28852, V1 0, V2 0, V3 108/5607, V4 0, V5 56/2690,
  aussen ueberall 0 — gleich wie meine Einzellaeufe und Dossier 9.6/9.7.
* Nebenwirkungen beide Szenarien (gemessen): Leon 10F0 Parking Lot Key unveraendert (55/3428, MD1 bitgleich, 0
  Verkleinerungsschritte); Leon 1050 leer (0 Pixel); Elza 1031 ohne Loescher; Elza 10F1 Aufnahme setzt (4,0xEC) fuer die
  Zelle ROOM1211 (integration_r36_elza_unlock und integration_r36_schluessel(_befreiung) gruen); Inventar-CHECK des Prison
  Key (Karte `10F1 1300 -2900 1024 cut:5 elza flag:9:83 leer give:0x42:1`, `RE15_INV_SHOT=inv.bmp RE15_INV_CHECK_SHOT=1`):
  `[inv] CHECK-Foto Item 0x42: crect (0,489) prect (832,256) 56x72 -> Foto und CLUT geladen`, Bild `k1/check_e.png`:
  Foto = Small Key, Text "I can access the / p(rison cells ...)" (Port-Text unveraendert), Icon = Small Key. Weitere
  Verbraucher der Prison-Key-Daten: nur der Prop-Lader main.c:1562 und die Icon/Bild-Einsetzer schluessel_r36.c:337/346
  (grep). Der PSX-Port zeichnet diese Port-Props nicht (unveraendert).

Vertrag: **haelt**.

## 6. Hinweise (kein Mangel, fuer die Integration)

* H1 (aus Abnahme 0, unveraendert): ROOM1030 Slot 20 und ROOM10F0/10F1 Slot 15 muessen bei der Integration frei bleiben,
  sonst bleibt der Original-Satz im Zonen-Slot (Gegenstand sichtbar, nicht aufnehmbar); die Logzeile
  `[schluessel] ... Zone Slot 8` erschiene trotzdem. Nach dem Merge `unit_r38_itemorte_raum` fahren.
* H2: Kommentar-Drift `scd_room_setup.c` ("Feuerloescher 1050/1090/1091") bei der Integration nachziehen.
* H3: `engine/src/itemorte_r38.c` ist eine neue Engine-Quelldatei (GLOB) -> Android-Bau neu konfigurieren.
* H4: Der Kopf von gen/itemorte_r38_lage.inc nennt fuer den Prison Key die affine Huelle (x 132.53..144.42 y 78.68..84.28),
  Dossier 9.4/9.5 die gezeichnete (x 132.53..144.49 y 78.35..84.28) — beides Werkzeug-Ausgabe, kein Widerspruch.
* H5: Im 320x240-Bild (Skala 1, Riegel-Messgroesse) liest sich der Small Key als heller Ring mit dunklem Schaft
  (L-Mittel der geaenderten Pixel 57 gegen 73 Untergrund); im Spielfenster (960x720 auf dieser Maschine, Standard-Skala
  3..4) ist er ein heller Bartschluessel. (Steam Deck/Android nicht gemessen; dort laeuft derselbe render_pc.c-Pfad im
  Vollbild mit logischer 320x240-Groesse, render_pc.c:619-638.)

## 7. Maengel

Keine.

## 8. Ergebnis

| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Feuerloescher in ROOM1030 an der Stelle aus fire_extinguisher.bmp statt 1050 | **erfuellt** | Abschnitt 3: 59 Pixel / 50 in der Marke / aussen 0 (Skala 1), 537 (Skala 3, `k1/p1_s3.png`), Tuer-Weg 1050->1030 175 Pixel, Aufnahme + Monitor-Satz, 1050 0 Pixel, Elza 1031 ohne |
| 2 Parking Lot Key / Prison Key je Szenario gut sichtbar an der Stelle aus key_location.bmp | **erfuellt** | Abschnitt 2: Elza 27/1627 (S1), 217/11899 (S3 = Fenster des Nutzers), Leon 55/3428 bzw. 491/28852, aussen 0, Huelle in der Marke; Bilder `k1/s3_1zu1.png`, `s3_zoom3.png`, `cut6_s3.png`, `geh_s3.png`; Tuer-Weg 10D1->10F1 217 Pixel; Aufnahme + (4,0xEC), Item-Bild/Icon/CHECK = Small Key |
| RE-Gate | haelt | Abschnitte 1 und 4: ROOM10F0 @0x6F0/@0x706 + Modelltabelle, RE2 0x80055260/@0x80055320, @0x800365e8 (Sprungziele nach NCCT durchsucht), RE1.5 @0x8002c2f4, Text @0x800A11EF, ROOM20E0/2020; gen/*.inc bitgleich reproduziert; 0 Rate-Woerter |
| Vertrag / Tests | haelt | Abschnitt 5: Pfade erlaubt, keine neuen Ressourcen, gezielt **130/130** gruen, Riegel messend |

**Bestanden: JA** — Mangel M1 der Abnahme 0 ist behoben (Prison Key = RE2 Small Key, im Spielfenster klar als Schluessel
an der Nutzer-Marke zu sehen); Punkt 1 unveraendert erfuellt; Gates halten.

(Messdaten, Skripte und Bilder der Abnahme liegen im Scratchpad der Sitzung unter `k1/` — `lauf.sh`, `diff.py`,
`regen.py`, `regex.txt`, `ctest.log`, `runs/<lauf>/debug.log + fd_*.ppm`; nicht committet.)
