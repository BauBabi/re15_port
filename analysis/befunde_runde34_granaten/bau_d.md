# Runde 34 (Granaten) — Bau Spur D: RE2-FX-Maschine (Saeure 0x0A / Brand 0x0B)

Stand: 2026-09-29, Zweig `r34g/d-re2fx`, Arbeitsbaum `.claude/worktrees/r34g_d`, Basis C0 `8d8651e4`.
Auftrag: BAUPLAN §1.3, §1.4, §3.3 C5/C6, E8, K8, P28, O-VB1, O-VB2, O9 (Orchestrator-Teilung: D = RE2-FX-Maschine).
Dateibesitz: `engine/src/re2_fx.c`, `include/re2_fx.h` (additiv), `platform/pc/src/re2fx_pc.c/.h`, `tools/` (neu),
`tests/unit/probe_r34_re2fx*.c`, `tests/unit/probes/r34_re2fx.cmake`.

Alle RE2-Adressen = `info/re2leon/PSX.EXE` (t_addr 0x80010000), in DIESER Sitzung mit
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` gelesen; Roh-Disasm unter `build/r34g_d/dis/` (unversioniert).
RE1.5-Adressen = `info/Re1.5/PSX.EXE` ueber `re15_disasm.py`. GTE-Worte, die das Werkzeug als `.word` zeigt, sind von Hand
dekodiert und jeweils mit ihrem Rohwort zitiert.

STATUS: Pakete 1-6 gebaut, gemessen und committet; volle Suite siehe §7.

---

## 1. O-VB1 — Bezugsrahmen des Saeure-Spritzers (FUN_8001d894, Bit 0x400)

### 1.1 Disasm FUN_8001d894 (Weltlage je Platz, gerufen von FUN_8001d68c @0x8001d6c8)

```
8001d8a4: lhu v0,24(a3) / 8001d8ac: andi v0,v0,0x800 / 8001d8b0: beq v0,zero,0x8001d900
8001d8b8: lw v0,108(a3)                     ; Bit 0x800: Matrix *(+0x6C) -> +0x4C..+0x6B (8 Worte @0x8001d8c0-fc)
8001d908: lui a1,0x800a / 8001d90c: addiu a1,a1,-9404   ; 0x8009DB44 = EINHEITSMATRIX
8001d914..8001d950: 8 x lw/sw -> 0x1F800000 (Scratch := Einheitsmatrix)
8001d954: lw v1,52(a2) / 8001d964: sw v1,60(a2)          ; +0x3C/+0x3E := +0x34/+0x36 (Vorbild-Lage)
8001d95c: lhu a0,56(a2) / 8001d96c: sh a0,64(a2)         ; +0x40 := +0x38
8001d960: andi v0,v0,0x400 / 8001d968: beq v0,zero,0x8001dac8   ; Bit 0x400 aus -> Normalzweig
--- Bit 0x400 gesetzt:
8001d97c..8001d998: 0x48cc0000/0x48cd0800/0x48cc1000/0x48cd1800/0x48ce2000 = ctc2 t4..t6,$0..$4 (SetRotMatrix(Scratch = EINHEIT))
8001d9a0/a4: 0xc8400000/0xc8410004 = lwc2 VXY0/VZ0 <- +0x24 (lokal)
8001d9b0: 0x4a486012 = MVMVA sf=1 mx=RT v=V0 cv=none (rtv0)
8001d9bc..c4: 0xe9190000/0xe91a0004/0xe91b0008 = swc2 MAC1..3 -> 0x1F800020
8001d9dc..8001da24: 0x1F80002C/2E/30 := +0x2C/+0x2E/+0x30 + MAC1/2/3   (offset + lokal, s16)
8001da28..8001da4c: SetRotMatrix(+0x4C)   (ctc2 wie oben, Quelle v1 = Platz+76)
8001da58/5c: 0xc9000000/0xc9010004 = lwc2 VXY0/VZ0 <- 0x1F80002C
8001da68: 0x4a486012 = rtv0 ; 8001da74..7c: swc2 MAC1..3 -> 0x1F800020
8001da8c..8001dc1c: +0x34 := +0x60 + MAC1, +0x36 := +0x64 + MAC2, +0x38 := +0x68 + MAC3  (sh)
--- Normalzweig (Bit 0x400 aus):
8001dac8: lh a0,34(a2) / 8001dacc: jal 0x8008e8b4 (RotMatrixY(+0x22, Scratch))
8001dad8..8001db34: SetRotMatrix(Scratch), lwc2 +0x24, rtv0, swc2 -> +0x34/36/38 := MAC (sh @0x8001db50/60/70)
8001db78..8001dbc0: SetRotMatrix(+0x4C), lwc2 +0x2C, rtv0, swc2
8001dbd0..8001dc1c: +0x34 += +0x60 + MAC1 ... (lhu/addu/sh)
```

**Befund (1) — "basis" ist die EINHEITSMATRIX**: im 0x400-Zweig wird der Scratch (= Kopie von 0x8009DB44, Bytes
`00 10 00 00 00 00 00 00 00 10 00 00 00 00 00 00 00 10 …`, gelesen `bytes 0x8009db44 32`) NICHT gedreht (RotMatrixY
nur im Normalzweig @0x8001dacc). Damit gilt exakt

* Bit 0x400: `welt = M.t + M.rot·(lokal + offset)` (M = Platz+0x4C, M.t = +0x60/+0x64/+0x68 als s32),
* Normal:   `welt = M.t + RotY(+0x22)·lokal + M.rot·offset`.

RotMatrixY FUN_8008e8b4 liest die Tafel `lw t9,-8532(t9)` = 0x800ADEAC (@0x8008e8dc/@0x8008e900) und rechnet
`m0j' = (c·m0j − t1·m2j)>>12`, `m2j' = (t1·m0j + c·m2j)>>12` mit t1 = −sin (@0x8008e918-0x8008ea14) → RotY·I =
[[c,0,s],[0,1,0],[−s,0,c]]. Die RE2-Tafel 0x800ADEAC (16384 B) ist **bytegleich** mit RE1.5 DAT_800794C4
(Vergleich in dieser Sitzung) = `re15_trig_lut` des Ports → `re15_sin_q12/re15_cos_q12` sind fuer RotMatrixY exakt.

### 1.2 Wo der 0x400-Rahmen wirkt (Reichweite von O-VB1)

* Op 49 Phase 0 setzt `ori v0,zero,0x8403 / sh v0,24(v1)` @0x80021694-98 → der Saeure-Platz bewegt sich im Rahmen
  seiner Matrix M. Seine Kinder spawnen mit a2 = Einheitsmatrix, a3 = Platz+0x34 (@0x80021790-a4 usw.), sie selbst
  laufen im Normalzweig → nur die LAGE der Phase-1..4-Kinder haengt an M.rot.
* Op 48 (Brand) setzt Status 0x8000 (@0x80020ff4-f8, kein 0x400/0x2/0x1) und reicht M als a2 = Platz+0x4C an die
  Kinder weiter (a3 = 0 @0x800210a0/d4) — die Kinder (Normalzweig, offset 0) benutzen davon nur M.t. **Brand haengt
  nicht an M.rot.** Die Bodenflammen ebenso (a2 = Platz+0x4C, a3 = 0).

### 1.3 Port-Zuordnung M fuer den Aufschlag-Platz

M.t = Q (Granaten-Weltlage, Vertrag V1d). M.rot = RotY(gier) · B mit B aus §1.4 (RotY in der Konvention von
RotMatrixY FUN_8008e8b4, s. §1.1). Die RE2-Runde bekommt als a2 die Waffenknochen-Matrix
`lw s0,408(s1) / addiu s0,s0,1964` (= *(+0x198) + 0x7AC = Teil 11, Satz 0xAC, Matrix +0x48; @0x80044f78/@0x80044f90)
im Bild `+0x14D == 1` (`lbu v1,333(s1) / addiu v0,zero,1 / bne` @0x80044f58-60) — die RE1.5-Granate hat keinen
solchen Knochen; der Port nimmt deshalb die Knochendrehung der WAAGRECHTEN GL-Haltung, gedreht um die Wurf-Gier.

### 1.4 B aus den RE2-Daten (Messung, Port-Skelettcode)

Werkzeug: `build/r34g_d/gl/gl_knochen.c` (unversioniert; Nachbau als Sonde `probe_r34_re2fx_knochen`, s. §5). Es liest
`info/re2leon/PL0/PLD/PL01.PLD` (Claire; Verzeichnis @146836: EDD 8, EMR 1456, MD1 24992, TIM 46964) und
`PL01W09.PLW` (Verzeichnis @37816: EDD 8, EMR 1740, …; md5 `3eccb0d3…` = PL01W0A = PL01W0B), nimmt das Skelett aus
der PLD-EMR und die Keyframes aus der PLW-EMR (Kopf @2 = Keyframe-Offset, dieselbe Kombination wie `main.c:1822-1839`
fuer RE1.5) und rechnet `re15_skel_compute_pose` (Port-Zwilling FUN_8001f3bc, mit dem der Port auch die RE2-EMDs
rendert). Skelett: 15 Knochen, Teil 11 = Kind von 10 = Kind von 9 = Kind von 0 (rechter Arm, Rel. (14,360,−46)).

| PLW-Clip | Bild | Keyframe | Teil-11-Drehung (Zeilen) | Laufachse (lokal y) | lokal x |
|---|---|---|---|---|---|
| 10 (Feuern) | 0 | 271 | [12 4079 −1 \| −4076 7 −25 \| −23 −5 4080] | (4079, 7, −5) = **waagrecht** | (12, −4076, −23) = oben |
| 10 | 1 | 272 | [3 4077 8 \| −4073 2 −10 \| −6 −13 4077] | (4077, 2, −13) | (3, −4073, −6) |
| 11 (Halten) | 0 | 270 | wie Keyframe 271 | (4079, 7, −5) | |
| 12 (Feuern) | 0 | 310 | [−1234 3885 −8 \| −3887 −1239 −20 \| −20 −4 4079] | (3885, −1239, −4) = 17,7° **hoch** | |
| 14 (Feuern) | 0 | 349 | [2836 2932 6 \| −2933 2832 −8 \| −10 −6 4081] | (2932, 2832, −6) = 44° **tief** | |

* Laufachse = lokal +y: Muendungsversatz {120, 1200, 0} der Runde (`addiu v0,zero,120 / sh v0,16(sp)`,
  `addiu v0,zero,1200 / sh v0,18(sp)`, `sh zero,20(sp)` @0x80044f7c-8c).
* lokal +x zeigt im waagrechten Clip nach Welt-OBEN (y negativ) → die Runden-Beschleunigung acc.x −10 (Step
  `f6 00 00` @CORE00.ESP 0x1934) und der Saeure-Wert acc.x −23 (@0x80021738) ziehen nach UNTEN; vel.y 640/240 laeuft
  entlang des Laufs. Das ist jetzt gemessen, nicht angenommen.
* Clip 12 (hoch) hat im Waffen-Effekt-Handler die Sonderzeile `lbu v1,332(s1) / addiu v0,zero,12 / bne` @0x80044ffc-5004.
* Bild-Bezug: Der Handler spawnt bei +0x14D == 1 mit der zuletzt gerechneten Matrix; wie in RE1.5 (Wurf-Dossier:
  "Anker = Teil-11-Weltmatrix des VORBILDS") ist das das Vorbild = Clip-Bild 0 = Keyframe 271. Bild 1 (Keyframe 272)
  weicht je Element um hoechstens 17 Q12 ab (< 0,3°).

**B (Q12, zeilenweise) = [12 4079 −1 | −4076 7 −25 | −23 −5 4080]** (`s_gl_basis`, re2_fx.c). Port-Zuordnung: die
Aufschlag-Matrix ist die einer WAAGRECHT abgefeuerten GL-Runde in Wurfrichtung, unabhaengig von der Wurfhoehe
HOCH/MITTE/TIEF (die Granate liegt beim Aufschlag am Boden; eine Zielhoehe hat sie dort nicht).

Folge fuer die Saeure (Phase 0 setzt vel (0,240,·), acc.x −23): lokal nach k Bildern = (−23·k(k−1)/2, 240·k, 0);
Welt = Q + RotY(gier)·B·lokal → die Phase-1..4-Kinder liegen 240/480/720/960 vor Q in Wurfrichtung und fallen um
0/23/69/138 (Welt-y waechst = nach unten).

---

## 2. O-VB2 — Boden/Wand/Wasser der Flammen (FUN_8004fba0, FUN_800527b4)

### 2.1 RE2 FUN_8004fba0(P, r, mask, a3) — Semantik aus dem Disasm

```
8004fbdc: lw s6,0(s3) / 8004fbe0: lw s5,8(s3)            ; P.x / P.z (s32)
8004fbf4: lw s2,32(v0)                                   ; *(RDT+0x20) = Kollisionsblock (Kopf 16 B, dann 16-B-Formen)
8004fc0c: jal 0x8004c198                                 ; Zellen-Maske fp an (x,z)
8004fc14..28: 0x800D5BE4 = 0x800EA198 := Kopf+8
8004fc34: sw zero,-13368(at)  ; DAT_800DCBC8 (KONTAKT) := 0
8004fc3c: sh zero,15228(at)   ; DAT_800C3B7C (RUECKGABE) := 0      <- Grundebene y = 0
8004fc48: blez v1 ... 8004fc58: sw v0(=1),-13368(at)     ; P.y > 0 -> KONTAKT := 1
8004fc5c: bne s0(=a3),zero,0x800500c8                    ; a3 != 0 -> Objekt-Schleife aus
--- Objekt-Schleife 0x800D0324 .. *(0x800D4224), Schritt 504 (@0x8004fc64-0x8004fd68):
8004fc98: jal 0x80036e30   (Objekt-Kasten: +0x84/+0x88/+0x8C aus +0x94..+0x9C, Drehung bei +0x76&0x400)
8004fcac: jal 0x80038950   (Punkt-gegen-Objekt)
8004fcbc..8004fd54: oben = +0x88 − +0x9E; P ueber oben -> RUECKGABE = min(RUECKGABE, oben);
                    P in [oben, unten] -> KONTAKT |= 1, RUECKGABE = min(RUECKGABE, P.y − 1)
