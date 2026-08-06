/* probe_adv_ejectcam_1060.c — ADVERSARIALE GEGENPROBE zu LANE-B Finding LB-01.
 *
 * FRAGE: Ist der Port-EJECT im Finalize-Tick (stair_common.c:155-184) die URSACHE
 * dafuer, dass die Kamera-Kette in ROOM1060 desynchronisiert (0->0->0->1->2)?
 *
 * METHODE (Trial-Patch auf Sonden-Ebene, KEIN Engine-Code geaendert):
 *   A) Kette wie ausgeliefert (Port inkl. Eject).
 *   B) Kette mit UNTERDRUECKTEM Eject: nach dem Finalize-Tick wird x/z auf den
 *      Wert VOR dem Tick zurueckgesetzt (der Finalize schreibt sonst nur y/band/
 *      motion — genau das, was LAB_80038e50 @0x80038e50-0x80038eec tut).
 *   C) Kette mit der BYTE-TRUEN Gait (Original-Modell: Pose = keyframe[cursor VOR
 *      dem ++] @0x8001f35c + FUN_8001f3bc:89-91, FK-Referenz PRO KNOCHEN
 *      `lw a0,84(s3)` @0x800391a0 mit s3 = base+footsel*0x204+0x2b0 @0x80039148-58,
 *      KEIN Skip beim Fuss-Wechsel `jal FUN_800390e0` @0x80038d50 unbedingt,
 *      KEIN Eject) — Position pro Frame in den RVD-Scan gefuettert.
 *
 * Zusaetzlich: fuer jeden Treppen-Start werden ALLE CAM_SWITCH-Zonen mit
 * cam_from == aktuellem Cut aufgelistet und geprueft, ob die jeweilige Bahn sie
 * ueberhaupt beruehren KANN (Bounding-Box-Vergleich).
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
static int g_cut = 0;
static unsigned g_frame = 0;
static int g_quiet = 0;

/* Bounding-Box der gerade laufenden Bahn */
static int32_t bb_x0, bb_x1, bb_z0, bb_z1;
static void bb_reset(int32_t x, int32_t z) { bb_x0 = bb_x1 = x; bb_z0 = bb_z1 = z; }
static void bb_add(int32_t x, int32_t z)
{
    if (x < bb_x0) bb_x0 = x; if (x > bb_x1) bb_x1 = x;
    if (z < bb_z0) bb_z0 = z; if (z > bb_z1) bb_z1 = z;
}

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

static void scan_frame(const char *what)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_scd.cut_auto_enabled = 1;
    bb_add(pl->x, pl->z);
    re15_aot_scan(pl->x, pl->z, (uint8_t)g_cut);
    if (g_scd.cam_change_pending) {
        int nc = (int)g_scd.cam_id;
        g_scd.cam_change_pending = 0;
        if (nc != g_cut) {
            if (!g_quiet)
                printf("      F%-4u CUT %d -> %-2d  @(%ld,%ld) y=%ld  [%s]\n",
                       g_frame, g_cut, nc, (long)pl->x, (long)pl->z, (long)pl->y, what);
            g_cut = nc;
        }
    }
    g_frame++;
}

static void march_to(int32_t tx, int32_t tz, int rot)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->rot_y = (int16_t)rot;
    for (int i = 0; i < 4000; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        if (ad <= 40) break;
        int32_t sx = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
        int32_t sz = (dz > 0) ? 1 : (dz < 0 ? -1 : 0);
        int32_t stepx = (dx < 0 ? -dx : dx) > 40 ? sx * 40 : dx;
        int32_t stepz = (dz < 0 ? -dz : dz) > 40 ? sz * 40 : dz;
        pl->x += stepx; pl->z += stepz;
        scan_frame("walk");
    }
    pl->x = tx; pl->z = tz;
    scan_frame("walk-arrive");
}

/* Liste die Zonen auf, die vom aktuellen Cut aus ueberhaupt feuern koennen. */
static void list_live_zones(int cut)
{
    printf("      Zonen mit cam_from=%d:\n", cut);
    for (int i = RE15_AOT_MAX - 1; i >= 0; i--) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;
        if (a->cam_from_filter != (uint8_t)cut) continue;
        printf("         slot%-2d -> cut %-2d  x[%ld..%ld] z[%ld..%ld]\n",
               i, (int)a->event_id,
               (long)(a->x - a->half_w), (long)(a->x + a->half_w),
               (long)(a->z - a->half_h), (long)(a->z + a->half_h));
    }
}

