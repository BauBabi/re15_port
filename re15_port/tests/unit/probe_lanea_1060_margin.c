/* probe_lanea_1060_margin.c — LANE A / Teil 2 (Diagnose, KEIN ctest)
 *
 * ROOM1060, Nutzer-Report "Kamera schwenkt nach der letzten Treppe TEILWEISE
 * nicht um".  Teil 1 (probe_lanea_1060_zonepath) hat gezeigt: bei EINEM
 * bestimmten Startpunkt entsteht die volle Soll-Kette 0->1->2->3->4->5->7.
 * Diese Sonde misst die MARGE: ueber welchen Bereich von Ausloese-Positionen
 * feuert die zustaendige Zone noch — und ab wo nicht mehr?
 *
 * Gemessen wird pro Ausloese-z:
 *   - der reine Gait-Weg (z vor dem Finalize-Tick)
 *   - der Finalize-Sprung (stair_common.c:162, bis 48x100u in EINEM Tick)
 *   - ob das zustaendige RVD-Rechteck in irgendeinem gescannten Frame getroffen
 *     wurde (echter re15_aot_scan) und ob der Cut wirklich umgeschaltet hat.
 * Zusaetzlich: welche z-Werte sind ueberhaupt LEGAL (SCA-begehbar) und erfuellen
 * den byte-true 620-Sondentest (@0x80042bd0) fuer den jeweiligen Treppen-Record.
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

/* SCA-Modell (re15_collision.c Kopf): Zellen mit band == player-band sind WAENDE,
 * der Spieler laeuft in deren KOMPLEMENT.  re15_collision_on_floor()==1 heisst also
 * "im Wand-Zellen-Inneren" — NICHT begehbar.  Zusaetzlich schiebt der Resolver mit
 * Radius PR=450 (+0x12 Skin) aus der Zelle heraus, d.h. die real erreichbare Grenze
 * liegt 468 vor der Zellkante.  Hier: die BEGEHBARE Menge = !on_floor, und der per
 * echtem constrain erreichbare Extrempunkt. */
static void legal_range(int32_t x, int band, int32_t z_lo, int32_t z_hi)
{
    re15_collision_set_band(band);
    printf("  band=%d, x=%ld : WAND-Zellen (on_floor==1) bei z", band, (long)x);
    int32_t run_start = -1, prev = -1;
    for (int32_t z = z_lo; z <= z_hi; z += 5) {
        int ok = re15_collision_on_floor(&g_rdt, x, z);
        if (ok && run_start < 0) run_start = z;
        if (!ok && run_start >= 0) { printf(" [%ld..%ld]", (long)run_start, (long)prev); run_start = -1; }
        if (ok) prev = z;
    }
    if (run_start >= 0) printf(" [%ld..%ld]", (long)run_start, (long)prev);
    printf("\n");
}

/* Wie weit kommt der Spieler mit dem ECHTEN Resolver an die Treppe heran? */
static void reach_limit(int32_t x, int band, int32_t from_z, int dir)
{
    re15_collision_set_band(band);
    int32_t px = x, pz = from_z;
    for (int i = 0; i < 4000; i++) {
        int32_t nx = px, nz = pz + dir * 20;
        re15_collision_constrain(&g_rdt, px, pz, &nx, &nz);
        if (nx == px && nz == pz) break;
        px = nx; pz = nz;
    }
    printf("  band=%d x=%ld: von z=%ld Richtung %+d erreicht der Resolver z=%ld  (= WAND-Kante -/+ (PR450+Skin18))\n",
           band, (long)x, (long)from_z, dir, (long)pz);
}

