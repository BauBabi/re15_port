/**
 * @file test_scd_aot_opcodes.c
 * @brief Unit-Tests für SCD AOT- und Interaktions-Opcodes
 *
 * Testet die korrekte Ausführung der AOT-/Interaktions-Opcodes der SCD-VM:
 * Cut_chg (0x29), Aot_set (0x2C), Obj_model_set (0x2D), Se_on (0x36),
 * Sce_em_set (0x44), Item_aot_set (0x50) und Door_aot_set (0x3B).
 *
 * Requirements: 4.1, 4.6
 *
 * --- REAKTIVIERUNG 2026-06-28 (API-Transplant) ---
 * Der Engine-Umbau hat die öffentliche SCD-API geändert. Der Test wurde auf
 * die AKTUELLE API portiert; die Test-INTENTION ("jeder Opcode konsumiert die
 * richtige Byte-Anzahl und erzeugt seinen Seiteneffekt") bleibt erhalten.
 *
 * ALT -> NEU (verifiziert am echten Header/Quelltext):
 *   - g_scd.handlers[op](&g_scd,&thread)  ENTFERNT (kein öffentlicher
 *       Opcode-Dispatch mehr). Die Handler in engine/src/scd_vm.c sind `static`
 *       und nehmen nur `scd_thread_t*`. Statt dessen wird über die ÖFFENTLICHE
 *       API gefahren: scd_vm_init() + scd_thread_start(slot,pc) + scd_vm_tick().
 *   - condition_flag / scd_opcode_handler_t / scd_vm_t.handlers  ENTFERNT.
 *   - scd_thread_t.sleep_counter  -> sleep_count (re15_scd.h:56).
 *
 * Mechanismus (öffentliche API):
 *   Bytecode = [Opcode + Operanden][0x02 = Evt_next].
 *   - Der getestete Opcode läuft, advanced PC um seine Größe N, return 1
 *     (SCD_R_CONTINUE) -> der Dispatcher führt sofort den nächsten Opcode aus.
 *   - 0x02 (Evt_next, scd_vm.c op_evt_next): advanced PC um 1, return 2
 *     (SCD_R_YIELD) -> der Thread YIELDET und bleibt AKTIV.
 *   Nach einem scd_vm_tick() gilt daher:
 *       g_scd.threads[slot].pc == bytecode + N + 1
 *   => N (= konsumierte Bytes des getesteten Opcodes) ist nachprüfbar, der
 *   Thread lebt noch und der Seiteneffekt (g_scd.* / g_aot.*) ist gesetzt.
 *
 * Geänderte/erwartete Byte-Größen (alle byte-true aus scd_vm.c verifiziert,
 * NICHT mehr die Legacy-Werte):
 *   - Cut_chg   0x29 = 2  Bytes (op_cut_chg: t->pc+=2; cam_id=pc[1]) [war 4]
 *   - Aot_set   0x2C = 20 (op_aot_set: t->pc+=20)
 *   - Obj_model_set 0x2D = 34 (op_obj_model_set: t->pc+=34, byte-true
 *       LAB_80040914 +0x22) [Legacy-Test erwartete 22]
 *   - Se_on     0x36 = 12 (op_se_on: t->pc+=12)
 *   - Sce_em_set 0x44 = 20 (op_sce_em_set: t->pc+=20) [Legacy-Test erwartete 22]
 *   - Item_aot_set 0x50 = 22 (op_item_aot_set; t->pc+=22). item_type/amount sind LE
 *       u16 @+14/+16 -> das LOW byte pc[14]/pc[16] ist Typ/Menge (nicht pc[15]/pc[17]).
 *   - Door_aot_set 0x3B = 32 (op_door_aot_set: t->pc+=32). Der Legacy-Opcode
 *       0x68 "Door_aot_set_4p" hat keinen dedizierten Handler mehr (-> op_unknown,
 *       skippt nur s_opcode_sizes[0x68]=40 Bytes OHNE Tür zu installieren). Der
 *       AKTUELLE, bedeutungstragende Tür-Opcode ist 0x3B (installiert eine
 *       DOOR-AOT mit door_params). Daher testet dieser Fall jetzt 0x3B.
 */

#include <stdio.h>
#include <string.h>
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_audio.h"

