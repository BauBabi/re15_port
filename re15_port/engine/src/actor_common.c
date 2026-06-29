/*
 * RE1.5 Rebuilt — Actor storage + member accessors (Phase 4.4.8, 2026-05-19).
 *
 * The member-id table is the BYTE-TRUE RE1.5 table from the engine's own
 * member cores FUN_8004116c (set) / FUN_80041358 (get) — ids 0..0x13 map
 * DIRECTLY to actor offsets. (Earlier this mirrored RE2's FUN_80055cb0 via a
 * RE1.5→RE2 translation, which wrote Member id12 → Leon.y and mis-mapped ids
 * 6-19; corrected [#11/#12].) Each entry maps a SCD `Member_set`/`Member_cmp`
 * id byte to a field in re15_actor_t. Ids >= 0x14 → 0 (read) / no-op (write),
 * matching the original's range guard (sltiu id,0x14).
 */

#include <string.h>
#include <stdio.h>
#include "re15_actor.h"
#include "re15_engine.h"
#include "re15_damage.h"   /* re15_player_apply_hitbox — wire the player's +0x78 hitbox */

re15_actor_t g_actors[RE15_ACTOR_MAX];
uint8_t      g_actor_count = 0;

void re15_actor_init(void)
{
    memset(g_actors, 0, sizeof(g_actors));
    /* Slot 0 = player, always active (RE2-pure). Main initialises x/y/z
     * from the room's spawn-entry data (stage-transition or fallback). */
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].type   = 0;     /* PLAYER = type 0 */
    g_actors[RE15_ACTOR_SLOT_PLAYER].flags  = 0x01;  /* visible */
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp     = 100;   /* nominal */
    /* Root-motion delta tracker starts uninitialized so first tick just
     * records the current keyframe without applying any delta. */
    g_actors[RE15_ACTOR_SLOT_PLAYER].root_prev_kf     = -1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].root_prev_motion = -1;
    /* Byte-true player attack-hitbox (EXE-static @0x80073e94): radius 450, height 1530. */
    re15_player_apply_hitbox(&g_actors[RE15_ACTOR_SLOT_PLAYER]);
    g_actor_count = 1;
}

int re15_actor_alloc(uint8_t type)
{
    /* Skip slot 0 (reserved for player). NPCs spawn in 1..N-1. */
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        if (!g_actors[i].active) {
            memset(&g_actors[i], 0, sizeof(g_actors[i]));
            g_actors[i].active = 1;
            g_actors[i].type   = type;
            g_actors[i].flags  = 0x01;
            if ((uint8_t)(i + 1) > g_actor_count) {
                g_actor_count = (uint8_t)(i + 1);
            }
            return i;
        }
    }
    return -1;
}

void re15_actor_free(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    g_actors[slot].active = 0;
    /* Don't decrement g_actor_count — keeps HUD steady. Next alloc will
     * reuse the slot. */
}

/* RE1.5 SCD Member accessor — byte-true id→field table from the engine's OWN
 * member cores (NO RE2 translation; the RE2 id-map wrote Member id12 → Leon.y =
 * "player under the floor", and mis-mapped/dropped ids 6-19 [#11/#12]):
 *   SET = FUN_8004116c @0x8004116c (ghidra1_V2.txt:151322-151380)
 *   GET = FUN_80041358 @0x80041358 (ghidra1_V2.txt:151486-151575)
 * id : engine offset (store/load)            → port field
 *   0  +0x34 sw/lw   x          8  +0x04 sb/lbu  state
 *   1  +0x38 sw/lw   y          9  +0x05 sb/lbu  sub_state_1
 *   2  +0x3c sw/lw   z          10 +0x06 sb/lbu  sub_state_2
 *   3  +0x68 sh/lh   rot_x      11 +0x07 sb/lbu  sub_state_3
 *   4  +0x6a sh/lh   rot_y      12 +0x09 sb/lbu  grid_id   (most frequent Member_set)
 *   5  +0x6c sh/lh   rot_z      13 +0x08 sb/lbu  sub_state_4
 *   6  +0x00 sw/lw   flags      14 +0x0a sb/lbu  member_0a
 *   7  +0x0c sw/lw   member_0c  15 +0x0b sb/lbu  member_0b
 *   16 +0x1c4 sh/lhu anim_flags 18 +0x82  sb/lbu floor
 *   17 +0x98  sh/lhu status_flags 19 +0x1ba sh/lh hp
 * GET signedness: lbu/lhu zero-extend, lh sign-extend, lw = full s32. The port
 * field TYPES match those widths, so (int32_t)field reproduces the original GET
 * sign exactly. NOTE: RE1.5 has NO Member id for `motion` (set via Plc_motion →
 * +0x94), so the old id8→motion mapping was wrong — id8 = state. */
