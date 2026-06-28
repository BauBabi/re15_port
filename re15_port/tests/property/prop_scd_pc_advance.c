/**
 * @file prop_scd_pc_advance.c
 * @brief Property-Test: SCD-Opcode PC-Advancement
 *
 * **Validates: Requirements 4.6**
 *
 * Property 6: Fuer jeden implementierten Opcode wird geprueft, dass die
 * Ausfuehrung den PC um exakt die vom AKTUELLEN Engine-Handler vorgesehene
 * Byte-Anzahl vorrueckt (= das byte-true Verhalten der op_*-Handler in
 * engine/src/scd_vm.c). Damit bleibt die urspruengliche Test-Absicht
 * erhalten — "ein Opcode rueckt den PC deterministisch um seine Laenge vor".
 *
 * ---------------------------------------------------------------------------
 * API-MIGRATION (Engine-Transplant 2026-06):
 *
 *   ALT (entfernt)                         NEU (oeffentlich, re15_scd.h)
 *   ------------------------------------   --------------------------------------
 *   scd_opcode_handler_t                   (entfernt — Handler sind file-static)
 *   g_scd.handlers[op](&g_scd, thread)     scd_vm_tick()  (dispatcht s_op_table[op])
 *   g_scd.condition_flag                   (entfernt — If nutzt block_stack-Modell;
 *                                           op_if/op_ck: pc += 4 unbedingt)
 *   thread.sleep_counter[]                 thread.sleep_count[]/thread.sleep_slots[][]
 *   thread.call_depth (manuell gesetzt)    von scd_thread_start() initialisiert
 *
 * Die Opcode-Handler (op_nop, op_evt_end, …) sind jetzt `static` in scd_vm.c
 * und NICHT mehr ueber eine oeffentliche Dispatch-Tabelle erreichbar. Der
 * einzige oeffentliche Weg, einen Opcode auszufuehren, ist die Tick-API:
 *   scd_vm_init()                — VM + Subsysteme zuruecksetzen
 *   scd_thread_start(slot, pc)   — Thread mit Bytecode-PC starten
 *   scd_vm_tick()                — alle aktiven Threads einen Tick laufen
 *
 * MESS-METHODE (run_one_opcode):
 *   Hinter den Ziel-Opcode wird ein 1-Byte-Sentinel Evt_next (0x02) gelegt.
 *   In EINEM Tick laeuft: Ziel-Opcode (return 1 = continue) → Evt_next
 *   (pc += 1; return 2 = yield) → Thread-Schleife bricht ab. Danach zeigt
 *   thread.pc hinter den Sentinel; der gemessene PC-Vorschub des Ziel-Opcodes
 *   ist also (thread.pc - start) - 1 (Sentinel abgezogen). Der Thread bleibt
 *   AKTIV (yield, kein Kill) — so faellt zugleich auf, falls ein Opcode den
 *   Thread faelschlich beendet.
 *
 * ---------------------------------------------------------------------------
 * Getestete Opcodes und ihre byte-true PC-Laengen IM AKTUELLEN ENGINE-STAND
 * (jeweils op_*-Handler in scd_vm.c zitiert):
 *   0x00 Nop              —  1 B  (op_nop: t->pc++)
 *   0x01 Evt_end          —  Thread deaktiviert (op_evt_end: pc++, return 0 →
 *                            scd_vm_tick deaktiviert bei call_depth==0)
 *   0x04 Evt_exec         —  4 B  (op_evt_exec: t->pc += 4)
 *   0x06 If (Ifel_ck)     —  4 B  (op_if: t->pc += 4 — IMMER, Praedikat im Body)
 *   0x07 Else_ck          —  +offset (op_else: t->pc += pc[2..3] LE; hier 4)
 *   0x08 Endif            —  2 B  (op_end_if: t->pc += 2)
 *   0x09 Sleep            —  1 B  (op_sleep: t->pc++ — Sleeping@pc+1 liest Slot;
 *                            disasm 0x8003F3E0. War im Alt-Test als 4 dokumentiert.)
 *   0x0A Sleeping         —  3 B  (op_sleeping, kein Slot: t->pc += 3 — disasm
 *                            LAB_8003f428. War im Alt-Test als 2 dokumentiert.)
 *   0x0D For              —  6 B  (op_for, count>0: t->pc += 6)
 *   0x0E Next             —  2 B  (op_next, Schleife erschoepft: t->pc += 2)
 *   0x21 Ck               —  4 B  (op_ck: t->pc += 4)
 *   0x22 Set              —  4 B  (op_set: t->pc += 4)
 *   0x29 Cut_chg          —  2 B  (op_cut_chg: t->pc += 2 — Java-Disasm Z.231
 *                            "Cut_chg ist 2 Bytes". War im Alt-Test als 4 dok.)
 *   0x2C Aot_set          — 20 B  (op_aot_set: t->pc += 20)
 *   0x2D Obj_model_set    — 34 B  (op_obj_model_set: t->pc += 34 — byte-true
 *                            LAB_80040914 +0x22. War im Alt-Test als 22 dok.)
 *   0x2E Work_set         —  3 B  (op_work_set: t->pc += 3)
 *   0x2F Speed_set        —  4 B  (op_speed_set: t->pc += 4)
 *   0x30 Add_speed        —  1 B  (op_add_speed: t->pc += 1)
 *   0x36 Se_on            — 12 B  (op_se_on: t->pc += 12)
 *   0x44 Sce_em_set       — 20 B  (op_sce_em_set: t->pc += 20 — Java-Size-Tabelle.
 *                            War im Alt-Test als 22 dokumentiert.)
 *   0x4E Item_aot_set     — 22 B  (op_item_aot_set: t->pc += 22)
 *   0x68 Door_aot_set_4p  — 40 B  (kein eigener Handler → op_unknown nutzt
 *                            s_opcode_sizes[0x68] = 40)
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

/* Sentinel-Opcode hinter dem Ziel-Opcode: Evt_next (0x02) yieldet nach
 * genau einem PC++-Schritt → die Thread-Schleife stoppt direkt nach dem
 * Ziel-Opcode + Sentinel. (op_evt_next in scd_vm.c: t->pc += 1; return 2.) */
