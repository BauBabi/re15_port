# SF V4 - Pruefung Runde 4 (Sicht): V4 r4 gegen die Nutzer-Beschriftung Beschreibung1/2

Pruefer "Auge des Nutzers", 2026-11-07, Worktree r41_sichel (Zweig r41/sichel_nb2, HEAD cbc917a48), nichts committet.
Gegenstand: `sfv4/emd/EM30_V4.EMD` = `EM30_V4r4.EMD` sha256 00d642b7... (= `SF_belege/v4/emd_sha256.txt`), Vergleich V4 r3
`EM30_V4r3.EMD` dfbc30bc..., Leisten `SF_belege/v4/01..08` (r4) und `SF_belege/v4/r3/`, Spiellauf `sfv4/r/V4r4` (RE15_FRAMEDUMP,
registriert ueber `c6.port`, Versatz 243 wie V4r3). Nutzerbilder `Version_80%/Beschreibung1/2(_Vorlage).png` unveraendert (mtime
2026-11-06 21:46-21:57). Spiel-Asset `EM30_HANGAR.EMD` sha256 cfd1ac42... = Pin (unveraendert).

**Ergebnis: NICHT bestanden.** Vier Punkte gehoeren in die dritte Klasse:

1. **Schwanz, waagerechter Teil = Ringel-Rohr statt organischem Koerper** (Beobachtung (ii)): kein flaches Brett mehr (12-seitiges Rohr,
   Stufe quer 7,6-8,9 L*), aber drei einfarbige Abschnitte mit harten Quernaehten: entlang der Achse im Videoraum L* 7,3 / 28,6 / 13,2
   und a* 10 / 2 / 22 (x 280-316 / 318-350 / 352-376), Sprung 16,7 dE76 auf 3 px (Schirm 25,8) an den Meshgrenzen 17|18 und 16|17.
   Das Original verlaeuft stetig (L* 26 -> 10 -> 14, a* 3 -> 10, groesster Sprung 9,5 nur am Maskenrand). Die Helligkeit liegt
   verkehrt herum (Original hell an der Biegung, Port dort 7 L*), und der Ansatz am Ruecken ist eine weinrote Manschette (a* 22 gegen
   8-9) mit gerader Kante - nicht "in den Ruecken ueberfuehrend". Innerhalb jedes Abschnitts keine Textur (Plateau 0,41-0,58 gegen
   Original 0,08-0,39; glatt 0,47 gegen 0,19), Oberkontur 1,77 px RMS gegen 3,87.
2. **Neuer gelber Fleck auf der (B)-linken Schulter** (Mesh 13, neue CLUT-Zeile 490): Buehne 45 gelbe Texel 57,5/-5,1/41,1 (V4 r3 und
   Referenz 2: 0); im Spielbild ein Fleck mit 44-62 px in JEDEM Bild h100-h135 (Videoraum 51,5-53,1 / 0,9-1,5 / 23,6-24,9), das Video an
   derselben Stelle 27,6-28,4 / 0,1-0,3 / 0,7-1,6 (b* im Umkreis 20 px hoechstens 3,3 / 9,7). Seitlich (Pose 85) 11 gelbe Texel hinter
   dem Kopf. Dazu neue grelle Einzelpixel (C* > 45: 253 gegen 19 in V4 r3; blau 142 gegen 79), v. a. an Jeans/Beinen (CLUT 491).
3. **Rot-Verteilung Oberkoerper vorn** (Beobachtung (i)): besser als r3, aber das Rot liegt weiter nicht in der Brustmitte. Mitte/aussen
   im Spielbild 1,55 (h118) / 1,49 (h135) gegen Video 2,91 / 2,75; Schultern a* > 12 0,08 / 0,07 (Video 0,00-0,05); die kraeftige
   rote Brustmitte fehlt (Mitte a* > 12 0,15 / 0,13 gegen 0,23 / 0,28). Im Spiel h85/h100 kraeftig rote Pixel (a* > 22) 1170 / 531
   gegen 2 / 107 im Video.
