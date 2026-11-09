# Messblatt K5f: nicht blass, wie die Gegner des Spiels (Messer, 2026-11-09)

Auftrag (Nutzer 2026-11-09 nach dem Spielen von K5e): "Und der 4 Beiner sieht von der Textur her jetzt absolut furchtbar aus. Komplett
augeblasst und richtig scheiße.... Null wie die restlichen Gegner im Spiel." Nachtrag: "Und auf der Unterseite am Hals hat er komische rote
streifen, die sollten da auch nicht sein."

Regeln: Memory `reai-v2-texturen-nicht-aus-video-eichen` mit den Nachtraegen 2026-11-08 und 2026-11-09. Es gibt keine Eichung je Region,
je Fleck oder je Texel. Helligkeit wird nur multiplikativ gesetzt. Ruhe hat ein zweiseitiges Band.

Hier wurde nur gemessen und geplant. Es gibt kein Asset und keine Varianten-Datei im Baum.

- Die K5f-Vorschau (v2 bis v7) liegt nur im Scratchpad: `.../scratchpad/k5f/vorschau/`.
- Sie wurde dort gebaut, im echten Spiel ueber `RE15_RE2_ASSET_ROOT` geladen und gemessen.
- Das Spiel-Asset ist unveraendert. Nach der Messung nachgerechnet: `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` sha256 f277916d7d6518b5...,
  `.TIM` 31a35e66428bac1e...

## 0. Ergebnis zuerst

### 0.1 "Blass" in Zahlen

K5e_f1 ist nicht zu hell, sondern zu flach. Gemessen wurde im echten Spiel, 5080 in der Bild-1-Lage, Framedump des echten Renderers.

| Mass | K5e_f1 | Originale (Spanne) |
|---|---|---|
| Figur-L\* | 9,45 | 5,6-18,8 (liegt mitten im Band) |
| Kontrast L-cv (Std/Mittel) | **0,41** | 0,48-1,43 |
| Lokalkontrast 5 x 5 relativ | **0,17** | 0,24-0,66 |
| Buntheit C\* | **4,33** | 4,41-7,59 |
| C\*-Streuung | **1,76** | 2,39-7,45 |

Die Originale sind Zombie 1140, Gorilla 11C0, EM30-Hangar und Tyrant/EM2B 50A0, jeweils im eigenen Raumlicht.

Ohne Raumlicht (Albedo-Quotient Normal/Schluessel) sieht es genauso aus:

| Mass | K5e_f1 | Originale |
|---|---|---|
| L-cv | **0,24** | 0,37-1,02 |
| Lokalkontrast | **0,12** | 0,21-0,53 |
| C\* | **11,5** | 12,1-15,1 |
| C\*-Std | **3,98** | 6,53-8,88 |

Unter weissem Licht ebenfalls:

| Mass | K5e_f1 | Originale (alle 9) |
|---|---|---|
| L-cv | **0,29** | 0,41-1,13 |
| Lokalkontrast | **0,15** | 0,18-0,46 |
| C\*-Std | **3,14** | 4,78-13,55 |

Ursache ist die additive Hebung +21,4 L\*. Sie staucht die G4-Hautrampe p10..p90 = 6,1..19,1 (Faktor 3,1) auf 27..40 (Faktor 1,5). Dazu
kommt EIN Lab-Punkt h -26,3 / C\* 11,68 fuer alle Hauttexel: Die Buntheit haengt dann nicht mehr an der Helligkeit, also C\*-Std 3,1.

### 0.2 Rote Streifen unter dem Hals

**Ort:** Es sind 6 Flaechen von Mesh 22 (Kopf), Viereck qf 1, 8, 9, 194, 201 und 209. Sie liegen aussen an der Kopf-Unterseite und am
Halsansatz und wurden als "Maulinneres" auf die G4-Zeile 0 gelegt. Ihre Eintraege sind G4-Mundfleisch:

| Eintrag | Lab |
|---|---|
| 196 | 38,8/49/12 |
| 37 | 23,9/41/28 |
| 208 | 38,8/48/18 |
| 244 | 38,8/52/28 |

**Messung weisses Licht:** Kennungs-Render von unten, unten-seitlich und unten-3/4. 2514 warm-rote Pixel (a\* > 12, C\* > 15, Ton > -10 Grad)
auf Hals, Kopf und Kiefer, **alle** aus Zeile 0, Material Maulinneres.

**Messung im Spiel:** 5090, Pose Clip 27 Bild 20, Schluessel nur auf Zeile 0. 846 von 963 Zeile-0-Pixeln sind warm-rot.

**Sichtbarkeit:** Die 6 Flaechen sind in einem Rundum-Lauf (12 x 6 Winkel, Clip 2 und 0) 705-2879 Pixel gross. Die uebrigen 29
Maulinneres-Flaechen kommen auf hoechstens 38 Pixel, liegen also wirklich innen.

**Herkunft:** Der Fehler stammt aus der Materialliste (Messblatt RUHE 3.4, `material.json`: "Zeile-0-Flaechen ohne Saum und Augen"). K5c hatte
dieselbe rote Stelle, dazu Lippensaum und Kopfzeile 14 (zusammen 7985 rote Pixel), auf dunkler Haut nur weniger auffaellig.

**Plan:** Die 6 Flaechen werden Haut (G4-Seite 0, Hautzeile 1). In der Vorschau v7 bleiben 0 warm-rote Pixel aussen; die 14 Pixel im Maulspalt
gehoeren zum echten Maulinneren. Im Spiel an derselben Pose: 0 von 963.

