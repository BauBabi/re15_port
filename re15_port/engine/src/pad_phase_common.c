/* =============================================================================================
 * pad_phase_common.c — PHASEN-RIEGEL fuer den R1-Umschalter des Touch-Overlays (Runde 30, Thema C).
 *
 * Dossier: analysis/befunde_runde30/android-r1-toggle.md, Abschnitt 5 Schritt 3.
 *
 * WAS: re15_player_pad_live() sagt, ob der Spieler-Dispatcher das Pad in diesem Bild als
 * SPIELEREINGABE liest. Nur dann darf der R1-Umschalter (platform/pc/src/touch_r1_toggle_pc.h)
 * eine Raste halten. In jeder anderen Phase FAELLT die Raste und R1 ist wieder eine
 * Halte-Taste — die Menues, die Kiste und der Datei-Schirm lesen R1 als FLANKE und brauchen
 * genau die Finger-Flanken, die eine stehende Raste verschlucken wuerde (gemessen, Sonde
 * probe_r30_android_r1_toggle Fall B: NAIV verschluckt den FILE-Sprung, Fall C: NAIV blaettert
 * die Kiste nach dem zweiten Tipp von selbst weiter).
 *
 * PORT-KOMFORTFUNKTION auf Nutzerwunsch, KEIN Original-Verhalten: das Original hat keinen
 * Umschalter. Der Riegel fragt nur Zustaende ab, die der Port bereits byte-true fuehrt; die
 * Original-Mechanismen, auf die sich die einzelnen Zeilen stuetzen (selbst disassembliert,
 * info/Re1.5/PSX.EXE):
 *   Menue    Status-Task Phase 0 @0x800460b8 ruft FUN_80029bf8(0) @0x800460c4 = Task 0
 *            (Spielschleife mit dem Spieler-Dispatcher) |= 0x40 @0x80029c10 -> suspendiert,
 *            solange das Menue lebt; Wiederaufnahme FUN_80029c2c (andi 0xffbf @0x80029c44).
 *   Pause    Spieler-Dispatcher FUN_80031c44: lw a0,g_pauseflags @0x80031c54 /
 *            bltz a0,0x80031da8 @0x80031c78 (Bit 0x80000000 -> kein Kommando-Dispatch).
 *            Pad-Remap FUN_80030444: lui v1,0x100 @0x800304f8 / andi v0,v0,0xf000 @0x80030514 /
 *            sw v0,0x800ac768 @0x8003051c (Bit 0x01000000 kappt das HELD-Wort, R1 = 0x100 faellt).
 *   Blende   Transitions-FSM State 4: g_pauseflags |= 0xff000000 (@0x8001cc5c / @0x8001cc6c),
 *            State 5 gibt erst nach der Blende frei (@0x8001cc8c / @0x8001cc94).
 * Tod und Cutscene sind Port-Zustaende ohne einzelnes Original-Bit (Tod: cmd 3 ersetzt cmd 1,
 * sb v0(=3),4(s1) @0x80012ef4; Cutscene: Plc_motion schreibt cmd 4, sb v1(=4),4(v0) @0x80041bb0).
 *
 * Bewusst NICHT im Riegel (die Raste bleibt stehen): Treffer, Knockdown, Griff, Treppe,
 * Klettern. Dort liest der Dispatcher das Pad zwar nicht, aber die Waffe kommt danach von
 * selbst wieder hoch wie bei gehaltenem R1 (Sonde Fall H: 32 Bilder in HALTEN und RASTE).
 *
 * Kept free of platform code: laeuft identisch im PSX- und PC-Bau (GLOB engine/src/*.c).
 * ============================================================================================= */
#include "re15_player.h"
#include "re15_menu.h"          /* re15_menu_is_open / re15_menu_gameplay_frozen            */
#include "re15_item_modal.h"    /* re15_item_modal_active                                    */
#include "re15_item_discard.h"  /* re15_discard_active                                       */
#include "re15_scd.h"           /* g_re15_pauseflags, RE15_PAUSE_*, g_scd                    */
#include "re15_room.h"          /* re15_room_transition_active, g_room_change,
                                 * re15_death_presentation_active                          */
#include "re15_damage.h"        /* re15_player_is_dead                                       */

int re15_player_pad_live(void)
{
    /* Status-/Karten-/Datei-Schirm, Item-Kiste und die Menue-Blenden (Anforderung gelatcht,
     * Stufen 1-5): der Spielschritt ist komplett eingefroren (game_step_common.c,
     * re15_menu_gameplay_frozen), die R1-Flanken gehoeren dem Menue (menu_common.c,
     * re15_itembox.c). */
    if (re15_menu_is_open() || re15_menu_gameplay_frozen()) return 0;
    /* Item-Modal ("You got ...") und die Wegwerf-Abfrage: eigene Freeze-Returns im
     * Spielschritt (game_step_common.c), das Pad gehoert der Abfrage. */
    if (re15_item_modal_active() || re15_discard_active()) return 0;
    /* Text-Freeze und Tuer-/Raumblende: Spieler-Dispatcher bzw. Pad-Remap gesperrt
     * (@0x80031c78 / @0x80030514, s. Kopf). */
    if (g_re15_pauseflags & (RE15_PAUSE_PLAYER | RE15_PAUSE_PAD)) return 0;
    /* Raumwechsel angefordert bzw. Einblendung laeuft (room_common.c). */
    if (re15_room_transition_active() || g_room_change.pending) return 0;
    /* Skript-Szene: dieselbe Bedingung, mit der der Spielschritt das Inventar sperrt
     * (game_step_common.c, in_cinematic) und der AOT-Scan pausiert (aot_common.c). */
    if (g_scd.player_mode == 2 || g_scd.letterbox_countdown != 0) return 0;
    /* Tod bzw. Todes-Praesentation (auch der Fress-Finisher, re15_death_presentation_active). */
    if (re15_player_is_dead() || re15_death_presentation_active()) return 0;
    return 1;
}
