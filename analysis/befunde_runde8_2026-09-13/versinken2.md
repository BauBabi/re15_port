# "Versinkt immer noch komisch im Boden" (Schrot, ROOM1030) — der Ragdoll verankert die BEINE, der Port nicht

Nutzer 2026-09-13, ZWEITE Meldung nach v0.7.90. Messwerte aus seinem Lauf
(`re15_port/build/platform/pc/befund.log`, Marken 1/2 in ROOM1030, Zeilen 62326 / 62362-62367;
Bilder `befund_1030_F1975_marke1.bmp` / `befund_1030_F2238_marke2.bmp`).
NUR statische Analyse. Alle Zahlen unten sind SELBST gerechnet: Skripte im Scratchpad
(`emsdump.py` = EMS/EMD/EDD/EMR-Parser, `fk.py` = Nachbau der Port-Kette
`mat3_from_euler`/`re15_skel_compute_pose` inkl. Lean-Injektion, Q12, byte-true Trig-LUT
`re15_trig_lut.c`). Quellen: `re15_port/shared_assets/RE2/CDEMD0.EMS` (EM016, TOC
`re15_port/engine/src/gen/re2_ems_toc.inc`), `build/extracted/re2_ems/CDEMD0_EM10_ai1.BIN`
(Zombie-Overlay @0x80100000, selbst disassembliert), `info/re2leon/PSX.EXE`,
`RE2_Quellcode_V2/FUN_80029614.c` / `FUN_8002959c.c` / `FUN_80015e7c.c` / `FUN_80027160.c` /
`FUN_80027434.c` / `FUN_8002ce94.c` / `FUN_8004fba0.c`.
Vorarbeit: `analysis/befunde_runde7_2026-09-13/liegend-unter-boden.md`.

---

## 0. Kurzfassung

Das wachsende Entity-Y **ist byte-true** — der Port rechnet den Integrator auf die Einheit genau
richtig (Beleg §1). Die Praemisse "die Clip-Wurzel hebt den Koerper gleichzeitig an" ist
**falsch**: die Ragdoll-Clips 3/4 sind AUFRECHTE Clips (Wurzel -1882..-1917, Fuss-Bone -212..-251
ueber alle 44 Bilder). Das Original haelt den Unterkoerper trotzdem am Boden, weil es beim
AUFSCHLAG-Bild (+0x14D == 20) **Part 1 (Becken) aus der Kette haengt und an eine BODEN-Matrix
umhaengt** (`sw v0,288(v1)` @0x80106D00 = part1+0x74 := entity+0x11C), deren Y jeden Tick auf
`part0.t[1] + +0x232` gesetzt wird (@0x80106E64-74) — **ohne +0x3C**. Becken und Beine sinken im
Original also GAR NICHT mit. Der Port kennt diesen Umhaenger nicht und zieht die komplette
Kette am sinkenden `e->y` mit: bis **1403 Einheiten unter den Boden** (§3-Tabelle).

## 1. Q1 — Ist das wachsende Entity-Y byte-true? JA (auf die Einheit nachgerechnet)

Integrator, selbst disassembliert (`CDEMD0_EM10_ai1.BIN`):

    80106e78: lh    v1,346(s2)      ; +0x15A
    80106e7c: lw    v0,60(s2)       ; +0x3C
    80106e84: addu  v0,v0,v1
    80106e88: sw    v0,60(s2)       ; +0x3C += +0x15A
    80106e8c: lbu   v0,333(s2)      ; +0x14D
    80106e94: sltiu v0,v0,0x23      ; < 35 ?
    80106e90/9c: addiu v1,a0,5      ; ja   -> +0x15A += 5
    80106ea0-a4: addiu v0,a0,55     ; nein -> +0x15A += 55
    80106ea8: lh    v0,562(s2)      ; +0x232 (Boden)
    80106eac: lw    v1,48(s3)       ; part0.matrix.t[1]   (s3 = +0x198, reload @0x80106DB8)
    80106eb4-bc: (gy-200) - rooty < +0x3C ?
    80106ee4-f4: +0x15A = -(+0x15A >> 3) ; 80106f00-08: +0x16A-- ; 80106ef0-f10: +0x3C = gy-300-rooty

