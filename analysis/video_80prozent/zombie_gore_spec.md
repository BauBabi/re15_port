# RE1.5 zombie hurt/death gore: port-ready spec (STAGE1, root 0x80100424)

This is research only. No repo files were changed.

Every value below was read from `info/Re1.5/PSX/BIN/STAGE1.BIN` (the overlay is loaded at 0x80100000) or from `PSX.EXE`, using `re15_disasm.py dis/bytes/table/read`. The full dump is `scratchpad/zg/all.txt`, covering 0x80105a00 plus 3600 instructions. Ghidra decompiles were not used for any constant.

## 0. Conventions and shared facts

### Entity fields

The entity is `e = *(0x800ac784)`.

| Field | Meaning | Evidence |
|---|---|---|
| +0x4 | state | |
| +0x5 | weapon id | |
| +0x6 | elevation column | |
| +0x7 | phase | |
| +0x8 | type | |
| +0x9 | grid | |
| +0x8c | speed, s16 | |
| +0x8f | blend | |
| +0x93 | hit bits | |
| +0x94 | clip | |
| +0x95 | frame | |
| +0x9a | **HP**, s16 | resolver `sh a0,154(s1)` @0x80012504 |
| +0x9c, +0x9e, +0x9f | scratch | |
| +0x1b8 | death-commit byte | |
| +0x1c6 | kill-flag bit | |
| +0x1d4 | walk clip | |
| +0x1d8 | ai_flags, u16 | |
| +0x1dc | **poise**, s16. It is not HP. | |
| +0x84, +0x16c | anim bank 0 (loco/walk) | |
| +0x170, +0x174 | anim bank 1 (action) | |
| +0x188 | model pool | |

### Word +4 writes

A word store to +4 writes four bytes: `+4 = w & 0xff`, `+5 = (w>>8) & 0xff`, `+6 = (w>>16) & 0xff`, `+7 = w>>24`.

| Value | Bytes | Meaning |
|---|---|---|
| 0x10201 | state 1, sub 2, +6 1, +7 0 | |
| 0x201 | state 1, sub 2, +6 0, +7 0 | |
| 0x801 | +5 = 8 | |
| 0x701 | +5 = 7 | |
| 1 | state 1, +5/+6/+7 = 0 | |

A half store `sh x,6` writes `+6 = x & 0xff` and `+7 = x>>8`. A half store `sh 7,4` writes `+4 = 7` and `+5 = 0`.

### Model parts

- Part k base = `pool + k*0xac`.
- `a2 = pool + k*0xac + 0x40` is the part MATRIX: 9×s16 rotation, then the translation as int32 x3 at part+0x54/+0x58/+0x5c.
- The gore bone is `u8[0x8011f784 + type]`. Bytes @0x8011f794: `0e 0e 0e 08 08 0e 0e 0e 0e 0e 0e 08 08 08 08 00`.
  - Types 0x10-0x12 and 0x15-0x1a → part 14.
  - Types 0x13, 0x14 and 0x1b-0x1e → part 8.
  - Type 0x1f → part 0.
  - This matches the port's `s_gore_bone` (re15_damage.c ~3263).

### FUN_80019700(a0, a1, a2, a3): the blood/effect spawn

- **a0 decoding:**
  - effect id = a0>>24;
  - sub = (a0>>16)&7;
  - scale = a0 & 0xffff, stored at slot+0x72.
  - In the port: `re15_esp_fx_spawn_ex(bank, a0>>24, (a0>>16)&7, a0&0xffff, …, param=a1)`.
- **a1** = `lh a1,106(e)`, i.e. e+0x6a (yaw), in every zombie call below.
- **a2** = the part matrix pointer. It is stored at slot+0x74, so the effect follows the part.
- **a3** = the 16-byte offset. 0x80019700 copies 4 words into slot+0x40..+0x4c. The position tick reads them at `lhu 64/68/72(a0)` → scratch SVECTOR @0x8001a208/0x8001a228/0x8001a238, then applies them with `ApplyMatrix(slot+0x4c = anchor R, …)`, `jal 0x800661c0` @0x8001a248. So the offset is **(s16 of word0, s16 of word1, s16 of word2) in part-local space**. Port primitive: `re15_enemy_part_point(e, part, ofs, out)` (re15_damage.c:3304).

| a3 | Bytes | Offset |
|---|---|---|
| 0x8011fe84 | `c8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00` | (200, 0, 0) |
| 0x8011fe9c | `64 00 00 00 38 ff ff ff 00 00 00 00 00 00 00 00` | **(100, -200, 0)** |
| 0x8012016c | `c8 00 00 00 00 …` | (200, 0, 0) |
| 0x8012017c | `00 00 00 00 44 fd ff ff 00 …` | **(0, -700, 0)** |
| 0x80100068 | 16 zero bytes | (0, 0, 0) |

### Other helpers

- **rng** = FUN_8001af20. It is used with masks &1, &3, &7, &0xf, &0x1f and &0x3f as noted per call.
- **SE** = FUN_800453d0(n).
- **anim_set** = FUN_8001f314(a0, a1, a2 = 0 forward, a3 = rate). It returns 1 at clip end; handlers do `+7 += v0`.
- **pos_advance** = FUN_800245d8(a0). It rotates (+0x8c, 0, 0) by `RotMatrixY(+0x6a + (s16)a0)` and adds the result to the position (@0x80024654-64). **a0 is a yaw offset**: 0 = forward, 0x800 = backward.
- **FUN_8010939c(a0, a1)** = foot-lock. Port: `re15_enemy_footlock_step`, enemy_ai_common.c:3357.
- **FUN_8001a780(&player)** = facing_aligned.
- **FUN_8001a8f8(&playerX, rate)** = yaw slew toward a point. Port: `re15_enemy_steer_point`.
- **Kill flag KF**: `a0 = (*(s16*)0x800b0fe0 < 3) ? 0x800b1038 : 0x800b1058`, `a1 = +0x1c6`, then `jal 0x8004ef90`. FUN_8004ef90 is a flag set, not an effect.

### +0x93 bits and how the resolver uses them

The resolver is FUN_80011f50, verified @0x80012370-0x80012538.

