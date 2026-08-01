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
#include "re15_savepoint.h"   /* re15_savepoint_reset on room change */
#include "re15_itembox.h"     /* re15_itembox_reset on room change (box pending) */
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
    re15_savepoint_reset();   /* new room: drop any stale save pending/latched-cut from the old room */
    re15_itembox_reset();     /* likewise the box-screen pending (save-phone precedent) */
    g_scd_self_reenter_fired = 0;  /* new room: re-arm the intro self-reenter latch (a genuine
                                    * later re-entry must be able to run its own door scenario) */

    /* (1) ARCH: load the destination ROOM####.RDT (CD on PSX / file on PC) into
     * g_room_rdt. Abort the transition if it fails (player stays put). */
    if (c->load_rdt(g_room_change.room_id) != 0) return 0;

    /* (2) PER-RAUM-TEARDOWN — VOR dem Re-Aliasing, denn ab hier zeigt *c->rdt auf den neuen Raum
     * und jede Aufraeum-Entscheidung, die noch den alten Zustand braucht, kaeme zu spaet.
     *
     * REIHENFOLGE NACH DEM ORIGINAL: FUN_800396FC gibt die Arena frei (@0x80039738 setzt den
     * Bump-Pointer 0x800AC77C auf die Basis 0x800AC780) und laedt die neue RDT ab genau dieser
     * Adresse (@0x80039740/@0x800397E8) — Freigabe steht also VOR der Installation, nicht danach.
     * Der Port kann das Laden nicht davor schieben (Schritt 1 bricht bei Ladefehler ab und laesst
     * den Spieler stehen — diese Semantik bleibt), aber der Teardown gehoert vor das Re-Aliasing.
     * Frueher lief er als Schritt (3) NACH dem Alias und auf PC ausserdem als leerer Rumpf. */
    c->reset_render();

    /* (3) Re-alias the port's resident RDT to the freshly-parsed new room. */
    *c->rdt    = g_room_rdt;
    *c->rdt_ok = 1;

    /* (3b) ARCH: load the destination room's Obj_model_set prop set into the now-
     * reset prop VRAM (data-driven per room; clears the previous room's prop slots).
     * Ports without per-room props leave this NULL. */
    if (c->load_props) c->load_props(g_room_change.room_id);

    /* (4) Wipe the previous room's NPCs/props (re-seeds slot 0 = player); the
     * new room's SCD re-spawns its own via Obj_model_set. (scd_room_reenter
     * memset-clears g_scd props below; NOT done here as that path is also the
     * intro self-reentry, which must keep the cast.) */
    /* SPIELER-GESUNDHEIT UEBER DEN RAUMWECHSEL RETTEN — re15_actor_init() memset-t ALLE Slots und
     * setzt Slot 0 danach auf hp=100 (actor_common.c). Damit heilte der Port den Spieler an JEDER
     * Tuer voll und loeschte Gift/Blutung; g_actors[0].hp ist die echte Spieler-HP (der Todes-Check
     * re15_damage.c liest sie).
     *
     * DAS ORIGINAL TUT DAS NICHT: der Raumwechsel wischt NUR den Entity-Pool — FUN_8001A4C0 nullt
     * 20 Slots ab 0x800ACC2C mit Stride 0x1F4, und zwar ausschliesslich deren Feld +0x00
     * (@0x8001A4E8). Der SPIELERBLOCK liegt bei 0x800ACA54 und damit AUSSERHALB dieses Pools. Die
     * einzige HP-Schreibstelle im gesamten Raumwechsel-Pfad ist @0x80031718 (sh 0x64 -> +0x9A) in
     * FUN_800314B0, und die ist hinter dem Charakterwechsel-Gate @0x80039760-8C eingesperrt: sie
     * feuert nur, wenn (0x800ACA5C & 0x0F) != Etage 0x800B0FF0 — also beim Modellwechsel, NICHT bei
     * einem gewoehnlichen Raumwechsel. Dasselbe gilt fuer das Statuswort +0x98 (@0x80031720,
     * Bit 0x2 = Gift/Blutung).
     *
     * Deshalb: Slot 0 durch die Neuinitialisierung schicken (Hitbox, Root-Motion-Tracker, Typ/Flags
     * kommen sauber zurueck), aber die beiden Felder wiederherstellen, die das Original nicht
     * anfasst. */
    const int16_t  keep_hp     = g_actors[RE15_ACTOR_SLOT_PLAYER].hp;
    const uint16_t keep_status = g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags;
    re15_actor_init();
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp           = keep_hp;
    g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags = keep_status;

    /* SKRIPT-ANTRIEB DES SPIELERS BEENDEN — das fehlende Gegenstueck zum Original.
     *
     * Die Transitions-FSM nullt das Kommandoregister des Spielers: @0x8001CBDC 'sb zero,0x800ACA58'
     * (= Spieler +0x04, State 0). Der State-0-Handler @0x800318F8 (Tabelle @0x80073F90, Eintrag 0)
     * raeumt danach auf: Motion +0x94 = 0 (@0x80031924), +0x95 = 0 (@0x80031954), +0x1C4 = 0
     * (@0x8003197C), und setzt State auf 1 (@0x8003192C).
     *
     * WARUM DAS WICHTIGER IST ALS ES AUSSIEHT: genau dieser Reset macht im Original den
     * Clip-Index-Ueberlauf UNERREICHBAR. Das Original prueft nirgends gegen eine Clip-Anzahl —
     * Plc_motion schreibt den Index ungeprueft (@0x80041BA8 sb a1,148(v0)), und die drei Leser von
     * +0x94 (@0x8001F324, @0x8001F8E4, @0x8001AE48) rechnen EDD_base + motion*4 roh. Eine
     * Clip-Anzahl existiert im EDD nicht einmal als Feld. Der Ueberlauf ist im Original also nur
     * deshalb harmlos, WEIL der Raumwechsel Motion und State vorher auf 0 setzt.
     *
     * Der Port hatte dafuer kein Gegenstueck (scd_room_setup.c mappt nur die Port-Sentinels) und
     * fing den Ueberlauf stattdessen mit einem MODULO-Wrap ab (anim_select_common.c) — ein Netz,
     * das das Original nicht hat. Mit diesem Reset ist der Fall gar nicht mehr erreichbar. */
    {
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p->motion      = 0;    /* +0x94 @0x80031924 */
        p->anim_frame  = 0;    /* +0x95 @0x80031954 */
        p->anim_flags  = 0;    /* +0x1C4 @0x8003197C */
        p->sub_state_1 = 0;
        p->sub_state_2 = 0;
        p->sub_state_3 = 0;
        p->state       = 1;    /* @0x8003192C — Routine 0 laeuft durch, State steht danach auf 1 */
    }
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

    /* (7) Re-init the SCD VM for the new room (main00+sub00 of the new RDT). This runs
     * main00 THROUGH its spawned sub's first yield — so a room whose main00 fires an
     * Evt_exec that immediately issues a Cut_chg has that Cut_chg queued here. */
    scd_room_reenter(c->rdt, g_room_change.x, g_room_change.z, 0);

    /* (8) Camera precedence (byte-true, RE'd 2026-07-22 from the ROOM1170 pre-intro trace):
     * the new room's SCD init may have issued its OWN Cut_chg — e.g. ROOM1170 main00 ->
     * Evt_exec(0x180B)=sub11 (the pre-intro narrator) -> Cut_chg(7), whose cut 7 is a VOID
     * camera that renders the black narrator backdrop. The ORIGINAL runs main00 AFTER the door
     * cut, so that Cut_chg OVERRIDES the door entry cut and also turns the RVD auto-scan OFF
     * (op_cut_chg -> cut_auto_enabled=0). The port previously ALWAYS dropped the SCD's cut
     * (cam_change_pending=0) + forced cut_auto=1 below, so the CAM_SWITCH scan picked the
     * player's helipad zone and the narration read over the helipad instead of the black cut 7.
     * FIX: when the SCD init queued a DIFFERENT cut, HONOR it (keep cam_change_pending + the
     * cut_auto=0 it set); only otherwise force the door's entry cut (frames the teleported
     * player) + enable the gameplay auto-camera (steps 8b/12 below). */
    int scd_queued_cut = (g_scd.cam_change_pending && (int)g_scd.cam_id != cut)
                       ? (int)g_scd.cam_id : -1;
    if (scd_queued_cut >= 0) {
        cut = scd_queued_cut;          /* the scripted cut is authoritative; leave cam_change_pending=1
                                        * so the main-loop cut block applies it + loads its BG */
        *c->cam_active_cut = cut;
    } else {
        g_scd.cam_change_pending = 0;  /* no SCD cut -> the door's entry cut frames the spawn */
    }
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
     * (scd_room_reenter memset-cleared g_scd.cut_auto). NOT when the SCD init locked a
     * scripted cut (its Cut_chg set cut_auto=0): re-enabling the scan would let it override
     * the scripted narrator cut 7 with the player's zone (the ROOM1170 pre-intro bug). */
    if (scd_queued_cut < 0)
        g_scd.cut_auto_enabled = 1;

    /* (13) New room floor band from the door spawn Y (-(Y/0x708)). */
    re15_collision_set_band(re15_collision_band_from_y(pl->y));

    /* (14) No half-finished stair traversal carries across rooms. */
    re15_stair_reset();

    /* (15) Per-room BGM (SS_BGMTBL): switch to the new room's track. */
    re15_audio_start_room_bgm(0, (int)((g_room_change.room_id >> 4) & 0xff));

    return 1;
}
