# Messblatt EICHUNG: Farbe der Riesenform ueber Referenzen (Messer, 2026-11-09)

Anlass ist die Frage des Nutzers vom 2026-11-09: "Gibt es denn irgendeine Möglichkeit wie ich dich besser unterstützen kann bei der korrekten
Farbgebung? ... die Rückseite finde ich gibt schon aufschluss über die Farbgebung. Man sieht ja auch bei vlcsnap-2026-10-31-00h28m45s774.png
zum Beispiel auf der linken Seite von dem Wesen Leon. Der ist im Spiel ja sauber vorhanden mit Textur. Von daher müsste man doch irgendwie
dadurch ableiten können wie die Spielfarbe auf dem Screenshot falsch ist und das zurückrechnen können?"

Hier wurde nur gemessen: es gibt kein Asset, keine Variante und keinen Engine-Code. Die Regel aus Memory
`reai-v2-texturen-nicht-aus-video-eichen` gilt weiter. Das Ergebnis sind Zielfarben je Material mit Unsicherheit, keine Texelwerte.
Keine Zahl aus diesem Blatt ist ein RE-Wert oder gehoert in die Engine, denn sie beschreiben die Abfilmung.

## 0. Ergebnis zuerst

1. **Ja, der Weg des Nutzers traegt, aber nur in einer Szene voll.**
   - **Bild 2 (Zugwagen 5090, Frau):** Die Frau ist eine exakte Referenz. 5090 Schnitt 11 hat nur Umgebungslicht (83,78,78), alle drei
     Lichtfarben sind 0 (RDT-Licht @0x55c, `form_buehne.Raum('5090').licht(11)`). Damit gilt Schirmfarbe = Textur x (82,78,78)/255,
     unabhaengig von Pose und Normale. Kittel und schwarzes Oberteil stimmen mit dem Port ueberein, das Haar nicht (Abschnitt 2.3).
   - **Bild 1 (Bahnsteig 5080, Leon):** Leon ist die einzige Referenz. Bild 1 hat keinen Schnitt auf der Disc (kamera.md, Tafel T2 unten),
     es gibt also kein Hintergrund-Pixelpaar. Leon traegt im Video dieselbe RPD-Uniform wie im Port.
2. **Farbmodell und Gueltigkeit** (Abschnitt 3):
   - Ein Modell nur aus dem Raumhintergrund (Kurven + 3x3) **traegt nicht** auf die Frau. Am Hintergrund erreicht es dE76 Median 9,1,
     an der Frau aber 40-53. Der Hintergrund von 5090/11 ist fast nur Orange/Braun, der weisse Kittel liegt ausserhalb seines Farbraums.
   - Gemeinsam gefittet (Hintergrund + Frau, `em_frei`) und an einer ausgelassenen Klasse geprueft ergibt dE76 der Mediane: dunkles
     Oberteil 9,2, Kittel 27,9, Kopf 25,0, Hintergrund rechts/links 9,5 / 13,6 (Pixel-Median).
   - Der Bestand `form_video` (39 Dias, an der Frau nie gefittet) erreicht: Kittel 15,0, dunkel 18,5, Kopf 26,1, Hintergrund 9,4.
   - **Bild 1:** `form_video` mit an Leon gefitteter Kanalbalance EX = (0,951, 0,868, 0,803). Gegenprobe mit ausgelassener Leon-Klasse,
     dE76 der Mediane: Uniform 6,5, dunkel 3,4, Haut/Haar 11,9. Mit dem alten EX aus farbe.md waren es 11,1 / 7,2 / 13,5.
3. **Das Wesen ist im Video zu 75-80 % Streulicht** (Abschnitt 4).
   - Von 24 072 (Bild 1) bzw. 22 669 (Bild 2) Kernpixeln haben 18 988 bzw. 18 046 mehr als 50 % Streulichtanteil aus der Umgebung.
     Gemessen werden kann nur der Rest: 4 550 bzw. 2 475 Pixel.
   - **Messbar:** Haut, Gelenk, F1 (Rosa-Stelle Ruecken) aus Bild 1. Haut, Gelenk, Buckel, Kopf, Hals aus Bild 2.
   - **Nicht messbar:** Schwanzspitze (Bild 1 nur 71 Pixel, alle streulichtdominiert), Krallen (0 Pixel), Lippe/Unterkiefer (Bild 2
     nur streulichtdominiert, Farbton modellabhaengig), Adern (unter der Video-Aufloesung: Unschaerfe sigma 2 px plus Chroma 6 x 3 px).
