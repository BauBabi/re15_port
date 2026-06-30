/**
 * @file re15_esp_vm.c
 * @brief RE1.5 effect-script VM (Phase ESP-C1) — 0x170-instance pool + FUN_8003f0a0 dispatch loop.
 *
 * Byte-true port of the third ESP layer (disasm-verified against PSX.EXE @0x8003edec/ee3c/f0a0,
 * 2026-06-30). See re15_esp_vm.h for the architecture. C1 = the VM infrastructure; the opcode
 * handlers are stubs (C2) and there is no GPU draw / game_step wiring (C3) yet.
 */
#include "re15_esp_vm.h"
#include <string.h>

/* The 0x170-instance pool (DAT_800b2b4c; 10 instances). The original packs the menu pool into the
 * same array (slots 10..13, DAT_800b3f7a==1 path); the port models the in-game pool (slots 0..9). */
static re15_espvm_instance_t s_pool[RE15_ESPVM_INSTANCES];

/* The opcode table (PTR_800744a8). C1 fills every entry with a stub; C2 installs the real ops. */
static re15_espvm_op_fn s_optable[RE15_ESPVM_OPCODES];

/* ---- byte-true little-endian field access ------------------------------------------------- */

static uint16_t rd_u16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}
static uint32_t rd_u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static void wr_u16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
}
static void wr_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
}

/* op 0x0c wait-condition gate (the port's stand-in for the FUN_8004efe4(0x800aca3c,0x1a) /
 * DAT_800b8520&0x80 global pause flags). Default 0 = normal play. */
static int s_espvm_wait_gate = 0;
void re15_espvm_set_wait_gate(int waiting) { s_espvm_wait_gate = waiting ? 1 : 0; }

/* The VM global register file DAT_800b0fd0: 256 u16 (byte-addressed, LE), shared by all instances. */
static uint8_t s_reg[256 * 2];
uint16_t re15_espvm_reg_get(int idx) { return (idx >= 0 && idx < 256) ? (uint16_t)(s_reg[idx*2] | (s_reg[idx*2+1] << 8)) : 0; }
void     re15_espvm_reg_set(int idx, uint16_t v) { if (idx >= 0 && idx < 256) { s_reg[idx*2] = (uint8_t)v; s_reg[idx*2+1] = (uint8_t)(v >> 8); } }

uint32_t re15_espvm_get_pc(const re15_espvm_instance_t *inst)
{
    return rd_u32(inst->mem + RE15_ESPVM_OFF_PC);
}
void re15_espvm_set_pc(re15_espvm_instance_t *inst, uint32_t pc_off)
{
    wr_u32(inst->mem + RE15_ESPVM_OFF_PC, pc_off);
}

/* ---- the C1 stub opcode --------------------------------------------------------------------
 * Every opcode is this until C2 ports the real @0x800744a8 handlers. Returning STOP terminates
 * the instance's frame cleanly (no infinite loop, no spurious sub-stack pop). It does NOT pretend
 * to implement any opcode semantics — that is exactly what Phase ESP-C2 will replace. */
static int re15_espvm_op_stub(re15_espvm_instance_t *inst)
{
    (void)inst;
    return RE15_ESPVM_RET_STOP;
}

void re15_espvm_set_opcode(uint8_t op, re15_espvm_op_fn fn)
{
    s_optable[op] = fn ? fn : re15_espvm_op_stub;
}

/* The ported opcode handlers (defined below, after the allocator/init they depend on). */
static int op_nop(re15_espvm_instance_t *inst);
static int op_end(re15_espvm_instance_t *inst);
static int op_wait(re15_espvm_instance_t *inst);
static int op_jump_script(re15_espvm_instance_t *inst);
static int op_spawn(re15_espvm_instance_t *inst);
static int op_kill(re15_espvm_instance_t *inst);
static int op_loop_push(re15_espvm_instance_t *inst);
static int op_loop_jump(re15_espvm_instance_t *inst);
static int op_loop_pop(re15_espvm_instance_t *inst);
static int op_counter_set(re15_espvm_instance_t *inst);
static int op_counter_wait(re15_espvm_instance_t *inst);
static int op_counter_push(re15_espvm_instance_t *inst);
static int op_wait_cond(re15_espvm_instance_t *inst);
static int op_for_begin(re15_espvm_instance_t *inst);
static int op_for_begin_nc(re15_espvm_instance_t *inst);
static int op_for_next(re15_espvm_instance_t *inst);
static int op_for_loop(re15_espvm_instance_t *inst);
static int op_for_break(re15_espvm_instance_t *inst);
static int op_for_popci(re15_espvm_instance_t *inst);
static int op_skip6(re15_espvm_instance_t *inst);
static int op_skip2(re15_espvm_instance_t *inst);
static int op_block_setup(re15_espvm_instance_t *inst);
static int op_pop_level(re15_espvm_instance_t *inst);
static int op_set_u16(re15_espvm_instance_t *inst);
static int op_integrate(re15_espvm_instance_t *inst);
static int op_for_begin_reg(re15_espvm_instance_t *inst);
static int op_reg_compare(re15_espvm_instance_t *inst);
static int op_reg_set(re15_espvm_instance_t *inst);
static int op_reg_copy(re15_espvm_instance_t *inst);
static int op_reg_arith_imm(re15_espvm_instance_t *inst);
static int op_reg_arith_reg(re15_espvm_instance_t *inst);
static int op_gosub(re15_espvm_instance_t *inst);
static int op_if_begin(re15_espvm_instance_t *inst);
static int op_cond_chain(re15_espvm_instance_t *inst);
static int op_switch(re15_espvm_instance_t *inst);

/* Install the ported real opcode table (PTR_800744a8). Unported entries stay the stub. C2 grows
 * this as more @0x800744a8 handlers are reverse-engineered. NB: re15_espvm_reset() must call this
 * AFTER the stub-fill so the real ops overwrite the stubs. */
