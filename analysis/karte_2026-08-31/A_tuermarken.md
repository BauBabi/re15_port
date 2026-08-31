# A — WELCHE TUEREN EXISTIEREN WIRKLICH? (ROOM1170, Karten-Seite 5)

Nutzer-Report zu v0.3.70: "Auf dem grossen Rechteck sind wieder 3 Tueren eingezeichnet,
wobei nur eine oben existiert."

**Ergebnis: der Nutzer hat exakt recht. Auf Seite 5 / Rect 1 gehoert GENAU EINE Tuermarke.**
Die anderen zwei sind (a) ein Skript-Warp mit Rechteck 0x0 und (b) ein `sce=0`-Record, den
das Spiel im Leon-Szenario nie scharf schaltet. Zusaetzlich fallen zwei weitere
Fehlerklassen auf: 4-Punkt-Tueren (40 Byte) und die Vermischung der Szenarien Leon/Elza.

Alle Zahlen unten sind selbst gemessen. Sonden im Scratchpad
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/25fdb723-6925-4eae-ab46-1dd9a6a8a4c0/scratchpad/mapA/`:
`probe_aot_live.c` (Live-AOT-Dumper), `scddis.py` (SCD-Disasm mit RE1.5-Groessen),
`walk2.py`, `cmp_static_live.py`, `classify2.py`, `marks_fixed.py`, `marks_scen.py`.

---

## 1. Die Rueckrechnung ist bestaetigt (eigene Messung, VORWAERTS gerechnet)

Statt aus Pixeln zurueckzurechnen habe ich die Generator-Abbildung VORWAERTS nachgerechnet
und gegen die ausgelieferte Tabelle `s_map_marks[]` gehalten.

Zone 0 (Hof) = `{ 0x1170, -13430, -10910, 15658, 17746, 5, 1, 0, 26 }`
(`re15_port/engine/src/re15_map_zones.h:60`), Rect 1 von Seite 5 = `(148,101,48,56)`
(EXE-Tabelle: Anzahl `@0x80076840 + 5*8`, Zeiger `@0x80076844 + 5*8`).

Abbildung (identisch zu `gen_map_zones.py:to_map` und `re15_map_zones.c`, z gespiegelt):
`mx = 148 + fx*48//29088`, `my = 101+56-1 - fz*56//28656`

| Welt (Mitte des Tuer-Rechtecks) | -> Karte | steht so in `s_map_marks[]` |
|---|---|---|
| (2550, 15250)  Slot 0 | **(174,105)** | ja |
| (5660, -7940)  Slot 1 | **(179,151)** | ja |
| (0, 0)  Slot 2 **und** Slot 3 | **(170,135)** | ja (dedupliziert) |

Gegenprobe auf Rect 0 (Zone 1 = Grube, `{0x1170,-28900,-28841,-8784,-17070,5,0,1,27}`,
Rect 0 = `(140,80,48,24)`): alle 7 dort stehenden Marken (3 Tueren + 4 Treppen) werden
pixelgenau reproduziert — (158,81), (157,100), (182,101), (147,87), (147,92), (153,98),
(159,97). **Damit ist das Projektionsmodell verifiziert: 10/10 Marken der Seite 5.**

Ein Kartenpixel ist auf Rect 1 **606 x 512 Welteinheiten** gross. Die im Auftrag genannte
Rueckrechnung `(179,151) -> (5354,-8351)` liegt also innerhalb EINES Pixels von der echten
Record-Mitte `(5660,-7940)`: sie war richtig, nur unscharf.

---

## 2. Woher der dritte Datensatz kommt — und warum er nie eine Tuer ist

`ROOM1170.RDT` `mainScd` Thread 0 (Sektion @Datei `0x0011F4`), disassembliert mit der
**RE1.5**-Groessentabelle (`scd_vm.c:s_opcode_sizes`, Sprungtabelle `PTR_LAB_800744a8`):

