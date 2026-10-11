# Runde 36 — Spur K "szene1150" — UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme-Agent 0 (2026-10-11). Baum `.claude/worktrees/r36_szene1150`, Zweig `r36/szene1150`, HEAD 0c3f17a5,
Basis 78a4a748. Massstab: Wortlaut AUFTRAG.md Z. 67-71 (K1 Tuerknall, K2 11C0-Dialog, K3 Knall -> Spawn -> Kamera).
Grundsatz: nur das zaehlt, was hier selbst am gebauten Stand gemessen wurde.

## 0. Ergebnis

| Punkt | Urteil | Beleg (Kurz) |
|---|---|---|
| K1 Tuerknall = RE2-Knall statt Tuer-zu | **erfuellt** | Lauf A: `[se] se=0 layer 0/1 vag=0 note=67 (11025 Hz)` + `Signal (5,28) ... RE2 ROOM1060 Se_on 0x0a`; eigene KK: RE2_1060 Bild 1908 r=0.934 / 2499 r=0.915, DOOR04 max 0.239; Bank-VAG == ROOM1060.RDT @0x66B8 bytegleich |
| K2 11C0 Ada/Marvin unterhalten sich | **erfuellt** | state.log-Gierungen je Zeile (Sprecher/Zuhoerer +-1 Einheit zum Partner), Bilder F120/F180/F220/F340 |
| K3 Knall -> Spawn -> Kamera | **erfuellt** | Folge #1..#14 (K < S < C je Raum, Spawn im Raumaufbau), erstes Bild je Montage-Raum mit Zombies, Bilder 1130/1040/1030 |
| RE-Gate | haelt | 6 Stellen selbst disassembliert (RE1.5 + RE2), RDT-Bytes geprueft, keine Rate-Woerter |
| Vertrag | haelt | nur eigene Dateien, keine Bits/IDs/Ereignisse ausser Zuteilung |
| Tests | gruen | 62/62 gezielt (inkl. integration_r36_szene1150, integration_r35_cut1150) |

**bestanden = true.** Keine Maengel. Hinweise (kein Mangel der Spur) unter §6.

## 0b. Stand der Abnahme (fortlaufend)
- [x] Diff gelesen
- [x] Bau
- [x] gezielte Tests
- [x] K1 gemessen
- [x] K2 gemessen
- [x] K3 gemessen (+ Variante 1140/1070 leer)
- [x] RE-Gate (6 Stellen selbst disassembliert)
- [x] Vertrag / Pfade / Nebenwirkungen

