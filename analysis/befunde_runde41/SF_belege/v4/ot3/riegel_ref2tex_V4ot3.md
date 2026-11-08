EMD sha256 a1bc9309

| Mass | Soll | Ist | |
|---|---|---|---|
| T-K1a Koerper-Meshes = Ref2 | Meshes 0-8, 10-15: Ecken, Normalen, Flaechen, UV inkl. CLUT-Feld und TPage bytegleich | abweichend: keine | OK |
| T-K1b CLUT = Ref2 | Zeilen 480-485 bytegleich; weitere Zeilen nur als Kopie | Zeilen 480-485 gleich; TIM 6 Zeilen; weitere keine | OK |
| T-K1c Koerper-Texel = Ref2 | jeder von einer Koerperflaeche (Meshes 0-8, 10-15) abgedeckte Texel bytegleich | 0 von 34716 abgedeckten Texeln abweichend; ausserhalb der Koerperflaechen geaendert 22936 Texel (Kopf-/Schwanzflaechen) | OK |
| T-K2 Ref2-Farbe oder belegte Ableitung (Kopf, Schwanz) | jeder abgedeckte Texel von Mesh 9 und 16-22: 15-Bit-Wert in der Ref2-CLUT oder eine im Protokoll belegte Ableitung (0); Zeilen = Ref2-Zeile oder Kopie | 0 von 14008 Texeln mit Farbe ausserhalb Ref2 und Ableitungen (0 Ableitungs-Texel); 160 verschiedene Farben; Kopf: Zeile 480 (Kopie/gleich [480]) 3652 Texel, nicht in Ref2: 0; Schwanz: Zeile 480 (Kopie/gleich [480]) 10356 Texel, nicht in Ref2: 0 | OK |
| T-K3a Schwanz ohne Quernaht | groesster Sprung (dE76, 3-px-Abschnitte entlang der Kette) zwischen Gliedern <= groesster Sprung innerhalb eines Glieds | zwischen Gliedern max 13.0 (Id 20|21 bei 177 px), innerhalb max 14.0; je Gelenk 16|17 5.7, 17|18 8.9, 18|19 4.3, 19|20 10.0, 20|21 13.0, 21|22 3.5 | OK |
| T-K3b keine einfarbigen Bloecke | je Glied Std(L*) >= 0,5 x Oberarm Id 13 (10.9 -> 5.4) und >= 16 Texel-Indizes | Id 16 Std 14.6 / 107 Indizes; Id 17 Std 13.0 / 124 Indizes; Id 18 Std 11.1 / 154 Indizes; Id 19 Std 12.2 / 139 Indizes; Id 20 Std 11.5 / 136 Indizes; Id 21 Std 10.9 / 92 Indizes; Id 22 Std 8.8 / 86 Indizes | OK |
| T-K3c Wurzel ohne Kante | Sprung Id 16 <-> Rumpf Id 7/8 (je 3-px-Band an der Grenze) <= groesster Sprung innerhalb eines Glieds | Sprung 4.7 (Wurzel 28.6/15.8/8.8 / Rumpf 26.7/12.6/5.9, 269 / 307 px), Grenze 14.0 | OK |
| T-K3d Wurzel Id 16 ohne Streifen (Anisotropie je Teilflaeche) | waagerecht/senkrecht je Teilflaeche in der Spanne von Rumpf Id 8 und Oberarm Id 13 (0.67-1.44) | ganz 0.81; oben14 1.29; Qol 0.99; Qor 0.83; Qul 0.71; Qur 0.94 | OK |
| T-K3d Glied Id 17 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 1.03; oben14 0.88; Qol 1.12; Qor 0.94; Qul 1.02; Qur 1.05 | INFO |
| T-K3d Glied Id 18 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 0.97; oben14 0.90; Qol 1.29; Qor 0.85; Qul 1.01; Qur 0.67 | INFO |
| T-K3d Glied Id 19 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 1.14; oben14 1.12; Qol 1.03; Qor 1.42; Qul 1.18; Qur 1.04 | INFO |
| T-K3d Glied Id 20 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 1.46; oben14 1.31; Qol 1.86; Qor 1.13; Qul 1.44; Qur 1.77 | INFO |
| T-K3d Glied Id 21 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 1.11; oben14 1.11; Qol nan; Qor nan; Qul nan; Qur 1.28 | INFO |
| T-K3d Glied Id 22 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 1.35; oben14 1.44; Qol 0.88; Qor 2.30; Qul 2.24; Qur 1.25 | INFO |
| T-K4 K1 Augen vorn | 1 Komponente; Kontrast >= 20 (Original 29,5) | 1 Komp.; 20.2 | OK |
| T-K4 K2 Augenfleck vorn | L* >= KV5 + 20; hell und unbunt (a* <= 15, b* >= 0); Video 46,9/7,2/-1,1 nur Richtung | 54.5/4.9/1.9; KV5 25.3 | OK |
| T-K4 K3 Augenfleck Groesse | B 0,016-0,036 / H 0,023-0,043 H | 0.030 x 0.033 H (11 x 12 px) | OK |
| T-K4 K4 Augenlage vorn | (B)-links 0,035-0,065 H; 0,07-0,12 H unter OK | 0.054 H links; 0.095 H unter OK | OK |
| T-K4 K5 Haarfeld vorn (KV2) | braeunlich: L* 20.2-29.3 (0.62-0.90 x Kopf-Median 32.6, Videoraum), a* > 0, b* > 0; Video 13,3/7,4/1,5 nur Richtung | 28.0/4.6/7.2 | OK |
| T-K4 K6 zweites Auge verdeckt | Spiegelstelle in der Haarflaeche: Mittel L* <= Kopf-Median - 3 (29.6), kein heller Fleck (max L* <= Auge - 15 = 39.5) | Mittel 23.1, max 31.6 (Box (367, 183, 377, 194)) | OK |
| T-K4 K7 Nase (KV3) | Zonengrenze, mitteldunkel wie die Wange gleicher Hoehe: |L* - KV5| <= 4 (KV5 25.3; Vorlage -0,7) und L* >= Haar rechts daneben KV3r + 1 (17.7; Vorlage +4,9); Zonen-P50 Videoraum Haar 30.5 / grau 34.9 | 23.2/1.1/5.1 | OK |
| T-K4 K8 Kinn/Maulzone (KV4, Kopf-Id 9 erodiert) | kein dunkles Maul in der Textur: Albedo-L* P50 >= Ref2-Kopfhaut 37.2 - 8; b* >= 0 (Ref2 KV4 30.3/8.2/4.0 nur Ausgabe; das Kinn liegt in Pose 135 im Schatten der V4-Kopfform) | Albedo 31.9; 22.0/0.7/2.8 (dE00 zu Ref2 KV4 10.8) | OK |
| T-K4 K9 Kopf-Oberseite vorn (KV1) | dunkel rotbraun: a* >= 8, b* > 0; L* <= Kopf-Median + 8 (40.6) und >= Haar KV2; Video 22,6/15,4/5,2 nur Richtung | 37.3/14.4/6.7 | OK |
| T-K4 K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H | 0.154 x 0.187 H | OK |
| T-K4 S1 Auge seitlich | 1 heller Fleck; 0,028-0,052 x 0,040-0,070 H; hell und unbunt (a* <= 15, b* >= 0) | 53.3/4.6/1.9; 0.045 x 0.049 H; 1 Komp. | OK |
| T-K4 S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0.051 H; 0.817 H | OK |
| T-K4 S3 Haarkante seitlich (KS2) | dunkler Streifen vor dem Auge: L* <= Kopf-Median (Pose 85, Videoraum 37.0) - 5; Abstand zum Fleck <= 0,01 H; Video 19,1/1,2/1,1 nur Richtung | 23.1/0.7/1.3; Abstand 0.000 H | OK |
| T-K4 S4 Kopf-Oberseite seitlich (KS1) | dunkel rotbraun: a* >= 8, b* > 0; L* <= Kopf-Median (Pose 85) + 8 (45.0); Video 27,6/15,3/3,3 nur Richtung | 34.5/10.1/4.0 | OK |
| T-K5a Kopf-Grundton L* P50 Pose 135 | graue Zone (ausserhalb Haar/Kuppe) wie Ref2 (dort 30.8, ganzer Kopf 30.8) +- 4 | 27.9 (Haar+Kuppe 52 % der Kopfpixel; Albedo 34.8, Ref2 37.2) | OK |
| T-K5b Kopf-Grundton/Rumpf Pose 135 | graue Zone >= Ref2 - 0,2 (Ref2 dort 1.18, ganzer Kopf 1.28) | 1.10 | OK |
| T-K5c violett Pose 135 | Anteil b* < 0 <= 0,02 (Ref2 0.00) | 0.000 | OK |
| T-K5d dunkle Textur nur in Haar/Kuppe Pose 135 | Anteil Albedo-L* < 25 in der grauen Zone <= Ref2 + 0,05 (Ref2 0.00) | 0.03 (Render L* < 20 ausserhalb 0.25, Ref2 0.18; in Haar/Kuppe 0.39) | OK |
| T-K5 ganzer Kopf Pose 135 | Ref2 L* P50 30.8, Kopf/Rumpf 1.28, L* < 20 0.12 | L* P50 27.4, Kopf/Rumpf 0.99, L* < 20 0.32 (gewertet in T-K9e) | INFO |
| T-K5a Kopf-Grundton L* P50 Pose 85 | graue Zone (ausserhalb Haar/Kuppe) wie Ref2 (dort 30.8, ganzer Kopf 30.8) +- 4 | 30.6 (Haar+Kuppe 19 % der Kopfpixel; Albedo 37.9, Ref2 30.8) | OK |
| T-K5b Kopf-Grundton/Rumpf Pose 85 | graue Zone >= Ref2 - 0,2 (Ref2 dort 1.06, ganzer Kopf 1.05) | 0.98 | OK |
| T-K5c violett Pose 85 | Anteil b* < 0 <= 0,02 (Ref2 0.00) | 0.000 | OK |
| T-K5d dunkle Textur nur in Haar/Kuppe Pose 85 | Anteil Albedo-L* < 25 in der grauen Zone <= Ref2 + 0,05 (Ref2 0.00) | 0.00 (Render L* < 20 ausserhalb 0.31, Ref2 0.16; in Haar/Kuppe 0.05) | OK |
| T-K5 ganzer Kopf Pose 85 | Ref2 L* P50 30.8, Kopf/Rumpf 1.05, L* < 20 0.16 | L* P50 31.8, Kopf/Rumpf 0.99, L* < 20 0.26 (gewertet in T-K9e) | INFO |
| T-K9a graue Zone (Pose 135) | Chroma P50 <= 0,5 x Rumpf Id 8 (7.8 -> 3.9); L* P50 = Ref2-Kopf 30.8 +- 5; Lage: >= 0,80 der grauen Pixel links der Nase (x 354), >= 0,70 der Kopfpixel links der Nase grau (Rest Kuppe oben, Rand) | Chroma 3.5; L* 27.9; links 0.83; Kopf links grau 0.73; Anteil 0.44 der Kopfpixel (1195) | OK |
| T-K9b Haarflaeche braun (Pose 135) | L* P50 0.62-0.90 x Kopf-Median (27.4 -> 17.0-24.7, "um 22-24"); a* > 0, b* > 0; Flaeche Haar+Kuppe 0,40-0,55 der Kopfpixel | L* 18.4 (0.67 x Median), a* 3.3, b* 7.5; Flaeche 0.52 (Haar 0.36, Kuppe 0.16) | OK |
| T-K9b Rand und Kanten (Pose 135) | Randgradient (Sobel/8, L*) 3-8 ("um 6", Ref2-Kopf innen 2.7); Grenze je Zeile: Std zur Geraden > 4 px (keine gerade Kante) | Rand 3.2 (704 px); Kante Std 6.9 px (61 Zeilen) | OK |
| T-K9c Kuppe dunkel rotbraun (Pose 135) | L* P50 <= Kopf-Median + 6 (33.4) und >= Haar (18.4); a* >= 8, b* > 0; Anteil > 0,03 der Kopfpixel | L* 31.8, a* 17.4, b* 7.3; Anteil 0.16 | OK |
| T-K9d Auge / Nase / Seite | Augenfleck (K1-Schwerpunkt) in der grauen Zone und K4 OK; Nase: Fenster KV3 zu 0,3-0,7 grau (Grenze); S2 OK; KS2 (roh) <= Kopf-Median Pose 85 - 4 (27.8) | Auge Zone 1, K4 OK; KV3 grau 0.37; S2 OK; KS2 L* 14.9 (Zonen grau 1.00, Haar 0.00, Kuppe 0.00) | OK |
| T-K9e ganzer Kopf (Pose 135) | L* P50 >= 23; Kopf/Rumpf (Mittel L*) >= 0,9 (Ref2 30.8 / 1.28) | L* P50 27.4; Kopf/Rumpf 0.99 | OK |
| T-K9f Rundum kein schwarzes Oval | Gier +120 / +180 Grad (neutral): groesster Fleck L* < 15 im Kopf <= 100 px | +120: Fleck 2 px (dunkel 3 von 1335); +180: Fleck 0 px (dunkel 0 von 784) | OK |
| T-K8 Aenderungsumfang gegen EM30_V4ot2.EMD | Sektionen 0-7 (Geometrie, Skelett, Clips ...) bytegleich; CLUT bytegleich; Texel nur in den Kopf-UV-Flaechen | Sektionen gleich [0, 1, 2, 3, 4, 5, 6, 7]; CLUT gleich; Kopf-Texel geaendert vorn 2989 (Bbox [0, 0, 47, 63]), seite 3413 (Bbox [0, 0, 55, 63]); ausserhalb 0 | OK |
| G1 Koerper-Geometrie = Ref2 | Meshes 0-8, 10-15: Ecken, Normalen, Flaechen, UV gleich (O1: CLUT-Feld frei); Skelett/Keyframes 0-15, EDD gleich | Geometrie abweichend keine; CLUT-Feld geaendert keine; byte-gleich 15 von 15; Skelett True; Keyframes True; EDD True; uebrige Sektionen True; Ref2 sha256 cfd1ac42 | OK |
| M1 Dreiecke Schwanz | <= 212 | 198 ([33, 22, 44, 22, 22, 22, 33]) | OK |
| M2 Dreiecke Kopf | <= 118 | 109 | OK |
| M3 Farbtiefe | 8-bpp-TIM, Texel aus der CLUT | 8 bpp True; CLUT-Zeilen Kopf [480], Schwanz [480], Koerper [480, 481, 482, 484] (TIM 6 Zeilen) | OK |
| M4 Texeldichte | Kopf und Schwanz 0,045-0,091 Texel/E. | Kopf 0.057; Schwanz [0.06, 0.07, 0.065, 0.072, 0.07, 0.069, 0.065] | OK |
| T1 Schwanz Beginn | 0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03) | 0.172 H hinter, 0.742 H ueber (Id 16 an Id 7/8) | OK |
| T2 Biegung | Lage dx -0,245 dy +0,036 (+-0,03) nach t2_mass_v4 (Original -0.268 / 0.056); Drehung 68.4 +-5.0 Grad (Original 70.1/65.8/69.5) | dx -0.259 dy 0.049; 64.0 Grad (Bauer-Mittellinie: dx -0.266 dy 0.061) | OK |
| T3 Knick (Taille) | dx -0,265 dy +0,257 (+-0,03) | dx -0.239 dy 0.247 | OK |
| T4 Ende | dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle | dx -0.218 dy 0.388; 0.354 H ueber Sohle | OK |
| TA Schwanz absolut | Beginn (357,243), Knick (290,308), Ende (295,343) je Achse +-0,03 H | Beginn (358,238) +0.004/-0.019 H; Knick (295,303) +0.019/-0.018 H; Ende (300,340) +0.020/-0.010 H | OK |
| T5 Breite nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 | 0.032/0.049/0.098/0.163 H; Abfall 0.000 | ROT |
| T5q Breite quer (unabhaengig) | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 (Original 3 Masken: Abfall 0.016/0.020/0.000) | 0.032/0.047/0.110/0.161 H; Abfall 0.002; Profil f 0..1 [0.025, 0.032, 0.045, 0.047, 0.081, 0.11, 0.133, 0.161, 0.157, 0.142, 0.144] | OK |
| T6 Endstueck | B 0,025-0,05; L 0,12-0,17 H; 75-95 Grad; Taille <= 0,035 und schmalste | B 0.036; L 0.144 H; 82 Grad (vorn); Taille 0.021, schmalste davor 0.021 | OK |
| TK Schwanz Oberkontur | x 284-332: RMS zur Geraden >= 3.02 px (Original 3.87/4.71/4.69 minus Spanne); Oberkante x 276-292 je Spalte <= 0,03 H vom Original 251/250/243/236/234 | RMS 3.69 px; Oberkante 249/243/238/233/228 (max 0.027 H) (Sicht-Pruefung V4 r3 1.3: V4 r3 0,6 px, Oberkante 237/235/232/230/228; Toleranz Oberkante = TA (0,03 H)) | OK |
| T7a Gang koerperfest Lage | Gelenke 1/3/5/7 je <= 0,06 H vom Ziel (Keyframe der gezeichneten Pose), alle Gangtakte | 0 von 47 Takten > 0,06 H; max 0.056 H (h85 {'Beginn': 0.003, 'Biegung': 0.017, 'Knick': 0.036, 'Ende': 0.056}) | OK |
| T7b Biegung/Knick im Gang | Gelenk 3 >= Keyframe - 10 Grad; Gelenk 5 >= halber Keyframe-Winkel | Biegung 69.5-76.8 Grad (Keyframe 70.2-76.2), 0 Takte darunter; Knick 11.2-16.2 (Keyframe 15.6-16.5), 0 darunter | OK |
| T7c keine waagerechte Kette | Strecke Biegung -> Ende in Zielrichtung >= 0,75 x Keyframe | min 0.370 H (Keyframe 0.364 H); 0 Takte darunter [] | OK |
| T7d keine Rueckfaltung | alle Gelenkwinkel <= 90 Grad, ganzer Lauf | max 79.4 Grad; 0 von 144 Takten > 90 | OK |
| Boden Mesh | kein Glied-Mesh und keine Dornspitze unter dem Boden, ganzer Lauf | tiefster Mesh-Punkt -6 E. (h35, Dornspitze -53 E.); 0 Takte darunter (Radien je Gelenk [315, 315, 343, 343, 135, 98, 83, 83] E., Dorn 308 E. (Mesh 22)) | OK |
