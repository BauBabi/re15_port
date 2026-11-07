# Einbau Runde 1 - Sicht-Pruefung (Riesenform K5c V3 Runde 5 als Spiel-Asset v0.8.30, E30)

Stand 2026-11-07 ~22:20. Pruefer: Sicht (Auge des Nutzers). Worktree `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form`, Zweig r41/form,
HEAD ce2c3a32d (Bauer-Commits f813205ca, 6ce793004, a005b357a, a582a8705, ce2c3a32d). Nichts committet; eigene Dateien unter
`analysis/befunde_runde41/form/varianten/K5/einbau/sicht/` (untracked, 4,4 MB).

**Ergebnis: BESTANDEN.** Das echte Spielfenster mit dem neuen Asset aus `re15_port/shared_assets/RE2/` zeigt dieselbe Form wie die vom
Nutzer abgenommene Vorlage (Tafel 05): in 5080 Schnitt 1 pixelgleich bis auf 2 nahezu schwarze Pixel, in 5090 Schnitt 11 innerhalb des
Feuer-Rauschbodens. Keine neuen Artefakte, K4 nirgends mehr. Zwei kleine Abweichungen (Punkte 7 und 8), keine, die der Nutzer ablehnen wuerde.

## 1. Was abgenommen wurde (Referenz)

Vorlage `C:/workspace/git/reAi_v2_entscheidung_2026-11-06/7_riesenform_runde5_krallen_farben/`, Bilder 01-09, Nutzer: "Nein, wie lassen es so
wie es jetzt ist. Sieht doch gut aus. Auch die Leuchtstelle sieht gut aus." Tafel 05 (`05_spielfenster_5080_5090.png`, 1932 x 2460) = Spalte 3
sind die rohen Spielbilder `farbe/spiel_v3/spiel_*.png` (640 x 480, gdigrab, 17-Zeilen-Variante ueber RE15_RE2_ASSET_ROOT, 3-Clip-EMD):

| Tafel-05-Zeile | Spielbild der Vorlage | Lage/Clip |
|---|---|---|
| Z1 5080 Nachstellung Bild 1 | `spiel_pose1_nachstellung_r5080.png` | -20711,-15984, Gier 756, Variante Clip 0 Bild 0, Schnitt 1 |
| Z2 5080 Lage Bild 1 | `spiel_pose1_r5080_s1.png` | form.json bild1 -18564,-18386, Gier 80, Clip 0 |
| Z3 5080 Ruhe | `spiel_ruhe_r5080_s1.png` | -18564,-18386, Gier 80, Variante Clip 2 (eigene Ruhe) |
| Z4 5090 Pose Bild 2 | `spiel_pose2_r5090_s11.png` | 941,-23776, Gier 3874, Variante Clip 1, Schnitt 11 |
| Z5 5090 Ruhe | `spiel_ruhe_r5090_s11.png` | 941,-23776, Gier 3874, Variante Clip 2 |

## 2. Was gemessen wurde

### 2.1 Asset und Textur (statisch, unabhaengig nachgerechnet)

- `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` sha256 1ab56076b20ef022c45a4517d944c0829ff93120124b6c7bb3601c7373736041,
  `BAHNSTEIG_FORM.TIM` sha256 7187ff097f42427dc40fc77264f43cc5860f6b28de70fb1549c626ef92af75e3 = `stufe3/asset/` byte-gleich,
  TIM = `stufe3/basis_k5c/FORM.TIM`. K4 (16eb6ea8... / ec280802...) liegt nur noch unter `varianten/K4/`. Einziger Ladepfad im Port:
  `re15_port/platform/pc/main.c:1111-1112` (`re15_pc_read_re2("BAHNSTEIG_FORM.EMD"/".TIM")`), kein zweiter Fundort unter `shared_assets/` (`find -iname "*BAHNSTEIG*"`).
