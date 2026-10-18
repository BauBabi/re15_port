# Runde 39 — Spur M "schluessel2" (Dossier, fortlaufend)

Baum `.claude/worktrees/r39_schluessel2` (Zweig `r39/schluessel2`, Basis f70f6ec4). Vorstand: Spur L Punkt 2
(`L_optik.md` Punkt 2, `L_abnahme_0.md` 3.x) — Schluessel 10F0/10F1 auf RE2-Bildgroesse (oberes Quartil `RE2_QUANTIL = 0.75`).
Messwerkzeuge (Kopien in `M_bilder/`, Aufruf `bash mess.sh <exe-kopie> <ordner> [skalen]`): `mess.sh` (Sprung-Weg wie
`integration_r39_optik_schluessel`), `mess_continue.sh` (Spielstand + CONTINUE wie `integration_r38_itemorte` k_lauf),
`mess_r38.sh` (Szenen B2/B3/V3/V5 der Runde-38-Abnahme), `mess_aufnahme.sh` (Aufnahme Leon/Elza + Skala 2); Auswerter
`probe_r38_itemorte_kontrast` (Pixel mit |dR|+|dG|+|dB| > 30 zwischen "mit" und "ohne" (Taken-Bit gesetzt), Huelle,
Sum|dL|, AUSSEN). Messkopien der exe neben der exe des Baums: `re15_pc_r39m_vorher.exe` (Basis), `_nach.exe` (nur stetige
Regel), `_nach2.exe` (Endstand).

## Punkt (Wortlaut des Nutzers, AUFTRAG.md Nachtrag 5, 2026-10-18 19:29)
1. "Und die Schlüssel im Communicationsroom sind immer noch zu groß, die sollten maximal halb so groß sein."

"Communicationsroom" = ROOM10F0 (Raumname "COMMUNIC. ROOM", DEBUG.BIN @Datei 0x027C0, `map_hint_common.c:38`), Elza-Zwilling
ROOM10F1. "Die Schluessel" = Parking Lot Key (Leon, Item 0x45, ROOM10F0) und Prison Key (Elza, Item 0x42, ROOM10F1), beide
an der Pinnwand (Lage (1358,-2495,-3400), Zone Slot 8), seit Runde 38 K als skalierte RE2-MD1 (`gen/itemorte_r38_md1.inc`).

## Messung vorher (exe des Baums = Basis f70f6ec4 = master mit Spur L, gebaut 19:35, Messkopie `re15_pc_r39m_vorher.exe`)

Szenen wie Abnahme L0: Leon `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=10F0@250
RE15_PLAYER_POS=0,4000,1024 RE15_FORCE_CUT=5 RE15_SET_FLAG=9:71,3:94 RE15_EXIT_AT=150#10F0` ("ohne" + `9:82`), Elza
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_GOTO_ROOM=10F1 RE15_PLAYER_POS=0,4000,1024
RE15_FORCE_CUT=5 RE15_EXIT_AT=150#10F1` ("ohne" `RE15_SET_FLAG=9:83`), Framedump Bild 150, Fenster = Pinnwand-Fenster der
Runde-38-Abnahme x Skala (x 131..145 y 75..87 bei Skala 1). debug.log: Leon `Spieler-Familie PL00`, Elza `PL04` in allen Laeufen.

| Lauf | Pixel | Huelle (Breite x Hoehe) | Sum|dL| | AUSSEN |
|---|---|---|---|---|
| Leon Parking Lot Key, Skala 1 (320x240) | 7 | x 136..140 y 80..81 (5 x 2) | 506 | 0 |
| Leon, Skala 3 (960x720, so spielt der Nutzer) | 69 | x 408..421 y 239..246 (14 x 8) | 3731 | 0 |
| Leon, Skala 4 (1280x960) | 125 | x 544..561 y 318..328 (18 x 11) | 6311 | 0 |
| Elza Prison Key, Skala 1 | 5 | x 137..140 y 80..81 (4 x 2) | 317 | 0 |
| Elza, Skala 3 | 63 | x 408..423 y 239..248 (16 x 10) | 3960 | 0 |
| Elza, Skala 4 | 118 | x 543..564 y 318..331 (22 x 14) | 5524 | 0 |

Bitgleich zu Abnahme L0 (69 / 14 x 8, 63 / 16 x 10) und Dossier L (Skala 4: 125 / 18 x 11, 118 / 22 x 14).
ECHTER Lade-Weg (Spielstand mit `probe_r37_feuerloescher_karte` + `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`, Bild 100, debug.log
`CONTINUE: resumed in room 10f0` / `10f1`, Elza `PL04`): Skala 3 Leon 69 / x 408..421 y 239..246 / 3731, Elza 63 / x 408..423
y 239..248 / 3960; Skala 4 Leon 125 / 18 x 11 / 6311, Elza 118 / 22 x 14 / 5524 — bitgleich zum Sprung-Weg.

