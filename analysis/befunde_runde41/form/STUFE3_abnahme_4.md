# Stufe 3 Bahnsteig-Form - Unabhaengige Abnahme 4 (Nachbesserung 4 Runde 2: Wache zurueck auf die Achsenregel, Bildmass nur fuer die Szene, Riegel Kampfkamera)

Pruefer: unabhaengig (nicht der Autor, nicht die Pruefer der Abnahmen 1-3), 2026-11-07 Nacht. Gegenstand: Zweig r41/form_nb4, Delta
616c41ecd..81ec52a59 (128d52e2e Wache zurueck auf die Achsenregel / Bildmass nur Szene, 0c53ef647 Riegel Pruefung 7 Kampfkamera + Teile
warten_leon/warten_elza, 5844efc4a Dossier NB4-10/NB4-11, 81ec52a59 Leisten), Arbeitsbaum .claude/worktrees/r41_form_nb4. Belege abn4/
(blatt/ = selbst angesehene Kontaktblaetter, laeufe/<lauf>/ = Messtabellen je Bild mass_*.txt, kam.log, deck.log, env.txt, exe_sha.txt;
laeufe/<lauf>.auswertung.txt = Auswertung je Lauf; auswertung.txt = Summenzeilen aller Laeufe; rdt_bytes.txt = selbst gelesene und
dekodierte RDT-Bytes; hashes.txt; build_*.txt; ctest/; env/; werkzeug/). Alle Zahlen dieses Berichts stammen aus eigenen Laeufen mit
eigener exe-Kopie und eigenem bzw. gelesenem Werkzeug; nichts ist vom Bauer oder den Vorgaenger-Abnehmern uebernommen (deren Zahlen
werden nur zum Vergleich genannt).
Bindend: Auftrag NB4 (Szenenkamera nach der Lage von Form und Spieler aus den vorhandenen Kameras des Raums, Original-Zonenkamera wo
kein Schnitt passt, M-B mitloesen, M-D E23/E29), Pruefmass der Abnahme 2 M-A (kein Szenenbild mit mehr als 60 % Bildpunkten Luma < 24,
kein Spielerkoerper ueber einem Drittel des Bildes, Form im Bild und nicht vom Spieler verdeckt, Taumeln sichtbar), Abnahme 3 (N1
blockierend, N2, E1, M-B-Fenster), Hinweis des Orchestrators (M-B-Rest = PORT-WAHL Cut 13, nicht blockierend, solange das Fenster
nicht groesser ist als in Abnahme 2 und M-A im echten Wagenweg behoben ist; letzte Nachbesserungsrunde vor v0.8.30), NUTZER_ENTSCHEIDE.md
(E23/E29: Aussehen K4 nicht Gegenstand).

## 0. Urteil

**Urteil: BESTANDEN - kein blockierender Mangel.** Dokumentierter Rest: M-B (Cut 13 nach dem Wurf, PORT-WAHL des Orchestrators, Fenster
genau wie in Abnahme 2). Dazu sechs Hinweise (Abschnitt 5), zwei davon aus meiner eigenen dritten Lage (Spieler weicht 3-4 s zurueck),
die dem Nutzer gezeigt werden sollten.

- M-A (Abnahme 2, blockierend) ist behoben: In 13 eigenen Szenenlaeufen (Standardweg w8, 2 s zurueck, 1 s zurueck, 3 s zurueck, 4 s
  zurueck; Leon und Elza; echte Spieler-HP; Renderer des Nutzers; jedes Bild gedumpt) ist die Form in jedem Taumelbild zu sehen und
  taumelt erkennbar; in den vier beauftragten Wegen (w/v, Leon/Elza) 0 Bilder ueber 60 % fast schwarz (max 54,6 %), der Spieler
  bedeckt hoechstens 7,6 % des Bildes, die Form ist hoechstens zu 40,5 % verdeckt, genau 2 bis 3 Schnitte je Lauf ohne Flattern.
  Mit der Vorher-exe (Stand 2e3713c74) zeigen dieselben Eingaben in der 2-s-Lage 10 Bilder ueber 60 % (Leons Ruecken, Cut 12 fest) und
  in meiner 4-s-Lage 29 Bilder bis 99 % (Form direkt vor dem Auge von Cut 12).
- N1 (Abnahme 3, blockierend), N2 und E1 sind behoben: Die Kampfkamera vor der Szene ist in allen Wegen je Bild identisch mit der
  Vorher-exe (k 782/782, a 501/501, v 394/394, x4 611/611 Bilder gleich, Spiel und Kamera) - die Wache ist wieder die Achsenregel von
  2e3713c74 (Funktion bytegleich bis auf einen Kommentar, selbst gediffed). k-Weg 7 (Leon) / 1 (Elza) Bilder ueber 60 % statt 38, kein
  Cut 14 im v-Kampf (0 statt 28), e-Weg Leons Ruecken 19,9 % statt 37,9 %.
- M-B: 62 (Leon) / 53 (Elza) Bilder Cut 13 mit der Form gross (ab F1048 / F1042) = Stand 2e3713c74 und Abnahme 2, nicht groesser.
  Fuer F1049-F1125 hat der Raum keinen Schnitt im Pruefmass (Cut 12 dort: Leons Ruecken gezeichnet bis 24585 Bildpunkte = 32,0 % des
  Bildes schon vor dem Wechsel). Bleibt PORT-WAHL des Orchestrators (Original-Zonenkamera Cut 13, RVD-Anker @0x4e0), dem Nutzer zeigen.