```
0x0012 f0x001206  3B Door_aot_set slot=0 sce=2 sat=0x31 band=4  rect(1500,14400,2100,1700)  dest=0x1170 cut=11
0x0032 f0x001226  3B Door_aot_set slot=1 sce=0 sat=0x31 band=4  rect(5040,-9400,1240,2920)  dest=0x10B0 cut=0
0x0096 f0x00128A  13 Switch  var=10                  ; work_vars[0x0A] = EINTRITTS-CUT
0x009A f0x00128E  14 Case   value=0                  ; nur bei Eintritts-Cut 0
0x00A0 f0x001294  06 IfelCk skip=44 -> 0x00D0
0x00A4 f0x001298  21 Ck  bank=3 bit=193 val=0
0x00A8 f0x00129C  3B Door_aot_set slot=2 sce=2 band=0  rect(0,0,0,0)  dest=0x1240
0x00C8 f0x0012BC  04 EvtExec 0x1803                   ; sub03
0x00CC f0x0012C0  07 ElseCk skip=52 -> 0x0100
0x00D0 f0x0012C4  06 IfelCk skip=42 -> 0x00FE
0x00D4 f0x0012C8  21 Ck  bank=3 bit=125 val=0
0x00D8 f0x0012CC  3B Door_aot_set slot=3 sce=2 band=0  rect(0,0,0,0)  dest=0x1170 (SELBST)
0x00F8 f0x0012EC  04 EvtExec 0x180B                   ; sub11
0x0100 f0x0012F4  1A Break
0x0102 f0x0012F6  16 Eswitch
0x0104 f0x0012F8  3B Door_aot_set slot=4 sce=2 band=0  rect(-22370,-18380,2280,910)  dest=0x1130 cut=7
0x0124 f0x001318  06 IfelCk skip=28 -> 0x0144
0x0128 f0x00131C  21 Ck  bank=4 bit=195 val=0
0x012C f0x001320  2C Aot_set slot=5 typ=1 (MESSAGE msg=12, mask 0xFFFF)
0x0140 f0x001334  07 ElseCk skip=38 -> 0x0166
0x0144 f0x001338  3B Door_aot_set slot=5 sce=2 band=0  rect(-22740,-27610,2280,1210)  dest=0x1140 cut=6
0x0166 f0x00135A  3B Door_aot_set slot=6 sce=2 band=4  rect(-11940,-28450,1750,1200)  dest=0x1170 cut=0
0x0186..0x01C2    2C Aot_set slots 7..10, typ 13/13/12/12  = die TREPPEN-Baender
```

### 2a. Marke (179,151) = Slot 1, dest 0x10B0 — `sce=0`, dauerhaft inert

Record `@Datei 0x001226`, Byte `pc[2] = 0x00`.

* Port: `op_door_aot_set` (`re15_port/engine/src/scd_vm.c:3484-3487`) setzt bei `pc[2]==0`
  `g_aot.slots[slot].type = RE15_AOT_TYPE_NONE`.
* Original: der ACTION-Scan verwirft `sce==0` ohne Dispatch —
  `@0x80042f48 lbu v0,0x0(s0)` / `@0x80042f50 beq v0,zero,LAB_8004301c` (naechster Record).
  Der Tastendruck wird nicht einmal verbraucht.
* Ein `Aot_reset` (Opcode 0x46) auf Slot 1 existiert in ROOM1170 **nirgends**. Ich habe
  main00 UND alle 16 sub-Threads durchdisassembliert: die einzigen AOT-Nachtraege im Raum
  sind `Aot_on 2` (sub03 `@Datei 0x001614`) und `Aot_on 3` (sub11 `@Datei 0x0016D0`).
  Der Record ist im Leon-Szenario **permanent tot**.
* **Live-Gegenprobe** (`probe_aot_live.exe 1170`, nach `scd_room_reenter` + 4 Ticks):
  `AOT 1170 1 NONE 11 x=5660 z=-7940 hw=620 hh=1460 dest=10B0`.

