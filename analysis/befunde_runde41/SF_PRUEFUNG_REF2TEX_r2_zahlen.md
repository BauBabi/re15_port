# SF V4 Originaltexturen Runde 2 - Zahlen-Pruefung (2026-11-07)

Pruefling: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/EM30_V4.EMD
(sha256 415a921f436fe8e1..., 262004 Byte, = EM30_V4ot2.EMD), Zweig r41/sichel_nb2, HEAD d8c702aa5.
Eigener Code (nicht committet): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/zp2/
(emdl.py = eigener EMD/MD1/TIM-Leser, tk12.py, emr.py, geo.py, tk2b.py, tk3tex.py, aniso.py, stretch.py, m3.py, m3b.py, m4.py, m4b.py, m4d.py).
Fuer die Render-Masse (T-K3 am Bild, T-K4/T-K5) kommen nur Bild und Id-Puffer aus dem gemeinsamen Offline-Zeichner (b2/v4_gemein.zeichne,
Posen 85/135 aus posen_v4.json); alle Masse darauf sind eigener Code (renders.pkl).

## Ergebnis

Alle Zahlen des Bauers bestaetigt. Der Riegel laeuft reproduzierbar: Nachlauf riegel_ref2tex.py auf EM30_V4.EMD + V4ot2/debug.log
= zeichengleich mit SF_belege/v4/ot2/riegel_ref2tex_V4ot2.txt (1 rot von 49, 8 INFO, Exit 1). Die eine rote Zeile T5 ist seit Runde 5
unveraendert (Geometrie bytegleich, Vorgabe 4). Dazu 6 INFO-Befunde fuer die Sicht-Pruefung und den Orchestrator (Abschnitt 7).

## 1. T-K1 Koerper = Ref2 (eigener Leser)

| Mass | Soll | Ist (eigener Code) | |
|---|---|---|---|
| Sektionen 0, 1, 3-6 (EDD, Baenke) | bytegleich | bytegleich | OK |
| Sektion 2 (EMR) | Knochen 0-15 gleich | relpos nur 16-22 anders; Knochentabelle gleich; 912 Keyframes: Kopfbytes 0 Abweichungen, Winkel nur Knochen 16-22 anders | OK |
| T-K1a Meshes 0-8, 10-15 | Ecken/Normalen/Flaechen/UV inkl. CLUT/TPage bytegleich | 0 abweichende Bloecke (tv, tn, tf, tu, qv, qn, qf, qu) | OK |
| T-K1b CLUT | Zeilen 480-485 bytegleich | TIM 6 x 256, 0 Zeilen abweichend | OK |
| T-K1c Koerpertexel | bytegleich | 0 von 34431 (eigene Rasterung; Riegel 34716, andere Kantenregel); auch im 1/2/3-px-Saum um die Koerperflaechen 0 geaendert | OK |
| geaenderte Texel gesamt | nur Kopf/Schwanz | 22995, alle ausserhalb der Koerperflaechen (Riegel 22995) | OK |

Gegenprobe V4 r5 (a762bd66): 14 UV-Bloecke (Meshes 2, 3, 5, 6, 10, 13, 14), CLUT 6 -> 12 Zeilen, Zeilen 480/481/482/484 mit
256/255/255/199 Eintraegen anders, 34200 Koerpertexel anders - der Leser sieht den alten Fehler.

## 2. T-K2 keine neue Farbe

| Mass | Ist | |
|---|---|---|
| CLUT-Zeile von Kopf (Mesh 9) und Schwanz (16-22) | nur 480 (Kopf 3621, Schwanz 10356 Texel; 13977 eindeutig) | OK |
| Farben ausserhalb der Ref2-CLUT | 0 | OK |
| Farben ausserhalb der Ref2-KOERPER-Farben (strenger) | 0 | OK |
| verschiedene Farben | 161 (Riegel 161) | OK |
| Ueberlappung frei/Koerper im Atlas | 0 Texel | OK |
| Herkunft Schwanz | 100 % der Texel in der Oberarm(11-13)/Ruecken(7, 8)-Palette | OK (Vorgabe 2) |

Gegenprobe V4 r5: 3114 Texel ausserhalb der Ref2-CLUT, Zeilen 486/487.

