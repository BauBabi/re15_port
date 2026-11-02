# Kandidat A (Vorsprung) - Dossier, fortlaufend

Stand: Beginn. Deutung A: Vorsprung am Ende N, Stapel Z1-Z4 darauf, Haken = Profil, Sicht Flanke, Bein NR durchgehend mit Taille, Ende N vorn.
Weg: Rundkegel-Primitive je Knochen, Passung per torch auf die Zielmasken, SDF -> Marching Cubes -> Blender-Reduktion -> UV -> Texturbacken -> form_pruefung.

## Log

- Durchgang 0 (Pipeline): entwurf.py (23 Knochen, 30 Rundkegel), passung.py (torch, 2D-Rundkegel), volumen.py (Marching Cubes + Blender), zusammenbau.py, maler.py. Erster Bau: Bild 2 7/7 Punkte, Bild 1 15/33, Flaechen 2308. Abweichung: Render 4-6 px zu gross, weil die Passung im grauen Saum nach aussen driftet.

- Durchgang 1 (Umriss): Kegelschnitt im SDF (Sehkegel beider Bilder, Boden), Taille A14 ist Figur (Soll-Umriss liess dort eine Rinne), versteckte Flaechen entfernt, Rasterpackung. Stand zw/l5: Bild 1 IoU 0.999, 30/33 Punkte; Bild 2 IoU 0.997, 7/7 Punkte, 9/9 Flaechen. Offen: nl_y342, rundling_2, L2, Textur noch Platzhalter.

- Durchgang 2 (Sicht, Kopf): Zwischen beiden Kameras lagen bei Gier 931 im Modellraum nur 33 Grad, der Stapel Z1-Z4 stand dann in Bild 2 sichtbar (Pink-Kopf). Gier Bild 2 jetzt 1283 (a rund 67, innerhalb 6B), Hals und Kopf in Pose 2 um rund 85 Grad zur Frau gedreht: Stapel abgewandt, Haken = abgewandte Kopfseite. Das ist Deutung, kein Messwert. Stand zw/l8: Bild 1 NCC 0.744, Bild 2 NCC 0.678, Bild 2 7/7 + 9/9.

- Durchgang 3 (Haut, Normalen): Haut als gemalte Wuelste statt Griess, Lichtnormalen gestaucht (L0 steht fast senkrecht ueber der Figur), Bogenlinien Q duerfen in Bild 1 bis in den Randbereich. Stand zw/l15: Bild 1 NCC 0.781, MAE 11.8; Bild 2 NCC 0.735, MAE 8.8. Abweichung: Zug Q erscheint in Bild 1 am Scheitel als helles Band.

- Durchgang 4 (Pose 2, Pruefpunkte): Rumpf in Pose 2 um 11 Grad nach vorn geneigt, Gier 1336 (a rund 63), Bein FR fast senkrecht im Feuer. Bild 1 33/33 Punkte, IoU 0.999. Offen: L2 (4-5 px auf Nadelloechern der Zielmaske im Querband A14), Q3. Stand zw/m2.


# Abschlussstand Kandidat A

Abgabe = Lauf zw/m8. Gegenprobe: `form_pruefung.py pruefe lage.json`, Ausgabe `pruefung_abgabe/`.

## Messwerte

| | Bild 1 | Bild 2 |
|---|---|---|
| IoU auf sicheren Pixeln | 0,999 | 0,963 |
| Weiss gedeckt | 1,000 | 1,000 |
| Pruefpunkte | 33/33 | 7/7 |
| Boden, Verdeckung | 2/3, L2 faellt | 9/9 |
| Zeichnung | 9/9 | 7/7 |
| Verbote | 1/1 | 1/1 |
| MAE / NCC in Weiss | 10,3 / 0,837 | 7,3 / 0,823 |

1862 Flaechen (1077 Dreiecke, 785 Vierecke), 23 Knochen, Textur 384x256, 8 bpp, eine CLUT-Zeile, Clips 0/1/2 je ein Bild, Boden je Pose y=0. FE.pruefe: 0 Fehler, 1 Warnung (Tick-Clips fehlen, in dieser Stufe gewollt).
Knochen 10 rumpf, 13 nl_hand, 18 nr_hand, Eltern stehen vor Kindern. Rumpf aus drei Knochen, Beine je mindestens drei Abschnitte, Hals, Kopf, Kiefer beweglich.

## Was noch abweicht

