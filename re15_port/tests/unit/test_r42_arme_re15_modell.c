/* test_r42_arme_re15_modell.c — RIEGEL (Runde 42, Strang "original", Stufe 2, 18.11.2026; Runde 5):
 * Die ROOM1210/1211-Gitterhaende zeigen das RE1.5-Modell EM1A wie das Original, das Greifen kommt vom RE2-Zellenarm-Gehirn.
 *
 * NUTZERWUNSCH 18.11.2026, woertlich: "Und unten in den Gefängnis in STAGE1 hast du die Arme aus Resident Evil 2
 * übernommen. Aber du solltest EIGENTLICH NUR die funktionalität aus Resident Evil 2 übernehmen, nicht das Modell. Das
 * Modell darf schon noch das aus Resident Evil 1.5 sein...." und 02.11. (Memory reai-v2-re15-modellaussehen-entscheidend):
 * "Haben wir im Port jetzt Stummel Aermchen oder irgendeinen anderen Quatsch gemacht ist das falsch."
 *
 * Verlangt (Belege im Modul engine/src/enemy_ai_re2_zellenarm.c, Bloecke MODELL, KOERPER und HALTE-LAGE):
 *   K0..K2  Vor dem Ausloeser nichts, danach Weg B[1] = Original-RAM (lauf1/proben.txt).
 *   W1..W3  Danach wiegt der Koerper wie das Original: B[2] (rueckwaerts/vorwaerts um +0x8C) und B[3] (Zeit/Zustand).
 *   S1..S3  Runde 5 (Vorgabe des Orchestrators Punkt 1): KEIN Teil wird gestreckt, gestaucht oder skaliert - Faktor genau 1
 *           fuer alle vier EM1A-Teile in JEDEM Bild (auch in B[3] und im Griff), unabhaengig an der gezeichneten Matrix
 *           gemessen (Spaltenlaengen). Eine Streckung ist rot.
 *   H0..H4  Gegriffen wird nur, wenn die RE1.5-Hand die Figur erreicht (Klemmradius 450 @0x80073e9a): mitten im Flur nie;
 *           an der Wand vor dem Fenster tritt der Koerper bis an seine Wandklemme (-22450) vor und greift. Im Halten steht die
 *           Figur FEST (wie RE2) im Flur, und die Hand liegt in jedem Bild an ihr: weder in der Luft noch tief im Koerper
 *           (hoechstens die halbe Handbreite 101 von der Oberflaeche, naechster Vertex <= Handdicke 202). Leon und Elza.
 *   R1..R4  RE2-Funktion bleibt: Halten 152 Bilder ohne Tasten, Cooldown 120, Rueckzug, hp -1, Toene, Treffer.
 *
 * GEGENPROBE: auf aaea5f8c2 fehlen die Koerper-/Hand-Funktionen (hier SCHWACH deklariert) -> rot. Auf db67b1388 (Runde 3)
 * streckt der Port den Arm zur RE2-Hand, auf a65162849 (Runde 4) zeichnet er die B[3]-Streckung des Originals und Leon
 * pendelt im Halten (Pruefbericht Runde 4 R2/R3) -> rot.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_math.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_ai_flavor.h"
#include "re15_roy_r37.h"
#include "re2_ems.h"
#include "re15_enemy_ai_re2_zellenarm.h"
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

/* schwach: fehlen auf aaea5f8c2 / db67b1388 (Gegenprobe) */
extern void re15_re2arm_modell_einrichten(re15_enemy_bank_t *anzeige, const re15_enemy_bank_t *em2d_logik) __attribute__((weak));
extern int  re15_re2arm_modell_re15(const re15_actor_t *e) __attribute__((weak));
extern int  re15_re2arm_zeichen_teil(const re15_actor_t *e, int teil, const re15_skel_pose_t *poses, int n,
                                     int32_t rot[9], int32_t trans[3], int32_t *streck) __attribute__((weak));
extern int  re15_re2arm_koerper_lage(const re15_actor_t *e, int32_t p[3]) __attribute__((weak));
extern int  re15_re2arm_koerper_info(int slot, int *bild, int *clip, int *frame, int32_t rec[3]) __attribute__((weak));
extern int  re15_re2arm_koerper_zustand(int slot, int *mode, int *phase, int sk[2], int an[2], int32_t *ky) __attribute__((weak));
extern int  re15_re2arm_hand15(int slot, int32_t hand[3], int32_t schulter[3]) __attribute__((weak));
extern int  re15_re2arm_kontakt_rest(int slot) __attribute__((weak));

#define WEST_FLAECHE (-21090)          /* SCA-Zelle [6] ROOM1210, Flurseite der Westwand */
#define WEST_ZELLE   (-22150)          /* SCA-Zelle [6] ROOM1210, Zellenseite der Westwand */
/* Original-RAM (DuckStation, MZD-Disc, ROOM1210, Leon): scratchpad original/s2r2/orig/lauf1/proben.txt, Satz s4 = West,
 * s8 = Ost: Bild 24 x -23000 / -15938 (Clip 2, Bild 25), Ruhe -22580 / -16358. */