--- Formen-Schleife (@0x8004fd74-0x800500cc), je Form 16 B:
8004fd74..8004fdd0: (+10 & fp), Kasten +/- r, (+8 & mask), Typ != 10
8004fdec..8004fe04: unten s1 = −1800 * (Index des untersten gesetzten Bits von +12)
8004fe08..8004fe30: oben  s0 = −1800 * ((+10 >> 6) & 0x1f)
8004fe44..8004fe54: Sprungtabelle 0x80011104 (Typ 1..8 Formtest, 11/12/13 Rampen/Treppen)
8004ffb0..8004ffd0: Treffer -> oben −= 100 * (+10 >> 11)
8004ffdc..80050024: P unter unten -> Buchhaltung 0x800D5BE4; P zwischen oben/unten -> KONTAKT |= 1;
                    P auf/ueber oben -> RUECKGABE = min(RUECKGABE, oben) (@0x80050028-44)
8005005c..800500c4: KONTAKT |= 2 wenn P.y < unten (Decke/Buchhaltung 0x800EA198)
800500d4: lh v0,15228(v0) ; Rueckgabe = DAT_800C3B7C (s16, sign-extended)
```

**Rueckgabe** = hoechste Oberflaeche (kleinstes y) unter/auf P aus {Grundebene 0, Oberkanten der Formen und Objekte,
ueber denen P liegt}. **KONTAKT (DAT_800DCBC8) != 0** = P liegt unter der Grundebene (P.y > 0) oder IN einer Form/einem
Objekt (zwischen dessen Ober- und Unterkante). Die Objekt-Schleife laeuft ueber den Objekt-Pool 0x800D0324 (Schritt
0x1F8, FUN_80036e30 rechnet den Kasten aus +0x94..+0x9C wie ein Obj_model_set-Prop).

Anteil der Flammen-Klasse 0x2000 an den RE2-Formen (eigener Scan aller 495 Leon-RDTs, Kollisionsblock = RDT+0x20):
**4259 von 5009 Formen (85 %)** tragen Bit 0x2000 — die Flammen stossen praktisch an jede Wand.

### 2.2 RE2 FUN_800527b4(x, z) — Wasserspiegel

Der Port hat den byte-true Zwilling schon: `re15_aot_water_at(x, z)` (`engine/src/aot_common.c:446`, Kopf
`include/re15_aot.h:331-363`, Sitzung 2026-08-20: sce-7-Scan @0x800527d4-0x800528c4, sce-Bruecke RE2 7 ↔ RE1.5 8).
Rueckgabe 0 = kein Wasser. Keine Neu-Umsetzung noetig.

### 2.3 Die Aufrufer in den Flammen-Ops (Disasm)

| Op | Aufruf | Verwendung |
|---|---|---|
| 27 @0x8001fa9c | `jal 0x8004fba0` @0x8001fb4c mit a0 = &{+0x34,+0x36,+0x38}, a1 = 2, a2 = 8192, a3 = 0 (@0x8001fb28-50) | `sw v0,20(v1)` @0x8001fb60: +0x14..+0x17 := Rueckgabe (32 Bit, vorzeichenerweitert → +0x16 = 0xFFFF bei negativer Hoehe) |
| 28 @0x8001fbd0 | Wasser `jal 0x800527b4(x,z)` @0x8001fbec; Boden `jal 0x8004fba0` @0x8001fc7c mit P.y = y − 900 (`addiu v0,v0,-900` @0x8001fc6c), a1 2, a2 8192, a3 0 | Wasser: w != 0 && w < y → Op A/Status := 0 (@0x8001fc14-24); f < y → Op[step[2]] (@0x8001fc94-cc); sonst KONTAKT → f == +0x14 ? Op[step[3]] : Op[step[2]] (@0x8001fcd8-34); dann +0x14 := f (@0x8001fd44) |
| 29 @0x8001fd5c | `jal 0x8004fba0` @0x8001fe70 mit P.y = y − 100 (`addiu v0,v0,-100` @0x8001fe60) | KONTAKT → Op[step[3]] (@0x8001fe84-b4) |
| 25 @0x8001fa08 | Wasser `jal 0x800527b4` @0x8001fa20 | w != 0 && w < y → Op A/Status := 0 (@0x8001fa2c-58) |

### 2.4 Abbildung auf die RE1.5-Raeume des Ports (Port-Zuordnung, je Glied belegt)

Der Port rendert RE1.5-Raeume; es gibt dort keine RE2-Formen. Das RE1.5-Gegenstueck derselben Frage ("Boden unter
einem fallenden Effekt-Partikel") ist die **RE1.5-ESP-Routine 12** (Blutstropfen-Aufprall, @0x8001779c):

```
800177b4..800177cc: P = (+0x28,+0x2a,+0x2c)  ; a1 = 0 (`addu a1,zero,zero` @0x800177b8)
800177c4: ori a2,zero,0x8                     ; Startband 8
800177d0: ori a3,zero,0x100                   ; Maske 0x100
800177d4: jal 0x8001c6e8                      ; room_coll = RE1.5-Bodensonde (SCA-Baender + Objekt-Oberkanten)
800177e4..800177f0: slt v1,a0(=Boden),v1(=P.y) -> Aufprall;  Vergleich mit +0x1E (letzter Boden) @0x80017804-0c
```

Beide Sonden haben denselben Aufbau (Grundwert 0, Oberkanten der Kollisionsdaten, Objekt-Oberkanten der Props), und
Routine 12 prueft wie Op 28 "Boden gegen gespeicherten Boden". Die Port-Abbildung `re2fx_boden()` (re2_fx.c) ist damit:

| RE2-Glied (FUN_8004fba0) | Port (RE1.5-Daten) | Beleg |
|---|---|---|
| Rueckgabe-Grundwert 0 | 0 | @0x8004fc3c |
| Oberkanten Formen + Objekte | `re15_collision_room_coll(&g_room_rdt, x, z, 0, 8, 0x100)` = FUN_8001c6e8 mit den Argumenten der RE1.5-Routine 12 | RE1.5 @0x800177b8/c4/d0/d4 |
| KONTAKT: P.y > 0 | P.y > 0 | @0x8004fc48-58 |
| KONTAKT: P in einer Form | P in einer SOLIDEN SCA-Zelle des Bandes von P.y, Rand r, Klassen-Bit 2 (`re15_collision_box_blocked` = FUN_8003b558-Zwilling; Bit 2 = Objekt-Klasse, einziger reiner Punkt-Test des RE1.5-Objekt-Schiebers `jal 0x8003b558` @0x8002bfb0 mit `ori a1,zero,0x2` @0x8002bfb4) | RE1.5-Zellen: 20344 von 20874 (97,5 %) tragen u0 = 0xFF, d.h. jedes Klassen-Bit waehlt dieselben Waende (eigener Scan aller 206 RDTs) |
| KONTAKT: P in einem Objekt | P im Prop-Kasten (`re15_collision_prop_box_hit`, FUN_8002da4c) UND Oberkante < P.y <= Prop-y | RE2 @0x8004fcbc-0x8004fd54 (zwischen Ober- und Unterkante) |
| Wasser FUN_800527b4 | `re15_aot_water_at(x, z)` | Port-Zwilling (aot_common.c:446) |

Der Formtyp-genaue Test (Kreise, Schraegen: RE2-Typen 1..13) wird wie ueberall im Port durch den Zellen-Kasten ersetzt
(`re15_collision_box_blocked` prueft nur das Rechteck — bestehende Port-Vereinfachung, nicht Teil dieser Spur).
Fuer Sonden gibt es einen Haken `re2fx_boden_hook` (flache Ebene ohne Raum).

### 2.5 Messung der Abbildung an echten RE1.5-Raeumen

Werkzeug `build/r34g_d/boden/boden.c` (unversioniert; 250er-Gitter ueber die SCA-Huelle, ruft
`re15_collision_room_coll(&rdt, x, z, 0, 8, 0x100)` und `re15_collision_box_blocked(&rdt, x, z, 0, 2, 2)`):

| Raum | Gitterpunkte | Boden 0 | Boden −1800 | −3600 | −5400 | Zellkontakt Band 0 |
|---|---|---|---|---|---|---|
| ROOM1140 | 12100 | 3859 | 8241 | 0 | 0 | 8463 |
| ROOM1150 | 4292 | 2076 | 2216 | 0 | 0 | 2144 |
| ROOM1170 | 33473 | 19646 | 679 | 0 | 390 | 902 |
| ROOM1000 | 13952 | 9683 | 4269 | 0 | 0 | 4424 |
| ROOM2090 | 8343 | 4913 | 2172 | 1258 | 0 | 3619 |

Die RE1.5-Bodensonde liefert −1800 ueber den Wand-/Hindernis-Zellen des Bandes 0 (Zahlen deckungsgleich mit dem
Zellkontakt) und 0 auf freiem Boden. Folge fuer die Flammen: RE2 wertet einen Punkt IN einer Form als Kontakt, der Port
wertet die Oberkante der Zelle als Boden; das unterscheidet sich nur fuer eine Flamme, die UEBER dem Boden (y < 0) in eine
Wandzelle fliegt. Das ist im Port nicht erreichbar: die Granate liegt immer mit Welt-y > 0 (Routine 29 `lh t1,42(t0) /
blez t1` @0x80018330-38, BAUPLAN §1.1 "steckt 3..20 im Boden"), die Flammen starten an Q (Op 48 Translation := Q
@0x80021000-18), fallen nur (acc.y 5..12) und landen deshalb im ersten Op-28-Bild (`slt v1,s0,v1` @0x8001fc94: 0 < y) —
nach der RE2- wie nach der Port-Regel. Der Gleit-Kontakt (Op 29, P.y = y − 100) ist in beiden Regeln "in der Wandzelle".

---

## 3. Die Maschine (engine/src/re2_fx.c, include/re2_fx.h)

| Datei:Zeile | Inhalt | Beleg |
|---|---|---|
| re2_fx.c:84 | `s_gl_basis` = B (O-VB1) | §1.4 |
| re2_fx.c:124 | `re2fx_reset` = Pool leer (Boot @0x8001bac4-e4) | |
| re2_fx.c:135 | `re2fx_register_core` = FUN_8001babc-Kopf + FUN_8001bca0 (rueckwaerts, 8 Ids, Tabelle `(2ca+cb+2)*4`) | @0x8001bb54-90, @0x8001bcc8-2c |
| re2_fx.c:176 | `spawn_kern` = FUN_8001cbe8 / FUN_8001bf10 inkl. Mehrteil-Zweig | @0x8001cbe8-0x8001cef4, @0x8001bfa8 |
| re2_fx.c:279 | `weltlage` = FUN_8001d894 (0x800-Folgen, Vorbild-Lage, 0x400- und Normalzweig) | §1.1 |
| re2_fx.c:346-785 | Ops 0/1/2/19/25/27/28/29/30/40/46/48/49/50/58/64, Dispatcher `op_rufen` | Tabelle 0x8009D868 |
| re2_fx.c:378 | `re2fx_boden` = O-VB2-Abbildung | §2.4 |
| re2_fx.c:778 | `schritt` = FUN_8001d68c (Op A?, Weltlage, Op B, Physik, Anim) | @0x8001d68c-0x8001d88c |
| re2_fx.c:822 | `re2fx_tick` = FUN_8001d300 (Update-/Draw-Pass, Waisen-Kill, Befoerderung 0xA003, Pause-Liste) | @0x8001d300-0x8001d688 |
| re2_fx.c:856 | `re2fx_quads` = FUN_80077924 + FUN_80077ed0 (§4) | |
| re2_fx.c:923 | `re2fx_aufschlag` = Port-Zuordnung E8 (§3.2) | |

### 3.1 Op-Belege (je Konstante mit Adresse; alle in dieser Sitzung disassembliert)

| Op (Optab) | Adresse | Umgesetzt |
|---|---|---|
| 0 | 0x8001dc28 | `jr ra` |
| 1 | 0x8001dc30 | Status := step[0x12] @0x8001dc3c-44, Anim := step[2] @0x8001dc40-4c, TPage \|= step[0x14] @0x8001dc48-60, Countdown := Dauer @0x8001dc64-7c, Index += step[0xB] @0x8001dc8c-9c, 24 B nachladen @0x8001dcac-dd28 (lwl/lwr Rohworte 0x88430003…0xb8850014) |
| 2 | 0x8001dd2c | wie Op 1, Anim := step[2] + rand % (step[0x16]+1) (`div` @0x8001dd70, `mfhi` @0x8001dd98) |
| 19 | 0x8001f2c0 | Schaden-Tor +0x4A @0x8001f2d0, ≥ 16 @0x8001f2e8, > 0x1000 @0x8001f2fc, Op 40 @0x8001f308, +0x16++ @0x8001f31c-28; Zustand 0 tot @0x8001f37c-84; 1: ×990/×980 @0x8001f3a4-e4 (0x10624dd3, sra 6, Vorzeichen); 2: ×1009/×1002 @0x8001f40c-44, Ende → Zustand 1, 90 + r%11 @0x8001f47c-4c8; 3: Zaehler, dann Zustand 2 mit 30 + r mod 8 @0x8001f4cc-51c |
| 25 | 0x8001fa08 | Wasser `jal 0x800527b4` @0x8001fa20, w ≠ 0 && w < y → Op A/Status := 0 @0x8001fa2c-58 |
| 27 | 0x8001fa9c | 0xB003 @0x8001fabc-c0, TPage \|= 0x20 @0x8001fac4-cc, Anim r%3 @0x8001faf4, Countdown @0x8001fb04-1c, Boden `jal 0x8004fba0` @0x8001fb4c → +0x14 als **u32** @0x8001fb60, Op A 58 @0x8001fb64-68, Op B 28 @0x8001fb74-78, +0x1B := 2 @0x8001fb8c, 8 + r%3 @0x8001fbb4-b8 |
| 28 | 0x8001fbd0 | Wasser @0x8001fbec-24, vel.x < 0 → 0 @0x8001fc3c-48, P.y − 900 @0x8001fc6c, f < y → Op[step[2]] @0x8001fc94-cc (ohne +0x14), Kontakt → f == +0x14 ? Op[step[3]] : Op[step[2]] @0x8001fcd8-34, +0x14 := f @0x8001fd44 |
| 29 | 0x8001fd5c | vel.x ≤ 0 → 0 @0x8001fd74-84; vel.x ≥ 61 @0x8001fd78 und step[2] % 15 == 0 @0x8001fd94-b8 → 0x0504 \| Skala×0.8 @0x8001fdc0-f8 mit a3 = Platz+0x34, Kind +0x4A := 1 @0x8001fe20; step[2]++ @0x8001fe30-3c; P.y − 100 @0x8001fe60, Kontakt → Op[step[3]] @0x8001fe84-b4 |
| 30 | 0x8001fecc | Op 2 @0x8001fed4; Sub == 4 → +0x1B 2, 2 + r mod 8 @0x8001fee8-ff30; sonst 3, 700 + (r%6)·50 @0x8001ff34-84 |
| 40 | 0x80020758 | Box 0x80010910 = [−600,0,300,150] (lwl/lwr @0x80020770-8c), P.y − 100 @0x800207a4, 0x2002000A @0x80020794/a0, `re2fx_applier` (NULL = kein Treffer), Treffer → Op 50 @0x800207cc |
| 46 | 0x80020b60 | acc.y/step[2]/vel.y := 0 @0x80020b70-80, Op A 19 @0x80020b84, Op B 29 @0x80020b94, vel.x 180 @0x80020ba4, acc.x −10 − r%11 @0x80020bb0-f4, 38 + r mod 8 @0x80020c20, +0x12 &= 0xFFFE @0x80020c28 |
| 48 | 0x80020f3c | §3.2 |
| 49 | 0x800215c8 | §3.2 |
| 50 | 0x80021970 | step[2] 64 @0x80021978, acc.x/step[3]/vel.x := 0 @0x8002198c-9c, +0x12 &= 0xFFFD @0x800219a8 |
| 58 | 0x80022254 | Zustand 0 tot @0x800222a0-a8; 1: ×880 (mit) / ×800 (OHNE Vorzeichenkorrektur) @0x800222c8-328; 2: ×1010/×1007 @0x80022350-84, Ende → 1, 2 + r mod 2 @0x800223b8-e4 |
| 64 | 0x80022728 | **erreichbar** (Korrektur zu Saeure-GP OFFEN 3): Op 28 Kontakt + gleicher Boden → Op 50 (step[2] := 64), naechste Bodenberuehrung → Op[64]: y := +0x14, acc.y/step[2]/vel.y := 0, Op B := 0, +0x12 &= 0xFFFE @0x80022734-70 |

Erreichbare Ops (Skript-Dump CORE00.ESP, Baenke 2/3/4/5): genau die obigen; jeder andere Op zaehlt `re2fx_op_unbekannt()`
(Sonde 222/429: 0).

### 3.2 Aufschlaege (Op 48 / Op 49) und die Uebergabe (E8)

`re2fx_aufschlag(re2_art, q, gier)` legt einen **RE2-Runden-Platz im Aufschlagbild** an (Port-Zuordnung, ohne RE2-Flug):
FUN_8001bf10-Zwilling mit 0x020C1000 (Bank 2 Skr. 4 = Brand/Saeure-Runde `lui a0,0x20c / ori a0,a0,0x1000`
@0x80044f9c-a0), a1 = gier, a2 = M (M.rot = RotY(gier)·B, M.t = Q), a3 = NULL; dann der Op-17-Stand: +0x1B := Art
(`sb v0,27(v1)` @0x8001f1b8, explizit 2 = Saeure / 1 = Brand, nie Id − 9), Op B := 47 + Art (Op 15 → Op[step[2] = 47 + Art]
@0x8001f0e4-104), Status 0xB403 @0x8001f1e4, Anim 18 @0x8001f1ec, TPage \|= 0x20, Lebensdauer 15 (@0x8001f1d4/@0x8001f284,
≠ 255 = kein Wasser). Op A := 0 (die Rauchspur Op 22 gehoert zum RE2-Flug, den es im Port nicht gibt). Phase 0 laeuft im
Draw-Pass desselben Bildes (re2fx_tick direkt nach dem ESP-Tick).

* **Op 49 Saeure** (Sprungtabelle 0x80010950): Phase 0 Status 0x8403, Op A 0, Op B 49, SE 0x01130001 an (x,y,z),
  vel.y 240 / vel.x 0 / acc.x −23 / acc.y 0, Kinder 0x030F2000, 0x040C2000, 0x041D1800 an a3 = Platz+0x34; Phase 1..4:
  0x031F2000 / 0x03142000 / 0x040D2800 / 0x030F2000, Platz frei in Phase 4. Die zwei RE2-Treffer 0x1002000B laufen NICHT (E3).
* **Op 48 Brand**: Phase 0 Translation := Weltlage, Status 0x8000, SE 0x01120001 an +0x60, Kinder 0x040C2800/0x041D2700
  (a2 = Platz+0x4C), IMMER drei Bodenflammen (Skala 7168 + (r mod 8)·768, Gier + r%40 / + r%80 + 400 / + r%80 − 400,
  vel.x += r%25, acc.y += r mod 8, +0x4A := 1, nur bei gueltigem Platz — die Zufallszuege r3/r4 fallen bei Pool-voll weg
  wie im Original @0x80021174-7c), Ende: +0x60/+0x64/+0x68 := alte Translation mit y + 3600 (@0x80021578-a0); Phase 1 frei.
  Die zwei RE2-Treffer 0x0002000A laufen NICHT (E3).
* Zeitpunkt der Kinder: sie liegen auf den naechsten freien Plaetzen UNTER dem Aufschlag-Platz (Suche 95 abwaerts) und
  werden deshalb im Draw-Pass des FOLGEbilds befoerdert (Pumpe laeuft 0 → 95). Das ist der RE2-Mechanismus; welches Bild
  im Original, haengt dort ebenso an der Poolbelegung.

---

## 4. Zeichner (re2_fx.c `re2fx_quads`, platform/pc/src/re2fx_pc.c)

* `re2fx_quads` (engine, plattformneutral): Schleife 95 → 0, Sichtbar (st & 0xA000) == 0xA000, Region-Test
  `re15_esp_fx_culled` (= FUN_8002c820 auf das Viereck des aktiven Cuts; RE2 @0x80077a30 liest 0x800CE338, RE1.5-Raum:
  DAT_800AC790), nprim/size/cell, Code 0x2C/0x2E, RTPS wie pc_draw_effects, Near-Gate SZ3 >> 9 (@0x80077f58 — RE2 verwirft
  Sprites unter SZ 512, RE1.5-ESP erst unter 4), Klemme 32767, step/Breite/Hoehe/Texelschritt, Kanten-Trim, Ecken aus s8
  cx/cy, UV = (u, v)..(u + size', v + size'). Status-Bit 0x200 (Alternativpfad @0x80077a54-60, 3D-Quads) wird ausgelassen —
  kein Aufschlag-Skript setzt es (Status 0xB003/0xB803/0x8403/0x8000). Spiegelboden (0x800CFBD8 & 0x10000) entfaellt: RE1.5-
  Raeume haben keinen.
* `re2fx_pc_lade_tex`: TEX.TIM nach dem Lader FUN_80076a40 — **die Bildlage der Datei (0,0) wird ersetzt** durch (768,256)
  (0x800CFBF0 = 28 per Halbwort-Store `sh v0,-1040(at)` @0x8002b8d4; x = 28·64 − 1024 @0x80076a64-80, y = 256
  @0x80076a9c-a8), CLUT-y = 480 + 0 (@0x80076b00-0c). Die Seiten 0x1E/0x1F werden nebeneinander (512 × 256 Texel) mit den 19
  CLUT-Zeilen der Spalte x 272 in EINEN Textur-Slot gelegt (Mehr-CLUT-Upload von render_pc.c: Zeile = CLUT-y − 480).
* `re2fx_pc_draw`: je Quad zwei Dreiecke, u + 256 fuer Seite 0x1F, CLUT-Wort und TPage durchgereicht, Mischmodus aus
  TPage-Bits 5-6 wie pc_draw_effects (main.c:436-448), Farbe 255 = PSX 0x80 (Paketfarbe 0x808080 @0x800783cc-d0; die
  unbeleuchtete Queue-Funktion moduliert mit Farbe/255, render_pc.c:2505; die beleuchtete rechnet 0x80 → 0xFF,
  render_pc.c:2536-2539). Puffer 384 Quads je Bild = RE2-Paketpuffer 0x3C00/40; das Loeschen eines nicht mehr passenden
  Platzes (@0x80077e40-54) ist nicht nachgebaut (Aufschlaege < 130 Quads).
* STP-Pruefung (Lauf `re2fx_katalog.py` + Zaehlung): in den CLUT-Zeilen 480..483 tragen alle 15 benutzten Farben das
  STP-Bit; in Zeile 484 (Flammen) ist Eintrag 14 = 0x0001 ohne STP, aber 0 der 5849 deckenden Flammen-Texel (Zellen 0..9)
  benutzen ihn → die Ganz-Quad-Mischung des PC-Renderers ist fuer alle gezeichneten Texel exakt.
* UV-Ueberlauf-Pruefung: in den Baenken 2/3/4/5 hat keine Zelle u + size oder v + size ≥ 256.

---

## 5. Sonden und Mutationsproben

`tests/unit/probes/r34_re2fx.cmake`: `unit_r34_re2fx`, `unit_r34_re2fx_knochen`, `unit_r34_re2fx_bild` (alle gruen).

| Pruefung | Inhalt | Negativ-Kontrolle |
|---|---|---|
| 101-116 | Registrierung (8572 B, Ids 03 05 00 01 02 06 07 04), Spawn auf Platz 95, Step Bank 5 Skr. 5 `00 1b 2e 32 …`, Status 0x4000, Anim/UV/Step-Offsets (0x05F8 / 0x0698 / 0x0800), TPage/CLUT/+0x42/Countdown, CLUT + (sub>>3)·0x40, Pool voll → 0xFF | Rumpf-Datei → Fehler, Spawn ohne Registrierung → −1, Bank 9 → −1 |
| 201-222 | Saeure: Platz Bank 2/0x0C Status 0xB403 Op B 49; SE 0x01130001 genau 1× an Q; Phase 0 → 0x8403/Phase 1/Op A 0, vel.y 240, vel.x −23 nach der Physik; Kinder X = 0x030F2000/0x040C2000/0x041D1800 (0x4000, Versatz Q); X+1..X+4 je ein Phasen-Kind, Lage = Q + RotY(1024)·B·lokal (±2); X+4 Platz frei | Art 0 und 3 → nichts |
| 301-336 | Brand: SE 0x01120001 an +0x60, Status 0x8000, Ende +0x64 = Q.y + 3600, Kinder 0x040C2800/0x041D2700 mit Matrix-T = Q, drei Flammen gegen einen UNABHAENGIG nachgebauten RE2-Strom (Skala, Gier, vel.x, acc.y, +0x4A), Strom danach synchron, X+1 Platz frei, Flammen nach Op 27 (0xB003, Op A 58, Op B 28, Zustand 2) | — |
| 401-430 | Landung → Op A 19 / Op B 29, Zaehler 38..45 / 90..100, Zustandsdauern exakt Zaehler + 1 / + 2; Lebensdauer 140 (Band 131..148); Applier nur bei step[0x16] ≥ 16 und X ≥ 0x1001, Box [−600,0,300,150], Hitcode 0x2002000A; Treffer → Op 50; Wand beim Gleiten → Op 50; Wand in der Luft → Op 64 | Flamme ohne +0x4A → kein Applier; ohne Wand kein Op 64 |
| 440-451 | Landung im Luft-Schrumpfen: +0x1B bleibt 1, Tod nach Zaehler + 2; Pause 0x10000000 haelt an, danach Phase 0 | — |
| 601-602 | Folgeflammen-Takt: vel.x 200 / acc.x 0 → genau 4 in 60 Bildern | vel.x 60 → 0 |
| 701-705 | Aspekt-Folgen je Bild exakt (Op 58 Z2/Z1, Op 19 Z2/Z1), alle vier Zweige durchlaufen | — |
| knochen 1-20 | B aus PL01.PLD + PL01W09.PLW nachgerechnet (Keyframe 271), Clip 10/12/14 waagrecht/hoch/tief, lokal +x oben | Clip 12 ≠ B |
| bild 1-17 | Offscreen: keine Quads vor dem Aufschlag, Seiten nur 0x1E/0x1F, CLUT-Spalte 272, kein UV-Ueberlauf, jedes Sprite deckend, Saeure/Brand zeichnen | Bild 0 (vor dem Aufschlag) leer |

Mutationsproben (Konstante in re2_fx.c kurz verstellt → Sonde rot → zurueck → gruen; alle in dieser Sitzung gefahren):

| Mutation | Ergebnis |
|---|---|
| Op 19 `>= 0x10` → `>= 0x0F` | FAIL 412 |
| Op 49 acc.x −23 → −22 | FAIL 206 |
| B[1] 4079 → 4078 | knochen FAIL 10 |
| Flammen-Skala 7168 → 7169 | FAIL 313 |
| Op 29 `% 15` → `% 14` | FAIL 601 |
| Op 64 aus der Tabelle entfernt | FAIL 429 |
| Op 19 ×1009 → ×1008 | FAIL 703 |
| Op 58 ×880 → ×881 | FAIL 702 |
| Op 29 vel.x 61 → 60 | FAIL 602 |
| Op 46 setzt +0x1B := 2 | FAIL 441 |
| Pause-Gate aus | FAIL 450 |

---

## 6. Sichtpruefung ohne main.c (Paket 6, O9)

* `probe_r34_re2fx_bild` rastert die Saeure- (16 Bilder) und Brand-Folge (48 Bilder) der echten Maschine vor einer festen
  Kamera (Standort (−1500,−1800,−5200), Blick (1000,−400,0), fov 26684, camf 208) ueber ein **VRAM-Modell** (Lader-Lage wie
  FUN_80076a40, PSX-4-bpp-Abruf aus TPage/CLUT-Wort, PSX-ABR je Texel mit STP-Bit) → PPM je Bild + `re2fx_quads.txt` +
  `re2fx_crops.txt`. Lauf: 1060 Quads, 626 eindeutige Ausschnitte, 16 leere Ecken-Zellen (Daten), Pixel nach dem Aufschlag
  Saeure 135182 / Brand 374878.
* `tools/re2fx_katalog.py` (neu, unabhaengiger Dekoder direkt aus CORE00.ESP + TEX.TIM): Katalog der zehn Aufschlag-Kinder
  (`build/r34g_d/katalog/re2fx_katalog.png`) und Abgleich: **626 Ausschnitte, 0 abweichende Texel** zwischen Port-Pfad
  (re2fx_quads + VRAM-Modell) und Katalog-Dekoder. Katalog-Befund (O9 geschlossen): Bank 3 Skr. 7 CLUT 481 grauer Puff
  (additiv, Anim 11..22), Bank 4 Skr. 4 CLUT 481 dunkler Rauch (subtraktiv, Anim 3..22), Bank 4 Skr. 5 CLUT 483 oranger Puff
  (B + F/4, Anim 32..35), 0x031F2000 CLUT 483 weiss-gelb-roter Feuerball (additiv), 0x03142000 CLUT 482 gelb-brauner Puff,
  Bodenflammen CLUT 484 Flammen (additiv, Anim 0..9 Schleife). Bildfolge `build/r34g_d/katalog/re2fx_bildfolge.png`.
* Die exe-Sichtabnahme (gdigrab/RE15_FRAMEDUMP) braucht die Bindung in main.c (INTEGRATIONSWUNSCH 1-4) und ist damit
  Sache der Integration.

---

## INTEGRATIONSWUNSCH (fremde Dateien — NICHT geaendert)

1. `platform/pc/src/render_pc.c:204` — `#define RE15_TIM_SLOT_MAX 50` → **51**, Slot 50 = RE2-FX-Seiten (`RE2FX_TIM_SLOT`,
   re2fx_pc.h). Ohne das laedt `re2fx_pc_lade_tex` nichts (Rueckgabe −7, stderr-Meldung) und `re2fx_pc_draw` zeichnet nichts.
