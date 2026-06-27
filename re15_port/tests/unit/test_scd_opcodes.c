/**
 * @file test_scd_opcodes.c
 * @brief Unit-Tests fuer SCD-VM Opcodes
 *
 * Testet jeden Opcode isoliert mit bekannten Eingabe/Ausgabe-Paaren:
 * - Nop, Evt_end (Basis)
 * - Sleep/Sleeping (Timer-Dekrementierung)
 * - Ifel_ck/Else_ck/Endif (Kontrollfluss-Verschachtelung)
 * - For/Next (Schleifen)
 * - Ck/Set (Game-Flags)
 * - Work_set, Speed_set (Per-Thread-Zustand)
 *
 * Requirements: 4.6, 12.4
 */

#include "re15_scd.h"
#include "re15_error.h"

#include <stdio.h>
#include <string.h>

/* =========================================================================
 * Hilfsfunktionen
 * ========================================================================= */

/** 16-Bit Little-Endian in Puffer schreiben */
static void write_le16(uint8_t* p, uint16_t val)
{
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)((val >> 8) & 0xFF);
}

/* =========================================================================
 * Test 1: Nop — PC wird um 1 erhoeht
 * ========================================================================= */

static int test_nop(void)
{
    uint8_t bytecode[] = { SCD_OP_NOP, SCD_OP_EVT_END };
    const uint8_t* start_pc;

    scd_vm_init();
    scd_thread_start(0, bytecode);

    start_pc = g_scd.threads[0].pc;

    /* Ein Tick: Nop (continue) → Evt_end (thread end) */
    scd_vm_tick();

    /* Nach Nop sollte PC um 1 vorgerueckt sein, dann Evt_end beendet Thread */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_nop: Thread sollte inaktiv sein nach Nop+Evt_end\n");
        return 1;
    }

    /* Verifiziere, dass PC tatsaechlich auf Evt_end vorgerueckt war */
    /* Da Thread beendet ist, pruefen wir indirekt: Thread wurde inaktiv nach 2 Opcodes */
    (void)start_pc;

    printf("PASS: test_nop\n");
    return 0;
}

/* =========================================================================
 * Test 2: Evt_end — Thread wird deaktiviert
 * ========================================================================= */

static int test_evt_end(void)
{
    uint8_t bytecode[] = { SCD_OP_EVT_END };

    scd_vm_init();
    scd_thread_start(5, bytecode);

    /* Pruefen: Thread ist aktiv */
    if (g_scd.threads[5].active != 1) {
        fprintf(stderr, "FAIL: test_evt_end: Thread 5 sollte aktiv sein vor Tick\n");
        return 1;
    }

    scd_vm_tick();

    /* Thread muss inaktiv sein */
    if (g_scd.threads[5].active != 0) {
        fprintf(stderr, "FAIL: test_evt_end: Thread 5 sollte inaktiv sein nach Evt_end\n");
        return 1;
    }

    printf("PASS: test_evt_end\n");
    return 0;
}

/* =========================================================================
 * Test 3: Sleep + Sleeping — Timer-Dekrementierung ueber mehrere Ticks
 *
 * Bytecode: Sleep(count=3) → Sleeping → Evt_end
 * Erwartet: Sleep setzt Counter auf 3 und yielded.
 *           Tick 2: Sleeping decrements 3→2, yield.
 *           Tick 3: Sleeping decrements 2→1, yield.
 *           Tick 4: Sleeping decrements 1→0, yield.
 *           Tick 5: Sleeping counter==0, PC+=2 (continue), Evt_end.
 * ========================================================================= */

