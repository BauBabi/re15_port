# Urteil Spielmodell (Linse SPIELMODELL), Rohbauten A, B, C

Stand 2026-11-01. Unabhaengiger Pruefer, an keinem Rohbau beteiligt. Frage: Taugt das als Boss von 1997, ohne Ruecksicht auf die Screenshots?
Punkte sind Urteil am Bild, die Zahlen stuetzen es. Eichmass nur fuers Handwerk: RE2-Modelle K34 (G4), K33, K28 (geht auf allen vieren, mit Schwanz), K23 (Alligator), K22, K20. Keine Formvorlage. Belege und Skripte: `urteil_spiel/`.

## 1. Urteil

| Rohbau | Punkte | Kurz |
|---|---|---|
| B | **7** | Staerkste Gestalt, traegt jede Pose, Glieder und Rumpf biegen sauber. Haende splittrig, Seitensporn, Hand am Gelenk offen |
| C | **6** | Bester Schwanz und bestes Bein, Gliedenden sauber. Aber schief, auf die Bildposen gezogen, rechter Unterarm ausgerenkt, Kopf ein Stachelball |
| A | **5,5** | Dichtestes Netz, beste Hand, Rumpf haelt am besten. Aber hochbeinig, Krater an Knie und Ellbogen, Schwanz sitzt lose auf dem Becken |

Keiner ist abnahmefaehig, G4 laege im selben Massstab bei rund 9. Alle drei sind besser gebaut als die Kandidaten der Vorrunde (B 6, A 5,5, C 5): Der Rumpf reisst nicht mehr, alle stehen auf demselben Gerippe, keiner ist ein Ballonmodell im Sinn von Kandidat A.

## 2. Teilnoten (Urteil)

| Kriterium | A | B | C | Tragender Befund |
|---|---|---|---|---|
| Starke Gestalt | 5,5 | 8 | 6 | B: tief und schwer, Massenverteilung wie G4. A: hochbeinig. C: kraeftige Linie, aber Schwanzbuckel und schief |
| Flaechenfuehrung | 6 | 6,5 | 6 | A: gleichmaessige Vierecke. B: klare Ebenen, 4,8 % Splitter. C: keine Splitter, aber Keile an Kopf und Haenden, 190 Flaechen fuer den Rumpf |
| Verjuengung | 5 | 6,5 | 7 | C: Schwanz und Unterschenkel. A: Schwanz mit Taille und Blatt |
| Keine Ballons | 5 | 8 | 7 | A: Schulter-Eier, Kopfkugel. B, C: keine |
| Keine Risse | 8 | 7 | 8 | Rumpf und Glieder bei allen dicht. B offen an Hand und Klauen, A Becher an der Ferse |
| Nichts schwebt | 6,5 | 6 | 6 | Abschnitt 4 |
| Biegt sauber | 5 | 7,5 | 6 | Abschnitt 3 |

Mittel 5,7 / 7,0 / 6,3. Die Endnote ist kein Mittelwert: Maengel, die nur ein Neubau behebt (Stand und Proportion A, Fit-Verzug C), wiegen schwerer als solche, die sich nachbessern lassen (Sporn, Haende B).

## 3. Biegeprobe

**Winkel.** Aus `gerippe.json` (unterarm z -155..+2, unterschenkel z -2..+165, wirbel z +-50 y +-35, schwanz z, y +-50), gegengeprueft an RE2. Knick zweier Knochen in allen Bildern des ersten Clip-Paars von K34, K33, K28, K23, K22:

| Gelenk | p50 | p90 | p99 |
|---|---|---|---|
| Ellbogen | 27-96 | 45-127 | 63-158 |
| Knie | 59-110 | 102-152 | 128-169 |
| Wirbel | 23-25 | 46-54 | 59-67 |
| Schwanz je Glied | 10-24 | 19-39 | 24-58 |

