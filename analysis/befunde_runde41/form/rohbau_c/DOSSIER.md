# Rohbau C (Bildnah): Dossier

Stand 2026-11-01, Abgabestand `lauf/ende`. Arbeitsbaum r41_form, kein Commit. Kein RE-Wert: neues Modell nach zwei Videostandbildern. Formvorlagen nur `bild1.png`, `bild2.png`, bindend die Lesung in `nutzer_teile/`.

## 1. Ergebnis

Messung `form_pruefung.py pruefe form.json`, `lauf/ende/bericht.txt`. IoU am sichtbaren Teil, Logo-Schrift zaehlt nicht.

| Teil | Bild | IoU | Deckung | Innen | Laenge Ist/Soll | Breite Ist/Soll |
|---|---|---|---|---|---|---|
| tail | 1 | 0,91 | 0,97 | 0,93 | 288/285 | 68/66 |
| left_leg | 1 | 0,90 | 0,94 | 0,95 | 171/172 | 38/39 |
| right_leg | 1 | 0,86 | 0,97 | 0,88 | 168/171 | 51/50 |
| right_arm | 1 | 0,84 | 0,91 | 0,91 | 130/129 | 40/41 |
| part_of_Body | 1 | 0,83 | 0,96 | 0,86 | 100/111 | 91/84 |
| face_and_upper_body | 2 | 0,89 | 0,94 | 0,94 | 226/220 | 164/160 |
| left_arm_front | 2 | 0,82 | 0,88 | 0,92 | 145/153 | 54/52 |
| right_arm_front | 2 | 0,81 | 0,91 | 0,89 | 186/188 | 110/110 |

Ziel 0,80 je Teil erreicht. **Einschraenkung:** `face_and_upper_body` zaehlt in Bild 2 rechtes Bein und rechten Oberarm mit (Abschnitt 4). Nur Rumpf, Hals, Kopf, Kiefer und linker Oberarm: IoU 0,74, Deckung 0,77. Der Rest des Umrisses ist im Modell Bein.

| | Bild 1 | Bild 2 |
|---|---|---|
| Umriss IoU auf sicheren Pixeln | 0,975 | 0,963 |
| Pruefpunkte | 25/31 | 7/7 |
| Boden frei, Verdeckung | 1/3 | 9/9 |
| Zeichnung | 8/9 | 7/7 |
| NCC im Weiss | 0,589 | 0,642 |

Klauenspitzen im Bild gegen die Spitzen des Nutzer-Umrisses, `rc_spitzen.py`:

| Finger | Bild | Ist | Soll | Abstand px |
|---|---|---|---|---|
| rechte Hand aussen | 1 | (425,362) | (425,363) | 1,4 |
| rechte Hand aussen | 2 | (144,350) | (145,353) | 3,7 |
| rechte Hand Mitte | 2 | (197,473) | (197,476) | 3,2 |
| rechte Hand innen | 2 | (221,472) | (222,475) | 3,5 |
| rechter Daumen | 2 | (274,440) | (274,443) | 3,4 |
| linke Hand aussen | 2 | (461,389) | (461,392) | 2,6 |
| linke Hand Mitte | 2 | (467,414) | (468,417) | 2,7 |
| linker Daumen | 2 | (439,412) | (439,418) | 5,6 |

Der Bericht meldet trotzdem `Spitze fehlt` bei (222,475), (274,443): Seine Spitzen sind Gipfel im Umriss mit 12 px Abstand zum Sattel, duenne Klauen im Umriss der Hand zaehlen dort nicht. Soll unter Logo bei (197,476), (222,475). Fuesse: 4 und 7 px. Schwanzspitze: Merkmal X zu 0,90 gedeckt. Das Ende laeuft unten aus dem Bild, der Wert `Spitze 41` der Teile-Messung ist die linke Ecke des Keils unter dem Logo gegen die Soll-Ecke.

