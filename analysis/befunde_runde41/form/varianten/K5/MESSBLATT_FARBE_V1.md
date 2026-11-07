# Messblatt Farbe und Struktur V1 (Riesenform K5c, Pruefer, 2026-11-06)

Anlass: Nutzer 2026-11-06 zu `rundum_bild1_K5c.png`: "eher c. Aber es passt auch von den Farben usw. her ueberhaupt nicht und von der Struktur ...
Es passt auch nicht zum Rest des Spieles ... sieht eher aus wie ein Pokemon". Grundlage: Leser A (`FARBE_LESER_A.md`, Spiel-Stil) und
Leser B (`FARBE_LESER_B.md`, Vorlage/Nutzer), alle entscheidenden Zahlen hier mit eigenem Code neu gerechnet.

Kein RE-Wert: K5c ist ein neues Modell (PORT-WAHL, NUTZER_ENTSCHEIDE E5/E19/E20). Jede Grenze unten traegt ihre Quelle: gemessenes Spielmodell
(Datei, Zone, Zahl) oder gemessene Screenshot-Zone. Seiten: (W) = aus Sicht des Wesens. Farben RGB 0..255 und CIE-Lab D65 (OpenCV, float RGB/255).

Pruefer-Werkzeug (versioniert): `analysis/befunde_runde41/form/varianten/K5/farbe_lesung/P_skripte/`
- `p_lade.py` Lader (EM30, RE2-G4/G5 ueber `re2_ems_toc.inc`, RE1.5-Typen, Varianten) und Texelsammlung: Texelzentrum (x+0,5, y+0,5) im UV-Dreieck, CLUT-Zeile der Flaeche.
- `p_messen.py` -> `p_messung.json` (Stil je Zone), `p_zonen.py` -> `p_zonen.json` (Screenshots und K5c durch dieselbe Kette), `p_extra.py` -> `p_extra.json`,
  `p_auge.py`, `p_chroma_korn.py`, **`p_riegel.py <Variantenordner>` -> `riegel_<name>.json` (alle Riegel aus Abschnitt 6, GRUEN/ROT)**.
- `boden_fuss_links/rechts.png` = Bodenpixel des Pruefers (189 + 160 px, aus scratchpad `leser/pruefer/boden_fuss_*.npy`).

## 0. Ergebnis in Kuerze

1. K5c ist **unbunt**: Koerper C\* 4,9 gegen 9,3-19,8 bei allen Spielkreaturen (EM30-Haut 14,3). Sechs von acht Palettenclustern sind Grau.
2. K5c hat **kein Fleischrot**: 0,5 % der Koerpertexel, und dieses Rot ist Magenta (Farbton -10 Grad); die Familie hat 11,5-38 % warmes Rot (Farbton 25-48 Grad).
3. **Je Teil fast eine Farbe**: Farbstreuung a\*b\* je Teil 4,0 gegen 7,4-10,7 (Familie). Die Koernung des Koerpers liegt in der Familie (HF 0,105).
4. **Maul, Krallen, Augen sind die "Spielzeug"-Teile**: Kiefer einfarbig Creme (172,149,129) mit 77 % Zahnfarbe, 0 % Rot, 0 % dunkel, Rauschen (HF 0,461);
   Krallen Rauschen (HF 0,324); Augen Magenta (207,95,161) C\* 54, im Screenshot sind die Augenstellen dunkel (L\* max 22).
5. **Silberweisse Flecken**: 0,79 % neutralweisse Koerpertexel (L\* > 85, C\* < 10), Familie 0,00-0,19 %; die Engine hat kein Glanzlicht.
6. **Die Tafeln** waren mit Gamma 0,5/0,6 nur fuer K5c aufgehellt: aus C\* 4 wird helles Lavendelgrau (Leser A und B unabhaengig, Abschnitt 7).
7. Gegenprobe der Riegel: **K5c 3 von 22 GRUEN** (nur Texeldichte, Schwanz, Farbkorn). Variante D (Nutzer E13: "Farben FAST richtig", aber "Mischmasch")
   ist bei Farbe GRUEN (A1-A4, K6, K8-K10) und genau bei Weissflecken (A5) und Farbkorn (A12) ROT, wie der Nutzer damals urteilte.

## 1. Mal-Stil der Spielfamilie (Texel, ohne Licht)

