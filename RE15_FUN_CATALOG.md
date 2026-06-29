---
name: reference-re15-fun-catalog
description: "LIVING SINGLE-SOURCE-OF-TRUTH function catalog for RE1.5 (content) on RE2 (architecture). Maps FUN_/DAT_/LAB_ addresses → subsystem → purpose → evidence → confidence. 79 rows code-verified against in-repo disasm 2026-05-29 (verify-fun-catalog workflow, 8 subsystems, 0 not-found). Look here FIRST before re-RE-ing any subsystem. Grows every RE round per [[feedback_build_persistent_fun_catalog_2026_05_29]]."
metadata:
  node_type: reference
  type: reference
  originSessionId: 0a2f4870-554b-42c1-80aa-3553c9ee74f1
---

# RE1.5/RE2 Function Catalog — living reference

**Look here FIRST.** Before RE-ing any subsystem, check this map. Append every newly code-verified
address. All rows below were verified against the in-repo disasm 2026-05-29 unless marked ⚠️. Evidence =
`ghidra1_V2.txt:line`, Overlay-Decompilate `RE_15_Quellcode_Overlays/STAGE*/FUN_*.c` (**primär** für Spiel-/Raum-/AI-Logik), EXE-Decompilate `RE_15_Quellcode_V2/*.c` / `RE2_Quellcode_V2/*.c` (**Fallback**, 0x8001xxxx-Engine). Fehlt etwas → selbst via Ghidra (headless) + `stage_saves/` ermitteln. Engine=RE2, Content=RE1.5.