### 0.3 Zielband aus den Originalen im Spiellicht

Die Baender stehen in Abschnitt 2.

- **K5e faellt raus** in Kontrast, Lokalkontrast, C\* und C\*-Std (P und Q) und beim Hals.
- **K5c faellt raus** in der Ruhe (R1-R4 weiss: 6 / 16,0 / 88,1 / 26,4), in L\* (2,77 zu dunkel) und in C\* (P und Q).

### 0.4 Materialplan K5f (Abschnitt 3)

- **Haut:** EIN Farbton und EINE Chromatizitaet je Material, die Helligkeit kommt multiplikativ aus der G4-Originalrampe:
  L\*' = knie(f x L\*), C\*' = s x L\*', Farbton h. Die Werte:
  - h = -22,9 Grad: G4-Hautton, C\*-gewichtet ueber alle Haut- und Gelenk-Texelzugriffe der Form
  - f = 2,46: kleinster Faktor mit Bild 2 Video >= Vorlage 15,37
  - s = 0,52: kleinste Buntheit mit P-C\* >= 4,41 und Q-C\* >= 12,09 im Spiel; 0,47 war zu knapp
- **Adern:** rangtreu dunkler als die Haut (RV6).
- **Wuensche:** als ganze Materialflaechen, Fleisch mit konstantem C\* 18 und Ton -8 Grad. Neu ist "Grau" fuer Lippe, Unterkiefer und
  Krallen mit C\* <= 4,8.
- **Zeilen:** 12 (0-11), Port-Texturhoehe 3072.
- **Vorschau v7 im Spiel:** liegt in jedem bindenden Band, ebenso in Ruhe, Wuenschen, Hals und Helligkeit (Tabelle 4.6).

### 0.5 Offen

Fuenf Punkte, alle in Abschnitt 6:

1. **Ton:** -22,9 Grad (G4) ist mauve. Neben dem "Pokemon"-Satz zu K5c verlangt das ein Bild fuer den Nutzer.
2. **Bild 1** bleibt 2,4-mal so hell wie die Vorlage.
3. **G5 und G1** sind im Spiel nicht ins Bild gekommen.
4. **Knappe Raender** bei C\* (P 4,45 gegen 4,41, Q 12,37 gegen 12,09).
5. **F1-Feld:** 6,9 L\* unter der Nachbarhaut. Das ist kein schwarzer Ring wie bei K5e (-21,2), aber dunkler als bei K5e_f1 (-2,3).

## 1. Messung "blass" in Zahlen

### 1.1 Verfahren

**Spiel** (`skripte/lauf.sh`, `alle*.sh`):
- Eigene exe-Kopie je Lauf (`re15_pc_k5f_<name>_<pid>.exe`), echter Renderer, kein AUTOSHOT, kein SOFTWARE_RENDER.
- `RE15_FRAMEDUMP` = SDL_RenderReadPixels des Fensters, 640 x 480. Ende per `RE15_EXIT_AT` bzw. nur ueber die eigene PID.
- Die Laufordner unter `re15_port/build/form_spiel/k5f_*` sind danach geloescht; die Bilder liegen im Scratchpad unter `spiel/`.

**Figurmaske:**
- Zwei Laeufe mit identischen Umgebungsvariablen: Normal und Schluessel.
- Im Schluessel-Lauf ist jede CLUT-Farbe != 0 durch 0x7FFF ersetzt, das STP-Bit bleibt. Pixel, UV und Geometrie bleiben gleich.
- Maske = Pixel, die sich unterscheiden (`skripte/schluessel.py`). Beide Laeufe gehen ueber denselben Ladeweg:
  - RE1.5-Typen als Split-Datei `EMD/EM%02X.EMD` ueber `RE15_CD_ROOT` (main.c `pc_enemy_read_re15_emd` liest sie vor dem EMS-Blob)
  - EM30_HANGAR.EMD ebenso
  - RE2-EM36 als eigene CDEMD0.EMS ueber `RE15_RE2_ASSET_ROOT`
  - die Form als BAHNSTEIG_FORM.EMD/.TIM
- **Determinismus gemessen:** Normal gegen Normal (1140, 11 Bilder) ergibt 0 Pixel Unterschied, Schluessel gegen Normal 7780-8784 Pixel.
- Fuer die Vorschau-Laeufe dient die Maske des K5e_f1-Paares (gleiche Geometrie und Pose; nur die CLUT ist anders).

**Zwei Lesarten je Bild:**
- **P** = Spielpixel, also das, was der Spieler sieht.
- **Q** = Albedo-Quotient normal/key x 255. Der Schluessel-Lauf liefert das Raumlicht je Pixel (Weiss x Licht); Q rechnet es heraus und macht
  die Raeume vergleichbar. Es zaehlen nur Pixel mit key-Mittel >= 24.

**Weisses Licht:** `rl.weiss_id` (Tafel-07-Licht 0,55 + 0,45 max(0, N.L), 380 px, 4 Winkel). Clip 0 fuer die Originale, Clip 2 fuer die Form.

**Masse** (`skripte/fam.py`, Lab = OpenCV float wie Messblatt RUHE; Figur um 2 px erodiert):

| Mass | Definition |
|---|---|
| L-Mittel, L-Std, **L-cv** | Std / Mittel |
| **lokal5** | mittlere L\*-Std im 5 x 5-Fenster, nur Fenster ganz in der Figur |
| **lokal5_rel** | lokal5 / L-Mittel; macht Raeume und Figurgroessen vergleichbar |
| C\*-Mittel, C\*-Std | |
| dunkel % / hell % | L\* < 10 / > 50 |
| R1-R4 | S5, F_fremd, Spanne 90 %, Block-Farbton (`ruhe_mass.masse` unveraendert) |