int32_t re15_actor_get_member(int slot, uint8_t member_id)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    const re15_actor_t *a = &g_actors[slot];
    switch (member_id) {                          /* FUN_80041358 (sltiu id,0x14) */
    case 0:  return a->x;                          /* +0x34 lw  */
    case 1:  return a->y;                          /* +0x38 lw  */
    case 2:  return a->z;                          /* +0x3c lw  */
    case 3:  return (int32_t)a->rot_x;             /* +0x68 lh  */
    case 4:  return (int32_t)a->rot_y;             /* +0x6a lh  */
    case 5:  return (int32_t)a->rot_z;             /* +0x6c lh  */
    case 6:  return (int32_t)a->flags;             /* +0x00 lw (subset: low byte) */
    case 7:  return a->member_0c;                  /* +0x0c lw  */
    case 8:  return (int32_t)a->state;             /* +0x04 lbu */
    case 9:  return (int32_t)a->sub_state_1;       /* +0x05 lbu */
    case 10: return (int32_t)a->sub_state_2;       /* +0x06 lbu */
    case 11: return (int32_t)a->sub_state_3;       /* +0x07 lbu */
    case 12: return (int32_t)a->grid_id;           /* +0x09 lbu */
    case 13: return (int32_t)a->sub_state_4;       /* +0x08 lbu */
    case 14: return (int32_t)a->member_0a;         /* +0x0a lbu */
    case 15: return (int32_t)a->member_0b;         /* +0x0b lbu */
    case 16: return (int32_t)a->anim_flags;        /* +0x1c4 lhu */
    case 17: return (int32_t)a->status_flags;      /* +0x98 lhu */
    case 18: return (int32_t)a->floor;             /* +0x82 lbu */
    case 19: return (int32_t)a->hp;                /* +0x1ba lh */
    default: return 0;                             /* id>=20 → default (FUN_80041358 LAB_8004146c) */
    }
}

void re15_actor_set_member(int slot, uint8_t member_id, int32_t value)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *a = &g_actors[slot];
    switch (member_id) {                           /* FUN_8004116c (sltiu id,0x14) */
    case 0:  a->x            = value;          break;  /* +0x34 sw */
    case 1:  a->y            = value;          break;  /* +0x38 sw */
    case 2:  a->z            = value;          break;  /* +0x3c sw */
    case 3:  a->rot_x        = (int16_t)value; break;  /* +0x68 sh */
    case 4:  a->rot_y        = (int16_t)value; break;  /* +0x6a sh */
    case 5:  a->rot_z        = (int16_t)value; break;  /* +0x6c sh */
    case 6:  a->flags        = (uint8_t)value; break;  /* +0x00 sw (subset: low byte) */
    case 7:  a->member_0c    = value;          break;  /* +0x0c sw */
    case 8:  a->state        = (uint8_t)value; break;  /* +0x04 sb */
    case 9:  a->sub_state_1  = (uint8_t)value; break;  /* +0x05 sb */
    case 10: a->sub_state_2  = (uint8_t)value; break;  /* +0x06 sb */
    case 11: a->sub_state_3  = (uint8_t)value; break;  /* +0x07 sb */
    case 12: a->grid_id      = (uint8_t)value; break;  /* +0x09 sb — NOT Y (the old bug) */
    case 13: a->sub_state_4  = (uint8_t)value; break;  /* +0x08 sb */
    case 14: a->member_0a    = (uint8_t)value; break;  /* +0x0a sb */
    case 15: a->member_0b    = (uint8_t)value; break;  /* +0x0b sb */
    case 16: a->anim_flags   = (uint16_t)value; break; /* +0x1c4 sh */
    case 17: a->status_flags = (uint16_t)value; break; /* +0x98 sh */
    case 18: a->floor        = (uint8_t)value; break;  /* +0x82 sb */
    case 19: a->hp           = (int16_t)value; break;  /* +0x1ba sh */
    default: /* id>=20 → no-op */               break;
    }
}