Zonen (`p_messen.zonen_von`): EM30 = `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD`, Haut = Meshes ohne 0, 1, 3, 4, 6 (Jeans/Schuhe, Mesh-Mittel b\* -5,8..-9,7)
und ohne Kopf 9. RE2-G4 = `re15_port/shared_assets/RE2/CDEMD0.EMS` Typ 0x34, Koerper ohne Kopf 26, Krallen 4/8/14/19/22/25, Schulterauge x 270-321 y 160-245.
RE2-G5 = Typ 0x36, Koerper ohne Maul (Atlas x 0-227, y 143-255) und Augen. Hund = RE1.5 `PSX/EMD/CDEMD0.EMS` Typ 0x20 ohne Kopf 3/4, Zombie Typ 0x10 ohne Kopf 14.
K5c = `varianten/K5/K5c/FORM.EMD/.TIM`, Koerper = genutzte Texel x < 384 (Seiten 0-2), Kopf/Kiefer/Auge/Kralle = Felder aus `K_alligator/generator/k_form.py:11`.
Jeder Atlastexel einmal (erste Flaeche in Mesh-Reihenfolge bestimmt die CLUT-Zeile).

| Mass (Koerper) | EM30 Haut | G4 | G5 | Hund | Zombie | **K5c** |
|---|---|---|---|---|---|---|
| Texel | 28 856 | 40 063 | 53 197 | 10 450 | 23 830 | 61 896 |
| Lab Mittel | (40,0, 11,7, 6,2) | (19,1, 10,3, 4,3) | (28,8, 14,9, 12,3) | (7,9, 7,3, 5,3) | (29,8, 6,2, -8,7) | **(22,5, 2,6, -2,8)** |
| Saettigung C\* Mittel / Streuung | 14,3 / 9,1 | 12,9 / 17,7 | 19,8 / 10,7 | 9,3 / 10,7 | 12,6 / 5,4 | **4,9 / 4,3** |
| Farbstreuung je Teil sigma_ab (Teile >= 200 Texel) | 9,4 | 10,7 | 9,4 | 10,1 | 7,4 | **4,0** |
| Fleischrot (a\* > 15, C\* > 20, Farbton -40..70 Grad) | 18,0 % | 23,9 % | 38,0 % | 11,5 % | 2,0 % (Kleidung) | **0,5 %** |
| Farbton des Rots (Mittel der Rottexel) | 25 Grad (29,7/26,0/12,4) | 29 Grad (35,6/34,0/19,1) | 36 Grad | 48 Grad | 14 Grad | **-10 Grad (46,1/30,7/-5,3), Magenta** |
| Glanz L\* > 85 / davon neutral (C\* < 10) | 0,03 / 0,03 % | 0,35 / 0,19 % | 0,28 / 0,00 % | 0 / 0 % | 0,22 / 0,03 % | **0,81 / 0,79 %** |
| Hochfrequenz HF_fft (L\*, f > 0,25/Texel, Hann, je Atlasfeld) | 0,113 | 0,151 | 0,056 | 0,071 | 0,213 | 0,105 |
| Gegenmass HF_rest (Restvarianz nach Gauss 1 Texel) | 0,059 | 0,062 | 0,049 | 0,039 | 0,182 | 0,085 |
| Farbkorn (a\*b\*-Rest nach Gauss 1 Texel, Lab) | 4,43 | 5,61 | 3,62 | 2,70 | 5,22 | 2,63 |
| L\* Streuung / p5-p95 | 19,8 / 8,5-70,6 | 14,9 / 4,7-49,9 | 19,4 / 0,5-61,5 | 9,0 / 0,5-30,9 | 11,1 / 12,9-45,7 | 12,6 / 8,2-44,0 |
| Texeldichte Texel/Einheit (Mittel / flaechengewichtet) | 0,062 / 0,055 | 0,046 / 0,042 | 0,040 / 0,034 | 0,058 / 0,059 | 0,082 / 0,081 | 0,038 / 0,027 |
| Dichte Kopf / Koerper | 1,94 | 0,97 | - | 2,66 | 2,11 | 2,05 |

Palette (k-Mittel k = 8, Lab, Anteil), nach L\*:
- **EM30 Haut**: (12,0/5,5/-0,5) 18 %, (18,6/23,8/12,3) 10 %, (29,7/11,6/3,5) 10 %, (36,5/37,4/20,5) 3,5 %, (42,1/7,8/6,0) 15 %, (47,3/17,5/7,4) 13 %, (55,5/9,9/6,9) 16 %, (68,8/6,8/6,8) 16 % - rosa Fleisch in Helligkeitsstufen plus zwei satte Rote.
- **RE2-G4 Koerper**: (6,1/1,0/-1,6) 30 %, (14,1/2,5/-1,2) 32 %, (18,9/25,6/12,2) 7,6 %, (26,2/6,0/1,6) 11 %, (32,8/43,7/25,8) 4,2 %, (39,3/21,3/14,4) 9,2 %, (46,4/65,1/35,5) 3,1 %, (64,3/24,7/16,3) 3,2 % -
  **62 % fast unbuntes Schwarzgrau plus 24 % warmes Rot in vier Stufen. Das ist das Vorbild fuer die Riesenform**: die dunkle, fast neutrale Grundhaut von K5c passt dazu (und zu Bild 1), es fehlen die Rotstufen.
