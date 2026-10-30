/*
 * RE1.5 Rebuilt — BRENNENDE ZOMBIES (Flag Zone 1 / Index 31 = DAT_800aca3c & 1). Belege: engine/src/brandzombie_r15.c.
 *
 * Gesetzt von ROOM5120/5121/5140/5141 (`Set 1,31,1` am Skriptanfang). Ablauf im Original (STAGE1-Adressen, die
 * STAGE5-Zwillinge sind byte-gleich bis auf die Overlay-Verschiebung):
 *   INIT  @0x801008f4-0x80100c14 (Maedchen @0x8010ace8-...): +0x1d8 |= 0x100 | (rand&1)<<9, +0x1da = 600 + 2 Wuerfe,
 *         acht Flammen (Effekt 8 / Sub 3) an Koerperteilen aus @0x8011f7a4[(rand&1)*8 + k].
 *   ACTIVE @0x80101224 (Maedchen @0x8010b288): +0x1da--; == 300 -> FUN_80019d50(8,3,0x16, Teil) (Aufflammen);
 *         == 0 -> +0x4 = 0x1503 (DEATH-Zeile 21).
 *   DEATH[21][0] = 0x80108d74: Verbrennen (alle acht Flammen flammen auf, Kill-Flag, Sturz, Leiche).
 * RE2-Gehirn: GESTALTUNG (s. re15_brand_re2_takt).
 */
#ifndef RE15_BRANDZOMBIE_H
#define RE15_BRANDZOMBIE_H

#include "re15_actor.h"

/* INIT-Teil (@0x801008f4): nur bei gesetztem Flag. Aus re15_enemy_ai_live_init (Zombie-Familie und Maedchen). */
void re15_brand_init(re15_actor_t *e, int slot);
/* Derselbe Arm ohne Flag-Test (Maedchen-INIT ruft ihn hinter seinem eigenen Gate @0x8010ace8). */
void re15_brand_scharf(re15_actor_t *e, int slot);
/* ACTIVE-Zeitzaehler-Ereignis "== 300" (@0x80101284-0x80101504): das Aufflammen. */
void re15_brand_aufflammen_300(re15_actor_t *e, int slot);
/* DEATH-Zeile 21 = 0x80108d74. Rueckgabe 1 = behandelt. */
int  re15_brand_tod(re15_actor_t *e, int slot);
/* RE2-Gehirn (Standard): derselbe Zeitzaehler, Tod ueber die RE2-Brand-Zeile. Aus dem RE2-Tick. */
void re15_brand_re2_takt(re15_actor_t *e, int slot);

#endif
