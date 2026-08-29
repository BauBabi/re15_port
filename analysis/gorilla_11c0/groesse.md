# ROOM11C0 Gorilla-Boss (Typ 0x27) — „zu klein"-Analyse

Datum: 2026-08-29/30. Nutzer-Report: „die Gorillas sind irgendwie sehr klein" (im Spiel getestet, Parking-Garage ROOM11C0, Typ 0x27, AI-Root 0x80116db8).

Status: ABGESCHLOSSEN 2026-08-30.

## ERGEBNIS (Kurzfassung)
**Das Original rendert die Gorillas mit einem uniformen Entity-Render-Scale von 1.7×
(entity+0x166 = 0x1b33 Q12, Gate = Entity-Flag 0x800, angewandt via ScaleMatrix @0x80065ff0
im Charakter-Render-Root FUN_8001e8c8 @0x8001e904-40). Der Port kennt weder das Feld noch den
Mechanismus — er rendert das (zombie-hohe) EM027-Modell mit 1.0× → „sehr klein".**
Modellwahl und Spawn-Positionen des Ports sind korrekt; Savestate-Live-Beweis: beide Gorillas
tragen im echten Spiel Flag 0x800 + 0x1b33 = exakt 1.7000×.

## Abgearbeitete Wege
1. [x] MODELL: CDEMD0.EMS Blob idx 12 (EM027) — korrekt geladen, aber nur zombie-hoch (dY≈3034 vs. Zombie 2993).
2. [x] SKALIERUNG: +0x166/Flag 0x800/ScaleMatrix — GEFUNDEN (Fund 1/2).
3. [x] SAVESTATE: mzd_stage1_maggot(.heavy).sav — 1.7000× live bestätigt (Fund 5).
4. [x] PORT: kein Scale-Feld, kein Scale im Render (Fund 4).
5. [x] SPAWN-POSITION: RDT-Records = Port-Spawn, kein Kamera-/Abstands-Artefakt (Fund 3).

## Zwischenfunde

### FUND 1 (KERN): Das Original rendert den Gorilla mit ENTITY-SCALE 1.7× — der Port ignoriert entity+0x166 komplett

**Mechanismus (EXE, Render-Root der Charaktere) — `RE_15_Quellcode_V2/FUN_8001e8c8.c`:**
```c
RotMatrix((SVECTOR *)(DAT_800ac784 + 0x68),(MATRIX *)(DAT_800ac784 + 0x20));
if ((*param_1 & 0x800) != 0) {                       // Entity-Flag-Bit 0x800 = "Scale aktiv"
    local_20.vx = (long)*(short *)(DAT_800ac784 + 0x166);
    local_20.vy = (long)*(short *)(DAT_800ac784 + 0x166);
    local_20.vz = (long)*(short *)(DAT_800ac784 + 0x166);
    ScaleMatrix((MATRIX *)(DAT_800ac784 + 0x20),&local_20);   // PsyQ libgte, Q12: 0x1000 = 1.0
}
```
→ **entity+0x166 (s16, Q12) ist ein uniformer Render-Scale**, angewandt auf die Root-Matrix
VOR dem Bone-Rendering (FUN_8001ef54/FUN_8001e9ec-Schleife über +0x83 Bones). Gate = Flag-Bit
0x800 im Entity-Wort +0x0.

**Gorilla-INIT (STAGE1-Overlay, Typ 0x27) — `RE_15_Quellcode_Overlays/STAGE1_full/FUN_80116f50.c`:**
```c
*_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
*_DAT_800ac784 = *_DAT_800ac784 | 0x800;              // Scale-Flag AN
*(undefined2 *)((int)_DAT_800ac784 + 0x166) = 0x1b33; // 0x1b33 = 6963 = 6963/4096 = 1.6999 ≈ 1.7×
if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x40) != 0) {
    // Spawn-Flag-Override: Scale = ((flags&0xf)+10)*0x1000/10  → 1.0 … 2.5 in 0.1er-Schritten
    *(short *)((int)_DAT_800ac784 + 0x166) =
        (short)((((*(byte *)((int)_DAT_800ac784 + 9) & 0xf) + 10) * 0x1000) / 10);
}
```
→ Der Gorilla wird im Original **1.7× so groß gerendert wie sein EMD-Modell** (bzw. per
Spawn-Byte +0x9 Bit 0x40 + Nibble übersteuert). Instruktions-Adressen (Disasm) folgen unten.