Im **Elza**-Szenario ist derselbe Tuer-Record scharf: `ROOM1171.RDT @Datei 0x00126C`
Slot 2, `sce=2`, gleiches Rechteck. Deshalb ueberlebt die Marke im heutigen Generator —
er verschmilzt beide Szenarien (`gen_map_zones.py`: "beide Szenario-Varianten teilen sie").
**Das ist der eigentliche Grund, warum diese Marke trotz `sce=0` gezeichnet wird.**

### 2b. Marke (170,135) = Slot 2 UND Slot 3 — Rechteck 0x0 = Skript-Warp

Beide Records tragen `rect = (0,0,0,0)`. Es sind keine begehbaren Tueren, sondern
**Warp-Traeger, die per `Aot_on` gefeuert werden** — dasselbe Muster wie ROOM1090
(dokumentiert in `re15_port/tests/unit/probe_1090_gate_selfdoor.c`):

* `sub03 @Datei 0x001614: 47 Aot_on 2` — feuert Slot 2 (Warp nach ROOM1240)
* `sub11 @Datei 0x0016D0: 47 Aot_on 3` — feuert Slot 3 (Selbst-Warp im Hof)
* main00 startet genau diese Subs direkt nach dem Install (`EvtExec 0x1803` / `EvtExec 0x180B`).
* Opcode 0x47 = `LAB_800407bc`, ruft `@0x8004082c jalr` -> `PTR_8007469c[rec[0]]`,
  also den sce-Handler DIREKT, ohne Rechteck-Test.

Dass ein 0x0-Rechteck durch Herumlaufen nicht erreichbar ist, ist byte-true belegt.
Der AABB-Test des Originals ist `FUN_80042b64`:

```
80042b68  lh   v0,0x0(a1)    ; rect.x0 (SIGNED, Ecke)
80042b6c  lw   v1,0x0(a2)    ; Spieler-X
80042b70  lhu  a0,0x4(a1)    ; rect.w  (UNSIGNED, VOLLE Breite)
80042b74  subu v1,v1,v0
80042b78  sltu a0,a0,v1      ; w < dx ?  (unsigned -> dx<0 wird riesig = ausserhalb)
80042b7c  bne  a0,zero,raus
80042b84..80042b98            ; dasselbe fuer z mit rect.d @0x6(a1)
80042ba0  ori  v0,zero,0x1   ; DRIN
```

= `(unsigned)(px-x0) <= w && (unsigned)(pz-z0) <= d`, **inklusiv**. Bei `w=d=0` ist die
Zone also genau EIN Punkt: der Welt-Nullpunkt.

Zusaetzlich greift die BAND-Sperre: `@0x80042cb4 andi 0x80` auf `rec[2]` (= `pc[4]`, hier
`0x00`, Bit nicht gesetzt) -> `@0x80042cc0 lbu v1,0x82(s1)` (Spieler-Band) gegen
`@0x80042cc4 lbu v0,0x2(s0)` -> `@0x80042ccc bne -> naechster Record`. Der Spieler steht im
Hof auf **Band 4** (ROOM1170 fuehrt Baender 0/2/4, `re15_aot.h:104`), die Records tragen
Band 0. **Slot 2/3 sind damit doppelt unerreichbar.**

Meine Live-Sonde hat Slot 2 einmal gefeuert (`[aot] DOOR FIRE slot=2 rect=(0,0,hw=0,hh=0)`)
— weil sie den Spieler zum Test auf exakt `(0,0)` und `y=0` (Band 0) setzt. Das ist ein
Sonden-Artefakt und zugleich der Beweis, dass der Port den Original-Test korrekt inklusiv
nachbildet.

### 2c. Warum Slot 3 im Live-Dump gar nicht auftaucht