4. (geht in 1 ein, eigener Nutzer-Punkt) **Schwanz-Farbe**: der Nutzer hat ausdruecklich "komplett die falsche Farbe" gesagt; der
   Mittelwert stimmt (dE00 1,9), die Verteilung (dunkel / hellgrau / weinrot in Bloecken, Rotanteil a* > 12 0,19 gegen 0,10) nicht.

Bestanden haben und duerfen nicht verloren gehen: Auge/Haare/Nase vorn, Auge seitlich, **Haarkante seitlich (jetzt W)**, Schwanz-Linie
(Beginn-Lage, Biegung 66,6 Grad, Spitze), Unterarm (W)-rechts (dunkel, keine weissen Flecken), Unterarm (W)-links, Jeans, Kopfmittelfarbe,
Gang koerperfest. **Kopf vorn im Spiel ist von A auf K gestiegen.**

Konventionen: (B) = aus Sicht des Betrachters, (W) = aus Sicht des Wesens. Lab = CIE-Lab D65, dE00 = CIEDE2000. "Videoraum" =
Vergleichsregel Messblatt V4 (Gauss sigma 1,5, V = a + b x P). "Schirm" = Port-Bild ohne diese Abbildung. Vorlage 1 entblendet, Vorlage 2
roh. H1 = 302 px, H2 = 253 px, H_port 364 (Pose 135) / 264 (Pose 85).

Werkzeuge (untracked): `re15_port/tools/r41_sichel/v4/pruefung_sicht_r4/` - die Skripte der Runde 3 auf r4 umgestellt (`render_r4.py`
Hash geprueft 00d642b7 / dfbc30bc / cfd1ac42, `spiel_r4.py`, `messen_r4.py`, `oberkoerper_vorn.py`, `oberkoerper_spiel.py`,
`rotkappe_spiel.py`, `kopf_struktur_spiel.py`, `kopf_textur_vorn_r4.py`, `kopf_seite_r4.py`, `auge_spiel_r4.py`, `schwanz_brett.py`,
`schwanz_silhouette_r4.py`, `tafel_schwanz.py`, `unterarm_links_r4.py`) und neu `schwanz_farbe_waagerecht.py`, `schwanz_naehte.py`,
`gelb_und_ausreisser.py`. Ausgaben `sfv4/sicht_r4/`, Belege `SF_belege/v4/PS4_*`. Polygone/Kreise im Spielbild wie in Runde 3
(Lauf und Szene gleich, Versatz 243). Gegenprobe: meine Werte reproduzieren die Riegel des Bauers (FO 1,55/1,49; KS 30,9/36,8, glatt
0,072/0,134; TQ Stufe 8,2-8,7, TK 1,77 px).

## 1. Befunde Punkt fuer Punkt

Klassen: **W** = wie Vorlage, **K** = Abweichung klein, **A** = Abweichung, die der Nutzer wieder ablehnen wuerde.

### 1.1 Kopf vorn (Beschreibung1, Kamera 50A0/12)

