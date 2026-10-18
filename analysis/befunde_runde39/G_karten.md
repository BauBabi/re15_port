# Runde 39 Spur G "karten" — Dossier (fortlaufend)

Baum: .claude/worktrees/r39_karten (Zweig r39/karten, Basis 1038dc15). Start 2026-10-18.

## Punkte (Wortlaut AUFTRAG.md)
1. "Bei den Maps des Police Departement passt es noch nicht so ganz. [...] map3F_correct.png, map2F_correct.png,
   map1F_correct.png, mapB2_correct.png. Dort ist immer links, wie es bei uns aussieht und rechts wie es eigentlich
   aussehen sollte. Keine Doppelwände, Tür immer genau auf Wand platziert, treppen nicht in der Wand platziert usw. Und
   natürlich muss der Spielermarker da entsprechend passen. Der Room B1 den habe ich nicht gemacht. [...] Die Karte von B1
   ist noch ziemlicher schrott."
2. "In ROOM 1090 ist Leons Positionsmarker der Map außerhalb des aktuellen Raums."

## Fortschritt (gegen die Commits abgeglichen, Fortsetzung 2026-10-18 ~17:10)
- [x] Bestand lesen (Karten-Code, Memory, Runde 36/33/27-28) - 997a2cec
- [x] Messung vorher 1F/2F/3F/B2 (M1/M2: Vorlagen = 3x-Abzuege unseres Stands, Soll-Unterschiede je Blatt) - 97dca68e
- [x] Umsetzung 1F/2F/3F/B2: Kachel-Korrekturen engine/src/karte_r39.c + Marken re15_map_zones.h (4 Haken
      re15_inv_screen.c) - ba258ae4 (vom Kompositor gegen das Soll gerechnet, NICHT gebaut/gemessen)
- [x] Reste des Limit-Abbruchs 716eda1d: test_r35_karte.c/.cmake (1210-Tuer 11E0 x187->x186) + test_r39_karten.c
      (Sonde marker) - die Sonde kompilierte NICHT (Zeilenumbrueche in drei String-Literalen), Fortsetzung
      behoben; Bau (configure+build) gruen
- [x] Messung nachher 1F/2F/3F/B2 an der ECHTEN exe (Sweep, 0 Abweichungen gegen das Soll) - Abschnitt N1
- [x] Punkt 2 ROOM1090: gemessen (160/1225 Rasterpunkte ausserhalb), Zeilen je Ebene aus den Waenden, nachher 0 - P2
- [x] B1 neu als Schema-Zeichnung aus Kollision + Tueren (B1-M1/R/U/N) - 99a166ca, 7eb1f744
- [x] Marker-Pruefung ueber alle Raeume der Stage 1 (P1-M), Zeilen 10C0/1120/10E0 - 9ed100c3, 64ee6338
- [x] Riegel unit_r39_karten_1090 / _b1, integration_r39_karten; alte B1-Riegel umgestellt (Abschnitt Tests) - 4dda19b4
- [x] Messung vorher B1 "wie man sich bewegt" (Bau mit dem Kopf von 10876ebb), Echtlauf-Abzug B1 - 6d23aa53
- [x] Gezielter Lauf 66/66 gruen, Dossier abgeschlossen

## Messung vorher

### M1 Die Nutzer-Vorlagen sind pixelgenaue 3x-Abzuege UNSERES Stands (gemessen, 2026-10-18)
Werkzeug `G_karten_werkzeug/` (halves.py, phase.py, reg.py, soll.py; Lauf `lauf.sh` = exe-Kopie + Spielstand aus
`probe_r35_karte karte` + `RE15_MAP_SHOT_SWEEP`, `RE15_WINDOW_SCALE=1`, Stand 1038dc15 im Baum gebaut):
* Jede Vorlage = zwei gleich grosse Haelften (links 0..954/959, rechts ab 1045..1164), Versatz links<->rechts (0,0),
  links JEDER 3x3-Block einfarbig (Gleichfoermigkeit 1,000) -> reine 3x-Nearest-Neighbour-Abzuege.
* Linke Haelfte, auf 320x240 zurueckgerechnet, gegen den Sweep-Abzug des Baums (Startraum ROOM1190):
  1F 100,0 % / B2 100,0 % / 3F 99,999 % / 2F 98,8 % pixelgleich (2F: der Nutzer stand in ROOM1090 oben, aktuelle
  Flaeche (48,8,48) - mit der Op-Liste aus ROOM1090 Band 6 ebenfalls deckungsgleich bis auf den Marker-Ring).
  => links = exakt unser Stand 1038dc15; rechts = Soll (vom Nutzer bearbeitet, 238 Farben -> auf die 36 Farben der
  linken Haelfte geschnappt, max. Farbabstand 8,4).
* Soll-/Ist-Bilder 320x240: `G_karten_bilder/{ist,soll}_map{1F,2F,3F,B2}.png`.

### M2 Was der Nutzer geaendert hat (Klassenbild: P Panel, B Raumkoerper, W Wand, X Koerper ueber Wand, D Tuer, S Treppe)
Kompositor `komp.py`/`modell.py` (rastert die echte Op-Liste von `probe_r39_karten ops` mit MAP0x.PIX + CLUT-Zeile 21,
STP-Mittelung wie der Rasterer; Schwenk-Tilgung des Renderers datengetrieben aus dem Ist-Bild) bildet das Ist nach:
3F 0 / B2 2 / 1F 1 / 2F 14 (Marker-Ring) abweichende Pixel. Unterschiede Ist -> Soll (`komps.py`, Klassen):
| Blatt | Pixel | Inhalt |
|---|---|---|
| 1F (Blatt 2) | 322 | Doppelwaende weg: x164 + x179 (Kacheln r7/r8 zwischen r1-Ostwand x163 und r0/r6-Westwand x180), x144 (r1), x196 (r3), x118 (r2/r9 gegen r10-Westwand x119), x187 y145..154 (r0 gegen r4-Ostwand x186), Loch (185,145) gefuellt; ROOM1000-Schemakaesten: Westkanten x207 (gegen Flur-Ostwand x206), Oberkante y89 (gegen y88), Nord-WC-Unterkante y138 (gegen y139); Tuer (120,149..153) -> x119; Treppen (127,144) -> (122,144), (135,146) -> (133,146) |
| 2F (Blatt 3) | 53 | Tuer (188,116..120) -> x186 (auf die Wand), (186,121) X -> W; Treppen (126,144) -> (121,144), (134,146) -> (132,146) |
| 3F (Blatt 4) | 98 | X-Streifen x144 y81..118 -> B (r4-Westwand unter r2-Koerper), x152/x153 y119..157 (Wand nach x153, X weg); Tuer (166..170,89) -> (164..168,89); Treppe senkrecht (168,106) -> (168,107) |
| B2 (Blatt 1) | 93 | Doppelwand x187 y79..112 weg (x186 bleibt); Tuer (187,72..76) -> x186; Tuer (163..167,139) -> y138; Treppen (177,115) -> (173,115), (186,112) -> (183,112) |


### N1 Messung nachher 1F/2F/3F/B2 (echte exe, Stand f6ef49cf + Bau 17:20)
`G_karten_werkzeug/lauf2.sh <ordner> 1190 300 1190 3174 -11182 0 -- RE15_MAP_SHOT_SWEEP=s` (Spielstand per
probe_r35_karte karte, exe-Kopie re15_pc_g39f_<id>.exe, RE15_WINDOW_SCALE=1, alle 13 Blaetter in einem Lauf), Vergleich
`vgl_voll.py <ordner>` (Klassenbild ganzer Schirm, aktuell = Koerper, Spieler-Ring ausgenommen):
```
map1F Abweichungen: 0   map2F Abweichungen: 0   map3F Abweichungen: 0   mapB2 Abweichungen: 0   GESAMT 0
```
Rohfarben: 1F/B2 0 Pixel, 3F 1 Pixel (Rahmen (0,238)), 2F nur der Zustand (Nutzer stand in ROOM1090 = aktuell rot)
und sein Marker-Ring (184..188,81..85). Die vier Blaetter der echten exe SIND die rechten Haelften der Vorlagen.

### P2 Punkt 2 ROOM1090 - Messung vorher
Nutzerstandort aus seinem befund.log (nutzer/befund_marken_2026-10-17.txt Zeile 125ff, F1126..F1351):
R1090 pos=(-12528,-9000,959) = obere Ebene (Band 5). Sonde `probe_r39_karten marker 1090 -12528 959 5 -9000`:
`Blatt 3, Zone Blatt 3 rect 7 zid 22, Marker (187,84), Kachel-Index am Marker 0` - der Ring (186,83) steht im
gruenen Nachbarraum neben dem roten Gamma, wie in der linken Haelfte von map2F_correct.png.
NEU `probe_r39_karten markerflaeche <raum> <band> <x> <z> <y>`: begehbare Flaeche per ECHTEM Kollisionsloeser
(re15_collision_constrain_contact_band, Port FUN_8003b0a4 mit Rechteck/Kreis/Diagonale, Spielerradius 450) ab dem
Ankunftspunkt, jeder 3. Rasterpunkt (300 Einheiten) durch den echten Kartenpfad, gezaehlt: Marker NICHT auf der
Zeichnung der eigenen Zone (Kachel-Index 0 bzw. ausserhalb der Schema-Zellen).
```
ROOM1090 Band 5 Saat (-13600,1300) y -9000: 1225 geprueft, Marker x 184..213 y 80..110, ausserhalb 160
ROOM1090 Band 1 Saat (-10000,6000) y -1800:  590 geprueft, Marker x 186..215 y 63..75,  ausserhalb 0
(ROOM1091 Elza: dieselben Zahlen)
```
URSACHE: beide Zeilen (Blatt 2 Hof, Blatt 3 Gast-Zeile obere Ebene) strecken die Huelle BEIDER Ebenen
(x -17500..10285, z -18700..11754) auf ihr Rechteck. Die obere Ebene (begehbar x -14600..2700, z -16392..3808)
landet damit auf x 184..214 / y 78..110 - das Querband (z 508..3808) auf y 78..84 statt auf die gemalten y 68..75,
also unter das gemalte Band in die durchsichtige Ecke der Kachel. Der Hof liegt gestaucht, aber innen.

### P2 RE-Belege
* Original-Zeile ROOM1090 = Index 9: PSX.EXE @0x800768F8 = `00 00 00 00 01 00 01 00` (Stub, jede Lage -> (0,0));
  der Stage-1-Seiten-Setzer kennt fuer 1090 nur Blatt 2 (Runde 10/30). Das Original hat hier KEINEN brauchbaren
  Marker - die Zeile ist eine PORT-ERGAENZUNG, die Form ist die des Originals:
* Formel FUN_800473f8 (re15_disasm.py dis 0x8004741c): `addiu v0,v0,32000` @0x80047428, x10 per `sll/addu/sll`
  @0x8004742c-48, `lhu v0,0(at)` sx @0x80047444 (0x800768b4+8i), `sra t0,t0,20` @0x8004745c, `addiu t0,t0,5`
  @0x80047460, /10 per 0x66666667 @0x80047450-54, `lh v0` ox @0x800474e8; z: `lhu a0` sy @0x8004747c,
  `subu v1,zero,v1` @0x800474b0 (negiert), `lhu v1` oy @0x80047518. Der Port rechnet sie in re15_map_zone_marker
  (Zonen-Zeile ox/oy/sx/sy).
* Kalibrierung je Ebene: jede Wandflaeche der begehbaren Flaeche (SCA-Zelle des Bands, Datei-Offset ROOM1090.RDT,
  `sca_liste.py 1090 <band>`) auf ihre gemalte Wandlinie (Kachel-Index 4, `kachel.py`), kleinste Quadrate
  (`zeile_fit.py`), ganzzahlig mit der exakten Formel nachgerechnet:
  - Hof Band 1, Blatt 2 rect 5 (180,59,48,32) MAP03.PIX uv(112,64): x -14546 @0x01E84 -> 180, -4414 @0x01EC0 -> 203,
    3972 @0x01E00 (Typ 4) -> 222 (Reste 0,0,0); z 7720 @0x01DF4 -> 59, 3800 @0x01E30 -> 69, -4214 @0x01F5C -> 88
    (Reste 0,-1,0) => ox 140, oy 155, sx 2376, sy 2535.
  - obere Ebene Band 5, Blatt 3 rect 7 (179,67,48,48) MAP04.PIX uv(112,74): x -14672 @0x01E54 -> 179, -6878 @0x01FBC
    -> 197, -4814 @0x01E48 -> 204, 2752 @0x01F38 -> 222 (Reste 0,1,-1,0); z 3404 @0x01E6C -> 67, 491 @0x01FC8 -> 76,
    -5276 @0x01F2C -> 87, -16403 @0x01F20 -> 114 (Reste 1,-1,1,0) => ox 136, oy 150, sx 2597, sy 2435.
  Acht unabhaengige Waende je Ebene treffen ihre Linie auf hoechstens 1 px - die Kachel ist massstabsgerecht
  gemalt, nur die Zeile fehlte. Probe Hof: Tuer -> ROOM1050 (Mitte (-10080,3060), ROOM1090.RDT Tuer slot 0) faellt auf
  (190,70) = das gemalte Tuerblatt (187..195,64..68) an der Marke (191,69).

### P2 Umsetzung
re15_port/engine/src/re15_map_zones.h: Zeilen 0x1090/0x1091 Blatt 2 (ox 140, oy 155, sx 2376, sy 2535) und die
Gast-Zeilen Blatt 3 (ox 136, oy 150, sx 2597, sy 2435), Kommentar "Runde 39 Spur G" mit allen Offsets.
Weltbox unveraendert (Zonen-Zugehoerigkeit: der Raum hat EINE Zone).

### P2 Messung nachher (Sonde, Bau 17:35)
```
ROOM1090 Band 5: 1225 geprueft, Marker x 183..221 y 71..111, ausserhalb 0     (vorher 160)
ROOM1090 Band 1:  590 geprueft, Marker x 184..219 y 63..86,  ausserhalb 0
ROOM1091 (Elza): dieselben Zahlen
Nutzerpunkt (-12528,-9000,959): Blatt 3 rect 7, Marker (184,73), Kachel-Index 1 (Querband)
```

## B1 (Punkt 1, zweiter Teil) - "Die Karte von B1 ist noch ziemlicher schrott"

### B1-M1 Messung vorher (Stand 10876ebb)
* Abzug Blatt 0 (Sweep, alles aufgedeckt): `G_karten_bilder/b1_vorher_sweep_x2.png` - die gemalte Original-Kunst
  MAP01.PIX (Liste @0x800762a0, 7 Rechtecke): der Gang als RING um die Ostkammer, die anderen Orte als Kaesten.
* Die Zeichnung ist ein SCHEMA, der Marker wurde ihr nachgebogen: der Gang 1180/1230 (Original-Zeile @0x80076970 /
  @0x800769c8 = Stub 00 00 00 00 01 00 01 00) lief in FUENF Zonen, je auf einen gemalten Streifen gestreckt
  (Runde 35: sx/sy z.B. 4950/547 im Suedbein, 1915/408 im Nordteil = 1 px je 212 bzw. 2570 Einheiten - quer zur
  Laufrichtung fast 10x schneller als laengs). 11C0 (ein RING um eine Insel, s.u.) als Rechteck mit der Zeile von
  11B0; 11D0 als Kasten, dessen Streckungs-Box von Hand an die gemalten Waende gelegt war.
* ROOM1160 (der Zwinger OHNE Strom - ROOM1180 main00 `21 04 f3 00` Ck(4,243) @0x009EA waehlt fuer Tuer-Slot 0
  Ziel 0x16 = 1160 @0x009EE, sonst 0x1d = 11D0 @0x00A12) hatte KEINE Zonenzeile: `markerflaeche 1160` 588/588
  Rasterpunkte ohne Zeichnung, kein Marker, solange der Strom aus ist.
* Wie man sich bewegt (Grundriss aus der Kollision, b1_layout.py, Bild `G_karten_bilder/b1_lageplan_welt.png`):
  der Gang ist ein S (Nordarm, Querarm mit Westbein zur Halle, Laengsgang an der Ostseite, Versatz, Suedbein zum
  Zwinger), die Ostkammer liegt IN der Schleife, 11B0 und 11C0 beide oestlich an der Gang-Ostwand, 11C0 ist ein
  Fahrring um eine Insel, 11D0 ein U mit eigener, nur aus ROOM1260 erreichbarer Leiter-Kammer.