- **L2 faellt mit 1 Pixel.** (314,350) ist ein einzelnes schwarzes Maskenpixel im Querband A14, also mitten in der Taille. Der Versuch, es wegzuschneiden (Lauf m9), kostete b1.nr_y346_links. Bewusst belassen, nicht geloest.
- **Bild 2, schwarz getroffen 908 px**, die IoU 0,963 kommt daher. Huefte FR ragt bei (421-471,237-309) ueber die grauen Feuerzonen. Kein gezaehlter Pruefpunkt, aber im gefilterten Bild sichtbar: rechts unter der Kuppel steht dunkle Masse, wo das Video roten Raum zeigt. Beine FR und FL in Pose 2 sind Annahme.
- **Schlitz am Bauch**, von unten im Rundumblatt sichtbar, grau wie der Hintergrund. Ursache nicht gefunden: Deckel behalten (m5) und mehr Rumpfflaechen behalten (m10) beseitigten ihn nicht. Aus Spielkameras nicht gesehen, nur fuer Unterseite geprueft im Blatt.
- **Bild 1, Helligkeit:** Z2 92 gegen 130, Z4 77 gegen 141, Z8 75 gegen 96. Bild 2: Q1 29 gegen 58, Q4 43 gegen 96. Lage und Muster stimmen, die Stellen bleiben dunkler als im Video.
- **Bild 1, Form:** Innen fehlen die blauen und tuerkisen Streifen im Unterbau, A15 und A16 sind nur matt gemalt. Der Umriss ist glatter als die Vorlage. Die helle Luecke bei C5 zeigt den ausgemalten Hintergrund.
- **Bild 2:** G ist eine flache Bahn, im Video gegliedert und heller (Feuerschein fehlt im Render). V-Form F ist nicht gebaut. Zug Q im oberen Teil nur stueckweise, weil Bild 1 dort Vorrang hat.
- **Spiel ROOM5080:** Erzwungener Schnitt 2, der Bild 1 am naechsten liegt, zeigte mehrfach ein ganz schwarzes Bild (Maximum 0), Ursache nicht gefunden. Schnitt 0, 3, 4 zeigen die Figur nicht. Abgegeben ist Schnitt 1, also eine andere Sicht als Bild 1. Die Bild-1-Sicht ist nur auf der Buehne belegt.
- Bild 2 im Spiel: Flammen stehen vor Beinen und Unterbau, die Figur sitzt an der Sollstelle. Haut wirkt im Spiel heller als im Video.
- Texturdichte rund 32 Einheiten je Texel, rund 2,5 Bildpixel. Zeichnung ist dadurch stufig. Haut wirkt im aufgehellten Rundumblatt grob, Falten sind kein Ersatz fuer echte Falten im Netz.
- Keine Animation geprueft: Gelenke sind ueberlappend gebaut, gebogen getestet ist nur der Weg Pose 1 nach Pose 2. Ruhepose ist Pose 1 mit Hals zur Mitte.

## Frei gesetzt, kein Bildbeleg

- Kopf rund, ohne Gesicht, Augen, Zaehne. Z1 als Gesichtsflaeche, Z2 am Kieferrand, Z3 und Z4 am Hals: Deutung A.
- **Pose 2: Hals und Kopf um rund 85 Grad zur Frau gedreht, Rumpf 11 Grad nach vorn geneigt.** Grund: Der Stapel fehlt in Bild 2, liegt aber in der Sicht beider Kameras, und das Band Q erschien sonst am Scheitel in Bild 1. Das ist Folgerung aus dem Bild, kein Messwert. a = 63 Grad, innerhalb 6 B, aber nicht gemessen.
- Bein FL ganz, Bein FR in Pose 2, Seite L, Ende F, Unterseite, Sohlen, Fussform.
- C5 als Boden, nicht Figur. Zwei Stege unter Z7 (A20, A22, A23), weil das Knick-Band sonst in der Luft stuende.
- Hautstruktur (Falten, Adern, Knoten), Kontaktschatten: frei, unter der Video-Aufloesung.
- Lichtnormalen gestaucht (Oben-Anteil 0,16-0,34), damit Baender in Bild 1 unter fast senkrechtem L0 sichtbar bleiben. Das sind gesetzte Normalen, nicht die Geometrie.
- Feinton: Haut je Texel mit dem Helligkeitsverhaeltnis Video gegen gefilterten Render multipliziert (sigma 3 px, Faktor 0,55-2,0). Damit stecken die grossen Hell-Dunkel-Flaechen des Videos in der Textur, und NCC ist darum nicht unabhaengig vom Bild. Ohne Feinton: 0,779 / 0,788 (Lauf m3).
- Ziel-Korrekturen im Kegelschnitt in `volumen.py`: Soll-Umriss um A14, Z7, Kanten Abschnitt 7 ergaenzt, C5 und Ecke Bein NL entfernt. Damit passt das Modell auch an von mir geaenderte Masken, die Zahlen oben messen aber gegen die unveraenderten Zielmasken.
- Der Kegelschnitt macht den Umriss treu, die Pruefpunkte sind daher kein unabhaengiger Beweis der Form.

## Nachbau

Reihenfolge der Passung, jeweils `passung.py`: `lauf` ergab p10, dann `pose2` mit `--lage2 2320,0,-23467 --gier2 1283` (q1283), `--setze becken=0,0,0.2` (r1), `--gier2 1340 --gier-soll 1340` (r1340), `--setze huefte_r=0,0,-0.10;fr_ober=0,0,-0.05;fr_unter=0,0,0;fr_fuss=0,0,-0.05` (r2 = `passung_stand.json`).
Bau: `FEINTON=3 bash lauf.sh passung_stand.json zw/neu`. Die Kette von Grund auf wurde nicht in einem Lauf wiederholt, entwurf.py steht auf dem Endstand, passung.py ebenso, Zwischenstaende liegen in `zw/`.

Nicht gelaufen: ctest, Birkin-Tests, Port-Aenderungen. Kein Commit, Hauptbaum und pri/, synchro/ unberuehrt.
