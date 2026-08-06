/* probe_advc_chain1060.c — ADVERSARIALE GEGENPROBE zu Lane C (kein ctest)
 *
 * FRAGE: Der Lane-C-Bericht (LC-8/LC-9) behauptet, die Ursache des Symptoms
 *        liege VOR der letzten Treppe, weil die gemessene Kette am Kopf der
 *        letzten Treppe mit Cut 1 bzw. 3 ankommt statt mit 4.
 *        ABER: die gemessene Kette benutzt SONDEN-Wegpunkte
 *          Ost-Treppe  Trigger (25150,23500)   Rechteck z[19800..24200]
 *          West-Treppe Trigger (21400,21500)   Rechteck z[20700..25000]
 *        Beide liegen NICHT am Kopf des jeweiligen Laufs.
 *
 * KOPF-BESTIMMUNG (byte-true aus dem Record, stair_common.c:374-376 =
 *   LAB_80043500/LAB_800435cc @0x80043510-90):
 *     delta    = z - corner ; low_half = delta < extent>>1
 *     ASCEND <=> (low_half && side==0) || (!low_half && side==1)
 *   Ost  corner=19800 extent=4400 side=0 -> DESCEND nur in der HOHEN Haelfte
 *                                          => Kopf = z ~ 24200 (Abstieg -Z)
 *   West corner=20700 extent=4300 side=1 -> DESCEND nur in der NIEDRIGEN Haelfte
 *                                          => Kopf = z ~ 20700 (Abstieg +Z)
 *   => Wendeltreppe/Switchback: Ost laeuft -Z runter, West +Z runter, die
 *      Uebergaenge liegen am NIEDRIGEN Z-Ende (Ost-Fuss ~20200, West-Kopf ~20700)
 *      und am HOHEN Z-Ende (West-Fuss ~24700, Ost-Kopf ~24200).
 *
 * DIESE SONDE misst die Cut-Kette mit KOPF-Triggern und drei Uebergangs-Pfaden
 * (X-zuerst / Z-zuerst / diagonal), plus zum Vergleich die Sonden-Wegpunkte des
 * Lane-C-Laufs. Wenn die Kopf-Variante 0->1->2->3->4->5 liefert, ist die
 * Lane-C-Kausalaussage ("Ursache liegt vor der letzten Treppe") NICHT belegt,
 * sondern ein Artefakt der Sonden-Wegpunkte.
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

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static re15_rdt_t g_rdt;
static re15_emd_skeleton_t  g_skel;
static re15_emd_animation_t g_anim;
static int g_skel_ok = 0;

static int      g_cut;
static unsigned g_frame;
static int      g_verbose;
static int      g_use_collision;
static char     g_trace[512];

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

static int zone_is_anchor(int i)
{
    return (i == 0 || g_rdt.zones[i - 1].cam_from != g_rdt.zones[i].cam_from);
}

static void scan_frame(const char *what)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_scd.cut_auto_enabled = 1;
    re15_aot_scan(pl->x, pl->z, (uint8_t)g_cut);
    int hit = -1;
    for (int i = 0; i < g_rdt.zone_count; i++) {
        if (zone_is_anchor(i)) continue;
        if ((int)g_rdt.zones[i].cam_from != g_cut) continue;
        if (re15_aot_point_in_quad(pl->x, pl->z, g_rdt.zones[i].xs, g_rdt.zones[i].zs)) { hit = i; break; }
    }
    int newcut = g_cut;
    if (g_scd.cam_change_pending) { newcut = (int)g_scd.cam_id; g_scd.cam_change_pending = 0; }
    if (newcut != g_cut) {
        printf("      F%-4u CUT %d -> %-2d @(%ld,%ld) y=%ld Zone[%d] [%s]\n",
               g_frame, g_cut, newcut, (long)pl->x, (long)pl->z, (long)pl->y, hit, what);
        char b[16]; snprintf(b, sizeof b, "%d", newcut);
        if (strlen(g_trace) + 8 < sizeof g_trace) { strcat(g_trace, "->"); strcat(g_trace, b); }
        g_cut = newcut;
    }
    g_frame++;
}

/* Ein Schritt Lauf. mode: 0=diagonal, 1=X zuerst, 2=Z zuerst. */
static void walk_to(int32_t tx, int32_t tz, int mode)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int i = 0; i < 4000; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        int32_t adx = dx < 0 ? -dx : dx, adz = dz < 0 ? -dz : dz;
        if (adx + adz <= 40) break;
        int32_t nx = pl->x, nz = pl->z;
        int do_x = 1, do_z = 1;
        if (mode == 1 && adx > 40) do_z = 0;      /* X zuerst */
        if (mode == 2 && adz > 40) do_x = 0;      /* Z zuerst */
        if (do_x) nx += (adx > 40) ? ((dx > 0) ? 40 : -40) : dx;
        if (do_z) nz += (adz > 40) ? ((dz > 0) ? 40 : -40) : dz;
        if (g_use_collision) {
            int32_t cx = nx, cz = nz;
            re15_collision_constrain(&g_rdt, nx, nz, &cx, &cz);
            nx = cx; nz = cz;
        }
        if (nx == pl->x && nz == pl->z) break;    /* festgefahren */
        pl->x = nx; pl->z = nz;
        scan_frame("walk");
    }
    if (!g_use_collision) { pl->x = tx; pl->z = tz; }
    scan_frame("walk");
}

