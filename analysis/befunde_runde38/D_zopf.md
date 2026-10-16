# Runde 38 Spur D "zopf" — Elzas Zopf wie Claires Zopf in RE2

Baum `.claude/worktrees/r38_zopf` (Zweig `r38/zopf`, Basis f587d34e).

## Punkt (Wortlaut, AUFTRAG.md massgeblich)

1. "Bei Elza fehlt der Zopf - schaue in Resident Evil 2 wie es da gemacht ist mit Clairs Zopf."

## Fortschritt (fortlaufend)

- [x] Schritt 0: Pflichtlektuere (AUFTRAG, VERTRAG, UEBERGABE Runde 38; Runde 37 N_elzaems)
- [x] Schritt 1: Messen — Elzas PL04 hat den Zopf (Teile 17..20), der Port zeichnet nur 15 Teile (§1)
- [x] Schritt 2: RE — RE1.5 HAT die Zopf-Mechanik (FUN_80024c30/FUN_80024e40); RE2 Claire = dieselbe Routine (§2)
- [ ] Schritt 3: Bauen
- [ ] Schritt 4: Messung nachher
- [ ] Schritt 5: Riegel probes/r38_zopf.cmake


## 1. Messung vorher (Daten + Port-Code)

### 1.1 Mesh-Satz der Spielermodelle (MD1-Objektzahl / EMR-Knochen, eigenes Skript ueber die PLD-Verzeichnisse)

| Datei | Teile (MD1 object_count/2) | EMR-Knochen |
|---|---|---|
| RE1.5 PL00..PL03 (Leon) | 17 | 15 |
| RE1.5 **PL04..PL07 (Elza)** | **21** | 15 |
| RE2 PL00 (Leon) | 15 | 15 |
| RE2 **PL01 (Claire)** | **19** | 15 |
| RE2 PL01CH | 20 | 15 |

Elzas PL04.MD1 (Teile 15..20, Bounding-Box x/y/z, Vertices):
`15` 4 V (-54..51, 17..214) · `16` 8 V Quader (-41..41, -90..90, -25..25) · `17` 5 V (-115..2, -98..-1) ·
`18` 7 V (-153..-34, -56..140) · `19` 6 V (-62..48, 20..163) · `20` 4 V (-54..51, 17..214).
Claires PL01.MD1 Teile 15..18: 5 V (-115..43, -122..-1) · 7 V (-153..-34, -80..131) · 6 V (-62..48, 20..163) ·
4 V (-54..50, 20..176) — **Elzas Teile 17..20 sind dieselben vier Zopfglieder** (Teil 19 = Claire 17, gleiche Box).
PL04.EMR und RE2 PL01.EMR haben dieselbe Knochenkette (Kopf = Knochen 8, Elter = Knochen 0, rel (-21,-623,0)).

### 1.2 Port: was gezeichnet wird
`platform/pc/main.c` Spieler-Zeichenschleife: `int n_bones = (pose_ok && player_visible) ? skel.bone_count : 0;`
`for (bi = 0; bi < n_bones; bi++) { int mi = bi; ... }` — es werden genau `skel.bone_count` = **15** Teile
gezeichnet. Elzas Teile 15..20 (darunter der ganze Zopf 17..20) werden nie gezeichnet und nie bewegt. Kein
Port-Code fuehrt 0x80024c30/0x80024e40 aus (grep: nur Kommentare). => "Bei Elza fehlt der Zopf" ist ein
PORT-Defekt: Netz vorhanden, Zeichnen + Pendel fehlen.

## 2. RE-Belege

### 2.1 RE1.5 hat den Zopf (Original-Code, info/Re1.5/PSX.EXE, re15_disasm.py)

**Teile-Anzahl** FUN_8001e56c: `entity+0x83 = *(MD1+8) >> 1` (= 21 bei Elza), Teile-Feld `entity+0x188`, Stride
0xac. Zeichner FUN_8001e8c8 zeichnet ALLE `+0x83` Teile (`FUN_8001e9ec(part); part += 0xac`, jal @0x8001e990).
FUN_8001e5b0 setzt jedes Teil auf Flag 1 und haengt Teile jenseits der EMR-Knochen an die Einheitsmatrix
DAT_80072d4c (= u32 [0x1000,0,0x1000,0,0x1000,0,0,0], selbst gelesen). FUN_8001e9ec:
`if ((*part & 0x40) == 0) FUN_80022da0(part[0x1b], part+0x18, part+0x40)` — mit Flag 0x40 bleibt die
Weltmatrix part+0x40 unangetastet (sie kommt dann von der Zopf-Routine).

