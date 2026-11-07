# Stufe 3 Bahnsteig-Form - Unabhaengige Abnahme 3 (Nachbesserung 4: Rueckstoss-Kamera, M-B, M-D)

Pruefer: unabhaengig (nicht der Autor), 2026-11-07 Abend. Gegenstand: Zweig r41/form_nb4, Delta 2e3713c74..b9ab865af
(7ebda1d64 Doku E23/E29, c81a3a931 Messschiene RE15_SLOTDECKUNG, 4a22c3dbc Schnittwahl Szene + Wache, 63b3b02a0 Riegel,
b9ab865af Dossier), Arbeitsbaum .claude/worktrees/r41_form_nb4. Belege abn3/ (blatt/ = selbst angesehene Kontaktblaetter,
laeufe/<lauf>/ = Messtabellen je Bild mass_*.txt, kam.log, deck.log, Zeitleiste, env.txt, exe_sha.txt; auswertung.txt =
Summenzeilen aller Laeufe; rdt_bytes.txt = selbst gelesene RDT-Bytes; werkzeug/, env/, ctest/, hashes.txt, build_*.txt).
Ein Vorgaenger-Abnehmer hatte vor einem Abbruch Rohmaterial hinterlassen; es liegt unveraendert unter abn3/vorgaenger/
(README dort). Alle Zahlen dieses Berichts stammen aus eigenen Laeufen mit eigener exe-Kopie und eigenem Werkzeug.
Bindend: Auftrag NB4 (Szenenkamera nach der Lage von Form und Spieler aus den vorhandenen Kameras des Raums, keine
erfundene Kamera, wo kein passender Schnitt: Original-Zonenkamera mit Beleg; M-B mitloesen; M-D E23/E29), Pruefmass der
Abnahme 2 M-A (kein Bild mit mehr als 60 % Bildpunkten Luma < 24, kein Spielerkoerper ueber einem Drittel des Bildes, Form
im Bild und nicht vom Spieler verdeckt), Hinweis Orchestrator (M-B-Rest nicht blockierend, solange das Fenster nicht
groesser ist als in Abnahme 2 und M-A im echten Wagenweg behoben ist), NUTZER_ENTSCHEIDE.md (Aussehen K4 nicht Gegenstand).

## 0. Urteil

**Urteil: NICHT BESTANDEN - ein blockierender Mangel (N1: die neu gebaute Nahsicht-Wache macht die Kampfkamera im
normalen Spielweg schlechter als vorher, mit Flattern), dazu M-B nicht geloest und das Fenster groesser als in Abnahme 2
(gleiche Ursache), N2 mittel.**

Der beauftragte Kern, M-A, ist behoben: Die Rueckstoss-Szene waehlt ihren Schnitt nach der Lage. In acht eigenen
Szenenlaeufen (Standardweg, 2 s zurueck, eigene Lagen 1 s zurueck und 3 s vor; je Leon und Elza; echte Spieler-HP,
Renderer des Nutzers, jedes Bild gedumpt) ist die Form in jedem Taumelbild zu sehen, kein Bild ueber 60 % fast schwarz
(max 54,6 %), der Spieler bedeckt hoechstens 7,9 % des Bildes, die Form ist hoechstens zu 40,5 % verdeckt, je Lauf genau
2 bis 3 Schnitte ohne Flattern. Die Vorher-exe (Stand 2e3713c74) zeigt mit denselben Eingaben in der 2-s-Lage 7,5 s Leons
Ruecken (10 Bilder > 60 %) und in meiner 1-s-Lage die Form 485 vor dem Auge von Cut 12 (96,7 % schwarz). Alles aus
Abnahme 2 Bestaetigte ist unveraendert (HP 300/150/150, 10 Treffer, Sturz, G5, Ton, START im Griff, Bahnsteig 5080/5081
bildgleich). M-D (a) ist erledigt.

Blockierend ist der zweite Teil von Commit 4a22c3dbc, die Nahsicht-Wache im Kampf (bf_nahsicht_wache ueber
bf_schnitt_waehlen). Gleiche Eingabe, Spiel je Bild identisch (Spieler und Form in jedem Bild gleich), nur die Kamera
anders als mit der Vorher-exe:

- **N1 (blockierend), k-Weg** (echter Wagenweg w8, 24 s warten, spaet feuern; Abnahme-2-Lauf k5090): Leon wird gegriffen,
  geworfen und steht bei x 12000-14000, die Form greift von Osten an. Vorher Cut 13 fast durchgehend, 7 Bilder > 60 %
  (max 61,8 %). Nachher **38 Bilder > 60 % (max 70,4 %), 36 davon in Cut 12**, wo die Form 1660-2160 vor dem Auge steht:
  ihre schwarze Masse fuellt 53-61 % des Bildes, Leon ist zu 56-82 % verdeckt (deck.log). Dazu Kurzschnitte: Cut 12 fuer
  4 Bilder (F1785-F1788) und 7 Bilder (F1841-F1847) zwischen Cut-13-Bildern. Elza gleich (38 Bilder, bis 89 % verdeckt).
  Mechanik (kam.log): ist kurz keine Kamera "gut", faellt die Wache auf die stabile Zonenabbildung der Spielerlage zurueck
  (x 13960 -> Cut 12, obwohl der Raum dort wegen der Hysterese Cut 13 zeigt) - und zwar auch dann, wenn dieser Rueckfall
  "nah=1" (Form an der Bildebene) und "dp=1000" (Spieler voll verdeckt) traegt; beim Zurueckgeben an die Zonen greift
  keine Haltezeit. Das ist genau das Bild (Form fuellt das Bild, Spieler dahinter), das M4/M-A abstellen sollten, neu in
  einem normalen Spielweg.
