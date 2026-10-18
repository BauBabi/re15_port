/*
 * RE1.5 Rebuilt — Runde 39 Spur H "elza". Belege und Herleitung: include/re15_elza_r39.h,
 * Dossier analysis/befunde_runde39/H_elza.md.
 */
#include "re15_elza_r39.h"
#include "re15_collision.h"   /* re15_collision_band_from_y = -(y / 0x708), re15_collision.c:186 */

int re15_elza_r39_latch_hinfaellig(int state)
{
    /* Plc_motion @0x80041bb0 `sb v1(=4),4(v0)`: Kommandowort 4 -> Dispatcher-Tabelle @0x80073f90[4] = 0x80030660;
     * cmd 0 (@0x80031c10-c24) und Fall 0 (@0x80032074-b4) kommen nicht mehr dran. */
    return state == RE15_R39H_KOMMANDOWORT_PLC;
}

int re15_elza_r39_prop_band_ueber_boden(int32_t y)
{
    /* Original-Form fuer Null-Box-Requisiten: ROOM1091 obj 2 @0x02202 pc[4] = 2 auf Bodenhoehe -1800 (Spielerband 1),
     * ROOM1010 @0x00930 pc[4] = 1 auf Bodenhoehe 0 (Spielerband 0). Das Band-Gate der Klettersonde FUN_8002d2c0
     * @0x8002d364-70 (Objekt +0x82 == 0x800acad6) laesst ein solches Objekt damit nie als Kletterobjekt zu. */
    return re15_collision_band_from_y(y) + 1;
}