| Punkt | Vorlage (gemessen) | V4 r4 (gemessen) | Klasse |
|---|---|---|---|
| Auge, Lage | Nutzerpunkt (382,165) | K4 0,057 H links / 0,102 H unter OK (Soll 0,035-0,065 / 0,07-0,12) | W |
| Auge, Farbe | 46,9/7,2/-1,1 | K2 47,1/8,0/-1,2 dE00 0,8 | W |
| Auge, Groesse/Kontrast im Spielbild | 52 px, 0,026 x 0,033 H1, Sprung Kern/Ring 45,0 | Buehne K3 0,030 x 0,025 H OK; Spiel h135 41 px, 7 x 8 px = 0,019 x 0,022 H, Flaeche 0,54 x Original, Sprung 66,1 (`PS4_auge_spiel.json`; r3 1,72 x / 45,9). Im Spielbild ein kleiner, weicher heller Fleck, kein "aufgemaltes" Rund (`PS4_kopf_vorn_spiel_x4.png`) | K |
| Haare | 13,3/7,4/1,5 | K5 13,9/8,2/0,9 dE00 1,0 | W |
| Nase | 18,2/7,1/1,3 | K7 18,1/7,1/-0,1 dE00 1,2 | W |
| Kopf-Oberseite rot (Buehne) | KV1 22,6/15,4/5,2 | K9 20,5/16,7/4,9 dE00 1,8 | W |
| Kopf-Mittelfarbe | 20,1/8,5/2,1 | Id 9 Videoraum 19,3/8,7/1,5 dE00 0,8 | W |
| Kinn/Maulzone | 17,1/11,6/4,4 | 18,2/7,5/2,7 dE00 3,9 | K |
| **Kopf-Oberflaeche im Spielbild** (O4) | Innenflaeche Gradient 32,9 (Fenster +-3 px 29,0-40,4), glatt (< 8) 0,074 | **h118 30,9 / glatt 0,072, h135 36,8 / glatt 0,134** (r3 26,9/0,126 und 27,2/0,197) (`PS4_kopf_struktur_spiel_innen.json`). Beide Gradienten in der Originalspanne, h118 glatt = Original, h135 glatt 1,8 x Original. Sicht (`PS4_kopf_vorn_spiel_x4.png`, `PS4_oberkoerper_spiel_x2.png`): das Innere ist jetzt hell/dunkel gefleckt mit roten Strichen an der Kappe, nicht mehr die einheitlich mauve Kuppel. Rest: die Kopfform ist weiter von einem dunklen kreisrunden Rand umschlossen (Original: unregelmaessige Masse ohne Ring), die Mitte um das Auge ist bei h135 glatter. Buehne: Kopf/Rumpf 1,04 (Original 1,57; der Port-Rumpf ist strukturreicher als das Original) | K |

### 1.2 Kopf seitlich (Beschreibung2, Pose 85)

| Punkt | Vorlage | V4 r4 | Klasse |
|---|---|---|---|
| Auge seitlich | 48,4/4,0/0,5, 77 px | S1 48,5/3,1/0,9 dE00 1,2; 0,042 x 0,049 H (Band 0,028-0,052 x 0,040-0,070); 1 Komp. | W |
| Augenlage | 0,03-0,07 H; 0,76-0,82 H | S2 0,054 H; 0,809 H | W |
| Haare seitlich | 19,1/1,2/1,1, 0-2 px vor dem Fleck | S3 17,2/2,8/0,6 dE00 2,5, Abstand 0,004 H (r3 0,015 H) | W |
| Kopf seitlich Flaeche/Form | Gradient Kopfbox 46,3, glatt 0,103 | Id 9 49,1, glatt 0,071 (`PS4_kopf_seite.json`); Form weiter ein Keil mit gerader Oberkante (`04_kopf_nah_seite_x4.jpg`) | K |
| Kopf-Oberseite rot seitlich | 27,6/15,3/3,3 | S4 27,6/15,6/2,9 dE00 0,4 | W |
| Gelber Strich hinter dem Kopf | kein Gelb | 11 gelbe Texel Mesh 13 bei x 436-444, y 212-225 (63,2/-4,5/43,9), sichtbar in `04_kopf_nah_seite_x4.jpg` Spalte r4 - geht in 1.5 "gelber Fleck" ein | (A, s. 1.5) |

### 1.3 Schwanz (Beschreibung2)

