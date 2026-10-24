/* re15_giftkugel_r41.h — Runde 41 Spur D "kleinkram_a": Giftkugel der Spinne trifft den Spieler wie in RE2.
 * Dossier: analysis/befunde_runde41/D_kleinkram_a.md §3. Code: engine/src/giftkugel_r41.c, Haken: spinnen_r38.c
 * Routine 25 (vor dem RE1.5-Resolver), game_step_common.c (Messschiene).
 *
 * NUTZER-VORGABE (AUFTRAG.md 2026-10-23): "Der Spieler kann nicht sterben, wenn er noch mal mit Gift von den Spinnen
 * angeschossen wird, aber bereits auf Danger ist..."
 *
 * GEMESSEN (Port vor dieser Spur, Leon ROOM2030, RE2-Gehirn): jeder Klumpen -10 HP (RE1.5 Routine 25 ->
 * FUN_80012d60(500,&P,0) @0x80018008, Art 0 = DAT_8006f418[0] = 10), Tod nur bei HP < 0 (`bgez` @0x80012ee8):
 * 17 -> 7 ueberlebt (Danger = HP < 20, FUN_8004ed6c Schwelle @0x800112b5), erst 1 -> -9 stirbt. Der Gift-Takt (RE2
 * FUN_8003bfac, `slti v0,v0,0x2` @0x8003c174) hoert bei HP 1 auf und toetet nie.
 *
 * RE2 (Ziel, Beta -> Retail; ghidra_re2_Leon.txt): Op 32 @0x800200b0 ruft den Applier `jal FUN_80047664` @0x800200e0
 * mit a1 = 0x320 (Radius 800 @0x800200d0) und a2 = 0x14 (SCHADEN 20 @0x800200dc). FUN_80047664: Tore word0 & 1
 * (@0x800476e0), +0x1d3 != 0 (@0x800476ec-f4), HP < 0 (@0x800476fc-704), +0x10e & 0xc000 (@0x8004770c-18); Treffer
 * -> Kommando +4 := 2 (@0x800477b8); Spieler (s2 == 0) -> `jal FUN_800401d4` @0x800477fc (a0 = Schaden, a1 = 0),
 * Rueckgabe 2 -> Kommando +4 := 3 = TOD (@0x80047804-0c). FUN_800401d4 Modus 0 (RE2_Quellcode_V2/FUN_800401d4.c):
 * HP -= dmg; HP < 0 und (HP < -14 ODER Latch DAT_800cfd4c & 0x1000) -> DAT_800cfb74 |= 0x4000000, DAT_800cfdcb |= 0x80,
 * ret 2 (Tod); sonst HP := 0, Latch := 1, ret 1 ("ein Mal gerettet").
 *
 * PORT (RE2-Gehirn der Spinne, re15_ai_re2_for_type(0x25)): Schaden 20 wie RE2 — bei Danger (HP < 20) geht JEDER
 * Treffer unter 0 = Tod (Kommando 3 -> Zustand 3, Todesfolge des Ports). NUTZER-VORGABE "Danger + Treffer = Tod":
 * der Rettungszweig von FUN_800401d4 (HP := 0 + Latch, einmal je Raum) wird fuer die Giftkugel NICHT uebernommen,
 * er wuerde genau den gemeldeten Fall (noch ein Treffer bei Danger, kein Tod) einmal je Raum erzeugen. Der Treffer-
 * Kasten bleibt der RE1.5-Effektkasten (Radius 500, `ori a0,zero,0x1f4` @0x80017ffc; RE2 800 — Hybrid wie das
 * RE1.5-ESP-Geschoss selbst, spinnen_r38.c). RE1.5-Gehirn: unveraendert Beta-Wert 10 (byte-true).
 * Tore wie Routine 25's `frei`: +0x93 & 1 (hit-once @0x80012e24-30, RE2-Gegenstueck +0x1d3 @0x800476ec-f4), Griff
 * (re15_player_is_grabbed = +0x1D3 & 0x80), HP < 0 (@0x800476fc-704); Treffer-SE 10 wie der Port-Spielerzweig
 * (`jal FUN_800453d0` a0 = 10 @0x80012e70). */
#ifndef RE15_GIFTKUGEL_R41_H
#define RE15_GIFTKUGEL_R41_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_damage.h"   /* re15_attack_box_t */

#ifdef __cplusplus
extern "C" {
#endif

#define RE15_GIFTKUGEL_R41_SCHADEN 20   /* RE2 Op 32: `li a2,0x14` @0x800200dc */
#define RE15_GIFTKUGEL_R41_DANGER  20   /* Statusschirm-Schwelle "Danger": `lb` @0x800112b5 = 0x14 (FUN_8004ed6c) */

/* Routine-25-Haken (spinnen_r38.c), VOR dem RE1.5-Resolver: 0 = nicht zustaendig (Absender keine Spinne unter dem
 * RE2-Gehirn / Kasten trifft nicht / Tor zu), 1 = Treffer (Kommando 2), 2 = Tod (Kommando 3). Setzt +0x93 Bit 0, damit
 * der nachfolgende Resolver-Spielerzweig (hit-once) nicht noch einmal abzieht. */
int      re15_giftkugel_r41_treffer(re15_actor_t *pl, const re15_attack_box_t *box, int besitzer);

/* Messschiene (env, kein Spielverhalten): RE15_R41_HP_AT=<bild>:<hp> setzt im Spielbild <bild> die HP des Spielers.
 * Einmal je Spielbild aus game_step_common.c. */
int      re15_giftkugel_r41_messhaken(re15_actor_t *pl);   /* 1 = HP in diesem Bild gesetzt (Aufrufer setzt seine HP-Baseline nach) */

/* Tests: Zaehler seit reset. */
unsigned re15_giftkugel_r41_treffer_zaehler(void);
unsigned re15_giftkugel_r41_tode(void);
void     re15_giftkugel_r41_reset(void);

#ifdef __cplusplus
}
#endif
#endif /* RE15_GIFTKUGEL_R41_H */