static void re15_espvm_install_ops(void)
{
    s_optable[0x00] = op_nop;          /* nop-continue (0x8003f1d8) */
    s_optable[0x01] = op_end;          /* end/return-level (0x8003f1f0) */
    s_optable[0x02] = op_wait;         /* wait-1-frame (0x8003f258) */
    s_optable[0x03] = op_jump_script;  /* jump-script (0x8003f270) */
    s_optable[0x04] = op_spawn;        /* spawn instance (0x8003f2a0) */
    s_optable[0x05] = op_kill;         /* kill instance (0x8003f2dc) */
    s_optable[0x06] = op_loop_push;    /* loop-push (0x8003f328) */
    s_optable[0x07] = op_loop_jump;    /* loop-jump (0x8003f368) */
    s_optable[0x08] = op_loop_pop;     /* loop-pop  (0x8003f3a4) */
    s_optable[0x09] = op_counter_set;  /* counter-set  (0x8003f3e0) */
    s_optable[0x0a] = op_counter_wait; /* counter-wait (0x8003f428) */
    s_optable[0x0b] = op_counter_push; /* counter-push (0x8003f490) */
    s_optable[0x0c] = op_wait_cond;    /* wait-condition (0x8003f4c4) */
    s_optable[0x0d] = op_for_begin;    /* FOR-begin w/ count (0x8003f540) */
    s_optable[0x0e] = op_for_next;     /* NEXT (0x8003f674) */
    s_optable[0x10] = op_for_loop;     /* loop-back (0x8003f878) */
    s_optable[0x11] = op_for_begin_nc; /* FOR-begin no-count (0x8003f8bc) */
    s_optable[0x0f] = op_if_begin;     /* IF/while block-begin + predicate chain (0x8003f6f4) */
    s_optable[0x12] = op_cond_chain;   /* compound IF (AND/OR predicate chain) (0x8003f930) */
    s_optable[0x13] = op_switch;       /* register switch/case (0x8003fa5c) */
    s_optable[0x14] = op_skip6;        /* pc+=6 (0x8003fb38) */
    s_optable[0x15] = op_skip2;        /* pc+=2 (0x8003fb50) */
    s_optable[0x16] = op_for_popci;    /* pop counter-index (0x8003fb68) */
    s_optable[0x17] = op_block_setup;  /* block setup (0x8003fb9c) */
    s_optable[0x18] = op_gosub;        /* GOSUB / push level (0x8003fbe8) */
    s_optable[0x19] = op_pop_level;    /* pop level (0x8003fc50) */
    s_optable[0x1a] = op_for_break;    /* FOR-break (0x8003fca8) */
    s_optable[0x2f] = op_set_u16;      /* set instance u16 field (0x80040f14) */
    s_optable[0x31] = op_integrate;    /* per-frame position integrator (0x80040fd4) */
    s_optable[0x1b] = op_for_begin_reg;/* FOR-begin, count from register (0x8003f5d0) */
    s_optable[0x23] = op_reg_compare;  /* register compare -> CONT/YIELD (0x8003ff68) */
    s_optable[0x24] = op_reg_set;      /* register = s16 imm (0x80040018) */
    s_optable[0x25] = op_reg_copy;     /* register = register (0x8004004c) */
    s_optable[0x26] = op_reg_arith_imm;/* register op= s16 imm (0x8004008c) */
    s_optable[0x27] = op_reg_arith_reg;/* register op= register (0x800400dc) */
    s_optable[0x1c] = op_nop;          /* nop-continue (0x8003f1c0) */
    s_optable[0x1d] = op_nop;          /* aliases of 0x8003f1d8 in the table (0x1d..0x20) */
    s_optable[0x1e] = op_nop;
    s_optable[0x1f] = op_nop;
    s_optable[0x20] = op_nop;
}

void re15_espvm_reset(void)
{
    memset(s_pool, 0, sizeof(s_pool));
    memset(s_reg, 0, sizeof(s_reg));   /* the DAT_800b0fd0 register file resets with the VM */
    for (int i = 0; i < RE15_ESPVM_OPCODES; i++) s_optable[i] = re15_espvm_op_stub;
    re15_espvm_install_ops();
}

re15_espvm_instance_t *re15_espvm_instance(int i)
{
    if (i < 0 || i >= RE15_ESPVM_INSTANCES) return NULL;
    return &s_pool[i];
}

int re15_espvm_active_count(void)
{
    int n = 0;
    for (int i = 0; i < RE15_ESPVM_INSTANCES; i++)
        if (s_pool[i].mem[RE15_ESPVM_OFF_ACTIVE]) n++;
    return n;
}

/* ---- FUN_8003edec: instance init ----------------------------------------------------------- */
/* param_1[1]=1; *param_1=0; param_1[4]=0xff; param_1[8]=0xff;
 * *(int**)(param_1+0x140) = param_1 + (s8)param_1[2]*0x20 + 0xc0;     (sub-stack base ptr)
 * *(uint*)(param_1+0x1c)  = DAT_800b3f70 + *(u16*)(param_2*2 + DAT_800b3f70);   (pc = base+off) */
static void re15_espvm_init(re15_espvm_instance_t *inst, uint16_t bytecode_id, const uint8_t *code_base)
{
    inst->code_base = code_base;
    inst->mem[RE15_ESPVM_OFF_ACTIVE] = 1;
    inst->mem[RE15_ESPVM_OFF_OP0]    = 0;
    inst->mem[RE15_ESPVM_OFF_DEPTH0] = 0xff;
    inst->mem[RE15_ESPVM_OFF_CIDX0]  = 0xff;

    /* stack pointer = instance + level*0x20 + 0xc0 (port: a mem-offset). level (mem[0x02]) is 0
     * here, set by the allocator below, so the stack base is +0xc0. */
    int8_t level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    wr_u32(inst->mem + RE15_ESPVM_OFF_SP, (uint32_t)((int)level * 0x20 + RE15_ESPVM_OFF_STACK));

    /* pc = offset_table[id] (the bytecode blob opens with a u16 offset table). Port stores the
     * pc as an offset into code_base; the original stores the absolute DAT_800b3f70+off. */
    uint32_t pc_off = code_base ? rd_u16(code_base + (uint32_t)bytecode_id * 2u) : 0;
    wr_u32(inst->mem + RE15_ESPVM_OFF_PC, pc_off);
}

/* ---- FUN_8003ee3c: allocator (in-game path) ------------------------------------------------ */
re15_espvm_instance_t *re15_espvm_alloc(uint8_t slot_index, uint16_t bytecode_id,
                                        const uint8_t *code_base)
{
    if (!code_base) return NULL;

    /* DAT_800b3f7a == 0 (in-game, not the menu): slot = (idx < 10) ? idx : 2. The menu path
     * (idx<0xe, free-slot scan 10..12) is deferred — it is menu rendering, not in-game effects. */
    uint8_t slot = (slot_index < RE15_ESPVM_INSTANCES) ? slot_index : 2;
    re15_espvm_instance_t *inst = &s_pool[slot];

    /* (&DAT_800b2b4e)[i*0x170] = 0  ->  instance[0x02] = 0; then zero 6 words @ instance[0x158]. */
    inst->mem[RE15_ESPVM_OFF_LEVEL] = 0;
    memset(inst->mem + 0x158, 0, 6 * 4);

    re15_espvm_init(inst, bytecode_id, code_base);
    return inst;
}