- `+0x93 &= 1` @0x80012378-7c.
- Bit 0x40 if (w==8 && dist<3000) or w==7 @0x80012380-b8.
- Bit 0x80 if facing_aligned @0x800123dc-f8.
- **If +0x93 bit0 is still set**, i.e. a handler owns the zombie: `+0x93 |= 2`, recurse and return. This happens @0x80012404-24, with **no write to +4..+7 and no HP change**. That is how the headless walker and twitch handlers see "extra hits".
- Otherwise:
  - `+7 = 0` @0x80012428.
  - `+6 = u8[0x8006f410 + (player_word>>29)]`. The table is `07 00 01 07 02 00 00 00` @0x80012430-50.
  - Y-clamps:
    - if playerY < eY and +6 == 0 → +6 = 1 (@0x80012454-7c);
    - if eY < playerY and +6 == 2 → +6 = 1 (@0x80012480-ac).
  - `+5 = weapon` (`sb fp,5(s1)` @0x800124bc).
  - `hp -= u16[0x8006e0d0 + type*0x58 + w*4]` and `+93 |= 1` @0x800124c0-0x80012504.
  - crit && type < 0x20 → hp = -1 @0x800124fc-1c.
  - `+4 = 2`, or 3 if hp < 0 (@0x80012520-38).

**Correction to re_gore.md: column 0 (DOWN) is reachable on flat ground.**
- FUN_80012aa4(3000) is called from the zombie live state @0x80101600 for a non-crawler. It computes `s0 = (playerY - eY)/1800` (magic `0x91a2b3c5`, `sra 10`, @0x80012abc-0x80012b1c).
- It clears word0 bits 29-31 (@0x80012aec-fc).
- If `s0 ∈ {0,1}`, it sets bit30 (LEVEL) (@0x80012b20-44).
- Then, if `1000*|s0| < dist < 3000 + 1000*|s0|`, it sets bit31 (0x80000000, UP) when s0 > 0, else bit29 (0x20000000, DOWN) (@0x80012b64-d4).
- So with |dy| < 1800 (s0 = 0) and dist < 3000, the zombie carries LEVEL+DOWN.
- I did not trace how the player picks its aim bits from the target bands. If the player aims down, +6 = 0, which reaches DEATH [7/8/19][0] = 0x80108abc.
- The Y-clamp is strict `slt`, so on equal Y it does nothing (the port says the same at re15_damage.c:2852).
- re_gore's statement "needs a height difference of at least 1800 / on flat ground column 1 is always used" is **wrong**.

### Dispatch tables (dumped)

**HURT @0x8011fb90**, index `[+5*0x20 + +6*4]`:

| Row (+5) | Col 0 | Col 1 | Other cols |
|---|---|---|---|
| 1, 3, 4 | 0x80105b7c | 0x80105b7c | NULL |
| 5, 6 | 0x80106290 | 0x80106290 | NULL |
| 7, 8 | NULL | 0x80106624 | NULL |
| 19 | 0x80106048 | 0x80106048 | NULL |
| all other rows (0, 2, 9-18, 20) | NULL | NULL | NULL |

**DEATH @0x8011feac**:

| Row (+5) | Col 0 | Col 1 | Col 4 | Col 5 | Col 6 | Other cols |
|---|---|---|---|---|---|---|
| 1, 3-6 | 6c18 | 6c18 | 7634 | – | – | – |
| 7 | **8abc** | **6edc** | 7634 | **7244** | – | – |
| 8 | **8abc** | **7ee0** | 7634 | 7244 | 6c18 | – |
| 19 | **8abc** | 6c18 | 7634 | – | – | – |
| 0, 2, 9-18, 20 | NULL | NULL | 7634 | – | – | – |

All handler names abbreviate 0x8010xxxx. Every other cell is NULL.

- A NULL cell means `jalr 0`, so the original crashes there.
- Grenade kills come through FUN_80012d60 with +5 = 9/10/11 and +6 = 1. That is a NULL cell in both tables. Port: re15_damage.c:3170-3172.
- +6 = 7 comes from aim index 0 or 3. Column 7 is NULL everywhere.

**Router behaviour:**

- **HURT router FUN_80105a8c**:
  - If +9 & 0x80 (downed/crawler):
    - +5 ∈ {18, 19} → FUN_80106a38 (@0x80105ab0-d8);
    - otherwise → FUN_801068a0.
    - The poise gate is skipped.
  - Otherwise: jalr the table cell. Afterwards, every tick, if `(s16)+0x1dc < 0` and !(+9 & 0x80): `+4 = 1, +5 = 0x11, +6 = 0` (knockdown) (@0x80105b18-68).
- **DEATH router FUN_80106ba4**: if +9 & 0x80 → FUN_80107cb0; otherwise jalr the table cell. **It re-resolves every tick.**

**Poise table @0x8011fe30 [+5]**, called inside hurt phase 0:

| Rows | Target | Effect on +0x1dc |
|---|---|---|
| 0, 7-16 | 0x80105a24 | `jr ra`, no change |
| 1, 2 | 0x80105a4c | -2 |
| 3-6 | 0x80105a6c | -3 |
| 17-20 | 0x80105a2c | -1 |

These match the port's `stun_step` (enemy_ai_common.c ~5185).

---

## 1. HURT handlers

### 1a. FUN_80105b7c, standard (w1/3/4): for comparison. The port is mostly correct here.

The prologue sets `s0 = pool + 0x4b4` (part 7) @0x80105ba4.

**Phase 0** (+7 == 0):
1. +7 = 1.
2. +0x9e = 2.
3. +0x9c = 0 (sh).
4. +0x8c = 0x14.
5. +0x8f = 0.
6. +0x94 = +0x1d4. There is no +0x95 write.
7. Blood:
   - if +6 == 1: `0x80019700(0x2000, +0x6a, pool + gore*0xac + 0x40, 0x8011fe84)` @0x80105c9c. s0 becomes the gore part base.
   - if +6 == 0: `0x80019700(0x2000, +0x6a, pool + 0x198 = part 2, 0x8011fe84)` @0x80105cd4. s0 = pool + 0x158 (part 2).
8. +93 |= 1.
9. SE 6 (unconditional) @0x80105cf4.
10. jalr poise[+5] @0x80105d28.
11. Fall through into phase 1.

**Phase 1:**
- anim_set(+0x84, +0x16c, 0, 0x200).
- `s0+0x64 += +0x9c`, then `+0x9c -= 128`, then `RotMatrix(s0+0x60 → s0+0x18)` (`jal 0x80068098`). This is the part bend.
- `+0x9e--`; if it was 0 → +7 = 2.

**Phase 2:** +7 = 3, +0x9e = 2, fall through.

