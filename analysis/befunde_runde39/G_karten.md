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
- [ ] Messung vorher B1 "wie man sich bewegt" (Bau mit dem Kopf von 10876ebb), Echtlauf-Abzug B1, Abschluss

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

## Umsetzung
(folgt)

## Messung nachher
(folgt)

## Tests
(folgt)

## OFFEN
(folgt)

## Fuer den Nutzer
(folgt)