| Punkt | Vorlage | V4 r4 | Klasse |
|---|---|---|---|
| Beginn, Lage | (357,243), 0,15 H hinter Scheitel | T1 0,172 / 0,742 H; TA <= 0,024 H | W |
| Beginn "in den Ruecken ueberfuehrend" (Uebergang) | stetiger Uebergang: an der Wurzel (x 352-358) L* 14,2-14,8, a* 8,0-9,4, kein Sprung > 9,5 dE76 | Mesh 16 ist eine weinrote Manschette: x 352-376 L* 13,2, **a* 22,0-22,3**, an x 352 Quernaht 16,7 dE76 (Videoraum) / 25,8 (Schirm) zum hellgrauen Abschnitt; gerade senkrechte Kante sichtbar (`PS4_schwanz_seite_x4.png`, `PS4_schwanz_brett.png`) | **A** |
| Biegung | Pruefer-Mittellinie 65,8/69,5/70,1 Grad | T2 66,6 Grad, Lage dx -0,251 dy +0,051 | W |
| Knick | (290,308) | T3 dx -0,233 (Soll -0,265 +-0,03, 0,002 H ausserhalb) / 0,266 | K |
| "von da an breiter nach oben" | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H | T5 0,028/0,053/**0,095**/0,121 H, monoton (Ausbuchtung weg); an der Biegung schmaler als das Original | K |
| Spitze Ende | (294,344), 0,37 H | T4 0,357 H | W |
| Umriss | Leser-Maske | IoU 0,466 (r3 0,514), 63 % gedeckt; Oberkante x 276-292 246..232 gegen 251..234 (max 0,030 H); Dorn an der Bogenecke weg (kleinster Eckwinkel 87 Grad bei (298,254)); neu ein Absatz 69/91 Grad bei (322,206)-(323,209) an der Meshgrenze (`PS4_schwanz_silhouette.json`) | K |
| **Form waagerechter Teil (Beobachtung ii)** | Querprofil stetig, Plateau x 290-350: 0,08-0,39, Stufe 3 px 3,5-10,0; Oberkontur 3,87 px RMS; glatt (Grad < 3) 0,19; Textur mit Glanzlicht | Kein Brett mehr: je Glied 12-seitiges Rohr (48 Ecken / 12 Flaechen, Dicke/Breite 0,82-1,07; Referenz 2: 120 Ecken / 50 Flaechen), Stufe 7,6-8,9 L*. Aber: Plateau **0,41-0,58** an 10 von 11 Spalten ueber dem Original-Maximum, glatt **0,47** (2,5 x), Oberkontur **1,77 px** RMS; je Spalte 2-4 Laengsbaender konstanter Helligkeit (Facetten); entlang der Achse drei einfarbige Bloecke mit zwei Quernaehten (s. Kopf des Berichts, `PS4_schwanz_naehte.json`). Sicht (`PS4_schwanz_seite_x4.png`): ein gerader, untexturierter Kegel aus drei Ringen dunkelrot / hellgrau / weinrot statt eines gefleckten, weich schattierten Koerperteils; im Spiel h90 ein glatter mauve Kegel ohne Textur (`PS4_schwanz_brett.png`), 08 rundum Gier 180-300 dieselbe Ringelung | **A** |
| **Farbe (Nutzer: "komplett die falsche Farbe")** | waagerecht 17,9/6,1/1,3, a* > 12 0,10; senkrecht 15,6/3,3/1,9; Achse hell an der Biegung, dunkel zur Mitte, mittel an der Wurzel | Mittel waagerecht 15,8/6,4/2,7 dE00 1,9, senkrecht 15,0/5,5/1,7 dE00 2,6 (`PS4_schwanz_farbe.json`), T8 gruen. Verteilung: a* > 12 **0,19** (2 x); Helligkeit entlang der Achse **umgekehrt** (Port an der Biegung 7,3 L*, Original 26-28); Wurzel weinrot statt mauve | **A** (zu Punkt Form) |

### 1.4 Unterarme

| Punkt | Vorlage | V4 r4 | Klasse |
|---|---|---|---|
| Unterarm (B)-links = (W)-rechts (Id 14) | 8,8/-3,4/3,2, p90 11,1, 0 % L* > 30 | 11,4/-1,9/2,2, p10/50/90 7,0/11,4/15,6, 0 % L* > 30, dE00 2,6 (`PS4_unterarm_links.json`, = r3) | W |
| Unterarm (B)-rechts = (W)-links (Id 11) | 25,0/1,0/-2,1 | 26,9/1,7/-1,5 dE00 1,8 | W |

### 1.5 Farben je Region und Gesamteindruck vorn

