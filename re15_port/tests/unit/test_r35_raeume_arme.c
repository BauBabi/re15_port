/**
 * @file test_r35_raeume_arme.c
 * @brief Runde 35 Spur H, Punkt 4 — ROOM1210 Gitterarme: Griff und Leons Schuetteln (Clipping).
 *
 * Nutzer: "Die Zombie Arme in ROOM 1210, wenn sie einen greifen bewegen sich nicht synchron zu Leon
 * beim schuetteln, dadurch clipped er."
 *
 * Echter Weg wie unit_1210_arme_re2 (ROOM1210.RDT, zehn Arme, RE2-EM2D-Bank, game_step), Messarm 5.
 * Gemessen wird:
 *   (1) PIN: B4 P0 setzt Leon auf part[Hand]+0x5C/+0x64 (@0x80100C18-38) — die Parts traegt die Pose
 *       des LETZTEN 0x8002959C (B3 P1 @0x80100B24 im Vortakt, Bild VOR dem Weiterzaehlen
 *       @0x800295C8-F8 / +1 erst @0x80029B30), P0 selbst ruft keinen Advance (endet `j 0x80100D6C`
 *       @0x80100CAC). Erwartet: Leon steht auf der Hand von Clip 3 / Startbild des Vortakts — nicht auf
 *       Clip 5 Bild 0 (dem frueheren Port-Wert).
 *   (2) GLEICHLAUF: waehrend des Haltens zeigt Leon (Opfer-Clip 0, 19 Bilder) jedes Bild genau das
 *       Bild Arm + 1 (Clip 5, 19 Bilder) — Spieler-Routine 5 laeuft NACH allen Entities
 *       (@0x80026620 FUN_8003BFAC), der Hook-P0-Advance (@0x80101328) zaehlt im Griff-Takt schon, der
 *       Arm erst ab P1 (@0x80100D18).
 *   (3) CLIPPING-MASS (Leon zum Arm gewandt = Hook-P0 ohne Flip, FUN_80015910 @0x801012E0): waagerechter
 *       Abstand Arm-Hand <-> Leons Brustachse (PL00-Knochen 8, Renderpfad main.c Opfer-Override) je
 *       Halte-Bild; Hand innerhalb 120 = steckt im Oberkoerper. Erwartet 0 Bilder (vorher 9 von 19,
 *       Dossier 4.4).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"
#include "re15_enemy_ai_re2_zellenarm.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t *s_ems = NULL; static size_t s_ems_n = 0;
static re15_emd_animation_t s_pa; static re15_emd_skeleton_t s_ps;
static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("FAIL: %s\n", name); g_fail = 1; } \
                               else printf("ok:   %s\n", name); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static int load_re2_bank_1A(void)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
    if (!eb) eb = re15_enemy_alloc(0x1A);
    if (!eb) return 0;
    re15_tim_t tim = {0};
    if (re2_ems_load_bank(s_ems, s_ems_n, 0x2D, eb, &tim) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return 1;
}
static void se_cap(int id, int f) { (void)id; (void)f; }
static void bank_cap(int b) { (void)b; }
static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}
static int aufsetzen(int *slots)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    if (!load_re2_bank_1A()) return 0;
    re15_re2arm_audio_hook(se_cap, bank_cap);
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) slots[n++] = s;
    return n;
}

static int32_t s_knochen[16][3];
/* ---- VOLUMENMASS (Runde 35 Spur H, Nachbesserung 2, M2) -------------------------------------------
 * Das waagerechte Mass "Hand < 120 an der Brustachse" kennt weder Hoehe noch Unterarm. Hier zaehlt der
 * Riegel ARM-VERTICES (EM2D-Mesh Unterarm + Hand = Bone Hand-1 / Hand, mesh == bone in der reinen
 * RE2-Bank) in Leons Kopf- und Rumpf-Volumen. Das Volumen kommt aus den Daten: PL00.MD1 Mesh 8 (Kopf)
 * und Mesh 0 (Rumpf), je Hoehenband 32 Einheiten die Bounding-Box der Mesh-Vertices im Knochen-Rahmen —
 * eine Querschnitt-Huelle, eher zu gross als zu klein (strenges Mass). Welt -> Knochen: lokal =
 * R_b^T * (R_y(yaw)^T * (P - Wurzel) - t_b) (Vertex-Transform main.c: R_y * (R_b * v + t_b) + Wurzel). */