- **K5c Koerper**: (10,4/2,1/1,1) 15 %, (11,4/4,8/-5,0) 10 %, (19,5/0,6/0,5) 17 %, (20,9/3,7/-4,8) 31 %, (30,9/1,9/-3,9) 19 %, (44,8/27,0/-6,6) 0,7 %, (48,3/0,5/-3,7) 5 %, (90,7/0,5/-2,1) 1,2 %.
- Palettenabstand (gewichtetes Mittel des naechsten EM30-Haut-Zentrums je Zentrum): G4 7,4, Hund 9,6, G5 10,4, Zombie 14,3, **K5c 9,2**. Das Mass trennt nicht (die grauen K5c-Cluster
  finden dunkle EM30-Partner); es ist deshalb **kein Riegel**. Getrennt wird ueber Saettigung, Rotanteil, Rot-Farbton und Farbstreuung je Teil.

Vorgaben fuer den Stil (Riegel A1-A7, A10, A12): Saettigung 9,5-21, Farbstreuung je Teil >= 7, Fleischrot 8-40 % bei Farbton 15-50 Grad, Neutralweiss <= 0,20 %,
HF_fft je Feld 0,04-0,26 und HF_rest <= 0,19 (Familienspanne einschliesslich Koepfe/Maeuler: 0,040 EM30-Kopf bis 0,259 Zombie-Kopf; HF_rest bis 0,182),
Texeldichte jedes Teils 0,5-2,7-fach des Koerpers, Farbkorn <= 5,7 (Familie 2,70-5,61).

## 2. Zonenverteilung nach den Screenshots

Kette (`p_zonen.py`): Original `form/bild1.png`, `bild2.png` (= die beiden Version_80%-Standbilder) -> `form_video.video_umkehr` -> Gauss 1,0 px -> T = e / Licht
(`farbe_palette.json` licht: Bild 1 faktor_figur (0,487, 0,432, 0,374), Bild 2 faktor (0,385, 0,342, 0,307)). Zonen = `nutzer_teile/maske_*.png` wie Leser B
(Abzug Boden 349 px und Logo x 0-261, y 428-479). Pruefer-Klassen: R = a\* >= 12 und C\* >= 15 und L\* >= 20; D = L\* < 20; H = L\* >= 45 (nicht R); M = Rest.
In Klammern Leser-B-Klassen (Rot nur bei L\* >= 45 oder C\* >= 30); die Rotanteile haengen an der Schwelle, die Aussage nicht.
K5c: `form_pruefung.Kandidat.rendern` (Licht 'mess') -> `video_filter(entzerren=False, rauschen=True)` -> dieselbe Umkehr. Werte = T.

| Zone | Anteil | Original Lab / C\* / ab-Streuung | Original D/M/R/H % | K5c Kette Lab / C\* / ab-Streuung | K5c D/M/R/H % |
|---|---|---|---|---|---|
| Bild 1 Schwanz | 0,36 | (17,4, 5,9, -1,5) / 9,6 / 9,7 | 78/12/6 (2)/4 | (19,3, 5,7, -0,6) / 7,2 / 5,9 | 70/24/2/4 |
| Bild 1 Ruecken/Flanke | 0,23 | (24,2, 5,7, -1,7) / 10,5 / 10,7 | 56/23/**12** (5)/10 | (32,4, 5,2, -2,7) / 7,4 / 6,7 | 37/35/7/**22** |
| Bild 1 Hinterbein l (W) | 0,12 | (24,3, 2,7, -3,6) / 8,1 / 8,6 | 51/40/2/8 | (24,8, 1,3, -2,1) / 5,0 / 5,2 | 46/48/0/7 |
| Bild 1 Hinterbein r (W) | 0,17 | (23,1, 0,7, -7,9) / 10,4 / 8,6 | 51/42/1/6 | (26,0, 0,2, -4,4) / 6,2 / 5,7 | 44/48/0/8 |
| Bild 1 Vorderglied r (W) | 0,13 | (21,4, -0,1, -3,0) / 5,9 / 6,2 | 63/31/**0**/6 | (24,0, 0,5, -1,1) / 3,3 / 3,6 | 58/32/0/11 |
| **Bild 1 Figur** | 1 | (21,2, 3,8, -3,0) / **9,3 / 9,8** | 63/25/**5,2** (2,0)/6 | (24,6, 3,6, -1,8) / **6,3 / 6,3** | 54/34/**2,5**/10 |
| Bild 2 Oberkiefer | 0,13 | (16,7, 22,0, 10,2) / 25,2 / 8,6 | 88/0/11/0 | (17,6, 15,5, 0,1) / 15,8 / 5,1 | 84/12/4/0 |
| Bild 2 Kopf/Rumpf | 0,54 | (18,8, 18,3, 4,6) / 21,3 / 12,5 | 73/9/15/3 | (23,7, 14,4, -1,0) / 15,1 / 8,5 | 56/25/13/6 |
| Bild 2 Unterkiefer | 0,07 | (56,8, 24,9, 15,2) / 33,0 / 21,2 | 0/0/**77**/23 | (54,8, 13,7, 10,3) / 17,7 / 9,0 | 5/3/52/41 |
| Bild 2 Klaue r (W) | 0,13 | (32,1, 24,1, 21,9) / 34,2 / 16,0 | 21/1/70/8 | (30,5, 13,8, 3,0) / 15,2 / 10,2 | 16/55/27/2 |
| Bild 2 Klaue l (W) | 0,12 | (35,0, 22,2, 19,6) / 31,1 / 13,1 | 6/2/81/12 | (27,0, 22,8, 10,3) / 25,5 / 10,7 | 34/4/59/3 |
| **Bild 2 Figur** | 1 | (25,1, 20,5, 10,3) / **25,6 / 15,4** | 55/5/34 (16)/6 | (26,4, 15,5, 1,9) / **16,7 / 10,2** | 48/23/22/7 |