/* ============================================================================================
 * Phase ESP-C2 batch 1 — the control-flow core opcodes @0x800744a8[0x00..0x08] + the nop aliases.
 * Each handler receives the instance (a0), reads its operands from code_base[pc+n], advances the
 * pc itself, and returns CONT / STOP / YIELD. All disasm-verified @0x8003f1c0..f3dc.
 * ============================================================================================ */

#define INST_PC(i)      rd_u32((i)->mem + RE15_ESPVM_OFF_PC)
#define INST_SETPC(i,v) wr_u32((i)->mem + RE15_ESPVM_OFF_PC, (v))

/* 0x00 / 0x1c / 0x1d-0x20: nop-continue — pc++ (no operand), CONT. (0x8003f1d8 / 0x8003f1c0) */
static int op_nop(re15_espvm_instance_t *inst)
{
    INST_SETPC(inst, INST_PC(inst) + 1);
    return RE15_ESPVM_RET_CONT;
}

/* 0x02: wait-1-frame — pc++ (no operand), STOP (the instance resumes next frame). (0x8003f258) */
static int op_wait(re15_espvm_instance_t *inst)
{
    INST_SETPC(inst, INST_PC(inst) + 1);
    return RE15_ESPVM_RET_STOP;
}

/* 0x01: end/return — at level 0 deactivate the instance + STOP; else pop one nesting level
 * (restore pc from inst[0x144 + (level-1)*4], restore the loop-stack pointer) + CONT. (0x8003f1f0) */
static int op_end(re15_espvm_instance_t *inst)
{
    int8_t level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    if (level == 0) {
        inst->mem[RE15_ESPVM_OFF_ACTIVE] = 0;           /* sb zero -> inst[0x01] */
        return RE15_ESPVM_RET_STOP;
    }
    int8_t nl = (int8_t)(level - 1);
    uint32_t pc = rd_u32(inst->mem + RE15_ESPVM_OFF_RETPC + (uint32_t)nl * 4u);
    wr_u32(inst->mem + RE15_ESPVM_OFF_PC, pc);          /* pc = saved[level-1] */
    int8_t depth = (int8_t)inst->mem[nl + 4];           /* loop-depth of level-1 */
    inst->mem[RE15_ESPVM_OFF_LEVEL] = (uint8_t)nl;      /* pop level */
    uint32_t sp = (uint32_t)((int)nl * 0x20 + RE15_ESPVM_OFF_STACK + (int)depth * 4 + 4);
    wr_u32(inst->mem + RE15_ESPVM_OFF_SP, sp);          /* restore loop-stack pointer */
    return RE15_ESPVM_RET_CONT;
}

/* 0x03: jump-script — re-seed THIS instance with bytecode id = operand @pc+3 (FUN_8003edec),
 * then CONT (immediately start the new script). (0x8003f270) */
static int op_jump_script(re15_espvm_instance_t *inst)
{
    uint32_t pc = INST_PC(inst);
    uint8_t  id = inst->code_base[pc + 3];
    re15_espvm_init(inst, id, inst->code_base);
    return RE15_ESPVM_RET_CONT;
}

/* 0x04: spawn — allocate a sibling instance (slot = operand @pc+1, bytecode = operand @pc+3) via
 * FUN_8003ee3c, advance pc by 4, CONT. (0x8003f2a0) */
static int op_spawn(re15_espvm_instance_t *inst)
{
    uint32_t pc   = INST_PC(inst);
    uint8_t  slot = inst->code_base[pc + 1];
    uint8_t  id   = inst->code_base[pc + 3];
    INST_SETPC(inst, pc + 4);
    re15_espvm_alloc(slot, id, inst->code_base);
    return RE15_ESPVM_RET_CONT;
}

/* 0x05: kill — clear the active flag of pool instance index = operand @pc+1, pc+=2, CONT.
 * (0x8003f2dc; the original computes pool[idx]+1 with no bound check — the port guards idx<10.) */
static int op_kill(re15_espvm_instance_t *inst)
{
    uint32_t pc  = INST_PC(inst);
    uint8_t  idx = inst->code_base[pc + 1];
    if (idx < RE15_ESPVM_INSTANCES)
        s_pool[idx].mem[RE15_ESPVM_OFF_ACTIVE] = 0;
    INST_SETPC(inst, pc + 2);
    return RE15_ESPVM_RET_CONT;
}

/* 0x06: loop-push — push (pc+4 + u16 @pc+2) onto the loop-return stack, increment the level's
 * depth counter, advance pc by 4, CONT. (0x8003f328) */
static int op_loop_push(re15_espvm_instance_t *inst)
{
    uint32_t pc      = INST_PC(inst);
    int8_t   level   = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    uint16_t operand = rd_u16(inst->code_base + pc + 2);
    uint32_t next_pc = pc + 4;
    INST_SETPC(inst, next_pc);
    inst->mem[level + 4] = (uint8_t)(inst->mem[level + 4] + 1);   /* depth++ */
    uint32_t sp = rd_u32(inst->mem + RE15_ESPVM_OFF_SP);
    wr_u32(inst->mem + sp, next_pc + operand);                    /* *(sp) = target */
    wr_u32(inst->mem + RE15_ESPVM_OFF_SP, sp + 4);                /* sp += 4 */
    return RE15_ESPVM_RET_CONT;
}

/* 0x07: loop-jump — pop (sp-=4), relative jump pc += u16 @pc+2, decrement the level's depth,
 * CONT. (0x8003f368) */
static int op_loop_jump(re15_espvm_instance_t *inst)
{
    uint32_t sp      = rd_u32(inst->mem + RE15_ESPVM_OFF_SP);
    uint32_t pc      = INST_PC(inst);
    wr_u32(inst->mem + RE15_ESPVM_OFF_SP, sp - 4);                /* sp -= 4 */
    uint16_t operand = rd_u16(inst->code_base + pc + 2);
    int8_t   level   = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    wr_u32(inst->mem + RE15_ESPVM_OFF_PC, pc + operand);          /* pc += operand */
    inst->mem[level + 4] = (uint8_t)(inst->mem[level + 4] - 1);   /* depth-- */
    return RE15_ESPVM_RET_CONT;
}

