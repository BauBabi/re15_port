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
| U3 | **RUN motion → W01 sentinel 100** (no-overlay catalog; was aliased to WALK clip 19) | re15_to_re2.c:199 | ROOM1170 uses the rbj overlay branch, not this | A gameplay room where Leon RUNs without an rbj overlay → real run animation (not walk) |
| U4 | **Ifel_ck non-Ck condition eval** (Member_cmp real / Sce_key_ck+Cmp conservative-FALSE) | scd_vm.c scd_eval_condition | Intro is Ck-only (Ck path IS verified); the 483 non-Ck branches are in other rooms | A room with Member_cmp/Sce_key_ck/Cmp Ifel_ck — branch taken iff the real condition holds |
| U5 | **mode7/8 backward-walk speed 70** (was placeholder 96) | actor_locomotion.c:77-78 | Intro uses walk(4)/run(5)/turn(9), not mode 7/8 | A scene with a backward-walk Plc_dest (mode 7/8) |
| U6 | **work-slot leak fix on op_member_set2/add_aspeed/plc_rot** (per-thread) | scd_vm.c | Intro path didn't hit these three ops cross-thread | A multi-thread scene using Member_set2 / Add_aspeed / Plc_rot under concurrent Work_set |
| U7 | **Door/Item SFX fabrication REMOVED** (was {bank2,sample2,vol0x60/0x50,pan0x40}) | aot_common.c door/item handlers | Intro has no doors/item pickups | A room with a door / item pickup — the sound must come from the room SCD's Se_on (currently silent: our door is C-driven, so the canonical SCD-event-sub→Se_on path is a FOLLOW-UP — wire AOT event_sub_id → fire the sub) |
| U8 | **VAB tone ADSR** (real tone+0x10/0x12 envelope vs dummy 0x00FF/0x0000) | audio_psx.c play_sample | audio_psx.c is the PSX-target SPU path — NOT in the PC build (PC uses audio_pc.c SDL); not even build-compiled here | A PSX build + audio: SFX have proper attack/decay/release, not flat full-volume |
| U9 | **VAB pitch (note2pitch + LUT) — FOUNDATION only** | vab_common.c (helpers built+verified); audio_psx.c pitch NOT yet wired (kept 22050) | note2pitch/LUT/tone-table are build-verified, but the consumer pitch wiring is uncertain (play_sample gets a VAG index not the Se_on note; 22050 implies a per-bank base the LUT doesn't encode) | A PSX build + audio + the Se_on→note + bank→program plumbing RE'd; then SPU_CH_FREQ = note2pitch(note,center,shift) scaled by the bank base |

| U10 | **pri_common.c square-mask `(size>>4)*8`** (was raw byte = 2× too big) | pri_common.c:95 | sprite.pri overdraw is currently DISABLED in the engine | A room with sprite.pri foreground masks + overdraw re-enabled — square occluders are the right size (not 2×) |

> NOTE: U8/U9's groundwork (re15_vab_tone_t + tone-table parse + the 192-u16 pitch LUT + re15_vab_note2pitch
> + re15_vab_find_tone in vab_common.c) IS build-verified in the PC build. Only the audio_psx.c CONSUMER
> (PSX SPU path) is untested/unbuilt-here.

## ✅ Move here once runtime-verified (date + how)
_(empty — fill as rooms are reached)_

## How I maintain this
- Every canonical fix I apply that the intro doesn't exercise → add a 🔴 row.
- When a runtime scenario exercises one → test it, then strike it + note in ✅ with date + evidence.
- Canonical location: this file at repo root (`UNTESTED_IMPLEMENTATIONS.md`). No psx_dev/ mirror exists in reAi_v2.
