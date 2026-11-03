# Nachbesserung 2, Textur-Spur (Nutzerbefund "Die Textur vom Vierbeiner wirkt immer noch kaputt", E22)

Teil der Nachbesserung 2 der Stufe 3 (Kopf-Spur im Hauptstrang, diese Datei nur Textur). Kein RE-Wert: Textur eines neuen Modells,
alle Zahlen sind Gestaltungs- bzw. Messwerte mit Herkunft (Werkzeug, Datei). Laufendes Dossier; Abschnitt 0 ist der Stand.

## 0. Stand

| Schritt | Stand |
|---|---|
| 1 Messwerkzeug (Netz, Bild, Heatmap) | erledigt: `stufe3/gen/s3_n2_textur.py netz|bild`, Tabelle `s3_n2_t_tabelle.py`, Blaetter `s3_n2_t_blatt.py` |
| 2 Ursache | gemessen (Abschnitt 1): Gelenkkoerper = 30 % der sichtbaren Figur mit 97 Einheiten je Texel (Mosaik); Seite 3 mit gespiegelten Wundflecken; Faecherkappen von Kopf/Kiefer als Streifen |
| 3 Bau-Funktion | erledigt: `python s3_n2_textur.py bauen <K-Varianten-Dir> <AUS_DIR>` (Abschnitt 2), Probe mit K3: `stufe3/roh/n2_textur/k3_n2az` |
| 4 haut_stoff fuer Seite 3 | erledigt (Abschnitt 3); Einbau in k_bau.malen = Hauptstrang |
| 5 Formpruefung nachher (K3-Kopf) | Bild 1 0,954 31/31 5/7, Bild 2 0,948 7/7 9/9 (= NB1); Zeichnung 3/9 / 2/7 (NB1 4/9 / 2/7: Z9 faellt, Abschnitt 4) |
| 6 Echtes Fenster | `stufe3/nachbesserung2/textur/echt_J_NB1_NB2.png` (Abschnitt 5) |
| Offen | Abschnitt 6 |

## 1. Messung und Ursache

Werkzeug `stufe3/gen/s3_n2_textur.py` (Masse im Kopf der Datei):
- **netz** (blickunabhaengig): je gemeinsamer 3D-Kante zweier Flaechen eines Meshes die oertliche Mittelfarbe beider Seiten (Texel 0,5/1,5/2,5
  nach innen, drei Stellen) -> Kantensprung |dRGB|, Anteil Kantenlaenge mit Sprung > 30; Blockmass = dY je Texelschritt x Texelgroesse
  (Einheiten je Texel) / 100; Hellfleck = Texel > Median + 60; Gestreckt = Flaechenanteil mit Texelgroesse > 3 x Median oder UV <= 1 Texel.
- **bild**: Port-Zeichenweg der Buehne (form_buehne, Spielkamera, Raumlicht) mit Kennung, Albedo und Texel je Pixel; 7 Ansichten
  (5080 Schnitt 4, Lage -23500,0,-18200, Clip 2 Bild 0, Gier 0/1024/2048/3072; 5090 Schnitt 11, Lage 941,0,-23776, Clip 10 Bild 45,
  Gier 3874/802/1826). Kante = Sprung der Mittel von je 2 Pixeln beiderseits einer Polygonkante, Innen = dasselbe innerhalb einer Flaeche,
  Block = Sprung an Texelgrenzen x Kantenlaenge des Texels in Pixeln (gross und kontrastreich = Mosaik), Texel px = mittlere Texelgroesse
  im Bild.

Saetze: J = Spiel-Asset der Abnahme 1 (`stufe3/roh/n1_vorher`, bzw. netz: `varianten/J_dicker`), K3 = `varianten/K_alligator/K3_fit`
(vor dem NB1-Texturfix), NB1 = `stufe3/basis_n1` / `stufe3/asset` (Stand jetzt im Spiel), NB2 = Bau dieser Spur auf K3
(`stufe3/roh/n2_textur/k3_n2az`, Spiel-Asset `asset_k3_n2az`, Kopf noch K3).

