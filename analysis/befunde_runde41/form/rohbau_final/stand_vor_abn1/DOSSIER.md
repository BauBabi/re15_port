# Rohbau final: Dossier

Stand 2026-11-01, Abgabestand `lauf/ende`, `stand_final.json`. Arbeitsbaum r41_form, kein Commit. Kein RE-Wert: neues Modell nach zwei Videostandbildern. Formvorlagen nur `bild1.png`, `bild2.png`, bindend die Lesung in `nutzer_teile/`.
Grundlage: Rohbau C (Sieger, 19 Punkte), dazu die Einfaelle aus den drei Urteilen. Generator neu als `rf_*.py`, die Kopie des Siegers `rc_*.py` liegt daneben.

## 1. Ergebnis

Messung `form_pruefung.py pruefe form.json`, `lauf/ende/bericht.txt`. Spalten C und B streng aus `urteil_mass.md` (gleiche Zuordnung: Oberkoerper nur 0, 7, 10, 11, 21, 22, 23).

| Teil | Bild | IoU | Deckung | Innen | C streng | B streng |
|---|---|---|---|---|---|---|
| tail | 1 | 0,89 | 0,95 | 0,92 | 0,91 | 0,89 |
| left_leg | 1 | 0,67 | 0,94 | 0,70 | 0,90 | 0,78 |
| right_leg | 1 | 0,77 | 0,92 | 0,83 | 0,86 | 0,90 |
| right_arm | 1 | 0,76 | 0,94 | 0,79 | 0,84 | 0,78 |
| part_of_Body | 1 | 0,81 | 0,89 | 0,90 | 0,83 | 0,78 |
| face_and_upper_body | 2 | **0,84** | **0,88** | 0,95 | 0,74 | 0,79 |
| left_arm_front | 2 | 0,78 | 0,81 | 0,96 | 0,82 | 0,69 |
| right_arm_front | 2 | 0,75 | 0,87 | 0,84 | 0,81 | 0,74 |
| Mittel Bild 1 / Bild 2 | | 0,779 / 0,790 | | | 0,867 / 0,790 | 0,827 / 0,742 |

Bild 1 liegt im Mittel unter C. Das ist der Preis zweier Auflagen: `left_leg` 0,67, weil das Bein jetzt den Keil fuellt (Keil 99 %, bei C 19 %), und `right_leg`, `right_arm`, weil links und rechts dieselben Glieder sind und die Finger in den Grenzen bleiben. Der Oberkoerper ohne Bein ist der beste aller Staende.

| Messung | Final | C | Soll / Bemerkung |
|---|---|---|---|
| Umriss-IoU Bild 1 / 2 | 0,950 / 0,940 | 0,972 / 0,963 | Weiss gedeckt 0,955 / 0,941 |
| Schwarz getroffen px | 186 / 57 | 112 / 87 | |
| Kantenpunkte | 21/31, 6/7 | 25/31, 7/7 | Bild 1 NEIN: Scheitel 3, rand_rechts (O1), Beine und Arm je 3,5-5,5 px |
| Boden, Verdeckung | 1/3, 7/9 | 1/3, 9/9 | L1 23 px, L2 69 px, BO 18 px, V6 -3,5 gegen Grenze 2,5 |
| Zeichnung | 6/9, 6/7 | 8/9, 7/7 | Abschnitt 5 |
| Maskenfrei, Abstand zur Bildkante, Mittel / Median | 3,46 / 2,2, 7,08 / 4,0 | 3,35 / 2,0, 6,94 / 4,2 | Nutzer-Soll 3,48 / 2,0, 5,65 / 2,2 |
| Rahmen | 1900 Flaechen, 32 Knochen, 384x256, 3 Clips, Boden je Pose y = 0 | | alles eingehalten |

**Was diese Zahlen beweisen.** IoU misst gegen die Masken, an die gerechnet wurde. Bestanden heisst: Es passt, nicht: Es ist belegt. Aussagekraft haben Einsatz, Rundblick, Biegeprobe und Spielbild. Y-Werte der Marken sind eingestellt, sie belegen nicht die Beta-Beleuchtung.