Lesart je Region (fuer den Maler):
- **Grundhaut** (Ruecken, Schwanz, Beine, Vorderglied): dunkel (T L\* 17-24, 51-78 % dunkle Klasse), fast unbunt bis leicht warm-violett (a\* 0-6, b\* -1..-8; Beine kuehler).
  Das deckt sich mit der G4-Grundhaut (62 % bei a\* 1-2,5). Die Grundhaut von K5c ist im Farbton richtig, aber zu einfoermig: ab-Streuung 0,6-fach.
- **Mehrtoenig**: in jeder Bild-1-Zone ab-Streuung 6,2-10,7 und L\* p5-p95 8-58; K5c 3,6-6,7. Die Haut wechselt zwischen Violett, Rotbraun, Blaugrau (E10).
- **Fleischige Stellen**: Bild 1 Ruecken/Flanke 12 % rot (Z1 rosa, T Lab 54/17/-3 nach Leser B) und das rosa-weisse Schwanzende; **Vorderglied r (W) 0 % rot** (E14, Z5 blaeulich-weiss).
  Bild 2 (Vorderseite): Kopf/Rumpf 15 %, Klauen 70-81 %, Unterkiefer 77 % rot. Die Vorderseite ist das fleischige Ende der Figur.
- **Helle Baender** (H): grau, T L\* 51-58, a\* -4..3 (Leser B), Bild 1 Ruecken 10 %; K5c dort 22 % und 3,9 % geclippt (Original 0,7 %).
- Der Rotanteil der Bild-2-Kette ist zum Teil Kette: K5c-Albedo a\* 2,7 wird in der Kette a\* 15,5 (Einstreuung, farbe.md Abschnitt 4: dunkle Stoffe bis +21 R).
  Deshalb wird Bild 2 nur **Kette gegen Kette** verglichen (Riegel K6-K10), nie Albedo gegen Original-T.

## 3. Kopf, Kiefer, Augen

| Teil | EM30 Kopf | RE2-G4 Kopf | RE2-G5 Maul | Hund Kopf | **K5c Kiefer** | K5c Kopf |
|---|---|---|---|---|---|---|
| Zahn (L\* > 60, C\* < 35) | 18,6 % | 9,4 % | 1,8 % | 0,1 % | **77,1 %** (64,5/5,5/13,8) | 15,1 % (Gaumenstreifen) |
| Rot (a\* > 20) / Lab | 21,3 % (38,9/35,5/19,9) | 32,3 % (38,4/38,5/19,4) | 36,1 % (33,9/28,4/19,8) | 26,7 % | **0 %** | 0 % |
| Dunkel (L\* < 20) | 23,6 % | 31,1 % | 59,9 % | 63,9 % | **0 %** | 20,9 % |
| HF_fft / HF_rest | 0,040 / 0,072 | 0,146 / 0,119 | 0,078 / 0,084 | 0,074 / 0,054 | **0,461 / 0,436** | 0,119 / 0,040 |
| Lab Mittel, C\*-Streuung | (37,8/12,1/9,4), 14,2 | (32,2/16,9/9,7), 18,4 | (18,6/15,2/10,1), 14,6 | (17,8/13,6/10,8), 13,3 | **(63,2/5,3/13,6), 2,1** | (28,8/3,2/0,5), 5,1 |

