/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe Kleinkram: T-08 (Ada im Schutzraum 6020 nur lebend) und T-09 (Beschreibung der
 * Blue Master Keycard). Belege und Konstanten: include/re15_story_klein_r42.h. PORT-STORY.
 */
#include <stdint.h>
#include <stdio.h>

#include "re15_story_klein_r42.h"
#include "re15_scd.h"
#include "re15_engine.h"

#include "gen/story_klein_r42_texte.inc"

#define SKL_LOG(...) do { fprintf(stderr, "[story-klein] F%u ", (unsigned)g_engine.frame_count); fprintf(stderr, __VA_ARGS__); \
                          fputc('\n', stderr); } while (0)

unsigned g_re15_skl_zaehler[2];

int re15_story_klein_r42_spawn_weg(uint16_t room_id, uint8_t type)
{
    if (room_id != RE15_SKL_RAUM_SCHUTZRAUM || type != RE15_SKL_TYP_ADA) return 0;
    if (re15_game_flag_get(RE15_SKL_TOD_BANK, RE15_SKL_TOD_BIT)) {
        g_re15_skl_zaehler[0]++;
        SKL_LOG("ROOM6020: Ada (Typ 0x42, sub00 @0x00BAA) verworfen - tot in 50B0 ((3,0x62) = 1, ROOM50B0 sub02 @0x005A6), T-08");
        return 1;
    }
    g_re15_skl_zaehler[1]++;
    SKL_LOG("ROOM6020: Ada (Typ 0x42, sub00 @0x00BAA) erscheint - lebt ((3,0x62) = 0), T-08");
    return 0;
}

const uint8_t *re15_story_klein_r42_desc(uint8_t item_id)
{
    return item_id == RE15_SKL_ITEM_BLUE ? k_skl_blue_desc : NULL;   /* T-09: "... someone named "Michael."" */
}
