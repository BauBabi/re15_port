# Farbe und Textur der Riesenform: Leser B ("Vorlage und Nutzer")

Stand 2026-11-06, Arbeitsbaum r41_form (HEAD 19a9b2b17), kein Commit. Kein RE-Wert: Messung an zwei Videostandbildern und an Port-Modellen.
Seiten: (W) = aus Sicht des Wesens. Farben RGB 0..255 und CIE-Lab (D65, sRGB, OpenCV). **e** = Spielfarbe auf dem Schirm (rueckgerechnet),
**T** = Texturschaetzung = e / Licht (Licht Bild 1 (0,487, 0,432, 0,374), Bild 2 (0,385, 0,342, 0,307), `farbe_palette.json` licht).
Kein Bild in diesem Blatt ist aufgehellt, ausser Zeilen 3-4 von B_6 (dort beide Modelle gleich, beschriftet).

Belegbilder: `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/B_1 ... B_8`.
Skripte und Rohzahlen: `.../farbe_lesung/B_skripte/` (`zonen.py`, `lauf_orig.py`, `lauf_var.py`, `stil.py`, `render.py`, `historie.py`, `blaetter.py`;
JSON `zonen_orig.json`, `zonen_var_K5c_K4_heute.json`, `stil.json`, `render_mass.json`, `historie.json`).

## 0. Kurz

1. **In den beiden Screenshot-Kameras trifft K5c die Rueckansicht (Bild 1) gut, die Vorderseite (Bild 2) nicht.** Bild 1, gleiche Kette
   (Render, video_filter, Umkehr): Figur Original T Lab (21,2/3/-3), K5c (24,7/3/-2); Klassenanteile D/M/R/H/W 62/30/2/5/0,5 % gegen 53/36/2/8/1,0 %.
   Bild 2: Original (25,5/20/10), K5c (27,1/15/1). Die Vorderseite ist im Original roter und waermer (a* +5, b* +9), Kiefer und Klauen am meisten.
2. **Was den Eindruck "passt nicht zum Spiel / Pokemon" traegt, liegt in der Textur selbst, nicht im Kamerablick** (Atlas, `stil.json`):
   - fast unbunt: Chroma 6,2 +- 5,2 (EM30-Haut 14,7 +- 9,3), mittleres a* 2,3 (EM30 10-12);
   - je Teil eine Farbe: 1,5 Farbcluster je Mesh (EM30 5,4);
   - Fleischrot fehlt: 0,5 % der Texel rot (EM30 22,3 % in 6 Rot-Clustern, L* 23-58);
   - Foto-Rauschen statt gemalter Form: Hochfrequenzanteil 0,17 (EM30 0,089), Unterkiefer 0,46 (Rauschen auf einer Flaeche);
   - 507 silberweisse Texel (L* 97, a* 0, b* -1) auf Rumpf, Armen, Hals, Bein (EM30: 29 Texel, nur Augen/Zaehne);
   - Augen magenta (Textur (200,93,157), Lab 55/50/-15, Chroma 52 = buntestes Element des Modells);
   - Unterkiefer einfarbig beige (172,149,129), Lab 63/5/14, Streuung L* 59-71, ein einziger Farbcluster. Im Original ist der Kiefer ein **lachsrotes Fleischband**
     T (184,108,104), Lab 54/30/16, mit weissen Zahnflecken (202,193,178).
