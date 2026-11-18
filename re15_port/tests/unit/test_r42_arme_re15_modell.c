/* test_r42_arme_re15_modell.c — RIEGEL (Runde 42, Strang "original", Stufe 2, 18.11.2026; Runde 4):
 * Die ROOM1210/1211-Gitterhaende zeigen das RE1.5-Modell EM1A wie das Original, das Greifen kommt vom RE2-Zellenarm-Gehirn.
 *
 * NUTZERWUNSCH 18.11.2026, woertlich: "Und unten in den Gefängnis in STAGE1 hast du die Arme aus Resident Evil 2
 * übernommen. Aber du solltest EIGENTLICH NUR die funktionalität aus Resident Evil 2 übernehmen, nicht das Modell. Das
 * Modell darf schon noch das aus Resident Evil 1.5 sein...." und 02.11. (Memory reai-v2-re15-modellaussehen-entscheidend):
 * "Haben wir im Port jetzt Stummel Aermchen oder irgendeinen anderen Quatsch gemacht ist das falsch."
 *
 * Verlangt (Belege im Modul engine/src/enemy_ai_re2_zellenarm.c, Bloecke MODELL und KOERPER):
 *   K0..K2  Vor dem Ausloeser nichts, danach Weg B[1] = Original-RAM (lauf1/proben.txt).
 *   W1..W3  Danach wiegt der Koerper wie das Original: B[2] (rueckwaerts/vorwaerts um +0x8C) und B[3] (Arme wachsen).
 *   S1..S3  KEIN Teil wird vom Port gestreckt, gestaucht oder gedreht: Rumpf/Kopf immer 4096; die Arme nur mit der Skalierung
 *           des Originals (B[3], Zuwachs +0x76 je Bild, wie in der Original-RAM lauf4: +6 je Bild) — unabhaengig an der
 *           gezeichneten Matrix gemessen (Spaltenlaengen). Im Griff steht die Skalierung.
 *   H1..H4  Gegriffen wird nur, wenn die RE1.5-Hand Leon erreicht (Klemmradius 450 @0x80073e9a) und Leon im Flur so stehen
 *           kann, dass die Hand an ihm liegt; im Halten liegt die Hand an Leon (naechster Leon-Vertex <= halbe Handbreite),
 *           Leon bleibt im Flur. Leon und Elza.
 *   R1..R4  RE2-Funktion bleibt: Halten 152 Bilder ohne Tasten, Cooldown 120, Rueckzug, hp -1, Toene, Treffer.
 *
 * GEGENPROBE: auf aaea5f8c2 fehlen die Koerper-/Hand-Funktionen (hier SCHWACH deklariert) -> rot. Auf db67b1388 (Runde 3)
 * fehlen koerper_zustand/hand15/kontakt_rest und der RE2-Arm wird gestreckt (S1) -> rot.
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
/* naechster Leon-Vertex zum Punkt P (3D), in der aktuellen Opfer-Pose */
static double leon_naechster(const re15_actor_t *pl, const int32_t P[3])
{
    const re15_enemy_bank_t *vb = re15_enemy_find(0x1A);
    static re15_emd_skeleton_t vs; static re15_skel_pose_t Q[RE15_EMD_MAX_BONES];
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
    if (rc != 0) return 1e9;
    double best = 1e9;
    for (int b = 0; b < s_plsk.bone_count; b++) {
        int mi = s_plsk.bone_mesh_index[b];
        if (mi < 0 || mi >= s_plmd.mesh_count) continue;
        const re15_md1_mesh_t *m = &s_plmd.meshes[mi];
        for (int q = 0; q < 2; q++) {
            const re15_md1_vertex_t *vv = q ? m->quad_vertices : m->tri_vertices;
            int nv = q ? m->quad_vertex_count : m->tri_vertex_count;
            for (int i = 0; i < nv; i++) {
                int32_t l[3], w[3];
                for (int r = 0; r < 3; r++)
                    l[r] = (int32_t)(((int64_t)Q[b].rot[r*3] * vv[i].x + (int64_t)Q[b].rot[r*3+1] * vv[i].y +
                                      (int64_t)Q[b].rot[r*3+2] * vv[i].z) >> 12) + Q[b].trans[r];
                re15_skel_bone_to_world(l, pl->rot_y, pl->x, pl->y, pl->z, w);
                double dx = w[0] - P[0], dy = w[1] - P[1], dz = w[2] - P[2], d = sqrt(dx*dx + dy*dy + dz*dz);
                if (d < best) best = d;
            }
        }
    }
    return best;
}

