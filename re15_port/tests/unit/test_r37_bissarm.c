/* test_r37_bissarm.c — Runde 37 Spur J "bissarm" (Dossier analysis/befunde_runde37/J_bissarm.md).
 *
 * Nutzer-Wortlaut (AUFTRAG.md Nachtrag 6): "Ja, wie ich gerade gesehen habe fehlt in unseren Port
 * teilweise noch der dranhängende Arm wenn man vom Zombie gebissen wird."
 *
 * Teile:
 *   griff <raum-hex> <px> <pz> <rot> <vorn|hinten> [budget] [abstand]
 *       Echter game_step-Weg (SCD-Spawns, RE2-Standardgehirn, RE2-Baenke aus CDEMD0.EMS): genau EIN
 *       Zombie (der naechste zu (px,pz)) wird vor/hinter Leon gestellt, greift, beisst. Je Griffbild:
 *       Leons Lage gegen die Original-Platzierung (Anker + RotY(Yaw + PL+0x158) * Wurzelversatz des
 *       Opfer-Clips, 0x80015CB8 @0x80015D3C-68), und die Teile-Maske des Greifers.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re2_ems.h"
#include "re15_skeleton.h"
#include "re15_ems.h"
#include "re15_bissarm_r37.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_inv_find_item(uint8_t id);
extern void re15_clip_root_motion_abs_pub(re15_actor_t *a, const re15_emd_skeleton_t *skel,
                                          const re15_emd_animation_t *anim, int clip, int frame);
extern int  re15_re2z_gore_resolve(const re15_actor_t *e, const int8_t *bone_parent, int n,
                                   uint8_t *out_draw, uint32_t *out_tint, uint8_t *out_mesh);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;
static int                s_fail = 0;

#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); s_fail++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}

/* Der Hybrid der exe (main.c pc_enemy_hybrid_re15_models): RE2-Gehirn/-Clips + RE1.5-Meshes/-Bindlaengen
 * (re2_hybrid_apply). Seit 2026-08-21 laeuft der RE2-Flavor IMMER auf diesem Rig (main.c ~Z.1175). */
static int s_hybrid = 1;
static uint8_t *s_blob15[64];
static void hybrid_anwenden(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb || !eb->ok || eb->remap_ok || !s_hybrid) return;
    const char *base = getenv("RE15_ASSET_DIR");
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) { free(ems); return; }
    uint8_t *blob = (uint8_t *)malloc(len); memcpy(blob, ems + off, len); free(ems);
    if (type < 64) { free(s_blob15[type]); s_blob15[type] = blob; }
    static re15_md1_t md15; static re15_emd_skeleton_t sk15; static re15_emd_animation_t an15;
    if (re15_emd_parse_container(blob, len, &md15, &sk15, &an15, NULL) != 0) return;
    int um = -1;
    int rc = re2_hybrid_apply(eb, (int)type, &md15, &sk15, &um);
    printf("  Hybrid Typ 0x%02X: rc=%d unmapped=%d\n", type, rc, um);
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int raum_laden(unsigned room)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR, (room >> 12) & 0xFu, room);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("SKIP: %s nicht lesbar\n", path); return 77; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse %s\n", path); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;     /* wie der Plattform-Lader: die Kollision des Raums ist scharf */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (int)room;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) {
            load_bank2(g_actors[s].type);
            hybrid_anwenden(g_actors[s].type);
        }
    return 0;
}

/* Die Original-Platzierung des Opfers (RE2 0x80015CB8: pos = Anker(+0x164) + RotY(Yaw) * Wurzel-
 * versatz, Yaw = rot_y + PL+0x158 @0x8010AB20-34) — OHNE die Port-Wandklemme. */
static void soll_lage(const re15_actor_t *pl, int32_t *sx, int32_t *sz)
{
    re15_enemy_bank_t *vb = re15_enemy_find(re15_player_victim_type());
    re15_actor_t k = *pl;
    *sx = pl->x; *sz = pl->z;
    if (!vb || !vb->victim_ok) return;
    k.rot_y = (int16_t)(((int)pl->rot_y + (int)pl->re2z_t158) & 0x0fff);
    re15_clip_root_motion_abs_pub(&k, &vb->skel_victim, &vb->anim_victim, (int)pl->motion,
                                  (int)pl->anim_frame);
    *sx = k.x; *sz = k.z;
}

/* ---- Geometrie: Hand des Greifers gegen Leons Koerper (Vergleich mit dem RE2-Original-RAM) ----------
 * Pose = re15_skel_compute_pose (der Zeichner-Weg), Welt = re15_skel_bone_to_world (main.c NPC-Zeichner).
 * Greifer: die Bank, die der Zeichner im Griff nimmt (Aktionsbank eb->skel/eb->anim — RE2-Rig mit den
 * RE1.5-Bindlaengen des Hybrids); Leon: PL00.EMR (Knochen) + Opfer-Keyframes der Bank (main.c s_victim_skel). */
extern void *g_anim_pose_actor;
static re15_emd_skeleton_t s_pl00; static int s_pl00_ok = 0;
static void pl00_laden(void)
{
    if (s_pl00_ok) return;
    const char *base = getenv("RE15_ASSET_DIR");
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/PLD/%s.EMR", (base && *base) ? base : RE15_ASSET_PSX_DIR,
             getenv("R37J_ELZA") ? "PL04" : "PL00");
    uint8_t *b = slurp(p, &n);
    if (b && re15_emd_parse_skeleton(b, n, &s_pl00) == 0) s_pl00_ok = 1;
}
static int kf_von(const re15_emd_animation_t *a, int clip, int frame)
{
    if (!a || clip < 0 || clip >= a->clip_count) return -1;
    const re15_emd_clip_t *c = &a->clips[clip];
    if (c->frame_count <= 0) return -1;
    int fi = c->first_frame + (frame % c->frame_count), fend = c->first_frame + c->frame_count - 1;
    while ((a->frames[fi] & 0x8000u) && fi < fend) fi++;
    return (int)(a->frames[fi] & 0xFFFu);
}
static int posen_welt(const re15_emd_skeleton_t *sk, int kf, const re15_actor_t *a, int32_t w[][3], int nmax)
{
    static re15_skel_pose_t P[RE15_EMD_MAX_BONES];
    if (!sk || kf < 0) return 0;
    void *sav = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rc = re15_skel_compute_pose(sk, kf, P);
    g_anim_pose_actor = sav;
    if (rc != 0) return 0;
    int n = sk->bone_count < nmax ? sk->bone_count : nmax;
    for (int b = 0; b < n; b++) re15_skel_bone_to_world(P[b].trans, a->rot_y, a->x, a->y, a->z, w[b]);
    return n;
}
static int abst3(const int32_t a[3], const int32_t b[3])
{
    double dx = a[0] - b[0], dy = a[1] - b[1], dz = a[2] - b[2];
    return (int)(sqrt(dx * dx + dy * dy + dz * dz) + 0.5);
}

/* ---- VOLUMENMASS "Arm in Leon" (Muster tests/unit/r35_raeume_volumen.h, Runde 35 Spur H) ---------------
 * Zaehlt die Vertices von Unterarm + Hand des Greifers (RE2-Parts 10/11 und 13/14), die in Leons Rumpf (Mesh 0),
 * Kopf (Mesh 8) oder Oberarmen (Mesh 9/12) liegen — Huelle je 32er-Hoehenband = Bounding-Box der PL00.MD1-
 * Vertices im Knochen-Rahmen. Liegt ein Arm-Vertex IN Leon, ist er im Bild verdeckt (der Zeichner sortiert je
 * Dreieck nach Tiefe, Leons Oberflaeche liegt davor). */
#include "re15_md1.h"
#define HB 32
typedef struct { int ymin, nb; int16_t x0[128], x1[128], z0[128], z1[128]; uint8_t ok[128]; } hul_t;
static const int s_hul_mesh[4] = { 0, 8, 9, 12 };
static hul_t s_hul[4]; static int s_hul_ok = 0;
static re15_md1_t s_plmd1; static uint8_t *s_plmd1_buf = NULL;
static void hul_add(hul_t *v, const re15_md1_vertex_t *p, int n, int pass)
{
    for (int i = 0; i < n; i++) {
        if (pass == 0) { if (p[i].y < v->ymin) v->ymin = p[i].y; continue; }
        int b = (p[i].y - v->ymin) / HB;
        if (b < 0 || b >= 128) continue;
        if (!v->ok[b]) { v->x0[b] = v->x1[b] = p[i].x; v->z0[b] = v->z1[b] = p[i].z; v->ok[b] = 1; }
        if (p[i].x < v->x0[b]) v->x0[b] = p[i].x;
        if (p[i].x > v->x1[b]) v->x1[b] = p[i].x;
        if (p[i].z < v->z0[b]) v->z0[b] = p[i].z;
        if (p[i].z > v->z1[b]) v->z1[b] = p[i].z;
        if (b + 1 > v->nb) v->nb = b + 1;
    }
}
static int huelle_laden(void)
{
    if (s_hul_ok) return 1;
    const char *base = getenv("RE15_ASSET_DIR");
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.MD1", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    s_plmd1_buf = slurp(p, &n);
    if (!s_plmd1_buf || re15_md1_parse(s_plmd1_buf, (int)n, &s_plmd1) != 0) return 0;
    for (int k = 0; k < 4; k++) {
        hul_t *v = &s_hul[k]; memset(v, 0, sizeof *v); v->ymin = 1 << 30;
        const re15_md1_mesh_t *m = &s_plmd1.meshes[s_hul_mesh[k]];
        for (int pass = 0; pass < 2; pass++) {
            hul_add(v, m->tri_vertices, m->tri_vertex_count, pass);
            hul_add(v, m->quad_vertices, m->quad_vertex_count, pass);
        }
    }
    s_hul_ok = 1;
    return 1;
}
static int hul_lokal(const hul_t *v, int64_t x, int64_t y, int64_t z)
{
    if (y < v->ymin) return 0;
    int b = (int)((y - v->ymin) / HB);
    if (b < 0 || b >= v->nb || !v->ok[b]) return 0;
    return x >= v->x0[b] && x <= v->x1[b] && z >= v->z0[b] && z <= v->z1[b];
}
/* Welt -> Knochen ueber eine WELT-Matrix (M Q12 s16/s32, t): lokal = M^T (P - t). */
static int hul_welt(const hul_t *v, const int32_t M[9], const int32_t t[3], const int32_t P[3])
{
    int64_t dx = P[0] - t[0], dy = P[1] - t[1], dz = P[2] - t[2];
    int64_t x = ((int64_t)M[0] * dx + (int64_t)M[3] * dy + (int64_t)M[6] * dz) >> 12;
    int64_t y = ((int64_t)M[1] * dx + (int64_t)M[4] * dy + (int64_t)M[7] * dz) >> 12;
    int64_t z = ((int64_t)M[2] * dx + (int64_t)M[5] * dy + (int64_t)M[8] * dz) >> 12;
    return hul_lokal(v, x, y, z);
}
/* Port-Pose -> Welt-Matrix eines Knochens: W = Ryaw * R_b, t = Ryaw * t_b + Wurzel (main.c NPC-/Spieler-Zeichner). */
static void welt_matrix(const re15_skel_pose_t *p, int yaw, const re15_actor_t *a, int32_t M[9], int32_t t[3])
{
    int64_t cs = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    const int64_t Y[9] = { cs, 0, sn,  0, 4096, 0,  -sn, 0, cs };
    for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++)
        M[r * 3 + c] = (int32_t)((Y[r * 3 + 0] * p->rot[0 * 3 + c] + Y[r * 3 + 1] * p->rot[1 * 3 + c] +
                                  Y[r * 3 + 2] * p->rot[2 * 3 + c]) >> 12);
    int32_t w[3]; re15_skel_bone_to_world(p->trans, (int16_t)yaw, a->x, a->y, a->z, w);
    t[0] = w[0]; t[1] = w[1]; t[2] = w[2];
}
/* Arm-Vertices (Mesh am Knochen mit Welt-Matrix MA/tA) in Leons Huellen (Welt-Matrizen ML/tL je Huelle). */
static int arm_in_leon(const re15_md1_mesh_t *m, const int32_t MA[9], const int32_t tA[3],
                       int32_t ML[4][9], int32_t tL[4][3], int *gesamt)
{
    int innen = 0;
    for (int q = 0; q < 2; q++) {
        const re15_md1_vertex_t *vv = q ? m->quad_vertices : m->tri_vertices;
        int nv = q ? m->quad_vertex_count : m->tri_vertex_count;
        for (int i = 0; i < nv; i++) {
            int32_t P[3];
            for (int r = 0; r < 3; r++)
                P[r] = tA[r] + (int32_t)(((int64_t)MA[r * 3 + 0] * vv[i].x + (int64_t)MA[r * 3 + 1] * vv[i].y +
                                          (int64_t)MA[r * 3 + 2] * vv[i].z) >> 12);
            int in = 0;
            for (int k = 0; k < 4 && !in; k++) in = hul_welt(&s_hul[k], ML[k], tL[k], P);
            innen += in; (*gesamt)++;
        }
    }
    return innen;
}

