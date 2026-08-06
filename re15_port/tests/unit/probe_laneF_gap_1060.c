/* probe_laneF_gap_1060.c — LANE F DIAGNOSE (kein ctest).
 *
 * Der RVD-Scan des Originals ist ein PUNKT-Test pro Frame:
 *   FUN_80014230 @0x80014298 `jal FUN_80014368(&DAT_800aca88, rec)` — Punkt-in-Quad
 *   auf der AKTUELLEN Spielerposition, kein Segment, keine Interpolation.
 * Er kann also jede Zone verfehlen, ueber die der Spieler in EINEM Tick hinwegspringt.
 *
 * Das Original springt waehrend der Treppe NIE:
 *   - Vorwaerts pro Tick = DAT_800acae0 = 10  (`ori v0,zero,0xa` @0x80038cc0,
 *     `sh v0,DAT_800acae0` @0x80038cc8; angewandt von FUN_800245d8 @0x80038dc4)
 *   - Der FINALIZE-Tick LAB_80038e50 (@0x80038e50..0x80038eec) hat KEINEN Store nach
 *     playerX(0x800aca88)/playerZ(0x800aca90) — nur Band (@0x80038edc), Clip, Modus.
 * Der Port dagegen versetzt den Spieler im Finalize um bis zu 48*100u
 * (stair_common.c: `for (int step = 0; step < 48; step++)` + `tx += cs*100>>12`).
 *
 * Diese Sonde MISST fuer jede der vier ROOM1060-Treppen und ueber einen Sweep von
 * Startpositionen:
 *   (G1) den groessten Ein-Tick-Versatz waehrend der Gangart
 *   (G2) den Versatz des FINALIZE-Ticks
 *   (G3) ob die DISKRETE Abtastung eine Kamera-Zone VERFEHLT, die eine dichte
 *        (10u) Abtastung derselben Bahn getroffen haette  = beweisbarer Zonen-Skip
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

/* Zustandsmaschinen-Schritt auf einem Punkt — GENAU FUN_80014230:
 * die erste (in Tabellen-Reihenfolge) Zone mit cam_from == cur, in der der Punkt
 * liegt, gewinnt; Floor-Byte 0xFF = jede Etage. */
static int cut_step(int cur, int32_t x, int32_t z)
{
    for (int i = 0; i < g_rdt.zone_count; i++) {
        if (i == 0 || g_rdt.zones[i-1].cam_from != g_rdt.zones[i].cam_from) continue; /* Anker */
        const re15_rdt_zone_t *zn = &g_rdt.zones[i];
        if ((int)zn->cam_from != cur) continue;
        if (zn->floor != 0xFF) continue;   /* ROOM1060: alle 0xFF */
        if (re15_aot_point_in_quad(x, z, zn->xs, zn->zs)) return (int)zn->cam_to;
    }
    return cur;
}