Werkzeug-Stand (`itemorte_export.py` ohne `--schreiben`, Basis): Park s 2,4100, gezeichnete Huelle 4,461 x 2,985 px (320er
Bild), Prison s 2,5623, 5,770 x 2,868 px; `--schreiben` erzeugt die committeten `gen/itemorte_r38_*.inc` bytegleich (nur
Zeilenenden; `git diff` leer) — der Generator liest seine eigene Ausgabe nicht (Fixpunkt).

**Grenze "maximal halb so gross" in den Messgroessen (Skala 3):** Parking Lot Key Huelle <= 7 x 4, Prison Key <= 8 x 5
(Skala 4: <= 9 x 5 bzw. 11 x 7); die Flaeche (Pixelzahl) faellt damit auf hoechstens etwa ein Viertel.

## Belege (Form der Nutzer-Vorgabe, Werkzeug-Kette)

* **Zahl = NUTZER-VORGABE, kein RE-Wert:** Faktor 0,5 linear (`NUTZER_FAKTOR_M = 0.5` in
  `tools/r38_itemorte/itemorte_export.py`, `RE15_R39M_FAKTOR100 50` in `gen/itemorte_r38_lage.inc`) auf den Stand Spur L
  (RE2-Quartil-Groesse). "maximal halb so gross" ist eine OBERGRENZE: 0,5 linear haelt sie fuer beide Lesarten (Laenge 0,5;
  Flaeche 0,25 <= 0,5). Genommen wird das GROESSTE Bild, das sie einhaelt (groesste zulaessige Stufe des GTE-Rasters,
  s.u.) — nicht kleiner als noetig, weil der Schluessel sichtbar und aufnehmbar bleiben muss (Runde 38 "gut sichtbar",
  Runde 39 L "kleiner/subtiler"); der Massstab ist die Mitte des Intervalls, das genau dieses Bild zeichnet.
* **Form: Bildgroesse linear im Massstab.** Der Massstab steckt nur in den Ecken des gelieferten MD1 (@0x44, Runde 38 K;
  `schluessel_r36.c:359` liefert `re15_itemorte_r38_schluessel_md1`, kein Laufzeit-Massstab — `re15_itemorte_r38_schluessel_massstab`
  lesen nur Riegel). Die Projektion ist die GTE-RTPS `MAC0=(((H*20000h/SZ3)+1)/2)*IR1+OFX, SX2=MAC0/10000h`
  (psx-spx `geometrytransformationenginegte.md:418`): bei fester Tiefe SZ3 linear in IR1, also in s. H kommt aus dem
  Kamera-Satz, selbst disassembliert (`re15_disasm.py dis 0x80021e50`, RE1.5 PSX.EXE):
  ```
  80021e68: lhu a0,2(v0)        ; Kamera-Satz (RDT+0x24 + 32*cut) +2 = fov
  80021e6c: jal 0x80066c30      ; SetGeomScreen
  80021e70: srl a0,a0,7         ; (Verzoegerungsplatz) H = fov >> 7
  80066c30: 00 d0 c4 48  gte_ldH a0  (ghidra1_V2.txt "Possible REG13.OBJ/SetGeomScreen")
  ```
  ebenso @0x80046120 `lhu a0,2(v0)` / @0x80046124 `jal 0x80066c30` / @0x80046128 `srl a0,a0,7`. Im Port
  `camera_common.c` `fov_to_screen_dist` (fov >> 7). Die Pinnwand-Ebene (z = -3400, Schluessel flach darauf) liegt im
  Nutzer-Cut 5 bei vz ~13000 fast auf einer Tiefe; den Rest (Tiefenunterschied ueber die Schluesselflaeche, gerundete
  Ecken) rechnet das Werkzeug am GEZEICHNETEN Netz genau (Bisektion, s.u.).
* **Zweite Grenze: das GTE-Raster (ganzzahlige Bildschirm-Ecken).** Gemessen und disassembliert im Abschnitt
  "Zwischenstand 1 / Mechanismus": der Objekt-Zeichner FUN_80022f0c schreibt die RTPT-Ergebnisse SXY0..2 als ganze Zahlen
  ins GPU-Polygon (`@0x80022fb0 4a280030` RTPT, `@0x80022fd8 08 00 8c e9 / 14 00 8d e9 / 20 00 8e e9` swc2 nach +8/+0x14/
  +0x20); der Port tut dasselbe (`objpoly_r38.c`, TRILOG-Ecken). Bei 2..6 Bildpunkten Groesse entscheidet dieses Raster,
  wie gross der Schluessel gezeichnet wird -> "maximal halb so gross" gilt auch fuer das GTE-Viereck.
* **Lage bleibt:** das Werkzeug zentriert das MD1 auf die Mitte des sichtbaren Texel-Rechtecks (mx, mz) und skaliert um diese
  Mitte; `pos = round(mitte)` (Sehstrahl der Nutzer-Marke x Wand z -3400) haengt nicht von s ab -> Lage (1358,-2495,-3400),
  Drehung (3072,0,2048) bzw. (1024,0,0), flaches Licht 0x0010 und Aufhebe-Zone (798,-3600) 1000x1000 / Slot 8 unveraendert.