/* Evt_next (0x02): 1-Byte-Yield-Opcode. Folgt jedem getesteten Opcode als
 * Sentinel, damit der Thread nach dem Tick AKTIV bleibt und seine PC lesbar
 * ist (siehe Dateikopf). scd_vm.c op_evt_next: t->pc+=1; return 2 (yield). */
#define OP_EVT_NEXT 0x02

/* =========================================================================
 * Test-Framework (minimal, inline)
 * ========================================================================= */

static int g_pass = 0;
static int g_fail = 0;

#define TEST_ASSERT_EQ(name, expected, actual) do {                          \
    long long _e = (long long)(expected);                                    \
    long long _a = (long long)(actual);                                      \
    if (_e != _a) {                                                          \
        printf("  FAIL [%s]: expected %lld, got %lld (%s:%d)\n",             \
               name, _e, _a, __FILE__, __LINE__);                            \
        g_fail++;                                                            \
        return;                                                              \
    }                                                                        \
} while(0)

#define TEST_OK(name) do { printf("  PASS: %s\n", name); g_pass++; } while(0)

/* Test-Slot (0 = Main-Thread; beliebig, solange frei). */
#define TEST_SLOT 0

/* =========================================================================
 * Helper: VM frisch initialisieren und EINEN Opcode ausführen
 * ========================================================================= */

static void setup_vm(void)
{
    scd_vm_init();      /* nullt g_scd, registriert Opcodes, ruft re15_aot_init() */
    re15_aot_init();    /* idempotent — explizit, damit die AOT-Slots garantiert leer sind */
}

/* Startet einen Thread auf `bytecode` und führt EINEN 30-Hz-Tick aus.
 * Erwartet, dass auf den getesteten Opcode ein OP_EVT_NEXT folgt, sodass der
 * Thread nach dem Tick yieldet (aktiv bleibt). Liefert die Anzahl der vom
 * getesteten Opcode konsumierten Bytes (= PC-Advance ohne das Evt_next-Byte)
 * oder -1, falls der Thread nicht (mehr) wie erwartet aktiv/geparkt ist. */
static long run_one_opcode(const uint8_t *bytecode)
{
    if (scd_thread_start(TEST_SLOT, bytecode) != 0) return -1;
    scd_vm_tick();
    const scd_thread_t *th = &g_scd.threads[TEST_SLOT];
    if (!th->active || th->pc == NULL) return -1;
    /* th->pc steht hinter dem Evt_next-Sentinel: pc = bytecode + N + 1. */
    long advanced = (long)(th->pc - bytecode);
    return advanced - 1;   /* das eine Evt_next-Byte abziehen -> N */
}

/* =========================================================================
 * Test: Cut_chg (0x29, 2 Bytes) — [op][cam_id]
 * ========================================================================= */

static void test_cut_chg(void)
{
    /* op_cut_chg (scd_vm.c:941): cam_id = pc[1]; cam_change_pending=1; pc+=2. */
    uint8_t bytecode[] = { 0x29, 0x05, OP_EVT_NEXT }; /* cam_id = 5 */
    long n;

    setup_vm();
    n = run_one_opcode(bytecode);

    TEST_ASSERT_EQ("Cut_chg: Opcode-Größe = 2", 2, n);
    TEST_ASSERT_EQ("Cut_chg: cam_change_pending", 1, g_scd.cam_change_pending);
    TEST_ASSERT_EQ("Cut_chg: cam_id", 5, g_scd.cam_id);

    TEST_OK("Cut_chg (0x29)");
}

/* =========================================================================
 * Test: Aot_set (0x2C, 20 Bytes)
 * ========================================================================= */