#define ORIG_B24_WEST  (-23000)
#define ORIG_B24_OST   (-15938)
#define ORIG_RUHE_WEST (-22580)
#define ORIG_RUHE_OST  (-16358)
#define K_B2 1
#define K_B3 2
#define K_GRIFF 3
#define K_SINKEN 4

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static uint8_t *s_ems2 = NULL;  static size_t s_ems2_n = 0;
static uint8_t *s_em1a = NULL;  static size_t s_em1a_n = 0;
static re15_enemy_bank_t s_em2d;
static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static const uint8_t *toc_blob(const uint8_t *ems, size_t n, int archiv, size_t *len)
{
    uint32_t sek = 0, lg = 0;
    if (!ems || !re15_ems_toc(archiv, 0x1A, &sek, &lg)) return NULL;     /* Index type-0x10 @0x80022338 */
    if ((size_t)sek * 2048u + lg > n) return NULL;
    *len = lg; return ems + (size_t)sek * 2048u;
}
static int banken_laden(void)
{
    if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
    if (!s_ems2)  s_ems2  = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems15 || !s_ems2) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
    if (!eb) eb = re15_enemy_alloc(0x1A);
    if (!eb) return 0;
    re15_tim_t tim = {0};
    if (!s_em1a) {
        size_t lg = 0; const uint8_t *b = toc_blob(s_ems15, s_ems15_n, RE15_EMS_ARCHIV_LEON, &lg);
        if (!b) return 0;
        s_em1a = (uint8_t *)malloc(lg); memcpy(s_em1a, b, lg); s_em1a_n = lg;
    }
    if (re15_emd_parse_container(s_em1a, s_em1a_n, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return 0;
    eb->victim_ok = 0;
    eb->ok = 1; eb->buf = NULL;
    memset(&s_em2d, 0, sizeof s_em2d);
    re15_tim_t t2 = {0};
    if (re2_ems_load_bank(s_ems2, s_ems2_n, 0x2D, &s_em2d, &t2) != 0) return 0;
    if (re15_re2arm_modell_einrichten) re15_re2arm_modell_einrichten(eb, &s_em2d);
    return 1;
}
static int s_se_n = 0;
static void se_cap(int id, int f) { (void)id; (void)f; s_se_n++; }
static void bank_cap(int b) { (void)b; }
static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}
static int aufsetzen(int *slots, int modell)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_re2z_rng_reset();
    g_scd.work_vars[0x10] = (int16_t)modell;             /* 0 = Leon PL00, 4 = Elza PL04 (main.c character & 0x0F) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    if (!banken_laden()) return 0;
    re15_re2arm_audio_hook(se_cap, bank_cap);
    s_se_n = 0;
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) slots[n++] = s;
    return n;
}

/* ---- Leons Koerper (unabhaengig vom Modul: PL%02X.MD1 + .EMR aus shared_assets, Opfer-Pose der Bank) ---- */
extern void *g_anim_pose_actor;
static re15_emd_skeleton_t s_plsk; static re15_md1_t s_plmd; static uint8_t *s_plb[2]; static int s_pl_geladen = -1;
static int leon_laden(int modell)
{
    if (s_pl_geladen == modell) return 1;
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/PLD/PL%02X.EMR", (unsigned)modell);
    s_plb[0] = slurp(p, &n);
    if (!s_plb[0] || re15_emd_parse_skeleton(s_plb[0], n, &s_plsk) != 0) return 0;
    snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/PLD/PL%02X.MD1", (unsigned)modell);
    s_plb[1] = slurp(p, &n);
    if (!s_plb[1] || re15_md1_parse(s_plb[1], (int)n, &s_plmd) != 0) return 0;
    s_pl_geladen = modell;
    return 1;
}
/* kontakt / luecke (unabhaengig vom Modul nachgerechnet, eigene Leon-Ladung PL%02X.EMR/.MD1, Opfer-Pose der Bank):
 *   kontakt = kleinster Abstand der 12 Hand-Vertices (Quad-Vertices 0..11 des greifenden EM1A-Arms, gezeichnete Matrix) zu
 *             Leons Dreiecken (Quads als (v0,v1,v2)+(v1,v3,v2)) - 0 = beruehrt; > Handbreite 2*HAND_HB = in der Luft.
 *   innen   = die Handmitte (Mittel der Vertices 8..11) liegt in einem von Leons Koerperteilen: von drei Strahlen in die
 *             GEGENRICHTUNG des Moduls (-y, -x, -z) schneiden mindestens zwei die Dreiecke dieses Teils ungerade oft.
 *             Rueckgabe 0 = nicht berechenbar. */
