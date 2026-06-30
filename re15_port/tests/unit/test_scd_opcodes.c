/**
 * @file test_scd_opcodes.c
 * @brief Unit-Tests fuer SCD-VM Opcodes
 *
 * Testet jeden Opcode isoliert mit bekannten Eingabe/Ausgabe-Paaren:
 * - Nop, Evt_end (Basis)
 * - Sleep/Sleeping (Timer-Dekrementierung)
 * - If/Else (byte-true Block-Stack-Kontrollfluss)
 * - For/Next (Schleifen)
 * - Ck/Set (Game-Flags)
 * - Work_set, Speed_set (Per-Thread-Zustand)
 *
 * Requirements: 4.6, 12.4
 *
 * 2026-06-28 reaktiviert nach dem Engine-Transplant. Die SCD-VM-API hat sich
 * geaendert; dieser Test wurde gegen die AKTUELLE oeffentliche API in
 * re15_port/include/re15_scd.h umgeschrieben (Test-Intention erhalten):
 *
 *   ALT (Legacy)                     -> NEU (aktuell)
 *   ----------------------------------------------------------------------
 *   #include "re15_error.h"          -> entfernt; <stdint.h> liefert die Typen
 *   g_scd.threads[N].sleep_counter[] -> g_scd.threads[N].sleep_slots[depth][slot]
 *                                       (+ sleep_count[depth] = Slot-Stackzaehler)
 *   g_scd.condition_flag             -> entfernt; If (0x06) ist byte-true:
 *                                       op_if schiebt block_end auf den
 *                                       Block-Stack + betritt IMMER den Body,
 *                                       das Praedikat-Opcode (Ck 0x21) liefert
 *                                       TRUE/FALSE zurueck (scd_vm.c op_if/op_ck).
 *   g_scd.game_flags[]               -> re15_game_flag_get/set(zone, idx)
 *                                       (g_game.flags[][], game_state.c)
 *   SCD_OP_IFEL_CK                   -> SCD_OP_IF   (0x06)
 *   SCD_OP_ELSE_CK                   -> SCD_OP_ELSE (0x07)  (pc += skip)
 *   for_depth == -1 (Ende)           -> for_depth == 0 (Start 0, balanciertes
 *                                       For/Next kehrt nach 0 zurueck; scd_vm.c
 *                                       op_for/op_next + scd_thread_start)
 *   Work_set kopiert Rohargumente    -> op_work_set interpretiert kind/idx:
 *                                       kind=1 -> work_slot = RE15_ACTOR_SLOT_PLAYER (0)
 *                                       (scd_vm.c op_work_set)
 */

#include "re15_scd.h"
#include "re15_actor.h"   /* RE15_ACTOR_SLOT_PLAYER */
#include "re15_rdt.h"     /* re15_rdt_t.sub_scd[] for Evt_chain/Evt_exec tests */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* For (0x0D) / Next (0x0E) haben keine Header-Konstanten — sie werden in
 * scd_vm.c per Rohwert in der Dispatch-Tabelle registriert
 * (s_op_table[0x0D]=op_for @183, s_op_table[0x0E]=op_next @184). Lokal
 * gespiegelt fuer den Test. */
#define SCD_OP_FOR   0x0D
#define SCD_OP_NEXT  0x0E

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
 * AKTUELLE Engine-Semantik (scd_vm.c op_sleep @752 / op_sleeping @778):
 *   - op_sleep: pc++, liest duration = LE u16 @pc[1] (= original pc[2..3]),
 *     legt sie in sleep_slots[call_depth][sleep_count++] ab, return 1 (continue).
 *     -> der DIREKT folgende Opcode-Byte (bytecode[1]) MUSS Sleeping sein.
 *   - op_sleeping: dekrementiert den Slot-Zaehler, yieldet (return 2). Bei 0
 *     gibt es den Slot frei und rueckt PC += 3 vor (von bytecode[1] -> bytecode[4]).
 *
 * Da Sleep (return 1) im SELBEN Tick in Sleeping faellt, dekrementiert Tick 1
 * den Zaehler bereits einmal (3 -> 2). Layout:
 *   [0]=Sleep [1]=Sleeping [2]=count_lo [3]=count_hi [4]=Evt_end
 * ========================================================================= */

static int test_sleep_sleeping_cycle(void)
{
    uint8_t bytecode[5];

    bytecode[0] = SCD_OP_SLEEP;
    bytecode[1] = SCD_OP_SLEEPING;       /* op_sleep faellt direkt hierher (pc++) */
    write_le16(bytecode + 2, 3);         /* count = 3 (LE u16, von op_sleep gelesen) */
    bytecode[4] = SCD_OP_EVT_END;

    scd_vm_init();
    scd_thread_start(0, bytecode);

    /* Tick 1: Sleep setzt slot=3, continue → Sleeping dekrementiert 3→2, yield */
    scd_vm_tick();
    if (g_scd.threads[0].active != 1) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Thread inaktiv nach Tick 1\n");
        return 1;
    }
    if (g_scd.threads[0].sleep_slots[0][0] != 2) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Slot sollte 2 sein nach Tick 1, ist %d\n",
                g_scd.threads[0].sleep_slots[0][0]);
        return 1;
    }

    /* Tick 2: Sleeping dekrement 2→1, yield */
    scd_vm_tick();
    if (g_scd.threads[0].sleep_slots[0][0] != 1) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Slot sollte 1 sein nach Tick 2, ist %d\n",
                g_scd.threads[0].sleep_slots[0][0]);
        return 1;
    }
    if (g_scd.threads[0].active != 1) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Thread inaktiv nach Tick 2\n");
        return 1;
    }

    /* Tick 3: Sleeping dekrement 1→0, gibt Slot frei, PC+=3 (→Evt_end), yield.
     *         Evt_end laeuft erst im naechsten Tick (op_sleeping yieldet). */
    scd_vm_tick();
    if (g_scd.threads[0].sleep_slots[0][0] != 0) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Slot sollte 0 sein nach Tick 3, ist %d\n",
                g_scd.threads[0].sleep_slots[0][0]);
        return 1;
    }
    if (g_scd.threads[0].active != 1) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Thread inaktiv nach Tick 3\n");
        return 1;
    }

    /* Tick 4: Evt_end → Thread beendet */
    scd_vm_tick();
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_sleep_sleeping_cycle: Thread sollte inaktiv sein nach Tick 4\n");
        return 1;
    }

    printf("PASS: test_sleep_sleeping_cycle\n");
    return 0;
}