## 0a. Bau
`bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` ->
`ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (exe 02:28 = Stand HEAD 0c3f17a5, Arbeitsbaum ohne
Aenderungen an verfolgten Dateien).

## 1. Diff (Umfang)
`git diff master --stat` zeigt 86 Dateien, davon 63 Loeschungen unter `synchro/` und 15 Zeilen in AUFTRAG.md —
das sind KEINE Aenderungen der Spur, sondern master ist seit der Basis 78a4a748 weitergelaufen (565da62b
Elza-Sprachdateien, 836023f5 Nachtrag Spur J). Massgeblich ist `git diff 78a4a748...HEAD`: 23 Dateien
(irons_tod_1150.c, re15_irons_tod.h, gen/knall_bank.inc, tools/r35_l/knall_cut.py, tools/r36_k/*, Tests, Dossier,
Belegbilder). Keine Datei unter release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists.

## 2. RE-Gate — selbst nachgepruefte Belege (Bytes/Instruktionen, eigene Werkzeuge)

### 2.1 K1 Datenherkunft (eigenes Skript `scratchpad/K_abn0/vab_check.py`, parst knall_bank.inc und die RDTs)
* RE2 `info/re2leon/PL0/RDT/ROOM1060.RDT`: Kopfworte +0x08/+0x0C/+0x10 = EDT 0x3F58 / VH 0x4018 / VB 0x4C38.
  `xxd -s 0x0F40`: @0x00F48 `36 02 0a 01 00 00 dc 88 f8 f8 3d 05` (Se_on Bank 2 Satz 0x0a), danach `09 0a 1e 00`
  (Sleep 30) und `36 02 0b 01 ...` (0x0b) — wie behauptet. EDT[0x0a] @0x03F80 = `00 00 85 20`.
* VH ROOM1060: 1 Programm, 15 Tones; Tone 8 / Tone 9 = VAG 5, center 91, Feinstimmung 0 / 1, Note 67;
  VAG-Groessentabelle: VAG 5 liegt bei 0x4C38 + 48 + 3152 + 1904 + 1680 = **0x66B8**, Groesse **8768** — wie behauptet.
* knall_bank.inc (19976 B): SE-Map `00 00 05 20 | 00 00 27 15`; VH 1 Programm / 3 Tones; Bank-Tone 0/1 sind
  **bytegleich** mit RE2-Tone 8/9 bis auf die VAG-Nummer (5 -> 1); Bank-VAG 1 (8768 B, sha1 192e09df...) ==
  ROOM1060.RDT[0x66B8:+8768] **True**; Bank-VAG 2 == RE1.5 ROOM1030.RDT[0x8BA0:+8080] **True** (Satz 1 unveraendert).
* Abspielweg RE2 (`re2_disasm.py dis`, info/re2leon/PSX.EXE): `8005ba30 srl t1,a0,24`; `8005ba7c srl v0,a0,16`;
  `8005ba80 andi s7,v0,0xff`; `8005ba8c lw a1,-17544(at)` (0x800dbb78 + Bank*4); `8005ba98 lw v0,0(a1)`;
  `8005baa0 beq v0,v1` (v1 = -1, leerer Satz). Raumlader: `80059f1c lw a1,8(a2)` -> `80059f44 sw a1,-17536(at)`
  = 0x800dbb80 = Bank 2. **Bestaetigt.**

### 2.2 K3 Mechanismus RE1.5 (`re15_disasm.py dis`, info/Re1.5/PSX.EXE)
* `8001ca44 sw zero,-13760(at)` = g_pauseflags geloescht, `8001ca54 jal 0x8001d600` (Raumlader) danach. **Bestaetigt.**
* State-3-Rumpf: `8001cbcc lw t1,g_pauseflags` -> `8001cbe4 sw v0(=7),g_pauseflags` -> `8001cbfc sw t1,0x8008f628`;
  `8001cc00 jal 0x800217b0`, `8001cc18 jal 0x800216ec`; `8001cc58 or v1,v1,a0` / `8001cc5c or a0,a0,0xff000000`;
  Rueckgabe `8001cc84 lw v0,0x8008f628` -> `8001cc94 sw v0,g_pauseflags`. **Bestaetigt** (die Init-Lauf-Bits
  ueberleben im Original die Blende; der Port verliert sie — die OFFEN-Notiz des Bau-Agenten stimmt).
* Init-Lauf: `8001d988 jal 0x800396fc` (Raumlader), darin `80039a00 jal 0x8003ef6c`, darin
  `8003f018 jal 0x8003f0a0` (Dispatcher). **Bestaetigt.**

### 2.3 K2 Form = ROOM11C0 sub02/sub03 (`xxd` auf shared_assets/PSX/STAGE1/ROOM11C0.RDT)
`@0x01886 41 01 fb dc 00 00 f5 c7 64 00` | `@0x01890 40 00 09 00 fb dc f5 c7` | `@0x01898 18 05` |
`@0x018C0 41 01 cb e4 00 00 33 cf 64 00` | `@0x018CA 40 00 09 01 cb e4 33 cf` | `@0x018D2 18 06` |
`@0x018D4 2b 01 00 00 3f 00 13 00 09 0a 28 00` (Zeile + Clip 19 + Sleep 40) | `@0x01964 40 00 09 00 df f1 7a ba` |
`@0x01BB2 41 00 .. 64 00`, `09 0a 05 00`, `@0x01BC0 41 03 01 00 00 00 00 00 00 3c`, `09 0a 14 00` |
`@0x01C3E 22 05 00 00 0f 04 08 00 21 05 00 00 02 00 10 00` (Set(5,0)=0; While Ck(5,0)==0 {Evt_next}) |
`@0x01C50` dasselbe fuer Bit 1 | `@0x01712 3b 00 02 31 .. 24 96 1c 3e a0 0f 8c 0a` (Tuer-Rechteck -27100,15900,
4000x2700 -> Mitte -25100,17250) | `@0x01770 44 00 42 40 .. fb dc 00 00 f5 c7` (Ada -8965,-14347). **Alles bestaetigt.**
Adas Geste: `@0x018D4 2b 01 00 00 | 3f 00 13 00 | 09 0a 28 00 | 3f 00 13 00 | 43 00 80 00 | 09 0a 32 00` = Zeile, Clip 19,
Sleep 40, Clip 19 rueckwaerts, Sleep 50 — Port `OP_MOTION(0,19), OP_SLEEP(40), OP_MOTION(0,19), OP_FLG_REV, OP_SLEEP(50)`
deckungsgleich.

### 2.4 Rate-Woerter / Kennzeichnung
`git diff 78a4a748...HEAD -- re15_port | grep '^+' | grep -iE 'deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|getenv|...'`
-> **kein Treffer**. Neue Zeiten (Sleep 130/34/174/29, 45 Bilder Knall -> Tuer, Dialog-Sleeps) sind im Code als
PORT-WAHL (Takt der r35-Fassung bzw. "wie k_p_szene") gekennzeichnet; Orte mit @0x (Ada @0x01770, Tuer @0x01712) bzw.
"r35 M5, PORT-WAHL" (Marvin). Kein neuer Env-Schalter mit Spielwirkung.

## 3. Messung an der exe (eigene Laeufe)

Werkzeug: `scratchpad/K_abn0/lauf.sh` (eigene Messkopie `re15_pc_kabn0.exe`; Karte `probe_r35_cut1150_karte
re15_card.mcr 1130 nach10f0 ersteszene`; CONTINUE in ROOM1130 vor Irons' Tuer, `RE15_INPUT_SCRIPT=W1,A0.2,W200` =
Aktionstaste an der Tuer -> ECHTER Tuerweg 1130 -> 1150 (debug.log `[tuer] Sequenz Archiv 2 DOOR09`), Szene, Montage).
Normaler Renderer (KEIN RE15_SOFTWARE_RENDER), `RE15_WINDOW_SCALE=1`, `RE15_FRAMEDUMP`.

**Lauf A** (`RE15_EXIT_AT=480#11C0`, `RE15_FRAMEDUMP=0-480/10`, `RE15_AUDIO_CAP_SYNC=cap.raw`, STATE/NECK/SE-Logs):
exit 0 nach 114 s; debug.log `[flow] EXIT_AT: Bild 480 in Raum 11c0 erreicht`.

