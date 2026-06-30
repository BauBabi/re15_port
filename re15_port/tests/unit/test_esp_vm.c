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
#include "re15_actor.h"

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
    s_fcode[0x14]=0xc0; s_fcode[0x16]=0xd0;   /* id10 GOSUB main, id11 sub-script */
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
    /* id10 @0xc0: op18 GOSUB to id11, halt@0xc2 (the return lands here); id11 @0xd0: op1 END */
    s_fcode[0xc0]=0x18; s_fcode[0xc1]=0x0b; s_fcode[0xc2]=0xFF;
    s_fcode[0xd0]=0x01;
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

    /* (9i) op18 GOSUB + op1 RETURN round-trip: main GOSUBs id11 (level 0->1), the sub's END pops
     * back (level 1->0) to the saved return pc 0xc2. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *gs = re15_espvm_alloc(0, 10, s_fcode);
    re15_espvm_run_all();
    if (gs->mem[RE15_ESPVM_OFF_LEVEL] != 0 || re15_espvm_get_pc(gs) != 0xc2) {
        fprintf(stderr, "FAIL: (9i) GOSUB/RETURN level=%d pc=0x%x (want 0/0xc2)\n",
                gs->mem[RE15_ESPVM_OFF_LEVEL], re15_espvm_get_pc(gs)); fail = 1; }

    if (!fail) printf("  (9) PASS: FOR-loop ops 0x0d..0x1a + GOSUB 0x18 byte-true "
                      "(begin/next[=run-body-N-times]/loop-back/break/pop-ci/skips/block/pop-level/gosub-return)\n");
    return fail;
}

/* ESP-C2 batch 4 — the two instance-local field ops 0x2f / 0x31. */
static uint8_t s_xcode[48];
static void build_xcode(void)
{
    for (int i = 0; i < 48; i++) s_xcode[i] = 0;
    s_xcode[0x0]=0x20; s_xcode[0x2]=0x30;
    s_xcode[0x20]=0x2f; s_xcode[0x21]=0x03; s_xcode[0x22]=0x34; s_xcode[0x23]=0x12; s_xcode[0x24]=0xFF; /* op2f id3=0x1234 */
    s_xcode[0x30]=0x31; s_xcode[0x31]=0xFF;                                                            /* op31 integrate */
}