- Alles aus Abnahme 2 Bestaetigte ist unveraendert: HP 300/150/150, Kampfstart F1169, 10 Treffer, Rueckstoss F1486 (w) / F1544 (v), Sturz
  F1749 / F1841 (Elza F1843), G5 F1850 / F1942 (F1944), START im Griff verworfen (ms=0 in 300 Bildern, Leon und Elza), Bahnsteig
  5080/5081 bildgleich mit den Zeitleisten der Abnahme 2, Raumtext msg 3 ab F1522, Spiel-Asset und Pins unveraendert. M-D (a) erledigt.
- Riegel: Satz D 13/13 gruen, Satz A 19/19 gruen (darunter die sechs Wagenweg-Teile mit Pruefung 7); kein roter Riegel, nichts nachzufahren.

| Schritt | Stand |
|---|---|
| Bau | local_build.sh configure + build gruen, ninja "no work to do", exe sha256 aa370951... = Angabe des Bauers = Kopf (Abschnitt 1) |
| Kamerawahl gegen RID/RVD/SCD 5090/5091 | selbst per xxd gelesen und dekodiert, alle Werte wie zitiert; Code gelesen und gegen 2e3713c74 gediffed (Abschnitt 2) |
| Szene M-A, Leon und Elza, 5 Lagen | 13 Laeufe + 2 Vorher-Laeufe, behoben (Abschnitt 3.1) |
| Kampfkamera N1/N2/E1, M-B | 9 Laeufe + 3 Vorher-Laeufe, behoben / Rest dokumentiert (Abschnitt 3.2) |
| Eigene dritte Lage (3 s / 4 s zurueck) | 4 Laeufe + 1 Vorher-Lauf: Szene im Pruefmass bis auf das gehaltene Schnittbild, Kampf in der Raum-Zonenkamera Cut 14 (Abschnitt 3.3, Hinweise H-A/H-B) |
| Bestaetigtes aus Abnahme 2 | unveraendert (Abschnitt 3.4) |
| M-D (a) | erledigt (Abschnitt 3.5) |
| Riegel | Satz D 13/13, Satz A 19/19 (Abschnitt 4) |

## 1. Bau, Delta, exe

- Delta 616c41ecd..81ec52a59 ausserhalb analysis/: engine/src/birkin_formen.c (bf_nahsicht_wache (b) zurueck auf die Achsenregel mit
  BF_NAH_ACHSE 4500 und k_gang_kam {Cut 12 Auge x 18526, Blick -x} / {Cut 13 Auge x 11050, Blick +x}; bf_schnitt_waehlen ohne den
  Parameter szene, nur noch Motiv Form; bf_bild_kampf_gut entfernt; kamlog ohne Spalte kp), tests/integration/test_r41_form_wagenweg.cmake
  (Pruefung 7 Kampfkamera, Teile warten_leon/warten_elza, Framedump 1150-2000 jedes Bild, Abschlusszeile mit Figur), tests/unit/probes/
  r41_form_kamera.cmake (zwei Teile registriert). git diff 0c53ef647..HEAD -- re15_port release ist leer (5844efc4a, 81ec52a59 nur
  analysis/). Spiel-Asset RE2/BAHNSTEIG_FORM.EMD/.TIM: Blobs 1fba8a34 / 4e7db099 wie 2e3713c74, sha256 16eb6ea8... / ec280802...,
  TIM 362528 Byte CRC32 0x76775364 selbst gerechnet = Pin tests/unit/form_j_posen.h (Datei im Delta nicht angefasst).
- Bau aus Git-Bash: bash re15_port/tools/local_build.sh configure -> "LOCAL-BUILD-OK (configure)" (ctest -N zeigt die sechs Wagenweg-Teile
  1150-1155); build -> Preflight Toolchain OK, "ninja: no work to do", "LOCAL-BUILD-OK (build)" (abn4/build_configure.txt, build_build.txt).
  exe re15_port/build/platform/pc/re15_pc.exe 2026-11-07 20:08:32, sha256 aa370951fe9768dd... vor und nach meinem Bau gleich
  (exe_vor_bau.txt, hashes.txt) = Angabe des Bauers; ninja -n "no work to do" = Stand des Baums; git status re15_port leer. Die exe wurde
  vor den Commits 128d52e2e (20:14) / 0c53ef647 (20:26) gebaut; 0c53ef647 aendert nur CMake-Dateien, die ctest zur Laufzeit liest bzw. die
  mein Configure registriert hat.
- Messkopien abn4e/exe/re15_pc_formabn3.exe (aa370951) und re15_pc_formabn3_vorher.exe (f4deb1a3 = re15_pc.exe des Worktrees r41_form,
  2026-11-03 06:19, nur gelesen; Stand 2e3713c74 fuer re15_port laut Abnahme 3); je Lauf eine eigene Kopie re15_pc_formabn3[_vorher]_<lauf>_<pid>.exe,
  Hash je Lauf in laeufe/<lauf>/exe_sha.txt (17 Laeufe aa370951, 4 Laeufe f4deb1a3).

## 2. Kamerawahl gegen die Raumdaten und den Code (selbst gelesen, abn4/rdt_bytes.txt)

ROOM5090.RDT (sha256 2580e8a4... = info/Re1.5/PSX) und ROOM5091.RDT (54705bea...): Kopf +0x24 RID = 0x60, +0x28 RVD = 0x260; RID
Cut 11-15 @0x1c0-0x25f und RVD @0x47c-0x55b sind in beiden Dateien bytegleich (cmp). Dekodiert (eigene Rechnung, 32 Byte je Cut:
flag u16, fov u16, Auge xyz s32, Ziel xyz s32; H = fov >> 7):

| Cut | RID @ | Auge | Ziel | H | Blick |
|---|---|---|---|---|---|
| 11 | 0x1c0 | (7626,-3256,-25360) | (-1504,-986,-21970) | 208 | -x |
| 12 | 0x1e0 | (18526,-2656,-24660) | (1496,1514,-20370) | 208 | -x |
| 13 | 0x200 | (11050,-3110,-24812) | (15974,-1894,-23306) | 231 | +x |
| 14 | 0x220 | (23870,-7450,-29130) | (22450,-1442,-23356) | 188 | +z, Auge ausserhalb des Wagens (z -29130 gegen Streifen z -28225..-18700) |
| 15 | 0x240 | (46,-1236,-24302) | (5190,-2246,-20090) | 208 | +x+z, flag 1 |

