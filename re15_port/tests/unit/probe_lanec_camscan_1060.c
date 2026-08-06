/* probe_lanec_camscan_1060.c — LANE C DIAGNOSE (kein ctest)
 *
 * FRAGE: Laeuft der RVD-/Cut-Zonen-Scan waehrend der Treppen-Animation, und
 *        mit welcher Abtastrate/Phase?
 *
 * ORIGINAL-BEFUND (Disasm, siehe Bericht):
 *   FUN_80014230 hat GENAU EINEN Aufrufer: @0x8001ccec, im Game-Flow-Task
 *   @0x8001c958. Gates: s2 (Debug-Pad-Toggle, DAT_800ac760 & 0x20004
 *   @0x8001ccb4-c0) und DAT_800aca3c & 0x100 @0x8001cce0-e4. KEIN Spieler-
 *   Zustands-Gate. Der Scan laeuft @0x8001ccec VOR der Spieler-FSM
 *   FUN_80031c44 @0x8001ce0c (in der die Treppen-Handler LAB_80038c60 /
 *   LAB_80038850 ueber die Tabelle @0x80073f90[DAT_800aca58] haengen).
 *   -> Der Scan laeuft waehrend der Treppe JEDEN Frame, aber mit der Position
 *      VOR dem Tick dieses Frames.  Der Port scannt NACH dem Tick.
 *
 * DIESE SONDE MISST:
 *   1) pro Frame: Position, dz/dx, Band, angezeigter Cut, ob der Scan lief,
 *      welche Zone getroffen wurde
 *   2) SEGMENT-ORAKEL: wurde zwischen Frame N-1 und Frame N eine Zone mit
 *      cam_from == aktueller Cut UEBERSPRUNGEN (Segment schneidet die Zone,
 *      aber KEIN Abtastpunkt liegt drin)?  Das entscheidet die Finalize-
 *      Sprung-Hypothese.
 *   3) Phasenvergleich: Scan VOR dem Tick (Original-Reihenfolge) vs. NACH dem
 *      Tick (Port-Reihenfolge) — liefert derselbe Lauf denselben End-Cut?
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
static int32_t  g_px, g_pz;        /* Position beim VORIGEN Scan-Abtastpunkt */
static int      g_have_prev;
static int      g_verbose;
static int      g_skips;           /* Zaehler uebersprungener Zonen */

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

/* Ist (x,z) in RVD-Zone i? (dieselbe Quad-Mathematik wie der Port-Scan) */
static int zone_hit(int i, int32_t x, int32_t z)
{
    const re15_rdt_zone_t *zz = &g_rdt.zones[i];
    return re15_aot_point_in_quad(x, z, zz->xs, zz->zs);
}

/* ist Zone i ein ANKER (erste Zone ihrer cam_from-Gruppe)? */
static int zone_is_anchor(int i)
{
    return (i == 0 || g_rdt.zones[i - 1].cam_from != g_rdt.zones[i].cam_from);
}

/* SEGMENT-ORAKEL: feine Abtastung von (x0,z0) nach (x1,z1); meldet jede
 * NICHT-Anker-Zone mit cam_from == cut, die das Segment schneidet, ohne dass
 * einer der beiden ENDPUNKTE drin liegt. */
static void segment_oracle(int32_t x0, int32_t z0, int32_t x1, int32_t z1, int cut,
                           const char *what)
{
    int32_t dx = x1 - x0, dz = z1 - z0;
    int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
    if (ad < 2) return;
    int steps = (int)(ad / 8) + 2;          /* 8-Einheiten-Raster */
    if (steps > 4000) steps = 4000;
    for (int i = 0; i < g_rdt.zone_count; i++) {
        if (zone_is_anchor(i)) continue;
        if ((int)g_rdt.zones[i].cam_from != cut) continue;
        if (zone_hit(i, x0, z0) || zone_hit(i, x1, z1)) continue;  /* Endpunkt drin = nicht verpasst */
        for (int s = 1; s < steps; s++) {
            int32_t sx = x0 + (int32_t)((int64_t)dx * s / steps);
            int32_t sz = z0 + (int32_t)((int64_t)dz * s / steps);
            if (zone_hit(i, sx, sz)) {
                printf("      !! ZONE UEBERSPRUNGEN  F%-4u  Zone[%d] %d->%d  "
                       "Segment (%ld,%ld)->(%ld,%ld) len=%ld  Treffer @(%ld,%ld)  [%s]\n",
                       g_frame, i, (int)g_rdt.zones[i].cam_from, (int)g_rdt.zones[i].cam_to,
                       (long)x0, (long)z0, (long)x1, (long)z1, (long)ad,
                       (long)sx, (long)sz, what);
                g_skips++;
                break;
            }
        }
    }
}

