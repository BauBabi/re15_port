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

/* MESSSCHIENE (env RE15_SPINNEN_LOG=<datei>, kein Spielverhalten): eine Zeile je Bild und
 * Spinne (Typ 0x25/0x26) mit Lage X/Y/Z, Zustandsbytes, Modus und Boden. Gerufen einmal je
 * Spielbild aus platform/pc/main.c (Haken "Runde 38 Spur R"). */
void re15_spinnen_r38_log_frame(unsigned frame);

#endif /* RE15_SPINNEN_R38_H */