| Region | Vorlage | V4 r4 | Klasse |
|---|---|---|---|
| **Gesamteindruck Oberkoerper vorn (Beobachtung i)**, Spielbild gegen Video (Polygone wie Runde 3, `PS4_oberkoerper_spiel*.json`, `PS4_oberkoerper_spiel_x2.png`) | Rotanteil Drittel h118 0,23/**0,55**/0,14, h135 0,18/**0,58**/0,24 -> Mitte/aussen 2,91 / 2,75; Schultern a* > 12 0,00/0,05 und 0,00/0,04; Mitte a* > 12 0,23 / 0,28; L*90 (B)-links 45,8 / 48,6 | h118 0,34/0,47/0,27 -> **1,55**, h135 0,29/0,42/0,28 -> **1,49** (r3 1,18 / 1,06); Schultern a* > 12 **0,08/0,01** und **0,07/0,01**; Mitte a* > 12 **0,15 / 0,13**; L*90 (B)-links 43,5 / 41,6. Buehne 2,33 (Original 2,76). Kraeftig rot (a* > 22) im Spiel h85 1170 px gegen Video 2, h100 531 gegen 107 (`PS4_rot_spiel.json`). Mittel und Helligkeit treffen (h135 Drittel L* 25,6/19,9/24,5 gegen 24,9/18,1/24,0) - "dunkler" ist r4 nicht mehr. Sicht: r4 wirkt mauve-braun mit roten Adern ueberall; die leuchtend rote Brustmitte des Originals fehlt, die Schultern sind nicht so hell rosa-grau | **A** |
| **Gelber Fleck (B)-linke Schulter** (neu) | Video an der Stelle 27,6/0,3/1,6 (h118), 28,4/0,1/0,7 (h135), kein Gelb | Mesh 13 (CLUT 490) 45 gelbe Texel 57,5/-5,1/41,1 (V4 r3 / REF2: 0); Spielbild-Fleck 44-62 px in h100, h110, h118, h125, h135 (Videoraum 51,5-53,1 / 0,9-1,5 / 23,6-24,9) und h90 (58 px) (`PS4_gelb_ausreisser.json`, `PS4_schulter_kopf_spiel_x4.png`; sichtbar auch in `01_vorn_kamera12.jpg`, `07_gang.jpg` h118/h135, `08_rundum_V4.jpg` Gier +60) | **A** |
| Grelle Einzelpixel (neu) | - | Buehne C* > 45: 253 px (V4 r3 19), davon Jeans/Beine Mesh 0/1/3/4/5 174; blau (b* < -15) 142 (r3 79); im Spielbild-Schirm 19-20 neue blaue Pixel h118/h135 (in `01` am Guertel sichtbar) | K |
| Oberarm (B)-rechts (Id 10) | 16,8/5,2/0,0, Rotanteil 0,25 | 19,9/6,1/1,0 dE00 2,5, Rotanteil 0,21 | W |
| Oberarm (B)-links (Id 13) | 17,3/3,6/1,3, Rotanteil 0,12 | 22,0/5,2/2,9 dE00 3,9, Rotanteil 0,33 | K |
| Brust rot F6 (Box) | 16,6/16,7/6,1 | Box 19,3/11,2/4,8 dE00 4,5 (Riegel F6 Wortlaut 22,1/9,7/3,8 dE00 7,0 ROT) - Teil von Beobachtung (i) | (A, s. oben) |
| Rumpf (Id 8) | 13,0/4,7/2,1 | 21,6/7,2/3,1 dE00 6,4 | K |
| Jeans | 14,3/2,3/-2,4 | 15,9/1,3/-3,1 dE00 1,9 | W |
| Unterschenkel (Box) | 14,7/1,6/-1,4 | 15,9/1,6/-2,0 dE00 1,0 | W |
| Seite: Buckel-Glanz F14 | 60,0/11,0/1,2 | 57,5/11,0/1,8 dE00 2,2 | W |

### 1.6 Gang

| Punkt | Soll | Ist | Klasse |
|---|---|---|---|
| T7 koerperfest | Gelenke <= 0,06 H | 0/47 Takte, max 0,048 H; Biegung 61-71 Grad; Boden: tiefster Mesh-Punkt -13 E. (Riegel) | W |
| Schwanz im Gang | - | in allen Feder-Bildern h100-h135 (`07_gang.jpg` letzte Zeile) dasselbe Ringel-Rohr - geht in 1.3 Form ein | - |

