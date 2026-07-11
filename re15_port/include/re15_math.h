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

#endif /* RE15_MATH_H */
