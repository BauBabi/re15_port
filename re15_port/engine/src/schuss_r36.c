/*
 * schuss_r36.c — Runde 36 Spur C: Sichtlinie des Spieler-Schusses (RE1.5 FUN_8001b9b4).
 * Belege und Einordnung: include/re15_schuss_r36.h, Dossier analysis/befunde_runde36/C_schuss.md §1.
 */
#include "re15_schuss_r36.h"
#include "re15_room.h"       /* g_room_rdt_ok */

static int s_verworfen = 0;

int  re15_schuss_r36_sichtlinie_verworfen(void) { return s_verworfen; }
void re15_schuss_r36_sichtlinie_reset(void)     { s_verworfen = 0; }

int re15_schuss_r36_sichtlinie_blockiert(const re15_actor_t *pl, const re15_actor_t *ziel)
{
    int region;
    if (!pl || !ziel) return 0;
    if (!g_room_rdt_ok) return 0;                  /* keine SCA -> leere Regionen -> 0 (@0x8003dd94) */
    /* FUN_8001b9b4: `ori s0,zero,0x3` @0x8001ba08, Schleife `addiu s0,s0,-1` / `bne` @0x8001ba34-38,
     * je Region FUN_8003dcc4(delta, region, 0xf00, 0x300) @0x8001ba1c-24; das Ergebnis wird
     * ODER-verknuepft (`or s1,s1,v0` @0x8001ba28) — der erste Treffer entscheidet. Ursprung und
     * Band (+0x82) sind die des Spielers (DAT_800ac784, @0x8003dd00/@0x8003de60). */
    for (region = 3; region >= 0; region--) {
        if (re15_los_ray_blocked(pl, ziel, region, 0u)) {
            s_verworfen++;
            return 1;
        }
    }
    return 0;
}

/* ---- Punkt 2: Blut-Anker = Anker.R * a3 + Anker.T (Belege im Header) ------------------------- */
#include "re15_damage.h"       /* re15_enemy_bone_world_pos */
#include "re15_enemy.h"        /* re15_enemy_find */
#include "re15_skeleton.h"     /* re15_skel_compute_pose / g_anim_pose_actor / re15_sin_q12 */
#include "re15_anim_select.h"  /* re15_compute_actor_kf */

int re15_schuss_r36_blut_anker(const re15_actor_t *e, int bone, const int32_t a3[3], int32_t out[3])
{
    static re15_actor_t sh;                                   /* Schatten-Aktor (Muster enemy_ai_re2_zellenarm.c:462-82) */
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int32_t v[3], c, s;
    int kf, rv, r;
    if (!e || !out) return 0;
    re15_enemy_bone_world_pos(e, bone, out);                 /* Anker.T (Welt) aus der ROHEN Pose, inkl. Ragdoll */
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || b->skel.bone_count <= 0 || bone < 0 || bone >= b->skel.bone_count) return 1;
    if (e->re2z_rag_anchor_on) {                             /* abgetrennte Unterhaelfte: Anker wie bisher */
        if (!a3 || (a3[0] == 0 && a3[1] == 0 && a3[2] == 0)) return 1;
        void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
        rv = re15_skel_compute_pose(&b->skel, re15_compute_actor_kf(&b->anim, &b->skel, e, -1, e->anim_frame), poses);
        g_anim_pose_actor = save;
        if (rv != 0) return 1;
    } else {
        /* Das Original liest die Matrix, die der ZEICHNER geschrieben hat (Record +0x48, FUN_8001e8c8 im
         * Vorbild) — also die CROSSFADE-geblendete Pose (FUN_8001f3bc), nicht das rohe Keyframe. Der Port
         * blendet nur mit gesetztem g_anim_pose_actor und schreibt dabei den Blend-Zustand zurueck; eine
         * KOPIE des Aktors liefert dieselbe Pose, die der Renderer in diesem Bild zeichnet, ohne den
         * Zustand des echten Aktors zu veraendern (gemessen probe_r36_blut: roh vs. gezeichnet bis 98
         * Einheiten im zweiten Treffer-Clip). */
        sh = *e;
        kf = re15_compute_actor_kf(&b->anim, &b->skel, &sh, -1, e->anim_frame);
        {   void *save = g_anim_pose_actor; g_anim_pose_actor = &sh;
            rv = (kf >= 0) ? re15_skel_compute_pose(&b->skel, kf, poses) : -1;
            g_anim_pose_actor = save;
        }
        if (rv != 0) return 1;
        re15_skel_bone_to_world(poses[bone].trans, e->rot_y, e->x, e->y, e->z, out);   /* Anker.T gezeichnet */
        if (!a3 || (a3[0] == 0 && a3[1] == 0 && a3[2] == 0)) return 1;
    }
    /* R_part * a3 (Q12, GTE ApplyMatrix-Zwilling FUN_800661c0 @0x8001a248) ... */
    for (r = 0; r < 3; r++)
        v[r] = (int32_t)(((int64_t)poses[bone].rot[r*3+0] * a3[0] + (int64_t)poses[bone].rot[r*3+1] * a3[1] +
                          (int64_t)poses[bone].rot[r*3+2] * a3[2]) >> 12);
    /* ... dann in die Welt des Aktors (Ryaw wie re15_skel_bone_to_world: Netz blickt +X bei Gier 0). */
    c = re15_cos_q12(e->rot_y); s = re15_sin_q12(e->rot_y);
    out[0] += (int32_t)(((int64_t)c * v[0] + (int64_t)s * v[2]) >> 12);
    out[1] += v[1];
    out[2] += (int32_t)(((int64_t)-s * v[0] + (int64_t)c * v[2]) >> 12);
    return 2;
}