3. **Gamma 0,5 der alten Tafeln** hebt K5c von L* 17-20 auf 43-45 bei Chroma 4,6: lavendelgrau-pastell. EM30 wird gleich behandelt blassrosa (L* 48-52, Chroma 10-12) und bleibt dabei doppelt so bunt (B_6).
4. **Die vom Nutzer gebilligten Farben sind nicht mehr im Modell.** A/B/C (E10, "alle in Ordnung", Basis C: Lab 13,0/11,6/-4,1, Chroma 13,3) und D (E13, "FAST in die richtige
   Richtung", Chroma 18,1, Rot 22,9 %) sind gebilligt. Der Bunt- und Rotanteil verschwand im Schritt D -> F (E13-Umsetzung "Farbton nur aus Bild 1"): Chroma 4,6, Rot 0,6 %.
   Seitdem (G, H2, J, K4, K5c) bleibt die Haut bei Chroma 4,6-5,4 und a* 2,1-2,5 (Abschnitt 4).

## 1. Verfahren

- **Original**: `bild1.png`/`bild2.png` (= `Version_80%/vlcsnap-2026-10-31-00h28m45s774.png` / `...00h29m02s830.png`, MAE 0,0 gegen die Kopien).
  Umkehr `re15_port/tools/form_video.video_umkehr(bild=n)`, danach Gauss 1,0 px, T = e / Licht. Geclippt = Videopixel mit min(R,G,B) >= 200.
- **Zonen** = Nutzer-Teile (`nutzer_teile/maske_*.png`). Abgezogen werden die Bodenpixel des Pruefers (`scratchpad/leser/pruefer/boden_fuss_{links,rechts}.npy`, 189 + 160 px)
  und das Logo (x 0-261, y 428-479).
  - Bild 1: Schwanz = tail; Ruecken/Flanke = part_of_Body ohne tail; Hinterbein links (W) = left_leg + bein_links + fuss_links ohne tail;
    Hinterbein rechts (W) = right_leg + bein_rechts + fuss_rechts ohne tail/part_of_Body; Vorderglied rechts (W) = right_arm ohne part_of_Body.
  - Bild 2: Oberkiefer; Kopfoberseite/Rumpf = face_and_upper_body ohne Kiefer; Unterkiefer; Klaue rechts (W) = right_arm_front ohne Kiefer; Klaue links (W) = left_arm_front.
- **Materialklassen je Pixel/Texel (PORT-WAHL dieses Lesers)**: D L* < 20; M 20 <= L* < 45; R (rosa/rot) L* >= 45 und a* >= 10, oder L* >= 30 und C* >= 30 und a* > 0;
  H (helles Band) L* >= 45 und a* < 10; W = geclippt. Die Grenzen sind an `farbe_palette.json` gelegt: Grundhaut L* 9, Rotbraun 32, Rosa 61 (a* 17), Baender 58 (a* -3).
  M ist nur eine Helligkeitsstufe. Darin liegen auch die neutralgrauen K5c-Texel; den Farbton zeigen a*/b* und Chroma.
- **Mehrtoenigkeit**: k-Mittel k = 6 in Lab, Zentren mit dE < 10 verschmolzen, gezaehlt werden Cluster ab 5 %; dazu L* p5-p95, Farbtonstreuung (zirkulare Std, nur C* > 8) und mittleres Chroma.
- **K5c / K4 in den Kameras**: `form_pruefung.Kandidat.rendern` (Licht 'mess' = Disc-Licht x licht.json), dann DIESELBE Kette wie beim Original:
  `video_filter(entzerren=False, rauschen=True)` -> `video_umkehr` -> / Licht. Zusaetzlich die Albedo (Texturfarbe am Pixel). Zonen = Nutzer-Teile ∩ Renderfigur.
- **STIL T1-T8** an den Texeln (`stil.py`): je Flaeche die Texel im UV-Dreieck mit der CLUT-Zeile der Flaeche (`(uv[2]>>6) - clut_y`, Seite `(uv[5]&0xF)*128`).
  EM30 = `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD` (RE1.5-EMD, TIM in dir[8]). Als Haut zaehlen die Meshes ohne Jeans/Schuhe: ausgenommen sind 0, 1, 3, 4, 6
  (Mittel b* -5,9 bis -9,9, Atlas x 128-254, y 124-254). K5c = `varianten/K5/K5c/FORM.EMD/.TIM`. Zaehne/Augen sind ausgenommen (Atlasfeld x >= 384, y >= 175).
  T3 Dichte = sqrt(UV-Flaeche / 3D-Flaeche) je Dreieck (Texel je Einheit). T5 = Leistungsanteil bei f > 0,25 Zyklen/Texel (Periode < 4 Texel),
  L*, je Atlasfeld (Zusammenhangskomponente je Mesh, mindestens 12x12), Hann-Fenster, Flaechenmittel. T7 rot = a* >= 15, L* >= 20, Farbton -30..70 Grad.
- **Render-Gegenprobe** (`render.py`, B_6): `form_buehne.rastern`, Gouraud wie `k5_blatt.blick_bild`, Licht fuer beide 0,55 + 0,45 * max(0, N*L) weiss, Umkugel = 0,46 Kachel,
  EM30 Clip 0 Bild 0, K5c Clip 2 (Ruhe) Bild 0, drei Winkel; dann beide mit Gamma 0,5 (wie `k5_blatt.blick_bild`).

## 2. Zonen: Screenshot gegen K5c

Anteil = Pixel der Zone an der Figur im selben Bild. Klassen D/M/R/H/W in %. "Kette" = Original bzw. K5c durch Filter und Umkehr (vergleichbar).
"Albedo" = K5c-Texturfarbe am Pixel (ohne Licht und Video). Mehrton = Cluster / L* p5-p95 / Farbtonstreuung / Chroma.

### Bild 1 (Rueckansicht, B_1, B_4)

| Zone (Lage, bbox x0,y0,x1,y1) | Anteil O / K5c | Klassen Original | Klassen K5c Kette | T Original (Lab) | T K5c Kette (Lab) | K5c Albedo (Lab) | Mehrton Original | Mehrton K5c Kette |
|---|---|---|---|---|---|---|---|---|
| Schwanz (244,184,322,477) | 0,36 / 0,38 | 77/17/2/2/1,1 | 68/27/2/2/0,1 | (49,39,45) 16,9/6/-2 | (55,45,49) 20,0/5/-1 | (41,32,36) 13,3/5/-1 | 4 / 8-44 / 52 / 9,7 | 3 / 10-43 / 37 / 7,2 |
| Ruecken/Flanke (291,205,380,317) | 0,23 / 0,24 | 54/32/5/8/0,7 | 36/38/6/17/3,9 | (65,55,61) 24,3/5/-2 | (80,71,78) 31,4/5/-3 | (79,70,79) 31,0/6/-4 | 6 / 10-57 / 58 / 10,5 | 4 / 12-62 / 28 / 7,4 |
| Hinterbein l (W) (193,257,250,427) | 0,12 / 0,11 | 50/42/0/8/0 | 44/49/0/7/0 | (59,58,64) 24,7/2/-4 | (60,59,63) 25,1/1/-2 | (54,54,62) 22,7/1/-5 | 6 / 11-49 / 55 / 8,2 | 4 / 13-47 / 62 / 5,0 |
| Hinterbein r (W) (287,311,345,461) | 0,17 / 0,12 | 50/44/0/6/0 | 42/50/0/9/0 | (49,56,68) 23,5/0/-8 | (59,63,69) 26,7/0/-4 | (38,39,47) 15,7/2/-6 | 6 / 11-48 / 41 / 10,5 | 5 / 15-51 / 24 / 6,2 |
| Vorderglied r (W) (375,242,425,375) | 0,13 / 0,14 | 61/33/0/6/0 | 56/33/0/11/0,3 | (49,53,57) 22,0/-1/-3 | (58,58,60) 24,3/1/-1 | (50,51,55) 21,2/1/-3 | 3 / 11-47 / 53 / 6,0 | 3 / 12-55 / (115) / 3,4 |
| **Figur** (32327 / 29879 px) | | 62/30/2/5/0,5 | 53/36/2/8/1,0 | (54,50,56) 21,2/3/-3 | (62,57,61) 24,7/3/-2 | (53,47,53) 20,4/4/-3 | 5 / 9-49 / 60 / 9,3 | 4 / 11-54 / 45 / 6,3 |

Klassenfarben im Original, Bild 1 (T): D (35-40, 29-36, 33-46), M Ruecken/Schwanz (73-77, 64-65, 74) Lab 28/7/-3 bis -6, Beine (56-74, 69-74, 82) Lab 28-32/-2..1/-5..-10
(kuehler), R Ruecken = Z1 (156,118,134) Lab 54/17/-3 und Schwanzende (155,104,117) Lab 49/22/1, H (115-148, 124-136, 127-147) Lab 51-58/-4..3/-8..6, W nur Untergrenze.

### Bild 2 (Vorderseite im Feuerlicht, B_2, B_5)

| Zone (bbox) | Anteil O / K5c | Klassen Original | Klassen K5c Kette | T Original (Lab) | T K5c Kette (Lab) | K5c Albedo (Lab) | Mehrton Original | Mehrton K5c Kette |
|---|---|---|---|---|---|---|---|---|
| Oberkiefer (217,280,329,374) | 0,13 / 0,14 | 87/11/1/0/0 | 82/18/0/0/0 | (72,26,28) 16,5/22/9 | (64,34,44) 17,6/16/0 | (51,48,53) 20,4/2/-3 | 1 / 12-25 / 17 / 25,3 | 1 / 13-22 / 11 / 15,9 |
| Kopfoberseite/Rumpf (201,142,425,374) | 0,54 / 0,54 | 72/24/1/3/0 | 54/36/4/6/0 | (71,34,41) 18,4/19/4 | (77,49,60) 23,9/15/-2 | (65,59,64) 25,5/4/-2 | 6 / 10-40 / 30 / 21,3 | 3 / 11-56 / 15 / 15,1 |
| Unterkiefer (243,318,343,408) | 0,07 / 0,07 | 0/8/74/18/0,1 | 4/19/49/29/0 | (182,119,113) 56,5/24/13 | (161,124,115) 55,3/13/10 | (145,131,121) 55,7/4/7 | 6 / 30-86 / 37 / 33,0 | 6 / 20-77 / 14 / 17,7 |
| Klaue r (W) (143,314,333,443) | 0,13 / 0,13 | 19/38/35/8/0 | 14/80/5/2/0 | (118,59,43) 32,2/24/22 | (93,64,68) 30,6/13/3 | (76,77,81) 32,5/0/-3 | 6 / 16-62 / 21 / 34,3 | 4 / 17-43 / 21 / 15,2 |
| Klaue l (W) (367,262,468,418) | 0,12 / 0,13 | 5/43/42/10/0 | 33/49/16/1/0 | (122,67,53) 34,9/22/19 | (100,49,50) 27,1/23/10 | (70,70,71) 29,8/1/-1 | 5 / 20-60 / 21 / 31,1 | 5 / 15-46 / 11 / 25,5 |
| **Figur** (45038 / 42352 px) | | 53/25/16/5/0 | 46/40/8/6/0 | (92,47,46) 25,5/20/10 | (86,55,62) 27,1/15/1 | (71,66,70) 28,6/3/-2 | 6 / 11-61 / 29 / 25,6 | 4 / 13-61 / 19 / 16,7 |

### Mehrfarbigkeit in Zahlen (Nutzer: "Haut hat mehr als eine Farbe, Wunden fleischig")

- Original, Kette: Bild 1 je Zone 3-6 Cluster, L* p5-p95 8-57, Farbton streut 41-58 Grad, Chroma 6-10,5. Bild 2: 1-6 Cluster, Chroma 21-34.
- K5c, gleiche Kette: Bild 1 3-5 Cluster, Chroma 3,4-7,4 (Figur 6,3 gegen 9,3 = 0,68-fach). Bild 2 Chroma 15-25 (Figur 16,7 gegen 25,6 = 0,65-fach), Rot 8 % gegen 16 %.
- K5c-Albedo in den Zonen: 1-6 Cluster, Chroma 1,6-8,6. Die Mehrtoenigkeit, die die Kette zeigt, macht zum Teil erst das Video (Korn, Chroma-Unschaerfe).
- "Wunden fleischig": Rot (R) ist im Original Bild 1 nur Z1 und das Schwanzende (2,1 % der Figur, Farbton pink: T Lab 49-54/17-22/-3..1). In Bild 2 sind 16 % rot,
  getragen von Unterkiefer (74 % der Zone) und Klauen (35-42 %). K5c-Albedo: Unterkiefer 10 % R, Klauen 0 % R.

### Kiefer Bild 2 (B_8, B_3 unterste Zeile)

- Band (Klasse R, 2477 px): T (184,108,104), Lab 54,1/30/16. Zahnflecken (Klasse H, 603 px): T (202,193,178), Lab 78/1/8.
- Bezug am selben Bild: der Mantel der Frau (Textur (201,209,213), x 140-199, y 200-339, L* > 55) kommt zurueck als T (204,195,184), Lab 79/1/7.
  Zahnflecken und Mantel sind damit gleich neutral. Der Kiefer bekommt kein farbiges Feuerlicht, das Rot des Bandes ist **Oberflaeche**.
  Mit den Zaehnen als Weissbezug (235) wird das Band (214,132,137) = Lab 64/32/11 (lachsrosa Fleisch).
- Klauen dagegen: helle Stellen T (154-165, 146-159, 118-119), b* +16..22, gelblich. Dort steckt Feuerschein; ihr Orange ist nicht sicher Oberflaeche.
- K5c-Kiefer: Textur (172,149,129) Lab 63/5/14, einfarbig. In der Kette Lab 55/13/10 gegen Original 56,5/24/13: Rot (a*) fehlt zur Haelfte.
  Die Annahme in E21, Licht und Videofilter machten aus dem beigen Kiefer das rote Band, stimmt gemessen nicht.

### Glanz

- Original: Pixel mit T L* > 85: Bild 1 0,13 %, Bild 2 0,52 % der Figur. Geclippt: Bild 1 0,54 %, Bild 2 0,00 %. Die hellen Stellen (Klasse H) sind
  mittelgraue Oberflaeche: T L* 51-58, a* -4..3, kuehl in Bild 1, nicht silberweiss. In Bild 2 zeigt das flache Licht dieselben Baender (farbe.md Abschnitt 6).
  Die Engine hat kein Glanzlicht (`re15_light.h`, farbe.md Abschnitt 5). Weisse Kerne sind im Video geclippt und tragen keine Farbaussage (farbe.md Abschnitt 1).
  **Im Original gibt es keine silbrigen Glanzflecken auf der Haut**, nur helle graue Baender und kleine geclippte Kerne.
- K5c: 507 Texel mit L* 97, neutral (a* 0,1, b* -0,9), auf Mesh 0, 7, 9, 10, 11, 13, 17, 18, 21, 28, 31 (Rumpf 7/10, Arme, Hals, Bein). Atlas 0,60 %, Koerper 0,62 %.
  Im Render unter neutralem Licht L* > 85: 0,11-1,30 % (EM30 0,00 %). Bild-2-Kamera Albedo L* > 85: 2,07 % gegen Original 0,52 %. Bild 1 Kette geclippt 1,02 % gegen 0,54 %.
  Die Texel sind rueckprojizierte geclippte Videokerne bzw. die "feuchten Glanzpunkte" aus D (E10-Umsetzung), als Weiss gelesen. Das widerspricht farbe.md 1 ("Weiss im Video heisst nicht weiss im Spiel").

## 3. STIL T1-T8: EM30_HANGAR gegen K5c (und K4)

| Mass | EM30 Haut | K5c gesamt (ohne Zaehne/Augen) | K5c Koerper J | K5c Hals+Kopf | K5c Unterkiefer | K4 gesamt |
|---|---|---|---|---|---|---|
| Texel | 58188 | 84226 | 73315 | 15798 | 8770 | 98461 |
| T1 Palette (Top 4, L*/a*/b* Anteil) | 54/12/7 23 %; 69/7/7 16 %; 41/17/6 14 %; 42/6/6 12 % | 23/4/-5 22 %; 20/1/1 17 %; 10/2/0 14 %; 16/4/-5 14 % | 25/4/-4 22 %; 17/4/-5 18 %; 35/1/-3 17 %; 10/3/0 16 % | 20/4/-5 34 %; 20/0/0 24 %; 26/3/-3 19 %; 62/6/13 13 % | 64/9/13 23 %; 62/3/15 18 %; 59/6/12 16 % | 21/3/-3 31 %; 12/3/-1 21 %; 29/3/-4 15 % |
| T2 Farben je Teil (Cluster >= 5 %) | **5,42** (12 Meshes) | **1,53** (32) | 1,69 | 1,5 | 1,0 | 1,81 |
| T3 Texeldichte Mittel / Median (Texel je Einheit) | 0,067 / 0,055 | 0,042 / 0,032 | 0,038 / 0,031 | 0,065 / 0,057 | 0,069 / 0,061 | 0,042 / 0,032 |
| T4 Glanz L* > 85 | **0,05 %** | **0,60 %** | 0,62 % | 0,31 % | 0,00 % | 0,81 % |
| T5 Hochfrequenz | **0,089** | **0,174** | 0,151 | 0,113 | **0,461** | 0,175 |
| T6 Chroma Mittel / Streuung | **14,7 / 9,3** | **6,2 / 5,2** | 5,1 / 4,1 | 5,9 / 4,9 | 14,7 / 2,1 | 6,2 / 4,9 |
| L* Mittel (p5-p95) | 40,9 (9-72) | 27,8 (9-64) | 25,5 (9-49) | 28,1 (17-64) | 63,3 (59-71) | 30,1 (9-61) |
| T7 Rot: Anteil, Cluster, L* p5-p95, Mittel Lab | **22,3 %**, 6, 23-58, 42/23/10 | **0,5 %**, (6 in 0,5 %), 22-66, 42/26/-2 (magenta) | 0,5 % | 0,1 %, 0 | 0,0 %, 0 | 0,4 % |
| Klassen D/M/R/H | 22/28/26/24 | 34/50/2/15 | 34/59/0/7 | 22/62/2/15 | 0/0/8/92 | 27/54/3/16 |

T8 Maul/Kiefer/Zaehne: K5c Zaehne/Augen-Feld: Zaehne Lab 94/2/1 (30 %) und 91/-6/18 (13 %), Augen Lab 56/52/-15 (32 %, magenta), Rand 63/6/12.
Vorbild am Bild: Kieferband Lab 54/30/16 (Zaehne als Weissbezug: 64/32/11), Zahnflecken 78/1/8. Kiefer und Maul der RE2-Formen G4/G5 misst Leser A.

Was die Zahlen sagen:
- **Farbe**: EM30 malt warm (a* 6-17, b* 6-7) mit sattem Rot (38/40/22, 3 %) und dunkelroten Adern (20/23/12, 10 %). K5c ist neutralgrau-violett (a* 0-4, b* -5..1).
  Sein einziges Rot ist pink-magenta (a* 26, b* -2), 0,5 %.
- **"Aus einem Guss / PSX-Stil"**: EM30 hat 5,4 Farben je Teil bei halber Hochfrequenz. Es ist gemalt: grosse Formen, Adern, Muskelzeichnung, wenig Korn.
  K5c hat 1,5 Farben je Teil bei doppelter Hochfrequenz: je Teil eine Farbe mit Korn darauf. Der Kiefer ist eine Flaeche Rauschen um einen Mittelwert (HF 0,46, Chroma-Streuung 2,1).
- **"Hoeher aufgeloest" liegt nicht an der Texeldichte**: K5c hat je Einheit weniger Texel als EM30 (0,042 gegen 0,067; Koerper 0,038).
  Den Eindruck machen Rauschen je Texel und fehlende gemalte Form. Das deckt sich mit `varianten/VARIANTEN.md` Abschnitt 1/5.
- **Glanz**: 12-mal so viele weisse Texel wie EM30, alle neutral silbrig.

## 4. Render-Gegenprobe (B_6, `render_mass.json`)

| | 3/4 vorn | Seite | 3/4 hinten |
|---|---|---|---|
| EM30 roh: L* / C* / L*>70 / L*>85 | 24,5 / 10,0 / 0,04 % / 0,00 % | 29,0 / 12,5 / 0,43 % / 0,00 % | 26,7 / 10,5 / 0,16 % / 0,00 % |
| K5c roh | 19,7 / 4,6 / 1,35 % / 0,83 % | 18,5 / 5,0 / 1,70 % / 1,30 % | 17,2 / 4,0 / 0,23 % / 0,11 % |
| EM30 Gamma 0,5 | 48,2 / 10,0 / 6,8 % / 0,02 % | 52,0 / 11,8 / 13,5 % / 0,19 % | 50,1 / 10,2 / 10,2 % / 0,07 % |
| K5c Gamma 0,5 | 44,7 / 4,6 / 4,9 % / 1,29 % | 43,3 / 5,2 / 4,6 % / 1,58 % | 42,6 / 4,4 / 1,6 % / 0,21 % |

Roh ist K5c dunkler als EM30 (L* -5..-11) und halb so bunt. Seine einzigen hellen Flecken sind silberweiss (L* > 85 bis 1,3 %, EM30 0 %).
Gamma 0,5 verdoppelt L* bei beiden. EM30 wird blassrosa, K5c lavendelgrau mit weissen Flecken, magenta Augen und beigem Kiefer.
Die alten Tafeln (`rundum_*`, Blattlicht 0,46 + 0,86 N*L, Gamma 0,5) zeigten nur die aufgehellte Fassung.

## 5. Nutzeraussagen gegen K5c

| Entscheid | Aussage (Wortlaut-Kern) | Erfuellt K5c? | Messung |
|---|---|---|---|
| E4 (11-01 18:55) | "tail im Screenshot ... eher glatt ... die Texturen fehlen halt so" | ja | Textur vorhanden; Schwanz-Atlas je Glied (Mesh 1-6) L* 15,5/12,1/15,2/15,5/14,0/21,0, Spruenge <= 3,4 bis Glied 5, Spitze +7 (rosa-weisses Ende X) |
| E8 (11-01 21:05) | "Farben und Textur weicht noch weit davon ab ... nicht ... aus einen Guss und psyx style ... hoeher aufgeloester" | nein | Farben je Teil 1,5 (EM30 5,4), HF 0,17 (EM30 0,089), Chroma 6,2 (EM30 14,7) |
| E9 (11-01 21:15) | "Schwanz ... optisch ... eines ... ohne Teilstuecke" | weitgehend ja | E9-Geometrie seit A/B/C uebernommen; Helligkeit je Glied siehe E4; nicht neu im Render je Gelenk gemessen |
| E10 (11-01 21:50) | "A, B, C sind alle in Ordnung" (Grundton) | nein (nicht mehr vorhanden) | C: Lab 13,0/11,6/-4,1, C* 13,3; K5c Koerper 25,5/2,3/-2,7, C* 5,1 (Abschnitt 6) |
| E10 | "Flecken und Haut ... so einfarbig ... Haut hat ... mehr als eine Farbe ... Wunden ... richtig fleischig ... billig einfarbig bemalt" | nein | 1,5 Farben je Teil; Rot 0,5 % (EM30 22,3 %); Kette Bild 1 Chroma 0,68-fach, Bild 2 0,65-fach des Originals |
| E11 (11-01 22:10) | "Uebernimm es ... aus den Screenshot ... unterschiedliche Farben in den verschiedenen Bereichen ... Rueckansicht" | teilweise | Bild 1: Zonenfarben in der Kette dE 2,3-7,2, Klassen dicht dran (Abschnitt 2). Bild 2: a* 4-11 zu wenig (ausser Klaue links (W)), Klauen b* 9-19 zu wenig, Kiefer a* 13 statt 24 |
| E12 (11-01 23:00) | "Textur wirkt noch irgendwie kaputt und chaotisch" | nein | HF 0,17, Kiefer 0,46 (EM30 0,089) |
| E13 (11-01 23:25) | "D ... Mischmasch bei Pixeln ... Farben FAST in die richtige Richtung" | Mischmasch: teilweise; Farben von D: nein | D: C* 18,1, Rot 22,9 %, a* 12,4; K5c: 5,1 / 0,5 % / 2,3. Der Verlust kam mit F (Abschnitt 6) |
| E14 (11-01 23:55) | "WO kommen ... die roten Pixel ... auf den weissen Fleck? Die gibt es ... im Screenshot ueberhaupt nicht" | ja | Bild 1 Kette R 2,2 % gegen Original 2,1 %, nur Z1 und Schwanzende (B_4); Vorderglied/Beine R 0 % wie das Original |
| E15 (11-02 00:10) | "Farben ... einigermassen ... komplett gezeichnet ... cell shading ... passt nicht zu den restlichen Stil des spiels" | Cel: ja (weg); Stil: nein | Hartkanten-Flaechen weg (H2-Material). Dafuer Rauschen statt Malerei: HF 2-fach EM30, 1,5 Farben je Teil |
| E16 (11-02 00:30) | "sehr viel Besser ... H1 oder H2" | ja (H2 im Koerper) | Koerper J = H2-Stand: Lab 25,5/2,3/-2,7 (H2 21,9/2,5/-2,3) |
| E19 (11-02) | "Zu Variante J, sieht doch gut aus" | ja (Koerper unveraendert) | Koerper K5c = K4 = J (Vertex-Diff K5.md Abschnitt 9); Texel Koerper K5c/K4: T2 1,69/1,86, T5 0,151/0,150, T6 5,1/5,4 (Unterschied nur Fusstextur R9) |
| E21 (11-02, Orchestrator) | beiger Unterkiefer bleibt, "im Spiel macht Licht und Videofilter daraus das rote Band" | nein | Kette Kiefer a* 13 gegen Original 24 (Albedo a* 4); Band ist Oberflaeche (Abschnitt 2, Kiefer) |
| E22 (11-02) | "Die Textur vom Vierbeiner wirkt immer noch kaputt" | nein | wie E12; dazu 507 silberweisse Texel, magenta Augen |
| E26 (11-05) | "diese langen hellen Dinge waren keine Fusssohlen, sondern gehoerten zum Boden" | ja | Fusstextur ohne Tuerkis: Mesh 25 Lab 27,7/1,2/-6,8, Mesh 27 16,1/1,2/-6,1 (bau.json fuss_textur), R9 0 Bodenpixel |
| heute (11-06) | "passt ... von den Farben usw. her ueberhaupt nicht und von der Struktur" | nein | Chroma 6,2 gegen 14,7 (EM30); 1,5 gegen 5,4 Farben je Teil; HF 0,17 gegen 0,089 |
| heute | "passt auch nicht zum Rest des spieles" | nein | B_6: EM30 roh C* 10-12,5, warm (a* 7-10); K5c C* 4-5, neutral (a* 2,4-3,1), silberweisse Flecken 0,1-1,3 % (EM30 0 %) |
| heute | "sieht eher aus wie ein Pokemon" | nein | Pastell durch Gamma 0,5 (L* 43-45 bei C* 4,6), magenta Augen C* 52, glatter beiger Kiefer ohne Fleisch (1 Cluster), weisse Flecken. Kopfform siehe K5.md, hier nicht gemessen |

**A/B/C damals und was davon in K5c steckt.** E10: "A, B, C sind alle in Ordnung" (nur der Grundton). Der Orchestrator nahm C als Basis.
Danach wurde mit E11 auf Screenshot-Projektion umgestellt (D). Fortgefuehrt wurden D -> F -> G -> H2 -> I -> J -> K4/K5c. Was davon heute in K5c ist:
- **Grundton C**: nein. C ist pflaumen-rotbraun (Lab 13/11,6/-4,1), K5c neutralgrau (25,5/2,3/-2,7).
- **E9-Schwanz** (in A-G gebaut): ja.
- **Farben aus dem Screenshot** (D, E11/E13): nur fuer Bild 1. Der Farbton von Bild 2 wurde in F verworfen, gemessen a* 12,4 -> 2,4.
- **Zeichnung treu (G, E14)**: ja, Z1 pink, Z5 ohne Rot.
- **H2-Material (E16)**: ja, Koerper.
- **Glanzpunkte aus D (E10-Umsetzung "helle feuchte Glanzlichter")**: als silberweisse Texel noch da. Das hatte der Nutzer nie verlangt.

## 6. Farbverlauf ueber die Varianten (`historie.json`, Koerper = alle Meshes ausser 21-23)

| Variante | Lab Mittel | Chroma | Rot % | L*>85 % | Farben je Teil | HF |
|---|---|---|---|---|---|---|
| A Rueckrechnung | 12,8 / 13,0 / 0,1 | 13,7 | 16,1 | 0,22 | 2,1 | 0,135 |
| B Screenshot | 13,1 / 10,8 / -8,3 | 15,2 | 5,8 | 0,22 | 3,3 | 0,138 |
| **C Mitte (gebilligt E10)** | 13,0 / 11,6 / -4,1 | 13,3 | 15,0 | 0,22 | 2,7 | 0,137 |
| **D Fleisch (E13 "FAST richtig")** | 23,3 / 12,4 / 6,3 | 18,1 | 22,9 | 0,92 | 4,6 | 0,205 |
| F Sauber | 22,0 / 2,4 / -2,0 | **4,6** | **0,6** | 0,61 | 1,6 | 0,091 |
| G treu | 22,1 / 2,3 / -2,1 | 4,6 | 0,6 | 0,69 | 1,5 | 0,066 |
| H2 | 21,9 / 2,5 / -2,3 | 4,7 | 0,7 | 0,74 | 1,6 | 0,132 |
| J | 24,3 / 2,3 / -3,1 | 5,2 | 0,6 | 0,90 | 1,7 | 0,108 |
| K4 | 26,7 / 2,1 / -3,0 | 5,4 | 0,5 | 0,74 | 1,9 | 0,150 |
| K5c | 25,5 / 2,3 / -2,7 | 5,1 | 0,5 | 0,62 | 1,7 | 0,151 |

Der Bruch liegt bei D -> F. F nahm den Farbton nur aus Bild 1 ("Bild 2 nur Helligkeit, Farbton aus Bild 1", VARIANTEN.md Abschnitt 9) und fasste je Teil per k-Mittel zusammen.
Bild 1 liest neutral (Figur a* 3), Bild 2 rot (a* 20). farbe.md mittelt beide zu dunklem Rotbraun (Grundhaut (44,18,27) = Lab 9/15/0, Rotbraun (103,63,84) = 32/21/-5).
K5c liegt am neutralen Ende von Bild 1. Dazu ist die Kanalbalance von Bild 1 nur geschaetzt (+-8 %, farbe.md Abschnitt 3).

## 7. Abweichungen K5c gegen Vorlage (Messung, nach Gewicht)

1. Haut-Farbton: Koerper a* 2,3, Chroma 5,1. Vorlage: Bild 2 Kette a* 19-22 (K5c 15-16). farbe.md-Material Rotbraun a* 21. EM30 a* 10-12. Gebilligt waren C (a* 11,6) und D (a* 12,4).
2. Unterkiefer: einfarbig beige Lab 63/5/14 (1 Cluster, HF 0,46). Vorlage: lachsrotes Fleischband Lab 54-64/30-32/11-16 mit weissen Zahnflecken 78/1/8. Rot ist Oberflaeche (Mantel-Bezug).
3. Silberweisse Texel: 507 (0,6 %), L* 97 neutral, auf Rumpf/Armen/Hals. Vorlage: keine silbrigen Flecken (T L* > 85 0,13 % in Bild 1), helle Baender L* 51-58 grau. EM30: 0,05 %.
4. Augen magenta, Textur (200,93,157), Lab 55/50/-15, C* 52. Vorlage: im Bild nicht als Farbe lesbar; Eichziel war das hellste Drittel (82,36,52) = Lab 21/23/0 im Render.
5. Farben je Teil 1,5 gegen 5,4 (EM30); Rot 0,5 % gegen 22,3 % (EM30). Vorlage Bild 1: 2,1 % rot, Bild 2: 16 %.
6. Hochfrequenz 0,17 (Koerper 0,15, Kiefer 0,46) gegen 0,089: Korn statt gemalter Form.
7. Klauen Bild 2: Kette Lab 27-31/13-23/3-10 gegen 32-35/22-24/19-22. Der Unterschied ist zum Teil Feuerschein (helle Stellen gelb, b* +16..22), also nicht sicher Oberflaeche.
8. Bild 1 (Rueckansicht): in der Kette nah am Original. Ruecken/Flanke hell zu viel: H 17 % gegen 8 %, W 3,9 % gegen 0,7 %; Ruecken L* 31,4 gegen 24,3.

## 8. Offen

- Wahrer Farbton der Grundhaut zwischen Bild 1 (neutral) und Bild 2 (rot). Die Kanalbalance von Bild 1 ist nicht gemessen (farbe.md Abschnitt 7).
  Mit dem Mantel-Bezug ist Bild 2 am Kiefer neutral. An dunklen Stoffen sind dort bis +21 Rot Einstreuung moeglich.
- Klauen Bild 2: Anteil Feuerschein am Orange nicht getrennt (keine neutrale Referenz direkt an den Klauen).
- Augenfarbe des Originals: im Video nicht als Farbe ablesbar (Augen bis 8 px, Unschaerfe 2 px).
- T8 fuer G4/G5 und die Stilmasse der uebrigen Familie (G5, Zombie, Hund): Leser A.
- Die Klassen-Schwellen sind eine Lesewahl. Die Aussagen (Chroma, a*, Rot-Anteil, L* > 85) haengen nicht an ihnen.
