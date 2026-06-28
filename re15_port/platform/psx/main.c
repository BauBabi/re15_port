/*
 * RE1.5 Rebuilt — Main entry point (Phase 4.1, 2026-05-18).
 *
 * GOAL: a complete new Resident Evil engine in C built via PSn00bSDK,
 * architected after RE2's stable code, running RE1.5's assets.
 *
 * PHASE 4.1 — Skeleton boot. This EXE does:
 *   - Initialize GPU (320x240 double-buffered)
 *   - Initialize controller pad
 *   - Main loop with frame counter + pad state readout
 *   - Show splash text proving the engine boots
 *
 * Subsequent phases add asset loading, SCD VM, rendering, audio, etc.
 * The goal is NOT to be ready to play — it's to prove the foundation
 * and incrementally bring up subsystems with confidence at each step.
 */

#include <stdio.h>
#include <psxapi.h>          /* GetRCnt — perf sub-phase timers */
#include "re15_engine.h"
#include "re15_scd.h"
#include "re15_md1.h"
#include "re15_bg.h"
#include "re15_room.h"   /* multi-room: re15_room_load, g_room_change, g_current_room_id */
#include "re15_collision.h"  /* player vs room SCA collision */
#include "re15_stair.h"      /* stair traversal (action-triggered band transition) */
#include "re15_game_step.h"  /* SHARED per-frame interpreter step (PSX+PC) */
#include "re15_audio.h"
#include "re15_emd.h"
#include "re15_camera.h"
#include "re15_pri.h"   /* sprite.pri foreground-occlusion masks + emit */
extern int  re15_pri_psx_load_cut(int cut_idx);          /* pri_psx.c  */
extern void re15_render_pri_sprites(const re15_pri_cut_t *pri); /* render.c */
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_inventory.h"
#include "re15_msg.h"       /* intro subtitles: runtime .msg text */
#include "re15_cdfs.h"      /* Phase 4.7: CD asset streaming */
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_anim_select.h"  /* SHARED actor bank/clip selection view-model */
#include "re15_enemy.h"        /* generic enemy-model registry (re15_enemy_find) */
extern int re15_enemy_load(uint8_t type);   /* asset_psx.c — PSX Step B lazy EMD loader */
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_light.h"      /* #1c: lighting diagnostic */

extern re15_md1_t           re15_test_md1;
extern int                  re15_test_md1_ok;
extern re15_emd_animation_t re15_test_anim;
extern re15_emd_skeleton_t  re15_test_skel;
extern int                  re15_test_skel_ok;
/* PL00W01 walk-source (composite: PL00 bind + W01 keyframes). Selected for
 * any actor in walk/run motion (sentinel 105 → clip 5, 100 → clip 0). */
extern re15_emd_animation_t re15_w01_anim;
extern re15_emd_skeleton_t  re15_w01_skel;
/* PL00 base track: idle (clip 0) + back-walk (clip 1), sentinels 200/201. */
extern re15_emd_animation_t re15_pl00_anim;
extern re15_emd_skeleton_t  re15_pl00_skel;
extern int                  re15_pl00_ok;
extern int                  re15_w01_ok;
extern re15_rdt_t           re15_test_rdt;
extern int                  re15_test_rdt_ok;
extern void                 re15_assets_reset_vram_allocators(void);  /* asset_psx.c — room-enter VRAM rewind */
extern void                 re15_load_room_props(unsigned room_id);   /* asset_psx.c — data-driven per-room prop set */
extern void                 re15_load_room_cinematic(unsigned room_id); /* asset_psx.c — data-driven per-room rbj overlay + special NPC */
extern re15_md1_t           re15_heli_md1;       /* #2: helicopter body */
extern int                  re15_heli_md1_ok;
extern int                  re15_heli_tim_tpage; /* #2: heli's own VRAM texture */
extern int                  re15_heli_tim_clut;
extern int                  re15_heli_tim_ok;
extern re15_md1_t           re15_obj_md1[6];     /* #2: prop models by obj_id */
extern int                  re15_obj_tpage[6];
extern int                  re15_obj_clut[6];
extern int                  re15_obj_ok[6];
extern re15_md1_t           re15_elliot_md1;     /* Elliot (NPC type 0x47) */
extern re15_emd_skeleton_t  re15_elliot_skel;
extern re15_emd_animation_t re15_elliot_anim;
extern int                  re15_elliot_tpage;
extern int                  re15_elliot_clut;
extern int                  re15_elliot_clut_yshift;
extern int                  re15_elliot_tpage_xshift;
extern int                  re15_elliot_ok;
/* (em21 globals removed 2026-06-14f — type 0x21 crows now load via the generic enemy
 * arena; the selector reads them from re15_enemy_find(0x21), not banks->em21_*.) */

/* compute_actor_kf + the actor bank/clip selection now live in the SHARED
 * anim_select_common.c (re15_anim_select.h) so they drive both renderers. */

re15_engine_state_t g_engine;

/* PSX cross-room render reset (re15_room_apply_pending ctx callback): rewind the
 * prop-VRAM bump allocator + invalidate the BG cache so the new room re-uploads. */
static void psx_room_reset_render(void)
{
    re15_assets_reset_vram_allocators();
    re15_bg_invalidate();
}

