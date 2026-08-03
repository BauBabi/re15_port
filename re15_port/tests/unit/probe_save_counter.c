/* probe_save_counter.c — DIAGNOSE (kein ctest): Nutzer-Report 2026-08-03:
 * "Bei New Game, wenn ich auf einen bestehenden Save speichere/ueberschreibe,
 *  wird der Zaehler vom aktuellen Save hochgezaehlt. Aber bei einem neuen Spiel
 *  soll der Zaehler natuerlich von vorne losgehen."
 *
 * ORIGINAL (RE1.5 PSX.EXE, FUN_80025c00 = Memory-Card-Screen):
 *   - Save-Count = DAT_800b0fbd, Byte +1 im LIVE-Game-State-Block (GSB)
 *     @0x800b0fbc (0x800b0fbc = Inventar-Kapazitaet, +0x4861 ab Work-Base
 *     0x800ac75c, vgl. lhu 0x4870(s0) @0x80030494).
 *   - SAVE (case 8): memcpy(card+.., &DAT_800b0fbc, 0x1230) @0x800261c4-d8;
 *     danach bei Erfolg DAT_800b0fbd++ @0x80026488-9c
 *     (lbu v0,DAT_800b0fbd; addiu v0,v0,1; sb v0,DAT_800b0fbd).
 *     Der gespeicherte Wert ist der PRE-Inkrement-Stand des LAUFENDEN Spiels —
 *     die Karte/der Ziel-Slot wird fuer die Nummer NIE gelesen.
 *   - LOAD (case 9): memcpy(&DAT_800b0dbc, buf, 0x1430) @0x80026290-a0
 *     restauriert den Zaehler als Teil des Game-State wholesale.
 *   - NEW GAME: DAT_800b0fbd hat KEINEN weiteren Writer (Ghidra-Xrefs:
 *     nur 0x8002648c(R)/0x8002649c(W) + Titel-Digits 0x80026eac/ecc(R)) —
 *     der Zaehler ist reiner Spielzustand (BSS=0 beim Boot), NICHT Karten-Zustand.
 *
 * PORT-HYPOTHESE (main.c): s_save_counter (Datei-static, main.c:491) wird bei
 * NEW GAME NIE zurueckgesetzt, und der Erst-Save-Seed main.c:2626
 *     if (s_save_counter == 0) s_save_counter = re15_memcard_max_save_count(..);
 * liest das MAX ueber ALLE Karten-Slots — ein frisches Spiel erbt also den
 * Zaehler der alten Saves auf der Karte, statt bei 1 zu beginnen.
 *
 * Diese Probe MISST das ueber die echten Port-Funktionen (memcard-Backend +
 * exakt replizierte main.c-Zeilen). */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_savedata.h"
#include "re15_memcard.h"

#define CARD "probe_savecnt_card.mcr"

int main(void)
{
    int fails = 0;
    remove(CARD);

    /* ---- Vorgeschichte: ein ALTES Spiel hat Saves mit Zaehler 7 (Slot 0)
     * und 3 (Slot 2) auf der Karte hinterlassen. ---- */
    re15_savedata_t old7, old3;
    re15_savedata_capture(&old7, 0, 7);
    re15_savedata_capture(&old3, 0, 3);
    if (re15_memcard_save(CARD, 0, &old7, "BIO HAZARD Leon /07/") != 0) { puts("FAIL: card write slot0"); return 1; }
    if (re15_memcard_save(CARD, 2, &old3, "BIO HAZARD Leon /03/") != 0) { puts("FAIL: card write slot2"); return 1; }

    /* ---- MESSUNG 1: NEW GAME (frischer Prozess), erster Save.
     * Exakte Replikation von main.c:2626-2628. ---- */
    uint16_t s_save_counter = 0;                       /* main.c:491 Startwert; NEW GAME setzt ihn NICHT */
    if (s_save_counter == 0)                           /* main.c:2626 */
        s_save_counter = (uint16_t)re15_memcard_max_save_count(CARD);
    int scount = (int)s_save_counter + 1;              /* main.c:2627 */
    printf("[M1] NEW-GAME-Erst-Save: Port-scount = %d (Karte: max=7 in slot0, 3 in slot2)\n", scount);
    printf("[M1] Original-Referenz: DAT_800b0fbd des FRISCHEN Spiels = 0 -> Save #%d unabhaengig von der Karte\n", 0);
    if (scount != 8) { puts("UNERWARTET: Seed kam nicht vom Karten-Max"); fails++; }
    else puts("[M1] BELEGT: der Port erbt den Zaehler vom Karten-Maximum (8 statt Neu-Start)");

    /* ---- MESSUNG 2: der geerbte Zaehler landet wirklich im Save-Datensatz,
     * egal welcher Slot gewaehlt wird (auch ein LEERER Slot 1 erbt). ---- */
    re15_savedata_t ng;
    re15_savedata_capture(&ng, 0, (uint16_t)scount);   /* main.c:2628 */
    if (re15_memcard_save(CARD, 1, &ng, "BIO HAZARD Leon /08/") != 0) { puts("FAIL: card write slot1"); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(CARD, 1, &back) != 0) { puts("FAIL: card read slot1"); return 1; }
    printf("[M2] Neuer-Spielstand in LEEREM Slot 1 traegt save_count=%u (erwartet fuer frisches Spiel: 0/1)\n",
           (unsigned)back.save_count);
    if (back.save_count != 8) fails++;

    /* ---- MESSUNG 3: In-Prozess-Weg (CONTINUE -> Abbruch -> NEW GAME):
     * main.c:2526 setzt s_save_counter = geladener save_count; der Game-Over-/
     * Title-Reset (main.c:4030-4032) memset't nur g_inv/g_game — s_save_counter
     * bleibt stehen, re15_gameflow_new_game() fasst ihn ebenfalls nicht an. ---- */
    re15_savedata_t cont;
    if (re15_memcard_load(CARD, 0, &cont) != 0) { puts("FAIL: card read slot0"); return 1; }
    s_save_counter = cont.save_count;                  /* main.c:2526 (CONTINUE) */
    /* ... NEW GAME: kein Reset-Pfad existiert ... */
    int scount2 = (int)s_save_counter + 1;             /* naechster Save nach NEW GAME */
    printf("[M3] CONTINUE(7) -> NEW GAME -> erster Save: Port-scount = %d (Original: 0)\n", scount2);
    if (scount2 != 8) fails++;
    else puts("[M3] BELEGT: auch in-Prozess erbt NEW GAME den Zaehler des geladenen Saves");

    remove(CARD);
    printf("probe_save_counter: %s (fails=%d)\n", fails ? "DIVERGENZ NICHT REPRODUZIERT?" : "BUG REPRODUZIERT", fails);
    return 0;   /* Diagnose-Probe: Exit 0, Befund steht im stdout */
}
