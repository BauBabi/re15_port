# SF Pruefung V8 Runde 3 - Zahlen-Pruefer (2026-11-09, nicht committet)

Pruefgegenstand: Zweig r41/sichel_v8, HEAD 100900c97 (Commits f433b48ce, 13f57529b, 68eb0b120, 61cc6af85, 119614f7b, 100900c97; Diff gegen 3db35d865).
Spiel-Asset EMD/EM30_HANGAR.EMD sha256 66806bc4ae2537809ce2ebd827a661974ca560d024fccaca4ee79c559134fab3 (271 012 Byte) = Pin release/em30_hangar.sha256
= git show HEAD. Gegenproben: V7 = git show 6a25db27b (319bbd17), V8 r2 = git show 3db35d865 (d0ef852c).

**Ergebnis: NICHT BESTANDEN.** Rot sind vier Punkte:
- S10/O5: Die Wurzel ist von hinten weiter sichtbar. Eigene Zaehlung in den echten Framedumps: 46 090 px in F328-F346; V8 r2 hatte 48 331 px.
- O1-Wortlaut: Der Oberarm (W)-rechts liegt mit 24,3 % nicht im Vorlage-Band 35-59 %.
- O1-Wortlaut: Der Rumpf (W)-rechts liegt mit 62,2 % ueber dem Original-EM30 in gleicher Pose (49,4 %).
- O3: Ueber dem Kopf klafft von vorn ein neuer Schlitz durch die Figur. In den Framedumps sind es 807 Loch-Pixel, bei V7 3.

Der Bauer meldet S10 selbst rot. Die beiden O1-Teilpunkte hat er im Riegel zu "nur ausgegeben" abgeschwaecht; den Schlitz misst kein Riegel.

## Verfahren
Eigener Pruefer-Code liegt in C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr3/.
Nichts davon ist aus re15_port/tools importiert, riegel_v8.py wurde nicht gestartet.
- pl.py, form.py, form_j.py, s6.py, kette3d.py, s7lit*.py: Pruefer-Code aus Runde 1/2, unveraendert.
- Neu fuer Runde 3:
  - s7r3.py: vergleicht jede Flaeche mit dem Original-EM30 (gleiche Topologie, Mesh 8 104, Mesh 13 56, Mesh 14 37 Flaechen).
  - huegelflaechen.py, huegel3.py, s9.py (Vorderansicht offline), s9szene.py (Vorderansicht in den Framedumps), s10.py.
- Marker-Assets fuer echte Framedumps (lauf_v4.sh, BAUM r41_sichel_v8, eigene exe-Kopie):
  - MK_A: CLUT-Zeile 487 (nur Schwanz) magenta.
  - MK_B: wie MK_A, dazu die 22 Wurzelflaechen von Mesh 16 (Band Ring 0 -> Ring 1 und Kegel) auf Zeile 480. Das Alpha-Muster ist gleich, 0 abweichende Texel.
  - Wurzel sichtbar = magenta in A und nicht in B. Das ist eine echte z-Test-Zaehlung des Port-Renderers.
  - GR_*: alle Zeilen gruen, fuer die Silhouette.
  - Die Zeilen "Schwanz B y" der Markerlaeufe sind gleich dem normalen Lauf (0 Unterschiede).

## Soll/Ist

