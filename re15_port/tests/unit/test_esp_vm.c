/**
 * @file test_esp_vm.c
 * @brief Phase ESP-C1 — the effect-script VM infrastructure (re15_esp_vm.c).
 *
 * Verifies the byte-true port of the third ESP layer's pool + dispatch loop (FUN_8003edec init,
 * FUN_8003ee3c allocator, FUN_8003f0a0 walker) WITHOUT the real opcodes (those are Phase ESP-C2).
 * The opcode handlers installed here are SYNTHETIC — they exist only to drive the three dispatch
 * return paths (CONT / STOP / YIELD) and the sub-return-stack pop, so the VM mechanism itself can
 * be tested standalone. They do not claim to implement any real @0x800744a8 opcode semantics.
 */
#include "re15_esp_vm.h"

#include <stdint.h>
#include <stdio.h>

/* ---- synthetic opcodes + a dispatch log ---------------------------------------------------- */

#define OP_CONT     0x01   /* advance pc by 1, return CONT (keep executing this frame)           */
#define OP_YIELD    0x02   /* return YIELD (drives the sub-stack pop path)                        */
#define OP_STOP     0x03   /* return STOP (instance done this frame; stays active)               */
#define OP_DESPAWN  0x04   /* clear active flag, return STOP (despawn)                            */

static int     s_log_op[16];
static uint32_t s_log_pc[16];
static int     s_log_n;

static void log_dispatch(uint8_t op, const re15_espvm_instance_t *inst)
{
    if (s_log_n < 16) { s_log_op[s_log_n] = op; s_log_pc[s_log_n] = re15_espvm_get_pc(inst); s_log_n++; }
}

static int op_cont(re15_espvm_instance_t *inst)
{
    log_dispatch(OP_CONT, inst);
    re15_espvm_set_pc(inst, re15_espvm_get_pc(inst) + 1);
    return RE15_ESPVM_RET_CONT;
}
static int op_yield(re15_espvm_instance_t *inst)
{
    log_dispatch(OP_YIELD, inst);
    return RE15_ESPVM_RET_YIELD;
}
static int op_stop(re15_espvm_instance_t *inst)
{
    log_dispatch(OP_STOP, inst);
    return RE15_ESPVM_RET_STOP;
}
static int op_despawn(re15_espvm_instance_t *inst)
{
    log_dispatch(OP_DESPAWN, inst);
    inst->mem[RE15_ESPVM_OFF_ACTIVE] = 0;
    return RE15_ESPVM_RET_STOP;
}

static void install_test_ops(void)
{
    re15_espvm_set_opcode(OP_CONT,    op_cont);
    re15_espvm_set_opcode(OP_YIELD,   op_yield);
    re15_espvm_set_opcode(OP_STOP,    op_stop);
    re15_espvm_set_opcode(OP_DESPAWN, op_despawn);
}

/* A synthetic bytecode blob: a u16 offset table (ids 0..3) then four tiny scripts.
 *   id0 -> 0x10 : [OP_CONT, OP_STOP]    (exercises CONT then STOP)
 *   id1 -> 0x20 : [OP_YIELD]            (exercises the YIELD / sub-stack pop)
 *   id2 -> 0x30 : [OP_DESPAWN]          (exercises despawn)
 *   id3 -> 0x32 : [OP_STOP]             (the "returned-to" script for the pop test) */
static uint8_t s_code[64];
static void build_code(void)
{
    for (int i = 0; i < 64; i++) s_code[i] = 0;
    s_code[0] = 0x10; s_code[1] = 0x00;   /* id0 -> 0x10 */
    s_code[2] = 0x20; s_code[3] = 0x00;   /* id1 -> 0x20 */
    s_code[4] = 0x30; s_code[5] = 0x00;   /* id2 -> 0x30 */
    s_code[6] = 0x32; s_code[7] = 0x00;   /* id3 -> 0x32 */
    s_code[0x10] = OP_CONT; s_code[0x11] = OP_STOP;
    s_code[0x20] = OP_YIELD;
    s_code[0x30] = OP_DESPAWN;
    s_code[0x32] = OP_STOP;
}

static uint32_t rd_u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static void wr_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
}