**Aufbau.** Bindepose, jedes Gelenk einzeln, Ellbogen und Knie 0 bis 150 Grad, Wirbel, Brust, sechs Schwanzgelenke +-15, 30, 45 Grad um z und y. Weil alle drei dasselbe Gerippe tragen, misst das die Teile am Gelenk. Dazu ganze Koerperposen und identische Posen vom Gerippe aus, Abschnitt 5.

**Masse.** Festlegungen dieses Pruefers, keine RE2-Regel.
- Kerbe: ungedeckter Anteil im Dreieck Aussenkante Eltern, Aussenkante Kind, Gelenkpunkt. Silhouette aus Vorderseiten, Blick entlang der Achse. Erst ab 30 Grad, darunter ist das Dreieck zu schmal.
- Krater: von aussen sichtbare, nach innen zeigende Flaeche relativ zum Gliedquerschnitt. Hohl = Aussennormale zur Mittellinie des Teils unter -0,3.
- Eintauchen: Anteil des Kindes im Eltern-Volumen (Windungszahl).

| Gelenkklasse | Kerbe max A / B / C | Krater max A / B / C | Am Bild |
|---|---|---|---|
| Ellbogen | 0,00 / 0,00 / 0,09 | 0,44 / 0 / 0 | A offener Trichter ab 90 Grad. B am ruhigsten. C links gut, rechts ausgerenkt |
| Knie | 0,00 / 0,00 / 0,00 | 0,60 / 0 / 0 | A Ringkrater. B Sternkappe mit Zacken. C geschlossener Stumpf |
| Wirbelsaeule | 0,03 / 0,00 / 0,09 | 0,04 / 0,07 / 0 | A und B dicht. B bei 35 Grad seitlich klafft der Plattenrand. C flache Saegeschnitte, ab 45 Grad eingeschnuert (Dicke 0,82) |
| Schwanzglieder | 0,00 / 0,19 / 0,10 | 0,34 / 0 / 0 | A hohl, aussen glatt. B Platten teleskopieren, nach unten bis 0,19 Stufe. C spitze Kerben |
| Schwanzwurzel | 0,19 / 0,02 / 0,00 | 0,35 / 0 / 0 | A Stiel: 2 % eingesteckt, Schlitz 773 px. B 26 %, 32 px. C 35 %, 11 px, massiger Keil |

Krater im Mittel beider Seiten: A Ellbogen 0,16 bei 90 Grad, 0,30 bei 120, 0,42 bei 150, Knie 0,20 / 0,37 / 0,53. RE2 am p90-Bild: Ellbogen Median 0,08, hoechstens 0,17, Knie Median 0,06, hoechstens 0,16, ein Ausreisser am Schwanzansatz von K28 mit 0,45. Bis 90 Grad liegt A am oberen Rand von RE2, darueber klar ueber allen Gliedern. Knie p90 liegt bei RE2 ueber 100 Grad.

**Ergebnis.**
- Kein Gelenk reisst auf. Groesster Flaechenabstand ueber den ganzen Bereich unter dem Probenabstand von 14 Einheiten.
- Die Silhouette trennt nicht, RE2 bleibt dort ebenfalls bei 0. Die Unterschiede liegen im Knick.
- RE2 steckt Teile teleskopartig ineinander (`eich_gelenke.png`): Eltern weit, Kind eingezogen, jeder Abschnitt verjuengt sich. B und C arbeiten so, A setzt hohle Rohre stumpf aneinander.
- Zacken am Knick zeigt RE2 ebenfalls. Sie zaehlen hier nicht als Fehler. Malerfehler ohne Tiefenpuffer: alle 4-6 %, RE2 4-5,5 %.
- C rechter Ellbogen: Eintauchen 1 %. Der Unterarm liegt in gerader Stellung neben dem Oberarm und sitzt erst bei 60-90 Grad, also in der Bildpose (`biege_ellbogen_r.png`). Die Passung hat das Teil auf die Pose gezogen. Das ist kein Biegefehler im engen Sinn, aber die Animation trifft genau diese Winkel.
- Im Spiel haelt die Kauer mit 120 Grad bei allen drei (`spiel_tafel_neutral.png`). Der Krater von A faellt in dieser Groesse auf dunkler Haut kaum auf. Das Urteil dazu ruht auf Ton und Messung, nahe Kameras sind nicht geprueft. Ich werte ihn deshalb als Mangel, nicht als Sperre.

