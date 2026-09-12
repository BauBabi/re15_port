# G5 (RE2 EM036): der dir[0]-VERTEX-MORPH der Fleischmasse — Konsument, Format,
# Gewichte, Umsetzungs-Spezifikation (2026-09-13, Runde 7)

Nutzer: „Birkin hat keine Animationen usw." — der Kriecher (Mesh 0/1) animiert, die
**FLEISCHMASSE (Mesh 2, 7,3 m breit, 4,2 m hoch)** steht starr davor und verdeckt alles.
RE2 verformt genau dieses Mesh jeden Frame per Vertex-Morph aus `dir[0]` des EM036-EMD,
getrieben von 4 Gewichten, die die KI führt. `enemy_ai_boss_g5.c g5_blob_tick`
simuliert die Gewichte bereits — es fehlt der KONSUMENT.

Alle Belege in diesem Dossier sind in dieser Runde selbst gemessen:
* EXE-Disasm: `info/re2leon/PSX.EXE` (PS-X EXE, `t_addr 0x80010000`, Text ab Datei 0x800;
  Datei-Offset = Adresse − 0x80010000 + 0x800), Werkzeug
  `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr>`.
* Overlay-Disasm: `build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN` (23476 B = 0x5BB4,
  md5 `b57f8315dc8ae5583df13b4699395f6d`, gelinkt @0x80100000; Datei-Offset =
  Adresse − 0x80100000), `RE_OVERLAY_DIR=build/extracted/re2_ems … --bin CDEMD0_EM36_ai1.BIN`.
* EMD: `re15_port/shared_assets/RE2/CDEMD0.EMS` **Datei-Offset 0x6E6800**
  (TOC-Eintrag kind 0x36 rec 3 = Sektor **0xDCD**, Größe **0x1B6D4** —
  `re15_port/engine/src/gen/re2_ems_toc.inc`, selbst nachgerechnet; der Byte-Schnitt
  `build/extracted/re2_ems/CDEMD0_EM36_emd.EMD` ist **bytegleich** zu diesem Slice).
  Alle „EMD 0x…"-Offsets unten sind EMD-relativ (= EMS 0x6E6800 + Offset).

---

## 0. Kurzfassung (was gebaut werden muss)

1. `dir[0]` ist **kein Reserve-Verzeichnis**, sondern ein **Morph-Block**:
   1 Basis-Vertextabelle + **4 absolute Ziel-Vertextabellen** für **Mesh 2** (635 Vertices,
   je 3× s16, Segmentgröße 0xEE4). Byte-bewiesen, §2.
2. Der Konsument ist **`FUN_8004BF90` @0x8004bf90** (EXE-Handassembler, kein Decompile im
   Repo — hier erstmals roh disassembliert, §1). Er stellt jeden Frame die Basis-Pose in
   den Vertexpuffer des Meshes zurück und blendet danach Segment für Segment:
   `v = ((4095−w)·v + w·ziel) >> 12`, Segmente mit `w == 0` werden **übersprungen**.
3. Die 4 Gewichte liegen als **u16 @ctrl+12/+16/+20/+24** — die Zuordnung aus
   birkin-g5-ki.md §4 ist **BESTÄTIGT** (§3), inklusive der genauen Wertebereiche.
4. Im Port fehlt (a) der Parser (`re2_ems.c`), (b) die Morph-Funktion (neue
   `emd_morph.c`), (c) die Abfrage im G5-Modul, (d) der Zeiger-Tausch im Zeichner
   (`platform/pc/main.c:8475/8497/8576`). Spezifikation in §4.

---

## 1. DER KONSUMENT — `FUN_8004BF90` @0x8004bf90 (Datei 0x3C790…0x3C994)

### 1.1 Aufrufstelle und Argumente (Overlay)

```
80103bcc: sll   v0,v0,2                       ; v0 = entity+0x218 (Blob-Zustand) * 4
80103bdc: lw    v0,23228(at)                  ; Tabelle 0x80105ABC[zustand]
80103be4: jalr  v0                            ; Blob-Maschine (§3)
80103be8: addu  s1,a1,zero                    ; s1 = a1 = &part2
80103bec: addu  a0,s0,zero                    ; a0 = ENTITY
80103bf0: lw    a2,8(s1)                      ; a2 = *(part2+0x08)  = MD1-Objekt-Zeiger
80103bf4: lw    a3,136(s1)                    ; a3 = *(part2+0x88)  = Morph-Controller
80103bf8: jal   0x8004bf90
80103bfc: addu  a1,s1,zero                    ; a1 = &part2
```
Aufruf aus dem Per-Frame-Main: `801002e8: lw a1,408(s3)` (= `*(entity+0x198)` = Part-Array),
`801002f0: jal 0x80103bb4`, `801002f4: addiu a1,a1,344` → **a1 = parts + 0x158 = part2**
(Part-Record 0xAC Bytes, 2·0xAC = 0x158). Damit ist `part2+0x88 = parts+0x1E0` — exakt die
Adresse, die der Konstruktor benutzt (`80100744: lw a1,408(s0)`, `8010074c: lw v0,480(a1)`,
`80100754: sh zero,12(v0)`, `80100764: sh zero,16(v0)` = Gewicht[0]/[1] = 0).

**Ein einziger Aufruf pro Frame, ungegatet.** Korrektur zu birkin-g5-ki.md §1.5: der
`(+0x226 & 8)`-Test @0x801001a8 (`bne v0,zero,0x801002b8`) springt auf **0x801002b8**, also
VOR die Augen-Wanderer — er überspringt nur das Augen-Ziel-Neuwerfen. Schüttel-Treiber,
Ooze-Emitter, Frame-Flag-SE **und die Blob-Maschine samt Morph** (0x801002f0) laufen auch
in der Todesphase weiter. Für die Absink-Rampe [T20] (Gewicht[2], §3) ist das zwingend.

### 1.2 Der Code, instruktionsweise