**Phase 3:** same as phase 1 but `+0x9c += 128`. At the end:
1. word +4 = 0x10201.
2. +0x9e = (rng & 0x1f) + 8.
3. +0x94 = +0x1d4.
4. +0x8f = 7.
5. +0x1de = (g_spawncount ≥ 5 ? 0x8011faf0 : 0x8011fb00)[rng & 0x1f]. If that value is 2: +5 = 0x13, +6 = 0.
6. +0x9f = rng & 0x1f.
7. +0x9c = u16[0x8011f9f0 + (+0x1de)*128 + (+0x9f)*4].
8. +93 &= 0xfe.
9. rng & 0xf == 0 → anim_set(+0x84, +0x16c, 0, 0x200), then word +4 = 0x801.
10. facing_aligned → word +4 = 0x701, then anim_set again.

**Tail (every tick):** pos_advance(0).

Port gaps in the standard handler:
- the blood ignores the a3 offset (200, 0, 0);
- re15_enemy_hurt_blood also spawns a port-only splatter (re15_damage.c:3407).

### 1b. FUN_80106290, Beretta/Glock (w5, w6), columns 0/1

All anim calls use bank 1: anim_set(+0x170, +0x174, 0, 0x200).

**Phase 0:**
1. +7 = 1 @0x801062e8.
2. +0x94 = 6, +0x95 = 0, +0x8f = 7 (@0x801062f8-0x80106318).
3. Blood:
   - +6 == 1: `0x80019700(`**`0x1000`**`, +0x6a, gore part + 0x40, 0x8011fe84)` @0x8010637c. a0 is loaded @0x80106334.
   - +6 == 0: `0x80019700(0x2000, +0x6a, pool + 0x198 = part 2, 0x8011fe84)` @0x801063b0.
4. +93 |= 1.
5. SE 6 (unconditional) @0x801063d0.
6. jalr poise[+5]. For w5/6 that is -3.
7. Fall through.

**Phase 1** (@0x8010640c): anim_set. If it returns 1:
- +7 = 2, +0x94 = 7, +0x8f = 7. **+0x95 is not reset.**
- Blood with the same zone logic: +6 == 1 → 0x1000 at the gore part (@0x80106530); +6 == 0 → 0x2000 at part 2 (@0x80106568). Both use a3 = 0x8011fe84.

**Phase 2** (@0x80106484): anim_set. If it returns 1:
- +7 = 3, +0x94 = 8, +0x8f = 7 (no +0x95 reset).
- The same two zone-gated bursts (0x1000 @0x80106530 for +6 == 1, 0x2000 @0x80106568 for +6 == 0).

**Phase 3** (@0x80106578): anim_set. If it returns 1:
1. word +4 = 0x201.
2. +93 &= 0xfe.
3. rng & 7 == 0 → +5 = 8 (@0x801065d8).
4. facing_aligned → +5 = 7 (@0x80106600).

**Tail:** FUN_8010939c(0, 0) @0x80106608. **There is no pos_advance and no +0x8c write.** The HURT-router poise gate then applies.

- Zone 2 (up) spawns no blood in any phase.
- Every phase-advance tick fires its bursts once. That is up to 3×2 bursts in total.

### 1c. FUN_80106624, magnum/shotgun level (w7/8 col 1, non-lethal)

This handler does **not** call the poise table.

**Phase 0** (+7 == 0, @0x80106648):
1. +7 = 1.
2. +0x9f = +0x93>>7.
3. +0x94 = 16 - (s8)+0x9f, i.e. clip 0x10 or 0x0f.
4. +0x95 = 0, +0x8f = 3.
5. +0x8c = 100 (0x64).
6. +0x9e = 12.
7. +93 |= 1.
8. Blood:
   - `0x80019700(0x2000, +0x6a, pool + 0x40 = part 0, 0x8011fe84)` @0x801066fc;
   - `0x80019700(0x2000, +0x6a, gore part + 0x40, 0x8011fe84)` @0x80106750.
9. rng & 1 == 0 → SE 6 (@0x80106758-70).
10. Fall through.

**Phase ≥ 1** (@0x80106774): anim_set(+0x170, +0x174, 0, **0x400**). On end:
1. word +4 = 0x201.
2. +93 &= 0xfe.
3. rng & 1 == 0 → +5 = 8.
4. facing_aligned → +5 = 7.

**Every tick** (@0x8010680c-0x80106888):
1. `pos_advance(0x800 - ((s8)+0x9f << 11))`. With +0x9f = 0 that is a backward slide; with +0x9f = 1 it is forward.
2. Then `+0x8c -= (s8)+0x9e`.
3. `+0x9e--`, clamped at 0.

So the speed sequence is 100, 88, 77, … and ends at 22. 22 is the first value that stays, after 12 ticks.

### 1d. FUN_80106048, MC51 (w19), columns 0/1

**Phase 0:**
1. +7 = 1.
2. +0x94 = u8[0x8011fe94 + (rng & 7)] = {6, 8, 6, 7, 6, 8, 8, 7}.
3. +0x95 = 0, +0x8f = 7.
4. +0x8c = 0x0a.
5. Blood:
   - +6 == 1: `0x80019700(`**`0x1500`**`, +0x6a, pool + 0x4f4 = PART 7, `**`0x8011fe9c`**`)` @0x80106120. a0 is loaded @0x8010610c.
   - +6 == 0: `0x80019700(0x2000, +0x6a, pool + 0x198 = part 2, 0x8011fe84)` @0x80106154.
6. +93 |= 1.
7. rng & 7 == 0 → SE 6 (@0x80106188).
8. jalr poise[19] = -1.
9. Fall through.

**Phase 1:** `+7 += anim_set(+0x170, +0x174, 0, 0x200)`.

**Phase 2:**
1. word +4 = 0x201.
2. rng & 7 == 0 → +5 = 8.
3. facing_aligned → +5 = 7.
4. +93 &= 0xfe.

**Tail:**
1. FUN_8010939c(0, 0) @0x8010626c.
2. **pos_advance(0x800)** @0x80106274, i.e. backward at +0x8c = 10 every tick.

### 1e. Downed-hurt handlers (port has them, verify list)

**FUN_801068a0** (downed, +5 ∉ {18, 19}):