/* 0x08: loop-pop — pop (sp-=4), advance pc by 2, decrement the level's depth, CONT. (0x8003f3a4) */
static int op_loop_pop(re15_espvm_instance_t *inst)
{
    uint32_t sp    = rd_u32(inst->mem + RE15_ESPVM_OFF_SP);
    wr_u32(inst->mem + RE15_ESPVM_OFF_SP, sp - 4);                /* sp -= 4 */
    uint32_t pc    = INST_PC(inst);
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    wr_u32(inst->mem + RE15_ESPVM_OFF_PC, pc + 2);               /* pc += 2 */
    inst->mem[level + 4] = (uint8_t)(inst->mem[level + 4] - 1);   /* depth-- */
    return RE15_ESPVM_RET_CONT;
}

/* ---- Phase ESP-C2 batch 2 — the per-level loop-counter ops 0x09..0x0c ----------------------
 * A frame-counter loop: 0x09 sets up a counter (count = u16 @pc+2), 0x0a counts it down one per
 * frame and skips past once it hits 0. Together [09 0a lo hi] = "wait `lo|hi<<8` frames". The
 * counters live in the per-level u16 array (inst + level*8 + 0xa0), indexed by inst[level+8]. */

/* 0x09: counter-set — cidx++, store count = u16 @pc+2 into counter[cidx], pc+=1, CONT. The op
 * advances pc by only 1 (onto the paired 0x0a byte); 0x0a's pc+=3 later skips the count u16.
 * (0x8003f3e0) */
static int op_counter_set(re15_espvm_instance_t *inst)
{
    uint32_t pc    = INST_PC(inst);
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    uint16_t count = rd_u16(inst->code_base + pc + 2);
    uint8_t  cidx  = (uint8_t)(inst->mem[level + 8] + 1);        /* cidx++ */
    inst->mem[level + 8] = cidx;
    INST_SETPC(inst, pc + 1);
    int idx = (int8_t)cidx;                                       /* (s8) then *2, per the original */
    wr_u16(inst->mem + level * 8 + RE15_ESPVM_OFF_CARRAY + idx * 2, count);
    return RE15_ESPVM_RET_CONT;
}

/* 0x0a: counter-wait — counter[cidx]--; while != 0 STOP (re-run next frame); when it reaches 0,
 * pc+=3 (skip this op + the count u16) and pop the counter (cidx--), STOP. (0x8003f428) */
static int op_counter_wait(re15_espvm_instance_t *inst)
{
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    int8_t   cidx  = (int8_t)inst->mem[level + 8];
    uint8_t *slot  = inst->mem + level * 8 + RE15_ESPVM_OFF_CARRAY + cidx * 2;
    uint16_t c     = (uint16_t)(rd_u16(slot) - 1);
    wr_u16(slot, c);
    if (c != 0)
        return RE15_ESPVM_RET_STOP;
    INST_SETPC(inst, INST_PC(inst) + 3);
    inst->mem[level + 8] = (uint8_t)(inst->mem[level + 8] - 1);   /* cidx-- */
    return RE15_ESPVM_RET_STOP;
}

/* 0x0b: counter-push — cidx++, pc+=1, CONT. (0x8003f490) */
static int op_counter_push(re15_espvm_instance_t *inst)
{
    int8_t level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    inst->mem[level + 8] = (uint8_t)(inst->mem[level + 8] + 1);
    INST_SETPC(inst, INST_PC(inst) + 1);
    return RE15_ESPVM_RET_CONT;
}

/* 0x0c: wait-condition — while the global pause/freeze gate holds, STOP (stay at pc); once it
 * clears, pc+=1, cidx--, STOP. Gate = FUN_8004efe4(0x800aca3c,0x1a) || (*(u8*)0x800b8520 & 0x80),
 * which the port models via re15_espvm_set_wait_gate (default 0 = normal play). (0x8003f4c4) */
static int op_wait_cond(re15_espvm_instance_t *inst)
{
    if (s_espvm_wait_gate)
        return RE15_ESPVM_RET_STOP;
    int8_t level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    INST_SETPC(inst, INST_PC(inst) + 1);
    inst->mem[level + 8] = (uint8_t)(inst->mem[level + 8] - 1);   /* cidx-- */
    return RE15_ESPVM_RET_STOP;
}

/* ---- Phase ESP-C2 batch 3 — the 2nd loop subsystem (FOR-loops, 0x0d..0x1a) -----------------
 * Indexed by the per-level counter-index ci = inst[level+8] over four per-level arrays:
 *   DEPTHARR byte  @ inst + level*4    + 0x0c + ci
 *   PCARR    u32   @ inst + level*0x10 + 0x20 + ci*4   (loop body start)
 *   TGTARR   u32   @ inst + level*0x10 + 0x60 + ci*4   (loop end target)
 *   CARRAY   u16   @ inst + level*8    + 0xa0 + ci*2   (iteration count, shared with 0x09/0x0a)
 * All disasm-verified @0x8003f540..fcf0. */

static uint32_t for_deptharr(int level, int ci) { return (uint32_t)(level * 4    + RE15_ESPVM_OFF_DEPTHARR + ci);     }
static uint32_t for_pcarr(int level, int ci)    { return (uint32_t)(level * 0x10 + RE15_ESPVM_OFF_PCARR    + ci * 4); }
static uint32_t for_tgtarr(int level, int ci)   { return (uint32_t)(level * 0x10 + RE15_ESPVM_OFF_TGTARR   + ci * 4); }
static uint32_t for_carray(int level, int ci)   { return (uint32_t)(level * 8    + RE15_ESPVM_OFF_CARRAY   + ci * 2); }

/* 0x0d: FOR-begin (count from bytecode) — ci++; CARRAY[ci]=u16@pc+4; TGTARR[ci]=(pc+6)+s16@pc+2;
 * PCARR[ci]=pc+6; DEPTHARR[ci]=loop-depth[level]; pc+=6; CONT. (0x8003f540) */
static int op_for_begin(re15_espvm_instance_t *inst)
{
    uint32_t pc    = INST_PC(inst);
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    int16_t  rel   = (int16_t)rd_u16(inst->code_base + pc + 2);
    uint16_t count = rd_u16(inst->code_base + pc + 4);
    int      ci    = (int8_t)(inst->mem[level + 8] + 1);
    inst->mem[level + 8] = (uint8_t)ci;
    uint32_t npc   = pc + 6;
    wr_u16(inst->mem + for_carray(level, ci), count);
    wr_u32(inst->mem + for_tgtarr(level, ci), npc + (uint32_t)(int32_t)rel);
    wr_u32(inst->mem + for_pcarr(level, ci), npc);
    INST_SETPC(inst, npc);
    inst->mem[for_deptharr(level, ci)] = inst->mem[level + 4];
    return RE15_ESPVM_RET_CONT;
}