**Kopf + Basis-Restore** (5-fach abgerollt, `t0 -= 5` je Runde):
```
8004bf90: addiu sp,sp,-16               ; Bytes f0 ff bd 27 @Datei 0x3C790
8004bf94: addiu t3,zero,1                     ; t3 = Segment-Index, startet bei 1
8004bfa0: lw    t0,4(a2)                      ; t0 = obj[1] = VERTEX-ZAHL   (EM036 Mesh2: 635)
8004bfa4: lw    a2,0(a2)                      ; a2 = obj[0] = VERTEX-ARRAY (Ziel, SVECTOR-Stride 8)
8004bfa8: lw    v1,444(a0)                    ; v1 = *(entity+0x1BC) = MORPH-BLOCK-BASIS
8004bfac: lw    v0,0(a3)                      ; v0 = ctrl[0] = data_off
8004bfb0: addu  s1,t0,zero                    ; s1 = Vertexzahl (gesichert)
8004bfb4: addu  s0,a2,zero                    ; s0 = Vertex-Array (gesichert)
8004bfb8: addu  v1,v0,v1                      ; v1 = Basis + data_off  = SEGMENT 0
8004bfbc: addu  t9,v1,zero                    ; t9 = Segment-0-Zeiger (gesichert)
8004bfc0: addiu a0,a2,4
8004bfc4: lhu   v0,0(v1) … sh v0,0(a2)        ; kopiert je Vertex 3 s16 (Stride 6)
8004bfcc: addiu t0,t0,-5                      ;   in den Vertexpuffer (Stride 8; +6 = pad
8004c0a0: bne   t0,zero,0x8004bfc4            ;   bleibt UNBERUEHRT)
```
⇒ **Schritt 1: der Vertexpuffer des Meshes wird jeden Frame aus Segment 0 komplett
zurückgesetzt.** (EM036 Mesh 2: 635 = 5·127 Vertices, die 5-fache Abrollung geht exakt auf.)

**Segment-Schleife** (`t3` = 1 … seg_count):
```
8004c0a8: addiu t8,zero,4095            ; Bytes ff 0f 18 24 @Datei 0x3C8A8
8004c0ac: lw    v0,4(a3)                      ; ctrl[1] = SEGMENTGROESSE in Bytes
8004c0b0: lhu   t2,8(a3)                      ; ctrl[2] (u16) = SEGMENT-ZAHL
8004c0b4: srl   t7,v0,1                       ; t7 = seg_size/2
8004c0b8: mult  t3,t7 / mflo a1               ; a1 = t3 * seg_size/2
8004c0c0: sll   v0,t3,2                       ; ---- Schleifenkopf ----
8004c0c4: addu  v0,v0,a3
8004c0c8: lhu   a0,8(v0)                      ; a0 = GEWICHT = *(u16*)(ctrl + 4*t3 + 8)
8004c0d0: beq   a0,zero,0x8004c174            ;   w == 0 -> Segment KOMPLETT UEBERSPRINGEN
8004c0d4: subu  t1,t8,a0                      ; t1 = 4095 - w
8004c0d8: sll   v0,a1,1
8004c0dc: addu  v1,t9,v0                      ; v1 = Segment0 + t3*seg_size = ZIELTABELLE
8004c0e0: addu  a2,s0,zero                    ; a2 = Vertexpuffer von vorn
8004c0e4: addu  t0,s1,zero                    ; t0 = Vertexzahl
```
**Innenschleife je Vertex** (GTE-Worte selbst dekodiert — `re2_disasm.py` druckt sie roh):
```
8004c0e8: .word 0x48894000 = mtc2 t1,$8             = gte_ldIR0(4095 - w)
8004c0ec: lhu t4,0(a2) / lhu t5,2(a2) / lhu t6,4(a2)     ; AKTUELLER Vertex aus dem Puffer
8004c0f8: .word 0x488c4800 = mtc2 t4,$9  (IR1)
8004c0fc: .word 0x488d5000 = mtc2 t5,$10 (IR2)
8004c100: .word 0x488e5800 = mtc2 t6,$11 (IR3)
8004c104: lhu v0,0(v1); sh v0,0(sp)           ; Ziel.x  (Segmenttabelle, Stride 6)
8004c110: .word 0x4b98003d = COP2 cmd 0x3D, sf=1 -> GPF  -> MAC = IR0*IR(n) >>12
8004c114: lhu v0,0(v1); sh v0,2(sp)           ; Ziel.y
8004c120: lhu v0,0(v1); sh v0,4(sp)           ; Ziel.z
8004c12c: .word 0x48844000 = mtc2 a0,$8             = gte_ldIR0(w)
8004c130: lhu t4,0(sp) / lhu t5,2(sp) / lhu t6,4(sp)
8004c13c: mtc2 t4,$9 / mtc2 t5,$10 / mtc2 t6,$11
8004c14c: addiu t0,t0,-1
8004c150: .word 0x4ba8003e = COP2 cmd 0x3E, sf=1 -> GPL  -> MAC += IR0*IR(n); IR = MAC>>12
8004c154: .word 0x480c4800 = mfc2 t4,$9   (IR1)
8004c158: .word 0x480d5000 = mfc2 t5,$10  (IR2)
8004c15c: .word 0x480e5800 = mfc2 t6,$11  (IR3)
8004c160: sh t4,0(a2) / sh t5,2(a2) / sh t6,4(a2)        ; ZURUECK in den Vertexpuffer
8004c16c: bne t0,zero,0x8004c0e8
8004c170: addiu a2,a2,8                        ; SVECTOR-Stride 8
8004c174: addu  a1,a1,t7                       ; naechstes Segment
8004c178: addiu t2,t2,-1
8004c17c: bne   t2,zero,0x8004c0c0
8004c180: addiu t3,t3,1
8004c190: jr    ra                      ; Bytes 08 00 e0 03 @Datei 0x3C990 (Funktionsende)
```

### 1.3 Die Formel — exakt

Pro Segment i (1 … seg_count), nur wenn `w_i != 0`, für jeden Vertex v:

```
IR0a = (s16)(4095 - w_i)          IR0b = (s16)w_i          ; MTC2 schreibt 16 Bit
v'   = ( IR0a * v  +  IR0b * ziel_i ) >> 12                ; GPF (sf=1) dann GPL (sf=1)
v'   = clamp_s16(v')                                       ; GTE IR1..3, lm=0
```
Wichtig für die Portierung:
* **Kettenbildung.** Segment 2 rechnet auf dem ERGEBNIS von Segment 1, nicht auf der Basis.
  Das ist kein gewichteter Mittelwert über 4 Ziele, sondern 4 aufeinanderfolgende Lerps.
* **4095, nicht 4096.** Jedes angewandte Segment skaliert den Ist-Vertex zusätzlich mit
  4095/4096 (≈ −0,024 %). Bei bis zu 4 aktiven Segmenten ≤ 0,1 % — trotzdem 1:1 übernehmen.
* **Die Gewichte dürfen weit außerhalb [0,4095] liegen.** Die KI schreibt bis **12000**
  (≈ 2,93× Extrapolation, §3 Todes-Blob) und bis **−4048** (≈ −0,99×, Gegenrichtung). Die
  Identität `IR0a + IR0b = 4095` bleibt nach der 16-Bit-Kappung erhalten, solange
  `4095 − w` in s16 passt (w ∈ [−28672, 36863]) — für alle vorkommenden Werte erfüllt.
