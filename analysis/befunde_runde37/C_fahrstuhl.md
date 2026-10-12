# Runde 37 — Spur C "fahrstuhl" (Dossier)

Baum: `.claude/worktrees/r37_fahrstuhl`, Zweig `r37/fahrstuhl`, Basis master f1e019ca. Stand: FERTIG - beide Punkte
gebaut, gemessen, gepinnt (Endstand am Ende).

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

### N4 Durchlauf in EINEM Prozess (echter Einstieg, Kabine, Ausstieg)
```
RE15_SET_FLAG=4:243 RE15_DEBUG_JUMP=10C0@120 RE15_PLAYER_POS=1500,7000,1024,0 RE15_PAD_AT=150:A,160:A,170:A
RE15_FORCE_EVENT=8@200 RE15_EXIT_AT=400#10C0 RE15_AUDIO_CAP_SYNC=cap.raw
  [aot] DOOR FIRE slot=1 rect=(1500,6400,hw=1000,hh=500) target_cut=0 spawn=(-13650,0,-900)
  [tuer] Sequenz Archiv 2 DOOR27 Port-Archiv P27F Variante 0 Bit7 0 Tuer 39 Seite S040 T017 ... Bild 70 Se_on
  [room] PC loaded room1080.rdt
  [force-event] scd_event_fire(8) at F200      (Kabine steht auf 2F: ROOM10C0 @0x00FEE Set(3,0x37)=1 -> keine Fahrt)
  [aot] DOOR FIRE slot=1 rect=(0,0,hw=0,hh=0) target_cut=0 spawn=(1450,0,7300)
  [tuer] Sequenz Archiv 2 DOOR27 Port-Archiv P27C Variante 0 Bit7 0 Tuer 39 Seite S028 T017 ... Bild 70 Se_on
  [room] PC loaded room10c0.rdt
dasselbe mit RE15_FORCE_EVENT=9@200 (Knopf 3F): [scd F202] Cut_chg(4) .. [scd F217] Cut_chg(0) .. Plc_dest ..
  DOOR FIRE slot=2 -> P27C Seite S029 T018 -> room1120.rdt
```

## Tests

Neu (`tests/unit/probes/r37_fahrstuhl.cmake`, `tests/unit/probe_r37_fahrstuhl.c`, `tests/integration/test_r37_fahrstuhl.cmake`):

| Test | misst |
|---|---|
| `unit_r37_fahrstuhl_archiv` | P27F/P27C in der Tabelle (eigen 31/32, Basis 0x27, md1_eigen 2), Datei = Groesse/FNV, Tonteil + SCD bytegleich DOOR27; Fluegel 1860 breit an der Fuge +-3000, y -6600..0, x 100, Fugenkante z-Spanne 0 (Gegenprobe DOOR27: 618/619 = Zaehne); Maschine V0 Bild fuer Bild = DOOR27 (271 Bilder, Fluegel -2240/+2240); Bild 20 durch den Tuerzeichner: 128 Vorderseiten-Dreiecke alle Eckfarbe 73; gezeigt (Texel*73/128) gegen gemaltes Zeilenprofil: P27F max 1.6, P27C max 3.3 (Grenze 6), grau |
| `unit_r37_fahrstuhl_einstieg` | echter Durchgang ROOM1040/10C0/1120 (Strom (4,0xF3)) -> ROOM1080: genau 1 Anfrage P27F V0 S013/S040/S057 |
| `unit_r37_fahrstuhl_ausstieg` | echter Skriptweg mit player_mode-Proxy: ROOM1080 sub07/08/09 -> P27C S027/S028/S029 (-> 1040/10C0/1120), ROOM1081 sub08 (Elza) -> P27C S028, ROOM4020 sub07/08/09 -> DOOR27 V0 RE2-Datei S221/S222/S223, ROOM4021 sub09 -> S223; beim Feuern player_mode 2; ohne Fahrt ((3,0x37) gesetzt, sub08 und Tuer-auf sub10) dieselbe Sequenz mit player_mode 0; Gegenprobe normale Tuer ROOM10C0 in player_mode 2 -> keine Sequenz |
| `integration_r37_fahrstuhl` | echte exe: A Seiten S013/S040/S057 = P27F + Bogen-Bilder; B echter Einstieg 2F (Aktionstaste) P27F S040 vor room1080; C Ausstieg 1080 sub08 P27C S028 vor room10c0 mit Ton (Bild 70 Se_on, 1 Se_on); D Ausstieg 4020 DOOR27 V0 S222 vor room5000; E Elza 1081 sub09 P27C S029 vor room1121 |

