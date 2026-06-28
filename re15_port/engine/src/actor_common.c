/*
 * RE1.5 Rebuilt — Actor storage + member accessors (Phase 4.4.8, 2026-05-19).
 *
 * The member-id table mirrors RE2's actor-property dispatch table
 * (RE2_Quellcode/FUN_80055cb0.c). Each entry maps a SCD `Member_set`/
 * `Member_cmp` argument byte to a field in re15_actor_t.
 *
 * For the MVP demo we only handle ~10 property ids out of RE2's 44.
 * Unknown ids return 0 (read) or are no-ops (write) — matches RE2's
 * default-case behavior at FUN_80055f50.c:14.
 */

#include <string.h>
#include <stdio.h>
#include "re15_actor.h"
#include "re15_engine.h"

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

/* Property ids mirror RE2's PTR_LAB_800a74c8 → FUN_80055cb0 table.
 * 44 entries verified by F1 agent on 2026-05-21. Only IDs we use are
 * mapped; everything else falls through to the default 0/no-op handler.
 *
 * RE2 ID → field map (canonical):
 *   0x00 flags         0x10 rot_z       0x1A vel_w (angular)
 *   0x02 state         0x11 floor       0x20 target_x
 *   0x03 sub_state_1   0x13 hp          0x21 target_y
 *   0x04 sub_state_2   0x16 speed_h     0x22 target_z
 *   0x05 sub_state_3   0x17 vel_x       0x23 lookat_x
 *   0x06 sub_state_4   0x18 vel_y       0x24 lookat_y
 *   0x08 motion        0x19 vel_z       0x25 lookat_z
 *   0x0B x  0x0C y  0x0D z  0x0E rot_x  0x0F rot_y                      */
int32_t re15_actor_get_member(int slot, uint8_t member_id)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    const re15_actor_t *a = &g_actors[slot];
    /* member_id is a RE2 id (callers translate RE1.5→RE2 via re15_to_re2_member_id,
     * which never emits 0x00-0x05 — so these low cases are direct-call/future safe).
     * Map = FUN_80055f50 + the struct field comments. (2026-06-09: the old
     * 0x00=X / 0x02=Z / 0x03=rot_y were WRONG RE2 ids — a BioClone bolt-on, dead via
     * translation but a latent hazard; corrected to flags / state / sub_state_1.) */
    switch (member_id) {
    case 0x00: return (int32_t)a->flags;        /* RE2 +0x00 flags */
    case 0x02: return (int32_t)a->state;        /* RE2 +0x04 state */
    case 0x03: return (int32_t)a->sub_state_1;  /* RE2 +0x05 sub_state_1 */
    case 0x04: return (int32_t)a->sub_state_2;
    case 0x05: return (int32_t)a->sub_state_3;
    case 0x06: return (int32_t)a->sub_state_4;
    case 0x08: return (int32_t)a->motion;
    case 0x0B: return a->x;       /* RE2-style alias */
    case 0x0C: return a->y;
    case 0x0D: return a->z;
    case 0x0E: return (int32_t)a->rot_x;
    case 0x0F: return (int32_t)a->rot_y;
    case 0x10: return (int32_t)a->rot_z;
    case 0x11: return (int32_t)a->floor;
    case 0x13: return (int32_t)a->hp;
    case 0x16: return (int32_t)a->speed_h;
    case 0x17: return (int32_t)a->vel_x;
    case 0x18: return (int32_t)a->vel_y;
    case 0x19: return (int32_t)a->vel_z;
    case 0x1A: return (int32_t)a->vel_w;
    case 0x20: return (int32_t)a->target_x;
    case 0x21: return (int32_t)a->target_y;
    case 0x22: return (int32_t)a->target_z;
    case 0x23: return (int32_t)a->lookat_x;
    case 0x24: return (int32_t)a->lookat_y;
    case 0x25: return (int32_t)a->lookat_z;
    default: return 0;
    }
}

void re15_actor_set_member(int slot, uint8_t member_id, int32_t value)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *a = &g_actors[slot];
#ifdef RE15_PLATFORM_PC
    if (slot == 0 && member_id == 0x0C) {   /* RE2 id 0x0C = world Y */
        fprintf(stderr, "[Y-TRACE F%u] Member_set Leon Y: %d → %d (m=0x%02X)\n",
                (unsigned)g_engine.frame_count, a->y, value, (unsigned)member_id);
    }
#endif
    /* member_id is a RE2 id (callers translate RE1.5→RE2 via re15_to_re2_member_id;
     * coords arrive as 0x0B/0x0C/0x0D). Map = FUN_80055cb0 + the struct field
     * comments. (2026-06-09: dropped the BioClone 0x00=X / 0x01=Y / 0x02=Z /
     * 0x03=rot_y bolt-ons — wrong RE2 ids, dead via translation; 0x00/0x02/0x03
     * are now the canonical flags / state / sub_state_1.) */
    switch (member_id) {
    case 0x00: a->flags        = (uint8_t)value; break;   /* RE2 +0x00 flags */
    case 0x02: a->state        = (uint8_t)value; break;   /* RE2 +0x04 state */
    case 0x03: a->sub_state_1  = (uint8_t)value; break;   /* RE2 +0x05 sub_state_1 */
    case 0x04: a->sub_state_2  = (uint8_t)value; break;
    case 0x05: a->sub_state_3  = (uint8_t)value; break;
    case 0x06: a->sub_state_4  = (uint8_t)value; break;
    case 0x08: re15_actor_set_motion(a, (int16_t)value); break;
    case 0x0B: a->x            = value;          break;   /* X (RE2 alias) */
    case 0x0C: a->y            = value;          break;
    case 0x0D: a->z            = value;          break;
    case 0x0E: a->rot_x        = (int16_t)value; break;
    case 0x0F: a->rot_y        = (int16_t)value; break;
    case 0x10: a->rot_z        = (int16_t)value; break;
    case 0x11: a->floor        = (uint8_t)value; break;
    case 0x13: a->hp           = (int16_t)value; break;
    case 0x16: a->speed_h      = (int16_t)value; break;
    case 0x17: a->vel_x        = (int16_t)value; break;
    case 0x18: a->vel_y        = (int16_t)value; break;
    case 0x19: a->vel_z        = (int16_t)value; break;
    case 0x1A: a->vel_w        = (int16_t)value; break;
    case 0x20: a->target_x     = (int16_t)value; break;
    case 0x21: a->target_y     = (int16_t)value; break;
    case 0x22: a->target_z     = (int16_t)value; break;
    case 0x23: a->lookat_x     = (int16_t)value; break;
    case 0x24: a->lookat_y     = (int16_t)value; break;
    case 0x25: a->lookat_z     = (int16_t)value; break;
    default: /* unknown id — silently ignore */ break;
    }
}