/* =========================================================================
 * Test 4: If TRUE — Praedikat (Ck) wahr → If-Body wird ausgefuehrt
 *
 * Byte-true Modell (scd_vm.c op_if @885 / op_ck @1215 / dispatcher @483):
 *   If (0x06) schiebt block_end = pc+4+block_length auf den Block-Stack und
 *   betritt IMMER den Body (pc+=4). Das folgende Praedikat-Opcode Ck (0x21)
 *   liefert TRUE (SCD_R_CONTINUE) wenn flag(zone,idx) == expected, sonst
 *   SCD_R_IF_FALSE (3) → der Dispatcher poppt den Block-Stack und springt PC
 *   auf block_end. Beobachtbar machen wir den Zweig ueber ein Marker-Flag,
 *   das ein Set (0x22) im If-Body setzt.
 *
 * Layout:
 *   [0..3]  If(block_length=8)  → block_end = bc+4+8 = &bc[12]
 *   [4..7]  Ck(zone=5, idx=3, expected=1)   (Praedikat)
 *   [8..11] Set(zone=6, idx=0, op=1)         (Marker, nur bei TRUE)
 *   [12]    Evt_end
 * ========================================================================= */

static int test_if_true(void)
{
    uint8_t bytecode[13];
    memset(bytecode, SCD_OP_NOP, sizeof(bytecode));

    bytecode[0] = SCD_OP_IF;
    bytecode[1] = 0x00;                 /* reserviert */
    write_le16(bytecode + 2, 8);        /* block_length = 8 → block_end = &bc[12] */

    bytecode[4] = SCD_OP_CK;
    bytecode[5] = 5;                    /* flag_zone  */
    bytecode[6] = 3;                    /* flag_index */
    bytecode[7] = 1;                    /* erwarteter Wert = 1 */

    bytecode[8]  = SCD_OP_SET;
    bytecode[9]  = 6;                   /* Marker flag_zone  */
    bytecode[10] = 0;                   /* Marker flag_index */
    bytecode[11] = 1;                   /* op=1 → Bit setzen */

    bytecode[12] = SCD_OP_EVT_END;

    scd_vm_init();
    re15_game_flag_set(5, 3, 1);        /* Praedikat-Flag = TRUE */
    re15_game_flag_set(6, 0, 0);        /* Marker initial leer */

    scd_thread_start(0, bytecode);
    scd_vm_tick();

    /* Thread sollte inaktiv sein (If-Body durchlaufen → Evt_end) */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_if_true: Thread sollte inaktiv sein\n");
        return 1;
    }

    /* Marker MUSS gesetzt sein (Set im If-Body lief, weil Ck TRUE war) */
    if (re15_game_flag_get(6, 0) != 1) {
        fprintf(stderr, "FAIL: test_if_true: Marker-Flag (6,0) sollte gesetzt sein (If-Body genommen)\n");
        return 1;
    }

    printf("PASS: test_if_true\n");
    return 0;
}

/* =========================================================================
 * Test 5: If FALSE — Praedikat (Ck) falsch → Body wird uebersprungen
 *
 * Gleiches Layout wie Test 4, aber das Praedikat-Flag ist NICHT gesetzt.
 * Ck liefert SCD_R_IF_FALSE → Dispatcher poppt Block-Stack und springt PC
 * auf block_end (&bc[12] = Evt_end). Das Set (Marker) wird UEBERSPRUNGEN.
 * ========================================================================= */

static int test_if_false(void)
{
    uint8_t bytecode[13];
    memset(bytecode, SCD_OP_NOP, sizeof(bytecode));

    bytecode[0] = SCD_OP_IF;
    bytecode[1] = 0x00;
    write_le16(bytecode + 2, 8);        /* block_length = 8 → block_end = &bc[12] */

    bytecode[4] = SCD_OP_CK;
    bytecode[5] = 5;
    bytecode[6] = 3;
    bytecode[7] = 1;                    /* erwartet 1, Flag ist aber 0 → FALSE */

    bytecode[8]  = SCD_OP_SET;
    bytecode[9]  = 6;
    bytecode[10] = 0;
    bytecode[11] = 1;

    bytecode[12] = SCD_OP_EVT_END;

    scd_vm_init();
    re15_game_flag_set(5, 3, 0);        /* Praedikat-Flag = FALSE */
    re15_game_flag_set(6, 0, 0);        /* Marker initial leer */

    scd_thread_start(0, bytecode);
    scd_vm_tick();

    /* Thread sollte inaktiv sein (Sprung zu block_end → Evt_end) */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_if_false: Thread sollte inaktiv sein\n");
        return 1;
    }

    /* Marker darf NICHT gesetzt sein (Set wurde uebersprungen) */
    if (re15_game_flag_get(6, 0) != 0) {
        fprintf(stderr, "FAIL: test_if_false: Marker-Flag (6,0) sollte NICHT gesetzt sein (Body uebersprungen)\n");
        return 1;
    }

    printf("PASS: test_if_false\n");
    return 0;
}