2. `platform/pc/main.c` Boot (nach Renderer-Start, neben dem Laden der RE1.5-CORE00.ESP): `shared_assets/RE2/CORE00.ESP`
   resident laden → `re2fx_register_core(buf, n)` (Puffer bleibt gehalten); `shared_assets/RE2/TEX.TIM` →
   `re2fx_pc_lade_tex(buf, n)` (Puffer danach frei).
3. `platform/pc/main.c:10516` direkt nach `pc_draw_effects(...)`: `re2fx_pc_set_ansicht(&cam_view, cx, cy, pc_fx_camf(),
   has_region, rxs, rzs); re2fx_pc_draw();`
4. `platform/pc/main.c` 30-Hz-Block (C1): `re2fx_tick()` direkt hinter `re15_esp_fx_tick(...)` (RE2: Gegner-Schleife
   0x800267c0-0x80026930 vor `jal 0x8001d300` @0x80026980).
5. Raumwechsel: `re2fx_reset()` an denselben Stellen wie `re15_esp_fx_reset()` (`room_pc.c:130`, `scd_room_setup.c:199`) —
   Port-Zuordnung nach RE1.5 FUN_80019354 (`sb zero` @0x80019378, gerufen @0x8003996c). RE2 selbst leert den Pool nur ueber
   FUN_8001d07c (einziger Rufer @0x800569a8, ein SCD-Op) bzw. beim Boot FUN_8001babc; der Raumlader FUN_8001bba4 (@0x8004a2ec)
   registriert nur die Raum-Baenke 8..15 neu.
