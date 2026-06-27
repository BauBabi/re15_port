/*
 * Unit tests for the SCD VM (Phase 4.4).
 *
 * Each test sets up a tiny bytecode program, runs scd_vm_tick() one or
 * more times, and asserts thread state evolves correctly.
 */
#include <stdio.h>
#include <string.h>
#include "re15_scd.h"

extern char g_last_dbgtext[];
extern int  g_dbgtext_call_count;

#define TEST(cond, msg)                                                \
    do {                                                                \
        if (!(cond)) {                                                  \
            fprintf(stderr, "FAIL %s:%d %s — %s\n",                     \
                    __FILE__, __LINE__, __func__, msg);                 \
            return 1;                                                   \
        }                                                               \
    } while (0)

/* ---- Test 1: Nop + Evt_end terminates thread ---------------------------- */
static int test_nop_evt_end(void)
{
    scd_vm_init();
    static const uint8_t code[] = { SCD_OP_NOP, SCD_OP_NOP, SCD_OP_EVT_END };
    scd_thread_start(0, code);
    TEST(g_scd.threads[0].active == 1, "thread should be active after start");

    scd_vm_tick();
    TEST(g_scd.threads[0].active == 0, "Evt_end should terminate (depth was 0)");
    TEST(g_scd.tick_count == 1, "tick_count incremented");
    return 0;
}

/* ---- Test 2: Sleep yields multiple ticks -------------------------------- */
static int test_sleep_yields(void)
{
    scd_vm_init();
    /* [Sleep][Sleeping][3][Evt_end] — sleep 3 ticks then terminate */
    static const uint8_t code[] = {
        SCD_OP_SLEEP, SCD_OP_SLEEPING, 0x03, 0x00, SCD_OP_EVT_END
    };
    scd_thread_start(0, code);

    /* Tick 1: Sleep runs (advances PC), then Sleeping runs (counter=3 → 2, yield) */
    scd_vm_tick();
    TEST(g_scd.threads[0].active == 1, "still alive after 1 tick");

    scd_vm_tick();
    TEST(g_scd.threads[0].active == 1, "still alive after 2 ticks");

    /* Tick 3: counter 1 → 0, advances PC to Evt_end but YIELDS (op_sleeping returns
     * 2). Sleep(N) yields EXACTLY N ticks — the opcode after Sleep fires on tick
     * N+1, NOT the tick the counter hits 0 (byte-true LAB_8003f428, BO-round). */
    scd_vm_tick();
    TEST(g_scd.threads[0].active == 1, "still alive after 3 ticks (Sleep yields N)");

    /* Tick 4: Evt_end runs → thread terminates. */
    scd_vm_tick();
    TEST(g_scd.threads[0].active == 0, "Evt_end reached on tick N+1");
    return 0;
}

/* ---- Test 3: Goto loops forever ---------------------------------------- */
static int test_goto_loops(void)
{
    scd_vm_init();
    /* [Sleep][Sleeping][1][Goto -4] — Goto is 6 bytes [op f0 f1 _pad rel_lo rel_hi];
     * the signed rel lives at pc[4..5] LE (pc[1..2] are ignored flag fields). */
    static const uint8_t code[] = {
        SCD_OP_SLEEP, SCD_OP_SLEEPING, 0x01, 0x00,        /* +0..+3 */
        SCD_OP_GOTO, 0x00, 0x00, 0x00, 0xFC, 0xFF         /* +4..+9  rel=-4 → back to +0 */
    };
    scd_thread_start(0, code);

    /* Run many ticks; thread should never die */
    for (int i = 0; i < 100; i++) {
        scd_vm_tick();
        TEST(g_scd.threads[0].active == 1, "Goto loop should keep thread alive");
    }
    TEST(g_scd.tick_count == 100, "100 ticks executed");
    return 0;
}

