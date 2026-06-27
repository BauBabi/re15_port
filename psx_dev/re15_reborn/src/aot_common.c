/*
 * RE1.5 Rebuilt — AOT runtime (Phase 4.4.6, 2026-05-19).
 *
 * Target-agnostic. Per-frame point-in-rect over up to RE15_AOT_MAX
 * trigger zones, with edge-trigger semantics so each entry fires
 * exactly once even when the player loiters inside.
 *
 * RE2 has rect AND circle AOT shapes (per FUN_80051088.c flag bit 0x80).
 * We only implement rect for now — covers all of Aot_set / Door_aot_set
 * / Item_aot_set in the extracted RE1.5 scripts. Circle can be added
 * when something actually uses it.
 */

#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif
#include "re15_aot.h"
#include "re15_scd.h"        /* g_game (player pos), g_scd (cam, audio queue) */
#include "re15_inventory.h"
#include "re15_audio.h"      /* audio event kinds for door/pickup SFX */
#include "re15_actor.h"      /* Phase 4.5.9-D: player = g_actors[0] */
#include "re15_skeleton.h"   /* re15_sin_q12/cos_q12 — door forward-reach trigger */
#include "re15_collision.h"  /* set the floor band at a same-room door (band from spawn Y) — shared (PSX + PC) */
#include "re15_room.h"       /* g_current_room_id + cross-room transition request.
                              * SHARED now (room_common.c) — both ports queue the
                              * cross-room change; only the RDT LOADER (CD vs file)
                              * differs, behind the re15_room_apply_pending ctx. */

re15_aot_state_t g_aot;
uint8_t g_aot_action_pressed = 0;   /* set per-frame by the main loop (door action gate) */

void re15_aot_init(void)
{
    memset(&g_aot, 0, sizeof(g_aot));
}

int re15_aot_set(int slot, uint8_t type, uint8_t event_id,
                  int32_t cx, int32_t cz, int32_t half_w, int32_t half_h)
{
    if (slot < 0 || slot >= RE15_AOT_MAX) return -1;
    re15_aot_t *a = &g_aot.slots[slot];
    a->active          = 1;
    a->type            = type;
    a->event_id        = event_id;
    a->was_inside      = 0;
    a->cam_from_filter = 0xFF;   /* default = no filter; CAM_SWITCH overrides */
    a->x               = cx;
    a->z               = cz;
    a->half_w          = half_w < 0 ? -half_w : half_w;
    a->half_h          = half_h < 0 ? -half_h : half_h;
    return 0;
}

int re15_aot_set_door(int slot, int32_t cx, int32_t cz,
                       int32_t half_w, int32_t half_h,
                       uint8_t target_cut,
                       int32_t spawn_x, int32_t spawn_y, int32_t spawn_z,
                       int16_t spawn_yaw_4096)
{
    int rc = re15_aot_set(slot, RE15_AOT_TYPE_DOOR, 0, cx, cz, half_w, half_h);
    if (rc != 0) return rc;
    re15_aot_door_params_t *d = &g_aot.door_params[slot];
    d->target_cut      = target_cut;
    d->spawn_x         = spawn_x;
    d->spawn_y         = spawn_y;
    d->spawn_z         = spawn_z;
    d->spawn_yaw_4096  = spawn_yaw_4096;
    return 0;
}

int re15_aot_set_item(int slot, int32_t cx, int32_t cz,
                       int32_t half_w, int32_t half_h,
                       uint8_t item_type, uint8_t amount)
{
    int rc = re15_aot_set(slot, RE15_AOT_TYPE_ITEM, 0, cx, cz, half_w, half_h);
    if (rc != 0) return rc;
    re15_aot_item_params_t *p = &g_aot.item_params[slot];
    p->item_type = item_type;
    p->amount    = amount;
    return 0;
}

int re15_aot_set_stair(int slot, int32_t cx, int32_t cz,
                       int32_t half_w, int32_t half_h,
                       uint8_t down_end, uint8_t band)
{
    /* event_id carries the down/up-end flag (data0); band = the runtime aot+0x82
     * (Aot_set chain pc[4]) = the platform this stair end sits on. */
    int rc = re15_aot_set(slot, RE15_AOT_TYPE_STAIR, down_end, cx, cz, half_w, half_h);
    if (rc != 0) return rc;
    g_aot.slots[slot].band = band;
    return 0;
}