typedef struct { int32_t x, z; } pt_t;

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
    pl->x = 25150; pl->z = 24000; pl->y = -8 * 0x708;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    /* Die vier Abstiege: {Band, X, Achsen-Start-Z, rot, Start-Cut}. Blickrichtung
     * geometrisch korrekt (Treppe hinab, side-Regel LAB_800435cc @0x800435ec-90). */
    struct { int band; int32_t x; int32_t z_top; int rot; int cut0; const char *name; } fl[4] = {
        { 8, 25150, 24200, 1024, 0, "T1 Band8->6 (Ost, abwaerts -Z)  Start-Cut 0" },
        { 6, 21400, 20700, 3072, 1, "T2 Band6->4 (West, abwaerts +Z) Start-Cut 1" },
        { 4, 25150, 24200, 1024, 2, "T3 Band4->2 (Ost, abwaerts -Z)  Start-Cut 2" },
        { 2, 21400, 20700, 3072, 4, "T4 Band2->0 (West, abwaerts +Z) Start-Cut 4" },
    };

    printf("=== LANE F (G1..G3): Zonen-Skip durch diskrete Abtastung, ROOM1060 ===\n");
    printf("PL00 %s\n\n", g_skel_ok ? "geladen" : "FEHLT");

    for (int f = 0; f < 4; f++) {
        printf("---- %s ----\n", fl[f].name);
        int worst_gap = 0; int32_t worst_off = 0;
        /* off > 0 = weiter OBEN starten (ausserhalb des Rechtecks). Legal, solange die
         * 620u-Vorwaertssonde (`ori v0,zero,0x26c` @0x80042bd0) noch im Rechteck landet. */
        for (int32_t off = -800; off <= 800; off += 10) {
            int32_t sz = fl[f].z_top + ((fl[f].rot == 3072) ? +off : -off);
            /* Startpunkt innerhalb des Rechtecks halten */
            pl->x = fl[f].x; pl->z = sz;
            pl->y = -(int32_t)fl[f].band * 0x708;
            pl->rot_y = (int16_t)fl[f].rot;
            re15_collision_set_band(fl[f].band);
            re15_stair_reset();
            if (!re15_stair_try_start(&g_rdt, 1)) continue;

            static pt_t trace[1200];
            int n = 0;
            trace[n].x = pl->x; trace[n].z = pl->z; n++;
            int32_t maxstep = 0, finstep = 0;
            int fin_seen = 0; int32_t pre_z = 0, post_z = 0;
            while (re15_stair_active() && n < 1100) {
                int was_last = 0;
                int32_t px = pl->x, pz = pl->z;
                re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
                if (!re15_stair_active()) was_last = 1;   /* das war der FINALIZE-Tick */
                int32_t dx = pl->x - px, dz = pl->z - pz;
                int32_t d = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
                if (was_last) { finstep = d; fin_seen = 1; pre_z = pz; post_z = pl->z; }
                else if (d > maxstep) maxstep = d;
                trace[n].x = pl->x; trace[n].z = pl->z; n++;
            }

            /* DISKRET (wie der Port scannt: ein Punkt pro Tick) */
            int cut_d = fl[f].cut0;
            int chain_d[32]; int nd = 0; chain_d[nd++] = cut_d;
            for (int i = 0; i < n; i++) {
                int nc = cut_step(cut_d, trace[i].x, trace[i].z);
                if (nc != cut_d) { cut_d = nc; if (nd < 32) chain_d[nd++] = cut_d; }
            }
            /* DICHT (10u entlang jeder Tick-Strecke = "kontinuierliche" Bahn) */
            int cut_c = fl[f].cut0;
            int chain_c[64]; int nc2 = 0; chain_c[nc2++] = cut_c;
            for (int i = 1; i < n; i++) {
                int32_t x0 = trace[i-1].x, z0 = trace[i-1].z;
                int32_t x1 = trace[i].x,   z1 = trace[i].z;
                int32_t dx = x1 - x0, dz = z1 - z0;
                int32_t dist = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
                int sub = (int)(dist / 10) + 1;
                for (int s = 1; s <= sub; s++) {
                    int32_t px = x0 + (int32_t)((int64_t)dx * s / sub);
                    int32_t pz = z0 + (int32_t)((int64_t)dz * s / sub);
                    int nn = cut_step(cut_c, px, pz);
                    if (nn != cut_c) { cut_c = nn; if (nc2 < 64) chain_c[nc2++] = cut_c; }
                }
            }
            int gap = (cut_d != cut_c);
            if (gap && !worst_gap) { worst_gap = 1; worst_off = off; }
            /* Legalitaets-Check: die 620u-Vorwaertssonde muss im Treppen-Rechteck liegen
             * (`ori v0,zero,0x26c` @0x80042bd0 -> Rect-Test @0x80042f0c-2c, flags-Bit 0x20
             * @0x80042ef8). Rechteck-Z: T1/T3 [19800..24200], T2/T4 [20700..25000]. */
            int32_t probe_z = (fl[f].rot == 3072) ? (sz + 620) : (sz - 620);
            int32_t rlo = (fl[f].rot == 3072) ? 20700 : 19800;
            int32_t rhi = (fl[f].rot == 3072) ? 25000 : 24200;
            int legal = (probe_z >= rlo && probe_z <= rhi);
            if (!legal) continue;
            if (off % 200 == 0 || gap) {
                printf("  start_z=%6ld  ticks=%3d  maxTick=%4ld  FIN=%4ld (z %ld->%ld)%s"
                       "  diskret:", (long)sz, n - 1, (long)maxstep, (long)finstep,
                       (long)pre_z, (long)post_z, fin_seen ? "" : "(?)");
                for (int i = 0; i < nd; i++) printf("%s%d", i ? "->" : "", chain_d[i]);
                printf("   dicht:");
                for (int i = 0; i < nc2; i++) printf("%s%d", i ? "->" : "", chain_c[i]);
                printf("%s\n", gap ? "   <<< ZONEN-SKIP" : "");
            }
        }
        if (worst_gap) printf("  => SKIP reproduzierbar (erstes off=%ld)\n", (long)worst_off);
        else           printf("  => kein Skip in diesem Sweep\n");
        printf("\n");
    }

    /* ---- (G5) RUECKWAERTS-Schritte waehrend der Gangart (Ursache des 1-Frame-
     * Kamera-Flackerns an den geschlossenen Zonen-Raendern: RVD[1] 0->1 z[21500..22500]
     * und RVD[3] 1->0 z[22500..23500] teilen die Kante 22500, FUN_80014368 ist auf
     * dieser Ecken-Reihenfolge auf ALLEN VIER Kanten inklusiv). */
    printf("---- (G5) Rueckwaerts-Schritte in der Gangart (T1, start_z=24000) ----\n");
    {
        pl->x = 25150; pl->z = 24000; pl->y = -8 * 0x708; pl->rot_y = 1024;
        re15_collision_set_band(8);
        re15_stair_reset();
        if (re15_stair_try_start(&g_rdt, 1)) {
            int nback = 0; int32_t worst = 0; int t = 0;
            while (re15_stair_active()) {
                int32_t pz = pl->z;
                re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
                int32_t d = pl->z - pz;          /* Abstieg = -Z, also d<0 = vorwaerts */
                if (d > 0) { nback++; if (d > worst) worst = d; }
                t++;
            }
            printf("   %d von %d Ticks bewegen den Spieler RUECKWAERTS, max +%ld u\n",
                   nback, t, (long)worst);
        }
    }
    printf("\n");

    /* ---- (G4) Erholt sich die Kamera nach einem verpassten 4->5?  ------------
     * Letzte Treppe mit start_z=20150 (im belegten Skip-Fenster), danach zur Tuer
     * marschieren und pruefen, ob der Tuer-Cut 7 je erreicht wird.
     * Tuer-Zone = RVD[16] cam_from=5 -> 7, x[23500..24500] z[24500..27500]. */
    printf("---- (G4) Erholung nach verpasstem 4->5 (letzte Treppe) ----\n");
    for (int variant = 0; variant < 2; variant++) {
        int32_t sz = variant ? 20500 : 20150;   /* 20150 = Skip-Fenster, 20500 = gesund */
        pl->x = 21400; pl->z = sz; pl->y = -2 * 0x708; pl->rot_y = 3072;
        re15_collision_set_band(2);
        re15_stair_reset();
        if (!re15_stair_try_start(&g_rdt, 1)) { printf("   kein Start\n"); continue; }
        int cut = 4;
        while (re15_stair_active()) {
            re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
            cut = cut_step(cut, pl->x, pl->z);
        }
        printf("   start_z=%ld -> nach Treppe: pos=(%ld,%ld) cut=%d\n",
               (long)sz, (long)pl->x, (long)pl->z, cut);
        /* Marsch zur Tuer-Zone-Mitte (24000, 26000), 40u/Frame Manhattan. */
        int32_t tx = 24000, tz = 26000;
        for (int i = 0; i < 400; i++) {
            int32_t dx = tx - pl->x, dz = tz - pl->z;
            if ((dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz) <= 40) break;
            int32_t sx = dx > 0 ? 1 : (dx < 0 ? -1 : 0);
            int32_t szg = dz > 0 ? 1 : (dz < 0 ? -1 : 0);
            pl->x += ((dx < 0 ? -dx : dx) > 40) ? sx * 40 : dx;
            pl->z += ((dz < 0 ? -dz : dz) > 40) ? szg * 40 : dz;
            cut = cut_step(cut, pl->x, pl->z);
        }
        printf("       -> an der Tuer (%ld,%ld): CUT = %d %s\n",
               (long)pl->x, (long)pl->z, cut,
               cut == 7 ? "(korrekt: Tuer-Cut)" : "<<< FALSCHER CUT, keine Erholung");
    }
    return 0;
}