## 2. Was den Nutzer-Eindruck bestimmt

1. **Schwanz seitlich**: Linie, Knick, Spitze stimmen; aber man sieht ein gerades, untexturiertes Rohr aus drei verschiedenfarbigen
   Ringen (dunkelrot, hellgrau, weinrot) mit harten Quernaehten, das wie eine Manschette am Ruecken steckt. Das Original ist ein weich
   schattierter, gefleckter, mauve Koerperteil, hell an der Biegung und stetig in den Ruecken uebergehend. Der Nutzer hat beim Schwanz
   die Farbe schon einmal mit "komplett die falsche Farbe" abgelehnt.
2. **Gelber Fleck** auf der Schulter in jedem Frontbild - eine Farbe, die das Original nirgends hat.
3. **Vorn** fehlt die leuchtend rote Brustmitte; das Rot ist als Adern ueber Schultern und Arme verteilt (Mitte/aussen ~1,5 statt ~2,8).

## 3. Empfehlung an den Bauer

- Schwanz: Textur entlang der Kette stetig (keine einfarbigen Abschnitte je Mesh); Achsenverlauf nach dem Original (x 281-358:
  L* 26 -> 10 -> 14, a* 3 -> 10); an der Wurzel die Farbe des Buckels (a* 8-9, nicht 22); Riegelvorschlag: Quernaht entlang der Achse
  <= 9,5 dE76 auf 3 px (Original-Maximum, `PS4_schwanz_naehte.json`), glatt (Grad < 3) im waagerechten Teil <= 0,25 (Original 0,19),
  Plateau quer <= 0,39 an allen Spalten x 290-350. Linie/Knick/Breite/Spitze und Biegung halten.
- Gelb: CLUT-Zeile 490 (Mesh 13) ohne gelbe Eintraege; Riegelvorschlag: gelbe Texel (b* > 20, a* < 12, L* > 25) = 0 auf Buehne und
  Spielbild-Differenz; dazu C* > 45 nicht ueber V4 r3 (19 px).
- Oberkoerper: Rot in die Brustmitte (Mitte a* > 12 >= 0,23), Schultern a* > 12 <= 0,05, Mitte/aussen >= 2,75 im Spielbild.
- Kopf vorn: h135 glatt <= 0,098 (Mitte um das Auge fleckig); dunklen Kreisrand pruefen.
- Erhalten: Auge/Haare/Nase vorn, Auge und Haarkante seitlich, Unterarme, T1/T2/T4/T6/T7, Kopf im Spielbild (Gradient im Band).

## 4. Belege (`SF_belege/v4/`, untracked)

- `PS4_schwanz_seite_x4.png` (Vorlage 2 | V4 r3 | V4 r4 | V4 r4 neutral), `PS4_schwanz_brett.png`, `PS4_schwanz_brett.json`,
  `PS4_schwanz_brett_kennzahlen.json`, `PS4_schwanz_naehte.json`, `PS4_schwanz_farbe.json`, `PS4_schwanz_silhouette_x4.png`, `PS4_schwanz_silhouette.json`
- `PS4_oberkoerper_spiel_x2.png` (Video | r3 Videoraum | r4 Videoraum | r4 Schirm, h135/h118), `PS4_oberkoerper_spiel.json`,
  `PS4_oberkoerper_spiel_drittel.json`, `PS4_oberkoerper_vorn.json`, `PS4_rot_spiel.json`
- `PS4_gelb_ausreisser.json`, `PS4_schulter_kopf_spiel_x4.png` (Schulter und Kopf: Video | r3 | r4, h135/h118)
- `PS4_kopf_vorn_spiel_x4.png`, `PS4_kopf_struktur_spiel.json`, `PS4_kopf_struktur_spiel_innen.json`, `PS4_kopf_textur_vorn.json`,
  `PS4_kopf_seite.json`, `PS4_auge_spiel.json`, `PS4_unterarm_links.json`
- Regionen-Rohwerte: `sfv4/sicht_r4/messen.txt`, `sfv4/sicht_r4/messung_r3.json` (Dateiname aus dem Runde-3-Skript, Inhalt V4 r4)