static int test_sleep_sleeping_cycle(void)
{
    /*
     * Sleep-Layout:  [0x09][unused][count_lo][count_hi] = 4 Bytes
     * Sleeping-Layout: [0x0A][unused] = 2 Bytes
     * Evt_end-Layout: [0x01] = 1 Byte
     */
    uint8_t bytecode[7];

    /* Sleep(count=3) */
    bytecode[0] = SCD_OP_SLEEP;
    bytecode[1] = 0x00; /* unused */
    write_le16(bytecode + 2, 3); /* count = 3 */

    /* Sleeping */
    bytecode[4] = SCD_OP_SLEEPING;
    bytecode[5] = 0x00; /* unused */

    /* Evt_end */
    bytecode[6] = SCD_OP_EVT_END;

    scd_vm_init();
    scd_thread_start(0, bytecode);

    /* Tick 1: Sleep setzt counter=3, PC += 4, yield */
    scd_vm_tick();
    if (g_scd.threads[0].active != 1) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Thread inaktiv nach Tick 1\n");
        return 1;
    }
    if (g_scd.threads[0].sleep_counter[0] != 3) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: counter sollte 3 sein nach Sleep, ist %d\n",
                g_scd.threads[0].sleep_counter[0]);
        return 1;
    }

    /* Tick 2: Sleeping decrement 3→2, yield */
    scd_vm_tick();
    if (g_scd.threads[0].sleep_counter[0] != 2) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: counter sollte 2 sein nach Tick 2, ist %d\n",
                g_scd.threads[0].sleep_counter[0]);
        return 1;
    }
    if (g_scd.threads[0].active != 1) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Thread inaktiv nach Tick 2\n");
        return 1;
    }

    /* Tick 3: Sleeping decrement 2→1, yield */
    scd_vm_tick();
    if (g_scd.threads[0].sleep_counter[0] != 1) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: counter sollte 1 sein nach Tick 3, ist %d\n",
                g_scd.threads[0].sleep_counter[0]);
        return 1;
    }

    /* Tick 4: Sleeping decrement 1→0, yield */
    scd_vm_tick();
    if (g_scd.threads[0].sleep_counter[0] != 0) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: counter sollte 0 sein nach Tick 4, ist %d\n",
                g_scd.threads[0].sleep_counter[0]);
        return 1;
    }
    if (g_scd.threads[0].active != 1) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Thread inaktiv nach Tick 4\n");
        return 1;
    }

    /* Tick 5: Sleeping counter==0 → PC+=2 (continue) → Evt_end */
    scd_vm_tick();
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Thread sollte inaktiv sein nach Tick 5\n");
        return 1;
    }

    printf("PASS: test_sleep_sleeping_cycle\n");
    return 0;
}

/* =========================================================================
 * Test 4: Ifel_ck TRUE — condition_flag=1, PC += 4 (in den If-Body)
 * ========================================================================= */

static int test_ifel_ck_true(void)
{
    /*
     * Ifel_ck-Layout: [0x06][unused][else_offset_lo][else_offset_hi] = 4 Bytes
     * Im If-Body: Evt_end
     */
    uint8_t bytecode[5];
    bytecode[0] = SCD_OP_IFEL_CK;
    bytecode[1] = 0x00; /* unused */
    write_le16(bytecode + 2, 10); /* else_offset = 10 (irrelevant bei TRUE) */
    bytecode[4] = SCD_OP_EVT_END;

    scd_vm_init();
    g_scd.condition_flag = 1; /* TRUE */
    scd_thread_start(0, bytecode);

    scd_vm_tick();

    /* Thread sollte Ifel_ck (PC+=4) → Evt_end ausfuehren → inaktiv */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_ifel_ck_true: Thread sollte inaktiv sein (If-Body → Evt_end)\n");
        return 1;
    }

    printf("PASS: test_ifel_ck_true\n");
    return 0;
}

/* =========================================================================
 * Test 5: Ifel_ck FALSE — condition_flag=0, PC springt um else_offset
 * ========================================================================= */

static int test_ifel_ck_false(void)
{
    /*
     * Bytecode:
     *   [0] Ifel_ck mit else_offset=10 (4 Bytes)
     *   [4..9] Dummy (6 Bytes If-Body, nicht erreicht)
     *   [10] Evt_end (Else-Block Beginn)
     */
    uint8_t bytecode[11];
    memset(bytecode, SCD_OP_NOP, sizeof(bytecode));

    bytecode[0] = SCD_OP_IFEL_CK;
    bytecode[1] = 0x00;
    write_le16(bytecode + 2, 10); /* else_offset = 10 → PC springt auf bytecode[10] */

    bytecode[10] = SCD_OP_EVT_END;

    scd_vm_init();
    g_scd.condition_flag = 0; /* FALSE */
    scd_thread_start(0, bytecode);

    scd_vm_tick();

    /* Thread sollte Ifel_ck (PC+=10) → Evt_end → inaktiv */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_ifel_ck_false: Thread sollte inaktiv sein (Jump to Else → Evt_end)\n");
        return 1;
    }

    printf("PASS: test_ifel_ck_false\n");
    return 0;
}

/* =========================================================================
 * Test 6: Else_ck — springt um endif_offset (ueberspringt Else-Body)
 * ========================================================================= */

