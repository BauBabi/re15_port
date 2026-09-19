/* probe_p3_gator_maul.c — MESS-SONDE Phase 3 (2026-09-19), kein add_test.
 *
 * Offener Punkt 1 des Dossiers analysis/befunde_2026-09-19/gator-finisher-sounds.md
 * ("Offen": "Leon bleibt 1x gross im 2/3-Maul (Scale 2731 = Nutzer-Entscheidung ohne
 * @0x); der SITZ der Wurzel ist jetzt die Original-Relation, die Proportion nicht.").
 *
 * FRAGE, die diese Sonde beantwortet: WIE GROSS ist Leon relativ zum Maul-Mesh?
 * Metrik (dieselbe wie probe_r16_gator_finisher_anker, nur statt der WURZEL der ganze
 * KOERPER): jeder gerenderte Leon-Vertex (PL00-MD1-Mesh, Victim-Keyframes im
 * PL00-Skelett, Renderpfad main.c:7338-7375) wird in den gator-LOKALEN Rahmen
 * zurueckgerechnet   local = RotY(-yaw_g) * (v_welt - G) / s_g
 * und gegen die lokalen AABBs von MD1-Mesh 6 (Oberkiefer) / 7 (Unterkiefer) der
 * GEPOSTEN Gator-Kette geprueft. In diesem Rahmen ist die Gator-Geometrie
 * massstabsfrei — Leon dagegen erscheint um 4096/s_g VERGROESSERT, solange er selbst
 * ungeskaliert gezeichnet wird. Genau das misst Teil 2.
 *
 * Teil 1: Gator-Maul-Mass (Clip 11 f0 = P3-Kau-Loop, + Clip 5 f0/f60 = P2).
 * Teil 2: Leons Koerper-AABB im selben Rahmen, fuer Spieler-Scale 4096 (heute) und
 *         2731 (= GB_SCALE_Q12, Vorschlag), inkl. Vertex-Trefferquote im Maulraum.
 * Teil 3: der ECHTE Boss-Tick in ROOM2090 (RE15_GB_TEST=1) — dieselbe Messung am
 *         Live-Zustand, damit die Zahl nicht an einem nachgebauten Zustand haengt.
 */
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_collision.h"
#include "re15_boss_gator.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz;
    return b;
}

static re15_enemy_bank_t B;                    /* RE2 EM23 (Gator + Opfer-Paar 3) */
static re15_emd_skeleton_t  PL_SKEL;           /* PL00.EMR */
static re15_emd_animation_t PL_ANIM;           /* PL00.EDD (nur fuer die Vollstaendigkeit) */
static re15_md1_t           PL_MD1;            /* PL00.MD1 */

static int kf_of(const re15_emd_animation_t *an, int clip, int fr)
{
    const re15_emd_clip_t *c = &an->clips[clip];
    int slot = fr % c->frame_count;
    int fi = c->first_frame + slot;
    int fend = c->first_frame + c->frame_count - 1;
    while ((an->frames[fi] & 0x8000u) && fi < fend) fi++;
    return (int)(an->frames[fi] & 0xFFFu);
}

typedef struct { int32_t lo[3], hi[3]; int n; } aabb_t;
static void ab_init(aabb_t *a) { int i; for (i=0;i<3;i++){a->lo[i]=0x7fffffff;a->hi[i]=-0x7fffffff;} a->n=0; }
static void ab_add(aabb_t *a, const int32_t p[3])
{ int i; for (i=0;i<3;i++){ if(p[i]<a->lo[i])a->lo[i]=p[i]; if(p[i]>a->hi[i])a->hi[i]=p[i]; } a->n++; }
static int ab_in(const aabb_t *a, const int32_t p[3])
{ return p[0]>=a->lo[0]&&p[0]<=a->hi[0]&&p[1]>=a->lo[1]&&p[1]<=a->hi[1]&&p[2]>=a->lo[2]&&p[2]<=a->hi[2]; }
static void ab_print(const char *tag, const aabb_t *a)
{
    printf("  %-26s x[%6d..%6d] (%5d)  y[%6d..%6d] (%5d)  z[%6d..%6d] (%5d)  n=%d\n",
           tag, a->lo[0],a->hi[0], a->hi[0]-a->lo[0],
                a->lo[1],a->hi[1], a->hi[1]-a->lo[1],
                a->lo[2],a->hi[2], a->hi[2]-a->lo[2], a->n);
}

