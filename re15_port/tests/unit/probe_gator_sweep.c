/* probe_gator_sweep.c — BRUTE-FORCE-KONVERGENZTEST des ROOM2090-Bosskampfs.
 *
 * ANLASS (Nutzer 2026-09-11, nach sechs Klemm-/Pendel-Markern in Folge):
 * "Kannst du mal ein bisschen voraus schauen was den Aligator noch alles
 * blocken wird, statt dass wir Fall fuer Fall durchspielen?"
 *
 * Der Test laesst den ECHTEN Boss-Tick (re15_gator_boss_tick + die
 * Dispatcher-SCA-Klemme, KEIN Nachbau — Lektion reai-v2-live-statt-
 * nachbildung) ueber ein Raster ALLER Gator-Startpositionen x einer
 * kuratierten Menge Leon-Positionen laufen (Ufer, Ecken, Nischen, Wasser,
 * OBEN auf Plattform und Rampe) und prueft je Paar die Konvergenz-Garantie:
 *
 *   Binnen 1500 Frames (50 s) muss der Gator in Kontakt-/Belagerungsnaehe
 *   kommen: min_dist < 4500 (Leon unten, Biss-Anbahnung) bzw. < 6200
 *   (Leon oben: die Belagerungskante der Plattform-Mitte liegt bei ~5700).
 *
 * Jedes Paar, das durchfaellt, ist ein kuenftiger Nutzer-Marker. Der Test
 * bleibt in der Suite und schuetzt jede weitere Wegfindungs-Aenderung.
 *
 * Geometrie-Anker: Raum-SCA x[-8900..7200] z[-27000..-5400]; Plattform
 * x[-1700..1850] z[-20000..-12450]; Rampe x[1850..7200] z[-18100..-14500]
 * (RDT STAGE2/ROOM2090, s. probe_sca_wall_hit.c); Wasserzonen @0x09D6/
 * @0x09EA/@0x09FE. Leon-oben-Hoehe -1800 (floor-Ebene 1 @0x8010c7a4).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_boss_gator.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz;
    return b;
}

/* Insel-Rechtecke (+m) — fuer die Raster-Filterung der Startpunkte. */
static int in_insel(int32_t x, int32_t z, int32_t m)
{
    if (x >= -1700 - m && x <= 1850 + m && z >= -20000 - m && z <= -12450 + m) return 1;
    if (x >=  1850 - m && x <= 7200 + m && z >= -18100 - m && z <= -14500 + m) return 1;
    return 0;
}

typedef struct { int32_t x, z, y; const char *wo; } leon_pos_t;

static int g_nur_gx, g_nur_gz, g_nur_li = -1;   /* Einzelfall-Modus (argv) */

