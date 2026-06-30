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

/* High opcode indices (0xF0+) so these SYNTHETIC test ops do not collide with the real ported
 * opcodes 0x01-0x08 (which re15_espvm_reset now installs). They exist only to drive the dispatch
 * loop's three return paths; they are not real @0x800744a8 semantics. */
#define OP_CONT     0xF0   /* advance pc by 1, return CONT (keep executing this frame)           */
#define OP_YIELD    0xF1   /* return YIELD (drives the sub-stack pop path)                        */
#define OP_STOP     0xF2   /* return STOP (instance done this frame; stays active)               */
#define OP_DESPAWN  0xF3   /* clear active flag, return STOP (despawn)                            */

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

/* ============================================================================================
 * Phase ESP-C2 batch 1 — the REAL ported opcodes 0x00..0x08. These run the actual installed
 * handlers (re15_espvm_reset installs them) against hand-built bytecode. 0xFF is a synthetic HALT
 * sentinel (returns STOP without touching pc) used to freeze execution right after the op under
 * test, so its exact byte effects can be asserted.
 * ============================================================================================ */

static int op_halt(re15_espvm_instance_t *inst) { (void)inst; return RE15_ESPVM_RET_STOP; }

static uint8_t s_rcode[160];
static void build_rcode(void)
{
    for (int i = 0; i < 160; i++) s_rcode[i] = 0;
    /* u16 offset table, ids 0..7 */
    s_rcode[0x0]=0x20; s_rcode[0x2]=0x30; s_rcode[0x4]=0x40; s_rcode[0x6]=0x50;
    s_rcode[0x8]=0x60; s_rcode[0xa]=0x70; s_rcode[0xc]=0x80; s_rcode[0xe]=0x90;
    s_rcode[0x20]=0x00; s_rcode[0x21]=0x02;                       /* id0: nop, wait        */
    s_rcode[0x22]=0xFF;                                           /* halt (7c saved-pc target) */
    s_rcode[0x30]=0x01;                                          /* id1: end              */
    s_rcode[0x40]=0x05; s_rcode[0x41]=0x03; s_rcode[0x42]=0xFF;   /* id2: kill inst3, halt */
    s_rcode[0x50]=0x04; s_rcode[0x51]=0x05; s_rcode[0x53]=0x00; s_rcode[0x54]=0xFF; /* id3: spawn slot5 id0, halt */
    s_rcode[0x60]=0x06; s_rcode[0x62]=0x10; s_rcode[0x64]=0xFF;   /* id4: loop-push 0x10, halt */
    s_rcode[0x70]=0x07; s_rcode[0x72]=0x08; s_rcode[0x78]=0xFF;   /* id5: loop-jump +8 -> halt */
    s_rcode[0x80]=0x08; s_rcode[0x82]=0xFF;                       /* id6: loop-pop -> +2 halt  */
    s_rcode[0x90]=0x03; s_rcode[0x93]=0x01;                       /* id7: jump-script to id1   */
}