#define HAND_HB 101                    /* halbe Breite der EM1A-Hand (Quad-Vertices 0..11 von Mesh 2/3, lokal x/z), Modul */
static double tri_abstand(const double p[3], const double a[3], const double b[3], const double c[3])
{   /* Punkt-Dreieck: Projektion auf die Ebene, sonst kleinster Abstand zu den drei Kanten (eigene Rechnung, nicht die des Moduls) */
    double u[3], v[3], w[3], n[3];
    for (int i = 0; i < 3; i++) { u[i] = b[i] - a[i]; v[i] = c[i] - a[i]; w[i] = p[i] - a[i]; }
    n[0] = u[1]*v[2] - u[2]*v[1]; n[1] = u[2]*v[0] - u[0]*v[2]; n[2] = u[0]*v[1] - u[1]*v[0];
    const double nn = n[0]*n[0] + n[1]*n[1] + n[2]*n[2];
    if (nn > 1e-9) {
        const double uu = u[0]*u[0]+u[1]*u[1]+u[2]*u[2], vv = v[0]*v[0]+v[1]*v[1]+v[2]*v[2], uv = u[0]*v[0]+u[1]*v[1]+u[2]*v[2];
        const double wu = w[0]*u[0]+w[1]*u[1]+w[2]*u[2], wv = w[0]*v[0]+w[1]*v[1]+w[2]*v[2];
        const double den = uv*uv - uu*vv;
        const double s = (uv*wv - vv*wu) / den, t = (uv*wu - uu*wv) / den;
        if (s >= 0 && t >= 0 && s + t <= 1) return fabs(w[0]*n[0] + w[1]*n[1] + w[2]*n[2]) / sqrt(nn);
    }
    const double *E[3][2] = { { a, b }, { b, c }, { c, a } };
    double best = 1e30;
    for (int k = 0; k < 3; k++) {
        const double *e0 = E[k][0], *e1 = E[k][1];
        double d[3], q[3], l2 = 0, t = 0;
        for (int i = 0; i < 3; i++) { d[i] = e1[i] - e0[i]; l2 += d[i]*d[i]; t += (p[i] - e0[i]) * d[i]; }
        t = l2 > 0 ? t / l2 : 0; if (t < 0) t = 0; if (t > 1) t = 1;
        double s2 = 0; for (int i = 0; i < 3; i++) { q[i] = e0[i] + t*d[i] - p[i]; s2 += q[i]*q[i]; }
        if (s2 < best) best = s2;
    }
    return sqrt(best);
}
static int hand_welt(const re15_actor_t *e, double hv[12][3])
{
    const re15_enemy_bank_t *b = re15_enemy_find(0x1A);
    const int teil = (e->re2z_f10e & 1u) ? 3 : 2;           /* greifender Arm (Modul em1a_arm) */
    int32_t R[9], X[3], st = 0;
    if (!b || !re15_re2arm_zeichen_teil || re15_re2arm_zeichen_teil(e, teil, NULL, 0, R, X, &st) != 1) return 0;
    const re15_md1_mesh_t *m = &b->md1.meshes[b->skel.bone_mesh_index[teil]];
    if (m->quad_vertex_count < 12) return 0;
    for (int v = 0; v < 12; v++) {
        const re15_md1_vertex_t *p = &m->quad_vertices[v];
        for (int r = 0; r < 3; r++)
            hv[v][r] = (double)(((int64_t)R[r*3] * p->x + (int64_t)R[r*3+1] * p->y + (int64_t)R[r*3+2] * p->z) / 4096) + X[r];
    }
    return 1;
}
static int leon_kontakt(const re15_actor_t *pl, const double hv[12][3], double *kontakt, int *innen)
{
    const re15_enemy_bank_t *vb = re15_enemy_find(0x1A);
    static re15_emd_skeleton_t vs; static re15_skel_pose_t Q[RE15_EMD_MAX_BONES];
    static double W[2048][3];
    vs = s_plsk;
    int kf = 0;
    if (vb && vb->victim_ok) {
        vs.keyframe_data = vb->skel_victim.keyframe_data; vs.keyframe_data_size = vb->skel_victim.keyframe_data_size;
        vs.keyframe_count = vb->skel_victim.keyframe_count; vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
        const int fc = vb->anim_victim.clips[pl->motion].frame_count;
        kf = (int)(re15_emd_get_frame_entry(&vb->anim_victim, pl->motion, pl->anim_frame % fc) & 0xfffu);
    }
    void *sv = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rc = re15_skel_compute_pose(&vs, kf, Q);
    g_anim_pose_actor = sv;
    if (rc != 0) return 0;
    double M[3] = { 0, 0, 0 };
    for (int v = 8; v < 12; v++) for (int r = 0; r < 3; r++) M[r] += hv[v][r] / 4.0;
    double best = 1e30; int in = 0;
    for (int b = 0; b < s_plsk.bone_count; b++) {
        int mi = s_plsk.bone_mesh_index[b];
        if (mi < 0 || mi >= s_plmd.mesh_count) continue;
        const re15_md1_mesh_t *m = &s_plmd.meshes[mi];
        int schnitt[3] = { 0, 0, 0 };                     /* Paritaet je Koerperteil (Dreiecke + Quads), je Strahl */
        for (int q = 0; q < 2; q++) {
            const re15_md1_vertex_t *vv = q ? m->quad_vertices : m->tri_vertices;
            int nv = q ? m->quad_vertex_count : m->tri_vertex_count;
            if (nv > 2048) nv = 2048;
            for (int i = 0; i < nv; i++) {
                int32_t l[3], w[3];
                for (int r = 0; r < 3; r++)
                    l[r] = (int32_t)(((int64_t)Q[b].rot[r*3] * vv[i].x + (int64_t)Q[b].rot[r*3+1] * vv[i].y +
                                      (int64_t)Q[b].rot[r*3+2] * vv[i].z) >> 12) + Q[b].trans[r];
                re15_skel_bone_to_world(l, pl->rot_y, pl->x, pl->y, pl->z, w);
                W[i][0] = w[0]; W[i][1] = w[1]; W[i][2] = w[2];
            }
            const int nf = q ? m->quad_count : m->triangle_count;
            for (int f = 0; f < nf; f++) {
                int ix[4], k = 3;
                if (q) { ix[0] = m->quads[f].v0; ix[1] = m->quads[f].v1; ix[2] = m->quads[f].v2; ix[3] = m->quads[f].v3; k = 4; }
                else   { ix[0] = m->triangles[f].v0; ix[1] = m->triangles[f].v1; ix[2] = m->triangles[f].v2; }
                if (ix[0] >= nv || ix[1] >= nv || ix[2] >= nv || (k == 4 && ix[3] >= nv)) continue;
                for (int t = 0; t < k - 2; t++) {
                    const double *A = W[t ? ix[1] : ix[0]], *B = W[t ? ix[3] : ix[1]], *C = W[ix[2]];
                    for (int h = 0; h < 12; h++) { double d = tri_abstand(hv[h], A, B, C); if (d < best) best = d; }
                    /* Strahlen -y / -x / -z: Handmitte in der Projektion (Ebene xz / yz / xy), Schnittpunkt in Strahlrichtung */
                    for (int k = 0; k < 3; k++) {
                        const int ax = (k == 0) ? 1 : (k == 1) ? 0 : 2, i0 = (ax == 0) ? 1 : 0, i1 = (ax == 2) ? 1 : 2;
                        const double d0 = (B[i1]-C[i1])*(A[i0]-C[i0]) + (C[i0]-B[i0])*(A[i1]-C[i1]);
                        if (fabs(d0) <= 1e-6) continue;
                        const double a0 = ((B[i1]-C[i1])*(M[i0]-C[i0]) + (C[i0]-B[i0])*(M[i1]-C[i1])) / d0;
                        const double a1 = ((C[i1]-A[i1])*(M[i0]-C[i0]) + (A[i0]-C[i0])*(M[i1]-C[i1])) / d0, a2 = 1.0 - a0 - a1;
                        if (a0 >= 0 && a1 >= 0 && a2 >= 0 && a0*A[ax] + a1*B[ax] + a2*C[ax] < M[ax]) schnitt[k] ^= 1;
                    }
                }
            }
        }
        if (schnitt[0] + schnitt[1] + schnitt[2] >= 2) in = 1;
    }
    *kontakt = best; *innen = in;
    return 1;
}