## Zwischenstand 1: stetige Regel (Commit 84348b07) — gemessen, NICHT ausreichend

Werkzeug mit nur der stetigen Regel (gezeichnete Huelle <= 0,5 x Spur L): Park s 2,4100 -> 1,1992 (Huelle 2,212 x 1,485 px
= 49,6 % / 49,7 %), Prison s 2,5623 -> 1,2809 (2,878 x 1,429 px = 49,9 % / 49,8 %). Gebaut (exe 19:42, Messkopie
`re15_pc_r39m_nach.exe`), gleiche Laeufe wie vorher:

| Lauf | Pixel | Huelle (B x H) | Sum|dL| | gegen vorher |
|---|---|---|---|---|
| Leon Skala 1 | **0** | - | 0 | 7 Pixel -> unsichtbar |
| Leon Skala 3 | 13 | x 413..416 y 241..246 (**4 x 6**) | 589 | 14 x 8 -> Breite 29 %, **Hoehe 75 %** |
| Leon Skala 4 | 24 | x 550..555 y 322..328 (6 x 7) | 1163 | 18 x 11 -> 33 % / **64 %** |
| Elza Skala 1 | **0** | - | 0 | 5 Pixel -> unsichtbar |
| Elza Skala 3 | 16 | x 410..417 y 242..246 (8 x 5) | 748 | 16 x 10 -> 50 % / 50 % |
| Elza Skala 4 | 28 | x 547..556 y 322..328 (10 x 7) | 1215 | 22 x 14 -> 45 % / 50 % |

(CONTINUE-Weg Skala 3/4 bitgleich.) Der Parking Lot Key ist in der HOEHE nicht halbiert (6 statt <= 4 Zeilen bei 960x720)
und als Form zerfallen (Bild `M_bilder/m_s3_s4_vorher_stetig_maske_x8.png`, Spalten Spur L | Maske | stetige Regel |
Maske: Kopf als senkrechter Strich, Klinge fast weg). Die stetige Huelle
sagt das nicht voraus -> Mechanismus gesucht statt Zahl gedreht.

### Mechanismus: ganzzahlige Bildschirm-Ecken (GTE RTPT -> GPU-Polygon), gemessen und disassembliert
* **Gemessen (Port):** `RE15_TRILOG=tri.log@133,77,144,86` (Messschiene render_pc.c, protokolliert jedes eingereihte
  texturierte Dreieck mit den Ecken, die `re15_render_textured_tri_lit` als `int` bekommt), Bild 150, Skala 3:
  - Parking Lot Key Spur L: Ecken (135,79) (141,79) (141,82) (135,82), Mitte (138,81) = Viereck **6 x 3** (320er Raster)
  - Parking Lot Key stetige Regel (s 1,1992): (137,80) (139,80) (139,82) (137,82), Mitte (138,81) = **2 x 2**
  - Prison Key Spur L: (135,79) (141,79) (141,83) (135,83) = **6 x 4**; stetige Regel (s 1,2809): (136,80) (139,80)
    (139,82) (136,82) = **3 x 2**
  UV je Dreieck unveraendert (Park u 0..127 v 192..255, Prison u 0..127 v 128..191, Mitte (63,224) bzw. (63,160)).
  Die Textur (128 x 64 Texel, Seitenverhaeltnis 2:1) wird also beim Parking Lot Key in ein 2 x 2-Viereck gezogen:
  Breite 1/3, Hoehe 2/3 von Spur L -> 4 x 6 Pixel. Die ganze Fenster-Aufloesung (Skala 3) rastert nur die Flaechen
  feiner; die ECKEN liegen auf dem 320 x 240-Raster (`render_pc.c` `t->v[k].position.x = (float)x0 + ab`).
* **Original (RE1.5 PSX.EXE, selbst gelesen):** der Objekt-Zeichner FUN_80022f0c rechnet RTPT und schreibt die
  GANZZAHLIGEN Bildschirm-Ecken SXY0..2 unmittelbar ins GPU-Polygon (POLY_GT3: xy0 +8, xy1 +0x14, xy2 +0x20):
  ```
  80022fb0: 4a280030   RTPT
  80022fd4: addu t4,a1,zero          ; t4 = Polygon
  80022fd8: e98c0008   swc2 SXY0, 8(t4)
  80022fdc: e98d0014   swc2 SXY1, 0x14(t4)
  80022fe0: e98e0020   swc2 SXY2, 0x20(t4)
  80022fec: 4b400006   NCLIP
  80023010: bltz v0,0x80023080       ; MAC0 < 0 -> verwerfen
  80023020: 4b58002d   AVSZ3
  ```
  (Bytes `@0x80022fd8: 08 00 8c e9 14 00 8d e9 20 00 8e e9`, `@0x80022fec: 06 00 40 4b`.) RTPS/RTPT liefert SX2 =
  MAC0/10000h als ganze Zahl (psx-spx `geometrytransformationenginegte.md:418`). Der Port bildet das in `objpoly_r38.c`
  (`re15_objpoly_r38_rtps`, UNR-Division `re15_gte_divide`) nach — die TRILOG-Ecken oben SIND diese Werte.
