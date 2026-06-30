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

static uint16_t rd_u16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
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

/* ESP-C2 batch 2 — the loop-counter ops 0x09..0x0c (counter array @inst+level*8+0xa0). */
static uint8_t s_ccode[96];
static void build_ccode(void)
{
    for (int i = 0; i < 96; i++) s_ccode[i] = 0;
    s_ccode[0x0]=0x20; s_ccode[0x2]=0x30; s_ccode[0x4]=0x40; s_ccode[0x6]=0x50;
    s_ccode[0x20]=0x09; s_ccode[0x21]=0x0a; s_ccode[0x22]=0x03; s_ccode[0x24]=0xFF; /* id0: wait 3 frames, halt */
    s_ccode[0x30]=0x09; s_ccode[0x31]=0xFF; s_ccode[0x32]=0x05;                      /* id1: isolated counter-set 5 */
    s_ccode[0x40]=0x0b; s_ccode[0x41]=0xFF;                                          /* id2: counter-push          */
    s_ccode[0x50]=0x0c; s_ccode[0x51]=0xFF;                                          /* id3: wait-condition        */
}

static int run_counter_op_tests(void)
{
    int fail = 0;
    build_ccode();
    printf("  --- ESP-C2 batch 2: loop-counter ops 0x09..0x0c ---\n");

    /* (8a) op9+opa = "wait 3 frames": frame1 sets counter[0]=3 then decrements to 2 (STOP);
     * frames 2,3 decrement; at 0 the loop skips pc+=3 (0x21->0x24) and pops the counter. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *w = re15_espvm_alloc(0, 0, s_ccode);
    re15_espvm_run_all();   /* frame 1 */
    if (rd_u16(w->mem + RE15_ESPVM_OFF_CARRAY) != 2 || re15_espvm_get_pc(w) != 0x21 ||
        w->mem[RE15_ESPVM_OFF_CIDX0] != 0x00) {
        fprintf(stderr, "FAIL: (8a) frame1 counter=%u pc=0x%x cidx=0x%x\n",
                rd_u16(w->mem + RE15_ESPVM_OFF_CARRAY), re15_espvm_get_pc(w),
                w->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }
    re15_espvm_run_all();   /* frame 2 -> counter 1 */
    re15_espvm_run_all();   /* frame 3 -> counter 0 -> skip + pop */
    if (re15_espvm_get_pc(w) != 0x24 || w->mem[RE15_ESPVM_OFF_CIDX0] != 0xff) {
        fprintf(stderr, "FAIL: (8a) after 3 frames pc=0x%x cidx=0x%x (want 0x24/0xff)\n",
                re15_espvm_get_pc(w), w->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }

    /* (8b) op9 isolated: cidx 0xff->0x00, counter[0]=5, pc->0x31. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *c = re15_espvm_alloc(0, 1, s_ccode);
    re15_espvm_run_all();
    if (rd_u16(c->mem + RE15_ESPVM_OFF_CARRAY) != 5 || c->mem[RE15_ESPVM_OFF_CIDX0] != 0x00 ||
        re15_espvm_get_pc(c) != 0x31) {
        fprintf(stderr, "FAIL: (8b) op9 counter=%u cidx=0x%x pc=0x%x\n",
                rd_u16(c->mem + RE15_ESPVM_OFF_CARRAY), c->mem[RE15_ESPVM_OFF_CIDX0],
                re15_espvm_get_pc(c)); fail = 1; }

    /* (8c) opb counter-push: cidx 5->6, pc->0x41. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *b = re15_espvm_alloc(0, 2, s_ccode);
    b->mem[RE15_ESPVM_OFF_CIDX0] = 0x05;
    re15_espvm_run_all();
    if (b->mem[RE15_ESPVM_OFF_CIDX0] != 0x06 || re15_espvm_get_pc(b) != 0x41) {
        fprintf(stderr, "FAIL: (8c) opb cidx=0x%x pc=0x%x\n",
                b->mem[RE15_ESPVM_OFF_CIDX0], re15_espvm_get_pc(b)); fail = 1; }

    /* (8d) opc wait-condition: gate clear -> advance (pc+1, cidx--); gate set -> stay. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_set_wait_gate(0);
    re15_espvm_instance_t *g = re15_espvm_alloc(0, 3, s_ccode);
    g->mem[RE15_ESPVM_OFF_CIDX0] = 0x03;
    re15_espvm_run_all();
    if (re15_espvm_get_pc(g) != 0x51 || g->mem[RE15_ESPVM_OFF_CIDX0] != 0x02) {
        fprintf(stderr, "FAIL: (8d) opc gate-clear pc=0x%x cidx=0x%x\n",
                re15_espvm_get_pc(g), g->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_set_wait_gate(1);
    re15_espvm_instance_t *g2 = re15_espvm_alloc(0, 3, s_ccode);
    g2->mem[RE15_ESPVM_OFF_CIDX0] = 0x03;
    re15_espvm_run_all();
    if (re15_espvm_get_pc(g2) != 0x50 || g2->mem[RE15_ESPVM_OFF_CIDX0] != 0x03) {
        fprintf(stderr, "FAIL: (8d) opc gate-set should stay pc=0x%x cidx=0x%x\n",
                re15_espvm_get_pc(g2), g2->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }
    re15_espvm_set_wait_gate(0);

    if (!fail) printf("  (8) PASS: loop-counter ops 0x09..0x0c byte-true "
                      "(set/wait[=wait-N-frames]/push/wait-cond)\n");
    return fail;
}

/* ESP-C2 batch 3 — the 2nd FOR-loop subsystem (0x0d..0x1a). 0xF4 = a synthetic body op that
 * counts its executions (pc+=1, CONT); 0xFF = HALT sentinel. */
static int s_tick;
static int op_tick(re15_espvm_instance_t *inst)
{
    s_tick++;
    re15_espvm_set_pc(inst, re15_espvm_get_pc(inst) + 1);
    return RE15_ESPVM_RET_CONT;
}

static uint8_t s_fcode[192];
static uint16_t rd16(const uint8_t *p) { return rd_u16(p); }
static void build_fcode(void)
{
    for (int i = 0; i < 192; i++) s_fcode[i] = 0;
    /* offset table ids 0..9 */
    s_fcode[0x0]=0x20; s_fcode[0x2]=0x30; s_fcode[0x4]=0x40; s_fcode[0x6]=0x50; s_fcode[0x8]=0x60;
    s_fcode[0xa]=0x70; s_fcode[0xc]=0x80; s_fcode[0xe]=0x90; s_fcode[0x10]=0xa0; s_fcode[0x12]=0xb0;
    /* id0 @0x20: op0d isolated (count=7, rel=4), halt at the post-pc 0x26 */
    s_fcode[0x20]=0x0d; s_fcode[0x22]=0x04; s_fcode[0x24]=0x07; s_fcode[0x26]=0xFF;
    /* id1 @0x30: full FOR loop (count=3, rel=6): op0d, body tick@0x36, NEXT@0x37, halt@0x39 */
    s_fcode[0x30]=0x0d; s_fcode[0x32]=0x06; s_fcode[0x34]=0x03;
    s_fcode[0x36]=0xF4; s_fcode[0x37]=0x0e; s_fcode[0x39]=0xFF;
    /* id2 @0x40: op11 FOR-begin-no-count (rel=5), halt@0x44 */
    s_fcode[0x40]=0x11; s_fcode[0x42]=0x05; s_fcode[0x44]=0xFF;
    /* id3 @0x50: op10 loop-back (PCARR target poked to 0x55), halt@0x55 */
    s_fcode[0x50]=0x10; s_fcode[0x55]=0xFF;
    /* id4 @0x60: op1a FOR-break (TGTARR poked to 0x66), halt@0x66 */
    s_fcode[0x60]=0x1a; s_fcode[0x66]=0xFF;
    /* id5 @0x70: op16 pop-ci (pc+=2 -> 0x72) */
    s_fcode[0x70]=0x16; s_fcode[0x72]=0xFF;
    /* id6 @0x80: op14 skip6 (pc+=6 -> 0x86) */
    s_fcode[0x80]=0x14; s_fcode[0x86]=0xFF;
    /* id7 @0x90: op15 skip2 (pc+=2 -> 0x92) */
    s_fcode[0x90]=0x15; s_fcode[0x92]=0xFF;
    /* id8 @0xa0: op17 block-setup (depth=2, ci=3, rel=8 -> pc 0xa8) */
    s_fcode[0xa0]=0x17; s_fcode[0xa1]=0x02; s_fcode[0xa2]=0x03; s_fcode[0xa4]=0x08; s_fcode[0xa8]=0xFF;
    /* id9 @0xb0: op19 pop-level (saved-pc poked to 0xb6), halt@0xb6 */
    s_fcode[0xb0]=0x19; s_fcode[0xb6]=0xFF;
}

static int run_for_op_tests(void)
{
    int fail = 0;
    build_fcode();
    printf("  --- ESP-C2 batch 3: FOR-loop ops 0x0d..0x1a ---\n");

    /* (9a) op0d isolated: ci 0xff->0, CARRAY[0]=7, TGTARR[0]=0x26+4=0x2a, PCARR[0]=0x26,
     * DEPTHARR[0]=loop-depth[0](=0xff init), pc=0x26. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *d = re15_espvm_alloc(0, 0, s_fcode);
    re15_espvm_run_all();
    if (d->mem[RE15_ESPVM_OFF_CIDX0] != 0x00 || rd16(d->mem + 0xa0) != 7 ||
        rd_u32(d->mem + 0x60) != 0x2a || rd_u32(d->mem + 0x20) != 0x26 ||
        d->mem[0x0c] != 0xff || re15_espvm_get_pc(d) != 0x26) {
        fprintf(stderr, "FAIL: (9a) op0d ci=0x%x C=%u T=0x%x P=0x%x D=0x%x pc=0x%x\n",
                d->mem[RE15_ESPVM_OFF_CIDX0], rd16(d->mem+0xa0), rd_u32(d->mem+0x60),
                rd_u32(d->mem+0x20), d->mem[0x0c], re15_espvm_get_pc(d)); fail = 1; }

    /* (9b) full FOR loop: op0d count=3 + tick body + op0e NEXT -> body runs exactly 3x in one
     * frame; counter consumed to 0, ci popped to 0xff, pc lands on the post-loop halt 0x39. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt); re15_espvm_set_opcode(0xF4, op_tick);
    re15_espvm_instance_t *L = re15_espvm_alloc(0, 1, s_fcode);
    s_tick = 0;
    re15_espvm_run_all();
    if (s_tick != 3 || re15_espvm_get_pc(L) != 0x39 || L->mem[RE15_ESPVM_OFF_CIDX0] != 0xff) {
        fprintf(stderr, "FAIL: (9b) FOR loop ticks=%d pc=0x%x ci=0x%x (want 3/0x39/0xff)\n",
                s_tick, re15_espvm_get_pc(L), L->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }

    /* (9c) op11 FOR-begin-no-count: ci->0, PCARR[0]=0x44, TGTARR[0]=0x44+5=0x49, no CARRAY write, pc=0x44. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *n = re15_espvm_alloc(0, 2, s_fcode);
    re15_espvm_run_all();
    if (n->mem[RE15_ESPVM_OFF_CIDX0] != 0x00 || rd_u32(n->mem + 0x20) != 0x44 ||
        rd_u32(n->mem + 0x60) != 0x49 || re15_espvm_get_pc(n) != 0x44) {
        fprintf(stderr, "FAIL: (9c) op11 ci=0x%x P=0x%x T=0x%x pc=0x%x\n",
                n->mem[RE15_ESPVM_OFF_CIDX0], rd_u32(n->mem+0x20), rd_u32(n->mem+0x60),
                re15_espvm_get_pc(n)); fail = 1; }

    /* (9d) op10 loop-back: ci=0, PCARR[0]=0x55 -> pc=0x55, ci->0xff. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *lb = re15_espvm_alloc(0, 3, s_fcode);
    lb->mem[RE15_ESPVM_OFF_CIDX0] = 0; wr_u32(lb->mem + 0x20, 0x55);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(lb) != 0x55 || lb->mem[RE15_ESPVM_OFF_CIDX0] != 0xff) {
        fprintf(stderr, "FAIL: (9d) op10 pc=0x%x ci=0x%x\n",
                re15_espvm_get_pc(lb), lb->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }

    /* (9e) op1a FOR-break: ci=0, TGTARR[0]=0x66, DEPTHARR[0]=0x42 -> pc=0x66, loop-depth[0]=0x42, ci->0xff. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *br = re15_espvm_alloc(0, 4, s_fcode);
    br->mem[RE15_ESPVM_OFF_CIDX0] = 0; wr_u32(br->mem + 0x60, 0x66); br->mem[0x0c] = 0x42;
    re15_espvm_run_all();
    if (re15_espvm_get_pc(br) != 0x66 || br->mem[RE15_ESPVM_OFF_DEPTH0] != 0x42 ||
        br->mem[RE15_ESPVM_OFF_CIDX0] != 0xff) {
        fprintf(stderr, "FAIL: (9e) op1a pc=0x%x depth=0x%x ci=0x%x\n",
                re15_espvm_get_pc(br), br->mem[RE15_ESPVM_OFF_DEPTH0],
                br->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }

    /* (9f) op16 pop-ci (ci 5->4, pc+=2); op14 skip6; op15 skip2. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *pc16 = re15_espvm_alloc(0, 5, s_fcode);
    pc16->mem[RE15_ESPVM_OFF_CIDX0] = 5;
    re15_espvm_run_all();
    if (pc16->mem[RE15_ESPVM_OFF_CIDX0] != 4 || re15_espvm_get_pc(pc16) != 0x72) {
        fprintf(stderr, "FAIL: (9f) op16 ci=0x%x pc=0x%x\n",
                pc16->mem[RE15_ESPVM_OFF_CIDX0], re15_espvm_get_pc(pc16)); fail = 1; }
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *s6 = re15_espvm_alloc(0, 6, s_fcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(s6) != 0x86) { fprintf(stderr, "FAIL: (9f) op14 pc=0x%x\n", re15_espvm_get_pc(s6)); fail = 1; }
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *s2 = re15_espvm_alloc(0, 7, s_fcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(s2) != 0x92) { fprintf(stderr, "FAIL: (9f) op15 pc=0x%x\n", re15_espvm_get_pc(s2)); fail = 1; }

    /* (9g) op17 block-setup: loop-depth[0]=2, counter-index[0]=3, sp=0xc0+2*4+4=0xcc, pc=0xa0+8=0xa8. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *bs = re15_espvm_alloc(0, 8, s_fcode);
    re15_espvm_run_all();
    if (bs->mem[RE15_ESPVM_OFF_DEPTH0] != 2 || bs->mem[RE15_ESPVM_OFF_CIDX0] != 3 ||
        rd_u32(bs->mem + RE15_ESPVM_OFF_SP) != 0xcc || re15_espvm_get_pc(bs) != 0xa8) {
        fprintf(stderr, "FAIL: (9g) op17 depth=0x%x ci=0x%x sp=0x%x pc=0x%x\n",
                bs->mem[RE15_ESPVM_OFF_DEPTH0], bs->mem[RE15_ESPVM_OFF_CIDX0],
                rd_u32(bs->mem + RE15_ESPVM_OFF_SP), re15_espvm_get_pc(bs)); fail = 1; }

    /* (9h) op19 pop-level: level 1->0, pc=saved-pc[0]=0xb6, loop-depth[0]=0 -> sp=0xc0+0+4=0xc4. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *pl = re15_espvm_alloc(0, 9, s_fcode);
    pl->mem[RE15_ESPVM_OFF_LEVEL] = 1; wr_u32(pl->mem + RE15_ESPVM_OFF_RETPC, 0xb6);
    pl->mem[RE15_ESPVM_OFF_DEPTH0] = 0;
    re15_espvm_run_all();
    if (pl->mem[RE15_ESPVM_OFF_LEVEL] != 0 || re15_espvm_get_pc(pl) != 0xb6 ||
        rd_u32(pl->mem + RE15_ESPVM_OFF_SP) != 0xc4) {
        fprintf(stderr, "FAIL: (9h) op19 level=%d pc=0x%x sp=0x%x\n",
                pl->mem[RE15_ESPVM_OFF_LEVEL], re15_espvm_get_pc(pl),
                rd_u32(pl->mem + RE15_ESPVM_OFF_SP)); fail = 1; }

    if (!fail) printf("  (9) PASS: FOR-loop ops 0x0d..0x1a byte-true "
                      "(begin/next[=run-body-N-times]/loop-back/break/pop-ci/skips/block/pop-level)\n");
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
    /* (8) the loop-counter ops 0x09..0x0c. */
    if (run_counter_op_tests()) fail = 1;
    /* (9) the FOR-loop ops 0x0d..0x1a. */
    if (run_for_op_tests()) fail = 1;

    if (fail) { fprintf(stderr, "\nESP-VM TEST FAILED\n"); return 1; }
    printf("\nPASS: ESP effect-script VM (C1) — pool + FUN_8003f0a0 dispatch byte-true\n");
    return 0;
}