Start @0x80106D50-5C: `+0x15A = 10`, `+0x16A = 2` bei `+0x14D == 20 && +0x6 == 1`.
Nachgerechnet (Scratchpad `fk.py`, gy232 = 0):

| +0x14D | 20 | 25 | 30 | **32** | **34** | **36** | 38 | 39 | 42 | **43** |
|---|---|---|---|---|---|---|---|---|---|---|
| sim y | 10 | 135 | 385 | **520** | **675** | **900** | 1345 | Klemme | 1515 | **1488** |
| Log   |    |     |     | **520** | **675** | **900** |      |        | 1569* | **1488** |

Die drei vorgeklemmten Staende (520/675/900) treffen **exakt** und sind vom Rootfix unabhaengig.
Der Endwert 1488 faellt exakt auf die Klemme `gy-300-(kf_py+K)` mit **K = +129** (mit K = 0 kaeme
1628 heraus) — das ist die erste direkte MESSUNG des Hybrid-Rootfixes aus dem Nutzer-Log, und sie
bestaetigt Runde 7 (K ist klein). (*die 1569 stammen aus einer anderen Marke/Instanz.)

**Aber die Clip-Wurzel hebt NICHTS an.** Selbst aus dem EMS gedumpt (EM016 = kind 0x16, Paar 2 =
D[3] EDD @0x506C / D[4] Pool @0x64A8; Binder-Beleg `sw v1,388(s1)` / `sw v1,384(s1)`
@0x8001ABD0/E0):

| Clip | Bilder | rootY (kf_py) | Pose (FK, RE2-Binds) |
|---|---|---|---|
| 3 (Rueckentreffer) | 40 | -1785 … -1847 | aufrecht |
| **4 (Fronttreffer)** | **44** | **-1882 … -1917** | **aufrecht, Fuss-Bone -212..-251** |
| 1/2 (Knockdown) | 60/60 | -1976 -> -222/-227 | legt sich selbst hin |
| 5 (Kriechen) | 50 | -175 konstant | liegend |

Gerenderte Wurzel = `e->y + kf_py + K`: -1203 (Bild 32) -> -1082 (34) -> -874 (36) -> **-300** (43).
Die **Wurzel** landet also sauber 300 ueber dem Boden — genau was die Klemme bestellt
(`+0x3C = gy-300-rooty` ⇒ Wurzelwelt == gy-300). Der **Koerper** aber nicht: er steht in diesem
Clip aufrecht, also faehrt alles unterhalb der Brust mit `e->y` in den Boden.

## 2. Q2 — Was der PORT zeichnet (Code-Beleg)

* `skeleton_common.c:255-256` setzt `kf_py += re15_skel_root_y_fix(...)`, `:705` schreibt
  `poses[0].trans[1] = rt_y`; jede Kind-Kette haengt daran (`:715`).
* `platform/pc/main.c:8370-8373`: `nbone_world_trans[1] = nyawed_trans[1] + npc->y`
  (die Yaw-Matrix laesst Y unveraendert) — **die gezeichnete Welt-Y JEDES Bones ist
  `npc->y + poses[b].trans[1]`**.
* Die Klemme liest dieselbe Zahl (`enemy_ai_re2_zombie.c:5772`, `py + re15_skel_root_y_fix`,
  Runde-6-Fix). Klemme und Renderer sind also konsistent; **der root_y_fix erdet nichts und ist
  nicht der Taeter** — er verschiebt beide Seiten um dieselben ~129 Einheiten (gemessen §1),
  nicht um 1300.
* Bank: der Ragdoll faehrt das DISPATCHER-Paar (Paar 2). Beleg: Root-Dispatch
  `lw a1,384(s0)` / `lw a2,388(s0)` @0x801004C8/D0 -> HURT-Wurzel s2/s3 -> `jalr` @0x80105410
  (a1/a2 durchgereicht) -> Ragdoll-Prolog `s4=a1, s5=a2` @0x8010670C/44 -> Advance
  `jal 0x8002959c` @0x801069D4. `re15_re2z_poses_loco_bank` liefert fuer die Zelle 66FC 0
  (= Paar 2) — Renderer und `re2z_root_py` lesen also **dieselbe** Bank. Kein Bank-Bruch.