int re15_aot_set_cam_switch(int slot, int32_t cx, int32_t cz,
                             int32_t half_w, int32_t half_h,
                             uint8_t cam_from, uint8_t target_cut)
{
    int rc = re15_aot_set(slot, RE15_AOT_TYPE_CAM_SWITCH,
                          target_cut, cx, cz, half_w, half_h);
    if (rc != 0) return rc;
    g_aot.slots[slot].cam_from_filter = cam_from;
    return 0;
}

void re15_aot_reset(int slot)
{
    if (slot < 0 || slot >= RE15_AOT_MAX) return;
    g_aot.slots[slot].active = 0;
}

/* Re-type an existing AOT slot into an EXAMINE/MESSAGE AOT (byte-true Aot_reset sce=1):
 * keep the slot's rect (set earlier by Door_aot_set/Aot_set), switch its type so the
 * scan shows `msg_index` on action instead of teleporting. ROOM1130 sub01 calls
 * Aot_reset(3, sce=1, msg=1) every frame → door-3 (→1170) becomes a permanent
 * "It's not necessary to go back" examine that blocks the return. */
/* Cut_replace(a,b) — swap cut ids a<->b in the live CAM_SWITCH AOTs (cam_from_filter +
 * the target cut packed in event_id), to match the RVD-zone swap in op_cut_replace.
 * Byte-true companion to LAB_80040414 (the zone-table swap). */
void re15_aot_cut_replace(uint8_t a, uint8_t b)
{
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        re15_aot_t *aot = &g_aot.slots[i];
        if (!aot->active || aot->type != RE15_AOT_TYPE_CAM_SWITCH) continue;
        if      (aot->cam_from_filter == a) aot->cam_from_filter = b;
        else if (aot->cam_from_filter == b) aot->cam_from_filter = a;
        if      (aot->event_id == a) aot->event_id = b;
        else if (aot->event_id == b) aot->event_id = a;
    }
}

void re15_aot_set_message(int slot, uint8_t msg_index)
{
    if (slot < 0 || slot >= RE15_AOT_MAX) return;
    re15_aot_t *a = &g_aot.slots[slot];
    a->active     = 1;
    a->type       = RE15_AOT_TYPE_MESSAGE;
    a->event_id   = msg_index;          /* MESSAGE: event_id holds the .msg index */
    a->was_inside = 0;
}

/* Aot_reset sce>=2 (action EVENT AOT): byte-true LAB_80040738 RE-TYPES the slot WITHOUT
 * clearing it — re-assert it as a GENERIC action-event AOT, KEEPING its existing event_id
 * + rect from the prior Aot_set. ROOM1140 sub00 installs the painting examine (slot 5 =
 * GENERIC, event 3); its sub01 runs Aot_reset(5, sce=3) every frame, which must KEEP the
 * painting firing sub03 ("A beautiful painting … Will you push it?"). Our old op_aot_reset
 * cleared every sce!=1 slot → the painting was installed then instantly wiped → no text.
 * The original never clears: sce selects the new behaviour (0=inert SCE_AUTO/disabled,
 * 1=message, >=2=event), the rect is preserved. */
void re15_aot_reassert_event(int slot)
{
    if (slot < 0 || slot >= RE15_AOT_MAX) return;
    re15_aot_t *a = &g_aot.slots[slot];
    a->active     = 1;
    a->type       = RE15_AOT_TYPE_GENERIC;
    a->was_inside = 0;
    /* event_id + rect kept from the prior Aot_set. */
}

static int abs_i32(int32_t v) { return v < 0 ? -v : v; }

/* AQ-round 2026-05-26: PSX-canonical point-in-quad test (FUN_80014368).
 * RVD quads in RE1.5 are trapezoidal (non-axis-aligned). Each of the 4
 * edges defines a half-plane via cross-product against the player's
 * relative position. Player is inside iff on the "inside" side of all 4
 * edges.
 *
 * Edge winding follows CW or CCW convention from the RDT data. We compare
 * cross-product sign consistency across all 4 edges — sign convention is
 * determined dynamically from the first edge so we don't have to know
 * which winding RE1.5 used (verified non-zero for trapezoid; degenerate
 * quads return 0). */
/* BO-round (Tier-3): promoted from static to shared — also used by the prop/NPC
 * per-cut region-quad cull (PSX FUN_80014368 / FUN_8002c18c). */