RVD (20 Byte je Zone: be, Boden, von, nach, 4 Ecken x/z s16): Anker 11 @0x47c, 12 @0x4a4, 13 @0x4e0, 15 @0x544 (x -5600..28100,
z -29700..-16975), Anker 14 @0x51c (x 17000..28050); Streifen 11->12 @0x490 x 2800..4200, 12->11 @0x4b8 x 1203..2703, 12->13 @0x4cc
x 14000..15400, 13->12 @0x4f4 x 12249..13873, 13->14 @0x508 x 20250..21750, 14->13 @0x530 x 18502..20252; Ende FFFFFFFF @0x558. SCD
sub04: Pos_set @0x12fe (32 00 b0 04 00 00 ca a4 = (1200,0,-23350)), Cut_chg 0x0F @0x130e (29 0f), Cut_chg 0x0C @0x1342 (29 0c). Alles
wie in Abnahme 3, Dossier NB4-2 und den Codekommentaren (birkin_formen.c 387-404, 420-430, 582-586, 717-722).

Code (birkin_formen.c, Stand 81ec52a59):
- bf_nahsicht_wache (Z. 668-712): eigener Diff gegen 2e3713c74 (git show 2e3713c74:..., Funktion extrahiert): einziger Unterschied die
  Kommentarzeile "(Achsenregel, Stand 2e3713c74, s. oben)". bf_kamera, bf_kamera_setzen, bf_kamera_zurueck, re15_bf_schnitt_aus_lage
  bytegleich. BF_NAH_ACHSE 4500 und k_gang_kam sind dieselben Werte wie in 2e3713c74 (dort Z. 393-396); die Augen x 18526 / 11050 stimmen
  mit RID @0x1e0 / @0x200. Keine neue Verhaltenskonstante im Delta; die Schranken BF_BM_* der Runde 1 sind unveraendert.
- bf_schnitt_waehlen (Z. 642-664) nur noch fuer die Szene (Aufrufe Z. 744 in re15_bf_rueckstoss_start, Z. 807 in bf_rs_riesenform_phase1):
  Kandidaten = bf_wagen_kameras (Ankerzonen der RVD, Cut 14 nur fuer Motiv x >= 18502), Reihenfolge Zonenkamera der Formlage, aktueller
  Schnitt, Zonenkamera der Spielerlage, groesstes gutes Formbild, sonst Zonenkamera der Formlage; Haltezeit BF_RS_KAM_HALT 30 tauscht
  (1) und (2). Nur Kameras des Raums. In den kam.log meiner 13 Szenenlaeufe sind die Kandidaten je Bild (11,12,13,15) bzw. mit Form
  x >= 18502 (11,12,13,14,15); der gewaehlte Schnitt ist in jedem Bild einer davon und in jedem Bild "gut" (kein Rueckfall-Bild in
  keinem Lauf; werkzeug/kamlog.py). Cut 11 ab Form x <= 3000 (BF_RS_TAUMEL_KAM_X) und die Sturzkamera sind unveraendert.
- Die Original-Zonenregel (re15_bf_schnitt_aus_lage: x < 2750 -> 11, < 14000 -> 12, < 20250 -> 13, sonst 14) liegt jeweils in der
  Hysterese der RVD-Streifen (2703..2800, 13873..14000, 20250..20252).

## 3. Spiel (eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT/SOFTWARE_RENDER, RE15_FRAMEDUMP jedes Bild 640x480 im Pruefbereich, RE15_SLOTDECKUNG + RE15_BF_KAMLOG als Messschienen, kein Messhaken, echte Spieler-HP)

Eingaben (abn4/env/): w/v/k/a/s/b/e bytegleich mit abn3/env (cmp; w/v/k/a/s/b = Abnahme-2-Eingaben): Raumladung 5090/5091 am
6030-Eintrittspunkt (27200,4900), Flag (3,89), Tastenskript w8, nach dem Kampfstart W2 dann w = Zielen + Feuer (MA4 ...), v = D2.0
(2 s zurueck), e = D1.0, k = W24 statt W2 (24 s warten, Lauf k5090), a/s = Debug-Sprung Wagen B (500,-9175) stehend (s mit
RE15_PAD_AT START im Griff), b = 5080/5081. Eigene dritte Lage: x3 = D3.0 (3 s zurueck), x4 = D4.0 (4 s zurueck), aus v_*.env
abgeleitet (einziger Unterschied D2.0 -> D3.0/D4.0, Laufende 2300, Framedump 1150-2250). Werkzeug abn4/werkzeug/: lauf.sh (eigen),
mass.py / blatt.py / vergleich.py / hp_weg.py / ton.py (Werkzeug der Abnahme 3, gelesen, unveraendert uebernommen), zustand.py
(Abnahme 2), kamlog.py, auswert.sh, belege.sh (eigen).

### 3.1 Rueckstoss-Szene (M-A) - auswertung.txt, laeufe/*/mass_szene.txt, Blaetter blatt/<lauf>_szene.jpg

Pruefmass ueber die Taumelbilder (Clip 13, HP 1): BP = gezeichnete Bildpunkte im 320x240-Kennungspuffer (RE15_SLOTDECKUNG),
"verdeckt" = Promille der Formflaeche hinter Naeherem, "dunkel" = Promille Bildpunkte Luma < 24 im 640x480-Framedump.

