# Runde 38 — Spur K "itemorte" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_itemorte` (Zweig `r38/itemorte`, Basis 1bcbd0ff). Beginn 2026-10-16 16:34.

## Punkte (Wortlaut, AUFTRAG.md)

1. „Ich hätte den fire extinguisher gerne in room 1030 hängen an der stelle von fire_extinguisher.bmp, anstatt in room 1050."
2. „Ich möchte das du den Parking Lot key, und den Prison Key - je nach Szenario - gut sichtbar an die Location platzierst,
   die ich im Bild key_location.bmp markiert habe."

Ressourcen (VERTRAG §1.2): Nachrichten-Ids 1030/1031 29-31 (Loescher), 10F0/10F1 nur vorhandene Ids — beide NICHT
gebraucht (keine neue Nachricht). Gemeinsame Dateien (VERTRAG §1.4): schluessel_r36.c + feuerloescher_r37.c gehoeren
dieser Spur. Kein Bank-9-Bit neu (Loescher (9,0x86)/(3,0x85) wie Runde 37, Schluessel (9,82)/(9,83)/(4,0xEC) wie Runde 36).

## Stand

- 16:34 Dossier angelegt; Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE, Runde-37-Dossier I (Loescher) + Abnahme 1,
  Runde-36-Dossier Q (Schluessel), Runde-36-Dossier N (Lageplaene = dasselbe Verfahren "Marke im Nutzerbild -> Wand").
- 17:17 (Commit d7934141) Messung vorher, Cut-Zuordnung, Geometrie (Pfeiler/Pinnwand), RE-Belege fertig (Abschnitte 1/2); Werkzeug
  `re15_port/tools/r38_itemorte/itemorte_export.py` rechnet Lage, Drehung, Massstab.
- 17:37 (e8461907) Umsetzung; 17:48 (36a14abc) Riegel; 18:00 (025531bb) Messung nachher (Abschnitte 3/4), RE2-Licht der
  Schluessel belegt (2.7).