- TIM (eigener Parser): 8 bpp, Pixel 256 x 256 (= 512 Texel), CLUT bei (0, 480), Asset 16 Zeilen, Variante 17 Zeilen. Pixelblock byte-gleich.
  Jede Asset-Zeile r entspricht der Varianten-Zeile (r < 2 ? r : r + 1) fuer alle 16 Zeilen: True. Die entfallene Varianten-Zeile 2 ist
  weder Zeile 0 noch Zeile 1 (also wirklich G4-Zeile 2, von keiner Flaeche gelesen).
- EMD (`re15_port/tools/form_emd.py lese_emd`): 3828 Flaechen in 32 Meshes, Knochen gleich, Ecken je Mesh gleich; fuer alle 3828 Flaechen gilt
  Asset-Zeile = (Varianten-Zeile < 2 ? Zeile : Zeile - 1), die unteren 6 clut-Bits gleich, Ecken/UV/Page/Normalen 0 abweichend.
  Asset-Zeilen 0:155 1:252 2:770 3:696 4:62 5:6 6:25 7:114 8:45 9:187 10:152 11:204 12:131 13:229 14:232 15:568 (= Bauer-Tabelle E.1).
  Hoechste Zeile 15 -> Port-Texturhoehe 256 x 16 = 4096. Clips: Asset 28, Variante 3.
- Spiel-Log (eigener Lauf mit RE15_TIM_LOG=1, `re15_port/build/form_spiel/5271_221133/debug.log`):
  `[tim] slot 11: 512x4096 8bpp cluts=16 — 0.0% transparent, 22.6% opak-schwarz`; in allen 8 eigenen Laeufen
  `[enemy] Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips -> Slot 11/12`, keine FAILED-/WARNUNG-/"fehlt oder defekt"-Zeile; stderr nur `[lautstaerke] re15_options.cfg fehlt`.

### 2.2 Eigene Spiellaeufe (`einbau/sicht/sicht_lauf.sh`, Log `sicht_lauf.log`, 8/8 rc 0, alle "Verfahren: gdigrab (Versuch 1)")

`re15_port/tools/form_spiel.sh` mit eigener exe-Kopie je Lauf (`re15_port/build/form_spiel/<pid>_<zeit>`), echtes Fenster per gdigrab,
kein AUTOSHOT, kein SOFTWARE_RENDER, Ende nur der eigenen PID. Asset-Laeufe OHNE --emd/--tim (keine Umleitung, das Spiel liest shared_assets/RE2);
Gegenprobe 17 Zeilen mit --emd/--tim `farbe/K5c_farbe_v3/FORM.EMD/.TIM` (Umleitung), gleiche Kette.

| Datei (`einbau/sicht/`) | Raum/Schnitt | Lage | Clip/Bild |
|---|---|---|---|
| `sicht_nachst_r5080.png` | 5080 / 1 | -20711,-15984, Gier 756 | Asset Clip 0 Bild 11 |
| `sicht_pose1_r5080_s1.png` | 5080 / 1 | -18564,-18386, Gier 80 | Asset Clip 0 Bild 11 |
| `sicht_ruhe_r5080_s1.png` | 5080 / 1 | -18564,-18386, Gier 80 | Asset Clip 2 Bild 0 |
| `var17_ruhe_r5080_s1.png` | 5080 / 1 | -18564,-18386, Gier 80 | Variante Clip 2 |
| `sicht_pose2_r5090_s11_{1,2,3}.png` | 5090 / 11 | 941,-23776, Gier 3874 | Asset Clip 12 Bild 0 |
| `sicht_ruhe_r5090_s11_{1,2,3}.png` | 5090 / 11 | 941,-23776, Gier 3874 | Asset Clip 2 Bild 0 |
| `var17_pose2_r5090_s11_{1,2,3}.png` | 5090 / 11 | dito | Variante Clip 1 |
| `var17_ruhe_r5090_s11_{1,2,3}.png` | 5090 / 11 | dito | Variante Clip 2 |

### 2.3 Pixelmessung 5080 (deterministisch, kein Feuer; `sicht_auswertung.py`, max 8-Bit-Kanalabstand)