/* Ein Frame Scan wie game_step_common.c:456 (Stair-Zweig). */
static void scan_frame(const char *what)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    if (g_have_prev)
        segment_oracle(g_px, g_pz, pl->x, pl->z, g_cut, what);

    int32_t ddx = g_have_prev ? (pl->x - g_px) : 0;
    int32_t ddz = g_have_prev ? (pl->z - g_pz) : 0;

    g_scd.cut_auto_enabled = 1;
    re15_aot_scan(pl->x, pl->z, (uint8_t)g_cut);
    int hit = -1;
    for (int i = 0; i < g_rdt.zone_count; i++) {
        if (zone_is_anchor(i)) continue;
        if ((int)g_rdt.zones[i].cam_from != g_cut) continue;
        if (zone_hit(i, pl->x, pl->z)) { hit = i; break; }
    }
    int newcut = g_cut;
    if (g_scd.cam_change_pending) {
        newcut = (int)g_scd.cam_id;
        g_scd.cam_change_pending = 0;
    }
    if (g_verbose)
        printf("      F%-4u %-6s pos=(%6ld,%6ld) y=%-7ld d=(%+5ld,%+5ld) band=%d cut=%d "
               "scan=JA zone=%s%d%s\n",
               g_frame, what, (long)pl->x, (long)pl->z, (long)pl->y,
               (long)ddx, (long)ddz, re15_collision_debug_band(), g_cut,
               hit >= 0 ? "[" : "", hit, hit >= 0 ? "]" : "");
    if (newcut != g_cut) {
        printf("      F%-4u CUT %d -> %-2d  @(%ld,%ld) y=%ld  Zone[%d]  [%s]\n",
               g_frame, g_cut, newcut, (long)pl->x, (long)pl->z, (long)pl->y, hit, what);
        g_cut = newcut;
    }
    g_px = pl->x; g_pz = pl->z; g_have_prev = 1;
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
        pl->x += ((dx < 0 ? -dx : dx) > 40) ? sx * 40 : dx;
        pl->z += ((dz < 0 ? -dz : dz) > 40) ? sz * 40 : dz;
        scan_frame("walk");
    }
    pl->x = tx; pl->z = tz;
    scan_frame("walk");
}

/* order: 0 = Port (Tick, dann Scan) ; 1 = Original (Scan, dann Tick) */
static void descend(const char *label, int32_t sx, int32_t sz, int rot, int band, int order)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)rot;
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    re15_stair_reset();
    g_have_prev = 0;   /* Teleport an den Trigger: kein Segment-Orakel darueber */
    printf("   -- %s: Trigger @(%ld,%ld) rot=%d band=%d cut=%d\n",
           label, (long)sx, (long)sz, rot, band, g_cut);
    if (!re15_stair_try_start(&g_rdt, 1)) { printf("      KEIN Stair-Start!\n"); return; }
    int32_t z0 = pl->z, x0 = pl->x;
    int n = 0;
    int32_t last_x = pl->x, last_z = pl->z;
    int32_t maxstep = 0; unsigned maxstep_f = 0;
    while (re15_stair_active() && n < 900) {
        if (order) { scan_frame("stair"); re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL,
                                                          g_skel_ok ? &g_anim : NULL); }
        else       { re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL,
                                     g_skel_ok ? &g_anim : NULL); scan_frame("stair"); }
        int32_t st = (pl->x - last_x); if (st < 0) st = -st;
        int32_t st2 = (pl->z - last_z); if (st2 < 0) st2 = -st2;
        if (st + st2 > maxstep) { maxstep = st + st2; maxstep_f = g_frame - 1; }
        last_x = pl->x; last_z = pl->z;
        n++;
    }
    if (order) scan_frame("stair-post");   /* der Frame NACH dem letzten Tick */
    printf("      Ende nach %d Frames @(%ld,%ld) y=%ld band=%d cut=%d  dz=%+ld dx=%+ld"
           "  GROESSTER Ein-Frame-Sprung=%ld @F%u\n",
           n, (long)pl->x, (long)pl->z, (long)pl->y,
           re15_collision_band_from_y(pl->y), g_cut,
           (long)(pl->z - z0), (long)(pl->x - x0), (long)maxstep, maxstep_f);
}

typedef struct { int32_t x, z; int rot, band; const char *label; } step_t;