- **M-B nicht geloest, Fenster groesser als in Abnahme 2** (a-Weg, Debug-Sprung Wagen B, stehender Spieler): vorher Cut 13
  ab F1048 mit 62 Bildern > 60 %, nachher Cut 13 ab F1018 mit **92** Bildern > 60 % (Leon bis 96 % verdeckt), Elza 83.
  Die Vorbedingung des Orchestrator-Hinweises ("keine Kamera des Raums erfuellt dort das Pruefmass") stimmt fuer
  F1018-F1048 nicht: Cut 12 zeigte dort gezeichnet 19,7-32 % Spielerflaeche (F1017 eigene Messung 15180 von 76800
  Bildpunkten; F1018-F1047 Eichung des Bauers 251-320 Promille) und 39-49 % fast schwarz (Vorher-exe), also innerhalb des
  Pruefmasses; das Kugelmodell verwarf ihn (p 450-500, pl_nah=1: Spielerkugel naeher als 1000 an der Bildebene) und fiel auf
  Cut 13 mit "nah=1, dp=1000" zurueck.
- **N2 (mittel), v-Weg** (2 s zurueck, einer der beiden beauftragten Pruefwege): im Kampf vor der Szene schaltet die Wache
  F1340-F1367 (28 Bilder) auf Cut 14, den Aussenblick durch die Wagenwand: 27 Bilder 74-77 % fast schwarz, Form und
  Spieler winzig im Dunkeln. Vorher blieb Cut 13 (max 57,7 %). Leon und Elza.

Die vier Wagenweg-Riegel sind gruen, pruefen aber nur die Taumelbilder der Szene; die Kampfkamera (N1, N2, M-B) prueft
kein Riegel.

| Schritt | Stand |
|---|---|
| Bau | local_build.sh configure + build gruen, ninja "no work to do", exe sha256 b9b3d574... = Kopf (Abschnitt 1) |
| Kamerawahl gegen RID/RVD/SCD 5090/5091 | selbst per xxd gelesen, alle Offsets wie zitiert; Code gelesen: nur Raumkameras, aber Rueckfall auf die stabile Zonenabbildung statt des gezeigten Zonen-Schnitts, ohne Guete-Vergleich und ohne Haltezeit (Abschnitt 2) |
| Szene M-A, Leon und Elza, 4 Lagen | 8 Laeufe + 3 Vorher-Laeufe, behoben (Abschnitt 3.1) |
| Bestaetigtes aus Abnahme 2 | unveraendert (Abschnitt 3.2) |
| Wache im Kampf: M-B / N1 / N2 | nicht geloest / blockierend / mittel (Abschnitt 3.3) |
| M-D (a) | erledigt (Abschnitt 3.4) |
| Riegel | Satz D 13/13, Satz A und Satz C siehe Abschnitt 4 |

## 1. Bau, Delta, exe

- Delta 2e3713c74..b9ab865af ausserhalb analysis/: engine/src/birkin_formen.c (+330/-57: bf_schnitt_waehlen, Bildmass,
  Wagenkameras, Szene und Wache), platform/pc/src/render_pc.c (+89: Messschiene RE15_SLOTDECKUNG; ohne Env je Bild nur ein
  Zeigervergleich), tests/integration/test_r41_form_wagenweg.cmake (Teile leon/elza/zurueck_*, Pruefung 6a-6d),
  tests/unit/CMakeLists.txt (Registrierung entfernt), tests/unit/probes/r41_form_kamera.cmake (vier Teile neu registriert),
  tests/unit/probe_r41_form_kamera_bild.c (Luma-Werkzeug). Testliste: 1413 Tests im nb4-Bau gegen 1410 im r41_form-Bau
  (+4 Wagenweg-Teile, -1 alte Registrierung). Spiel-Asset RE2/BAHNSTEIG_FORM.EMD/.TIM unveraendert (sha256 16eb6ea8... /
  ec280802..., hashes.txt).
- Bau aus Git-Bash: bash re15_port/tools/local_build.sh configure -> "LOCAL-BUILD-OK (configure)"; build -> Preflight
  Toolchain OK, "ninja: no work to do", "LOCAL-BUILD-OK (build)" (build_configure.txt, build_build.txt). exe
  re15_port/build/platform/pc/re15_pc.exe 2026-11-07 16:39:56, sha256 b9b3d5743c65d32a... = Stand nach 63b3b02a0
  (b9ab865af aendert nur analysis/). Arbeitsbaum sauber bis auf diese Abnahme.
- Messkopie abn3e/exe/re15_pc_formabn3.exe (gleicher Hash); je Lauf eine eigene Kopie re15_pc_formabn3_<lauf>_<pid>.exe,
  Hash je Lauf in laeufe/<lauf>/exe_sha.txt (alle b9b3d574). Vorher-exe: re15_pc.exe des Worktrees r41_form (nur gelesen),
  2026-11-03 06:19:27, sha256 f4deb1a3... = Stand 2e3713c74 fuer re15_port (git diff 2e3713c74..657512427 -- re15_port
  leer, ninja -n dort "no work to do"); Laeufe damit heissen kv_/vv_/av_/ev_/uv_.

## 2. Kamerawahl gegen die Raumdaten (selbst gelesen, abn3/rdt_bytes.txt)

ROOM5090.RDT (sha256 2580e8a4..., = info/Re1.5/PSX) und ROOM5091.RDT (54705bea...): Kopf +0x24 RID = 0x60, +0x28 RVD =
0x260; die Eintraege fuer Wagen C sind in beiden Dateien bytegleich.