### 3.1 K1 — Tuerknall
debug.log Lauf A (Reihenfolge woertlich):
```
1407 [se] Stimme: se=0 layer=0 vag=0 note=67 fine=0 center=91 vol=127 pitch=0x400 (11025 Hz) -> SE-Stimme -16 (SPU 0)
1408 [se] Stimme: se=0 layer=1 vag=0 note=67 fine=1 center=91 vol=127 pitch=0x400 (11025 Hz) -> SE-Stimme -15 (SPU 1)
1409 [irons-tod] ROOM1150 Zustand 1: Signal (5,28): Tuerknall (Knall-Bank Satz 0 = RE2 ROOM1060 Se_on 0x0a)
1524 [se] Stimme: se=1 layer=0 vag=1 note=68 ...           1525 ... ROOM1130 ... Signal (5,29): Knall wie ROOM1030
1705/1706 [se] se=0 layer 0/1 vag=0 note=67 (11025 Hz)      1707 ... ROOM1040 ... Signal (5,28): Tuerknall (... RE2 ROOM1060 ...)
1861 [se] Stimme: se=12 layer=0 vag=5 note=68 center=92 ... (Original-Se_on 0x0c ROOM1030 @0x02776)
```
Eigene Kreuzkorrelation (`scratchpad/K_abn0/knall_kk.py`: EIGENER PSX-ADPCM-Dekoder, Referenzen direkt aus
ROOM1060.RDT @0x66B8, ROOM1030.RDT @0x8BA0 und der ALTEN Bank aus `git show 78a4a748:.../knall_bank.inc` (DOOR04);
normierte KK der ersten 0,4 s gegen den Mono-Mix des Mitschnitts):
```
Mitschnitt: 5249370 Abtastwerte = 119.0 s = 3571 Spielbilder (1470/Bild)
RE2_1060  t=  63.60 s Bild  1908 r=0.934 Spitze  16189 RMS   5428     <- Ende Szene 1150 (Knall vor 1130)
RE2_1060  t=  83.30 s Bild  2499 r=0.915 Spitze  18979 RMS   5515     <- Ende 1040 (Knall vor 1030 Cut 7)
R1030     t=  69.67 s Bild  2090 r=0.983 ...   R1030 t= 90.83 s Bild 2725 r=0.896 ...
DOOR04    kein Treffer >= 0.5 (max 0.239)
```
Charakter der Welle (eigene 25-ms-Huellkurve der dekodierten Referenzen): RE2_1060 1,39 s, sofort voll ausgesteuert
(32368 28672 32415 ...), Einbruch bei ~0,18 s (16384), zweiter Schlag ab ~0,2 s, Abklingen bis ~1 s; DOOR04 Door_exit
0,98 s: Klinke 21733 -> 4795, dann EIN Schliess-Thud 30242..30960 bei ~0,13 s, schnelles Abklingen.
**Urteil K1: erfuellt.** Der Tuer-zu-Ton (DOOR04 Door_exit) ist an allen Stellen weg (r <= 0,24); an seiner Stelle
spielt der RE2-Ton, den RE2 als Schlag gegen eine Tuer abspielt, hinter der Zombies stehen (ROOM1060 = Nutzer-Liste
"ROOM106 Cabin sounds - Zombies breaking in"), bytegleich aus der RE2-RDT, mit beiden Lagen wie in RE2. Einschraenkung
(kein Mangel der Spur): Hoeren am Lautsprecher ist auf diesem Rechner nicht moeglich; die Geschmacksfrage
ROOM1060-Schlag vs. ROOM2050-Bretterbruch liegt offen beim Nutzer (Dossier "Entscheidungsfrage").