static void run(const char *base, const step_t *steps, int nsteps, int start_cut,
                int order, int verbose, const char *title)
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
    pl->x = steps[0].x; pl->z = steps[0].z; pl->y = -(int32_t)steps[0].band * 0x708;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    g_cut = start_cut; g_frame = 0; g_have_prev = 0; g_skips = 0; g_verbose = verbose;
    g_scd.cam_id = (uint8_t)start_cut; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;

    printf("\n===== %s  (Reihenfolge: %s, Start-Cut=%d) =====\n",
           title, order ? "SCAN-DANN-TICK (Original)" : "TICK-DANN-SCAN (Port)", start_cut);
    for (int i = 0; i < nsteps; i++) {
        if (steps[i].label) {
            if (i > 0) march_to(steps[i].x, steps[i].z, steps[i].rot);
            descend(steps[i].label, steps[i].x, steps[i].z, steps[i].rot, steps[i].band, order);
        } else {
            march_to(steps[i].x, steps[i].z, steps[i].rot);
            printf("   -- Wegpunkt (%ld,%ld) cut=%d\n", (long)steps[i].x, (long)steps[i].z, g_cut);
        }
    }
    printf("   END-CUT = %d   uebersprungene Zonen = %d\n", g_cut, g_skips);
    free(data);
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    int verbose = (argc > 1 && strcmp(argv[1], "-v") == 0);
    {
        char p2[600]; size_t s1 = 0, s2 = 0;
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EDD", base);
        uint8_t *edd = read_file(p2, &s1);
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EMR", base);
        uint8_t *emr = read_file(p2, &s2);
        if (edd && emr && re15_emd_parse_animation(edd, s1, &g_anim) == 0 &&
            re15_emd_parse_skeleton(emr, s2, &g_skel) == 0) g_skel_ok = 1;
        printf("PL00 %s\n", g_skel_ok ? "geladen" : "FEHLT (FK-Foot-Lock inaktiv!)");
    }

    /* ROOM1060 RVD-Tabelle (Anker markiert) */
    {
        char path[600]; size_t size = 0;
        snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
        uint8_t *d = read_file(path, &size);
        if (d && re15_rdt_parse(d, size, &g_rdt) == 0) {
            printf("\nROOM1060 RVD (%d Zonen):\n", g_rdt.zone_count);
            for (int i = 0; i < g_rdt.zone_count; i++) {
                const re15_rdt_zone_t *z = &g_rdt.zones[i];
                printf("  [%2d]%s from=%-2d to=%-2d floor=0x%02x x[%d..%d] z[%d..%d]\n",
                       i, zone_is_anchor(i) ? " ANKER" : "      ",
                       (int)z->cam_from, (int)z->cam_to, (int)z->floor,
                       (int)(z->cx - z->half_w), (int)(z->cx + z->half_w),
                       (int)(z->cz - z->half_h), (int)(z->cz + z->half_h));
            }
        }
        free(d);
    }

    /* Die Kette wie probe_stair_cam_chain (unveraendert uebernommen). */
    static const step_t chain_asis[] = {
        { 25150, 23500, 3072, 8, "Treppe 1 8->6 (ost)"  },
        { 21400, 21500, 3072, 6, "Treppe 2 6->4 (west)" },
        { 25150, 23500, 1024, 4, "Treppe 3 4->2 (ost)"  },
        { 21400, 21500, 3072, 2, "Treppe 4 2->0 (west) = LETZTE" },
        { 23000, 25500,    0, 0, NULL },
    };
    /* Dieselbe Kette, aber Treppe 1 mit derselben Blickrichtung wie Treppe 3
     * (beide sind DIESELBE Ost-Treppe: rot=1024 = -Z = die Laufrichtung, die
     * das Rechteck z[19800..24200] von seinem HOCH-Z-Ende her durchlaeuft). */
    static const step_t chain_fixdir[] = {
        { 25150, 23500, 1024, 8, "Treppe 1 8->6 (ost)"  },
        { 21400, 21500, 3072, 6, "Treppe 2 6->4 (west)" },
        { 25150, 23500, 1024, 4, "Treppe 3 4->2 (ost)"  },
        { 21400, 21500, 3072, 2, "Treppe 4 2->0 (west) = LETZTE" },
        { 23000, 25500,    0, 0, NULL },
    };

    /* SWEEP: mit welchem EINGANGS-Cut schwenkt die LETZTE Treppe ueberhaupt?
     * (quantifiziert das "TEILWEISE" des Nutzer-Reports) */
    {
        static const step_t last_only[] = {
            { 21400, 21500, 3072, 2, "LETZTE Treppe 2->0 (west)" },
        };
        printf("\n===== SWEEP: LETZTE Treppe (west, band 2->0) je Eingangs-Cut =====\n");
        for (int c0 = 0; c0 < 8; c0++)
            run(base, last_only, 1, c0, 0, 0, "SWEEP letzte Treppe");
    }

    run(base, chain_asis,  5, 0, 0, verbose, "KETTE wie probe_stair_cam_chain");
    run(base, chain_asis,  5, 0, 1, verbose, "KETTE wie probe_stair_cam_chain");
    run(base, chain_fixdir,5, 0, 0, verbose, "KETTE, Treppe1 Blickrichtung wie Treppe3");
    run(base, chain_fixdir,5, 0, 1, verbose, "KETTE, Treppe1 Blickrichtung wie Treppe3");
    return 0;
}
