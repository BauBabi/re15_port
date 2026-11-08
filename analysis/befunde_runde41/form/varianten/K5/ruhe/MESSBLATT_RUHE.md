# Messblatt RUHE: ruhige Haut fuer K5e (Messer, 2026-11-08)

Auftrag: Nutzer 2026-11-08 zu K5d: "Die Flecken an der Textur sehen aus wie drauf gemalt.... nicht so sauber wie bei den rest des spieles" /
"Alles wirkt unruhig und so frei verteilt." / "Wie als hätte das Modell Hautprobleme. Alles bunt." Dazu: "unterkiefer soll außen natürlich auch
nicht leuchten sondern dunkel/grau sein." Regel: Memory `reai-v2-texturen-nicht-aus-video-eichen` samt Nachtrag 2026-11-08.

Hier wurde nur gemessen und geplant. Es gibt kein Asset und keine Varianten-Datei. Die "Vorschau" in Abschnitt 3.5 existiert nur im Speicher
der Messskripte, damit Helligkeitsfaktor und Riegel an einem echten Render geprueft sind. Spiel-Asset unveraendert:
`re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` sha256 1ab56076..., `.TIM` 7187ff09... (nach der Messung nachgerechnet).

## 0. Ergebnis zuerst

1. **Ursache in Zahlen: "bunt" heisst zwei Farbton-Familien, nicht mehr Flecken.**
   - Unter weissem Licht (Licht der Familien-Tafel 07, 4 Winkel, Ruhe-Clip) haben G4 und EM30 je **eine** warme Familie (0-60 Grad).
     Sektoren >= 5 % (gepoolt): G4 4, EM30 4. K5c und K5d haben **zwei** Familien: mauve 300-345 Grad (K5c 73 %) und braun 15-45 Grad (27 %).
     Sektoren: K5c 6, K5d 6.
   - Farbtonspanne (90 % der bunten Pixel): G4 61,8 / EM30 60,2 Grad gegen K5c 88,1 / K5d 88,3 Grad.
   - Block-Farbton-Streuung (16-px-Bloecke): G4 13,1 / EM30 8,5 Grad gegen K5c 26,4 / K5d 24,2 Grad.
   - Farbfremde Flecken (Farbton > 45 Grad vom Hauptton): G4 3,5 / EM30 5,9 je 10 000 px gegen K5c 16,0 / K5d 17,0.
   - Die zweite Familie stammt aus den K5c-Gruppenzeilen Rumpf Ruecken (+32,8 / +21,9 Grad) und Vorderbeine (+42,5 Grad). Die uebrigen
     8 Zeilen liegen bei -11 bis -46 Grad (Tabelle 3.2). Das ist die Eichung je Region aus FARBE.md V3R4.1.
   - Die K5d-Inseln (grau-blaue Gelenke, heller Ruecken) setzen sich darauf. K5d hat 34 % mehr Flecken als K5c (117,5 gegen 87,5).
   - Die reine Fleckenzahl ist **nicht** trennscharf: G4 104 und EM30 119 je 10 000 px liegen ueber K5c und K5d, wegen der Capcom-Adern.
     Ebenso die C\*-Streuung (G4 13,6, K5c 8,9). Beide Masse sind darum nur Info; geschaerft wurde auf Farbton-Masse (Abschnitt 2).
2. **Ruhe-Massstab** (aus G4 und EM30-Haut, Gegenprobe K5c und K5d ROT in allen bindenden Zeilen unter weissem Licht):
   - Sektoren <= 5
   - farbfremde Flecken <= 7,4 je 10 000 px (Flaeche <= 0,64 %)
   - Farbtonspanne <= 76,8 Grad
   - Block-Farbton <= 16,3 Grad
   - Dazu Szenen-Zeilen Bild 1 und Bild 2 (Abschnitt 2.2).
3. **Materialplan K5e: 10 CLUT-Zeilen** (Port-Texturhoehe 2560).
   - 3 Hautzeilen (G4-Seite 0/1/2), 3 Gelenkzeilen, 1 Fleischzeile, dazu G4-Zeile 0, F1 und Zaehne bytegleich.
   - Je Material EIN Parametersatz auf der G4-Originalrampe: Haut -26,3 Grad / C\* 11,68, Gelenk Haut + 9,1 L\* / C\* 4,8,
     Fleisch Haut + 6,9 L\* / -8 Grad / C\* 18.
   - Indizes = G4, UV = K5c, Geometrie = K5d.
   - Jede der 3828 Flaechen ist genau einem Grundmaterial zugeordnet (`messung/material.json`).
4. **Helligkeit:** Fuer Bild 2 mit Videofilter >= Vorlage (15,37) braucht die eine Mauve-Haut **f = 2,73** (L\* x f) oder die Hebung
   **d = +21,0 L\*** (L\* + d).
   - Bild 1 wird dadurch 2,3- bis 2,5-mal so hell wie die Vorlage (Video 42,7-45,1 gegen 18,3). Das ist der groesste offene Punkt (Offen 1).
   - Mit der Hebung bleiben alle bindenden Ruhe-Zeilen GRUEN (Flecken 47 je 10 000 px).
   - Der Faktor verstaerkt den G4-Texelkontrast mal 2,73: Flecken 114, Bild-1-Block-Farbton 13,3 > 12,25, also ROT. **Empfehlung: Hebung.**
5. **Riegel-Entwurf** in Abschnitt 4. Die Plan-Vorschau besteht alle Wunsch-Zeilen.
   - K5c ist ROT in Ruhe, Herkunft-Farbton, Gelenk, Schwanzspitze, Buckel, Lippe, Krallen, Helligkeit und Geometrie.
   - K5d ist ROT in Ruhe, Herkunft (Inseln/UV), Schwanzspitze-Anteil und Lippe/Unterkiefer.