* **Normalen werden NICHT gemorpht** (kein zweiter Durchlauf im Code) — die Beleuchtung
  läuft auf der Basis-Normalentabelle weiter.
* Nur x/y/z werden geschrieben, das vierte Halbwort (`pad`) bleibt stehen.

### 1.4 Abgrenzung zu `FUN_8004baf0` (die einzige Morph-Funktion im Decompile-Satz)

`RE2_Quellcode_V2/FUN_8004baf0.c` ist die **2-Argument-C-Fassung** desselben Algorithmus
(sie holt sich `part+0x08` und `part+0x88` selbst). Sie hat **zusätzlich** einen Block, der
die **Bind-Translationen der Part-Records** (`part[b]+0x2C/+0x30/+0x34`) aus einer
Vektortabelle bei `block+block[1]` morpht, indiziert über eine Byte-Liste im EMR-Pool.
Für EM036 ist dieser Block **tot**: die Bedingungen `uVar15 < uVar2` und
`*(int*)((int)r0_01+8) == 0` prüfen `part_index (=2) < bone_count (=2)` — beide falsch.
Das Overlay ruft `0x8004baf0` genau **einmal** auf (Konstruktor `80100760: jal 0x8004baf0`,
mit vorher genullten Gewichten = reiner Basis-Restore) und danach **jeden Frame nur noch
`0x8004bf90`**, das den Bind-Block gar nicht erst enthält. ⇒ **Der Port braucht nur die
Vertex-Hälfte.** (Vollständiger Scan: die einzigen `jal` ins Fenster 0x8004b000-0x8004cfff
im Overlay sind 0x80100760 → 0x8004baf0 und 0x80103bf8 → 0x8004bf90.)

---

## 2. DAS FORMAT DES `dir[0]`-BLOCKS (selbst geparst, byte-belegt)

### 2.1 Beweis: `entity+0x1BC` = EMD + dir[0]

EMD-Binder in der RE2-EXE (roh disassembliert):
```
8001ab88: lw   v1,0(s2)      ; s2 = EMD-Basis; v1 = *(emd+0)   = dir_off
8001ab90: addu s0,s2,v1      ; s0 = Verzeichnis
8001ab94: lw   v1,0(s0)      ; v1 = dir[0]
8001ab9c: addu v1,s2,v1
8001aba0: sw   v1,444(s1)    ; ENTITY+0x1BC = EMD + dir[0]      <-- MORPH-BLOCK
8001abb0: sw   v1,380(s1)    ; +0x17C = dir[1] (EDD)
8001abc0: sw   v1,264(s1)    ; +0x108 = dir[2] (EMR/Struktur)
8001abd0: sw   v1,388(s1)    ; +0x184 = dir[3]
8001abe0: sw   v1,384(s1)    ; +0x180 = dir[4]
8001abf0: sw   v1,396(s1)    ; +0x18C = dir[5]
8001ac00: sw   v1,392(s1)    ; +0x188 = dir[6]
8001ac14: sw   v1, 20(s1)    ; +0x14  = dir[7] (MD1)
```
(Die Zuordnung dir[1..7] deckt sich mit `re15_port/engine/src/re2_ems.c:102-115`; **dir[0]
war dort bisher unbelegt** — hiermit erledigt.)
`FUN_8004b3b8` (RE2_Quellcode_V2/FUN_8004b3b8.c) verteilt den Block anschließend auf die
Parts (Aufrufer `0x80057674` / `0x80057d08`, jeweils mit `lw a1,444(s0)` davor):
`*(entity+0x1BC) = block`; Maske `block[0]` bitweise über alle Meshes; für jedes gesetzte Bit
`part[i]+0x88 = record`, danach `record += (3 + record[2]) Worte`.

### 2.2 Blockaufbau

| Offset im Block | Typ | Feld | EM036-Wert | EMD-Offset | Beleg |
|---|---|---|---|---|---|
| +0x00 | u32 | **Mesh-Bitmaske** | **0x00000004** (nur Mesh 2) | 0x08 | `FUN_8004b3b8.c:7 (*param_2)` |
| +0x04 | u32 | Offset der Bone-Bind-Basistabelle | 0x28 → EMD 0x30 | 0x0C | `FUN_8004baf0.c` `*(iVar20+4)+iVar20` |
| +0x08 | u32 | Stride einer Bind-Segmenttabelle | 0x0C (= 2 Bones × 6 B) | 0x10 | `FUN_8004baf0.c` `*(iVar20+8)` |
| +0x0C | … | **Controller-Record** (erstes gesetztes Maskenbit) | | 0x14 | `FUN_8004b3b8.c:10 param_2+3` |

**Controller-Record** (Größe `(3 + seg_count) · 4`, EM036 = 28 B, EMD 0x14…0x2F):

| Offset im Record | Typ | Feld | EM036 | EMD-Offset | Beleg |
|---|---|---|---|---|---|
| +0x00 | u32 | `data_off` (relativ zur **Block**-Basis) | **0x34** → EMD 0x3C | 0x14 | `8004bfac: lw v0,0(a3)` |
| +0x04 | u32 | `seg_size` (Bytes je Segmenttabelle) | **0xEE4** = 3812 | 0x18 | `8004c0ac: lw v0,4(a3)` |
| +0x08 | u16 | `seg_count` (Zahl der ZIEL-Segmente) | **4** | 0x1C | `8004c0b0: lhu t2,8(a3)` |
| +0x0C+4i | u16 | **Gewicht[i]** (i = 0…seg_count−1) | 0/0/0/0 (Datei) | 0x20/0x24/0x28/0x2C | `8004c0c8: lhu a0,8(v0)` mit `v0 = a3 + 4·t3` |
| +0x0E+4i | u16 | Bind-Segment-Index (nur `FUN_8004baf0`) | 0/0/0/0 | 0x22/0x26/0x2A/0x2E | `FUN_8004baf0.c` `*(u16*)((int)piVar16+10)` |

**Vertex-Tabellen:** `seg_count + 1` Tabellen à `seg_size` Bytes ab `Block + data_off`,
jeweils `nverts` × 3 × s16 (Stride 6, am Ende auf 4 gepaddet).
* Segment **0** = **BASIS** (wird verbatim in den Vertexpuffer kopiert).
* Segment **1…seg_count** = **absolute ZIELPOSEN** (keine Deltas!).

