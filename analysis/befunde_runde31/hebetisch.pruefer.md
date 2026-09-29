# Gegenpruefung r31/hebetisch (ROOM1170-Hebetisch-Items + ROOM11F0-Generatorzeiger)

Pruefer-Dossier. Stand: laufend gefuellt. Pruefer aendert KEINEN Spielcode.

## 0. Auftrag (Nutzer, woertlich, relevanter Teil)
"Außerde baue mir danach room 1170 die items im model an, und packe mir die Granate links in die
hochfahrende Box und die Sicherung rechts. Außerdem starte mit dem Aufnahme Dialog der items erst
wenn das Modell wirklich komplett hochgefahren ist. gleiches in room 11f0 bei den generator rätsel.
warte erst bis der zeiger final auf 80 steht, bevor du mit ok das abnimmst, das Licht anschaltest etc."

Pruefpunkte:
- P1 Items (Granate links, Sicherung rechts) sichtbar IM Modell der hochfahrenden Box ROOM1170
- P2 Aufnahme-Dialog erst, wenn Modell komplett oben
- P3 ROOM11F0: Zeiger final auf 80, erst dann OK/Abnahme/Licht
- P4 Faelle, die der Bauer nicht gefahren ist (Lade-Weg, zweite Fahrt, No, xxx1, Wiedereintritt, Save/Load)
- P5 Konstanten mit Beleg (@0x / Datei-Offset / PORT-WAHL)
- P6 Tests pruefen Ergebnis, nicht nur Absicht
- P7 Regressionen Nachbarthemen

## 1. Bauer-Stand (Commits, Dossier)

Basis 7cb74897 (merge-base mit r30/integration; r30/integration ist inzwischen auf cade0796 weiter,
die release/-Differenz in `git diff r30/integration..HEAD` stammt von dort, nicht vom Bauer).
Bauer-Commits: 4ada307e, 424177f2, a380704a, 12a30af9, 9595b574, 137bd422 (Dossier
`analysis/befunde_runde31/hebetisch.md`, 409 Zeilen, gelesen).

Spielcode-Aenderung (git diff 7cb74897..HEAD -- re15_port/engine re15_port/include):
* neu `engine/src/hebetisch_1150.c` + `include/re15_hebetisch.h`: 56-Byte-Signatur-Suche im
  Raum-RDT (gerufen aus `scd_register_current_rdt`), `re15_hebetisch_ruht_oben()` = irgendein
  aktiver SCD-Thread mit PC in [Sig+0x0A, Sig+0x32) = ROOM1150 [@0x101A,@0x1042).
* `sicherung_1150.c`/`granate_1150.c`: y-Schranke `py > OBEN_BIS(-1100)` ersetzt durch
  `!re15_hebetisch_ruht_oben()`.
* Sitze: Sicherung (-280,-1062,1280) rot_y 1440 (vorher 1260/1024), Granate (-260,-1091,1140)
  rot_y 1792 (vorher -362/-1088/1260/1024). Alle als PORT-WAHL gekennzeichnet, Herleitung
  per Suchwerkzeug aus Geometrie-Bytes.
* `game_step_common.c`: nur Mess-Protokoll-Aufruf (RE15_HEBETISCH_LOG).

Geltungsbereich: Dieses Thema ist NUR "H" (Hebetisch). "G" (ROOM11F0 Zeiger auf 80) liegt im
Zweig r31/generator und wird hier NICHT geprueft. "room 1170" im Auftrag = ROOM1150/1151
(Runde 30 vom Nutzer bestaetigt, AUFTRAG.md Lesart H).

## 2. Bau + Suite

Selbst gebaut: `bash re15_port/tools/local_build.sh all` (HEAD 137bd422 + Pruefer-Dossier), Log
`build/r31_hebetisch/pruefer_suite.log`:
```
100% tests passed, 0 tests failed out of 406
Total Test time (real) = 603.17 sec
=== LOCAL-BUILD-OK (all) — Tests 406/406
```
Parallel lief eine fremde ctest-Sitzung (r30_n_linux/build_pruef) - kein Flattern, keine Wiederholung
noetig. Deckt sich mit dem Bauer (406/406).

## 3. Stichproben Bytes/Adressen