**Am Bild.** Im Einsatz (`mess/bild/nah_*_voll.png`, `auf_*`) liegt der Umriss in beiden Bildern eng am Original, Raute L2 und die Bogenform stehen. Im Rundblick (`rundblick_*.png`) steht ein schweres, tiefes Tier, spiegelgleich, mit klarem Kopf, Pranken und durchgehendem Schwanz. Im Spiel laedt es in beiden Raeumen und liest sich als dieselbe Figur. 1:1 ist es nicht, Abschnitt 7.

Spiel: `Bahnsteig-Form: 32 Meshes, 32 Knochen, 3 Clips`, drei Laeufe gdigrab, `spiel_pose2_r5090_s11.png`, `spiel_pose1_r5080_s1.png`, `spiel_ruhe_r5080_s1.png`.

## 2. Bau

Ring-Bauweise, keine Metaballs, kein Voxel, kein Skinning. Mesh i haengt an Knochen i. FORM.EMD md5 0d717025, FORM.TIM md5 8ad6f5f9.

| Gruppe | Knochen | Ecken je Ring | Flaechen |
|---|---|---|---|
| Rumpf (Huefte, Wirbel, Brust) | 0, 7, 10 | 10 | 310 (C 190, G4 435) |
| Hals, Kopf, Kiefer, 4 Fangzaehne, 2 Wangenspitzen | 21, 22, 23 | 8, 10, 8, 4 | 262 |
| Schwanz | 1-6 | 8 | 256 |
| Arme | 11, 12, 16, 17 | 8 | 288 |
| Beine | 8, 9, 24, 26 | 8 | 240 |
| Haende mit vier Fingern | 13-15, 18-20, 28-31 | 8, 6 | 400 |
| Fuesse mit drei Zehen | 25, 27 | 8, 4 | 144 |

- Mitte spiegelgleich gebunden (Ecke j = Ecke n-j, Mitte quer 0), links ist der Spiegel von rechts. Parameter gibt es nur je Meister-Teil. Volumen links/rechts 1,00 an allen Paaren, nur Hand und Finger 0,51 (`mess/paare.txt`): ein einziger Skalenfaktor `HAND_L = 0.80`.
- Eine Normale je Ecke wie RE2. Harte Spruenge 0 % (C 28 %), offene Kanten 0, Splitter 3, Durchblick 0 in allen drei Clips.
- Rumpf aus drei geschlossenen Stuecken mit Kuppel-Enden, vorderes Stueck traegt einen Teleskop-Rand ueber das hintere. Glieder wie C: Kind eingezogen, Enden geschlossen.
- Kopf: Hufeisen-Kamm, unten offen, Rand nach hinten gekippt (`Teil.dt`, Ecken eines Rings laengs versetzt), davor zurueckgesetzt Gesicht und schmale Schnauze.
- Passung `rf_fit.py`: Parameter nur Radien, Ringmitten, Gelenke, Wurzel. Neu gegen C: Sichtkennung (harte Kennung, wer steht vorn), Keile als Deckung, je Glied der tiefste Punkt am Boden, Glieder nicht im Rumpf, Bein nicht durch den Arm, Gelenkgrenzen samt Drehung, Rolle des rechten Unterarms.
- Atlas: je Teil ein benannter Streifen, U Umfang, V Laenge, `atlas.json`, `atlas_blick.png`, 0,029 Texel je Einheit. Zaehne und Zehen teilen je einen Streifen, sonst alles frei fuers Malen.

## 3. Was aus den Rohbauten kam