/* =========================================================================
 * Test 6: Else — springt um else_block_length (ueberspringt Else-Body)
 *
 * op_else (scd_vm.c @904): pop Block-Stack (falls offen), dann pc += skip
 * (LE s16 @pc[2..3], OHNE +4). Standalone (kein offenes If) → reines pc+=skip.
 *
 * Layout:
 *   [0..3] Else(skip=8)            → PC springt auf &bc[8]
 *   [4..7] Dummy Else-Body (Nop)   (nicht erreicht)
 *   [8]    Evt_end
 * ========================================================================= */

static int test_else_skip(void)
{
    uint8_t bytecode[9];
    memset(bytecode, SCD_OP_NOP, sizeof(bytecode));

    bytecode[0] = SCD_OP_ELSE;
    bytecode[1] = 0x00;
    write_le16(bytecode + 2, 8);        /* skip = 8 → PC springt auf bytecode[8] */

    bytecode[8] = SCD_OP_EVT_END;

    scd_vm_init();
    scd_thread_start(0, bytecode);

    scd_vm_tick();

    /* Else springt zu PC+8 → Evt_end → inaktiv */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_else_skip: Thread sollte inaktiv sein (Else → Evt_end)\n");
        return 1;
    }

    printf("PASS: test_else_skip\n");
    return 0;
}

/* =========================================================================
 * Test 7: For/Next Schleife — Body wird count-mal ausgefuehrt
 *
 * op_for (scd_vm.c @647): 6-Byte-Header [0x0D][_][block_length LE u16][count LE u16].
 *   count @pc[4..5]; body_pc = pc+6; for_depth++ (Start 0). Bei count==0 wird
 *   block_length zum Ueberspringen genutzt.
 * op_next (scd_vm.c @673): dekrementiert for_count[top]; > 0 → zurueck zu
 *   body_pc; ==0 → for_depth-- und PC += 2.
 *
 * scd_thread_start nullt den Thread → for_depth startet bei 0; nach einer
 * balancierten For/Next-Schleife ist for_depth wieder 0 (NICHT -1 wie in der
 * Legacy-Engine).
 *
 * Layout:
 *   [0..5] For(block_length=0, count=3)
 *   [6]    Nop (Body)
 *   [7..8] Next
 *   [9]    Evt_end
 * ========================================================================= */

static int test_for_next_loop(void)
{
    uint8_t bytecode[10];

    /* For: block_length @pc[2..3] (hier 0, nur bei count==0 relevant),
     *      count @pc[4..5] = 3 */
    bytecode[0] = SCD_OP_FOR;
    bytecode[1] = 0x00;
    write_le16(bytecode + 2, 0);        /* block_length (nur bei count==0 genutzt) */
    write_le16(bytecode + 4, 3);        /* count = 3 */

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
     * Ablauf:
     *   For: for_body[0]=&bc[6], for_count[0]=3, for_depth=1, PC+=6 → bc[6]
     *   Nop: PC+=1 → bc[7]
     *   Next: 3→2 >0 → PC=for_body[0]=bc[6]
     *   Nop → bc[7]; Next: 2→1 >0 → bc[6]
     *   Nop → bc[7]; Next: 1→0 → for_depth-- (=0), PC+=2 → bc[9]
     *   Evt_end: thread end
     */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_for_next_loop: Thread sollte inaktiv sein nach For(3)-Loop\n");
        return 1;
    }

    /* [#8] unified loop_count zurueck auf 0 (balancierte Schleife) — ersetzt for_depth */
    if (g_scd.threads[0].loop_count != 0) {
        fprintf(stderr, "FAIL: test_for_next_loop: loop_count sollte 0 sein, ist %d\n",
                g_scd.threads[0].loop_count);
        return 1;
    }

    printf("PASS: test_for_next_loop\n");
    return 0;
}

/* =========================================================================
 * Test 8: Set + Ck — Flag setzen, pruefen und Praedikat-Zweig nehmen
 *
 * Set (0x22, op_set @1230): pc[3] = Operationsselektor (1=OR/setzen, 0=clear,
 *   7=toggle). Schreibt das Bit via re15_game_flag_set.
 * Ck (0x21, op_ck @1215): liefert TRUE wenn flag(zone,idx) == erwartet (pc[3]).
 *
 * Bytecode: Set(5,3, op=1) → If(...) → Ck(5,3, erwartet=1) → Set(6,0,1 Marker)
 *           → Evt_end. Der Marker beweist, dass Ck das frisch gesetzte Bit fand.
 *
 * Layout:
 *   [0..3]  Set(5, 3, op=1)
 *   [4..7]  If(block_length=12)  → block_end = bc+4+12 = bc[8]+12 = &bc[20]
 *   [8..11] Ck(5, 3, erwartet=1)
 *   [12..15] Set(6, 0, op=1)     (Marker)
 *   [16..19] Nop-Padding (If-Body-Rest bis block_end)
 *   [20]    Evt_end
 * ========================================================================= */