Rahmen: 1807 Flaechen (1220 Vierecke, 587 Dreiecke), 32 Knochen, 32 Meshes, Textur 384x256 8 bpp, 10 Farben, 1 CLUT-Zeile, 3 Clips je 1 Bild, Boden je Pose y=0. `FE.pruefe` 0 Fehler, Warnung nur fuer die fehlenden Tick-Clips.

Spiel: `Bahnsteig-Form: 32 Meshes, 32 Knochen, 3 Clips`, beide Laeufe mit gdigrab, `spiel_pose2_r5090_s11.png`, `spiel_pose1_r5080_s1.png`.

**Was diese Zahlen beweisen.** IoU, Deckung und Spitzen messen gegen dieselben Masken, an die gerechnet wurde. Die Zeichnung ist in die Polygone gebacken, die der Test abfragt. Bestanden heisst hier: Es passt, nicht: Es ist belegt. Aussagekraft haben Rundblick, Biegeprobe und Spielbild.

**Urteil am Bild.** Im gefilterten Vergleich liest man dieselbe Figur: Bild 1 Schwanz, zwei Beine, Arm mit Dreieck, rosa Stelle, Baender, Raute zwischen Schwanz und Bein. Bild 2 dunkle Kuppel mit Zug Q, gerippter Unterarm, grosse Klauenhand links unten, linker Arm mit Klauen. Im Rundblick steht ein geducktes, schweres Raubtier mit langem Schwanz. 1:1 ist es nicht, Abschnitt 5.

## 2. Bau

Ring-Bauweise, keine Metaballs, kein Voxel, kein Skinning. Mesh i haengt an Knochen i.

| Gruppe | Knochen | Ecken je Ring | Ringe je Teil | Flaechen |
|---|---|---|---|---|
| Rumpf (huefte, wirbel, brust) | 0, 7, 10 | 10 | 5-6 | 190 |
| Hals, Kopf, Kiefer | 21, 22, 23 | 8, 10, 8 | 5, 10, 5 | 206 |
| Schwanz | 1-6 | 8 | 4-6 | 256 |
| Beine | 8, 9, 24-27 | 8 | 5-7 | 336 |
| Arme, Haende | 11-13, 16-18 | 8 | 5-7 | 352 |
| Finger | 14, 15, 19, 20, 28-31 | 6, Spitze als Kegel | 6 | 336 |
| Zaehne, Hauer, Zehen | auf 21-23, 25, 27 | 3 und 5, Kegel | 1-2 | 131 |

Jeder Ring: Mitte, feste Ausrichtung, eigener Radius je Ecke. Grate und Wuelste sind feste Reliefs je Ecke (`rc_plan.relief_von`): Wirbelgrat, Schulterblatt, Beckenkamm und Taille, Wade, Knie, Deltamuskel, Ellbogenkappe (Stelle Z5), Rueckengrat des Schwanzes. Normalen mit Knick 38 Grad, Flaechen darueber bleiben hart.

Gelenke: Kind-Teil beginnt eingezogen unter dem Eltern-Teil, Enden geschlossen. Biegeprobe `biegeprobe.png`, Knie, Ellbogen, Schultern, Huefte, Hals, Schwanz +-35 Grad: keine Loecher gesehen. Das ist eine Sichtprobe, keine Messung.

Passung (`rc_fit.py`): Parameter sind nur Radien, Ringmitten und Gelenkwinkel, keine freien Ecken. Weiche Deckung je Dreieck auf der GPU, Verlust je Nutzer-Teil: Maske decken, nicht ueberstehen, dabei zaehlen Teile davor als erlaubt. Regeln: zweite Differenz rund um den Ring und laengs, Naehte an Gelenken, links gleich rechts am Rumpf, Ringmitten an Gelenken gepinnt.

Atlas: je Teil ein Streifen, U = Umfang, V = Laenge, Lage in `atlas.json`, Bild `atlas_blick.png`. Teile mit gleichem `feld` teilen einen Streifen (Zaehne, Zehen). Dichte 0,029 Texel je Einheit, also rund 1 Texel je 2-3 Bildpixel. Unten bleibt ein freier Streifen, die Finger koennen beim Malen zusammengelegt werden.