| Lauf | Szene (Taumeln + Sturz) | Form / Spieler x bei Beginn | Schnitte (Taumeln) | Form min sichtbar (BP) | Spieler max (BP) | Form max verdeckt | dunkel max | Bilder > 600 |
|---|---|---|---|---|---|---|---|---|
| w_leon | F1486-F1849 (263 Taumelbilder) | 17290 / 14998 | 13 -> 12 (F1545) -> 11 (F1679) | 1247 (F1678) | 5866 = 7,6 % (F1512) | 405 (F1667) | 533 (F1540) | 0 |
| w_elza | F1486-F1849 (263) | 17313 / 15021 | 13 -> 12 (F1545) -> 11 (F1679) | 1596 | 4147 | 238 | 524 | 0 |
| v_leon (D2.0) | F1544-F1941 (297) | 20124 / 17815 | 13 -> 15 (F1662) -> 11 (F1768) | 2796 (F1670) | 2161 | 166 | 546 (F1652) | 0 |
| v_elza (D2.0) | F1544-F1943 (299) | 20124 / 17665 | 13 -> 15 (F1663) -> 11 (F1769) | 2723 | 1626 | 113 | 538 | 0 |
| e_leon (D1.0) | F1624-F2020 (296) | 19011 / 15088 | 13 -> 12 (F1713) -> 11 (F1850) | 1162 (F1849) | 6055 = 7,9 % | 382 | 535 | 0 |
| x3_leon (D3.0) | F1665-F2049 (284) | 20124 / 23870 | [14 gehalten, 1 Bild] 13 -> 12 (F1753) -> 11 (F1880) | 1924 | 76 | 94 | 777 (F1665, Schnittbild), danach 595 (F1752) | 1 (F1665) |
| x3_elza (D3.0) | F1671-F2058 (287) | 20130 / 21191 | [14 gehalten] 13 -> 12 (F1757) -> 11 (F1888) | 1997 | 791 | 37 | 774 (F1671), danach 560 | 1 (F1671) |
| x4_leon (D4.0) | F1761-F2145 (284) | 20124 / 25041 | [14 gehalten] 13 -> 12 (F1849) -> 11 (F1976) | 1924 | 92 | 83 | 785 (F1761), danach 590 (F1848) | 1 (F1761) |
| x4_elza (D4.0) | F1761-F2145 (284) | 20124 / 25041 | [14 gehalten] 13 -> 12 (F1849) -> 11 (F1976) | 1924 | 40 | 83 | 782 (F1761), danach 590 | 1 (F1761) |

Die Zahlen fuer w/v/e decken sich mit den Angaben des Bauers (Dossier NB4-5/NB4-10) und der Abnahme 3 (1247/5866/405/533,
1596/4147/238/524, 2796/2161/166/546, 2723/1626/113/538, 1162/6055/382/535). Mechanik (kam.log, kamlog.py): w/e Beginn Cut 13 =
Zonenkamera der Formlage (x 17290 / 19011), Wechsel auf Cut 12, sobald die Form westlich von x 14000 ist (Zonenkamera der Formlage,
Cut 12 dort "gut", F1545 / F1713), Cut 11 ab x <= 3000; v: Cut 12 ist bei Form x 11724 nicht gut (Leon 711 vor dem Auge, pl_nah), Cut 13
nicht (Form 674 vor dem Auge, nah), Cut 11 nicht (Form hinter dem Auge) -> groesstes gutes Formbild = Cut 15 (F1662), dann Cut 11 ab
x <= 3000 (F1768). Kein Rueckfall-Bild in keinem Lauf.

Gegenprobe Vorher-exe (gleiche Eingaben, Spiel je Bild identisch: v_leon/vv_leon F1544-F1941 398/398, x4_leon/xv4_leon F1761-F2145
385/385 Bilder gleich, Kamera 170/398 bzw. 297/385 gleich): vv_leon Cut 12 fest F1544-F1771, 10 Bilder > 600 (max 658, F1613), Leons
Ruecken gross (blatt/vv_leon_szene.jpg); xv4_leon Cut 12 fest F1761-F1975, 29 Bilder > 600, max 990 (F1811, Form unmittelbar vor dem
Auge von Cut 12). Der Fix wirkt in den beiden Abnahme-2-Lagen und in meinen eigenen Lagen.

Gesehen (blatt/w_leon_szene, w_elza_szene, v_leon_szene, v_elza_szene, x3_leon_szene, dazu e_leon/x3_elza/x4_* per Messtabelle und
Stichprobe): In jedem Szenenbild ist die Form zu sehen und taumelt erkennbar (Clip 13 Bild fuer Bild, Vortrieb rund 80 je Bild nach
Westen, Fuesse und Kopf bewegen sich); Cut 13 zeigt sie von hinten rechts neben dem Spieler, Cut 12 den Spieler links vorn und die Form
in die Tiefe laufend, Cut 11 den Krokodilkopf frontal bis zum Sturz durch die Tuer; Cut 15 (v) die Form rechts im Bild rueckwaerts auf
die Kamera zu, Leon dort nicht im Bild. Kein Rueckenbild des Spielers, keine schwarze Folge, kein Flattern. Schnittbilder sind
bytegleich mit dem Bild davor (cmp: w_leon F1545 == F1544, F1679 == F1678, v_leon F1662 == F1661; Bildhalt beim Schnitt, wie bisher).

### 3.2 Kampf vor der Szene (Abnahme 3 N1/N2/E1, M-B) - vorher = Vorher-exe f4deb1a3 (Stand 2e3713c74), nachher = Kopf-exe, gleiche Eingabe