### 3.2 K2 — Dialog Ada/Marvin in ROOM11C0
Lauf A, state.log Besuch 5 (ROOM11C0), eigenes Skript `state_ana.py blick` (Gierung aus `@(x,z,r)`; Abweichung der
Koerper-Gierung zur Richtung auf den Partner bzw. zur Tuer-Mitte (-25100,17250), 4096 = 360 Grad):
```
F100..F190 msg 10 Ada "What was this noise?":  Ada r=3679 zu Marvin +1   | Marvin r=2745 zur Tuer -1 (Koerper), Kopf -> Ada
F200..F250 msg 11 Marvin "Yes!....":           Ada r=3679 zu Marvin +1   | Marvin r=1631 zu Ada +1
F260..F320 msg 12 Marvin "Oh, no, Leon!":      Ada r=3679 zu Marvin +1   | Marvin r=2745 zur Tuer -1
F330..F380 msg 13 Marvin "I have to help him": Ada r=3679 zu Marvin +1   | Marvin r=1631 zu Ada +1 (Clip 15 Arm)
F400..F480 msg 14 Ada "Marvin!...":            Ada r=2763 zur Tuer -1 (Marvin rennt Richtung Tuer, -128..+67 zu ihm)
F50..F70 vor msg 10: beide zur Tuer (-1 / -1) = r35-Beat "beide drehen sich Richtung Gebaeude"
```
Bilder (`scratchpad/K_abn0/k2_a.png`, `k2_b.png`, Ausschnitt 3x `k2_zoom.png`: F90/F120/F180/F220/F300/F340):
F120/F180 Ada im Profil zu Marvin mit Geste, Marvins Kopf sichtbar zu Ada gedreht; F220 beide im Profil einander
zugewandt ("Yes!...."); F300 Marvin zum Gebaeude, Ada sieht ihn an; F340 Marvin zu Ada, Arm zu ihr; F420..F480 Ada
zur Tuer mit Arm. Vorher (Dossier K2.1, Bild k2_vor_11c0.png) sprachen beide frontal in die Kamera.
**Urteil K2: erfuellt.** Bei jeder der fuenf Zeilen ist der Sprecher dem Partner zugewandt (Koerper +-1 Einheit) bzw.
bei "Oh, no, Leon!" bewusst zum Gebaeude (r35-Beat "Marvin dreht sich [danach] zu Ada"); der Zuhoerer sieht den
Sprecher an. Kein "in die Kamera"/"irgendwohin" mehr.