static int teil_griff(int argc, char **argv)
{
    if (argc < 7) { printf("griff <raum-hex> <px> <pz> <rot> <vorn|hinten> [budget] [abstand]\n"); return 2; }
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    int32_t px = atoi(argv[3]), pz = atoi(argv[4]);
    int rot = atoi(argv[5]);
    int hinten = (argv[6][0] == 'h');
    int budget = (argc > 7) ? atoi(argv[7]) : 400;
    int abst = (argc > 8) ? atoi(argv[8]) : 700;
    if (getenv("R37J_REIN_RE2")) s_hybrid = 0;
    int rc = raum_laden(room);
    if (rc) return rc;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();

    int zA = -1; long best = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || !re15_re2z_owns_type(e->type)) continue;
        long dx = e->x - px, dz = e->z - pz, d = dx * dx + dz * dz;
        printf("  Zombie Slot %d Typ 0x%02X state %d sub %d pos (%d,%d,%d) rot %d\n", s, e->type,
               e->state, e->sub_state_1, (int)e->x, (int)e->y, (int)e->z, (int)e->rot_y);
        if (best < 0 || d < best) { best = d; zA = s; }
    }
    if (zA < 0) { printf("FAIL: kein Zombie in ROOM%04X\n", room); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s != zA && g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) g_actors[s].active = 0;
    re15_actor_t *z = &g_actors[zA];
    re15_enemy_bank_t *zb = re15_enemy_find(z->type);
    if (!zb || !zb->ok) { printf("SKIP: RE2-Bank Typ 0x%02X fehlt\n", z->type); return 77; }

    /* Leon steht auf (px,pz), Blick rot; der Zombie `abst` Einheiten VOR ihm (vorn) bzw. HINTER ihm
     * (hinten), Blick auf Leon. Vorwaerts eines Aktors = (cos yaw, -sin yaw): genau so dreht
     * re15_clip_root_motion_abs den Wurzelversatz (x += cs*sx + sn*sz, z += -sn*sx + cs*sz; sx = vorwaerts),
     * Zwilling RE2 0x80015CB8 — im RE2-Original-RAM gegengeprueft (Zombie Blick 3147, Haende +z). */
    int32_t sn = re15_sin_q12(rot), cs = re15_cos_q12(rot);
    int vz = hinten ? -1 : 1;
    pl->x = px; pl->z = pz; pl->rot_y = (int16_t)rot;
    z->x = px + vz * (int32_t)(((int64_t)cs * abst) >> 12);
    z->z = pz - vz * (int32_t)(((int64_t)sn * abst) >> 12);
    z->y = pl->y;
    z->rot_y = (int16_t)((rot + (hinten ? 0 : 2048)) & 0x0fff);
    printf("ROOM%04X Greifer Slot %d Typ 0x%02X, Leon (%d,%d) rot %d, Zombie (%d,%d) %s\n", room, zA,
           z->type, (int)px, (int)pz, rot, (int)z->x, (int)z->z, hinten ? "HINTEN" : "VORN");

    int griff_bilder = 0, abweichung_bilder = 0, max_abw = 0, teile_fehl = 0;
    int erster = -1, v_prev = 0;
    for (int f = 0; f < budget; f++) {
        int v = re15_player_victim_state();
        if (v == 0 && !(pl->hit_react & 1)) { pl->x = px; pl->z = pz; pl->rot_y = (int16_t)rot; }
        if (pl->hp < 60) pl->hp = 100;
        frame(0, 0);
        v = re15_player_victim_state();
        if (v == 1 || v == 2) {
            int32_t sx, sz; soll_lage(pl, &sx, &sz);
            int abw = abs((int)(pl->x - sx)) + abs((int)(pl->z - sz));
            if (erster < 0) erster = f;
            griff_bilder++;
            if (abw > 0) abweichung_bilder++;
            if (abw > max_abw) max_abw = abw;
            uint8_t gd[16], gm[16]; uint32_t gt[16];
            int n = zb->skel.bone_count < 16 ? zb->skel.bone_count : 16;
            int on = re15_re2z_gore_resolve(z, zb->skel.bone_parent, n, gd, gt, gm);
            int gezeichnet = 0; for (int i = 0; i < n; i++) gezeichnet += on ? (gd[i] != 0) : 1;
            if (gezeichnet != n) teile_fehl++;
            if (getenv("R37J_VOL") && huelle_laden()) {
                static re15_skel_pose_t ZP[RE15_EMD_MAX_BONES], LP[RE15_EMD_MAX_BONES];
                re15_enemy_bank_t *vb = re15_enemy_find(re15_player_victim_type());
                pl00_laden();
                int okz = 0, okl = 0;
                void *sav = g_anim_pose_actor; g_anim_pose_actor = NULL;
                int kz = kf_von(&zb->anim, z->motion, z->anim_frame);
                okz = (kz >= 0 && re15_skel_compute_pose(&zb->skel, kz, ZP) == 0);
                if (vb && vb->victim_ok && s_pl00_ok) {
                    static re15_emd_skeleton_t vs;
                    vs = s_pl00;
                    vs.keyframe_data = vb->skel_victim.keyframe_data;
                    vs.keyframe_data_size = vb->skel_victim.keyframe_data_size;
                    vs.keyframe_count = vb->skel_victim.keyframe_count;
                    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
                    int kl = kf_von(&vb->anim_victim, pl->motion, pl->anim_frame);
                    okl = (kl >= 0 && re15_skel_compute_pose(&vs, kl, LP) == 0);
                }
                g_anim_pose_actor = sav;
                if (okz && okl) {
                    int32_t ML[4][9], tL[4][3];
                    for (int k = 0; k < 4; k++) welt_matrix(&LP[s_hul_mesh[k]], pl->rot_y, pl, ML[k], tL[k]);
                    const int ab[4] = { 10, 11, 13, 14 };
                    int innen[4], ges[4];
                    for (int a = 0; a < 4; a++) {
                        int mi = zb->remap_ok ? (int)zb->mesh_remap[ab[a]] : ab[a];
                        int32_t MA[9], tA[3]; welt_matrix(&ZP[ab[a]], z->rot_y, z, MA, tA);
                        ges[a] = 0;
                        innen[a] = (mi >= 0 && mi < zb->md1.mesh_count)
                                 ? arm_in_leon(&zb->md1.meshes[mi], MA, tA, ML, tL, &ges[a]) : -1;
                    }
                    printf("    VOL F%d clip %d/%d | U-Arm10 %d/%d Hand11 %d/%d | U-Arm13 %d/%d Hand14 %d/%d\n", f,
                           z->motion, z->anim_frame, innen[0], ges[0], innen[1], ges[1], innen[2], ges[2], innen[3], ges[3]);
                }
            }
            if (getenv("R37J_IK")) {
                /* Reichweite: Hybrid-Schulter -> RE2-Hand (reines RE2-Rig, gleicher Keyframe) gegen die RE1.5-Armlaenge */
                static re15_enemy_bank_t rein; static int rein_ok = -1;
                if (rein_ok < 0) {
                    memset(&rein, 0, sizeof rein);
                    rein_ok = (s_ems && re2_ems_load_bank(s_ems, s_ems_n, (int)z->type, &rein, NULL) == 0) ? 1 : 0;
                }
                if (rein_ok == 1) {
                    int32_t hw[RE15_EMD_MAX_BONES][3], rw[RE15_EMD_MAX_BONES][3];
                    int kz = kf_von(&zb->anim, z->motion, z->anim_frame);
                    int hn = posen_welt(&zb->skel, kz, z, hw, RE15_EMD_MAX_BONES);
                    int rn = posen_welt(&rein.skel, kz, z, rw, RE15_EMD_MAX_BONES);
                    if (hn >= 15 && rn >= 15) {
                        for (int arm = 0; arm < 2; arm++) {
                            int sb = arm ? 12 : 9;
                            double a = sqrt((double)zb->skel.bone_relative_pos[sb + 1][0] * zb->skel.bone_relative_pos[sb + 1][0] +
                                            (double)zb->skel.bone_relative_pos[sb + 1][1] * zb->skel.bone_relative_pos[sb + 1][1] +
                                            (double)zb->skel.bone_relative_pos[sb + 1][2] * zb->skel.bone_relative_pos[sb + 1][2]);
                            double b = sqrt((double)zb->skel.bone_relative_pos[sb + 2][0] * zb->skel.bone_relative_pos[sb + 2][0] +
                                            (double)zb->skel.bone_relative_pos[sb + 2][1] * zb->skel.bone_relative_pos[sb + 2][1] +
                                            (double)zb->skel.bone_relative_pos[sb + 2][2] * zb->skel.bone_relative_pos[sb + 2][2]);
                            printf("    IK F%d Arm%d: Hybrid-Schulter (%d,%d,%d) RE2-Schulter (%d,%d,%d) RE2-Hand (%d,%d,%d) Hybrid-Hand (%d,%d,%d) | d(HybS->RE2H)=%d  a+b=%d  dHand=%d\n",
                                   f, arm, hw[sb][0], hw[sb][1], hw[sb][2], rw[sb][0], rw[sb][1], rw[sb][2],
                                   rw[sb + 2][0], rw[sb + 2][1], rw[sb + 2][2], hw[sb + 2][0], hw[sb + 2][1], hw[sb + 2][2],
                                   abst3(hw[sb], rw[sb + 2]), (int)(a + b), abst3(hw[sb + 2], rw[sb + 2]));
                        }
                    }
                }
            }
            if (getenv("R37J_GEOM")) {
                int32_t zw[RE15_EMD_MAX_BONES][3], lw[RE15_EMD_MAX_BONES][3];
                int zn = posen_welt(&zb->skel, kf_von(&zb->anim, z->motion, z->anim_frame), z, zw, RE15_EMD_MAX_BONES);
                re15_enemy_bank_t *vb = re15_enemy_find(re15_player_victim_type());
                int ln = 0;
                pl00_laden();
                if (vb && vb->victim_ok && s_pl00_ok) {
                    static re15_emd_skeleton_t vs;
                    vs = s_pl00;
                    vs.keyframe_data = vb->skel_victim.keyframe_data;
                    vs.keyframe_data_size = vb->skel_victim.keyframe_data_size;
                    vs.keyframe_count = vb->skel_victim.keyframe_count;
                    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
                    ln = posen_welt(&vs, kf_von(&vb->anim_victim, pl->motion, pl->anim_frame), pl, lw, RE15_EMD_MAX_BONES);
                }
                if (zn >= 15 && ln >= 15) {
                    int hb[2] = { 11, 14 };
                    printf("    GEOM F%d d=%d", f, (int)sqrt((double)(pl->x - z->x) * (pl->x - z->x) + (double)(pl->z - z->z) * (pl->z - z->z)));
                    for (int h = 0; h < 2; h++) {
                        int bk = 0, bd = 1 << 30;
                        for (int k = 0; k < 15; k++) { int d = abst3(zw[hb[h]], lw[k]); if (d < bd) { bd = d; bk = k; } }
                        printf(" | Hand%d (%d,%d,%d) naechst L%d:%d L12:%d L8:%d L9:%d", hb[h], zw[hb[h]][0], zw[hb[h]][1], zw[hb[h]][2],
                               bk, bd, abst3(zw[hb[h]], lw[12]), abst3(zw[hb[h]], lw[8]), abst3(zw[hb[h]], lw[9]));
                    }
                    printf(" | L8 (%d,%d,%d) L9 (%d,%d,%d) L12 (%d,%d,%d)\n", lw[8][0], lw[8][1], lw[8][2],
                           lw[9][0], lw[9][1], lw[9][2], lw[12][0], lw[12][1], lw[12][2]);
                }
            }
            if (getenv("R37J_KLEMME")) {
                /* die Klemme von re15_victim_place nachrechnen: n1 = constrain(alt -> Soll), c1 = constrain(n1 -> n1) */
                static int32_t alt_x = 0, alt_z = 0; static int alt_ok = 0;
                if (!alt_ok) { alt_x = px; alt_z = pz; alt_ok = 1; }
                int32_t n1x = sx, n1z = sz;
                re15_collision_ensure_band(pl->y);
                re15_collision_constrain(&g_room_rdt, alt_x, alt_z, &n1x, &n1z);
                int32_t c1x = n1x, c1z = n1z;
                re15_collision_constrain(&g_room_rdt, n1x, n1z, &c1x, &c1z);
                printf("    KLEMME F%d alt (%d,%d) Soll (%d,%d) -> n1 (%d,%d) c1 (%d,%d)%s | Ist (%d,%d)\n", f, (int)alt_x, (int)alt_z,
                       (int)sx, (int)sz, (int)n1x, (int)n1z, (int)c1x, (int)c1z,
                       (c1x == n1x && c1z == n1z) ? " idempotent" : " NICHT idempotent -> Rueckfall", (int)pl->x, (int)pl->z);
                alt_x = pl->x; alt_z = pl->z;
            }
            if (getenv("R37J_TRACE") || v != v_prev || abw > 0)
                printf("  F%3d v=%d clip=%d fr=%d Leon (%d,%d) Soll (%d,%d) Abw %d | Z ss1=%d ss2=%d clip=%d fr=%d (%d,%d) rot %d | Teile %d/%d\n",
                       f, v, pl->motion, pl->anim_frame, (int)pl->x, (int)pl->z, (int)sx, (int)sz, abw,
                       z->sub_state_1, z->sub_state_2, z->motion, z->anim_frame, (int)z->x, (int)z->z,
                       (int)z->rot_y, gezeichnet, n);
        }
        v_prev = v;
        if (erster >= 0 && v == 0 && f > erster + 10) break;
    }
    printf("ERGEBNIS ROOM%04X %s: Griffbilder %d (erstes F%d), mit Abweichung %d, max %d, Bilder mit fehlenden Teilen %d\n",
           room, hinten ? "HINTEN" : "VORN", griff_bilder, erster, abweichung_bilder, max_abw, teile_fehl);
    CHECK(griff_bilder > 0, "kein Griff zustande gekommen");
    return s_fail ? 1 : 0;
}