int main(void)
{
    int fail = 0;
    build_code();
    printf("=== ESP effect-script VM (Phase ESP-C1) ===\n");

    /* (1) FUN_8003ee3c + FUN_8003edec: alloc id0 in slot 0, byte-true init fields. */
    re15_espvm_reset();
    install_test_ops();
    re15_espvm_instance_t *a = re15_espvm_alloc(0, 0, s_code);
    if (!a) { fprintf(stderr, "FAIL: (1) alloc returned NULL\n"); return 1; }
    if (a->mem[RE15_ESPVM_OFF_ACTIVE] != 1 || a->mem[RE15_ESPVM_OFF_OP0] != 0 ||
        a->mem[RE15_ESPVM_OFF_DEPTH0] != 0xff || a->mem[RE15_ESPVM_OFF_DEPTH1] != 0xff ||
        a->mem[RE15_ESPVM_OFF_LEVEL] != 0) {
        fprintf(stderr, "FAIL: (1) init flags wrong\n"); fail = 1; }
    if (re15_espvm_get_pc(a) != 0x10) {
        fprintf(stderr, "FAIL: (1) pc=0x%x expected 0x10\n", re15_espvm_get_pc(a)); fail = 1; }
    if (rd_u32(a->mem + RE15_ESPVM_OFF_SP) != RE15_ESPVM_OFF_STACK) {
        fprintf(stderr, "FAIL: (1) sp=0x%x expected 0x%x\n",
                rd_u32(a->mem + RE15_ESPVM_OFF_SP), RE15_ESPVM_OFF_STACK); fail = 1; }
    if (!fail) printf("  (1) PASS: alloc/init byte-true (active=1, depth=0xff, pc=0x10, sp=0xc0)\n");

    /* (2) FUN_8003f0a0 dispatch: CONT advances + re-executes, STOP ends the frame. */
    s_log_n = 0;
    int ticked = re15_espvm_run_all();
    if (ticked != 1) { fprintf(stderr, "FAIL: (2) ticked=%d expected 1\n", ticked); fail = 1; }
    if (s_log_n != 2 || s_log_op[0] != OP_CONT || s_log_pc[0] != 0x10 ||
        s_log_op[1] != OP_STOP || s_log_pc[1] != 0x11) {
        fprintf(stderr, "FAIL: (2) dispatch log wrong (n=%d)\n", s_log_n); fail = 1; }
    if (a->mem[RE15_ESPVM_OFF_ACTIVE] != 1) {
        fprintf(stderr, "FAIL: (2) STOP must not despawn\n"); fail = 1; }
    if (!fail) printf("  (2) PASS: CONT re-exec + STOP terminate (cont@0x10 -> stop@0x11)\n");

    /* (3) despawn opcode clears the active flag. */
    re15_espvm_reset(); install_test_ops();
    re15_espvm_alloc(0, 2, s_code);             /* id2 -> [OP_DESPAWN] */
    s_log_n = 0;
    re15_espvm_run_all();
    if (re15_espvm_active_count() != 0) {
        fprintf(stderr, "FAIL: (3) despawn -> active_count=%d\n", re15_espvm_active_count()); fail = 1; }
    if (s_log_n != 1 || s_log_op[0] != OP_DESPAWN) {
        fprintf(stderr, "FAIL: (3) despawn dispatch log\n"); fail = 1; }
    if (!fail) printf("  (3) PASS: despawn opcode clears active flag\n");

    /* (4) YIELD + the sub-return stack pop (FUN_8003f0a0 @0x8003f128..f178). Push a return
     * pc-offset 0x32 at the stack base, set depth>=0, then YIELD must pop it and resume there. */
    re15_espvm_reset(); install_test_ops();
    re15_espvm_instance_t *y = re15_espvm_alloc(0, 1, s_code);   /* id1 -> [OP_YIELD] @0x20 */
    wr_u32(y->mem + RE15_ESPVM_OFF_STACK, 0x32);                 /* push return offset 0x32 */
    wr_u32(y->mem + RE15_ESPVM_OFF_SP, RE15_ESPVM_OFF_STACK + 4);/* sp = base + 4 */
    y->mem[RE15_ESPVM_OFF_DEPTH0] = 0;                           /* depth >= 0 -> take the pop */
    s_log_n = 0;
    re15_espvm_run_all();
    if (s_log_n != 2 || s_log_op[0] != OP_YIELD || s_log_pc[0] != 0x20 ||
        s_log_op[1] != OP_STOP || s_log_pc[1] != 0x32) {
        fprintf(stderr, "FAIL: (4) yield/pop log wrong (n=%d)\n", s_log_n); fail = 1; }
    if (re15_espvm_get_pc(y) != 0x32) {
        fprintf(stderr, "FAIL: (4) pc after pop=0x%x expected 0x32\n", re15_espvm_get_pc(y)); fail = 1; }
    if (rd_u32(y->mem + RE15_ESPVM_OFF_SP) != RE15_ESPVM_OFF_STACK) {
        fprintf(stderr, "FAIL: (4) sp after pop=0x%x expected 0xc0\n",
                rd_u32(y->mem + RE15_ESPVM_OFF_SP)); fail = 1; }
    if (y->mem[RE15_ESPVM_OFF_DEPTH0] != 0xff) {
        fprintf(stderr, "FAIL: (4) depth after pop=0x%x expected 0xff\n",
                y->mem[RE15_ESPVM_OFF_DEPTH0]); fail = 1; }
    if (!fail) printf("  (4) PASS: YIELD pops the sub-stack (yield@0x20 -> resume stop@0x32, depth--)\n");

    /* (5) slot fallback: slot_index >= 10 -> slot 2 (FUN_8003ee3c in-game overflow). */
    re15_espvm_reset(); install_test_ops();
    re15_espvm_instance_t *f = re15_espvm_alloc(12, 0, s_code);
    if (f != re15_espvm_instance(2) || re15_espvm_get_pc(f) != 0x10) {
        fprintf(stderr, "FAIL: (5) slot overflow should map to slot 2\n"); fail = 1; }
    if (!fail) printf("  (5) PASS: slot_index>=10 falls back to slot 2\n");

    /* (6) empty pool -> no ticks. */
    re15_espvm_reset();
    if (re15_espvm_run_all() != 0) { fprintf(stderr, "FAIL: (6) empty pool ticked\n"); fail = 1; }
    if (!fail) printf("  (6) PASS: empty pool ticks nothing\n");

    if (fail) { fprintf(stderr, "\nESP-VM TEST FAILED\n"); return 1; }
    printf("\nPASS: ESP effect-script VM (C1) — pool + FUN_8003f0a0 dispatch byte-true\n");
    return 0;
}