/* Lokale AABB eines GATOR-MD1-Meshes in der Pose kf (unskaliert, ungedreht) */
static int gator_mesh_aabb(int kf, int mesh, aabb_t *out)
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    const re15_md1_mesh_t *m;
    int pass, i, r, k;
    void *sv = g_anim_pose_actor; g_anim_pose_actor = NULL;
    r = re15_skel_compute_pose(&B.skel, kf, poses);
    g_anim_pose_actor = sv;
    if (r != 0) return -1;
    if (mesh < 0 || mesh >= B.md1.mesh_count) return -2;
    m = &B.md1.meshes[mesh];
    ab_init(out);
    for (pass = 0; pass < 2; pass++) {
        const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
        int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
        for (i = 0; i < NV; i++) {
            int32_t v[3] = { V[i].x, V[i].y, V[i].z }, w[3];
            for (r = 0; r < 3; r++) {
                int64_t s = 0;
                for (k = 0; k < 3; k++) s += (int64_t)poses[mesh].rot[r*3+k] * v[k];
                w[r] = (int32_t)(s >> 12) + poses[mesh].trans[r];
            }
            ab_add(out, w);
        }
    }
    return out->n ? 0 : -3;
}

/* Leons gerenderte Vertices (Renderpfad main.c:7338-7375: yaw_rot skaliert die 3x3,
 * yawed_rot = yaw_rot x pose.rot, world = yawed_rot*v + yaw_rot*pose.trans + model_pos)
 * -> gator-lokal. s_pl = Spieler-Render-Scale (4096 = heute), s_g = Gator-Scale. */
static int leon_local_aabb(const re15_actor_t *e, const re15_actor_t *pl,
                           int vclip, int vframe, int s_pl, aabb_t *out,
                           const aabb_t *m6, const aabb_t *m7, int *in6, int *in7,
                           int *ntot)
{
    re15_emd_skeleton_t vs = PL_SKEL;               /* Leons Knochen + Bind-Pose */
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int32_t yaw[9], cs, sn, s_g;
    int bi, nb, r, c, k, pass, i;
    int kf;
    vs.keyframe_data       = B.skel_victim.keyframe_data;
    vs.keyframe_data_size  = B.skel_victim.keyframe_data_size;
    vs.keyframe_count      = B.skel_victim.keyframe_count;
    vs.keyframe_size_bytes = B.skel_victim.keyframe_size_bytes;
    kf = kf_of(&B.anim_victim, vclip, vframe);
    { void *sv = g_anim_pose_actor; g_anim_pose_actor = NULL;
      r = re15_skel_compute_pose(&vs, kf, poses);
      g_anim_pose_actor = sv; }
    if (r != 0) return -1;

    cs = re15_cos_q12((int)pl->rot_y); sn = re15_sin_q12((int)pl->rot_y);
    /* main.c: yaw_rot_q12 = { c,0,s, 0,0x1000,0, -s,0,c } */
    yaw[0]=cs; yaw[1]=0;      yaw[2]=sn;
    yaw[3]=0;  yaw[4]=0x1000; yaw[5]=0;
    yaw[6]=-sn;yaw[7]=0;      yaw[8]=cs;
    if (s_pl != 4096) for (k = 0; k < 9; k++) yaw[k] = (yaw[k] * s_pl) >> 12;

    s_g = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    ab_init(out);
    if (in6) *in6 = 0; if (in7) *in7 = 0; if (ntot) *ntot = 0;
    nb = PL_SKEL.bone_count;
    if (nb > PL_MD1.mesh_count) nb = PL_MD1.mesh_count;
    if (nb > 15) nb = 15;                        /* 15/16 = Waffen-Slots, nicht Leon */
    for (bi = 0; bi < nb; bi++) {
        const re15_md1_mesh_t *m = &PL_MD1.meshes[bi];
        int32_t yrot[9], ytr[3], bw[3];
        for (r = 0; r < 3; r++) for (c = 0; c < 3; c++) {
            int64_t s = 0;
            for (k = 0; k < 3; k++) s += (int64_t)yaw[r*3+k] * (int64_t)poses[bi].rot[k*3+c];
            yrot[r*3+c] = (int32_t)(s >> 12);
        }
        for (r = 0; r < 3; r++) {
            int64_t s = 0;
            for (k = 0; k < 3; k++) s += (int64_t)yaw[r*3+k] * (int64_t)poses[bi].trans[k];
            ytr[r] = (int32_t)(s >> 12);
        }
        bw[0] = ytr[0] + pl->x; bw[1] = ytr[1] + pl->y; bw[2] = ytr[2] + pl->z;
        for (pass = 0; pass < 2; pass++) {
            const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
            int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
            for (i = 0; i < NV; i++) {
                int32_t v[3] = { V[i].x, V[i].y, V[i].z }, w[3], d[3], L[3];
                for (r = 0; r < 3; r++) {
                    int64_t s = 0;
                    for (k = 0; k < 3; k++) s += (int64_t)yrot[r*3+k] * v[k];
                    w[r] = (int32_t)(s >> 12) + bw[r];
                }
                d[0] = w[0]-e->x; d[1] = w[1]-e->y; d[2] = w[2]-e->z;
                { int32_t gc = re15_cos_q12((int)e->rot_y), gs = re15_sin_q12((int)e->rot_y);
                  L[0] = (int32_t)(((int64_t)gc*d[0] - (int64_t)gs*d[2]) >> 12);
                  L[2] = (int32_t)(((int64_t)gs*d[0] + (int64_t)gc*d[2]) >> 12);
                  L[1] = d[1]; }
                L[0] = (int32_t)((int64_t)L[0]*4096/s_g);
                L[1] = (int32_t)((int64_t)L[1]*4096/s_g);
                L[2] = (int32_t)((int64_t)L[2]*4096/s_g);
                ab_add(out, L);
                if (ntot) (*ntot)++;
                if (m6 && in6 && ab_in(m6, L)) (*in6)++;
                if (m7 && in7 && ab_in(m7, L)) (*in7)++;
            }
        }
    }
    return 0;
}