int re15_aot_point_in_quad(int32_t px, int32_t pz,
                          const int16_t xs[4], const int16_t zs[4])
{
    int sign = 0;
    for (int i = 0; i < 4; i++) {
        int j = (i + 1) & 3;
        int32_t ex = (int32_t)xs[j] - (int32_t)xs[i];
        int32_t ez = (int32_t)zs[j] - (int32_t)zs[i];
        int32_t dx = px - (int32_t)xs[i];
        int32_t dz = pz - (int32_t)zs[i];
        int64_t cross = (int64_t)ex * (int64_t)dz - (int64_t)ez * (int64_t)dx;
        if (cross == 0) continue;   /* on edge — treat as inside */
        int s = (cross > 0) ? 1 : -1;
        if (sign == 0) sign = s;
        else if (sign != s) return 0;
    }
    return 1;
}

/* Phase 4.5.12: initialize edge-trigger state from a spawn position
 * without firing any AOT. Call after spawning the player and after
 * AOTs are registered, so doors the player materializes inside (e.g.
 * a door zone right at the room-entry pos) don't immediately trip on
 * the first frame. */
void re15_aot_settle_at(int32_t player_x, int32_t player_z)
{
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        int inside = (abs_i32(player_x - a->x) <= a->half_w) &&
                     (abs_i32(player_z - a->z) <= a->half_h);
        a->was_inside = (uint8_t)inside;
    }
}