**Befund im Bild** (`s3_n2_textur.py bild`, alle 7 Ansichten zusammen, `nachbesserung2/textur/bild_J_NB1_NB2.json`):
die Gelenkkoerper tragen bei NB1 **30 % der sichtbaren Figurpixel** (Schulter, Ruecken-Buckel, Hals, Knie, Fuss; Kennungsbild: in 5080 Gier
3072 allein Arm r - Brust + Muffe 23 800 px). Ihre Haut ist seit NB1 ein Stueck von 12-32 Texeln, auf Ellipsoide bis 5800 Einheiten
Umfang gezogen: **97 Einheiten je Texel** (Rumpf 41, Glieder 26-49), im Bild 9,9 px je Texel gegen 3,2 am Rumpf. Das sind die grossen,
flachen Quadrate (Innenkontrast 1,8, Kante 16,2), die im Spiel als Mosaik/Schachbrett an Schulter, Hals und Ruecken stehen.

Nebenursachen (Kopf-Spur, Seite 3):
- Seite 3 (Kopf/Kiefer) ist aus `k_bau.stoff` = Brust-Haut von J **gespiegelt gekachelt**, samt der weiss-rosa Wundflecken der Brust:
  Hellfleck Kopf 2,6 % / Kiefer 5,8 % (J 1,9 / 0,0), Kantensprung Kiefer 17,9 (J 8,0). Im Spiel die weiss-rosa Kleckse am Kopf.
- Faecherkappen der Rohre (`k_form.rohr`, spitze0/1): Mitte und Ring liegen in derselben Texelzeile, die Kappe zeigt EINE gezogene
  Texelreihe = konzentrische Streifen auf der Schnauzenspitze (Gestreckt Kopf 9,4 %, Kiefer 5,3 %; J 2,9 / 0,0).
- Kein Seiten-/CLUT-Fehler (NB1 N0 bestaetigt): Buehne und echtes Fenster zeigen dasselbe.

## 2. Behebung: `s3_n2_textur.py bauen K_DIR AUS_DIR` (`s3_n2_textur_bau.py`)

- Je Gelenkkoerper eine **periodische Hautkachel** P x P = 64 x 64 Texel der CLUT-Zeile seiner Umgebung (Zeile der 12 naechsten
  Originalflaechen desselben Meshes), durch **Quilting auf dem Torus** (Bloecke 20 x 20, Raster 16, Schnitt mit minimalem Fehler auch ueber
  den Umlauf) aus den sauberen Hauttexeln DIESER Zeile (ohne Hell-/Wundflecken: Y bzw. Buntheit gegen den oertlichen 9 x 9-Median <= +35).
  Nur vorhandene Indizes; danach Mittelfarbe auf die Umgebung (Median RGB) gebracht, indem jede Texelfarbe skaliert und auf den
  naechsten VORHANDENEN Eintrag derselben CLUT-Zeile gesetzt wird: keine neue Farbe, keine Spiegelung. Koerper gleicher Zeile mit
  Ton +-7 teilen eine Kachel (K3: 13 Kacheln fuer 30 Koerper).
- Ablage: neue Seiten rechts der TIM (ab x 512, je Seite zwei Felder 128 x 128 = Kachel 2 x 2, damit jede Flaeche bis 64 Texel
  zusammenhaengend darin liegt). K3: 7 neue Seiten, **TIM 1408 x 256** (Port und form_emd lesen bis 15 x 128 + 255; im echten Fenster
  geprueft, Abschnitt 5).
- UV: **mittabstandstreue Azimutalabbildung** um den Aussenpol des Ellipsoids (+n: bei den Kugeln die sichtbare Beule), Radius = Bogen
  vom Pol / Texeldichte der Umgebung (Median der 12 naechsten Flaechen, 22-50 Einheiten je Texel), jede Flaeche um Vielfache von P ins
  Feld verschoben (Kachel periodisch = gleicher Inhalt). Stetig ueberall ausser am Gegenpol (steckt im Glied); keine Umlaufnaht, kein
  Pol-Stern aussen. Verworfen (gemessen): Laengen-/Breitengrad (Pol-Stern genau auf der sichtbaren Beule, Feld 64: 61 Flaechen ueber dem
  Feldrand, davon 6484 px sichtbar in 5080 Gier 2048); Feld 64 statt 128 (Klemmung sichtbar, s. o.).
- Ausnahme Ellbogen r: NB1-Projektion (traegt in Bild 1 das weisse Dreieck Z5, E1/E3/E14).
- **Kappen von Kopf/Kiefer** (`kappen_uv`): Faecher mit UV-Flaeche <= 1 Texel planar senkrecht zur Achse Ringmitte -> Spitze, Texeldichte
  der Rohrinsel, Mitte unter der UV-Mitte des Rings, um den Radius in die Insel hinein. Laeuft fuer jede Kopfform aus `k_form.rohr`.
