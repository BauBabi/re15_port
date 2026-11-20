# Pruefung K5f, Runde 1 - Sicht-Pruefer (Auge des Nutzers, Ziel: widerlegen)

Datum 2026-11-09. Pruefling: C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/k5f/K5f/BAHNSTEIG_FORM.EMD (sha256 9045b6dc...) / .TIM (sha256 64b8d8bd...).
Spiel-Asset unveraendert geprueft: re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD f277916d..., .TIM 31a35e66... (sha256sum, git status sauber).

## Ergebnis: NICHT BESTANDEN

K5f ist ein klarer Schritt weg von "komplett ausgeblasst" (Kontrast verdoppelt, dunkler). Aber K5f bringt
**schwarze Flaechen**: Die 6 Flaechen an der Hals-Unterseite sind von rot zu fast reinem Schwarz geworden
(der Streifen ist also noch da, nur schwarz), und Lippe, Unterkiefer und der Rand um den Kopf sind ebenfalls schwarz statt grau.
Der Nutzer hat schon bei K5e "um den Fleck herum ... schwarz gemacht" abgelehnt und fuer den Unterkiefer "dunkel/grau",
fuer die Lippe "durchweg grau/blau" verlangt. Das wuerde er ablehnen.

## Gesehen

- Tafeln 01-08 unter C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/k5f/ (alle mit Read angesehen)
- Messbild C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/k5f/messung/hals_unterseite.png
- Eigener Spiellauf (re15_port/tools/form_spiel.sh, eigene exe-Kopie, RE15_RE2_ASSET_ROOT auf K5f, gdigrab Versuch 1, kein AUTOSHOT/SOFTWARE_RENDER, nur eigene PID), 5 Bilder, roh, unter
  C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f/sicht_r1/ :
  k5f_pose1_r5080.png (Clip 0 Bild 11), k5f_ruhe_r5080.png (Clip 2), k5f_pose2_r5090.png (Clip 12 Bild 0), k5f_ruhe_r5090.png (Clip 2), k5f_griff_r5090.png (Clip 27 Bild 20, Hals-Unterseite);
  Tafeln tafel_eigen_roh.png (ganze Fenster) und tafel_eigen_nah.png (Ausschnitte x2 nearest). Jeder Lauf: "[enemy] Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips", typ 0x30 in der verlangten Pose.
- Andere Gegner im Raumlicht: Tafel 08 (Zombie EM10 1140, Gorilla EM27 11C0, Hangar-Birkin EM30 50A0, Tyrant EM2B 50A0, Framedumps des echten Renderers).

## Messungen (eigene, Lab aus sRGB, D65)

### M1 Familie unter weissem Licht (Tafel 07, nur Figurpixel, Hintergrund 92/92/92 ausgenommen), 4 Winkel

| Modell | L* Mittel | L* Std (Kontrast) | Farbton Mittel | C* Mittel | C* Std | Anteil L* < 8 |
|---|---|---|---|---|---|---|
| RE2-G4 Original | 17,0-21,7 | 14,5-17,1 | +29..+32 Grad | 9,6-14,5 | 10,9-15,4 | 20-33 % |
| Hangar-Birkin EM30 | 24,5-28,9 | 15,3-17,3 | +18..+23 Grad | 9,9-12,5 | 6,9-8,0 | 12-16 % |
| K5e heute | 23,5-25,1 | 6,9-7,8 | -32..-34 Grad | 8,4-8,9 | 3,0-3,3 | 0,1-0,2 % |
| K5f | 19,5-21,6 | 13,5-14,1 | -25..-27 Grad | 7,4-8,3 | 6,4-6,8 | 10-14 % |

Kontrast und Dunkelanteil liegen jetzt im Band der Originale (K5e lag bei der Haelfte). Der Farbton bleibt auf der Gegenseite
der Original-Familie (violett/mauve statt warm braun-rot, Abstand rund 55 Grad).

### M2 Hals-Unterseite (6 Flaechen Mesh 22 qf 1/8/9/194/201/209, gruene Maske aus messung/hals_unterseite.png), weisses Licht