static int test_else_ck_skip(void)
{
    /*
     * Bytecode:
     *   [0] Else_ck mit endif_offset=8 (4 Bytes)
     *   [4..7] Dummy Else-Body (4 Bytes, nicht erreicht)
     *   [8] Evt_end
     */
    uint8_t bytecode[9];
    memset(bytecode, SCD_OP_NOP, sizeof(bytecode));

    bytecode[0] = SCD_OP_ELSE_CK;
    bytecode[1] = 0x00;
    write_le16(bytecode + 2, 8); /* endif_offset = 8 → PC springt auf bytecode[8] */

    bytecode[8] = SCD_OP_EVT_END;

    scd_vm_init();
    scd_thread_start(0, bytecode);

    scd_vm_tick();

    /* Else_ck springt zu PC+8 → Evt_end → inaktiv */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_else_ck_skip: Thread sollte inaktiv sein (Else_ck → Evt_end)\n");
        return 1;
    }

    printf("PASS: test_else_ck_skip\n");
    return 0;
}

/* =========================================================================
 * Test 7: For/Next Schleife — Body wird count-mal ausgefuehrt
 *
 * Bytecode: For(count=3) → Nop (Body) → Next → Evt_end
 * Erwartet: Nop wird 3x ausgefuehrt, dann Evt_end
 * ========================================================================= */

static int test_for_next_loop(void)
{
    /*
     * For-Layout:  [0x0D][unused][count_lo][count_hi][body_ofs_lo][body_ofs_hi] = 6 Bytes
     * Nop-Layout:  [0x00] = 1 Byte (Body)
     * Next-Layout: [0x0E][unused] = 2 Bytes
     * Evt_end:     [0x01] = 1 Byte
     */
    uint8_t bytecode[10];

    /* For(count=3) */
    bytecode[0] = SCD_OP_FOR;
    bytecode[1] = 0x00;
    write_le16(bytecode + 2, 3); /* count = 3 */
    write_le16(bytecode + 4, 0); /* body_offset (unused by impl) */

    /* Body: Nop */
    bytecode[6] = SCD_OP_NOP;

    /* Next */
    bytecode[7] = SCD_OP_NEXT;
    bytecode[8] = 0x00;

    /* Evt_end */
    bytecode[9] = SCD_OP_EVT_END;

    scd_vm_init();
    scd_thread_start(0, bytecode);

    scd_vm_tick();

    /*
     * Execution:
     *   For: for_depth=0, for_count[0]=3, for_body[0]=bytecode+6, PC+=6
     *   Nop: PC+=1 → bytecode[7]
     *   Next: count 3→2 >0 → PC=for_body[0]=bytecode[6]
     *   Nop: PC+=1 → bytecode[7]
     *   Next: count 2→1 >0 → PC=for_body[0]=bytecode[6]
     *   Nop: PC+=1 → bytecode[7]
     *   Next: count 1→0 → for_depth--, PC+=2 → bytecode[9]
     *   Evt_end: thread end
     */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_for_next_loop: Thread sollte inaktiv sein nach For(3)-Loop\n");
        return 1;
    }

    /* for_depth sollte -1 sein (keine aktive Schleife mehr) */
    if (g_scd.threads[0].for_depth != -1) {
        fprintf(stderr, "FAIL: test_for_next_loop: for_depth sollte -1 sein, ist %d\n",
                g_scd.threads[0].for_depth);
        return 1;
    }

    printf("PASS: test_for_next_loop\n");
    return 0;
}

/* =========================================================================
 * Test 8: Set + Ck — Flag setzen und pruefen
 * ========================================================================= */

