/*
 * RE1.5 Rebuilt — Runde 38 Spur K "itemorte". Herleitung, Belege und alle Konstanten: include/re15_itemorte_r38.h,
 * re15_port/tools/r38_itemorte/itemorte_export.py und analysis/befunde_runde38/K_itemorte.md.
 * Die Prop-/Zonen-Anlage selbst bleibt in feuerloescher_r37.c / schluessel_r36.c (dort nur die Lage-Zeile und je ein
 * Haken); diese Datei liefert die gemessene Lage, das Freimachen des Slots und die Sichtbarkeitsregel.
 */
#include "re15_itemorte_r38.h"

#include <stddef.h>

#include "re15_aot.h"
#include "re15_schluessel_r36.h"   /* RE15_R36Q_PARK_ITEM / RE15_R36Q_PRISON_ITEM / RE15_R36Q_OBJ */
#include "re15_feuerloescher_r37.h" /* RE15_FL37_OBJ */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/itemorte_r38_md1.inc"

/* Den Satz aus `von` unveraendert nach `nach` tragen (alle parallelen Felder, wie sie der Scan liest) — dieselbe
 * Form wie kartenitems_r36.c satz_umziehen (Runde 36 N, Muenztelefon ROOM1030 Slot 8 -> 39). */
static void satz_umziehen(int von, int nach)
{
    g_aot.slots[nach]        = g_aot.slots[von];
    g_aot.door_params[nach]  = g_aot.door_params[von];
    g_aot.item_params[nach]  = g_aot.item_params[von];
    g_aot.flag_params[nach]  = g_aot.flag_params[von];
    g_aot.env_params[nach]   = g_aot.env_params[von];
    g_aot.stair_params[nach] = g_aot.stair_params[von];
    g_aot.slots[nach].was_inside = 0;
    g_aot.slots[von].active = 0;
    g_aot.slots[von].was_inside = 0;
}

int re15_itemorte_r38_platz_frei(uint16_t room_id, int slot)
{
    int nach = -1, alt_msg = -1;
    if (room_id == RE15_R38K_FL_RAUM && slot == RE15_R38K_FL_SLOT) {
        nach = RE15_R38K_FL_VERDRAENGT;    alt_msg = RE15_R38K_FL_SLOT_ALT_MSG;
    } else if ((room_id == 0x10F0 || room_id == 0x10F1) && slot == RE15_R38K_SCHL_SLOT) {
        nach = RE15_R38K_SCHL_VERDRAENGT;  alt_msg = RE15_R38K_SCHL_SLOT_ALT_MSG;
    }
    if (nach < 0 || slot < 0 || slot >= RE15_AOT_MAX) return 0;
    const re15_aot_t *a = &g_aot.slots[slot];
    /* nur den ORIGINAL-Satz dieser Stelle umziehen (sce 1 MESSAGE, event_id = msg-Index, re15_aot.h) */
    if (!a->active || a->type != RE15_AOT_TYPE_MESSAGE || a->event_id != (uint8_t)alt_msg) return 0;
    if (g_aot.slots[nach].active) return 0;           /* kein freier Ausweich-Slot: nichts anfassen */
    satz_umziehen(slot, nach);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[itemorte] ROOM%04X: Slot %d (msg %d) -> Slot %d, Slot %d frei fuer die Aufhebe-Zone\n",
            (unsigned)room_id, slot, alt_msg, nach, slot);
#endif
    return 1;
}

int re15_itemorte_r38_schluessel_lage(uint8_t item, int32_t pos[3], int16_t rot[3])
{
    if (item == RE15_R36Q_PARK_ITEM) {
        pos[0] = RE15_R38K_PARK_X; pos[1] = RE15_R38K_PARK_Y; pos[2] = RE15_R38K_PARK_Z;
        rot[0] = RE15_R38K_PARK_ROT_X; rot[1] = RE15_R38K_PARK_ROT_Y; rot[2] = RE15_R38K_PARK_ROT_Z;
        return 1;
    }
    if (item == RE15_R36Q_PRISON_ITEM) {
        pos[0] = RE15_R38K_PRISON_X; pos[1] = RE15_R38K_PRISON_Y; pos[2] = RE15_R38K_PRISON_Z;
        rot[0] = RE15_R38K_PRISON_ROT_X; rot[1] = RE15_R38K_PRISON_ROT_Y; rot[2] = RE15_R38K_PRISON_ROT_Z;
        return 1;
    }
    return 0;
}

const uint8_t *re15_itemorte_r38_schluessel_md1(uint8_t item, int *out_size)
{
    if (item == RE15_R36Q_PARK_ITEM)   { if (out_size) *out_size = (int)sizeof k_r38k_park_md1;   return k_r38k_park_md1; }
    if (item == RE15_R36Q_PRISON_ITEM) { if (out_size) *out_size = (int)sizeof k_r38k_prison_md1; return k_r38k_prison_md1; }
    if (out_size) *out_size = 0;
    return NULL;
}

int re15_itemorte_r38_schluessel_massstab(uint8_t item)
{
    if (item == RE15_R36Q_PARK_ITEM)   return RE15_R38K_PARK_S10000;
    if (item == RE15_R36Q_PRISON_ITEM) return RE15_R38K_PRISON_S10000;
    return 0;
}

/* PORT-ZUSATZ (Kopf re15_itemorte_r38.h, Dossier 2.4): Cuts, in denen das Anker-Viereck den Ursprung enthaelt, die
 * Stelle aber hinter Vordergrund liegt, den die Masken des Cuts nicht abdecken. */
int re15_itemorte_r38_verdeckt(uint16_t room_id, int cut, int obj_id)
{
    if (room_id == RE15_R38K_FL_RAUM && obj_id == RE15_FL37_OBJ)
        return cut == 3 || cut == 10;                 /* Pfeiler aus Kamera 3 (Cut 10 = Cut_replace 3 -> 10) */
    if ((room_id == 0x10F0 || room_id == 0x10F1) && obj_id == RE15_R36Q_OBJ)
        return cut == 7;                              /* Buecherregal aus Kamera 7, Cut 7 ohne Masken */
    return 0;
}
