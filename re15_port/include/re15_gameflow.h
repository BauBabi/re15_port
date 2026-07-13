/*
 * re15_gameflow.h — the shared top-level game-mode machine (FE-0.2 of
 * PORTING_ROADMAP.md). De-hardcodes the boot-into-a-room monolith: the engine
 * carries the mode + transitions; each platform main.c drives the per-mode
 * render/input. Byte-true source: the RE1.5 lifecycle-flag word DAT_800aca38
 * (main loop @0x8001cce0, INGAME = bit 0x40000000; attract handoff @0x80015838
 * clears it). We model those flags as a CLEAN enum (the observable behavior is
 * byte-true; the internal representation is the port's).
 */
#ifndef RE15_GAMEFLOW_H
#define RE15_GAMEFLOW_H

#include <stdint.h>

typedef enum {
    RE15_MODE_BOOT = 0,     /* cold init (SDL / PSX boot) -> TITLE (or INGAME debug fast-path) */
    RE15_MODE_TITLE,        /* DATA/TITLEU.TIM + PRESS START + NEW GAME/CONTINUE/OPTION menu     */
    RE15_MODE_CHARSELECT,   /* Leon / Elza pick (PL00 / PL01-04)                                 */
    RE15_MODE_FMV,          /* CAPCOM.STR opening movie (opcode 0x6F Movie_on)                   */
    RE15_MODE_INGAME,       /* the shared game step (game_step_common.c) — byte-true, unchanged  */
    RE15_MODE_PAUSE,        /* START-pause                                                       */
    RE15_MODE_INVENTORY,    /* inventory/status overlay                                          */
    RE15_MODE_GAMEOVER,     /* death presentation FSM (game_step_common.c re15_gameover_fsm_tick)*/
} re15_gameflow_mode_t;

typedef struct {
    re15_gameflow_mode_t mode;
    uint16_t start_room;    /* room entered on NEW GAME. Byte-true RE1.5 new game = the intro
                             * ROOM1240 (pre-intro montage -> ROOM1170 helipad -> gameplay). */
    int      enter_ingame;  /* one-shot request: the platform should (re)enter INGAME by loading
                             * `start_room` this frame, then clear it. Set by NEW GAME / CONTINUE. */
    int      character;     /* 0 = Leon (PL00), 1 = Elza (PL01) — the char-select result. */
    int      boot_movie;    /* the CAPCOM.STR opening-logo gate = DAT_800aca38 bit 0x8000, tested
                             * one-shot at the main-loop front (main.c:20 / 0x80020c28) and cleared
                             * on play (& ~0x8000 @0x80020c48). In the shipped RE1.5 MZD build this
                             * bit is DORMANT — NOTHING sets it (exhaustive PSX.EXE + STAGE1-6.BIN +
                             * DEBUG.BIN scan; 7 savestates read 0; SYSTEM.CNF BOOT=PSX.EXE = single-
                             * EXE boot, no shell) — so the logo never auto-plays; the disc boots
                             * straight to the front-end. Modelled faithfully: default 0 (dormant).
                             * The byte-true movie player runs iff this is armed. */
} re15_gameflow_t;

extern re15_gameflow_t g_gameflow;

/* Initialise the machine. debug_room >= 0 = the RE15_START_ROOM debug fast-path
 * (boot straight into that room, INGAME) so every parity/room-probe harness is
 * preserved; debug_room < 0 = normal boot to TITLE. */
void re15_gameflow_init(int debug_room);

static inline re15_gameflow_mode_t re15_gameflow_mode(void) { return g_gameflow.mode; }
static inline void re15_gameflow_set_mode(re15_gameflow_mode_t m) { g_gameflow.mode = m; }

/* Transitions (the byte-true edges of the RE1.5 flag machine). */
void re15_gameflow_new_game(int character);  /* TITLE -> (CHARSELECT ->) enter INGAME at start_room */
void re15_gameflow_to_gameover(void);        /* INGAME -> GAMEOVER (death FSM took over)            */
void re15_gameflow_to_title(void);           /* GAMEOVER/attract -> TITLE (clears the INGAME flag)  */

#endif /* RE15_GAMEFLOW_H */