- **Kiefer**: Die Familie malt Maeuler dreiteilig: dunkles Maulinneres/Zahnfleisch 24-64 %, Fleischrot 21-36 % (Farbton 27-35 Grad), getrennte helle Zaehne 2-19 %.
  Screenshot Bild 2 (Unterkiefer-Maske, T): **Band 77 % Lab (50,9, 31,3, 17,2) RGB (176,98,95), Farbton 29 Grad, C\* 36; Zahnflecken 17,5 % Lab (79,6, 1,4, 9,5)**.
  Weissbezug am selben Bild: Mantel der Frau (x 140-199, y 200-339, L\* > 55) T Lab (79,4, 1,9, 6,9) = so neutral wie die Zahnflecken. Das Rot des Bandes ist Oberflaeche,
  nicht Licht oder Videofilter. Damit ist die Annahme in **E21 gemessen falsch** (Kette K5c-Kiefer a\* 13,7 gegen Original 24,9).
  Vorgabe: Band in Fleischrot mit Farbton 15-50 Grad wie G4/EM30 (Bild 2: 29 Grad), 5-6 getrennte helle Zahnflecken (E27), dunkles Maulinneres; Helligkeit des Bandes
  ueber die Kette gegen Bild 2 (Riegel K7), nicht ueber den Familienwert (L\* 34-39), weil das Video Zahn und Band mischt.
- **Augen**: Screenshot Bild 2 an den Nutzerpunkten E24 (rechtes Auge (W) (223/271), linkes Auge (W) (288/269), Scheibe r 4 px): T Lab (14,6, 16,4, 2,6) und
  (12,1, 15,2, 11,1), **L\* max 22,1 / 13,3**, Umgebung (Ring 6-12 px) L\* 17,4 / 15,0. Die Augenstellen sind so dunkel wie die Haut ringsum. K5c-Kette dort L\* max 51,5 / 36,2.
  Vorgabe: Augenfeld dunkel (L\* <= 30, C\* <= 30, warm wie die Kopfhaut), Kette an E24 L\* max <= 30.
- **Kopf/Oberkiefer**: Bild 2 T (16,7, 22,0, 10,2), 88 % dunkel: dunkles Rotbraun. K5c-Kette (17,6, 15,5, 0,1): zu violett (b\* -10). K5c-Kopfalbedo (28,8, 3,2, 0,5) neutralgrau.

## 4. "Aus einem Guss"

- Je Feld derselbe Malstil: HF_fft K5c Koerper 0,105, Hals 0,079, Schwanz 0,080, Kopf 0,119 (alle in der Familie), **Kiefer 0,461, Kralle 0,324** (doppelt bis dreifach ueber dem
  Familienmaximum 0,259). Kiefer und Krallen sind Rauschflaechen um einen Mittelwert, keine gemalte Form (Leser A: Spektrum flach).
- Saettigung je Teil: Koerper 4,9, Hals 4,0, Schwanz 6,0, Kopf 6,2, Kralle 5,6, Kiefer 14,9 (Creme), Augen 53,7 (Magenta): die beiden Ausreisser sind die buntesten Teile eines sonst
  grauen Modells. Vorgabe: Koerper, Hals, Schwanz, Kopf in einer Farbfamilie (Riegel A1-A4 je Gesamtzone), Kiefer/Augen nach Abschnitt 3.
- Texeldichte: Hals 0,027 und Schwanz 0,028 gegen Koerper 0,038 (0,7-fach), Kopf 0,077 (2,05-fach); Familie Kopf/Koerper 0,97-2,66. Dichte ist **nicht** der Grund fuer
  "hoeher aufgeloest" (E8); K5c ist je Einheit groeber als jedes Familienmodell. Riegel A10 haelt den Stand.
- **E9 Schwanz**: Atlas-L\* je Glied (Mesh 1-6) 15,4 / 12,0 / 15,1 / 15,4 / 13,8 / 21,0; Spruenge 3,4 / 3,1 / 0,3 / 1,6 bis Glied 5, Spitze +7,2 (rosa-weisses Schwanzende
  in Bild 1). Riegel A11 (Glied 1-5 <= 4). Gelenknaehte im Render sind nicht neu gemessen (Leser B ebenso).

## 5. K5c-Ist: warum "Pokemon" (beziffert)

1. Grau statt Fleisch: Koerper C\* 4,9 (Familie 9,3-19,8), Lab (22,5, 2,6, -2,8). Im neutralen Render C\* 4,0 gegen EM30 9,4 / G4 9,6 (Leser A), 4,0-5,0 gegen 10,0-12,5 (Leser B).
2. Kein Fleischrot: 0,5 % Magenta (-10 Grad) gegen 11,5-38 % warmes Rot (25-48 Grad).
3. Je Teil eine Farbe: sigma_ab 4,0 gegen 7,4-10,7; Bild-1-Kette ab-Streuung 6,3 gegen 9,8 (0,64-fach), Bild 2 10,2 gegen 15,4 (0,66-fach).
4. Silberweisse Flecken: 0,79 % neutralweisse Koerpertexel (247,247,249) auf Rumpf, Armen, Hals (Hals 1,75 %); Familie 0,00-0,19 %. Herkunft: rueckprojizierte Video-Hellstellen,
   4-8-fach uebersteuert (Leser A, 85 % der weissen Texel liegen in Bild 1/2 sichtbar) bzw. die Glanzpunkte aus D (Leser B; D hat 0,55 %). Bild-1-Kette Ruecken hell 22 % gegen 10 %.