/* Zustand P3 nachbauen wie der Port ihn setzt (Anker Clip4 f13 + Skalierung).
 * variante: 'O' = Original (kein Scale nirgends), 'T' = heute (Gator 2731, Leon 1x,
 * gb_opfer_skalieren MIT POSy-Ausgleich), 'N' = neu (Gator 2731, Leon 2731, OHNE
 * POSy-Ausgleich — der Renderer skaliert den POSE-Kanal dann selbst mit). */
static void place_p3(re15_actor_t *e, re15_actor_t *pl, int gclip, int gfr,
                     int vclip, int vfr, char variante)
{
    int16_t px=0, py=0, pz=0;
    int32_t s = (variante == 'O') ? 4096 : 2731;
    memset(e, 0, sizeof *e); memset(pl, 0, sizeof *pl);
    e->type = 0x23; e->render_scale_q12 = (int16_t)(variante == 'O' ? 0 : s);
    e->x = -5608; e->y = -1200; e->z = -22446; e->rot_y = 113;   /* Spielwerte gator_boss.log */
    e->motion = 4; e->anim_frame = 13;
    re15_clip_anchor_set_y3_pub(e, &B.skel, &B.anim, 4, 13);
    pl->anchor_x = e->anchor_x; pl->anchor_y = e->anchor_y; pl->anchor_z = e->anchor_z;
    pl->rot_y = e->rot_y;
    e->motion = (uint8_t)gclip; e->anim_frame = (uint32_t)gfr;
    re15_clip_root_motion_abs_y3_pub(e, &B.skel, &B.anim, gclip, gfr);
    pl->motion = (uint8_t)vclip; pl->anim_frame = (uint32_t)vfr;
    re15_clip_root_motion_abs_y3_pub(pl, &B.skel_victim, &B.anim_victim, vclip, vfr);
    /* gb_opfer_skalieren (enemy_ai_boss_gator.c:437-450) */
    re15_emd_get_keyframe_position(&B.skel_victim, kf_of(&B.anim_victim, vclip, vfr), &px,&py,&pz);
    pl->x = e->x + (int32_t)(((int64_t)(pl->x - e->x) * s) >> 12);
    pl->z = e->z + (int32_t)(((int64_t)(pl->z - e->z) * s) >> 12);
    pl->y = e->y + (int32_t)(((int64_t)(pl->y - e->y) * s) >> 12);
    if (variante == 'T')
        pl->y -= (int32_t)(((int64_t)py * (4096 - s)) >> 12);
    pl->render_scale_q12 = (int16_t)((variante == 'N') ? s : 0);
}

