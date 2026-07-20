/*
 * RE1.5 Rebuilt — AOT runtime (Phase 4.4.6, 2026-05-19).
 *
 * Target-agnostic. Per-frame point-in-rect over up to RE15_AOT_MAX
 * trigger zones, with edge-trigger semantics so each entry fires
 * exactly once even when the player loiters inside.
 *
 * RE2 has rect AND circle AOT shapes (per FUN_80051088.c flag bit 0x80).
 * We implement rect AND the 4-vertex quad (has_quad / re15_aot_point_in_quad, L188) — covers all of Aot_set / Door_aot_set
 * / Item_aot_set in the extracted RE1.5 scripts. Circle can be added
 * when something actually uses it.
 */

#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif
#include "re15_aot.h"
#include "re15_scd.h"        /* g_game (player pos), g_scd (cam, audio queue) */
#include "re15_savepoint.h"  /* latch the gameplay cut at a save-phone examine */
#include "re15_inventory.h"
#include "re15_audio.h"      /* audio event kinds for door/pickup SFX */
#include "re15_actor.h"      /* Phase 4.5.9-D: player = g_actors[0] */
#include "re15_skeleton.h"   /* re15_sin_q12/cos_q12 — door forward-reach trigger */
#include "re15_room_list.h"   /* re15_room_ids[] — Intro-Auto-Tür: Ziel-Raum-Index */
#include "re15_room_spawns.h" /* re15_room_spawns[] — korrekter Eintritts-Spawn des Intro-Zielraums */
#include "re15_collision.h"  /* set the floor band at a same-room door (band from spawn Y) — shared (PSX + PC) */
#include "re15_room.h"       /* g_current_room_id + cross-room transition request.
                              * SHARED now (room_common.c) — both ports queue the
                              * cross-room change; only the RDT LOADER (CD vs file)
                              * differs, behind the re15_room_apply_pending ctx. */
#include "re15_item_modal.h" /* item pickup PRESENTATION modal (FUN_8001db28) — deferred grant */

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
    a->sce_flags       = 0;      /* installers with real record flags override (op_aot_set pc[3]) */
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
    return re15_aot_set_item_tk(slot, cx, cz, half_w, half_h, item_type, amount, 0);
}

