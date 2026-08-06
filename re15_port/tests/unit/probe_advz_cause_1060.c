/* probe_advz_cause_1060.c — ADVERSARIALE GEGENPROBE (kein ctest)
 *
 * Prueft die KAUSALKETTE der Lane-E-Befunde E-3/E-4 mit einem TRIAL-PATCH,
 * der NUR in der Sonde lebt (kein Engine-Code angefasst):
 *
 *  (T1) BASIS: letzte Treppe ROOM1060 (West, Band 2->0) bei z=20140, Cut 4.
 *       Erwartung laut Befund: Cut bleibt 4 (RVD[13] 4->5 uebersprungen).
 *
 *  (T2) TRIAL-PATCH "Finalize verteilt": identischer Lauf, aber der EINE
 *       Finalize-Tick wird in 100er-Schritten abgetastet (Position wird auf
 *       das Gait-Ende zurueckgesetzt und in Schritten auf den Finalize-
 *       Endpunkt gefahren, mit re15_aot_scan pro Schritt).  Endpunkt und
 *       Gait sind IDENTISCH zu T1 — nur die Abtastung aendert sich.
 *       Schaltet der Cut hier auf 5, ist der EIN-TICK-SPRUNG die proximate
 *       Ursache.  Bleibt er 4, ist es der Gait.
 *
 *  (T3) GEGENPROBE "Original-artiger Resolver": was macht ein reiner
 *       Nearest-Edge-Constrain (re15_collision_constrain, old==new) am
 *       Gait-Ende?  Das ist die Bewegung, die das ORIGINAL laut Port-
 *       Kommentar (stair_common.c:110-141) im NAECHSTEN Frame ausfuehren
 *       wuerde.  Ist dieser Push ebenfalls >1000 oder ueberquert er den
 *       Streifen, dann kann auch das Original Zonen ueberspringen und
 *       "im Original unmoeglich" waere widerlegt.
 *
 *  (T4) FENSTER-SWEEP: alle Ausloese-z von 20080..20340 in 10er-Schritten,
 *       Trefferquote gegen die Behauptung "8 von 14 scheitern (z<=20220)".
 *
 *  (T5) dasselbe fuer Treppe 3 (Ost, Band 4->2, RVD[10] 3->4).
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
static int g_verbose = 1;

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

static void scan_at(int32_t x, int32_t z, const char *what)
{
    g_scd.cut_auto_enabled = 1;
    re15_aot_scan(x, z, (uint8_t)g_cut);
    if (g_scd.cam_change_pending) {
        int nc = (int)g_scd.cam_id;
        g_scd.cam_change_pending = 0;
        if (nc != g_cut) {
            if (g_verbose)
                printf("        F%-4u CUT %d -> %-2d @(%ld,%ld)  [%s]\n",
                       g_frame, g_cut, nc, (long)x, (long)z, what);
            g_cut = nc;
        }
    }
    g_frame++;
}

static void room_setup(const char *base, int32_t px, int32_t pz, int band)
{
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->z = pz; pl->y = -(int32_t)band * 0x708;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
    (void)base;
}

/* mode 0 = Basis (Finalize wie im Port, ein Tick)
 * mode 1 = TRIAL: Finalize-Tick in `substep` Einheiten abgetastet
 * Rueckgabe: End-Cut.  gait_end/final_pos werden gefuellt. */
