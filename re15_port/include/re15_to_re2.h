/*
 * re15_to_re2.h — RE1.5 → RE2 conversion / adapter layer.
 *
 * Per user mandate (2026-05-23): "if you can't port directly, write a
 * converter that translates each RE1.5 opcode to RE2 opcodes correctly,
 * and converters that map RE1.5 technology to RE2 technology."
 *
 * Project governing principle (MEMORY): Engine = RE2, Content = RE1.5.
 * Our SCD VM, render pipeline, and game state are RE2-pure. This module
 * is the SINGLE PLACE where RE1.5's quirks get translated to fit the
 * RE2 engine. When a new RE1.5/RE2 difference is discovered, add the
 * translation here — not scattered through opcode handlers.
 *
 * Categories:
 *   1. Opcode argument translators (member_id remap, mode remap, etc.)
 *   2. Data format converters (EDD, EMR, MD1, RBJ layouts)
 *   3. Motion/clip selection adapters
 *   4. Address/coordinate space adapters
 *
 * Naming convention: re15_to_re2_<op>_<arg>() for arg translators,
 * re15_to_re2_<format>_convert() for data converters.
 */
#ifndef RE15_TO_RE2_H
#define RE15_TO_RE2_H

#include <stdint.h>
#include <stddef.h>

/*=========================================================================
 * SCD OPCODE ARG TRANSLATORS
 *=========================================================================*/

/* Member_set / Member_cmp (op 0x34 / 0x3E) member-id translation.
 *
 * RE1.5 SCD uses BioClone-style member IDs:
 *   0  = X        1  = Y        2  = Z        3  = yaw (rot_y)
 *   4  = state byte (sub_state_2)
 *   5  = sub_state_3
 *   ...
 *
 * RE2 uses a different table (per F1 agent's PTR_LAB_800a74c8 audit):
 *   0x00 = flags     0x0B = X      0x0E = rot_x
 *   0x02 = state     0x0C = Y      0x0F = rot_y
 *   0x03..6 = sub-states            0x10 = rot_z
 *   0x08 = motion   0x0D = Z       0x16 = speed_h
 *
 * Translation maps RE1.5 IDs to RE2 IDs. Sub02's `Member_set(0,-6010)`
 * (RE1.5 ID 0 = X) → RE2 ID 0x0B (X) → writes actor.x = -6010.
 *
 * Returns -1 for unknown IDs (treat as no-op). */
int re15_to_re2_member_id(int re15_id);

/* Plc_dest mode → motion clip selection.
 *
 * RE1.5 Plc_dest mode byte selects per-mode walker handler via PSX.EXE
 * dispatch table at 0x80073E40. Each per-mode handler writes a hardcoded
 * scalar speed to actor+0x8C and calls FUN_800245D8 (translate). They
 * also use per-mode motion_id LUTs at RAM 0x80073EA5 (mode 4 WALK),
 * 0x80073F25 (mode 5 RUN), etc.
 *
 * Per P9 agent dump, the LUT values are clip 0x30 (=48) for WALK, 0x48
 * (=72) for RUN, 0x60 (=96) for TURN. These don't fit in our 24-clip
 * PL00.EDD — RE1.5's actual EDD might have more clips OR the LUT is
 * indexed differently. Until we resolve, return our best-known WALK clip
 * for each mode.
 *
 * Returns the motion_id (clip index) to apply, or -1 to leave unchanged.
 * `with_rbj_overlay` flag selects between PL00 and rbj clip catalogs. */
int re15_to_re2_plc_dest_clip(int mode, int with_rbj_overlay);

/* Plc_dest mode → forward speed (PSX units per tick).
 *
 * From P9's empirical reading of handler 0x80030AF0 (forward-walk: speed
 * 0x4B=75 state-0, 0x60=96 state-1) and 0x80030D28 (run: 0xC8=200, 0x60).
 * We use the steady-state (state-1) value since the cinematic doesn't
 * implement the state machine.                                          */
int re15_to_re2_plc_dest_speed(int mode);

/* Message_on slot → subtitle text mapping for ROOM1170 cinematic. */
const char *re15_to_re2_room1170_subtitle(int message_id);

/*=========================================================================
 * MOTION CLIP CATALOG ADAPTER
 *=========================================================================*/

/* Resolve a "canonical motion role" to the actual clip index in our
 * active animation set. The room may have an rbj overlay active that
 * replaces the default PL00.EDD; this function picks the right clip
 * index from whichever catalog is active.
 *
 * Roles:
 *   0 = IDLE        1 = WALK loop    2 = RUN loop
 *   3 = TURN_LEFT   4 = TURN_RIGHT   5 = FALL
 *   6 = CLIMB       7 = WAVE / "Hey!" gesture
 *   8 = ARM_LIFT    9 = POINT       10 = HEAD_TURN                     */
typedef enum {
    RE15_MOTION_IDLE      = 0,
    RE15_MOTION_WALK      = 1,
    RE15_MOTION_RUN       = 2,
    RE15_MOTION_TURN      = 3,
    RE15_MOTION_FALL      = 5,
    RE15_MOTION_CLIMB     = 6,
    RE15_MOTION_WAVE      = 7,
    RE15_MOTION_ARM_LIFT  = 8,
    RE15_MOTION_POINT     = 9,
    RE15_MOTION_HEAD_TURN = 10,
} re15_motion_role_t;

int re15_to_re2_resolve_motion(re15_motion_role_t role, int with_rbj_overlay);

#endif /* RE15_TO_RE2_H */