## 4. Messwerte

**Durchblick** = Pixel, die nur eine Rueckseite traegt, Typ 0x30 zeichnet Rueckseiten nicht. Summe ueber 60 Blicke, Oeffnung 3 x 3. Gesamt / nur Rumpf und Glieder.

| Clip | A | B | C |
|---|---|---|---|
| Ruhe | 0 / 0 | 2974 / 471 | 9438 / 0 |
| Pose 1 | 0 / 0 | 14214 / 169 | 8848 / 0 |
| Pose 2 | 0 / 0 | 14108 / 4397 | 7820 / 0 |

Die Gesamtzahl taeuscht. Bei C sind es einseitige Zaehne und Zehen (`kopf`, `fuss`), sie verschwinden nur von hinten. Bei B dasselbe, dazu die Hand: `hand_l` 2827, `hand_r` 1570, `klaue_l4` 7187 in Pose 2, die Hand ist am Gelenk offen. Groesstes Einzelloch im Koerper 515 px von 900 x 900 (`lochkoerper_B_pose2.png`). RE2 am selben Mass: K34 1312, K33 551, K28 147, K23 603.

**Hoehe ueber dem Boden**, tiefster Punkt je Glied, Beine r, l / Arme r, l:

| Clip | A | B | C |
|---|---|---|---|
| Ruhe | 62, 62 / 0, 10 | 70, 70 / 2, 0 | 5, 14 / 0, 9 |
| Pose 1 | 109, 14 / 1, 0 | 39, 0 / 1, 804 (gehoben, hinter dem Rumpf) | 5, 14 / 0, 9 |
| Pose 2 | 0, 4 / 61, 8 | 56, 53 / 5, 43 | **266**, 0 / 36, 25 |

**Netz und Proportion**

| | A | B | C | K34 | K28 |
|---|---|---|---|---|---|
| Flaechen, davon Vierecke | 1917, 1739 | 1876, 1334 | 1807, 1220 | 1427, 486 | 913, 374 |
| Als Dreiecke gerechnet | 3656 | 3210 | 3027 | 1913 | 1287 |
| Offene Kanten | 0 | 174 | 91 | 19 | 49 |
| Splitter (Laenge/Hoehe ueber 8) | 13 | 153 | 0 | 55 | 7 |
| Harte Normalenspruenge, Anteil Kantenlaenge | 20 % | 26 % | 28 % | 0 | 0 |
| Normalen gegen Geometrie, Median | 18,7 Grad | 23,8 | 23,8 | 9,4 | 7,6 |
| Bauch ueber Boden, Ruhe | 1460 | 546 | 902 | | |
| Masse Rumpf / Kopf / Schwanz in % | 50 / 20 / 5,8 | 60 / 13 / 6,6 | 52 / 19 / 9,3 | 59 / - / 0 | 52 / - / 9,7 |
| Spiegelabstand je Teil | 0-92 | 0-14 | 41-155 | | |
| Flaechen Rumpf / Haende / Kopf | 280 / 312 / 327 | 320 / 272 / 256 | 190 / 432 / 277 | 435 / - / 97 | |

Schwanz, Radius entlang der Kette, Wurzel zur Spitze: A 200, 260, 245-160, 140-100, 95-115, 250, 53: Stiel, Taille, Blatt. B 300-350, 350, 300-250, 200-180, 150-200, 228, 53: Stufen je Platte. C 300-380, 310, 345-180, 260-140, 155-170, 200, 30: durchgehend bis auf das rosa Ende. K28 280, 210, 165-110, 100-80, 70-4.
Unterschenkel, Verjuengung vorderes zu hinterem Drittel: A 1,27, B 1,30, C 1,61, K28 1,29. Oberarm Bauch: A 1,50, B 1,31, C 1,33, K28 1,34.
Lose Stuecke: nur Zaehne, Hauer, Schuppen, 8-36 Einheiten neben der Flaeche, im Spiel unter 3 Pixeln.
`FE.pruefe`: je 0 Fehler, eine Warnung, Tick-Clips fehlen. Gehoert zur naechsten Stufe.