Textur: Zonen und Zeichnung, nicht gemalt. Grundhaut (26,10,16), Marken per Rueckprojektion aus dem Bild auf sichtbare Texel (`rc_textur.py`): alle Stellen aus FORMBLATT2 Abschnitt 6, dazu die gemessenen Flecken aus `farbe_palette.json`. Rippen von G aus der Bildhelligkeit in seinem Polygon. Rosa Ende X auf (112,72,82).

## 3. Gerippe nachgestellt

| Aenderung | Grund |
|---|---|
| Zwei Knochen mehr: klaue_l4 (30) an hand_l, klaue_r4 (31) an hand_r | Umriss Bild 2 hat vier Spitzen, gerippe.json drei Finger. Teile 10, 13, 18 fuer den Tick bleiben |
| Fingerwurzeln auf Handbreite, Achsen gerade +X, 1 innen, 2 Mitte, 3 aussen, 4 Daumen | Rechte Hand im Bild passt so: Finger 1, 2 zur Kamera, Daumen zum Koerper, aussen Sporn (145,353) |
| Hand und Finger je Pose neu (`rc_ik.py`, dann Passung) | Haende lagen falsch, FORMBLATT2 Abschnitt 7 |
| Kiefer Pose 2 rund 24 statt 10 Grad offen | Kiefer steht vor dem Ellbogen, reicht bis Kante R |
| Hals und Kopf Pose 1 je rund 11 Grad gehoben | Nach dem Einziehen des Bauchs ragten Kopf und Zaehne unter dem Bauch hervor, Bild 1 zeigt dort nichts. Kopf ist dort verdeckt |
| Alle Gelenke klein nachgestellt | Bild 1 hoechstens oberarm_r 7,1, unterarm_r 4,0, sonst bis 2,6 Grad. Bild 2 kiefer 13,7, unterschenkel_r 10,6, oberschenkel_r 7,9, fuss_r 7,2, oberschenkel_l 5,6, hals 4,4, sonst weniger |

Laengen, Versaetze ausser Fingern, Lage und Gier unveraendert. Stand als Datei: `gerippe_c.json`.

## 4. Frei gesetzt

- Kopf: Schaedelform, Brauengrat, Augenhoehle, Jochbein, Schnauze, Zaehne, Augenzone. Belegt ist nur der Rand auf Zug Q (Vorgabe aus FORMBLATT2) und die Lage in Bild 2.
- Kiefer: Form und Groesse, breit und schwer, damit er vor dem Ellbogen steht.
- Hauer: Spitze liegt auf dem Sehstrahl durch Stelle P, dort sitzt ein heller Kern im Bild. Dass es ein Hauer ist, ist Annahme. Rechter Hauer ist Spiegel.
- Bein in `face_and_upper_body`: Die dunkle untere Spitze des Umrisses fuellt in Bild 2 das rechte Bein in der Hocke, wie im Gerippe. Fremde Lesung moeglich (Brust, Kinn). Darum die Zuordnung Knochen 9, 16, 26, 27 zu diesem Teil in `form.json`.
- Vierter Finger links: Im Bild nicht einzeln zu sehen.
- Fingerlaengen 700-960, Spreizung, Hoehe der Spitzen. Rechts stehen die Finger in Pose 2 rund 300 ueber dem Boden, der aeussere 1360. Auf den Boden gezwungen verfehlen sie die Spitzen.
- Schwanzwurzel: Keil unter dem Schwanz bis ans Becken, Beckenbuckel. Bild 1 sieht beides nicht. Ohne sie liegt der Schwanz als Bruecke auf dem Ruecken.
- Tiefe aller Glieder: Bild 1 sieht Beine nur von hinten, die Radien vorn/hinten sind Vorgabe.
- Rechter Unterarm flach im Querschnitt, damit er in beiden Bildern passt. Linker Unterarm und beide Haende weichen von rechts ab: Bildnah, nicht symmetrisch.
- Keil rechts (283-303,360-432) gefuellt, Keil links nur bis y 398. Beide liegen in keinem Teil, sind im Bild dunkel.
- Zehen, drei je Fuss.
- Ruhepose: Pose 1 mit gehobenem Kopf und geschlossenem Maul.
- Farben: Grund- und Markenfarben an der unteren Palettengrenze, Zaehne (40,26,28), Rosa X (112,72,82) ausserhalb der Palette. Grund Abschnitt 5.

