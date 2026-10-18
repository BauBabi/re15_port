/*
 * heliport1171_r39.c — Runde 39, Spur K "heliport1171": NPC-INIT-Pose (K2). Herleitung und alle Adressen:
 * include/re15_heliport1171_r39.h, Dossier analysis/befunde_runde39/K_heliport1171.md Abschnitt 6.
 */
#include "re15_heliport1171_r39.h"
#include "re15_szenenleon_r36.h"   /* re15_szl_schub_spieler_ende: Sub 0x13 endet mit +0x5 = 6 (@0x80052be0) */
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

/* ===================================================================================================================
 * NACHBESSERUNG 1 — Spieler-Sub 6 (Event-Reach 0x800517f0): Saattakt posiert Bild 0, Start im Bild nach dem Dispatcher.
 * Belege: include/re15_heliport1171_r39.h (M1/M3a). Der Port fuehrt Sub 6 in game_step_common.c
 * (re15_player_event_reach_*); hier liegt nur der Saat-Zustand.
 * =================================================================================================================== */
static int s_reach_saat = 0;   /* 0 = Sub 6 laeuft (oder aus) / 1 = Phase 0 im naechsten Event-Reach-Takt /
                                * 2 = Dispatcher-Bild: +0x5 = 6 steht, Sub 6 laeuft erst im naechsten Bild */

void re15_r39k_reach_saat(int naechstes_bild) { s_reach_saat = naechstes_bild ? 2 : 1; }
void re15_r39k_reach_aus(void)                { s_reach_saat = 0; }
int  re15_r39k_reach_wartet(void)             { return s_reach_saat == 2; }

void re15_r39k_reach_bildanfang(void)
{
    /* Naechstes Bild, Spieler nicht angehalten (Gate @0x80031c78, wie der Walker-Takt): der Dispatcher wird jetzt Sub 6
     * aufrufen (@0x8003068c-ac) — es sei denn, das SCD dieses Bildes hat +0x5 schon neu gesetzt (re15_r39k_reach_aus). */
    if (s_reach_saat == 2) s_reach_saat = 1;
}

int re15_r39k_reach_takt(re15_actor_t *pl)
{
    if (s_reach_saat == 2) return 1;     /* Dispatcher-Bild: dessen Gang-/Dreh-/Schub-Pose steht (f314 @0x80030c78 /
                                          * @0x80031494, Phase 7 ohne anim_set) */
    if (s_reach_saat != 1 || !pl) return 0;
    s_reach_saat = 0;
    re15_szl_schub_spieler_ende();       /* ein gehaltener Schub-Ausgang (Phase 7) ist mit +0x5 = 6 vorbei */
    pl->motion      = 1;                 /* +0x94 = 1 @0x80051854 */
    pl->anim_frame  = 0;                 /* +0x95 = 0 @0x80051864 — f314 @0x8005188c posiert Bild 0 */
    pl->anim_frac   = 7;                 /* +0x8f = 7 @0x80051874 (f314 zieht erst NACH dem Posieren ab) */
    pl->anim_freeze = 0;
    pl->walk_active = 0;
    return 1;
}