## 5. Am Bild

Ton (`ton_*.png`) zeigt die Form ohne Textur. Spielbilder: 36 Laeufe, gdigrab, alle rc=0, `spiel_tafel_ganz.png`, `spiel_tafel_nah.png`, `spiel_tafel_neutral.png`, Einzelbilder `spiel/`. `klein_alle.png`: Figur 100 Bildpunkte hoch.

### B, 7 Punkte
Stark: Einziger Boss der drei. Tiefer Schwerpunkt, Rumpf 60 % wie G4, klarer Kopf mit Brauengrat, Schnauzenflaechen, Kiefer (`nah_kopf_ton.png`). Die Plattenlinien geben dem dunklen Koerper Rhythmus und tragen noch auf 100 Bildpunkten, im Spiel liest man Schulter, Rippenplatten, Kopf und Klaue (`spiel_tafel_nah.png`). Haelt identische Posen ohne Bruch (`neutral_kauer.png`, `neutral_kurve.png`), der Schwanz geht als Fortsetzung des Rueckens heraus.
Noch nicht gut:
- Seitensporn: Der aeussere Finger steht in Ruhe flach zur Seite, Breite 4911 statt rund 3000. Von vorn und hinten wirkt das wie zwei Lanzen am Boden.
- Haende und Knoechel unruhig: Bruchstuecke, die sich durchdringen (`nah_hand_ton.png`, `nah_knoechel_ton.png`).
- 153 Splitterflaechen, alle an den Plattenraendern: Brust 60, Huefte 37, Wirbel 35, Hals 16, Kopf 5. An Haenden und Fuessen keine.
- Hand am Gelenk offen, Abschnitt 4. In einem stark gebeugten Handgelenk sieht man durch.
- Rand des rechten Oberschenkels (Knochen 9) steht in Pose 2 stirnseitig zur Kamera, im Spiel wie ein Rohrende (`aus_B_ring_spiel.png`, bei (222,298)). Kein Loch, die Flaeche deckt.
- Zaehne als Leiste duenner Kegel, zerfallen in Spielgroesse.
- Beine 70 ueber dem Boden in Ruhe, die Klauen der Haende tragen allein.

### C, 6 Punkte
Stark: Glieder enden in geschlossenen Stuempfen, Krater 0. Bestes Bein: Tropfen-Unterschenkel mit Verjuengung 1,61, gebogener Fuss (`nah_knoechel_ton.png`). Bester Schwanz, massiger Keil an der Wurzel, Radius faellt durchgehend. Keine Splitter. Bodenlage in Ruhe am saubersten. In 3/4-Sicht von hinten die staerkste Linie aller drei (`ton_C_ruhe.png`).
Noch nicht gut:
- Schief: Spiegelabstand bis 155, von vorn und hinten wirkt die Figur verdreht (`ton_C_ruhe.png` vorn, hinten). Das ist Bildnaehe, kostet als Spielmodell aber viel, denn die Figur dreht sich im Spiel.
- Schwanz liegt in Ruhe und Pose 1 als zweiter Buckel auf dem Ruecken. Das kommt aus der Pose: In der Kauer waechst er sauber aus dem Rumpf (`spiel_tafel_neutral.png`), dort am schluessigsten der drei.
- Rechter Unterarm auf die Bildpose gezogen, Abschnitt 3.
- Kopf: Zaehne rundum, Stachelball, Hals als Stufenringe (`nah_kopf_ton.png`).
- Rumpf nur 190 Flaechen, 432 in den Haenden. Rumpfstuecke enden flach, bei 30-45 Grad sieht man Saegeschnitte (`biege_brust_z.png`). Handflaechen kastig.
- Pose 2: rechtes Bein 266 ueber dem Boden, das schwebt sichtbar.
- Im Spiel bleibt in Spielgroesse fast nur Masse, wenig Binnenform.

