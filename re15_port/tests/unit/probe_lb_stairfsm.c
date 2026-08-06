/* probe_lb_stairfsm.c — LANE B DIAGNOSE (kein ctest).
 *
 * Misst die PORT-Treppen-Maschine (re15_stair_try_start/_tick) FRAME FUER FRAME
 * und zerlegt die XZ-Bewegung in ihre Beitraege, damit sie gegen das
 * disassemblierte Original (LAB_80038c60 / LAB_80038e50) geprueft werden kann:
 *
 *   - Frame-Zahl der Gait-Phase + der Finalize-Tick
 *   - dz/dx PRO Frame (Sprung-Erkennung: |d| > 60 = kein Gait-Schritt mehr)
 *   - Beitrag OHNE Skelett (skel=NULL) = nur der Vorwaerts-10 (FUN_800245d8)
 *   - Beitrag MIT Skelett = Vorwaerts-10 + FK-Foot-Lock (FUN_800390e0)
 *   - die Verschiebung im LETZTEN (Finalize-)Tick isoliert = der Port-EJECT
 *
 * Original-Referenz (disassembliert, Adressen im Report):
 *   Vorwaerts-Schritt = DAT_800acae0 = 10 Einheiten, nur wenn cursor < 31
 *   (`sltiu v0,v0,0x1f` @0x80038db8 -> jal FUN_800245d8 @0x80038dc4).
 *   Ein 2-Band-Abstieg = 32 Gait-Ticks (cursor 0->32) + 30 (cursor 2->32)
 *   + 1 Finalize-Tick = 63 Ticks; Vorwaerts-Ticks = 30 + 28 = 58 -> 580 Einheiten.
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

static void run_stair(const char *label, int32_t sx, int32_t sz, int rot, int band,
                      int use_skel, int verbose)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)rot;
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    re15_stair_reset();
    if (!re15_stair_try_start(&g_rdt, 1)) { printf("   %-28s KEIN Stair-Start\n", label); return; }

    int32_t x0 = pl->x, z0 = pl->z;
    int32_t px = pl->x, pz = pl->z, py = pl->y;
    int32_t last_x = 0, last_z = 0;      /* Delta des LETZTEN Ticks (= Finalize) */
    int32_t gait_x = 0, gait_z = 0;      /* Summe ohne den letzten Tick          */
    int32_t maxstep = 0;
    int n = 0, jumps = 0;
    int rot_used = pl->rot_y;
    printf("   %-28s start=(%ld,%ld) rot=%d->%d %s FK=%s\n", label,
           (long)sx, (long)sz, rot, rot_used,
           pl->motion == (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN ? "DOWN" : "UP",
           use_skel ? "an" : "AUS");
    while (re15_stair_active() && n < 900) {
        gait_x += last_x; gait_z += last_z;
        re15_stair_tick(&g_rdt, (use_skel && g_skel_ok) ? &g_skel : NULL,
                                (use_skel && g_skel_ok) ? &g_anim : NULL);
        last_x = pl->x - px; last_z = pl->z - pz;
        int32_t step = (last_x < 0 ? -last_x : last_x) + (last_z < 0 ? -last_z : last_z);
        if (step > maxstep) maxstep = step;
        if (step > 60) jumps++;
        if (verbose)
            printf("        f%-3d d=(%+5ld,%+5ld) pos=(%7ld,%7ld) y=%7ld%s\n",
                   n, (long)last_x, (long)last_z, (long)pl->x, (long)pl->z, (long)pl->y,
                   step > 60 ? "   <== SPRUNG" : "");
        px = pl->x; pz = pl->z; py = pl->y;
        n++;
    }
    {   /* Landung VOR dem Eject = Gait-Endpunkt: steckt der ueberhaupt in einer Wand? */
        int32_t gx = pl->x - last_x, gz = pl->z - last_z, cx, cz;
        cx = gx; cz = gz;
        re15_collision_constrain(&g_rdt, gx, gz, &cx, &cz);
        printf("        -> %d Ticks | GAIT d=(%+ld,%+ld) | LETZTER TICK (Finalize/EJECT) d=(%+ld,%+ld)"
               " | gesamt d=(%+ld,%+ld) | maxstep=%ld sprünge=%d | Gait-Landung (%ld,%ld) %s (push %+ld,%+ld)\n",
               n, (long)gait_x, (long)gait_z, (long)last_x, (long)last_z,
               (long)(pl->x - x0), (long)(pl->z - z0), (long)maxstep, jumps,
               (long)gx, (long)gz, (cx == gx && cz == gz) ? "FREI" : "STECKT",
               (long)(cx - gx), (long)(cz - gz));
    }
    (void)py;
}

