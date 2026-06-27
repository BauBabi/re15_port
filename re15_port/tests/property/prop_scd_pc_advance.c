/**
 * @file prop_scd_pc_advance.c
 * @brief Property-Test: SCD-Opcode PC-Advancement
 *
 * **Validates: Requirements 4.6**
 *
 * Property 6: Für jeden implementierten Opcode wird geprüft, dass die
 * Ausführung den PC um exakt die dokumentierte Opcode-Länge vorrückt.
 *
 * Getestete Opcodes und ihre dokumentierten Längen:
 *   0x00 Nop              —  1 Byte
 *   0x01 Evt_end          —  1 Byte (Thread deaktiviert)
 *   0x04 Evt_exec         —  4 Bytes
 *   0x06 Ifel_ck          —  4 Bytes (condition_flag=1 → linear)
 *   0x07 Else_ck          —  4 Bytes (springt um else_offset)
 *   0x08 Endif            —  2 Bytes
 *   0x09 Sleep            —  4 Bytes
 *   0x0A Sleeping         —  2 Bytes (counter == 0 → advance)
 *   0x0D For              —  6 Bytes
 *   0x0E Next             —  2 Bytes (loop exhausted → advance)
 *   0x21 Ck               —  4 Bytes
 *   0x22 Set              —  4 Bytes
 *   0x29 Cut_chg          —  4 Bytes
 *   0x2C Aot_set          — 20 Bytes
 *   0x2D Obj_model_set    — 22 Bytes
 *   0x2E Work_set         —  3 Bytes
 *   0x2F Speed_set        —  4 Bytes
 *   0x30 Add_speed        —  1 Byte
 *   0x36 Se_on            — 12 Bytes
 *   0x44 Sce_em_set       — 22 Bytes
 *   0x4E Item_aot_set     — 22 Bytes
 *   0x68 Door_aot_set_4p  — 40 Bytes
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "re15_scd.h"
#include "re15_audio.h"

/* =========================================================================
 * Test-Infrastruktur
 * ========================================================================= */

static int g_passed = 0;
static int g_failed = 0;

#define PROP_ASSERT(cond, fmt, ...)                                          \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("  FAIL: " fmt "\n", ##__VA_ARGS__);                       \
            printf("        at %s:%d\n", __FILE__, __LINE__);                 \
            g_failed++;                                                        \
            return 1;                                                          \
        }                                                                      \
    } while (0)

/* =========================================================================
 * Hilfsfunktion: Thread vorbereiten
 * ========================================================================= */

/**
 * Initialisiert die VM und einen Thread im Slot 0 mit gegebenem PC.
 */
static void setup_thread(scd_thread_t* thread, const uint8_t* bytecode)
{
    scd_vm_init();
    memset(thread, 0, sizeof(scd_thread_t));
    thread->active = 1;
    thread->pc = bytecode;
    thread->call_depth = 0;
    thread->for_depth = -1;
    thread->work_slot = -1;
    thread->work_prop_idx = -1;
}

/**
 * Führt genau einen Opcode auf dem Thread aus.
 * Verwendet den Handler aus der globalen VM-Dispatch-Tabelle.
 */
static int exec_one(scd_thread_t* thread)
{
    scd_opcode_handler_t handler = g_scd.handlers[thread->pc[0]];
    return handler(&g_scd, thread);
}

/* =========================================================================
 * Property-Tests: PC-Advancement pro Opcode
 * ========================================================================= */

/**
 * Nop (0x00): PC += 1
 */
