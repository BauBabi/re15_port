# Bericht Strang "karte", Stufe 1, Runde 2

Zweig r42/karte, Commit **b0a9ddbb9** (auf cb4f078c1, Basis aaea5f8c2). Kein Push.

## Kurz und in Alltagssprache

- **Der Punkt "wo ist Leon" laeuft jetzt in keinem Raum mehr seitenverkehrt.** In der Vorrunde lief er in
  drei Raeumen spiegelverkehrt (30A0, 20A0, 3050). Ein Raum kann auf der Karte nur gedreht liegen, nie
  gespiegelt. Jetzt sind nur noch Drehungen erlaubt, und jede Drehung ist mit mindestens zwei Hinweisen
  belegt.
- **Drei Raeume liegen um eine Vierteldrehung verdreht auf dem Blatt** (3050, 30A0 und 20A0). Das konnte die
  Karte bisher nicht darstellen. Der Port kann jetzt "Achsen tauschen". Im echten Spiel nachgemessen: Geht
  Leon in 30A0 nach Osten, wandert der Punkt nach oben, und genau dort unten liegt die Tuer, durch die er
  hereinkam.
- **Die Treppenzeichen sind wieder da**, auch die beiden von Raum 3040, die in der Vorrunde faelschlich
  geloescht wurden. Dazu kommen Treppen in 3010, 30C0, 2030 und 2080.
- **Keine doppelten Tuerzeichen mehr.** Wo das Original eine Tuer selbst gemalt hat, sitzt das gelbe
  Tuerzeichen jetzt genau darauf (wie in STAGE1). Auf den Blaettern 6 und 7 tragen 20 von 21 gemalten Tueren
  ihr Zeichen. Bei der einen fehlenden fuehrt im Spiel keine Tuer dorthin.
- **Der Gang 30E0 (zwischen 3050 und 3030)** hat jetzt den Punkt an der richtigen Stelle: im unteren
  Streifen der Kachel von Raum 3040. Der Kuenstler hat den Gang dort mit hineingemalt. Deshalb leuchtet
  im Gang die ganze 3040-Kachel auf.
- **Ehrliche Zahlen:** Die Kennzahl "Punkt auf der eigenen Kachel" nenne ich nicht mehr als Erfolg. Sie
  war bei abgeleiteten Raeumen von vornherein erfuellt. Gemessen wird jetzt an Dingen, die die Herleitung
  nicht kennt: an den gemalten Tueren und an den Zeilen, die das Spiel selbst mitliefert. Der Punkt an
  einer Tuer liegt im Median 4 px neben der gemalten Tuer, hoechstens 8 px.
- STAGE4 bis 6 sind noch nicht repariert (das ist Stufe 2). Fuer sie gibt es jetzt aber auch Abzuege aus
  dem echten Spiel.

Fachwoerter: "Blatt" = eine Kartenseite. "Rechteck"/"Kachel" = ein gemalter Raum auf dem Blatt. "Zeile" =
Umrechnung von der Spielposition auf den Kartenpunkt. "Drehung" = Raum auf dem Blatt gedreht (0, 90, 180,
270 Grad). "Spiegel" = links und rechts vertauscht (unmoeglich). "Riegel" = automatischer Test, der einen
Fehler festhaelt. "FB_SHOT" = Abzug des Kartenbildschirms aus dem echten Spiel. "Schleier" = Raeume, die
man noch nicht besucht hat, sind auf der Karte nicht zu sehen.

## Die roten Punkte der Vorrunde

**R1 Spiegelverkehrter Marker** - behoben.
- Neues Feld `tausch` am Ende von `re15_map_zone_t` (re15_room.h) und in `re15_map_zone_marker`
  (re15_map_zones.c): Karten-x aus Welt-z, Karten-y aus Welt-x. PORT-ERGAENZUNG; die ausgelieferten Zeilen
  (FUN_800473f8 @0x8004741c-0x80047528, sx/sy per lhu, feste Vorzeichen) tragen immer 0. Ohne Tausch gilt
  flip_x == flip_z, mit Tausch flip_x != flip_z - nur dann ist die Abbildung eine Drehung.
