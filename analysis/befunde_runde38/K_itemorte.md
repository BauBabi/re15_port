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
- 17:15 Messung vorher, Cut-Zuordnung, Geometrie (Pfeiler/Pinnwand), RE-Belege fertig (Abschnitte 1/2); Werkzeug
  `re15_port/tools/r38_itemorte/itemorte_export.py` rechnet Lage, Drehung, Massstab.
- 17:40 Umsetzung + Messung nachher (Abschnitte 3/4), RE2-Licht der Schluessel belegt (2.7), Riegel gebaut (5).

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
  Cut 9 (= Kamera 0 nach dem Tausch): untere ~3 Zeilen unter der groben Tischmaske 3 (x 0..47, y 121..136).
* ROOM10F0, Schluessel-Mitte (1348,-2494,-3390): Cut 5 (138,7 ; 81,5) frei; Cut 6 (90,6 ; 72,8) frei, auf der
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

## 5. Tests

(folgt)

## 6. OFFEN

(folgt)

## 7. Fuer den Nutzer

(folgt)