/* ------------------------------------------------------------------ *
 * ORIGINAL-MODELL (byte-true nachgebaut aus LAB_80038c60 / LAB_80038850)
 *
 *   tick:  anim_set(FUN_8001f314 @0x80038d20)  -> Pose = keyframe[cursor]
 *                                                cursor++ (FUN_8001f3bc:89-90),
 *                                                Wrap bei clip.frame_count
 *          FUN_800390e0(0,sel) @0x80038d50     -> p -= (footWorld(pose) - bone[+0x54])
 *                     sel = (unsigned)(cursor-11) < 15   @0x80038d34-3c   (descend)
 *                     bone = sel ? 7 : 4  (Kette 0-1-2-3-4 / 0-1-5-6-7 @0x80039148-88)
 *          bob   @0x80038d68/0x80038d90        -> cursor==11 / ==27 : Y += 150
 *          fwd   @0x80038db8-c8                -> cursor < 31 : FUN_800245d8 (Speed 10)
 *          band  @0x80038de4-e3c               -> cursor == 32 : Y = savedY+1800,
 *                                                 acaf2--, cursor=2 bzw. Phase 2
 *
 * bone[+0x54..0x5c] = t[] der WELT-MATRIX des Knochens (MATRIX +0x40, t bei +0x14),
 * die der Renderer JEDEN Frame fuer JEDEN Knochen neu schreibt -> die Referenz ist
 * die Pose des VORIGEN Ticks, PRO KNOCHEN. Der Spieler-Anteil hebt sich weg
 * (RotMatrix(player+0x68 -> player+0x20) hat t[] == player X/Y/Z @0x34/38/3c),
 * also ist die Drift rein lokal und offline berechenbar.
 * ------------------------------------------------------------------ */
/* mode-Bits: 1 = Pose NACH dem cursor++ abtasten (PORT-Reihenfolge),
 *            2 = FK-Delta beim Fuss-Wechsel ueberspringen (PORT),
 *            4 = EINE gemeinsame Referenz + Reset beim Band-Loop (PORT). */
