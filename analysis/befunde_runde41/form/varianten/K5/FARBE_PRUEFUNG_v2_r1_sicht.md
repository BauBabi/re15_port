# K5c Farbe, Versuch 2 Runde 1: Sicht-Pruefung (Pruefer "Auge des Nutzers", 2026-11-07)

Gegenstand: Tafeln 01-08 in `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe/` (Stand 01:14-01:20,
Commit e38c7996c), die zehn Spielbilder `farbe/spiel/` (`spiel_lauf.log` 10/10 rc=0, gdigrab Versuch 1, K5c V2 gegen K4 heute), Riegel
`farbe/riegel_K5c_farbe_v2.txt` (38/45, EXIT 1), Dossier `FARBE.md` Abschnitt "Versuch 2", Generator `generator/kf_mal4.py`, die Originale
`C:/workspace/git/reAi_v2/Version_80%/vlcsnap-2026-10-31-00h28m45s774.png` (Bild 1) und `...00h29m02s830.png` (Bild 2), Entscheide E1-E28.
Massstab: Nutzer 2026-11-06 ("Pokemon", "passt von den Farben und von der Struktur ueberhaupt nicht zum Rest des Spiels"), E8/E10 ("Haut mehr als
eine Farbe", "Wunden fleischig", "nicht aus einem Guss, hoeher aufgeloest"), E15 (kein Cel-Shading), E16 ("streifige Fasern am Ruecken beheben"),
E22 (Haut im Spiel wie durchgehende Haut). Verglichen mit EM30_HANGAR, RE2-G5, RE2-G4 auf denselben Tafeln. Nichts am Code geaendert, nichts
committet. Seiten (W) = Wesen, (B) = Betrachter. Lab = CIE-Lab D65 (OpenCV float). Ausschnitte (2x-8x nearest) und Messskripte:
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe2/v2sicht/`
(`crops.py`, `crops2.py`, `mess.py` .. `mess4.py`; `08_flanke_V2_wunde_x4.png`, `04_V2_wunde_a_x8.png`, `06_seite_V2_x3b.png`,
`sp_5090_kopf_V2_x4.png`, `sp_ruhe5080_flanke_x6.png`, `02_Z1_V2_x2.png`, `01_Z2_*_x2.png` u. a.).

## 0. Ergebnis

**Nicht bestanden** - ein Punkt in der Klasse "Abweichung, die der Nutzer wieder ablehnen wuerde" (P2), alles andere "wie Spielfamilie" oder
"Abweichung klein". Der Abstand zu Runde 3 ist gross und in die richtige Richtung: Das Fleisch hat jetzt Wundstufen (dunkler Rand, Rot, rosa Kern) und
liest sich im Spiel 5080 als offene Wunde; das Rot auf dem Bahnsteig liegt am richtigen Ort und in der richtigen Menge. Was neu dazugekommen ist
und den Nutzer nach meiner Einschaetzung wieder stoert, ist die **Laengsstreifung der Haut und des Kopfs** (die "Koernung laengs" des Bauers): im
Nahbild und im Feuerlicht 5090 sieht der Kopf aus wie ein behaarter Baeren-/Hundekopf (feine parallele braune Striche ueber den ganzen Schaedel,
dazu ockerfarbene Striche auf der Kopfoberseite), der Ruecken wie gebuerstetes Holz. Das widerspricht E16 ("streifige Fasern am Ruecken beheben"),
E8 ("gemalte grosse Formen statt Foto-Rauschen") und der Familie (G4-Kopf = glatter Knochenschaedel mit wenigen Platten, G5 glatt beige, EM30
glatte Haut mit Adern). Der Bauer nennt es selbst (FARBE.md V2.7 "Kopf laengs gestreift, im Atlas haarartig", V2-O4).

## 1. Befunde

Klassen: **F** = wie Spielfamilie, **K** = Abweichung klein, **A** = Abweichung, die der Nutzer wieder ablehnen wuerde.

| # | Punkt | Soll (Spielfamilie / Original / Nutzer) | Ist K5c V2 (gemessen / gesehen) | Klasse |
|---|---|---|---|---|
| P1 | Binnenzeichnung des Fleischs (Z1: Wundstufen, Adern, Wundrand) | G4/EM30: Wunde mit dunklem Rand, 2-4 Rotstufen, heller Kern, wenige dunkle Adern mit Richtung (08 links). Nutzer E10 "Wunden fleischig" | Atlas (`04_V2_wunde_a_x8.png`): rosa Kern mit 3-5 diagonalen dunklen Strichen, dunkelroter Ring, schwarzroter Rand - die Stufen sind da. Render 08 Flanke: Rotflaeche L* sd 8,0 / a* sd 5,4 (G4 9,5/10,1, EM30 12,5/3,0, r3 5,9/3,4); Ruecken 11,3/6,5 (G4 9,7/7,4). Spiel 5080 Ruhe (Figurfenster 12 679 px): Rot 12,8 % (r3 7,1), Rot-L* sd 11,2, a* sd 5,9 - in der Familie (Messblatt V2 (e) 9,6-11,2 / 5,0-8,7). Im 6x-Ausschnitt `sp_ruhe5080_flanke_x6.png` liest sich die Flanke als rohe Wunde mit rosa Kern. ABER: die Adern kommen im Render kaum als Linien an - Anteil der dunklen Top-Hat-Pixel in elongierten Komponenten: V2 12 % (Flanke) / 22 % (Schulter) / 20 % (Ruecken) gegen G4 22/28/31 %, EM30 55/58/51 %; der Rest sind Flecken. Die Flanke im Nahbild (`08_flanke_V2_wunde_x4.png`) ist ein Mosaik aus dunkelroten, orange-ockerfarbenen, rosa und blaugrauen Flecken, keine gezeichnete Form. Der Bauer sagt es selbst ("Wunden im Nahbild pixelig-fleckig, G4-Adern sind klarer") | **K** (Stufen und Fleischigkeit erreicht, Linien/Ordnung nicht) |
| P2 | Haut und Kopf: Laengsstreifung ("Koernung laengs 13,5 Texel", Kopflinien 1,22 je 1000) | E16: "streifige Fasern am Ruecken beheben"; E8: gemalte grosse Formen, keine aufgestempelten Marken; E22: durchgehende Haut; Familie: G4-Kopf Knochenschaedel mit wenigen Platten, G4-Haut schwarzbraun glatt mit Adern, G5 glatt, EM30 Haut glatt mit Adern; Bild 2 Original: dunkle glatte Masse | 06 Seite/3/4 vorn (`06_seite_V2_x3b.png`, `06_34vorn_V2_x3b.png`): der ganze Schaedel ist mit feinen parallelen braunen Strichen bedeckt (dunkle Linien 13-15 % der Kopfpixel, elongiert 6,4-7,4 %, gegen K5c heute 9 % / 3 %; G4 9-23 % / 4-12 %, aber dort als Rippen/Platten, nicht als Haar), Kopfoberseite ockerfarben gestreift, Augenwuelste als dunkle Knoepfe darin. Liest sich als Fell/Holzmaserung, nicht als Haut oder Knochen. Spiel 5090 Pose 2 (`sp_5090_kopf_V2_x4.png`, Feuerlicht): Kopf und Nacken zeigen die Streifen deutlich als gebuerstete Maserung, Rueckenhaut ebenso; Tafel 02 Z.1 (`02_Z1_V2_x2.png`) dasselbe. Im Spiel 5080 (Entfernung) unauffaellig. Atlas 04: Koerperhaut "wie Fell" (Bauer V2.6 exp_lic), Kopf haarartig (V2-O4). Zahlen: starke Richtung der Hauttexel 23,5 % (EM30 21,2 %, G4 35,6 %), davon senkrecht 66 % (EM30 32 %, G4 46 %) - die Menge ist in der Familie, die Erscheinung nicht: EM30/G4 sind an Muskel- und Rippenformen gestreift, V2 ueberall gleichmaessig wie Haare. Ein Nutzer, der "Pokemon" und "passt nicht zum Rest des Spiels" gesagt hat, wird hier "Fell" oder "Holz" sagen; die Streifung ist genau das, was E16 verbot | **A** |
| P3 | Farbfamilie / Machart gegen EM30, G5, G4 | G4 03: L* 17-22, C* 9,6-12,6; EM30 24,5-29 / 9,9-12,5 | 03: V2 L* 15,9-18,4, C* 9,3-12,6; dunkles Rotbraun, Fleischrot an Flanke/Ruecken, Blaugrau an den Gliedern, Krallen beige-rot. Kein Weiss, kein Magenta (K4 heute: C* 4,0-5,0 mit Weissflecken). Neben G4 dieselbe Palette; Ocker-Anteil (b* > 12, a* < 14) 0,6 % der Figur (G4 4,7 %, G5 28,8 %) - in der Ganzfigur kein Problem | F |
| P4 | Glanz / Neutralweiss | Familie L* > 85: 0,00-0,21 % | 03/05: 0,00-0,04 %, 06 3/4 vorn 0,28 % (Kieferband); keine silbrigen Flecken | F |
| P5 | Mehrtoenigkeit der Haut (E10) | Familie sigma_ab je Teil 7,4-10,7 | A2 9,2; im Render Rotbraun (Ruecken/Kopf), Fleischrot (Wunden), Blaugrau (Glieder), Beige (Krallen); B7 Farbtonstreuung 10,4 Grad (r3 4,5) | F |
| P6 | Texeldichte / PSX-Stil (E8 "hoeher aufgeloest") | EM30 grob, HF 0,04-0,26 | A6/A7 gruen, A10 0,64-2,33 unveraendert, B6 0,29 % (Grenze 0,28, Laufstreuung). Neu: in den Wundraendern des Atlas sitzen isolierte dunkle Einzeltexel (0,51 % der Fleischtexel, EM30 0,17 %, G4 0,00 %; `04_V2_wunde_a_x8.png` oben links: Punktreihen) - im Render als Sprenkel sichtbar ("pixelig"), aber kein Gitter (Autokorrelation faellt monoton, kein Raster) | K |
| P7 | Kiefer / Maul gegen RE2-G4 | G4: dunkles Maulinneres, rosa Zahnfleisch, beige Zaehne | 06 unten vorn (`06_untenvorn_V2_x3b.png`): Maulinneres dunkel, Zahnreihen beige, Zahnfleisch rosa-orange; A8 6,8/29,0/40,5 % gruen, B8 15,9 % (Original 22,6). Schwaeche: das Kinn/Unterkieferband ist rosa-weiss mit schwarzen Sprenkeln ("Streusel", 06 3/4 vorn, L* > 85 0,28 %), B6 Kiefer 1,83 % (gestreckte G4-Kopie, Doppelbloecke, V2-O1). Im Spiel 5090 als helles Band lesbar, Sprenkel dort nicht stoerend | K |
| P8 | Bild 1 (Bahnsteig, Z2): Rot am richtigen Ort und in der Menge | Original Z.2 (Figurfenster 81 236 px): Rot (a* > 15) 21,5 %, Rosa (a* > 15, L* > 35) 7,8 %, C* 19,6, b* -14,6; rosa Fleck oben auf dem Ruecken | V2 Z.2: Rot 15,9 %, Rosa 3,5 %, C* 17,1, b* -11,8 (K4 heute 21,3 / 14,6 / 19,9 / -14,2, aber als Glanzflecken). Riegel K1v2/K2v2/K4/K4b/K3 gruen (Kette C* 6,7 bei Rundlauf 7,8; Rot Ruecken/Flanke 11,5 % gegen 11,9 %). Gesehen (`01_Z2_V2_x2.png`): dunkelrote Wunde oben rechts auf dem Ruecken, wo das Original einen helleren magenta-rosa Fleck hat; Glieder dunkelblau, Pfoten hell - Ort und Menge stimmen, der Wundkern ist in der Kette dunkler als im Original (Rosa 3,5 gegen 7,8 %) | K |
| P9 | Bild 2 (5090 Cut 11) | Original: dunkle Masse, Unterkieferband hell, Klauen rot | 02 Z.1/Z.2 und Spiel: dunkle rotbraune Masse, Klauen rot, Kieferband hell; K6-K10 gruen. Die Streifung (P2) ist in Z.1 und im Spiel sichtbar; das Original zeigt eine glatte Masse | F (Farbe) / A-Anteil siehe P2 |
| P10 | Augen | E24 dunkel, Original Lichtpunkt schwach | K10 19,4 / 15,7; dunkle Wuelste, kein Punkt | F |
| P11 | Schwanz aus einem Stueck (E9) | keine Stufe | A11 0,5; 05 Seite/hinten keine Gliedgrenze; Schwanzspitze hellgrau mit rotem Fleck statt rosa Kegel (r3 P8 behoben) | F |
| P12 | Pfoten / Krallen | G4 Krallen knochenbeige, Fuesse Haut wie Koerper | 05 vorn (`05_ruhe_vorn_x3b.png`): Vorderpfoten (W) hellgrau-beige mit roten Krallen, Hinterpfoten blaugrau - weiterhin "helle Socken", unveraendert gegen r3 | K |
| P13 | Flaechengrenzen an Polygonkanten (r3 P1, Nr. 2) | Fleischgrenze loest sich von der Mesh-Kante | 08 Flanke: der schwarze Keil und die untere Kante der Wunde folgen weiter Polygonkanten; Spiel 5080 Pose 1: Schulterwunde mit gerader Oberkante an der Mesh-Naht | K |
| P14 | Rot im Umrissband (r3 B9) | EM30 <= 11,3 % | B9 8,1 %, B9b 5,6 %; Umriss im Spiel 5080 schwarz-grau, rote Polygonkanten an der Hinterbein-Innenseite weg | F |
| P15 | Konstanten mit Quelle (Z3) | jede Konstante mit Messquelle | FARBE.md V2.3: jede Zeile mit Quelle; K_GRAPH/SPIEGEL_ABSTAND ohne Unabhaengigkeitsnachweis (V2.4, 10 Riegel kippen) - nach Messblatt V1 7.5 geht die Tafel damit noch nicht als Abnahmevorlage an den Nutzer (Bauer sagt es in V2.0 selbst) | K (Zahlen-Pruefer) |

## 2. Belege zu P2 (Streifung)

- `06_seite_V2_x3b.png` (3x aus Tafel 06 Zeile 3): Schaedel vollstaendig von 1-2 px breiten parallelen Strichen in Braun/Ocker bedeckt, Richtung
  Schnauze-Nacken; Augenwuelste als dunkle Knoepfe; Kinn rosa-weiss gesprenkelt. Daneben `06_seite_G4_x3.png`: beige Knochen, wenige Rippenlinien.
- `sp_5090_kopf_V2_x4.png` (Spiel ROOM5090 Schnitt 11, Clip 1, echtes Fenster): Kopf und Nacken mit durchgehender Maserung, Rueckenhaut gestreift;
  `sp_5090_kopf_heute_x4.png` (K4 heute) glatt mit Glanzflecken; Original Bild 2 glatt dunkel.
- Zahlen 06 (dunkle duenne Linien, Black-Top-Hat 5x5 > 6 L*): V2 3/4 vorn 13,4 % / elongiert 6,5 % / 67 Linien, Seite 14,7 / 6,4 / 64, unten vorn
  14,0 / 7,4 / 54; K5c heute 9,1 / 3,0 / 29, 9,0 / 2,9 / 26, 9,8 / 4,1 / 30; G4 9,7 / 5,0 / 30, 8,6 / 4,2 / 18, 23,1 / 12,4 / 39.
  V2 hat in der Seitenansicht etwa doppelt so viele Linien wie G4, ueber die ganze Flaeche verteilt; G4 hat sie an den Rippen gebuendelt.
- Spiel 5090 Pose 2 Figurfenster: dunkle Linien V2 9,2 % / elongiert 2,6 % / 55 Linien, r3 7,9 / 1,6 / 40, K4 heute 6,5 / 1,7 / 38.
- Herkunft laut FARBE.md V2.2: "Koernung laengs: r3-Koernung entlang der Achse gemittelt (13,5 Texel)", V2.6 exp_lic "Haut wie Fell im Atlas",
  Kopf "Linien laengs parallel (Breite 2,1, Kontrast 16,2, 1,22 je 1000)". Die Streifung ist also absichtlich gebaut, um B17 (Kohaerenz) und
  B12 (Kopf-Canny) zu heben - ein Riegel-Ziel, das in der Familie durch Muskel-/Rippenformen entsteht, hier durch Haarstriche.

## 3. Belege zu P1 und P8 (Fleisch, Bahnsteig)

- Rotflaeche 08 (a* > 15, Figurpixel ohne Beschriftung): siehe Tabelle P1; L*-Streuung V2 Flanke 8,0 liegt zwischen r3 (5,9) und G4 (9,5), Ruecken
  11,3 ueber G4 (9,7). Die "flache dunkelrote Flaeche" von r3 ist weg.
- Linienanteil (elongierte Komponenten >= 12 px, Elongation >= 3, Anteil an den Top-Hat-Pixeln): Flanke G4 22 %, EM30 55 %, r3 27 %, V2 12 %;
  Schulter 28 / 58 / 39 / 22; Ruecken 31 / 51 / 23 / 20. V2 liegt unter r3 - die Zeichnung besteht aus Flecken, nicht aus Linien.
- Spiel 5080 Ruhe (Figurfenster aus |V2-heute| oder |r3-heute| > 18, 3x3 geoeffnet): heute Rot 0,0 %, r3 7,1 % (L* sd 12,5, a* sd 3,4), V2 12,8 %
  (L* sd 11,2, a* sd 5,9), L* > 45 0,54 % - dunkel wie Bild 1 verlangt, Wunde lesbar.
- Bild 1 Z.2 (Tafel 01, Figurfenster |K4-V2| > 45, 3x geoeffnet, 81 236 px): Original L* 22,1 / a* 8,6 / b* -14,6 / Rot 21,5 % / Rosa 7,8 %;
  V2 35,6 / 7,3 / -11,8 / 15,9 % / 3,5 %; K4 37,7 / 9,5 / -14,2 / 21,3 % / 14,6 %. Die Helligkeitsdifferenz (Z.1 heller als das Spielfenster)
  ist in der Kopfzeile genannt; die Rot-Menge V2 ist 0,74x Original, das Rosa 0,45x - "Abweichung klein", kein Fehlort.

## 4. Fairness der Tafeln

| Pruefung | Ergebnis |
|---|---|
| Gamma / Aufhellung | alle Kacheln "roh", gleiche Beschriftung wie r3; `kf_tafeln.py` unveraendert in der Lichtregel (0,55 + 0,45 N.L, Umkugel 0,46) |
| 01/02 | Disc-Licht x k fuer K4 und V2 gleich (1,06/0,95/0,82 bzw. 1,20/1,12/1,00), Z.2 beide durch denselben Videofilter; Kopfzeile nennt Z.1 heller als 07 |
| 03/05/06/08 | gleicher Renderer, gleiches Licht; Figurgroesse K5c 15-17 k px gegen G4 35-39 k px (Kopfzeile). 08: "gleiche Ausschnittregel (Figurrahmen)" - der Ausschnitt "Schulter" trifft bei K5c den Kopf, bei G4/EM30 die Schulter (Bauer nennt es in V2.7); die Zeile ist damit kein Schulter-Vergleich, aber nicht zugunsten von V2 verzerrt |
| 04 | gleiche Texelskala (2 px je Texel); Kanten-/Richtungsbild fuer G4 und V2 mit denselben Schwellen |
| 07 | echtes Fenster, gdigrab, kein AUTOSHOT/SOFTWARE_RENDER, gleiche Szene/Pose/Kamera, `spiel_lauf.log` 10/10 rc=0; 28 Clips (heute) gegen 3 Clips (Kandidat) ist nur Asset-Umfang, Pose gleich |
| Vergleichsmodell gleich behandelt | ja - kein Modell aufgehellt |
| Einschraenkung Bauer | V2.0: Unabhaengigkeitsnachweis K_GRAPH/SPIEGEL_ABSTAND fehlt, "nach Messblatt V1 7.5 gehen die Tafeln deshalb nicht als Abnahmevorlage an den Nutzer" - zutreffend |

Urteil: fair.

## 5. Was fuer "bestanden" fehlt (Sicht)

1. **P2:** Die Laengsstreifung von Haut und Kopf zuruecknehmen, ohne die Wundstufen (P1), B14-B16 und die Rot-Riegel zu verlieren. Kohaerenz (B17)
   und Kopf-Canny (B12) muessen aus gemalten Formen kommen (Kopf: wenige Platten/Stirn/Schnauze/Augenhoehle wie G4 Mesh 26, Haut: Muskelstraenge
   entlang der Glieder mit Abstand, nicht Haarstriche ueber die ganze Flaeche). Massstab: 06 Seite neben G4 darf nicht wie Fell lesen; im Spiel 5090
   Pose 2 soll der Kopf wie bei Bild 2 eine dunkle glatte Masse mit Kieferband sein. E16/E22 gelten.
2. P1: Adern im Render als Linien (Linienanteil Richtung G4 >= 22 % statt 12 %); Fleckenmosaik der Flanke (Ocker/Olive neben Rosa) auf 2-3 Toene je
   Flaeche zusammenziehen (E8 "wenige Farben je Teil").
3. P6/P7: Einzeltexel-Sprenkel in den Wundraendern und am Kinn entfernen; B6 Kiefer (gemaltes Maulinneres statt gestreckter Kopie, V2-O1).
4. P12/P13 wie r3 (Pfotenkappen, Flaechengrenzen an Polygonkanten).