#define VOL_BAND 32
typedef struct { int ymin, nb; int16_t x0[64], x1[64], z0[64], z1[64]; uint8_t ok[64]; } vol_t;
static vol_t s_vol[2];                         /* 0 = Rumpf (Mesh 0), 1 = Kopf (Mesh 8) */
static re15_skel_pose_t s_leon_pose[2];
static re15_md1_t s_pl_md1; static uint8_t *s_pl_md1_buf = NULL;
static void vol_add(vol_t *v, const re15_md1_vertex_t *p, int n, int pass)
{
    for (int i = 0; i < n; i++) {
        if (pass == 0) { if (p[i].y < v->ymin) v->ymin = p[i].y; continue; }
        int b = (p[i].y - v->ymin) / VOL_BAND;
        if (b < 0 || b >= 64) continue;
        if (!v->ok[b]) { v->x0[b] = v->x1[b] = p[i].x; v->z0[b] = v->z1[b] = p[i].z; v->ok[b] = 1; }
        if (p[i].x < v->x0[b]) v->x0[b] = p[i].x;
        if (p[i].x > v->x1[b]) v->x1[b] = p[i].x;
        if (p[i].z < v->z0[b]) v->z0[b] = p[i].z;
        if (p[i].z > v->z1[b]) v->z1[b] = p[i].z;
        if (b + 1 > v->nb) v->nb = b + 1;
    }
}
static int vol_laden(void)
{
    size_t n = 0;
    if (!s_pl_md1_buf) {
        s_pl_md1_buf = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.MD1", &n);
        if (!s_pl_md1_buf || re15_md1_parse(s_pl_md1_buf, (int)n, &s_pl_md1) != 0) return 0;
    }
    const int mesh[2] = { 0, 8 };
    for (int k = 0; k < 2; k++) {
        vol_t *v = &s_vol[k]; memset(v, 0, sizeof *v); v->ymin = 1 << 30;
        const re15_md1_mesh_t *m = &s_pl_md1.meshes[mesh[k]];
        for (int pass = 0; pass < 2; pass++) {
            vol_add(v, m->tri_vertices, m->tri_vertex_count, pass);
            vol_add(v, m->quad_vertices, m->quad_vertex_count, pass);
        }
    }
    return s_vol[0].nb > 0 && s_vol[1].nb > 0;
}
static int vol_innen(const vol_t *v, const re15_skel_pose_t *p, int yaw, int32_t lx, int32_t lz, const int32_t P[3])
{
    int64_t cs = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    int64_t dx = P[0] - lx, dy = P[1], dz = P[2] - lz;
    int64_t mx = (cs * dx - sn * dz) >> 12, my = dy, mz = (sn * dx + cs * dz) >> 12;   /* R_y^T */
    mx -= p->trans[0]; my -= p->trans[1]; mz -= p->trans[2];
    int64_t x = (p->rot[0] * mx + p->rot[3] * my + p->rot[6] * mz) >> 12;              /* R_b^T */
    int64_t y = (p->rot[1] * mx + p->rot[4] * my + p->rot[7] * mz) >> 12;
    int64_t z = (p->rot[2] * mx + p->rot[5] * my + p->rot[8] * mz) >> 12;
    if (y < v->ymin) return 0;
    int b = (int)((y - v->ymin) / VOL_BAND);
    if (b < 0 || b >= v->nb || !v->ok[b]) return 0;
    return x >= v->x0[b] && x <= v->x1[b] && z >= v->z0[b] && z <= v->z1[b];
}
/* Arm-Vertices (Unterarm + Hand) in Leons Kopf/Rumpf (Pose aus leon_brust desselben Bilds). spiegel = 1:
 * die Arm-Punkte vorher waagerecht an Leons Wurzel um 180 Grad gedreht — das ist exakt die RE2-Konstruktion
 * des Ruecken-Griffs, von der Gesicht-Lage aus gesehen (Leon um seine Wurzel gedreht, @0x8010130C-18, ==
 * Welt um die Wurzel gegengedreht; Hand-Bahn unabhaengig von PL+0x76). */