| Vergleich | Pixel-Diff | max |
|---|---|---|
| eigener Lauf Nachstellung vs Bauer-Lauf `einbau/spiel/einbau_pose1_nachstellung_r5080.png` (Wiederholbarkeit) | 0 | 0 |
| eigener Lauf Nachstellung (Asset 16 Z.) vs Vorlage-Spielbild Tafel 05 Z1 (Variante 17 Z.) | **2** | 5 |
| Asset Clip 0 Bild 11 an form.json-Lage vs Vorlage Tafel 05 Z2 (Variante Clip 0) | **0** | 0 |
| Asset Ruhe Clip 2 Bild 0 vs Vorlage Tafel 05 Z3 (Variante eigene Ruhe) | 571 | 85 |
| Asset Ruhe vs 17-Zeilen-Variante Ruhe, gleiche Kette heute | 571 | 85 |
| 17-Zeilen-Variante Ruhe heute vs Vorlage-Spielbild Z3 (Kette wiederholbar) | 0 | 0 |
| Asset vs K4 heute, Lage Bild 1 (`heute_spiel_pose1_r5080_s1.png`) | 18 054 | 208 |
| Asset Ruhe vs K4 heute Ruhe | 17 329 | 191 |

Die 2 Pixel: (311,190) Asset RGB 12/7/10 gegen Variante 15/10/12; (331,265) Asset 12/12/12 gegen Variante 12/7/12 - beide nahezu schwarz,
Abstand <= 5 von 255, mit dem Auge nicht sichtbar (Bauer E.6 Punkt 4; Ursache SDL-Texturkoordinate v/tex_h 4352 gegen 4096, nicht nachgewiesen, fuer die Sicht unerheblich).
Die 571 Pixel der Ruhe liegen alle im Kopfkasten x 251-291, y 111-140 (Kopf-/Kiefer-Winkel: Asset = Ruhe von J, Variante = eigene Ruhe
Kopf 150 / Kiefer 60); Koerper, Beine, Krallen, Schwanz, Farben sind pixelgleich (Kachel 3 mit Diffkarte).

### 2.4 Messung 5090 (Feuer flackert, kein Pixelvergleich moeglich)

Minimum ueber 3 Bilder je Lauf (Feuer additiv hell, Kreatur stabil), Kreatur-Kasten x 190-560, y 140-430, Feuermaske (R > 140, R > G + 40, G > B + 20), Anteil Pixel mit Abstand > 8:

| Vergleich | > 8 | > 32 |
|---|---|---|
| Rauschboden: Bild 1 vs Bild 2 desselben Asset-Laufs (Pose 2) | 15,6 % | 13,3 % |
| Pose 2: Asset (min 3) vs 17-Zeilen-Variante (min 3) | **11,3 %** | 9,4 % |
| Pose 2: Asset (min 3) vs Vorlage-Spielbild Tafel 05 Z4 (1 Bild) | 14,1 % | 11,9 % |
| Ruhe: Asset (min 3) vs 17-Zeilen-Variante Ruhe (min 3) | **12,5 %** | 8,4 % |
| Pose 2: Asset vs K4 heute | 46,9 % | 19,9 % |
| Ruhe: Asset vs K4 heute | 42,3 % | 16,0 % |

Asset gegen Vorlage/Variante liegt unter dem Rauschboden, Asset gegen K4 beim Drei- bis Vierfachen. Der Rest ist Sichtvergleich (Kacheln 2 und 4).

### 2.5 Eigene Kacheln (roh, NEAREST, `einbau/sicht/`)

- `kachel1_5080_nachstellung.png` - 5080 Nachstellung: Becken-Leuchtstelle/Ruecken, Vorderbeine/Krallen, Hinterbein/Krallen, Schwanz; Spalten Asset neu | Vorlage-Spielbild | 17 Zeilen gleiche Kette | K4.
- `kachel2_5090_pose2.png` - 5090 Pose Bild 2: Kopf/Maul/Gesichts-Leuchtstelle, Vorderkrallen im Feuer (Einzelbilder).
- `kachel3_5080_ruhe.png` - 5080 Ruhe an der Vorlage-Lage: Asset | Variante | Vorlage-Spielbild | K4, dazu Diffkarte Asset gegen Variante x8.
- `kachel4_5090.png` - 5090 Minimum-Bilder: Kopf/Maul Pose 2, Kopf/Maul Ruhe, Vorderkrallen Pose 2.
- Bauer-Leisten gesehen: `farbe/11_einbau_spiel.png` (Original | K4 | Asset neu, 4 Zeilen), `einbau/ez3_gegenprobe.png` (17 | 16 | Diff x 32, Szenen 01/02/05, alle Diff 0 - die dritte Spalte ist durchgehend schwarz, wie es sein muss).

