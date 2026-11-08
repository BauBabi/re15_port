# Messblatt Farbe V3, Nachtrag "Rot wie die Vorlage" (Riesenform K5c, Messer, 2026-11-07)

**Anlass, Nutzerurteil zu Runde 3 (2026-11-07, woertlich):** "Naja, passt von den farben her nicht ganz der vierbeiner, zu viel rot im Vergleich zur
Vorlage, aber immerhin bedrohlich...."

**Gegenstand:** Variante Versuch 3 Runde 3, `farbe/K5c_farbe_v3/` (FORM.EMD sha256 d19fe9012ff8a5a1..., FORM.TIM abd0ba1b2390c942...).
**Vorlage:** Bild 1 = `Version_80%/vlcsnap-2026-10-31-00h28m45s774.png` (Bahnsteig, blaues Licht), Bild 2 = `Version_80%/vlcsnap-2026-10-31-00h29m02s830.png`
(5090 Cut 11, Feuerlicht), im Baum als `form/bild1.png`, `form/bild2.png` (`form_pruefung.original`).
**Vergleich:** RE2-G4 (CDEMD0.EMS Typ 0x34, Clip 0) unter neutralem Licht.

Dieser Nachtrag aendert keine Farbe und kein Asset. Er misst, was "zu viel Rot" in Zahlen heisst, setzt Zielwerte mit Toleranz aus der Streuung der
Vorlage und prueft das Riegel-Paket. Spiel-Asset `BAHNSTEIG_FORM`, Pins und `re15_port` sind unveraendert.
Die Leuchtstellen (F1 am Becken in Bild 1, Fleischband am Maul in Bild 2) sind ausgenommen und bleiben wie in Runde 3 (Messblatt V3 7.1 ist offen).

