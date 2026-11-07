# K5c Farbe Runde 1: Sicht-Pruefung (Pruefer "Auge des Nutzers", 2026-11-06)

Gegenstand: Tafeln 01-07 und die fuenf Spielbilder des Bauers (`varianten/K5/farbe/`, Dossier `FARBE.md`, Riegel 24/24 GRUEN).
Massstab (Auftrag): Nutzer 2026-11-06 "passt von den Farben und von der Struktur nicht ... nicht zum Rest des Spiels ... wie ein Pokemon";
frueher E8/E10/E12-E15 (mehrfarbige Haut, fleischige Wunden, PSX-Stil, geordnet, kein Cel-Shading). Verglichen wird mit den Spielmodellen
auf denselben Tafeln (EM30_HANGAR, RE2-G5, RE2-G4) und mit Bild 1/Bild 2. Nichts am Code geaendert, nichts committet.
Seiten (W) = aus Sicht des Wesens, (B) = Betrachter. Lab = CIE-Lab D65 (OpenCV), gemessen an den Tafel-Pixeln bzw. Spielbild-Pixeln selbst.
Messskript-Ausgaben und Ausschnitte: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe/`
(`03_*_x2.png`, `05_row*.png`, `06_kopf_r*.png`, `04_*_x2.png`, `sp_*.png`, `mess_5080_regionen.png`, `mess_5090_regionen.png`).

## 0. Ergebnis

**Nicht bestanden.** Zwei Punkte fallen in die Klasse "Abweichung, die der Nutzer wieder ablehnen wuerde":

1. **Das Rot sitzt nur in duennen, hellen Adern, die als gleichmaessiges Netz ueber den ganzen Koerper laufen** (P1). Keine Fleischflaeche,
   keine ruhige Partie; die Adern sind doppelt so hell wie die Haut. Das ist der "Pokemon"-Eindruck (gluehende Risse auf dunklem Grund),
   nicht die Machart von G4 (24 % Rot in grossen Fleischstufen plus wenige Adern) oder EM30 (dunkle Adern auf hellem Fleisch).
2. **Blaugraue Haut an Beinen, Fuessen und Unterarmen** im neutralen Licht der Tafeln 03/05 (P2). In der Spielfamilie hat keine Kreatur
   blaue Haut (G4/G5 0,0 %; bei EM30 ist das Blau die Jeans). Im Spielbild 5080 bleibt davon wenig (2,1 %), aber der Nutzer urteilt an den Tafeln.

Das Uebrige ist Spielfamilie oder kleine Abweichung. Die vom Orchestrator genannten Verdachtspunkte sind an K5c neu **behoben**:
kein silbriger Glanz (L* > 85 = 0,00 % in allen 14 K5c-Kacheln), kein Creme-Kiefer (Band lachsrot, Maulinneres dunkel, Zaehne getrennt),
keine Magenta-Augen, keine Gamma-Aufhellung. Die Tafeln sind fair (Abschnitt 3).

## 1. Befunde

Klassen: **F** = wie Spielfamilie, **K** = Abweichung klein, **A** = Abweichung, die der Nutzer wieder ablehnen wuerde.

| # | Punkt | Soll (Spielfamilie / Original / Nutzer) | Ist K5c neu (gemessen) | Klasse |
|---|---|---|---|---|
| P1 | Rot-Struktur: Adern gegen Fleischflaechen | G4 (03 Seite / 3/4 vorn / 3/4 hinten): Rotpixel (a*>15, C*>20) ueberleben eine 3x3-Erosion zu 34-53 %, groesste Rotflaeche 1297-4382 px; EM30 25-39 %, 876-1034 px. G4 malt 24 % Rot in vier Fleischstufen (Messblatt 1), Adern nur als Akzent; Nutzer E10 "Wunden fleischig", E12 "geordnet, mit Richtung" | Rotpixel ueberleben die Erosion nur zu **14-24 %**, groesste Rotflaeche **286-483 px**: alles Rot ist Linie, nichts Flaeche. Die Adern sind heller als die Haut (Rot L* 24-29 gegen Figur L* 13-15; EM30: Adern L* 23 DUNKLER als Haut 27). Im Atlas (04, Koerper, Texelskala): Rest nach Erosion 22 % (EM30 35 %, K5c heute 47 %), Netz als Zellmuster gleich dicht in allen 46 Feldern, a*-Streuung 5x5 3,0 gegen EM30 2,1. Im Spielbild 5080 (`spiel_ruhe_r5080_s1.png`) sichtbar als rotes Rissnetz auf dunkelviolettem Grund | **A** |
| P2 | Blaugraue Haut (Beine, Fuesse, Unterarme) | Familie 03: Blau-Klasse (b*<-6, C*>8) G4 0,0 %, G5 0,0 %, EM30 4,9-9,4 % (nur Jeans). Rueckrechnung Original (farbe.md 1): "Violett, Lavendel und Blau gehoeren dem Video" | 03/05: **10,6-14,7 %** der Figurpixel blau-klassig, Beine/Fuesse stahlblau-grau (Bauer: Farbfenster bis -125 Grad bewusst offen, FARBE.md 2). Spielbild 5080: Hinterbein Lab (24,8, -0,3, -3,0), Box Figur 2,1 % blau: das Spiellicht nimmt es weitgehend weg. Der Nutzer sieht aber die Tafeln | **A** |
| P3 | Helle Vorderpfote im Spiel | Original Bild 1: Beine so hell wie der Ruecken (T L* 23 gegen 24, Messblatt 2); helle Stellen an den Fuessen = Boden (E26); Z5 = blaeulich-weisses Dreieck am Ellbogen (E3/E14, vom Nutzer bestaetigt) | 5080 Ruhe: linke Vorderpfote (B) Lab (40, 3,6, 1,2), RGB (100,93,93), gegen Rumpf L* 15: die ganze Pfote ist hellgrau, nicht nur ein Fleck am Ellbogen. In der Nachstellung Bild 1 sitzt der helle Fleck richtig am Arm | K (pruefen, ob Z5 bis in die Pfote laeuft) |
| P4 | Neutralgraue Facetten am Hinterkopf/Halsansatz | G4-Kopf (06): hell-unbunte Pixel (L*>35, C*<12) 9,2 % mit Lab (41, 6, 9) = warmes Knochenbeige | 06 3/4 vorn: 5,1 % hell-unbunt mit Lab (50, 3, -1) = neutralgrau, als flache Polygone oben hinten am Kopf (auch 03 Seite, 05 oben). Im Spielbild 5080 Kopf nur 0,5 % kuehl-hell. Wahrscheinlich O4 (Gaumenstreifen im Kopffeld) | K |
| P5 | Rosa Fleck Ruecken (Z1) | EM30 Wunden (03 3/4 vorn): rosa/hellrot (a*>20, L*>45) Lab (50, 26, 13), Farbton 27, Umring 19 % dunkelrot; G4 (53, 27, 23), 41, 20 %; Nutzer E10 "fleischig mit dunkelroten Tiefen" | Lab (57, 33, 12), Farbton 20, Umring 13 % dunkelrot, kein Weiss im Umring. In 5090 Pose 2 richtig abwesend (0 rosa Pixel; Original dort dunkel). Etwas pastelliger und flacher als die Familie, aber dieselbe Farbfamilie | K |
| P6 | Kiefer | Familie dreiteilig (dunkel / Fleischrot / getrennte Zaehne), Bild-2-Band Lab (50, 31, 16), Farbton 27 | 06: Band lachsrot, Maulinneres dunkel, Zahnreihen getrennt (G4-Maultextur); A8 2,7/33,8/44,5 %. Im Spielbild 5090 Cut 11 ist das Kieferfenster zu 94 % L*<20, Median L* 5,6: dunkel wie im Original, kein Creme | F |
| P7 | Augen | Original E24: Augenstellen dunkel (L* max 22 / 13), aber als schwache Lichtpunkte ueber dem Umfeld (+12,8 / +29,3 Y, FORMBLATT_V2 1.2 #1) | K10 13,5 / 15,9; in keiner Kachel ein Auge erkennbar. Dunkel stimmt, der schwache Lichtpunkt fehlt | K |
| P8 | Glanz / Neutralweiss | Engine rein diffus (farbe.md 5); Familie L*>85 0,00-0,21 % | 0,00 % in allen K5c-Kacheln von 03/05/06; keine silbrigen Flecken mehr sichtbar | F |
| P9 | Helligkeit gesamt | G4 L* 17,0-21,6, EM30 24,5-29,0 (03) | 13,8-16,1 (03), 10,9-20,0 (05). Dunkler als alles in der Familie; die Bild-1-Kette verlangt es (K3 dE <= 1,3). Im Spiel 5080 Rumpf L* 15, 5090 L* 12: die Figur ist dort eine dunkle Masse mit rotem Netz | K |
| P10 | Flaechenruhe Helligkeit / "hoeher aufgeloest" | EM30 Atlas (04, Texelskala): lokale L*-Streuung 5x5 Median 3,3, ruhige Texel (sd<3) 49 %, Gradient 3,3/Texel | K5c neu Koerper: 4,0 / 40 % / 3,0 (K5c heute 3,8 / 36 %). Im Render (03) Median 4,3-5,0 gegen EM30 4,7-5,8, G4 3,3-4,3. In der Helligkeit ist K5c neu NICHT feiner oder unruhiger als EM30; Texeldichte A10 unveraendert. Der "hoeher aufgeloest"-Eindruck kommt aus der Farbe (P1), nicht aus der Helligkeit | F |
| P11 | Mehrtoenigkeit je Teil | Familie sigma_ab 7,4-10,7; Nutzer E10 | 11,9 (A2), im Render C* 8,9-9,6 wie G4 9,2-12,0. Erfuellt; die Toene wechseln aber fast nur ueber das Adernetz (P1), die Grundhaut dazwischen bleibt ein Ton | F |
| P12 | Grundhaut-Farbton | G4 Grund (L*<40, C*<15) Farbton -2..4 Grad, EM30 1..8; Rueckrechnung Original Grundhaut (44,18,27) = Lab (9, 14,6, -0,2), Farbton -1, Mitteltoene -14 | -22..-34 Grad (b* -1,3..-2,3) bei C* 3-4: leicht violett statt neutral/warm; im Spiel 5080 Rumpf Lab (15, 8, 2), Farbton 13, also warm. Klein, weil C* klein | K |
| P13 | Kachelnaehte / Wiederholung (O3) | Familie: je Feld eine gemalte Form ohne Wiederholung | 04: in grossen Feldern senkrechte Wiederholung der G4-Kacheln sichtbar (z. B. Reihe 3 Feld 1: dasselbe Zellmuster dreimal nebeneinander, Huefte/Kopf streifig). Im Render schwach, im Atlas deutlich | K |
| P14 | Krallen | G4-Krallen knochenbeige, Spitzen getrennt | 05 vorn Ruhe: Krallen beige-rosa mit getrennten Spitzen; im Spiel 5090 Lab (43, 25, 32) im Feuerschein. Machart G4 | F |
| P15 | Schwanz aus einem Stueck (E9) | keine Stufe in Helligkeit/Textur | A11 0,5; in 05 Seite / 3/4 hinten und 5080 Nachstellung keine Gliedgrenze sichtbar | F |

## 2. Warum P1 und P2 die "Pokemon"-Punkte sind

- Im neutralen Licht (03/05) besteht die Figur aus drei Dingen: fast schwarzer, leicht violetter Grund, ein helles rotes Netz gleich dicht
  ueberall, und stahlblaugraue Beine/Fuesse. Das ist eine Zweifarb-Zeichnung (Rot auf Blau-Schwarz), wie sie der Nutzer "bunt / wie ein Pokemon" nennt.
  G4 daneben ist schwarzbraun mit grossen Fleischflaechen (Bauch, Brust, Auge) und Knochen; EM30 ist helles Fleisch mit dunklen Adern.
- Die Rotklasse im Riegel (A3 15,1 %, A4 19 Grad) ist erfuellt, aber der Riegel misst Anteil und Farbton, nicht die Form des Rots.
  Der Unterschied liegt in der Form: Linie statt Flaeche (Erosionsrest 14-24 % gegen 34-53 %; groesste Flaeche 286-483 px gegen 1297-4382 px).
- Das Original Bild 1 zeigt auf dem Ruecken keine roten Adern, sondern "feine helle Linien" und "helle Baender" (farbe.md 1: zurueckgerechnet
  Lab (51, 1, -12) und (59, -4, -8), also hellgrau) und eine rosa Stelle. Die roten Adern sind Malform aus G4, nicht aus dem Screenshot.
  Durch die Videokette (Unschaerfe 2 px, Kennlinie) faellt das in den Riegeln K1-K5 nicht auf; im Spiel und auf der Familientafel schon.
- Das Blau (P2) kommt aus dem Farbfenster "-125..-55 Grad bis C* 14,5" (FARBE.md 2), begruendet mit der Rueckrechnung der Beine. Die
  Rueckrechnung selbst warnt aber, dass Blau und Lavendel dem Video gehoeren. In der Familie gibt es keine blaue Haut.

## 3. Fairness der Tafeln

| Pruefung | Ergebnis |
|---|---|
| Hintergrund je Kachel (03, 15 Kacheln, Stichprobe 70x10 px) | ueberall RGB (92,92,92): gleiche Buehne |
| Gamma / Aufhellung | `generator/kf_tafeln.py`: kein pow, kein Gamma (grep); Beschriftung "roh" auf jeder Kachel. Die frueheren Gamma-0,5/0,6-Stellen liegen in `k5_blatt.py:146/308` und `k5_rueckbild.py:65/100` (alte Tafeln), nicht in den neuen |
| Licht 03/05/06 | identisch fuer alle Modelle (0,55 + 0,45 N.L, weiss, Umkugel 0,46); L*/C*/L*>85 in jeder Beschriftung |
| Licht 01/02 "Spiellicht ROOM5080/5090, roh" | = Disc-Licht mal k aus `form/pruefung/licht.json` (Bild 1 k 1,06/0,95/0,83, Bild 2 1,20/1,12/1,00; `form_pruefung.py:286-292`), fuer K4 und K5c gleich. Fair zwischen den Kandidaten; die Beschriftung "roh" verschweigt den Faktor k. Gegen das echte Fenster (07) ist die 01-Kachel heller: dunkle Figurpixel (L*<35) im Mittel L* 15,0 (Kachel) gegen 8,4 (Nachstellung im Spiel) |
| Figurgroesse 03 | gleiche Umkugel, aber K5c (liegender Vierbeiner) deckt 16-19 k px, G4 37-41 k px, EM30 22-30 k px: die K5c-Textur wird mit etwa halb so vielen Pixeln gezeigt wie G4 und wirkt dadurch feiner, als sie ist |
| 07 Spiel | echtes Fenster (gdigrab), kein AUTOSHOT/SOFTWARE_RENDER (`spiel.log`: 4 Laeufe rc=0, Verfahren gdigrab) |
| Posen | 03 vergleicht K5c "Clip 2 Ruhe" mit EM30/G4/G5 "Clip 0"; Posen verschieden, fuer Farbe/Machart unerheblich |

Urteil: fair, mit zwei Einschraenkungen (Faktor k in der 01/02-Beschriftung, halbe Pixelzahl der K5c-Figur in 03).

## 4. Was Runde 2 aendern muesste (Sicht, keine Vorgabe fuer Zahlen)

1. Rot als Flaeche malen, nicht als Netz: wenige grosse Fleischpartien (Brust/Bauch/Kiefer wie G4, rosa Stelle Z1 wie Bild 1), Adern nur dort,
   dunkler oder gleich hell wie die Haut, mit Richtung entlang der Glieder (E12). Mass dafuer: Erosionsrest der Rotpixel im 03-Render >= 34 % (G4-Minimum),
   groesste Rotflaeche >= 1000 px auf der 03-Kachel.
2. Farbfenster fuer Blau schliessen (keine Koerpertexel mit b* < -6 bei C* > 8) oder auf den Familienwert 0 % im neutralen Render; die hellen
   Linien/Baender aus Bild 1 als Hellgrau (Lab 51-59, C* <= 9) fuehren, nicht als Blau.
3. P3/P4 nachsehen (Z5-Ausdehnung in die Pfote; O4 Gaumenstreifen im Kopffeld).
4. Beschriftung 01/02 um den Faktor k ergaenzen.