/* re2gt <frames.bin> — dasselbe Volumenmass auf der RE2-ORIGINAL-RAM (analysis/befunde_runde37/J_belege/re2_mess/
 * re2_biss_gdb.py: je P3-Halt PL 0x248 + 16 Leon-Parts + Zombie 0x248 + 17 Zombie-Parts, Part-Welt-Matrix +0x48
 * (s16 Q12) / +0x5C (s32)). Arm-Meshes = das RE2-EM010-MD1 (reine RE2-Bank, Part == Mesh), Leon-Huelle = PL00.MD1. */
static int teil_re2gt(int argc, char **argv)
{
    if (argc < 3) { printf("re2gt <frames.bin> [typ]\n"); return 2; }
    size_t n = 0; uint8_t *d = slurp(argv[2], &n);
    if (!d) { printf("FAIL: %s\n", argv[2]); return 1; }
    uint8_t typ = (uint8_t)((argc > 3) ? strtol(argv[3], NULL, 0) : 0x10);
    re15_enemy_reset();
    s_hybrid = 0; load_bank2(typ);
    re15_enemy_bank_t *zb = re15_enemy_find(typ);
    if (!zb || !zb->ok || !huelle_laden()) { printf("SKIP: RE2-Bank/Huelle fehlt\n"); return 77; }
    const size_t ENT = 0x248, PS = 0xAC, REC = 12 + ENT + 16 * PS + ENT + 17 * PS;
    int summe[4] = {0}, bilder = 0;
    for (size_t o = 0; o + REC <= n; o += REC) {
        if (memcmp(d + o, "R2B1", 4) != 0) break;
        uint32_t nr; memcpy(&nr, d + o + 4, 4);
        const uint8_t *lp = d + o + 12 + ENT, *zp = d + o + 12 + ENT + 16 * PS + ENT;
        const uint8_t *ze = d + o + 12 + ENT + 16 * PS;
        int32_t ML[4][9], tL[4][3];
        for (int k = 0; k < 4; k++) {
            const uint8_t *r = lp + s_hul_mesh[k] * PS;
            for (int i = 0; i < 9; i++) { int16_t v; memcpy(&v, r + 0x48 + 2 * i, 2); ML[k][i] = v; }
            for (int i = 0; i < 3; i++) memcpy(&tL[k][i], r + 0x5C + 4 * i, 4);
        }
        const int ab[4] = { 10, 11, 13, 14 };
        int innen[4], ges[4];
        for (int a = 0; a < 4; a++) {
            const uint8_t *r = zp + ab[a] * PS;
            int32_t MA[9], tA[3];
            for (int i = 0; i < 9; i++) { int16_t v; memcpy(&v, r + 0x48 + 2 * i, 2); MA[i] = v; }
            for (int i = 0; i < 3; i++) memcpy(&tA[i], r + 0x5C + 4 * i, 4);
            ges[a] = 0;
            innen[a] = arm_in_leon(&zb->md1.meshes[ab[a]], MA, tA, ML, tL, &ges[a]);
            summe[a] += innen[a];
        }
        uint32_t cw; memcpy(&cw, ze + 0x14C, 4);
        printf("    RE2 Halt %u clip %u/%u | U-Arm10 %d/%d Hand11 %d/%d | U-Arm13 %d/%d Hand14 %d/%d\n", nr,
               cw & 0xFFu, (cw >> 8) & 0xFFu, innen[0], ges[0], innen[1], ges[1], innen[2], ges[2], innen[3], ges[3]);
        bilder++;
    }
    printf("RE2-ORIGINAL %d Halts: Summe innen U-Arm10 %d Hand11 %d U-Arm13 %d Hand14 %d\n", bilder,
           summe[0], summe[1], summe[2], summe[3]);
    return 0;
}

/* skel <typ...> — RE1.5-EMD (CDEMD0.EMS) gegen RE2-EMD (shared_assets/RE2/CDEMD0.EMS): Elternkette, Bindversatz und
 * Vertexzahl je Mesh, damit man sieht, ob die Hybrid-Permutation k_perm_zombie (re2_ems.c) fuer JEDEN Zombie-Typ
 * dasselbe Koerperteil trifft. */
static int teil_skel(int argc, char **argv)
{
    pl00_laden();
    if (s_pl00_ok) {
        printf("PL (%s.EMR): %d Bones\n", getenv("R37J_ELZA") ? "PL04" : "PL00", s_pl00.bone_count);
        for (int b = 0; b < s_pl00.bone_count; b++)
            printf("   b%2d parent %2d bind (%5d,%5d,%5d)\n", b, s_pl00.bone_parent[b], s_pl00.bone_relative_pos[b][0],
                   s_pl00.bone_relative_pos[b][1], s_pl00.bone_relative_pos[b][2]);
    }
    for (int a = 2; a < argc; a++) {
        uint8_t typ = (uint8_t)strtol(argv[a], NULL, 0);
        const char *base = getenv("RE15_ASSET_DIR");
        char p[600]; size_t n = 0;
        snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", (base && *base) ? base : RE15_ASSET_PSX_DIR);
        uint8_t *ems = slurp(p, &n);
        int idx = re15_ems_index_for_type(typ);
        size_t off = 0, len = 0;
        if (!ems || idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) { printf("Typ 0x%02X: RE1.5-EMD fehlt\n", typ); continue; }
        static re15_md1_t md; static re15_emd_skeleton_t sk; static re15_emd_animation_t an;
        if (re15_emd_parse_container(ems + off, len, &md, &sk, &an, NULL) != 0) { printf("Typ 0x%02X: Parse\n", typ); continue; }
        printf("RE1.5 EM0%02X: %d Bones, %d Meshes\n", typ, sk.bone_count, md.mesh_count);
        for (int b = 0; b < sk.bone_count; b++)
            printf("   b%2d parent %2d bind (%5d,%5d,%5d) verts %3d\n", b, sk.bone_parent[b], sk.bone_relative_pos[b][0],
                   sk.bone_relative_pos[b][1], sk.bone_relative_pos[b][2],
                   b < md.mesh_count ? md.meshes[b].tri_vertex_count + md.meshes[b].quad_vertex_count : -1);
        re15_enemy_reset(); s_hybrid = 0; load_bank2(typ);
        re15_enemy_bank_t *eb = re15_enemy_find(typ);
        if (eb && eb->ok) {
            printf("RE2   EM0%02X: %d Bones, %d Meshes\n", typ, eb->skel.bone_count, eb->md1.mesh_count);
            for (int b = 0; b < eb->skel.bone_count; b++)
                printf("   b%2d parent %2d bind (%5d,%5d,%5d) verts %3d\n", b, eb->skel.bone_parent[b], eb->skel.bone_relative_pos[b][0],
                       eb->skel.bone_relative_pos[b][1], eb->skel.bone_relative_pos[b][2],
                       b < eb->md1.mesh_count ? eb->md1.meshes[b].tri_vertex_count + eb->md1.meshes[b].quad_vertex_count : -1);
        }
        free(ems);
    }
    return 0;
}

/* ---- vergleich: Port-Griff gegen das RE2-ORIGINAL (RAM-Fixture) -----------------------------------
 * vergleich <frames.bin> <vorn|hinten> [mit|ohne]
 * Port: ROOM1140, ein Greifer (Typ 0x10, Hybrid wie in der exe), Griff von vorn/hinten auf freier Flaeche; je
 * Griffbild die GEZEICHNETE Pose (re15_skel_compute_pose + re15_bissarm_r37_handkontakt, derselbe Aufruf wie
 * main.c) -> Vektor Zombie-Hand 11/14 minus Leons Schultergelenk 12/9 (Welt, Leon = Opfer-Pose PL00 + Opfer-
 * Keyframes). Original: dieselben Vektoren aus der RAM (Part +0x5C), Halt mit Clipwort-Bild k+1 <-> Port-Bild k
 * (am Haltepunkt stammen die Part-Matrizen vom Zeichnen des Vorbilds, Runde 35 H_raeume.md Abschnitt 6a).
 * Mass je Bild: Abstand Port-Vektor zu RE2-Vektor fuer beide Haende; "ohne" = Handkontakt aus (Stand vorher). */
typedef struct { int ok; int16_t yaw; int32_t h11[3], h14[3], l12[3], l9[3], l8[3]; } griffbild_t;
/* Welt-Vektor in den Rahmen des Greifers (Ryaw^T, Ryaw wie re15_skel_bone_to_world) — die Spielstaende haben
 * verschiedene Blickrichtungen, verglichen wird die Formation, nicht die Weltachse. */
