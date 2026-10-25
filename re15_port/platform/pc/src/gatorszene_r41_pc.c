/*
 * RE1.5 Rebuilt — Runde 41 Spur F "gatorszene", PC-Teil: Ton-Anbindung der 2090-Ankunftsszene.
 * Herleitung: include/re15_gatorszene_r41.h, Dossier analysis/befunde_runde41/F_gatorszene.md.
 *
 *  - Gator-SEs (ENEMSE Bank 17, Paar-Tabelle @0x800A7400) ueber denselben Weg wie der Boss (main.c pc_gator_se_re15 +
 *    re15_audio_re2_enemy_bank, re15_gator_audio_hook): Schrei der Grossreaktion SE 0 (`addu a0,zero,zero` @0x80102c0c +
 *    `jal 0x8005bd6c` @0x80102c10).
 *  - Der Knall des Hereinbrechens (Nachtrag 17) laeuft plattformneutral in gatorszene_r41.c (Knall-Bank Satz 0 ueber
 *    re15_audio_re2_tuer_laden/_se wie irons_tod_1150.c); hier nichts.
 */
#include "re15_gatorszene_r41.h"
#include "re15_audio.h"

extern void pc_gator_se_re15(int se_id, int flag2000);   /* main.c: ENEMSE-Mapper des Boss-Alligators */

void re15_pc_gatorszene_r41_init(void)
{
    re15_gatorszene_r41_audio_hook(pc_gator_se_re15, re15_audio_re2_enemy_bank);
}
