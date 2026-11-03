/* npc_schuss.c - Abnahme 3 Spur AN (Runde 41): Waffen-Resolver mit lebendem NPC im Schusskeil.
 * Nur Messwerkzeug (kein Port-Code). Linkt gegen libre15_engine.a des Arbeitsbaums; als Variante "vor NB4" wird
 * re15_damage.c aus b646c72a6^ vorangestellt (gleiche Symbole, die Bibliotheks-Fassung wird dann nicht gezogen).
 *
 * Aufbau je Fall: Leon bei (0,0,0) Gier 2048 (Blick -x, wie test_r41_annette test_schuss: Gier 2049 = Westen),
 * NPC (Typ T, hp -1, Kasten re15_enemy_apply_hitbox) bei x -3000, Gegner bei x -6000 (beide auf der Schusslinie).
 * 5 Pistolenschuesse (Waffe 3). Zwischen den Schuessen wird NUR beim Gegner +0x93 Bit 0 geloescht (wie es seine KI
 * im HURT-Ausgang tut); das NPC bleibt unberuehrt. Ausgabe je Schuss: Rueckgabe, NPC hp/st/+0x93, Gegner hp/st.
 * Zusatz: NPC seitlich versetzt (z 900 / 2000), NPC hinter dem Gegner, Kontrolle ohne NPC.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

static void fall(const char *name, int npc_typ, int npc_x, int npc_z, uint8_t feind_typ, int16_t feind_hp)
{
    re15_enemy_reset();
    re15_actor_init();
    for (int i = 1; i < RE15_ACTOR_MAX; i++) g_actors[i].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 2048;
    re15_actor_t *n = &g_actors[1];
    memset(n, 0, sizeof *n);
    if (npc_typ >= 0) {
        n->active = 1; n->type = (uint8_t)npc_typ; n->hp = -1; n->x = npc_x; n->y = 0; n->z = npc_z; n->rot_y = 0;
        re15_enemy_apply_hitbox(n, (uint8_t)npc_typ);
    }
    re15_actor_t *f = &g_actors[2];
    memset(f, 0, sizeof *f);
    f->active = 1; f->type = feind_typ; f->hp = feind_hp; f->x = -6000; f->y = 0; f->z = 0; f->rot_y = 0;
    re15_enemy_apply_hitbox(f, feind_typ);
    printf("%-28s NPC %s (%d,%d) Kasten r %d | Gegner 0x%02X hp %d Kasten r %d\n", name,
           npc_typ >= 0 ? "ja" : "--", npc_x, npc_z, npc_typ >= 0 ? (int)n->hit_radius_min : 0,
           feind_typ, feind_hp, (int)f->hit_radius_min);
    int treffer_npc = 0, treffer_feind = 0, leer = 0;
    for (int k = 1; k <= 5; k++) {
        int r = re15_player_weapon_fire(3);
        if (r == 2) treffer_npc++; else if (r == 3) treffer_feind++; else leer++;
        printf("   Schuss %d: Rueckgabe %d | NPC hp %d st %d +0x93 %02X | Gegner hp %d st %d\n", k, r,
               npc_typ >= 0 ? n->hp : 0, npc_typ >= 0 ? n->state : 0, npc_typ >= 0 ? n->hit_react : 0, f->hp, f->state);
        f->hit_react = (uint8_t)(f->hit_react & ~1u);
    }
    printf("   SUMME %s: NPC getroffen %d, Gegner getroffen %d, ins Leere %d\n", name, treffer_npc, treffer_feind, leer);
}

int main(void)
{
    static const int typen[] = { 0x40, 0x42, 0x45, 0x47, 0x49, 0x4b, 0x4d, 0x41, 0x43, 0x44, 0x46, 0x48, 0x4a, 0x4c };
    char nm[64];
    fall("Kontrolle ohne NPC, G5", -1, 0, 0, 0x36, 345);
    fall("Kontrolle ohne NPC, Zombie", -1, 0, 0, 0x10, 100);
    for (unsigned i = 0; i < sizeof typen / sizeof typen[0]; i++) {
        snprintf(nm, sizeof nm, "NPC 0x%02X davor, G5", typen[i]);
        fall(nm, typen[i], -3000, 0, 0x36, 345);
    }
    fall("NPC 0x40 davor, Zombie", 0x40, -3000, 0, 0x10, 100);
    fall("NPC 0x40 davor quer 900, G5", 0x40, -3000, 900, 0x36, 345);
    fall("NPC 0x40 davor quer 2000, G5", 0x40, -3000, 2000, 0x36, 345);
    fall("NPC 0x40 dahinter, G5", 0x40, -9000, 0, 0x36, 345);
    return 0;
}