**Init (Spieler-Kommando 0, Tabelle @0x80073f90[0] = 0x800318f8):**

    800319d8 lbu v0,0x800aca5c ; 800319e0 sltiu v0,v0,0x2 / 800319f8 bne v1,2  -> Leon-Modelle 0,1,2
        (Teil 15 an Teil 2, Versatz (-9,0x66,-0x94) @0x80031a30-48, Teil 16 Flag 0 @0x80031a74)
    80031a88 addiu v0,v0,-4 ; 80031a8c sltiu v0,v0,2 ; 80031aa0 bne v1,6 -> Elza-Modelle 4,5,6:
    80031abc sw zero,0xa14(v1)   Teil 15 Flag 0 (aus)        80031b30 sw zero,0xac0(v0)   Teil 16 Flag 0 (aus)
    80031b38 lw v1,0x800acbdc    v1 = Teile-Feld (Spieler 0x800aca54+0x188)
    Teil 17 (+0xb6c): 80031b44 li -0x73 -> +0xb98 ; 80031b4c li -0x15a -> +0xb9c ; 80031b88 sw zero,+0xba0
    Teil 18 (+0xc18): 80031b54 li -0x2d -> +0xc44 ; 80031b5c li -0x20  -> +0xc48 ; 80031b98 sw zero,+0xc4c
    Teil 19 (+0xcc4): 80031b64 li -0x65 -> +0xcf0 ; 80031b6c ori 0x61  -> +0xcf4 ; 80031ba8 sw zero,+0xcf8
    Teil 20 (+0xd70): 80031b74 li -0x1  -> +0xd9c ; 80031b7c ori 0x6a  -> +0xda0 ; 80031bb8 sw zero,+0xda4
    je Teil sh zero +0x94/+0x96 (Winkelgeschwindigkeiten) und +0x62 (rot.vy):
      0xc00/0xc02/0xbce, 0xcac/0xcae/0xc7a, 0xd58/0xd5a/0xd26, 0xe04/0xe06/0xdd2 (@0x80031b8c-80031be0)
    80031bc4/bd0/bd8/bec ori 0x40 + sw  -> Teile 17..20 Flag |= 0x40

Lokaler Versatz = part+0x2c/+0x30/+0x34 (= t der lokalen Matrix part+0x18). Dieser Init laeuft bei jedem
Raumwechsel (Transitions-FSM `sb zero,0x800aca58` @0x8001cbdc -> Kommando 0) und nach dem PLD-Laden
(FUN_800314b0 `sw zero,0x800aca58` @0x80031518). Die alte Lage (part+0x54..0x5c) setzt er NICHT zurueck.

**Pro Bild (Spieler-Dispatcher FUN_80031c44, nur ohne Spieler-Freeze `bltz` @0x80031c78):**

    80031d78 jal 0x80037358          Nacken-FSM
    80031d84 lbu v0,0x800aca5c ; 80031d8c andi v0,v0,0x4 ; 80031d90 beq v0,zero,0x80031da0
    80031d98 jal 0x80024c30          ZOPF (nur Elza-Modelle, Bit 2)

**FUN_80024c30 (Treiber, ganz gelesen @0x80024c30-80024e3c):** `RotMatrix(ent+0x68, ent+0x20)` @0x80024c68;
Kopf-Weltmatrix `FUN_80022da0(ent+0x20, Teil0+0x18, tmp)` @0x80024c88, `FUN_80022da0(tmp, Teil8+0x18, Teil8+0x40)`
@0x80024c9c; Winkel `t0 = ent.rot.vy(+0x6a) + Teil0.rot.vy(+0x62) + Teil8.rot.vy(+0x5c2)` (lhu, @0x80024ca4-b4);
`Teil0.rot.vz &= 0xfff` (sh @0x80024cc4); `if ((vz - 0x5dd) < 0x3e7) t0 += 0x800` (@0x80024cc8-d8);
`if ((u8 0x800acc0c & 1) == 0) t0 += Teil8+0x94` (Nacken-Gier, @0x80024ce0-d00); `s0 = (short)t0` (@0x80024d30-34).
Vier Aufrufe FUN_80024e40(Teil, Elternmatrix, p3, p4, p5..p12, s0):