| Cut | RID @ | Bytes (Auszug) | Auge | Ziel | H = fov >> 7 |
|---|---|---|---|---|---|
| 11 | 0x1c0 | 00 00 3c 68 ca 1d 00 00 48 f3 ff ff f0 9c ff ff ... | (7626,-3256,-25360) | (-1504,-986,-21970) | 208 |
| 12 | 0x1e0 | 00 00 3c 68 5e 48 00 00 a0 f5 ff ff ac 9f ff ff ... | (18526,-2656,-24660) | (1496,1514,-20370) | 208 |
| 13 | 0x200 | 00 00 b7 73 2a 2b 00 00 da f3 ff ff 14 9f ff ff ... | (11050,-3110,-24812) | (15974,-1894,-23306) | 231 |
| 14 | 0x220 | 00 00 41 5e 3e 5d 00 00 e6 e2 ff ff 36 8e ff ff ... | (23870,-7450,-29130) | (22450,-1442,-23356) | 188 |
| 15 | 0x240 | 01 00 3c 68 2e 00 00 00 2c fb ff ff 12 a1 ff ff ... | (46,-1236,-24302) | (5190,-2246,-20090) | 208 |

RVD (20 Byte je Zone): Anker 11/12/13/15 @0x47c/0x4a4/0x4e0/0x544 (x -5600..28100, z -29700..-16975), Anker 14 @0x51c
(x 17000..28050); Streifen 11->12 @0x490 x 2800..4200, 12->11 @0x4b8 x 1203..2703, 12->13 @0x4cc x 14000..15400,
13->12 @0x4f4 x 12249..13873, 13->14 @0x508 x 20250..21750, 14->13 @0x530 x 18502..20252; Ende FFFFFFFF @0x558.
SCD: Pos_set (1200,-23350) @0x12fe (32 00 b0 04 00 00 ca a4), Cut_chg 0x0F @0x130e (29 0f), Cut_chg 0x0C @0x1342 (29 0c).
Alle Offsets und Werte der Dossier-Tabelle NB4-2 und des Codekommentars (birkin_formen.c 411-430) stimmen.

Bewertung des Codes (birkin_formen.c, Stand b9ab865af):
- Kandidaten (bf_wagen_kameras, Z. 578): nur Cuts, deren Ankerzone das Motiv enthaelt; Cut 14 (Auge z -29130 ausserhalb der
  Streifen) nur fuer x >= 18502 (x-Spanne seiner Streifen, zur Wagenseite offen; PORT-WAHL mit Messbeleg im Kommentar).
  Es kommen nur die fuenf Kameras des Raums vor, keine erfundene. In Ordnung.
- Szene (bf_schnitt_waehlen Z. 637-659, Aufrufe Z. 728-729 und 791-794): Reihenfolge Zonenkamera der Formlage, aktueller Schnitt, Zonenkamera der Spielerlage,
  groesstes gutes Formbild; Haltezeit BF_RS_KAM_HALT 30 (Z. 153, Gestaltung); Cut 11 ab x <= 3000 unveraendert. Gemessen
  tragfaehig (3.1).
- "Original-Zonenkamera" im Code = re15_bf_schnitt_aus_lage (Z. 709-715: x < 2750 -> 11, < 14000 -> 12, < 20250 -> 13,
  sonst 14), eine stabile Abbildung. Der Raum selbst hat Hysterese-Baender (x 2703..2800, 13873..14000, 20250..20252), in
  denen der gezeigte Schnitt vom Weg abhaengt. Bei x 13960 (N1, F1723) zeigt der Raum Cut 13, die Abbildung liefert Cut 12.
- Rueckfall (Z. 657): ist keine Kamera "gut", wird die Zonenkamera ueber bf_kamera erzwungen (Zonen aus), ohne Vergleich mit
  dem aktuellen Schnitt - auch wenn sie selbst "nah=1" (Formkugel an der Bildebene) und "dp=1000" (Spieler voll verdeckt)
  traegt (k_leon F1723, F1785; a_leon F1018). Wache (Z. 688-699): Haltezeit nur beim Wechsel auf eine Nicht-Zonenkamera
  (Z. 694); das Zurueckgeben an die Zonen (Z. 697) hat keine, danach kann die naechste "gute" Kamera sofort wieder kommen
  (k_leon: 4- und 7-Bilder-Schnitte).
- Kugelmodell gegen gezeichnete Flaeche: BF_BM_NAH 1000 (Z. 435) gilt mit Spielerkugeln r 500 auch fuer den Spieler
  (pl_nah ab 1500 vor der Bildebene). Gemessen (a_leon F1017, Cut 12): gezeichnet 197 Promille Spielerflaeche, Modell
  p=406 und ab F1018 pn=1 -> Cut 12 "nicht gut", obwohl im Pruefmass. Die Konstanten tragen ihren Beleg im Kommentar
  (PORT-WAHL, Eichung an n2/n3), keine neue RE1.5-Verhaltenskonstante; die Eichung deckt aber den Fall "Spieler 1300-1500
  vor dem Auge" nicht, dort ueberschaetzt das Modell.

## 3. Spiel (eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT/SOFTWARE_RENDER, RE15_FRAMEDUMP jedes Bild im
Pruefbereich, Fenster 640x480, RE15_SLOTDECKUNG + RE15_BF_KAMLOG als Messschienen, kein Messhaken)

