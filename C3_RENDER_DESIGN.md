# C3-Render — Effect-Sprite Rendering: byte-true Architektur + Port-Plan

> Ziel: die SCD-Effekt-Sprites (Muzzle, Blut, ambiente Raum-Effekte) im C-Port
> **sichtbar** machen — byte-true. Dieses Dokument ist das verifizierte RE-Fundament
> (3-Agenten-Breitensuche + eigene Disasm-Verifikation + Savestate-Pool-Dumps,
> 2026-06-30). Jede Konstante zitiert eine Disasm-Adresse / einen Save-Offset.

## 0. Der zentrale Befund — DREI getrennte Sprite-Subsysteme

Die Vorsession-Notiz „ESP-B = effect-sprite pool + AABB-cull (FUN_8004d5f0)" war eine
**Fehlattribution**. Es gibt drei separate Systeme; der Port modelliert in `re15_esp.c`
**das falsche**:

> ⚠️ **KORREKTUR (2026-06-30, nach Verifikation):** Subsystem 3 war FEHLATTRIBUIERT —
> es ist NICHT der Muzzle/Blut-Rasterizer, sondern das **sprite.pri Foreground-Mask-System**
> (`FUN_800392d4` liest `room_struct+0x24` = **`cameraStart`**, RDT-Offset 0x24, per-Cut →
> Sprite-Prioritäts-Liste). Der Port hat das **bereits byte-true portiert** (main.c:1034-1075).
> Der echte Effekt-Sprite-Gap (Muzzle/Blut) ist **Subsystem 2** (op 0x3a, model_inst_pool).

| # | Pool / Globals | Spawn / Tick / Draw | Was es WIRKLICH ist | Port-Status |
|---|---|---|---|---|
| 1 | `DAT_800b2360` (count) / `DAT_800b2368` (ptr-array) | Cull `FUN_8004d5f0` → dispatch `PTR_LAB_80074c68[type]` → `FUN_8003ee3c` (SCD-Slot-Alloc), **kein Draw** | **Status/Inventar/MAP-Screen-Sprite-Layer** (OT `0x800aa6d8`, `FUN_8004d96c`) | `re15_esp.c` ESP-B modelliert dies (Menü-Layer, nicht In-World) |
| 2 | `DAT_800a73b8` „model_inst_pool" (96 × `0x84`) | Spawn op 0x3a→`FUN_80019700`; Tick+Transform `FUN_80019e20` (`RotMatrix`); Draw `PTR_LAB_80071d40[type]` | **model_inst_pool — shared für Charakter-Modell-Render (FUN_8002c444/FUN_80045630, 12+ Spawns) UND op-0x3a-Effekt-Sprites** (Muzzle/Blut). Per-Frame neu aufgebaut (darum Saves „0 busy") | ⚠️ **DER echte Effekt-Gap** — nicht portiert |
| 3 | `DAT_800bb4d4`/`d8` + `DAT_800b2584` | Producer `FUN_800392d4` (liest `cameraStart`!) → Rasterizer `FUN_80039590` | **sprite.pri Foreground-Occlusion-Masks** (per-Cut Geländer/Kisten/Pfeiler) | ✅ **BEREITS byte-true portiert** (main.c:1034-1075, bg_pc.c Foreground-Atlas, pri_common.c, re15_pri.h) |

## 1. Subsystem 3 — der In-World-Rasterizer (PRIMÄRZIEL für C3)

### Producer `FUN_800392d4` @0x800392d4 — VOLL DEKODIERT (decompile L26-110)
Quelle: **Raum-Effekt-Command-List** `puVar10 = *(*(DAT_800ac778+0x24) + DAT_800b0fe4*0x20 + 0x1c)`
(L26) — `DAT_800b0fe4`=Szenario-Index, `DAT_800ac778`=Raum-Struct. **Das sind Raum-Daten,
statisch parsebar** (kein Live-Savestate für die Sprite-Liste nötig). Word0 = `count<<16 | flags`;
0xffffffff → keine Effekte (`*DAT_800ac778=0`, L28). Pro Sprite-Gruppe (`uVar21` Gruppen,
`*puVar11` Sub-Sprites je Gruppe) baut der Producer in **BEIDE** Buffer (A=`DAT_800bb4d4`,
B=`DAT_800bb4d8`; Init `FUN_80039270` @0x800392a0/b8) einen Record **stride 0x20**,
SPRT bei `record+0xc`, und setzt `DAT_800b2584[k] |= 1` (L57) + den Sprite-Index `+1` (L58):