* **Folge:** Ein Objekt dieser Groesse (Spur L: 6 x 3 Pixel im 320er Bild) kann nur in ganzen Pixeln halbiert werden.
  Die stetige Regel trifft beim Parking Lot Key das Viereck 2 x 2 — die Hoehe springt durch das Raster von 3 auf 2 statt
  auf 1,5. Die Groesse, die der Spieler sieht, entscheidet das GTE-Viereck, nicht die stetige Huelle.
* **Nachrechnung im Werkzeug (exakt):** dieselbe Ganzzahl-Kette in Python (Kamera-Satz R/T/H aus
  `probe_r34n_e_dokumente` wie bisher; Prop-Drehung pc_prop_rot_q12 fuer Vielfache von 1024; Zusammensetzung
  `re15_camera_compose_view_bone`; `sicht` + RTPS + UNR-Tabelle `unr[i] = max(0,(0x40000/(i+0x100)+1)/2-0x101)` wie
  `re15_math.c`) trifft alle vier TRILOG-Vierecke Ecke fuer Ecke. Verlauf ueber s (Park, Mitte unveraendert):
  2 x 1 fuer s 0,7206..0,8941, 2 x 2 fuer 0,8941..1,2147, 3 x 2 fuer 1,2147..1,2207, 4 x 2 ab 1,2207 — ein 3 x 1-Viereck
  gibt es an dieser Stelle nicht. Prison: 2 x 2 fuer 0,8135..1,1223, **3 x 2 fuer 1,1223..1,3286**, 4 x 2 ab 1,3286
  (Abtastschritt dieses Verlaufs 0,0005 x s_L; die Bisektion des Werkzeugs auf GLEICHE Ecken: Park 0,7195..0,8931,
  Prison 1,1338..1,2809, s. Umsetzung).

## Umsetzung (Endstand, Commit f21c2ee8 + Abschluss)

Nur Werkzeug + erzeugte Dateien, KEIN Engine-Code (das MD1 traegt den Massstab, `schluessel_r36.c:359` liefert es wie
bisher). Keine gemeinsame Datei angefasst, keine Bank-9-Bits, keine Nachrichten-Ids, kein Asset.

