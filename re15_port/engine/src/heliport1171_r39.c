/*
 * heliport1171_r39.c — Runde 39, Spur K "heliport1171": NPC-INIT-Pose (K2). Herleitung und alle Adressen:
 * include/re15_heliport1171_r39.h, Dossier analysis/befunde_runde39/K_heliport1171.md Abschnitt 6.
 */
#include "re15_heliport1171_r39.h"
#include "re15_szenenleon_r36.h"   /* re15_szl_schub_spieler_ende: Sub 0x13 endet mit +0x5 = 6 (@0x80052be0) */
#include "re15_room.h"             /* NB2: g_room_rdt / g_room_rdt_ok (Wand-Pass der Kraehen-Wurzel) */
#include "re15_collision.h"        /* NB2: re15_collision_constrain_contact_band = FUN_8003b0a4 mit Band +0x82 */
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

/* NACHBESSERUNG 1 — M3(b) Kraehen: +0x94/+0x95/+0x8f des Originals je Aktor-Slot (Belege im Header). Die Aktorfelder
 * motion/anim_frame/anim_frac tragen das zuletzt POSIERTE Bild (= was der Renderer zeichnet). */
typedef struct {
    uint8_t gueltig;            /* Tabelle traegt +0x94/+0x95/+0x8f                                               */
    uint8_t saat;               /* Clip-Setzer seit dem letzten Posieren                                          */
    uint8_t posiert;            /* dieser Slot hat schon posiert (p_clip/p_bild gelten)                          */
    uint8_t clip, bild, blende; /* +0x94 / +0x95 / +0x8f                                                         */
    int16_t p_clip; int32_t p_bild;   /* zuletzt posiert                                                          */
} r39k_kraehe_t;
static r39k_kraehe_t s_kraehe[RE15_ACTOR_MAX];

static r39k_kraehe_t *kraehe_von(const re15_actor_t *e)
{
    const int slot = e ? (int)(e - g_actors) : -1;
    return (slot >= 0 && slot < RE15_ACTOR_MAX) ? &s_kraehe[slot] : NULL;
}

/* 1 = die Aktorfelder selbst tragen +0x94/+0x95/+0x8f (alte Port-Form): nie ueber den Clip-Setzer gelaufen, oder ein
 * fremder Schreiber (RE2-Kraehenhirn, Zwang ROOM1120) hat seit dem letzten Posieren Clip/Bild umgesetzt. */
static int kraehe_fremd(const r39k_kraehe_t *k, const re15_actor_t *e)
{
    if (!k->gueltig) return 1;
    if (k->saat || !k->posiert) return 0;
    return e->motion != k->p_clip || e->anim_frame != k->p_bild;
}

void re15_r39k_kraehe_clip(re15_actor_t *e, uint8_t clip)
{
    r39k_kraehe_t *k = kraehe_von(e);
    if (!k) return;
    k->gueltig = 1;
    k->saat    = 1;
    k->clip    = clip;   /* +0x94 @0x80115da0 */
    k->bild    = 0;      /* +0x95 @0x80115db0 */
    k->blende  = 7;      /* +0x8f @0x80115dc4 */
}

int re15_r39k_kraehe_anim(re15_actor_t *e, const uint8_t *laengen, int n)
{
    r39k_kraehe_t *k = kraehe_von(e);
    if (!k) return 0;
    if (kraehe_fremd(k, e)) {
        k->clip = (uint8_t)e->motion; k->bild = (uint8_t)e->anim_frame; k->blende = (uint8_t)e->anim_frac;
        k->gueltig = 1;
    }
    k->saat = 0;
    int fc = (k->clip < n) ? laengen[k->clip] : 1;
    if (fc <= 0) fc = 1;
    /* posieren (@0x8001f35c-36c): das zeichnet der Renderer */
    e->motion = (int16_t)k->clip; e->anim_frame = k->bild; e->anim_frac = k->blende;
    k->p_clip = e->motion; k->p_bild = e->anim_frame; k->posiert = 1;
    /* dann vorschieben (@0x8001f5b0-b4, @0x8001f618-1c, Umlauf @0x8001f63c) */
    if (k->blende > 0) k->blende--;
    if ((int)++k->bild >= fc) { k->bild = 0; return 1; }
    return 0;
}

int re15_r39k_kraehe_bild(const re15_actor_t *e)
{
    const r39k_kraehe_t *k = kraehe_von(e);
    if (!k) return e ? (int)e->anim_frame : 0;
    return kraehe_fremd(k, e) ? (int)e->anim_frame : (int)k->bild;
}

/* NACHBESSERUNG 2 — N1: Wand-Pass der Kraehen-Wurzel nach jedem Zustand (Belege im Header). */
void re15_r39k_kraehe_wandpass(re15_actor_t *e, int32_t alt_x, int32_t alt_z)
{
    if (!e) return;
    if (!g_room_rdt_ok || (e->flags & 0x08u)) {        /* Wort0 Bit 3 -> `j 0x8003b524`, Rueckgabe 0 (@0x8003b134-44) */
        e->crow_wall = 0;
        return;
    }
    int32_t nx = e->x, nz = e->z;
    const int hit = re15_collision_constrain_contact_band(&g_room_rdt, alt_x, alt_z, &nx, &nz,
                                                          200,             /* Kasten[3] (`lhu a1,6(v0)` @0x80112200) */
                                                          (int)e->floor,   /* +0x82 (`sb v0,130(a0)` @0x80112154)    */
                                                          4u,              /* `ori a2,zero,0x4` @0x801121fc           */
                                                          NULL, NULL);
    e->x = nx; e->z = nz;
    e->crow_wall = (uint8_t)(hit ? 1 : 0);             /* +0x1d1 = Rueckgabe (`sb v0,465(v1)` @0x80112218) */
}

void re15_r39k_install(uint16_t room)
{
    (void)room;
    memset(s_init_typ, 0, sizeof s_init_typ);
    memset(s_kraehe, 0, sizeof s_kraehe);   /* NB1 M3(b): Raumaufbau spawnt neu (Kraehen-INIT setzt den Clip) */
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
