/**
 * @file scd_opcodes_aot.c
 * @brief SCD-VM Opcodes: AOT- und Interaktions-Befehle
 *
 * Implementiert die SCD-Opcodes für Kamerawechsel, Trigger-Zonen,
 * Tür-Übergänge, Item-Zonen, Objektmodelle, Sound und Feind-Spawning:
 *
 *   0x29 Cut_chg          — Kamera wechseln (4B)
 *   0x2C Aot_set          — Trigger-Zone setzen (20B)
 *   0x2D Obj_model_set    — Objekt-Modell platzieren (22B, Stub)
 *   0x36 Se_on            — SFX abspielen (12B)
 *   0x44 Sce_em_set       — Feind/NPC spawnen (22B, Stub)
 *   0x4E Item_aot_set     — Item-Zone setzen (22B)
 *   0x68 Door_aot_set_4p  — Tür-Übergang 4-Punkt (40B)
 *
 * Diese Datei wird automatisch über den GLOB-Mechanismus in die
 * re15_engine-Library eingebunden. Die Registrierung der Handler
 * erfolgt über scd_register_aot_opcodes(), aufgerufen von scd_vm_init().
 */

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_audio.h"
#include "re15_error.h"

#include <string.h>

/* ============================================================================
 * Hilfsfunktionen: Little-Endian Byte-Zugriff
 * ========================================================================= */

