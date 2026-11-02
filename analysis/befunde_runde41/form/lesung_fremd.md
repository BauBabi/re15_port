# Lesung FREMDKOERPER

Linse: was in und um die Figur gehoert NICHT zu ihr. Alle Koordinaten: Originalbild 640x480.
Sicherheit: sicher / wahrscheinlich / offen. "Nicht erkennbar" heisst: das Bild gibt es nicht her.
Der Port-Screenshot wurde nur fuer Raum und Feuer benutzt, nie fuer die Figur.

## Kurzfassung

- Bild 2 laesst sich auf den echten Hintergrund legen. Damit ist die Figur dort nicht nur Lesart, sondern **Verdeckung**: Sie steht nachweislich vor Tuerplatte, Lichtbalken und Bodenleiste.
- Die rote "Scheibe" (350-395,112-172) in Bild 2 ist **kein Objekt**, sondern der unverdeckte Rest der Tuerplatte. Ihre Unterkante ist die Aussenkante der Figur.
- Der gegliederte Streifen in Bild 2 ist zweigeteilt: oben Figur, unter dem Knoten (293,353) offen. Eine Bodennaht ist er in keinem Teil.
- Der tuerkis-blaue Guertel in Bild 1 ist **keine Form**, sondern Boden-Cyan, das in dunklen Koerper einblutet. Die Silhouette in Bild 1 gilt nur aus der Helligkeit.
- Die dunkle Bahn in Bild 1 ist Hintergrund. Sie gehoert nicht zur Figur.
- Der rosa-weisse Gegenstand in Bild 1 bleibt offen: weder Logo noch Boden noch Bildfehler, aber nicht als Koerperteil belegt.

## 1. Grundlage

**Bild 2.** Hintergrund: `ROOM509.BSS` Chunk 11, dekodiert mit dem Engine-Dekoder (`tests/unit/probe_bg_dump.c`). Von 16 Chunks ist nur dieser der passende Blick (Sichtpruefung). Der rechte Teil des Port-Screenshots deckt sich damit 1:1 (mittlere Abweichung 3.8 Grauwerte).

Registrierung: robuste Homographie aus 24 Hand-Passpunkten und 3 Linien. Median 4.9 px, Mittel 5.5 px, Maximum 18.3 px (Deckensteg). Matrix und Polygone stehen in `lesung_fremd.json`.

| Gegenprobe (nicht in die Anpassung eingeflossen) | Ergebnis |
|---|---|
| Dunkle Boden-Mittelnaht, Parallelsuche | Rot-Minimum bei Versatz 0 bis +5 px (R 138 gegen 148-163). Boden sitzt auf rund 5 px. |
| Boden-Querleiste gegen Feuer-Oberkante | Richtung passt (Steigung -0.15/-0.18), 13 px Versatz. Nur Hinweis. |
| Raumtextur, Hochpass-NCC | Freier Raum Median +0.52 (1661 Bloecke), sicher Figur -0.05 (306), grau +0.06 (82). |

Offen: Die Reste an Decke und Kisten (bis 18 px) lassen zu, dass die Beta-Kamera minimal von Cut 11 abweicht.

**Bild 1.** Keiner der 6 Chunks von `ROOM508.BSS` zeigt diese Kamera (Sichtpruefung, wahrscheinlich). Es gibt also keine Verdeckungsprobe, die Lesung stuetzt sich nur auf das Bild.

## 2. Bild 2

