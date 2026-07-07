/*
 * RE1.5 Rebuilt — Skeleton trig, PC backend.
 *
 * SUPERSEDED (byte-true audit #2, 2026-07-07): re15_sin_q12 / re15_cos_q12 now come from the
 * shared, byte-true DAT_800794c4 table in engine/src/re15_trig_lut.c (extracted verbatim from
 * PSX.EXE). The old sinf/cosf + (int)(x*4096) truncation here diverged from that table on
 * 2008/4096 angles (round vs trunc) and fed the reported pixel-shift. This translation unit is
 * intentionally empty now (both platforms + the tests link the engine LUT). Kept as a file so the
 * platform CMake glob and any historical references stay valid.
 */