static void in_greiferrahmen(int16_t yaw, int32_t v[3])
{
    int64_t cs = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    int32_t x = (int32_t)((cs * v[0] - sn * v[2]) >> 12), z = (int32_t)((sn * v[0] + cs * v[2]) >> 12);
    v[0] = x; v[2] = z;
}
static griffbild_t s_re2bild[64], s_portbild[64];
static int vergleich_re2_laden(const char *pfad)
{
    size_t n = 0; uint8_t *d = slurp(pfad, &n);
    if (!d) return 0;
    const size_t ENT = 0x248, PS = 0xAC, REC = 12 + ENT + 16 * PS + ENT + 17 * PS;
    int cnt = 0;
    for (size_t o = 0; o + REC <= n; o += REC) {
        if (memcmp(d + o, "R2B1", 4) != 0) break;
        const uint8_t *lp = d + o + 12 + ENT, *ze = d + o + 12 + ENT + 16 * PS, *zp = ze + ENT;
        uint32_t cw; memcpy(&cw, ze + 0x14C, 4);
        if ((cw & 0xFFu) != 12u) continue;                     /* nur der Biss-Loop (Clip 12) */
        int k = (int)((cw >> 8) & 0xFFu) - 1;                  /* Clipwort k+1 <-> Port-Bild k */
        if (k < 0 || k >= 64) continue;
        griffbild_t *g = &s_re2bild[k];
        memcpy(&g->yaw, ze + 0x76, 2);
        memcpy(g->h11, zp + 11 * PS + 0x5C, 12); memcpy(g->h14, zp + 14 * PS + 0x5C, 12);
        memcpy(g->l12, lp + 12 * PS + 0x5C, 12); memcpy(g->l9, lp + 9 * PS + 0x5C, 12); memcpy(g->l8, lp + 8 * PS + 0x5C, 12);
        g->ok = 1; cnt++;
    }
    free(d);
    return cnt;
}
static int s_vgl_bilder, s_vgl_max11, s_vgl_max14, s_vgl_mit11, s_vgl_mit14;
static int s_vgl_sch_n, s_vgl_sch_hoehe, s_vgl_sch_hoehe_max, s_vgl_sch_abst, s_vgl_sch_abst_max, s_vgl_frei_hoehe_max;
static int vergleich_lauf(const char *fixture, int hinten, int mit, int ausgabe)
{
    memset(s_re2bild, 0, sizeof s_re2bild); memset(s_portbild, 0, sizeof s_portbild);
    int nre2 = vergleich_re2_laden(fixture);
    if (nre2 <= 0) { printf("SKIP: RE2-Fixture %s fehlt/leer\n", fixture); return 77; }
    int rc = raum_laden(0x1140);
    if (rc) return rc;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0); re15_player_set_aim_clip_len(12); re15_inv_load_briefing();
    int zA = -1;                                               /* Greifer = erster Typ 0x10 (wie im RE2-Spielstand) */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10 && zA < 0) zA = s;
    if (zA < 0) { printf("FAIL: kein Typ 0x10\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s != zA && g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) g_actors[s].active = 0;
    re15_actor_t *z = &g_actors[zA];
    re15_enemy_bank_t *zb = re15_enemy_find(z->type);
    pl00_laden();
    if (!zb || !zb->ok || !s_pl00_ok) { printf("SKIP: Bank/PL00 fehlt\n"); return 77; }
    const int32_t px = -1000, pz = -19600; const int rot = 0, abst = 900;
    int32_t sn = re15_sin_q12(rot), cs = re15_cos_q12(rot);
    int vz = hinten ? -1 : 1;
    pl->x = px; pl->z = pz; pl->rot_y = (int16_t)rot;
    z->x = px + vz * (int32_t)(((int64_t)cs * abst) >> 12); z->z = pz - vz * (int32_t)(((int64_t)sn * abst) >> 12);
    z->y = 0; z->rot_y = (int16_t)((rot + (hinten ? 0 : 2048)) & 0x0fff);
    int erster = -1;
    for (int f = 0; f < 400; f++) {
        int v = re15_player_victim_state();
        if (v == 0 && !(pl->hit_react & 1)) { pl->x = px; pl->z = pz; pl->rot_y = (int16_t)rot; }
        if (pl->hp < 60) pl->hp = 100;
        frame(0, 0);
        v = re15_player_victim_state();
        if (v != 1) { if (erster >= 0 && f > erster + 40) break; continue; }
        if (erster < 0) erster = f;
        if (z->state != 1 || z->sub_state_1 != 3 || z->motion != 12) continue;
        int k = (int)z->anim_frame;
        if (k < 0 || k >= 64 || s_portbild[k].ok) continue;
        static re15_skel_pose_t ZP[RE15_EMD_MAX_BONES], LP[RE15_EMD_MAX_BONES];
        int kz = kf_von(&zb->anim, z->motion, z->anim_frame);
        void *sav = g_anim_pose_actor; g_anim_pose_actor = NULL;
        int okz = (kz >= 0 && re15_skel_compute_pose(&zb->skel, kz, ZP) == 0);
        re15_enemy_bank_t *vb = re15_enemy_find(re15_player_victim_type());
        static re15_emd_skeleton_t vs; vs = s_pl00;
        int okl = 0, kl = -1;
        if (vb && vb->victim_ok) {
            vs.keyframe_data = vb->skel_victim.keyframe_data; vs.keyframe_data_size = vb->skel_victim.keyframe_data_size;
            vs.keyframe_count = vb->skel_victim.keyframe_count; vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
            kl = kf_von(&vb->anim_victim, pl->motion, pl->anim_frame);
            okl = (kl >= 0 && re15_skel_compute_pose(&vs, kl, LP) == 0);
        }
        g_anim_pose_actor = sav;
        if (!okz || !okl) continue;
        if (mit == 1) re15_bissarm_r37_opfer_merken(pl, &vs, kl);   /* wie main.c: Opfer vor dem Greifer
                                                                     * (mit == 2: ohne Opfer-Bezug, Gegenprobe) */
        if (mit) re15_bissarm_r37_handkontakt(z, &zb->skel, kz, ZP, zb->skel.bone_count);
        griffbild_t *g = &s_portbild[k];
        g->yaw = z->rot_y;
        re15_skel_bone_to_world(ZP[11].trans, z->rot_y, z->x, z->y, z->z, g->h11);
        re15_skel_bone_to_world(ZP[14].trans, z->rot_y, z->x, z->y, z->z, g->h14);
        re15_skel_bone_to_world(LP[12].trans, pl->rot_y, pl->x, pl->y, pl->z, g->l12);
        re15_skel_bone_to_world(LP[9].trans, pl->rot_y, pl->x, pl->y, pl->z, g->l9);
        re15_skel_bone_to_world(LP[8].trans, pl->rot_y, pl->x, pl->y, pl->z, g->l8);
        g->ok = 1;
    }
    int bilder = 0, max11 = 0, max14 = 0, summe11 = 0, summe14 = 0;
    for (int k = 0; k < 64; k++) {
        if (!s_portbild[k].ok || !s_re2bild[k].ok) continue;
        const griffbild_t *a = &s_portbild[k], *b = &s_re2bild[k];
        int32_t va[3], vb2[3], wa[3], wb[3];
        for (int i = 0; i < 3; i++) { va[i] = a->h11[i] - a->l12[i]; vb2[i] = b->h11[i] - b->l12[i];
                                      wa[i] = a->h14[i] - a->l9[i];  wb[i] = b->h14[i] - b->l9[i]; }
        in_greiferrahmen(a->yaw, va); in_greiferrahmen(a->yaw, wa);
        in_greiferrahmen(b->yaw, vb2); in_greiferrahmen(b->yaw, wb);
        int d11 = abst3(va, vb2), d14 = abst3(wa, wb);
        if (ausgabe)
            printf("  Biss-Bild %2d | Hand11-L12 Port (%5d,%5d,%5d) RE2 (%5d,%5d,%5d) Diff %4d | Hand14-L9 Port (%5d,%5d,%5d) RE2 (%5d,%5d,%5d) Diff %4d\n",
                   k, va[0], va[1], va[2], vb2[0], vb2[1], vb2[2], d11, wa[0], wa[1], wa[2], wb[0], wb[1], wb[2], d14);
        bilder++; summe11 += d11; summe14 += d14;
        if (d11 > max11) max11 = d11;
        if (d14 > max14) max14 = d14;
    }
    /* Zweites Mass (Elza-Frage): je Biss-Bild ab 4 (davor Reichweite der RE1.5-Arme) die KONTAKT-Hand = die Hand,
     * die im RE2-Bild einer Schulter (L9/L12) am naechsten liegt — das ist der "dranhaengende" Arm ueber der
     * Schulter — und dort (a) die HOEHE der Hand ueber dieser Schulter, (b) der ABSTAND Hand-Schulter, Port gegen
     * RE2; dazu die Hoehe der FREIEN Hand ueber ihrer naechsten Schulter. Fuer Leon dasselbe wie oben (exakt);
     * fuer Elza misst es, ob die Hand auf IHRER Schulter liegt wie die RE2-Hand auf Leons. */
    {
        int n = 0, sum_h = 0, max_h = 0, sum_a = 0, max_a = 0, max_fh = 0;
        for (int k = 4; k < 64; k++) {
            if (!s_portbild[k].ok || !s_re2bild[k].ok) continue;
            const griffbild_t *a = &s_portbild[k], *b = &s_re2bild[k];
            int dh2[2], da2[2], db2[2], kk2[2];
            for (int hnd = 0; hnd < 2; hnd++) {
                const int32_t *hb = hnd ? b->h14 : b->h11, *ha = hnd ? a->h14 : a->h11;
                int k9 = abst3(hb, b->l9) <= abst3(hb, b->l12);
                const int32_t *sb = k9 ? b->l9 : b->l12, *sa = k9 ? a->l9 : a->l12;
                dh2[hnd] = abs((int)((ha[1] - sa[1]) - (hb[1] - sb[1])));
                da2[hnd] = abs(abst3(ha, sa) - abst3(hb, sb));
                db2[hnd] = abst3(hb, sb);
                kk2[hnd] = k9 ? 9 : 12;
                if (ausgabe && (k % 7) == 0)
                    printf("  Biss-Bild %2d Hand %d an Schulter %d: Hoehe Port %5d RE2 %5d | Abstand Port %4d RE2 %4d\n",
                           k, hnd ? 14 : 11, kk2[hnd], (int)(ha[1] - sa[1]), (int)(hb[1] - sb[1]), abst3(ha, sa), abst3(hb, sb));
            }
            int kh = (db2[1] < db2[0]) ? 1 : 0;                       /* Kontakt-Hand dieses Bildes */
            sum_h += dh2[kh]; sum_a += da2[kh]; n++;
            if (dh2[kh] > max_h) max_h = dh2[kh];
            if (da2[kh] > max_a) max_a = da2[kh];
            if (dh2[1 - kh] > max_fh) max_fh = dh2[1 - kh];
        }
        s_vgl_sch_n = n;
        s_vgl_sch_hoehe = n ? sum_h / n : -1; s_vgl_sch_hoehe_max = max_h;
        s_vgl_sch_abst = n ? sum_a / n : -1;  s_vgl_sch_abst_max = max_a;
        s_vgl_frei_hoehe_max = max_fh;
        printf("KONTAKT %s %s: %d Biss-Bilder ab 4 | Kontakt-Hand: Hoehe ueber der Schulter Abweichung mittel %d max %d,"
               " Abstand Abweichung mittel %d max %d | freie Hand: Hoehe Abweichung max %d\n", hinten ? "HINTEN" : "VORN",
               mit == 1 ? "MIT Handkontakt" : (mit == 2 ? "Handkontakt OHNE Opfer-Bezug" : "OHNE Handkontakt"),
               n, s_vgl_sch_hoehe, max_h, s_vgl_sch_abst, max_a, max_fh);
    }
    s_vgl_bilder = bilder; s_vgl_max11 = max11; s_vgl_max14 = max14;
    s_vgl_mit11 = bilder ? summe11 / bilder : -1; s_vgl_mit14 = bilder ? summe14 / bilder : -1;
    printf("VERGLEICH %s %s: %d Biss-Bilder gegen RE2 (Fixture %d Halts) | Hand 11 mittel %d max %d | Hand 14 mittel %d max %d\n",
           hinten ? "HINTEN" : "VORN", mit ? "MIT Handkontakt" : "OHNE Handkontakt", bilder, nre2,
           s_vgl_mit11, max11, s_vgl_mit14, max14);
    return 0;
}
static int teil_vergleich(int argc, char **argv)
{
    if (argc < 4) { printf("vergleich <frames.bin> <vorn|hinten> [mit|ohne|absolut]\n"); return 2; }
    int hinten = (argv[3][0] == 'h');
    int mit = !(argc > 4 && argv[4][0] == 'o');
    if (argc > 4 && strcmp(argv[4], "absolut") == 0) mit = 2;   /* Handkontakt ohne Opfer-Bezug */
    int rc = vergleich_lauf(argv[2], hinten, mit, 1);
    if (rc) return rc;
    return s_vgl_bilder > 0 ? 0 : 1;
}

/* ======================================================================================================
 * RIEGEL
 * ====================================================================================================== */

/* pruef_re2 <fixture> <vorn|hinten>
 * (A) Handkontakt: die GEZEICHNETE Hand (Pose + re15_bissarm_r37_handkontakt, wie main.c) liegt relativ zu Leons
 * Schultergelenk dort, wo sie im RE2-ORIGINAL liegt (RAM-Fixture aus re2_biss_gdb.py, SLUS-00748_4.sav, P3
 * @0x8010F838). Gemessen 2026-10-12: ohne Handkontakt mittel 222 daneben (Hybrid-Hand ~200 tiefer), mit
 * Handkontakt 0..4 ab Biss-Bild 4; die Bilder 0..3 liegen jenseits der RE1.5-Armlaenge (936 gegen bis 1049
 * benoetigt) — dort ist der Arm gestreckt und die Hand um den Rest kurz (Hand 11 max 83).
 * Schranken: mit Handkontakt Mittel <= 12 je Hand und JEDES Bild ab 4 <= 6; ohne Handkontakt Mittel >= 150
 * (der Riegel misst also den Mechanismus und nicht eine Konstante). */
static int teil_pruef_re2(int argc, char **argv)
{
    if (argc < 4) { printf("pruef_re2 <fixture> <vorn|hinten>\n"); return 2; }
    int hinten = (argv[3][0] == 'h');
    int rc = vergleich_lauf(argv[2], hinten, 0, 0);
    if (rc) return rc;
    int ohne11 = s_vgl_mit11, ohne14 = s_vgl_mit14, ohne_bilder = s_vgl_bilder;
    rc = vergleich_lauf(argv[2], hinten, 1, 1);
    if (rc) return rc;
    CHECK(ohne_bilder >= 20 && s_vgl_bilder >= 20, "zu wenige Biss-Bilder im Vergleich (ohne %d, mit %d)", ohne_bilder, s_vgl_bilder);
    CHECK(ohne11 >= 150 && ohne14 >= 150, "OHNE Handkontakt muss der Hybrid daneben liegen (Mittel %d/%d) — sonst misst der Riegel nichts", ohne11, ohne14);
    CHECK(s_vgl_mit11 <= 12 && s_vgl_mit14 <= 12, "MIT Handkontakt Mittel zu gross (%d/%d, Schranke 12)", s_vgl_mit11, s_vgl_mit14);
    int schlecht = 0;
    for (int k = 4; k < 64; k++) {
        if (!s_portbild[k].ok || !s_re2bild[k].ok) continue;
        const griffbild_t *a = &s_portbild[k], *b = &s_re2bild[k];
        int32_t va[3], vb2[3], wa[3], wb[3];
        for (int i = 0; i < 3; i++) { va[i] = a->h11[i] - a->l12[i]; vb2[i] = b->h11[i] - b->l12[i];
                                      wa[i] = a->h14[i] - a->l9[i];  wb[i] = b->h14[i] - b->l9[i]; }
        in_greiferrahmen(a->yaw, va); in_greiferrahmen(a->yaw, wa);
        in_greiferrahmen(b->yaw, vb2); in_greiferrahmen(b->yaw, wb);
        if (abst3(va, vb2) > 6 || abst3(wa, wb) > 6) { schlecht++; printf("  Biss-Bild %d ueber 6: %d / %d\n", k, abst3(va, vb2), abst3(wa, wb)); }
    }
    CHECK(schlecht == 0, "%d Biss-Bilder (ab 4) weichen > 6 vom RE2-Original ab", schlecht);
    printf("%s pruef_re2 %s: ohne %d/%d -> mit %d/%d (Mittel Hand 11/14), max %d/%d\n", s_fail ? "FAIL" : "OK",
           hinten ? "HINTEN" : "VORN", ohne11, ohne14, s_vgl_mit11, s_vgl_mit14, s_vgl_max11, s_vgl_max14);
    return s_fail ? 1 : 0;
}

/* pruef_elza <fixture> <vorn|hinten>   (Opfer = Elza: der Test laeuft mit R37J_ELZA=1, pl00_laden -> PL04.EMR)
 * (A2) Opfer-Bezug. Elzas Schultern liegen tiefer als Leons; die RE2-Hand AUF Leons Schulter laege bei Elza mit
 * dem absoluten RE2-Ziel ueber ihrer Schulter. Gemessen 2026-10-12 je Biss-Bild ab 4 an der KONTAKT-Hand (die Hand,
 * die im RE2-Bild einer Schulter am naechsten liegt), Abweichung der Hoehe ueber der Schulter gegen RE2/Leon:
 *     vor dem Fix (Hybrid ohne Handkontakt)        vorn 83 (max 92)   hinten 79 (max 96)   -> zu TIEF
 *     Handkontakt mit absolutem RE2-Ziel           vorn 117 (max 132) hinten 121 (max 141) -> zu HOCH
 *     Handkontakt mit Opfer-Bezug                  vorn 0 (max 2)     hinten 2 (max 8)
 *     (Abstand Hand-Schulter mit Opfer-Bezug: vorn 4 (max 11), hinten 6 (max 14); freie Hand Hoehe max 19 / 32)
 * Schranken: Kontakt-Hand Hoehe mittel <= 5, jedes Bild <= 12, Abstand mittel <= 10, jedes Bild <= 20; freie Hand
 * Hoehe jedes Bild <= 40; Gegenprobe absolutes Ziel: Hoehe mittel >= 60 (sonst misst der Riegel nichts). */
static int teil_pruef_elza(int argc, char **argv)
{
    if (argc < 4) { printf("pruef_elza <fixture> <vorn|hinten>\n"); return 2; }
    if (!getenv("R37J_ELZA")) { printf("FAIL: pruef_elza braucht R37J_ELZA=1 (Opfer PL04)\n"); return 1; }
    int hinten = (argv[3][0] == 'h');
    int rc = vergleich_lauf(argv[2], hinten, 2, 0);
    if (rc) return rc;
    int abs_h = s_vgl_sch_hoehe;
    rc = vergleich_lauf(argv[2], hinten, 1, 1);
    if (rc) return rc;
    CHECK(s_vgl_sch_n >= 20, "zu wenige Biss-Bilder ab 4 (%d)", s_vgl_sch_n);
    CHECK(abs_h >= 60, "mit absolutem Ziel muss Elzas Kontakt-Hand hoeher liegen (Hoehe mittel %d) - sonst misst der Riegel nichts", abs_h);
    CHECK(s_vgl_sch_hoehe <= 5, "Kontakt-Hand: Hoehe ueber Elzas Schulter weicht mittel %d ab (Schranke 5)", s_vgl_sch_hoehe);
    CHECK(s_vgl_sch_hoehe_max <= 12, "Kontakt-Hand: Hoehe ueber Elzas Schulter weicht bis %d ab (Schranke 12)", s_vgl_sch_hoehe_max);
    CHECK(s_vgl_sch_abst <= 10, "Kontakt-Hand: Abstand zur Schulter weicht mittel %d ab (Schranke 10)", s_vgl_sch_abst);
    CHECK(s_vgl_sch_abst_max <= 20, "Kontakt-Hand: Abstand zur Schulter weicht bis %d ab (Schranke 20)", s_vgl_sch_abst_max);
    CHECK(s_vgl_frei_hoehe_max <= 40, "freie Hand: Hoehe weicht bis %d ab (Schranke 40)", s_vgl_frei_hoehe_max);
    printf("%s pruef_elza %s: Kontakt-Hand Hoehe ueber der Schulter absolut %d -> mit Opfer-Bezug %d (max %d), Abstand %d (max %d),"
           " freie Hand max %d\n", s_fail ? "FAIL" : "OK", hinten ? "HINTEN" : "VORN", abs_h, s_vgl_sch_hoehe,
           s_vgl_sch_hoehe_max, s_vgl_sch_abst, s_vgl_sch_abst_max, s_vgl_frei_hoehe_max);
    return s_fail ? 1 : 0;
}

/* pruef_klemme <raum-hex> <px> <pz> <rot> <vorn|hinten> [max_abstand]
 * (B) Wandklemme im Griff wie RE2 FUN_8003567c: je Griffbild GENAU EIN Durchgang re15_collision_constrain vom
 * Standpunkt des Vorbilds zur Opfer-Platzierung (Anker + RotY(Yaw+0x158) * Wurzelversatz). Geprueft:
 * (1) ab dem 2. Griffbild ist Ist == dieser eine Durchgang (Toleranz 4 = Rundungsrest der Schraeg-Schieber; im
 *     Zupack-Bild hat der Koerperschub des Greifers Leon schon vor dem Latch verschoben, der Startwert ist dann
 *     dieser Stand — die Sonde kennt ihn nicht genau), also kein Rueckfall auf einen alten Standpunkt;
 * (2) kein Sprung: von Bild zu Bild hoechstens 1200, zum Greifer hoechstens 3000 (vorher ROOM1010: Leon den
 *     ganzen Griff auf (0,0), 5250 entfernt);
 * (3) Leon steht nach dem Durchgang in keiner Wandzelle (constrain(Ist->Ist) aendert hoechstens 4). */
static int teil_pruef_klemme(int argc, char **argv)
{
    if (argc < 7) { printf("pruef_klemme <raum-hex> <px> <pz> <rot> <vorn|hinten> [max_abstand]\n"); return 2; }
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    int32_t px = atoi(argv[3]), pz = atoi(argv[4]);
    int rot = atoi(argv[5]);
    int hinten = (argv[6][0] == 'h');
    int max_abst = (argc > 7) ? atoi(argv[7]) : 3000;
    int rc = raum_laden(room);
    if (rc) return rc;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0); re15_player_set_aim_clip_len(12); re15_inv_load_briefing();
    int zA = -1; long best = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || !re15_re2z_owns_type(e->type)) continue;
        long dx = e->x - px, dz = e->z - pz, d = dx * dx + dz * dz;
        if (best < 0 || d < best) { best = d; zA = s; }
    }
    if (zA < 0) { printf("FAIL: kein Zombie in ROOM%04X\n", room); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s != zA && g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) g_actors[s].active = 0;
    re15_actor_t *z = &g_actors[zA];
    const int abst = 700;
    int32_t sn = re15_sin_q12(rot), cs = re15_cos_q12(rot);
    int vz = hinten ? -1 : 1;
    pl->x = px; pl->z = pz; pl->rot_y = (int16_t)rot;
    z->x = px + vz * (int32_t)(((int64_t)cs * abst) >> 12); z->z = pz - vz * (int32_t)(((int64_t)sn * abst) >> 12);
    z->y = 0; z->rot_y = (int16_t)((rot + (hinten ? 0 : 2048)) & 0x0fff);
    int griff = 0, falsch = 0, weit = 0, in_wand = 0, max_d = 0;
    int32_t alt_x = 0, alt_z = 0; int v_alt = 0;
    for (int f = 0; f < 500; f++) {
        int v = re15_player_victim_state();
        if (v == 0 && !(pl->hit_react & 1)) { pl->x = px; pl->z = pz; pl->rot_y = (int16_t)rot; }
        if (pl->hp < 60) pl->hp = 100;
        int32_t vor_x = pl->x, vor_z = pl->z;
        frame(0, 0);
        v = re15_player_victim_state();
        if (v == 1) {
            if (v_alt != 1) { alt_x = vor_x; alt_z = vor_z; }        /* Zupack-Bild: Startwert = Stand */
            int32_t sx, sz; soll_lage(pl, &sx, &sz);
            int32_t nx = sx, nz = sz;
            re15_collision_ensure_band(pl->y);
            re15_collision_constrain(&g_room_rdt, alt_x, alt_z, &nx, &nz);
            if (griff > 0 && abs((int)(pl->x - nx)) + abs((int)(pl->z - nz)) > 4) {
                falsch++;
                if (falsch <= 5) printf("  F%d Ist (%d,%d) != ein Durchgang (%d,%d) (alt (%d,%d), Soll (%d,%d))\n", f, (int)pl->x, (int)pl->z,
                                        (int)nx, (int)nz, (int)alt_x, (int)alt_z, (int)sx, (int)sz);
            }
            int32_t cx = pl->x, cz = pl->z;
            re15_collision_constrain(&g_room_rdt, pl->x, pl->z, &cx, &cz);
            if (abs((int)(cx - pl->x)) + abs((int)(cz - pl->z)) > 4) in_wand++;
            int d = (int)sqrt((double)(pl->x - z->x) * (pl->x - z->x) + (double)(pl->z - z->z) * (pl->z - z->z));
            int sprung = (int)sqrt((double)(pl->x - alt_x) * (pl->x - alt_x) + (double)(pl->z - alt_z) * (pl->z - alt_z));
            if (d > max_d) max_d = d;
            if (d > max_abst || sprung > 1200) weit++;
            alt_x = pl->x; alt_z = pl->z;
            griff++;
        }
        v_alt = v;
        if (griff > 0 && v == 0 && f > 60) break;
    }
    printf("pruef_klemme ROOM%04X %s: %d Griffbilder, %d ohne den einen Durchgang, %d mit Sprung > 1200 oder weiter als %d (max Abstand %d), %d in einer Wandzelle\n",
           room, hinten ? "HINTEN" : "VORN", griff, falsch, weit, max_abst, max_d, in_wand);
    CHECK(griff >= 30, "kein Griff zustande gekommen (%d Bilder)", griff);
    CHECK(falsch == 0, "%d Bilder weichen vom einen Wandklemm-Durchgang ab (Rueckfall/Startwert)", falsch);
    CHECK(weit == 0, "%d Bilder mit Sprung > 1200 oder Leon weiter als %d vom Greifer (alter Standpunkt)", weit, max_abst);
    CHECK(in_wand == 0, "%d Bilder mit Leon in einer Wandzelle", in_wand);
    return s_fail ? 1 : 0;
}