Phase 0:
1. +7 = 1.
2. +0x94 = 0x1e, +0x95 = 0, +0x8f = 3.
3. Blood: `0x80019700(`**`0x1500`**`, +0x6a, pool + 0x4f4 = part 7, `**`0x8011fe9c`**`)`.
   - The jal is @0x8010693c.
   - a0 = 0x1500 is loaded @0x8010690c.
   - a2 is set in the delay slot `addiu a2,a2,1268` @0x80106940.
   - a3 = 0x8011fe9c is built @0x8010692c-30.
4. +93 |= 1.
5. SE 6.
6. If `(s16)+0x1dc == 0x80` → +4 = 1, +5 = 0x11, +6 = 4.

Phase 1: `+7 += anim_set(…, 0x400)`.

Phase 2:
1. word +4 = 1.
2. (+9 & 0x1f) == 3 → +5 = 5.
3. +93 &= 0xfe.

**FUN_80106a38** (downed, +5 ∈ {18, 19}): same structure, with these differences:
- +0x94 = 37 + (rng & 1) @0x80106a98;
- +0x8f = 4;
- blood `0x80019700(0x2000, +0x6a, pool + 0x40 = part 0, 0x8011fe9c)` @0x80106ad8;
- rng & 7 == 0 → SE 6;
- anim rate 0x200.

---

## 2. DEATH handlers

Several death handlers have their phase-≥2 dispatch branch nulled. The bytes at the slti delay are `00000000 00000000`, so +7 ≥ 2 runs only the tail.

| Handler | Nulled words |
|---|---|
| 6c18 | @0x80106c40/44 |
| 6edc | @0x80106f0c/10 |
| 7cb0 | @0x80107cd4/d8 |

The live handlers (5b7c, 6290, 6048, 68a0, 6a38) keep `05 00 40 10` = `beq v0,zero`.

### 2a. FUN_80106c18, standard death (comparison)

**Phase 0:**
1. +93 |= 1.
2. +7 = 1.
3. +0x94 = 13 - 2*(+93>>7), i.e. clip 13 or 11.
4. +0x95 = rng & 3.
5. +0x8f = 0xf.
6. Blood, all with a3 = 0x8012016c:
   - 0x2000 at the gore part, always (@0x80106d2c);
   - +6 == 1 → 0x2000 at part 0 (pool + 0x40) (@0x80106d5c);
   - +6 == 0 → 0x2000 at part 2 (pool + 0x198) (@0x80106d8c).
7. rng is drawn @0x80106d94 and masked &1, but **the branch @0x80106da0 is nulled** (bytes `00 00 00 00`). `ori a0,8` and then `ori a0,5` both run, so the call is **always SE 5** (@0x80106dac).
8. +0x1b8 = 1.
9. KF @0x80106e0c.
10. Fall through.

**Phase 1:** `+7 += anim_set(+0x170, +0x174, 0, 0x100)`.

**Phase ≥ 2:** dead code. The tail is: +0x8f == 0 → FUN_8010939c(0, 0).

### 2b. FUN_80106edc, magnum level kill: head burst

**Phase 0:**
1. rng & 1 == 0 → **+6 = 5** and return (@0x80106f38-0x80106f5c). The next tick the router resolves [7][5] = 7244. Only this handler writes +6 = 5, and it only runs for +5 = 7. **So [8][5] (shotgun) has no producer.** re_gore.md's statement "magnum or shotgun col 5" is wrong.
2. Otherwise:
   1. +93 |= 1.
   2. +7 = 1.
   3. +0x94 = 13 - 2*(+93>>7).
   4. +0x95 = rng & 3.
   5. +0x8f = 0xf.
   6. Blood, with gore part G = pool + gore*0xac and a3 = 0x8012016c in all three calls:
      - `0x80019700(0x2000, +0x6a, G + 0x40, …)` @0x8010702c;
      - 0x2000 @0x80107084;
      - **0x5002800** (effect 5, sub 0, scale 0x2800) @0x801070a4. a0 is built @0x8010708c-90.
   7. G part writes (@0x801070b0-e4):
      - +0x68 = 0x8f (sw);
      - +0x94 = 0, +0x96 = **-50**, +0x98 = 0, +0x9a = **3**, +0x9c = 0, +0x9e = 0 (sh each);
      - word |= 0x4a.
   8. SE 9 (@0x801070e0).
   9. +0x1b8 = 1.
   10. Part 7 writes (@0x80107104-20): `sw 0x80 → +0x548` (part7 + 0x94), `sw 0x20 → +0x54c` (part7 + 0x98), part7 word |= 0x80.
   11. KF (@0x8010716c).
   12. Fall through.

**Phase 1:** `+7 += anim_set(+0x170, +0x174, 0, 0x100)`.

**Phase ≥ 2:** dead (@0x801071b0-fc).

**Tail:** +0x8f == 0 → FUN_8010939c(0, 0).

### 2c. FUN_80107244, column 5: headless walker (magnum only, see 2b)

**Phase 0:**
1. +93 |= 1.
2. +7 = 1.
3. +0x9e = 0x28 (40), used as the turn rate.
4. +0x9f = (rng & 0x3f) + 60.
5. +0x94 = +0x1d4 (walk clip).
6. +0x95 = rng & 3.
7. +0x8f = 7.
8. +0x9c = 4 (sh), the spurt counter.
9. Blood at the gore part, all with a3 = 0x8012016c. **The order differs from 6edc**:
   - 0x2000 @0x80107358;
   - **0x5002800** @0x801073b4;
   - 0x2000 @0x801073d0.
10. The same G part burst as 6edc: +0x68 = 0x8f, +0x96 = -50, +0x9a = 3, the other four halves = 0, word |= 0x4a (@0x801073dc-0x80107410).
11. SE 9.
12. +0x1b8 = 1.
13. Part 7: +0x548 = 0x80, +0x54c = 0x20, word |= 0x80.
14. +93 &= 0xfd.
15. KF.
16. Fall through.

**Phase 1** (every tick, @0x801074b8):
1. If +93 & 2:
   - `0x80019700(0x2000, +0x6a, pool + 0x4f4 = part 7, `**`0x8012017c`**` = (0, -700, 0))` @0x801074e8;
   - +93 &= 0xfd;
   - `+0x9c--`. If it was 0 before the decrement → +0x9f = 0.
   - So there are **5 spurts maximum**; the 5th forces the end. re_gore.md said "up to 4".
