/*
 * RE1.5 Rebuilt — RE2-ZELLENARM (RE2 Typ 0x2D "Zombie-Arme im Fenster", ROOM2050) als
 * Gehirn fuer die ROOM1210-Gitterhaende (RE1.5-Typ 0x1A) unter dem RE2-Flavor.
 *
 * Runde 16 / Phase 2 (2026-09-19), Dossier analysis/befunde_2026-09-19/arme-1210-re2.md.
 * Alle Original-Adressen beziehen sich auf das RE2-Retail-Overlay CDEMD0_EM2D_ai1.BIN
 * (CDEMD0.EMS Sektor 0x9B0, 0x1528 B, geladen @0x80100000; Volltext
 * analysis/befunde_2026-09-19/arme-1210-re2_em2d_ai1.dis) bzw. auf info/re2leon/PSX.EXE.
 * Definition + Belege: engine/src/enemy_ai_re2_zellenarm.c.
 */
#ifndef RE15_ENEMY_AI_RE2_ZELLENARM_H
#define RE15_ENEMY_AI_RE2_ZELLENARM_H

#include <stdint.h>
#include "re15_actor.h"

/* 1 = dieser Aktor gehoert dem RE2-Zellenarm-Gehirn (Typ 0x1A UND RE2-Flavor fuer 0x1A). */
int  re15_re2arm_owns(const re15_actor_t *e);

/* Der ganze Tick (Root @0x80100018 + Routinen-Tabelle @0x80101424). Rueckgabe 1 = uebernommen,
 * 0 = nicht mein Aktor (der Aufrufer faehrt dann den RE1.5-Writher). */
int  re15_re2arm_tick(int slot);

/* ENEMSE-Wiedergabe (PC registriert; engine bleibt link-sauber). bank_fn wird SOFORT mit der
 * Bank des Arms gerufen, se_fn spaeter mit (id, flag2000=1) — Belege an der Definition. */
void re15_re2arm_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank));

/* Zeichner-Seite: Bitmaske der Parts (Bone-Index), die NICHT gezeichnet werden — die sieben
 * Bones des inaktiven Arms (INIT `sw zero,0(part)` @0x80100240 / @0x801002F8). Der ganze
 * Aktor ist ausserdem ueber e->no_draw verborgen, solange Entity-Bit 2 gesetzt ist
 * (0x80101164 mit a2 = 0). */
uint16_t re15_re2arm_part_hide_mask(const re15_actor_t *e);

/* Mess-/Test-Auskunft: Heimat (+0x218 Yaw, +0x21C x, +0x220 z) und der globale Griff-
 * Cooldown 0x800CFBF4 (== g_re2_room_gflags, EIN Wort mit Hund/Kraehe). */
void re15_re2arm_home(int slot, int16_t *yaw, int32_t *x, int32_t *z);
int  re15_re2arm_holder_slot(void);      /* -1 = kein Arm haelt den Spieler */
int  re15_re2arm_hand_bone(const re15_actor_t *e);   /* 3 (Arm A) oder 10 (Arm B) */

#endif /* RE15_ENEMY_AI_RE2_ZELLENARM_H */
