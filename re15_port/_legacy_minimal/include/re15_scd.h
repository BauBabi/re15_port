/**
 * @file re15_scd.h
 * @brief SCD-VM (Script Virtual Machine) Datenstrukturen
 *
 * Definiert die Strukturen für den SCD-Bytecode-Interpreter:
 * - 24 Thread-Slots (0 = Main, 1-9 = Player/AOT, 10-23 = Event-Slots)
 * - Opcode-Enum mit den RE1.5-relevanten Bytecodes
 * - Handler-Signatur für Opcode-Dispatch
 *
 * Tick-Rate: 30 Hz (jeder zweite VSync). Sleep-Dauern in 30-Hz-Ticks.
 *
 * Return-Codes der Opcode-Handler:
 *   0 = Thread beendet (pop call frame)
 *   1 = Continue (nächsten Opcode sofort dispatchen)
 *   2 = Yield (Thread pausiert bis nächster Tick)
 */
#ifndef RE15_SCD_H
#define RE15_SCD_H

#include "re15_types.h"

/* ============================================================================
 * Thread-Konfiguration
 * ========================================================================= */

/** Gesamtanzahl Thread-Slots (0=Main, 1-9=Player/AOT, 10-23=Events) */
#define SCD_THREAD_COUNT        24

/** Erster Event-Slot (Evt_exec Ziel-Pool) */
#define SCD_EVENT_SLOT_FIRST    10

/** Letzter Event-Slot */
#define SCD_EVENT_SLOT_LAST     23

/** Maximale Call-Stack-Tiefe (Gosub/Return) */
#define SCD_CALL_DEPTH_MAX      4

/** Maximale For-Schleifen-Verschachtelungstiefe */
#define SCD_FOR_DEPTH_MAX       4

/** Lokale Variablen pro Thread (Bytes) */
#define SCD_LOCALS_SIZE         128

/* ============================================================================
 * Opcode-Enum
 *
 * Mindestumfang gemäß Design-Dokument. Opcodes mit dokumentierter Byte-Länge.
 * ========================================================================= */

typedef enum {
    SCD_OP_NOP              = 0x00, /**< 1B  — Keine Aktion                  */
    SCD_OP_EVT_END          = 0x01, /**< 1B  — Thread beenden                */
    SCD_OP_EVT_EXEC         = 0x04, /**< 4B  — Sub-Thread starten            */
    SCD_OP_IFEL_CK          = 0x06, /**< 4B  — If-Block öffnen               */
    SCD_OP_ELSE_CK          = 0x07, /**< 4B  — Else-Block                    */
    SCD_OP_ENDIF            = 0x08, /**< 2B  — If-Block schließen             */
    SCD_OP_SLEEP            = 0x09, /**< 4B  — Timer initialisieren           */
    SCD_OP_SLEEPING         = 0x0A, /**< 2B  — Timer dekrementieren           */
    SCD_OP_FOR              = 0x0D, /**< 6B  — Schleife starten              */
    SCD_OP_NEXT             = 0x0E, /**< 2B  — Schleife iterieren            */
    SCD_OP_CK               = 0x21, /**< 4B  — Flag prüfen                   */
    SCD_OP_SET              = 0x22, /**< 4B  — Flag setzen                   */
    SCD_OP_CUT_CHG          = 0x29, /**< 4B  — Kamera wechseln              */
    SCD_OP_AOT_SET          = 0x2C, /**< 20B — Trigger-Zone setzen           */
    SCD_OP_OBJ_MODEL_SET    = 0x2D, /**< 22B — Objekt-Modell platzieren      */
    SCD_OP_WORK_SET         = 0x2E, /**< 3B  — Work-Entity wählen            */
    SCD_OP_SPEED_SET        = 0x2F, /**< 4B  — Geschwindigkeit setzen        */
    SCD_OP_ADD_SPEED        = 0x30, /**< 1B  — Geschwindigkeit anwenden      */
    SCD_OP_SE_ON            = 0x36, /**< 12B — SFX abspielen                 */
    SCD_OP_SCE_EM_SET       = 0x44, /**< 22B — Feind/NPC spawnen             */
    SCD_OP_ITEM_AOT_SET     = 0x4E, /**< 22B — Item-Zone setzen              */
    SCD_OP_DOOR_AOT_SET_4P  = 0x68, /**< 40B — Tür-Übergang (4-Punkt)        */
} scd_opcode_t;

/* ============================================================================
 * Thread-Struktur
 *
 * Pro Thread: PC-Pointer, Call-Stack, Sleep-Counter, lokale Variablen,
 * For-Loop-Stack, per-Thread Velocity.
 * ========================================================================= */

typedef struct scd_thread_s scd_thread_t;
typedef struct scd_vm_s     scd_vm_t;