2. FUN_8001a8f8(&playerX, (s8)+0x9e = 40) @0x80107554.
3. anim_set(+0x84, +0x16c, 0, 0x200). This is bank 0, the walk.
4. `+0x9f--`. If it was 0:
   - +6/+7 = **0x101**, i.e. +6 = 1 and +7 = 1, so the next tick runs [7][1] = 6edc phase 1, the fall anim;
   - +0x94 = 0xb;
   - +0x95 = 0;
   - +0x8f = 0xf.

**Tail:** FUN_8010939c(0, (*(u32*)(+0x168) & 0x2000) ? 1 : 0) @0x801075e4-0x8010760c.

There is no pos_advance and no +0x8c write. The walk moves only through the foot-lock.

### 2d. FUN_80107ee0, shotgun level kill

**Prologue:** copies 16 zero bytes from 0x80100068 to sp+16. Dispatch on +7 < 5 through the jump table @0x8010012c: [0] 0x80107f58, [1] 0x80108448, [2] 0x801087b8, [3] 0x801089b0, [4] 0x80108a74.

**Phase 0:**

Common writes:
1. +93 |= 1.
2. +7 = 1.
3. +0x94 = 13 - 2*(+93>>7).
4. +0x95 = rng & 3.
5. +0x8f = 0xf.
6. +0x1b8 = 1.

**Non-crit branch** (`andi 0x40` @0x80107ff4 is 0, i.e. dist ≥ 3000): `sh 6, +6` → **+6 = 6, +7 = 0** and exit (@0x80108000-08). The next tick runs [8][6] = 6c18 phase 0 (standard death, KF there).

**Crit branch:**
1. Part 7: +0x548 = 0x80, +0x54c = 0x20 (sw), word |= 0x80 (@0x8010800c-28).
2. Part 0: +0x94 = 0x80, +0x98 = 0x20 (sw), word |= 0x80 (@0x80108038-54).
3. rng & 1 == 1 && type != 0x13 → part 0 spin: +0x88 = 0x5f4, +0x8a = 0x1000, +0x8c = 0x5f4 (sh each), word |= 0x400 (@0x80108058-98).
4. Blood `0x80019700(0x2800, +0x6a, pool + 0x40 = part 0, 0x8012016c)` @0x801080b8.
5. SE 9.
6. rng & 1 picks one of two outcomes (@0x801080c8-d4): ==1 is the blow-apart, ==0 is the torso-off.

**Blow-apart** (rng & 1 == 1):
1. +7 = 3.
2. FUN_80108d04(gore part base) @0x80108134.
3. rng & 1 for the part-8 arm:
   - ==1: part 8 spin (+0x5e8 = 0x5f4, +0x5ea = 0xc18, +0x5ec = 0x5f4, word |= 0x400), then d04(part 9 = pool + 0x60c) @0x8010818c.
   - ==0: d04(part 8 = pool + 0x560) @0x8010819c, then part 9 word = 0 @0x801081b8.
4. Part 10 word = 0 (@0x801081d0), on both paths.
5. The same choice for the part-11 arm (rng & 1):
   - ==1: part 11 spin (+0x7ec/+0x7ee/+0x7f0 = 0x5f4/0xc18/0x5f4, word |= 0x400), then d04(part 12 = pool + 0x810) @0x80108224.
   - ==0: d04(part 11 = pool + 0x764), then part 12 word = 0 @0x80108250.
6. Part 13 word = 0 @0x8010826c.
7. +0x8c = 200.
8. KF (@0x801082a4 or @0x801082c4).
9. Exit.

**Torso-off** (rng & 1 == 0, @0x801082d4):
1. rng & 3 == 0 → SE (rng & 1 ? 6 : 8) (@0x801082d4-0x80108300).
2. Part 7 word |= **0x40** (@0x80108328).
3. sp+16 = {eX - playerX, 0, eZ - playerZ}. VectorNormalS(sp+16, part7 + 0x38) (`jal 0x80066a1c`; the PsyQ VectorNormalS signature is verified @0x80066a1c/0x80066a70).
4. Part 7 fields:
   - +0x3a (vy) = 0x50;
   - +0x75 = 3 (bounces, s8);
   - +0x76 = 10 (gravity, s8);
   - +0x82 = 8 (sh, spin counter);
   - +0x38 >>= 3, +0x3c >>= 3 (s16 arithmetic).
5. Blood, all with a3 = 0x8012016c:
   - `0x80019700(0x2800, +0x6a, part 7 + 0x40, …)` @0x801083cc;
   - **0x5002800** at part 7 @0x801083ec;
   - 0x2000 at the gore part @0x80108440.
6. Fall into phase 1. There is no KF, because the zombie survives.

**Phase 1** (torso flight, @0x80108448). Here P7 = pool + 0x4b4:
1. P7+0x54 += (s16)P7+0x38 and P7+0x5c += (s16)P7+0x3c (int32 translation).
2. Collision with the entity temporarily moved:
   - save e+0x34..+0x40;
   - set e pos = P7+0x54..+0x60;
   - FUN_8002b498(e), FUN_8002aec4(&player, e), FUN_8002b544();
   - FUN_8003b0a4(e+0x34, u16[*(e+0x78)+6], 4) = wall collision;
   - P7 translation = e pos;
   - restore e pos.
3. P7+0x38 and +0x3c >>= 1.
4. Bounce state (s8 P7+0x75 < 4, always true):
   - if the count == 0: P7+0x76 = 0 and vy = 0;
   - P7+0x58 += vy;
   - vy += (s8)P7+0x76.
5. Floor hit, when `(e+0x1ba) - 200 < P7+0x58`:
   - vy = -(vy >> 2);
   - P7+0x58 = floor - 200;
   - blood `0x80019700(0x2000, +0x6a, part 7 + 0x40, 0x8012016c)` @0x80108630;
   - count-- if it is nonzero.
6. Spin, if P7+0x82 != 0:
   - decrement it;
   - M = RotMatrix?(0x80) built from the copy of 0x80072d4c (`jal 0x80065b70`);
   - P7+0x40 = P7+0x40 × M (`jal 0x80022da0`);
   - if the new counter is odd → blood 0x2000 at part 7 (a3 = 0x8012016c) @0x80108774. Counters 7/5/3/1 give **4 spurts**.
7. Once the spin counter is 0: part 8 word |= 0x200 and part 11 word |= 0x200 (@0x801086f8-40).
8. `+7 += anim_set(+0x170, +0x174, 0, 0x100)`.
9. Tail: +0x8f == 0 → FUN_8010939c(0, 0).

