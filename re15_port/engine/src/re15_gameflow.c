/*
 * re15_gameflow.c — the shared top-level game-mode machine (FE-0.2).
 * See re15_gameflow.h. This carries the mode + the byte-true transitions; the
 * per-mode render/input lives in each platform main.c so PSX and PC share the
 * exact same flow. The RE1.5 original keyed everything off the lifecycle-flag
 * word DAT_800aca38 (INGAME = bit 0x40000000, set/cleared at @0x8001ca2c-cbf4 /
 * @0x80015838); we surface those edges as named transitions.
 */
#include "re15_gameflow.h"

re15_gameflow_t g_gameflow;

/* Byte-true NEW-GAME start room: the RE1.5 prototype boots the intro at ROOM1240
 * (pre-intro narrator montage -> ROOM1170 helipad cutscene -> handoff to play).
 * This is the current default boot room, so NEW GAME reproduces the real opening. */
#define RE15_NEWGAME_ROOM 0x1240

void re15_gameflow_init(int debug_room)
{
    if (debug_room >= 0) {
        /* RE15_START_ROOM debug fast-path: straight into the room (harness-preserving). */
        g_gameflow.mode         = RE15_MODE_INGAME;
        g_gameflow.start_room   = (uint16_t)debug_room;
        g_gameflow.enter_ingame = 1;     /* platform loads it this frame */
    } else {
        g_gameflow.mode         = RE15_MODE_TITLE;
        g_gameflow.start_room   = RE15_NEWGAME_ROOM;
        g_gameflow.enter_ingame = 0;
    }
    g_gameflow.character   = 0;           /* Leon (PL00) */
    g_gameflow.boot_movie  = 0;           /* dormant, byte-true to the MZD build (see header) */
}

void re15_gameflow_new_game(int character)
{
    g_gameflow.character    = character;
    g_gameflow.start_room   = RE15_NEWGAME_ROOM;
    g_gameflow.enter_ingame = 1;         /* platform enters INGAME + loads the start room */
    g_gameflow.mode         = RE15_MODE_INGAME;
}

void re15_gameflow_to_gameover(void)
{
    g_gameflow.mode = RE15_MODE_GAMEOVER;
}

void re15_gameflow_to_title(void)
{
    /* attract handoff (@0x80015838 clears the INGAME flag) -> back to the title. */
    g_gameflow.mode = RE15_MODE_TITLE;
}