static int run_real_op_tests(void)
{
    int fail = 0;
    build_rcode();
    printf("  --- ESP-C2 batch 1: real opcodes 0x00..0x08 ---\n");

    /* (7a) op0 nop (CONT) then op2 wait (STOP): pc += 2, instance stays active. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *t = re15_espvm_alloc(0, 0, s_rcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(t) != 0x22 || t->mem[RE15_ESPVM_OFF_ACTIVE] != 1) {
        fprintf(stderr, "FAIL: (7a) op0/op2 pc=0x%x active=%d\n",
                re15_espvm_get_pc(t), t->mem[RE15_ESPVM_OFF_ACTIVE]); fail = 1; }

    /* (7b) op1 end at level 0: deactivate. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_alloc(0, 1, s_rcode);
    re15_espvm_run_all();
    if (re15_espvm_active_count() != 0) {
        fprintf(stderr, "FAIL: (7b) op1 end level0 -> active=%d\n", re15_espvm_active_count()); fail = 1; }

    /* (7c) op1 end at level>0: pop one level (restore pc/level/sp). Set level=1, saved pc[0]=0x22,
     * loop-depth[0]=0 -> sp restore = 0*0x20 + 0xc0 + 0*4 + 4 = 0xc4. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *p = re15_espvm_alloc(0, 1, s_rcode);
    p->mem[RE15_ESPVM_OFF_LEVEL] = 1;
    wr_u32(p->mem + RE15_ESPVM_OFF_RETPC, 0x22);
    p->mem[RE15_ESPVM_OFF_DEPTH0] = 0;
    re15_espvm_run_all();
    if (p->mem[RE15_ESPVM_OFF_LEVEL] != 0 || re15_espvm_get_pc(p) != 0x22 ||
        rd_u32(p->mem + RE15_ESPVM_OFF_SP) != 0xc4) {
        fprintf(stderr, "FAIL: (7c) op1 pop level: level=%d pc=0x%x sp=0x%x\n",
                p->mem[RE15_ESPVM_OFF_LEVEL], re15_espvm_get_pc(p),
                rd_u32(p->mem + RE15_ESPVM_OFF_SP)); fail = 1; }

    /* (7d) op4 spawn: instance 0 spawns instance 5 (id0); the same-frame walker then ticks the
     * fresh instance (byte-true: FUN_8003f0a0 iterates 0..9) -> its [nop,wait] runs to pc 0x22. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_alloc(0, 3, s_rcode);
    re15_espvm_run_all();
    re15_espvm_instance_t *sp5 = re15_espvm_instance(5);
    if (sp5->mem[RE15_ESPVM_OFF_ACTIVE] != 1 || re15_espvm_get_pc(sp5) != 0x22) {
        fprintf(stderr, "FAIL: (7d) op4 spawn: inst5 active=%d pc=0x%x\n",
                sp5->mem[RE15_ESPVM_OFF_ACTIVE], re15_espvm_get_pc(sp5)); fail = 1; }

    /* (7e) op5 kill: instance 0 deactivates instance 3 (pre-spawned). */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_alloc(3, 0, s_rcode);          /* victim */
    re15_espvm_alloc(0, 2, s_rcode);          /* killer (id2 -> kill inst3) */
    re15_espvm_run_all();
    if (re15_espvm_instance(3)->mem[RE15_ESPVM_OFF_ACTIVE] != 0) {
        fprintf(stderr, "FAIL: (7e) op5 kill: inst3 still active\n"); fail = 1; }

    /* (7f) op6 loop-push: push (pc+4 + 0x10), depth[0] 0xff->0x00, sp 0xc0->0xc4, pc->0x64. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *l = re15_espvm_alloc(0, 4, s_rcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(l) != 0x64 || l->mem[RE15_ESPVM_OFF_DEPTH0] != 0x00 ||
        rd_u32(l->mem + RE15_ESPVM_OFF_STACK) != (0x64 + 0x10) ||
        rd_u32(l->mem + RE15_ESPVM_OFF_SP) != 0xc4) {
        fprintf(stderr, "FAIL: (7f) op6 push: pc=0x%x depth=0x%x stack=0x%x sp=0x%x\n",
                re15_espvm_get_pc(l), l->mem[RE15_ESPVM_OFF_DEPTH0],
                rd_u32(l->mem + RE15_ESPVM_OFF_STACK), rd_u32(l->mem + RE15_ESPVM_OFF_SP)); fail = 1; }

    /* (7g) op7 loop-jump: with sp=0xc4 depth[0]=0 -> sp 0xc0, pc 0x70+8=0x78, depth 0xff. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *j = re15_espvm_alloc(0, 5, s_rcode);
    wr_u32(j->mem + RE15_ESPVM_OFF_SP, 0xc4);
    j->mem[RE15_ESPVM_OFF_DEPTH0] = 0;
    re15_espvm_run_all();
    if (re15_espvm_get_pc(j) != 0x78 || rd_u32(j->mem + RE15_ESPVM_OFF_SP) != 0xc0 ||
        j->mem[RE15_ESPVM_OFF_DEPTH0] != 0xff) {
        fprintf(stderr, "FAIL: (7g) op7 jump: pc=0x%x sp=0x%x depth=0x%x\n",
                re15_espvm_get_pc(j), rd_u32(j->mem + RE15_ESPVM_OFF_SP),
                j->mem[RE15_ESPVM_OFF_DEPTH0]); fail = 1; }

    /* (7h) op8 loop-pop: with sp=0xc4 depth[0]=0 -> sp 0xc0, pc 0x80+2=0x82, depth 0xff. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *q = re15_espvm_alloc(0, 6, s_rcode);
    wr_u32(q->mem + RE15_ESPVM_OFF_SP, 0xc4);
    q->mem[RE15_ESPVM_OFF_DEPTH0] = 0;
    re15_espvm_run_all();
    if (re15_espvm_get_pc(q) != 0x82 || rd_u32(q->mem + RE15_ESPVM_OFF_SP) != 0xc0 ||
        q->mem[RE15_ESPVM_OFF_DEPTH0] != 0xff) {
        fprintf(stderr, "FAIL: (7h) op8 pop: pc=0x%x sp=0x%x depth=0x%x\n",
                re15_espvm_get_pc(q), rd_u32(q->mem + RE15_ESPVM_OFF_SP),
                q->mem[RE15_ESPVM_OFF_DEPTH0]); fail = 1; }

    /* (7i) op3 jump-script: re-seed THIS instance to id1's script (0x30), which is op1 end ->
     * deactivate. Proves the re-seed (pc landed at 0x30 + instance deactivated). */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *r = re15_espvm_alloc(0, 7, s_rcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(r) != 0x30 || r->mem[RE15_ESPVM_OFF_ACTIVE] != 0) {
        fprintf(stderr, "FAIL: (7i) op3 jump-script: pc=0x%x active=%d\n",
                re15_espvm_get_pc(r), r->mem[RE15_ESPVM_OFF_ACTIVE]); fail = 1; }

    if (!fail) printf("  (7) PASS: real opcodes 0x00..0x08 byte-true "
                      "(nop/wait/end/jump/spawn/kill/loop-push/jump/pop)\n");
    return fail;
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
        a->mem[RE15_ESPVM_OFF_DEPTH0] != 0xff || a->mem[RE15_ESPVM_OFF_CIDX0] != 0xff ||
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

    /* (7) the real ported opcodes 0x00..0x08. */
    if (run_real_op_tests()) fail = 1;

    if (fail) { fprintf(stderr, "\nESP-VM TEST FAILED\n"); return 1; }
    printf("\nPASS: ESP effect-script VM (C1) — pool + FUN_8003f0a0 dispatch byte-true\n");
    return 0;
}