**Vergleich anderer INITs (gleicher Mechanismus, andere Werte):**
- Zombie-Familie STAGE1 `FUN_80109554.c:63`: `+0x166 = 0x1000` (= 1.0).
- STAGE2 `FUN_80110b6c.c:47`: `+0x166 = 0xccc` (= 0.8).
- Sterbe-/Effekt-Shrink `FUN_80109164.c` (STAGE1): `+0x166 -= 2` pro Frame, Floor 100.
- Weitere 0x1b33-Setter in STAGE1: FUN_80115f58 / FUN_80115f70 / FUN_80116750 (Zuordnung offen,
  vermutlich die Nachbar-Typen der Gorilla-Familie) + Kind-Spawns via FUN_8011696c
  (`param_1+0x166 = ((v1&0xf)+10)*0x1000/10`).

**Modell selbst ist NICHT groß** (gemessen aus CDEMD0.EMS, Bind-Pose Translation-only compose):
| Blob | Typ (s_ems_order) | Bones | kf | Meshes | Extent dX/dY/dZ |
|---|---|---|---|---|---|
| 0 | 0x10 Zombie | 15 | 80 | 15 | 635 / 2993 / 1010 |
| 12 | 0x27 Gorilla | 18 | 96 | 22 | 986 / 3034 / 1478 |
| 13 | 0x29 | 18 | 96 | 19 | 986 / 3017 / 1478 |

→ Gorilla-EMD ist in etwa zombie-hoch; die Bühnen-Größe entsteht NUR durch den 1.7×-Scale.
(CDEMD0- und CDEMD1-Blobs sind für idx 0/12 byte-gleich groß; Port lädt CDEMD0 idx 12 — kein
Split-EM27.EMD vorhanden, `pc_enemy_read_re15_emd`, `re15_port/platform/pc/main.c:556-570`.)

### FUND 2: Roh-Instruktions-Beleg (byte-true)

**INIT ist wirklich state[0] des 0x27-Bosses:** PRIMARY-Dispatch-Tabelle @0x801213c8
(STAGE1.BIN, Datei-Offset 0x213c8, Overlay-Base 0x80100000, kein 0x800-Header):
`[0]=0x80116f50 [1]=0x80117254 [2]=0x8011af5c [3]=0x8011b6fc …` → **FUN_80116f50 = INIT**.

**Overlay STAGE1.BIN (Gorilla-INIT), disasm `re15_disasm.py`:**
```
80117130: lw   v0,0(v1)          ; entity-Wort +0x0
80117138: ori  v0,v0,0x800       ; SCALE-ENABLE-Flag
8011713c: sw   v0,0(v1)
80117148: ori  v0,zero,0x1b33    ; 0x1b33 = 6963 → 6963/4096 = 1.6999 ≈ 1.7
8011714c: sh   v0,358(v1)        ; 358 = 0x166  → entity+0x166 = SCALE
80117158: —
8011715c: lbu  v1,9(a0)          ; Spawn-Byte +0x9 (Sce_em_set pc[3] "behavior")
80117164: andi v0,v1,0x40        ; Override-Gate
80117168: beq  v0,zero,0x80117194
80117170: lui  v0,0x6666         ; magic 0x66666667 = div 10
80117178: andi v1,v1,0xf
8011717c: addiu v1,v1,10         ; (nibble+10)
80117180: sll  v1,v1,12          ; ×0x1000
80117184: mult v1,v0
80117188: mfhi v0
8011718c: sra  v0,v0,2           ; /10
80117190: sh   v0,358(a0)        ; Override-Scale = ((n&0xf)+10)*0x1000/10 → 1.0…2.5
```