| Stelle | Urteil | Sicherheit | Maske | Beleg |
|---|---|---|---|---|
| Aussenkante oben (268,215)-(278,190)-(290,170)-(300,162)-(320,156)-(345,160)-(365,170)-(380,177)-(390,195)-(397,207) | Figur | sicher | weiss | Links davon Hintergrund schwarz (0,0,0), Rand violett B-G +26. Rechts bricht die Platte ab: R 159 (y=156) auf 44 (y=177) bei x=365. |
| Randstrich (397,207)-(418,225) | Figur | sicher | weiss | Kreuzt das senkrechte Rohr diagonal, B-G +26. Der Raum hat dort keine Diagonale. |
| Rote Scheibe (350-395,112-172) | Raum, nicht Figur | sicher nicht Figur, Raum wahrscheinlich | schwarz | Links, oben, rechts deckt sie sich mit den Plattenkanten (x~345, x~395, y~113). Rotton G~5. Unten schneidet die Figur ab. |
| Rechte Tuerplatte (345-398,172-290) | Figur davor | sicher | weiss | R flach 43-52, obwohl der Hintergrund 34-96 zeigt. Helle Kante x~360 (R 96) fehlt (45). Spiegelgitter links R 143 Streuung 38, rechts R 51 Streuung 9.8. Querband y=229-235 unsichtbar (R 43-47 gegen 65-69). |
| Inneres im Tuerschwarz (262-345,205-285) | Figur | sicher | weiss | Stufe auf gleichem Abstand zu hellen Stellen (40-80 px): leere Tuer B 6.8/R 27, Inneres B 16/R 40. Bei Farbbluten fiele der Wert mit dem Abstand, er tut es nicht. Fuer y=258-286 allein keine Stufe belegbar (dort beginnt der Innenboden), aber von oben und unten eingeschlossen. |
| Lichtbalken (222-293,286-299) | Figur davor | sicher | weiss | Hintergrund (202,65,16), Video (45,9,16). Die Deckenlampe, gleich rot (197,11,8), wird im Video fast weiss (206,182,178): Der Balken muesste also leuchten. |
| Dunkle Bodenmasse (230-300,300-360) | Figur, kein Schatten | sicher | weiss | Helle Bodenleiste: Kontrast 27.3 im freien Boden, 0.7 in der Masse. Ein Schatten dieser Tiefe (R 68 gegen 91) liesse rund 20 uebrig. |
| Unterkante (240,372)-(280,352)-(310,338)-(330,317) | Figur | sicher | weiss/grau | Stetiger Anstieg, Cb-Grat laeuft genau darauf. |
| Streifen oben (318-338,316-338) | Figur | sicher | weiss | Violett: B-G +29..+49, Cb -0.4..+9.2, Kittel +10.7. Hintergrund (50,19,8). Steigung -1.28, Bodenleiste -0.13, liegt auf der Kante der Masse. |
| Streifen Mitte und Knoten (285-320,338-360) | Figur | wahrscheinlich | weiss | Heller Rand derselben Kante, Cb -3. Am Rand zum roten Boden, Farbe dort abgeschwaecht. |
| Streifen unten, V-Form (263-304,355-399) | offen | keine Naht: sicher; Figur oder Feuer: offen | grau | Die Naht ist eine dunkle Linie 10-40 px daneben. Cb -17 ist hier kein Beleg: Kleine Teile im roten Boden nehmen dessen Farbe an (Abschnitt 4). |
| Fleck (245-263,372-394) | offen | offen | grau | (204,106,139), B-G +33, frei auf rotem Boden. |
| Bogen (205-245,215-262) | Figur | wahrscheinlich | grau | Nicht Raum: Hintergrund (0,0,0), sicher. Nicht Feuer: B-G +14..+29, kein Weisskern, sicher. Kein Geisterbild: Versatz (+40,-6) kopiert weder Kopf (erwartet +42, gemessen G=5) noch Rohrglanz (+38, G=6), sicher. Nicht Kittel: B 30-67 gegen 134-172, wahrscheinlich. Die Flaeche haengt ueber die Stufe an der Figur. |
| Spitze (356-376,290-315) | Figur | wahrscheinlich | grau | Nicht Raum: Hintergrund (27,8,0), sicher. Spitze (217,193,204), B-G +11, Feuerfunken -3..-26. Direkt darueber Violett B-G +21..+29 (365,279-290). |
| Rechter Rand (398-415,225-298) | nicht erkennbar | offen | grau | Feuerschein liegt ueber der Kante. Rohr-Ornament (408,255-262) fehlt im Video, Grenze aber nicht ablesbar. |
| Weisse Flammen F1 (160-225,325-375), F2 (415-462,225-300), F3 (415-505,310-400), F4 (270-450,395-475) | Feuer | sicher nicht Figur | schwarz | R=G=B bis 224, B-G ~0, Saum warm B-G -5..-10. F2: gerade Linkskante x~415-421, ob die Figur sie abschneidet: offen. |
| Roter Boden (235-420,360-395) | Raum | sicher | schwarz | Naht und Bodenleiste bleiben sichtbar. |
| Kistenflaechen rechts (540-640,265-450) | Raum, ueberstrahlt | sicher | schwarz | Form deckt sich mit den Kisten. Zwei Kanten sind in die Anpassung eingeflossen (Rest 1-2 px), also keine unabhaengige Probe. |