static void test_aot_set(void)
{
    /* op_aot_set (scd_vm.c:1706): slot=pc[1], type=pc[2], flags=pc[3],
     * rect (LE) @pc[6..13], eventId=pc[17]. eventId!=0/0xFF + flags-Bit 0x10
     * gesetzt => GENERIC-AOT in `slot` aktiv. pc+=20. */
    uint8_t bytecode[21] = {
        0x2C, 0x03, 0x03, 0x10,             /* slot=3, type=3, flags=0x10 (GENERIC-Action) */
        0x00, 0x00, 0x10, 0x00,             /* rect_x=16 (LE @6) */
        0x20, 0x00, 0x30, 0x00,             /* rect_z=32, rect_w=48 */
        0x40, 0x00, 0x00, 0x00,             /* rect_d=64; pc[14]=act */
        0x00, 0x01, 0x00, 0x00,             /* pc[17]=eventId=1 */
        OP_EVT_NEXT                          /* Sentinel */
    };
    long n;

    setup_vm();
    n = run_one_opcode(bytecode);

    TEST_ASSERT_EQ("Aot_set: Opcode-Größe = 20", 20, n);
    TEST_ASSERT_EQ("Aot_set: Slot 3 aktiv", 1, g_aot.slots[3].active);
    TEST_ASSERT_EQ("Aot_set: Typ GENERIC", RE15_AOT_TYPE_GENERIC, g_aot.slots[3].type);

    /* LONG form (pc[3]&0x80): a 4-VERTEX POLYGON @pc[6..21] + event fields shifted +8 (eventId
     * pc[17]->pc[25]). The ROOM1090-shaped quad (100,1300),(1850,2150),(3550,-2000),(1800,-2800) has
     * pc[17]=0xF8 (v2.z high byte) — the OLD code read that as the event (dead) and installed a garbage
     * rect from the first two vertices. Fixed: install the quad + read eventId at pc[25]. pc += 28. */
    uint8_t bc_poly[29] = {
        0x2C, 0x07, 0x03, 0x80,             /* slot=7, type=3 (EVENT, wie ROOM1090 @0x2336 `2C..03 B1`),
                                             * flags=0x80 (LONG, bit0x10 clear -> AUTO). (sce=1 ist jetzt
                                             * byte-true MESSAGE — wf_f536e1ee div #3 — separater Case.) */
        0x00, 0x00,                          /* band, pad */
        0x64, 0x00, 0x14, 0x05,             /* v0=(100,1300) */
        0x3A, 0x07, 0x66, 0x08,             /* v1=(1850,2150) */
        0xDE, 0x0D, 0x30, 0xF8,             /* v2=(3550,-2000)  <- pc[17]=0xF8 (old garbage event) */
        0x08, 0x07, 0x10, 0xF5,             /* v3=(1800,-2800) */
        0xFF, 0x00, 0x00, 0x02,             /* act=pc[22]=0xFF; eventId=pc[25]=2 */
        0x00, 0x00,                          /* pc[26..27] */
        OP_EVT_NEXT                          /* @pc[28] sentinel */
    };
    setup_vm();
    n = run_one_opcode(bc_poly);
    TEST_ASSERT_EQ("Aot_set LONG: Opcode-Größe = 28", 28, n);
    TEST_ASSERT_EQ("Aot_set LONG: Slot 7 aktiv", 1, g_aot.slots[7].active);
    TEST_ASSERT_EQ("Aot_set LONG: has_quad", 1, g_aot.slots[7].has_quad);
    TEST_ASSERT_EQ("Aot_set LONG: v0.x=100", 100, g_aot.slots[7].xs[0]);
    TEST_ASSERT_EQ("Aot_set LONG: v0.z=1300", 1300, g_aot.slots[7].zs[0]);
    TEST_ASSERT_EQ("Aot_set LONG: v2.x=3550", 3550, g_aot.slots[7].xs[2]);
    TEST_ASSERT_EQ("Aot_set LONG: v2.z=-2000", -2000, g_aot.slots[7].zs[2]);
    TEST_ASSERT_EQ("Aot_set LONG: eventId=2 (pc[25], not pc[17]=0xF8)", 2, g_aot.slots[7].event_id);
    TEST_ASSERT_EQ("Aot_set LONG: Typ AUTO_EVENT", RE15_AOT_TYPE_AUTO_EVENT, g_aot.slots[7].type);

    /* BYTE-TRUE ACTION GEOMETRY (wf_f536e1ee step 2): record[1] 0x40 = CENTRE test first
     * (stamps work_var[1]), 0x20 = FORWARD 620-point vs the EXACT rect (stamps work_var[0]).
     * A 0x21 zone (forward-only) must NOT fire from centre-inside; a 0x51 zone (centre-only)
     * must NOT fire from the forward position. */
    {
        setup_vm();
        /* forward-only examine (flags 0x21), rect centre (620,0) half (100,100): the player at
         * (0,0) rot 0 has his FORWARD point at (620,0) = inside; his CENTRE is outside. */
        uint8_t bc_fwd[21] = {
            0x2C, 0x02, 0x05, 0x21,  0x00, 0x00,
            0x08, 0x02, 0x00, 0x00,             /* rect_x=520 LE, rect_z=0 */
            0xC8, 0x00, 0xC8, 0x00,             /* w=200, d=200 -> centre (620,100)? no: cz=0+100 */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, OP_EVT_NEXT };
        /* use z centred: rect_z = -100 so cz = 0 */
        bc_fwd[8] = 0x9C; bc_fwd[9] = 0xFF;     /* rect_z = -100 LE */
        run_one_opcode(bc_fwd);
        g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
        g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y  = 0;
        g_scd.work_vars[0] = -1;
        g_aot_action_pressed = 1;
        re15_aot_scan(0, 0, 0xFF);              /* centre outside, forward (620,0) inside */
        TEST_ASSERT_EQ("geom 0x21: forward-620 hit stamps work_var[0]=slot", 2, g_scd.work_vars[0]);
        /* centre-only zone (0x51) at the same rect must NOT fire from this position */
        setup_vm();
        bc_fwd[3] = 0x51; bc_fwd[1] = 0x03;
        run_one_opcode(bc_fwd);
        g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
        g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y  = 0;
        g_scd.work_vars[1] = -1;
        g_aot_action_pressed = 1;
        re15_aot_scan(0, 0, 0xFF);
        TEST_ASSERT_EQ("geom 0x51: centre-only does NOT fire from the forward pos", -1, g_scd.work_vars[1]);
        re15_aot_scan(620, 0, 0xFF);            /* stand INSIDE -> centre hit */
        TEST_ASSERT_EQ("geom 0x51: centre hit stamps work_var[1]=slot", 3, g_scd.work_vars[1]);
    }

    /* sce=1 -> MESSAGE (div #3: 427 shipped examine zones were inert invalid-spawn DOORs) +
     * sce=4 -> FLAG_CHG (div #4) incl. the ENEMY-pool gating (0x42 zones: the player must NOT
     * set the flag; an enemy inside must). [wf_f536e1ee steps 3+4] */
    {
        setup_vm();
        uint8_t bc_msg[21] = {
            0x2C, 0x08, 0x01, 0x31,  0x00, 0x00,          /* sce=1, flags 0x31 (action+forward) */
            0x00, 0x00, 0x00, 0x00,  0xC8, 0x00, 0xC8, 0x00,
            0x07, 0x00, 0xFF, 0xFF,                        /* msg index 7; pause 0xFFFF (the old
                                                            * ev=0xFF heuristic trap) */
            0x00, 0x00, OP_EVT_NEXT };
        run_one_opcode(bc_msg);
        TEST_ASSERT_EQ("sce=1: installs as MESSAGE (not DOOR)", RE15_AOT_TYPE_MESSAGE, g_aot.slots[8].type);
        TEST_ASSERT_EQ("sce=1: msg index from payload u16@0", 7, g_aot.slots[8].event_id);

        setup_vm();
        uint8_t bc_flag[21] = {
            0x2C, 0x09, 0x04, 0x42,  0x80, 0x00,          /* sce=4, flags 0x42 = AUTO + ENEMY pool;
                                                            * band 0x80 = any floor */
            0x00, 0x00, 0x00, 0x00,  0xC8, 0x00, 0xC8, 0x00,   /* rect um (100,100) */
            0x05, 0x00, 0x21, 0x00,                        /* group 5, bit 0x21 */
            0x01, 0x00, OP_EVT_NEXT };                     /* on = 1 */
        run_one_opcode(bc_flag);
        TEST_ASSERT_EQ("sce=4: installs as FLAG_CHG", RE15_AOT_TYPE_FLAG_CHG, g_aot.slots[9].type);
        g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
        re15_aot_scan(100, 100, 0xFF);                     /* PLAYER inside the enemy-only zone */
        TEST_ASSERT_EQ("sce=4 0x42: the player does NOT set the enemy flag", 0, re15_game_flag_get(5, 0x21));
        g_actors[3].active = 1; g_actors[3].x = 100; g_actors[3].z = 100;   /* an enemy inside */
        re15_aot_scan(-5000, -5000, 0xFF);                 /* player far away */
        TEST_ASSERT_EQ("sce=4 0x42: an ENEMY inside sets the flag", 1, re15_game_flag_get(5, 0x21));
    }

    TEST_OK("Aot_set (0x2C)");
}

