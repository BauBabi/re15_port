/* test_za_spawnvariante.c — RIEGEL Runde 41 Spur ZA: die Zombie-Spawn-Variante (abgemagerte Gliedmassen) ist im
 * Original UNSICHTBAR und deshalb im Port fuer ALLE Zombie-Typen abgeschaltet (Dossier
 * analysis/befunde_runde41/ZA_zombiearm.md).
 *
 * Original (STAGE1.BIN, byte-gleich auf Version_40%): INIT FUN_80100688 wuerfelt @0x801008b0 (`jal rng`,
 * `andi v0,v0,0x3` @0x801008bc, `bne` @0x801008c0) und ruft bei 0 @0x801008c8-ec eine der Routinen aus
 * @0x8011f7ec. Die Routinen setzen Teil-Wort |= 0x400|0x80 + Skala + Tinte; der Zeichner skaliert nur bei
 * `andi v0,v0,0x400` @0x8001ebb0 (ScaleMatrix @0x8001ebdc). In allen 26 sauberen Savestates mit Zombies steht
 * die Skala/Tinte der Variante im Teil-Record, das Teil-Wort aber auf 0x00000001 -> nichts wird gezeichnet.
 *
 * PIN 1  Fuer jeden Zombie-Typ 0x10/0x11/0x12/0x16/0x18 und 400 Seeds: nach re15_enemy_ai_live_init ist die
 *        Teilschicht AUS (re15_teile_an == 0), jedes Teil-Wort 1, jede Skala 0x1000, keine Tinte.
 * PIN 2  Gegenprobe Zufallsstrom: die Wuerfe @0x801008b0 (immer) und @0x801008c8 (bei rng&3 == 0) bleiben —
 *        der naechste Wurf nach dem INIT ist derselbe wie bei Poise/Stagger/HP/Varianten-Wurf von Hand.
 * PIN 3  Gegenprobe Abdeckung: in ~25 % der Seeds faellt der Varianten-Wurf auf 0 (sonst prueft PIN 1 nichts).
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"
#include "re15_teile_r15.h"

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

#define Z 3

static void zombie(uint8_t type)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl); pl->active = 1; pl->hp = 100; pl->x = 0; pl->z = -3000;
    re15_actor_t *e = &g_actors[Z];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = type; e->grid_id = 0; e->em_flag_id = 0xFF;
    re15_teile_reset(Z);
}

int main(void)
{
    static const uint8_t typen[5] = { 0x10, 0x11, 0x12, 0x16, 0x18 };
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    for (int ti = 0; ti < 5; ti++) {
        uint8_t typ = typen[ti];
        int wurf0 = 0, n = 0, markiert = 0, skaliert = 0, strom = 0;
        for (uint32_t seed = 1; seed <= 400; seed++) {
            zombie(typ);
            re15_damage_seed_rng(seed);
            re15_enemy_ai_live_init(Z);
            uint8_t nach_init = re15_engine_rand8();

            /* Erwartung von Hand: Poise @0x80100824, Stagger @0x80100774, HP @0x801007bc, Variante @0x801008b0
             * (+ @0x801008c8). Die Reihenfolge im Port: Poise, Stagger, HP, Variante. */
            re15_damage_seed_rng(seed);
            (void)re15_engine_rand8(); (void)re15_engine_rand8(); (void)re15_engine_rand8();
            uint8_t v = re15_engine_rand8();
            if ((v & 3) == 0) { wurf0++; (void)re15_engine_rand8(); }
            uint8_t erwartet = re15_engine_rand8();
            if (nach_init != erwartet) strom++;

            n++;
            if (re15_teile_an(Z)) markiert++;
            const re15_teil_t *t = re15_teile(Z);
            for (int k = 0; t && k < 15; k++) {
                if (t[k].w != 1 || t[k].scale[0] != 0x1000 || t[k].scale[1] != 0x1000 || t[k].scale[2] != 0x1000
                    || t[k].h94[0] != 0 || t[k].h94[2] != 0) { skaliert++; break; }
            }
        }
        printf("Typ %02x: %d Seeds, Varianten-Wurf 0 in %d, Teilschicht an %d, Teil veraendert %d, Strom-Abweichung %d\n",
               typ, n, wurf0, markiert, skaliert, strom);
        CHECK(markiert == 0, "PIN 1 Typ %02x: Teilschicht in %d von %d Spawns an (Spawn-Variante aktiv)", typ, markiert, n);
        CHECK(skaliert == 0, "PIN 1 Typ %02x: Teil-Wort/Skala/Tinte in %d von %d Spawns veraendert", typ, skaliert, n);
        CHECK(strom == 0, "PIN 2 Typ %02x: Zufallsstrom nach dem INIT in %d Seeds verschoben", typ, strom);
        CHECK(wurf0 >= n / 8 && wurf0 <= n / 2, "PIN 3 Typ %02x: Varianten-Wurf 0 nur %d von %d (erwartet ~25 %%)", typ, wurf0, n);
    }
    if (fails) { fprintf(stderr, "test_za_spawnvariante: %d Fehler\n", fails); return 1; }
    printf("test_za_spawnvariante: OK\n");
    return 0;
}