4. **Zielfarben je Material** (Textur-Einheiten 0..255 wie TIM << 3; Lab D65). Volle Tabelle in Abschnitt 5, Kacheln in Tafel T5.

   | Material | Ziel L\* / C\* / h | Unsicherheit dE | Quelle |
   |---|---|---|---|
   | Haut | 23,4 / 5,7 / -9 Grad | 8,6 | Bild 1 L\* 20,9 + Bild 2 L\* 35,0 |
   | Gelenk | 30,1 / 4,2 / -44 Grad (fast unbunt) | 8,6 | Bild 1 + Bild 2 |
   | F1 Rosa-Stelle | 41,9 / 22,2 / -5 Grad | 11,2 | Bild 1 |
   | Buckel | 36,7 / 24,1 / +1 Grad | 19,9 | nur Bild 2, Rot ist Obergrenze |
   | Hals | 35,4 / 13,9 / +9 Grad | 14,4 | nur Bild 2, Rot ist Obergrenze |
   | Kopf | 33,7 / 21,2 / +21 Grad | 26,1 | nur Bild 2, Rot ist Obergrenze |

   **Bild 1 und Bild 2 widersprechen sich in der Helligkeit.** Bild 2 verlangt fuer Haut und Gelenk rund das 1,5-fache von Bild 1 (G- und
   B-Kanal), mit R das 1,9-fache. Dafuer gibt es zwei gemessene Ursachen, die gegeneinander wirken:
   - In Bild 1 bekommt das Wesen im Port 2,1-mal so viel Licht wie in Bild 2 (Schattierung 172 gegen 80). Der Grund ist L0 aus 5080 Schnitt 2,
     das rund 260 Einheiten neben dem Wesen steht. Leon steht 3 500 Einheiten von L0 entfernt und prueft dessen Staerke darum kaum.
   - In Bild 2 liest jedes Modell dunkle Flaechen zu rot. Am schwarzen Oberteil der Frau (Spiel e = 5) misst die Umkehr R +13 bis +22 e
     (em_frei / Bestand), G und B nur +1 bis +12. Das Wesen steht naeher am Feuer.
5. **Vergleich der Staende** (gleiche Kette, gleiche Pixel; Abschnitt 5):
   - **K5c ist in jedem messbaren Material zu dunkel.** dL\* liegt zwischen -8 (F1) und -24 (Buckel), bei Haut -13 und Gelenk -19.
     In Bild 2 reproduziert K5c das Video am schlechtesten: dE76 Pixel-Median 20,2 gegen 10,7 (K5e_f1) und 11,7 (K5f).
   - **K5e_f1:** Die Helligkeit liegt zwischen den beiden Szenen; gegen Bild 1 ist sie zu hell, gegen Bild 2 zu dunkel (Haut +9,3 / -7,3 L\*).
     Haut-Ton -19 Grad violetter als das Ziel, C\* +4,5. Gelenk C\* passt (+2), ist aber in Bild 1 10,7 L\* zu hell.
   - **K5f:** Haut-Helligkeit am naechsten am Ziel (dL\* +2,9), aber mit C\* +7,2 die buntste Haut und -11 Grad violetter.
     Hals in Bild 2 -13,2 L\* und +7,9 C\* (dE 17,2, K5e_f1 10,5). F1 und Buckel gleichauf mit K5e_f1.
   - **Gemeinsamer Befund aller drei Staende:** Der Haut-Ton ist violetter als das Video (dh -9 bis -19 Grad), F1 ist zu wenig bunt
     (dC\* -6 bis -8). Bild 2 verlangt bei Buckel/Kopf/Hals einen waermeren Ton (h 0..+21 Grad), dort ist Rot aber nur Obergrenze.
6. **Was der Nutzer liefern kann** (Abschnitt 6): Am meisten bringen weitere Standbilder, in denen das Wesen **hell und gross** vor ruhigem
   Hintergrund steht, ohne Feuer, und dazu ein bekanntes Objekt im selben Bild. Auf Platz zwei: eine Bereichsmarkierung Haut/Gelenk/Fleisch
   auf der Rueckseite von Bild 1.

## 1. Bilder (Schritt 1)

`skripte/e1_bilder.py`, Ergebnis `daten/e1_bilder.json`.