**Raeume und Szenen:**

| Gegner | Raum, Szene | Bilder |
|---|---|---|
| Zombie EM10/11/16 (Hybrid: RE1.5-Geometrie und -TIM) | 1140, Bilder 300-900/60 | 11 |
| Gorilla EM27 | 11C0 | 3 mit Figur |
| EM30 Hangar | 50A0 Schnitt 4 (`RE15_FORCE_CUT=4`; Schnitt per Projektion der Spawn-Lage durch die RID gewaehlt, `skripte/schnitte.py`) | 11 |
| Tyrant/EM2B | ebenso | 11 |
| Form K5c, K5e_f1, Vorschau | 5080 Bild-1-Lage (Pose -20711,-15984,756,0,11,1) und 5090 Bild-2-Pose (941,-23776,3874,12,0,11), wie Tafel 04/08 | je 5 |

### 1.2 Spiel, 5080 (die Form) und Raumlicht der Originale

`messung/spiel_a.json` ... `spiel_e.json`; key-Mittel = Raumlicht auf Weiss (0..255).

| Figur | Licht | P L\* | P L-cv | P lokal_rel | P C\* | P C-std | Q L\* | Q L-cv | Q lokal_rel | Q C\* | Q C-std |
|---|---|---|---|---|---|---|---|---|---|---|---|
| Zombie 1140 | 77 | 8,04 | 0,481 | 0,273 | 4,41 | 2,39 | 32,1 | 0,369 | 0,213 | 13,32 | 6,53 |
| Gorilla 11C0 | 111 | 7,38 | 1,142 | 0,380 | 5,74 | 5,87 | 18,9 | 0,663 | 0,317 | 14,56 | 8,68 |
| EM30 50A0 | 122 | 18,80 | 0,849 | 0,243 | 7,59 | 7,45 | 40,3 | 0,504 | 0,210 | 15,05 | 8,61 |
| Tyrant 50A0 | 122 | 5,57 | 1,431 | 0,656 | 5,47 | 6,07 | 11,7 | 1,020 | 0,528 | 12,09 | 8,88 |
| **K5c** 5080 | 83 | **2,77** | 0,816 | 0,297 | **1,93** | **2,27** | 11,0 | 0,757 | 0,306 | **8,38** | **9,02** |
| **K5e_f1** 5080 | 83 | 9,45 | **0,409** | **0,168** | **4,33** | **1,76** | 34,0 | **0,238** | **0,116** | **11,51** | **3,98** |
| K5f v2 f2,5 (G4-Chromatizitaet s 0,284) | 83 | 9,06 | 0,752 | 0,333 | **3,01** | 2,56 | 31,3 | 0,565 | 0,276 | **8,02** | **5,74** |
| K5f v5 (s 0,47) | 83 | 9,02 | 0,758 | 0,331 | **4,33** | 3,01 | 31,2 | 0,564 | 0,273 | **11,91** | 6,46 |
| **K5f v7 (Plan)** | 83 | 8,91 | 0,771 | 0,337 | 4,45 | 3,23 | 30,8 | 0,577 | 0,278 | 12,37 | 6,97 |

- **5090 (nur Info):** Das Feuer liegt vor der Figur und mischt sich in die Maske (Q-Spanne 72-89 Grad bei allen Formen). Werte P L\* / L-cv / C\* / C-std:

  | Form | L\* | L-cv | C\* | C-std |
  |---|---|---|---|---|
  | K5e_f1 | 11,9 | 1,03 | 10,5 | 17,5 |
  | K5c | 9,5 | 1,57 | 12,6 | 20,8 |
  | v7 | 11,0 | 1,19 | 10,1 | 17,8 |

  Q lokal_rel: K5e_f1 0,117, K5c 0,225, v7 0,255.
- **Ruhe im Spiel (Info):** Kleidung (Zombie, EM30) gibt grosse Spannen, im Spiel P-Spanne Zombie 144 / EM30 80 Grad. Darum bleibt die Ruhe
  am weissen Licht und an Haut-Originalen (Abschnitt 2.2).

  | Figur | P-Spanne | Block |
  |---|---|---|
  | Gorilla | 54 | 9,6 |
  | Tyrant | 42 | 0,0 |
  | K5c | 43,5 | 16,9 |
  | K5e_f1 | 29,4 | 9,7 |
  | v7 | 19,9 | 5,3 |

### 1.3 Weisses Licht (Mittel der 4 Winkel; `messung/weiss_fam.json`, `weiss_var_*.json`)

