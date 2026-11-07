# Nachbesserung 1, Asset-Spur (K3 ins Spiel-Asset, Loecher, Clips, Messwerte fuer den Tick)

Teil der Nachbesserung 1 der Stufe 3 (Auftrag Orchestrator, Abnahme 1 Maengel M1/M2/M5/M8e, Abschnitt 6; E20/E21: Kopf K3).
Diese Datei fuehrt die Asset-Seite; der Hauptstrang (C-Seite: Tick, Rueckstoss, Kamera, Ton, Kollision) fuehrt sie ins Dossier
STUFE3.md ueber. Kein RE-Wert: alle Zahlen im Generator sind Gestaltung (PORT-WAHL), was aus G4 kommt, bleibt markiert (Bildzahl,
EDD-Treffer-/Tonbits und Spur der G4-Clips 0, 3, 4, 5, 8, 9, 10, 26, 27 unveraendert, Test vergleicht weiter gegen CDEMD0.EMS).

## 0. Stand

| Schritt | Stand |
|---|---|
| A K3 ins Spiel-Asset | erledigt (Abschnitt 1); Basis seit dem Texturbefund stufe3/basis_n1 (Abschnitt 1a) |
| Texturbefund "Texturen kaputt" | Ursache gemessen, behoben (Abschnitt 1a): Texel ausserhalb TIM 35 -> 0, Mosaik der Gelenkkoerper -> zusammenhaengende Haut; Belege echtes Fenster textur_vorher_nachher.png |
| Tests (eigener Baum build_n1a) | Asset-Pruefungen (test_asset/test_masse) gruen; unit_r41_form_emd, unit_birkin_szenen gruen. unit_birkin_bahnsteig zuletzt rot nur durch die laufenden C-Aenderungen des Hauptstrangs (Rueckstoss erwartet Clip 8 statt 13, Hieb-/Rennen-Ablaeufe nach dem Biss-Entscheid 5000) - vor diesen Aenderungen mit dem Endstand-Asset gruen |
| C Clips 12/13, Spruenge (M1/M5/M8e), Hiebe | erledigt (Abschnitt 2) |
| B Loecher | Abschnitt 3: Achsel/Huefte -89 %, Rest belegt |
| D Messwerte Kopf/Biss/Laenge | erledigt (Abschnitt 4), an den Hauptstrang gemeldet |
| E Kontaktblaetter | erledigt (Abschnitt 5) |
| Kopfform "Schildkroete" | zurueckgestellt (Orchestrator), Blatt kopf_frontal_spiel.png fuer die Klaerung |

## 1. A: K3 im Spiel-Asset

Generator `stufe3/gen` (Aufruf wie bisher: `python s3_clips.py`, dann Asset nach `re15_port/shared_assets/RE2` kopieren,
`python s3_testkopf.py`):
- `s3_rig.py`: Basis ist `varianten/K_alligator/K3_fit/FORM.EMD/.TIM` (`BASIS_DIR`); Gerippe (Eltern, Versaetze) wird beim Laden gegen J
  geprueft (gleich). Bild 1 = K3 Clip 0, Bild 2 = K3 Clip 1 (Kopfwinkel Hals/Kopf/Kiefer aus K3 kopf.json). Ruhe = Ruhe von J mit
  Kiefer zu (`RUHE_KIEFER` = Knochen 23 auf 0; K3-Ruhe mit Kopf 13 Grad tiefer und Kiefer 5 Grad offen war nur fuer das Rundumblatt).
  Gemessen: in der Ruhe liegt die Schnauze 1617 ueber dem Boden (steckt nicht im Boden), Kiefer bei az 0 am Gaumen.