Eingaben (abn3/env/): Raumladung 5090/5091 am 6030-Eintrittspunkt (27200,4900), Flag (3,89), Tastenskript w8 (bytegleich
mit abn2/env w5090/v5090), nach dem Kampfstart W2 (2 s warten) und dann: w = sofort Zielen + Feuer, v = D2.0 (2 s zurueck),
e = D1.0 (1 s zurueck, eigene Lage: Form bei Szenenbeginn 485 vor dem Auge von Cut 12), u = U3.0 (3 s vor, eigene Lage:
Form weit westlich, Spieler hinter dem Auge von Cut 11), k = W24 statt W2 (Abnahme-2-Lauf k5090). a/s/g = Debug-Sprung
Wagen B (500,-9175), stehender Spieler (Abnahme-2-Laeufe a5090/s5090/g5090). b = 5080/5081 (Abnahme-2-Laeufe b5080/b5081).
Werkzeug: mass.py (je Bild Cut, Lagen, gezeichnete Flaechen aus deck.log, fast schwarz aus dem Framedump), blatt.py
(Kontaktblaetter), hp_weg.py, ton.py, vergleich.py (Spiel je Bild zweier Laeufe).

### 3.1 Rueckstoss-Szene (M-A) - auswertung.txt Abschnitt 1, Tabellen laeufe/*/mass_szene.txt

| Lauf | Szene (Taumeln + Sturz) | Form / Spieler bei Beginn | Schnitte | Form min sichtbar (BP) | Spieler max (BP) | Form max verdeckt | max fast schwarz | Bilder > 60 % |
|---|---|---|---|---|---|---|---|---|
| w_leon | F1486-F1849 | 17290 / 14998 | 13 -> 12 (F1545) -> 11 (F1679) | 1247 | 5866 (7,6 %) | 405 | 533 | 0 |
| w_elza | F1486-F1849 | 17313 / 15021 | 13 -> 12 (F1545) -> 11 (F1679) | 1596 | 4147 | 238 | 524 | 0 |
| v_leon | F1544-F1941 | 20124 / 17815 | 13 -> 15 (F1662) -> 11 (F1768) | 2796 | 2161 | 166 | 546 | 0 |
| v_elza | F1544-F1943 | 20124 / 17665 | 13 -> 15 (F1663) -> 11 (F1769) | 2723 | 1626 | 113 | 538 | 0 |
| e_leon (D1.0) | F1624-F2020 | 19011 / 15088 | 13 -> 12 (F1713) -> 11 (F1850) | 1162 | 6055 (7,9 %) | 382 | 535 | 0 |
| e_elza (D1.0) | F1624-F2020 | 19014 / 15091 | 13 -> 12 (F1713) -> 11 (F1850) | 1536 | 4248 | 203 | 537 | 0 |
| u_leon (U3.0) | F1651-F1862 | 6262 / 9649 | 12 -> 11 (F1692) | 1540 | 883 | 226 | 510 | 0 |
| u_elza (U3.0) | F1651-F1862 | 6255 / 9645 | 12 -> 11 (F1692) | 1708 | 603 | 290 | 513 | 0 |

BP = gezeichnete Bildpunkte im 320x240-Puffer (Messschiene RE15_SLOTDECKUNG; Code gelesen: rastert die eingereihten
Dreiecke in Zeichenfolge samt Masken in einen Kennungspuffer, env-gegatet); "verdeckt" = Promille der Formflaeche hinter
Naeherem; fast schwarz selbst gerechnet (PIL 'L' < 24 im 640x480-Framedump, wie Abnahme 2). Pruefmass ueber die
Taumelbilder (Clip 13; w 263, v 297/299, e 296, u 111 Bilder): 0 Verstoesse in allen acht Laeufen. Die Werte fuer w/v
decken sich mit den Riegel-Statuszeilen und der Dossier-Tabelle NB4-5.

Gegenprobe mit der Vorher-exe (gleiche Eingaben; Spiel je Bild identisch: v_leon/vv_leon 775/775, e_leon/ev_leon 852/852,
u_leon/uv_leon 694/694 Bilder gleich): vv_leon Cut 12 fest F1544-F1771, 10 Bilder > 60 % (max 65,8 %, F1613), Leons
Ruecken gross (blatt/vv_leon_szene_vorher.jpg); ev_leon Cut 12 fest, 10 Bilder > 60 %, max 96,7 % (F1634, Form 485 vor dem
Auge; blatt/ev_leon_szene_vorher.jpg); uv_leon wie u_leon (Form bei 6262 schon vor dem Auge von Cut 12, Kamera gleich).
Der Fix wirkt also in den beiden Abnahme-2-Lagen und in einer dritten, von mir gewaehlten Lage.

Gesehen (blatt/w_leon_szene, w_elza_szene, w_leon_schnitt13_12, w_leon_ende_cut12, v_leon_szene, v_elza_szene,
v_leon_schnitt13_15, v_leon_cut15, e_leon_szene, e_elza_szene, e_elza_cut13, u_leon_szene, u_elza_szene): In jedem
Szenenbild ist die Form zu sehen und taumelt erkennbar (Clip 13, Fuesse und Kopf bewegen sich, Vortrieb nach Westen); kein
Rueckenbild des Spielers (Abnahme 2: 7,5 s), keine schwarze Folge (Abnahme 2: 16 Bilder 77-100 %). Cut 13 zeigt die Form
von hinten rechts neben dem Spieler, Cut 12 den Spieler links vorn und die Form in die Tiefe laufend, Cut 11 den
Krokodilkopf frontal bis zum Sturz durch die Tuer, Cut 15 (v) die Form rechts, rueckwaerts auf die Kamera zu. Schnittbilder
sind bytegleich mit dem Bild davor (Bildhalt beim Schnitt, wie bisher). Kein Flattern.

Hinweise (kein Mangel, dem Nutzer zeigen):
- H1 Cut 15 (v): die Form steht 3,5 s rechts im Bild, von hinten, am Rand teils angeschnitten, links grosse Feuerflaechen.
  Es ist die einzige Kamera des Raums, die in dieser Lage weder Leons Ruecken (Cut 12) noch eine Form hinter dem Auge
  (Cut 13/11) zeigt.
