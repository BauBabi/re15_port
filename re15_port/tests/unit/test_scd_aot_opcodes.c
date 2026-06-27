/**
 * @file test_scd_aot_opcodes.c
 * @brief Unit-Tests für SCD AOT- und Interaktions-Opcodes
 *
 * Testet die korrekte Ausführung aller in scd_opcodes_aot.c implementierten
 * Opcodes: Cut_chg, Aot_set, Obj_model_set, Se_on, Sce_em_set,
 * Item_aot_set, Door_aot_set_4p.
 *
 * Requirements: 4.1, 4.6
 */

#include <stdio.h>
#include <string.h>
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_audio.h"

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

/* =========================================================================
 * Helper: Thread+VM für Tests vorbereiten
 * ========================================================================= */

static void setup_vm(void)
{
    scd_vm_init();
    re15_aot_init();
}

/* =========================================================================
 * Test: Cut_chg (0x29, 4 Bytes)
 * ========================================================================= */

static void test_cut_chg(void)
{
    /* Layout: [0x29][unused][cut_id_lo][cut_id_hi] */
    uint8_t bytecode[] = { 0x29, 0x00, 0x05, 0x00 }; /* cut_id = 5 */
    scd_thread_t thread;
    int rc;

    setup_vm();
    memset(&thread, 0, sizeof(thread));
    thread.active = 1;
    thread.pc = bytecode;

    rc = g_scd.handlers[0x29](&g_scd, &thread);

    TEST_ASSERT_EQ("Cut_chg: return 1 (continue)", 1, rc);
    TEST_ASSERT_EQ("Cut_chg: PC += 4", (long long)(bytecode + 4), (long long)thread.pc);
    TEST_ASSERT_EQ("Cut_chg: cam_change_pending", 1, g_scd.cam_change_pending);
    TEST_ASSERT_EQ("Cut_chg: cam_id", 5, g_scd.cam_id);

    TEST_OK("Cut_chg (0x29)");
}

/* =========================================================================
 * Test: Aot_set (0x2C, 20 Bytes)
 * ========================================================================= */

static void test_aot_set(void)
{
    /* Layout: [opcode][slot][type][floor][x0_lo][x0_hi][x1_lo][x1_hi]
     *         [x2_lo][x2_hi][x3_lo][x3_hi]
     *         [z0_lo][z0_hi][z1_lo][z1_hi][z2_lo][z2_hi][z3_lo][z3_hi] */
    uint8_t bytecode[20] = {
        0x2C, 0x03, 0x03, 0x01,             /* slot=3, type=GENERIC, floor=1 */
        0x10, 0x00, 0x20, 0x00,             /* x[0]=16, x[1]=32 */
        0x30, 0x00, 0x40, 0x00,             /* x[2]=48, x[3]=64 */
        0x50, 0x00, 0x60, 0x00,             /* z[0]=80, z[1]=96 */
        0x70, 0x00, 0x80, 0x00              /* z[2]=112, z[3]=128 */
    };
    scd_thread_t thread;
    int rc;

    setup_vm();
    memset(&thread, 0, sizeof(thread));
    thread.active = 1;
    thread.pc = bytecode;

    rc = g_scd.handlers[0x2C](&g_scd, &thread);

    TEST_ASSERT_EQ("Aot_set: return 1", 1, rc);
    TEST_ASSERT_EQ("Aot_set: PC += 20", (long long)(bytecode + 20), (long long)thread.pc);

    TEST_OK("Aot_set (0x2C)");
}

/* =========================================================================
 * Test: Obj_model_set (0x2D, 22 Bytes) — Stub
 * ========================================================================= */

static void test_obj_model_set(void)
{
    uint8_t bytecode[22];
    scd_thread_t thread;
    int rc;

    memset(bytecode, 0, sizeof(bytecode));
    bytecode[0] = 0x2D;

    setup_vm();
    memset(&thread, 0, sizeof(thread));
    thread.active = 1;
    thread.pc = bytecode;

    rc = g_scd.handlers[0x2D](&g_scd, &thread);

    TEST_ASSERT_EQ("Obj_model_set: return 1", 1, rc);
    TEST_ASSERT_EQ("Obj_model_set: PC += 22", (long long)(bytecode + 22), (long long)thread.pc);

    TEST_OK("Obj_model_set (0x2D) stub");
}

/* =========================================================================
 * Test: Se_on (0x36, 12 Bytes) — ohne Audio-Backend
 * ========================================================================= */

static void test_se_on(void)
{
    /* Layout: [opcode][unused][bank][sample_id][vol][pan][pad×6] */
    uint8_t bytecode[12] = {
        0x36, 0x00, 0x02, 0x0A,             /* bank=2, sample_id=10 */
        0x7F, 0x40, 0x00, 0x00,             /* vol=127, pan=64 */
        0x00, 0x00, 0x00, 0x00              /* pad */
    };
    scd_thread_t thread;
    int rc;

    setup_vm();
    memset(&thread, 0, sizeof(thread));
    thread.active = 1;
    thread.pc = bytecode;

    /* g_audio ist NULL — sollte Info loggen, nicht crashen */
    rc = g_scd.handlers[0x36](&g_scd, &thread);

    TEST_ASSERT_EQ("Se_on: return 1", 1, rc);
    TEST_ASSERT_EQ("Se_on: PC += 12", (long long)(bytecode + 12), (long long)thread.pc);

    TEST_OK("Se_on (0x36) no backend");
}