## 3. T-K3 Schwanz

Texturraum (eigenes Mass, Lab-Mittel der 3 Randzeilen je Glied an der Naht):

| Naht | 16/17 | 17/18 | 18/19 | 19/20 | 20/21 | 21/22 |
|---|---|---|---|---|---|---|
| dE76 Runde 2 | 2,7 | 1,8 | 1,1 | 0,8 | 0,3 | 1,5 |
| dE76 Runde 1 | 12,1 | 1,8 | 1,1 | 0,8 | 0,3 | 1,5 |

Groesster Bandsprung innerhalb eines Glieds 3,5-8,5. Texel-Indizes je Glied 107/124/154/139/136/92/86 (gleich Riegel T-K3b).

Render Pose 85 (eigenes Mass: 3-px-Saeume beiderseits der Id-Grenze; innen 3-px-Baender entlang der Hauptachse je Glied):
Naehte max 8,2 (17/18), innen max 25,3 - keine Quernaht (OK). Std L* je Glied 14,6/13,0/11,1/12,2/11,5/10,9/8,8 = Riegel T-K3b.
Wurzel Id 16 <-> Rumpf 7/8: 4,7 (28,6/15,8/8,8 gegen 26,7/12,6/5,9) = Riegel T-K3c (OK).

T-K3d Anisotropie (waagerecht/senkrecht, eigenes Mass, Bild mit Licht): Id 16 ganz 0,81, oben14 1,29, Quadranten 0,99/0,83/0,71/0,94;
Spanne Rumpf Id 8 + Oberarm Id 13 0,67-1,44 - zahlengleich mit dem Riegel (OK nach Riegel-Definition). Runde 1: 1,04 / 1,70.

## 4. T-K4 / T-K5 Kopf (eigenes Mass am Render)

| Mass | Ref2 | V4 Runde 1 | V4 Runde 2 | Riegel R2 |
|---|---|---|---|---|
| Kopfbox vorn (B x H / H) | 0,126 x 0,170 | 0,154 x 0,187 | 0,154 x 0,187 | 0,154 x 0,187 OK |
| Kopf L* P50 vorn (ganz) | 30,8 | 11,5 | 23,9 | 23,9 |
| ausserhalb Haarfeld P50 vorn | - | 11,2 | 31,3 | 30,9 OK |
| ausserhalb Haarfeld / Rumpf vorn | (ganz 1,27) | 0,67 | 1,06 | 1,07 OK |
| Anteil b* < 0 vorn / seitlich | 0,000 / 0,000 | 0,43 / 0,33 | 0,000 / 0,000 | OK |
| Albedo L* < 25 ausserhalb Haar vorn | 0,00 | 0,12 | 0,00 | 0,03 OK |
| Kopf L* P50 seitlich | 30,8 | 18,0 | 33,1 | 34,2 ausserhalb Haar OK |
| Auge vorn: Komponenten / L*a*b* | - | 1 / 45,0/6,3/2,2 | 1 / 54,3/6,0/2,2 | 54,5/6,9/2,7 OK |
| Auge vorn: Lage x / unter OK (H) | - | 0,055 links / 0,103 | 0,055 links / 0,095 | 0,054 / 0,095 OK |
| Auge seitlich: Groesse | - | - | 0,042 x 0,053 H | 0,042 x 0,049 OK |
| Kopf-Oberseite vorn ; seitlich (oberste 20 % Zeilen) | 41,0/20,5/9,4 ; 31,1/24,5/11,2 | - | 37,9/26,1/8,6 ; 34,7/25,9/7,1 | a* >= 11, dL* <= 8 OK |

## 5. Form-Riegel