### 3.3 K3 — Reihenfolge Knall -> Spawn -> Kamera
Lauf A debug.log (woertlich, Reihenfolge = Ausgabereihenfolge):
```
#1 K ROOM1150 T1284 Knall Satz 0                     (Ende Szene 1150, Kamera Cut 5 = Leon bei Irons)
#2 S ROOM1130 T1328 Spawn + 5 (Raumaufbau, vor dem ersten Bild)   #3 C ROOM1130 T1328 Kamera Cut 0 (Raumaufbau ...)
#4 K ROOM1130 T1460 Knall Satz 1                     (noch auf 1130 Cut 0)
#5 S ROOM1040 T1504 Spawn + 5 (Raumaufbau ...)        #6 C ROOM1040 T1504 Kamera Cut 1 (Raumaufbau ...)
#7 K ROOM1040 T1865 Knall Satz 0                     (noch auf 1040 Cut 1)
#8 S ROOM1030 T1909 Spawn + 11 (Raumaufbau ...)       #9 C ROOM1030 T1909 Kamera Cut 7 (Raumaufbau ...)
#10 K ROOM1030 T2084 Knall Satz 2 (Original-Se_on @0x02776 auf Cut 7)  #11 S ROOM1030 T2094 Spawn + 3  #12 C ROOM1030 T2116 Kamera Cut 12
#13 S ROOM11C0 T2451 Spawn + 4 (Raumaufbau ...)       #14 C ROOM11C0 T2451 Kamera Cut 13 (Raumaufbau ...)
```
state.log Lauf A (eigenes Skript `state_ana.py besuche`, erste Zeile jedes Montage-Besuchs):
```
Besuch 2 ROOM1130: F1 cam=0  gegner=5  pf=FF000007 ; F6 pf=01000007
Besuch 3 ROOM1040: F1 cam=1  gegner=5  pf=FF000007 ; F6 pf=01000007   (Positionen F1 = F6, erst ab F7 Bewegung)
Besuch 4 ROOM1030: F1 cam=7  gegner=11 pf=FF000007 ; F6 pf=01000007 ; F190 cam=7 gegner=14 ; F212 cam=12
Besuch 5 ROOM11C0: F1 cam=13 gegner=4 (Ada t42, Marvin t40, 2 Affen) ; F6 pf=01000007
```
Bilder (eigene Laeufe, normaler Renderer, WINDOW_SCALE=1):
* Lauf B (`RE15_EXIT_AT=170#1130`, Dump F0-170/2), `k3_1130.png` + aufgehellt `k3_1130_fade.png`: F2 schwarz, F4 Einblendung
  (Flur + dunkle Zombie-Umrisse schon da), ab F6 hell mit den fuenf Zombies vor der Briefing-Room-Tuer bis F170 —
  kein Erscheinen "aus dem Nichts" mehr (vorher laut Dossier F32).
* Lauf C (`RE15_EXIT_AT=230#1030`, Dump F0-230/2), `k3_1030_a.png`/`k3_1030_b.png`: F4 Einblendung mit den fuenf
  1070-Kopien schon an der Tuer, F6..F210 Cut 7 unveraendert (die drei Kriecher erscheinen bei F190 bei z=-24838 hinter
  dem Tor und sind auf Cut 7 NICHT im Bild), F212 Schnitt auf Cut 12: aufgebrochenes Tor, Kriecher schon da.
* Lauf D (`RE15_EXIT_AT=300#1040`, Dump F0-300/6), `k3_1040.png`: F6..F60 Cut 1 mit geschlossenem Rolltor, kein Zombie im
  Bild (sie stehen dahinter; state.log F1 = F6 Positionen, keine Versetzung mehr in Bild 6 wie r35), F96 Tor faehrt hoch,
  Fuesse sichtbar, F150..F294 die fuenf kommen durch.
**Urteil K3: erfuellt.** In jedem Montage-Schritt: Knall (noch auf der alten Kamera) -> Zombies erscheinen (im Raumaufbau
bzw. ausserhalb des Bildes) -> erst dann zeigt die Kamera die Zombies. Kein Zombie erscheint mehr auf der Kamera, die
gerade zeigt.

## 4. Tests

