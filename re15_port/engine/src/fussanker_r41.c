/*
 * RE1.5 Rebuilt — Runde 41 Spur J: Glied-Anker FUN_800369f8 (Executor-Bits +0x1c4 & 1 / & 2).
 * Belege je Zeile im Header include/re15_fussanker_r41.h, Dossier analysis/befunde_runde41/J_stage3szenen_a.md §2.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_fussanker_r41.h"
#include "re15_actor.h"
#include "re15_enemy.h"        /* re15_enemy_find, re15_actor_rbj_skel/_anim */
#include "re15_skeleton.h"     /* re15_skel_compute_pose, g_skel_lokal_rot, g_anim_pose_actor, g_anim_kf_tween */
#include "re15_anim_select.h"  /* re15_actor_playback_slot */
#include "re15_engine.h"       /* g_engine.frame_count (Messzeile) */
#include "re15_room.h"         /* g_current_room_id */

typedef struct {
    int      ok;
    uint8_t  type;
    unsigned room;
    int32_t  w[2][3];          /* Welt-Translation Part 4 / Part 7 (Part+84/+88/+92 des Originals) */
} anker_alt_t;
static anker_alt_t s_alt[RE15_ACTOR_MAX];

#ifdef RE15_PLATFORM_PC
static int s_log = -1;
static int log_an(void)   /* Messzeile [fussanker41] nur mit RE15_FUSSANKER_LOG=1 (Spur-J-Riegel, kein Verhalten) */
{
    if (s_log < 0) { const char *v = getenv("RE15_FUSSANKER_LOG"); s_log = (v && *v && *v != '0') ? 1 : 0; }
    return s_log;
}
#endif

void re15_fussanker_r41_reset(void)
{
    memset(s_alt, 0, sizeof s_alt);
}

int re15_fussanker_r41_alt(int slot, int kette, int32_t out[3])
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || kette < 0 || kette > 1 || !s_alt[slot].ok) return 0;
    memcpy(out, s_alt[slot].w[kette], sizeof(int32_t) * 3);
    return 1;
}

/* Kanal je State-4-Sub — die anim_set-Paare direkt vor `jal 0x8001f314` (Tabelle 0x80076ca0):
 *   Sub 0 0x80050cb8 @0x80050d40/48 +0x180/+0x184 = RBJ-Binder-Record (FUN_8001b3f8)
 *   Sub 1 0x80050ddc @0x80050e64/6c +0x84/+0x16c  = Loco-Bank
 *   Sub 2 0x80050f00 @0x80050f88/90 +0x170/+0x174 = eigene Bank 1
 *   Sub 3 0x80051024                +0x178/+0x17c = Victim-Bank
 * (dieselbe Zuordnung wie der NPC-Zeichner in platform/pc/main.c, "EXECUTOR-KANAL-Render"). */
int re15_fussanker_r41_kanal(const re15_actor_t *e, const re15_emd_skeleton_t **sk, const re15_emd_animation_t **an)
{
    *sk = NULL; *an = NULL;
    if (!e || e->state != 4 || e->walk_active) return 0;
    int slot = (int)(e - g_actors);
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    switch (e->sub_state_1) {
    case 0: *sk = re15_actor_rbj_skel(slot); *an = re15_actor_rbj_anim(slot); break;
    case 1: if (b && b->loco_ok)   { *sk = &b->skel_loco;   *an = &b->anim_loco;   } break;
    case 2: if (b && b->own_ok)    { *sk = &b->skel_own;    *an = &b->anim_own;    } break;
    case 3: if (b && b->victim_ok) { *sk = &b->skel_victim; *an = &b->anim_victim; } break;
    default: break;
    }
    if (!*sk || !*an || (*an)->clip_count <= 0 || (*sk)->keyframe_count <= 0) return 0;
    if ((int)e->motion < 0 || (int)e->motion >= (*an)->clip_count) return 0;   /* Zeichner: motion < clip_count */
    if ((*sk)->bone_count <= RE15_FUSSANKER_R41_KETTE_ENDE1) return 0;          /* Parts 0..7 muessen da sein */
    return 1;
}

/* Lokale Pose des aktuellen Bildes (Abfrage-Modus: ohne Ueberblendung/Nacken — die Anker-Bits stehen in ROOM3061 erst
 * ab Bild 20 bzw. 8 nach dem Clipwechsel, die 7-Bilder-Ueberblendung (+0x8f = 7, @0x80050cfc) ist dann abgelaufen).
 * Bild -> Keyframe wie re15_compute_actor_kf (Kanal-Override-Zweig: re15_actor_playback_slot, Halt bei anim_freeze,
 * 0x8000-Marker als Tween). */