## ⚠️ HIGH-VALUE CORRECTIONS (seed labels that were WRONG — read first)
- **FUN_800245d8** ≠ walker atan2. It's **position-advance by yaw** (rotate (speed,0,0), add to world pos). The real walker-target atan2 is **FUN_8001a6d4**.
- **FUN_80015850** (camera soft-track) — seed said "dead code, not for static cuts". **WRONG for PSX**: it runs EVERY rendered frame for all camera states incl. cinematics (soft-tracks active actor, XZ/60 & Y/20, +400 Y). ⚠️ Tension with [[bugfix_BI_round_camera_softtrack_REVERTED_2026_05_29]] (user rejected our impl — likely our params were wrong, not the concept). Re-examine, don't blindly re-enable.
- **Evt_exec (0x8003f2a0)** — PSX does **NOT** defer spawned threads to next tick; same-tick if higher index. The BN-round next-tick defer is **engine-added, NOT canonical → revert**.
- **Plc_ret (0x80041f88)** — PSX just sets actor+0x4=1, pc+1. **No 12-frame cleanup window** (BK-round engine-added).
- **Fade**: real FSM = **FUN_80021a0c** (±0x10/frame, range 0x00–0xF0 = ~15 steps ≈0.5s @30fps), NOT FUN_8002bda8 (that addr is mid-body of the NPC AI loop FUN_8002bd44). Our BN 6-frame measured ramp is too fast → replace with canonical 15-step.
- **Letterbox**: real PolyF4 quads init = **FUN_80020f8c**, driven by FUN_80021a0c. NOT 0x8003014c (that's the Config/Credits 48px dialog box). Our BJ 17px was ablauf-measured — check FUN_80020f8c for the canonical quad height.
- **SCD opcode 0x18 = GOSUB ✅ RESOLVED 2026-05-29 (workflow w71w6amhp): engine is CORRECT.** 0x18 = GOSUB via the sub_scd table; `DAT_800b3f70` is the **sub_scd table base**, NOT a message table. The earlier "message handler / PC-redirect to msg table" reading was a misread of an `Evt_exec(0x04)` argument byte (`04 FF 18 03` — the `18` is an arg, not an opcode). Verified against ROOM1170 raw bytes: sub02.scd `18 0A`→Sub10, `18 09`→Sub09 (these fire correctly). Engine `op_gosub`→`sub_scd[]` is canonical. (Message_on=0x2B unchanged — separate, also correct.)
- **0x80016b54** = GT3 triangle batch renderer (per-entity), not "main render loop". No far-clip CONFIRMED (validates #4).
- **0x800392E4 / 0x8006A1E8** = mid-function offsets; real entries **FUN_800392d4** (sprite builder) / **0x80068c88** (LoadImage; 0x8006A1E8 is internal `_dws`).

## Player / animation
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| DAT_800aca58 | Player-mode global (1 byte); index into PTR_LAB_80073f90 dispatch table (0–7) | ghidra1:474601,127085 | high |
| FUN_80031c44 | Player-mode dispatcher: DAT_800ac784=player, reads DAT_800aca58 (player STATE, port g_actors[0].state), `<<2` → calls **PTR_LAB_80073f90[state]** once/frame @0x8001ce0c. state0/1=Idle/Gameplay, state2=Hit (FUN_80035af0), state3=Death (FUN_800366bc) [#22] | ghidra1:127059-127096 | high |
| FUN_80035af0 (state2 Hit) | Hit-reaction sub-FSM: sub-states DAT_800aca59/DAT_800aca5a (port sub_state_1/2) via PTR_LAB_800741a8; plays hurt **clip 0xa** (DAT_800acae8=0xa) via FUN_8001f314, counts down, returns to Idle. State SET by #13 damage; ⚠️ DISPATCH (idle-gate + this handler) DEFERRED — needs hurt clips + the deferred enemy-AI damage trigger [#22] | ghidra1:131869-131960 | high |
| LAB_80041b90 (0x3F Plc_motion) | motion_id @pc+1 → actor+0x94 (DIRECT clip idx), player_mode=4, flags@pc+2→+0x1c4, pc+4 | ghidra1:152125-152146 | high |
| FUN_8001f314 | Keyframe anim advance: param_3 fwd/rev; bit0x8000→interp(FUN_8001f8b4) else FUN_8001f3bc; anim_flags 0x80=reverse | ghidra1:98294-98339 | high |
| FUN_8001f664 | 12-bit packed bone-angle unpacker (ZYX, 2/uint32) EDD→bones. From FUN_8001f3bc | ghidra1:98547 | high |
| LAB_80041f88 (0x42 Plc_ret) | actor+0x4=1 (gameplay), clears +5..7, pc+1. NO 12-frame window | ghidra1:152415-152427 | high |
| LAB_80030af0 | Player yaw-slew handler (idx4 of PTR_LAB_80073e30). FSM DAT_800aca5a: init→slew 0x60→0x30+pos-adv. Arrival Euclid <0x64 | ghidra1:125367-125465 | high |
| FUN_8001a6d4 | **Walker atan2 target yaw (Q12)** from (sx,sz,dx,dz), catan+quadrant, 0x000–0xFFF (0x400=+Z) | ghidra1:87894 | high |
| LAB_80030AF0 (walker 3-state FSM) | DAT_800aca5a: **S0** sets speed (0x4b walk/0xc8 run)→DAT_800acae0, fsm→1, falls through; **S1** FUN_8001ab9c convergence gate (0 when within ~31°/0x15e of dest), slew 0x60, NO advance; **S2** slew 0x30 walk/0x48 run, FUN_800245d8 advance, arrival Euclid <0x64 walk/0x12c run | ghidra1:125310-125670 | high |
| FUN_8001ab9c | yaw convergence gate: returns 0 when within threshold 0x15e of dest heading (→ S1→S2) | ghidra1:88539 | high |
| FUN_80024e40 | Neck damped-spring (4× from FUN_80024c30, divisors 0x190/15e/12c/fa, gate DAT_800aca5c&0x4): pitch→actor+0x94, yaw→+0x96, signed-12bit clamp (bit0x800→−0x1000) | ghidra1:108385-108713 | high |
| FUN_8001aac4 | Yaw-slew by (dx,dz,rate): calls FUN_8001a6d4, clamps step ±rate. From LAB_80030af0 | ghidra1:88465 | high |
| FUN_8001a8f8 | Yaw-slew toward waypoint struct (Plc_dest variant); param_1[0]=x,[4]=z | ghidra1:88250 | high |
| FUN_800245d8 | **Position-advance by yaw** (NOT walker): RotMatrixY(rot_y+arg), GTE rtv0 (speed,0,0), add X/Z to +0x34/+0x3c | ghidra1:125466 | high |
| LAB_80040f14 (0x2F Speed_set) | axis@pc+1(lbu), value@pc+2(lhu LE), store thread+0x158+idx*2 (per-thread vel) | ghidra1:151147-151158 | high |
| 0x80040f40 (0x30 Add_speed) | actor@thread+0x154, vel[axis]@+0x158, add to actor pos/rot | ghidra1:151160-151175 | high |
| FUN_8004116c | **Member_SET core** (id a1, value a2, actor a0). RE1.5 DIRECT id→offset (NOT RE2 ids!): 0→+0x34/sw(x) 1→+0x38(y) 2→+0x3c(z) 3→+0x68/sh(rotx) 4→+0x6a(roty) 5→+0x6c(rotz) 6→+0x00/sw(flags) 7→+0x0c 8→+0x04/sb(state) 9→+0x05 10→+0x06 11→+0x07 12→**+0x09**(grid-id, NOT Y!) 13→+0x08 14→+0x0a 15→+0x0b 16→+0x1c4/sh(anim) 17→+0x98 18→+0x82(floor) 19→+0x1ba/sh(hp). **Ported** [#11]: actor_common.c | ghidra1:151322-151380 | high |
| FUN_80041358 | **Member_GET core** (mirror of SET): same id→offset, signedness lbu(8-15,18)/lh(3-5,19)/lhu(16,17)/lw(0-2,6,7). idx 0..0x13, id≥0x14→default. Used by Member_cmp 0x3E | ghidra1:151486-151575 | high |
| LAB_80041290 (0x3E Member_cmp) | id=lo-byte@pc+2, op=hi-byte, **value=lh@pc+4 (LE signed)**, pc+=6. FUN_80041358(id)→cur. switchD_800412f0 ops {0:== 1:> 2:>= 3:< 4:<= 5:!= 6:&(test)}, op≥7→false. **Ported** [#12]: scd_vm.c op_member_cmp | ghidra1:151416-151468 | high |

## Combat / player damage (verified 2026-06-28, ghidra1_V2.txt:77607-77814 + 223455-223490)
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| FUN_80012d60 | **Unified hit/damage resolver.** Tests one attack hitbox (param_2) via FUN_8002b5d0 against every active enemy (DAT_800acc2c, stride 0x1f4) AND the player block (DAT_800aca54), applies DAT_8006f418[param_3&0xff] damage. **Player branch** @80012e18-f04: hit-once gate +0x93 bit0 (@80012e30) → HP(s16@+0x9a)-=dmg (@80012e44-64) → type<2 bleed/poison +0x98 bit0x2 via 2×FUN_8001af20&1 (@80012ea4) → state+0x4=2(hurt)/+0x5=front-back+2/+0x6=0/+0x93\|=1 → signed HP<0 → state=3(death). **Ported** (player branch): re15_damage.c | ghidra1:77607-77814 | high |
| DAT_8006f418 | Damage table (11×s16 LE) by attack_type: {10,20,1000,1000,1000,50,100,200,300,1000,**0**}. read @80012e54/80012ff4 | ghidra1:223455-223478 | high |
| DAT_8006f430 | Hit-reaction code table (11 bytes) by attack_type: {03,03,09,0A,0B,0E,0F,10,11,12,14}. Written to enemy +0x5 @80012fe8 | ghidra1:223480-223490 | high |
| FUN_80017fa4 | Enemy attack-action handler (dispatch table @0x80071da4). Builds hitbox @sp+0x10 from attacker(DAT_800b52c4) pos +0x28/+0x2a/+0x2c via FUN_8001c6e8, calls FUN_80012d60(0x1f4, hitbox, **0**) = attack_type 0 = 10 dmg (clear a2 @80018004). On hit → SE (FUN_800199d4/FUN_80045024) + reset attacker. 2nd call-site @800185b8 = other action (a2 unverified). **NOT ported** (no enemy AI) | ghidra1:84648-84717 | high |
| FUN_8001a7a8 | Front/back hit selector: 1 if hit from player front hemisphere (±90°). ang=FUN_8001a6d4(hbX,hbZ,p+0x34,p+0x3c); ((ang-facing+0x400)&0xfff)<0x800. → hurt-anim sub-state (2=back,3=front) | ghidra1:87971-87992 | high |
| FUN_8002b5d0 | Actor-vs-hitbox collision test (called per-target by FUN_80012d60). **NOT ported** — the deferred trigger for in-game player damage | ghidra1:118005 | med |
| DAT_800acaee | **Player HP** (s16) = player block DAT_800aca54 + 0x9a. init 0x64=100 @0x80031718. Enemy HP at the same +0x9a offset | ghidra1:474973,77706 | high |
| DAT_800acae7 | Player +0x93 hit-react byte: bit0x1 = "already hit this attack" guard (1 dmg/attack window) | ghidra1:474922,77705 | high |
| DAT_800acaec | Player +0x98 status word: bit0x2 = bleed/poison (set on type<2 hit); bit0x4 gates the #41 poison-drain | ghidra1:77691-77695 | high |

## Stairs / floor-band traversal (verified 2026-06-10, ghidra1_V2.txt:136723-136901)
Player-mode sub-states for the auto stair walk (outdoor ROOM1170 type-12/13 Aot zones).
DAT_800aca5b = stair phase (0=idle, 1=stepping, 2=finalize). Per-band the body steps
forward over ~32 cursor ticks + a +0x708 Y drop; |Δband| = number of cycles.
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| LAB_80038c60 | **Stair DESCEND handler** (player sub-state 0xc). Phase0 init: clip DAT_800acae8=0x15 (PL00 c21), fwd speed DAT_800acae0=0xa, band DAT_800acad6=7, cursor DAT_800acae9=0, savedY DAT_800acaf0=Y. Phase1/tick: FUN_8001f314 cursor+1 → FK FUN_800390e0(0,sel∈[0xb,0x1a)) → Y+=0x96 bob @cursor 0xb&0x1b → FUN_800245d8 fwd while cursor<0x1f → @cursor 0x20: Y=savedY+0x708, count(DAT_800acaf2)--, cursor→2; count==0 → set phase 2 | ghidra1:136723-136851 | high |
| LAB_80038850 | **Stair ASCEND handler** (sub-state 0xb). Same as descend but every window +1 (footsel [0xc,0x1b), bob @0xc&0x1c, band step @0x21, loop→3) and Y step −0x708 | ghidra1 (ascend body) | high |
| LAB_80038e50 | **Stair FINALIZE/settle** (DAT_800aca5b==2). ⚠️**KEY: does NOT reposition the player** — only sets clip DAT_800acae8=2 (standby), cursor=0, FUN_8001f314 one tick, recomputes band DAT_800acad6 = −DAT_800acc0e/0x708 (committed Y), clears control-lock DAT_800aca59/5a/5b=0. NO collision eject, NO X/Z push. It avoids the mid-stair wedge purely because the full fwd+FK travel lands the player a few hundred u PAST the destination wall-cell midpoint, so the next frame's stationary nearest-edge push (FUN_8003b0a4) goes FORWARD onto the platform | ghidra1:136857-136893 | high |
| FUN_800390e0 | **Stair FK foot-lock**: body (player +0x34/38/3c) dragged by the planted support foot's per-tick WORLD delta (RotMatrix +0x68 chained to foot bone, prev cached at +0x54). Major XZ component of the stair walk; foot = bone 7 in window else bone 4 | ghidra1 (137057-137125) | high |
| FUN_8003b0a4 (SCA push-out resolver) | per band-matching cell: dispatch `handler[cell.type](cell, player+0x34, player+0x40)` via runtime jump-table **DAT_800b2858**[type] (FUN_8003b0a4 @8003b480-a8). Quadrant helper FUN_8003b068 (port `quadrant_of`). Ported: type1 push_rect, type3 push_circle, type8/9 push_caps. **type 2/4/5/6/7 = slope/diagonal, DEFERRED [#29]** | ghidra1:142908-142919 | high |
| Slope push-out handlers [#29] | **NOT ported** (~230 fixed-point lines EACH, line/edge intersection + quadrant sub-cases): t2 LAB_8003d00c (ROOM1010), t4 LAB_8003beb0 (ROOM1050/10D0), t5 LAB_8003c734 (ROOM1050), t6 LAB_8003cb9c (ROOM10D0), t7 LAB_8003c2cc (ROOM10D0). Additive (ROOM1170 has none); player currently walks through slopes. Deserves a dedicated worktree run + slope-room savestate verification | ghidra1:145450+ | high |
| FUN_8002c444 | Stair Y-lower helper (sinks player Y toward the next band as he walks) | ghidra1 (xref) | med |
| DAT_800acc0e / DAT_800acaf0 / DAT_800acaf2 | committed landing Y / saved band-start Y / remaining 0x708 band-cycle count (count-1 at entry) | ghidra1:136824-136845 | high |
| **PORT NOTE (our fix, 2026-06-10)** | Our forward-10+FK port UNDERSHOOTS ~400u → lands AT the wall-cell midpoint where nearest-edge eject is a coin-flip → "Leon hängt auf halber Treppe in der Wand" (~rare in the original too). Fix in stair_common.c finalize: eject DETERMINISTICALLY FORWARD (accept nearest-edge only if its push projects forward along rot_y; else nudge +100u fwd & retry → wall-cell forward edge = foot of stairs, never backward). Autopilot-verified 280/280 runs 0 hang. See [[stair_hang_forward_eject_fix_2026_06_10]] | autopilot debug.log | high |

## Camera
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| FUN_80015850 | Per-frame camera orbit+soft-track animator: dolly residuals, XZ via rcos/rsin(yaw), soft-track active actor XZ/60 & Y/20, +400 Y, rebuild via 0x80053ca4. **Runs every frame incl. cinematics** | ghidra1:81665-81811 | high |
| 0x8001500c | Camera frame dispatcher: once/frame; active only if DAT_800aca58∈{3,6,7}; jump table 0x80071d10 (FUN_80015850 @ idx 2/4/6) | ghidra1:81104-81137 | high |
| 0x80053ca4 | setupCameraLookAtMatrix: GTE view matrix from cam block (pos+4/8/c, target+10/14/18) → DAT_800b5288 | ghidra1:180931 | high |
| 0x80014324 | kamera_daten_suchen: room cam-def list (DAT_800ac778+0x28), 0x14-byte entries, match byte+0x2==cut-id | ghidra1:80164-80188 | high |
| 0x800142f4 | raum_kamera_wechseln (cut-select/CAM_SWITCH): DAT_800afbb5=cam-id, FUN_80014324 → DAT_800ac794. Called by Cut_chg + RVD-zone hit | ghidra1:80144-80160 | high |

## SCD VM
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| 0x800744a8 | SCD opcode dispatch table (256×4-byte handler ptrs) | ghidra1:243037 | high |
| 0x8003f0a0 | scd_execute_scripts: main tick loop, slots 0..9, dispatch until return==2, then FUN_8003ebf4/ec28 | scd_execute_scripts.c | high |
| 0x800b2b4c | SCD thread pool base: 10 slots (14 if DAT_800b3f7a) × 0x170; slot N = base+N×0x170 | sys_init_script_slot.c | high |
| thread layout | +0x01 active, +0x02 depth, +0x04/+0x08 sleep, +0x1c PC, +0x140 call-SP, +0x154 work-actor, +0x158.. per-thread vel (6×s16 LE) | ghidra1:151148-151175 | high |
| 0x8003ee3c | allocate_script_slot: find free (0..9/13), call init FUN_8003edec. Called by Evt_exec | sys_allocate_script_slot.c | high |
| 0x8003f2a0 (0x04 Evt_exec) | slot-hint@pc[1], sub_id@pc[3], pc+4, FUN_8003ee3c alloc+start. **PSX runs same-tick (no next-tick defer)** | ghidra1:149139-149154 | high |
| 0x8003f540 (0x0D For) / 0x8003f674 (0x0E Next) | For(6B): block_len@pc+2, count@pc+4(LE), push body+count. Next(2B): dec count, jump back if >0 | ghidra1:149318-149431 | high |
| 0x800402a0 (0x29 Cut_chg) | cam_id@pc+1, set bit0x100 DAT_800aca3c, write DAT_800b0fe4, call FUN_800142f4, pc+2 | ghidra1:150271-150306 | high |
| 0x800403ac (0x3C Cut_auto) | mode@pc+1. mode==1 CLEARS bit0x100 (auto cam on); else SETS (enter cutscene). Polarity inverted vs naming | ghidra1:150340-150368 | high |
| 0x80041be4 (0x40 Plc_dest) | walk-mode@pc[2], dx@pc[4..5], dz@pc[6..7], heading@pc[3] → actor+0x1bc/be/c3 | ghidra1:152147-152182 | high |
| 0x800420a0 (0x44 Sce_em_set) | 20B: type@pc+1(bits0-6 type,bit7 flag), index@pc+7, pos/angle; spawn @DAT_800ac77c+type*stride | ghidra1:152496-152575 | high |
| 0x8003fbe8 (PSX 0x18 = GOSUB) | **GOSUB** (sub_id@pc[1], save ret-PC thread+0x144, redirect PC→`DAT_800b3f70`[sub_id] = **sub_scd table base**). ✅ Engine `op_gosub` CORRECT (w71w6amhp 2026-05-29). Earlier "message handler" label = misread `Evt_exec(0x04)` arg byte. **[#17]** Gosub also inits the new frame's block-level/-loop to -1 + rebases block_sp = fresh empty per-frame block stack | ghidra1:149773-149799 | high |
| LAB_8003fb9c (0x17 Goto) | **Goto** byte-true: jump pc += (s16)pc[4..5]; UNWIND block_sp = base+(s8)pc[1]*4+4 (empty=-1→0) + loop-counter = pc[2]. Per-frame (call_depth). **Ported [#17]** op_goto: 15/22 real Gotos use pc[1]!=0xFF (out-of-block unwind), pc[2] always 0xFF. If-push LAB_8003f328 = signed block-level++ | ghidra1:149753-149772 | high |
| LAB_8003fc50 (0x19 Return) | pop frame: call_depth--, PC=call_stack[depth-1], restore caller block_sp = base+(s8)caller_block_level*4+4. Per-frame isolation | ghidra1:149800-149822 | high |
| LAB_8003fcf4 (0x21 Ck) | flag predicate: group=pc[1], idx in u16@pc[2] (bit=u16&0x1f, word=(u16>>5)&7), value=pc[3]; result = (flag_bit!=0) XOR (value==0). ✅ `op_ck` byte-true [#19] (full value, not &1) | ghidra1:149843-149867 | high |
| LAB_8003fd54 (0x58) / LAB_8003fe90 (0x59) | **RE1.5 indexed flag CHECK / SET** (NOT RE2 Plc_rot/Xa_on!): flag word indexed via **DAT_800b0fd0[pc[2]]>>5** (= the AOT-zone scratch written by FUN_80042bac). 0x59 op=pc[3] (1 OR / 0 AND-clear / 7 XOR). Empirical scan: 0x58 = 0 uses, 0x59 = 1 use (room1030/sub09). ⚠️ Port maps 0x58→op_plc_rot (DEAD), 0x59→op_xa_on (wrong but PC-correct) — faithful port DEFERRED, coupled to #14 (DAT_800b0fd0) [#19] | ghidra1:149868-149954 | high |
| 0x8003f258 (0x02 Evt_next) / 0x8003f930 (0x11 Do) / 0x8003f8bc (0x12 Edwhile) | Evt_next: pc+1 yield. Do/Edwhile: do-while block | ghidra1:149119-149583 | high |
| LAB_8003f428 (0x0A Sleeping) | Sleep(N) yields EXACTLY N ticks — on the tick counter hits 0: pc+3, free slot, FALL THROUGH to LAB_8003f488 `ori v0,zero,0x2` → return 2 (next opcode tick N+1). Setup LAB_8003f3e0 reads LE u16 | ghidra1:149224-149270 | high |
| LAB_80041e98 (0x41 Plc_neck) | mode 0-4 → actor+0x1b8 neck_flags (0x12/4/8/2a/58); targets s16 → +0x160/162/164; split byte +0x9e/9f. bit0x4=override,0x8=extra-yaw,0x40=countdown-revert | ghidra1:152345-152414 | high |

## Lighting / NCCT
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| 0x80053fc0 | Per-actor light setup: param_1=int[3] world pos (eval_pos = actor own pos, CONFIRMED); dist to ≤3 lights; FUN_800542dc writes LLM/LCM; SetBackColor from ambient | FUN_80053fc0.c:34-38 | high |
| 0x800542dc | LLM+LCM row builder (3×, per light): VectorNormal(dir); −global_scale×N→LLM (DAT_80076d14); color<<4→LCM (DAT_80076d34) | FUN_800542dc.c | high |
| 0x80039ca0 | NPC render+light: RotMatrix+FUN_80022da0, FUN_80053fc0 w/ NPC pos@+0x50, matrices, FUN_80014368 (RVD), dispatch FUN_800254a0 | ghidra1:138111-138244 | high |
| 0x80039b2c | NPC light init: 0x808080 face_rgb @NPC+0x64 → CODE=0x80; LCM from light.lit <<8 | FUN_80039b2c.c | high |
| GTE NCCT 0x0118043F | funct=0x3F, lm=1, sf=1. 3-stage: LLM·V→IR; (BK<<16)+LCM·IR→IR_c; CODE×IR>>12→RGB. CODE=0x80 | inline_n.h:816-820 | high |
| 0x800254a0 | Mesh renderer: per-tri gte_ldv3→gte_ncct→stsxy3_gt3→strgb3_gt3→avsz3 | FUN_800254a0.c | high |

## RVD / AOT
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| 0x80014368 | Point-in-quad: world pos in trapezoidal RVD quad (4 cross-product edge tests). Zone has 4-byte prefix | ghidra1:80204-80258 | high |
| 0x80014230 | RVD zone scan / CAM_SWITCH: iterate 20-byte zones (DAT_800aca88), FUN_80014368 per zone w/ player pos, on match FUN_800142f4 | ghidra1:80082-80135 | high |
| FUN_800436a8 | **AOT-scan dispatcher (3 pools)** — main loop @0x8001ce1c, runs AFTER the door/object loop FUN_8002bd44 (@0x8001ce14). Tests FUN_80042bac(entity, kind, 0) for: PLAYER (DAT_800aca54, kind=1) @800436cc; EVERY active enemy (DAT_800acc2c stride 0x1f4, count DAT_800aca4e, kind=2) @80043724; EVERY object/AOT (DAT_800b3f98 stride 0x94, count DAT_800ac778[2], kind=4) @8004378c. FUN_80042bac iterates the zones internally. ⚠️ Port `re15_aot_scan` tests ONLY the player + uses edge-triggers (was_inside) — the 3-pool/level-trigger refactor = #14, DEFERRED (savestate-verification gated, high ROOM1170-regression risk) | ghidra1:154537-154617 | high |
| 0x80042bac | Per-entity AOT test (called per pool-entity by FUN_800436a8). Iterates zone-ptr table DAT_800ac9b0[] (count DAT_800afbb4). **Zone struct (SAVESTATE-VALIDATED 2026-06-28 vs stage_saves/mzd_demo STAGE1 RAM):** `[0]`=type (dispatch PTR_LAB_8007469c[type], handler arg = zone+0x14 if flags&0x80 else zone+0xc); `[1]`=**kind-mask in low 3 bits (0x1 player / 0x2 enemy / 0x4 object — `&kind`@80042c84) + 0x10 (RVD/separate-caller filter vs a2 @80042c98) + 0x20 center-test + 0x40 reach-test + 0x80 handler-arg select**; `[2]`=**band: (band&0x80)→ignore else entity+0x82==band** (@80042cac); `[3]`=shape (0x80 has-shape, 0x40 quad FUN_80014368 else rect FUN_80042b64, &0x3f shape-idx); `[4..0x13]`=rect/quad corner offsets. **NO previous-frame "inside" field → level-trigger (fires every frame inside; re-trigger suppressed in the type HANDLER, not the scan).** Real STAGE1 data: 5×{type2,mask0x31,band1}=RVD(player,bit0x10) + 2×{type8,mask0x47,band0}=action(all 3 pools). ⚠️ Port `re15_aot_scan` = player-only + edge/action-press + folded door = #14/#15 DEFERRED | ghidra1:153690-154057 | high |
| 0x80042b64 | AABB rect test: a1 cx,cz@+0/+2, half_w/h@+4/+6; player x@+0/z@+8 | ghidra1:153663-153686 | high |
| 0x8002c18c | AOT settle/scan + object render: iterate props (DAT_800b3f98), FUN_80053fc0 light, **type==3 → FUN_8003e64c (sprite.pri path, SKIPS the whole-object cull); type != 3 → whole-object FUN_80014368 quad cull + mesh**. ⚠️ ALL room1170 props are type 0 (verified 2026-06-13) → they take the quad-cull else-path; the type-3 path is DORMANT in room1170 | ghidra1:118917-119102 | high |
| 0x8003e64c | Sprite.pri AOT handler (type-3): GTE matrices, decode zone dir (+0x6a>>0xa), iterate sprite-zone via FUN_80014368, set/clear 0xC00 occlusion mask. (No type-3 prop in room1170 → not exercised) | ghidra1:148119-148370 | high |
| 0x80040914 | **Obj_model_set (SCD opcode 0x2D) handler** (jump-table 8007455c). FIXED **34-byte** opcode (advances thread PC by +0x22 in EVERY (type&0xc0) branch @80040aa4/aa8). Reads: slot=pc[1]; **render TYPE=pc[2]** (`lbu 0x2(a2)` → object pool +0x08, the byte FUN_8002c18c branches on); grid-id=pc[3]→+0x09; band=pc[4]→+0x82; enable pc[6..7]→+0x00\|1; flags pc[8..9]→+0x0c; pos X/Y/Z=pc[10/12/14] LE→+0x34/38/3c; rot=pc[16/18/20] LE→+0x68/6a/6c; data ptr=pc[16]→+0x78; collision box pc[22..33]. Pool **DAT_800b3f98 stride 0x94** (a3*0x94). (type&0xc0) only selects the MODEL-DATA TABLE @+0x74 (==0→DAT_80072d4c, ==0x40→DAT_800aca74, 0x80/0xc0→type-indexed) — NEVER the opcode size. ⚠️ The old engine read TYPE from pc[4] (the BAND field) → mis-tagged obj00/01=3, heli=5; ALL are actually type 0. .scd-verified: 0x2D at room1170 main00 offsets 80→114 = 34 apart | ghidra1:150718-150860 | high |

## Render / fade / letterbox
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| 0x80016b54 | GT3 polygon batch renderer (per-entity, from FUN_800166c4): RTPT, gte_stopz near-cull (OTZ<0), gte_stotz OT insert, NO far-clip, DivideGT3 subdiv | FUN_80016b54.c | high |
| 0x80021a0c | **Real per-frame fade FSM**: DAT_800b5568 ±0x10/frame (dir=bit0x10 of DAT_800aca38), range 0x00–0xF0 (~15 steps ≈0.5s @30fps), writes RGB to DrawEnv bg + AddPrim PolyF4 overlays; suppressed if bit0x4000 | FUN_80021a0c.c | high |
| 0x80020f8c | One-time init + **letterbox PolyF4 init**: double-buffered SetPolyF4+SemiTrans (STP=1) two black quads (DAT_80072ecc top / 80072efc bottom). **Cinematic bar geometry: top y0=0→y2=0x18=24px, bottom y0=0xD8=216→240, width 0x140=320** (NOT 48px=Config/Credits FUN_8003014c, NOT our 17px). RGB driven by the SAME DAT_800b5568 counter as the fade (one unified FSM) | ghidra1:100541-100744 | high |
| 0x8002bd44 | NPC/enemy AI dispatch loop (count@DAT_800ac778+2, table DAT_800b3f98 stride 0x94). (0x8002bda8 = mid-body) | ghidra1:118614 | high |
| 0x8003014c | Config/Credits dialog-box TILE updater (48px bars) — NOT cinematic letterbox | FUN_8003014c.c | high |

## Sprite / BG / MDEC / TIM
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| FUN_800392d4 | Sprite primitive builder: reads sprite.pri per-cut table, SPRT/TILE prims w/ TPage 0x7800 (ori 0x7800+sh), SetShadeTex | ghidra1:137242-137388 | high |
| 0x80039590 | Sprite-pri draw-flag processor (DAT_800b2584 flags from FUN_800392d4) → SPRT overdraw layer. From render @0x8001ce54 | ghidra1:137451 | high |
| 0x800396fc | Room sprite-subsystem orchestrator: FUN_800314b0+FUN_800443ec+FUN_80019354. From room loader FUN_8001d600 | ghidra1:137568 | high |
| 0x800194f8 | TIM atlas batch loader: 8 TIMs → VRAM TPages 0xF..0x8 (OpenTIM+ReadTIM+LoadImage per) | ghidra1:86342-86436 | high |
| 0x80068c88 | LoadImage (libgpu public entry): RECT*+pixels, dispatch via `_dws` (0x8006A1E8) for DMA upload | ghidra1:211156 | high |
| 0x8006BBBC | OpenTIM: stores u_long* into DAT_8008ff68, returns 0 | ghidra1:215712 | high |
| 0x8006bbcc | ReadTIM: parse TIM fields from DAT_8008ff68 → TIM_IMAGE | ghidra1:215728 | high |
| 0x8002a2a8 | BSS stream init: DecDCTReset+DecDCToutCallback+StSetStream+CdRead2. From cinematic entry FUN_80029cd8 | ghidra1:116188 | high |
| 0x8002a630 | BSS per-frame MDEC pump: StGetNext+DecDCTReset(1)+DecDCTin+DecDCTout, double-buffered | ghidra1:116444 | high |
| VLC id 0x3800 | MDEC VLC magic (2nd header word). NOT an address. DecDCTvlc entry = 0x8006dd64 | VlcDecoder.java | high |

## Audio (SPU / VAB) — BO-round 2026-05-29
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| SsUtKeyOnV | SCD Se_on vol 0..127 → SPU 0..0x3FFF LINEAR (no halving); quadratic curve is downstream in libsnd | SsUtKeyOnV.c:83 | high |
| note2pitch2 | SPU pitch from VAB tone center-note (tone+4) + shift (tone+5) + MIDI note → getSPUSampleRate | note2pitch2.c:1-27 | high |
| SpuVmKeyOnNow | 4-stage quadratic vol chain voll×master×prog×tone → (V*V)/0x3FFF | SpuVmKeyOnNow.c:13-48 | high |
| SpuVmDoAllocate | writes ADSR1 from tone+0x10, ADSR2 from tone+0x12 to SPU | SpuVmDoAllocate.c:25-29 | high |
| LAB_80041624 (Se_on) | packs (bank<<24)|(vol<<16)|sample_id; byte[3] is a VOICE-COUNT gate (FUN_80045024 `if 0x20<v return`), NOT amplitude; real L/R vol = dist-based FUN_80045a64 | ghidra1:151736 | high |

## Camera / RVD (BO-round detail)
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| main @0x8001cce0 | per-frame: if bit0x100 of DAT_800aca3c SET → SKIP the FUN_80014230 RVD scan (cut-auto gate). Zero-init=disabled; Cut_auto(1) clears bit | ghidra1:94299-94303 | high |
| FUN_80014230 (anchor rule) | RVD scan starts at the SECOND zone of each cam_from group; the FIRST (anchor) is never collision-tested. Zones grouped by cam_from | ghidra1:80082-80135 | high |

## Save / item-box (RE2_Quellcode_Overlays primär / RE2_Quellcode_V2 fallback — future rebuild)
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| FUN_80032150 | MC/CD op initiator: select card/CD/RAM by slot, set I/O bufs, op-size 0x10/0x18, activate FSM (bit0x200 DAT_800cfb74). Save+load | FUN_80032150.c | high |
| FUN_80032340 | Per-frame MC/CD I/O FSM: dispatch PTR_LAB_800a23ac[state] until <2. Each VSync from FUN_8002b968 | FUN_80032340.c | high |
| FUN_80026b7c | Game load/resume: DAT_800d4a2c new-vs-load; load restores live record DAT_800ce550 + FUN_8004a3c0 room load | FUN_80026b7c.c | high |
| FUN_80034a04 | New-game save record init: defaults to *DAT_800ce550 (coords 0x17f4/0/0x6db5/0x400) | FUN_80034a04.c | high |
| FUN_800703b8 | Box deposit/withdraw SLOT SWAP: inv DAT_800d5bfc / box (DAT_800d5c14+2)&0x3f; cVar12 0=withdraw/1=deposit/2=discard/3=split | FUN_800703b8.c | high |
| DAT_800d4a68 | Box 64-slot ring, 4 B/slot [id,qty,flags,pad], idx&0x3f | FUN_800703b8.c:26 | high |
| DAT_800d4a3c | Inventory 11 slots (0-10), 4 B/slot; runtime count DAT_800d46ac (forced 8 in box mode) | FUN_8006b358.c | high |
| FUN_80068f9c | Box mode enter/exit: 0x0e deposit / 0x0f withdraw / else exit; saves+sets count=8 (hide top 3) | FUN_80068f9c.c | high |
| FUN_80069714 | Inventory compaction after removal: shift slots down, zero last, update cursors | FUN_80069714.c | high |
| FUN_80069668 | Inventory free-slot scanner: 0=first empty, !=0 count occupied; 0xffffffff if none | FUN_80069668.c | high |
| FUN_8006947c | Direct single-slot write at idx: id/qty/flags @DAT_800d4a3c+slot*4 | FUN_8006947c.c | high |

## Prop / fixture rendering + perf (2026-06-02, this session)
| Addr | Purpose | Evidence | Conf |
|------|---------|----------|------|
| FUN_8002c18c | Prop render dispatch (per obj, stride 0x94): RotMatrix → FUN_80053fc0 (per-light setup) → SetColorMatrix + MulMatrix0(LLM,objrot)+SetLightMatrix → SetRot/Trans; then type==3 → FUN_8003e64c; **type != 3 (e.g. type 0 — ALL room1170 props) → whole-object quad-cull FUN_80014368 → FUN_800254a0/FUN_800256b0 lit, or flag-0x10 → FUN_80022f0c/98**. The "type 4/5" framing was from the old pc[4] mis-read; the real type byte is pc[2] (see 0x80040914) and every room1170 prop is type 0 | FUN_8002c18c.c:23-58 | high |
| FUN_8003e64c | Type-3 FIXTURE draw + per-tile occlusion: FUN_80022f0c(tri)+FUN_80023098(quad), then a tile-grid loop sets each tile's prim flag 0xc000000 via point-in-quad FUN_80014368 (occlusion PRIORITY, not a fill cull) | FUN_8003e64c.c | high |
| FUN_80022f0c | Type-3 tri draw: param_3->cd=code; gte_ldrgb(param_3); per-tri gte_ldv3+gte_rtpt+gte_stsxy3_gt3+gte_nclip+gte_avsz3; prim colour = CONSTANT *param_3 (obj+0x70), NO gte_ncct → FLAT/UNLIT (fixture shade baked in texture) | FUN_80022f0c.c:24-53 | high |
| FUN_8002b898 | Object init: obj+0x70 (the CODE/flat colour, &DAT_800b4008) = 0x808080 | FUN_8002b898.c:16 | high |
| FUN_8002d0e8 | RE2 canonical per-poly loop: gte_rtpt_b/nclip_b/avsz3_b (NON-blocking) + software pipelining (pre-load next verts) + gte_stsxy3_gt3 DIRECT to prim + inline OT swap-link + direct prim-ptr advance (NO function calls/poly) | RE2 FUN_8002d0e8.c | high |
| FUN_800254a0 | RE2/RE1.5 lit tri NCCT loop: gte_ldrgb(CODE, cd=param<<1|0x34) once; per tri gte_ldv3(normals)+gte_ncct+gte_stsxy3_gt3+gte_strgb3_gt3 (SXY survives ncct) | FUN_800254a0.c | high |
| DAT_800b0fe4 | THE active cut index — used for BOTH camera (base+0x24, ×0x20) AND lighting (base+0x2c, ×0x28). Locked: camera-cut ≡ light-cut | FUN_80053fc0.c:30, FUN_80021bbc.c:45 | high |
| PERF: soft-float | R3000 has NO FPU → every double/float op = libgcc soft-float (~1000cyc/double-div). RE2 lighting (FUN_80053fc0/FUN_800542dc) is ALL integer + libgte SquareRoot0/VectorNormal. Our light-setup double-sqrt cost ~17-20ms/frame; integer isqrt → 16-19→25-30fps | this session | high |
| PERF: frame loop | submit (DrawOTagEnv, non-blocking) BEFORE the VSync wait (RE2 main.c:45-46) so GPU draws during the wait. + adaptive 3D skip on MDEC/cut frames (FUN_8002137c/FUN_8002af20) | this session | high |
| re15_room_load / re15_cdfs (OURS) | Multi-room foundation: CdSearchFile+CdRead a ROOM####.RDT into a resident reusable buffer (re15_room.c) + stream BG/TIM assets (re15_cdfs.c) instead of incbin. RAM 40KB→639KB | this session | n/a |
| FUN_8001b064 | RE1.5 character floor SHADOW draw (per entity): RotMatrixY(cam+0x6a) camera-yaw, corners ±500X/±600Z centered on camera focus, RotAverage4 project, clone+emit the FUN_8001af5c template. Skipped if work-flag bit 0x400. PORTED to PSX 2026-06-03 (mesh_psx_render_actor_shadow) | ghidra1_V2.txt:89495-89699 | high |
| FUN_8001af5c | RE1.5 shadow PRIM template builder: POLY_FT4 (SetPolyFT4, code 0x2C) + SetSemiTrans(p,1) + tpage getTPage(tp=1, abr=2 SUBTRACTIVE, ...) + GetClut(272,y); UV (1,225)-(26,254) blob in TEX.TIM; prim RGB 0x808080 neutral (darkening = ABR2, not colour) | ghidra1_V2.txt:89380-89446 | high |
| FUN_80028868 | RE1.5 subtitle TEXT renderer: per-glyph SPRT from TEX.TIM 4bpp font (U=(code&0xF)<<4, V=(code&0xF0)+0x20, page (256,0)), per-glyph advance (&DAT_800c4416)[code]; control 0x01 end/0x05(2B) colour→CLUT row (N&3)*2+(N>>2)/0x08 newline. PORTED to PSX 2026-06-03 (re15_render_msg_text, POLY_FT4) | ghidra1_V2.txt FUN_80028868 | high |
| DAT_800c4416 | 256-byte per-glyph advance-width table (BSS, from DEBUG.BIN[0x4416], landmark [0x44b6]=0xFFFF). Embedded as font_width.h | DEBUG.BIN | high |

## RE2 RENDERING PIPELINE (from the 85-agent render audit 2026-06-02b; ghidra_source_re2_Leon.txt) — the model our PSX port should converge on
| RE2 FUN_/addr | Purpose | Source | Conf |
|---|---|---|---|
| FUN_80027bec | RE2 lit textured TRIANGLE loop (the real one; 800254a0/256b0 don't exist in that build): hoist gte_ldrgb; per-poly ldv3→rtpt_b→[CPU next-poly ptrs]→nclip_b→stopz(cull FIRST)→avsz3_b→stsxy3→stotz→ldv3(normals)→ncct→strgb3; NO per-poly gte_stflg; cross-poly software-pipelined; 4 cop2 reads/tri | ghidra_re2:15303-15385 | high |
| FUN_80027dbc | RE2 lit textured QUAD loop (sibling; 6 cop2 reads/quad, +rtps/nccs for v4) | ghidra_re2:15388-15469 | high |
| FUN_8002d0e8 | RE2 UNLIT direct prim-emit/addPrim loop (3 cop2 reads, no color) | ghidra_re2:17001-17059 | high |
| FUN_8002da80 / FUN_8002ddf0 | RE2 PERSISTENT pre-baked per-model packet pool (GT3 / GT4, stride 0x14 words): UV/clut/tpage/code baked ONCE at load; hot loop writes only XY(FIFO)+RGB(FIFO)+OT-link. THE big CPU lever we lack | ghidra_re2| high |
| FUN_8002ce94 | RE2 CompMatrixLV — GTE-accelerated view×bone matrix concat (we do it in software C); the per-bone light matrix is a free by-product | ghidra_re2| high |
| FUN_8002ea40 / FUN_8002eaf8 / FUN_8002e2d8 | RE2 pose/redraw DIRTY-GATE: rebuild pose+projection only on keyframe advance; memcpy a cached prim template for unchanged objects (we rebuild every frame) | ghidra_re2| high |
| FUN_8002c820 | RE2 whole-object integer 2D-AABB reject BEFORE the poly loop (cheap early-out we lack) | ghidra_re2| med |
| PERF: CPU-bound (MEASURED) | PSX intro is CPU-bound NOT GPU-fill (GPU-wait=0 always). per-poly EMIT = 84% of the full-cast frame; cost class = UNCACHED MEMORY (R3000 no D-cache), not GTE stalls. Profiler in render.c (RCntCNT1, RE15_PROFILE_HUD) | this session | high |
| PERF: rank-1 WIN (DONE) | gte_ldv3 DIRECT from MD1 arrays (re15_md1_vertex_t ≡ SVECTOR, no per-poly stack copy) → full-cast 619→477 ticks, 25→29fps. Removing gte_stflg / _b pipelining = NO win (stall relocates) | this session | high |