static int run_descent(const char *base, int32_t sx, int32_t sz, int rot, int band,
                       int start_cut, int mode, int substep,
                       int32_t *gx, int32_t *gz, int32_t *fx, int32_t *fz)
{
    room_setup(base, sx, sz, band);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)rot;
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    re15_stair_reset();
    g_cut = start_cut; g_frame = 0;
    g_scd.cam_id = (uint8_t)start_cut; g_scd.cam_change_pending = 0;

    if (!re15_stair_try_start(&g_rdt, 1)) { printf("        KEIN Stair-Start!\n"); return -1; }

    int32_t prevx = pl->x, prevz = pl->z;
    int n = 0;
    while (re15_stair_active() && n < 900) {
        prevx = pl->x; prevz = pl->z;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        int was_last = !re15_stair_active();          /* der Finalize-Tick */
        if (was_last && mode == 1) {
            int32_t ex = pl->x, ez = pl->z;
            int32_t dx = ex - prevx, dz = ez - prevz;
            int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            int steps = (ad + substep - 1) / substep;
            if (steps < 1) steps = 1;
            for (int s = 1; s <= steps; s++) {
                int32_t ix = prevx + (int32_t)(((int64_t)dx * s) / steps);
                int32_t iz = prevz + (int32_t)(((int64_t)dz * s) / steps);
                scan_at(ix, iz, "finalize-substep");
            }
            *gx = prevx; *gz = prevz; *fx = ex; *fz = ez;
        } else {
            if (was_last) { *gx = prevx; *gz = prevz; *fx = pl->x; *fz = pl->z; }
            scan_at(pl->x, pl->z, was_last ? "finalize" : "stair");
        }
        n++;
    }
    return g_cut;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data || re15_rdt_parse(data, size, &g_rdt) != 0) { printf("FAIL RDT\n"); return 1; }
    {
        char p2[600]; size_t s1 = 0, s2 = 0;
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EDD", base);
        uint8_t *edd = read_file(p2, &s1);
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EMR", base);
        uint8_t *emr = read_file(p2, &s2);
        if (edd && emr && re15_emd_parse_animation(edd, s1, &g_anim) == 0 &&
            re15_emd_parse_skeleton(emr, s2, &g_skel) == 0) g_skel_ok = 1;
        printf("PL00 %s\n\n", g_skel_ok ? "geladen" : "FEHLT");
    }

    int32_t gx, gz, fx, fz;
    int c;

    printf("=========== T1 BASIS: letzte Treppe (West 2->0) z=20140, Start-Cut 4 ===========\n");
    c = run_descent(base, 21400, 20140, 3072, 2, 4, 0, 0, &gx, &gz, &fx, &fz);
    printf("   Gait-Ende=(%ld,%ld)  Finalize-Ziel=(%ld,%ld)  Versatz=%ld  END-CUT=%d\n\n",
           (long)gx, (long)gz, (long)fx, (long)fz, (long)(fz - gz), c);

    printf("=========== T2 TRIAL-PATCH: derselbe Lauf, Finalize in 100er-Schritten ===========\n");
    c = run_descent(base, 21400, 20140, 3072, 2, 4, 1, 100, &gx, &gz, &fx, &fz);
    printf("   Gait-Ende=(%ld,%ld)  Finalize-Ziel=(%ld,%ld)  Versatz=%ld  END-CUT=%d\n",
           (long)gx, (long)gz, (long)fx, (long)fz, (long)(fz - gz), c);
    printf("   => %s\n\n", c == 5 ? "EIN-TICK-SPRUNG IST DIE PROXIMATE URSACHE (Cut schaltet bei Abtastung)"
                                  : "Sprung ist NICHT allein schuld");

    printf("=========== T3 GEGENPROBE: reiner Nearest-Edge-Constrain am Gait-Ende ===========\n");
    {
        /* Was wuerde der ORIGINAL-Resolver (stationaer, old==new) mit dem Gait-Ende tun? */
        int32_t tx = gz ? gx : gx, tz = gz;
        int32_t cx = gx, cz = gz;
        re15_collision_constrain(&g_rdt, gx, gz, &cx, &cz);
        printf("   Gait-Ende (%ld,%ld) begehbar? %s\n", (long)gx, (long)gz,
               re15_collision_on_floor(&g_rdt, gx, gz) ? "JA" : "NEIN");
        printf("   constrain(old=(%ld,%ld), new=(%ld,%ld)) -> (%ld,%ld)  d=(%+ld,%+ld) |dz|=%ld\n",
               (long)gx, (long)gz, (long)tx, (long)tz, (long)cx, (long)cz,
               (long)(cx - gx), (long)(cz - gz), (long)(cz - gz < 0 ? gz - cz : cz - gz));
        /* Und was macht er, wenn man ihn wie das Original einen Frame spaeter mit
         * einem winzigen Vorwaerts-Schritt aufruft? */
        int32_t nx = gx, nz = gz + 10;
        re15_collision_constrain(&g_rdt, gx, gz, &nx, &nz);
        printf("   constrain(old=Gait-Ende, new=+10z) -> (%ld,%ld)  d=(%+ld,%+ld)\n",
               (long)nx, (long)nz, (long)(nx - gx), (long)(nz - gz));
    }
    printf("\n");

    printf("=========== T4 FENSTER-SWEEP letzte Treppe (West 2->0), Start-Cut 4 ===========\n");
    g_verbose = 0;
    {
        int fail = 0, tot = 0;
        for (int32_t z = 20080; z <= 20340; z += 20) {
            c = run_descent(base, 21400, z, 3072, 2, 4, 0, 0, &gx, &gz, &fx, &fz);
            tot++;
            if (c != 5) fail++;
            printf("   z=%ld  Gait-Ende z=%ld  Finalize z=%ld (Versatz %+ld)  END-CUT=%d  %s\n",
                   (long)z, (long)gz, (long)fz, (long)(fz - gz), c,
                   c == 5 ? "OK" : "<<< KAMERA SCHALTET NICHT");
        }
        printf("   -> %d von %d Messpunkten scheitern\n\n", fail, tot);
    }

    printf("=========== T5 FENSTER-SWEEP Treppe 3 (Ost 4->2), Start-Cut 3 (RVD[10]) ===========\n");
    {
        int fail = 0, tot = 0;
        for (int32_t z = 24550; z <= 24820; z += 20) {
            c = run_descent(base, 25150, z, 1024, 4, 3, 0, 0, &gx, &gz, &fx, &fz);
            tot++;
            if (c != 4) fail++;
            printf("   z=%ld  Gait-Ende z=%ld  Finalize z=%ld (Versatz %+ld)  END-CUT=%d  %s\n",
                   (long)z, (long)gz, (long)fz, (long)(fz - gz), c,
                   c == 4 ? "OK" : "<<< KAMERA SCHALTET NICHT");
        }
        printf("   -> %d von %d Messpunkten scheitern\n\n", fail, tot);
    }

    printf("=========== T6 TRIAL-PATCH ueber das ganze Fenster (letzte Treppe) ===========\n");
    {
        int fail = 0, tot = 0;
        for (int32_t z = 20080; z <= 20340; z += 20) {
            c = run_descent(base, 21400, z, 3072, 2, 4, 1, 100, &gx, &gz, &fx, &fz);
            tot++;
            if (c != 5) fail++;
            printf("   z=%ld  END-CUT=%d %s\n", (long)z, c, c == 5 ? "OK" : "<<< immer noch nicht");
        }
        printf("   -> mit verteiltem Finalize scheitern %d von %d\n", fail, tot);
    }

    printf("\n=========== T7 KERNFRAGE: waere der Sprung OHNE den Port-Eject weg? ===========\n");
    printf("   Der Port-Eject (stair_common.c:142-181) marschiert nur, solange der\n");
    printf("   Nearest-Edge-Push RUECKWAERTS zeigt. Hier wird gemessen, was der REINE\n");
    printf("   byte-true Resolver FUN_8003b0a4 (re15_collision_constrain, r=450, mask=1)\n");
    printf("   am Gait-Ende macht — also das, was das ORIGINAL im naechsten Frame tut.\n");
    for (int32_t z = 20080; z <= 20340; z += 40) {
        c = run_descent(base, 21400, z, 3072, 2, 4, 0, 0, &gx, &gz, &fx, &fz);
        re15_collision_set_band(0);
        int32_t cx = gx, cz = gz;
        re15_collision_constrain(&g_rdt, gx, gz, &cx, &cz);          /* stationaer (old==new) */
        int32_t nx = gx, nz = gz + 10;                                /* +10 vorwaerts (Pad) */
        re15_collision_constrain(&g_rdt, gx, gz, &nx, &nz);
        printf("   z=%ld Gait-Ende=(%ld,%ld) Port-Eject->z=%ld | REINER Resolver: stationaer->z=%ld (%+ld)"
               "  mit +10z->z=%ld (%+ld)\n",
               (long)z, (long)gx, (long)gz, (long)fz,
               (long)cz, (long)(cz - gz), (long)nz, (long)(nz - gz));
    }

    printf("\n=========== T8 dasselbe fuer Treppe 3 (Ost 4->2, Band 2) ===========\n");
    for (int32_t z = 24710; z <= 24810; z += 20) {
        c = run_descent(base, 25150, z, 1024, 4, 3, 0, 0, &gx, &gz, &fx, &fz);
        re15_collision_set_band(2);
        int32_t cx = gx, cz = gz;
        re15_collision_constrain(&g_rdt, gx, gz, &cx, &cz);
        printf("   z=%ld Gait-Ende=(%ld,%ld) Port-Eject->z=%ld | REINER Resolver stationaer->z=%ld (%+ld)\n",
               (long)z, (long)gx, (long)gz, (long)fz, (long)cz, (long)(cz - gz));
    }

    printf("\n=========== T9 SIMULATION 'Eject-Block ERSATZLOS GESTRICHEN' ===========\n");
    printf("   Ablauf wie game_step_common.c: Finalize-Tick OHNE Versatz -> Scan am Gait-Ende;\n");
    printf("   naechster Frame: re15_collision_constrain (byte-true FUN_8003b0a4) -> DANN Scan\n");
    printf("   (Reihenfolge Move->constrain->aot_scan, game_step_common.c:516-523).\n");
    g_verbose = 1;
    for (int32_t z = 20080; z <= 20340; z += 40) {
        /* Gait fahren, Finalize-Position verwerfen, Spieler auf das Gait-Ende setzen */
        c = run_descent(base, 21400, z, 3072, 2, 4, 0, 0, &gx, &gz, &fx, &fz);
        /* Der obige Lauf hat den Port-Eject bereits gescannt. Jetzt der Gegenlauf: */
        room_setup(base, 21400, z, 2);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->x = 21400; pl->z = z; pl->rot_y = 3072; pl->y = -2 * 0x708;
        re15_collision_set_band(2);
        re15_stair_reset();
        g_cut = 4; g_frame = 0; g_scd.cam_id = 4; g_scd.cam_change_pending = 0;
        if (!re15_stair_try_start(&g_rdt, 1)) { printf("   KEIN Start\n"); continue; }
        int32_t px = pl->x, pz = pl->z;
        while (re15_stair_active()) {
            px = pl->x; pz = pl->z;
            re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
            if (!re15_stair_active()) { pl->x = px; pl->z = pz; }   /* Eject gestrichen */
            scan_at(pl->x, pl->z, "stair");
        }
        int cut_after_gait = g_cut;
        /* naechster normaler Frame: Pad neutral -> constrain(old==new) -> scan */
        int32_t nx = pl->x, nz = pl->z;
        re15_collision_constrain(&g_rdt, pl->x, pl->z, &nx, &nz);
        re15_collision_objects(&nx, &nz);
        pl->x = nx; pl->z = nz;
        scan_at(pl->x, pl->z, "naechster Frame nach constrain");
        printf("   z=%ld  Gait-Ende=(%ld,%ld) Cut=%d -> nach constrain (%ld,%ld) END-CUT=%d %s\n",
               (long)z, (long)px, (long)pz, cut_after_gait, (long)pl->x, (long)pl->z, g_cut,
               g_cut == 5 ? "OK" : "<<< KAMERA SCHALTET IMMER NOCH NICHT");
    }

    printf("\n=========== T10 Wie oft weicht der Port-Eject ueberhaupt vom reinen Resolver ab? ===========\n");
    printf("   (Nur wenn er abweicht, ist der Eject-Block ueberhaupt verhaltensrelevant.)\n");
    {
        static const char *rooms[] = { "ROOM1060", "ROOM10A0", "ROOM1170" };
        static const int   rids [] = { 0x1060, 0x10A0, 0x1170 };
        for (int r = 0; r < 3; r++) {
            char p[600]; size_t sz2 = 0;
            snprintf(p, sizeof p, "%s/STAGE1/%s.RDT", base, rooms[r]);
            uint8_t *d2 = read_file(p, &sz2);
            if (!d2 || re15_rdt_parse(d2, sz2, &g_rdt) != 0) { printf("   %s: FAIL\n", rooms[r]); continue; }
            /* Raum einmal hochfahren, um die Treppen-AOTs zu bekommen */
            re15_actor_init(); scd_vm_init();
            g_current_room_id = (uint16_t)rids[r];
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->active = 1; pl->type = 0; pl->hp = 100;
            scd_register_room_events(&g_rdt); scd_room_reenter(&g_rdt, 0, 0, 0);
            for (int f = 0; f < 10; f++) scd_vm_tick();
            re15_aot_t stairs[RE15_AOT_MAX]; int ns = 0;
            for (int i = 0; i < RE15_AOT_MAX; i++)
                if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_STAIR)
                    stairs[ns++] = g_aot.slots[i];
            int runs = 0, differ = 0, maxdiff = 0;
            for (int s = 0; s < ns; s++) {
                const re15_aot_t *a = &stairs[s];
                for (int32_t zz = a->z - a->half_h - 600; zz <= a->z + a->half_h + 600; zz += 200)
                for (int32_t xx = a->x - a->half_w; xx <= a->x + a->half_w; xx += 400)
                for (int rr = 0; rr < 4; rr++) {
                    int rot = rr * 1024;
                    g_verbose = 0;
                    room_setup(base, xx, zz, (int)a->band);
                    g_current_room_id = (uint16_t)rids[r];
                    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                    pl->x = xx; pl->z = zz; pl->rot_y = (int16_t)rot;
                    pl->y = -(int32_t)a->band * 0x708;
                    re15_collision_set_band((int)a->band);
                    re15_stair_reset();
                    g_cut = 0; g_frame = 0; g_scd.cam_id = 0; g_scd.cam_change_pending = 0;
                    if (!re15_stair_try_start(&g_rdt, 1)) continue;
                    if (pl->motion != (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN) { re15_stair_reset(); continue; }
                    int32_t px = pl->x, pz = pl->z; int nt = 0;
                    while (re15_stair_active() && nt < 900) {
                        px = pl->x; pz = pl->z;
                        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
                        nt++;
                    }
                    int32_t ejx = pl->x, ejz = pl->z;
                    int32_t cx = px, cz = pz;
                    re15_collision_constrain(&g_rdt, px, pz, &cx, &cz);
                    runs++;
                    int dd = (int)((cx - ejx < 0 ? ejx - cx : cx - ejx) + (cz - ejz < 0 ? ejz - cz : cz - ejz));
                    if (dd > 0) { differ++; if (dd > maxdiff) maxdiff = dd; }
                }
            }
            printf("   %s: %d Abstiege, Port-Eject != reiner Resolver in %d Faellen (max. Abweichung %d)\n",
                   rooms[r], runs, differ, maxdiff);
        }
    }
    return 0;
}