**EM036 konkret** (alles nachgerechnet):
* Mesh 2 hat `tv_cnt = qv_cnt = 635` Vertices, Array @EMD 0x118FC (MD1 @dir[7]=0x11110,
  Header 12 B, Mesh-Record 2 @EMD 0x11190). Tri- und Quad-Vertexarray sind **dasselbe**
  Array → ein Morph deckt beide Zeichenschleifen ab.
* `635 · 6 = 3810`, auf 4 gepaddet = **3812 = 0xEE4** ✓ = `seg_size`.
* 5 Tabellen × 0xEE4 ab EMD 0x3C → Ende **EMD 0x4AB0** = **dir[1]** ✓ (lückenlos).
* **Segment 0 (EMD 0x0003C) ist BYTEGLEICH mit dem MD1-Vertexarray @EMD 0x118FC**
  (635/635 Vertices identisch, selbst verglichen) — der endgültige Beweis, dass
  Segment 0 die Basis-Pose ist und nicht eine fünfte Zielpose.
* Bind-Basistabelle @EMD 0x30 = `{(0,−4534,0), (2146,807,0)}` — das sind exakt die beiden
  Bone-Binds (Bone-0-Bind-Y −4534 = die Baseline, die [T0] nach 0x80105BB0 sichert;
  Bone-1-Bind (2146,807) = das Negat der Naht-SVECTOR (−2146,−807) @0x801059d8).
* Segment-Tabellen: 0 @EMD 0x0003C · 1 @0x00F20 · 2 @0x01E04 · 3 @0x02CE8 · 4 @0x03BCC.

### 2.3 Was die vier Ziele TUN (selbst vermessen, Mittelwerte je Höhenband)

Basis-Bounding-Box Mesh 2: `x −3600…2694, y −4224…24, z −3648…3648` (PSX: −y = oben).

| Segment | EMD-Offset | getrieben von | max. Versatz | Kuppe (y≤−3200) | Mitte | Sohle (y≥−1000) | Deutung |
|---|---|---|---|---|---|---|---|
| 1 | 0x00F20 | **Gewicht[0]** | 420 | (+14, +1) | (+23…−16, +25…−51, \|z\|−66…−109) | (+59, −49, \|z\|−48) | **ATMEN**: Rumpf zieht sich zusammen/weitet sich |
| 2 | 0x01E04 | **Gewicht[1]** | 1909 | (0, +13, \|z\|−539) | (+211…+718, −442…−1092, \|z\|−874…−888) | (+740, −1661, \|z\|−1108) | **AUFBÄUMEN**: Masse schmälert und hebt sich |
| 3 | 0x02CE8 | **Gewicht[2]** | 5213 | (−15, **+1850**, \|z\|−290) | (+58…+717, +1297…+694) | (**+1990**, +189) | **ZERFLIESSEN**: Kuppe sackt 1,85 m ab, Sohle schiebt 2 m vor |
| 4 | 0x03BCC | **Gewicht[3]** | 2191 | (+14, +1) | (**+580…+1003**, ±60) | (+323, −68) | **VORSTOSS**: Rumpfmitte schnellt nach vorn (+X) |

Ergebnis-Bounding-Boxen bei den tatsächlich gefahrenen Gewichten (Algorithmus aus §1.3
integergenau nachgerechnet):

| Zustand | Gewichte | Ergebnis-BBox | max. Vertex-Versatz |
|---|---|---|---|
| Ruhe | 0,0,0,0 | x −3600…2694, y −4224…24, z ±3648 | 0 |
| Puls oben | w0=8000 | x −3600…2693, y −4223…23 | **821** |
| Puls unten | w0=−548 | — | 55 |
| Biss ausholen | w0=−4048 | — | 414 |
| Biss zuschnappen | w3=5573 | **x −3600…4250** | **2980** |
| Intro-Hebung | w1=2895 | **y −4223…−1326, z ±2357** | 1350 |
| Tod-Rampe Ende | w2=4096 | **x −3600…7625, y −2189…16** | **5212** |
| Todes-Blob Spitze | w0=12000 | — | 1231 |
| Devour-Spitze | w0=10000 | — | 1026 |

⇒ Die Masse bewegt sich zwischen **0,8 m (Atmen)** und **5,2 m (Todes-Zerfliessen)**.
Genau das ist die fehlende „Animation" des Bosses.

---

## 3. WIE DIE GEWICHTE GESETZT WERDEN — Bestätigung + Wertebereiche

### 3.1 Zuordnung (birkin-g5-ki.md §4 — **BESTÄTIGT**)

| Port `gewicht[i]` | Adresse | Segment | Beleg |
|---|---|---|---|
| `gewicht[0]` | ctrl+12 (EMD 0x20) | Segment **1** | `8004c0c0/c4/c8` mit t3=1 → `a3+4+8` |
| `gewicht[1]` | ctrl+16 (EMD 0x24) | Segment **2** | t3=2 → `a3+8+8` |
| `gewicht[2]` | ctrl+20 (EMD 0x28) | Segment **3** | t3=3 |
| `gewicht[3]` | ctrl+24 (EMD 0x2C) | Segment **4** | t3=4 |

Gegenprobe im Overlay: `sh zero,12(v0)` / `sh zero,16(v0)` @0x80100754/0x80100764 (Ctor),
`sh zero,12/16/20/24(v0)` @0x80101180-ac ([T0]), `lh v0,12(v0)` @0x80101d0c (Devour-Gate),
`lh a0,20(v0)` @0x80103740 (Todes-Rampe), `sh a0,16(v1)` @0x80101368 ([T2]).
**Alle Gewichte sind 16-Bit** (`sh`/`lh`/`lhu`) — im Port als `int16_t` führen.

### 3.2 Alle Treiber, mit Adresse und Wertebereich

