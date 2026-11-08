EMD sha256 a1bc9309

| Mass | Soll | Ist | |
|---|---|---|---|
| T-K1a Koerper-Meshes = Ref2 | Meshes 0-8, 10-15: Ecken, Normalen, Flaechen, UV inkl. CLUT-Feld und TPage bytegleich | abweichend: keine | OK |
| T-K1b CLUT = Ref2 | Zeilen 480-485 bytegleich; weitere Zeilen nur als Kopie einer Ref2-Zeile oder als RE2-Kopfzeile (G2 / G3, T-R1d) | Zeilen 480-485 gleich; TIM 6 Zeilen; weitere keine | OK |
| T-K1c Koerper-Texel = Ref2 | jeder von einer Koerperflaeche (Meshes 0-8, 10-15) abgedeckte Texel bytegleich | 0 von 34716 abgedeckten Texeln abweichend; ausserhalb der Koerperflaechen geaendert 22936 Texel (Kopf-/Schwanzflaechen) | OK |
| T-K2s Schwanz Ref2-Farben | Meshes 16-22: jeder Texel 15-Bit-Wert in der Ref2-CLUT; Zeile = Ref2-Zeile oder Kopie | 0 von 10356 Texeln ausserhalb; Zeilen 480 ([480]) | OK |
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
| T-R0 Aenderungsumfang gegen EM30_V4ot2.EMD | Sektionen 0-6 bytegleich; MD1 nur Mesh 9 anders; CLUT 480-485 gleich; Texel nur in alten Kopfflaechen und G2-/G3-Orten geaendert | Sektionen gleich [0, 1, 2, 3, 4, 5, 6]; andere Meshes abweichend keine; Meshes 23 -> 23; CLUT 480-485 gleich; Texel ausserhalb 0 | OK |
| T-R1a Kopf-Geometrie = G2 Mesh 3 (0x31) | Ecken / Normalen / Flaechen wie G2 Mesh 3 (48 / 48 / 56 Tri / 14 Quad) | 61 Ecken / 61 Normalen / 73 Tri / 18 Quad | ROT |
| T-R1b Kopf-Texel = G2-TIM (u 0-125 v 0-89) bis auf Andunkeln | abweichende Texel = angedunkelte (Protokoll ?), alle mit Eintrag derselben G2-Zeile, kein Texel heller als die Quelle | 11254 von 11340 Texeln abweichend; davon Eintrag der G2-Zeile 11254; dunkler/gleich 1710; unveraendert 86 | ROT |
| T-R1c Zahn-Kachel = G3-TIM (u 2-13 v 5-40) | bytegleich | 431 von 432 Texeln abweichend | ROT |
| T-R1d CLUT-Zeilen 486 / 487 = RE2 | Zeile 486 = G2 (0x31) CLUT-Zeile 0, Zeile 487 = G3 (0x33) CLUT-Zeile 0, 15-Bit-Rohwerte bytegleich | TIM 6 Zeilen; 486 ABWEICHEND; 487 ABWEICHEND | ROT |
| T-R2 K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H (Messblatt; ot3 0,154 x 0,187) | 0.154 x 0.187 H (Box (325, 154, 380, 221), H 364) | OK |
| T-R2 K4 Augenlage vorn | (B)-links 0,035-0,065 H; 0,07-0,12 H unter OK (Marker = Kern des (W)-rechten G2-Auges, Flaechen T19/T20/T41) | kein Auge sichtbar | ROT |
| T-R2 K7 Maul-Lage vorn | Zahn-Schwerpunkt bei x 0,3-0,7 / y 0,55-1,0 der Kopfbox (unter dem Auge, Mitte; Leser B: Maul an der Unterseite) | keine Zaehne sichtbar | ROT |
| T-R2 S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | kein Auge sichtbar | ROT |
| T-R2 KS2 Haarkante seitlich | Fenster KS2 (0,84-0,96 / 0,52-0,80 der Kopfbox), nur Kopf-Bildpunkte: L* <= Kopf-Median (Pose 85, Videoraum 37.0) - 5 (die Schnauze fuellt die untere Vorderecke der Box nicht, ganzes Fenster nur Ausgabe) | Kopf L* 23.1 (100 % des Fensters; ganzes Fenster 23.1; Box (472, 212, 477, 226)) | OK |
| T-R3 Zaehne sichtbar (Pose 135) | Zahn-Texel (G3-Kachel L* > 60, Chroma < 20) >= 20 Bildpunkte im Marker-Render; helle unbunte Bildpunkte (L* > 45, Chroma < 20) im Kopf unterhalb 0,5 der Kopfbox im rohen Render >= 5 | 0 Marker-Bildpunkte; 88 helle Bildpunkte; Pose 85: 0 Marker | ROT |
| T-R4 CLUT-Zeilen | <= 16 Zeilen; Port-Texturhoehe TIM-Hoehe x Zeilen <= 4096 (render_pc.c ~2598-2606, SDL_render.c:1266-1270) | 6 Zeilen; 256 x 6 = 1536 | OK |
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