| Lauf | Fenster | nachher: Schnitte / Bilder > 600 (max) | vorher (eigener Lauf) | Abnahme 3 Runde 1 |
|---|---|---|---|---|
| k_leon (w8 + 24 s warten) | F1150-F1931 | 11 Auftritt / 12 / 13 (1) / 12 (7) / **13 F1332-F1896 durchgehend** / 12 F1897-F1899 (3) / 13: **7** (618, F1889) | kv_leon: 7 (618), Spiel und Kamera 782/782 gleich | 38 (704) |
| k_elza | F1150-F1931 | wie Leon: **1** (615, F1889) | - | 38 |
| v_leon (2 s zurueck), Kampf | F1150-F1543 | 13 F1231-F1543 durchgehend, **kein Cut 14**: **0** (589 im Auftritt Cut 11) | vv_leon: 0, 394/394 gleich | 28 (Cut 14) |
| v_elza | F1150-F1543 | **0** (590) | - | 28 |
| e_leon (1 s zurueck), Kampf | F1150-F1623 | 13 / 12 F1313-F1339 / 13: **0** (589); Leon max 15319 BP = 19,9 % (F1339) | (Abnahme 3: 0) | 6, Leon 37,9 % |
| w_leon / w_elza, Kampf | F1150-F1485 | 0 / 0 (589 / 590) | - | 0 / 0 |
| a_leon (Wagen B, steht; M-B) | F700-F1200 | 12 / 13 F731-F844 / 12 F845-F1047 / **13 ab F1048: 62** (710, F1062); in Cut 12 Leon max 24585 BP = 32,0 % (F1037) | av_leon: 62, 501/501 gleich | 92 (ab F1018) |
| a_elza | F700-F1200 | 13 ab F1042: **53** (706) | - | 83 |

Die sieben dunklen Bilder des k-Wegs (601-618 Promille, F1889-F1890 und F1923-F1927, Form gross vor Cut 13 kurz vor Leons Tod) und
die Kurzschnitte F1324 / F1325-F1331 / F1897-F1899 sind der vorbestehende Stand (bildgleich mit der Vorher-exe). Gesehen
(blatt/k_leon_kampf.jpg, k_elza_kampf.jpg): Cut 13 durchgehend, Leon in der Mitte sichtbar, die Form rechts von hinten; keine schwarze
Formmasse, keine 4- oder 7-Bilder-Schnitte wie in Abnahme 3. blatt/v_leon_kampf.jpg: Cut 13 bleibt, kein Aussenblick. N1, N2, E1 behoben.

M-B (blatt/a_leon_kampf.jpg, a_elza_kampf.jpg): Cut 12 F845-F1047 zeigt Leons Ruecken rechts bis 32,0 % des Bildes mit der Form
daneben; ab F1048 Cut 13 mit der Form gross vor Leon (60-71 % fast schwarz, Leon bis 960 Promille verdeckt) bis F1125. Fenster und
Zahl (62 / 53) sind der Stand 2e3713c74 (av_leon 501/501 gleich) und damit nicht groesser als in Abnahme 2 (dort F1051-F1174 gezaehlt,
gleiche exe-Logik). Fuer F1049-F1125 bietet der Raum keinen Schnitt im Pruefmass: Cut 12 hat dort Leons Ruecken schon vor dem Wechsel
bei 32 % (ein Drittel = 333 Promille), Cut 14 zeigt die Lage hinter dem Hintergrund (Abnahme 3), Cut 11/15 haben Leon hinter dem Auge
bzw. hinter der Form. Nach Auftrag und Hinweis des Orchestrators gilt die Original-Zonenkamera Cut 13 (RVD-Anker @0x4e0, Zonenregel
x 14000..20250) als PORT-WAHL - kein Nutzerentscheid, dem Nutzer zeigen.

### 3.3 Eigene dritte Lage: Spieler weicht 3 s (x3) bzw. 4 s (x4) zurueck, Leon und Elza

Der Spieler steht dann bei x 23870 / 21191 (x3 Leon / Elza) bzw. 25041 (x4), also im Ostteil von Wagen C hinter der Zone 13->14
(x >= 20250, RVD @0x508); die Form kommt in allen 21 Laeufen nie weiter nach Osten als x 20141 (bei Szenenbeginn 20124 / 20130; in x4
steht sie F1510-F1760 im Gang-Clip 5 bei konstantem x 20124, waehrend der Spieler bei 25041 steht - die Wurzel kommt an der Kollision
des Ostteils nicht weiter). Damit beginnt die Szene nie mit der Zonenkamera 14 der Formlage.

- Szene: Im ersten Taumelbild (F1665 / F1671 / F1761) waehlt die Szene Cut 13 (Zonenkamera der Formlage, "gut"; Cut 14 ist Kandidat
  und ebenfalls gut, kommt aber erst an zweiter Stelle), das gezeigte Bild ist aber noch das gehaltene Kampfbild der Zonenkamera
  Cut 14 (cmp: bytegleich mit dem Bild davor) mit 774-785 Promille fast schwarz: **1 Bild je Lauf ueber dem Pruefmass** (Hinweis H-A).
  Danach Cut 13 (Form 20124 -> 13164, von hinten, Spieler weit dahinter kaum im Bild), Cut 12 (Spieler hinter dem Auge, nicht im Bild;
  Form in die Tiefe), Cut 11; Form min 1924 BP sichtbar, max 94 Promille verdeckt, dunkel max 595 (x3_leon F1752, Cut 13 kurz vor dem
  Wechsel) / 590 (x4 F1848) / 560 (x3_elza) - innerhalb der Schranke, aber knapp (Hinweis H-C). Vorher-exe (xv4_leon): Cut 12 fest,
  29 Bilder ueber 600, max 990. Der Fix traegt also auch hier.