/* =========================================================================
 * Test: Obj_model_set (0x2D, 34 Bytes) — registriert eine Prop
 * ========================================================================= */

static void test_obj_model_set(void)
{
    /* op_obj_model_set (scd_vm.c:2393): pc[1]=obj_id, pc[2]=type, Pos/Rot/Box
     * (LE) @pc[10..33]; registriert g_scd.props[prop_count++]. pc+=34
     * (byte-true LAB_80040914 +0x22). */
    uint8_t bytecode[35];   /* 34 Opcode-Bytes + 1 Sentinel */
    long n;

    memset(bytecode, 0, sizeof(bytecode));
    bytecode[0]  = 0x2D;
    bytecode[1]  = 0x01;            /* obj_id = 1 */
    bytecode[2]  = 0x00;            /* render type = 0 */
    bytecode[34] = OP_EVT_NEXT;     /* Sentinel direkt hinter dem 34-Byte-Opcode */

    setup_vm();
    n = run_one_opcode(bytecode);

    TEST_ASSERT_EQ("Obj_model_set: Opcode-Größe = 34", 34, n);
    TEST_ASSERT_EQ("Obj_model_set: prop_count = 1", 1, g_scd.prop_count);
    TEST_ASSERT_EQ("Obj_model_set: Prop 0 aktiv", 1, g_scd.props[0].active);

    TEST_OK("Obj_model_set (0x2D)");
}