6. Haken: `re15_esp_aufschlag_hook = re2fx_aufschlag` (V1d); `re2fx_se_hook` → 0x01130001 = ARMS10 Satz 10, 0x01120001 =
   ARMS11 Satz 10 (E9, audio_pc.c); `re2fx_applier = re15_re2_gl_apply` (nach Merge B).
7. **O-VB4 (Orchestrator-Entscheid) liegt im Applier (Spur B, `re15_re2_gl_apply`, re15_damage.c)**: die Maschine ruft Op 40
   fuer JEDE Flamme ab step[0x16] ≥ 16 / X > 0x1000 ohne Typfilter; Gegner ohne RE2-KI muessen dort ueber den RE1.5-
   Gegnerzweig mit Art 5 "Flaechenfeuer" getroffen werden (DAT_8006f418[5] = 50 @0x8006f422, DAT_8006f430[5] = 14
   @0x8006f435), E7-Rueckfall bei NULL-Zeile 14.
8. `release/make_package.sh:186-189` Paket-Gate um `CORE00.ESP TEX.TIM` ergaenzen; `platform/android/app/build.gradle:113-115`
   Existenz-Gate um `shared_assets/RE2/CORE00.ESP` und `shared_assets/RE2/TEX.TIM` (C0-Hinweis; jetzt liest Spur D beide).
