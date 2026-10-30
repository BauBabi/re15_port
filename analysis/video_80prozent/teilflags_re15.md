# RE1.5 model-part flags: the EXE consumers (PSX.EXE), compared with RE2 and the port

Research only. No repo files were changed.
Sources:
- `re15_disasm.py dis/read` on `info/Re1.5/PSX.EXE` and `STAGEn.BIN`;
- own jal/ori/andi scanners in the scratchpad folder `pf/` (`lib.py`, `orisw.py`, `andi*.py`, `partrd.py`);
- the decompiles `RE_15_Quellcode_V2/FUN_8001e8c8 / e9ec / ef54 / f024 / f1e0 / f220 / 8002441c / 80023410 / 80023708 / 80023a58 / 80022da0 / 8001f3bc`, all re-checked against the disasm;
- for RE2: `RE2_Quellcode_V2/FUN_80027434.c`, `FUN_8002940c.c`, and the port code in `enemy_ai_re2_zombie.c` and `main.c`.

## 0. Where it runs: the frame order

- **Logic first.** The frame function calls the entity AI loop FUN_8001a50c at `jal 0x8001a50c` @0x8001ce04. That loop runs the overlay handlers, which write the part flags.
- **Draw second, in the same frame function:**
  - the player: `addiu a0,s0,720` (= 0x800aca54), then `jal 0x8001e8c8` @0x8001d09c;
  - each enemy with word0&1: `andi v0,v0,0x1` @0x8001d0fc, then `jal 0x8001e8c8` @0x8001d108.
  - g_entity 0x800ac784 is set to the drawn entity first (`sw a0,0(s0)` @0x8001d0a0, @0x8001d0cc).
- **So the per-part matrix composition, the part physics and the shard animation all live in the DRAW pass.** They run once per drawn frame, after that frame's AI tick.
- **FUN_8001e8c8(ent)** does the following:
  - `RotMatrix(ent+0x68 → ent+0x20)` @0x8001e8f4.
  - If ent.word0&0x800, it scales the entity matrix by (ent+0x166)×3 (@0x8001e904-940).
  - It sets the light reference to ent+0x34 (`jal 0x80053fc0` @0x8001e94c).
  - **Visibility test** FUN_80014368(ent+0x34, *0x800ac790) @0x8001e974:
    - result != 0 → `FUN_8001e9ec(part)` for every part (compose + draw), @0x8001e990;
    - result == 0 → `FUN_8001ef54(part)` (compose + physics, **no draw**), @0x8001e9b4.
  - The parts are walked flat: `addiu s1,s1,172`, with the count from `lbu s0,131(s0)` = ent+0x83.
  - **There is no parent cascade and no tree cut.**

## 1. Part record (stride 0xAC), fields that FUN_8001e5b0 initialises

FUN_8001e5b0 is called @0x80031624 / 0x80031888 (player) and @0x80042530 (enemy spawn).
- `+0x00` word0 = 1.
- `+0x04..+0x6b` come from a template:
  - local matrix +0x18 = identity;
  - the template's word at +0x68 = 0x808080. In the decompile this is `local_54`, which sits at local_b8+0x64, so it lands at part+0x68.
- `+0x08` / `+0x10` = mesh pointers (ent+0x10 + k*0x38, plus +0x1c). `+0x74` (byte) = part index.
- `+0x2c/+0x30/+0x34` (int) = local translation, the EMR relative position. Its s16 copy goes to `+0x38/+0x3a/+0x3c`.
- `+0x6c` = parent world matrix pointer (root: ent+0x20; child: parent+0x40). `+0x90` = parent record (root: the entity).
- **+0x88..+0x9f are NOT initialised by FUN_8001e5b0.** The overlay producers write them.
- **Reset helper FUN_8001ee68(ent)** (@0x8001ee68-ef4c) does three things per part:
  - `sw s3(=1),0(s1)` = word0 = 1;
  - `sw s2(=0x808080),104(s1)` = +0x68 = 0x808080;
  - FUN_8001f278, which recomposes the matrix unless 0x40 is set.

  It does **not** clear +0x82..+0x9f.

  Callers: STAGE1 @0x80100fe0 (zombie init), STAGE2 @0x80100e74, STAGE3 @0x801010cc and @0x801168fc, STAGE4 @0x80100f94, STAGE5 @0x80101114 and @0x80117110.

