# Runde 38 Spur D "zopf" — Elzas Zopf wie Claires Zopf in RE2

Baum `.claude/worktrees/r38_zopf` (Zweig `r38/zopf`, Basis f587d34e).

## Punkt (Wortlaut, AUFTRAG.md massgeblich)

1. "Bei Elza fehlt der Zopf - schaue in Resident Evil 2 wie es da gemacht ist mit Clairs Zopf."

## Fortschritt (fortlaufend)

- [x] Schritt 0: Pflichtlektuere (AUFTRAG, VERTRAG, UEBERGABE Runde 38; Runde 37 N_elzaems)
- [x] Schritt 1: Messen — Elzas PL04 hat den Zopf (Teile 17..20), der Port zeichnet nur 15 Teile (§1)
- [x] Schritt 2: RE — RE1.5 HAT die Zopf-Mechanik (FUN_80024c30/FUN_80024e40); RE2 Claire = dieselbe Routine (§2)
- [x] Schritt 3: Bauen (Vorgaenger 6f162af1/03dd8cbc; Fortsetzung 2026-10-17 geprueft, §3)
- [x] Schritt 4a: Messung nachher an der exe — Drehung (Winkelverlauf), Pixel am Hinterkopf, Leon bitgleich zur
      Basis-exe, Elza ohne Zopfglieder bitgleich zur Basis-exe (§4.1-4.3)
- [x] Schritt 4b: Gehen/Rennen an der exe gegen das ORIGINAL (§4.4), Huelle der Spur P (§4.5) — Fortsetzung 3
      (2026-10-17 09:00-10:00, cd9d9891 + Abschluss)
- [x] Schritt 4c: Gegenprobe am ORIGINAL (DuckStation-GDB, Elza): Teile 17..20 vor/nach FUN_80024c30 aus der RAM,
      Port-Schritt darauf nachgerechnet, 235/235 Bilder bitgleich (§4.6; 65dd6d59, 90252339)
- [x] Schritt 5: Riegel probes/r38_zopf.cmake (§5)
- [x] Schritt 6: OFFEN und "Fuer den Nutzer" (§6, §7)

Fortsetzung 2026-10-17 (Vorgaenger am 2026-10-16 ~23:45 vom Sitzungslimit beendet): Endstand 5782ec05 gebaut
(`ninja: no work to do`), `ctest -R r38_zopf` 7/7 gruen; die Reste aus 5782ec05 (D_belege) sind die Ausgaben
derselben Riegel und stimmen mit dem Nachlauf ueberein (unit_ruhe/unit_drehung unveraendert reproduziert).