static void descend(const char *label, int32_t sx, int32_t sz, int rot, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)rot;
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    re15_stair_reset();
    printf("   -- %s: Trigger @(%ld,%ld) rot=%d band=%d cut=%d cut_auto=%d\n",
           label, (long)sx, (long)sz, rot, band, g_cut, (int)g_scd.cut_auto_enabled);
    if (!re15_stair_try_start(&g_rdt, 1)) { printf("      KEIN Stair-Start!\n"); return; }
    int32_t z0 = pl->z, x0 = pl->x, lx = pl->x, lz = pl->z, maxstep = 0;
    int n = 0;
    while (re15_stair_active() && n < 900) {
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        int32_t s = (pl->x - lx < 0 ? lx - pl->x : pl->x - lx) + (pl->z - lz < 0 ? lz - pl->z : pl->z - lz);
        if (s > maxstep) maxstep = s;
        lx = pl->x; lz = pl->z;
        scan_frame("stair");
        n++;
    }
    printf("      Ende nach %d Frames @(%ld,%ld) y=%ld band=%d cut=%d dz=%+ld dx=%+ld maxstep=%ld\n",
           n, (long)pl->x, (long)pl->z, (long)pl->y, re15_collision_band_from_y(pl->y),
           g_cut, (long)(pl->z - z0), (long)(pl->x - x0), (long)maxstep);
}

typedef struct { int32_t x, z; int rot, band; const char *label; } step_t;

static void setup(const char *base)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("FAIL: %s\n", path); exit(1); }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { printf("FAIL parse\n"); exit(1); }
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();
    free(data);
}

