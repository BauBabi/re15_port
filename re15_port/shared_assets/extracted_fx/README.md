# Global effect textures — byte-true VRAM extraction (RE1.5 CORE00.ESP bank)

The global effect bank `DATA/CORE00.ESP` holds effect ids **{3,8,0,2,4}** (loaded once at
game-init by the boot installer, param_4=0). Their **anim/coord records** are in CORE00.ESP;
their **textures** are NOT in any file — they live only in VRAM (uploaded at boot). We
recovered them byte-true from a live ROOM1140 frame via DuckStation `[Debug] ShowVRAM` (the
1024×512 view = VRAM 1:1), reconstructing the raw 15-bit halfwords from the PNG and decoding
the tpage/clut. Tool: `re15_port/tools/vram_png_to_tim.py` (or `.claude/skills/re15-room-capture/
scripts/re15_vram_extract.py`). Ground truth: `vram_view_ground_truth.png`.

## The map — 2 sprite sheets, recoloured per CLUT

The 5 global effects are only **TWO texture pages**, recoloured via different CLUTs:

| effect-id | word1 (runtime==file) | tpage → VRAM page | CLUT → VRAM | what it is |
|-----------|-----------------------|-------------------|-------------|------------|
| **0** | 0x001f**7951** | 0x001f → (960,256) 4bpp | 0x7951 → (272,485) | **dark blood** (hit splatter — the one wired in) |
| 2 | 0x001f**7a51** | 0x001f → (960,256) 4bpp | 0x7a51 → (272,489) | bright/fresh blood (same splatter sheet) |
| 4 | 0x001f**7ad1** | 0x001f → (960,256) 4bpp | 0x7ad1 → (272,491) | brown/dried blood (same sheet) |
| **8** | 0x001e**7911** | 0x001e → (896,256) 4bpp | 0x7911 → (272,484) | **fire / explosion / muzzle** (~340 spawns game-wide) |
| 3 | 0x001e**7811** | 0x001e → (896,256) 4bpp | 0x7811 → (272,480) | grey smoke / dust (same cloud sheet) |

- **Blood sheet @VRAM(960,256):** ~20 splatter frames + round pools/drops + streaks + a hit-marker.
- **Cloud sheet @VRAM(896,256):** puffy burst clouds + flickering flames (fire when orange, smoke when grey).

## Files here

- `effect0_blood.tim` — effect-0 as a byte-true 4bpp PSX TIM (CLUT[0]=0x0000). **This is the one
  wired into the port** (loaded at game-init → `RE15_TIM_SLOT_EFFECT_GLOBAL` slot 20; the effect
  slots treat index-0 as transparent, see render_pc.c). Renders on every zombie hit.
- `effect0_blood.png` … `effect8_fire.png`, `effect2_blood_bright.png`, `effect3_smoke.png`,
  `effect4_blood_brown.png` — human-readable REFERENCE decodes of each variant.
- `vram_view_ground_truth.png` — the full 1024×512 VRAM (source for all of the above; also holds
  the room/model textures, framebuffers, etc.).

## Status / TODO

- effect-0 (blood): extracted + integrated + rendered (byte-true). **Done.**
- effect-8 (fire/muzzle) and 2/3/4: extracted + mapped, NOT yet integrated — the port has no
  spawner for them yet (they're spawned by overlay behaviours, e.g. FUN_80100688/8010ab2c/
  80114ba4 in STAGE1, not the ROOM1140 zombie-hit path). Integrate their TIMs + a per-effect-id
  slot map when those spawners are ported.
- The ORIGINAL global-effect-TIM upload (the boot installer that puts these two sheets in VRAM)
  is not in the decompiled set → boot-init disasm would find the authoritative source file; these
  extracts are the byte-true VRAM-recovered stand-in until then.
