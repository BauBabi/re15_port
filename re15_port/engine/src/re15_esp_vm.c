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
static void wr_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
}

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

void re15_espvm_reset(void)
{
    memset(s_pool, 0, sizeof(s_pool));
    for (int i = 0; i < RE15_ESPVM_OPCODES; i++) s_optable[i] = re15_espvm_op_stub;
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
    inst->mem[RE15_ESPVM_OFF_DEPTH1] = 0xff;

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
