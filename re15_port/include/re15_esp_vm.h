/**
 * @file re15_esp_vm.h
 * @brief RE1.5 effect-script VM — the third ESP layer (Phase ESP-C1: pool + dispatch loop).
 *
 * ESP rendering is NOT a sprite quad — it is a dedicated bytecode VM, the effect analog of the
 * SCD VM. The full three-layer chain (byte-true mapped 2026-06-30, see HANDOVER "ESP-SPRITE-
 * SUBSYSTEM"):
 *   1. effect pool   DAT_800b2360/2368  (ESP-B, re15_esp.c) — AABB-cull walker FUN_8004d5f0 ->
 *                    handler_table[type] @0x80074c68; type-3 @0x8004d728 -> FUN_8003ee3c(a,id).
 *   2. allocator     FUN_8003ee3c  -> FUN_8003edec : claims a slot in the 0x170-instance pool
 *                    DAT_800b2b4c (10 instances x 0x170) and seeds its bytecode pc.
 *   3. per-frame VM  FUN_8003f0a0 : for each active instance, runs an EFFECT-SCRIPT VM
 *                    `(*PTR_800744a8[*pc])(instance)` — opcode table @0x800744a8 (52+ ops; one
 *                    draw-op = the 40x30 SPRT builder FUN_80046a1c).
 *
 * Phase ESP-C1 delivered the instance-pool struct, the allocator/init, and the FUN_8003f0a0
 * dispatch loop (byte-true, disasm-verified). Phase ESP-C2 ports the ~52 opcode handlers
 * @0x800744a8 one by one. Done so far (C2 batches 1+2, the control-flow + loop-counter core,
 * disasm-verified @0x8003f1c0..f53c):
 *   0x00/0x1c/0x1d-0x20 nop-continue · 0x01 end/return-level · 0x02 wait-1-frame ·
 *   0x03 jump-script · 0x04 spawn · 0x05 kill-instance · 0x06 loop-push · 0x07 loop-jump ·
 *   0x08 loop-pop · 0x09 counter-set · 0x0a counter-wait · 0x0b counter-push · 0x0c wait-cond ·
 *   0x0d FOR-begin · 0x0e NEXT · 0x10 loop-back · 0x11 FOR-begin(no count) · 0x14 skip6 ·
 *   0x15 skip2 · 0x16 pop-ci · 0x17 block-setup · 0x19 pop-level · 0x1a FOR-break ·
 *   0x2f set-u16 · 0x31 integrator · 0x1b FOR-begin(reg) · 0x23 compare · 0x24 reg-set ·
 *   0x25 reg-copy · 0x26 reg-arith-imm · 0x27 reg-arith-reg.  (0x09+0x0a = "wait N frames";
 *   0x0d..0x1a = the 2nd FOR-loop subsystem; 0x1b/0x23..0x27 = the DAT_800b0fd0 register file.)
 * Full opcode classification: RE15_ESP_VM_OPCODES.md (95 ops). Other opcodes = stub. Still ahead:
 * the work-struct (+0x154) / effect-pool / draw-GTE / game-flag / audio ops, then the bytecode
 * source (DAT_800b3f70) + TIM/UV LUTs (Phase ESP-C3). No GPU draw, no game_step wiring yet.
 *
 * Pointer handling: the original instance stores a 32-bit absolute pc at +0x1c and 32-bit return
 * pointers on the +0xc0 sub-stack. The PC port keeps the 0x170-byte instance image byte-true but
 * stores the pc and the stack entries as 32-bit OFFSETS into a separately-held bytecode base
 * (`code_base`) — same field sizes, same slots, faithful behaviour.
 */
#ifndef RE15_ESP_VM_H
#define RE15_ESP_VM_H

#include <stdint.h>
#include <stddef.h>

#define RE15_ESPVM_INSTANCES   10     /* FUN_8003f0a0: `do { ... } while (uVar4 < 10)`           */
#define RE15_ESPVM_STRIDE      0x170  /* per-instance image size (iVar3 += 0x170)                */
#define RE15_ESPVM_OPCODES     256    /* byte-indexed dispatch (*pc is u8); table @0x800744a8    */

