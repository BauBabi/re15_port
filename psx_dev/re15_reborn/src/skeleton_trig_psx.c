/*
 * RE1.5 Rebuilt — Skeleton trig, PSX backend (Phase 4.5.7.2, 2026-05-18).
 *
 * Thin wrapper around PSn00bSDK's isin()/icos() — same Q12 output, no
 * extra computation. Used by skeleton_common.c's matrix builders.
 */
#include <psxgte.h>
#include "re15_skeleton.h"

int re15_sin_q12(int angle_4096) { return isin(angle_4096); }
int re15_cos_q12(int angle_4096) { return icos(angle_4096); }