static void bb_verdict(int cut)
{
    printf("      Bahn-BBox x[%ld..%ld] z[%ld..%ld] — Zonen-Ueberlappung:\n",
           (long)bb_x0, (long)bb_x1, (long)bb_z0, (long)bb_z1);
    int any = 0;
    for (int i = RE15_AOT_MAX - 1; i >= 0; i--) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;
        if (a->cam_from_filter != (uint8_t)cut) continue;
        int32_t zx0 = a->x - a->half_w, zx1 = a->x + a->half_w;
        int32_t zz0 = a->z - a->half_h, zz1 = a->z + a->half_h;
        int ov = !(bb_x1 < zx0 || bb_x0 > zx1 || bb_z1 < zz0 || bb_z0 > zz1);
        printf("         slot%-2d -> cut %-2d : %s\n", i, (int)a->event_id,
               ov ? "BBox ueberlappt" : "KEINE Ueberlappung (unerreichbar)");
        any |= ov;
    }
    if (!any) printf("         => vom Start-Cut %d ist auf dieser Bahn KEIN Wechsel moeglich.\n", cut);
}

/* ---------- A/B: echte Port-Treppe, optional ohne Eject ---------- */
static void descend_port(const char *label, int32_t sx, int32_t sz, int rot, int band,
                         int suppress_eject)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)rot;
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    re15_stair_reset();
    printf("   -- %s Trigger @(%ld,%ld) band=%d cut=%d%s\n", label,
           (long)sx, (long)sz, band, g_cut, suppress_eject ? "   [EJECT UNTERDRUECKT]" : "");
    if (!re15_stair_try_start(&g_rdt, 1)) { printf("      KEIN Stair-Start!\n"); return; }
    list_live_zones(g_cut);
    int cut_at_start = g_cut;
    bb_reset(pl->x, pl->z);
    int n = 0;
    int32_t ejx = 0, ejz = 0;
    while (re15_stair_active() && n < 900) {
        int32_t px = pl->x, pz = pl->z;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        if (!re15_stair_active()) {                 /* das war der Finalize-Tick */
            ejx = pl->x - px; ejz = pl->z - pz;
            if (suppress_eject) { pl->x = px; pl->z = pz; }
        }
        scan_frame("stair");
        n++;
    }
    printf("      Ende nach %d Frames @(%ld,%ld) y=%ld cut=%d  (Finalize-Delta d=(%+ld,%+ld))\n",
           n, (long)pl->x, (long)pl->z, (long)pl->y, g_cut, (long)ejx, (long)ejz);
    bb_verdict(cut_at_start);
}

