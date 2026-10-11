# Runde 36 — Spur R "dachkiste" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_dachkiste`, Zweig `r36/dachkiste`, Basis r36/integration 99398dc7
(elf integrierte Spuren + G/H/M; Spur A objfall_r36.c integriert). Diff-/Log-Bezuege gegen 99398dc7.

## Auftrag (Orchestrator, aus dem Befund der Spur H, Abschnitte 2.2/2.4 und OFFEN)

Die Kiste obj 1 auf dem DACH von ROOM1090/1091 ist die einzige Stufe zum Tor nach ROOM1100 (Tuer-Record
slot 1 @0x0213A verlangt Band 6 = Kistenoberkante y -10800; aus ROOM1100 fuehrt keine Tuer zurueck). Seit
Spur A (Objekt-Fall FUN_8002c444 in engine/src/objfall_r36.c) kann die Kiste vom Dach fallen — dann waere
das Tor unerreichbar (Softlock). Nutzer-Wortlaut der Spur A: "Das macht ein Fortschritt im Spiel unmoeglich"
— ein vom Port erzeugter Softlock ist ausgeschlossen.

1. An der exe messen, ob Leon (und Elza in ROOM1091) die Dach-Kiste im echten Spiel bis zum Fall-Ausloeser
   schieben kann (echter Weg: aus ROOM10F0 ueber den Tritt, Tuer nach 1090, Kiste in jede moegliche Richtung).
2. Am ORIGINAL messen (DuckStation, Savestate stage_saves/room1090_orig.sav): laesst das Original die Kiste
   bis zur Absprungzelle schieben (Schubpruefung FUN_8003b558 je Schritt @0x8003b634-660, Objekt-Pass
   @0x8002c0fc-c14c), oder blockt es vorher?
3. Blockt das Original -> der Port blockt an derselben Stelle mit demselben Mechanismus (Beleg @0x..., Pin).
   Laesst das Original die Kiste fallen -> byte-true belassen, Softlock als Grenze benennen, Pin auf den
   Original-Weg.
4. Dieselbe Pruefung fuer ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6: haengt dort ein Fortschritt an der Kiste?

## Stand (Arbeitsprotokoll)

* 2026-10-11 Start: Baum sauber auf 99398dc7; AUFTRAG/VERTRAG/H 2.x/A Nachbesserung 3/objfall_r36.c/
  probe_r36_objfall.c/aot_common.c re15_prop_push_tick/re15_collision.c gelesen. Dossier angelegt.
* Bau: `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`; Messkopie
  `re15_port/build/platform/pc/re15_pc_r36r.exe`.
* Geometrie ROOM1090 (SCA, eigener Dumper `R_belege/sca_dump.py`, Layout wie rdt_common.c @RDT+0x20): s. Abschnitt
  RE-Belege. Spielstand `stage_saves/room1090_orig.sav` (Hauptbaum, nur gelesen): MZD-Disc HASH-957757946319438E,
  `@0x80026e4c = 08 00 e0 03` (Auslieferungs-EXE, nicht die gepatchte), Leon AUF DEM DACH (-5282,-9000,-1514)
  Blick 672 Band 5, obj 1 (Dach-Kiste) auf dem Record (-5960,-9000,-15482) Band 5 w0 0x101, obj 0 schon auf
  x -9836 (sub07 gefeuert). Idealer Startpunkt fuer die Original-Messung.
* Erkundungslauf exe `R_belege/lauf_r1` (JUMP 1090, Spieler am Ankunftspunkt der Tuer aus 10F0 (-13600,1300) Blick
  3072 = +Z): R dreht den Blick AUFWAERTS (3072 -> 3264/3648/4032 -> 4128 = 32 in 12 Bildern, ~96/Bild, 0.37 s
  ~ 1024), L abwaerts; Gehen ~74 Einheiten/Bild (+~444 Auslauf nach dem Loslassen); Suedwand: der Spieler bleibt
  bei z 3370 stehen = Absprungzelle r1[1] (z 3826 - 450 - Haut) BLK1 — die Karte stimmt an der exe.

## Punkt 1 — Messung an der exe (Leon ROOM1090, Elza ROOM1091)

(folgt)

## Punkt 2 — Messung am Original (DuckStation, room1090_orig.sav)