**Gegenprobe am Code** (Riegel misst den Defekt, nicht sich selbst): Haken in `aot_common.c` auskommentiert, neu gebaut,
`probe_r37_fahrstuhl ausstieg` -> `ausstieg: ROT (20 Fehler)`, jeder der 10 Ausstiege "Raumwechsel -> ROOMxxxx,
Laeufer 0 x" (auch die beiden ohne Fahrt: ohne Haken haben Null-Rechtecke keine Zeile); Haken zurueck -> GRUEN.

Angepasst (U4): `unit_r36_tueren_fahrstuhl`, `integration_r36_tueren`, `unit_r33_tueren_archive`.

Gezielte Laeufe (KEINE volle Suite), Regex = eigene Tests + Testgruppen der beruehrten Module (Tueren/Tuersequenz,
AOT-Feuerweg, Fahrstuhl, Elza-Selbsttueren, Kopflauf):
```
ctest --test-dir re15_port/build -R "r37_fahrstuhl|r36_tueren|r33_tuer|unit_r31_|r32_unterteilung|unit_door_seq|r35_inhalt_tueren|fahrstuhl|elevator|unit_elev_se|r30_tuer|door_|keydoor|unit_aot_|unit_scd_aot_opcodes|unit_tor_1170|r35_cut10f0_tuerton|r30_elza_selbsttuer|r35_zgirl_tuer|r35_zgirl_selbsttueren|integration_headless_run|r34_plattform_ton|r35_spiegel|tuer" --timeout 900 -j 2
```
Ergebnis Endstand (Baum bc10f30b, alle Riegel inkl. der Ausstiege ohne Fahrt, Protokoll build/ ct_final):
```
100% tests passed, 0 tests failed out of 63
```
(u. a. integration_r37_fahrstuhl 173.6 s, integration_r35_karte_fahrstuhl 181.9 s, integration_r36_tueren 45.5 s,
unit_fahrstuhl_1080_etagen, unit_fahrstuhl_4020_etagen, unit_r33_tueren_archive/_zuordnung/_maschine, unit_r31_*,
unit_r37_fahrstuhl_archiv/_einstieg/_ausstieg). Ein Zwischenlauf vor den zwei Ausstiegen ohne Fahrt: ebenfalls
`100% tests passed, 0 tests failed out of 63`.

## OFFEN

* Lastenaufzug ROOM3080 ("WAREHOUSE LIFT", STAGE3): auch dort verlaesst der Spieler die Kabine per Null-Rechteck-Tuer
  (ROOM3080.RDT main00 @0x0072A Platz 0 -> ROOM4000 Cut 1, Aot_on 0 @0x00A22 am Ende der Fahrt sub02) ohne Sequenz.
  Nicht Teil des Auftrags ("dem Fahrstuhl" = Revier-Fahrstuhl ROOM1080 aus Runde 36; Vertrag: ROOM1080/4020); der
  Lastenaufzug ist eine Hubbuehne mit Gitter (Einstieg S195 = P14A Schiebe-Gittertor, Gegenrichtung S218 = P2DS
  Hubbuehne DOOR2D) - eine Ausstiegssequenz braeuchte eine eigene RE2-Herleitung (DOOR2D-Variante abwaerts).
* Gestalt der Fluegel = PORT-WAHL (RE1.5 hat keine Tuersequenz, RE2 keine Revier-Fahrstuhltuer): Masse und Farben sind
  gemessen/belegt (U2), die Geometrie (glatte Platten, 200 dick, 4 x 8 Felder) ist gebaut, nicht aus einem Original.
* Etagenanzeige ueber der Kabinentuer (gemalt ROOM10800: Knopfleiste "v 1 2 3 ^") ist kein Teil der Fluegel und fehlt
  in der Sequenz - wie bei allen RE2-Tuersequenzen steht die Tuer vor Schwarz.

## Fuer den Nutzer

