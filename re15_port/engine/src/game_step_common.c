/*
 * game_step_common.c — the SHARED per-frame gameplay/interpreter step.
 * See re15_game_step.h. This file is compiled by BOTH the PSX-native and the
 * PC/SDL build so the two ports run the byte-identical interpreter order; the
 * only legitimate per-port divergence is architecture (render/input/audio/IO),
 * which stays in each main.c.
 */
#include "re15_game_step.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_player.h"
#include "re15_scd.h"
#include "re15_anim_select.h"   /* re15_actor_footstep (foot-plant query) */
#include "re15_audio.h"         /* re15_audio_footstep */
#include "re15_rdt.h"           /* re15_rdt_floor_sound */
#include "re15_enemy_ai.h"      /* re15_enemy_ai_run_all — the LIVE-zombie per-frame pass (8.6) */
#include "re15_damage.h"        /* re15_player_is_dead / re15_player_death_tick (8.10 death FSM) */

void re15_game_step(const re15_game_ctx_t *c)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* Action-button press edge (Square). Drives BOTH the stair trigger and the
     * door AOT scan below. */
    g_aot_action_pressed = (c->pad_pressed & RE15_PAD_BIT_SQUARE) ? 1 : 0;

    /* Expose the full per-frame press EDGE mask to the SCD VM so the YES/NO message
     * cursor (op_message_on) can read UP/DOWN to toggle the choice — the original
     * dialog FSM (FUN_80028134) reads the newly-pressed pad (DAT_800ac76c). */
    extern uint16_t g_scd_pad_edge;
    g_scd_pad_edge = c->pad_pressed;

    /* HELD action-button state (Square) for the dialog FSM's fast-forward: the original
     * (FUN_80028134 state 1) reads DAT_800ac768 (held pad) — holding the button makes the
     * typewriter timer fall 4× = speed up the writing. g_aot_action_pressed above is the
     * EDGE (advance/confirm); this is the LEVEL (held). */
    extern uint8_t g_scd_action_held;
    g_scd_action_held = (c->pad_current & RE15_PAD_BIT_SQUARE) ? 1 : 0;

    /* (Plc_neck head-look FSM is computed inside re15_skel_compute_pose at the head bone —
     * it needs the root bone matrix there to get the look angle in the correct frame. The
     * old re15_neck_update body-relative slew is retired to avoid double-slewing.) */

    if (c->rdt_ok && re15_stair_active()) {
        /* Engine-driven stair traversal (action-triggered): auto-walk Leon
         * up/down + force the stair clip + sink/raise Y. The player does NOT
         * steer — SKIP player_tick + collision (the 0x4000-latch behaviour) —
         * but the RVD camera scan KEEPS running (byte-true: the original's
         * per-frame cam scan is ungated by the stair) so the pit's RVD zone
         * flips the cut as Leon crosses it during the descent. */
        re15_stair_tick(c->rdt, c->pl00_skel, c->pl00_anim);
        g_scd.cut_auto_enabled = 1;
        re15_aot_scan(pl->x, pl->z, (uint8_t)c->active_cut);
    } else if (c->rdt_ok && re15_player_is_dead()) {
        /* PLAYER DEATH (Phase 8.10, byte-true core of the death FSM): HP < 0 -> the player is dead.
         * The original routes the player's per-frame command FSM to the death-sequence handler
         * (@0x80073f90[state]: [7] = the GRABBED death @0x8003694c, what the grab reaches; [3] = the
         * generic death @0x800366bc), which freezes input and runs a fade + death camera on a
         * 120-frame timer, then game over — it NEVER reads the pad. The port freezes the player
         * exactly like the stair/grab branches (skip re15_player_tick + collision) and advances the
         * byte-true death timer (re15_player_death_tick = 0x78 -> 0). The colour fade + death camera +
         * the eaten-anim FSM + the game-over screen are the DEFERRED presentation (no port fade/
         * game-over infra). This branch is unreachable until a hit drives HP < 0 (the grab, in
         * ROOM1140), so a healthy room is unaffected = no 1170 regression. Death takes precedence over
         * the grab: a zombie that killed the player then dead-grabs the corpse (the engage's hp<0
         * dead-grab arm) while the player runs the death sequence. Keep the RVD cam scan (death cam). */
        re15_player_death_tick();
        re15_aot_scan(pl->x, pl->z, (uint8_t)c->active_cut);
    } else if (c->rdt_ok && re15_player_is_grabbed()) {
        /* PLAYER-GRABBED LOCK (Phase 8.10, byte-true LAB_80036834): a live zombie has the player
         * latched (DAT_800aca58 = cmd 5). The original routes the player's per-frame command FSM to
         * the cmd-5 grabbed handler, which pins the player + plays the grabbed pose and NEVER reads
         * the pad — so the player cannot steer or walk away while held (he takes the repeated -5
         * bites). The port pins the player exactly as the stair traversal does (engine-driven, the
         * player does NOT steer): SKIP re15_player_tick + collision + the door-start scan. The player
         * holds his current XZ (the exact grab-pin pose/offset DAT_800acc0e = -floor*1800 is the
         * deferred anim layer). The grabbing zombie's hitbox + the bite damage are applied by
         * re15_enemy_ai_run_all at the END of this step (same slot as the lunge). The RVD camera cut
         * scan KEEPS running (byte-true: the per-frame cam scan is ungated by the player's state), so
         * the cut still frames the grab. This branch is unreachable unless a live zombie grabs, so a
         * room with no live zombie (ROOM1170/1240 boot) never enters it = no 1170 regression. */
        re15_aot_scan(pl->x, pl->z, (uint8_t)c->active_cut);
    } else {
        int32_t ox = pl->x, oz = pl->z;
        re15_player_tick(c->cam_view, c->pad_current);
        if (c->rdt_ok) {
            /* Room SCA collision then object collision: push the player out of
             * his band's perimeter wall cells, then out of solid Obj_model_set
             * props (helipad BOX). Walls first, then objects. */
            int32_t nx = pl->x, nz = pl->z;
            re15_collision_ensure_band(pl->y);
            re15_collision_constrain(c->rdt, ox, oz, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            pl->x = nx;
            pl->z = nz;
        }
        /* PLAYER WEAPON FIRE (Phase 8.10, two-sided combat — faithful-line input). Classic-RE
         * aim+fire: hold R1 to AIM, press Square to FIRE. While aiming, Square fires the equipped
         * weapon — re15_player_weapon_fire (the byte-true FUN_80011f50 core: auto-targets the nearest
         * zombie in FRONT within reach, applies the per-weapon byte-true damage -> the zombie HURT/
         * DEATH) — and does NOT trigger a door/stair (the original blocks actions while the weapon is
         * raised; here the R1 aim suppresses g_aot_action_pressed). The AIM SEQUENCE + FIRE RECOIL ARE
         * shown (8.14/8.16): re15_player_tick runs the byte-true action-8 aim sub-FSM @0x80035810 —
         * R1-held first plays the RAISE clip 17 (10 frames, PL00.EDD-verified) ONCE, then holds the
         * AIM-READY pose clip 18 (hold-last). Square fires ONLY once aim-ready (re15_player_aim_ready,
         * the byte-true state-5 gate — no shot mid-raise) and resets anim_frame = 0 (the discharge
         * @0x80035a00 re-plays clip 18 = recoil). FAITHFUL-LINE / DEFERRED: the exact 3-level command
         * FSM, the weapon INVENTORY UI/selection (DAT_800aca5d -> byte-true defaulted to the briefing
         * handgun = weapon 1 = ARMS01, re15_player_equipped_weapon; the equip UI is deferred), the
         * aim-elevation pitch (entity+0x66), and the muzzle-flash sprite. 1170-SAFE: fires only on
         * R1+Square (no input in a boot) and re15_player_weapon_fire only hits live zombies. */
        if (c->rdt_ok && (c->pad_current & RE15_PAD_BIT_R1)) {
            if ((c->pad_pressed & RE15_PAD_BIT_SQUARE) && re15_player_aim_ready()) {
                re15_player_weapon_fire(re15_player_equipped_weapon());  /* the equipped weapon (DAT_800aca5d;
                                               * byte-true briefing default = weapon 1 = the handgun, ARMS01,
                                               * savestate-confirmed — was hardcoded 2 = the wrong weapon/damage) */
                re15_audio_weapon_se(8);      /* GUNSHOT muzzle SE (byte-true FUN_80035538/FUN_80011f50 ->
                                               * FUN_80045024(0x1080001) = bank1 idx 8). The bank = the equipped
                                               * weapon's ARMS (primed weapon 1 = ARMS01, the briefing handgun,
                                               * savestate-confirmed; idx 8 -> VAG 4). See RE15_COMBAT_SE_SUBSYSTEM.md §3. */
                pl->anim_frame = 0;           /* FIRE recoil: restart the aim/fire clip 18 (action-8 phase5
                                               * @0x80035a00 re-plays clip 18 on discharge). The aim POSE
                                               * itself (clip 18, held) is set by re15_player_tick while R1
                                               * is held; this restart is the visible recoil on the shot. */
            }
            g_aot_action_pressed = 0;         /* aiming blocks the door/stair action (no doors while aiming) */
        }
        /* A stair may START this frame: ACTION pressed while in/against a stair
         * zone. If so it consumes the action and we SKIP the door scan;
         * otherwise scan the door AOTs (also action-gated). */
        if (!(c->rdt_ok && re15_stair_try_start(c->rdt, g_aot_action_pressed))) {
            re15_aot_scan(pl->x, pl->z, (uint8_t)c->active_cut);
        }
    }

    /* FOOTSTEP SE (byte-true LAB_80030af0 walk / LAB_80030d28 run): while the player
     * walks(105)/runs(100), the W01 clip's CURRENT frame carries a foot-plant flag
     * (bit 0x4000) → fire one footstep, its material from floor.flr
     * (re15_rdt_floor_sound at the player XZ). Edge-gate on the anim frame so the
     * same foot-plant isn't double-fired (PC halves anim_frame at 60fps). */
    {
        static uint32_t s_last_foot_cur = 0xFFFFFFFFu;
        if (c->rdt_ok && c->w01_anim && (pl->motion == 105 || pl->motion == 100)) {
            uint32_t cur = (uint32_t)pl->anim_frame;
            if (cur != s_last_foot_cur) {
                s_last_foot_cur = cur;
                int clip = (pl->motion == 105) ? 5 : 0;   /* walk=W01 c5, run=c0 */
                int foot = re15_actor_footstep(c->w01_anim, pl, clip, cur);
                if (foot) re15_audio_footstep(foot, re15_rdt_floor_sound(c->rdt, pl->x, pl->z));
            }
        } else {
            s_last_foot_cur = 0xFFFFFFFFu;   /* reset when not locomoting */
        }
    }

    /* Same-room SCENARIO re-entry: a SELF-room door (dest == current room) queued
     * g_scd_pending_scenario = its target_cut during the scan above. Consume it
     * HERE — same frame, immediately after the scan (the canonical PSX order; PC
     * used to consume it a frame early at the top of the loop, which drifted) — to
     * re-run main00+sub00 with locals[10]=scenario → sub00's sub15 dispatch:
     *   door 0 (cut 11) → sub14 (outdoor courtyard dialog cutscene);
     *   door 6 (cut 0, courtyard→helipad return) → sub15 else = the 7 crows
     *     (the re-entry memset already despawned Elliot + heli).
     * No asset reload (same room); game flags persist; Leon is at the door spawn. */
    if (g_scd_pending_scenario >= 0 && c->rdt_ok) {
        uint8_t sc = (uint8_t)g_scd_pending_scenario;
        /* The door fire queued a camera change (g_scd.cam_id = target_cut,
         * cam_change_pending = 1) during the scan — but scd_room_reenter's
         * memset(&g_scd) is about to WIPE it. Capture the door's entry cut now. */
        uint8_t entry_cut = g_scd.cam_id;
        g_scd_pending_scenario = -1;
        scd_room_reenter(c->rdt, pl->x, pl->z, sc);
        /* Door entry = GAMEPLAY → enable the RVD/CAM_SWITCH auto-camera so it FOLLOWS
         * the player between the section's camera cuts. scd_room_reenter's memset
         * cleared cut_auto_enabled, and a gameplay scenario (e.g. door 6 → sub15 crows)
         * never issues Cut_auto(1) — so without this the camera stays frozen on the
         * entry cut and "man läuft außerhalb des Kamera Views". The cross-room door
         * (re15_room_apply_pending) already does this; self-doors must too. Camera-debug
         * autopilot proved cut_auto=0 → no follow, cut_auto=1 → follows (11→10).
         * A cutscene scenario (sub14) re-issues its own Cut_auto/Cut_chg, so this is
         * harmless there (RVD is byte-true ALSO live during the cinematic). */
        g_scd.cut_auto_enabled = 1;
        /* Re-apply the door's entry cut so the camera FRAMES the teleported player
         * (the port's camera block applies this next frame: build view + load BG).
         * Only if the scenario's own SCD didn't already queue a Cut_chg this tick —
         * then respect that. Without this, a self-door whose scenario issues NO
         * Cut_chg (door 6 → sub15 ELSE = the crows) leaves the camera on the OLD
         * courtyard cut while the player teleports to the helipad → player drawn
         * off-screen ("Leon verschwindet"). The scenario's later Cut_chg (e.g. door
         * 0 → sub14 → Cut_chg 12) still overrides on its own tick. */
        if (!g_scd.cam_change_pending) {
            g_scd.cam_id             = entry_cut;
            g_scd.cam_change_pending = 1;
        }
    }

    /* Dispatch any AOT event fired this frame to its SCD handler (the handler
     * may set cam_change_pending (Cut_chg), enqueue audio (Se_on), or arm a
     * subtitle (Message_on) — each consumed downstream by the port). */
    if (g_aot.fired_event_id_this_frame != 0) {
        scd_event_fire(g_aot.fired_event_id_this_frame);
    }

    /* ===== Phase 8.6 — the LIVE STAGE1 zombie AI pass ==================================
     * Byte-true to the original per-frame entity-update loop FUN_8001a50c (@0x8001ce04 in the
     * main update): it walks the entity array and dispatches @0x80072bac[type] per active entity.
     * re15_enemy_ai_run_all is the port's TYPE-GATED slice of that loop — it ticks ONLY the live
     * zombie types (0x10/0x11/0x16) through their handler (FUN_80100424 tick + the shared lunge
     * slice); every other type (Elliot 0x47, crows 0x21, the player, props) keeps its existing
     * path. That gate is what makes this 1170-SAFE: the ROOM1170 helipad + the ROOM1240 boot room
     * spawn no live zombie, so this is a verified no-op there (no intro/cinematic regression).
     *
     * Placement: at the END of the step, AFTER the player move/collision + AOT scan have settled
     * the frame. The original runs the AI-tick half (FUN_8001a50c) just BEFORE the AOT scan and
     * the lunge-EXECUTION half (the action driver FUN_80019e20 @0x8001ce2c) just AFTER it; the
     * port folds tick+execution into run_all, and placing it here lands the lunge hitbox after the
     * AOT scan (faithful to FUN_80019e20's slot) against the player's final XZ this frame.
     *
     * combat_active = g_scd.combat_active = the byte-true DAT_800aca3c & 1 latch the attack-arm
     * (FUN_8010ab2c) gates on; held, cleared on room load (see re15_scd.h). The briefing zombies
     * spawn in the feeding/lying sub-modes (grid_id & 0xf != 0), so the combat decision brain is
     * not even entered yet — they tick (INIT->ACTIVE) but do not attack until the deferred
     * feeding->combat wake-up handler runs. Verified in a real room by test_room1140_combat. */
    if (c->rdt_ok)
        re15_enemy_ai_run_all(g_scd.combat_active);
}

/* SHARED helicopter-rotor spatialization driver — see re15_game_step.h. Was inline
 * in the PSX main loop only; the PC had re15_audio_rotor_update implemented but never
 * called it (rotor never faded). Now both ports call this so the rotor tracks the
 * camera→heli distance + azimuth identically. */
void re15_rotor_drive(const re15_camera_cut_t *active_cut)
{
    if (!active_cut) return;
    for (int pi = 0; pi < (int)g_scd.prop_count; pi++) {
        if (!g_scd.props[pi].active || g_scd.props[pi].obj_id != 2) continue;
        int32_t eye[3] = { active_cut->pos_x, active_cut->pos_y, active_cut->pos_z };
        int32_t tgt[3] = { active_cut->target_x, active_cut->target_y, active_cut->target_z };
        int32_t hp[3]  = { g_scd.props[pi].x, g_scd.props[pi].y, g_scd.props[pi].z };
        re15_audio_rotor_update(eye, tgt, hp);
        return;
    }
}
