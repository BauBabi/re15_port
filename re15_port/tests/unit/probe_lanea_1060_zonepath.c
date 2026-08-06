/* probe_lanea_1060_zonepath.c — LANE A (Diagnose, KEIN ctest)
 *
 * Nutzer-Report 2026-08-06: "Im Treppenhaus schwenkt nach dem Runterlaufen der
 * letzten Treppe die Kamera TEILWEISE nicht richtig um."  Raum = ROOM1060.
 *
 * Diese Sonde beantwortet die Lane-A-Frage:
 *   (a) LUECKENLOSER Dump aller RVD-Zonen / RID-Kameras / AOTs / SCA-Baender
 *       aus ROOM1060.RDT (nur ueber die RDT-Sektionstabelle, keine Mustersuche).
 *   (b) Welche Zonen-Uebergaenge MUSS das Original beim Abstieg 8->6->4->2->0
 *       nacheinander durchlaufen, damit 0->1->2->3->4->5 entsteht?
 *   (c) Liegen die zustaendigen Rechtecke auf dem Weg, den DER PORT nimmt?
 *
 * Der Abstieg wird REALISTISCH gefahren: der Spieler steht VOR dem Treppen-
 * Rechteck (die 620-Vorwaerts-Sonde @0x80042bd0 muss hineinreichen), die
 * Treppen laufen ueber den echten Port-Code, die Laufwege dazwischen sind
 * kollisions-constrained (re15_collision_constrain gegen die echte SCA).
 * Pro Frame laeuft der echte RVD-Scan (re15_aot_scan) UND eine reine
 * GEOMETRIE-Spur (in welchen RVD-Rechtecken liegt der Punkt, unabhaengig vom
 * cam_from-Gate).
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
static int g_trace = 0;

/* Protokoll: fuer jede RVD-Zone der erste/letzte Frame, in dem der Punkt drin lag. */
static int  z_first[RE15_RDT_MAX_ZONES];
static int  z_last [RE15_RDT_MAX_ZONES];
static int32_t z_first_z[RE15_RDT_MAX_ZONES], z_first_x[RE15_RDT_MAX_ZONES];
static int  z_hits [RE15_RDT_MAX_ZONES];

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

/* AABB-Test gegen die RVD-Rechtecke (ROOM1060: alle Zonen sind achsparallel —
 * nachgewiesen im Dump: je 4 Ecken bilden ein Rechteck bzw. bei [18]/[21] ein
 * Trapez; fuer die Spur reicht die AABB, der ECHTE Scan laeuft parallel). */
static int pt_in_zone_aabb(int32_t x, int32_t z, const re15_rdt_zone_t *zz)
{
    int32_t ax = zz->cx - zz->half_w, bx = zz->cx + zz->half_w;
    int32_t az = zz->cz - zz->half_h, bz = zz->cz + zz->half_h;
    return x >= ax && x <= bx && z >= az && z <= bz;
}

static void scan_frame(const char *what)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* GEOMETRIE-Spur */
    for (int i = 0; i < g_rdt.zone_count; i++) {
        int anchor = (i == 0 || g_rdt.zones[i-1].cam_from != g_rdt.zones[i].cam_from);
        if (anchor) continue;                       /* Anker wird nie getestet */
        if (pt_in_zone_aabb(pl->x, pl->z, &g_rdt.zones[i])) {
            if (z_first[i] < 0) { z_first[i] = (int)g_frame; z_first_z[i] = pl->z; z_first_x[i] = pl->x; }
            z_last[i] = (int)g_frame; z_hits[i]++;
        }
    }
    g_scd.cut_auto_enabled = 1;
    re15_aot_scan(pl->x, pl->z, (uint8_t)g_cut);
    if (g_scd.cam_change_pending) {
        int nc = (int)g_scd.cam_id;
        g_scd.cam_change_pending = 0;
        if (nc != g_cut) {
            printf("      >>> F%-4u CUT %d -> %-2d  @(%ld,%ld) y=%ld  [%s]\n",
                   g_frame, g_cut, nc, (long)pl->x, (long)pl->z, (long)pl->y, what);
            g_cut = nc;
        }
    }
    if (g_trace)
        printf("        f%-4u (%6ld,%6ld) y=%6ld cut=%d %s\n", g_frame,
               (long)pl->x, (long)pl->z, (long)pl->y, g_cut, what);
    g_frame++;
}