`Switch var=10` (= `work_vars[0x0A]` = Eintritts-Cut) `Case 0` -> `IfelCk` + `Ck(3,193,==0)`.
Bei frischem Flag-Stand ist Bank3/Bit193 = 0, also laeuft der **THEN**-Zweig
(Slot 2 + `EvtExec sub03`), und `ElseCk @0x00CC` springt nach `0x0100 Break`.
Slot 3 (`@0x0012CC`) liegt im ELSE und wird nie ausgefuehrt. Der Live-Dump zeigt genau das:
Slots 0,1,2,4,5,6,7..10 aktiv, **Slot 3 fehlt**. `re15_aot_reset()` hat im ganzen Port
**keinen einzigen Aufrufer** (`grep -rn "re15_aot_reset("` -> nur die Definition
`aot_common.c:142`), es kann also nichts nachtraeglich geloescht haben.

Gleiche Struktur bei Slot 5: `Ck(4,195,==0)` -> `Aot_set typ=1` (MESSAGE, msg 12) ODER
`Door_aot_set -> 0x1140`. Live (Flag 0) ist Slot 5 `MESSAGE ev=12`.

### 2d. Slot 2 ist KEINE echte Hof-Tuer nach ROOM1240

Frage 3 aus dem Auftrag: `half_w = half_h = 0`, `sce_flags` (= `pc[3]`) `= 0x31`,
`band` (= `pc[4]`) `= 0`. Der Original-Scan kann ihn nur bei Spieler-Position exakt `(0,0)`
UND Spieler-Band 0 treffen — im Hof (Band 4) nie. Die Gegenrichtung bestaetigt es:
`ROOM1240.RDT @Datei 0x00051A` traegt spiegelbildlich
`Door_aot_set slot=0 rect(0,0,0,0) dest=0x1170`. ROOM1240 hat damit **null** begehbare
Tueren, was zur Pre-Intro-Montage passt. Das Paar ist der Cutscene-Handoff
Hof <-> Pre-Intro, kein Durchgang.

---

## 3. Zwei WEITERE Fehlerklassen desselben Generators

### 3a. Falsche Opcode-Groessen (RE2 statt RE1.5)

`gen_map_zones.py:SZ` weicht von `scd_vm.c:s_opcode_sizes` ab bei
**0x38 (3 vs 12), 0x3D (4 vs 3), 0x4C (5 vs 18), 0x4D (22 vs 10), 0x4E (22 vs 5),
0x4F (4 vs 22)** und bei **0x5F..0x8E** (RE2-Laengen statt 1 — die RE1.5-Sprungtabelle
`PTR_LAB_800744a8` endet bei 0x5E, 95 Eintraege, letzter `@0x80074620`). Ausserdem
behandelt der Generator **0x68 als Tuer-Opcode**, was es in RE1.5 nicht gibt.

**Gemessen ueber alle 240 RDTs:** 0 Treffer fuer 0x68, und nur EIN Raum weicht ab —
ROOM1090 (GEN-Tabelle findet 2 Records, RE1.5-Tabelle 3; der verlorene ist der
0x0-Warp `@0x002352`). Die Klasse ist real, der heutige Schaden klein.

### 3b. 4-Punkt-Tueren (40 Byte) werden als 32-Byte-Record gelesen

`pc[3] & 0x80` schaltet den Record von 32 auf 40 Byte
(`LAB_800405bc @0x80040618 lbu v0,0x3(v1)` / `@0x80040620 andi 0x80` /
`@0x80040630 +0x28 (40)` vs `@0x80040634 +0x20 (32)`). Der Rechteck-Teil ist dann **kein
AABB mehr, sondern ein 4-Punkt-Polygon**: `FUN_80042bac @0x80042dcc andi 0x80` auf `rec[1]`
kopiert 8 Halbworte (`@0x80042dd8..0x80042e58`) und testet mit `FUN_80014368`
(point-in-quad, `@0x80042ebc`) statt mit `FUN_80042b64`.