**Phase 2** (crawler commit, @0x801087b8):
1. If !(+93 & 0x80): part 0 +0x7c and +0x64 are each set to (value + 0x800) & 0xfff, then RotMatrix(part0+0x60 → part0+0x18).
2. Parts 1, 2, 4, 5 (+0xac, +0x158, +0x2b0, +0x35c) word |= 0x40.
3. e+0x34 = P7+0x54 and e+0x3c = P7+0x5c. The entity jumps to the torso.
4. P7 word &= ~0x40.
5. Blood `0x80019700(0x2000, +0x6a, part 7 + 0x40, 0x8012016c)` @0x801088f4.
6. Part 8 and part 11 word &= ~0x200.
7. **+9 = 0x81** @0x80108950.
8. **word +4 = 1** @0x80108960.
9. **hp (+0x9a) = 0x1e** @0x80108970.
10. +93 &= 0xfe.
11. +0x1d8 |= 0x80.

**Phase 3** (blow-apart slide, @0x801089b0):
1. `+7 += anim_set(…, 0x100)`.
2. `pos_advance(((~(u8)+0x93) >> 7) << 11)`. That is -0x1000 (≡ forward) when bit 7 is set, and -0x800 (≡ back) otherwise.
3. +0x8c -= 20, clamped at 0 (200 → 0 in 10 ticks).
4. Tail: foot-lock if +0x8f == 0.

**Phase 4** (@0x80108a74): word0 |= 2, +4 = 7, +5 = 0 (corpse). This is **reachable**.

Corrections to re_gore.md for this handler:
- parts 10 and 13 are zeroed (missed);
- the torso spurts are "4, on odd spin counters" and the bounce spurts are separate, not "every second frame";
- the phase-2 relocation, the +0x1d8 write and the parts 8/11 0x200 bits were missed.

### 2e. FUN_80108abc: column 0 (magnum, shotgun or MC51 aimed down), legs off → crawler

**Phase 0:**
1. +93 |= 1.
2. +7 = 1.
3. **+0x94 = 0xb** (fixed).
4. +0x95 = rng & 3.
5. +0x8f = 0xf.
6. +0x1d8 |= 0x80.
7. +0x1b8 = 1.
8. FUN_80108d04 on four parts:
   - part 2 = pool + 0x158 (`addiu a0,a0,344` @0x80108ba0);
   - part 3 = pool + 0x204 (@0x80108bb8);
   - part 5 = pool + 0x35c (@0x80108bd0);
   - part 6 = pool + 0x408 (@0x80108be8).
9. Part 1: +0x140 = 0x80, +0x144 = 0x20 (sw), word |= 0x80.
10. Part 4: +0x344 = 0x80, +0x348 = 0x20 (sw), word |= 0x80.
11. rng & 1 != 0 → part 4 word = 0 (@0x80108c48-50; the delay slot is a nop).
12. SE (rng & 1 ? 6 : 5) @0x80108c6c.
13. **No KF.**

**Phase 1:** `+7 += anim_set(+0x170, +0x174, 0, 0x100)`.

**Phase 2:**
1. **+9 = 0x81** @0x80108cb0.
2. **word +4 = 1** @0x80108cc0.
3. **hp = 0x1e** @0x80108cd0.
4. +93 &= 0xfe.

**FUN_80108d04(part P):**
1. `0x80019700(0x2000, +0x6a, P + 0x40, 0x8012016c)` @0x80108d2c.
2. P+0x68 = 0x8f (sw).
3. P+0x94, +0x96, +0x98, +0x9a, +0x9c, +0x9e = 0 (sh each).
4. P word |= 0x4a.

This is not the same as the 6edc inline burst, which sets +0x96 = -50 and +0x9a = 3.

### 2f. FUN_80107cb0, downed death (port has it; values to fix)

**Phase 0:**
1. +93 |= 1.
2. +7 = 1.
3. +0x94 = 0x1f, +0x95 = 0, +0x8f = 0xf.
4. Blood `0x80019700(`**`0x2500`**`, +0x6a, `**`pool + 0x40 = part 0`**`, 0x8012016c)`:
   - the jal is @0x80107d68;
   - a0 is loaded @0x80107d5c;
   - a2 is set in the delay slot `addiu a2,a2,64` @0x80107d6c;
   - a3 is built @0x80107d38-3c.
5. +0x1b8 = 1.
6. KF @0x80107dc4.
7. Fall through.

**Phase 1:**
1. +0x95 == 7 → SE (rng & 1 ? 5 : 8) @0x80107de8-0x80107e00. The beq takes the delay-slot a0 = 8 when the bit is 0.
2. +0x95 == 0x23 → `0x80019700(0x2000, +0x6a, gore part + 0x40, 0x8012016c)` @0x80107e68.
3. `+7 += anim_set(…, 0x100)`.

**Phase ≥ 2:** dead (@0x80107cd4/d8 nulled).

### 2g. Column 4, FUN_80107634

Dead blocks are the only producers of +6 = 4 in death state: 6c18 @0x80106e60-70 and 6edc @0x801071c0-d0.

FUN_801068a0 writes +6 = 4 together with +4 = 1, which is a different state.

The +93 & 2 readers in 0x80105a00-0x801091dc are only @0x801074cc (7244) and @0x80107810 (7634).

---

## 3. Crawler (+9 = 0x81) after 7ee0 / 8abc

- Live state 1 is FUN_80101224, root table @0x8011f7b4[1].
- Each tick, with +9 & 0x80, it clears word0 bit 30 and calls FUN_80012974(5000) (@0x80101614-38) instead of the standing aa4(3000).
- It then dispatches `0x8011f80c[+9 & 0xf]` (@0x8010164c-70). [1] = FUN_80101708.
- FUN_80101708 is a double dispatch: DECIDE 0x8011f8e0[+5], then ANIMATE 0x8011f920[+5], with +5 read fresh in between.

| +5 | DECIDE | ANIMATE |
|---|---|---|
| 0 | 0x801035f8 | 0x801036dc |
| 1, 2 | 0x80103b8c | 0x80103b94 |
| 3, 4 | 0x80104540 | 0x80104548 |
| 5 | 0x8010466c | 0x80104808 |
| 6 | 0x80104f78 | 0x80104f80 |
| 15 | 0x80109e44 | 0x80109e4c |
| 7-14 | NULL | NULL |

When a crawler is hit: HURT goes to 68a0/6a38 and DEATH goes to 7cb0 (both through +9 & 0x80).

