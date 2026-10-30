# Spur P "alligator2" (Runde 41, Nachtrag 34 / Abschlussplan v0.8.30) - Dossier

Baum `.claude/worktrees/r41_alligator2`, Zweig `r41/alligator2`, Basis master f5a5e2f7e.

Punkte (Wortlaut AUFTRAG.md, massgeblich):
1. Nachtrag 31 Punkt 2: Kampfbeginn im Becken VOR Leon zu sehen, nicht direkt vor der Linse der laufenden Kamera (Cut 0);
   RE2-Band F-E2 (Abstand 11430..13924, erster Kontakt 121..158 Bilder, Zonen wie RE2) bleibt; Riegel Abstand zur Kamera / Bildanteil.
2. FG-E7 Option (a): Schluessel-Lagewahl sucht weiter, wenn die beste Zonenkamera < 10,4 px liefert; DISABLED der Riegel
   integration_r41_gatorszene_tod_r1/_elza entfernen, beide gruen.
3. Lagespruenge mit dem Kanister im Maul (Abnahme G 4 O1): Kopf-Klemme <-> Koerper-Klemme.
4. Riegel Uebergabe Ankunftsszene 2090 -> Knall + 8 Zombies 2080 ueber den echten Weg, ohne RE15_SET_FLAG=9:110, Leon + Elza.

## Stand (fortlaufend)
- Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.

## Punkt 1 (Nachtrag 31.2) - Zwischenstand Messung vorher (Baum f5a5e2f7e + Dossier, exe, RE15_WINDOW_SCALE=1, Leiterweg)
- Lauf V_L1/V_E1 (Rezept Abnahme F 5): Kampfbeginn F297, Gator (3197,-24407), Leon (-8200,-25550), Abstand 11454, Biss +131, Treffer +138,
  Gator-Wurzel <-> Kamera Cut 0 (2754,-26190) 1837 (kleinster Wert 1755), kleinster Knochen-Abstand 610 (test_r41_alligator2 kamera);
  Vordergrund-Anteil rechte Bildhaelfte > 25 % in 121 Bildern (F298..F429), > 50 % in 23 Bildern (F310..F392) - Leon = Elza.
- Geometrie: Band (Abstand >= 11402) + Zone 0 (z <= -18100) + Bild von Cut 0 schliessen sich fuer die WURZEL aus (alle Punkte im Bild von Cut 0
  mit z <= -18100 liegen <= ~10000 von Leon): die Wurzel kann am Kampfbeginn nicht im Bild liegen, der Kopf aber schon - Ziel: Nordostteil des
  Suedbeckens, von dort schwimmt er rechts ins Bild, statt vor der Linse zu liegen.

### Punkt 1 - Wahl (Stand wip f817b9e3)
Werkzeug test_r41_alligator2 lagesuche2 (echter Szenenablauf + 150 Bilder Kamera-Mass je Kandidat), Vorauswahl 3437 Kandidaten (Scratch P41/tools/kand_p.py = Form
leiche_lage_n5.py --kandidaten, Kampfbeginn >= 3500 von der Kamera). Gewaehlt Leiche (600,-24400) Gier 2560, Anlauf 1680, Durchbruch 22, Schluessel rx -640 rz -384.
Messung nachher (exe, Lauf D_L1/D_E1, Leon = Elza Bild fuer Bild): Kampfbeginn F299, Gator (3033,-21904) Gier 1843 Zone 0, Leon Zone 0, Abstand 11810 (SquareRoot0
11808), Biss +137, Treffer +144, Clip-Folge 0 F300, 1 F409, 4 F436 (kein Dash), Fress-Cut 1 frontal 4005, Wurzel <-> Kamera >= 4007 (vorher 1755), Knochen >= 2684
(vorher 610), rechte Bildhaelfte Median 28 % (vorher 45 %), 0 Bilder > 50 % (vorher 18..23). Schluessel: Primitivfarbe 86 -> 128, Quad-Kasten 10,4 x 12,6.
Riegel unit_r41_alligator2_kamera(_elza), integration_r41_alligator2_kamera(_elza).