- Alte Seiten: Texel rechts ueber x 511 auf 511 geklemmt (vorher hinter der TIM, jetzt laege es auf Seite 4).
- Bericht `AUS_DIR/textur_n2.json` (Kacheln mit Zielfarbe/Ist, Koerper mit Dichte/Kachel, Kappen, Klemmungen).

Aufruf fuer die neue Kopf-Variante (Hauptstrang): `python stufe3/gen/s3_n2_textur.py bauen varianten/K_alligator/<K4> stufe3/basis_n2`,
dann `s3_rig.BASIS_DIR` = basis_n2, `s3_clips.py`, Asset kopieren, `s3_testkopf.py` (TIM-Laenge/CRC aendern sich).
Hilfen: `s3_n2_t_asset.py BASIS SPIEL_EMD AUS` (Spiel-Asset mit neuer UV/TIM ohne s3_clips, nur fuer Messung/Fenster),
`s3_n2_t_sicht.py` (sichtbare Pixel geklemmter Flaechen), `s3_n2_t_lauf.sh`/`s3_n2_t_spielreihe.sh` (echtes Fenster).

## 3. `haut_stoff` fuer Seite 3

`s3_n2_textur.haut_stoff(tx, bau, ziel_wh, quelle=('huefte', 'wirbel', 'brust'), seed=7, glatt=10.0) -> (struktur, basis)` - gleiche
Signatur und Rueckgabe wie `k_bau.stoff`: tx = `form_buehne.Textur(J-TIM)`, bau = `J_dicker/bau.json`, ziel_wh = (W, H) Vielfache von 8;
struktur H x W x 3 float32 (Mittel 1 je Kanal), basis = Mittel-RGB der sauberen Quelltexel. Quelle: Rumpfinseln von J ohne Hell-/Wund-/
Braunflecken (oertlich wie oben, dazu Buntheit <= p80 + 8, Y zwischen p5 und Median + 30). Quilting auf dem Torus (Raster 8, Bloecke 11 x 11,
zufaellig unter den 8 besten), also periodisch in W und H: keine Naht am Umlauf der Zylinder-UV, keine Spiegelung. Grossform raus (Teilen
durch Gauss sigma 10, Umlauf). Gemessen (128 x 256): Hellfleck (Y > Median + 40) 5,2 % -> 0 %, Y max 245 -> 75, Buntheit p99 71 -> 14,
Umlaufsprung 11,1 -> 6,2 (Nachbarspalten 6,4, also unsichtbar). Laufzeit 4,5 s. Bild: `stufe3/roh/n2_textur/haut_stoff_vergleich.png`.
Einbau (Hauptstrang): in `k_bau.malen` und `k_bau.bauen` `stoff(` durch `haut_stoff(` ersetzen (`sys.path` auf stufe3/gen, `import
s3_n2_textur as T2`).

## 4. Zahlen vorher / nachher (Endstand `stufe3/roh/n2_textur/k3_n2az`, Kopf noch K3)

**Bild** (`s3_n2_textur.py bild`, 7 Spielansichten, `nachbesserung2/textur/bild_J_NB1_NB2.json`, Tabelle `s3_n2_t_tabelle.py --md`):
Kante / Innen / Block / Texel px / Anteil der Figurpixel.