- Quelle: `Version_80%/Biohazard 1.5 Final Build.mp4`, 29,97 Bilder/s. Gelesen wurden die Bilder 3380-3540 (ffmpeg select + showinfo).
- Das Video ist eine **Diashow**: Jedes Dia steht 24 Videobilder lang still. Die Nachbarbilder zeigen also exakt dieselbe Szene, mit
  Bewegung null. Andere Posen oder Blickwinkel liefern sie nicht.

| | Bild 1 (vlcsnap ...28m45s774) | Bild 2 (vlcsnap ...29m02s830) |
|---|---|---|
| Dia | 143 | 145 |
| Videobilder | 3432-3455 (24), 114,548-115,315 s | 3480-3503 (24), 116,149-116,917 s |
| vlcsnap = Bild | 3445 (114,982 s), mittlerer Abstand 0,000 | 3483 (116,250 s), 0,000 |
| groesster Sprung im Dia | 0,29 (Kompression) | 0,61 |
| Streuung je Pixel im Dia | 0,18 | siehe json |

- Verwendet wurde das Mittel der 24 Bilder je Dia (`daten/bild{1,2}_mittel.png`, float-Fassung im Scratchpad). Es weicht vom vlcsnap im
  Mittel um 0,06 ab und senkt nur das Kompressionsrauschen.
- Dia 144 (Bilder 3456-3479) liegt dazwischen und zeigt 5090 Schnitt 1 (farbe.md).

## 2. Referenzpaare (Schritt 2)

### 2.1 Raumhintergrund Bild 2

`skripte/e3_hintergrund.py`, `daten/e3_hintergrund.json`.

- Port: `ROOM50911.ppm` (Engine-Dekoder `probe_bg_dump`) ueber `kamera_bild2.json` B in die Video-Bildlage gelegt.
  Das ist bytegleich zu `pruefung/hintergrund_bild2.png` (MAE 0,000).
- **Lage gemessen:** Die ECC-Verschiebung gefilterter Port gegen Video betraegt dx -0,12, dy +1,54 px (ECC 0,882). Die Lage stimmt also.
- **Maske:** 148 323 Pixel. Ausgenommen sind `form_video._MASKE2` (Figur, Frau, Feuer, Logo), `figur_bild2` +15 px, das Logo +5 px,
  Raender und V >= 220 (8 540 Pixel geclippt).
- Roh (Port direkt gegen Video): dE76 Median 32,8, p90 57,7.

### 2.2 Raumhintergrund Bild 1: nicht moeglich

- Keiner der 6 Schnitte von ROOM5080 zeigt die Ansicht von Bild 1 (Tafel T2 unten, Bild 1 neben Schnitt 0-5). Das deckt sich mit
  kamera.md, wo die Suche ueber alle 1119 Hintergruende ergebnislos blieb.
- Ein Pixelpaar gibt es darum nicht. Ein Vergleich von Flaechen derselben Oberflaeche ueber verschiedene Schnitte waere moeglich, aber jeder
  Schnitt hat eigenes eingebackenes Licht. Er wurde nicht gemacht (Offen 3).

### 2.3 Frau in Bild 2

`skripte/e4_frau.py`, `daten/e4_frau.json`.

- **Modell:** Typ 0x4D (`CDEMD0.EMS`), Lage (1166,0,-25211) aus kamera.md. Pose und Versatz per NCC gesucht: Clip 0 Bild 8, Gier 128,
  Versatz (+2,+16) px, NCC 0,738.
- **Licht gemessen:** Die Schattierung ist auf allen Frau-Pixeln (81..82, 77..78, 77..78). Das ist nur Umgebungslicht, wie die Disc sagt.
- **Kleidung geprueft** (Tafel T1 unten):
  - Kittel: im Port Textur (200,208,208). Im Video lavendel-weiss, und genau so faerbt die Abfilmung Weiss (farbe.md). Brauchbar.
  - Schwarzes Oberteil: im Port Textur (16,16,16), im Video dunkel. Brauchbar.
  - Haar/Kopf: im Port blond (Textur (168,128,80)). Im Video ist der Kopf hell lavendel, kein Orange-Blond, obwohl die Abfilmung Blond
    orange zeigen muesste. Jedes Modell liegt hier 20-26 dE daneben. **Als Referenz verworfen:** Entweder ist das Haar anders oder die
    Lage des Kopfes passt nicht (Versatz 16 px).
  - Beine: hinter dem Feuer, nicht verwendet.
