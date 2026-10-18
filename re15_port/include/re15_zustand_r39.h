/* re15_zustand_r39.h — Runde 39 Spur F "zustand": Spielerzustand (Gift, Kleidung, EKG).
 * Dossier analysis/befunde_runde39/F_zustand.md.
 *
 *   1. Volle Heilung = ganze Kleidung (NUTZER-VORGABE; das Original loescht die Blut-Decals nie,
 *      analysis/blood_decals.md: Writer-Zensus auf 0x800b10ec = nur Builder FUN_80037c1c + FUN_80037edc).
 *   2. Gift zieht Energie wie RE2 Retail (Beta -> Retail: der RE1.5-Takt @0x80031e44-ecc ist die
 *      unfertige Beta-Form, RE2 PSX.EXE FUN_8003bfac @0x8003c12c-180 die Zielform) und das EKG
 *      zeigt den Gift-Zustand in der RE2-Farbe (@0x800ab178-7d). */
#ifndef RE15_ZUSTAND_R39_H
#define RE15_ZUSTAND_R39_H

#include <stdint.h>
#include "re15_actor.h"

/* Spieler-Takt, einmal je Spielbild aus game_step_common.c (neben dem Zopf-Haken hinter dem
 * Pause-`bltz` @0x80031c78). aktiv = Raum geladen und Spieler nicht eingefroren (RE2: FUN_8003bfac
 * kehrt bei +0x4 < 0 sofort um, `bltz` @0x8003bfb8). Rueckgabe: HP-Abzug des Gift-Takts in diesem
 * Bild (0 = keiner) — der Aufrufer nimmt ihn aus dem Port-Treffer-Detektor (s_prev_hp) heraus,
 * denn der Takt schreibt im Original NUR HP und Zaehler (@0x8003c15c-180: kein Kommando-Schreiber). */
int  re15_zustand_r39_spieler_takt(re15_actor_t *pl, int aktiv);

/* Gift gesetzt (Gift-Treffer): RE2 Op 32 schreibt den Takt-Zaehler 0x800cfe12 := 30
 * (`li v1,0x1e` @0x80020138 / `sb v1,-0x1ee(at)` @0x80020140). Gerufen aus spinnen_r38.c. */
void re15_zustand_r39_gift_setzen(void);
/* Neues Spiel / Testanfang / Spieler-Laden: Zaehler und Takt-Zaehlung auf 0. */
void re15_zustand_r39_reset(void);
/* Messschiene/Test: Takt-Zaehler (0x800cfe12) und Zahl der HP-Abzuege seit reset. */
uint8_t  re15_zustand_r39_gift_zaehler(void);
unsigned re15_zustand_r39_gift_takte(void);

#endif /* RE15_ZUSTAND_R39_H */
