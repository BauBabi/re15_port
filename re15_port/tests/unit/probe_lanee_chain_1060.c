/* probe_lanee_chain_1060.c — LANE E MESSSONDE (kein ctest)
 *
 * FRAGE: bricht die Kamera-Kette im Treppenhaus ROOM1060 auch im ORIGINAL,
 * oder ist der Bruch ein PORT-Artefakt?
 *
 * Diese Sonde misst DREI Dinge, die die vorhandenen Sonden nicht messen:
 *
 *  (1) SEGMENT-SKIP-DETEKTOR.  Der RVD-Scan FUN_80014230 (@0x8001ccec, jeden
 *      Frame, nur durch Bit 0x100 von DAT_800aca3c gesperrt) ist ein PUNKT-Test
 *      (FUN_80014368 @0x80014298) — er sieht nur die ABGETASTETE Position, nie
 *      die Strecke dazwischen.  Springt der Spieler in EINEM Tick ueber einen
 *      1000 Einheiten tiefen RVD-Streifen hinweg, feuert die Zone NIE.
 *      Die Sonde tastet darum jede Frame-Strecke (prev -> cur) in 10er-Schritten
 *      ab und meldet jede Zone, die auf der Strecke lag, an den ENDPUNKTEN aber
 *      nicht gesehen wurde = UEBERSPRUNGEN.
 *
 *  (2) Getrennte Buchfuehrung fuer den GAIT-Anteil und den FINALIZE-Anteil des
 *      Treppen-Ticks.  LAB_80038e50 (@0x80038e50-0x80038eec) hat NACHWEISLICH
 *      KEINEN Store auf player+0x34/+0x38/+0x3c (Disasm: nur 0x800acae8=2,
 *      0x800acae9=0, 0x800acae3=0, anim_set, 0x800aca59/5a/5b=0,
 *      hit_react&=0xfe, player.floor = -DAT_800acc0e/0x708).  Jeder XZ-Versatz
 *      im Finalize ist also PORT-EIGEN (stair_common.c:162ff, bis 48x100u).
 *
 *  (3) Die Kette mit den RICHTIGEN Blickrichtungen.  Die Richtung ergibt sich
 *      aus dem Record selbst (LAB_800435cc, sce 13 = Z-Achse):
 *        delta = playerZ - corner ; low_half = delta < extent>>1
 *        ASCEND <=> (low_half && side==0) || (!low_half && side==1)
 *      Ost-Treppen (slot 3/4/7/8): corner=19800 extent=4400 side=0
 *        -> obere Haelfte (z>22000) = ABSTIEG, und der Abstieg laeuft nach -z.
 *      West-Treppen (slot 5/6/9/10): corner=20700 extent=4300 side=1
 *        -> untere Haelfte (z<22850) = ABSTIEG, Abstieg laeuft nach +z.
 *      probe_stair_cam_chain faehrt Treppe 1 (Ost) mit rot=3072 = +z, also
 *      RUECKWAERTS die Treppe hinauf-hinaus (Ende z=29418, ausserhalb des
 *      Raum-Quads z<=29000).  Das ist ein SONDEN-Fehler, kein Port-Fehler —
 *      diese Sonde faehrt beide Varianten und stellt sie gegenueber.
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
static int g_cut = 0;
static unsigned g_frame = 0;
static int32_t g_px = 0, g_pz = 0;      /* Position beim letzten Scan */
static int g_skips = 0;
static int g_total_skips = 0;

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

/* Punkt-in-Quad wie re15_aot_point_in_quad (Port) — hier nur zur Strecken-
 * Abtastung, der echte Scan laeuft weiter ueber re15_aot_scan. */
static int in_zone(const re15_rdt_zone_t *z, int32_t x, int32_t zz)
{
    return re15_aot_point_in_quad(x, zz, z->xs, z->zs);
}

/* Tastet die Strecke (g_px,g_pz) -> (nx,nz) in 10er-Schritten ab und meldet
 * jede Zone mit cam_from == g_cut, die UNTERWEGS getroffen, an BEIDEN
 * Endpunkten aber verfehlt wird. */
