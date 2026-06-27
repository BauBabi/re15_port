I'll write the report directly from the provided data. Let me dedup, group, and rank.

# OVERNIGHT BYTE-TRUE AUDIT — RE1.5 Reborn Engine

All entries below are adversarially-confirmed deviations from the RE1.5 disassembly. Overlapping findings have been deduplicated (notably the zombie feed-chew cadence, the grab/mash escape mechanic, the per-state frame-count timing, the Member_cmp/Cmp operator tables, and the stair eject march each had 2–4 source entries collapsed into one). Engine-specific items deliberately kept are in the appendix.

---

## 🧟 ZOMBIE AI (`zombie_common.c`)

### `ZB-01` · HIGH — pursue walk speed ~2.25× too slow, wrong constant
- **our loc:** `src/zombie_common.c:39-44, 232-248`
- **wrong:** `ZB_STEP=0x200` is used as the per-frame pursue XZ step, with a provably-false comment claiming `0x474` is "the anim/Y-arm value".
- **proof:** `ghidra1_V2.txt:106718` (lhu `entity[0x8c]`→SVECTOR.vz) → `:106746,106769-106778` (RotMatrixY(`entity[0x6a]`), add to `entity[0x34]/[0x3c]`); walk magnitude `entity[0x8c]=0x474` at `FUN_80102bc8.c:20`. `0x200` is the anim arg to `FUN_8001f314`, not a translation step.
- **fix:** set the pursue step to `0x474` along `rot_y`, but gate it on the walk-clip move keyframe (`FUN_80102bc8.c:33` `iVar3!=0`) — do **not** translate `0x474` every frame, or average speed is wrong. Fix the comment.

### `ZB-02` · HIGH — lunge XZ step uses wrong field (8.5× too slow)
- **our loc:** `src/zombie_common.c:269`
- **wrong:** ATTACK uses `step=0x1e` as the per-frame XZ translation; `0x1e` is `entity[0x8c]` (a non-XZ field), not the move.
- **proof:** `FUN_80102edc.c:20` lunge XZ = `func_0x8001f314(...,0,0x100)` = `0x100`/frame; `:9` writes `0x1e` to `entity[0x8c]` (never fed to the mover).
- **fix:** in RE15_ZB_ATTACK use `step=0x100`; keep the existing `step = min(step, dist - GRAB)` clamp until the ring is removed (ZB-09).

### `ZB-03` · HIGH — FEED→STANDUP gated on distance only (no cone, no engage-lock, no band)
- **our loc:** `src/zombie_common.c:197` (also `:180,209` cone)
- **wrong:** disturb fires on `d2 < ZB_AGGRO_DIST²` alone; the code comment admits it dropped "no facing cone, no global lock".
- **proof:** `FUN_80102df8.c:10-14` (mirrored `FUN_80103f60/80103e6c`): 4-way AND — `DAT_800acae7==0` (engage-lock) `&& entity[0x1d0]<0x4b0 && func_0x8001a9cc(&player,0x200)==0` (±45° cone) `&& DAT_800acad6==entity[0x82]` (band). Lock set by attack/grab handlers (`FUN_8010e7bc/87c/f598`), cleared on recover/death (`FUN_8010305c.c:18`).
- **fix:** gate FEED→STANDUP on all four: `s_engage_lock==0`, `d2<0x4b0²`, `faced` (±45° cone), band match; set `s_engage_lock=1` on the chosen riser so the others keep eating one-at-a-time.

### `ZB-04` · LOW — wrong IDLE aggro radius + cone constant
- **our loc:** `src/zombie_common.c:31,46,209`
- **wrong:** IDLE→PURSUE gate uses `ZB_AGGRO_DIST=0x4b0(1200)` (the feed radius) and `ZB_COS_CONE2=2563201` (~67°).
- **proof:** `FUN_80101364.c:20` idle gate = `entity[0x1d0]<2000(0x7d0)`. Two distinct radii/cones exist: idle `0x7d0`, feed-disturb `0x4b0`.
- **fix:** IDLE gate → `dist<0x7d0`; set `ZB_COS_CONE2=8388608` (=cos²(45°)·4096²) for the byte-true ±45° feed/idle cone (the `0x200` half-angle of `func_0x8001a9cc`). Keep `0x4b0` only for FEED-disturb.

### `ZB-05` · MED — state timing uses fixed frame counts instead of clip-wrap
- **our loc:** `src/zombie_common.c:51-54,228,295,310` (`ZB_RISE_FRAMES=59/ZB_LUNGE_FRAMES=65/ZB_RECOVER_FRAMES=16`, `ZB_LUNGE_HIT=32`)
- **wrong:** STANDUP/ATTACK/RECOVER leave-conditions are hand-entered timers; the bite fires at a hardcoded mid-frame.
- **proof:** `FUN_801040e8.c:25-29`, `FUN_80104250.c:35-37`, `FUN_80103d48.c:30-32` advance on `func_0x8001f314(...,0x200)!=0` (clip-done); mechanism in `FUN_8001f3bc.c:89-95` (`entity[0x95]>=clip_len` wrap). Bite fires on the lunge clip's `0x2000` keyframe flag (`FUN_80102edc.c:26→FUN_8010383c`), not frame 32. Per-keyframe countdown seeds `entity[0x8f]=0xf`/`7` (`FUN_80102edc.c:13`, `FUN_80102bc8.c:19`).
- **fix:** drive STANDUP/ATTACK-end/RECOVER off `(z->anim_frame+1) >= clip->frame_count` (already available via `anim_select_common.c:46`); fire the bite on the lunge clip's `0x2000` keyframe (or `entity[0x95]=='#'/'P'`). Drop all four magic constants.