int re15_aot_set_item_tk(int slot, int32_t cx, int32_t cz,
                         int32_t half_w, int32_t half_h,
                         uint8_t item_type, uint8_t amount, uint8_t taken_bit)
{
    int rc = re15_aot_set(slot, RE15_AOT_TYPE_ITEM, 0, cx, cz, half_w, half_h);
    if (rc != 0) return rc;
    re15_aot_item_params_t *p = &g_aot.item_params[slot];
    p->item_type = item_type;
    p->amount    = amount;
    p->taken_bit = taken_bit;
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

    /* Auto-Advance-Tür-Trigger (ROOM1240-Intro → ROOM1170): das Intro-Skript ist
     * FERTIG, wenn KEIN SCD-Thread mehr aktiv ist. Schlafende (Sleep) Cinematic-
     * Threads bleiben active==1, daher wird scd_idle erst wahr, wenn der Cinematic-
     * Thread sein Evt_end erreicht — nicht mid-Intro. tick_count-Gate verhindert das
     * Frame-0-Rennen, bevor die Room-Setup-Threads überhaupt gespawnt sind. */
    int scd_idle = 1;
    for (int ti = 0; ti < SCD_THREAD_COUNT; ti++)
        if (g_scd.threads[ti].active) { scd_idle = 0; break; }
    int scd_ran = (g_scd.tick_count > 90);

    /* RVD CAM_SWITCH pick (byte-true FUN_80014230): among the RVD zones the player is inside for the
     * current cam_from, the ORIGINAL selects the FIRST in RVD TABLE order. The port installs RVD zones
     * top-down (install#0 = lowest table index -> slot 63) and scans slots ASCENDING, so "first inside
     * wins" picked the LOWEST slot = the LAST table zone = wrong cut (e.g. ROOM1190 dog cam_from=3:
     * zone[14]->cut5 vs zone[15]->cut4 overlap ~49 pts; PSX shows cut5, the port showed cut4 — and since
     * cam_to becomes next frame's cam_from, the whole trajectory could cascade). Fix: don't return on the
     * first inside CAM_SWITCH; record it and keep scanning. Because the RVD region occupies the TOP slots
     * (scanned last, after every SCD AOT) the LAST inside match = highest slot = lowest table index =
     * the byte-true FIRST zone, and no SCD AOT is scanned after it so precedence is unchanged. */
    int best_cam_id = -1;

    for (int i = 0; i < RE15_AOT_MAX; i++) {
        re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        /* STAIR zones are owned by stair_common.c (action-triggered band
         * transition) — never fire them as a generic event here. */
        if (a->type == RE15_AOT_TYPE_STAIR) continue;
        /* INTRO-HANDOFF / AUTO-ADVANCE-TÜR: ein cross-room DOOR-AOT mit
         * degeneriertem (0×0) Rechteck am Player-Spawn ist eine Auto-Advance-Tür
         * (z.B. ROOM1240-Pre-Intro → ROOM1170). Das Original gated Türen NICHT an
         * den Script-Cinematic-Flags (FUN_8002bd44 @0x8002bf38 liest (1,27)/(2,7)
         * nicht); diese Tür wird erst per Aot_on am Intro-ENDE scharf, der Spieler
         * steht exakt im Punkt-Trigger. Sie darf daher auch während des (vom Intro
         * absichtlich nie geklärten) Cinematic feuern und braucht KEINEN Action-Press.
         * Normale Lauf-Türen (echtes Rechteck) bleiben Cinematic-unterdrückt + Action-
         * gated. */
        int is_auto_door = 0;
        if (a->type == RE15_AOT_TYPE_DOOR && a->half_w == 0 && a->half_h == 0) {
            /* Ziel-Raum-ID byte-true (wie im Door-Fire-Handler): dest muss ein
             * ANDERER Raum sein — ein Null-Rechteck-„Tür", deren dest zum aktuellen
             * Raum auflöst, ist ein Same-Room-Teleport (z.B. ROOM1170 slot3) und darf
             * NICHT als Auto-Advance ausgelöst werden. Raum-Index 0 ist ein GÜLTIGES
             * Ziel (ROOM_x00; FUN_8001d600 liest struct+9 ohne ==0-Sonderfall) — der
             * alte `dest_room != 0`-Vorfilter warf ROOM1000-Ziele weg.
             * [audit wf_559c230f DOOR-DESTROOM-ZERO] */
            unsigned dd = (((unsigned)g_aot.door_params[i].dest_stage + 1u) << 12)
                        | ((unsigned)g_aot.door_params[i].dest_room << 4)
                        | (g_current_room_id & 0x000Fu);
            is_auto_door = (dd != g_current_room_id);
        }
        /* Suppress non-RVD AOTs during cinematic (außer der Auto-Advance-Tür). */
        if (in_cinematic && a->type != RE15_AOT_TYPE_CAM_SWITCH && !is_auto_door) continue;
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
            /* FLOOR gate: the RVD zone's floor byte (entry+1, stored in a->band) must match the
             * player's collision band, unless it is 0xFF (any floor). Only gated when a band is
             * established (pb >= 0), mirroring the DOOR/GENERIC band gates. This byte is 0xFF across
             * all STAGE1 rooms today, so the gate is currently inert there — it's the mechanism the
             * original applies (a stacked-floor room would otherwise switch cut on the wrong floor). */
            int cam_pb = re15_collision_debug_band();
            int floor_ok = (a->band == 0xFF) || (cam_pb < 0) || ((int)a->band == cam_pb);
            if (inside && g_scd.cut_auto_enabled && floor_ok) {
                best_cam_id   = (int)a->event_id;   /* keep the LAST (highest-slot = lowest table index) */
                a->was_inside = 1;
                continue;    /* do NOT return — a later (higher-slot / lower table index) zone wins */
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
            if (pb >= 0 && (int)g_aot.door_params[i].band != pb && !is_auto_door)
                door_inside = 0;
        }
        /* GENERIC event AOT (e.g. the ROOM1130 roller-door SWITCH "tableau") fires on an
         * ACTION press. Like a door, a wall-mounted switch is approached from the FRONT —
         * the player stands before it, so his CENTRE is usually NOT inside the small rect.
         * So accept EITHER centre-in-rect OR the door-style forward-reach (a point 563
         * ahead of his facing within ±900 of the AOT centre). (Globalization 2026-06-13:
         * GENERIC = action-triggered event-runner → scd_event_fire(event_id) →
         * sub_scd[event_id]; the switch's sub Aot_resets itself so it won't re-trigger.) */
        /* BYTE-TRUE ACTION GEOMETRY (wf_f536e1ee step 2, FUN_80042bac): per the record flag
         * byte — CENTRE test (bit 0x40, @0x80042ea8) tried FIRST, then the FORWARD point =
         * entity pos + rotate((620,0)) by yaw (ori 0x26c @0x80042bd0, FUN_8004f008) tested
         * against the AOT's EXACT rect (FUN_80042b64) or quad (bit 0x20, @0x80042ef8) — NOT
         * the old ±900-around-centre transplant. sce_flags==0 (legacy/synthetic installs)
         * keeps both tests. The hit KIND is tracked for the byte-true work_var stamps
         * (centre-hit -> work_var[1], forward-hit -> work_var[0]). */
        int gen_reach = 0, gen_fwd_hit = 0;
        if (a->type == RE15_AOT_TYPE_GENERIC || a->type == RE15_AOT_TYPE_MESSAGE ||
            a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR || a->type == RE15_AOT_TYPE_ITEM ||
            (a->type == RE15_AOT_TYPE_FLAG_CHG && (a->sce_flags & 0x10))) {
            uint8_t fl = a->sce_flags;
            if ((fl == 0 || (fl & 0x40)) && inside) gen_reach = 1;      /* centre first */
            if (!gen_reach && (fl == 0 || (fl & 0x20))) {               /* forward 620 exact */
                int ry = (int)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y;
                int32_t c = re15_cos_q12(ry), s = re15_sin_q12(ry);
                int32_t fx = player_x + (int32_t)((620 * c) >> 12);
                int32_t fz = player_z - (int32_t)((620 * s) >> 12);
                int hit = a->has_quad
                        ? re15_aot_point_in_quad(fx, fz, a->xs, a->zs)
                        : ((abs_i32(fx - a->x) <= a->half_w) && (abs_i32(fz - a->z) <= a->half_h));
                if (hit) { gen_reach = 1; gen_fwd_hit = 1; }
            }
        }
        /* #14(c) GENERIC band-gate (byte-true FUN_80042cac @0x80042cac): the scan tests the AOT's band
         * byte BEFORE the geometry — a wrong-FLOOR event AOT does not fire. (band & 0x80) = ignore-band;
         * otherwise require player_band == band. Only gate when a collision band is established (pb >= 0),
         * exactly like the DOOR gate above (a pre-band / -1 room keeps the ungated behaviour). Doors use
         * their own door_params band; stair/cam are handled/skipped earlier — so this covers the event
         * types (GENERIC/AUTO_EVENT/MESSAGE/EXAMINE), whose band is now populated from Aot_set pc[4]. */
        if (a->type == RE15_AOT_TYPE_GENERIC || a->type == RE15_AOT_TYPE_AUTO_EVENT ||
            a->type == RE15_AOT_TYPE_MESSAGE || a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR ||
            a->type == RE15_AOT_TYPE_FLAG_CHG) {
            int pb = re15_collision_debug_band();
            if (pb >= 0 && !(a->band & 0x80) && (int)a->band != pb) {
                gen_reach = 0; inside = 0;   /* band mismatch -> gate the fire this frame */
            }
        }
        /* sce=4 FLAG_CHG (wf_f536e1ee step 3+4): the 3-POOL test per the record's pool-mask bits
         * (byte-true FUN_800436a8: player mask 1 / every active enemy mask 2 / every object mask
         * 4). 10 shipped zones are enemy-ONLY (0x42, e.g. the ROOM1030 zombie-window flags) — the
         * player walking through must NOT set them; a zombie inside must. Auto variants fire
         * every frame (idempotent flag write); action variants (0x31) go through gen_reach. */
        /* sce=7/8 ENV zones (WATER/RAMP — wf_f536e1ee step 4): per-ENTITY application during the
         * pool walk (the original stamps each pool entity inside, every frame). No fire dispatch. */
        if (a->type == RE15_AOT_TYPE_WATER || a->type == RE15_AOT_TYPE_RAMP) {
            uint8_t pm = (uint8_t)(a->sce_flags & 0x07);
            if (pm == 0) pm = 0x07;                              /* shipped water = 0x47 all pools */
            for (int es = 0; es < RE15_ACTOR_MAX; es++) {
                if (!g_actors[es].active) continue;
                if (es == RE15_ACTOR_SLOT_PLAYER) { if (!(pm & 0x01)) continue; }
                else                              { if (!(pm & 0x02)) continue; }
                /* PER-ENTITY band gate (byte-true FUN_80042bac @0x80042cac-ccc, runs for EVERY sce AOT
                 * BEFORE geometry): unless the AOT band ignores it (bit 0x80), require this entity's own
                 * floor band (+0x82) == the AOT band. An actor inside the rect but on a mismatched floor
                 * must NOT get water_y/ramp-Y stamped. (The GENERIC gate above uses the PLAYER band; here
                 * each pooled entity is gated by its OWN band.) */
                if (!(a->band & 0x80) && g_actors[es].floor != a->band) continue;
                int32_t ex = g_actors[es].x, ez = g_actors[es].z;
                int in2 = a->has_quad
                        ? re15_aot_point_in_quad(ex, ez, a->xs, a->zs)
                        : ((abs_i32(ex - a->x) <= a->half_w) && (abs_i32(ez - a->z) <= a->half_h));
                if (!in2) continue;
                if (a->type == RE15_AOT_TYPE_WATER) {
                    g_actors[es].water_y = g_aot.env_params[i].p0;   /* +0x88 stamp (LAB_8004330c) */
                } else {
                    const re15_aot_env_params_t *ep = &g_aot.env_params[i];
                    if (ep->p1 != 0) {
                        int32_t dist;
                        switch (ep->p0 & 3) {                        /* reference edge (LAB_800431cc) */
                            case 0:  dist = ex - (a->x - a->half_w); break;   /* x1     */
                            case 1:  dist = (a->z + a->half_h) - ez; break;   /* z1 + h */
                            case 2:  dist = (a->x + a->half_w) - ex; break;   /* x1 + w */
                            default: dist = ez - (a->z - a->half_h); break;   /* z1     */
                        }
                        g_actors[es].y = (int32_t)((dist / ep->p1) + 1) * ep->p2;   /* -> +0x38 */
                    }
                }
            }
            /* DEFERRED (audit wf_1c0e60d8 #5, LOW): the original FUN_800436a8 runs FUN_80042bac a THIRD
             * time over the OBJECT/prop pool (mask 0x04), so a prop inside a water/ramp rect is stamped
             * too. Not ported because it is a no-op / negligible here: (a) WATER stamps prop+0x88, but the
             * port's prop struct has no water_y and NOTHING consumes a prop water level (only actor render
             * reads +0x88); (b) RAMP would set prop.y, but props rarely overlap a ramp trigger (prop Y is
             * SCD-set) AND the byte-true per-prop band gate needs the prop's +0x82 floor band, which the
             * port's prop struct does not carry. A full port would add both fields + a prop-pool pass here. */
            continue;                                             /* env zones: no fire dispatch */
        }
        int flag_pool_inside = 0;
        if (a->type == RE15_AOT_TYPE_FLAG_CHG && !(a->sce_flags & 0x10)) {
            uint8_t pm = (uint8_t)(a->sce_flags & 0x07);
            if (pm == 0) pm = 0x01;                                 /* legacy: player */
            if ((pm & 0x01) && inside) flag_pool_inside = 1;
            if (!flag_pool_inside && (pm & 0x02)) {                 /* any active enemy */
                for (int es = RE15_ACTOR_SLOT_PLAYER + 1; es < RE15_ACTOR_MAX && !flag_pool_inside; es++) {
                    if (!g_actors[es].active) continue;
                    int in2 = a->has_quad
                            ? re15_aot_point_in_quad(g_actors[es].x, g_actors[es].z, a->xs, a->zs)
                            : ((abs_i32(g_actors[es].x - a->x) <= a->half_w) &&
                               (abs_i32(g_actors[es].z - a->z) <= a->half_h));
                    if (in2) flag_pool_inside = 1;
                }
            }
            if (!flag_pool_inside && (pm & 0x04)) {                 /* any active prop object */
                for (int ps = 0; ps < (int)g_scd.prop_count && !flag_pool_inside; ps++) {
                    if (!g_scd.props[ps].active) continue;
                    int in3 = (abs_i32(g_scd.props[ps].x - a->x) <= a->half_w) &&
                              (abs_i32(g_scd.props[ps].z - a->z) <= a->half_h);
                    if (in3) flag_pool_inside = 1;
                }
            }
        }
        /* DOOR 9-frame press-and-HOLD accumulator (byte-true FUN_8002bd44, obj+0x8C @0x8002bf60):
         * the original opens a door only after the action button is HELD for 9 consecutive frames
         * while the forward-reach + band both hold — NOT on a single tap-edge (which is what the port
         * fired on). The counter resets to 0 the instant any condition fails. (The blocked-path
         * variant that latches the counter at 10 via FUN_8003b558 is a faithful-line deferral — the
         * port's collision already keeps the player out of a physically blocked doorway.) */
        if (a->type == RE15_AOT_TYPE_DOOR) {
            extern uint8_t g_scd_action_held;
            if (door_inside && g_scd_action_held && !msg_block) {
                if (a->door_hold < 9) a->door_hold++;
            } else {
                a->door_hold = 0;
            }
        }
        int is_action = (a->type == RE15_AOT_TYPE_DOOR ||
                         a->type == RE15_AOT_TYPE_GENERIC ||
                         a->type == RE15_AOT_TYPE_MESSAGE ||
                         a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR ||
                         a->type == RE15_AOT_TYPE_ITEM ||
                         (a->type == RE15_AOT_TYPE_FLAG_CHG && (a->sce_flags & 0x10)));
        int fire = is_auto_door
                       /* Auto-Advance-Tür: das Rechteck ist ein (0,0)-Sentinel (nie
                        * positions-erreichbar) → KEIN Forward-Reach/Action-Press. Feuert,
                        * sobald das Intro-Skript fertig ist (alle SCD-Threads idle).
                        * NUR während in_cinematic: das Intro lässt die Cinematic-Flags
                        * (1,27)/(2,7) absichtlich gesetzt (Hand-off mit aktivem Cinematic).
                        * Normale Räume klären sie via Plc_ret → in_cinematic=false → diese
                        * Tür wird dort NICHT spurious ausgelöst. */
                       ? (in_cinematic && scd_idle && scd_ran && !msg_block && !action_fired)
                 : (a->type == RE15_AOT_TYPE_DOOR)
                       ? (a->door_hold == 9 && !msg_block && !action_fired)   /* opens on the 9th held frame */
                 : (a->type == RE15_AOT_TYPE_ITEM)
                       /* ITEM = ACTION-gated (wf_f536e1ee #5: all 162 shipped items carry flags
                        * bit 0x10 SET — the sce9 handler runs from the press-edge scan only;
                        * the old walk-in level trigger hoovered items silently). */
                       ? (gen_reach && g_aot_action_pressed && !msg_block && !action_fired)
                 : (a->type == RE15_AOT_TYPE_FLAG_CHG)
                       ? ((a->sce_flags & 0x10)
                              ? (gen_reach && g_aot_action_pressed && !msg_block && !action_fired)
                              : flag_pool_inside)                     /* auto: every frame, idempotent */
                 : (a->type == RE15_AOT_TYPE_GENERIC || a->type == RE15_AOT_TYPE_MESSAGE ||
                    a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR)
                       ? (gen_reach && g_aot_action_pressed && !msg_block && !action_fired)
                       /* #14 byte-true (FUN_80042bac @0x80043018): the AUTO zone has NO prev-frame-inside
                        * field — it fires EVERY frame the entity is inside, not just on the entry EDGE.
                        * Re-trigger is stopped HANDLER-SIDE:
                        *   ITEM (2): the ITEM case self-disables synchronously in this scan pass (a->active=0).
                        *   AUTO_EVENT (6): the SCD sub's own Aot_reset(slot,0) disables the zone. The order is
                        *     scd_tick -> aot_scan -> scd_event_fire, and the started sub first ticks NEXT frame
                        *     BEFORE that frame's scan; the byte-true AUTO subs make Aot_reset their FIRST
                        *     (yield-free) opcode, so the zone is dead before the 2nd scan = exactly one fire.
                        *     VERIFIED: ROOM1150 sub08 (Irons cutscene) bytecode opens `46 06 00...` =
                        *     Aot_reset(6,0) as opcode 0. (The original has no edge either, so any AUTO sub that
                        *     did NOT self-disable first would multi-fire in the original too -> all working AUTO
                        *     subs self-disable first.) The old `!was_inside` edge was mechanism-divergent. */
                       : inside;
        if (fire) {
            if (is_action) action_fired = 1;   /* one action handler per press (byte-true) */
            /* WORK_VAR stamps (byte-true FUN_80042bac: centre-hit @0x80042ea8 -> DAT_800b0fd2 =
             * work_var[1], forward-hit @0x80042ef8 -> DAT_800b0fd0 = work_var[0], value = slot;
             * consumed by the 0x58/0x59 indexed flag ops — catalog #19). Doors keep their own
             * (object-probe) path; the examine handler's work_vars[0] write below stays. */
            if (a->type == RE15_AOT_TYPE_GENERIC || a->type == RE15_AOT_TYPE_MESSAGE ||
                a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR) {
                g_scd.work_vars[gen_fwd_hit ? 0 : 1] = (int16_t)i;
                /* Latch the GAMEPLAY cut NOW — before this examine's SCD sub (fired below)
                 * can Cut_chg to an interaction close-up (the phone/computer save objects do
                 * Cut_chg(N)+Message_on). If this examine turns out to be a save-point, the
                 * save block stores THIS cut, so a load restores the gameplay framing, not the
                 * transient close-up. Harmless for non-save examines (only read on save). */
                re15_savepoint_set_cut((int)g_scd.cam_id);
            }
            /* Edge: just entered. (2026-06-03: removed the invented 90-frame
             * HUD echo — last_event_* had no readers in either build.) */
            switch (a->type) {
            case RE15_AOT_TYPE_DOOR: {
                a->door_hold = 0;   /* consumed the hold -> reset the accumulator so a still-held
                                     * button can't re-fire before the room actually changes (the
                                     * original sets 8 and relies on the transition state; resetting
                                     * to 0 is the safe equivalent — the door is gone after the pass) */
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
                 * artifacts). Room INDEX 0 is a VALID destination (ROOM_x00): the warp
                 * FUN_8001d600 reads the door struct dest bytes (+8 stage / +9 room) with
                 * NO room==0 special case — the old `dest_room != 0` pre-filter threw
                 * ROOM1050's three doors to ROOM1000 into the in-room-teleport branch
                 * (wrong room + wrong camera + wrong spawn). The same-room-vs-cross-room
                 * decision is carried SOLELY by dest_id != current (a genuine same-room
                 * door resolves dest_id == current and falls through).
                 * [audit wf_559c230f DOOR-DESTROOM-ZERO]
                 * The actual load runs after the scan (re15_room_apply_pending). SHARED
                 * (PC links room_common.c + a file RDT loader → PC doors work too). */
                {
                    unsigned dest_id = (((unsigned)d->dest_stage + 1u) << 12)
                                     | ((unsigned)d->dest_room << 4)
                                     | (g_current_room_id & 0x000Fu);
                    if (dest_id != g_current_room_id) {
                        int32_t sx = d->spawn_x, sy = d->spawn_y, sz = d->spawn_z;
                        int16_t syaw = d->spawn_yaw_4096;
                        int     scut = (int)d->target_cut;
                        /* INTRO-AUTO-TÜR: ihre Null-Rechteck-„next-pos" trägt KEINE echte
                         * Ziel-Spawn-Position — sie ist der EIGENE Spawn des Quellraums
                         * (z.B. 1240: -26214,0,-3861, Y=0). Den Intro-Raum stattdessen an
                         * SEINEM korrekten Eintritts-Spawn betreten (re15_room_spawns),
                         * sonst landet der Spieler auf der falschen Ebene/Band (Y aus dem
                         * Quellraum). Normale Lauf-Türen behalten ihren echten Spawn. */
                        if (is_auto_door) {
                            for (int ri = 0; ri < RE15_ROOM_COUNT; ri++) {
                                if (re15_room_ids[ri] == dest_id) {
                                    const re15_room_spawn_t *rs = &re15_room_spawns[ri];
                                    sx = rs->x; sy = rs->y; sz = rs->z;
                                    syaw = rs->yaw; scut = rs->cut;
                                    break;
                                }
                            }
                        }
                        re15_room_request_change(dest_id, sx, sy, sz, syaw, scut);
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
                 * SFX {bank2,sample2,vol0x60,pan0x40}. NON-ISSUE / byte-true SILENT
                 * (RE wf_4a2da55b): the door AOT SCE handler FUN_800430bc @0x800430bc
                 * (SCE-table[2] @0x8007469c) only sets transition-state DAT_800b5359=1
                 * + freezes and runs NO sub; the per-AOT dispatcher FUN_80042bac invokes
                 * exactly that one handler (no 2nd dispatch); and the transition FSM
                 * FUN_8001c958 state-1 STOPS sound (FUN_80061fc0(-1)). So there is NO
                 * door event_sub that plays a Se_on — the room1040 Se_on(2,12)/(2,10) are
                 * the ROOM script's own cues, not door-triggered. The door is correctly
                 * silent; the earlier "wire the event_sub" follow-up was a phantom. */
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
                /* Byte-true item-pickup PRESENTATION (arming LAB_80043328 @0x80043328 -> the FSM
                 * FUN_8001db28): DON'T grant here — start a freeze-the-game zoom/flip MODAL that
                 * defers the grant to its END. The modal does the re15_inv_grant INSERT
                 * (FUN_8004dc4c), sets the zone-9 taken-bit (FUN_8004ef90 @0x8001e0d0), and
                 * deactivates THIS AOT on confirm — all gated on inventory-room (a FULL inventory
                 * plays the shrink-away and LEAVES the item in the world @0x8001e0ec). The AOT stays
                 * active during the modal (the game is frozen, so it never re-scans). See
                 * re15_item_modal.h. WAS: an immediate silent grant with no presentation — the
                 * divergence this closes (U11). The pickup SE is the room's own SCD Se_on (already
                 * SCD-driven), never a fabricated value (door/item hack audit BO-round 2026-05-29). */
                const re15_aot_item_params_t *p = &g_aot.item_params[i];
                re15_item_modal_start(p->item_type, p->amount, p->taken_bit, i);
                break;
            }
            case RE15_AOT_TYPE_CAM_SWITCH: {
                /* RVD camera transit: only swap the active cut. No SFX,
                 * no teleport. target_cut packed in event_id field. */
                g_scd.cam_id              = a->event_id;
                g_scd.cam_change_pending  = 1;
                break;
            }
            case RE15_AOT_TYPE_FLAG_CHG: {
                /* sce=4 (LAB_80043120): flag table 0x80074664[group], MSB-first bit, on/off.
                 * Idempotent — the auto variants re-write every frame inside (byte-true; the
                 * script may clear the flag while the entity stays inside -> re-assert). */
                const re15_aot_flag_params_t *fp = &g_aot.flag_params[i];
                if (fp->group < RE15_FLAG_ZONES)
                    re15_game_flag_set(fp->group, fp->bit, fp->on);
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

    /* Apply the RVD pick recorded above: the highest-slot inside CAM_SWITCH = the lowest RVD table
     * index the player is inside for this cam_from (byte-true FIRST-in-table). Only reached when no
     * earlier SCD AOT fired (returned) — preserving the original SCD-AOT-vs-camera precedence. */
    if (best_cam_id >= 0) {
        g_scd.cam_id             = (uint8_t)best_cam_id;
        g_scd.cam_change_pending = 1;
    }
}