Game-weit gibt es genau **4** solche Records: `ROOM4030/4031 @Datei 0x00047E` und `@0x0004A6`
(`sat = 0xB1`). Mit der 32-Byte-Deutung ergeben sie Muell — das sind exakt die "4 Misses",
die `aot_common.c:501-503` als "non-door scan artifacts in ROOM4030/4031 @0x47e/@0x4a6"
fuehrt. Mit der 40-Byte-Deutung (alle Felder ab `pc[22]` um **+8** verschoben) loesen sie
sauber auf:

| Datei | Quad (4 Punkte) | next_pos | yaw | stage/room | cut | dest |
|---|---|---|---|---|---|---|
| 0x00047E | (-24190,-25490) (-25170,-26560) (-26630,-25220) (-25250,-23930) | (2750,0,9350) | 1536 | 3/4 | 12 | **ROOM4040** |
| 0x0004A6 | (-20490,-24080) (-19400,-25070) (-20740,-26460) (-21840,-25240) | (-3900,0,2100) | 512 | 3/8 | 8 | **ROOM4080** |

Beide Zielraeume existieren (`STAGE4/ROOM4040.RDT`, `ROOM4080.RDT`). **Damit loesen 653/653
Door_aot_set-Records auf, nicht 649/653.** Der Port installiert diese beiden heute mit
Muell-Rechteck (Live-Dump: `x=-36775 z=-38770 hw=12585 hh=13280 dest=BF0A0`) — ein echter
Gameplay-Bug in ROOM4030, unabhaengig von der Karte.

---

## 4. Zahlen ueber ALLE 240 RDTs

Statischer Walk mit RE1.5-Groessen UND den bedingten Vorschueben (0x3B 32/40, 0x2C 20/28)
-> **653 `Door_aot_set`-Records**. Klassifikation:

| Klasse | Anzahl | Kriterium (Beleg) |
|---|---:|---|
| echte begehbare Tuer | **595** | Rest |
| Rechteck 0x0 = Skript-Warp | **36** | `w==0 && d==0`; Test `FUN_80042b64` ist Ecke+Ausdehnung |
| `sce=0`, nie per `Aot_reset(sce=2)` scharf | **18** | `pc[2]==0` und kein 0x46 mit `sce=2` auf dem Slot; Scan-Skip `@0x80042f48-50` |
| 4-Punkt-Tuer (40 B) | **4** | `pc[3]&0x80`; ROOM4030/4031 |

Live-Dump (alle Raeume, `scd_room_reenter` + 4 Ticks, Flag-Stand 0, Eintritts-Cut 0):
**514 AOTs vom Typ DOOR**, davon **31 mit Rechteck 0x0**, plus **68 Slots vom Typ NONE**
(= `sce=0`). Ein reiner Live-Dump filtert also die `sce=0`-Faelle automatisch, die
**0x0-Warps aber NICHT** (die installieren mit `sce=2` regulaer als DOOR).

**Verpasst der statische Walk Tueren? Nein.** Abgleich Live gegen statisch ueber alle Raeume
(Slot + Rechteck-Mitte): **0 Live-Tueren ohne statischen Record**.
Umgekehrt sind 595 - (514-31) = 112 Records statisch-only — die zweig-abhaengigen
Alternativen (z.B. ROOM1170 Slot 5 Tuer-vs-Message, ROOM1140 Slot 1 `sce=0`@0x000A7A vs
`sce=2`@0x000AB2). Fuer eine KARTE sind die richtig: die Karte zeigt den ORT einer Tuer,
nicht ihren aktuellen Schaltzustand.

Stichprobe Live-Tueren (Auftrag Punkt 4):