### `ZB-06` · MED — feed chew SE is a fabricated modulo with a synthetic per-zombie offset
- **our loc:** `src/zombie_common.c:196`
- **wrong:** chew fires on `(ai->timer + i*23) % 69 == 0x28`; the `% 69` and the `i*23` stagger are invented, and the premise comment ("anim_frame is free-running, no per-clip reset") is false.
- **proof:** `FUN_80103d48.c:21,25` (`entity[0x95]=0` on feed entry; fire `func_0x800453d0(3)` when frame index `==0x28`). `entity[0x95]` is the per-clip frame index (`FUN_8001f8b4.c:76-80`), wrapped at the data-driven clip length (`:24-27`). `anim_frame` already resets per clip (`re15_actor.h:183-185`).
- **fix:** PSX `if (z->anim_frame == 0x28) zb_play_se(-2)`; PC use `(anim_frame>>1)` edge-gated. Wrap at the real em10 bank-1 clip-0x27 `frame_count`, not 69. Remove `i*23` (stagger emerges naturally per zombie's own feed-entry tick). Fix the false comment.

### `ZB-07` · MED — pursue/standup advance gated on invented `dot>0`
- **our loc:** `src/zombie_common.c:222,248`
- **wrong:** the zombie only steps forward when the player is in its forward hemisphere — an anti-drift hack (the comment admits "post-bite drift").
- **proof:** `FUN_80102bc8.c:33-40` calls the mover `func_0x800245d8(0)` **unconditionally** on the anim event; `:54` only re-aims at `0x10`/frame. No facing test.
- **fix:** remove the `dot>0` gate from PURSUE and STANDUP; always step along `rot_y` and rely solely on `zb_turn_toward(...,0x10)` to re-aim.

### `ZB-08` · LOW — uniform turn-rate `0x10` understates engage/grab slew
- **our loc:** `src/zombie_common.c:43-44,219,264,309`
- **wrong:** `ZB_TURN_RATE=0x10` is applied in every state.
- **proof:** PURSUE `0x10` is byte-true (`FUN_80102edc.c:17`), but head-up/rise (`FUN_801040e8.c:41`), grab (`FUN_8010383c.c:25`) and recover (`FUN_801035e8.c:42`) turn at `0x20`.
- **fix:** split into `ZB_TURN_RATE_WALK=0x10` (PURSUE/IDLE) and `ZB_TURN_RATE_ENGAGE=0x20` (STANDUP/ATTACK/RECOVER).

### `ZB-09` · MED — invented `ZB_GRAB_DIST=350` stop ring
- **our loc:** `src/zombie_common.c:37-38,222-224,244,268-271,297`
- **wrong:** a 350u "never translate past" ring caps every step; the comment calls it an anti-fly-through heuristic. No `350/0x15e` exists in any zombie handler (grep clean).
- **proof:** `FUN_80102bc8.c:31-41` (mover, no dist cap), `FUN_80101364.c:20` (handoff = `dist<2000`), `FUN_80102df8.c:16` (contact via `entity[0x1c4]&0x2000`).
- **fix:** drop the ring; transition to ATTACK on `dist<2000 && cone`, and let collision + the contact flag stop the close. Verify `re15_collision_constrain` treats the player as a blocker so the lunge doesn't overshoot.

### `ZB-10` · MED — PURSUE→ATTACK missing the ±0x2c8 facing cone
- **our loc:** `src/zombie_common.c:238-251`
- **wrong:** ATTACK is entered purely on `dist<=2000`; the zombie can launch at a player off to the side.
- **proof:** `FUN_80101364.c:20-22` requires `entity[0x1d0]<2000 && func_0x8001a9cc(&player,0x2c8)!=0` before `entity[4]=0x701`.
- **fix:** add `ZB_COS_CONE2_ATTACK` for ±62.6° (`(cos(0x2c8/4096·360°)·4096)² ≈ 3.54e6`) and gate the ATTACK entry on `dist<2000 && faced_attack`.

### `ZB-11` · LOW — pursue collapses two clips into one (no reach-forward 0x02)
- **our loc:** `src/zombie_common.c:70,234`
- **wrong:** `ZB_CLIP_WALK=0x01` for the entire PURSUE; the original also emits `0x02` (ROW2 reach-approach, `entity[0x1d4]`), savestate-proven (slot1 cycles `…0x01→0x02→0x09`, two zombies show `0x01`+`0x02` simultaneously).
- **proof:** `FUN_80102814.c:17,38` (`0x94=0x1d4`, sets ROW2), `FUN_801025c8.c:15`, `FUN_80102bc8.c:48` (`0x01`).
- **fix:** **do not wire yet** — the `0x01`/`0x02` selector is the runtime sub-state byte `entity[6]` (st6=03 vs 05) from runtime-filled tables, not a static distance. RE the st6 sub-state machine (or capture savestates with a locatable player) before coding. Visually low-impact.

### `ZB-12` · LOW — mover wall-constrains every step (player-band only)
- **our loc:** `src/zombie_common.c:136`
- **wrong:** `zb_step_forward` clamps via `re15_collision_constrain`, which is band-gated on the **player's** band — zombies get push-tested against player-band cells. `FUN_800245d8` adds raw rotated velocity with no clamp.
- **proof:** `ghidra1_V2.txt:106769-106778` (raw add, no collision), `FUN_80102df8.c:16-18` (walls handled separately via contact flag).
- **fix:** apply raw `nx/nz` in `zb_step_forward`; if walls matter, port the per-zombie contact-flag→FSM path. Low priority (room1140 is open-floor).

### `ZB-13` · LOW — stale/fabricated handler & clip citations
- **our loc:** `src/zombie_common.c:4-21`, `include/re15_zombie.h:16-17,31,33`
- **wrong:** header cites a non-existent `FUN_80104348` and clips `0x18/0x19/0x0C/0x1F` that no room1140 handler writes.
- **proof:** `FUN_801040e8.c:37`/`FUN_80104250.c:27` write clip `0x29`; feed `FUN_80103d48.c:20` (`entity[5]+6`). Executable constants (`0x29/0x01/0x09`) are already correct.
- **fix:** comment-only — replace the fabricated refs with the verified handlers/clips so future edits don't regress the correct constants.

---

## 🩸 LEON DAMAGE / GRAB (`player_damage_common.c`, `game_step_common.c`)

### `LE-01` · HIGH — entire mash-to-escape + kick-off mechanic is fabricated
- **our loc:** `player_damage_common.c:50-53,80-86,170-179`; `game_step_common.c:47-51`
- **wrong:** `PLY_GRAB_STRUGGLE_MAX=100`, decay, `re15_player_grab_struggle()`, the `>=MAX→kick-off` branch (`PLY_KICK_FRAMES=14`, `PLY_KICK_STEP=45`) are invented; the comment mis-cites `FUN_80011f50` (an **enemy** hit-target selector, no pad/counter).
- **proof:** `FUN_8011a1c8.c:39,53` (escape gate is `entity[0x95]==0x0f` anim-substate + range test `func_0x8001a780`, **no pad read**); grab/eaten are player states 5/6 (`PTR_LAB_80073f90[5]/[6]→LAB_80036834/800368c0`) running fixed sub-state FSMs (`DAT_800ac758/858`). The 6 savestates show hp100→-1 with no escape.
- **fix:** delete the struggle ticks + the 4 tunables. Drive break-free off `FUN_8011a1c8`: advance grab substate via the anim driver; at `0x95==0x0f` run `func_0x8001a780`; on success set `player[6]=7/player[0x95]=0x23` (kick) + `func_0x800453d0(7)`. Escape is anim-time + range gated, never input-mash.

### `LE-02` · HIGH — periodic self-timed grab bite is fabricated
- **our loc:** `player_damage_common.c:48-49,164-167` (`PLY_GRAB_BITE_PERIOD=40`, `PLY_GRAB_BITE_DMG=10`)
- **wrong:** a "bite every 40 frames for 10 dmg" drip with no disasm basis. The savestates show HP dropping in steps (100→90→65→40→-1) — multiple latch-gated hits, not a flat 40f/10 timer and not once-total.
- **proof:** the real eat drain is `FUN_801035e8.c:19-20` (HP-1/frame while mouth within `0x200`, attacker-driven, called every frame from `FUN_80102bc8`); discrete hits go through `FUN_80012d60.c:38` (`DMG_TABLE[type]` once, gated by i-frame latch `DAT_800acae7&1`).
- **fix:** remove the 40/10 block. Drive grab HP loss from the zombie overlay port — decrement HP by 1 each frame the grab is active AND mouth-within-`0x200` (cite `FUN_801035e8.c:19-20`). Keep discrete bites on the existing `FUN_80012d60`/`DMG_TABLE` path.

### `LE-03` · HIGH — kick-off uses wrong clip + tuned step instead of move-to-target
- **our loc:** `player_damage_common.c:52-53,183-186`
- **wrong:** kick plays WALK clip `0x0c` and pushes `45u`/frame for 14 frames.
- **proof:** `FUN_8011a1c8.c:18` plays clip `0x1c`; drives toward target `DAT_800aca88` via `func_0x8001a804(3000,0x100,&tgt)` (frame 4) and `func_0x8001bff8(...,800,&tgt)` (frame 0xf), ending on clip wrap.
- **fix:** play clip `0x1c`; replace the fixed push with move-toward-`DAT_800aca88` gated on the clip-0x1c keyframes; end on clip wrap. **Blocker:** `func_0x8001a804/8001bff8` not yet decompiled — confirm clip+model now, finalize the `3000/0x100/800` magnitudes after RE'ing those helpers (or a RAM trace).

### `LE-04` · HIGH — damage table over-read (12→16 entries) + wrong index mask
- **our loc:** `player_damage_common.c:14-16,106`
- **wrong:** `DMG_TABLE[16]` with bogus tail `{771,2569,3595,4111}` (= the adjacent `DAT_8006f430` byte array read as s16), and the index masks `type & 0x0f`.
- **proof:** `ghidra1_V2.txt:223453-223491` (`DAT_8006f418` = 12 s16 `{10,20,1000,1000,1000,50,100,200,300,1000,0,0}`); `ghidra1_V2.txt:77664` (`andi a0,s7,0xff` → `& 0xff`, not `0x0f`). `FUN_80012d60.c:78` proves `0x430` is a distinct byte array.
- **fix:** `static const int16_t DMG_TABLE[12] = {10,20,1000,1000,1000,50,100,200,300,1000,0,0};` and change the mask `& 0x0f → & 0xff`. **Do not** add a bounds guard — the EXE has none (it trusts callers 0–9); a guard would itself be non-byte-true.

### `LE-05` · MED — damage-HIT react clip is `0x02`, should be `0x0a`
- **our loc:** `player_damage_common.c:29,140`
- **wrong:** `PLY_CLIP_HIT=0x02` plays on the normal-hit react path; the comment (lines 110-116) already says `0x0a`, contradicting the define.
- **proof:** `ghidra1_V2.txt:131929-131931` & `132014-132016` (HIT substate handlers `FUN_80035b70`/`FUN_80035ca8` both write `0xa` to `entity[0x94]`=`DAT_800acae8`, consumed by `FUN_8001f314.c:8`).
- **fix:** set the damage-HIT clip to `0x0a`. The GRAB path (clips `0x01/0x02/0x06`, savestate-derived) is separate and unaffected.

### `LE-06` · MED — plain-death collapse clip is `0x06`, should be `7`
- **our loc:** `player_damage_common.c:30,203`
- **wrong:** `PLY_CLIP_DEATH=0x06` used for the state-3 plain-death path (the inline comment even says "clip 7 = fall").
- **proof:** `FUN_80036718.c:16` (`DAT_800acae8=7`, disasm `ghidra1_V2.txt:132747-132749`); state-3 reached on non-grab HP<0 (`FUN_80012d60.c:52-55`, `PTR_LAB_80073f90[3]`).
- **fix:** `PLY_CLIP_DEATH=7`. Keep `PLY_CLIP_EATEN=0x06` for the grab-driven state 5→6.

### `LE-07` · MED — HIT/DEATH/EATEN end on fixed frame counts, not clip-wrap
- **our loc:** `player_damage_common.c:35-36,54,145,198,207` (`PLY_HIT_FRAMES=24/PLY_DEATH_FRAMES=44/PLY_EATEN_FRAMES=50`)
- **wrong:** state termination (and the i-frame window) is hardcoded; the comments admit it.
- **proof:** HIT clears the latch when the react sub-FSM finishes (`ghidra1_V2.txt:132422`, root-motion counter `DAT_800acae0` init `0x3e8`, decay `~0xf`/frame); DEATH/EATEN advance `DAT_800aca5a += FUN_8001f314()` (clip-wrap) and flip to game-over (state 7) only on completion (`FUN_80036718.c:24-25`, `:8-10`).
- **fix:** end HIT→NONE and DEATH/EATEN→GAMEOVER on the actor clip-wrap (`cur >= clip->frame_count`, already exposed via `re15_actor.h` + EDD `frame_count`); for death decrement `DAT_800acae0` by `0xf`/frame. `44/50` are not real keyframe counts.

### `LE-08` · MED — fabricated knockback (no position write in original)
- **our loc:** `player_damage_common.c:38,118-125,144` (`PLY_KB_STEP=33`, `PLY_KB_FRAMES=6`)
- **wrong:** unit-vector pushback (`~198`, comment "byte-true 0xc8") plus a fabricated `0x1428` header value. The hit-react in the original writes **no** player x/z.
- **proof:** `FUN_80012d60` writes no position; the standard HIT handlers `FUN_80035b70/ca8` **zero** `DAT_800acae0` (`ghidra1_V2.txt:131937,132022`). `0xc8=200` is a recoil **counter** in unrelated react slots (`de0/f64`), never a per-frame position delta; `0x1428` appears nowhere.
- **fix:** remove the x/z knockback (`s_kb_x/z=0`, delete the `p->x+=s_kb_x` line). Byte-true standard bite = stagger clip `0xa`, no knockback. Delete the `0xc8`/`0x1428` comments.

### `LE-09` · MED — state-7 game-over red fade missing
- **our loc:** `player_damage_common.c:210-212`
- **wrong:** PLY_GAMEOVER is a bare freeze stub.
- **proof:** `ghidra1_V2.txt:132872-132917` (`LAB_8003694c`): seed `0x78`(120)-frame hold (`DAT_800acaf2`), set fade color (`DAT_800acb18/40` masked `0xff000000`|`0xff38`), each frame ramp `DAT_800acb10/12 += 0xc`, decrement counter, at 0 set substate `DAT_800aca59=2`.
- **fix:** implement the 120-frame red fade: seed counter, set the `0xff38` overlay color, `+0xc`/frame ramp on the two accumulators, mark fade-complete at 0. Bind to the port's screen overlay (PSX GPU fill / PC quad).

---

## 🔊 AUDIO (`audio_psx.c`, `audio_pc.c`, `vab_common.c`, `rotor_common.c`)

### `AU-01` · HIGH — enemy SE bypasses the snd1 EDT (hardcoded prog0/tone=ID+1)
- **our loc:** `audio_psx.c:217-220`; `audio_pc.c:482-485`
- **wrong:** `em_resolve_pt(vab,0,ID+1)` hardcodes program/tone; `s_em_edt=rdt->snd_edt[1]` is loaded but dead.
- **proof:** `FUN_800453d0.c:28,32-41` — `program=EDT[ID][1]&0x7f`, `tone=EDT[ID][2]>>4` from the room snd1 EDT (`DAT_800ac778+0x14`, = `rdt->snd_edt[1]`). Latent in room1140 (its EDT happens to be `{prog0,tone=ID+1}` for IDs 0-10) but IDs 17-21 use prog1, proving it's data-driven.
- **fix:** `vag = re15_se_resolve(rdt->snd_edt[1], &s_em_vab, ID, &tn)` with the raw situation ID; drop the hardcode + dead `s_em_edt`. (Note: bite=ID1, not ID2 as some comments say.)

### `AU-02` · HIGH — rotor distance fade: wrong vertical term
- **our loc:** `rotor_common.c:102-104`
- **wrong:** uses plain `dy² = (cam.Y - src.Y)²`. Original squares `(cam.Y - |cam.Y - src.Y|)`, dominated by `cam.Y`'s magnitude.
- **proof:** `ghidra1_V2.txt:157806-157826` (`s0=|cam.Y-src.Y|`; `v1=cam.Y-s0`; `v1*v1`); `FUN_80045a64.c:33-34`. For PSX cam.Y ≈ −1000s this changes the fade distance massively.
- **fix:** `ady = llabs(cam_eye[1]-heli_pos[1]); vy = cam_eye[1] - ady; r = isqrt(vy*vy + dx*dx + dz*dz)`.

### `AU-03` · HIGH — rotor output volume clamps instead of masks
- **our loc:** `rotor_common.c:130-131`
- **wrong:** `panL -= atten; clamp[0,0x7f]`. Original is `(value - atten) & 0x7f` (a wrap).
- **proof:** `ghidra1_V2.txt:157956,157964,157969` (`subu` then `andi 0x7f`, no clamp); `FUN_80045a64.c:72-73`. Diverges sharply on the `0x89` near channel (orig `(0x89-0)&0x7f=0x09` quiet vs our clamp→`0x7f` full).
- **fix:** `panL=(panL-(int)SE_ATTEN(di))&0x7f; panR=(panR-...)&0x7f;` — `0x89` must reach the subtract unclamped.

### `AU-04` · HIGH — rotor azimuth LUT ≠ PsyQ `catan` CORDIC
- **our loc:** `rotor_common.c:47-75`
- **wrong:** hand-built `ATAN256[257]` LUT diverges up to 7 catan-units; flips the SE_PAN index on ~29% of geometries.
- **proof:** `ghidra1_V2.txt:206403-206494` (12-iter CORDIC, table `DAT_80078c90={0x1FF,0x12E,0x9F,...}`); `FUN_80045d6c.c:23`.
- **fix:** port the exact 12-iteration CORDIC (`x=0x1000,y=z`; per-iter arithmetic shifts + the `CT[]` increments) with the truncating-division ratio. Cosmetic (rotor L/R balance) but a real byte deviation.

### `AU-05` · MED — Se_on played LINEAR (missing SPU square law)
- **our loc:** `audio_psx.c:308-316,335-344`
- **wrong:** `spu_vol = vol·0x3FFF/127` flat; the comment "LINEAR (user-validated)" is a tuned exception.
- **proof:** `SpuVmKeyOnNow.c:47-48` squares (`mag²/0x3fff`); the Se_on handler `LAB_80041624` (`ghidra1_V2.txt:151814`) routes through `FUN_80045024→SsUtKeyOnV→SpuVmKeyOnNow`.
- **fix:** route through the footstep square cascade (`se_vol_byte_true(0x7f, tone_vol)`). ⚠️ **User explicitly accepted the louder LINEAR Se_on** (memory 2026-06-18) — confirm audibly before applying; disasm is unambiguous it should be squared.

### `AU-06` · MED — PC Se_on plays at half the PSX level
- **our loc:** `audio_pc.c:383`
- **wrong:** PC applies a `>> 1` "headroom halve": `(vol*0x4000/127)>>1` → `0x2000` where PSX emits `0x3FFF`.
- **proof:** `SpuVmKeyOnNow.c:13` / `FUN_80045024.c:90` apply no `/2`; both ports map to the same `0x3FFF` SE ceiling.
- **fix:** `int q15 = vol * 0x3FFF / 127;` (drop the `>>1`). Restores PC↔PSX parity.

### `AU-07` · MED — rotor horizontal term keeps precision the original drops
- **our loc:** `rotor_common.c:104`
- **wrong:** uses exact `dx²+dz²`; original uses `floor(sqrt(dx²+dz²))²` (double `SquareRoot0`), shifting the SE_ATTEN index by up to 1.
- **proof:** `ghidra1_V2.txt:157816-157829` (two `SquareRoot0`, then `mult` = h·h); `FUN_80045a64.c:31-34`.
- **fix:** `h = rotor_isqrt(dx*dx+dz*dz); r = rotor_isqrt(vy*vy + h*h);`.

### `AU-08` · MED — note2pitch ignores the doubled tone pitch_shift
- **our loc:** `vab_common.c:214-215` (+ callers `audio_psx.c:243/292`, `audio_pc.c:351`)
- **wrong:** the SE/enemy key-on path is `note2pitch2`, which sums the played fine **plus** the tone's `pitch_shift` (carry into the semitone) before `>>3`; ours applies `pitch_shift/8` once. Byte-true only for room1170 (all footstep tones `pitch_shift=0`); wrong for room1140 enemy tones (shift 18/57/60).
- **proof:** `SsUtKeyOnV.c:83` (calls `note2pitch2`), `ghidra1_V2.txt:184904-184906` (sums fine+tone[+5] before `>>3`), `FUN_80045024.c:90`.
- **fix:** `fine_total=(2*pitch_shift)>>3`; if `>=16` carry +192 into the semitone, low fine `= combined-16`; keep the negative-fine `+7` rounding bias.

### `AU-09` · LOW — footstep note missing the `+ (rand & 2)` jitter
- **our loc:** footstep caller `re15_audio_footstep` (`audio_psx.c:350`, `audio_pc.c:1476`)
- **wrong:** passes bare `tn->min_note`; the original detunes footsteps by 0-or-2 semitones.
- **proof:** `FUN_80045630.c:70-71` (`min_note + (FUN_8001af20() & 2)`); siblings `FUN_80045024.c:115`/`FUN_800453d0.c:57` use bare min_note (so apply **only** to footsteps).
- **fix:** `note = tn->min_note + (re15_rand() & 2)` in the footstep keyer only (`re15_rand` = port of `FUN_8001af20`).

### `AU-10` · LOW — VAB alternate-bank flag documented on wrong byte + cross-bank override unimplemented
- **our loc:** `include/re15_vab.h:48-49`, `vab_common.c:171-176`
- **wrong:** comment puts the alternate-bank flag on `e[1]` bit7; it's `edt[0]` bit7 (`vabId=edt[0]&0x7f`). `e[0]` is never read.
- **proof:** `FUN_80045024.c:66-72` (`edt[0]&0x80→vabId`; program=`edt[1]&0x7f`, `edt[1]` bit7 unused).
- **fix:** correct the comment; optionally read `e[0]&0x80` in `re15_se_resolve` for cross-bank SE. Footstep path (fixed vabId) is already byte-true; no demonstrated cross-bank SE fires today.

---

## 🧱 COLLISION (`re15_collision.c`)

### `CO-01` · MED — resolver drops the SCA section origin
- **our loc:** `re15_collision.c:53-55,67-72,114-115,298-299`
- **wrong:** quadrant/broad-phase/push all use origin=0; the resolver should add the per-section origin to the player x/z before each cell compare.
- **proof:** `FUN_8003b0a4.c:36-39,57-60,96-99` (`*psVar13` folded in), `FUN_8003bca8.c:15`, `FUN_8003d6a8.c:13-14`, `FUN_8003b068.c:4-5`. The floor query `FUN_8003b7f0` genuinely zeroes it (so `re15_collision_on_floor` is correct).
- **fix:** RAM-dump `*(u32*)(DAT_800ac784+0x7c)[0/2]` in a non-1170 room; if nonzero, expose `rdt->sca_origin_x/z` and add it at all four resolver sites. Keep origin=0 until proven (room1170 verified); treat non-1170 collision as unverified.

### `CO-02` · MED — props' Y-overlap gate omitted (horizontal push runs unconditionally)
- **our loc:** `re15_collision.c:340-350`
- **wrong:** prop push fires on X/Z overlap alone; the original gates it on Y-overlap too (`bVar1`).
- **proof:** `FUN_8002cabc.c:39-40` (`if(bVar1)` wraps the whole horizontal push, plus swept-Y branch `:49-71`). Room1170-only simplification (flat helipad → Y always overlaps).
- **fix:** `pl->y` is available; props carry `box_hy/box_cy`. Compute `dy`, gate the push on `|dy| <= box_hy + player_Y_halfextent` (find `psVar7[4]` ≈ `DAT_80073e94`). Horizontal math itself is already byte-true.

### `CO-03` · LOW — slope/diagonal cell types 2/4/5/6/7 not handled (latent)
- **our loc:** `re15_collision.c:300-308`
- **wrong:** player passes through types 2/4/5/6/7 cells.
- **proof:** dispatch table `0x800b2858` maps them to real handlers `LAB_8003d00c/8003beb0/8003c734/8003cb9c/8003c2cc` (`ghidra1_V2.txt:142347-142367`). Room1170 has only types 1/3 (150 cells verified) → zero impact there.
- **fix:** decompile + port the 5 handlers. Required only before any slope room is claimed byte-true.

---

## 🎥 CAMERA (`camera_common.c`)

### `CA-01` · MED — cinematic orbit cam-pos skipped (held static)
- **our loc:** `camera_common.c:359-372`
- **wrong:** cam_pos held at `cut.pos`; the comment blames a "~14% loss" in the trig round-trip.
- **proof:** `FUN_80015850.c:9-15` recomputes per-frame `cam.x=(rcos(yaw)·dist>>12)+tgt.x`, `cam.z=(rsin·…)+tgt.z`. Init seeds `yaw_step=0x384` and `dist_step=(dist-0xce4)/3` (`ghidra1_V2.txt:81356,81347-81360`) — both nonzero, so it genuinely orbits.
- **fix:** port the formula **and** seed `yaw_step=0x384`/`dist_step` at init, using a PSX-accurate `ratan2/rcos/rsin` (the "14% loss" was the approximate `cam_atan2_q12`, not the formula). Focus-track divisors `/60,/20` already match.

---

## 🪜 STAIRS (`stair_common.c`)

### `ST-01` · HIGH — invented 48-step forward-eject march in finalize
- **our loc:** `stair_common.c:140-168` (incl. `:159-160` `+100u` nudge)
- **wrong:** finalize repositions the player along `rot_y`; the comment admits "the original's finalize does NOT reposition the player at all".
- **proof:** `ghidra1_V2.txt:136857-136893` (`LAB_80038e50`: clip2, cursor=0, control restore, band=`-DAT_800acc0e/0x708` — **no** writes to `entity[0x34]/[0x3c]`).
- **fix:** finalize does ONLY: motion=clip2, `anim_frame=0`, control restore, `p->y=target_y`, `band=from_y(p->y)`. Drop the whole march + the `+100u` nudge; rely on the next frame's `FUN_8003b0a4` to eject. The per-band forward+FK travel must overshoot the dest wall-cell midpoint (correct `FUN_800245d8`/`FUN_800390e0` constants) or this stays a documented hack.

### `ST-02` · HIGH — stair entry band gate too narrow (1 of 4 cases)
- **our loc:** `stair_common.c:300-304` (`a->band == cur`)
- **wrong:** accepts a stair only when `zone.band == player_band` (margin 450). Original scans 4 band relations across 2 passes.
- **proof:** `FUN_8002d474` loop1 `{cur+1, cur-2}` margin `0xc8=200`, loop2 `{cur-1, cur}` margin `0x1c2=450` (`ghidra1_V2.txt:120491-120503,120565-120578`); compare in `FUN_8002da4c` (margin added to both extents).
- **fix:** Pass A (margin 200) accept `{cur+1, cur-2}`; Pass B (margin 450) accept `{cur-1, cur}`, ordered (first match wins). The two passes are mutually exclusive via `FUN_8002d2c0` — reproduce that selector for full exactness. Re-validate the 2026-06-08 top-stair-vs-door guard.

### `ST-03` · MED — only the 450 margin implemented (missing the 200 pass)
- **our loc:** `stair_common.c:39,75-84` (`STAIR_REACH=450`)
- **wrong:** `point_in_zone`/`player_in_zone` only know margin 450 (plus a non-original margin-0 probe).
- **proof:** `ghidra1_V2.txt:120492` (`0xc8` loop1) vs `120566/120579` (`0x1c2` loop2).
- **fix:** parameterize the margin per pass (folds into ST-02).

### `ST-04` · MED — wrong ascend stair clip
- **our loc:** `anim_select_common.c:218`, `stair_common.c:186` (ascend→clip 20)
- **wrong:** ascend uses clip `20`; it was an uncited "USER-CONFIRMED" guess (and the audit's own `0x14=20` guess was also wrong).
- **proof:** ascend `LAB_80038850` phase-0 writes clip `0x5` (`ghidra1_V2.txt:136465-136467`); descend `LAB_80038c60` writes `0x15=21` (`:136747`, correct).
- **fix:** change the ascend branch `?21:20 → ?21:5` in both files; cite `800388a4`/`80038cb4`; fix the stale "ascend=clip 20" comments.

### `ST-05` · MED — FK foot-lock uses a per-tick cache instead of the pose reference field
- **our loc:** `stair_common.c:196-233` (`s_footref`, reset per band `:249`)
- **wrong:** subtracts the foot's per-tick world delta vs a local cache; original subtracts `(curFoot - s3[0x54/58/5c])` where `s3[0x54..]` is the pose's per-frame **reference** foot (re-read each call, never written by this fn).
- **proof:** `ghidra1_V2.txt:137100-137126` (`p[0x34]-= local-s3[0x54]`); same pattern in the walk foot-lock `FUN_800369f8`. No per-tick snapshot store exists anywhere.
- **fix:** each tick compute both the current locked foot and the same-frame reference foot (the bone field the FK builder writes at `entity[0x188]+footsel*0x204+0x2b0+0x54`), subtract every tick with no `s_footref` carry/per-band reset. Pin the `0x204`-block writer first.

---

## 📜 SCD VM (`scd_vm.c`, `scd_room_setup.c`)

### `SD-01` · HIGH — `Sce_em_set (0x44)` spawns unconditionally (ignores the progress flag)
- **our loc:** `scd_vm.c:2247-2248`
- **wrong:** every actor activated; `pc[7]` (spawn flag) never read. For room1170 the 7 crows have `pc[7]=0x32..0x38` (conditional on progress); Elliot `pc[7]=0xFF` (always).
- **proof:** `ghidra1_V2.txt:152530-152544` (`pc[7]==0xff→spawn`; else `FUN_8004efe4` bit-test → if flag SET suppress + run `pc[5]` callback; `pc[1]&0x80` bail). Flag bank `DAT_800b1038`, or `DAT_800b1058` when scenario word `>=3`.
- **fix:** before `a->active=1`: read `fl=pc[7]`; if `fl!=0xFF`, pick the flag zone by scenario word, `if (re15_game_flag_get(zone,fl)) skip`; `if (pc[1]&0x80) skip`. `re15_game_flag_get` already mirrors `FUN_8004efe4`.

### `SD-02` · HIGH — `Member_set` ids 6-13 mistranslated (`re15_to_re2`)
- **our loc:** `re15_to_re2.c:52-59`, `actor_common.c:91-95,137-141`
- **wrong:** the `0x0B..0x25` blanket pass-through routes RE1.5 status/flag/model bytes to RE2 coordinate/rotation fields. Concretely: id6→sub_state_4 (should be entity type @+0x00, 32-bit), id7 dropped (subtype @+0x0C), id8→`set_motion` (should be u8 @+0x04, **never** the clip), id12→`a->y` (should be u8 @+0x09), ids 9/10/13 dropped/misrouted.
- **proof:** `FUN_8004116c.c:28-68` (the canonical id→field map). The clip `+0x94` is written **only** by `Plc_motion` (`LAB_80041b90`, `ghidra1_V2.txt:152132`). Real callers: room1040/1041 `Member_set(9,15)/(10,0)`, room1020/1070 `Member_set(12,138)`, room11B0 `Member_set(6,513)`.
- **fix:** delete the `0x0B..0x25` pass-through; map each RE1.5 id explicitly to its struct field: 6→type(32b), 7→subtype(32b), 8→u8 state@+0x04 (**not** clip), 9→+0x05, 10→+0x06, 0xB→+0x07, 0xC→+0x09, 0xD→+0x08, 0xE→+0x0a, 0xF→+0x0b, 0x10→s16@+0x1c4, 0x11→s16@+0x98, 0x12→u8@+0x82, 0x13→s16@+0x1ba. ids 0x14+ → return −1 (RE1.5 drops them). Only `op_plc_motion` may call `set_motion`. Room1170 only uses ids 0/1/2/4/18 → mostly latent, but **`Member_set(18,4)`=id 0x12 runs in room1170 sub14 and is silently dropped today** (should write `entity+0x82`, a flag read by `FUN_80031c44.c:15`) — fix id 0x12 first.

### `SD-03` · HIGH — `Cmp (0x23)` reads var from `pc[1]`, should be `pc[2]`
- **our loc:** `scd_vm.c:2105`
- **wrong:** work-var index from `pc[1]` (a separate selector byte); effectively always compares `work_vars[0]`.
- **proof:** `ghidra1_V2.txt:150019-150030` (`lhu v1,0x2` → `andi 0xff` = `pc[2]` index, `srl 0x8` = `pc[3]` op). Contrast `Switch(0x13)` which genuinely reads `pc[1]`.
- **fix:** `var_idx = t->pc[2]`. ⚠️ A prior `pc[2]` trial **crashed PSX boot** (a real boot-path Cmp branch was masked by the wrong var) — land only behind a DuckStation-verified boot (≥40s graceful); PC build can take it immediately.

### `SD-04` · HIGH — `Cmp (0x23)` operator table: cases 2/3/4 permuted
- **our loc:** `scd_vm.c:2112-2118`
- **wrong:** ours `{2:<, 3:<=, 4:>=}`; original `{2:>=, 3:<, 4:<=}`.
- **proof:** `ghidra1_V2.txt:150042-150066` (`switchD_8003ffc0`).
- **fix:** reorder to `0:==,1:>,2:>=,3:<,4:<=,5:!=,6:&`.

### `SD-05` · MED — `Member_cmp (0x3E)` operator table wrong (4 ops, mis-mapped)
- **our loc:** `scd_vm.c:2139-2145`
- **wrong:** ours `{0:==,1:!=,2:<,3:>}`; ops 4/5/6 fall to false.
- **proof:** `ghidra1_V2.txt:151444-151468` — same 7-op table as `Cmp`.
- **fix:** `0:==,1:>,2:>=,3:<,4:<=,5:!=,6:(cur&arg)!=0, default:0`.

### `SD-06` · MED — `Member_cmp (0x3E)` reads compare value big-endian
- **our loc:** `scd_vm.c:2132`
- **wrong:** `scd_read_be_s16(&pc[4])`; original `lh` = little-endian.
- **proof:** `ghidra1_V2.txt:151424` (`lh s1,0x4(v0)`).
- **fix:** `scd_read_le_s16(&pc[4])`; drop the "mixed-endian" comment. (`member_id=pc[2]`, `cmp_op=pc[3]` already correct.)

### `SD-07` · LOW — `op_case (0x3E case-skip)` off-by-2 on no-match
- **our loc:** `scd_vm.c:1115`
- **wrong:** no-match advances `Case_addr + block_length + 4`; correct is `+6` (lands 2 bytes inside the prior case body).
- **proof:** `ghidra1_V2.txt:149711-149714` (unconditional `addiu a3,a3,0x6` in the delay slot, then `addu a3,a3,t0`). Data: room1030 switch lands on the next Case only with `+6`.
- **fix:** change `t->pc += skip + 4` → `t->pc += skip + 6`. Latent in room1170, live in room1010/1011/1030.

### `SD-08` · MED — `Se_on (0x36)` field layout wrong (selector on wrong byte)
- **our loc:** `scd_vm.c:1140-1162`, `audio_psx.c:337` (`bank != 2` gate)
- **wrong:** bank=`pc[1]`, sample_id=`pc[2]`; the 0..5 bank-type **switch selector** is actually `pc[4]`, the emitter id is the `pc[2..3]` halfword.
- **proof:** `ghidra1_V2.txt:151744-151747` (`lh a1,0x4(s0)`; `andi a2,a1,0xff`=`pc[4]`; `sltiu v0,a2,0x6` switch; `lbu a3,0x1`=`pc[1]`; `lh a0,0x2`=`pc[2..3]`).
- **fix:** selector=`pc[4]`, id=`pc[2..3]` halfword; re-point the `play_se_on` gate from `pc[1]` to the `pc[4]` selector (or port the full 0..5 case→bank resolution). Pos LE (`pc[6/8/10]`) and `PC+=12` already byte-true.

### `SD-09` · MED — room re-entry doesn't seed work_vars[0..3] = −1
- **our loc:** `scd_vm.c:336` (init) + `scd_room_setup.c:112` (re-enter)
- **wrong:** memset leaves `work_vars[0..3]=0`; the original room-reload chain writes `0xFFFF`.
- **proof:** `FUN_8003ebf4.c:4-8` (`DAT_800b0fd0/2/4/6=0xffff`), runs on every room load (`FUN_800396fc.c:67→FUN_8003ef6c.c:5/18`). Observable: rooms 1190/1191 sub01 `switch(0)` with a `-1`-guarded case 0 → our `0` fires a spurious `Evt_exec(10,0x1802)`.
- **fix:** after both memsets, `work_vars[0]=work_vars[1]=work_vars[2]=work_vars[3]=(int16_t)-1`. Leave `work_vars[10]` (scenario) untouched.

---

## 🦴 SKELETON / RENDER (`skeleton_common.c`, `pri_common.c`)

### `SK-01` · MED — `mat3_from_euler` single-shift Q36 ≠ original per-product truncation
- **our loc:** `skeleton_common.c:124-162` (rows m[3],m[4],m[6],m[7])
- **wrong:** accumulates both terms in Q36 and shifts once (`>>24`); original `RotMatrix` truncates each product to 12 bits **separately** (triple product truncated twice), then sums the shorts → ours differs by ~1-2 LSB/element. **Consistent with the user-reported recurring pixel shift.**
- **proof:** `RotMatrix.c:34-39`; `ghidra1_V2.txt:210107-210167` (each `multu`→own `sra 0xc`→`sh`, then `addu/subu`).
- **fix:** per off-diagonal, pre-truncate the inner product: `iv=(short)((cz*sy)>>12); m=(short)((sz*cx)>>12) ± (short)((iv*sx)>>12)`. On PSX call a `RotMatrix` that builds `Ry*Rx*Rz` with this order (not PSn00bSDK's).

### `SK-02` · LOW — PSX trig backend ≠ game's `DAT_800794c4` table
- **our loc:** `skeleton_trig_psx.c:10-11` (PSn00bSDK `isin/icos`); PC uses `sinf/cosf` (`skeleton_trig_pc.c:17-29`)
- **wrong:** PSn00bSDK `isin` is a polynomial approx; the original uses the 4096-entry LUT `DAT_800794c4`. They differ at the LSB on early entries (e.g. idx1 sin 6 vs 7).
- **proof:** `ghidra1_V2.txt:263165-263251` (table xref'd by all RotMatrix*/FGO_*_OBJ), unpack at `209988-209994`; PSn00bSDK `isin.c:11-28`.
- **fix:** extract `DAT_800794c4` (4096 words: low16=sin, high16=cos, Q12) and back both `re15_sin_q12/re15_cos_q12` with direct table reads on **both** ports.

### `SK-03` · MED — `sprite.pri` mask cap (64) silently drops masks
- **our loc:** `include/re15_pri.h:38`, `pri_common.c:116`
- **wrong:** `RE15_PRI_MAX_MASKS_PER_CUT=64`; real data exceeds it (room1190: 79, room1210/1211: **149**). Masks 64..N never draw → missing FG occlusion.
- **proof:** `FUN_800392d4.c:38-110` (no 64-cap; group-driven). Verified against extracted `sprite.json`.
- **fix:** raise the cap to **256** (matches the PSX declared-count ceiling `(char)(uVar2>>0x10)` and the existing parser guard at `pri_common.c:50`); 128 from the original note is too small (drops room1210). Resize `masks[]` and the PC stack arrays (`main.c:952-955`).

### `SK-04` · LOW — `sprite.pri` parser is mask-driven, not group-driven (latent)
- **our loc:** `pri_common.c:75,108-114`
- **wrong:** loops `hi-u16` (the OT-emit count) as the parse bound; original loops groups outer / `group_n` inner (parses `sum(group_n)`). `hi-u16` is only the emit count (`FUN_80039590`).
- **proof:** `FUN_800392d4.c:49-110` vs `FUN_80039590.c:9,12,23`.
- **fix:** **leave it** — verified across all 480 sections: 478 have `hi==sum`, the 2 that differ (room1210/1211 cut4: hi=75,sum=77) produce byte-identical emitted pixels. Zero observable effect game-wide; only refactor if a byte-true parse model is wanted.

---

## 🎒 INVENTORY (`inventory_common.c`)

### `IN-01` · MED — `MAX_SLOTS=11` with fabricated justification
- **our loc:** `include/re15_inventory.h:19`
- **wrong:** hardcoded 11 with a comment citing `DAT_800d46ac` (which is unreferenced BSS, not the count). Real count is dynamic `DAT_800b0fbc`, branched on exactly 8 or `0xa(10)`; max valid index is 9.
- **proof:** `ghidra1_V2.txt:160363-160366,161427-161429`; `DAT_800d46ac` has zero xrefs (`640245`).
- **fix:** store the live count (init 8, → 10 with side-pack); size the array 10; never treat index 10 as valid; delete the false comment. Currently latent.

### `IN-02` · MED — add policy is "stack onto same id", original is class-split
- **our loc:** `inventory_common.c:26-46`
- **wrong:** same-id qty-merge has no counterpart; non-stackables go to first free slot via `DAT_800b0fbc`-bounded search, stackables right-shift all slots into a paired double slot.
- **proof:** `inventory_add_item.c:26-95`; free-slot loop `FUN_8004df2c` (`ghidra1_V2.txt:168629-168654`). The cited `FUN_8006947c` does not exist.
- **fix:** implement both paths: `class=(uint8_t)(type-0x0E)>5`; non-stackable→first free in `[0,count)`; stackable→shift + paired slot (flags 1/2, count+=2). Remove the same-id merge + the bogus cite. (Reconcile with the project's confirmed 4-byte/11-slot flat model — the paired-slot scheme may be deliberately simplified, but the merge + bound are deviations regardless.)

---

## 💬 MESSAGE / TYPEWRITER (`msg_common.c`)

### `MS-01` · MED — typewriter ~2× too fast (wrong scroll/timer defaults)
- **our loc:** `msg_common.c:339-340`
- **wrong:** hardcodes `scroll=2,timer=1` (the menu-mode `bVar2=0` branch). In-game `DAT_800b5456==0` → `bVar2=1` → `scroll=2<<1=4`, `timer=1<<1=2`.
- **proof:** `ghidra1_V2.txt:112735-112767`; `FUN_80028134.c:14,19,20`.
- **fix:** for in-game dialog set `scroll=4,timer=2`; ideally carry the display-mode flag and derive `bVar2=(display_mode==0)`.

### `MS-02` · LOW — typewriter handler missing control codes 0x06/0x07/0x09/0x0A/0x0B
- **our loc:** `msg_common.c:368-398`
- **wrong:** only `0x01-0x05,0x08` handled; `0x09/0x0A/0x0B` render as garbage glyphs, `0x06/0x07` (substring call/return) misrender.
- **proof:** `FUN_80028134.c:93-110`; `ghidra1_V2.txt:112920-112944`.
- **fix:** add `0x09/0x0A/0x0B → parse+=2`; `0x06 → substring (or +=2)`; `0x07 → return`.

### `MS-03` · LOW — fast-forward reveals 1 glyph/frame instead of 2
- **our loc:** `msg_common.c:369-398`
- **wrong:** on hold-to-skip our loop breaks after one glyph; the timer `-4` alone doesn't compensate → half the original skip speed.
- **proof:** `FUN_80028134.c:30,40,187-192` (`iVar8=2` budget, `LAB_80028434` loops a 2nd glyph). Default `scroll=2(<4)` so this is the normal case.
- **fix:** give the reveal loop a budget (2 when `held && scroll<4`, else 1), decrement per printable glyph, stop at control codes.

### `MS-04` · LOW — `0x04` scroll-speed handler ignores the arg==0 forward scan
- **our loc:** `msg_common.c:392`
- **wrong:** on arg 0 we leave scroll unchanged + skip 2; original scans forward to the **next** `0x04` and uses its arg.
- **proof:** `FUN_80028134.c:69-88`; `ghidra1_V2.txt:112870-112911`.
- **fix:** on arg 0, walk control codes (5/6/9/A/B = 2-byte) from `parse+2` to the next `0x04`, take its arg `<<bVar2` as scroll, advance past it.

### `MS-05` · LOW — typewriter fast-forward bound to SQUARE, original is CROSS
- **our loc:** `game_step_common.c:40` (`g_scd_action_held = SQUARE 0x8000`), consumed `msg_common.c:370`
- **wrong:** original gates fast-forward on `DAT_800ac768 & 0x4000` = CROSS; input layer is the unmodified PSX pad word on both sides (not remapped).
- **proof:** `ghidra1_V2.txt:112786-112788`; `FUN_80028134.c:29`; `re15_player.h:41` (CROSS=0x4000).
- **fix:** bind the held fast-forward to `RE15_PAD_BIT_CROSS` (0x4000). The edge/confirm path (still SQUARE) is a separate unproven question — verify against the original confirm gates first.

---

## 💡 LIGHTING (`light_common.c`)

### `LI-01` · LOW — `re15_isqrt` ≠ libgte `SquareRoot0` / `VectorNormal`
- **our loc:** `light_common.c:11-34,124,169`
- **wrong:** exact-floor isqrt; `SquareRoot0` is a 192-entry-table 12-bit-mantissa **approximation** (e.g. `SquareRoot0(289)=16` vs floor `17`). A ±1 dist error shifts 8-bit RGB falloff output.
- **proof:** PSn00bSDK `squareroot.s:58-94` + `vectornormals.s:11-79`; `FUN_80053fc0.c:39`, `FUN_800542dc.c:7`.
- **fix:** port the exact `SquareRoot0` (LZCR normalize + `sqrt_table[]` >>12, with the `0xa1d5`→`0x1ad5` typo corrected) and `VectorNormalS` (`_norm_table[]`). Intent (FPU-less) is fine; only the table result must match. Low priority.

---

## ENGINE-SPECIFIC (left as-is)

| item | loc | why kept |
|---|---|---|
| Room1140 BGM silence override (`stage==0 && room==0x14 → -1`) | `audio_psx.c:713` | Contradicts the table (`MAIN16`), but **user-validated 2026-06-18** the briefing room is silent + MAIN16 masked the feeding SE. SS_BGMTBL is a reconstructed-proto artifact; no real data gate exists to replace the hardcode. |
| HIT/EATEN react clips `0x02/0x06` as `#define` | `player_damage_common.c:29-30,47` | Savestate-derived (not EXE literals); EXE queues from a runtime motion table. Clip ids are evidence-backed; selection mechanism abstracted. |
| RVD/CAM_SWITCH on level + `cut_auto_enabled` | `aot_common.c:285-316` | A **separate** per-frame RVD camera scan (`FUN_80014230`), not the SCE dispatch — correctly modeled as architecture. |
| `push_circle` 64-bit sum-of-squares | `re15_collision.c:116` | Original is 32-bit (overflows past ~46340u). Behaviorally identical in-room; harmless robustness gain. |
| FOV upper clamp only (no `0xd0` H-clamp) | `camera_common.c:66-95` | The `0xd0` clamp exists **only in RE2** (a per-frame animator), not RE1.5. RE1.5 feeds `fov>>7` to `gte_ldH` unclamped. Harmless for current rooms. |
| Player transient-locomotion reset (200 IDLE sentinel) | `scd_room_setup.c:133-142` | Cleanup of our own persistent-walker abstraction; original has no walker latch. |
| RVD zones at AOT slot base 16 | `scd_room_setup.c:88` | Layout workaround for static-then-script install ordering; original installs in script order. |
| Room-load step order (SCD before audio banks) | `room_common.c:102,135,139` | Disasm differs, but our audio path is **queue-deferred** (drained next frame after banks are resident) → no actual stale-bank access. |
| snd0+snd1 per-room reload | `room_common.c:139` | Mechanism matches `FUN_80043eac/fb0`; only the order is the (neutralized) deviation above. |
| `ssx_all_keyoff` hard-zeros CH_VOL | `audio_psx.c:483-499` | Deliberate fix for the SPU-region-rewind DMA race (BGM ringing after room change). |
| SUB (rotor) BGM master `0x1400`, LINEAR | `audio_psx.c:847,910` | libsnd-less synth; original level path is a runtime `SsSeqSetVol` table, not a hardcoded master. Tuned, no clean cite. |
| Rotor base master drift PSX `0x1400` / PC `0x1a00` | `audio_pc.c:197` | Both uncited synth-ambience masters; non-proportional but neither is byte-true. *(Candidate to harmonize, but no disasm anchor.)* |
| Voice-clip auto-leveling (PC) | `audio_pc.c:733` | Synchro English VO is the project's own AI-TTS; RE1.5 ships no English voice — nothing to match. |
| BGM synth path (software SsSeq + ADSR + reverb) | `audio_pc.c:218` | SDL has no SPU; control logic (MIDI/note2pitch/square-law/CC7/table) is shared. Accepted libsnd-less path. |
| ADPCM decode (no shift>12 clamp, loop-start resets predictor) | `vab_common.c:288-305` | PSX decodes VAG in **hardware** (no software decoder in disasm); this C runs only on the PC SDL mixer and matches the Java reference. |
| Mask OT bucket `depth*2` | `re15_pri.h:78` | Consequence of the accepted RE2 mesh bucketing (`otz>>3` into 1024-OT) vs RE1.5's `otz>>4`/4096-OT; keeps the mask at the same relative depth. |
| `re15_actor_footstep` returns 0 on `anim_freeze` | `anim_select_common.c:81` | `anim_freeze` is a port construct; frozen ⇒ no foot-plant is behaviorally consistent. |
| Slot 0 reserved for player | `actor_common.c:36-53` | Chosen pool layout; no observable effect on the audited rooms. |
| PC Y-trace `fprintf` | `actor_common.c:120-125` | Diagnostic behind `RE15_PLATFORM_PC`, no behavioral effect. |
| HUD pickup-echo (`last_pickup_*`, 90f) | `inventory_common.c:9,29-42` | Invented HUD state; original add routine's only side effect is an icon VRAM blit. Doesn't corrupt the byte-true slot layout. |
| Voiced timed-subtitle path gated `player_mode==2` | `scd_vm.c:1070` | Added voice-subtitle subsystem; gating to cutscene-only preserves the original universal typewriter FSM for all gameplay text. |
| `room1170_subtitle` English strings | `re15_to_re2.c:138-162` | RE2-port presentation layer (verbatim from the room's `.msg`); RE1.5 has no English subtitle track. |

---

## TOP 10 FIXES (do first)

1. **`LE-01` + `LE-02` (grab is fully fabricated).** Delete the mash/kick/40-frame-bite mechanic; drive break-free off `FUN_8011a1c8` anim-substate + range test, and grab HP loss off `FUN_801035e8` (HP-1/frame within `0x200`). Biggest behavioral lie, directly contradicts the 6 user savestates.
2. **`SD-02` (Member_set id translation).** Fixes silently-dropped/misrouted writes across rooms 1020/1040/1041/1070/11B0 **and** the `Member_set(18,4)` dropped in room1170 sub14. High blast radius, mostly comment+table work.
3. **`SD-04` + `SD-05` + `SD-06` (Cmp/Member_cmp operator tables + endianness).** Wrong comparisons silently mis-branch scripts. Trivial, self-contained, shared PSX+PC.
4. **`AU-01` (enemy SE → EDT).** One-line switch to `re15_se_resolve`; makes enemy audio data-driven/byte-true across all rooms.
5. **`SK-01` (RotMatrix per-product truncation).** The ~1-2 LSB matrix drift is the most likely root of the **user's recurring pixel-shift complaint** — highest-value visual fix.
6. **`ST-01` + `ST-02`/`ST-03` (stair finalize march + entry band gate).** Remove the invented eject march; restore the 4-relation/2-margin entry scan. Fixes the wall-hang heuristics with real disasm.
7. **`ZB-01` + `ZB-02` (zombie pursue/lunge speed).** Walk `0x474` (keyframe-gated), lunge `0x100` — the zombies currently crawl at ~½ and ~⅛ speed. Most visible zombie deviation.
8. **`ZB-03` + `ZB-04` + `ZB-10` (zombie engage gates).** Add the ±45°/±62.6° cones, engage-lock, and correct radii so feeders rise one-at-a-time facing the player and don't lunge off-axis.
9. **`AU-02` + `AU-03` + `AU-06` (rotor vertical term, mask-vs-clamp, PC half-level).** Three confirmed HIGH audio porting bugs with concrete one-block fixes.
10. **`LE-04` (damage table over-read + mask).** Shrink to 12 entries + `& 0xff`; removes a latent garbage-damage read. Cheap correctness.