/* ---- Pruefung der gezeichneten Teile je Bild (S1/S2): Skalierung = Original-Zustand, Spaltenlaengen unabhaengig ---- */
typedef struct {
    int bilder, fehl_s1, fehl_s2, sk_max, b3_bilder, b2_bilder, griff_bilder, sk_sprung, griff_sk_aend;
    int b2_vor, b2_rueck; int32_t b2_x_min, b2_x_max; int ky_min, ky_max; int d_fehl, d_bilder;
    int hand_x_max; int hand_x_max_sk;
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
    if (!re15_re2arm_koerper_zustand) {                   /* Gegenprobe db67b1388: ohne Zustand gilt Soll 4096 (kein B[3]) */
        T.fehl_s1++;
        if (re15_re2arm_koerper_lage && !re15_re2arm_koerper_lage(e, NULL)) return;
    } else if (!re15_re2arm_koerper_zustand(slot, &mode, &phase, sk, an, &ky)) return;
    T.bilder++;
    for (int t = 0; t < 4; t++) {
        int32_t R[9], X[3], st = -1;
        if (re15_re2arm_zeichen_teil(e, t, NULL, 0, R, X, &st) != 1) { T.fehl_s1++; continue; }
        const int soll = (t >= 2 && an[t - 2]) ? sk[t - 2] : 4096;
        if (st != soll) T.fehl_s1++;
        /* S2: gezeichnete Matrix = Rotation * diag(1, soll/4096, 1): Spalten 0/2 Laenge 4096 (Q12-Rundung der Pose: die
         * Rotationen der Kette messen 4084..4096), Spalte 1 im Verhaeltnis zu ihnen = soll/4096 (+-0,2 %) */
        const double c0 = spalte(R, 0), c2 = spalte(R, 2), c1 = spalte(R, 1), cm = (c0 + c2) / 2.0;
        if (fabs(c0 - 4096.0) > 20.0 || fabs(c2 - 4096.0) > 20.0 || fabs(c1 / cm - soll / 4096.0) > 0.002) {
            if (T.fehl_s2 < 5) printf("    S2 Teil %d: Spalten %d %d %d soll %d (Modus %d)\n", t, spalte(R, 0), spalte(R, 1),
                                      spalte(R, 2), soll, mode);
            T.fehl_s2++;
        }
        if (soll > T.sk_max) T.sk_max = soll;
    }
    if (mode == K_B3) T.b3_bilder++;
    if (mode == K_B2) T.b2_bilder++;
    const int L = ist_west ? 0 : 1;
    if (mode == K_GRIFF) { T.griff_bilder++; if (T.last_mode[L] == K_GRIFF && (sk[0] != T.last_sk[L][0] || sk[1] != T.last_sk[L][1])) T.griff_sk_aend++; }
    /* W3: B[3] P1 waechst je Bild um genau +0x76 (Original-RAM lauf4: +6 je Bild bei +0x76 = 6) */
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
    if (ist_west && re15_re2arm_hand15 && re15_re2arm_hand15(slot, h, NULL) && h[0] > T.hand_x_max) {
        T.hand_x_max = h[0]; T.hand_x_max_sk = an[(e->re2z_f10e & 1u) ? 1 : 0] ? sk[(e->re2z_f10e & 1u) ? 1 : 0] : 4096;
    }
    T.last_mode[L] = mode; T.last_phase[L] = phase; T.last_sk[L][0] = sk[0]; T.last_sk[L][1] = sk[1]; T.last_an[L][0] = an[0]; T.last_an[L][1] = an[1];
}

/* ---- Griff-Lauf: warten, bis der Arm von Satz 5 im Original-B[3] lang genug ist, dann Leon an die Westwand davor ---- */
/* Szenario (keine Verhaltens-Konstante): gewartet wird, bis die RE1.5-Hand von Satz 5 im Original-B[3] so weit in den Flur
 * reicht (Wandflaeche -21090); per Umgebung R42_HAND_SOLL veraenderbar fuer Messungen. */