- **Klassen** aus der Textur-Albedo, je um 2 px erodiert, ohne V >= 220:
  - Kittel (L\* > 65, C\* < 14): 1 324 Pixel
  - dunkel (L\* < 30): 418 Pixel
  - Kopf (oberste 34 px): 391 Pixel

### 2.4 Leon in Bild 1

`skripte/e4_leon.py`, `daten/e4_leon.json`.

- **Modell:** PL00 (MD1/EMR/EDD/TIM). Der Fusspunkt (125,366) liegt auf dem Boden bei (-17993,0,-14623) (`kamera_bild1.json`).
- **Licht:** 5080 Schnitt 2 (Disc), wie in der Szenenkette der Form. Leon bekommt Umgebungslicht plus rund 27 % von L0, Schattierung
  74..115 (p10..p90).
- **Kleidung geprueft:** dunkelblaue RPD-Uniform, Weste, Gesicht. Im Video dunkelblau mit hellem Gesicht (Tafel T2): gleich.
- **Ausrichtung im Spielraum:** kleinster Abstand zwischen Umkehr(Video) und Render.
  - Eine NCC der Helligkeit fand Leon nicht (NCC ~ 0): Er ist dunkel vor hellem Zug.
  - Gewaehlt: Tiefe 0, Gier 2048, Clip 0, Abstand 41,0 e.
  - Streuung ueber 3 Tiefen (+-1000) und 3 Gierwinkel: EX R 0,95-1,00, G 0,71-0,96, B 0,55-0,84. Die Tiefe -1000 passt deutlich
    schlechter (Abstand 58-63 statt 41-49).
- **Klassen:**
  - Uniform (b\* < -6): 1 854 Pixel
  - dunkel (L\* < 22): 630 Pixel
  - Haut/Haar (b\* > 8): 136 Pixel

## 3. Farbmodell und Gueltigkeit (Schritt 3)

`skripte/em.py`. Die Kette verwendet dieselbe Unschaerfe wie `form_video.video_filter` (Optik sigma 2, Streulicht KA/KB, Chroma 6 x 3),
diese Konstanten wurden nicht neu gefittet:

    L = (e/255)^2,2 ;  X = G(L,2) + KA G(L,32) + KB G(L,128) ;  Y = k * (C X) ;  u = a Y^p + b ;  V = 227 soft(u/227) ; V = chroma(V)

Dazu kommen je Kanal a, p, b und das Uebersprechen C (6 Werte ausserhalb der Diagonale).
Der Fit ist robust (soft_l1, Massstab 6 V): Stufe 1 ohne Chroma an Stichproben, Stufe 2 mit Chroma am ganzen Bild.

### 3.1 Nur Hintergrund Bild 2 (e3)

| Variante | BG dE76 Median / p90 | links fit -> rechts | rechts fit -> links | Frau (e4) gesamt dE76 Median |
|---|---|---|---|---|
| diag (C = I) | 10,7 / 43,5 | 10,8 | 11,4 | 39,7 |
| ridge | 9,1 / 25,3 | 8,8 | 10,8 | 50,0 |
| frei | 9,2 / 25,3 | 8,6 | 10,8 | 53,3 |
| form_video Bestand | 9,4 / 25,5 | - | - | 18,4 |

- **Ergebnis:** Der Hintergrund allein bestimmt das Modell nicht.
  - Ridge und frei laufen an die Schranken des Uebersprechens (+-0,6): Die Farben liegen fast auf einer Linie.
  - An der Frau sagen sie den Kittel (Video (184,115,133)) als dunkles Blau voraus: frei (12,35,43), diag (154,166,139).
- Ein freier Kanal-Fit nur an der Frau (k, e4 Teil B) ergibt k = (17,7, 2,7, 2,4). Das ist entartet und wurde verworfen.

### 3.2 Hintergrund + Frau gemeinsam (e4b), zurueckgehaltene Referenz

`skripte/e4b_gemeinsam.py`, `daten/e4b_gemeinsam.json`. dE76 der Mediane je Klasse; die Klasse ist beim Fit ausgelassen:

| Modell | Kittel | dunkel | Kopf | BG links->rechts | BG rechts->links | BG gesamt (im Fit) |
|---|---|---|---|---|---|---|
| diag | 37,9 | 23,3 | 20,2 | 11,7 | 12,9 | 11,0 |
| **frei (em_frei)** | 27,9 | **9,2** | 25,0 | 9,5 | 13,6 | 11,1 |
| form_video Bestand (nie an der Frau gefittet) | **15,0** | 18,5 | 26,1 | 8,9 | 9,8 | 9,4 |