/* =========================================================================
 * Test: Se_on (0x36, 12 Bytes) — reiht Audio-Event in g_scd.audio_queue
 * ========================================================================= */

static void test_se_on(void)
{
    /* op_se_on (scd_vm.c:1128): bank=pc[1], sample_id=pc[2], vol=pc[3],
     * pan=pc[4]; enqueue_audio(SE_ON); pc+=12. Hängt NICHT von g_audio ab —
     * schreibt nur in g_scd.audio_queue. */
    uint8_t bytecode[13] = {
        0x36, 0x02, 0x0A, 0x7F,             /* bank=2, sample_id=10, vol=127 */
        0x40, 0x00, 0x00, 0x00,             /* pan=64, pos... */
        0x00, 0x00, 0x00, 0x00,             /* pos... */
        OP_EVT_NEXT                          /* Sentinel */
    };
    long n;

    setup_vm();
    n = run_one_opcode(bytecode);

    TEST_ASSERT_EQ("Se_on: Opcode-Größe = 12", 12, n);
    TEST_ASSERT_EQ("Se_on: ein Audio-Event eingereiht", 1, g_scd.audio_count);

    TEST_OK("Se_on (0x36)");
}

/* =========================================================================
 * Test: Sce_em_set (0x44, 20 Bytes) — spawnt Enemy/NPC
 * ========================================================================= */

static void test_sce_em_set(void)
{
    /* op_sce_em_set (scd_vm.c:2204): slot=pc[1], type=pc[2], behavior=pc[3],
     * Pos (LE) @pc[8..13], dir @pc[16]; spawnt Actor; pc+=20.
     * (Der Actor-Zustand liegt in re15_actor.h — hier nicht eingebunden —, daher
     * prüfen wir die Opcode-Größe als Verhaltensbeleg.) */
    uint8_t bytecode[21];
    long n;

    memset(bytecode, 0, sizeof(bytecode));
    bytecode[0]  = 0x44;
    bytecode[1]  = 0x01;            /* enemy slot */
    bytecode[2]  = 0x10;            /* type */
    bytecode[20] = OP_EVT_NEXT;     /* Sentinel */

    setup_vm();
    n = run_one_opcode(bytecode);

    TEST_ASSERT_EQ("Sce_em_set: Opcode-Größe = 20", 20, n);

    TEST_OK("Sce_em_set (0x44)");
}

/* =========================================================================
 * Test: Item_aot_set (0x50, 22 Bytes) — installiert ITEM-AOT
 * ========================================================================= */

