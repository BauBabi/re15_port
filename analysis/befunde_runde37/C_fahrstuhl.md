# Runde 37 — Spur C "fahrstuhl" (Dossier)

Baum: `.claude/worktrees/r37_fahrstuhl`, Zweig `r37/fahrstuhl`, Basis master f1e019ca. Stand: IN ARBEIT (wird
fortlaufend geschrieben und nach jedem Schritt committet).

## Auftrag (Wortlaut des Nutzers, AUFTRAG.md #6 / #7)

1. "Die Fahrstuhl Tür in der Tür Animation ist noch nicht korrekt. DA musst du noch mal schauen ... wenn es keine
   passende Fahrstuhl Tür gibt, müssen wir die wieder bauen...."
2. "Beim Aussteigen aus dem Fahrstuhl fehlt noch die Türsequenz. << stimmt."

Ausgangslage (Runde 36 Spur G, `analysis/befunde_runde36/G_tueren.md`, `G_abnahme_0.md` H1/H3/X1): Einstieg in die
Kabine ROOM1080 (Seiten S013/S040/S057) spielt RE2 DOOR27 V0 (RE2-Labor-Fahrstuhl A-2, oliv-grau, Bedienfeld);
Ausstieg (ROOM1080 -> Etage) hat KEINE Sequenz.

## Messung vorher (Stand f1e019ca, eigener Bau im Baum, Messkopie `re15_pc_r37c_mess.exe`)

Messordner `build/r37_fahrstuhl/` (nicht eingecheckt), Belegbilder `analysis/befunde_runde37/C_belege/`.

### V1 Einstieg: welche Tuer die Sequenz zeigt
`RE15_TUER_SEITE=S013,S040,S057,S215,S260,S290 RE15_TUER_BOGEN=... RE15_TUER_SCHNELL=1`:
```
[tuer-seite] S013 ROOM1040 DOOR27 V0 Spender FF      [tuer-seite] S215 ROOM4000 DOOR27 V0 Spender FF
[tuer-seite] S040 ROOM10C0 DOOR27 V0 Spender FF      [tuer-seite] S260 ROOM5000 DOOR27 V0 Spender FF
[tuer-seite] S057 ROOM1120 DOOR27 V0 Spender FF      [tuer-seite] S290 ROOM50C0 DOOR27 V0 Spender FF
[tuer] Sequenz fertig: 271 Bilder + 1 Warten, 0 Se_on, Schliesston 1 (je Seite)
```
Bild `C_belege/c0_vorher_door27_s040_s215.jpg`, Serie `c0_vorher_door27_s040_serie.jpg` (jedes 24. Bild):
RE2-Labor-Fahrstuhl, oliv-grau, Fluegelkanten VERZAHNT (Zaehne im Netz, nicht in der Textur: DOOR27 Mesh 0
102 Ecken / 180 Dreiecke z 0..3236, Mesh 1 142 / 259 z -3186..0 - die Fluegel ueberlappen in der Mitte um 422),
Bedienkasten, Warnschilder. Bild 70 Fluegel auf, Kamera faehrt durch, ab ~200 Abblende.

### V2 Was RE1.5 malt (Soll der Gestalt), `C_belege/c0_gemalt_revier_fahrstuhl.jpg`
Hintergruende aus den BSS (probe_bg_dump, dieselbe MDEC-Kette wie im Spiel), Messwerte je Bildpunkt:
* **Revier-Fahrstuhl (ROOM1080, Einstiege 1F/2F/3F)**: ZWEI glatte graue Stahlfluegel mit senkrechter
  Mittelfuge in tiefer Nische, kein Bedienkasten, kein Schild, keine Verzahnung.
  * ROOM10C00 (2F, S040): Tuer x 71..116, Kopfbalken y 37..43 (L = 0), Fuge als Helligkeitsminimum (Zeile 60:
    links 37..42, Mitte 25..35, rechts 35..43). Zeilenmittel L: y 46..60 ~38, y 90 ~30, y 120 ~20, y 140 ~8.
  * ROOM11200 (3F, S057): Fuge x 97 (Zeile 60) / 100 (90) / 104 (120); linker Fluegel 45 -> 32 zur Fuge,
    rechter 40 -> 53 von der Fuge weg (beide Fluegel zur Mitte dunkler); unten dunkel (Zeile 140 ~12).
  * ROOM10405 (1F, S013): Tuer x 94..109 (daneben die helle Nischenwand x 77..93), blau beleuchtet.
  * Median RGB der Tuerflaechen: 10C00 (26,30,29), 11200 (31,34,30), 10405 (21,39,44).
  * ROOM10800 (Kabine von innen, Cut 0): zweifluegelig, Mittelfuge, darueber Etagenanzeige; Median (55,55,55).
