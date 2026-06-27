/*
 * PSX basic types for RE1.5 RE2-faithful port (Phase 3, 2026-05-18).
 *
 * MIPS R3000A, little-endian, 32-bit.
 */
#ifndef PSX_TYPES_H
#define PSX_TYPES_H

#include <stdint.h>

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;

/* Pointer-sized integers for raw memory access. */
#define MEM_B(addr)  (*(volatile u8  *)(addr))
#define MEM_H(addr)  (*(volatile u16 *)(addr))
#define MEM_W(addr)  (*(volatile u32 *)(addr))

#endif