/* ======================================================================================================
 * ECHTLAUF-AUSWERTER (tests/integration/test_r37_bissarm_echtlauf.cmake)
 * ====================================================================================================== */

/* pruef_armlog <bissarm.log> [min_biss=20] [opfer: leon|elza]
 * (A) im Echtlauf: die exe schreibt mit RE15_BISSARM_LOG je GEZEICHNETEM Griffbild die Hand-Gelenke 11/14 nach
 * re15_bissarm_r37_handkontakt und den RE2-Zielpunkt (Welt, engine/src/bissarm_r37.c). Geprueft:
 * (1) der Haken im NPC-Zeichner (platform/pc/main.c) laeuft im Griff ueberhaupt (>= 30 Zeilen; vor dem Fix: Datei
 *     fehlt);
 * (2) in JEDER Zeile liegt die gezeichnete Hand hoechstens |rest|+3 vom Ziel: rest 0 = erreicht, rest > 0 = Ziel
 *     jenseits der RE1.5-Armlaenge (Arm gestreckt darauf gerichtet), rest < 0 = Ziel naeher an der Schulter als
 *     |Oberarm-Unterarm|+1 (Arm ganz gebeugt; gemessen im Abwurf-Clip 13); 3 = Rundung der Weltumrechnung;
 * (3) mindestens `min_biss` Biss-Bilder (Clip 12, P3 @0x80102838) mit BEIDEN Haenden auf dem Ziel (rest 0,
 *     Abstand <= 2);
 * (4) Opfer-Bezug (re15_bissarm_r37_opfer_merken, PL00.PLD ueber re15_asset_read_file): "leon" = in JEDER Zeile 0
 *     (Leon ist der Bezug), "elza" = in den Biss-Bildern im Mittel >= 50 in Y (Elzas Schultern liegen tiefer;
 *     gemessen 2026-10-12 ~100) — belegt, dass die exe PL00.PLD findet und den Bezug anlegt. */