| Ansicht | K5e_f1 Hals-6 | K5f Hals-6 | K5f Rest der Figur |
|---|---|---|---|
| von unten | L* 24,1 a* 32,0 b* 14,9 (rot) | L* 3,6 a* 0,2 b* 0,0; 50 % der Pixel RGB-Max <= 12 | L* 18,8 a* 6,2 b* -2,4 |
| unten seitlich | L* 18,3 a* 27,1 b* 12,1 | L* 2,6; 97 % RGB-Max <= 12 | L* 17,8 |
| unten 3/4 | L* 19,5 a* 29,3 b* 13,1 | L* 2,8; 88 % RGB-Max <= 12 | L* 19,9 |

Das Rot ist weg (a* 27-32 -> 0), aber die Flaechen sind fast schwarz (L* 2,6-3,6 gegen Haut 17,8-19,9, Differenz rund -16 L*).
Das ist keine "Haut wie der Rest", sondern derselbe Streifen in Schwarz.

### M3 Schwarze Pixel am Kopf (Tafel 05, 6 Kopf-Kacheln, Anteil RGB-Max <= 12 an der Figur)

K5c 0,1-1,1 % | K5e 0,0 % | **K5f 3,1-9,1 %** (Front 90/-45: 9,1 % bei Clip 2, 7,8 % bei Clip 1).
Sichtbar als schwarzes Band an Lippe/Unterkiefer und als schwarzer Rahmen um den Kopf von vorn.

### M4 Spiel 5090 (gdigrab, gleicher Ausschnitt, gleiche Szene; K5e = spiel/heute_*.png gegen meine K5f-Bilder)

- Griff Clip 27 Bild 20, Ausschnitt x 380-640 y 0-240: RGB-Max <= 12: K5e 332 px, **K5f 4414 px**; <= 20: K5e 2681, K5f 10093. Im Bild ein schwarzer Riemen diagonal ueber Hals und Unterkiefer (tafel_eigen_nah.png, zweites Feld).
- Bild-2-Pose Clip 12, Ausschnitt x 300-560 y 250-420: <= 12: K5e 990, K5f 1521; <= 20: K5e 2283, K5f 4968.

### M5 CLUT (TIM, Kopf ab Byte 20, 15 bit, je Zeile 256 Eintraege)

- K5f Zeile 10 (Teil von "Grau" fuer Lippe/Unterkiefer, 81 Flaechen): 207 von 256 Eintraegen mit Kanal-Max <= 1 (von 31), also schwarz.
- K5f Zeile 1 (Hautzeile, auf die HU1 die 6 Hals-Flaechen legt, 249 Flaechen): 209 von 256 Eintraegen mit Kanal-Max <= 3.
- Zum Vergleich K5e_f1: Zeile 1 hat 0 Eintraege <= 3; nur Zeile 4 (6 Flaechen) hat 206.
Damit ist die Ursache der schwarzen Flaechen belegt: Die Zeilen 1 und 10 von K5f sind zum grossen Teil schwarz.

### M6 Riegel des Bauers (K5f/riegel_k5f.txt)

38/39 GRUEN, W5 Krallen ROT (dE76 7,04 > 6; Kralle Lab [20,5, 0,0, 0,0] neutral grau gegen Hand [17,8, 5,4, -3,7]).
HU2/HU3 pruefen nur "kein Warm-Rot" und sind deshalb gruen, obwohl die Flaechen schwarz sind; W4 prueft Rotklasse und eine L*-Obergrenze,
keine Untergrenze. Schwarz faellt durch beide Netze.

## Antworten

**(1) Wie die anderen Gegner, nicht blass, Kontrast und Tiefe?** - Abweichung klein.
Im Spiel 5080 (k5f_pose1_r5080.png, k5f_ruhe_r5080.png) dunkel-violett mit hellen Knien, nicht mehr ausgewaschen, deutliche Tiefe;
in 5090 vor dem Feuer dunkel mit lesbarer Hautstruktur. Kontrast unter weissem Licht L*-Std 13,5-14,1 im Band von G4/EM30 (M1).
Rest-Abweichung: Farbton -25..-27 Grad (violett/mauve) gegen +18..+32 Grad aller Original-Gegner; unter weissem Licht (Tafel 06/07)
wirkt K5f rosa-lila mit rosa-weissen Lichtern. Im Spiellicht faellt das deutlich weniger auf, und die Farbrichtung ist dieselbe wie im akzeptierten K5e.