## 2. Per-part compose + draw: FUN_8001e9ec (on screen) and FUN_8001ef54 (off screen)

All addresses below are FUN_8001e9ec. The FUN_8001ef54 counterparts are in brackets.

| Order | Instructions | Meaning |
|---|---|---|
| 1 | `andi v0,v0,0x40` @0x8001ea10 / `bne` → skip; `jal 0x80022da0(*(+0x6c), part+0x18, part+0x40)` @0x8001ea24 [ef54: @0x8001ef70 / ef80] | **0x40 = keep your own world matrix.** Without it: +0x40 = parentWorld ∘ local. FUN_80022da0 = CompMatrix: R = Rp·Rl, t = Rp·(s16)tl + tp |
| 2 | `andi 0x800` @0x8001ea34 (not in ef54) | Translation of the local matrix scaled by `(s16)parent(+0x90)+0x88/+0x8a/+0x8c >>12`, then recomposed (also only if !0x40) |
| 3 | `andi 0x20` @0x8001eb14 → `jal 0x8001f024(part, part+0x40)` @0x8001eb24 [ef54: @0x8001ef90 / ef9c] | **Flight physics**, see §3 |
| 4 | Light matrix = 0x80076d14 · world (@0x8001eb3c / 44). `andi 0x1000` @0x8001eb54 → light reference = part+0x54 (`jal 0x80053fc0` @0x8001eb60), then back to ent+0x34 @0x8001eb88 | 0x1000 = the part is lit from its own position |
| 5 | `jal 0x80022da0(0x800b5288 camera, part+0x40, sp+16)` @0x8001eba0 | View × world |
| 6 | `andi 0x400` @0x8001ebb0 → `ScaleMatrix(view·world, {(s16)+0x88, +0x8a, +0x8c})` @0x8001ebdc | **0x400 = scale along the LOCAL axes.** ScaleMatrix @0x80065ff0 does m[i][j] = m[i][j]·v[j] >> 12 (column scale). Only the drawn matrix is scaled: +0x40 is untouched, so the children are NOT scaled |
| 7 | `andi 0x2000` @0x8001ebec: S = ScaleMatrix(identity @0x80072d4c, {+0x88, +0x8a, +0x8c}) @0x8001ec64; `MulMatrix0(S, world, S)` @0x8001ec74; t = world.t (part+0x54/58/5c, `lw 20..32(s1)` @0x8001ec7c-98); then camera·that @0x8001eca4 | **0x2000 = scale along the WORLD axes** (rows of R). The translation is not scaled |
| 8 | SetRot/SetTransMatrix @0x8001ecac / b4 | |
| 9 | `andi v0,v1,0x1` @0x8001ecc4 / `beq` → return | **Bit 0 = draw enable** (flat, per part). In ef54 the bit also gates the physics calls below (@0x8001efac) |
| 10 | `(w&0x18)==0` @0x8001eccc: normal mesh, `FUN_800254a0(tris, pkt, &part+0x68, 0)` @0x8001ed04, `FUN_800256b0(quads, …)` @0x8001ed34; then `andi 0x80` @0x8001ed44 → `FUN_8002441c(part+4, +0x94, +0x98)` @0x8001ed58 [ef54: @0x8001efc0 / efd4, which blends without drawing] | **+0x68 = colour word**: `cd = a3<<1 \| 0x34`, `ldrgb` + NCCT, so it multiplies the lighting result (identical to RE2's +0x70). a3 is hard-wired to 0, so there is never semi-transparency. **0x80 = tint, see §4** |
| 11 | `w&0x08` @0x8001ed68: `FUN_80023410` (tris) @0x8001ed90, `FUN_80023708` (quads) @0x8001edbc, `FUN_8001f1e0` @0x8001edc4 [ef54: only `jal 0x8001f1e0` @0x8001efec] | **0x08 = burst into shards, see §5.** The normal mesh is NOT drawn; 0x08 takes precedence over 0x10 |
| 12 | `w&0x10` (and !0x08) @0x8001edd4: `FUN_80023a58` / `FUN_80023e90` (lit, per-vertex Y sag clamped to the floor) @0x8001ee08 / ee38, then `FUN_8001f220` @0x8001ee40 [ef54: @0x8001f004] | **0x10 = melt, see §6** |

**Bit 0x02 has no consumer in the RE1.5 EXE.**
- Every `andi …,0x2` (and every mask with bit 1) after a `lw/lh/lhu rX,0(rY)` in the EXE belongs to an entity, pad or other struct: 0x8002caec, 0x8002cff4, 0x80032620, 0x80020db4 and 0x800306e8 were checked one by one.
- No overlay tests word0&2 at a k*0xac offset either (`partrd.py`: the only part-word tests in all STAGE BINs are `andi 0x41` at STAGE2 @0x80114218 / @0x801147f8).
- In RE2, a separate word +0x04 bit 2 drives semi-transparency (`*(puVar17+0x40)=param_2[1]`, `(uVar2&2)>>1` in RE2 FUN_80027434.c:160). RE1.5 passes the constant `addu a3,zero,zero` @0x8001ece0.
- So in RE1.5, 0x02 is a dead marker: the 0x4a and 0x1062 producers set it, and nobody reads it.

## 3. 0x20 flight physics: FUN_8001f024(p, M=p+0x40), @0x8001f024-0x8001f1dc

```
M.t[0] += (s16)p+0x38                                @8001f03c-4c
p+0x3a = (s16)(p+0x3a + (s8)p+0x75)                  @8001f050-64   (vy += gravity)
M.t[1] += (s16)p+0x3a                                @8001f068-78
M.t[2] += (s16)p+0x3c                                @8001f07c-8c
floor = (s16)g_entity+0x1ba                          @8001f09c       (entity floor Y)
if (floor < M.t[1]) {                                @8001f0a8 slt
    M.t[1] = floor                                   @8001f0b4
    p+0x3a = (s16)(-(s16)p+0x3a) >> 3                @8001f0c0-cc  (sll16 / sra19)
    if (w & 0x10) { w |= 0x2000; p+0x88 = 10000; p+0x8c = 8000; p+0x8a = 0; w &= ~0x10; }  @8001f0d0-100
}
if ((s16)p+0x3a == 0 && M.t[1] == 0) { (s16)p+0x54 = 1; w &= ~1; }   @8001f104-134  (vanish)
if ((s16)p+0x82 != 0) {                              @8001f138-140
    p+0x82 += (s8)p+0x76                              @8001f194-1b4
    T = identity(0x80072d4c); RotMatrixZ((s16)p+0x82, T)   @8001f1b0 (0x80065b70)
    MulMatrix0(M, T, M)                              @8001f1c0       (M = M·Rz)
}
```
- With 0x40 set, M persists, so position and rotation integrate over frames. Without 0x40, M is recomposed each frame, so the term is a one-frame offset.
- The spin only starts if +0x82 was pre-set to non-zero, because the increment sits inside the `!= 0` test.
- **No wall test, no blood, no bounce budget, no blend.**

## 4. 0x80 tint: FUN_8002441c(p+4, c1 = p+0x94, c2 = p+0x98), @0x8002441c-0x800245d0

- It runs only in the normal-mesh branch (w&0x18 == 0), right after the packets are built.
- For every tri packet (stride 0x50 = 2 buffers × 0x28, count = *(*(p+8)+0x14), at `pkt+DAT_800aca34*0x28+30`):
  - rgb1 (packet +0x10..0x12) = (rgb1 + c1.rgb) >> 1;
  - rgb2 (packet +0x1c..0x1e) = (rgb2 + c2.rgb) >> 1.
  - Instructions: `lbu -14/-13/-12(t1)` + `srl 1` and `lbu -2/-1/0(t1)` + `srl 1` @0x80024488-0x800244ec.
- For every quad packet (stride 0x68): the same, on rgb1 and rgb2 (@0x80024560…).
- **Vertex 0 (and quad vertex 3) are untouched.** c1 and c2 are 0x00BBGGRR words (low byte = R).
- The blend is applied after NCCT, to the lit colour.
- RE2's FUN_8002940c is different:
  - tri rgb1 is SET to c1 (`pbVar1[-0xe] = (byte)param_2`), not averaged;
  - the quad slot set differs (rgb3 set, rgb1/rgb2 averaged).

## 5. 0x08 burst into shards: FUN_80023410 / FUN_80023708 + FUN_8001f1e0

**Per primitive (both tris and quads).** Indices are taken from the high half-words of the prim words:
- vertex 0 = +2, vertex 1 = +6, vertex 2 = +10 (+14 for the 4th quad vertex).
- The prim stride is 12 (tri) or 16 (quad).

```
k  = s16 table_80073454[(u16)p+0x9c]       @800234ec-f8 (tri) / @800237ec-fc (quad, (s16)<<1)
V  = mesh vertex of the prim's SECOND corner (offset +6)   @80023468 / @8002376c
d  = { ((s16)V.x >> 5)*k, ((s16)V.y + 300) >> 6 * k, ((s16)V.z >> 5)*k }  @80023528-78 / @80023828-78
TR' = TR + (R·(s16)d) >> 12      (MVMVA 0x4a480012, sf=1, cv=TR; d truncated to 16 bits by lhu/lwc2)  @800235a0
draw the prim with TR' (RTPT 0x4a280030); colours = (p+0x68 & 0xffffff) | 0x34000000 (GT3) / 0x3c000000 (GT4)
on all vertices → FLAT colour word, no NCCT, no NCLIP (no back-face cull); skip if (OTZ>>6)==0
```

**Table @0x80073454** (30 s16 entries, one row):

`30, 59, 87, 114, 140, 165, 189, 212, 234, 255, 275, 294, 312, 329, 345, 360, 374, 387, 399, 410, 420, 429, 437, 444, 450, 455, 459, 462, 464, 465`

This is identical to RE2's row 0 @0x8009DC28. RE1.5 has no second row.

**Frame counter.**
- `(p+0x9c)++` happens only at the end of FUN_80023708, unconditionally, even with 0 quads: `lhu v0,156(s4)` … `sh v0,156(s4)` @0x80023a00-a0c.
- FUN_8001f1e0 (@0x8001f1e0-0x8001f21c):
  - `p+0x96 += p+0x9a` (vy += vy_inc), `M.t[1] (p+0x58) += (s16)p+0x96`;
  - `if ((u16)p+0x9c >= 0x1d) word0 = 0` (`sltiu v1,v1,0x1d` @0x8001f208, `sw zero,0(a1)` @0x8001f214).
- **Result:** indices 0..28 are drawn (29 frames), then the part is gone.
- **Off screen**, ef54 runs FUN_8001f1e0 but never FUN_80023708. The counter freezes and the shards keep drifting. The lifetime only advances while the entity is visible.
- **Movement is vertical only.** There is no yaw or horizontal speed (RE2's FUN_80028DAC has +0x98 yaw, +0x9c speed, +0xa4 speed increment). Usually 0x40 is set as well (0x4a), so the base matrix is frozen at the moment of the burst.

**Producer values (STAGE1):**
- FUN_80108d04 @0x80108d04-6c (gore bone, legs, arms):
  - blood 0x2000 at part+0x40 (`jal 0x80019700` @0x80108d2c);
  - `sw 0x8f,104` → +0x68 = 0x0000008F, which is R=0x8F, G=B=0: pure dark red;
  - `sh zero` +0x94..+0x9e (@0x80108d40-54), so vy = vy_inc = frame = 0 and both tint words are cleared;
  - `|=0x4a` @0x80108d58.
- Head burst in FUN_80106edc @0x801070b0-e4: the same, except `+0x96 = -50` (@0x801070bc-c0) and `+0x9a = 3` (@0x801070c4/d0). The shards rise and then fall: vy goes -47, -44, …

## 6. 0x10 melt (documented; no RE1.5 producer found)

- FUN_80023a58 / 80023e90 draw the lit mesh with per-vertex Y offset `(s16)p+0x94`, which grows each draw by `(s8)p+0x98`. A vertex is clamped so that vy + p.t.y + offset <= 0 (floor y = 0).
- FUN_8001f220 (@0x8001f220-0x8001f274):
  - `p+0x9a--`; when it was 0: +0x98 = +0x99 = 0;
  - if (u8)p+0x9c != 0: +0x75 = 0x14, +0x82 = 0, +0x76 = 8, +0x38/+0x3a/+0x3c = 0, `w ^= 0x1062` (`xori v1,v1,0x1062` @0x8001f268). That turns on flight 0x20, detach 0x40, own light 0x1000 and the dead 0x02.
- The `ori/sw` scan over STAGE1-6 finds no part write of 0x10. The off-0 EXE hit @0x8003b504 is an entity word.
- 0x1062 is written directly at STAGE2 @0x80114220 / @0x80114264 / @0x80114800 / @0x80114844 (spider parts 1/2), and with `xori` at STAGE4 @0x80116348 / @0x80116450 / @0x80116720 and STAGE5 @0x801164c8 / @0x801165d0 / @0x801168a0 (parts 8 and 12).

## 7. Other bits

- **0x200 = freeze the local rotation from the animation.** In FUN_8001f3bc, `andi v0,v0,0x200` @0x8001f564 and @0x8001f5e0 skips `RotMatrix(p+0x78 / p+0x60 → p+0x18)`. The same test appears in FUN_8001fb94 / FUN_8001ffd8 (decompile `& 0x200`).
  - Producers: zombie torso-off, STAGE1 `ori 0x200` @0x80108714 (part 8) and @0x80108738 (part 11).
- **0x40 elsewhere.** Player init FUN_800314b0 `ori 0x40` on parts 17-20 (@0x80031bc4-bf0). Those are the hair chain whose world matrix FUN_80024e40 writes directly (@0x80024c30). This confirms "0x40 = world matrix supplied externally".
- **0x800** (part): scaled local translation, see §2 row 2.
- **0x1000:** own light reference, see §2 row 4.
- **Children follow a detached part.** Every child composes from its parent's **+0x40**: `*(+0x6c)` is the parent's +0x40 (FUN_8001e5b0). So children follow whatever is in that matrix, frozen or moved by the overlay.
  - Example: the zombie torso-off, STAGE1 @0x80108324 sets part 7 `|=0x40`. The overlay itself moves part 7's +0x54/+0x58/+0x5c:
    - vy +0x3a, gravity +0x75=3 @0x8010837c, +0x76=10 @0x80108384;
    - floor bounce against ent+0x1ba-200 @0x801085d8-614;
    - spin `RotMatrixZ(0x80)` and CompMatrix(M, Rz, M) @0x801086b8 / 0x801086cc.
  - Head and arms (parts 8-14, children of 7) fly with it.

## 8. Producers (all use the consumers above)

| Site | Parts | Write |
|---|---|---|
| STAGE1 FUN_80108d04 @0x80108d3c-5c | any part | 0x4a, +0x68=0x8f, +0x94..+0x9e = 0 |
| FUN_80106edc @0x801070b0-e4, @0x80107108-120 | head; part 7 | Head: 0x4a, vy -50, inc 3. Part 7: `|=0x80`, +0x94 = 0x80, +0x98 = 0x20 (red stump) |
| FUN_80107ee0 crit @0x80108014-98 | parts 7 and 0; part 0 or 8 | Parts 7 and 0: 0x80 with tints 0x80 / 0x20. 50% (not type 0x13): 0x400 with scale (0x5f4, 0x1000, 0x5f4) on part 0, or (0x5f4, 0xc18, 0x5f4) on part 8 @0x80108160-178. This is a **stump shrink, not a spin** |
| FUN_80107ee0 torso-off | part 7; parts 8/11; parts 1/2/4/5 | Part 7: 0x40 (@0x80108324). Parts 8/11: 0x200. Parts 1/2/4/5: 0x40 (@0x80108828-894) |
| FUN_80108abc @0x80108b9c-be4, @0x80108c00-c50 | parts 2/3/5/6; parts 1/4 | Parts 2/3/5/6: d04. Parts 1/4: 0x80 with tints 0x80 / 0x20; 50% part 4 word0 = 0 (not drawn, but still composed) |
| Zombie spawn variants STAGE1 0x80101008… (table @0x8011f7ec) | e.g. part 2 | e.g. part 2 `|=0x400\|0x80` with scale (0x7e8, 0x1000, 0x7e8) and tints 0x80 / 0x8080e0. Port: `enemy_ai_common.c:2086-2092` "OPEN" |
| Birkin throw-kill STAGE3 @0x8011b2f4-33c, @0x8011b3a4-418, @0x8011b4ac, @0x8011b54c | player parts 0, 2, 5, 1, 8 | `|=0x2000\|0x80` with +0x88 = +0x8c = 5000, +0x8a = 3000, +0x94 = 0x70, +0x98 = 0x20. That is world-axis flatten ×1.22 / 0.73 / 1.22 plus red. STAGE5 copy @0x8011bcc0 / @0x8011bd60 |

## 9. RE1.5 vs RE2 (what the port implements): differences

| Aspect | RE2 (port: enemy_ai_re2_zombie.c + main.c) | RE1.5 EXE |
|---|---|---|
| Record offsets | world matrix +0x48, parent ptr +0x74, parent record +0x94, colour +0x70, scale +0x8c/+0x8e/+0x90, 0x80 tints +0x98/+0x9c, burst +0x98..+0xa4 | world +0x40, parent +0x6c, record +0x90, colour +0x68, scale +0x88/+0x8a/+0x8c, tints +0x94/+0x98, burst +0x96/+0x9a/+0x9c (stride 0xAC in both) |
| Bit 0 | draw enable, flat | same |
| Parent cascade `(parent & 0x21) == 0x20` → word = 0x20 | yes (@0x80027480-94) | **NONE**: e9ec has no parent read. Hiding a part never hides its children |
| 0x40 | own matrix | same. Children compose from it |
| Colour word | NCCT multiply, neutral 0x808080 | **same** (FUN_800254a0 `ldrgb` + NCCT) |
| Semi-transparency bit | word +0x04 bit 2 | none (a3 = 0 @0x8001ece0). 0x02 is dead |
| 0x08 shard offset | along the prim's n0 normal, `n>>10 · k` | along the prim's **2nd vertex position**: `(x>>5, (y+300)>>6, z>>5) · k` |
| 0x08 table | 2 rows (life bit 15) | 1 row, identical to RE2 row 0 |
| 0x08 lifetime | test `life&0x7fff < 0x1d` → ++, else word = 0, **after** drawing; indices 0..29 = 30 frames | ++ inside the quad draw, then `>= 29` → word = 0; indices 0..28 = **29 frames**. Frozen while off screen |
| 0x08 drift | vy + yaw/speed horizontal (FUN_80028DAC) | **vy only** (+0x96 += +0x9a; t.y += vy) |
| Shard colour | flat tint word, unlit | same (flat +0x68, no NCLIP) |
| 0x20 physics | FUN_80028AD8: yaw-rotated launch, 3-hit budget, wall bounce, ×¼ damping, blood on impact, matrix settle blend | FUN_8001f024: `t += v` (x/z s16, vy += (s8)+0x75), floor = ent+0x1ba, `vy = -vy >> 3`, 0x10 → 0x2000 squash (10000/0/8000), vanish if vy == 0 && t.y == 0, optional Z spin `+0x82 += +0x76` |
| 0x80 | FUN_8002940c: tri rgb1 SET to c1, rgb2 averaged | FUN_8002441c: rgb1 averaged with c1, rgb2 averaged with c2 (tris and quads); v0/v3 untouched |
| 0x400 / 0x2000 | same maths (port: "no part scaling", open, header comment (5)) | 0x400 local-column scale after view·world; 0x2000 world-row scale S·R, t unscaled |
| 0x1062 | flying leg | flying part (spider, STAGE4/5) |

**Conclusion.**
- The bit meanings 0x01 / 0x08 / 0x20 / 0x40 / 0x400 / 0x2000 / 0x1000 and the colour-word semantics are shared.
- The RE2 physics, the burst offset/lifetime, the 0x80 blend, the cascade and the field offsets are NOT.
- `re15_re2z_gore_resolve`, `re15_re2z_gore_part_matrix` (re2z_part_phys_ad8 / _dac) and `re15_re2z_gore_part_burst` therefore **cannot be reused unchanged** for RE1.5 actors. They are also gated to the RE2 zombie / crow / gator types and the RE2 part permutation (`re2z_bone_to_part`).
- The PC renderer hooks in main.c can be reused:
  - the gore_draw skip @10502;
  - the tint macro RE2_GORE_TINT @10468;
  - the matrix override @10669;
  - the burst offset/flat colour @10781-10850 / @10877-10934.

## 10. Recommended minimal port design

1. **New per-actor RE1.5 part state, in RE1.5 part numbering** (EMR bone order == pool order, no permutation). Size it to at least 21 entries: the player uses part 20 (0xd70 @0x80024c30, 3440 @0x80031bec), the spider part 19 (0xcc4). Mirror the overlapping tail as in the original, because producers clear it as halfwords (FUN_80108d04 zeroes +0x94..+0x9e, which wipes both tint words too):
   ```c
   typedef struct {
     uint16_t w;              /* +0x00 */
     uint32_t col;            /* +0x68  default 0x808080 */
     int16_t  v[3];           /* +0x38/3a/3c */
     int8_t   grav, spin_inc; /* +0x75/+0x76 */
     int16_t  spin;           /* +0x82 */
     int16_t  scale[3];       /* +0x88/8a/8c */
     uint16_t h94[6];         /* +0x94..+0x9e raw halves: tint1 = h94[0] | h94[1]<<16 (also vy = (s16)h94[1]),
                                 tint2 = h94[2] | h94[3]<<16 (vy_inc = (s16)h94[3]), frame = h94[4] */
     int16_t  m[9]; int32_t t[3]; uint8_t latched; /* +0x40 world matrix when 0x40 */
   } re15_part_t;
   ```
   Reset = FUN_8001ee68: w = 1, col = 0x808080, tail untouched.
2. **A `re15_part_step(actor, i, world_rot, world_trans, onscreen)` called from the renderer**, in the original order per part:
   - (a) if !(w & 0x40): world = parent_world ∘ local, else use the latched m/t (latch it the first frame 0x40 appears, like re2z_part_seeded);
   - (b) w & 0x20 → FUN_8001f024;
   - (c) if (w & 1): w & 0x08 → draw shards with k = tbl[frame]; if onscreen, frame++; then FUN_8001f1e0. Else, if off screen with 0x08: FUN_8001f1e0 only.

   Children must compose from the parent's **resulting** world matrix, not from the skeleton pose. The current NPC loop takes `npc_poses[nbi]` independently, which breaks torso-off (head and arms must follow part 7). The player path needs the same.
3. **Renderer changes (main.c NPC loop and the player loop):**
   - draw skip = !(w & 1), flat, no cascade;
   - normal draw: multiply by col, then if 0x80, average rgb1 with tint1 and rgb2 with tint2;
   - 0x08 path: offset from vertex v1 position `(x>>5, (y+300)>>6, z>>5) * k`, flat col, no back-face cull;
   - 0x400: scale model-space x/y/z by scale/4096 before the bone rotation;
   - 0x2000: scale world-space rows after rotation, translation unscaled;
   - 0x200: skip the animated rotation for that bone (skeleton side, FUN_8001f3bc @0x8001f564 / @0x8001f5e0).
4. **Hooks.** The RE1.5 overlay ports (zombie gore handlers, writher 0x1a hand, spider 0x25, the Birkin throw-kill on the player, the zombie spawn variants) only write `re15_part_t` fields with their @0x addresses. The renderer is the only consumer, as in the EXE.
5. **Effects anchored at parts.** FUN_80019700 calls with a2 = part+0x40 need the same world matrix, after step (a).