static int teil_pruef_armlog(int argc, char **argv)
{
    if (argc < 3) { printf("pruef_armlog <bissarm.log> [min_biss]\n"); return 2; }
    int min_biss = (argc > 3) ? atoi(argv[3]) : 20;
    int opfer = (argc > 4) ? (argv[4][0] == 'e' ? 2 : 1) : 0;   /* 0 = nicht pruefen, 1 = Leon, 2 = Elza */
    FILE *f = fopen(argv[2], "r");
    if (!f) { printf("FAIL: %s fehlt - der Handkontakt-Haken im Zeichner lief nie\n", argv[2]); return 1; }
    char line[640];
    int zeilen = 0, biss = 0, biss_auf = 0, daneben = 0, max_rest = 0, max_d = 0;
    int bezug_zeilen = 0, bezug_nicht0 = 0; long bezug_y_summe = 0;
    while (fgets(line, sizeof line, f)) {
        unsigned fr = 0, typ = 0; int ss2 = 0, clip = 0, af = 0, r0 = 0, r1 = 0;
        int a[6], b[6];
        if (sscanf(line, "F%u typ=0x%X ss2=%d clip=%d fr=%d hand11=(%d,%d,%d) ziel11=(%d,%d,%d) rest11=%d"
                         " hand14=(%d,%d,%d) ziel14=(%d,%d,%d) rest14=%d",
                   &fr, &typ, &ss2, &clip, &af, &a[0], &a[1], &a[2], &b[0], &b[1], &b[2], &r0,
                   &a[3], &a[4], &a[5], &b[3], &b[4], &b[5], &r1) != 19) continue;
        zeilen++;
        int32_t h0[3] = { a[0], a[1], a[2] }, z0[3] = { b[0], b[1], b[2] };
        int32_t h1[3] = { a[3], a[4], a[5] }, z1[3] = { b[3], b[4], b[5] };
        int d0 = abst3(h0, z0), d1 = abst3(h1, z1);
        if (d0 > max_d) max_d = d0;
        if (d1 > max_d) max_d = d1;
        if (abs(r0) > max_rest) max_rest = abs(r0);
        if (abs(r1) > max_rest) max_rest = abs(r1);
        if (d0 > abs(r0) + 3 || d1 > abs(r1) + 3) {
            daneben++;
            if (daneben <= 5) printf("  F%u clip %d fr %d: Hand 11 %d vom Ziel (rest %d), Hand 14 %d (rest %d)\n",
                                     fr, clip, af, d0, r0, d1, r1);
        }
        if (clip == 12) { biss++; if (r0 == 0 && r1 == 0 && d0 <= 2 && d1 <= 2) biss_auf++; }
        const char *bz = strstr(line, " bezug11=(");
        int bzv[6];
        if (bz && sscanf(bz, " bezug11=(%d,%d,%d) bezug14=(%d,%d,%d)", &bzv[0], &bzv[1], &bzv[2], &bzv[3], &bzv[4], &bzv[5]) == 6) {
            bezug_zeilen++;
            if (bzv[0] || bzv[1] || bzv[2] || bzv[3] || bzv[4] || bzv[5]) bezug_nicht0++;
            if (clip == 12) bezug_y_summe += abs(bzv[1]) + abs(bzv[4]);
        }
    }
    fclose(f);
    int bezug_y_mittel = biss ? (int)(bezug_y_summe / (2 * biss)) : 0;
    printf("pruef_armlog: Opfer-Bezug in %d von %d Zeilen ungleich 0, |Y| im Biss mittel %d\n", bezug_nicht0, bezug_zeilen, bezug_y_mittel);
    if (opfer == 1) CHECK(bezug_zeilen == zeilen && bezug_nicht0 == 0, "Leon: Opfer-Bezug muss 0 sein (%d von %d Zeilen ungleich 0)", bezug_nicht0, bezug_zeilen);
    if (opfer == 2) CHECK(bezug_zeilen == zeilen && bezug_y_mittel >= 50, "Elza: Opfer-Bezug fehlt (|Y| mittel %d, %d Zeilen) - PL00.PLD nicht gefunden?", bezug_y_mittel, bezug_zeilen);
    printf("pruef_armlog: %d Griffbilder gezeichnet, %d Biss-Bilder (Clip 12), davon %d mit beiden Haenden auf dem "
           "RE2-Ziel; %d Zeilen weiter als |rest|+3; max |Rest| %d, max Abstand Hand-Ziel %d\n",
           zeilen, biss, biss_auf, daneben, max_rest, max_d);
    CHECK(zeilen >= 30, "nur %d Griffbilder mit Handkontakt im Zeichner (Haken nicht im Griff gelaufen)", zeilen);
    CHECK(daneben == 0, "%d Griffbilder: gezeichnete Hand weiter als |rest|+3 vom RE2-Ziel", daneben);
    CHECK(biss_auf >= min_biss, "nur %d Biss-Bilder mit beiden Haenden auf dem RE2-Ziel (Schranke %d)", biss_auf, min_biss);
    return s_fail ? 1 : 0;
}

/* pruef_griffweg <state.log> [max_abstand=2000] [min_griff=30]
 * (B) im Echtlauf: RE15_STATE_LOG (main.c, eine Zeile je Bild: PL(x,z,..), gr=, je Gegner [slot .. st ss1 .. mo ..
 * @(x,z,r)]). In jedem Bild mit gr=1 wird der Greifer gesucht (RE2-Exec[3]: st=1 ss1=3, @0x801025EC). Geprueft:
 * >= min_griff Griffbilder; im HALTEN (Greifer-Clip 11 Zupacken / 12 Biss) steht Leon nie weiter als max_abstand
 * vom Greifer; im ganzen Griff kein Sprung > 1200 von Bild zu Bild und nie (0,0). Im Abwurf (Clip 13) laeuft der
 * Greifer mit seiner Clip-Wurzel davon (gemessen bis 2841), deshalb dort kein Abstandsmass.
 * Vorher (Bau 419380dc) ROOM1010: Leon den ganzen Griff auf (0,0), ~5000 vom Greifer; nachher max ~1100. */