**Eigener Code und Rohzahlen:** `farbe_lesung/V3R4_skripte/` (Kopie des Scratchpads
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/`):
- `basis.py`: Regionen je Pixel (Knochen + lokale Flaechennormale), CLUT-Zeilenkarte der Variante, neutraler Render mit Regionen.
- `zeilen.py` (Vorlauf), `mess.py` -> `mess.json`: alle Masse in drei Messraeumen, Neutral-Probe, Block-Bootstrap.
- `ziele.py` -> `ziele.json`, `ziele_tabelle.py` -> `ziele_tabelle.md`, `tabelle.py` -> `tabellen.md`: Ziele, Toleranzen, Tabellen (keine Handabschrift).
- `riegel_rot.py`: die neuen Riegel RV1-RV5 als ausfuehrbares Skript, feste Messmaske `rot_masken.npz`.
  Gegenproben: `gegen_v3r3.txt`, `gegen_rundlauf.txt`, `gegen_k5c_heute.txt`.
- `probe.py` -> `probe.json`, `probe_auswert.py` -> `probe_auswert.json`: Ketten-Vorhersage im Speicher (Abschnitt 6, kein Ziel).
- `belege.py`: Belegbilder.
- Fremde Eingaben (nur gelesen): Leser B `formfarbe3/leserb/fid1.npz`, `fid2.npz` (Knochen und Flaeche je Pixel, K5c-Geometrie; G0: V3 hat dieselbe
  Geometrie), `generator/v3_daten/masken.npz` (F1, Band, Zaehne des Messblatt-Pruefers), Nutzer-Teilmasken `form/nutzer_teile/maske_*.png`.

**Belegbilder** (`farbe_lesung/`, nichts aufgehellt, Klassen und Regionen als beschriftete Falschfarbe):
- `V3R4_1_bild1_rot_x3.png`: Bild 1, 3-fach. Oben Original-Video | V3 r3 durch `form_video` (Saat 15), roh. Unten Klassenkarten im Texelraum T.
- `V3R4_2_bild2_rot_x3.png`: dasselbe fuer Bild 2.
- `V3R4_3_neutral_g4_v3.png`: RE2-G4 und V3 r3 im Tafel-03-Licht, drei Winkel; Licht roh, Klassen auf Texeln, Regionenkarte.

Seiten: (W) = aus Sicht des Wesens. Lab = CIE-Lab D65, OpenCV float (RGB/255), wie V1-V3. Zahlen mit Komma.

## 0. Ergebnis in Kuerze

1. **"Zu viel Rot" ist messbar, aber nicht als Gesamtmenge in Bild 1.** In der Bild-1-Kette hat V3 r3 20,9 % Rot (Vorlage 14,1 %, Fenster 6,9-21,4).
   Zu viel ist es dort, wo es sitzt:
   - Schwanz 43,5 % gegen 23,5 % (Fenster bis 34,5);
   - Vorderglied r (W) 12,3 % gegen 2,4 % (bis 7,1);
   - **groesste zusammenhaengende Rotflaeche** 10,6 % der Figur gegen 2,3 % (bis 5,5), am Schwanz 22,9 % gegen 6,0 % (bis 9,7).
   Die Vorlage hat ihr Rot in kleinen, verstreuten Flecken; V3 hat einen grossen roten Block am Schwanz und rote Bahnen am Vorderglied.
2. **In Bild 2 ist es auch die Menge:** Ruecken 71,5 % gegen 53,6 % (bis 65,2), Brust/Bauch 74,9 % gegen 42,8 % (bis 67,7),
   Hals 87,1 % gegen 45,1 % (bis 72,3), Vorderbeine 82,3 % gegen 65,4 % (bis 74,5).
3. **Der Farbton ist zu orange, in beiden Bildern.** Farbton der a\*-Klasse, V3 gegen Vorlage:
   - Bild 1 Figur -0,2 gegen -15,8 Grad (Fenster -30,6..-1,0), Flanke +5,8 gegen -19,5.
   - Bild 2 Ruecken 30,6 gegen -5,3 Grad, Brust 30,6 gegen 7,9, Hals 30,7 gegen 11,0, Kopf 30,4 gegen 18,9.
   - Das Rot der Vorlage ist in Bild 1 ein Mauve-Rosa, in Bild 2 (trotz Feuerlicht) ein Karminrot. V3 traegt am Koerper das G4-Rot mit 28-31 Grad.
   - Messblatt V3 4 ("Rot in Bild 2 ... 31 Grad passend zum G4") galt fuer die ganze Figur samt Kiefer und Klauen. Je Region stimmt es nicht.
4. **Die Adern leuchten heller als die Haut, in der Vorlage nicht.** dL = L\*(Rot) - L\*(uebrige Haut):
   - Vorlage Bild 1 Figur -0,6, Bild 2 Figur -2,5. Das Rot der Vorlage sitzt in den dunklen Stellen, nicht auf hellen Linien.
   - V3 r3 in der Kette +2,0 / +3,5; ohne Kette (Texel, neutral) +10,6. RE2-G4 (Texel, neutral) +22,1.
   - In Bild 2 sind Ruecken, Hals, Kopf und Figur deshalb ROT.
5. **RE2-G4 ist noch roter als V3:** neutral, Texel: Rot 37,5 % (V3 27,0 %), Farbton 34,9 Grad (V3 20,6), groesste Rotflaeche 8,2 % (V3 2,7).
   Die G4-Machart (schwarze Haut, Adern) kann bleiben. Farbton, Helligkeit und Menge der Adern muessen auf die Vorlage, nicht auf G4.
6. **Das alte Riegel-Paket misst das nicht** (Abschnitt 4):
   - K4, K4b und K12 zaehlen nur Rot mit L\* >= 20. Am Schwanz sehen sie 5,8 % gegen 6,2 % (gleich); dieselbe Stelle hat mit RV1 43,5 % gegen 23,5 %.
   - K11 hat das Fenster "<= +4 Grad" und keine Bild-2-Zeile.
   - B2 (groesste Rotflaeche >= 3,2 %), N1, A1, K1v2 und K2v2 verlangen **mehr** Farbe. Sie zeigen gegen das Nutzerurteil.
   - Neu sind RV1-RV4 (bindend) und RV5 (Info): 45 bindende Zeilen. V3 r3: 21/45, also 24 ROT. Rundlauf der Vorlage: 45/45. K5c ohne Farbe: 30/45 (zu wenig Rot, Untergrenzen).
7. **Ketten-Vorhersage (Abschnitt 6, kein Ziel):**
   - Bild-1-Zeilen mit Rot-Texeln auf Farbton -20 Grad und Buntheit x 0,6 treffen 19 von 21 Bild-1-Zeilen.
   - In Bild 2 reichen Farbton und Buntheit allein nicht (beste 15/24). RV3 (Adern heller als die Haut) bleibt ROT, bis die Adern dunkler oder die Haut heller wird.

## 1. Messverfahren

### 1.1 Drei Messraeume

| Raum | Vorlage | V3 r3 | Zweck |
|---|---|---|---|
| **T, Kette rueckgerechnet** (bindend) | `p_zonen.rueck(Original)`: `video_umkehr`, Gauss 1,0, / Figurlicht (farbe.md 5) | `Kandidat.rendern(n)` (Disc-Licht x k, wie Tafel 01/02) -> `video_filter` (Saaten 15 / 12345 / 7) -> `rueck`, Mittel der 3 Saaten | gleicher Weg wie p_riegel.kette_riegel und K11-K15 |
| **V, roh in der Kette** (Gegenprobe) | Videobild selbst | `video_filter`-Ausgabe | ohne Umkehr; prueft, dass die Aussage nicht an der Umkehr haengt |
| **Texel, neutral** | - | Albedo im Tafel-03-Renderer (`render.blick`, Licht 0,55 + 0,45 N.L, Umkugel 0,46, 3 Winkel, Clip 2) | Vergleich mit RE2-G4 (Clip 0) im selben Renderer |

**Rundlauf R:** Vorlage-T x Figurlicht -> `video_filter` (3 Saaten) -> `rueck`, wie Messblatt V3 K11. R zeigt, wie stark eine zweite Kette ein Mass verschiebt.

### 1.2 Regionen (`basis.region_k5c`, `basis.region_g4`)

- **Je Pixel der Knochen** (Leser B `fid%d.npz`) und **die lokale Normale der Flaeche** (Mittel der Eckennormalen aus dem MD1).
  Die lokalen Achsen sind bei K5c und G4 gleich: x vorn, y unten, z links (W) (Leser A 2).
- **Rumpf** (K5c 0/7/10, G4 0/9/10), aufgeteilt nach der Normalen:
  - **Ruecken:** ny < -0,5, also weniger als 60 Grad von oben.
  - **Brust/Bauch:** ny > 0,5, oder Brustknochen 10 mit nx > 0,5.
  - **Flanke:** der Rest.
  - Die Grenze 0,5 ist eine **Definition** (cos 60 Grad), keine Bildaussage.
- **Uebrige Regionen** (K5c-Knochen nach Messblatt V3 2.3, G4 nach Leser A 2):
  - Schwanz: K5c 1-6.
  - Hinterbeine: K5c 8/9/24-27, G4 1-8.
  - Vorderbeine samt Klauen: K5c 11-20/28-31, G4 11-19.
  - Hals: K5c 21.
  - Kopf samt Kiefer: K5c 22/23, G4 26.
  - G4 hat keinen Schwanz und keinen Hals. Die kleinen G4-Arme 20-25 zaehlen nicht mit.
- **Messmaske je Bild** = Nutzer-Figur (`p_zonen.regionen`, ohne Boden und Logo) x K5c-Geometrie x Figur der Variante,
  ohne geclippte Pixel (V >= 200) und ohne Leuchtstellen (1.5):
  - Bild 1: 28 536 von 32 327 Nutzer-Figurpixeln.
  - Bild 2: 38 853 von 45 038.
  - Vorlage und Variante werden auf **denselben Pixeln** gemessen. Die Maske ist fest (`rot_masken.npz`).
- **Nicht sichtbar** (< 150 px): Bild 1 Hals (0 px) und Kopf (125 px, Kieferrand, Messblatt V3 0.3); Bild 2 Schwanz (0) und Hinterbeine (64).
  Jede Region hat damit genau ein Bild, Ruecken, Flanke, Brust/Bauch und Vorderbeine haben beide.
- **Schwach** (< 20 Bootstrap-Bloecke, nur Info): Bild 1 Brust/Bauch (261 px, 12 Bloecke), Bild 2 Flanke (1008 px, 18 Bloecke).

### 1.3 Rotklasse: a\*-Schwelle aus der Neutral-Probe

**Begruendung der Schwelle:** Was die Kette an a\* aus einer **unbunten** Figur macht, ist kein Rot.

**Neutral-Probe** (`mess.py`):
- Die Vorlage-T wird auf der Figur unbunt gesetzt (a\* = b\* = 0, L\* bleibt), der Hintergrund bleibt.
- Dann mal Figurlicht -> `video_filter` (3 Saaten) -> `rueck`, auf der Messmaske.
- Die Schwelle A0 ist das aufgerundete 99. Perzentil von a\*.
- Damit ist Einstreuung aus dem Hintergrund (orange Wand in Bild 1, Feuer in Bild 2) eingerechnet.

| Bild | a\* der unbunten Figur, T: p50 / p95 / p99 | **A0 (T)** | a\* in V: p50 / p95 / p99 | **A0 (V)** |
|---|---|---|---|---|
| 1 | 0,8 / 3,1 / 7,7 | **8** | 7,0 / 8,9 / 13,1 | **14** |
| 2 | 2,2 / 10,3 / 18,5 | **19** | 11,3 / 25,2 / 29,4 | **30** |

Fuer die neutralen Texel (G4, V3 im Tafel-03-Licht) gilt A0 = 8. Das ist die Bild-1-Schwelle des Texelraums; ohne Kette gibt es keine Einstreuung.

**Klassen:**
- **a\*-Klasse:** a\* >= A0, ohne L\*-Untergrenze. Dunkle Adern zaehlen mit, anders als bei K4 und K12.
- **Rot eng:** a\*-Klasse mit Farbton -30..70 Grad. Das Fenster ist das von Messblatt V3 4 (Leser B 4: "rot = ... Farbton -30..70 Grad").
- **Mauve:** a\*-Klasse mit Farbton < -30 Grad. Mauve gehoert in Bild 1 zur Vorlage (Brust/Bauch 35,6 %, Schwanz 10,3 %). Es ist nicht das "Rot" des Nutzerurteils.

### 1.4 Masse je Region

| Mass | Definition |
|---|---|
| Rot eng % | Anteil der Pixel in "Rot eng" |
| Farbton a\*-Klasse | atan2(Mittel b\*, Mittel a\*) der a\*-Klasse. Mauve zieht ihn nach unten; er zeigt damit Rot gegen Mauve |
| C\* Rotklasse | Mittel von C\* der a\*-Klasse |
| dL Rot - Haut | Mittel L\* der a\*-Klasse minus Mittel L\* der uebrigen Pixel der Region: "Helligkeit der Adern gegen die Haut" |
| groesste Rotflaeche eng % | groesste 8-zusammenhaengende Flaeche "Rot eng" in der Region, in % der Region |

### 1.5 Leuchtstellen ausgenommen

- **Bild 1:** F1-Maske des Messblatt-Pruefers (646 px) vereinigt mit
  - derselben Regel 3.1 am V3-Video (Video-Lab L\* >= 30 und a\* >= 20, zusammenhaengend mit dem a\*-Maximum der Box x 300-345, y 225-265),
  - den hellen F1-Texeln der Variante (CLUT-Zeile 6, L\* > 28,66, Dossier V3R3.2).
- **Bild 2:** Band- und Zahnmaske des Messblatt-Pruefers vereinigt mit den CLUT-Zeilen 7 und 8 der Variante.
- Je 2 px Saum (= Optik-sigma 2 px, farbe.md 3).
- Ausgenommen sind in Bild 1 1110 px, in Bild 2 3751 px.
- Die Leuchtstellen selbst (nur Info): Bild 1 F1 Vorlage T Lab (46,3 / 19,3 / -2,6), V3 (47,8 / 17,3 / -1,1); Bild 2 Band Vorlage (49,0 / 33,7 / 19,6), V3 (50,9 / 30,9 / 21,0).
- **Bild 1 Ruecken:** Die groesste Rotflaeche der Vorlage (289 px, x 326-348, y 208-237) liegt 4-10 px neben F1.
  Sie ist der Hof der Leuchtstelle, deshalb ist diese eine Zeile nur Info.

### 1.6 Toleranz aus der Streuung der Vorlage

- **Ziel** = Vorlage O im selben Messraum.
- **Toleranz** fuer Anteil, Farbton, C\* und dL = 2 sigma + |R - O|:
  - **2 sigma** aus einem Block-Bootstrap der Vorlage: 12 x 12 px Bloecke, 300 Zuege, Saat 15. 12 px > 3 x Optik-sigma 2 px und > Chroma-sigma 6 x 3 px (farbe.md 3).
  - **|R - O|:** Verschiebung durch eine zweite Kette, Mittel aus 3 Saaten.
- **Groesste Rotflaeche** (raeumlich, ohne Bootstrap):
  - Fenster [0,6 x O, max(1,5 x O, O + 3)], beidseitig erweitert um |R - O| + Spannweite der 3 Rundlauf-Saaten.
  - Faktoren wie Messblatt V1 K4 (0,6-1,5 x Original) und Messblatt V3 K12 (max(1,5 x Original, Original + 3 Punkte)).
  - **Bindend ist nur die Obergrenze**, weil das Urteil "zu viel" lautet. Die Untergrenze ist Info.
- **Bindend** sind Regionen mit >= 20 Bloecken und die Figur je Bild.

## 2. Messwerte je Region: Vorlage | Rundlauf | V3 r3 und V3 r3 | RE2-G4 (`tabellen.md`, aus `mess.json`)

### 2.1 Kette, Texelraum T (bindend)

**Bild 1, Kette, Texelraum T = Umkehr / Figurlicht (p_zonen.rueck).** Schwelle a\* >= 8 (Neutral-Probe p99). Spalten je Mass: Vorlage O | Rundlauf R (3 Saaten) | V3 r3 (3 Saaten).

| Region | px (Bloecke) | Rot eng % | Farbton a\*-Klasse Grad | C\* Rotklasse | dL Rot - Haut (L\*) | groesste Rotflaeche eng % | a\*-Klasse % (rot + mauve) |
|---|---|---|---|---|---|---|---|
| Ruecken | 1341 (38) | 27,1 / 25,7 / 20,7 | 9,5 / 11,5 / 15,8 | 14,7 / 13,3 / 13,6 | -1,2 / -2,6 / -0,4 | 21,2 / 22,0 / 8,3 | 27,7 / 25,7 / 20,7 |
| Flanke | 4327 (61) | 12,9 / 6,6 / 5,9 | -19,5 / -21,1 / 5,8 | 15,7 / 15,8 / 12,5 | -0,4 / 0,2 / 2,5 | 4,3 / 4,5 / 1,9 | 21,3 / 13,1 / 5,9 |
| Brust/Bauch | 261 (12) | 11,5 / 2,9 / 0,0 | -41,1 / -44,7 / - | 14,4 / 14,2 / - | 2,3 / -1,0 / - | 10,7 / 2,9 / 0,0 | 47,1 / 37,8 / 0,0 |
| Vorderbeine | 4006 (49) | 2,4 / 0,0 / 12,3 | -28,1 / - / -6,1 | 11,9 / - / 12,1 | -6,6 / - / 7,3 | 1,3 / 0,0 / 6,5 | 4,1 / 0,0 / 12,6 |
| Hinterbeine | 7519 (89) | 5,1 / 2,2 / 2,2 | -30,4 / 1,8 / 3,9 | 16,0 / 15,1 / 10,9 | -4,1 / -2,3 / -2,7 | 2,6 / 1,9 / 1,1 | 13,2 / 4,0 / 2,2 |
| Hals | 0 | nicht sichtbar (< 150 px) | | | | | |
| Kopf | 125 | nicht sichtbar (< 150 px) | | | | | |
| Schwanz | 10957 (109) | 23,5 / 18,6 / 43,5 | -12,3 / -7,1 / -1,0 | 15,7 / 14,4 / 12,7 | 3,9 / 5,3 / -0,7 | 6,0 / 5,5 / 22,9 | 33,8 / 24,8 / 43,6 |
| Figur | 28536 (270) | 14,1 / 10,0 / 20,9 | -15,8 / -8,2 / -0,2 | 15,5 / 14,5 / 12,6 | -0,6 / 0,8 / 2,0 | 2,3 / 2,1 / 10,6 | 22,1 / 14,1 / 21,0 |

**Bild 2, Kette, Texelraum T = Umkehr / Figurlicht (p_zonen.rueck).** Schwelle a\* >= 19 (Neutral-Probe p99). Spalten je Mass: Vorlage O | Rundlauf R (3 Saaten) | V3 r3 (3 Saaten).

| Region | px (Bloecke) | Rot eng % | Farbton a\*-Klasse Grad | C\* Rotklasse | dL Rot - Haut (L\*) | groesste Rotflaeche eng % | a\*-Klasse % (rot + mauve) |
|---|---|---|---|---|---|---|---|
| Ruecken | 3357 (54) | 53,6 / 53,3 / 71,4 | -5,3 / -6,6 / 30,6 | 30,1 / 26,6 / 30,2 | -7,4 / -5,0 / 1,8 | 39,7 / 40,8 / 61,4 | 59,2 / 55,3 / 71,4 |
| Flanke | 1008 (18) | 52,3 / 33,9 / 24,1 | 10,4 / 9,0 / 26,1 | 28,7 / 24,4 / 25,0 | -10,3 / -7,4 / 3,1 | 50,7 / 28,3 / 10,5 | 53,6 / 34,4 / 24,1 |
| Brust/Bauch | 5157 (84) | 42,8 / 26,9 / 74,9 | 7,9 / 5,2 / 30,6 | 26,7 / 24,1 / 28,2 | -4,3 / -2,2 / 0,9 | 9,4 / 7,8 / 24,7 | 43,8 / 26,9 / 74,9 |
| Vorderbeine | 12142 (152) | 65,4 / 62,6 / 82,3 | 36,6 / 36,3 / 28,5 | 36,1 / 33,7 / 36,0 | -4,2 / -7,6 / -3,9 | 30,9 / 32,7 / 39,8 | 65,5 / 62,6 / 82,3 |
| Hinterbeine | 64 | nicht sichtbar (< 150 px) | | | | | |
| Hals | 3856 (62) | 45,1 / 29,9 / 87,1 | 11,0 / 14,7 / 30,7 | 25,9 / 22,6 / 29,0 | -3,9 / -3,3 / 0,8 | 14,3 / 11,7 / 51,6 | 47,2 / 30,0 / 87,1 |
| Kopf | 13269 (123) | 59,0 / 32,5 / 55,6 | 18,9 / 22,5 / 30,4 | 27,1 / 23,8 / 28,9 | -1,5 / 0,4 / 3,0 | 55,7 / 21,9 / 32,8 | 59,8 / 32,5 / 55,6 |
| Schwanz | 0 | nicht sichtbar (< 150 px) | | | | | |
| Figur | 38853 (338) | 56,9 / 42,8 / 70,2 | 22,8 / 25,3 / 29,7 | 30,5 / 28,6 / 31,5 | -2,5 / 0,5 / 3,5 | 41,4 / 16,8 / 52,4 | 58,0 / 43,1 / 70,2 |


### 2.2 Roh in der Kette, Videobild V (Gegenprobe)

**Bild 1, roh in der Kette (Videobild V).** Schwelle a\* >= 14 (Neutral-Probe p99). Spalten je Mass: Vorlage O | Rundlauf R (3 Saaten) | V3 r3 (3 Saaten).

| Region | px (Bloecke) | Rot eng % | Farbton a\*-Klasse Grad | C\* Rotklasse | dL Rot - Haut (L\*) | groesste Rotflaeche eng % | a\*-Klasse % (rot + mauve) |
|---|---|---|---|---|---|---|---|
| Ruecken | 1341 (38) | 12,4 / 13,0 / 14,4 | -30,1 / -25,0 / -24,1 | 22,1 / 20,1 / 19,8 | -0,2 / -1,7 / -0,8 | 7,8 / 8,7 / 6,4 | 26,2 / 21,2 / 23,2 |
| Flanke | 4327 (61) | 3,3 / 3,2 / 1,6 | -44,2 / -46,5 / -28,9 | 27,2 / 26,8 / 19,7 | 3,3 / 6,2 / 2,0 | 1,3 / 1,7 / 0,8 | 19,5 / 14,9 / 4,6 |
| Brust/Bauch | 261 (12) | 0,0 / 0,0 / 0,0 | -53,8 / -54,8 / - | 29,0 / 28,3 / - | 7,6 / 7,5 / - | 0,0 / 0,0 / 0,0 | 47,5 / 48,1 / 0,0 |
| Vorderbeine | 4006 (49) | 0,0 / 0,0 / 2,9 | -54,2 / - / -34,4 | 25,8 / - / 22,9 | -5,2 / - / 10,1 | 0,0 / 0,0 / 1,7 | 4,0 / 0,0 / 11,5 |
| Hinterbeine | 7519 (89) | 2,5 / 2,4 / 0,3 | -53,3 / -51,0 / -35,6 | 30,0 / 27,5 / 20,0 | -1,2 / 1,7 / -3,1 | 2,4 / 2,1 / 0,2 | 20,4 / 9,3 / 2,3 |
| Hals | 0 | nicht sichtbar (< 150 px) | | | | | |
| Kopf | 125 | nicht sichtbar (< 150 px) | | | | | |
| Schwanz | 10957 (109) | 7,5 / 5,6 / 17,1 | -38,9 / -39,4 / -31,0 | 27,9 / 26,8 / 21,7 | 9,0 / 13,0 / 2,1 | 1,9 / 2,1 / 10,9 | 27,1 / 19,9 / 42,4 |
| Figur | 28536 (270) | 4,6 / 3,9 / 7,9 | -43,9 / -42,4 / -31,0 | 27,9 / 26,5 / 21,6 | 3,4 / 6,6 / 2,7 | 0,8 / 0,8 / 4,2 | 21,0 / 13,8 / 20,3 |

**Bild 2, roh in der Kette (Videobild V).** Schwelle a\* >= 30 (Neutral-Probe p99). Spalten je Mass: Vorlage O | Rundlauf R (3 Saaten) | V3 r3 (3 Saaten).

| Region | px (Bloecke) | Rot eng % | Farbton a\*-Klasse Grad | C\* Rotklasse | dL Rot - Haut (L\*) | groesste Rotflaeche eng % | a\*-Klasse % (rot + mauve) |
|---|---|---|---|---|---|---|---|
| Ruecken | 3357 (54) | 18,4 / 13,0 / 5,1 | -1,2 / 0,5 / 27,7 | 34,7 / 32,9 / 36,1 | 9,7 / 10,3 / 9,3 | 11,0 / 6,6 / 3,8 | 18,4 / 13,0 / 5,1 |
| Flanke | 1008 (18) | 0,0 / 0,0 / 0,0 | - / - / - | - / - / - | - / - / - | 0,0 / 0,0 / 0,0 | 0,0 / 0,0 / 0,0 |
| Brust/Bauch | 5157 (84) | 3,7 / 3,2 / 0,3 | 11,7 / 11,5 / 28,4 | 35,9 / 33,8 / 38,0 | 8,2 / 7,7 / 12,2 | 3,6 / 3,2 / 0,3 | 3,7 / 3,2 / 0,3 |
| Vorderbeine | 12142 (152) | 49,1 / 55,5 / 68,7 | 28,6 / 27,4 / 28,2 | 44,9 / 43,4 / 45,1 | 12,1 / 13,2 / 15,9 | 25,9 / 31,7 / 42,0 | 49,1 / 55,5 / 68,7 |
| Hinterbeine | 64 | nicht sichtbar (< 150 px) | | | | | |
| Hals | 3856 (62) | 0,4 / 0,0 / 0,0 | 9,0 / - / - | 30,9 / - / - | 1,1 / - / - | 0,3 / 0,0 / 0,0 | 0,4 / 0,0 / 0,0 |
| Kopf | 13269 (123) | 1,3 / 1,6 / 2,3 | 13,6 / 18,8 / 29,5 | 34,1 / 34,6 / 40,7 | 9,3 / 14,9 / 10,9 | 0,5 / 0,8 / 1,3 | 1,3 / 1,6 / 2,3 |
| Schwanz | 0 | nicht sichtbar (< 150 px) | | | | | |
| Figur | 38853 (338) | 17,9 / 19,5 / 22,8 | 25,9 / 25,8 / 28,2 | 43,5 / 42,4 / 44,8 | 17,5 / 17,4 / 17,0 | 8,1 / 10,0 / 13,6 | 17,9 / 19,5 / 22,8 |


### 2.3 Neutral, Texel: V3 r3 | RE2-G4

**Neutral (Tafel-03-Licht 0,55 + 0,45 N.L, 3 Winkel zusammen), Texel = Licht herausgerechnet, Schwelle a\* >= 8.** Spalten je Mass: V3 r3 | RE2-G4.

| Region | px V3 / G4 | Rot eng % | Farbton a\*-Klasse Grad | C\* Rotklasse | dL Rot - Haut (L\*) | groesste Rotflaeche eng % | a\*-Klasse % |
|---|---|---|---|---|---|---|---|
| Ruecken | 6329 / 25199 | 23,9 / 31,5 | 27,2 / 37,3 | 34,4 / 33,1 | 9,9 / 27,5 | 6,5 / 15,1 | 23,9 / 31,8 |
| Flanke | 8768 / 15260 | 7,7 / 33,7 | 17,0 / 38,2 | 29,2 / 29,5 | 12,4 / 28,2 | 1,2 / 10,6 | 7,7 / 34,1 |
| Brust/Bauch | 1640 / 7999 | 21,7 / 72,1 | 27,0 / 44,3 | 32,5 / 31,5 | 7,0 / 14,7 | 7,3 / 11,6 | 21,7 / 73,4 |
| Vorderbeine | 11475 / 21631 | 47,9 / 42,9 | 18,5 / 28,8 | 24,0 / 32,5 | 11,5 / 13,7 | 11,4 / 7,0 | 48,1 / 43,4 |
| Hinterbeine | 4699 / 23333 | 5,7 / 25,0 | 8,3 / 26,5 | 15,7 / 25,3 | 4,1 / 17,6 | 1,0 / 7,2 | 5,7 / 25,2 |
| Hals | 793 / 0 | 34,2 / - | 28,2 / - | 33,9 / - | 6,3 / - | 5,4 / - | 34,2 / - |
| Kopf | 6688 / 5217 | 37,9 / 57,5 | 27,7 / 38,4 | 34,8 / 19,8 | 5,4 / 6,1 | 10,0 / 28,4 | 37,9 / 57,5 |
| Schwanz | 7485 / 0 | 24,0 / - | -3,3 / - | 17,2 / - | 5,2 / - | 3,6 / - | 24,0 / - |
| Figur | 47877 / 98639 | 27,0 / 37,5 | 20,6 / 34,9 | 26,9 / 29,9 | 10,6 / 22,1 | 2,7 / 8,2 | 27,0 / 37,9 |


### 2.4 Lesart

- **Bild 1, Menge:**
  - Rot eng in der Figur ist im Fenster, aber falsch verteilt.
  - Zu viel: Schwanz (+20,0 Punkte gegen O) und Vorderglied (+9,9).
  - Zu wenig: Flanke (-7,0) und Ruecken (-6,4). Dort hat die Vorlage diffuses Mauve-Rot, V3 fast nur schwarze Haut mit duennen Adern.
  - Im Rohvideo (V) dasselbe Bild: Schwanz 17,1 % gegen 7,5 %, Figur 7,9 % gegen 4,6 %.
- **Bild 1, Form:**
  - Die groesste Rotflaeche von V3 (10,6 % der Figur) ist ein zusammenhaengender Block am Schwanzansatz, gelb umrandet in `V3R4_1`.
  - Die Vorlage hat dort einzelne Flecken; ihre groesste zusammenhaengende Flaeche liegt an der Schwanzspitze (2,3 % der Figur).
- **Bild 1, Farbton:** Die a\*-Klasse der Vorlage ist zu 8,0 % der Figur mauve (Farbton < -30), die von V3 zu 0,1 %. Daher kommt V3 -0,2 gegen O -15,8 Grad.
- **Bild 2:**
  - Die Haut von V3 ist dunkler (T-L\* Figur 18,0 gegen 22,1). Die Adern (L\* 19,1) liegen ueber der Haut (15,5).
  - In der Vorlage liegt das Rot (21,1) unter der Haut (23,6): das Rot ist Schatten- und Feuerschein-Ton, keine hell leuchtende Ader.
  - Der Farbton liegt am Koerper bei -5..+19 Grad, bei V3 bei 30.
  - Nur die Klauen (Vorderbeine) sind in der Vorlage oranger (36,6 Grad, Feuerschein, K9) als bei V3 (28,5).
  - Ohne die Klauen: Arm-Teil V3 Rot 84,4 % gegen O 66,0 %, Farbton 25,9 gegen 36,1, dL +7,8 gegen -4,6. Klauen-Teil V3 79,0 % gegen 64,2 %, 33,2 gegen 37,7 Grad.
- **Rohvideo (V) als Gegenprobe:**
  - Gleiche Richtung: Bild 2 Figur-Farbton 28,2 gegen 25,9; Kopf 29,5 gegen 13,6; Ruecken 27,7 gegen -1,2; Brust 28,4 gegen 11,7 Grad.
  - Bild 1 Figur-Farbton -31,0 gegen -43,9 (V3 weniger violett, also roter).
  - Die Mengen in V haengen an der Helligkeit (V3 ist in Bild 2 dunkler, A0 30 in V). Ruecken und Brust in Bild 2 haben in V **weniger** Rot (5,1 / 0,3 % gegen 18,4 / 3,7 %).
  - Deshalb bindet der T-Raum, der die Helligkeit des Lichts herausnimmt. V ist nur Gegenprobe.

## 3. RE2-G4 gegen die Vorlage

Der G4 ist nur neutral messbar (keine Pose in Bild 1/2). Im Texelraum, gegen V3 r3 neutral:
- **Menge:** Rot eng 37,5 % (V3 27,0 %). Brust/Bauch 72,1 % (V3 21,7 %), Kopf 57,5 % (37,9 %).
- **Farbton:** 34,9 Grad (V3 20,6, Vorlage in der Kette -15,8 / +22,8).
- **Adern:** heller als die Haut um dL +22,1 (V3 +10,6, Vorlage -0,6 / -2,5 in der Kette).
- **Groesste Rotflaeche:** 8,2 % (V3 2,7 %); gelb umrandet in `V3R4_3`.
- **Folgerung:** V3 ist schon weniger rot als G4. "Rot wie die Vorlage" heisst darum weniger G4.
  - Die Haut (dunkle Klasse) traf Bild 1 schon in Messblatt V3 0.1.
  - Von der G4-Uebertragung bleibt die Zeichnung (H1, P1, B10-B17): wo Adern liegen, nicht wie rot und wie hell sie sind.

## 4. Riegel

### 4.1 Misst das Runde-3-Paket "Rot wie die Vorlage"?

| Riegel (r3) | Wert V3 r3 | misst | Befund | Entscheid |
|---|---|---|---|---|
| K4, K4b, K12 (Rotklasse a\* >= 12, C\* >= 15, **L\* >= 20**) | GRUEN | helles Rot | Gleiche Pixel, K12-Klasse gegen RV1, O / V3: Bild 1 Schwanz 6,2 / 5,8 gegen 23,5 / 43,5; Figur 3,4 / 2,5 gegen 14,1 / 20,9; Bild 2 Ruecken 43,0 / 15,1 gegen 53,6 / 71,5. Die L\*-Grenze laesst das dunkle Aderrot von V3 (L\* 15-21) aus und zaehlt das hellere Rot der Vorlage. K12 Schwanz ist zudem auf B1_C_SEITE1_EICH geeicht (Zahlen-Pruefer r3 Hinweis C) | **durch RV1 ersetzt**, als Info weiter ausgeben |
| K11 (Farbton, gleiche Klasse, Bild 1, <= +4,0 Grad) | GRUEN (0,3 / -1,8) | Farbton hellen Rots in Bild 1 | Fenster einseitig, kein Bild 2. Mit a\*-Klasse und Bootstrap-Toleranz: Bild 1 Figur -0,2 gegen <= -1,0 ROT, Bild 2 Figur 29,7 gegen <= 27,9 ROT | **durch RV2 ersetzt** |
| K1v2 / K2v2 (Chroma / ab-Streuung Figur Bild 1, Untergrenze) | ROT (6,3 / 6,0) | Gesamtbuntheit | Der Mangel liegt in b\*, nicht in a\*: Figur T-Lab V3 (19,1 / 4,4 / 1,1), Vorlage (19,6 / 3,7 / -2,7). V3 hat schon mehr a\*; es fehlt die blau-mauve Haut (Dossier V3.5 Offen 3, K3) | bleiben, **duerfen nicht mit Rot erfuellt werden**: gelten nur zusammen mit RV1/RV2 GRUEN |
| K3 (dE je Zone Bild 1) | ROT (Hinterbein r 10,5) | Lab-Mittel je Zone | Treiber b\* und L\* (Hinterbein r Vorlage b\* -8,3, Variante +2,2, Dossier V3.3), nicht Rot | bleibt; kein Rot-Riegel |
| K6 (Bild 2 Figur dE / Chroma) | ROT (6,8) | Lab-Mittel Figur | V3 (21,0 / 24,6 / 14,5) gegen (25,1 / 20,5 / 9,6): zu dunkel, zu orange. Gleiche Richtung wie RV2/RV3 | bleibt |
| K7, K15, F2t (Unterkiefer, Band) | ROT / GRUEN / ROT | Leuchtstelle Maul | nicht Gegenstand (Leuchtstelle bleibt) | unveraendert |
| B2 (groesste Rotflaeche neutral **>= 3,2 %**, Familienminimum G4/EM30/G5) | ROT (2,6 / 1,98) | Rot als Flaeche, Untergrenze | Die Untergrenze stammt aus der Familie, nicht aus der Vorlage, und verlangt **mehr** zusammenhaengendes Rot. Die Vorlage hat in Bild 1 2,3 % (V3 10,6 %). G4 neutral 8,2 % | **entfaellt**, ersetzt durch RV4 (Obergrenze aus der Vorlage) |
| B9 (Rot im Umrissband <= 11,3 %) | ROT (14,2) | Rot an Umrisskanten | Richtung passt zum Urteil | bleibt |
| N1 (neutral C\* >= 0,75 x EM30), A1 (Atlas-Chroma Koerper) | ROT | Gesamtbuntheit gegen Familie | verlangen mehr Farbe gegen eine blasse Menschenfigur (EM30) | bleiben, **duerfen nicht mit Rot erfuellt werden** (wie K1v2) |
| A3, A4 (Atlas Fleischrot-Anteil, -Farbton) | GRUEN | Atlas gegen Familie | kein Bezug zur Vorlage | unveraendert |
| H1, P1, B10-B17, D1, S1, G0 | - | G4-Herkunft und Zeichnung | "G4-Aderzeichnung und Machart bleiben" (Auftrag) | unveraendert |

### 4.2 Neue Riegel (`farbe_lesung/V3R4_skripte/riegel_rot.py`)

Gemeinsam fuer alle:
- **Kette:** `Kandidat.rendern(n)` -> `video_filter` (Saaten 15 / 12345 / 7) -> `rueck`, Mittel der 3 Saaten.
- **Messmaske und Regionen:** fest (`rot_masken.npz`, 1.2 und 1.5). Schwelle A0 nach 1.3, Fenster aus `ziele.json` (Abschnitt 5).
- **Bindend:** Regionen mit >= 20 Bloecken und die Figur je Bild.

| # | Mass | Regionen (bindend) | Fenster | Quelle |
|---|---|---|---|---|
| **RV1** | Rot eng % (a\* >= A0, Farbton -30..70) | Bild 1: Ruecken, Flanke, Vorderbeine, Hinterbeine, Schwanz, Figur. Bild 2: Ruecken, Brust/Bauch, Vorderbeine, Hals, Kopf, Figur | O +- (2 sigma + \|R - O\|), Untergrenze >= 0 | Messung 1.3-1.6; Untergrenze haelt "bedrohlich": Die Adern duerfen nicht verschwinden |
| **RV2** | Farbton der a\*-Klasse (Grad) | wie RV1, ohne Bild 1 Vorderbeine (Rundlauf ohne Rotklasse) | O +- (2 sigma + \|R - O\|) | wie RV1 |
| **RV3** | dL = L\*(a\*-Klasse) - L\*(uebrige) | wie RV2 | O +- (2 sigma + \|R - O\|) | wie RV1 |
| **RV4** | groesste zusammenhaengende Flaeche "Rot eng", % der Region | wie RV1; Bild 1 Ruecken nur Info (F1-Hof, 1.5) | <= max(1,5 x O, O + 3) + \|R - O\| + Saaten-Spannweite | V1 K4 / V3 K12 |
| RV5 | C\* der a\*-Klasse | alle, **nur Info** | O +- (2 sigma + \|R - O\|) | Haengt in der Kette an der Aderbreite, nicht an der Texelfarbe: Buntheit x 0,6 senkt C\* in Bild 1 nur von 12,6 auf 10,5 (`probe.json`). Bindend wuerde es breitere Rotflaechen verlangen, gegen "G4-Aderzeichnung bleibt" |

**Gegenproben** (`gegen_*.txt`):

| Lauf | bindend GRUEN | erwartet | Ergebnis |
|---|---|---|---|
| Rundlauf der Vorlage (`--rundlauf`) | **45/45** | alle GRUEN | Exit 0, wie verlangt |
| V3 r3 (`farbe/K5c_farbe_v3`) | **21/45** | ROT (Nutzer: zu viel Rot) | Exit 1, ROT: RV1 6, RV2 8, RV3 5, RV4 5 |
| K5c ohne Farbe (`K5c/`, Violett-Grau) | **30/45** | ROT in die andere Richtung | Exit 1. Bild 2 Figur Rot 21,6 % < 39,0, Hals 5,5 < 17,9, Kopf 10,7 < 25,4. Die Untergrenzen trennen "zu wenig" |

Die Riegel trennen also in beide Richtungen.

## 5. Zielwerte je Region (Texelraum T, Kette; `ziele_tabelle.md`)

O = Vorlage. Fenster nach 1.6. V3 = V3 r3, Mittel der 3 Saaten. "(Info)" = nicht bindend (1.2, 1.5, RV5).
Jede Region hat ihre Zeile in dem Bild, in dem sie sichtbar ist (Hals und Kopf nur Bild 2, Schwanz nur Bild 1).

| Bild / Region (Bloecke) | RV1 Rot eng % | RV2 Farbton a\*-Klasse Grad | RV3 dL Rot - Haut | RV4 groesste Rotflaeche eng % (Obergrenze) | RV5 C\* Rotklasse (Info) |
|---|---|---|---|---|---|
| Bild 1 Ruecken (38) | O 27,1, 11,2..42,9; V3 20,7 GRUEN | O 9,5, -13,2..32,2; V3 15,8 GRUEN | O -1,2, -5,6..3,2; V3 -0,4 GRUEN | O 21,2, <= 33,8; V3 8,3 GRUEN (Info) | O 14,7, 12,2..17,2; V3 13,6 GRUEN (Info) |
| Bild 1 Flanke (61) | O 12,9, 0,3..25,5; V3 5,9 GRUEN | O -19,5, -33,6..-5,4; V3 5,8 **ROT** | O -0,4, -5,1..4,3; V3 2,5 GRUEN | O 4,3, <= 7,5; V3 1,9 GRUEN | O 15,7, 12,6..18,8; V3 12,5 **ROT** (Info) |
| Bild 1 Brust/Bauch (12) | O 11,5, 0,0..38,7; V3 0,0 GRUEN (Info) | O -41,1, -67,5..-14,7; V3 ohne Rotklasse (Info) | O 2,3, -10,5..15,1; V3 ohne Rotklasse (Info) | O 10,7, <= 27,0; V3 0,0 GRUEN (Info) | O 14,4, 11,7..17,1; V3 ohne Rotklasse (Info) |
| Bild 1 Vorderbeine (49) | O 2,4, 0,0..7,1; V3 12,3 **ROT** | kein Ziel (Rundlauf ohne Rotklasse) | kein Ziel (Rundlauf ohne Rotklasse) | O 1,3, <= 5,5; V3 6,5 **ROT** | kein Ziel (Rundlauf ohne Rotklasse) |
| Bild 1 Hinterbeine (89) | O 5,1, 0,0..11,1; V3 2,2 GRUEN | O -30,4, -81,9..21,1; V3 3,9 GRUEN | O -4,1, -9,7..1,5; V3 -2,7 GRUEN | O 2,6, <= 6,4; V3 1,1 GRUEN | O 16,0, 13,6..18,4; V3 10,9 **ROT** (Info) |
| Bild 1 Schwanz (109) | O 23,5, 12,5..34,5; V3 43,5 **ROT** | O -12,3, -25,9..1,3; V3 -1,0 GRUEN | O 3,9, -0,6..8,4; V3 -0,7 **ROT** | O 6,0, <= 9,7; V3 22,9 **ROT** | O 15,7, 13,0..18,4; V3 12,7 **ROT** (Info) |
| Bild 1 Figur (270) | O 14,1, 6,9..21,4; V3 20,9 GRUEN | O -15,8, -30,6..-1,0; V3 -0,2 **ROT** | O -0,6, -4,2..3,0; V3 2,0 GRUEN | O 2,3, <= 5,5; V3 10,6 **ROT** | O 15,5, 13,7..17,4; V3 12,6 **ROT** (Info) |
| Bild 2 Ruecken (54) | O 53,6, 42,0..65,2; V3 71,5 **ROT** | O -5,3, -12,9..2,3; V3 30,6 **ROT** | O -7,4, -15,6..0,8; V3 1,8 **ROT** | O 39,7, <= 62,2; V3 61,4 GRUEN | O 30,1, 25,4..34,8; V3 30,2 GRUEN (Info) |
| Bild 2 Flanke (18) | O 52,3, 10,3..94,3; V3 24,1 GRUEN (Info) | O 10,4, -0,4..21,2; V3 26,1 **ROT** (Info) | O -10,3, -17,2..-3,4; V3 3,1 **ROT** (Info) | O 50,7, <= 108,2; V3 10,5 GRUEN (Info) | O 28,7, 22,2..35,2; V3 25,0 GRUEN (Info) |
| Bild 2 Brust/Bauch (84) | O 42,8, 17,8..67,7; V3 74,9 **ROT** | O 7,9, -0,1..15,9; V3 30,6 **ROT** | O -4,3, -10,0..1,4; V3 0,9 GRUEN | O 9,4, <= 17,8; V3 24,7 **ROT** | O 26,7, 22,8..30,6; V3 28,2 GRUEN (Info) |
| Bild 2 Vorderbeine (152) | O 65,4, 56,4..74,5; V3 82,3 **ROT** | O 36,6, 34,5..38,8; V3 28,5 **ROT** | O -4,2, -11,4..3,0; V3 -3,9 GRUEN | O 30,9, <= 49,0; V3 39,8 GRUEN | O 36,1, 31,9..40,3; V3 36,0 GRUEN (Info) |
| Bild 2 Hals (62) | O 45,1, 17,9..72,3; V3 87,1 **ROT** | O 11,0, 1,7..20,3; V3 30,7 **ROT** | O -3,9, -6,3..-1,5; V3 0,8 **ROT** | O 14,3, <= 29,8; V3 51,6 **ROT** | O 25,9, 21,7..30,1; V3 29,0 GRUEN (Info) |
| Bild 2 Kopf (123) | O 59,0, 25,4..92,5; V3 55,6 GRUEN | O 18,9, 13,1..24,7; V3 30,4 **ROT** | O -1,5, -4,4..1,4; V3 3,0 **ROT** | O 55,7, <= 118,3; V3 32,8 GRUEN | O 27,1, 23,1..31,1; V3 28,9 GRUEN (Info) |
| Bild 2 Figur (338) | O 56,9, 39,0..74,8; V3 70,2 GRUEN | O 22,8, 17,7..27,9; V3 29,7 **ROT** | O -2,5, -7,2..2,2; V3 3,5 **ROT** | O 41,4, <= 87,1; V3 52,4 GRUEN | O 30,5, 27,7..33,3; V3 31,5 GRUEN (Info) |

## 6. Ketten-Vorhersage: Spielraum fuer den Bauer (`probe.py`, `probe_auswert.json`; kein Ziel, kein Bau)

**Verfahren:**
- Im Szenenrender von V3 r3 werden die roten Texel (Texel-a\* >= 8) im Speicher umgefaerbt: Farbton H, Buntheit x c, L\* bleibt.
- Das Licht je Pixel (sauber / Albedo) bleibt. Dann `video_filter` mit Saat 15, `rueck` und die Masse aus 1.4.
- Gezaehlt werden die bindenden RV1-RV4-Zeilen des jeweiligen Bildes.
- Das ist eine Pixelprobe, kein CLUT-Bau. H2 (Rangfolge der Eintraege) ist dabei nicht geprueft.

| Bild | Probe | bindend GRUEN | ROT |
|---|---|---|---|
| 1 | wie V3 r3 (G4-Farbton, c 1,0) | 14/21 | Flanke Farbton, Vorderglied Menge + Flaeche, Schwanz Menge + Flaeche, Figur Farbton + Flaeche |
| 1 | H -30, c 1,0 | 18/21 | Ruecken Farbton -21,8; Schwanz Flaeche 19,1; Figur Flaeche 7,5 |
| 1 | **H -20, c 0,6** | **19/21** | Ruecken Menge 11,0 (Grenze 11,2); Hinterbeine Farbton 30,2 |
| 1 | H -10, c 0,6 | 18/21 | Flanke Farbton -5,0; Hinterbeine Farbton 22,9; Schwanz Flaeche 12,4 |
| 2 | wie V3 r3 (G4-Farbton, c 1,0) | 8/24 | - |
| 2 | H -10, c 0,6 | **15/24** | Ruecken Farbton 13,5, dL 0,9; Brust Farbton 18,9; Vorderbeine Menge 76,0, Farbton 18,5; Hals dL 1,2, Flaeche 46,2; Kopf dL 2,9; Figur dL 4,5 |
| 2 | H 0, c 0,6 | 15/24 | aehnlich, Farbton am Ruecken 18,6 |
| 2 | H -20, c 0,6 | 14/24 | Ruecken Farbton 7,6, dL 1,0; Vorderbeine Menge 74,7, Farbton 14,8; Hals dL 1,3, Flaeche 44,5; Kopf Farbton 11,6, dL 2,9; Figur Farbton 13,6, dL 4,8 |

**Lesart:**
- Fuer die Bild-1-Zeilen (Schwanz, Becken, Wirbel, Hinterbeine, Vorderglied r) liegt das Ziel bei Rot-Texeln mit etwa **-20 Grad** (Mauve-Rot statt -3,2 Grad)
  und **0,6-facher Buntheit**.
- Fuer die G4-Zeilen (in Bild 2 sichtbar) gilt: Farbton -10 Grad statt 30 und Buntheit x 0,6 bringen RV1 an Ruecken, Brust, Hals und Kopf ins Fenster,
  RV2 an Hals und Kopf. Ruecken (Fenster bis 2,3 Grad) und Brust (bis 15,9) brauchen kaeltere Rot-Texel: bei -20 Grad ist die Brust GRUEN (14,3), der Ruecken noch nicht (7,6).
- **RV3 bleibt in Bild 2 ROT, solange die Adern heller sind als die Haut.**
  - Die Vorlage hat dL -2,5, die Probe +4,3..+4,8.
  - Mit Farbe allein geht das nicht. Es braucht dunklere Aderfarben (L\* der Rot-Eintraege) oder eine hellere Haut (K6: Figur L\* 21,0 gegen 25,1).
  - Der dunkle Grundton in Bild 1 (dunkle Klasse 63,6-65,3 %, Messblatt V3 0.1) begrenzt den zweiten Weg.
- **Vorderbeine in Bild 2:** Die Klauen brauchen eher mehr Orange (Vorlage 37,7 Grad), der Arm weniger Rot. Eine gemeinsame Umfaerbung trifft beides nicht.

## 7. Offen (nur, was kein Bild beantwortet)

1. **Weg zu RV3 in Bild 2:** dunklere Adern oder hellere Haut (K6). Beides ist messbar, die Wahl ist Gestaltung. Rueckfrage an den Bauer/Pruefer, nicht an den Nutzer.
2. **Mauve-Haut in Bild 1** (Brust/Bauch 35,6 %, Flanke 8,4 % der Region mauve, V3 0 %):
   - Sie gehoert zur Vorlage (K3, Dossier V3.5 Offen 3: "blaeuliche Haut ohne G4-Hautzeile").
   - Sie ist kein Rot und nicht Gegenstand dieses Nachtrags.
   - K1v2/K2v2/N1/A1 werden erst gruen, wenn sie kommt, nicht durch mehr Rot (4.1).
3. **Messblatt V3 7.1** (Gesicht oder Becken in Bild 1) bleibt offen; die Leuchtstellen sind hier ausgenommen.
4. **Hals und Kopf** sind nur in Bild 2 sichtbar, also nur unter Feuerlicht gemessen.
   - Ihr Farbton-Ziel (11,0 / 18,9 Grad) enthaelt den Feuerschein.
   - Ein Ziel ohne Feuer gibt kein Bild her.