static uint16_t aot_read_le16(const uint8_t* p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static int16_t aot_read_le16s(const uint8_t* p)
{
    return (int16_t)aot_read_le16(p);
}

/* ============================================================================
 * Cut_chg (0x29, 4 Bytes): Kamera wechseln
 *
 * Layout: [opcode(1)][unused(1)][cut_id_le16(2)]
 *
 * Setzt cam_change_pending = 1 und cam_id auf den neuen Kamera-Index.
 * Der Renderer wertet cam_change_pending im nächsten Frame aus.
 * ========================================================================= */

static int scd_op_cut_chg(scd_vm_t* vm, scd_thread_t* thread)
{
    uint16_t cut_id;

    cut_id = aot_read_le16(thread->pc + 2);

    vm->cam_change_pending = 1;
    vm->cam_id = (uint8_t)(cut_id & 0xFF);

    thread->pc += 4;
    return 1; /* continue */
}

/* ============================================================================
 * Aot_set (0x2C, 20 Bytes): Trigger-Zone setzen
 *
 * Layout: [opcode(1)][slot(1)][type(1)][floor(1)]
 *         [trigger_x[4] × s16 LE (8B)]
 *         [trigger_z[4] × s16 LE (8B)]
 *
 * Registriert eine generische Trigger-Zone im AOT-System.
 * ========================================================================= */

static int scd_op_aot_set(scd_vm_t* vm, scd_thread_t* thread)
{
    const uint8_t* p = thread->pc;
    re15_aot_slot_t slot;
    int slot_idx;
    int i;

    (void)vm;

    memset(&slot, 0, sizeof(re15_aot_slot_t));

    slot_idx        = p[1];
    slot.active     = 1;
    slot.type       = p[2];
    slot.floor_band = p[3];

    /* Trigger-Polygon: 4 X-Koordinaten (Offset 4..11) */
    for (i = 0; i < 4; i++) {
        slot.trigger_x[i] = aot_read_le16s(p + 4 + i * 2);
    }

    /* Trigger-Polygon: 4 Z-Koordinaten (Offset 12..19) */
    for (i = 0; i < 4; i++) {
        slot.trigger_z[i] = aot_read_le16s(p + 12 + i * 2);
    }

    re15_aot_set_slot(slot_idx, &slot);

    thread->pc += 20;
    return 1; /* continue */
}

/* ============================================================================
 * Obj_model_set (0x2D, 22 Bytes): Objekt-Modell platzieren (Stub)
 *
 * Für Phase 1: Nur PC vorrücken und loggen. Volles Modell-System kommt
 * in einer späteren Task.
 * ========================================================================= */

static int scd_op_obj_model_set(scd_vm_t* vm, scd_thread_t* thread)
{
    (void)vm;

    RE15_INFO("SCD", "Obj_model_set (0x2D): stub, PC += 22%s", "");

    thread->pc += 22;
    return 1; /* continue */
}

/* ============================================================================
 * Se_on (0x36, 12 Bytes): SFX abspielen
 *
 * Layout: [opcode(1)][unused(1)][bank(1)][sample_id(1)]
 *         [vol(1)][pan(1)][pad(6)]
 *
 * Ruft g_audio->sfx_play() auf, falls ein Audio-Backend gesetzt ist.
 * ========================================================================= */

static int scd_op_se_on(scd_vm_t* vm, scd_thread_t* thread)
{
    const uint8_t* p = thread->pc;
    uint8_t bank;
    uint8_t sample_id;
    uint8_t vol;
    uint8_t pan;

    (void)vm;

    bank      = p[2];
    sample_id = p[3];
    vol       = p[4];
    pan       = p[5];

    if (g_audio && g_audio->sfx_play) {
        g_audio->sfx_play((int)bank, (int)sample_id, (int)vol, (int)pan);
    } else {
        RE15_INFO("SCD", "Se_on: bank=%d id=%d vol=%d pan=%d (kein Audio-Backend)",
                  bank, sample_id, vol, pan);
    }

    thread->pc += 12;
    return 1; /* continue */
}

/* ============================================================================
 * Sce_em_set (0x44, 22 Bytes): Feind/NPC spawnen (Stub)
 *
 * Für Phase 1: Nur PC vorrücken und loggen. Vollständiges Enemy-System
 * kommt in einer späteren Task.
 * ========================================================================= */

static int scd_op_sce_em_set(scd_vm_t* vm, scd_thread_t* thread)
{
    (void)vm;

    RE15_INFO("SCD", "Sce_em_set (0x44): stub, PC += 22%s", "");

    thread->pc += 22;
    return 1; /* continue */
}

/* ============================================================================
 * Item_aot_set (0x4E, 22 Bytes): Item-Zone setzen
 *
 * Layout: [opcode(1)][slot(1)][type(1)][floor(1)]
 *         [trigger_x[4] × s16 LE (8B)]
 *         [trigger_z[4] × s16 LE (8B)]
 *         [item_id(1)][amount(1)]
 *
 * Registriert eine Item-Aufnahme-Zone im AOT-System.
 * ========================================================================= */

static int scd_op_item_aot_set(scd_vm_t* vm, scd_thread_t* thread)
{
    const uint8_t* p = thread->pc;
    re15_aot_slot_t slot;
    int slot_idx;
    int i;

    (void)vm;

    memset(&slot, 0, sizeof(re15_aot_slot_t));

    slot_idx        = p[1];
    slot.active     = 1;
    slot.type       = AOT_TYPE_ITEM;
    slot.floor_band = p[3];

    /* Trigger-Polygon: 4 X-Koordinaten (Offset 4..11) */
    for (i = 0; i < 4; i++) {
        slot.trigger_x[i] = aot_read_le16s(p + 4 + i * 2);
    }

    /* Trigger-Polygon: 4 Z-Koordinaten (Offset 12..19) */
    for (i = 0; i < 4; i++) {
        slot.trigger_z[i] = aot_read_le16s(p + 12 + i * 2);
    }

    /* Item-spezifische Daten */
    slot.data.item.item_id = p[20];
    slot.data.item._pad    = 0;
    slot.data.item.amount  = (int16_t)p[21];

    re15_aot_set_slot(slot_idx, &slot);

    thread->pc += 22;
    return 1; /* continue */
}

/* ============================================================================
 * Door_aot_set_4p (0x68, 40 Bytes): Tür-Übergang (4-Punkt-Polygon)
 *
 * Layout (40 Bytes total):
 *   [opcode(1)][aot_slot(1)][type(1)][floor(1)]         — 4 Bytes
 *   [trigger_x[4] × s16 LE (8B)]                        — 8 Bytes (Offset 4)
 *   [trigger_z[4] × s16 LE (8B)]                        — 8 Bytes (Offset 12)
 *   [dest_stage(1)][dest_room(1)][dest_cut(1)][pad(1)]   — 4 Bytes (Offset 20)
 *   [spawn_x(s16)][spawn_y(s16)][spawn_z(s16)][spawn_rot(s16)] — 8 Bytes (Offset 24)
 *   [reserved(8B)]                                       — 8 Bytes (Offset 32)
 *                                                        = 40 Bytes total
 *
 * Registriert eine Tür-Trigger-Zone im AOT-System mit Ziel-Raum-Info.
 * ========================================================================= */

static int scd_op_door_aot_set_4p(scd_vm_t* vm, scd_thread_t* thread)
{
    const uint8_t* p = thread->pc;
    re15_aot_slot_t slot;
    int slot_idx;
    int i;

    (void)vm;

    memset(&slot, 0, sizeof(re15_aot_slot_t));

    slot_idx        = p[1];
    slot.active     = 1;
    slot.type       = AOT_TYPE_DOOR;
    slot.floor_band = p[3];

    /* Trigger-Polygon: 4 X-Koordinaten (Offset 4..11) */
    for (i = 0; i < 4; i++) {
        slot.trigger_x[i] = aot_read_le16s(p + 4 + i * 2);
    }

    /* Trigger-Polygon: 4 Z-Koordinaten (Offset 12..19) */
    for (i = 0; i < 4; i++) {
        slot.trigger_z[i] = aot_read_le16s(p + 12 + i * 2);
    }

    /* Tür-spezifische Daten */
    slot.data.door.dest_stage = p[20];
    slot.data.door.dest_room  = p[21];
    slot.data.door.dest_cut   = p[22];
    /* p[23] = pad */

    slot.data.door.spawn_x   = aot_read_le16s(p + 24);
    slot.data.door.spawn_y   = aot_read_le16s(p + 26);
    slot.data.door.spawn_z   = aot_read_le16s(p + 28);
    slot.data.door.spawn_rot = aot_read_le16s(p + 30);

    re15_aot_set_slot(slot_idx, &slot);

    thread->pc += 40;
    return 1; /* continue */
}

/* ============================================================================
 * Registrierung: scd_register_aot_opcodes
 *
 * Wird von scd_vm_init() aufgerufen, um alle AOT/Interaktions-Handler
 * in die globale Dispatch-Tabelle einzutragen.
 * ========================================================================= */

void scd_register_aot_opcodes(scd_vm_t* vm)
{
    vm->handlers[SCD_OP_CUT_CHG]         = scd_op_cut_chg;
    vm->handlers[SCD_OP_AOT_SET]         = scd_op_aot_set;
    vm->handlers[SCD_OP_OBJ_MODEL_SET]   = scd_op_obj_model_set;
    vm->handlers[SCD_OP_SE_ON]           = scd_op_se_on;
    vm->handlers[SCD_OP_SCE_EM_SET]      = scd_op_sce_em_set;
    vm->handlers[SCD_OP_ITEM_AOT_SET]    = scd_op_item_aot_set;
    vm->handlers[SCD_OP_DOOR_AOT_SET_4P] = scd_op_door_aot_set_4p;
}