### 3.1 sub04-Bytes (xxd re15_port/shared_assets/PSX/STAGE1/ROOM1150.RDT)
```
00000ff0: 0000 2f01 f6ff 0d00 0400 5b00 3002 0e00   Speed_set vy -10 @0xFF2, For 91 @0xFF6
00001000: 3602 0d00 0300 ...        2f01 0100       Se_on 0x0d @0x1000, Speed_set +1 @0x100C
00001010: 0d00 0400 0a00 3002 0e00 090a 1e00 2e03   For 10 @0x1010, Add_speed, Evt_next, Next, Sleep 30 @0x101A
00001020: 0000 3602 0a00 ...             090a       Work_set 0, Se_on 0x0a @0x1022, Sleep @0x102E
00001030: 0a00 3602 0c00 ...             2f01       Sleep 10, Se_on 0x0c @0x1032, Speed_set @0x103E
00001040: 0a00 0d00 0400 5a00 3002 0e00             vy +10, For 90 @0x1042 (Abfahrt)
```
Stimmt Byte fuer Byte mit Dossier §2.1 und `s_sig[56]` in hebetisch_1150.c. ROOM1151.RDT: dieselbe
Folge ab @0x0FEE (`0d00 0400 0a00 3002 0e00 090a 1e00 2e03 0000 3602 0a00 ...`), Fenster
[0x0FF8, 0x1020) - bestaetigt.
Vor dem Hub: For 15 @0x0FC0 (Work_set 1/2, Deckel je +-10 x 15 = 150), Sleep 30 @0x0FDE, Se_on
0x0c, Work_set 0 - die Deckel sind VOR dem Hub fertig offen; waehrend Hub/Setzen bewegt nur
Work_set 0 (Plattform). Also ruht zwischen @0x1018 und @0x1048 WIRKLICH das ganze Modell.

### 3.2 Freeze-/Sleep-Mechanik (re15_disasm.py, info/Re1.5/PSX.EXE)
```
8003f040: lw v0,-13760(v0)   g_pauseflags     8003f044: lui v1,0x200
8003f048: and v0,v0,v1                        8003f04c: bne v0,zero,0x8003f090   (Skript-Schritt ueberspringen)
8001db94: addiu t1,t1,-13760 g_pauseflags     8001db98: lui t0,0xff00
8001dbb8: or v0,v0,t0                         8001dbc8: sw v0,0(t1)              (|= 0xFF000000)
8003f3e8: addiu v0,a2,1 (PC+1)  8003f3f8: sw v0,28(a0)  8003f414: lhu a0,2(a2) (Dauer)  8003f424: ori v0,zero,0x1
8003f454: addiu v0,v0,-1  8003f460: bne v0,zero,0x8003f488  8003f470: addiu v0,v0,3  8003f48c: ori v0,zero,0x2
```
Alle vom Bauer zitierten Adressen/Instruktionen stimmen. PC im Sleeping = Sleep+1 = 0x101B
(= Logzeile `sub04-PC @0x101B`).

### 3.3 Riegel/Test-Aenderungen (P6)

* `unit_r31_hebetisch` (probe_r31_hebetisch.c): Pruefungen 2..6 rechnen die Sitze gegen die
  RDT-Geometrie (Ergebnis, nicht Absicht). Pruefungen 8..11 fahren eine EIGENE Bildschleife "in der
  Reihenfolge des Spiels" (SCD-Tick nur ohne Aufnahme, dann Sicherung-, Granaten-, Aufnahme-Tick) -
  das ist ein Nachbau der main.c-Reihenfolge, nicht die main.c selbst; der echte Takt ist nur durch
  die Framedump-/Protokoll-Laeufe belegt (s. §4). Mutation M1 (alte y-Schranke) macht 8..11 rot -
  der Riegel faengt also die Regression, die der Nutzer bemaengelt hat.
* Pruefung 7 (links/rechts) rechnet mit der Engine-Kamera ueber ALLE Proben, nicht die sichtbaren;
  Beleg fuer das Bild ist allein die MIT/OHNE-Differenz im Framedump.
* r30-Riegel nachgezogen: `unit_r30_granate` 4/5/7/8, `unit_r30_sicherung_sitz` (POS_Z-Mitte
  gestrichen, "auf der Naht" -> "rechts der Naht", Achse "genau z" -> "naeher an z"),
  `unit_r30_sicherung_nein` 2 (y == -1205), Integration EXIT_AT 250 -> 280. Jede Aenderung folgt
  dem neuen Nutzerwunsch (Sitz/Zeitpunkt), keine lockert eine Pruefung, die nicht vom Wunsch
  betroffen ist. `(void)zmin;` in probe_r30_granate: die frueher geforderte "ganz in der Oeffnung"
  (zmin >= 1110) ist gefallen - die Granate ragt 35 unter den linken Deckel (Dossier offen gelegt).
