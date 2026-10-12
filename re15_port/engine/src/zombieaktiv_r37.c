/*
 * Runde 37 Spur H "zombieaktiv" — RE1.5-Deskriptor 0x00 (aktiver Zombie der RE1.5-Daten) -> RE2-Deskriptor 6
 * (Gang ab Spawn). Belege und Abbildungstabelle: include/re15_zombieaktiv_r37.h, Dossier
 * analysis/befunde_runde37/H_zombieaktiv.md.
 */
#include "re15_zombieaktiv_r37.h"
#include "re15_ai_flavor.h"   /* re15_re2z_owns_type */
#include "re15_irons_tod.h"   /* re15_irons_tod_zustand / RE15_IT_AUS — Montage-Schritt: Spieler geparkt */

typedef struct {
    uint8_t gang;    /* 1 = Spawn-Satz trug RE15_R37_RE15_AKTIV */
    uint8_t type;
    int32_t x, z;    /* Spawn-Lage (Sce_em_set pc[8]/pc[12], nach re15_zombie_abstand_anwenden) */
} r37_marke_t;

static r37_marke_t s_marke[RE15_ACTOR_MAX];
static int s_an = 1;

void re15_r37_zombieaktiv_enable(int an) { s_an = an ? 1 : 0; }
void re15_r37_marken_loeschen(void)       { for (int i = 0; i < RE15_ACTOR_MAX; i++) s_marke[i].gang = 0; }
int  re15_r37_zombieaktiv_enabled(void)  { return s_an; }

void re15_r37_em_set(int actor_slot, const re15_actor_t *a, uint8_t behavior)
{
    if (actor_slot < 1 || actor_slot >= RE15_ACTOR_MAX || !a) return;
    r37_marke_t *m = &s_marke[actor_slot];
    m->gang = 0;
    if (!s_an) return;
    if (behavior != RE15_R37_RE15_AKTIV) return;                 /* nur die aktive RE1.5-Klasse (Kopf) */
    if (!re15_re2z_owns_type(a->type)) return;                   /* nur die Zombie-Familie des RE2-INIT */
    if (re15_irons_tod_zustand() != RE15_IT_AUS) return;         /* Montage-Schritt: Spieler geparkt (Kopf) */
    m->gang = 1;
    m->type = a->type;
    m->x = a->x;
    m->z = a->z;
}

int re15_r37_re2_gang(int slot, const re15_actor_t *e)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX || !e) return 0;
    r37_marke_t *m = &s_marke[slot];
    if (!m->gang) return 0;
    m->gang = 0;                                                 /* verbraucht: genau ein INIT */
    if (!s_an) return 0;
    return m->type == e->type && m->x == e->x && m->z == e->z;
}
