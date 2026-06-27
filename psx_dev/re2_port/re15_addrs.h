/*
 * RE1.5 RAM map and key game-state addresses (Phase 3, 2026-05-18).
 *
 * Derived from forensic analysis across multiple sessions, verified by
 * 20-agent multi-angle audit (2026-05-18).
 *
 * EXE load base: 0x80010000. File header: 0x800 (so file_off = ram - 0x80010000 + 0x800).
 * RAM range: 0x80000000..0x80200000 (2MB).
 */
#ifndef RE15_ADDRS_H
#define RE15_ADDRS_H

#include "psx_types.h"

/* =========================================================================
 * Save / Load
 * ========================================================================= */

/* Save-trigger flag word. Bit 18 = save requested (set by AOT_TYPE1_HOOK,
 * consumed in FUN_8001C958 at ~0x8001CB80 before jal to save driver). */
#define G_SAVE_TRIGGER_WORD     0x800ACA38
#define G_SAVE_TRIGGER_BIT_18   (1u << 18)

/* Save / inventory / menu mode byte. State machine driven by FUN_80046540.
 * NOT save-specific — also used by inventory + other modal UIs. */
#define G_MODE_BYTE             0x800B25BF

/* Save FSM internal state byte. Dispatched via table @ 0x80074C00. */
#define G_SAVE_FSM_STATE        0x800B25C0

/* Save FSM auxiliary state bytes. Cleared at FSM exit (0x800464E8..0x8004676C). */
#define G_SAVE_FSM_STATE_AUX0   0x800B25C1
#define G_SAVE_FSM_STATE_AUX1   0x800B25C2
#define G_SAVE_FSM_STATE_AUX2   0x800B25C3

/* Save data persistence */
#define G_SAVE_COUNT            0x800B0FBD   /* incremented on each save */
#define G_GSB_START             0x800B0FBC   /* Game State Block, ~10 bytes header */

/* =========================================================================
 * Weapon / Inventory
 * ========================================================================= */

#define G_EQUIPPED_SLOT_INDEX   0x800B25C8   /* current weapon slot */
#define G_ENTITY_WEAPON_TYPE    0x800ACA5D   /* weapon type byte for entity */
#define G_WEAPON_SLOT_STASH     0x8007F0C4   /* stashed pre-save weapon slot */
#define G_WEAPON_TYPE_STASH     0x8007F0C5   /* stashed pre-save weapon type */

/* =========================================================================
 * libsnd internals (RE1.5 RAM-resident state)
 * AA16 critical trio per 20-agent analysis.
 * ========================================================================= */

/* libsnd cached pointers — written by sub_8005AA60 (VH-parse helper at libsnd
 * VAB-open path). Read by SsUtKeyOnV during voice key-on. Stale values cause
 * post-save shotgun-fire freeze. */
#define G_LIBSND_PROG_ATR       0x800B2B28   /* program-attribute table base */
#define G_LIBSND_VH             0x800B2B34   /* VH header pointer */
#define G_LIBSND_TONE_ATR       0x800B2B3C   /* tone-attribute table base (85 readers) */

/* libsnd VAB load-state array. Each byte = state[i] for VAB slot i.
 * state == 1 = LOADED (required by SsUtKeyOnV's bne v1,1 gate).
 * WIPING THIS WAS THE v1.23 BUG — v1.24 PASS-THROUGH CDFIX keeps it intact. */
#define G_LIBSND_VAB_USED       0x800B5334   /* 16 bytes, slots 0..15 */

/* libsnd sequencer / VAB-open re-entry busy flag. */
#define G_LIBSND_SEQ_BUSY       0x800B283C

/* libsnd voice / VAB descriptor area. CDFIX previously memcpy'd 208B here
 * from PRESAVE_AUDIO_BUFFER (room-specific snapshot) — that workaround was
 * also retired. */
#define G_LIBSND_DESCRIPTORS    0x800B2200   /* 208 bytes through 0x800B22D0 */

/* Bank byte table — indexed by SE bank ID (per SE_play / weapon-fire dispatch).
 * Slot 1 = bank=1 (cursor/ambient?), slot 3 = bank=3 (ARMS = weapon fire).
 * Slot[1] uninit at cold boot is the knife-sound bug. */
#define G_BANK_BYTE_TABLE       0x800B21EC   /* 8 bytes, banks 0..7 */

/* Sample descriptor base pointer — read by SE_play fall-through at
 * 0x80045108. If uninit, plays sample[0] (= knife). */
#define G_SAMPLE_DESCRIPTOR_BASE 0x800AC778

/* Canonical VAB scratch buffer pointer, set by game init paths.
 * Used by model_setup (FUN_80043D8C) and FUN314B0_WRAPPER for VAB loads. */
#define G_CANONICAL_SCRATCH_PTR 0x800AC77C

/* libsnd program count (number of programs in current VAB). */
#define G_LIBSND_PROGRAM_COUNT  0x800B284C   /* 2 bytes */

/* libsnd reentry-lock byte. Cleared by card_cleanup's `sb zero, 0x283C(t0)`. */
#define G_LIBSND_LOCK           0x800B283C

/* AA18 fade state (cleared by save EXIT in v1.25 plan). */
#define G_LIBSND_FADE_STATE     0x800B5218
#define G_LIBSND_FADE_COUNTER   0x800B5360

/* =========================================================================
 * Game state / Rendering
 * ========================================================================= */

#define G_CAM_DIRTY             0x800B5457
#define G_CURRENT_CAM_ID        0x800BFBB5
#define G_CUR_CAM_IDX           0x800B0FE4
#define G_PREV_CAM_IDX          0x800B0FE8
#define G_CURRENT_ROOM_ID       0x800B0FE2
#define G_GPU_DMA_STATE         0x800ACA38

/* =========================================================================
 * PATCH.BIN memory layout (loaded by PATCH_LOADER to 0x801F0000)
 * ========================================================================= */

#define PATCH_BIN_BASE          0x801F0000
#define PB_SAVE_ENTRY_RAM       0x801F0E00   /* our state-2 hook target */
#define PATCH2_BIN_BASE         0x801F1000

/* Cave allocations in PSX.EXE space (BSS region, never overwritten by overlays) */
#define CAVE_0_BASE             0x80070340   /* 236B */
#define CAVE_2_BASE             0x800719D4   /* 768B */
#define CAVE_3_BASE             0x8007153C   /* 736B */

#endif