static int run_field_op_tests(void)
{
    int fail = 0;
    build_xcode();
    printf("  --- ESP-C2 batch 4: instance-local field ops 0x2f / 0x31 ---\n");

    /* (10a) op2f: inst[0x158 + 3*2 = 0x15e] = 0x1234; pc += 4 -> 0x24. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *s = re15_espvm_alloc(0, 0, s_xcode);
    re15_espvm_run_all();
    if (rd_u16(s->mem + 0x15e) != 0x1234 || re15_espvm_get_pc(s) != 0x24) {
        fprintf(stderr, "FAIL: (10a) op2f field=0x%x pc=0x%x\n",
                rd_u16(s->mem + 0x15e), re15_espvm_get_pc(s)); fail = 1; }

    /* (10b) op31: six accumulators (+0x158..+0x162) += six deltas (+0x164..+0x16e); pc+=1. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *g = re15_espvm_alloc(0, 1, s_xcode);
    const uint16_t acc0[6] = {100, 200, 300, 400, 500, 600};
    const uint16_t dlt[6]  = {1, 2, 3, 4, 5, 6};
    for (int i = 0; i < 6; i++) {
        g->mem[0x158 + i*2] = (uint8_t)acc0[i]; g->mem[0x159 + i*2] = (uint8_t)(acc0[i] >> 8);
        g->mem[0x164 + i*2] = (uint8_t)dlt[i];  g->mem[0x165 + i*2] = (uint8_t)(dlt[i] >> 8);
    }
    re15_espvm_run_all();
    for (int i = 0; i < 6; i++) {
        uint16_t got = rd_u16(g->mem + 0x158 + i*2);
        if (got != (uint16_t)(acc0[i] + dlt[i])) {
            fprintf(stderr, "FAIL: (10b) op31 acc[%d]=%u expected %u\n", i, got, acc0[i]+dlt[i]); fail = 1; }
    }
    if (re15_espvm_get_pc(g) != 0x31) { fprintf(stderr, "FAIL: (10b) op31 pc=0x%x\n", re15_espvm_get_pc(g)); fail = 1; }

    if (!fail) printf("  (10) PASS: field ops 0x2f (u16-set) / 0x31 (position integrator) byte-true\n");
    return fail;
}

/* ESP-C2 batch 5 — the VM register file (DAT_800b0fd0) ops 0x1b/0x23/0x24/0x25/0x26/0x27. */
static uint8_t s_gcode[160];
static void build_gcode(void)
{
    for (int i = 0; i < 160; i++) s_gcode[i] = 0;
    s_gcode[0x0]=0x20; s_gcode[0x2]=0x30; s_gcode[0x4]=0x40; s_gcode[0x6]=0x50;
    s_gcode[0x8]=0x60; s_gcode[0xa]=0x70; s_gcode[0xc]=0x80;
    s_gcode[0x20]=0x24; s_gcode[0x21]=0x03; s_gcode[0x22]=0x34; s_gcode[0x23]=0x12; s_gcode[0x24]=0xFF; /* op24 reg[3]=0x1234 */
    s_gcode[0x30]=0x25; s_gcode[0x31]=0x03; s_gcode[0x32]=0x05; s_gcode[0x33]=0xFF;                      /* op25 reg[3]=reg[5] */
    s_gcode[0x40]=0x23; s_gcode[0x46]=0xF4; s_gcode[0x47]=0xFF;                                          /* op23; tick; halt (w/rhs patched) */
    s_gcode[0x60]=0x26; s_gcode[0x66]=0xFF;                                                              /* op26 (w/val patched) */
    s_gcode[0x70]=0x27; s_gcode[0x74]=0xFF;                                                              /* op27 (op/dst/src patched) */
    s_gcode[0x80]=0x1b; s_gcode[0x82]=0x06; s_gcode[0x85]=0x04;                                          /* op1b rel=6, count=reg[4] */
    s_gcode[0x86]=0xF4; s_gcode[0x87]=0x0e; s_gcode[0x89]=0xFF;                                          /* body tick; NEXT; halt */
}

static int arith_case(int op, uint16_t initial, int16_t val, uint16_t expected)
{
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(7, initial);
    uint16_t w = (uint16_t)((7 << 8) | op);
    s_gcode[0x62]=(uint8_t)w; s_gcode[0x63]=(uint8_t)(w>>8);
    s_gcode[0x64]=(uint8_t)val; s_gcode[0x65]=(uint8_t)((uint16_t)val>>8);
    re15_espvm_alloc(0, 4, s_gcode);   /* id4 @0x60 = op26 */
    re15_espvm_run_all();
    uint16_t got = re15_espvm_reg_get(7);
    if (got != expected) {
        fprintf(stderr, "FAIL: arith op%x %u,%d -> 0x%x (want 0x%x)\n", op, initial, val, got, expected); return 1; }
    return 0;
}

static int compare_case(int sel, int16_t lhs, int16_t rhs, int expect_cont)
{
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt); re15_espvm_set_opcode(0xF4, op_tick);
    re15_espvm_reg_set(2, (uint16_t)lhs);
    uint16_t w = (uint16_t)((sel << 8) | 2);            /* id=2, selector=sel */
    s_gcode[0x42]=(uint8_t)w; s_gcode[0x43]=(uint8_t)(w>>8);
    s_gcode[0x44]=(uint8_t)rhs; s_gcode[0x45]=(uint8_t)((uint16_t)rhs>>8);
    re15_espvm_alloc(0, 2, s_gcode);   /* id2 @0x40 = op23 then tick */
    s_tick = 0;
    re15_espvm_run_all();
    /* CONT -> the tick after op23 runs; YIELD -> frame ends (depth 0xff) and tick does NOT run. */
    if (s_tick != (expect_cont ? 1 : 0)) {
        fprintf(stderr, "FAIL: cmp sel%d %d?%d tick=%d (want cont=%d)\n", sel, lhs, rhs, s_tick, expect_cont); return 1; }
    return 0;
}

