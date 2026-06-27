/*
 * RE1.5 Save FSM — RE2-faithful C reimplementation (Phase 3.2).
 *
 * Status: STUB. Phase 3.1 (foundation) sets up the headers + build infra.
 * Phase 3.2 (next session) fills in the save logic.
 *
 * Architecture:
 *
 *   RE1.5 save trigger (AOT bit 18 of G_SAVE_TRIGGER_WORD)
 *     ↓
 *   FUN_8001C958 clears bit 18, calls save driver at 0x80025C00
 *     ↓
 *   Save driver eventually calls FUN_80046540 (RE1.5's broken save FSM)
 *     ↓
 *   ┌──────────────────────────────────────────────────────────┐
 *   │ PHASE 3.2 GOAL: skip FUN_80046540 entirely.              │
 *   │ Replace with pb_save_main() — a clean, RE2-faithful save │
 *   │ flow that touches NO audio state.                        │
 *   └──────────────────────────────────────────────────────────┘
 *
 * Hook strategy (Phase 3.2):
 *   - At FUN_80046540 entry (0x80046540): redirect to pb_save_main()
 *   - pb_save_main runs to completion (multi-frame state machine in C)
 *   - On exit, return cleanly to FUN_80046540's caller
 *
 * What pb_save_main does:
 *   1. Initialize card if needed (sys_bu_init)
 *   2. Display save dialog UI (reuse RE1.5's text rendering)
 *   3. Wait for confirm/cancel
 *   4. On confirm: serialize GSB + write to card via sys_FileWrite
 *   5. Display result (success/fail)
 *   6. Return to gameplay
 *
 * What pb_save_main does NOT do (vs RE1.5 native):
 *   - NO call to model_setup (FUN_80043D8C) — that's RE1.5's broken audio reload
 *   - NO call to FUN_80021634 — clears libsnd VAB slots
 *   - NO call to FUN_80029C2C — clears voice descriptors
 *   - NO call to card_cleanup (RE1.5 0x80026594) — kills SPU voices
 *   - NO touch of G_LIBSND_PROG_ATR/VH/TONE_ATR
 *   - NO touch of G_LIBSND_VAB_USED
 *   - NO touch of G_BANK_BYTE_TABLE / G_SAMPLE_DESCRIPTOR_BASE
 *
 * Result: libsnd state survives save unchanged → no post-save freeze.
 */

#include "psx_types.h"
#include "re15_addrs.h"
#include "bios.h"
/* libsnd.h intentionally NOT included here — we don't call any of those. */

/* Entry point that replaces RE1.5's FUN_80046540 state-2 jal target.
 * MUST be first in this file so it lands at 0x801F0E00 (the jal target).
 * Currently a no-op (Phase 2.2 baseline). Phase 3.2 will switch this to
 * jal pb_save_main() once that function is implemented.
 */
__attribute__((noinline))
void pb_save_entry(void)
{
    /* No-op until Phase 3.2. */
}

/* Phase 3.1 stub — Phase 3.2 fills this in. */
__attribute__((noinline))
void pb_save_main(void)
{
    /* TODO Phase 3.2:
     *   - State machine in C
     *   - Card init + write
     *   - UI: reuse show_message / native text renderer
     *   - GSB serialization
     */
}
