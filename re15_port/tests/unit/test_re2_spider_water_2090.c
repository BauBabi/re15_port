/* test_re2_spider_water_2090.c — PIN fuer den WASSER-KANAL der RE2-Spinne (FUN_800527B4).
 *
 * Der Kanal war bis 2026-08-20 nicht verdrahtet ("im Port immer 0 = kein Wasser"). Dieser
 * Test pinnt beide Haelften an den ECHTEN ausgelieferten Raumdaten fest:
 *
 *   POSITIV — STAGE2/ROOM2090.RDT stellt in main00 (Datei 0x994..0xADC) DREI sce-8-Wasser-
 *     rechtecke und DAHINTER seine beiden einzigen Adult-Spinnen mitten hinein:
 *       Aot_set    @0x09D6 `2c 02 08 47 00 00 3c dd 88 96 e4 3e c4 22 ac f9`
 *                  Slot 2 sce 8 flags 0x47 band 0  x[-8900..7200]  z[-27000..-18100] p0 -1620
 *       Aot_set    @0x09EA Slot 3 sce 8 band 0     x[-8900..7200]  z[-14500.. -5400] p0 -1620
 *       Aot_set    @0x09FE Slot 4 sce 8 band 0     x[-8900..-1700] z[-18100..-14500] p0 -1620
 *       Sce_em_set @0x0AB0 `44 00 25 00 00 00 00 73 9c ff 00 00 98 df` Slot 0 Typ 0x25
 *                  floor 0 -> (-100, 0, -8296)   -> in Wasserzone Slot 3
 *       Sce_em_set @0x0AC4 `44 01 25 00 00 00 00 74 04 f7 00 00 a8 a2` Slot 1 Typ 0x25
 *                  floor 0 -> (-2300, 0, -23896) -> in Wasserzone Slot 2
 *
 *   MECHANISMUS-BELEG (alles selbst disassembliert):
 *     RE1.5 AOT-Dispatch @0x8007469c[8] = 0x8004330c: `lhu v0,0(a0); sh v0,136(entity)` (+0x88)
 *     RE2   AOT-Dispatch @0x800A73C4[7] = 0x80051a2c: `lhu v0,0(a0); sh v0,268(entity)` (+0x10C)
 *       -> dieselbe Zonenklasse, andere sce-Nummer und anderes Entity-Feld.
 *     RE2   FUN_800527B4 @0x800527B4-0x800528C8 = der PULL ueber genau diese Zonen
 *       (`lbu v1,0(s0)` == 7 @0x80052804-08, Rechteck rec+4 / Nutzlast rec+12 @0x8005289c-ac,
 *        Nicht-Treffer 0 @0x800528C4) — Port-Zwilling re15_aot_water_at.
 *     Beide Spiele behandeln die Stempel-Kopie als PRO-FRAME-Wert: RE1.5 nullt +0x88 nach
 *       jedem Gegner-Handler (@0x8001a59c) und fuer den Spieler (@0x8002c154), RE2 nullt
 *       +0x10C unmittelbar vor dem Scan-Aufruf (@0x800526dc / @0x80052788).
 *
 *   NEGATIV — derselbe Aufbau in einem Raum OHNE sce-8-Zone muss 0 liefern (der Kanal darf
 *     nicht "immer an" sein), und ein Punkt ausserhalb der Rechtecke ebenfalls 0. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"
#include "re15_player.h"     /* re15_actors_anim_advance */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *rel, long *out_sz)
{
    char p[512];
    snprintf(p, sizeof p, "%s%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* Raum hochfahren: main00 + sub00, dann 120 VM-Ticks (wie der ROOM1090-A/B-Test). */
static int bringup(const re15_rdt_t *rdt)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    if (rdt->main_scd) scd_thread_start(0, rdt->main_scd);
    if (rdt->sub_scd[0]) scd_thread_start(1, rdt->sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    return 0;
}

int main(void)
{
    long sz = 0;
    uint8_t *buf = slurp("/STAGE2/ROOM2090.RDT", &sz);
    CHECK(buf != NULL, "STAGE2/ROOM2090.RDT muss lesbar sein");
    if (!buf) return 1;

    /* ---- 1. ROHDATEN direkt aus der Datei (unabhaengig von jedem Walker) ------------------ */
    {
        /* Die drei Aot_set-Records und die zwei Sce_em_set-Records liegen an festen
         * Datei-Offsets; hier byte-weise gepinnt, damit ein kuenftiger Parser-Umbau nicht
         * unbemerkt die Grundlage dieses Tests verschiebt. */
        static const struct { long off; uint8_t b[8]; } k_raw[] = {
            { 0x09D6, { 0x2c, 0x02, 0x08, 0x47, 0x00, 0x00, 0x3c, 0xdd } },
            { 0x09EA, { 0x2c, 0x03, 0x08, 0x47, 0x00, 0x00, 0x3c, 0xdd } },
            { 0x09FE, { 0x2c, 0x04, 0x08, 0x47, 0x00, 0x00, 0x3c, 0xdd } },
            { 0x0AB0, { 0x44, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x73 } },
            { 0x0AC4, { 0x44, 0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x74 } },
        };
        for (unsigned i = 0; i < sizeof k_raw / sizeof k_raw[0]; i++) {
            CHECK(k_raw[i].off + 8 <= sz && memcmp(buf + k_raw[i].off, k_raw[i].b, 8) == 0,
                  "ROOM2090.RDT @0x%04lX: Rohbytes des Records weichen ab", k_raw[i].off);
        }
        /* Wasser-Y = Payload-Halbwort pc[14..15] jedes Aot_set, LE, alle drei gleich. */
        for (long off = 0x09D6; off <= 0x09FE; off += 0x14) {
            int16_t p0 = (int16_t)((uint16_t)buf[off + 14] | ((uint16_t)buf[off + 15] << 8));
            CHECK(p0 == -1620, "ROOM2090.RDT @0x%04lX: Wasser-Y p0 muss -1620 sein, ist %d", off, (int)p0);
        }
        /* Spinnen-Positionen: LE s16 bei pc[8..13]. */
        int16_t x0 = (int16_t)((uint16_t)buf[0x0AB0 + 8] | ((uint16_t)buf[0x0AB0 + 9] << 8));
        int16_t z0 = (int16_t)((uint16_t)buf[0x0AB0 + 12] | ((uint16_t)buf[0x0AB0 + 13] << 8));
        int16_t x1 = (int16_t)((uint16_t)buf[0x0AC4 + 8] | ((uint16_t)buf[0x0AC4 + 9] << 8));
        int16_t z1 = (int16_t)((uint16_t)buf[0x0AC4 + 12] | ((uint16_t)buf[0x0AC4 + 13] << 8));
        CHECK(x0 == -100 && z0 == -8296, "Spinne Slot 0 @0x0AB0 = (-100,-8296), ist (%d,%d)", x0, z0);
        CHECK(x1 == -2300 && z1 == -23896, "Spinne Slot 1 @0x0AC4 = (-2300,-23896), ist (%d,%d)", x1, z1);
    }

    /* ---- 2. Raum laden: Zonen + Spawns wie erwartet ---------------------------------------- */
    re15_rdt_t rdt;
    CHECK(re15_rdt_parse(buf, (size_t)sz, &rdt) == 0, "ROOM2090.RDT muss parsen");
    bringup(&rdt);

    int water_zones = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        if (!g_aot.slots[i].active) continue;
        if (g_aot.slots[i].type != RE15_AOT_TYPE_WATER) continue;
        water_zones++;
        CHECK(g_aot.env_params[i].p0 == -1620,
              "Wasserzone Slot %d: p0 muss -1620 sein, ist %d", i, (int)g_aot.env_params[i].p0);
        CHECK(g_aot.slots[i].sce_flags == 0x47,
              "Wasserzone Slot %d: flags 0x47 (Pools 0x07 + CENTRE 0x40), ist 0x%02X",
              i, g_aot.slots[i].sce_flags);
    }
    CHECK(water_zones == 3, "ROOM2090 main00 installiert 3 sce-8-Zonen, gezaehlt %d", water_zones);

    int spiders[8], n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && n < 8; s++)
        if (g_actors[s].active && g_actors[s].type == 0x25) spiders[n++] = s;
    CHECK(n == 2, "ROOM2090 stellt GENAU 2 Adult-Spinnen auf, gezaehlt %d", n);

    /* ---- 3. Der PULL (== FUN_800527B4) liefert an beiden Spawn-Punkten das Wasser-Y ------- */
    CHECK(re15_aot_water_at(-100, -8296) == -1620,
          "FUN_800527B4(-100,-8296) muss -1620 sein (Zone Slot 3), ist %d",
          (int)re15_aot_water_at(-100, -8296));
    CHECK(re15_aot_water_at(-2300, -23896) == -1620,
          "FUN_800527B4(-2300,-23896) muss -1620 sein (Zone Slot 2), ist %d",
          (int)re15_aot_water_at(-2300, -23896));
    /* NEGATIV: ausserhalb aller drei Rechtecke -> Nicht-Treffer-Wert 0 (@0x800528C4). */
    CHECK(re15_aot_water_at(100000, 100000) == 0,
          "NEGATIV: ausserhalb jeder Zone muss FUN_800527B4 0 liefern");
    CHECK(re15_aot_water_at(-100, 0) == 0,
          "NEGATIV: z = 0 liegt hinter der Nordkante aller drei Zonen -> 0");

    /* ---- 4. Der Engine-Stempel erreicht den RE2-Spinnen-Tick ------------------------------- */
    {
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->hp = 100; pl->floor = 0;
        pl->x = -100; pl->y = 0; pl->z = -8296;
        re15_aot_scan(pl->x, pl->z, 0);
        for (int i = 0; i < n; i++)
            CHECK(g_actors[spiders[i]].water_y == -1620,
                  "sce-8-Stempel (+0x88, LAB_8004330c) auf Spinne %d, ist %d",
                  spiders[i], (int)g_actors[spiders[i]].water_y);

        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        for (int i = 0; i < n; i++)
            CHECK(re15_re2spider_owns(&g_actors[spiders[i]]) == 1,
                  "RE2-Brain muss Spinne %d besitzen", spiders[i]);
        for (int f = 0; f < 3; f++) {
            re15_enemy_ai_run_all(1);
            re15_actors_anim_advance();
        }
        for (int i = 0; i < n; i++)
            CHECK(g_actors[spiders[i]].re2s_water10c == -1620,
                  "+0x10C (@0x80051a3c) muss am Modul-Kopf -1620 tragen, Spinne %d hat %d",
                  spiders[i], (int)g_actors[spiders[i]].re2s_water10c);
    }
    free(buf);

    /* ---- 5. NEGATIV: ein Raum OHNE sce-8-Zone laesst den Kanal bei 0 ----------------------- */
    {
        long sz2 = 0;
        uint8_t *b2 = slurp("/STAGE1/ROOM1090.RDT", &sz2);   /* der STAGE1-Spinnenraum, trocken */
        CHECK(b2 != NULL, "STAGE1/ROOM1090.RDT muss lesbar sein");
        if (b2) {
            re15_rdt_t r2;
            if (re15_rdt_parse(b2, (size_t)sz2, &r2) == 0) {
                bringup(&r2);
                int dry = 0;
                for (int i = 0; i < RE15_AOT_MAX; i++)
                    if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_WATER) dry++;
                CHECK(dry == 0, "NEGATIV: ROOM1090 hat keine sce-8-Zone, gezaehlt %d", dry);
                re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                pl->active = 1; pl->hp = 100; pl->floor = 0;
                /* ROOM1090 stellt seine Spinnen erst spaeter im Skript auf; fuer den Negativ-
                 * Test wird derselbe Aktor-Typ AN DENSELBEN ROOM2090-Koordinaten in den
                 * trockenen Raum gesetzt — der Unterschied ist dann NUR die fehlende Zone. */
                re15_actor_t *sp = &g_actors[1];
                memset(sp, 0, sizeof *sp);
                sp->active = 1; sp->type = 0x25; sp->state = 0; sp->floor = 0;
                sp->x = -100; sp->y = 0; sp->z = -8296;
                re15_aot_scan(pl->x, pl->z, 0);
                CHECK(sp->water_y == 0,
                      "NEGATIV: trockener Raum -> kein sce-8-Stempel, water_y ist %d",
                      (int)sp->water_y);
                CHECK(re15_aot_water_at(-100, -8296) == 0,
                      "NEGATIV: FUN_800527B4 muss im trockenen Raum 0 liefern");
                re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
                CHECK(re15_re2spider_owns(sp) == 1, "NEGATIV-Aufbau: RE2-Brain muss die Spinne besitzen");
                for (int f = 0; f < 3; f++) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }
                CHECK(sp->re2s_water10c == 0,
                      "NEGATIV: trockener Raum -> +0x10C bleibt 0, ist %d", (int)sp->re2s_water10c);
            }
            free(b2);
        }
    }

    printf("test_re2_spider_water_2090: %d FAIL\n", fails);
    return fails ? 1 : 0;
}