| Aus | Uebernommen |
|---|---|
| C | Grundstock Gerippe, Passung Radien und Ringmitten, Schwanz mit Keil an der Wurzel, Bein, Klauenspitzen als Punktziele, Rueckprojektion der Marken, Kind eingezogen |
| B | Spiegelgleiche Grundform, Rumpf fuellt den Oberkoerper ohne Bein, Kopf mit Schild und Schnauze, Zug Q als durchgehende Linie, Grundton nah am Video, Stufen im Beinumriss, Plattenrand am Rumpf |
| A | Pranke mit vier getrennten, gekruemmten Krallen, Kuppel-Enden, Keil und dunkle Stellen neben den Masken ernst nehmen, neutrale Ruhepose, Kopf zur Kamera (10 Grad, C 18) |
| RE2 | Eine Normale je Ecke, Zaehne in der Textur (Kopf 110 Flaechen, C 277) |

## 4. Maengel der Urteile

| Urteil | Mangel | Stand |
|---|---|---|
| Mass 1 | Oberkoerper nur 0, 7, 10, 11, 21, 22, 23 | erledigt, 0,84 / 0,88, rechtes Bein in der Maske 0,04 (C 0,146) |
| Mass 2 | Kein Glied ueber 0,10 im Rumpf | erledigt, alle Glieder unter 0,03 in Pose 1 und 2 |
| Mass 3 | Vier Glieder am Boden je Pose | erledigt, tiefster Punkt je Hand und Fuss hoechstens 23 Einheiten |
| Mass 4 | Fuss rechts Pose 2, Schulter an die Brust, Horn | erledigt: Fuss rechts Hoehe -2. Schulter Abstand zur Brust 4 und 3 (C 64). Sporn Hoehe 699 (C 1357), Spitze (143,355) |
| Mass 5 | Volumen links/rechts im Bericht | `mess/paare.txt`, Abweichung nur Hand, Beleg Abschnitt 8 |
| Mass | Durchdringung (C 0,07, B 0,77) | groesster Wert 0,06 (Kopf gegen rechten Arm) |
| Mass | Rechter Unterarm ausgerenkt, ungleiche Seiten | erledigt, Ellbogen Drehung -5 und -14 Grad |
| Bild | Rhombus Bein/Schwanz | erledigt, Keil links 99 %, rechts 100 % |
| Bild | Marken zu gross, zu hell | Z5 19x9 gegen 15x9 (C 29x28), Y 80 gegen 75 (C 196). Z1 323 gegen 290 px, Z3/4 298 gegen 308 px, X 989 gegen 911 px |
| Bild | Unterarm G | 90 % der Laenge (C 77), Rot 119 gegen 157 (C 80), Rippen aus dem Bild |
| Bild | Rotes Licht Bild 2 | Zonen Rotbraun auf Handruecken und Armfront, nur was Bild 1 nicht sieht |
| Bild | Dunkelzone zwischen den Armen | 72 % (C 27 %), linkes Bein steht dort |
| Bild | Bogen unterbrochen | Zug Q als Linie, im Filter lesbar, noch gestrichelt |
| Spiel | Schief, Spiegelabstand bis 155 | erledigt, 0 ausser Hand |
| Spiel | Kopf Stachelball, Hals Stufenringe | erledigt, vier Fangzaehne, Zahnreihe in der Textur |
| Spiel | Rumpf 190 Flaechen, Saegeschnitte | 310 Flaechen, Kerbe Rumpf hoechstens 0,08, Krater 0 |
| Spiel | Pose 2 Bein schwebt 266 | erledigt |
| Spiel | Schwanz als Buckel in Ruhe | erledigt, Ruhe laeuft aus dem Ruecken zum Boden. In Pose 1 bleibt er, Bild 1 verlangt ihn |
| Spiel | Ruhepose nur Pose 1 mit Kopf hoch | erledigt, neutral und spiegelgleich, Bauch rund 690 ueber Boden |

Sporn zu Mass 4: Er laeuft nach hinten aussen, kein Horn ueber der Hand mehr. Im Rundblick bleibt ein langer Aussenfinger, in Ruhe liegt er nach vorn an. Fingerspitzen Bild 2 gegen die Umriss-Spitzen: 0,6 bis 5,5 px (`mess/m2.txt`). Der Bericht meldet (274,443) als fehlend, weil seine Spitzen Gipfel des Umrisses sind, der Daumen trifft die Stelle auf 0,6 px.