- 18:18 Gezielter Lauf 112/112 gruen (5.3); Dossier fertig. Danach Sitzungslimit (Reste c01e409e, ungeprueft).
- 21:05 FORTSETZUNG: Stand gelesen (Dossier, `git log --stat master..HEAD`, Code-Diff), Bau am Endstand neu
  (`local_build.sh build` OK 21:11; die gen/*.inc trugen mtime 18:18:52 = Neuerzeugung beim Abbruch, Inhalt
  bytegleich mit e8461907, `git status` leer). Wortlaut-Pruefung (Abschnitt 8): Luecken waren nur die Elza-Messung
  der anderen Pinnwand-Cuts (10F1 Cut 6/7) und eine Zahl fuer "gut sichtbar" in der Fenstergroesse, in der gespielt
  wird -> gemessen (4.2 "Fortsetzung"), Riegel V4/V5/B4 (8544862d), gezielter Lauf am Endstand (5.3).

## 1. Messung vorher (Stand 1bcbd0ff, Baum gebaut 16:40, Messkopie `re15_pc_r38k_mess.exe`)

Lade-Weg: Kartenwerkzeug `probe_r37_feuerloescher_karte` + CONTINUE (`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`),
Bilder `RE15_FRAMEDUMP` mit `RE15_WINDOW_SCALE=1`; Lauf-Skript + Rohdaten im Scratchpad (`k/lauf.sh`, `k/runs/`).

* **ROOM1030 Leon, Cut 0** (Karte `1030 -11500 -19500 2048 cut:0`, Bild 100): debug.log OHNE jede
  `[feuerloescher]`-Zeile, `[prop-render]` nur obj 0 (Deckenventilator) und obj 15 (1F-Lageplan Runde 36); an der
  Marke (x 38..48, y 111..122) die leere Pfeilerflaeche (8 von 132 Pixeln weichen vom Engine-Dekoder-Hintergrund um
  > 30 ab = Dekoderrauschen, kein Objekt). Der Loescher haengt heute in ROOM1050 (Runde 37: obj 5 bei
  (17200,-1150,-23775), Slot 13).
* **ROOM10F0 Leon, Cut 5** (Karte `10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94`, Bild 100): an der Marke
  (x 133..143, y 77..85) **0 von 99** Pixeln anders als der Hintergrund; `[schluessel] ROOM10F0: PARKING LOT KEY ...
  Prop obj 4 bei (8050,-1460,2700), Zone Slot 15` = Runde-36-Lage auf der Funkkonsole (im Cut 5 nicht im Bild).
* Sonde `probe_r34n_e_dokumente aots` (echter Raumstart mit allen Port-Installern): ROOM1030 Slots 0..17 Original,
  8 = 1F-Lageplan (Telefon nach 39), erster freier Slot 18; ROOM1031 zusaetzlich 18 = Tuer (Opening), erster freier 19;
  ROOM10F0/10F1 Slots 0..14 Original, 15 = Schluessel (Runde 36), erster freier 16.

### 1.1 Welcher Raum, welcher Cut (Bildvergleich, kein Augenmass)
Engine-Dekoder `probe_bg_dump` (alle 428 STAGE1-Hintergruende, standalone gebaut aus bss_common/bss_vlc/bss_mdec),
mittlere Abweichung je Pixel AUSSERHALB der Marke (Verfahren `tools/r34n_e/marken.py`, hier ueber alle Raeume):

| Nutzerbild | bester Cut | Abweichung | naechste | Marke (Huelle, Mitte) |
|---|---|---|---|---|
| fire_extinguisher.bmp | **ROOM1030 Cut 0** | 15,94 | Cut 9 (dieselbe Kamera, Zustand nach Cut_replace) 22,29; Rest >= 62,09 | x 38..48, y 111..122 (132 Pixel, voll), (43,5 ; 117,0) |
| key_location.bmp | **ROOM10F0 Cut 5** | 4,34 | ROOM1030 Cut 8: 90,92 | x 133..143, y 77..85 (99 Pixel, voll), (138,5 ; 81,5) |

=> Feuerloescher: Lobby ROOM1030 (Rolltor im Hintergrund), Marke auf dem Pfeiler unter dem Notruf-Monitor.
=> Schluessel: ROOM10F0 = Communications Room (derselbe Raum wie Runde 36), Marke auf der Pinnwand an der Rueckwand
   unten rechts. Der Prison Key bleibt damit im Communications Room (VERTRAG §1.5, Spur G Tagebuch).

## 2. RE-Belege und Geometrie

### 2.1 Aktions-Scan (Vorrang, Pruefpunkt) — PSX.EXE, `re15_disasm.py dis`
```
80042bd0  ori  v0,zero,0x26c        ; Pruefpunkt 620 vor dem Spieler
80042c50  lw   s0,0(s4)             ; Schleife ueber die AOT-Saetze in Slot-Reihenfolge (s2 = Slot+1)
80042ea8  andi v0,v1,0x40           ; sat 0x40: Spieler-Lage gegen das Rechteck (bei sat 0x31 aus)
80042ef8  andi v0,v1,0x20           ; sat 0x20: 620-Punkt (sp+24) gegen das Rechteck
80042f50  beq  v0,zero,0x8004301c   ; sce == 0 (genommene Item-Zone) -> weiter zum naechsten Slot
80042f8c  jalr v0                   ; Handler PTR_8007469c[sce]
80042f94  j    0x80043028           ; -> Ende: der KLEINSTE treffende Slot gewinnt
```
Folge: eine Aufhebe-Zone muss im Rechteck ihres Pruefpunkts den kleinsten Slot haben. ROOM1030 Slot 14 (msg 14
"Seems like even the emergency broadcast has gone offline...", main00 @0x01DA6 `2c 0e 01 31 ...`, x -4190..-2860
z -18984..-17198) deckt die ganze Stelle vor dem Pfeiler; ROOM10F0/10F1 Slot 8 (msg 4 "Several files from past
cases. Nothing useful here.", main00 @0x00FEA `2c 08 01 31 ...`, x 700..2300 z -3800..-3000) deckt die Pinnwand.
Loesung wie Runde 36 N (Lageplan 1F nimmt Slot 8 des Muenztelefons, das Telefon zieht nach 39, kartenitems_r36.c):
die Zone nimmt den Slot, der Original-Satz zieht UNVERAENDERT in einen freien Slot (ROOM1030: 20, frei in 1030 UND
1031; 10F0/10F1: 15, frei geworden). Ausserhalb des Zonen-Rechtecks antwortet der Satz wie bisher; nach der Aufnahme
ist die Zone sce 0 (@0x80042f50 uebersprungen) und der Satz antwortet ueberall. Kein Skript fasst die Slots an
(scd_dump_room.py: ROOM1030/1031 nur main00 Aot_set 14; ROOM10F0/10F1 nur main00 Aot_set 8).

### 2.2 ROOM1030 — der Pfeiler (Triangulation, Strahlen der Engine-Matrix `probe_r34n_e_dokumente kamera 1030`)
Der Pfeiler ist in Cut 0, 1, 2, 3 sichtbar. Senkrechte Kanten: jede Bildspalte ergibt in der Draufsicht eine Gerade,
Schnitt kleinster Quadrate (Kantenspalte = 50-%-Uebergang im Helligkeitsprofil, Pixel p deckt [p, p+1)):

| Kante | Beobachtungen (Cut: Spalte @ Zeile) | Ergebnis (x, z) | Rest |
|---|---|---|---|
| (-x,+z) | Cut 3: 228,6 @55; Cut 1: 296,6 @62 | (-2668, -17564) | 0 |
| (-x,-z) | Cut 0: 49,4 @72; Cut 1: 318,3 @57, 317,5 @67, 314,6 @110, 313,8 @125 | (-2709, -18459) | <= 8 |
| (+x,+z) | Cut 3: 206,5 @55; Cut 0: 23,3 @72 | (-1790, -17606) | 0 |

Pfeiler ~ 900 x 880 Einheiten (0,53 m). Kontrollpunkte: Aushang-Mitte (Cut 0/2/3) = (-2268,-3871,-17581), liegt auf
der +z-Flaeche (Bildfehler <= 0,6 px in allen drei Cuts); Monitor-Anzeige (Cut 0/1/2) = (-3225,-3940,-18026).
In Cut 0 zeigt der Pfeiler links die +z-Flaeche (Spalten 24..35,3, mit Aushang) und rechts die -x-Flaeche
(35,3..49,4, mit der hellen Platte unter dem Monitor). **Die Marke (Spalten 38..48) liegt auf der -x-Flaeche.**
In Cut 1 (Spalten 296..318) dieselbe -x-Flaeche frontal mit Monitor und heller Platte (Bild z_c1_rechts).
-x-Flaeche achsparallel: x = Mittel der beiden -x-Ecken = -2688.

### 2.3 ROOM10F0 — die Pinnwand
Rueckwand: SCA-Zelle 0 (x -600..11300, z -5400..-3400, alle 5 Baender) mit Flaeche z = -3400. Gegenprobe:
zwei Zettel der Pinnwand aus Cut 5 + Cut 6 trianguliert (kleiner Zettel unten rechts (1001,-2443,-3481), Foto oben
Mitte (1790,-3277,-3352), Bildfehler <= 0,2 px). Pinnwand auf z -3400: x 691..3240, y -3659..-2222 (Cut 5,
Rahmenlinien Spalte 107/149, Zeile 62/86); Marke = x 1074..1678, y -2769..-2277.

### 2.4 Sicht in den anderen Cuts (Sonde `sicht`, Anker-Viereck FUN_80014368)
Das Original zeichnet ein Objekt nur, wenn sein Ursprung im Anker-Viereck des aktiven Cuts liegt (FUN_8002c18c:
`FUN_80014368(&pos, DAT_800ac790)`, Port re15_prop_culled) und verdeckt es mit den Masken des Cuts.
* ROOM1030, Loescher-Mitte (-2813,-2518,-17904): Cut 0 (43,4 ; 117,1) keine verdeckende Maske (Maske 49 des
  Pfeilers erst ab vz 12236, Loescher 11277); Cut 1 (299,9 ; 115,1) frei; Cut 2 (276,0 ; 99,1) frei; Cut 3
  (225,1 ; 118,2) VERDECKT von Maske 2 (Pfeiler, ab vz 8200) — richtig, die -x-Flaeche liegt aus Kamera 3 hinter dem
  Pfeiler. ⛔ **Cut 10** (= Kamera 3 nach `Cut_replace 3 -> 10`, sub00 @0x01FF7 / sub08 @0x02785, sobald (4,0x0F)
  steht — Rolltor-Szene sub08) traegt fuer denselben Pfeiler Maske 2 mit Tiefe 164 (ab vz 10570): der Loescher
  (vz 8295) schiene DURCH den Pfeiler. Das Anker-Viereck von Cut 10 ist nach dem Tausch das alte von Cut 3 (Zone 12,
  op_cut_replace tauscht cam_from/cam_to), der Loescher liegt darin.
  Cut 9 (= Kamera 0 nach dem Tausch): die Sonde meldet die Tisch-Maske 3 (Rechteck x 0..47, y 121..136) ueber den
  untersten Zeilen; an der exe gemessen deckt sie dort nichts (58 Pixel, Huelle y 110..123, Cut 0: 59) — die Maske
  ist ein Sprite, nur ihre Tisch-Pixel sind deckend, die Pfeilerflaeche im Rechteck ist durchsichtig.
* ROOM10F0, Schluessel-Mitte (Vorab-Punkt (1348,-2494,-3390), endgueltig (1358,-2495,-3400)): Cut 5 (138,7 ; 81,5) frei; Cut 6 (90,6 ; 72,8) frei, auf der
  Pinnwand; Cut 0/1/11 auf den Trennwaenden (Anker-Vierecke enthalten den Ursprung NICHT -> gar nicht gezeichnet,
  richtig). ⛔ **Cut 7**: Ursprung im Anker-Viereck (Zone 20), Bildpunkt (99,0 ; 76,8) liegt auf dem Buecherregal
  (das Regal steht aus Kamera 7 vor dem unteren rechten Teil der Pinnwand), Cut 7 hat **keine** Masken -> der
  Schluessel schiene durch das Regal.
  => Fuer diese zwei Faelle ist ein Sichtbarkeits-Haken noetig (Abschnitt 3); Masken nachzeichnen ginge nur ueber
  shared_assets/PSX/MASKS (TABU).

### 2.5 Kamera bei der Aufnahme
ROOM1030: an jeder Standstelle vor den Tischen (x -4100..-4600, z -17700..-18200) fuehren alle Zonen nach Cut 0
(`zonen 1030`): beim Aufheben sieht der Spieler genau das Nutzerbild. ROOM10F0: vor der Pinnwand (x 500..1800,
z -2900..-1300 begehbar, `stand 10F0`) gelten Cut 5/6/7-Vierecke; Uebergaenge 5->6 nur bei x -1500..-1000.

### 2.6 Die RE2-Schluesselmodelle (aus den Bytes, md5 wie Runde 36)
* P. Room Key (ROOM6150.RDT MD1 @0x004FBC): 5 Ecken (+-76, 0, +-38) + Mitte, 4 Dreiecke, Normalen (0,+4096,0);
  UV: u waechst mit Modell +x (0..127), v mit +z (192..255), CLUT-Feld 0x78C0 = Zeile 483. Von -y (oben) gesehen ist
  die Textur gespiegelt (det -0,693) und die Dreiecke sind Rueckseiten (Kreuzprodukt -5776); von +y nicht gespiegelt,
  Vorderseite (+5776). Der Port zeichnet Props ohne Rueckseiten-Test (Prop-Pfad main.c), die Seite entscheidet also
  nur, ob die Textur gespiegelt erscheint.
* Cabin Key (ROOM1050.RDT MD1 @0x001558): 1 Viereck (+-90, 0, +-90), Normale (0,-4096,0), UV-Parallelogramm
  (93,28)(58,58)(55,1)(20,31), CLUT 0x7800; von -y nicht gespiegelt (det +0,064), Vorderseite (+32400).
* Sichtbare Texel (CLUT-Farbe != 0x0000): P. Room Key 3642, Cabin Key 313.
* Natuerliche Groesse an der Pinnwand (Cut 5, vz ~ 13000): 152 Einheiten = 2,4 Bildpunkte — NICHT "gut sichtbar".

### 2.7 Licht der RE2-Schluesselmodelle — RE2 zeichnet sie FLACH (selbst disassembliert, info/re2leon/PSX.EXE)
RE2-SCD-Opcode-Tabelle 0x800a74c8 (geprueft: Eintrag 0x37 -> LAB_8005655c = Sca_id_set, Eintrag 0x2E = 0x80055904 =
Work_set mit Pool 0x800d0324 + id*0x1F8 @0x80055994..ac). Eintrag 0x2D @0x800a757c = **0x80055260 = Obj_model_set**:
```
80055290  lbu  t1,1(s2)          ; pc[1] = Objekt-Id
80055298..800552a8               ; s1 = 0x800cc1e8 + 16700 + id*504 = Pool 0x800d0324 + id*0x1F8
800552d0  lhu  v0,10(s2)         ; pc[10..11] Freigabe-Wort
800552d8  ori  v0,v0,0x1         ; |1 -> Pool+0 (@0x800552e4)
8005530c  lh   a1,12(s2)         ; pc[12..13] ATTRIBUT-WORT
80055320  sw   a1,16(s1)         ; -> Pool+0x10
```
Zeichner FUN_800363f0: @0x800365e0 `lw v0,16(s1)`, @0x800365e8 `andi v0,v0,0x10` -> mit Bit FUN_8002d0e8 (@0x80036610,
unbeleuchtet), sonst FUN_80027bec (NCCT). Die Schluessel-Saetze in RE2 Retail:
`ROOM6150.RDT @0x02C98 2d 08 00 00 00 00 00 00 00 00 00 00 10 00 ...` (P. Room Key, Objekt 8) und
`ROOM1050.RDT @0x01274 2d 00 00 00 00 00 00 00 00 00 0a 00 10 00 ...` (Cabin Key, Objekt 0): pc[12..13] = 0x0010.
=> RE2 stellt BEIDE Schluessel flach auf (Textur unveraendert, 0x808080). Runde 36 hatte sie mit Attribut 0 (NCCT)
gesetzt; an der Pinnwand gemessen ist der Parking Lot Key damit ein dunkelgrauer Strich (Messung 4.2). RE1.5-Gegenstueck
des Zweigs: FUN_8002c18c @0x8002c2f4 `andi v0,v0,0x10` -> FUN_80022f0c @0x8002c320 (Runde 36 N 8.3). (Damit ist auch die
Luecke der Runde 36 N geschlossen, die den RE2-Handler "positionell" angenommen hatte: er steht jetzt mit Adresse fest.)

### 2.8 Vorgaben, die NICHT aus dem Original kommen (NUTZER-VORGABE / PORT-WAHL, Form belegt)
| Groesse | Wert | Herkunft |
|---|---|---|
| Ort Loescher | Mitte = Sehstrahl der Markenmitte (43,5 ; 117,0) x Achsenebene x = -2688 - 125 | NUTZER-VORGABE (Marke); Form Runde 36 N ("Mitte = Sehstrahl der Markenmitte x Ebene") |
| Wandabstand | Achse R + 15 = 125 vor der Flaeche | Runde 37 Dossier I 3.2 (dieselbe Regel wie ROOM1050) |
| rot_y Loescher | 3072 (Modell -z = Wandseite -> Welt +x, Etikett zum Raum) | Runde 37: "rot_y 0 = Etikett zum Raum" an einer z-Wand |
| Groesse Loescher | unveraendert 0,47 m (Netz Runde 37) | derselbe Loescher wie in 1090/1091 (dort liegt er spaeter leer) |
| Ort Schluessel | Mitte des SICHTBAREN Texel-Rechtecks auf dem Sehstrahl der Markenmitte (138,5 ; 81,5), Ebene z -3400 | NUTZER-VORGABE (Marke) |
| Massstab Schluessel | groesster Wert, bei dem alle sichtbaren Texel in der Marke liegen (+-0,5/+1,5 px): P. Room Key 6,4636, Cabin Key 3,5896 | NUTZER-VORGABE "gut sichtbar"; Form Runde 36 N ("Massstab s = groesster Wert ... in der Marken-Huelle") |
| Lage in der Ebene | Textur-u waagerecht nach rechts, Sichtseite = Wicklungs-Vorderseite = nicht gespiegelte Seite | PORT-WAHL (2.6); P. Room Key: Kopf rechts wie im RE2-Itembild ITPS 95 |
| Zonen | 1000 x 1000 (r35-Muster, 69 von 162 Item_aot_set); 1030 Mitte x = Klemmgrenze -4100 + 620 (@0x80042bd0), z ab -18370 (Slot 13 endet -18372); 10F0 Mitte (x_Marke, Wand + 300) wie Runde 36 N, rechts vor Slot 12 (x 1800) beendet | Runde 35/36-Muster |
| Ausweich-Slots | ROOM1030 20 (frei in 1030 und 1031), 10F0/10F1 15 (bisheriger Schluessel-Slot) | Muster Runde 36 N (Telefon 8 -> 39) |

## 3. Umsetzung

### 3.1 Dateien
Neu: `engine/src/itemorte_r38.c`, `include/re15_itemorte_r38.h`, `engine/src/gen/itemorte_r38_lage.inc` (#define-Lagen),
`engine/src/gen/itemorte_r38_md1.inc` (zwei skalierte RE2-MD1), Werkzeug `tools/r38_itemorte/itemorte_export.py`
(erzeugt beide .inc aus Nutzer-BMP, Engine-Kameramatrix, gemessener Ebene; RE2-Bytes md5-geprueft wie Runde 36),
`tests/unit/test_r38_itemorte.c`, `tests/unit/probes/r38_itemorte.cmake`, `tests/integration/test_r38_itemorte.cmake`.
Haken (Kommentar "Runde 38 Spur K"):
* `engine/src/feuerloescher_r37.c` (Spur-K-Datei laut VERTRAG 1.4): Lage-Zeile 0x1050 -> RE15_R38K_FL_* (0x1030, Slot 14),
  1 Haken `re15_itemorte_r38_platz_frei(room_id, slot)` vor dem Zonen-Test, Include, Kommentare; Kopf
  `include/re15_feuerloescher_r37.h` (Fundort-Satz, Pool-Id-Kommentar).
* `engine/src/schluessel_r36.c` (Spur-K-Datei): schluessel_anlegen nimmt Lage/Drehung aus re15_itemorte_r38_schluessel_lage,
  Attribut RE15_R38K_SCHL_ATTR, Zone Slot 8 + Haken platz_frei; re15_schluessel_r36_md1_bytes liefert das skalierte MD1.
  `include/re15_schluessel_r36.h`: Konsolen-Lage der Runde 36 (X/Y/Z/ROT_Y/SLOT/RECT_*) entfernt, Verweis auf R38.
* `platform/pc/main.c`: Include + 3 Zeilen nach `re15_prop_culled` im Prop-Zeichner (`re15_itemorte_r38_verdeckt`).
Keine Bank-9-Bits, keine Nachrichten-Ids, kein Asset, kein Patch an shared_assets.

### 3.2 Konstanten (gen/itemorte_r38_lage.inc, Herleitung 2.2/2.3/2.8)
* Loescher: Ursprung (Bodenmitte) (-2813,-2122,-17904), rot_y 3072, Zone x -3980..-2980 z -18370..-17370 (Slot 14).
* Parking Lot Key: (1358,-2495,-3400) rot (3072,0,2048), Massstab 6,4636 (sichtbar 735 x 468 Einheiten).
* Prison Key: (1358,-2495,-3400) rot (1024,3618,2048), Massstab 3,5896 (sichtbar 538 x 620).
* Schluessel-Zone x 798..1798 z -3600..-2600 (Slot 8); Licht 0x0010 (2.7).

### 3.3 Sichtbarkeitsregel (PORT-ZUSATZ, gemessen begruendet)
`re15_itemorte_r38_verdeckt`: ROOM1030 Cut 3 und Cut 10 (Loescher obj 5), ROOM10F0/10F1 Cut 7 (Schluessel obj 4) werden
nicht gezeichnet. Messung ohne Regel (Lauf mit ausgeschaltetem Haken, sonst identisch, Unterschied mit/ohne Taken-Bit):
Cut 10 102 Pixel x 222..227 y 108..127 (Loescher VOR dem Pfeiler), Cut 3 36 Pixel x 224..227 y 109..124 (rechts neben
der Maske 2, die bei x 226 endet, die Pfeilerkante liegt bei 228,5), 10F0 Cut 7 44 Pixel x 94..104 y 72..80 (Schluessel
ueber dem Buecherregal). Mit Regel jeweils 0 Pixel. Geometrisch liegt der Loescher aus Kamera 3/10 hinter dem Pfeiler
(sichtbarer Rand rechnerisch 13 Einheiten = 0,3 px), der Schluessel aus Kamera 7 hinter dem Regal (Bildpunkt im Regal).
Bild `K_belege/k3_ohne_regel_cut10_mit_regel_cut10_ohne_regel_cut7.png`.

## 4. Messung nachher (Baum gebaut, Messkopie `re15_pc_r38k_mess.exe`, Lade-Weg CONTINUE)

### 4.1 Punkt 1 — Feuerloescher
* `aots 1030`: `[itemorte] ROOM1030: Slot 14 (msg 14) -> Slot 20`, `[feuerloescher] ROOM1030: Loescher an der Wand obj 5 bei
  (-2813,-2122,-17904) rot_y 3072, Zone Slot 14 Aufnahme Item 0x31 Bit (9,0x86)`; Slot 14 ITEM x[-3980..-2980]
  z[-18370..-17370] prop 5; Slot 20 MESSAGE evt 14 x[-4190..-2860]. `aots 1031`: kein Loescher, Slot 14 msg 14.
  `aots 1050`: kein Slot 13, kein obj 5.
* Bild Nutzer-Cut (ROOM1030 Cut 0, Bild 100, mit/ohne (9,0x86)): **59 Pixel, Huelle x 40..45 y 110..123, 50 davon in der
  Marke x 38..48 y 111..122**, ausserhalb des Fensters (Marke +-2 px) 0. Netz-Projektion (Unit-Riegel, Engine-Kamera):
  x 40,9..46,3 y 109,6..124,4, Mitte (43,6 ; 117,0) = Markenmitte (43,5 ; 117,0). Bilder
  `K_belege/k1_nutzerbild_vs_port_1030_cut0.png` (links Nutzerbild, rechts Port, Marke rot umrandet),
  `k1_1030_cut0_mit_ohne_6x.png`.
* Andere Cuts (RE15_FORCE_CUT, mit/ohne): Cut 1 90 Pixel x 296..303 y 106..122 (frontal auf der -x-Flaeche), Cut 2 175
  Pixel x 271..280 y 87..110, Cut 9 (= Kamera 0 nach dem Rolltor-Tausch) 58 Pixel x 40..45 y 110..123, Cut 3 0, Cut 10 0.
* Aufnahme (exe, Stand (-4300,-17904) Blick +x, Cut 0, Skript A@90/A@198/A@321): Modal mit ITPS-Bild, "Will you take the
  Fire Extinguisher?", Ja -> `[feuerloescher] Aufnahme Item 0x31 (9,0x86) -> Set(3,0x85)=1 wie ROOM1000 sub01 @0x00D0C`;
  Bild 200: der Pfeiler ist leer; dritter Druck an derselben Stelle -> `[msgtext] room=1030 id=14 "Seems like even the
  emergency broadcast has gone offline..."` (Bild `K_belege/k1_aufnahme_1030_ablauf.png`).
* Kamera beim Aufheben = Cut 0 (alle Zonen an der Standstelle fuehren nach Cut 0, 2.5): der Spieler sieht das Nutzerbild.
* Elza: ROOM1031 kein Loescher (Runde 37: in ROOM1051 hing auch keiner — Leon war zuerst da und hat ihn benutzt),
  ROOM1091 der leere Loescher am Boden unveraendert (`Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300)`).
  Leon ROOM1090 Loesch-Ablauf unveraendert (Riegel unit_r37_feuerloescher_loeschen/kiste, integration A/F/G/I/M/N/O).

### 4.2 Punkt 2 — Schluessel an der Pinnwand
* `aots 10F0/10F1`: `[itemorte] ROOM10F0: Slot 8 (msg 4) -> Slot 15`, `[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45,
  Bit (9,82)) Prop obj 4 bei (1358,-2495,-3400) rot (3072,0,2048), Zone Slot 8`; 10F1 PRISON KEY rot (1024,3618,2048).
* Bild Nutzer-Cut (ROOM10F0/10F1 Cut 5, Bild 100, mit/ohne Taken-Bit): Parking Lot Key **55 Pixel, Huelle x 132..143
  y 78..84**, Prison Key **14 Pixel, Huelle x 134..143 y 78..85**, ausserhalb des Fensters (Marke +-2) je 0.
  Erste Fassung mit NCCT-Licht (Runde-36-Attribut 0): Parking Lot Key 50 Pixel, mittlere Farbe (91,82,82) gegen Kork
  (71,70,55) = dunkler Strich; mit RE2-Attribut 0x0010 (2.7) silberne Klinge, schwarzer Kopf, rotes Emblem.
  Netz-Abtastung (Unit-Riegel): ALLE sichtbaren Texel in der Marke (+-0,5/+1,5), Huelle x 132,59..144,41 (Park) bzw.
  134,08..144,47 (Prison) = die Marke ist ausgefuellt.
* Fensterskala 3 (RE15_WINDOW_SCALE=3, so spielt der Nutzer; 3D-Geometrie in Fensteraufloesung): der Parking Lot Key als
  Schluessel mit Zacken und rotem Emblem, der Prison Key als Ring mit kleinem Schluessel (`K_belege/
  k2_10f0_10f1_fensterskala3.png`). Bei Skala 1 ist die Ringlinie des Cabin Key (1..1,5 Texel = 0,3 px) nur
  stellenweise getroffen.
* Cut 6 (die zweite Kamera auf die Pinnwand): 108 Pixel x 81..98 y 67..76; Cut 7: 0 (Regel 3.3); Cut 0/1/11: das
  Anker-Viereck enthaelt den Ursprung nicht (Original-Cull, nichts gezeichnet).
* Aufnahme (exe, Stand (1300,-2900) Blick -z): Modal mit RE2-Bild P. Room Key / Cabin Key, Ja ->
  `PARKING LOT KEY genommen: Item 0x45, Bit (9,82)` bzw. `PRISON KEY genommen: Item 0x42, Bit (9,83)` + `Prison Key
  genommen -> Flag (4,0xEC) = 1 (Zelle ROOM1211)`; dritter Druck -> `[msgtext] room=10f0 id=4 "Several files from past
  cases.Nothing useful here."` (Bild `K_belege/k2_aufnahme_10f0_10f1_ablauf.png`).
* Parkplatz-Tuer 11B0 und Zelle 1211 unveraendert (Riegel unit_r36_schluessel_tuer/zelle/befreiung, integration
  zu/auf/check/befreiung).

#### 4.2a Fortsetzung 2026-10-16 21:20 (exe am Endstand, Messkopie `re15_pc_r38k_mess.exe` frisch kopiert)
Lade-Weg CONTINUE mit Elza-Karte (`elza` -> debug.log `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`,
`CONTINUE: resumed in room 10f1`), je zwei Laeufe mit/ohne (9,83), Bild 100, Werkzeug `probe_r34n_e_bild`:
* **Elza Cut 6** (RE15_FORCE_CUT=6, debug.log `[pri] cut=6 ... masks=102`): **29 Pixel, Huelle x 83..97 y 67..78**
  (auf der Pinnwand), aussen 0. **Elza Cut 7** (`[pri] cut=7 ... masks=0`): **0 Pixel** (Regel 3.3 gilt fuer 10F1
  wie fuer 10F0). Bild `K_belege/k2_10f1_elza_cut6_cut7.png`.
* **Skala 3** (RE15_WINDOW_SCALE=3, `[framedump] 960x720`; so oeffnet das Fenster auf einem 1080p-Bildschirm:
  render_pc.c nimmt die groesste ganze Skala, deren Fenster in 90 % der Arbeitsflaeche passt; 3D-Geometrie wird in
  Fensteraufloesung gezeichnet), Fenster = B3-Fenster x 3 (x 393..437 y 225..263):
  | Schluessel | Pixel | Huelle | aussen | mittlere Farbe mit / ohne |
  |---|---|---|---|---|
  | Prison Key (10F1, Elza) | **112** | x 402..431 y 234..257 (30 x 24) | 0 | (70,67,67) / (68,67,53) |
  | Parking Lot Key (10F0, Leon) | **491** | x 397..432 y 233..255 (36 x 23) | 0 | (80,72,72) / (71,69,55) |
  Bei Skala 1 trifft die Abtastung die 0,25..0,4 px breite Ringlinie des Prison Key nur stellenweise (B3: 14 Pixel);
  in der Spielgroesse ist sie eine zusammenhaengende Linie (112 Pixel ~ Umfang 85 + Bart). Die Ringlinie ist im
  RE2-Asset dunkles Eisen (Farben der sichtbaren Texel, aus dem Texturbild des Werkzeugs gezaehlt: (24,16,16) ..
  (115,115,115), am haeufigsten (74,74,74) / (49,41,41) / (115,115,115)): sie hebt sich vom Kork ueber Farbton und
  Glanzpunkte ab, nicht ueber die Helligkeit. Bild `K_belege/k2_nutzerbild_vs_port_skala3_10f0_10f1.png` (links
  die Nutzer-Marke x3, Mitte Leon, rechts Elza, rote Linie = Marke).

## 5. Tests

### 5.1 Neu (registriert nur in `tests/unit/probes/r38_itemorte.cmake`)
* `unit_r38_itemorte_lage` — das GEZEICHNETE Netz im Nutzer-Cut (Engine-Kamera re15_camera_build_view): Loescher
  ROOM1030 Cut 0 Mitte (43,6 ; 117,0) auf der Markenmitte +-1 px, Spalten 40,9..46,3 in der Marke, Rueckseite x -2703 =
  15 vor der Pfeilerflaeche, z -18074..-17761 auf der -x-Flaeche, rot_y 3072, Attribut 0; Schluessel 10F0/10F1 Cut 5:
  jede sichtbare Texel-Abtastung (CLUT != 0, Vierecke wie der Port (0,1,3)+(0,3,2)) in der Marke (0 ausserhalb von 2221
  bzw. 355), Huelle 11,8 bzw. 10,4 von 11 Spalten, Attribut 0x0010, Massstab > 1.
* `unit_r38_itemorte_raum` — Raumstart 1030 (Prop, Zone Slot 14, Monitor-Satz in 20 mit Original-Rechteck), 1031/1050
  ohne Loescher, genommen -> Slot 14 Original; 10F0/10F1 (Prop an der Pinnwand mit Drehung, Zone Slot 8, Satz msg 4 in
  15), genommen -> Slot 8 Original; zweiter Installer-Lauf aendert nichts.
* `unit_r38_itemorte_aufnahme` — echter Druck: 1030 ausserhalb der Zone -> msg 14 aus Slot 20; vor dem Pfeiler -> Modal
  0x31 -> Ja -> (9,0x86), (3,0x85), Inventar, Modell weg; danach an derselben Stelle msg 14 (sce-0-Zone uebersprungen,
  @0x80042f50); 10F0/10F1 genauso mit msg 4 / Slot 15, (4,0xEC) nur beim Prison Key.
* `unit_r38_itemorte_sicht` — Regel-Tabelle + Mechanik: Raumstart mit (4,0x0F) -> Cut_replace 3->10, Anker-Viereck von
  Cut 10 enthaelt den Loescher, Pfeiler-Maske ueber dem Bildpunkt verdeckt bei vz 8297 NICHT, Sehstrahl Kamera 3 geht
  durch den Pfeiler-Quader; 10F0 Cut 7: Anker enthaelt den Schluessel, 0 Masken; Cut 5 Anker enthaelt ihn.
  `test_r38_itemorte` ohne Argument: **50 Pruefungen, 0 Fehler**.
* `integration_r38_itemorte` — ECHTE exe (Lade-Weg CONTINUE): B1/B2/B3 Bilder mit/ohne Bit im Nutzer-Cut (59 / 55 / 14
  Pixel im Fenster, 0 aussen), A/K1/K2 Aufnahme + verschobener Satz danach, V1/V2 0 Pixel (1030 Cut 10 nach Tausch,
  10F0 Cut 7), V3 108 Pixel (10F0 Cut 6), E Elza 1031 ohne / 1091 mit leerem Loescher, O 1050 ohne. 326 s.
  Fortsetzung (8544862d): **B4** Prison Key bei RE15_WINDOW_SCALE=3 >= 90 Pixel im Fenster x3 (gemessen 112), aussen 0;
  **V4** Elza 10F1 Cut 7 0 Pixel; **V5** Elza 10F1 Cut 6 >= 20 Pixel (gemessen 29), aussen 0 — sechs Laeufe mehr.

### 5.2 Angepasst (gewollte Verhaltensaenderung = NUTZER-VORGABE, nur die Orte)
* `test_r37_feuerloescher.c` Teile modell/raum/aufnahme: 1050 -> 1030 (Lage, Slot 14, Monitor-Satz in 20, 1031/1050/
  1051 ohne Loescher, Druck (-4300,-17904) Blick +x). Alle anderen Teile (loeschen/elza/spind/spielstand/leer/kiste)
  unveraendert gruen.
* `test_r37_feuerloescher.cmake` Laeufe A (Aufnahme jetzt in 1030), B (`pruef_nicht "[feuerloescher] ROOM1050"`),
  E (Bild jetzt 1030 Cut 0); C/D/F..O unveraendert.
* `test_r36_schluessel.c` Teil raum: Stand (1300,-2900) Blick -z, Prop an der Pinnwand mit Drehung + Attribut 0x0010,
  Zone Slot 8 x 798..1798 z -3600..-2600, kein Slot 0..7 schneidet, msg 4 in 15. `test_r36_schluessel.cmake` Laeufe
  leon/elza: Stand (1300,-2900) rot 1024. Tuer/Zelle/Namen/Befreiung unveraendert.

### 5.3 Gezielter Lauf (KEINE volle Suite; -j 3, --timeout 600), -R woertlich:
`^(unit_r38_itemorte|integration_r38_itemorte|unit_r37_feuerloescher|integration_r37_feuerloescher|unit_r36_schluessel|integration_r36_schluessel|unit_r37_kistenregel|integration_r37_kistenregel|unit_r36_kartenitems|integration_r36_kartenitems_bild|unit_r35_cut10f0|integration_r35_cut10f0|unit_r36_szenenleon_abgang10f0|unit_r36_klettern_10f0|unit_r22_10f0_figur|unit_1030_|unit_cam_1030_reentry|unit_r36_gegnerzahl_raum103|unit_r35_cut1150_montage_1030|unit_r34n_e_dokumente|integration_r34n_e_dokumente_bild|unit_aot_sce_census|integration_flag_census|integration_item_name_census|unit_r35_inhalt|unit_r35_inventar1050|integration_r35_inventar1050|unit_r34n_d_adaruf|unit_ada_eintritt_1050|unit_r27_ada_cutscene_1050|unit_r19_marke3_1050_c1_tiefe|unit_room1050_sicherung|unit_r37_sperre1050|integration_r37_sperre1050|unit_r36_story|integration_r36_story_leon|integration_r36_story_elza|unit_r21_discard_wegwerfen|unit_inv_|unit_item_|unit_itembox|unit_room1150_itembox|unit_r26_inventar|integration_r36_inventar|unit_r30_sicherung|integration_r30_sicherung)`
-> **`100% tests passed, 0 tests failed out of 112`** (1110,74 s). Darin alle Gruppen der beruehrten Module
(feuerloescher_r37.c, schluessel_r36.c, main.c-Prop-Zeichner) und Raeume (1030/1031, 1050/1051, 10F0/10F1, 1090/1091,
11B0-Tuer, 1211-Zelle, Kistenregel K1/K11, Lageplan 1030, Szene 10F0, Montage 1030, Story-Laeufe Leon/Elza).
Vorher einzeln: `^(unit_r38_itemorte|unit_r37_feuerloescher|unit_r36_schluessel)` -> `100% tests passed, 0 tests failed
out of 18`; `^integration_r38_itemorte$` -> `100% tests passed, 0 tests failed out of 1` (258 s).

## 6. OFFEN
* Nichts Funktionales. Ehrliche Grenzen:
  - Prison Key = RE2 "Cabin Key" (Runde-36-Modellwahl): ein grosser DUENNER Ring (Linie 1..1,5 Texel) mit kleinem
    Schluessel. Er fuellt die Marke wie der Parking Lot Key, im 320x240-Bild aendern sich aber nur 14 Pixel (Parking Lot
    Key 55); in der Fensteraufloesung (Skala 3) ist er klar als Ring mit Schluessel zu erkennen
    (`K_belege/k2_10f0_10f1_fensterskala3.png`; gezaehlt 4.2a: 112 Pixel zusammenhaengende Ringlinie, Parking Lot Key
    491; Riegel B4). Ein kraeftigerer Prison Key braeuchte ein anderes Modell (Runde-36-Wahl, nicht der Ort): groesser
    geht nicht, ohne die Marke zu verlassen (Huelle 10,5 x 8,4 px in der 11 x 9-Marke), eine dickere Linie waere ein
    veraendertes RE2-Asset.
  - Die Schluessel sind groesser als echte Schluessel (Parking Lot Key sichtbar 735 x 468 Einheiten = 43 x 28 cm, Prison
    Key 32 x 37 cm): "gut sichtbar" aus Cut 5 (Kamera ~7,6 m entfernt) — in RE2-Groesse waeren es 2..3 Bildpunkte.
  - Sichtbarkeitsregel ist ein PORT-ZUSATZ (das Original hatte an beiden Stellen kein Objekt, die Masken
    shared_assets/PSX/MASKS sind TABU); der PSX-Port zeichnet diese Port-Gegenstaende nicht (platform/psx/main.c:
    nur obj < 6 mit RDT-Modell), braucht die Regel also nicht.
* Fuer die Integration: neue Quelldatei `engine/src/itemorte_r38.c` (GLOB) -> Android-Bau neu konfigurieren (Memory
  reai-v2-android-glob-cache). Merge-Stellen: `platform/pc/main.c` (Include neben re15_feuerloescher_r37.h, 3 Zeilen nach
  `re15_prop_culled` im Prop-Zeichner), `schluessel_r36.c` (schluessel_anlegen + md1_bytes; Spur G koennte
  `re15_schluessel_r36_umleiten` (Roy, Ereignis 6->7) anfassen — andere Funktion), `feuerloescher_r37.c` (Lage-Zeile +
  1 Haken). Kein k_port[]-Eintrag (keine neuen Bank-9-Bits), keine Nachrichten-Ids.
* Spur G (Roys Tagebuch, Kartenmarke Communications Room) liest das Aufnahme-Bit `RE15_R36Q_PRISON_BIT` (9,83):
  unveraendert (include/re15_schluessel_r36.h:67, nur Lage/Drehung/Slot/Licht sind neu; Riegel K2 `PRISON KEY genommen:
  Item 0x42, Bit (9,83)`). Der Prison Key liegt weiter im Communications Room (VERTRAG 1.5, passt zum Tagebuchtext
  "locked it up in the Communications Room").
* Kommentar-Drift in der gemeinsamen Datei `scd_room_setup.c` (Z. 43 und 466: "Feuerloescher 1050/1090/1091")
  absichtlich nicht angefasst (nur Kommentar; gemeinsame Datei, Merge-Risiko ohne Nutzen). Der K1-Kommentar in
  `kistenregel_r37.c` ("Port: Aufnahme in 1050") ist nachgezogen (8544862d, nur Kommentar).
* Volle Suite NICHT gefahren (Spurbaum-Regel); gezielt 112/112 (5.3).

## 7. Fuer den Nutzer

### Was sich aendert
* **Feuerloescher (Leon):** haengt jetzt in der Lobby (ROOM1030) am Pfeiler unter dem Notruf-Monitor, genau an der Stelle
  aus fire_extinguisher.bmp (Bild `K_belege/k1_nutzerbild_vs_port_1030_cut0.png`: links dein Bild, rechts das Spiel).
  Aufheben: vor den Tischen links neben dem Pfeiler stehen, Blick zum Pfeiler, Aktionstaste -> "Will you take the Fire
  Extinguisher?" -> Ja. Danach ist der Pfeiler leer; der Monitor antwortet wie immer ("Seems like even the emergency
  broadcast has gone offline..."). In ROOM1050 haengt keiner mehr. Elza findet wie bisher keinen (Leon war zuerst da),
  in ROOM1091 liegt fuer sie weiter der leere am Boden. Loeschen in ROOM1090 unveraendert.
* **Parking Lot Key (Leon) / Prison Key (Elza):** haengen an der Pinnwand im Communications Room (ROOM10F0/10F1) unten
  rechts, wie in key_location.bmp (Bild `K_belege/k2_nutzerbild_vs_port_10f0_cut5.png`), so gross, dass sie deine Marke
  ausfuellen, und ohne Raumlicht gezeichnet wie in Resident Evil 2 (dort stehen beide Schluessel mit dem Flach-Attribut):
  der Parking Lot Key zeigt die silberne Klinge und den schwarzen Kopf mit rotem Emblem, der Prison Key den Ring mit dem
  kleinen Schluessel. Aufheben: vor die Pinnwand stellen, Blick zur Wand, Aktionstaste. Die Konsole ist leer.
* Neue Assets fuer das Paket-Gate: **keine** (alles in die exe gebacken: gen/itemorte_r38_*.inc). Keine Sprachdateien,
  keine neuen Texte.

### Hinweis
* Der Prison Key ist RE2s "Cabin Key" (duenner Ring): er ist bei normaler Fenstergroesse gut zu erkennen, wirkt aber
  feiner als der Parking Lot Key (`K_belege/k2_10f0_10f1_fensterskala3.png`, `k2_nutzerbild_vs_port_skala3_10f0_10f1.png`:
  links deine Marke, Mitte Leon, rechts Elza).

## 8. Wortlaut-Pruefung (Fortsetzung 2026-10-16)

| Wortlaut | erfuellt durch | Beleg |
|---|---|---|
| "fire extinguisher ... in room 1030" | Prop obj 5 + Aufhebe-Zone Slot 14 in ROOM1030 (Leon) | 4.1, B1/A, unit_r38_itemorte_raum/aufnahme |
| "haengen" | an der -x-Flaeche des Pfeilers, Rueckseite 15 vor der Flaeche, Unterkante y -2122 ueber dem Boden (wie Runde 37 an der Wand in 1050) | 2.2/3.2, unit_r38_itemorte_lage |
| "an der stelle von fire_extinguisher.bmp" | Nutzer-Cut = ROOM1030 Cut 0 (Bildvergleich gegen 428 Hintergruende); gezeichnete Mitte (43,6 ; 117,0) auf der Markenmitte (43,5 ; 117,0); 50 von 59 Pixeln in der Marke, aussen 0; beim Aufheben ist Cut 0 aktiv | 1.1, 2.5, 4.1, B1 |
| "anstatt in room 1050" | ROOM1050 ohne Prop/Zone, Spind 1000 unveraendert Raumtext | O, unit raum |
| (Elza) | der Wand-Loescher war in Runde 37 nur Leons (1051 ohne; Elza findet den leeren am Boden in 1091 — Nutzer-Wortlaut Runde 37 Nachtrag 4) -> 1031 ohne, 1091 unveraendert | E, unit raum |
| "Parking Lot key, und den Prison Key - je nach Szenario" | Leon ROOM10F0 Item 0x45 Bit (9,82), Elza ROOM10F1 Item 0x42 Bit (9,83) + Zellen-Flag (4,0xEC) | B2/B3/K1/K2 |
| "an die Location ... key_location.bmp" | ROOM10F0/10F1 Cut 5 (Bildvergleich 4,34 gegen >= 90,9), Pinnwand unten rechts, jeder sichtbare Texel in der Marke | 1.1, 2.3, unit_r38_itemorte_lage |
| "gut sichtbar" | fuellt die Marke (Huelle 11,8 / 10,4 von 11 Spalten), Licht flach wie RE2 (@0x80055320 Attribut 0x0010), in Cut 5 UND Cut 6 beider Szenarien gezeichnet; Skala 3: Prison Key 112, Parking Lot Key 491 Pixel; in Cut 7 (Regal davor, keine Masken) bewusst nicht | 4.2/4.2a, B2/B3/B4/V3/V5, V2/V4 |

Offen gegen den Wortlaut bleibt nur die Grenze des Prison-Key-Modells (Abschnitt 6): die Ringlinie ist duenn und
dunkel, weil sie es im RE2-Asset ist.
