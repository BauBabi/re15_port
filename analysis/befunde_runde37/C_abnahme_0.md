# Runde 37 — Spur C "fahrstuhl": Abnahme 0 (unabhaengig)

Stand: FERTIG (fortlaufend geschrieben). Baum `.claude/worktrees/r37_fahrstuhl`, Zweig `r37/fahrstuhl`,
Bau-Stand df341fa5 (seit 11:46 liegt df9511a5 = Nachtrag 5, nur AUFTRAG.md, darueber), Basis f1e019ca
(merge-base mit master 31c66b57).

## 0. Gelesen
* AUFTRAG.md (#6/#7 im Baum; master hat Nachtraege 2-4c fuer andere Spuren), VERTRAG.md, Dossier C_fahrstuhl.md.
* `git diff master...HEAD` (merge-base): 39 Dateien, Code: aot_common.c (+4), fahrstuhl_r37.c (neu, 122),
  re15_fahrstuhl_r37.h (neu), re15_door_seq.h (Kommentar md1_eigen 2), gen/re15_tuer_eigen.inc (Eintraege 31/32,
  6 Zeilen S013/S040/S057 -> eigen 31), Assets RE15DOOR/P27F.DO2 + P27C.DO2, Tests (probe_r37_fahrstuhl.c,
  probes/r37_fahrstuhl.cmake, test_r37_fahrstuhl.cmake), Riegel r33/r36 angepasst, plan.json Runde 33.
  (`git diff master` zweipunktig zeigt AUFTRAG/UEBERGABE/orchestrator als "geloescht" - das ist master-Fortschritt
  nach der Basis, kein Eingriff der Spur.)

## 1. Bau + Tests (selbst gefahren)
* `bash re15_port/tools/local_build.sh configure` + `build`: `LOCAL-BUILD-OK`, ninja "no work to do" - der
  Baum war schon gebaut; geprueft: aot_common.c (11:37:36) ist VOR seinem Objekt (11:37:38) gespeichert, der
  Haken `re15_fahrstuhl_r37_ausstieg` steht in der Quelle, `P27C` im exe-String - die exe ist der Stand HEAD.
* `ctest -R "unit_r37_fahrstuhl" -V`: 3/3 gruen. Ausgaben (eigene Zeilen):
  * archiv: DOOR27 Mesh 0/1 Fugenkante z-Spanne 618/619 (Zaehne); P27F/P27C Mesh 0/1 z 1140..3000 / -3000..-1140,
    Spanne 0, x 100, y -6600..0, 74 Dreiecke; V0 271 Bilder = DOOR27, z-Weg -2240/+2240; Vorderseite 128/128 mit
    Eckfarbe 73; gezeigt gegen gemalt max 1.6 (P27F) / 3.3 (P27C).
  * einstieg: ROOM1040/10C0/1120 -> ROOM1080 je 1 Anfrage DOOR27 V0 P27F S013/S040/S057.
  * ausstieg: 1080 sub07/08/09 -> 1040/10C0/1120 P27C S027/S028/S029 (player_mode 2 beim Feuern), 1081 sub08 ->
    10C1, 4020 sub07/08/09 -> 4000/5000/50C0 DOOR27 V0 RE2-Datei S221/S222/S223, 4021 sub09 -> 50C1; ohne Fahrt
    ((3,0x37)) sub08 und sub10 -> P27C S028 in player_mode 0; Gegenprobe normale Tuer 10C0 in player_mode 2 ->
    "Laeufer 0 x" (Sperre der Runde 31 bleibt).
* Gezielt, Regex `tuer|door|aot|fahrstuhl|elev|unit_r31_|r32_unterteilung|integration_headless_run|r34_plattform_ton|unit_tor_1170|r35_spiegel`
  (`--timeout 900 -j 2`): **100% tests passed, 0 tests failed out of 64** (u.a. integration_r37_fahrstuhl 171 s,
  integration_r35_karte_fahrstuhl 172 s, integration_r36_tueren 51 s, unit_r33_tueren_*, unit_r31_*,
  unit_fahrstuhl_1080/4020_etagen, unit_aot_*, integration_door_traversal/lock, integration_keydoor).

## 2. Messungen an der exe (eigene Messkopie `re15_pc_c_abn0_mess.exe`, Ordner im Scratchpad `abn0/`)

Messhelfer: Spielstand-Werkzeug `probe_r35_karte karte` (wie integration_r35_karte_fahrstuhl), Start mit
`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`, Eingabe `RE15_INPUT_SCRIPT` (BASIS spiel, START 60),
Bilder `RE15_TUER_SERIE` (jedes Tuerbild) + `RE15_FRAMEDUMP` (RE15_WINDOW_SCALE=1), Ton `RE15_AUDIO_CAP_SYNC`.

### M1 Punkt 1 - Einstieg 2F ueber den echten Weg (Spielstand vor der Tuer, Aktionstaste)
Karte: `karte re15_card.mcr 10C0 1500 7000 1024 besucht:10C0:1500:7000 flag:4:243`, Skript `W1,A0.3,W1,A0.3`:
```
[save] CONTINUE: resumed in room 10c0 (hp=100)
[aot] DOOR FIRE slot=1 rect=(1500,6400,hw=1000,hh=500) target_cut=0 spawn=(-13650,0,-900)
[tuer] Sequenz Archiv 2 DOOR27 Port-Archiv P27F Variante 0 Bit7 0 Tuer 39 Seite S040 T017 Spender FF (10 Skripte)
[tuer] Bild 70 Se_on Tonkopf-Eintrag 0
[tuer] Sequenz fertig: 271 Bilder + 1 Warten, 1 Se_on, Schliesston 1, Ton geladen, Notizen 0x0
[room] PC loaded room1080.rdt (177696 bytes)
```
Serie (32 Abdunkel- + 271 Tuerbilder) gesichtet: Bild 20 zwei glatte, dunkle graugruene Fluegel mit kaum sichtbarer
gerader Mittelfuge vor Schwarz, kein Bedienkasten, keine Zaehne, keine Schilder; Bild 80..170 schieben die Fluegel
aus der Mitte auseinander, Kamera faehrt durch, ab ~210 schwarz; danach Kabine ROOM1080 mit Leon.
Vergleich mit dem Bild VOR der Tuer (FRAMEDUMP Bild 80 in ROOM10C0) und den gemalten Hintergruenden (probe_bg_dump
STAGE1: ROOM10C00/11200/10405/10800): die gemalte Revier-Fahrstuhltuer ist eine glatte graue Stahltuer in einer
Nische, die Kabinentuer (ROOM10800) zweifluegelig mit Mittelfuge - die Sequenz zeigt diese Gestalt, nicht mehr die
oliv-verzahnte Labortuer (Gegenbild S215 = DOOR27 im selben Lauf).
Seitenhaken `RE15_TUER_SEITE=S013,S040,S057,S215` (+BOGEN): S013/S040/S057 je `Port-Archiv P27F`, 271 Bilder;
S215 (Labor) weiter `DOOR27 V0` ohne Port-Archiv.

### M2 Punkt 2 - Ausstieg ueber den ECHTEN Bedienweg der Kabine (Panel, Cursor, Knopf 3F, Fahrt)
Karte in der Kabine vor dem Panel: `karte re15_card.mcr 1080 -12050 -670 3072 ... flag:4:243` (Standplatz wie
probe_elevator_1080_full), Skript `W1,A0.1,W1,U0.7,W0.5,A0.1` (Panel auf, Cursor 21 Bilder hoch = Zelle 6 = 3F, Knopf):
```
[save] CONTINUE: resumed in room 1080 (hp=100)
[input-script] Tick 30 -> F90 Tasten 0x8000      [scd F91] Cut_chg(1)            (Panel)
[input-script] Tick 63 -> F123 Tasten 0x0010     (Cursor hoch)
[input-script] Tick 99 -> F159 Tasten 0x8000     [scd F161] Cut_chg(4) ... [scd F176] Cut_chg(0)   (Fahrt)
[scd] Plc_dest(slot=0 mode=0x04 dest=(-13650,-600) flag=32)
[aot] DOOR FIRE slot=2 rect=(0,0,hw=0,hh=0) target_cut=0 spawn=(1300,0,7300)
[tuer] Sequenz Archiv 2 DOOR27 Port-Archiv P27C Variante 0 Bit7 0 Tuer 39 Seite S029 T018 Spender FF (10 Skripte)
[tuer] Bild 70 Se_on Tonkopf-Eintrag 0
[tuer] Sequenz fertig: 271 Bilder + 1 Warten, 1 Se_on, Schliesston 1, Ton geladen, Notizen 0x0
[room] PC loaded room1120.rdt (86900 bytes)
```
Bilder: Kabine (Leon geht zur Tuer) -> Abdunkeln -> helle Kabinentuer P27C (zwei Fluegel, Mittelfuge, oben heller)
-> Fluegel schieben auf -> schwarz -> ROOM1120 (3F) mit Leon vor dem Fahrstuhl.
Ton: RMS je Spieltakt der Aufnahme (1470 Stereo-Frames/Takt): Takt 737..808 = 125, 317, 1248, 2337, 1693, 1850,
1923, 2261, 2976 ... - BITGLEICH dem Profil derselben Sequenz allein (`RE15_TUER_ARCHIV=P27C:0`, Takt 51..122:
125, 317, 1248, 2337, 1693 ...); Probe auf Abtastwerte: die 72 Takte (211680 Werte) sind identisch (max. Abweichung 0).
Der Schiebeton der Tuer laeuft im echten Ausstieg also wirklich.
Nebenbeobachtung (nicht Spur C, Ursache nicht untersucht): mit 3 Bildern gehaltener Knopftaste startet sub09 zweimal
(`thread start slot=10/11`, Cut_chg/Plc_dest/DOOR FIRE je doppelt); es bleibt bei EINER Sequenz und einem Raumwechsel.

### M3 Gegenprobe am Stand VOR der Spur (exe des Hauptbaums, master hat seit f1e019ca keine re15_port-Aenderung)
Kopie als `re15_pc_c_abn0_vorher.exe` im Baum-build (danach geloescht), gleicher Spielstand + gleiches Skript wie M2:
```
[save] CONTINUE: resumed in room 1080 (hp=100)
[aot] DOOR FIRE slot=2 rect=(0,0,hw=0,hh=0) target_cut=0 spawn=(1300,0,7300)
[room] PC loaded room1120.rdt (86900 bytes)          <- KEINE [tuer]-Zeile: Befund #7 am alten Stand reproduziert
```
`RE15_TUER_SEITE=S040` am alten Stand: `[tuer-seite] S040 ROOM10C0 DOOR27 V0 Spender FF` (RE2-Labortuer, ohne Port-Archiv).

### M4 weitere Wege (neuer Stand)
| Lauf | Weg | Ergebnis (debug.log) |
|---|---|---|
| r6a | Spielstand ROOM1040 (-21786,-10500) rot 3072, Aktionstaste | `Port-Archiv P27F ... Seite S013 T011` -> room1080 |
| r6b | Spielstand ROOM1120 (1300,6400) rot 1024, Aktionstaste | `Port-Archiv P27F ... Seite S057 T018` -> room1080 |
| r5 | Spielstand Kabine mit (3,0x37)=1 (Kabine steht auf 2F), Panel, Cursor R0.1/D1 auf "Tuer auf" (Zelle 7), Knopf | `DOOR FIRE slot=1` -> `Port-Archiv P27C ... Seite S028 T017` -> room10c0 (Ausstieg OHNE Fahrt) |
| r4 | Spielstand Labor-Kabine ROOM4020 (-20450,-25230) rot 1024, Panel, Cursor R0.2/U0.1 (Zelle 5), Knopf | Fahrt, `Plc_dest (-21400,-23400)`, `DOOR FIRE slot=1` -> `DOOR27 Variante 0 ... Seite S222 T115`, 1 Se_on -> room5000 |
| r3e | Elza (PSELECT, Elza-Bit=1), GOTO 1081, FORCE_EVENT 9 (= Knopf 3F) | `Port-Archiv P27C ... Seite S029 T018`, 1 Se_on -> room1121, Elza vor dem Fahrstuhl |
Nach jeder Ankunft: `letterbox closed -> gameplay (player_mode=0)` bei Bild 15, der Spieler laeuft auf Eingabe
(r2b: (1300,7300) -> (1300,8875); r4: (-10550,-14750) -> (-10379,-14609)) - kein haengender Zwischensequenz-Zustand.

## 3. RE-Gate (Stichproben SELBST disassembliert / Bytes selbst gelesen)
1. **PSX.EXE Aot_on @0x800407bc** (`re15_disasm.py dis 0x800407bc`): `lbu v0,1(v0)` (Platz) -> `lw v1,0(at)` aus
   0x800ac9b0[Platz], `andi v0,v0,0x80` -> Nutzlast +20/+12, `lw v0,0(at)` aus 0x8007469c[sce], **`8004082c: jalr v0`**.
   `table 0x8007469c`: [2] = 0x800430bc; `dis 0x800430bc`: `sw a0,-13912(at)` (=0x800ac9a8) + `sb v0,21337(at)`
   (=0x800b5359 := 1) = derselbe Tuer-Handler wie beim Betreten. Behauptung B1 haelt.
2. **RE2 ROOM6010.RDT** (info/re2leon/PL0/RDT, xxd + r30_re2_scd.py aus dem Hauptbaum): @0x004C2
   `3b 00 01 31 00 00 7a ae 5e 9d 08 07 3c 0f 26 b7 00 00 3c fb 00 00 05 02 04 00 27 00` -> Byte 26/27 = 27 00
   (DOOR27 V0), Rechteck x -20870..-19070 / z -25250..-21350; sub02 @0x006F6 `22 02 07 01`, @0x006FA `22 01 1b 01`,
   @0x00740 `46 00 01 21 26 b7 00 00 3c fb` (Aot_reset aot 0 sce 1 sat 0x21), @0x0075A `40 00 04 20 e0 b1 52 a4`
   (Plc_dest (-20000,-23470) = IM Tuerrechteck), @0x00762 Evt_end - kein Ruecksetzen von (2,7)/(1,0x1B) dazwischen.
   Behauptung B2 haelt.
3. **RE2 EXE @0x800142e8** (ghidra_re2_Leon.txt Z. 78902): `li a3,0x44` x3 + SetBackColor, Zweig bei
   `lhu v0,0x130(s1); andi 0x1000` (sonst 0x88) = "BK 68 ohne Flag 0x1000". Haelt.
4. RE1.5-RDT-Bytes: ROOM1080 @0x00482/4A2/4C2 (Platz 0/1/2 Null-Rechteck -> 1040 (-21936,0,-11000) Cut 5 / 10C0
   (1450,0,7300) / 1120 (1300,0,7300)), sub08 @0x007BE `22 02 07 01`, @0x007C2 `22 01 1b 01`, @0x007F8
   `40 00 04 20 ae ca a8 fd`, @0x0080E `47 01`; ROOM10C0 @0x00E82 w `d0 07` = 2000, @0x00E66 `21 04 f3 00`,
   @0x00FEE `22 03 37 01`; ROOM1120 @0x00CB6 w 2000; ROOM1040 @0x01096; ROOM4020 @0x0063A/65A/67A (Stage 3/4/4,
   Raum 00/00/0C) und Aot_on-Bytes @0x00990/A36/ACC/AE0/AF0/B00 = `47 0N`. Alle Offsets im Code stimmen.
* Konstanten im Diff: fahrstuhl_r37.c (Offsets, Ziel-Stage/-Raum, Archiv 0x27, Variante 0) mit Beleg; Seiten-/
  Tuernummern = Runde-31-Nummern (nur Protokoll). Werkzeug-Konstanten: OEFFNUNG 2000 (@0x00E82/@0x00CB6), MASSSTAB
  6602/3549 (Runde 31 tueren_01_zensus.md Z. 16/216), OBJ_Z 3000 (DOOR27 Skript 1), FLUEGEL_H 6600 (RE2-Standardblatt),
  FUGE 0,75 (gemessen am gemalten Bild), DICKE 100 / KANTE 0,6 / Teilung 4x8 als PORT-WAHL gekennzeichnet.
* Suche im Diff nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/geschaetzt: kein Treffer; kein
  neuer getenv-Schalter in Engine/Plattform.
* Werkzeug reproduzierbar: `fahrstuhl_archiv_bauen.py --pruefen` ergibt dieselben FNV 0x5608136D / 0xB8E6D960.
* Fix erklaert den Befund: alter Stand = Null-Rechteck-Tuer hat keine Tabellenzeile und die Fahrt setzt
  (2,7)/(1,0x1B) -> player_mode 2 -> Sperre; der Haken fragt die Kabinen-Ausstiege vor Zuordnung + Sperre an (M3 alt
  ohne, M2 neu mit Sequenz).

## 4. Vertrag / Pfade / Nebenwirkungen
* Keine Bank-9-Bits, keine Nachrichten-Ids, keine Ereignisse/Slots belegt.
* Pfade (`git diff master...HEAD --name-only`): nichts unter release/, platform/android/, shared_assets/PSX/, keine
  CMakeLists.txt. Neue Assets nur shared_assets/RE15DOOR/P27F.DO2 + P27C.DO2 (§1.6 erlaubt). Neue Logik in neuer
  Datei fahrstuhl_r37.c + re15_fahrstuhl_r37.h (§1.4). Tests nur ueber probes/r37_fahrstuhl.cmake; die
  Integrations-cmake hat `cmake_minimum_required(VERSION 3.16)`, Messkopie der exe unter eigenem Namen.
* aot_common.c ist laut §1.4 eine Haken-Datei der Spur D; Spur C setzt dort 1 Include + 2 Kommentarzeilen + 1 Codezeile (Kommentar
  "Runde 37 Spur C") = kleiner Haken. Probe-Merge (`git merge-tree`) r37/fahrstuhl mit r37/itemband, beretta, pri1100,
  sperre1050: sauber (D: Include nach re15_item_modal.h + Haken in re15_aot_scan - andere Stellen). Konflikte nur in
  AUFTRAG.md (Nachtraege des Orchestrators in mehreren Zweigen) - Doku, kein Code.
* Fremde Riegel angepasst (unit_r36_tueren_fahrstuhl, integration_r36_tueren, unit_r33_tueren_archive): nur die
  Erwartung "S013/S040/S057 = RE2-Datei" -> "Port-Archiv P27F"; die Maschinen-/Ton-/SCD-Pruefungen bleiben; die
  MD1-Pruefung fuer md1_eigen 2 liegt in unit_r37_fahrstuhl_archiv (z-Spanne der Fuge, Masse, Mesh 2 bytegleich).
* analysis/befunde_runde33/tueren_rest/plan.json (Eingabe des Runde-33-Bauwerkzeugs) auf P27F nachgezogen, damit ein
  voller tuer_archiv_bauen-Lauf die Wahl nicht still zurueckdreht - nachvollziehbar.
* Paket: make_package.sh kopiert RE15DOOR ganz und vergleicht jede *.DO2; `release/apk_asset_gate.py --quellbaum`
  im Baum: `Port-Tuerarchiv ... 32/32 wie die Engine-Tabelle (Groesse, Aufbau, FNV-1a)` ->
  `APK-ASSET-GATE-QUELLBAUM-OK`. Ein eigener Gate-Eintrag je Datei ist nicht noetig. Hinweis fuer die Integration:
  neue Quelldatei fahrstuhl_r37.c -> Android-GLOB-Cache (app/.cxx) beim Paketbau neu konfigurieren.
* Nebenwirkungen: der Haken greift nur bei Null-Rechteck UND Raum 0x108x/0x402x UND Ziel aus der Tabelle; normale
  Tueren in player_mode 2 bleiben ohne Sequenz (unit-Gegenprobe "Laeufer 0 x"). Gezielte Suite 64/64 gruen.

## 5. Waehrend der Abnahme neu: AUFTRAG.md Nachtrag 5 (Commit df9511a5, 11:46, NACH dem Bau-Stand df341fa5 11:43)
Wortlaut (AUFTRAG.md, massgeblich): "ja, dann mache den noch. Mir ist auch aufgefallen in den Stage2 kleineres
aufzügen fehlt noch das sound. Den sollte man auch aus Resident Evil 2 extrahieren." Lesart des Orchestrators: (a)
Ausstieg Lastenaufzug ROOM3080 -> ROOM4000 bekommt eine Tuersequenz (RE2 herleiten), (b) kleinere Aufzuege in STAGE2
bekommen den RE2-Aufzugton. "ja, dann mache den noch" beantwortet die Entscheidungsfrage des Bau-Dossiers.
* (a) gemessen am neuen Stand: `RE15_DEBUG_JUMP=3080@120` (+Titel-Haken) ->
  `[debug-menu] JUMP -> 308 WAREHOUSE LIFT (ROOM3080)`, `[room] PC loaded room3080.rdt`,
  `[aot] DOOR FIRE slot=0 rect=(0,0,hw=0,hh=0) target_cut=1 spawn=(-7600,0,-6760)`, `[room] PC loaded room4000.rdt`
  - KEINE [tuer]-Zeile. ROOM3080.RDT main00 @0x0072A Platz 0 Null-Rechteck -> Stage 3 Raum 00 Cut 1, Aot_on 0 @0x00A22
  (sub02, von sub00 @0x007E8 gestartet) und @0x00BFA (sub09) - der Haken der Spur kennt nur 0x1080/0x4020. Offen.
* (b) Der Diff enthaelt keine Ton-/Aufzug-Aenderung ausserhalb ROOM1080/4020 (nur aot_common-Haken, fahrstuhl_r37.c,
  Tuertabellen, P27F/P27C). Offen; die Raeume (STAGE2 = Kanal: L TUNNEL ... SERVICE TUNNEL, keine eigene
  Aufzug-Raumkarte) muss der Nachbau erst bestimmen.

## 6. Urteile
| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| #6 Fahrstuhltuer in der Tueranimation korrekt (sonst neu bauen) | **erfuellt** | Alle RE2-Tuerarchive gesichtet (Bogen des Bau-Agenten + selbst die 6 leeren Kacheln ueber ihre Texturen info/re2leon/COMMON/DOOR/DOORxx_00nn.bmp: DOOR20/21/32/34/36 = Platzhalter-Holztextur, DOOR28 = rostiger Industrie-Lift mit Knopfleiste) - keine glatte graue Revier-Fahrstuhltuer -> gebaut. Echter Weg 1F/2F/3F (M1, M4 r6a/r6b): `Port-Archiv P27F`; Bild = zwei glatte graue Fluegel, gerade Mittelfuge, kein Kasten/keine Zaehne; gezeigt gegen gemalt selbst nachgerechnet max 1.4 (P27F, S040) / 5.2 (P27C, S028); alter Stand: DOOR27 (M3). |
| #7 Tuersequenz beim Aussteigen | **erfuellt** | Echter Panel-Weg 1080 -> 1120 mit Fahrt (M2): `Port-Archiv P27C ... S029`, Schiebeton bitgleich der Einzelsequenz; ohne Fahrt per "Tuer auf" (r5) P27C S028; Labor 4020 -> 5000 per Panel (r4) DOOR27 V0 S222 mit Ton; Elza 1081 -> 1121 (r3e) P27C S029; danach spielbar. Alter Stand ohne Sequenz (M3). |
| Nachtrag 5a Ausstieg Lastenaufzug ROOM3080 | **nicht erfuellt** (kam nach dem Bau-Stand) | 3080 -> 4000 ohne [tuer]-Zeile (Abschnitt 5). |
| Nachtrag 5b Ton der kleineren STAGE2-Aufzuege | **nicht erfuellt** (kam nach dem Bau-Stand) | keine Aenderung dafuer im Diff (Abschnitt 5). |

Gates: RE-Gate haelt (3 Stichproben + alle RDT-Offsets selbst gelesen), Vertrag haelt, gezielte Tests 64/64 gruen,
Tests je Punkt vorhanden und messend (Gegenprobe am alten Stand M3 zeigt genau den Befund).

## 7. Maengel (nummeriert)
1. **Nachtrag 5a offen**: ROOM3080 (WAREHOUSE LIFT) -> ROOM4000 spielt beim Aussteigen keine Tuersequenz
   (Null-Rechteck @0x0072A, Aot_on @0x00A22/@0x00BFA; Elza-Datei ROOM3081 ist eine ANDERE Datei: Satz ebenfalls
   @0x0072A, Aot_on @0x0081E/@0x00862 - nicht gefahren). Nachpruefbar: `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
   RE15_DEBUG_JUMP=3080@120 RE15_EXIT_AT=60#4000` -> debug.log ohne `[tuer] Sequenz`. Soll: RE2-Herleitung
   (Hubbuehne DOOR2D bzw. der passende RE2-Aufzugausstieg) mit @0x-Belegen, Riegel wie unit_r37_fahrstuhl_ausstieg.
2. **Nachtrag 5b offen**: Ton der kleineren Aufzuege in STAGE2 fehlt (Nutzer); kein Code dafuer im Diff. Soll: Raeume
   bestimmen, RE2-Ton belegen (RE2_SOUND_LISTE.md zuerst), Riegel mit Tonnachweis (RE15_AUDIO_CAP_SYNC).
Zu #6/#7 KEIN Mangel - der Nachbau soll P27F/P27C, fahrstuhl_r37.c und die Riegel unveraendert lassen.

## 8. Hinweise (kein Mangel, fuer Nutzer/Integration)
* Gestalt der Fluegel ist PORT-WAHL (es gibt kein Original-Modell); Masse/Farben belegt bzw. gemessen.
* 1F (S013): die gemalte Tuer in ROOM10405 ist blau beleuchtet (Median 21,39,44), die Sequenz zeigt das graue
  Fluegelfeld aus 2F/3F (gezeigt 27,32,27) - wie bei RE2 steht jede Tuer unter festem Tuerlicht; Nutzer ggf. fragen.
* Rahmen/Nische und die Rufknopfleiste an der Wand sind nicht Teil der Sequenz (wie bei allen RE2-Tuersequenzen).
* Beobachtet, nicht Spur C (Ursache nicht untersucht): mit 3 Bildern gehaltener Knopftaste startet sub09 im Port
  zweimal (`thread start slot=10/11`, ein Bild versetzt); es bleibt bei genau einer Sequenz und einem Raumwechsel.
* Integration: neue Quelldatei -> Android-GLOB-Cache neu konfigurieren; AUFTRAG.md-Konflikte beim Mischen (Doku).

Stand: FERTIG (Abnahme 0). Messkopie der exe danach geloescht.