* Clip: `lui a2,0x3 / ori a2,a2,0x4` @0x80106838-3C, `sw a2,332(s2)` @0x80106888 ⇒ Byte
  +0x14C = 4. FUN_8002959c indiziert die EDD mit genau diesem Byte
  (`*(uint*)(param_3 + *(byte*)(param_1+0x14c)*4)`), +0x14D ist der Bildzeiger (`+1` pro Aufruf,
  Wrap = Rueckgabe; FUN_80029614 Ende) und +0x14E die BLEND-Zahl, NICHT die Rate
  (`iVar18 = (param_4 & 0xffff) * +0x14E`, GPF12/GPL12 auf die Wurzel). Der Port bildet das 1:1 ab.

## 3. DER FUND — die BODEN-VERANKERUNG von Becken+Beinen fehlt im Port

Beim Aufschlagbild (`+0x14D == 20 && +0x6 == 1`, @0x80106CD8-F0) tut das Original drei Dinge,
von denen der Port nur das letzte hat:

    80106cf4: addiu v0,s2,284        ; v0 = entity+0x11C  (Ragdoll-MATRIX)
    80106cf8: lw    v1,408(s2)       ; v1 = +0x198 = Part-Pool
    80106d00: sw    v0,288(v1)       ; pool+288 = part1+0x74  := entity+0x11C   <<< UMHAENGEN
    80106d04-40: lw 72..100(v1) -> sw 284..312(s2)   ; +0x11C := part0-WELTMATRIX (+0x48)
    80106d44/60: lw v1,56(s2)  -> sw v1,304(s2)      ; +0x130 = entity.x   (EINGEFROREN)
    80106d48/68: lw a0,64(s2)  -> sw a0,312(s2)      ; +0x138 = entity.z   (EINGEFROREN)
    80106d50-5c: +0x15A = 10 ; +0x16A = 2            ; (das hat der Port)

und danach JEDEN Tick, solange `+0x14D >= 20 && +0x6 == 1` (@0x80106D90-E74):

    80106dc0-e0c: parts 1..7 Flagwort |= 0x9000      ; Zeichner-Pfad FUN_80027434
    80106e20:     jal 0x8002ce94(a0=entity+0x24, a1=part0+24, a2=sp+32)   ; Welt-Rotation
    80106e28-60:  sp+32..48 -> +0x11C..+0x12C        ; nur die ROTATION
    80106e64-74:  +0x134 = part0.t[1] + +0x232       ; <<< Y = Clip-Wurzel + BODEN, OHNE +0x3C

Layout-Belege: Part-Stride 0xAC, `+0x24` lokale Matrix (t[1] bei +48), `+0x48` komponierte
WELT-Matrix, **`+0x74` = Zeiger auf die ELTERN-Matrix** (`child[+0x74] = &parent+0x48`,
FUN_80028368; im Port bereits dokumentiert, `enemy_ai_re2_zombie.c:4046-4060`); 288 = 172*1 + 116
= part1+0x74. FUN_80027434 liest genau diesen Zeiger als Elternmatrix
(`pMVar10 = param_2[0x1d]`), und die Bind-Position von Bone 1 ist **(0,0,0)** (EMR D[2], selbst
gedumpt) — Becken und Beinkette sitzen damit exakt auf `+0x130/+0x134/+0x138`.

**Wirkung, ausgerechnet (FK ueber die echte Bank, Lean-Kette wie im Port, K = 129):**

| +0x14D | e->y (Log) | tiefster Bein-Bone PORT | tiefster Bein-Bone ORIGINAL (verankert) |
|---|---|---|---|
| 32 | 520 | **+439** | -210 |
| 34 | 675 | **+593** | -211 |
| 36 | 900 | **+817** | -212 |
| 43 | 1488 | **+1403** | -214 |