6. **Melden (nicht entfernt):** Die F1-Leuchtstelle wirkt im helleren, ruhigen Umfeld wie ein aufgemalter Fleck.
   - Sie ist im Mittel 8-10 L\* dunkler als die umgebende Haut, ihr dunkler Rand liegt 23-25 L\* darunter (`messung/v4_plan_faktor_gegen_hebung.png`, "Seite").
   - In K5c lag sie +9,3 L\* ueber dem Umfeld (Offen 2).

## 1. Ursache in Zahlen

**Verfahren** (`skripte/ruhe_mass.py`, `ruhe_lauf.py`; Lab = CIE-Lab D65, OpenCV float):

- **Weisses Licht:** `render.blick` der Familien-Tafel 07: 0,55 + 0,45 max(0, N.L), Umkugel 0,46, 380 px.
  - 4 Winkel: 3/4 vorn 45/15, Seite 0/8, 3/4 hinten -135/20, Gegenseite 180/8.
  - Clips: G4 und EM30 Clip 0, K5c und K5d Clip 2 (Ruhe).
  - Der Kennungs-Render `rl.weiss_id` ist pixelgleich zu `render.blick` (0 Pixel Unterschied, alle 4 Modelle).
- **Figurmaske:** um 2 px erodiert (Videokette 3 px).
  - EM30 ohne Kleidung: Meshes 0-4 und 6, Median-Farbton -66 bis -109 Grad = Jeansblau und Schuhe. Mit Kleidung waere die EM30-Spanne 99-165 Grad.
- **Szenen:** wie `Kandidat.rendern` (Disc-Licht x k, `pruefung/licht.json`). G4 und EM30 Clip 0 an Lage und Gier der K5c.
  Videofilter mit Saaten 15/12345/7, Mittel.
- **(a) Sektoren S5:** 24 Sektoren zu 15 Grad, nur C\* >= 6; gezaehlt werden Sektoren mit >= 5 %.
- **(b) Flecken F:**
  - Klasse = (Sektor oder unbunt, L\*/8). 4-Komponenten von 6 px bis 5 % der Figur.
  - Ring 4 px; Fleck, wenn dL\* >= 8 oder der Farbton um >= 15 Grad abweicht (bzw. bunt gegen unbunt mit dC\* >= 6).
  - **F_fremd**: Fleck mit eigenem Farbton (C\* >= 6) mehr als 45 Grad vom C\*-gewichteten Hauptton der Figur.
    45 = halbe Originalspanne 62/2 + 1 Sektor.
- **(c) Streuung:** C\*-Standardabweichung; Spanne = kleinster Kreisbogen mit 90 % der bunten Pixel.
- **(d) Block-Farbton:** Kreis-Streuung des Mittelfarbtons von 16 x 16-Bloecken (Bloecke mit >= 50 % Figur und Mittel-C\* >= 4, gewichtet mit C\*).

### 1.1 Weisses Licht (Mittel der 4 Winkel; S5 gepoolt)

| Mass | G4 | EM30 Haut | K5c | K5d | Vorschau Hebung | Vorschau Faktor |
|---|---|---|---|---|---|---|
| Figurpixel (Mittel) | 33 078 | 17 468 | 13 531 | 15 781 | 15 781 | 15 781 |
| (a) S5 gepoolt / je Winkel | 4 / 4,0 | 4 / 4,0 | **6** / 5,75 | **6** / 5,5 | 3 / 3,0 | 3 / 3,0 |
| Farbton-Hauptgipfel | 15-30 (36 / 31 %) | 15 (51 %) | **315 (37 %) + 30 (14 %)** | **315-345 (68 %) + 30 (12 %)** | 315 | 315 |
| (b) F alle | 104,5 | 118,9 | 87,5 | 117,5 | 47,0 | 114,4 |
| (b) F_fremd / Flaeche | 3,5 / 0,36 % | 5,9 / 0,51 % | **16,0 / 3,74 %** | **17,0 / 3,82 %** | 0,0 / 0 | 0,0 / 0 |
| (b) graue helle Inseln in bunter Haut | 2,9 | 6,5 | 28,3 | 16,9 | | |
| (c) C\*-Std | 13,55 | 7,60 | 8,92 | 8,18 | 3,21 | 3,41 |
| (c) Farbtonspanne 90 % | 61,8 | 60,2 | **88,1** | **88,3** | 22,3 | 22,8 |
| (d) Block-Farbton | 13,1 | 8,5 | **26,4** | **24,2** | 5,4 | 5,4 |
| L\* Mittel | 19,1 | 30,3 | 8,8 | 14,4 | 24,3 | 21,2 |

Je Winkel (S5, F_fremd, Spanne, Block):
- K5c: 3/4 vorn 6, 25,3, 90,3, 25,6; Seite 5, 16,7, 83,7, 20,2; hinten 6, 12,5, 89,5, 29,9; Gegenseite 6, 9,5, 89,0, 30,0.
- K5d: 5, 22,5, 89,4, 26,9; 5, 11,9, 83,4, 21,3; 6, 14,8, 92,5, 24,3; 6, 18,6, 88,0, 24,1.
- G4: max S5 5, F_fremd 4,4, Spanne 73,3, Block 16,2.
- EM30: max S5 4, F_fremd 7,8, Spanne 70,4, Block 13,2.

Quelle: `messung/ruhe_mass.json`, Vorschau `messung/vorschau.json` / `vorschau_hebung.json`.

### 1.2 Szenenkette ohne Filter (Disc-Licht x k), ganze Figur (EM30 ohne Kleidung)

Bild 1 = 5080, Bild 2 = 5090 Schnitt 11.