/* 0x11: FOR-begin (no count) — ci++; PCARR[ci]=pc+4; TGTARR[ci]=(pc+4)+s16@pc+2;
 * DEPTHARR[ci]=loop-depth[level]; pc+=4; CONT. (0x8003f8bc) */
static int op_for_begin_nc(re15_espvm_instance_t *inst)
{
    uint32_t pc    = INST_PC(inst);
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    int16_t  rel   = (int16_t)rd_u16(inst->code_base + pc + 2);
    int      ci    = (int8_t)(inst->mem[level + 8] + 1);
    inst->mem[level + 8] = (uint8_t)ci;
    uint32_t npc   = pc + 4;
    wr_u32(inst->mem + for_pcarr(level, ci), npc);
    INST_SETPC(inst, npc);
    wr_u32(inst->mem + for_tgtarr(level, ci), npc + (uint32_t)(int32_t)rel);
    inst->mem[for_deptharr(level, ci)] = inst->mem[level + 4];
    return RE15_ESPVM_RET_CONT;
}

/* 0x0e: NEXT — CARRAY[ci]--; if != 0 pc=PCARR[ci] (loop back); else pc+=2, ci--. CONT. (0x8003f674) */
static int op_for_next(re15_espvm_instance_t *inst)
{
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    int8_t   ci    = (int8_t)inst->mem[level + 8];
    uint8_t *cnt   = inst->mem + for_carray(level, ci);
    uint16_t c     = (uint16_t)(rd_u16(cnt) - 1);
    wr_u16(cnt, c);
    if (c != 0) {
        wr_u32(inst->mem + RE15_ESPVM_OFF_PC, rd_u32(inst->mem + for_pcarr(level, ci)));
    } else {
        INST_SETPC(inst, INST_PC(inst) + 2);
        inst->mem[level + 8] = (uint8_t)(inst->mem[level + 8] - 1);
    }
    return RE15_ESPVM_RET_CONT;
}

/* 0x10: loop-back — pc=PCARR[ci]; ci--. CONT. (0x8003f878) */
static int op_for_loop(re15_espvm_instance_t *inst)
{
    int8_t level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    int8_t ci    = (int8_t)inst->mem[level + 8];
    wr_u32(inst->mem + RE15_ESPVM_OFF_PC, rd_u32(inst->mem + for_pcarr(level, ci)));
    inst->mem[level + 8] = (uint8_t)(inst->mem[level + 8] - 1);
    return RE15_ESPVM_RET_CONT;
}

/* 0x1a: FOR-break — pc=TGTARR[ci]; loop-depth[level]=DEPTHARR[ci]; ci--. CONT. (0x8003fca8) */
static int op_for_break(re15_espvm_instance_t *inst)
{
    int8_t level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    int8_t ci    = (int8_t)inst->mem[level + 8];
    wr_u32(inst->mem + RE15_ESPVM_OFF_PC, rd_u32(inst->mem + for_tgtarr(level, ci)));
    inst->mem[level + 4] = inst->mem[for_deptharr(level, ci)];
    inst->mem[level + 8] = (uint8_t)(inst->mem[level + 8] - 1);
    return RE15_ESPVM_RET_CONT;
}

/* 0x16: pop counter-index — ci--; pc+=2. CONT. (0x8003fb68) */
static int op_for_popci(re15_espvm_instance_t *inst)
{
    int8_t level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    inst->mem[level + 8] = (uint8_t)(inst->mem[level + 8] - 1);
    INST_SETPC(inst, INST_PC(inst) + 2);
    return RE15_ESPVM_RET_CONT;
}

/* 0x14: pc+=6; CONT. (0x8003fb38)   0x15: pc+=2; CONT. (0x8003fb50) */
static int op_skip6(re15_espvm_instance_t *inst) { INST_SETPC(inst, INST_PC(inst) + 6); return RE15_ESPVM_RET_CONT; }
static int op_skip2(re15_espvm_instance_t *inst) { INST_SETPC(inst, INST_PC(inst) + 2); return RE15_ESPVM_RET_CONT; }

/* 0x17: block setup — loop-depth[level]=u8@pc+1; counter-index[level]=u8@pc+2;
 * sp = level*0x20 + 0xc0 + depth*4 + 4; pc += s16@pc+4 (relative). CONT. (0x8003fb9c) */
static int op_block_setup(re15_espvm_instance_t *inst)
{
    uint32_t pc    = INST_PC(inst);
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    uint8_t  depth = inst->code_base[pc + 1];
    uint8_t  ci    = inst->code_base[pc + 2];
    int16_t  rel   = (int16_t)rd_u16(inst->code_base + pc + 4);
    inst->mem[level + 4] = depth;
    uint32_t sp = (uint32_t)(level * 0x20 + RE15_ESPVM_OFF_STACK + (int)depth * 4 + 4);
    wr_u32(inst->mem + RE15_ESPVM_OFF_SP, sp);
    inst->mem[level + 8] = ci;
    INST_SETPC(inst, pc + (uint32_t)(int32_t)rel);
    return RE15_ESPVM_RET_CONT;
}

/* 0x18: GOSUB / push level — save the return pc (pc+2) to saved-pc[level] (+0x144 + level*4),
 * level++, reinit the new level's loop-depth(+4)/counter-index(+8) to 0xff and its stack pointer,
 * then jump pc to sub-script id@pc+1 (pc = offset_table[id], the DAT_800b3f70/code_base table).
 * CONT. (0x8003fbe8) */
static int op_gosub(re15_espvm_instance_t *inst)
{
    uint32_t pc    = INST_PC(inst);
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    uint8_t  subid = inst->code_base[pc + 1];
    wr_u32(inst->mem + RE15_ESPVM_OFF_RETPC + (uint32_t)((int)level * 4), pc + 2);  /* saved-pc[level] = pc+2 */
    int nl = level + 1;
    inst->mem[nl + 4] = 0xff;          /* loop-depth[level+1] = 0xff */
    inst->mem[nl + 8] = 0xff;          /* counter-index[level+1] = 0xff */
    wr_u32(inst->mem + RE15_ESPVM_OFF_SP, (uint32_t)(nl * 0x20 + RE15_ESPVM_OFF_STACK));
    inst->mem[RE15_ESPVM_OFF_LEVEL] = (uint8_t)nl;
    INST_SETPC(inst, rd_u16(inst->code_base + (uint32_t)subid * 2));   /* pc = offset_table[subid] */
    return RE15_ESPVM_RET_CONT;
}