* GEMESSEN "wie man sich bewegt" (b1_weg.py: geradlinig durch die Gang-Abschnitte je 100 Einheiten durch den echten
  Kartenpfad; Tuer = Marker im Trigger der einen Seite gegen Marker an der Ankunft der anderen; frische Prozesse).
  VORHER = Bau mit dem Zonen-Kopf von 10876ebb (build_bisect), NACHHER = dieser Stand:
```
Weg (Gang 1180)        Welt    vorher px/1000  quer  max.Schritt  Zonen     nachher px/1000  quer  max  Zonen
Suedbein n. Norden     8100        0.62         0      1         [108]          2.22          0    1   [25]
Versatz n. Osten      10100        3.56         8     14         [107,108]      2.28          0    1   [25]
Laengsgang n. Norden  34300        1.14         0      1         [25,109]       2.22          0    1   [25]
Querarm n. Westen     14000        2.00         7     13         [25,106]       2.29          0    1   [25]
Westbein n. Sueden     3750        2.40         0      1         [106]          2.13          0    1   [25]
Tuer-Sprung (px)   Gang->Halle 7->3  Gang->Ostk. 10->1  Gang->11B0 25->5  Gang->10A0 12->2  Gang->11D0 8->3
                   11B0->11C0 1->1   Halle->Gang 8->2   11C0->11B0 6->5
```
  Vorher lief der Marker im Suedbein 0,6 px je 1000 Einheiten, im Versatz 3,6 - also 6x so schnell -, sprang beim
  Wechsel der fuenf Gang-Abschnitte bis 14 px und an der 11B0-Tuer 25 px. Genau das meint "da passt nichts so richtig
  zusammen mit dem wie man sich bewegt".

### B1-R RE-Belege / Herleitung
* Begehbare Flaeche je Ort mit dem ECHTEN Kollisionsloeser: `probe_r39_karten flut <raum> <band> <x> <z> 0` =
  re15_collision_constrain_contact_band (Port von FUN_8003b0a4 mit push_rect/push_circle/push_diag2..7), Raster 100,
  Saaten = Ankunftspunkte der Tueren. Ergebnisse (Mitten, +-50 = Wandflaeche): Gang x -9492..5508 z -16377..31023
  (Laeufe s. b1_plan.py), Halle (1190) x -23104..4596 z -26404..-10304, Ostkammer (1190) x 14096..21296
  z -25404..-11504, 11B0 West x -27100..-22200 + Werkstatt x -21400..1700 (z -26700..-15400; Trennwand @0x00E8C/
  @0x00EC8, sub00 `37 03 0c 00` @0x0118C macht @0x00EC8 nach "Reservestrom an" nicht-fest), 11C0 x -27400..12400
  z -17000..17700 mit Insel (groesstes leeres Rechteck x -11900..4700 z -7200..8300), 11D0 x -11450..4950
  z -23050..-11550, 11D0-Kammer x 1550..6050 z -23050..-19450 (Saat = 1260-Ankunft (3900,-22000)), 10A0 Band 4
  x 19850..26950 z 21650..26250 (+ Treppenlaeufe Aot_set sce 13 Band 4 @0x00E7C/@0x00E90).
* Lage je Tuerpaar (Wand auf Wand, Tuermitten laengs der Wand): 1180 slot 2 @0x00A54 <-> 1190 slot 4 @0x028B2,
  1230 slot 4 @0x00CE6 <-> 1190 slot 6 @0x02B1C, 1180 slot 3 @0x00A74 <-> 11B0 slot 0 @0x00F64, 1180 slot 1
  @0x00A34 <-> 10A0 slot 1 @0x00DDA, 1180 slot 0 @0x009EE/@0x00A12 <-> 11D0 slot 0 @0x011E2 / 1160 slot 0 @0x007C6,
  11B0 slot 1 @0x00FAA <-> 11C0 slot 0 @0x01712. Alle Paare gegenlaeufig (Spawn->Trigger), keine Drehung.
  Lagen im 1180-Rahmen: Halle (-11118,19727), Ostkammer (-19404,23627) (die beiden 1190-Bereiche liegen im eigenen
  Rahmen 8286/3900 anders als am Gang), 11B0 (32708,45764), 10A0 (-21427,-33627), 11D0=1160 (-4537,73), 11C0
  (33008,1264) - 11C0 ohne Gang-Tuer mit der Westwand auf der von 11B0 (Original: r3/r4 beide x 168 @0x800762a0).
* Massstab: RE2 Retail FUN_8006e120 `lui v0,0x91a2` @0x8006e1dc / `ori v0,v0,0xb3c5` @0x8006e1e8 / `addiu a0,a0,28000`
  @0x8006e1ec / `sra v1,v1,8` @0x8006e20c / z `subu v0,zero,v0` @0x8006e268 = (x+28000)/450 (0x91A2B3C5/2^40 = 1/450),
  EIN Massstab fuer alle Raeume (Beta -> Retail) -> Affine 65536/450 = 145,6 -> 146.
* Die beiden Wege nach ROOM1260 sind BODENLOECHER: 11B0 slot 5 @0x01010 Trigger (-895,-21121,1667,1667) = Kreiszelle
  @0x00EA4 (gleiche Masse), 11D0 slot 1 @0x01228 Trigger (1700,-22900,1800,1800) ueber Kreiszelle @0x00A10
  (1800..3300, -22750..-21250) - keine Wandtueren, also keine Wandmarke.

### B1-U Umsetzung
* `re15_map_zones.h`: Blatt-0-Zonen auf Schema-Zeichnungen (rect 255, synth 4..11): Gang 1180/1181/1230/1231 EINE
  Zone (statt fuenf), Halle/Ostkammer, 11B0, 11C0, 11D0 + NEU 1160/1161 (zid 30), NEU 11D0 Zone 1 Kammer (zid 111,
  Weltbox um ZONE_SLACK eingezogen), 10A0 Gast-Zeile; Etagenzeile 10A0 Band 4 -> (0, 255); s_map_synth Index 3..10,
  s_map_synth_cells Index 3..20 (je Ort disjunkt - die Fuellung mischt halbtransparent, Ueberdeckungen mischten
  doppelt, im ersten Abzug gemessen); 8 neue Blatt-0-Marken (6 Tueren auf der gemeinsamen Linie, 2 Treppen =
  exakte Projektionen ihrer AOT-Mitten), die 8 alten entfernt. Rechnung + Selbstprobe: b1_plan.py.