static void test_item_aot_set(void)
{
    /* op_item_aot_set (0x4E = Legacy-Alias auf den 0x50-Handler): slot=pc[1], rect (LE) @pc[6..13],
     * item_type = LE LOW byte pc[14], amount = LE LOW byte pc[16] (u16-Felder; die High-Bytes
     * pc[15]/pc[17] sind 0x00 fuer jedes echte STAGE1-Item). Hier ein echtes STAGE1-Item: Handgun-
     * Ammo (0x15) x30 (0x1e), wie ROOM1050.RDT @0xb9a. Ein frueherer "BE-Fix" las pc[15]/pc[17] (=0)
     * -> jedes Pickup gewaehrte type 0 / amount 0 = nichts. pc+=22. */
    uint8_t bytecode[23] = {
        0x50, 0x05, 0x02, 0x00,             /* op 0x50 (Item_aot_set), slot=5, sce, sat (kein 0x80) */
        0x00, 0x00, 0x00, 0x01,             /* floor, super, rect_x (LE @6) */
        0x00, 0x02, 0x00, 0x03,             /* rect_z, rect_w */
        0x00, 0x04, 0x15, 0x00,             /* rect_d; item_type (LE) = 0x15 @pc[14] */
        0x1e, 0x00, 0x00, 0x00,             /* amount (LE) = 0x1e @pc[16]; action */
        0x00, 0x00,                          /* flags @pc[20..21] */
        OP_EVT_NEXT                          /* Sentinel */
    };
    long n;

    setup_vm();
    n = run_one_opcode(bytecode);

    TEST_ASSERT_EQ("Item_aot_set: Opcode-Größe = 22", 22, n);
    TEST_ASSERT_EQ("Item_aot_set: Slot 5 aktiv", 1, g_aot.slots[5].active);
    TEST_ASSERT_EQ("Item_aot_set: Typ ITEM", RE15_AOT_TYPE_ITEM, g_aot.slots[5].type);
    /* THE byte-read regression: item_type + amount come from the LE LOW bytes pc[14]/pc[16], NOT
     * the high bytes pc[15]/pc[17] (=0 = the old bug -> re15_inv_grant rejects -> pick up nothing). */
    TEST_ASSERT_EQ("Item_aot_set: item_type=0x15 (LE low byte pc[14])", 0x15, g_aot.item_params[5].item_type);
    TEST_ASSERT_EQ("Item_aot_set: amount=30 (LE low byte pc[16])", 0x1e, g_aot.item_params[5].amount);

    /* LONG form (pc[3]&0x80): the extended AOT inserts 8 bytes before the item fields, so type/amount
     * shift +8 to pc[22]/pc[24] (byte-true FUN @0x80040670-84: long record+26/+28 vs short +18/+20).
     * DECOY 0xAA/0xBB sit at the short offsets pc[14]/pc[16] — reading those (the bug) would grant
     * garbage; the real 0x15 x30 are at pc[22]/pc[24]. pc += 30. */
    uint8_t bc_long[31] = {
        0x50, 0x06, 0x02, 0x80,             /* op, slot=6, sce, sat=0x80 (LONG form) */
        0x00, 0x00, 0x00, 0x01,             /* floor, super, rect_x (LE @6) */
        0x00, 0x02, 0x00, 0x03,             /* rect_z, rect_w */
        0x00, 0x04, 0xAA, 0x00,             /* rect_d; DECOY @pc[14]=0xAA (must NOT be read) */
        0xBB, 0x00, 0x00, 0x00,             /* DECOY @pc[16]=0xBB; extended-AOT extra bytes */
        0x00, 0x00, 0x15, 0x00,             /* pc[20..21] extra; item_type (LE) = 0x15 @pc[22] */
        0x1e, 0x00, 0x00, 0x00,             /* amount (LE) = 0x1e @pc[24]; trailing */
        0x00, 0x00,                          /* pc[28..29] */
        OP_EVT_NEXT                          /* @pc[30] sentinel */
    };
    setup_vm();
    n = run_one_opcode(bc_long);
    TEST_ASSERT_EQ("Item_aot_set LONG: Opcode-Größe = 30", 30, n);
    TEST_ASSERT_EQ("Item_aot_set LONG: item_type=0x15 (pc[22], not decoy pc[14])", 0x15, g_aot.item_params[6].item_type);
    TEST_ASSERT_EQ("Item_aot_set LONG: amount=30 (pc[24], not decoy pc[16])", 0x1e, g_aot.item_params[6].amount);

    /* TAKEN-BIT persistence (byte-true @0x800406d4, catalog wf_f536e1ee): the installer
     * re-checks flag zone 9 bit pc[18] — taken -> the AOT stays uninstalled (no respawn on
     * room re-entry); the grant path sets it. */
    {
        setup_vm();
        uint8_t bc_tk[23] = {
            0x50, 0x07, 0x02, 0x00,
            0x00, 0x00, 0x00, 0x01,             /* rect_x */
            0x00, 0x02, 0x00, 0x03,             /* rect_z, rect_w */
            0x00, 0x04, 0x15, 0x00,             /* rect_d; item 0x15 */
            0x1e, 0x00, 0x2a, 0x00,             /* amount 30; taken-bit pc[18] = 0x2a */
            0x00, 0x00,
            OP_EVT_NEXT
        };
        run_one_opcode(bc_tk);
        TEST_ASSERT_EQ("Item taken-bit: fresh install carries the bit", 0x2a, g_aot.item_params[7].taken_bit);
        TEST_ASSERT_EQ("Item taken-bit: AOT live on fresh install", 1, g_aot.slots[7].active);
        /* simulate the pickup grant (the aot_common grant path sets zone-9 bit) */
        re15_game_flag_set(9, 0x2a, 1);
        setup_vm();                             /* room re-entry: AOTs cleared, flags persist... */
        re15_game_flag_set(9, 0x2a, 1);         /* (setup_vm clears flags; re-assert the taken bit) */
        run_one_opcode(bc_tk);
        TEST_ASSERT_EQ("Item taken-bit: re-install is SUPPRESSED (no respawn)", 0, g_aot.slots[7].active);
    }

    TEST_OK("Item_aot_set (0x50)");
}