### 4.1 Gezielter ctest-Lauf (selbst gefahren, -j 2, Rechner unter Last durch andere Spuren)
```
ctest --test-dir re15_port/build -R '^unit_r35_cut1150_|^integration_r35_cut1150$|^unit_r36_szene1150_|
  ^integration_r36_szene1150$|^unit_1030_|^unit_room1040_switch$|^unit_cam_1030_reentry$|^unit_room1150_itembox$|
  ^unit_irons_|^unit_sicherung_1150$|^unit_r30_irons|^integration_r30_irons|^unit_montage_fx$|^probe_r16_tuer1040|
  ^unit_r35_fenster_knall$|^unit_se_bank_routing$|^unit_r34_plattform|^integration_r34n_g_schrift1150$|^unit_r35_cut10f0_|
  ^integration_r35_cut10f0$|^unit_marvin_|^unit_neck_headlook$|^unit_door_seq$|^integration_room_transition$|
  ^unit_r30_tuerse_bank$|^unit_r35_affen_szene$|^unit_r35_affen_ada$' --timeout 900 -j 2
100% tests passed, 0 tests failed out of 62        Total Test time (real) = 547.13 sec
```
(= die 54 des Bau-Agenten + unit_marvin_*, unit_neck_headlook, unit_door_seq, integration_room_transition,
unit_r30_tuerse_bank, unit_r35_affen_szene/_ada als Nachbarn der beruehrten Raeume 11C0/Tuer-Wege.)

### 4.2 Tests je Punkt (messend?)
* K1 `unit_r36_szene1150_knall_re2`: prueft die RE2-Bytes (@0x0F48 Se_on, EDT @0x03F80) und den bytegleichen VAG-Vergleich
  Bank <-> RDT @0x066B8, Lagen/Tones, "nicht 9216 B (DOOR04)". Messend (gegen die Original-Datei), gruen.
* K3 `unit_r36_szene1150_folge`: ganze Kette ueber die Tuer-Anforderungen, Faelle A/B/C (alle leben / 1140+1070 leer /
  Tor offen): Knall < Spawn < Kamera je Raum, Spawn+Kamera im Raumaufbau, kein Zuwachs auf der zeigenden Kamera. Gruen.
* K2 `unit_r36_szene1150_dialog11c0`: je Zeile Koerper im Ausrichtungskegel (re15_ai_arc_test) + Kopf auf dem Partner. Gruen.
* `integration_r36_szene1150`: echte exe ueber den Tuerweg; Logzeile K1, Folge-Indizes K3, state.log erstes Bild je Raum,
  pf in Bild 10, neck.log/Plc_dest K2. Gruen (im Lauf 4.1).
* Riegel r35 (`test_r35_cut1150.c`) an die neue Reihenfolge angepasst (Diff gelesen): Erwartungen folgen dem neuen Ablauf,
  keine Pruefung ersatzlos gestrichen; zwei wurden strenger (1040 "18 tot": Raum- UND Auffuell-Records getrennt gezaehlt;
  Nachspawn 1030: die 7 Montage-Records ueber em_flag_id statt "+7 oder 15").
* Generator reproduzierbar: Kopie von `tools/r35_l/knall_cut.py` mit Ausgabe in den Scratch-Ordner ->
  `cmp knall_bank_regen.inc re15_port/engine/src/gen/knall_bank.inc` -> **IDENTISCH**.

## 5. Vertrag / Pfade / Nebenwirkungen
* Pfade (`git diff 78a4a748...HEAD --name-only`): nur irons_tod_1150.c + re15_irons_tod.h (Eigentum K laut VERTRAG §1.4),
  gen/knall_bank.inc + tools/r35_l/knall_cut.py (Generator dieses Moduls), tools/r36_k/*, tests/unit/probes/r36_szene1150.cmake,
  tests/unit/test_r36_szene1150.c, tests/integration/test_r36_szene1150.cmake, tests/unit/test_r35_cut1150.c (Riegel des
  eigenen Moduls), Dossier + Belegbilder. KEINE gemeinsame Datei (scd_vm.c, scd_room_setup.c, room_common.c, main.c ...),
  nichts unter release/, platform/android/, shared_assets/PSX/, keine tests/*/CMakeLists.txt. **Haelt.**
* Bank-9-Bits: keine neuen (Diff: nur Lesen von (9,74)/(9,76) = r35-Bits). Nachrichten-IDs: 10..14 in ROOM11C0 wie r35,
  keine neuen. Ereignis: nur 21 (K). Bank 5 Bits 0/1 in ROOM11C0 = die raumlokalen Ankunfts-Bits der Original-Subs
  sub05/sub06 (Bytes oben). **Haelt.**
* Integrationshaken: Messkopie `re15_pc_r36k_haken_<id>.exe` (danach geloescht), Arbeitsordner `r36_szene1150_lauf/lauf_<id>`
  (kein Target-Name), RE15_WINDOW_SCALE=1. **Haelt.**