Biegeprobe (`mess/spiel/biege.txt`, `krater.txt`, `spalt.txt`, Blatt `biegeprobe.png`): Krater 0 an Ellbogen und Knie. Kerbe hoechstens: Ellbogen 0,16, Knie 0,04, Wirbel 0,08, Brust 0,00, Schwanz 0,15 (bei 15 Grad). Kein Riss, Kind im Eltern 0,09-0,35.

## 5. Zeichnung

Marken in echter Form statt ganzer Prueffelder: Pixel ueber halber Hoehe zwischen Umgebung und Spitze (`rf_textur.marke_form`). Farbe je Marke aus der Stellschleife `rf_stell.py`: Render gegen zurueckgerechnete Schirmfarbe, Grenzen je Marke. Faktoren `zw/marken.json`.

Die Pruefung faellt dadurch ehrlich tiefer als bei C. Sie zaehlt Material im Albedo und belohnt Uebergroesse.
- Z4: Material 0,00, im Bild Y 136 gegen 141. Faktor unter der Palette, weil L0 dort Faktor bis 1 gibt.
- Z5: Material 0,39, im Bild 19x9 gegen 15x9, Y 80 gegen 75. Echte Groesse deckt das Polygon nicht.
- Z9: nicht gebaut, liegt auf O1.
- Q5: 0,55, knapp unter 0,6.
- Bild-1-Stellen, die zu hell oder zu dunkel bleiben: Z2 Y 72 gegen 130, Z3 189 gegen 172, Z1 160 gegen 142.

Rechter Unterarm: Bild 1 sieht ihn von hinten, Bild 2 den Streifen G, und dieselben Ecken lagen zunaechst in beiden Bildern. Deshalb rollt der Unterarm in Pose 1 rund 60 Grad (Oberarm Drehung +50), Bild 1 sieht die Ecken 1-5, Bild 2 die Ecken 5-8. Der Rest bleibt in Bild 1 dunkel, N2 0,006.

## 6. Gerippe und Posen gegen FORMBLATT2

| Aenderung | Grund |
|---|---|
| Pose 2 neu: Becken tief, Rumpf rund 45 Grad, Wurzelhoehe 1113, sitzend aufgestuetzt | Nur so fuellt der Rumpf den Oberkoerper-Umriss links unten ohne Bein. Mit dem Gerippe (Hoehe 1543) stand dort das Bein |
| Pose 2: rechtes Bein unter dem Unterarm, linker Fuss in der Dunkelzone auf V3 | Beide Fuesse am Boden, kein Bein links neben dem Umriss |
| Pose 1: linker Arm hinter dem rechten Bein, Handgelenk (328,353), Hand am Boden | L2 frei ohne den Trick von B |
| Pose 1: rechter Oberarm gedreht | Abschnitt 5 |
| Finger: Wurzeln auf Handbreite, Grundspreizung im Mesh (14, 0, -26, 50 Grad) | Der Faecher von Bild 2 braucht so nur rund +-30 Grad im Gelenk |
| Oberschenkelkopf kurz, Kopf hinten schmal | Innen-Test, rechter Oberarm und Kopf |
| Gier Bild 2 3871 statt 3917, Lage (941,-23776) | Passung. Gier bleibt in der Spanne aus FORMBLATT2 Abschnitt 4 |

Laengen und Versaetze ausser Fingern unveraendert, Knochen 10, 13, 18 bleiben fuer den Tick.
Gelenke: Ellbogen und Knie Drehung hoechstens 18 Grad (Grenze 20), Knoechel 38 (Grenze 40). Rest knapp ueber Grenze: Fingerrolle bis 3,7 Grad, Finger Y 31-32 gegen 30, `fuss_r` Y 40,6 gegen 40, Unterarm rechts X 12,9 gegen 12 (`mess/m2.txt`). Rechter Oberarm Pose 2 steht weiter am Drehlimit (+99), Erbe des Gerippes.