Fortsetzung 3 (2026-10-17 ab 09:00; der vierte Bau-Lauf hing 06:29-09:00 in EINEM Werkzeug-Aufruf "Bau + exe-Lauf
gehen1" und wurde vom Orchestrator gestoppt): 447c0e35 (Messzeile `pos=` im ZOPF-Log) gebaut und an der exe benutzt
(§4.4), Werkzeug-Regel ab hier: Bau und exe-Lauf nie im selben Aufruf, jeder exe-Lauf mit `timeout 300` im Hintergrund.
Keine Messung des Vorgaengers wiederholt.


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

## 3. Umsetzung (Commits 6f162af1, 03dd8cbc, 65dd6d59, 90252339, 447c0e35, cd9d9891 und der Abschluss-Commit)

**Neue Dateien (Logik):**
- `re15_port/engine/src/zopf_r38.c` — Nachbau der RE1.5-Routine, jede Konstante mit Adresse:
  - libgte: RotMatrix @0x80068098, RotMatrixYXZ @0x800666d0, RotMatrixY @0x800659d0, RotMatrixZ @0x80065b70 (je mit dem
    Negativ-Zweig `subu t7,zero,t7` + negiertem Sinus, Tabelle DAT_800794c4 = `re15_trig_lut`), ApplyMatrix @0x800661c0
    (MVMVA 0x4a486012, RT*V0 sf=1 ohne TR), Kettenmatrix FUN_80022da0 (gte_rtir lm=0 -> s16-Saettigung).
  - Winkelhelfer FUN_80025360 (Grundwert 0xc00/0x1400, `+0x800` bei y > 0) und FUN_800253fc (Grundwert 0x400/0xc00,
    negiert bei y > 0), catan 0x800658fc / rsin 0x800683e8 aus `re15_math.c`.
  - Glied FUN_80024e40 (Schritte 1-7 aus §2.1, Packungen `sll 4 / sll 20` @0x8002502c-50 und `sll 2 / sll 18`
    @0x800250f0-134, 12-Bit-Vorzeichenerweiterung @0x8002524c-260 / @0x800252e0-2f4).
  - Treiber FUN_80024c30 mit den vier Parametersaetzen @0x80024d04-e08 (Tabelle §2.1: p3/p4/p5/p6/p7/p8/p9/p10/p11/p12),
    Koerperwinkel `s0` @0x80024ca4-d34 (lhu-Summe, `+0x800` fuer Teil0.vz in [0x5dd, 0x9c4) @0x80024cc8-cc, Nacken-Gier
    ohne Bit 0 von 0x800acc0c @0x80024ce0-d00).
  - Init Kommando 0 (Handler @0x800318f8): nur Modelle 4/5/6 (`addiu -4; sltiu 2` @0x80031a88-8c, `bne v1,6` @0x80031aa0),
    Versaetze Teil 17..20 @0x80031b44-b80 ((-0x73,-0x15a) (-0x2d,-0x20) (-0x65,0x61) (-1,0x6a)), z = 0 @0x80031b88-bb8,
    v94/v96/rot.vy = 0 @0x80031b8c-be0, Flag 0x40 @0x80031bc4-bf0; die alte Lage bleibt (wie im Original).
  - Gates: Modell-Bit 2 `andi v0,v0,0x4` @0x80031d8c, mindestens 21 Teile (FUN_8001e56c `+0x83 = *(MD1+8)>>1`), Takt nur
    ohne Spieler-Freeze (`bltz a0` @0x80031c78), hoechstens ein Schritt je Bild.
- `re15_port/include/re15_zopf_r38.h` — Schnittstelle (Eingang = was FUN_80024c30 aus Spielerblock 0x800aca54 und
  Teile-Feld +0x188 liest; Glied = Felder +0x2c.., +0x40.., +0x54.., +0x60.., +0x94/+0x96).

**Haken in gemeinsamen Dateien (je 1-15 Zeilen, Kommentar "Runde 38 Spur D"):**
- `engine/src/game_step_common.c` (+2): `re15_zopf_r38_takt(rdt_ok && !RE15_PAUSE_PLAYER)` — der Spieler-Dispatcher-Rumpf
  lief (Zopfaufruf @0x80031d98 sitzt hinter dem Freeze-`bltz` @0x80031c78).
- `engine/src/room_common.c` (+2): `re15_zopf_r38_kommando0(-1)` an beiden Stellen, an denen der Port das Spieler-Kommando 0
  setzt (Raumwechsel `sb zero,0x800aca58` @0x8001cbdc, Uebergabe @0x8003192c).
- `engine/src/skeleton_common.c` (+4) / `include/re15_skeleton.h` (+3): `g_skel_lokal_rot` faengt je Knochen die fertige
  LOKALE Rotation (Teil 8 = +0x578, die FUN_80024c30 @0x80024c9c verkettet); NULL = aus (Standard).
- `platform/pc/main.c` (+13/-2): Fangpuffer um `re15_skel_compute_pose` des Spielers, danach `re15_zopf_r38_bild_aus_pose`
  (FUN_80024c30 @0x80031d98 laeuft im Original vor dem Zeichner, Haupt-Loop @0x8001ce0c -> @0x8001d09c); die Zeichenschleife
  zeichnet nach den 15 Skelett-Teilen die Teile 17..20 mit der Weltmatrix aus FUN_80024e40 (Flag 0x40 -> FUN_8001e9ec
  ueberspringt die Verkettung @0x8001ea10). Teile 15/16 bleiben fuer Elza aus (Flag 0 @0x80031abc / @0x80031b30).
- Mess-Log `RE15_ZOPF_LOG=<datei>` (env, kein Verhalten; Zeile je Schritt mit Eingang und `pos=`) und MESS-SONDE
  `RE15_ZOPF_SONDE_AUS=1` (Zopf gerechnet, nicht gezeichnet — nur fuer den Pixel-Riegel; kein Spielverhalten, kein Schalter
  fuer den Nutzer).

**Keine neuen Assets:** das Netz sind Elzas Original-Teile 17..20 aus `shared_assets/PSX/PLD/PL04.PLD` (MD1 21 Teile);
kein `gen/*.inc`, kein `shared_assets/RE2/`. Leon (Modelle 0/1/2, 17 Teile) laeuft durch keinen neuen Pfad (Modell-Bit 2).

## 4. Messung nachher

### 4.1 Drehung an der exe (Winkelverlauf) — Riegel `integration_r38_zopf` Teil "elza"
ROOM1121 (2500,10000) Blick 2048, Eingabe `W1,R1.0,W4`, `RE15_ZOPF_LOG`. `test_r38_zopf spur` (Lauf dieser Fortsetzung,
§5): siehe Riegel-Ausgabe; Stand des Vorgaengers (ctest 2026-10-16 23:43): Koerperwinkel s0 dreht sich um 2383, max
|v94|/|v96| = 102, Zopfspitze schlaegt 20,2 Grad seitlich gegen die Ruhelage 0,3 Grad aus, 6 Seitenwechsel, danach 52
Schritte ruhig. (Der Koerperwinkel s0 enthaelt wie im Original die Nacken-Gier: beim Raumeintritt dreht der Port-Nacken
dort auf 512 — §6 N4.)

### 4.2 Pixel am Hinterkopf — Riegel `integration_r38_zopf` Teil "elza_aus"
Gleiches Bild 50 mit Zopf und mit der Mess-Sonde (Zopf gerechnet, nicht gezeichnet): 102 Pixel verschieden, alle im
Hinterkopf-Fenster (195,75)-(255,135), Huelle (218,94)-(227,116). Bilder `D_belege/nachher_1121_f50.png`,
`nachher_1121_f50_mit_ohne.png`, `nachher_1121_drehung_lauf_mit_ohne.png`; vorher (Basis f587d34e) `vorher_1121_f50.png`.

### 4.3 Leon unveraendert, Elza ohne Zopfglieder = vorher (65dd6d59, `D_belege/vorher_nachher_vergleich.txt`)
`mess_vorher_nachher.sh`: Basis-exe f587d34e gegen Zweig-exe, gleiche Fahrt, Bilder 20..200 je 30:
Leon vorher/nachher 7/7 Bilder BITGLEICH; Elza Basis gegen Zweig mit Mess-Sonde 7/7 BITGLEICH (der Zopf aendert sonst
nichts am Bild); Elza mit Zopf: Unterschiede nur am Kopf (8..126 Pixel, Huellen (218,94)-(234,117)).

### 4.4 Gehen und Rennen an der exe gegen das ORIGINAL (Fortsetzung 3)
**Aufbau:** `D_belege/mess_laufen.sh` faehrt die exe mit DERSELBEN Eingabe wie die Original-Messung `orig_zopf_gdb.py`
(ROOM1121 (150,9550) Blick 2048: stehen, Tank-Drehung R, stehen, gehen U 40 Bilder, stehen). Das Original drehte in seinen
30 "drehung"-Bildern 29-mal (Bild 15 noch Blick 2048, Bild 44 = 4832 = 2048 + 29*96) -> Art `origfolge29` mit R 29 Bildern
(gleiche Endrichtung 4832, gleiche Gehrichtung); Art `lauf` = origfolge29 + Kehrt (R 21 Bilder) + Rennen X+U 14 Bilder.
`D_belege/vergleich_orig.py` richtet exe-Log und `orig_zopf_frames.txt` an Drehbeginn (Original Bild 16) und Gehbeginn
(Original Bild 135) aus. Ergebnisse `D_belege/vergleich_exe_original.txt`, Bilder `D_belege/nachher_1121_gehen_rennen.png`
(gehen Bild 285 / 295, rennen 445, Halt 455: der Zopf haengt, fliegt beim Gehen nach hinten, beim Rennen fast waagerecht).

**Eingaenge exe == Original** (der Port fuettert die Routine richtig): Blick-Abweichung 0 (origfolge29), Animationswinkel
Teil0 vy/vz + Teil8 vy beim Gehen hoechstens 1 Einheit (Mittel 0,1), bei der Drehung hoechstens 17 (Mittel 2,4; Phase der
Leerlauf-Animation beim Druck). Nicht gleich: die Lage-Schrittweite (exe 74 je Bild gleichmaessig, Original 55..90 mit dem
Gang, Mittel 77,7) und ab Port-Schritt 316 die Nacken-Gier (Port -96..-512, Original 0) — §6 N3/N4.

**Zopf beim Gehen, solange die Eingaenge gleich sind** (36 Bilder, Port-Schritte 280..315 / Original-Bilder 135..170):

| Glied | xang Abw. Mittel / max | zang Abw. Mittel / max | Original-Spanne zang |
|---|---|---|---|
| 17 | 1,3 / 4 | 20,1 / 36 (3,2 Grad) | 280 |
| 18 | 2,4 / 8 | 19,1 / 34 (3,0 Grad) | 364 |
| 19 | 5,7 / 18 | 26,9 / 48 (4,2 Grad) | 504 |
| 20 | 6,6 / 40 | 21,8 / 38 (3,3 Grad) | 528 |

**Die Restabweichung erklaert allein die gleichmaessige Port-Schrittweite** (`test_r38_zopf konst`, `D_belege/
konst_schritt74.txt`): dieselbe Original-Nachrechnung (r38_zopf_orig.inc-Eingaenge, Original-RAM), nur die Lage beim Gehen
mit konstant 74 je Bild in der mittleren Original-Gehrichtung -> zang-Abweichung Mittel 19,8 / 17,9 / 25,8 / 19,1, max
36 / 36 / 48 / 40 (xang max 4 / 8 / 10 / 10); Gegenprobe mit der Original-Lage: 38/38 Gehbilder bitgleich. Die Zopf-Routine
und ihre Verdrahtung an der exe sind also originalgetreu; die verbleibenden 3-4 Grad kommen aus der Fortbewegung des Ports
(`player_common.c` WALK_SPEED_PER_FRAME 75 * Richtung, kein Gang-abhaengiger Schritt) — nicht Teil dieser Spur (§6 N3).

**Groesster Ausschlag nach hinten beim Gehen** (Riegel `lauf`, Lauf `lauf`): Glied 17 exe 339 / Original 367 (29,8 / 32,3
Grad), Glied 18 745 / 765, Glied 19 615 / 637, Glied 20 637 / 645 — je Glied innerhalb der konst-Schranke 36/36/48/40.

**Drehung (origfolge29, Fenster Drehbeginn..Gehbeginn, 119 Bilder):** zang/xang-Abweichung Mittel 1,5..33,3, max 16..194
(Glied 20) bei Original-Spannen 84..1062 — der Startzustand (Phase der Leerlauf-Animation) ist verschieden, ab Blick 3008
liegen exe und Original innerhalb weniger Einheiten (z.B. Glied 20 bei Blick 4448: exe (-685,83) / Original (-671,175),
bei 4832: (-691,97) / (-683,185)).

**Rennen (nur exe, das Original wurde nicht rennend gemessen):** Glied 17 zang bis 667 (58,6 Grad; Gehen 339) — der Zopf
fliegt fast waagerecht nach hinten; Lauf `rennen` gegen die Wand: Glied 17 bis 717, Glieder 18..20 bis 943 (83 Grad). Nach
dem Halt schwingt Glied 17 durch die Ruhelage nach vorn (-219 bei Ruhe 3) und schwingt aus (Abstand 336 -> 80 in 10 -> 30
Schritten). Die Routine ist dieselbe, die das Original beim Gehen bitgleich zeigt (§4.6); beim Rennen aendert sich nur der
Eingang.

### 4.5 Huelle der Runde-37-Spur P (Figurkasten der Generator-Masken) — `test_r38_zopf huelle`, `D_belege/huelle_spur_p.txt`
Spur P (r37/masken36, in r38/integration gemergt, NICHT in diesem Baum) nimmt Masken-Rechtecke heraus, die an einem
erreichbaren Stehplatz einen Figurpunkt ohne Wand dazwischen verdecken; die Figur ist dort ein Kasten quer zur Kamera,
Hoehe 0..3000, quer +-350 (`tests/unit/test_r37_masken36.c` FIGUR_HOCH / FIGUR_HALB, an Leon gemessen: 2999 / 706).
Gemessen (Zopf-NETZ Teile 17..20 je Schritt mit der Weltmatrix aus FUN_80024e40 gegen die Spielerlage):
- Hoehe: hoechster Zopfpunkt 2847 (Gehen) / 2853 (Rennen) ueber dem Boden -> **innerhalb** (3000).
- waagerecht von der Figurachse: im Stehen 384..404, beim Gehen bis 485, beim Rennen bis 718 (gegen die Wand) bzw. 809
  (Halt + Leerlauf-Animation) -> **ragt seitlich hinaus** (350), wenn Elza von der Seite gesehen wird.
- Berichtigung zum Vorgaenger (Commit 65dd6d59, `skelett_pl04.txt`): "Zopf waagerecht hoechstens 251" galt fuer Keyframe 0
  der EMR (Kopf geneigt), nicht fuer die Spielpose; im Spiel haengt der Zopf-Ansatz 124 hinter der Kopfmitte.
Folge: die Ausschluss-Tabelle der Spur P prueft keine Zopfpunkte ausserhalb ihres Kastens; wo ein nicht ausgeschlossenes
Rechteck dort ohne Wand davor liegt, kann eine Generator-Maske (nur Stages 3-6) die Zopfspitze uebermalen — dieselbe Klasse
wie P OFFEN 2 ("ausgestreckter Arm/Waffe beim Zielen"). Fuer die Integration: §6 N5.

### 4.6 Gegenprobe am ORIGINAL (65dd6d59, 90252339)
`D_belege/orig_zopf_gdb.py` (DuckStation-GDB, Auslieferungs-EXE: Stub @0x80026e4c = `08 00 e0 03`): Elza per RAM
(PL-Index 0x800b0ff0 = 4, Elza-Bit 0x800aca3c) + Debug-JUMP 0x112 -> ROOM1121 mit PL04: Teile +0x83 = 21, Flags Teil 15..20 =
0x0, 0x0, 0x41, 0x41, 0x41, 0x41 (`orig_zopf_log.txt`) — **das Original zeichnet Elzas Zopf** (Flag 1) mit eigener
Weltmatrix (Flag 0x40). Haltepunkte @0x80024c30 (Eingang + Glieder vorher) / @0x80024e10 (nach dem 4. Glied): 235 Bilder
(ruhe 15, drehung 30, nachlauf 90, gehen 40, stehen 60), `orig_zopf_frames.txt`. Kette im Original geschlossen ("vor" von
Bild n+1 == "nach" von Bild n). Port-Schritt darauf: einzeln 235/235, fortlaufend ab Bild 0 235/235 BITGLEICH (Glieder
m/t/rot/v94/v96 + Teil 8 +0x40). Riegel `unit_r38_zopf_original` (Datensatz `tests/unit/r38_zopf_orig.inc`, erzeugt von
`orig_zopf_inc.py`). Mutation p7 0x12c -> 0x96 (RE2-Wert) faellt im Riegel durch (Vorgaenger, 65dd6d59).

## 5. Tests (probes/r38_zopf.cmake)

| Test | misst |
|---|---|
| unit_r38_zopf_libgte | RotMatrix/YXZ/Y/Z-Nachbauten inkl. Negativ-Zweig, Winkelhelfer |
| unit_r38_zopf_init | Gate Modell-Bit 2 + Init 4/5/6 + 21 Teile, Versaetze @0x80031b44-b80, Leon nie aktiv |
| unit_r38_zopf_ruhe | echte PL04-Pose: Zopf haengt, Glieder verbunden, kommt zur Ruhe |
| unit_r38_zopf_drehung | Tank-Drehung: Zopf bleibt zurueck, schwingt durch, kehrt zurueck |
| unit_r38_zopf_laufen | Vorwaertslauf: Zopfspitze bleibt hinter der Ruhelage |
| unit_r38_zopf_freeze | ohne Takt (Freeze @0x80031c78) kein Schritt |
| unit_r38_zopf_original | 235 Bilder Original-RAM fortlaufend bitgleich |
| integration_r38_zopf | exe: elza (Winkelverlauf Drehung), elza_aus (Pixel am Hinterkopf), leon (kein Zopf, 17 Teile), **elza_lauf** (neu: Gehen je Glied wie im Original, Rennen > Gehen, Schwung durch die Ruhelage, Ausschwingen; Huelle als Ausgabe) |

Werkzeuge (kein Riegel): `test_r38_zopf orig <frames.txt>`, `skelett`, `huelle <zopf.log>`, `konst <frames.txt> <schritt>`;
`D_belege/mess_laufen.sh`, `vergleich_orig.py`, `mess_vorher_nachher.sh`, `ppm_vergleich.py`.
Mutation zum neuen Riegel `lauf`: Zopf im Gehfenster auf Ruhe gesetzt (zang 3) -> 4 FEHLER (Glied 17..20, Abweichung
364..762 > Schranke).

Gezielter Lauf (Ausgabe des Abschluss-Laufs unten, §5.1).

## 6. OFFEN

Spur D: keiner. Der Nutzerpunkt ist umgesetzt (Netz aus Elzas Original-Daten, Mechanik = RE1.5-Routine = Claires RE2-Routine,
an der exe gegen das Original gemessen).

Nebenbefunde (NICHT beauftragt, nicht geaendert):
- **N1 Leon Teil 15** (6 V, (-22..34, -256..0)): im Original an (Flag 1), an Teil 2 mit Versatz (-9,0x66,-0x94)
  (@0x80031a00-74); der Port zeichnet es nicht (Zeichenschleife nur 15 Skelett-Teile). Leon bleibt in dieser Spur bitgleich.
- **N2 Port-RotMatrix Negativ-Zweig:** `skeleton_common.c` mat3_from_euler liest negative Winkel ueber `& 0xfff`, das
  Original @0x80068098 ueber einen eigenen Zweig mit negiertem Sinus; die Tabelle DAT_800794c4 ist nicht antisymmetrisch
  -> 1 Tabellenschritt Abweichung fuer negative Winkel, alle Knochen (der Zopf rechnet mit dem richtigen Zweig).
- **N3 Gehschritt:** die exe bewegt Elza beim Gehen gleichmaessig 74 je Bild (`player_common.c` WALK_SPEED_PER_FRAME 75 *
  Richtung), das Original 55..90 je Bild mit dem Gang (orig_zopf_frames.txt Bilder 135..172, Mittel 77,3). Am Zopf: 3-4 Grad
  (§4.4, `konst`).
- **N4 Nacken in ROOM1121:** der Port-Nacken dreht in ROOM1121 auf +-512 (beim Eintritt an (2500,10000); an (1297,7071) beim
  Gehen; Kraehe EM021 im Bild), das Original stand in der GDB-Messung an derselben Stelle mit Nacken-Gier 0 (Flags 0x18).
  Ob die Kraehe im Original-Spielstand (Debug-JUMP aus mzd_debugmenu.sav) aktiv war, ist nicht gemessen. Der Zopf folgt der
  Nacken-Gier wie im Original (@0x80024ce0-d00).
- **N5 Huelle Spur P (fuer die Integration):** §4.5 — der Zopf ragt seitlich bis 404 (Stehen) / 485 (Gehen) / 809 (Rennen)
  ueber den Figurkasten +-350 der Ausschluss-Tabelle hinaus. Pruefung/Anpassung gehoert zur Spur P (Tabelle
  `gen/masken36_ausschluss_r37.inc` + Riegel `unit_r37_masken36_ausschluss`), nicht in diesen Baum.
- **N6 Modell 7:** der Original-Init kennt nur 4/5/6 (@0x80031a88-aa0); PL07 hat ebenfalls 21 Teile, bekaeme im Original
  keinen Zopf-Init (Teile 15..20 Flag 1 an der Einheitsmatrix). Der Port laedt fuer Elza PL04; fuer 7 zeichnet er keinen Zopf.

## 7. Fuer den Nutzer

- **Was sich aendert:** Elza hat ihren Zopf. Er war in RE1.5 schon da — Elzas Spielermodell (PL04) traegt vier Zopfglieder
  und das Original rechnet sie jedes Bild mit derselben Pendel-Routine, die RE2 fuer Claires Zopf benutzt (RE1.5
  FUN_80024c30/80024e40 = RE2 FUN_8001844c/80018728). Der Port hatte nur die 15 Skelett-Teile gezeichnet. Jetzt haengt der Zopf
  am Hinterkopf, bleibt bei Drehungen zurueck und schwingt nach, fliegt beim Gehen nach hinten (wie im Original, gemessen bis
  auf 3-4 Grad), beim Rennen fast waagerecht, und schwingt beim Stehenbleiben nach vorn durch. Leon ist unveraendert.
- **RE1.5 oder RE2:** RE2 hat fuer Claire denselben Algorithmus; es unterscheidet sich in einem Ruhewinkel (zweites Glied:
  RE1.5 300 = 26 Grad, RE2 150 = 13 Grad, @0x80024d58-98 / RE2 @0x8001850c-614) und einer Sonderregel (RE1.5 dreht den Zopf
  um 180 Grad, wenn der Koerper um die z-Achse zwischen 132 und 220 Grad gekippt ist, @0x80024cc8-d8; RE2 ohne). Der Port
  nimmt die RE1.5-Werte, weil sie zu Elzas Netz gehoeren (RE2s Versaetze und Werte sind auf Claires Netz abgestimmt).
- **Neue Assets / Paket-Gate:** keine (Elzas Original-PLD). **Sprachdateien:** keine. **Bedienung:** keine.
- **Hinweis Integration:** in den Raeumen der Stages 3-6 mit automatisch erzeugten Vordergrund-Masken (Spur P der Runde 37)
  kann die Zopfspitze an einer Maskenkante einzelne Punkte verlieren, wenn Elza von der Seite gesehen rennt (§4.5, N5).