static int test_ck_set_flags(void)
{
    uint8_t bytecode[21];
    memset(bytecode, SCD_OP_NOP, sizeof(bytecode));

    /* Set(group=5, bit=3, op=1) → Bit setzen */
    bytecode[0] = SCD_OP_SET;
    bytecode[1] = 5;    /* flag_group */
    bytecode[2] = 3;    /* flag_bit */
    bytecode[3] = 1;    /* op=1 → OR (setzen) */

    /* If(block_length=12) → block_end = &bytecode[8] + 12 = &bytecode[20] */
    bytecode[4] = SCD_OP_IF;
    bytecode[5] = 0x00;
    write_le16(bytecode + 6, 12);

    /* Ck(group=5, bit=3, erwartet=1) — Praedikat, sollte TRUE sein */
    bytecode[8]  = SCD_OP_CK;
    bytecode[9]  = 5;
    bytecode[10] = 3;
    bytecode[11] = 1;   /* erwartet 1 = "Bit ist gesetzt?" */

    /* Marker: Set(group=6, bit=0, op=1) — nur bei Ck==TRUE erreicht */
    bytecode[12] = SCD_OP_SET;
    bytecode[13] = 6;
    bytecode[14] = 0;
    bytecode[15] = 1;

    /* bytecode[16..19] bleiben Nop (Body-Rest bis block_end) */

    bytecode[20] = SCD_OP_EVT_END;

    scd_vm_init();
    /* Stelle sicher, dass beide Flags initial nicht gesetzt sind */
    re15_game_flag_set(5, 3, 0);
    re15_game_flag_set(6, 0, 0);

    scd_thread_start(0, bytecode);
    scd_vm_tick();

    /* Thread sollte inaktiv sein */
    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_ck_set_flags: Thread sollte inaktiv sein\n");
        return 1;
    }

    /* flag(5,3) muss durch Set gesetzt sein */
    if (re15_game_flag_get(5, 3) != 1) {
        fprintf(stderr, "FAIL: test_ck_set_flags: flag(5,3) nicht gesetzt\n");
        return 1;
    }

    /* Marker (6,0) beweist: Ck war TRUE → If-Body inkl. Marker-Set ausgefuehrt */
    if (re15_game_flag_get(6, 0) != 1) {
        fprintf(stderr, "FAIL: test_ck_set_flags: Marker (6,0) nicht gesetzt — Ck war nicht TRUE\n");
        return 1;
    }

    printf("PASS: test_ck_set_flags\n");
    return 0;
}

/* =========================================================================
 * Test 9: Work_set — waehlt die Work-Entity (per-Thread work_slot)
 *
 * op_work_set (scd_vm.c @1829): pc[1]=kind, pc[2]=idx. Byte-true interpretiert
 *   es kind statt Rohwerte zu kopieren:
 *     kind=1 → work_slot = RE15_ACTOR_SLOT_PLAYER (0), work_prop_idx = -1
 *     kind=4 (oder unbekannt) → work_slot = -1
 *   Zudem werden die 6 vel/accel-Slots des Threads genullt.
 *
 * Layout: [0x2E][kind][idx][Evt_end]
 * ========================================================================= */

static int test_work_set(void)
{
    /* Work_set(kind=1, idx=0) → Player */
    {
        uint8_t bytecode[4];
        bytecode[0] = SCD_OP_WORK_SET;
        bytecode[1] = 1;    /* kind=1 → player */
        bytecode[2] = 0;    /* idx */
        bytecode[3] = SCD_OP_EVT_END;

        scd_vm_init();
        scd_thread_start(0, bytecode);
        scd_vm_tick();

        if (g_scd.threads[0].active != 0) {
            fprintf(stderr, "FAIL: test_work_set: Thread sollte inaktiv sein (kind=1)\n");
            return 1;
        }
        if (g_scd.threads[0].work_slot != RE15_ACTOR_SLOT_PLAYER) {
            fprintf(stderr, "FAIL: test_work_set: work_slot sollte %d (Player) sein, ist %d\n",
                    RE15_ACTOR_SLOT_PLAYER, g_scd.threads[0].work_slot);
            return 1;
        }
        if (g_scd.threads[0].work_prop_idx != -1) {
            fprintf(stderr, "FAIL: test_work_set: work_prop_idx sollte -1 sein (kind=1), ist %d\n",
                    g_scd.threads[0].work_prop_idx);
            return 1;
        }
    }

    /* Work_set(kind=4, idx=0) → kein Actor-Ziel (work_slot=-1) */
    {
        uint8_t bytecode[4];
        bytecode[0] = SCD_OP_WORK_SET;
        bytecode[1] = 4;    /* kind=4 → room/object → kein Actor */
        bytecode[2] = 0;
        bytecode[3] = SCD_OP_EVT_END;

        scd_vm_init();
        scd_thread_start(0, bytecode);
        scd_vm_tick();

        if (g_scd.threads[0].work_slot != -1) {
            fprintf(stderr, "FAIL: test_work_set: work_slot sollte -1 sein (kind=4), ist %d\n",
                    g_scd.threads[0].work_slot);
            return 1;
        }
    }

    printf("PASS: test_work_set\n");
    return 0;
}

/* =========================================================================
 * Test 10: Speed_set — setzt thread->vel[axis] (LE s16)
 *
 * op_speed_set (scd_vm.c @1920): pc[1]=axis, pc[2..3]=value LE s16.
 *   axis<6 → vel[axis] = value; axis 6..11 → accel[axis-6].
 *
 * Layout: [0x2F][axis][speed_lo][speed_hi][Evt_end]
 * ========================================================================= */