- `s3_clips.py`: Textur aus der Basis (TIM 512 x 256, CLUT-Zeile 3 fuer Kopf/Kiefer/Krallen). Kiefer zu in Stehen (Clip 2), Gehen (5) und
  allen Platzhaltern. `Bau.kopf`: Kieferwerte der Clips (fuer J gesetzt) mal `KIEFER_K3` = 0,65 (K3-Kiefer 1089 lang, bei gleichem
  Winkel 1,5-fache Spaltweite an der Spitze, K.md 8.2), und nie ueber den Gaumen hinaus zu (az >= 0). Groesste Maulwinkel damit rund
  31 Grad (Bild 2: 24,5 Grad). Sturz (Clip 12, Basis Pose Bild 2) hoechstens 8 Grad ueber Bild 2.
- Kopf am Boden (K.md 8.5): in keinem Clip liegt Kopf/Kiefer unter der Fussebene (k_s3_kopf.py, Rennen/Flucht hoechstens auf Hoehe 0).
- `s3_testkopf.py` / `re15_port/tests/unit/form_j_posen.h`: Referenz = Basis des Rigs (seit dem Texturfix `stufe3/basis_n1`: MD1 161228 Byte
  CRC 0xe8df6d49, TIM 133152 Byte CRC 0x792bced3) und die drei Posen des Rigs; Makronamen FORM_J_* bleiben. Hoehe Gehen 3126.
- Asset: 28 Clips, 799 Bilder (vorher 760; Clip 13 neu mit 40 Bildern), EMD 285400 Byte (Endstand md5 74e9f465...). `form_emd.py
  rundlauf`: gleich, FE.pruefe 0 Fehler, 0 Warnungen. MD1 und TIM bytegleich mit der Basis.

**Formpruefung am Spiel-Asset** (`form_pruefung.py pruefe stufe3/formpruefung.json`, Bericht `stufe3/formpruefung/bericht.txt`):

| | Bild 1 IoU | Bild 1 Punkte | Bild 1 Boden/Verd. | Bild 2 IoU | Bild 2 Punkte | Bild 2 Boden/Verd. | Zeichnung 1 / 2 |
|---|---|---|---|---|---|---|---|
| J im Spiel (vorher) | 0,948 | 31/31 | 5/7 | 0,935 | 7/7 | 9/9 | 5/9 / 4/7 |
| K3 offline (Soll) | 0,954 | 31/31 | 5/7 | 0,948 | 7/7 | 9/9 | 5/9 / 3/7 |
| Spiel-Asset nachher (vor dem Texturfix) | **0,954** | **31/31** | 5/7 | **0,948** | **7/7** | 9/9 | 5/9 / 3/7 |
| Spiel-Asset Endstand (Texturfix, Abschnitt 1a) | **0,954** | **31/31** | 5/7 | **0,948** | **7/7** | 9/9 | 4/9 / 2/7 |

Zeichnung Bild 2 3/7 wie K3 offline (Q3/Q4/P sind Merkmale am Kopfrand von J, K.md Abschnitt 6).

**Tests** (eigener Baum `re15_port/build_n1a`): unit_birkin_bahnsteig, unit_r41_form_emd, unit_birkin_szenen gruen. Angepasst in
`tests/unit/test_birkin_bahnsteig.c`: test_asset prueft zusaetzlich Clip 13 (40 Bilder, Spur -80 je Bild, z 0) und die Sturz-Spur bis
Bild 30 (-3500..-3000); test_masse Drohen-Ausnahme Bild 31-60 bis 70 unter y = 0 (vorher Bild 33-38 bis 250, gemessen jetzt 67 in Bild
55, vorher 242). Testgeruest: `raum_laden` leert `g_room_rdt`/`g_room_rdt_ok` je Raumladung. Ohne das stuerzte der Test mit dem
K3-Asset in test_m5_takt ab (gdb: collision_constrain_impl <- re15_collision_constrain_contact_band <- re15_npc_wall_clamp <-
re15_npc_ai_tick): test_sub10_tor/test_r2_koerper setzen `g_room_rdt = s_rdt` und am Ende nur `_ok = 0`, die NPC-Wandklemme las danach
Zeiger in den schon freigegebenen Raumpuffer. Mit dem alten Asset im selben Bau kein Absturz (Heap lag anders) - ein alter Fehler im
Geruest, kein Fehler des Assets.