| Aufruf | Teil | Eltern | p3 | p4 | p5 | p6 | p7 | p8 | p9 | p10 | p11 | p12 | Adresse |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 1 | 17 (+0xb6c) | Teil 8 +0x40 | 0x190 | 0x64 | 0x28 | 2 | 0 | 0x190 | 0x64 | 0x18 | 4 | 0 | @0x80024d04-50 |
| 2 | 18 (+0xc18) | Teil 17 +0x40 | 0x15e | 0 | 0x30 | 4 | **0x12c** | 0x96 | 0 | 0x18 | 6 | 0 | @0x80024d58-98 |
| 3 | 19 (+0xcc4) | Teil 18 +0x40 | 0x12c | 0 | 0x30 | 4 | 0 | 0x96 | 0 | 0x18 | 6 | 0 | @0x80024da0-d0 |
| 4 | 20 (+0xd70) | Teil 19 +0x40 | 0xfa | 0 | 0x30 | 4 | 0 | 0x64 | 0 | 0x18 | 6 | 0 | @0x80024dd8-e08 |

**FUN_80024e40 (ein Glied, ganz gelesen @0x80024e40-8002535c):**
1. `r = (rot.vx + v96, 0, rot.vz + v94)` (@0x80024e74-ee8), `RotMatrixYXZ(r, M)` @0x80024ee4, `RotMatrixY(rot.vy, M)`
   @0x80024ef0 (rot.vy = Koerperwinkel des Vorbilds).
2. GTE `MVMVA 0x4a486012` (RT*V0, sf=1, ohne TR) @0x80024f58/f88: `A = M*(0,p3,0)>>12 + alte Lage + (0,p4,0)`,
   `B = M*(0,p8,0)>>12 + alte Lage + (0,p9,0)` (@0x80024f8c-80025028). Alte Lage = part+0x54/58/5c.
3. Neue Lage: `r = (t.x<<4 | t.y<<20, t.z<<4)` aus part+0x2c/30/34 (lhu, @0x8002502c-50 — Bits 12..15 von t.x
   landen per `or` in den unteren 4 Bits von r.vy), `ApplyMatrix(Eltern, r)` @0x8002504c,
   `part+0x54.. = (out>>4) + Eltern.t` (@0x80025054-8c).
4. `M = RotMatrixY(-rot.vy, I)` (I = DAT_80072d4c, @0x80025090-e4); `r = (Lage-A)*4` gepackt (`andi 0xffff; sll 2`
   / `sll 18`, @0x800250f0-34); `ApplyMatrix` -> `zang = FUN_80025360(out.x, out.y)` @0x80025140.
5. `M = RotMatrixZ(-(rot.vz+v94), M)` @0x8002519c; `r = (Lage-B)*4`; `ApplyMatrix` ->
   `xang = FUN_800253fc(-out.z, out.y)` @0x800251c4.
6. Feder-Daempfer: `v94 = v94 - v94/p6 - (p5*rsin(zang-p7))>>12`, auf 12 Bit vorzeichenerweitert (@0x800251ec-60);
   `v96 = v96 - v96/p11 - (p10*rsin(xang-p12))>>12` (@0x80025278-f4).
7. `rot = (xang, 0, zang)` -> `RotMatrixYXZ(rot, part+0x40)` @0x80025308; `rot.vy = s0`; `RotMatrixY(s0, part+0x40)`
   @0x80025324. Weltmatrix des Glieds = Ry(s0) * RYXZ(xang,0,zang), t = Lage aus 3.

