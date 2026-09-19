/* probe_gator_fress.c — MESS-SCHIENE des ROOM2090-Fress-Finishers (Befund B,
 * analysis/befunde_runde4_2026-09-12/fress-sync.md).
 *
 * ANLASS (Nutzer 2026-09-12): "leon steckt bei der Todesanimation immer noch
 * nicht dann im Aligator Maul und wird rumgeschleudert, das ist noch sehr
 * asynchron."
 *
 * Der Probe-Lauf treibt den ECHTEN Boss-Tick (re15_gator_boss_tick + die
 * Dispatcher-SCA-Klemme wie probe_gator_sweep.c, KEIN Nachbau) mit GELADENER
 * RE2-EM23-Bank (22 Bones — ohne Bank faellt re15_enemy_bone_world_pos auf
 * die Wurzel zurueck und taeuscht, Lehre reai-v2-re2-trace-datei) bis in die
 * FRESS-Sequenz: Leon steht mit hp=30 im Wasser, der erste Biss (50 Schaden,
 * dmg_table[5] @DAT_8006f418) macht hp<0 -> gb_biss_abschluss -> GBP_FRESSEN.
 * Die Messreihe selbst schreibt der Engine-Tick als FSYNC-Zeilen nach
 * gator_boss.log (pro Frame: analytischer Bahn-Anker kx/kz, Leon-Wurzel,
 * gerenderte Kiefer-Bones 6/7 inkl. pitch/jaw-vz-Hook + Render-Scale,
 * 3D-Abstand Leon<->Maulmitte).
 *
 * Bestehen = die Sequenz laeuft bis zum Verschlingen (pl->no_draw == 1).
 * Die eigentliche Auswertung (Asynchronitaet beziffern) liest gator_boss.log.
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

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz;
    return b;
}

/* RE2-Bank aus CDEMD0.EMS laden — Muster test_re2_zombie_teardeath.c:139-152. */
static int load_re2_bank(uint8_t type)
{
    static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
    if (!s_ems) { size_t n = 0;
        s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
        s_ems_sz = (long)n; }
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

/* ==== PIN Phase 2 (gator-und-audio, 2026-09-19): LEONS WURZEL IM MAUL ==================
 * Metrik wie probe_r16_gator_finisher_anker: Leons GERENDERTE Wurzel (pl->xyz + RotY(yaw)*
 * POSE_v, exakt re15_skel_bone_to_world) zurueck in den GATOR-LOKALEN Rahmen,
 * local = RotY(-yaw)*(root - G) / s. Im Original (kein Scale) ist das exakt
 * (off_v - off_g) + POS_v (RE2 Anker-Paar FUN_80015B94/CB8). Bestehen: bei P3 (Clip 11 f0
 * / Opfer f119) |local - Original| <= 5 je Achse UND local liegt in der lokalen AABB von
 * MD1-Mesh 7 (Unterkiefer) der geposten Kette (heute vor dem Fix: (7892,-1804,295), dRef
 * (2628,599,97), ausserhalb beider Kiefer - "er liegt darunter"). */
static int kf_of(const re15_emd_animation_t *an, int clip, int fr)
{
    const re15_emd_clip_t *c = &an->clips[clip];
    int fi = c->first_frame + (fr % c->frame_count);
    int fend = c->first_frame + c->frame_count - 1;
    while ((an->frames[fi] & 0x8000u) && fi < fend) fi++;
    return (int)(an->frames[fi] & 0xFFFu);
}
static void spd(const re15_emd_skeleton_t *sk, int kf, int32_t o[3])
{ int16_t x=0,y=0,z=0; re15_emd_get_keyframe_speed(sk, kf, &x,&y,&z); o[0]=x;o[1]=y;o[2]=z; }
static void pos(const re15_emd_skeleton_t *sk, int kf, int32_t o[3])
{ int16_t x=0,y=0,z=0; re15_emd_get_keyframe_position(sk, kf, &x,&y,&z); o[0]=x;o[1]=y;o[2]=z; }
static int mesh_local_aabb(re15_enemy_bank_t *B, int kf, int mesh, int32_t lo[3], int32_t hi[3])
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    const re15_md1_mesh_t *m; int pass, i, r, k, n = 0;
    void *sv = g_anim_pose_actor; g_anim_pose_actor = NULL;
    r = re15_skel_compute_pose(&B->skel, kf, poses);
    g_anim_pose_actor = sv;
    if (r != 0 || mesh < 0 || mesh >= B->md1.mesh_count) return -1;
    m = &B->md1.meshes[mesh];
    lo[0]=lo[1]=lo[2]=0x7fffffff; hi[0]=hi[1]=hi[2]=-0x7fffffff;
    for (pass = 0; pass < 2; pass++) {
        const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
        int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
        for (i = 0; i < NV; i++) {
            int32_t v[3] = { V[i].x, V[i].y, V[i].z }, w[3];
            for (r = 0; r < 3; r++) { int64_t s = 0;
                for (k = 0; k < 3; k++) s += (int64_t)poses[mesh].rot[r*3+k] * v[k];
                w[r] = (int32_t)(s >> 12) + poses[mesh].trans[r]; }
            for (r = 0; r < 3; r++) { if (w[r] < lo[r]) lo[r] = w[r]; if (w[r] > hi[r]) hi[r] = w[r]; }
            n++;
        }
    }
    return n ? 0 : -1;
}
static void leon_local(re15_enemy_bank_t *B, const re15_actor_t *e, const re15_actor_t *pl,
                       int vkf, int32_t out[3])
{
    int32_t P[3], w[3], d[3], cs, sn;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    pos(&B->skel_victim, vkf, P);
    /* Phase 3: traegt der Spieler selbst den Render-Scale (+0x166, Fress-Finisher),
     * dann legt der Zeichner den Wurzel-POSE-Kanal MIT diesem Faktor ab
     * (platform/pc/main.c, Spieler-Zweig: yaw_rot_q12 *= render_scale_q12 — Original-
     * Pfad ScaleMatrix VOR der Bone-Schleife @0x8001e904-40). Die Metrik muss dem
     * Zeichner folgen, sonst misst sie ein Modell, das niemand zeichnet. */
    if (pl->render_scale_q12) {
        int _i;
        for (_i = 0; _i < 3; _i++)
            P[_i] = (int32_t)(((int64_t)P[_i] * pl->render_scale_q12) >> 12);
    }
    re15_skel_bone_to_world(P, (int16_t)pl->rot_y, pl->x, pl->y, pl->z, w);
    d[0] = w[0]-e->x; d[1] = w[1]-e->y; d[2] = w[2]-e->z;
    cs = re15_cos_q12((int)e->rot_y); sn = re15_sin_q12((int)e->rot_y);
    out[0] = (int32_t)((int64_t)(((int64_t)cs*d[0] - (int64_t)sn*d[2]) >> 12) * 4096 / s);
    out[2] = (int32_t)((int64_t)(((int64_t)sn*d[0] + (int64_t)cs*d[2]) >> 12) * 4096 / s);
    out[1] = (int32_t)((int64_t)d[1] * 4096 / s);
}
static int pin_leon_im_maul(re15_enemy_bank_t *B, const re15_actor_t *e, const re15_actor_t *pl)
{
    int gkf = kf_of(&B->anim, 11, (int)e->anim_frame), vkf = kf_of(&B->anim_victim, 1, 119);
    int32_t L[3], ref[3], og[3], ov[3], P[3], lo7[3], hi7[3], lo6[3], hi6[3];
    int in7, in6, ok = 1, i;
    spd(&B->skel, gkf, og); spd(&B->skel_victim, vkf, ov); pos(&B->skel_victim, vkf, P);
    for (i = 0; i < 3; i++) ref[i] = ov[i] - og[i] + P[i];
    leon_local(B, e, pl, vkf, L);
    if (mesh_local_aabb(B, gkf, 7, lo7, hi7) != 0 || mesh_local_aabb(B, gkf, 6, lo6, hi6) != 0) {
        printf("FAIL: Kiefer-AABB nicht berechenbar\n"); return 0; }
    in7 = L[0]>=lo7[0]&&L[0]<=hi7[0]&&L[1]>=lo7[1]&&L[1]<=hi7[1]&&L[2]>=lo7[2]&&L[2]<=hi7[2];
    in6 = L[0]>=lo6[0]&&L[0]<=hi6[0]&&L[1]>=lo6[1]&&L[1]<=hi6[1]&&L[2]>=lo6[2]&&L[2]<=hi6[2];
    printf("PIN P3: G=(%d,%d,%d) yaw=%d s=%d pl=(%d,%d,%d) | LOKAL=(%d,%d,%d) Original=(%d,%d,%d) "
           "dRef=(%d,%d,%d) | Mesh7 x[%d..%d] y[%d..%d] z[%d..%d] -> 6=%d/7=%d\n",
           e->x, e->y, e->z, (int)e->rot_y, e->render_scale_q12 ? e->render_scale_q12 : 4096,
           pl->x, pl->y, pl->z, L[0], L[1], L[2], ref[0], ref[1], ref[2],
           L[0]-ref[0], L[1]-ref[1], L[2]-ref[2],
           lo7[0], hi7[0], lo7[1], hi7[1], lo7[2], hi7[2], in6, in7);
    for (i = 0; i < 3; i++) if (L[i]-ref[i] > 5 || L[i]-ref[i] < -5) ok = 0;
    if (!ok) printf("FAIL: Leons lokale Wurzel weicht > 5 vom Original-Sitz ab (Scale-Nachzug fehlt?)\n");
    if (!in7) { printf("FAIL: Leons Wurzel liegt nicht im Unterkiefer (Mesh 7) - 'er liegt darunter'\n"); ok = 0; }
    return ok;
}