int main(int argc, const char **argv)
{
    re15_render_init();
    re15_input_init();
    re15_cdfs_init();     /* Phase 4.7: CD subsystem (CdInit) for asset streaming */
    re15_assets_init();   /* Phase 4.3: load + upload test TIM to VRAM */
    mesh_psx_init();      /* Phase 4.5.2: initialize GTE for 3D rendering */

    /* Phase 4.5.6.3: BG subsystem. DecDCTReset must run AFTER InitGeom
     * (mesh_psx_init) because DecDCTvlc internally uses the GTE.
     * Then decode the bundled test BSS frame once into VRAM cache. */
    re15_bg_init();
    re15_bg_load_test_asset();

    /* Phase 4.6.1: SPU init + queue consumer. After mesh/BG so SPU DMA
     * doesn't contend with the boot-time MDEC + LoadImage flurry. */
    re15_audio_init();
    /* BGM (RE2 SsSeq on SPU) is DEFERRED to the pre-intro→helipad handoff in the
     * main loop — the original plays NO music under the narrator prologue, only
     * once the helipad cinematic (sub00→sub02) begins. See the deferred spawn. */

    /* Phase 4.4: SCD VM init + start demo thread */
    scd_vm_init();
    scd_register_room_events(re15_test_rdt_ok ? &re15_test_rdt : NULL);

    /* Phase 4.5.13-D: ROOM1170 authentic cinematic chain wire-up (PSX).
     *
     * 2026-05-21: re-enabled after 8-agent forensic identified all three
     * PSX-specific root causes of the previous "cycling wrong anims then
     * blackscreen" failure:
     *   A1: rbj never overlaid → fixed in asset_psx.c (incbin + overlay)
     *   A2: MDEC Cut_chg storm wedges DecDCToutSync → fixed in bg_psx.c
     *       (DrawSync+Reset before DecDCTin, bounded poll, skip redundant
     *       reloads of the only-bundled test.bss)
     *   A6: re15_debug_text FntSort overflow corrupts next buffer's OT
     *       → fixed in render.c (bounds + z clamp)
     *
     * The chain (from disassembled ROOM1170 scd/*.c):
     *   main00         registers doors + branches on flags (3,193)+(3,125)
     *   sub00 case 0   goto sub15           (locals[10]==0 at boot ✓)
     *   sub15 Ck(4,242,1)  Evt_exec(0x1802) → sub02 ✓ cinematic
     *
     * DATA-DRIVEN progression (2026-06-03 keystone audit fix): we set ONLY the
     * single flag (3,193) that sub03 would have set on the (not-yet-implemented
     * multi-room) PRIOR visit. With (3,193)=1 AND (3,125)=0, room1170 main00
     * itself fires Evt_exec(0x180B) → sub11 (the narrator prologue) through the
     * faithful op_evt_exec path — we no longer force (3,125)/(4,242) nor manually
     * start the narrator. sub11 sets (3,125)/(4,242)/(2,7) and runs Cut_chg(7)+the
     * 4 narrator Message_on lines from its OWN bytecode; sub00→sub15 then spawns
     * the helipad cinematic once (4,242) is set. (3,193) is the one honest shortcut
     * pending the multi-room door-self-reentry — see multiroom_remaining_steps_TODO. */
    re15_game_flag_set(3, 193, 1);   /* = sub03 ran on the prior (multi-room) visit */

    /* Außenbereich door hub (2026-06-06): ROOM1170 main00 gates the WHOLE outdoor
     * door set behind `else` of `if(Ck(4,195,0))` — door 6 (the return from the
     * outdoor area back to the helipad) AND door 5 (→ ROOM1140) + the examine AOTs
     * only register when (4,195)==1. Door 0 (helipad→outdoor) is always on, so with
     * (4,195)==0 you reach the outdoor area but it's a DEAD END (no return, no 1140
     * door). The original sets (4,195) in ROOM1140/sub02 — i.e. once the player has
     * been outside; we stage it here (same data-driven pattern as (3,193)) so the
     * unmodified main00 registers the return door 6 + the on-foot door 5→1140.
     * ROOM1140's own return door (door1→1170) is registered in BOTH branches, so
     * this doesn't change the way back from 1140. */
    re15_game_flag_set(4, 195, 1);

    /* Spawn in cut 0's helipad position (matches PC main.c, AM-round).
     *
     * Y = -7200 = the HELIPAD FLOOR (cinematic surface), per sub15's NPC
     * spawns — Elliot, the pilot and the heli are all placed at Y=-7200
     * (Obj_model_set), so that IS the ground plane Leon stands on. The
     * earlier -7965 was misappropriated from sub14's railing-area Pos_set
     * (the post-cinematic monologue spot); using it here floated Leon ~765
     * units ABOVE the NPC ground ("Leon fliegt in der Luft"). Corrected on
     * PC by a 30-agent RE; the old PSX value predated that fix. */
    g_actors[RE15_ACTOR_SLOT_PLAYER].x      = 1272;
    g_actors[RE15_ACTOR_SLOT_PLAYER].y      = -7200;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z      = 10898;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y  = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp     = 100;   /* RE1.5 max HP (DAT_800acaee
        init 0x64). Drives the HP-gated injured idle (clip22 <50 / clip23 <30);
        full HP now -> healthy idle, drops to injured idle once combat lands. */

    /* Collision floor band from the spawn Y (FUN_8003b0a4 push-out of band-matching
     * cells; ROOM1170 spawn Y=-7200 -> band 4). RESTORED 2026-06-07 after a 135-agent
     * RE + raw-byte check: the player walks in the band-4-FREE complement, band-4
     * cells are the walls (USER-CONFIRMED-good helipad). The band stays fixed; doors
     * re-set it from their spawn Y. */
    re15_collision_set_band(re15_collision_band_from_y(g_actors[RE15_ACTOR_SLOT_PLAYER].y));

    /* Phase 4.5.9-C: prefer the real RDT main SCD script over the demo
     * fallback. Unknown opcodes now skip safely via the size table
     * (scd_vm.c op_unknown) so the VM flows through unfamiliar bytecode
     * without killing the thread. */
    const uint8_t *main_pc = (re15_test_rdt_ok && re15_test_rdt.main_scd)
        ? re15_test_rdt.main_scd
        : scd_fallback_bytecode();
    scd_thread_start(0, main_pc);

    /* sub00 (the room MAIN/exec → sub15 → sub02 helipad cinematic) is NOT spawned
     * here. It is DEFERRED to the main loop until the narrator (sub11, which main00
     * spawns by itself below) finishes — modelling the door-3 self-reentry that
     * re-evaluates sub15 with (4,242) now set by sub11. */

    /* CRITICAL: tick the SCD VM ONCE so main00 registers all Doors and
     * AOTs, THEN settle the AOT edge-state from the player's spawn pos.
     * Without this, Leon spawns inside Door 0's rect (which contains
     * the cut-0 area) and the FIRST aot_scan() in the render loop
     * detects an inside-edge → fires Door 0 → teleports Leon to its
     * destination (17106,-7965,31896, yaw=-29429), instantly leaving
     * cut 0's frustum. PC main.c had this two-call sequence but PSX
     * was missing it — root cause of "PSX Leon invisible" regression. */
    scd_vm_tick();
    re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                       g_actors[RE15_ACTOR_SLOT_PLAYER].z);

    /* The boot tick ran main00, which (with (3,193)=1,(3,125)=0) fired
     * Evt_exec(0x180B) → sub11 into an event slot. Detect it: if a narrator is
     * running we're in the PRE-INTRO (black + narration); sub00 (helipad) is held
     * until sub11 ends. If no narrator spawned (flags already advanced), run the
     * cinematic dispatch (sub00) immediately = the old behaviour. */
    int s_preintro   = 0;
    int s_sub11_slot = -1;
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {
        if (g_scd.threads[s].active) { s_sub11_slot = s; s_preintro = 1; break; }
    }
    if (!s_preintro && re15_test_rdt_ok && re15_test_rdt.sub_scd[0]) {
        scd_thread_start(1, re15_test_rdt.sub_scd[0]);
    }

    /* Phase 4.5.9: prefer cuts parsed from RDT, fall back to the static
     * ROOM1100 array for safety if the RDT parse failed. Both have the
     * same byte layout (32-B BIO15 RID) so the pointer is fungible. */
    const re15_camera_cut_t *active_cuts = re15_test_rdt_ok
        ? re15_test_rdt.cuts : re15_camera_room1100_cuts;
    int active_cut_count = re15_test_rdt_ok
        ? re15_test_rdt.cut_count : re15_camera_room1100_cut_count;

    re15_camera_view_t cam_view;
    int                cam_active_cut = 0;
    re15_camera_build_view(&active_cuts[0], &cam_view);
    mesh_psx_set_camera_fov(&cam_view);

    /* sprite.pri foreground occlusion (2026-06-09): the current cut's masks (parsed
     * once per cut-change) + its CD-loaded atlas (pri_psx.c). Emitted each frame in
     * the render section via re15_render_pri_sprites(). static = off-stack. */
    static re15_pri_cut_t s_pri_cut;
    s_pri_cut.mask_count = 0;

    /* #1d port: active cut's region quad (anchor zone with cam_from==cut) for
     * the per-cut actor/prop cull. Off-stage (teleport-hidden, e.g. -31000)
     * actors outside the quad are not drawn (PSX FUN_8002c18c→FUN_80014368).
     * Refreshed on every cut change below. */
    int16_t cam_region_xs[4] = {0}, cam_region_zs[4] = {0};
    int     cam_has_region = re15_test_rdt_ok
        ? re15_rdt_get_region_quad(&re15_test_rdt, cam_active_cut,
                                   cam_region_xs, cam_region_zs)
        : 0;

    g_engine.frame_count = 0;

    /* RE2-canonical adaptive 3D frame-skip (FUN_8002137c/FUN_8002af20): on a frame
     * that decodes a new MDEC background (initial load + every cut change), the
     * ~10ms MDEC decode + the full 3D render together blow the 33ms budget → the
     * fps dips. RE2 SKIPS the 3D render on those frames (the BG changes that frame
     * anyway, so a 1-frame character skip is invisible across the hard cut). Set on
     * the BG-decode frame, cleared after rendering. Starts 1 (skip the first frame's
     * 3D = the initial BG-load hitch). */
    int skip_3d_frame = 1;

    /* Pre-intro → helipad handoff (2026-06-03). While sub11 (slot 2) narrates over
     * black, sub00 is unspawned; once sub11's thread ends we spawn it, start the
     * BGM, and fade the helipad in from black. */
    int s_sub00_spawned = 0;
    int s_fade_frames   = 0;
    int s_cine_was_active = 0;   /* edge-detect the cinematic-flag set→clear handoff */

    for (;;) {
        re15_render_begin_frame();
        re15_input_tick();

        int _pf_logic0 = GetRCnt(RCntCNT1);   /* perf: logic-phase start */

        /* CANONICAL LOOP (2026-06-01, RE'd from RE_15_Quellcode_V2/main.c): the
         * original does ONE logic-update + ONE render + ONE swap per loop
         * iteration (no (frame&1) gate, no double-render), with the loop paced
         * to 30 Hz. We were gating logic to every-2nd-frame while rendering
         * every frame → identical frames re-rendered + logic running at half
         * the loop rate. Now: tick SCD + walker + FSM EVERY iteration; the loop
         * is paced to 30 Hz by VSync(2) in re15_render_end_frame, so SCD/walker/
         * anim all run at the canonical 30 Hz. */
        {
            scd_vm_tick();
            /* Phase 4.5.13 L-FINAL (2026-05-21): single walker emulating
             * RE1.5's FUN_800245d8 (player_move_by_rotation): per-tick
             * compute target yaw from dest, slew rot_y, translate by
             * per-mode scalar speed. Keyframes are rotations-only — see
             * memory re_locomotion_lround_FINAL_2026_05_21.md. */
            re15_actor_step_all_walkers();

            /* Canonical player-mode + cinematic-end FSM (2026-06-03 audit fix:
             * the handoff is now FLAG-DRIVEN, not fabricated inside op_plc_ret).
             * The intro script (sub02) itself does `Set(2,7,0); Set(1,27,0)` right
             * before `Plc_ret`, so the cinematic ends when those guard flags clear.
             *  - WHILE (1,27)||(2,7): player is SCRIPTED (mode 2), bars held (-1).
             *  - On the set→CLEAR edge (cinematic just ended): start the letterbox
             *    close ramp (the original closes it via the flag-driven FUN_80021a0c
             *    ramp, orthogonal to Plc_ret).
             *  - countdown→0: finalize to gameplay (mode 0, clear subtitle, settle
             *    AOTs so no suppressed door/item fires on the first gameplay frame).
             * NOTE (2026-06-03, RE-verified): Elliot (em type 0x47) is NOT despawned
             * or moved off-stage by the original — he is a behaviour-less, pure-SCD-
             * animated model that simply stays at his last helipad position and is
             * hidden because the gameplay cut (Cut_chg 0x03, the railing) does not
             * frame the helipad centre. The former type==0x47 deactivate sweep was a
             * stand-in and is removed; visibility is left to camera framing + cull. */
            int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
            if (cine_active) {
                g_scd.player_mode         = 2;    /* scripted */
                g_scd.letterbox_countdown = -1;   /* bars held while scripted */
            } else if (s_cine_was_active) {
                g_scd.letterbox_countdown = 15;   /* FUN_80021a0c 0xF0→0 @0x10/frame */
            }
            s_cine_was_active = cine_active;
            if (g_scd.letterbox_countdown > 0 &&
                --g_scd.letterbox_countdown == 0) {
                g_scd.player_mode            = 0;
                /* Do NOT zero message_display_frames here. The original's
                 * cinematic-end path (Plc_ret 0x80041f88, Cut_chg, Cut_auto)
                 * never touches the dialog FSM — a subtitle is dismissed SOLELY
                 * by its own end-wait countdown (DAT_800b8525 in FUN_80028134).
                 * Zeroing it on letterbox-close prematurely killed the LAST
                 * message of a cinematic (e.g. sub14's main16), which — unlike
                 * the earlier lines — has no successor Message_on to re-arm the
                 * display. The message's own duration now governs its lifetime,
                 * exactly like the PSX. */
                re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                                   g_actors[RE15_ACTOR_SLOT_PLAYER].z);
                /* (The 7 type-0x21 crows are NOT spawned here — that was too early. They
                 * appear on HELIPAD RE-ENTRY from the outdoor courtyard, once Elliot/heli/
                 * pilot are gone. See the re-entry spawn wiring below.) */
            }
        }

        /* PRE-INTRO → HELIPAD handoff: once main00's narrator (sub11, running in
         * event slot s_sub11_slot) ends — its Cut_chg(7)+Sleep 20+100*4+20 of "We
         * barricaded ourselves…" over black — fire the FAITHFUL door-3 self-reentry
         * (sub11's Aot_on(3) → door 3, dest = THIS room): re-run the room's
         * init(main00)+main(sub00) with sub11's flags persisting, so main00 takes the
         * Ck(3,125,1)→BGM path and sub00→sub15 sees Ck(4,242,1) → Evt_exec(0x1802) =
         * the helipad cinematic. (Replaces the old hand-deferred sub00 spawn — that
         * faked exactly this.) Start the BGM bank first (before main00's Sce_bgm_control),
         * then re-enter, then kick the 15-frame fade-in. Frame cap = safety. */
        if (s_preintro && !s_sub00_spawned &&
            (!g_scd.threads[s_sub11_slot].active || g_engine.frame_count >= 900)) {
            s_sub00_spawned = 1;
            /* BGM (stage,room) derived from RE15_BOOT_ROOM — single source of truth, no magic
             * literals. 0x1170 → stage 0, room 0x17 (door-graph id encoding: ((stage+1)<<12)|
             * (room<<4)|variant). (This whole block is the BOOT cutscene kickoff — gated by
             * s_preintro, which is set only at boot — not per-room Ballast; the helipad intro
             * scenario 0 is the deliberate boot path.) */
            re15_audio_start_room_bgm((RE15_BOOT_ROOM >> 12) - 1, (RE15_BOOT_ROOM >> 4) & 0xff);
            if (re15_test_rdt_ok)
                scd_room_reenter(&re15_test_rdt,
                                 g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                                 g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0);  /* helipad scenario */
            s_preintro    = 0;
            s_fade_frames = 15;
        }

        /* Phase 4.6.1: drain SCD audio events into the backend each frame. */
        re15_audio_tick();

        /* Rotor (BGM SUB layer) distance attenuation, RE1.5 FUN_80045a64 — SHARED
         * driver (re15_rotor_drive): while the heli prop (obj_id 2) is on stage, fade
         * its rotor by the active cut camera→heli distance. On/off stays with the SCD
         * Sce_bgm_control (0x54); this only attenuates the playing layer. */
        re15_rotor_drive(&active_cuts[cam_active_cut]);

        g_re15_pf_logic = (uint16_t)(GetRCnt(RCntCNT1) - _pf_logic0); /* perf */

        /* Phase 4.3 leftover: re15_render_test_tim drew the loaded TIM as
         * a flat sprite for debug ("we uploaded a texture, here it is").
         * Now that we render full skeletal Leon, the texture-atlas-preview
         * overlap was confusing the user into thinking the flat atlas was
         * the character. Disabled. */
        /* re15_render_test_tim(SCREEN_XRES - 130, 8, 2); */

        /* Phase 4.5.6.3: BG is now blitted from VRAM cache directly into
         * the framebuffer by re15_render_end_frame (via re15_bg_blit),
         * before the OT walks paint meshes/HUD on top — the RE2 pipeline.
         * Gradient kept as a fallback for the rare case where bg_load
         * fails; harmless because BG blit overpaints anything below it. */
        if (!re15_bg_is_loaded()) {
            re15_render_background_gradient(60, 80, 140, 20, 30, 60);
        }

        /* Phase 4.5.5: single rotating textured mesh, centred.
         *
         * pos_z = 10000 is dictated by DOOR00.md1's geometry:
         *   Y-extent  = 6602   (door is 6.6m tall)
         *   Z-extent  = 3599   (door is 3.6m deep)
         *   X-extent  = 290    (door is 0.29m wide — it's a slab asset)
         *
         * The PSX GTE perspective-divides projected screen coords; when
         * any vertex falls inside the near plane (world_z < ~SCREEN_DIST
         * × Y_extent / 1024 ≈ 1933) the projected screen-Y saturates to
         * ±1024 and the GP0 silently discards primitives whose vertex
         * |Δy| then exceeds 511.
         *
         * Worst case for Y-rotation: at θ ≈ 0° the back face (model
         * Z=-3599) approaches the camera. world_z_min = pos_z - 3601.
         * For comfortable headroom over the Y-saturation threshold:
         *
         *   pos_z - 3601 ≥ 6600 × 300 / 511   ⇒   pos_z ≥ 7475
         *
         * We pick 10000 (≈ 33% margin) so a sloppy demo rotation never
         * brushes the limit. Door appears smaller on screen (~10 m
         * camera distance for a 6.6 m door) but stays fully renderable
         * at every rotation angle.
         *
         * Phase 4.5.6+ TODO: implement proper near-plane software
         * clipping so in-game (smaller-scale) cameras can place the
         * player right up against geometry without the demo workaround. */
        /* Per-actor keyframe + model are now resolved inside the render loop
         * (compute_actor_kf), so Leon and Elliot each animate from their own
         * EDD/EMR. */
        /* RE2-pure cut handling: cuts change ONLY via SCD `Cut_chg`
         * (set by SCD scripts or by Door AOTs whose dispatcher writes
         * the request). No timer cycle, no manual cycle keys. The
         * player stays at their world position when the cut changes —
         * Door AOTs that need to teleport do it themselves in their
         * dispatcher (re15_aot_scan case RE15_AOT_TYPE_DOOR). */
        int target_cut = cam_active_cut;
        if (g_scd.cam_change_pending) {
            target_cut = (int)g_scd.cam_id;
            if (target_cut >= active_cut_count) {
                target_cut = active_cut_count - 1;
            }
            g_scd.cam_change_pending = 0;
        }
        if (target_cut != cam_active_cut) {
            cam_active_cut = target_cut;
            re15_camera_build_view(&active_cuts[cam_active_cut],
                                   &cam_view);
            /* Phase 4.5.13-B4: drain any in-flight GPU work before
             * uploading the new FOV. Without this, the GP0 H-register
             * change applies mid-pipeline and the first frame of the
             * new cut renders with the previous cut's perspective
             * matrix → one-frame visual stutter at every cut switch. */
            DrawSync(0);
            mesh_psx_set_camera_fov(&cam_view);
            /* #1c port: sync the NCCT lighting to the active cut — each cut has
             * its own light set (light.lit). Without this every cut used cut 0's
             * lights. Mirrors PC main.c (BE-round re15_light_apply_cut on cut). */
            if (g_re15_room_lights_ok) {
                re15_light_apply_cut(&g_re15_room_lights, cam_active_cut);
            }
            /* #1d: refresh the active cut's region quad for the cull. */
            cam_has_region = re15_test_rdt_ok
                ? re15_rdt_get_region_quad(&re15_test_rdt, cam_active_cut,
                                           cam_region_xs, cam_region_zs)
                : 0;
            /* Phase 4.5.10/4.5.12: swap BG MDEC frame to match new cut.
             * Demo room is ROOM1170 (intro). Per-cut TIM swap deferred to
             * Phase 4.7 CD-load — cut 0's TIM stays resident. */
            re15_bg_load_cut(cam_active_cut);
            /* sprite.pri foreground occlusion: CD-load + upload this cut's atlas, and
             * parse its masks once (re15_render_pri_sprites emits them each frame). */
            re15_pri_psx_load_cut(cam_active_cut);
            s_pri_cut.mask_count = 0;
            if (re15_test_rdt_ok && re15_test_rdt.raw) {
                re15_pri_parse_section(re15_test_rdt.raw, (size_t)re15_test_rdt.raw_size,
                                       active_cuts[cam_active_cut].pri_offset, &s_pri_cut);
            }
            /* RE2 adaptive frame-skip: this frame just did the heavy MDEC decode →
             * skip the 3D render so the cut-change frame fits the budget. */
            skip_3d_frame = 1;
        }

        /* Phase 4.4.5: D-Pad → camera-relative walk. Must run AFTER
         * the cam_view rebuild so movement basis matches the active
         * cut's orientation. */
        /* Action-button press edge (Square) — triggers the stair traversal and
         * the door AOT scan below. */
        /* --- SHARED interpreter step (commons re15_game_step) ---
         * Canonical order: action latch -> stair traversal OR (player move +
         * SCA/object collision) -> door/stair AOT scan -> same-room scenario
         * re-entry consume -> fired-event dispatch. IDENTICAL on PSX and PC
         * (game_step_common.c) so the interpreter cannot drift; only
         * render/input/audio/asset + the SCD-tick cadence + the cross-room
         * CD-load stay per-port. */
        {
            re15_game_ctx_t gctx;
            gctx.rdt         = &re15_test_rdt;
            gctx.rdt_ok      = re15_test_rdt_ok;
            gctx.pl00_skel   = &re15_pl00_skel;
            gctx.pl00_anim   = &re15_pl00_anim;
            gctx.w01_anim    = &re15_w01_anim;   /* walk-source = footstep flags */
            gctx.cam_view    = &cam_view;
            gctx.active_cut  = cam_active_cut;
            gctx.pad_current = (uint16_t)g_engine.pad_current;
            gctx.pad_pressed = (uint16_t)g_engine.pad_pressed;
            re15_game_step(&gctx);
        }
        /* (Same-room door teleports now set the floor band at their source in
         * aot_common.c from the door spawn Y — no position-diff inference here.) */

        /* (The 2026-06-05 L1+R1+SELECT debug warp to ROOM1140 was REMOVED 2026-06-13:
         * door5 is now reachable on foot — descend the outdoor stairs into the band-0
         * pit, then trigger door5 — USER-VERIFIED "funktioniert alles einwandfrei".
         * The real cross-room door is the only path now.) */

        /* (Same-room SCENARIO re-entry consume is now in the shared
         * re15_game_step() above — fires the same frame, right after the scan.) */

        /* Multi-room (2026-06-04): a DOOR AOT may have queued a cross-room
         * transition during the scan. Execute it HERE (after the scan, never
         * mid-scan): CD-load the destination RDT, re-alias re15_test_rdt, rewind
         * the prop-VRAM bump allocator + discard the cached BG, re-init the SCD
         * VM for the new room, and drop the player at the door's destination
         * spawn. (Same-room doors don't reach here — handled as cut+pos teleport
         * in the scan.) RE2 mirror: door-pass → room CdRead + re-init. */
        /* Cross-room DOOR transition (a door queued g_room_change during the scan):
         * run the SHARED re15_room_apply_pending (room_common.c) — identical
         * LOGIC on PSX+PC; only the 3 ARCH callbacks differ (here: CD RDT load,
         * VRAM/BG reset, MDEC BG decode). It re-aliases the RDT, re-inits
         * actors/SCD, sets spawn + entry cut, per-room lights/messages/band/BGM. */
        {
            re15_room_apply_ctx_t rc;
            rc.rdt              = &re15_test_rdt;
            rc.rdt_ok           = &re15_test_rdt_ok;
            rc.active_cuts      = &active_cuts;
            rc.active_cut_count = &active_cut_count;
            rc.cam_active_cut   = &cam_active_cut;
            rc.cam_view         = &cam_view;
            rc.load_rdt         = re15_room_load;             /* CD */
            rc.reset_render     = psx_room_reset_render;      /* VRAM rewind + BG invalidate */
            rc.load_bg_cut      = re15_bg_load_cut;  /* MDEC (room-aware names = multi-room TODO) */
            rc.load_props       = re15_load_room_props;       /* data-driven per-room Obj_model_set props */
            rc.load_cinematic   = re15_load_room_cinematic;   /* data-driven per-room rbj overlay + special NPC */
            if (re15_room_apply_pending(&rc)) {
                /* Refresh the per-cut region-quad cull for the NEW room/cut. It is
                 * otherwise ONLY updated in the cut-CHANGE block (line ~453), which
                 * this direct entry-cut set bypasses → the player + props would be
                 * culled against the PREVIOUS room's stale region until the next cut
                 * switch. That was "Leon nicht sofort sichtbar nach Raumwechsel — erst
                 * bei einem Kamera-Change" (the entry cut frames him, but the stale
                 * region quad culled him). Mirrors lines 453-455. */
                cam_has_region = re15_test_rdt_ok
                    ? re15_rdt_get_region_quad(&re15_test_rdt, cam_active_cut,
                                               cam_region_xs, cam_region_zs)
                    : 0;
            }
        }

        /* (Fired-AOT-event dispatch is now in the shared re15_game_step().) */

        /* Pre-intro narrator runs over a BLACK screen — suppress all 3D actors/props
         * (they'd otherwise paint over the blackout in the OT). The subtitle text is
         * emitted separately below, so the narrator lines still show. */
        if (s_preintro) skip_3d_frame = 1;

        int _pf_rb0 = GetRCnt(RCntCNT1);   /* perf: 3D render-build start */

        /* Phase 4.5.9-D: unified render loop over every active actor.
         * Slot 0 = player (uses live kf_idx from anim_frame), slots 1+
         * are NPCs (frame 0 fallback until they get their own AI). */
        for (int ai = 0; ai < (int)g_actor_count; ai++) {
            if (skip_3d_frame) break;   /* RE2 adaptive skip on MDEC/cut frames */
            const re15_actor_t *a = &g_actors[ai];
            if (!a->active) continue;
            /* Player (slot 0) is region-cull-EXEMPT (the gameplay camera always
             * frames him), but the sub02 cinematic teleport-HIDES Leon at x=-31000
             * during cuts where only the heli/NPCs show (e.g. the sky-view). The PSX
             * skeletal render has no whole-object far-clip, so without this gate the
             * hidden Leon projects as a tiny figure floating in the sky. The SCD
             * hides him by parking his X off-stage at -31000 (Member_set(0,-31000));
             * skip the whole actor (mesh + shadow) at that off-stage X. On-stage
             * cinematic + gameplay X stays well above -30000 (helipad/landing). */
            /* Per-cut region-quad cull for ALL actors INCLUDING the player — byte-true
             * FUN_80039ca0→FUN_80014368 against DAT_800ac790, the SAME quad the NPCs/
             * props already use. An actor whose world XZ falls outside the active cut's
             * region quad is not drawn: a teleport-hidden actor (SCD Member_set(0,-31000):
             * the off-stage Leon in the sky cut, Elliot/pilot, or the flown-off heli)
             * lands outside → culled; on-stage gameplay/cinematic actors are inside →
             * drawn. (Replaces the eyeballed player `x<=-30000` off-stage gate — #18
             * 2026-06-09; the player is no longer exempt.) */
            if (cam_has_region &&
                !re15_aot_point_in_quad(a->x, a->z, cam_region_xs, cam_region_zs)) {
                continue;
            }
            int16_t a_sin = (int16_t)re15_sin_q12((int)a->rot_y);
            int16_t a_cos = (int16_t)re15_cos_q12((int)a->rot_y);
            /* Actors are skeletal characters: slot 0 = Leon (player, live kf_idx),
             * type 0x47 = Elliot (NPC; PC dispatch is_elliot = type==0x47). The
             * helicopter is NOT an actor — it's prop obj_id 2, drawn by the prop
             * loop. (Elliot still renders with the bundled Leon model + frame 0
             * until his own skel/anim/md1 are wired — next step.) */
            /* Pick the actor's model + texture: Elliot (NPC type 0x47) uses his
             * own PL05 skel/anim/md1 + relocated TIM; everyone else (the player,
             * any other NPC) uses the bundled Leon model with its baked texture
             * (tpage/clut = -1). Keyframe comes from each actor's OWN anim, so
             * Leon and Elliot animate independently (also covers #1b). */
            /* SHARED anim selection (unify 2026-06-06): the bank/clip view-model
             * is game logic, so it lives in anim_select_common.c and drives BOTH
             * renderers. Fill the bank table from the PSX globals; the selector
             * encodes locomotion 105/100, idle-fidget 200/210/211/212, HP-gated
             * injured idle 213/214, Elliot's own bank + 2-tpage/2-CLUT texture,
             * else the actor's own cinematic bank (motion = direct clip index). */
            /* GENERIC ENEMY (Step B, 2026-06-13; em21 folded 2026-06-14f): the first frame
             * a non-player, non-Elliot actor of a given type appears (INCLUDING type 0x21,
             * the crows), lazy CD-load its EM<NN>.EMD into the shared per-room arena
             * (re15_enemy_load). The selector routes it via re15_enemy_find; until loaded it
             * falls back to the Leon placeholder. */
            if (ai != RE15_ACTOR_SLOT_PLAYER && a->type && a->type != 0x47
                && !re15_enemy_find(a->type))
                re15_enemy_load(a->type);

            re15_anim_banks_t banks = {
                .def_mesh = &re15_test_md1,
                .def_skel = &re15_test_skel,
                .def_anim = &re15_test_anim,
                .w01_skel = &re15_w01_skel,  .w01_anim = &re15_w01_anim,  .w01_ok = re15_w01_ok,
                .pl00_skel = &re15_pl00_skel, .pl00_anim = &re15_pl00_anim, .pl00_ok = re15_pl00_ok,
                .elliot_mesh = &re15_elliot_md1,
                .elliot_skel = &re15_elliot_skel,
                .elliot_anim = &re15_elliot_anim,
                .elliot_ok = re15_elliot_ok,
                .elliot_tpage_xshift = re15_elliot_tpage_xshift,
                .elliot_clut_yshift  = re15_elliot_clut_yshift,
            };
            re15_anim_view_t av;
            re15_actor_anim_select(a, ai == RE15_ACTOR_SLOT_PLAYER, &banks, &av);
            const re15_md1_t           *amd1   = av.mesh;
            const re15_emd_skeleton_t  *askel  = av.skel;
            const re15_emd_animation_t *aanim  = av.anim;
            int atpage = av.tpage, aclut = av.clut, ayshift = av.yshift, axshift = av.xshift;
            int a_kf = re15_compute_actor_kf(aanim, askel, a, av.clip_override,
                                             (uint32_t)a->anim_frame);
            /* RE1.5 character floor shadow (FUN_8001b064): subtractive blob quad
             * under this actor, oriented by the CAMERA yaw (active cut forward).
             * Emitted before the mesh; the OT depth-sorts it onto the floor so the
             * figure occludes its own feet. Off-stage actors self-cull (otz<near). */
            mesh_psx_render_actor_shadow(/*z*/ 3, &cam_view, a->x, a->y, a->z,
                active_cuts[cam_active_cut].target_x - active_cuts[cam_active_cut].pos_x,
                active_cuts[cam_active_cut].target_z - active_cuts[cam_active_cut].pos_z);
            /* Byte-true FRAC crossfade: tell re15_skel_compute_pose which actor this
             * body is so it can blend the new clip in from the actor's prior pose. */
            g_anim_pose_actor = a;
            mesh_psx_render_skeletal(/*z*/ 3,
                                     &cam_view, amd1, askel,
                                     a->x, a->y, a->z,
                                     a_sin, a_cos,
                                     /*backface_cull*/ 1,
                                     /*mode*/ MESH_PSX_MODE_TEXTURED,
                                     /*keyframe_index*/ a_kf,
                                     atpage, aclut, ayshift, axshift);
        }

        /* #2 prop loop: render Obj_model_set props (g_scd.props) — the helipad
         * fixtures (obj00/01 = the BOX left of Leon), main+tail rotor (obj03/04)
         * and pilot (obj05). All are drawn MESHES. RE'd FUN_8002c18c iterates the
         * whole object pool and renders EACH active object (type != 3 → mesh +
         * whole-object quad cull; type 3 → sprite-grid path). Every room1170 prop
         * is type 0 (the real type byte pc[2], not the old pc[4]=BAND field), so
         * there is NO "render only type 3/5" gate — that was a coincidence of the
         * wrong type read. The render set is the active props with a valid model.
         * obj02 (heli) renders via the actor 0x47 path → re15_obj_ok[2] is false,
         * so it is skipped here by the model gate (never the type). */
        for (int pi = 0; pi < (int)g_scd.prop_count; pi++) {
            if (skip_3d_frame) break;   /* RE2 adaptive skip on MDEC/cut frames */
            int oid = g_scd.props[pi].obj_id;
            int otype = g_scd.props[pi].obj_type;
            if (!g_scd.props[pi].active)          continue;
            if (oid < 0 || oid >= 6 || !re15_obj_ok[oid]) continue;
            int32_t px = g_scd.props[pi].x, py = g_scd.props[pi].y, pz = g_scd.props[pi].z;
            /* Byte-true per-prop cull, SHARED with the PC port (re15_prop_culled in
             * re15_aot.h): type-0 props (all of room1170's) take the whole-object
             * FUN_80014368 region-quad cull, with a sink-gate (x<-25000) fallback
             * for region-less cinematic cuts so the SCD-sunk box stays hidden. */
            if (re15_prop_culled(otype, px, pz, cam_has_region,
                                 cam_region_xs, cam_region_zs))
                continue;
            int16_t ps = (int16_t)re15_sin_q12((int)g_scd.props[pi].rot_y);
            int16_t pc = (int16_t)re15_cos_q12((int)g_scd.props[pi].rot_y);
            mesh_psx_render_prop(/*z*/ 3, &cam_view, &re15_obj_md1[oid],
                                 px, py, pz, ps, pc,
                                 /*backface_cull*/ 1, /*mode*/ MESH_PSX_MODE_TEXTURED,
                                 re15_obj_tpage[oid], re15_obj_clut[oid]);
        }
        g_re15_pf_rbuild = (uint16_t)(GetRCnt(RCntCNT1) - _pf_rb0);   /* perf */

        /* RE2 adaptive frame-skip: 3D was skipped this frame only — resume next. */
        skip_3d_frame = 0;

        /* sprite.pri FOREGROUND OCCLUSION: emit the cut's foreground sprites into the
         * OT (ot[1], in front of the actor tris, behind the ot[0] subtitle text) so
         * railings/boxes/fixtures occlude the actors. No-op if the cut has no atlas. */
        re15_render_pri_sprites(&s_pri_cut);

        /* #1e port (2026-06-01): dead demo scaffolding (bouncing square) + the
         * entire debug HUD removed — not present in the PC build; they cluttered
         * the cinematic and blocked clean visual comparison. The message-display
         * countdown still ticks (game state, not HUD). */
        /* Subtitle (shared dismiss FSM, re15_msg_tick): a message lives for its own
         * duration countdown and is dismissed ONLY when it expires — identical to the
         * PC build, so the LAST line of a cinematic (sub14 main16) is never cut short.
         * Intro subtitle (2026-06-03): the REAL TEX.TIM glyph font (FUN_80028868)
         * replaces the old ASCII + BIOS-debug-font path. Render the RAW .msg bytes so
         * the embedded control codes are honoured: 0x05 → per-speaker CLUT-row colour
         * (Leon green / Pilot orange / Elliot blue / white), 0x08 → newline. Box origin
         * = the RE'd (34,180) (0x300 Message_on). */
        {
            const unsigned char *raw = 0;
            int rlen = 0;
            int _tk = re15_msg_tick(&raw, &rlen, 0);
            if (_tk && raw && rlen > 0)
                re15_render_msg_text(34, 180, raw, rlen);
            /* PAGE BREAK (FSM state 1): blinking down-arrow = "press action for the next
             * page" (byte-true FUN_80028134 state 2 draws DAT_80010938 when timer&0x18). */
            if (_tk && g_scd.message_fsm == 1 && (g_scd.message_blink & 0x18))
                re15_render_msg_down_arrow(160, 222);
            /* YES/NO selection prompt (ROOM1130 switch etc.): when a query Message_on is
             * waiting for the player, draw the two options + a blinking '>' cursor on the
             * live choice. Byte-true layout from FUN_80028134 state 4 (Yes col 0xa0=160,
             * No col 0xa0+0x46=230, row (0xb4+0x10)=196). Option glyph codes are the .msg
             * charset (A-Z @0x1D, a-z @0x3D): "Yes"={0x35,0x41,0x4F}, "No"={0x2A,0x4B}.
             * The cursor is a '>' triangle (the original's menu-font glyph 0x26, an 8×8
             * TEX.TIM sub-region we don't load) and blinks per the state-4 timer
             * (visible when message_blink & 0x18 — counter decremented each frame). */
            if (g_scd.message_select) {
                /* Byte-true positions (FUN_80028134 state 4): cursor cell X =
                 * choice*0x46 + 0xa0 = 160 (Yes) / 230 (No); options string at 0xae=174;
                 * row (0xb4+0x10)=196. So each option sits 14px right of its cursor —
                 * Yes@174 (cursor 160), No@244 (cursor 230, = 174+0x46). */
                static const unsigned char yes_g[3] = { 0x35, 0x41, 0x4F };
                static const unsigned char no_g[2]  = { 0x2A, 0x4B };
                re15_render_msg_text(174, 196, yes_g, 3);
                re15_render_msg_text(244, 196, no_g,  2);
                if (g_scd.message_blink & 0x18)
                    re15_render_msg_cursor((g_scd.message_choice ? 230 : 160), 196);
            }
        }
        /* #1e: inventory pickup-echo countdown still ticks (HUD text removed). */
        if (g_inv.last_pickup_display_frames > 0) g_inv.last_pickup_display_frames--;



        /* #1a port: drive the cinematic letterbox from the shared FSM. 24px =
         * canonical PSX POLY_F4 bar height; off (0) once the countdown hits 0.
         * letterbox_countdown != 0 covers both held (-1, scripted) and the
         * closing window (15..1 after Plc_ret). */
        re15_render_set_letterbox(g_scd.letterbox_countdown != 0 ? 24 : 0);

        /* Pre-intro black hold + helipad fade-in (FUN_80021a0c). During the
         * narrator the framebuffer is held black; on the handoff a 15-frame
         * subtractive ramp 0xF0→0 reveals the helipad. */
        re15_render_set_blackout(s_preintro);
        if (s_fade_frames > 0) {
            re15_render_set_fade((s_fade_frames * 0xF0) / 15);
            s_fade_frames--;
        } else {
            re15_render_set_fade(0);
        }

        re15_render_end_frame();
        g_engine.frame_count++;
    }

    return 0;
}