/* ---- Pruefung der gezeichneten Teile je Bild (S1/S2): Faktor 1, Spaltenlaengen unabhaengig ---- */
typedef struct {
    int bilder, fehl_s1, fehl_s2, b3_bilder, b2_bilder, griff_bilder, b3_lang;
    int32_t b2_x_min, b2_x_max; int ky_min, ky_max; int d_fehl, d_bilder;
    int hand_x_max;
    int last_sk[2][2], last_mode[2], last_phase[2], last_an[2][2];
} teile_t;
static teile_t T;
static int spalte(const int32_t R[9], int c)
{
    double s = (double)R[c] * R[c] + (double)R[3 + c] * R[3 + c] + (double)R[6 + c] * R[6 + c];
    return (int)(sqrt(s) + 0.5);
}
static void teile_pruefen(int slot, int ist_west)
{
    re15_actor_t *e = &g_actors[slot];
    if (!re15_re2arm_zeichen_teil) { T.fehl_s1++; return; }
    int mode = -1, phase = 0, sk[2] = {0, 0}, an[2] = {0, 0}; int32_t ky = 0;
    if (!re15_re2arm_koerper_zustand) {                   /* Gegenprobe db67b1388: ohne Zustand */
        T.fehl_s1++;
        if (re15_re2arm_koerper_lage && !re15_re2arm_koerper_lage(e, NULL)) return;
    } else if (!re15_re2arm_koerper_zustand(slot, &mode, &phase, sk, an, &ky)) return;
    T.bilder++;
    for (int t = 0; t < 4; t++) {
        int32_t R[9], X[3], st = -1;
        if (re15_re2arm_zeichen_teil(e, t, NULL, 0, R, X, &st) != 1) { T.fehl_s1++; continue; }
        if (st != 4096) T.fehl_s1++;                       /* S1: gemeldete Skalierung = 4096 (Faktor 1) */
        /* S2: gezeichnete Matrix = reine Rotation: alle drei Spalten Laenge 4096 (Q12-Rundung der Pose-Kette 4084..4096) */
        const int c0 = spalte(R, 0), c1 = spalte(R, 1), c2 = spalte(R, 2);
        if (abs(c0 - 4096) > 20 || abs(c1 - 4096) > 20 || abs(c2 - 4096) > 20) {
            if (T.fehl_s2 < 5) printf("    S2 Teil %d: Spalten %d %d %d (Modus %d)\n", t, c0, c1, c2, mode);
            T.fehl_s2++;
        }
    }
    if (mode == K_B3) { T.b3_bilder++; if (an[0] && sk[0] > 4096) T.b3_lang++; }
    if (mode == K_B2) T.b2_bilder++;
    const int L = ist_west ? 0 : 1;
    if (mode == K_GRIFF) T.griff_bilder++;
    /* W3: B[3] P1 fuehrt den Zustand +0x8A des Originals weiter (je Bild +0x76; Original-RAM lauf4: +6 je Bild), auch wenn er
     * nicht mehr gezeichnet wird */
    if (mode == K_B3 && T.last_mode[L] == K_B3 && phase == 1 && T.last_phase[L] == 1 && an[0] && T.last_an[L][0]) {
        T.d_bilder++;
        const int d0 = sk[0] - T.last_sk[L][0], d1 = sk[1] - T.last_sk[L][1];
        if (d0 < 5 || d0 > 12 || d1 < 0 || d1 > 7) T.d_fehl++;        /* (rng&7)+5 @0x8010cc38-40 / rng&7 @0x8010cc74-78 */
    }
    if (ky < T.ky_min) T.ky_min = ky;
    if (ky > T.ky_max) T.ky_max = ky;
    int32_t kp[3];
    if (ist_west && mode == K_B2 && re15_re2arm_koerper_lage(e, kp)) {
        if (kp[0] < T.b2_x_min) T.b2_x_min = kp[0];
        if (kp[0] > T.b2_x_max) T.b2_x_max = kp[0];
    }
    int32_t h[3];
    if (ist_west && re15_re2arm_hand15 && re15_re2arm_hand15(slot, h, NULL) && h[0] > T.hand_x_max) T.hand_x_max = h[0];
    T.last_mode[L] = mode; T.last_phase[L] = phase; T.last_sk[L][0] = sk[0]; T.last_sk[L][1] = sk[1]; T.last_an[L][0] = an[0]; T.last_an[L][1] = an[1];
}

/* ---- Griff-Lauf: Leon steht an der Westwand vor dem Fenster von Satz 5 und wartet ---- */
/* Szenario (keine Verhaltens-Konstante): Leon steht an der Westwand (seine Flur-Klemme x -20622 = Wandflaeche -21090 + r 450
 * @0x80073e9a, gerundet; Memory reai-v2-runde10-arme-1090-stimme) auf der z-Hoehe der Hand und wartet. */
