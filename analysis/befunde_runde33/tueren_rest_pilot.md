# Runde 33 / Thema T — Pilot (Stufe 1)

Zweig `r33/tueren`. Plan: `tueren_rest_plan.md`. Stand 2026-09-29, laufend fortgeschrieben.

## 0. Umfang

| Gruppe | Port-Archiv | Tueren | Seiten | Basis (Bewegung, Ton, Griff) |
|---|---|---|---|---|
| G1 glatte dunkle STAGE1-Stahltuer (groesste Gruppe, VOLLSTAENDIG) | P07G | 12: T000 T001 T002 T003 T004 T006 T012 T013 T015 T016 T021 T022 | 26 | DOOR07, Ton F3, Druecker flach |
| G4 glatte Stahl-Doppeltuer (Pilot T054, dasselbe Archiv traegt T026) | P1DG | 2: T026 T054 | 4 | DOOR1D V2/V3, eigener Blechton, Druecker flach |
| G6 Messingleiter ROOM1260 (Pilot T053, dasselbe Archiv traegt T023 T052) | P16M | 3: T023 T052 T053 | 6 | DOOR16 V4/V5, eigener Ton (4 Sprossen) |
| **Summe** | 3 | **17 Tueren** | **36 Seiten (72 Tabellenzeilen xxx0/xxx1)** | |

Nach dem Pilot spielen **100 von 144** physischen Tueren eine Tuersequenz (83 Runde 31 + 17).

## 1. Archive (gemessen, `analysis/befunde_runde33/tueren_rest/archive.json`)

| Kennung | Basis | Datei | FNV-1a | gemalt Mittel RGB (Mitte v 80..140) | Texel Mittel RGB | Palette | gekappt |
|---|---|---|---|---|---|---|---|
| P07G | DOOR07 | 55 908 B | 0x9155935A | 23,3/34,4/36,2 (21,5/33,2/37,4) | 40,9/60,3/63,5 | 111, verlustfrei | 0 |
| P1DG | DOOR1D | 60 676 B | 0xB736A098 | 37,2/42,3/41,7 (35,0/40,0/39,2) | 65,1/74,2/73,0 | 57, verlustfrei | 0 |
| P16M | DOOR16 | 58 860 B | 0xF3556E1C | Messing R/L 1,1125 G/L 1,020 B/L 0,5815 | L 48,3 (wie RE2) | 29, verlustfrei | 0 |

- **P07G**: Blatt = DOOR07-Blech: Lueftungsschlitze (u 26..100, v 164..206, Zeilenprofil + Gitterbild
  `build/r33_tueren/d07_grid.png`) und der Pfeil-Umriss des Druecker-Schilds (u 0..20, v 94..122) im KORN
  durch glattes Blech 46 bzw. 40 Zeilen daneben ersetzt; Korn = L/Tiefpass(L, sigma 1,5) - 1 (Flecken > 3
  Texel fallen heraus), halb so stark (PORT-WAHL "glattes Blatt", Merkmal Runde 31: "glatter CG-Lack");
  Farbe = Zeilenprofil x Spaltenfaktor des Medians ueber 19 entzerrte RE1.5-Blaetter (Liste in archive.json),
  Texel = gemalt * 128 / 73. Druecker-Mesh (Mesh 1) und seine Texel (v 238..254) bytegleich DOOR07.
  Das Rechteckschild unter dem Druecker ist NICHT gemalt: in den 30..95-px-Ausschnitten nicht aufloesbar.
  **Druecker silbern** (Kontaktbogen 1: der RE2-Druecker war ein dunkler brauner Strich, RE1.5 malt ihn
  silbern): gemessen in 18 Ausschnitten (Griffbox u 0..40 v 90..150, Punkte > Blatt + 30, Median je
  Ausschnitt, Median) = [72.0, 79.5, 80.0]; Texel der Druecker-Dreiecke (Mesh 1, 983 Texel) = Helligkeitsverlauf des
  RE2-Druckers um das Mittel gemalt*128/73 (52.2 -> 135.6), Farbton des gemalten Griffs, Kontrast 2.423 (so, dass
  kein Kanal ueber 255 geht; 0 gekappt).