Winkelhelfer: FUN_80025360(x,y) = `y==0 ? (x>=0 ? 0x1400 : 0xc00) : catan((x<<12)/-y) (+0x800 wenn y>0)`
(@0x80025360-800253f8); FUN_800253fc(x,y) = `y==0 ? (x>=0 ? 0xc00 : 0x400) : catan((x<<12)/-y)`, negiert wenn y>0
(@0x800253fc-8002549c). Bibliothek (jal-Kodierungen in ghidra1_V2.txt aufgeloest): RotMatrix 0x80068098,
RotMatrixYXZ 0x800666d0, RotMatrixY 0x800659d0, RotMatrixZ 0x80065b70, ApplyMatrix 0x800661c0, rsin 0x800683e8,
catan 0x800658fc. RotMatrixY/Z/YXZ selbst disassembliert: Tabelle DAT_800794c4 mit `& 0xfff`, NEGATIVE Winkel
ueber einen eigenen Zweig (`subu t7,zero,t7` + Vorzeichenwechsel des Sinus, z.B. @0x800659e0-0x80065a0c). Die
Tabelle ist NICHT antisymmetrisch (Eintrag 4095 = sin 0, Eintraege 2047/2048 = sin 0; selbst gelesen) — der Zweig
ist also beobachtbar und wird nachgebaut. RotMatrixY = Ry*M, RotMatrixZ = Rz*M (Vormultiplikation).

### 2.2 RE2 (Claire) = DIESELBE Routine (info/re2leon/PSX.EXE, re2_disasm.py)
- Treiber FUN_8001844c, Aufrufer @0x8003bf10: `lbu v1,8(s0)` (Spieler-Id), `sltiu v0,v1,0xa`, `andi v0,v1,1` ->
  nur Claire-Ids (ungerade < 10). Glied FUN_80018728 mit IDENTISCHEM Algorithmus (MVMVA 0x4a486012 @0x80018838,
  Packung `sll 4 / sll 20` @0x800189a4-c0, Feder-Daempfer).
- Aufrufe (@0x8001850c-80018614): (400,100,0x28,2,0,400,100,0x18,4,0), (0x15e,0,0x30,4,**0x96**,0x96,0,0x18,6,0),
  (300,0,0x30,4,0,0x96,...), (0xfa,0,0x30,4,0,100,...) — gleich bis auf p7 des 2. Glieds (RE1.5 0x12c).
- Zopf-Teile 15..18 (Claire-MD1), Versaetze (@0x8003c45c-8003c49c): (-0x28,-0x170), (-0x30,-0x20), (-0x5b,0x61),
  (-1,0x7f), Flags |= 0x41 (@0x8003c4e8-50c) — auf Claires Netz abgestimmt (RE1.5: Elzas Netz, s.o.).
- RE2 laesst die `+0x800`-Regel (Teil0.rot.vz in [0x5dd,0x9c4)) weg (@0x800184d4-e8 nur `andi 0xfff; sh`).
- RE2-Zusatzblock @0x800188b4-80018954 (Kopfmatrix, FUN_8002d030 nach sp+136) wird danach NIE gelesen (kein Zugriff
  auf sp+136..159 bis `jr ra` @0x80018cc0) = toter Code.

**Folgerung:** Elzas Zopf ist im Original RE1.5 fertig vorhanden (Netz + Init + Pendel); RE2 hat ihn fuer Claire
mit derselben Routine uebernommen. Der Port baut die RE1.5-Routine byte-genau nach (Beta -> Retail greift nicht,
RE1.5 ist hier nicht unfertig); RE2 ist der Gegenbeleg fuer den Mechanismus.

### 2.3 Nebenbefunde (NICHT Teil dieser Spur, fuer OFFEN)
- Leon (Modelle 0,1,2): Teil 15 (6 V, (-22..34, -256..0)) bleibt im Original an (Flag 1) und haengt an Teil 2 mit
  Versatz (-9,0x66,-0x94) (@0x80031a00-74); der Port zeichnet es nicht. Leon bleibt in dieser Spur unveraendert.
- Port-RotMatrix (skeleton_common.c mat3_from_euler) liest negative Winkel ueber `& 0xfff`; das Original
  RotMatrix @0x80068098 hat einen eigenen Negativ-Zweig (Decompilat RotMatrix.c: `if (r->vx < 0) FGO_01_OBJ_64()`).
  Wegen der nicht antisymmetrischen Tabelle weicht das um 1 Tabellenschritt ab. Betrifft alle Knochen, nicht der Zopf.