static void run_chain(const char *base, const step_t *steps, int nsteps, int walkmode,
                      int use_coll, const char *title)
{
    setup(base);
    g_cut = 0; g_frame = 0; g_use_collision = use_coll;
    g_trace[0] = 0; strcat(g_trace, "0");
    g_scd.cam_id = 0; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = steps[0].x; pl->z = steps[0].z; pl->y = -(int32_t)steps[0].band * 0x708;

    printf("\n===== %s  (Lauf-Pfad: %s%s) =====\n", title,
           walkmode == 0 ? "diagonal" : (walkmode == 1 ? "X-zuerst" : "Z-zuerst"),
           use_coll ? " +Kollision" : "");
    printf("   cut_auto_enabled nach Raum-Init = %d\n", (int)g_scd.cut_auto_enabled);
    for (int i = 0; i < nsteps; i++) {
        if (steps[i].label) {
            if (i > 0) walk_to(steps[i].x, steps[i].z, walkmode);
            descend(steps[i].label, steps[i].x, steps[i].z, steps[i].rot, steps[i].band);
        } else {
            walk_to(steps[i].x, steps[i].z, walkmode);
            printf("   -- Wegpunkt (%ld,%ld) cut=%d\n", (long)steps[i].x, (long)steps[i].z, g_cut);
        }
    }
    printf("   END-CUT = %d    KETTE: %s\n", g_cut, g_trace);
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    (void)argc; (void)argv;
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

    /* KOPF-Trigger (siehe Kopfkommentar). Ost-Kopf z=24100 (Rechteck bis 24200),
     * West-Kopf z=20800 (Rechteck ab 20700). rot: Ost 1024 = -Z, West 3072 = +Z. */
    static const step_t head[] = {
        { 25150, 24100, 1024, 8, "T1 8->6 OST  (Kopf z=24100, -Z)"  },
        { 21400, 20800, 3072, 6, "T2 6->4 WEST (Kopf z=20800, +Z)" },
        { 25150, 24100, 1024, 4, "T3 4->2 OST  (Kopf z=24100, -Z)"  },
        { 21400, 20800, 3072, 2, "T4 2->0 WEST = LETZTE"           },
        { 23000, 25500,    0, 0, NULL },
    };
    /* Zum Vergleich: die Lane-C-Sonden-Wegpunkte mit korrigierter Richtung. */
    static const step_t lanec[] = {
        { 25150, 23500, 1024, 8, "T1 8->6 OST  (LaneC z=23500)"  },
        { 21400, 21500, 3072, 6, "T2 6->4 WEST (LaneC z=21500)" },
        { 25150, 23500, 1024, 4, "T3 4->2 OST  (LaneC z=23500)"  },
        { 21400, 21500, 3072, 2, "T4 2->0 WEST = LETZTE"        },
        { 23000, 25500,    0, 0, NULL },
    };

    /* EJECT-SWEEP: wie gross ist der Ein-Tick-Sprung des Port-Finalize (stair_common.c:162,
     * 48x100u, ohne @0x-Beleg) ueber den GANZEN legalen Abstiegs-Bereich beider Laeufe?
     * LC-3 hat ihn nur an EINEM Wegpunkt-Satz gemessen (131/207). Zonentiefe = 1000. */
    {
        printf("\n===== EJECT-SWEEP: Ein-Tick-Sprung im Finalize je Trigger-Z =====\n");
        struct { const char *nm; int32_t x; int rot; int32_t z0, z1; int band; } fl[] = {
            { "OST  (x=25150, -Z, descend z>22000)", 25150, 1024, 22100, 24150, 4 },
            { "WEST (x=21400, +Z, descend z<22850)", 21400, 3072, 20750, 22800, 2 },
        };
        for (int f = 0; f < 2; f++) {
            int32_t worst = 0, worst_z = 0, worst_end = 0;
            printf("  %s\n", fl[f].nm);
            for (int32_t tz = fl[f].z0; tz <= fl[f].z1; tz += 50) {
                setup(base);
                g_cut = 0; g_frame = 0; g_use_collision = 0;
                re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                pl->x = fl[f].x; pl->z = tz; pl->rot_y = (int16_t)fl[f].rot;
                pl->y = -(int32_t)fl[f].band * 0x708;
                re15_collision_set_band(fl[f].band);
                re15_stair_reset();
                if (!re15_stair_try_start(&g_rdt, 1)) { printf("    z=%ld KEIN START\n", (long)tz); continue; }
                int32_t lx = pl->x, lz = pl->z, mx = 0; int n = 0;
                while (re15_stair_active() && n < 900) {
                    re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
                    int32_t s = (pl->x > lx ? pl->x - lx : lx - pl->x) + (pl->z > lz ? pl->z - lz : lz - pl->z);
                    if (s > mx) mx = s;
                    lx = pl->x; lz = pl->z; n++;
                }
                if (mx > worst) { worst = mx; worst_z = tz; worst_end = pl->z; }
            }
            printf("    GROESSTER Ein-Tick-Sprung = %ld  bei Trigger-z=%ld  (Landung z=%ld)  "
                   "[Zonentiefe der relevanten RVD-Zonen = 1000]\n",
                   (long)worst, (long)worst_z, (long)worst_end);
        }
    }

    /* DIE ENTSCHEIDENDE MESSUNG fuer den Nutzer-Report: LETZTE Treppe (west, band 2->0),
     * Eingangs-Cut 4 (= der byte-true Zustand laut Kopf-Trigger-Kette), Trigger-Z ueber den
     * GANZEN legalen Abstiegs-Bereich. Schwenkt sie IMMER 4->5 (Zone[13] z[24000..25000])
     * oder gibt es legale Startpunkte, an denen der Finalize-Sprung die Zone UEBERSPRINGT?
     * -> Das ist genau "TEILWEISE schwenkt die Kamera nicht um". */
    {
        printf("\n===== LETZTE TREPPE, Eingangs-Cut 4, Trigger-Z-Sweep =====\n");
        int nfail = 0, ntot = 0;
        for (int32_t tz = 20750; tz <= 22800; tz += 25) {
            setup(base);
            g_cut = 4; g_frame = 0; g_use_collision = 0; g_trace[0] = 0;
            g_scd.cam_id = 4; g_scd.cam_change_pending = 0;
            for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->x = 21400; pl->z = tz; pl->rot_y = 3072;
            pl->y = -(int32_t)2 * 0x708;
            re15_collision_set_band(2);
            re15_stair_reset();
            if (!re15_stair_try_start(&g_rdt, 1)) { printf("  z=%ld KEIN START\n", (long)tz); continue; }
            int32_t lx = pl->x, lz = pl->z, mx = 0, jz0 = 0, jz1 = 0; int n = 0;
            while (re15_stair_active() && n < 900) {
                re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
                int32_t s = (pl->x > lx ? pl->x - lx : lx - pl->x) + (pl->z > lz ? pl->z - lz : lz - pl->z);
                if (s > mx) { mx = s; jz0 = lz; jz1 = pl->z; }
                lx = pl->x; lz = pl->z;
                /* Scan OHNE Log-Rauschen */
                g_scd.cut_auto_enabled = 1;
                re15_aot_scan(pl->x, pl->z, (uint8_t)g_cut);
                if (g_scd.cam_change_pending) { g_cut = (int)g_scd.cam_id; g_scd.cam_change_pending = 0; }
                n++;
            }
            ntot++;
            if (g_cut != 5) {
                nfail++;
                printf("  !! z=%-6ld  END-CUT=%d (KEIN 4->5)  Landung z=%ld  groesster Sprung=%ld "
                       "(z %ld->%ld)\n", (long)tz, g_cut, (long)pl->z, (long)mx, (long)jz0, (long)jz1);
            }
        }
        printf("  -> %d von %d legalen Trigger-Z ohne 4->5-Schwenk\n", nfail, ntot);
    }

    for (int m = 0; m < 3; m++) run_chain(base, head, 5, m, 0, "KOPF-Trigger (byte-true Lauf-Enden)");
    run_chain(base, head, 5, 0, 1, "KOPF-Trigger + echte Kollision");
    for (int m = 0; m < 3; m++) run_chain(base, lanec, 5, m, 0, "LANE-C-Wegpunkte (Vergleich)");
    return 0;
}
