/*
 * RE1.5 Rebuilt — Runde 38 Spur R "spinnen": Stage-2-Spinnen (Typ 0x25 Adult, RE2-Baby 0x26).
 *
 * Nutzer-Punkte (analysis/befunde_runde38/AUFTRAG.md Z. 68-70, Dossier
 * analysis/befunde_runde38/R_spinnen.md):
 *   1. Spinnen schweben in der Luft und kommen teilweise nicht aus den Schaechten raus.
 *   2. Spinnen verschiessen kein Gift (Vergiftung -> Gegengift).
 *   3. Minispinnen nach dem Tod schweben und greifen nicht an.
 *
 * Belege (Adressen/Bytes) im Dossier und an den Definitionen in engine/src/spinnen_r38.c.
 */
#ifndef RE15_SPINNEN_R38_H
#define RE15_SPINNEN_R38_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_esp.h"
#include "re15_enemy.h"   /* re15_enemy_bank_t (Teil 3: Animationspaar der RE2-Babybank) */

/* ---- Teil 1: Schacht-Ausstieg (STAGE2.BIN Zustand 4 Unterzustand 1, grid 0x41) --------------
 * EM025-Cliplaengen (CDEMD0.EMS Blob 10 = Typ 0x25, Bank-0-EDD @Blob+0xc, 17 Clips). */
extern const uint8_t re15_spinnen_r38_em025_clip_len[17];

/* RE1.5-KI: Zustand-4-Handler 0x80114e18 fuer +0x5 == 1 (grid 0x41, Phasentabelle @0x8010027c).
 * Gerufen aus enemy_ai_common.c re15_adult_spider_ai_tick (case 4). */
void re15_spinnen_r38_lauer(re15_actor_t *e);

/* RE2-KI: Schachtspinnen (Spawn grid 0x41) laufen zuerst die RE1.5-Choreografie (mit den
 * keyframe-gleichen RE2-Clips 15/2/14/0) und werden nach der Landung an den RE2-INIT
 * uebergeben. Rueckgabe 1 = dieser Tick ist erledigt (RE2-Gehirn NICHT rufen). */
int re15_spinnen_r38_lauer_re2(re15_actor_t *e);

/* ---- Teil 2: Gift ------------------------------------------------------------------------------
 * Spucken: Effekt 0x0d sub 0 der Raum-ESP am Teil-0-Anker der Spinne. re2 = 1: Anker-Versatz
 * der RE2-Spinne (FUN_801058A4), sonst der RE1.5-Versatz (DAT_80118ee8). */
void re15_spinnen_r38_spucken(re15_actor_t *e, int re2);

/* Routine A 24/25 der ESP-Zeilenmaschine (Tabelle 0x80071d40[24]/[25]). Gerufen aus
 * re15_esp.c esp_fx_dispatch; Rueckgabe 1 = A war 24 oder 25 (erledigt). */
int re15_spinnen_r38_routine_a(re15_esp_fx_t *f);

/* Gift-Wirkung im Spieler-Kommando 1 (Handler 0x80031de8, @0x80031e44-ecc): alle 121 Bilder
 * HP -2, nie toedlich (ausser Status-Bit 0x4). Gerufen aus game_step_common.c (cmd-1-Prolog). */
void re15_spinnen_r38_gift_tick(re15_actor_t *pl);
/* Neues Spiel / Testanfang: Gift-Zaehler (Spieler+0x1d3) auf 0. */
void re15_spinnen_r38_reset(void);
/* Messschiene/Test: aktueller Gift-Zaehler (Spieler+0x1d3 des Originals). */
uint8_t re15_spinnen_r38_gift_zaehler(void);
/* Messschiene/Test: Zahl der Gift-Treffer (Spieler im Kasten eines Giftklumpens) seit reset. */
unsigned re15_spinnen_r38_gift_treffer(void);

/* ---- Teil 3: RE2-Minispinnen -------------------------------------------------------------------
 * Biss-Reichweite = Ziel+0x9A + 10 (EMS26 @0x80100E40) mit dem RE2-Spieler-Radius 450
 * (RE2 PSX.EXE `addiu v1,zero,450` @0x8003bdc0 / `sh v1,154(s2)` @0x8003bdc4). */