| Figur | L\* | L-cv | lokal_rel | C\* | C-std | hell % | F | F_hell | S5 | F_fremd | Spanne | Block |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| G4 (RE2-Original) | 19,1 | 0,796 | 0,268 | 12,28 | 13,55 | 5,2 | 104 | 40 | 4,0 | 3,45 | 61,8 | 13,1 |
| EM30 ganze Figur | 27,5 | 0,587 | 0,258 | 11,08 | 7,27 | 9,2 | 108 | 43 | 4,25 | 11,0 | 132 (Jeans) | 30,1 |
| Zombie EM10 | 22,5 | 0,409 | 0,175 | 9,81 | 4,78 | 1,5 | 51 | 10 | 2,75 | 7,0 | 73 (Uniform) | 30,7 |
| Gorilla EM27 | 15,2 | 0,729 | 0,341 | 11,36 | 7,04 | 1,1 | 117 | 34 | 4,75 | 1,8 | 55,1 | 9,9 |
| G1 EM30 (STAGE3) | 25,9 | 0,621 | 0,256 | 10,70 | 7,26 | 7,9 | 97 | 36 | 5,25 | 17,7 | 162 (Hose) | 38,6 |
| Tyrant EM2B | 7,4 | 1,066 | 0,460 | 8,63 | 5,92 | 0,0 | 42 | 7 | 4,0 | 0,0 | 45,8 | 7,2 |
| Hund EM20 | 7,5 | 1,128 | 0,267 | 8,80 | 10,18 | 0,0 | 34 | 8 | 3,25 | 0,6 | 40,2 | 11,6 |
| G5 RE2-EM36 (nur Knochen 0/1) | 34,9 | 0,455 | 0,176 | 23,41 | 12,08 | 17,8 | 155 | 72 | 5,0 | 1,6 | 54,2 | 12,7 |
| **K5c** | 8,8 | 0,862 | 0,400 | 10,87 | 8,93 | 0,6 | 88 | 13 | **5,75** | **16,0** | **88,1** | **26,4** |
| **K5e_f1** | 24,7 | **0,291** | **0,147** | 8,80 | **3,14** | 0,6 | 39 | 22 | 1,5 | 0,0 | 3,3 | 4,4 |
| K5f v1 f2 (Ton je G4-Eintrag) | 15,8 | 0,72 | 0,38 | 4,59 | 5,47 | 1,9 | | | 3,75 | **12,98** | **93,3** | **29,0** |
| **K5f v7 (Plan)** | 20,1 | 0,68 | 0,37 | 8,17 | 5,68 | 3,7 | 125 | 58 | 3,0 | 0,0 | 21,1 | 3,8 |

**Gegenprobe Farbton je G4-Eintrag (v1):**
- Mit dem G4-Farbton und der G4-Buntheit je Eintrag (L\*, a\*, b\* x f) wird die Haut unruhig: F_fremd 13-22, Spanne 92-116, Block 22-32.
- Die G4-Hauteintraege der drei Seiten haben verschiedene Toene: Seite 0 -60,4 Grad, Seite 1 -18,2 Grad, Seite 2 +0,9 Grad (`skripte/g4rampe.py`).
  Die K5c-UV legt diese Seiten nebeneinander.
- Einzelne "Haut"-Eintraege ausserhalb des Ader-Tonfensters sind magenta-rot (C\* bis 30).
- Darum: ein Ton je Material (Ruhe) und die Buntheit proportional zur Helligkeit (Kontrast). Das ist v2 bis v7.

## 2. Zielband "wie die restlichen Gegner"

### 2.1 Kontrast und Buntheit im Spiel

**Herkunft:**
- Bindend ist die Form in 5080 Bild-1-Lage, Bilder 360-440.
- Das Band ist min..max der vier Originale im eigenen Raumlicht: Zombie 1140, Gorilla 11C0, EM30 50A0, Tyrant 50A0.
- Keine Erweiterung und kein Faktor; die Originale selbst sind die Grenze.
- 5090 ist nur Info (Feuer).

| Zeile | Band (min..max Originale) | K5e_f1 | K5c | v7 (Plan) |
|---|---|---|---|---|
| Z1 P L\* | 5,57..18,80 | 9,45 | **2,77 ROT** | 8,91 |
| Z2 P L-cv | 0,481..1,431 | **0,409 ROT** | 0,816 | 0,771 |
| Z3 P lokal_rel | 0,243..0,656 | **0,168 ROT** | 0,297 | 0,337 |
| Z4 P C\* | 4,41..7,59 | **4,33 ROT** | **1,93 ROT** | 4,45 |
| Z5 P C-std | 2,39..7,45 | **1,76 ROT** | **2,27 ROT** | 3,23 |
| Z6 Q L-cv | 0,369..1,020 | **0,238 ROT** | 0,757 | 0,577 |
| Z7 Q lokal_rel | 0,210..0,528 | **0,116 ROT** | 0,306 | 0,278 |
| Z8 Q C\* | 12,09..15,05 | **11,51 ROT** | **8,38 ROT** | 12,37 |
| Z9 Q C-std | 6,53..8,88 | **3,98 ROT** | **9,02 ROT** | 6,97 |

Weisses Licht als zweite Gegenprobe (Band = min..max der 9 Originale aus 1.3):

| Zeile | Band | K5e_f1 | K5c | v7 |
|---|---|---|---|---|
| W-L-cv | 0,409..1,128 | **0,291 ROT** | 0,862 | 0,68 |
| W-lokal_rel | 0,175..0,460 | **0,147 ROT** | 0,400 | 0,37 |
| W-C-std | 4,78..13,55 | **3,14 ROT** | 8,93 | 5,68 |
| W-C\* (Info) | 8,63..23,41 | 8,80 | 10,87 | **8,17** (knapp darunter, darum nur Info) |
| W-hell % / F_hell (Decke) | <= 17,8 / <= 71,7 | 0,6 / 22 | 0,6 / 13 | 3,7 / 58 |

- **W-C\* ist nur Info.** v7 haelt das Spiel-Band (Z4/Z8). Unter weissem Licht fehlen 0,46 C\*, weil Grau und Gelenke mitzaehlen.
  s = 0,55 bringt erst 8,53 (`weiss_var_*`, Gamut-Knie).
