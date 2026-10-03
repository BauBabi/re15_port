/*
 * RE1.5 Rebuilt — Kampfmesser als Rueckfall statt Inventar-Gegenstand (Runde 35, Spur E).
 *
 * NUTZER-VORGABE (analysis/befunde_runde35/AUFTRAG.md Z.21, woertlich):
 *   "Dann moechte ich, das du das Kampfmesser aus den Player Inventar raus nimmst. Das Kampfmesser
 *    soll IMMER der fallback sein, wenn keine Waffe ausgewaehlt ist. Dafuer muss es nicht noch extra
 *    im Player Inventory liegen."
 *
 * Die FORM ist die des Originals — die Rueckfall-Regel steht schon in PSX.EXE:
 *   Ausruest-Commit beim Schliessen des Statusschirms @0x80046654..88:
 *     80046658 lbu v1,9672(v1)      25c8 = ausgeruesteter PLATZ
 *     8004665c ori v0,zero,0x80     0x80 = nichts ausgeruestet
 *     80046660 bne v1,v0,0x80046670
 *     80046668 j 0x80046680 / 8004666c ori v0,zero,0x1     -> Waffe 1 = COMBAT KNIFE
 *     80046688 sb v0,-13731(at)     0x800aca5d = Waffen-Id
 *   Ablegen derselben Waffe im Statusschirm (UNEQUIP @0x8004aaec) setzt 25c8 := 0x80.
 * Das Messer liegt im Original nur deshalb im Inventar, weil das Spielstart-Init es als ersten
 * Gegenstand eintraegt (Tabelle @0x80074bb8 / @0x80074bc4, 25c8 := 0 @0x80045fec). Neu ist allein,
 * dass dieser eine Eintrag entfaellt und das Spiel mit 25c8 = 0x80 beginnt.
 * Alle Belege: analysis/befunde_runde35/E_inventar1050.md (RE-Belege P2).
 */
#ifndef RE15_MESSER_H
#define RE15_MESSER_H

#include <stdint.h>

/* Item 1 = COMBAT KNIFE: erste Zeile der Leon-Starttabelle @0x80074bb8 (`01 03 15 ..`), Elza @0x80074bc4
 * (`01 00 ..`); Commit-Rueckfall `ori v0,zero,0x1` @0x8004666c. */
#define RE15_MESSER_ID          0x01

/* 25c8-Wert "nichts ausgeruestet": `ori v0,zero,0x80` @0x8004665c (Commit), 25c9-Init @0x80045fe0. */
#define RE15_MESSER_NICHTS      0x80

/* Spielstart (ersetzt den Aufruf re15_inv_load_briefing in platform/pc/main.c): Starttabelle des
 * Originals je Charakter (`sltiu v0,v0,0x4` @0x80045e28 auf 0x800aca5c = g_gameflow.character)
 * OHNE das Messer, Ausruest-Platz 0x80, Vorgaenger 0x80, Waffen-Id 1. */
void re15_messer_startinventar(void);

/* Spielstand-Laden (Haken re15_savedata.c, nach dem Kisten-Import): ein Messer aus einem alten
 * Spielstand (Inventar ODER Kiste) wird entfernt. War es ausgeruestet, gilt danach 0x80 -> Messer.
 * Liefert die Zahl entfernter Messer (Inventar + Kiste). */
int  re15_messer_aus_inventar(void);

#endif /* RE15_MESSER_H */