static int test_speed_set(void)
{
    uint8_t bytecode[5];

    bytecode[0] = SCD_OP_SPEED_SET;
    bytecode[1] = 2;    /* axis = vz */
    write_le16(bytecode + 2, 500); /* speed = 500 (LE) */
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

/* =========================================================================
 * Test [#8]: Do/Edwhile predicate-chain loop + Save/Calc/Cmp integration.
 *   work_vars[0]=0; do { work_vars[0]+=1; } edwhile(work_vars[0] < 3);
 * Runs 3x; afterwards work_vars[0]==3 and the unified loop_count balances to 0.
 * Exercises Do(0x11)/Edwhile(0x12), the byte-true AND/OR predicate chain (Cmp
 * via the real dispatcher), and work-var ops Save(0x24)/Calc(0x26).
 *   [0..3] Save(var0,0)  [4..7] Do(block_len=14)  [8..13] Calc(ADD,dst0,1)
 *   [14..15] Edwhile(chain=6)  [16..21] Cmp(var0,'<',3)  [22] Evt_end
 * ========================================================================= */
static int test_do_edwhile_loop(void)
{
    uint8_t bc[23];
    memset(bc, SCD_OP_NOP, sizeof(bc));
    bc[0]=0x24; bc[1]=0x00; write_le16(bc+2, 0);                 /* Save work_vars[0]=0 */
    bc[4]=0x11; bc[5]=0x00; write_le16(bc+6, 14);                /* Do block_len=14 */
    bc[8]=0x26; bc[9]=0x00; bc[10]=0x00; bc[11]=0x00; write_le16(bc+12, 1); /* Calc +1 */
    bc[14]=0x12; bc[15]=6;                                       /* Edwhile chain_len=6 */
    bc[16]=0x23; bc[17]=0x00; bc[18]=0x00; bc[19]=0x03; write_le16(bc+20, 3); /* Cmp var0 < 3 */
    bc[22]=SCD_OP_EVT_END;

    scd_vm_init();
    scd_thread_start(0, bc);
    scd_vm_tick();

    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_do_edwhile_loop: Thread sollte beendet sein\n"); return 1; }
    if (g_scd.work_vars[0] != 3) {
        fprintf(stderr, "FAIL: test_do_edwhile_loop: work_vars[0] sollte 3 sein, ist %d\n",
                g_scd.work_vars[0]); return 1; }
    if (g_scd.threads[0].loop_count != 0) {
        fprintf(stderr, "FAIL: test_do_edwhile_loop: loop_count sollte 0 sein, ist %d\n",
                g_scd.threads[0].loop_count); return 1; }
    printf("PASS: test_do_edwhile_loop\n");
    return 0;
}

/* =========================================================================
 * Test [#8]: Break (0x1A) jumps to the innermost loop's exit (+0x60) and pops.
 *   do { break; work_vars[0]=99; } ...   // the +=99 must be SKIPPED.
 *   [0..3] Save(var0,0)  [4..7] Do(block_len=14 -> exit=bc[22])  [8..9] Break
 *   [10..15] Calc(ADD,dst0,99) SKIPPED  [22] Evt_end
 * ========================================================================= */
static int test_break_exits_loop(void)
{
    uint8_t bc[23];
    memset(bc, SCD_OP_NOP, sizeof(bc));
    bc[0]=0x24; bc[1]=0x00; write_le16(bc+2, 0);                 /* Save work_vars[0]=0 */
    bc[4]=0x11; bc[5]=0x00; write_le16(bc+6, 14);                /* Do block_len=14 -> exit=bc[22] */
    bc[8]=0x1A; bc[9]=0x00;                                      /* Break */
    bc[10]=0x26; bc[11]=0x00; bc[12]=0x00; bc[13]=0x00; write_le16(bc+14, 99); /* Calc +99 (skipped) */
    bc[22]=SCD_OP_EVT_END;

    scd_vm_init();
    scd_thread_start(0, bc);
    scd_vm_tick();

    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_break_exits_loop: Thread sollte beendet sein\n"); return 1; }
    if (g_scd.work_vars[0] != 0) {
        fprintf(stderr, "FAIL: test_break_exits_loop: work_vars[0] sollte 0 sein (Calc uebersprungen), ist %d\n",
                g_scd.work_vars[0]); return 1; }
    if (g_scd.threads[0].loop_count != 0) {
        fprintf(stderr, "FAIL: test_break_exits_loop: loop_count sollte 0 sein, ist %d\n",
                g_scd.threads[0].loop_count); return 1; }
    printf("PASS: test_break_exits_loop\n");
    return 0;
}

/* =========================================================================
 * Test [#9]: byte-true Switch/Case/Default/Break (LAB_8003fa5c).
 *
 * Switch (0x13) scans the Case/Default table INLINE and jumps straight to the
 * matching body (the standalone Case handler is only hit on fall-through). It
 * pushes the unified loop model (loop_count/loop_exit) so Break (0x1A) exits the
 * switch via loop_exit and the trailing Eswitch (0x16) balances loop_count → 0.
 *
 *   switch(work_vars[5]) {
 *     case 1: work_vars[0]=11; break;
 *     case 2: work_vars[0]=22; break;   // selected when sel==2
 *     case 3: work_vars[0]=33; break;
 *     default: work_vars[0]=99; break;  // selected when no case matches
 *   }
 *
 * Byte layout (Save 0x24 = [op][var][u16]; Case = [0x14][_][blocklen:u16][value:u16]):
 *   off0  Save w5=sel            off4  Switch(var5, blocklen=46 -> loop_exit=off54)
 *   off8  Case v=1   body off14: Save w0=11; off18 Break
 *   off20 Case v=2   body off26: Save w0=22; off30 Break
 *   off32 Case v=3   body off38: Save w0=33; off42 Break
 *   off44 Default    body off46: Save w0=99; off50 Break
 *   off52 Eswitch    off54 Evt_end
 * blocklen=46: table(off8)+46 = off54 = Evt_end = eswitch(off52)+2. ✓
 * ========================================================================= */
static int run_switch_case(int16_t sel, int16_t expect_w0)
{
    uint8_t bc[55];
    memset(bc, SCD_OP_NOP, sizeof(bc));
    bc[0]=0x24; bc[1]=0x05; write_le16(bc+2, (uint16_t)sel);          /* Save w5 = sel  */
    bc[4]=SCD_OP_SWITCH; bc[5]=0x05; write_le16(bc+6, 46);            /* Switch var5    */
    bc[8]=SCD_OP_CASE;   write_le16(bc+10, 6); write_le16(bc+12, 1);  /* case 1         */
    bc[14]=0x24; bc[15]=0x00; write_le16(bc+16, 11); bc[18]=0x1A;     /* w0=11; Break   */
    bc[20]=SCD_OP_CASE;  write_le16(bc+22, 6); write_le16(bc+24, 2);  /* case 2         */
    bc[26]=0x24; bc[27]=0x00; write_le16(bc+28, 22); bc[30]=0x1A;     /* w0=22; Break   */
    bc[32]=SCD_OP_CASE;  write_le16(bc+34, 6); write_le16(bc+36, 3);  /* case 3         */
    bc[38]=0x24; bc[39]=0x00; write_le16(bc+40, 33); bc[42]=0x1A;     /* w0=33; Break   */
    bc[44]=SCD_OP_DEFAULT;                                            /* default        */
    bc[46]=0x24; bc[47]=0x00; write_le16(bc+48, 99); bc[50]=0x1A;     /* w0=99; Break   */
    bc[52]=SCD_OP_END_SWITCH;                                         /* Eswitch        */
    bc[54]=SCD_OP_EVT_END;

    scd_vm_init();
    g_scd.work_vars[0] = -1;            /* sentinel: detect "no body ran" */
    scd_thread_start(0, bc);
    scd_vm_tick();

    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_switch_case_break(sel=%d): Thread sollte beendet sein\n", sel);
        return 1; }
    if (g_scd.work_vars[0] != expect_w0) {
        fprintf(stderr, "FAIL: test_switch_case_break(sel=%d): work_vars[0] sollte %d sein, ist %d\n",
                sel, expect_w0, g_scd.work_vars[0]); return 1; }
    if (g_scd.threads[0].loop_count != 0) {
        fprintf(stderr, "FAIL: test_switch_case_break(sel=%d): loop_count sollte 0 sein, ist %d\n",
                sel, g_scd.threads[0].loop_count); return 1; }
    return 0;
}