## 5. Was im Bild noch abweicht

Bild 1:
- Oberflaeche zu glatt. Das Original zerfaellt unten in Lappen, Baender und blaugruene Streifen, das Modell hat dort Roehren mit Flecken.
- Helligkeit der Marken stimmt nicht: Y Video gegen Filter Z1 142/196, Z3 172/215, Z4 141/224, Z5 75/195, dagegen Z2 130/61. L0 steht fast senkrecht, Oberseiten bekommen Faktor bis 1, Seiten 0,35. Die Farben sind schon an der Palettenuntergrenze. Das ist fuers Malen offen, vielleicht hilft nur eine andere Flaechenlage.
- Links unten bleibt der Keil (218-255,395-445) frei, im Original dunkel. Fuellen ginge, left_leg faellt dann unter 0,80 (Lauf d8: 0,67). Die Buehne zeigt keinen Schatten, im Spiel liegt dort Schatten.
- Oberkante Schwanz rechts 13,5 px zu tief bei (322,187), Grenze 8,6. Rand rechts neben dem Arm (429,272) und Z9 nicht gebaut, der Nutzer-Umriss endet davor, das ist O1 aus `pruefung.md`.
- Boden frei: L1 32 px, L2 25 px, Eindringen 2,8 px.
- Rosa Ende im Filter gleichmaessig, im Original zerrissen.

Bild 2:
- Zug Q, Q4 und G schwaecher als im Video: Y 30/58, 52/96, 61/113. Das Feuer fehlt in der Buehne, seine Einstreuung auch.
- Linker Arm und Klauen im Video rotbraun, im Filter fast schwarz. Feuerschein, im Spiel flaches Licht.
- Zwischen Kiefer, linkem Arm und rechtem Unterarm (352-375,295-326) bleibt ein Loch mit 400 px, dort steht der Raum. Stelle P ist nur zu 0,37 Figur.
- Heller Rest im Gesicht: Zaehne dunkel gemalt, im Original ist dort nichts. N1 besteht.
- Aeusserer Finger rechts ist ein langer Kegel und wirkt wie ein Horn. Der Sporn des Umrisses verlangt ihn. Kann auch etwas anderes sein.

Rundblick:
- Handflaechen kastig, vor allem rechts, die Passung hat sie auf den Umriss gezogen.
- Rumpf seitlich noch glatt, keine Rippen.
- Schwanz in Pose 1 ein Bogen ueber dem Becken: Der Keil schliesst die Luecke, von vorn rechts bleibt ein Riss.
- Spitzer Grat oben am Ruecken in Pose 2, wo Wirbel und Brust knicken.
- Kopf von der Seite lang. Die Laenge 1300 ist Gerippe-Wert, im Bild nirgends belegt.

Spiel:
- Pose 2: Lage, Groesse und Dunkelheit passen, das Spielfeuer steht anders als im Video und deckt Beine und Hand.
- Pose 1: Bild 1 hat keinen Schnitt, Schnitt 1 zeigt die Figur klein von der anderen Seite.

Nicht geprueft: bewegte Clips, Tick-Clips 3 bis 27, Kasten, Schatten-Festwerte, Masken, andere Schnitte.

## 6. Durchgaenge

Volle Blaetter behalten: `lauf/d1`, `d3`, `d9`, `d21`, `ende`.