| Mass | G4 | EM30 | K5c | K5d | Hebung | Faktor | Vorlage (Video, Info) |
|---|---|---|---|---|---|---|---|
| Bild 1 S5 | 3 | 4 | **6** | **6** | 3 | 4 | 5 |
| Bild 1 Spanne | 43,9 | 42,3 | **70,1** | **83,1** | 39,4 | 55,2 | 64,0 |
| Bild 1 Block-Farbton | 9,8 | 7,0 | **17,2** | **19,0** | 9,7 | **13,3** | 14,9 |
| Bild 1 F_fremd | 0,0 | 1,7 | 0,9 | 2,4 | 3,0 | 5,5 | 5,1 |
| Bild 2 S5 | 4 | 3 | **7** | 3 | 3 | 3 | 5 |
| Bild 2 Spanne | 45,8 | 40,4 | **84,8** | 60,7 | 52,0 | 50,5 | 48,7 |
| Bild 2 Block-Farbton | 8,7 | 8,3 | **21,6** | **17,8** | 6,7 | 7,8 | 13,8 |
| Bild 2 F_fremd | 0,0 | 0,0 | 4,7 | 3,2 | 2,7 | 2,7 | 0,0 |

- Mit Videofilter (3 Saaten) glaettet der Filter fast alles: Spanne 23-46 Grad bei allen Modellen, F_fremd 0-1,4.
  Diese Zeilen sind nur Info (`ruhe_mass.json` "video").
- Woraus die farbfremden Szenenflecken bestehen (`skripte`, Karte Wert 4 je Material; Pixel):
  - Bild 2: Plan 214 px = Zaehne (unveraendert seit E30). K5c: Buckel 199, Haut 107, Gelenk 59. K5d: Zaehne 214, Haut 80.
  - Bild 1: Plan Hebung 136 px = Gelenke. K5c: Haut 16, Schwanzspitze 15. K5d: Haut 497, Spitze 139.
  - Ohne Sondermaterialien (Zahn, F1, Auge, Maulinneres; `vorschau_szene_ohne_sonder.json`): Plan Hebung Bild 2 F_fremd 0,0, Bild 1 3,3 (Gelenke).
  - Die Gelenke werden im 5080-Licht farbfremd, weil das Szenenlicht wenig bunte Flaechen kuehl faerbt. Mit C\* 2 statt 4,8 steigt es auf
    11,7 je 10 000 px. Die Vorlage zeigt die Gelenke ebenfalls kuehl (Messblatt V4F 2.3).

**Kacheln** (roh, kein Gamma):

- `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/messung/r1_weiss_rundum.png`:
  G4 | EM30 | K5c | K5d, 4 Winkel, Zahlen je Kachel.
- `.../ruhe/messung/r2_weiss_flecken.png`: Fleckkarte (cyan L\*, magenta Farbton, gelb beides, rot farbfremd).
- `.../ruhe/messung/r3_weiss_farbtonkarte.png`: Farbtonsektor je Pixel. Zeigt die zwei Familien (violetter Rumpf, braune Glieder) bei K5c/K5d.
- `.../ruhe/messung/r6_farbton_histogramm.png`: Sektor-Haeufigkeit gepoolt.
- `.../ruhe/messung/r4_szene_bild1.png`, `r5_szene_bild2.png`: Vorlage | G4 | EM30 | K5c | K5d, ohne Filter / Videofilter.
- `.../ruhe/messung/r7_szene_bild1_nah.png`, `r8_szene_bild2_nah.png`: dieselben Szenen nah (nearest).

## 2. Ruhe-Massstab (Schwellen aus G4 und EM30-Haut, Gegenprobe)

### 2.1 Weisses Licht (bindend)

| Zeile | Regel | Schwelle | G4 / EM30 | K5c | K5d | Status |
|---|---|---|---|---|---|---|
| R1 Sektoren | S5 gepoolt <= max + 1 | <= 5 | 4 / 4 | 6 ROT | 6 ROT | bindend |
| R2 farbfremde Flecken | F_fremd <= 1,25 x max | <= 7,4 je 10 000 px | 3,5 / 5,9 | 16,0 ROT | 17,0 ROT | bindend (geschaerft) |
| R2b Flaeche farbfremd | <= 1,25 x max | <= 0,64 % | 0,36 / 0,51 | 3,74 ROT | 3,82 ROT | bindend |
| R3 Farbtonspanne | <= max + 1 Sektor | <= 76,8 Grad | 61,8 / 60,2 | 88,1 ROT | 88,3 ROT | bindend |
| R4 Block-Farbton | <= 1,25 x max | <= 16,3 Grad | 13,1 / 8,5 | 26,4 ROT | 24,2 ROT | bindend |
| R5 Flecken alle | <= 1,25 x max | <= 148,6 | 104,5 / 118,9 | 87,5 gruen | 117,5 gruen | **nicht trennscharf**: Decke, Info |
| R6 C\*-Std | <= 1,25 x max | <= 16,9 | 13,6 / 7,6 | 8,9 gruen | 8,2 gruen | **nicht trennscharf**: Info |

**Schaerfung:** Die Auftragsdefinition (b) misst jede Abweichung im 4-px-Ring. Die Originale haben viel Capcom-Zeichnung (G4: rote Adern
in schwarzer Haut), also viele solche Flecken. Getestet wurden auch Median 5 x 5, Gauss 1,5 und halbe Aufloesung
(`scratchpad/k5e/var.txt`): Keine Variante trennt K5c oder K5d von den Originalen. Getrennt wird erst durch die Farbe des Flecks
relativ zur Figur (F_fremd). Das ist der Nutzersatz "Alles bunt": Ein gemalter Fleck hat eine andere Farbfamilie als die Haut.