/**
 * Opcode-Handler-Signatur.
 *
 * @param vm      Zeiger auf die VM-Instanz (globaler Zustand)
 * @param thread  Zeiger auf den ausführenden Thread
 * @return        0 = Thread beendet, 1 = continue, 2 = yield
 */
typedef int (*scd_opcode_handler_t)(scd_vm_t* vm, scd_thread_t* thread);

/**
 * Per-Thread-Zustand im SCD-VM.
 */
struct scd_thread_s {
    uint8_t         active;         /**< 0 = Slot frei, 1 = aktiv           */
    uint8_t         kill_pending;   /**< 1 = Thread stirbt bei nächstem Yield */
    int8_t          call_depth;     /**< Aktuelle Stack-Tiefe (0..3)         */
    uint8_t         _pad0;

    const uint8_t*  pc;             /**< Aktueller Bytecode-Programmzähler   */

    /** Call-Stack: Return-Adressen für Gosub/Return */
    const uint8_t*  call_stack[SCD_CALL_DEPTH_MAX];

    /** Sleep-Counter pro Call-Depth (30-Hz-Ticks) */
    uint16_t        sleep_counter[SCD_CALL_DEPTH_MAX];

    /** Lokale Variablen / Scratch-Speicher */
    uint8_t         locals[SCD_LOCALS_SIZE];

    /** For-Schleifen-Stack: Body-Start-PC pro Verschachtelungsebene */
    const uint8_t*  for_body[SCD_FOR_DEPTH_MAX];

    /** For-Schleifen-Stack: Verbleibende Iterationen */
    uint16_t        for_count[SCD_FOR_DEPTH_MAX];

    /** Aktuelle For-Schleifen-Tiefe (-1 = keine aktive Schleife) */
    int8_t          for_depth;

    /** Per-Thread Velocity (6 Achsen: vx, vy, vz, vrx, vry, vrz) */
    int16_t         vel[6];

    /** Per-Thread Work-Entity-Slot (-1 = nicht gesetzt) */
    int8_t          work_slot;

    /** Per-Thread Work-Prop-Index (-1 = nicht gesetzt) */
    int8_t          work_prop_idx;
};

/* ============================================================================
 * VM-Struktur (Globaler Zustand)
 * ========================================================================= */

/**
 * SCD Virtual Machine — globaler Interpreter-Zustand.
 */
struct scd_vm_s {
    /** Thread-Pool (24 Slots) */
    scd_thread_t    threads[SCD_THREAD_COUNT];

    /** Globaler Tick-Zähler (inkrementiert pro 30-Hz-Tick) */
    uint32_t        tick_count;

    /** Kamerawechsel-Request (gesetzt durch Cut_chg, cleared durch Renderer) */
    uint8_t         cam_change_pending;
    uint8_t         cam_id;             /**< Ziel-Kamera-Index          */

    /** Globaler Work-Entity-Slot (Fallback wenn per-Thread nicht gesetzt) */
    int8_t          work_slot;
    int8_t          work_prop_idx;

    /** Condition-Flag: Ergebnis der letzten Ck-Prüfung (0=false, 1=true) */
    uint8_t         condition_flag;

    /** Game-Flags: 256 individuelle Flags (32 Gruppen × 8 Bits) */
    uint8_t         game_flags[256];

    /** Opcode-Dispatch-Tabelle (256 Einträge, NULL = unbekannter Opcode) */
    scd_opcode_handler_t handlers[256];
};

/* ============================================================================
 * VM-API
 * ========================================================================= */

/** Globale VM-Instanz */
extern scd_vm_t g_scd;

/**
 * Initialisiert die SCD-VM (alle Threads inaktiv, Tick-Counter auf 0).
 */
void scd_vm_init(void);

/**
 * Führt einen 30-Hz-Tick aus (alle aktiven Threads dispatchen).
 */
void scd_vm_tick(void);

/**
 * Startet einen Thread in einem bestimmten Slot.
 *
 * @param slot  Thread-Slot-Index (0..SCD_THREAD_COUNT-1)
 * @param pc    Start-Bytecode-Adresse
 * @return      0 bei Erfolg, -1 wenn Slot ungültig oder belegt
 */
int scd_thread_start(int slot, const uint8_t* pc);

/**
 * Markiert einen Thread zum Beenden (wirksam beim nächsten Yield).
 *
 * @param slot  Thread-Slot-Index
 */
void scd_thread_kill(int slot);

/**
 * Registriert AOT- und Interaktions-Opcodes in der VM-Dispatch-Tabelle.
 *
 * Wird von scd_vm_init() aufgerufen. Implementiert in scd_opcodes_aot.c.
 *
 * @param vm  Zeiger auf die VM-Instanz
 */
void scd_register_aot_opcodes(scd_vm_t* vm);

#endif /* RE15_SCD_H */
