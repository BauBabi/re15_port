/**
 * @file scd_vm.c
 * @brief SCD-VM Kern — Dispatch-Loop, Thread-Management, Basis-Opcodes
 *
 * Implementiert den SCD-Bytecode-Interpreter mit 24 Thread-Slots,
 * einer 256-Einträge-Handler-Tabelle und den grundlegenden Opcodes:
 *
 *   0x00 Nop         — Keine Aktion (PC += 1, continue)
 *   0x01 Evt_end     — Thread beenden (return 0)
 *   0x04 Evt_exec    — Sub-Thread im Event-Pool starten
 *   0x09 Sleep       — Timer initialisieren (yield)
 *   0x0A Sleeping    — Timer dekrementieren (yield oder continue)
 *
 * Tick-Rate: 30 Hz. Return-Codes der Handler:
 *   0 = Thread beendet
 *   1 = Continue (nächsten Opcode sofort dispatchen)
 *   2 = Yield (Thread pausiert bis nächster Tick)
 */

#include "re15_scd.h"
#include "re15_error.h"

#include <string.h>

/* ============================================================================
 * Globale VM-Instanz
 * ========================================================================= */

scd_vm_t g_scd;

/* ============================================================================
 * Hilfsfunktionen: Little-Endian Byte-Zugriff
 * ========================================================================= */