- Kampf vor der Szene: ab dem Ueberschreiten von x 20250 (F1341 / F1320) zeigt der Raum seine Zonenkamera Cut 14 (Aussenblick durch die
  Wagenwand, Auge (23870,-7450,-29130)) bis zur Szene: x3 323 / 329 Bilder, x4 440 / 440 Bilder mit 74-79 % fast schwarz, Form (4275-4386
  BP min sichtbar) und Spieler klein im Dunkeln (blatt/x4_leon_kampf.jpg). Das ist die Original-Zonenregel des Raums (RVD 13->14 @0x508
  x 20250..21750, zurueck 14->13 @0x530 erst bei x <= 20252), nicht Teil dieses Deltas und bildgleich mit der Vorher-exe (x4_leon/xv4_leon
  F1150-F1760 611/611 Bilder Spiel und Kamera gleich); die Achsenregel-Wache betrifft nur Cut 12/13 (Hinweis H-B).
- Spiel: x3_leon HP 100 -> 50 (Griff F1397-F1415), x3_elza 100 -> 40, x4 100 -> 30 (Spieler laesst sich am Ostende greifen) -
  Spielerverhalten, kein Mangel. Rueckstoss F1665 / F1671 / F1761, Sturz F1949 / F1958 / F2045, G5 F2050 / F2059 / F2146.

### 3.4 Bestaetigtes aus Abnahme 2 (auswertung.txt, laeufe/*.auswertung.txt, laeufe/*.zeitleiste.txt)

| Punkt | Soll (Abnahme 2) | Ist (eigene Laeufe, Leon und Elza) | Bewertung |
|---|---|---|---|
| HP Wagen A/B/C, echter Weg | 300 / 150 (F498) / 150 (F946) | 15 Wagenweg-Laeufe (w/v/e/k/x3/x4, Kopf- und Vorher-exe): 300 (F251) / 150 (F498) / 150 (F946) | unveraendert |
| Kampf, Rueckstoss, Sturz, G5 | Kampfstart F1169, 10 Treffer, Rueckstoss F1486 (w) / F1544 (v), Sturz F1749 / F1841 (Elza F1843), G5 F1850 / F1942 (F1944) | identisch; e F1624 / F1920 / F2021; Treffer ab F1237 (w), F1297 (v), F1267 (e) | unveraendert |
| Taumeln statt Gleiten, Sturz im Wagen | Clip 13 mit Vortrieb, Clip 12 ab x <= -2500 | Clip 13 x 17290 -> -2538 in 263 Bildern (w), Sturz Clip 12 bei x -2538 / -2521 (Leon / Elza), Cut 11 zeigt das Kippen durch die Tuer | unveraendert |
| Ton (Raum-Ton 10) | gespielt, einzelne vom SE-Tor verworfen (M-C) | w 4/1 (Leon und Elza), v 5/0, e 5/2, k 6/0, a 6/0, b 6/1, s 6/0, x3 7/0 bzw. 6/1, x4 7/0; SILENT nur se=11 (Gang-Clipende, wie Abnahme 2). Zaehlung schwankt bei bildgleichem Spiel (v_leon 5/0 gegen vv_leon 4/1, Abnahme 3 w 3/2 gegen hier 4/1) - Hinweis H-D | unveraendert, Hinweis |
| START im Griff | verworfen, keine Vormerkung | s_leon (RE15_PAD_AT 726/735/745/755) und s_elza (719/728/738/748): ms=0 in allen 300 Bildern F700-F999, gr=1 in 34 Bildern | unveraendert |
| Bahnsteig 5080/5081 | Fall, Auftritt, Kampf, Griff, Flucht, Raumtext msg 3 F1522 | Zeitleisten (zustand.py, 50 Zeilen) bildgleich mit abn2/zeitleisten b5080_leon / b5081_elza; msg id=3 ab F1522 (Leon und Elza) | unveraendert |
| Spiel-Asset, Pins | K4/NB2 | Blobs wie 2e3713c74 (hashes.txt), TIM-CRC32 selbst gerechnet = Pin, form_j_posen.h im Delta nicht angefasst | unveraendert |
| Kein Absturz, kein Haenger | - | 21 Laeufe, alle exit 0 per RE15_EXIT_AT (55-134 s), keine kaputten Texturen oder Loecher auf den Blaettern | in Ordnung |

### 3.5 M-D (a)