### A, 5,5 Punkte
Stark: 0 offene Kanten, 0 Durchblick in jeder Pose. Beste Hand: Pranke mit vier klar getrennten Krallen (`nah_hand_ton.png`). Rumpf biegt am ruhigsten, Kerbe hoechstens 0,03, Teile tauchen 19-31 % ein. Gleichmaessige Vierecke. Atlas je Teil ein benanntes Rechteck, 78 % belegt.
Noch nicht gut:
- Hochbeinig: Bauch 1460, Beine als Pfeiler, Schultern als Eier auf Saeulen, im Spiel Stempelbeine (`spiel_tafel_nah.png`, `A ruhe_g2145`). Das liest sich als Tier, aber nicht als Boss.
- Krater an Knie und Ellbogen ab 90 Grad, Abschnitt 3. Die hohle Bauweise zeigt sich schon in der Abgabe: Becher an der Ferse in Ruhe, Mulde vor den Zehen in Pose 2 (`nah_knoechel_ton.png`).
- Schwanz sitzt als Balken auf dem Becken (`neutral_bind.png`, `biege_wurzel_z.png`), im Spiel eine Stange ueber dem Rumpf (`spiel_tafel_neutral.png`, Zeile 1). Stiel, Taille, Blatt, duennster der drei.
- Kopf ist eine Kugel, 20 % der Masse.
- Krallen breit wie Blaetter und hell, sie ziehen das Auge von der Figur ab.
- Rechteckige Felder auf Ruecken und Flanke im Spiel.

## 6. Allgemeine Befunde

- **Normalen.** RE2 hat je Ecke genau eine Normale, 0 harte Spruenge. Die Rohbauten 20-28 %. Versuch `glatt_versuch.png`, gleiche Netze: Die Lichttreppen von A verschwinden, die Glieder von C lesen sich rund, B bleibt klar. Billigste Verbesserung ueberhaupt. Nicht in ROOM5080 im Spiel geprueft, die Rohbauten biegen die Normalen teils wegen des steilen Lichts dort (B `NORM_U0`).
- **Kleinteile.** RE2 legt Zaehne in die Textur. K34 Kopf 97 Flaechen, Rohbauten 256-327. Das Budget gehoert in den Rumpf: K34 435, C 190.
- **Polygonlast.** Rahmen 1900 Flaechen eingehalten, als Dreiecke aber 1,6-1,9-mal G4. Fuer das PSX-Ziel pruefen.
- **Dunkelheit.** Folgt aus `farbe.md`, kein Fehler. Trotzdem kommt die Binnenform nur ueber helle Linien und Klauen, siehe `klein_alle.png`.

## 7. Das Beste je Rohbau

**A:** geschlossene Teile, aber Kappen gewoelbt statt hohl. Hand als Pranke mit vier getrennten Krallen (`rohbau_a/entwurf.py`), schlanker und gekruemmter. Rumpfstuecke mit Kugel in Pfanne, 19-31 % Eintauchen. Atlas je Teil ein Rechteck.

**B:** Gestalt und Proportion als Grundlage (`rohbau_b/figur.py`). Kopfaufbau mit Brauengrat und Schnauzenebenen. Plattenrand als Teleskop an Rumpf und Schwanz (`rohbau_b/ringe.py`). Schwanz als Fortsetzung des Rueckens. Symmetrie links gleich rechts. Passung nur ueber ganze Ringe (`rohbau_b/anpassen.py`), das schont die Form.

**C:** Gelenkabschluss Kind eingezogen, Ende geschlossen, Krater 0 (`rohbau_c/rc_kern.py`). Schwanzwurzel als Keil und durchgehende Verjuengung (`rohbau_c/rc_plan.py`). Unterschenkel und Fuss. Keine Splitter. Bodenkontakt in Ruhe. Klauenspitzen als Punktziele.