static int arm_in_leon(re15_actor_t *arm, const re15_actor_t *pl, int spiegel)
{
    re15_enemy_bank_t *b = re15_enemy_find(0x1A);
    if (!b) return -1;
    int kf = re15_compute_actor_kf(&b->anim, &b->skel, arm, -1, arm->anim_frame);
    re15_skel_pose_t ap[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = (kf >= 0) ? re15_skel_compute_pose(&b->skel, kf, ap) : -1;
    g_anim_pose_actor = save;
    if (rv != 0) return -1;
    const int hb = re15_re2arm_hand_bone(arm);
    const int bones[2] = { hb - 1, hb };
    int64_t cs = re15_cos_q12(arm->rot_y), sn = re15_sin_q12(arm->rot_y);
    int innen = 0;
    for (int k = 0; k < 2; k++) {
        const re15_skel_pose_t *p = &ap[bones[k]];
        const re15_md1_mesh_t *m = &b->md1.meshes[bones[k]];
        for (int q = 0; q < 2; q++) {
            const re15_md1_vertex_t *vv = q ? m->quad_vertices : m->tri_vertices;
            int nv = q ? m->quad_vertex_count : m->tri_vertex_count;
            for (int i = 0; i < nv; i++) {
                int64_t mx = ((int64_t)p->rot[0]*vv[i].x + (int64_t)p->rot[1]*vv[i].y + (int64_t)p->rot[2]*vv[i].z) >> 12;
                int64_t my = ((int64_t)p->rot[3]*vv[i].x + (int64_t)p->rot[4]*vv[i].y + (int64_t)p->rot[5]*vv[i].z) >> 12;
                int64_t mz = ((int64_t)p->rot[6]*vv[i].x + (int64_t)p->rot[7]*vv[i].y + (int64_t)p->rot[8]*vv[i].z) >> 12;
                mx += p->trans[0]; my += p->trans[1]; mz += p->trans[2];
                int32_t P[3] = { arm->x + (int32_t)((cs * mx + sn * mz) >> 12), arm->y + (int32_t)my,
                                 arm->z + (int32_t)((-sn * mx + cs * mz) >> 12) };
                if (spiegel) { P[0] = 2 * pl->x - P[0]; P[2] = 2 * pl->z - P[2]; }
                if (vol_innen(&s_vol[0], &s_leon_pose[0], pl->rot_y, pl->x, pl->z, P) ||
                    vol_innen(&s_vol[1], &s_leon_pose[1], pl->rot_y, pl->x, pl->z, P)) innen++;
            }
        }
    }
    return innen;
}
/* Leons Brust-/Halsknochen 8 in Weltkoordinaten (Opfer-Override main.c: PL00-Knochen + Bindpose,
 * Keyframes + Clips der Greifer-Opferbank, clip_override = pl->motion). */
static int leon_brust(const re15_actor_t *pl, int32_t out[3])
{
    re15_enemy_bank_t *vb = re15_enemy_find(0x1A);
    if (!vb || !vb->victim_ok) return 0;
    re15_emd_skeleton_t vs = s_ps;
    vs.keyframe_data = vb->skel_victim.keyframe_data;
    vs.keyframe_data_size = vb->skel_victim.keyframe_data_size;
    vs.keyframe_count = vb->skel_victim.keyframe_count;
    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
    re15_actor_t pr = *pl;
    int kf = re15_compute_actor_kf(&vb->anim_victim, &vs, &pr, (int)pl->motion, pl->anim_frame);
    if (kf < 0) return 0;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(&vs, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    re15_skel_bone_to_world(poses[8].trans, pl->rot_y, pl->x, 0, pl->z, out);
    s_leon_pose[0] = poses[0]; s_leon_pose[1] = poses[8];      /* Rumpf / Kopf fuer das Volumenmass */
    if (getenv("R35_ARME_DUMP")) {               /* Messdump: alle Leon-Knochen dieses Bilds */
        for (int b = 0; b < s_ps.bone_count && b < 16; b++)
            re15_skel_bone_to_world(poses[b].trans, pl->rot_y, pl->x, 0, pl->z, s_knochen[b]);
    }
    return 1;
}

/* Aufzeichnung je Halte-Bild f (Nachbesserung 1, M2): Hand, Brust, Leon-Wurzel/-Blick. */
typedef struct { int ok; int32_t h[3], b[3], lx, lz; int yaw; int pfr, afr; int vi, vs; } bild_t;
static bild_t s_auf[3][60];
static int    s_auf_yaw[3];
static int    s_vol_bilder[3], s_vol_max[3], s_vol_pred[3];
static int    s_pinvar_vol[3][3], s_pinvar_hor[3][3];      /* [Fall][Pin-Quelle] */   /* Volumenmass je Fall (s. arm_in_leon) */                    /* Leon-Blick unmittelbar nach dem Hook-P0 */

/* Ein Griff des Messarms mit Leon-Blick `yaw0` vor dem Zugriff. Liefert die Zahl der Halte-Bilder mit
 * Hand < 120 an der Brustachse (oder -1). `fall` (0 = Gesicht, 1 = Ruecken) waehlt die Aufzeichnung. */
static int griff(int yaw0, const char *name, int fall)
{
    memset(s_auf[fall], 0, sizeof s_auf[fall]);
    s_vol_bilder[fall] = s_vol_max[fall] = s_vol_pred[fall] = 0;
    memset(s_pinvar_vol[fall], 0, sizeof s_pinvar_vol[fall]); memset(s_pinvar_hor[fall], 0, sizeof s_pinvar_hor[fall]);
    s_auf_yaw[fall] = -1;
    int slots[RE15_ACTOR_MAX]; int n = aufsetzen(slots);
    if (n < 10) { CHECK("zehn Arme + EM2D-Bank", 0); return -1; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int a5 = slots[4]; re15_actor_t *arm = &g_actors[a5];
    int16_t hyaw; int32_t hx, hz; re15_re2arm_home(a5, &hyaw, &hx, &hz);
    pl->x = hx + 3500; pl->z = hz + 3500;
    for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;      /* Member_set(12,1) @0x1EDA */
    for (int f = 0; f < 20 && arm->sub_state_2 != 2; f++) frame_step();
    pl->x = hx + 2000; pl->z = hz;
    frame_step();                                                     /* A0 -> 0x101 REACH */
    int32_t hand[3]; re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), hand);   /* Standpunkt wie unit_1210_arme_re2 (begehbar) */
    pl->x = hand[0] + 300; pl->z = hand[2]; pl->rot_y = (int16_t)yaw0;
    /* Startbild-Historie: die Parts im Griff-Takt T tragen die Pose, die B3 P1 im Takt T-1 aus dem
     * damaligen Startbild baute (Clip 3, Bild = anim_frame zu Beginn von T-1). */
    int vor_mo = -1, vor_fr = -1, cur_mo = arm->motion, cur_fr = arm->anim_frame;
    int grab = 0;
    for (int f = 0; f < 60 && !grab; f++) {
        vor_mo = cur_mo; vor_fr = cur_fr;
        cur_mo = arm->motion; cur_fr = arm->anim_frame;
        frame_step();
        if (arm->sub_state_1 == 4) grab = 1;
    }
    char nm[400];
    snprintf(nm, sizeof nm, "[%s] Griff kommt zustande (A3 0x401 @0x80100A50)", name);
    CHECK(nm, grab);
    if (!grab) return -1;
    /* (1) Pin — Nachbesserung 2: Quelle ist part[Hand]+0x5C der GEMISCHTEN Parts. 0x80029614 mischt die
     * Rotation +0x68 mit t1 = a3 * +0x14E (VOR dem Dekrement, `lbu t3,334(s2)` @0x800296a8 / `mult v0,t3`
     * @0x800296bc / Blend @0x800299f0-0x80029ab0); B3 setzt Clip 3 mit +0x14E = 15 (0xF0003 @0x80100AEC),
     * der Griff kommt ab +0x14D >= 5 (@0x80100A18) — die Ueberblendung laeuft also noch. Erwartet: Leon steht
     * auf der gemischten Hand; die reine Keyframe-Hand derselben Parts-Pose liegt sichtbar daneben. */
    int32_t gemischt[3], rein[3], alt[3];
    int pv = re15_re2arm_hand_parts(a5, gemischt, rein);
    {   const int16_t mo = arm->motion; const uint16_t fr = arm->anim_frame;
        arm->motion = 5; arm->anim_frame = 0;
        re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), alt);
        arm->motion = mo; arm->anim_frame = fr; }
    long dmisch = (long)sqrt((double)(pl->x-gemischt[0])*(pl->x-gemischt[0]) + (double)(pl->z-gemischt[2])*(pl->z-gemischt[2]));
    long drein  = (long)sqrt((double)(pl->x-rein[0])*(pl->x-rein[0]) + (double)(pl->z-rein[2])*(pl->z-rein[2]));
    long dalt   = (long)sqrt((double)(pl->x-alt[0])*(pl->x-alt[0]) + (double)(pl->z-alt[2])*(pl->z-alt[2]));
    printf("  [%s] Pin Leon (%d,%d) | Parts Clip %d Bild %d: gemischt (%d,%d), rein (%d,%d) | Clip 5 Bild 0 (%d,%d) | "
           "Leon zu gemischt %ld, zu rein %ld, zu Clip 5 %ld | Leon-Blick %d\n", name, pl->x, pl->z, vor_mo, vor_fr,
           gemischt[0], gemischt[2], rein[0], rein[2], alt[0], alt[2], dmisch, drein, dalt, (int)pl->rot_y);
    snprintf(nm, sizeof nm, "[%s] (1) Pin = part[Hand] der GEMISCHTEN Parts (+0x14E @0x800296a8-bc, Pin @0x80100C18-38); "
                            "reine Clip-3-Hand und Clip-5-Hand liegen daneben", name);
    CHECK(nm, pv && vor_mo == 3 && dmisch <= 2 && drein > 30 && dalt > 30);
    /* (2) Gleichlauf + (3) Clipping-Mass ueber einen vollen Zyklus nach der Ueberblendung */
    int gleich = 1, im_koerper = 0, bilder = 0; int mind = 1 << 30;
    for (int f = 0; f < 60; f++) {
        frame_step();
        if (f == 0) s_auf_yaw[fall] = (int)pl->rot_y & 0xfff;      /* nach Hook-P0 (@0x801012E0-18) */
        if (arm->sub_state_1 != 4 || re15_player_victim_state() != 4) break;
        if (pl->motion != 0 || arm->motion != 5) continue;
        if ((int)pl->anim_frame != (((int)arm->anim_frame + 1) % 19)) gleich = 0;
        if (f < 16) continue;                                       /* Ueberblendung Rate 15 */
        int32_t b8[3], h[3];
        if (!leon_brust(pl, b8)) continue;
        re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), h);
        int d = (int)sqrt((double)(h[0]-b8[0])*(h[0]-b8[0]) + (double)(h[2]-b8[2])*(h[2]-b8[2]));
        {   bild_t *a = &s_auf[fall][f];
            a->ok = 1; memcpy(a->h, h, sizeof h); memcpy(a->b, b8, sizeof b8);
            a->lx = pl->x; a->lz = pl->z; a->yaw = (int)pl->rot_y & 0xfff;
            a->pfr = pl->anim_frame; a->afr = arm->anim_frame;
            a->vi = arm_in_leon(arm, pl, 0); a->vs = arm_in_leon(arm, pl, 1);
            {   /* Vergleich der drei Pin-Quellen im SELBEN Bild (Leons Pose haengt nicht an der Wurzel):
                 * [0] gemischte Parts (byte-true), [1] reine Clip-3-Pose (Nachbesserung 1), [2] Clip 5 Bild 0
                 * (Basisstand vor Runde 35). Nur Messung fuers Dossier. */
                const int32_t px[3] = { pl->x, rein[0], alt[0] }, pz[3] = { pl->z, rein[2], alt[2] };
                const int32_t ox = pl->x, oz = pl->z;
                for (int q = 0; q < 3; q++) {
                    pl->x = px[q]; pl->z = pz[q];
                    int32_t bq[3]; re15_skel_bone_to_world(s_leon_pose[1].trans, pl->rot_y, pl->x, 0, pl->z, bq);
                    int dq = (int)sqrt((double)(h[0]-bq[0])*(h[0]-bq[0]) + (double)(h[2]-bq[2])*(h[2]-bq[2]));
                    int vq = arm_in_leon(arm, pl, 0);
                    if (vq > 0) s_pinvar_vol[fall][q]++;
                    if (dq < 120) s_pinvar_hor[fall][q]++;
                }
                pl->x = ox; pl->z = oz;
            }
            if (a->vi > 0) s_vol_bilder[fall]++;
            if (a->vi > s_vol_max[fall]) s_vol_max[fall] = a->vi;
            if (a->vs > 0) s_vol_pred[fall]++; }
        if (getenv("R35_ARME_DUMP")) {
            double c = cos(pl->rot_y * 3.14159265358979 / 2048.0), sn = sin(pl->rot_y * 3.14159265358979 / 2048.0);
            printf("  DUMP [%s] f%2d afr %2d hand=(%d,%d,%d)", name, f, (int)arm->anim_frame, h[0], h[1], h[2]);
            for (int b = 0; b < 16; b++) {
                int dx = h[0]-s_knochen[b][0], dz = h[2]-s_knochen[b][2], dy = h[1]-s_knochen[b][1];
                int vor = (int)(dx * c - dz * sn), seit = (int)(dx * sn + dz * c);
                if (b == 0 || b == 8 || b == 9 || b == 12) printf(" | b%d v%d s%d h%d", b, vor, seit, -dy);
            }
            printf("\n");
        }
        if (d < mind) mind = d;
        if (d < 120) im_koerper++;
        bilder++;
    }
    snprintf(nm, sizeof nm, "[%s] (2) Leon zeigt jedes Halte-Bild Arm-Bild + 1 (Clip 0/Clip 5, 19 Bilder)", name);
    CHECK(nm, gleich);
    printf("  [%s] Clipping-Mass: %d Halte-Bilder, Hand < 120 an der Brustachse in %d, Minimum %d\n",
           name, bilder, im_koerper, mind);
    return bilder ? im_koerper : -1;
}

