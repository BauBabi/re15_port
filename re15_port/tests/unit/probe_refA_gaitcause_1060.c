/* probe_refA_gaitcause_1060.c — ADVERSARIALER PRUEFER (Lane "zonen-karte", Gegenprobe)
 *
 * Zweck: die KAUSALITAET der Befunde A2/A4/A5 pruefen, nicht nur die Korrelation.
 *   (1) Per-Tick-Trace des LETZTEN Treppenlaufs (WEST 2->0): dz je Tick,
 *       Anzahl der Ticks mit dz == +10 (= FK-Zug ausgelassen) und der
 *       RUECKWAERTS-Ticks. Prueft A4 ("5-6 tote Zug-Ticks") und A5 ("-50..-65").
 *   (2) TRIAL-PATCH: derselbe Lauf, aber pro Gait-Tick wird ein zusaetzlicher
 *       Vorwaerts-Betrag injiziert, so dass der Gesamt-Gait einen Zielwert
 *       erreicht. Ueber das VOLLE legale Ausloese-Fenster gemessen: ab welchem
 *       Gait-Weg schaltet RVD[13] (bzw. RVD[10]) in 100 % der Faelle?
 *       -> zeigt, ob der Gait-Weg WIRKLICH die Ursache ist und ob die von A2
 *          geforderten 5036 noetig sind oder weniger reicht.
 *   (3) A4-SIMULATION: der FK-Zug mit ZWEI Fuss-Caches und UNBEDINGTER Anwendung
 *       (Original FUN_800390e0 @0x800390e0: s3 = *(entity+0x188)+688+sel*516,
 *        `lw a0,84(s3)` @0x800391a0 / `lw a0,92(s3)` @0x800391c0, Stores
 *        @0x800391b0/0x800391d0 UNBEDINGT) — nachgerechnet mit derselben
 *       Pose-API, die stair_common.c benutzt. Liefert den Weg, den A4 wirklich
 *       zurueckgibt.
 *
 * KEIN Engine-Code geaendert. Reine Messung.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_aot.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static re15_rdt_t g_rdt;
static re15_emd_skeleton_t  g_skel;
static re15_emd_animation_t g_anim;
static int g_skel_ok = 0;

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* ---------------------------------------------------------------- (1)+(2) */
typedef struct {
    int32_t gait_end, land, eject;
    int     in_zone_frames, ticks;
    int     dead_ticks;     /* |dz| == 10 exakt -> FK-Zug ausgelassen */
    int     back_ticks;     /* dz gegen die Laufrichtung */
    int32_t worst_back;     /* groesster Rueckwaerts-Ausschlag */
    char    seq[160];
    int     end_cut;
    int     flips;
} runres_t;

/* extra_num/extra_den: pro Gait-Tick wird der bisherige Tick-Vorschub um
 * (extra_num/extra_den) gestreckt (Trial-Patch, NUR in der Sonde). */