static void messe_var(const char *tag, char variante,
                      int gclip, int gfr, int vclip, int vfr, int kopf)
{
    re15_actor_t e, pl;
    aabb_t m6, m7, mu, LA;
    int gkf = kf_of(&B.anim, gclip, gfr);
    int in6, in7, ntot, inu = 0, i, s_pl;
    char nm[64];
    if (gator_mesh_aabb(gkf, 6, &m6) != 0 || gator_mesh_aabb(gkf, 7, &m7) != 0) {
        printf("  %s: Kiefer-AABB FEHLT\n", tag); return;
    }
    mu = m6;
    for (i = 0; i < 3; i++) { if (m7.lo[i] < mu.lo[i]) mu.lo[i] = m7.lo[i];
                              if (m7.hi[i] > mu.hi[i]) mu.hi[i] = m7.hi[i]; }
    if (kopf) {
        printf("-- %s (Gator Clip %d f%d / Opfer Clip %d f%d)\n", tag, gclip, gfr, vclip, vfr);
        ab_print("Mesh6 Oberkiefer", &m6);
        ab_print("Mesh7 Unterkiefer", &m7);
        ab_print("MAULRAUM (6 u 7)", &mu);
    }
    place_p3(&e, &pl, gclip, gfr, vclip, vfr, variante);
    s_pl = pl.render_scale_q12 ? pl.render_scale_q12 : 4096;
    leon_local_aabb(&e, &pl, vclip, vfr, s_pl, &LA, &m6, &m7, &in6, &in7, &ntot);
    {   aabb_t tmp; int a, b, n2;
        leon_local_aabb(&e, &pl, vclip, vfr, s_pl, &tmp, &mu, &mu, &a, &b, &n2);
        inu = a; (void)b; (void)n2; }
    snprintf(nm, sizeof nm, "LEON [%c] s_pl=%d", variante, s_pl);
    ab_print(nm, &LA);
    printf("     -> Laenge %d / Hoehe %d / Breite %d ; Vertices im Maulraum %d/%d = %.1f%% "
           "(Mesh6 %d, Mesh7 %d)\n",
           LA.hi[0]-LA.lo[0], LA.hi[1]-LA.lo[1], LA.hi[2]-LA.lo[2],
           inu, ntot, ntot ? 100.0*inu/ntot : 0.0, in6, in7);
}

static void messe(const char *tag, int gclip, int gfr, int vclip, int vfr)
{
    messe_var(tag, 'O', gclip, gfr, vclip, vfr, 1);   /* Original, kein Scale */
    messe_var(tag, 'T', gclip, gfr, vclip, vfr, 0);   /* heute */
    messe_var(tag, 'N', gclip, gfr, vclip, vfr, 0);   /* neu */
}

