# Runde 34 (Granaten) — Bau Spur D: RE2-FX-Maschine (Saeure 0x0A / Brand 0x0B)

Stand: 2026-09-29, Zweig `r34g/d-re2fx`, Arbeitsbaum `.claude/worktrees/r34g_d`, Basis C0 `8d8651e4`.
Auftrag: BAUPLAN §1.3, §1.4, §3.3 C5/C6, E8, K8, P28, O-VB1, O-VB2, O9 (Orchestrator-Teilung: D = RE2-FX-Maschine).
Dateibesitz: `engine/src/re2_fx.c`, `include/re2_fx.h` (additiv), `platform/pc/src/re2fx_pc.c/.h`, `tools/` (neu),
`tests/unit/probe_r34_re2fx*.c`, `tests/unit/probes/r34_re2fx.cmake`.

Alle RE2-Adressen = `info/re2leon/PSX.EXE` (t_addr 0x80010000), in DIESER Sitzung mit
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` gelesen; Roh-Disasm unter `build/r34g_d/dis/` (unversioniert).
RE1.5-Adressen = `info/Re1.5/PSX.EXE` ueber `re15_disasm.py`. GTE-Worte, die das Werkzeug als `.word` zeigt, sind von Hand
dekodiert und jeweils mit ihrem Rohwort zitiert.

STATUS: IN ARBEIT (Paket 1 = O-VB1/O-VB2).

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