static int pose(const re15_actor_t *e, const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an,
                int32_t lokal[RE15_EMD_MAX_BONES][9], re15_skel_pose_t poses[RE15_EMD_MAX_BONES])
{
    const re15_emd_clip_t *c = &an->clips[(int)e->motion];
    if (c->frame_count <= 0) return 0;
    int fs = e->anim_freeze ? 0 : re15_actor_playback_slot(e, (uint32_t)e->anim_frame, c->frame_count);
    uint32_t w = an->frames[c->first_frame + fs];
    g_anim_kf_tween.active = 0;
    if (w & 0x8000u) {
        int fc = c->frame_count, before = fs, after = fs;
        for (int n = 0; n < fc && (an->frames[c->first_frame + before] & 0x8000u); n++) before = (before + fc - 1) % fc;
        for (int n = 0; n < fc && (an->frames[c->first_frame + after]  & 0x8000u); n++) after  = (after + 1) % fc;
        int kf_from = (int)(an->frames[c->first_frame + before] & 0xFFFu);
        if (kf_from >= sk->keyframe_count) kf_from = sk->keyframe_count - 1;
        g_anim_kf_tween.active = 1; g_anim_kf_tween.kf_from = kf_from; g_anim_kf_tween.weight = (int)(w & 0x0FFFu);
        w = an->frames[c->first_frame + after];
    }
    int kf = (int)(w & 0xFFFu);
    if (kf >= sk->keyframe_count) kf = sk->keyframe_count - 1;
    void *pa = g_anim_pose_actor;              g_anim_pose_actor = NULL;
    int32_t (*lr)[9] = g_skel_lokal_rot;       g_skel_lokal_rot = lokal;
    int rc = re15_skel_compute_pose(sk, kf, poses);
    g_skel_lokal_rot = lr;                     g_anim_pose_actor = pa;
    g_anim_kf_tween.active = 0;
    return rc == 0;
}

/* [R|t] := [R|t] o [L|u] wie 0x80022da0: R = R x L (je Element >> 12), t = t + (R x u) >> 12. */
static void verketten(int32_t R[9], int32_t t[3], const int32_t L[9], const int32_t u[3])
{
    int32_t n[9];
    for (int r = 0; r < 3; r++)
        for (int k = 0; k < 3; k++)
            n[r * 3 + k] = (int32_t)(((int64_t)R[r * 3 + 0] * L[0 * 3 + k] + (int64_t)R[r * 3 + 1] * L[1 * 3 + k] +
                                      (int64_t)R[r * 3 + 2] * L[2 * 3 + k]) >> 12);
    int32_t v[3];
    for (int r = 0; r < 3; r++)
        v[r] = (int32_t)(((int64_t)R[r * 3 + 0] * u[0] + (int64_t)R[r * 3 + 1] * u[1] +
                          (int64_t)R[r * 3 + 2] * u[2]) >> 12);
    t[0] += v[0]; t[1] += v[1]; t[2] += v[2];
    memcpy(R, n, sizeof n);
}

/* Entity-Matrix: RotMatrix(+0x68) @0x80036a34 (`jal 0x80068098`), Translation = +0x34/+0x38/+0x3c. */
static void entity_matrix(const re15_actor_t *e, int32_t R[9], int32_t t[3])
{
    re15_skel_euler_matrix((int)e->rot_x, (int)e->rot_y, (int)e->rot_z, R);
    t[0] = e->x; t[1] = e->y; t[2] = e->z;
}

/* Lokale Translation eines Parts: Part 0 = Keyframe-Wurzel (FUN_8001f3bc), sonst EMR-Relativlage (Binder). */
static void lokal_t(const re15_emd_skeleton_t *sk, const re15_skel_pose_t *poses, int p, int32_t u[3])
{
    if (p == 0) { u[0] = poses[0].trans[0]; u[1] = poses[0].trans[1]; u[2] = poses[0].trans[2]; return; }
    u[0] = sk->bone_relative_pos[p][0]; u[1] = sk->bone_relative_pos[p][1]; u[2] = sk->bone_relative_pos[p][2];
}

int re15_fussanker_r41_kette(const re15_actor_t *e, const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an,
                             int kette, int32_t out[3])
{
    static int32_t lokal[RE15_EMD_MAX_BONES][9];
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    if (!e || !sk || !an || kette < 0 || kette > 1 || sk->bone_count <= RE15_FUSSANKER_R41_KETTE_ENDE1) return 0;
    if ((int)e->motion < 0 || (int)e->motion >= an->clip_count) return 0;
    if (!pose(e, sk, an, lokal, poses)) return 0;
    const int teile[5] = { 0, 1, 2 + 3 * kette, 3 + 3 * kette, 4 + 3 * kette };   /* @0x80036a48..a9c */
    int32_t R[9], t[3], u[3];
    entity_matrix(e, R, t);
    for (int i = 0; i < 5; i++) { lokal_t(sk, poses, teile[i], u); verketten(R, t, lokal[teile[i]], u); }
    memcpy(out, t, sizeof t);
    return 1;
}