5. Creme-Maul: Kiefer (172,149,129), 77 % Zahnfarbe, 0 % Rot, 0 % dunkel, HF 0,461. Im Screenshot ein lachsrotes Fleischband (176,98,95) mit Zahnflecken.
6. Magenta-Augen (207,95,161), C\* 54, buntestes Element; im Screenshot dunkel (L\* <= 22).
7. Rauschen statt gemalter Form an Kiefer und Krallen (HF 0,46 / 0,32, Familie <= 0,26).
8. Tafeln: Gamma 0,5 (`K5/generator/k5_blatt.py:263`) bzw. 0,6 (`k5_rueckbild.py:65`) nur auf K5c, kein Vergleichsmodell gleich behandelt: L\* 16 -> 41-45 bei C\* 4,5 = pastell.
Was nicht der Grund ist: Texeldichte (K5c am groebsten), Koernung des Koerpers (HF 0,105 in der Familie), Farbton der dunklen Grundhaut (passt zu Bild 1 und zur G4-Grundhaut).
Kopfform (kurze Schnauze K5c) ist hier nicht gemessen; siehe Abschnitt 8.

## 6. Riegel-Tabelle

Messung: `python analysis/befunde_runde41/form/varianten/K5/farbe_lesung/P_skripte/p_riegel.py <Variantenordner>` (Ordner mit FORM.EMD, FORM.TIM, form.json im Format von
`form_pruefung.Kandidat`). Ausgabe GRUEN/ROT je Riegel und `riegel_<name>.json`. Bauer und Pruefer rufen dasselbe Skript am selben Ordner auf; die Kette ist deterministisch
(`video_filter` mit festem Saatwert). Gegenprobe gelaufen: **K5c 3/22 GRUEN** (`riegel_K5c.json`), D_fleisch 12/20 (`riegel_D_fleisch.json`).
Zonen im Atlas: Koerper = genutzte Texel x < 384; Kopf (384,0,128,104), Kiefer (384,106,128,72), Auge (384,196,24,10), Kralle (410,180,100,40) (`k_form.py:11`).
Liegt der Neubau anders im Atlas, sind die Felder im Skript nachzutragen und im Dossier zu nennen.