9. Hinweis an Spur C (nur gelesen, nicht gemessen): `pc_draw_effects` uebergibt `128,128,128` an die unbeleuchtete
   Queue-Funktion (main.c:460/462), die die Farbe unveraendert als SDL-Faktor/255 nutzt (render_pc.c:2505) — die RE1.5-ESP-
   Effekte liefen damit bei halber Helligkeit, falls nicht anderswo ausgeglichen. Fuer RE2-FX nimmt re2fx_pc 255 (§4).

10. Optional (nur Tempo, Ergebnis unveraendert laut Kopf von release/build_linux_deck.sh:80-86): `KOPIE` um
   `info/re2leon/PL0/PLD/PL01.PLD` und `info/re2leon/PL0/PLD/PL01W09.PLW` ergaenzen (liest `unit_r34_re2fx_knochen`).

## OFFEN

* O-VB2 Formtyp-Genauigkeit: der Zellen-Kontakt nutzt das Zellrechteck (`re15_collision_box_blocked`), nicht die RE2-
  Formtests der Typen 1..13 (Schraegen/Treppen/Kreise). Bestehende Port-Vereinfachung; Weg: die RE1.5-Formtests
  (re15_collision.c "SCA DIAGONAL / SLOPE cells") in re2fx_boden nutzen, sobald sie fuer Punkt-Tests exportiert sind.