## 1a. Texturbefund (Nutzer: "Texturen vollkommen kaputt")

Messung (`stufe3/gen/s3_n1_textur.py pruefen`, Bytes des Assets; Ursache auch in STUFE3.md N0):
- Port und Offline-Buehne lesen Texel gleich (Texel-x = u + (page & 0xF) * 128, CLUT-Zeile ueber das CLUT-Wort); die Buehne zeigt in
  derselben Pose und Kamera (5080 Schnitt 4, Clip 10 Bild 100, Gier 2048) dieselben grauen Bloecke am Hals wie das echte Fenster.
  Also kein Seiten-/CLUT-Fehler im Port.
- Ursache 1: die 31 Gelenkkoerper (k_naht.kugel) kopieren fuer jede ihrer 40 Flaechen die UV der naechsten Elternflaeche unveraendert
  und beliebig gedreht: 14-64 verschiedene Stuecke je Koerper-Mesh, Helligkeitsspanne p95-p5 bis 186 (weisse Flecken, Kanten).
- Ursache 2: 35 Flaechen von Kopf (27) und Kiefer (8) reichen auf Seite 3 bis u = 128 = Texel-x 512 (TIM 512 breit), eine Spalte
  hinter der Textur.

Behebung (`s3_n1_textur.py bauen ../basis_n1`, Fassung 'gemischt'; Basis `stufe3/basis_n1` = K3_fit mit neuer UV, `s3_rig.BASIS_DIR`):
- Je Gelenkkoerper ein zusammenhaengendes Hautstueck ('flaeche'): Breiten-/Laengengrad des Ellipsoids -> u/v auf ein Rechteck 12-32 x
  12-32 Texel (rund 60 Einheiten je Texel), Nachbarflaechen teilen ihre UV-Kanten (kein Mosaik, Sprung nur an der Laengsnaht). Seite und
  CLUT-Zeile der 12 naechsten Originalflaechen desselben Meshes; Lage = das Rechteck unter deren UV-Mitten (+-16 Texel), dessen Mittelfarbe
  und Koernung der Umgebung am naechsten liegen und das keinen einzigen weissen/bunten Texel enthaelt (Y <= Median + 60, Buntheit <=
  Umgebung + 40), notfalls kleiner (bis 8 Texel).
- Ausnahme Ellbogen r ('projektion'): er liegt in Bild 1 genau auf dem weissen Dreieck Z5 (E1/E3/E14; Kennungsbild der Kamera von Bild 1:
  169 von 169 Pixeln um die Sollmitte (403,289) gehoeren ihm). Als Hautstueck verschwand Z5 (Pruefpunkte b1.kappe_spitze/_basis fielen,
  31 -> 29). Dort setzt der Koerper die Haut seiner Unterlage fort (je Flaeche die ebene Abbildung Raum -> UV der naechsten Originalflaeche,
  Ecken projiziert, Ueberstand je Gewicht bis -0,5).
- Kopf/Kiefer: u auf den letzten Texel der Seite geklemmt (30 UV-Ecken).
- Verworfen, gemessen: '2 x 2 Texel' (einfarbig, Koernung 0,21 der Umgebung: dunkle Plastikbeulen an Handgelenk und Hals), 'Hautstueck'
  ueberall (Z5 weg, 29/31), 'projektion' ueberall (59 % der Koerperecken mit UV-Sprung > 2 Texel: wieder fleckig), 'projektion' auch fuer
  Hals/Bein r (brachte Q3/Z4 nicht zurueck), Projektion aus Eltern UND Gelenkpartner (Z5 weg, 30/31).

