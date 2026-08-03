/* probe_bite_fx.c — DIAGNOSE (kein ctest): misst, ob die Biss-/Release-/Devour-Blut-Spawns
 * (bite_blood_fx.md D1/D3/D4) im Grab-Ablauf tatsaechlich feuern. Zaehlt re15_esp_fx_count()
 * pro Tick. Bank-los (engine-only): ohne Modell-Bank ist clip_done=1 => der Biss-Zyklus wrappt
 * jeden Tick — die KADENZ ist hier also verdichtet; gemessen wird, DASS die Spawns im richtigen
 * Zweig feuern (Biss-Zweig / Release / Devour-Entry / Frame-0x37-Burst). */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_player.h"
#include "re15_esp.h"

int main(void)
{
    re15_actor_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0;

    re15_actor_t *z = &g_actors[1];
    memset(z, 0, sizeof *z);
    z->active = 1; z->type = 0x10; z->flags = 1; z->hp = 200;
    z->x = 300; z->z = 0; z->em_flag_id = 0xFF;
    re15_enemy_ai_live_init(1);
    z->hp = 200;

    /* Grab erzwingen: state 1, +0x5=3 (face), +0x6=0 -> die FSM laeuft [0]->[1]->[2]->[3] */
    z->state = 1; z->sub_state_1 = 3; z->sub_state_2 = 0;

    int prev = re15_esp_fx_count(), bite_spawns = 0, rel_spawns = 0;
    int prev_ss2 = 0;
    printf("== Grab-Ablauf (bank-los, Kadenz verdichtet) ==\n");
    for (int f = 0; f < 40 && z->sub_state_1 >= 3 && z->sub_state_1 <= 4 && z->state == 1; f++) {
        re15_enemy_ai_live_tick(1);
        re15_player_victim_tick();
        int now = re15_esp_fx_count();
        if (now > prev) {
            if (z->sub_state_2 == 3 || prev_ss2 == 3) bite_spawns += now - prev;
            else rel_spawns += now - prev;
            printf("  f%02d ss2=%d: +%d FX (gesamt %d) hp=%d\n", f, z->sub_state_2, now - prev, now, pl->hp);
        }
        prev = now; prev_ss2 = z->sub_state_2;
        if (f == 5) {                        /* Mash-Escape frueh ausloesen -> Throw-off [4] */
            z->ai_timer = -1;
        }
    }
    printf("Biss-Zweig-Spawns: %d, Release-Zweig-Spawns: %d\n", bite_spawns, rel_spawns);
    printf("Wund-Level nach Release: p0=%d p5=%d p7=%d (Akku-Stand nach 1 Grab: 10/50/50)\n",
           re15_wound_level(0), re15_wound_level(5), re15_wound_level(7));

    /* Devour-Pfad: neuer Grab, kein Mash -> kill_ctr ablaufen lassen */
    pl->hp = 100; pl->hit_react = 0; pl->state = 0;
    z->state = 1; z->sub_state_1 = 3; z->sub_state_2 = 0;
    int before_devour = re15_esp_fx_count();
    for (int f = 0; f < 300 && pl->state != 7; f++) {
        re15_enemy_ai_live_tick(1);
        re15_player_victim_tick();
    }
    printf("Devour: FX %d -> %d (Entry- + Frame-Burst enthalten), player state=%d hp=%d\n",
           before_devour, re15_esp_fx_count(), pl->state, pl->hp);
    return 0;
}
