/*
 * zug_r42.c — Runde 42 Strang "finale", Stufe 2: Loch im Boden von Wagen B (ROOM509, Leon 5090 / Elza 5091) nach dem Zugstart.
 * Herleitung, Belege und offene Punkte: include/re15_zug_r42.h. Der Zugfilm und das WARNING im Fuehrerstand liegen in den
 * Zugstart-Programmen (annette_r41.c k_zug_seg, gen/story6_r42_szenen.inc k_s6_6031_zug) und in der Filmtabelle (zws_r41.c).
 */
#include "re15_zug_r42.h"
#include "re15_annette_r41.h"   /* RE15_AN_ZUG_BANK / RE15_AN_ZUG_BIT: Leons Zugstart (9,133) */
#include "re15_story_r42.h"     /* RE15_R42_BANK / RE15_R42_BIT_ZUG_ELZA: Elzas Zugstart (9,130) */
#include "re15_scd.h"
#include "re15_engine.h"

#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

unsigned g_re15_zug_r42_loch = 0;

/* Cut_replace(8,10) `4b 08 0a` (Form ROOM30E1 main00 @0x9F2 `4b 01 05`), dann sub03 unveraendert (5090 @0x012DA..@0x012F1 =
 * 5091 @0x012BE..@0x012D5, Riegel unit_r42_zug_loch vergleicht gegen beide RDTs). */
static const uint8_t k_loch[3 + RE15_ZUG_R42_SUB03_N] = {
    0x4b, RE15_ZUG_R42_CUT_HEIL, RE15_ZUG_R42_CUT_LOCH,
    0x22, 0x01, 0x1c, 0x01, 0x09, 0x0a, 0x08, 0x00, 0x22, 0x01, 0x1c, 0x00, 0x09, 0x0a, 0x14, 0x00,
    0x17, 0xff, 0xff, 0x00, 0xf0, 0xff, 0x01, 0x00,
};

const uint8_t *re15_zug_r42_programm(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_loch;
    return k_loch;
}

static int zug_gestartet(uint16_t room_id)
{
    if (room_id == RE15_ZUG_R42_RAUM_LEON) return re15_game_flag_get(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT) != 0;
    if (room_id == RE15_ZUG_R42_RAUM_ELZA) return re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_ZUG_ELZA) != 0;
    return 0;
}

const uint8_t *re15_zug_r42_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *target)
{
    if (sub_id != RE15_ZUG_R42_SUB_LICHT || !target) return target;
    if (room_id != RE15_ZUG_R42_RAUM_LEON && room_id != RE15_ZUG_R42_RAUM_ELZA) return target;
    if (!zug_gestartet(room_id)) return target;
    /* Nur, wenn der Sub wirklich die ausgelieferte Licht-Schleife ist (anderer Stand -> Original laufen lassen). */
    if (memcmp(target, k_loch + 3, RE15_ZUG_R42_SUB03_N) != 0) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[zug_r42] ROOM%04X sub03 weicht ab -> kein Loch (Original)\n", (unsigned)room_id);
#endif
        return target;
    }
    g_re15_zug_r42_loch++;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[zug_r42] F%u ROOM%04X Wagen B: Cut_replace(%d,%d) - Loch im Boden (Bild 10) nach dem Zugstart\n",
            (unsigned)g_engine.frame_count, (unsigned)room_id, RE15_ZUG_R42_CUT_HEIL, RE15_ZUG_R42_CUT_LOCH);
#endif
    return k_loch;
}