* **Labor-Fahrstuhl (ROOM4020, Einstiege 4000/5000/50C0)**: entzerrt mit dem Runde-31-Werkzeug
  (`tools/fahrstuhl_r37/gemalt_ausschnitte.py` -> zensus_ausschnitte.schneide,
  `C_belege/c0_gemalt_ausschnitte_r31werkzeug.jpg`): S290 ROOM50C00 und S260 ROOM50005 zeigen die VERZAHNTE
  Mittelfuge mit Bedienkasten = RE2 DOOR27. Fuer ROOM4020 ist DOOR27 also richtig (so auch Runde 31,
  T109/T114/T115 "eigen (DOOR27)").

### V3 Alle RE2-Tuerarchive als Bogen (`C_belege/c0_re2_alle_tuerarchive.jpg`)
`RE15_RE2_ASSET_ROOT=info/re2leon/COMMON RE15_TUER_ARCHIV=<alle 55 V0 (Treppen/Leiter V4) + 18 Zusatz-Varianten>`,
73 Laeufe, je Anfang/Mitte. Mittig oeffnende Schiebetueren in RE2: **DOOR27** V0..V2 (Labor-Fahrstuhl, verzahnt,
Bedienkasten), **DOOR2C** V0..V3 (zwei Fluegel mit grossen Drahtglasfenstern, roter Sockel), **DOOR2A** V2/V3 (zwei
Standardblaetter mit Sichtfenster + Lueftung, Bahnsteig 7000-7040). Hubtuer DOOR25 ("SHAFT TYPE-L", Schild,
Warnaufkleber, Rippen), Drehtuer DOOR29 ("TYPE-M", Bedientafel), Scherengitter DOOR10 (Irons-Kabine RE2 ROOM21B0).
**Keine RE2-Tuer ist eine glatte graue zweifluegelige Revier-Fahrstuhltuer** -> eigenes Port-Archiv (Auftrag:
"wenn es keine passende Fahrstuhl Tuer gibt, muessen wir die wieder bauen").

### V4 Ausstieg (echter Skriptweg: Knopf der Kabine = Evt_exec, Fahrt, Gang zur Tuer, Aot_on)
```
ROOM1080: RE15_DEBUG_JUMP=1080@120 RE15_FORCE_EVENT=8@200 (sub08 = Knopf 2F) RE15_EXIT_AT=60#10C0
  [scd F217] Cut_chg(0) ... [scd] Plc_dest(slot=0 mode=0x04 dest=(-13650,-600) flag=32)
  [aot] DOOR FIRE slot=1 rect=(0,0,hw=0,hh=0) target_cut=0 spawn=(1450,0,7300)
  [room] PC loaded room10c0.rdt                       <- KEINE "[tuer] Sequenz"-Zeile
ROOM4020: RE15_DEBUG_JUMP=4020@120 RE15_FORCE_EVENT=8@200 RE15_EXIT_AT=60#5000
  [aot] DOOR FIRE slot=1 rect=(0,0,hw=0,hh=0) target_cut=5 spawn=(-10550,0,-14750)
  [room] PC loaded room5000.rdt                       <- KEINE "[tuer] Sequenz"-Zeile
```

## RE-Belege