* RE2-Paketpuffer-Ueberlauf (Platz-Loeschen @0x80077e40-54) nicht nachgebaut (unerreichbar, §4).
* Mischreihenfolge: die PSX-OT haengt jedes Paket vorn an seinen Bucket (@0x80077f94-fac, Bucket = SZ >> 5) — innerhalb
  eines Buckets gilt "zuletzt eingereiht, zuerst gezeichnet". re2fx_pc reiht deshalb rueckwaerts ein (stabiler Port-Sortierer
  render_pc.c:963-971), womit GLEICH tiefe Quads (die Kinder an Q) exakt wie im Original liegen. Quads mit verschiedenem
  View-Z im selben 32er-Bucket ordnet der Port nach View-Z, das Original nach Einreihung — offen, wirkt nur, wo additive
  (ABR 1/3) und subtraktive (ABR 2) Sprites mit Saettigung ueberlappen (reine Additionen/Subtraktionen sind kommutativ).

---

## 7. Bau und Suite

* Bauverzeichnis `re15_port/build_r34_d` (eigenes; nur ueber `local_build.sh` bzw. PATH mit msys64 zuerst). Neue Tests:
  `unit_r34_re2fx`, `unit_r34_re2fx_knochen`, `unit_r34_re2fx_bild` → N = 428 (C0) + 3 = **431** (`ctest -N` nach dem
  Configure). `RE15_MIN_TESTS` NICHT angefasst (Integration hebt es, BAUPLAN §4).
* **Lauf 1** (`local_build.sh` all, 2026-09-30 00:32-01:02, parallel zu den Suiten anderer Spuren; CPU-Last gemessen 94 %,
  3 ctest + 3 re15_pc gleichzeitig): `99% tests passed, 6 tests failed out of 431`, `Total Test time (real) = 1698.51 sec`.
  Rot NUR exe-/GUI-Tests: `integration_r30_cut_blitz` (exit=1 nach 0,29 s, debug.log endet nach `[window] windowed 960x720`),
  `integration_r30_granate_laden` (Lauf abgerissen, exit=1), `integration_r30_irons_tisch_laden` (keine CONTINUE-Zeile,
  exit=1), `integration_r30_irons_tisch_licht` (Timeout 400,32 s), `integration_r30_titel_puls` ((D) 1 von 3 Perioden
  ausserhalb der Bilddauer-Toleranz — Zeitmessung), `integration_relatch_pin` (exit=1 nach F60).
* **Einzelwiederholung** (je `ctest -R "^<name>$"`, nacheinander): alle sechs **Passed** — cut_blitz 71,93 s,
  granate_laden 109,01 s, irons_tisch_laden 32,74 s, irons_tisch_licht 57,52 s, titel_puls 28,79 s, relatch_pin 24,68 s →
  Last-Flattern (Memory reai-v2-gui-tests-flattern-bei-parallelen-agenten), kein reproduzierbares Rot. Keiner dieser Tests
  beruehrt Spur-D-Code: main.c ruft keine re2fx-Funktion (Bindung = INTEGRATIONSWUNSCH), re2fx_pc.c ist gelinkt, aber
  ohne Aufrufer.
* **Lauf 2** (`local_build.sh` all, 01:07-01:24, CPU-Last waehrend des Laufs 91 %): `99% tests passed, 4 tests failed out
  of 431`, `Total Test time (real) = 863.79 sec`. Rot wieder NUR exe-Tests, andere Menge als Lauf 1:
  `integration_r30_cut_blitz` (Teil C exit=1), `integration_r30_granate_laden` (abgerissen, exit=1),
  `integration_r30_irons_tisch_bild` (exit=1), `integration_r30_titel_puls` ((D) Perioden-Messung).
* **Einzelwiederholung** nach Lauf 2: cut_blitz Passed 102,45 s, granate_laden Passed 132,97 s, irons_tisch_bild Passed
  83,22 s; titel_puls bei CPU-Last 99 % **Failed** ((D) 62 bzw. 58 Engine-Schritte je Periode statt 60 — Bilddauern bis
  105 ms), bei Last 42 % gestartet **Passed** 29,36 s. Der Test misst die Echtzeit-Taktung des Titelschirms (title_pulse.c,
  main.c-Titelschleife), die Spur D nicht beruehrt → Last-Flattern, kein reproduzierbares Rot.
* **Lauf 3** (`local_build.sh` all auf 0a7eb5bf, also mit der OT-Reihenfolge 6b746ff4; Start 02:06:48 nach Lastabfall auf
  42/30 %, waehrend des Laufs wieder 96-100 % durch die Suiten der Spuren A/B/C und weitere Baeume): `99% tests passed,
  4 tests failed out of 431`, `Total Test time (real) = 749.10 sec`, Abschlusszeile `!!! [local_build] FEHLER: ctest
  fehlgeschlagen (exit=8), Log: re15_port/build_r34_d/local_build_ctest.log`. Rot NUR exe-Tests, wieder eine andere Menge:
  `integration_r30_granate_laden` ([b] exit=1, debug.log endet nach `[pad] kein Controller gefunden`),
  `integration_r30_sicherung_laden` ([a] exit=1 vor Bild 280), `integration_r30_titel_puls` ((D) 61 Engine-Schritte in
  Periode 3), `integration_relatch_pin` (exit=1 nach 4,45 s, debug.log 47 Zeilen).