- `tools/karte_r42_zeilen.py` neu: die Drehung steht je Raum im Plan und wird gegen die Tueren GEPRUEFT.
  Jede Tuer gibt ihre Weltseite (Trigger-Form und Lage, gegengeprueft am Ankunfts-Yaw des Gegen-Datensatzes;
  bei quadratischem Trigger in einer Ecke die Ecke) und unabhaengige Kartenseiten: (a) gemaltes Tuersymbol an
  der Grenze zu genau diesem Nachbarn, (b) Gegen-Tuer mit AUSGELIEFERTER Zeile, (c) Lage des
  Nachbar-Rechtecks. Verlangt: mindestens zwei Stimmen (zwei Tueren oder zwei Belegarten), keine dagegen,
  und der Schnitt aller Stimmen ist genau diese Drehung. Ergebnis: alle 17 hergeleiteten Orte BELEGT, nur
  20A0 ist mit einer Tuer eine benannte PORT-WAHL (Tabelle in BESTAND.md §3).
- Riegel [E]: alle 226 Zonen aller Blaetter haben die Haendigkeit der ausgelieferten Zeilen. Riegel [F]: an
  17 Tueren laeuft der Marker auf den unabhaengigen Beleg zu und endet 2-8 px daneben (Median 4).
- Im echten Spiel (FB_SHOT, Messbilder `messbild_richtung_blatt6_r2.png` / `_blatt7_r2.png`), Erwartung aus
  der Zonentabelle in Klammern:
  - 30A0 bei z=11700: x -25500 -> (241,152) (241,152), x -18000 -> (241,135) (241,135): Osten = nach oben;
    bei x=-22000: z 9000 -> (244,144) (245,144), z 19000 -> (231,144) (229,144): Norden = nach links.
    Die Tuer zu 3090 sitzt auf der Suedwand von r10 (242,155) = der Westwand des Raums.
  - 3050: x -23000 -> (163,54), x -5000 -> (163,89): Osten = nach unten; z -20000 -> (145,70), z 0 ->
    (182,70): Norden = nach rechts (Erwartung je 0-1 px).
  - 20A0: Osten = nach oben (148 -> 144), Norden = nach links (285 -> 281).
  - 2010 / 2050 (R180): Osten = nach links (85 -> 70 / 127 -> 101); 2070 Zone 0 und 30E0 Zone 0 (R0): Osten =
    nach rechts (117 -> 153 / 199 -> 228).

**R2 Kennzahl misst sich selbst** - berichtigt. Der Kopf von `test_r42_karte_st23.c` sagt jetzt, dass [B] und
[C] bei hergeleiteten Zeilen per Konstruktion erfuellt sind. Als Guetemass nenne ich nur Groessen
ausserhalb des Fits: Tuer -> gemaltes Symbol / ausgelieferte Nachbar-Zeile ([F], Median 4 px, max 8 px),
Treppen ([G]) und gemalte Tuersymbole mit Marke ([H]). Die Tuer-Belege waehlen die DREHUNG (diskret), nicht
Versatz und Massstab - das steht so im Testkopf und im Werkzeug.

**R3 Treppenmarken geloescht** - zurueck, aus den SCD-Zonen Aot_set Typ 12/13 (Handler LAB_80043500 /
LAB_800435cc; Paare wie gen_map_zones.py: gleiche Achse und Stufenzahl, Bandabstand = count, Weltmitte):
3040 (203,75) und (231,76) und 3010 (135,188) mit ihren AUSGELIEFERTEN Zeilen @0x80076A60 / @0x80076A48 -
bytegleich mit den Marken vor Runde 1. Dazu 30C0/30D0 (189,131), 2030 (187,124), 2080 (226,155). Bei
Achsentausch drehen sich die Sprossen mit. OFFEN: die 2040-Treppe (-1350,-16100) im ungemalten Suedteil.