static int teil_pruef_griffweg(int argc, char **argv)
{
    if (argc < 3) { printf("pruef_griffweg <state.log> [max_abstand] [min_griff]\n"); return 2; }
    int max_abst = (argc > 3) ? atoi(argv[3]) : 2000;
    int min_griff = (argc > 4) ? atoi(argv[4]) : 30;
    FILE *f = fopen(argv[2], "r");
    if (!f) { printf("FAIL: %s fehlt\n", argv[2]); return 1; }
    static char line[8192];
    int griff = 0, halten = 0, weit = 0, spruenge = 0, null_lage = 0, max_d = 0, erster = -1, letzter = -1;
    int alt_ok = 0, alt_x = 0, alt_z = 0;
    while (fgets(line, sizeof line, f)) {
        unsigned fr = 0;
        if (sscanf(line, "F%u", &fr) != 1) continue;
        const char *g = strstr(line, " gr=");
        if (!g || atoi(g + 4) != 1) { alt_ok = 0; continue; }
        int px = 0, pz = 0;
        const char *pp = strstr(line, "PL(");
        if (!pp || sscanf(pp, "PL(%d,%d,", &px, &pz) != 2) continue;
        int gefunden = 0, gx = 0, gz = 0, gmo = -1;
        for (const char *q = strstr(line, " ["); q; q = strstr(q + 2, " [")) {
            int slot = 0, st = 0, ss1 = 0, ex = 0, ez = 0; unsigned t = 0;
            if (sscanf(q, " [%d t=%x st=%d ss1=%d", &slot, &t, &st, &ss1) != 4) continue;
            const char *at = strstr(q, "@(");
            const char *mo = strstr(q, " mo=");
            if (!at || !mo || sscanf(at, "@(%d,%d,", &ex, &ez) != 2) continue;
            if (st == 1 && ss1 == 3 && re15_re2z_owns_type(t)) { gefunden = 1; gx = ex; gz = ez; gmo = atoi(mo + 4); break; }
        }
        if (!gefunden) continue;
        griff++;
        if (erster < 0) erster = (int)fr;
        letzter = (int)fr;
        if (px == 0 && pz == 0) null_lage++;
        if (gmo == 11 || gmo == 12) {
            halten++;
            int d = (int)sqrt((double)(px - gx) * (px - gx) + (double)(pz - gz) * (pz - gz));
            if (d > max_d) max_d = d;
            if (d > max_abst) { weit++; if (weit <= 5) printf("  F%u Leon (%d,%d) Greifer (%d,%d): %d entfernt\n", fr, px, pz, gx, gz, d); }
        }
        if (alt_ok) {
            int sp = (int)sqrt((double)(px - alt_x) * (px - alt_x) + (double)(pz - alt_z) * (pz - alt_z));
            if (sp > 1200) { spruenge++; if (spruenge <= 5) printf("  F%u Sprung %d: (%d,%d) -> (%d,%d)\n", fr, sp, alt_x, alt_z, px, pz); }
        }
        alt_ok = 1; alt_x = px; alt_z = pz;
    }
    fclose(f);
    printf("pruef_griffweg: %d Griffbilder (F%d..F%d), davon %d im Halten (Clip 11/12): max Abstand Leon-Greifer %d "
           "(Schranke %d), %d weiter; %d Spruenge > 1200, %d auf (0,0)\n",
           griff, erster, letzter, halten, max_d, max_abst, weit, spruenge, null_lage);
    CHECK(griff >= min_griff, "nur %d Griffbilder (Schranke %d) - kein Griff im Lauf", griff, min_griff);
    CHECK(halten >= min_griff / 2, "nur %d Halte-Bilder (Clip 11/12)", halten);
    CHECK(weit == 0, "%d Halte-Bilder mit Leon weiter als %d vom Greifer", weit, max_abst);
    CHECK(spruenge == 0, "%d Spruenge > 1200 im Griff", spruenge);
    CHECK(null_lage == 0, "%d Griffbilder mit Leon auf (0,0)", null_lage);
    return s_fail ? 1 : 0;
}

/* region <raum-hex> <state.log>
 * Messwerkzeug: liegt im Griff (gr=1) der Ursprung des Greifers ausserhalb des Region-Vierecks des aktiven Cuts,
 * waehrend Leon darin liegt? Dann zeichnet der Port den ganzen Greifer nicht (main.c `npc_region_culled`, Original
 * FUN_8001e8c8 @0x8001e970-7c: Punkt-im-Viereck mit DAT_800ac790; aussen nur zusammensetzen @0x8001e9b4) und
 * Leon ringt allein. Zaehlt je Lauf: Griffbilder, Leon innen, Greifer innen, Leon innen + Greifer aussen. */
static int teil_region(int argc, char **argv)
{
    if (argc < 4) { printf("region <raum-hex> <state.log>\n"); return 2; }
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR, (room >> 12) & 0xFu, room);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT %s\n", path); return 1; }
    FILE *f = fopen(argv[3], "r");
    if (!f) { printf("FAIL: %s fehlt\n", argv[3]); return 1; }
    static char line[8192];
    int griff = 0, l_in = 0, g_in = 0, allein = 0, ohne_quad = 0;
    while (fgets(line, sizeof line, f)) {
        unsigned fr = 0;
        if (sscanf(line, "F%u", &fr) != 1) continue;
        const char *g = strstr(line, " gr=");
        if (!g || atoi(g + 4) != 1) continue;
        int px = 0, pz = 0, cam = -1;
        const char *pp = strstr(line, "PL(");
        const char *cp = strstr(line, " cam=");
        if (!pp || !cp || sscanf(pp, "PL(%d,%d,", &px, &pz) != 2) continue;
        cam = atoi(cp + 5);
        int gefunden = 0, gx = 0, gz = 0;
        for (const char *q = strstr(line, " ["); q; q = strstr(q + 2, " [")) {
            int slot = 0, st = 0, ss1 = 0, ex = 0, ez = 0; unsigned t = 0;
            if (sscanf(q, " [%d t=%x st=%d ss1=%d", &slot, &t, &st, &ss1) != 4) continue;
            const char *at = strstr(q, "@(");
            if (!at || sscanf(at, "@(%d,%d,", &ex, &ez) != 2) continue;
            if (st == 1 && ss1 == 3 && re15_re2z_owns_type(t)) { gefunden = 1; gx = ex; gz = ez; break; }
        }
        if (!gefunden) continue;
        griff++;
        int16_t xs[4], zs[4];
        if (!re15_rdt_get_region_quad(&s_rdt, cam, xs, zs)) { ohne_quad++; l_in++; g_in++; continue; }
        int li = re15_aot_point_in_quad(px, pz, xs, zs), gi = re15_aot_point_in_quad(gx, gz, xs, zs);
        l_in += li; g_in += gi;
        if (li && !gi) { allein++; if (allein <= 5) printf("  F%u cam %d: Leon (%d,%d) innen, Greifer (%d,%d) AUSSEN\n", fr, cam, px, pz, gx, gz); }
    }
    fclose(f);
    printf("region ROOM%04X: %d Griffbilder, Leon innen %d, Greifer innen %d, Leon innen + Greifer aussen %d (ohne Viereck %d)\n",
           room, griff, l_in, g_in, allein, ohne_quad);
    return 0;
}

/* re15_kontakt <ems 0|1> <vorn|hinten>      (Opfer Leon PL00, mit R37J_ELZA=1 Elza PL04 — wie pl00_laden)
 * Messwerkzeug fuer die Elza-Frage: der RE1.5-ORIGINAL-Griff (RE1.5-Gehirn, Griff FUN_80102548, Biss-Schleife
 * +0x6 == 3 @0x801027dc) mit der RE1.5-Bank EM10 aus EMD/CDEMD<ems>.EMS (Aktionsbank dir[3]/dir[4], Opferbank
 * dir[5]/dir[6]; CDEMD0 und CDEMD1 unterscheiden sich GENAU in diesen vier Abschnitten, Netz/Skelett dir[2]/[7]/[8]
 * gleich). Gemessen wird, wo die Zombie-Hand relativ zur Schulter des Opfers liegt: RE1.5-Knochen Hand rechts 10,
 * links 13 (re2_ems.c k_perm_zombie: RE2-Slot 11/14), Opfer Schulter links 12 / rechts 9. Ausgabe: Mittel der
 * Vektoren Hand - Schulter im Greiferrahmen ueber die Biss-Schleife. */
static int raum_laden15(unsigned room, int ems)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR, (room >> 12) & 0xFu, room);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("SKIP: %s nicht lesbar\n", path); return 77; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse %s\n", path); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (int)room;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    char ep[600]; size_t en = 0;
    snprintf(ep, sizeof ep, "%s/EMD/CDEMD%d.EMS", (base && *base) ? base : RE15_ASSET_PSX_DIR, ems);
    uint8_t *e = slurp(ep, &en);
    if (!e) { printf("SKIP: %s fehlt\n", ep); return 77; }
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++) {
        if (!g_actors[sl].active || !re15_re2z_owns_type(g_actors[sl].type)) continue;
        uint8_t type = g_actors[sl].type;
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (eb && eb->ok) continue;
        int idx = re15_ems_index_for_type(type);
        size_t off = 0, len = 0;
        if (idx < 0 || re15_ems_get_entry(e, en, idx, &off, &len) != 0) continue;
        if (!eb) eb = re15_enemy_alloc(type);
        if (!eb) continue;
        uint8_t *blob = (uint8_t *)malloc(len); memcpy(blob, e + off, len);   /* bleibt fuer die Laufzeit liegen */
        if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) continue;
        eb->ok = 1; eb->buf = NULL;
        eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
        eb->loco_ok   = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    }
    free(e);
    return 0;
}
static int teil_re15_kontakt(int argc, char **argv)
{
    if (argc < 4) { printf("re15_kontakt <ems 0|1> <vorn|hinten>\n"); return 2; }
    int ems = atoi(argv[2]);
    int hinten = (argv[3][0] == 'h');
    int rc = raum_laden15(0x1140, ems);
    if (rc) return rc;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0); re15_player_set_aim_clip_len(12); re15_inv_load_briefing();
    int zA = -1;
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++)
        if (g_actors[sl].active && g_actors[sl].type == 0x10 && zA < 0) zA = sl;
    if (zA < 0) { printf("FAIL: kein Typ 0x10\n"); return 1; }
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++)
        if (sl != zA && g_actors[sl].active && re15_re2z_owns_type(g_actors[sl].type)) g_actors[sl].active = 0;
    re15_actor_t *z = &g_actors[zA];
    re15_enemy_bank_t *zb = re15_enemy_find(z->type);
    pl00_laden();
    if (!zb || !zb->ok || !zb->victim_ok || !s_pl00_ok) { printf("SKIP: Bank/Opferbank/PL fehlt\n"); return 77; }
    const int32_t px = -1000, pz = -19600; const int rot = 0, abst = 900;
    int32_t sn = re15_sin_q12(rot), cs = re15_cos_q12(rot);
    int vz = hinten ? -1 : 1;
    pl->x = px; pl->z = pz; pl->rot_y = (int16_t)rot;
    z->x = px + vz * (int32_t)(((int64_t)cs * abst) >> 12); z->z = pz - vz * (int32_t)(((int64_t)sn * abst) >> 12);
    z->y = 0; z->rot_y = (int16_t)((rot + (hinten ? 0 : 2048)) & 0x0fff);
    int bilder = 0, erster = -1;
    double sum[2][3] = { { 0 } };
    for (int f = 0; f < 500; f++) {
        int v = re15_player_victim_state();
        if (v == 0 && !(pl->hit_react & 1)) { pl->x = px; pl->z = pz; pl->rot_y = (int16_t)rot; }
        if (pl->hp < 60) pl->hp = 100;
        frame(0, 0);
        v = re15_player_victim_state();
        if (v != 1) { if (erster >= 0 && f > erster + 60) break; continue; }
        if (erster < 0) erster = f;
        if (z->state != 1 || (z->sub_state_1 != 3 && z->sub_state_1 != 4) || z->sub_state_2 != 3) continue;
        static re15_skel_pose_t ZP[RE15_EMD_MAX_BONES], LP[RE15_EMD_MAX_BONES];
        int kz = kf_von(&zb->anim, z->motion, z->anim_frame);
        static re15_emd_skeleton_t vs; vs = s_pl00;
        vs.keyframe_data = zb->skel_victim.keyframe_data; vs.keyframe_data_size = zb->skel_victim.keyframe_data_size;
        vs.keyframe_count = zb->skel_victim.keyframe_count; vs.keyframe_size_bytes = zb->skel_victim.keyframe_size_bytes;
        int kl = kf_von(&zb->anim_victim, pl->motion, pl->anim_frame);
        void *sav = g_anim_pose_actor; g_anim_pose_actor = NULL;
        int ok = (kz >= 0 && kl >= 0 && re15_skel_compute_pose(&zb->skel, kz, ZP) == 0 && re15_skel_compute_pose(&vs, kl, LP) == 0);
        g_anim_pose_actor = sav;
        if (!ok) continue;
        int32_t h[2][3], sch[2][3];
        re15_skel_bone_to_world(ZP[10].trans, z->rot_y, z->x, z->y, z->z, h[0]);
        re15_skel_bone_to_world(ZP[13].trans, z->rot_y, z->x, z->y, z->z, h[1]);
        re15_skel_bone_to_world(LP[12].trans, pl->rot_y, pl->x, pl->y, pl->z, sch[0]);
        re15_skel_bone_to_world(LP[9].trans,  pl->rot_y, pl->x, pl->y, pl->z, sch[1]);
        for (int a = 0; a < 2; a++) {
            int32_t vv[3] = { h[a][0] - sch[a][0], h[a][1] - sch[a][1], h[a][2] - sch[a][2] };
            in_greiferrahmen(z->rot_y, vv);
            for (int i = 0; i < 3; i++) sum[a][i] += vv[i];
            if (bilder % 8 == 0 && a == 0)
                printf("  F%d clip %d fr %d | Hand10-Schulter12 (%5d,%5d,%5d)", f, (int)z->motion, (int)z->anim_frame, vv[0], vv[1], vv[2]);
            if (bilder % 8 == 0 && a == 1) printf(" | Hand13-Schulter9 (%5d,%5d,%5d)\n", vv[0], vv[1], vv[2]);
        }
        bilder++;
    }
    if (bilder == 0) { printf("FAIL: keine Biss-Schleife (RE1.5-Griff) erreicht\n"); return 1; }
    printf("RE15_KONTAKT CDEMD%d %s %s: %d Biss-Bilder | Mittel Hand10-Schulter12 (%d,%d,%d) | Hand13-Schulter9 (%d,%d,%d) | Opfer %d Knochen\n",
           ems, getenv("R37J_ELZA") ? "ELZA" : "LEON", hinten ? "HINTEN" : "VORN", bilder,
           (int)(sum[0][0] / bilder), (int)(sum[0][1] / bilder), (int)(sum[0][2] / bilder),
           (int)(sum[1][0] / bilder), (int)(sum[1][1] / bilder), (int)(sum[1][2] / bilder), s_pl00.bone_count);
    return 0;
}