/* ==== PIN Phase 3 (gator-maul): LEONS KOERPER im Maul, nicht nur seine Wurzel =========
 * Die Wurzel sass seit Phase 2 richtig, Rumpf und Beine ragten trotzdem heraus, weil der
 * Gator mit GB_SCALE_Q12 = 2731 (2/3) und Leon 1x gezeichnet wurde — im gator-lokalen
 * Rahmen also 1,5x zu gross (GEMESSEN probe_p3_gator_maul: Laenge 3915 gegen Maulraum
 * 3655, 620/776 Vertices drin). Seit Phase 3 traegt Leon waehrend der Fress-Phase
 * denselben Render-Scale (+0x166-Pfad, ScaleMatrix @0x8001e904-40) und trifft damit die
 * authored 1x-Relation auf +-2 Einheiten: Laenge 2609 (Original 2609), 776/776 Vertices
 * im Maulraum. Der Pin verlangt >= 95 % — heute 100,0 %, vor dem Fix 79,9 %. */
static re15_emd_skeleton_t s_pl00_skel;
static re15_md1_t          s_pl00_md1;
static int pl00_geladen(void)
{
    static int s_ok = -1;
    size_t rsz = 0, msz = 0;
    uint8_t *emr, *md1;
    if (s_ok >= 0) return s_ok;
    emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    md1 = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.MD1", &msz);
    s_ok = (emr && md1
            && re15_emd_parse_skeleton(emr, rsz, &s_pl00_skel) == 0
            && re15_md1_parse(md1, (int)msz, &s_pl00_md1) == 0);
    return s_ok;
}
static int pin_leon_koerper_im_maul(re15_enemy_bank_t *B, const re15_actor_t *e,
                                    const re15_actor_t *pl)
{
    int gkf = kf_of(&B->anim, 11, (int)e->anim_frame);
    int vkf = kf_of(&B->anim_victim, 1, 119);
    int32_t lo6[3], hi6[3], lo7[3], hi7[3], mlo[3], mhi[3];
    int32_t yaw[9], cs, sn, s_g, s_pl;
    int32_t Llo[3], Lhi[3];
    re15_emd_skeleton_t vs;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int bi, nb, r, c, k, pass, i, ntot = 0, nin = 0;
    if (!pl00_geladen()) { printf("FAIL: PL00.EMR/MD1 nicht ladbar\n"); return 0; }
    if (mesh_local_aabb(B, gkf, 6, lo6, hi6) != 0 || mesh_local_aabb(B, gkf, 7, lo7, hi7) != 0) {
        printf("FAIL: Kiefer-AABB nicht berechenbar\n"); return 0; }
    for (i = 0; i < 3; i++) { mlo[i] = lo6[i] < lo7[i] ? lo6[i] : lo7[i];
                              mhi[i] = hi6[i] > hi7[i] ? hi6[i] : hi7[i]; }
    vs = s_pl00_skel;                                  /* Leons Knochen + Bind-Pose */
    vs.keyframe_data       = B->skel_victim.keyframe_data;
    vs.keyframe_data_size  = B->skel_victim.keyframe_data_size;
    vs.keyframe_count      = B->skel_victim.keyframe_count;
    vs.keyframe_size_bytes = B->skel_victim.keyframe_size_bytes;
    {   void *sv = g_anim_pose_actor; g_anim_pose_actor = NULL;
        r = re15_skel_compute_pose(&vs, vkf, poses);
        g_anim_pose_actor = sv; }
    if (r != 0) { printf("FAIL: Opfer-Pose nicht berechenbar\n"); return 0; }
    cs = re15_cos_q12((int)pl->rot_y); sn = re15_sin_q12((int)pl->rot_y);
    yaw[0]=cs; yaw[1]=0; yaw[2]=sn; yaw[3]=0; yaw[4]=0x1000; yaw[5]=0;
    yaw[6]=-sn; yaw[7]=0; yaw[8]=cs;
    s_pl = pl->render_scale_q12 ? pl->render_scale_q12 : 4096;
    if (s_pl != 4096) for (k = 0; k < 9; k++) yaw[k] = (yaw[k] * s_pl) >> 12;
    s_g = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    for (i = 0; i < 3; i++) { Llo[i] = 0x7fffffff; Lhi[i] = -0x7fffffff; }
    nb = s_pl00_skel.bone_count;
    if (nb > s_pl00_md1.mesh_count) nb = s_pl00_md1.mesh_count;
    if (nb > 15) nb = 15;                              /* 15/16 = Waffen-Slots */
    for (bi = 0; bi < nb; bi++) {
        const re15_md1_mesh_t *m = &s_pl00_md1.meshes[bi];
        int32_t yrot[9], ytr[3], bw[3];
        for (r = 0; r < 3; r++) for (c = 0; c < 3; c++) {
            int64_t s2 = 0;
            for (k = 0; k < 3; k++) s2 += (int64_t)yaw[r*3+k] * (int64_t)poses[bi].rot[k*3+c];
            yrot[r*3+c] = (int32_t)(s2 >> 12);
        }
        for (r = 0; r < 3; r++) {
            int64_t s2 = 0;
            for (k = 0; k < 3; k++) s2 += (int64_t)yaw[r*3+k] * (int64_t)poses[bi].trans[k];
            ytr[r] = (int32_t)(s2 >> 12);
        }
        bw[0] = ytr[0] + pl->x; bw[1] = ytr[1] + pl->y; bw[2] = ytr[2] + pl->z;
        for (pass = 0; pass < 2; pass++) {
            const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
            int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
            for (i = 0; i < NV; i++) {
                int32_t v[3] = { V[i].x, V[i].y, V[i].z }, w[3], d[3], L[3];
                int gc, gs, a;
                for (r = 0; r < 3; r++) {
                    int64_t s2 = 0;
                    for (k = 0; k < 3; k++) s2 += (int64_t)yrot[r*3+k] * v[k];
                    w[r] = (int32_t)(s2 >> 12) + bw[r];
                }
                d[0] = w[0]-e->x; d[1] = w[1]-e->y; d[2] = w[2]-e->z;
                gc = re15_cos_q12((int)e->rot_y); gs = re15_sin_q12((int)e->rot_y);
                L[0] = (int32_t)((int64_t)(((int64_t)gc*d[0] - (int64_t)gs*d[2]) >> 12) * 4096 / s_g);
                L[2] = (int32_t)((int64_t)(((int64_t)gs*d[0] + (int64_t)gc*d[2]) >> 12) * 4096 / s_g);
                L[1] = (int32_t)((int64_t)d[1] * 4096 / s_g);
                for (a = 0; a < 3; a++) { if (L[a] < Llo[a]) Llo[a] = L[a];
                                          if (L[a] > Lhi[a]) Lhi[a] = L[a]; }
                ntot++;
                if (L[0]>=mlo[0]&&L[0]<=mhi[0]&&L[1]>=mlo[1]&&L[1]<=mhi[1]
                    && L[2]>=mlo[2]&&L[2]<=mhi[2]) nin++;
            }
        }
    }
    printf("PIN P3-KOERPER: s_pl=%d Leon-LOKAL x[%d..%d] y[%d..%d] z[%d..%d] "
           "(Laenge %d Hoehe %d) | Maulraum x[%d..%d] y[%d..%d] z[%d..%d] -> %d/%d = %.1f%%\n",
           s_pl, Llo[0],Lhi[0], Llo[1],Lhi[1], Llo[2],Lhi[2],
           Lhi[0]-Llo[0], Lhi[1]-Llo[1],
           mlo[0],mhi[0], mlo[1],mhi[1], mlo[2],mhi[2],
           nin, ntot, ntot ? 100.0*nin/ntot : 0.0);
    if (!ntot || nin * 100 < ntot * 95) {
        printf("FAIL: Leons Koerper liegt nicht im Maul (< 95%% der Vertices) - "
               "ragt er wieder heraus?\n");
        return 0;
    }
    return 1;
}

