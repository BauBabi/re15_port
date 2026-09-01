/* probe_hund_bellen.c — MESSUNG zum Nutzer-Befund 2026-08-26 "Zombie Hunde barken nicht".
 *
 * Kein Fix, nur Zaehlung: einen Cerberus (Typ 0x20) gegen einen Spieler laufen lassen und
 * ueber den SE-Spion des Harnischs (g_test_room_se_log, test_support.c) protokollieren,
 * WELCHE SE-Ids in welchem Zustand fallen. Interessant sind:
 *    SE 0 Knurren (sub 6)        SE 2 BELLEN (sub 4)     SE 6 Trittschall
 *    SE 8 Anlauf (sub 5)         SE 3 Treffer/Latch      SE 5/7 Kampf
 * Das Bell-Tor ist byte-true eng (@0x8010e194-1c8, selbst disassembliert):
 *    lh v0,486(v1)   / bne  -> +0x1E6 Cooldown muss 0 sein
 *    slti v0,v0,81   / bne  -> Spieler-HP muss > 80 sein
 *    slti v0,v0,7001 / bne  -> Distanz muss >= 7001 sein
 *    andi v0,v0,0x1  / beq  -> +0x1D0 Bit 0
 *    zwei rng-Zuege, mult   -> (r1&1)*(r2&1), also nur jeder 4. Versuch
 * KEIN add_test. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_math.h"

extern int g_test_room_se_log[];
extern int g_test_room_se_tickof[];
extern int g_test_room_se_n;
extern int g_test_room_se_tick;

static void face(re15_actor_t *e, const re15_actor_t *t)
{
    e->rot_y = (int16_t)(((int)re15_atan2_q12(t->z - e->z, t->x - e->x) - 0x400) & 0xfff);
}

static void lauf(const char *titel, int32_t startdist, int frames, int spieler_bewegt)
{
    memset(g_actors, 0, sizeof g_actors);
    g_test_room_se_n = 0; g_test_room_se_tick = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = startdist; pl->hp = 100;
    re15_actor_t *d = &g_actors[1];
    d->active = 1; d->type = 0x20; d->state = 0; d->x = 0; d->y = 0; d->z = 0;
    d->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(d, 0x20);

    int se_zaehler[16]; memset(se_zaehler, 0, sizeof se_zaehler);
    int sub_gesehen[16]; memset(sub_gesehen, 0, sizeof sub_gesehen);
    for (int t = 0; t < frames; t++) {
        g_test_room_se_tick = t;
        face(d, pl);
        re15_enemy_ai_run_all(0);
        if (d->sub_state_1 < 16) sub_gesehen[d->sub_state_1]++;
        if (spieler_bewegt && (t % 3) == 0 && pl->z > 1200) pl->z -= 40;
    }
    for (int i = 0; i < g_test_room_se_n; i++)
        if (g_test_room_se_log[i] >= 0 && g_test_room_se_log[i] < 16)
            se_zaehler[g_test_room_se_log[i]]++;

    printf("%s (Start-Distanz %d, %d Bilder)\n", titel, startdist, frames);
    printf("   SE gesamt: %d  ->", g_test_room_se_n);
    for (int i = 0; i < 16; i++) if (se_zaehler[i]) printf("  SE%d x%d", i, se_zaehler[i]);
    printf("%s\n", se_zaehler[2] ? "" : "   <<< KEIN BELLEN (SE2)");
    printf("   Sub-Zustaende:");
    for (int i = 0; i < 16; i++) if (sub_gesehen[i]) printf("  %d:%d", i, sub_gesehen[i]);
    printf("\n");
}

int main(void)
{
    printf("=== Hunde-SE-Zensus (Nutzer: \"Zombie Hunde barken nicht\") ===\n");
    lauf("A) Spieler nah, statisch",        2000, 1200, 0);
    lauf("B) Spieler weit, statisch",       9000, 1200, 0);
    lauf("C) Spieler weit, laeuft heran",   9000, 1200, 1);
    lauf("D) sehr weit, statisch",         15000, 2400, 0);
    return 0;
}