**EXE PSX.EXE (Charakter-Render-Root FUN_8001e8c8), disasm:**
```
8001e8f4: jal  0x80068098        ; RotMatrix(entity+0x68 → entity+0x20)
8001e8fc: lw   v0,0(s0)          ; entity-Wort +0x0
8001e904: andi v0,v0,0x800       ; ← SCALE-GATE
8001e908: beq  v0,zero,0x8001e948
8001e91c: lh   v0,358(v1)        ; entity+0x166 → vec.vx
8001e928: lh   v0,358(v1)        ;             → vec.vy
8001e938: lh   v0,358(v1)        ;             → vec.vz
8001e940: jal  0x80065ff0        ; ScaleMatrix(entity+0x20, vec)
```
**0x80065ff0 = ScaleMatrix** (verifiziert am Code selbst: spaltenweises Q12-Multiply der
3×3-s16-Matrix mit vec, `mflo/sra 12`, @0x80065ff0-…; PsyQ-libgte-Semantik). Danach läuft die
Bone-Schleife (FUN_8001ef54/FUN_8001e9ec über +0x83 Parts) mit der SKALIERTEN Root-Matrix →
alle Bone-Offsets UND Vertices werden uniform ×1.7 gerendert (Entity-Weltposition unskaliert).

### FUND 3: ROOM11C0-Spawns nutzen den Override NICHT → es gilt der Default 1.7×

`ROOM11C0.RDT` (re15_port/shared_assets/PSX/STAGE1/), Sce_em_set-Records (Opcode 0x44) Typ 0x27:
```
off=0x1784 slot=1 behavior=0x30 pos=(-1220,-20000,-21568) dir=2656
off=0x1798 slot=2 behavior=0x30 pos=(-554,-20000,-25423)  dir=2656
off=0x17e8 slot=1 behavior=0x10 pos=(-9013,0,-15461)      dir=2656
off=0x17fc slot=2 behavior=0x10 pos=(9434,0,2189)         dir=2656
```
behavior (= entity+0x9, Port: `a->grid_id`, scd_vm.c:3341) ist 0x30/0x10 — **Bit 0x40 nirgends
gesetzt** → kein Override, alle vier Gorilla-Records rendern im Original mit **Scale 0x1b33 = 1.7×**.
(Die Spawn-POSITIONEN übernimmt der Port 1:1 aus demselben Record — „zu klein" ist also KEIN
Abstands-/Kamera-Artefakt, sondern der fehlende Scale.)

### FUND 4: Port-Seite — entity+0x166/Flag 0x800 existieren NICHT, Render skaliert nie

- Der Port kennt das Feld nicht einmal dem Namen nach: `enemy_ai_common.c:10149` notiert wörtlich
  „`+0x166=0xccc` (unidentified consumer, no port field, OPEN)" — beim STAGE2-RE gesehen,
  nie als Scale identifiziert.
- `re15_actor_t` hat kein Scale-Feld; `re15_skel_compute_pose` (engine/src/skeleton_common.c:191)
  komponiert die Bone-Kette ohne Scale; alle Render-Pfade komponieren `yaw × pose.rot` /
  `yaw × pose.trans + weltpos` ohne jeden Scale-Schritt:
  - PC In-Game-NPC/Gegner: `platform/pc/main.c:7184-7289` (nyaw-Aufbau @7184-7186 = das
    RotMatrix-Äquivalent; genau HIER fehlt das ScaleMatrix-Äquivalent).
  - PC weitere Pfade: main.c:1520-1553 (Player-Select), 6253ff (Player in-game).
  - PSX-Target: `platform/psx/src/mesh_psx.c:788-839` (gleiche Komposition).
