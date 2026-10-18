/*
 * heliport1171_r39.c — Runde 39, Spur K "heliport1171": NPC-INIT-Pose (K2). Herleitung und alle Adressen:
 * include/re15_heliport1171_r39.h, Dossier analysis/befunde_runde39/K_heliport1171.md Abschnitt 6.
 */
#include "re15_heliport1171_r39.h"
#include <string.h>

/* Typ-Byte des Aktors, der seit seiner INIT noch keinen anim_set auf einem anderen Kanal hatte; 0 = kein Merker. */
static uint8_t s_init_typ[RE15_ACTOR_MAX];

static int slot_ok(int slot) { return slot >= 1 && slot < RE15_ACTOR_MAX; }

void re15_r39k_init_pose_setzen(int slot, uint8_t typ)
{
    if (slot_ok(slot)) s_init_typ[slot] = typ;
}

void re15_r39k_init_pose_loesen(int slot)
{
    if (slot_ok(slot)) s_init_typ[slot] = 0;
}

int re15_r39k_init_pose(const re15_actor_t *a)
{
    if (!a) return 0;
    const int slot = (int)(a - g_actors);
    if (!slot_ok(slot) || !s_init_typ[slot] || s_init_typ[slot] != a->type) return 0;
    if (a->type < 0x40 || a->type > 0x4d) return 0;                     /* NPC-Familie (Tabelle @0x80072bac) */
    if (a->state != 0 && a->state != 1 && a->state != 4) return 0;      /* INIT / Eskorte / Executor vor dem 1. Takt */
    /* Die INIT-Pose ist +0x94 = 2 / +0x95 = 0 (@0x8011de7c/@0x8011de8c). Hat ein anderer Port-Pfad den Aktor
     * inzwischen selbst animiert (Clip oder Bild veraendert) oder laeuft der Plc_dest-Walker, gilt sie nicht mehr. */
    if (a->walk_active || a->motion != RE15_R39K_INIT_CLIP || a->anim_frame != RE15_R39K_INIT_BILD) return 0;
    return 1;
}

void re15_r39k_install(uint16_t room)
{
    (void)room;
    memset(s_init_typ, 0, sizeof s_init_typ);
}