**Rueckrechnung am bekannten Spielwert** (Video -> e gegen den Port-Wert, sigma-2-unscharf), Median-Bias in e:

| Bereich | em_frei | Bestand |
|---|---|---|
| Hintergrund e 0-15 | +2,9 / +15,0 / +9,9 | +8,3 / -1,0 / +1,0 |
| Hintergrund e 15-30 | -0,3 / -10,9 / -5,0 | +2,9 / -10,2 / -1,6 |
| Hintergrund e 30-50 | +3,2 / -12,0 / -9,7 | +1,0 / +2,9 / +3,6 |
| Hintergrund e 50-80 | -33,5 / +14,2 / +16,7 | -30,8 / +18,3 / +19,4 |
| **Frau dunkel e 0-15** | **+12,7 / +11,5 / +7,4** | **+21,7 / +1,5 / +1,5** |
| Frau Kittel e 50-80 | -1,5 / +2,4 / -0,9 | +13,0 / +8,1 / +0,5 |

- **Gueltigkeit Bild 2:** Fuer dunkle Flaechen (der Bereich des Wesens) liegt der Fehler bei 9-19 dE76. Rot ist in dunklen Flaechen um
  +13 bis +22 e zu hoch. Geteilt durch die Schattierung 0,32 sind das bis +40..+68 in Textur-Einheiten: **Rot ist in Bild 2 eine Obergrenze.**
- **Wahl fuer die Umkehr:** Hauptmodell ist der Bestand.
  - em_frei laesst nur 569 auswertbare Wesen-Pixel uebrig (Bestand 2 475).
  - Im streulichtdominierten Bereich liefern die beiden Modelle gegensaetzliche Farbtoene: em_frei gruen-tuerkis a\* -16, Bestand rot a\* +23.
  - em_frei dient als Zweitmodell, der Abstand der beiden geht in die Unsicherheit ein.

### 3.3 Bild 1 (e4_leon)

| Leon-Klasse ausgelassen | EX an den anderen | dE76 der Mediane (EX an Leon) | mit EX farbe.md |
|---|---|---|---|
| Uniform | 1,000 / 0,850 / 0,834 | 6,5 | 11,1 |
| dunkel | 0,951 / 0,868 / 0,803 | 3,4 | 7,2 |
| Haut/Haar | 0,948 / 0,868 / 0,800 | 11,9 | 13,5 |

- Alle Leon-Pixel: dE76 Pixel-Median 12,1 (EX an Leon) gegen 14,5 (EX farbe.md).
- **Grenze:** Leon prueft vor allem Umgebungslicht. Die Staerke von L0 an der Wesen-Lage, 260 Einheiten von L0, prueft er nicht.

## 4. Umkehr auf das Wesen (Schritt 4)

`skripte/e5_wesen.py`, `daten/e5_wesen.json`, Tafeln T3/T4.

**Kette:**
1. Video -> e (inverses Modell, Streulicht abgezogen).
2. Geteilt durch die Port-Schattierung je Pixel. Dafuer wurde K5e_f1 mit Kennung gerendert: Disc-Licht (5080/2 bzw. 5090/11), Lage aus
   form.json, Mesh/Flaeche je Pixel, an `figur_bildN.png` ausgerichtet.
   - Bild 1: IoU 0,774, Versatz (+1,+1)
   - Bild 2: IoU 0,560, Versatz (+3,-8); die Pose von Bild 2 deckt schlechter
3. Ergebnis ist die Albedo je Pixel in Textur-Einheiten.
4. Material je Pixel aus `ruhe/messung/material.json`. Dazu gilt: Kopf = Mesh 22 ohne Lippensaum, Lippe/Unterkiefer = Lippensaum + Mesh 23,
   Hals = Mesh 21 + die 6 Flaechen aus MESSBLATT_K5F 0.2, F1 eigenes Material, Sonder = Zahn/Auge/Maulinneres.

**Ausschluss** (Pixel; Kern = Figur, um 6 x 3 px gegen den Chroma-Saum erodiert):