| Gewicht | Wer | Adresse | Rechnung | Bereich |
|---|---|---|---|---|
| [0] | Blob **Puls** ph1 | 0x80103cac-d0c | `w0 += speed`; `speed += 6` bis 162 (Latch +0x21B) | bis > 8000 |
| [0] | Blob **Puls** ph1-Ausklang | 0x80103d10-50 | ab `w0 ≥ 8001`: `speed -= 6`; `speed < 0` → ph2, `speed = 8` | — |
| [0] | Blob **Puls** ph2 | 0x80103d54-b4 | `w0 -= speed`; `speed += 8` bis 192 | bis < −548 |
| [0] | Blob **Puls** ph2-Ausklang | 0x80103db8-dfc | ab `w0 < −548`: `speed -= 8`; `< 0` → ph1, `speed = 6` | **[−548, 8000+]** |
| [0] | **Biss** ph1 (Ausholen) | 0x80103f78-8c | `w0 -= speed(+0x21C)`; `speed += 32` @0x80103fd4 | bis **< −4048** @0x80103fa0 |
| [0] | **Biss** ph2 (Zuschnappen) | 0x80103fe0-104010 | `w0 += speed`; danach `if (w0 > 0) w0 = 0` (`blez` @0x80104008) | **[−4048, 0]** |
| [0] | **Devour** ph1 | 0x80104904-4c | `w0 -= speed`; `speed += 32`; `speed = 1024` solange w3≠0 @0x801048fc | bis < −4048 |
| [0] | **Devour** ph2 | 0x80104950-80 | `w0 += speed` (kein weiterer Rampenschritt!) | bis **≥ 10001** |
| [0] | **Todes-Blob** ph1 | 0x80104c18-60 | `w0 -= speed`; `speed += 32` @0x80104c58 | bis < −4048 @0x80104c40 |
| [0] | **Todes-Blob** ph2 | 0x80104c64-ca8 | `w0 += 32 ± 4·(rng&0xFF)`, Vorzeichen aus `(+0x21E&1)` | Klemme **[−5000, 8000]** @0x80104cbc/cdc, 301 Ticks @0x80104d04 |
| [0] | **Todes-Blob** ph3 | 0x80104dc8-f4 | `w0 += 64` | bis **≥ 12001** |
| [0] | **Todes-Blob** ph4 | 0x80104e04-34 | `w0 -= 128` | bis `< 0` → ph5 (Ende) |
| [0] | **Vorkampf** ph1/ph2 | 0x80104564-6dc | wie Puls, Schwellen **8501** @0x801045f8 / **−1048** @0x801046a8 | — |
| [0] | **Vorkampf** ph7 (Welle) | 0x801047d8-828 | `w0 = 8000 − 24·tab[i]`, `tab` = 110 s16 @**0x80105AD4**, `i` = +0x220, Loop | **[−4288, 9680]** |
| [1] | Intro **[T2]** | 0x8010132c-68 | `w1 = ((part0.bind_y − *(s16*)0x80105BB0) · 4415) >>u 11` | 0…~2895 (Clip 1: root −4536…−3191) |
| [1] | Intro **[T6]** | 0x80101570-b4 | `w1 = ((diff · 4415) · 2) >> 12` — **wertidentisch** | dito (Clip 4) |
| [1] | Intro **[T7]** | 0x801015b8-c8 | `w1 = 0` | 0 |
| [2] | **Todes-Rampe [T20]** | 0x80103730-90 | `w2 += 16/12/8/4` (Schwellen 1025/2049/3073 @0x80103748/5c/64) | **0 → 4097** |
| [3] | Puls / Biss ph1 / Devour / Tod | 0x80103c8c-a8, 0x80103f58-74, 0x801048dc-f8, 0x80104bf8-c14 | `w3 = (s16)w3 >> 1` **je Frame** (arithmetisch, `sll 16 / sra 17`) | Abklingen |
| [3] | **Biss** ph1 | 0x80103fb0-c8 | `w3 += (s16)speed >> 1` | — |
| [3] | **Biss** ph2 | 0x80104014-28 | `w3 += speed` | bis **≥ 5573** @0x8010403c |
| [3] | **Biss** ph3 (15 Ticks) | 0x80104204-24 | `w3 += 64 − 256·(+0x21E & 1)` | ±Nachwackeln |
| [3] | **Biss** ph4 | 0x80104228-74 | `w3 -= 64`, zusätzlich `−32` bei Trefferbereich (0x800157d4, r=3000) | bis `< 0` → Blob 0 |
| — | Ctor / [T0] | 0x80100754/64, 0x80101180-ac | alle 4 = 0 | — |
| — | Devour-Ende-Gate | 0x80101d04-24 | `if ((s16)w0 < 6000)` → Blob 0 | Leser |

### 3.3 Abweichungen des Ports (`enemy_ai_boss_g5.c`), die mit dem Morph SICHTBAR werden

Solange der Morph nicht gezeichnet wurde, waren die folgenden Punkte unsichtbar. Sie
gehören VOR bzw. MIT dem Morph korrigiert, sonst atmet die Masse falsch:

1. **Puls-Ausklang fehlt** (`:226/:230`): der Port springt bei `>8000` / `<−548` sofort in die
   nächste Phase; das Original bremst erst die Rampe aus (`speed -= 6` / `-= 8`) und wechselt
   erst bei negativer Geschwindigkeit (@0x80103d2c-48 / @0x80103dd4-f0). Der Port erreicht
   dadurch nie die Amplitude ~8000+; die Masse atmet zu flach und zu hart.
2. **ph0 fällt im Original in ph1 durch** (kein Sprung hinter 0x80103c88) — der Port verliert
   einen Puls-Frame je Zyklus.
3. **Rücksprung ph2→ph1, nicht ph2→ph0** (`sb v0,537(s0)` mit v0=1 @0x80103df0): der Port
   würfelt den 180+rng-Timer jeden Zyklus neu; das Original nur einmal.
4. **`gewicht[3] /= 2` → `>> 1`**: C-Division rundet gegen 0, das Original arithmetisch
   gegen −∞ (`sll 16 / sra 17` @0x80103c9c-a8); außerdem halbiert der Port in **allen**
   Puls-Phasen, das Original nur im ph0/ph1-Pfad (ph2 springt nach 0x80103d54).
5. **Biss ph1 Reihenfolge** (`:251-253`): Original `w3 = (w3>>1) + (speed>>1)`, **dann**
   `speed += 32` (@0x80103f58-fdc); der Port erhöht zuerst.
6. **Biss ph3 Vorzeichen invertiert** (`:286`): Original `+64` bei geradem Zähler,
   `−192` bei ungeradem (`(+0x21E&1)<<8` @0x8010420c-1c) — der Port genau andersherum.
7. **Devour ph1/ph2** (`:316-320`): Original `speed += 32` (nicht 256) und ph2 addiert die
   LAUFENDE Geschwindigkeit statt konstant 512; Schwelle 10001 statt 10000.
8. **Todes-Blob ph1** (`:331`): Original `w0 += 32 ± 4·(rng&0xFF)` (Amplitude bis ±1020),
   der Port `(rng&0x3F) − 32` (±32) — das Zucken der Leiche ist ~30× zu schwach.
   Klemmen (−5000/8000) und 301 Ticks stimmen.
9. **`gewicht[1]` wird nie gefüllt**: die Intro-Kopplung [T2]/[T6] fehlt komplett
   (`g5_intro_tick` case 2/6). Genau sie lässt die Masse beim Heranrobben aufgehen.