- **P1DG**: dasselbe Blech-Korn (DOOR07), Farbe = Zeilenprofil der Fluegelflaechen aus 5 Ausschnitten von
  T026/T054 (S045 c08/c09, S049 c00, S104 c00/c01; S100 trifft nur die Kachelwand); DOOR1D-Lueftungsgitter,
  Griffkasten und Sockel entfallen (RE1.5: "ohne Felder, ohne Fenster"). Druecker-Texel bytegleich DOOR1D.
  **Griff-Tausch P1DG <- P07G** (Kontaktbogen 2: der DOOR1D-Druecker steht in Ruhe um x -780 gekippt -
  Simulator: Objekt 2/4 rot x 64756 - fuer seinen gemalten Griffkasten; RE1.5 malt zwei WAAGERECHTE Druecker an
  der Fuge): Mesh + Grund-Drehung des DOOR07-Drueckers (Formfamilie "Druecker flach", DOOR07.m1 = DOOR1D.m1
  bytegleich), Textur aus P07G (silbern), am 1D-Anhaengepunkt (130,-2850,-3410), Bewegung des 1D-Griffs mit
  Ausschlag -245 -> -702 ([SIM] tuer_zuordnung_gen.griff_daten, gen/re15_tuer_eigen.inc
  re15_griff_tausche_eigen). Gemalte G4-Grifffarbe zum Vergleich [96.0, 101.0, 100.0] (P07G-Druecker gemalt [72.0, 79.5, 80.0]).
- **P16M**: Helligkeit je Texel = RE2-Leiter, Farbton = gemalte Messingleiter (ROOM12607.bmp x 126..141
  y 15..149: 1,109/1,021/0,596; ROOM12609.bmp x 203..225 y 0..149: 1,116/1,019/0,567; Leiterpunkte R > B+12,
  L > 30). Nur die 5 252 Texel, die ein Leiter-Dreieck liest.

## 2. Riegel (probe_r33_tueren, `tests/unit/probes/r33_tueren.cmake`)

`unit_r33_tueren_archive` (Ausgabe):
```
P07G: Basis DOOR07, 55908 B, Tonteil+Kopf+SCD+MD1 bytegleich, TIM 8 bit 128x256 CLUT 256, 33175 TIM-Bytes neu, FNV 885641AC
  P07G V1: 301 Bilder (Basis 301), 1 Se_on, Schliesston 1, Notizen 0x0, md1 1 tim 1 -> gleich
  P07G V0: 301 Bilder (Basis 301), 1 Se_on, Schliesston 1, Notizen 0x0, md1 1 tim 1 -> gleich
P1DG: Basis DOOR1D, 60676 B, Tonteil+Kopf+SCD+MD1 bytegleich, TIM 8 bit 128x256 CLUT 256, 32777 TIM-Bytes neu, FNV B736A098
  P1DG V2: 291 Bilder (Basis 291), 1 Se_on, Schliesston 1, Notizen 0x0, md1 1 tim 1 -> gleich
  P1DG V3: 311 Bilder (Basis 311), 1 Se_on, Schliesston 1, Notizen 0x0, md1 1 tim 1 -> gleich
P16M: Basis DOOR16, 58860 B, Tonteil+Kopf+SCD+MD1 bytegleich, TIM 8 bit 128x256 CLUT 256, 32521 TIM-Bytes neu, FNV F3556E1C
  P16M V5: 331 Bilder (Basis 331), 4 Se_on, Schliesston 0, Notizen 0x0, md1 1 tim 1 -> gleich
  P16M V4: 331 Bilder (Basis 331), 4 Se_on, Schliesston 0, Notizen 0x0, md1 1 tim 1 -> gleich
Port-Archive: 3, Varianten gegen das Basis-Archiv: 6
```
("TIM-Bytes neu" = fast alle, weil die Palette neu sortiert ist.)

`unit_r33_tueren_zuordnung`:
```
Port-Zeilen: 72, 72 treffen ihren Satz, 36 Tuerseiten, 17 Tueren
ROOM1000 Slot 0 (S000): Standplatz 1, Raumwechsel 1 -> ROOM1050, Laeufer 1 x: DOOR07 V1 eigen 1 (P07G)
```
`unit_r31_zuordnung` (angepasst): `Tabelle: 368 Zeilen, 368 treffen ihren Satz, 184 Tuerseiten ...`,
`ROOM1050 Slot 5 (nicht abgedeckt): Standplatz 1, Raumwechsel 1 -> ROOM1090, Laeufer 0 x`.

## 3. Kontaktbogen (echte exe, RE15_TUER_SEITE + RE15_TUER_BOGEN)