| | Bild 1 | Bild 2 (Bestand) |
|---|---|---|
| Figur Render und Video | 30 695 | 27 146 |
| Kern nach Chroma-Saum | 24 072 | 22 669 |
| uebersteuert (V >= 215, +3 px) | 534 | 128 |
| Feuer / Ueberstrahlung (V >= 200, +15 px) | - | 2 020 |
| **Streulichtanteil > 50 %** | **18 988** | **18 046** |
| Schattierung < 20 | 0 | 0 |
| **gueltig** | **4 550** | **2 475** |

Der Streulichtanteil > 50 % ist der Hauptausschluss. Bei 50 % Anteil und +-15 % Fehler des Streulichts (farbe.md) betraegt der Signalfehler
15 %, bei 75 % schon 45 %. Fuer die ausgeschlossenen Pixel stehen zwei Zusatzwerte in `e5_wesen.json`: `ohne_streu_ausschluss` (Schaetzung
mit grossem Fehler) und `obergrenze_ohne_streu_abzug` (Obergrenze ohne Abzug des Streulichts).

**Albedo je Material** (Hauptmodell; Median, p10..p90 je Kanal, Pixel):

| Material | Bild 1 (Rueckseite) | Bild 2 (Vorderseite) | Zweitmodell dE |
|---|---|---|---|
| Haut | (55,49,52), p10 (35,29,32), p90 (109,97,101), 2 010 px, Lab 20,9/3,9/-1,1 | (103,75,83), p10 (87,47,58), p90 (117,113,103), 445 px, Lab 35,0/13,5/-0,3 | 3,4 / - |
| Gelenk | (69,67,73), p10 (35,32,36), p90 (101,97,106), 1 389 px, Lab 28,8/1,7/-3,1 | (118,88,99), p10 (102,53,77), p90 (141,152,146), 165 px, Lab 40,8/13,9/-1,3 | 4,0 / - |
| F1 Rosa-Stelle | (132,85,103), p10 (72,36,46), p90 (183,152,168), 1 149 px, Lab 41,9/22,1/-2,0 | nicht sichtbar | 6,6 |
| Buckel | nicht gueltig (0 px) | (123,72,87), p10 (105,47,66), p90 (155,97,119), 1 058 px, Lab 36,7/24,1/0,3 | 19,9 |
| Kopf | 0 px | (112,67,69), p10 (85,46,47), p90 (145,97,89), 273 px, Lab 33,7/19,8/7,5 | 26,1 |
| Hals | 0 px | (106,75,80), p10 (86,54,65), p90 (126,97,100), 534 px, Lab 35,4/13,7/2,2 | 14,4 |
| Schwanzspitze | 1 px gueltig; ohne Streu-Ausschluss 71 px (20,13,15), Obergrenze (33,25,25) | 0 px | - |
| Krallen | 0 px | 0 px | - |
| Lippe/Unterkiefer | 1 px | 0 px gueltig; ohne Streu-Ausschluss 1 056 px, Farbton modellabhaengig: Bestand a\* +27, em_frei a\* +0,6 | - |
| Adern | unter der Aufloesung | unter der Aufloesung | - |

## 5. Vergleich mit K5c, K5e_f1, K5f (Schritt 5)

`skripte/e7_ziel.py`, `daten/e7_ziel.json`, Tafel T5.

**Verfahren:**
- Jeder Stand wurde an denselben Pixeln durch dieselbe Kette geschickt: Render mit Disc-Licht, Modell vorwaerts, Modell zurueck, geteilt
  durch die Schattierung. Dann wurde er gegen die Video-Albedo verglichen. So sind Stand und Video gleich unscharf.
- Ziel = pixelgewichtetes Mittel der Lab-Mediane beider Szenen.
- Unsicherheit = max(halber Szenenabstand, Modellabstand, L\*-Spanne p10..p90 / 4).