/* =========================================================================
 * Test: Door_aot_set (0x3B, 32 Bytes) — installiert DOOR-AOT
 *
 * Ersetzt den obsoleten Legacy-Opcode 0x68 ("Door_aot_set_4p"): der hat keinen
 * dedizierten Handler mehr (op_unknown skippt nur 40 Bytes). 0x3B ist der
 * aktuelle, bedeutungstragende Tür-Opcode (op_door_aot_set, scd_vm.c:2300).
 * ========================================================================= */

static void test_door_aot_set(void)
{
    /* op_door_aot_set: slot=pc[1], rect (LE) @pc[6..13], next_pos (LE)
     * @pc[14..19], target_cut=pc[24]; re15_aot_set_door -> DOOR-AOT +
     * door_params. pc+=32. */
    uint8_t bytecode[33];
    long n;

    memset(bytecode, 0, sizeof(bytecode));
    bytecode[0]  = 0x3B;
    bytecode[1]  = 0x02;            /* slot = 2 */
    /* rect_w (pc[10..11], LE) != 0 ist nicht nötig — re15_aot_set_door
     * installiert die AOT unabhängig davon. */
    bytecode[24] = 0x03;           /* target_cut = 3 */
    bytecode[32] = OP_EVT_NEXT;     /* Sentinel */

    setup_vm();
    n = run_one_opcode(bytecode);

    TEST_ASSERT_EQ("Door_aot_set: Opcode-Größe = 32", 32, n);
    TEST_ASSERT_EQ("Door_aot_set: Slot 2 aktiv", 1, g_aot.slots[2].active);
    TEST_ASSERT_EQ("Door_aot_set: Typ DOOR", RE15_AOT_TYPE_DOOR, g_aot.slots[2].type);
    TEST_ASSERT_EQ("Door_aot_set: target_cut", 3, g_aot.door_params[2].target_cut);

    TEST_OK("Door_aot_set (0x3B)");
}

/* =========================================================================
 * Main
 * ========================================================================= */

int main(void)
{
    printf("=== SCD AOT Opcodes Unit Tests ===\n\n");

    test_cut_chg();
    test_aot_set();
    test_obj_model_set();
    test_se_on();
    test_sce_em_set();
    test_item_aot_set();
    test_door_aot_set();

    printf("\n=== Ergebnis: %d PASS, %d FAIL ===\n", g_pass, g_fail);
    return (g_fail > 0) ? 1 : 0;
}