/* ---- Phase ESP-C2 batch 7 — the conditional block evaluators (0x0f/0x12/0x13) --------------
 * 0x0f/0x12 evaluate a chain of predicate sub-opcodes (each dispatched through the opcode table,
 * combined with AND when the inter-predicate u16 flag == 0 else OR); 0x13 is a register switch.
 * Disasm-verified @0x8003f6f4 / @0x8003f930 / @0x8003fa5c. A predicate sub-op returns 1 (true) or
 * 0 (false) — exactly what 0x23 (compare) yields; an unported flag-test predicate (stub returns
 * STOP=2) would read as "true", so the conditional ops are byte-true only over ported predicates. */

/* Evaluate the predicate chain [chain_start, s3): dispatch the first predicate, then loop reading
 * a u16 AND/OR flag + the next predicate until pc reaches s3. Returns the combined boolean. */
static int espvm_eval_chain(re15_espvm_instance_t *inst, uint32_t chain_start, uint32_t s3)
{
    INST_SETPC(inst, chain_start);
    int res = s_optable[inst->code_base[chain_start]](inst);          /* first predicate */
    int guard = 0;
    while (INST_PC(inst) != s3 && guard++ < 64) {                     /* guard: malformed-chain hang */
        uint32_t p    = INST_PC(inst);
        uint16_t flag = rd_u16(inst->code_base + p);                  /* 0 = AND, != 0 = OR */
        INST_SETPC(inst, p + 2);
        int r = s_optable[inst->code_base[INST_PC(inst)]](inst);
        res = (flag == 0) ? (res & r) : (res | r);
    }
    return res;
}

/* 0x0f: IF/while block-begin — push a FOR-frame (ci++, PCARR[ci]=this op's addr, TGTARR[ci]=
 * (pc+4)+s16@pc+2, DEPTHARR[ci]=loop-depth[level]); pc+=4; evaluate the predicate chain of
 * len=u8@pc+1 bytes. TRUE -> enter the block (pc left after the chain); FALSE -> pc=TGTARR[ci]
 * (skip the block) and ci--. CONT. (0x8003f6f4) */
static int op_if_begin(re15_espvm_instance_t *inst)
{
    uint32_t pc    = INST_PC(inst);
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    int      ci    = (int8_t)(inst->mem[level + 8] + 1);
    inst->mem[level + 8] = (uint8_t)ci;
    uint8_t  len   = inst->code_base[pc + 1];
    int16_t  rel   = (int16_t)rd_u16(inst->code_base + pc + 2);
    uint32_t npc   = pc + 4;
    wr_u32(inst->mem + for_pcarr(level, ci), pc);                     /* PCARR[ci] = this op's addr */
    wr_u32(inst->mem + for_tgtarr(level, ci), npc + (uint32_t)(int32_t)rel);
    inst->mem[for_deptharr(level, ci)] = inst->mem[level + 4];
    int res = espvm_eval_chain(inst, npc, npc + len);
    if (res == 0) {
        INST_SETPC(inst, rd_u32(inst->mem + for_tgtarr(level, ci))); /* skip block */
        inst->mem[level + 8] = (uint8_t)(inst->mem[level + 8] - 1);  /* ci-- */
    }
    return RE15_ESPVM_RET_CONT;
}

/* 0x12: compound IF — evaluate the predicate chain of len=u8@pc+1 bytes (chain @pc+2). TRUE ->
 * pc=PCARR[ci] (loop back to the block body); FALSE -> ci-- (exit). CONT. (0x8003f930) */
static int op_cond_chain(re15_espvm_instance_t *inst)
{
    uint32_t pc    = INST_PC(inst);
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    uint8_t  len   = inst->code_base[pc + 1];
    uint32_t cs    = pc + 2;
    int res = espvm_eval_chain(inst, cs, cs + len);
    int ci  = (int8_t)inst->mem[level + 8];
    if (res != 0)
        INST_SETPC(inst, rd_u32(inst->mem + for_pcarr(level, ci))); /* loop back */
    else
        inst->mem[level + 8] = (uint8_t)(inst->mem[level + 8] - 1); /* exit */
    return RE15_ESPVM_RET_CONT;
}

/* 0x13: register switch — push a FOR-frame (ci++, TGTARR[ci]=(pc+4)+u16@pc+2 = default/exit,
 * DEPTHARR[ci]=loop-depth[level]); switch value = reg[u8@pc+1]. Walk the case list at pc+4: a
 * 0x15 byte ends it, 0x16 is the default (ci--), otherwise each case = [marker, _, u16 skip,
 * s16 value]: on a value match jump to the case body (a3+=6) else skip (a3+=skip). pc=a3. CONT.
 * (0x8003fa5c) */
static int op_switch(re15_espvm_instance_t *inst)
{
    uint32_t pc    = INST_PC(inst);
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    uint8_t  ridx  = inst->code_base[pc + 1];
    uint16_t drel  = rd_u16(inst->code_base + pc + 2);
    uint32_t a3    = pc + 4;
    int      ci    = (int8_t)(inst->mem[level + 8] + 1);
    inst->mem[level + 8] = (uint8_t)ci;
    wr_u32(inst->mem + for_tgtarr(level, ci), a3 + drel);
    inst->mem[for_deptharr(level, ci)] = inst->mem[level + 4];
    int16_t  sw    = (int16_t)rd_u16(s_reg + (uint32_t)ridx * 2);
    int guard = 0;
    for (;;) {
        if (guard++ > 256) break;                                    /* malformed-list guard */
        uint8_t marker = inst->code_base[a3];
        if (marker == 0x15) { a3 += 2; break; }                      /* end of switch */
        if (marker == 0x16) {                                        /* default case */
            inst->mem[level + 8] = (uint8_t)(inst->mem[level + 8] - 1);
            a3 += 2; break;
        }
        uint16_t skip = rd_u16(inst->code_base + a3 + 2);
        int16_t  val  = (int16_t)rd_u16(inst->code_base + a3 + 4);
        if (sw == val) { a3 += 6; break; }                           /* match -> case body */
        a3 += skip;                                                  /* skip to next case */
    }
    INST_SETPC(inst, a3);
    return RE15_ESPVM_RET_CONT;
}

