/*
 * RE1.5 Rebuilt — Runde 41 Spur "pipe": die PIPE (Item 2) als Nahkampfwaffe wie das Messer, mit mehr Reichweite.
 *
 * NUTZER 2026-11-16 (woertlich): "Mir ist ausserdem noch aufgefallen das wir die Pipe als Waffe im Inventar spawnen
 * koennen, aber sie ist noch nicht nutzbar. Eigentlich sollte die Pipe so aehnlich funktionieren wie ein Messer, aber
 * mit mehr Reichweite. [...] Ausserdem sollte pipe auch wie Messer keine Anzahl haben, sondern halt einfach unendlich
 * da sein."
 *
 * WAS DAS ORIGINAL HERGIBT (echte Disc Version_40%/PSX.EXE, Mod-Exe info/Re1.5/PSX.EXE an allen Stellen gleich):
 *   Klasse      @0x80074038 = 0x80034E70  Nahkampf-FSM wie das Messer (@0x80074034); Schusswaffen 0x80032E9C
 *   Tester      @0x8006E550 = 0x800127FC  Nahkampf-KEGEL wie das Messer (@0x8006E54C)
 *   Schaden     @0x8006E658 = 24          Zombiezeile (Messer @0x8006E654 = 6)
 *   Item-Zeile  @0x80074DC0 = 00 00 00 00 88 4c 07 80 00 00 00 00  byte-gleich zur Messerzeile @0x80074DB4
 *               (Obergrenze 0 = kein Magazin, Ziffern-CLUT-Index 0)
 * WAS DAS ORIGINAL NICHT HERGIBT (= die Pipe ist im 40%-Stand unfertig):
 *   Reichweite  @0x8006E5A8 = 1000 = Grundwert (Messer @0x8006E5A4 = 1100) — KEIN Vorsprung
 *   Drehsatz    @0x80074095 = 00 00 00 00 00 (Messer @0x80074090 = 18 30 00 01 01; Leser @0x80034FF8/@0x80035270)
 *   Bank        PL00W02/PL04W02.PLW = 11-Clip-Schusswaffen-Platzhalter ohne Rohr-Netz (s. tools/pipe_r41/)
 *   Ton-Bank    SOUND/ARMS02 = byte-gleich ARMS03..06/0A/0B (Pistolen-Bank, sha256 ff146f31...)
 *   Treffer-Ton nur Waffe 1: `ori v0,zero,0x1` / `bne s0,v0` @0x800123C0-C4 -> SE 0x01080001 @0x800123D4
 * Die Zahl im Inventar zeichnet das Original fuer JEDE Id < 0x22 (`sltiu v0,v0,0x22` @0x80049124, keine Ausnahme
 * fuer Messer oder Rohr). "Keine Anzahl" ist damit NUTZERWUNSCH, kein Original-Merkmal.
 */
#ifndef RE15_PIPE_R41_H
#define RE15_PIPE_R41_H

#include <stdint.h>

#define RE15_PIPE_R41_ID      0x02   /* "PIPE" s_item_names[2] (DAT_800c4a28-Katalog); Dispatch-Index 2 @0x80074038 */
#define RE15_PIPE_R41_MESSER  0x01   /* COMBAT KNIFE (re15_messer.h RE15_MESSER_ID) */

/* NUTZERWUNSCH "keine Anzahl, unendlich wie Messer": 1 = keine Mengen-Ziffern (Inventar-Gitter, Ausruest-Feld,
 * Kisten-Liste). Gilt fuer die beiden Nahkampf-Ids 1 und 2 (Tester 0x800127FC @0x8006E54C/@0x8006E550, Item-Zeilen
 * @0x80074DB4/@0x80074DC0 mit Obergrenze 0). Alle anderen Ids: unveraendert das Original-Gate @0x80049124. */
int re15_pipe_r41_ohne_zahl(uint8_t id);

/* Reichweite des Nahkampf-Kegels FUN_800127FC je Waffe. Waffe 2 -> RE15_PIPE_R41_REICHWEITE (NUTZERWUNSCH "mehr
 * Reichweite", Eichung engine/src/gen/pipe_r41.inc); alle anderen -> der uebergebene Tabellenwert @0x8006E5A0. */
uint32_t re15_pipe_r41_reichweite(int weapon_id, uint32_t tabelle);

/* ARMS-Ton-Bank je Waffe. Waffe 2 -> 1 (PORT-WAHL Vorlage Messer: ARMS02 ist im Original die Pistolen-Bank);
 * sonst die Waffe selbst (FUN_80043D8C laedt ARMS<aca5d>). */
int re15_pipe_r41_arms_bank(int weapon_id);

/* Treffer-Ton 0x01080001 (Satz 8): Original nur Waffe 1 (@0x800123C0-C4); PORT-WAHL Vorlage Messer: auch Waffe 2. */
int re15_pipe_r41_treffer_ton(int weapon_id);

#endif /* RE15_PIPE_R41_H */
