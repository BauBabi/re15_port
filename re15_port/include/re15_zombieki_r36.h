/*
 * Runde 36 Spur F "zombieki" — Gorilla (Typ 0x27, ROOM11C0) Schatten-Record und Blutlache der Leiche.
 * Dossier: analysis/befunde_runde36/F_zombieki.md, Punkt 3.
 *
 * Nutzer: "Bei den Monkeys in ROOM 11C0 laeuft das Blut nicht aus wenn sie tot sind."
 *
 * ORIGINAL (STAGE1.BIN, selbst disassembliert mit re15_disasm.py):
 *   INIT FUN_80116f50: Schatten-Record FUN_8001af5c(a0=0, a1=0, a2 = rec+6 + 100, a3 = rec+6 + 200,
 *     sp+16 = entity+0xB0, sp+20 = 0x80808080) @0x801171cc-f0; rec = *(entity+0x78) =
 *     Tabelle 0x80121368[+0x1e2 = 4 (@0x80117098-9c)] = 0x80121350 -> u16 @0x80121356 = 1600
 *     (Hitbox-Halbbreite). FUN_8001af5c legt a2/a3 als `sh s3,12(s0)` @0x8001b030 / `sh s4,14(s0)`
 *     @0x8001b038 auf entity+0xBC / +0xBE (s0 = sp+16 = entity+0xB0). => +0xBC = 1700, +0xBE = 1800.
 *     (Der Zombie uebergibt an derselben Stelle 0x258/0x2bc = 600/700 @0x80100c2c/c38 — die Port-
 *     Konvention nhx/nhz der Halbmasse ist damit dieselbe.)
 *   LEICHE FUN_8011be54 (Zustand 7): +0x7 == 0: +0x9e = 0x5a @0x8011be80-84, +0x7 = 1 @0x8011be90-94,
 *     Wort0 |= 2 @0x8011beac, |= 0x40 @0x8011bec8; Durchfall; +0x7 == 1: Tinte +0xC4 / +0xEC =
 *     (alt & 0xff000000) | 0x00ffff38 @0x8011bed0-f00, **+0xBC += 8 / +0xBE += 8 @0x8011bf08-14**,
 *     +0x9e-- (alt == 0 -> +0x7 = 2) @0x8011bf24-44; +0x7 >= 2: `jr ra` @0x8011bf48.
 *   Der Port fuehrte die Leiche bisher nur mit dem Zaehler (+0x9e) — Tinte und Wachstum standen als
 *   Kommentar, wurden aber nie geschrieben, und der Renderer zeichnete fuer 0x27 den 500x600-Standard-
 *   Charakterschatten (dieselbe Fehlerklasse wie der Hund in Runde 34).
 */
#ifndef RE15_ZOMBIEKI_R36_H
#define RE15_ZOMBIEKI_R36_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_R36_AFFEN_HITBOX_HALB   1600   /* u16 @0x80121356 (Record 0x80121350[+6], Tabelle 0x80121368[4]) */
#define RE15_R36_AFFEN_SCHATTEN_X    (RE15_R36_AFFEN_HITBOX_HALB + 100)   /* `addiu a2,a3,100` @0x801171e8 */
#define RE15_R36_AFFEN_SCHATTEN_Z    (RE15_R36_AFFEN_HITBOX_HALB + 200)   /* `addiu a3,a3,200` @0x801171f0 */
#define RE15_R36_AFFEN_LACHE_SCHRITT 8      /* `addiu v0,v0,8` / `addiu v1,v1,8` @0x8011bf08-0c */
#define RE15_R36_AFFEN_LACHE_TAKTE   0x5a   /* `ori v0,zero,0x5a` / `sb v0,158(a0)` @0x8011be80-84 */

/* INIT-Haken (re15_maggot_ai_tick Zustand 0): Schatten-Record +0xBC/+0xBE wie FUN_8001af5c @0x801171cc-f0. */
void re15_r36_affen_lache_init(re15_actor_t *e);

/* Leichen-Haken (re15_maggot_ai_tick Zustand 7, waehrend +0x7 == 1): Tinte + Wachstum @0x8011bed0-bf14.
 * Der Zaehler +0x9e bleibt im Aufrufer (unveraendert). */
void re15_r36_affen_lache_tick(re15_actor_t *e);

/* Renderer-Haken (platform/pc/main.c Schattenquad): liefert fuer Typ 0x27 Halbmasse und Lachenfarbe.
 * Rueckgabe 1 = uebernommen (nhx/nhz gesetzt; *pool = 1 mit Farbe 0x00ffff38 -> r 0x38 g 0xff b 0xff
 * wie der RE1.5-Hund @0x801117F0), 0 = nicht zustaendig. */
int re15_r36_affen_lache_render(const re15_actor_t *npc, int32_t *nhx, int32_t *nhz, int *pool,
                                uint8_t *sh_r, uint8_t *sh_g, uint8_t *sh_b);

/* Nur fuer Sonden: Leichen-Haken abschalten (Gegenprobe). */
void re15_r36_affen_lache_enable(int an);

#endif /* RE15_ZOMBIEKI_R36_H */
