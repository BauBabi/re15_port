# ROOM1150 „Modell in der Mitte" — was es ist, welche Animation daran haengt, und wie man sie ausloest

Nutzer-Frage (woertlich): *„du sagtest du hast dieses tisch model in der mitte von Irons Office
repariert zum ausloesen. wie triggert man das?"* — und auf meine (falsche) Antwort „das ist die
Item-Box": *„nein, nicht die item boxen. irgendwie konnte man in den modell in der mitte noch eine
ani[mation]"*.

## 0 Kurzfassung

* ROOM1150 = **CHIEF OFFICE** (Debug-Sprungtabelle Stage 0, Platz 21 → `re15_port/include/debug_jump_table.h`).
* In der Mitte des Raums steht ein **Hebetisch**: Objekt 0 (MD1 6804 B, 251 Dreiecke) ist eine
  **Plattform mit Papierstapeln und einer Ablage**, die aus einem Schacht in der Tischplatte
  hochfaehrt. Darauf sitzen **Objekt 1 und 2** (je 692 B, 16 Dreiecke) — die **zwei Haelften eines
  gestuften Deckels** ueber der Ablage. Objekt 3 (1572 B) ist die Item-Box („Baggage box"-Textur),
  die steht am Rand, nicht in der Mitte.
* Die Animation ist **sub04** (`@0x0F96-0x10B6`): Deckel oeffnet (beide Haelften ±150 auf der
  lokalen Z-Achse), Plattform faehrt 910 Einheiten hoch, wieder runter, Deckel schliesst — mit
  drei Geraetegeraeuschen (`Se_on` Bank 2, ids 0x0a/0x0c/0x0d).
* **Im Auslieferungsstand kann man das NICHT ausloesen.** Der Ausloeser existiert vollstaendig
  (Rechteck + Nutzlast „Event sub 4") in `main00 @0x0D7E`, ist aber mit **`sce = 0` abgeschaltet**;
  Handler[0] `@0x8004305C` ist inert und der ACTION-Scan ueberspringt sce-0-Records
  (`@0x80042f48-50`). Kein Aot_reset im ganzen Raum schaltet ihn je scharf.
* **Neuer Port-Befund (echter Defekt):** die **Anhaenge-Form** `Obj_model_set pc[5] & 0xC0 == 0xC0`
  (LAB_80040914 `@0x80040a84-9c`) fehlte im Port. Objekt 1 und 2 haengen im Original an Objekt 0;
  im Port standen sie bei **(0,0,0)** — 27 000 Einheiten neben dem Raum. Die Deckelbewegung, also
  genau die Animation, an die sich der Nutzer erinnert, war **unsichtbar**. Gefixt + Sichtprobe.
* Die Port-Aktivierung des schlafenden Inhalts sitzt jetzt auf dem **autorisierten Record**
  (Slot 1 `@0x0D7E`) statt auf einem erfundenen Record in Slot 60, der gemessen unerreichbar war
  **und** eine RVD-Kamerazone ueberschrieb.

## 1 Reproduktion / Messung

Werkzeuge (neu bzw. benutzt):

| Werkzeug | Zweck |
|---|---|
| `re15_port/tools/scd_dump_room.py` | opcode-exakter SCD-Walk (main00 + alle subs), Abbruch statt Desync |
| `re15_port/tools/rdt_objekt_ansicht.py` (**neu**) | zeichnet jedes Prop-MD1 einer RDT einzeln + im Verbund |
| `re15_port/tests/unit/probe_irons_mittelmodell.c` (**neu**) | misst Records, Objekte, Szene, Ausloesbarkeit |

### 1.1 Was die vier Objekte sind — angesehen, nicht geraten

`analysis/befunde_2026-09-20/irons-mittelmodell/objekte_1150.png` (fuenf Ansichten je Objekt
plus Textur, letzte Zeile = Objekt 0+1+2 in EINEM Rahmen):

| Objekt | MD1 | lokale Ausdehnung | was es ist |
|---|---|---|---|
| 0 | 6804 B, 251 Dreiecke | X 1260 Y 1884 Z 1800 | Hebe-Plattform mit Papier-/Aktenstapeln und dunkler Ablage |
| 1 | 692 B, 16 Dreiecke | X 411 Y 149 Z 385 | vordere Deckelhaelfte ueber der Ablage |
| 2 | 692 B, 16 Dreiecke | X 411 Y 149 Z 385 | hintere Deckelhaelfte |
| 3 | 1572 B, 58 Dreiecke | X 1440 Y 2160 Z 1800 | **Item-Box** (Textur traegt woertlich „Baggage box") |

Objekt 1 und 2 liegen mit ihren Vertizes bereits IM Rahmen von Objekt 0
(x[-485..-74], y[-1185..-1036], z 1260..1645 bzw. 875..1260 — zusammen eine durchgehende Platte,
in der Mitte bei z=1260 geteilt). Ihre Installations-Position ist (0,0,0), weil sie **Kinder** von
Objekt 0 sind (§2.2).

### 1.2 Das Rechteck, das die Szene ausloesen wuerde

`probe_irons_mittelmodell` (Sonde faehrt den echten Scan-Pfad `re15_aot_scan` mit demselben
Flag, das `game_step_common.c:894` aus dem Pad setzt):

```
slot1   typ=0 ev=4 flags=0x31 x=[-21800..-20300] z=[-20000..-16700]   <- der Ausloeser
slot4   typ=5 ev=0 flags=0x31 x=[-21900..-17700] z=[-21000..-15800]   <- grosser Examine-Text
RVD-Kamerazonen in ROOM1150: 16 -> belegte Slots 48..63
Slot 60 vor dem Port-Einbau: aktiv=1 typ=3 (CAM_SWITCH) cam 1->0 x=[-19205..-18005] z=[-12398..-9998]
Aktionsdruck-Raster IST-Stand : 472 von 2816 Kombinationen starten sub04 (davon 370 auf Band-0-Zellen)
Aktionsdruck-Raster ALT-Stand :   0 von 2816
```

### 1.3 Die Szene im laufenden Spiel

`RE15_DEBUG_JUMP=1150@30 RE15_FORCE_EVENT="4@60" RE15_FRAMEDUMP=...`:
`szene_ohne_anhaenger.png` (vorher) und `szene_mit_anhaenger.png` (nachher), gleiche Bildnummern.
Vorher traegt die Plattform nur die glatte Oberseite mit der dunklen Ablage; nachher liegt der
gestufte Deckel darauf und **schiebt sich sichtbar auf**. Ohne Szene ist an der Stelle nur ein
dunkler Schacht in der Tischplatte zu sehen (Vergleichslauf ohne Szene).

## 2 Original-Mechanismus mit @0x

### 2.1 Der Ausloeser ist ab Werk ABGESCHALTET

ROOM1150.RDT `main00 @0x0D7E` (identisch in ROOM1151 `@0x0D7E`):

```
2c 01 00 31 00 00  d8 aa e0 b1 dc 05 e4 0c  ff 00 18 04 00 00
   |  |  |                                   \_________/
   |  |  +-- flags 0x31 = ACTION | forward     Nutzlast: Event-Form, sub = p1>>8 = 4
   |  +----- sce = 0  -> Handler[0] @0x8004305C, INERT
   +-------- Slot 1     Rect Ecke(-21800,-20000) Groesse(1500,3300)
```

Zum Vergleich die Nachbarn, die dieselbe Nutzlast-Form mit `sce = 3` tragen und deshalb leben:
`@0x0DA6` Slot 3 `... ff 00 18 06` → sub06 (Save-Telefon), `@0x0DCE` Slot 5 `... ff 00 18 07` →
sub07 (Item-Box). Der ACTION-Scan ueberspringt sce-0-Records (`@0x80042f48-50`); ein `Aot_reset`
auf Slot 1 kommt in keinem der neun Subs des Raums vor (vollstaendiger Walk, `scd_dump_room.py`).
**Antwort auf die Nutzerfrage: im Original gibt es keinen Weg, die Szene auszuloesen.**

### 2.2 Anhaenge-Form `Obj_model_set pc[5]` — LAB_80040914

```
8004097c: lbu a0,5(a2)              ; a0 = Opcode-Byte pc[5]
80040a04: andi v1,a0,0xc0
80040a48: lui v0,0x8007 / addiu v0,v0,11596   -> 0x80072d4c        (0x00)
80040a58: lui v0,0x800b / addiu v0,v0,-13708  -> 0x800aca74        (0x40)
80040a68..a80: v0 = 500*a0 + 0x8009d24c                            (0x80)
80040a84..a9c: v0 = 148*a0 + 0x800ad0e0                            (0xC0)
80040aa0: sw v0,116(a1)             ; Pool-Eintrag +116
```

`0x80072d4c` habe ich nachgelesen: Diagonale 4096, Translation 0 = **Einheitsmatrix**.
Fuer `a0 = 0xC0 | n` ergibt der 0xC0-Zweig `0x800ad0e0 + 148*(192+n) = 0x800b3fe0 + 148*n`
= Objektpool `0x800b3f98` + `148*n` + `0x48`.

Der Zeichner `FUN_8002c18c` macht je Pool-Eintrag:

```
RotMatrix(pool+104, pool+0x20)                 ; lokale Drehmatrix aus rot
FUN_80022da0(pool+116, pool+0x20, pool+0x48)   ; Weltmatrix = ELTERN o LOKAL
if (pool+0x08 == 4) pool+0x48.t[1] -= 900      ; @0x8002c23c/4c
FUN_80014368(&DAT_800b3ff4 + i, DAT_800ac790)  ; Region-Cull auf pool+0x48.t (!)
```

`FUN_80022da0(eltern, lokal, aus)` dreht jede Spalte der lokalen Matrix mit der Elternmatrix
(`gte_SetRotMatrix(eltern)` + 3× `rtir`) und setzt `aus.t = eltern.rot * lokal.t + eltern.t`
(`gte_SetTransMatrix(eltern)` + `rt`). Also klassische Verkettung. `pool+116` ist damit die
**Elternmatrix**, und `0xC0|n` heisst: *dieses Objekt haengt an der Weltmatrix von Objekt n*.

**Zensus ueber alle 240 ausgelieferten RDTs** (672 `Obj_model_set`, Walk opcode-exakt,
0 abgebrochene Regionen): `pc[5] = 0x00` **666×**, `0x80` **2×** (ROOM4030/4031 Objekt 0),
`0xC0` **4×** — und das sind genau ROOM1150/1151 Objekt 1 und 2. `0x40` kommt nie vor.

### 2.3 sub04 — die Szene (`@0x0F96..0x10B6`)

```
@0x0F96  Set(2,0,1) / Set(2,2,1)                 Cutscene-Klammern
@0x0F9E  Work_set(3,0)                           -> Objekt 0
@0x0FA2  Se_on(2, 0x0a)   Sleep 5   Cut_chg(4)
@0x0FB4  Pos_set(-20700, -305, -17460)           Plattform in den Raum holen
@0x0FC0  For 15 { Work_set(3,1) Speed_set(2,+10) Add_speed ;
                  Work_set(3,2) Speed_set(2,-10) Add_speed }   Deckel auf, ±150 LOKAL
@0x0FE2  Se_on(2,0x0c)  Speed_set(1,-10) For 91 Add_speed      Plattform 910 hoch
@0x1000  Se_on(2,0x0d)  Speed_set(1, +1) For 10                Nachlauf
@0x1022  Se_on(2,0x0a) / 0x0c ; Speed_set(1,+10) For 90        wieder runter
@0x104C  Se_on(2,0x0d) ; ±2 / 2 Bilder                         Setzen
@0x1078  For 15 { ... }                                        Deckel zu
@0x109E  Pos_set(-20700, -20224, -17460)                       parkt sich selbst zurueck
@0x10A6  Set(5,0,0) Set(2,0,0) Set(2,2,0) Cut_old              Klammern loesen
```

Y ist nach UNTEN positiv (Gegenprobe: Item-Box Objekt 3 steht auf `y = 0` und ihr Mesh reicht
`y[-2160..0]`, also nach OBEN). `-305` = 305 ueber dem Boden, `-1215` = 1215 ueber dem Boden.
Kamera `cut4` steht auf `(-21942,-2160,-18378)` und blickt auf `(-19980,-1566,-18396)`, also aus
1,2 m Abstand von Westen auf den Tisch — deshalb schiebt sich die hochfahrende Plattform durch
das Bild.

## 3 Port-Ist (vor dieser Runde)

1. `re15_port/engine/src/scd_vm.c` `op_obj_model_set` las `pc[1] pc[2] pc[4] pc[6..7] pc[10..21]
   pc[22..33]` — **`pc[5]` gar nicht**. Objekt 1/2 landeten auf ihrer Roh-Position (0,0,0).
2. `re15_port/platform/pc/main.c` gab dem Region-Cull `re15_prop_culled(...)` die **rohe**
   Position `g_scd.props[pi].x/z` statt der Translation der Weltmatrix — fuer ein angehaengtes
   Objekt ist das (0,0,0), also immer ausserhalb der Region = weggeschnitten.
3. `re15_port/engine/src/scd_room_setup.c` installierte als „schlafender Content"-Aktivierung
   einen **erfundenen** Record `re15_aot_set(60, GENERIC, 4, -20700,-17460, 900,900)`.
   Gemessen: 0 von 2816 Aktionsdruecken loesten damit sub04 aus (der grosse MESSAGE-Record Slot 4
   liegt in der Scan-Reihenfolge davor und verbraucht den Druck), und Slot 60 ist in ROOM1150 gar
   nicht frei — der Raum belegt mit 16 RVD-Kamerazonen die Slots 48..63 (`rdt_common.c:441`
   vergibt von oben nach unten), Slot 60 war die Zone `cam 1->0` am Eingang.

## 4 Umsetzung

| Datei | Aenderung |
|---|---|
| `re15_port/include/re15_scd.h` | Prop-Feld `parent_obj` (Herleitung im Kommentar, `@0x80040a04-aa0`) |
| `re15_port/engine/src/scd_vm.c` | `op_obj_model_set` liest `pc[5]`; `0xC0|n` → `parent_obj = n`. Zusaetzlich: Armieren des autorisierten Slot-1-Records im sce-0-Zweig (statt Slot 60) |
| `re15_port/platform/pc/main.c` | `pc_prop_rot_q12()` + `pc_prop_world()` (Elternkette, Typ-4-Absenkung an der Original-Stelle); Region-Cull liest jetzt die WELTposition |
| `re15_port/engine/src/scd_room_setup.c` | erfundener Slot-60-Record entfernt |
| `re15_port/tests/unit/test_dormant_activation.c` | Pin auf den autorisierten Record + auf `parent_obj` |
| `re15_port/tests/unit/probe_irons_mittelmodell.c` (neu) | Sonde: Records, Objekte, Anhaenge-Form, Deckelbewegung, Ausloese-Raster, ROOM6020-Messung |
| `re15_port/tools/rdt_objekt_ansicht.py` (neu) | Ansichtsblatt der RDT-Objektmodelle |

Suite: **328/328 gruen** (`re15_port/build_r18`, 164 s).

## 5 Offen / ehrlich

1. **Live-Playtest der Aktivierung steht aus.** Auf dem Engine-Scan-Pfad feuert der armierte
   Record (370 begehbare Trefferstandorte). Im laufenden Spiel konnte ich es NICHT automatisiert
   nachstellen: der Autopilot erreicht den Punkt am Tisch nicht (er pendelt um die Tischgruppe),
   und `RE15_PLAYER_POS` friert den Lauf ein — nachgemessen mit einer Gegenprobe OHNE Teleport,
   die genauso einfriert, also ein Harness-Artefakt und kein Befund ueber den Ausloeser
   (`harness_ohne_ausloesen.png`).
   Bitte im Spiel probieren: in ROOM1150 an die **Westseite des Tisches in der Raummitte**
   stellen, zum Tisch schauen, Aktionstaste (Tastatur J). Wenn stattdessen der Untersuchungstext
   kommt, einen Schritt versetzen — der grosse Text-Record deckt denselben Bereich ab.
2. **`pc[5] & 0xC0 == 0x80`** (Figuren-Pool `0x8009d24c + 500*a0`) ist NICHT umgesetzt. Betrifft
   genau zwei Records: ROOM4030/4031 Objekt 0. Eigener Auftrag.
3. **ROOM6020 hat denselben Slot-60-Konflikt.** Gemessen (`probe_irons_mittelmodell`):
   18 RVD-Kamerazonen → Slots 46..63, Slot 60 ist dort eine Kamerazone, und die
   Item-Box-Aktivierung des Ports schreibt hinein. Nicht angefasst (die Position dort ist eine
   dokumentierte Port-Wahl ohne autorisierten Record), aber sie gehoert unter die Zonen gelegt.
4. **Wofuer der Hebetisch erzaehlerisch gut ist**, sagt das Skript nicht — es gibt keinen Text,
   keine Flagge und keinen Nachfolger; sub04 setzt und loest seine Klammern selbst. Ich behaupte
   deshalb keine Deutung, sondern beschreibe nur die Mechanik.
