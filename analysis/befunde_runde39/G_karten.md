# Runde 39 Spur G "karten" — Dossier (fortlaufend)

Baum: .claude/worktrees/r39_karten (Zweig r39/karten, Basis 1038dc15). Start 2026-10-18.

## Punkte (Wortlaut AUFTRAG.md)
1. "Bei den Maps des Police Departement passt es noch nicht so ganz. [...] map3F_correct.png, map2F_correct.png,
   map1F_correct.png, mapB2_correct.png. Dort ist immer links, wie es bei uns aussieht und rechts wie es eigentlich
   aussehen sollte. Keine Doppelwände, Tür immer genau auf Wand platziert, treppen nicht in der Wand platziert usw. Und
   natürlich muss der Spielermarker da entsprechend passen. Der Room B1 den habe ich nicht gemacht. [...] Die Karte von B1
   ist noch ziemlicher schrott."
2. "In ROOM 1090 ist Leons Positionsmarker der Map außerhalb des aktuellen Raums."

## Fortschritt
- [x] Bestand lesen (Karten-Code, Memory, Runde 36/33/27-28)
- [ ] Messung vorher (FB_SHOT je Etage, Marker 1090)
- [ ] RE-Belege
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests

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


## RE-Belege
(folgt)

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