/* =========================================================================
 * Test: Sce_em_set (0x44, 22 Bytes) — Stub
 * ========================================================================= */

static void test_sce_em_set(void)
{
    uint8_t bytecode[22];
    scd_thread_t thread;
    int rc;

    memset(bytecode, 0, sizeof(bytecode));
    bytecode[0] = 0x44;

    setup_vm();
    memset(&thread, 0, sizeof(thread));
    thread.active = 1;
    thread.pc = bytecode;

    rc = g_scd.handlers[0x44](&g_scd, &thread);

    TEST_ASSERT_EQ("Sce_em_set: return 1", 1, rc);
    TEST_ASSERT_EQ("Sce_em_set: PC += 22", (long long)(bytecode + 22), (long long)thread.pc);

    TEST_OK("Sce_em_set (0x44) stub");
}

/* =========================================================================
 * Test: Item_aot_set (0x4E, 22 Bytes)
 * ========================================================================= */

static void test_item_aot_set(void)
{
    /* Layout: [opcode][slot][type][floor][x×4×2B][z×4×2B][item_id][amount] */
    uint8_t bytecode[22] = {
        0x4E, 0x05, 0x02, 0x00,             /* slot=5, type=ITEM, floor=0 */
        0x00, 0x01, 0x00, 0x02,             /* x[0]=256, x[1]=512 */
        0x00, 0x03, 0x00, 0x04,             /* x[2]=768, x[3]=1024 */
        0x00, 0x01, 0x00, 0x02,             /* z[0]=256, z[1]=512 */
        0x00, 0x03, 0x00, 0x04,             /* z[2]=768, z[3]=1024 */
        0x07, 0x01                          /* item_id=7, amount=1 */
    };
    scd_thread_t thread;
    int rc;

    setup_vm();
    memset(&thread, 0, sizeof(thread));
    thread.active = 1;
    thread.pc = bytecode;

    rc = g_scd.handlers[0x4E](&g_scd, &thread);

    TEST_ASSERT_EQ("Item_aot_set: return 1", 1, rc);
    TEST_ASSERT_EQ("Item_aot_set: PC += 22", (long long)(bytecode + 22), (long long)thread.pc);

    TEST_OK("Item_aot_set (0x4E)");
}

/* =========================================================================
 * Test: Door_aot_set_4p (0x68, 40 Bytes)
 * ========================================================================= */

static void test_door_aot_set_4p(void)
{
    /* 40-Byte Layout:
     * [opcode][slot][type][floor]                    — 4B
     * [x0..x3 as s16 LE]                            — 8B
     * [z0..z3 as s16 LE]                            — 8B
     * [dest_stage][dest_room][dest_cut][pad]         — 4B
     * [spawn_x][spawn_y][spawn_z][spawn_rot] as s16  — 8B
     * [reserved 8B]                                  — 8B
     */
    uint8_t bytecode[40] = {
        0x68, 0x02, 0x01, 0x00,             /* slot=2, type=DOOR, floor=0 */
        0x00, 0xFE, 0x00, 0xFF,             /* x[0]=-512, x[1]=-256 */
        0x00, 0x01, 0x00, 0x02,             /* x[2]=256, x[3]=512 */
        0x00, 0xFE, 0x00, 0xFF,             /* z[0]=-512, z[1]=-256 */
        0x00, 0x01, 0x00, 0x02,             /* z[2]=256, z[3]=512 */
        0x02, 0x17, 0x03, 0x00,             /* dest_stage=2, dest_room=0x17, dest_cut=3 */
        0xD0, 0xFB, 0x00, 0x00,            /* spawn_x=-1072, spawn_y=0 */
        0x48, 0x0D, 0x00, 0x08,            /* spawn_z=3400, spawn_rot=2048 */
        0x00, 0x00, 0x00, 0x00,            /* reserved */
        0x00, 0x00, 0x00, 0x00             /* reserved */
    };
    scd_thread_t thread;
    int rc;

    setup_vm();
    memset(&thread, 0, sizeof(thread));
    thread.active = 1;
    thread.pc = bytecode;

    rc = g_scd.handlers[0x68](&g_scd, &thread);

    TEST_ASSERT_EQ("Door_aot_set_4p: return 1", 1, rc);
    TEST_ASSERT_EQ("Door_aot_set_4p: PC += 40", (long long)(bytecode + 40), (long long)thread.pc);

    TEST_OK("Door_aot_set_4p (0x68)");
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
    test_door_aot_set_4p();

    printf("\n=== Ergebnis: %d PASS, %d FAIL ===\n", g_pass, g_fail);
    return (g_fail > 0) ? 1 : 0;
}