- H2 Cut 13 haelt, bis die Form gross ist: v F1628-F1660 51-55 % fast schwarz (Form fuellt die linke Bildhaelfte), e
  F1696-F1708 48-53 %; innerhalb der Schranke.
- H3 Ende Cut 12 (w, e): Form klein (1162-1247 BP) und bis 40 % hinter dem erhobenen Arm des Spielers, aber erkennbar.

### 3.2 Bestaetigtes aus Abnahme 2 (auswertung.txt Abschnitte 6-9, zeitleiste.txt je Lauf)

| Punkt | Soll (Abnahme 2) | Ist (alle eigenen Laeufe, Leon und Elza) | Bewertung |
|---|---|---|---|
| HP Wagen A/B/C, echter Weg | 300 / 150 (F498) / 150 (F946) | 14 Wagenweg-Laeufe (w/v/e/u/k, Kopf- und Vorher-exe): 300 / 150 / 150 | unveraendert |
| Kampf, Rueckstoss, Sturz, G5 | 10 Treffer, Rueckstoss F1486 (w) / F1544 (v), Sturz F1749 / F1841 (Elza F1843), G5 F1850 / F1942 | identisch; e: Rueckstoss F1624 (Form 19011), Sturz F1920, G5 F2021; u: F1651 / F1762 / F1863 | unveraendert |
| Ton (Raum-Ton 10) | w 3 gespielt / 2 vom SE-Tor verworfen, v 5/0, 5080 6/1, SILENT nur se=11 | w 3/2/0, v 5/0/0, e 6/1 (Leon) und 7/0 (Elza), u 4/1, b 6/1/0, a 6/0 mit 2x SILENT se=11 (Gang-Clipende, wie Abnahme 2) | unveraendert |
| START im Griff | verworfen, keine Vormerkung | s_leon RE15_PAD_AT 726/735/745/755, s_elza 719/728/738/748: ms=0 in allen 300 Bildern F700-F999 (34 Bilder gr=1), Zeitleiste bis F999 bildgleich mit a_leon/a_elza; Gegenprobe g_leon START F770 nach dem Loslassen (F759): ms=2 (blatt/g_leon_start.jpg) | unveraendert |
| Bahnsteig 5080/5081 | Fall, Auftritt, Kampf, Griff, Flucht, Raumtext msg 3 F1522 | Zeitleisten (abn2/werkzeug/zustand.py) bildgleich mit abn2/zeitleisten b5080_leon / b5081_elza (50 Zeilen), msg id=3 ab F1522 (blatt/b_leon_5080.jpg) | unveraendert |
| Spiel-Asset, Pins | K4/NB2 | Blobs unveraendert (hashes.txt), Pins nicht angefasst (Delta ohne tests/unit/form_j_posen.h) | unveraendert |

### 3.3 Nahsicht-Wache im Kampf (M-B, N1, N2, E1) - vorher = exe f4deb1a3 (Stand 2e3713c74), nachher = Kopf-exe, gleiche Eingabe

| Lauf | Fenster | vorher: Schnitte / Bilder > 60 % | nachher: Schnitte / Bilder > 60 % |
|---|---|---|---|
| k Leon (w8 + 24 s warten) | F1300-F1931 | 12 / 13 (1) / 12 (7) / 13 F1332-F1896 / 12 (3) / 13: **7** (max 618) | 12 / 13 F1355-F1722 / **12 F1723-F1753** / 13 / **12 F1785-F1788** / 13 / **12 F1841-F1847** / 13 / 12 F1879-F1931: **38** (36 in Cut 12, max 704) |
| k Elza | F1300-F1931 | - | wie Leon: 38 (36 in Cut 12, max 697) |
| a Leon (Wagen B, steht; M-B) | F700-F1200 | 12 / 13 F731-F844 / 12 F845-F1047 / 13 ab F1048: **62** (F1049-F1108, F1124-F1125) | 12 / 13 F731-F741 / 12 F742-F1017 / 13 ab F1018: **92** (F1019-F1108, F1124-F1125), Leon bis 960 Promille verdeckt |
| a Elza | F700-F1200 | - | 13 ab F1012: 83, Elza bis 997 Promille verdeckt |
| v Leon (2 s zurueck), Kampf | F1169-F1543 | 13 F1231-F1543: **0** (max 577) | 13 / **14 F1340-F1367** / 13: **28** (27 in Cut 14, 741-772; 1 Schnittbild) |
| v Elza | F1169-F1543 | - | wie Leon: 28 (max 769) |
| e Leon (1 s zurueck), Kampf | F1169-F1623 | 13 / 12 F1313-F1339 / 13: **0** (max 540) | 13 / 12 F1317-F1347 / 13: **6** (F1343-F1347 bis 699; Leons Ruecken 29124 BP = 37,9 % > ein Drittel, F1347) |
| e Elza | F1169-F1623 | - | 0 (Elza max 16248 BP = 21 %) |

Spiel je Bild (Spieler x/z/hp, Form x/z/Clip/Bild/HP/Zustand) ist in allen Vorher/Nachher-Paaren identisch (k 632/632,
a 501/501, v 775/775, e 852/852); der Unterschied ist allein die Kamera.

