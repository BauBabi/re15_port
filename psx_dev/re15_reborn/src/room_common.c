/*
 * room_common.c — SHARED cross-room transition state + logic.
 *
 * The cross-room door transition is the SAME on PSX and PC; only the asset I/O
 * (RDT load = CD vs file, render reset = VRAM/BG vs texcache, BG decode = MDEC
 * vs file) is architecture-specific and supplied by each port through callbacks
 * in re15_room_apply_ctx_t. This file holds the cross-room state globals + the
 * request queue + the apply logic so the two main loops cannot drift.
 *
 * (Split out of the PSX-only re15_room.c — which now keeps ONLY the CD loader —
 * so the PC build can link the same transition; see re15_room.h.)
 */
#include <stdint.h>
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_enemy.h"   /* re15_enemy_reset on room change */
#include "re15_scd.h"
#include "re15_camera.h"
#include "re15_light.h"
#include "re15_msg.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_audio.h"

/* Cross-room state — the parsed RDT of the resident room + the queued change.
 * (Moved here from re15_room.c so BOTH ports link it.) The actual RDT BYTES
 * live in each port's loader buffer; g_room_rdt just holds the parsed pointers. */
re15_rdt_t          g_room_rdt;
int                 g_room_rdt_ok    = 0;
unsigned            g_current_room_id = 0x1170;   /* room resident in g_room_rdt */
re15_room_change_t  g_room_change    = {0};       /* queued cross-room transition */

void re15_room_request_change(unsigned room_id, int32_t x, int32_t y, int32_t z,
                              int16_t yaw, int target_cut)
{
    g_room_change.pending    = 1;
    g_room_change.room_id    = room_id;
    g_room_change.x = x; g_room_change.y = y; g_room_change.z = z;
    g_room_change.yaw_4096   = yaw;
    g_room_change.target_cut = target_cut;
}

int re15_room_apply_pending(const re15_room_apply_ctx_t *c)
{
    if (!g_room_change.pending) return 0;
    g_room_change.pending = 0;

    /* (1) ARCH: load the destination ROOM####.RDT (CD on PSX / file on PC) into
     * g_room_rdt. Abort the transition if it fails (player stays put). */
    if (c->load_rdt(g_room_change.room_id) != 0) return 0;

    /* (2) Re-alias the port's resident RDT to the freshly-parsed new room. */
    *c->rdt    = g_room_rdt;
    *c->rdt_ok = 1;

    /* (3) ARCH: reset the renderer for the new room (PSX: rewind prop-VRAM bump
     * allocator + invalidate the BG cache; PC: drop the per-room texture cache). */
    c->reset_render();

    /* (3b) ARCH: load the destination room's Obj_model_set prop set into the now-
     * reset prop VRAM (data-driven per room; clears the previous room's prop slots).
     * Ports without per-room props leave this NULL. */
    if (c->load_props) c->load_props(g_room_change.room_id);

    /* (4) Wipe the previous room's NPCs/props (re-seeds slot 0 = player); the
     * new room's SCD re-spawns its own via Obj_model_set. (scd_room_reenter
     * memset-clears g_scd props below; NOT done here as that path is also the
     * intro self-reentry, which must keep the cast.) */
    re15_actor_init();
    re15_enemy_reset();   /* drop the previous room's loaded enemy models (free PC bufs);
                           * the new room lazy-loads its own on first spawn. */

    /* (4b) ARCH: load the dest room's CINEMATIC overlay (rbj) + special NPC (Elliot) into the
     * per-room arena — data-driven per room (PSX; PC keeps its boot-loaded copies). Runs AFTER
     * reset_render (arena reset + Leon's skel re-pointed to PL00 base) and the enemy reset, so
     * it overlays from the clean base. NULL for ports/rooms without one. */
    if (c->load_cinematic) c->load_cinematic(g_room_change.room_id);

    /* (5) Re-point the camera cut table at the new room + set the ENTRY cut the
     * door specified (Door_aot_set dest_cut) — that cut frames the door spawn.
     * RVD zones then switch cuts as the player walks. */
    *c->active_cuts      = g_room_rdt.cuts;
    *c->active_cut_count = g_room_rdt.cut_count;
    int cut = g_room_change.target_cut;
    if (cut >= *c->active_cut_count) cut = *c->active_cut_count - 1;
    if (cut < 0) cut = 0;
    *c->cam_active_cut = cut;

    /* (6) Drop the player at the door's destination spawn (AFTER the actor wipe). */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = g_room_change.x;
    pl->y = g_room_change.y;
    pl->z = g_room_change.z;
    pl->rot_y = g_room_change.yaw_4096;

    /* (7) Re-init the SCD VM for the new room (main00+sub00 of the new RDT). */
    scd_room_reenter(c->rdt, g_room_change.x, g_room_change.z, 0);

    /* (8) The door's entry cut is authoritative — drop any cut the new room's
     * SCD init queued so it can't override the framed view; rebuild the view. */
    g_scd.cam_change_pending = 0;
    re15_camera_build_view(&(*c->active_cuts)[cut], c->cam_view);

    /* (9) ARCH: stream the entry cut's BG now (the per-cut load only fires on a
     * cut CHANGE; we just set cam to the entry cut after that block ran). */
    c->load_bg_cut(cut);

    /* (10) Per-room lighting: re-parse the new room's NCCT light set + apply. */
    if (c->rdt->lights &&
        re15_light_parse(c->rdt->lights, (size_t)c->rdt->lights_size,
                         &g_re15_room_lights) == 0) {
        g_re15_room_lights_ok = 1;
        re15_light_apply_cut(&g_re15_room_lights, cut);
    }

    /* (11) Per-room messages: interpret the new room's RDT message table. */
    re15_msg_load_room_block(c->rdt->messages, c->rdt->messages_size);

    /* (12) A door entry is GAMEPLAY → enable the RVD/CAM_SWITCH auto-camera
     * (scd_room_reenter memset-cleared g_scd.cut_auto). */
    g_scd.cut_auto_enabled = 1;

    /* (13) New room floor band from the door spawn Y (-(Y/0x708)). */
    re15_collision_set_band(re15_collision_band_from_y(pl->y));

    /* (14) No half-finished stair traversal carries across rooms. */
    re15_stair_reset();

    /* (15) Per-room BGM (SS_BGMTBL): switch to the new room's track. */
    re15_audio_start_room_bgm(0, (int)((g_room_change.room_id >> 4) & 0xff));

    return 1;
}