/* ---------- C: Original-Modell (byte-true Gait, kein Eject) ---------- */
static void descend_orig(const char *label, int32_t sx, int32_t sz, int rot, int band, int bands)
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t probe; memset(&probe, 0, sizeof probe);
    probe.active = 1; probe.type = 0;

    pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)rot;
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    printf("   -- %s [ORIGINAL-MODELL] Trigger @(%ld,%ld) band=%d cut=%d\n",
           label, (long)sx, (long)sz, band, g_cut);
    /* Yaw-Endpunkt = wie der Port ihn setzt (mode-0xb-Settle-Endpunkt, @0x800389a8) */
    int yaw = rot;
    probe.rot_y = (int16_t)yaw;
    int32_t cs = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    int clip = 21, flen = g_anim.clips[clip].frame_count;
    int cursor = 0, counter = bands - 1, ticks = 0;
    int32_t prev[16][3]; int have[16];
    memset(prev, 0, sizeof prev); memset(have, 0, sizeof have);
    int32_t savedY = pl->y;
    int cut_at_start = g_cut;
    bb_reset(pl->x, pl->z);
    while (ticks < 900) {
        /* (1) anim_set: Pose = keyframe[cursor], DANN cursor++ */
        int sample = cursor;
        probe.anim_frame = (uint16_t)sample;
        int kf = re15_compute_actor_kf(&g_anim, &g_skel, &probe, clip, (uint32_t)sample);
        g_anim_pose_actor = NULL;
        re15_skel_compute_pose(&g_skel, kf, poses);
        int32_t w[16][3];
        for (int b = 0; b < g_skel.bone_count && b < 16; b++) {
            int32_t lx = poses[b].trans[0], ly = poses[b].trans[1], lz = poses[b].trans[2];
            w[b][0] = (int32_t)(( (int64_t)cs * lx + (int64_t)sn * lz) >> 12);
            w[b][1] = ly;
            w[b][2] = (int32_t)((-(int64_t)sn * lx + (int64_t)cs * lz) >> 12);
        }
        cursor++; if (cursor >= flen) cursor = 0;
        /* (2) FK, Referenz pro Knochen, kein Skip */
        int sel  = ((unsigned)(cursor - 11) < 15u) ? 1 : 0;
        int bone = sel ? 7 : 4;
        if (have[bone]) {
            pl->x -= (w[bone][0] - prev[bone][0]);
            pl->z -= (w[bone][2] - prev[bone][2]);
            pl->y -= (w[bone][1] - prev[bone][1]);
        }
        for (int b = 0; b < g_skel.bone_count && b < 16; b++) {
            prev[b][0] = w[b][0]; prev[b][1] = w[b][1]; prev[b][2] = w[b][2]; have[b] = 1;
        }
        /* (3) bob */
        if (cursor == 11 || cursor == 27) pl->y += 150;
        /* (4) forward 10 */
        if (cursor < 31) {
            pl->x += (int32_t)(((int64_t)cs * 10) >> 12);
            pl->z += (int32_t)((-(int64_t)sn * 10) >> 12);
        }
        ticks++;
        /* (5) band step */
        if (cursor == 32) {
            savedY = savedY + 1800; pl->y = savedY;
            if (counter == 0) { scan_frame("stair-orig"); break; }
            counter--; cursor = 2;
        }
        scan_frame("stair-orig");
    }
    scan_frame("stair-orig-finalize");   /* Finalize-Tick: KEIN XZ-Store */
    printf("      Ende nach %d Frames @(%ld,%ld) y=%ld cut=%d\n",
           ticks + 1, (long)pl->x, (long)pl->z, (long)pl->y, g_cut);
    bb_verdict(cut_at_start);
}

typedef struct { int32_t x, z; int rot, band; const char *label; } step_t;

static const step_t s1060[] = {
    { 25150, 23500, 3072, 8, "Treppe 1 8->6 (ost)"  },
    { 21400, 21500, 3072, 6, "Treppe 2 6->4 (west)" },
    { 25150, 23500, 1024, 4, "Treppe 3 4->2 (ost)"  },
    { 21400, 21500, 3072, 2, "Treppe 4 2->0 (west) = LETZTE" },
    { 23000, 25500,    0, 0, NULL },
};

static void run_chain(const char *base, int mode, int start_cut)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("FAIL: %s\n", path); return; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { printf("FAIL parse\n"); return; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = s1060[0].x; pl->z = s1060[0].z; pl->y = -(int32_t)s1060[0].band * 0x708;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    g_cut = start_cut; g_frame = 0;
    g_scd.cam_id = (uint8_t)start_cut; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;

    printf("\n===== MODUS %s | Start-Cut %d =====\n",
           mode == 0 ? "A: PORT (mit Eject)" :
           mode == 1 ? "B: PORT OHNE EJECT"  : "C: ORIGINAL-MODELL (byte-true Gait, kein Eject)",
           start_cut);
    for (int i = 0; i < 5; i++) {
        if (s1060[i].label) {
            if (i > 0) march_to(s1060[i].x, s1060[i].z, s1060[i].rot);
            if (mode == 2) descend_orig(s1060[i].label, s1060[i].x, s1060[i].z,
                                        s1060[i].rot, s1060[i].band, 2);
            else           descend_port(s1060[i].label, s1060[i].x, s1060[i].z,
                                        s1060[i].rot, s1060[i].band, mode == 1);
        } else {
            march_to(s1060[i].x, s1060[i].z, s1060[i].rot);
            printf("   -- Wegpunkt (%ld,%ld) cut=%d\n", (long)s1060[i].x, (long)s1060[i].z, g_cut);
        }
    }
    printf("   >>> END-CUT = %d\n", g_cut);
    free(data);
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
        printf("PL00 %s\n", g_skel_ok ? "geladen" : "FEHLT");
    }
    for (int m = 0; m < 3; m++) run_chain(base, m, 0);
    /* Gegenprobe: dieselben drei Modi ab Start-Cut 4 (dort schaltet die letzte
     * Treppe laut Vorbefund korrekt 4->5). */
    for (int m = 0; m < 3; m++) run_chain(base, m, 4);
    return 0;
}