### 2.2 Szenen ohne Filter (ganze Figur wie 1.2; die Riegel 4.1 S1-S4 rechnen zusaetzlich ohne Zahn/F1/Auge/Maulinneres, `vorschau_szene_ohne_sonder.json`)

| Zeile | Schwelle | K5c | K5d | Plan Hebung | Status |
|---|---|---|---|---|---|
| S1 Bild 1 S5 <= max(3,4) + 1 | <= 5 | 6 ROT | 6 ROT | 3 | bindend |
| S2 Bild 1 Spanne <= 43,9 + 15 | <= 58,9 | 70,1 ROT | 83,1 ROT | 39,4 | bindend |
| S3 Bild 1 Block <= 1,25 x 9,8 | <= 12,25 | 17,2 ROT | 19,0 ROT | 9,7 (Faktor 13,3 ROT) | bindend |
| S4 Bild 2 Block <= 1,25 x 8,7 | <= 10,9 | 21,6 ROT | 17,8 ROT | 6,7 | bindend |
| S5 Bild 2 Spanne <= 45,8 + 15 | <= 60,8 | 84,8 ROT | 60,7 (gruen, knapp) | 52,0 | Decke (nur K5c rot) |
| S6 Bild 2 S5 | <= 5 | 7 ROT | 3 | 3 | Decke |
| S7 F_fremd Szene | Originale 0-1,7 | Bild 1 1,0, Bild 2 4,7 | Bild 1 2,7, Bild 2 0,5 | Bild 1 3,3, Bild 2 0,0 | **Info**: Schwelle 0 bzw. 2,1 ist zu eng, Gegenprobe nicht durchgehend rot, Gelenk-Wunsch (kuehl im 5080-Licht) kollidiert |

## 3. Materialplan K5e

### 3.1 Grundlage (Zahlen aus `skripte/ruhe_material.py` -> `messung/material.json`)

- Das K5c-TIM hat auf den Seiten 0-2 die G4-Indizes bytegleich, die CLUT-Zeilen 0-2 sind die G4-Rampen bytegleich (FARBE.md V3.1).
- Jede K5c-Gruppenzeile ist aus der G4-Zeile ihrer Seite abgeleitet (`farbe/K5c_farbe_v3/bau.json` zeilen.basis; Nummern nach E.1):
  - G4-Seite 1: Zeilen 2, 11, 12, 13, 14, 15
  - G4-Seite 2: Zeilen 3, 8, 10
  - G4-Seite 0: Zeilen 4, 9
  - Sonderzeilen: F1 = Seite 2, Band = Seite 0, Zaehne = Seite 0
- Die Seite 3 traegt G4-Indizes der Quellzeile: Band aus G4-F7 ueber Zeile 0, F1 ueber Zeile 2.
- Klassen je G4-Eintrag (`kf_g4.r4_eintraege`, Texelzugriffe der K5c-Flaechen):

| G4-Seite | Haut (w 0, L\* < 52,4): Eintraege, Zugriffe, L\* p10/p50/p90, C\* | Ader (Rotgewicht > 0) | hell (L\* >= 52,4) |
|---|---|---|---|
| 0 | 15, 26 200, 2,2/5,1/8,5, 4,3 | 113, 15 647, Lab (39,3/24,8/20,6) | 1, 68 |
| 1 | 45, 189 121, 8,5/12,7/17,4, 3,5 | 121, 92 500, (22,4/29,9/16,3) | 18, 2 715 (Krallenspitzen) |
| 2 | 40, 26 569, 4,9/12,7/24,1, 1,2 | 13, 10 196, (37,9/33,1/-15,9) | 17, 10 559 (Meshes 0, 7-9, 24-27) |

### 3.2 Die Ursache in den K5c-Zeilen (Haut-Eintraege, texelgewichtet, Albedo)

| Zeile | Gruppe | G4-Seite | Texel | L\* | C\* | Farbton |
|---|---|---|---|---|---|---|
| 2 | Schwanz | 1 | 44 540 | 13,1 | 6,3 | -23,9 |
| 3 | Hinterbeine | 2 | 12 626 | 10,7 | 8,0 | -23,4 |
| 4 | Rumpf Ruecken | 0 | 5 854 | 5,6 | 6,5 | **+32,8** |
| 8 | Rumpf Ruecken | 2 | 2 875 | 8,4 | 0,2 | **+21,9** |
| 9 | Rumpf Flanke | 0 | 19 542 | 4,7 | 21,7 | -40,6 |
| 10 | Rumpf Flanke | 2 | 11 068 | 6,8 | 15,7 | -46,2 |
| 11 | Vorderbein r B1 | 1 | 12 278 | 11,5 | 4,6 | -19,2 |
| 12 | Brust | 1 | 19 175 | 16,0 | 20,5 | -32,8 |
| 13 | Hals Bauch | 1 | 39 898 | 17,9 | 8,3 | -24,9 |
| 14 | Kopf | 1 | 22 421 | 13,1 | 16,1 | -11,2 |
| 15 | Vorderbeine | 1 | 46 267 | 18,1 | 16,9 | **+42,5** |

- 11 Zeilen haben 11 verschiedene Paare (Farbton, C\*) mit einer Spanne von 88,7 Grad. Das ist die Flickenhaut.
- **Hauptfamilie (E30):** 8 Zeilen mit Farbton < 0, 181 548 Texel, texelgewichtet **-26,3 Grad, C\* 11,68, L\* 12,9**.
- Zweite Familie: Zeilen 4, 8, 15, 54 996 Texel.

### 3.3 Zeilenplan (10 Zeilen, <= 16; Port-Texturhoehe 2560 <= 4096)