| Material | Ziel Lab (+-dE) | K5c dE / dL / dC / dh | K5e_f1 dE / dL / dC / dh | K5f dE / dL / dC / dh |
|---|---|---|---|---|
| Haut | 23,4 / 5,7 / -0,9 (+-8,6) | 13,1 / **-13,0** / +1,7 / -9 | 8,2 / +6,3 / +4,5 / **-18,9** | 7,9 / +2,9 / **+7,2** / -10,8 |
| Gelenk | 30,1 / 3,0 / -2,9 (+-8,6) | 19,5 / **-19,2** / +2,8 / +26 | 9,4 / +9,0 / +2,1 / +18 | 8,9 / +8,2 / +2,3 / +25 |
| F1 | 41,9 / 22,1 / -2,0 (+-11,2) | 11,6 / -8,0 / -8,3 / +3 | 8,1 / -3,7 / -6,3 / -11 | 7,7 / -3,5 / -6,6 / -6 |
| Buckel | 36,7 / 24,1 / 0,3 (+-19,9) | 25,9 / **-24,2** / -5,3 / -20 | 10,3 / -4,6 / -8,1 / -13 | 8,9 / -5,4 / -5,6 / -12 |
| Kopf | 33,7 / 19,8 / 7,5 (+-26,1) | 23,8 / **-21,4** / -7,1 / -25 | 14,9 / -4,1 / -8,1 / **-42** | 14,8 / -8,3 / -4,7 / **-35** |
| Hals | 35,4 / 13,7 / 2,2 (+-14,4) | 20,7 / **-20,6** / -1,9 / +4 | 10,5 / -7,7 / +0,4 / -29 | 17,2 / **-13,2** / +7,9 / -26 |

**Je Szene Haut** (dE / dL):

| Stand | Bild 1 | Bild 2 |
|---|---|---|
| K5c | 11,4 / -11,1 | 21,3 / -21,2 |
| K5e_f1 | 11,0 / **+9,3** | 8,6 / -7,3 |
| K5f | 10,3 / +6,3 | 13,4 / -12,5 |

**Ganzes Wesen, Video gegen Kette** (dE76 Pixel-Median ueber die gueltigen Pixel):

| Szene | K5c | K5e_f1 | K5f |
|---|---|---|---|
| Bild 1 | 18,6 | 20,8 | 20,2 |
| Bild 2 (Bestand / em_frei) | 20,2 / 27,5 | **10,7 / 8,1** | 11,7 / 9,6 |

**Wo welcher Stand daneben liegt:**
- **K5c:** durchweg 8-24 L\* zu dunkel. Das deckt sich mit dem Messblatt K5f ("K5c faellt raus ... L\* 2,77 zu dunkel").
- **K5e_f1:** Die Helligkeit passt im Mittel beider Szenen.
  - Haut violetter als das Ziel (dh -19 Grad). Kopf -42 Grad und Hals -29 Grad gegen Bild 2, wo Rot allerdings Obergrenze ist.
  - Gelenk in Bild 1 zu hell (+10,7 L\*).
  - F1 zu wenig bunt (-6 C\*).
- **K5f:** Haut am naechsten in L\*, aber am buntesten (+7 C\*).
  - Hals zu dunkel und zu bunt (dE 17).
  - Gelenk wie K5e_f1 zu hell in Bild 1.
- **Nicht trennscharf:** K5e_f1 und K5f unterscheiden sich in Haut, F1 und Buckel um weniger als die Unsicherheit (dE 7,7-10,3 gegen +-8,6
  bis +-19,9). Das Video entscheidet zwischen ihnen nur am Hals, zugunsten von K5e_f1.

## 6. Was der Nutzer liefern kann (Schritt 6)

Nach Wirkung geordnet:

1. **Weitere Standbilder mit Zeitstempel, in denen das Wesen hell und gross ist,** am besten die Rueckseite, ohne Feuer und vor ruhigem
   Hintergrund. Das Video ist eine Diashow mit 174 Dias. Brauchbar sind nur Dias, in denen das Wesen nicht ueberwiegend Streulicht ist
   (hier 75-80 %).
   - Vielleicht zeigen "Resident Evil 1.5 (All Footage).mp4" oder die Disc-Videos 1-4 das Wesen in Bewegung.
   - Bewegtes Material liefert viele Posen derselben Flaeche. Das mittelt Streulicht und Lage-Fehler heraus.
2. **Dieselbe Szene in einer anderen Quelle**, etwa eine zweite Abfilmung oder einen Screenshot desselben Moments aus anderer Hand.
   Unterschiedliche Abfilmungen haben unterschiedliche Kurven. Wo beide dasselbe sagen, ist es das Spiel.
3. **Eine Bereichsmarkierung auf Bild 1 (Rueckseite)**, als Lasso wie bei den PRI-Masken:
   - Haut, Gelenk/Knochen, Fleisch (Schwanzspitze, Buckel), Rosa-Stelle.
   - Dazu eine Markierung "hier ist es Licht, nicht Farbe" (Glanz/Ueberstrahlung).
   - Heute kommt die Zuordnung aus dem K5e-Modell. Bei IoU 0,77 bzw. 0,56 faellt ein Teil der Pixel ins falsche Material.