static void segment_check(int32_t nx, int32_t nz, const char *what)
{
    int32_t dx = nx - g_px, dz = nz - g_pz;
    int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
    if (ad <= 10) return;
    int steps = (int)(ad / 10) + 1;
    if (steps > 4000) steps = 4000;
    for (int i = 0; i < g_rdt.zone_count; i++) {
        const re15_rdt_zone_t *z = &g_rdt.zones[i];
        if ((int)z->cam_from != g_cut) continue;
        if (in_zone(z, g_px, g_pz) || in_zone(z, nx, nz)) continue;  /* an einem Endpunkt sichtbar */
        for (int s = 1; s < steps; s++) {
            int32_t sx = g_px + (int32_t)(((int64_t)dx * s) / steps);
            int32_t sz = g_pz + (int32_t)(((int64_t)dz * s) / steps);
            if (in_zone(z, sx, sz)) {
                printf("      !! F%-4u UEBERSPRUNGEN: RVD[%d] %d->%d  Strecke (%ld,%ld)->(%ld,%ld) len=%ld  [%s]\n",
                       g_frame, i, (int)z->cam_from, (int)z->cam_to,
                       (long)g_px, (long)g_pz, (long)nx, (long)nz, (long)ad, what);
                g_skips++; g_total_skips++;
                break;
            }
        }
    }
}

static void scan_frame(const char *what)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    segment_check(pl->x, pl->z, what);
    g_px = pl->x; g_pz = pl->z;
    g_scd.cut_auto_enabled = 1;
    re15_aot_scan(pl->x, pl->z, (uint8_t)g_cut);
    if (g_scd.cam_change_pending) {
        int nc = (int)g_scd.cam_id;
        g_scd.cam_change_pending = 0;
        if (nc != g_cut) {
            printf("      F%-4u CUT %d -> %-2d  @(%ld,%ld) y=%ld  [%s]\n",
                   g_frame, g_cut, nc, (long)pl->x, (long)pl->z, (long)pl->y, what);
            g_cut = nc;
        }
    }
    g_frame++;
}

/* Marsch mit ECHTER Kollision (re15_collision_constrain) — Manhattan, 40 u/Frame. */
static void march_to(int32_t tx, int32_t tz, int use_collision)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int i = 0; i < 4000; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        if (ad <= 40) break;
        int32_t sx = (dx > 40) ? 40 : ((dx < -40) ? -40 : dx);
        int32_t sz = (dz > 40) ? 40 : ((dz < -40) ? -40 : dz);
        int32_t wx = pl->x + sx, wz = pl->z + sz;
        if (use_collision) {
            int32_t cx = wx, cz = wz;
            re15_collision_constrain(&g_rdt, wx, wz, &cx, &cz);
            wx = cx; wz = cz;
        }
        if (wx == pl->x && wz == pl->z) break;   /* festgefahren */
        pl->x = wx; pl->z = wz;
        scan_frame("walk");
    }
    scan_frame("walk-arrive");
}

static void descend(const char *label, int32_t sx, int32_t sz, int rot, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)rot;
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    re15_stair_reset();
    g_px = pl->x; g_pz = pl->z;
    g_skips = 0;
    printf("   -- %s: Start @(%ld,%ld) band=%d rot=%d cut=%d\n",
           label, (long)sx, (long)sz, band, rot, g_cut);
    if (!re15_stair_try_start(&g_rdt, 1)) { printf("      KEIN Stair-Start!\n"); return; }
    printf("      Richtung=%s\n", pl->motion == (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN ? "AB" : "AUF");
    int n = 0;
    int32_t gx = pl->x, gz = pl->z;      /* Position VOR dem letzten (Finalize-)Tick */
    int32_t maxstep = 0;
    while (re15_stair_active() && n < 900) {
        int32_t bx = pl->x, bz = pl->z;
        gx = bx; gz = bz;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        int32_t st = (pl->x - bx < 0 ? bx - pl->x : pl->x - bx) +
                     (pl->z - bz < 0 ? bz - pl->z : pl->z - bz);
        if (st > maxstep) maxstep = st;
        scan_frame("stair");
        n++;
    }
    int32_t ejx = pl->x - gx, ejz = pl->z - gz;
    printf("      Ende nach %d Frames @(%ld,%ld) y=%ld band=%d cut=%d\n"
           "         GAIT-Ende (vor Finalize) = (%ld,%ld)   FINALIZE-VERSATZ = (%+ld,%+ld) |%ld|\n"
           "         groesster Ein-Tick-Schritt = %ld   (RVD-Streifen in ROOM1060 sind 1000 tief)\n"
           "         uebersprungene Zonen in dieser Treppe: %d\n",
           n, (long)pl->x, (long)pl->z, (long)pl->y,
           re15_collision_band_from_y(pl->y), g_cut,
           (long)gx, (long)gz, (long)ejx, (long)ejz,
           (long)((ejx<0?-ejx:ejx)+(ejz<0?-ejz:ejz)), (long)maxstep, g_skips);
}