* **exit=1 ist kein Absturz** (gemessen): das Windows-Anwendungsprotokoll (Ereignis 1000/1001/1002, 00:25-02:25) enthaelt
  KEINEN re15_pc-Eintrag; Kontrolle, dass es aufzeichnet: 78 Ereignisse 1000 in 30 Tagen, darunter re15_pc mit
  0xc0000005 am 06./07.09. Die exit(1)-Pfade der exe (render_pc.c:582/617/630/638) schreiben vorher eine Meldung ins
  debug.log — sie fehlt. Das Prozessende kommt also von aussen (TerminateProcess mit Code 1, wie `taskkill /F`). Zur selben
  Zeit zeigen die Suiten der Spuren B und C dieselbe Klasse (cut_blitz, elza_vollstart, irons_tisch_laden rot; nur gelesen).
* **Einzelwiederholung** nach Lauf 3 (nacheinander, je `ctest -R "^<name>$"`): granate_laden **Passed** (Last 100 %),
  sicherung_laden **Passed** (99 %), relatch_pin **Passed** (97 %); titel_puls bei 96 % **Failed** ((D) 61/59 Engine-
  Schritte), nach drei Lastproben < 50 % (63 → 22/16/7 %) gestartet **Passed** (02:37:56-02:38:24). Ueber drei Laeufe:
  kein Test ist reproduzierbar rot, die rote Menge wechselt von Lauf zu Lauf, jeder rote Test ist ein exe-Test ohne
  Spur-D-Aufrufer (`grep re2fx platform/pc/main.c` leer; Diff zu C0 nur in den Spur-D-Dateien).
* **Lauf 4** (`local_build.sh` all auf d3eb4fc8, Start 02:38 bei 30-47 % Last, ab 02:41 wieder 88-100 %; Suite B, ein
  Android-Bau und weitere Baeume parallel): `99% tests passed, 3 tests failed out of 431`, `Total Test time (real) =
  1103.50 sec`, Abschlusszeile `!!! [local_build] FEHLER: ctest fehlgeschlagen (exit=8), Log:
  re15_port/build_r34_d/local_build_ctest.log`. Rot NUR exe-Tests, wieder eine andere Menge:
  `integration_elza_vollstart` ((c) Szene nicht bis zur Spielfreigabe im Zeitbudget), `integration_r30_irons_tisch_licht`
  ([P2] exit=1 vor dem Laden), `integration_r33_speichern` ([c] exit=1, Lauf unvollstaendig). Die drei Spur-D-Sonden
  `unit_r34_re2fx`, `unit_r34_re2fx_knochen`, `unit_r34_re2fx_bild` sind in allen vier Laeufen gruen.
* **Einzelwiederholung** nach Lauf 4 (nacheinander): irons_tisch_licht **Passed** (Last 97-100 %), r33_speichern
  **Passed** (100 %), elza_vollstart **Passed** (100 → 76 %). Stand ueber vier Laeufe: 17 rote Eintraege (6 + 4 + 4 + 3) in 10 verschiedenen
  exe-Tests, KEINER reproduzierbar (jeder einzeln gruen, titel_puls bei niedriger Last); die Ziel-Zeile
  `=== LOCAL-BUILD-OK (all)` kam unter der Dauerlast der parallelen Spuren in keinem Lauf zustande.

---

## NACHBESSERUNG (nach Gegenpruefung bau_d.gegenpruefung.md: M1-M4, Hinweise M5/M6)

Stand: 2026-09-30, Zweig `r34g/d-re2fx`, Basis `0749ed00`. Alle Adressen in DIESER Sitzung mit
`re2_disasm.py` (RE2 `info/re2leon/PSX.EXE`) bzw. `re15_disasm.py` (RE1.5 `info/Re1.5/PSX.EXE`) gelesen.
Mutationsproben mit `build/r34g_d/mut/mut.py` (unversioniert): genau eine Ersetzung in `re2_fx.c` →
`local_build.sh build` → Sonde(n) → `git checkout -- re2_fx.c` → `git diff` leer (jede Zeile unten so gelaufen).

### N1 — M1 behoben: `re2fx_boden` nach der RE2-Vergleichsregel (re2_fx.c `re2fx_boden`, `zelle_im_band`)

**Befund bestaetigt** (Messung mit Mutation A1b = alte Regel nur in Op 27): ROOM1140-Wandpunkt → Flammen-+0x14
`FFFFF8F8`, erster Applier-Ruf **X+3**; mit der neuen Regel `00000000` und **X+19** (wie auf freiem Boden).

**RE2 FUN_8004fba0, selbst disassembliert** (0x8004fba0-0x8005010c, 330 Instruktionen):

```
8004fc34 sw zero,-13368(at)   ; Kontakt 0x800DCBC8 := 0
8004fc3c sh zero,15228(at)    ; Rueckgabe 0x800C3B7C := 0
8004fc48 blez v1 / 8004fc58 sw v0(=1),-13368(at)   ; P.y > 0 -> Kontakt 1
8004fc5c bne s0(=a3),zero,0x800500c8                ; a3 != 0 -> keine Objekte
--- Objekte 0x800D0324.. Schritt 504: jal 0x80036e30 (Kasten), jal 0x80038950(P,obj,r,0)
8004fcbc lhu a2,22(s1) / lw a0,0(s1) / subu v1,a0,a2   ; oben = +0x88 - +0x9E
8004fccc slt v0,v1,a1 / bne -> 0x8004fce0 ; delay addu v0,a0,a2 (unten)
8004fcd8 j 0x8004fd38 / addu a0,v1,zero    ; P.y <= oben: Kandidat oben
8004fce0 slt v0,v0,a1 / bne -> naechstes  ; P.y > unten: nichts
8004fd0c ori v0,v0,0x1 (Kontakt) ... 8004fd34 addiu a0,v0,-1   ; innen: Kandidat P.y - 1
8004fd44 slt v0,a0,v0 / 8004fd54 sh a0,15228(at)              ; Rueckgabe = min
--- Formen (16 B): 8004fd84-fdb8 (u32)(P.x + r - x) < (u32)(w + 2r)  [ERWEITERT um r]
8004fde4-fe04 unten s1 = -1800 * Bitindex(+12) ; 8004fe08-fe30 oben s0 = -1800 * ((+10>>6)&0x1f)
8004fe44 Sprungtabelle 0x80011104 (Typ 0/9 Rechteck, 1..8 Formtests, 11..13 Rampen)
8004ffdc slt v0,s1,v1 / bne 0x8005005c     ; P.y > unten -> nur Buchhaltung 0x800D5BE4
8004ffe4 slt v0,s0,v1 / beq 0x80050028     ; P.y <= oben -> Rueckgabe-Zweig
80050000 ori v0,v0,0x1                     ; innen: Kontakt |= 1
80050034 slt v0,s0,v0 / 80050044 sh s0,15228(at)   ; Rueckgabe = min(oben)
80050050 slt v0,v0,s0 / bne naechste Form  ; P.y < oben: fertig
80050064 slt v0,v0,s1 / 80050080 ori v0,v0,0x2     ; P.y < unten: Kontakt |= 2
800500d8 lh v0,15228(v0)                   ; Rueckgabe s16
```

FUN_80038950 mit a3 = 0: nur XZ (`beq a3,zero,0x800389e0` @0x800389a4), Rand `addu v0,v0,a2` @0x80038978 —
**kein Band-/Hoehentor**. Op 28/29 werten den Kontakt als `!= 0` (`beq v0,zero` @0x8001fce4 / @0x8001fe84).

**Port-Zuordnung (neu, je Glied belegt):**

| RE2-Glied | Port (RE1.5-Daten) | Beleg |
|---|---|---|
| Form | SCA-Zelle Band b, Filter wie FUN_8001c6e8: `andi v0,v0,0xf002 / bne s3,v0` @0x8001c89c-a0, Maske 0x100 (`lh v0,6(a1) / and v0,v0,fp` @0x8001c8a8-b4; Routine-12-Argument `ori a3,zero,0x100` @0x800177d0), Baender 7..0 (`ori a2,zero,0x8` @0x800177c4) | RE1.5-Gegenstueck "Boden unter Effekt-Teilchen" (ESP-Routine 12, §2.4) |
| Form-Oberkante | −1800·(b+1) | FUN_8001c6e8 `srl v1,s3,12 / addiu v1,v1,1 / … / sll v0,v0,19 / subu / sra t0,v0,16` @0x8001c868-88c |
| Form-Unterkante | −1800·b | Standhoehe des Bandes: Spieler-y := −1800·(+0x82) `sll v0,v1,3 / … / subu v0,zero,v0 / sw v0,-13684(at)` @0x8001d7b8-d4 |
| Rechteck ± r | FUN_8001c6e8-Zellenausdruck mit **−r** (RE1.5 SCHRUMPFT um r: `addu v0,v0,s6` / `subu a0,a0,a3` @0x8001c8c8-d0; RE2 erweitert) → identisch `(u32)(P.x + r − x) < (u32)(w + 2r)` | @0x8004fd84-98 |
| Quadrant | FUN_8003b068 (`and/srl 1/or/srl 30` @0x8003b084-a0, Versatz 0x80010694 = {0,0,0,0}) | wie `quadrant_of` (re15_collision.c) |
| Objekt | aktives Obj_model_set-Prop, XZ = FUN_8002da4c-Zwilling mit Rand r und dem EIGENEN Band des Props (RE2 ohne Tor); oben = y − 2·hy (`lhu v1,8(v0) / lhu v0,56(s0) / sll v1,v1,1 / subu` @0x8001c828-834), unten = y | @0x8004fcbc-fd54 |
| Vergleich | wortgleich zur Disasm oben (inkl. Kontakt-Bit 2 und Kandidat P.y − 1) | re2_fx.c `re2fx_boden` |

