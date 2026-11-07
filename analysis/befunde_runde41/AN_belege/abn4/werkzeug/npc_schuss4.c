/* npc_schuss4.c - Abnahme 4 Spur AN (Runde 41): Waffen-Resolver nach NB5 (RE2-Regel "Schuss geht durch Begleiter").
 * Nur Messwerkzeug (kein Port-Code), linkt gegen libre15_engine.a des Arbeitsbaums r41_annette (Bau 2026-11-03 10:29:52 = Kopf
 * cefd57402). Aufbau wie Abnahme 3 npc_schuss.c: Leon (0,0) Gier 2048 (Blick -x), NPC bei x -3000, Gegner bei x -6000.
 * Je Fall 5 Schuesse; zwischen den Schuessen wird nur beim Gegner +0x93 Bit 0 geloescht.
 * Faelle: alle NPC-Typen 0x40..0x4D (hp -1) vor einem Zombie 0x10 (hp 100) fuer Pistole 3, Magnum 7, Schrot 8, Waffe 12/13/19
 * (Schuss-Streifen); NPC 0x40 mit hp 0 (RE2: hp >= 0 ist Kandidat); Gegner 0x10 mit hp -1 vor einem Zombie (RE1.5-Semantik fuer
 * Typen < 0x40 unveraendert); Annette 0x4D: Kasten nach NB5. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

static void fall(const char *name, int waffe, int npc_typ, int16_t npc_hp, int npc_z, uint8_t feind_typ, int16_t feind_hp)
{
    re15_enemy_reset();
    re15_actor_init();
    for (int i = 1; i < RE15_ACTOR_MAX; i++) g_actors[i].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 2048;
    re15_actor_t *n = &g_actors[1];
    memset(n, 0, sizeof *n);
    if (npc_typ >= 0) {
        n->active = 1; n->type = (uint8_t)npc_typ; n->hp = npc_hp; n->x = -3000; n->y = 0; n->z = npc_z; n->rot_y = 0;
        re15_enemy_apply_hitbox(n, (uint8_t)npc_typ);
    }
    re15_actor_t *f = &g_actors[2];
    memset(f, 0, sizeof *f);
    f->active = 1; f->type = feind_typ; f->hp = feind_hp; f->x = -6000; f->y = 0; f->z = 0; f->rot_y = 0;
    re15_enemy_apply_hitbox(f, feind_typ);
    int tn = 0, tf = 0, leer = 0; int16_t n0 = n->hp; uint8_t s0 = n->state; int16_t f0 = f->hp;
    for (int k = 1; k <= 5; k++) {
        int r = re15_player_weapon_fire(waffe);
        if (r == 2) tn++; else if (r == 3) tf++; else leer++;
        f->hit_react = (uint8_t)(f->hit_react & ~1u);
    }
    printf("%-34s Waffe %2d | NPC %s Kasten r %d h %d: hp %d->%d st %d->%d | Gegner 0x%02X hp %d->%d | Ziel NPC %d, Gegner %d, leer %d\n",
           name, waffe, npc_typ >= 0 ? "ja" : "--", npc_typ >= 0 ? (int)n->hit_radius_min : 0, npc_typ >= 0 ? (int)n->hit_height : 0,
           (int)n0, (int)n->hp, (int)s0, (int)n->state, feind_typ, (int)f0, (int)f->hp, tn, tf, leer);
}

int main(void)
{
    static const int typen[] = { 0x40, 0x42, 0x45, 0x47, 0x49, 0x4b, 0x4d };
    static const int waffen[] = { 3, 7, 8, 12, 13, 19 };
    char nm[64];
    for (unsigned w = 0; w < sizeof waffen / sizeof waffen[0]; w++) {
        fall("Kontrolle ohne NPC", waffen[w], -1, 0, 0, 0x10, 100);
        for (unsigned i = 0; i < sizeof typen / sizeof typen[0]; i++) {
            snprintf(nm, sizeof nm, "NPC 0x%02X hp -1 davor", typen[i]);
            fall(nm, waffen[w], typen[i], -1, 0, 0x10, 100);
        }
    }
    fall("NPC 0x40 hp 0 davor (RE2: Kandidat)", 3, 0x40, 0, 0, 0x10, 100);
    fall("Zombie 0x10 hp -1 davor (RE1.5)", 3, 0x10, -1, 0, 0x10, 100);
    fall("NPC 0x4D hp -1 quer 1300", 3, 0x4d, -1, 1300, 0x10, 100);
    return 0;
}
