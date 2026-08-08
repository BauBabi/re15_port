/*
 * RE1.5 Rebuilt — Area-of-Trigger (Phase 4.4.6, 2026-05-19).
 *
 * AOTs are rectangular trigger zones in the room's XZ plane. The SCD
 * VM populates them via Aot_set (0x2C) / Door_aot_set (0x3B) / Item_aot_
 * set (0x50) opcodes. Each frame, the main loop tests the player's XZ
 * position against every active AOT — when the player enters a zone
 * (was outside last frame, inside this frame), the AOT's event fires:
 * a small "edge-trigger" state is written into g_scd so consumers can
 * react (start a SCD thread, swap a cut, grant an item, etc).
 *
 * Per RE2's AOT runtime (RE2_Quellcode/FUN_80051088.c): the system
 * walks an array, dispatches per-AOT-type via PTR_LAB_800a73c4 (16-
 * entry). For our minimal port we collapse all types into one rect-
 * check + one event-id; specialised behaviour can come later when SCD
 * scripts actually use Door_aot_set vs Item_aot_set differently.
 */

#ifndef RE15_AOT_H
#define RE15_AOT_H

#include <stdint.h>

#define RE15_AOT_MAX  64   /* per-room AOT slot count. BO-round 2026-05-29:
                            * bumped 32→64 (= RE2 retail). ROOM1170 has 28 RVD
                            * zones, ROOM1260/1261 have 44; with slot_offset=16
                            * a 32-slot array silently truncated the gameplay
                            * walking cam-switch zones. +2KB RAM, negligible. */

/* AOT type ids — passed in `type` field, drives per-frame dispatch in
 * the scanner.
 *  GENERIC    : fires the event handler bytecode (Phase 4.4.6.1)
 *  DOOR       : C-side cut+pos teleport + door SFX (Phase 4.4.7)
 *  ITEM       : C-side inventory grant + pickup SFX + deactivate (Phase 4.4.7)
 *  CAM_SWITCH : C-side Cut_chg, target_cut stored in event_id (Phase 4.5.9-B —
 *               for RVD camera-transit zones, no SFX, no teleport) */
#define RE15_AOT_TYPE_GENERIC     0
#define RE15_AOT_TYPE_DOOR        1
#define RE15_AOT_TYPE_ITEM        2
#define RE15_AOT_TYPE_CAM_SWITCH  3
/* STAIR (2026-06-07): the band-transition zones (SCD Aot_set type 12/13 — the
 * outdoor staircase in ROOM1170). The scanner SKIPS these (stair_common.c owns
 * them): on an action press while the player stands in a stair zone, Leon auto-
 * walks the steps and his floor band changes (descend to the pit / ascend to the
 * courtyard), which is what makes the lower-level doors reachable. The two SCD
 * types are the two ends of one staircase (13 = the (1,0,2) end, 12 = the (0,0,2)
 * end); the destination band is derived from the room's band range, not the type. */
#define RE15_AOT_TYPE_STAIR       4
/* EXAMINE / MESSAGE AOT (the original's sce=1 AOT type). On an action press while
 * inside/reaching, it shows a .msg index and returns WITHOUT a room change — the
 * byte-true "It's not necessary to go back" back-door in ROOM1130 (sub01 re-types the
 * door-3 slot to this via Aot_reset(slot, sce=1, msg)). `event_id` holds the msg index. */
#define RE15_AOT_TYPE_MESSAGE     5
/* AUTO event AOT (2026-06-15, byte-true FUN_80042bac): an event AOT whose
 * trigger-flags byte (SCD Aot_set pc[3]) has bit 0x10 CLEAR — it fires its SCD
 * sub on WALK-IN (player rect-overlap), no action button. The per-frame auto-scan
 * (FUN_800436a8) calls the AOT dispatcher with param_3=0, matching only AOTs whose
 * flags bit 0x10 is clear; the player-state action scan passes param_3=0x10, matching
 * the bit-set (GENERIC) ones. ROOM1150 AOT6 (pc[3]=0x41 → Irons cutscene sub08) is
 * such an auto-event; it self-disables via its own first op Aot_reset(6,0,…). The
 * scanner runs it through the edge-triggered default path (inside && !was_inside →
 * fired_event_id_this_frame), so no per-type firing code is needed — only a distinct
 * type so it is NOT routed through the action-press GENERIC branch. */