static void run_original_model(const char *label, int32_t sx, int32_t sz, int yaw,
                               int bands, int verbose, int mode)
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    re15_actor_t probe;
    memset(&probe, 0, sizeof probe);
    probe.active = 1; probe.type = 0;
    probe.rot_y = (int16_t)yaw;
    int32_t x = sx, z = sz;
    int32_t cs = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    int clip = 21;
    int flen = g_anim.clips[clip].frame_count;
    int cursor = 0, counter = bands - 1, ticks = 0, prev_sel = -1;
    int32_t prev[16][3]; int have[16];
    memset(prev, 0, sizeof prev); memset(have, 0, sizeof have);
    int32_t x0 = x, z0 = z;
    int32_t maxstep = 0;
    while (ticks < 900) {
        int32_t bx = x, bz = z;
        /* (1) anim_set: Pose = keyframe[cursor], DANN cursor++ (mit Wrap) */
        int sample = cursor;
        if (mode & 1) { sample = cursor + 1; if (sample >= flen) sample = 0; }
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
        cursor++;
        if (cursor >= flen) cursor = 0;
        /* (2) FK: KEIN Skip, Referenz PRO KNOCHEN */
        int sel  = ((unsigned)(cursor - 11) < 15u) ? 1 : 0;
        int bone = sel ? 7 : 4;
        int ref  = (mode & 4) ? 0 : bone;              /* Port: EINE Referenz  */
        int ok   = have[ref] && (!(mode & 2) || sel == prev_sel);
        if (ok) { x -= (w[bone][0] - prev[ref][0]); z -= (w[bone][2] - prev[ref][2]); }
        if (mode & 4) { prev[0][0] = w[bone][0]; prev[0][2] = w[bone][2]; have[0] = 1; }
        else for (int b = 0; b < g_skel.bone_count && b < 16; b++) {
            prev[b][0] = w[b][0]; prev[b][1] = w[b][1]; prev[b][2] = w[b][2]; have[b] = 1;
        }
        prev_sel = sel;
        /* (4) forward 10 */
        if (cursor < 31) { x += (int32_t)(((int64_t)cs * 10) >> 12);
                           z += (int32_t)((-(int64_t)sn * 10) >> 12); }
        int32_t step = ((x-bx) < 0 ? -(x-bx) : (x-bx)) + ((z-bz) < 0 ? -(z-bz) : (z-bz));
        if (step > maxstep) maxstep = step;
        if (verbose)
            printf("        f%-3d cur=%-2d sel=%d d=(%+5ld,%+5ld) pos=(%7ld,%7ld)\n",
                   ticks, cursor, sel, (long)(x-bx), (long)(z-bz), (long)x, (long)z);
        ticks++;
        /* (5) band step */
        if (cursor == 32) {
            if (counter == 0) break;          /* -> Phase 2, KEIN weiterer XZ-Store */
            counter--;
            cursor = 2;
            if (mode & 4) { have[0] = 0; prev_sel = -1; }   /* Port: Referenz-Reset */
        }
    }
    int32_t cx = x, cz = z;
    re15_collision_constrain(&g_rdt, x, z, &cx, &cz);
    printf("   %-28s ORIGINAL-MODELL: %d Gait-Ticks + 1 Finalize | d=(%+ld,%+ld) | maxstep=%ld"
           " | Landung (%ld,%ld) -> Kollision %s (push %+ld,%+ld)\n",
           label, ticks, (long)(x - x0), (long)(z - z0), (long)maxstep,
           (long)x, (long)z,
           (cx == x && cz == z) ? "FREI" : "STECKT",
           (long)(cx - x), (long)(cz - z));
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
        printf("PL00 %s (clips=%d bones=%d)\n", g_skel_ok ? "geladen" : "FEHLT",
               g_anim.clip_count, g_skel.bone_count);
        if (g_skel_ok) {
            for (int c = 18; c <= 23 && c < g_anim.clip_count; c++)
                printf("   clip %2d: first=%d frame_count=%d\n",
                       c, g_anim.clips[c].first_frame, g_anim.clips[c].frame_count);
        }
    }

    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("FAIL: %s\n", path); return 1; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { printf("FAIL parse\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 25150; pl->z = 23500; pl->y = -8 * 0x708;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    printf("\n=== ROOM1060 — FK AUS (nur Vorwaerts-10, Original-Erwartung 580u) ===\n");
    run_stair("T1 8->6 ost",  25150, 23500, 3072, 8, 0, 0);
    run_stair("T2 6->4 west", 21400, 21500, 3072, 6, 0, 0);
    run_stair("T3 4->2 ost",  25150, 23500, 1024, 4, 0, 0);
    run_stair("T4 2->0 west", 21400, 21500, 3072, 2, 0, 0);

    printf("\n=== ROOM1060 — FK AN (Port-Ist) ===\n");
    run_stair("T1 8->6 ost",  25150, 23500, 3072, 8, 1, 0);
    run_stair("T2 6->4 west", 21400, 21500, 3072, 6, 1, 0);
    run_stair("T3 4->2 ost",  25150, 23500, 1024, 4, 1, 0);
    run_stair("T4 2->0 west", 21400, 21500, 3072, 2, 1, 0);

    printf("\n=== ROOM1060 — T4 (LETZTE Treppe) FRAME FUER FRAME, FK an ===\n");
    run_stair("T4 2->0 west", 21400, 21500, 3072, 2, 1, 1);

    printf("\n=== ROOM1060 — T1 FRAME FUER FRAME, FK an ===\n");
    run_stair("T1 8->6 ost",  25150, 23500, 3072, 8, 1, 1);

    printf("\n=== ORIGINAL-MODELL (kein Skip beim Fuss-Wechsel, Pose = keyframe[cursor_vorher],"
           " KEIN Eject) ===\n");
    if (g_skel_ok) {
        run_original_model("ORIG  (0) west", 21400, 21500, 3072, 2, 0, 0);
        run_original_model("ORIG  (0) ost ", 25150, 23500, 1024, 2, 0, 0);
        printf("\n   Isolierung der Port-Abweichungen (west):\n");
        run_original_model("nur Pose-Shift   (1)", 21400, 21500, 3072, 2, 0, 1);
        run_original_model("nur Wechsel-Skip (2)", 21400, 21500, 3072, 2, 0, 2);
        run_original_model("nur 1 Referenz   (4)", 21400, 21500, 3072, 2, 0, 4);
        run_original_model("PORT-Nachbau     (7)", 21400, 21500, 3072, 2, 0, 7);
        printf("\n   T4 Frame fuer Frame (Original-Modell):\n");
        run_original_model("T4 2->0 west", 21400, 21500, 3072, 2, 1, 0);
    }
    return 0;
}