## 3. Bild 1

| Stelle | Urteil | Sicherheit | Maske | Beleg |
|---|---|---|---|---|
| Dunkle Bahn (250-300,150-190) | Hintergrund | sicher | schwarz | Reicht ohne Stufe bis zur Bildoberkante: Helligkeit bei x=250-270 steigt von y=0 bis y=215 stetig 9 auf 22. Wandband y~157 links und rechts sichtbar. |
| Helle Stange (272-283,130-190) | Hintergrund | sicher | schwarz | Bricht bei y~190 ab (62 auf 27): dort beginnt die Figur. |
| Kuppeloberkante (248,212)-(254,205)-(277,191)-(290,188)-(306,189)-(326,206)-(352,210)-(378,237) | Figur | wahrscheinlich | Rand grau | Warme Wand endet (B-G negativ auf positiv) bei x=320-340 auf y~205. Bei x=250-275 dunkel auf dunkel: Lage dort offen. |
| Rosa-weisser Gegenstand (250-288,420-479) | offen | offen | grau | Siehe unten. |
| Lavendelstrich (228-250,423-447) | offen | offen | grau | (78,62,107), Figurenton, fehlt in Bild 2, also kein Logo. Laeuft auf die Kappe des Gegenstands zu. |
| Tuerkis-blauer Guertel (340,355)-(372,395)-(355,425)-(305,455) | Fremdfarbe auf dunklem Koerper | wahrscheinlich | weiss, Rand grau | Abschnitt 4. Aussen liegt noch Koerperviolett (45,33,88), danach harte Kante (354,429). Rechts bei y=392 weich (Y 63 auf 215 ueber 24 px): dort Kantenlage offen. |
| Weisse Bodenflecken (283-311,328-369), (345-378,325-375) | Boden | sicher | schwarz | Y 224-230, neutral, Kanten 5-8 px scharf. |
| Lavendelspitze ueber Bodenfleck (286-304,309-328) | Figur | wahrscheinlich | grau | (111,103,150), G-R -9: Koerperton, nicht Boden. |
| Fleck (310-336,285-309) | offen | offen | grau | Rund, Lavendelrand: spricht fuer Glanz. Einzige Insel mit geklipptem Weiss (9 %), auf Gelaenderhoehe: spricht fuer Hintergrund durch eine Luecke. Gelaender links flach, rechts Steigung 0.1, Verlaengerung also nicht schluessig. |
| Fleck (237-248,280-295) | offen | offen | grau | Blaeulich wie Figurenglanz, aber direkt am Ende des Gelaenders. |
| Dreieck (391-410,278-287) | Figur | wahrscheinlich | grau | Pale Spitze, gerade Unterkante, nicht auf Gelaenderhoehe (rechts y~303). |
| Rumpfglanz (305-335,218-278) | Figur | sicher | weiss | Innerhalb des Umrisses, keine Wandfarbe. Rosa nicht als Eigenfarbe lesen. |
| Tuerkisfleck (291-315,407-428) | offen | offen | grau | Lavendel oben, Cyan unten, Y bis 159. Glanz oder Boden durch Luecke. |
| Band rechts (404-425,250-285) | dunkle Masse | wahrscheinlich | weiss | Nicht warm, harte Kante x~425 (Y 38 auf 75). |
| Fussleiste links (232-248,262-276) | nicht erkennbar | offen | grau | Dunkle Leiste geht ohne Grenze in die Figur ueber. |
| Fussleiste rechts (430-640,288-300) | Hintergrund | sicher | schwarz | Dunkel und blaeulich wie die Figur, laeuft aber bis weit rechts. |
| Logo (0-253,436-480) | Logo | sicher | schwarz | Minimum beider Bilder, endet bei x~253. Was darunter liegt: nicht erkennbar. |

**Der rosa-weisse Gegenstand (250-288,420-479).** Sicher ausgeschlossen:
- Logo: Die Schrift endet bei x~253, der weisse Riegel (253-278,447-460) liegt daneben.
- Fester Bildfehler: Bild 2 hat zwar bei (262-268,428-442) einen aehnlichen rosa Fleck, die Korrelation (0.37/0.21/-0.19) bleibt aber unter der 95-%-Grenze von Zufallsfenstern (0.50/0.63/0.61).
- Boden: Cr +46, Boden -9 bis -33.