#define RE15_AOT_TYPE_AUTO_EVENT  6
#define RE15_AOT_TYPE_EXAMINE_WORKVAR  7   /* examine→work_vars[0]=index (ev==0 Aot_set) */
#define RE15_AOT_TYPE_FLAG_CHG    8   /* sce=4 flag set/clear (LAB_80043120) — wf_f536e1ee step 4 */
#define RE15_AOT_TYPE_WATER       9   /* sce=8 water-level stamp (LAB_8004330c): entity+0x88 = u16@0
                                       * per frame, ALL 3 pools (flags 0x47) — 32 zones (sewers) */
#define RE15_AOT_TYPE_RAMP       10   /* sce=7 stair-ramp Y driver (LAB_800431cc): Y from the rect
                                       * edge distance -> entity Y. 4 zones, ROOM5060/5061. */
/* sce=0 NONE (aot_sce_census d7376834): the record is REGISTERED (installers @0x80040580/
 * 0x80040608/0x800406d0 store pc+2 unconditionally) but INERT — handler table entry [0]
 * @0x8004305C only RESTORES the work-var latch from the snapshot 0x800bbde8/0x800bbeec
 * (undoing the scan's pre-dispatch stamp), and the ACTION scan skips sce-0 records outright
 * (@0x80042f48-50 `lbu v0,0(s0); beq v0,zero,+loop` — no dispatch, press NOT consumed).
 * 74 shipped fresh sce-0 installs (37 doors, of which 21 are NEVER retyped = permanently
 * dead); a live Aot_reset (0x46) can RETYPE the slot later (re15_aot_retype). The port's
 * scan skips this type entirely — geometry/params stay stored for the retype. */
#define RE15_AOT_TYPE_NONE       11

typedef struct {
    uint8_t  active;      /* 0 = slot free, 1 = active */
    uint8_t  type;        /* RE15_AOT_TYPE_* */
    uint8_t  event_id;    /* fired on player-enter; for GENERIC only */
    uint8_t  was_inside;  /* 1 = player was inside last frame
                            * (prevents repeat-fire while standing inside) */
    /* Phase 4.5.9-B.1: cam_from filter. RVD CAM_SWITCH zones only fire
     * when this AOT's cam_from matches the active cut. 0xFF disables
     * the filter (DOOR/ITEM/GENERIC AOTs are always active). */
    uint8_t  cam_from_filter;
    /* AQ-round 2026-05-26: has_quad=1 → use xs/zs point-in-quad (CAM_SWITCH
     * RVD). has_quad=0 → use x/z + half_w/half_h AABB (DOOR/ITEM/GENERIC).
     * Trapezoidal RVD zones cannot be safely approximated by AABB — see
     * [[bugfix_AQ_round_rvd_point_in_quad_2026_05_26]]. */
    uint8_t  sce_flags;   /* raw record[1] (= Aot_set pc[3]) — byte-true FUN_80042bac test bits
                           * (wf_f536e1ee step 2): 0x40 = CENTRE test first (stamps work_var[1]),
                           * 0x20 = FORWARD 620-point vs the EXACT rect/quad (stamps work_var[0]),
                           * 0x10 = action-scan-only, low 3 bits = pool mask. 0 = legacy install
                           * (synthetic/tests) -> both tests. */
    uint8_t  has_quad;
    /* STAIR (2026-06-07): the runtime AOT band = Aot_set `chain` operand pc[4]
     * (entry+0x82 in the original) = the platform this stair end sits on
     * (0/2/4 in ROOM1170). Stair entry is gated on band == player band. */
    uint8_t  band;
    uint8_t  door_hold;   /* DOOR only: byte-true 9-frame press-and-hold accumulator (obj+0x8C,
                           * FUN_8002bd44) — the door opens when this reaches 9, not on a tap-edge */
    int16_t  xs[4];       /* quad corners X (LE from RDT zone) */
    int16_t  zs[4];       /* quad corners Z */
    int32_t  x;           /* axis-aligned rect, XZ plane (PSX world units) */
    int32_t  z;
    int32_t  half_w;      /* half-extents — saves one mul per frame */
    int32_t  half_h;      /* "h" here is the Z half-extent (depth) */
} re15_aot_t;

/* Per-slot params for the specialized AOT types. Kept in parallel arrays
 * (one entry per AOT slot) so re15_aot_t stays compact for the generic
 * path. */