* **Neue Assets (Paket-Gate):** NEU `re15_port/shared_assets/RE15DOOR/P27F.DO2` (63412 B, FNV 0x5608136D) und
  `re15_port/shared_assets/RE15DOOR/P27C.DO2` (63412 B, FNV 0xB8E6D960), sha1 in `C_belege/archiv_protokoll.json`.
  Beide liegen im Ordner RE15DOOR, den make_package.sh (`cp -r RE15DOOR`) und build.gradle (ganzer Ordner) schon
  mitnehmen; ein Gate-Eintrag je Datei ist Sache des Orchestrators (release/ ist tabu).
* **Sprachdateien:** keine.
* **Was man sieht/hoert:** Fahrstuhl im Revier (1F/2F/3F): beim Einsteigen schieben zwei glatte, dunkle graugruene
  Stahlfluegel mit gerader Mittelfuge aus der Mitte auseinander (wie die gemalte Tuer in der Nische), Ton der
  RE2-Fahrstuhltuer. Beim Aussteigen (nach der Fahrt, oder direkt mit dem Etagen-/Tuer-auf-Knopf) dieselbe Tuer von
  innen, hell wie die Kabinentuer im Bild der Kabine. Labor-Fahrstuhl (A-2): beim Aussteigen jetzt ebenfalls die
  Sequenz, dort die RE2-Labor-Fahrstuhltuer (so ist sie im RE1.5-Labor auch gemalt). Leon und Elza gleich.
* **Bedienhinweise (Messhaken):** `RE15_TUER_ARCHIV="P27F:0,P27C:0,27:0"` + `RE15_TUER_BOGEN=<dir>` zeigt die drei
  Fahrstuhltueren nebeneinander; `RE15_TUER_SEITE=S013,S040,S057` die Einstiege.
* **Entscheidungsfrage (nur falls gewuenscht):** Lastenaufzug ROOM3080 -> ROOM4000 hat beim Aussteigen ebenfalls keine
  Sequenz (OFFEN, nicht beauftragt).

## Endstand

* #6 Fahrstuhltuer der Tuersequenz: keine der 55 RE2-Tueren ist die gemalte Revier-Fahrstuhltuer (Bogen V3) -> gebaut:
  Port-Archive P27F (Flur) / P27C (Kabine) = RE2-Fahrstuhltuer DOOR27 (Mechanik, Ablauf, Ton bytegleich) mit zwei
  glatten Fluegeln und gerader Mittelfuge statt der verzahnten Labortuer; Farbe/Licht gezeigt = gemalt (max 1.4 / 5.2
  L-Stufen je Zeile gegen ROOM10C00+ROOM11200 bzw. ROOM10800; vorher DOOR27 40.5, oliv). Einstiege S013/S040/S057.
  Labor-Fahrstuhl ROOM4020: bleibt DOOR27 (dort gemalt wie RE2).
* #7 Tuersequenz beim Aussteigen: ROOM1080/1081 -> P27C, ROOM4020/4021 -> DOOR27 V0 (RE2 ROOM6010 sub02 @0x00740/
  @0x0075A spielt die Fahrstuhltuer beim Verlassen der Kabine), nach der Fahrt UND ohne Fahrt, Leon und Elza; in der
  exe gemessen inkl. Ton und im Durchlauf Einstieg -> Kabine -> Ausstieg.
* Tests: 3 neue Unit-Riegel + 1 Integration (Gegenprobe ohne Haken ROT), 3 fremde Riegel auf die neue Voraussetzung
  nachgezogen; gezielt `100% tests passed, 0 tests failed out of 63`.

## Nachbesserung 1 (nach Abnahme 0, 2026-10-12)