/* ---- Test 4: Gosub + Return ------------------------------------------- */
static int test_gosub_return(void)
{
    scd_vm_init();
    /* [Gosub +5][Evt_end][padding][Evt_end][Return]
     * Gosub jumps to offset +5 (= the second Evt_end).
     * The +5 lands on... let me layout:
     *   +0  Gosub
     *   +3  Evt_end (return point after Gosub)
     *   +4  padding
     *   +5  Return    ← Gosub target should execute this
     *
     * That gives: Gosub → push +3 → jump +5 → Return → pop → run +3 (Evt_end) → terminate
     */
    static const uint8_t code[] = {
        SCD_OP_GOSUB, 0x05, 0x00,     /* +0: Gosub +5 */
        SCD_OP_EVT_END,                /* +3: return target */
        SCD_OP_NOP,                    /* +4: padding */
        SCD_OP_RETURN                  /* +5: subroutine body (just returns) */
    };
    scd_thread_start(0, code);

    scd_vm_tick();
    /* After tick: Gosub→Return→Evt_end all execute in one tick (each returns 1) */
    TEST(g_scd.threads[0].active == 0, "Gosub+Return+Evt_end finishes in one tick");
    return 0;
}

/* ---- Test 5: Unknown opcode kills thread (safe-fail) -------------------- */
static int test_unknown_op_kills(void)
{
    scd_vm_init();
    static const uint8_t code[] = { 0x7F };   /* Unknown */
    scd_thread_start(0, code);

    scd_vm_tick();
    TEST(g_scd.threads[0].active == 0, "unknown opcode should kill thread");
    return 0;
}

/* ---- Test 6: Multiple threads independent ------------------------------- */
static int test_multithread(void)
{
    scd_vm_init();
    static const uint8_t code_short[] = { SCD_OP_EVT_END };
    static const uint8_t code_loop[] = {
        SCD_OP_SLEEP, SCD_OP_SLEEPING, 0x05, 0x00,
        SCD_OP_GOTO, 0xFC, 0xFF
    };
    scd_thread_start(0, code_short);
    scd_thread_start(1, code_loop);

    scd_vm_tick();
    TEST(g_scd.threads[0].active == 0, "thread 0 terminated");
    TEST(g_scd.threads[1].active == 1, "thread 1 still alive in sleep");

    for (int i = 0; i < 30; i++) scd_vm_tick();
    TEST(g_scd.threads[1].active == 1, "thread 1 still alive after 30 more ticks");
    return 0;
}

/* ---- Test 7: DbgText opcode invokes re15_debug_text -------------------- */
static int test_dbgtext(void)
{
    scd_vm_init();
    g_dbgtext_call_count = 0;
    memset(g_last_dbgtext, 0, 64);

    /* [DbgText x=10 y=20 len=5 "HELLO"][Evt_end] */
    static const uint8_t code[] = {
        SCD_OP_DBG_TEXT, 10, 20, 5, 'H', 'E', 'L', 'L', 'O',
        SCD_OP_EVT_END
    };
    scd_thread_start(0, code);

    scd_vm_tick();
    TEST(g_dbgtext_call_count == 1, "re15_debug_text called once");
    TEST(strcmp(g_last_dbgtext, "HELLO") == 0, "text should be 'HELLO'");
    TEST(g_scd.threads[0].active == 0, "thread done after Evt_end");
    return 0;
}

/* ---- Test 8: scd_thread_kill marks for kill ----------------------------- */
static int test_thread_kill(void)
{
    scd_vm_init();
    static const uint8_t code[] = {
        SCD_OP_SLEEP, SCD_OP_SLEEPING, 0xFF, 0xFF,
        SCD_OP_GOTO, 0xFC, 0xFF
    };
    scd_thread_start(0, code);
    scd_vm_tick();
    TEST(g_scd.threads[0].active == 1, "alive");

    scd_thread_kill(0);
    scd_vm_tick();
    TEST(g_scd.threads[0].active == 0, "kill_pending caused next-tick termination");
    return 0;
}

