---
name: untested-implementations
description: "LIVING REGISTER of canonical fixes that are APPLIED + build-verified + logic-correct, but NOT runtime-verified (the ROOM1170 intro doesn't exercise their code path). When the user asks 'welche Implementierung ist aktuell noch ungetestet?' — answer from here. When a room/scenario that exercises one is reached, runtime-verify it and move it to VERIFIED. Each entry: what, file:line, why untested, what would verify it."
metadata:
  node_type: project
  type: project
  originSessionId: 0a2f4870-554b-42c1-80aa-3553c9ee74f1
---

# Untested implementations register (living)

Canonical fixes that are **applied + build-verified + logic-correct** but **NOT runtime-verified** because
the ROOM1170 intro (our only running scenario) doesn't exercise their code path. **When the user asks
"what's still untested?" → list these.** When we reach a room/scenario that exercises one, runtime-verify
it and strike it through (move to "verified").

## 🔴 UNTESTED — needs a runtime scenario to verify
| # | Implementation | File:line | Why untested | What would verify it |
|---|---|---|---|---|
| U1 | **Plc_neck s16 read** (operands as 4×s16 LE, not u8<<4) | scd_vm.c op_plc_neck | ROOM1170 has no Plc_neck | A room where an SCD fires Plc_neck (head-look) — head turns to the right target, no 360° flip |
| U2 | **Neck sign-mask fix** (signed-12bit restore vs `&0x0FFF`) | skeleton_common.c:191 | Dead path in intro (lookat_y/x=0) | Same Plc_neck room — negative neck offset doesn't wrap to ~+360° |
| ~~U3~~ | ~~**RUN motion → W01 sentinel 100**~~ ✅ VERIFIED 2026-07-12 | re15_to_re2.c:199 | — | ROOM1140: Leon runs (X+fwd) with **mo=100** (RUN sentinel, not walk) at **~200/frame** (RUN=0xC8), walls at x=−5118. Byte-true. |
| U4 | **Ifel_ck non-Ck condition eval** (Member_cmp real / Sce_key_ck+Cmp conservative-FALSE) | scd_vm.c scd_eval_condition | Intro is Ck-only (Ck path IS verified); the 483 non-Ck branches are in other rooms | A room with Member_cmp/Sce_key_ck/Cmp Ifel_ck — branch taken iff the real condition holds |
| ~~U5~~ | ~~**mode7/8 backward-walk speed 70**~~ ✅ VERIFIED 2026-07-12 | actor_locomotion.c:77-78 | — | ROOM1140: Leon walks backward (D) at a measured **69.7/frame** (F69→F70 dx=−69,dz=−10 → \|v\|=70) = disasm 0x46=70. Byte-true. |
| U6 | **work-slot leak fix on op_member_set2/add_aspeed/plc_rot** (per-thread) | scd_vm.c | Intro path didn't hit these three ops cross-thread | A multi-thread scene using Member_set2 / Add_aspeed / Plc_rot under concurrent Work_set |
| U7 | **Door/Item SFX fabrication REMOVED** (was {bank2,sample2,vol0x60/0x50,pan0x40}) | aot_common.c door/item handlers | Intro has no doors/item pickups | RESOLVED (wf_8e6a4d88): the RE1.5 door TRANSITION plays NO sound — the trigger FUN_800430bc only STOPS sound (FUN_80061fc0(-1)). So the door itself is correctly silent; any door-open audio would be a room-SCD Se_on (already SCD-driven in the port). The transition is a fade-only FSM (no door model) — byte-true fade-IN now added (Commit 4816fec1). Item-pickup Se_on still needs a room with a pickup to verify. |
| U8 | **VAB tone ADSR** (real tone+0x10/0x12 envelope vs dummy 0x00FF/0x0000) | audio_psx.c play_sample | audio_psx.c is the PSX-target SPU path — NOT in the PC build (PC uses audio_pc.c SDL); not even build-compiled here | A PSX build + audio: SFX have proper attack/decay/release, not flat full-volume |
| U9 | **VAB pitch (note2pitch + LUT) — FOUNDATION only** | vab_common.c (helpers built+verified); audio_psx.c pitch NOT yet wired (kept 22050) | note2pitch/LUT/tone-table are build-verified, but the consumer pitch wiring is uncertain (play_sample gets a VAG index not the Se_on note; 22050 implies a per-bank base the LUT doesn't encode) | A PSX build + audio + the Se_on→note + bank→program plumbing RE'd; then SPU_CH_FREQ = note2pitch(note,center,shift) scaled by the bank base |

| U10 | **pri_common.c square-mask `(size>>4)*8`** (was raw byte = 2× too big) | pri_common.c:95 | sprite.pri overdraw is currently DISABLED in the engine | A room with sprite.pri foreground masks + overdraw re-enabled — square occluders are the right size (not 2×) |

> NOTE: U8/U9's groundwork (re15_vab_tone_t + tone-table parse + the 192-u16 pitch LUT + re15_vab_note2pitch
> + re15_vab_find_tone in vab_common.c) IS build-verified in the PC build. Only the audio_psx.c CONSUMER
> (PSX SPU path) is untested/unbuilt-here.

## ✅ Move here once runtime-verified (date + how)
- **U3 — RUN motion sentinel 100** (2026-07-12): ROOM1140 live, Leon runs (X+fwd) → state-log `mo=100`
  (RUN sentinel = W01 clip 0, NOT walk clip) at ~200/frame (RUN=0xC8=200); walls at x=−5118.
- **U5 — backward-walk speed 70** (2026-07-12): ROOM1140 live, Leon walks backward (D) → measured
  69.7/frame (F69→F70 dx=−69, dz=−10 → |v|=70), matching the disasm mode-7/8 constant 0x46=70.

## How I maintain this
- Every canonical fix I apply that the intro doesn't exercise → add a 🔴 row.
- When a runtime scenario exercises one → test it, then strike it + note in ✅ with date + evidence.
- Canonical location: this file at repo root (`UNTESTED_IMPLEMENTATIONS.md`). No psx_dev/ mirror exists in reAi_v2.
