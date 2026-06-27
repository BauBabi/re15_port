/*
 * re15_to_re2.c — RE1.5 → RE2 conversion / adapter layer.
 *
 * The single source of truth for RE1.5 quirks. When you discover a new
 * difference (an opcode arg means something different in RE1.5 vs RE2,
 * a data field is at a different offset, etc.), document and translate
 * it here. Do NOT scatter `if (re15_format) { ... }` conditionals through
 * the opcode handlers — route through this module instead.
 *
 * Sources cross-referenced when authoring:
 *   - RE_15_modified_V2/*.c        (Ghidra-decompiled RE1.5 PSX.EXE)
 *   - RE2_Quellcode/*.c             (Ghidra-decompiled RE2 retail)
 *   - info/Resident_Evil_und_Playstation_Information/
 *     ├── re1.5-specific/lib_bio-master/lib_bio/lib_*.py  (Python parsers)
 *     ├── BioClone Redux 10152023/animation##.ini         (decoded keyframes)
 *     └── secondary_source/BioRdt-master/*.cpp            (C++ RDT editor)
 *   - extracted/PSX/STAGE1/ROOM1170/scd/sub*.c (disassembled scripts)
 *
 * Phase 4.5.13 P-CONVERTER (2026-05-23): created per user directive.
 */

#include "re15_to_re2.h"

/*=========================================================================
 * Member_set / Member_cmp ID translation
 *=========================================================================*/

int re15_to_re2_member_id(int re15_id)
{
    /* RE1.5 BioClone-style → RE2 PTR_LAB_800a74c8 IDs.
     *
     * RE1.5 sub02 evidence:
     *   Member_set(0, -6010) → player.x = -6010  → RE2 ID 0x0B
     *   Member_set(2, 6265)  → player.z = 6265   → RE2 ID 0x0D
     *   Member_set(4, 4)     → state byte         → RE2 ID 0x04
     *
     * RE2 actor struct (per F1 agent's audit of FUN_80055cb0):
     *   +0x00 flags   +0x04 state   +0x06 sub_state_1  +0x07 sub_state_2
     *   +0x0B x  +0x0C y  +0x0D z   +0x0E rot_x  +0x0F rot_y                */
    switch (re15_id) {
    case 0:  return 0x0B;   /* RE1.5 X → RE2 +0x0B */
    case 1:  return 0x0C;   /* RE1.5 Y → RE2 +0x0C */
    case 2:  return 0x0D;   /* RE1.5 Z → RE2 +0x0D */
    /* G14 corrected per RE_15_modified_V2/entity_set_field.c:
     * RE1.5 members 3/4/5 = rot_x/rot_y/rot_z (NOT yaw/state/state-byte).
     * Previously case 4 silently routed yaw writes to sub_state_2,
     * so Member_set(4, N) had no visible effect — Leon's facing during
     * cinematic wasn't being set. */
    case 3:  return 0x0E;   /* RE1.5 rot_x → RE2 rot_x */
    case 4:  return 0x0F;   /* RE1.5 rot_y → RE2 rot_y */
    case 5:  return 0x10;   /* RE1.5 rot_z → RE2 rot_z */
    case 6:  return 0x06;
    case 7:  return 0x07;
    case 8:  return 0x08;   /* motion (same ID in both) */
    case 9:  return 0x09;
    case 10: return 0x0A;
    /* IDs 0x0B+ are typically pass-through if scripts ever use them. */
    default:
        if (re15_id >= 0x0B && re15_id <= 0x25) return re15_id;
        return -1;
    }
}

/*=========================================================================
 * Plc_dest mode → motion clip + speed
 *=========================================================================*/