typedef struct {
    uint8_t  target_cut;       /* room cut index to switch to on entry */
    uint8_t  dest_stage;       /* Door_aot_set pc[22] — destination stage */
    uint8_t  dest_room;        /* Door_aot_set pc[23] — destination room byte.
                                * 0 / resolves to current room → same-room
                                * teleport; else → cross-room load. */
    int32_t  spawn_x;          /* player spawn position in target room */
    int32_t  spawn_y;
    int32_t  spawn_z;
    int16_t  spawn_yaw_4096;
    uint8_t  floor;            /* Door_aot_set pc[3] — a near-CONSTANT AOT flag (0x31 in
                                * 649/653 doors, 0xB1 in 4), NOT the door's level. Stored
                                * for reference only — the band gate uses door_band below. */
    uint8_t  band;             /* Door_aot_set pc[4] = the door object's BAND (obj[0x82]).
                                * BYTE-TRUE per ROOM1170 main00: door0/1/6 pc4=4 (courtyard
                                * band-4), door4/5 pc4=0 (pit band-0). The original gates a
                                * door's interaction on player_band == obj[0x82]
                                * (FUN_8002bd44 @0x8002bf38: lbu DAT_800acad6 / lbu obj+0x82
                                * / bne → skip the FUN_8002d1e8 forward-reach). This stops a
                                * band-4 courtyard player from reaching/firing the band-0 pit
                                * doors (and vice-versa) — band crossing only via a stair. */
} re15_aot_door_params_t;

/* sce=4 FLAG_CHG payload (byte-true LAB_80043120: bitarray table 0x80074664[u16@0=group],
 * bit u16@2 (MSB-first 0x80000000>>bit), on/off u16@4) — wf_f536e1ee step 4. */
typedef struct {
    uint8_t  group;       /* flag zone (the ptr-table index)  */
    uint8_t  bit;         /* MSB-first bit index               */
    uint8_t  on;          /* 1 = set, 0 = clear                */
} re15_aot_flag_params_t;

/* sce=7/8 environment-zone payload (WATER: p0 = water Y; RAMP: p0 = mode, p1 = step, p2 = height). */
typedef struct { int16_t p0, p1, p2; } re15_aot_env_params_t;

typedef struct {
    uint8_t  item_type;
    uint8_t  amount;
    uint8_t  taken_bit;   /* payload[2] (pc[18]/pc[26]) — the DAT_800b1078 taken-bit index
                           * (flag zone 9 = ptr-table 0x80074664[9] @0x80074688). Set on pickup
                           * (FUN_8004ef90); re-checked at INSTALL (@0x800406d4) so taken items
                           * do not respawn on room re-entry. [wf_f536e1ee #14-step-1] */
} re15_aot_item_params_t;

/* STAIR per-record direction data (byte-true sce-12/13 handlers LAB_80043500/LAB_800435cc —
 * analysis/stair_10a0.md): the original has NO zone pairing; each stair AOT decides UP/DOWN
 * alone from the rect HALF the player stands in along the stair axis, tied to the record's
 * side flag:
 *   side   = u16 rec+0xC = Aot_set pc[14..15] (lhu @0x80043530/58 sce12, @0x800435fc/24
 *            sce13) — names the rect half that is the LOWER stair end (0 = low-coordinate
 *            half, 1 = high). Standing there = ASCEND, the other half = DESCEND.
 *   count  = u8 rec+0xE = pc[16] (lbu @0x800435b8 / @0x8004367c) — bands traversed (&7;
 *            the gait counter DAT_800acaf2 starts at (count&7)-1 @0x800389cc-dc).
 *   axis   = the sce itself: 12 = X (lh 0x4(a2) + lw 0x34(a1) @0x80043510-14),
 *            13 = Z (lh 0x6(a2) + lw 0x3c(a1) @0x800435dc-e0).
 *   corner/extent = the RAW record rect fields along that axis (the half test is
 *            `player[axis]-corner < extent>>1`, srl+slt @0x80043520-24 / @0x800435ec-f0). */
typedef struct {
    uint16_t side;
    uint8_t  count;
    uint8_t  axis;        /* 12 = X, 13 = Z */
    int32_t  corner;
    int32_t  extent;
} re15_aot_stair_params_t;

typedef struct {
    re15_aot_t              slots[RE15_AOT_MAX];
    re15_aot_door_params_t  door_params[RE15_AOT_MAX];
    re15_aot_item_params_t  item_params[RE15_AOT_MAX];
    re15_aot_flag_params_t  flag_params[RE15_AOT_MAX];
    re15_aot_env_params_t   env_params[RE15_AOT_MAX];
    re15_aot_stair_params_t stair_params[RE15_AOT_MAX];

    /* Edge-trigger state: set when an AOT fires, cleared by consumer or
     * after N frames of HUD display. */
    uint8_t   last_event_id;
    uint8_t   last_event_aot;
    int       last_event_frame_remaining;  /* HUD countdown */

    /* For GENERIC AOT types only — set non-zero by re15_aot_scan() ONLY
     * on the frame an entry-edge fires (event_id of the AOT), cleared at
     * the start of the next scan call. Consumed by main loop to dispatch
     * to the SCD event-handler. DOOR / ITEM AOTs handle themselves in C
     * inside the scan and do NOT set this field. */
    uint8_t   fired_event_id_this_frame;
} re15_aot_state_t;