- **Begruendung des Bands:** Der Nutzer vergleicht mit dem, was er im Spiel sieht. Darum zaehlen die Spielpixel (P) im Raumlicht.
- **Warum Q dazu:** P haengt am Raum, Q nur an der Textur. Die Form muss in beiden Lesarten wie die Familie sein. Eine Figur, die nur durch
  dunkles Licht "kontrastreich" wirkt, besteht Q nicht.

### 2.2 Ruhe (unveraendert aus Messblatt RUHE, weisses Licht, Haut-Originale G4 / EM30 ohne Kleidung)

**Schwellen:** R1 S5 <= 5, R2 F_fremd <= 7,4 je 10 000 px (Flaeche <= 0,64 %), R3 Spanne <= 76,8 Grad, R4 Block <= 16,3 Grad.

**Weitere Haut-Originale** liegen alle innerhalb (1.3: S5 / F_fremd / Spanne / Block); G5 liegt bei S5 auf der Grenze:

| Original | S5 | F_fremd | Spanne | Block |
|---|---|---|---|---|
| Gorilla | 4,75 | 1,8 | 55,1 | 9,9 |
| Tyrant | 4,0 | 0 | 45,8 | 7,2 |
| Hund | 3,25 | 0,6 | 40,2 | 11,6 |
| G5 | 5,0 | 1,6 | 54,2 | 12,7 |

**Ergebnis:**
- K5c ist ROT in R1-R4 (5,75 / 16,0 / 88,1 / 26,4).
- K5e_f1 ist GRUEN (1,5 / 0 / 3,3 / 4,4), aber ROT in Z2-Z9: das ist die zweiseitige Ruhe.
- v7 ist GRUEN (3,0 / 0 / 21,1 / 3,8).

### 2.3 Hals-Unterseite

**Band:** Die Originale haben am Hals keine Mundfleisch-Eintraege. Deshalb gilt: 0 warm-rote Pixel (a\* > 12, C\* > 15, Ton > -10 Grad,
weisses Licht; im Spiel a\* > 6 wegen des dunklen Lichts) auf den 6 Aussenflaechen.

**Gegenprobe:**

| Messung | K5e_f1 | K5c | v7 |
|---|---|---|---|
| Hals/Kopf/Kiefer gesamt, weiss (5 Ansichten von unten) | 2514 px ROT | 7985 px ROT (Lippensaum 4760, Maulinneres 2129, Kopfzeile 14 987, Band 109) | 14 px, nur echtes Maulinneres im Spalt |
| Spiel 5090 Pose Clip 27 Bild 20, Schluessel nur Zeile 0 | 846 / 963 ROT | | 0 / 963 |

## 3. Materialplan K5f

### 3.1 Grundsatz

**EINE Transformation je Material auf der G4-Originalrampe** der Seite (CDEMD0.EMS Typ 0x34, TIM @0x65e800). Texel, UV und Seite bleiben wie
im Asset K5c/K5e (TIM-Pixel bytegleich), Geometrie = K5d. Je Flaeche aendert sich nur das CLUT-Wort.

- **Helligkeit:** L\*' = knie(f x L\*_G4), f = 2,46.
  - Multiplikativ; der G4-Kontrast bleibt: L-cv weiss G4 0,80, v7 0,68 (das Knie kappt nur die Spitzen).
  - Knie: L > 75 -> 75 + 25 tanh((L - 75) / 25). Es betrifft nur die hellen G4-Eintraege (Krallenspitzen L\* 63-85).
  - Riegel dafuer: W-hell % und F_hell innerhalb der Originale.
- **Farbton und Buntheit:**
  - EIN Farbton je Material und C\*' = s x L\*', also EINE Chromatizitaet, deren Helligkeit das G4-Muster traegt. Das ist dieselbe Form wie
    "Albedo x Licht".
  - Ton h = **-22,9 Grad**: G4-Hautton, C\*-gewichtet ueber 253 964 Texelzugriffe der Haut- und Gelenkflaechen (`skripte/g4ton.py`).
  - Buntheit s = **0,52**. Die G4-Haut selbst hat s = 0,284 (C\* 3,39 / L\* 11,92) und faellt damit im Spiel aus dem Band (v2: P-C\* 3,01,
    Q-C\* 8,02). Die G4-FIGUR hat C\* 12,28 durch die roten Adern, die nach RV6 nicht roeter sein duerfen als die Haut.
  - s ist darum die kleinste Buntheit, mit der die Form im Spiel Z4 und Z8 erreicht: s 0,47 ergibt 4,33 / 11,91 (ROT), s 0,52 ergibt 4,45 / 12,37.
  - Weich an den sRGB-Rand (`kf_g4.r4_cmax`), naechste 15-Bit-Farbe (`kf_k5e.runde15`).

### 3.2 Materialien (ein Satz je Material, `skripte/k5f_vorschau.py` = Referenz-Umsetzung der Vorschau)

**Haut** (Zeilen 1/2/3 = G4-Seite 0/1/2):
- Flaechen: alle uebrigen, dazu die **6 Aussenflaechen Mesh 22 qf 1, 8, 9, 194, 201, 209** (bisher Maulinneres).
- Haut-Eintraege: L' = knie(f L), C = s L', h -22,9. Helle Eintraege: ebenso, durch das Knie.
- Quelle: G4-Rampe; f aus Bild 2 (3.3); s aus dem Zielband Z4/Z8; h = G4-Hautton.