NUTZER_ENTSCHEIDE.md enthaelt E23 (2026-11-03, "passt erst einmal", Pose von Bild 2 nicht drehen: "nein") und E29 (2026-11-07 ~10:15,
Vorschlag "K4 bleibt fuer v0.8.30 im Spiel, der G4-Ansatz danach", Wortlaut des Nutzers "Das koennte schon passen mit der riesen Form
..." vollstaendig, Folgen: kein Rueckbau, Farbvarianten getrennt) - Commit 7ebda1d64, selbst gelesen. Erledigt. M-C und M-D (b)/(c) waren
nicht beauftragt und sind unveraendert offen.

## 4. Riegel (ctest aus Git-Bash, PATH msys64 zuerst, SDL_ASSERT=always_ignore, --test-dir r41_form_nb4/re15_port/build; Logs abn4/ctest/)

- Satz D -R "^unit_(birkin|r41_form|bb_|g5|n2_griff|r41_birkin)" -j 4: **100% tests passed, 0 tests failed out of 13** (276,7 s;
  unit_r41_birkin_wand_griff 276,6 s unter der Last der parallelen Agenten; unit_birkin_szenen mit test_schnitt_aus_lage, unit_birkin_formen,
  unit_birkin_bahnsteig, unit_r41_form_emd, unit_n2_griff_start, G5, Wand, Opfer, AI). Log ctest/satzD.txt.
- Satz A -R "form|bahnsteig|birkin_form|r41_form|integration_r41_birkin_pistole|integration_r41_g5_tod|integration_r41_g5_westen" -j 4:
  **100% tests passed, 0 tests failed out of 19** (384,4 s): integration_r41_form_wagenweg (103,8 s), _elza (102,8 s), _zurueck_leon
  (97,2 s), _zurueck_elza (107,0 s), _warten_leon (98,8 s), _warten_elza (98,4 s), integration_r41_birkin_pistole (244,5 s),
  integration_r41_g5_tod (265,3 s), integration_r41_g5_westen (285,9 s), integration_r35_werfer_form, integration_r34_plattform_takt /
  _esp_eintritt, unit_birkin_bahnsteig, unit_birkin_formen, unit_r41_form_emd, unit_r39_funk10f1_form, unit_r34_plattform(_ton),
  unit_r35_fenster_satzform. Log ctest/satzA.txt. Statuszeilen der Wagenweg-Teile (re15_port/build/Testing/Temporary/LastTest.log):
  Szene leon 13;12;11 Form min 1247, Leon max 5866, verdeckt max 405, dunkel max 540; elza 1596/4147/238/530; zurueck_leon 13;15;11
  2796/2161/166/550; zurueck_elza 2723/1626/113/544 - gleich meinen eigenen Laeufen (3.1; dunkel im Riegel bei 320x240 um 7-10 Promille
  hoeher). Kampfkamera F1150 bis vor die Szene: leon/elza/zurueck_* 0 Bilder > 600 (max 596, F1187), warten_leon **9 > 600 (erlaubt 9)**,
  max 622 (F1889), warten_elza 1 (max 619). Kein roter Riegel, nichts nachzufahren.
- Pruefung 7 misst bei 320x240 zwei Grenzbilder mehr als meine 640x480-Messung (9 gegen 7 im k-Weg; Bilder zwischen 601 und 622
  Promille): dieselben Bilder F1889-F1890/F1923-F1927, dazu zwei an der Grenze. Die Schranke 9 fuer warten_leon ist der gemessene
  vorbestehende Stand ohne Abstand (Hinweis H-F).
- render_pc.c ist seit Abnahme 3 unveraendert (Delta 616c41ecd..HEAD ohne platform/); deren Satz C (64 Render-/Masken-Tests gruen) gilt
  fuer diesen Stand weiter, nicht wiederholt.

## 5. Maengel und Hinweise

| Nr | Was | Beleg | Schwere | Vorschlag |
|---|---|---|---|---|
| M-B (Rest) | Cut 13 nach dem Wurf (Debug-Sprung Wagen B, stehender Spieler): a_leon 62 Bilder F1049-F1125 (Leon bis 960 Promille verdeckt, bis 71 % fast schwarz), a_elza 53 ab F1043. Fenster = Stand 2e3713c74 = Abnahme 2, nicht groesser; der Raum hat dort keinen Schnitt im Pruefmass (Cut 12: Leons Ruecken 32 % schon vor dem Wechsel). | auswertung.txt a_leon/a_elza/av_leon, laeufe/a_leon/mass_700_1200.txt, blatt/a_leon_kampf.jpg, a_elza_kampf.jpg | dokumentierter Rest (PORT-WAHL des Orchestrators: Original-Zonenkamera Cut 13, RVD-Anker @0x4e0), nicht blockierend | dem Nutzer zeigen (Cut 12 mit grossem Spielerruecken oder Cut 13 mit grosser Form) |
| H-A | Eigene Lage 3-4 s zurueck: das erste Taumelbild zeigt noch das gehaltene Kampfbild der Zonenkamera Cut 14 (bytegleich mit dem Bild davor), 774-785 Promille fast schwarz, 1 Bild (33 ms) je Lauf; die Szenenwahl selbst steht ab diesem Bild auf Cut 13. Der Bildhalt beim Schnitt ist der bisherige Mechanismus (w F1545 == F1544, F1679 == F1678). Ein Riegelteil fuer diese Lage waere in Pruefung 6d um dieses eine Bild rot. | laeufe/x3_leon, x3_elza, x4_leon, x4_elza (mass_szene.txt, kam.log), auswertung.txt Abschnitt Szene, blatt/x3_leon_szene.jpg (Kachel F1665) | niedrig (nicht blockierend: 1 Bild, Schnittmechanik, Kamerawahl richtig) | falls ein Riegel diese Lage bekommt: Schnittbild (bytegleich mit dem Vorbild) in 6d ausnehmen oder die Szene beim Start ein Bild warten lassen; sonst Hinweis an den Nutzer |
| H-B | Weicht der Spieler mehr als rund 2,5 s zurueck (x >= 20250), zeigt der Raum fuer den Rest des Kampfs seine Zonenkamera Cut 14 (Aussenblick, Auge z -29130 ausserhalb des Wagens): x3 323/329, x4 440/440 Bilder 74-79 % fast schwarz, Figuren klein; zurueck nach Cut 13 erst bei x <= 20252 (RVD @0x530). Original-Zonenregel des Raums (RVD 13->14 @0x508), bildgleich mit der Vorher-exe (611/611), nicht Teil des Deltas; der Riegel-Kopf nennt denselben Cut 14 als Zonenkamera der Tuer beim Eintritt. | laeufe/x3_*, x4_*, xv4_leon (mass_1150_*.txt), blatt/x4_leon_kampf.jpg, x4_elza_kampf.jpg, rdt_bytes.txt | Hinweis (vorbestehend, Raumregel) | dem Nutzer zeigen; falls stoerend eine eigene Spur (Wache fuer Cut 14 im Kampf: Form im Wagen und Spieler in Zone 14 -> Cut 13), nicht in dieser Runde |
| H-C | Cut 13 kurz vor dem Wechsel auf Cut 12 (Form 2100-2600 vor dem Auge): x3_leon 581-595 Promille (F1749-F1752), x4 590, v 546, e 535 - innerhalb der Schranke 600, in der 3-s-Lage nur 5 Promille Abstand; der Software-Renderer dunkelt laut Riegel-Kopf um 40-50 Promille ab. | laeufe/x3_leon/mass_szene.txt, auswertung.txt | Hinweis | Riegel bleiben beim Renderer des Nutzers; Lage dem Nutzer zeigen (Abnahme 3 H2 verschaerft) |
| H-D | Zaehlung Raum-Ton 10 gespielt/verworfen schwankt bei bildgleichem Spiel zwischen Laeufen (v_leon 5/0, vv_leon 4/1; w hier 4/1, Abnahme 3 3/2): das SE-Tor FUN_80045a18 entscheidet nach belegten Stimmen, die mit dem Dummy-Audiotreiber nicht bildfest enden. | auswertung.txt (Raum-Ton-Zeilen), laeufe/*/re2se.log | Hinweis (M-C, nicht Teil des Deltas) | mit M-C |
| H-E | Form-Ostgrenze: in 21 Laeufen Form x max 20141, Wurzel bleibt im Gang-Clip bei x 20124 stehen (x4 F1510-F1760) - deshalb ist die Zonenkamera 14 nie erste Wahl der Szene; die Grenze ist Kollision, nicht Code der Szene (gemessen, nicht hergeleitet). | auswertung.txt (Form x max), laeufe/x4_leon/mass_1150_1760.txt | Hinweis (Messbeleg fuer die Robustheit der Szenenwahl) | - |
| H-F | Riegel Pruefung 7, Teil warten_leon: 9 Bilder > 600 gemessen bei erlaubten 9 (kein Abstand; warten_elza 1 von 9). Das Spiel ist bildfest, aber ein anderer Rasterer (Linux-Lauf, Memory reai-v2-pixelgrenze-rasterer) oder Renderer kann ein Grenzbild (601-622 Promille) kippen, ohne dass sich das Spiel aendert. | ctest/satzA.txt, LastTest.log Statuszeile warten_leon, test_r41_form_wagenweg.cmake Z. 103-111 | Hinweis (Riegel-Robustheit, kein Spielfehler) | Schranke mit Abstand (z. B. 12) oder Grenzbilder 601-625 nicht zaehlen; vor dem Linux-Lauf der Suite pruefen |
| H1-H3 (Abnahme 3) | Cut 15 Form am rechten Rand (v), Cut 13 bis 55 % vor dem Wechsel (v/e; jetzt H-C), Form am Ende von Cut 12 klein (1162-1247 BP, bis 40 % hinter dem Arm) | 3.1 | Hinweis, unveraendert | dem Nutzer zeigen |

Nicht als Mangel gewertet: Aussehen K4 (E23/E29); Cut 14 als Original-Zonenkamera fuer x >= 20250 (H-B); se=11 stumm am Gang-Clipende
(Bank 24 Id 11 = FFFFFFFF, wie Abnahme 2); die Kurzschnitte F1324/F1325-F1331/F1897-F1899 und die 7 dunklen Bilder im k-Weg
(vorbestehend, bildgleich mit der Vorher-exe); Spielerverluste am Ostende (x3/x4). Nicht nachgemessen: u-Weg (3 s vor) der Abnahme 3
mit Hinweis H4 (Effektblock, vorbestehend) - nicht Teil des Deltas, nicht beauftragt. Offen und nicht beauftragt: M-C, M-D (b)/(c).

## 6. Messweg und Umgebung

- Werkzeug abn4/werkzeug/: lauf.sh (eigene exe-Kopie je Lauf aus abn4e/exe im Scratchpad, Laufverzeichnis im Scratchpad,
  RE15_RE2_ASSET_ROOT = private Kopie der HEAD-Blobs BAHNSTEIG_FORM (hashes.txt), RE15_FRAMEDUMP-Serie jedes Bild (FD_SERIE aus der
  env-Datei), RE15_EXIT_AT, Notbremse timeout nur auf den eigenen Kindprozess, nie taskkill /IM; 4. Argument "vorher" nimmt die
  Vorher-exe), auswert.sh (mass.py --szene, mass.py Kampffenster ab F1150, hp_weg.py, ton.py, blatt.py), kamlog.py (Mechanik der
  Szenenwahl aus kam.log), belege.sh (Belege ins Repo, Blaetter als JPG wegen der *.png-Regel der form/.gitignore), vergleich.py
  (Spiel je Bild zweier Laeufe), zustand.py (Zeitleisten gegen abn2/zeitleisten). Eingaben abn4/env/*.env.
- 21 Laeufe, alle exit 0 per RE15_EXIT_AT: 17 mit der Kopf-exe aa370951 (w/v/x3/x4/k/a/s/b je Leon und Elza, e Leon), 4 mit der
  Vorher-exe f4deb1a3 (kv_leon, av_leon, vv_leon, xv4_leon). Framedumps 640x480 (Fenster, RE15_WINDOW_SCALE=2), deck.log 320x240,
  SDL_AUDIODRIVER=dummy. Laeufe parallel zu je 4-8 neben den Laeufen anderer Agenten; Spiel je Bild deterministisch (alle Kopf/Vorher-
  Paare im Spiel bildgleich, Leon/Elza bis auf die Figur gleich). Python 3.10 (C:/Python310, numpy 1.26.4, PIL 11.3.0).
- Plattenplatz: 29 GB frei bei Beginn; die fd/-Ordner (Framedumps) der abgeschlossenen Vorgaenger-Laeufe im selben Scratchpad (abn3e/
  13 GB, r2/ 8,6 GB) habe ich geloescht, deren Logs und Messtabellen bleiben; meine eigenen fd/-Ordner (rund 13 GB) nach der
  Auswertung ebenfalls geloescht - die Messtabellen je Bild (mass_*.txt), kam.log, deck.log und die Kontaktblaetter liegen unter abn4/.
- Gebaut nur per local_build.sh configure/build (nichts zu tun), nichts am Spielcode geaendert; committet nur dieser Bericht und abn4/.
