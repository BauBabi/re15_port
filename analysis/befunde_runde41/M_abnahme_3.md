# Spur M masken36 — Abnahme 3 (unabhaengig, 2026-10-25, nach Nachbesserung 3)

Baum `.claude/worktrees/r41_masken36` (Zweig r41/masken36, merge-base master 4c667c97, Kopf 4eaf67f4; Nachbesserung 3 =
d12e7f93..4eaf67f4, 16 Commits nach der Abnahme 2 99f85a92). Pruefer: Abnahme 3. Massstab: Wortlaut AUFTRAG.md (Punkte 1/2,
Nachtrag 11, Orchestrator-Hinweis 03:21), VERTRAG.md §1.3/§2, CLAUDE.md RE-Gate, Maengel 1/2 der Abnahme 2 (`M_abnahme_2.md`).
Bericht wird fortlaufend geschrieben (Sitzungslimit). Messordner (Scratchpad):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/M41abn3/`.

## Stand
- [x] Lektuere AUFTRAG (Spurbaum bis Nachtrag 11; master-AUFTRAG bis F-E1 02:51 gelesen: kein neuer Entscheid zu Spur M; master-UEBERGABE: keiner), VERTRAG, Dossier (NB3-Abschnitte), Abnahme 2; `git log master..HEAD`, NB3-Diff `99f85a92..HEAD` gelesen
- [x] Bau: `local_build.sh configure` OK, `build` -> "ninja: no work to do", exe md5 597ba53ae9e69535721af30b418aa1ee (= Behauptung), RE15_ASSETS_PATH = Spurbaum
- [x] Statische Gegenprobe NB3 mit eigenem Leser (s.u.)
- [x] Gezielte Tests: unit 32/32, Integration 27 Riegel (26 gruen, _zelle einzeln nachgefahren gruen)
- [x] Punkt 1 an der exe (Abnahme-2-Stellen, eigene Stellen, Gehlauf natuerlicher Cut, 3D-Rueckschritt-Suche in allen 25 Cuts, 6000-C5-Riegelumbau)
- [x] Punkt 2 (NB3 beruehrt STAGE1/2 nicht; eigene exe-Stichprobe)
- [x] RE-Gate (LAB_8003d00c + 0x8003b068, FUN_8002c18c + 0x80014368, DAT_80073e94, Zellbytes, RVD)
- [x] Vertrag
- [x] Urteil: **nicht bestanden** (Punkt 1 teilweise, Punkt 2 erfuellt)

## Messmittel (eigene, Scratchpad M41abn3)
- `py/mskio.py`: eigener Leser wie die Engine - R15M-Container + Sektion wie `re15_pri_parse_section` (Gruppensumme, draw_count =
  min(Kopf & 0xFF, gebaut), hoechstens 105), Ausschluss wie `re15_masken36_filter` (Bit je Rechteck, nur i < draw_count gezeichnet),
  eigener Atlas wie `pri_publish_tim` (8 bpp, **Index 0 = durchsichtig**), Schluessel (depth+1)*65536/1023 wie
  `re15_pri_mask_camera_z`; Renderer `pc_masken_schatten_flush`: Dreieck verdeckt gdw. tri_z >= Schluessel.
- Vorher-Stand = Abnahme-2-Commit 99f85a92: `vor_nb3/MASKS` (git archive), `vor_nb3/ausschluss_vor.inc`.

## Statische Gegenprobe NB3 (eigener Leser `py/stat_nb3.py`, vor = 99f85a92, nach = 4eaf67f4, Leon Stage 3..6)
- Geaenderte Dateien: 66 unter MASKS (MSK + _PRIcc.TIM von 3040/3050/30A0/4040/4080/5020/5050/5090/6000 je Leon+Elza), **keine
  ROOM1xxx/2xxx** (STAGE1/2 unberuehrt). Leon- und Elza-MSK/-TIM je Paar bytegleich (alle 9 MSK-Paare, alle TIM-Paare der Stages 3..6).
- Gezeichnete Generator-Punkte: **956 982 -> 983 624** (+26 642), nur in ausgeschlossenen Rechtecken **273 961 -> 247 319**,
  **weggefallen 0**, Farbe an Punkten, die vorher und nachher gezeichnet werden, **0 verschieden** (Atlas-Abbildung erhalten) -
  Zahlen des Bau-Agenten bitgenau bestaetigt.
- Tiefe je Bildpunkt (vorderste gezeichnete Maske) an Punkten, die vorher und nachher gezeichnet werden: **75 550 naeher**, 20 689
  ferner. Naeher = mehr Verdeckung (gewollt hinter dem Gegenstand, Risiko VOR ihm): 30A0 C0 11 420, 5020 C7 7 564, 4040 C9 6 920,
  30A0 C1 6 523, 5090 C13 5 218, 4040 C6 4 483, 6000 C8 3 537, 3050 C9 3 472, 5090 C11 3 314, 6000 C7 3 304, 4040 C7 3 260,
  5020 C0 3 233, 30A0 C3 2 525, 6000 C9 2 237, 4040 C11 2 033, 4040 C10 1 668, 3040 C9 1 404, 3040 C0 1 261, 6000 C5 1 186 ...
  -> DAVOR-Pruefung an der exe in diesen Cuts (Abschnitt Punkt 1).
- 25 Cuts geaendert (Liste `stat_nb3.json`), Rechtecke je Cut z.B. 30A0 C1 105 -> 61, 30A0 C0 105 -> 102, 5020 C12 28 -> 49.

## Punkt 1a - Mangel 1 der Abnahme 2: Buero-Trennwand ROOM30A0/30A1 Cut 0/1 (an der exe gemessen)
Laeufer `run.sh` (RE15_DEBUG_JUMP + RE15_PLAYER_POS + RE15_FORCE_CUT, Bild 120, Messkopie md5 597ba53a; je Szene drei Laeufe:
mit Masken / RE15_NO_PRI / "leer" = mit Masken, Figur bei (-19800,19763) ausserhalb des Bilds; jeder Lauf geprueft auf
"[framedump] 320x240 -> f000120.ppm" + EXIT_AT, sonst wiederholt - zwei Laeufe mit Segmentation fault / DEVICELOST verworfen und
neu gefahren). Auswertung `py/ana3.py`: Figur = vorderstes Dreieck aus Slot 0 im LETZTEN Bild des RE15_TRILOG (je Bild zeichnet der Port Slot 0 zuerst,
dann NPC/Objekte: letztes Bild = ab dem letzten Wechsel auf Slot 0; bei reinen Slot-0-Protokollen ab dem vorletzten Auftreten der Signatur
des letzten Dreiecks, ~120 Bildenden je Lauf; geprueft gegen |B - A0|: die Figur deckt sich mit dem Unterschiedsbild bis auf Schatten und
farbgleiche Punkte. Die Vereinigung ueber alle Bilder, wie sie `ana1.letztes` der Abnahmen 1/2 bei reinen Slot-0-Protokollen liefert,
ist 15..100 % zu gross - in 4 Laeufen kam die Anfangs-Signatur nur einmal vor - und wurde verworfen); **sichtbar** = Figurpunkt mit
|A - A0| > 24 (Summe RGB; verdeckt eine Maske die Figur, ist A dort A0); **uebermalt** = |A - B| > 24; Koerper = Zelle 6
@RDT+0x0045C `96 19 e2 04 63 98 2d 37 01 ff 00 03` (selbst gelesen: x -26525..-19975, z 14125..15375, Typ 1, fest, Band 0) als
Quader mit **Hoehe 3025 - selbst am Hintergrund abgelesen** (Kanten der Zelle in 0/2500/3025/3300/3600 projiziert, die 3025-Kante
liegt auf der Oberkante des dunklen Rahmens, Bild `bilder/hoehe30A0_c0.png`; Kamera aus RDT+0x24 = exe-Log `cut.pos=(-16896,-3600,9440)`,
sd 208); "hinter" = der Sehstrahl tritt vor der Figur-z in den Quader ein.

| Szene | Platz | Cut | Figur | unter Maske / verdeckt | sichtbar | davon hinter dem Quader | sichtbar auf gezeichneter Maske | VOR dem Quader uebermalt |
|---|---|---|---|---|---|---|---|---|
| s30A0_00 (Abnahme 2: 1072 ueber dem oberen Feld) | (-21743,16263) | 0 | 1628 | 1613 / 1613 | 11 | 9 (Deckel = Haar ueber der Oberkante, y 83) | 0 | 0 |
| u30A0_00 (986) | (-23000,16300) | 0 | 1194 | 1194 / 1194 | **0** | 0 | 0 | 1 (Rand) |
| t30A0_00 (858) | (-24300,16400) | 0 | 904 | 904 / 904 | **0** | 0 | 0 | 1 (Rand) |
| h30A0_00 | (-22343,16063) | 0 | 1403 | 1403 / 1401 | 1 (Haar) | 1 | 1 | 0 |
| e30A1_00 Elza (704) | (-21743,16263) | 0 | 1161 | 1161 / 1161 | **0** | 0 | 0 | 0 |
| g30A0_00 Elza (eigene) | (-24300,16400) Gier 1024 | 0 | 610 | 610 / 610 | **0** | 0 | 0 | 0 |
| h30A0_01 (988, erzwungen) | (-19143,15863) | 1 | 1380 | 1370 / 1370 | 7 | 7 | 0 | 6 (innen 0) |
| d30A0_00 DAVOR | (-21943,13663) | 0 | 1901 | 1728 / 0 | 1799 | 0 | - | **0** |
| d30A0_01 DAVOR | (-21343,13663) | 1 | 3320 | 2866 / 0 | 3203 | 0 | - | 1 (innen 0) |
| v30A0_00 DAVOR (eigene) | (-24000,13400) Gier 2048 | 0 | 1117 | 975 / 0 | 975 | 0 | - | **0** |
| w30A0_00 DAVOR (eigene) | (-20800,13500) Gier 1024 | 0 | 2754 | 2346 / 0 | 2647 | 0 | - | **0** |
| x30A0_00 Elza DAVOR (eigene) | (-25800,13300) | 0 | 587 | 0 / 0 | 490 | 0 | - | 0 |
| v30A0_03a/b/d VOR den Spinden C3 (eigene) | (-21800,16500) / (-22100,16000) / (-20800,16300) | 3 | 3543 / 3586 / 2512 | 0 / 2400 / 0 verdeckt 0 | 2872 / 3304 / 1902 | 0 | - | 0 / 2 (innen 0) / 0 |
| v30A0_03c Elza VOR den Spinden C3 | (-21600,15850) | 3 | 1773 | 925 / 0 | 1690 | 0 | - | 0 |
An den Stellen der Abnahme 2 ist der Mangel behoben: hinter der Trennwand 0..11 sichtbare Figurpunkte (nur Haar ueber der Oberkante),
vor ihr 0 uebermalt (die 1..6 sind Randpunkte, innen 0). Gehlauf (natuerlicher Cut, `walk.sh` mit RE15_INPUT_SCRIPT, RE15_CAM_TRACE):
w2 Leon (-18900,16300) Gier 2048 U6 hinter der Trennwand nach Westen bis (-26046,16832), Elza w3 gleich - **Cut 0 durchgehend**;
w1 (-19000,12500) Gier 3072 nordwaerts an der Ostkante vorbei - Leon verlaesst das Bild rechts.

**Neuer Befund (eigene Stellen am Ostende, im Gehlauf w2 F45..F60 sichtbar):** geht die Figur hinter der Trennwand vom Ostende her
nach Westen (der Weg hinter die Trennwand fuehrt um ihr Ostende, x -19975..-16680), ist sie durch die **Ost-Stirn der Trennwand**
(Flaeche x = -19975, z 14125..15375; im Bild Spalten x 282..325, die dunkelgraue Stirnseite rechts vom Rahmenpfosten) sichtbar:
| Szene | Platz (Gier 2048) | Figur | sichtbar | sichtbar hinter dem Quader, Eintritt durch die Ost-Stirn | Bildspalten |
|---|---|---|---|---|---|
| o1_30A0 | (-20100,16300) | 142 | 125 | 62 | x 307..319, y 154..186 |
| o2_30A0 | (-20475,16300) | 671 | 370 | **353** | x 307..319, y 110..182 |
| o3_30A0 | (-20850,16300) | 1418 | 421 | **416** | x 308..319, y 88..174 |
| o4_30A0 | (-21225,16300) | 1746 | 318 | 266 (+39 Deckel) | x 307..319, y 88..126 |
| o5_30A1 Elza | (-20475,16300) | 353 | 285 | **272** | x 307..319, y 131..181 |
| a30A0_00 | (-20600,16000) Gier 1024 | 1915 | 724 | **634** (+10 Deckel) | x 307..319, y 88..190 |
Alle diese Punkte liegen AUSSERHALB jeder Maske (o4: 2 Randpunkte auf einer Maske) (`mskio`: auf gezeichneter Maske 0, auf ausgeschlossener 0, vor NB3 ebenso 0) - die
Generator-Region von 30A0 C0 endet am Rahmenpfosten bzw. wenige Spalten dahinter (gezeichnet bis x 304..317 je Zeile), die Stirnseite
dahinter hat keine Maske. Gegenprobe Geometrie (`py/flaeche.py`, Eintrittsflaeche je Strahl): 266..634 Punkte je Stelle treten durch
die Ost-Stirn in den Koerper, BEVOR sie die Figur erreichen (z.B. a30A0 (315,180): Figur-z 6204, Eintritt 5784, Weltpunkt
(-20245,-237,15812)); in den Spalten 282..325 liegt sonst keine Zelle (Zelle 9 und das Ostende von Zelle 8 projizieren nach x 339..458,
ausserhalb des Bilds). Bild `bilder/M3_30A0_ost_stirn.png` (magenta = sichtbar hinter der Ost-Stirn), Gehlauf `bilder/walk_w2_ostende.png`.
Das ist kein Rueckschritt der NB3 (vorher gleich), aber ein offenes Loch an genau dem Gegenstand des Mangels 1 auf dem natuerlichen Weg
hinter die Trennwand (Leon und Elza).

## Punkt 1b - Rueckschritt-Suche in allen 25 durch NB3 geaenderten Cuts (eigene 3D-Suche `py/scan_nb3.py`, nicht die Werkzeuge der Spur)
Figur grosszuegig (Zylinder Radius 450 = DAT_80073e94[6], Hoehe 50..1800, 37 Strahlen je Hoehe), Plaetze = `test_r37_masken36 plaetze`
(nur zur Auswahl, Stage 3/4/5 jeder 2., Stage 6 jeder 3.), eigene Kamera/SCA/Masken (`mskio`, vor = 99f85a92, nach = HEAD).
**neu verdeckt** = Schluessel nach <= vz < Schluessel vor; Gegenprobe: liegt IRGENDEINE solide Zelle (alle Baender, Quader bis 2400 ueber
dem Bandboden) zwischen Kamera und Punkt? **neu frei** = vorher verdeckt, jetzt nicht.
| Cut | Plaetze im Bild | neu verdeckt (Plaetze / Punkte) | davon ohne Zelle davor | neu frei (Punkte) | davon mit Zelle davor |
|---|---|---|---|---|---|
| 30A0 C0 / C1 / C3 | 834 / 681 / 459 | 73 / 18 788 ; 35 / 1 601 ; 28 / 2 524 | **0 / 0 / 0** | 0 / 0 / 1 | 0 |
| 3040 C0 / C9 | 586 / 1 250 | 0 / 0 ; 8 / 434 | 0 / 0 | 8 / 1 | 0 / 1 |
| 3050 C9 | 3 285 | 478 / 13 261 | **0** | 34 649 | 34 649 (s.u.) |
| 4040 C5 / C6 / C7 / C9 / C10 / C11 | 2 452 / 2 951 / 3 481 / 1 387 / 1 904 / 978 | 5 430 ; 81 088 ; 84 405 ; 5 159 ; 9 106 ; 8 967 | **0** in allen | 29 ; 0 ; 0 ; 0 ; 124 ; 66 | 29 ; - ; - ; - ; 124 ; 66 |
| 4080 C7 | 630 | 6 | 0 | 3 | 3 |
| 5020 C0 / C6 / C7 / C12 | 1 503 / 876 / 882 / 231 | 441 ; 11 ; 554 ; 0 | **0** | 0 ; 1 ; 28 ; 13 | 0 |
| 5050 C1 | 1 335 | 0 | 0 | 0 | 0 |
| 5090 C11 / C13 | 757 / 864 | 182 ; 1 284 | **0** | 33 ; 0 | 33 ; - |
| 6000 C5 / C7 / C8 / C9 | 1 772 / 1 292 / 1 508 / 2 419 | 71 834 ; 338 ; 916 ; 1 330 | 13 374 ; 0 ; 0 ; 0 | 0 | - |
- **6000 C5** (die 13 374): alle Strahlen laufen ueber den Grundriss der Zelle 12 @RDT+0x007EC (`d8 4f c8 0f 4a e1 d6 e9 01 ff 00 23`, x -7862..12578
  z -5674..-1634 Band 2) HOEHER als 2400. Selbst nachgesehen: die Senkrechte der Zellenecke (12578,-5674) faellt in Cut 5 genau auf die
  Gitterstuetze am linken Bildrand und reicht ueber den oberen Bildrand (Bild `bilder/k6000_05a_ecke.png`, Kamera C5 (8794,-9404,-7278) Ziel
  (16480,-4408,-8998), Ecke Boden (76,229), 3000 ueber dem Boden (45,148)); das Cut-2-Bild des Bau-Agenten zeigt dieselbe Ecke als Wandkante bis
  zum oberen Bildrand. Mit Zelle 12 als Wandblock bis 7000: **7 Punkte ohne Zelle davor (alle Band 0 = Gleisbett)**. Der Riegel-Umbau
  d6000_05/d6000_05t/e6001_05 (V) -> m6000_05/m6000_05t/m6001_05 (M) ist damit geometrisch richtig (m6000_05: 1 602 von 1 613 Leon-Punkten hinter dem
  Block; vor NB3 0 davon verdeckt, nach NB3 536 = Zahl des Dossiers). Beide Plaetze liegen aber in der Kamera-Wechselzone 5 -> 2 @RDT+0x0033C (x 12099..16710 z
  -4539..-3495; RVD selbst gelesen `py/rvd.py`) - im Spiel zeigt dort Cut 2, nicht Cut 5; ebenso liegt die neue DAVOR-Szene d6000_05a (12233,-9984) in
  der Wechselzone 5 -> 4 @0x00350 (x 11583..12533). Eigene natuerliche Cut-5-Stellen im Rueckweg-Band (suedlich der 5->2-Zone, Hysterese):
  | Szene | Platz Band 2 | Figur | hinter dem Wandblock (H 7000) | davon NICHT verdeckt vor NB3 -> nach | VOR dem Block verdeckt vor -> nach |
  |---|---|---|---|---|---|
  | k6000_05a | (14333,-4584) | 1941 | 1941 | 1941 -> **1041** (an der exe sichtbar 964, davon 730 auf einer Maske) | 0 -> **0** |
  | k6000_05b | (13933,-4584) Gier 1024 | 2189 | 2158 | 2158 -> **660** (sichtbar 617 / 521) | 0 -> **0** |
  | k6000_05c | (14233,-4684) Gier 2048 | 2017 | 1831 | 1831 -> **964** (sichtbar 876 / 584) | 0 -> **0** |
  | m6000_05 (Bau-Agent, Wechselzone) | (15433,-4184) | 1613 | 1602 | 1602 -> 1066 (verdeckt 536 = Dossier) | 0 -> 0 |
  | d6000_05a (Bau-Agent) | (12233,-9984) | 2500 | 0 | 0 -> 0 | 0 -> 0 (unter Masken 1547, uebermalt 0 = Dossier) |
  NB3 verbessert die Ecke (keine Uebermalung VOR dem Block), hinter der Ecke bleiben 660..1 041 Leon-Punkte unverdeckt (an der exe 617..964
  sichtbar, 521..730 davon auf einer gezeichneten Maske mit zu ferner Tiefe = Rumpf unter R37, Tiefe 176 - OFFEN 14 des Bau-Agenten); im Spiel ein
  kurzes Fenster (z ~-4900..-4539, ~10 Bilder Gehen nach Norden vor dem Wechsel auf Cut 2).
- **3050 C9** (34 649 neu frei mit Zelle davor): alle an Band-4-Plaetzen suedlich z -18050 (hinter dem fernen Gelaender Zelle 33 @0x00A38); dort ist
  im Spiel Cut 11 aktiv (RVD: Wechsel 9 -> 11 @0x00488 bei z -17725..-16991, 11 -> 12 erst bei x -10985..-10286, die Zone 12 -> 11 @0x00500 enthaelt (-9200,-22168)), im Cut-9-Gebiet
  (x > -8800, z -16991..-6000) **0** neu freie Punkte. Exe f3050_09a (-9200,-22168) erzwungen Cut 9: Beine vorher verdeckt (86), jetzt sichtbar (36
  verdeckt) - die NB3-Teile des Treppengelaenders tragen dort Tiefe 448/491 (Bild `bilder/f3050_09_zoom.png`). Keine Spielstelle, aber ein Fall,
  den das Mass "zu fern" nicht sieht (kein Modell-Koerper: OFFEN 15 des Bau-Agenten).
- **Ergebnis 1b:** keine durch NB3 neu verdeckten Figurpunkte ohne Gegenstand davor (ausser 7 im Gleisbett). "Neu frei" mit Zelle davor (Gegenprobe
  mit der Zellenform, Hoehe 2400, alle Plaetze): 4040 C5 70, C10 514, C11 236, 4080 C7 4, 5090 C11 141 Punkte, je Platz hoechstens 14 (Raender);
  an der exe f4040_10 (-14400,-11522): verdeckt 3 -> 83, neu frei 3, davon sichtbar 1; f5090_11 (2717,-25765): 0 -> 0, neu frei 0 - ohne Belang.

## Punkt 1c - die 7 neu zugeordneten Gegenstaende und die Restliste (nachgesehen)
- Zuordnungen von Hand (`tools/maske/zuordnung_r41.json`): **34 von 34 Eintraegen** - die im Beleg genannten 12 SCA-Bytes stehen am genannten
  RDT-Offset (eigener Leser `geo.sca`, SCA-Tabelle @RDT+0x20, 12 B je Zelle w,d,x,z,Typ,u0,u1,Band): 30A0 C0/C1/C3 @0x0045C, 3040 C0 @0x00870,
  5020 C0 @0x007F8 / C6+C12 @0x00870 / C7 @0x00864, 5090 C13 @0x009B0 / @0x009BC, 6000 C5 @0x007EC.
- 30A0 C3 "Spinde" = Rueckseite der Trennwand (Zelle 6, Bild `NB3_zuordnung_30A0_03.png` angesehen; Kamera C3 (-26044,-3300,18040) hinter der
  Trennwand). Eigene Stellen VOR ihr (v30A0_03a..d, Tabelle 1a): **0 uebermalt** (2 Randpunkte innen 0); 3D-Suche 30A0 C3: 0 neu verdeckte Punkte
  ohne Zelle davor.
- 4040/4080 (Raute Typ 2 jetzt mit Rautenform): Gegenprobe `py/scan_form.py` mit der FORM der Zellen (Typ 2 Raute nach LAB_8003d00c, selbst
  disassembliert, s. RE-Gate): 4040 C5/C6/C7/C9/C10/C11, 4080 C7 - 2 478 / 34 720 / 35 715 / 2 182 / 4 019 / 3 605 / 0 neu verdeckte Punkte, **0**
  davon ohne Zelle davor.
- Restliste (Bilder `M_bilder/NB3_rest/`, Texte je Cut): Kamera ueber dem Grundriss - nachgerechnet: 5050 C1 Kamera (-14078,-4020,-24718) ueber
  Zelle 6/7, C2 (-24302,-3720,-23493) ueber Zelle 4/10 (Aussage stimmt); 50B0 C1/C3 keine Zelle unter der Kamera, Roboterarm ohne Bodenkontakt
  (Bild); Gelaender 30E0 C7 (Bild angesehen): die Generator-Region ist dort nur Boden (rot, zu Recht aus), das Gelaender links hat **keine Maske**.
  Diese Gegenstaende bleiben ohne Maske; der Bau-Agent fragt den Nutzer nach Freistellungen. Ein Entscheid, dass die Restliste den Punkt schliesst,
  liegt nicht vor (master-AUFTRAG bis F-E1 2026-10-25 02:51, UEBERGABE gelesen).

## Tests (gezielt, Spurbaum, KEINE volle Suite)
1. `ctest --test-dir re15_port/build -R "unit_r41_masken36|unit_r37_masken36|unit_pri_|unit_r36_pri|unit_r37_pri1100|unit_sld_atlas|unit_r38_itemorte|unit_r39_optik|unit_r34n_e_dokumente|unit_bss|prop_bss|unit_stage6_hintergrund" -j 3 --timeout 1800 --output-on-failure`
   -> **`100% tests passed, 0 tests failed out of 32`** (unit_r37_masken36_ausschluss 391 s, unit_r41_masken36_fern 200 s, zellen3d 100 s;
   Protokoll M41abn3/ctest_unit.log).
2. `ctest ... -R "integration_r41_masken36|integration_r37_masken36|integration_pri_masken|integration_r36_pri|integration_r37_pri1100|integration_r38_itemorte|integration_r39_optik|integration_r34n_e_dokumente|integration_r35_zgirl_5090|integration_r38_affen3|integration_r35_granate|integration_r37_elzaems_elza5091|integration_r36_dachkiste|integration_r37_beretta|integration_r37_itemband" -j 2 --timeout 2400 --output-on-failure`
   (die Masken-/PRI-Riegel und die Verhaltens-Riegel der durch NB3 geaenderten Raeume 3040/3050/5090) -> `96% tests passed, 1 tests failed out of 27`:
   rot **integration_r41_masken36_zelle**, Szene d4040_09 "1185 von 1387 Figurpunkten uebermalt". Nachgesehen (Laufordner
   `build/tests/integration/r41_masken36_zelle_wd/lauf_3c0f29fe_d4040_09_a/debug.log`): der Lauf A wurde nach dem Raumwechsel ROOM1240 -> 4040 bei
   4040-Bild 120 ohne zweiten Framedump und ohne EXIT_AT beendet (180-s-Grenze unter Last); `f.ppm` war das Bild 120 des BOOT-Raums 1240 (Cut 8) -
   kein Maskenfehler. Eigener Lauf derselben Stelle (mit/ohne/leer): 1998 Figurpunkte, 0 uebermalt, 0 verdeckt. Einzeln nachgefahren:
   `ctest -R "integration_r41_masken36_zelle$" --timeout 1500` -> **`100% tests passed, 0 tests failed out of 1`** (531 s).
   Gruen u.a. integration_r41_masken36_fern (404 s), _zellen3d (519 s), _stage2 (612 s), integration_r37_masken36_stage3..6/_nb2,
   integration_pri_masken, r36_pri_schatten, r37_pri1100_*, r38_affen3 x3, r35_granate, r35_zgirl_5090, r37_elzaems_elza5091, r36_dachkiste, r37_itemband(_lift).
   Endstand: **27 von 27 gruen** (einer nach Wiederholung).
3. Schwaeche des Laeufers (gehoert zu Mangel 3): `tests/integration/test_r41_masken36_stage2.cmake` (Laeufer aller vier r41_masken36-Integrationsriegel)
   prueft je Lauf nur `EXISTS f.ppm`; `RE15_FRAMEDUMP=120:f.ppm` schreibt aber schon im Boot-Raum bei Bild 120 (vor dem Sprung @250). Wird ein Lauf nach
   dem Raumwechsel abgebrochen, vergleicht der Riegel das Boot-Bild - rot wie hier, oder bei Art V/Z (A und B bzw. A und A0 beide Boot-Bild) **leer gruen**
   (H = 0 bzw. V = 0 bei S > 50 gegen den Hintergrund des Raums).

## Punkt 2 - die 53 Nutzer-Masken 11A0/STAGE2 (Sichtpruefung)
NB3 hat keine STAGE1/2-Maske beruehrt (`git diff --name-only 99f85a92..HEAD -- re15_port/shared_assets/PSX/MASKS/`: 66 Dateien, keine ROOM1xxx/2xxx);
die Engine-Tabelle enthaelt nur Raeume 0x3040..0x6031; einzige Engine-Aenderung der NB3 ist diese Tabelle. pri/ unveraendert (`git status pri/` leer,
`git diff --stat master...HEAD -- pri/` leer); auf master seit 5924939d nur 919cc1ed (Uebersichtsbilder durch den Orchestrator), keine neuen
Nutzer-PRIs. Eigene Sichtpruefung an der exe (andere Messung als der Riegel: Schluessel-Modell + |A - B|, robustes letztes Bild):
| Szene | Raum/Cut | Platz | Figur | unter Maske | verdeckt | uebermalt ohne Schluessel |
|---|---|---|---|---|---|---|
| Kurbel Leon | 2020 C3 | (3650,-2925) Band 1 | 1423 | 179 | 179 (Beine hinter der Reling, Zelle 11 - Abnahme 1) | 0 |
| Kurbel Elza | 2021 C3 (Masken 2020) | dto. | 961 | 196 | 196 | 0 |
| VOR | 2040 C9 | (5700,-10200) Band 3 | 4333 | 3755 | **0** | 0 |
| HINTER Gelaender | 2040 C7 | (-17500,-5600) Band 3 | 5439 | 359 | 359 | 0 |
| HINTER | 11A0 C1 | (3025,15950) Band 1 | 1642 | 1584 | 1536 | 5 (Rand) |
| VOR | 11A0 C1 | (2825,18150) Band 1 | 3071 | 2089 | **0** | 0 |
Dazu gruen: `integration_r41_masken36_stage2` (13 Szenen Leon/Elza, 612 s). Stand wie Abnahme 1/2: erfuellt.

## RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE; andere Stellen als Abnahme 1/2)
1. **LAB_8003d00c** (NB3: "Raute Typ 2 = push_diag2", Formel in `tools/maske/koerper_r41.py`): Prolog `8003d00c addiu sp,sp,-64`; liest die Zelle als
   Halbworte [0]/[2]/[4]/[6] = w/d/x/z; `8003d088 addu s7,t1,v1` = x+w, `8003d08c addu s6,t0,a0` = z+d, `8003d090 srl a1,a1,1` + `8003d094 addu
   v1,v1,a1` = **cx = x + (w>>1)**, `8003d098 srl a2,a2,1` + `8003d09c addu a0,a0,a2` = **cz = z + (d>>1)**; `8003d0d8 jal 0x8003b068`; das Sprungziel
   ist quadrant_of: `8003b078 subu v0,v0,a3` (pz-cz), `8003b08c subu v1,v1,a2` (px-cx), Vorzeichenbits, `8003b0a0 srl v0,v0,30` = Quadrant
   2*(pz<cz) + (px<cx); Quadrant 0 (@0x8003d128): t0 = (cz-(z_max+r))*(px-cx)/((x_max+r)-cx), `8003d1a0 slt v0,v1,t0` -> fest, wenn
   pz-(z_max+r) < t0 - mit r = 0 genau die q0-Zeile in koerper_r41.py. ghidra1_V2.txt:145450 `LAB_8003d00c XREF[1]: FUN_8003aea0:8003af14(*)`
   (Sprungtabelle der Zellentypen). **stimmt.**
2. **FUN_8002c18c** (NB3-Szenendatei/Werkzeug: "A0 = Figur ausserhalb des Regionsvierecks = Akteur-Cull FUN_8002c18c"): `8002c18c addiu sp,sp,-96`,
   Schleife ueber den Objekt-Pool `8002c1f0 addiu v0,v0,16280` (= 0x800b3f98) mit `8002c1f8 addiu s6,s6,148` (Satz 0x94), darin `8002c2dc jal
   0x80014368` = Punkt-im-Viereck (`80014368 lh t5,4(a1) ...` vier Kreuzprodukte; RE15_FUN_CATALOG Zeile 148). Die Regions-Pruefung steht in der
   Funktion, aber fuer OBJEKTE (Katalog Zeile 213: Prop render dispatch); fuer Figuren ruft der NPC-Zeichner 0x80039ca0 dieselbe 0x80014368. Die
   Bezeichnung "Akteur-Cull FUN_8002c18c" ist ungenau (Hinweis); verhaltensneutral (Kommentar im Test), die Wirkung selbst gemessen: in allen
   eigenen "leer"-Laeufen kein Slot-0-Dreieck.
3. **DAT_80073e94[6] = 450** (NB3: Spielerradius PR = 450 in kacheln_r41.py / `test_r37_masken36 hinter`): `read 0x80073e94 8 --w 2 --signed` ->
   `[0, -1530, 0, 450, 1530, 450, 16020, -32761]` - +6 = 450 (Katalog: Treffer-Struktur des Spielers, Radius 450, Hoehe 1530). **Wert stimmt**; als
   Figur-Ausdehnung im Werkzeug verwendet (Masken sind PORT-WAHL, Original-PRI dieser Cuts ff ff ff ff - Abnahme 2). Dazu nachgelesen:
   `8002565c sra v1,v1,4` (Figur-Bucket otz>>4), `80066c70 addiu t0,zero,341` + `80066c74 0x48c8e800` (ctc2 t0,$29 = ZSF3) - **stimmt**.
- Zellbytes selbst gelesen: Zelle 6 @0x0045C (ROOM30A0), Zelle 12 @0x007EC / 13 @0x007F8 (ROOM6000) und alle 34 Zuordnungen von Hand (s. 1c) -
  bytegleich. RVD-Zonen selbst gelesen (RDT+0x28, 20 B, wie rdt_common.c parse_zones).
- Diff NB3 (`99f85a92..HEAD`, re15_port + Dossier) auf "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|geschaetzt|vorerst|ungefaehr|
  sieht richtig|platzhalter|heuristi": **0 Treffer**; **0 neue getenv** im ganzen Zweig. Engine-Aenderung der NB3 nur `gen/masken36_ausschluss_r37.inc`
  (erzeugte Tabelle, Riegel unit_r37_masken36_ausschluss rechnet sie nach - gruen).
- Fix erklaert den Befund der Abnahme 2: ja fuer Mangel 1 (obere Felder auf Rueckwand-Zellen 8/0/1 + eine Tiefe je Rechteck; nach der Zuordnung zu
  Zelle 6 und Teilrechtecken an allen Abnahme-Stellen 0..11 sichtbare Punkte, nur Haar ueber der Oberkante); nicht erfasst ist die Ost-Stirn der
  Trennwand (Region endet am Pfosten, Mangel 1 unten).

## Vertrag (VERTRAG.md §1.3/§2)
- `git diff --name-only master...HEAD`: keine Aenderung unter release/, platform/android/, pri/, synchro/, keine CMakeLists/RELEASE_NOTES;
  shared_assets nur PSX/MASKS/ (66 Dateien in NB3, Stage 3..6, Leon/Elza bytegleich). NB3 ausserhalb M_bilder/MASKS: Dossier, Engine-Tabelle,
  tests/unit/{test_r37_masken36.c, test_r41_masken36.c, gen_r41_masken36_zellen.inc, probe_r41_masken36_sicht.c (neu), probes/r41_masken36.cmake},
  tests/integration/{test_r41_masken36_stage2.cmake, test_r41_masken36_fern_szenen.cmake (neu), test_r41_masken36_zellen3d_szenen.cmake},
  tools/maske/{kacheln_r41.py, koerper_r41.py, bilder_kacheln_r41.py (neu), zuordnung_r41.json} - alles Spur-Dateien; Registrierung nur in der
  Probe-Datei der Spur. Keine Bank-9-Bits, Nachrichten-Ids, Slots, Ereignisse (M hat keine Zuteilung).
- Riegel je NB3-Aenderung vorhanden und messend: unit_r41_masken36_fern (Mass "zu fern" 40 276, Stage 3 == 0), integration_r41_masken36_fern (Art Z,
  9 Szenen), zellen3d (1025 Zeilen byte-genau, Gegenprobe >= 127), loecher (14,3 %). Linux: probe_r41_masken36_sicht.c nur stdio/stdlib/string,
  test_r37_masken36 linkt m, -P-Skript mit cmake_minimum_required, keine .exe-/Backslash-Pfade; eigenes exe-Praefix r41m5, eigener Laufordner.
- Schwaeche: der gemeinsame Laeufer prueft den Lauf nicht (Tests 3. - Mangel 3).

## Urteile je Punkt (gegen den Wortlaut)
| Punkt | Urteil | Kern |
|---|---|---|
| 1 "Masken fuer STAGE3-6 ... ueberall Loecher" -> "Masken schliessen" | **teilweise** | NB3 bitgenau bestaetigt (956 982 -> 983 624 Punkte, 0 weggefallen, Farbe 0 verschieden, Leon/Elza bytegleich); Mangel 1 der Abnahme 2 an allen ihren Stellen behoben (s/u/t/h30A0_00, e30A1_00, h30A0_01: 0..11 sichtbar, nur Haar; DAVOR 0 uebermalt; Gehlauf Cut 0); in den 25 geaenderten Cuts keine neue Uebermalung VOR einem Gegenstand (3D-Suche, 4040/4080 mit Rautenform, 6000 C5 mit dem Wandblock), 6000-C5-Riegelumbau geometrisch richtig. ABER: (a) die **Ost-Stirn der Trennwand** 30A0/30A1 C0 hat keine Maske - auf dem natuerlichen Weg hinter die Trennwand (Gehlauf w2 F45..F60) sind 266..634 Leon- und 272 Elza-Punkte durch die Stirn sichtbar; (b) weiter echte Gegenstaende ohne Maske (Gelaender 3050 C6 / 30E0 C6..C9 / 3040 C3, Pulte 5050 C1/C2, Moebel 30A0 C0, Lok-Unterteil 6000 C7, Roboterarm 50B0), 247 319 Punkte nur in ausgeschlossenen Rechtecken, hinter der Wandecke 6000 C5 im natuerlichen Cut-5-Band 660..1 041 Leon-Punkte unverdeckt (OFFEN 14); kein Entscheid, dass die Restliste den Punkt schliesst |
| 2 "neue PRIs fuer STAGE2 ... alle sauber eingebaut" (Sichtpruefung) | **erfuellt** | NB3 beruehrt keine STAGE1/2-Maske, keine neuen Nutzer-PRIs; eigene exe-Stichprobe (2020/2021 C3 Kurbel, 2040 C7/C9, 11A0 C1): VOR 0 uebermalt, HINTER verdeckt; integration_r41_masken36_stage2 gruen |

## Maengel (nummeriert, nachpruefbar)
1. **ROOM30A0/30A1 Cut 0: Ost-Stirn der Buero-Trennwand ohne Maske - Figur hinter der Trennwand durch die Stirn sichtbar.** Zelle 6 @RDT+0x0045C
   (`96 19 e2 04 63 98 2d 37 01 ff 00 03`), Stirnflaeche x = -19975, z 14125..15375, Hoehe 3025 (selbst abgelesen), im Bild Spalten x 282..325
   (dunkelgraue Stirnseite rechts vom Rahmenpfosten); gezeichnete Masken enden je Zeile bei x 304..317, es gibt dort auch keine ausgeschlossene
   Maske (Region endet). Gemessen (Bild 120, Cut 0, Gier 2048): o1 (-20100,16300) 62, o2 (-20475,16300) 353, o3 (-20850,16300) 416, o4 (-21225,16300)
   266, Elza o5 (-20475,16300) 272, a30A0_00 (-20600,16000) 634 Figurpunkte sichtbar, deren Sehstrahl VORHER durch die Stirn in den Koerper tritt;
   Gehlauf w2 (Leon (-18900,16300) Gier 2048 U6, natuerlich Cut 0) zeigt es in F45..F60 (Bilder `M41abn3/bilder/M3_30A0_ost_stirn.png`,
   `walk_w2_ostende.png`). Erwartet: Stirnseite als Teil des Gegenstands Trennwand gezeichnet (Tiefe aus Zelle 6, Teilrechtecke), danach an o1..o5 /
   a30A0_00 "sichtbar hinter dem Koerper" == 0 (ausser Haar ueber der Oberkante) und VOR der Trennwand (d30A0_00, v/w30A0_00) weiter 0 uebermalt;
   Szene(n) im Riegel integration_r41_masken36_fern (Art Z) dazu.
2. **Punkt 1 weiter nicht geschlossen (Restbestand, vom Bau-Agenten selbst ausgewiesen):** echte Gegenstaende ohne Maske - Gelaender 3050 C6,
   30E0 C6..C9, 3040 C3 (nicht in der Generator-Region), Schreibtisch/Pulte 5050 C1/C2 und Moebel vorn 30A0 C0 (Kamera ueber der Zelle), Lok-Unterteil
   6000 C7 und Roboterarm 50B0 C1/C3 (keine Zelle), Restpunkte am Gittertraeger 6000 C5; 247 319 Punkte nur in ausgeschlossenen Rechtecken; hinter der
   Wandecke 6000 C5 (Zelle 12 @0x007EC) bleiben an natuerlichen Cut-5-Stellen 660..1 041 Leon-Punkte unverdeckt (k6000_05a/b/c (14333/13933/14233,
   -4584/-4684), 521..730 davon auf einer Maske mit zu ferner Tiefe = Quelle-R37 Tiefe 176, OFFEN 14). Die Frage an den Nutzer (Freistellungen) ist
   offen; ohne Entscheid des Orchestrators/Nutzers bleibt Punkt 1 teilweise.
3. **Integrations-Laeufer ohne Laufpruefung** (`re15_port/tests/integration/test_r41_masken36_stage2.cmake`, Laeufer von integration_r41_masken36_stage2 /
   _zelle / _zellen3d / _fern): je Lauf nur die Pruefung `if(NOT EXISTS WORKDIR/f.ppm)`; `RE15_FRAMEDUMP=120:f.ppm` schreibt bereits im Boot-Raum ROOM1240
   bei Bild 120 (vor `RE15_DEBUG_JUMP=...@250`). In meinem Lauf brach ein Lauf nach dem Raumwechsel ab (Log endet bei 4040 F120 ohne zweiten
   Framedump / EXIT_AT) -> d4040_09 verglich das Boot-Bild -> falsch rot (1185 von 1387); bei Art V/Z kann derselbe Abbruch in beiden Laeufen **falsch
   gruen** ergeben (H = 0 / V = 0). Erwartet: Auswertung nur, wenn debug.log `[flow] EXIT_AT: Bild 120 in Raum <zr>` enthaelt (sonst Lauf wiederholen
   oder Fehler), Gegenprobe ein absichtlich abgebrochener Lauf.

## Hinweise (keine Maengel der NB3)
- 6000 C5: m6000_05/m6000_05t/m6001_05 und die neue DAVOR-Szene d6000_05a liegen in Kamera-Wechselzonen (5 -> 2 @0x0033C bzw. 5 -> 4 @0x00350);
  im Spiel zeigt dort Cut 2 bzw. Cut 4. Geometrisch richtig, aber kein natuerlicher Cut-5-Blick; eine natuerliche Cut-5-Szene (z.B. k6000_05b
  (13933,-4584) Band 2, Rueckweg nach Norden) waere der bessere Riegel.
- 3050 C9: die NB3-Teile des Treppengelaenders tragen fuer Stellen suedlich z -18050 (hinter dem fernen Gelaender) Tiefe 448/491; erzwungen Cut 9 sind
  dort Leons Beine jetzt sichtbar (f3050_09a verdeckt 86 -> 36). Dort ist im Spiel Cut 11 aktiv - ohne Wirkung; das Mass "zu fern" sieht den Fall
  nicht (kein Modell-Koerper, OFFEN 15).
- Zitat "Akteur-Cull FUN_8002c18c" (Szenendatei test_r41_masken36_fern_szenen.cmake, test_r37_masken36.c) bezeichnet den Objekt-Zeichner; die
  Figuren-Pruefung laeuft ueber 0x80014368 (z.B. NPC-Zeichner 0x80039ca0) - Zitat berichtigen. DAT_80073e94[6] ist der Treffer-Radius des Spielers;
  als Figur-Ausdehnung im Werkzeug als PORT-WAHL kennzeichnen.
- Orchestrator: die Uebersichtsbilder der 25 durch NB3 geaenderten Cuts (M_bilder/uebersicht, Liste NB3_zahlen.txt) nach pri/ uebernehmen (OFFEN 5);
  OFFEN 4 (platform/psx liest MASKS nicht) bleibt.

## Gates
- RE-Gate: haelt (3 Stellen selbst disassembliert + 2 nachgelesen, Zellbytes 34/34, keine Rate-Woerter, keine neuen Schalter; ein ungenaues Zitat =
  Hinweis).
- Vertrag: haelt (Pfade, keine Zuteilungen noetig); Laeufer-Schwaeche = Mangel 3.
- Tests: unit 32/32; Integration 26/27 im Gruppenlauf, der rote Riegel einzeln nachgefahren gruen (Ursache: abgebrochener Lauf, nicht die Masken).

**Ergebnis: nicht bestanden** - Punkt 1 teilweise (Maengel 1 und 2), Punkt 2 erfuellt; Mangel 3 (Laeufer) zusaetzlich.