static void one_run(int32_t x, int32_t z0, int rot, int band, int cut_in,
                    int zone_lo, int zone_hi, int dir,
                    int extra_num, int extra_den, int trace, runres_t *out)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(out, 0, sizeof *out);
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = x; pl->z = z0; pl->y = -(int32_t)band * 0x708;
    pl->rot_y = (int16_t)rot; pl->motion = 0; pl->anim_frame = 0; pl->anim_flags = 0;
    re15_stair_reset();
    re15_collision_set_band(band);
    int cut = cut_in;
    g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;

    if (!re15_stair_try_start(&g_rdt, 1)) { out->end_cut = -1; return; }

    int sl = 0; sl += snprintf(out->seq + sl, sizeof out->seq - sl, "%d", cut);
    int32_t z_pre = pl->z; int n = 0;
    while (re15_stair_active() && n < 900) {
        z_pre = pl->z;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        int32_t dz = pl->z - z_pre;
        if (re15_stair_active()) {           /* Gait-Tick (kein Finalize) */
            if (dz == 10 * dir) out->dead_ticks++;
            if ((int32_t)(dz * dir) < 0) {
                out->back_ticks++;
                if (-(int32_t)(dz * dir) > out->worst_back) out->worst_back = -(int32_t)(dz * dir);
            }
            if (extra_num) {                  /* TRIAL-PATCH: Gait strecken */
                int32_t add = (int32_t)(((int64_t)dz * extra_num) / extra_den);
                pl->z += add;
            }
            if (trace)
                printf("    t%02d cursor~%d  z=%6ld  dz=%+5ld%s\n", n, n,
                       (long)pl->z, (long)dz,
                       (dz == 10 * dir) ? "   <-- nur Vorwaerts-10 (FK ausgelassen)"
                                        : ((int32_t)(dz * dir) < 0 ? "   <-- RUECKWAERTS" : ""));
        }
        if (pl->z >= zone_lo && pl->z <= zone_hi) out->in_zone_frames++;
        g_scd.cut_auto_enabled = 1;
        re15_aot_scan(pl->x, pl->z, (uint8_t)cut);
        if (g_scd.cam_change_pending) {
            int nc = (int)g_scd.cam_id; g_scd.cam_change_pending = 0;
            if (nc != cut) { cut = nc; out->flips++;
                if (sl < (int)sizeof out->seq - 8)
                    sl += snprintf(out->seq + sl, sizeof out->seq - sl, "->%d", cut); }
        }
        n++;
    }
    out->ticks = n; out->gait_end = z_pre; out->land = pl->z;
    out->eject = pl->z - z_pre; out->end_cut = cut;
}

/* A3-EXPERIMENT: identischer Lauf, aber der Finalize-Sprung wird durch einen
 * KONTINUIERLICHEN Marsch (Schrittweite `step`) ersetzt und jeder Zwischenschritt
 * gescannt. Zeigt, ob die DISKONTINUITAET (und nicht der Gait-Weg) den Schwenk
 * verschluckt. */
static void one_run_smooth(int32_t x, int32_t z0, int rot, int band, int cut_in,
                           int zone_lo, int zone_hi, int step, runres_t *out)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(out, 0, sizeof *out);
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = x; pl->z = z0; pl->y = -(int32_t)band * 0x708;
    pl->rot_y = (int16_t)rot; pl->motion = 0; pl->anim_frame = 0; pl->anim_flags = 0;
    re15_stair_reset();
    re15_collision_set_band(band);
    int cut = cut_in;
    g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
    if (!re15_stair_try_start(&g_rdt, 1)) { out->end_cut = -1; return; }
    int sl = 0; sl += snprintf(out->seq + sl, sizeof out->seq - sl, "%d", cut);
    int32_t z_pre = pl->z; int n = 0;
    while (re15_stair_active() && n < 900) {
        z_pre = pl->z;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        int last = !re15_stair_active();          /* das war der Finalize-Tick */
        int32_t land = pl->z;
        if (last) {                                /* Sprung durch Marsch ersetzen */
            int32_t from = z_pre, to = land, d = (to > from) ? step : -step;
            for (int32_t z = from; (d > 0) ? (z < to) : (z > to); z += d) {
                pl->z = z;
                if (pl->z >= zone_lo && pl->z <= zone_hi) out->in_zone_frames++;
                g_scd.cut_auto_enabled = 1;
                re15_aot_scan(pl->x, pl->z, (uint8_t)cut);
                if (g_scd.cam_change_pending) {
                    int nc = (int)g_scd.cam_id; g_scd.cam_change_pending = 0;
                    if (nc != cut) { cut = nc; out->flips++;
                        if (sl < (int)sizeof out->seq - 8)
                            sl += snprintf(out->seq + sl, sizeof out->seq - sl, "->%d", cut); }
                }
            }
            pl->z = land;
        }
        if (pl->z >= zone_lo && pl->z <= zone_hi) out->in_zone_frames++;
        g_scd.cut_auto_enabled = 1;
        re15_aot_scan(pl->x, pl->z, (uint8_t)cut);
        if (g_scd.cam_change_pending) {
            int nc = (int)g_scd.cam_id; g_scd.cam_change_pending = 0;
            if (nc != cut) { cut = nc; out->flips++;
                if (sl < (int)sizeof out->seq - 8)
                    sl += snprintf(out->seq + sl, sizeof out->seq - sl, "->%d", cut); }
        }
        n++;
    }
    out->ticks = n; out->gait_end = z_pre; out->land = pl->z;
    out->eject = pl->z - z_pre; out->end_cut = cut;
}