| Punkt | Soll | V8 r3 (eigen) | V8 r2 | V7 | Bewertung |
|---|---|---|---|---|---|
| S1 Kontur T | <= 4 / <= 8 | 3,05 / 7,0 | 1,82 / 5,0 | 16,77 / 50 | GELB: im Soll, aber Rueckschritt gegen den zu haltenden r2-Wert 1,8/5,0 |
| S2 a/b/c | <= 4/6; <= 2; <= 3 | 2,02/5; 1,6 bei x 332; 3,0 bei x 396 | 1,70/5; 1,7; 3,0 | 7,30/13; 6,6; 7,0 | GRUEN (c auf der Schwelle; Halbpixel: a max 7 in 1 von 7) |
| S3 a/b/d/e | <= 4; 5-10 bei x 428-445; <= 2; <= 3 | 4,0; 6,6 bei x 429; 1,5; 3,0 | 4,0; 6,8 bei 437; 1,5; 3,0 | rot | GRUEN, Kuppe fragil: bei Halbpixel-Verschiebung in 3 von 7 Varianten 5,0, H-Ort x 428/429 am Fensterrand |
| S4 Breite | Wurzel >= 55; s 90-130 >= 36; unten >= 27 / >= 9; Ende <= 10 | 60,6; 40,4/38,2/37,9; 31,6/31,0; 18,7/18,4/10,7/10,6; 5,6, monoton | unten 20,4 / 8,6-6,2 | rot | GRUEN (O4 erfuellt) |
| S5 Gesicht | bytegleich V7 | Mesh 9 4280 Byte gleich, Zeile 486 gleich, Texel gleich, Knochen 9 gleich | gleich | - | GRUEN |
| S6 Boden | 0/912, Szene y <= 0 | 0/912 (max -24 E.); Szene 144 Takte, tiefstes y -118, 0 Takte darunter | 0; -66 | 70/82; -108 | GRUEN |
| O6 Gelenk h69/h70 | <= 25 Grad | 25,1 / 25,0 (Takt-Max gesamt 25,14; 0 Takte > 25,5) | 31,7 / 39,3 | - | GRUEN (0,1 Grad = Rundung der ganzzahligen Log-Koordinaten) |
| S7a/O1 Herkunft | jeder Eintrag und Texel aus dem Original | siehe unten | - | - | GRUEN, Huegel GELB |
| O1 Oberarm 13 | Vorlage-Band 35-59 UND <= Original 57,5 | **24,3 %** | 43,0 | 22,9 | **ROT** (nach O1-Wortlaut) |
| O1 Rumpf 8 (W)-rechts (lok z < 0) | 25-80 UND <= Original gleiche Pose 49,4 | **62,2 %** (lok x < 0: 71,9) | 5,0 | 0,2 | **ROT** (ueber dem Original) |
| O1 Unterarm 14 | <= Original 22,1, kein Band | 20,4 %, alle 37 Flaechen = Original-UV/Zeile 482 | 17,4 (Band) | 0,0 | GRUEN |
| O1 Flecken (a* >= 15, >= 4 px, ohne Licht) | <= Original | Knochen 8+13: 45 (6,02 je 1000 px), Original 27 (6,09); 8+13+14: 51 (5,19), Original 31 (5,03) | 26 / 32 | 15 | GELB: absolut deutlich mehr; je Flaeche 8+13 knapp darunter, mit 14 3 % darueber. Bauer: 3,98 <= 4,37 erst nach Verkleinern |
| O2 Huegel rot | oberste 8 px x 428-445, a* >= 10 wie Vorlage 58,3 % | 78,5 % (Median a* 24,3, Vorlage 12,1) | 0,0 | 0,0 | GRUEN (eher roeter als die Vorlage) |
| S8 Technik | <= 16 Zeilen, <= 4096 | 16, 4096, Felder 480-495 gueltig, 23/23/21 | - | - | GRUEN |
| S9/O3 Kontur von vorn | <= 2 px ueber V7, keine neue Ecke | P1: 2 px (x 427), keine neue Ecke; Szene F344-F378: max 4 px bei 960x720 (2,7 px bei 640), in 7 Bildern > 3 px | P1 17 px, 10 Ecken; Szene 14-27 px | Referenz | GELB (Kontur), siehe O3-Schlitz |
| O3 Schlitz ueber dem Kopf | keine neue Ecke/Kerbe | **807 Loch-Pixel** in F344-F378 (F350 76, F351 61, F352 50) | 228 | 3 | **ROT** |
| S10/O5 Wurzel von hinten | 0 | **46 090 px** F328-F346 (je Bild ~3100-3250 in F328-F340) | 48 331 | - | **ROT** (-5 % gegen r2) |