**The port already has this machine:**
- the grid-1 crawler at enemy_ai_common.c:3790-4900, built for the ROOM1030/1010/1220 RDT crawlers;
- the DECIDE/ANIMATE switch at ~4780-4900;
- `grid_id = 0x81` at :4074.
- Only [15] is open.

So the gore crawler only needs `grid_id = 0x81`, `re15_ai_set_state_word(e, 1)` and `hp = 30`. The leg-part visibility (part words) still has to be rendered.

---

## 4. Port issues claimed in re_gore.md: verdicts

| # | Claim | Verdict | Exact bytes / instructions |
|---|---|---|---|
| 1 | Downed flinch blood scale 0x1500 @0x8010693c; the port uses 0x2000 | **CONFIRMED, and the descriptor is also wrong.** a0 = 0x1500 at part 7, a3 = **0x8011fe9c** (100, -200, 0), not 0x8011fe84. Port: re15_damage.c:3412-3418 `re15_enemy_blood_at_bone` hard-codes 0x2000 with no offset; called at enemy_ai_common.c:5117. The prone variant :5107 has the right scale 0x2000 at part 0, but also needs a3 = 0x8011fe9c (@0x80106acc). | `8010690c ori a0,zero,0x1500` / `8010692c-30 a3=0x8011fe9c` / `8010693c jal 0x80019700` / `80106940 addiu a2,a2,1268` |
| 2 | Downed death 0x2500 at part 0 @0x80107d68 | **CONFIRMED.** Port: `re15_enemy_death_fx` (re15_damage.c:3432) = 0x2000 at the GORE bone, plus a port-only splatter (:3445). Called at enemy_ai_common.c:5624 for the downed branch. Wrong scale and wrong part. The frame-35 call (:5648) is correct in scale and part, but has no offset and keeps the splatter. | `80107d5c ori a0,zero,0x2500` / `80107d38-3c a3=0x8012016c` / `80107d68 jal` / `80107d6c addiu a2,a2,64` |
| 3 | a3 offset x=200 @0x8011fe84 | **CONFIRMED but incomplete.** Four descriptors are used: fe84 (200,0,0), fe9c (100,-200,0), 0x8012016c (200,0,0), 0x8012017c (0,-700,0). The offset is part-local (ApplyMatrix with the anchor R, @0x8001a248). The port ignores all of them; `re15_enemy_part_point` (re15_damage.c:3304) is the ready primitive. | see §0 |
| 4 | `re15_enemy_gore_tick` (re15_damage.c:3226, cites "FUN_80106a44 @0x80106a98") is invented | **CONFIRMED invented.** 0x80106a44 is `sw ra,16(sp)` inside the prologue of FUN_80106a38. 0x80106a98 is `addiu v0,v0,37` (the prone clip). FUN_80106a38 never reads +93 & 2. Its spawn is at the actor root with a3 = 0x8012017c applied nowhere. The real +93 & 2 spurt is 7244 @0x801074c4-e8 (part 7, a3 = 0x8012017c, inside phase 1 of a death handler) and 7634 @0x80107810. **Remove it.** Call sites: enemy_ai_common.c:14804 and :15028. | — |
| 5 | `re15_enemy_gore_setup` (re15_damage.c:3334, `sub_state_1 == 0x58`) is a misread | **CONFIRMED.** 0x8011fe30 + 0x58*4 = 0x8011ff90 = 0x8011feac + 7*0x20 + 1*4, i.e. DEATH[7][1]. The index is [+5 = 7][+6 = 1], not +5 = 0x58. +5 is the weapon id, at most 20, so the function **never fires**. Even if it did, it skips the 6edc part writes, SE 9, the part-7 writes, KF and the +7 anim. **Remove it** (call sites enemy_ai_common.c:14805 and :15029) and replace it with the real DEATH-table dispatch. | — |
| 6 | "6c18 SE 5" | Port SE 5 always = **correct** (branch nulled @0x80106da0). | `80106d9c andi v0,v0,1 / 80106da0 00000000 / 80106da4 ori a0,8 / 80106da8 ori a0,5` |
| 7 | "Column 0 needs dy ≥ 1800" | **WRONG** (see §0, FUN_80012aa4). | — |
| 8 | "7244 = magnum or shotgun" | **WRONG.** Magnum only; [8][5] has no producer. | — |
| 9 | "Up to 4 neck spurts" | **WRONG.** 5 spurts (counter 4 → wrap); the 5th ends the walk. | — |

---

## 5. Port damage-path check (`re15_damage.c`, gun resolver ≈ FUN_80011f50)

These parts are correct:

| Original | Port line | Port code |
|---|---|---|
| `+0x93 &= 1` | :2740 | |
| crit 0x40 | :2745-2746 | Port adds w20 (Python), declared as a port choice |
| guard → `|= 2` + recurse | :2747-2750 | |
| `+0x5 = weapon_id` (@0x800124bc) | :2796 | `e->sub_state_1 = (uint8_t)weapon_id` |
| HP | :2811 | |
| hp = -1 crit | :2828 | |
| `+0x7 = 0` (@0x80012428) | :2831 | |
| `+0x4 = 2/3` | :2832 | |
| `+0x6` elevation (@0x80012430-ac) | :2842-2856 | 3-value model via `re15_player_aim_elevation`, plus both Y-clamps. The table value 7 (aim index 0/3) is not modelled. |

Caveats:
- `re15_re2_stamp_hit` (:2869, body :3001) overwrites +5/+6 **only** for RE2-owned types. It returns early for RE1.5-AI types, so it is OK.
- **`re15_re15_re2z_gore_hit` (:2871 gun, :3202 box path) is the imported RE2 dismemberment (option `re15_re15_re2z_import`, default ON).** It must be disabled for types that run the native RE1.5 handlers, or the two gore systems stack.
- Port deviation: w5/w6 damage = column 3 (:2786). This is a user decision.
- The explosion path `re15_enemy_take_damage_at` (:3170-3172) writes +6 = 1 and +5 = DAT_8006f430[art] (9/10/11 for grenades). In the original those are NULL cells; the new dispatch must keep a fallback for them, documented as a NULL-cell guard.

---

## 6. Integration points (concrete)

### A. A helper replacing `re15_enemy_blood_at_bone` and `re15_enemy_death_fx` (re15_damage.c:3412 / :3432)