### B1 RE1.5: der Ausstieg ist ein Tuer-Uebergang (Skript feuert die Tuer, nachdem der Spieler zur Tuer geht)
ROOM1080.RDT (= ROOM1081.RDT, gleiche Offsets) main00:
```
@0x00482 3b 00 02 31 00 00 | 00 x8 | 50 aa 00 00 08 d5 00 04 | 00 04 05 ..  Platz 0 -> ROOM1040 (-21936,0,-11000) Cut 5
@0x004A2 3b 01 02 31 00 00 | 00 x8 | aa 05 00 00 84 1c 00 0c | 00 0c 00 ..  Platz 1 -> ROOM10C0 (1450,0,7300) Cut 0
@0x004C2 3b 02 02 31 00 00 | 00 x8 | 14 05 00 00 84 1c 00 0c | 00 12 00 ..  Platz 2 -> ROOM1120 (1300,0,7300) Cut 0
```
sub08 (Knopf 2F; sub07/sub09 gleich gebaut, Aot_on 0/2):
```
@0x00788 21 03 37 00  Ck(3,0x37)==0 -> Fahrt     @0x007B0/B4 Set(2,0)=0 Set(2,2)=0   @0x007B8 Cut_chg 3
@0x007BE 22 02 07 01  Set(2,7)=1                  @0x007C2 22 01 1b 01  Set(1,0x1B)=1  @0x007C6 Cut_chg 0
@0x007CC 34 00 ae ca / 34 02 cc f7 / 34 04 00 0c   Spieler (-13650, -2100), Blick 3072
@0x007F8 40 00 04 20 ae ca a8 fd  Plc_dest -> (-13650,-600)   @0x00800 Do .. @0x00806 Edwhile Ck(5,0x20)==0
@0x0080E 47 01        Aot_on 1  (sub10 @0x008B4/C4/D4: "Tuer auf"-Knopf, Aot_on der aktuellen Etage)
```
ROOM4020.RDT (= ROOM4021.RDT): Platz 0..2 @0x0063A/@0x0065A/@0x0067A Null-Rechteck -> ROOM4000 (-26516,0,-8963) /
ROOM5000 (-10550,0,-14750) / ROOM50C0 (-6975,0,-10005); Aot_on 0/1/2 @0x00990/@0x00A36/@0x00ACC (sub07..09),
@0x00AE0/AF0/B00 (sub10).
Aot_on 0x47 @0x800407bc (PSX.EXE, selbst disassembliert): `lbu v0,1(v0)` (Platz) -> Satz aus 0x800ac9b0[Platz],
Nutzlast +12 (bzw. +20 bei sat&0x80), `lw v0,0(at)` aus 0x8007469c[sce] und `jalr v0` @0x8004082c = DERSELBE
sce-2-Tuer-Handler wie beim Betreten. RE1.5 startet dort die Tuermaschine mit DOOR00 = `Evt_end` (keine Sequenz,
Memory reai-v2-tuermodelle-polygone) -> Beta -> Retail: RE2 ist das Ziel.

### B2 RE2 Retail: die Kabine spielt beim AUSSTIEG die Fahrstuhltuer (ROOM6010 = A-2-Kabine)
`analysis/befunde_runde30/tools/r30_re2_scd.py 6010` (Bytes selbst gelesen, info/re2leon/PL0/RDT/ROOM6010.RDT):
```
main00 @0x004C2 3b 00 01 31 00 00 7a ae 5e 9d 08 07 3c 0f 26 b7 00 00 3c fb 00 00 05 02 04 00 27 00 ..  aot 0 -> ROOM6020, Archiv 0x27 V0
main00 @0x004E2 3b 01 ..                                       05 06 06 00 27 00 ..  aot 1 -> ROOM6060 DOOR27 V0
main00 @0x00502 3b 02 ..                                       06 00 05 00 27 00 ..  aot 2 -> ROOM7000 DOOR27 V0
sub00  @0x005FE..0x00618 Aot_reset aot 0/1/2 sce 0 (Tueren aus, solange die Kabine steht)
sub02  @0x006F6 22 02 07 01 Set(2,7)=1   @0x006FA 22 01 1b 01 Set(1,0x1B)=1   (dieselben Merker wie RE1.5 sub08)
       @0x00740 46 00 01 21 26 b7 00 00 3c fb  Aot_reset aot 0 sce 1 (Tuer) sat 0x21 (Nutzlast +12/+13 = 27 00 bleibt)
       @0x0074E 46 01 01 21 48 ae 00 00 20 b8  Aot_reset aot 1 ...
       @0x0075A 40 00 04 20 e0 b1 52 a4        Plc_dest -> (-20000,-23470) = in das Tuerrechteck (x -20870..-19070,
                                               z -25250..-21350) -> die Tuer feuert, Sequenz DOOR27 V0
```
= dieselbe Kette wie RE1.5 (Merker (2,7)/(1,0x1B) gesetzt, Spieler laeuft per Plc_dest zur Tuer, Tuer feuert), nur
mit Sequenz. Fuer ROOM4020 (Labor-Kabine "A-2 ELEVATOR", gemalt wie RE2) gilt damit DOOR27 V0 beim Aussteigen
unveraendert; fuer ROOM1080 dieselbe Tuer wie beim Einstieg (RE2: beide Seiten eines Paares tragen dasselbe Archiv,
238/238 Paare, analysis/befunde_runde31/tueren_02_re2.md 2.1).