(PSX-Y zeigt nach unten; positiv = UNTER dem Boden.) Im Original stehen Becken und Beine ueber
den ganzen Ragdoll bei -210..-214, also auf dem Boden — nur der Rumpf faellt (Brustwurzel
-1203 -> -300, dazu die Lean-Kippung @0x80106CA0-D4). Im Port faehrt der ganze Koerper mit:
**das ist das gemeldete Versinken**, und es ist exakt das, was Marke 1 (F1975) zeigt.
Der X/Z-Anteil gehoert dazu: der Original-Anker friert x/z beim Aufschlag ein, waehrend die
Wurzelbewegung des Clips (kf+6/+10 -> FUN_80015E7C -> FUN_800152C8) die Entity um ~1100 Einheiten
zurueckschiebt — im Port rutschen die Beine mit.

## 4. Q3 — Laeuft der P2-Restore im DEATH-Zweig? JA. (und der Blend-Seed fehlt)

* Der Restore ist UNBEDINGT und steht VOR dem Death-Gate:
  `lh v1,562(s2)` @0x80106AC0 / **`sw v1,60(s2)` @0x80106AC8** (= `+0x3C = +0x232`), erst danach
  `lbu v1,4 / bne 3 / sh 10,342` @0x80106ACC-E8 (hp = 10). Die DEATH-Zelle ist DIESELBE Funktion
  (Zeile 7 Spalte 1 = 0x801066FC); der Port dispatcht sie korrekt
  (`enemy_ai_re2_zombie.c:7152`).
* Die Vorzweige von `re2z_death` (`+0x10E&1`, `+0x21A&0x10`, `+0x21A&2`; @0x801083B0/EC/408)
  sind waehrend des Ragdolls alle klar (`|= 0x10` nur in den Liege-/Kriech-Executoren, Port
  :2132/:2554) — der Dispatch erreicht P2.
* Das Log bestaetigt es: Gegner 1 steht in F2238 auf `ss=7/2 clip=4 bild=43` — Bild 43 ist das
  LETZTE Bild von Clip 4 (44 Bilder), `re2z_clip_done` hat im selben Tick `+0x6` auf 2 gezogen,
  P2 laeuft im Folgetick. **y = 1488 ist der Uebergangs-Tick, kein Dauerzustand** — passend dazu
  steht KEINE Leiche und kein Kriecher im ganzen Log mit y != 0 (Gegner 5 st=7 y=0, Gegner 4/6
  st=3 clip=2 y=0). Das Versinken ist transient (~24 Bilder = 0,8 s) und trotzdem deutlich
  sichtbar — genau die Nutzer-Formulierung "versinkt komisch".
* **Zweiter, kleinerer Fund an derselben Stelle (P2):** das Original saet den Crossfade fuer den
  Folge-Clip 5 auf eine LIEGENDE Ausgangspose: `sh -200,58(s3)` @0x80106AAC (part0 "prev root py"
  := -200; FUN_80029614 liest Byte 56/58/60 als Blend-Quelle) und
  `sh zero,104(s3) / sh zero,106(s3) / sh 1024,108(s3)` @0x80106AB4-BC (part0-Winkel := (0,0,1024)
  = 90° um Z). Der Port hat dafuer keinen Zwilling und blendet aus der zuletzt GERENDERTEN
  Stehpose (`prev_root[1] ≈ -1788`) mit wp = 15*256 = 94 % — die ersten ~15 Bilder des
  Kriech-Clips ziehen die Wurzel bis 1,6 m hoch und sacken dann ab.

## 5. PATCH-PLAN (Datei:Zeile, jede Zahl mit @0x)

1. **Anker-Felder** — `re15_port/include/re15_actor.h`, neben `re2z_gy232` (:338):
   `int32_t re2z_rag_anchor_x, re2z_rag_anchor_z; uint8_t re2z_rag_anchor_on;`
   Kommentar-Anker: `+0x130/+0x138` eingefroren @0x80106D44-68, Umhaenger
   `part1+0x74 := entity+0x11C` @0x80106D00. Ein Y-Feld ist NICHT noetig: die Anker-Y ist
   `re2z_gy232 + rooty` und wird pro Tick neu gebildet (@0x80106E64-74) — `rooty` liefert schon
   `re2z_root_py()` (:5734).
2. **Setzen** — `enemy_ai_re2_zombie.c:5946` (Block `frame == 20 && sub_state_2 == 1`,
   @0x80106CD8-F0): `e->re2z_rag_anchor_x = e->x; e->re2z_rag_anchor_z = e->z;
   e->re2z_rag_anchor_on = 1;`
