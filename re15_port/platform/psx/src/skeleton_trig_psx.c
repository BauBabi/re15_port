/*
 * RE1.5 Rebuilt — Skeleton trig, PSX backend.
 *
 * SUPERSEDED (byte-true audit #2, 2026-07-07): re15_sin_q12 / re15_cos_q12 now come from the
 * shared, byte-true DAT_800794c4 table in engine/src/re15_trig_lut.c (extracted verbatim from
 * PSX.EXE). PSn00bSDK's isin/icos is a polynomial approximation that need not match the original
 * game's own table; the LUT is what the RE1.5 render math (RotMatrix @0x80068130, camera) actually
 * reads, so both platforms use it. This translation unit is intentionally empty now.
 */
