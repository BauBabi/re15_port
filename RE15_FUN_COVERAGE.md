---
name: reference-re15-fun-coverage-map
description: "Known-vs-unknown coverage map of RE1.5/RE2 systems. What we have CODE-VERIFIED (see [[reference_re15_fun_catalog]] for the addresses), what is PARTIALLY known (named/referenced but not fully decoded), and what is UNKNOWN (gaps to RE next). Honest scorecard so future sessions know where to point agents. Update alongside the catalog every round."
metadata:
  node_type: reference
  type: reference
  originSessionId: 0a2f4870-554b-42c1-80aa-3553c9ee74f1
---

# RE1.5/RE2 Coverage Map — known vs unknown

Companion to [[reference_re15_fun_catalog]] (the addresses). This is the honest scorecard: ✅ verified ·
🟡 partial (named, not fully decoded) · ❌ unknown (gap → RE next). Updated 2026-05-29 (BO-round).

## BO-round sky-view cull (2026-05-29) — APPLIED + verified ✅
- **Per-cut region-quad cull** = the canonical hide for ALL off-stage objects (box obj0/1, tail rotor
  obj4, light obj5 at sub02's (-31000,~26000) hide corner). PSX FUN_8002c18c draws a prop only if its
  (X,Z) is inside the active cut's region quad = the ANCHOR (first RVD zone with cam_from==cut), via
  FUN_80014324→DAT_800ac790→FUN_80014368. NO separate region table — it's the zone-table anchor we already
  parse. Engine: re15_rdt_get_region_quad (rdt_common.c) + shared re15_aot_point_in_quad + cull in main.c
  prop & NPC loops. Cut-6 quad (-32700,-32700)(0,32700)(32700,32700)(32700,0) excludes the NW hide corner,
  includes the flying body + helipad box. Replaced the wrong type-3 skip + the |x|<25000 NPC proxy + the
  removed BA far-clip. VERIFIED F1410/F1500/F1780/F504. The type-3 mesh-skip was a WRONG turn (removed the
  end-box) — reverted.

## BO-round Tier-3 + box (2026-05-29) — APPLIED + promoted to ✅
- **Sleep opcode** LAB_8003f428 (yields N not N−1) → ✅ APPLIED (scd_vm.c:698 return 2). Intro #6 root cause.
- **Walker 3-state FSM** LAB_80030AF0 (state0/1/2, FUN_8001ab9c gate, run-arrival 0x12c) → ✅ APPLIED (actor_locomotion.c + walk_fsm field). #6 residual.
- **Elliot stand→walk** (#1): Plc_dest sets no motion; walker sets clip at state1→2 → ✅ APPLIED.
- **Letterbox** FUN_80020f8c (24px) + **fade** FUN_80021a0c (15-step) → ✅ APPLIED (17→24, 6→15, close 23→15).
- **Neck sign-mask** (skeleton_common.c:191 signed-12bit) → ✅ APPLIED (A6-Fix1; dead path in intro).
- **Sky-view flying box** → ✅ APPLIED: it's the type-3 fixture obj 0x00/0x01 (Obj_model_set header type at pc[4]); PSX routes type-3 to the sprite path (FUN_8003e64c) NOT mesh. Fix: capture props[].obj_type, skip type-3 in the mesh loop. Verified: box gone in sky-view, heli (type-5) + helipad fixtures intact. Heli-preserving (NOT a far-clip).
- STILL DEFERRED: unified fade_level PolyF4 PSX-port (A3(3)+A4-PSX), neck spring (A6 Fix2+3), audio Phase 4.7, Tier-4 bone-0 decode.

## BO-round resolutions (2026-05-29) — promoted to ✅
- **cam_to==0 / RVD install** → ✅ FULLY RE'd: zone table is grouped by cam_from; FIRST zone of each group is
  the search-anchor FUN_80014230 never tests (FUN_80014324 finds the group, scan starts at 2nd zone). cam_to=0
  on a non-anchor is a real "switch to cut 0" trigger. Install rule = skip first-per-cam_from-group; needs
  RE15_AOT_MAX≥64. APPLIED.
- **CAM_SWITCH gate** → ✅: PSX skips the RVD scan when bit0x100 of DAT_800aca3c set (main @0x8001cce0);
  Cut_auto(1) clears it. gate = `cut_auto_enabled`. APPLIED.
- **player/NPC far-clip** → ✅: no far-clip on PSX (FUN_80016b54 OTZ<0 only); hide via |x|<25000 proxy. APPLIED.
- **Sce_em_set HP** → ✅: 0x44 carries NO hp field; PSX entity+0x1ba = -(sound_bank*1800) is a root-motion
  stride limiter, not HP. hp=100 nominal is safe; real HP would be an enemy-type table by pc[2].
- **no-normal tint** → ✅: PSX has only Mode A (full NCCT) / Mode B (flat face_rgb=0x808080 neutral); fallback = 128.
- **Audio (SPU/pitch/ADSR)** → 🟡 (newly identified, integration pending): vol = SsUtKeyOnV linear 0..0x3FFF (no
  halving, SPU path already correct); pitch = note2pitch2(center,shift) from VAB tone table (we hardcode 22050 →
  Phase 4.7); ADSR = SpuVmDoAllocate from tone+0x10/+0x12 (we use dummy → Phase 4.7). PC mixer >>1 = legit headroom.
- **sprite.pri overdraw** → ✅ canonical-as-is for ROOM1170 (pri depth always < char OT buckets); currently disabled.

## ✅ KNOWN — code-verified (in the catalog)
| Subsystem | What we have |
|-----------|--------------|
| Player/anim | player-mode FSM (DAT_800aca58, dispatch FUN_80031c44, Plc_ret LAB_80041f88); Plc_motion (motion_id = DIRECT EDD clip index, no table/clamp); anim advance FUN_8001f314; 12-bit angle unpacker FUN_8001f664; speed/add_speed per-thread (LAB_80040f14); Member_get FUN_80041358 |
| Walker | atan2 target FUN_8001a6d4; yaw-slew FUN_8001aac4 / FUN_8001a8f8; position-advance FUN_800245d8; arrival Euclid <0x64 |
| Camera | soft-track animator FUN_80015850 (runs every frame incl. cinematics); lookat-matrix 0x80053ca4; frame dispatcher 0x8001500c; cam-data find 0x80014324; cut-select FUN_800142f4 |
| SCD VM | dispatch table 0x800744a8; tick loop 0x8003f0a0; thread pool/layout (0x170 stride, +0x158 vel); alloc/init slot; Evt_exec (same-tick), Evt_next, For/Next, Do/Edwhile; Cut_chg/Cut_auto (polarity inverted); Plc_dest; Sce_em_set; Speed_set/Add_speed |
| Lighting | full NCCT pipeline VERIFIED CLEAN: setup FUN_80053fc0 (eval_pos = actor own pos), LLM/LCM builder FUN_800542dc, NPC light FUN_80039b2c, gte_ncct 0x0118043F, mesh renderer FUN_800254a0 |
| RVD/AOT | point-in-quad FUN_80014368; zone scan FUN_80014230; Door_aot_set FUN_80042bac; AABB FUN_80042b64; AOT settle FUN_8002c18c; sprite.pri handler FUN_8003e64c |
| Render | GT3 batch renderer FUN_80016b54 (near-clip OTZ<0 only, NO far-clip — CONFIRMED); fade FSM FUN_80021a0c; letterbox PolyF4 init FUN_80020f8c; NPC AI loop FUN_8002bd44 |
| Sprite/BG/MDEC/TIM | sprite builder FUN_800392d4 (TPage 0x7800); draw-flag proc 0x80039590; atlas loader FUN_800194f8; LoadImage 0x80068c88 / OpenTIM / ReadTIM; BSS init 0x8002a2a8 + pump 0x8002a630; VLC id 0x3800 |
| Save/box (RE2) | save FSM init FUN_80032150 + driver FUN_80032340; load/resume FUN_80026b7c; new-game init FUN_80034a04; box swap FUN_800703b8; box ring DAT_800d4a68; inventory DAT_800d4a3c; box mode FUN_80068f9c; compaction/free-slot/slot-write |

## 🟡 PARTIAL — named but NOT fully decoded (in-progress or shallow)
- **Neck damped-spring FSM** FUN_80024e40 (anim_bone_follow_3d.c) — referenced; not decoded. (Tier-3)
- **Walker 3-state FSM** LAB_80030AF0 states + threshold FUN_8001ab9c — partial; full state machine pending. (Tier-3)
- **Bone-0 keyframe decode** FUN_8001f3bc (bone 0 = 3×s16 @frame+6 vs packed) + root translation — KNOWN to be wrong in our code, exact PSX layout pending. (Tier-4)
- **Letterbox geometry** — TWO candidate FUNs (FUN_8003014c Config/Credits 48px vs FUN_80020f8c cinematic PolyF4); which draws the CINEMATIC bars + exact px unresolved. (Tier-1-remaining analysis)
- **RUN 2-state speed** LAB_80030d28 (200→96) — transition condition pending. (Tier-2 analysis)
- **cam_to==0 zone semantics** + AOT-slot reconcile — under analysis (the naive fix broke walking cam-switches). (Tier-1-remaining)
- **sprite.pri OT depth** — pri.depth field + insert order pending. (Tier-2 analysis)
- **Fade exact curve** — FUN_80021a0c (±0x10/frame, 0..0xF0 ≈15 frames) decoded; our integration pending.

## ❌ UNKNOWN — gaps to RE next
- **Sce_em_set HP**: does 0x44 carry HP, or is there an enemy HP table by type? (under analysis)
- **VAB audio**: tone center-note → SPU pitch (we hardcode 22050 Hz); ADSR from VAB program/tone (we use dummy 0x00FF/0x0000); Se_on/Sed_on handler chain + canonical volume.
- **MDEC BG VRAM dest** from RDT cam (+8/+10) — FUN_8002bdf4; we hardcode (640,256).
- **SCD opcodes not decoded**: Ifel_ck chained AND/OR FUN_80053f50; Member_calc/Member_calc2; Kage_set (shadow); Cut_be_set; Sce_espr_* (sprite FX); Plc_cnt; many no-ops we stub.
- **Opcode 0x18 mismatch**: PSX 0x18 = message handler; our engine uses 0x18 = GOSUB + Message_on = 0x2B (PSX 0x2B = FUN_80027e68). Full opcode-table reconcile.
- **Save FSM states** PTR_LAB_800a23ac[0..2] — the per-state handlers not decoded (future rebuild).
- **Per-room RVD record layout** — verified for ROOM1170 only; other rooms unknown.
- **Door/Item SFX** params — our aot_common values are fabricated; real Se_on source in the door/item handler unknown.

## Open ROOM1170 intro items (not pure-FUN, but tracked)
- **#1** Elliot's first-frame run-pose: where he gets motion 100 on frame 1 (spawn motion? a Plc_dest?) — root cause OPEN.
- **#6** cumulative cut-timing drift post-F682 — per-cut calibration pending (relates to walker speed FSM + arrival).

## How to use
New task → find its subsystem above. ✅ = look it up in [[reference_re15_fun_catalog]], don't re-RE.
🟡/❌ = dispatch RE agents (per [[feedback_send_agents_for_re]]), then promote the result to ✅ + add the
address to the catalog. Per [[feedback_always_reverse_engineer_never_analytical_2026_05_29]], never fill a
gap with an approximation — RE it.