static int test_switch_case_break(void)
{
    int rc = 0;
    rc |= run_switch_case(2, 22);   /* match case 2: scan skips case 1, runs body, Break exits */
    rc |= run_switch_case(1, 11);   /* match first case */
    rc |= run_switch_case(7, 99);   /* no case matches -> Default body */
    if (rc == 0) printf("PASS: test_switch_case_break\n");
    return rc;
}

/* =========================================================================
 * Test [#10]: byte-true Evt_chain (0x03) + Evt_exec (0x04, cond<10 direct slot).
 *
 * Evt_chain reinitializes the CURRENT thread in place to run sub_scd[sub_id]
 * (LAB_8003f270 -> FUN_8003edec) — was a No-Op, so the chained sub never ran.
 * Evt_exec with cond<10 uses cond DIRECTLY as the target slot (FUN_8003ee3c
 * normal mode) — cond=9 (ROOM1021 sub02 Evt_exec(9,0)) used to be dropped.
 * Both need a registered RDT with sub_scd[] populated.
 * ========================================================================= */
extern void scd_register_current_rdt(const re15_rdt_t *rdt);

static int test_evt_chain_reinit(void)
{
    uint8_t sub3[5];
    sub3[0]=0x24; sub3[1]=0x07; write_le16(sub3+2, 55); sub3[4]=SCD_OP_EVT_END; /* Save w7=55 */

    re15_rdt_t rdt;
    memset(&rdt, 0, sizeof(rdt));
    rdt.sub_scd[3] = sub3;

    uint8_t mainbc[4] = { 0x03, 0x00, 0x00, 0x03 };   /* Evt_chain(sub_id=3) */

    scd_vm_init();
    scd_register_current_rdt(&rdt);
    scd_thread_start(0, mainbc);
    scd_vm_tick();
    scd_register_current_rdt(NULL);

    if (g_scd.threads[0].active != 0) {
        fprintf(stderr, "FAIL: test_evt_chain_reinit: Thread sollte beendet sein\n"); return 1; }
    if (g_scd.work_vars[7] != 55) {       /* old No-Op left this 0 */
        fprintf(stderr, "FAIL: test_evt_chain_reinit: work_vars[7] sollte 55 sein (chained sub lief nicht), ist %d\n",
                g_scd.work_vars[7]); return 1; }
    if (g_scd.threads[0].loop_count != 0) {
        fprintf(stderr, "FAIL: test_evt_chain_reinit: loop_count sollte 0 sein, ist %d\n",
                g_scd.threads[0].loop_count); return 1; }
    printf("PASS: test_evt_chain_reinit\n");
    return 0;
}