/* A4-TRAJEKTORIE gegen den ECHTEN Kamera-Scan: der mit zwei Fuss-Caches
 * gerechnete Abstieg wird Tick fuer Tick in den Scanner gefuettert. */
static void a4_scan_run(int32_t x, int32_t z0, int rot, int band, int cut_in,
                        int zone_lo, int zone_hi, int seed_cache, runres_t *out);

/* ------------------------------------------------------------------- (3) */
/* A4-SIMULATION: exakt der Gait aus stair_common.c:196-278, aber mit ZWEI
 * Fuss-Caches und UNBEDINGTEM Zug (Original-Mechanismus). Rein rechnerisch,
 * ohne re15_stair_tick — deshalb nur der WEG, nicht die Kamera. */
static void a4_sim(int32_t z0, int32_t x, int band, int rot, int seed_cache,
                   int32_t *out_dz, int *out_back, int32_t *out_worst_back)
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    re15_actor_t A; memset(&A, 0, sizeof A);
    A.active = 1; A.type = 0; A.x = x; A.z = z0; A.y = -(int32_t)band * 0x708;
    A.rot_y = (int16_t)rot; A.motion = 220 /* STAIR_DOWN */; A.anim_frame = 0; A.anim_flags = 0;

    const int CLIPLEN = 42, CLIP = 21;
    int32_t cs = re15_cos_q12(rot), sn = re15_sin_q12(rot);
    int32_t ref[2][3]; int ref_ok[2] = {0, 0};
    int cursor = 0, bands_left = 1, back = 0; int32_t worst = 0, z_start = z0;

    if (seed_cache) {                       /* Cache aus der Pose bei cursor 0 */
        int kf = re15_compute_actor_kf(&g_anim, &g_skel, &A, CLIP, 0u);
        g_anim_pose_actor = NULL;
        re15_skel_compute_pose(&g_skel, kf, poses);
        for (int f = 0; f < 2; f++) {
            int b = f ? 7 : 4;
            int32_t lx = poses[b].trans[0], ly = poses[b].trans[1], lz = poses[b].trans[2];
            ref[f][0] = (int32_t)(( (int64_t)cs * lx + (int64_t)sn * lz) >> 12);
            ref[f][1] = ly;
            ref[f][2] = (int32_t)((-(int64_t)sn * lx + (int64_t)cs * lz) >> 12);
            ref_ok[f] = 1;
        }
    }
    for (int n = 0; n < 900; n++) {
        int32_t zb = A.z;
        cursor++;
        int frame = cursor % CLIPLEN;
        A.anim_frame = (uint16_t)frame;
        int sel  = (cursor >= 11 && cursor < 26) ? 1 : 0;
        int kf = re15_compute_actor_kf(&g_anim, &g_skel, &A, CLIP, (uint32_t)frame);
        g_anim_pose_actor = NULL;
        re15_skel_compute_pose(&g_skel, kf, poses);
        int32_t w[2][3];
        for (int f = 0; f < 2; f++) {
            int b = f ? 7 : 4;
            int32_t lx = poses[b].trans[0], ly = poses[b].trans[1], lz = poses[b].trans[2];
            w[f][0] = (int32_t)(( (int64_t)cs * lx + (int64_t)sn * lz) >> 12);
            w[f][1] = ly;
            w[f][2] = (int32_t)((-(int64_t)sn * lx + (int64_t)cs * lz) >> 12);
        }
        if (ref_ok[sel]) {                    /* UNBEDINGT (Original) */
            A.x -= (w[sel][0] - ref[sel][0]);
            A.z -= (w[sel][2] - ref[sel][2]);
        }
        for (int f = 0; f < 2; f++) { ref[f][0]=w[f][0]; ref[f][1]=w[f][1]; ref[f][2]=w[f][2]; ref_ok[f]=1; }
        if (cursor < 31) {
            A.x += (int32_t)(( (int64_t)cs * 10) >> 12);
            A.z += (int32_t)((-(int64_t)sn * 10) >> 12);
        }
        int32_t dz = A.z - zb;
        if (dz < 0) { back++; if (-dz > worst) worst = -dz; }
        if (cursor >= 32) {
            if (bands_left <= 0) break;
            bands_left--; cursor = 2;         /* KEIN Cache-Reseed (A4-Fix) */
        }
    }
    *out_dz = A.z - z_start; *out_back = back; *out_worst_back = worst;
}