int main(int argc, char **argv)
{
    if (argc >= 4) {                       /* Einzelfall: Telemetrie AN */
        g_nur_gx = atoi(argv[1]); g_nur_gz = atoi(argv[2]); g_nur_li = atoi(argv[3]);
    }
#ifdef _WIN32
    _putenv("RE15_GB_TEST=1");     /* aggro sofort (nur aggro, keine Position) */
    if (g_nur_li < 0) _putenv("RE15_GB_STUMM=1");
#else
    setenv("RE15_GB_TEST", "1", 1);
    if (g_nur_li < 0) setenv("RE15_GB_STUMM", "1", 1);
#endif

    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf) { printf("FAIL: ROOM2090.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) {
        printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* Leon-Positionsmenge: Ufer-Umlauf, Ecken, bekannte Marker-Nischen,
     * Wasser-Mitte, OBEN auf Plattform (y=-1800) und Rampe (y=-1800). */
    static const leon_pos_t LEON[] = {
        /* Ufer/Ecken/Nischen (y=0, watend) */
        { -8400, -26500, 0, "SW-Ecke" },   {  6700, -26500, 0, "SO-Ecke" },
        { -8400,  -5900, 0, "NW-Ecke" },   {  6700,  -5900, 0, "NO-Ecke" },
        { -8200, -25550, 0, "Tuer A" },    {  6732, -20501, 0, "Ostwand-Sued" },
        {  -100,  -8296, 0, "Nord-Spinne" },{  2318, -12797, 0, "Rampen-NW-Wasser" },
        { -8400, -16300, 0, "Westwand" },  {  6700, -10000, 0, "Ostwand-Nord" },
        { -1000, -26500, 0, "Suedwand" },  {  3000,  -5900, 0, "Nordwand" },
        /* Wasser-Mitte */
        { -4000, -22000, 0, "Sued-Mitte" },{  2000, -22000, 0, "Sued-Ost" },
        { -6000, -16300, 0, "Westkanal" }, { -4000, -10000, 0, "Nord-Mitte" },
        {  2000, -10000, 0, "Nord-Ost" },  { -2500, -21000, 0, "Plattform-Sued-nah" },
        { -2500, -11800, 0, "Plattform-Nord-nah" },
        /* OBEN: Plattform */
        { -1000, -19000, -1800, "Platt-SW-oben" }, {   800, -19000, -1800, "Platt-SO-oben" },
        { -1000, -16300, -1800, "Platt-W-oben" },  {   800, -16300, -1800, "Platt-O-oben" },
        { -1000, -13500, -1800, "Platt-NW-oben" }, {   800, -13500, -1800, "Platt-NO-oben" },
        /* OBEN: Rampe */
        {  2600, -17400, -1800, "Rampe-SW-oben" }, {  4400, -17400, -1800, "Rampe-S-oben" },
        {  6200, -17400, -1800, "Rampe-SO-oben" }, {  2600, -15200, -1800, "Rampe-NW-oben" },
        {  4400, -15200, -1800, "Rampe-N-oben" },  {  6200, -15200, -1800, "Rampe-NO-oben" },
    };
    const int NLEON = (int)(sizeof LEON / sizeof LEON[0]);

    int paare = 0, tote = 0;
    for (int li = 0; li < NLEON; li++) {
        for (int32_t gx0 = -8000; gx0 <= 6400; gx0 += 1600) {
            for (int32_t gz0 = -26200; gz0 <= -6200; gz0 += 1600) {
                if (in_insel(gx0, gz0, 300)) continue;   /* kein Start im Block */
                if (g_nur_li >= 0 &&
                    (li != g_nur_li || gx0 != g_nur_gx || gz0 != g_nur_gz)) continue;

                /* Gator-Neuspawn wie main.c (Boss-INIT laeuft im 1. Tick) */
                int slot = RE15_ACTOR_MAX - 1;
                re15_actor_t *e = &g_actors[slot];
                memset(e, 0, sizeof *e);
                e->active = 1; e->type = 0x23u;
                e->x = gx0; e->y = 0; e->z = gz0;
                e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
                re15_enemy_apply_hitbox(e, 0x23u);

                /* Frame 0 = Boss-INIT: der setzt die Position IMMER auf den
                 * Lauerplatz (GB_START) - danach auf den Rasterpunkt setzen,
                 * sonst testet der Sweep nur EINEN Start (so geschehen im
                 * ersten Lauf: identische Attraktoren aus 28 "Starts"). */
                pl->active = 1; pl->type = 0; pl->hp = 100;
                pl->x = LEON[li].x; pl->z = LEON[li].z; pl->y = LEON[li].y;
                re15_gator_boss_tick(slot);
                e->x = gx0; e->z = gz0; e->y = -1200;

                int32_t mind = 0x7fffffff;
                for (int f = 0; f < 1500; f++) {
                    /* Leon = Statue: jede Frame zurueckpinnen (SEG-Push/Bisse
                     * verschieben ihn sonst; der Sweep prueft die WEGFINDUNG) */
                    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
                    pl->state = 0; pl->motion = 0;
                    pl->x = LEON[li].x; pl->z = LEON[li].z; pl->y = LEON[li].y;
                    pl->floor = (uint8_t)(LEON[li].y ? 1 : 0);

                    int32_t ox = e->x, oz = e->z;
                    re15_gator_boss_tick(slot);
                    if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
                        int32_t nx = e->x, nz = e->z;   /* Dispatcher-Klemme 1:1 */
                        re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz,
                                                       e->hit_radius_min, e->y, 4u);
                        e->x = nx; e->z = nz;
                    }
                    {
                        int64_t dx = (int64_t)pl->x - e->x, dz = (int64_t)pl->z - e->z;
                        int64_t d2 = dx * dx + dz * dz;
                        int32_t d = 0;
                        while ((int64_t)d * d < d2 && d < 40000) d += 32;
                        if (d < mind) mind = d;
                    }
                    if (mind < (LEON[li].y ? 6200 : 4500)) break;   /* konvergiert */
                }
                paare++;
                if (mind >= (LEON[li].y ? 6200 : 4500)) {
                    tote++;
                    printf("FAIL: start=(%ld,%ld) leon=%s(%ld,%ld,y=%ld) min_dist=%ld end=(%ld,%ld)\n",
                           (long)gx0, (long)gz0, LEON[li].wo,
                           (long)LEON[li].x, (long)LEON[li].z, (long)LEON[li].y,
                           (long)mind, (long)e->x, (long)e->z);
                    fails++;
                }
                e->active = 0;
            }
        }
    }
    {   /* TAENZEL-SZENARIEN (Nutzer 2026-09-11 "austricksen"): Leon oben
         * springt alle 60 F ueber die Flaechen-Mitte - der Gator muss trotz
         * flippender Seite in Belagerungsnaehe kommen (Seiten-Latch). */
        static const struct { int32_t x, za, zb; const char *wo; } TANZ[] = {
            { 4400, -17500, -15100, "Rampe-Taenzler" },
            {  400, -17600, -15000, "Platt-Taenzler" },
        };
        for (int ti = 0; ti < 2; ti++) {
            for (int32_t gx0 = -8000; gx0 <= 6400; gx0 += 3200) {
                for (int32_t gz0 = -26200; gz0 <= -6200; gz0 += 3200) {
                    if (in_insel(gx0, gz0, 300)) continue;
                    int slot = RE15_ACTOR_MAX - 1;
                    re15_actor_t *e = &g_actors[slot];
                    memset(e, 0, sizeof *e);
                    e->active = 1; e->type = 0x23u;
                    e->x = gx0; e->y = 0; e->z = gz0;
                    e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
                    re15_enemy_apply_hitbox(e, 0x23u);
                    pl->active = 1; pl->type = 0; pl->hp = 100;
                    pl->x = TANZ[ti].x; pl->z = TANZ[ti].za; pl->y = -1800;
                    re15_gator_boss_tick(slot);
                    e->x = gx0; e->z = gz0; e->y = -1200;
                    int32_t mind = 0x7fffffff;
                    for (int f = 0; f < 1800; f++) {
                        pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
                        pl->state = 0; pl->motion = 0;
                        pl->x = TANZ[ti].x; pl->y = -1800; pl->floor = 1;
                        pl->z = ((f / 60) & 1) ? TANZ[ti].zb : TANZ[ti].za;
                        int32_t ox = e->x, oz = e->z;
                        re15_gator_boss_tick(slot);
                        if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
                            int32_t nx = e->x, nz = e->z;
                            re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz,
                                                           e->hit_radius_min, e->y, 4u);
                            e->x = nx; e->z = nz;
                        }
                        {
                            int64_t dx = (int64_t)pl->x - e->x, dz = (int64_t)pl->z - e->z;
                            int64_t d2 = dx * dx + dz * dz;
                            int32_t d = 0;
                            while ((int64_t)d * d < d2 && d < 40000) d += 32;
                            if (d < mind) mind = d;
                        }
                        if (mind < 6200) break;
                    }
                    paare++;
                    if (mind >= 6200) {
                        tote++;
                        printf("FAIL: TANZ %s start=(%ld,%ld) min_dist=%ld end=(%ld,%ld)\n",
                               TANZ[ti].wo, (long)gx0, (long)gz0,
                               (long)mind, (long)e->x, (long)e->z);
                        fails++;
                    }
                    e->active = 0;
                }
            }
        }
    }
    printf("gator-sweep: %d Paare, %d nicht konvergiert\n", paare, tote);
    free(buf);
    if (fails) { printf("%d FAILURES\n", fails); return 1; }
    printf("OK\n");
    return 0;
}