#define FLUR_KLEMME_WEST (-20622)
typedef struct {
    int griff_bild, loesen_bild, rz_bilder, hp_nach, cd_nach, se_vor, se_nach, halten_n, leon_x_min, leon_x_max, leon_z_min,
        leon_z_max, sprung_max, kx_griff, ohne_band, luecke_n, klemme_fest;
    double gate_abstand, naechster_max, luecke_min, luecke_max, kontakt_sum; int griff_ok; int zugriff_clip; int sinken_clip;
    int kx_nach;
} griff_t;
static int griff_lauf(int modell, griff_t *g, int *slot_out)
{
    memset(g, 0, sizeof *g);
    g->griff_bild = g->loesen_bild = g->rz_bilder = -1; g->leon_x_min = g->leon_z_min = 999999;
    g->leon_x_max = g->leon_z_max = -999999; g->zugriff_clip = g->sinken_clip = -1;
    g->luecke_min = 1e30; g->luecke_max = -1e30; g->kx_griff = -999999;
    int slots[RE15_ACTOR_MAX];
    int n = aufsetzen(slots, modell);
    if (n < 10) return 0;
    const int s5 = slots[4]; *slot_out = s5;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *arm = &g_actors[s5];
    pl->x = -19393; pl->z = 6000;                          /* weit weg (> 3000 vom Anker): das RE2-Gehirn bleibt in B0 */
    for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;   /* Weckruf Member_set(12,1) @0x1EDA */
    for (int f = 0; f < 100; f++) frame_step();            /* Weg B[1] (39 Bilder) ist vorbei */
    if (!re15_re2arm_hand15 || !re15_re2arm_koerper_zustand) return 0;
    int32_t h[3];
    if (!re15_re2arm_hand15(s5, h, NULL)) return 0;
    pl->x = FLUR_KLEMME_WEST; pl->z = h[2]; pl->rot_y = 2048;
    g->se_vor = s_se_n;
    for (int f = 0; f < 3000 && g->griff_bild < 0; f++) {
        const int32_t vx = pl->x, vz = pl->z;              /* Lage, die das Tor A3 in diesem Bild sieht (vor dem Pin) */
        frame_step();
        int mode = 0; int32_t kp[3];
        re15_re2arm_koerper_zustand(s5, &mode, NULL, NULL, NULL, NULL);
        if (mode == K_GRIFF && g->zugriff_clip < 0) { int c = -1; re15_re2arm_koerper_info(s5, NULL, &c, NULL, NULL); g->zugriff_clip = c; }
        if (arm->sub_state_1 == 4) {
            g->griff_bild = f;
            int32_t hh[3]; re15_re2arm_hand15(s5, hh, NULL);
            double dx = hh[0] - vx, dz = hh[2] - vz; g->gate_abstand = sqrt(dx*dx + dz*dz);
            if (re15_re2arm_koerper_lage(arm, kp)) {
                g->kx_griff = kp[0];
                /* Wandklemme des Koerpers (r 300 @0x80120922, Maske 4 @0x8010c318): ein weiterer Schritt 200 nach +x bleibt stehen */
                int32_t nx = kp[0] + 200, nz = kp[2];
                re15_collision_constrain_enemy(&g_room_rdt, kp[0], kp[2], &nx, &nz, 300, kp[1], 4u);
                g->klemme_fest = (nx == kp[0] && nz == kp[2]);
            }
        }
    }
    if (g->griff_bild < 0) return 1;
    leon_laden(modell);
    int32_t lx = 0, lz = 0, erst = 1;
    for (int f = 0; f < 300 && g->loesen_bild < 0; f++) {
        frame_step();
        if (re15_player_is_grabbed() && re15_player_victim_state() == 4 && arm->sub_state_1 == 4) {
            int32_t hh[3], ss[3]; re15_re2arm_hand15(s5, hh, ss);
            double hv[12][3], ko = 1e30; int in = 0;
            if (hand_welt(arm, hv) && leon_kontakt(pl, hv, &ko, &in)) {
                g->luecke_n++;
                if (ko > g->naechster_max) g->naechster_max = ko;
                if (ko < g->luecke_min) g->luecke_min = ko;
                g->kontakt_sum += ko;
                g->ohne_band += in;
                if (getenv("R42_KONTAKT_LISTE") && (g->halten_n < 40 || in)) { int km = -1; re15_re2arm_koerper_zustand(s5, &km, NULL, NULL, NULL, NULL);
                    printf("    Halten Bild %d (Opfer-Clip %d Bild %d, Koerper-Modus %d): kontakt %.0f innen %d\n", g->halten_n,
                           (int)pl->motion, (int)pl->anim_frame, km, ko, in); }
            }
            if (pl->x < g->leon_x_min) g->leon_x_min = pl->x;
            if (pl->x > g->leon_x_max) g->leon_x_max = pl->x;
            if (pl->z < g->leon_z_min) g->leon_z_min = pl->z;
            if (pl->z > g->leon_z_max) g->leon_z_max = pl->z;
            if (!erst) { int sp = abs(pl->x - lx) + abs(pl->z - lz); if (sp > g->sprung_max) g->sprung_max = sp; }
            lx = pl->x; lz = pl->z; erst = 0;
            g->halten_n++;
        }
        if (arm->sub_state_1 == 5) g->loesen_bild = f + 1;
    }
    g->cd_nach = (int)g_re2_room_gflags;
    for (int f = 0; f < 80; f++) {
        frame_step();
        int mode = 0, c = -1; re15_re2arm_koerper_zustand(s5, &mode, NULL, NULL, NULL, NULL);
        if (mode == K_SINKEN && g->sinken_clip < 0) { re15_re2arm_koerper_info(s5, NULL, &c, NULL, NULL); g->sinken_clip = c; }
        if (g->rz_bilder < 0 && arm->sub_state_1 == 7) g->rz_bilder = f + 1;
    }
    {   int32_t kp[3]; g->kx_nach = re15_re2arm_koerper_lage(arm, kp) ? kp[0] : 0; }
    g->hp_nach = arm->hp; g->se_nach = s_se_n;
    g->griff_ok = 1;
    return 1;
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &sz);
    if (!buf) { printf("FAIL: ROOM1210.RDT fehlt\n"); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    printf("=== ROOM1210 Gitterhaende: RE1.5-Modell wie das Original (Faktor 1), Greifen vom RE2-Gehirn (Runde 42 Stufe 2, Runde 5) ===\n");
    g_current_room_id = 0x1210;

    /* ---- Lauf 1: Ausloeser, Weg B[1], dann 2400 Bilder Wiegen, Leon weit weg ---- */
    int slots[RE15_ACTOR_MAX];
    const int n = aufsetzen(slots, 0);
    CHECK("(M0) zehn Arme gespawnt, Zeichen-Bank EM1A (4 Knochen, Clips 30/39/30/54)", n == 10 &&
          re15_enemy_find(0x1A) && re15_enemy_find(0x1A)->skel.bone_count == 4 && re15_enemy_find(0x1A)->anim.clip_count == 4);
    if (n < 10) return 1;
    const int s5 = slots[4], s9 = slots[8];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *arm = &g_actors[s5];
    CHECK("(M1) das Gehirn erkennt das RE1.5-Modell", re15_re2arm_modell_re15 && re15_re2arm_modell_re15(arm) == 1);
    CHECK("(M2) alle vier EM1A-Teile werden gezeichnet (Maske 0)", re15_re2arm_part_hide_mask(arm) == 0);
    pl->x = -19393; pl->z = 6000;
    int vor = 0;
    for (int f = 0; f < 60; f++) { frame_step(); if (!re15_re2arm_koerper_lage || re15_re2arm_koerper_lage(arm, NULL)) vor++; }
    CHECK("(K0) vor dem Ausloeser wird kein Koerper gezeichnet (Original: Fenster leer, lauf3 t 13,0 s)", re15_re2arm_koerper_lage && vor == 0);
    for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;
    int b24 = 0, ruhe = 0; int32_t w24 = 0, o24 = 0, wr = 0, orr = 0; int c24 = -1, f24 = -1;
    memset(&T, 0, sizeof T); T.b2_x_min = 999999; T.b2_x_max = -999999; T.ky_min = 999999; T.ky_max = -999999; T.hand_x_max = -999999;
    for (int f = 0; f < 2400; f++) {
        frame_step();
        int bw = -1, bo = -1, cl = -1, fr = -1; int32_t pw[3] = {0,0,0}, po[3] = {0,0,0};
        if (re15_re2arm_koerper_info) { re15_re2arm_koerper_info(s5, &bw, &cl, &fr, NULL); re15_re2arm_koerper_info(s9, &bo, NULL, NULL, NULL); }
        if (re15_re2arm_koerper_lage) { re15_re2arm_koerper_lage(arm, pw); re15_re2arm_koerper_lage(&g_actors[s9], po); }
        if (bw == 25 && bo == 25 && !b24) { b24 = 1; w24 = pw[0]; o24 = po[0]; c24 = cl; f24 = fr; }
        if (bw == 39 && bo == 39 && !ruhe) { ruhe = 1; wr = pw[0]; orr = po[0]; }
        teile_pruefen(s5, 1);
        teile_pruefen(s9, 0);
    }
    const int hand_ruhe_max = T.hand_x_max;
    /* dazu 300 Bilder mit Leon an der Westwand vor Satz 5: das RE2-Gehirn faehrt aus/greift zu - auch dann wird nichts
     * gestreckt (Gegenprobe db67b1388: dort streckt der Port den Arm bis zur RE2-Hand) */
    {   int32_t hx = 0, hz = 0; re15_re2arm_home(s5, NULL, &hx, &hz);
        pl->x = -20600; pl->z = hz;
        for (int f = 0; f < 300; f++) { frame_step(); teile_pruefen(s5, 1); teile_pruefen(s9, 0); } }
    printf("  Weg B[1] Bild 24: West %d (Original %d), Ost %d (Original %d), Clip %d Bild %d; Ende B[1]: West %d / Ost %d\n",
           (int)w24, ORIG_B24_WEST, (int)o24, ORIG_B24_OST, c24, f24, (int)wr, (int)orr);
    CHECK("(K1) Weg B[1] Bild 24 = Original-RAM: West -23000, Ost -15938, Clip 2, Bild 25",
          b24 && w24 == ORIG_B24_WEST && o24 == ORIG_B24_OST && c24 == 2 && f24 == 25);
    CHECK("(K2) Ende B[1] = Original-RAM: West -22580, Ost -16358 (Wandklemme r 300)", ruhe && wr == ORIG_RUHE_WEST && orr == ORIG_RUHE_OST);
    printf("  Teile: %d Bilder, S1-Fehler %d, S2-Fehler %d, B[2] %d Bilder (West x %d..%d), B[3] %d Bilder (davon %d mit "
           "Original-Zustand +0x8A > 4096), Zuwachs-Bilder %d (Fehler %d), y %d..%d, RE1.5-Hand West x max %d (ohne Griff %d)\n",
           T.bilder, T.fehl_s1, T.fehl_s2, T.b2_bilder, (int)T.b2_x_min, (int)T.b2_x_max, T.b3_bilder, T.b3_lang,
           T.d_bilder, T.d_fehl, T.ky_min, T.ky_max, T.hand_x_max, hand_ruhe_max);
    CHECK("(W1) nach B[1] wiegt der Koerper (B[2]) hinter der Wand: West x zwischen -22580-5*177 und der Wandklemme -22450",
          T.b2_bilder > 100 && T.b2_x_max <= -22450 && T.b2_x_min >= ORIG_RUHE_WEST - 5 * 177);
    CHECK("(W2) und laeuft durch B[3]; y zittert +-5 um das gemerkte y (@0x8010cc88-b4, wandert je B[3] um +-5)",
          T.b3_bilder > 100 && T.ky_min < 0 && T.ky_max > 0 && T.ky_max - T.ky_min <= 5 * 2 * 8);
    CHECK("(W3) in B[3] P1 fuehrt Arm 2 den Original-Zustand +0x8A je Bild um (rng&7)+5 weiter, Arm 3 um rng&7",
          T.d_bilder > 100 && T.d_fehl == 0);
    CHECK("(S1) Faktor 1: jedes der vier EM1A-Teile meldet in jedem Bild Skalierung 4096 - auch in B[3] (dort waere das Original "
          "laenger) und mit Leon an der Wand", T.bilder > 1000 && T.b3_lang > 100 && T.fehl_s1 == 0);
    CHECK("(S2) Faktor 1 unabhaengig an der gezeichneten Matrix: alle drei Spalten jedes Teils = 4096 (+-20 Q12-Rundung)",
          T.bilder > 1000 && T.fehl_s2 == 0);
    CHECK("(S4) ohne Griff bleibt die RE1.5-Hand (Faktor 1) hinter der Wandflaeche -21090 (Original p029 Clip 1: -21229)",
          hand_ruhe_max > -999999 && hand_ruhe_max <= WEST_FLAECHE);

    /* ---- Lauf 1b: Leon mitten im Flur vor dem Fenster - die RE1.5-Hand erreicht ihn nie: kein Griff ---- */
    {   g_current_room_id = 0x1211; (void)re15_re2arm_tick(s5); g_current_room_id = 0x1210;   /* Modul-Reset */
        int sl[RE15_ACTOR_MAX];
        const int n0 = aufsetzen(sl, 0);
        const int a5 = sl[4];
        pl->x = -19393; pl->z = 6000;
        for (int i = 0; i < n0; i++) g_actors[sl[i]].grid_id = 1;
        for (int f = 0; f < 100; f++) frame_step();
        int32_t h[3] = {0, 0, 0}; if (re15_re2arm_hand15) re15_re2arm_hand15(a5, h, NULL);
        pl->x = -20200; pl->z = h[2];
        int griff = 0, zugriff = 0;
        for (int f = 0; f < 1500; f++) {
            frame_step();
            if (g_actors[a5].sub_state_1 == 4) griff++;
            if (g_actors[a5].sub_state_1 == 3) zugriff++;
        }
        printf("  Flurmitte x -20200: Zugriff-Bilder %d, Griff-Bilder %d\n", zugriff, griff);
        CHECK("(H0) Leon 420 vor der Flur-Klemme: die RE1.5-Hand erreicht ihn nicht, kein Griff in 1500 Bildern",
              re15_re2arm_hand15 && griff == 0);
    }

    /* ---- Lauf 2/3: Griff mit Leon und Elza ---- */
    for (int m = 0; m < 2; m++) {
        const int modell = m ? 4 : 0;
        const char *wer = m ? "Elza" : "Leon";
        g_current_room_id = 0x1211; (void)re15_re2arm_tick(s5); g_current_room_id = 0x1210;   /* Modul-Reset */
        griff_t g; int sg = 0;
        memset(&T, 0, sizeof T); T.ky_min = 999999; T.ky_max = -999999; T.b2_x_min = 999999; T.b2_x_max = -999999;
        const int rc = griff_lauf(modell, &g, &sg);
        printf("  %s: Griff Bild %d (Hand %.0f von der Figur, Koerper x %d), Zugriff-Clip %d, Halten %d Bilder bis Bild %d, "
               "Kontakt max %.0f (min %.0f, Mittel %.0f, %d Bilder, Handmitte innen %d), Figur x %d..%d z %d..%d (Sprung max %d), "
               "Rueckzug %d Bilder, Sinken-Clip %d, Koerper danach x %d, hp %d, cd %d, Toene %d\n",
               wer, g.griff_bild, g.gate_abstand, g.kx_griff, g.zugriff_clip, g.halten_n, g.loesen_bild,
               g.naechster_max, g.luecke_min, g.luecke_n ? g.kontakt_sum / g.luecke_n : 0.0, g.luecke_n, g.ohne_band, g.leon_x_min, g.leon_x_max,
               g.leon_z_min, g.leon_z_max, g.sprung_max, g.rz_bilder, g.sinken_clip, g.kx_nach, g.hp_nach, g.cd_nach,
               g.se_nach - g.se_vor);
        char t1[220];
        snprintf(t1, sizeof t1, "(H1 %s) an der Wand greift die RE1.5-Hand (Hand < 450 von der Figur, @0x80073e9a); Zugriff zeichnet "
                 "EM1A-Clip 2, der Koerper ist vor -22580 bis an seine Wandklemme (r 300, ein weiterer Schritt 200 bleibt stehen) getreten", wer);
        CHECK(t1, rc == 1 && g.griff_ok && g.griff_bild >= 0 && g.gate_abstand < 450.0 && g.zugriff_clip == 2 &&
                  g.kx_griff > ORIG_RUHE_WEST && g.klemme_fest);
        snprintf(t1, sizeof t1, "(H2 %s) im Halten liegt die Hand in JEDEM Bild an der Figur: Abstand der 12 Hand-Vertices zu ihrer "
                 "Oberflaeche <= Handbreite 202, im Mittel <= 50 (keine Hand in der Luft)", wer);
        CHECK(t1, g.halten_n > 100 && g.luecke_n == g.halten_n && g.naechster_max <= 2 * HAND_HB && g.kontakt_sum / g.luecke_n <= 50.0);
        snprintf(t1, sizeof t1, "(H4 %s) und steckt nicht tief in ihr: die Handmitte liegt in keinem Bild in einem ihrer Koerperteile", wer);
        CHECK(t1, g.halten_n > 100 && g.luecke_n == g.halten_n && g.ohne_band == 0);
        snprintf(t1, sizeof t1, "(H3 %s) die Figur steht das ganze Halten FEST (wie RE2: Pin nur @0x80100C18-38) und im Flur "
                 "(Klemmzylinder 450 vor der Westwand -21090)", wer);
        CHECK(t1, g.halten_n > 100 && g.leon_x_min == g.leon_x_max && g.leon_z_min == g.leon_z_max && g.sprung_max == 0 &&
                  g.leon_x_min - 450 >= WEST_FLAECHE);
        snprintf(t1, sizeof t1, "(R1 %s) RE2: Halten 152 Bilder ohne Tasten (@0x80100D28-40), dann Cooldown 120 (@0x80100D5C)", wer);
        CHECK(t1, g.loesen_bild == 152 && g.cd_nach == 119);
        snprintf(t1, sizeof t1, "(R2 %s) RE2-Rueckzug 10 Bilder -> 0x701, hp -1 (@0x80100E38-50); der Koerper sinkt mit Clip 3 und "
                 "geht hinter die Wandklemme zurueck", wer);
        CHECK(t1, g.rz_bilder == 10 && g.hp_nach == -1 && g.sinken_clip == 3 && g.kx_nach < -22450);
        snprintf(t1, sizeof t1, "(R3 %s) RE2-Toene laufen (Ausfahren/Halten/Rueckzug)", wer);
        CHECK(t1, g.se_nach - g.se_vor >= 3);
    }

    /* ---- Lauf 4: Griff-Bilder einzeln pruefen (S1/S2 im Griff), Leon ---- */
    if (!re15_re2arm_koerper_zustand || !re15_re2arm_hand15) CHECK("(S3) Koerper-Zustand/RE1.5-Hand abfragbar", 0);
    else {   g_current_room_id = 0x1211; (void)re15_re2arm_tick(s5); g_current_room_id = 0x1210;
        int sl[RE15_ACTOR_MAX];
        const int n4 = aufsetzen(sl, 0);
        const int a5 = sl[4];
        memset(&T, 0, sizeof T); T.ky_min = 999999; T.ky_max = -999999; T.b2_x_min = 999999; T.b2_x_max = -999999; T.hand_x_max = -999999;
        pl->x = -19393; pl->z = 6000;
        for (int i = 0; i < n4; i++) g_actors[sl[i]].grid_id = 1;
        for (int f = 0; f < 100; f++) frame_step();
        int32_t h[3]; re15_re2arm_hand15(a5, h, NULL);
        pl->x = FLUR_KLEMME_WEST; pl->z = h[2]; pl->rot_y = 2048;
        for (int f = 0; f < 1500; f++) { frame_step(); teile_pruefen(a5, 1); }
        printf("  Griff-Lauf Teile: %d Bilder, Griff %d Bilder, S1 %d, S2 %d\n", T.bilder, T.griff_bilder, T.fehl_s1, T.fehl_s2);
        CHECK("(S3) auch im Griff (Zugriff/Halten) wird kein Teil gestreckt (S1/S2 je Bild)",
              T.griff_bilder > 150 && T.fehl_s1 == 0 && T.fehl_s2 == 0);
    }

    /* ---- Lauf 5: Treffer -> Rueckzug, der Koerper sinkt (Clip 3) ---- */
    if (!re15_re2arm_koerper_zustand || !re15_re2arm_koerper_info) CHECK("(R4) Koerper-Zustand abfragbar", 0);
    else {   g_current_room_id = 0x1211; (void)re15_re2arm_tick(s5); g_current_room_id = 0x1210;
        int sl[RE15_ACTOR_MAX];
        const int n5 = aufsetzen(sl, 0);
        re15_actor_t *a9 = &g_actors[sl[8]];
        for (int i = 0; i < n5; i++) g_actors[sl[i]].grid_id = 1;
        pl->x = -19393; pl->z = 6000;
        for (int f = 0; f < 60; f++) frame_step();
        int32_t hx, hz; re15_re2arm_home(sl[8], NULL, &hx, &hz);
        pl->x = hx - 2000; pl->z = hz;
        for (int f = 0; f < 40; f++) frame_step();
        a9->state = 2;
        frame_step(); frame_step(); frame_step();
        int mode = -1, c = -1; re15_re2arm_koerper_zustand(sl[8], &mode, NULL, NULL, NULL, NULL);
        re15_re2arm_koerper_info(sl[8], NULL, &c, NULL, NULL);
        printf("  Treffer: Gehirn %d/%d, Koerper-Modus %d Clip %d\n", a9->state, a9->sub_state_1, mode, c);
        CHECK("(R4) Treffer -> RE2-Rueckzug (0x501), der RE1.5-Koerper sinkt mit Clip 3", a9->sub_state_1 == 5 && mode == K_SINKEN && c == 3);
    }

    printf(g_fail ? "=== FEHLGESCHLAGEN ===\n" : "=== OK ===\n");
    return g_fail;
}
