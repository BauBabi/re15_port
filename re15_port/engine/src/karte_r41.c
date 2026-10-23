/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Punkt 2: Karte B1, Tuer-Zug auf die Zone der Tuer-Ankunft.
 * Herleitung, Messung und Belege: include/re15_karte_r41.h; Dossier analysis/befunde_runde41/E_kleinkram_b.md.
 * Rufer: re15_inv_screen.c tuer_anziehen (Haken "Runde 41 Spur E").
 */
#include "re15_karte_r41.h"

#include "re15_aot.h"
#include "re15_room.h"

int re15_karte_r41_tuer_zielzone(int aot_slot, unsigned ziel_room)
{
    if (aot_slot < 0 || aot_slot >= RE15_AOT_MAX) return -1;
    const re15_aot_t *a = &g_aot.slots[aot_slot];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return -1;
    /* Door_aot_set +14..+19 (next x/y/z) -> door_params (op_door_aot_set); dieselbe Lage, mit der der Raumaufbau
     * nach dem Durchgang die Zone setzt (scd_room_setup.c re15_map_zone_update). */
    const re15_map_zone_t *zs = re15_map_zone_at(ziel_room, g_aot.door_params[aot_slot].spawn_x,
                                                 g_aot.door_params[aot_slot].spawn_z);
    return zs ? (int)zs->zid : -1;
}