### B3 Port-Hindernis: die Zwischensequenz-Sperre
`engine/src/aot_common.c tuer_sequenz_anfragen`: bei `g_scd.player_mode == 2 || letterbox_countdown != 0` faellt jede
Sequenz weg (PORT-WAHL Runde 31). player_mode = 2 leitet main.c jedes Bild aus `flag(1,27) || flag(2,7)` ab - genau
die beiden Merker, die RE1.5 sub07..09 UND RE2 ROOM6010 sub02 waehrend der Fahrt setzen. RE2 spielt die Tuer in
diesem Zustand (B2) -> die Kabinen-Ausstiege muessen an der Sperre vorbei. Ausserdem traegt die Zuordnung
(Schluessel Raum + Flaeche + Band) die drei Null-Rechteck-Ausstiege nicht auseinander (alle (0,0,0,0)) -> eigener
Weg nach Raum + Ziel (Seiten der Runde 31: ROOM1080 S027/S028/S029, ROOM4020 S221/S222/S223).

## Umsetzung

### U1 Gemaltes Fluegelfeld (Messwerkzeug, `re15_port/tools/fahrstuhl_r37/`)
* `gemalt_ausschnitte.py`: Runde-31-Werkzeug (zensus_ausschnitte.schneide, exakte RDT-Kamera) auf die sechs
  Fahrstuhl-Einstiege; Ergebnis V2 (Labor = DOOR27 gemalt; Revier: die Standardblatt-Verfeinerung liegt teils auf
  der Nischenwand, weil die Revier-Tuer tief in der Nische sitzt).
* `gemalt_fluegel.py`: die sechs Fluegel (ROOM10C00 / ROOM11200 / ROOM10800 je links + rechts) mit an
  Helligkeitskanten abgelesenen Ecken (Kopfbalken L = 0, Nischenkante, Fugen-Minimum, Rahmen, Bodenlinie; Ecken im
  Kopf der Datei) per Homographie auf 60 x 128 entzerrt, rechte Fluegel gespiegelt (Spalte 0 = Aussenkante, 59 =
  Fuge), gemittelt -> `C_belege/feld/feld_flur.png` (4 Fluegel), `feld_kabine.png` (2 Fluegel), `profil.json`.
  Gemessen: Flur Median (25.2,28.5,26.2), Zeilen L oben 39.7 -> unten 6.4, Spalten Aussenkante 31.4 -> Fuge 20.1;
  Kabine Median (48,49,49), Zeilen 67.3 / 85.3 (oben, Kabinenlicht) -> 16.4.

### U2 Port-Archive P27F / P27C (`tools/fahrstuhl_r37/fahrstuhl_archiv_bauen.py`, PORT-WAHL Gestalt/Textur)
* Basis = RE2 **DOOR27** = die RE2-Fahrstuhltuer (ROOM6010 @0x004C2 Byte 26 = 0x27): **Tonteil und SCD bytegleich**
  (Fahrstuhl-Mechanik und -Ton RE2: Skript 1 @Datei 0x6040 stellt obj 0 mesh 0 bei z -3000 und obj 1 mesh 1 bei
  z +3000 auf (`48 f4` / `b8 0b`), Skript 2/3 schieben je 2240 gegenlaeufig, Se_on Bild 70).
* **MD1 neu** (md1_eigen = 2, `include/re15_door_seq.h`): Mesh 0/1 = glatte Fluegel, lokal x -100..100, y -6600..0,
  z 1140..3000 bzw. -3000..-1140 (Welt -1860..0 / 0..1860, Fuge genau in der Mitte), Vorderseite 4 x 8 Felder
  (64 Dreiecke), Stirn-/Ober-/Unter-/Rueckseite je 2 = 74 Dreiecke je Fluegel; Mesh 2 = DOOR27 Mesh 2 bytegleich
  (kein Skript stellt es auf). Masse mit Herkunft im Dateikopf: Oeffnung 2000 Raumeinheiten (Door_aot_set w 0x07D0
  ROOM10C0 @0x00E82 / ROOM1120 @0x00CB6; Runde-31-Verfeinerung 2150/1850) x Massstab 6602/3549 = 1,86 -> 3720,
  Hoehe 6600 = RE2-Standardblatt; Sichtseite je Dreieck nach der NCLIP-Regel (do2_format._normale).