* `re15_port/tools/r38_itemorte/itemorte_export.py` (Runde-38/39-Werkzeug, Kopf um "RUNDE 39 SPUR M" erweitert):
  - `NUTZER_FAKTOR_M = 0.5` — NUTZER-VORGABE Nachtrag 5 ("maximal halb so gross"), 0,5 x RE2-Quartil-Groesse (Stand Spur
    L), linear in Breite und Hoehe; ausdruecklich KEIN RE-Wert.
  - Zwei Grenzen, je Breite UND Hoehe <= 0,5 x Spur L: (1) stetige gezeichnete Huelle (Raster 48/480 wie bisher);
    (2) GTE-Viereck = Ausdehnung der GANZZAHLIGEN Bildschirm-Ecken aller MD1-Ecken, nachgerechnet Ganzzahl fuer
    Ganzzahl wie der Port-Prop-Zeichner (`gte_ecken`: pc_prop_rot_q12 fuer Vielfache von 1024 — `re15_trig_lut` dort
    selbst ausgelesen 0/+-4096 —, `re15_camera_compose_view_bone` Q12 mit einem `>> 12`, `objpoly_r38.c` `sicht` + RTPS
    mit IR-/SZ3-Saettigung, `gte_divide` = `re15_gte_divide` mit UNR-Tabelle `unr[i] = max(0,(0x40000/(i+0x100)+1)/2
    - 0x101)`, Bildmitte 160/120 = `const int cx = 160, cy = 120` main.c:2077 = Original SetGeomOffset @0x8001643c
    `ori a0,zero,0xa0` / @0x80016440 `jal 0x80066d60` / @0x80016444 `ori a1,zero,0x78`, selbst disassembliert). Original:
    FUN_80022f0c RTPT @0x80022fb0, SXY ins Polygon @0x80022fd8..e0.
  - `s_oben` = groesster Massstab, der beide Grenzen haelt (Bisektion 40 Schritte, 0,1 x s_L .. 0,6 x s_L);
    `s_unten` = kleinster Massstab mit DENSELBEN ganzzahligen Ecken (Bisektion); geliefert `s = (s_unten + s_oben)/2` —
    jeder Massstab im Intervall zeichnet dasselbe Bild (gleiche ganzzahlige Ecken, gleiche UV), die Mitte haelt Abstand zu
    den Sprungstellen. Lage, Drehung, flaches Licht, Aufhebe-Zone: unveraendert (Riegel `unit_r39_schluessel2_konst`).
  - Werkzeug-Ausgabe (Endstand, `python re15_port/tools/r38_itemorte/itemorte_export.py`):
    ```
    park:   Runde 39 M: NUTZER-VORGABE Faktor 0.50; stetige Grenze 2.230 x 1.493 px (Spur L 4.461 x 2.985), GTE-Viereck
            Spur L x 135..141 y 79..82 = 6 x 3 -> Grenze 3.0 x 1.5
            -> s_oben 0.8931 (darueber bindet GTE-Raster: GTE 2 x 2, Huelle 1.653 x 1.110), s_unten 0.7195 -> s 0.8063
            (33.5 % von Spur L): GTE-Ecken [(137,81),(137,80),(139,81),(139,80),(138,81)] = x 137..139 y 80..81 (2 x 1),
            gezeichnete Huelle 1.483 x 0.995 px (33.2 % / 33.3 %), sichtbar 92 x 58 Einheiten
    prison: ... GTE-Viereck Spur L x 135..141 y 79..83 = 6 x 4 -> Grenze 3.0 x 2.0
            -> s_oben 1.2809 (darueber bindet stetig: GTE 3 x 2, Huelle 2.891 x 1.429), s_unten 1.1338 -> s 1.2073
            (47.1 % von Spur L): GTE-Ecken [(139,80),(139,82),(136,80),(136,82),(138,81)] = x 136..139 y 80..82 (3 x 2),
            gezeichnete Huelle 2.716 x 1.338 px (47.1 % / 46.6 %), sichtbar 168 x 79 Einheiten
    ```
    Warum der Parking Lot Key bei 1/3 landet: an seiner Stelle gibt es zwischen 2 x 1 und 2 x 2 kein 3 x 1-Viereck (Verlauf
    oben); 2 x 2 hat die Hoehe 2 > 1,5 = halbe Spur-L-Hoehe 3. Der Prison Key trifft mit 3 x 2 genau die Haelfte von 6 x 4.
  - `--schreiben` erzeugt `gen/itemorte_r38_lage.inc` (RE15_R38K_PARK_S10000 24100 -> **8063**, RE15_R38K_PRISON_S10000
    25623 -> **12073**; neu `RE15_R39M_FAKTOR100 50`, je Schluessel `RE15_R39M_*_{SL_10000, L_BREITE/L_HOEHE/BREITE/HOEHE_PX1000,
    L_GTE_X0/Y0/B/H, GTE_X0/Y0/B/H, S_UNTEN_10000, S_OBEN_10000, MX100000, MZ100000}`; `RE15_R39L_*` beschreiben weiter den
    Spur-L-Stand) und `gen/itemorte_r38_md1.inc` (die zwei MD1, nur die Ecken @0x44). Zwei Laeufe des Endstands -> md5
    gleich (lage c289f40a..., md1 bb8c307a...; `git diff` leer): Fixpunkt, der Generator liest seine Ausgabe nicht.

## Messung nachher (Endstand, exe 4e0ead5b = `re15_pc_r39m_nach2.exe`, dieselben Laeufe wie "vorher")

| Lauf | Pixel (vorher) | Huelle B x H (vorher) | Sum|dL| (vorher) | Anteil B / H | Grenze "halb" |
|---|---|---|---|---|---|
| Leon Parking Lot Key, Skala 3 (960x720) | 9 (69) | x 413..416 y 241..243 = **4 x 3** (14 x 8) | 403 (3731) | 29 % / 38 % | <= 7 x 4: **ja** |
| Leon, Skala 4 (1280x960) | 13 (125) | x 550..555 y 322..324 = 6 x 3 (18 x 11) | 571 (6311) | 33 % / 27 % | <= 9 x 5: ja |
| Leon, Skala 1 (320x240) | 0 (7) | - (5 x 2) | 0 (506) | - | (unsichtbar) |
| Elza Prison Key, Skala 3 | 16 (63) | x 410..417 y 242..246 = **8 x 5** (16 x 10) | 748 (3960) | 50 % / 50 % | <= 8 x 5: **ja** |
| Elza, Skala 4 | 28 (118) | x 547..556 y 322..328 = 10 x 7 (22 x 14) | 1215 (5524) | 45 % / 50 % | <= 11 x 7: ja |
| Elza, Skala 1 | 0 (5) | - (4 x 2) | 0 (317) | - | (unsichtbar) |

AUSSEN je 0; Elza-Laeufe `Spieler-Familie PL04`. ECHTER Lade-Weg (Spielstand + CONTINUE, `mess_continue.sh`, Bild 100,
`CONTINUE: resumed in room 10f0`/`10f1`): Skala 3 Leon 9 / x 413..416 y 241..243 / 403, Elza 16 / x 410..417 y 242..246 /
748; Skala 4 Leon 13 / 6 x 3 / 571, Elza 28 / 10 x 7 / 1215 — bitgleich zum Sprung-Weg. Szenen der Runde-38-Abnahme
(`mess_r38.sh`, CONTINUE, Skala 1): Cut 5 Park 0 (vorher 7 / 506), Prison 0 (vorher 5 / 317); Cut 6 (zweite Kamera auf
die Pinnwand) Park 2 Pixel / 111 (vorher 11 / 458), Prison 3 / 94 (vorher 13 / 646).
RE15_TRILOG des Endstands (Riegel `integration_r39_schluessel2` R): Park-Ecken (137,80) (139,80) (139,81) (137,81) Mitte
(138,81) = 2 x 1, Prison (136,80) (139,80) (139,82) (136,82) Mitte (138,81) = 3 x 2 — wie das Werkzeug und
`unit_r39_schluessel2_raster` (Engine-Funktionen).

