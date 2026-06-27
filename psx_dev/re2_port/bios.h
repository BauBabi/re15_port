/*
 * PSX BIOS syscall wrappers (Phase 3, 2026-05-18).
 *
 * BIOS dispatch tables on PSX:
 *   A(0x...) — 0xA0 vector
 *   B(0x...) — 0xB0 vector
 *   C(0x...) — 0xC0 vector
 *
 * Convention: load t1 with function index, t2 with vector address (0xA0/0xB0/0xC0),
 * jr t2. Args are in a0..a3 per usual MIPS convention.
 *
 * We declare wrappers as `static inline` so each call expands to a small
 * jal-via-pointer sequence that gcc can schedule cleanly.
 *
 * Card I/O functions (used by save/load):
 *   _bu_init             — B(0x4A) — initialize card subsystem
 *   InitCARD             — wrapper for A(0xAB) likely, used at boot
 *   FileOpen             — B(0x42) for memcard files (uses path like "bu00:BAS"")
 *   FileWrite            — B(0x44)
 *   FileRead             — B(0x46)? (varies)
 *   _card_status         — B(0x4D)
 *   _card_wait           — B(0x4F)
 *
 * For now, we declare prototypes for what RE2 uses. Implementation comes in Phase 3.2.
 */
#ifndef BIOS_H
#define BIOS_H

#include "psx_types.h"

/* BIOS syscall via jr t2. Used inline. */
static inline int bios_b(int func, int a0, int a1, int a2, int a3)
{
    register int v0 asm("v0");
    register int arg0 asm("a0") = a0;
    register int arg1 asm("a1") = a1;
    register int arg2 asm("a2") = a2;
    register int arg3 asm("a3") = a3;
    register int t1_func asm("t1") = func;
    asm volatile (
        "lui $t2, 0\n"
        "ori $t2, $t2, 0xB0\n"
        "jalr $t2\n"
        "nop\n"
        : "=r"(v0)
        : "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3), "r"(t1_func)
        : "ra", "t2", "memory"
    );
    return v0;
}

/* Card I/O — referenced by RE2 save flow */
static inline int sys_bu_init(void)               { return bios_b(0x4A, 0, 0, 0, 0); }
static inline int sys_card_status(int port)       { return bios_b(0x4D, port, 0, 0, 0); }
static inline int sys_card_wait(int port)         { return bios_b(0x4F, port, 0, 0, 0); }

/* File operations on memcard */
static inline int sys_FileOpen(const char *path, int mode)
{
    return bios_b(0x32, (int)path, mode, 0, 0);
}

static inline int sys_FileClose(int fd)           { return bios_b(0x33, fd, 0, 0, 0); }
static inline int sys_FileRead(int fd, void *buf, int n)
{
    return bios_b(0x34, fd, (int)buf, n, 0);
}
static inline int sys_FileWrite(int fd, const void *buf, int n)
{
    return bios_b(0x35, fd, (int)buf, n, 0);
}

#endif