| Record-Offset | Wert | decompile |
|---|---|---|
| +0xc | SPRT-Setup `SetShadeTex(blk+0xc,1)` | L76/77 |
| +0x10/+0x11/+0x12 | rgb = 0x80,0x80,0x80 (neutral-modulate) | L80-85 |
| +0x14 | x0 = `psVar13[-1] + (byte)puVar16[-4]` (cmdlist-X + UV-X-Off) | L88 |
| +0x16 | y0 = `*psVar13 + (byte)puVar16[-7]` | L87 |
| +0x18 / +0x19 | u0 = `(char)*puVar3`, v0 = `*(puVar16-9)` | L91/92 |
| +0x1a | **clut = `0x7800`** | L99/100 |
| +0x1c / +0x1e | u2/w = `uVar17`, v2/h = `uVar18` | L95/96 |

UV-Mode (L64): `(puVar16[-2] & 0xf000)==0` → 6-short-Sprite (eigene u/v Paare); sonst
`uVar17 = (puVar16[-2]>>0xc)<<3` (4-short-Sprite, abgeleitete Größe). Command-List-Cursor
`psVar13 += 4 shorts` je Gruppe (Anchor-X/Y bei +10/+12), `puVar16` = UV/Größe-Records.

### Rasterizer `FUN_80039590` @0x80039590
Walkt `DAT_800b2584` (Count = `*DAT_800ac778`); pro gesetztem Flag:
`SetSprt` / Prim-Setup, **`SetDrawMode(blk, 1,1, 0x95, 0)`** (texpage **`0x95`**),
`MargePrim`, `AddPrim` in OT `DAT_800adca4 + DAT_800aca34*0x1000 + (short@entry+2)*4`.

### VRAM-Mapping (Effekt-TIM-Upload `FUN_8004ee78` @0x8004ee78)
- texpage `0x95` → VRAM-Page **x=320, y=256**, 8-bit-CLUT, semi-trans (abr=0).
  Page-Allocator `DAT_800aca4c` init = `0x05` → x=`0x05*0x40`=320.
- clut `0x7800` → VRAM **x=0, y=480** (`DAT_800aca4d + 0x1e0`).
- Eine Sprite-id wählt **UV** über die per-Instanz-Command-List (Producer) und
  **VRAM-Page/CLUT** über `DAT_800aca4c/4d` (von `FUN_8004ee78` befüllt).

### Savestate-Befund (mzd_stage1_briefing.sav, RAM-Dump 2026-06-30)
- model_inst_pool (`0x800a73b8`): **0 busy** — keine 3D-Model-FX in STAGE1.
- map-sprite count (`0x800b2360`): **0** — Menü-Layer aus.
- `DAT_800b2584`: **~11 Flags gesetzt** (idx 1,6,21,22,25,26,33,37,38,48,49) —
  **schon im reinen Briefing-Raum ohne Combat** → das sind **persistente Raum-Effekt-Sprites**
  (kein Feuern nötig, byte-true Live-Daten verfügbar).
- Prim-Buffer-Base (`DAT_800bb4d4` → **`0x80124d14`** im Save): Producer-Output ist laut
  decompile **stride 0x20** (`iVar12+=0x20`), SPRT@+0xc — siehe §1-Tabelle. (Das im rohen
  Save-Dump gesehene stride-0x34/code-0x34-Muster war eine ANDERE Struktur — vermutlich die
  Command-List-Quelle bzw. ein stale Frame-Buffer, NICHT der Producer-Output; der Code ist
  maßgeblich.) rgb=0x80 neutral + clut 0x7800 = der Producer-Fingerabdruck.

