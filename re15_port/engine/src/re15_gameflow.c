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

/* KEIN Raum-Sprung-Parameter mehr. Es gab hier einen RE15_START_ROOM-Schnellweg, der direkt
 * INGAME in einen beliebigen Raum bootete. Der ist ENTFERNT (2026-08-01, auf Ansage des Nutzers),
 * weil er systematisch andere Ergebnisse lieferte als das, was im Spiel zu sehen ist:
 * er installierte den Raum AN re15_room_apply_pending VORBEI (eigene Boot-Sequenz in main.c),
 * waehrend jeder echte Raumwechsel — Tuer wie Debug-JUMP — durch apply_pending laeuft.
 * Damit fehlten dem Sprung genau die Schritte, die den Raum erst in den Spielzustand bringen
 * (Teardown, Motion-Reset, Bank-/BGM-Wechsel, Kamera-Cut, SCD-Reenter).
 * Raumwechsel gehen jetzt ausschliesslich ueber das ORIGINAL-Debug-Menue (UTILITY MENU,
 * PSX.EXE @0x80014444) bzw. ueber Tueren — beide muenden in re15_room_request_change(). */
void re15_gameflow_init(void)
{
    g_gameflow.mode         = RE15_MODE_TITLE;
    g_gameflow.start_room   = RE15_NEWGAME_ROOM;
    g_gameflow.enter_ingame = 0;
    g_gameflow.character    = 0;          /* Leon (PL00) */
    g_gameflow.boot_movie   = 0;          /* dormant, byte-true to the MZD build (see header) */
}

void re15_gameflow_new_game(int character)
{
    g_gameflow.character    = character;
    g_gameflow.start_room   = RE15_NEWGAME_ROOM;
    g_gameflow.enter_ingame = 1;         /* platform enters INGAME + loads the start room */
    g_gameflow.mode         = RE15_MODE_INGAME;
    {   /* Blut-Decal-Reset: der Wund-Builder FUN_80037c1c laeuft NUR im Spieler-Load-Pfad
         * (@0x800316c8/@0x800318cc, Teil von FUN_800314b0) und nullt Level+Akku — New Game
         * und CONTINUE/Load starten also blutfrei; Raumwechsel dagegen NICHT (BD-6,
         * analysis/blood_decals.md §5). CONTINUE laeuft durch denselben new_game-artigen
         * Einstieg der Plattform; ein separater Load-Pfad muss diesen Reset mitrufen. */
        extern void re15_wound_reset(void);
        re15_wound_reset();
        {   /* RE2-Kartensystem: neues Spiel = Karte leer (re15_map_visited.c) */
            extern void re15_map_visited_reset(void);
            re15_map_visited_reset();
        }
    }
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