| Lauf | Aenderung | Befund am Bild |
|---|---|---|
| d1 | Ringe frei, schwache Regeln | IoU 0,72-0,94, Bruch: Ringmitten wandern, Kiefer fuellt den Rumpf, Huefte vor dem Schwanz. Verworfen |
| d2 | Grosse Form, vier Finger per IK | Seite zerfaellt in Splitter, Ringmitten frei |
| d3 | Mitten gepinnt, Gelenke leicht frei, Kopfziel | Liest sich als ein Tier |
| d4 | Kopf, Zaehne, Zehen, Reliefs, Textur | Zaehne stechen heraus, Figur im Filter rosa |
| d5 | Grundhaut dunkel, Schwanzwurzel taucht ein | Dunkelheit passt |
| d6 | Feinlauf | Alle Teile ab 0,81 |
| d7 | Beckenbuckel, volle Beine, Hauer, Ruhepose | Erster Spiellauf gut |
| d8 | Neuer Schaedel, dunkle Keile | Bild naeher, left_leg 0,67 |
| d9 | Schwanzwurzel als Keil, Kind-Teile eingezogen | Keine Fenster mehr, Schwanz waechst aus dem Rumpf |
| d10-d12 | Keil links klein. d11: Oberarm links zum Arm verworfen, IoU 0,76 | 0,81-0,91 |
| d13-d15 | Stichloecher zu, Ueberstand strenger | d15 verliert Rand 413, darum zurueck auf d14 |
| d16-d17 | Klauenspitzen als Punktziele | Spitzen bis 7 px, rechter Arm 0,76, Unterarm laeuft seitlich aus dem Streifen |
| d18-d21 | Rechter Unterarm flach, Haende frei, Rand rechts | Kopf ragt in Bild 1 unter dem Bauch hervor |
| ende | Hals und Kopf Pose 1 gehoben, Zaehne dunkel | Abgabe |

Gelernt: Der schmale Streifen `right_arm_front` ist kein ganzer Arm, sondern der sichtbare Rest neben dem Rumpf. Der Unterarm muss flach werden, sonst laeuft er aus dem Umriss oder verdeckt den Rumpf.

## 7. Dateien und Lauf

Generator: `rc_kern.py` (Knochen, Kette, Ringteile, weiche Deckung), `rc_plan.py` (Stationen, Reliefs), `rc_aufsatz.py` (Zaehne, Zehen, Hauer), `rc_ik.py` (Hand), `rc_fit.py` (Passung), `rc_bau.py` (EMD, Atlas, Bericht, Vergleich), `rc_textur.py`, `rc_abgabe.py`. Sichten: `rc_blick.py`, `rc_nah.py`, `rc_biege.py`, `rc_spitzen.py`. Eingabe: `stand.json` (Passung), `zw/pose_ik.json` (Handwinkel, wird von `rc_kern.py` gelesen).

```bash
cd analysis/befunde_runde41/form/rohbau_c
export PYTHONIOENCODING=utf-8
python rc_bau.py stand.json --tag ende --voll     # baut FORM.EMD, FORM.TIM, form.json, atlas.json, lauf/ende
python rc_abgabe.py ende                          # Blaetter in den Ordner, gerippe_c.json
```

Zweimal hintereinander geprueft: FORM.EMD und FORM.TIM bytegleich, Abgabe md5 150d33ab..., b8aad8d0.... `stand.json` entstand ueber die Kette der Laeufe, mit Code-Aenderungen dazwischen. Gegenprobe mit dem Endstand des Codes von vorn, `python rc_fit.py --aus x.json --schritte 300 --fein 800`, rund 5 Minuten: IoU 0,79-0,90, Spitzen bis 6,6 px. Der Weg traegt also, `right_arm_front` liegt dabei knapp unter 0,80.

Abgabe: `FORM.EMD`, `FORM.TIM`, `form.json`, `vergleich_bild1.png`, `vergleich_bild2.png`, `teile_bild1.png`, `teile_bild2.png`, `rundum.png`, `rundum_hell.png`, `rundblick_pose1.png`, `rundblick_pose2.png`, `rundblick_ruhe.png`, `biegeprobe.png`, `spiel_pose1_r5080_s1.png`, `spiel_pose2_r5090_s11.png`, `atlas.json`, `atlas_blick.png`, `textur.png`, `gerippe_c.json`, `lauf/ende/`.

Kein Commit. Geschrieben nur in diesem Ordner und in `re15_port/build/form_spiel/` (eigene Laufordner danach geloescht).