`zelle_im_band` scannt nur den ZELLEN-Teil von FUN_8001c6e8 fuer ein Band (die Objekt-Stufe von FUN_8001c6e8
wuerde vor den Zellen eine Objekt-Oberkante liefern, @0x8001c810-840). Ein Aufruf mit Maske `0x100|0x10000`
waere FALSCH: FUN_8001c6e8 erweitert das Zellwort vorzeichenrichtig (`lh v0,6(a1)` @0x8001c8a8), bei u0-Bit 7
traefe Bit 0x10000.

Folge fuer die erreichbaren Faelle (Flammen immer mit y > 0, BAUPLAN §1.1): Op 27 (P.y = y) liegt unter jeder
Band-0-Zelle und unter jedem Prop (unten = 0) → Rueckgabe 0, +0x14..+0x17 = 0 wie in RE2 (vorher −1800 bzw. die
Prop-Oberkante → +0x16 = 0xFFFF). Op 28 (P.y = y − 900) in einer Wandzelle → Kontakt, Rueckgabe 0 < y → Landung
wie vorher. Op 29 (P.y = y − 100) in der Wandzelle → Kontakt → Op 50 (wie vorher). Zellen hoeherer Baender ueber
der Flamme (ROOM1170 Band 2) wirken jetzt wie freier Boden (vorher −5400 → +0x16 = 0xFFFF).

Neu (additiv, re2_fx.h): `re2fx_boden_sonde(p, r, mask, a3, &kontakt)` fuer Sonden.

### N2 — M2 behoben: Sonde `unit_r34_re2fx_raum` (tests/unit/probe_r34_re2fx_raum.c) mit echten Raeumen

Laedt ROOM1140/1150/1170 nach `g_room_rdt` (Haken NULL). Punkte (Werkzeug `build/r34g_d/raum/punkte.c`,
`zellen.c`, unversioniert; in der Sonde als Vorbedingung 101-108 nachgeprueft):

| Raum | Wandpunkt (Zelle) | Freipunkt | Sonderpunkt |
|---|---|---|---|
| ROOM1140 | (−2750, −5750) in Zelle 3 x −5750..16700 z −7800..1750, Band 0, u0 0xFF | (−3000, −20500), 0 im Umkreis 2500 | — |
| ROOM1150 | (−20000, −19000) in Zelle 0 x −21782..−17858 z −20864..−15914 | (−23750, −13750) | Zellrand x −21784 / −21785 |
| ROOM1170 | (−19250, −18250) in Zellen 16/17 | (−28750, −14250) | Band-2-Zellen 23/25 (floor 0x23) bei (−27750, −27000): −5400 |

(Die Prueferpunkte lagen teils am Zellrand — Tiefe 0 — bzw. ausserhalb der SCA-Huelle; die Sonde nimmt Punkte
mit Umkreis ≥ 1000 bzw. ≥ 2500.)

Gemessen und gepinnt (Rueckgabe / Kontakt der Bodensonde):

| Punkt | P.y −2500 | −1800 | −1799 | −900 | 0 | +10 |
|---|---|---|---|---|---|---|
| Wand (Band 0) | −1800 / 0 | −1800 / 2 | 0 / 3 | 0 / 3 | 0 / 1 | 0 / 1 |
| frei | 0 / 0 | — | — | 0 / 0 | 0 / 0 | 0 / 1 |
| Band-2-Zelle | −6000: −5400 / 0; −5400: −5400 / 2; −4000: 0 / 3; −3600: 0 / 1 | | | 0 / 0 | | 0 / 1 |
| Prop (oben −600) | −900: −600 / 0; −600: −600 / 0; −300: **−301** / 1; 0: −1 / 1 | | | | | 0 / 1 |

Flammenlauf (`re2fx_aufschlag(1, {x, 10, z}, 0)`, 60 Bilder, X = 0): Wand / Prop → +0x14 `0`, Landung X+2,
Op 50 in X+3 (erstes Gleitbild, Kontakt Op 29 @0x8001fe84), danach Stillstand, erster Applier-Ruf X+19, 123 Rufe;
frei / Band-2 → +0x14 `0`, gleitet 1258-1443 in 12 Bildern, kein Op 50, erster Applier-Ruf X+19. Alle drei
Raeume identisch. Negativ-Kontrollen: derselbe Wandpunkt ohne Raum (`g_room_rdt_ok = 0`) und mit Boden-Haken
gleitet wie frei (610/620); Gegenprobe mit Raum haelt (630); Prop-Band 1 statt 0 aendert nichts (503, RE2 ohne
Band-Tor); a3 = 1 schaltet die Objekte ab (502); Rand r = 2 an Zelle (208/209) und Prop (504/505).

Mutationsproben (alle rot, danach `git diff` leer):

| # | Mutation in re2_fx.c | Ergebnis |
|---|---|---|
| A1b | Op 27 nimmt wieder FUN_8001c6e8(P,0,8,0x100) (alte Regel) | `FAIL 301` (+0x14 = FFFFF8F8, Applier ab X+3) |
| A1 | Rueckgabe wieder y-unabhaengig (room_coll am Ende von re2fx_boden) | `FAIL 201` |
| A2 | Zellen aus (= Pruefer-Mutation B) | `FAIL 201` |
| A3 | Objekt-Kandidat P.y statt P.y − 1 | `FAIL 501` |
| A4 | Kontakt-Bit 2 aus | `FAIL 201` |
| A5 | Rechteck um r geschrumpft statt erweitert | `FAIL 208` |
| A6 | Prop mit Band-Tor aus P.y | `FAIL 501` |
| A7 | Unterkante −1800·(b+1)+900 statt −1800·b | `FAIL 201` |

### N3 — M4 behoben (+ Hinweise M5/M6): Weltlage-Normalzweig in `unit_r34_re2fx` (8xx)

* 801-834: Bodenflamme mit Gier genau 1024 / 2048 / 3072 (Aufschlag-Gier = Ziel − r%40, Zug aus dem
  Strom-Nachbau), jedes Bild ab X+1: Weltlage == M.t + Viertel(lokal VOR dem Bild) — Viertel-Drehung von Hand
  aus RotMatrixY FUN_8008e8b4 auf I = [[c,0,s],[0,1,0],[−s,0,c]] (`subu t1,zero,t7` @0x8008e910,
  @0x8008e918-a40): 1024 → (l.z, l.y, −l.x), 2048 → (−l.x, l.y, −l.z), 3072 → (−l.z, l.y, l.x); M.t == Q;
  lokal.x == Start + Σ vel.x (Physik @0x8001d720-794); Gleitrichtung 1024 → −z (Versatz (0, −1558)),
  2048 → −x, 3072 → +z; Gleitweg 1260.
* 840-843: M.rot·Versatz mit Nicht-Einheitsmatrix (direkter Spawn, M.rot = [[0,0,4096],[0,4096,0],[−4096,0,0]],
  M.t = (1000,20,−2000), Versatz (100,5,300) → (300,5,−100)) jedes Bild; Negativ-Kontrolle Versatz 0 → M.t.
  (Kein Aufschlag-Kind nutzt M.rot ≠ I zusammen mit Versatz ≠ 0 — Saeure-Kinder/Folgeflammen: M = I, Versatz =
  Elternlage; Brand-Kinder/Flammen: M = Aufschlag-Matrix, Versatz 0 — der Normalzweig rechnet es trotzdem.)
* M5: 401 jetzt exakt `vel.x == 180 + acc.x` nach dem Landebild (Op 46 `addiu v0,zero,180` @0x80020ba4, acc.x
  @0x80020bb0-f4, Physik danach), 408 acc.x ∈ [−20, −10].
* M6: 337/338 Op 27 in X+1 je Flamme (Draw-Pass aufsteigend 90, 91, 92): Anim r%3 (@0x8001faa4-f4), Luftzaehler
  8 + r%3 (@0x8001fbb4-b8) gegen den Strom-Nachbau.

| # | Mutation in re2_fx.c | Ergebnis |
|---|---|---|
| H | Normalzweig Gier negiert (= Pruefer-Mutation H) | `FAIL 803` |
| H2 | M.rot·Versatz mit transponierter Matrix | `FAIL 841` |
| C2 | Op 46 vel.x 180 → 189 (= Pruefer-Mutation C2) | `FAIL 401` (raum: `FAIL 305`) |
| D | Op 27 Luftzaehler 8 → 9 (= Pruefer-Mutation D) | `FAIL 338` |
| P | Physik: vel += acc VOR lokal += vel | `FAIL 216` |