static void a4_scan_run(int32_t x, int32_t z0, int rot, int band, int cut_in,
                        int zone_lo, int zone_hi, int seed_cache, runres_t *out)
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    memset(out, 0, sizeof *out);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = x; pl->z = z0; pl->y = -(int32_t)band * 0x708;
    pl->rot_y = (int16_t)rot; pl->motion = 220; pl->anim_frame = 0; pl->anim_flags = 0;
    re15_collision_set_band(band);
    int cut = cut_in;
    g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
    int sl = 0; sl += snprintf(out->seq + sl, sizeof out->seq - sl, "%d", cut);

    const int CLIPLEN = 42, CLIP = 21;
    int32_t cs = re15_cos_q12(rot), sn = re15_sin_q12(rot);
    int32_t ref[2][3]; int ref_ok[2] = {0,0};
    int cursor = 0, bands_left = 1;
    if (seed_cache) {
        int kf = re15_compute_actor_kf(&g_anim, &g_skel, pl, CLIP, 0u);
        g_anim_pose_actor = NULL; re15_skel_compute_pose(&g_skel, kf, poses);
        for (int f = 0; f < 2; f++) { int b = f ? 7 : 4;
            int32_t lx = poses[b].trans[0], ly = poses[b].trans[1], lz = poses[b].trans[2];
            ref[f][0] = (int32_t)(( (int64_t)cs * lx + (int64_t)sn * lz) >> 12);
            ref[f][1] = ly;
            ref[f][2] = (int32_t)((-(int64_t)sn * lx + (int64_t)cs * lz) >> 12);
            ref_ok[f] = 1; }
    }
    for (int n = 0; n < 900; n++) {
        cursor++;
        int frame = cursor % CLIPLEN;
        pl->anim_frame = (uint16_t)frame;
        int sel = (cursor >= 11 && cursor < 26) ? 1 : 0;
        int kf = re15_compute_actor_kf(&g_anim, &g_skel, pl, CLIP, (uint32_t)frame);
        g_anim_pose_actor = NULL; re15_skel_compute_pose(&g_skel, kf, poses);
        int32_t w[2][3];
        for (int f = 0; f < 2; f++) { int b = f ? 7 : 4;
            int32_t lx = poses[b].trans[0], ly = poses[b].trans[1], lz = poses[b].trans[2];
            w[f][0] = (int32_t)(( (int64_t)cs * lx + (int64_t)sn * lz) >> 12);
            w[f][1] = ly;
            w[f][2] = (int32_t)((-(int64_t)sn * lx + (int64_t)cs * lz) >> 12); }
        if (ref_ok[sel]) { pl->x -= (w[sel][0] - ref[sel][0]); pl->z -= (w[sel][2] - ref[sel][2]); }
        for (int f = 0; f < 2; f++) { ref[f][0]=w[f][0]; ref[f][1]=w[f][1]; ref[f][2]=w[f][2]; ref_ok[f]=1; }
        if (cursor < 31) {
            pl->x += (int32_t)(( (int64_t)cs * 10) >> 12);
            pl->z += (int32_t)((-(int64_t)sn * 10) >> 12);
        }
        if (pl->z >= zone_lo && pl->z <= zone_hi) out->in_zone_frames++;
        g_scd.cut_auto_enabled = 1;
        re15_aot_scan(pl->x, pl->z, (uint8_t)cut);
        if (g_scd.cam_change_pending) {
            int nc = (int)g_scd.cam_id; g_scd.cam_change_pending = 0;
            if (nc != cut) { cut = nc; out->flips++;
                if (sl < (int)sizeof out->seq - 8)
                    sl += snprintf(out->seq + sl, sizeof out->seq - sl, "->%d", cut); }
        }
        out->ticks = n + 1;
        if (cursor >= 32) { if (bands_left <= 0) break; bands_left--; cursor = 2; }
    }
    out->gait_end = pl->z; out->end_cut = cut;
}