Geometrie aller 23 Meshes (Ecken, Normalen, Flaechen) und Sektionen 0-6 bytegleich zu V4 Runde 5 (a762bd66) und Runde 1 (a9da924f);
UV-Koordinaten/TPage gleich, nur das CLUT-Feld anders. Folge: G1, M1, M2, M4, T1-T7d, TA, T5q, TK, Boden zeichengleich mit
riegel_V4r5.txt (nur G1/M3 wegen der CLUT-Zeilen anders). Eigene Zaehlung: Schwanz 198 Dreiecke (33/22/44/22/22/22/33), Kopf 109;
TIM 8 bpp (Flags 0x9); Texeldichte Kopf 0,057, Schwanz 0,060/0,070/0,065/0,072/0,070/0,069/0,065 Texel/E. (= M4).
T5 ROT: f 0,5 = 0,098 H, Grenze 0,125 - 0,025 = 0,100 (0,002 H = 0,5 px bei H 264 zu schmal); in Runde 5 identisch rot; T5q (zweites
Breitenmass) 0,110 OK. Durch Vorgabe 4 (Geometrie wie V4 Runde 5) in dieser Runde nicht zu beheben.

## 6. Spiel-Asset, Pin, Reproduktion, Tests

- EM30_HANGAR.EMD sha256 cfd1ac42729e5636...; release/em30_hangar.sha256 gleich; kein Commit seit 84e8a4d97 auf beiden Dateien.
- Engine/Plattform/include/tests seit 66e2562d3 unveraendert; birkin_formen.py nur in Runde 1 (514b69739) geaendert;
  Version_80%/, pri/, synchro/ unveraendert.
- birkin_formen.py --variante v4 --aus scratchpad/zp2/repro -> sha256 415a921f (gleich). ref2tex_v4.py erneut gelaufen (17,8 s):
  farben_ref2tex_v4.npz bytegleich (git status leer) - deterministisch.
- Spiellauf sfv4/r/V4ot2: cd/EMD/EM30_HANGAR.EMD = 415a921f, 191 FRAMEDUMPs, debug.log "EM30 loaded: 23 meshes, 23 bones, 21 clips".
- ctest (eigener Lauf, -R unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form): 4/4 bestanden,
  66,91 s, EXIT=0 (scratchpad/zp2/ctest_pruefer.txt).

## 7. INFO-Befunde (nicht rot; fuer Sicht-Pruefung und Orchestrator)

1. T-K3d gegen die Orchestrator-Zahl: Zusatz (b) sagt "um 0,8-1,0 wie am Rumpf". Id 16 oben14 = 1,29 liegt darueber; der Riegel nimmt
   die weitere Spanne 0,67-1,44 aus Rumpf-/Oberarm-Teilflaechen. Rumpf Id 8 ganz 0,83, Oberarm Id 13 0,97. Die obersten 6 Zeilen von
   Id 16 sind noch rot: Anteil a* > 20 = 0,85 (Runde 1 0,93; Rumpf Id 8 oberste 6 Zeilen 0,41); oben14 0,38 (Runde 1 0,67).
2. K3 Augengroesse ist methodenabhaengig: im rohen Render ist der Fleck ein Plateau von 14 x 16 px = 0,038 x 0,044 H (gleich fuer
   Schwellen L* 40-52), knapp ueber der Grenze 0,036 x 0,043; nach Messblatt-Verfahren (Videoraum, halbe Schwelle) 0,030 x 0,033 OK.
3. S2 Augenhoehe seitlich: Schwerpunkt 0,817-0,822 H ueber der Sohle (Schwelle 40-52), Riegel 0,818, Grenze 0,82 - Grenzfall.
4. Machart Kopf vorn: mittlerer |dL*| zwischen Nachbarpixeln 2,43 (Ref2-Kopf 5,06), Anteil glatt (|dL*| < 0,5) 0,52 (Ref2 0,31) - der
   Kopf ist vorn halb so koernig wie der Ref2-Kopf. Seitlich 2,86 (Ref2 2,31).
5. Haarfeld vorn: fast schwarz (L* innen 7,1, Median 4,5, Std 3,6), 23-30 % der Kopfpixel, rechteckig, Kantensprung 13,8 L*
   (dunkelste Ref2-Kopfkomponente: 10 %, 14,7). Riegel T-K5d OK; ob das ein "Block/Klecks" ist, entscheidet die Sicht.
6. Kopffarben: 100 % aus der Oberarm-/Ruecken-Palette (Zeile 480), nur 22,9 % kommen auch im Ref2-Kopf (Zeile 485) vor - zulaessig
   (Ref2-Palette). Glied 16 ist im Atlas gestreckt: 28 E. je Texel quer, 14 E. laengs (Verhaeltnis 1,97; Glieder 17-21 0,76-0,89).