int main(void)
{
    size_t sz = 0, esz = 0, rsz = 0, msz = 0;
    uint8_t *emd = slurp(RE15_ASSET_RE2_DIR "/EM23.EMD", &sz);
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    uint8_t *md1 = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.MD1", &msz);
    if (!emd) { printf("FAIL EM23.EMD\n"); return 1; }
    memset(&B, 0, sizeof B);
    if (re2_emd_parse_bank(emd, sz, &B) != 0) { printf("FAIL parse EM23\n"); return 1; }
    B.ok = 1;
    if (!edd || !emr || !md1
        || re15_emd_parse_animation(edd, esz, &PL_ANIM) != 0
        || re15_emd_parse_skeleton(emr, rsz, &PL_SKEL) != 0
        || re15_md1_parse(md1, (int)msz, &PL_MD1) != 0) {
        printf("FAIL PL00 (EDD/EMR/MD1)\n"); return 1;
    }
    printf("EM23: %d Clips, victim %d Clips, %d Meshes, %d Bones | PL00: %d Bones, %d Meshes\n",
           B.anim.clip_count, B.anim_victim.clip_count, B.md1.mesh_count, B.skel.bone_count,
           PL_SKEL.bone_count, PL_MD1.mesh_count);

    printf("\n=== TEIL 1+2: nachgebauter P3/P2-Zustand ===\n");
    printf("  [O] Original (Gator 1x, Leon 1x) = die authored Relation; [T] heute\n"
           "  (Gator 2731, Leon 1x); [N] neu (Gator 2731, Leon 2731 ohne POSy-Ausgleich).\n"
           "  Alle drei im GATOR-LOKALEN, massstabsfreien Rahmen — [N] muss [O] treffen.\n");
    messe("P3 Kau-Loop", 11, 0, 1, 119);
    messe("P2 sf=60 (Schleuderspitze)", 5, 60, 1, 60);
    messe("P2 sf=0", 5, 0, 1, 0);

    printf("\n=== TEIL 3: echter Boss-Tick ROOM2090 ===\n");
#ifdef _WIN32
    _putenv("RE15_GB_TEST=1"); _putenv("RE15_GB_STUMM=1");
#else
    setenv("RE15_GB_TEST", "1", 1); setenv("RE15_GB_STUMM", "1", 1);
#endif
    {
        size_t n = 0;
        uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
        uint8_t *ems = NULL; size_t es = 0;
        re15_enemy_bank_t *eb;
        re15_actor_t *pl, *e; int slot, f, p3 = 0;
        if (!buf || re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL RDT\n"); return 1; }
        g_room_rdt_ok = 1;
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_actor_init(); re15_aot_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_cmd_reset();
        re15_damage_seed_rng(0x2545f491u);
        g_current_room_id = 0x2090;
        ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &es);
        if (!ems) { printf("FAIL CDEMD0.EMS\n"); return 1; }
        eb = re15_enemy_alloc(0x23u);
        if (!eb || re2_ems_load_bank(ems, es, 0x23, eb, NULL) != 0) { printf("FAIL Bank 0x23\n"); return 1; }
        eb->buf = NULL; eb->ok = 1;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        slot = RE15_ACTOR_MAX - 1; e = &g_actors[slot];
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x23u; e->x = -6000; e->y = 0; e->z = -22000;
        e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
        re15_enemy_apply_hitbox(e, 0x23u);
        pl->active = 1; pl->type = 0; pl->hp = 30; pl->x = -4000; pl->z = -22000; pl->y = 0; pl->floor = 0;
        re15_gator_boss_tick(slot);
        e->x = -6000; e->z = -22000; e->y = -1200;
        for (f = 0; f < 3000; f++) {
            int32_t ox = e->x, oz = e->z;
            if (pl->hp >= 0) { pl->x = -4000; pl->z = -22000; pl->y = 0; pl->state = 0; pl->motion = 0; pl->hit_react = 0; }
            re15_gator_boss_tick(slot);
            if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz, e->hit_radius_min, e->y, 4u);
                e->x = nx; e->z = nz;
            }
            if (e->motion == 11 && ++p3 == 1) {
                aabb_t m6, m7, mu, LA; int gkf = kf_of(&B.anim, 11, (int)e->anim_frame);
                int in6, in7, ntot, inu = 0, i, s_pl;
                printf("   LIVE P3 F%d: G=(%d,%d,%d) yaw=%d s_g=%d pl=(%d,%d,%d) s_pl=%d gaf=%u laf=%u\n",
                       f, e->x,e->y,e->z, (int)e->rot_y, (int)e->render_scale_q12,
                       pl->x,pl->y,pl->z, (int)pl->render_scale_q12,
                       (unsigned)e->anim_frame, (unsigned)pl->anim_frame);
                gator_mesh_aabb(gkf, 6, &m6); gator_mesh_aabb(gkf, 7, &m7);
                mu = m6;
                for (i = 0; i < 3; i++) { if (m7.lo[i] < mu.lo[i]) mu.lo[i] = m7.lo[i];
                                          if (m7.hi[i] > mu.hi[i]) mu.hi[i] = m7.hi[i]; }
                ab_print("MAULRAUM (6 u 7)", &mu);
                s_pl = pl->render_scale_q12 ? pl->render_scale_q12 : 4096;
                leon_local_aabb(e, pl, 1, 119, s_pl, &LA, &m6, &m7, &in6, &in7, &ntot);
                {   aabb_t tmp; int a, b, n2;
                    leon_local_aabb(e, pl, 1, 119, s_pl, &tmp, &mu, &mu, &a, &b, &n2);
                    inu = a; (void)b; (void)n2; }
                ab_print("LEON LIVE", &LA);
                printf("     -> Laenge %d / Hoehe %d / Breite %d ; Vertices im Maulraum %d/%d = %.1f%%\n",
                       LA.hi[0]-LA.lo[0], LA.hi[1]-LA.lo[1], LA.hi[2]-LA.lo[2],
                       inu, ntot, ntot ? 100.0*inu/ntot : 0.0);
                break;
            }
        }
        if (!p3) printf("   P3 in 3000 Bildern NICHT erreicht\n");
    }
    free(emd);
    return 0;
}