3. **Loeschen** — `enemy_ai_re2_zombie.c:5891` (P2-Eintritt, vor `e->y = re2z_gy232`) sowie in
   `re2z_init`. ⛔ OFFEN, ehrlich benannt: WELCHE Stelle im Original den Zeiger `part1+0x74`
   zurueckstellt, habe ich nicht gefunden; wahrscheinlichster Kandidat ist der
   Modell-Variantenwechsel in P2 (`word0 &= 0xF3FFFFFF`, `| 0x04000000` @0x80106B38-50), der den
   Part-Block neu aufbaut (FUN_80028368 setzt `child[+0x74]` neu). Fuer den Port ist das Loeschen
   in P2 die einzige sinnvolle Entsprechung.
4. **Zeichnen** — `platform/pc/main.c:8370-8373`, direkt nach `nbone_world_trans` und VOR dem
   Gore-Haken (:8402). Neue Engine-Funktion in `enemy_ai_re2_zombie.c` (damit die Belege bei der
   KI bleiben), z.B. `re15_re2z_ragdoll_part_anchor(npc, nbi, part0_welt, nbone_world_trans)`:
   fuer `re2z_rag_anchor_on` und Bone 1..7

       anker = { re2z_rag_anchor_x, (int32_t)re2z_gy232 + poses[0].trans[1], re2z_rag_anchor_z }
       nbone_world_trans[k] += anker[k] - part0_welt[k]   /* part0_welt = yaw*poses[0].trans + npc->pos */

   Der Y-Term kuerzt sich zu **`gy232 - npc->y`** (poses[0].trans[1] faellt weg) — das ist genau
   die Senke, die heute mitgezeichnet wird. Bone-Index == RE2-Part-Index (die Hybrid-Hierarchie
   IST die RE2-Hierarchie); NICHT ueber `re2z_bone_to_part` gehen, das ist die Mesh-Permutation
   des Gore-Blocks.
5. **Blend-Seed in P2** — `enemy_ai_re2_zombie.c` (default-Zweig, vor `re2z_clip(e,5,...)` :5904):
   `e->prev_root[1] = -200;` (@0x80106AAC) und
   `e->prev_angles[0][0] = 0; e->prev_angles[0][1] = 0; e->prev_angles[0][2] = 1024;`
   (@0x80106AB4-BC). Beide Felder existieren (`re15_actor.h:718-719`) und werden von
   `skeleton_common.c:290-296` / `:316-336` konsumiert.
6. **NICHT anfassen** (alles nachgemessen sauber): die Klemme :5969-5977, `re2z_root_py` :5772
   (Runde-6-Fix), `re2z_ground_y` :7679 + der P0-Latch :5869 (Runde-7-Fix — er bleibt richtig und
   schuetzt den Wiedereintritt), der Rootfix in `re2_ems.c:299-322`, der Renderer
   `skeleton_common.c:256/264`, die Clip-Wahl 3/4.
7. **Abnahme**: `[z-ragdoll]`-Trace (:5959) um `beinY` (Welt-Y von Bone 4) erweitern; erwartet
   ueber den ganzen Ragdoll -180..-260 statt heute +439..+1403. Dazu eine Nutzer-Marke in
   ROOM1030 mit Schrot.

## 6. Rest-OPEN (mit Adresse, ohne Erfindung)

* `parts 1..7 |= 0x9000` @0x80106DC0-E0C — schaltet im Original auf den Zeichner FUN_80027434
  (Skalierung 0x800 / Eigenmatrix 0x40 / Flugphysik 0x20 / Lichtmatrix 0x1000). Kein
  Port-Zwilling; positionsneutral, deshalb nicht Teil des Fixes.
* `+0x11C`-Rotation (@0x80106E20-60) — im Port identisch zur ohnehin berechneten Weltrotation von
  Part 0 (FUN_8002ce94: out.rot = a.rot * b.rot), deshalb im Patch nur die Translation.
* `+0x1DE` (Wurzel-Y-Skalierung in FUN_80029614: `t[1] = (kf_py * +0x1DE) >> 12`) ist laut
  `FUN_8003baf0.c:58` konstant 0x1000 = 1.0 — der Port darf sie weiter weglassen.