**Adern (RV6)** (Eintraege der Hautzeilen):
- L' rangtreu linear auf [Haut-p10', 0,74 x Haut-p50'], C = 0,61 x s x L', Ton der Haut.
- Quelle: q 0,74 und kappa 0,61 = K5e-Satz (Median der 9 E30-Gruppensaetze). "Nie heller, nie roeter als die Haut": a\* der Ader liegt immer
  unter dem a\* der Haut bei gleichem L.

**Gelenk** (Zeilen 4/5/6, 680 Flaechen, Liste unveraendert):
- Hautabbildung x rg = 1,469 (L), Buntheit x cg = 0,41.
- Quelle: rg = K5d-Gelenk/Nachbar 28,5/19,4 (Nutzer zu K5d "ja, besser"), multiplikativ statt +9,1. cg = K5e-Gelenk-C\* 4,8 / Haut 11,68 ("weisslich").

**Fleisch** (Zeile 7, Schwanzspitze Mesh 6 und Buckel Mesh 10, 12 Flaechen):
- L' = knie(Haut-L' x rf), C\* konstant 18 (Adern 0,61 x 18), Ton -8 Grad.
- Quelle: Ton und C\* = K5e-Satz (E32 "ansonsten passt"). rf = kleinster Faktor mit Spitze gegen Schwanz >= +6,89 und Buckel gegen
  Mesh-10-Haut >= +4,77 (K4 "vorher da"), per Bisektion wie `kf_k5e_global`. Gemessen: rf 1,51 ergibt Buckel +3,57 (ROT), rf 1,60 ergibt +18,65 / +11,11.

**Grau, neu** (Zeilen 10/11 = G4-Seite 0/1; Seite 2 hat keine Flaeche dieses Materials):
- Flaechen: Lippensaum 140, Maul-Band 25, Krallen 252, Unterkiefer aussen = Mesh-23-Hautflaechen; 409 Flaechen.
- Abbildung: Haut-L', C\* = min(0,41 x s x L', 4,8), Ton -22,9.
- Quelle: Nutzer "Lippe ... durchweg grau/blau", "Unterkiefer ... dunkel/grau", Krallen "ohne Rot". Bei s 0,52 haetten Lippe und Krallen auf der
  Hautzeile 6,9 % bzw. 14,8 % Rotklasse (a\* > 15 & C\* > 15, Vorschau v4/v6). 4,8 = K5e-Gelenk-C\*.

**Bytegleich bleiben:**

| Material | Zeile | Inhalt |
|---|---|---|
| Auge + Maulinneres | 0 | 64 + 29 Flaechen, G4-Zeile 0 |
| F1 | 8 | 6 Flaechen, Kern bytegleich K5c-Zeile 5; F11-Haut-Eintraege = Hautzeile 3, Fleischrand = Hauttransformation (wie K5e_f1) |
| Zahn | 9 | 114 Flaechen, K5c-Zeile 7 |

### 3.3 Helligkeit: Zielband gegen Bild 2

**f = 2,46:** kleinster Faktor mit Bild 2 Video (3 Saaten, Pixelmenge V4F 3.1) >= Vorlage 15,37. v7 ergibt 15,37; f 2,45 bei s 0,44 ergab 15,37,
f 2,40 ergab 14,93.

Im Spiel liegt die Form damit im Band Z1: P L\* 8,91 in 5,57..18,80, Q L\* 30,8. **Kein Konflikt**: Bild 2 und Zielband gelten beide.

**Bild 1:**
- Video 43,3 gegen Vorlage 18,29 (K5e_f1 44,0).
- Unveraendert Offen 1 aus Messblatt RUHE; eine Haut mit einem Faktor kann nicht beide Vorlagen treffen.

### 3.4 Zeilenbudget

| Zeile | Inhalt | Flaechen (Vorschau v7, `messung/vorschau_v7_bau.json`) |
|---|---|---|
| 0 | G4-Zeile 0: Auge, echtes Maulinneres | 93 |
| 1 / 2 / 3 | Haut G4-Seite 0 / 1 / 2 | 249 / 1606 / 579 |
| 4 / 5 / 6 | Gelenk | 6 / 360 / 314 |
| 7 | Fleisch | 92 |
| 8 | F1 | 6 |
| 9 | Zaehne | 114 |
| 10 / 11 | Grau G4-Seite 0 / 1 | 81 / 328 |

- Summe 3828; 12 Zeilen <= 16; Port-Texturhoehe 12 x 256 = 3072 <= 4096.
- Die Vorschau legte eine leere Zeile 12 an (Grau Seite 2, 0 Flaechen). Der Bauer laesst sie weg.

## 4. Riegel-Entwurf

Gegenproben:
- K5e_f1 = Spiel-Asset (f277916d / 31a35e66)
- K5c = einbau_k5c
- v7 = Vorschau im Scratchpad

Alle Spielzeilen entstehen aus eigenen Laeufen mit Schluesselmaske (Verfahren 1.1). Die Werte stehen in `messung/spiel_*.json`.

### 4.1 Zielband im Spiel (bindend, 5080 Bild-1-Lage, Bilder 360-440)