| Zeile | Inhalt | Quelle | Flaechen |
|---|---|---|---|
| 0 | Auge, Maulinneres | G4-Zeile 0 bytegleich (= K5c-Zeile 0) | 64 + 35 |
| 1 | Haut, G4-Seite 0 (dazu die 25 Band-Flaechen auf Seite 3, Quellzeile 0) | T_Haut(G4-Zeile 0) | 299 + 25 |
| 2 | Haut, G4-Seite 1 (inkl. 252 Krallen) | T_Haut(G4-Zeile 1) | 1934 |
| 3 | Haut, G4-Seite 2 | T_Haut(G4-Zeile 2) | 579 |
| 4 / 5 / 6 | Gelenk, G4-Seite 0 / 1 / 2 | T_Gelenk(G4-Zeile 0/1/2) | 6 / 360 / 314 |
| 7 | Fleisch, G4-Seite 1 (Schwanzspitze + Buckel) | T_Fleisch(G4-Zeile 1) | 80 + 12 |
| 8 | F1-Leuchtstelle | K5c-Zeile 5 bytegleich (E30) | 6 |
| 9 | Zaehne | K5c-Zeile 7 bytegleich | 114 |

- Summe 3828 = alle Flaechen, 0 doppelt (`material.json` kontrolle).
- Je Flaeche aendert sich nur das CLUT-Wort. UV, Seite und TIM-Pixel bleiben wie im Asset K5c; die Geometrie ist K5d.
- Die K5d-Inseln entfallen damit von selbst. Die Flaechen-Eckindizes von K5c und K5d sind gleich, gemessen in 32/32 Meshes.

### 3.4 Materialien: Flaechen, Quelle, EINE Transformation

Flaechenlisten vollstaendig in `messung/material.json` -> `material.<Name>.liste` (Schluessel mesh|viereck|nr, Zaehlung wie `form_emd.lese_emd`).

| Material | Flaechen (Meshes) | Quelle (G4-Zeile) | Transformation (ein Satz fuer alle Flaechen) | Begruendung (nur Farbrichtung und Wortlaut) |
|---|---|---|---|---|
| **Haut** | 2837 (alle uebrigen; Krallen 252, Lippensaum 140, Maul-Band 25 und Unterkiefer gehoeren dazu) | 0/1/2 je Seite | Haut-Eintraege: L\*' = L\* + d (Empfehlung, d = 21,0) bzw. L\*' = f x L\* (f = 2,73); Farbton -26,3 Grad; C\* 11,68 (weich an den sRGB-Rand: Cmax x tanh(C/Cmax), `kf_g4.r4_cmax`). Helle Eintraege: Haut-Median' + 0,1 x (L\* - 52,4), gleicher Ton/C\*. | Nutzer: "einheitlich grau/blau" (K5c-Haut, die er sieht) und "Sieht doch gut aus" (E30) -> Ton und Buntheit der Hauptfamilie der gelobten K5c-Haut, eine fuer alle. Die zweite Familie (+22..+43 Grad) faellt weg: "Alles bunt". Helle G4-Eintraege (Horn, Spitzen) liegen auf K5c-UV verstreut; in der Hautmitte erzeugen sie keine Sprenkel. |
| **Adern** | Eintraege der Hautzeilen 1-3 | Ader-Klasse der G4-Rampe | L\*' rangtreu linear auf [Haut-p10', 0,74 x Haut-p50'], C\* = 0,61 x 11,68, Farbton wie Haut | RV6 / Nutzer 11-07: Adern "nie heller und nie roeter als die Haut"; q 0,74 und kappa 0,61 = Median der 9 E30-Gruppensaetze (`v3_daten/r4_par.json`), ein Satz statt neun |
| **Gelenk** | 680 = 17 Gelenkkoerper x 40 (Huefte l/r, Muffe Huefte l/r, Knie l/r, Fuss l/r, Schulter l/r, Muffe Schulter r, Ellbogen l/r, Hand l/r, Muffe Hand l/r; Meshes 0, 8-13, 16-18, 24, 26) | 0/1/2 | wie Haut, L\*' + 9,1; C\* 4,8; Farbton -26,3 (Adern und helle Eintraege wie Haut) | Nutzer: Verbindungsstuecke "weisslich". dL 9,1 = K5d Gelenk- gegen Nachbar-Albedo 28,5 - 19,4, zu K5d sagte der Nutzer "ja, besser". C\* 4,8 = K4-Albedo an den Gelenkflecken (a\* 1,7, b\* -4,5; "vorher da"). Farbton der Haut: Ein Lauf mit dem K4-Ton -69 Grad machte die Gelenke in Bild 1 farbfremd (F_fremd 11,3, `vorschau_lauf1_gelenk_minus69.json`). Die Schwanzgelenke sind nicht dabei (kein Nutzersatz). |
| **Fleisch** | Schwanzspitze = Mesh 6 ganz, 80 Flaechen; Buckel = Mesh 10, 12 Vierecke qf 54, 55, 65, 66, 67, 77, 78, 79, 80, 89, 90, 91 | 1 | wie Haut, L\*' + 6,9; C\* 18; Farbton -8 Grad | Nutzer: Spitze "fleisch roetlich", Buckel "rote fleischige Stelle". Ton -8 Grad = F1-Leuchtstelle (E30 gelobt, Lab 48,6/21,6/-3,0). C\* 18 = Mittel aus K4-Schwanzspitze und K4-Buckel F02 (11,6 / 24,9). dL 6,9 = K4-Spitze gegen K4-Schwanz 20,5 - 13,6 ("vorher da"). |
| **Kralle** | 252 (Meshes 14, 15, 19, 20, 28-31) = Haut | 1 | Haut | Nutzer: "wo kommt ... dieses rot her"; Vorlage Kralle = Hand dE 4. Hand (13/18) und Krallen liegen beide auf G4-Seite 1, also dieselbe Zeile 2. |
| **Lippe + Unterkiefer aussen** | Lippensaum 140 (Zeilen 0: 56, 6: 23, 14: 61) + Maul-Band 25 = Haut | 0/1 | Haut, das Band entfaellt | Nutzer: Lippe "durchweg grau/blau", Unterkiefer "nicht leuchten sondern dunkel/grau" |
| **Zahn** | 114 (Meshes 22, 23) | K5c-Zeile 7 bytegleich | keine | Zaehne L\* 74,5 "wie gewollt" (Messblatt V4F 1.3) |
| **Auge** | 64 (Mesh 22) | G4-Zeile 0 bytegleich | keine | A9 dunkel (V3.1) |
| **Maulinneres** | 35 (Mesh 22: 24, Mesh 23: 11) = Zeile-0-Flaechen ohne Saum und Augen | G4-Zeile 0 bytegleich | keine | G4-F2 wie bisher, E30 (Offen 5) |
| **F1** | 6 (Mesh 0, qf 7, 8, 19, 20, 31, 32) | K5c-Zeile 5 bytegleich | keine | E30 ausdruecklich gelobt (Offen 2) |