static int32_t g_hand_soll = -20450;
typedef struct {
    int griff_bild, loesen_bild, rz_bilder, hp_nach, cd_nach, se_vor, se_nach, halten_n, kontakt_max, leon_x_min, leon_x_max;
    double hand_leon_max, gate_abstand; int sk_griff; int griff_ok; int zugriff_clip; int sinken_clip;
} griff_t;
static int griff_lauf(int modell, griff_t *g, int *slot_out)
{
    memset(g, 0, sizeof *g);
    g->griff_bild = g->loesen_bild = g->rz_bilder = -1; g->leon_x_min = 999999; g->leon_x_max = -999999; g->zugriff_clip = g->sinken_clip = -1;
    int slots[RE15_ACTOR_MAX];
    int n = aufsetzen(slots, modell);
    if (n < 10) return 0;
    const int s5 = slots[4]; *slot_out = s5;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *arm = &g_actors[s5];
    pl->x = -19393; pl->z = 6000;                          /* weit weg (> 3000 vom Anker): das RE2-Gehirn bleibt in B0 */
    for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;   /* Weckruf Member_set(12,1) @0x1EDA */
    int ok = 0;
    for (int f = 0; f < 6000 && !ok; f++) {
        frame_step();
        int mode = 0, phase = 0, sk[2], an[2];
        if (!re15_re2arm_koerper_zustand || !re15_re2arm_koerper_zustand(s5, &mode, &phase, sk, an, NULL)) return 0;
        int32_t hh[3];
        if (mode == K_B3 && phase == 1 && re15_re2arm_hand15(s5, hh, NULL) && hh[0] >= g_hand_soll) ok = 1;   /* Szenario: Arm lang */
    }
    if (!ok) return 0;
    int32_t h[3]; re15_re2arm_hand15(s5, h, NULL);
    pl->x = -20600; pl->z = h[2];                           /* an der Westwand vor der Hand (Flur ab -20622) */
    g->se_vor = s_se_n;
    for (int f = 0; f < 400 && g->griff_bild < 0; f++) {
        const int32_t vx = pl->x, vz = pl->z;              /* Lage, die das Tor A3 in diesem Bild sieht (vor dem Pin) */
        frame_step();
        int mode = 0; re15_re2arm_koerper_zustand(s5, &mode, NULL, NULL, NULL, NULL);
        if (mode == K_GRIFF && g->zugriff_clip < 0) { int c = -1; re15_re2arm_koerper_info(s5, NULL, &c, NULL, NULL); g->zugriff_clip = c; }
        if (arm->sub_state_1 == 4) {
            g->griff_bild = f;
            int32_t hh[3]; re15_re2arm_hand15(s5, hh, NULL);
            double dx = hh[0] - vx, dz = hh[2] - vz; g->gate_abstand = sqrt(dx*dx + dz*dz);
            int sk[2]; re15_re2arm_koerper_zustand(s5, NULL, NULL, sk, NULL, NULL); g->sk_griff = sk[0];
        }
    }
    if (g->griff_bild < 0) return 1;
    leon_laden(modell);
    for (int f = 0; f < 300 && g->loesen_bild < 0; f++) {
        frame_step();
        if (re15_player_is_grabbed() && re15_player_victim_state() == 4 && arm->sub_state_1 == 4) {
            int32_t hh[3]; re15_re2arm_hand15(s5, hh, NULL);
            double d = leon_naechster(pl, hh);
            if (d > g->hand_leon_max) g->hand_leon_max = d;
            int r = re15_re2arm_kontakt_rest(s5); if (r > g->kontakt_max) g->kontakt_max = r;
            if (pl->x < g->leon_x_min) g->leon_x_min = pl->x;
            if (pl->x > g->leon_x_max) g->leon_x_max = pl->x;
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
    if (getenv("R42_HAND_SOLL")) g_hand_soll = atoi(getenv("R42_HAND_SOLL"));
    printf("=== ROOM1210 Gitterhaende: RE1.5-Modell wie das Original, Greifen vom RE2-Gehirn (Runde 42 Stufe 2, Runde 4) ===\n");
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
    /* dazu 300 Bilder mit Leon an der Westwand vor Satz 5: das RE2-Gehirn faehrt aus/greift zu — auch dann wird nichts
     * gestreckt (Gegenprobe db67b1388: dort streckt der Port den RE2-Arm bis zur RE2-Hand, S2 sieht es an der Matrix) */
    {   int32_t hx = 0, hz = 0; re15_re2arm_home(s5, NULL, &hx, &hz);
        pl->x = -20600; pl->z = hz;
        for (int f = 0; f < 300; f++) { frame_step(); teile_pruefen(s5, 1); teile_pruefen(s9, 0); } }
    printf("  Weg B[1] Bild 24: West %d (Original %d), Ost %d (Original %d), Clip %d Bild %d; Ende B[1]: West %d / Ost %d\n",
           (int)w24, ORIG_B24_WEST, (int)o24, ORIG_B24_OST, c24, f24, (int)wr, (int)orr);
    CHECK("(K1) Weg B[1] Bild 24 = Original-RAM: West -23000, Ost -15938, Clip 2, Bild 25",
          b24 && w24 == ORIG_B24_WEST && o24 == ORIG_B24_OST && c24 == 2 && f24 == 25);
    CHECK("(K2) Ende B[1] = Original-RAM: West -22580, Ost -16358 (Wandklemme r 300)", ruhe && wr == ORIG_RUHE_WEST && orr == ORIG_RUHE_OST);
    printf("  Teile: %d Bilder, S1-Fehler %d, S2-Fehler %d, Skalierung max %d (%.2f), B[2] %d Bilder (West x %d..%d), "
           "B[3] %d Bilder, Zuwachs-Bilder %d (Fehler %d), y %d..%d, RE1.5-Hand West x max %d (bei %.2f)\n",
           T.bilder, T.fehl_s1, T.fehl_s2, T.sk_max, T.sk_max / 4096.0, T.b2_bilder, (int)T.b2_x_min, (int)T.b2_x_max,
           T.b3_bilder, T.d_bilder, T.d_fehl, T.ky_min, T.ky_max, T.hand_x_max, T.hand_x_max_sk / 4096.0);
    CHECK("(W1) nach B[1] wiegt der Koerper (B[2]) hinter der Wand: West x zwischen -22580-5*177 und der Wandklemme -22450",
          T.b2_bilder > 100 && T.b2_x_max <= -22450 && T.b2_x_min >= ORIG_RUHE_WEST - 5 * 177);
    CHECK("(W2) und laesst die Arme wachsen (B[3]); y zittert +-5 um das gemerkte y (@0x8010cc88-b4, wandert je B[3] um +-5)",
          T.b3_bilder > 100 && T.ky_min < 0 && T.ky_max > 0 && T.ky_max - T.ky_min <= 5 * 2 * 8);
    CHECK("(W3) in B[3] P1 waechst Arm 2 je Bild um (rng&7)+5, Arm 3 um rng&7 (Original-RAM lauf4: +6 je Bild)",
          T.d_bilder > 100 && T.d_fehl == 0);
    CHECK("(S1) kein Teil wird vom Port gestreckt: Rumpf/Kopf 4096, Arme 4096 oder genau die Original-Skalierung (B[3])",
          T.bilder > 1000 && T.fehl_s1 == 0 && T.sk_max <= 4096 + 356 * 12);   /* 356 = (0xff)+100+1, 12 = 7+5 */
    CHECK("(S2) unabhaengig an der gezeichneten Matrix: Spalten 0/2 = 4096, Spalte 1 = Original-Skalierung (+-0,2 %)",
          T.bilder > 1000 && T.fehl_s2 == 0);

    /* ---- Lauf 2/3: Griff mit Leon und Elza ---- */
    for (int m = 0; m < 2; m++) {
        const int modell = m ? 4 : 0;
        const char *wer = m ? "Elza" : "Leon";
        g_current_room_id = 0x1211; (void)re15_re2arm_tick(s5); g_current_room_id = 0x1210;   /* Modul-Reset */
        griff_t g; int sg = 0;
        memset(&T, 0, sizeof T); T.ky_min = 999999; T.ky_max = -999999; T.b2_x_min = 999999; T.b2_x_max = -999999;
        const int rc = griff_lauf(modell, &g, &sg);
        printf("  %s: Griff Bild %d (Hand %.0f von der Figur, Arm-Skalierung %.2f), Zugriff-Clip %d, Halten %d Bilder bis Bild %d, "
               "Hand-Leon max %.0f, Kontakt-Rest max %d, Figur x %d..%d, Rueckzug %d Bilder, Sinken-Clip %d, hp %d, cd %d, Toene %d\n",
               wer, g.griff_bild, g.gate_abstand, g.sk_griff / 4096.0, g.zugriff_clip, g.halten_n, g.loesen_bild,
               g.hand_leon_max, g.kontakt_max, g.leon_x_min, g.leon_x_max, g.rz_bilder, g.sinken_clip, g.hp_nach, g.cd_nach,
               g.se_nach - g.se_vor);
        char t1[200];
        snprintf(t1, sizeof t1, "(H1 %s) mit langem Arm an der Wand greift die RE1.5-Hand (Hand < 450 von der Figur, @0x80073e9a); "
                 "Zugriff zeichnet EM1A-Clip 2", wer);
        CHECK(t1, rc == 1 && g.griff_ok && g.griff_bild >= 0 && g.gate_abstand < 450.0 && g.zugriff_clip == 2);
        snprintf(t1, sizeof t1, "(H2 %s) im Halten liegt die Hand an der Figur: Kontakt-Rest <= halbe Handbreite 101, naechster Leon-Vertex "
                 "<= Handdicke 202", wer);
        CHECK(t1, g.halten_n > 100 && g.kontakt_max <= 101 && g.hand_leon_max <= 202.0);
        snprintf(t1, sizeof t1, "(H3 %s) die Figur bleibt im Flur: ihr Klemmzylinder (450 @0x80073e9a) bleibt vor der Westwand -21090", wer);
        CHECK(t1, g.halten_n > 100 && g.leon_x_min - 450 >= WEST_FLAECHE);
        snprintf(t1, sizeof t1, "(R1 %s) RE2: Halten 152 Bilder ohne Tasten (@0x80100D28-40), dann Cooldown 120 (@0x80100D5C)", wer);
        CHECK(t1, g.loesen_bild == 152 && g.cd_nach == 119);
        snprintf(t1, sizeof t1, "(R2 %s) RE2-Rueckzug 10 Bilder -> 0x701, hp -1 (@0x80100E38-50); der Koerper sinkt mit Clip 3", wer);
        CHECK(t1, g.rz_bilder == 10 && g.hp_nach == -1 && g.sinken_clip == 3);
        snprintf(t1, sizeof t1, "(R3 %s) RE2-Toene laufen (Ausfahren/Halten/Rueckzug)", wer);
        CHECK(t1, g.se_nach - g.se_vor >= 3);
    }

    /* ---- Lauf 4: Griff-Bilder einzeln pruefen (S1/S2/S3 im Griff), Leon ---- */
    if (!re15_re2arm_koerper_zustand || !re15_re2arm_hand15) CHECK("(S3) Koerper-Zustand/RE1.5-Hand abfragbar", 0);
    else {   g_current_room_id = 0x1211; (void)re15_re2arm_tick(s5); g_current_room_id = 0x1210;
        int sl[RE15_ACTOR_MAX];
        const int n4 = aufsetzen(sl, 0);
        const int a5 = sl[4];
        memset(&T, 0, sizeof T); T.ky_min = 999999; T.ky_max = -999999; T.b2_x_min = 999999; T.b2_x_max = -999999; T.hand_x_max = -999999;
        pl->x = -19393; pl->z = 6000;
        for (int i = 0; i < n4; i++) g_actors[sl[i]].grid_id = 1;
        int ok = 0;
        for (int f = 0; f < 6000 && !ok; f++) {
            frame_step();
            int mode = 0, phase = 0, sk[2], an[2];
            re15_re2arm_koerper_zustand(a5, &mode, &phase, sk, an, NULL);
            int32_t hh[3];
            if (mode == K_B3 && phase == 1 && re15_re2arm_hand15(a5, hh, NULL) && hh[0] >= g_hand_soll) ok = 1;
        }
        int32_t h[3]; re15_re2arm_hand15(a5, h, NULL);
        pl->x = -20600; pl->z = h[2];
        for (int f = 0; f < 700; f++) { frame_step(); teile_pruefen(a5, 1); }
        printf("  Griff-Lauf Teile: %d Bilder, Griff %d Bilder, S1 %d, S2 %d, Skalierungs-Aenderungen im Griff %d\n",
               T.bilder, T.griff_bilder, T.fehl_s1, T.fehl_s2, T.griff_sk_aend);
        CHECK("(S3) im Griff steht die Skalierung (Zugriff/Halten), kein Teil gestreckt (S1/S2 auch im Griff)",
              ok && T.griff_bilder > 150 && T.griff_sk_aend == 0 && T.fehl_s1 == 0 && T.fehl_s2 == 0);
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