* Port-Abweichung Pause-Wort (Dossier OFFEN) selbst nachgesehen: room_common.c:134-140 `re15_room_transition_present()`
  ruft `re15_pauseflags_clear()` und sichert DANACH (`s_trans_pause_saved = g_re15_pauseflags`) — im Original liegt der
  Loesch-Store @0x8001ca44 vor dem Raumlader (`jal 0x8001d600` @0x8001ca54), die Sicherung @0x8001cbcc nach dem Laden
  (Disasm oben). Befund korrekt, sauber unter OFFEN mit Adressen und naechstem Schritt (Zensus Init-Lauf-Set(2,x)); die Spur
  umgeht ihn im eigenen Modul (Sleep 1 vor den Riegeln: Sleeping @0x8003f428 zaehlt herunter und gibt bei 0 mit
  `ori v0,zero,0x2` @0x8003f48c ab — selbst disassembliert). Gemessen: pf=01000007 ab Bild 6 in allen vier Montage-Raeumen.
* Elza-Szenario: das Modul schaltet nur auf ROOM1150/1130/1040/1030/11C0 (Leon-RDT-Nummern, `re15_irons_tod_install`
  switch Z. 1217-1292); die xxx1-Raeume sind nicht beruehrt.
* Variante (Lauf E, Karte `... tot1140 tot1070`, `RE15_EXIT_AT=480#11C0`, Mitschnitt): Folge
  `#1 K ROOM1150 Satz 1 -> #2 S ROOM1040 +5 (Raumaufbau) -> #3 C ROOM1040 Cut 1 -> #4 K ROOM1040 Satz 1 ->
  #5 S ROOM1030 +9 (Raumaufbau: 6 Raum + 3 Kriecher) -> #6 C ROOM1030 Cut 12 -> #7/#8 ROOM11C0`; Mitschnitt: R1030 Bild
  1909/2319, RE2_1060 kein Treffer (max 0.100, richtig: keine Tuer mit Zombies), DOOR04 kein Treffer (max 0.244).
  Reihenfolge haelt auch hier. 11C0-Rand: Marvin + Ada ab Bild 1.
* Bank-5-Bits 0/1 in ROOM11C0 (eigene Bytesuche ueber die RDT): Set/Ck(5,0)/(5,1) nur in sub05 @0x01C3E/@0x01C46 und
  sub06 @0x01C50/@0x01C58; die Plc_dest-Saetze mit Bit 00/01 liegen alle in den Leon/Ada-Szenen-Subs, die waehrend der
  Montage nicht laufen (sub01 gesperrt, Leon nicht im Raum). Kein Konflikt.

## 6. Hinweise an den Orchestrator (keine Maengel der Spur)
1. **Port-Abweichung Pause-Wort (spielweit, room_common.c):** `re15_room_transition_present()` loescht `g_re15_pauseflags`
   NACH dem Raumladen; Original @0x8001ca44 (Loeschen) vor @0x8001ca54 (`jal 0x8001d600`), Rettung @0x8001cbcc ->
   @0x8001cbfc -> @0x8001cc84/@0x8001cc94 (selbst disassembliert, §2.2). Jeder Init-Lauf-Set(2,x) eines Raums geht im Port
   verloren. Eigene Spur noetig (Zensus Init-Lauf-Set(2,x) ueber alle RDTs, Fix + Pin); Spur K umgeht es im Modul.
2. **K1 Hoerprobe:** nur der Nutzer kann am Lautsprecher entscheiden, ob ihm der RE2-ROOM1060-Schlag oder die Alternative
   ROOM2050 0x0b (Bretter-Barrikade bricht) besser gefaellt; Umstellung = ein Eintrag in `tools/r35_l/knall_cut.py`.
3. Die Belegbilder dieser Abnahme liegen im Scratch-Ordner der Abnahme-Sitzung (`scratchpad/K_abn0/`: k2_a/k2_b/k2_zoom,
   k3_1130/k3_1130_fade, k3_1030_a/b, k3_1040); gleichwertige Bilder des Bau-Agenten liegen versioniert in `K_belege/`.
4. Nach dem Zusammenfuehren in den Integrationsbaum: der Haken `integration_r36_szene1150` und `integration_r35_cut1150`
   haengen an der echten exe (115 s bzw. mehrere Minuten) — unter Last mit Timeout 900 fahren.