/* Kollisions-constrained Marsch (40 u/Frame Manhattan) — Hilfsmittel, kein
 * Original-Mechanismus; er benutzt aber die ECHTE SCA-Aufloesung, damit der
 * Weg nicht durch Waende laeuft. */
static void march_to(int32_t tx, int32_t tz, int rot, const char *tag)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->rot_y = (int16_t)rot;
    for (int i = 0; i < 2000; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        if (ad <= 40) break;
        int32_t stepx = (dx >  40) ?  40 : (dx < -40 ? -40 : dx);
        int32_t stepz = (dz >  40) ?  40 : (dz < -40 ? -40 : dz);
        int32_t cx = pl->x + stepx, cz = pl->z + stepz;
        re15_collision_constrain(&g_rdt, pl->x, pl->z, &cx, &cz);
        if (cx == pl->x && cz == pl->z) { scan_frame(tag); break; }   /* blockiert */
        pl->x = cx; pl->z = cz;
        scan_frame(tag);
    }
    scan_frame(tag);
    printf("   -- Marsch-Ende (%ld,%ld) Soll (%ld,%ld) cut=%d\n",
           (long)pl->x, (long)pl->z, (long)tx, (long)tz, g_cut);
}

/* Treppe: Spieler steht VOR dem Rechteck, 620-Sonde reicht hinein. */
static void descend(const char *label, int32_t sx, int32_t sz, int rot, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)rot;
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    re15_stair_reset();
    printf("   == %s: Trigger @(%ld,%ld) band=%d rot=%d cut=%d\n",
           label, (long)sx, (long)sz, band, rot, g_cut);
    if (!re15_stair_try_start(&g_rdt, 1)) { printf("      !! KEIN Stair-Start\n"); return; }
    printf("      Start %s, gesnappter rot=%d\n",
           pl->motion == (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN ? "DOWN" : "UP", (int)pl->rot_y);
    int32_t z0 = pl->z, x0 = pl->x;
    int32_t zmin = pl->z, zmax = pl->z;
    int32_t z_pre = pl->z, x_pre = pl->x;
    int n = 0;
    while (re15_stair_active() && n < 900) {
        z_pre = pl->z; x_pre = pl->x;         /* Zustand VOR dem (evtl. Finalize-)Tick */
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        if (pl->z < zmin) zmin = pl->z;
        if (pl->z > zmax) zmax = pl->z;
        scan_frame("stair");
        n++;
    }
    printf("      Ende nach %d Frames @(%ld,%ld) y=%ld band=%d cut=%d\n"
           "      Gait-Weg dz=%+ld (z %ld -> %ld), z-Spanne [%ld..%ld]  |  EJECT dz=%+ld dx=%+ld  ->  (%ld,%ld)\n",
           n, (long)pl->x, (long)pl->z, (long)pl->y,
           re15_collision_band_from_y(pl->y), g_cut,
           (long)(z_pre - z0), (long)z0, (long)z_pre, (long)zmin, (long)zmax,
           (long)(pl->z - z_pre), (long)(pl->x - x_pre), (long)pl->x, (long)pl->z);
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    g_trace = (argc > 1 && strcmp(argv[1], "-v") == 0);
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

    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("FAIL: %s\n", path); return 1; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { printf("FAIL parse\n"); return 1; }

    /* ---------------------------------------------------------------- (1) RVD */
    printf("\n######## ROOM1060 — (1) RVD-KAMERA-ZONEN (%d) ########\n", g_rdt.zone_count);
    printf(" idx  ANK from  to  floor | AABB x[min..max] z[min..max] | centre        half\n");
    for (int i = 0; i < g_rdt.zone_count; i++) {
        const re15_rdt_zone_t *z = &g_rdt.zones[i];
        int anchor = (i == 0 || g_rdt.zones[i-1].cam_from != z->cam_from);
        printf(" [%2d] %-3s %3d  %3d  0x%02x  | x[%6d..%6d] z[%6d..%6d] | (%6d,%6d) (%5d,%5d)\n",
               i, anchor ? "ANK" : "", (int)z->cam_from, (int)z->cam_to, (int)z->floor,
               (int)(z->cx - z->half_w), (int)(z->cx + z->half_w),
               (int)(z->cz - z->half_h), (int)(z->cz + z->half_h),
               (int)z->cx, (int)z->cz, (int)z->half_w, (int)z->half_h);
    }

    /* ---------------------------------------------------------------- (2) RID */
    printf("\n######## (2) RID-KAMERAS (%d) ########\n", g_rdt.cut_count);
    for (int i = 0; i < g_rdt.cut_count; i++) {
        const re15_camera_cut_t *c = &g_rdt.cuts[i];
        printf(" cut %d: flag=%u fov=%u pos(%7d,%7d,%7d) target(%7d,%7d,%7d) pri=0x%08x  [Ziel-Band ~%d]\n",
               i, c->flag, c->fov, c->pos_x, c->pos_y, c->pos_z,
               c->target_x, c->target_y, c->target_z, c->pri_offset,
               (int)((-c->target_y + 900) / 1800));
    }

    /* ---------------------------------------------------------------- SCA */
    printf("\n######## (4) SCA-KOLLISION: %d Zellen, Partitionen %d/%d/%d/%d/%d ########\n",
           g_rdt.sca_count, g_rdt.sca_rgn[0], g_rdt.sca_rgn[1], g_rdt.sca_rgn[2],
           g_rdt.sca_rgn[3], g_rdt.sca_rgn[4]);
    printf(" (floor-Byte = (band<<4)|3 -> Baender 0,2,4,6,8)\n");
    {
        int shown = 0;
        for (int i = 0; i < g_rdt.sca_count && shown < 40; i++) {
            const re15_sca_entry_t *e = &g_rdt.sca[i];
            if (i >= g_rdt.sca_rgn[0]) break;   /* nur Partition 0 = repraesentativ */
            printf("  [%3d] type=%2d floor=0x%02x(band %d) x[%6d..%6d] z[%6d..%6d]\n",
                   i, e->type, e->floor, (e->floor >> 4), e->x, e->x + (int)e->width,
                   e->z, e->z + (int)e->density);
            shown++;
        }
    }

    /* ---------------------------------------------------------------- AOTs */
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 26000; pl->y = -8 * 0x708; pl->z = 25300;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    printf("\n######## (3) INSTALLIERTE AOTs ########\n");
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        const char *tn = a->type == RE15_AOT_TYPE_STAIR ? "STAIR" :
                         a->type == RE15_AOT_TYPE_DOOR  ? "DOOR"  :
                         a->type == RE15_AOT_TYPE_CAM_SWITCH ? "CAMSW" : "other";
        printf("  slot=%2d %-5s type=%2d sce_flags=0x%02x band=%3d  x[%6ld..%6ld] z[%6ld..%6ld] centre=(%6ld,%6ld)",
               i, tn, (int)a->type, (int)a->sce_flags, (int)a->band,
               (long)(a->x - a->half_w), (long)(a->x + a->half_w),
               (long)(a->z - a->half_h), (long)(a->z + a->half_h),
               (long)a->x, (long)a->z);
        if (a->type == RE15_AOT_TYPE_CAM_SWITCH)
            printf("  from=%d to=%d", (int)a->cam_from_filter, (int)a->event_id);
        if (a->type == RE15_AOT_TYPE_STAIR)
            printf("  axis=%d corner=%d extent=%d side=%d count=%d",
                   (int)g_aot.stair_params[i].axis, (int)g_aot.stair_params[i].corner,
                   (int)g_aot.stair_params[i].extent, (int)g_aot.stair_params[i].side,
                   (int)g_aot.stair_params[i].count);
        printf("\n");
    }

    /* ---------------------------------------------------------------- SOLL-KETTE */
    printf("\n######## SOLL-KETTE (aus der Geometrie abgeleitet) ########\n"
     "  Cut 0 (Band 8) --RVD[ 1] OST  z[21500..22500]--> Cut 1   (waehrend Treppe 1, -z)\n"
     "  Cut 1 (Band 6) --RVD[ 4] WEST z[22000..23000]--> Cut 2   (waehrend Treppe 2, +z)\n"
     "  Cut 2 (Band 4) --RVD[ 7] OST  z[23000..24000]--> Cut 3   (waehrend Treppe 3, -z)\n"
     "  Cut 3 (Band 4) --RVD[10] OST  z[20000..21000]--> Cut 4   (SELBE Treppe 3, weiter -z)\n"
     "  Cut 4 (Band 2) --RVD[13] WEST z[24000..25000]--> Cut 5   (waehrend Treppe 4 = LETZTE, +z)\n"
     "  Cut 5 (Band 0) --RVD[16] x[23500..24500] z[24500..27500]--> Cut 7 (Tuer, Laufweg)\n");

    /* ---------------------------------------------------------------- LAUF */
    for (int i = 0; i < RE15_RDT_MAX_ZONES; i++) { z_first[i] = -1; z_last[i] = -1; z_hits[i] = 0; }
    g_cut = 0; g_frame = 0;
    g_scd.cam_id = 0; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;

    printf("\n######## REALISTISCHER ABSTIEG (Port-Code) ########\n");
    /* Eintritt aus ROOM1120: (26000, -14400, 25300), Band 8, Cut 0. */
    pl->x = 26000; pl->z = 25300; pl->y = -8 * 0x708; pl->rot_y = 1024;
    re15_collision_set_band(8);
    printf("   Spawn (%ld,%ld) Band 8 Cut %d\n", (long)pl->x, (long)pl->z, g_cut);
    scan_frame("spawn");

    /* Treppe 1: OST-Schacht. SCA-Blocker Band 8 = x[22900..27100] z[20000..24100],
     * d.h. der Spieler kann nur bis z ~24100 nach Sueden. Blick -z (rot 0x400). */
    march_to(25150, 24400, 1024, "walk->T1");
    descend("Treppe 1  OST 8->6 (abwaerts -z)", 25150, 24400, 1024, 8);

    /* Treppe 2: WEST-Schacht, Rechteck z[20700..25000], side=1 -> abwaerts +z.
     * Der Spieler muss suedlich des West-Blockers (z<20800) stehen und +z schauen. */
    march_to(21400, 19900, 2048, "walk->T2a");
    march_to(21400, 20600, 3072, "walk->T2b");
    descend("Treppe 2  WEST 6->4 (abwaerts +z)", 21400, 20600, 3072, 6);

    /* Treppe 3: OST-Schacht, Band 4. Erst nach Osten, dann nach Sueden bis z ~24400. */
    march_to(25150, 24950,    0, "walk->T3a");
    march_to(25150, 24400, 1024, "walk->T3b");
    descend("Treppe 3  OST 4->2 (abwaerts -z)", 25150, 24400, 1024, 4);

    /* Treppe 4 = LETZTE: WEST-Schacht, Band 2. */
    march_to(21400, 19900, 2048, "walk->T4a");
    march_to(21400, 20600, 3072, "walk->T4b");
    descend("Treppe 4  WEST 2->0 = LETZTE (abwaerts +z)", 21400, 20600, 3072, 2);

    /* Danach: zur Ausgangstuer ROOM1040 @ x[26600..27600] z[24300..26500]. */
    march_to(27100, 25400, 0, "walk->Tuer");

    printf("\nEND-CUT = %d @(%ld,%ld) band=%d\n", g_cut, (long)pl->x, (long)pl->z,
           re15_collision_band_from_y(pl->y));

    /* ---------------------------------------------------------------- Deckung */
    printf("\n######## GEOMETRIE-DECKUNG: welche RVD-Rechtecke hat der Weg beruehrt? ########\n");
    for (int i = 0; i < g_rdt.zone_count; i++) {
        int anchor = (i == 0 || g_rdt.zones[i-1].cam_from != g_rdt.zones[i].cam_from);
        if (anchor) continue;
        const re15_rdt_zone_t *z = &g_rdt.zones[i];
        printf("  RVD[%2d] %d->%-2d x[%6d..%6d] z[%6d..%6d] : %s",
               i, (int)z->cam_from, (int)z->cam_to,
               (int)(z->cx - z->half_w), (int)(z->cx + z->half_w),
               (int)(z->cz - z->half_h), (int)(z->cz + z->half_h),
               z_hits[i] ? "" : "NIE BERUEHRT");
        if (z_hits[i])
            printf("%d Frames, zuerst F%d @(%ld,%ld), zuletzt F%d",
                   z_hits[i], z_first[i], (long)z_first_x[i], (long)z_first_z[i], z_last[i]);
        printf("\n");
    }
    return 0;
}
