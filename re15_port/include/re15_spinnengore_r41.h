/*
 * RE1.5 Rebuilt — Runde 41 Spur S "spinnengore": das Platzen der erwachsenen Spinne 0x25 fuer BEIDE Gehirne.
 *
 * Nutzer (2026-11-01): "die spinnen platzen bei magnum Schuss z.b. immer noch nicht..."; Regel: das RE2-Gehirn liefert
 * Angriffsmuster und Treffer, der Gore ist das Maximum aus RE1.5, RE2 und den 80%-Videos.
 * Dossier: analysis/befunde_runde41/S_spinnengore.md (Messung, Disassembly, Inventur).
 *
 * Mechanismus = RE1.5 STAGE2.BIN (Mod-Baum und Disc Version_40% byte-gleich, Dossier 2b), Teilschicht re15_teile_r15.h:
 *   Teil platzt   : Farbwort +0x68 = 0x207f60, +0x94..+0x9e = {0,-50,0,3,0,0}, Wort |= 0x4a
 *                   (@0x80113cc8-cfc / @0x801140cc-10c / @0x801146bc-6f0).
 *   Beine fliegen : fuer i = 8..1, Paar A = 2i+1 / B = 2i+2, nur wenn (A.w & 0x41) == 1: Wort |= 0x1062, +0x75 = 20,
 *                   +0x82 = -128, +0x76 = 8, +0x38/+0x3a/+0x3c = 0/-100/0 (@0x801141dc-52c / @0x801147bc-b0c).
 *   Komplett-Platzer (0x80113ff8 Phase 0 @0x8011403c): Blut Teil 0, Teile 0 und 19 platzen, Blut Teil 0 und 19,
 *                   alle Beine fliegen. Waffen-Tabelle @0x80118f38: 7,8,9,10,11,13,16,17,18,21 -> 0x80113ff8 (IMMER),
 *                   der Rest -> 0x8011461c (Platzer nur mit Krit +0x93 & 0x40 @0x801146b0).
 *   Krit-Bein (HURT 0x80113aa4 @0x80113b74): Bein {3,5,..,17}[rng&7] (@0x80118f28) und Bein+1 platzen, Blut zweimal an
 *                   Teil Bein+2 (@0x80113dc4/@0x80113e1c).
 *
 * Unter dem RE2-Gehirn (PORT-WAHL "Gore-Maximum", abschaltbar mit RE15_KEIN_80PROZENT_GORE=1 wie der Zombie 8cda79eca):
 *   - jedes Bein, das RE2 abtrennt (FUN_80105BF0 / Zeile 11 @0x801039AC: +0x220 |= 1<<k), wird als RE1.5-Bein
 *     2k+3/2k+4 gezeichnet (RE2 selbst: Drift 0x4a dunkel 0x2f2f2f @0x80105c44-cf4; der Port nimmt das gruene
 *     RE1.5-Platzen, Video B 5:18.3 zeigt gelbgruene Brocken);
 *   - Krit ohne RE2-Abtrennung im selben Treffer: das RE1.5-Krit-Bein;
 *   - Tod mit einer RE1.5-Platzwaffe, mit Krit oder in einer RE2-Platzzeile (FUN_80104890-Aufrufer: Zeilen 5,6,7,8,17
 *     @0x80104588 und 9 ueber @0x80104A2C; Saeure 11 wirft Teil 19 @0x80104bcc-bf4): der Komplett-Platzer.
 */
#ifndef RE15_SPINNENGORE_R41_H
#define RE15_SPINNENGORE_R41_H

#include <stdint.h>
#include "re15_actor.h"

/* Schalter der RE2-Hebung: 1 = an (Standard), 0 = RE15_KEIN_80PROZENT_GORE gesetzt. Test-Vorgabe ueber
 * re15_spinnengore_test_schalter (-1 = Umgebung lesen). Die RE1.5-Mechanik im RE1.5-Gehirn haengt NICHT daran. */
int  re15_spinnengore_an(void);
void re15_spinnengore_test_schalter(int an);

/* Waffen-Id (RE1.5, +0x5 = weapon_id @0x800124bc) -> 1, wenn die Tabelle @0x80118f38 auf 0x80113ff8 zeigt. */
int  re15_spinnengore_platzwaffe(int weapon_id);

/* RE1.5-Bausteine (Adressen im Kopf). */
void re15_spinnengore_teil_platzt(int slot, int k);
int  re15_spinnengore_beine_fliegen(int slot);          /* Rueckgabe: Zahl der abgeworfenen Paare */
void re15_spinnengore_platzen(re15_actor_t *e);         /* Komplett-Platzer 0x8011403c */
void re15_spinnengore_bein_ab(re15_actor_t *e, int leg);/* Paar leg/leg+1 + Blut zweimal an leg+2 */

/* Hebung in das RE2-Gehirn (enemy_ai_re2_spider.c). beine_vor = +0x220 vor dem Zeilen-Handler, frisch = erster Takt
 * des Treffers (+0x6 == 0). weapon_id = +0x5 VOR der Zeilen-Uebersetzung, row = +0x23C danach. */
void re15_spinnengore_re2_treffer(re15_actor_t *e, uint8_t beine_vor, int frisch);
void re15_spinnengore_re2_tod(re15_actor_t *e, int weapon_id, int row);

/* Schritt 5 (Inventur je RE2-Gehirn): Hund 0x20 — Part-Bruecke beim RE2-Teile-Wurf 0x80104440 seeden, damit die
 * 7 geworfenen Parts (@0x80105680) gezeichnet werden (re15_re2z_gore_active Typ 0x20). */
void re15_gore_r41_hund_bruecke(re15_actor_t *e);

#endif