int re15_fussanker_r41_teil_matrix(const re15_actor_t *e, const re15_emd_skeleton_t *sk,
                                   const re15_emd_animation_t *an, int part, int32_t R[9], int32_t t[3])
{
    static int32_t lokal[RE15_EMD_MAX_BONES][9];
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    if (!e || !sk || !an || part < 0 || part >= sk->bone_count) return 0;
    if ((int)e->motion < 0 || (int)e->motion >= an->clip_count) return 0;
    if (!pose(e, sk, an, lokal, poses)) return 0;
    int kette[RE15_EMD_MAX_BONES], n = 0;
    for (int p = part; n < RE15_EMD_MAX_BONES; ) {                    /* Eltern-Kette bis zur Wurzel (FUN_8001e8c8) */
        kette[n++] = p;
        int q = (int)sk->bone_parent[p];
        if (q < 0 || q >= p) break;                                    /* Wurzel-Regel wie re15_skel_compute_pose */
        p = q;
    }
    int32_t u[3];
    entity_matrix(e, R, t);
    for (int i = n - 1; i >= 0; i--) {
        int p = kette[i];
        if (i == n - 1) { u[0] = poses[0].trans[0]; u[1] = poses[0].trans[1]; u[2] = poses[0].trans[2]; }   /* Wurzel */
        else lokal_t(sk, poses, p, u);
        verketten(R, t, lokal[p], u);
    }
    return 1;
}

int re15_fussanker_r41_teil(const re15_actor_t *e, const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an,
                            int part, int32_t out[3])
{
    int32_t R[9];
    return re15_fussanker_r41_teil_matrix(e, sk, an, part, R, out);
}

void re15_fussanker_r41_merken_aktor(const re15_actor_t *e)
{
    if (!e) return;
    int s = (int)(e - g_actors);
    if (s <= 0 || s >= RE15_ACTOR_MAX) return;
    s_alt[s].ok = 0;
    if (!e->active) return;
    const re15_emd_skeleton_t *sk; const re15_emd_animation_t *an;
    if (!re15_fussanker_r41_kanal(e, &sk, &an)) return;
    if (!re15_fussanker_r41_teil(e, sk, an, RE15_FUSSANKER_R41_KETTE_ENDE0, s_alt[s].w[0])) return;
    if (!re15_fussanker_r41_teil(e, sk, an, RE15_FUSSANKER_R41_KETTE_ENDE1, s_alt[s].w[1])) return;
    s_alt[s].ok = 1; s_alt[s].type = e->type; s_alt[s].room = g_current_room_id;
}

void re15_fussanker_r41_merken(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) re15_fussanker_r41_merken_aktor(&g_actors[s]);
}

void re15_fussanker_r41_anwenden(re15_actor_t *e, int kette)
{
    if (!e || kette < 0 || kette > 1) return;
    int s = (int)(e - g_actors);
    if (s <= 0 || s >= RE15_ACTOR_MAX) return;
    const anker_alt_t *a = &s_alt[s];
    const re15_emd_skeleton_t *sk; const re15_emd_animation_t *an;
    if (!a->ok || a->type != e->type || a->room != g_current_room_id) return;   /* PORT-WAHL: kein Vorbild-Stand */
    if (!re15_fussanker_r41_kanal(e, &sk, &an)) return;
    int32_t neu[3];
    if (!re15_fussanker_r41_kette(e, sk, an, kette, neu)) return;
    int32_t x0 = e->x, z0 = e->z;
    e->x -= neu[0] - a->w[kette][0];                                  /* @0x80036ab4..ac8 */
    e->z -= neu[2] - a->w[kette][2];                                  /* @0x80036ad4..aec */
#ifdef RE15_PLATFORM_PC
    if (log_an())
        fprintf(stderr, "[fussanker41] F%u Aktor %d Typ 0x%02X Kette %d Clip %d Bild %d neu(%d,%d) alt(%d,%d) "
                        "pos (%d,%d) -> (%d,%d)\n", (unsigned)g_engine.frame_count, s, e->type, kette, (int)e->motion,
                (int)e->anim_frame, neu[0], neu[2], a->w[kette][0], a->w[kette][2], x0, z0, e->x, e->z);
#else
    (void)x0; (void)z0;
#endif
}