**(2) Nicht bunt, ruhig, keine aufgemalten Flecken?** - Abweichung klein.
C*-Std 6,4-6,8 liegt zwischen K5e (3,0-3,3) und G4 (10,9-15,4), etwa bei EM30 (6,9-8,0); keine Inseln (H1: 0 Texel neu).
Unter weissem Licht nah (Tafel 05/06) aber auffaellig gesprenkelt: viele kleine rosa-weisse Lichter auf dunkler Haut (Beine, Schwanz, Hals),
eher Tarnmuster als ruhige Haut. Im Spielfenster ist es ruhig.

**(3) Wuensche sichtbar?** - Abweichung, die der Nutzer ablehnen wuerde.
Gelenke weisslich sichtbar (Tafel 01/03, Knie hell; W1 dL* 9,45). Schwanzspitze fleischfarben sichtbar (Tafel 06, W2). Buckel leicht roetlich (Tafel 03, schwach).
Krallen ohne Rot, aber neutral grau mit weissem Salz-und-Pfeffer-Rauschen an der rechten Klaue (Tafel 03 "Krallen ohne Filter", Tafel 04, k5f_pose2_r5090.png; W5 ROT).
**Lippe/Unterkiefer schwarz statt grau** (M3, M5 Zeile 10): Der Nutzer wollte "Lippe durchweg grau/blau" und den Unterkiefer "dunkel/grau".

**(4) Neue Fehler?** - Abweichung, die der Nutzer ablehnen wuerde.
Schwarze Flaechen an Lippe, Unterkiefer, Kopfrand und Hals-Unterseite (M2-M5). Im Spiel erscheinen sie als schwarzer Riemen in der Griff-Pose und als schwarzes Band ums Maul in 5090 Ruhe.
Kein Mosaik, keine Naht gesehen; F1 am Becken ohne sichtbaren Ring (Tafel 01, W8 gruen). Klauen-Rauschen siehe (3).

**(4b) Hals-Unterseite ohne rote Streifen?** - Abweichung, die der Nutzer ablehnen wuerde.
Das Rot ist vollstaendig weg (M2: a* 27-32 -> 0; eigenes Griff-Bild ohne Rot). Die Flaechen haben aber L* 2,6-3,6 statt Haut 18-20:
An derselben Stelle steht jetzt ein schwarzer Streifen. Der Nutzer will dort "Haut wie der Rest", keinen anders gefaerbten Streifen.

**(5) Klarer Schritt weg von "komplett ausgeblasst"?** - wie gewuenscht.
L*-Std 6,9-7,8 -> 13,5-14,1, Anteil sehr dunkler Pixel 0,1 % -> 10-14 %, Mittel L* 24-25 -> 19,5-21,6 (M1). Im Spiel sichtbar dunkler und plastischer.

## Hinweise an den Bauer (Befund, keine Werte geraten)

1. Die 6 Hals-Flaechen (Mesh 22 qf 1/8/9/194/201/209) muessen Haut zeigen, die so hell ist wie die Haut daneben: entweder eine Hautzeile, deren Eintraege an genau diesen Texeln nicht schwarz sind (M5: Zeile 1 ist zu 209/256 fast schwarz), oder die Transformation darf die dunklen Eintraege der Zeile 1 nicht auf Schwarz druecken.
2. Zeile 10 ("Grau" fuer Lippe/Unterkiefer) hat 207/256 schwarze Eintraege. Grau oder blau-grau heisst eine L*-Untergrenze (nicht dunkler als die dunkle Haut daneben), nicht Schwarz.
3. Riegel ergaenzen: HU2/HU3 und W4 brauchen eine L*-Untergrenze (Gegenprobe: K5f muss dort ROT werden).
4. Klauen: Rauschen und das neutrale Grau (W5) an die Hand angleichen.