* **TIM neu** (128 x 256, 8 bit, 1 CLUT wie DOOR27): u 0..119 Fluegel (u 0 Aussenkante, u 119 Fuge, beide Fluegel
  gleich gemappt = spiegelbildlich), Feld bikubisch auf 120 x 256 + Gauss 1,5, Texel = gemalt * 128 / 73 (c = NCCT
  der Vorderseite, BK 68 @0x800142e8 ohne Flag 0x1000 - DOOR27-Objektflags 0x0a80/0x0280), Feinkorn RE2-Stahl
  DOOR07 * 0,5 (Runde 33 KORN_GLATT; Schlitze + Druecker-Schild geflickt), Fuge Spalte 119 * 0,75 (gemalte Fuge
  0,7..0,8 der Nachbarn), u 120..127 Kantenstreifen Median * 0,6.
* P27F = Flurseite (feld_flur), P27C = Kabinenseite (feld_kabine); je 63412 B, Ton 23176, Modell 38836, Sektor 12;
  FNV P27F 0x5608136D, P27C 0xB8E6D960 (`C_belege/archiv_protokoll.json`).
* `engine/src/gen/re15_tuer_eigen.inc` gezielt nachgefuehrt (Muster r36_tueren_bauen.py): Eintraege 31 P27F /
  32 P27C, Zeilen S013/S040/S057 (xxx0 + xxx1) -> eigen 31. `analysis/befunde_runde33/tueren_rest/plan.json`:
  T011/T017/T018 archiv P27F (+ Archiv-Eintraege mit "bau": dieses Werkzeug; ein voller tuer_archiv_bauen-Lauf
  bricht dort laut ab ("geplante Seiten ohne Zeile"), statt die Wahl still zurueckzudrehen).

### U3 Ausstieg (`engine/src/fahrstuhl_r37.c`, `include/re15_fahrstuhl_r37.h`, Haken `engine/src/aot_common.c`)
* Tabelle der sechs Null-Rechteck-Ausstiege, Schluessel Raum (ohne Spielervariante) + Ziel-Stage/-Raum:
  ROOM1080 -> 1040/10C0/1120 = P27C V0, S027/S028/S029 (T011/T017/T018), Satz @0x00482/@0x004A2/@0x004C2;
  ROOM4020 -> 4000/5000/50C0 = RE2-Datei DOOR27 V0, S221/S222/S223 (T109/T115/T114), @0x0063A/@0x0065A/@0x0067A.
  Variante 0 = RE2 ROOM6010 @0x004C2 Byte 27. Gilt fuer Leon (xxx0) und Elza (xxx1).
* Haken (2 Zeilen + include) am Anfang von `tuer_sequenz_anfragen`: ein Kabinen-Ausstieg stellt seine Anfrage VOR
  der Zwischensequenz-Sperre der Runde 31 (B3: RE2 spielt die Tuer bei gesetzten (2,7)/(1,0x1B)). Alle anderen
  Tueren laufen unveraendert durch Zuordnung + Sperre. PSX: keine Anfrage (wie door_seq_zuordnung.c).
* Gespielt wird die Sequenz wie jede Aot_on-Tuer in main.c vor `re15_room_apply_pending` (Runde 31).

### U4 Angepasste Riegel anderer Spuren (ihre Voraussetzung "S013/S040/S057 = RE2-Datei DOOR27" ist entfallen)
* `tests/unit/probe_r36_tueren.c` (unit_r36_tueren_fahrstuhl): Zeilen/Durchgang erwarten jetzt Port-Archiv P27F
  (Basis DOOR27 V0) - Maschinen-Teil (DOOR27 zwei Fluegel, Gegenprobe DOOR25) unveraendert.
* `tests/integration/test_r36_tueren.cmake` (integration_r36_tueren C): Protokollzeilen von S040 mit "Port-Archiv P27F".
* `tests/unit/probe_r33_tueren.c` (unit_r33_tueren_archive): Zweig md1_eigen 2 (MD1 neu, gleiche Meshzahl; Tonteil,
  SCD, Bild-fuer-Bild-Lauf werden wie bisher gegen DOOR27 geprueft).