| Fassung | Texel ausserhalb TIM / ohne CLUT | Koerperecken mit UV-Sprung > 2 Texel (Mittel) | Koerper mit Fleck im Stueck | Farbabstand Koerper-Umgebung (Mittel / max) | Koernung Koerper / Umgebung (Mittel, min-max) | Bild 1 Punkte / Zeichnung | Bild 2 Punkte / Zeichnung |
|---|---|---|---|---|---|---|---|
| K3 vorher (Mosaik) | 35 | 89 % (13,3) | Mosaik aus Fleckstuecken | 3,7 / 19,3 | 1,10 (0,68-1,68) | 31/31, 5/9 | 7/7, 3/7 |
| 2 x 2 Texel | 0 | - | 0 | 5,6 / 19,4 | 0,21 (0,00-0,56) | - | - |
| Hautstueck ueberall | 0 | 18 % (1,6) | 0 | 5,7 / 31,7 | 0,82 (0,27-1,18) | 29/31, 3/9 | 7/7, 2/7 |
| Projektion ueberall | 0 | 59 % (7,3) | - | 3,5 / 20,8 | 1,15 (0,34-1,89) | 31/31, 3/9 | 7/7, 2/7 |
| **gemischt (gewaehlt)** | **0** | **19 % (1,7)** | **0** | 5,6 / 31,7 | 0,83 (0,27-1,18) | **31/31, 4/9** | **7/7, 2/7** |

Farbabstand = Mittel |RGB Koerper - RGB der 12 naechsten Originalflaechen| (0-255), Koernung = Streuung der Helligkeit der Koerpertexel
durch die der Umgebung (1 = gleich koernig). Umriss-IoU unveraendert 0,954 / 0,948. Zeichnung: Z4 (helles Band am rechten Oberschenkel,
Muffe Bein r 47 px), Z9 und Q3 (Hals in Bild 2, Muffe Hals 146 px) bestanden bei K3 nur, weil zufaellig helle Mosaikstuecke dort lagen
(Q3 ist nach K.md ein Merkmal am Kopfrand von J); mit ruhiger Haut fallen Z4 und Q3, Z9 bleibt. Geometrie, Clips und Posen unveraendert
(Loch-Zahlen Abschnitt 3 gelten), MD1-CRC neu 0xe8df6d49 (form_j_posen.h), TIM unveraendert.

## 2. C: Clips (M1 Sturz, M5 Taumeln, M8e Spruenge)

Zahlen von `s3_pruef.py` (wahre Knochendrehung je Bild, Gleiten stehender Glieder in der Welt gegen die Spur, tiefster Punkt; Generator
in Gleitkomma). Vorher = Spiel-Asset der Abnahme 1 (J), nachher = K3-Asset dieser Nachbesserung.

| Clip | Bilder | Drehung/Bild max vorher -> nachher (Grad) | Gleiten vorher -> nachher | tiefster Punkt unter y=0 vorher -> nachher |
|---|---|---|---|---|
| 0 Rennen | 18 | 106,3 -> 46,3 | 8,1 -> 5,8 | 334 -> 334 (Bild 11 = Pose Bild 1) |
| 2 Stehen | 60 | 2,6 -> 2,6 | 7,5 -> 7,5 | 15 -> 15 |
| 3 Hieb rechts | 60 | 40,5 -> 37,8 | 9,8 -> 9,8 | 11 -> 12 |
| 4 Hieb links | 50 | 49,6 -> 31,6 | 4,3 -> 3,9 | 12 -> 12 |
| 5 Gehen | 89 | 12,2 -> 12,2 | 7,6 -> 7,6 | 16 -> 16 |
| 8 Zucken | 30 | 24,9 -> 24,9 | 7,3 -> 7,3 | 4 -> 4 |
| 9 Nachhieb | 63 | 51,3 -> 38,1 | 3,7 -> 3,7 | 14 -> 13 |
| 10 Drohen | 126 | 53,7 -> 43,0 | 23,0 -> 23,0 | 242 -> 67 |
| 11 Flucht | 57 | 106,3 -> 46,3 | 8,1 -> 15,0 | (Bild 26 = Pose Bild 1) |
| 12 Sturz | 56 | 25,7 -> 25,7 | **150,7 -> 4,1** | Fall (gewollt) |
| 13 Taumeln (neu) | 40 | - -> 18,5 | - -> 3,4 | - -> 22 |
| 26 Biss | 35 | 22,1 -> 20,9 | 6,0 -> 6,0 | 27 -> 27 |
| 27 Griff | 100 | 49,2 -> 33,4 | 3,6 -> 3,6 | 24 -> 24 |

