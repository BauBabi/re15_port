# Zahlen-Pruefung Farbe Versuch 3, Runde 1 (K5c V3, G4-Uebertragung), 2026-11-07

Gegenstand: `varianten/K5/farbe/K5c_farbe_v3/` (FORM.EMD sha256 6f4d5fb0..., FORM.TIM sha256 47725110..., beide bytegleich mit
`farbe/K5C_FARBE_V3.EMD/.TIM`), Commits 47f5732c9 und 4b5800a76 auf r41/form.
Eigener Code (unabhaengig von FE/kf_g4/riegel, eigener EMD/MD1/TIM-Leser):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/zpruef/`
(`emd.py`, `g0.py`, `tim_chk.py`, `h1.py`, `s1.py`, `deg.py`, `vergr.py`/`vergr.json`, `feld.py`, `p1h2.py`, `h2b.py`, `p2kopf.py`, `ctest.log`,
`riegel_nach.txt`/`.json`).
Abtastung in meinem Code: jedes UV-Dreieck mit 861 baryzentrischen Punkten abgetastet, Texel = floor(u), floor(v). So waehlt auch die PSX-GPU
den Texel. Der Riegel zaehlt nur Texel, deren Mittelpunkt im Dreieck liegt (`kf_g4.texel_im_dreieck`).
Lab = OpenCV float (RGB/255), 15 Bit -> c5 x 255/31, wie im Messblatt.

## Ergebnis

**Nicht bestanden.**
- Der Riegel des Bauers ist ehrlich: 33/52 GRUEN, Exit 1. Mein Nachlauf ist zeilengleich (`riegel_nach.txt`).
- G0, die Herkunft der Seiten und der CLUT 0-2, P1 und die Tests bestaetige ich mit eigenem Code.
- Fuenf Riegelzeilen sind aber zu guenstig gemessen:
  - S1/H1 am Hals: bei mir ROT bzw. knapp.
  - F1t: nach dem Wortlaut des Messblatts ROT.
  - P2 am Kopf: gegen die tatsaechliche Quelle +21,2 statt +13,1.
  - D1 am Kopf: ortsgenau Median 4,0.
  - H2 in Zeile 7: 31 Vertauschungen.
- Zwei Aussagen der Bauer-Meldung sind im Beleg nicht gedeckt bzw. unvollstaendig.

## (1) Riegel-Zeilen nachgemessen

| Zeile | Bauer | eigene Messung | Bewertung |
|---|---|---|---|
| G0 | gleich, EDD+EMR bytegleich, 32 Kn., 3 Clips | Abschnitte 0-6 der EMD bytegleich. MD1: 32 Meshes, 1270 Dreiecke und 2558 Vierecke; tv/tn/tf/qv/qn/qf je Mesh gleich. 35 639 Bytes verschieden, **0 davon ausserhalb der UV-Bloecke**. Dateilaenge und Verzeichnis gleich | **bestaetigt GRUEN** |
| TIM | 512x256 8 bpp, Seiten 0-2 = G4, 9 Zeilen, 0-2 = G4 | Seiten 0/1/2 gegen G4-TIM @0x65e800: je 0 Pixel verschieden. CLUT 9x256, Zeilen 0-2 = G4. Genutzte CLUT-Woerter: Zeilen 0,1,3,4,5,6,7,8 (Zeile 2 nur als Basis von 5/6). TPage 128-131 | **bestaetigt** |
| H1 | Hals 99,85, sonst >= 99,91 | Abtastung mit Indexgleichheit und G4-Abtastmaske: Rumpf 99,22, Schwanz 99,46, **Hals 95,31**, Beine 99,19, Arme 99,14, Klauen 99,44, Kopf 99,11 | GRUEN, am Hals nur knapp (Soll >= 95) |
| S1 | max 0,15 % | Rumpf 0,78, Schwanz 0,54, **Hals 4,69**, Beine 0,81, Arme 0,86, Klauen 0,56, Kopf 0,89 % | **Hals ROT** (Soll <= 1 %). Ursache siehe Befund A |
| H1b | 1,13 % | 1,08 % | GRUEN |
| H2 | 0 Vertauschungen in den Zeilen 3-8 | Zeilen 3-6: 0. **Zeile 7: 31, Zeile 8: 2.** Ursache in Zeile 7: Eintrag 242 ist nicht umgefaerbt (L\* 49,0) und wird an Randtexeln abgetastet (0,27 % der Band-Abtastung). Der Riegel sieht ihn nicht, weil er nur Texelmittelpunkte zaehlt | formal ROT, in der Wirkung klein |
| P1 | 21,0 / 26,2 / 401 | 21,0 / 26,6 / 422 (ohne Sonderzeilen 19,4 / 27,5 / 351) | **bestaetigt GRUEN** |
| P2 | ROT (Kopf +13,1 gegen G4 10) | Der Riegel misst den Kopf gegen G4-Mesh 10. Die Quelle laut `kf_g4.ZUORDNUNG` ist aber G4-Mesh 16 (F18). Gegen 16 sind es **+21,2**, gegen 26 +23,6. Ausserdem ist Vorderbein l mit +8,2 ROT; die Meldung nennt das nicht | ROT, schlechter als gemeldet |
| D1 | ROT, Kopf 2,77, Beine 1,03 | Ortsgenau gemessen: je Flaeche sqrt((A3D/AUV) / G4-Dichte am selben Texel), flaechengewichtet. Median / Anteil > 2,0: Rumpf 1,61 / 27,5 %, Schwanz 1,96 / 47,9 %, Hals 1,45 / 26,6 %, Beine 1,06 / 11,0 %, Arme 1,68 / 33,0 %, Klauen 1,27 / 22,6 %, **Kopf 4,00 / 92,8 %** (p90 8,9) | ROT; am Kopf deutlich schlimmer als 2,77 |
| F1t | dE 0,2 (188 Texel) | Der Riegel mittelt nur Texel mit **umgefaerbtem** Eintrag: 188 von 620 genutzten. Ueber alle genutzten Texel: Lab (21,1 / 7,0 / -1,6), **dE 31,3**; abtastgewichtet dE 31,9. Nur die umgefaerbten: dE 0,19 | Nach dem Wortlaut des Messblatts ("genutzte Texel des Felds, Lab-Mittel") **ROT**. 70 % des Felds sind dunkle G4-Haut. Erlaubt ist nach 3.1 ein dunkler Rand, keine Mehrheit. Im Bild traegt K14 (siehe unten) |
| F2t | dE 6,18 ROT | Alle Texel: dE 5,45; abtastgewichtet: dE 3,21 | haengt an der Zaehlweise, Grenzfall. Das Bauziel ZIEL_BAND_BAU (183,99,92) liegt schon mit Absicht dE 5,12 neben dem Messziel |
| Zaehne | L\* 76,9, C\* 5,2 | L\* 74,4, C\* 6,35 | GRUEN |
| A1-A12, B10-B17, N1/N2, K1v2-K15, B2/B9/B9b | wie gemeldet | Riegel nachgelaufen, zeilengleich. K14: Lab (49,8 / 18,2 / -2,4) gegen (45,0 / 16,6 / -2,0), dE nachgerechnet 5,07. K15: dE nachgerechnet 7,87 | Ketten selbst nicht neu implementiert (Render- und Videokette), nur reproduziert |

Zaehlung: In der JSON stehen 55 Eintraege, davon 3 info-Zeilen (D1eff, K1, K2). 33 GRUEN und 19 ROT ergeben die 52 Zeilen.
ROT sind: D1, F2t, A1, P2, B11, B12, B17, N1, N2, K3, K7, K8, K1v2, K2v2, B2, B9, K12, K13, K15.

### Befund A: Flaechen ohne Texelmittelpunkt fallen aus H1, S1, P1 und F-Zeilen heraus

`riegel_farbe_v3.herkunft` ueberspringt Flaechen mit `if not len(xy): continue`. Gemeint sind Flaechen, deren UV auf einen Punkt oder eine Linie
geschrumpft ist. Ihre 3D-Flaeche zaehlt nur in D1. Im Spiel zeichnet die PSX diese Flaechen trotzdem, als einfarbige Fuellung bzw. als Streifen.
3D-Anteil dieser Flaechen je Teil (`deg.py`, `vergr.py`):

| Teil | UV ein Punkt | UV eine Linie | zusammen |
|---|---|---|---|
| Rumpf | 1,17 % | 0,46 % | 1,6 % |
| Schwanz | 0,38 % | 3,33 % | 3,7 % |
| **Hals** | **9,44 %** | 1,35 % | **10,8 %** |
| Beine | 0,55 % | 6,06 % | 6,6 % |
| Arme | 0,10 % | 0,94 % | 1,0 % |
| Klauen | 2,36 % | 0,25 % | 2,6 % |
| Kopf/Kiefer | 0 % | 8,57 % | 8,6 % |

- Am Hals sitzen zwei Vierecke (qf 55 und 111) mit allen vier UV auf (38,186). Der Texel liegt ausserhalb der G4-Abtastmaske.
- Das ist genau der "flache einfarbige Block", den Befund 2 der Bauer-Meldung als behoben meldet. `bau.json` zaehlt dazu "kollabiert".
- Der Riegel kann solche Flaechen nicht sehen.

## (2) Geometrie, Spiel-Asset, Pins

- G0: siehe oben, bestaetigt.
- `git diff 02aed15f7..HEAD`: Ausserhalb von `analysis/` ist nichts geaendert. `re15_port/` ist leer im Diff.
- `BAHNSTEIG_FORM.EMD` sha256 16eb6ea8...; zuletzt geaendert in 67408cfcb (2026-11-03, K4).
- `test_birkin_bahnsteig.c` mit MD1/TIM-CRC ist unveraendert.
- **Bestaetigt.**

## (3) Tests

- `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3` auf dem vorhandenen Bau: **11/11 bestanden**, 95,05 s (`zpruef/ctest.log`).
- Diese Tests pruefen K4 (das Spiel-Asset), nicht V3. Ueber die Farbe von V3 sagen sie nichts.
- Spiellauf-Log `spiel_lauf_v3.log`:
  - 10 x rc=0.
  - V3 laedt als "32 Meshes, 32 Knochen, 3 Clips -> Slot 12".
  - Die Meldung "laedt mit 9 CLUT-Zeilen" steht **nicht** im Log. Indirekt belegt ist sie durch Tafel 07: Die rosa F1-Stelle (Zeile 6) ist im Spielbild sichtbar.

## (4) Gegenproben

- Riegel-Gegenprobe V2r2: H1, P1, K11, K14 und K15 sind ROT, G4 ist bei P1 GRUEN. Das ist nachgelaufen und zeilengleich.
- Eigene Gegenprobe H1 mit Indexgleichheit an V2r2: 0,11-0,38 % je Teil, also ROT, wie es sein soll. Der Kopf faellt dort ganz in Sonderzeilen.
- Eigene Gegenprobe P1 an G4: 17,7 / 25,6 / 423, GRUEN. Das Messblatt nennt 17,1 / 25,6 / 420.
- **Nicht im Riegel**, obwohl in Messblatt 5 als Gegenprobe gefuehrt:
  - G4 selbst bei B10-B17 (8/8 GRUEN),
  - die (b2)-Probe A bei K11/K12/K13/S1,
  - Orange/Gelb bei F1t/F2t,
  - Kopf aus G4-F1 bei P2/D1.
  - Diese Gegenproben stehen nur als Werte im Messblatt, nicht als Lauf.

## (5) Konstanten im Diff 15ad19969..HEAD (generator/)

- Mit Quelle (Messblatt, Leser A/B, Messung):
  - G4_KIND, CLUT_Y, Z_*, KAPPE_NA,
  - B1_ROT_LAB, G4_ROT_LAB,
  - ZIEL_F1, ZIEL_BAND, ZIEL_ZAHN,
  - F1_FLAECHEN (eigene Pruefung: Mesh 0 qf 7/8/19/20/31/32 tragen Zeile 6),
  - K5C_ZAHNRECHT/AUGENRECHT, FELD, ZUORDNUNG,
  - alle Riegelgrenzen in `riegel_farbe_v3.py` 41-57.
- "PORT-WAHL Bauer" **ohne Nutzer-Wortlaut** (gegen die Auftragsregel "PORT-WAHL mit Nutzer-Wortlaut"):
  - VERSCHIEBE_N = 300,
  - SCHRUMPF-Stufen,
  - ROT_C0/C1/H0/H1,
  - B1_C_JE_SEITE (nach Riegellaeufen 1/2 gewaehlt),
  - ZIEL_BAND_BAU (183,99,92) (bewusst dE 5,1 neben dem Messziel, auf die Kette hin),
  - BAND_SCHWELLE = 0,3 (`kf_g4_daten.py`).
- Veraltete Kommentarzahlen:
  - `kf_g4.py:121` nennt K13 46,9 % / K6 dE 4,3; der Riegel sagt 46,2 / 4,7.
  - `kf_g4.py:80` nennt K15 dE 8,9 / K7 12,2; der Riegel sagt 7,9 / 10,2.
- `riegel_farbe_v3.py` P2_TEILE: Der Kopf wird gegen G4 [10] gemessen, die Quelle ist aber G4 16 (siehe P2).

## (6) Tafeln

- Tafeln 01/02/03/06/07 gesichtet.
  - Gleiches Licht und gleicher Filter fuer K4 und V3.
  - Die Familienkachel ist roh, mit Licht 0,55 + 0,45 N.L fuer alle fuenf Modelle.
  - In 01/02 ist vermerkt, dass Z.1 heller ist als das Spielfenster.
  - Nichts ist einseitig aufgehellt. **Fair.**
- Hinweise:
  - Die Kopf-Vergleichskachel in 06 zeigt G4-Mesh 26, nicht die tatsaechliche Quelle F18 (Mesh 16).
  - In 02 Z.1/Z.2 und 03 sind die hellen Krallenspitzen (N2) als weisse Bloecke sichtbar.
- Baumstand:
  - Sauber bis auf die fremde, unversionierte `FARBE_PRUEFUNG_v3_r1_sicht.md` des Sicht-Pruefers.
  - Kein Upstream, kein Remote-Zweig enthaelt HEAD, also kein Push.
  - Keine Merge-Commits in 02aed15f7..HEAD.