## Messung nachher

### N1 Einstieg (`RE15_TUER_SEITE=S013,S040,S057,S215`), Bild `C_belege/c1_nachher_p27f_p27c_door27.jpg`
```
[tuer-seite] S013 ROOM1040 DOOR27 V0 Spender FF Port-Archiv P27F
[tuer] Sequenz Archiv 2 DOOR27 Port-Archiv P27F Variante 0 Bit7 0 Tuer 39 Seite S013 T011 Spender FF (10 Skripte)
[tuer] Sequenz fertig: 271 Bilder + 1 Warten ... (S040 / S057 ebenso; S215 Labor weiter DOOR27 V0 RE2-Datei)
```
Echter Weg (Strom (4,0xF3), vor der Tuer Aktionstaste, integration_r37_fahrstuhl B):
`[tuer] Sequenz Archiv 2 DOOR27 Port-Archiv P27F Variante 0 Bit7 0 Tuer 39 Seite S040 T017` vor `room1080.rdt`.
Gesehen: zwei glatte graugruene Stahlfluegel mit gerader Mittelfuge schieben aus der Mitte auseinander (Bild 70..120),
kein Bedienkasten, keine Zaehne, keine Schilder.

### N2 gezeigt gegen gemalt (`tools/fahrstuhl_r37/gezeigt_gegen_gemalt.py`, Bild 20 der Bogen, Zeile <-> Fluegel-v
ueber die Tuerkamera Auge x 10000, Vorderseite x 2100, Brennweite 290), `C_belege/c1_gemalt_gegen_gezeigt.jpg`:

| | L gezeigt/gemalt je Zeile (Feldzeile 4..112) | max / Mittel abs. Fehler | Median Tuerflaeche gezeigt / gemalt |
|---|---|---|---|
| P27F (S040) | 41.0/40.5, 38.9/40.3, 36.0/37.2, 34.3/33.8, 30.5/31.0, 27.0/27.9, 23.0/23.9, 20.9/19.9, 14.0/13.3, 7.8/7.9 | 1.4 / 0.8 | (27,32,27) / (25,28,26) |
| P27C (S028) | 70.9/65.7, 86.0/86.0, 86.0/87.0, 82.0/84.0, 68.0/66.0, 55.0/54.0, 41.0/38.7, 36.0/35.5, 27.0/26.0, 18.0/18.0 | 5.2 / 1.5 | (55,55,55) / (48,49,49) |
| vorher DOOR27 (S040) | 0.0/40.5 ... 44.0/27.9, 35.0/13.3, 22.4/7.9 | 40.5 / 15.8 | (41,41,32) oliv / (25,28,26) |

### N3 Ausstieg (echter Skriptweg, `RE15_FORCE_EVENT` = der Evt_exec des Kabinenknopfs)
```
ROOM1080 sub08 (Leon):  [aot] DOOR FIRE slot=1 rect=(0,0,hw=0,hh=0) target_cut=0 spawn=(1450,0,7300)
                        [tuer] Sequenz Archiv 2 DOOR27 Port-Archiv P27C Variante 0 Bit7 0 Tuer 39 Seite S028 T017
                        [tuer] Bild 70 Se_on Tonkopf-Eintrag 0            (mit RE15_AUDIO_CAP_SYNC)
                        [tuer] Sequenz fertig: 271 Bilder + 1 Warten, 1 Se_on, Schliesston 1, Ton geladen
                        [room] PC loaded room10c0.rdt
ROOM1081 sub09 (Elza, Spieler-Familie PL04 Elza-Bit=1): P27C ... Seite S029 T018 -> room1121.rdt
ROOM4020 sub08 (Leon):  [tuer] Sequenz Archiv 2 DOOR27 Variante 0 Bit7 0 Tuer 39 Seite S222 T115 -> room5000.rdt
ROOM4021 sub07 (Elza):  [tuer] Sequenz Archiv 2 DOOR27 Variante 0 ... Seite S221 T109 -> room4001.rdt
```
Ablauf (`C_belege/c2_ausstieg_1080_ablauf.jpg`): Kabine blendet ab, die helle Kabinentuer (P27C) schiebt auf, Kamera
faehrt durch, Abblende, ROOM10C0 mit Leon vor dem Fahrstuhl (1450,0,7300).

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