| Riegel | Soll | K5e_f1 | K5c | v7 |
|---|---|---|---|---|
| Z1 P L\* | 5,57..18,80 | 9,45 | ROT 2,77 | 8,91 |
| Z2 P L-cv | 0,481..1,431 | ROT 0,409 | 0,816 | 0,771 |
| Z3 P lokal_rel | 0,243..0,656 | ROT 0,168 | 0,297 | 0,337 |
| Z4 P C\* | 4,41..7,59 | ROT 4,33 | ROT 1,93 | 4,45 |
| Z5 P C-std | 2,39..7,45 | ROT 1,76 | ROT 2,27 | 3,23 |
| Z6 Q L-cv | 0,369..1,020 | ROT 0,238 | 0,757 | 0,577 |
| Z7 Q lokal_rel | 0,210..0,528 | ROT 0,116 | 0,306 | 0,278 |
| Z8 Q C\* | 12,09..15,05 | ROT 11,51 | ROT 8,38 | 12,37 |
| Z9 Q C-std | 6,53..8,88 | ROT 3,98 | ROT 9,02 | 6,97 |

### 4.2 Weisses Licht (bindend als zweite Gegenprobe)

| Riegel | Soll | K5e_f1 | K5c | v7 |
|---|---|---|---|---|
| WK1 L-cv | 0,409..1,128 | ROT 0,291 | 0,862 | 0,68 |
| WK2 lokal_rel | 0,175..0,460 | ROT 0,147 | 0,400 | 0,37 |
| WK3 C-std | 4,78..13,55 | ROT 3,14 | 8,93 | 5,68 |
| WK4 hell % / F_hell | <= 17,8 / <= 71,7 | 0,6 / 22 | 0,6 / 13 | 3,7 / 58 |

### 4.3 Ruhe (R1-R4 aus Messblatt RUHE, unveraendert)

| Riegel | Soll | K5e_f1 | K5c | v7 |
|---|---|---|---|---|
| R1 S5 | <= 5 | 1,5 | ROT 5,75 (gepoolt 6) | 3,0 |
| R2 F_fremd | <= 7,4 | 0 | ROT 16,0 | 0 |
| R3 Spanne | <= 76,8 | 3,3 | ROT 88,1 | 21,1 |
| R4 Block | <= 16,3 | 4,4 | ROT 26,4 | 3,8 |
| H4 ein Ton je Material | genutzte Eintraege mit C\* >= 3 je Material <= 15 Grad Spanne | gruen | ROT (88,7 Grad) | Haut, Ader, Gelenk -22,9; Fleisch -8; Grau unbunt |

### 4.4 Hals-Unterseite

| Riegel | Soll | K5e_f1 | K5c | v7 |
|---|---|---|---|---|
| HU1 Zuordnung | Mesh 22 qf 1/8/9/194/201/209 lesen eine Hautzeile (G4-Seite 0), nicht Zeile 0 | ROT (Zeile 0) | ROT (Zeile 0) | Zeile 1 |
| HU2 weiss | warm-rote Pixel auf Mesh 21-23 ausserhalb der 29 echten Maulinneres-Flaechen (5 Ansichten von unten wie `skripte/hals4.py`) = 0 | ROT 2514 | ROT 7985 | 0 (14 px nur im Spalt) |
| HU3 Spiel | 5090 Pose Clip 27 Bild 20, Schluessel nur Zeile 0: warm-rot (a\* > 6, Ton > -10) an den Zeile-0-Orten des K5e_f1 = 0 | ROT 846/963 | | 0/963 |
| HU4 Sichtbarkeit | Rundum-Lauf (`skripte/hals3.py`): jede Flaeche auf Zeile 0 ausser Auge <= 40 px je 72 Ansichten | ROT 6 Flaechen 705-2879 px | | max 38 |

### 4.5 Wuensche (wie Messblatt RUHE 4.3; v7-Werte aus `messung/wunsch_v7.json`)

| Riegel | Soll | v7 | K5e_f1 |
|---|---|---|---|
| W1 Gelenk weiss | dL >= 5 je Winkel, Mittel >= 6 | min 7,04 / Mittel 9,72 | 5,53 / 6,63 |
| W1b Gelenk Albedo | C\* <= 8, L\* >= Nachbar + 6 | 6,51, +14,5 | 5,85, +9,2 |
| W2 Schwanzspitze | Fleischanteil >= 50 %, dL >= +6,89 | 76,6 %, +18,65 | 85,4 %, +8,64 |
| W3 Buckel | Ton -8 +- 15, Anteil >= 40 %, dL >= +4,77 | -8,5, 57,0 %, +11,1 | -13,9, 57,1 %, +5,29 |
| W4 Lippe + Unterkiefer | Rotklasse 0 %, L-p95 <= Haut-p95 | 0 %, 42,8 <= 51,1 | 0 %, 37,1 <= 43,9 |
| W5 Krallen | dE76 zur Hand <= 6, Rotklasse 0 % | 5,54, 0 % | 0,0, 0 % |
| W6 Bild 2 | Video >= 15,37 | 15,37 | 15,48 |
| W7 F1 Kern | bytegleich K5c-Zeile 5 | ja (Vorschau-Code) | ja |
| W8 F1-Rand | Nicht-Kern-F1 gegen Mesh-0-Nachbarhaut (6 px), weiss "Seite" >= -lokal5 der Figur (v7 7,35) | -6,9 (knapp) | -2,3 |

- Gegenprobe W8: K5e -21,2 ROT.
- Bei W5 liegt der Abstand Kralle zu Hand bei 5,54 nahe der Grenze 6, weil die Hand Haut ist (C\* 4,6) und die Kralle Grau (C\* 0).

### 4.6 Technik

| Riegel | Soll |
|---|---|
| T1 | <= 16 Zeilen, Hoehe <= 4096 (Plan 12 / 3072) |
| T2 | 0x0000 nur wo G4 0x0000; Pixel bytegleich K5c-Asset |
| T3 | UV und Seite 3828/3828 = K5c; Geometrie = K5d; EDD/EMR/Morph/28 Clips = Spiel-Asset |
| T4 | Spiel-Asset erst nach Nutzer-Ja (Einbau wie E32) |