/* ---- Test 9 (Phase 4.4.2): Cut_chg sets camera state -------------------- */
static int test_cut_chg(void)
{
    scd_vm_init();
    TEST(g_scd.cam_change_pending == 0, "initially no cam change");

    /* Cut_chg is 2 bytes [op, cam_id] (Java disasm line 231) — sets cam_id only;
     * cam_arg2/arg3 are forced 0. (Was a stale 4-byte expectation.) */
    static const uint8_t code[] = {
        SCD_OP_CUT_CHG, 0x07,
        SCD_OP_EVT_END
    };
    scd_thread_start(0, code);
    scd_vm_tick();

    TEST(g_scd.cam_change_pending == 1, "cam_change_pending after Cut_chg");
    TEST(g_scd.cam_id == 0x07, "cam_id");
    TEST(g_scd.cam_arg2 == 0, "cam_arg2 forced 0 (Cut_chg is 2 bytes)");
    TEST(g_scd.cam_arg3 == 0, "cam_arg3 forced 0");
    TEST(g_scd.threads[0].active == 0, "thread done after Evt_end");
    return 0;
}

/* ---- Test 10 (Phase 4.4.2): Message_on sets message state --------------- */
static int test_message_on(void)
{
    scd_vm_init();
    TEST(g_scd.message_active == 0, "no message initially");

    static const uint8_t code[] = {
        SCD_OP_MESSAGE_ON, 0x2A, 0x01, 0x00,
        SCD_OP_EVT_END
    };
    scd_thread_start(0, code);
    scd_vm_tick();

    TEST(g_scd.message_active == 1, "message active after opcode");
    TEST(g_scd.message_id == 0x2A, "message_id");
    TEST(g_scd.message_arg2 == 0x01, "message_arg2");
    return 0;
}

/* ---- Test 11 (Phase 4.4.2): Switch/Case dispatch ----------------------- */
static int test_switch_case(void)
{
    scd_vm_init();

    /* Switch reads work_vars[var]; Case is 6 bytes [op, _, skip(2 LE), value(2 LE)]:
     * on match pc+=6 (enter body); else pc += skip+4 (next Case/Default/EndSwitch).
     * work_vars[10]=1 → Case 0 skipped, Case 1 matched.
     *   +0  Switch var=10                 4 bytes [+0..+3]
     *   +4  Case value=0 skip=7           6 bytes [+4..+9]   no match → +8+7 = +15
     *   +10 DbgText 1 1 1 'X'             5 bytes [+10..+14] (case 0 body)
     *   +15 Case value=1 skip=0           6 bytes [+15..+20] MATCH → +21
     *   +21 DbgText 2 2 1 'Y'             5 bytes [+21..+25] (case 1 body)
     *   +26 EndSwitch                     2 bytes [+26..+27]
     *   +28 Evt_end                       1 byte  [+28] */
    static const uint8_t code[] = {
        SCD_OP_SWITCH,   10, 0, 0,                              /* +0  */
        SCD_OP_CASE,      0, 7, 0, 0, 0,                        /* +4   value=0 skip=7 → +15 */
        SCD_OP_DBG_TEXT,  1, 1, 1, 'X',                         /* +10 */
        SCD_OP_CASE,      0, 0, 0, 1, 0,                        /* +15  value=1 → MATCH → +21 */
        SCD_OP_DBG_TEXT,  2, 2, 1, 'Y',                         /* +21 */
        SCD_OP_END_SWITCH, 0,                                   /* +26 (2 bytes) */
        SCD_OP_EVT_END                                           /* +28 */
    };

    scd_thread_start(0, code);
    g_scd.work_vars[10] = 1;   /* select case 1 (byte-true: Switch reads work_vars) */

    g_dbgtext_call_count = 0;
    memset(g_last_dbgtext, 0, 64);

    scd_vm_tick();

    TEST(g_dbgtext_call_count == 1, "exactly one case body should run");
    TEST(g_last_dbgtext[0] == 'Y', "case 1 ('Y') should match");
    TEST(g_scd.threads[0].active == 0, "thread done");
    return 0;
}