extern re15_aot_state_t g_aot;

/* RE2-faithful door gating: DOOR-type AOTs fire only when the player presses
 * the action button while standing in the door zone (not auto-on-walk-in).
 * Set by the main loop from the pad each frame, BEFORE re15_aot_scan. */
extern uint8_t g_aot_action_pressed;

void re15_aot_init(void);

/* Place a GENERIC AOT in a slot. Returns 0 on success, -1 on bad slot. */
int  re15_aot_set(int slot, uint8_t type, uint8_t event_id,
                   int32_t cx, int32_t cz, int32_t half_w, int32_t half_h);

/* Place a DOOR-type AOT — extends the rect with target cut + spawn
 * position. On entry, the scan auto-applies cut/pos. */
int  re15_aot_set_door(int slot, int32_t cx, int32_t cz,
                        int32_t half_w, int32_t half_h,
                        uint8_t target_cut,
                        int32_t spawn_x, int32_t spawn_y, int32_t spawn_z,
                        int16_t spawn_yaw_4096);

/* Place an ITEM-type AOT. On entry, the scan grants the item to the
 * inventory and deactivates the AOT (one-shot pickup). */
int  re15_aot_set_item(int slot, int32_t cx, int32_t cz,
                        int32_t half_w, int32_t half_h,
                        uint8_t item_type, uint8_t amount);
/* + the taken-bit index (payload[2]); re15_aot_set_item keeps 0 = no persistence. */
int re15_aot_set_item_tk(int slot, int32_t cx, int32_t cz,
                         int32_t half_w, int32_t half_h,
                         uint8_t item_type, uint8_t amount, uint8_t taken_bit);

/* Place a STAIR-type AOT (SCD Aot_set type 12/13). The rect is the stair zone; the
 * scanner ignores STAIR AOTs — stair_common.c polls them for the action trigger. The
 * per-record direction data (side/count/axis/corner/extent, see
 * re15_aot_stair_params_t) is what the byte-true sce-12/13 handlers LAB_80043500/
 * LAB_800435cc read; event_id stores (uint8_t)side for the debug dumps. */
int  re15_aot_set_stair(int slot, int32_t cx, int32_t cz,
                        int32_t half_w, int32_t half_h,
                        uint8_t band, uint16_t side, uint8_t count,
                        uint8_t axis, int32_t corner, int32_t extent);

/* Place a CAM_SWITCH-type AOT (RVD zone). On entry, the scan sets
 * g_scd.cam_change_pending with cam_id = target_cut. No SFX, no
 * teleport. `cam_from` filters: zone only scans when the active cut
 * matches; pass 0xFF to disable filtering (always active). */
int  re15_aot_set_cam_switch(int slot, int32_t cx, int32_t cz,
                              int32_t half_w, int32_t half_h,
                              uint8_t cam_from, uint8_t target_cut);

/* Disable an AOT slot. */
void re15_aot_reset(int slot);
/* Re-type an existing AOT slot into an EXAMINE/MESSAGE AOT (sce=1), keeping its rect.
 * `msg_index` = the .msg index shown on action. Used by Aot_reset(slot, sce=1, msg). */
void re15_aot_set_message(int slot, uint8_t msg_index);

/* Aot_reset (0x46) = FULL RETYPE (byte-true LAB_80040738): rec[0] = new sce (@0x80040764),
 * rec[1] = (old_flags & 0x80) + pc[3] (@0x8004076c-78), payload halfwords +0xC/+0xE/+0x10
 * (or +0x14.. for a polygon record — old flags bit 0x80, @0x80040774-84) = pc[4..9]
 * (@0x80040788-a8). rec[2] (the band byte) is NOT written — band preserved. Maps the new
 * sce + 3 payload halfwords onto the port's typed slot params. See aot_common.c. */
void re15_aot_retype(int slot, uint8_t sce, uint8_t flags,
                     uint16_t p0, uint16_t p1, uint16_t p2);

/* Aot_on (0x47) fire-now (byte-true LAB_800407bc): dispatch the slot's sce handler ONCE
 * immediately — `jalr PTR_8007469c[rec[0]]` @0x8004082c with a0 = the record payload
 * (@0x80040804/808) — bypassing every geometry/band/action test. All 73 shipped uses
 * target sce-2 doors (66) or sce-9 items (7) [aot_sce_census d7376834]. */