Was geaendert ist (alles Gestaltung):
- **Clip 13 Taumeln (M5)**, `s3_angriffe.taumeln`: Seitenfolge-Gang rueckwaerts (Reihenfolge LH, RV, RH, LV), zwei Schrittfolgen je
  Durchlauf, Stand 62 %, alle vier Fuesse in der Welt verriegelt gegen die Spur. Spur exakt -80 je Bild (Bild f = -80 (f+1)), -3200 je
  Durchlauf, z 0; schleifenfaehig (Umlauf Bild 39 -> 0: groesste Drehung 7,4 Grad, stehende Fuesse versetzen 0-5 Einheiten). Darueber
  drei Trefferrucke je Durchlauf (Bild 0, 13, 27: Oberkoerper/Kopf/Schwanz reissen zurueck, 2 Bilder Anstieg, 6 Abklingen), Rumpf leicht
  zurueckgelehnt, Maul halb offen (Kiefer 22 + 14 je Ruck, mal 0,65). `gang()` hat dafuer den Haken `extra(p, f)` vor der Fuss-IK.
- **Clip 12 Sturz (M1)**: Bild 0 bleibt Pose Bild 2. Im Wagen (Bild 1-26) stehen die Hinterfuesse in der Welt und machen Schritte
  (`fuesse_auto`, Schwelle 300, 6 Bilder, Hub 240, Fussdrehung aus der Schluesselpose), ab Bild 27 (Kippen ueber die Kante) fuehrt die
  Pose. Vorher hielten die Fuesse die Lage von Pose Bild 2 im Aktorraum, waehrend die Spur 90-150 je Bild schob (Gleiten 150,7). Laenge
  56, Spur unveraendert: Bild 0-2 Tempo 0, 3-15 je -90, 16-29 je -150 (bis Bild 30 zusammen -3270), ab 30 je -200 (Fall). Mit Start bei
  Wurzel x -2500 liegt Bild 30 bei rund x -5770, die Tuer (Wagenende) bei -5683.
