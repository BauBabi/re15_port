# Cross-Class 1:1 Audit — Confirmed Deviations (2026-06-24)

## Intro

This is the finalized cross-class 1:1 audit of the RE1.5-reborn engine vs the byte-true
decompiled source. Of 104 raw candidate deviations across 24 subsystems, **36 survived BOTH
verifier lenses** (an adversarial decomp-faithful reading AND a real-effect reading both agreed
each is a real, exercised, non-architecture deviation).

**Dedup result: 36 confirmed, no exact duplicates.** Every entry has a unique `id`. Several share a
subsystem (the SCD VM and collision in particular); those are kept SEPARATE because each is an
independent code site with its own fix. They are grouped by subsystem below.

**Severity counts (deduped):** HIGH = 11, MED = 19, LOW = 6.

Each deviation lists: our behavior, the source citation (disasm addr / file-byte offset), the
byte-true source behavior, the exact fix, and where it is exercised.

> Validation gates noted inline: `scd-evtexec-slot-alloc` (re-validate the room1170 helicopter
> regression), `plr-move-velocity-vs-rootmotion` (blocked on root-bone read-back),
> `pad-no-logical-remap-layer` (verify per-bit vs a dialog savestate; supersedes two MED swaps),
> `anim-frac-seed-hardcoded-7` (map each enemy site's overlay seed before substituting).

---

## Subsystem: mesh / GTE pipeline / OT bucket (mesh_psx.c, render.c)

### [HIGH] ot-bucket-shift-3-vs-4 — OT bucket uses otz>>3, source uses otz>>4
- **Our behavior:** `mesh_psx.c:335 int bucket = (int)(otz >> 3);` (~8 OTZ/bucket, otz pre-clamped 0x1FFF).
- **Source:** FUN_80022f0c.c:51 `(uVar2>>4)*4`; FUN_800254a0.c:64; FUN_800256b0.c:75; shadow FUN_8001b064.c:91 `(uVar3>>4)`. The `*4` = byte offset into a WORD OT (stride 0x1000B = 0x400 words/actor).
- **Source behavior:** ALL RE1.5 mesh/shadow renderers index the per-actor OT by word index `otz>>4` (~16 OTZ/bucket, no far clamp).
- **Fix:** `mesh_psx.c:335` change `otz >> 3` to `otz >> 4`. Existing `bucket >= OT_BACKGROUND_BUCKET` clamp keeps it in-bounds. Update the RE2 comment at :318-334.
- **Exercised:** every 3D actor/prop/shadow in every room (Leon, zombies, helicopter, floor shadow).

### [HIGH] near-plane-drop-8-vs-64 — near-plane drop at 8, source at 64
- **Our behavior:** `mesh_psx.c:263 #define OTZ_NEAR_PLANE_DROP 8` (drop poly only if otz<8), used :496/:639.
- **Source:** FUN_80022f0c.c:50 `if (uVar2>>6 != 0)`; FUN_800254a0.c:63; FUN_800256b0.c:71 — emit only when `otz>>6 != 0`, i.e. otz >= 0x40.
- **Source behavior:** Polys with otz in 8..63 are DROPPED by the original but KEPT by us.
- **Fix:** `mesh_psx.c:263` change `8` to `0x40`. Update stale RE2/value-8 comment :257-262.
- **Exercised:** near-camera geometry (close cuts, Leon walking toward camera).

### [MED] extra-gte-flag-cull — extra GTE FLAG cull the source never does
- **Our behavior:** After gte_rtpt, reads GTE FLAG and drops the primitive if bits 17/18 set (mesh_psx.c:469-473, :598-602, :629-633; mask :287).
- **Source:** FUN_80022f0c / FUN_800254a0 / FUN_800256b0 contain NO gte_stflg/FLAG read; pipeline is rtpt -> nclip -> avsz -> (otz>>6) gate only.
- **Source behavior:** Saturated verts are handled implicitly by the otz>>6 near-gate; no per-poly FLAG cull.
- **Fix:** Delete all three `gte_stflg(&gte_flag); if((unsigned long)gte_flag & GTE_FLAG_ERROR_MASK) continue;` blocks. Rely on the otz near-gate + nclip backface only. GTE_FLAG_ERROR_MASK def :287 becomes unused.
- **Exercised:** near/behind-camera vertices where the GTE limiter fires.

---

## Subsystem: Collision (re15_collision.c)

### [HIGH] coll-pair-inverted-mover — wrong party yields (enemy vs player)
- **Our behavior:** Pushes the ENEMY out of the player; player never displaced (re15_collision.c:390-445, comment :376-384).
- **Source:** FUN_8002b544 a0=self/enemy, a1=DAT_800ac784; FUN_80031c44.c:6 sets DAT_800ac784=&DAT_800aca54 (PLAYER); FUN_8002aec4.c:172-173 writes param_2 only.
- **Source behavior:** The PLAYER (param_2) is pushed out of every active entity; the iterated entity (enemy) never moves.
- **Fix:** `re15_collision.c:431` change `actor_pair_push(rdt, pl, e);` to `actor_pair_push(rdt, e, (re15_actor_t *)pl);` (player=mover, enemy=fixed). Prefer dropping const on `re15_collision_actors(pl)`.
- **Exercised:** any room with an enemy touching the player (room1140 feeding/pursuing zombies).

### [HIGH] coll-pair-no-enemy-enemy — invented O(n²) enemy-vs-enemy pass
- **Our behavior:** Adds an enemy-vs-enemy double loop pushing b out of a for every ordered pair (re15_collision.c:435-444).
- **Source:** `jal FUN_8002aec4` ONLY @8002b598 (inside FUN_8002b544); `jal FUN_8002b544` ONLY @80031cbc (FUN_80031c44 player update, DAT_800ac784=player). No enemy step calls the driver.
- **Source behavior:** There is NO enemy-vs-enemy collision; the pair routine runs once/frame and only moves the player.
- **Fix:** Delete the double loop (re15_collision.c:433-444).
- **Exercised:** room1140 with multiple feeding zombies (they shove each other in our build, never in the original).

### [LOW] coll-pair-extra-constrain — extra wall constrain inside the pair routine
- **Our behavior:** After displacing other, re-runs SCA wall constrain on it (re15_collision.c:410-416).
- **Source:** FUN_8002aec4.c:172-181 writes position + contact bookkeeping then returns; SCA constrain runs separately later (FUN_80031c44.c:21 FUN_8003b0a4).
- **Source behavior:** The pair routine writes position + contact flags only; no wall re-constrain.
- **Fix:** In actor_pair_push (:410-416) remove the re15_collision_constrain block; write `other->x += pushX; other->z += pushZ;` and return. (Secondary unported: contact flag +0x1c2, push-source ptr [0x6b]; rdt/ox/oz can be dropped.)
- **Exercised:** general; subtle when a push lands the entity in a wall on the same frame.

---

## Subsystem: SCD VM — opcodes 0x00-0x1F (scd_vm.c, re15_scd.h)

### [HIGH] scd-evtexec-slot-alloc — wrong thread-pool / event-slot model
- **Our behavior:** Thread pool = 24, event slots 10..23; cond 0xFF scans 10..23, low cond dropped (scd_vm.c:744-757).
- **Source:** FUN_8003ee3c + dispatcher FUN_8003f0a0 loop `uVar4 < 10`; room-exec sets DAT_800b3f7a=0 (normal branch).
- **Source behavior:** Pool is only 10 threads (0..9); slots 0/1 = main00/main01. cond<10 -> slot=cond; cond>=10 (incl 0xFF) -> slot 2 if free else scan 2..8 (a2=2, a0=9 limit).
- **Fix:** re15_scd.h: SCD_THREAD_COUNT 24->10, SCD_EVENT_SLOT_FIRST 10->2, SCD_EVENT_SLOT_LAST 23->8. op_evt_exec (:745-755): if cond<10 slot=cond; else slot=2, and if slot 2 active scan 2..8 for first free. **GATE:** re-validate the room1170 helicopter regression (re15_scd.h:38-40) against a savestate (six sub02 Evt_exec land in slots 2..7) rather than reverting to the 24-slot pool.
- **Exercised:** room1170 main00 Evt_exec(255,...); any room firing Evt_exec.

### [MED] scd-evtchain-03-noop — Evt_chain (0x03) is a no-op
- **Our behavior:** `op_evt_chain` just does `t->pc += 4` (scd_vm.c:2810-2816).
- **Source:** LAB_8003f270 (ghidra1_V2.txt:149126-149139) -> FUN_8003edec(thread, pc[3]): active=1, +4=0xff, +8=0xff, +0x140=base, PC=sub_scd[pc[3]].
- **Source behavior:** RE-INITIALIZES the current thread to restart as a different sub-script.
- **Fix:** op_evt_chain: target = `s_current_rdt->sub_scd[pc[3]]` (NULL-guard -> pc+=4); else reset active=1, kill_pending=0, call_depth=0, block_sp=0, do_depth=0, for_depth=0; `t->pc = target`.
- **Exercised:** rooms whose event scripts chain into another sub (not room1170).

### [MED] scd-while-ewhile-0f10-unimplemented — While/Ewhile fall to op_unknown
- **Our behavior:** 0x0F While / 0x10 Ewhile unregistered -> blind PC advance, no condition eval, no loop.
- **Source:** 0x0F LAB_8003f6f4 (inline predicate-chain And/Or evaluator, ghidra1_V2.txt:149432+); 0x10 LAB_8003f878 (block-stack back-jump, :149535+).
- **Source behavior:** While evaluates an And/Or-combined predicate chain and, on FALSE, jumps to block_end; Ewhile jumps back. A real conditional loop.
- **Fix:** Implement op_while/op_ewhile mirroring LAB_8003f6f4/8003f878 and register ([0x0F],[0x10] after :187). Add while_start[4]/while_end[4]/while_depth to scd_thread_t (re15_scd.h after for_depth). Needs `scd_eval_pred_chain()` walking Ck/Cmp/Member_cmp predicates OR/AND-combined per the s0 separator byte (generalize op_edwhile's single-Ck eval). FALSE jump targets the block_length-derived end; Ewhile jumps back to while_start.
- **Exercised:** rooms using While/Ewhile wait/poll loops (not room1170).

---

## Subsystem: SCD VM — opcodes 0x20-0x3F (scd_vm.c)

### [HIGH] scd-dir_set-target-and-axes — Dir_set drops rot_x/rot_z and targets player
- **Our behavior:** Reads pc[1]; only if reg==0 writes a single yaw (LE pc[4..5]) to PLAYER.rot_y; rot_x/rot_z never written (scd_vm.c:1328-1340).
- **Source:** LAB_80041080 (ghidra1_V2.txt:151242-151256, opcode 0x33): lhu pc[2]->+0x68 rot_x, pc[4]->+0x6a rot_y, pc[6]->+0x6c rot_z on WORK ENTITY (thread+0x154); pc[1] never read.
- **Source behavior:** Sets all three rotation halfwords on the work entity selected by the preceding Work_set; pc[1] ignored, not the player.
- **Fix:** op_dir_set: rx=LE pc[2], ry=LE pc[4], rz=LE pc[6]; resolve work entity like op_pos_set (t->work_slot/g_scd.work_slot, else work_prop_idx); write rot_x/rot_y/rot_z; pc[1] NOT read. pc+=8. Update header comment (drop the pc[1]=register line).
- **Exercised:** any script Dir_set on an NPC/work entity or setting rot_x/rot_z (tilted spawn poses); room1170 Work_set(1,0)->player lands on player but still drops rot_x/rot_z.

### [HIGH] scd-member_set2-value-source — reads wrong array
- **Our behavior:** value = (pc[2]<128)? per-thread `t->locals[pc[2]]` : 0 (scd_vm.c:2630-2647).
- **Source:** LAB_80041108 (ghidra1_V2.txt:151278-151303, opcode 0x35): a2 = DAT_800b0fd0[pc[2]] (lh, the global work-var array), full-byte index.
- **Source behavior:** Value comes from the global work-var array `g_scd.work_vars[pc[2]]` (s16), index 0..255.
- **Fix:** op_member_set2: replace `int32_t value = (var_idx < 128) ? (int32_t)t->locals[var_idx] : 0;` with `int32_t value = (int32_t)g_scd.work_vars[var_idx];`.
- **Exercised:** any script copying a work-var into an entity member (computed positions/headings).

---

## Subsystem: SCD VM — opcodes 0x40-0x5F + high handlers (scd_vm.c, aot_common.c)

### [MED] scd_0x47-aoton-passive-not-fire — Aot_on doesn't force-fire
- **Our behavior:** Aot_on(slot) just sets `slots[slot].active=1` (scd_vm.c:2595-2604); fires later when the player walks in.
- **Source:** LAB_800407bc (ghidra1_V2.txt:150628, opcode 0x47): looks up DAT_800ac9b0[pc[1]] and IMMEDIATELY invokes PTR_LAB_8007469c[type] (force-fires), then pc+=2.
- **Source behavior:** RE1.5 Aot_on forces the AOT's type effect (message/door/event) now.
- **Fix:** Factor aot_common.c re15_aot_scan's per-slot `switch(a->type)` block (:423-572) into non-static `void re15_aot_fire_slot(int slot)` (use g_actors[PLAYER] x/z), declare in re15_aot.h. op_aot_on: keep `active=1;` then `re15_aot_fire_slot(slot);`. A bare scd_event_fire(slot) is WRONG for DOOR/ITEM/MESSAGE.
- **Exercised:** scripts that Aot_on to force-trigger a scripted AOT effect (auto-fire a door/message).

### [MED] scd-keyck-inverted-polarity — Sce_key_ck (0x51) inverted in no-key state
- **Our behavior:** no-key case returns raw param (scd_vm.c:2880-2890).
- **Source:** LAB_80042920 (ghidra1_V2.txt:153516-153519): `and v1,v1,v0; bne...; _move v0,a1; xori v0,a1,0x1` — no-key state returns param^1.
- **Source behavior:** Key-SET path returns raw param; no-key state returns `param ^ 1`.
- **Fix:** op_sce_key_ck (:2888-2890): `return (param ^ 1) ? SCD_R_CONTINUE : SCD_R_IF_FALSE;`. Update comment :2884.
- **Exercised:** any If/while gated on Sce_key_ck while no trigger register bit is set.

---

## Subsystem: actor set_motion / locomotion walk-turn FSM (actor_locomotion.c, scd_vm.c, re15_actor.h)

### [HIGH] loco-state1-slew-0x60 — ALIGN slew snaps instead of turning over time
- **Our behavior:** state-1 ALIGN clamps the yaw step to ±PLC_YAW_SLEW_INIT (0x15e), which equals the convergence window, so heading snaps in ~1 tick (actor_locomotion.c:94,105,227-256). RUN too.
- **Source:** ghidra1_V2.txt:125402-125417 (WALK) + :125560-125577 (RUN): FUN_8001ab9c a2=0x15e = convergence TEST only (flips DAT_800aca5a 1->2); FUN_8001aac4 a2=0x60 = the actual heading SLEW.
- **Source behavior:** Heading slews at 0x60/tick in ALIGN for both walk and run; 0x15e is only the convergence half-window. Alignment takes (yaw_error/0x60) ticks.
- **Fix:** Add `#define PLC_YAW_SLEW_S1 0x60`. actor_locomotion.c:231-232 clamp state-1 step to ±PLC_YAW_SLEW_S1. Keep abs_delta<=0x15e convergence test unchanged. Fix the misattribution comment on PLC_YAW_SLEW_INIT.
- **Exercised:** any Plc_dest scripted walk/run with a large initial heading error (intro walkers, sub02/sub08 in room1170/1150).

### [MED] setmotion-sameid-no-init-hold — same-id Plc_motion skips the init-frame hold
- **Our behavior:** Re-issuing the SAME motion id zeroes anim_frame but never re-arms motion_init_delay (set_motion `motion!=m` guard skips); the clip restarts without the state-0 init render (re15_actor.h:182-202; scd_vm.c:1397,1419,1432).
- **Source:** Plc_motion LAB_80041b90 (ghidra1_V2.txt:152125-152146) unconditional `sb zero,0x6(v0)`; player FSM FUN_80050cb8/FUN_80050ddc init block when +0x6==0 renders frame 0 once without advancing.
- **Source behavior:** Every Plc_motion clears +0x6 to 0 regardless of id, so same-id re-issues DO get the init-frame hold.
- **Fix:** scd_vm.c:1419 after `g_actors[slot].anim_frame = 0;` add `g_actors[slot].motion_init_delay = 1;`.
- **Exercised:** sub02 forward/reverse gesture; any script re-firing the playing clip to restart it.

---

## Subsystem: Player damage / hit-react / grab / eaten FSM (player_damage_common.c)

### [HIGH] ple-bite-frontback-inverted — bite-react FRONT/BACK clips swapped
- **Our behavior:** ply_attacker_front returns attacker AHEAD; clip = front? 0x09 : 0x08 -> AHEAD picks 0x09, BEHIND picks 0x08 (player_damage_common.c:45-50,:90-91,:398-399).
- **Source:** FUN_8001a7a8.c:6-8 returns 1 when attacker is BEHIND; FUN_80012d60.c:48-49 aca59=flag+2; PTR_LAB_800741a8 idx2->0x08 (@80035e38), idx3->0x09 (@80035fbc). So clip 0x08=FRONT, 0x09=BACK. Verified numerically (facing 0: atk +x ahead -> flag 0 -> 0x08; atk -x behind -> flag 1 -> 0x09).
- **Source behavior:** clip 0x08 = FRONT bite-react, 0x09 = BACK. Our mapping is fully inverted (and the #define names are swapped vs physical meaning).
- **Fix:** Swap the #define VALUES (:90-91): PLY_CLIP_BITE_FRONT=0x08, PLY_CLIP_BITE_BACK=0x09. Then :399 `front? FRONT : BACK` resolves FRONT->0x08 / BEHIND->0x09. Fix inverted comment :87-89 and trailing comment :399. s_grab_front is independent (unaffected).
- **Exercised:** any non-grab enemy bite/scratch on Leon (state-2 hit-react).

### [MED] ple-grab-hold-hp-drain-missing — HP frozen during grab hold
- **Our behavior:** During PLY_GRAB phase 1 only s_grab_struggle decrements; p->hp not drained; eaten reached only by the 90-frame timer (player_damage_common.c:459-484; s_grab_drain_t/PLY_GRAB_DRAIN/PERIOD dead).
- **Source:** FUN_80103b94.c case3:55-58 + FUN_801035e8.c:17-20: each bite keyframe (func_0x8001f314!=0) drains HP `_DAT_800acaee += -1`; eaten on entity[0x9e]==0 OR acaee<0 (case3:73). acaee==player HP.
- **Source behavior:** HP drains 1 per bite-keyframe while held; a held player can die by HP depletion and the bar ticks down.
- **Fix:** PLY_GRAB phase 1: add `if (++s_grab_drain_t >= PLY_GRAB_DRAIN_PERIOD){ s_grab_drain_t=0; p->hp -= PLY_GRAB_DRAIN; }`, and change the eaten transition to `if (p->hp < 0 || s_timer >= PLY_GRAB_HOLD)`.
- **Exercised:** room1140 zombie grab/struggle.

---

## Subsystem: helicopter rotor audio + camera-distance pan (rotor_common.c)

### [HIGH] rotor-lr-pan-mirrored — L/R stereo pan mirrored
- **Our behavior:** off-center case panL=SE_PAN(pi) (attenuated far), panR=0x89 (full near); out_volL feeds LEFT. So the near side feeds OUR LEFT attenuated (rotor_common.c:133-134).
- **Source:** FUN_80045a64.c:69-70 DAT_800b2824=voll(LEFT)=bVar2*0x89+..., DAT_800b2826=volr(RIGHT)=bVar2*tbl[pi]+...; FUN_80045024.c:90 SsUtKeyOnV(...,voll,volr); LIBSND.H:261-262.
- **Source behavior:** b2-case LEFT(voll)=0x89 full, RIGHT(volr)=SE_PAN(pi) attenuated.
- **Fix:** rotor_common.c:133-134 swap: `panL = b2 ? 0x89 : (int)SE_PAN(pi);` / `panR = b2 ? (int)SE_PAN(pi) : 0x89;`.
- **Exercised:** room1170 rotor SUB-layer pan; footstep pan (game_step_common.c:120) and enemy-SE pan (zombie_common.c:248) reuse this — every off-center positional SE is mirror-panned.

---

## Subsystem: enemy AI / zombie FSM (zombie_common.c)

### [MED] zb-pursue-walk-reaim — pursue re-aims every frame instead of committing
- **Our behavior:** While walking (phase 1) re-quantizes coarse_yaw and re-aims every frame, reverting to turn-in-place the instant Leon drifts > one turn-step (zombie_common.c:401-424).
- **Source:** STAGE1_full/FUN_801033c8.c:16-40: state2 snaps heading once (`[0x6a]=(entity[0x90]&0xf0)<<4`), state3 only advances forward on clip-wrap; NO turn slew, NO re-quantize, never resets entity[6].
- **Source behavior:** Once walking, commit to the coarse heading captured at alignment and walk straight; re-aim only on a fresh pursue re-entry. A wide polygonal arc, not continuous re-track.
- **Fix:** Phase 0: zb_turn_toward(coarse_yaw, ZB_TURN_RATE); on aligned snap `z->rot_y=(int16_t)(coarse_yaw & 0xFFF)` once then phase=1. Phase 1 (WALK): forward step only — NO zb_turn_toward, NO re-quantize, NO auto-revert.
- **Exercised:** room1140 zombie pursuit of Leon.

### [MED] anim-frac-seed-hardcoded-7 — crossfade seed hardcoded to 7 (enemy)
- **Our behavior:** FRAC crossfade counter (entity+0x8f) hardcoded to 7 on every motion change (scd_vm.c:1432; weight skeleton_common.c:199).
- **Source:** FUN_8001f3bc:23,81 weight=0x1000-0x200*entity[0x8f]; entity+0x8f seeded with 0xf/7/4/3/0 across STAGE*/V2 (enemy spawn-init handlers seed 0xf, e.g. FUN_80101d08.c:14-16).
- **Source behavior:** Crossfade DURATION is per call-site (0xf longer/heavier, 0 = snap).
- **Fix:** Player path is byte-true (do NOT touch scd_vm.c:1432, player_common.c:267/285/301, grab=0). Enemy-only: add `re15_actor_set_motion_frac(a,m,frac)` in re15_actor.h (~:202). In zombie_common.c use the cited seed per clip-set: PROVEN site :359 IDLE-init -> 0xf (FUN_80101d08). **GATE:** map RISE/ATTACK/WALK/framework sites to their owning overlay handler's +0x8f seed (0xf wake-init / 7 movement / 0 snap) BEFORE substituting; do not blindly set all to 0xf. 7-seed sites stay on re15_actor_set_motion.
- **Exercised:** any enemy motion transition whose source set-site seeds 0xf/4/3/0.

### [LOW] zb-feed-chew-cadence — chew SE on a timer instead of the keyframe
- **Our behavior:** chew SE fires on `(ai->timer++ + i*23) % 69 == 0x28` (fabricated per-slot offset) (zombie_common.c:303).
- **Source:** STAGE1_full/FUN_80102bd8.c:26-30: chew `func_0x800453d0(3)` fires only when clip sub-frame entity[0x95]==0x28.
- **Source behavior:** Chew keyed to the feed clip's keyframe 0x28; the per-feeder stagger emerges from clip phase.
- **Fix:** zombie_common.c:303 change to `if (((uint32_t)z->anim_frame % 69u) == 0x28u) zb_play_se(cam,z,-2);`. Drop ai->timer if now unused for FEED.
- **Exercised:** room1140 feeding chew SFX timing.

---

## Subsystem: player per-frame tick (player_common.c, anim_select_common.c)

### [MED] plr-move-velocity-vs-rootmotion — flat velocity instead of root motion
- **Our behavior:** XZ translation is a fixed velocity vector (speed byte 75/200/70) (player_common.c:245-253).
- **Source:** FUN_80036718.c:27 -> FUN_800369f8(0,1):19-24 advances actor +0x34(X)/+0x3c(Z) by the animated root-bone delta (clip +0x54/+0x5c); the speed byte is a clip-playback param, not a position delta.
- **Source behavior:** Position advances by the root-motion displacement of the playing locomotion clip's root bone each frame.
- **Fix:** Replace the flat-velocity step (:245-253) with a root-motion port of FUN_800369f8(0,...): after bones are posed, advance p->x/p->z by the root-bone world translation delta (RotMatrix of p->rot_y × per-frame root delta vs ref +0x54/+0x5c). Speed bytes must NOT scale position. **BLOCKING DEP:** needs GTE/skeleton root-bone read-back (reuse the grab/enemy FUN_80022da0 pose-readback path); not a 2-line edit.
- **Exercised:** any free-roam walk/run/back.

### [MED] plr-back-clip-bank-wrong — back-walk uses the forward walk clip
- **Our behavior:** BACK sets motion=105 -> WEAPON bank (W01) clip 5 (forward gait) (player_common.c:66,:190; anim_select_common.c:268-272).
- **Source:** BACK LAB_80032d20 (ghidra1_V2.txt:128263-128310) plays COMMON bank (DAT_800acad8/acbc0) clip 0 forward; forward WALK uses WEAPON bank (DAT_800acbc4/acbc8) clip 5 (@:127697-127708).
- **Source behavior:** Back-walk plays COMMON-bank (PL00.edd) clip 0, distinct from the forward walk.
- **Fix:** player_common.c:66 `#define RE15_MOTION_BACK 110` (free sentinel). anim_select_common.c before the `} else if (m == 200)` fallback (:305) add a branch: `} else if (banks->pl00_ok && m == 110) { out->skel=banks->pl00_skel; out->anim=banks->pl00_anim; out->clip_override=0; }`. Reverse-flag clear at player_common.c:259 stays.
- **Exercised:** holding DOWN to step backward (any room).

---

## Subsystem: AOT scan / forward-reach / stair traversal (aot_common.c, stair_common.c)

### [MED] generic-aot-reach-fixed-900-box — fixed ±900 reach box vs AOT half-extents
- **Our behavior:** GENERIC/MESSAGE/EXAMINE reach tests a fixed ±900 box (aot_common.c:378-388).
- **Source:** FUN_80042bac.c:99,113 -> FUN_80042b64.c tests abs(probe-center) <= param_2[2]/param_2[3] = the AOT's own half-extents.
- **Source behavior:** Reach tested against the AOT's stored half-extents, not 900.
- **Fix:** aot_common.c:387 change to `gen_reach = (abs_i32(fx - a->x) <= a->half_w) && (abs_i32(fz - a->z) <= a->half_h);` (already populated).
- **Exercised:** any GENERIC/MESSAGE examine AOT whose half-extent != 900 (room1130 switch, room1140 painting).

### [MED] action-fired-single-latch — one-action-per-press latch the source lacks
- **Our behavior:** A single action_fired latch suppresses every action AOT after the first this frame (aot_common.c:235,419-420).
- **Source:** FUN_80042bac.c:128-139 (action mode param_3==0) `goto LAB_80043018` and CONTINUES; fires EVERY matching AOT. Only QUERY mode returns on first match.
- **Source behavior:** All matching action AOTs in the flag class fire.
- **Fix:** Delete `int action_fired=0;` (:235 + comment :227-234); drop `&& !action_fired` from the door (:407), counter==9 (:414), generic (:417) gates; delete `if (is_action) action_fired = 1;` (:420). Keep the DOOR-fire `return;` (:530). Re-validate room1130 switch + room1150 examine overlap.
- **Exercised:** rooms with two overlapping action AOTs in the same scan.

### [MED] stair-finalize-forward-eject — invented forward-march reposition
- **Our behavior:** finalize runs a 48-step forward-march eject (re15_collision_constrain, +100u nudges) repositioning the player in XZ (stair_common.c:121-169).
- **Source:** LAB_80038e50 @0x80038e50-0x80038edc (ghidra1_V2.txt:136857-136893): only sets standby clip/control flags + DAT_800acad6=-DAT_800acc0e/0x708; does NOT touch player X/Z.
- **Source behavior:** Control is handed back with the player stationary in XZ; the next frame's FUN_8003b0a4 un-sticks him.
- **Fix:** In `if (s_finalize)` delete the forward-eject march and +100u nudge (:127-169 inner block). Keep: motion=IDLE, anim_frame=0, anim_flags=0, y=s_target_y, re15_collision_set_band(band_from_y(y)), s_finalize=0, s_active=0, return.
- **Exercised:** room1170 outdoor staircase descend/ascend.

### [LOW] stair-entry-probe-exact-yaw — probe uses raw yaw vs coarse 64-dir
- **Our behavior:** 800u entry probe uses the player's EXACT rot_y (stair_common.c:283-285).
- **Source:** FUN_8002d474.c:26 -> FUN_8004f008((short)(DAT_800acabe + 0x200 & 0xfc00), &800,...).
- **Source behavior:** Probe vector rotated by facing coarsened to 64 dirs: (facing+0x200)&0xfc00.
- **Fix:** stair_common.c:283 compute `int32_t probe_ang = (int16_t)(((int)p->rot_y + 0x200) & 0xfc00);` then sin/cos of probe_ang.
- **Exercised:** stair start when facing a diagonal near a 64-dir boundary.

---

## Subsystem: Camera RVD zone cut-scan (rdt_common.c, aot_common.c)

### [MED] rvd-reverse-scan-order — overlapping same-cut zones tested in reverse
- **Our behavior:** RVD zones installed top-down (DESCENDING slots) but the scan iterates ascending and returns on first inside -> effectively reverse RDT order; last zone wins (rdt_common.c:398 vs aot_common.c:268,333).
- **Source:** FUN_80014230.c:11-19 walks the active cut's zone sublist FORWARD (iVar3+=0x14), switches to the first zone the player is inside.
- **Source behavior:** Forward table order; first-inside wins.
- **Fix:** rdt_common.c:375 reverse the install loop: `for (int i = rdt->zone_count - 1; i >= 0; i--)` so the RDT-earliest non-anchor zone gets the lowest slot (scanned first). Anchor-skip :390 unchanged. Verify room1170 unchanged.
- **Exercised:** rooms with overlapping same-cut RVD trapezoids (z5/z8 overlap).

---

## Subsystem: RDT / room transition / INIT script handlers (room_common.c, scd_room_setup.c)

### [MED] room-crossroom-scenario-hardcoded-0 — cross-room entry scenario forced to 0
- **Our behavior:** Cross-room door apply always passes entry_scenario=0 -> sub00 always stamps work_vars[10]=0 (room_common.c:102; scd_room_setup.c:158).
- **Source:** scd_room_setup.c:151-158 work_vars[10]=entry_scenario drives sub00 switch; FUN_8001d600.c:42-45 DAT_800afbb5/DAT_800b0fe4 from door bytes +0x0a/+0x09.
- **Source behavior:** Entry scenario derived from the entering door's record fields; the same-room path already honors it (aot_common.c:501-503).
- **Fix:** room_common.c:102 change to `scd_room_reenter(c->rdt, g_room_change.x, g_room_change.z, (uint8_t)g_room_change.target_cut);`.
- **Exercised:** latent for STAGE1; a cross-room door into a room whose sub00 switches on a non-zero entry scenario would run the wrong init case.

### [MED] room-sub01-once-slot2-not-perframe-slot1 — sub01 runs once instead of per-frame
- **Our behavior:** sub01 started in slot 2 and ticked once at reentry; sub00 stays resident in slot 1 forever (scd_room_setup.c:166-169).
- **Source:** FUN_8003ef6c.c:9-11 (load starts only thread0=main00, thread1=sub00); FUN_8003f038.c:5-8 (EVERY gameplay frame re-points slot1 to sub01 and runs it).
- **Source behavior:** sub00 runs in slot 1 once at load; from the next frame slot 1 is REPLACED by sub01, which runs every frame.
- **Fix:** Remove the once-only slot-2 block (scd_room_setup.c:166-169). In scd_vm.c at top of scd_vm_tick() (after tick_count++, gated off menus/cutscene player_mode!=2), re-point slot 1 to sub01 each gameplay frame: memset(t1,0), active=1, pc=s_current_rdt->sub_scd[1], work_slot=-1, work_prop_idx=-1. Adjust the gameplay-active guard to the engine's real predicate.
- **Exercised:** rooms whose sub01 does per-frame work (continuous polling / re-arming).

---

## Subsystem: message typewriter / YES-NO cursor (msg_common.c)

### [MED] msg-yesno-pad-mask — YES/NO toggled by wrong buttons
- **Our behavior:** Toggle on PAD_LEFT(0x0080)|PAD_RIGHT(0x0020) (msg_common.c:364,372,448).
- **Source:** FUN_80028134.c:144-154 case4: `if((DAT_800ac76c & 0x3000)!=0)` toggle; logical 0x3000 at default config-0 remap DAT_80073dbc -> phys SQUARE(0x8000)|CIRCLE(0x2000).
- **Source behavior:** Toggle on the press edge of logical 0x3000 (config-0 -> phys SQUARE|CIRCLE), NOT LEFT|RIGHT and NOT TRIANGLE|CIRCLE.
- **Fix:** msg_common.c:364 `TOGGLE_MASK = RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_CIRCLE;` and :372 `lr_edge = (g_scd_pad_edge & TOGGLE_MASK) != 0;`. Confirm gameplay config index (DAT_800b0fcc default 0) before commit. :448 toggle already byte-true.
- **Exercised:** any YES/NO prompt (room1170 painting choice).

### [LOW] msg-pagetimed-resume-timer — timed-page resume timer wrong (1 vs scroll)
- **Our behavior:** case 2 (PAGE_TIMED) resumes typing with message_timer=1 (msg_common.c:441-445).
- **Source:** FUN_80028134.c:134-142 case3: resume timer = DAT_800b8524<<bVar2 = message_scroll.
- **Source behavior:** Next page's first glyph waits `scroll` frames, not 1.
- **Fix:** case 2: change `g_scd.message_timer = 1;` to `g_scd.message_timer = g_scd.message_scroll;`. Do NOT touch case 1 (PAGE_WAIT, =1 is byte-true).
- **Exercised:** multi-page messages with a timed (0x02 N, N!=0) page break.

---

## Subsystem: VAB / note2pitch / SE (vab_common.c, audio_psx.c)

### [MED] n2p-se-shift-half — SE/footstep pitch shift added once vs twice
- **Our behavior:** fine index = pitch_shift/8 (shift added once); SE/footstep pitch too low by ~half a step (vab_common.c:213-215 via audio_psx.c:243,292).
- **Source:** note2pitch2.c:10 `uVar3=((param_2&0xffff)+*(byte*)(iVar1+5))>>3`; caller FUN_80045024.c:90 passes fine=tone.shift, note2pitch2 ADDS tone.shift again -> (2*shift)>>3.
- **Source behavior:** SE/footstep fine-index = (2*tone.shift)>>3 = shift>>2.
- **Fix:** At the two SE/footstep sites only, pass `tn->pitch_shift * 2` to re15_vab_note2pitch (audio_psx.c:243, :292). Do NOT change the BGM site audio_psx.c:551. room1170 concrete footstep shift=0 -> unaffected.
- **Exercised:** any room/SE whose snd0 tone has pitch_shift!=0.

---

## Subsystem: sprite.pri FG-occluder render (render.c)

### [LOW] pri-depth0-mask-otz-clamp — depth-0 mask forced to bucket 1
- **Our behavior:** A depth-0 mask is forced to OT bucket 1 (render.c:454 `if (z < 1) z = 1;`).
- **Source:** FUN_80039590.c:19 `AddPrim(... + depth*4, p)` with no floor; depth 0 links at OT[0].
- **Source behavior:** A depth-0 mask links at OT[0] (frontmost).
- **Fix:** render.c replace `if (z < 1) z = 1;` with `if (z < 0) z = 0;` for the PRI mask bucket. Keep the upper clamp.
- **Exercised:** rooms with a depth-0 frontmost occluder mask (not room1170, masks depth>=0x70).

---

## Subsystem: pad mapping / input remap (game_step_common.c, input.c)

### [MED] pad-action-button-triangle — action button is Square, should be Triangle
- **Our behavior:** Action button hardcoded to physical SQUARE (game_step_common.c:28,33).
- **Source:** FUN_8002bd44.c:57 `(DAT_800ac768 & 1)`; FUN_80030444.c:13-18 remap; table @0x80073dbc default DAT_800b0fcc=0; logical bit0 <- raw 0x1000 = TRIANGLE.
- **Source behavior:** Action = logical bit 0 of the remapped mask = raw TRIANGLE in every config.
- **Fix:** game_step_common.c:28 change `c->pad_pressed & RE15_PAD_BIT_SQUARE` to `& RE15_PAD_BIT_TRIANGLE`; :33 same for `c->pad_current`. (Cleaner: implement FUN_80030444 remap, drive from logical bit 0.) Platform-shell main.c keys may stay.
- **Exercised:** any room: pressing the action/door button.

### [MED] pad-no-logical-remap-layer — no logical remap layer (structural)
- **Our behavior:** Gameplay consumes RAW physical bits directly; no per-frame remap table (input.c:22-37; game_step_common.c:38-48).
- **Source:** FUN_80030444.c:11-27 builds a remapped LOGICAL mask DAT_800ac768 from a 16-entry config table (default @0x80073dbc), edge on the logical mask; gameplay never sees raw bits.
- **Source behavior:** All gameplay/dialog/menu read the logical mask; raw mapping coincides only for face bits 0-3, diverges for d-pad/L/R/confirm.
- **Fix:** STRUCTURAL. **GATE:** verify per-bit vs a dialog-frame savestate of DAT_800ac768 first (current raw mapping is USER-validated). Add a per-frame remap in input.c: 3 config tables from 0x80073dbc/ddc/dfc (default 0 = {0x1000,0x2000,0x4000,0x8000,0x1000,0x4000,0x0080,0x0080,0x0008,0x0040,0x0008,0x0004,0x8000,0x2000,0x0080,0x0040}); build pad_logical + edges. Route DIALOG/MENU/door reads (g_aot_action_held :33, g_scd_action_held :48, msg/AOT/YES-NO) through pad_logical; keep player-locomotion on RAW. **This SUPERSEDES pad-action-button-triangle and msg-yesno-pad-mask if fully implemented.**
- **Exercised:** any code reading a non-face logical button (dialog confirm 0x4000, YES/NO 0x3000, aim L/R).