N1 im Bild (blatt/k_leon_1716_nachher.jpg gegen kv_leon_1716_vorher.jpg, k_elza_1716_nachher.jpg): nachher F1724-F1752
Cut 12, die schwarze Masse der Form fuellt das Bild, Leons Kopf und Schulter nur oben in der Mitte dahinter; vorher
dieselben Bilder in Cut 13 mit der Form rechts von hinten und Leon sichtbar in der Mitte. Flattern
(blatt/k_leon_flattern_1782.jpg, k_leon_flattern_1839.jpg): F1785-F1788 und F1841-F1847 vier bzw. sieben Bilder Cut 12
(schwarze Masse) zwischen Cut-13-Bildern. Mechanik (laeufe/k_leon/kam.log): F1723 ist keine Kamera "gut" (c12 nah=1 f=983
dp=1000, c13 f=913 > 900, c15 Spieler nicht im Bild) -> Rueckfall re15_bf_schnitt_aus_lage(13960) = 12 mit Haltezeit 30
(F1723-F1753), obwohl der Raum dort Cut 13 zeigt (Hysterese 13873..14000) und Cut 12 die schlechteste Kandidatin ist.
F1785 und F1841: Cut 13 ist beim Biss-Vorstoss kurz "nicht gut" (f 991 bzw. 954 > 900), c12 ebenfalls nicht -> Rueckfall
Zonenkamera der Spielerlage (x 13493 / 13026 -> 12) = Zonen-Schnitt -> bf_kamera_zurueck ohne Haltezeit -> sobald Cut 13
wieder "gut" ist (F1789, F1848), zurueck auf 13. Die Vorher-exe hatte ebenfalls Kurzschnitte (F1324 1 Bild, F1325-F1331,
F1897-F1899), aber in Cut 12 mit der Form fern (max 49,8 %), nicht die schwarze Masse.

M-B (blatt/a_leon_mb_nachher.jpg gegen av_leon_mb_vorher.jpg, a_elza_mb_nachher.jpg): Der Rest ist nicht geloest und das
Fenster ist groesser geworden. Nachher ab F1018 Cut 13: die Form fuellt das Bild (50-71 % fast schwarz), Leon zu 96 %
verdeckt; vorher blieb bis F1047 Cut 12 mit Leons Ruecken rechts unten und der Form neben ihm (F1018-F1047: 41-49 % fast
schwarz, av_leon; Leon gezeichnet 25-32 % des Bildes nach der Eichung des Bauers, nb4/eichung_n3_a_leon.txt p_bedeckt
251-320 Promille bei Modell p 450-500). Kalibrierbeleg (laeufe/a_leon/mass_700_1200.txt, kam.log F1016-F1019): F1017 Cut 12
gezeichnet Spieler 15180 BP = 197 Promille, fast schwarz 393; Modell c12 p=406, F1018 p=450 und pn=1 -> "nicht gut"; c13
nah=1 dp=1000 ebenfalls nicht
-> Rueckfall Zonenkamera der Spielerlage (x 17070 -> 13). Fuer F1049-F1125 (Leon 1308-812 vor dem Auge von Cut 12; nach
der Eichung des Bauers, nb4/eichung_n3_a_leon.txt, dort in Cut 12 gezeichnet 26-48 % des Bildes, also teils ueber einem
Drittel) hat der Raum tatsaechlich keinen Schnitt im Pruefmass - das ist der Rest, den der Orchestrator als PORT-WAHL
Cut 13 setzt; er ist so gross wie in Abnahme 2 (F1051-F1174). Die 30 Bilder F1019-F1048 kommen neu hinzu.

N2 (blatt/v_leon_kampf_1320.jpg, v_elza_kampf_1320.jpg): F1333-F1339 laeuft die Form in Cut 13 auf Leon zu und verdeckt
ihn tatsaechlich (gezeichnet 862-998 Promille, Rennen-Treffer F1336). F1340 wird Cut 14 Kandidat (Leon x 18553 >= 18502):
Modell c14 f=127 p=34 "gut" -> Wache schaltet; gezeichnet sind Form (4350-4644 BP) und Leon (603-948 BP) hinter der
Wagenwand klein im Dunkeln, 74-77 % fast schwarz, 28 Bilder (Haltezeit 30 abzueglich Schnittbild). Vorher blieb Cut 13; ab
F1340 ist Leon dort wieder frei (deck.log). Das Modell kennt die Helligkeit des Hintergrunds nicht; der Kommentar zu
bf_wagen_kameras nennt den Aussenblick selbst "dunkel".

E1 (blatt/e_leon_kampf_1310.jpg): F1317 schaltet die Wache zu Recht auf Cut 12 (Form 650 vor dem Auge von Cut 13); die
starre Haltezeit 30 laesst sie dort, waehrend Leon 1,3 s rueckwaerts bis 1309 vor das Auge von Cut 12 geht: F1343-F1347
61-70 % fast schwarz, Leons Ruecken 37,9 %. Die Vorher-exe (gleiche Achsenregel-Wahl Cut 12 F1313) ging bei F1340 auf Cut 13
zurueck (0 Bilder > 60 %). Elza (kleinere Figur) bleibt unter einem Drittel.

### 3.4 M-D (a)