Werkzeug `R_belege/orig_1090_push.py` (Muster analysis/befunde_runde35/H_raeume/re2_mess/re2_gdb_grab.py + Skill
re15-room-capture): DuckStation `-batch -statefile room1090_orig.sav <MZD-cue>`, GDB-Server 127.0.0.1:2345
(settings.ini gesichert, EnableGDBServer/ConfirmPowerOff/LogToFile fuer die Sitzung gesetzt, am Ende aus der Sicherung
zurueckgeschrieben — geprueft `EnableGDBServer = false`), virtueller Pad (vgamepad/ViGEm). Haltepunkte `Z0` auf
@0x8002bfc0 (Schub angenommen), @0x8002c0b0 (Schub gesperrt, vor der Ruecksetzung @0x8002c0b8-d4) und @0x8002c13c
(Fall-Ausloeser); je Halt obj 1 (0x800b3f98+0x94: +0x34/+0x38/+0x3C, +0x04, +0x05, +0x82, +0x8C) und Spieler
(0x800aca54: +0x34/+0x3C, +0x6A, +0x82, +0x05) aus der RAM. Navigation geschlossen geregelt (Blick/Lage lesen, Pad
drehen/laufen).

Werkzeug-Befunde (gemessen, `R_belege/orig_diag*/log.txt`):
* DuckStation antwortet auf ^C mit `S02` (anders als in H_raeume r35 notiert) — Stopp-Antworten werden weggelesen.
* `ConfirmPowerOff = true` haelt das geordnete Schliessen (taskkill ohne /F) im Dialog — fuer die Sitzung false.
* Am Bus hingen ZWEI XInput-Geraete (DuckStation-Log: "Controller 2 inserted -> SDL-0", "Controller 1 -> SDL-1"; ohne
  laufendes Skript zeigt Get-PnpDevice EIN "Xbox 360-Controller fuer Windows" USB\VID_045E&PID_028E) — der eigene
  Pad wurde SDL-1 und kam nie im Spiel an (Padwort 0x800ac768 = 0, Spieler stand). Fuer die Sitzung werden die
  [Pad1]-Bindungen zusaetzlich an SDL-1/SDL-2 gebunden (`pad_bind_alle`); danach: UP -> 0x800ac768 = 0x11, Leon laeuft
  (z -1514 -> -3549 -> -5568 -> -7587 je Sekunde, dabei an der Saeule r1[2] entlang: x bleibt -5282 = -4814-450-18),
  RIGHT 1 s -> Blick 672 -> 3456 (= +2784 = +96/Bild, derselbe Drehsinn wie der Port).
* Die Emulation laeuft nach `c` (Haltepunkt FUN_8002bd44 @0x8002bd44 trifft je Bild, `S05`).

(Messlauf folgt)

## RE-Belege

### B1 — Geometrie ROOM1090/1091 (SCA @RDT+0x20, 12-B-Zellen; Bandnibble = floor>>4; `R_belege/sca_dump.py`)

Typwort einer Zelle (FUN_8003b7f0 `lhu v0,0(a1)` @0x8003b900 = Zelle+10) = u1 | floor<<8. Zwei Praedikate:
* **Absprung** (Objekt-Pass @0x8002c130 `andi v0,v0,0x2` auf das Typwort) = u1 & 2.
* **Schubsperre Klasse 2** (FUN_8003b558 @0x8003b64c-660: `lhu v0,6(s0)` = Zelle+8, `sll 16 / sra 24` = u0,
  `and v0,s6,v0` mit s6 = a1 & 0xff = 2) = u0 & 2. Spielerwand = u0 & 1 (collision_constrain_impl, Maske 1).