## 7. Was im Bild noch abweicht

Bild 1:
- Oberflaeche: Das Original zerfaellt in Lappen, Baender und Glanzpunkte, das Modell hat Rippen im Umriss, sonst glatte Flaechen mit Marken. Das ist das Malen.
- Rechts neben dem Arm (405-429,262-300) ist nichts gebaut, dort steht im Video dunkle Masse. Kante -13,5 px, Z9 fehlt. Siehe Abschnitt 8.
- Oberkante Schwanz: x 322 um 9,5 px zu tief, x 300 um 5,5 px zu hoch.
- L2 69 px (C 25), L1 23 px, Rand von Schwanz und linker Hand.
- Spalt Brust/rechter Arm ist schmal, aber noch offen.

Bild 2:
- Zug Q Y 38 gegen 58, im Filter noch gestrichelt. Q4 Y 55 gegen 96, P Y 61 gegen 116, liegt nur zu 0,52 auf der Figur.
- Unterarm: Rot 119 gegen 157, oberes Viertel des Streifens schwach. Der Feuerschein fehlt, die Engine hat in ROOM5090 kein Farblicht.
- Innen keine Gesichtszuege. Das Maul ist im Spiel als dunkelrote Raute lesbar, im Video nicht.
- Kante (411,206) -3,5 gegen Grenze 2,5, BO 18 px. Hand rechts deckt ihren Umriss zu 0,87, links unten bleibt Maske frei.
- Rechter Oberarm steckt 0,21 im Kopf (C 0,27), kein Bild zeigt das. Kreis Zug Q zu 1,00 vom Kopf gedeckt, vom Kopf liegen 0,84 im Kreis.

Rundblick und Spiel:
- Saegezacken an Ueberlappungen (Schwanzwurzel, Huefte/Oberschenkel, Kopfansatz): Maler ohne Tiefenpuffer, wie im Urteil auch bei RE2 gemessen, im Ton deutlich, auf der dunklen Textur kaum.
- Hand rechts ist breit und flach, die linke kleiner. In Ruhe stehen die Fuesse 22 ueber dem Boden, Haende tragen.
- Spiel Bild 1 hat keinen Schnitt: Schnitt 1 zeigt die Figur klein von der anderen Seite.

Nicht geprueft: bewegte Clips, Tick-Clips 3 bis 27 (`FE.pruefe` warnt), Kasten, Schatten-Festwerte, Vordergrundmasken, PSX-Last (als Dreiecke rund 3200).

## 8. Fragen an den Nutzer

1. **O1:** Was liegt rechts neben dem Arm (405-429,262-300)? Blatt `urteil_bild/o1_blick.png`. Das Bild zeigt dunkle Masse, die Teile nennen nichts.
2. **Hand:** Darf die rechte Hand groesser sein als die linke? Gesetzt: linke Hand 0,80, Volumen 0,51. Beleg: Handschnitte 110 gegen 52 px bei Tiefenverhaeltnis 1,25, Wortlaut "grosse Klauenhand" gegen "Klaue". Spiegelgleich ging links im Bild deutlich schlechter (`left_arm_front` 0,58 im Lauf s2). Das bleibt Lesart, kein Beweis.
3. **Pose 2:** Stimmt die sitzende Haltung, Becken tief? Sie folgt aus "Oberkoerper ohne Bein". Das Bild zeigt das Becken nicht.
4. **Keil links:** Hier ist es das linke Bein. Es kann auch etwas anderes sein.

## 9. Frei gesetzt

Gesicht, Kieferform, Kammform, Fangzaehne, Wangenspitze (Spitze zielt auf P), Zahnreihe, Augen, Fingerzahl, Zehen, Rumpfseiten, Ruhepose, alle Ring-Reliefs. Belegt sind nur Umrisse und Lage aus den Bildern.

## 10. Durchgaenge