- **Buckel-Auswahl:** Mesh-10-Flaechen mit >= 10 px in den Vorlagenflecken B2-F01/F02 (Kennungs-Render K5c Bild 2). Das sind 12 Flaechen,
  1326 von 1380 Fleckpixeln, 1 Komponente ueber gemeinsame Ecken. Ausgelassen: Mesh 21 qf 26 (17 px) und Mesh 22 qf 205 (37 px).
- **Aus dem Video kommen nur:** die Orte (Buckel-Flecken, Schwanzspitze, Gelenkstellen) und die Bild-2-Helligkeit (3.5).
  Farbtoene und Buntheit stammen aus gelobten Staenden (E30, K4 "vorher da", K5d-Gelenke "besser").

### 3.5 Helligkeit (Vorschau im Speicher, `skripte/ruhe_vorschau.py`, `ruhe_hebung.py`)

**Pixelmenge:** wie Messblatt V4F 3.1, also Nutzer-Teilmasken & K5c-Modell & ohne Weisskappe. Bild 1 29 706 px, Bild 2 42 350 px.
Gegenprobe: Vorlage Bild 2 Video 15,37 = V4F, K5c 10,92 = V4F 10,9.

| Hautabbildung | Bild 1 Video / T / ohne | Bild 2 Video / T / ohne |
|---|---|---|
| Vorlage | 18,29 / 20,32 / 7,62 | 15,37 / 24,88 / 7,33 |
| K5c / K5d | 16,41 / - / 5,12; 27,63 / - / 9,67 | 10,92 / - / 4,86; 15,46 / - / 7,74 |
| f 1,0 | 18,66 / 20,00 / 5,97 | 8,13 / 14,53 / 3,13 |
| f 1,5 | 26,01 / 26,67 / 8,94 | 9,55 / 17,59 / 4,40 |
| f 2,0 | 33,45 / 32,81 / 11,64 | 11,54 / 21,26 / 5,66 |
| f 2,5 | 41,40 / 39,21 / 14,59 | 14,01 / 25,49 / 7,16 |
| **f 2,730** (Bisektion, Bild 2 Video = Vorlage) | 45,05 / 42,16 / 16,08 | **15,37** / 27,81 / 7,96 |
| **d +21,016** (Hebung, Bisektion) | 42,67 / 40,44 / 16,19 | **15,37** / 28,48 / 8,58 |

- Bei gleicher Albedo ist die eine Mauve-Haut in Bild 2 dunkler als K5c (f 1,0: 8,13 gegen 10,92).
- Grund: Das Feuerlicht in 5090 ist orange. K5c hatte dort warme Zeilen (Vorderbeine +42,5 Grad, Hauthelligkeit a 1,3-1,6 nach V3R4.1),
  und warme Albedo reflektiert mehr davon.
- Eine Haut mit einem Farbton und einem Faktor kann deshalb nicht beide Vorlagen treffen: Bild 1 braucht f ~1,0, Bild 2 f 2,73.

### 3.6 Vorschau-Kacheln (Plan, kein Asset)

- `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/messung/v1_plan_weiss_rundum.png`:
  G4 | EM30 | K5c | K5d | K5e-Plan (Faktor), weisses Licht, 4 Winkel.
- `.../ruhe/messung/v2_plan_szene_bild1.png`, `v3_plan_szene_bild2.png`: Vorlage | K5c | K5d | Plan, ohne Filter / Videofilter.
- `.../ruhe/messung/v4_plan_faktor_gegen_hebung.png`: Faktor (oben) gegen Hebung (unten), weisses Licht.
  - Die Hebung ist sichtbar ruhiger: Flecken 47 gegen 114 je 10 000 px.
  - Die F1-Stelle zeigt sich in "Seite" als dunkle Kerbe.

## 4. Riegel-Entwurf fuer den Bauer

Gegenprobe: K5c = Spiel-Asset (stufe3/basis_k5c), K5d = feinschliff/K5d. "Plan" = Vorschau Hebung / Faktor. Zahlen aus `messung/*.json`.

### 4.1 Ruhe (weisses Licht wie Tafel 07, 4 Winkel, Clip 2; Szene ohne Filter)