Augenschein (`M_bilder/m_s3_s4_vorher_stetig_endstand_x8.png`: Spalten Spur L | nur stetige Regel | Endstand, Zeilen Leon
Skala 3/4, Elza Skala 3/4; `M_bilder/m_s3_pinnwand_vorher_endstand_x2.png`: die Pinnwand bei 960x720, vorher | nachher):
der Parking Lot Key ist ein kleiner heller Schaft mit dunklem Kopfende in der richtigen Lage (die stetige Regel hatte ihn
zu einem senkrechten Strich gemacht), der Prison Key ein kleiner Ring mit Schaft — beide deutlich kleiner als die Zettel.
Bei 320x240 (Skala 1) sind die Bilder mit und ohne Schluessel BITGLEICH (groesste Abweichung im Fenster 0, aussen 0;
GTE-Vierecke 2 x 1 / 3 x 2 Bildpunkte): der Schluessel setzt dort keinen Bildpunkt mehr (vorher 7 / 5 Pixel).

Skala 2 (640x480, nachgemessen fuer kleine Bildschirme; Sprung-Weg, CONTINUE bitgleich): Leon vorher 36 Pixel / x 272..280
y 159..164 (9 x 6) / 1942 -> nachher 5 / x 275..277 y 161..162 (**3 x 2**) / 331 (33 % / 33 %); Elza vorher 26 / x 272..282
y 159..165 (11 x 7) / 1500 -> nachher 9 / x 273..278 y 161..163 (**6 x 3**) / 504 (55 % / 43 %). Die Prison-Breite liegt bei
640x480 einen Bildpunkt ueber der Haelfte, obwohl das GTE-Viereck genau halb ist (3 statt 6 Bildpunkte im 320er Raster):
bei Spur L zeigten 11 der 12 Fensterspalten des Vierecks einen Unterschied > 30, jetzt alle 6. Bei 960x720 (Spielgroesse
des Nutzers; Abnahme-Massstab der Spur L und dieses Auftrags) 50 % / 50 %, bei 1280x960 45 % / 50 %.

**Aufnahme (echter Lade-Weg, `mess_aufnahme.sh`, Spielstand vor der Pinnwand (1300,-2900) Blick -z, Aktion -> Modal -> Ja,
dritter Druck an derselben Stelle):** Leon `CONTINUE: resumed in room 10f0`, `Spieler-Familie PL00`,
`[schluessel] PARKING LOT KEY genommen: Item 0x45, Bit (9,82)`, danach `[msgtext] room=10f0 id=4 "Several files from past
cases.Nothing useful here."`; Elza `resumed in room 10f1`, `PL04`, `[schluessel] PRISON KEY genommen: Item 0x42, Bit (9,83)`,
`[schluessel] Prison Key genommen -> Flag (4,0xEC) = 1 (Zelle ROOM1211)`, danach msg 4. Dazu gruen: `integration_r38_itemorte`
K1/K2 (CONTINUE), `integration_r39_optik_schluessel` A1/A2 (Sprung), `integration_r36_schluessel` (s.u.).

Endstand-exe nach dem letzten Bau (nur Kommentare der erzeugten Dateien geaendert, Commit d080c50d): `.text` und `.data`
md5-gleich zur gemessenen `re15_pc_r39m_nach2.exe` (02595202... / 58e1af2f...); `.rdata` unterscheidet sich nur im
Bau-Zeitstempel des Befund-Log-Kopfs ("20:25:37" statt "20:00:23"), dazu PE-Zeitstempel/Pruefsumme.

## Tests (NEU registriert nur in `tests/unit/probes/r39_schluessel2.cmake`; keine CMakeLists angefasst)

Neu (Mechanik je Grenze, plus echte exe):
* `unit_r39_schluessel2_halb` — (1) am gelieferten Netz: MD1 + Prop-Lage/-Drehung durch die Engine-Kamera Cut 5, jeder
  sichtbare Texel abgetastet; Stand Spur L aus dem RE2-Modell (info/re2leon) mit den Werkzeug-Ecken `rint((v - Mitte) x s)`.
  Gemessen: Park 1,468 x 0,976 gegen 4,416 x 2,926 px (33,2 % / 33,4 %), Prison 2,706 x 1,327 gegen 5,748 x 2,844 px
  (47,1 % / 46,6 %); Mitte unveraendert; 2221 / 1565 Abtastpunkte wie Spur L; geliefertes MD1 == RE2-Modell mit genau diesen
  Ecken (alle anderen Bytes gleich).