**Vorschlag:** Gestalt, Kopf und Platten B, Gliedenden, Bein und Schwanzwurzel C, Hand A, eine Normale je Ecke, Zaehne in die Textur, Mitte symmetrisch, Passung nur ueber ganze Ringe. Pflicht davor: Seitensporn von B weg, Handgelenk schliessen, Beine in Ruhe auf den Boden. Plattenraender ohne Splitter bauen. Im Normalen-Versuch laufen die Plattenlinien weich aus, die Rille braucht also echte Tiefe.

## 8. Korrekturen waehrend der Pruefung

- `rohbau_a/spiel_*.png` sind aelter als `FORM.EMD`, zeigen aber denselben Stand: Pose 1 neu gegen Abgabebild 0 Pixel Abweichung. Kein Mangel.
- Ring am Fuss von A in Pose 2: Feuer vor der Ferse, kein Loch. Ring bei B: Plattenrand, kein Loch.
- Splitter von B: erst an den Haenden vermutet, gezaehlt sitzen alle am Rumpf.
- Schwanzmasse K28: erst 24 % gerechnet, Knochen 16 enthaelt das Bein. Richtig 9,7 %, Alligator 31,8 %. C liegt also auf K28-Niveau, nur A ist wirklich duenn.
- UV-Inseln (A 343) sagen hier nichts: Der Atlas von A ist je Teil ein Rechteck. Darum keine Wertung daraus.
- Treppen im Ton von A: erst fuer Tiefensortierung gehalten, sie verschwinden aber mit einer Normale je Ecke. Maler-Mass trennt nicht.

## 9. Grenzen

- Gesetzte Posen, keine Animation. Bodenlage der Biegeposen nicht bewertet, die Pose Biss hebt die Beine von C durch die Hebelverhaeltnisse.
- Spielbilder nur ROOM5080 Schnitt 1 und ROOM5090 Schnitt 11. Dort verdeckt Feuer die Beine. Tick-Clips, Teile 10, 13, 18, Kollision, Schatten nicht geprueft.
- Volumen an offenen Netzen ist Naeherung, die Massenanteile sind Anhalt.
- Texturen sind Zonen, nicht gemalt, und zaehlen hier kaum.
- Linse ist ohne Screenshots. Das Ziel 1:1 prueft eine andere Linse, dort mag C vorn liegen.

## 10. Dateien (`urteil_spiel/`)

| Datei | Inhalt |
|---|---|
| `ton_{A,B,C}_{ruhe,pose1,pose2}.png`, `ton_K*.png` | Tonblaetter, RE2 als Eichmass |
| `biege_*.png`, `eich_gelenke.png`, `nah_*.png` | Nahblaetter Biegeprobe, RE2-Gelenke, Kopf, Hand, Fuss, Knoechel |
| `posen_{A,B,C}.png`, `neutral_{bind,kauer,sprung,kurve}.png`, `glatt_versuch.png` | Koerperposen, identische Posen, Normalen-Versuch |
| `spiel_tafel_*.png`, `spiel/` | Spielreihe, 36 Bilder |
| `klein_alle.png`, `tex_{A,B,C}.png`, `aus_*.png`, `lochkoerper_*.png`, `loch_*.png` | Spielgroesse, Atlanten, Ausschnitte, Lochkarten |
| `biege.json`, `biege_zusammen.json`, `krater.json`, `mass.json`, `netz.json`, `posen.json`, `spalt.json`, `ausschlag.json` | Zahlen |
| `u_*.py`, `u_spielreihe*.sh`, `*_lauf.txt`, `spiel_reihe*.log` | Skripte, Lauflogs |

Kein Commit. Nur unter `form/urteil_spiel*` geschrieben, dazu Eichmodelle und Biege-EMD im Scratchpad. Hauptbaum, `pri/`, `synchro/` nicht beruehrt. Nur eigene PIDs, eigene Laufordner geloescht.