#define RE15_SPINNEN_R38_RE2_SPIELER_9A 450

/* Animationspaar der RE2-Bank: das Baby (kind 0x26) laeuft im Original IMMER auf Paar 1 (EMD dir[1] EDD ->
 * +0x17C, dir[2] EMR -> +0x108; Binder RE2 PSX.EXE @0x8001abb0/@0x8001abc0), weil die EMS26-Wurzel genau diese
 * zwei Zeiger an alle Zustaende reicht (`lw a1,264(s0)` / `lw a2,380(s0)` @0x8010005c/60 -> `jal 0x8002959c`).
 * Die Port-Regel "Paar mit den meisten Clips" (re2_ems.c) nahm Paar 3 (victim, Wurzel-Y -1797..-1585) — die
 * Babys schwebten 1166..1797 ueber ihrer Lage. Gerufen aus re2_ems_load_bank (Haken "Runde 38 Spur R"). */
void re15_spinnen_r38_re2_bank(int kind, re15_enemy_bank_t *eb);

/* Wurzel-Schwanz des RE2-Babys (EMS26.BIN @0x801000ac-c4): word0 == 0 -> nichts; sonst der Kugel-Schub
 * FUN_80035530 (FUN_80034d0c je anderer Figur, Spieler-Kugel 450/-1530/1530 @0x8003bdc0-ec, Baby 10/-10/10
 * @0x80100168-78) und der Wandpass FUN_8003567C(self, 0x400) mit dem Baby-Radius +0x90 = 10 (@0x80100180)
 * im eigenen Band (-Y)/1800 - Port-Zuordnung auf die RE1.5-SCA-Klemme wie bei Hund/Kraehe/Adult (Maske 4).
 * ox/oy/oz = Lage am Bildanfang (+0x44/+0x46/+0x48, re2s_pos_snapshot). Gerufen aus
 * enemy_ai_re2_spider.c re15_re2spider_baby_tick (Haken "Runde 38 Spur R"). */
void re15_spinnen_r38_baby_schwanz(re15_actor_t *e, int32_t ox, int32_t oy, int32_t oz);

/* ---- Teil 4: Objekt-Ausschiebung (Aktor-Durchgang des Objekt-Ticks FUN_8002bd44 @0x8002be0c-4c) ---
 * FUN_8002cabc(a0 = Spinne, a1 = Objekt, a2 = 0) je aktivem Obj_model_set-Objekt (obj_id von hinten,
 * @0x8002bdb0-c8), danach der Lage-Spiegel +0x40/+0x42/+0x44 (@0x8001d124-154). Gerufen aus
 * enemy_ai_common.c re15_actor_prop_pushout fuer Typ 0x25 (Haken "Runde 38 Spur R"). */
void re15_spinnen_r38_objekt_pass(re15_actor_t *e);

/* MESSSCHIENE (env RE15_SPINNEN_LOG=<datei>, kein Spielverhalten): eine Zeile je Bild und
 * Spinne (Typ 0x25/0x26) mit Lage X/Y/Z, Zustandsbytes, Modus und Boden. Gerufen einmal je
 * Spielbild aus platform/pc/main.c (Haken "Runde 38 Spur R"). */
void re15_spinnen_r38_log_frame(unsigned frame);
/* Messgroesse (Schiene + Test, kein Spielverhalten): die hoechste SCA-Bodenflaeche AUF ODER UNTER der Figur =
 * FUN_8001c6e8(&P, 0, (-y + 1799) / 1800, 0x100) (Aufruf wie Routine 25 @0x80017fe0-f4, Startband aus der eigenen
 * Hoehe). Unabhaengig vom eigenen Bodenfeld +0x1ba/+0x1C2 der Figur. */
int16_t re15_spinnen_r38_boden_unter(const re15_actor_t *a);

#endif /* RE15_SPINNEN_R38_H */