/* ---- Test 12 (Phase 4.4.2): Switch with Default fallthrough ------------ */
static int test_switch_default(void)
{
    scd_vm_init();

    /* Layout:
     *   +0  Switch var=5  skip=??          4 bytes
     *   +4  Case 99 skip=+8 (to +12)       4 bytes — won't match
     *   +8  DbgText 1 1 1 'X'              5 bytes — skipped
     *   +13 Default                         1 byte
     *   +14 DbgText 2 2 1 'D'              5 bytes — should run
     *   +19 EndSwitch                       1 byte
     *   +20 Evt_end                         1 byte
     */
    /* Case=6 bytes [op,_,skip(2 LE),value(2 LE)]; Default=4 bytes; EndSwitch=2.
     *   +0  Switch var=5                  4 bytes [+0..+3]
     *   +4  Case value=99 skip=7          6 bytes [+4..+9]   no match → +8+7 = +15
     *   +10 DbgText 1 1 1 'X'             5 bytes [+10..+14] (skipped)
     *   +15 Default                       4 bytes [+15..+18]
     *   +19 DbgText 2 2 1 'D'             5 bytes [+19..+23] (runs)
     *   +24 EndSwitch                     2 bytes [+24..+25]
     *   +26 Evt_end                       1 byte  [+26] */
    static const uint8_t code[] = {
        SCD_OP_SWITCH,    5, 0, 0,                  /* +0  */
        SCD_OP_CASE,      0, 7, 0, 99, 0,           /* +4   value=99 skip=7 → +15 (Default) */
        SCD_OP_DBG_TEXT,  1, 1, 1, 'X',             /* +10 (skipped) */
        SCD_OP_DEFAULT,   0, 0, 0,                  /* +15 (4 bytes) */
        SCD_OP_DBG_TEXT,  2, 2, 1, 'D',             /* +19 (default body) */
        SCD_OP_END_SWITCH, 0,                       /* +24 (2 bytes) */
        SCD_OP_EVT_END                               /* +26 */
    };

    scd_thread_start(0, code);
    g_scd.work_vars[5] = 7;   /* doesn't match case 99 (byte-true: Switch reads work_vars) */

    g_dbgtext_call_count = 0;
    memset(g_last_dbgtext, 0, 64);

    scd_vm_tick();
    TEST(g_dbgtext_call_count == 1, "default body runs once");
    TEST(g_last_dbgtext[0] == 'D', "should be default");
    return 0;
}

/* ---- Test 13 (Phase 4.4.3): Se_on enqueues audio event ----------------- */
static int test_se_on(void)
{
    scd_vm_init();
    TEST(g_scd.audio_count == 0, "no audio events initially");

    /* Se_on bank=2 sample=5 vol=80 pan=64 extra=0 pos_x=100 pos_y=-50 pos_z=200 */
    static const uint8_t code[] = {
        SCD_OP_SE_ON,
        0x02, 0x05, 0x50, 0x40, 0x00,
        100, 0,                          /* pos_x = 100 */
        (uint8_t)-50 & 0xFF, 0xFF,       /* pos_y = -50 (LE signed) */
        (uint8_t)(200 & 0xFF), (uint8_t)((200 >> 8) & 0xFF),  /* pos_z = 200 */
        SCD_OP_EVT_END
    };
    scd_thread_start(0, code);
    scd_vm_tick();

    TEST(g_scd.audio_count == 1, "one event queued");
    TEST(g_scd.audio_total == 1, "lifetime total = 1");

    scd_audio_event_t evt;
    int got = scd_audio_queue_pop(&evt);
    TEST(got == 1, "pop should return 1");
    TEST(evt.kind == SCD_AUDIO_SE_ON, "kind is Se_on");
    TEST(evt.bank == 2, "bank");
    TEST(evt.sample_id == 5, "sample_id");
    TEST(evt.volume == 0x50, "volume");
    TEST(evt.pan == 0x40, "pan");
    TEST(evt.pos_x == 100, "pos_x");
    TEST(evt.pos_y == -50, "pos_y signed");
    TEST(evt.pos_z == 200, "pos_z");

    TEST(g_scd.audio_count == 0, "after pop, count back to 0");
    return 0;
}

/* ---- Test 14: Sce_bgmtbl_set (0x57) — RE1.5 is 4 bytes [op,b1,b2,b3] -------
 * (NOT retail-RE2's 8-byte form). Enqueues a BGMTBL_SET event with
 * bank=pc[1], sample_id=pc[2], raw_w0=pc[3]; NO volume field. */