`python re15_port/tools/tueren/tuer_rest_bogen.py --erzeugen`: Kopie `re15_pc_r33t.exe` (eigener Name),
`RE15_TUER_SEITE=<36 Seiten> RE15_TUER_BOGEN=... RE15_TUER_SCHNELL=1 RE15_NOAUDIO=1`, beschleunigter Renderer,
Rueckleser vor dem Present (kein AUTOSHOT/SOFTWARE_RENDER). debug.log: 36 x "Sequenz fertig", 36 x
"Port-Archiv P07G/P1DG/P16M", 4 x "Griff-Tausch DOOR1D <- DOOR07: Spender-Griff geladen", Notizen 0.
Boegen (verkleinert): `tueren_rest_belege/pilot_kontaktbogen_01..03.jpg` (je Seite RE1.5-Ausschnitt | Anfang |
Mitte + Wahl), gross `pilot_gross_g1_g4_g6.jpg` (S001, S002, S104, S131), `pilot_g4_doppeltuer_griff_tausch.jpg`,
Texturen vorher/nachher `pilot_texturen_vorher_nachher.jpg`.

**Angesehen (alle 36 Seiten):**
- G1 (26 Seiten): glattes dunkles schiefer-/petrolfarbenes Blatt ohne Lueftungsschlitze/Flecken, oben heller
  (Glanz wie die gemalten Blaetter), silberner Druecker. Griffseite im Anfangsbild = gemalte Seite bei allen 26
  (V1 rechts, V0 links); V0 geht weg, V1 kommt hin. Die Treppenhausseiten S023/S024/S025 sind im RE1.5-Bild
  graugruen (Leuchtstofflicht), ihre Gegenseiten (S056/S039/S015) dunkel - ein Archiv je Tuer (RE2 238/238),
  also dunkel wie der Median.
- G4 (4 Seiten): zwei glatte graue Fluegel mit Fuge, zwei waagerechte silberne Druecker an der Fuge wie gemalt
  (nach dem Griff-Tausch; vorher steil gekippt, Bogen des ersten Laufs); V2 rechter Fluegel geht weg, V3 linker
  kommt hin. S100 zeigt im RE1.5-Ausschnitt nur die Kachelwand (Umriss daneben, Runde 31) - kein Gegenbeleg.
- G6 (6 Seiten): Messingleiter, 4 Sprossen-Takte; V4 (S128/S130/S131 hinauf) und V5 (Schaechte S038/S095/S099
  hinab) wie DOOR16.
- Ton im Bogenlauf aus (RE15_NOAUDIO); die Toene sind die bytegleichen Tonteile der Basis-Archive
  (probe_r33_tueren "archive": Tonteil bytegleich, Se_on-Bilder gleich).

## 4. Helligkeit (gezeigt gegen gemalt) - nicht doppelt abgedunkelt

`python re15_port/tools/tueren/tuer_rest_helligkeit.py <serienbild> <Kennung> <Variante> <Bild>`: Serienbild
der echten exe (RE15_TUER_SERIE, 960x720), Modell des Blatts (Objekt 0) im selben Bild aus dem Katalog-Simulator
(Geometrie/Bewegung = Basis-Archiv), je Innenpunkt F = gezeigt, T = Texel, P = gemaltes RE1.5-Feld an DIESEM
Texel (dasselbe Feld wie im Generator):

| Seite | Archiv | Bild | Punkte | c Mittel | F/T | Modell PSX gegen F | **F/P** | F RGB | P RGB |
|---|---|---|---|---|---|---|---|---|---|
| S001 | P07G V1 | 60 / 90 | 262 643 | 73,0 | 0,572 | 1,15 / 1,14 Stufen | **1,012** | 25,7/37,2/39,2 | 25,3/36,8/38,9 |
| S002 | P07G V0 | 60 / 90 | 262 643 | 73,0 | 0,572 | 1,16 / 1,13 | **1,012** | 25,7/37,1/39,2 | 25,3/36,8/38,9 |
| S045 | P1DG V2 | 60 / 100 | 262 643 | 73,0 | 0,571 | 1,51 / 1,47 | **1,008 / 1,007** | 39,7/45,1/44,5 | 39,4/44,8/44,1 |

F/T = c/128 = 0,570 (das RE2-Tuerlicht wirkt einmal, wie berechnet), F/P = 1,01: das Blatt erscheint so hell
wie das gemalte RE1.5-Blatt an denselben Stellen - keine doppelte Abdunklung. Rest-Abweichung zum Modell
(1,1..1,5 Stufen) = 5-Bit-Rundung + Korn.

Leiter P16M (Helligkeit = RE2-Leiter, s. 1): mit derselben Auswahl (Messingpunkte R > B + 12, L > 30) gezeigt
Median L 45,8..51,7 (S131 Anfang/Mitte, S099 Mitte), gemalt ROOM12607/12609 46,2 / 44,8 -> F/P 1,0..1,15; nicht
abgedunkelt.

## 5. Suite

(folgt)