static uint16_t read_le16(const uint8_t* p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

/* ============================================================================
 * Basis-Opcode-Handler
 * ========================================================================= */

/**
 * SCD_OP_NOP (0x00, 1 Byte): Keine Aktion, PC vorrücken.
 */
static int scd_op_nop(scd_vm_t* vm, scd_thread_t* thread)
{
    (void)vm;
    thread->pc += 1;
    return 1; /* continue */
}

/**
 * SCD_OP_EVT_END (0x01, 1 Byte): Thread deaktivieren.
 */
static int scd_op_evt_end(scd_vm_t* vm, scd_thread_t* thread)
{
    (void)vm;
    thread->active = 0;
    return 0; /* thread end */
}

/**
 * SCD_OP_EVT_EXEC (0x04, 4 Bytes): Sub-Thread starten.
 *
 * Layout: [opcode(1)] [target_slot(1)] [offset_lo(1)] [offset_hi(1)]
 *
 * Startet einen neuen Thread im angegebenen Slot. Der Offset wird relativ
 * zum aktuellen PC (Beginn des Opcodes) als Ziel-PC interpretiert.
 */
static int scd_op_evt_exec(scd_vm_t* vm, scd_thread_t* thread)
{
    uint8_t target_slot;
    uint16_t offset;
    const uint8_t* target_pc;

    (void)vm;

    target_slot = thread->pc[1];
    offset = read_le16(thread->pc + 2);

    /* Berechne Ziel-PC: Offset relativ zum Opcode-Beginn */
    target_pc = thread->pc + offset;

    /* Thread im Ziel-Slot starten */
    if (target_slot < SCD_THREAD_COUNT) {
        scd_thread_start((int)target_slot, target_pc);
    } else {
        RE15_WARN("SCD", "Evt_exec: Ungültiger Ziel-Slot %d", target_slot);
    }

    thread->pc += 4;
    return 1; /* continue */
}

/**
 * SCD_OP_SLEEP (0x09, 4 Bytes): Sleep-Counter initialisieren.
 *
 * Layout: [opcode(1)] [unused(1)] [count_lo(1)] [count_hi(1)]
 *
 * Setzt den Sleep-Counter auf den übergebenen Wert (30-Hz-Ticks).
 */
static int scd_op_sleep(scd_vm_t* vm, scd_thread_t* thread)
{
    uint16_t count;

    (void)vm;

    count = read_le16(thread->pc + 2);
    thread->sleep_counter[thread->call_depth] = count;

    thread->pc += 4;
    return 2; /* yield */
}

/**
 * SCD_OP_SLEEPING (0x0A, 2 Bytes): Sleep-Counter dekrementieren.
 *
 * Wenn Counter > 0: dekrementieren und yield.
 * Wenn Counter == 0: PC vorrücken und continue.
 */
static int scd_op_sleeping(scd_vm_t* vm, scd_thread_t* thread)
{
    (void)vm;

    if (thread->sleep_counter[thread->call_depth] > 0) {
        thread->sleep_counter[thread->call_depth]--;
        return 2; /* yield — noch am Schlafen */
    }

    /* Timer abgelaufen: weiter mit nächstem Opcode */
    thread->pc += 2;
    return 1; /* continue */
}

/* ============================================================================
 * Kontrollfluss-Opcode-Handler
 * ========================================================================= */

/**
 * SCD_OP_IFEL_CK (0x06, 4 Bytes): If-Block öffnen.
 *
 * Layout: [opcode(1)][unused(1)][else_offset_le16(2)]
 *
 * Prüft den condition_flag der VM.
 * Wenn FALSE (== 0): Springe zu PC + else_offset (zum Else-Block).
 * Wenn TRUE  (!= 0): PC += 4 (weiter im If-Body).
 */
static int scd_op_ifel_ck(scd_vm_t* vm, scd_thread_t* thread)
{
    uint16_t else_offset;

    if (vm->condition_flag) {
        /* Bedingung TRUE: weiter im If-Body */
        thread->pc += 4;
    } else {
        /* Bedingung FALSE: zum Else-Block springen */
        else_offset = read_le16(thread->pc + 2);
        thread->pc += else_offset;
    }

    return 1; /* continue */
}

/**
 * SCD_OP_ELSE_CK (0x07, 4 Bytes): Else-Block.
 *
 * Layout: [opcode(1)][unused(1)][endif_offset_le16(2)]
 *
 * Wird am Ende des If-True-Body erreicht: Springe zu PC + endif_offset
 * (überspringe den Else-Body).
 */
static int scd_op_else_ck(scd_vm_t* vm, scd_thread_t* thread)
{
    uint16_t endif_offset;

    (void)vm;

    endif_offset = read_le16(thread->pc + 2);
    thread->pc += endif_offset;

    return 1; /* continue */
}

/**
 * SCD_OP_ENDIF (0x08, 2 Bytes): If-Block schließen.
 *
 * Nur ein Marker — PC vorrücken und weiter.
 */
static int scd_op_endif(scd_vm_t* vm, scd_thread_t* thread)
{
    (void)vm;

    thread->pc += 2;
    return 1; /* continue */
}

/**
 * SCD_OP_FOR (0x0D, 6 Bytes): Schleife starten.
 *
 * Layout: [opcode(1)][unused(1)][count_le16(2)][body_offset_le16(2)]
 *
 * Pusht einen For-Loop-Frame: Speichert Iterationszähler und Body-Start-PC.
 * Body-Start = PC + 6 (direkt nach dem For-Opcode).
 */
static int scd_op_for(scd_vm_t* vm, scd_thread_t* thread)
{
    uint16_t count;

    (void)vm;

    count = read_le16(thread->pc + 2);
    /* body_offset bei Byte 4-5 wird hier nicht benötigt — Body beginnt nach dem Opcode */

    /* For-Tiefe erhöhen */
    thread->for_depth++;
    if (thread->for_depth >= SCD_FOR_DEPTH_MAX) {
        RE15_WARN("SCD", "For: Maximale Verschachtelungstiefe überschritten");
        thread->for_depth = SCD_FOR_DEPTH_MAX - 1;
    }

    /* Loop-Frame setzen */
    thread->for_count[thread->for_depth] = count;
    thread->for_body[thread->for_depth] = thread->pc + 6; /* Body-Start nach dem For-Opcode */

    thread->pc += 6;
    return 1; /* continue */
}

/**
 * SCD_OP_NEXT (0x0E, 2 Bytes): Schleife iterieren.
 *
 * Dekrementiert den for_count der aktuellen Schleife.
 * Wenn > 0: Springe zurück zum Body-Start.
 * Wenn == 0: PC += 2, pop for-depth.
 */
static int scd_op_next(scd_vm_t* vm, scd_thread_t* thread)
{
    (void)vm;

    if (thread->for_depth < 0) {
        RE15_WARN("SCD", "Next: Kein aktiver For-Loop");
        thread->pc += 2;
        return 1;
    }

    thread->for_count[thread->for_depth]--;

    if (thread->for_count[thread->for_depth] > 0) {
        /* Noch Iterationen übrig: zurück zum Body-Start */
        thread->pc = thread->for_body[thread->for_depth];
    } else {
        /* Schleife beendet: pop und weiter */
        thread->for_depth--;
        thread->pc += 2;
    }

    return 1; /* continue */
}

/**
 * SCD_OP_CK (0x21, 4 Bytes): Flag prüfen.
 *
 * Layout: [opcode(1)][flag_group(1)][flag_bit(1)][unused(1)]
 *
 * Prüft ob das Bit flag_bit in game_flags[flag_group] gesetzt ist.
 * Speichert das Ergebnis in vm->condition_flag.
 */
static int scd_op_ck(scd_vm_t* vm, scd_thread_t* thread)
{
    uint8_t flag_group;
    uint8_t flag_bit;

    flag_group = thread->pc[1];
    flag_bit   = thread->pc[2];

    /* Prüfe ob Bit gesetzt ist */
    if (flag_bit < 8) {
        vm->condition_flag = (vm->game_flags[flag_group] >> flag_bit) & 1;
    } else {
        vm->condition_flag = 0;
        RE15_WARN("SCD", "Ck: flag_bit %d außerhalb Bereich (0-7)", flag_bit);
    }

    thread->pc += 4;
    return 1; /* continue */
}

/**
 * SCD_OP_SET (0x22, 4 Bytes): Flag setzen.
 *
 * Layout: [opcode(1)][flag_group(1)][flag_bit(1)][value(1)]
 *
 * Setzt oder löscht das Bit flag_bit in game_flags[flag_group].
 * value != 0 → Bit setzen, value == 0 → Bit löschen.
 */
static int scd_op_set(scd_vm_t* vm, scd_thread_t* thread)
{
    uint8_t flag_group;
    uint8_t flag_bit;
    uint8_t value;

    flag_group = thread->pc[1];
    flag_bit   = thread->pc[2];
    value      = thread->pc[3];

    if (flag_bit < 8) {
        if (value) {
            vm->game_flags[flag_group] |= (uint8_t)(1u << flag_bit);
        } else {
            vm->game_flags[flag_group] &= (uint8_t)~(1u << flag_bit);
        }
    } else {
        RE15_WARN("SCD", "Set: flag_bit %d außerhalb Bereich (0-7)", flag_bit);
    }

    thread->pc += 4;
    return 1; /* continue */
}

/**
 * SCD_OP_WORK_SET (0x2E, 3 Bytes): Work-Entity wählen.
 *
 * Layout: [opcode(1)][slot(1)][prop(1)]
 *
 * Setzt den work_slot und work_prop_idx des Threads.
 */
static int scd_op_work_set(scd_vm_t* vm, scd_thread_t* thread)
{
    (void)vm;

    thread->work_slot     = (int8_t)thread->pc[1];
    thread->work_prop_idx = (int8_t)thread->pc[2];

    thread->pc += 3;
    return 1; /* continue */
}

/**
 * SCD_OP_SPEED_SET (0x2F, 4 Bytes): Geschwindigkeit setzen.
 *
 * Layout: [opcode(1)][axis(1)][speed_le16(2)]
 *
 * Setzt thread->vel[axis] = speed (signed 16-Bit).
 */
static int scd_op_speed_set(scd_vm_t* vm, scd_thread_t* thread)
{
    uint8_t axis;
    int16_t speed;

    (void)vm;

    axis  = thread->pc[1];
    speed = (int16_t)read_le16(thread->pc + 2);

    if (axis < 6) {
        thread->vel[axis] = speed;
    } else {
        RE15_WARN("SCD", "Speed_set: Ungültige Achse %d (max 5)", axis);
    }

    thread->pc += 4;
    return 1; /* continue */
}

/**
 * SCD_OP_ADD_SPEED (0x30, 1 Byte): Geschwindigkeit anwenden.
 *
 * Wendet die Velocity auf die Work-Entity an.
 * Stub-Implementierung: PC vorrücken, kein Effekt (Entity-System noch nicht vorhanden).
 */
static int scd_op_add_speed(scd_vm_t* vm, scd_thread_t* thread)
{
    (void)vm;

    /* TODO: Velocity auf Work-Entity anwenden sobald Entity-System vorhanden */
    thread->pc += 1;
    return 1; /* continue */
}

/* ============================================================================
 * Default-Handler für unbekannte Opcodes
 * ========================================================================= */

static int scd_op_unknown(scd_vm_t* vm, scd_thread_t* thread)
{
    uint8_t opcode = thread->pc[0];

    (void)vm;

    RE15_WARN("SCD", "Unbekannter Opcode: 0x%02X bei PC-Offset (Thread deaktiviert)", opcode);
    thread->active = 0;
    return 0; /* thread end — sicherer Abbruch */
}

/* ============================================================================
 * scd_vm_init — VM initialisieren
 * ========================================================================= */

void scd_vm_init(void)
{
    int i;

    /* Gesamte VM-Struktur nullen (alle Threads inaktiv, Tick = 0) */
    memset(&g_scd, 0, sizeof(scd_vm_t));

    /* Alle Handler auf den Unknown-Handler setzen */
    for (i = 0; i < 256; i++) {
        g_scd.handlers[i] = scd_op_unknown;
    }

    /* Basis-Opcodes registrieren */
    g_scd.handlers[SCD_OP_NOP]      = scd_op_nop;
    g_scd.handlers[SCD_OP_EVT_END]  = scd_op_evt_end;
    g_scd.handlers[SCD_OP_EVT_EXEC] = scd_op_evt_exec;
    g_scd.handlers[SCD_OP_SLEEP]    = scd_op_sleep;
    g_scd.handlers[SCD_OP_SLEEPING] = scd_op_sleeping;

    /* AOT- und Interaktions-Opcodes registrieren */
    scd_register_aot_opcodes(&g_scd);

    /* Kontrollfluss-Opcodes registrieren */
    g_scd.handlers[SCD_OP_IFEL_CK]  = scd_op_ifel_ck;
    g_scd.handlers[SCD_OP_ELSE_CK]  = scd_op_else_ck;
    g_scd.handlers[SCD_OP_ENDIF]    = scd_op_endif;
    g_scd.handlers[SCD_OP_FOR]      = scd_op_for;
    g_scd.handlers[SCD_OP_NEXT]     = scd_op_next;
    g_scd.handlers[SCD_OP_CK]      = scd_op_ck;
    g_scd.handlers[SCD_OP_SET]     = scd_op_set;
    g_scd.handlers[SCD_OP_WORK_SET]  = scd_op_work_set;
    g_scd.handlers[SCD_OP_SPEED_SET] = scd_op_speed_set;
    g_scd.handlers[SCD_OP_ADD_SPEED] = scd_op_add_speed;

    /* Work-Slots auf "nicht gesetzt" */
    g_scd.work_slot = -1;
    g_scd.work_prop_idx = -1;
}

/* ============================================================================
 * scd_vm_tick — Einen 30-Hz-Tick ausführen (alle aktiven Threads dispatchen)
 * ========================================================================= */

void scd_vm_tick(void)
{
    int slot;
    int rc;
    scd_thread_t* thread;
    scd_opcode_handler_t handler;

    g_scd.tick_count++;

    for (slot = 0; slot < SCD_THREAD_COUNT; slot++) {
        thread = &g_scd.threads[slot];

        if (!thread->active) {
            continue;
        }

        /* Kill-Pending prüfen: Thread beim nächsten Tick beenden */
        if (thread->kill_pending) {
            thread->active = 0;
            thread->kill_pending = 0;
            continue;
        }

        /* Dispatch-Loop: Opcodes ausführen bis Yield oder Thread-Ende */
        for (;;) {
            if (!thread->pc) {
                RE15_WARN("SCD", "Thread %d: NULL PC, deaktiviert", slot);
                thread->active = 0;
                break;
            }

            handler = g_scd.handlers[thread->pc[0]];
            rc = handler(&g_scd, thread);

            if (rc == 0) {
                /* Thread beendet */
                break;
            } else if (rc == 2) {
                /* Yield — Thread pausiert bis nächster Tick */
                break;
            }
            /* rc == 1: continue — nächsten Opcode sofort dispatchen */
        }
    }
}

/* ============================================================================
 * scd_thread_start — Thread in einem Slot aktivieren
 * ========================================================================= */

int scd_thread_start(int slot, const uint8_t* pc)
{
    scd_thread_t* thread;

    if (slot < 0 || slot >= SCD_THREAD_COUNT) {
        RE15_WARN("SCD", "thread_start: Ungültiger Slot %d", slot);
        return -1;
    }

    thread = &g_scd.threads[slot];

    /* Slot belegt und nicht kill_pending → Fehler */
    if (thread->active && !thread->kill_pending) {
        RE15_WARN("SCD", "thread_start: Slot %d bereits belegt", slot);
        return -1;
    }

    /* Thread-Zustand initialisieren */
    memset(thread, 0, sizeof(scd_thread_t));
    thread->active = 1;
    thread->pc = pc;
    thread->call_depth = 0;
    thread->for_depth = -1;
    thread->work_slot = -1;
    thread->work_prop_idx = -1;

    return 0;
}

/* ============================================================================
 * scd_thread_kill — Thread zum Beenden markieren
 * ========================================================================= */

void scd_thread_kill(int slot)
{
    if (slot < 0 || slot >= SCD_THREAD_COUNT) {
        RE15_WARN("SCD", "thread_kill: Ungültiger Slot %d", slot);
        return;
    }

    if (g_scd.threads[slot].active) {
        g_scd.threads[slot].kill_pending = 1;
    }
}