* `unit_r39_schluessel2_raster` — (2) mit den ECHTEN Engine-Funktionen (`re15_camera_build_view`, `re15_camera_compose_view_bone`,
  `re15_objpoly_r38_tri_mesh`, Drehung wie `pc_prop_rot_q12`): Spur L 6 x 3 / 6 x 4, geliefert 2 x 1 (137,80) / 3 x 2 (136,80)
  == Werkzeug; knapp ueber S_OBEN Park 2 x 2 (GTE verletzt), Prison 3 x 2 mit Huelle ueber der stetigen Grenze; an beiden
  Intervall-Enden dieselben ganzzahligen Ecken.
* `unit_r39_schluessel2_konst` — Faktor 50 (NUTZER-VORGABE), Huellen/Vierecke <= 0,5 x Spur L, Basis == Spur-L-Abnahme
  (24100 / 25623, RE15_R39L_*), Lage (1358,-2495,-3400), Drehung, Zone (798,-3600) 1000x1000, Slot 8, Licht 0x0010.
* `integration_r39_schluessel2` — ECHTER Lade-Weg (Spielstand + CONTINUE), Skala 3: RE15_TRILOG-Ecken == Kopf (R), Huelle
  <= 7 x 4 / 8 x 5 und sichtbar (Pixel/Sum|dL| >= 5/240 bzw. 9/450), AUSSEN 0 (H/S). Messzeilen (Endstand, `ctest -V`):
  `r39_schluessel2[R_park]: GTE-Viereck x 137..139 y 80..81 (2 x 1), Soll (137,80) 2 x 1`,
  `r39_schluessel2[R_prison]: GTE-Viereck x 136..139 y 80..82 (3 x 2), Soll (136,80) 3 x 2`,
  `ok   H_leon: Huelle 4 x 3 <= 7 x 4, 9 Pixel / Sum|dL| 403 >= 5 / 240, aussen 0`,
  `ok   H_elza: Huelle 8 x 5 <= 8 x 5, 16 Pixel / Sum|dL| 748 >= 9 / 450, aussen 0`.

Nachgezogen (gewollte Verhaltensaenderung = NUTZER-VORGABE Nachtrag 5, Begruendung im Kopf jeder Datei):
* `unit_r38_itemorte_lage` (`tests/unit/test_r38_itemorte.c`): "Huelle == RE2-Quartil +-0,5" -> "Huelle == Spur-M-Huelle
  +-0,1 und <= 0,5 x Spur L + 0,05"; "Massstab > 1" -> "Massstab <= 0,5 x Spur-L-Massstab" (der Parking Lot Key liegt mit
  0,8063 unter der RE2-Weltgroesse, weil erst das 2 x 1-Viereck die Hoehe halbiert).
* `unit_r39_optik_schluessel` (`tests/unit/test_r39_optik.c`): die Massstab-Pruefung der Spur L liest den Spur-L-Massstab
  `RE15_R39M_*_SL_10000` (der gelieferte ist seitdem hoechstens 0,5 x davon); RE2-Quartil-Nachrechnung und MD1-Spanne/s = 152 unveraendert.
* `integration_r39_optik_schluessel` (`tests/integration/test_r39_optik_schluessel.cmake`): Skala 3 Breite <= 7 / 8 (halbe
  Spur-L-Messung), Mindestpixel 5 / 9 (~60 % der neuen Messung 9 / 16); Skala 1 Mindestzahl 0 (gemessen 0 Pixel), das
  Auswerte-Muster laesst die leere Huelle "x320..-1" zu. Aufnahme A1/A2 unveraendert.
* `integration_r38_itemorte` (`tests/integration/test_r38_itemorte.cmake`): B2/B3 (Skala 1) nur noch AUSSEN == 0 (0 Pixel
  gemessen), B3V (Prison/Park bei Skala 1, jetzt 0/0) entfaellt, B4V bleibt (jetzt 186 %); B4/B4K 36/2300 -> 9/450, B5K
  40/2200 -> 5/240, V3 6 -> 1 (gemessen 2), V5 7 -> 1 (gemessen 3). Aufnahme K1/K2 unveraendert.

### Gezielter Lauf (KEINE volle Suite im Spurbaum), `ctest -j 2 --timeout 1200 --output-on-failure`, -R woertlich:
`^(unit_r39_schluessel2_.*|integration_r39_schluessel2|unit_r39_optik_schluessel|integration_r39_optik_schluessel|unit_r38_itemorte_.*|integration_r38_itemorte|unit_r36_schluessel_.*|integration_r36_schluessel.*|unit_r38_cursor_.*|integration_r38_cursor|unit_r35_cut10f0_.*|integration_r35_cut10f0|unit_r22_10f0_figur|unit_r36_klettern_10f0_.*|unit_r37_feuerloescher_.*|integration_r37_feuerloescher|integration_item_name_census)$`