typedef struct { int32_t x, z; int rot, band; const char *label; } step_t;

static void run_chain(const char *title, const step_t *steps, int nsteps,
                      int start_cut, int use_collision)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("\n=================== %s ===================\n", title);
    g_cut = start_cut; g_frame = 0; g_total_skips = 0;
    g_scd.cam_id = (uint8_t)start_cut; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = steps[0].x; pl->z = steps[0].z; pl->y = -(int32_t)steps[0].band * 0x708;
    g_px = pl->x; g_pz = pl->z;
    for (int i = 0; i < nsteps; i++) {
        if (steps[i].label) {
            if (i > 0) march_to(steps[i].x, steps[i].z, use_collision);
            descend(steps[i].label, steps[i].x, steps[i].z, steps[i].rot, steps[i].band);
        } else {
            march_to(steps[i].x, steps[i].z, use_collision);
            printf("   -- Wegpunkt (%ld,%ld) erreicht @(%ld,%ld) cut=%d\n",
                   (long)steps[i].x, (long)steps[i].z, (long)pl->x, (long)pl->z, g_cut);
        }
    }
    printf("\n   END-CUT = %d @(%ld,%ld) band=%d   uebersprungene Zonen GESAMT = %d\n",
           g_cut, (long)pl->x, (long)pl->z, re15_collision_band_from_y(pl->y), g_total_skips);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t size = 0;
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
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s\n", path); return 1; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { fprintf(stderr, "FAIL parse\n"); return 1; }

    printf("\nRVD von ROOM1060 (%d Zonen; RVD[0] je Gruppe = Anker, FUN_80014230\n"
           "startet @0x80014268 bei Record 1):\n", g_rdt.zone_count);
    for (int i = 0; i < g_rdt.zone_count; i++) {
        const re15_rdt_zone_t *z = &g_rdt.zones[i];
        int32_t mnx = z->xs[0], mxx = z->xs[0], mnz = z->zs[0], mxz = z->zs[0];
        for (int k = 1; k < 4; k++) {
            if (z->xs[k] < mnx) mnx = z->xs[k];
            if (z->xs[k] > mxx) mxx = z->xs[k];
            if (z->zs[k] < mnz) mnz = z->zs[k];
            if (z->zs[k] > mxz) mxz = z->zs[k];
        }
        printf("  [%2d] from=%-2d to=%-2d floor=0x%02x  x[%6ld..%6ld] z[%6ld..%6ld]  dz=%ld\n",
               i, (int)z->cam_from, (int)z->cam_to, (int)z->floor,
               (long)mnx, (long)mxx, (long)mnz, (long)mxz, (long)(mxz - mnz));
    }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 26000; pl->y = -8 * 0x708; pl->z = 25300;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    /* --- A: die Kette wie probe_stair_cam_chain sie faehrt (Treppe 1 rot=3072) --- */
    static const step_t s_alt[] = {
        { 25150, 23500, 3072, 8, "Treppe 1 Ost 8->6  rot=3072 (wie probe_stair_cam_chain)" },
        { 21400, 21500, 3072, 6, "Treppe 2 West 6->4 rot=3072" },
        { 25150, 23500, 1024, 4, "Treppe 3 Ost 4->2  rot=1024" },
        { 21400, 21500, 3072, 2, "Treppe 4 West 2->0 rot=3072 = LETZTE" },
        { 23000, 25500,    0, 0, NULL },
    };
    run_chain("A) Kette MIT der Richtung aus probe_stair_cam_chain (Treppe 1 = +z)",
              s_alt, 5, 0, 0);

    /* --- B: die Kette mit den record-konformen Richtungen --- */
    static const step_t s_ok[] = {
        { 25150, 23500, 1024, 8, "Treppe 1 Ost 8->6  rot=1024 (-z, record-konform)" },
        { 21400, 21500, 3072, 6, "Treppe 2 West 6->4 rot=3072 (+z)" },
        { 25150, 23500, 1024, 4, "Treppe 3 Ost 4->2  rot=1024 (-z)" },
        { 21400, 21500, 3072, 2, "Treppe 4 West 2->0 rot=3072 (+z) = LETZTE" },
        { 26600, 25400,    0, 0, NULL },
    };
    run_chain("B) Kette mit RECORD-KONFORMEN Richtungen (LAB_800435cc)",
              s_ok, 5, 0, 0);

    /* --- C: dieselbe Kette, aber der Marsch zwischen den Treppen mit echter
     *        Kollision (re15_collision_constrain) --- */
    run_chain("C) wie B, Marsch mit echter Kollision", s_ok, 5, 0, 1);

    /* --- D: die Kette an den TATSAECHLICH begehbaren Ausloese-Raendern
     *        (probe_lanee_reach_1060 / probe_lanee_margin_1060:
     *         Band 8/4 Ost-Rand z=24550, Band 6/2 West-Rand z=20340) --- */
    static const step_t s_edge[] = {
        { 25150, 24550, 1024, 8, "Treppe 1 Ost 8->6  @Rand z=24550" },
        { 21400, 20340, 3072, 6, "Treppe 2 West 6->4 @Rand z=20340" },
        { 25150, 24550, 1024, 4, "Treppe 3 Ost 4->2  @Rand z=24550" },
        { 21400, 20340, 3072, 2, "Treppe 4 West 2->0 @Rand z=20340 = LETZTE" },
        { 26600, 25400,    0, 0, NULL },
    };
    run_chain("D) Kette an den begehbaren Raendern (Spieler laeuft in die Wand, dann SQUARE)",
              s_edge, 5, 0, 1);

    /* --- E: DER SYMPTOM-REPRO. Identisch zu D, nur wird die LETZTE Treppe
     *        200 Einheiten frueher ausgeloest (z=20140 statt 20340) — voellig
     *        legal: der 620-Sondenpunkt (`ori v0,zero,0x26c` @0x80042bd0) faellt
     *        ab z=20080 ins West-Rechteck z[20700..25000], und z=20140 liegt
     *        mitten auf der begehbaren Band-2-Flaeche.
     *        Der Port-Gait endet dann bei z=23901, also 99 Einheiten VOR
     *        RVD[13] z[24000..25000], und der PORT-EIGENE Finalize-Versatz
     *        (stair_common.c, 48 x 100u) springt in EINEM Tick auf z=25368 —
     *        ueber den ganzen 1000 Einheiten tiefen Streifen hinweg.
     *        Das Original kann das nicht: LAB_80038e50 (@0x80038e50-0x80038eec)
     *        hat keinen Store auf player+0x34/+0x38/+0x3c. --- */
    static const step_t s_sym[] = {
        { 25150, 24550, 1024, 8, "Treppe 1 Ost 8->6  @Rand z=24550" },
        { 21400, 20340, 3072, 6, "Treppe 2 West 6->4 @Rand z=20340" },
        { 25150, 24550, 1024, 4, "Treppe 3 Ost 4->2  @Rand z=24550" },
        { 21400, 20140, 3072, 2, "Treppe 4 West 2->0 @z=20140 = LETZTE (SYMPTOM)" },
        { 26600, 25400,    0, 0, NULL },
    };
    run_chain("E) SYMPTOM-REPRO: letzte Treppe 200u frueher ausgeloest",
              s_sym, 5, 0, 1);

    /* --- F: derselbe Bruch eine Treppe FRUEHER (Treppe 3 bei z=24790):
     *        dann bleibt der Cut auf 3 und die LETZTE Treppe kann RVD[13]
     *        (cam_from=4) gar nicht mehr ausloesen — auch wenn sie perfekt
     *        gefahren wird. --- */
    static const step_t s_sym2[] = {
        { 25150, 24550, 1024, 8, "Treppe 1 Ost 8->6  @Rand z=24550" },
        { 21400, 20340, 3072, 6, "Treppe 2 West 6->4 @Rand z=20340" },
        { 25150, 24790, 1024, 4, "Treppe 3 Ost 4->2  @z=24790 (SYMPTOM-Quelle)" },
        { 21400, 20340, 3072, 2, "Treppe 4 West 2->0 @Rand z=20340 = LETZTE (perfekt)" },
        { 26600, 25400,    0, 0, NULL },
    };
    run_chain("F) Bruch schon bei Treppe 3 -> LETZTE Treppe chancenlos",
              s_sym2, 5, 0, 1);

    return 0;
}