## 2. Port-Render-Pfad (Andock-Punkte, aus `re15_port`)
- Frame-Order: `re15_render_begin_frame` → BG-Blit → 3D-Meshes → `re15_render_end_frame`
  (`platform/pc/main.c:768-2805`).
- Texturierte Quads: `re15_render_textured_tri[_lit]()` (`render_pc.c:873/968`) → `s_textri_queue`
  → depth-sort + `SDL_RenderGeometry` batched per TIM-Slot (`render_pc.c:450-535`).
  **Quad = 2 Tris** (Caller-Split, `main.c:2250-2304`).
- Effekt-TIM ist **noch nicht GPU-resident** — ESP-A parst nur `tim_off` (File-Offset).
  Upload-Pfad: `re15_tim_parse(rdt + tim_off)` → `re15_render_pc_*_bind_tim_slot`.
- SCD-Sprite-Ops sind Stubs: `op_sce_espr_on` (0x3a, `scd_vm.c:2839`),
  `op_sce_espr_kill` (0x4c, :2849), `op_sce_espr_control` (0x52, flag-predicate :3008).

## 2b. op-0x3a Draw-Pfad — STATISCH FERTIG RE'd (2026-06-30): model_inst-Partikel-System
Verifiziert: Subsystem 2 ist ein **Partikel-/Sub-Objekt-System**, kein einfacher Sprite.
- **Spawn** `FUN_80019700` (alive-tag **3**) + **Sub-Spawn** `FUN_800199d4` (IDENTISCHE Struktur,
  alive-tag **0xa**) → `DAT_800a73b8` (96×0x84). Effekte spawnen über `FUN_80045024`/op 0x3a;
  per-Entity-STATE-Spawner `FUN_8002c444` (liest `entity+0x5`=FSM-State → `FUN_80045024(0x2060000,
  entity+0x34)` = Kat 2/idx 6/Position).
- **Tick** `FUN_80019e20` (jeden Frame, walk stride 0x84, gate flag `+0x6c`): pro Instanz
  3D-Transform (`RotMatrix` aus slot[0x10/0x11+0x17/0x12]-Winkeln, `ApplyMatrix` slot[0x1a/0x1c/0x1e]-
  Translation → World-XYZ in **slot[0x14/0x15/0x16]**), dann **`PTR_LAB_80071d40[slot[1]]()`** (Draw/Anim-
  Dispatch, 24 Handler @0x80017248-0x80017ed8), dann Velocity-Integration (slot[0x10]+=slot[0xc]…) +
  Keyframe-Advance (slot+0x6d Timer, slot+0x3c Anim-Daten, 8-Byte-Entries).