**R4 "STAGE3 behoben" war ueberzogen** - die drei Punkte:
- 30E0 Zone 0: jetzt im Suedband von r4 (PORT-ERGAENZUNG R0, beide Tueren belegt). Die ausgelieferte
  30E0-Zeile @0x80076AB0 gilt nur fuer Zone 1; fuer Zone 0 legt sie die Tueren 30/38 px neben die
  Gegenstuecke. Preis: r4 (3040) leuchtet im Gang mit auf. Der Nordarm des L-Gangs hat keine eigene Kunst.
- 3050 um 90 Grad: siehe R1 (R90cw, sieben Stimmen, keine dagegen).
- 3040<->3060: die Marke sitzt auf dem gemalten Doppeltuer-Symbol (220,104) statt bei (237,104). Die
  beiden AUSGELIEFERTEN Zeilen legen diese Tuer selbst 13 bzw. 21 px neben das gemalte Symbol - das ist der
  Auslieferungsstand.

## Gelbe Punkte der Vorrunde

- G1 ROOM11A0 teilt Blatt 6 rect 2 mit ROOM2050: unveraendert offen (STAGE1-Raum, gehoert nicht in diese Stufe).
- G2 2040-Suedteil und 2070 Zone 2 ungemalt: unveraendert offen.
- G3 gemalte Tuersymbole ohne Marke: jetzt 20/21 (offen: (198,163) N zwischen r2 und r12 - kein Tuer-Datensatz
  von 3020/30C0/30D0 fuehrt dorthin).
- G4 FB_SHOT je Kamera mit sichtbarem Schleier: siehe unten und BESTAND.md §5 (nur der eigene Raum besucht).
- G5 Marker in der dunklen Pulsphase: Abzug jetzt bei Inventarbild 96 = Pulsspitze (Glow +4 je Bild bis Phase
  0x20, @0x80048ac8-b34 laut re15_inv_screen.c), Marker (208,200,0).
- G6 GUI-Tests auf dem Endstand: siehe Tests.

## Im echten Spiel: Abzuege je Kamera (FB_SHOT)

Abzug bei Inventarbild 96, nur der eigene Raum besucht, kein Plan: der Schleier ist sichtbar (fremde Raeume
nicht gezeichnet). STAGE2 und STAGE3 je Kamera eine Lage, STAGE4-6 bis drei Kameras je Raum, dazu je Elza-Raum
eine Lage - 335 Abzuege (BESTAND.md §5):

| Stage | geplant | Marker genau (<= 1 px) | abweichend, weil das Spiel die Figur verschoben hat (0 px zur echten Lage) | sonst abweichend | kein Marker | kein Abzug |
|---|---|---|---|---|---|---|
| STAGE2 | 83 | 60 | 6 | 0 | 0 | 17 (20B0/20B1, von keiner Tuer erreichbar) |
| STAGE3 | 119 | 97 | 12 | 0 | 0 | 10 (3000 und 3080: Raum-Ereignis beim Betreten, die Karte oeffnet nicht) |
| STAGE4 | 40 | 24 | 8 | 0 | 0 | 8 (4000, 4010, 4031) |
| STAGE5 | 75 | 66 | 1 | 0 | 8 (5070/5130 ohne Zone) | 0 |
| STAGE6 | 18 | 5 | 0 | 4 (6010 4 px, 6020 ohne Zeile) | 0 | 9 (6000, 6030, 6040) |

In STAGE2/3 liegt jeder gefundene Marker genau dort, wo die Zonentabelle ihn fuer die TATSAECHLICHE Lage der
Figur im Spiel (debug.log "[walk] pl pos") hinlegt. Schleier verletzt nur in 5090/5091 (STAGE5).
Messbilder: `analysis/karte_r42/messbild_blatt6_r2.png` und `messbild_blatt7_r2.png` (Marker je Raum, letzte
Kachel mit allen Orten besucht: Tueren und Treppen vollstaendig), `messbild_richtung_blatt6_r2.png` und
`messbild_richtung_blatt7_r2.png` (Richtung je Achse). Ein gdigrab-Video habe ich nicht gemacht.

