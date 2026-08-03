/*
 * RE1.5 Rebuilt — shared enemy-model registry (globalization 2026-06-13).
 * See re15_enemy.h. Pure table management; the actual EMD load is per-port.
 */
#include <string.h>
#include <stdlib.h>
#include "re15_enemy.h"
#include "re15_enemy_ai.h"   /* re15_player_victim_reset */
#include "re15_actor.h"      /* RBJ-Marker-Binder: Aktor-Slots (g_actors) */

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

/* ===== RBJ-MARKER-BINDER — byte-true FUN_8001b3f8 (marvin_10d0.md D2, adversarial verifiziert) =====
 * Der Raum-Setup ruft den Binder NACH den SCD-Spawns (jal @0x80039a08, nach FUN_8003ef6c): er
 * iteriert die RBJ-Record-Trailer und bindet jeden Record per MARKER-BITMASKE (u32 @EMR-Prefix)
 * an seine Entity — Bit 0 = Spieler, Bit (1+i) = Enemy-Entity i (Stride i*0x1F4 @0x8001b480-90).
 * ZIEL-Kanal = entity+0x180/+0x184 (Player 0x800acbd4/acbd8, Enemy 0x800acdac/acdb0+i*0x1F4) —
 * der Kanal, den NUR Executor-Sub 0 spielt (@0x80050d40/48); die eigene Bank +0x170/+0x174
 * bleibt unberuehrt (Verify-D2-Korrektur — KEIN Wholesale-Bank-Replace). Datengetrieben, keine
 * Raum-Tabelle. Port: lazy pro Aktor-Slot aufgeloest (die Spawns laufen im Port nach dem
 * Cinematic-Load), Cache unten; Raum-RBJ wird vom Platform-Loader registriert.
 * ⚠ ROOM1170/Elliot-AUSNAHME (O3): Typ 0x47 wird NICHT gebunden — der Marker sagt REC1, der
 * Port spielt bewusst Elliots eigene Bank (Clip 25 nur dort; Original-OOB ungemessen). */
static const uint8_t *s_room_rbj      = NULL;
static size_t         s_room_rbj_size = 0;

#define RE15_RBJ_CACHE_MAX 2
static struct {
    int  slot;                    /* Aktor-Slot (-1 = frei) */
    int  valid;
    re15_emd_skeleton_t  skel;    /* Entity-Hierarchie + Record-Keyframe-Pool */
    re15_emd_animation_t anim;    /* Record-EDD */
} s_rbj_cache[RE15_RBJ_CACHE_MAX] = { { -1, 0, {0}, {0} }, { -1, 0, {0}, {0} } };

void re15_rbj_bind_room(const uint8_t *rbj, size_t size)
{
    s_room_rbj = rbj; s_room_rbj_size = size;
    for (int i = 0; i < RE15_RBJ_CACHE_MAX; i++) { s_rbj_cache[i].slot = -1; s_rbj_cache[i].valid = 0; }
}

/* Marker-Wort des Records r (u32 @trailer[r].EMR_prefix — die 4 Bytes, die parse_rbj_record
 * mit prefix+4 ueberspringt). -1 = kein Record/kein RBJ. */
static uint32_t rbj_record_marker(int r)
{
    if (!s_room_rbj || s_room_rbj_size < 0x20) return 0;
    uint32_t total = (uint32_t)(s_room_rbj[0] | (s_room_rbj[1] << 8) |
                                (s_room_rbj[2] << 16) | ((uint32_t)s_room_rbj[3] << 24));
    uint32_t nrec  = s_room_rbj[4];
    if (total == 0 || total > s_room_rbj_size || nrec == 0 || nrec > 8) return 0;
    if ((uint32_t)r >= nrec) return 0;
    size_t toff = (size_t)total + (size_t)r * 8;
    if (toff + 8 > s_room_rbj_size) return 0;
    uint32_t prefix = (uint32_t)(s_room_rbj[toff] | (s_room_rbj[toff+1] << 8) |
                                 (s_room_rbj[toff+2] << 16) | ((uint32_t)s_room_rbj[toff+3] << 24));
    if (prefix == 0 || (size_t)prefix + 4 > s_room_rbj_size) return 0;
    return (uint32_t)(s_room_rbj[prefix] | (s_room_rbj[prefix+1] << 8) |
                      (s_room_rbj[prefix+2] << 16) | ((uint32_t)s_room_rbj[prefix+3] << 24));
}

static int rbj_resolve_slot(int slot)
{
    for (int i = 0; i < RE15_RBJ_CACHE_MAX; i++)
        if (s_rbj_cache[i].slot == slot) return i;
    if (!s_room_rbj || slot <= 0 || slot >= RE15_ACTOR_MAX) return -1;
    const re15_actor_t *e = &g_actors[slot];
    if (!e->active || e->type == 0x47) return -1;         /* Elliot-Ausnahme (O3) */
    /* Marker-Bit (1 + Enemy-Index); Port-Aktor-Slot = 1 + Enemy-Index -> Bit == slot. */
    int rec = -1;
    for (int r = 0; r < 8; r++)
        if (rbj_record_marker(r) & (1u << slot)) { rec = r; break; }
    /* negativ cachen (Slot ohne Record), damit der Scan nicht jeden Frame laeuft */
    int ci = -1;
    for (int i = 0; i < RE15_RBJ_CACHE_MAX; i++)
        if (s_rbj_cache[i].slot < 0) { ci = i; break; }
    if (ci < 0) return -1;
    s_rbj_cache[ci].slot = slot; s_rbj_cache[ci].valid = 0;
    if (rec >= 0) {
        re15_enemy_bank_t *eb = re15_enemy_find(e->type);  /* Basis-Hierarchie = die eigene Bank */
        if (eb && eb->ok &&
            re15_emd_parse_rbj_record(s_room_rbj, s_room_rbj_size, rec, &eb->skel,
                                      &s_rbj_cache[ci].skel, &s_rbj_cache[ci].anim) == 0)
            s_rbj_cache[ci].valid = 1;
    }
    return ci;
}

const re15_emd_animation_t *re15_actor_rbj_anim(int slot)
{
    int ci = rbj_resolve_slot(slot);
    return (ci >= 0 && s_rbj_cache[ci].valid) ? &s_rbj_cache[ci].anim : NULL;
}
const re15_emd_skeleton_t *re15_actor_rbj_skel(int slot)
{
    int ci = rbj_resolve_slot(slot);
    return (ci >= 0 && s_rbj_cache[ci].valid) ? &s_rbj_cache[ci].skel : NULL;
}

void re15_enemy_reset(void)
{
    extern void re15_enemy_spawn_count_reset(void);
    re15_enemy_spawn_count_reset();          /* DAT_800aca4e room-init reset (@0x8003f014) */
    for (int i = 0; i < RE15_ENEMY_MAX; i++) {
        if (g_enemy[i].buf) free(g_enemy[i].buf);   /* PC malloc'd buffer; PSX leaves NULL */
        memset(&g_enemy[i], 0, sizeof(g_enemy[i]));
        g_enemy[i].pc_tex_slot = -1;
    }
    re15_rbj_bind_room(NULL, 0);  /* Raum-RBJ + Kanal-Cache fallen mit den Banks (Raumwechsel) */
    re15_player_victim_reset();   /* clear Leon's grab-victim anim state (banks just dropped) */
    { extern void re15_crow_flock_reset(void); re15_crow_flock_reset(); }  /* 0x800aca50 = 0 on room change (FUN_8010d13c) */
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