| Lauf | Aenderung | Befund |
|---|---|---|
| s1 | Pose 2 ueber Anker neu, Becken tief | Erst Hueftgelenk als Anker, Brust sackte ab. Anker aufs Rumpfende, Rumpf 45 Grad |
| s2 | Spiegelbindung, strenge Masken | Mittel 0,79 / 0,67, Hand links steht ueber, linker Arm in L2 |
| s3 | Anker linker Arm, linke Hand 0,82 | Oberkoerper 0,83, L2 frei |
| s4 | Kamm neu, rechte Hand groesser | Passung blaest den Hals auf, verworfen |
| s5 | Hals festgehalten | Bester Zwischenstand 0,81 / 0,80, Spiel laeuft |
| s6-s8 | Spalt ueber den Rumpf | Brust waechst ueber den Arm, Drift, verworfen |
| s9-s10 | Von s5: Rumpf in Pose 1 fest, Rolle Unterarm | Streifen G frei von Bild 1 |
| s11-s12 | Kante 7, V1-V5 als Pflicht, Farben enger | Zeichnung 7/9, 7/7 |
| s13 | Oberschenkelkopf, Kopf hinten | Innen-Test unter 0,03, aber P verloren |
| s14, ende | P zurueck, N2 | Abgabe |

Falle im eigenen Aufruf: Python schreibt unter Windows CRLF, das CR hing an der Gier, `sscanf` im Spiel verwarf die Pose, Meldung `ANWESEND=0`. Das sah wie ein flatterndes Spiel aus. `rf_spiel.sh` entfernt das CR.

## 11. Dateien und Lauf

Generator: `rf_kern.py` (Knochen, Ringteile, Bindung, Raster), `rf_plan.py` (Bauplan), `rf_fit.py` (Passung), `rf_s1.py` bis `rf_s5.py` (Stufen), `rf_ruhe.py` (Clip 2), `rf_bau.py` (EMD, Atlas, Normalen), `rf_textur.py`, `rf_marken.py`, `rf_stell.py`, `rf_abgabe.py`, `rf_ton.py`, `rf_biege.py`, `rf_spiel.sh`. Sichten: `rf_auflage.py`, `rf_blick.py`, `rf_gelenke.py`, `rf_seiten.py`.

```bash
cd analysis/befunde_runde41/form/rohbau_final
export PYTHONIOENCODING=utf-8
python rf_bau.py stand_final.json --tag ende --voll     # FORM.EMD, FORM.TIM, form.json, atlas.json, lauf/ende (mit zw/ruhe.json, zw/marken.json)
bash rf_abschluss.sh stand_final.json                    # alles neu, samt Ruhepose, Marken, Spiel, Messreihe
```

Der EMD-Stand ist ueber zwei Abschlusslaeufe bytegleich (md5 0d717025). `stand_final.json` entstand ueber die Kette in Abschnitt 10, mit Code-Aenderungen dazwischen, ein Lauf von vorn ist nicht geprueft.

Abgabe: `FORM.EMD`, `FORM.TIM`, `form.json`, `stand_final.json`, `gerippe_final.json`, `atlas.json`, `atlas_blick.png`, `textur.png`, `vergleich_bild1.png`, `vergleich_bild2.png`, `teile_bild1.png`, `teile_bild2.png`, `rundum.png`, `rundum_hell.png`, `rundblick_pose1.png`, `rundblick_pose2.png`, `rundblick_ruhe.png`, `biegeprobe.png`, `spiel_pose1_r5080_s1.png`, `spiel_pose2_r5090_s11.png`, `spiel_ruhe_r5080_s1.png`, `bericht.txt`, `lauf/ende/`. Messreihe in `mess/` (Skripte der Urteile, auf diesen Stand umgestellt).

Kein Commit. Geschrieben nur in diesem Ordner und in `re15_port/build/form_spiel/`, eigene Laufordner danach geloescht. Hauptbaum, `pri/`, `synchro/` nicht beruehrt.
