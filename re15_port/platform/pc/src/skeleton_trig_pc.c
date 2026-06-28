/*
 * RE1.5 Rebuilt — Skeleton trig, PC backend (Phase 4.5.7.2, 2026-05-18).
 *
 * sinf/cosf-based equivalent of PSn00bSDK's isin/icos. Converts the
 * PSX 4096-unit-per-turn angle representation to radians, computes
 * sin/cos as float, then scales back to Q12 fixed-point.
 *
 * Output sign convention matches PSn00bSDK isin: positive sine for
 * angles 0..1024 (= 0..π/2), etc.
 */
#include <math.h>
#include "re15_skeleton.h"

/* 2*pi / 4096 = π / 2048 = 0.001533981f */
#define PSX_ANGLE_TO_RAD   (3.14159265358979f / 2048.0f)

int re15_sin_q12(int angle_4096)
{
    float rad = (float)angle_4096 * PSX_ANGLE_TO_RAD;
    float s   = sinf(rad);
    return (int)(s * 4096.0f);
}

int re15_cos_q12(int angle_4096)
{
    float rad = (float)angle_4096 * PSX_ANGLE_TO_RAD;
    float c   = cosf(rad);
    return (int)(c * 4096.0f);
}