Band-5-Zellen (Dach), die beides sperren (u0 0xff): r2[0]=r3[4] Bruestung x -7256..4624 z -18560..-16403
(@0x01F20/@0x01FD4); r3[2] Westwand x -9200..-6878 z -18524..265 (@0x01FBC); r3[3] x -15464..-6878 z -1234..491
(@0x01FC8); r2[6]=r3[13] Lueftungsblock x -3400..3400 z -17400..-15397 (@0x01F68); r2[1]=r3[6] x -4538..3994
z -5276..-3602; r2[2]=r3[7] Ostwand x 2752..4552 z -17120..-3584; r1[2]=r3[1] Saeule x -4814..-2718 z -5184..4300;
r1[3]=r3[5] x -16616..-14672 z -722..4819; r1[4] x -8191..-2726 z 3432..4425; r1[5] x -16400..-11504 z 3404..4401.
Absprungzellen Band 5 (u0 0x01 = sperren NUR den Spieler, u1 0x0e, Wort 0x530e): r0[4]=r1[14]=r2[7]=r3[16]
x -4100..3100 z -3600..7896 (@0x01E18); r1[1] x -11752..-8058 z 3826..8129 (@0x01E3C). Beide liegen SUEDLICH der
Dachplatte; die Kiste muesste von z -15482 um >= 11432 nach Sueden (Mitte z >= -4050 bzw. >= 3376).

Lage der Kiste obj 1 (Record @0x02190: (-5960,-9000,-15482), Box c(0,-900,0) h(900,900,900), r = hx = 900
@0x8003b68c-690): Westflanke x -6860 gegen die Westwand r3[2] (Ostkante -6878): **18 Einheiten** Luft (Mitte
>= -5978, `sltu` @0x8003b764 halboffen); Nordflanke z -16382 gegen die Bruestung (Suedkante -16403): **21 Einheiten**
(Mitte >= -15503). Oestlich bis zum Lueftungsblock: Mitte < -4300 (1660 Einheiten).

### B2 — Schubkette FUN_8002bd44 (selbst disassembliert, re15_disasm.py dis 0x8002bf14 76 / 0x8002bea8 28)

```
8002bf14: lbu v1,8(s2) / bne v1,v0(=4)          Typ 4
8002bf24: lw v0,-32(s6) / andi 0x1 / beq        UP gehalten (0x800ac768 & 1)
8002bf38: lbu v1,846(s6) / lbu v0,130(s2) / bne  Spielerband 0x800acad6 == obj+0x82
8002bf50: jal 0x8002d1e8 (a0=Spieler, a1=obj, a2=0)  Reichweite: Punkt 563 voraus, |dx|,|dz| <= 900
8002bf60-74: obj+0x8C++ / = 0
8002bf78-84: obj w0 & 0x80 -> 0x8002c0b8 (Ruecksetzung)
8002bf8c-94: obj+0x8C != 9 -> 0x8002c0b8
8002bfa4: jal 0x8002cabc (Spieler, obj, a2=1)  SCHUB: obj = Spieler +- (hx+450) auf der Achse |dx| vs |dz|
8002bfb0: jal 0x8003b558 (obj, a1=2)           Schubpruefung Klasse 2
8002bfb8: bne v0,zero,0x8002c0b0                gesperrt -> 8002c0b0: sb s7(=10),140(s2); 8002c0b8-d4 Ruecksetzung
8002bfc0: sb v0(=8),140(s2)                     angenommen
8002bfc4-dc: 0x800aca3c |= 0x2000               Kontaktbit
8002bfe0-e4: 0x800aca59 != 8 -> 0x8002c0b8      nur im Spieler-Substate 8 bleibt der Schub
8002c0d8-e8: jal 0x8002cabc (Spieler, obj, 0)   DANACH Spieler aus der Kiste schieben
```
Reihenfolge im Bild: Spieler-FSM (geht in die Kiste hinein) -> Schub (Kiste = Spieler +- 1350) -> Ausschiebung.
Darum kann die Kiste nur in die Richtung wandern, in die der Spieler laeuft, und nur, wenn der Spieler auf der
GEGENSEITE steht (|d_axis| = 1350 bei Beruehrung; Achse = die mit dem groesseren |d|, @0x8002cce8-d10).
FUN_8003b558 (dis 0x8003b558 160): @0x8003b634-644 Bandnibble == obj+0x82; @0x8003b64c-660 u0 & a1; @0x8003b668-674
obj w0 & 8 -> uebersprungen; @0x8003b684-690 hx == hz -> r = hx; @0x8003b738-768 (u32)(cx - (x0 - r)) < w + 2r;
@0x8003b770-794 dito z; Treffer -> FUN_8003da78 + s5 |= 1; Rueckgabe s5. Der Port (re15_collision_box_blocked,
re15_collision_push_prop, re15_prop_reach_test, re15_prop_push_tick) ist dieselbe Kette; w0 & 8 fuehrt der Port
nicht (kein Objekt der Auslieferung traegt es: Installer-Flags pc[6..7]|1).

