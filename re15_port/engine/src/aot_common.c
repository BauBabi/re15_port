/**
 * @file aot_common.c
 * @brief AOT-Trigger-System — Slot-Verwaltung und Containment-Prüfung
 *
 * Implementiert die AOT-Slot-Verwaltung (32 Slots) und die Basis-API
 * für das Trigger-System. Die Opcodes in scd_opcodes_aot.c rufen
 * re15_aot_set_slot() auf, um Zonen zu registrieren.
 *
 * Die Frame-weise Prüfung (re15_aot_check) wird vom Spieler-Subsystem
 * pro Frame aufgerufen.
 */

#include "re15_aot.h"
#include "re15_error.h"

#include <string.h>

/* ============================================================================
 * Globaler AOT-Slot-Pool
 * ========================================================================= */

static re15_aot_slot_t g_aot_slots[RE15_AOT_MAX_SLOTS];

/* ============================================================================
 * re15_aot_init — Alle Slots deaktivieren
 * ========================================================================= */

void re15_aot_init(void)
{
    memset(g_aot_slots, 0, sizeof(g_aot_slots));
}

/* ============================================================================
 * re15_aot_set_slot — Slot mit Daten belegen
 * ========================================================================= */

int re15_aot_set_slot(int slot_idx, const re15_aot_slot_t* slot)
{
    if (slot_idx < 0 || slot_idx >= RE15_AOT_MAX_SLOTS) {
        RE15_WARN("AOT", "set_slot: Ungültiger Index %d", slot_idx);
        return -1;
    }

    if (!slot) {
        RE15_WARN("AOT", "set_slot: NULL-Pointer für Slot %d", slot_idx);
        return -1;
    }

    g_aot_slots[slot_idx] = *slot;
    return 0;
}

/* ============================================================================
 * re15_aot_clear_slot — Slot deaktivieren
 * ========================================================================= */

void re15_aot_clear_slot(int slot_idx)
{
    if (slot_idx < 0 || slot_idx >= RE15_AOT_MAX_SLOTS) {
        RE15_WARN("AOT", "clear_slot: Ungültiger Index %d", slot_idx);
        return;
    }

    g_aot_slots[slot_idx].active = 0;
}

/* ============================================================================
 * re15_aot_get_slot — Read-only Zugriff auf einen Slot
 * ========================================================================= */

const re15_aot_slot_t* re15_aot_get_slot(int slot_idx)
{
    if (slot_idx < 0 || slot_idx >= RE15_AOT_MAX_SLOTS) {
        return NULL;
    }
    return &g_aot_slots[slot_idx];
}

/* ============================================================================
 * Point-in-4P-Polygon Hilfsfunktion (Cross-Product-Test)
 *
 * Prüft ob Punkt (px, pz) innerhalb des konvexen 4-Punkt-Polygons liegt.
 * Verwendet den Kreuzprodukt-Test über alle 4 Kanten: Punkt muss auf
 * derselben Seite aller Kanten liegen (konsistentes Vorzeichen).
 * ========================================================================= */

static int point_in_4p(int16_t px, int16_t pz,
                       const int16_t* poly_x, const int16_t* poly_z)
{
    int i;
    int sign = 0;

    for (i = 0; i < 4; i++) {
        int next = (i + 1) & 3;
        int32_t ex = (int32_t)poly_x[next] - (int32_t)poly_x[i];
        int32_t ez = (int32_t)poly_z[next] - (int32_t)poly_z[i];
        int32_t dx = (int32_t)px - (int32_t)poly_x[i];
        int32_t dz = (int32_t)pz - (int32_t)poly_z[i];
        int32_t cross = ex * dz - ez * dx;

        if (cross == 0) {
            continue; /* Auf der Kante — zählt als "drin" */
        }

        if (sign == 0) {
            sign = (cross > 0) ? 1 : -1;
        } else {
            if ((cross > 0 && sign < 0) || (cross < 0 && sign > 0)) {
                return 0; /* Außerhalb */
            }
        }
    }

    return 1; /* Innerhalb */
}

/* ============================================================================
 * re15_aot_check — Alle aktiven Slots gegen Spielerposition prüfen
 * ========================================================================= */

int re15_aot_check(int16_t player_x, int16_t player_z, uint8_t floor_band)
{
    int i;
    re15_aot_slot_t* slot;

    for (i = 0; i < RE15_AOT_MAX_SLOTS; i++) {
        slot = &g_aot_slots[i];

        if (!slot->active) {
            continue;
        }

        /* Floor-Band muss übereinstimmen */
        if (slot->floor_band != floor_band) {
            continue;
        }

        /* Point-in-Polygon prüfen */
        if (!point_in_4p(player_x, player_z, slot->trigger_x, slot->trigger_z)) {
            continue;
        }

        /* Bereits betreten? (verhindert Re-Trigger nach Spawn) */
        if (slot->entered) {
            continue;
        }

        return i; /* Ausgelöster Slot-Index */
    }

    return -1; /* Kein Slot ausgelöst */
}

/* ============================================================================
 * re15_aot_mark_entered — Überlappende Zonen als "betreten" markieren
 *
 * Wird nach Raumwechsel/Spawn aufgerufen, damit die Tür-Zone am
 * Spawn-Punkt nicht sofort erneut auslöst.
 * ========================================================================= */

void re15_aot_mark_entered(int16_t player_x, int16_t player_z, uint8_t floor_band)
{
    int i;
    re15_aot_slot_t* slot;

    for (i = 0; i < RE15_AOT_MAX_SLOTS; i++) {
        slot = &g_aot_slots[i];

        if (!slot->active) {
            continue;
        }

        if (slot->floor_band != floor_band) {
            continue;
        }

        if (point_in_4p(player_x, player_z, slot->trigger_x, slot->trigger_z)) {
            slot->entered = 1;
        }
    }
}

/* ============================================================================
 * re15_aot_mark_entered_doors — Tür-AOT-Zonen als "betreten" markieren
 *
 * Wie re15_aot_mark_entered(), aber nur AOT-Slots vom Typ AOT_TYPE_DOOR.
 * Wird nach Raumwechsel/Same-Room-Teleport aufgerufen, damit keine
 * Tür-Zone am Spawn-Punkt sofort erneut auslöst.
 *
 * Validates: Requirements 4.8
 * ========================================================================= */

void re15_aot_mark_entered_doors(int16_t player_x, int16_t player_z,
                                 uint8_t floor_band)
{
    int i;
    re15_aot_slot_t* slot;

    for (i = 0; i < RE15_AOT_MAX_SLOTS; i++) {
        slot = &g_aot_slots[i];

        if (!slot->active) {
            continue;
        }

        /* Nur Tür-AOTs markieren */
        if (slot->type != AOT_TYPE_DOOR) {
            continue;
        }

        if (slot->floor_band != floor_band) {
            continue;
        }

        if (point_in_4p(player_x, player_z, slot->trigger_x, slot->trigger_z)) {
            slot->entered = 1;
        }
    }
}