| Teil | J (Abnahme 1) | NB1 (jetzt im Spiel) | NB2 |
|---|---|---|---|
| Rumpf (Referenz) | 10,7 / 6,0 / 37,8 / 3,22 / 31 % | 13,7 / 6,0 / 38,7 / 3,18 / 21 % | 13,3 / 6,0 / 38,7 / 3,18 / 21 % |
| Schwanz | 8,1 / 3,3 / 25,6 / 3,46 / 13 % | 9,6 / 3,2 / 25,7 / 3,50 / 10 % | 9,5 / 3,2 / 25,7 / 3,50 / 10 % |
| Beine | 14,2 / 8,8 / 30,3 / 2,20 / 9 % | 15,5 / 8,7 / 30,4 / 2,25 / 7 % | 15,4 / 8,7 / 30,4 / 2,25 / 7 % |
| Arme | 16,1 / 9,0 / 54,4 / 3,34 / 18 % | 16,0 / 9,0 / 55,0 / 3,14 / 11 % | 15,7 / 9,0 / 55,0 / 3,14 / 11 % |
| Hals | 21,6 / 10,0 / 45,4 / 3,05 / 2 % | 17,4 / 6,9 / 41,1 / 3,10 / 1 % | 17,5 / 6,9 / 41,1 / 3,10 / 1 % |
| **Gelenkkoerper** | - | 16,2 / 1,8 / **85,0** / **9,94** / 30 % | 13,8 / 5,3 / **24,4** / **2,74** / 30 % |
| Kopf (K3, Seite 3) | 12,3 / 4,3 / 62,1 / 6,30 / 17 % | 22,9 / 6,0 / 44,9 / 4,29 / 12 % | 23,9 / 5,4 / 38,7 / 3,85 / 12 % |
| Kiefer (K3, Seite 3) | 9,2 / 2,1 / 51,8 / 7,16 / 7 % | 32,0 / 13,7 / 66,7 / 2,86 / 5 % | 35,6 / 13,3 / 67,4 / 2,96 / 5 % |
| Klauen | 19,3 / 10,6 / 48,7 / 2,55 / 3 % | 20,9 / 8,3 / 31,1 / 2,21 / 3 % | 20,9 / 8,3 / 31,1 / 2,21 / 3 % |
| Grenze Gelenkkoerper/Haut (Kante) | - | 21,7 | 20,5 |
| **Gesamt** (Innen / Block / Texel px) | 6,0 / **44,3** / 3,96 | 5,4 / **44,5** / 5,28 | 6,4 / **35,0** / 3,05 |

Je Ansicht Block J / NB1 / NB2: 5080 Gier 0 29,8 / 34,2 / 28,8; Gier 1024 27,9 / 31,9 / 26,7; Gier 2048 38,5 / 43,6 / 32,6;
Gier 3072 73,3 / 65,5 / 52,4; 5090 Gier 3874 35,0 / 34,9 / 26,1; Gier 802 34,4 / 34,8 / 26,1; Gier 1826 21,0 / 24,4 / 20,1.
**Gegenprobe Mosaik: NB2 <= J in allen 7 Ansichten** (gesamt 35,0 gegen 44,3).

**Netz** (`s3_n2_textur.py netz`, `nachbesserung2/textur/netz_J_K3_NB1_NB2.json`): Kantensprung / Anteil Sprung > 30 / Blockmass /
Einheiten je Texel / Hellfleck.

| Teil | J | K3 | NB1 | NB2 |
|---|---|---|---|---|
| Rumpf | 8,5 / 0,029 / 2,92 / 41,1 / 0,011 | gleich | gleich | gleich |
| Gelenkkoerper | - | 15,1 / 0,097 / 3,49 / 47,5 / 0,009 | 9,9 / 0,041 / **7,16 / 97,4** / 0 | **7,9 / 0,011 / 1,72 / 27,1** / 0,001 |
| Kopf | 12,7 / 0,069 / 3,00 / 35,8 / 0,019 | 15,6 / 0,127 / 1,91 / 28,0 / 0,026 | 15,7 / 0,127 / 1,93 / 28,1 / 0,026 | 16,3 / 0,136 / 1,96 / 21,3 / 0,026 |
| Kiefer | 8,0 / 0 / 1,94 / 34,0 / 0 | 18,1 / 0,111 / 3,28 / 20,3 / 0,058 | 17,9 / 0,101 / 3,30 / 20,4 / 0,058 | 19,7 / 0,127 / 3,33 / 19,6 / 0,059 |
| Gesamt | 12,2 / 0,073 / 2,93 / - / 0,012 | 13,4 / 0,083 / 3,13 / - / 0,012 | 11,4 / 0,061 / 4,76 / - / 0,008 | 10,7 / 0,051 / 2,35 / - / 0,008 |

Gestreckt Kopf / Kiefer: J 2,9 / 0 %, K3 = NB1 9,4 / 5,3 %, NB2 6,4 / 4,5 % (Kappen planar; Rest = Zaehne und Augenhoecker auf
eigenen Kleinfeldern, gewollt). Kopf/Kiefer-Hellfleck und -Kantensprung bleiben, bis Seite 3 mit `haut_stoff` gemalt ist (Hauptstrang).

Bau K3 -> NB2: 13 Kacheln fuer 30 Koerper (Ellbogen r Projektion), Zielfarbe je Kachel = Median der Umgebung, Ist nach dem Angleichen
innerhalb 0,6-2,9 je Kanal (`textur_n2.json` kacheln: farbe_ziel / farbe_kachel; vorher lagen die gequilteten Kacheln fuer helle Umgebungen
bis 48 Y daneben: Muffe Bein r 88,9 -> 40,7); 0 Flaechen ueber dem Feldrand (`s3_n2_t_sicht.py`: 0 sichtbare Pixel geklemmt); TIM 1408 x 256
(362 528 Byte, vorher 133 152), 28 UV-Ecken der alten Seiten auf x 511 geklemmt (K3-Kopfnaht wie NB1).