## O1 im Einzelnen (s7r3.py)
- Zeilen 493/494/495 sind gleich den Original-EM30-Zeilen 480/481/482. Kein Eintrag weicht ab; in V8 r2 waren es 186, 131 und 155.
- Die Zeilen 480-492 und die Textur sind gleich V7.
- Unter allen Flaechen der Zeilen 493-495 liegen nur Original-Texel:
  - Mesh 8: 0 von 9 747 Texeln weichen ab
  - Mesh 13: 0 von 11 974
  - Mesh 14: 0 von 6 668
  - Zum Vergleich: Die V7-Textur weicht insgesamt an 23 470 Texeln vom Original ab. Die betroffenen Flaechen liegen nicht darauf.
- Jede Flaeche auf 493-495 traegt die passende Original-Zeile. Bis auf die drei Huegel-Flaechen ist auch die UV gleich dem Original, Flaeche fuer Flaeche.
- Seite:
  - (W)-rechts = Knochenraum lok z < 0 (rel13 z -1136), wie in r2 gemessen. Die Vorgabe "x < 0" in O1 ist an dieser Stelle nicht die Seite.
  - Mesh 8 z < 0 hat 61 Flaechen: 18 Brust (488), 36 auf 493, 7 auf 494. Davon liegt keine mehr auf 480/481.
  - Mesh 8 z >= 0: unveraendert.
- Brust: alle 34 Flaechen auf 488, UV gleich V7.
- Feld C (Seite 0, u 78-127, v 0-100): Es sind 24 Flaechen von Mesh 13, alle auf 480/489 wie in V7. Unter den neuen Zeilen liegt 0-mal Feld C.
- Unterarm: 37 Flaechen gleich dem Original (Zeile 482, gleiche UV). Das gerade Band ist weg.
- Huegel-Flaechen v8, v14 und v18 (UV-Schub, Zeile 493):
  - Ziel ist Seite 0, u 0-18, v 1-22. Das ist nicht Feld C. Kein Texel ist durchsichtig.
  - Anteil a* >= 20: 98,1 / 95,5 / 88,7 %.
  - **GELB:** 52 von 107, 59 von 89 und 94 von 204 Ziel-Texeln nutzt **keine** Original-Flaeche.
    - Die Bytes sind original, zeigen aber einen Bereich, den der RE1.5-Birkin nie darstellt.
    - Die uebrigen Ziel-Texel nutzt das Original selbst mit Zeile 480: Mesh 8 v16/v11/v1/v0 und Mesh 13 v5/v4.

## O3 und O5 in den echten Framedumps
- s10_wurzel_F330_336_340.png zeigt pro Bild links das Spielbild und rechts die Wurzel in Cyan.
  - Von hinten bildet das Wurzelrohr (Mesh 16, Ring 0 -> Ring 1) die ganze Ansatzzone des Schwanzes auf dem Ruecken.
  - Links davon steht eine dunkle, hartkantige Spitze. Der Schwanz liegt weiter wie ein Horn/Schlauch auf dem Ruecken (O7 nicht sichtbar geloest).
  - Der obere Ruecken zeigt grosse, satt rote Polygonflaechen. Das deckt sich mit Dossier Offen 4.
- Der Bauer misst S10 offline nur in Takt F320-F332. Die Bilder F334-F346 (Framedump) fehlen dort; in F334-F341 sind es je ~2900-3150 px.
- s9_kopf_x2.png und kerbe_x4.png (F350-F352, GR-Silhouette):
  - Links neben dem Kopf geht ein schmaler, dunkler Schlitz durch die Figur; durch ihn sieht man die Wand.
  - V7 hat ihn nicht, V8 r2 schwaecher, V8 r3 am staerksten.
  - Der Sicht-Pruefer r2 hatte ihn schon genannt ("dunkle senkrechte Kerbe").
  - Offline in P1 zeigt mein Renderer kein Loch (V8 0 px). Der Fehler tritt nur in der Szenenpose auf und faellt deshalb durch S9.
  - Ueber dem Kopf liegt von vorn jetzt eine grosse, rote, ebene Flaeche (die Huegel-Flaechen). Das ist ein Hinweis fuer die Sicht-Pruefung.