/* 0x19: pop level — level--; pc = saved-pc[level-1] (+0x144 + (level-1)*4); restore the loop
 * stack pointer. CONT. (0x8003fc50; reads level unsigned then decrements, byte-true) */
static int op_pop_level(re15_espvm_instance_t *inst)
{
    uint8_t nl = (uint8_t)(inst->mem[RE15_ESPVM_OFF_LEVEL] - 1);
    inst->mem[RE15_ESPVM_OFF_LEVEL] = nl;
    int8_t nls = (int8_t)nl;
    wr_u32(inst->mem + RE15_ESPVM_OFF_PC,
           rd_u32(inst->mem + RE15_ESPVM_OFF_RETPC + (uint32_t)((int)nls * 4)));
    int8_t   depth = (int8_t)inst->mem[nls + 4];
    uint32_t sp    = (uint32_t)((int)nls * 0x20 + RE15_ESPVM_OFF_STACK + (int)depth * 4 + 4);
    wr_u32(inst->mem + RE15_ESPVM_OFF_SP, sp);
    return RE15_ESPVM_RET_CONT;
}

/* ---- Phase ESP-C2 batch 4 — the two instance-local field ops -------------------------------
 * The effect's position(+0x158..0x162)/velocity(+0x164..0x16e) u16 fields (the 6 words the
 * allocator zeroes @+0x158). The OTHER field-setters (0x32/0x4d/0x3f/0x42/0x4a/0x4c) dereference
 * an external pointer (the work struct @inst+0x154 or the effect pool DAT_800b2368) and are
 * deferred to the pool/work-struct integration. Disasm-verified @0x80040f14 / @0x80040fd4. */

/* 0x2f: u16-store — inst[0x158 + (u8@pc+1)*2] = u16@pc+2; pc+=4; CONT. (0x80040f14) */
static int op_set_u16(re15_espvm_instance_t *inst)
{
    uint32_t pc = INST_PC(inst);
    uint8_t  id = inst->code_base[pc + 1];
    uint16_t v  = rd_u16(inst->code_base + pc + 2);
    INST_SETPC(inst, pc + 4);
    wr_u16(inst->mem + 0x158 + (uint32_t)id * 2, v);
    return RE15_ESPVM_RET_CONT;
}

/* 0x31: per-frame integrator — six u16 accumulators inst[0x158..0x162] += six u16 deltas
 * inst[0x164..0x16e]; pc+=1; CONT. (the acc/delta ranges are disjoint, so order is moot). (0x80040fd4) */
static int op_integrate(re15_espvm_instance_t *inst)
{
    for (int i = 0; i < 6; i++) {
        uint8_t *acc = inst->mem + 0x158 + i * 2;
        uint16_t d   = rd_u16(inst->mem + 0x164 + i * 2);
        wr_u16(acc, (uint16_t)(rd_u16(acc) + d));
    }
    INST_SETPC(inst, INST_PC(inst) + 1);
    return RE15_ESPVM_RET_CONT;
}

/* ---- Phase ESP-C2 batch 5 — the VM register file (DAT_800b0fd0) ops ------------------------
 * 0x24/0x25 set/copy, 0x23 compare (CONT/YIELD = the IF mechanism), 0x26/0x27 arithmetic via
 * the 12-way helper FUN_80040140, 0x1b FOR-begin with the count read from a register.
 * Disasm-verified @0x80040018 / @0x8004004c / @0x8003ff68 / @0x8004008c / @0x800400dc /
 * @0x80040140 (+ jump tables @0x80010c3c / @0x80010c5c) / @0x8003f5d0. */

/* FUN_80040140: apply arithmetic op (0..0xb) to the u16 register at `reg` with value `val`. */
static void re15_espvm_arith(int op, uint8_t *reg, int16_t val)
{
    uint16_t cur = rd_u16(reg);
    int16_t  s   = (int16_t)cur;
    uint16_t r;
    switch (op) {
        case 0x0: r = (uint16_t)(cur + (uint16_t)val);              break;  /* add  */
        case 0x1: r = (uint16_t)(cur - (uint16_t)val);              break;  /* sub  */
        case 0x2: r = (uint16_t)((int32_t)s * (int32_t)val);        break;  /* mul  */
        case 0x3: if (val == 0 || (s == INT16_MIN && val == -1)) return;    /* div  (orig traps) */
                  r = (uint16_t)(s / val);                          break;
        case 0x4: if (val == 0 || (s == INT16_MIN && val == -1)) return;    /* mod  */
                  r = (uint16_t)(s % val);                          break;
        case 0x5: r = (uint16_t)(cur | (uint16_t)val);              break;  /* or   */
        case 0x6: r = (uint16_t)(cur & (uint16_t)val);              break;  /* and  */
        case 0x7: r = (uint16_t)(cur ^ (uint16_t)val);              break;  /* xor  */
        case 0x8: r = (uint16_t)(~cur);                             break;  /* not (val ignored) */
        case 0x9: r = (uint16_t)(cur << ((val) & 31));              break;  /* shl  */
        case 0xa: r = (uint16_t)((uint32_t)cur >> ((val) & 31));    break;  /* shr logical (lhu) */
        case 0xb: r = (uint16_t)((int32_t)s >> ((val) & 31));       break;  /* shr arith   (lh)  */
        default:  return;                                                   /* op>=0xc: no-op */
    }
    wr_u16(reg, r);
}

/* 0x24: reg[u8@pc+1] = s16@pc+2; pc+=4; CONT. (0x80040018) */
static int op_reg_set(re15_espvm_instance_t *inst)
{
    uint32_t pc = INST_PC(inst);
    uint8_t  id = inst->code_base[pc + 1];
    uint16_t v  = rd_u16(inst->code_base + pc + 2);
    INST_SETPC(inst, pc + 4);
    wr_u16(s_reg + (uint32_t)id * 2, v);
    return RE15_ESPVM_RET_CONT;
}

/* 0x25: reg[u8@pc+1] = reg[u8@pc+2]; pc+=3; CONT. (0x8004004c) */
static int op_reg_copy(re15_espvm_instance_t *inst)
{
    uint32_t pc   = INST_PC(inst);
    uint8_t  dst  = inst->code_base[pc + 1];
    uint8_t  src  = inst->code_base[pc + 2];
    INST_SETPC(inst, pc + 3);
    wr_u16(s_reg + (uint32_t)dst * 2, rd_u16(s_reg + (uint32_t)src * 2));
    return RE15_ESPVM_RET_CONT;
}