- **Spruenge (M8e)**:
  - Rennen Bild 15 -> 16 (Fuss L 106 Grad): die Fussdrehung im Schwung (Sohle zur Kamera in Pose Bild 1) folgt jetzt der Zeit (Abheben ->
    Bild 11 auf, Bild 11 -> Aufsetzen zu) statt der Hubhoehe, die den Fuss im letzten Bild vor dem Aufsetzen auf einmal flach drehte.
  - Rennen Bild 10 -> 11 (Unterarm L 57 Grad): die Vorderhaende standen im Stand Bild 8-12 auf der Spurbreite der Ruhe und sprangen in
    Bild 11 um 1393 (links) / 589 (rechts) seitlich auf Pose Bild 1; jetzt Spurbreite von Pose Bild 1. Vorderer Schwung beginnt sanfter
    (Stuetzpunkt Bild 15 statt 14).
  - Drohen Bild 30 -> 31 (Bein R 54 Grad) und 45/53 (mit K3-Gelenkkoerpern Bein L 69 Grad): Bodenkorrektur eines freien Glieds hebt
    jetzt zuerst das untere Glied (Unterschenkel/Unterarm bis 70 Grad), dann das obere (bis 50 Grad), `s3_clips.GLIED_REIHE`. Vorher
    zuerst der Oberschenkel: an einem fast senkrechten Bein hebt die Huefte den Fuss nur mit 1 - cos, die Suche drehte 40-69 Grad weg.
    Die Bilder 45-52 tragen ihre Pin-Pose jetzt ausdruecklich (ohne Bodenkorrektur). Rest unter dem Boden bis 67 (Bild 55), vorher 242.
  - Nachhieb (Krallen 51 Grad): Ausholen bis Bild 14, Schlag nach unten 10 statt 7 Bilder.
  - Hieb links (Krallen 45-50 Grad in Bild 15): Schlag Bild 12-17 statt 13-16. Trefferfenster unveraendert (G4-Bits), Reichweite
    (`s3_reichweite.py`, Spieler 2600 voraus): Hieb rechts trifft Bild 28-30, links 15-17, Einstiegsbilder des Doppelschlags (Clip 3 Bild
    20, Clip 4 Bild 7) treffen nicht - wie vorher.
  - Griff (Unterarm L 49 Grad in Bild 43): Uebergang Pose <-> Stand der Arme ueber 8 statt 5 Bilder (`GRIFF_RAMPE`).
  - Flucht (Clip 11, uebernimmt Rennen): Gebruell -> Galopp ueber 6 statt 4 Bilder, der Sprung beginnt aus der letzten Galopp-Pose
    (vorher 69 Grad Bild 15, 75 Grad Bild 37). Sonst unveraendert (Drehung macht der Tick).
  - Hiebe (Clip 3/4, fuer M2): die Huefte giert im Hieb nicht mehr mit (`HIEB_GIER` 0, vorher 1,0), die Drehung traegt der Ruecken; Hub
    beim Ausholen 120 statt 220 (`HIEB_HOCH`; ohne Huft-Gier reichten die Hinterbeine sonst nicht mehr zum Boden, Schweben bis 134).
    Damit ist der Spalt Bein R - Huefte in den Hieben zu (27 Pruefansichten Hieb Bild 22-38: 417 -> 0 px). Schweben Hieb links Bild 11:
    104 (Generator) / 111 (Port-Pose, Test-Grenze 120). Reichweite: Hieb rechts trifft Bild 28-30, Hieb links 16-17 (vorher 15-17),
    Einstiege 20/7 frei.
- Drohen Gleiten 23 und Rennen Bild 11 (334 unter dem Boden, Pose Bild 1) unveraendert.
- Rennen, Versuch gegen den Spalt Bein L - Huefte in Bild 9/10/12 (Oberschenkel in Pose Bild 1 muenden lassen, `RENNEN_BEIN_RAMPE`):
  Spalt zu, aber Fuss 67-88 unter dem Boden und Sprung 55-62 Grad. Abgeschaltet (Rampe 0), Rest in Abschnitt 3.

## 3. B: Loecher an Achsel und Huefte

Messung `k_loecher.py messen` (Werkzeug aus K, 13 Tick-Clips 0,2,3,4,5,8,9,10,11,12,13,26,27, je 8 Bilder, je 10 Ansichten, orthografisch
0,075 px je Einheit, Port-Rueckseitenregel; Nahtloch = Loch am Drehpunkt eines Gelenks). Dateien `stufe3/nachbesserung1/loecher/nachher.json`
(Endstand), `nachher_a.json` (vor der Hieb-Aenderung); vorher = `varianten/K_alligator/loecher/vorher_J_stufe3.json` (J, gleiche Clips).