static int test_ck_set_flags(void)
{
    /*
     * Set-Layout: [0x22][flag_group][flag_bit][value] = 4 Bytes
     * Ck-Layout:  [0x21][flag_group][flag_bit][unused] = 4 Bytes
     *
     * Bytecode: Set(group=5, bit=3, value=1) → Ck(group=5, bit=3) → Evt_end
     */
    uint8_t bytecode[9];

    /* Set(group=5, bit=3, value=1) */
    bytecode[0] = SCD_OP_SET;
    bytecode[1] = 5;    /* flag_group */
    bytecode[2] = 3;    /* flag_bit */
    bytecode[3] = 1;    /* value = setzen */

    /* Ck(group=5, bit=3) */
    bytecode[4] = SCD_OP_CK;
    bytecode[5] = 5;    /* flag_group */
    bytecode[6] = 3;    /* flag_bit */
    bytecode[7] = 0x00; /* unused */

    /* Evt_end */
    bytecode[8] = SCD_OP_EVT_END;

    scd_vm_init();
    /* Stelle sicher, dass Flag initial nicht gesetzt ist */
    g_scd.game_flags[5] = 0x00;
    g_scd.condition_flag = 0;

    scd_thread_start(0, bytecode);
    scd_vm_tick();

    /* Thread sollte inaktiv sein */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_ck_set_flags: Thread sollte inaktiv sein\n");
        return 1;
    }

    /* game_flags[5] Bit 3 muss gesetzt sein */
    if (!(g_scd.game_flags[5] & (1 << 3))) {
        fprintf(stderr, "FAIL: test_ck_set_flags: game_flags[5] Bit 3 nicht gesetzt\n");
        return 1;
    }

    /* condition_flag muss 1 sein (Ck hat Bit gefunden) */
    if (g_scd.condition_flag != 1) {
        fprintf(stderr, "FAIL: test_ck_set_flags: condition_flag sollte 1 sein, ist %d\n",
                g_scd.condition_flag);
        return 1;
    }

    printf("PASS: test_ck_set_flags\n");
    return 0;
}

/* =========================================================================
 * Test 9: Work_set — setzt work_slot und work_prop_idx im Thread
 * ========================================================================= */

static int test_work_set(void)
{
    /*
     * Work_set-Layout: [0x2E][slot][prop] = 3 Bytes
     * Evt_end: [0x01] = 1 Byte
     */
    uint8_t bytecode[4];

    bytecode[0] = SCD_OP_WORK_SET;
    bytecode[1] = 7;    /* slot */
    bytecode[2] = 12;   /* prop_idx */
    bytecode[3] = SCD_OP_EVT_END;

    scd_vm_init();
    scd_thread_start(0, bytecode);

    scd_vm_tick();

    /* Thread sollte inaktiv sein */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_work_set: Thread sollte inaktiv sein\n");
        return 1;
    }

    /* work_slot und work_prop_idx pruefen */
    if (g_scd.threads[0].work_slot != 7) {
        fprintf(stderr, "FAIL: test_work_set: work_slot sollte 7 sein, ist %d\n",
                g_scd.threads[0].work_slot);
        return 1;
    }
    if (g_scd.threads[0].work_prop_idx != 12) {
        fprintf(stderr, "FAIL: test_work_set: work_prop_idx sollte 12 sein, ist %d\n",
                g_scd.threads[0].work_prop_idx);
        return 1;
    }

    printf("PASS: test_work_set\n");
    return 0;
}

/* =========================================================================
 * Test 10: Speed_set — setzt thread->vel[axis]
 * ========================================================================= */

static int test_speed_set(void)
{
    /*
     * Speed_set-Layout: [0x2F][axis][speed_lo][speed_hi] = 4 Bytes
     * Evt_end: [0x01] = 1 Byte
     */
    uint8_t bytecode[5];

    bytecode[0] = SCD_OP_SPEED_SET;
    bytecode[1] = 2;    /* axis = vz */
    write_le16(bytecode + 2, 500); /* speed = 500 */
    bytecode[4] = SCD_OP_EVT_END;

    scd_vm_init();
    scd_thread_start(0, bytecode);

    scd_vm_tick();

    /* Thread sollte inaktiv sein */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_speed_set: Thread sollte inaktiv sein\n");
        return 1;
    }

    /* vel[2] (vz) muss 500 sein */
    if (g_scd.threads[0].vel[2] != 500) {
        fprintf(stderr, "FAIL: test_speed_set: vel[2] sollte 500 sein, ist %d\n",
                g_scd.threads[0].vel[2]);
        return 1;
    }

    printf("PASS: test_speed_set\n");
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(void)
{
    int failures = 0;

    printf("=== SCD Opcodes Unit Tests ===\n\n");

    failures += test_nop();
    failures += test_evt_end();
    failures += test_sleep_sleeping_cycle();
    failures += test_ifel_ck_true();
    failures += test_ifel_ck_false();
    failures += test_else_ck_skip();
    failures += test_for_next_loop();
    failures += test_ck_set_flags();
    failures += test_work_set();
    failures += test_speed_set();

    if (failures == 0) {
        printf("\nALL SCD OPCODE TESTS PASSED\n");
    } else {
        fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