```c
void re15_enemy_fx19700(re15_actor_t *e, uint32_t a0, int part, const int32_t ofs[3]);
// re15_enemy_part_point(e, part, ofs, pos);
// re15_esp_fx_spawn_ex(re15_esp_room_bank(), a0>>24, (a0>>16)&7, a0&0xffff, pos..., e->rot_y);
```

- Offset tables: `OFS_FE84 = {200,0,0}`, `OFS_FE9C = {100,-200,0}`, `OFS_016C = {200,0,0}`, `OFS_017C = {0,-700,0}`.
- Drop the port-only splatter.
- Then fix:
  - :5107 → a3 = FE9C;
  - :5117 → 0x1500, FE9C;
  - :5624 → downed 0x2500 at part 0, a3 = 016C; standing 0x2000 at the gore part, 016C;
  - :5632-5633 → 016C;
  - :5648 → 016C;
  - `re15_enemy_hurt_blood` (:3395) → FE84.
- A limitation to note: the effect follows `slot+0x74` (the part matrix). A spawn-time position is exact only for parts that stay still. The flying torso needs a follow anchor.

### B. HURT dispatch

Location: `re15_enemy_ai_live_hurt`, enemy_ai_common.c:5072.

- After the downed block (ends at :5138), before the standard body (:5140), insert a switch on `HURT[e->sub_state_1][e->sub_state_2]`:
  - 0x80106290 → new `zhurt_beretta(e)` (§1b);
  - 0x80106624 → new `zhurt_knockback(e)` (§1c);
  - 0x80106048 → new `zhurt_mc51(e)` (§1d);
  - 0x80105b7c → the existing body;
  - NULL → existing body plus a "FATAL-ORIGINAL" log, like the grid1 pattern at :4857.
- Each new handler must end at `router_gate` (:5285) for the poise gate (:5302). For 6290/6624, which don't advance, skip the forward-lurch lines :5292-5293:
  - 6624 needs `pos_advance(0x800 - b9f<<11)`;
  - 6048 needs `pos_advance(0x800)`.
  - The existing lurch only models offset 0, so it needs a yaw-offset variant.
- The poise call is `e->hit_stun += stun_step[+5]` (:5185-5189). 6290 and 6048 call it; 6624 does not.
- The re-roll exits (0x201 plus rng&7/&1 → +5 = 8, facing → 7) use `re15_ai_set_state_word` (as :5262-5265).
- Bank: these handlers animate bank 1 (+0x170/+0x174), like the downed handlers (:5079-5091 comment). Use `re15_enemy_clip_done`.

### C. DEATH dispatch

Location: `re15_enemy_ai_live_death`, enemy_ai_common.c:5597.

- Replace the hard-wired standing branch (:5602-5640) with a per-tick resolve:
  - `grid & 0x80` → 7cb0 (existing);
  - otherwise `DEATH[+5][+6]`.
- The resolve must happen **every tick**, because 6edc writes +6 = 5 and 7ee0 writes +6 = 6, then return.
- New functions:
  - `zdeath_headburst` (6edc) and `zdeath_headless` (7244), plus 6edc phase 1 for the 7244 → [7][1] handoff with +7 = 1;
  - `zdeath_shotgun` (7ee0, 5 phases);
  - `zdeath_legsoff` (8abc) and `zpart_burst` (8d04).
- Crawler commit: `e->grid_id = 0x81; re15_ai_set_state_word(e,1); e->hp = 30; e->hit_react &= ~1; e->ai_flags |= 0x80` (+0x1d8; 8abc phase 0 and 7ee0 phase 2). The existing grid-1 machine (:4780-4900) then drives it with no new AI.
- KF: the zombie death has no KF in the port. The only port substitute is the corpse-tick loop at :14800-14830, which sets the flag in state 7.
  - 7ee0 phase 4 does reach +4 = 7, so the loop covers it.
  - 6c18/6edc/7cb0 never reach state 7 in the original (dead phase 2). The port's :5695 CORPSE write is a known OPEN issue.
  - Recommendation: call `re15_game_flag_set(re15_em_status_zone(), e->em_flag_id, 1)` at the exact KF points (6c18, 6edc, 7244 phase 0; 7ee0 blow-apart; 7cb0 phase 0) and set +0x1b8 = 1.
- Remove `re15_enemy_gore_tick` and `re15_enemy_gore_setup`: re15_damage.c:3226 and :3334, header re15_damage.h:58/:63, calls enemy_ai_common.c:14804-14805 and :15028-15029.

### D. Part-word render layer (prerequisite for the visible gore)

The handlers write per-part words and fields for parts 0-14:

| Bits | Fields | Meaning |
|---|---|---|
| `|=0x4a` | +0x68 = 0x8f, +0x94..+0x9e | burst |
| `|=0x80` | +0x94 = 0x80, +0x98 = 0x20 | loose |
| `|=0x400` | +0x88..+0x8c | spin |
| `|=0x40` | | detached/hidden; the torso has it while flying, and it is cleared when the entity relocates |
| `|=0x200` | | |
| word = 0 | | |

- The port's only per-part store is `re2z_part_flags[20]` (re15_actor.h:541), with the draw bridge `re15_re2z_gore_resolve`. That bridge models **RE2's** walk FUN_80027160: bit0 = visible, parent cascade `(parent & 0x21) == 0x20`.
- In RE1.5 PSX.EXE, 0x80027160 is a different function (`lw a0,0x8008f66c; jal 0x8006e458`).
- **The RE1.5 part-draw/part-tick that consumes these bits is not yet located.** It must be RE'd before the semantics of 0x40/0x80/0x200/0x400 can be ported byte-true.
- What is consistent so far: "word = 0" hides the part (matches the RE2 bit0 = visible). The rest is UNVERIFIED for RE1.5.
- Until then: store the bits and fields byte-true (new `uint32_t z15_part_word[15]` plus the fields), and implement the render separately.

## 7. Open items, with the next route for each

1. The RE1.5 model-part draw/tick that reads part words 0x40/0x80/0x200/0x400/0x4a and fields +0x68/+0x88/+0x94..+0x9e. Next route: xref `lw …,0(part)` / `andi 0x4a` in PSX.EXE around the model-render caller of the entity pool (+0x188).
2. How the player chooses its aim bits (player_word>>29) from the target bands. This decides how often column 0 happens. Next route: the player aim code that reads the enemy word0 bits 29-31.
3. RotMatrix variant 0x80065b70 (X, Y or Z axis) for the torso spin. Next route: disassemble the whole function.