Fuer die Figur spricht (wahrscheinlich): Der obere Teil ist an drei Seiten koerperfarben eingefasst, oben Kappe (97,55,110), rechts dunkles Band (46,28,53) bis y~456, links Strich. Rosa kommt sonst nur am Rumpf vor.

Dagegen oder offen: Er laeuft unten aus dem Bild (y=479 noch (143,25,77)), die Ausdehnung nach links liegt unter dem Logo. Ob Koerperteil, loses Objekt oder etwas davor, ist **nicht erkennbar**. Eine gerade Kante gibt es nicht (Streuung 4.75 px).

## 4. Was die Bildkette verfaelscht

- **Farbe blutet in dunkle Flaechen.** Dunkle Pixel neben Cyan-Boden: G-R +24, +12, +7, +6, +7 fuer 1-30 px Abstand, bei 30-45 px -0.3. Neben neutralem Boden flach -2 bis -5. Ein Schatten laege auf beiden Bodenarten gleich, die Faerbung haengt aber an der Nachbarfarbe. Eine fruehere Zwischenaussage ("Saum = abgedunkelter Boden") ist damit ueberholt.
- **Folge:** Farbe taugt nur an grossen oder hellen Teilen. Kleine Teile nehmen die Farbe der Umgebung an (Bild 2, unterer Streifen).
- **Kein Farbversatz.** Beste Verschiebung Farbe gegen Helligkeit -1 bis +2 px, Kurve flach. Helligkeitskanten rund 5-8 px.
- **Dunkle Zonen in Bild 1 sind generell blaeulich** (B-G bis +17 in der Bahn, bis +27 in der Fussleiste). B-G trennt dort nur gegen warme Wand, es beweist keinen Koerper.

## 5. Verworfene Wege

- ECC-Registrierung: divergiert (NCC 0.17, Massstab 0.11).
- Begrenzte Eckensuche: NCC 0.59-0.62, untere Ecken unbestimmt, Versatz bis 35 px. Durch Passpunkte aus der Bodenzone ersetzt.
- Kantenbreiten Helligkeit gegen Farbe: nicht schluessig (Fenster mit mehreren Strukturen). Ersetzt durch den Nachbarfarben-Test.
- Tuerlinie (326,221-266) als Verdeckungsprobe: zu schwach (R~48).
- Port-Feuer als Lagehilfe: nur 4966 von 16418 Pixeln treffen Video-Weiss. Feuerlage kommt aus dem Video.

## 6. Masken

`maske_bild1.png`, `maske_bild2.png`: 640x480, Werte 0/128/255.

| | weiss | grau | schwarz |
|---|---|---|---|
| Bild 1 | 34484 | 9543 | 263173 |
| Bild 2 | 24766 | 8980 | 273454 |

Bau: Bild 2 aus gelesenen Polygonen, je mit Beleg. Bild 1 aus Helligkeit (Y<100, Glaettung 1.2) innerhalb einer gelesenen Huelle, kleine helle Inseln und die offenen Stellen grau. Um Weiss liegt ein grauer Rand von 4 px.

Pruefung: Lichtbalken 94.8 % weiss, Rest grau. Leere Tuer und Logo 0 % Figur. Weiss auf Feuerweiss: 3 Pixel. Bodenfleck-Kerne 98-100 % schwarz. Bahn 97.3 % schwarz (Rest ist der Kuppelrand).

6 von 306 weissen Bloecken in Bild 2 korrelieren ueber 0.5 mit dem Raum (x 372-404). Erklaert: Feuerschein-Kante faellt auf die Rohrkante, und bei y~235 und y~285 steigt Blau mit, das ist Violett und kein Holz. Die zuerst weisse Zone rechts ab x~398 wurde deshalb auf grau zurueckgestuft.

## 7. Fuer die Formarbeit

Nicht als Form uebernehmen: rote Scheibe, Guertel und Saum, dunkle Bahn, weisse Bodenflecken, weisse Flammen. Silhouette in Bild 1 nur aus Helligkeit. Grau ist ausdruecklich keine Vorgabe fuer Form.

Ansichtsbilder: `lesehilfen/fr_masken_uebersicht.png`, `fr_b2_ueberlagerung.png`, `fr_b2_streifen_mit_raumkanten_x4.png`, `fr_b1_klassen_x3.png`.
