/*
 * RE1.5 Rebuilt — Runde 41 Spur S "spinnengore", Nachbesserung 2: die LACHE unter der Spinnenleiche (0x25) und der
 * Fleck der toten RE2-Babyspinne (0x26). Dossier analysis/befunde_runde41/S_spinnengore.md (Abschnitt "Nachbesserung 2").
 *
 * Mechanik in beiden Spielen dieselbe wie bei Zombie, Hund, Kraehe und Gorilla: der Schatten-Record wird umgefaerbt und
 * waechst um 8 je Takt. Der Port fuehrt den Record in crow_shadow_w/h (+0xBC/+0xBE bzw. RE2 rec+4/+6) und crow_pool
 * (Farbwoerter umgefaerbt); die Tinte je Gehirn haelt dieses Modul.
 *
 * RE1.5, STAGE2.BIN (Mod = Disc Version_40%), CORPSE 0x801153d4 (Zustandstabelle @0x80118e24[7]), Phase = +0x7,
 * Phasentabelle @0x801002a4 = {0x80115414, 0x8011546c, 0x801155d4, 0x801154bc, 0x80115534}:
 *   Phase 0  `ori v0,zero,0x5a` / `sb v0,158(v1)` @0x8011541c-20 (+0x9E = 90), +0x7 = 1 @0x8011542c-30 -> faellt in 1
 *   Phase 1  `lui a2,0xdf` / `ori a2,a2,0x809f` @0x8011546c-70: +0xC4/+0xEC = (alt & 0xff000000) | 0x00DF809F
 *            (@0x80115488-a0); +0xBC/+0xBE += 8 (`addiu +8` @0x801154a8/ac, `sh` @0x801154b0/b8)
 *   Phase 3  `ori v0,zero,0x78` / `sb v0,158(v1)` @0x801154c4-c8 (+0x9E = 120), +0x7 = 4 @0x801154d4-d8, Clip 5
 *            (`sb v0,148` @0x80115520) -> faellt in 4 (Tinte und Wachsen wie Phase 1 @0x80115534-7c)
 *   Zaehler  `lbu a0,158` / `addiu v0,a0,-1` / `bne a0,zero` / `sb v0,158` @0x801155a8-b8: war +0x9E schon 0, dann
 *            +0x7 = 2 (@0x801155c8-d0) und Raum-SE 9 (`ori a0,zero,0x9` @0x801155bc, `jal 0x800453d0` @0x801155cc).
 *   Eintritt Platzweg 0x80113ff8 Phase 2: `ori v0,zero,0x7` / `sw v0,4(v1)` @0x80114578-80 -> +0x7 = 0 (90 -> 91 Schritte)
 *            Weg 0x8011461c Phase 4: `lui v1,0x300` / `ori v1,v1,0x7` / `sw v1,4(v0)` @0x80114d4c-60 -> +0x7 = 3
 *            (120 -> 121 Schritte). Krit-Platzer im Weg 0x8011461c setzt +0x5 = 7 (`ori v0,zero,0x7` / `sb v0,5(v1)`
 *            @0x80114b1c-20), die Wurzel springt danach ueber Tabelle @0x80118f38[7] in den Platzweg -> +0x7 = 0.
 *
 * RE2, EMS25.BIN FUN_80104DE8 "Boden-Automat +0x225", je Takt aus der Wurzel (`jal 0x80104de8` @0x80100180), Tabelle
 * @0x801000b4 = {0x80104e28, 0x80104e54, 0x80104e68, 0x80104ec8, 0x80104f00}, Record = +0x16C (`lw s1,364(s0)` @0x80104e00):
 *   CORPSE: +0x6 == 0 -> +0x225 = 2 (`addiu v0,zero,2` / `sb v0,549(s0)` @0x80104d7c-80)
 *   [2]  Wasser (`jal 0x800527b4` @0x80104e70, `bne v0,zero` @0x80104e78) -> bleibt in 2, keine Lache. Sonst +0x225 = 3
 *        (@0x80104e84-8c), rec+0x1C/+0x44 = (alt & 0xff000000) | 0x00BF10BF (`lui a1,0xbf` / `ori a1,a1,0x10bf`
 *        @0x80104e7c-80, `sw` @0x80104ea8/b0), +0x16B = 90 (`addiu v0,zero,90` / `sb v0,363(s0)` @0x80104eac-b4) -> faellt in 3
 *   [3]  rec+4/+6 += 8 (@0x80104ed0-dc), +0x16B-- (@0x80104ee0-ec); 0 -> +0x225 = 4 (@0x80104ef8-efc)
 *   [0]/[1] schalten nur das Verbergen des Schattens (word0 0x400, @0x80104e28-64) — kein Port-Konsument, nicht hier.
 *
 * RE2, EMS26.BIN Babyspinne, SPLAT @0x80100C84 (+0x6 == 0): am Boden (+0x222 == 0, `lbu v0,546(s0)` / `bne`
 * @0x80100ca8-b0) rec+0x1C/+0x44 = (alt & 0xff000000) | 0x00BF10BF (`lui a1,0xbf` / `ori a1,a1,0x10bf` @0x80100cb4-b8,
 * `sw v0,28(a2)` @0x80100cd8, `sw v1,68(a2)` @0x80100ce0). Record 200 x 200 (INIT `lui a2,0xc8` / `ori a2,a2,0xc8`
 * @0x8010024c-50 -> FUN_80016480 `sw a2,4(t0)` @0x80016530). Im naechsten Takt FUN_8001B250 (@0x80100d24): nullt nur
 * word0/word1 (@0x8001b258-5c), gibt den Record NICHT frei. Die Entity-Schleife ruft nach dem Handler noch
 * FUN_800168b4(rec, +0x1C2, +0x76, (word0 & 0x400) >> 9) (@0x800268c8-dc) -> Bit 4 gesetzt (@0x80016960-68), danach
 * ueberspringt sie die tote Entity (`andi v0,v0,0x1` / `beq` @0x800267d4-d8). Der Zeichner FUN_8001699c zeichnet jeden
 * Record mit Bit 4 und ohne Bit 8 (@0x80016a04-18). Freigegeben wird er erst vom Pool-Init FUN_8001633c
 * (`sb zero,-50(s1)` @0x8001639c), einziger Aufrufer FUN_80026b7c @0x80026e0c (Raumwechsel). Der gruene Fleck bleibt
 * also bis zum Raumwechsel liegen.
 */