#define SCD_OP_EVT_NEXT 0x02

#define TEST_SLOT 0

/* =========================================================================
 * Hilfsfunktion: einen Opcode messen
 * ========================================================================= */

/**
 * Fuehrt GENAU EINEN Ziel-Opcode aus und liefert dessen PC-Vorschub in Bytes.
 *
 * `code` muss den Ziel-Opcode an Offset 0 enthalten; an code[op_max_len] muss
 * Platz fuer den Evt_next-Sentinel sein (vom Aufrufer dort hingeschrieben).
 * Rueckgabe: PC-Vorschub des Ziel-Opcodes (Sentinel bereits abgezogen).
 *
 * Nutzt ausschliesslich die oeffentliche Tick-API (scd_vm_init/start/tick),
 * da die op_*-Handler + s_op_table jetzt file-static in scd_vm.c sind.
 */
static ptrdiff_t run_one_opcode(const uint8_t *code)
{
    scd_vm_init();
    scd_thread_start(TEST_SLOT, code);
    const uint8_t *start = g_scd.threads[TEST_SLOT].pc;   /* == code */
    scd_vm_tick();
    const uint8_t *after = g_scd.threads[TEST_SLOT].pc;
    /* Ziel-Opcode (continue) + Evt_next-Sentinel (pc += 1, yield) gelaufen.
     * Vorschub des Ziel-Opcodes = (after - start) - 1 (Sentinel). */
    return (after - start) - 1;
}

/* Schreibt den Evt_next-Sentinel an Offset `at` und nullt davor liegende
 * Bytes des Puffers, damit Operanden definiert sind. */
static void place_sentinel(uint8_t *code, size_t buf_len, size_t at)
{
    memset(code, 0, buf_len);
    code[at] = SCD_OP_EVT_NEXT;
}

/* =========================================================================
 * Property-Tests: PC-Advancement pro Opcode
 * ========================================================================= */

/**
 * Nop (0x00): PC += 1
 */
static int test_nop_pc_advance(void)
{
    uint8_t code[4];
    place_sentinel(code, sizeof(code), 1);   /* Nop = 1 B */
    code[0] = 0x00;

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 1, "Nop: PC advanced by %td, expected 1", adv);

    g_passed++;
    return 0;
}

/**
 * Evt_end (0x01): Thread wird deaktiviert (active == 0).
 * Kein Sentinel — op_evt_end gibt 0 (frame-return) zurueck; scd_vm_tick
 * deaktiviert den Thread bei call_depth==0 (siehe scd_vm.c tick-Loop).
 */