void re15_aot_scan(int32_t player_x, int32_t player_z, uint8_t active_cut)
{
    /* Reset per-frame event flag at scan start. Main loop reads it
     * between scan() and the next frame, dispatching to SCD. */
    g_aot.fired_event_id_this_frame = 0;

    /* BYTE-TRUE (RE'd 2026-06-14): the original AOT scan FUN_80042bac resolves the ONE
     * zone the player overlaps and fires a SINGLE action handler per press — it does NOT
     * fire every in-range AOT. We must do the same, else two overlapping action AOTs (e.g.
     * a GENERIC switch/query + a MESSAGE examine) BOTH fire on one press and their messages
     * fight over the single dialog state (the examine clobbered the query → no panel text).
     * `action_fired` latches after the first ACTION-triggered AOT (DOOR/GENERIC/MESSAGE)
     * fires this frame, suppressing the rest. Passive inside-edge AOTs (CAM_SWITCH, auto
     * zones) are NOT gated by it. */
    int action_fired = 0;

    /* I3-round (2026-05-24): suppress NON-CAM_SWITCH AOT firing during
     * cinematic. Sub02 sets flag (1,27)=1 + (2,7)=1; while set, sub02 is
     * scripting Leon's position via Plc_dest + Member_set — letting door
     * AOTs fire teleports him to invalid spawn coords.
     *
     * AN-round (2026-05-26): RVD CAM_SWITCH zones MUST fire during the
     * cinematic per agent-confirmed PSX behavior: sub02 line 68 fires
     * Cut_auto(1) which enables RVD per-frame scanning. Original engine
     * auto-switches camera as Leon walks (Plc_dest line 82+ moves him
     * into RVD zone 9 → triggers Cut_chg(3) for the "Hey!" framing).
     * Without RVD firing, Leon stays in Cut 0 = wrong framing.
     *
     * So we keep the in_cinematic suppress for DOORS/ITEMS/GENERIC but
     * let CAM_SWITCH through. */
    /* Declaration comes from re15_scd.h (uint8_t parameters). The
     * earlier local `extern int(int,int)` shadowed it and produced a
     * conflicting-types compile error. */
    /* BL-round 2026-05-29: suppress non-CAM_SWITCH AOTs (doors/items) while the
     * player is SCRIPTED (player_mode==2) OR the letterbox is still closing
     * (countdown!=0). Re-enabled only when both clear (gameplay). Replaces the
     * BK cinematic_active latch / the raw flags (which sub02 clears one cut early). */
    int in_cinematic = (g_scd.player_mode == 2) || (g_scd.letterbox_countdown != 0);

    /* 2026-06-13: while a message/prompt is on screen, action-fired AOTs (the
     * switch / doors) must NOT fire. This scan runs BEFORE the SCD tick that
     * consumes the action press, so the confirm-press that dismisses a YES/NO
     * prompt would otherwise be seen here and RE-FIRE the same switch → a second
     * sub02 → the prompt + cutscene bars reappear and the text lingers (the
     * double-fire the user reported). Gate every action-fired AOT on it. */
    int msg_block = (g_scd.message_query != 0) || (g_scd.message_display_frames > 0);

    for (int i = 0; i < RE15_AOT_MAX; i++) {
        re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        /* STAIR zones are owned by stair_common.c (action-triggered band
         * transition) — never fire them as a generic event here. */
        if (a->type == RE15_AOT_TYPE_STAIR) continue;
        /* Suppress non-RVD AOTs during cinematic. */
        if (in_cinematic && a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;
        /* cam_from filter: RVD CAM_SWITCH zones only fire when the
         * active cut matches. Other types use 0xFF (always active). */
        if (a->cam_from_filter != 0xFF && a->cam_from_filter != active_cut) {
            a->was_inside = 0;   /* reset edge-state when zone goes dormant */
            continue;
        }

        /* AQ-round 2026-05-26: CAM_SWITCH zones with has_quad use PSX
         * point-in-quad (true trapezoid test). AABB-only zones (DOOR/
         * ITEM/GENERIC, or legacy CAM_SWITCH without corners) fall back
         * to rect test. */
        int inside;
        if (a->has_quad) {
            inside = re15_aot_point_in_quad(player_x, player_z, a->xs, a->zs);
        } else {
            inside = (abs_i32(player_x - a->x) <= a->half_w) &&
                     (abs_i32(player_z - a->z) <= a->half_h);
        }

        /* AN-round (2026-05-26): CAM_SWITCH AOTs fire on LEVEL not EDGE.
         * Per agent RE of RE1.5 FUN_80014230: the per-frame trigger_check
         * iterates RVD zones and calls raum_kamera_wechseln() whenever
         * player is INSIDE a matching zone — no edge gating. (DOOR/ITEM/
         * GENERIC AOTs still use edge detection to fire only on entry.)
         * Without this, initial settle_at sets was_inside=1 and zones
         * matching the initial active_cut never fire even though player
         * is inside them. */
        if (a->type == RE15_AOT_TYPE_CAM_SWITCH) {
            /* BO-round 2026-05-29 (Tier-2 #12): gate the RVD/CAM_SWITCH scan on
             * cut_auto_enabled — PSX skips FUN_80014230 entirely when bit 0x100
             * of DAT_800aca3c is SET (main loop @0x8001cce0). Zero-init → scan
             * DISABLED at room entry; Cut_auto(1) (0x3C, mode==1) CLEARS the bit
             * to enable it for the gameplay walking sequence. Cinematic cuts are
             * issued via Cut_chg (not this scan), so gating here does NOT affect
             * the scripted intro cuts; it only stops spurious room-entry switches
             * before walking begins.
             *
             * BP-round 2026-05-29: a BP revert of this gate was a MISTAKE — the
             * apparent "intro frozen on cut0" was a TEST ARTIFACT (the exe was
             * launched from the wrong cwd so test.rdt wasn't found → demo
             * bytecode → empty pad), NOT this gate. From the correct cwd the
             * intro plays end-to-end with the gate in place. Gate RESTORED. */
            if (inside && g_scd.cut_auto_enabled) {
                g_scd.cam_id              = (uint8_t)a->event_id;
                g_scd.cam_change_pending  = 1;
                a->was_inside             = 1;
                return;  /* first match wins */
            }
            a->was_inside = 0;
            continue;  /* CAM_SWITCH handled, skip default edge logic */
        }

        /* DOOR trigger = the faithful FUN_8002d1e8 forward-reach (2026-06-07,
         * savestate-verified): a point 563 units IN FRONT of the player (his
         * facing direction) must lie within ±900 of the door CENTRE. This is the
         * action-reach — the player presses action while FACING the door from the
         * wall in front of it; he does NOT have to stand inside the door rect (a
         * door is often recessed behind a wall the collision holds him 468 out of,
         * so its rect is unreachable). The original (ROOM1170 outdoor savestate)
         * fires door6 from the player at (-11675,-26666) facing south: forward pt
         * (-11755,-27223) is within ±900 of door6 centre (-11065,-27850). Our old
         * "player centre in rect" was too strict → the return door never fired.
         * Mesh faces +X at rot_y=0 so forward = (cos, -sin) (re15 walker convention). */
        int door_inside = inside;
        if (a->type == RE15_AOT_TYPE_DOOR) {
            int ry = (int)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y;
            int32_t c = re15_cos_q12(ry), s = re15_sin_q12(ry);
            int32_t fx = player_x + (int32_t)((563 * c) >> 12);
            int32_t fz = player_z - (int32_t)((563 * s) >> 12);
            door_inside = (abs_i32(fx - a->x) <= 900) && (abs_i32(fz - a->z) <= 900);
            /* BAND GATE (byte-true FUN_8002bd44 @0x8002bf38-bf44): the original tests
             * player_band (DAT_800acad6) == door object band (obj+0x82) BEFORE the
             * forward-reach FUN_8002d1e8; mismatched bands skip the door entirely. The
             * door band = Door_aot_set pc[4] (door_params.band): ROOM1170 door0/1/6=band4
             * (courtyard), door4/5=band0 (pit). This is what keeps the floors separate —
             * a band-4 courtyard player can NOT walk into a band-0 pit door (its rect is
             * reachable in XZ because band-0 walls don't block a band-4 player, but the
             * band gate stops the trigger). You must descend the stair onto band 0 first.
             * Only gate when a collision band is established (>=0); a -1 (unknown) band
             * room (no SCA / pre-band) keeps the old ungated behaviour. */
            int pb = re15_collision_debug_band();
            if (pb >= 0 && (int)g_aot.door_params[i].band != pb)
                door_inside = 0;
        }
        /* GENERIC event AOT (e.g. the ROOM1130 roller-door SWITCH "tableau") fires on an
         * ACTION press. Like a door, a wall-mounted switch is approached from the FRONT —
         * the player stands before it, so his CENTRE is usually NOT inside the small rect.
         * So accept EITHER centre-in-rect OR the door-style forward-reach (a point 563
         * ahead of his facing within ±900 of the AOT centre). (Globalization 2026-06-13:
         * GENERIC = action-triggered event-runner → scd_event_fire(event_id) →
         * sub_scd[event_id]; the switch's sub Aot_resets itself so it won't re-trigger.) */
        int gen_reach = inside;
        if ((a->type == RE15_AOT_TYPE_GENERIC || a->type == RE15_AOT_TYPE_MESSAGE ||
             a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR) && !gen_reach) {
            int ry = (int)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y;
            int32_t c = re15_cos_q12(ry), s = re15_sin_q12(ry);
            int32_t fx = player_x + (int32_t)((563 * c) >> 12);
            int32_t fz = player_z - (int32_t)((563 * s) >> 12);
            gen_reach = (abs_i32(fx - a->x) <= 900) && (abs_i32(fz - a->z) <= 900);
        }
        int is_action = (a->type == RE15_AOT_TYPE_DOOR ||
                         a->type == RE15_AOT_TYPE_GENERIC ||
                         a->type == RE15_AOT_TYPE_MESSAGE ||
                         a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR);
        int fire = (a->type == RE15_AOT_TYPE_DOOR)
                       ? (door_inside && g_aot_action_pressed && !msg_block && !action_fired)
                 : (a->type == RE15_AOT_TYPE_GENERIC || a->type == RE15_AOT_TYPE_MESSAGE ||
                    a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR)
                       ? (gen_reach && g_aot_action_pressed && !msg_block && !action_fired)
                       : (inside && !a->was_inside);
        if (fire) {
            if (is_action) action_fired = 1;   /* one action handler per press (byte-true) */
            /* Edge: just entered. (2026-06-03: removed the invented 90-frame
             * HUD echo — last_event_* had no readers in either build.) */
            switch (a->type) {
            case RE15_AOT_TYPE_DOOR: {
                /* Built-in DOOR behavior: target cut + spawn pos +
                 * door SFX. Mirrors RE2's FUN_8003a9f4 door-pass code:
                 * Cut_chg + Pos_set + door-open audio queued. */
                const re15_aot_door_params_t *d = &g_aot.door_params[i];
#ifndef RE15_PLATFORM_PC
                printf("[AOT] DOOR slot=%d destroom=%u cut=%u spawn=(%d,%d) P=(%d,%d)\n",
                       i, d->dest_room, d->target_cut, d->spawn_x, d->spawn_z,
                       player_x, player_z);
#endif
                /* I3-round guard (2026-05-24): skip if spawn is (0,0,0)
                 * — invalid door, would teleport Leon into the floor.
                 * Was causing Leon's Y to become 0 mid-cinematic. */
                if (d->spawn_x == 0 && d->spawn_y == 0 && d->spawn_z == 0) {
#ifdef RE15_PLATFORM_PC
                    fprintf(stderr, "[aot] DOOR slot=%d SKIPPED (spawn=0,0,0 - invalid)\n", i);
#endif
                    a->was_inside = inside;
                    break;
                }
#ifdef RE15_PLATFORM_PC
                fprintf(stderr, "[aot] DOOR FIRE slot=%d rect=(%d,%d,hw=%d,hh=%d) target_cut=%u spawn=(%d,%d,%d) player=(%d,%d)\n",
                        i, a->x, a->z, a->half_w, a->half_h,
                        d->target_cut, d->spawn_x, d->spawn_y, d->spawn_z,
                        player_x, player_z);
#endif
                /* Cross-room door (multi-room): if the destination resolves to a
                 * DIFFERENT room, queue a room load instead of an in-room cut+pos
                 * teleport. BYTE-TRUE dest RDT id (globalization 2026-06-13):
                 *   dest_id = ((dest_stage+1) << 12) | (dest_room << 4) | variant
                 * dest_stage (Door_aot_set pc[22]) is 0-BASED (0=STAGE1) — the old
                 * `0x1000|(room<<4)` HARDCODED stage 1 and mis-loaded every CROSS-STAGE
                 * door (e.g. ROOM5130 door1 dest_stage=5 -> ROOM6020, not ROOM1020).
                 * The variant nibble (scenario/player, low bit of the room id) is
                 * carried over from the current room so we stay in the same scenario.
                 * VALIDATED: this formula resolves 563/567 cross-room doors game-wide to
                 * existing rooms (scripts/door_graph.py; the 4 misses are non-door scan
                 * artifacts). dest_room 0 -> self/same-room teleport (falls through).
                 * The actual load runs after the scan (re15_room_apply_pending). SHARED
                 * (PC links room_common.c + a file RDT loader → PC doors work too). */
                if (d->dest_room != 0) {
                    unsigned dest_id = (((unsigned)d->dest_stage + 1u) << 12)
                                     | ((unsigned)d->dest_room << 4)
                                     | (g_current_room_id & 0x000Fu);
                    if (dest_id != g_current_room_id) {
                        re15_room_request_change(dest_id, d->spawn_x, d->spawn_y,
                                                 d->spawn_z, d->spawn_yaw_4096,
                                                 (int)d->target_cut);
                        a->was_inside = 1;
                        return;
                    }
                }
                g_scd.cam_id              = d->target_cut;
                g_scd.cam_change_pending  = 1;
                /* Phase 4.5.9-D: player is g_actors[0]. */
                g_actors[RE15_ACTOR_SLOT_PLAYER].x     = d->spawn_x;
                g_actors[RE15_ACTOR_SLOT_PLAYER].y     = d->spawn_y;
                g_actors[RE15_ACTOR_SLOT_PLAYER].z     = d->spawn_z;
                g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = d->spawn_yaw_4096;
                /* Floor band for the destination from the door spawn Y
                 * (band = -(Y/0x708)). NOTE (2026-06-07): for ROOM1170 every spawn
                 * Y=-7200 -> band 4, so a same-room door keeps band 4. door6 (the
                 * outdoor return) sits inside a band-4 SCA cell which push-out treats
                 * as a wall, so door6 is currently NOT reachable on foot — see
                 * re_object_collision_and_door_return: the outdoor band needs the
                 * same-room re-stamp the original does (open, needs an outdoor
                 * savestate to pin). */
                re15_collision_set_band(re15_collision_band_from_y(d->spawn_y));
                /* SELF-room door (dest == current room): re-enter ROOM1170 in the door's
                 * entry SCENARIO (= target_cut). This re-runs main00+sub00 (memset clears
                 * the actor/prop pool → Elliot + heli despawn) and sub00 dispatches sub15
                 * on work_vars[10]=scenario. Byte-true (2026-06-09 RE, generalized from the
                 * old door-0-only special case):
                 *   door 0 (target_cut 11) → sub00 case 11 → sub14 (courtyard dialog);
                 *   door 6 (target_cut 0, courtyard→helipad return) → sub00 case 0 → sub15,
                 *     whose ELSE branch ((4,242) cleared by sub02, (3,125) still set) spawns
                 *     the 7 type-0x21 crows on the now-empty helipad. */
                if (d->dest_room != 0 &&
                    (0x1000u | ((unsigned)d->dest_room << 4)) == g_current_room_id)
                    g_scd_pending_scenario = (int)d->target_cut;
                /* BO-round 2026-05-29 (hack audit): removed the fabricated door
                 * SFX {bank2,sample2,vol0x60,pan0x40}. PSX door sound is NOT a
                 * fixed value — it comes from the room's own SCD Se_on, fired by
                 * the door's event sub (verified: room1040/sub02 Se_on(2,12)/
                 * (2,10); DOOR0001 Se_on(0,0)/(0,1) — all room-specific). Our
                 * door is C-driven (simplified), so the canonical SCD-sub-driven
                 * SFX is a follow-up (wire AOT event_sub_id → fire the sub →
                 * its Se_on). No fabricated value is correct here. */
                /* Phase 4.5.12: door teleport invalidates remaining AOT
                 * checks this frame AND any door zones the player just
                 * landed in — the player just jumped to a new location
                 * and shouldn't immediately trip the next door whose
                 * rect happens to contain that destination. Mark this
                 * door (we just entered it) AND every other door whose
                 * rect contains the new position as was_inside, so they
                 * only re-fire if the player walks out and back in. */
                a->was_inside = 1;
                for (int j = 0; j < RE15_AOT_MAX; j++) {
                    if (j == i) continue;
                    re15_aot_t *b = &g_aot.slots[j];
                    if (!b->active) continue;
                    if (b->type != RE15_AOT_TYPE_DOOR) continue;
                    int b_inside = (abs_i32(d->spawn_x - b->x) <= b->half_w) &&
                                   (abs_i32(d->spawn_z - b->z) <= b->half_h);
                    if (b_inside) b->was_inside = 1;
                }
                return;
            }
            case RE15_AOT_TYPE_ITEM: {
                /* Built-in ITEM behavior: grant + pickup SFX + one-shot
                 * deactivate (RE2 Item_aot_reset equivalent). */
                const re15_aot_item_params_t *p = &g_aot.item_params[i];
                re15_inv_grant(p->item_type, p->amount);
                /* BO-round 2026-05-29 (hack audit): removed the fabricated item-
                 * pickup SFX {bank2,sample2,vol0x50,pan0x40} — same as the door:
                 * the real pickup sound is the room SCD's own Se_on (room1190/
                 * sub02 Se_on(2,10) etc.), not a hardcoded value. Canonical SFX
                 * is the SCD-sub follow-up. */
                a->active = 0;
                break;
            }
            case RE15_AOT_TYPE_CAM_SWITCH: {
                /* RVD camera transit: only swap the active cut. No SFX,
                 * no teleport. target_cut packed in event_id field. */
                g_scd.cam_id              = a->event_id;
                g_scd.cam_change_pending  = 1;
                break;
            }
            case RE15_AOT_TYPE_MESSAGE: {
                /* EXAMINE/MESSAGE AOT (sce=1): show the .msg index (event_id) and do
                 * NOT change rooms — the byte-true ROOM1130 back-door "It's not
                 * necessary to go back" (msg index 1). msg_block then suppresses
                 * re-fire until the message clears. */
                extern void re15_scd_show_message(uint8_t index);
                re15_scd_show_message(a->event_id);
                break;
            }
            case RE15_AOT_TYPE_EXAMINE_WORKVAR:
                /* Action-examine of an ev==0 AOT: write this AOT's index (stored in event_id =
                 * its slot) into work_vars[0] + flag a one-shot sub01 re-poll (scd_vm_tick).
                 * Fires NO sub directly. ROOM1150 slot-2 → work_vars[0]=2 → sub01 → sub03. */
                g_scd.work_vars[0] = (int16_t)a->event_id;
                g_scd.examine_poll_pending = 1;
                break;
            default:
                /* GENERIC: fire bytecode event handler (Phase 4.4.6.1). */
                g_aot.fired_event_id_this_frame = a->event_id;
                break;
            }
        }
        a->was_inside = (uint8_t)inside;
    }
}