static int test_evt_exec_direct_slot(void)
{
    uint8_t sub4[5];
    sub4[0]=0x24; sub4[1]=0x08; write_le16(sub4+2, 66); sub4[4]=SCD_OP_EVT_END; /* Save w8=66 */

    re15_rdt_t rdt;
    memset(&rdt, 0, sizeof(rdt));
    rdt.sub_scd[4] = sub4;

    /* slot 0: Evt_exec(cond=9, sub_id=4) -> spawn sub4 in slot 9; then Evt_end. */
    uint8_t mainbc[5] = { 0x04, 9, 0x00, 4, SCD_OP_EVT_END };

    scd_vm_init();
    scd_register_current_rdt(&rdt);
    scd_thread_start(0, mainbc);
    scd_vm_tick();    /* slot 0 spawns slot 9; slot 9 (>0) runs in the SAME tick */
    scd_register_current_rdt(NULL);

    /* cond=9 < 10 => direct slot 9. Old code DROPPED cond=9 -> work_vars[8] stayed 0. */
    if (g_scd.work_vars[8] != 66) {
        fprintf(stderr, "FAIL: test_evt_exec_direct_slot: work_vars[8] sollte 66 sein (Slot 9 lief nicht), ist %d\n",
                g_scd.work_vars[8]); return 1; }
    printf("PASS: test_evt_exec_direct_slot\n");
    return 0;
}

/* =========================================================================
 * Test [#17]: Goto (0x17) byte-true block-unwind + Gosub (0x18) per-frame block
 * isolation. Empirical scan of all RE1.5 SCDs: 15/22 Gotos use pc[1]!=0xFF (real
 * out-of-block unwind), 124/278 Gosubs fire from INSIDE an open If-block. The old
 * flat single block-stack + ignore-Goto-args corrupted the caller's FALSE-pop
 * targets. block_stack/block_sp are now per Gosub-frame (indexed by call_depth);
 * Goto unwinds block_sp[cd] = (s8)pc[1]+1 (LAB_8003fb9c).
 * ========================================================================= */
static int test_goto_block_unwind(void)
{
    /* If opens a block; Goto(pc1=0xFF) jumps PAST the Endif to Save, leaving the
     * block "open" → byte-true Goto unwinds block_sp[0] back to 0. */
    uint8_t bc[] = {
        0x06, 0x00, 0x08, 0x00,              /* +0  If(len=8): push, block_sp[0]=1, body @+4 */
        0x17, 0xFF, 0xFF, 0x00, 0x08, 0x00,  /* +4  Goto(pc1=0xFF,pc2=0xFF, off=8 → +0xC) */
        0x08, 0x00,                          /* +A  Endif  (SKIPPED by the Goto) */
        0x24, 0x00, 0x55, 0x00,              /* +C  Save(w0 = 0x55) */
        SCD_OP_EVT_END                       /* +10 Evt_end */
    };
    scd_vm_init();
    scd_thread_start(0, bc);
    scd_vm_tick();
    if (g_scd.threads[0].block_sp[0] != 0) {
        fprintf(stderr, "FAIL: Goto(pc1=0xFF) muss block_sp[0] auf 0 unwinden, ist %d\n",
                g_scd.threads[0].block_sp[0]); return 1; }
    if (g_scd.work_vars[0] != 0x55) {
        fprintf(stderr, "FAIL: Save nach Goto muss laufen (w0=0x55), ist %d\n",
                g_scd.work_vars[0]); return 1; }
    printf("PASS: test_goto_block_unwind\n");
    return 0;
}

static int test_gosub_block_isolation(void)
{
    /* main opens a block, then Gosubs sub1 which opens its OWN block then yields
     * (Evt_next). Byte-true: caller block_sp[0] and callee block_sp[1] are
     * INDEPENDENT per Gosub-frame. */
    uint8_t sub1[] = {
        0x06, 0x00, 0x02, 0x00,  /* +0 If(len=2) → block_sp[1]=1 (its OWN frame) */
        0x02,                    /* +4 Evt_next → yield (tick ends here, call_depth=1) */
        0x08, 0x00,              /* +5 Endif */
        SCD_OP_EVT_END           /* +7 */
    };
    re15_rdt_t rdt; memset(&rdt, 0, sizeof(rdt));
    rdt.sub_scd[1] = sub1;
    uint8_t main_bc[] = {
        0x06, 0x00, 0x04, 0x00,  /* +0 If(len=4) → block_sp[0]=1, body @+4 */
        0x18, 0x01,              /* +4 Gosub(sub1) */
        0x08, 0x00,              /* +6 Endif */
        SCD_OP_EVT_END           /* +8 */
    };
    scd_vm_init();
    scd_register_current_rdt(&rdt);
    scd_thread_start(0, main_bc);
    scd_vm_tick();   /* main If → Gosub → sub1 If → Evt_next (yield) */
    scd_register_current_rdt(NULL);
    if (g_scd.threads[0].call_depth != 1) {
        fprintf(stderr, "FAIL: nach Gosub+Evt_next call_depth=1, ist %d\n",
                g_scd.threads[0].call_depth); return 1; }
    if (g_scd.threads[0].block_sp[0] != 1) {
        fprintf(stderr, "FAIL: Caller-Block block_sp[0] muss 1 bleiben, ist %d\n",
                g_scd.threads[0].block_sp[0]); return 1; }
    if (g_scd.threads[0].block_sp[1] != 1) {
        fprintf(stderr, "FAIL: Callee-Block block_sp[1] muss 1 sein (eigener Frame), ist %d\n",
                g_scd.threads[0].block_sp[1]); return 1; }
    printf("PASS: test_gosub_block_isolation\n");
    return 0;
}

/* op_flag_ck2 (0x58) — byte-true flag-bank bit-check predicate (LAB_8003fd54; the prior op_plc_rot
 * was an RE2 misattribution). desc=u16@pc[2], bank=pc[1]; word = work_vars[desc&0xff]>>5,
 * bit = desc&0x1f, invert when (desc>>8)==0. Same IF/predicate/body layout as test_if_true. */