static int test_evt_end_deactivates(void)
{
    uint8_t code[4] = { 0x01, 0x00, 0x00, 0x00 };

    scd_vm_init();
    scd_thread_start(TEST_SLOT, code);
    PROP_ASSERT(g_scd.threads[TEST_SLOT].active == 1,
        "Evt_end: thread should start active");

    scd_vm_tick();

    PROP_ASSERT(g_scd.threads[TEST_SLOT].active == 0,
        "Evt_end: thread should be deactivated after execution");

    g_passed++;
    return 0;
}

/**
 * Evt_exec (0x04): PC += 4
 */
static int test_evt_exec_pc_advance(void)
{
    /* Layout: [0x04][cond][unused][sub_id] */
    uint8_t code[8];
    place_sentinel(code, sizeof(code), 4);
    code[0] = 0x04;
    code[1] = 0x0A;   /* cond (slot hint; ohne RDT → no spawn, advance only) */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 4, "Evt_exec: PC advanced by %td, expected 4", adv);

    g_passed++;
    return 0;
}

/**
 * If / Ifel_ck (0x06): PC += 4 (op_if rueckt IMMER um 4 vor — das frueher
 * separate condition_flag entfaellt; das Praedikat im Body entscheidet).
 */
static int test_if_pc_advance(void)
{
    /* Layout: [0x06][unused][block_length_lo][block_length_hi] */
    uint8_t code[8];
    place_sentinel(code, sizeof(code), 4);
    code[0] = 0x06;
    code[2] = 0x08;   /* block_length (LE) — von op_if nur in block_stack gelegt */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 4, "If (Ifel_ck): PC advanced by %td, expected 4", adv);

    g_passed++;
    return 0;
}

/**
 * Else_ck (0x07): PC += else_offset (op_else: t->pc += pc[2..3] LE).
 * Wir setzen offset = 4 → PC springt um 4 Bytes.
 */
static int test_else_ck_pc_advance(void)
{
    /* Layout: [0x07][unused][skip_lo][skip_hi]; skip = 4 */
    uint8_t code[8];
    place_sentinel(code, sizeof(code), 4);
    code[0] = 0x07;
    code[2] = 0x04;   /* skip = 4 (LE) */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 4, "Else_ck: PC advanced by %td, expected offset=4", adv);

    g_passed++;
    return 0;
}

/**
 * Endif (0x08): PC += 2
 */
static int test_endif_pc_advance(void)
{
    uint8_t code[4];
    place_sentinel(code, sizeof(code), 2);
    code[0] = 0x08;

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 2, "Endif: PC advanced by %td, expected 2", adv);

    g_passed++;
    return 0;
}

/**
 * Sleep (0x09): PC += 1 (byte-true: op_sleep rueckt nur 1 Byte vor, das
 * folgende Sleeping@pc+1 liest den Sleep-Slot — disasm 0x8003F3E0).
 * Der Alt-Test dokumentierte 4; das war retail-RE2/Java-Disasm-Sicht.
 */
static int test_sleep_pc_advance(void)
{
    /* Layout: [0x09][unused][count_lo][count_hi] */
    uint8_t code[8];
    place_sentinel(code, sizeof(code), 1);   /* Sleep rueckt 1 B vor */
    code[0] = 0x09;
    code[2] = 0x05;   /* count_lo (LE) */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 1,
        "Sleep: PC advanced by %td, expected 1 (byte-true op_sleep)", adv);

    g_passed++;
    return 0;
}

/**
 * Sleeping (0x0A) ohne aktiven Sleep-Slot: PC += 3 (byte-true: op_sleeping
 * findet sleep_count[depth]-1 < 0 → t->pc += 3; return 1 — disasm
 * LAB_8003f428). Der Alt-Test dokumentierte 2.
 */
static int test_sleeping_pc_advance(void)
{
    uint8_t code[8];
    place_sentinel(code, sizeof(code), 3);   /* Sleeping (kein Slot) rueckt 3 B vor */
    code[0] = 0x0A;
    /* Frisch gestarteter Thread hat sleep_count[0]==0 → Slot -1 → 3-Byte-Advance. */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 3,
        "Sleeping (no slot): PC advanced by %td, expected 3", adv);

    g_passed++;
    return 0;
}

/**
 * For (0x0D): PC += 6 (count>0)
 */