## Szene, Kette, Tests, Code
- Kette: v8_kette.sh mit eigenem Ziel pr3/kette ergibt "V8-KETTE-OK sha256 66806bc4...", EXIT 0, 30 s.
  - Stufen 1-6 und V7 = 319bbd17 stimmen. Der Neubau ist bytegleich mit dem Asset (cmp).
- Bau: "ninja: no work to do", "=== LOCAL-BUILD-OK (build)". Die exe (03:31:55) ist juenger als schwanz_r41.c (03:31:44).
- Testgruppe: "100% tests passed, 0 tests failed out of 8" (305 s).
  - unit_schwanz_r41 -V: "Szenenziel getroffen auf 270, Freigabe-Schritt 541".
- Unit-Suite: "100% tests passed, 0 tests failed out of 1013" (234 s).
- Szene (pr3/szene, lauf_v4.sh, Asset "-", Ende 1110): EXIT 0.
  - state.log 1131 Zeilen, 0 Unterschiede zu V7 (sfv4/r/V7_szene) und 0 zum Lauf des Bauers.
  - "Szene Ende: Cut 4", "23 meshes, 23 bones, 21 clips".
  - Schwanz: Median Takt-Max 18,2 Grad, die Biegeebene wechselt in 43 Takten das Vorzeichen (r2: 40).
- P2: Die Gelenkfolge dreht gleichsinnig (-15,0 / -14,4 / -20,1 / -10,0 / -6,2 / -18,0 Grad). Spitze bei (273,332). Kein Haken.
- Code-Diff (nur schwanz_r41.c):
  - glaetten() dreht die Kette starr um Gelenk 1, nur wenn der Knick dort <= SW_V8_GLATT_MAX bleibt; sonst dreht sie um die Wurzel.
  - Toter Parameter v8 entfernt; Kappe u1_n kommentiert (37 x 5 > 180 stimmt); Kopfkommentar zeigt die Runde-3-Werte (541, -722/-145/108).
  - Neue Zahlen gibt es nicht. Hinzugefuegte "@0x" stehen nur im Text "keine @0x-Konstante". GRUEN.
- Tests: Die V8-r2-Pruefungen sind durch strengere ersetzt.
  - Neu sind volle Zeilengleichheit 493-495 = EM30, die Zaehlungen je Mesh und 3 UV-Schuebe.
  - Die Gegenprobe der Unit-Tests ist nur nach Konstruktion rot, nicht durch Tausch des Assets.
- Labor 50E0: Weder Bauer noch Pruefer liessen diese Runde einen Lauf. Es gibt keinen Beleg.

## Fazit
- Rot:
  - S10/O5: 46 090 sichtbare Wurzel-Pixel in F328-F346
  - O3: neuer Schlitz ueber dem Kopf in der Szene, 807 px gegen V7 3
  - O1: Oberarm 24,3 % ausserhalb des Vorlage-Bands
  - O1: Rumpf 62,2 % ueber dem Original in gleicher Pose (49,4 %)
- Gelb:
  - S1 schlechter als der zu haltende r2-Wert
  - S3a/S2c auf der Schwelle und nicht halbpixelfest
  - Flecken absolut 51 gegen 31
  - Huegel-Texel zur Haelfte aus einem Bereich, den das Original nicht nutzt
  - Szenenkontur bis 4 px (960x720) ueber V7
- Gruen:
  - S2, S4 (O4), S5, S6, O6, Herkunft O1 (Zeilen, Texel, Feld C, Brust, Unterarm), O2, S8, O8
  - Kette, Pin, Tests 8/8 und 1013/1013
  - state.log gleich V7

**bestanden = nein.**