Auftrag: Maengel 1/2 der Abnahme 0 = AUFTRAG.md Nachtrag 5 (woertlich: "ja, dann mache den noch. Mir ist auch
aufgefallen in den Stage2 kleineres aufzügen fehlt noch das sound. Den sollte man auch aus Resident Evil 2
extrahieren."). (a) Ausstieg Lastenaufzug ROOM3080 -> ROOM4000 mit Tuersequenz, (b) Ton der kleineren Aufzuege in
STAGE2. #6/#7 (P27F/P27C, fahrstuhl_r37.c, aot_common-Haken, Riegel) bleiben unveraendert.
Stand: IN ARBEIT (fortlaufend).

### N1-0 Raeume bestimmen (b)
STAGE2 = Kanal (debug_jump_table.h Stage 1 = Spiel-STAGE2). Nachrichten aller zwoelf STAGE2-Raeume
(`tools/rdt_msgdump.py`): nur ROOM2030/2031 "TREATMENT POOL" spricht von einem Aufzug, msg 1 @0x2038 "The elevator is
not responding...Maybe it should be activated / from somewhere." (Strom kommt aus ROOM2060 "ENGINE ROOM", Generator).
SCD ROOM2030 (`tools/scd_dump_room.py`): ZWEI Hubplattformen (obj 1 und obj 2) = "kleinere Aufzuege" (Mehrzahl):
* Lift A: obj 1 (6725, y, 12050), y -5400 (Ebene 3) <-> -10800 (Ebene 6), Rechtecke Platz 4/5 (5850,11350,1500,1500);
  Fahrt sub02 @0x01E30 (Spieler) + sub03 @0x01ECA (Plattform), Speed_set y -120/+120, For 0x2D x 2 Bilder = 90 Bilder.
* Lift B: obj 2 (-10675, y, 4050), y -5400 (Ebene 3) <-> -16200 (Ebene 9), Platz 6/7 (-10550,2850,1500,1500);
  Fahrt sub04 @0x01F0A + sub05 @0x01FA4, For 0x5A x 2 Bilder = 180 Bilder.
Keines der Fahrskripte traegt ein Se_on (0x36).

### N1-1 Messung vorher (Stand 06bf9b12, Messkopie `re15_pc_r37c_n1.exe`, Ordner `build/r37_fahrstuhl/n1/`)
(a) `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=3080@120 RE15_EXIT_AT=60#4000` (erste Fahrt, (4,96)=0,
sub02 mit Ada-Szene, Gitterfluegel sub03..08):
```
[debug-menu] JUMP -> 308 WAREHOUSE LIFT (ROOM3080) spawn=(-25000,0,-25000) cut=0
[room] PC loaded room3080.rdt (240472 bytes)
[aot] DOOR FIRE slot=0 rect=(0,0,hw=0,hh=0) target_cut=1 spawn=(-7600,0,-6760)
[room] PC loaded room4000.rdt (288632 bytes)          <- KEINE "[tuer] Sequenz"-Zeile (Befund 5a reproduziert)
```
(b) `RE15_DEBUG_JUMP=2030@120 RE15_SET_FLAG=3:126 (Generator) RE15_FORCE_EVENT=2@260 (sub02 = Lift A) RE15_SE_DEBUG=1
RE15_AUDIO_CAP_SYNC=v_2030.raw RE15_EXIT_AT=520`: `[force-event] scd_event_fire(2) at F260` -> `[scd F351] Cut_chg(10)`
(Fahrt 90 Bilder + 1), im ganzen Lauf KEINE `[se]`-Zeile = kein einziger SE (Befund 5b reproduziert).

### N1-2 RE-Belege (Bytes selbst gelesen)

**(a) Lastenaufzug ROOM3080 -> ROOM4000**
* RE1.5 ROOM3080.RDT main00 @0x0072A `3b 00 02 31 00 00 | 00 x8 | 50 e2 00 00 98 e5 80 07 | 03 00 01 00 00 00 ..` = Platz 0,
  Rechteck (0,0,0,0), Ziel (-7600,0,-6760) Blick 0x780, Stage 3 Raum 00 Cut 1, RE1.5-Archiv-Byte +26 = 00 (DOOR00 =
  Evt_end, keine Sequenz). sub00 @0x007A4 `06 00 48 00 / 21 04 60 00` Ck(4,96)==0 -> Evt_exec sub02 @0x007E8, sonst sub09
  @0x007F0. sub02 (erste Fahrt, Ada-Szene): @0x007FE `22 02 07 01` Set(2,7)=1, @0x00802 `22 01 1b 01` Set(1,0x1B)=1, Fahrt-
  Pulse @0x009FE..0x00A16 (SIG2), @0x00A1A Message_on 1 ("Ada: That's... got to be..."), @0x00A1E Sleep 100, @0x00A22
  `47 00` Aot_on 0. sub09 (weitere Fahrt): @0x00BBA/@0x00BBE Set(2,7)/(1,0x1B), @0x00BFA `47 00`. ROOM3081 (Elza): derselbe
  Satz @0x0072A, sub02 @0x0081E / sub03 @0x00862 `47 00`. Aot_on @0x800407bc -> `jalr v0` @0x8004082c = Tuer-Handler (B1).
* RE2-Vorbild = die EINZIGE DOOR2D-Tuer in RE2 (Runde 31 tueren_02_re2.md §5): ROOM6170 ("A huge lift used to reach the
  platform", msg 1 @0x04797) main00 @0x035A2 `3b 01 01 41 00 00 57 bc b4 ed 14 1e 80 0c f4 17 00 00 b5 6d 00 04 06 00 00 00
  2d 05 ..` = aot 1, sat 0x41, Ziel Stage 6 Raum 00 (ROOM7000, Bahnsteig UNTEN), Byte 26/27 = `2d 05` = DOOR2D Variante 5;
  sub02 @0x03604 Aot_reset aot 1 sce 0 (aus), sub15 @0x03F90 Aot_reset aot 1 sce 1 sat 0x41 (an, sobald der Lift da ist).
* DOOR2D-Skripte (`tools/tor/tuerskript_dump.py dis DOOR2D.DO2`): Skript 0 @Datei 0x601c Switch(var 12): Case 0/2/4 ->
  Gosub 1, Case 1/3/5 -> Gosub 2. Skript 1 @0x6050 (301 Bilder): Sleep 60, Se_on 0, Skript 3 (Ruetteln), Sleep 50, Skript 4,
  Sleep 190, Se_on 1. Skript 2 @0x6130 (451 Bilder): Sleep 20, Se_on 0, Skript 3, Sleep 35, Skript 5, Sleep 120, Se_on 1,
  Sleep 245, Blende, Sleep 30. Skript 4 @0x6300: obj 2 (Mesh 2) Member_set y -4400 / x 6600, For 60 { y += 125, x -= 200 }
  x4 = das Schacht-Fachwerk zieht nach UNTEN = Buehne faehrt HINAUF; Skript 5 @0x6344: y 3100 / x -5400, For 60 { y -= 125,
  x += 200 } = HINAB. Member 11/12/13 = obj x/y/z (RE2 @0x80055d30/38/40 `sw a2,56/60/64(a0)`, door_seq_common.c:435-437).
  => ROOM3080 -> ROOM4000 faehrt hinab = **Variante 5** (wie RE2 ROOM6170 -> ROOM7000); die Gegenrichtung S218 (ROOM4000 ->
  ROOM3070) spielt seit Runde 33 Variante 4. **Archiv P2DS** = das der Gegenrichtung (RE2: beide Seiten eines Paares tragen
  dasselbe Archiv, 238/238, Runde 31 tueren_02_re2.md 2.1).
* **Befund am Bestand (Runde 33 G10, P2DS):** P2DS zeigte die Fahrt 301/451 Bilder lang als STEHENDES Bild
  (`C_belege/n1_p2ds_re2_alt_neu.jpg`: Reihe 1 RE2 DOOR2D V5 mit vorbeiziehendem Fachwerk, Reihe 2 P2DS alt ohne). Ursache
  gemessen: das Rezept `tools/tueren/tuer_rezepte.py rezept_P2DS` nahm ausser der Bedientafel (Mesh 1, UV u 1..93 v 194..253)
  eine "Lampe" = Mesh-2-Teil u 96..127 v 193..255 weg - dort liegt aber GANZ Mesh 2 (60/60 Dreiecke, UV u 96..126 v 193..252,
  1737/1737 Texel auf Index 0), und Mesh 2 ist das rot beleuchtete Schacht-Fachwerk (obj 2 + Kinder 3/4/5) = die einzige
  Bewegung der Fahrt. Gemalt ist der Schacht MIT Fachwerk (ROOM4000 c01, Runde 31 stapel_5.pruefer.md S218: "offener
  Aufzugschacht mit Fachwerk, gelb-schwarzer Rand"). Korrektur: nur Mesh 1 weg (5580 Texel) -> P2DS neu (Reihe 3).

**(b) STAGE2-Aufzuege ROOM2030/2031**
* RE1.5 (N1-0): Lift A sub02 @0x01E30 `22 02 07 01` Set(2,7)=1, hinauf @0x01E54 `0d 00 06 00 2d 00 | 30 02 02 00 0e 00` For 45
  x (Add_speed, 2x Evt_next) = 90 Bilder (Speed_set y -120 @0x01E50), hinab @0x01E98 (Else @0x01E7C); Lift B sub04 @0x01F0A,
  @0x01F2E / @0x01F72 For 0x5A = 90 x 2 = 180 Bilder. Ausloeser: Platz 5/7 (Ebene 3) bzw. 4/6 (Ebene 6/9) Aot_set sce 3 auf der
  Plattform (Aktionstaste); ohne Strom (3,126)=0 setzt sub01 @0x01E18/@0x01E22 die Plaetze 5/7 auf msg 1. KEIN Se_on.
* RE2-Vorbild = die RE2-HUBPLATTFORM ROOM6030 (msg 9 @0x03CE0 "It's a switch for the lift. Will you push it?";
  RE2_SOUND_LISTE.md "ROOM603.SAP - ... the room with the scenario B box/elevator"), selbe Art wie ROOM2030 (Plattform obj 0 +
  Spieler fahren per Skript senkrecht 5400): sub10 @0x02F8C `22 02 07 01` Set(2,7)=1, @0x02F90 `36 02 11 00 ..` **Se_on(2,0x11)
  KLICK**, @0x02F9C Sleep 10, Cut_chg 1, Ruetteln, @0x02FBA `36 02 0b 00 ..` **Se_on(2,0x0B) FAHRT**, @0x02FDC
  `0d 00 60 00 b4 00` For 180 (obj 0 + Spieler y += 30 je Bild), danach Gosub sub23 @0x03680 (Plc_ret, Set(2,7)=0, Cut_auto 1).
  sub12 @0x030B6/@0x030BA/@0x030E4/@0x03106 Gegenrichtung. ROOMF030 dieselben Folgen (@0x02E24/@0x02E28/@0x02E52/@0x02E74).
  Andere RE2-Aufzuege (Nachrichten-Zensus "lift|elevator" ueber alle 495 RDTs): Kabinen 21B0/6010/6020/6060 (Kabinen-Ton
  liegt schon als ELEVSE.VBS vor), Strom-Schalter 6050, Riesen-Hubbuehne 6170 - die Hubplattform 6030/F030 ist die einzige
  offene Plattform mit eigenem Fahrt-Ton.
* Raumbank ROOM6030 (RDT-Kopf +0x08/+0x0C/+0x10): EDT @0x0E128, VH @0x0E1E8, VB @0x0F008.
  Satz 0x11 EDT @0x0E16C `00 01 a3 00` -> Prog 1 Ton 10 @0x0ED48 (vol 127, center 93, shift 0, note 69) -> VAG 15 @0x22F08
  2496 B, Tonhoehe 0x400 (11025 Hz) = 0,39 s Klick. Satz 0x0B EDT @0x0E154 `00 01 13 02` -> Prog 1 Ton 1 @0x0EC28 (vol 127,
  center 91, shift 42, note 60) -> VAG 22 @0x2F818 34320 B, note2pitch2 0x2C4 (7622,8 Hz) = 7,88 s. Huellkurve je Spielbild
  (254,09 Abtastwerte): Anlauf-Spitze T6, gleichmaessige Fahrt, Stopp-Schlag T200 = **20 Bilder nach dem Fahrt-Ende T180**,
  Ende T236. Die Fahrt-Welle steht nur in ROOM6030/ROOMF030 (bitgleich), im RE1.5-Baum 0 Treffer (512-B-Nadel).
* Laengen: RE2 faehrt 180 Bilder; RE1.5 Lift B ebenso 180, Lift A nur 90 -> fuer Lift A gibt es in RE2 keine Welle.

### N1-3 Umsetzung
* **(a)** `engine/src/lastenaufzug_r37.c` + `include/re15_lastenaufzug_r37.h` (NEU): Null-Rechteck in ROOM3080/3081 mit Ziel
  Stage 3 Raum 00 -> Anfrage Port-Archiv P2DS, RE2-Basis 0x2D, Variante 5, Tuer 112 (Runde 31 T112; Seite 0 = Runde 31 fuehrt
  fuer die Skript-Ankunft keine Seite). Haken `aot_common.c tuer_sequenz_anfragen` (Include + 3 Zeilen) DIREKT HINTER dem
  Kabinen-Haken, also vor der Zwischensequenz-Sperre der Runde 31 (beim Feuern ist player_mode 2, wie bei den Kabinen).
  fahrstuhl_r37.c, P27F/P27C und der Kabinen-Haken unveraendert.
* **(a, Bestand)** P2DS korrigiert: `tools/tueren/tuer_rezepte.py rezept_P2DS` nur noch Mesh 1 weg, `tuer_archiv_bauen.py --nur
  P2DS` -> `shared_assets/RE15DOOR/P2DS.DO2` 78244 B, FNV 0xC9CE31CD -> **0x4A900C54** (Tonteil/SCD/MD1 bytegleich DOOR2D),
  `gen/re15_tuer_eigen.inc` Eintrag 18, `analysis/befunde_runde33/tueren_rest/plan.json` + `archive.json` + `tuer_rest_plan.py`
  (Rezept-Text) nachgezogen. Betrifft auch S218 (die Fahrt hinauf zeigt jetzt das Fachwerk).
* **(b)** `engine/src/lift2030_r37.c` + `include/re15_lift2030_r37.h` (NEU): sechs Anker (Datei-Offset + 8-Byte-Signatur,
  ROOM2030 = ROOM2031): @0x01E30/@0x01F0A -> Satz KLICK (RE2 @0x02F90 direkt hinter Set(2,7)=1), @0x01E54/@0x01E98 -> FAHRT_KURZ
  (Lift A, 90 Bilder), @0x01F2E/@0x01F72 -> FAHRT (Lift B, 180 Bilder = RE2-Laenge; RE2 @0x02FBA direkt vor der Fahrt-
  Schleife). Haken `scd_vm.c` (Include, Scan in `scd_register_current_rdt`, Takt-Pruefung hinter dem Elza-Anker, Wachposten
  Ankerzahl = 0 ausserhalb ROOM2030/2031). For laeuft im Port einmal ueber den Anker (Next springt auf pc+6) -> ein Ton je Fahrt.
* **(b)** `platform/pc/src/lift2030_se_pc.c` (NEU, Muster elza_se_pc.c): Mini-Bank laden, `re15_audio_se_play_bank`; Haken in
  `platform/pc/main.c` (1 Zeile neben dem Elza-Haken). PSX/Tests: Haken NULL = stumm. audio_pc.c unberuehrt.
* **(b)** Mini-Bank `shared_assets/RE2/LIFT6030.VBS` (NEU, 61192 B, FNV 0x0C73F970, sha1 944a61942ac1d5f3ac940242c5bf662fb197922e)
  aus `tools/fahrstuhl_r37/re2_lift6030_cut.py --install` (Selbsttest gegen alle zitierten Bytes, ROOMF030-Gegenprobe) +
  `engine/src/gen/re2_lift6030_bank.inc`. Saetze: 0 KLICK = RE2 0x11 (bytegleich), 1 FAHRT = RE2 0x0B (bytegleich),
  2 FAHRT_KURZ. Nur Tone-Byte +0x14/+0x16 und EDT-Byte 1/2 (Lage in der Mini-Bank) geaendert.
* **PORT-WAHL (FORM aus den Daten):** FAHRT_KURZ = RE2-Fahrt-Welle ohne die Bloecke [794,1611): Kuerzung = round((180-90)
  Bilder x 254,09 / 28) = 817 Bloecke (RE2 For 180 @0x02FDC gegen RE1.5 For 45 x 2 @0x01E54); Schnittstelle = kleinste
  Abweichung des ADPCM-Decoder-Zustands (209) im Fenster zwischen Anlauf-Ende T26 und Stopp-Schlag T200 (beides Huellkurve,
  Regel im Werkzeug); Anlauf und Stopp-Schlag bytegleich. Naht: Fehler 115 im ersten, 12 im zweiten Block, dann 0; Sprung an
  der Naht 1535 (Median-Sprung der Welle 1344, 95-%-Sprung 6541). Ergebnis: Stopp-Schlag T110 = Fahrt 90 + 20 wie RE2.
  Ungekuerzt liefe der Motor 3,6 s ueber die Ankunft hinaus. Zwischen KLICK und FAHRT hat RE1.5 kein Sleep (RE2 Sleep 10
  @0x02F9C) - die RE1.5-Zeitachse bleibt, beide Toene fallen ins selbe Bild.
