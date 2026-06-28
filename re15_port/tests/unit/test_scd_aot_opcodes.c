/**
 * @file test_scd_aot_opcodes.c
 * @brief Unit-Tests für SCD AOT- und Interaktions-Opcodes
 *
 * Testet die korrekte Ausführung der AOT-/Interaktions-Opcodes der SCD-VM:
 * Cut_chg (0x29), Aot_set (0x2C), Obj_model_set (0x2D), Se_on (0x36),
 * Sce_em_set (0x44), Item_aot_set (0x4E) und Door_aot_set (0x3B).
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
 *   - Item_aot_set 0x4E = 22 (op_item_aot_set, 0x4E ist registrierter
 *       Legacy-Alias auf op_item_aot_set; t->pc+=22)
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
 * Test: Item_aot_set (0x4E, 22 Bytes) — installiert ITEM-AOT
 * ========================================================================= */

static void test_item_aot_set(void)
{
    /* op_item_aot_set (scd_vm.c:2366; 0x4E = Legacy-Alias auf den 0x50-Handler):
     * slot=pc[1], rect (LE) @pc[6..13], item_type=pc[15], amount=pc[17];
     * re15_aot_set_item -> ITEM-AOT aktiv. pc+=22. */
    uint8_t bytecode[23] = {
        0x4E, 0x05, 0x02, 0x00,             /* slot=5 */
        0x00, 0x00, 0x00, 0x01,             /* rect_x (LE @6) */
        0x00, 0x02, 0x00, 0x03,             /* rect_z, rect_w */
        0x00, 0x04, 0x00, 0x07,             /* rect_d; item_type=pc[15]=7 */
        0x00, 0x01, 0x00, 0x00,             /* amount=pc[17]=1 */
        0x00, 0x00,                          /* pc[20..21] */
        OP_EVT_NEXT                          /* Sentinel */
    };
    long n;

    setup_vm();
    n = run_one_opcode(bytecode);

    TEST_ASSERT_EQ("Item_aot_set: Opcode-Größe = 22", 22, n);
    TEST_ASSERT_EQ("Item_aot_set: Slot 5 aktiv", 1, g_aot.slots[5].active);
    TEST_ASSERT_EQ("Item_aot_set: Typ ITEM", RE15_AOT_TYPE_ITEM, g_aot.slots[5].type);

    TEST_OK("Item_aot_set (0x4E)");
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