## 3. Bewertung je Punkt

| # | Punkt | Soll (abgenommen) | Ist (Spiel, Asset neu) | Bewertung |
|---|---|---|---|---|
| 1 | Farben/Zweifarbigkeit 5080 (Rumpf dunkel violett, Vorderbeine/Schulter braunrot, Kopf grau-violett; Frage 1 -> A) | Tafel 05 Z1-Z3, Tafel 06 Runde 5 | Nachstellung 2 px Diff (max 5), Lage Bild 1 0 px Diff; Ruhe Koerper/Beine 0 px ausser Kopfkasten; Kachel 1 und 3 ohne Unterschied | wie abgenommen |
| 2 | Krallen (dunkelrote Klaue, khaki Spitze, durchgehender Verlauf, keine Sprenkel; Runde 5) | Tafel 04 Runde 5, Tafel 05 | 5080: Vorder- und Hinterkrallen pixelgleich (Kachel 1 Zeilen 2-3); 5090: Kachel 2/4 Krallen gleich, nur Feuer verschieden | wie abgenommen |
| 3 | Leuchtstelle Becken in Bild-1-Lage (Frage 2 -> bleibt) | Tafel 09, Tafel 05 Z1 | rosa-mauve Fleck auf dem Becken an derselben Stelle, gleiche Groesse und Farbe (Kachel 1 Zeile 1), pixelgleich | wie abgenommen |
| 4 | Leuchtstelle Gesicht am Maul + Maulband lachsrot, Zaehne hell (Bild 2) | Tafel 08 Z1, Tafel 05 Z4 | 5090 Pose 2 Kopf/Maul Asset = Variante = Vorlage innerhalb Feuer-Rauschen (Kachel 4 Zeile 1, 2.4) | wie abgenommen |
| 5 | Form/Kopfform (K5c, unveraendert) | Tafel 01/02/05 | Gleiche Silhouette, Kopf, Kiefer, Schwanz in allen 4 Szenen; Asset vs K4 17-18 k Pixel (5080) bzw. 42-47 % (5090) verschieden - K4 ist weg | wie abgenommen |
| 6 | Keine neuen Artefakte durch 17 -> 16 Zeilen (Mosaik, falsche Zeile, schwarze Flaechen, fehlende Textur, Zeilenversatz) | Bild gleich | Zeilenabbildung fuer 3828/3828 Flaechen korrekt, Paletten gleich, Spiel-Log cluts=16 / 512x4096, Sichtkacheln ohne Mosaik/Loecher/Schwarzflaechen; einzige Spur sind die 2 Pixel (<= 5/255) | wie abgenommen |
| 7 | Ruhe-Pose 5080/5090 (Clip 2 Bild 0 = "Ruhe von J", Dossier E.3/E.5: "Kiefer zu") | Tafel 05 Z3/Z5: Variante mit eigener Ruhe (Kopf 150 / Kiefer 60, Maul offen) | Asset-Ruhe weicht von der Vorlage-Ruhe nur im Kopfkasten ab (571 px, 5080); in 5090 steht das Maul in der Asset-Ruhe ebenso OFFEN mit sichtbaren Zaehnen (Kachel 4 Zeile 2, `min_asset_ruhe_r5090.png`), obwohl das Dossier "Ruhe mit geschlossenem Kiefer (wie K4)" sagt; K4-Ruhe hatte das Maul zu | Abweichung klein (Bild entspricht der abgenommenen Vorlage; der Dossier-Text "Kiefer zu" stimmt mit dem Bild nicht ueberein - fuer den Bauer/Integration: K5c-Kiefer (Knochen 23 [784,420,0]) wirkt mit dem J-Ruhewinkel offen) |
| 8 | Bauer-Tafel 11, Zeile 2 "Ruhe": K4 an der form.json-Lage (Gier 80) neben Asset neu an der Nachstellungs-Lage (Gier 756) | gleiche Lage je Zeile | Zwei verschiedene Lagen in einer Zeile; dadurch sieht die Ruhe in Tafel 11 anders aus als in Tafel 05 Z3. Meine Kachel 3 vergleicht an derselben Lage: dort passt es | Abweichung klein (nur die Tafel, nicht das Asset) |
| 9 | Spiel liest das Asset aus shared_assets (keine Umleitung), Lader/Logs sauber | Lade-Zeile 32/32/28, keine Fehler | 8/8 Laeufe, Asset-Laeufe ohne RE15_RE2_ASSET_ROOT, Lade-Zeile vorhanden, keine FAILED/WARNUNG, Hash der geladenen Datei = Commit a005b357a | wie abgenommen |