int main(void)
{
#ifdef _WIN32
    _putenv("RE15_GB_TEST=1");     /* aggro sofort */
#else
    setenv("RE15_GB_TEST", "1", 1);
#endif

    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf) { printf("FAIL: ROOM2090.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) {
        printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;

    if (!load_re2_bank(0x23u)) {
        printf("FAIL: RE2-EM23-Bank nicht ladbar (CDEMD0.EMS)\n"); return 1; }
    {   re15_enemy_bank_t *eb = re15_enemy_find(0x23u);
        printf("EM23-Bank: bones=%d clips=%d\n",
               eb ? eb->skel.bone_count : -1, eb ? eb->anim.clip_count : -1);
        if (!eb || eb->skel.bone_count != 22) {
            printf("FAIL: erwartet 22 Bones (EM23-Rig)\n"); return 1; }
    }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* Gator-Neuspawn wie main.c / probe_gator_sweep.c. */
    int slot = RE15_ACTOR_MAX - 1;
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x23u;
    e->x = -6000; e->y = 0; e->z = -22000;
    e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x23u);

    /* Leon im Wasser, hp=30: der erste Biss (50) toetet -> FRESSEN. */
    pl->active = 1; pl->type = 0; pl->hp = 30;
    pl->x = -4000; pl->z = -22000; pl->y = 0; pl->floor = 0;

    re15_gator_boss_tick(slot);           /* Frame 0 = Boss-INIT (setzt Lauerplatz) */
    e->x = -6000; e->z = -22000; e->y = -1200;

    int fress_gesehen = 0;
    /* ⛔ WANN STARTET DIE TODES-PRAESENTATION? (Nutzer 2026-09-13, zweite Meldung:
     * "Die Fressanimation findet immer noch nicht im YOU ARE DEAD Screen statt, sondern
     * davor.") Im Original laeuft sie MITTEN im Fressen: RE1.5s FSM FUN_8001500c gated
     * auf Spieler-Kommando 6 = GEFRESSEN (@0x80015014-30), RE2 setzt den Todes-Latch bei
     * Gator-Clip-4-FRAME 13 (@0x80101104-34). Gemessen wird deshalb der ABSTAND zwischen
     * dem Beginn des Fressens und dem Moment, in dem re15_death_presentation_active()
     * anschlaegt - und wie viele Frames danach noch Fress-Animation laeuft.
     * Die FSM selbst braucht ab dort fest 0x32 + 0x1b = 77 Ticks bis zum schwarzen
     * Grund (game_step_common.c, sub0/sub1) - das ist die zweite Haelfte der Rechnung. */
    extern int re15_death_presentation_active(void);
    int f_fress_start = -1, f_praesentation = -1, f_ende = -1;
    for (int f = 0; f < 3000; f++) {
        if (pl->hp >= 0) {                /* Statue nur solange er lebt — danach
                                           * fuehrt die FRESS-Sequenz Leon selbst */
            pl->x = -4000; pl->z = -22000; pl->y = 0;
            pl->state = 0; pl->motion = 0; pl->hit_react = 0;
        }
        int32_t ox = e->x, oz = e->z;
        re15_gator_boss_tick(slot);
        if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
            int32_t nx = e->x, nz = e->z;   /* Dispatcher-Klemme 1:1 (sweep) */
            re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz,
                                           e->hit_radius_min, e->y, 4u);
            e->x = nx; e->z = nz;
        }
        if (re15_gator_fressen_hold()) {
            fress_gesehen = 1;
            if (f_fress_start < 0) f_fress_start = f;
        }
        if (f_praesentation < 0 && re15_death_presentation_active()) f_praesentation = f;
        /* UMVERANKERT (Runde 6, gator-vollausbau.md 7b): der authored Finisher
         * verschlingt Leon nicht mehr per no_draw - er haengt im letzten
         * Opfer-Frame sichtbar im Maul, waehrend der Gator Clip 11 kaut. */
        if (e->motion == 11) { f_ende = f; break; }  /* P3-Kau-Loop = Sequenz komplett */
    }

    {   /* Die Rechnung, die der Nutzer sieht: schwarzer Grund = Praesentationsstart + 77
         * (FSM sub0 0x32 + sub1 0x1b). Liegt er VOR dem Ende der Fress-Animation, sieht
         * man das Fressen unter dem Bildschirm - genau das ist gefordert. */
        int blackbg = (f_praesentation >= 0) ? f_praesentation + 77 : -1;
        printf("Praesentation: Fressen ab F%d, Gate ab F%d, Kau-Loop ab F%d, "
               "schwarzer Grund bei F%d\n",
               f_fress_start, f_praesentation, f_ende, blackbg);
        if (f_praesentation < 0) {
            printf("FAIL: re15_death_presentation_active() schlaegt NIE an - die "
                   "Todes-Praesentation startet gar nicht waehrend des Fressens\n");
            return 1;
        }
        /* ⛔ KEIN "Gate am Phasenbeginn"-Test (korrigiert 2026-09-13): der Latch sitzt
         * im Original NICHT am Anfang der Fress-Phase, sondern bei CLIP-4-FRAME 13
         * (@0x80101104-34) - demselben Block, der das Opfer-Paar koppelt. Gemessen sind
         * das 14 Frames nach dem Phasenbeginn; ein 2-Frame-Fenster haette den richtigen
         * Stand zurueckgewiesen. Geprueft wird deshalb die Eigenschaft, um die es dem
         * Nutzer geht: ein nennenswerter Teil der Fress-Animation muss UNTER dem
         * Bildschirm laufen. Gemessener Stand: schwarzer Grund F102, Kau-Loop ab F146 =
         * 44 Frames Ueberlappung. Vor dem Runde-8-Fix startete die FSM erst NACH dem
         * Finisher - der schwarze Grund waere dann erst bei F146+77 gefallen. */
        if (f_ende >= 0 && blackbg >= 0 && blackbg > f_ende - 30) {
            printf("FAIL: der schwarze Grund faellt auf F%d, das Fressen endet schon F%d "
                   "- unter dem Bildschirm laeuft fast nichts mehr (Soll: mindestens 30 "
                   "Frames)\n", blackbg, f_ende);
            return 1;
        }
    }

    if (!fress_gesehen) { printf("FAIL: FRESSEN nie erreicht\n"); return 1; }
    if (e->motion != 11) { printf("FAIL: P3-Kau-Loop (Clip 11) nie erreicht\n"); return 1; }
    if (pl->motion != 1 || pl->anim_frame != 119) {
        printf("FAIL: Leon nicht im letzten Opfer-Frame geparkt (clip=%d af=%u)\n",
               (int)pl->motion, (unsigned)pl->anim_frame); return 1; }
    /* PIN Phase 2: Leons Wurzel sitzt im Maul (Original-Relation +-5, in Mesh 7). */
    {   re15_enemy_bank_t *eb = re15_enemy_find(0x23u);
        if (!eb || !eb->victim_ok) { printf("FAIL: Opfer-Bank fehlt\n"); return 1; }
        if (!pin_leon_im_maul(eb, e, pl)) return 1;
        /* y-Spur: die Sonde hat KEIN game_step; pl->y muss der Fix gesetzt haben (!= 0). */
        if (pl->y == 0) { printf("FAIL: pl->y blieb 0 - der Scale-Nachzug setzt die Hoehe nicht\n"); return 1; }
        /* PIN Phase 3: auch der KOERPER liegt im Maul (Massstab), nicht nur die Wurzel. */
        if (!pin_leon_koerper_im_maul(eb, e, pl)) return 1;
        if (pl->render_scale_q12 != e->render_scale_q12) {
            printf("FAIL: Leon traegt nicht den Gator-Render-Scale (pl=%d, gator=%d)\n",
                   (int)pl->render_scale_q12, (int)e->render_scale_q12);
            return 1;
        }
    }
    printf("OK: FRESS-Sequenz komplett - FSYNC-Messreihe in gator_boss.log\n");
    return 0;
}