NUTZER_ENTSCHEIDE.md (Commit 7ebda1d64): E23 (2026-11-03, "passt erst einmal", Pose Bild 2 nicht drehen: "nein") und E29
(2026-11-07 ~10:15, Vorschlag "K4 bleibt fuer v0.8.30 im Spiel, der G4-Ansatz danach", Wortlaut "Das koennte schon passen
mit der riesen Form ..." vollstaendig) nachgetragen, Folgen benannt (kein Rueckbau, Farbvarianten getrennt). Erledigt.
M-C und M-D (b)/(c) waren nicht beauftragt und sind unveraendert.

## 4. Riegel (ctest aus Git-Bash, PATH msys64 zuerst, SDL_ASSERT=always_ignore, --test-dir r41_form_nb4/re15_port/build; Logs abn3/ctest/)

- Satz D -R "^unit_(birkin|r41_form|bb_|g5|n2_griff|r41_birkin)" -j 6: **100% tests passed, 0 tests failed out of 13**
  (242,9 s; unit_birkin_szenen, unit_birkin_formen, unit_birkin_bahnsteig, unit_r41_form_emd, unit_n2_griff_start, G5, Wand).
- Satz A (Auftrag/Bauer) -R "form|bahnsteig|birkin_form|r41_form|integration_r41_birkin_pistole|integration_r41_g5_tod|
  integration_r41_g5_westen" -j 4: **100% tests passed, 0 tests failed out of 17** (354,7 s): integration_r41_form_wagenweg
  (113,0 s), _elza (110,0 s), _zurueck_leon (112,7 s), _zurueck_elza (111,9 s), integration_r41_birkin_pistole (244,6 s),
  integration_r41_g5_tod (264,8 s), integration_r41_g5_westen (285,9 s), integration_r35_werfer_form, integration_r34_plattform_takt
  /_esp_eintritt, unit_birkin_bahnsteig, unit_birkin_formen, unit_r41_form_emd, unit_r39_funk10f1_form, unit_r34_plattform(_ton),
  unit_r35_fenster_satzform. Die Szenenkennzahlen der vier Wagenweg-Laeufe habe ich aus deren Arbeitsverzeichnissen
  (re15_port/build/tests/integration/r41_form_wagenweg_<teil>_wd/lauf_*/state.log + deck.log) mit mass.py nachgerechnet:
  leon 13/12/11, Form min 1247, Leon max 5866, verdeckt max 405; elza 1596/4147/238; zurueck_leon 13/15/11 2796/2161/166;
  zurueck_elza 2723/1626/113 - gleich den Statuszeilen des Bauers und meinen eigenen Laeufen (3.1). Kein roter Riegel,
  nichts nachzufahren.
- Satz C (render_pc.c im Delta) -R "render|pri_|_pri|maske|masken|schatten|sprite|kamera|cut_|framedump|slot" -j 4:
  **100% tests passed, 0 tests failed out of 64** (2081,7 s; darunter die 28 langen masken36-/pri-Integrationslaeufe mit
  430-530 s). Kein roter Riegel, nichts nachzufahren.
- Die vier Wagenweg-Riegel pruefen nur die Taumelbilder der Szene (6a-6d); die Kampfkamera vor der Szene (N1, N2, M-B, E1)
  prueft kein Riegel - die Regression hat die Gates deshalb nicht ausgeloest.

## 5. Maengel und Hinweise

| Nr | Was | Beleg | Schwere | Vorschlag |
|---|---|---|---|---|
| N1 | **Nahsicht-Wache (Commit 4a22c3dbc) im echten Wagenweg mit wartendem Spieler:** Rueckfall auf die stabile Zonenabbildung (x 13960 -> Cut 12, Raum zeigt 13) ohne Guete-Vergleich, Cut 12 mit der Form 1660-2160 vor dem Auge: 36 Bilder > 60 % fast schwarz, Spieler 56-82 % verdeckt; Kurzschnitte 4 und 7 Bilder, weil das Zurueckgeben an die Zonen keine Haltezeit hat. Vorher (gleiches Spiel je Bild) 7 Bilder > 60 %. Leon und Elza. | auswertung.txt 3, laeufe/k_leon, k_elza, kv_leon (kam.log F1723, F1785-F1789, F1841-F1848; mass_1300_1931.txt), blatt/k_leon_1716_nachher, kv_leon_1716_vorher, k_elza_1716_nachher, k_leon_flattern_1782, k_leon_flattern_1839 | **hoch (blockierend)**: Regression dieser Nachbesserung in einem normalen Spielweg (Spieler wird gegriffen/geworfen und steht dann bei x 12000-14000), genau das Bild, das M4/M-A abstellen sollten, dazu Flattern | Kleinste Loesung: die Szenenwahl (bf_schnitt_waehlen fuer "rs", Z. 727-731 in re15_bf_rueckstoss_start und Z. 788-795 in bf_rs_riesenform_phase1) behalten - sie ist in 8 Laeufen gemessen gut - und bf_nahsicht_wache (Z. 668-701) auf den Stand 2e3713c74 (Achsenregel) zuruecknehmen: damit k 7 statt 38, a 62 statt 92, v 0 statt 28 dunkle Bilder (gemessen mit der Vorher-exe). Sonst Wache so aendern: nie auf eine Kamera mit nah=1 oder dp groesser als beim aktuellen Schnitt wechseln; Haltezeit auch nach dem Zurueckgeben; Zonenkamera = gezeigter Zonen-Schnitt (work_vars[0x0A] mit Hysterese) statt der stabilen Abbildung. Riegel fuer den Kampf: k-Weg Leon/Elza (kein Kamera-Abschnitt kuerzer als 30 Bilder, Bilder > 600 nicht mehr als 7), a-Weg (Bilder > 600 nicht mehr als 62), v-Weg Kampffenster (0). |
| M-B | Cut-13-Rest nicht geloest, Fenster groesser als in Abnahme 2: 92 statt 62 Bilder > 60 %, Beginn F1019 statt F1049 (Abnahme 2: F1051). Fuer F1018-F1048 erfuellte Cut 12 das Pruefmass (gezeichnet 19,7-32 % Spieler, 39-49 % fast schwarz); das Modell (p 450-500, pl_nah ab 1500 vor der Bildebene) verwarf ihn und fiel auf Cut 13 mit nah=1/dp=1000 zurueck. Der Rest F1049-F1125 ist wie in Abnahme 2 (Orchestrator: PORT-WAHL Cut 13). | auswertung.txt 4, laeufe/a_leon, av_leon, a_elza (kam.log F1016-F1019, mass_700_1200.txt), blatt/a_leon_mb_nachher, av_leon_mb_vorher, a_elza_mb_nachher | mittel, aber die Vorbedingung "nicht groesser als in Abnahme 2" ist nicht erfuellt (Teil von N1: gleiche Ursache im Rueckfall) | mit N1 (Ruecknahme der Wache stellt F1018-F1048 Cut 12 wieder her); Rest F1049-F1125 bleibt Nutzerentscheid (Cut 12 mit grossem Spielerruecken oder Cut 13 mit grosser Form) |
| N2 | v-Weg (beauftragter Pruefweg), Kampf vor der Szene: Wache schaltet F1340-F1367 auf den dunklen Aussenblick Cut 14 (27 Bilder 74-77 % fast schwarz, Figuren winzig), vorher Cut 13 mit Leon ab F1340 frei. Leon und Elza. | auswertung.txt 2, laeufe/v_leon, v_elza, vv_leon (kam.log F1338-F1341, mass_1169_1543.txt), blatt/v_leon_kampf_1320, v_elza_kampf_1320 | mittel | mit N1; Cut 14 im Kampf nur, wenn er auch gezeichnet hell genug ist (oder gar nicht als Wache-Kandidat, nur als Zonenkamera ab x >= 20250) |
| E1 | e-Weg (eigene Lage 1 s zurueck), Kampf: Cut 12 mit starrer Haltezeit 30, waehrend Leon rueckwaerts vor das Auge geht: F1343-F1347 61-70 % fast schwarz, Leons Ruecken 37,9 % (> ein Drittel); vorher 0 | auswertung.txt 2, laeufe/e_leon, ev_leon (mass_1169_1623.txt, kam.log F1316-F1318, F1348), blatt/e_leon_kampf_1310 | niedrig (5 Bilder, nur Leon) | mit N1 |
| H1-H3 | Szene: Cut 15 Form am rechten Rand angeschnitten (v); Cut 13 bis 55 % kurz vor dem Wechsel (v, e); Form am Ende von Cut 12 klein, bis 40 % hinter dem Arm (w, e) | 3.1 | Hinweis | dem Nutzer zeigen |
| H4 | u-Weg (eigene Lage 3 s vor, Spieler bei x 9649 feuert hinter dem Auge von Cut 11): in 6 Einzelbildern (F1697, F1787, F1811, F1922, F1988, F2053) ein grau-gruener Block oben links, deck.log Slot 51 (Effekt) bis 34410 Bildpunkte; bytegleich mit der Vorher-exe (gleiche 6 Bilder, gleiche Pixelzahlen) - vorbestehend, nicht Teil dieses Deltas (Projektion eines Effekts hinter der Bildebene). Elza 5 Bilder. | laeufe/u_leon, uv_leon, u_elza (deck.log F1787/F1811), blatt/u_leon_block_1783, u_elza_block_1783 | Hinweis (kein Delta-Befund) | eigene Spur: Effekt-Sprites hinter der Bildebene nicht zeichnen |
| H5 | Vorgaenger-Hinweis bestaetigt: die Dossier-Tabelle NB4-5 wurde mit einer Zwischen-exe gemessen (nb4/laeufe/*/exe_sha.txt b4cc39be); meine Laeufe mit der Kopf-exe ergeben dieselben Zahlen | abn3/vorgaenger/, 3.1 | Hinweis | - |
| H6 | Riegel-Abschlusszeile schreibt auch im Elza-Teil "Leon getroffen" (fester Text, test_r41_form_wagenweg.cmake Z. 416; die Fehlerzeile Z. 269 nutzt schon die Figur) | Quelltext | Hinweis | Text |

Nicht als Mangel gewertet: Aussehen K4 (E23/E29); Cut 14 als Original-Zonenkamera fuer x >= 20250 (Raumregel); se=11 stumm
am Gang-Clipende (wie Abnahme 2); die Kurzschnitte der Vorher-exe im k-Weg (vorbestehend, Form fern).

## 6. Messweg und Umgebung

- Werkzeug abn3/werkzeug/: lauf.sh (eigene exe-Kopie je Lauf aus abn3e/exe, Laufverzeichnis im Scratchpad,
  RE15_RE2_ASSET_ROOT = private Kopie der HEAD-Blobs BAHNSTEIG_FORM (hashes.txt), RE15_FRAMEDUMP-Serie, RE15_EXIT_AT,
  Notbremse timeout nur auf den eigenen Kindprozess, nie taskkill /IM; 4. Argument "vorher" nimmt die Vorher-exe),
  mass.py, blatt.py, hp_weg.py, ton.py, vergleich.py; fuer die Zeitleisten das Werkzeug der Abnahme 2
  (abn2/werkzeug/zustand.py) gegen deren Dateien abn2/zeitleisten/. Eingaben abn3/env/*.env (w/v/k/a/s/g/b bytegleich mit
  den Abnahme-2-Eingaben bis auf laengere Framedump-Serien; e/u eigene Lagen).
- 22 Laeufe, alle exit 0 per RE15_EXIT_AT (auswertung.txt 9): 17 mit der Kopf-exe b9b3d574, 5 mit der Vorher-exe f4deb1a3.
  Framedumps 640x480 (Fenster, RE15_WINDOW_SCALE=2), deck.log 320x240. Laeufe parallel zu je 4-6, Spiel je Bild
  deterministisch (Vorher/Nachher-Paare bildgleich im Spiel; Elza-Laeufe bis auf die Figur gleich).
- Python 3.10 (C:/Python310, numpy 1.26.4, PIL 11.3.0); die msys64-Python hat kein numpy.
- Gebaut nur per local_build.sh configure/build (nichts zu tun), nichts am Spielcode geaendert; committet nur dieser
  Bericht und abn3/ (eigene Belege sowie das Rohmaterial des Vorgaengers unter abn3/vorgaenger/).