* ROOM1130: 3x DOOR (dest 1140 / 1120 / 1150) + Slot 3 als MESSAGE (die "It's not necessary
  to go back"-Sperre der Tuer nach 1170) — statisch 4 Tuer-Records, alle mit echtem Rechteck.
* ROOM1140: 1x DOOR (dest 1130) + Slot 1 NONE (`sce=0`@0x000A7A); der zweite Install
  desselben Slots `@0x000AB2` traegt `sce=2` -> Tuer nach 1170 existiert zweig-abhaengig.
  -> 2 Marken.
* ROOM1150: 1x DOOR (dest 1130). Statisch ebenfalls genau 1 Record.
* ROOM1170: 4 echte (Slot 0 Selbst-Tuer, 4 -> 1130, 5 -> 1140, 6 Selbst-Tuer).

---

## 5. Was das konkret an der Marken-Tabelle aendert

Ich habe die Marken mit DENSELBEN Zonen/Rects wie der Auslieferungsstand
(`s_map_zones[]` aus `re15_map_zones.h`) neu gerechnet, nur mit den Filtern.
Kontrolle: **173 neue + 8 entfallene = 181 = exakt die ausgelieferte Tabelle, 0 zusaetzliche**
— das Modell ist deckungsgleich mit dem Generator.

Entfallende Marken `(Seite, Rect, mx, my, kind, zid)`:

```
(1,  0, 135, 120, 0, 42)   ROOM1250/1251 Slot 1  sce=0 tot
(1,  0, 141, 120, 0, 42)   ROOM1250/1251 Slot 2  sce=0 tot
(5,  1, 170, 135, 0, 26)   ROOM1170 Slot 2+3     Rechteck 0x0
(9,  3, 139, 109, 0, 92)   ROOM5120/5121 Slot 2  sce=0 tot
(9,  3, 142, 135, 0, 92)   ROOM5120/5121 Slot 3  sce=0 tot
(9,  7, 178,  92, 0, 83)   ROOM5070/5071 Slot 1  sce=0 tot
(9,  7, 178, 159, 0, 83)   ROOM5070/5071 Slot 2  sce=0 tot
(9, 10, 138, 152, 0, 85)   ROOM5090/5091 Slot 5  Rechteck 0x0
```

**(5,1,179,151) faellt dabei NICHT weg** — weil ROOM**1171** (Elza) denselben Tuer-Record
mit `sce=2` traegt. Erst mit Szenario-Trennung stimmt Seite 5:

| Szenario | Seite 5 / Rect 1 (grosses Rechteck) | Rect 0 (kleines) |
|---|---|---|
| **LEON** (gerade RID) | **(174,105)** — genau EINE Marke | (157,100) (158,81) (182,101) |
| ELZA (ungerade RID) | (174,105) + (179,151) | dieselben drei |

Das ist wortwoertlich der Nutzer-Befund. Gesamtzahl Marken: LEON 151, ELZA 152
(statt 181 verschmolzen).

---

## 6. EMPFEHLUNG

**Nicht** den Generator auf einen reinen Live-Dumper umstellen. Gemessener Grund: ein
Live-Lauf sieht nur EINEN Flag-/Cut-Zustand (514 statt 595 Tueren) und filtert die
0x0-Warps ohnehin nicht. Stattdessen den statischen Walk reparieren und den Live-Dumper
als PRUEFUNG danebenstellen:

1. **`re15_port/tools/gen_map_zones.py`, `SZ`-Tabelle (Zeilen ~66-79):** durch die
   RE1.5-Groessen aus `scd_vm.c:s_opcode_sizes` ersetzen (0x38=12, 0x3D=3, 0x4C=18,
   0x4D=10, 0x4E=5, 0x4F=22, 0x5F..0x8E=1) und den `0x68`-Tuer-Zweig in `read_rdt`
   ersatzlos streichen (existiert in RE1.5 nicht, 0 Treffer game-weit).
2. **`read_rdt` (Zeilen ~112-135):** bedingter Vorschub
   `0x3B -> (b[3]&0x80) ? 40 : 32` und `0x2C -> (b[3]&0x80) ? 28 : 20`
   (`@0x80040618-34` bzw. `@0x80040590-9c`). Bei der 40-Byte-Form die Felder ab `pc[22]`
   um **+8** verschoben lesen und die Marken-Position aus dem Schwerpunkt des
   4-Punkt-Quads (`pc[6..21]`) bilden.
3. **Drei Filter beim `doors.append`:**
   * `if rw == 0 and rd == 0: continue` (Skript-Warp; `FUN_80042b64` ist Ecke+Ausdehnung,
     0x0 = ein einziger Punkt)
   * `if b[2] == 0 and 2 not in aot_reset_sce[slot]: continue` (`@0x80042f48-50`)
   * dafuer einmal pro RDT alle `Aot_reset` (0x46, 10 Byte) einsammeln: `{pc[1]: {pc[2]}}`.
4. **Szenario-Trennung im Marken-Block (Zeilen ~355-380):** die Deduplizierung ueber
   `seen` verschmilzt heute Basisraum und `+1`-Variante. `re15_map_mark_t` (generierter
   Header, Zeile ~211) um ein Byte `scen` erweitern (0 = nur Leon, 1 = nur Elza,
   2 = beide) und in `re15_port/engine/src/re15_map_zones.c:re15_map_mark_get()`
   (Zeilen 224-237) zusaetzlich gegen `(g_current_room_id & 1)` gaten. Die Variante liegt
   bereits in Bit 0 der Room-ID (`aot_common.c:517  | (g_current_room_id & 0x000Fu)`).

**Verifikation danach** — die Sonde liegt fertig im Scratchpad (`probe_aot_live.c`) und
laesst sich unveraendert nach `re15_port/tests/unit/` uebernehmen. Sie dumpt pro RDT alle
installierten AOTs mit Typ, Position, Band, dest, half_w/half_h. Als ctest formulierbar:

* jede Live-DOOR mit `hw>0 || hh>0` hat eine Marke, und
* keine Marke liegt auf einem Slot, der live `NONE` ist oder `hw==hh==0` hat.

Fuer ROOM1170 ergibt das die eine Marke (174,105); ROOM1130 vier, ROOM1140 zwei,
ROOM1150 eine — alles vom Live-Dump bestaetigt.

Bauen der Sonde (funktioniert, so gemessen):

```
PATH="/c/msys64/mingw64/bin:/c/msys64/usr/bin:$PATH" gcc -O1 -o probe_aot_live.exe \
  probe_aot_live.c re15_port/tests/test_support.c \
  -Ire15_port/include -Ire15_port/engine/include \
  -DRE15_ASSET_PSX_DIR='"C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX"' \
  re15_port/build/engine/libre15_engine.a -lm
```

---

## 7. Nebenbefunde (nicht Karte, aber gemessen)

* **ROOM4030/4031 Slot 1+2 sind im Port kaputt.** `op_door_aot_set`
  (`re15_port/engine/src/scd_vm.c:3429-3496`) liest bei `pc[3]&0x80` weiterhin die
  32-Byte-Feldlage; der Kommentar dort sagt selbst "4P-Feld-Offsets (Rect-Punkte ab
  +0x20) = Folgearbeit; hier nur PC-Sync". Live-Ergebnis: zwei Riesen-Tueren mit
  `dest=0xBF0A0` / `0xC5F00` statt der echten Tueren nach ROOM4040 / ROOM4080. Ausserdem
  fehlt der Quad-Pfad (`has_quad` + `FUN_80014368`) fuer Tueren.
* **35 von 653 Tuer-Rechtecken haben ungerade `w` oder `d`.** Der Port rechnet
  `cx = x + w/2`, `hw = |w|/2` und testet `|px-cx| <= hw`; das Original testet
  `(unsigned)(px-x0) <= w` (Ecke + volle Ausdehnung). Bei ungeradem `w` ist die Zone im
  Port auf einer Seite eine Einheit kuerzer. Nicht geprueft, ob das je spuerbar wird —
  als Notiz.