static int test_for_pc_advance(void)
{
    /* Layout: [0x0D][unused][block_len_lo][block_len_hi][count_lo][count_hi] */
    uint8_t code[10];
    place_sentinel(code, sizeof(code), 6);
    code[0] = 0x0D;
    code[2] = 0x06;   /* block_length (LE) */
    code[4] = 0x03;   /* count (LE) = 3 → count>0 Pfad → pc += 6 */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 6, "For: PC advanced by %td, expected 6", adv);

    g_passed++;
    return 0;
}

/**
 * Next (0x0E) bei erschoepfter Schleife: PC += 2.
 * Frischer Thread hat for_depth==0 → op_next nimmt direkt den
 * "kein aktiver For"-Pfad (for_depth > 0 ist false) → t->pc += 2.
 */
static int test_next_pc_advance(void)
{
    uint8_t code[4];
    place_sentinel(code, sizeof(code), 2);
    code[0] = 0x0E;

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 2, "Next (exhausted): PC advanced by %td, expected 2", adv);

    g_passed++;
    return 0;
}

/**
 * Ck (0x21): PC += 4
 */
static int test_ck_pc_advance(void)
{
    /* Layout: [0x21][zone][idx][value] */
    uint8_t code[8];
    place_sentinel(code, sizeof(code), 4);
    code[0] = 0x21;
    code[2] = 0x01;   /* idx */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 4, "Ck: PC advanced by %td, expected 4", adv);

    g_passed++;
    return 0;
}

/**
 * Set (0x22): PC += 4
 */
static int test_set_pc_advance(void)
{
    /* Layout: [0x22][zone][idx][op] */
    uint8_t code[8];
    place_sentinel(code, sizeof(code), 4);
    code[0] = 0x22;
    code[2] = 0x02;   /* idx */
    code[3] = 0x01;   /* op = 1 (OR/set bit) */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 4, "Set: PC advanced by %td, expected 4", adv);

    g_passed++;
    return 0;
}

/**
 * Cut_chg (0x29): PC += 2 (byte-true: op_cut_chg t->pc += 2 — Java-Disasm
 * Z.231 "Cut_chg ist 2 Bytes [op, cam_id]"). Der Alt-Test dokumentierte 4.
 */
static int test_cut_chg_pc_advance(void)
{
    /* Layout: [0x29][cam_id] */
    uint8_t code[8];
    place_sentinel(code, sizeof(code), 2);
    code[0] = 0x29;
    code[1] = 0x03;   /* cam_id */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 2, "Cut_chg: PC advanced by %td, expected 2", adv);

    g_passed++;
    return 0;
}

/**
 * Aot_set (0x2C): PC += 20
 */
static int test_aot_set_pc_advance(void)
{
    uint8_t code[24];
    place_sentinel(code, sizeof(code), 20);
    code[0] = 0x2C;
    /* type (pc[2]) bleibt 0 → generischer AOT-Pfad; eventId pc[17]==0
     * → EXAMINE_WORKVAR-Pfad; alle nur Side-Effects, PC-Advance bleibt 20. */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 20, "Aot_set: PC advanced by %td, expected 20", adv);

    g_passed++;
    return 0;
}

/**
 * Obj_model_set (0x2D): PC += 34 (byte-true: op_obj_model_set t->pc += 34,
 * = LAB_80040914 +0x22). Der Alt-Test dokumentierte 22.
 */
static int test_obj_model_set_pc_advance(void)
{
    uint8_t code[40];
    place_sentinel(code, sizeof(code), 34);
    code[0] = 0x2D;

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 34, "Obj_model_set: PC advanced by %td, expected 34", adv);

    g_passed++;
    return 0;
}

/**
 * Work_set (0x2E): PC += 3
 */
static int test_work_set_pc_advance(void)
{
    /* Layout: [0x2E][kind][idx] */
    uint8_t code[6];
    place_sentinel(code, sizeof(code), 3);
    code[0] = 0x2E;
    code[1] = 0x01;   /* kind */
    code[2] = 0x02;   /* idx */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 3, "Work_set: PC advanced by %td, expected 3", adv);

    g_passed++;
    return 0;
}

/**
 * Speed_set (0x2F): PC += 4
 */
static int test_speed_set_pc_advance(void)
{
    /* Layout: [0x2F][axis][value_lo][value_hi] */
    uint8_t code[8];
    place_sentinel(code, sizeof(code), 4);
    code[0] = 0x2F;
    code[2] = 0x10;   /* value_lo (LE) */

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 4, "Speed_set: PC advanced by %td, expected 4", adv);

    g_passed++;
    return 0;
}