/* kriecher [budget]
 * Messwerkzeug: der KRIECHER-Griff (Kriecher-Hirn @0x80101210, EXEC-Tabelle @0x8010C918[1] = 0x801025EC, dieselbe
 * Griff-Funktion wie aufrecht [3]; Kriech-Variante ueber +0x10E Bit 0 @0x8010266C-74). ROOM1030, ein Zombie als
 * Kriecher (re15_re2z_enter_crawler wie probe_re2z_crawl_attack), Leon 700 daneben. Je Griffbild: Abstand der
 * Hybrid-Hand (RE1.5-Bindlaengen, gezeichnet) und der RE2-Rig-Hand (RE2-Bind, gleicher Keyframe) zum naechsten
 * Bein-Gelenk Leons (PL00 Knochen 2..7), sowie |RE2-Hand - Hybrid-Hand|. */
static int teil_kriecher(int argc, char **argv)
{
    int budget = (argc > 2) ? atoi(argv[2]) : 900;
    int rc = raum_laden(0x1030);
    if (rc) return rc;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0); re15_player_set_aim_clip_len(12); re15_inv_load_briefing();
    int cs[2], nc = 0;                                 /* zwei Kriecher wie probe_re2z_crawl_attack (mit einem */
    for (int sl = 1; sl < RE15_ACTOR_MAX && nc < 2; sl++)   /* greift keiner: gemessen 0 Griffe in 1800 Bildern) */
        if (g_actors[sl].active && re15_re2z_owns_type(g_actors[sl].type)) cs[nc++] = sl;
    if (nc == 0) { printf("FAIL: kein RE2-Zombie in ROOM1030\n"); return 1; }
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++) {
        int keep = 0;
        for (int i = 0; i < nc; i++) if (cs[i] == sl) keep = 1;
        if (!keep && g_actors[sl].active && re15_re2z_owns_type(g_actors[sl].type)) g_actors[sl].active = 0;
    }
    for (int i = 0; i < nc; i++) re15_re2z_enter_crawler(&g_actors[cs[i]], NULL, 0u);
    re15_actor_t *c0 = &g_actors[cs[0]];
    pl->x = c0->x + 700; pl->z = c0->z; pl->y = c0->y; pl->floor = c0->floor;
    pl00_laden();
    if (!s_pl00_ok) { printf("SKIP: PL00 fehlt\n"); return 77; }
    int bilder = 0, max_dhr = 0, sum_dhr = 0;
    long sum_hy = 0, sum_r2 = 0;
    for (int f = 0; f < budget; f++) {
        if (pl->hp < 60) pl->hp = 100;
        frame(0, 0);
        re15_actor_t *z = NULL;
        for (int i = 0; i < nc; i++) {
            re15_actor_t *q = &g_actors[cs[i]];
            if (q->active && q->state == 1 && (q->re2z_f10e & 1u) && q->sub_state_1 == 1) z = q;
        }
        if (!z || re15_player_victim_state() != 1) continue;
        re15_enemy_bank_t *zb = re15_enemy_find(z->type);
        const int16_t (*b2)[3] = re15_bissarm_r37_re2_bind(z->type);
        if (!zb || !zb->ok || !b2) continue;
        static re15_skel_pose_t HU[RE15_EMD_MAX_BONES], RU[RE15_EMD_MAX_BONES], LP[RE15_EMD_MAX_BONES];
        static re15_emd_skeleton_t r2, vs;
        int kz = kf_von(&zb->anim, z->motion, z->anim_frame);
        r2 = zb->skel; memcpy(r2.bone_relative_pos, b2, sizeof r2.bone_relative_pos); r2.root_y_fix = NULL; r2.root_y_fix_count = 0;
        re15_enemy_bank_t *vb = re15_enemy_find(re15_player_victim_type());
        if (!vb || !vb->victim_ok) continue;
        vs = s_pl00;
        vs.keyframe_data = vb->skel_victim.keyframe_data; vs.keyframe_data_size = vb->skel_victim.keyframe_data_size;
        vs.keyframe_count = vb->skel_victim.keyframe_count; vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
        int kl = kf_von(&vb->anim_victim, pl->motion, pl->anim_frame);
        void *sav = g_anim_pose_actor; g_anim_pose_actor = NULL;
        int ok = kz >= 0 && kl >= 0 && re15_skel_compute_pose(&zb->skel, kz, HU) == 0 && re15_skel_compute_pose(&r2, kz, RU) == 0
                 && re15_skel_compute_pose(&vs, kl, LP) == 0;
        g_anim_pose_actor = sav;
        if (!ok) continue;
        for (int a = 0; a < 2; a++) {
            int hb = a ? 14 : 11;
            int32_t hy[3], r2w[3];
            re15_skel_bone_to_world(HU[hb].trans, z->rot_y, z->x, z->y, z->z, hy);
            re15_skel_bone_to_world(RU[hb].trans, z->rot_y, z->x, z->y, z->z, r2w);
            int best_hy = 1 << 30, best_r2 = 1 << 30;
            for (int lb = 2; lb <= 7; lb++) {
                int32_t lw[3];
                re15_skel_bone_to_world(LP[lb].trans, pl->rot_y, pl->x, pl->y, pl->z, lw);
                int d1 = abst3(hy, lw), d2 = abst3(r2w, lw);
                if (d1 < best_hy) best_hy = d1;
                if (d2 < best_r2) best_r2 = d2;
            }
            int dhr = abst3(hy, r2w);
            sum_hy += best_hy; sum_r2 += best_r2; sum_dhr += dhr;
            if (dhr > max_dhr) max_dhr = dhr;
            if (bilder % 10 == 0)
                printf("  F%d clip %d fr %d Hand %d: Hybrid %4d / RE2-Rig %4d vom naechsten Bein-Gelenk, |RE2-Hybrid| %4d\n",
                       f, (int)z->motion, (int)z->anim_frame, hb, best_hy, best_r2, dhr);
        }
        bilder++;
    }
    if (!bilder) { printf("FAIL: kein Kriecher-Griff in %d Bildern\n", budget); return 1; }
    printf("KRIECHER: %d Griffbilder | mittel Hybrid-Hand %ld / RE2-Rig-Hand %ld vom naechsten Bein-Gelenk | |RE2-Hybrid| mittel %d max %d\n",
           bilder, sum_hy / (2 * bilder), sum_r2 / (2 * bilder), sum_dhr / (2 * bilder), max_dhr);
    return 0;
}

/* karte <datei> <raum-hex> <char 0|4> <x> <y> <z> <rot> <hp> <cut> <waffe|0> <flags|->
 * Schreibt Slot 0 einer PSX-Karte (wie test_r36_kleidung karte) — zusaetzlich die ausgeruestete Waffe
 * (Briefing-Inventar: 1 Messer, 3 Browning) und Spielflags "zone:bit,..." (z.B. die Kill-Flags der
 * Em-Status-Zone 7 = Sce_em_set pc[7], Gate @0x80042120-38: ein gesetztes Flag unterdrueckt den Spawn,
 * so steht im Raum genau EIN Greifer). Messwerkzeug fuer die exe-Laeufe (CONTINUE). */
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_gameflow.h"
static int teil_karte(int argc, char **argv)
{
    if (argc < 13) { printf("karte <datei> <raum-hex> <char> <x> <y> <z> <rot> <hp> <cut> <waffe> <flags|->\n"); return 2; }
    const char *path = argv[2];
    unsigned room = (unsigned)strtoul(argv[3], NULL, 16);
    int chr = atoi(argv[4]);
    scd_vm_init(); re15_actor_init(); re15_aot_init();
    g_current_room_id = (int)room;
    g_gameflow.character = (uint8_t)chr;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0;
    pl->x = atoi(argv[5]); pl->y = atoi(argv[6]); pl->z = atoi(argv[7]);
    pl->rot_y = (int16_t)atoi(argv[8]); pl->hp = (int16_t)atoi(argv[9]);
    g_scd.cam_id = (uint8_t)atoi(argv[10]);
    re15_inv_load_briefing();
    int waffe = (int)strtol(argv[11], NULL, 0);
    if (waffe > 0) {
        int s2 = re15_inv_find_item((uint8_t)waffe);
        if (s2 >= 0) { re15_inv_set_equipped_slot(s2); re15_player_set_equipped_weapon(waffe); }
    }
    const char *f = argv[12];
    if (strcmp(f, "-") != 0) {
        const char *q = f;
        while (*q) {
            int zone = (int)strtol(q, (char **)&q, 0);
            if (*q == ':') q++;
            int bit = (int)strtol(q, (char **)&q, 0);
            re15_game_flag_set((uint8_t)zone, (uint8_t)bit, 1);
            while (*q == ',' || *q == ' ') q++;
        }
    }
    re15_savedata_t sd; re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, chr ? "ELZA  BISS" : "LEON  BISS") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path); return 1;
    }
    printf("Karte %s: Raum %04X char %d pos (%d,%d,%d) rot %d hp %d cut %d waffe %d (slot %d) flags %s\n",
           path, room, chr, (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y, (int)pl->hp,
           (int)g_scd.cam_id, waffe, re15_inv_equipped_slot(), f);
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "";
    if (strcmp(teil, "griff") == 0) return teil_griff(argc, argv);
    if (strcmp(teil, "karte") == 0) return teil_karte(argc, argv);
    if (strcmp(teil, "re2gt") == 0) return teil_re2gt(argc, argv);
    if (strcmp(teil, "skel") == 0) return teil_skel(argc, argv);
    if (strcmp(teil, "vergleich") == 0) return teil_vergleich(argc, argv);
    if (strcmp(teil, "pruef_re2") == 0) return teil_pruef_re2(argc, argv);
    if (strcmp(teil, "pruef_klemme") == 0) return teil_pruef_klemme(argc, argv);
    if (strcmp(teil, "pruef_elza") == 0) return teil_pruef_elza(argc, argv);
    if (strcmp(teil, "kriecher") == 0) return teil_kriecher(argc, argv);
    if (strcmp(teil, "pruef_armlog") == 0) return teil_pruef_armlog(argc, argv);
    if (strcmp(teil, "pruef_griffweg") == 0) return teil_pruef_griffweg(argc, argv);
    if (strcmp(teil, "region") == 0) return teil_region(argc, argv);
    if (strcmp(teil, "re15_kontakt") == 0) return teil_re15_kontakt(argc, argv);
    printf("Teile: griff | karte | re2gt | skel | vergleich | pruef_re2 | pruef_klemme | pruef_armlog | pruef_griffweg | region | re15_kontakt | pruef_elza | kriecher\n");
    return 2;
}
