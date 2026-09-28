# Runde 31 — H: Hebetisch ROOM1150 (Granate links, Sicherung rechts, Dialog erst oben)

Auftrag (woertlich, Auszug): "baue mir danach room 1170 die items im model an, und packe mir die
Granate links in die hochfahrende Box und die Sicherung rechts. Außerdem starte mit dem Aufnahme
Dialog der items erst wenn das Modell wirklich komplett hochgefahren ist."
("room 1170" = Hebetisch in Irons' Buero ROOM1150/1151, Runde 30 vom Nutzer bestaetigt.)

Stand: IN ARBEIT (laufend gefuellt). Arbeitsbaum `.claude/worktrees/r31_hebetisch`, Zweig
`r31/hebetisch`, Basis 10d0c706. Werkzeuge: `analysis/befunde_runde31/hebetisch_werkzeug/`,
Belege: `analysis/befunde_runde31/hebetisch_belege/`, grosse Zwischenausgaben
`build/r31_hebetisch/` (nicht versioniert).

## 0. Bestand (Runde 30) — gelesen, nicht neu gemessen

* Sicherung (Item 0x40, obj 4, Flag (9,53)) Sitz (-280,-1062,1260) rot_y 1024, an der
  Elternmatrix der Plattform (parent_obj 0); Granate (Item 0x09, obj 7, Flag (9,56)) Sitz
  (-362,-1088,1260) rot_y 1024, 3 Einheiten im Fachboden versenkt. Beide liegen mittig an der
  Naht; die Granate liegt von Cut 4 aus HINTER der Sicherung (`nachtrag-granate.md` §5, Bild
  `nachtrag-granate_belege/abnahme_fahrt_ja.png`: das Rohr quer ueber die Oeffnung, die Granate
  mittig dahinter).
* Modal-Zeitpunkt heute: Fenster `(-5000, -1100]` auf Plattform-y (`sicherung_1150.c`
  `IM_RAUM_AB`/`OBEN_BIS`, `granate_1150.c` dasselbe) = Bild 134 der Fahrt bei y=-1105, also
  MITTEN in der Hubfahrt (Hub bis -1215 endet Bild 145, Setzen auf -1205 Bild 146..155).
  Gemessen Runde 30: `[sicherung] Modal auf (Hebetisch y=-1105)`, `[granate] Modal auf
  (Hebetisch y=-1115)` (`nachtrag-granate.md` §7.2).
* Geometrie (`sicherung.md` §3.6, `nachtrag-granate.md` §5): Fachboden-Achteck y=-1036,
  x[-485..-74], z[875..1645], Naht z=1260; Deckel offen je 150 -> Oeffnung z[1110..1410].

## 1. Anordnung: Granate links, Sicherung rechts (Cut 4)

### 1.1 Geometrie (Fach, Oeffnung, Masse der Gegenstaende)

Ausgelieferte Bytes (Werkzeug `geo_dump.py`, ROOM1150.RDT; ROOM1151 gleiche Werte):

* **Kuppelhaelfte Prop 1** (MD1 @0x138D4, 12 Punkte, 4 Dreiecke, 6 Vierecke): Grundpunkte y=-1036
  (-74,1260) (-128,1562) (-280,1645) (-432,1562) (-485,1260) (-280,1260); Ring y=-1138
  (-147,1260) (-186,1447) (-280,1510) (-374,1447) (-413,1260); Scheitel (-280,-1185,1260).
  Prop 2 (@0x13B88) spiegelbildlich z<=1260. Beide Haelften tragen an der Naht eine senkrechte
  Flaeche (Prop 1 Vierecke `(1,11,2,0)`/`(11,9,0,10)`); offen liegen diese Flaechen bei z=1110 und
  z=1410 und werden von Cut 4 aus fast genau von der Kante gesehen.
* Die Kuppel ist ein FLACHER Achteck-Dom: 102 hoch am Ring, 149 am Scheitel. Auf Rohrhoehe 52
  (Sicherung liegend, D52) ist der Innenraum in x nur ~337 breit (Grundkante x=-485 -> Ring
  x=-413 steigt 102 auf 72), in z ~632 lang.
* **Sicherung** (gen/sicherung_prop.inc): 406 lang, D52. **Granate** (gen/granate_prop.inc):
  154 lang, Achteck-Querschnitt 110; offen sind die Unterseite (+y) und am Modell-+x-Ende die
  -z-Kegelflaeche samt Eckdreieck (Flaechenliste ausgegeben, 5 Dreiecke/14 Vierecke).
* **Kamera Cut 4** (@Datei 0xE0): Plattform-Achse z = Schirm waagrecht, **+z = RECHTS**
  (Rechnung `achse.py`: lokal (-280,-1062,z) z=1110/1260/1410 -> Schirm-x 186/209/232 bei
  Plattform y=-305, 191/218/245 bei y=-1205); Plattform-x = Tiefe (Kamera bei lokal x=+1242).
  Links/rechts wird unten im Framedump GEMESSEN, die Rechnung dient nur der Planung.

**Fasst die Oeffnung (300 breit) beide Gegenstaende nebeneinander? — NEIN, nicht vollstaendig.**
Rechnung `r31_suche.py` (Hoehenfeld der Kuppel aus den Dreiecken, jede Probe = Punkt, Kante in 8,
Flaechenmitte; Bedingungen: Luft unter der GESCHLOSSENEN Kuppel >= 0, Luft unter den OFFENEN
Deckeln >= 0, Grundriss im Achteck):

* Sicherung LAENGS in x (rot_y 0, "Fachbreite in x 411"): **0 zulaessige Sitze** — auf Rohrhoehe
  52 ist die Kuppel innen nur ~337 breit, das 406-Rohr stuende an beiden Enden ~35 durch die
  geschlossene Kuppel (die ist zu Beginn jeder Fahrt, Bild 5..9, und nach "No" am Ende zu).
  Zulaessig sind nur rot_y 576..1472 (Laengsachse hoechstens ~56 Grad von z weg).
* Sicherung ganz in der rechten Haelfte der Oeffnung (z > 1260): geht in KEINER Drehung — der
  rechte Halbraum auf Hoehe 52 fasst diagonal hoechstens ~366, das Rohr ist 406 lang. Am weitesten
  rechts (Mitte z) liegt sie bei z-Mitte 1360..1370; dann liegen ~52 % ihrer Proben in der
  Oeffnung, der Rest unter dem rechten Deckel.
* Granate: braucht 110 Hoehe (Kuppel innen 102 am Ring, 149 am Scheitel) -> nur nahe der Mitte
  moeglich, links bis z-Mitte ~1095..1115.

=> "Granate links / Sicherung rechts" geht nur mit der Sicherung SCHRAEG, von hinten-links neben
der Granate nach vorne-rechts unter den rechten Deckel. Genau das ist gewaehlt (1.2).

### 1.2 Sitzwahl + Drehung (PORT-WAHL, keine Original-Adresse)

Das Original hat im Hebetisch keine Beute; Sitz und Drehung haben deshalb keine Instruktion.
Hergeleitet aus den ausgelieferten Geometrie-Bytes (1.1) per Suche, nicht geschaetzt:

1. `r31_suche.py`: alle zulaessigen Sitze je Gegenstand (Raster: Sicherung rot_y 0..2047 in 64er
   Schritten, x -440..-120, z 1150..1500 je 10; Granate rot_y 0..1792 in 256er, x -400..-160 je 5,
   z 1040..1300 je 5, y -1091 aufliegend / -1088): 2513 Sicherungs-, 13876 Granaten-Sitze.
2. `r31_raster.py`: z-Puffer-Rasterer der Cut-4-Szene (Plattform, beide Deckel offen, Gegenstaende)
   bei Plattform y=-305 (Start) und y=-1205 (Ruhe oben); je Gegenstand sichtbare Punkte und
   Schirm-Schwerpunkt. Gegenprobe am Bestand: Granate bbox y 150..163 (Runde 30 gemessen 149..162).
3. `r31_wahl.py` / `r31_wahl2.py`: Paare mit Abstand Granate -> Sicherungs-Mantel >= 1, Ziel
   = Schwerpunkt-Trennung (Sicherung minus Granate, kleinster Wert beider Blicke) und kleinste
   sichtbare Flaeche. **Granate nur rot_y 1024..2048**: dann zeigt ihre offene Ecke (Modell-+x-Ende,
   -z-Seite) von der Kamera weg (bei rot_y 0 laege sie zur Kamera).
4. `r31_pruef.py`: exakte Dreiecksrechnung (Kanten in 16 Stuecken) fuer das gewaehlte Paar in
   ROOM1150 UND ROOM1151.

**Gewaehlt** (Ausgabe `hebetisch_belege/pruef_sitz.txt`):

| | Sitz (Plattform) | rot_y | Luft Kuppel zu | Luft Deckel offen | tiefster Punkt | z-Bereich |
|---|---|---|---|---|---|---|
| Sicherung | (-280, -1062, 1280) | 1440 | 6,01 | 46,80 | 0,0 = liegt auf | 1101..1459 |
| Granate | (-260, -1091, 1140) | 1792 | 5,88 | 32,70 | 0,0 = liegt auf | 1075..1205 |

Abstand Granate -> Sicherungs-Mantel 6,77. Beide Grundrisse im Achteck. ROOM1151 Zahl fuer Zahl
gleich. Die Granate liegt jetzt AUF dem Fachboden (y = -1036 - 55), die 3 Einheiten Versenkung
aus Runde 30 entfallen. Planungswerte (Rasterer, Schirm-Schwerpunkt x bei 320 breit):
Start Sicherung 220,4 / Granate 194,9; oben 232,7 / 202,8 (Trennung >= 25 px); sichtbare
Punkte Start 324/235, oben 389/264. Die Sicherung laeuft von hinten-links (neben der Granate,
x -393 z 1111) schraeg nach vorne-rechts (x -167 z 1449) unter den rechten Deckel.
Planungsbild: `hebetisch_belege/planung_kandidaten.png` (Zeile 1 = gewaehlt; ID-Puffer, weiss Sicherung, gruen Granate, lila Deckel).

### 1.3 Messung im Framedump (MIT/OHNE-Differenz, Schwerpunkte)

Echte exe (`re15_port/build/platform/pc/re15_pc.exe` dieses Baums), beschleunigter Renderer,
`RE15_FRAMEDUMP` (Ruecklesen vor Present), KEIN AUTOSHOT/SOFTWARE_RENDER, `RE15_WINDOW_SCALE=1`
(320x240). Laeufe `hebetisch_werkzeug/lauf_fahrt.sh` (Tuerweg: Debug-Sprung ROOM1150, Spieler
(-21000,-18500), Viereck in Tick 30 = F230) und `lauf_laden.sh` (CONTINUE, `RE15_FIRE_AOT=1@90`).
Kill-Switch wie Runde 30: `RE15_SET_FLAG=9:56` (OHNE Granate), `9:53` (OHNE Sicherung),
`9:53,9:56` (OHNE beide); am Lade-Weg dieselben Flags in der Karte (`probe_r30_granate_karte`
`genommen` / `sicherung`). Auswertung `links_rechts.py`: Granate = MIT gegen OHNE-G, Sicherung =
MIT gegen OHNE-S, je Bild Punkte, Schwerpunkt, bbox.

Tuerweg ROOM1150 (`hebetisch_belege/links_rechts_tuerweg_1150.txt`, Auszug):

| F | Granate Punkte / Schwerpunkt-x | Sicherung Punkte / Schwerpunkt-x | Trennung | G links vom S-Schwerpunkt | S rechts vom G-Schwerpunkt |
|---|---|---|---|---|---|
| 236..240 (Kuppel zu) | 0 | 0 | — | — | — |
| 250 (Deckel gehen auf) | 52 / 199,3 | 257 / 215,6 | +16,3 | 100 % | 100 % |
| 256..280 (offen, Start) | 231 / 194,4 | 365 / 221,0 | +26,6 | 100 % | 100 % |
| 310 (Hub) | 233 / 196,4 | 379 / 224,5 | +28,2 | 100 % | 100 % |
| 352 | 249 / 200,3 | 453 / 230,0 | +29,7 | 100 % | 100 % |
| 376 (oben) | 269 / 202,2 | 397 / 235,1 | +32,9 | 100 % | 100 % |
| 382 | 258 / 202,3 | 398 / 234,3 | +32,0 | 100 % | 100 % |

In JEDEM Bild F250..F386 liegt die Granate LINKS und die Sicherung RECHTS: 100 % der
Granaten-Punkte links vom Sicherungs-Schwerpunkt, 100 % der Sicherungs-Punkte rechts vom
Granaten-Schwerpunkt, Trennung +16..+33 px (bei 320 Breite; die Oeffnung ist oben ~54 px breit).
Lade-Weg ROOM1150 und ROOM1151 (`links_rechts_ladeweg_1150.txt`/`_1151.txt`): Zahl fuer Zahl
dieselben Werte (F110 = 52/257 ... F240 = 266/404, Trennung +32,9).
Rasterer-Planung vs. Messung: Granate 235 geplant / 231 gemessen (Start), Sicherung 324 / 365.

**Kein Durchstoss bei geschlossener Kuppel:** MIT beide gegen OHNE beide, F230..F240 (Plattform
schon auf -305, Kuppel zu): **0 Punkte** (`kuppel_zu_start_mit_gegen_ohne.txt`; F242 13 Punkte,
die Deckel gehen auf). Am Ende einer Fahrt nach "No" (Sicherung bleibt liegen), MIT Bild
824..908 gegen OHNE (Versatz 283 = Parkbild 909 - 626): 420 Punkte offen, beim Schliessen 400 ->
13, ab Bild 848 bis zum Parken **0 Punkte** (`kuppel_zu_ende_nein.txt`). Runde 30 hatte bei
aufliegender Granate 2 Punkte durch die Schale (deshalb 3 tief versenkt) — jetzt 0 bei
aufliegender Granate.

Bilder (angesehen): `hebetisch_belege/fahrt_lupe_960.png` (Lauf mit `RE15_WINDOW_SCALE=3`,
F260/F300/F340/F384, Ausschnitt x3: Granate links mit Rautenmuster, Sicherung schraeg nach
rechts unter den rechten Deckel; die offene Ecke der Granate ist nicht zu sehen),
`fahrt_bogen_320x2.png` (F250..F400 ganzes Bild).

## 2. Zeitpunkt: Dialog erst, wenn die Plattform ruht

### 2.1 Skript-Zustand sub04 (Setzen-For @0x1010, Sleep 30 @0x101A)

sub04 ROOM1150.RDT, linear dekodiert (Laengen aus der 95er-Tabelle, Skill scd-disassembly):

```
0x0FF2 Speed_set  2f 01 f6 ff         vy = -10
0x0FF6 For        0d 00 04 00 5b 00   91 x { 0x0FFC Add_speed 30 ; 0x0FFD Evt_next 02 } 0x0FFE Next
0x1000 Se_on      36 02 0d 00 03 00 ...
0x100C Speed_set  2f 01 01 00         vy = +1
0x1010 For        0d 00 04 00 0a 00   10 x { 0x1016 Add_speed 30 ; 0x1017 Evt_next 02 } 0x1018 Next
0x101A Sleep      09 0a 1e 00         30 Bilder  <- RUHE OBEN beginnt
0x101E Work_set   2e 03 00 / 00
0x1022 Se_on      36 02 0a 00 03 00 ...
0x102E Sleep      09 0a 0a 00         10 Bilder
0x1032 Se_on      36 02 0c 00 03 00 ...
0x103E Speed_set  2f 01 0a 00         vy = +10
0x1042 For        0d 00 04 00 5a 00   90 x { 0x1048 Add_speed 30 ; ... }   <- Abfahrt
```

Selbst disassembliert (`re15_disasm.py`, Opcode-Tabelle @0x800744a8):

```
[0x2F] Speed_set -> 0x80040f14:  80040f1c lbu v1,1(v0) (Achse) / 80040f20 lhu a1,2(v0) (Wert)
                                 80040f24 addiu v0,v0,4 (PC+4) / 80040f3c sh a1,344(v1)
                                 -> NUR Thread+0x158+2*Achse, das Objekt bleibt stehen
[0x30] Add_speed -> 0x80040f40:  80040f40 lw a1,340(a0) (Arbeits-Objekt) / 80040f44 lh v0,344(a0)
                                 80040f48 lw v1,52(a1) / 80040f50 addu / 80040f54 sw v0,52(a1) ...
                                 -> Objekt-Lage += Thread-Geschwindigkeit (x/y/z, dann Drehung)
[0x09] Sleep     -> 0x8003f3e0:  8003f3e8 addiu v0,a2,1 / 8003f3f8 sw v0,28(a0) (PC+1)
                                 8003f414 lhu a0,2(a2) (Dauer) / 8003f424 ori v0,zero,0x1
[0x0A] Sleeping  -> 0x8003f428:  8003f454 addiu v0,v0,-1 (Zaehler-1) / 8003f460 bne -> 0x8003f488
                                 bei 0: 8003f470 addiu v0,v0,3 (PC+3) ; 8003f48c ori v0,zero,0x2
                                 -> gibt IMMER 2 zurueck = Yield, auch im letzten Durchlauf
```

Nur `Add_speed` (0x30) bewegt die Plattform. Zwischen dem letzten Add_speed des Setzens
(@0x1016, 10. Durchlauf) und dem ersten der Abfahrt (@0x1048) steht keins: solange der
sub04-Thread seinen PC in [@0x101A, @0x1042) hat, RUHT die Plattform auf -1205.
Takt im Port (`scd_vm.c` op_sleep/op_sleeping, Original Sleep @0x8003f3e8 PC+1, Sleeping
LAB_8003f428 gibt immer 2 = Yield): Bild k: letztes Add_speed -> y -1205, Yield an Evt_next (PC
0x1018). Bild k+1: Next endet -> Sleep -> Sleeping zaehlt 30->29, Yield mit PC 0x101B. **Bild
k+1 ist das erste Ruhebild** (Plattform steht wie im Bild davor).

### 2.2 Was tut die Szene waehrend des Item-Modals? (Original + Port)

**Original (PSX.EXE, selbst disassembliert, `re15_disasm.py`):**

* Aufnahme-FSM FUN_8001db28, Sprungtabelle @0x800106b4 (9 Eintraege: [1] 0x8001db74, [5]
  0x8001df14, [6] 0x8001dffc, [7] 0x8001e048, [8] 0x8001e10c).
* Zustand 1 friert ein: `8001db90 lui t1,0x800b` / `8001db94 addiu t1,t1,-13760` (=0x800aca40
  g_pauseflags) / `8001db98 lui t0,0xff00` / `8001dbac lw v0,0(t1)` / `8001dbb8 or v0,v0,t0` /
  `8001dbc8 sw v0,0(t1)` -> g_pauseflags |= 0xFF000000.
* Der SCD-Laeufer FUN_8003f038 prueft Bit 0x02000000: `8003f040 lw v0,-13760(v0)` /
  `8003f044 lui v1,0x200` / `8003f048 and v0,v0,v1` / `8003f04c bne v0,zero,0x8003f090` -> KEIN
  Skript-Schritt, solange das Bit steht. 0xFF000000 enthaelt es: **sub04 steht, der Sleep 30
  zaehlt nicht, die Plattform faehrt nicht.**
* Zustand 5 (@0x8001df14): `8001df3c sw v1,-13760(at)` mit v1 = s4 & 0xffff (`8001df34`) —
  oberes Halbwort geloescht — und sofort `jal 0x80027e68` (@0x8001df90 / @0x8001dfe0) mit
  `lui a3,0xff00` (@0x8001df94 / @0x8001dfdc): das Nachrichten-Oeffnen (Schnappschuss @0x80027ec8,
  `or` @0x80027ecc, `sw` @0x80027ed0) friert im SELBEN Bild wieder mit 0xFF000000 ein.
* Aufgehoben wird der Freeze erst beim Bestaetigen der Ja/Nein-Frage: FUN_80028134
  `80028594 lw a0,-31428(a0)` (Schnappschuss 0x800b853c) / `800285a4 sw a0,-13760(at)`. Der
  Schnappschuss stammt aus Zustand 5, also ohne das obere Halbwort -> ab der Antwort laeuft das
  Skript wieder. Zustand 7 "Yes" setzt den Zustand auf 0 (`sb zero,11579(at)` @0x8001e0e0); "No"/voll
  geht in Zustand 8 (17 Bilder Wegschrumpfen, `sltiu v0,v0,0x11` @0x8001e154) — dort laeuft das
  Skript im Original bereits.
* Wortweiser Scan der EXE nach der Ladeform von 0x800aca40 (Immediate 0xCA40): Schreiber nur
  @0x800144cc, @0x8001ca44, @0x8001caec, @0x8001cbe4, @0x8001cc94, @0x8001cde8, @0x8001d538,
  @0x8001df3c, @0x800285a4, @0x800286cc, @0x8002871c (+ Adressbildungen @0x8001db94,
  @0x80027e98, @0x800430cc). In der Aufnahme-FSM nur @0x8001dbc8 (ueber t1) und @0x8001df3c.

**Port:** `platform/pc/main.c` (Zweig `re15_item_modal_active()`, ~Z. 5308): solange die
Aufnahme-FSM nicht in Zustand 0 ist, unterbleibt `scd_vm_tick()`; `game_step_common.c` kehrt nach
`re15_sicherung_tick`/`re15_granate_tick` zurueck (`if (re15_item_modal_active()) return;`).
Also: sub04 steht waehrend beider Dialoge. Abweichung vom Original: der Port friert auch im
Zustand 8 ("No", 17 Bilder) ein — das Original nicht. Fuer den Hebetisch ohne Folge (s. 2.3:
die Ruhe oben dauert 30 + 10 Skriptbilder); allgemein ausserhalb dieses Auftrags, notiert in §6.

Zwischen den beiden Dialogen laeuft im Port genau EIN Skriptbild (Bild N: Modal-Tick setzt die
FSM auf 0; Bild N+1: `scd_vm_tick` -> dann `re15_granate_tick` oeffnet den zweiten). Runde 30
gemessen: -1105 -> -1115 (damals mitten im Hub). Mit dem neuen Zeitpunkt faellt dieses eine Bild
in den Sleep 30 @0x101A — die Plattform bewegt sich nicht.

### 2.3 Einbau

* `include/re15_hebetisch.h` + `engine/src/hebetisch_1150.c` (neu): `re15_hebetisch_raum_scan()`
  sucht beim Registrieren des Raum-RDT (`scd_register_current_rdt`, scd_vm.c — laeuft am Tuer-
  und am Boot/CONTINUE-Weg) die 56 Byte @0x1010..@0x1047; `re15_hebetisch_ruht_oben()` = ein
  aktiver SCD-Thread mit PC in [Sig+0x0A, Sig+0x32) = [@0x101A, @0x1042). Zensus
  (`ruhe_signatur.py`, `hebetisch_belege/ruhe_signatur.txt`): 240 RDTs, Treffer NUR ROOM1150
  @0x1010 (Fenster [0x101A,0x1042)) und ROOM1151 @0x0FEE ([0x0FF8,0x1020)).
* `sicherung_1150.c` / `granate_1150.c`: `if (py > OBEN_BIS) return 0;` (y-Schranke -1100) ersetzt
  durch `if (!re15_hebetisch_ruht_oben()) return 0;`. `OBEN_BIS` entfernt. `IM_RAUM_AB` (-5000)
  bleibt nur fuer das Wiederscharfmachen je Fahrt in der Parklage (Runde-30-Regel, unveraendert).
  Reihenfolge unveraendert: erst Sicherung, dann Granate (`re15_sicherung_fahrt_offen`).
  Logzeilen nennen jetzt den sub04-PC: `[sicherung] Modal auf (Hebetisch y=-1205, Ruhe oben:
  sub04-PC @0x101B)`.
* Mess-Protokoll `RE15_HEBETISCH_LOG=<datei>` (nur PC, kein Verhalten): je Spielbild
  `F<n> y=<plattform> ruht=<0|1> pc=<datei-offset> modal=<zustand>`, gerufen in
  `game_step_common.c` nach dem SCD-Tick und vor `re15_sicherung_tick`.
* Beide Dialoge an der ruhenden Plattform: die Aufnahme haelt das Skript an (2.2). Die Ruhe oben
  dauert 30 (Sleep @0x101A) + 10 (Sleep @0x102E) SKRIPT-Bilder; zwischen den Dialogen laeuft 1
  Skriptbild (Port) — gemessen: danach noch 38 Ruhebilder vor der Abfahrt (3.2).

## 3. Abnahme im echten Spiel

### 3.1 (a) Fahrt: Granate links, Sicherung rechts — s. 1.3

### 3.2 (b) Die letzten Bilder vor dem ersten Dialog (Mess-Protokoll, Tuerweg, Lauf `ja_ja`)

`hebetisch_belege/ruhe_protokoll_ja_ja_F370-392.txt`:

```
F375 y=-1205  F376 y=-1215 (Hochpunkt)  F377 -1214 ... F385 y=-1206
F386 y=-1205 ruht=0 pc=-1     modal=0   <- letztes Add_speed des Setzens (For @0x1010)
F387 y=-1205 ruht=1 pc=0x101B modal=0   <- erstes Ruhebild: Sleep 30 @0x101A; Modal auf in DIESEM Bild
F388 y=-1205 ruht=1 pc=0x101B modal=2   <- Modal zeichnet ("zeichnet in Bild 388")
```

`debug.log`: `[sicherung] Modal auf (Hebetisch y=-1205, Ruhe oben: sub04-PC @0x101B)`. Runde 30:
Bild 365, y=-1105 (22 Bilder frueher, mitten im Hub). Auswertung aller Laeufe
(`ruhe_pruef.py`, `hebetisch_belege/ruhe_pruef.txt`):

| Lauf | Fahrt | letzte Bewegung | erstes Ruhebild | Aufnahme offen (Bilder) | davon y != -1205 | Ruhe ohne Aufnahme | Abfahrt ab |
|---|---|---|---|---|---|---|---|
| ja_ja (Tuerweg) | 1 | 386 | 387 | 244 | **0** | 40 | 671 |
| nein_ja | 1 | 386 | 387 | 283 | **0** | 40 | 710 |
| nein_ja | 2 | 1313 | 1314 | 137 | **0** | 40 | 1491 |
| laden1150 | 1 | 246 | 247 | (Lauf endet im Dialog) | **0** | — | — |
| laden1151 | 1 | 246 | 247 | (Lauf endet im Dialog) | **0** | — | — (PC @0x0FF9) |

Zwischen den Dialogen (ja_ja): F506 "Yes" Sicherung, F507 `modal=0` (1 Skriptbild, PC bleibt
0x101B, der Sleep zaehlt), F508 Granaten-Dialog; nach dem zweiten "Yes" (F632) noch F633..F670
Ruhe (PC 0x101B -> 0x101E -> 0x102F -> 0x1032), Abfahrt F671 (y -1195). 40 Ruhebilder ohne
Aufnahme = genau Sleep 30 + Sleep 10: die Dialoge verbrauchen keinen Skript-Takt.

### 3.3 (c) "Yes" bei beiden (Lauf `ja_ja`, `hebetisch_belege/ja_ja_debug_auszug.txt`)

```
[sicherung] Modal auf (Hebetisch y=-1205, Ruhe oben: sub04-PC @0x101B)
[input-script] Tick 306 -> F506 Tasten 0x8000
[sicherung] Yes: genommen, Flag (9,53) gesetzt, Item 0x40 in Inventar-Platz 3
[granate] Modal auf (Hebetisch y=-1205, Ruhe oben: sub04-PC @0x101B)
[input-script] Tick 432 -> F632 Tasten 0x8000
[granate] Yes: genommen, Flag (9,56) gesetzt, Item 0x09 x1 in Inventar-Platz 4
```

Modelle weg: ja_ja F640/F652/F660/F668 gegen OHNE-beide F388/F390 (gleiche Ruhelage): **0
abweichende Punkte**; gegen den Lauf mit beiden Modellen (mit1 F386): 667 Punkte. Bilder
`ja_ja_bogen.png` (F400 Sicherungs-Dialog, F504 "Will you take the Fuse?", F512 Granaten-Dialog
zoomt, F600 "Will you take the Hand Grenade?", F640 Fach leer) und `ja_ja_status.png`
(F800..F896 Statusschirm: Sicherung und Granate x1 in der Item-Liste).

### 3.4 (d) "No" bei einem (Lauf `nein_ja`, `hebetisch_belege/nein_ja_debug_auszug.txt`)

Fahrt 1: Sicherung "No" (R F506, Viereck F527) -> `[sicherung] No/voll: nicht genommen ...
(Hebetisch y=-1205)`, Granaten-Dialog an derselben ruhenden Plattform, "Yes" F671 -> Flag
(9,56). Parklage: `[sicherung] Fahrt zu Ende ... Sperre geloest`. Fahrt 2 (Viereck F1157,
Cut_chg(4) F1163): `[sicherung] Modal auf (Hebetisch y=-1205, Ruhe oben: sub04-PC @0x101B)`,
"Yes" F1451 -> Flag (9,53), Item 0x40 in Platz 4. Kein Granaten-Dialog in Fahrt 2 (genommen).

### 3.5 (e) Lade-Weg (`lauf_laden.sh`)

`laden1150`/`laden1151`: `[save] CONTINUE: resumed in room 1150/1151`, `[sicherung] Boot-Weg:
Prop obj_id=4 im Pool`, `[granate] Boot-Weg: Prop obj_id=7 im Pool`, `[fire-aot] slot=1 at F90`,
`[sicherung] Modal auf (Hebetisch y=-1205, Ruhe oben: sub04-PC @0x101B / @0x0FF9)`. Links/rechts
wie am Tuerweg (1.3). Gegenprobe `laden1150_ohne` (Karte mit beiden Flags): weder Pool-Zeile
noch Dialog.

### 3.6 Messfalle

Der erste Lauf mit `RE15_WINDOW_SCALE=3` endete mit rc=1 und 0 Bildern (debug.log bricht nach
F150 ab, ohne Fehlermeldung); die Wiederholung lief sauber (41 Bilder bis EXIT_AT 402). Parallel
lief eine Docker-Bau-Sitzung — als Last-Flattern gewertet, nicht als Befund.
## 4. Riegel/Tests nachgezogen

**Neu `unit_r31_hebetisch`** (`tests/unit/probe_r31_hebetisch.c`, `probes/r31_hebetisch.cmake`),
13 Pruefungen je ROOM1150 und ROOM1151 (Ausgabe `hebetisch_belege/unit_r31_hebetisch.txt`,
ALLES BESTANDEN):

| # | prueft | gemessen (1150 = 1151) |
|---|---|---|
| 1 | Ruhe-Fenster aus den RDT-Bytes, Sleep 30 am Beginn, Setzen-For 10 davor, Abfahrt-For am Ende | [0x101A,0x1042) / [0x0FF8,0x1020) |
| 2 | beide liegen auf (tiefster Punkt -1036) | S -1036,0 / G -1036,0 |
| 3 | Luft unter der geschlossenen Kuppel > 0 | S 6,06 / G 5,88 |
| 4 | Luft unter den offenen Deckeln > 0 (Deckelweg aus den Bytes @0x0FC0 = 150) | S 46,87 / G 34,02 |
| 5 | Grundriss im Achteck | 0 Proben draussen |
| 6 | kein Durchdringen | Abstand 7,11 |
| 7 | Cut 4 (Engine-Kamera): Granate links, Sicherung rechts | Start 191,7 / 213,9; oben 197,7 / 224,0 |
| 8 | Sicherungs-Aufnahme im ersten Ruhebild | Bild 156, y -1206 -> -1205 -> -1205, PC Sleep+1 |
| 9 | Granaten-Aufnahme danach in der Ruhe | Bild 228, y -1205 |
| 10 | jedes Bild mit Aufnahme bei -1205 | 160 von 160 |
| 11 | 40 Ruhebilder ohne Aufnahme (Sleep 30 + Sleep 10), dann Abfahrt | 40, Abfahrt Bild 354 |
| 12 | Yes/Yes: Flags, Props weg, Inventar | ja |
| 13 | Negativ-Kontrolle ROOM1140: kein Fenster | -1 / -1, ruht_oben 0 |

(Die Engine-Rechnung in 7 nimmt alle Proben, nicht nur die sichtbaren; die GEMESSENEN
Schwerpunkte stehen in 1.3.)

**Mutationsproben** (Quelle geaendert, Ziele gebaut, gefahren, per `git checkout` zurueck):

| Mutation | Ergebnis |
|---|---|
| M1: alte y-Schranke `if (py > -1100) return 0;` statt `re15_hebetisch_ruht_oben()` (beide Ticks) | unit_r31_hebetisch 8/9/10/11 ROT (Sicherung Bild 134 y -1105, Granate y -1115, 160 von 160 Aufnahme-Bildern NICHT auf -1205, 0 Ruhebilder); unit_r30_granate 8 ROT; unit_r30_sicherung_nein 2 ROT |
| M2: Sitze an der Naht gespiegelt (Sicherung z 1240 rot_y 2656, Granate z 1380 rot_y 2304) | NUR unit_r31_hebetisch 7 ROT (Granate 228,1 rechts von Sicherung 206,0 — Geometrie symmetrisch, 2..6 gruen); unit_r30_granate 7 ROT; unit_r30_sicherung_sitz "POS_Z rechts der Naht" ROT |

**Bewusst nachgezogen** (die alten Zahlen beschrieben den Runde-30-Entwurf, nicht das Original):

* `unit_r30_granate`: 4 "3 unter dem Boden" -> "auf dem Boden -1036"; 5 Sicherungs-Zylinder um die
  gedrehte Achse statt fest bei x=-280/z 1057..1463; 7 "ganz in der Oeffnung" -> "links der
  Sicherung, Mitte in der Oeffnung" (die Granate liegt mit 35 unter dem linken Deckel);
  8 `oben()` = y == -1205 statt Fenster (-5000,-1100].
* `unit_r30_sicherung_sitz`: "POS_Z = Mitte" und "auf der Naht" -> "rechts der Naht"; "Achse genau
  auf z" -> "naeher an z als an x". POS_X-Mitte, POS_Y = Boden - Radius, Deckelweg 150, "Mitte in
  der Oeffnung" bleiben.
* `unit_r30_sicherung_nein` 2: y == -1205 statt Fenster (-5000,-1100].
* `integration_r30_sicherung_laden` / `integration_r30_granate_laden`: `RE15_EXIT_AT` 250 -> 280;
  das Modal zeichnet jetzt ab Bild 248 (vorher 226) — mit 250 stand die Pruefzeile nur 2 Bilder
  vor dem Ende.
* `RE15_MIN_TESTS` 405 -> 406 (`tools/local_build.sh` Z. 63 und 320/321).

## 5. Suite / Commits

* 4ada307e wip: Dossier + Planungswerkzeuge
* 424177f2 wip: Spielcode (Ruhe-Fenster, Sitze) + Messungen
* a380704a test: unit_r31_hebetisch, r30-Riegel nachgezogen, RE15_MIN_TESTS 406

## 6. Offen

* **Port friert auch im Aufnahme-Zustand 8 ein, das Original nicht.** Nach "No"/voll schrumpft
  das Bild 17 Bilder weg (@0x8001e154); im Original laeuft das Skript da schon wieder (Freeze
  aufgehoben beim Bestaetigen, @0x800285a4, Schnappschuss aus Zustand 5 ohne oberes Halbwort
  @0x8001df3c). Der Port haelt `scd_vm_tick` an, solange `re15_item_modal_active()` (Zustand != 0).
  Allgemeine Abweichung fuer JEDE abgelehnte Aufnahme im Spiel — nicht Teil dieses Auftrags, nicht
  geaendert. Fuer den Hebetisch folgenlos: auch mit 17 laufenden Skriptbildern bliebe die Ruhe
  oben (40 Skriptbilder) ueber beide Dialoge erhalten.
* **Mess-Variante** `tests/unit/probe_r30_sicherung_variante.c` (nur mit
  `-DRE15_R30_SICHERUNG_VARIANTE=ON`, Standard AUS, nicht Teil der Suite) traegt in ihrem eigenen
  `re15_sicherung_tick` weiter die alte y-Schranke -1100. Linkt weiter (kein neues Symbol in
  `sicherung_1150.c`), wuerde aber das alte Zeitverhalten messen — vor Gebrauch nachziehen.
* **Teilweise verdeckt, geometrisch erzwungen:** die Sicherung verschwindet mit gut der Haelfte
  ihrer Laenge unter dem rechten Deckel (auch in Runde 30 lagen ihre Enden unter beiden Deckeln);
  von der Granate liegen 35 Einheiten unter dem linken Deckel. Ganz in die Oeffnung passen beide
  nebeneinander nicht (1.1).
* **PSX-Ziel:** wie Runde 30 — kein Lader fuer die Zusatz-Props; das Ruhe-Fenster
  (`hebetisch_1150.c`) ist Engine-Code und gilt dort mit. **Android:** neue Quelldatei
  `engine/src/hebetisch_1150.c` -> frischer Configure noetig (GLOB-Cache, Memory
  `android-glob-cache`).
* **Sitz, Drehung und Zeitpunkt** sind PORT-WAHL ohne Original-Adresse (das Original hat im
  Hebetisch keine Beute). Belegt sind Geometrie-Bytes, Skript-Zustand und Freeze-Mechanik.
* **Nicht gemessen:** Elza-Durchlauf am Tuerweg in ROOM1151 (1151 nur ueber CONTINUE und im
  Engine-Riegel); "Inventar voll" im neuen Zeitpunkt (Engine-Riegel `unit_r30_sicherung_nein`
  Fall C deckt ihn, gruen).