static int test_sce_bgmtbl_set(void)
{
    scd_vm_init();
    static const uint8_t code[] = {
        SCD_OP_SCE_BGMTBL_SET, 0x03, 0x02, 0x04,
        SCD_OP_EVT_END
    };
    scd_thread_start(0, code);
    scd_vm_tick();

    TEST(g_scd.audio_count == 1, "one event queued");
    scd_audio_event_t evt;
    scd_audio_queue_pop(&evt);
    TEST(evt.kind == SCD_AUDIO_BGMTBL_SET, "kind");
    TEST(evt.bank == 0x03, "byte1 (table index)");
    TEST(evt.sample_id == 0x02, "byte2 (stage)");
    TEST(evt.raw_w0 == 0x04, "byte3 raw");
    return 0;
}

/* ---- Test 15: Xa_on (4 bytes) ----------------------------------------- */
static int test_xa_on(void)
{
    scd_vm_init();
    static const uint8_t code[] = {
        SCD_OP_XA_ON, 0x07, 0x70, 0x01,
        SCD_OP_EVT_END
    };
    scd_thread_start(0, code);
    scd_vm_tick();
    TEST(g_scd.audio_count == 1, "one event");
    scd_audio_event_t evt;
    scd_audio_queue_pop(&evt);
    TEST(evt.kind == SCD_AUDIO_XA_ON, "kind");
    TEST(evt.sample_id == 0x07, "track id");
    TEST(evt.volume == 0x70, "vol");
    return 0;
}

/* ---- Test 16: Se_vol (2 bytes) ---------------------------------------- */
static int test_se_vol(void)
{
    scd_vm_init();
    static const uint8_t code[] = {
        SCD_OP_SE_VOL, 0x55,
        SCD_OP_EVT_END
    };
    scd_thread_start(0, code);
    scd_vm_tick();
    TEST(g_scd.audio_count == 1, "one event");
    scd_audio_event_t evt;
    scd_audio_queue_pop(&evt);
    TEST(evt.kind == SCD_AUDIO_SE_VOL, "kind");
    TEST(evt.volume == 0x55, "volume scale");
    return 0;
}

/* ---- Test 17: queue ring buffer (overflow drops, FIFO ordering) -------- */
static int test_audio_queue_ring(void)
{
    scd_vm_init();

    /* Emit 20 Se_vol events (queue size = 16) — last 4 should drop */
    static const uint8_t one_se_vol[] = { SCD_OP_SE_VOL, 0xAA, SCD_OP_EVT_END };
    for (int i = 0; i < 20; i++) {
        scd_thread_start(0, one_se_vol);
        scd_vm_tick();
    }
    TEST(g_scd.audio_count == SCD_AUDIO_QUEUE_SIZE, "queue full at capacity");
    TEST(g_scd.audio_total == 20, "lifetime total = 20 (incl drops)");

    /* FIFO: pop should return in insertion order */
    scd_audio_event_t evt;
    int popped = 0;
    while (scd_audio_queue_pop(&evt)) {
        TEST(evt.kind == SCD_AUDIO_SE_VOL, "first-in events");
        popped++;
    }
    TEST(popped == SCD_AUDIO_QUEUE_SIZE, "popped exactly 16");
    TEST(g_scd.audio_count == 0, "empty after drain");
    return 0;
}

/* ---- Test 18 (2026-06-09): byte-true If block-stack — TRUE enters body ----
 * Predicate = Cmp(0x23) work_vars[0]==5. If(block_len=13) Cmp body EndIf Evt_end.
 * Layout: +0 If(4) · +4 Cmp(6) · +10 DbgText 'T'(5) · +15 EndIf(2) · +17 Evt_end. */
