---
name: re15-weapon-render
description: Byte-true RE + port of the equipped-weapon in-hand render (PLW hand+gun mesh replacing the hand bone). Load this for ANY weapon-swap / weapon-in-hand rendering work — player-select AND in-game equip.
---

# RE1.5 Equipped-Weapon Render (weapon-in-hand)

How the original draws the weapon a character HOLDS, byte-true. Characters swap weapons the whole
game (knife → handgun → shotgun …), and each must render correctly — so this is the reusable
mechanism, not a one-off. Fully verified against the LIVE player-select savestate
(`stage_saves/mzd_after_flow.sav`, 2026-07-14). Companion skills: `re15-savestate-ghidra`
(read the ground truth), the model/anim ground-truth memory [[reai-v2-model-anim-groundtruth]].

**Golden rule (the user's mandate): NEVER guess the weapon mesh/attach/texture — read it from a
savestate of the original and byte-match to the asset files.** The `re15_entity_assets.py` tool +
the reads below give every value with zero guessing.

## 1. The PLW file (per-weapon asset)

A character's weapon variant is `PL<char>W<weap>.PLW` (e.g. `PL00W03.PLW` = Leon + weapon 03 =
handgun; `PL04W03` = Elza + handgun). Container header: `word0 = dir_offset`, `word1 = count (4)`;
the 4-entry section directory is the LAST 16 bytes (at `dir_offset`). Sections:

| dir[] | contents | size (W03 ex.) |
|---|---|---|
| **[0] EDD** | the weapon animation (14-clip idle+stance set; clip 2 = the idle). KEYFRAME-only twin lives here too. | 1208–1428 B |
| **[1] EMR** | KEYFRAME-ONLY (8-byte header + N×80 keyframes, NO bone table — shares the base PLD skeleton). | ~19.5 KB |
| **[2] MD1** | the **hand+gun mesh** (obj_count 2 → 1 mesh; ~35 verts = the plain hand's ~23 verts REPLACED by a hand-gripping-the-gun mesh). | 2460 B |
| **[3] TIM** | the weapon's own gun texture (e.g. 28×32 8bpp, CLUT 256×1). See §4 — this is NOT sampled as a standalone; it belongs at a VRAM page the mesh UVs point into. | 2336 B |

Extract sections by slicing at the dir offsets (see `pselect_load_model` / the extractor in
`.claude/skills/re15-savestate-ghidra/scripts/` calibration). `re15_md1_parse(plw+dir[2], …)` gives
the mesh; `re15_emd_parse_skeleton` on dir[1] is keyframe-only (combine with the base PLD EMR — see
[[reai-v2-model-anim-groundtruth]]).

## 2. Attach: the weapon REPLACES the hand mesh at bone 11

The model draws through ONE shared skeleton-part pass (`FUN_8001e8c8`: walks `model+0x188` = parts
array, `model+0x83` = part count, stride **0xAC**, per-part composed matrix @**+0x40** (rot 9×s16
GTE /4096) with translation @**+0x54** (= matrix+0x14)). There is **no separate weapon-draw call** —
when a weapon is equipped the game **patches part[11]** (the right-hand part) to point at the weapon
MD1's sub-objects, so the hand mesh is *replaced* by the hand+gun mesh and renders inline with bone
11's transform. (Left hand = part[14]; both plain hands are ~22–23 verts, the weapon part is ~35.)

Verify per character from a savestate: read `parts=model+0x188`, `count=model+0x83`; every part[i]'s
tri coord-header (counts vtx/nrm/pri) equals the base MD1 mesh[i] EXCEPT part[11], which carries the
weapon's counts. Only the entities that HAVE a weapon equipped show the weapon part.

**Port**: at bone 11 draw the weapon MD1 (`wpn_md1.meshes[0]`) instead of body `mesh[11]`, using
bone 11's already-composed matrix. The port's bone-11 world matrix is byte-identical to the
original's part[11]@+0x40 (verified: rot `[2 204 4088]/[523 4048 -205]/[-4060 522 -18]`, trans
`(-2118,385,-66)`). No special weapon matrix needed — bone 11's pose matrix IS it.