10. **Vorkampf-Zustand 3** ist im Port eine Näherung; das Original läuft nach dem
    Anschwellen in die **110-Schritt-Welle @0x80105AD4** und wiederholt sie endlos
    (`+0x219 = 6` @0x8010482c → [U6] setzt 7 @0x801047d4). **Korrektur zu
    birkin-g5-ki.md §4:** die Konvulsions-Zyklen [U3]/[U4]/[U5] (−2136/+712) sind **TOTER
    CODE** — ein vollständiger Scan aller `sb …,0x219(…)`-Schreiber im Overlay findet
    **keinen** Setzer von Phase 3 (vorkommende Werte: 0,1,2,4,5,6,7). Ebenso bleibt
    Blob-Zustand 2 (0x80104440) unerreichbar (+0x218-Setzer: 0,1,3,4,5).

Wertetabelle der Welle @0x80105AD4 (110 s16, endet exakt am Overlay-Dateiende 0x5BB0):
`−15,−30,−50,−70,−60,−30,0,31,64,99,136,175,215,257,301,341,384,414,440,460,467,473,478,
482,478,490,483,497,488,503,492,506,493,508,493,509,494,511,496,512,497,511,497,511,496,
510,495,510,495,509,494,508,493,507,492,506,491,505,490,504,489,503,488,502,487,500,485,
473,461,449,437,425,410,400,390,380,370,358,344,330,315,300,285,270,256,240,222,202,183,
168,150,135,110,95,80,65,50,38,20,0` (danach 0er bis 110).

---

## 4. UMSETZUNGS-SPEZIFIKATION FÜR DEN PORT

### 4.1 Bank-Loader — `re15_port/engine/src/re2_ems.c` / `include/re15_enemy.h`

Neue Struktur (platform-agnostisch, keine Allokation — zeigt in den residenten EMS-Blob;
`eb->buf == NULL`, `pc_enemy_load_re2` in `platform/pc/main.c:538-544` hält das Archiv):

```c
/* dir[0]-Morph-Controller EINES Meshes (Format: g5-morph.md 2.2,
 * Konsument FUN_8004BF90 @0x8004bf90). */
typedef struct {
    const uint8_t *seg0;      /* Block + data_off  (ctrl+0x00 @8004bfac) — Basis-Pose  */
    uint32_t       seg_size;  /* ctrl+0x04 @8004c0ac — Bytes je Segmenttabelle          */
    uint16_t       seg_count; /* ctrl+0x08 @8004c0b0 — Zahl der ZIEL-Segmente           */
    uint16_t       nverts;    /* = md1.meshes[m].tri_vertex_count (obj[1] @8004bfa0)    */
} re15_morph_ctrl_t;

/* in re15_enemy_bank_t ergaenzen: */
    const uint8_t    *morph_block;                  /* EMD + dir[0]  (@0x8001aba0)      */
    uint32_t          morph_mask;                   /* block[0]      (FUN_8004b3b8.c:7) */
    re15_morph_ctrl_t morph[MD1_MAX_MESHES];        /* nur wo Maskenbit gesetzt          */
    uint8_t           morph_ok;
```

Parser in `re2_emd_parse_bank` direkt hinter dem MD1-Parse (`re2_ems.c:102-105`), exakt die
Schleife aus `FUN_8004b3b8.c:10-24`:

```c
if (D[0] && (size_t)D[0] + 12 <= emd_size) {
    const uint8_t *blk = emd + D[0];                  /* entity+0x1BC @0x8001aba0        */
    uint32_t mask = rd_u32(blk + 0);
    const uint8_t *rec = blk + 12;                    /* param_2 + 3 Worte               */
    for (int m = 0; m < eb->md1.mesh_count; m++) {
        eb->morph[m].seg_count = 0;
        if (!(mask & (1u << m))) continue;            /* Bit nicht gesetzt -> kein Morph */
        uint32_t doff = rd_u32(rec + 0), ssz = rd_u32(rec + 4), scnt = rd_u32(rec + 8);
        eb->morph[m].seg0      = blk + doff;
        eb->morph[m].seg_size  = ssz;
        eb->morph[m].seg_count = (uint16_t)(scnt & 0xFFFFu);
        eb->morph[m].nverts    = (uint16_t)eb->md1.meshes[m].tri_vertex_count;
        rec += (3 + scnt) * 4;                        /* puVar2 + puVar2[2] + 3          */
    }
    eb->morph_block = blk; eb->morph_mask = mask; eb->morph_ok = 1;
}
```
Plausibilitätsklammer (nicht im Original, Port-Sicherheit): `seg0 + (scnt+1)*ssz <= emd_ende`
und `ssz >= nverts*6`. EM036 muss dabei liefern: `mask 0x4`, `seg0 = emd+0x3C`,
`seg_size 0xEE4`, `seg_count 4`, `nverts 635` — das ist der ctest-Pin (§4.5).

### 4.2 Morph-Kern — neue `re15_port/engine/src/emd_morph.c`

```c
/* FUN_8004BF90 @0x8004bf90 (RE2-Leon PSX.EXE, Datei 0x3C790) — byte-true.
 * out MUSS nverts Eintraege fassen; die MD1-Basis wird NIE beschrieben
 * (der Port aliast das residente CDEMD0.EMS, re2_ems.c `eb->buf = NULL`). */
void re15_emd_morph_apply(const re15_morph_ctrl_t *c, const int16_t *w,
                          re15_md1_vertex_t *out)
{
    const int16_t *s0 = (const int16_t *)c->seg0;
    for (int v = 0; v < c->nverts; v++) {            /* Basis-Restore @0x8004bfc4-c0a4 */
        out[v].x = s0[v*3+0]; out[v].y = s0[v*3+1]; out[v].z = s0[v*3+2];
    }                                                 /* pad bleibt unberuehrt          */
    for (int i = 1; i <= (int)c->seg_count; i++) {    /* t3 = 1..seg_count              */
        int32_t wi = (int16_t)w[i-1];                 /* lhu + MTC2 -> 16 Bit           */
        if ((uint16_t)wi == 0) continue;              /* beq a0,zero @0x8004c0d0        */
        int32_t a = (int16_t)(4095 - wi);             /* subu t8,a0 + MTC2 @0x8004c0d4  */
        const int16_t *t = (const int16_t *)(c->seg0 + (size_t)i * c->seg_size);
        for (int v = 0; v < c->nverts; v++) {
            int32_t r;
            r = (a*(int32_t)out[v].x + wi*(int32_t)t[v*3+0]) >> 12; out[v].x = CLAMP_S16(r);
            r = (a*(int32_t)out[v].y + wi*(int32_t)t[v*3+1]) >> 12; out[v].y = CLAMP_S16(r);
            r = (a*(int32_t)out[v].z + wi*(int32_t)t[v*3+2]) >> 12; out[v].z = CLAMP_S16(r);
        }
    }
}
```
`>> 12` **arithmetisch** (GPF/GPL sf=1 ist SAR), `CLAMP_S16` = GTE-IR-Sättigung (lm=0).
PSX-Target: dieselbe Funktion, aber die Innenschleife 1:1 mit `gte_ldIR0/gte_ldsv/
gte_gpf12/gte_gpl12/gte_stsv` — dort kostet sie dasselbe wie im Original.
**Laufzeit PC:** 635 Vertices × 3 × (aktive Segmente). In der Praxis ist **genau ein**
Gewicht ≠ 0 (Puls/Biss/Tod fahren nacheinander), also ~1900 Multiply-Adds/Frame — die
`w == 0`-Klammer des Originals ist der Budget-Trick, sie darf nicht wegfallen.