| # | Mass | Vorgabe | Toleranz | Quelle | K5c heute |
|---|---|---|---|---|---|
| A1 | Saettigung C\* Mittel, Atlas Koerper | 9,5-21 | Grenzen fest | Familie Hund 9,3 ... G5 19,8 (Ganzmodell 20,2); EM30 Haut 14,3 | 4,9 ROT |
| A2 | Farbstreuung je Teil sqrt(var a\* + var b\*), Mittel ueber Meshes mit >= 200 Koerpertexeln | >= 7,0 | - | Familie 7,4-10,7 | 4,0 ROT |
| A3 | Fleischrot-Anteil Koerper (a\* > 15, C\* > 20, Farbton -40..70 Grad) | 8-40 % | - | Hund 11,5, EM30 18,0, G4 23,9, G5 38,0 | 0,5 % ROT |
| A4 | Farbton der Rottexel atan2(b\*, a\*) des Mittels | 15-50 Grad | - | EM30 25, G4 29, G5 36, Hund 48; Bild-2-Band 29 | -10 ROT |
| A5 | Neutralweiss Koerper (L\* > 85 und C\* < 10) | <= 0,20 % | - | Familie 0,00-0,19 %; Engine ohne Glanzlicht (farbe.md 5) | 0,79 % ROT |
| A6 | HF_fft je Feld (Koerper, Kopf, Kiefer, Kralle): L\*, Feld = Zusammenhangskomponente je Mesh >= 200 Texel, aussen Feldmittel, Hann, Energie f > 0,25 / f > 0, flaechengewichtet | 0,04-0,26 je Feld | - | Familie 0,040 (EM30-Kopf) ... 0,259 (Zombie-Kopf) | Kiefer 0,461, Kralle 0,324 ROT |
| A7 | HF_rest je Feld: normierte Faltung Gauss sigma 1 Texel im Feld, Restvarianz / Varianz, Innentexel (Erosion 3x3) | <= 0,19 | - | Familie 0,039-0,182 | Kiefer 0,436, Kralle 0,327 ROT |
| A8 | Kiefer: Zahn (L\* > 60, C\* < 35) / Rot (a\* > 20) / Dunkel (L\* < 20) | Zahn 2-20 %, Rot >= 20 %, Dunkel >= 20 % | - | EM30 18,6/21,3/23,6; G4 9,4/32,3/31,1; G5 1,8/36,1/59,9 | 77,1/0/0 ROT |
| A9 | Augenfeld Mittel L\* / C\* | L\* <= 30, C\* <= 30 | - | Bild 2 E24 L\* 12-15 (max 22) | 56,1/53,7 ROT |
| A10 | Texeldichte sqrt(UV-Flaeche/3D-Flaeche) je Mesh / Koerpermittel | 0,5-2,7 | - | Familie Kopf/Koerper 0,97-2,66 | 0,64-2,33 GRUEN |
| A11 | Schwanz: Atlas-L\* je Glied Mesh 1-6, Sprung Nachbarglied 1-5 | <= 4,0 | Spitze (Glied 6) frei | E9; Ist 3,4 | 3,4 GRUEN |
| A12 | Farbkorn Koerper: RMS des a\*b\*-Rests nach Gauss sigma 1 Texel (Innentexel) | <= 5,7 | - | Familie 2,70-5,61; D 6,32 = E13 "Mischmasch" | 2,63 GRUEN |
| K1 | Bild 1 Kette, Figur: C\* Mittel / Original | 0,85-1,2-fach | Original 9,3 | Abschnitt 2 | 6,3 ROT |
| K2 | Bild 1 Kette, Figur: ab-Streuung / Original | >= 0,85-fach | Original 9,8 | Abschnitt 2, E10 | 6,3 ROT |
| K3 | Bild 1 Kette, je Zone dE76 der Lab-Mittel gegen Original | <= 6 je Zone | - | Abschnitt 2 | Ruecken 8,3 ROT |
| K4 | Bild 1 Kette, Figur: Rot-Klasse (a\* >= 12, C\* >= 15, L\* >= 20) | 0,6-1,5-fach Original | Original 5,2 % | Abschnitt 2, E14 | 2,5 % ROT |
| K5 | Bild 1 Kette: Hell-Klasse (L\* >= 45, nicht rot) Ruecken und geclippte Pixel (Video min >= 200) Figur | je <= 1,5-fach Original | Original 9,9 % / 0,5 % | Abschnitt 2 | 21,9 % / 1,0 % ROT |
| K6 | Bild 2 Kette, Figur: dE76 und C\* | dE <= 6, C\* >= 0,85-fach | Original (25,1, 20,5, 10,3), C\* 25,6 | Abschnitt 2 | dE 9,9, C\* 16,7 ROT |
| K7 | Bild 2 Kette, Unterkiefer: dE76 und a\* | dE <= 6, a\* >= 0,85-fach | Original (56,8, 24,9, 15,2) | Abschnitt 3 | dE 12,4, a\* 13,7 ROT |
| K8 | Bild 2 Kette, Oberkiefer dE76 | <= 6 | Original (16,7, 22,0, 10,2) | Abschnitt 3 | 12,0 ROT |
| K9 | Bild 2 Kette, Klauen r/l (W): a\* | >= 0,75-fach Original | Original 24,1 / 22,2; b\* kein Riegel (Feuerschein) | Abschnitt 2 | 13,8 / 22,8 ROT |
| K10 | Bild 2 Kette, Augenpunkte E24 (Scheibe r 4 px): L\* max | <= 30 | Original 22,1 / 13,3 | Abschnitt 3 | 51,5 / 36,2 ROT |
| N1 | Neutraler Render gegen EM30 (Leser-B-Werkzeug `B_skripte/lauf_render.py`, Licht 0,55 + 0,45 max(0, N.L), 3 Winkel, roh): C\* je Winkel | >= 0,75-fach EM30 | EM30 10,0 / 12,5 / 10,5 | Leser A und B unabhaengig gleich (Pruefer nicht nachgerechnet) | 4,6 / 5,0 / 4,0 ROT |
| N2 | dasselbe: Anteil L\* > 85 je Winkel | <= 0,10 % | EM30 0,00 % | wie N1 | 0,83 / 1,30 / 0,11 % ROT |

Kette = `p_zonen.rueck`: `form_pruefung.Kandidat(<ordner>/form.json).rendern(n)` -> Figur = Maske & Albedo > 0 -> `form_video.video_filter(sauber, bild=n, entzerren=False, rauschen=True)`
-> `video_umkehr` -> Gauss 1,0 px -> / Licht (farbe_palette.json) -> Lab; Zonen `nutzer_teile/maske_*.png` wie Abschnitt 2, geschnitten mit der Figur.
Pose und Lage stehen in form.json (bild1/bild2: lage, gier, clip); fuer den Neubau gelten die K5c-Werte (Bild 1 Clip 0, Bild 2 Clip 1).

## 7. Darstellungsregel fuer Nutzer-Tafeln

1. **Kein Gamma, keine Aufhellung** auf ein Modell, ohne die Vergleichsmodelle in derselben Tafel identisch zu behandeln; die Behandlung steht in der Kachelbeschriftung
   ("roh" oder "Gamma 0,5 auf alle"). Rohe Fassung immer zuerst.
2. **Szenenlicht-Kacheln neben dem Screenshot**: je Bild drei Kacheln gleicher Groesse: Original | Neubau mit Spiellicht (`Kandidat.rendern`, sauber) | Neubau durch `video_filter`
   (wie `B_4`/`B_5`). Keine Kachel mit erfundenem Blattlicht als Hauptbild.
