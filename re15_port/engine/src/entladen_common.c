/*
 * entladen_common.c — Runde 35 Spur I: Generationszaehler des Raum-Entladens (beide Plattformen).
 * Herleitung und Original-Adressen: include/re15_entladen.h, analysis/befunde_runde35/I_entladen.md.
 */
#include "re15_entladen.h"
#include "re15_enemy.h"   /* RE15_ENEMY_MAX */

unsigned g_re15_entladen_gen = 1;

static unsigned s_gegner_gen[RE15_ENEMY_MAX];

void re15_entladen_gen_weiter(void)
{
    g_re15_entladen_gen++;
    if (g_re15_entladen_gen == 0) g_re15_entladen_gen = 1;   /* 0 bleibt "nie gefuellt" */
}

void re15_entladen_gegner_merken(int bank)
{
    if (bank >= 0 && bank < RE15_ENEMY_MAX) s_gegner_gen[bank] = g_re15_entladen_gen;
}

unsigned re15_entladen_gegner_gen(int bank)
{
    return (bank >= 0 && bank < RE15_ENEMY_MAX) ? s_gegner_gen[bank] : 0u;
}