static int run_reg_op_tests(void)
{
    int fail = 0;
    build_gcode();
    printf("  --- ESP-C2 batch 5: VM register ops 0x1b/0x23/0x24/0x25/0x26/0x27 ---\n");

    /* (11a) op24 set-imm + op25 copy. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_alloc(0, 0, s_gcode); re15_espvm_run_all();
    if (re15_espvm_reg_get(3) != 0x1234) { fprintf(stderr, "FAIL: (11a) op24 reg[3]=0x%x\n", re15_espvm_reg_get(3)); fail = 1; }
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(5, 0xABCD);
    re15_espvm_alloc(0, 1, s_gcode); re15_espvm_run_all();
    if (re15_espvm_reg_get(3) != 0xABCD) { fprintf(stderr, "FAIL: (11a) op25 reg[3]=0x%x\n", re15_espvm_reg_get(3)); fail = 1; }

    /* (11b) all 12 arithmetic ops (FUN_80040140) via op26. */
    fail |= arith_case(0x0, 100, 5, 105);          /* add */
    fail |= arith_case(0x1, 100, 30, 70);          /* sub */
    fail |= arith_case(0x2, 6, 7, 42);             /* mul */
    fail |= arith_case(0x3, 100, 7, 14);           /* div */
    fail |= arith_case(0x4, 100, 7, 2);            /* mod */
    fail |= arith_case(0x5, 0x00f0, 0x0f0f, 0x0fff); /* or */
    fail |= arith_case(0x6, 0x0ff0, 0x0f0f, 0x0f00); /* and */
    fail |= arith_case(0x7, 0x0ff0, 0x0f0f, 0x00ff); /* xor */
    fail |= arith_case(0x8, 0x0f0f, 0, 0xf0f0);    /* not */
    fail |= arith_case(0x9, 0x0001, 4, 0x0010);    /* shl */
    fail |= arith_case(0xa, 0x8000, 4, 0x0800);    /* shr logical */
    fail |= arith_case(0xb, 0x8000, 4, 0xf800);    /* shr arithmetic */
    fail |= arith_case(0x3, 50, 0, 50);            /* div by 0 -> unchanged (orig traps) */

    /* (11c) op27 reg-arith-reg: reg[3] *= reg[5]. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(3, 8); re15_espvm_reg_set(5, 2);
    s_gcode[0x71]=0x02; s_gcode[0x72]=0x03; s_gcode[0x73]=0x05;   /* op=mul dst=3 src=5 */
    re15_espvm_alloc(0, 5, s_gcode); re15_espvm_run_all();
    if (re15_espvm_reg_get(3) != 16) { fprintf(stderr, "FAIL: (11c) op27 reg[3]=%u\n", re15_espvm_reg_get(3)); fail = 1; }

    /* (11d) op23 compare — all 7 selectors, true and false. */
    fail |= compare_case(0, 10, 10, 1); fail |= compare_case(0, 10, 11, 0);  /* == */
    fail |= compare_case(1, 11, 10, 1); fail |= compare_case(1, 10, 10, 0);  /* >  */
    fail |= compare_case(2, 10, 10, 1); fail |= compare_case(2, 9, 10, 0);   /* >= */
    fail |= compare_case(3, 9, 10, 1);  fail |= compare_case(3, 10, 10, 0);  /* <  */
    fail |= compare_case(4, 10, 10, 1); fail |= compare_case(4, 11, 10, 0);  /* <= */
    fail |= compare_case(5, 10, 11, 1); fail |= compare_case(5, 10, 10, 0);  /* != */
    fail |= compare_case(6, 0x0f, 0x03, 1); fail |= compare_case(6, 0x10, 0x0f, 0); /* & test */

    /* (11e) op1b FOR-begin with count from reg[4]: body runs reg[4] times. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt); re15_espvm_set_opcode(0xF4, op_tick);
    re15_espvm_reg_set(4, 4);
    re15_espvm_instance_t *fb = re15_espvm_alloc(0, 6, s_gcode);
    s_tick = 0;
    re15_espvm_run_all();
    if (s_tick != 4 || re15_espvm_get_pc(fb) != 0x89) {
        fprintf(stderr, "FAIL: (11e) op1b ticks=%d pc=0x%x (want 4/0x89)\n", s_tick, re15_espvm_get_pc(fb)); fail = 1; }

    if (!fail) printf("  (11) PASS: VM register ops byte-true "
                      "(set/copy/12 arith/7 compare/FOR-begin-from-register)\n");
    return fail;
}

/* ESP-C2 batch 7 — the conditional block evaluators 0x0f/0x12/0x13 (predicates are op23). */
static uint8_t s_kcode[160];
static void build_kcode(void)
{
    for (int i = 0; i < 160; i++) s_kcode[i] = 0;
    s_kcode[0x0]=0x20; s_kcode[0x2]=0x30; s_kcode[0x4]=0x50; s_kcode[0x6]=0x70;
    /* id0 @0x20: op12 [0x12,len=6, op23(6B) @0x22], s3=0x28(false halt); PCARR[0] poked -> 0x40(true halt) */
    s_kcode[0x20]=0x12; s_kcode[0x21]=0x06; s_kcode[0x22]=0x23; /* op23 operands (w/rhs) patched per test */
    s_kcode[0x28]=0xFF; s_kcode[0x40]=0xFF;
    /* id1 @0x30: op0f [0x0f,len=6,rel=0x10, op23(6B) @0x34], s3=0x3a(block/true halt); TGTARR=0x34+0x10=0x44(false halt) */
    s_kcode[0x30]=0x0f; s_kcode[0x31]=0x06; s_kcode[0x32]=0x10; s_kcode[0x34]=0x23;
    s_kcode[0x3a]=0xFF; s_kcode[0x44]=0xFF;
    /* id2 @0x50: op13 [0x13,reg=5,dr=0], caseA @0x54 [0,_,skip=8,val=10], body@0x5a halt, end 0x15 @0x5c */
    s_kcode[0x50]=0x13; s_kcode[0x51]=0x05;
    s_kcode[0x56]=0x08; s_kcode[0x58]=0x0a;   /* caseA: skip=8, val=10 */
    s_kcode[0x5a]=0xFF; s_kcode[0x5c]=0x15; s_kcode[0x5e]=0xFF; /* body halt; end marker; post-switch halt */
    /* id3 @0x70: op13 [0x13,reg=5,dr=0], caseA @0x74 [0,_,skip=8,val=10], body@0x7a halt, default 0x16 @0x7c */
    s_kcode[0x70]=0x13; s_kcode[0x71]=0x05;
    s_kcode[0x76]=0x08; s_kcode[0x78]=0x0a;
    s_kcode[0x7a]=0xFF; s_kcode[0x7c]=0x16; s_kcode[0x7e]=0xFF;  /* body halt; default marker; post-switch halt */
}

/* patch the op23 predicate at `off` (selector sel, register id 2, immediate rhs). */
static void patch_pred(int off, int sel, int16_t rhs)
{
    uint16_t w = (uint16_t)((sel << 8) | 2);
    s_kcode[off+2]=(uint8_t)w; s_kcode[off+3]=(uint8_t)(w>>8);
    s_kcode[off+4]=(uint8_t)rhs; s_kcode[off+5]=(uint8_t)((uint16_t)rhs>>8);
}

static int run_cond_op_tests(void)
{
    int fail = 0;
    build_kcode();
    printf("  --- ESP-C2 batch 7: conditional evaluators 0x0f/0x12/0x13 ---\n");

    /* (12a) op12 TRUE: reg[2]=10 >= 5 -> loop back to PCARR[0]=0x40. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(2, 10); patch_pred(0x22, 2, 5);   /* sel 2 = >= */
    re15_espvm_instance_t *c1 = re15_espvm_alloc(0, 0, s_kcode);
    c1->mem[RE15_ESPVM_OFF_CIDX0] = 0; wr_u32(c1->mem + 0x20, 0x40);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(c1) != 0x40 || c1->mem[RE15_ESPVM_OFF_CIDX0] != 0) {
        fprintf(stderr, "FAIL: (12a) op12 TRUE pc=0x%x ci=0x%x\n", re15_espvm_get_pc(c1), c1->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }

    /* (12b) op12 FALSE: reg[2]=10 < 5 -> ci-- (0->0xff), pc falls through to s3=0x28. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(2, 10); patch_pred(0x22, 3, 5);   /* sel 3 = < */
    re15_espvm_instance_t *c2 = re15_espvm_alloc(0, 0, s_kcode);
    c2->mem[RE15_ESPVM_OFF_CIDX0] = 0; wr_u32(c2->mem + 0x20, 0x40);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(c2) != 0x28 || c2->mem[RE15_ESPVM_OFF_CIDX0] != 0xff) {
        fprintf(stderr, "FAIL: (12b) op12 FALSE pc=0x%x ci=0x%x\n", re15_espvm_get_pc(c2), c2->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }

    /* (12c) op0f TRUE: reg[2]=10 >= 5 -> enter block (pc=s3=0x3a); ci 0xff->0, PCARR[0]=0x30, TGTARR[0]=0x44. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(2, 10); patch_pred(0x34, 2, 5);
    re15_espvm_instance_t *c3 = re15_espvm_alloc(0, 1, s_kcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(c3) != 0x3a || c3->mem[RE15_ESPVM_OFF_CIDX0] != 0 ||
        rd_u32(c3->mem + 0x20) != 0x30 || rd_u32(c3->mem + 0x60) != 0x44) {
        fprintf(stderr, "FAIL: (12c) op0f TRUE pc=0x%x ci=0x%x P=0x%x T=0x%x\n",
                re15_espvm_get_pc(c3), c3->mem[RE15_ESPVM_OFF_CIDX0], rd_u32(c3->mem+0x20), rd_u32(c3->mem+0x60)); fail = 1; }

    /* (12d) op0f FALSE: reg[2]=10 < 5 -> skip block (pc=TGTARR[0]=0x44), ci-- (0->0xff). */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(2, 10); patch_pred(0x34, 3, 5);
    re15_espvm_instance_t *c4 = re15_espvm_alloc(0, 1, s_kcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(c4) != 0x44 || c4->mem[RE15_ESPVM_OFF_CIDX0] != 0xff) {
        fprintf(stderr, "FAIL: (12d) op0f FALSE pc=0x%x ci=0x%x\n", re15_espvm_get_pc(c4), c4->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }

    /* (12e) op0f AND-chain: 2 predicates (10>=5 AND 10<20) -> true. Build a 2-pred chain at 0x34. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    /* chain: [op23 a (6B)] [flag u16 = 0 (AND)] [op23 b (6B)] = 14 bytes; len=0x0e, block s3=0x34+0xe=0x42 */
    s_kcode[0x31]=0x0e; s_kcode[0x3a]=0x00; /* relocate: new block at 0x42 */ s_kcode[0x42]=0xFF;
    s_kcode[0x34]=0x23; patch_pred(0x34, 2, 5);     /* 10 >= 5 */
    s_kcode[0x3a]=0x00; s_kcode[0x3b]=0x00;          /* AND flag */
    s_kcode[0x3c]=0x23; patch_pred(0x3c, 3, 20);     /* 10 < 20 */
    re15_espvm_reg_set(2, 10);
    re15_espvm_instance_t *c5 = re15_espvm_alloc(0, 1, s_kcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(c5) != 0x42) {
        fprintf(stderr, "FAIL: (12e) op0f AND-chain pc=0x%x (want 0x42 true)\n", re15_espvm_get_pc(c5)); fail = 1; }
    build_kcode();   /* restore */

    /* (12f) op13 switch match: reg[5]=10 -> caseA matches -> pc=case body 0x5a; ci 0xff->0. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(5, 10);
    re15_espvm_instance_t *sw1 = re15_espvm_alloc(0, 2, s_kcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(sw1) != 0x5a || sw1->mem[RE15_ESPVM_OFF_CIDX0] != 0) {
        fprintf(stderr, "FAIL: (12f) op13 match pc=0x%x ci=0x%x\n", re15_espvm_get_pc(sw1), sw1->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }

    /* (12g) op13 switch no-match -> 0x15 end: reg[5]=99 -> skip caseA -> end -> pc=0x5e, ci=0 (only ++). */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(5, 99);
    re15_espvm_instance_t *sw2 = re15_espvm_alloc(0, 2, s_kcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(sw2) != 0x5e || sw2->mem[RE15_ESPVM_OFF_CIDX0] != 0) {
        fprintf(stderr, "FAIL: (12g) op13 end pc=0x%x ci=0x%x\n", re15_espvm_get_pc(sw2), sw2->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }

    /* (12h) op13 no-match -> 0x16 default: reg[5]=99 -> default -> pc=0x7e, ci 0xff->0->0xff. */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(5, 99);
    re15_espvm_instance_t *sw3 = re15_espvm_alloc(0, 3, s_kcode);
    re15_espvm_run_all();
    if (re15_espvm_get_pc(sw3) != 0x7e || sw3->mem[RE15_ESPVM_OFF_CIDX0] != 0xff) {
        fprintf(stderr, "FAIL: (12h) op13 default pc=0x%x ci=0x%x\n", re15_espvm_get_pc(sw3), sw3->mem[RE15_ESPVM_OFF_CIDX0]); fail = 1; }

    if (!fail) printf("  (12) PASS: conditional evaluators byte-true "
                      "(op12 IF true/false, op0f block true/false/AND-chain, op13 switch match/end/default)\n");
    return fail;
}

/* ESP-C2 batch 8 — the work-target ops 0x2e (bind) / 0x32 (set position). */
static uint8_t s_wcode[48];

static int run_work_op_tests(void)
{
    int fail = 0;
    printf("  --- ESP-C2 batch 8: work-target ops 0x2e (bind) / 0x32 (set-pos) ---\n");

    /* (13a) bind to the player (kind 1) and set its position. */
    for (int i = 0; i < 48; i++) s_wcode[i] = 0;
    s_wcode[0]=0x20;
    s_wcode[0x20]=0x2e; s_wcode[0x21]=0x01; s_wcode[0x22]=0x00;     /* bind kind 1 (player slot 0) */
    s_wcode[0x23]=0x32; s_wcode[0x25]=100; s_wcode[0x27]=200;
    s_wcode[0x29]=(uint8_t)300; s_wcode[0x2a]=(uint8_t)(300>>8); s_wcode[0x2b]=0xFF;
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    g_actors[0].x = g_actors[0].y = g_actors[0].z = -1;
    re15_espvm_instance_t *w = re15_espvm_alloc(0, 0, s_wcode);
    re15_espvm_run_all();
    if (w->bound_slot != 0 || g_actors[0].x != 100 || g_actors[0].y != 200 || g_actors[0].z != 300 ||
        re15_espvm_get_pc(w) != 0x2b) {
        fprintf(stderr, "FAIL: (13a) player bind/set-pos slot=%d x=%d y=%d z=%d pc=0x%x\n",
                w->bound_slot, g_actors[0].x, g_actors[0].y, g_actors[0].z, re15_espvm_get_pc(w)); fail = 1; }

    /* (13b) bind to an entity slot (kind 2, index 3) and set its position. */
    for (int i = 0; i < 48; i++) s_wcode[i] = 0;
    s_wcode[0]=0x20;
    s_wcode[0x20]=0x2e; s_wcode[0x21]=0x02; s_wcode[0x22]=0x03;     /* bind kind 2 entity slot 3 */
    s_wcode[0x23]=0x32; s_wcode[0x25]=50; s_wcode[0x27]=60; s_wcode[0x29]=70; s_wcode[0x2b]=0xFF;
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    g_actors[3].x = g_actors[3].y = g_actors[3].z = -1;
    re15_espvm_instance_t *w2 = re15_espvm_alloc(0, 0, s_wcode);
    re15_espvm_run_all();
    if (w2->bound_slot != 3 || g_actors[3].x != 50 || g_actors[3].y != 60 || g_actors[3].z != 70) {
        fprintf(stderr, "FAIL: (13b) entity bind/set-pos slot=%d x=%d y=%d z=%d\n",
                w2->bound_slot, g_actors[3].x, g_actors[3].y, g_actors[3].z); fail = 1; }

    /* (13c) op2e clears the 6 pos/vel words @+0x158, and an unsupported kind (3) leaves bound_slot=-1. */
    for (int i = 0; i < 48; i++) s_wcode[i] = 0;
    s_wcode[0]=0x20;
    s_wcode[0x20]=0x2e; s_wcode[0x21]=0x03; s_wcode[0x22]=0x00; s_wcode[0x23]=0xFF;   /* kind 3 deferred */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_instance_t *w3 = re15_espvm_alloc(0, 0, s_wcode);
    for (int k = 0; k < 24; k++) w3->mem[0x158 + k] = 0xAB;   /* dirty the pos/vel region */
    re15_espvm_run_all();
    int cleared = 1; for (int k = 0; k < 24; k++) if (w3->mem[0x158 + k] != 0) cleared = 0;
    if (w3->bound_slot != -1 || !cleared) {
        fprintf(stderr, "FAIL: (13c) kind3 slot=%d cleared=%d\n", w3->bound_slot, cleared); fail = 1; }

    /* (13d) op33 set-rotation on the bound player. */
    for (int i = 0; i < 48; i++) s_wcode[i] = 0;
    s_wcode[0]=0x20;
    s_wcode[0x20]=0x2e; s_wcode[0x21]=0x01; s_wcode[0x22]=0x00;
    s_wcode[0x23]=0x33; s_wcode[0x25]=0x00; s_wcode[0x26]=0x04; /* rot_x=0x0400 */
    s_wcode[0x27]=0x00; s_wcode[0x28]=0x08;                     /* rot_y=0x0800 */
    s_wcode[0x29]=0x00; s_wcode[0x2a]=0x0c; s_wcode[0x2b]=0xFF; /* rot_z=0x0c00 */
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    g_actors[0].rot_x = g_actors[0].rot_y = g_actors[0].rot_z = -1;
    re15_espvm_alloc(0, 0, s_wcode); re15_espvm_run_all();
    if (g_actors[0].rot_x != 0x0400 || g_actors[0].rot_y != 0x0800 || g_actors[0].rot_z != 0x0c00) {
        fprintf(stderr, "FAIL: (13d) op33 rot=%d/%d/%d\n", g_actors[0].rot_x, g_actors[0].rot_y, g_actors[0].rot_z); fail = 1; }

    /* (13e) op42 init-state on the bound player: state=1, sub_state_1/2/3=0. */
    for (int i = 0; i < 48; i++) s_wcode[i] = 0;
    s_wcode[0]=0x20;
    s_wcode[0x20]=0x2e; s_wcode[0x21]=0x01; s_wcode[0x22]=0x00;
    s_wcode[0x23]=0x42; s_wcode[0x24]=0xFF;
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    g_actors[0].state = 9; g_actors[0].sub_state_1 = 9; g_actors[0].sub_state_2 = 9; g_actors[0].sub_state_3 = 9;
    re15_espvm_alloc(0, 0, s_wcode); re15_espvm_run_all();
    if (g_actors[0].state != 1 || g_actors[0].sub_state_1 != 0 ||
        g_actors[0].sub_state_2 != 0 || g_actors[0].sub_state_3 != 0) {
        fprintf(stderr, "FAIL: (13e) op42 state=%d sub=%d/%d/%d\n", g_actors[0].state,
                g_actors[0].sub_state_1, g_actors[0].sub_state_2, g_actors[0].sub_state_3); fail = 1; }

    /* (13f) op34 member-set immediate: member 4 (rot_y) = 0x0700. */
    for (int i = 0; i < 48; i++) s_wcode[i] = 0;
    s_wcode[0]=0x20;
    s_wcode[0x20]=0x2e; s_wcode[0x21]=0x01; s_wcode[0x22]=0x00;
    s_wcode[0x23]=0x34; s_wcode[0x24]=0x04; s_wcode[0x25]=0x00; s_wcode[0x26]=0x07; s_wcode[0x27]=0xFF;
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    g_actors[0].rot_y = -1;
    re15_espvm_alloc(0, 0, s_wcode); re15_espvm_run_all();
    if (g_actors[0].rot_y != 0x0700) { fprintf(stderr, "FAIL: (13f) op34 rot_y=%d\n", g_actors[0].rot_y); fail = 1; }

    /* (13g) op35 member-set from register: member 8 (state) = reg[6]=0x0234 -> state=(u8)0x34. */
    for (int i = 0; i < 48; i++) s_wcode[i] = 0;
    s_wcode[0]=0x20;
    s_wcode[0x20]=0x2e; s_wcode[0x21]=0x01; s_wcode[0x22]=0x00;
    s_wcode[0x23]=0x35; s_wcode[0x24]=0x08; s_wcode[0x25]=0x06; s_wcode[0x26]=0xFF;
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    re15_espvm_reg_set(6, 0x0234); g_actors[0].state = 0;
    re15_espvm_alloc(0, 0, s_wcode); re15_espvm_run_all();
    if (g_actors[0].state != 0x34) { fprintf(stderr, "FAIL: (13g) op35 state=0x%x\n", g_actors[0].state); fail = 1; }

    /* (13h) op43 anim_flags: SET 0x00f0, then OR 0x0f00 -> 0x0ff0, then XOR 0x00ff -> 0x0f0f. */
    for (int i = 0; i < 48; i++) s_wcode[i] = 0;
    s_wcode[0]=0x20;
    s_wcode[0x20]=0x2e; s_wcode[0x21]=0x01; s_wcode[0x22]=0x00;
    s_wcode[0x23]=0x43; s_wcode[0x24]=0x01; s_wcode[0x25]=0xf0; s_wcode[0x26]=0x00;   /* SET 0x00f0 */
    s_wcode[0x27]=0x43; s_wcode[0x28]=0x00; s_wcode[0x29]=0x00; s_wcode[0x2a]=0x0f;   /* OR  0x0f00 */
    s_wcode[0x2b]=0x43; s_wcode[0x2c]=0x02; s_wcode[0x2d]=0xff; s_wcode[0x2e]=0x00;   /* XOR 0x00ff */
    s_wcode[0x2f]=0xFF;
    re15_espvm_reset(); re15_espvm_set_opcode(0xFF, op_halt);
    g_actors[0].anim_flags = 0;
    re15_espvm_alloc(0, 0, s_wcode); re15_espvm_run_all();
    if (g_actors[0].anim_flags != 0x0f0f) { fprintf(stderr, "FAIL: (13h) op43 anim_flags=0x%x\n", g_actors[0].anim_flags); fail = 1; }

    if (!fail) printf("  (13) PASS: work-target ops byte-true "
                      "(bind/clear, set-pos/rot/state, member-set imm+reg, anim_flags)\n");
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
    /* (10) the instance-local field ops 0x2f / 0x31. */
    if (run_field_op_tests()) fail = 1;
    /* (11) the VM register ops 0x1b/0x23/0x24/0x25/0x26/0x27. */
    if (run_reg_op_tests()) fail = 1;
    /* (12) the conditional block evaluators 0x0f/0x12/0x13. */
    if (run_cond_op_tests()) fail = 1;
    /* (13) the work-target ops 0x2e / 0x32. */
    if (run_work_op_tests()) fail = 1;

    if (fail) { fprintf(stderr, "\nESP-VM TEST FAILED\n"); return 1; }
    printf("\nPASS: ESP effect-script VM (C1) — pool + FUN_8003f0a0 dispatch byte-true\n");
    return 0;
}
