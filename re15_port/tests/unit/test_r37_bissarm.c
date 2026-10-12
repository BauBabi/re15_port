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
    printf("Teile: griff | karte\n");
    return 2;
}