## Tests

- `unit_r42_karte_st23` [A]-[H] gruen. GEGENPROBE mit demselben Test in eigenen Baeumen aus `git archive`:
  aaea5f8c2 ROT in allen acht Punkten; cb4f078c1 ROT in [C] (30E0 Zone 0), [E] (2070 z1, 20A0, 3050, 30A0
  gespiegelt), [F] (3050, 30E0), [G] (alle sechs Treppen), [H] (4 Symbole) - [A]/[B]/[D] dort gruen, die hatte
  Runde 1 schon behoben.
- `ctest -R "unit_.*(map|karte)"`: 50/50 gruen. `integration_map_uebergang`, `integration_map_raum_live` gruen.
- `ctest -R "map|karte|kartenbits|inv_screen|lageplan" -j 6` auf dem Endstand, GUI-Tests eingeschlossen:
  63/63 gruen (1020 s).

## Offen

- STAGE2: 2070 Zone 2 und der Suedteil von 2040 (samt Treppe) sind nicht gemalt; ROOM11A0 teilt r2 mit 2050;
  20A0 auf r10 ist eine PORT-WAHL (Original: Blatt 0xD ohne Kunst), r10 ist nur 11x8 px; 2030<->2080
  (Treppentuer) springt 17 px.
- STAGE3: (198,163) N ohne Tuer-Datensatz; 30E0 Zone 0 leuchtet als ganze r4-Kachel; 3030/3080 sind 8 px
  breit gemalt (Marker kann waagerecht kaum laufen, Original-Zeilen); Raum 3000 (Eingangsszene) laesst sich
  per Spielstand nicht abziehen.
- STAGE4-6: unveraendert, Befunde in BESTAND.md §6/§7 (Stufe 2 dieses Strangs).

## Kollisionen

`git merge-tree --write-tree` von b0a9ddbb9 gegen r42/finale a89c8254f, r42/gegenstaende 5515f2601, r42/gore
65b26bc11, r42/lageplaene bb87d614c, r42/rest 649e3a57a, r42/story 5849c8996, r42/hangar_aussen fe5436f55:
**kein Konflikt**. Keiner dieser Zweige aendert bisher `re15_map_zones.h/.c` oder `re15_room.h`.

Vorwarnung: U-11 (Wandplan 402 -> Laborblatt 8) und U-12 (U-Bahn-Blatt 12, Wagenkacheln) in r42/rest werden
dieselben Tabellen anfassen (`s_map_zones`, `s_map_marks`, `s_map_rectfix`). Neu von hier: Feld `tausch` am
ENDE von `re15_map_zone_t` - Zeilen mit 17 Werten bleiben gueltig (Wert 0). Wer eigene Zeilen-Projektionen
nachrechnet (Tests), muss `tausch` beruecksichtigen.

## Dateien

- Bestand: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/BESTAND.md`
  (im Repo `analysis/karte_r42/BESTAND_s1_r2.md`).
- Messbilder (Kopfzeile "Messbild - kein Ergebnis"): `analysis/karte_r42/messbild_richtung_blatt6_r2.png`,
  `messbild_richtung_blatt7_r2.png`, `messbild_blatt6_r2.png`, `messbild_blatt7_r2.png`.
- Werkzeuge der Runde: `scratchpad/karte/r2/` (belege.py, schuss1.sh, auswerten_schuss.py, montage.py).

## Eine Frage an den Nutzer

Der Gang 30E0 (zwischen 3050 und 3030) ist im Original als unterer Streifen in das Bild von Raum 3040
hineingemalt. Steht Leon im Gang, leuchtet deshalb das ganze 3040-Feld rot auf; der Punkt sitzt aber richtig
im Streifen. Soll das so bleiben, oder soll nur der Streifen aufleuchten (dafuer braeuchte es einen Teilbereich
wie bei ROOM1110)?