static int test_if_true_enters(void)
{
    scd_vm_init();
    static const uint8_t code[] = {
        SCD_OP_IF, 0x00, 13, 0,                       /* +0  block_end=+17 (past EndIf) */
        0x23, 0x00, 0x00, 0x00, 5, 0,                 /* +4  Cmp work_vars[0]==5 */
        SCD_OP_DBG_TEXT, 1, 1, 1, 'T',                /* +10 if-body */
        SCD_OP_END_IF, 0x00,                          /* +15 End_if = 2 bytes */
        SCD_OP_EVT_END                                /* +17 */
    };
    scd_thread_start(0, code);
    g_scd.work_vars[0] = 5;                           /* predicate TRUE */
    g_dbgtext_call_count = 0; memset(g_last_dbgtext, 0, 64);
    scd_vm_tick();
    TEST(g_dbgtext_call_count == 1, "if-true: body must run");
    TEST(g_last_dbgtext[0] == 'T', "if-true: body text 'T'");
    TEST(g_scd.threads[0].active == 0, "if-true: thread done at Evt_end");
    TEST(g_scd.threads[0].block_sp == 0, "if-true: block stack balanced");
    return 0;
}

/* ---- Test 19: byte-true If — FALSE skips body (predicate returns false) ---- */
static int test_if_false_skips(void)
{
    scd_vm_init();
    static const uint8_t code[] = {
        SCD_OP_IF, 0x00, 13, 0,
        0x23, 0x00, 0x00, 0x00, 5, 0,                 /* Cmp work_vars[0]==5 */
        SCD_OP_DBG_TEXT, 1, 1, 1, 'T',
        SCD_OP_END_IF, 0x00,                          /* End_if = 2 bytes */
        SCD_OP_EVT_END
    };
    scd_thread_start(0, code);
    g_scd.work_vars[0] = 0;                           /* predicate FALSE */
    g_dbgtext_call_count = 0; memset(g_last_dbgtext, 0, 64);
    scd_vm_tick();
    TEST(g_dbgtext_call_count == 0, "if-false: body must NOT run");
    TEST(g_scd.threads[0].active == 0, "if-false: jumps to block_end (Evt_end)");
    TEST(g_scd.threads[0].block_sp == 0, "if-false: block stack balanced");
    return 0;
}

/* ---- Test 20: byte-true If/Else — TRUE runs if-body, FALSE runs else-body --
 * +0 If(4) · +4 Cmp(6) · +10 DbgText 'T'(5) · +15 Else(4) · +19 DbgText 'E'(5) · +24 Evt_end.
 * If block_end=+19 (else-body start); Else skip=9 → +24 on TRUE. */
static int test_if_else(void)
{
    static const uint8_t code[] = {
        SCD_OP_IF, 0x00, 15, 0,                       /* +0  block_end=+19 (else-body) */
        0x23, 0x00, 0x00, 0x00, 5, 0,                 /* +4  Cmp work_vars[0]==5 */
        SCD_OP_DBG_TEXT, 1, 1, 1, 'T',                /* +10 if-body */
        SCD_OP_ELSE, 0x00, 9, 0,                      /* +15 skip else-body → +24 */
        SCD_OP_DBG_TEXT, 2, 2, 1, 'E',                /* +19 else-body */
        SCD_OP_EVT_END                                /* +24 */
    };
    /* TRUE → 'T' */
    scd_vm_init();
    scd_thread_start(0, code);
    g_scd.work_vars[0] = 5;
    g_dbgtext_call_count = 0; memset(g_last_dbgtext, 0, 64);
    scd_vm_tick();
    TEST(g_dbgtext_call_count == 1, "if/else TRUE: exactly one body");
    TEST(g_last_dbgtext[0] == 'T', "if/else TRUE: 'T'");
    TEST(g_scd.threads[0].block_sp == 0, "if/else TRUE: balanced");
    /* FALSE → 'E' */
    scd_vm_init();
    scd_thread_start(0, code);
    g_scd.work_vars[0] = 0;
    g_dbgtext_call_count = 0; memset(g_last_dbgtext, 0, 64);
    scd_vm_tick();
    TEST(g_dbgtext_call_count == 1, "if/else FALSE: exactly one body");
    TEST(g_last_dbgtext[0] == 'E', "if/else FALSE: else-body 'E'");
    TEST(g_scd.threads[0].block_sp == 0, "if/else FALSE: balanced");
    return 0;
}

/* ---- Test 21: nested If — outer TRUE, inner FALSE → only outer body runs ----
 * +0 If_o(4) +4 Cmp v0==1(6) +10 If_i(4) +14 Cmp v1==1(6) +20 DbgText 'I'(5)
 * +25 EndIf_i(2) +27 DbgText 'O'(5) +32 EndIf_o(2) +34 Evt_end. */
