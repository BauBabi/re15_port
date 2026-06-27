/*
 * RE1.5 Custom Save System — Phase 2.2 baseline (no-op).
 *
 * Replaces RE1.5's broken FUN_80046540 state-2 jal which corrupted SPU
 * sample RAM via garbage a1=0x80198000. Pure no-op: save FSM state-2 does
 * nothing. CDFIX is v1.24 PASS-THROUGH (no VAB wipe).
 *
 * Phase 2.3 snapshot/restore attempts were reverted 2026-05-18 — surgical
 * patching reached diminishing returns. Strategic pivot (Phase 3):
 * replace whole RE1.5 subsystems (save/load/voice/room) with RE2-faithful
 * C implementations compiled via PSn00bSDK.
 */

#include <stdint.h>

__attribute__((noinline))
void pb_save_entry(void)
{
    /* No-op. */
}