| | Nahtloecher (Anzahl / px) | ohne Mundwinkel (px) | Bein l - Huefte | Bein r - Huefte | Arm l - Brust | Arm r - Brust | Achsel+Huefte |
|---|---|---|---|---|---|---|---|
| J vorher (Abnahme 1) | 282 / 16924 | 16534 | 62 / 2635 | 18 / 1413 | 11 / 261 | 10 / 156 | **4465 px** |
| K3-Asset ohne Hieb-Aenderung | 511 / 9194 | 3963 | 10 / 106 | 9 / 308 | 6 / 56 | 2 / 22 | 492 px |
| **Endstand** | 503 / 8768 | 3183 | 8 / 82 | 7 / 120 | 8 / 95 | 2 / 22 | **319 px (-93 %)** |

Mundwinkel (Kiefer - Kopf, 5585 px) ist der Blick durch das offene Maul (E20, gewollt); Clip 13 (neu) traegt davon 1403.

Rest an Achsel/Huefte (25 Ansichten, je 3-37 px bei 0,075 px je Einheit): Rennen/Flucht Bild 9-12 Bein L (5-11 px, von der Seite, siehe
unten), Hieb links Bild 7 (Bein R 37, Arm L 19/20), Nachhieb Bild 9 (Arm L 10-14, Ansichten oben/unten), Taumeln Bild 0 (Bein R 17),
Sturz (Arm L 3-11, oben/unten), Drohen (Arm R 8/14, nur von unten). Gesucht und verworfen:
- Zusatzkoerper an Eltern/Kind (s3_n1_naht.py: Lochstrahlen -> Punkte im Raum des Elternknochens, Kugeln, die in Bild 1/2 die Grenzen von
  k_naht halten): fuer Bein R und die Achseln fanden sich Kugeln, die die Lochpunkte decken, aber sie beulen in anderen Posen sichtbar aus
  (10 Posen x 7 Ansichten: 2000-7500 neue Umrisspixel schon bei r 100-180). Laengere Muffen (fa 2,0-3,2): Bein L 106 -> 55 px, Beulen 830-4040.
- Bein L in Rennen/Flucht Bild 9-12: jede Kugel/Scheibe, die den Spalt von der Seite schliesst, setzt in Bild 1 (Pose = Rennen Bild 11,
  Rueckansicht) 66-278 px auf Ziel-Schwarz: der Screenshot zeigt dort Hintergrund zwischen linkem Oberschenkel und Rumpf. Pose Bild 1 selbst
  ist aus diesen Ansichten zu, nur die Nachbarbilder 9, 10, 12 oeffnen 5-22 px.

Stichprobe echtes Fenster (Abnahme 1 M2: 5080 Schnitt 4, RE15_BB_POSE -23500,-18200; eigene exe-Kopie re15_pc_n1a, gdigrab; Inseln mit der
Buehne derselben Kamera, `stufe3/gen/s3_n1_lochecht.py`, Blatt `stufe3/nachbesserung1/loecher_echt_vorher_nachher.png`):

| Fall | vorher (J) | nachher (K3) | Raender der Insel (Knochen, Buehne mit Kennungsbild) |
|---|---|---|---|
| Clip 5 Bild 28 Gier 2048 ("Achsel") | 1059 px | 414 + 15 px | 7 Wirbel, 11-13 Arm L, 16 Oberarm R (+ 22 Maul) |
| Clip 5 Bild 28 Gier 0 ("Huefte") | 1074 px | 764 px | 0 Huefte, 8 Oberschenkel L, 9 Oberschenkel R, 26 Unterschenkel R |
| Clip 10 Bild 100 Gier 2048 ("Achsel") | 1649 px | 1213 + 29 px | 7, 11-13 Arm L, 14/15 Klaue, 16 Oberarm R, 21 Hals (+ 22/23 Maul) |