/* Byte-true instance field offsets (from FUN_8003f0a0 disasm + FUN_8003edec init). */
#define RE15_ESPVM_OFF_OP0     0x00   /* cleared by init                                         */
#define RE15_ESPVM_OFF_ACTIVE  0x01   /* lbu 1(s0): active flag (0 = skip instance)              */
#define RE15_ESPVM_OFF_LEVEL   0x02   /* lb 2(s0): (s8) level/track index                        */
#define RE15_ESPVM_OFF_DEPTH0  0x04   /* loop-stack depth for level 0 (inst[level+4]; init 0xff) */
#define RE15_ESPVM_OFF_CIDX0   0x08   /* loop-counter index for level 0 (inst[level+8]; init 0xff)*/
#define RE15_ESPVM_OFF_DEPTHARR 0x0c  /* FOR-loop saved-depth array (inst + level*4 + 0x0c + ci) */
#define RE15_ESPVM_OFF_PC      0x1c   /* lw 28(s0): program counter (port: u32 offset)           */
#define RE15_ESPVM_OFF_PCARR   0x20   /* FOR-loop body-start array (inst + level*0x10 + 0x20 + ci*4)*/
#define RE15_ESPVM_OFF_TGTARR  0x60   /* FOR-loop end-target array (inst + level*0x10 + 0x60 + ci*4)*/
#define RE15_ESPVM_OFF_CARRAY  0xa0   /* per-level loop-counter array (inst + level*8 + 0xa0 + ci*2)*/
#define RE15_ESPVM_OFF_STACK   0xc0   /* loop-return stack base (instance + level*0x20 + 0xc0)   */
#define RE15_ESPVM_OFF_SP      0x140  /* lw 320(s0): stack pointer (port: u32 offset into mem)   */
#define RE15_ESPVM_OFF_RETPC   0x144  /* per-level saved pc array (inst + 0x144 + level*4)       */

/** Opcode handler return codes (FUN_8003f0a0 dispatch). */
enum {
    RE15_ESPVM_RET_YIELD = 0,   /* return/yield: pop the sub-stack (or end if depth < 0)         */
    RE15_ESPVM_RET_CONT  = 1,   /* keep executing the next opcode in the same frame             */
    RE15_ESPVM_RET_STOP  = 2    /* instance done for this frame                                 */
};

/** One effect-VM instance (the 0x170-byte image, plus the port-side bytecode base). */
typedef struct {
    uint8_t        mem[RE15_ESPVM_STRIDE];  /* byte-true instance image (offsets above)          */
    const uint8_t *code_base;               /* DAT_800b3f70 analog (the bytecode blob base)      */
} re15_espvm_instance_t;

/** An opcode handler: reads/advances `inst` and returns one of RE15_ESPVM_RET_*. */
typedef int (*re15_espvm_op_fn)(re15_espvm_instance_t *inst);

/* ---- pool / lifecycle --------------------------------------------------------------------- */

/** Clear the whole instance pool and reset the opcode table to the C1 stub (returns STOP).
 *  (The pool clear is the room/per-frame setup; the stub table is C1 scaffolding for C2.) */
void re15_espvm_reset(void);

/** FUN_8003ee3c (in-game path, DAT_800b3f7a == 0) + FUN_8003edec init.
 *  Claims instance `slot_index` (>=10 falls back to slot 2), zeroes its scratch, and seeds the
 *  pc from the bytecode offset table: pc = u16le(code_base + bytecode_id*2). Returns the
 *  instance, or NULL on bad args. */
re15_espvm_instance_t *re15_espvm_alloc(uint8_t slot_index, uint16_t bytecode_id,
                                        const uint8_t *code_base);

/** FUN_8003f0a0: per-frame walker over all 10 instances. For each active one, dispatch
 *  `optable[*pc](inst)` until the opcode returns STOP/YIELD, honouring the sub-return stack.
 *  Returns the number of active instances ticked this frame. */
int re15_espvm_run_all(void);

/* ---- opcode table (C2 hook) --------------------------------------------------------------- */

/** Install an opcode handler at index `op` (C2 ports the real @0x800744a8 entries here; tests
 *  install synthetic handlers to exercise the dispatch loop). */
void re15_espvm_set_opcode(uint8_t op, re15_espvm_op_fn fn);

/** Opcode 0x0c (wait-condition) gate. Byte-true the original waits while
 *  (*(u32*)0x800aca3c & 0x20) || (*(u8*)0x800b8520 & 0x80) — a global pause/freeze the port does
 *  not model yet. Default 0 (normal play: op 0x0c advances). Set nonzero to make op 0x0c wait. */
void re15_espvm_set_wait_gate(int waiting);

/** The VM global register file (DAT_800b0fd0): 256 u16 variables shared by all instances, used by
 *  opcodes 0x1b/0x23/0x24/0x25/0x26/0x27. Zeroed by re15_espvm_reset. */
uint16_t re15_espvm_reg_get(int idx);              /* idx in [0,256) */
void     re15_espvm_reg_set(int idx, uint16_t v);

/* ---- accessors (inspection / tests) ------------------------------------------------------- */

re15_espvm_instance_t *re15_espvm_instance(int i);   /* i in [0,10); NULL otherwise */
int                    re15_espvm_active_count(void);

/* byte-true little-endian field access into the instance image. */
uint32_t re15_espvm_get_pc(const re15_espvm_instance_t *inst);
void     re15_espvm_set_pc(re15_espvm_instance_t *inst, uint32_t pc_off);

#endif /* RE15_ESP_VM_H */