int re15_to_re2_plc_dest_clip(int mode, int with_rbj_overlay)
{
    /* Map Plc_dest mode → motion role → clip. The W01 locomotion source IS the
     * canonical stage-0 player anim the original CD-loads (file index 0x4C, verified
     * BYTE-IDENTICAL to PL00W01 by disc extraction) — so its clips are correct.
     *
     * WALK and RUN are DISTINCT motions — PROVEN from the PSX Plc_dest handler
     * FUN_80041BE4 (ghidra1_V2.txt), NOT just visual feedback: the handler reads
     * pc[2]=mode and switches on (mode-4) into a PER-MODE motion table indexed by
     * the player anim-set (player+0x8). Each table holds {motion_id, speed} pairs:
     *   mode 4 WALK → DAT_80073EA5 → {0x30, 0x4B}  (motion 48, speed 75)
     *   mode 5 RUN  → DAT_80073F25 → {0x48, 0xC8}  (motion 72, speed 200)
     *   mode 9 TURN → DAT_80073EE5 → {0x60, 0x00}  (motion 96, speed 0)
     * Distinct motion ids 0x30 vs 0x48 ⇒ walk ≠ run. The actual EDD clip is set by
     * the locomotion substate handlers at player+0x94 (read by resolver FUN_8001f314):
     * WALK handler 0x80030AF0 sets +0x94=5 (entry @80030B58, steady keeps it) → clip 5;
     * RUN handler 0x80030D28 sets +0x94=5 for one transition tick then its steady state
     * (@80030DD8 `sb zero`) sets +0x94=0 → clip 0. So byte-level: walk→clip5, run→clip0.
     * We render these on the W01 EDD: 105 = W01 clip 5
     * (walk), 100 = W01 clip 0 (run). This W01 clip pick is now DATA-DERIVED, not a
     * visual guess: scripts/analyze_w01_clips.py parsed PL00W01.edd/.emr and proved
     * clip 0 and clip 5 are DISTINCT animations — disjoint keyframes (0-21 vs
     * 102-131, 0 shared); clip 0 (22f, limb swings up to ~111deg) = the energetic
     * RUN, clip 5 (30f, swings ~44deg) = WALK. So run!=walk in the data, and these
     * two clips are the correct distinct realizations. An earlier "all-loco =
     * clip 5" reading was a shallow trace of the category byte and is wrong. */
    switch (mode) {
    case 0x04:
        return re15_to_re2_resolve_motion(RE15_MOTION_WALK, with_rbj_overlay);
    case 0x05:
        return re15_to_re2_resolve_motion(RE15_MOTION_RUN, with_rbj_overlay);
    case 0x07:
    case 0x08:
        return re15_to_re2_resolve_motion(RE15_MOTION_WALK, with_rbj_overlay);
    case 0x09:
        /* TURN — keep current clip while rotating in place. */
        return -1;
    default:
        return -1;
    }
}

/* BO-round 2026-05-29 (Tier-2 #15): DEAD CODE — no callers (verified). The
 * live per-mode walker speeds are PLC_STEP_* in actor_locomotion.c. Kept for
 * reference only; do not "fix" the steady-state values here without wiring it
 * up to the real 2-state walker FSM (LAB_80030d28 burst 0xC8→steady 0x60). */
int re15_to_re2_plc_dest_speed(int mode)
{
    /* P9 empirical from PSX.EXE per-mode walker handlers:
     *   mode 4 WALK handler 0x80030AF0: state-0 = 0x4B (75), state-1 = 0x60 (96)
     *   mode 5 RUN  handler 0x80030D28: state-0 = 0xC8 (200), state-1 = 0x60 (96)
     *   mode 9 TURN handler 0x80031360: pure rotate, speed 0
     *   mode 7/8: similar to WALK/RUN
     * We use steady-state (state-1) since cinematic doesn't implement the
     * state machine and these are the "in motion" values.                */
    switch (mode) {
    case 0x04: return 96;    /* WALK */
    case 0x05: return 96;    /* RUN  (would be 200 at burst, 96 steady) */
    case 0x06: return 0;     /* stop */
    case 0x07: return 96;
    case 0x08: return 96;
    case 0x09: return 0;     /* TURN — no translation */
    default:   return 0;
    }
}

/*=========================================================================
 * Subtitle text for ROOM1170 cinematic
 *=========================================================================*/

const char *re15_to_re2_room1170_subtitle(int message_id)
{
    /* Verbatim text from extracted/PSX/STAGE1/room1170/msg/main##.txt.
     * sub02 uses IDs 0..7, sub11 uses 8..11, sub14 uses 13..16. */
    switch (message_id) {
    case 0:  return "Elliot: We must go! There is no time to lose!";
    case 1:  return "Pilot: Is there no one else left inside?";
    case 2:  return "Elliot: There is no one left, we must go!";
    case 3:  return "Elliot: Those things are already close, they almost caught me!";
    case 4:  return "Pilot: Alright, get on board we're off!";
    case 5:  return "Leon: Hey! Hey wait! Down here!";
    case 6:  return "Leon: Oh, that's just freaking great...";
    case 7:  return "Leon: Now what am I gonna do?";
    case 8:  return "We barricaded ourselves inside the police station";
    case 9:  return "Where we thought it would be safe";
    case 10: return "It was no use, and so began a life and death struggle";
    case 11: return "To escape";
    case 12: return "It's locked from the other side";
    case 13: return "Leon: Damn it, there must be another way out of here";
    case 14: return "Leon: If only I could contact the outside";
    case 15: return "Leon: ...";
    case 16: return "Leon: That's right! I have to report this to the chief";
    default: return NULL;
    }
}