## 5. Tafeln und Daten

Alle Tafeln sind roh, ohne Gamma.

- `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/k5f/messung/familie_spiellicht.png`
  - Je Zeile: Zahlen | Spielbild (Framedump, Figur hell, Umfeld x0,35) | Albedo-Quotient | weisses Licht 3/4 vorn.
  - Zeilen: Zombie 1140, Gorilla 11C0, EM30 50A0, Tyrant 50A0, K5c / K5e_f1 / K5f-v7 in 5080 und 5090, G4 und G5 nur weiss.
- `.../k5f/messung/hals_unterseite.png`: K5e_f1 und v7 von unten, unten-seitlich und unten-3/4; die 6 Flaechen gruen markiert; dazu das
  Spielbild 5090 Griff-Pose (K5e_f1 | Zeile-0-Orte gruen | v7).
- `.../k5f/messung/weiss_familie_4winkel.png`: 10 Figuren x 4 Winkel, weisses Licht.

**Daten** in `.../k5f/messung/`:

| Datei | Inhalt |
|---|---|
| `spiel_a.json` | Originale, K5c/K5e_f1, v2 |
| `spiel_b.json` | v3 |
| `spiel_c.json` | v5 |
| `spiel_d.json` | v6 |
| `spiel_e.json` | v7 |
| `weiss_fam.json` | weisses Licht, Familie |
| `weiss_var_*.json` | weisses Licht, Vorschauen |
| `maulinneres_sicht.json` | Sichtbarkeit der Maulinneres-Flaechen |
| `wunsch*.json` | Wunsch-Masse |
| `vorschau_v7.json`, `vorschau_v7_bau.json` | Parameter, Statistik, sha der Vorschau |

**Skripte** in `.../k5f/skripte/` (Python ohne -I):

| Skript | Inhalt |
|---|---|
| `lauf.sh`, `alle*.sh` | Spiellaeufe |
| `schluessel.py` | Schluessel-Assets |
| `spiel_fam2.py` | Spielmasse P/Q |
| `fam.py`, `weiss_fam.py`, `weiss_var.py` | Masse, weisses Licht |
| `k5f_vorschau.py` | Vorschau-Generator, haengt sich an `kf_k5e.bauen` |
| `bild12.py` | Bild 1/2 |
| `wunsch.py` | Wunsch-Masse |
| `hals1-4.py`, `hals_szene.py`, `hals_tafel.py` | Hals-Unterseite |
| `f1ring.py` | F1-Rand |
| `schnitte.py` | RID-Projektion |
| `g4rampe.py`, `g4ton.py` | G4-Statistik |
| `tafel.py` | Tafel |

Die Spielbilder (ppm) liegen nur im Scratchpad:
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f/spiel/`

## 6. Offen

1. **Farbton -22,9 Grad (mauve) neben dem Nutzersatz zu K5c "wie ein Pokemon".**
   - Der Ton ist G4-eigen und liegt nahe an E30 (-26,3).
   - Mit s 0,52 ist die Haut im Albedo deutlich bunter als K5e; im Q-Bild der Tafel wirkt sie lila-rosa.
   - Im Spiellicht 5080 bleibt sie dunkel und dunkelviolett (P C\* 4,45, mitten zwischen Zombie 4,41 und Gorilla 5,74).
   - Vor dem Einbau braucht es ein Spielbild fuer den Nutzer.
   - Alternative ohne neue Farbrichtung: ein waermerer G4-Ton (Seite-2-Haut +0,9 Grad) waere ein Nutzerentscheid.
2. **Bild 1:** Video 43,3 gegen Vorlage 18,29 (wie K5e). Das ist unveraendert Offen 1 aus Messblatt RUHE.
3. **G5 und G1 im Spiel nicht im Bild:**
   - G5 (5090, `RE15_ENDKAMPF_NUR_G5=1`): Spawn bei (-14700,-23350). Versucht wurden 4 Spielerlagen und Schnitt 11 erzwungen; 0 Maskenpixel.
   - G1 (3070): Spawn geparkt bei (30000,30000), story-gesteuert.
   - Probe-Laeufe 4010 und 50B0 (Spawn-Diagnose): 4010 hat NPCs 0x40/0x42, 50B0 hat 0x29 und 0x42, kein 0x30.
   - 1091 (fuer stehende Zombies gedacht) spawnte 7 x Typ 0x26 und keinen Zombie; darum steht nur der liegende Briefing-Zombie 1140 im Band.
   - G5 und G1 stehen darum nur unter weissem Licht im Band (1.3). Naechster Weg: G5 ueber die Rueckstoss-Szene (`kf_k5e_f1_szenen.sh`,
     `RE15_BIRKIN_HP_AT`) mit Schluessel-EMS.
4. **Knappe Raender:**
   - Z4 P C\* 4,45 gegen 4,41 und Z8 Q C\* 12,37 gegen 12,09.
   - W8 F1-Rand -6,9 gegen -7,35.
   - Bild 2 15,37 gegen 15,37.
   - Der Bauer bisektiert s und f mit Spiellauf und haelt alle vier; der Zombie (liegende Briefing-Leichen, 8400 px) setzt die Untergrenzen Z2/Z4/Z5.
5. **Die Q-Lesart in 5090** ist vom Feuer verfaelscht und nur Info. Bindend ist 5080.
