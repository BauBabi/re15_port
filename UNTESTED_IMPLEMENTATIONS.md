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
| ~~U1~~ | ~~**Plc_neck s16 read** (operands as 4×s16 LE, not u8<<4)~~ ✅ VERIFIED 2026-07-12 | scd_vm.c op_plc_neck | — | ROOM1150 Irons-cutscene (sub03/thread#3, fired via `RE15_FORCE_EVENT=3`): op 0x41 logs `tgt=(-22000,-720,-26131) speed=24672` — the s16-LE decode reproduces the RDT bytes at scd 0xf04 **byte-identically** (`41 01 10 aa 30 fd ed 99 60 60`); the old u8<<4 split would give garbage. Leon looks at the dying Irons. Byte-true. |
| ~~U2~~ | ~~**Neck sign-mask fix** (signed-12bit restore vs `&0x0FFF`)~~ ✅ VERIFIED 2026-07-12 | skeleton_common.c:362 | — | Same cutscene, `RE15_NECK_TRACE`: head-look slew `neck_yaw 96→192→288→372` (constant step=96=0x60 Plc_neck speed) **settles at resY=372** (~33°, signed-12 range) and holds — NO wrap to ~+360°. The residual sign-restore `((x+0x800)&0xFFF)-0x800` is what keeps a negative offset from becoming ~4000. Head turns naturally to Irons, letterbox active. Byte-true. |
| ~~U3~~ | ~~**RUN motion → W01 sentinel 100**~~ ✅ VERIFIED 2026-07-12 | re15_to_re2.c:199 | — | ROOM1140: Leon runs (X+fwd) with **mo=100** (RUN sentinel, not walk) at **~200/frame** (RUN=0xC8), walls at x=−5118. Byte-true. |
| U4 | **Ifel_ck non-Ck condition eval** (Member_cmp real / Sce_key_ck+Cmp conservative-FALSE) | scd_vm.c scd_eval_condition | Intro is Ck-only (Ck path IS verified); the 483 non-Ck branches are in other rooms | A room with Member_cmp/Sce_key_ck/Cmp Ifel_ck — branch taken iff the real condition holds |
| ~~U5~~ | ~~**mode7/8 backward-walk speed 70**~~ ✅ VERIFIED 2026-07-12 | actor_locomotion.c:77-78 | — | ROOM1140: Leon walks backward (D) at a measured **69.7/frame** (F69→F70 dx=−69,dz=−10 → \|v\|=70) = disasm 0x46=70. Byte-true. |
| U6 | **work-slot leak fix on op_member_set2/add_aspeed/plc_rot** (per-thread) | scd_vm.c | Intro path didn't hit these three ops cross-thread | A multi-thread scene using Member_set2 / Add_aspeed / Plc_rot under concurrent Work_set |
| U7 | **Door/Item SFX fabrication REMOVED** (was {bank2,sample2,vol0x60/0x50,pan0x40}) | aot_common.c door/item handlers | Intro has no doors/item pickups | RESOLVED (wf_8e6a4d88): the RE1.5 door TRANSITION plays NO sound — the trigger FUN_800430bc only STOPS sound (FUN_80061fc0(-1)). So the door itself is correctly silent; any door-open audio would be a room-SCD Se_on (already SCD-driven in the port). The transition is a fade-only FSM (no door model) — byte-true fade-IN now added (Commit 4816fec1). Item-pickup Se_on still needs a room with a pickup to verify. |
| U8 | **VAB tone ADSR** (real tone+0x10/0x12 envelope vs dummy 0x00FF/0x0000) | audio_psx.c play_sample | audio_psx.c is the PSX-target SPU path — NOT in the PC build (PC uses audio_pc.c SDL); not even build-compiled here | A PSX build + audio: SFX have proper attack/decay/release, not flat full-volume |
| U9 | **VAB pitch (note2pitch + LUT) — FOUNDATION only** | vab_common.c (helpers built+verified); audio_psx.c pitch NOT yet wired (kept 22050) | note2pitch/LUT/tone-table are build-verified, but the consumer pitch wiring is uncertain (play_sample gets a VAG index not the Se_on note; 22050 implies a per-bank base the LUT doesn't encode) | A PSX build + audio + the Se_on→note + bank→program plumbing RE'd; then SPU_CH_FREQ = note2pitch(note,center,shift) scaled by the bank base |

| U10 | **pri_common.c square-mask `(size>>4)*8`** (was raw byte = 2× too big) | pri_common.c:95 | sprite.pri overdraw is currently DISABLED in the engine | A room with sprite.pri foreground masks + overdraw re-enabled — square occluders are the right size (not 2×) |
| ~~U11~~ | ~~**Item-get modal + "WILL YOU TAKE THE X?" Yes/No box**~~ ✅ VERIFIED 2026-07-12 | item_modal_common.c + itps_common.c (FUN_8001db28) | — | Live (`RE15_ITEM_MODAL_TEST=1` + `RE15_MODAL_LOG` + autoshot): byte-true 8-state FSM — 17f zoom-spin, 9f coin-flip, then a MESSAGE BOX "WILL YOU TAKE THE ⟨item⟩." with a Yes/No cursor (or "YOU CAN'T CARRY ANY MORE ITEMS" when full), grant DEFERRED to state 7 and gated on **Yes** (@0x8001e068 andi 0x1 = No-flag → item stays). The picture is the per-item **112×72 TIM from ITEM/ITPS.ITP** at id×0x3000 (NOT the 40×30 icon) — autoshot shows the real "TONY's ARMS" ammo-box photo + the Yes/No prompt. NO pickup SE (adversarial full-jal audit @0x8001db28–0x8001e4b0 = byte-true silent). Freeze via game_step early-return + 30 Hz tick gate; state 6 is PLAYER-GATED. The prompt renders in the real **TEX.TIM game font** by REPLAYING the game's own glyph bytes (BSS scripts @0x800c4fc6 + name blob @0x800c4a28, savestate-extracted → gen/item_prompt_data.inc; name resolver FUN_80028840), TYPEWRITERED at **2 frames/glyph** (DAT_800b5456=2) with the Yes/No confirm gated until fully revealed. The glyph replay is byte-true incl. the GREEN item-name color (0x05 op), Title-Case, and the "?" terminator — "Will you take the H. Gun Bullets?" (the ASCII path had all-caps + "." — wrong). ctest `unit_item_modal` (21/21) + `unit_aot_edge`. **CORRECTION:** the earlier "image-only, no text" was WRONG (FUN_80027e68 = the message-box opener, not a fade — wf_9e42f4cb). **The modal is now byte-true END TO END** — picture (ITPS), zoom/flip, game-font prompt + color + case + punctuation, typewriter, player-gated Yes/No, No-declines. No faithful-line remains. |

> NOTE: U8/U9's groundwork (re15_vab_tone_t + tone-table parse + the 192-u16 pitch LUT + re15_vab_note2pitch
> + re15_vab_find_tone in vab_common.c) IS build-verified in the PC build. Only the audio_psx.c CONSUMER
> (PSX SPU path) is untested/unbuilt-here.

## ✅ Move here once runtime-verified (date + how)
- **U3 — RUN motion sentinel 100** (2026-07-12): ROOM1140 live, Leon runs (X+fwd) → state-log `mo=100`
  (RUN sentinel = W01 clip 0, NOT walk clip) at ~200/frame (RUN=0xC8=200); walls at x=−5118.
- **U5 — backward-walk speed 70** (2026-07-12): ROOM1140 live, Leon walks backward (D) → measured
  69.7/frame (F69→F70 dx=−69, dz=−10 → |v|=70), matching the disasm mode-7/8 constant 0x46=70.
- **U11 — item-get modal + Yes/No box** (2026-07-12): `item_modal_common.c` + `itps_common.c` (byte-true
  FUN_8001db28, workflows wq41xdnn2 + wf_9e42f4cb). Live-verified via `RE15_ITEM_MODAL_TEST=1` +
  `RE15_MODAL_LOG` + autoshot: 8-state FSM (17f zoom-spin → 9f coin-flip → "WILL YOU TAKE THE ⟨item⟩." Yes/No
  box → grant on Yes). The autoshot captures the per-item ITPS.ITP 112×72 photo + the prompt text
  (`SDL_RenderReadPixels` reads the `SDL_RenderGeometry` quad + the text-overlay). ctest `unit_item_modal`
  19/19. CORRECTION of the earlier "image-only, no text" claim — the modal DOES show the take-prompt.

## How I maintain this
- Every canonical fix I apply that the intro doesn't exercise → add a 🔴 row.
- When a runtime scenario exercises one → test it, then strike it + note in ✅ with date + evidence.
- Canonical location: this file at repo root (`UNTESTED_IMPLEMENTATIONS.md`). No psx_dev/ mirror exists in reAi_v2.
