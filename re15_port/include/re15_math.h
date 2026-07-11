/*
 * re15_math.h — byte-true PSX math primitives shared across subsystems.
 *
 * The RE1.5 engine has NO exact sqrt: the PsyQ BIOS/GTE only provides the
 * table-based approximation `SquareRoot0` (PSX.EXE 0x80065f60), and the game
 * calls it for EVERY distance/length: collision circle push-out, enemy AI
 * player-distance, camera LookAt, character-shadow yaw, and lighting.
 *
 * A naive port that substitutes an exact floor(sqrt) is NOT byte-true — the
 * BIOS routine systematically UNDER-estimates floor(sqrt) (it normalizes the
 * input by an EVEN leading-zero count, indexes a 192-entry Q12 mantissa table,
 * then denormalizes), so its result diverges from the exact value on ~92% of
 * inputs (e.g. SquareRoot0(289)=16, not 17; SquareRoot0(10000)=99, not 100).
 * That ±(1..N) delta reaches AI threshold comparisons and committed positions.
 *
 * `re15_squareroot0` reproduces the exact BIOS algorithm bit-for-bit so both
 * the PC and PSX builds compute identical distances.
 */
#ifndef RE15_MATH_H
#define RE15_MATH_H

#include <stdint.h>

/* Byte-true replica of PsyQ BIOS SquareRoot0 (PSX.EXE 0x80065f60).
 * Input: a 32-bit unsigned value (the PSX passes the low 32 bits of the mult).
 * Output: the table-approximated integer square root (NOT exact floor(sqrt)). */
uint32_t re15_squareroot0(uint32_t x);

/* Byte-true PsyQ BIOS trig. Angle unit 4096 = 360deg (Q12); results Q12 (4096 = 1.0).
 * These are NOT the game's rotation trig (re15_sin_q12/cos_q12 / re15_atan2_q12): those use
 * a DIFFERENT table (DAT_800794c4) that diverges by up to ~9 Q12 off-axis, and the game atan2
 * uses a quarter-resolution table + the 0=+Z convention. These reproduce the BIOS functions the
 * body-push anisotropic ellipse (FUN_8002aec4) calls.
 *   rsin   @0x800683e8 (core 0x80068424, table 0x8007db04)
 *   rcos   @0x80068348  (rcos(a) = rsin(|a| + 90deg))
 *   ratan2 @0x80065de0 (table 0x80078cc0): angle of (x,y) CCW from +X, 0..4095. */
int32_t re15_rsin(int32_t a);
int32_t re15_rcos(int32_t a);
int32_t re15_ratan2(int32_t y, int32_t x);

#endif /* RE15_MATH_H */