#ifndef RE15_SPINNENLACHE_R41_H
#define RE15_SPINNENLACHE_R41_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_SPINNENLACHE_TINTE_R15   0x00DF809Fu   /* @0x8011546c-70 (Phase 1) / @0x80115534-38 (Phase 4) */
#define RE15_SPINNENLACHE_TINTE_RE2   0x00BF10BFu   /* EMS25 @0x80104e7c-80, EMS26 @0x80100cb4-b8 */
#define RE15_SPINNENLACHE_SCHRITT     8             /* RE1.5 @0x801154a8/ac, RE2 @0x80104ed0/d4 */
#define RE15_SPINNENLACHE_R15_TAKTE_0 0x5a          /* @0x8011541c */
#define RE15_SPINNENLACHE_R15_TAKTE_3 0x78          /* @0x801154c4 */
#define RE15_SPINNENLACHE_RE2_TAKTE   90            /* @0x80104eac */
#define RE15_SPINNENLACHE_BABY_HALB   200           /* EMS26 @0x8010024c-50 */
#define RE15_SPINNENLACHE_FLECKEN_MAX 50            /* RE2-Record-Pool: 50 Plaetze @0x800164a4 (`addiu v1,zero,50`) */

/* RE1.5-Gehirn. Eintritts-Phase (+0x7) der Leiche: Platzweg (Tabelle @0x80118f38 -> 0x80113ff8) 0, sonst 3. */
uint8_t re15_spinnenlache_r15_eintritt(int platzweg);
/* CORPSE 0x801153d4 Phasen 0/1 und 3/4 (+0x7 = sub_state_3, +0x9E = grab_kill_ctr). Raum-SE 9 am Ende. */
void re15_spinnenlache_r15_leiche(re15_actor_t *e);

/* RE2-Gehirn: FUN_80104DE8 Zustaende [2]/[3]/[4] (+0x225 = re2s_gs225, +0x16B = re2z_gaitrow). */
void re15_spinnenlache_re2_boden(re15_actor_t *e);

/* RE2-Babyspinne: SPLAT am Boden (Tinte) und das Ende des Aktors (der Record bleibt als Fleck liegen). */
void re15_spinnenlache_baby_platsch(re15_actor_t *e);
void re15_spinnenlache_baby_ende(const re15_actor_t *e);

/* Zeichner (main.c): Halbmasse und Tinte des Schatten-Records einer Spinne bzw. Babyspinne. Rueckgabe 1 = gesetzt. */
int  re15_spinnenlache_render(const re15_actor_t *npc, int32_t *nhx, int32_t *nhz, int *pool,
                              uint8_t *sh_r, uint8_t *sh_g, uint8_t *sh_b);

/* Liegengebliebene Flecken toter Babyspinnen (bis zum Raumwechsel). */
int  re15_spinnenlache_flecken_zahl(void);
int  re15_spinnenlache_fleck(int i, int32_t pos[3], int16_t *yaw, int32_t *hx, int32_t *hz, uint8_t rgb[3]);
void re15_spinnenlache_raum_reset(void);

/* Tinte als (R,G,B) fuer re15_render_shadow_quad_c: Wort 0x00BBGGRR wie beim Zombie (0x00FFFF38 -> 0x38/0xff/0xff). */
void re15_spinnenlache_tinte_rgb(uint32_t tinte, uint8_t *r, uint8_t *g, uint8_t *b);

/* Messhaken RE15_BABYTOD_TEST=<Spielbild>: 1 = dieses Baby soll jetzt wie zertreten sterben (nur Messlaeufe). */
int  re15_spinnenlache_test_babytod(const re15_actor_t *e);

#endif