## 3. Which mesh/texture: read it, don't assume

Byte-match the weapon mesh from the savestate: read part[11]'s vtx pointer (coord-header +0) and
compare the s16 (x,y,z,pad) vertices to `PL<char>W<weap>.PLW dir[2]`. **Load-order gotcha
(verified):** the player-select shares ONE weapon buffer — with two characters loaded, the LAST
one's weapon (PL04W03) wins, so BOTH characters' part[11] verts matched **PL04W03** 6/6 (Leon's own
PL00W03 did NOT). Always confirm against the RAM, not the "expected" file.

## 4. Texture: from the player-skin TIM's gun-art page, NOT the PLW dir[3] TIM directly

**This is where a naive port breaks.** Every tri/quad of the weapon mesh reads the SAME tpage/clut
as the body's hand mesh — e.g. `page=0x81` (tpage X=1) / `clut=0x7840` (→ CLUT row 1). Those UVs
(u 74–127 → +tpage pxo 128 = 202–255; v 125–239) point into the **player skin TIM** (`PL<char>.TIM`,
here 384×256 8bpp, 3 CLUT rows) where the **gun art is baked in** (decode `PL04.TIM` clut-row 1: the
gun texels sit at u 202–255 / v 125–239). The PLW's own `dir[3]` 28×32 TIM is the SOURCE of that gun
art — in the console it is DMA'd into VRAM at the weapon-texture page; the skin TIM the mesh samples
already contains it. So:
- Do **not** bind the tiny `dir[3]` TIM as a standalone slot — its UVs (126,145 with pxo 128 = 254)
  fall far outside a 56×32 texture → untextured/garbage.
- **Do** bind the skin TIM that has the gun art at the mesh's tpage. In the player-select that is
  **PL04.TIM (slot 21)** for BOTH characters (shared PL04 weapon), NOT each character's own body TIM.
- CLUT-row selection is `clut_y = (clut>>6)&0x1ff; idx = clut_y - tim.clut_y` — 0x7840 → row 1.

### ⚠️ In-game weapon SWAP (the real generalization — still to wire)
In-game the equipped weapon changes (`PL00W00`=unarmed, `W01`=knife, `W03`=handgun, …). Each PLW
carries its OWN `dir[2]` mesh + `dir[3]` gun texture. To render a swap byte-true the port must, on
equip: (a) swap part[11] to the new PLW's `dir[2]` mesh, (b) get the new PLW's `dir[3]` gun texture
into the VRAM/skin page the mesh's tpage (0x81 region) samples — i.e. **composite dir[3] into the
skin TIM's weapon-art rect**, not bind it as its own slot. The current in-game weapon-in-hand
(`main.c` ~3609, RE15_TIM_SLOT_WPN_GUN 25) is a *faithful-line* that binds dir[3] standalone and is
therefore the same texture bug — fix it with this same page-composite approach. (Port texture model
is per-slot standalone textures + a `pxo=(page&0xF)*128` page shift + CLUT-row stacking; it is a
partial VRAM atlas, so multi-page / gun-into-skin cases need an explicit composite.)

## 5. Method checklist (any character × any weapon)
1. Savestate of the original with the weapon equipped → `re15_entity_assets.py <sav> <entity>` for
   clip + EDD/EMR/mesh pointers; read `parts@+0x188`, `count@+0x83`, part[11] matrix @+0x40 / verts.
2. Byte-match part[11] verts → the exact `PL*W*.PLW dir[2]`. Byte-match the anim/skeleton (§1 + the
   base-EMR combine). Confirm the attach bone (11) via the part-count anomaly + matrix match.
3. Read the weapon tris' `page`/`clut` (port `re15_md1` parse) → decode `PL<char>.TIM` at that
   tpage/clut-row and confirm the gun art is there. Bind THAT skin TIM for the weapon mesh.
4. For a swap: composite the equipped PLW's dir[3] into the skin's weapon rect before drawing.
5. Verify visually AND cite the bytes for every claim (bone, matrix, verts, tpage) — the mechanism,
   not just the screenshot ([[disasm-verify-decompiles]], [[reai-v2-parity-oracle]]).