/*=========================================================================
 * Motion role → clip resolver
 *=========================================================================*/

int re15_to_re2_resolve_motion(re15_motion_role_t role, int with_rbj_overlay)
{
    /* Two catalogs depending on whether the room's rbj overlay is active.
     *
     * PL00.EDD (no overlay) per P3 + P12 empirical bone-diff analysis:
     *   clip 0  = IDLE       clip 10 = WALK loop   clip 12 = RUN-skid
     *   clip 15 = RUN-like   clip 3  = FALL        clip 20 = CLIMB
     *
     * rbj overlay (per P2 agent's bone analysis):
     *   clips 0..14 = T-pose padding (alignment slots)
     *   clip 15 = WALK locomotion (root-baked Speed field 22→111)
     *            ALSO confirmed visually by user as "pointing" pose —
     *            the walk clip has Leon's arms in pointing position
     *            (because sub02 plays it while he's calling out "Hey!").
     *   clip 16 = idle-talk hold
     *   clip 17 = head-turn + gesture
     *   clip 20 = point / look-at
     *   clip 22 = "Hey!" wave (both upper-arms 28° sweep)
     *   clip 23 = head-turn-left + arm reaction                          */

    if (with_rbj_overlay) {
        switch (role) {
        case RE15_MOTION_IDLE:      return 0;     /* T-pose; rbj has no real idle */
        /* AD-round (2026-05-26): Use sentinel 105 = PL00W01 clip 5
         * (Walk_Forward), 100 = PL00W01 clip 0 (Run). Per RE2-Nov96
         * weapon-track architecture: PL00.EDD (normal track) has only
         * Walk_Backward; the forward walk + run live in PL00W*.EDD which
         * RE1.5 also has. Renderer detects motion >= 100 and switches to
         * pl00w01_skel/anim, subtracting 100 to get the real clip index. */
        case RE15_MOTION_WALK:      return 105;   /* W01(=file-0x4C) clip 5 = Walk_Forward (motion 0x30 per FUN_80041BE4) */
        case RE15_MOTION_RUN:       return 100;   /* W01(=file-0x4C) clip 0 = RUN  (motion 0x48 per FUN_80041BE4 — distinct from walk) */
        case RE15_MOTION_WAVE:      return 22;    /* "Hey!" wave */
        case RE15_MOTION_ARM_LIFT:  return 16;
        case RE15_MOTION_POINT:     return 20;
        case RE15_MOTION_HEAD_TURN: return 17;
        default: return 0;
        }
    } else {
        switch (role) {
        case RE15_MOTION_IDLE:  return 0;
        /* AB-round (2026-05-25): per gltf-extractor analysis of all 24
         * PL00 clips: clip 19 is the CANONICAL WALK FORWARD GAMEPLAY LOOP.
         * Evidence:
         *   - 20 frames, 0.63s, perfect loop closure (frame 0 == frame 19)
         *   - R-thigh sweep -19°→+7° anti-phase L-thigh +18°→+7°
         *   - Phase shift 0.40 (~half period)
         *   - Anti-phase shoulder swing
         *   - Root Y subtle bob (-0.42 to -0.44 = natural step bob)
         *   - Root X/Z = 0 (in-place loop; walker adds locomotion via
         *     scalar speed at +0x8C per L-round memory)
         * Clip 4 (prev AA): user-verified "fall on knee", NOT walk.
         * Clip 1 (Z): walk-backward (Speed.X negative).
         * Clip 12 (Y): climb-ladder loop.
         * Clip 8 (22f, 47°/35° knee amp): likely RUN/SPRINT.  */
        case RE15_MOTION_WALK:  return 19;
        /* BO-round 2026-05-29 (hack audit): RUN was aliased to the WALK clip
         * (19) "because run is too aggressive" — an aesthetic fudge. PSX RUN is
         * its own motion (id 0x48, ghidra1_V2.txt:241934). Use the W01 Run
         * sentinel (100), same mechanism the rbj-overlay path already uses;
         * main.c intercepts 100→PL00W01 Run clip 0. (ROOM1170 takes the rbj
         * overlay branch, so this only affects non-overlay gameplay rooms.) */
        case RE15_MOTION_RUN:   return 100;  /* W01 clip 0 = RUN sentinel */
        case RE15_MOTION_TURN:  return 10;
        case RE15_MOTION_FALL:  return 7;
        case RE15_MOTION_CLIMB: return 11;
        default: return 0;
        }
    }
}