4. **Bestaetigung der Kleidung im 80 %-Stand:**
   - Hat die Frau in Bild 2 blondes Haar wie im Port, oder hellgraues/weisses? Davon haengt ab, ob der Kopf als dritte Referenz zaehlt.
   - Traegt Leon die normale RPD-Uniform? Gemessen sieht es so aus.
5. **Eine Szene mit Wesen und bekanntem Licht**, zum Beispiel ein Dia in 5090, in dem das Wesen weiter vom Feuer weg steht.
   - Nur in 5090 ist das Licht exakt bekannt (nur Umgebungslicht).
   - In 5080 ist L0 neben dem Wesen der groesste Unsicherheitsposten: Dort verlangt Bild 1 rund 1,5-1,9-mal weniger Albedo als Bild 2.
6. **Kein Bedarf** an weiteren Nachbarbildern derselben Dias. Alle 24 Bilder eines Dias sind identisch (Abschnitt 1).

## 7. Offen

1. **Helligkeit Bild 1 gegen Bild 2.**
   - Bild 1 verlangt rund das 0,5- bis 0,65-fache der Albedo von Bild 2.
   - Ursachen: L0 aus 5080/2 an der Wesen-Lage (Port-Schattierung 172 gegen 80 in Bild 2), Bild 1 ohne eigenen Schnitt, Lage geschaetzt.
     Gegenlaeufig wirkt das rote Streulicht in Bild 2.
   - Nicht aufgeloest. Die Zielwerte mitteln beide Szenen, die Unsicherheit traegt den halben Abstand.
2. **Rot in Bild 2 ist Obergrenze**, gemessen am schwarzen Oberteil der Frau mit R +13 bis +22 e. Buckel-, Kopf- und Hals-Ton (h 0..+21 Grad)
   sind darum nur als "waermer als heute hoechstens" lesbar.
3. **Kein Hintergrundpaar fuer Bild 1.** Eine Flaechen-Statistik ueber die 5080-Schnitte (Zug, Boden, Gelbstreifen) wurde nicht gemacht.
   Jeder Schnitt traegt eigenes eingebackenes Licht, so waere das ein schwaches Mass.
4. **Pose Bild 2:** IoU 0,56. Die Material-Zuordnung ist dort unsicherer als in Bild 1.
5. **Nicht messbar:** Schwanzspitze, Krallen, Lippe/Unterkiefer, Adern.

## 8. Dateien

Alle Pfade liegen unter `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/eichung/`.

| Datei | Inhalt |
|---|---|
| `skripte/ek.py`, `skripte/em.py` | Lader (Leon PL00, Frau 0x4D, Kameras, Disc-Licht), Farbmodell vorwaerts und zurueck |
| `skripte/e1_bilder.py` | Dia-Suche und Mittelbild; Aufruf `python e1_bilder.py <scratch> <eichung>` nach ffmpeg (Bilder 3380-3540) |
| `skripte/e3_hintergrund.py`, `e4_frau.py`, `e4b_gemeinsam.py`, `e4_leon.py` | Referenzpaare, Fits, Gegenproben |
| `skripte/e5_wesen.py`, `e7_ziel.py`, `e6_tafeln.py` | Umkehr auf das Wesen, Zielfarben und Vergleich, Tafeln |
| `daten/*.json` | alle Zahlen dieses Blatts; `daten/bild{1,2}_mittel.png` Dia-Mittel |
| `tafeln/T1_referenz_bild2.png` | Bild 2: Video, Port roh, em_frei, Bestand, dE-Karte; Frau x3 vorher/nachher |
| `tafeln/T2_referenz_bild1.png` | Leon x3: Video, Port roh, Bestand, EX an Leon; Bild 1 gegen die 6 Schnitte von 5080 |
| `tafeln/T3_wesen_bild1.png`, `T4_wesen_bild2.png` | Vorlage, Umkehr e, Albedo (gueltig), K5e_f1 im Spiel, K5e_f1 Textur, K5e_f1 durch die Kette, Material- und Ausschlusskarte |
| `tafeln/T5_zielfarben.png` | Zielfarben je Material (Bild 1 / Bild 2 je p10, Median, p90) neben K5c, K5e_f1, K5f |

Nicht angefasst: `Version_80%/`, `pri/`, `synchro/`, `generator/kf_k5f*`, `k5f/`, das Spiel-Asset und `build/`. Es wurde nichts gebaut,
gerendert wurde nur offline (form_buehne/v4f).
