#ifndef RE15_GAME_STEP_H
#define RE15_GAME_STEP_H

#include <stdint.h>
#include "re15_rdt.h"
#include "re15_emd.h"
#include "re15_camera.h"

/*
 * SHARED per-frame gameplay / interpreter step — IDENTICAL on PSX and PC.
 *
 * The two ports historically hand-wrote their main loops and DRIFTED (the
 * door-AOT scan, the same-room scenario re-entry consume, and the fired-event
 * dispatch ended up in a different order on PC than on PSX → stairs + the
 * ROOM1170 outdoor cutscene worked on PSX but not PC). This commons captures
 * the canonical (PSX) order so the interpreter behaves the same on both:
 *
 *   action-button latch
 *   → IF a stair traversal is active: stair_tick + keep RVD cam + AOT scan
 *     ELSE: player move + SCA/object collision + (stair_try_start || AOT scan)
 *   → same-room SCENARIO re-entry consume (g_scd_pending_scenario)
 *   → fired AOT event dispatch
 *
 * Only ARCHITECTURE-specific work stays in each port's main.c: rendering
 * (GTE/GPU vs SDL), the input SOURCE (pad vs SDL keys), audio output, asset
 * I/O (CD vs files), the SCD VM tick cadence, and the cross-room CD-load.
 * Each main loop fills this context from its own handles and calls
 * re15_game_step() at the same point every gameplay frame.
 */
typedef struct {
    const re15_rdt_t           *rdt;         /* current room (re15_test_rdt / rdt)    */
    int                         rdt_ok;      /* room loaded?                          */
    const re15_emd_skeleton_t  *pl00_skel;   /* player skeleton (stair FK pose)       */
    const re15_emd_animation_t *pl00_anim;   /* player animation set                  */
    const re15_emd_animation_t *w01_anim;    /* PL00W01 walk-source anim (footstep flags) */
    re15_camera_view_t         *cam_view;    /* active cut's view (player move basis)  */
    int                         active_cut;  /* active camera cut index (scan + RVD)   */
    uint16_t                    pad_current; /* held pad bits  (g_engine.pad_current) */
    uint16_t                    pad_pressed; /* edge pad bits  (g_engine.pad_pressed) */
} re15_game_ctx_t;

void re15_game_step(const re15_game_ctx_t *ctx);

/* SHARED per-frame helicopter-rotor spatialization driver. While the heli prop
 * (Obj_model_set obj_id 2) is on stage, attenuate + pan the rotor (BGM SUB layer)
 * by the active cut's camera→heli distance + azimuth (FUN_80045a64, applied in the
 * port's re15_audio_rotor_update). On/off stays with the SCD Sce_bgm_control — this
 * only tracks the playing layer. Both ports call this once per frame so the rotor
 * fades identically (the PSX always drove it; the PC had the backend but never
 * called it). `active_cut` = the current cut's camera record. */
void re15_rotor_drive(const re15_camera_cut_t *active_cut);

#endif /* RE15_GAME_STEP_H */