- Modell-Lade-Pfad ist KORREKT: kein falsches EMD (`pc_enemy_read_re15_emd` main.c:556-570 lädt
  CDEMD0.EMS Blob idx 12 via `s_ems_order` re15_ems.c:63-67; Split-EM27.EMD existiert nicht).
  Das Modell ist bauartbedingt „zombie-hoch" — die Bühnengröße kam im Original NUR vom 1.7×.
- Der Port-INIT des Gorillas (`re15_maggot_ai_tick` state 0, enemy_ai_common.c ~8163ff) portiert
  HP/Clip/Steering, aber nicht die beiden Scale-Stores (+0x166, Flag 0x800) aus FUN_80116f50.

### Folgewirkungen (für den Fix mitdenken, alle aus demselben Mechanismus)
- Die AI-Bone-Abfragen des Originals (Pool +0x448/+0x64c, bone-square-Angriffe) lesen Part-
  Matrizen, die mit der SKALIERTEN Root-Matrix komponiert wurden → Bone-Hebelarme im Original
  sind 1.7×. Der Port rechnet eigene FK (unskaliert) → Angriffs-Reichweiten minimal anders.
- `re15_maggot_footlock` (Locator-Foot-Plant-Rootmotion, enemy_ai_common.c:8099-8148): die
  Fuß-Verschiebung pro Frame ist im Original ebenfalls 1.7×-skaliert → der Port-Gorilla kriecht
  langsamer als das Original (passt zur alten Notiz „Crawl ~1.7").
- Schatten/Kollisionsbox +0x78 (1600×1440, re15_damage.c:2220) werden im Original separat
  gesetzt und NICHT über +0x166 skaliert — nicht anfassen.
- Weitere +0x166-Nutzer im Spiel (gleicher Mechanismus, andere Werte): Zombie-INIT STAGE1
  FUN_80109554: 0x1000 (1.0); STAGE2 FUN_80110b6c: 0xccc (0.8); Shrink-Routine FUN_80109164:
  −2/Frame Floor 100. Ein genereller Port-Fix (Scale-Feld am Actor + Anwendung im Renderer)
  deckt alle ab.
- ⚠️ Decompile-Adressen-Falle bestätigt: die Verzeichnisse `STAGE1/` und `STAGE1_overlay.c`
  sind gegenüber den ECHTEN Adressen um **−0x800 verschoben** (bekannter Audit-Befund):
  Overlay-Label „FUN_80116750" = echte 0x80116f50 (raw-file-disasm-verifiziert), Label
  „FUN_8011696c" (`param_1+0x166 = Override`) = echte 0x8011716c = der Override-TAIL derselben
  INIT-Funktion, KEIN separater Kind-Spawn. `STAGE1_full/` stimmt mit den echten Adressen überein.
  Die zwei übrigen 0x1b33-Sites (Overlay-Labels FUN_80115f58/FUN_80115f70 = echte
  0x80116758/0x80116770, direkt vor dem Root 0x80116db8) gehören zur selben Gorilla-Familie
  (Re-INIT-/Spawn-Helfer); ob auch Typ 0x29 (Blob 13, identisches 18-Bone-Rig) irgendwo mit 1.7
  spawnt, ist offen — in ROOM11C0 spawnt er nicht.

### FUND 5: SAVESTATE-LIVE-BEWEIS — beide Gorillas tragen 1.7000× im echten Spiel

`re15_ss.Ram` über die Entity-Bank DAT_800acc2c (Stride 0x1f4):
```
=== stage_saves/mzd_stage1_maggot_heavy.sav
  slot 0: type=0x42 word0=0x40000001 (0x800 clear) +0x166=0x0000
  slot 1: type=0x27 word0=0x60000801 (0x800 SET)   +0x166=0x1b33 (6963) = 1.7000x
  slot 2: type=0x27 word0=0x60000811 (0x800 SET)   +0x166=0x1b33 (6963) = 1.7000x
=== stage_saves/mzd_stage1_maggot.sav
  slot 1: type=0x27 word0=0x00000801 (0x800 SET)   +0x166=0x1b33 = 1.7000x
  slot 2: type=0x27 word0=0x00000801 (0x800 SET)   +0x166=0x1b33 = 1.7000x
```
→ INIT-Store (Fund 2) ⇒ Live-RAM (Fund 5) ⇒ Render-Konsument (Fund 1) — Kette geschlossen.
(NPC 0x42 ohne Flag/Scale = Gegenprobe: das Feld ist wirklich per-Typ, nicht global.)

## FIX-VORSCHLAG (nur Vorschlag — NICHT umgesetzt)

1. **Actor-Feld:** `re15_actor_t` (include/re15_actor.h) bekommt `int16_t render_scale_q12`
   (0 = aus, byte-true zum Gate-Flag 0x800; alternativ explizites `uint8_t render_scale_on`).
2. **INIT (byte-true, mit Belegen):** im Gorilla-INIT von `re15_maggot_ai_tick`
   (enemy_ai_common.c, state-0-Zweig ~8170):
   ```c
   e->render_scale_q12 = 0x1b33;                      /* @0x80117148/4c sh 0x1b33 -> +0x166 */
   if (e->grid_id & 0x40)                             /* @0x80117164 andi 0x40 (Spawn-Byte +0x9) */
       e->render_scale_q12 = (int16_t)((((e->grid_id & 0xf) + 10) * 0x1000) / 10);
                                                      /* @0x80117170-90 */
   ```
3. **Render (das ScaleMatrix-Äquivalent):** in `platform/pc/main.c` direkt nach dem
   nyaw-Aufbau (Zeile 7184-7186, NPC/Gegner-Pfad):
   ```c
   if (npc->render_scale_q12) {                       /* Gate @0x8001e904 andi 0x800 */
       for (int i = 0; i < 9; i++)                    /* ScaleMatrix @0x80065ff0, vx=vy=vz */
           nyaw[i] = (int32_t)(((int64_t)nyaw[i] * npc->render_scale_q12) >> 12);
   }
   ```
   Weil sowohl `nyawed_rot` (Vertex-Rotation) als auch `nyawed_trans` (Bone-Offsets) aus `nyaw`
   komponiert werden, skaliert dieser eine Multiplikator das Modell exakt wie das Original
   (Entity-Weltposition bleibt unskaliert — sie wird NACH der Matrix addiert, main.c:7289ff).
   Gleiches Muster im PSX-Target `platform/psx/src/mesh_psx.c` (~788-839) — dort kann sogar
   wörtlich `ScaleMatrix` (PSn00bSDK) auf die Root-Matrix angewandt werden.
4. **NICHT anfassen:** Kollisionsbox +0x78 (1600/1440, re15_damage.c:2220) und Schatten —
   im Original unabhängig vom +0x166-Scale.
5. **Folge-RE (separat messen, NICHT blind mitfixen):** Bone-Positions-Abfragen der AI
   (re15_maggot_bone_square Bones 6/9/10, re15_maggot_footlock-Rootmotion) lesen im Original
   SKALIERTE Part-Matrizen → Hebelarme/Foot-Plant-Deltas ×1.7. Ob der Port dort nachziehen
   muss, erst per Savestate-Vergleich der Pool-Positionen (+0x448/+0x64c) klären.
6. **Wache auf den Aufrufer** (Lehre „Pin prüft den ZUSTAND, nicht den WEG"): Pin nicht nur auf
   `render_scale_q12 == 0x1b33` nach INIT, sondern auf den RENDER-Pfad (z. B. Bone-9-Welt-Y des
   gerenderten Gorillas ≈ 1.7 × unskaliert in einem 11C0-Probe-Test).
