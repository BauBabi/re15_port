/*
 * RE1.5 Rebuilt — shared enemy-model registry (globalization 2026-06-13).
 * See re15_enemy.h. Pure table management; the actual EMD load is per-port.
 */
#include <string.h>
#include <stdlib.h>
#include "re15_enemy.h"

re15_enemy_bank_t g_enemy[RE15_ENEMY_MAX];

re15_enemy_bank_t *re15_enemy_find(uint8_t type)
{
    if (type == 0) return NULL;
    for (int i = 0; i < RE15_ENEMY_MAX; i++)
        if (g_enemy[i].ok && g_enemy[i].type == type)
            return &g_enemy[i];
    return NULL;
}

re15_enemy_bank_t *re15_enemy_alloc(uint8_t type)
{
    if (type == 0) return NULL;
    for (int i = 0; i < RE15_ENEMY_MAX; i++) {
        if (!g_enemy[i].ok && g_enemy[i].type == 0) {
            memset(&g_enemy[i], 0, sizeof(g_enemy[i]));
            g_enemy[i].type        = type;
            g_enemy[i].tpage       = -1;
            g_enemy[i].clut        = -1;
            g_enemy[i].xshift      = -1;
            g_enemy[i].pc_tex_slot = -1;
            return &g_enemy[i];
        }
    }
    return NULL;   /* table full */
}

void re15_enemy_reset(void)
{
    for (int i = 0; i < RE15_ENEMY_MAX; i++) {
        if (g_enemy[i].buf) free(g_enemy[i].buf);   /* PC malloc'd buffer; PSX leaves NULL */
        memset(&g_enemy[i], 0, sizeof(g_enemy[i]));
        g_enemy[i].pc_tex_slot = -1;
    }
}

/* Room-cinematic overlay — see re15_enemy.h. PC-port only (its boot + cross-room reload);
 * the PSX keeps a hand-tuned inline copy in asset_psx.c (RAM-tight boot can't host this
 * fn's frame). Keep the overlay logic + the room→enemy table in parity with that inline. */
void re15_apply_room_cinematic(const uint8_t *rbj, size_t rbj_size, unsigned room_id,
        const re15_emd_skeleton_t *pl00_base,
        re15_emd_skeleton_t *leon_skel, re15_emd_animation_t *leon_anim,
        const re15_emd_skeleton_t *elliot_base, int elliot_ok,
        re15_emd_skeleton_t *elliot_skel, re15_emd_animation_t *elliot_anim,
        re15_emd_skeleton_t *scratch_skel, re15_emd_animation_t *scratch_anim,
        void (*ensure_enemy)(uint8_t type))
{
    if (!rbj || rbj_size == 0 || !scratch_skel || !scratch_anim) return;

    /* Scratch is CALLER-PROVIDED (re15_emd_animation_t carries frames[1664] = ~7 KB; it must
     * NOT be a stack local on the RAM-tight PSX, nor a permanent static). The PSX caller hands
     * in its free CD staging buffer (no new BSS, no deep-stack frame); the PC caller hands in a
     * stack/static buffer (it has the RAM). */
    re15_emd_skeleton_t  *s = scratch_skel;
    re15_emd_animation_t *a = scratch_anim;

    /* (1) Leon's cinematic-gesture overlay (Plc_motion clip indices = this room's bank). */
    if (pl00_base && leon_skel && leon_anim) {
        if (re15_emd_parse_rbj(rbj, rbj_size, pl00_base, s, a) == 0) {
            *leon_skel = *s; *leon_anim = *a;
        }
    }

    /* (2) Elliot's dialog-gesture overlay (room1170 intro). base==out aliasing is safe:
     * parse_rbj fully reads the base before we assign the result. */
    if (elliot_ok && elliot_base && elliot_skel && elliot_anim) {
        if (re15_emd_parse_rbj(rbj, rbj_size, elliot_base, s, a) == 0) {
            *elliot_skel = *s; *elliot_anim = *a;
        }
    }

    /* (3) Per-room RBJ→ENEMY rebind: a NON-idle RBJ record (RECORD 1+) overrides a named
     * entity's animation. ROOM1150 RECORD 1 = the prone wounded-breathing Irons bank onto
     * em45 (em45's own clips are all upright). Keep the em45 mesh+bind; replace kf+EDD. */
    static const struct { unsigned room; uint8_t type; int record; } s_room_rbj_enemy[] = {
        { 0x1150, 0x45, 1 },   /* Irons: animation.rbj RECORD 1 = prone wounded-breathing */
    };
    for (unsigned k = 0; k < sizeof(s_room_rbj_enemy)/sizeof(s_room_rbj_enemy[0]); k++) {
        if (s_room_rbj_enemy[k].room != room_id) continue;
        uint8_t etype = s_room_rbj_enemy[k].type;
        if (ensure_enemy && !re15_enemy_find(etype)) ensure_enemy(etype);
        re15_enemy_bank_t *eb = re15_enemy_find(etype);
        if (eb) {
            if (re15_emd_parse_rbj_record(rbj, rbj_size, s_room_rbj_enemy[k].record,
                                          &eb->skel, s, a) == 0) {
                eb->skel = *s; eb->anim = *a;
            }
        }
    }
}