| Riegel | Soll | Plan H / F | K5c | K5d |
|---|---|---|---|---|
| R1 Sektoren gepoolt | <= 5 | 3 / 3 | 6 ROT | 6 ROT |
| R2 farbfremde Flecken | <= 7,4 je 10 000 px und <= 0,64 % | 0 / 0 | 16,0 / 3,74 ROT | 17,0 / 3,82 ROT |
| R3 Farbtonspanne 90 % | <= 76,8 Grad | 22,3 / 22,8 | 88,1 ROT | 88,3 ROT |
| R4 Block-Farbton | <= 16,3 Grad | 5,4 / 5,4 | 26,4 ROT | 24,2 ROT |
| S1-S4 Szene (Figur ohne Zahn/F1/Auge/Maulinneres; K5c ganze Figur: S5 6, Spanne 70,1 ROT) | B1 S5 <= 5, Spanne <= 58,9, Block <= 12,25; B2 Block <= 10,9 | 3, 40,3, 10,0; 5,3 / Faktor B1 Block 13,8 ROT | 5 gruen (Haut ohne Sonder; ganze Figur 6 ROT), 55,2 gruen, 16,4 ROT; 22,8 ROT | 6 ROT, 87,0 ROT, 20,1 ROT; 16,7 ROT |
| Info | Flecken alle <= 148,6; C\*-Std <= 16,9; F_fremd Szene | 47 / 114; 3,2 / 3,4 | 87,5; 8,9 | 117,5; 8,2 |

### 4.2 Herkunft

| Riegel | Soll | Gegenprobe |
|---|---|---|
| H1 Texel | TIM-Pixel (512 x 256, Seiten 0-3) bytegleich K5c-Asset (TIM 7187ff09..., nur die CLUT aendert sich); keine neue Insel | K5d: 41 510 Texel neu, 72 Inseln (FARBE.md F.2) ROT |
| H2 UV | UV, Seite und Eckreihenfolge je Flaeche = K5c, 3828/3828; nur das CLUT-Wort darf abweichen | K5d: 1044 Flaechen mit neuer UV ROT |
| H3 Ein Satz je Material | Jede Zeile 1-7 wird aus der G4-Zeile ihrer Seite mit dem Materialsatz (3.4) neu berechnet = Datei, 0 Abweichung; Zeilen 0/8/9 bytegleich K5c 0/5/7 | K5c: 9 Gruppensaetze (V3R4.1) ROT; K5d: Fleck-L\* je Insel (`kf_k5d_eichen.py`) ROT |
| H4 Ein Farbton je Material | genutzte Eintraege mit C\* >= 3 je Material: Farbtonspanne <= 15 Grad (Haut/Ader/Gelenk -26,3, Fleisch -8) | K5c-Hautzeilen -46,2..+42,5 = 88,7 Grad ROT; K5d (gleiche Zeilen + Inseln) ROT |
| H5 Rangfolge | je G4-Zeile und Klasse (Haut, Ader, hell) 0 Vertauschungen in L\* gegen G4; Adern unter der Haut (RV6, alle Regionen dL < 0) | K5c gruen (H2 V3R4), nur Herkunftsschutz |

### 4.3 Wuensche (Albedo = Texel des UV-Polygons ueber die Zeile; weisses Licht wie 4.1)

| Riegel | Soll | Plan H / F | K5c | K5d |
|---|---|---|---|---|
| W1 Gelenk heller als das Nachbarglied (weisses Licht, Ring 6 px auf den Gliedflaechen derselben Meshes) | dL\* >= 5 in jedem Winkel, Mittel >= 6 | min 6,22 / Mittel 7,44; F 5,62 / 8,10 | -0,35 / 0,55 ROT | 5,92 / 8,94 |
| W1b Gelenk weisslich | Albedo-Median C\* <= 8 und L\* >= Nachbar + 6 | 3,7, +9,3; F 5,6, +9,6 | 13,6, 0,0 ROT | 5,4, +10,3 |
| W2 Schwanzspitze (Mesh 6) | Albedo-Fleischanteil (a\* >= 12, C\* >= 15) >= 50 % und dL\* gegen Schwanz 1-5 >= +5; dazu `riegel_k5d (b)` in der Bild-1-Kette (Anteil 25..50 %, Vorlage 37,5) | 78,0 %, +6,3; F 62,7 %, +8,4 | 6,8 %, 0 ROT | 21,2 % ROT, +9,6 |
| W3 Buckel | 12 Flaechen, 1 Komponente; Albedo-Farbton -8 +- 15 Grad; Fleischanteil >= 40 %; dL\* gegen die Mesh-10-Haut >= +3; dazu `riegel_k5d (e)` | -8,4, 52,4 %, +3,3; F -9,3, 45,0 %, +3,6 | -33,2 ROT, 44,8 %, -1,4 ROT | -18,9, 92,5 %, +11,3 |
| W4 Lippe + Unterkiefer aussen | Lippensaum (140) + Band (25): Albedo-Rotklasse (a\* > 15, C\* > 15) = 0 %; kein Leuchten: L\*-p95 <= Haut-L\*-p95 | 0 %, 37,1 <= 41,3; F 0 %, 44,7 <= 57,6 | 54,4 % ROT, 58,1 > 36,0 ROT | 3,7 % ROT, 43,1 > 39,1 ROT |
| W5 Krallen = Hand | Albedo-Median dE76 Krallen gegen Haende 13/18 <= 6; Rotklasse 0 % | 0,0 / 0 %; F 0,31 / 0 % | 30,6 / 47,2 % ROT | 3,3 / 0 % |
| W6 Helligkeit | Bild 2 Video >= Vorlage 15,37 und ohne Filter >= 7,33 (Pixelmenge V4F 3.1) | 15,37 / 8,58; F 15,37 / 7,96 | 10,92 / 4,86 ROT | 15,46 / 7,74 |
| W7 F1 | Zeile 8 = K5c-Zeile 5 bytegleich, 6 Flaechen/UV gleich | ja | ja | ja (L-Riegel K5d) |

### 4.4 Technik

