EMD sha256 a9da924f

| Mass | Soll | Ist | |
|---|---|---|---|
| T-K1a Koerper-Meshes = Ref2 | Meshes 0-8, 10-15: Ecken, Normalen, Flaechen, UV inkl. CLUT-Feld und TPage bytegleich | abweichend: keine | OK |
| T-K1b CLUT = Ref2 | Zeilen 480-485 bytegleich; weitere Zeilen nur als Kopie | Zeilen 480-485 gleich; TIM 6 Zeilen; weitere keine | OK |
| T-K1c Koerper-Texel = Ref2 | jeder von einer Koerperflaeche (Meshes 0-8, 10-15) abgedeckte Texel bytegleich | 0 von 34716 abgedeckten Texeln abweichend; ausserhalb der Koerperflaechen geaendert 22914 Texel (Kopf-/Schwanzflaechen) | OK |
| T-K2 keine neue Farbe (Kopf, Schwanz) | jeder abgedeckte Texel von Mesh 9 und 16-22: 15-Bit-Wert in der Ref2-CLUT; Zeilen = Ref2-Zeile oder Kopie | 0 von 14008 Texeln mit Farbe ausserhalb Ref2; 165 verschiedene Farben; Kopf: Zeile 480 (Kopie/gleich [480]) 3652 Texel, nicht in Ref2: 0; Schwanz: Zeile 480 (Kopie/gleich [480]) 10356 Texel, nicht in Ref2: 0 | OK |
| T-K3a Schwanz ohne Quernaht | groesster Sprung (dE76, 3-px-Abschnitte entlang der Kette) zwischen Gliedern <= groesster Sprung innerhalb eines Glieds | zwischen Gliedern max 13.0 (Id 20|21 bei 177 px), innerhalb max 13.5; je Gelenk 16|17 6.0, 17|18 8.9, 18|19 4.3, 19|20 10.0, 20|21 13.0, 21|22 3.5 | OK |
| T-K3b keine einfarbigen Bloecke | je Glied Std(L*) >= 0,5 x Oberarm Id 13 (10.9 -> 5.4) und >= 16 Texel-Indizes | Id 16 Std 15.8 / 174 Indizes; Id 17 Std 13.1 / 139 Indizes; Id 18 Std 11.1 / 154 Indizes; Id 19 Std 12.2 / 139 Indizes; Id 20 Std 11.5 / 136 Indizes; Id 21 Std 10.9 / 92 Indizes; Id 22 Std 8.8 / 86 Indizes | OK |
| T-K3c Wurzel ohne Kante | Sprung Id 16 <-> Rumpf Id 7/8 (je 3-px-Band an der Grenze) <= groesster Sprung innerhalb eines Glieds | Sprung 5.2 (Wurzel 26.9/16.8/8.9 / Rumpf 26.7/12.6/5.9, 269 / 307 px), Grenze 13.5 | OK |
| T-K3d Wurzel Id 16 ohne Streifen (Anisotropie je Teilflaeche) | waagerecht/senkrecht je Teilflaeche in der Spanne von Rumpf Id 8 und Oberarm Id 13 (0.67-1.44) | ganz 1.04; oben14 1.70; Qol 1.31; Qor 1.17; Qul 1.07; Qur 0.74 | ROT |
| T-K3d Glied Id 17 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 1.01; oben14 0.87; Qol 1.12; Qor 0.91; Qul 1.02; Qur 1.03 | INFO |
| T-K3d Glied Id 18 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 0.97; oben14 0.90; Qol 1.29; Qor 0.85; Qul 1.01; Qur 0.67 | INFO |
| T-K3d Glied Id 19 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 1.14; oben14 1.12; Qol 1.03; Qor 1.42; Qul 1.18; Qur 1.04 | INFO |
| T-K3d Glied Id 20 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 1.46; oben14 1.31; Qol 1.86; Qor 1.13; Qul 1.44; Qur 1.77 | INFO |
| T-K3d Glied Id 21 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 1.11; oben14 1.11; Qol nan; Qor nan; Qul nan; Qur 1.28 | INFO |
| T-K3d Glied Id 22 (Anisotropie) | ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe | ganz 1.35; oben14 1.44; Qol 0.88; Qor 2.30; Qul 2.24; Qur 1.25 | INFO |
| T-K4 K1 Augen vorn | 1 Komponente; Kontrast >= 20 (Original 29,5) | 1 Komp.; 20.7 | OK |
| T-K4 K2 Augenfleck vorn | L* >= KV5 + 20; blass (a* <= 15, b* >= 0); Video-Farbziel 46,9/7,2/-1,1 ueberholt | 47.6/5.3/1.4; KV5 19.0 | OK |
| T-K4 K3 Augenfleck Groesse | B 0,016-0,036 / H 0,023-0,043 H | 0.027 x 0.036 H (10 x 13 px) | OK |
| T-K4 K4 Augenlage vorn | (B)-links 0,035-0,065 H; 0,07-0,12 H unter OK | 0.052 H links; 0.096 H unter OK | OK |
| T-K4 K5 Haarfeld vorn (KV2) | L* <= Auge - 25 und <= Ref2-Kopf L* P50 - 8 (22.8); Video-Ziel 13,3/7,4/1,5 ueberholt | 14.2/6.4/2.0 (Auge 47.6) | OK |
| T-K4 K6 zweites Auge verdeckt | L* <= 18 (Original 12,9) | L* 11.5 (Box (367, 183, 376, 195)) | OK |
| T-K4 K7 Nase (KV3) | L* zwischen Haarfeld und Auge, >= Haarfeld + 8 (eigenes Merkmal neben dem Haar); Video-Ziel ueberholt | 16.4/3.8/0.5 (Haar 14.2, Auge 47.6) | ROT |
| T-K4 K8 Kinn/Maulzone (KV4, Kopf-Id 9 erodiert) | kein dunkles Maul: Albedo-L* P50 >= Ref2-Kopfhaut 37.2 - 8; Farbe dE00 <= 10 zu Ref2 KV4 (30.3/8.2/4.0, Albedo 47.9); Video-Ziel 9,9/9,9/3,8 ueberholt | Albedo 10.5; 10.8/7.6/3.6 dE00 13.6 | ROT |
| T-K4 K9 Kopf-Oberseite rot vorn (KV1) | a* >= 11 (Messblatt); |dL*| zu Ref2 <= 8 (Ref2 41.8/18.2/8.8); Video-Ziel ueberholt | 22.7/15.9/5.1 | ROT |
| T-K4 K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H | 0.154 x 0.187 H | OK |
| T-K4 S1 Auge seitlich | 1 heller Fleck; 0,028-0,052 x 0,040-0,070 H; blass (a* <= 15, b* >= 0); Video-Farbziel ueberholt | 42.7/3.7/0.6; 0.042 x 0.053 H; 1 Komp. | OK |
| T-K4 S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0.046 H; 0.817 H | OK |
| T-K4 S3 Haarkante seitlich (KS2) | L* <= Ref2-Kopf L* P50 (Pose 85) - 8 (22.8); Abstand zum Fleck <= 0,01 H; Video-Ziel 19,1/1,2/1,1 ueberholt | 18.5/1.3/0.8; Abstand 0.000 H | OK |
| T-K4 S4 Kopf-Oberseite rot seitlich (KS1) | a* >= 11 (Messblatt); |dL*| zu Ref2 <= 8 (Ref2 34.5/20.1/8.9); Video-Ziel ueberholt | 27.7/14.8/3.2 | OK |
| T-K5a Kopf-Grundton L* P50 Pose 135 | ausserhalb des Haarfelds wie Ref2 (dort 30.8, ganzer Kopf 30.8) +- 4 | 13.5 (Haarfeld 26 % der Kopfpixel; Albedo 18.0, Ref2 37.2) | ROT |
| T-K5b Kopf-Grundton/Rumpf Pose 135 | ausserhalb des Haarfelds >= Ref2 - 0,2 (Ref2 dort 1.18, ganzer Kopf 1.28) | 0.57 | ROT |
| T-K5c violett Pose 135 | Anteil b* < 0 <= 0,02 (Ref2 0.00) | 0.257 | ROT |
| T-K5d dunkle Textur nur im Haarfeld Pose 135 | Anteil Albedo-L* < 25 ausserhalb des Haarfelds <= Ref2 + 0,05 (Ref2 0.00) | 0.86 (Render L* < 20 ausserhalb 0.81, Ref2 0.18; im Haarfeld 0.99) | ROT |
| T-K5 ganzer Kopf Pose 135 | Ref2 L* P50 30.8, Kopf/Rumpf 1.28, L* < 20 0.12 | L* P50 11.5, Kopf/Rumpf 0.52, L* < 20 0.85 (mit Haarfeld; nicht gewertet, Grundton in T-K5a/b) | INFO |
| T-K5a Kopf-Grundton L* P50 Pose 85 | ausserhalb des Haarfelds wie Ref2 (dort 30.8, ganzer Kopf 30.8) +- 4 | 20.7 (Haarfeld 8 % der Kopfpixel; Albedo 23.9, Ref2 30.8) | ROT |
| T-K5b Kopf-Grundton/Rumpf Pose 85 | ausserhalb des Haarfelds >= Ref2 - 0,2 (Ref2 dort 1.06, ganzer Kopf 1.05) | 0.66 | ROT |
| T-K5c violett Pose 85 | Anteil b* < 0 <= 0,02 (Ref2 0.00) | 0.206 | ROT |
| T-K5d dunkle Textur nur im Haarfeld Pose 85 | Anteil Albedo-L* < 25 ausserhalb des Haarfelds <= Ref2 + 0,05 (Ref2 0.00) | 0.60 (Render L* < 20 ausserhalb 0.50, Ref2 0.16; im Haarfeld 0.81) | ROT |
| T-K5 ganzer Kopf Pose 85 | Ref2 L* P50 30.8, Kopf/Rumpf 1.05, L* < 20 0.16 | L* P50 18.0, Kopf/Rumpf 0.64, L* < 20 0.52 (mit Haarfeld; nicht gewertet, Grundton in T-K5a/b) | INFO |
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