* RE15_MIN_TESTS 405 -> 406 an beiden Stellen (Z. 63 und 321) - geprueft im diff.

## 4. Sichtpruefung echtes Spiel (Framedump)

Echte exe `re15_port/build/platform/pc/re15_pc.exe` (selbst gebaut, s. §2), beschleunigter
Renderer, `RE15_FRAMEDUMP` (Ruecklesen vor Present), KEIN AUTOSHOT/SOFTWARE_RENDER, 320x240.
Lauf-Skript `hebetisch_pruefer_belege/p_lauf.sh` (Tuerweg per RE15_DEBUG_JUMP + RE15_PLAYER_POS
wie der Bauer, Eingabe per RE15_INPUT_SCRIPT = echte Viereck/Rechts-Tasten, Mess-Protokoll
RE15_HEBETISCH_LOG). Auswertung `p_ruhe.py`, Bogen `p_bogen.py`. Alle zitierten Bilder angesehen.

Messfalle: erster Versuch ohne `RE15_TITLE_SHOT`/`_AF` hing im Titel (rc=124, 0 Bilder, debug.log
endet nach der input-script-Zeile) - der Titel laeuft ohne diese Variablen nicht automatisch weiter.
Mit ihnen (wie lauf_fahrt.sh des Bauers) sauber.

### 4.1 Lauf p_ja_nein (NEU, vom Bauer nicht gefahren): Fahrt 1 Sicherung Yes / Granate NO, Fahrt 2

`p_ja_nein_debug_auszug.txt`, `p_ja_nein_ruhe.txt`:
```
[sicherung] Modal auf (Hebetisch y=-1205, Ruhe oben: sub04-PC @0x101B)   (Bild 387)
Tick 306 -> F506 Yes -> [sicherung] Yes: genommen, Flag (9,53)
[granate] Modal auf (Hebetisch y=-1205, Ruhe oben: sub04-PC @0x101B)     (Bild 507)
F632 R, F653 Viereck -> [granate] No/voll: nicht genommen, Granate bleibt liegen
[... Fahrt zu Ende, Parklage y=-20224: Sperre geloest] (beide)
F1199 Viereck (Fahrt 2), Cut_chg(4) F1205
[granate] Modal auf (Hebetisch y=-1205, Ruhe oben: sub04-PC @0x101B)     (Bild 1356)
F1490 Yes -> [granate] Yes: genommen, Flag (9,56), Item 0x09 x1 in Platz 4
Aufnahme auf in F387:  y=-1205 pc=0x101B | y F-3..F-1 = [-1207, -1206, -1205]
Aufnahme auf in F507:  y=-1205 pc=0x101B | y F-3..F-1 = [-1205, -1205, -1205]
Aufnahme auf in F1356: y=-1205 pc=0x101B | y F-3..F-1 = [-1207, -1206, -1205]
Bilder mit Aufnahme: 416, davon y != -1205: 0
Ruhe F387..F708 (322 Bilder), davon ohne Aufnahme 40; naechstes Bild F709 y=-1195
Ruhe F1356..F1529 (174 Bilder), davon ohne Aufnahme 40
```
Bilder `p_ja_nein_fahrt1.png` (F260/F320 Fahrt: Granate links, Sicherung schraeg nach rechts unter
den rechten Deckel; F376/F384 oben, beide an der oberen Bildkante sichtbar; F388 Aufnahme-Bild
beginnt; F400 Sicherungs-Aufnahme). `p_ja_nein_fahrt2.png` (F1248/F1300 Fahrt 2: NUR die Granate,
links im Fach - die genommene Sicherung ist weg; F1352 oben; F1360 Granaten-Aufnahme; F1500 Fach
leer; F1640 Statusschirm mit Sicherung und Granate x1). Lupe `p_lupe_fach.png` (Ausschnitte x4).

### 4.2 Lauf p_nein_nein (NEU): Fahrt 1 No/No, Fahrt 2 Yes/Yes, ROOM1150

