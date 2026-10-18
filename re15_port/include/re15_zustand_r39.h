/* re15_zustand_r39.h — Runde 39 Spur F "zustand": Spielerzustand (Gift, Kleidung, EKG).
 * Dossier analysis/befunde_runde39/F_zustand.md.
 *
 *   1. Volle Heilung = ganze Kleidung (NUTZER-VORGABE; das Original loescht die Blut-Decals nie:
 *      analysis/blood_decals.md, Writer-Zensus auf 0x800b10ec = nur Builder FUN_80037c1c + FUN_80037edc).
 *      FORM (PORT-WAHL, belegt): "vollstaendig" = die HP erreichen den Wert des Heilsprays (`sh s0`,
 *      s0 = 100 @0x8004ae14, + Westen-Bonus des Ports); die Wiederherstellung ist der Spieler-Lade-Pfad
 *      des Originals (FUN_800314b0: Wund-Builder FUN_80037c1c nullt Level+Akku, Haut-Upload FUN_8004ee78
 *      Slot 0x17 @0x800315b0/@0x80031814; Wund-Builder FUN_80037c1c @0x800316c8/@0x800318cc) — derselbe Weg, auf dem ein Spielstand ohne Blut geladen wird.
 *   2. Gift zieht Energie wie RE2 Retail (Beta -> Retail: der RE1.5-Takt @0x80031e44-ecc, -2 HP alle
 *      121 Bilder, ist die Beta-Form; Zielform RE2 PSX.EXE FUN_8003bfac @0x8003c12c-180: alle 31 Bilder
 *      -1 HP bis HP 1) und das EKG zeigt den Gift-Zustand in der RE2-Farbe (Tabelle @0x800ab160, Eintrag
 *      4 = @0x800ab178: ff 20 ff / Abdunkelung 08 01 08 je Linie, FUN_800754ac). */
#ifndef RE15_ZUSTAND_R39_H
#define RE15_ZUSTAND_R39_H

#include <stdint.h>
#include "re15_actor.h"

/* ---- Gift ---------------------------------------------------------------------------------------- */
/* Spieler-Takt, einmal je Spielbild aus game_step_common.c (neben dem Zopf-Haken hinter dem Pause-`bltz`
 * @0x80031c78). aktiv = Raum geladen und Spieler nicht eingefroren (RE2 FUN_8003bfac kehrt bei
 * DAT_800cfbdc < 0 sofort um, @0x8003bfac-c0). Rueckgabe: HP-Abzug des Gift-Takts in diesem Bild (0 =
 * keiner) — der Aufrufer nimmt ihn aus dem Port-Treffer-Detektor (s_prev_hp) heraus, denn der Takt
 * schreibt im Original NUR Zaehler und HP (@0x8003c14c-180: kein Kommando-/Zustandsschreiber). */
int  re15_zustand_r39_spieler_takt(re15_actor_t *pl, int aktiv);
/* Gift gesetzt (Gift-Treffer): RE2 Op 32 schreibt den Takt-Zaehler 0x800cfe12 := 30
 * (`li v1,0x1e` @0x80020138 / `sb v1,-0x1ee(at)` @0x80020140). Gerufen aus spinnen_r38.c am Giftklumpen. */
void re15_zustand_r39_gift_setzen(void);
/* Neues Spiel / Testanfang: Zaehler und Takt-Zaehlung auf 0. */
void re15_zustand_r39_reset(void);
/* Messschiene/Test: Takt-Zaehler (= 0x800cfe12) und Zahl der HP-Abzuege seit reset. */
uint8_t  re15_zustand_r39_gift_zaehler(void);
unsigned re15_zustand_r39_gift_takte(void);

/* ---- Kleidung ------------------------------------------------------------------------------------ */
/* Nach dem Heil-Applier (item_use_common.c re15_item_use_apply = Tabelle @0x80010fbc): sind die HP voll
 * (>= 100 + Westen-Bonus), werden die Wund-Tabelle genullt (re15_wound_reset) und der Haut-Neuaufbau
 * angefordert. Rueckgabe 1 = Kleidung wiederhergestellt. */
int  re15_zustand_r39_heilung(uint8_t item_id);
/* Plattform (main.c-Haken im Blut-Decal-Sync -> zustand_r39_pc.c): 1 genau einmal je Anforderung -> Spielerhaut (Slot 0) aus der
 * PLD neu laden. */
int  re15_zustand_r39_haut_neu_abholen(void);
/* Test: steht eine Anforderung an (ohne sie abzuholen)? */
int  re15_zustand_r39_haut_neu_pending(void);
/* Volle HP (Schwelle der Wiederherstellung): 100 (`sh s0`, s0 = 100 @0x8004ae14) + Westen-Bonus. */
int  re15_zustand_r39_hp_voll(void);

/* ---- EKG ----------------------------------------------------------------------------------------- */
/* Linienfarbe des EKG im Statusschirm fuer Zustand `cond` (FUN_8004ed6c: 3 = Gift) und Linie i
 * (0 = Kopf der Spur, 31 = Schwanz). Rueckgabe 1 = Farbe ueberschrieben (RE2 POISON @0x800ab178-7d:
 * r = 0xff - 8 i, g = 0x20 - i, b = 0xff - 8 i, FUN_800754ac `base - step * i`), 0 = RE1.5-Tabelle gilt. */
int  re15_zustand_r39_ekg_farbe(int cond, int i, uint8_t *r, uint8_t *g, uint8_t *b);

#endif /* RE15_ZUSTAND_R39_H */