### B3 — Objekt-Pass-Ausloeser (dis 0x8002c0ec 28)
```
8002c0fc: lbu v1,8(s2) / bne v1,4     8002c110: lbu v0,4(s2) / bne (faellt schon)
8002c120: addiu a0,s2,52 / lbu a2,130(s2) / jal 0x8003b7f0 / ori a1,zero,0x1c2
8002c130: andi v0,v0,0x2 / beq -> 0x8002c150       8002c13c: jal 0x8002d3d8 / sb 1,4(s2) / sb zero,5(s2)
```

### B4 — Folgerung (zu MESSEN, nicht anzunehmen)
Eine Bewegung nach Sueden (+Z) verlangt einen Spieler NOERDLICH der Kistenmitte um >= 1350 - v (Beruehrung nach dem
FSM-Schritt), also Spieler-z <= -16832 + v; die Bruestung r2[0] laesst den Spieler (Radius 450) nur bis z >= -15953
(x >= -7706), und westlich davon sperrt r3[2] jeden Spieler mit x < -6428 fuer z < 715. Also gibt es KEINE
Spielerlage fuer einen Schub nach Sueden, solange die Kiste an der Bruestung steht — und sie kann die Bruestung nur
verlassen, wenn sie nach Sueden geschoben wird. Erwartung: Kiste bleibt in x [-5978, -4300), z [-15503, -15482].

## Punkt 3 — Umsetzung

(folgt)

## Punkt 4 — ROOM3050/3051 obj 1, ROOM2050/2051 obj 6

### 4.1 Zensus MIT Spieler (Sonde `tests/unit/probe_r36_dachkiste.c`, `R_belege/zensus_diag.txt`)

Die Freiraum-Zensus der Spur A flutet nur die Kiste. Diese Sonde fuehrt den Spieler mit: Zustand (Kistenzelle,
Spielerseite), Schub nur von der Gegenseite (Kiste = Spieler +- (hx+450), @0x8002cd30-44/@0x8002cd98-ac), Spieler-
Wandklemme Radius 450 Maske 1 (grosszuegig 432), Schubstellung axial 1350..1463 (Reichweite 900 + 563 voraus,
@0x8002d204/@0x8002d224), seitlich bis +-1238, Spieler-Freiraum je Kistenlage exakt komprimiert geflutet. Start =
Tuer-Ankunft des Raums. Ergebnis (Auslieferung, obere Lage):

| Raum / Objekt | Band | Start Spieler | erreichbare Kistenzellen | Ausloeser | Kistenlage (Mitte) |
|---|---|---|---|---|---|
| 1090 / 1091 obj 1 | 5 | (-13600,1300) aus 10F0/10F1 | 9 | **0** | x [-5978,-5959) z [-15503,-15481) — Seiten W und N ohne Schubstellung |
| 3050 / 3051 obj 1 | 4 | (-3596,1523) aus 30E0 | 12729 | **7** (erste (-23875,-18500), Spieler NOERDLICH) | x [-24100,-1800) z [-22350,1123) |
| 2050 / 2051 obj 6 | 3 | (9100,4100) aus 2000/20B0 | 3575 | **1** ((7550,-12000), Spieler SUEDLICH) | x [-6650,9000) z [-11700,-5800) |
| 2050 obj 6 nach sub02 (Zelle 0x33 u0 0, 0x32 u0 0xff) | 3 | (9100,4100) und (-6800,-5650) | 3575 | 1 (dito) | dito |

### 4.2 ROOM3050/3051 obj 1 — der Fall ist der VORGESEHENE Weg, kein Fortschritt haengt an der oberen Lage

* Records (ROOM3050.RDT): obj 1 oben @0x01F1A Band 4 (-18000,-7200,-20800); unter `Ck (3,93)=1` @0x01F40 Band 0
  (-24000,0,-22200) = die GEFALLENE Lage; Latch sub01 @0x02184-2196: `Ck (3,93)=0 / Work_set 3,1 / Member_cmp 15 == 7 /
  Set (3,93)=1` — Kerbe = Aot slot 7 @0x01F06 (sce 5, floor 0, Rechteck x -25000..-22950, z -23200..-21150, unter
  `Ck (3,93)=0` @0x01F02). Absprungzelle Band 4: r*[46] x -25000..-22950 z -18050..-16800 Wort 0x4002 (u0 0x01 = sperrt
  nur den Spieler).