static int test_nop_pc_advance(void)
{
    uint8_t code[4] = { 0x00, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 1,
        "Nop: PC advanced by %td, expected 1",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Evt_end (0x01): Thread wird deaktiviert (active == 0)
 */
static int test_evt_end_deactivates(void)
{
    uint8_t code[4] = { 0x01, 0x00, 0x00, 0x00 };
    scd_thread_t thread;

    setup_thread(&thread, code);
    PROP_ASSERT(thread.active == 1,
        "Evt_end: thread should start active");

    exec_one(&thread);

    PROP_ASSERT(thread.active == 0,
        "Evt_end: thread should be deactivated after execution");

    g_passed++;
    return 0;
}

/**
 * Evt_exec (0x04): PC += 4
 */
static int test_evt_exec_pc_advance(void)
{
    /* Layout: [0x04][target_slot][offset_lo][offset_hi] */
    uint8_t code[8] = { 0x04, 0x0A, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 4,
        "Evt_exec: PC advanced by %td, expected 4",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Ifel_ck (0x06): mit condition_flag=1 → PC += 4 (linear advance)
 */
static int test_ifel_ck_pc_advance(void)
{
    /* Layout: [0x06][unused][else_offset_lo][else_offset_hi] */
    uint8_t code[8] = { 0x06, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    g_scd.condition_flag = 1; /* TRUE → linear advance */
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 4,
        "Ifel_ck (flag=1): PC advanced by %td, expected 4",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Else_ck (0x07): PC += else_offset (jump)
 * Wir setzen else_offset = 4 → PC springt um 4 Bytes
 */
static int test_else_ck_pc_advance(void)
{
    /* Layout: [0x07][unused][endif_offset_lo][endif_offset_hi] */
    uint8_t code[8] = { 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    /* Else_ck springt um den offset-Wert (hier: 4) */
    PROP_ASSERT(thread.pc == start + 4,
        "Else_ck: PC advanced by %td, expected offset=4",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Endif (0x08): PC += 2
 */
static int test_endif_pc_advance(void)
{
    uint8_t code[4] = { 0x08, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 2,
        "Endif: PC advanced by %td, expected 2",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Sleep (0x09): PC += 4
 */
static int test_sleep_pc_advance(void)
{
    /* Layout: [0x09][unused][count_lo][count_hi] */
    uint8_t code[8] = { 0x09, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 4,
        "Sleep: PC advanced by %td, expected 4",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Sleeping (0x0A): mit sleep_counter == 0 → PC += 2
 */
static int test_sleeping_pc_advance(void)
{
    uint8_t code[4] = { 0x0A, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    /* sleep_counter == 0 → Timer abgelaufen, PC vorrücken */
    thread.sleep_counter[0] = 0;
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 2,
        "Sleeping (counter=0): PC advanced by %td, expected 2",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * For (0x0D): PC += 6
 */
static int test_for_pc_advance(void)
{
    /* Layout: [0x0D][unused][count_lo][count_hi][body_off_lo][body_off_hi] */
    uint8_t code[8] = { 0x0D, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 6,
        "For: PC advanced by %td, expected 6",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Next (0x0E): mit for_count == 0 (loop exhausted) → PC += 2
 */
static int test_next_pc_advance(void)
{
    uint8_t code[4] = { 0x0E, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    /* Set up for-loop state so it exhausts: for_depth=0, for_count[0]=1
     * Next decrements first, then checks: 1-1=0 → exhausted → PC += 2 */
    thread.for_depth = 0;
    thread.for_count[0] = 1;
    thread.for_body[0] = code; /* doesn't matter, won't be used */
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 2,
        "Next (exhausted): PC advanced by %td, expected 2",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Ck (0x21): PC += 4
 */
static int test_ck_pc_advance(void)
{
    /* Layout: [0x21][flag_group][flag_bit][unused] */
    uint8_t code[8] = { 0x21, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 4,
        "Ck: PC advanced by %td, expected 4",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Set (0x22): PC += 4
 */
static int test_set_pc_advance(void)
{
    /* Layout: [0x22][flag_group][flag_bit][value] */
    uint8_t code[8] = { 0x22, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 4,
        "Set: PC advanced by %td, expected 4",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Cut_chg (0x29): PC += 4
 */
static int test_cut_chg_pc_advance(void)
{
    /* Layout: [0x29][unused][cut_id_lo][cut_id_hi] */
    uint8_t code[8] = { 0x29, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 4,
        "Cut_chg: PC advanced by %td, expected 4",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Aot_set (0x2C): PC += 20
 */
static int test_aot_set_pc_advance(void)
{
    uint8_t code[24];
    scd_thread_t thread;
    const uint8_t* start;

    memset(code, 0, sizeof(code));
    code[0] = 0x2C;
    code[1] = 0x00; /* slot */

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 20,
        "Aot_set: PC advanced by %td, expected 20",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Obj_model_set (0x2D): PC += 22
 */
static int test_obj_model_set_pc_advance(void)
{
    uint8_t code[24];
    scd_thread_t thread;
    const uint8_t* start;

    memset(code, 0, sizeof(code));
    code[0] = 0x2D;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 22,
        "Obj_model_set: PC advanced by %td, expected 22",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Work_set (0x2E): PC += 3
 */
static int test_work_set_pc_advance(void)
{
    /* Layout: [0x2E][slot][prop] */
    uint8_t code[4] = { 0x2E, 0x01, 0x02, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 3,
        "Work_set: PC advanced by %td, expected 3",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Speed_set (0x2F): PC += 4
 */
static int test_speed_set_pc_advance(void)
{
    /* Layout: [0x2F][axis][speed_lo][speed_hi] */
    uint8_t code[8] = { 0x2F, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 4,
        "Speed_set: PC advanced by %td, expected 4",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Add_speed (0x30): PC += 1
 */
static int test_add_speed_pc_advance(void)
{
    uint8_t code[4] = { 0x30, 0x00, 0x00, 0x00 };
    scd_thread_t thread;
    const uint8_t* start;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 1,
        "Add_speed: PC advanced by %td, expected 1",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Se_on (0x36): PC += 12
 */
static int test_se_on_pc_advance(void)
{
    uint8_t code[16];
    scd_thread_t thread;
    const uint8_t* start;

    memset(code, 0, sizeof(code));
    code[0] = 0x36;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 12,
        "Se_on: PC advanced by %td, expected 12",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Sce_em_set (0x44): PC += 22
 */
static int test_sce_em_set_pc_advance(void)
{
    uint8_t code[24];
    scd_thread_t thread;
    const uint8_t* start;

    memset(code, 0, sizeof(code));
    code[0] = 0x44;

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 22,
        "Sce_em_set: PC advanced by %td, expected 22",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Item_aot_set (0x4E): PC += 22
 */
static int test_item_aot_set_pc_advance(void)
{
    uint8_t code[24];
    scd_thread_t thread;
    const uint8_t* start;

    memset(code, 0, sizeof(code));
    code[0] = 0x4E;
    code[1] = 0x00; /* slot */

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 22,
        "Item_aot_set: PC advanced by %td, expected 22",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/**
 * Door_aot_set_4p (0x68): PC += 40
 */
static int test_door_aot_set_4p_pc_advance(void)
{
    uint8_t code[44];
    scd_thread_t thread;
    const uint8_t* start;

    memset(code, 0, sizeof(code));
    code[0] = 0x68;
    code[1] = 0x00; /* slot */

    setup_thread(&thread, code);
    start = thread.pc;
    exec_one(&thread);

    PROP_ASSERT(thread.pc == start + 40,
        "Door_aot_set_4p: PC advanced by %td, expected 40",
        (ptrdiff_t)(thread.pc - start));

    g_passed++;
    return 0;
}

/* =========================================================================
 * Main — Alle Opcode PC-Advancement-Tests ausführen
 * ========================================================================= */

int main(void)
{
    printf("=== Property-Test: SCD-Opcode PC-Advancement ===\n");
    printf("    Validates: Requirements 4.6\n\n");

    printf("[1/22] Nop (0x00): PC += 1\n");
    if (test_nop_pc_advance() != 0) goto done;

    printf("[2/22] Evt_end (0x01): thread deactivated\n");
    if (test_evt_end_deactivates() != 0) goto done;

    printf("[3/22] Evt_exec (0x04): PC += 4\n");
    if (test_evt_exec_pc_advance() != 0) goto done;

    printf("[4/22] Ifel_ck (0x06, flag=1): PC += 4\n");
    if (test_ifel_ck_pc_advance() != 0) goto done;

    printf("[5/22] Else_ck (0x07): PC += offset (4)\n");
    if (test_else_ck_pc_advance() != 0) goto done;

    printf("[6/22] Endif (0x08): PC += 2\n");
    if (test_endif_pc_advance() != 0) goto done;

    printf("[7/22] Sleep (0x09): PC += 4\n");
    if (test_sleep_pc_advance() != 0) goto done;

    printf("[8/22] Sleeping (0x0A, counter=0): PC += 2\n");
    if (test_sleeping_pc_advance() != 0) goto done;

    printf("[9/22] For (0x0D): PC += 6\n");
    if (test_for_pc_advance() != 0) goto done;

    printf("[10/22] Next (0x0E, exhausted): PC += 2\n");
    if (test_next_pc_advance() != 0) goto done;

    printf("[11/22] Ck (0x21): PC += 4\n");
    if (test_ck_pc_advance() != 0) goto done;

    printf("[12/22] Set (0x22): PC += 4\n");
    if (test_set_pc_advance() != 0) goto done;

    printf("[13/22] Cut_chg (0x29): PC += 4\n");
    if (test_cut_chg_pc_advance() != 0) goto done;

    printf("[14/22] Aot_set (0x2C): PC += 20\n");
    if (test_aot_set_pc_advance() != 0) goto done;

    printf("[15/22] Obj_model_set (0x2D): PC += 22\n");
    if (test_obj_model_set_pc_advance() != 0) goto done;

    printf("[16/22] Work_set (0x2E): PC += 3\n");
    if (test_work_set_pc_advance() != 0) goto done;

    printf("[17/22] Speed_set (0x2F): PC += 4\n");
    if (test_speed_set_pc_advance() != 0) goto done;

    printf("[18/22] Add_speed (0x30): PC += 1\n");
    if (test_add_speed_pc_advance() != 0) goto done;

    printf("[19/22] Se_on (0x36): PC += 12\n");
    if (test_se_on_pc_advance() != 0) goto done;

    printf("[20/22] Sce_em_set (0x44): PC += 22\n");
    if (test_sce_em_set_pc_advance() != 0) goto done;

    printf("[21/22] Item_aot_set (0x4E): PC += 22\n");
    if (test_item_aot_set_pc_advance() != 0) goto done;

    printf("[22/22] Door_aot_set_4p (0x68): PC += 40\n");
    if (test_door_aot_set_4p_pc_advance() != 0) goto done;

done:
    printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