/* 0x23: compare reg[w&0xff] (signed) against s16@pc+4 using selector (w>>8); w=u16@pc+2. pc+=6.
 * Returns CONT(1) if the comparison holds, else YIELD(0) (the IF/skip mechanism). (0x8003ff68) */
static int op_reg_compare(re15_espvm_instance_t *inst)
{
    uint32_t pc  = INST_PC(inst);
    uint16_t w   = rd_u16(inst->code_base + pc + 2);
    int16_t  rhs = (int16_t)rd_u16(inst->code_base + pc + 4);
    INST_SETPC(inst, pc + 6);
    int16_t  lhs = (int16_t)rd_u16(s_reg + (uint32_t)(w & 0xff) * 2);
    int      res;
    switch (w >> 8) {
        case 0: res = (lhs == rhs);            break;
        case 1: res = (lhs >  rhs);            break;
        case 2: res = (lhs >= rhs);            break;
        case 3: res = (lhs <  rhs);            break;
        case 4: res = (lhs <= rhs);            break;
        case 5: res = (lhs != rhs);            break;
        case 6: res = ((lhs & rhs) != 0);      break;   /* bitwise-AND test */
        default: res = 0;                      break;
    }
    return res ? RE15_ESPVM_RET_CONT : RE15_ESPVM_RET_YIELD;
}

/* 0x26: reg[w>>8] op= s16@pc+4, op = w&0xff; w=u16@pc+2. pc+=6; CONT. (0x8004008c) */
static int op_reg_arith_imm(re15_espvm_instance_t *inst)
{
    uint32_t pc = INST_PC(inst);
    uint16_t w  = rd_u16(inst->code_base + pc + 2);
    int16_t  v  = (int16_t)rd_u16(inst->code_base + pc + 4);
    INST_SETPC(inst, pc + 6);
    re15_espvm_arith(w & 0xff, s_reg + (uint32_t)(w >> 8) * 2, v);
    return RE15_ESPVM_RET_CONT;
}

/* 0x27: reg[u8@pc+2] op= reg[u8@pc+3], op = u8@pc+1. pc+=4; CONT. (0x800400dc) */
static int op_reg_arith_reg(re15_espvm_instance_t *inst)
{
    uint32_t pc  = INST_PC(inst);
    uint8_t  op  = inst->code_base[pc + 1];
    uint8_t  dst = inst->code_base[pc + 2];
    uint8_t  src = inst->code_base[pc + 3];
    int16_t  v   = (int16_t)rd_u16(s_reg + (uint32_t)src * 2);
    INST_SETPC(inst, pc + 4);
    re15_espvm_arith(op, s_reg + (uint32_t)dst * 2, v);
    return RE15_ESPVM_RET_CONT;
}

/* 0x1b: FOR-begin with the iteration count read from reg[u8@pc+5] (else identical to 0x0d):
 * ci++; CARRAY[ci]=reg[u8@pc+5]; TGTARR[ci]=(pc+6)+s16@pc+2; PCARR[ci]=pc+6;
 * DEPTHARR[ci]=loop-depth[level]; pc+=6; CONT. (0x8003f5d0) */
static int op_for_begin_reg(re15_espvm_instance_t *inst)
{
    uint32_t pc    = INST_PC(inst);
    int8_t   level = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];
    int16_t  rel   = (int16_t)rd_u16(inst->code_base + pc + 2);
    uint8_t  ridx  = inst->code_base[pc + 5];
    uint16_t count = rd_u16(s_reg + (uint32_t)ridx * 2);
    int      ci    = (int8_t)(inst->mem[level + 8] + 1);
    inst->mem[level + 8] = (uint8_t)ci;
    uint32_t npc   = pc + 6;
    wr_u16(inst->mem + for_carray(level, ci), count);
    wr_u32(inst->mem + for_tgtarr(level, ci), npc + (uint32_t)(int32_t)rel);
    wr_u32(inst->mem + for_pcarr(level, ci), npc);
    INST_SETPC(inst, npc);
    inst->mem[for_deptharr(level, ci)] = inst->mem[level + 4];
    return RE15_ESPVM_RET_CONT;
}

/* ---- FUN_8003f0a0: per-frame VM walker ----------------------------------------------------- */
int re15_espvm_run_all(void)
{
    int ticked = 0;

    for (int i = 0; i < RE15_ESPVM_INSTANCES; i++) {
        re15_espvm_instance_t *inst = &s_pool[i];
        if (inst->mem[RE15_ESPVM_OFF_ACTIVE] == 0)   /* beq v0,zero,next */
            continue;
        ticked++;

        for (;;) {
            /* do { ret = optable[*pc](inst); } while (ret == CONT); */
            int ret;
            do {
                uint32_t pc = rd_u32(inst->mem + RE15_ESPVM_OFF_PC);
                uint8_t  op = inst->code_base[pc];           /* lbu 0(pc) */
                ret = s_optable[op](inst);                   /* (*PTR_800744a8[op])(inst) */
            } while (ret == RE15_ESPVM_RET_CONT);

            if (ret == RE15_ESPVM_RET_STOP)                  /* beq v1,2,next */
                break;

            /* ret == YIELD: pop the sub-return stack (FUN_8003f0a0 @0x8003f128..f178). */
            int8_t idx   = (int8_t)inst->mem[RE15_ESPVM_OFF_LEVEL];   /* lb 2(s0) */
            int8_t depth = (int8_t)inst->mem[idx + 4];                /* lb (idx+4)(s0) */
            if (depth < 0)                                            /* bltz -> next */
                break;

            uint32_t sp = rd_u32(inst->mem + RE15_ESPVM_OFF_SP);      /* lw 320(s0) */
            wr_u32(inst->mem + RE15_ESPVM_OFF_SP, sp - 4);            /* sp -= 4 */
            uint32_t popped = rd_u32(inst->mem + (sp - 4));           /* lw -4(sp) */
            wr_u32(inst->mem + RE15_ESPVM_OFF_PC, popped);            /* pc = popped */
            inst->mem[idx + 4] = (uint8_t)(inst->mem[idx + 4] - 1);  /* depth-- */
            /* loop back to exec with the popped pc */
        }
    }

    /* FUN_8003ebf4() + FUN_8003ec28(): GPU ordering-table / entity-sprite housekeeping for the
     * frame's draw — deferred to the render integration (no draw in the headless C1). */
    return ticked;
}