/* Ein Treppenlauf, Ausloesung bei (x, z0), Blick rot; misst alles. */
static void one_run(int32_t x, int32_t z0, int rot, int band, int cut_in,
                    int want_zone_lo, int want_zone_hi, int want_cut)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = x; pl->z = z0; pl->y = -(int32_t)band * 0x708;
    pl->rot_y = (int16_t)rot; pl->motion = 0; pl->anim_frame = 0; pl->anim_flags = 0;
    re15_stair_reset();
    re15_collision_set_band(band);
    int cut = cut_in;
    g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;

    int in_wall = re15_collision_on_floor(&g_rdt, x, z0);   /* ==1 -> IN der Wandzelle */
    if (!re15_stair_try_start(&g_rdt, 1)) {
        printf("  z0=%6ld %s | KEIN Stair-Start (620-Sonde ausserhalb)\n",
               (long)z0, in_wall ? "IN WAND " : "begehbar");
        return;
    }
    int32_t z_pre = pl->z; int n = 0, in_zone_frames = 0, flips = 0;
    char seq[200]; int sl = 0; sl += snprintf(seq+sl, sizeof seq-sl, "%d", cut);
    while (re15_stair_active() && n < 900) {
        z_pre = pl->z;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        if (pl->z >= want_zone_lo && pl->z <= want_zone_hi) in_zone_frames++;
        g_scd.cut_auto_enabled = 1;
        re15_aot_scan(pl->x, pl->z, (uint8_t)cut);
        if (g_scd.cam_change_pending) {
            int nc = (int)g_scd.cam_id; g_scd.cam_change_pending = 0;
            if (nc != cut) { cut = nc; flips++;
                if (sl < (int)sizeof seq - 8) sl += snprintf(seq+sl, sizeof seq-sl, "->%d", cut); }
        }
        n++;
    }
    int32_t gait_end = z_pre, land = pl->z, eject = land - z_pre;
    printf("  z0=%6ld %s | Gait %6ld->%6ld (%+5ld) | EJECT %+5ld -> %6ld | Frames in Zone[%d..%d]=%2d | Cut %-10s %s\n",
           (long)z0, in_wall ? "IN WAND " : "begehbar",
           (long)z0, (long)gait_end, (long)(gait_end - z0),
           (long)eject, (long)land, want_zone_lo, want_zone_hi, in_zone_frames, seq,
           (cut == want_cut) ? "OK" : "<<<<<< KAMERA SCHWENKT NICHT");
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

    printf("\n==== SCA-WAENDE + REAL ERREICHBARE ANNAEHERUNG ====\n");
    printf("OST-Schacht x=25150 (AOT-Rechteck z[19800..24200], side=0, Abstiegshaelfte z>=22000, Sonde z-620<=24200 -> z<=24820)\n");
    legal_range(25150, 8, 19000, 25500);
    legal_range(25150, 4, 19000, 25500);
    reach_limit(25150, 8, 25400, -1);
    reach_limit(25150, 4, 25400, -1);
    printf("WEST-Schacht x=21400 (AOT-Rechteck z[20700..25000], side=1, Abstiegshaelfte z<22850, Sonde z+620>=20700 -> z>=20080)\n");
    legal_range(21400, 6, 19000, 25500);
    legal_range(21400, 2, 19000, 25500);
    reach_limit(21400, 6, 19400, +1);
    reach_limit(21400, 2, 19400, +1);

    printf("\n==== TREPPE 1 (OST 8->6): braucht RVD[1] 0->1 z[21500..22500] ====\n");
    for (int32_t z = 24540; z <= 24830; z += 10)
        one_run(25150, z, 1024, 8, 0, 21500, 22500, 1);

    printf("\n==== TREPPE 2 (WEST 6->4): braucht RVD[4] 1->2 z[22000..23000] ====\n");
    for (int32_t z = 20070; z <= 20360; z += 10)
        one_run(21400, z, 3072, 6, 1, 22000, 23000, 2);

    printf("\n==== TREPPE 3 (OST 4->2): braucht RVD[7] 2->3 z[23000..24000] UND RVD[10] 3->4 z[20000..21000] ====\n");
    for (int32_t z = 24540; z <= 24830; z += 10)
        one_run(25150, z, 1024, 4, 2, 20000, 21000, 4);

    printf("\n==== TREPPE 4 = LETZTE (WEST 2->0): braucht RVD[13] 4->5 z[24000..25000] ====\n");
    for (int32_t z = 20070; z <= 20360; z += 10)
        one_run(21400, z, 3072, 2, 4, 24000, 25000, 5);
    return 0;
}