/* dasselbe, aber MIT dem Port-Verhalten (ein Cache + Skip bei Fusswechsel und
 * Band-Loop) — Kontrolle, dass die Nachrechnung den Port reproduziert. */
static void port_sim(int32_t z0, int32_t x, int band, int rot,
                     int32_t *out_dz, int *out_dead, int *out_back, int32_t *out_worst)
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    re15_actor_t A; memset(&A, 0, sizeof A);
    A.active = 1; A.type = 0; A.x = x; A.z = z0; A.y = -(int32_t)band * 0x708;
    A.rot_y = (int16_t)rot; A.motion = 220; A.anim_frame = 0; A.anim_flags = 0;
    const int CLIPLEN = 42, CLIP = 21;
    int32_t cs = re15_cos_q12(rot), sn = re15_sin_q12(rot);
    int32_t ref[3] = {0,0,0}; int ref_ok = 0, prev_sel = -1;
    int cursor = 0, bands_left = 1, dead = 0, back = 0; int32_t worst = 0, z_start = z0;
    for (int n = 0; n < 900; n++) {
        int32_t zb = A.z;
        cursor++;
        int frame = cursor % CLIPLEN;
        A.anim_frame = (uint16_t)frame;
        int sel  = (cursor >= 11 && cursor < 26) ? 1 : 0;
        int b = sel ? 7 : 4;
        int kf = re15_compute_actor_kf(&g_anim, &g_skel, &A, CLIP, (uint32_t)frame);
        g_anim_pose_actor = NULL;
        re15_skel_compute_pose(&g_skel, kf, poses);
        int32_t lx = poses[b].trans[0], ly = poses[b].trans[1], lz = poses[b].trans[2];
        int32_t wx = (int32_t)(( (int64_t)cs * lx + (int64_t)sn * lz) >> 12);
        int32_t wz = (int32_t)((-(int64_t)sn * lx + (int64_t)cs * lz) >> 12);
        if (ref_ok && sel == prev_sel) { A.x -= (wx - ref[0]); A.z -= (wz - ref[2]); }
        ref[0] = wx; ref[1] = ly; ref[2] = wz; ref_ok = 1; prev_sel = sel;
        if (cursor < 31) {
            A.x += (int32_t)(( (int64_t)cs * 10) >> 12);
            A.z += (int32_t)((-(int64_t)sn * 10) >> 12);
        }
        int32_t dz = A.z - zb;
        if (dz == 10) dead++;
        if (dz < 0) { back++; if (-dz > worst) worst = -dz; }
        if (cursor >= 32) {
            ref_ok = 0;                        /* Port: Reseed ueber den Band-Loop */
            if (bands_left <= 0) break;
            bands_left--; cursor = 2;
        }
    }
    *out_dz = A.z - z_start; *out_dead = dead; *out_back = back; *out_worst = worst;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    {
        char p2[600]; size_t s1 = 0, s2 = 0;
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EDD", base);
        uint8_t *edd = read_file(p2, &s1);
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EMR", base);
        uint8_t *emr = read_file(p2, &s2);
        if (edd && emr && re15_emd_parse_animation(edd, s1, &g_anim) == 0 &&
            re15_emd_parse_skeleton(emr, s2, &g_skel) == 0) g_skel_ok = 1;
        printf("PL00 %s\n", g_skel_ok ? "geladen" : "FEHLT!");
    }
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data || re15_rdt_parse(data, size, &g_rdt) != 0) { printf("FAIL\n"); return 1; }
    re15_actor_init(); scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 26000; pl->y = -8*0x708; pl->z = 25300;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    runres_t r;

    printf("\n######## (1) PER-TICK-TRACE letzte Treppe WEST 2->0, z0=20332 (max. erreichbar) ########\n");
    one_run(21400, 20332, 3072, 2, 4, 24000, 25000, +1, 0, 1, 1, &r);
    printf("  -> Gait %+ld ueber %d Ticks, tote Ticks(dz==+10)=%d, Rueckwaerts-Ticks=%d (max %ld)\n",
           (long)(r.gait_end - 20332), r.ticks, r.dead_ticks, r.back_ticks, (long)r.worst_back);
    printf("  -> Kette %s, Frames in RVD[13]=%d\n", r.seq, r.in_zone_frames);

    printf("\n######## (1b) PER-TICK-TRACE Treppe 3 OST 4->2, z0=24570 (min. erreichbar) ########\n");
    one_run(25150, 24570, 1024, 4, 2, 20000, 21000, -1, 0, 1, 1, &r);
    printf("  -> Gait %+ld, tote Ticks=%d, Rueckwaerts-Ticks=%d (max %ld), Kette %s\n",
           (long)(r.gait_end - 24570), r.dead_ticks, r.back_ticks, (long)r.worst_back, r.seq);

    printf("\n######## (2) TRIAL-PATCH: Gait strecken, volles legales Fenster ########\n");
    printf("  Legales Fenster LETZTE Treppe (WEST 2->0): z0 in [20080..20332] (620-Sonde .. Resolver-Grenze)\n");
    printf("  Legales Fenster Treppe 3   (OST  4->2): z0 in [24568..24820]\n");
    printf("  num/den   Gait      | letzte Treppe: ok/fail/pingpong | Treppe 3: ok/fail/pingpong\n");
    for (int num = 0; num <= 20; num++) {
        int okA = 0, failA = 0, ppA = 0, okB = 0, failB = 0, ppB = 0; int32_t gaitA = 0, gaitB = 0;
        for (int32_t z = 20080; z <= 20332; z += 4) {
            one_run(21400, z, 3072, 2, 4, 24000, 25000, +1, num, 20, 0, &r);
            if (r.end_cut < 0) continue;
            gaitA = r.gait_end - z;
            if (r.end_cut == 5) okA++; else failA++;
            if (r.flips > 1) ppA++;            /* SOLL = genau 1 Wechsel (4->5) */
        }
        for (int32_t z = 24568; z <= 24820; z += 4) {
            one_run(25150, z, 1024, 4, 2, 20000, 21000, -1, num, 20, 0, &r);
            if (r.end_cut < 0) continue;
            gaitB = z - r.gait_end;
            if (r.end_cut == 4) okB++; else failB++;
            if (r.flips > 2) ppB++;            /* SOLL = genau 2 Wechsel (2->3->4) */
        }
        printf("   +%2d/20   %5ld/%5ld | %3d ok %3d FAIL %3d pingpong   | %3d ok %3d FAIL %3d pingpong\n",
               num, (long)gaitA, (long)gaitB, okA, failA, ppA, okB, failB, ppB);
    }

    printf("\n######## (3) A4-SIMULATION (zwei Fuss-Caches, unbedingter Zug) ########\n");
    if (g_skel_ok) {
        int32_t dzp, dza, dza2; int deadp, backp, backa, backa2; int32_t worstp, worsta, worsta2;
        port_sim(20332, 21400, 2, 3072, &dzp, &deadp, &backp, &worstp);
        a4_sim(20332, 21400, 2, 3072, 0, &dza,  &backa,  &worsta);
        a4_sim(20332, 21400, 2, 3072, 1, &dza2, &backa2, &worsta2);
        printf("  WEST 2->0 (rot=3072):\n");
        printf("    Port-Nachrechnung (1 Cache + Skip): dz=%+ld  tote=%d  rueckw=%d (max %ld)\n",
               (long)dzp, deadp, backp, (long)worstp);
        printf("    A4 (2 Caches, unbedingt, kein Seed): dz=%+ld  rueckw=%d (max %ld)\n",
               (long)dza, backa, (long)worsta);
        printf("    A4 (2 Caches, unbedingt, mit Seed) : dz=%+ld  rueckw=%d (max %ld)\n",
               (long)dza2, backa2, (long)worsta2);
        printf("    GEOMETRIE-FORDERUNG A2 = 5036 (Zelle 4100 + 2x468)\n");
    }

    printf("\n######## (4) A4-TRAJEKTORIE (2 Caches, unbedingt) gegen den ECHTEN Scan ########\n");
    {
        int ok=0, fail=0, pp=0, ok2=0, fail2=0, pp2=0, minz=1<<30;
        for (int32_t z = 20080; z <= 20332; z += 4) {
            a4_scan_run(21400, z, 3072, 2, 4, 24000, 25000, 1, &r);
            if (r.end_cut == 5) ok++; else fail++;
            if (r.flips > 1) pp++;
            if (r.in_zone_frames < minz) minz = r.in_zone_frames;
        }
        printf("  letzte Treppe WEST 2->0, Fenster [20080..20332]: %d ok / %d FAIL / %d pingpong, min Frames in Zone=%d\n",
               ok, fail, pp, minz);
        minz = 1<<30;
        for (int32_t z = 24568; z <= 24820; z += 4) {
            a4_scan_run(25150, z, 1024, 4, 2, 20000, 21000, 1, &r);
            if (r.end_cut == 4) ok2++; else fail2++;
            if (r.flips > 2) pp2++;
            if (r.in_zone_frames < minz) minz = r.in_zone_frames;
        }
        printf("  Treppe 3     OST  4->2, Fenster [24568..24820]: %d ok / %d FAIL / %d pingpong, min Frames in Zone=%d\n",
               ok2, fail2, pp2, minz);
    }

    printf("\n######## (5) A3-EXPERIMENT: Finalize-Sprung durch KONTINUIERLICHEN Marsch ersetzt ########\n");
    for (int step = 60; step <= 60; step += 60) {
        int ok=0, fail=0, pp=0;
        for (int32_t z = 20080; z <= 20332; z += 4) {
            one_run_smooth(21400, z, 3072, 2, 4, 24000, 25000, step, &r);
            if (r.end_cut == 5) ok++; else fail++;
            if (r.flips > 1) pp++;
        }
        printf("  Schrittweite %d: letzte Treppe %d ok / %d FAIL / %d pingpong\n", step, ok, fail, pp);
        ok=fail=pp=0;
        for (int32_t z = 24568; z <= 24820; z += 4) {
            one_run_smooth(25150, z, 1024, 4, 2, 20000, 21000, step, &r);
            if (r.end_cut == 4) ok++; else fail++;
            if (r.flips > 2) pp++;
        }
        printf("  Schrittweite %d: Treppe 3     %d ok / %d FAIL / %d pingpong\n", step, ok, fail, pp);
    }
    printf("  (Vergleich stock: siehe (2) Zeile +0/20)\n");

    printf("\n######## (6) GESAMT-VERSATZ Gait+Eject vs. A2-Forderung 5036 ########\n");
    for (int32_t z = 20080; z <= 20332; z += 84) {
        one_run(21400, z, 3072, 2, 4, 24000, 25000, +1, 0, 1, 0, &r);
        printf("  z0=%ld: Gait %+ld  Eject %+ld  GESAMT %+ld  Landung %ld  (Wandflaeche->Wandflaeche = %ld)\n",
               (long)z, (long)(r.gait_end - z), (long)r.eject, (long)(r.land - z), (long)r.land,
               (long)(r.land - 20332));
    }

    printf("\n######## (7) A6: wirkt STAIR_REACH 450 (sce_flags==0 Legacy-Zweig) irgendwo? ########\n");
    {
        struct { int32_t x, z; int rot, band; const char *what; } cases[] = {
            { 21400, 25368, 3072, 0, "Landepunkt LETZTE Treppe, Blick +z (WEG von der Treppe)" },
            { 21400, 25368, 1024, 0, "Landepunkt LETZTE Treppe, Blick -z (ZUR Treppe)" },
            { 21400, 20332, 3072, 2, "max. Annaeherung WEST oben, Blick +z" },
            { 25150, 19532, 1024, 2, "Landepunkt OST unten, Blick -z (WEG)" },
            { 25150, 19532, 3072, 2, "Landepunkt OST unten, Blick +z (ZUR Treppe)" },
            { 21400, 24500, 3072, 0, "mitten im West-Schacht (nicht erreichbar), Blick +z" },
        };
        for (unsigned i = 0; i < sizeof cases / sizeof cases[0]; i++) {
            re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            /* Sonden-Punkt + Reach-Test von Hand, gegen JEDEN Stair-Slot des Bandes */
            int32_t sn2 = re15_sin_q12(cases[i].rot), cs2 = re15_cos_q12(cases[i].rot);
            int32_t fx = cases[i].x + (cs2 * 620) / 4096, fz = cases[i].z - (sn2 * 620) / 4096;
            int probe_hit = -1, reach_hit = -1;
            for (int s = 0; s < RE15_AOT_MAX; s++) {
                const re15_aot_t *a = &g_aot.slots[s];
                if (!a->active || a->type != RE15_AOT_TYPE_STAIR) continue;
                if ((int)a->band != cases[i].band) continue;
                int32_t ddx = fx - a->x, ddz = fz - a->z;
                if ((ddx < 0 ? -ddx : ddx) <= a->half_w && (ddz < 0 ? -ddz : ddz) <= a->half_h && probe_hit < 0) probe_hit = s;
                ddx = cases[i].x - a->x; ddz = cases[i].z - a->z;
                if ((ddx < 0 ? -ddx : ddx) <= a->half_w + 450 && (ddz < 0 ? -ddz : ddz) <= a->half_h + 450 && reach_hit < 0) reach_hit = s;
            }
            /* und was TUT der Port wirklich? */
            p->active = 1; p->type = 0; p->hp = 100;
            p->x = cases[i].x; p->z = cases[i].z; p->y = -(int32_t)cases[i].band * 0x708;
            p->rot_y = (int16_t)cases[i].rot; p->motion = 0; p->anim_frame = 0; p->anim_flags = 0;
            re15_stair_reset(); re15_collision_set_band(cases[i].band);
            int started = re15_stair_try_start(&g_rdt, 1);
            printf("  %-56s : Sonde620->slot %2d | Reach450->slot %2d | Port startet=%d rot_nach=%d\n",
                   cases[i].what, probe_hit, reach_hit, started, (int)p->rot_y);
            re15_stair_reset();
        }
    }
    return 0;
}
