/*
 * RE1.5 Rebuilt — Runde 36 Spur I "inventar": Memory Card wird beim Speichern verbraucht, leere
 * Memory Cards und Handgranaten verschwinden aus dem Spieler-Inventar.
 *
 * NUTZER-VORGABE (analysis/befunde_runde36/AUFTRAG.md Z.29, woertlich):
 *   "Mit Nutzung von Memory Cards beim Speichern soll sich die Anzahl reduzieren. Sind Memory Cards
 *    oder Handgranaten 0 sollen sie aus dem Player inventory verschwinden"
 *
 * Die FORM ist die des Originals (alle Belege: analysis/befunde_runde36/I_inventar.md, RE-Belege):
 *   * Verbrauch beim Speichern = RE2 Farbband-Block im Speicherschirm (info/re2leon/COMMON/BIN/
 *     MEM_CARD.BIN, laedt @0x801C0000): Platz suchen `jal 0x800696cc` @0x801c1190 (a0 = 30 @0x801c118c),
 *     Menge - 1 `addiu v0,v0,-1` @0x801c11ac / `sb` @0x801c11b8, bei 0 `sb zero` Id @0x801c11d0 und
 *     +2 @0x801c11dc, verdichten `jal 0x80069714` @0x801c11e0. Der Abzug liegt VOR dem Bau des
 *     Spielstands (der geschriebene Stand traegt die neue Menge) und wird bei einem Schreibfehler
 *     zurueckgenommen (@0x801c1290-12bc memcpy der Sicherung von @0x801c1168-84); Abbrechen im
 *     Speicherschirm erreicht den Block nie. Memory Card (RE1.5-Item 0x21) statt Ink Ribbon (0x1E).
 *   * Entfernen bei Menge 0 = RE1.5-Verbrauch beim Benutzen @0x8004aee4-af28 (Id/Menge/Flag := 0)
 *     + Verdichten FUN_8004dadc (`jal 0x8004dadc` @0x8004af2c) = re15_inv_remove_slot.
 *   * War eine leere Granate ausgeruestet: Ablegen wie UNEQUIP @0x8004ad44-4c (25c8 := 0x80), Waffe
 *     nach der Commit-Regel @0x80046654-88 (0x80 -> Waffe 1 = Messer, `ori v0,zero,0x1` @0x8004666c,
 *     `sb v0,-13731(at)` @0x80046688) + Bank `jal 0x80043d8c` @0x800466c4.
 * RE1.5 selbst entfernt die leere Granate NICHT (FUN_8004eae4 @0x8004eb44-60 zieht nur ab) und RE2 hat
 * keine Handgranaten — die Entfernung der Granaten ist deshalb NUTZER-VORGABE in dieser Form.
 */
#ifndef RE15_INVENTAR_R36_H
#define RE15_INVENTAR_R36_H

#include <stdint.h>
#include "re15_savedata.h"

/* Memory Card = RE1.5-Item 0x21 (Namenstabelle DEBUG.BIN @0x800C4BEA; == RE15_SAVEPOINT_CARD_ITEM). */
#define RE15_R36_KARTE_ID        0x21
/* Handgranaten 0x09 HAND / 0x0A ACID / 0x0B INCEND. GRENADE: Entlade-Handler 0x80033B38/58/78 = je nur
 * `jal 0x8004eae4` (@0x80033b40/60/80), Waffen-Parametersatz @0x800740b8/bd/c2 je `18 30 0a 01 00`. */
#define RE15_R36_GRANATE_ERSTE   0x09
#define RE15_R36_GRANATE_LETZTE  0x0B

/* 1 = dieser Gegenstand verschwindet bei Menge 0 aus dem Inventar (Memory Card, Handgranaten). */
int re15_inventar_r36_leer_weg_id(uint8_t id);

/* I1/I2, VOR dem Schreiben: den Abzug in den zu schreibenden Block legen (RE2: Abzug vor dem Bau des
 * Spielstands) — Menge - 1, bei 0 Platz leeren + verdichten (Ausruest-Platz zieht mit), Pruefsumme neu.
 * Rueckgabe: Platz der Memory Card im Block oder -1 (keine Karte -> Block unveraendert). */
int re15_inventar_r36_karte_im_block(re15_savedata_t *sd);

/* I1/I2, NACH erfolgreichem Schreiben: derselbe Abzug im Live-Inventar (re15_inv_remove_slot bei 0).
 * Danach ist g_inv.slots == sd->inv des geschriebenen Blocks. Rueckgabe: Restmenge oder -1. */
int re15_inventar_r36_karte_verbrauchen(void);

/* I3 (+ leere Memory Card aus jeder Quelle), je Spielbild VOR dem START-Poll (game_step_common.c):
 *   (a) jeder Inventarplatz mit Memory Card / Handgranate und Menge 0 wird entfernt; war er
 *       ausgeruestet, wird zuerst abgelegt (25c8 := 0x80, @0x8004ad44-4c);
 *   (b) ist danach nichts ausgeruestet (0x80), die Waffen-Id aber noch eine Handgranate, und der Wurf
 *       vorbei (Zielzustand verlassen, kein Granatenplatz mehr im Effekt-Pool), gilt die Commit-Regel
 *       0x80 -> Messer (@0x80046654-88) samt Waffenbank (@0x800466c4).
 * Laeuft nicht bei offenem Statusschirm. Rueckgabe: Zahl entfernter Plaetze in diesem Bild. */
int re15_inventar_r36_tick(void);

#endif /* RE15_INVENTAR_R36_H */