Keine Bewertung der dritten Klasse ("Abweichung, die der Nutzer ablehnen wuerde") -> **bestanden**.

## 4. Widerlegungsversuche (alle gescheitert, d. h. kein Fehler gefunden)

1. "Das Spiel laedt noch K4": Hash geladen = neu; Spielbild Asset vs K4 heute 18 054 px (5080) - K4 unterscheidet sich massiv, Asset nicht von der Vorlage.
2. "Zeilenversatz durch die Zusammenlegung": alle 3828 Flaechen lesen die farbgleiche Zeile; Spielbild 16 Zeilen = 17 Zeilen bis auf 2 Pixel <= 5/255; Rundumrender (Bauer E-Z3) 0.
3. "Die Pose im 28-Clip-Asset ist nicht die der Vorlage": Clip 0 Bild 11 an der form.json-Lage 0 px gegen Variante Clip 0; Nachstellung 2 px.
4. "Die Ruhe sieht anders aus als in der Vorlage" (Tafel 11 Z2 legt das nahe): an der gleichen Lage 571 px, nur Kopfwinkel; die Orientierung in Tafel 11 kam von der anderen Gier (756 statt 80).
5. "5090 ist nicht messbar": Minimum ueber 3 Bilder + Feuermaske: Asset vs Variante 11-13 % unter dem Rauschboden 15,6 %, Asset vs K4 42-47 %.
6. "Textur zu gross / nicht geladen auf dem Zielgeraet": Port-Log 512x4096 cluts=16 (= Grenze re2fx_pc.c:35-45); 17 Zeilen waeren 4352.

## 5. Offen / Hinweise fuer Bauer und Integration (keine Sicht-Maengel)

- Punkt 7: Dossier FARBE.md E.3/E.5 sagt "Ruhe = Ruhe von J mit Kiefer zu"; im Spiel steht das Maul in Clip 2 Bild 0 offen (Zaehne sichtbar, 5090 `einbau/sicht/min_asset_ruhe_r5090.png`, Kachel 4 Zeile 2). Entweder Text berichtigen oder - falls das Maul im Stehen zu sein soll wie bei K4 - den Kieferwinkel am K5c-Kiefer (Knochen 23, Versatz [784,420,0] statt J [500,400,0]) pruefen. Der Nutzer hat das offene Maul in Tafel 05 Z5 gesehen und abgenommen; fuer v0.8.30 keine Sichtaenderung noetig.
- Punkt 8: Tafel 11 Zeile 2 mischt zwei Lagen; fuer eine Nutzer-Vorlage besser die form.json-Lage in beiden Spalten (wie Tafel 05 Z3).
- E-Z3 Szene 07 "ROT nach 1,0-Schwelle" (2 Pixel <= 5/255): aus Sicht des Auges hinnehmbar; die Schwelle fuer echte Spielbilder sollte den 8-Bit-Abstand (hier 5) statt "0 Pixel" fassen.