Ergebnis (Stand f21c2ee8 = Endstand ohne die Kommentar-Commits, `ctest_ziel1.log`): **`100% tests passed, 0 tests failed out
of 45`** (ctest exit 0, 777 s; u.a. integration_r38_itemorte 306,6 s, integration_r39_optik_schluessel 221,5 s,
integration_r37_feuerloescher 334,7 s, integration_r35_cut10f0 252,7 s, integration_r38_cursor 176,8 s (E/F: Schluessel-
Vorderseite weiter gezeichnet), integration_r36_schluessel 110,8 s, integration_r36_schluessel_befreiung 79,2 s,
integration_r39_schluessel2 40,2 s). Gruppen nach `git diff master --name-only` + `git grep -l` auf 10F0/Schluessel/
itemorte in re15_port/tests (gen/itemorte_r38_*.inc -> r38_itemorte, r39_optik, r36_schluessel, r38_cursor, r37_feuerloescher;
Raum 10F0/10F1 -> r35_cut10f0, r22_10f0, r36_klettern_10f0; Item-Namen -> item_name_census).
Nach dem letzten Bau (Kommentare): `ctest -V -R "^(unit_r39_schluessel2_.*|integration_r39_schluessel2)$"` ->
**`100% tests passed, 0 tests failed out of 4`** (40,6 s).

Querbezug (Memory reai-v2-gezielte-integration-querbezug): `ctest -j 6 --timeout 600 --output-on-failure -R "^(unit_|prop_|probe_)"`
im Endstand -> **`100% tests passed, 0 tests failed out of 825`** (1214 s; die langen sind die Release-/Android-Skript-Riegel
unit_r35_android_bash_mutanten 1056 s, unit_r38_releaseskript_* 421..955 s).

## OFFEN
* Keine offenen Teilpunkte des Nutzer-Punkts.
* Integration: `tools/r38_itemorte/itemorte_export.py` + `gen/itemorte_r38_*.inc` (Runde 38 K / Spur L) und die Riegel
  `test_r38_itemorte.c/.cmake`, `test_r39_optik.c`, `test_r39_optik_schluessel.cmake` sind gewollt geaendert — bei Konflikten
  diese Fassung nehmen (Begruendung oben). Spur I (funk10f1, Szene in ROOM10F1): falls ihre Riegel Bilder der Pinnwand
  ROOM10F1 vergleichen, ist der Prison Key dort jetzt kleiner (GTE-Viereck 3 x 2 statt 6 x 4).
* PSX-Ziel nicht gebaut (kein PSn00bSDK auf dieser Maschine) — keine Code-Aenderung, nur die erzeugten Daten.
* Volle Suite: faehrt der Orchestrator im Integrationsbaum.

## Fuer den Nutzer
* **Schluessel im Communications Room (ROOM10F0 Leon / ROOM10F1 Elza), "maximal halb so gross":** beide haengen an
  derselben Stelle der Pinnwand, gleich gedreht, gleiche Aufhebe-Stelle — nur kleiner. Bei 960x720 (so spielst du):
  - Prison Key (Elza): genau halb so gross — 16 x 10 -> 8 x 5 Pixel (Ring + Schaft weiter zu erkennen).
  - Parking Lot Key (Leon): 14 x 8 -> 4 x 3 Pixel, also etwa ein Drittel. Der Grund ist die PlayStation-Technik: so kleine
    Objekte zeichnet das Spiel mit Ecken auf ganzen Bildpunkten des 320x240-Bildes. Vorher war der Schluessel dort 6 x 3
    Bildpunkte gross; die naechste moegliche Stufe ueber dem jetzigen 2 x 1 waere 2 x 2 gewesen — Hoehe 2 von vorher 3,
    also zwei Drittel statt hoechstens halb, und der Kopf stand dann als senkrechter Strich da (gemessen 4 x 6 Pixel).
    2 x 1 ist die groesste Stufe, die hoechstens halb so gross ist.
* In einem 320x240-Fenster (Fensterskala 1) ist von den Schluesseln nichts mehr zu sehen: ihre Vierecke sind dort nur
  noch 2 x 1 bzw. 3 x 2 Bildpunkte gross, die Bilder mit und ohne Schluessel sind dort bitgleich (vorher 7 bzw. 5
  Bildpunkte Unterschied). Bei 640x480 (Fensterskala 2) sind sie zu sehen (5 bzw. 9 Pixel); dort ist der Prison Key 6 statt
  11 Pixel breit — einen Bildpunkt mehr als die Haelfte, weil das Raster bei dieser Groesse keine halben Bildpunkte kennt.
  Das Aufheben an der Pinnwand funktioniert unveraendert (Leon und Elza gemessen).
* Hebel, falls sie dir jetzt zu klein sind: die eine Zahl `NUTZER_FAKTOR_M` (0,5) in
  `re15_port/tools/r38_itemorte/itemorte_export.py` (Werkzeug neu laufen lassen mit `--schreiben`); der Parking Lot Key
  bekommt die naechste Stufe (2 x 2) erst ab einem Faktor von 2/3.
* Neue Assets fuer das Paket-Gate: keine (nur Werkzeug + erzeugte `gen/itemorte_r38_*.inc`). Sprachdateien: keine.