* `re15_inv_screen.c` (Schema-Durchgang, 3 Haken): (a) unbesuchte Schema-Zeichnung als UMRISS, wenn der Plan des
  Blattes gehoert (wie die Kacheln, RE2 Zeile 498 @0x8006E71C); (b) Kartenhinweis-Ziel auf einer Schema-Zeichnung
  blinkt (rote Phase = aktuell, sonst Umriss) - ROOM11C0 "PARKING LOT" nach der ROOM10F0-Szene; (c) ROT NUR FUER
  DIE ZONE, IN DER MAN STEHT (wie die Kacheln, re15_map_rect_state) und Gast-Zeichnung nur auf ihrer Etage - die
  Raum-Regel stammte aus dem Grundriss-Modus und widersprach dem Nutzer (1190 "2 separate Bereich ... sauber
  getrennt", Runde 36; ROOM1000 "es soll immer nur der markiert ... in dem man auch drin ist", 2026-09-13).
* `map_hint_common.c` (2 Haken): Zielzone darf eine Schema-Zone sein; Ziel-"Rechteck" = 128 + synth
  (RE15_KARTE_R39_SCHEMA_RECT, re15_karte_r39.h).
* `karte_r39.c`: re15_karte_r39_ziel_rect / _schema_ziel / _etage_hier.

### B1-N Messung nachher
`markerflaeche` (echter Kartenpfad an jedem 3. begehbaren Rasterpunkt, Spielerradius 450):
```
ROOM1180/1181/1230  1707/1707/1764 geprueft, ausserhalb 0      ROOM1190 Halle 3770, Ostkammer 734 (1191 dito), 0
ROOM11B0 West 406 + Werkstatt 1588, 0                         ROOM11C0/11C1 5706, 0
ROOM11D0 601, 0   ROOM1160/1161 588, 0 (vorher 588 ausserhalb) ROOM10A0/10A1 Band 4 138, 0
```
Abzug Blatt 0 (Sweep): `G_karten_bilder/b1_nachher_sweep_x3.png`; ECHTER WEG (Spielstand ROOM1230 an der 11D0-Tuer,
Gang/Halle/11B0/11D0 besucht, LOAD GAME, Kartenschirm, RE15_INV_FB_SHOT): `G_karten_bilder/b1_echtlauf_1230_x3.png` -
Gang rot, besuchte Orte gruen, Ostkammer/11C0 unbesucht ungezeichnet, Tuerbalken auf den gemeinsamen Linien, Marker an
der 11D0-Tuer. Gesamtschiene integration_map_uebergang (alle Stages, gleicher Bau gegen den Kopf von 10876ebb):
Uebergaenge 149 -> 151 (1160 neu), ueber 8 px 64 -> 62, Median/Mittel/Schlimmster 6/17/91 unveraendert; Tueren
durchschritten 184 -> 187, richtig rot 181 -> 184; Symbole Identitaet 120 -> 129, ohne Marke 49 -> 44.

## P1-M "Und natuerlich muss der Spielermarker da entsprechend passen" (1F/2F/3F/B2)
Pruefung ueber ALLE Raeume der Stage 1 (`alle_raeume.py <probe>`: Saaten = Ankunftspunkte aller Tueren, je Raum/Band
`markerflaeche`; Stand nach P2, vor B1):
```
ausserhalb der Zeichnung: 1030 63/2288  1060 4/155  10A0(B2) 8/140  10C0 123/999  10E0 26/1199  1110 8/1237
                          1120 77/1080  1170(Zone 1, 3F) 51/311  1090(oben) 160/1225 [P2]  1160 588/588 [B1]
                          alle anderen 0 (1000 1010 1020 1040 1050 1080 10A0 10D0 10F0 1100 1130 1140 1150 1180 1190
                          11B0 11D0 11E0 11F0 1200 1220 1230); 10B0/1260 ohne Blatt (nicht Polizeistation)
```
Ursache bei 10C0/1120/1060/10A0(B2): die Bbox-Streckung bildet den Raum auf das NOMINALE Rechteck ab - die Liste
@0x80076840 fuehrt Groessen auf 8 aufgerundet, gemalt ist weniger (gemessen ueber alle 40 Rechtecke der Blaetter 1..5:
rechts 0..24, unten 0..20 px frei; 2F rect 0 gemalt x 102..135 von 102..141). 1110/10E0/1170(Zone 1): die gemalte
L-Form deckt sich nicht mit der Raumform unter dem gesetzten Spiegel (Runden 30/36 von Hand gesetzt) - offen, s.u.
1030: AUSGELIEFERTE Zeile @0x800768c8 legt die Westkante 2 px links neben die gemalte Kachel.
UMGESETZT (Methode wie P2, Zeile je Raum aus Wand-Paaren, Spiegel 1,1 wie bisher): 10C0 (ox 191 oy 59 sx 2415
sy 2324), 1120 (ox 204 oy 60 sx 2277 sy 2372), 10E0 (ox 199 oy 18 sx 2468 sy 2492) - Belege (SCA-Offsets, gemalte Linien)
im Kommentar der Zeilen (re15_map_zones.h), Reste <= 1 px. GEGENPROBE an den Tueren (Trigger-Mitte mit der neuen Zeile
gegen die von frueheren Runden an die GEMALTEN Tuersymbole gesetzten Marken): 10C0->10D0 (-7600,-2250) -> (135,125),
Marke (135,126); 10C0->1060 -> (119,151), Marke (118,149); 10C0->1080 -> (114,144), Marke (113,143); 1120->1130 ->
(153,126), Marke (153,126); 1120->1080 -> (132,147), Marke (131,146); 1120->1060 -> (136,154), Marke (136,152);
10E0->10D0 (-8050,4300) -> (143,104), Marke (141,105) - alle <= 2 px: die Kunst ist massstabsgerecht, nur die
Streckung auf das nominale Rechteck war falsch.
NICHT umgesetzt (OFFEN, s.u.): 1060 (4) und 10A0 B2 (8) - Treppenhaeuser, deren Treppenmarken an der bisherigen
Abbildung haengen; 1110 (8), 1170 Zone 1 (51) - die gemalte Form passt unter dem gesetzten Spiegel nicht zur Raumform
(1110: Nordnase der Welt liegt gespiegelt rechts oben, gemalt ist links oben; 1170 Zone 1: Welt-L oben+rechts, Kachel
links+unten = 180 Grad gedreht, Spiegel 0,0) - braucht eine eigene Orientierungs-Messung; 1030 (63) - die Zeile ist
die AUSGELIEFERTE @0x800768c8, ihre Westkante liegt 2 px neben der gemalten Kachel (Original-Abweichung).

## RE-Belege
(siehe je Punkt: P2, B1-R, P1-M; 1F/2F/3F/B2 = NUTZER-VORGABE, Form = rechte Haelften der Vorlagen, M1/M2)

## Umsetzung (alle Dateien)
* `engine/src/karte_r39.c` + `include/re15_karte_r39.h` (NEU): Kachel-Korrekturen 1F/2F/3F/B2 nach den Vorlagen
  (Vorgaenger ba258ae4, 19 Texel-Rechtecke + 5 Schema-Kanten, je mit Seiten-Tabelle @0x80076840 / MAP0x.PIX), dazu
  Runde 39 B1: Hinweis-Ziel auf einer Schema-Zeichnung (re15_karte_r39_ziel_rect / _schema_ziel), Gast-Zeichnung nur
  auf der aktuellen Etage aktuell (re15_karte_r39_etage_hier).
* `engine/src/re15_map_zones.h` (Karten-Code, Spur G): Marken 1F/2F/3F/B2 (Vorgaenger), Zeilen ROOM1090/1091 je Ebene
  (P2), Blatt 0 komplett als Schema (B1-U: Zonen, s_map_synth 3..10, s_map_synth_cells 3..20, 8 Marken, Etagenzeile),
  NEU 1160/1161 und 11D0 Zone 1, Zeilen 10C0/1120/10E0 (P1-M). Jede Konstante mit Offset/Adresse im Kommentar.
* `engine/src/re15_inv_screen.c` (Haken "Runde 39 Spur G"): 4 Kachel-Haken (Vorgaenger), Schema-Kanten-Haken
  (Deklaration vor die Anweisung gezogen), Schema-Durchgang: Rot je Zone + Etage, Umriss bei Plan-Besitz,
  Hinweis-Ziel blinkt.
* `engine/src/map_hint_common.c` (Haken): Zielzone darf Schema sein, Ziel-Rechteck 128 + synth.
* Keine Assets, kein Patch an shared_assets, keine Sprachdateien.

## Messung nachher (Zusammenfassung, Einzelheiten je Punkt oben)
* 1F/2F/3F/B2: echte exe, Sweep - je Blatt 0 Abweichungen gegen das Soll-Klassenbild (56 207/56 234/56 214/56 216
  verglichene Kartenpixel), 0 Doppelwand-Pixel auf B1..3F (vorher 1F 188, B2 30) - N1, integration_r39_karten.
* ROOM1090 (Punkt 2): 0 von 1225 (oben) / 590 (Hof) Rasterpunkten neben der Kachel, Leon und Elza (vorher 160);
  echte exe am Nutzerpunkt: Ring (183,72), ringsum rot 36, gruen 0 (vorher Ring (186,83) im gruenen Nachbarraum).
* B1: alle Orte 0 Rasterpunkte neben der Zeichnung (vorher 1160 588/588); Gang gleichmaessig 2,13..2,29 px je 1000
  Einheiten (vorher 0,62..3,56), Tuer-Spruenge 1..5 px (vorher bis 25) - B1-M1/B1-N.
* Stage 1 gesamt: neben der Zeichnung 1225 -> s. P1-M (10C0 123 -> 0, 1120 77 -> 0, 10E0 26 -> 0).

## Tests
NEU (probes/r39_karten.cmake, Quelle test_r39_karten.c + r39_karte_b1.h + r39_karten_soll.inc):
* `unit_r39_karten_1090` - P2: 1090/1091 beide Ebenen, Nutzerpunkt, Tuer -> 1050 an ihrer Marke. VORHER-Stand
  (Zeilen ohne Kalibrierung): obere Ebene 160 Punkte daneben -> ROT.
* `unit_r39_karten_b1` - B1: Marker auf der eigenen Zeichnung (14 Orte inkl. Elza, 1160, Kammer, Absatz), Tueren auf
  der gemeinsamen Linie beider Zeichnungen, Treppen im Innenraum, Gang-Gleichmass, Tuer-Spruenge <= 3 px,
  Hinweis-Ziel 11C0 blinkt auf der Schema-Zeichnung. Gegen den Kopf von 10876ebb gebaut: 1160 + 5 Gang-Wege + 5 Tueren
  ROT (Tabelle B1-M1).
* `integration_r39_karten` - echte exe: Sweep B2/1F/2F/3F gegen die Soll-Haelften, Doppelwand B1..3F, ROOM1090-Marker
  im Rot. Auf die linken Haelften der Vorlagen (= Stand 1038dc15) angewandt: sollbild 93/322/53/98 Abweichungen,
  doppelwand 30/188/0/0 -> ROT.
ANGEPASSTE Riegel anderer Runden - je dieselbe ABSICHT, uebertragen von der gemalten B1-Kachel auf die
Schema-Zeichnung (die Kachel-Rechtecke 0..6 von Blatt 0 werden nicht mehr gezeichnet, Nutzer: "Die Karte von B1 ist
noch ziemlicher schrott"):
* test_r35_karte.c `teil_r1230` (Gang an 6 Punkten NUR die Gang-Zeichnung, Marker darauf; 10A0 Band 4 NUR der Absatz);
  test_r35_karte.cmake Lauf A Kasten (124,67)-(158,173) statt Ring (120,60)-(175,131); Lauf D (Vorgaenger 716eda1d):
  Kasten ab x186 statt x187 und test_r35_karte.c 1210-Tuer <= 4 px statt 3 - die Vorlage mapB2_correct.png legt die
  11E0-Tuer auf die Flur-Ostwand x186 (vorher x187 = Doppelwand daneben), die Wand-Bedingung bleibt.
* test_r36_karte.c Riegel 1-4 (Treppe 10A0 B1, Marker 11D0, 1190 Halle/Ostkammer, Tuer 11B0/11C0): Schema-Zustand und
  -Zellen statt rect_state/Texel; Marke 11D0<->Gang jetzt zid 25/30; "1260-Tuer an der Suedwand" ersetzt durch das
  Ostende des Sued-Gangs (die 1260-Wege sind Bodenloecher, B1-R) und die Ankunft aus 1260 in der Kammer (Zone 1).
* test_r36_karte.cmake Laeufe A-D: Kaesten auf die Schema-Lage (Absatz 141..158 x 153..168, Tuer 11B0/11C0 x161..165
  y94, Halle 70..131 x 116..150, Ostkammer 133..149 x 110..139).
* test_r36_kartenitems.c (Blatt 0): Plan-Besitz zeigt unbesuchte Orte als UMRISS ohne Koerper (Wandlinien mehr,
  Koerper gleich) statt Kacheln der Zeile 498.
* test_r35_cut10f0.c / test_r38_tagebuch.c: Ziel ROOM11C0 = Blatt 0 Pseudo-Rechteck 136 (128 + synth 8) statt rect 4.
* test_map_synth.c: Kartenfeld = Huelle der ORIGINAL-Rechtecke aller 13 Seiten-Tabellen x 39..293 y 44..206 statt der
  Port-Annahme 90..240 / 50..200 (B1s eigene Original-Kachel r1 begann schon bei x 76).
GEZIELTER LAUF (Bau nach 64ee6338 + Kommentar-Korrektur, 2026-10-18 ~19:05; KEINE volle Suite):
```
ctest --test-dir re15_port/build -R "^(unit_(map_|karte_|r30_karte|r30_hinweis|r33_karte|r35_karte|r35_cut10f0|r36_karte|r36_kartenbits|r36_kartenitems|r38_tagebuch|r39_karten|r39_inventar_status)|integration_(r39_karten|r35_karte|r35_karte_fahrstuhl|r36_karte|map_raum_live|map_uebergang|r36_kartenitems_bild|r38_tagebuch_karte|r39_elza_karte|r39_inventar_status))" --timeout 900
100% tests passed, 0 tests failed out of 66
```
(danach nur ein Kommentar in re15_map_zones.h berichtigt, neu gebaut, unit_r39_karten_*: 100% tests passed, 0 tests
failed out of 2)

## OFFEN
* 1210 (B2): die Suedwest-Kammer (z < -20140, x -26928..-22428) ist begehbar, aber weder im Original noch in der
  Vorlage mapB2_correct.png gemalt - dort steht der Marker neben der Kachel (207 von 1141 Rasterpunkten). Entscheidung
  des Nutzers (s.u.).
* 1170 Zone 1 (3F/Dach, 51/311), 1110 (2F, 8/1237): die gemalte L-Form passt unter dem gesetzten Spiegel nicht zur
  Raumform (P1-M) - braucht eine Orientierungs-Messung (Tuer-/Treppen-Symbole beider Blaetter), nicht geraten.
* 1030 (1F, 63/2288): AUSGELIEFERTE Zeile @0x800768c8, Westkante 2 px neben der gemalten Kachel - Original-Abweichung,
  nicht angefasst.
* 1060 (4) / 10A0 B2 (8): Treppenhaus-Kacheln kleiner als das nominale Rechteck; nicht angefasst, weil ihre
  Treppenmarken an der bisherigen Abbildung haengen.
* Membership-Rand 11D0-Kammer: die Weltbox ist um ZONE_SLACK eingezogen; in einem 100-Einheiten-Streifen an ihrer
  West-/Nordwand gilt der Zwinger als aktuell (Marker steht trotzdem richtig - beide Zonen haben dieselbe Abbildung).

## Fuer den Nutzer
* B1 ist neu gezeichnet: aus der Kollision der Raeume und ihren Tueren, im RE2-Massstab, Waende als EINE Linie, Tueren
  auf der gemeinsamen Wand, Treppen im Treppenhaus. Der Gang ist kein Ring mehr, sondern sein echter S-Verlauf; das
  Parkdeck 11C0 ist ein Ring um seine Insel; der Zwinger ohne Strom (ROOM1160) hat jetzt Karte und Marker; die
  Kanal-Leiterkammer im Zwinger (nur aus der Kanalisation erreichbar) ist ein eigener kleiner Raum. Bild:
  analysis/befunde_runde39/G_karten_bilder/b1_nachher_sweep_x3.png / b1_echtlauf_1230_x3.png.
* Die beiden Wege nach ROOM1260 (Kanaldeckel in 11B0, Leiterloch im Zwinger) sind Bodenloecher und haben deshalb KEIN
  Tuersymbol auf der Wand (vorher standen dort Tuermarken an Waenden, an denen es keine Tuer gibt).
* Rot wird jetzt nur der Teil eines Raums, in dem du stehst (1190 Halle/Ostkammer, ROOM1000 Ostraum/WCs) - wie du es
  fuer 1190 in Runde 36 und fuer ROOM1000 am 2026-09-13 beschrieben hast.
* Entscheidungsfrage: ROOM1210 hat im Suedwesten eine begehbare Kammer, die weder das Original noch deine Vorlage
  mapB2_correct.png zeichnet. Soll sie auf B2 als eigener kleiner Raum dazukommen (dann steht der Marker dort im Raum),
  oder bleibt B2 genau wie deine Vorlage?
* Keine neuen Assets, keine Sprachdateien.

## Nachbesserung 1 (nach Abnahme 0, 2026-10-18 ~20:00)
Grundlage: analysis/befunde_runde39/G_abnahme_0.md (Maengel M1..M5). Messwerkzeuge der Abnahme liegen im Scratchpad
`G39abn0/` (allein.sh, allein_ausw.py, lauf.sh, VORHER-exe vorher_build) und werden fuer "nachher" wiederverwendet; die
VORHER-Messungen der Abnahme werden NICHT wiederholt.

### Fortschritt
- [x] M3 (hoch, Regression): Doppelwand-Korrekturen nur, wenn die verbleibende Linie gezeichnet wird (Code + Echtlauf, Riegel folgt)
- [x] M4 (mittel): Treppenhaeuser 1060/10A0 - Abbildung zu den verschobenen Treppensymbolen (Zeilen aus den Waenden, Riegel folgt)
- [x] M5 (mittel): Marker neben der Zeichnung - 1030/1110/1170 Z1 Zeilen aus den Waenden, 1060/10A0 ueber M4; 1210 Nutzerfrage
- [x] M1 (gering): 3F-Treppe (168,107) auf Linie y110 -> (168,106)
- [x] M2 (gering): B1 10A0-Treppe auf der Unterkante y168 -> (145,164)
- [x] Bau, gezielte Tests, Abschluss (96/96 gruen, danach 6/6 nach dem letzten Bau)

### M3 Doppelwand-Korrektur oeffnet Raeume in Teilzustaenden (Regression)
* BISECT: die Regression liegt in diesem Zweig - `git log -- re15_port/engine/src/karte_r39.c`: ba258ae4 (14:50, "Blaetter
  1F/2F/3F/B2 nach den Nutzer-Vorlagen") fuegt die 19 Korrekturen + 5 ROOM1000-Kanten ein; 7eb1f744 aendert nur den
  B1-Teil. Stand vor der Spur (Merge-Basis 22abaed9 = VORHER-exe der Abnahme) geschlossen, 40328436 offen.
  Verursachender Commit: ba258ae4. Negativprobe (Gatter aus) reproduziert die Zahlen der Abnahme (s. Riegel).
* URSACHE (Code gelesen): `karte_r39.c` s_korr (19 Texel-Rechtecke) und s_kanten (5 Schema-Kanten von ROOM1000)
  ersetzten die eigene Wandlinie UNBEDINGT. Die Vorlagen zeigen den Zustand "alle Raeume gezeichnet"; die verbleibende
  Linie gehoert dem NACHBARN. Ist der Nachbar nicht gezeichnet (unbesucht, Plan nicht im Besitz), fehlt die Wand.
* MESSUNG VORHER: Abnahme 0 Abschnitt 6 (NICHT wiederholt): je Ort allein besucht, VORHER-exe gegen den Stand 40328436 -
  1050 8 / 1020 16 / 1040 17 / 1070 34 / 1010 Zone 0+1 je 30 / 1080 8 / ROOM1000 Ostraum 41, Nord-WC 26, Sued-WC 12 /
  1220 Zone 0+2 je 16 verlorene Wandpixel. Eigene Gegenprobe ohne Referenzbild (G39nb1/offen.py: Raumkoerper rot/gruen
  grenzt 4-nachbarlich an den Grund) auf denselben Abzuegen der Abnahme: VORHER 0 je Ort (Ausnahmen 1030 2 = das Loch
  (185,145), das die Korrektur fuellt; 1110 4 und 1210 2, s.u.), Stand 40328436 1050 1 / 1020 15 / 1040 14 / 1070 34 /
  1010 30+30 / 1080 8 / 1000 38/23/9 / 1220 16+16.
* MECHANISMUS-BELEG (welche Linie die geloeschte ersetzt): `G39nb1/nachbar.py` liest die Seiten-Tabelle @0x80076840
  (+8*Seite; Blatt 3 s_map_rectfix) und DATA/MAP0<p+1>.PIX und sucht je Korrektur das ANDERE Rechteck, das auf der
  Nachbarspalte/-zeile Kachel-Index 4 traegt:
  1F r0 x187 -> r4 (1030) x186 10/10; r1 x144 -> r3 (1070) x145 16/16; r2 x118 y126..133 -> r4 8/8, y144..154 -> r10
  (1060) 11/11; r3 x196 -> r0 (1050) x197 34/34; r7 x164 -> r1 (1020) 15/15, x179 -> r6 (10A0) 15/15; r8 x164 -> r1 15/15,
  x179 y78..87 -> r0, y88..92 -> r6; r9 x118 -> r10 8/8; 2F r5 (186,121) -> r6 (1100) dasselbe Pixel; B2 r4 x187 -> r0
  (11E0) 16/16, r5 x187 y96..100 -> r0, y101..111 -> r9 (10A0); Schema ROOM1000: x206 = 1F r0 65/65, y88 = 1F r5 (1090)
  15/15, y139 = Sued-WC-Zeichnung (synth 3).
  IMMER (kein Wandverlust moeglich): 1F r4 (185,145) fuellt ein Loch; 3F r4 verschiebt seine Linie INNERHALB desselben
  Rechtecks (x144 -> x145, x152 -> x153); B2 r3 x187 y72..76 - dort steht der Tuerbalken 11E0<->1210 auf x186, der mit
  ROOM1210 gezeichnet wird (Abzug 1210 allein: D auf x186 y72..76).
* AENDERUNG: `karte_r39.c` - je Korrektur und je Schema-Kante ein Feld `traeger` (Rechteck derselben Seite bzw.
  R39_SCHEMA(synth)); die Korrektur gilt nur, wenn der Traeger in DIESEM Kartenschirm gezeichnet wird. Das Gatter
  (kachel_gezeichnet / schema_gezeichnet) ist dasselbe wie das der Zeichner in re15_inv_screen.c build_status:
  Hinweis-/Zielkachel immer (RE2 FUN_8006F1C4 @0x8006F4E8-0x8006F608), sonst CURRENT/VISITED, UNVISITED nur mit dem Plan
  (RE2 @0x8006E744 / @0x8006E66C, Umriss Zeile 498 @0x8006E71C), Teilbereiche je Teil; Schema: Besucht-/Etagen-Bit,
  aktuell, Hinweisziel, Plan. Zwei Eintraege geteilt, weil zwei Traeger je Teilstueck (1F r8 x179, B2 r5 x187).
  Haken re15_inv_screen.c: die fuenf vorhandenen "Runde 39 Spur G"-Aufrufe reichen `st` durch (keine neue Zeile).
  Kein Datenwechsel im Vollzustand: alle Traeger gezeichnet -> dieselben Korrekturen wie vorher.
* MESSUNG NACHHER (echte exe, Bau 2026-10-18 ~20:45, G39nb1/allein_nb1.sh = Liste und Lauf der Abnahme, nur NEU-exe;
  Auswertung allein_ausw.py der Abnahme gegen ihre VORHER-Abzuege):
```
1050 0  1020 0  1040 0  1070 0  1030 0  1010a 0  1010b 0  1080 0  1000o 0  1000n 0  1000s 0  1110 0  1210 0  1220a 0  1220c 0
(verlorene Wandpixel VORHER -> NEU; Abnahme: 8/16/17/34/0/30/30/8/41/26/12/0/0/16/16)
offen.py: alle 0 ausser 1110 4 (VORHER 4) und 1210 4 (VORHER 2)
```
  1110: die Ostseite des Evidence-Raums x186 y115..120 ist schon VORHER offen - die Schwenk-Tilgung des Renderers
  (inv_render_pc.c re15_schwenke_entfernen) nimmt dort den gemalten Tuerschwenk (MAP04.PIX r5, x181..186 y115..120,
  Index 4) samt Wandstueck weg; geschlossen wird er von der Westwand von ROOM1100 (r6). Nicht durch Runde 39.
  1210: die Kachel hat auf x187 bei y71 und y77 Index 0 (VORHER wie NEU offen); der Tuerbalken steht jetzt (Vorlage)
  auf x186, so liegen statt zwei Tuer- jetzt zwei Koerperpixel an den Luecken. ROOM1210 erreicht man nur durch
  ROOM11E0 (dessen Ostwand x186 dann gezeichnet ist) - im Spiel kein Teilzustand ohne Traeger.

### M4 Marker auf der Treppe neben den verschobenen Treppensymbolen (1060 1F/2F, 10A0 1F/B2)
* URSACHE: die Vorlagen ruecken die Treppensymbole aus der Wand; die Abbildung der beiden Treppenhaeuser blieb die
  Bbox-Streckung auf das NOMINALE Rechteck (24x24), gemalt sind 18x22 (1060, 10A0 1F) bzw. 17x21 (10A0 B2). Dadurch lag
  der ganze Raum 3..4 px nach rechts versetzt: die Treppen-Projektion landete auf dem alten (= in die Wand gerueckten)
  Symbolort, und 4 (1060 je Etage) bzw. 8 (10A0 B2) begehbare Rasterpunkte neben der Kachel (M5).
* MESSUNG VORHER: Abnahme 0 Abschnitt 7 (nicht wiederholt): Projektion der STAIR-AOT-Mitten 1F 1060 (127,144)/(135,146)
  gegen die Symbole (122,144)/(133,146), 2F (126,144)/(134,146) gegen (121,144)/(132,146), 1F 10A0 (187,100)/(196,99)
  gegen (183,100)/(194,99), B2 (177,115)/(186,112 Wandpixel) gegen (173,115)/(183,112); echte exe Ring (126,143).
  Eigene Sonde markerflaeche (alle_raeume.py, Stand ad1f5cd7): 1060 Band 0/4/8 je 4 ausserhalb, 10A0 Band 1 8 (x187).
* RE-BELEGE: Original-Zeilen PSX.EXE @0x800768b0 + 8*Index (selbst gelesen, t_addr 0x80010000):
  1060 @0x800768E0 = `00 00 00 00 01 00 01 00`, 10A0 @0x80076900 = `00 00 00 00 01 00 01 00` (Stubs - das Original hat
  fuer beide Treppenhaeuser KEINE Zeile; Gegenprobe ausgeliefert: 1020 @0x800768C0 (100,136,2287,2287), 1030 @0x800768C8
  (108,194,2428,2229), 1040 @0x800768D0 (94,193,2080,2320), 1070 @0x800768E8 (79,205,2229,2088)). Formel FUN_800473f8
  (@0x8004741c-0x80047528, Abnahme Abschnitt 1 selbst disassembliert). STAIR-AOTs = SCD Aot_set (0x2C) sce 12/13,
  Band = pc[4] (G_karten_werkzeug/nb1/treppen_aot.py, Parser wie tools/gen_map_zones.py):
  ROOM1060.RDT @0x00D72 Band 8 (25150,22000), @0x00D86/@0x00D9A Band 6, @0x00DAE Band 4 (21400,22850), @0x00DC2 Band 4
  (25150,22000), @0x00DD6/@0x00DEA Band 2, @0x00DFE Band 0 (21400,22850); ROOM10A0.RDT @0x00E40 Band 8 (21450,21650),
  @0x00E54/@0x00E68 Band 6, @0x00E7C Band 4 (25350,22350), @0x00E90 Band 4 (21450,20650), @0x00EA4/@0x00EB8 Band 3,
  @0x00ECC Band 1 (25350,22350).
* UMSETZUNG (re15_map_zones.h, PORT-ERGAENZUNG wie 10C0/1120/10E0): Zeile je Blatt aus den vier Waenden (Wandflaeche
  -> gemalte Linie, Spiegel unveraendert; s am exakten Massstab gewaehlt, nb1/fit2.py, Reste 0):
  1060 1F (260,14,2511,2516) / 2F (259,14,2511,2516) / 3F (277,17,2511,2516), Spiegel 1,1 - Waende x 26900 @0x004B8,
  19800 @0x004D0, z 18200 @0x004E8, 26950 @0x004C4; 10A0 1F (57,228,2493,2517) / B2 (54,234,2346,2397), Spiegel 0,0 -
  Waende x 19850 @0x005DC, 27000 @0x005E8, z 26300 @0x005D0, 17550 @0x00630. Die Treppensymbole der Vorlagen bleiben.
  3F (Vorlage unveraendert, Symbol (144,147) = alte Bbox-Projektion): die Marke folgt der Projektion der Band-8-Treppe
  (25150,22000) @0x00D72 -> (140,147) - derselbe Lauf, den der Nutzer auf 1F/2F an x122/x121 gelegt hat (Marker im
  Treppenhaus 3F sonst 4 px neben dem Symbol). Abweichung von map3F_correct.png an dieser einen Marke - siehe "Fuer den
  Nutzer".
* MESSUNG NACHHER (Sonde, Bau ~21:10): Projektion der STAIR-AOT-Mitten (probe_r39_karten marker) -
  1F 1060 (123,144)/(132,146), 2F (122,144)/(131,146), 3F (140,147), 1F 10A0 (184,99)/(193,98), B2 (182,110), alle IM
  5x5-Kasten ihres Symbols (vorher 2..5 px daneben); markerflaeche 1060 Band 0/4/8 0/0/0, 10A0 Band 1/4/8 0/0/0
  (vorher 4/4/4, 8/0/0). Tueren (Mitte des Tuer-Rechtecks, Sonde = echter Kartenpfad MIT der Klemmung
  re15_inv_screen.c re15_inv_map_marker "reserve = 4" ins nominale Rechteck und tuer_anziehen): 1060 1F (123,152) /
  2F (122,152) / 3F (140,155), 10A0 1F (197,92) = Marke, B2 (174,105). Die Formel selbst laege auf der Wand (1060 1F
  (118,152), B2 (173,101)); die 4-px-Reserve der Kachel-Klemmung haelt den Marker von der West-/Nordkante des NOMINALEN
  Rechtecks fern (vorbestehend, gilt fuer jede Kachel, nicht angefasst). Tuerwechsel-Sprung (Ankunft im Nachbarraum,
  dessen Zeile unveraendert): 1060 -> 1040 7 -> 8, -> 10C0 5 -> 6, -> 1120 6 -> 7 (je +1 px in y: die Wandzeile legt
  die Tuermitte z 25400 auf y152, die Bbox auf y151 = die Zeile von 1040); 10A0 B2 -> 11E0 9 -> 6.

### M5 Marker weiter neben der Zeichnung (1030, 1110, 1170 Zone 1, 1060/10A0, 1210)
Messung vorher = Abnahme 0 Abschnitt 5 (nicht wiederholt): 1210 207/1141, 1170 Zone 1 51/311, 1030 63/2288, 1110 8/1237,
1060 4/155, 10A0 Band 1 8/140. Werkzeug je Raum: Waende (SCA-Zelle, Innenflaeche, Datei-Offset) -> gemalte Linie der
Kachel (Kachel-Index 4), Zeile fuer FUN_800473f8 per kleinsten Quadraten (zeile_fit.py), Gegenprobe an Tueren/Treppen.
* 1060 / 10A0: ueber M4 (Zeilen aus den Waenden) - 0 ausserhalb auf allen Baendern.
* 1030: AUSGELIEFERTE Zeile @0x800768C8 = `6c 00 c2 00 7c 09 b5 08` (108,194,2428,2229) - selbst gelesen. Gegen die eigene
  Kachel (MAP03.PIX r4 uv(184,56)) liegt sie bis 5 px daneben: z -1784 @0x01A70 -> y130 (gemalt 125), -5212 @0x01A40 ->
  137 (134), -10568 @0x01A10 -> 148 (144), -22771 @0x01AE8 -> 174 (173); x -26732 @0x01A4C -> 120 (119), -19254 @0x01A34
  -> 138 (136), 2398 @0x01A1C -> 188 (186). Die 63 Punkte sind der Westfluegel (Welt z -4710 -> y136 unter der gemalten
  Suedwand y134). Mit DENSELBEN Offsets (108,194) und den an den acht Waenden gemessenen Massstaeben sx 2403 / sy 2373
  treffen alle Waende auf <= 1 px. Beta -> Retail: die ausgelieferte Zeile passt nicht zur eigenen Zeichnung (unfertig);
  RE2 bildet mit EINEM Massstab ab (FUN_8006e120 /450, @0x8006e1dc-@0x8006e20c, Abnahme 0 Abschnitt 1), seine Kunst
  ist darauf gezeichnet - der Marker liegt dort per Konstruktion auf der Zeichnung. PORT-KORREKTUR nur der Massstaebe.
* 1110: Original-Zeile @0x80076938 = Stub. Die Bbox-Streckung legte den Tuergang nach 1100 (Welt x -3250..-950,
  z -6650..-4000) auf x187 neben die Ostwand x186. Zeile aus den Waenden des Evidence-Raums (Spiegel 1,1 unveraendert):
  x 12650 @0x00734 -> 146, -3250 @0x0071C -> 186; z -6650 @0x00788 -> 114, 5100 @0x00728 -> 144 => (258,49,2638,2677).
  Gegenproben: Ostwand der kleinen Raeume x -13850 @0x007A0 -> 212 = gemalte Ostlinie; Tuergang (-2650,-6150..-4050)
  -> (184,115..120) = genau der gemalte Tuerschwenk x181..186 y115..120 (Lage und Spiegel bestaetigt - die Vermutung
  "L-Form passt unter dem Spiegel nicht" aus dem Bau galt nur fuer die Bbox).
* 1170 Zone 1: die ausgelieferte Zeile @0x80076968 (100,206,2280,2268) gehoert Zone 0; Zone 1 hatte die Bbox. Die
  gemalte L-Form (MAP05/06.PIX uv(192,16)) passt mit Spiegel 0,0 - Band 0 Kopfband + Arm nach Sueden, Band 4 Streifen
  nach Osten = Unterarm (die "180 Grad gedreht"-Vermutung des Baus sah nur Band 0). Neun Waende (x -26926 @0x00B70,
  -24183 @0x00B94, -20000 @0x00B64, -20471 @0x00BF4, -10432 @0x00BD0; z -18272 @0x00B58, -24434 @0x00BE8, -26881
  @0x00B7C, -27134 @0x00BDC) -> 3F (140,122,2597,2511), Dach (128,113,2597,2511), Reste <= 1. Treppen 3F: Lauf Band 0 -> 2
  @0x0137A (156,95) .. @0x0138E (156,101): Marke (159,99) (Bbox-Projektion der Laufmitte) -> (156,98); Band 4 @0x013B6
  -> (168,107) bzw. (168,106) (M1). Dach: Marke (156,97) gegen (156,98) - bleibt.
* 1210 (207/1141): die Suedwest-Kammer ist begehbar, aber weder im Original (MAP02.PIX r3) noch in mapB2_correct.png
  gezeichnet - Zielkonflikt Vorlage gegen Marker; NICHT eigenmaechtig dazugezeichnet, Entscheidungsfrage an den Nutzer
  bleibt (s. "Fuer den Nutzer").
* MESSUNG NACHHER (Sonde markerflaeche, alle_raeume.py, Bau ~21:35): 1030 0/2288, 1110 0/1237 (+ kleine Raeume 0/150,
  0/180), 1170 Zone 1 3F 0/311, Dach 0/186 (vorher 12), 1060 0/0/0, 10A0 0/0/0.

### M1 3F-Treppe (168,107): alle Sprossen auf der Linie y110
* Die Vorlage verschiebt die Treppe um 1/3 Spielpixel; je nach 3x-Phase ergibt die Abtastung y104..108 (Abnahme,
  Blockmitten) oder y105..109 (Bau). Bei 1x passen 5 Sprossenzeilen in die 6 freien Zeilen y104..109 nicht mit Abstand
  zu beiden Linien. Gezaehlt: y107 = 3 Beruehrpixel (alle Sprossen auf y110, x166 auf dem Tuerbalken 1130/1170), y106 =
  1 Beruehrpixel ((170,104) unter der Wandecke (170,103)). Die Projektion der Band-4-Treppe @0x013B6 mit der neuen
  Zeile liegt je nach Wandsatz auf y106/y107 - beide im Kasten. Entscheidung: (168,106) (weniger Beruehrung,
  Blockmitten-Abtastung der Vorlage). Riegel-Soll entsprechend.

### M2 B1-Treppe 10A0 (145,165) auf der Unterkante y168
* Projektion der Mitte (21450,20650) @0x00E90 = (145,165); unterste Sprosse (143..147,167) Pixel an Pixel auf der
  Unterkante y168 der Absatz-Zelle (141,153,18,16). Marke -> (145,164): Sprossen y162/164/166, eine Zeile Abstand; die
  Projektion bleibt im 5x5-Kasten (Riegel r36 Treppe <= 1 px haelt). Riegel unit_r39_karten_b1 prueft jetzt "keine
  Sprosse beruehrt die Randlinie" (Nachbarpixel jeder Sprosse im Innenraum).

### Nachbesserung 1 - Riegel (neu/erweitert) und Messung an der echten exe
* `unit_r39_karten_teil` (M3): (a) jede bedingte Korrektur hat einen Traeger, dessen Kachel auf der Ersatzlinie Index 4
  traegt (15 Eintraege); (b) jeder Ort der Abnahme ALLEIN besucht: kein Raumkoerper grenzt an den Grund (Klassenbild der
  echten Op-Liste, komposit()); (c) Ort + Traeger besucht: die Korrektur greift (keine Doppelwand); (d) ROOM1000-Kanten
  mit Traeger weg. NEGATIVPROBE (Traeger-Gatter voruebergehend aus = Stand 40328436, nur die Sonde gebaut, danach
  zurueckgesetzt): `1050 5, 1020 15, 1040 14, 1070 34, 1010 30/30, 1080 8, 1000 45/28/13, 1110 1, 1220 16/16` -> ROT.
* `unit_r39_karten_treppen` (M4/M5/M1): jedes Treppensymbol von 1060 (1F/2F/3F), 10A0 (1F/B2/B1) und 1170 Zone 1 (3F)
  liegt <= 2 px (Chebyshev = 5x5-Kasten) von der Projektion seines STAIR-AOT-Laufs (Aot_set sce 12/13 aus dem
  geladenen RDT, Zeile des Blatts, re15_map_zone_marker = FUN_800473f8). Gemessen: 1060 1/1, 1/1, 0; 10A0 1/1, 1/2,
  0/0; 1170 0/1. Vorher (Bbox, Abnahme Abschnitt 7) 5/2, 5/2, 4/2, 4/3 px -> ROT.
* `unit_r39_karten_marker` (M5): markerflaeche 1030, 1060 Band 0/4/8, 10A0 Band 1/8, 1110 (drei Teile + Elza 1111),
  1170 Zone 1 3F + Dach, 10C0, 1120 - je 0 begehbare Rasterpunkte neben der Zeichnung.
* `unit_r39_karten_b1` (M2): zusaetzlich "keine Sprosse beruehrt die Randlinie" - (145,164) und (154,162) PASS; mit
  (145,165) waere die unterste Sprosse am Rand y168.
* `integration_r39_karten` (echte exe): Sweep B2/1F/2F 0 Abweichungen; 3F 0 Abweichungen ausserhalb der drei
  begruendeten Treppen-Kaesten, die neuen Symbole (140,147), (156,98), (168,106) je 15/15 Sprossenpixel; Doppelwand
  0/0/0/0/0; NEU Laeufe TN/TW/TZ (ROOM1000 Nord-WC, 1070, 1010 Zone 0 je allein): `0 Koerperpixel am Grund` (vorher
  26/34/30 verlorene Wandpixel). Lauf 92 s, PASS.
* Echte exe, Stichproben Ring (ring.py der Abnahme, Bau ~22:05): Leon auf der 1060-Treppe (25150,0,22000) -> Ring
  (122,143) auf dem Vorlagen-Symbol x120..124 (vorher (126,143) daneben); 1030 Westfluegel (-25000,-4500) -> (123,131)
  in der gemalten Kachel (y126..133); 1110 Tuergang (-2650,-5500) -> (183,117) im gemalten Schwenk. Sonde = exe - 1
  (Ringsprite). 1170 Zone 1 per CONTINUE nicht abziehbar (der Raum startet mit der Intro-Szene, Spawn am
  Intro-Wegpunkt) - dort misst die Sonde denselben Pfad (re15_inv_map_marker).

### Nachbesserung 1 - Uebersicht je Mangel
| Mangel | Ursache | Beleg | Aenderung | vorher -> nachher |
|---|---|---|---|---|
| M3 (hoch) | Doppelwand-Korrektur loeschte die eigene Wand unbedingt | Traeger je Korrektur aus MAP0x.PIX Index 4 (nachbar.py), Gatter wie RE2 @0x8006E744/@0x8006E66C | `traeger` je Korrektur/Kante, gilt nur mit gezeichnetem Traeger | verlorene Wandpixel je Ort allein 8..41 -> 0 (echte exe, 15 Orte) |
| M4 (mittel) | Treppenhaeuser 1060/10A0 mit Bbox-Streckung auf nominale 24x24 | Stubs @0x800768E0 / @0x80076900; Waende ROOM1060/10A0.RDT; STAIR-AOTs @0x00D72..@0x00DFE / @0x00E40..@0x00ECC | Zeilen je Blatt aus den Waenden; 3F-Marke 1060 auf die Projektion | Treppe 2..5 px daneben -> im 5x5-Kasten; Ring 1060 (126,143) -> (122,143) |
| M5 (mittel) | 1030 ausgelieferte Zeile mit falschem Massstab; 1110/1170 Z1 Bbox | 1030 @0x800768C8 bis 5 px gegen die eigene Kachel; 1110 Stub @0x80076938; 1170 @0x80076968 nur Zone 0 | Zeilen aus 8/4/9 Waenden (Reste <= 1) | 1030 63 -> 0, 1110 8 -> 0, 1170 Z1 51 -> 0 (Dach 12 -> 0), 1060 4 -> 0, 10A0 8 -> 0; 1210 Nutzerfrage |
| M1 (gering) | 1/3-px-Verschiebung der Vorlage bei 1x nicht darstellbar | Beruehrpixel gezaehlt: y107 3, y106 1; Band-4-Treppe @0x013B6 -> y106/107 | (168,106) | 3 -> 1 Beruehrpixel |
| M2 (gering) | Treppe (145,165) Pixel an Pixel auf der Unterkante y168 | Projektion @0x00E90 (145,165); Zelle (141,153,18,16) | (145,164) | Abstand 0 -> 1 Zeile, Projektion im Kasten |

### OFFEN (Stand Nachbesserung 1)
* ROOM1210 (B2): begehbare Suedwest-Kammer weder im Original noch in mapB2_correct.png - Marker dort neben der Kachel
  (207 von 1141 Rasterpunkten). Entscheidungsfrage an den Nutzer (unten), nicht eigenmaechtig gezeichnet.
* Vorbestehend, nicht durch Runde 39, nicht angefasst: (a) die Kachel-Klemmung des Markers (re15_inv_map_marker,
  `reserve = 4` ins NOMINALE Rechteck) haelt den Marker 4 px von dessen West-/Nordkante fern - an Tueren dort (1060 ->
  1040/10C0/1120) springt er beim Raumwechsel 6..8 px (vorher 5..7); (b) im Abzug der echten exe nimmt die
  Schwenk-Tilgung des Renderers an zwei Stellen Wandtexel mit: Evidence-Raum 1110 Ostseite x186 y115..120 (geschlossen
  durch ROOM1100s Westwand) und ROOM1210 x187 y71/y77 (geschlossen durch ROOM11E0) - offen nur, solange der Nachbar
  ungezeichnet ist (1210 ist nur durch 11E0 erreichbar).
* 11D0-Kammer: Weltbox um ZONE_SLACK eingezogen (Bau) - laut Abnahme 0 an begehbaren Randpunkten nicht reproduziert.

### Fuer den Nutzer (Nachbesserung 1)
* Raeume bleiben jetzt auch dann geschlossen, wenn der Nachbarraum noch nicht auf der Karte steht: die "keine
  Doppelwand"-Korrektur nach deinen Vorlagen greift nur, wenn die Wand des Nachbarn gezeichnet ist (vorher z.B. ROOM1000
  WC ohne Westwand, 1070 ohne Westwand, 1010 ohne Seitenwaende, solange der 1F-Plan fehlte).
* Treppenhaeuser 1060 und 10A0: der Marker steht auf der Treppe jetzt auf deinen verschobenen Treppensymbolen (vorher
  2..5 px daneben bzw. auf der Wand).
* Marker bleibt in der Zeichnung: Eingangshalle 1030 (Westfluegel), Evidence-Raum 1110 (Tuergang), Treppenaufgang 1170
  (3F und Dach) und die Treppenhaeuser.
* 3F weicht an DREI Treppensymbolen bewusst von map3F_correct.png ab: (1) Treppenhaus 1060 x144 -> x140 - derselbe
  Treppenlauf, den du auf 1F/2F an x122/x121 gelegt hast; (2) Treppenaufgang 1170 (159,99) -> (156,98) - auf den echten
  Lauf; (3) die Treppe (168,107) aus deiner Vorlage ist um 1/3 Spielpixel verschoben, das geht bei 1x nicht: y107 stellt
  alle drei Sprossen auf die Linie darunter (eine auf den Tuerbalken), y106 beruehrt nur mit einem Eckpixel die Wand
  darueber - genommen ist y106. Wenn du es anders willst: kurze Ansage reicht.
* B1: die Treppe nach B2 im Treppenhaus 10A0 sitzt eine Zeile hoeher und beruehrt die Unterkante nicht mehr.
* Entscheidungsfrage (unveraendert): ROOM1210 hat im Suedwesten eine begehbare Kammer, die weder das Original noch deine
  Vorlage mapB2_correct.png zeichnet. Soll sie auf B2 als kleiner Raum dazukommen (dann steht der Marker dort im Raum),
  oder bleibt B2 genau wie deine Vorlage?
* Keine neuen Assets, keine Sprachdateien.

### Nachbesserung 1 - Bau und gezielte Tests (KEINE volle Suite)
Bau `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`. Gezielter Lauf (die 93 Karten-/Hinweis-/
Inventar-Gruppen der Abnahme + die drei neuen Riegel), 2026-10-18 20:44..20:54, `-j 4`, Last durch Parallel-Laeufe
anderer Spuren:
```
ctest --test-dir re15_port/build -R "^(unit_map_|unit_karte_|unit_r30_karte|unit_r30_hinweis|unit_r33_karte|unit_r35_karte|unit_r35_cut10f0|integration_r35_cut10f0|unit_r36_karte|unit_r36_kartenbits|unit_r36_kartenitems|integration_r36_karte|integration_r36_kartenitems_bild|unit_r38_tagebuch|integration_r38_tagebuch|unit_r39_karten|integration_r39_karten|integration_r39_elza_karte|unit_r39_inventar|integration_r39_inventar|integration_map_uebergang|integration_map_raum_live|integration_r35_karte|unit_inv_|unit_r26_inventar|unit_r35_inventar1050|integration_r35_inventar1050|unit_r36_inventar|integration_r36_inventar|unit_r38_kleinkram_inventar|integration_r38_kleinkram_inventar|unit_pri_inventar_riegel|unit_r30_android_n1_inventar_cmd)" -j 4 --timeout 900 --output-on-failure
100% tests passed, 0 tests failed out of 96
```
Danach nur ein Kommentar in karte_r39.c (Werkzeugpfad) berichtigt, neu gebaut:
```
ctest --test-dir re15_port/build -R "^(unit_r39_karten|integration_r39_karten)" --timeout 900 --output-on-failure
100% tests passed, 0 tests failed out of 6
```

## Nachbesserung 2 (nach Abnahme 1, 2026-10-18 ~22:30)
Grundlage: analysis/befunde_runde39/G_abnahme_1.md (Mangel N1) und der ORCHESTRATOR-ENTSCHEID dazu: die begehbare
Suedwest-Kammer von ROOM1210 wird samt Durchgang auf B2 DAZUGEZEICHNET - aus der Kollision (SCA-Waende, Form wie
B1/ROOM1000), keine Wahlfrage an den Nutzer (Memory reai-v2-original-oder-nicht, Beta -> Retail: RE2 zeichnet jede
begehbare Flaeche). Die Abweichung von mapB2_correct.png wird unten "Fuer den Nutzer" benannt (Bild vorher/nachher).
Riegel-Soll: markerflaeche 1210 = 0 Punkte neben der Zeichnung (vorher 1693 von 10387); echte exe am Abnahme-Weg
(Spielstand 1210, W1,U4,W5, RE15_INV_OPEN_AT=200#1210) Ring in der Zeichnung; uebrige Blaetter pixelgleich zu Abnahme 1.
Arbeitsordner der Messungen: Scratchpad `nb2/` (nicht im Baum); die Werkzeuge liegen in `G_karten_werkzeug/nb2/` (proj.py = FUN_800473f8 mit der Zeile @0x800769B8, aussen1210.py, scd_dump.py, gdi_k.sh).

### Fortschritt
- [x] Messung vorher (Sonde markerflaeche 1210, echte exe am Abnahme-Weg) - a6ba2ff0
- [x] RE: Kammer + Durchgang aus der SCA von ROOM1210 (Offsets), Begehbarkeit im Original (Kameras 6/7, kein Schalter
      fuer Leon; Elza: sub00 schliesst) - a6ba2ff0 + unten
- [x] Umsetzung: Zone 1 von ROOM1210/1211 + Schema-Zeichnung auf B2 + Durchgang; Querbalken-Fenster (184 Punkte) - 8dcd780d
- [x] Messung nachher (Sonde, echte exe Lauf K, Sweep aller Blaetter gegen Abnahme 1) - d29ee63b + unten
- [x] Riegel + Gegenprobe (Kammer/Fenster abgeschaltet -> ROT; Koerper-Gatter abgeschaltet -> ROT)
- [x] gezielte Tests (99/99), Tuer-Uebergaenge gegen Abnahme 1, gdigrab, Abschluss

### N1 Messung vorher (Stand b1da8c01/0c1ded3c, Bau der Abnahme; die Abnahme-Zahlen bestaetigt, nicht neu erhoben)
`probe_r39_karten markerflaeche 1210 0 -25800 -2200 0 100 1` (echter Kollisionsloeser, jeder Rasterpunkt):
`10387 begehbare Rasterpunkte, 10387 geprueft, Marker x 191..224 y 74..128, ausserhalb der Zeichnung 1693` - alle in der
Kammer (Beispiele Welt (-25128,-26240) -> (193,128) Index 0). `probe_r39_karten marker 1210 -25500 -21700 0 0`:
`Zone Blatt 1 rect 3 zid 34, Marker (192,118), Kachel-Index am Marker 0` = Ring (191,117) der echten exe (Abnahme
Abschnitt 6, Sonde = exe + (1,1)). `probe_r39_karten flut 1210 0 -25500 -21700 450 100`: die Kammer haengt an der
Flurflaeche (10387 Punkte, eine Komponente), Kammer-Mitten x -26828..-22928, z -26240..-20440; Durchgang z -22040..-21340.

### N1 RE-Belege (selbst gelesen)
* **Begehbar im Original, eigene Kameras.** ROOM1210.RDT RVD (Zeiger RDT+0x28 = 0x188, 20 B je Eintrag): @0x002DC
  Schnitt 5->6 genau in der Oeffnung (x -22000..-21300, z -23500..-19500), @0x002F0 Schnitt-6-Bereich (x -28600..-17300,
  z -27800..-17200), @0x00304 6->5 (x -21200..-20300), @0x00318 6->7 in der Kammer (z -24260..-23419), @0x0032C
  Schnitt-7-Bereich, @0x00340 7->6. Das Original fuehrt fuer die Kammer ZWEI eigene Kameraschnitte (6, 7) - sie ist ein
  gewollter, begehbarer Teil von ROOM1210, kein Kollisionsloch.
* **Keine Sperre im Skript.** main00 (@0x01CC6..@0x01E9C, Laengen wie scd_opcode_size_at): sechs Door_aot_set 0x3B
  (@0x01CC6 -> ROOM11E0, @0x01CE6/@0x01D06/@0x01D26/@0x01D46/@0x01D66 -> ROOM1220 Schnitte 0/2/4/6/8 = die FUENF Zellen)
  - fuer die Suedwest-Zelle gibt es KEINE Tuer; Obj_model_set 0x2D @0x01E4E Objekt 0 an (-21750,0,-20760) rot 1216 =
  `2d 00 00 00 01 00 0a 00 10 00 0a ab 00 00 e8 ae 00 00 c0 04 ...` mit Kollisionsbox 0 (Bytes 22..33 = 0) = die
  aufgeschwenkte Zellentuer am oberen Ende der Oeffnung; kein 0x37/0x38 (SCA-Schalter) in main00/sub00..02.
* **Die Kammer ist die dritte West-Zelle, offen.** Seiten-Tabelle B2 @0x800762F4 (10 Eintraege, selbst gelesen): r4
  (187,78) / r5 (187,95) West-Zellen, r6/r7/r8 (212,78/95/112) Ost-Zellen von ROOM1220; an der Stelle (187,112) fehlt die
  sechste. ROOM1210-Zeile @0x800769B8 = `b1 00 8c 00 c0 09 ca 08` = (177,140,2496,2250), MAP02.PIX r3 uv(96,48) malt dort
  nichts (Index 0 auf x186..200 y113..128). RE1.5 unfertig -> Beta -> Retail (Orchestrator-Entscheid).
* **Waende -> Linien** (FUN_800473f8 @0x8004741c-0x80047528 mit der Zeile @0x800769B8, nb2/proj.py; gemalte Linien
  MAP02.PIX Index 4, selbst gelesen):
  - West @0x01BEC x -28908..-27358 -> px 184..188 -> Linie x186 = Ostwand von r9 (10A0 B2, Index 4 auf x186 y101..121);
  - Nord @0x01BF8 z -19898..-17948 -> py 114..110 -> Linie y112 = Unterkante von r5 (Index 4 x187..200) + r9 (186,112);
  - Sued @0x01BE0 z -28496..-26696 (x -28928..-16328, dieselbe Zelle unter dem Flur) -> py 132..129 -> Linie y129 = die
    Flur-Unterkante von r3 (Index 4 ab x201);
  - Ost @0x01C40 x -22397..-21185 z -27560..-22520 und Flur-Westwand @0x01BC8 x -22150..-21090 z -20746..-4084 -> px
    200..203 -> Linie x201 = r3 Spalte 14 (Index 4 auf y110..129);
  - Oeffnung: @0x01BC8 endet bei z -20746 -> py 116, @0x01C40 beginnt bei z -22520 -> py 120 -> Durchgang = x201 y117..119.
  - Hindernisse IN der Kammer (keine Waende, Memory reai-v2-hindernis-ist-keine-wand): @0x01C64 x -28208..-25652
    z -27794..-23402 (Pritsche), @0x01C58 x -25291..-21644 z -20795..-18995 (Bank an der Nordwand), Kreis @0x01C4C.
  - Marker: Kammer-Mitten x -26828..-22928 -> px 189..199, z -20440..-26240 -> py 115..128 = im Innenraum x187..200
    y113..128; Durchgang z -21196..-22070 -> py 117..119.

### N1 Befund beim Nachmessen: 184 der 1693 Punkte liegen NICHT in der Kammer
Nach dem Einbau der Kammer meldete `markerflaeche 1210 ... 100 1` noch **184** Punkte neben der Zeichnung - alle im
QUERBALKEN des T-Flurs: Marker (191..200,79) und (213..224,79), Welt z -3440 (Beispiel (-26328,-3440) -> (191,79), Index 0).
Nachgebildet mit nb2/aussen1210.py (Zeile + Klemmung + MAP02.PIX r3) = exakt dieselben 184. Die Abnahme hatte sie der
Kammer zugerechnet (1693 = Kammer 1509 + Querbalken 184). URSACHE: die Kachel r3 malt den Balken GESTAUCHT - Linien y70 und
y78 (MAP02.PIX r3 Zeilen 0/8, Index 4; y78 mit T-Oeffnung x202..211 Index 1), die Waende liegen mit der Original-Zeile
@0x800769B8 11,7 px auseinander (Mitten: Nord @0x01B8C z 466 -> py 70, Sued @0x01BD4 z -4994 -> py 82). Der suedlichste
begehbare Streifen (Mitte bis z -3470 = Innenseite @0x01BD4 z -3920 + Spielerradius 450; py 79 ab z -3337) landet
damit 1 px UNTER der Linie y78 - in den Zellen r4/r6 von ROOM1220 (fremder Raum). Gleiche Fehlerklasse (Marker neben der
eigenen Zeichnung), derselbe Raum, vorbestehend (Original-Zeile + Original-Kunst; das Original klemmt gar nicht).
Eine Neu-Eichung der Zeile hilft nicht: kein linearer Massstab trifft Balken UND Flur/Kammer (Balken 8 px gemalt fuer
11,7 px Welt), und 1220 teilt den Rahmen.

### N1 Umsetzung (Dateien, Konstanten)
* `engine/src/re15_map_zones.h`: Zonen `{ 0x1210/0x1211, -25858, -25196, -23628, -21398, 1, 255, 1, 112, 177, 140, 2496,
  2250, 0, 0, 12, 0 }` = Zone 1 (zid 112 neu, frei). Zeile = die des Flurs @0x800769B8 (ein Raum, ein Rahmen; gemessen:
  Marker an der Zonengrenze stetig, (-22128) -> (200,118) Zone 1, (-22127) -> (201,118) Zone 0). Weltbox =
  Innenflaeche um ZONE_SLACK 1500 eingezogen (wie 11D0 Zone 1): West -27358 (@0x01BEC), Nord -19898 (@0x01BF8), Sued
  -26696 (@0x01BE0), Ost -22128 = letzte Weltlage mit px 200 (FUN_800473f8: px(-22128) = 200, px(-22127) = 201 = die
  gemalte Flur-Westwand). Besucht-Bit: zone_bit idx 1 = 2n+1 von ROOM1210 (bisher ungenutzt; alte Spielstaende: 0).
  `s_map_synth` synth 12 = `{ 186, 112, 15, 18, 21, 1, 0,0,0,0,0,0 }` (keine Affine - Marker per Zeile), Zelle 21 =
  `{ 186, 112, 15, 18 }`: Linien x186 / y112 / y129 liegen auf den gemalten (r9 Ostwand, r5 Unterkante, r3 Flur-
  Unterkante), die Zelle endet bei x200.
* `engine/src/karte_r39.c`: s_korr `{ 1, 3, 14, 47, 1, 3, 1, R39_SCHEMA(12) }` = DURCHGANG x201 y117..119 (r3 Spalte 14
  Zeilen 47..49, Index 4 -> Koerper), Traeger = die Kammer (ohne sie bliebe hinter der Luecke Grund); s_kanten
  `{ 1, 186, 112, 15, 18, 3, 3 }` = Ostkante der Kammer entfaellt, wenn r3 gezeichnet wird (eine Linie: x201). NEU
  `re15_karte_r39_fenster` + Tabelle s_fenster: ROOM1210/1211 Zone 0, Welt z > -4084 (Nordende @0x01BC8) ausserhalb der
  Flurspanne x -21090 (@0x01BC8) .. -17696 (@0x01C04): Klemmfenster = gemalter Balken r3 x187..226 y70..78 (wirksam nur
  y <= 78). R39_SCHEMA nach oben gezogen (vor s_korr).
* `engine/src/re15_inv_screen.c`: EIN Haken `re15_karte_r39_fenster(zn, world_x, world_z, &lo_x, &hi_x, &lo_y, &hi_y);`
  direkt nach re15_karte_fahrstuhl_fenster (Kommentar "Runde 39 Spur G"). `include/re15_karte_r39.h`: Deklaration.
* Keine Assets, kein Patch an shared_assets, keine Bank-9-Bits, keine Nachrichten-Ids, keine Sprachdateien.

### N1 Messung nachher
* Sonde (Bau 8dcd780d): `markerflaeche 1210 0 -25800 -2200 0 100 1` -> `10387 begehbare Rasterpunkte, 10387 geprueft,
  Marker x 189..224 y 74..128, ausserhalb der Zeichnung 0` (vorher 1693). Elza ROOM1211: Kammer offen (Flags (4,0xEC) +
  (4,0xFC)) 0 von 10391; Kammer zu (Flags 0, sub00 Sca_id_set Zelle 15): Flur 0 von 8783, Kammer 0 von 2022.
  Zonengrenze im Durchgang (z -21700): x -22200/-22128 -> (200,118) Zone 1; -22127/-22000/-21800 -> (201,118) Zone 0;
  -21600/-21300 -> (202,118); -21000 -> (203,118) - stetig, kein Sprung.
* ECHTE exe, Weg der Abnahme (integration_r39_karten Lauf K: Spielstand 1210 Flur (-19500,0,-21700) rot 2048, 11E0 + 1210
  besucht, LOAD GAME, RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT=W1,U4,W5, RE15_INV_OPEN_AT=200#1210, FB_SHOT):
  `letzte Spielerlage: x=-25500 z=-21700` (wie Abnahme), `MARKER 191 117` (Ring-Mitte, wie Abnahme), um den Ring
  `rot=36 besucht=0` (vorher: Blau), Kasten Kammer+Durchgang offen 0, Doppelwand 0. Bild
  `G_karten_bilder/b2_kammer_echtlauf_vorher_nachher_x4.png` (links Abnahme-1-Abzug w1210b, rechts Lauf K).
* Sweep aller 13 Blaetter (Lauf S) gegen den Sweep der Abnahme 1 (G39abn1/neu_S): s00, s02..s12 **0** Pixel verschieden;
  s01 (B2) 249 Pixel, alle im Kasten x186..201 y113..129 (= Kammer + Durchgang). Bild
  `G_karten_bilder/b2_kammer_soll_nachher_x4.png` (links mapB2_correct.png rechts, rechts Sweep).
* B2 gegen die Vorlage (sollbild): 55935 Kartenpixel ausserhalb der Kammer, 0 Abweichungen; Kammer 272 von 272 Pixeln
  wie die Zeichnung aus der SCA. Doppelwand B1..3F je 0.

### N1 Riegel und Gegenprobe
* `unit_r39_karten_marker` + ROOM1210 (Dichte 1) und ROOM1211 (offen / zu: Flur / zu: Kammer) je 0 neben der Zeichnung.
* `unit_r39_karten_teil` (a) Durchgang = Koerper an einer Zelle der Kammer; (e) fuenf Zustaende: Flur allein (Kammer
  ungezeichnet, x201 y117..119 Wand 3/3), Flur+Kammer aktuell Kammer / aktuell Flur (Linien 65/65, Durchgang 3/3,
  Innenraum 224/224, x200 keine zweite Linie, offen 0), Kammer allein (Ostkante x200 18/18, offen 0), Umriss mit B2-Plan
  (Linien 34/34, Durchgang durchsichtig 3/3, x200 frei).
* `integration_r39_karten` + Lauf K (s.o.), sollbild-Kasten der Kammer gegen die Zeichnung aus der SCA.
* GEGENPROBE (Kammer-Zonen und Fenster voruebergehend auf Raum 0x9210/0x9211 gestellt, nur gebaut und die Sonde
  gefahren, danach zurueckgesetzt und neu gebaut): `pruef marker` ROT - 1210 1693 von 10387, 1211 offen 1693 von 10391,
  1211 zu: Flur 184 von 8783, Kammer 2022 von 2022; `pruef teil` ROT - Flur+Kammer Linien 17/65, Durchgang 0/3, Innenraum
  0/224; Kammer allein x200 0/18.

### N1 Nachschaerfung: Durchgang nur mit gefuellter Kammer (Plan-Umriss)
Beim Durchgehen der Teilzustaende (Muster M3): Flur besucht + B2-Plan, Kammer noch nie betreten -> die Kammer steht als
UMRISS (RE2 Zeile 498, Koerper 0x0000), der Durchgang haette den roten Flurkoerper in die Luecke neben den LEEREN Umriss
gelegt (3 Koerperpixel am Grund = offene Seite). Neuer Traeger-Typ `R39_SCHEMA_KOERPER(s)` (karte_r39.c, 192 + synth):
zaehlt nur eine Zeichnung MIT Koerper (besucht/aktuell, Hinweis nur in der roten Phase), kein Plan-Umriss. Damit bleibt die
Flur-Westwand bis zum ersten Betreten der Kammer geschlossen (Plan zeigt die Kammer als geschlossenen Umriss an der Wand);
danach Durchgang offen. Riegel unit_r39_karten_teil (e) Zustaende 5 (alles Umriss: x201 y117..119 Wand 3/3) und 6 (Flur
aktuell + Plan: Wand 3/3, Umriss leer, offen 0). GEGENPROBE (Traeger voruebergehend wieder R39_SCHEMA(12), gebaut, Sonde,
zurueckgesetzt und neu gebaut): Zustand 5 `x201 y117..119 Wand 0/3`, Zustand 6 `Wand 0/3 ... offene Seite 3` -> ROT; mit
R39_SCHEMA_KOERPER 3/3 bzw. offen 0.

### N1 Sicht am ECHTEN Fenster (Skill re15-port-visual-verify)
nb2/gdi_k.sh = derselbe Abnahme-Weg (Spielstand 1210 Flur, LOAD GAME, W1,U4,W5, Karte bei Bild 200) mit dem
Direct3D-Renderer (`[render] SDL-Renderer direct3d`), Kartenschirm offen gehalten, `ffmpeg -f gdigrab -i hwnd=<Fenster der
exe-Kopie>`: debug.log `F600 ... pl pos=(-25500,0,-21700)`; gdigrab-Bild gegen den FB-Abzug derselben Sitzung 320x240
deckungsgleich bis auf 13 Pixel im Kasten (189..193,115..119) = Blinkphase des Rings (wie Abnahme 1 bei 1090). Ausschnitt:
`G_karten_bilder/b2_kammer_gdigrab_x4.png` (Kammer rot, Ring darin, Durchgang in der Flurwand).

### Nachbesserung 2 - Uebersicht
| Mangel | Ursache | Beleg | Aenderung | vorher -> nachher |
|---|---|---|---|---|
| N1 Kammer | begehbare dritte West-Zelle (offen) ohne Zeichnung; Original-Kachel/-Zeile/Vorlage zeichnen sie nicht | RVD @0x002DC..@0x00340 (Schnitte 6/7), Door_aot_set nur fuenf Zellen, SCA @0x01BEC/@0x01BF8/@0x01BE0/@0x01C40/@0x01BC8 | Zone 1 (zid 112, synth 12, Zeile @0x800769B8), Zelle (186,112) 15x18 auf den gemalten Linien, Durchgang x201 y117..119 (Traeger Kammer mit Koerper), Ostkante entfaellt mit r3 | markerflaeche 1210 Kammer 1509 -> 0; echte exe Ring (191,117) im Blau -> im Rot der Kammer |
| N1 Rest | Querbalken der Kachel gestaucht (8 px gemalt, 11,7 px Welt) - Suedstreifen projiziert auf y79 in die Zellen r4/r6 | Mitten @0x01B8C -> py70, @0x01BD4 -> py82; r3 Zeilen 0/8 Index 4 | Klemmfenster r3 y70..78 im Balken ausserhalb der Flurspanne (@0x01BC8/@0x01C04) | 184 -> 0 (1210 gesamt 1693 -> 0 von 10387) |

### OFFEN (Stand Nachbesserung 2)
* Keine offenen Punkte aus N1. Die Entscheidungsfrage "ROOM1210-Kammer zeichnen oder B2 wie die Vorlage" ist durch den
  Orchestrator-Entscheid erledigt (gezeichnet, s.u.).
* Vorbestehend, unveraendert und nicht Teil von N1 (Hinweise der Abnahme 1): Kerben (187,71)/(187,77) der Schwenk-Tilgung
  am Tuerbalken 11E0<->1210 (im Lauf K ausserhalb des Kammer-Kastens: 4 Koerperpixel am Grund, mit ROOM11E0 gezeichnet
  steht die Wand x186 direkt daneben); Kachel-Klemmung reserve 4 (H2); Tuer 1030<->1050 bei ungezeichnetem 1030 (H1);
  1130 Wandnasen (H4); 11A0 Band 1 102/444 (Blatt 6, nicht Polizeistation); 10B0/1260 ohne Kartenblatt.

### Fuer den Nutzer (Nachbesserung 2)
* **B2: die Suedwest-Kammer in ROOM1210 ist jetzt auf der Karte.** Unten links im Zellentrakt gibt es eine dritte Zelle
  auf der Westseite, deren Tuer offen steht - man kann aus dem Flur hineinlaufen (das Original hat dafuer sogar zwei eigene
  Kamerawinkel). Weder das Original noch deine Vorlage mapB2_correct.png zeichnen sie; dort stand der Marker deshalb im
  Blauen neben der Karte. Jetzt ist sie gezeichnet wie die Zellen daneben (gleiche Breite, Waende auf denselben Linien, keine
  Doppelwand), mit einer Oeffnung in der Flurwand, wo die Zellentuer offen steht. Sie erscheint, sobald du sie einmal
  betreten hast (wie die WCs in ROOM1000), und ist nur rot, solange du drin stehst. Das ist eine bewusste Abweichung von
  deiner Vorlage - Bild: analysis/befunde_runde39/G_karten_bilder/b2_kammer_soll_nachher_x4.png (links deine Vorlage,
  rechts jetzt) und b2_kammer_echtlauf_vorher_nachher_x4.png (Marker vorher im Blau, jetzt in der Kammer).
* Bei Elza ist dieselbe Zelle im Original zunaechst verschlossen (das Raumskript macht die Tuer fest, bis zwei
  Story-Flags gesetzt sind); ist sie offen, gilt dasselbe wie bei Leon.
* Nebenbei behoben (gleiche Fehlerklasse, derselbe Raum): ganz unten im Querbalken des T-Flurs von ROOM1210 rutschte der
  Marker 1 Pixel in die Zellen darunter (die Kachel malt den Balken schmaler, als er im Raum ist). Er bleibt jetzt auf dem
  Balken. Damit steht der Marker in ROOM1210 an jedem begehbaren Punkt auf der Zeichnung (vorher 1693 von 10387 daneben).
* Keine neuen Assets, keine Sprachdateien, keine neuen Flags/Nachrichten.

### Nachbesserung 2 - Nebenwirkungen und gezielte Tests (KEINE volle Suite)
* Tuer-Uebergaenge (test_map_uebergang, RE15_UEBERGANG_LISTE=1, Bau mit allen Aenderungen): `[Sprung] 151 Uebergaenge auf
  gleichem Blatt: Median 6 px, Mittel 17 px, schlimmster 91 px, ueber 8 px: 62` - je Tuerpaar gegen die Liste der
  Abnahme 1 (G39abn1/uebergang_neu.txt): 132 Paare, **0 verschieden**.
* Marker ueber alle Raeume der Stage 1 (alle_raeume.py, jeder 3. Punkt): Polizeistation ueberall 0, ROOM1210 0/1141
  (vorher 207/1141); unveraendert nur 10B0 508/508 und 1260 266/266 (ohne Kartenblatt) und 11A0 Band 1 102/444 (Blatt 6).
* Sweep aller 13 Blaetter gegen Abnahme 1: 12 Blaetter pixelgleich, B2 nur im Kammer-Kasten (249 Pixel).
* Gezielter Lauf (Bau 6027bf4d-Stand, alle Karten-/Hinweis-/Inventar-Gruppen der Nachbesserung 1 + unit_1210_ fuer den
  Raum), 2026-10-18 ~23:10, `-j 4`:
```
ctest --test-dir re15_port/build -R "^(unit_map_|unit_karte_|unit_r30_karte|unit_r30_hinweis|unit_r33_karte|unit_r35_karte|unit_r35_cut10f0|integration_r35_cut10f0|unit_r36_karte|unit_r36_kartenbits|unit_r36_kartenitems|integration_r36_karte|integration_r36_kartenitems_bild|unit_r38_tagebuch|integration_r38_tagebuch|unit_r39_karten|integration_r39_karten|integration_r39_elza_karte|unit_r39_inventar|integration_r39_inventar|integration_map_uebergang|integration_map_raum_live|integration_r35_karte|unit_inv_|unit_r26_inventar|unit_r35_inventar1050|integration_r35_inventar1050|unit_r36_inventar|integration_r36_inventar|unit_r38_kleinkram_inventar|integration_r38_kleinkram_inventar|unit_pri_inventar_riegel|unit_r30_android_n1_inventar_cmd|unit_1210_)" -j 4 --timeout 900 --output-on-failure
100% tests passed, 0 tests failed out of 99
```
* Vertrag: keine Bank-9-Bits, keine Nachrichten-Ids, keine Slots; gemeinsame Datei re15_inv_screen.c nur EINE Zeile (Haken
  re15_karte_r39_fenster, "Runde 39 Spur G"); re15_map_zones.h = Karten-Code (Spur G); kein getenv, keine Aenderung an
  release/, platform/android/, shared_assets/, CMakeLists.txt; neue Tests nur ueber probes/r39_karten.cmake
  (integration_r39_karten-Skript cmake_minimum_required, exe-Kopie je Lauf, eigener Laufordner).

## Nachbesserung 3 (nach Abnahme 2, 2026-10-19)
Grundlage: analysis/befunde_runde39/G_abnahme_2.md (Mangel N2, Punkt 1a "Tuer immer genau auf Wand platziert").
N2: solange ROOM1030 (Eingangshalle) nicht gezeichnet ist, schweben seine Tuermarken neben der Wand des Nachbarraums -
(a) Tuer 1030<->1050 `{ 2, 4, 186, 152, 3, 4, 6, 1 }` (re15_map_zones.h:753) Balken x186 y150..154 links neben der
1050-Westwand x187; (b) Tuer 1030<->1040 `{ 2, 4, 119, 129, 3, 4, 5, 1 }` (re15_map_zones.h:730) Balken x119 y127..131
rechts neben der 1040-Ostwand x118. Riegel-Soll: in jedem erreichbaren Kartenzustand liegt jeder gezeichnete
Tuerbalken auf einer gezeichneten Wand (Einzelzustaende 1050/1040 ohne 1030; die uebrigen Einzelzustaende frei).
Arbeitsordner der Messungen: Scratchpad `nb3/` (nicht im Baum); Werkzeuge kommen nach `G_karten_werkzeug/nb3/`.

### Fortschritt
- [x] Messung vorher (echter Tuerweg 1090 -> 1050, Einzelzustand 1040 an der echten exe) - unten
- [x] RE: wie RE2 Retail Tuersymbole eines ungezeichneten Raums behandelt; Linien x186/x187, x118/x119 aus MAP03.PIX - unten
- [x] Sonde ueber ALLE Tuermarken x Zustaende (Teilmengen der beteiligten Raeume, mit/ohne Plan) - unten
- [x] Umsetzung (re15_karte_r39_tuer_lage; B2 1210-Westende ohne 11E0, Nebenbefund) - 5ce60f2e, df42c697
- [x] Messung nachher (echte exe + Sonde), Riegel + Gegenprobe - unten
- [x] gezielte Tests (100/100, danach 7/7), Abschluss

### N2 Messung vorher (Stand 4ea5e62c, Bau "ninja: no work to do" = exe der Abnahme 2)
Werkzeuge `G_karten_werkzeug/nb3/`: lauf_nb3.sh (= lauf.sh der Abnahme 2, eigene exe-Kopie re15_pc_g39nb3_<id>.exe),
tueren_bmp.py (Kriterium wie tuer2.py der Abnahme: Balken schwebt, wenn ein Ende auf dem Grund liegt oder quer daneben
ein Wandpixel steht). Arbeitsordner Scratchpad `nb3/`.
* (a) ECHTER TUERWEG Hof ROOM1090 -> ROOM1050 (Spielstand `1090 -10100 -1800 4200 1024 besucht:10F0:8400:-350
  besucht:1090:-13600:1300:5 besucht:1090:-10100:4200:1`, LOAD GAME, `RE15_INPUT_SCRIPT=W1,U0.3,W0.3,A0.1,W8`,
  Karte 40 Bilder nach der Ankunft): debug.log `[aot] DOOR FIRE slot=0 rect=(-10080,3060,hw=2350,hh=800) target_cut=6
  spawn=(19850,0,-22300)`, `[room] PC loaded room1050.rdt`; tueren_bmp.py: `6 Tuerbalken, schwebend 1 -
  x186..186 y150..154 Enden [0, 0] Wand quer 5 SCHWEBT`. Klassenbild (W Wand, B Koerper, D Tuer, . Grund):
  ```
       x180   x187
  y144 .......WWWWWWWW
  y149 .......WBBBBBBB
  y150 ......DWBBBBBBB      <- Balken x186 y150..154 LINKS neben der 1050-Westwand x187, im Grund
  y154 ......DWBBBBBBB
  y155 .......WWWWWWWW
  ```
* (b) ROOM1040 allein (Spielstand (-21008,0,-13134), nur 1040 besucht): `3 Tuerbalken, schwebend 1 - x119..119
  y127..131 Enden [0, 0] Wand quer 5 SCHWEBT`: Balken x119 RECHTS neben der 1040-Ostwand x118 (W x118 y125..134).
* Beides wie in der Abnahme 2 (Abschnitt 6), am selben Bau selbst reproduziert.

### N2 RE-Belege (selbst gelesen / disassembliert)
* **Die Kacheln (MAP03.PIX, Seiten-Tabelle 1F @0x8007636C, 12 B je Eintrag; G_karten_werkzeug/kachel.py + schwenk.py =
  die Schwenk-Tilgung des Renderers):** r0 ROOM1050 (180,69,32,96) uv(0,32) hat im Westen einen Stich y144..155 mit der
  Westwand x187 (Index 4 y144..155; der gemalte RE1.5-Schwenk x187..192 y147..151 wird vom Renderer getilgt);
  r4 ROOM1030 (119,125,72,64) uv(184,56) hat die Ostwand x186 (Index 4 ab y144) und im Westen einen Stich y125..134 mit
  der Westwand x119; r2 ROOM1040 (102,77,24,80) uv(88,48) hat im Osten den Stich y125..134 mit der Ostwand x118. Die
  Korrekturen s_korr (karte_r39.c, Nachbesserung 1 M3) loeschen x187 y145..154 (r0 Spalte 7 Zeilen 76..85) und x118
  y126..133 (r2 Spalte 16 Zeilen 49..56) NUR, wenn ihr Traeger r4 (ROOM1030) gezeichnet wird - dann ist x186 bzw. x119
  die eine Wand. Ohne r4 bleibt die eigene Linie x187 bzw. x118 stehen.
* **Die Marken** `{ 2, 4, 186, 152, 3, 4, 6, 1 }` / `{ 2, 4, 119, 129, 3, 4, 5, 1 }` (re15_map_zones.h:753/730) liegen auf
  der r4-Linie (x186 / x119), sind aber per auf_partner 1 schon sichtbar, wenn nur der Partner (zid2 6 = 1050 bzw. 5 =
  1040) besucht ist: re15_map_zones.c re15_map_mark_get prueft dann nur, ob ein GEZEICHNETES Rechteck den Punkt im KASTEN
  enthaelt - r0 (x180..211) bzw. r2 (x102..125) tun das, ihre Kachel traegt dort aber Index 0. Der Kopfkommentar der
  Tabelle verlangt fuer auf_partner 1 "die Marke liegt auf der GEMALTEN Flaeche des Partners" - fuer diese zwei Marken
  stimmt das nur, solange r4 gezeichnet ist.
* **RE1.5** malt immer alle Rechtecke einer Seite (FUN_80046fd8 Schleife @0x800472fc-0x800473e0) und fuehrt keine
  Tuermarken - die Tuer 1030<->1050 ist dort nur der gemalte Schwenk in r0; Massstab fuer die Port-Marken ist RE2
  (Nutzer 2026-09-02: RE2-Kartenstil, re15_inv_screen.c RE2_TUER_*).
* **RE2 Retail (Beta -> Retail): jede Raum-Kachel traegt die Tuer auf ihrer EIGENEN Wand.** nb3/re2_tueren.py liest
  info/re2leon/PSX.EXE Area-Tabelle @0x800aaa38 (Area 2 = RPD 1F: RoomTable 0x800aa388, 18 Raeume, MAPS_005.TIM) und die
  Kartenseite: die Tuer zwischen Raum [1] (@0x800aa398, uv(24,80) 32x25, Schirm (99,139)) und Raum [0] (@0x800aa388,
  uv(0,112) 92x90, Schirm (127,92)) ist ZWEIMAL gemalt - Raum 1 Index 3 auf x125..126 y147..154 an seiner Ostwand x126,
  Raum 0 Index 3 auf x127..128 y147..154 an seiner Westwand x127 (das jeweils andere Sprite hat dort Index 0):
  ```
        x120     x133        Raum 1          Raum 0
  y146  1111114....          ......4111111
  y147  1111133....          ......3311111   <- Tuerhaelfte je Raum, auf der eigenen Wand
  y154  1111133....          ......3311111
  y155  1111114....          ......4111111
  ```
  Ebenso JEDE der 19 Tueren zwischen zwei Raeumen der Area 2 (Paare 0/1, 0/2, 0/10, 1/4, 2/9, 3/4, 4/5, 5/6, 5/7, 7/8,
  7/9, 10/11, 10/12, 11/15, 11/17, 12/13, 12/14 zweimal, 15/16): je Raum eine Haelfte an der eigenen Wand; einseitig
  nur die Aussentuer von Raum 0 (x151..158 y171..173). Gezeichnet wird ein
  Raum-Sprite mit seinen Tuerhaelften nur als Ganzes (FUN_8006e120 Raumschleife): Plan-Bit `lbu a1,-21955(at)`
  (0x800aaa3d+8*Area) @0x8006e660, `jal 0x80077360` auf Bank 0x800d4924 @0x8006e66c; besucht `lbu a1,12(s1)` @0x8006e67c /
  @0x8006e730, `jal 0x80077360` auf 0x800d490c (a0 = 0x800d4924 - 24) @0x8006e688 / @0x8006e73c; ohne Plan und unbesucht
  `beq v0,zero,0x8006e768` @0x8006e744 = kein Sprite; mit Plan unbesucht CLUT 498 `addiu s5,zero,498` @0x8006e71c
  (Umriss); `jal 0x8008f828` (GetClut) @0x8006e750, `jal 0x8008f918` (AddPrim) @0x8006e760. Es gibt kein eigenes
  Tuer-Prim (analysis/karte_2026-08-31/C_re2_karte.md 3.1). => In RE2 steht eine Tuer IMMER auf der Wand des Raums, der
  gezeichnet wird; ist nur einer der beiden Raeume gezeichnet, steht sie auf DESSEN Wand.
* Folgerung fuer den Port (eine Linie im Vollzustand nach den Vorlagen, Nachbesserung 1 M3): die Tuermarke gehoert auf
  die Linie, die TATSAECHLICH gezeichnet wird - die gemeinsame (Traeger-)Linie, wenn die Korrektur greift, sonst die
  eigene Linie des gezeichneten Nachbarn (x187 bzw. x118). Sichtbarkeit unveraendert (RE2 zeigt die Tuer mit jedem der
  beiden Raeume; der Nutzer hat fehlende Tueren an genau dieser Wand schon einmal gemeldet - Runde 7, ROOM1050).

### N2 Sonde ueber alle Zustaende (Messung vorher, Bau ohne den Fix)
`probe_r39_karten tueren 0 4` (neu, test_r39_karten.c t3_blatt): je Tuermarke der Blaetter B1..3F die Orte, deren
Zeichnung den Balken +-3 px beruehrt, plus die Orte ihrer zid/zid2; JEDE nichtleere Teilmenge besucht, jeder davon einmal
aktuell, ohne/mit Plan des Blatts (Bank 9: 78/77/89/88/87, gen/kartenitems_r36.inc) - 798 Zustaende, 3498 gezeichnete
Balken; Kriterium wie tuer2.py (Ende auf Grund / Wand quer daneben, quer erst ab 3 von 5 wegen der ungetilgten RE1.5-
Schwenke im Op-Listen-Bild) plus "unter dem Balken < 4 von 5 Wand". VORHER 3 Befunde:
```
Blatt 1 Balken x186..186 y72..76: Ende auf Grund nicht auf der Wand in 21 Zustaenden, z.B. 1210(-19352,-13637)
Blatt 2 Balken x119..119 y127..131: Ende auf Grund Wand quer daneben nicht auf der Wand in 17 Zustaenden, z.B. 1040(-24176,2824)
Blatt 2 Balken x186..186 y150..154: Ende auf Grund Wand quer daneben nicht auf der Wand in 38 Zustaenden, z.B. 1050(19400,-4272)
```
Die beiden 1F-Befunde sind N2 (a)/(b) - alle 38 bzw. 17 Zustaende ohne gezeichnetes ROOM1030. Der B2-Befund (Tuer
11E0<->1210) tritt nur ohne gezeichnetes ROOM11E0 auf: im Spiel nicht als besuchter Zustand erreichbar -
nb3/tuer_quellen.py: Door_aot_set nach ROOM1210 nur in ROOM11E0.RDT @0x015A2/@0x016C4 und ROOM1220.RDT
@0x00DD2/@0x00DF2/@0x00E12/@0x00E32/@0x00E52, nach ROOM1220 nur in ROOM1210.RDT @0x01CE6..@0x01D66 (Elza: 11E1/1221/1211
dieselben Offsets). Tuer-Korrektur-Nachbarschaft (nb3/tuer_korr.py): 8 Tuermarken liegen direkt neben geloeschten
Texeln einer Korrektur - die zwei N2-Marken (Traeger r4 = ROOM1030, eigenes Rechteck ebenfalls r4), drei mit dem
eigenen Rechteck als Traeger (1F (163,81)/(163,106) an r8/r7, (119,151) an r2 - nur mit ihm sichtbar, also nie
schwebend), drei an R39_IMMER (B2 (186,74), 3F (145,84)/(153,126)).

### N2 Nebenbefund beim Durchgehen der Zustaende: Westende ROOM1210/1211 offen, wenn ROOM11E0 fehlt (Hinweis-Zustand)
Die Korrektur `{ 1, 3, 0, 2, 1, 5, 1, R39_IMMER }` (B2 r3 x187 y72..76 -> Koerper, ba258ae4) war als "immer" gefuehrt,
weil ROOM1210 nur durch ROOM11E0 erreichbar ist. Es gibt aber einen ERREICHBAREN Zustand, in dem r3 OHNE r0 gezeichnet
wird: der Kartenhinweis nach Roys Tagebuch (Elza; map_hint_common.c s_hints Eintrag 4: Ziel ROOM1211 Zone 0 = B2 r3,
blinkt bis (4,0xFC); B2 dadurch waehlbar, re15_map_ziel_blatt_frei).
* Kachel (kachel.py/schwenk.py): MAP02.PIX r3 uv(96,48) @0x80076318 traegt die Westwand x187 y70..78 (Spalte 0 Zeilen
  0..8 Index 4) mit dem gemalten Schwenk x187..192 y72..76; die Schwenk-Tilgung des PC-Renderers (inv_render_pc.c
  re15_schwenke_entfernen, `const int WAND = 4, S_SAAT = 4, S_WACHS = 10, MAX_AUSD = 10`) nimmt den nur 9 px langen
  Lauf mit: x187 y71..77 durchsichtig (im Vollzustand schliesst die 11E0-Ostwand x186, Kerben (187,71)/(187,77) =
  bekannter Hinweis der Abnahme 1).
* ECHTE exe (lauf_nb3.sh, Spielstand ROOM1020 (-4000,0,-18000) + flag:9:68, LOAD GAME, Karte, `RE15_INPUT_SCRIPT=
  W1.5,D0.1,W3` = einmal RUNTER von 1F -> B2 (Reihenfolge B2 B1 1F, menu_common.c ORDER; B1 unbekannt), Abzug Bild 150):
  debug.log `[inv] Blatt 1: 6 RE1.5-Tuerschwenke entfernt (79 Pixel)`; x187 y70..78 = Wand, 7x Grund (0,16,88), Wand -
  die Westseite der blinkenden Zielkachel ist OFFEN. Sonde (`probe_r39_karten zielbild 1 3 1 ...`, rote Phase, ohne
  Tilgung): x187 y72..76 Koerper am Grund (5 Pixel).
* RE2 (selbst disassembliert, re2_disasm.py): der Hinweis-Zeichner FUN_8006F1C4 zeichnet das ZIEL-Sprite als Ganzes,
  besucht oder nicht - `bne s4,a2,0x8006f518` @0x8006f4e8 (Zielraum), Blinkphase `lbu v0,0x5c19(v0)` @0x8006f4f4, rote
  Phase `j 0x8006f604` @0x8006f510 mit `addiu s2,s2,1` (CLUT +1), Umriss `addiu s2,zero,498` @0x8006f5dc, beide ->
  `jal 0x8008f828` (GetClut) @0x8006f608 -> AddPrim. Das Sprite traegt seine eigene Wand und Tuerhaelfte (RoomTable
  @0x800aa388, s.o.) - eine Zielkachel ist in RE2 deshalb immer geschlossen.

### N2 Umsetzung (Dateien, Konstanten)
* `engine/src/karte_r39.c` NEU `re15_karte_r39_tuer_lage(st, page, kind, &mx, &my)`: eine Tuermarke, deren um eine
  Spalte/Zeile quer verschobener 5-px-Balken GANZ in den geloeschten Texeln einer bedingten Korrektur liegt (s_korr,
  neu != 4, Traeger != R39_IMMER), steht nur dann auf der Ersatzlinie, wenn der Traeger gezeichnet wird (dasselbe Gatter
  traeger_gezeichnet wie die Korrektur, Nachbesserung 1 M3); sonst eine Spalte weiter auf der eigenen Linie, die dann
  noch steht. Keine neue Konstante - Geometrie aus s_korr + Seiten-Tabelle @0x80076840. Treffer: 1F (186,152) -> x187
  ohne r4 (r0 Spalte 7 Zeilen 76..85 = x187 y145..154), 1F (119,129) -> x118 ohne r4 (r2 Spalte 16 Zeilen 49..56 =
  x118 y126..133), B2 (186,74) -> x187 ohne r0 (r3 Spalte 0 Zeilen 2..6).
* `engine/src/karte_r39.c` s_korr B2: `{ 1, 3, 0, 2, 1, 5, 1, 0 }` (Traeger r0 statt R39_IMMER) und NEU
  `{ 1, 3, 0, 1, 1, 7, 4, R39_OHNE(0) }` = x187 y71..77 Wand, wenn r0 (ROOM11E0) NICHT gezeichnet wird (die gemalte
  Linie Spalte 0 Zeilen 1..7 Index 4, die die Tilgung nahm). NEU `R39_OHNE(t)` = 64 + t im Traeger-Feld,
  traeger_gezeichnet liefert dafuer "t nicht gezeichnet" (ohne Schirmzustand: nie). Vollzustand (r0 gezeichnet):
  dieselben Texel wie vorher.
* `engine/src/re15_inv_screen.c`: ZWEI Haken je eine Zeile "Runde 39 Spur G" - Tuer-Zeichner (nach re15_map_mark_get)
  und tuer_anziehen (Marker-Zug zur Tuer, derselbe Punkt wie der gezeichnete Balken; wirkt nur auf Schema-Zonen, keine
  der drei Treffer-Tueren ist eine - Zug-Ziel unveraendert).
* `include/re15_karte_r39.h`: Deklaration.
* Sichtbarkeit der Marken unveraendert (re15_map_mark_get): RE2 zeigt die Tuer mit JEDEM der beiden Raeume.

### N2 Messung nachher (Bau 580928bc-Stand, echte exe + Sonde)
* (a) ECHTER TUERWEG Hof ROOM1090 -> ROOM1050 (derselbe Spielstand und dieselbe Eingabe wie vorher): debug.log
  `[aot] DOOR FIRE slot=0 rect=(-10080,3060,hw=2350,hh=800) target_cut=6 spawn=(19850,0,-22300)`, `[room] PC loaded
  room1050.rdt`; `probe_r39_karten tuerbild`: alle 6 Balken ok, `Balken x187..187 y150..154 ok (Wand quer 0)` - der Balken
  steht IN der 1050-Westwand x187 (y144..149 und y155 Wand). Gegen den Vorher-Abzug: 10 Pixel verschieden, alle im Kasten
  x186..187 y150..154. Bild `G_karten_bilder/n2_tuer_1030_1050_vorher_nachher_x6.png` (links vorher: gelb im Blau neben
  der grauen Wand; rechts nachher: gelb in der Wand).
* (b) ROOM1040 allein: Balken x118 y127..131 in der 1040-Ostwand (y125/126 und y132..134 Wand), x119 Grund; gegen den
  Abzug der Abnahme 2 (G39abn2/al_1040) 10 Pixel im Kasten x118..119 y127..131. Bild
  `G_karten_bilder/n2_tuer_1030_1040_vorher_nachher_x6.png`.
* ALLE EINZELZUSTAENDE an der echten exe (nb3/allein_nb3.sh = Liste und Weg der Abnahme 2, 46 Lagen; 10F0/1170/11C0/1240
  oeffnen die Karte wegen ihrer Eintritts-Szene nicht, wie in der Abnahme): 42 Abzuege, tueren_bmp.py 40x `schwebend 0`;
  uebrig nur die zwei Faelle, die die Abnahme 2 als "kein Schweben" gewertet hat (1090 obere Ebene: Balken x197..201 y114
  auf dem Eckpixel der Suedwand; 1130: Balkenrest unter dem Spieler-Ring). Gegen die Abzuege der Abnahme 2 (G39abn2/al_*)
  pixelgleich bis auf: al_1050 10 px (x186..187 y150..154), al_1040 10 px (x118..119 y127..131), al_1210 12 px
  (x186..187 y71..77: Tuer auf x187, Wand (187,71)/(187,77) - der Zustand ist im Spiel unerreichbar).
* VOLLZUSTAND: Sweep aller 13 Blaetter (Spielstand ROOM1190, RE15_MAP_SHOT_SWEEP) gegen G39abn2/neu_S: **0 Pixel
  verschieden auf allen 13 Blaettern** (GESAMT 0) - die Vorlagen-Zustaende (1F/2F/3F/B2) sind unberuehrt. ROOM1090
  Nutzerpunkt (-12528,-9000,959), Sweep gegen G39abn2/neu_M: 0 Pixel auf allen 13 Blaettern (Punkt 2 unveraendert).
  Lauf K (ROOM1210 Flur -> Kammer, W1,U4,W5) gegen G39abn2/w1210: 0 Pixel, `pl pos=(-25500,0,-21700)`.
* TUER-UEBERGAENGE: `RE15_UEBERGANG_LISTE=1 test_map_uebergang.exe`: `[Sprung] 151 Uebergaenge auf gleichem Blatt: Median
  6 px, Mittel 17 px, schlimmster 91 px, ueber 8 px: 62`; je Paar gegen die Liste der Abnahme 2 (ueb_vgl2.py):
  **132 Paare, 132 gleich, 0 verschieden**.
* NEBENBEFUND B2-Zielkachel (Spielstand ROOM1020 + flag:9:68, RUNTER auf B2): Umriss-Phase (Bild 150/180)
  `WAND 7 von 7 (x187 y71..77)` (vorher 0 von 7); rote Phase (Bild 120) `WAND 7 von 7`, `offen 0` (vorher `7 Koerperpixel
  am Grund (offene Raumseite), erstes bei (188,71)`). Bild `G_karten_bilder/n2_b2_zielkachel_1211_vorher_nachher_x6.png`.
* SONDE `probe_r39_karten tueren 0 4`: 798 Zustaende, 3498 Balken, **0 Befunde** (vorher 3).

### N2 Riegel und Gegenprobe
* NEU `unit_r39_karten_tueren` (probes/r39_karten.cmake, `probe_r39_karten pruef tueren`): (a) die Sonde ueber alle
  798 Zustaende der Blaetter B1..3F - 0 Befunde; (b) 1050 allein / 1040 allein Pixel fuer Pixel (Balken 5/5 auf x187 bzw.
  x118, Enden Wand 2/2, x186 bzw. x119 frei) und 1050+1030 / 1040+1030 (Balken auf der 1030-Linie wie map1F_correct.png,
  die eigene Linie Koerper 5/5); (c) jede Hinweis-Zielkachel (map_hint_common.c s_hints, 5 Eintraege) allein gezeichnet
  geschlossen, ROOM1211 Westwand x187 y71..77 7/7.
* `unit_r39_karten_teil`: (a) prueft die R39_OHNE-Korrektur (eigene gemalte Linie Index 4 + Linie des Traegers daneben);
  (b) "1210 allein": Tuer auf der eigenen Westwand x187 y72..76 5/5, Wand (187,71)/(187,77), x186 frei, 0 offene Pixel
  (vorher dort "soll <= 4" wegen der Kerben und der Tuer auf x186).
* `integration_r39_karten` + Laeufe NA (echter Tuerweg 1090 -> 1050: debug.log `PC loaded room1050`, `tuerbild` im Kasten
  und im ganzen Kartenfeld ohne Schweben, `Balken x187..187 y150..154 ok`), NB (1040 allein: `Balken x118..118 y127..131
  ok`), NZ (B2 nach Roys Tagebuch: `[inv] Blatt 1:` im debug.log, `WAND 7 von 7`).
* GEGENPROBE (die drei Engine-Dateien voruebergehend auf 4ea5e62c zurueckgesetzt, gebaut, Riegel gefahren, wieder
  hergestellt und neu gebaut): `pruef tueren` ROT - 3 Befunde (die drei von vorher), "1050 allein ... x187 0/5 ... x186
  5 belegt", "1040 allein ... x118 0/5", "Hinweis 4 ... 5 Koerperpixel am Grund, erstes (187,72)", "Westwand x187 y71..77
  2/7"; die Vollzustands-Pruefungen 1050+1030 / 1040+1030 auch vorher gruen (unveraendert). `pruef teil` ROT nur an
  "1210 allein ... x187 (0 von 5) ... auf x186 (5)". An den Abzuegen der echten exe: `tuerbild` auf den Vorher-Abzuegen
  `Balken x186..186 y150..154 SCHWEBT (Wand quer 5)` / `Balken x119..119 y127..131 SCHWEBT (Wand quer 5)` -> FAIL,
  `wand` auf dem Vorher-Abzug NZ `WAND 0 von 7`.

### Nachbesserung 3 - Uebersicht
| Mangel | Ursache | Beleg | Aenderung | vorher -> nachher |
|---|---|---|---|---|
| N2 (a) Tuer 1030<->1050 ohne ROOM1030 | Marke auf der 1030-Linie x186 (eine Wand im Vollzustand), per auf_partner schon mit 1050 sichtbar; ohne 1030 greift die Korrektur r0 x187 nicht (M3) und die 1050-Linie x187 steht | Kachel MAP03.PIX r0/r4 (Seiten-Tabelle 1F @0x8007636C), s_korr r0 Spalte 7 Zeilen 76..85; RE2: Tuer je Raum-Kachel auf der eigenen Wand (RoomTable @0x800aa388, Raum 1 @0x800aa398 x125..126 / Raum 0 x127..128; FUN_8006e120 @0x8006e66c/@0x8006e688/@0x8006e760) | re15_karte_r39_tuer_lage: Tuer auf der Ersatzlinie nur mit gezeichnetem Traeger, sonst auf der eigenen Linie (Haken Tuer-Zeichner + tuer_anziehen) | echter Tuerweg 1090 -> 1050: x186 SCHWEBT (Enden Grund, Wand quer 5) -> x187 in der Wand, ok |
| N2 (b) Tuer 1030<->1040 ohne ROOM1030 | dasselbe an r2 x118 / r4 x119 | s_korr r2 Spalte 16 Zeilen 49..56 | dieselbe Regel | 1040 allein: x119 SCHWEBT -> x118 in der Wand, ok |
| Nebenbefund: B2-Westende ROOM1210/1211 ohne ROOM11E0 | R39_IMMER-Korrektur r3 x187 y72..76 + Schwenk-Tilgung (S_WACHS 10) nimmt die 9-px-Westwand x187 y71..77 mit; erreichbar als Hinweis-Ziel nach Roys Tagebuch (B2 r3 blinkt bis (4,0xFC)) | MAP02.PIX r3 @0x80076318 Spalte 0 Zeilen 0..8 Index 4; inv_render_pc.c Schwellen; RE2-Hinweiszeichner FUN_8006F1C4 `bne s4,a2` @0x8006f4e8 -> GetClut @0x8006f608 (Ziel-Sprite als Ganzes) | Koerper x187 y72..76 nur mit r0; NEU R39_OHNE(0): x187 y71..77 Wand ohne r0; Tuer 11E0<->1210 folgt der Linie | echte exe B2 nach dem Tagebuch: Westwand 0/7 (rote Phase 7 Koerperpixel am Grund) -> 7/7, offen 0 |

### OFFEN (Stand Nachbesserung 3)
* Aus N2 bleibt nichts offen: an der echten exe 0 schwebende Tueren in allen 42 abziehbaren Einzelzustaenden (die zwei
  Treffer 1090-Eckpixel und 1130-Ringrest wie in der Abnahme 2 "kein Schweben"), Sonde 0 von 798 Zustaenden.
* Vorbestehend und unveraendert (Hinweise der Abnahmen 1/2): Kerben (187,71)/(187,77) der Schwenk-Tilgung am Tuerbalken
  11E0<->1210 im Vollzustand (mit ROOM11E0 gezeichnet; Vorlage mapB2_correct.png gleich); Kachel-Klemmung reserve 4
  (H2); 1130-Wandnasen (H4); 11A0 Band 1 auf Blatt 6; 10B0/1260 ohne Kartenblatt.
* Der Zustandsraum der Sonde sind besuchte Teilmengen (mit/ohne Plan) und die Hinweis-Zielkacheln allein; Hinweis-Ziel
  PLUS besuchte Nachbarn ist dieselbe Kachel mit gezeichnetem Nachbarn (= Vollzustand dieser Wand, schon geprueft).

### Fuer den Nutzer (Nachbesserung 3)
* **1F: die Tueren der Eingangshalle stehen jetzt auch dann auf der Wand, wenn du die Halle noch nicht betreten hast.**
  Vom Hof ueber die Tuer nach ROOM1050 (der normale erste Weg ins 1F, der Plan liegt ja erst in der Halle) schwebte die
  Tuer zur Halle einen Pixel links NEBEN der 1050-Wand im Leeren; ebenso in ROOM1040 rechts neben der Ostwand. Jetzt
  sitzt sie in der Wand des Raums, der gezeichnet ist - so macht es auch Resident Evil 2 (jeder Raum traegt die Tuer auf
  seiner eigenen Wand). Sobald die Halle gezeichnet ist, steht die Tuer wie in deiner Vorlage map1F_correct.png auf der
  EINEN gemeinsamen Wand - dabei rueckt sie um einen Pixel mit der Wand mit. Bilder:
  analysis/befunde_runde39/G_karten_bilder/n2_tuer_1030_1050_vorher_nachher_x6.png und n2_tuer_1030_1040_vorher_nachher_x6.png
  (links vorher, rechts jetzt).
* **B2 bei Elza nach Roys Tagebuch:** das Gefaengnis (ROOM1211) blinkt als Ziel auf B2; solange du den Gang davor
  (ROOM11E1) noch nicht kennst, fehlte an seinem linken Ende die Wand (7 Pixel offen). Jetzt geschlossen. Bild:
  analysis/befunde_runde39/G_karten_bilder/n2_b2_zielkachel_1211_vorher_nachher_x6.png.
* Sonst aendert sich nichts: alle 13 Kartenblaetter im aufgedeckten Zustand und ROOM1090 sind pixelgleich zur Abnahme 2,
  der Marker springt an keiner Tuer anders (132 Tuerpaare gleich).
* Keine neuen Assets, keine Sprachdateien, keine neuen Flags/Nachrichten, keine Entscheidungsfrage.

### Nachbesserung 3 - Bau und gezielte Tests (KEINE volle Suite)
Bau `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`. Gezielter Lauf (die Gruppen der
Nachbesserung 2 - alle Karten-/Hinweis-/Inventar-Gruppen, die der Diff beruehrt, + unit_1210_; mit `ctest -N` gegen alle
Namen mit karte/map/hinweis/rundlauf abgeglichen: keiner fehlt), 2026-10-19 ~01:35..01:47, `-j 4`:
```
ctest --test-dir re15_port/build -R "^(unit_map_|unit_karte_|unit_r30_karte|unit_r30_hinweis|unit_r33_karte|unit_r35_karte|unit_r35_cut10f0|integration_r35_cut10f0|unit_r36_karte|unit_r36_kartenbits|unit_r36_kartenitems|integration_r36_karte|integration_r36_kartenitems_bild|unit_r38_tagebuch|integration_r38_tagebuch|unit_r39_karten|integration_r39_karten|integration_r39_elza_karte|unit_r39_inventar|integration_r39_inventar|integration_map_uebergang|integration_map_raum_live|integration_r35_karte|unit_inv_|unit_r26_inventar|unit_r35_inventar1050|integration_r35_inventar1050|unit_r36_inventar|integration_r36_inventar|unit_r38_kleinkram_inventar|integration_r38_kleinkram_inventar|unit_pri_inventar_riegel|unit_r30_android_n1_inventar_cmd|unit_1210_)" -j 4 --timeout 900 --output-on-failure
100% tests passed, 0 tests failed out of 100
```
Danach nur der Kopfkommentar von re15_karte_r39.h (Traeger-Kodierung) ergaenzt, neu gebaut, Spur-G-Riegel nachgefahren:
```
ctest --test-dir re15_port/build -R "^(unit_r39_karten|integration_r39_karten)" -V --timeout 900
100% tests passed, 0 tests failed out of 7
```
(darin `r39_karten[NA] ... Balken x187..187 y150..154 ok`, `r39_karten[NB] ... Balken x118..118 y127..131 ok`,
`r39_karten[NZ] ... WAND 7 von 7 (x187 y71..77)`).
Vertrag: keine Bank-9-Bits, keine Nachrichten-Ids, keine Slots; gemeinsame Datei re15_inv_screen.c zwei Haken je eine
Zeile ("Runde 39 Spur G"); karte_r39.c/re15_karte_r39.h = Karten-Code der Spur; kein getenv; keine Aenderung an
release/, platform/android/, shared_assets/, CMakeLists.txt; neue Tests nur ueber probes/r39_karten.cmake
(integration_r39_karten-Skript mit cmake_minimum_required, exe-Kopie je Lauf, eigener Laufordner je Lauf).