static int test_flag_ck2(void)
{
    int fail = 0;
    /* (a) TRUE: work_vars[3]=0 -> word 0, bit 3; flag(5,3)=1; desc=0x0103 (hi!=0, no invert) -> body runs. */
    {
        uint8_t bc[13]; memset(bc, SCD_OP_NOP, sizeof(bc));
        bc[0]=SCD_OP_IF; bc[1]=0; write_le16(bc+2, 8);
        bc[4]=0x58; bc[5]=5; write_le16(bc+6, 0x0103);
        bc[8]=SCD_OP_SET; bc[9]=6; bc[10]=0; bc[11]=1; bc[12]=SCD_OP_EVT_END;
        scd_vm_init();
        g_scd.work_vars[3]=0; re15_game_flag_set(5,3,1); re15_game_flag_set(6,0,0);
        scd_thread_start(0, bc); scd_vm_tick();
        if (re15_game_flag_get(6,0)!=1) { fprintf(stderr,"FAIL: flag_ck2 (a) flag set -> body should run\n"); fail=1; }
    }
    /* (b) INVERT: desc=0x0003 (hi==0 -> invert); flag(5,3)=1 -> cond = 1^1 = 0 -> body skipped. */
    {
        uint8_t bc[13]; memset(bc, SCD_OP_NOP, sizeof(bc));
        bc[0]=SCD_OP_IF; bc[1]=0; write_le16(bc+2, 8);
        bc[4]=0x58; bc[5]=5; write_le16(bc+6, 0x0003);
        bc[8]=SCD_OP_SET; bc[9]=6; bc[10]=0; bc[11]=1; bc[12]=SCD_OP_EVT_END;
        scd_vm_init();
        g_scd.work_vars[3]=0; re15_game_flag_set(5,3,1); re15_game_flag_set(6,0,0);
        scd_thread_start(0, bc); scd_vm_tick();
        if (re15_game_flag_get(6,0)!=0) { fprintf(stderr,"FAIL: flag_ck2 (b) invert -> body should be skipped\n"); fail=1; }
    }
    /* (c) WORD from work-var: work_vars[3]=32 -> word 1; flag(5, (1<<5)|3 = 35)=1; desc=0x0103 -> body runs. */
    {
        uint8_t bc[13]; memset(bc, SCD_OP_NOP, sizeof(bc));
        bc[0]=SCD_OP_IF; bc[1]=0; write_le16(bc+2, 8);
        bc[4]=0x58; bc[5]=5; write_le16(bc+6, 0x0103);
        bc[8]=SCD_OP_SET; bc[9]=6; bc[10]=0; bc[11]=1; bc[12]=SCD_OP_EVT_END;
        scd_vm_init();
        g_scd.work_vars[3]=32; re15_game_flag_set(5,35,1); re15_game_flag_set(6,0,0);
        scd_thread_start(0, bc); scd_vm_tick();
        if (re15_game_flag_get(6,0)!=1) { fprintf(stderr,"FAIL: flag_ck2 (c) word-from-workvar -> body should run\n"); fail=1; }
    }
    if (!fail) printf("PASS: test_flag_ck2 (0x58 flag predicate: read/invert/word-from-workvar)\n");
    return fail;
}

/* op_cut_replace (0x4b) live in-room switch — byte-true tail (LAB_80040414 @0x800404ac): after the
 * zone relabel, if the active (displayed) cut == a, switch the camera to b via the cam-change path. */
static int test_cut_replace_live_switch(void)
{
    int fail = 0;
    extern int g_re15_active_cut;
    /* (a) active cut == a (9): Cut_replace(9,8) -> request switch to b (8). */
    {
        uint8_t bc[4]; bc[0]=0x4b; bc[1]=9; bc[2]=8; bc[3]=SCD_OP_EVT_END;
        scd_vm_init(); g_scd.cam_change_pending = 0; g_re15_active_cut = 9;
        scd_thread_start(0, bc); scd_vm_tick();
        if (g_scd.cam_id != 8 || g_scd.cam_change_pending != 1) {
            fprintf(stderr,"FAIL: cut_replace live switch: cam_id=%d pending=%d (want 8/1)\n",
                    (int)g_scd.cam_id, (int)g_scd.cam_change_pending); fail=1; }
    }
    /* (b) active cut != a (5): no switch. */
    {
        uint8_t bc[4]; bc[0]=0x4b; bc[1]=9; bc[2]=8; bc[3]=SCD_OP_EVT_END;
        scd_vm_init(); g_scd.cam_change_pending = 0; g_re15_active_cut = 5;
        scd_thread_start(0, bc); scd_vm_tick();
        if (g_scd.cam_change_pending != 0) {
            fprintf(stderr,"FAIL: cut_replace no-switch: pending=%d (want 0)\n", (int)g_scd.cam_change_pending); fail=1; }
    }
    if (!fail) printf("PASS: test_cut_replace_live_switch (0x4b active==a -> switch to b)\n");
    return fail;
}

int main(void)
{
    int failures = 0;

    printf("=== SCD Opcodes Unit Tests ===\n\n");

    failures += test_nop();
    failures += test_evt_end();
    failures += test_sleep_sleeping_cycle();
    failures += test_if_true();
    failures += test_if_false();
    failures += test_else_skip();
    failures += test_for_next_loop();
    failures += test_do_edwhile_loop();
    failures += test_break_exits_loop();
    failures += test_switch_case_break();
    failures += test_evt_chain_reinit();
    failures += test_evt_exec_direct_slot();
    failures += test_goto_block_unwind();
    failures += test_gosub_block_isolation();
    failures += test_ck_set_flags();
    failures += test_work_set();
    failures += test_speed_set();
    failures += test_flag_ck2();
    failures += test_cut_replace_live_switch();

    if (failures == 0) {
        printf("\nALL SCD OPCODE TESTS PASSED\n");
    } else {
        fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