void re15_aot_fire_slot(int slot);
/* Cut_replace(a,b): swap cut ids a<->b in the live CAM_SWITCH AOTs (companion to the
 * RVD-zone swap in op_cut_replace). */
void re15_aot_cut_replace(uint8_t a, uint8_t b);

/* Per-frame scanner. Tests player_x/z against each active AOT. On
 * entry-edge: GENERIC sets fired_event_id_this_frame, DOOR applies cut+pos
 * directly, ITEM grants inventory + deactivates, CAM_SWITCH sets
 * cam_change_pending (filtered by cam_from). Call once per frame from
 * main, AFTER player tick. `active_cut` filters CAM_SWITCH zones. */
/* Glied 1 — AOT-Stempel auf entity+0x0B (byte-true FUN_800436a8 + FUN_80042bac): Clear + Stempel
 * fuer JEDEN Aktor, LAST-WINS ueber alle AUTO-Records. Wird von re15_aot_scan() zuerst gerufen;
 * separat exportiert, damit Sonden/Tests ihn ohne den Handler-Dispatch fahren koennen.
 * Soll-Werte am Original gemessen: analysis/room1030_crawl_mechanism.md §15. */
void re15_aot_stamp_entities(void);

void re15_aot_scan(int32_t player_x, int32_t player_z, uint8_t active_cut);

/* Phase 4.5.12: prime was_inside from a spawn position without firing
 * AOTs. Call once after the player is positioned at room-load so doors
 * that enclose the spawn don't auto-trigger on frame 1. */
void re15_aot_settle_at(int32_t player_x, int32_t player_z);

/* Point-in-quad (PSX FUN_80014368), int64 cross-products. Shared with the
 * per-cut region-quad prop/NPC cull. Returns 1 if (px,pz) is inside. */
int  re15_aot_point_in_quad(int32_t px, int32_t pz,
                            const int16_t xs[4], const int16_t zs[4]);

/* Combination-lock NOTCH probe (FUN_80042bac @0x80042f5c): the sce=5 grid-cell slot the point
 * (px,pz) is over, or -1. The keypad cursor's member+0xb = this each frame. */
int  re15_aot_object_notch(int32_t px, int32_t pz);
/* Per-frame object-pool notch pass: set every active prop's member_0b = the grid cell it is over.
 * Called once per frame by re15_game_step so the keypad dial cursor's confirm reads the live cell. */
void re15_object_notch_update(void);

/* Byte-true per-prop visibility cull — SHARED by BOTH ports so the Obj_model_set
 * prop loop culls identically. Per FUN_8002c18c (object render/scan loop):
 *   type != 3 (the common path — INCLUDING every room1170 prop, which are ALL
 *     type 0; see below): whole-object FUN_80014368 quad cull — the object is
 *     drawn iff its world (x,z) lies inside the active cut's RVD anchor quad
 *     (DAT_800ac790). A script-sunk box (Member_set(0,-31000)) is hidden simply
 *     because its XZ then falls outside every cut's quad.
 *   type == 3: FUN_8002c18c routes to FUN_8003e64c (sprite-grid fixture) which
 *     does its OWN per-cell occlusion and SKIPS the whole-object cull → return 0
 *     (don't whole-object cull). room1170 has NO type-3 prop, so this is dormant.
 *
 * KEY CORRECTION (2026-06-13): obj00/01 are type 0, NOT type 3 — the old code
 * read the render type from pc[4] (the +0x82 BAND field, box=3/heli=5) instead
 * of pc[2] (the real type byte LAB_80040914 stores to pool+0x08). With the type
 * read fixed in scd_vm.c, every room1170 prop is type 0 and takes the quad cull.
 *
 * Region-less safety net: if the active cut carries NO RVD region (cam_has_region
 * == 0) we cannot run the quad test; fall back to the sink-gate (x < -25000) so a
 * cinematic cut without region data still hides the SCD-sunk box instead of
 * leaving it floating. With a region present the cull is the pure byte-true quad
 * test. Returns 1 = CULL (skip this prop), 0 = draw. */
static inline int re15_prop_culled(int obj_type, int32_t x, int32_t z,
                                   int cam_has_region,
                                   const int16_t region_xs[4],
                                   const int16_t region_zs[4])
{
    if (obj_type == 3)
        return 0;   /* sprite-grid path owns its own per-cell occlusion */
    if (cam_has_region)
        return !re15_aot_point_in_quad(x, z, region_xs, region_zs);
    return x < -25000;   /* no region (cinematic): hide only the SCD-sunk box */
}

#endif /* RE15_AOT_H */
