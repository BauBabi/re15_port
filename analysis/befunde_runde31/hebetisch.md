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
## 3. Abnahme im echten Spiel
## 4. Riegel/Tests nachgezogen
## 5. Suite / Commits
## 6. Offen