### 4.3 Das G5-Modul reicht die Gewichte weiter — `enemy_ai_boss_g5.c`

Muster wie `re15_re2z_gore_part_burst` (`re15_actor.h:1050`): eine Abfrage, die der Zeichner
pro Mesh stellt. Der Morph wird am ENDE von `re15_g5_boss_tick` (nach `g5_blob_tick`,
`:668`) berechnet — genau die Reihenfolge des Originals (Blob-Maschine @0x80103bc8, dann
`jal 0x8004bf90` @0x80103bf8):

```c
/* re15_actor.h */
const re15_md1_vertex_t *re15_g5_morph_verts(const re15_actor_t *e, int mesh_index);

/* enemy_ai_boss_g5.c — Modul-Zustand */
static re15_md1_vertex_t s_g5_morph[RE15_G5_MORPH_MAX_V];  /* 635 -> 1024 reicht */
static int s_g5_morph_mesh = -1, s_g5_morph_gueltig = 0;

/* am Ende von re15_g5_boss_tick(): */
{   re15_enemy_bank_t *eb = re15_enemy_find(0x36);
    s_g5_morph_gueltig = 0;
    if (eb && eb->morph_ok) {
        for (int m = 0; m < eb->md1.mesh_count; m++) {
            if (!eb->morph[m].seg_count) continue;
            if (eb->morph[m].nverts > RE15_G5_MORPH_MAX_V) continue;
            int16_t w[4] = { (int16_t)g->gewicht[0], (int16_t)g->gewicht[1],
                             (int16_t)g->gewicht[2], (int16_t)g->gewicht[3] };
            re15_emd_morph_apply(&eb->morph[m], w, s_g5_morph);
            s_g5_morph_mesh = m; s_g5_morph_gueltig = 1;
            break;                     /* EM036: Maske 0x4 -> genau ein Mesh */
        }
    }
}
```
`(int16_t)`-Kappung ist **Pflicht** (das Original führt die Gewichte als `sh`/`lh`, §3.1) —
`gewicht[]` als `int32_t` passt bei w0 = 12000 zwar noch, aber die Kappung dokumentiert die
Semantik und schützt gegen künftige Überläufe.

### 4.4 Zeichner — `re15_port/platform/pc/main.c`

Eine Ergänzung hinter `main.c:8475` (`const re15_md1_mesh_t *nm = &src_md1->meshes[nmi];`):

```c
/* G5-BLOB (Typ 0x36): dir[0]-Vertex-Morph, Konsument FUN_8004BF90 @0x8004bf90.
 * Das MD1 aliast das residente CDEMD0.EMS -> die Basis wird NIE ueberschrieben,
 * der Zeichner liest den Morph-Puffer des Moduls. Tri- und Quad-Array sind
 * dasselbe Array (EMD 0x118FC fuer beide) -> ein Zeiger genuegt. */
const re15_md1_vertex_t *nmv = nm->tri_vertices;
const re15_md1_vertex_t *nmq = nm->quad_vertices;
if (npc->type == 0x36) {
    const re15_md1_vertex_t *mv = re15_g5_morph_verts(npc, nmi);
    if (mv) { nmv = mv; nmq = mv; }
}
```
und dann in den beiden `vp[]`-Initialisierern **nur die Quelle tauschen**:
* `main.c:8497-8501` → `&nmv[tri->v0]`, `&nmv[tri->v1]`, `&nmv[tri->v2]`
* `main.c:8576-8581` → `&nmq[qd->v0]` … `&nmq[qd->v3]`

Der Rest (Burst-Offset, Bone-Matrix, Projektion, Normalen/Beleuchtung) bleibt unberührt —
das Original morpht ebenfalls nur die Vertices und lässt die Normalen stehen (§1.3).
`re15_md1_vertex_t` ist bereits SVECTOR-kompatibel (`int16_t x,y,z,pad`, `re15_md1.h:43-45`),
und die Index-Klammern (`tri->v0 >= nm->tri_vertex_count`) bleiben gültig, weil der
Morph-Puffer dieselbe Länge hat.
**Achtung Reihenfolge:** die Part-Kette für `nbi == 2` (Blob) hängt an der Wurzelpose mit
Bind (1800,4500,0) (`main.c:8330-8345`) — sie ist vom Morph unabhängig; eine Bind-Morphung
gibt es für Part 2 auch im Original nicht (§1.4).

### 4.5 Verifikation (ctest — in dieser Runde NICHT gebaut, reine Spez)

1. **Format-Pin** (`tests/…re2_ems…`): EM036 aus `shared_assets/RE2/CDEMD0.EMS` laden und
   `morph_mask == 0x4`, `morph[2].seg_count == 4`, `seg_size == 0xEE4`, `nverts == 635`,
   `seg0 == emd + 0x3C` prüfen; zusätzlich Vertex-für-Vertex-Gleichheit von Segment 0 mit
   `md1.meshes[2].tri_vertices` (635/635, §2.2).
2. **Morph-Pin**: `w = {0,0,0,0}` → Ergebnis identisch zur Basis; `w = {8000,0,0,0}` →
   max |Δ| == **821**; `w = {0,0,4096,0}` → BBox `x ≤ 7625`, `y ≥ −2189`, max |Δ| == **5212**;
   `w = {0,2895,0,0}` → |z| ≤ **2357**. (Alle vier Zahlen stammen aus der integergenauen
   Nachrechnung in §2.3 und sind damit Regressionsanker.)
3. **Sichtprüfung** nach Skill `re15-port-visual-verify` (gdigrab, echtes Fenster) —
   die Masse muss im Leerlauf sichtbar atmen, beim Biss nach vorn schnellen und im Tod
   zerfliessen.