static int test_nested_if(void)
{
    static const uint8_t code[] = {
        SCD_OP_IF, 0x00, 30, 0,                       /* +0  outer, block_end=+34 */
        0x23, 0x00, 0x00, 0x00, 1, 0,                 /* +4  Cmp v0==1 */
        SCD_OP_IF, 0x00, 13, 0,                       /* +10 inner, block_end=+27 */
        0x23, 0x01, 0x00, 0x00, 1, 0,                 /* +14 Cmp v1==1 */
        SCD_OP_DBG_TEXT, 1, 1, 1, 'I',                /* +20 inner body */
        SCD_OP_END_IF, 0x00,                          /* +25 inner EndIf (2 bytes) → +27 */
        SCD_OP_DBG_TEXT, 2, 2, 1, 'O',                /* +27 outer body */
        SCD_OP_END_IF, 0x00,                          /* +32 outer EndIf (2 bytes) → +34 */
        SCD_OP_EVT_END                                /* +34 */
    };
    /* outer TRUE, inner FALSE → only 'O' */
    scd_vm_init();
    scd_thread_start(0, code);
    g_scd.work_vars[0] = 1; g_scd.work_vars[1] = 0;
    g_dbgtext_call_count = 0; memset(g_last_dbgtext, 0, 64);
    scd_vm_tick();
    TEST(g_dbgtext_call_count == 1, "nested: only outer body runs");
    TEST(g_last_dbgtext[0] == 'O', "nested: outer 'O', inner skipped");
    TEST(g_scd.threads[0].block_sp == 0, "nested: block stack balanced");
    /* outer FALSE → nothing */
    scd_vm_init();
    scd_thread_start(0, code);
    g_scd.work_vars[0] = 0; g_scd.work_vars[1] = 1;
    g_dbgtext_call_count = 0; memset(g_last_dbgtext, 0, 64);
    scd_vm_tick();
    TEST(g_dbgtext_call_count == 0, "nested outer-false: nothing runs");
    TEST(g_scd.threads[0].block_sp == 0, "nested outer-false: balanced");
    return 0;
}

/* ---- Test 22 (2026-06-09): Cmp (0x23) — all 7 operators on the work-var array.
 * Runs `if (Cmp work_vars[0] OP imm) { DbgText } EndIf` and returns 1 if the body
 * ran (predicate TRUE) else 0. Exercises the byte-true work-var compare. */
static int run_cmp_if(int16_t var_val, uint8_t op, int16_t imm)
{
    uint8_t code[18];
    int n = 0;
    code[n++] = SCD_OP_IF; code[n++] = 0; code[n++] = 13; code[n++] = 0;   /* block_end past EndIf */
    code[n++] = 0x23; code[n++] = 0; code[n++] = 0; code[n++] = op;        /* Cmp var=0 op */
    code[n++] = (uint8_t)(imm & 0xff); code[n++] = (uint8_t)((imm >> 8) & 0xff);
    code[n++] = SCD_OP_DBG_TEXT; code[n++] = 1; code[n++] = 1; code[n++] = 1; code[n++] = 'Y';
    code[n++] = SCD_OP_END_IF; code[n++] = 0;
    code[n++] = SCD_OP_EVT_END;
    scd_vm_init();
    scd_thread_start(0, code);
    g_scd.work_vars[0] = var_val;
    g_dbgtext_call_count = 0;
    scd_vm_tick();              /* code[] stays valid for this synchronous tick */
    return g_dbgtext_call_count;
}