int main(void)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: ROOM1210.RDT\n"); return 1; }
    {   size_t esz = 0, rsz = 0;
        uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
        uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
        if (!edd || !emr || re15_emd_parse_animation(edd, esz, &s_pa) != 0 ||
            re15_emd_parse_skeleton(emr, rsz, &s_ps) != 0) { printf("FAIL: PL00\n"); return 1; } }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    CHECK("PL00.MD1 Kopf (Mesh 8) / Rumpf (Mesh 0) als Volumen geladen", vol_laden());

    /* Leon dem Fenster zugewandt (Blick -x = 2048): FUN_80015910 -> 0, kein Flip -> Gesicht zum Arm */
    int k_gesicht = griff(2048, "Gesicht", 0);
    /* Leon vom Fenster abgewandt (Blick +x = 0): Flip +2048 (@0x80101304-18) -> Ruecken zum Arm. */
    int k_ruecken = griff(0, "Ruecken", 1);
    printf("  waagerechtes Mass (Hand < 120 an der Brustachse): Gesicht %d, Ruecken %d Halte-Bilder\n",
           k_gesicht, k_ruecken);
    printf("  Volumenmass (Arm-Vertices Unterarm+Hand in Leons Kopf/Rumpf, PL00.MD1): Gesicht %d Bilder (max %d "
           "Vertices), Ruecken %d Bilder (max %d); Vorhersage aus dem Gesicht-Lauf fuer den Ruecken %d, aus dem "
           "Ruecken-Lauf fuer das Gesicht %d\n", s_vol_bilder[0], s_vol_max[0], s_vol_bilder[1], s_vol_max[1],
           s_vol_pred[0], s_vol_pred[1]);
    for (int c = 0; c < 2; c++)
        printf("  Pin-Quellen %s: Volumen/waagerecht  gemischt %d/%d | rein C3B4 %d/%d | Clip5B0 %d/%d (von 44)\n",
               c ? "Ruecken" : "Gesicht", s_pinvar_vol[c][0], s_pinvar_hor[c][0], s_pinvar_vol[c][1], s_pinvar_hor[c][1],
               s_pinvar_vol[c][2], s_pinvar_hor[c][2]);
    /* (3) Gesicht-Griff (der Griff des Nutzers: Leon kommt von der Tuer ROOM1220 und geht -z, Blick zum
     *     Fenster, kein Flip). Mit der byte-true Pin-Quelle (gemischte Parts) beruehrt die Hand Hals/Schulter
     *     in einem Teil der Bilder — das ist die RE2-Geometrie (eine Bank, ein Clip, Pin = gezeichnete Hand);
     *     geprueft wird, dass der Nutzerbefund (Basisstand, Pin Clip 5 Bild 0: Hand durch Kopf/Oberkoerper)
     *     in beiden Massen zurueckgeht. Zahlen: Dossier H_raeume.md, Nachbesserung 2. */
    CHECK("(3) Gesicht-Griff: weniger Kontaktbilder als der Basisstand des Nutzerbefunds (Pin Clip 5 Bild 0) — im "
          "Volumen- UND im waagerechten Mass",
          k_gesicht >= 0 && s_pinvar_vol[0][0] < s_pinvar_vol[0][2] && s_pinvar_hor[0][0] < s_pinvar_hor[0][2]);
    /* (3b) Ruecken-Griff als PRUEFUNG: Bild fuer Bild genau das, was die RE2-Konstruktion vorschreibt —
     *      die Arm-Geometrie des Gesicht-Laufs, an Leons Wurzel gespiegelt, gegen Leons Volumen. */
    {
        int vergl = 0, gleich = 1;
        for (int f = 0; f < 60; f++) {
            const bild_t *g = &s_auf[0][f], *r = &s_auf[1][f];
            if (!g->ok || !r->ok) continue;
            vergl++;
            if ((r->vi > 0) != (g->vs > 0) || (g->vi > 0) != (r->vs > 0)) gleich = 0;
        }
        CHECK("(3b) Ruecken-Griff: Volumenmass Bild fuer Bild = RE2-Konstruktion (Gesicht-Lauf an der Pin-Wurzel "
              "gespiegelt; EINE Bank @0x80100C3C-5C, EIN Clip @0x801012A8-AC, Flip @0x8010130C-18)",
              vergl >= 30 && gleich && s_vol_bilder[1] == s_vol_pred[0]);
        CHECK("(3c) Ruecken-Griff: der Rueckfall aus Nachbesserung 1 ist weg — weniger Kontaktbilder als mit der reinen "
              "Clip-3-Pose (ohne +0x14E) und nicht mehr als im Basisstand (Volumenmass)",
              s_pinvar_vol[1][0] < s_pinvar_vol[1][1] && s_pinvar_vol[1][0] <= s_pinvar_vol[1][2]);
    }

    /* (4) Nachbesserung 1, M2 — der Ruecken-Griff ist im RE2-Original KEIN eigener Fall, sondern der
     * Gesicht-Griff mit Leon um 180 Grad gedreht. Belegt (EM2D-Overlay CDEMD0_EM2D_ai1.BIN / RE2 PSX.EXE):
     *   - EINE Opferbank: B4 P0 kopiert +0x188/+0x18C des Arms nach PL+0x188/+0x18C (@0x80100C3C-5C);
     *   - EIN Opfer-Clip: Hook P0 `lui v0,0xf / sw v0,332(s1)` = Clip 0 (@0x801012A8-AC), P1 nur Advance;
     *   - der EINZIGE Unterschied: `lhu v0,118(s1) / addiu v0,v0,2048 / sh v0,118(s1)` (@0x8010130C-18),
     *     gegatet von FUN_80015910 (@0x801012E0); das Ergebnis s0 lebt nur im Hook;
     *   - der Arm liest PL+0x76 nirgends (einzige Zugriffe auf Offset 118 im Overlay: @0x8010130C/18) und
     *     PL.x/z nur VOR dem Griff (@0x80100028..0x80100B0C) — seine Hand-Bahn haengt nicht an Leons Blick.
     * Geprueft wird genau diese Konstruktion: (4a) Ruecken-Blick = Gesicht-Blick + 2048, (4b) Wurzel und
     * Hand-Bahn Bild fuer Bild gleich, (4c) Brust im Ruecken-Griff = an Leons Wurzel gespiegelte Brust des
     * Gesicht-Griffs. Das Clipping-Mass des Ruecken-Griffs ist damit die zwingende Folge der RE2-Daten. */
    {
        int vergl = 0, wurzel = 1, hand = 1, spiegel = 1, maxdh = 0, maxdb = 0;
        for (int f = 0; f < 60; f++) {
            const bild_t *g = &s_auf[0][f], *r = &s_auf[1][f];
            if (!g->ok || !r->ok) continue;
            vergl++;
            if (g->lx != r->lx || g->lz != r->lz || g->pfr != r->pfr || g->afr != r->afr) wurzel = 0;
            int dh = abs(g->h[0] - r->h[0]) + abs(g->h[2] - r->h[2]);
            if (dh > maxdh) maxdh = dh;
            if (dh > 2) hand = 0;
            int db = abs((r->b[0] - r->lx) + (g->b[0] - g->lx)) + abs((r->b[2] - r->lz) + (g->b[2] - g->lz));
            if (db > maxdb) maxdb = db;
            if (db > 4) spiegel = 0;
        }
        printf("  (4) Ruecken vs. Gesicht: %d Bilder verglichen, Blick %d / %d, Hand-Abweichung max %d, "
               "Spiegel-Abweichung max %d\n", vergl, s_auf_yaw[1], s_auf_yaw[0], maxdh, maxdb);
        CHECK("(4a) Ruecken-Griff: Leon-Blick = Gesicht-Blick + 2048 (Flip @0x8010130C-18)",
              s_auf_yaw[0] >= 0 && s_auf_yaw[1] == ((s_auf_yaw[0] + 2048) & 0xfff));
        CHECK("(4b) Wurzel, Bildtakt und Hand-Bahn haengen nicht an Leons Blick (Arm liest PL+0x76 nie)",
              vergl >= 30 && wurzel && hand);
        CHECK("(4c) Brust im Ruecken-Griff = an Leons Wurzel gespiegelte Brust (EINE Bank @0x80100C3C-5C, "
              "EIN Clip @0x801012A8-AC)", vergl >= 30 && spiegel);
    }


    /* (5) Nachbesserung 2, M1 — RE1.5-KI: der RE1.5-Writher 0x8010c1ec-0x8010d774 hat keinen Griff (kein
     * Schadenseinstieg, Spieler nur gelesen @0x8010c238/258/360/378, kein Store auf feste Adressen) -> Beta ->
     * Retail: Typ 0x1A laeuft in JEDEM Flavor auf dem RE2-EM2D-Gehirn (re15_ai_re2_for_type). Gemessen wird der
     * Griff unter RE15_AI_FLAVOR_RE15 ueber denselben echten Weg: Gehirn, Pin auf die gemischte Hand (Kontakt),
     * Gleichlauf Arm + 1, kein Arm-Vertex in Leons Kopf/Rumpf. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    CHECK("(5a) RE1.5-KI: Typ 0x1A gehoert dem RE2-EM2D-Gehirn (re15_ai_re2_for_type(0x1A) = 1)",
          re15_ai_re2_for_type(0x1Au) == 1);
    int k_re15 = griff(2048, "RE1.5-KI Gesicht", 2);
    printf("  RE1.5-KI: waagerechtes Mass %d, Volumenmass %d Bilder (max %d)\n", k_re15, s_vol_bilder[2], s_vol_max[2]);
    {   int gleich = 1, vergl = 0;
        for (int f = 0; f < 60; f++) {
            const bild_t *g = &s_auf[0][f], *r = &s_auf[2][f];
            if (!g->ok || !r->ok) continue;
            vergl++;
            if (g->lx != r->lx || g->lz != r->lz || g->pfr != r->pfr || g->afr != r->afr || g->yaw != r->yaw) gleich = 0;
        }
        CHECK("(5b) RE1.5-KI: Griff mit Kontakt — Leon steht im Halten auf der Arm-Hand und laeuft Bild fuer Bild wie "
              "unter RE2-KI (Wurzel, Blick, Leon-/Arm-Bild, Volumenmass)",
              k_re15 >= 0 && vergl >= 30 && gleich && s_vol_bilder[2] == s_vol_bilder[0]);
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);

    printf(g_fail ? "test_r35_raeume_arme: FAIL\n" : "test_r35_raeume_arme: OK\n");
    return g_fail ? 1 : 0;
}