---

## 5. OFFEN

* **EM037-Tentakel**: deren EMD (`CDEMD0.EMS` Sektor 0xE2D, 0xE7F0 B) ist nicht geprüft —
  ob auch sie `dir[0]` tragen, ist ungeklärt (für ROOM5090 ohne Belang, dort keine 0x37).
* **Bind-Translations-Morph** (`FUN_8004baf0`, Block+0x04/+0x08, Bind-Segment-Index
  ctrl+0x0E+4i): für EM036 nachweislich tot; welches RE2-Modell ihn benutzt, ist offen.
* **`+0x21B`-Doppelbelegung**: im Puls/Vorkampf ist es der Geschwindigkeits-Kappen-Latch
  (@0x80103cdc/@0x80103d04), im Biss die Blutserie (@0x801041a8). Zwei Bedeutungen, ein
  Byte — vor einer Portierung des Blut-Tails auseinanderhalten.
* **Augen-Wanderer** (0x80105064/0x8010503c, Byteströme @0x801055EC…): rein dekorativ,
  nicht Teil dieses Zensus.
* Wer `+0x219 = 3` im Vorkampf setzen SOLLTE (die Konvulsions-Zyklen [U3]-[U5]) — kein
  Schreiber im Overlay; entweder Restcode oder ein EXE-seitiger Setzer, den der Scan
  (nur Overlay) nicht sieht.

---

## FAZIT

Der fehlende Baustein ist gefunden und vollständig spezifiziert: `dir[0]` des EM036-EMD ist
ein **Vertex-Morph-Block**, kein Leerraum. Er liegt bei `CDEMD0.EMS` Datei-Offset 0x6E6800
(EMD-relativ 0x08), trägt die Mesh-Maske `0x4` (nur Mesh 2 = die Fleischmasse) und einen
Controller `{data_off 0x34, seg_size 0xEE4, seg_count 4}` mit vier u16-Gewichten bei
EMD 0x20/0x24/0x28/0x2C. Ab EMD 0x3C folgen **fünf** Vertextabellen zu je 635×3 s16 —
Tabelle 0 ist **bytegleich** mit dem MD1-Vertexarray des Meshes (der Beweis, dass sie die
Basis-Pose ist), die Tabellen 1…4 sind absolute Zielposen. Das Ganze endet lückenlos auf
dir[1] = 0x4AB0.

Der Konsument ist `FUN_8004BF90` @0x8004bf90 — in keinem Decompile-Satz des Repos
enthalten, hier erstmals roh disassembliert (Datei 0x3C790…0x3C994). Er wird genau einmal
pro Frame aufgerufen (`80103bf8: jal 0x8004bf90`, Argumente entity / &part2 /
*(part2+0x08) / *(part2+0x88)) und arbeitet in zwei Schritten: erst schreibt er die
Basis-Pose komplett in den Vertexpuffer des Meshes zurück (5-fach abgerollte Kopie,
Stride 6 → 8), dann blendet er Segment für Segment mit der GTE-Paarung GPF/GPL (sf=1,
selbst aus den Rohworten 0x4b98003d / 0x4ba8003e dekodiert):
`v = ((4095−w)·v + w·ziel) >> 12`, mit s16-Sättigung, Segmenten mit `w == 0` übersprungen
und **kettenweise** — Segment 2 rechnet auf dem Ergebnis von Segment 1, nicht auf der Basis.
Normalen bleiben unangetastet. Die zweite, im Repo vorhandene Fassung `FUN_8004baf0` morpht
zusätzlich Bone-Bind-Translationen; für Part 2 ist dieser Zweig durch
`part_index < bone_count` (2 < 2) nachweislich abgeschaltet, und die Per-Frame-Kette ruft
sie ohnehin nicht auf.

Die Gewichtszuordnung aus birkin-g5-ki.md §4 ist bestätigt: ctrl+12/+16/+20/+24 =
Segment 1/2/3/4. Was die vier Ziele leisten, ist jetzt vermessen statt vermutet: Segment 1
= Atmen (max 420, bei w0=8000 auf 821 extrapoliert), Segment 2 = Aufbäumen/Schmälern
(bis 1909, im Intro an die Wurzelhebung gekoppelt), Segment 3 = Zerfliessen im Tod
(bis 5213, Kuppe sackt 1,85 m, Sohle schiebt 2 m vor, getrieben 0→4097), Segment 4 =
Vorstoss beim Biss (bis 2191, Rumpfmitte +1 m). Die Masse bewegt sich also zwischen 0,8 m
und 5,2 m — genau die „fehlende Animation", die der Nutzer meldet.

Nebenbefunde, die im Port korrigiert gehören: der `(+0x226&8)`-Gate schaltet **nicht** die
Blob-Maschine ab (er springt auf 0x801002b8, also nur über das Augen-Ziel-Neuwerfen) —
der Morph läuft auch im Tod, sonst stünde die Absink-Rampe still. Die Konvulsions-Phasen
[U3]/[U4]/[U5] des Vorkampfs sind **toter Code** (kein einziger Schreiber setzt +0x219=3;
Blob-Zustand 2 ist ebenso unerreichbar) — der Vorkampf läuft stattdessen in die endlos
wiederholte 110-Schritt-Welle @0x80105AD4 (`w0 = 8000 − 24·tab[i]`, Bereich −4288…9680).
Und die Gewichts-Simulation des Ports weicht an zehn Stellen ab (§3.3) — am gröbsten
fehlen die Puls-Ausklänge (die Amplitude bleibt zu klein), `gewicht[1]` wird nie gefüllt
(kein Aufbäumen im Intro), das Biss-Nachwackeln hat das Vorzeichen vertauscht und das
Zucken der Leiche ist um Faktor 30 zu schwach. Diese Punkte werden erst MIT dem Morph
sichtbar und sollten zusammen mit ihm eingebaut werden.

Die Umsetzung ist klein und klar geschnitten: Parser in `re2_ems.c` (elf Zeilen, exakt die
Schleife aus `FUN_8004b3b8`), Kern in einer neuen `emd_morph.c` (PC integer, PSX per GTE),
ein Puffer plus Abfragefunktion im G5-Modul nach dem Muster `re15_re2z_gore_part_burst`,
und im Zeichner ein Zeigertausch an `main.c:8475/8497/8576`. Die MD1-Basis darf dabei nie
beschrieben werden — sie aliast das residente CDEMD0.EMS. Vier Regressionsanker (821 /
5212 / 2357 / Segment-0-Gleichheit) stehen aus der integergenauen Nachrechnung bereit.