| Riegel | Soll | Gegenprobe |
|---|---|---|
| T1 Zeilen | <= 16 CLUT-Zeilen (Plan 10), Port-Texturhoehe Zeilen x 256 <= 4096 (2560) | K5c / K5d 16 gruen |
| T2 gueltig | jede Flaeche liest Zeile 0..9; kein Eintrag wird 0x0000, wo G4 != 0 (Schwarz = 0x8000 wie `kf_g4`) | |
| T3 Geometrie = K5d | Ecken/Normalen gleich K5d (Krallen 19/20/28/29 eingezogen), Eckindizes = K5c | K5c: Krallen nicht eingezogen ROT |
| T4 Knochen/Clips | EDD/EMR/Morph und 28 Clips byte-gleich Spiel-Asset K5c (32 Meshes, 32 Knochen) | K5d gruen |
| T5 Einbau | Pins form_j_posen.h (MD1/TIM CRC) neu wie E.3, `riegel_einbau.py` E-Z1/E-Z2 | |

## 5. Tafeln

Alle Tafeln liegen unter `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/messung/`,
alle roh, kein Gamma: r1_weiss_rundum.png, r2_weiss_flecken.png, r3_weiss_farbtonkarte.png, r4_szene_bild1.png, r5_szene_bild2.png,
r6_farbton_histogramm.png, r7_szene_bild1_nah.png, r8_szene_bild2_nah.png, v1_plan_weiss_rundum.png, v2_plan_szene_bild1.png,
v3_plan_szene_bild2.png, v4_plan_faktor_gegen_hebung.png.

**Daten:** ruhe_mass.json, material.json, vorschau.json, vorschau_hebung.json, vorschau_szene_ohne_sonder.json,
vorschau_lauf1_gelenk_minus69.json, vorschau_hebung_lauf1_gelenk_minus69.json.

**Skripte** in `ruhe/skripte/` (Python ohne -I, Aufruf aus dem Skriptordner):

| Skript | Inhalt |
|---|---|
| rl.py | Lader, weisses Licht, Kennung, Szene |
| ruhe_mass.py | Masse |
| ruhe_lauf.py | Teil 1 |
| ruhe_material.py | Teil 2 |
| ruhe_vorschau.py | Plan im Speicher, Faktor |
| ruhe_hebung.py | Hebung |
| ruhe_wunsch.py | Wunsch-Masse |

## 6. Offen

1. **Helligkeit Bild 1 gegen Bild 2 (Entscheid Orchestrator/Nutzer):**
   - Mit EINER Haut-Helligkeit fuer Bild 2 ist Bild 1 2,3- bis 2,5-mal so hell wie die Vorlage: Video 42,7 / 45,1 gegen 18,3, ohne Filter 16,2 gegen 7,6.
   - Moeglich sind:
     - (a) so, Auftrag: "Bild 1 darf heller sein";
     - (b) ein Mittelwert, z. B. f 1,5 / d ~ +8: Bild 1 26,0 / Bild 2 9,6, also 62 % der Vorlage;
     - (c) Bild 2 waermer ueber das Szenenlicht statt ueber die Haut. Das waere kein Texturthema.
   - Eine Eichung je Region, wie K5d sie hatte, ist verboten.
   - Im echten Spielfenster ist das nicht gemessen; das ist Bauer-Pflicht, gdigrab.
2. **F1-Leuchtstelle im neuen Umfeld wirkt wie ein aufgemalter Fleck (nur gemeldet):**
   - weisses Licht "Seite": F1 -8,3 (Faktor) / -10,4 (Hebung) L\* unter dem Ring, Rand-p10 -22,7 / -24,8. K5c: +9,3, K5d: +8,1.
   - Ursache: Die F1-Zeile (G4-F11-Haut als Rand, L\* ~5-13) bleibt bytegleich, die Haut daneben wird gehoben.
   - Moeglicher Weg, falls freigegeben: Nur die Haut-Eintraege der F1-Zeile folgen der Hauttransformation, die Fleisch-Eintraege
     (die eigentliche Leuchtstelle) bleiben bytegleich.
3. **Faktor gegen Hebung:** Der Auftrag nennt einen "Faktor". f x L\* verstaerkt den G4-Texelkontrast mal 2,73: Flecken 114 statt 47 je
   10 000 px, Szene Bild 1 Block-Farbton 13,3 > 12,25, also ROT. Die Hebung L\* + d haelt den Capcom-Kontrast 1:1 und besteht alle bindenden
   Ruhe-Riegel. Empfehlung: Hebung (ebenfalls ein einziger globaler Parameter der Haut).
4. **Gelenke im 5080-Licht farbfremd (F_fremd Szene Bild 1 3,0-3,3, Schwelle aus den Originalen 2,1):** Das ist der Gelenk-Wunsch
   (Vorlage kuehl-weisslich) im Szenenlicht. Darum ist F_fremd in der Szene nur Info; bindend ist F_fremd unter weissem Licht (Plan 0).
5. **Maulinneres:** G4-Zeile 0 (35 Flaechen, E30) zeigt im Ruhe-Clip im Maulspalt eine rote Linie (v1/v4, "Seite"). Das ist keine Lippe.
   Falls der Nutzer sie als Lippe liest: Maulinneres in die Hautzeile 1.
6. **"grau-blau":** Die Haut liegt bei -26,3 Grad, unter weissem Licht mauve wie die K5c-Mehrheit. Ein bluerer Ton waere eine neue
   Farbrichtung ohne Nutzersatz und ist darum nicht geplant.
7. Kettenriegel `riegel_k5d.py` (b) Schwanzspitze und (e) Buckel am gebauten K5e laufen lassen. Die Vorschau prueft nur Albedo und
   weisses Licht.