**Formpruefung** (`form_pruefung.py pruefe stufe3/roh/n2_textur/k3_n2az/form.json`): Bild 1 IoU 0,954, 31/31, Boden/Verdeckung 5/7,
Zeichnung 3/9; Bild 2 IoU 0,948, 7/7, 9/9, Zeichnung 2/7. Gegen NB1 faellt **Z9** (Bild 1, Schraegstrich neben dem rechten Oberarm,
Material z9 = (85,80,60); FORMBLATT2: "ausserhalb aller Nutzer-Teile, kann Hintergrund sein"): Material 0,65 -> 0,55. Dort liegt der
Gelenkkoerper Arm r - Brust (Kennungsbild Bild 1: 37 von 44 px bei (411,240)); sein NB1-Stueck war zufaellig heller (Y 57) als die Brust,
jetzt traegt er den Ton seiner Umgebung (Y 49). Kein Verlust an Umriss/Punkten. Vergleich ohne Kappenfix: ebenfalls 3/9 (Z9 0,55), also
nicht vom Kopf.

## 5. Echtes Fenster

`stufe3/gen/s3_n2_t_lauf.sh` (eigene exe-Kopie `re15_pc_n2t_basis.exe` = re15_pc.exe Stand 8c7cc6754, gdigrab hwnd=, Renderer des
Nutzers, kein AUTOSHOT/SOFTWARE_RENDER, Fenster 1264/560, nur eigene PIDs beendet), Asset per `RE15_RE2_ASSET_ROOT`, Reihe
`s3_n2_t_spielreihe.sh` (5080 Schnitt 4 Ruhe Gier 0/2048/3072, 5090 Schnitt 11 Clip 10 Bild 45 Gier 3874/802), Laeufe unter
`re15_port/build_n2/spiel/tex_{J,NB1,NB2}_*`. debug.log in allen NB2-Laeufen `Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips`; die
1408 breite TIM wird im Port richtig gelesen (keine schwarzen/fremden Flaechen, Seiten 4-10 an Schulter, Ruecken, Hals sichtbar).

Blaetter:
- `stufe3/nachbesserung2/textur/echt_J_NB1_NB2.png` - echtes Fenster, je Ansicht J | NB1 | NB2, 2-fach: NB1 grosse graue Facetten/Quadrate
  an Schulter, Hals, Ruecken; NB2 dort durchgehende Hautkoernung wie am Rumpf; Schnauzenspitze ohne Streifen.
- `stufe3/nachbesserung2/textur/heatmap_J_NB1_NB2.png` - Buehne, je Ansicht und Satz Render | Texelbloecke | Kanten (rot = Gelenkkoerper-
  Grenze > 25, blau <= 25, gelb = sonstige Polygonkante > 25).
- `stufe3/nachbesserung2/textur/haut_stoff_alt_neu.png` - Seite-3-Stoff alt (k_bau.stoff, gespiegelt, Wundflecken) | haut_stoff | haut_stoff
  2 x gekachelt (keine Naht).

## 6. Offen

- Seite 3 (Kopf/Kiefer) ist noch der alte Stoff: Hellfleck Kiefer 5,9 %, Kante Kiefer 35,6 / Kopf 23,9 im Bild (J 9,2 / 12,3). Erst mit dem
  Einbau von `haut_stoff` in k_bau.malen (Hauptstrang, neue Kopfvariante) messbar besser; danach `bauen` auf die neue Variante und
  dieselben Masse (`netz`, `bild`) wiederholen.
- Zeichnung Bild 1 Z9 (s. Abschnitt 4).
- Grenze Gelenkkoerper/Haut 20,5 (Haut-Polygonkanten 12-16): Inhalt wechselt an der Durchdringungslinie (anderes Hautstueck), Ton ist
  angeglichen. Rest an J-eigenen Malstellen (helle Baender/Wunden neben Schulter und Hals, E11/E14, gewollt).
- J-eigene Inselnaehte an Armen/Beinen/Klauen (Kantensprung 14-20, 18-31 % der Kantenlaenge UV-unstetig) unveraendert wie J.
- Die Formpruefung des Spiel-Assets (s3_clips) laeuft erst mit dem Endstand des Hauptstrangs.