* Was haengt an der Kiste: `Item_aot_set slot 6 @0x01EC6` floor 1 (= Oberkante der gefallenen Kiste, y -1800) Rechteck
  x -24800..-23000, z -23700..-22900, Item 0x07 = **SUPER REDHAWK** (s_item_names[7]), Menge 6 — nur vom Dach der
  gefallenen Kiste erreichbar. Beide Tueren brauchen die Kiste NICHT: slot 0 @0x01E86 floor 0 -> ROOM3040, slot 1
  @0x01EA6 floor 4 (Kistenband selbst, keine Oberkante) -> ROOM30E0.
* Folge: der Fall (Spur A) oeffnet hier den Weg zur Magnum, er sperrt nichts. Der Zensus belegt, dass der Spieler die
  Kiste wirklich bis zur Absprungzelle bringt (von Norden; Fallrichtung FUN_8002d3d8 = vom Spieler weg = +Z).

### 4.3 ROOM2050/2051 obj 6 — Tuer nach ROOM2040 braucht die Kistenoberkante, aber ROOM2040 hat drei weitere Tueren

* Records (ROOM2050.RDT main00): obj 6 @0x01D0E Band 3 (-1600,-5400,-9348) rot 2048, unbedingt; `Door_aot_set slot 2
  @0x01C8A floor 4` (Rechteck x -8200..-7200, z -6200..-5000) -> ROOM2040 (21700,-5400,-5400) — floor 4 = Band 3 + 1 =
  Oberkante der Kiste obj 6 (dieselbe Mechanik wie in 1090). Die Tuer-Flaeche liegt WESTLICH der Band-3-Wand r*[51]
  (x -7550..-6150, z -4500..850), die sub02 @0x02294-22A4 per `Sca_id_set r*[0x33] u0 = 0` erst oeffnet (Raetsel: drei
  Kisten obj 1/2/3 (Band 0) in die Kerben slot 3/4/5 -> (5,0..2) -> sub01 @0x0201A `Aot_reset 6 -> sub02`).
* Absprung: Band-3-Zelle r*[49] x 6350..8750 z -13950..-12150 Wort 0x3202; darunter r*[23] Band 2 x 4550..6350 Wort 0x2102
  und r*[77]/[78] Band 1 Wort 0x1002 — eine KASKADE von Absprungzellen bis Band 0: die Kiste soll dort hinunter. An der
  Kante: Aot slot 7 @0x01E3C/@0x01EC6 (floor 3, x 6800..8300, z -12000..-11200; sce 0 bzw. sce 1 Nachricht nach (3,104))
  und slot 6 @0x01E28 (floor 3, x 4700..5500, z -11550..-10750, sce 3 -> sub04: Nachricht, (3,104), obj 4 Member 1 (y)
  -16200 -> -1400, Kameras). sub02 sperrt nach dem Raetsel r*[0x32] (x 6650..8550, z -12150..-11650) mit u0 0xff.
* Tueren nach ROOM2040 (Zensus aller STAGE2-RDT, Tuer-Nutzlast Byte 23 == 0x04): ROOM2050/2051 slot 2 (floor 4),
  ROOM2060/2061 slot 1 (floor 1), ROOM2070/2071 slot 5 (floor 0), ROOM2040/2041 selbst slot 2/3 (floor 5, Selbsttueren).
  Die Kiste obj 6 ist also NICHT der einzige Weg nach ROOM2040.
* Zensus: obj 6 erreicht mit dem Spieler die Absprungzelle (von Sueden geschoben, Fallrichtung -Z = ueber die Kante
  nach Norden), vor und nach sub02. Ob die Kaskade im Original die Kiste bis Band 0 bringt und ob sie unten gebraucht
  wird (obj 8 @0x01D4C ist die geparkte Band-0-Kiste unter `Ck (5,7)=1`, (5,7) wird in 2050 nirgends gesetzt), ist
  Teil des STAGE2-Raetsels und hier nicht Auftrag; byte-true belassen (Original-Mechanik, Tuer anderweitig erreichbar).

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