Lesung: die verbliebenen Inseln dieser Kamera (hoch, schraeg von hinten/seitlich) sind keine Nahtloecher, sondern der Blick UNTER dem Rumpf
hindurch zwischen den beiden Vorderbeinen (Arm L gegen Oberarm R) bzw. zwischen den beiden Hinterbeinen (Oberschenkel L gegen R), also
die natuerliche Luecke eines Vierbeiners (k_loecher ordnet sie keiner Naht zu, die orthografische Nachmessung in Blickrichtung findet dort
nur Inseln zwischen den Beinen). Der Anteil an der Achsel/Huefte selbst ist durch die Gelenkkoerper von K3 verschwunden (vorher grenzte die
Achsel-Insel an Oberarm 11/Unterarm 12 und Hals 21 der eigenen Seite). Ob der Nutzer genau diese Bein-Luecken meinte, kann ich nicht belegen;
schliessen liessen sie sich nur mit einem Bauch-/Brust-"Rock" zwischen den Beinen, der in Bild 1/2 und in jeder Gehpose sichtbar waere.

## 4. D: Kopf-Folgen (Messwerte fuer den Tick, `stufe3/gen/s3_kopfmass.py`, `stufe3/nachbesserung1/kopfmass.json`)

- Kieferspitze im Raum von Knochen 23 (K3-Mesh, vorderste Ecke): (1139, -75, 0); Mitte der vorderen Zahnreihe (911, -75, 0); Kiefer x -330..1139.
- Biss Clip 26, Trefferbilder 16-30 (Aktorraum, Gier 0): Spitze x 4781-4891, y -1438..-1675 (Brusthoehe des Spielers), z bis +-700; im Raum
  von Teil 10: x 2998-3055, y 371-606. Der Bisspunkt des Ticks Teil 10 + (1400, 0, 0) liegt 1640-1690 hinter der Spitze.
- Vorschlag an den Hauptstrang, Entscheid Orchestrator: Probe am Kiefer bb_nah(e, pl, 23, 911, -75, 1500) (G4-Radius @0x80104f88-a4),
  Biss-Entscheidung 0xce4 + 1700 = 5000 (PORT-WAHL); Umsetzung C-Seite.
- Hiebe: Krallenform unveraendert (Variante K: nur Ton). `s3_reichweite.py` (Spieler 2600 voraus): Hieb rechts trifft Bild 28-30, Hieb links
  16-17, Einstiege des Doppelschlags (Clip 3 Bild 20, Clip 4 Bild 7) ausserhalb der Probe.
- Ausdehnung je Clip im Aktorraum (vorn / hinten / seitlich / Schnauze vorn): Rennen 4882/-3572/1681/4291, Stehen 4360/-3591/2258/4360,
  Hieb r 5610/-3752/2989/4386, Hieb l 5094/-3753/3038/4378, Gehen 4550/-3612/2256/4340, Zucken 4585/-3603/2721/4336, Nachhieb
  4781/-3942/2254/4398, Drohen 4742/-4062/3009/4363, Flucht 5210/-3572/2312/4357, Sturz 3838/-4291/2815/2490, Taumeln 4598/-3729/2224/4272,
  Biss 4893/-3522/2272/4893, Griff 5091/-3519/3074/5017. Kopf/Kiefer liegen in keinem Bild unter der Fussebene.

## 5. E: Kontaktblaetter und Belege

- `stufe3/kontakt/clip_NN_*.png` und `00_uebersicht.png` neu (s3_kontakt.py, alle Tick-Clips inkl. Clip 13 Taumeln).
- `stufe3/nachbesserung1/`: `K3_offline_bild1_bild2.png` (Spiel-Asset in den Posen/Kameras von Bild 1/2 neben den Screenshots, aus der
  Formpruefung), `spruenge_vorher_nachher.png` (M8e), `sturz_clip12_vorher_nachher.png` (M1), `taumeln_clip13.png` (M5),
  `loecher_vorher_nachher.png` (Buehne, 8 schlimmste Posen von J neben dem Endstand), `loecher_rest_nachher.png` (8 schlimmste verbliebene
  Posen), `loecher_echt_vorher_nachher.png` (echtes Fenster, Abschnitt 3), `textur_vorher_nachher.png` (Abschnitt 1a),
  `kopf_frontal_spiel.png` (Klaerung Kopfform).