static int test_cmp_operators(void)
{
    TEST(run_cmp_if(5, 0, 5) == 1, "Cmp == : 5==5 true");
    TEST(run_cmp_if(4, 0, 5) == 0, "Cmp == : 4==5 false");
    TEST(run_cmp_if(6, 1, 5) == 1, "Cmp >  : 6>5 true");
    TEST(run_cmp_if(5, 1, 5) == 0, "Cmp >  : 5>5 false");
    TEST(run_cmp_if(4, 2, 5) == 1, "Cmp <  : 4<5 true");
    TEST(run_cmp_if(5, 2, 5) == 0, "Cmp <  : 5<5 false");
    TEST(run_cmp_if(5, 3, 5) == 1, "Cmp <= : 5<=5 true");
    TEST(run_cmp_if(6, 3, 5) == 0, "Cmp <= : 6<=5 false");
    TEST(run_cmp_if(5, 4, 5) == 1, "Cmp >= : 5>=5 true");
    TEST(run_cmp_if(4, 4, 5) == 0, "Cmp >= : 4>=5 false");
    TEST(run_cmp_if(4, 5, 5) == 1, "Cmp != : 4!=5 true");
    TEST(run_cmp_if(5, 5, 5) == 0, "Cmp != : 5!=5 false");
    TEST(run_cmp_if(0x06, 6, 0x02) == 1, "Cmp bittest: 0x06 & 0x02 true");
    TEST(run_cmp_if(0x04, 6, 0x02) == 0, "Cmp bittest: 0x04 & 0x02 false");
    return 0;
}

/* ---- Test 23: For/Next — the body runs exactly `count` times. -------------
 * +0 For(block_len=7, count=3)[6] · +6 DbgText 'L'[5] · +11 Next[2] · +13 Evt_end. */
static int test_for_next(void)
{
    scd_vm_init();
    static const uint8_t code[] = {
        0x0D, 0, 7, 0, 3, 0,        /* 0x0D=For */             /* +0  count=3 (block_len=7 = body+Next, used only if count==0) */
        SCD_OP_DBG_TEXT, 1, 1, 1, 'L',         /* +6  loop body */
        0x0E, 0x00,                            /* +11 Next (0x0E, 2 bytes) */
        SCD_OP_EVT_END                          /* +13 */
    };
    scd_thread_start(0, code);
    g_dbgtext_call_count = 0;
    scd_vm_tick();
    TEST(g_dbgtext_call_count == 3, "For(3): body runs exactly 3 times");
    TEST(g_scd.threads[0].active == 0, "For/Next: thread done at Evt_end");
    TEST(g_scd.threads[0].for_depth == 0, "For/Next: loop stack balanced");

    /* count=0 → loop skipped entirely (block_len jumps past body+Next). */
    scd_vm_init();
    static const uint8_t code0[] = {
        0x0D, 0, 7, 0, 0, 0,        /* 0x0D=For */             /* +0 count=0 → skip 6+7=+13 */
        SCD_OP_DBG_TEXT, 1, 1, 1, 'L',         /* +6 (skipped) */
        0x0E, 0x00,                            /* +11 Next (skipped) */
        SCD_OP_EVT_END                          /* +13 */
    };
    scd_thread_start(0, code0);
    g_dbgtext_call_count = 0;
    scd_vm_tick();
    TEST(g_dbgtext_call_count == 0, "For(0): body skipped entirely");
    TEST(g_scd.threads[0].active == 0, "For(0): falls through to Evt_end");
    return 0;
}

int main(void)
{
    int fails = 0;
    fails += test_nop_evt_end();
    fails += test_sleep_yields();
    fails += test_goto_loops();
    fails += test_gosub_return();
    fails += test_unknown_op_kills();
    fails += test_multithread();
    fails += test_dbgtext();
    fails += test_thread_kill();
    /* Phase 4.4.2 */
    fails += test_cut_chg();
    fails += test_message_on();
    fails += test_switch_case();
    fails += test_switch_default();
    /* Phase 4.4.3 */
    fails += test_se_on();
    fails += test_sce_bgmtbl_set();
    fails += test_xa_on();
    fails += test_se_vol();
    fails += test_audio_queue_ring();
    /* 2026-06-09: byte-true If/Else/End_if block-stack condition model */
    fails += test_if_true_enters();
    fails += test_if_false_skips();
    fails += test_if_else();
    fails += test_nested_if();
    fails += test_cmp_operators();
    fails += test_for_next();

    if (fails == 0) {
        printf("test_scd: all 23 tests PASSED\n");
        return 0;
    } else {
        printf("test_scd: %d test(s) FAILED\n", fails);
        return 1;
    }
}