/**
 * Add_speed (0x30): PC += 1
 */
static int test_add_speed_pc_advance(void)
{
    uint8_t code[4];
    place_sentinel(code, sizeof(code), 1);
    code[0] = 0x30;

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 1, "Add_speed: PC advanced by %td, expected 1", adv);

    g_passed++;
    return 0;
}

/**
 * Se_on (0x36): PC += 12
 */
static int test_se_on_pc_advance(void)
{
    uint8_t code[16];
    place_sentinel(code, sizeof(code), 12);
    code[0] = 0x36;

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 12, "Se_on: PC advanced by %td, expected 12", adv);

    g_passed++;
    return 0;
}

/**
 * Sce_em_set (0x44): PC += 20 (byte-true: op_sce_em_set t->pc += 20 — Java-
 * Size-Tabelle). Der Alt-Test dokumentierte 22.
 */
static int test_sce_em_set_pc_advance(void)
{
    uint8_t code[24];
    place_sentinel(code, sizeof(code), 20);
    code[0] = 0x44;

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 20, "Sce_em_set: PC advanced by %td, expected 20", adv);

    g_passed++;
    return 0;
}

/**
 * Item_aot_set (0x4E): PC += 22 (legacy-Alias auf op_item_aot_set, t->pc += 22)
 */
static int test_item_aot_set_pc_advance(void)
{
    uint8_t code[26];
    place_sentinel(code, sizeof(code), 22);
    code[0] = 0x4E;

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 22, "Item_aot_set: PC advanced by %td, expected 22", adv);

    g_passed++;
    return 0;
}

/**
 * Door_aot_set_4p (0x68): PC += 40 (kein dedizierter Handler → op_unknown
 * nutzt s_opcode_sizes[0x68] = 40).
 */
static int test_door_aot_set_4p_pc_advance(void)
{
    uint8_t code[44];
    place_sentinel(code, sizeof(code), 40);
    code[0] = 0x68;

    ptrdiff_t adv = run_one_opcode(code);
    PROP_ASSERT(adv == 40, "Door_aot_set_4p: PC advanced by %td, expected 40", adv);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Main — Alle Opcode PC-Advancement-Tests ausfuehren
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

    printf("[4/22] If/Ifel_ck (0x06): PC += 4\n");
    if (test_if_pc_advance() != 0) goto done;

    printf("[5/22] Else_ck (0x07): PC += offset (4)\n");
    if (test_else_ck_pc_advance() != 0) goto done;

    printf("[6/22] Endif (0x08): PC += 2\n");
    if (test_endif_pc_advance() != 0) goto done;

    printf("[7/22] Sleep (0x09): PC += 1\n");
    if (test_sleep_pc_advance() != 0) goto done;

    printf("[8/22] Sleeping (0x0A, no slot): PC += 3\n");
    if (test_sleeping_pc_advance() != 0) goto done;

    printf("[9/22] For (0x0D): PC += 6\n");
    if (test_for_pc_advance() != 0) goto done;

    printf("[10/22] Next (0x0E, exhausted): PC += 2\n");
    if (test_next_pc_advance() != 0) goto done;

    printf("[11/22] Ck (0x21): PC += 4\n");
    if (test_ck_pc_advance() != 0) goto done;

    printf("[12/22] Set (0x22): PC += 4\n");
    if (test_set_pc_advance() != 0) goto done;

    printf("[13/22] Cut_chg (0x29): PC += 2\n");
    if (test_cut_chg_pc_advance() != 0) goto done;

    printf("[14/22] Aot_set (0x2C): PC += 20\n");
    if (test_aot_set_pc_advance() != 0) goto done;

    printf("[15/22] Obj_model_set (0x2D): PC += 34\n");
    if (test_obj_model_set_pc_advance() != 0) goto done;

    printf("[16/22] Work_set (0x2E): PC += 3\n");
    if (test_work_set_pc_advance() != 0) goto done;

    printf("[17/22] Speed_set (0x2F): PC += 4\n");
    if (test_speed_set_pc_advance() != 0) goto done;

    printf("[18/22] Add_speed (0x30): PC += 1\n");
    if (test_add_speed_pc_advance() != 0) goto done;

    printf("[19/22] Se_on (0x36): PC += 12\n");
    if (test_se_on_pc_advance() != 0) goto done;

    printf("[20/22] Sce_em_set (0x44): PC += 20\n");
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