Gedacht als ROOM1151-Tuerweg (`RAUM=1151`), aber `RE15_DEBUG_JUMP=1151` landet in ROOM1150
(`[debug-menu] AUTO-JUMP -> ROOM1151` / `JUMP -> 115 CHIEF OFFICE (ROOM1150)` / `PC loaded
room1150.rdt`) - der Sprung waehlt die Raumvariante selbst. Deshalb ist dieser Lauf ein 1150-Lauf
(EXIT_AT#1151 griff nie -> rc=124 nach 280 s, der Inhalt bis F1900 ist vollstaendig).
```
Aufnahme auf in F387:  (Sicherung, Fahrt 1) y=-1205 pc=0x101B | davor -1207,-1206,-1205
Aufnahme auf in F545:  (Granate, Fahrt 1, nach Sicherung-No + 17 Bilder Zustand 8) y=-1205
Aufnahme auf in F1455: (Sicherung, Fahrt 2) y=-1205 pc=0x101B | davor -1207,-1206,-1205
Aufnahme auf in F1584: (Granate, Fahrt 2) y=-1205
Bilder mit Aufnahme: 580, davon y != -1205: 0
Ruhe F387..F747, davon ohne Aufnahme 40 / Ruhe F1455..F1753, davon ohne Aufnahme 40
```
Nach No/No bietet Fahrt 2 BEIDE wieder an, wieder erst in der Ruhe oben, Reihenfolge Sicherung ->
Granate; Yes/Yes -> Flags (9,53)/(9,56), Inventar-Platz 3/4.

### 4.3 Lauf p_laden1151 (Lade-Weg ROOM1151, Werkzeug des Bauers lauf_laden.sh, eigene Ausgabe)

`[save] CONTINUE: resumed in room 1151`, Boot-Weg obj 4 und obj 7 im Pool, `[fire-aot] slot=1 at
F90`, `[sicherung] Modal auf (Hebetisch y=-1205, Ruhe oben: sub04-PC @0x0FF9)`;
Aufnahme auf in F247, y davor -1207/-1206/-1205, 53 Aufnahme-Bilder alle bei -1205.
Bild `p_laden1151.png`: F120/F160 Granate links, Sicherung rechts; F240/F246 oben; F260 Aufnahme.
(ROOM1151 nur ueber CONTINUE + RE15_FIRE_AOT erreichbar, nicht per Tuerweg-Sprung - s. 4.2.)

### 4.4 Kuppel geschlossen: kein Durchstoss (Start und Ende nach No/No)

`p_kuppel_zu_start.png` (p_ja_nein F240 Kuppel zu: nichts von Granate/Sicherung sichtbar; F244
Deckel oeffnen, zuerst das Rohr in der Naht; F248/F252 Granate links am Oeffnungsrand).
`p_kuppel_zu_ende_nein_nein.png` (p_nein_nein nach No/No, beide liegen noch: F856/F868 offen, F876
schliessend, F884 fast zu, F900/F940 zu - nichts ragt durch die Schale). Deckt die Behauptung des
Bauers (0 Punkte MIT gegen OHNE) im Bild.

### 4.5 Eigener Zensus der Ruhe-Signatur
56-Byte-Folge ueber alle 240 RDTs unter re15_port/shared_assets/PSX/STAGE*/: genau
ROOM1150 @0x1010 (Fenster 0x101A..0x1042) und ROOM1151 @0x0FEE (0x0FF8..0x1020), je 1 Treffer.
Deckt sich mit `ruhe_signatur.txt` des Bauers.

## 5. Nicht gefahrene Faelle

| Fall | Bauer | Pruefer | Ergebnis |
|---|---|---|---|
| Yes/Yes Fahrt 1 (Tuerweg 1150) | ja_ja | (Bilder des Bauers angesehen) | ok |
| Sicherung No, Granate Yes, Fahrt 2 | nein_ja | - | ok (Bauer-Beleg) |
| Sicherung Yes, Granate NO, Fahrt 2 bietet Granate allein | - | p_ja_nein | ok, Ruhe oben, Fach zeigt nur Granate links |
| No/No, Fahrt 2 Yes/Yes | nur Engine-Riegel unit_r30_granate 8/9 | p_nein_nein | ok |
| Lade-Weg 1150 / 1151 | laden1150/1151 | p_laden1151 | ok (PC @0x0FF9) |
| Tuerweg ROOM1151 | nicht gefahren | nicht erreichbar per Sprung | offen (Sprung waehlt 1150); Engine-Riegel + CONTINUE decken die Bytes |
| Wiedereintritt (Tuer raus/rein) | - | - | nicht gefahren; install laeuft an beiden Raumstart-Wegen (scd_room_setup.c:412/422, main.c:4621/4648), Sperre wird bei install genullt |
| Inventar voll | Engine-Riegel unit_r30_sicherung_nein C | - | nicht im Spiel gefahren; Zweig unveraendert (gleicher No-Pfad) |
| Save/Load | CONTINUE-Laeufe | p_laden1151 | ok (Speichern im Hebetisch-Modal nicht moeglich) |

## 6. Befunde

Kein Befund gegen den Nutzerauftrag H. Hinweise (alle niedrig):

1. **Sicherung nicht ganz rechts, sondern schraeg von der Mitte nach rechts, gut die Haelfte unter
   dem rechten Deckel.** Relativ ist die Anordnung erfuellt (Granate links, Sicherung rechts; Bauer
   MIT/OHNE: 100 % der Granaten-Punkte links vom Sicherungs-Schwerpunkt; Pruefer-Bilder F260/F320/
   F1248 bestaetigen). Geometrisch erzwungen (Kuppel zu Beginn/Ende geschlossen, Rohr 406 lang);
   Bauer-Dossier §1.1/§6 legt das offen. Falls der Nutzer "ganz rechts" meint, geht das nur mit
   Durchstoss durch die geschlossene Kuppel.
2. **In der Ruhe oben stehen Fach und Gegenstaende an der oberen Bildkante** (Cut 4 ist die
   Original-Kamera, @Datei 0xE0; F376/F384/F1352). Der Nutzer sieht die Gegenstaende beim Hochfahren
   gut, beim Dialog-Beginn nur noch am oberen Rand. Nicht Teil des Auftrags, keine Aenderung noetig.
3. **Tuerweg ROOM1151 nicht gefahren** (weder Bauer noch Pruefer): `RE15_DEBUG_JUMP=1151` waehlt
   selbst ROOM1150. 1151 ist ueber CONTINUE (Bauer + Pruefer, PC @0x0FF9) und im Engine-Riegel
   belegt; die Bytes (Fenster [0x0FF8,0x1020)) sind identisch aufgebaut.
4. **Riegel-Zeitpruefungen 8..11 fahren eine nachgebaute Bildschleife**, nicht main.c. Der echte
   Takt ist nur ueber die Spiel-Laeufe belegt (alle gruen, 0 Aufnahme-Bilder ausserhalb -1205).
5. **Android**: neue Quelldatei `engine/src/hebetisch_1150.c` -> frischer Configure noetig
   (GLOB-Cache); ohne ihn bricht der Link laut (undefined re15_hebetisch_*), nicht still.
6. **Vorbestand, nicht Teil des Auftrags**: Port friert auch im Aufnahme-Zustand 8 ("No", 17 Bilder)
   ein, das Original nicht (@0x800285a4 / @0x8001df3c) - vom Bauer dokumentiert, fuer den Hebetisch
   folgenlos (gemessen: in p_nein_nein bleiben 40 Ruhebilder nach beiden Dialogen).
7. **Inventar voll** nicht im Spiel gefahren (Engine-Riegel unit_r30_sicherung_nein C, Zweig
   unveraendert).

## 7. Urteil

**haltbar** (fuer Thema H; Thema G "ROOM11F0 Zeiger auf 80" liegt im Zweig r31/generator und ist
hier nicht geprueft).

* P1 Gegenstaende im Modell, Granate links / Sicherung rechts: im echten Spiel gesehen (Tuerweg
  1150 Fahrt 1 und 2, Lade-Weg 1151), fahren mit, kein Durchstoss bei geschlossener Kuppel.
* P2 Aufnahme erst, wenn komplett hochgefahren: in JEDEM gefahrenen Fall (Yes/Yes, Yes/No, No/No,
  Fahrt 2 mit einem bzw. beiden, Lade-Weg 1151) geht die Aufnahme im ersten Ruhebild auf (y davor
  -1207/-1206/-1205, sub04-PC = Sleep 30 + 1), 0 von 1049 Aufnahme-Bildern (416 + 580 + 53) ausserhalb y=-1205,
  danach noch 40 Ruhebilder vor der Abfahrt. Mechanik (Freeze @0x8001dbc8 / @0x8003f04c, Sleep
  @0x8003f3e8 / Sleeping @0x8003f428) und sub04-Bytes selbst nachgeprueft.
* P5 Konstanten: Sitze/Drehungen/IM_RAUM_AB als PORT-WAHL mit Herleitung gekennzeichnet, das
  Fenster aus Datei-Bytes (Signatur, Zensus selbst wiederholt), Commit-Messages tragen die Adressen.
* P7 Suite 406/406 selbst gebaut.
