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

(folgt)

## Messung nachher

(folgt)

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
