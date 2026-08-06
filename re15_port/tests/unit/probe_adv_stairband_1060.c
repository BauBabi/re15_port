/* probe_adv_stairband_1060.c — ADVERSARIALE GEGENPROBE zu Lane "trajektorie-diff".
 *
 * Prueft EINEN konkreten Methodenvorwurf gegen Finding LD-2:
 * dessen "geometrische Untergrenze >= 5025" wurde aus `collision_reach(slot, band)`
 * abgeleitet, und dort ist `band` das START-Band der Treppe (2/4/6/8) — der
 * LANDEPUNKT liegt aber auf dem ZIEL-Band (0/2/4/6). Wenn die Kollision
 * band-geschichtet ist, ist die zitierte Untergrenze auf der falschen Ebene
 * gemessen.
 *
 * Zusaetzlich: ist der Gait-Endpunkt (ohne Eject) auf dem ZIEL-Band begehbar?
 * Wenn ja, faellt LD-2s Praemisse ("der Gait muss weiter tragen, sonst landet der
 * Spieler in der Kollision") ersatzlos weg.
 *
 * KEIN Engine-Code geaendert; reine Messung.
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

static void load_room(const char *base)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s\n", path); exit(1); }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { fprintf(stderr, "FAIL parse\n"); exit(1); }
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 26000; pl->y = -8 * 0x708; pl->z = 25300;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();
}

/* Begehbare z-Intervalle bei festem x auf einem BESTIMMTEN Band. */
static void walk_intervals(int32_t x, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    printf("    band=%d y=%-7ld x=%-6ld begehbar z:", band,
           (long)pl->y, (long)x);
    int open = 0; int32_t seg = 0;
    for (int32_t z = 17000; z <= 28000; z += 25) {
        int32_t cx = x, cz = z;
        re15_collision_constrain(&g_rdt, x, z, &cx, &cz);
        int ok = (cx == x && cz == z);
        if (ok && !open) { open = 1; seg = z; }
        else if (!ok && open) { open = 0; printf(" [%ld..%ld]", (long)seg, (long)(z - 25)); }
    }
    if (open) printf(" [%ld..28000]", (long)seg);
    printf("\n");
}

static int point_walkable(int32_t x, int32_t z, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    int32_t cx = x, cz = z;
    re15_collision_constrain(&g_rdt, x, z, &cx, &cz);
    return (cx == x && cz == z);
}

/* Treppe fahren, Gait-Endpunkt (VOR dem Eject) und Eject-Endpunkt liefern. */
static void run_stair(int slot, int band_from, int band_to, int32_t trig_z, int rot,
                      const char *label)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const re15_aot_t *a = &g_aot.slots[slot];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = a->x; pl->z = trig_z; pl->y = -(int32_t)band_from * 0x708;
    pl->rot_y = (int16_t)rot;
    pl->motion = 0; pl->anim_frame = 0; pl->anim_flags = 0;
    re15_stair_reset();
    re15_collision_set_band(band_from);
    if (!re15_stair_try_start(&g_rdt, 1)) {
        printf("  %s trigZ=%ld: KEIN Start\n", label, (long)trig_z);
        return;
    }
    int32_t gx = pl->x, gz = pl->z;
    int n = 0;
    while (re15_stair_active() && n < 900) {
        int32_t bx = pl->x, bz = pl->z;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        if (!re15_stair_active()) { gx = bx; gz = bz; }   /* Stand VOR dem Finalize */
        n++;
    }
    int gait_ok  = point_walkable(gx, gz, band_to);
    int land_ok  = point_walkable(pl->x, pl->z, band_to);
    int gait_ok0 = point_walkable(gx, gz, band_from);
    printf("  %s trigZ=%-6ld gaitEnd=(%ld,%ld) begehbar[Ziel b%d]=%s begehbar[Start b%d]=%s"
           "  |  Eject-> (%ld,%ld) begehbar[Ziel]=%s  | Gait-Weg dz=%ld\n",
           label, (long)trig_z, (long)gx, (long)gz, band_to, gait_ok ? "JA" : "NEIN",
           band_from, gait_ok0 ? "JA" : "NEIN",
           (long)pl->x, (long)pl->z, land_ok ? "JA" : "NEIN",
           (long)(gz - trig_z));
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
    load_room(base);

    printf("\n=== A) Begehbare z-Intervalle PRO BAND (West-Treppenachse x=21400) ===\n");
    for (int b = 0; b <= 8; b += 2) walk_intervals(21400, b);
    printf("\n=== A) Begehbare z-Intervalle PRO BAND (Ost-Treppenachse x=25150) ===\n");
    for (int b = 0; b <= 8; b += 2) walk_intervals(25150, b);

    printf("\n=== B) Gait-Endpunkt: auf dem ZIEL-Band begehbar? ===\n");
    /* LETZTE Treppe: slot 9, Band 2->0, bergab +z (rot 3072). */
    for (int32_t tz = 20100; tz <= 20325; tz += 25)
        run_stair(9, 2, 0, tz, 3072, "LETZTE(West 2->0)");
    /* Treppe 3: slot 7, Band 4->2, bergab -z (rot 1024). */
    for (int32_t tz = 24600; tz <= 24820; tz += 55)
        run_stair(7, 4, 2, tz, 1024, "T3(Ost 4->2)   ");

    printf("\n=== C) Ist der ALTE Sweep-Bereich 20800..22800 (probe_stair_cam_1060) "
           "auf dem START-Band 2 begehbar? ===\n");
    for (int32_t z = 20800; z <= 22800; z += 200)
        printf("    z=%-6ld band2=%s band0=%s\n", (long)z,
               point_walkable(21400, z, 2) ? "begehbar" : "BLOCKIERT",
               point_walkable(21400, z, 0) ? "begehbar" : "BLOCKIERT");
    return 0;
}