3. **Familienkachel**: EM30_HANGAR | RE2-G5 | RE2-G4 | Neubau, selber Renderer (`form_buehne.rastern`), selbes weisses Licht, gleiche Umkugel (0,46 der Kachel), drei Winkel
   (3/4 vorn, Seite, 3/4 hinten), roh. Zahlen L\*, C\*, L\* > 85 je Kachel in die Beschriftung.
4. Nahkacheln Kopf/Kiefer: Bild-2-Ausschnitt x 200-360, y 290-420, 3-fach, Original | T | Neubau Kette (wie `B_8`).
5. Bilder erst zum Nutzer, wenn `p_riegel.py` fuer den Neubau alle Riegel GRUEN meldet und ein zweiter Pruefer dieselben Zahlen erhalten hat.

## 8. Abgleich Leser A / Leser B (Entscheide des Pruefers)

| Punkt | Leser A | Leser B | Pruefer gemessen | Entscheid |
|---|---|---|---|---|
| Farben je Teil | 3,4 (EM30) / 4,5 (K5c), "trennt nicht" | 5,42 / 1,53 | Zaehlart entscheidet: globale Palette 4,45/4,46 (= A), Kette dE < 10 4,82/1,38 (= B), vollstaendige Verknuepfung 5,64/3,08, roh 7,1/6,4; Hund mit Kette 1,83 | Keine Clusterzahl als Riegel (B trennt K5c nicht vom Hund, A misst Helligkeitsstufen). Aussage von B stimmt; Mass = sigma_ab je Teil (A2): Familie 7,4-10,7, K5c 4,0 |
| Hochfrequenz | K5c-Koerper 0,104 = Familie | K5c 0,174 (Koerper 0,151) gegen EM30 0,089 | Koerper 0,105, EM30-Haut 0,113; Kiefer 0,461, Kralle 0,324, EM30-Kopf 0,040 | A. B mischte Seite-3-Krallentexel (Meshes 13-15, 18-20, 28-31) und Kiefer in K5c, den Kopf in EM30. Rauschen sitzt in Kiefer und Krallen |
| Glanzanteil | 0,79 % | 0,60 % | Koerper 0,79 %, Gesamt 0,59 % | beide richtig, Zonenwahl; Riegel am Koerper |
| Rotanteil EM30 | 13,4 % (mit Jeans), Haut 18,2 % | 22,3 % | Haut ohne Kopf, jeder Texel einmal: 18,0 % (A-Def.) / 20,1 % (B-Def.) | 18 %; B zaehlte Spiegel-Meshes doppelt und den Kopf mit |
| Kiefer-Vorbild | Familie (32,4/17,7/10) | Bild-2-Band (54/30/16) | Band (50,9/31,3/17,2) 29 Grad; G4-Rot 27 Grad, EM30-Rot 29 Grad | kein Widerspruch: Farbton/Saettigung aus Bild 2 = Familie; Aufbau (dunkel/rot/Zahn) aus der Familie (A8); Helligkeit ueber die Kette (K7) |
| Bild 1 "trifft gut" | - | dE 2,3-7,2 | Ruecken dE 8,3, H 22 % gegen 10 % | Bild 1 trifft nur im Farbton; Ruecken zu hell, zu weiss, zu einfoermig (K2, K3, K5) |

Nicht nachgerechnet, weil A und B unabhaengig gleich: Render-Gegenprobe (N1/N2), Herkunft der Weisstexel (Rueckprojektion), Farbverlauf der Varianten (Bruch D -> F:
Chroma 18,2 -> 4,5; Pruefer: C_mitte 13,4, D_fleisch 18,2, J 5,2).

Offen (nur Gestaltung, kein Bild/Modell entscheidet):
1. "von der Struktur" (Nutzer 2026-11-06): Texturstruktur oder Koerperbau/Kopfform? Dieses Blatt deckt die Textur ab; die Kopfform K5c ist nicht gemessen.
2. E19 ("Form und Textur werden nicht mehr veraendert") gegen die heutige Ablehnung der Farben: Freigabe, die Koerpertextur (Seiten 0-2) umzufaerben, ohne die Form zu aendern.
   Die Riegel A1-A5, A12, K1-K5 sind ohne neue Koerpertextur nicht erreichbar.

Messgrenzen (keine Gestaltungsfrage): Kanalbalance Bild 1 nur geschaetzt (+-8 %, farbe.md 7); Klauen-Orange in Bild 2 teils Feuerschein (daher K9 nur a\*);
Augen im Video <= 8 px, nur Helligkeit lesbar; 74 % der Koerpertexel sind in keinem Bild sichtbar (Leser A) und werden nach E11 aus den sichtbaren Bereichen ergaenzt.