- **Handler `PTR_LAB_80071d40`** (verifiziert [3]/[7]/[8]): Animations/State-Controller —
  [7]=`FUN_800174e4` Keyframe-Interpolator (kopiert 0x28-Byte-Keyframe `slot+0x6f*0x28+slot[0x20]`
  in den Slot-Transform-Kopf via unaligned-memcpy); [3]=Timed-Wait+Keyframe; [8]=Sub-Spawn
  `FUN_800199d4`+Keyframe. Sie operieren auf `DAT_800b52c4` (= aktueller Slot, Ghidra: „attack_workstruct").
- **Der per-Instanz Pixel-Draw ist DATENGETRIEBEN:** `slot[1]` (aus dem Bank-Clip-Block slot+0x00..0x1c)
  wählt den Primitiv-Handler. Welcher Index der Muzzle/Blut-Effekt nutzt + welcher Primitiv-Typ/UV →
  **steckt in den Effekt-Bank-Daten** (`DAT_800b2248[cat]`/`DAT_800b22d4[cat]`), die statisch ohne
  einen konkreten geparsten Effekt-Bank bzw. ein Live-Capture NICHT auf „der Muzzle = Handler N" pinbar
  sind. → **genau hier liegt der Live-Capture-vs-statisch-Fork.**
- **Port-Konsequenz:** op-0x3a-Effekte sind animierte texturierte Partikel-MODELLE (3D-projiziert +
  texturierte Primitive aus der Effekt-TIM @VRAM(320,256)), gezeichnet durch die Modell-Pipeline —
  ANDERS als der Port-Skeletal-Pfad. Port = eigenes Partikel-Subsystem (Pool + Tick/Transform +
  Projektion → textri-Quad mit Effekt-TIM), positioniert per Owner-Transform.

## 2c. EFF-Bank-Clip-Format (empirisch aus ROOM1140.RDT, 2026-06-30)
Die op-0x3a-Effekt-Bank = RDT+0x4C `effectStart` (=0x11e0), die `ESP-A` (`re15_esp.c`) bereits
parst (EFF-Bodies @0x11E8 effect-05, @0x13B8 effect-07; verifiziert). EFF-Body-Layout
(parsed real bytes):
- `word0` (+0): `count_a (lo16) | count_b (hi16)`. effect-05: count_a=9, count_b=29.
- **Bank-Header** (8 B @+4) = `FUN_80019700` bankhdr[+4]/+6/+8/+10: `{clut≈0x7840, V/0, clipPtr=0x0100,
  params=0x1801}`. (bankhdr[0]=count_a treibt `t9=count_a*4+4`.)
- **Anim-Records** (8 B, @+0xc..): `{U(2), V(2), clipRef f3(2), params f4(2)}`; f3 läuft in 4er-Schritten
  (0x0401,0x0405,0x0409,…) = Frame→UV-Cell-Index; f4=0x1801 (Dauer/Flags).
- **UV-Cells** (4 B, count_b Stück, @+0x4c): `{u, v, w, h}` = Textur-Rechtecke in der Effekt-TIM;
  U-Grid 0/24/48/72/96…, W/H ≈ 232/240, V=0 (eine Zeile). effect-07: U-Grid 0/32/64/96… W/H=240.
- **Parse-Schleife `FUN_80019700` DEKODIERT** (@0x800198a4-0x800199b8; Agent A's „*0x14" war falsch):
  - `slot+0x7c = bankhdr + (count_a*4+4)*2` (Ende der Anim-Record-Tabelle, t9=count_a*4+4).
  - Walk zu Sub-Index N (t4 aus Offset-Tabelle): `a0 += *(u16)a0 * 0x28 + 4` pro Record
    (Stride = `count*0x28 + 4`, 0x28=40-Byte-Sub-Element).
  - Am Ziel: `a0 += 4` (Count-Header) → `slot+0x80 = a0` → **32-Byte-Geometrie-Block** kopiert
    (`lwl/lwr/swl/swr` unaligned bzw. `lw/sw` aligned) → `slot+0x00..0x1c` (enthält slot[0]/slot[1]
    Dispatch-Indizes + Geometrie/UV).
  - ⚠️ Verbleibend für den Port: die **Semantik des 32-Byte-Blocks** (was slot[0]/slot[1]/Geometrie als
    Primitiv zeichnen — der datengetriebene Draw aus §2b) + das f4-params-Encoding. Der Parser-Walk selbst
    ist jetzt byte-true bekannt.
- Effekt-TIM: `out->eff[i].tim_off` (ESP-A) → TIM @0x1A628/0x1CA68; GPU-Upload nötig.

## 2d. DER ECHTE ROOM1140-EFFEKT-TRIGGER — präzise lokalisiert (2026-06-30)
**ROOM1140 feuert KEIN op 0x3a** (korrekt disassembliert: main 1 / sub 6 / extra 2 Scripts = 0× op 0x3a/0x36/0x4c; die Roh-Scan-Treffer @0xd99/dcd/df9 waren Daten-Fehlalarme). Die Effekte 05/07 werden vom **STAGE1-Overlay-Zombie-AI-Code** gespawnt:
- `FUN_8001945c` indexiert `DAT_800b2248[id_VALUE*4]` → ROOM1140 belegt nur `[0x05]` und `[0x07]` (nach ID-Wert). cat 0/2/6 (andere Overlay/`FUN_8002c444`-Spawns) sind in ROOM1140 unbelegt → gehören zu anderen Räumen.
- **Effekt-ID 5 Spawner (verifiziert):** `FUN_801066dc`, `FUN_80106a44`, `FUN_80104b40`, `FUN_80106edc`, `FUN_80107244`, `FUN_80107ee0` (alle STAGE1) rufen
  `func_0x80019700(0x5002800, *(s16)(entity+0x6a), <bone_pos>, &LAB_8012016c)` —
  a0=`0x5002800`: effect-id **5**, sub 0, sprite 0x2800; Position `<bone_pos>` =
  `LAB_8011f784[entity+8]*0xac + *(entity+0x188) + 0x40/+0x10` (Bone-Daten); a3=`&LAB_8012016c` (gemeinsamer Effekt-Handler).
- Gespawnt beim **Verhaltens-Setup** (`entity+7` 0→1-Übergang) zusammen mit 2× effect-id-0 (`0x2000`). `FUN_8002c444` (effect-id 6 12-fach-Burst) = ANDERER Raum/Kinematik.
- **Port-Status:** der Port hat eine VEREINFACHTE Zombie-AI (`enemy_ai_common.c`: INIT/ACTIVE/HURT/DEATH/IDLE) OHNE diese Verhaltens-Sub-Handler. **Nächster Port-Schritt = diese AI-Verhaltens-Handler portieren** (FUN_801066dc/a44/… + die Zombie-Internals entity+7/+8/+0x188/+0x8f/+0x93… + der Effekt-Handler LAB_8012016c), dann den Effekt-Spawn byte-true einklinken. Substanzielle, eigene Port-Einheit (Teil des 100%-Ports, siehe Memory full-port-mandate).

## 2e. Zombie-Gore-Port — Fortschritt + die nächsten Bricks (2026-06-30)
**GELIEFERT (Commit e4fca663, getestet):** `re15_enemy_gore_tick` = byte-true `FUN_80106a44`
@0x80106a98-abc (der `+0x93 & 2`-Hit-Spawn): pro Live-Zombie in `re15_enemy_ai_run_all`, wenn
das 2.-Hit-Bit (von `re15_enemy_take_damage` @0x80012fcc gesetzt) an ist → spawn effect-id 0
(`a0=0x2000`, `a1`=rot_y) + Bit löschen. Position = Actor-Welt (faithful, Bone-Block +0x188 nicht
im Port). Bank-gated: effect-id 0 ist in ROOM1140 unbelegt → no-op dort (byte-true); sichtbar wo
effect-0 geladen ist.

**NÄCHSTE BRICKS (für die SICHTBARE ROOM1140-Gore = effect-id 5 am Behavior-SETUP):**
**GELÖST + GELIEFERT (Commit a405f446, getestet):** der effect-5-Dispatch ist
`(*(code*)(&PTR_LAB_8011fe30)[entity+0x5])()` = die EFFEKT-Aspekt-Tabelle, indiziert per
**`entity+0x5` = sub_state_1 = die ANIM-PHASE**. `0x8011fe30 + 0x58*4 = 0x8011ff90 = FUN_80106edc`
→ **Trigger = Anim-Phase `0x58` (88)**; die Effekt-Handler stehen an den hohen Phasen 79-96 = das
ENDE einer langen Zombie-Animation (Gore-Höhepunkt). `re15_enemy_gore_setup` = byte-true `FUN_80106edc`
(2× effect-0 + 1× effect-5, gated sub_state_1==0x58 + sub_state_3==0 + rand&1), in `run_all` verdrahtet.
**LETZTER BRICK zur sichtbaren Gore — PRÄZISIERT:** `sub_state_1` (entity+5) ist eine Behavior-State
(0x00-0x7b), NICHT ein freier Frame-Counter. Die volle `PTR_LAB_8011fe30`-Tabelle (in STAGE1.BIN
dekodiert): States 0x00-0x14 = niedrige Behaviors (`0x80105a24/a4c/a6c/a2c`); **Gore-States 0x57-0x65**
(`0x57→0x80108abc`, `0x58→FUN_80106edc effect-5`, `0x5c→FUN_80107244`, `0x60→FUN_80107ee0`) = die
**Death-Animations-Sequenz**. Die WALKING-Behavior `FUN_80105b7c` (Phase via entity+7) setzt entity+5
nur auf NIEDRIGE States (0x02/0x07/0x08/0x13 via State-Word `0x10201`/`0x801`/`0x701`/`entity+5=0x13`)
— **nie 0x58**. → Der Zombie erreicht 0x58 nur über den **DEATH-Handler** (Zombie-Main-Dispatch
@0x8011f7b4[3] = `FUN_8011db88`, im Port DEFERRED/state 3). **NÄCHSTE EINHEIT = den Zombie-Death-State
porten** (`FUN_8011db88` + die Death-Anim-Sequenz, die entity+5 in 0x57→0x58→… setzt) → dann feuert der
schon-verdrahtete `re15_enemy_gore_setup` und die effect-5-Gore ist beim Zombie-Tod SICHTBAR. Das ist
Teil des Zombie-AI-Ports (Death-Body war schon vorher deferred, siehe enemy_ai_common.c:119).

1. **Behavior-Dispatch mappen — ERLEDIGT (siehe oben). Historischer Stand:**
   - Der Top-Level Zombie-Dispatch ist `(*(code*)(&PTR_FUN_8011fXXX)[*(byte*)(entity+5)])()` —
     **indiziert per `entity+0x5` (sub_state_1, Anim-Phase)**, ~30 stride-**4** PTR-Tabellen
     @`0x8011f840`–`0x8011f9dc` (STAGE1_full/_ALL_combined.c:292-393).
   - **ABER die effect-5-Handler (`FUN_80106edc`/`80107244`/`80107ee0`) stehen NICHT in den
     entity+5-Tabellen** — sie sind in stride-**0x10**-STRUCT-Tabellen @`0x8011ff90` und `0x80120720`
     (Handler @entry+0 UND @entry+0xc=`0x80107634`/`0x80108abc`). Kein direkter `lui 0x8012`-Caller
     gefunden → diese Tabellen werden über einen **Sub-Dispatch / einen in einem Global gespeicherten
     Pointer** erreicht, NICHT entity+5 direkt. **DAS ist der nächste RE-Schritt:** finden, was die
     stride-0x10-Tabelle indiziert (Kandidaten: entity+0x8 sub_state_4, entity+0x94, eine Behavior-ID)
     und welche entity+5-State dorthin führt. (Workflow `zombie-fx-dispatch-re` war angesetzt, scheiterte
     am Session-Limit; bei Reset erneut laufen lassen — 3 Tasks: dispatch-chain / FUN_80106edc / port-state.)
   - `FUN_80106edc`/`FUN_80106a44` (STAGE1/.c) = die effect-5-Setup-Behaviors; Entry `entity+7==0`.
   - **STAGE1.BIN-Scan-Befund (2026-06-30):** die stride-0x10-Tabellen haben Basen @`0x8011fe30`/
     `0x8011fe84`/`0x8011fe94`/`0x8011fe9c`/`0x8011feac` (referenziert von den Verhaltens-Funktionen
     `FUN_80105c5c`/`80106xxx` via `lui 0x8012;addiu`). Die Effekt-Config-Blöcke `LAB_8012016c`/
     `0x8012017c`/`0x8012018c` (= a3-Arg der Spawns, der „Effekt-Handler") werden von ~20 Funktionen
     referenziert (`0x80106ce4`/`80106fe4`/`80107310`/…) — auch die zu portieren. **Nächster konkreter
     Schritt:** eine dieser Verhaltens-Funktionen (z.B. `FUN_80106ce4`, referenziert `0x8012016c`)
     disassemblieren/decompilen → wie sie die stride-0x10-Tabelle indiziert (welches entity-Feld) +
     welche entity+5-State dort hinführt → dann den effect-5-Setup byte-true an die Port-State hooken.
2. **Den Behavior-SETUP porten** (`FUN_80106a44:17-34`): wenn der Zombie diese Behavior betritt
   (`entity+7`==0 → der Port hat `sub_state_3`), spawn **effect-id 5** (`a0=0x5002800`, a1=rot_y,
   pos=bone→Actor faithful) + 2× effect-0, und setze die Setup-Felder (entity+0x9c=4, +0x8f=7,
   +0x93|=1, +0x1b8=1 …). → SICHTBAR in ROOM1140 (effect-5 ist in der Bank).
3. **Den Rest von `FUN_80106a44` + die Geschwister** (`801066dc/104b40/106edc/107244/107ee0`) +
   `LAB_8012016c`/`8012017c` (Effekt-Handler-Configs) byte-true porten → die volle Briefing-Zombie-
   Verhaltens-AI (Teil des 100%-Ports, [[reai-v2-full-port-mandate]]).

## 2f. KORREKTUR (2026-07-01, Workflow+Verifikation): der Effekt-Dispatch ist eine Master-Tabelle, entity+5 wird ON DAMAGE gesetzt
Wichtige Korrektur zu §2c-§2e (die „Anim-Phase/Death-Sequenz"-Deutung war falsch):
- **`entity+0x5` (sub_state_1) wird ON DAMAGE gesetzt**, von genau zwei Funktionen (die EINZIGEN Writer des
  Enemy-+5-Bytes in EXE+Overlay): `FUN_80012d60`:78 (Melee) `entity+5 = DAT_8006f430[hit_zone]`,
  `FUN_80011f50`:165 (Spieler-Schuss) `entity+5 = weapon_id`. Gleicher Block: `entity+4=2`(hurt)/`=3`(death),
  `entity+6=hit-dir-col`, `entity+7=0`. **Der Port macht das schon byte-true** (`re15_react_table[11]` ==
  `DAT_8006f430` = `[03,03,09,0A,0B,0E,0F,10,11,12,14]`, gesetzt @`re15_enemy_take_damage:270`).
- **Der Effekt-Dispatch ist eine 2D-MASTER-TABELLE @0x8011fb90**, `record=u32 code-ptr`, row-stride 0x20 (8 cols),
  col-stride 4, indiziert `[entity+5 row][entity+6 col]`. Drei Views: HURT `FUN_80105a8c` (base 0x8011fb90,
  verifiziert: `entity+5<<5 + entity+6<<2`), DEATH `FUN_80106ba4` (base 0x8011feac), ACTIVE flat
  `PTR_LAB_8011fe30` (= master row 0x15 @0x8011fe30). Zombie-Main-Dispatch @0x8011f7b4[entity+4]:
  [0]init FUN_80100688 / [1]active FUN_80101224 / [2]hurt FUN_80105a8c / [3]death FUN_80106ba4.
- **`entity+5` des ROOM1140-Zombies ∈ {3,9,0xa,0xb,0xe,0xf,0x10,0x11,0x12,0x14}** (aus DAT_8006f430). Master-row 3 =
  `FUN_80105b7c` (spawnt effect-0 0x2000); rows 9/…/0x14 col0 = NULL. **effect-0 ist NICHT in ROOM1140s Bank (05/07)**.
  Der **effect-5-Handler `FUN_80106edc` ist master-row 0x20 (entity+5==0x20/flat 0x58) → ein HÖHERER/ANDERER Enemy**,
  NICHT der ROOM1140-m0-Zombie (der maxt bei 0x14). → **meine Gore-Bricks (effect-0 Tick / effect-5 Setup) sind
  byte-true, aber treffen ROOM1140 nicht.**
- **Fazit:** die sichtbare ROOM1140-Zombie-Gore hat KEINEN sauberen Pfad über diese Mechanik (Zombie spawnt
  effect-0/NULL, nicht die Room-05/07). Die Room-Effekte 05/07 sind wahrscheinlich kinematisch / für einen anderen
  Enemy. **Nächste Einheit (byte-true, korrekt, per Mandat): den Master-Tabellen-Effekt-Dispatch porten**
  (`FUN_80105a8c` HURT + die row-Handler) — die richtige Mechanik, auch wenn die ROOM1140-Sichtbarkeit von der
  Effekt-ID↔Bank-Zuordnung abhängt (die noch zu klären ist: lädt das Original einen globalen Bank für effect-0?).

## 3. op 0x3a `Sce_espr_on` (Spawn, Subsystem 2) — byte-true (16 Bytes)
`FUN_80019700(a0,a1,a2,a3)` spawnt in `DAT_800a73b8` (verifiziert: `addiu s3,s3,29624`
@0x80019724; stride 132 @0x80019794-9c; 96 Slots @0x8004978c; busy `+0x6c`, alive=3;
full→`0xff`). Kategorie `pc[4]&0xff` wählt Owner `a2`: 0=`0x80072d4c`, 1=`0x800aca74`,
2=`entity[pc4>>8]` (stride 0x1f4), 3=`0x800b3fb8+idx*0x94`, 4/5=unbenutzt. a2[0..7] →
slot+0x4c..0x68 (Owner-Transform-Snapshot); pc+8/10/12 → slot+0x40/44/48 (lokaler Offset);
UV-Seed slot+0x32/+0x30 aus Bank-Header (`DAT_800b2248[cat]`). Return = Slot-Index.

## 4. Implementierungs-Plan — RETARGET auf Subsystem 2 (op 0x3a Effekt-Sprites)
Subsystem 3 (Foreground-Masks) ist erledigt; der echte „sichtbare-Effekte"-Gap = die
SCD-gespawnten Effekt-Sprites (Muzzle/Blut) via op 0x3a → `FUN_80019700` → `DAT_800a73b8`.
Architektur-Hürde: der Pool ist shared mit Charakter-Modell-Render (das der Port ANDERS macht),
und es gibt KEIN Live-Save mit aktivem Effekt (Pool per-Frame transient). Plan:
1. **op-0x3a Draw statisch RE'en** — `FUN_80019e20` (Tick+`RotMatrix`) + `PTR_LAB_80071d40[type]`-
   Dispatch: wie ein model_inst-Slot zum gezeichneten Sprite/Quad wird (3D-Projektion + UV aus
   dem Bank-Header `DAT_800b2248[cat]`, Felder slot+0x32/+0x30).
2. **Live-Daten** — Savestate mit aktivem op-0x3a-Effekt (Pistole feuern, mid-frame capturen)
   ODER rein statisch porten (byte-true-Risiko ohne Live-Verifikation klar kennzeichnen).
3. **Port-Spawn** — `op_sce_espr_on` (Stub→echt): port-seitiger Effekt-Slot (Position aus
   Owner-Transform `a2[0..7]` + lokalem Offset pc+8/10/12; UV aus Effekt-Bank/TIM); `op_sce_espr_kill` despawn.
4. **Draw + TIM** — `re15_esp_effect_draw_frame()`: pro Slot 3D-Position projizieren → textri-Quad
   mit Effekt-TIM-UV; Effekt-TIM GPU-Upload (`re15_esp_parse`-`tim_off` → TIM-Parse → Slot).

## 5. Verifizierter Port-Status der drei Subsysteme
- **Subsystem 3 (Foreground-Masks) = ✅ erledigt** (main.c:1034-1075, `re15_pri_*`, bg_pc.c-Atlas).
- **Subsystem 1 (Menü-Sprites):** `re15_esp.c` ESP-B (`s_esp_pool`, `re15_esp_run`=`FUN_8004d5f0`)
  modelliert dies — Menü-Layer, kein In-World-Effekt (für C3 irrelevant).
- **Subsystem 2 (op 0x3a Effekt-Sprites) = der offene Gap** (siehe §4). ESP-A-Parser
  (RDT-EFF/TIM, RDT+0x4C) bleibt korrekt und liefert die Effekt-TIM-Daten.
