/* probe_save_injured.c — DIAGNOSE (kein ctest): Nutzer-Frage 2026-08-03:
 * "Speichert das Spiel die Schadensanimationen/-texturen der Charaktere
 *  (Caution/Danger) mit? Oder haben sie nach dem Laden wieder ihre
 *  unverletzten Texturen/Animationen?"
 *
 * ORIGINAL (RE1.5 MZD, dormantes Card-System — Belege analysis/save_injured_state.md):
 *   - Save-Blob = 0x800 Bytes ab 0x800b0dbc (write(fd,0x800b0dbc,0x800):
 *     Laenge @0x80026220 `ori v0,zero,0x800`, Basis @0x800261f8) =
 *     0x200 Header + die ersten 0x600 des Game-State-Blocks @0x800b0fbc.
 *   - Die WUND-TABELLE @0x800b10ec (8 Panels x 0x1c, Level+Akku) liegt bei
 *     GSB+0x130..+0x210 => im Blob (File-Offset 0x330..0x410): die Blut-Level
 *     WERDEN mitgespeichert und beim Load restauriert
 *     (memcpy(0x800b0dbc, buf, 0x1430) @0x80026290-a0).
 *   - ABER: HP wird NICHT gespeichert (Capture FUN_80026f48 schreibt nur
 *     Slot+X/Yaw/Z in den GSB; HP @0x800acaee liegt ausserhalb; Xref-Zensus
 *     geschlossen), der Spieler-Load setzt HP:=100 (@0x80031718) und nullt
 *     die Wund-Tabelle (FUN_80037c1c, einzige jal-Sites @0x800316c8/@0x800318cc),
 *     und der Re-Apply-Pfad LAB_80037d1c hat 0 Caller (Binary-Scan EXE+alle BINs).
 *
 * PORT-HYPOTHESEN (diese Probe MISST beide):
 *   M1/M2: re15_savedata_t hat KEIN Wund-Feld -> Save mit Danger-HP+Blut,
 *          frischer Boot, Load: HP kommt zurueck (=> Injured-Idle-Anims
 *          player_common.c:551-590 automatisch korrekt), Blut-Level = 0
 *          (Leon laedt "sauber", obwohl er blutig gespeichert wurde).
 *   M3:    der CONTINUE-Pfad (main.c:1786-93) ruft re15_gameflow_new_game
 *          NICHT -> re15_wound_reset() entfaellt -> Tod-mit-Blut -> Title ->
 *          CONTINUE eines SAUBEREN Saves startet BLUTIG (stale carry-over;
 *          der Wound-Sync main.c:5235ff stempelt die stale Level auf die
 *          frisch geladene Textur). */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_actor.h"
#include "re15_damage.h"

#define CARD "probe_saveinj_card.mcr"

/* Ground-Truth-Wundbild aus stage_saves/mzd_blood_decals_hp30.sav
 * (analysis/blood_decals.md §3.4): 3x Hurt-Substate-0-Helper 0x8010a1cc =
 * je Panel0+10, Panel5+50, Panel7+50 -> Level: p5=1, p7=1, p0=0 (Akku 30). */
static void inflict_groundtruth_wounds(void)
{
    for (int i = 0; i < 3; i++) {
        re15_wound_add(0, 10);
        re15_wound_add(5, 50);
        re15_wound_add(7, 50);
    }
}

static void dump_levels(const char *tag)
{
    printf("%s: wound levels =", tag);
    for (int p = 0; p < 8; p++) printf(" %d", re15_wound_level(p));
    printf("  hp=%d\n", (int)g_actors[0].hp);
}

int main(void)
{
    int fails = 0;
    remove(CARD);

    /* ---- Spielstand wie der Ground-Truth-Save: Danger-HP + Blut ---- */
    re15_wound_reset();                       /* New-Game-Basis (re15_gameflow_new_game) */
    g_actors[0].hp = 30;                      /* Danger (<0x1e-Schwelle @0x800322a4 knapp drueber,
                                               * Injured-Idle 2 <30 im Port; GT-Save hatte hp=30) */
    inflict_groundtruth_wounds();
    dump_levels("[M0] Zustand beim SPEICHERN");
    if (re15_wound_level(5) != 1 || re15_wound_level(7) != 1 || re15_wound_level(0) != 0) {
        puts("FAIL: Wund-Akkumulator reproduziert das Ground-Truth-Bild nicht (p5/p7=1, p0=0)");
        return 1;
    }

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 1234, 1);

    /* ---- M1: enthaelt der serialisierte Block ueberhaupt Wund-Zustand? ----
     * re15_savedata_t hat kein Wund-Feld (Compile-Fakt; capture liest s_wounds
     * nie). Messbar: Block byte-identisch, egal ob mit oder ohne Wunden. */
    re15_wound_reset();
    re15_savedata_t sd_clean;
    re15_savedata_capture(&sd_clean, 1234, 1);
    if (memcmp(&sd, &sd_clean, sizeof sd) == 0)
        puts("[M1] BELEGT: Save-Block MIT Blut == Save-Block OHNE Blut (byte-identisch)"
             " -> Wund-Tabelle wird NICHT serialisiert");
    else {
        puts("[M1] UNERWARTET: Bloecke differieren -> Wund-Zustand steckt doch im Save?");
        fails++;
    }

    /* ---- M2: Voll-Roundtrip ueber die echte Karte, frischer Boot ---- */
    if (re15_memcard_save(CARD, 0, &sd, "BIO HAZARD Leon /01/") != 0) { puts("FAIL: card write"); return 1; }
    /* frischer Prozess-Zustand: New Game war der letzte Reset, Wunden 0, hp voll */
    g_actors[0].hp = 100;
    re15_savedata_t ld;
    if (re15_memcard_load(CARD, 0, &ld) != 0) { puts("FAIL: card read"); return 1; }
    uint16_t room = 0;
    if (re15_savedata_restore(&ld, &room) != 0) { puts("FAIL: restore"); return 1; }
    dump_levels("[M2] Zustand nach LOAD (frischer Boot)");
    if (g_actors[0].hp == 30)
        puts("[M2] HP wird restauriert -> Injured-Idle-Anims (HP-getrieben pro Frame,"
             " player_common.c:551-590) sind nach dem Laden automatisch korrekt");
    else { printf("[M2] FAIL: hp=%d statt 30\n", (int)g_actors[0].hp); fails++; }
    if (re15_wound_level(5) == 0 && re15_wound_level(7) == 0)
        puts("[M2] BELEGT: Blut-Decals sind nach dem Laden WEG (Level 0) —"
             " Leon war beim Speichern blutig (p5/p7=1)");
    else { puts("[M2] UNERWARTET: Wund-Level ueberlebten den Load"); fails++; }

    /* ---- M3: stale carry-over ueber den CONTINUE-Pfad ----
     * main.c:1786-93 (LOAD GAME) setzt g_gameflow direkt und ruft
     * re15_gameflow_new_game()/re15_wound_reset() NICHT; der Gameover-Pfad
     * main.c:4030-33 memset't nur g_inv/g_game. Simulation: Tod mit Blut,
     * dann CONTINUE eines sauberen Saves. */
    inflict_groundtruth_wounds();             /* der Run, in dem der Spieler stirbt */
    /* CONTINUE: kein re15_wound_reset() — exakt der Port-Codepfad */
    if (re15_savedata_restore(&sd_clean, &room) != 0) { puts("FAIL: restore clean"); return 1; }
    dump_levels("[M3] Zustand nach Tod->Title->CONTINUE (sauberer Save)");
    if (re15_wound_level(5) != 0 || re15_wound_level(7) != 0) {
        puts("[M3] BELEGT (BUG): der geladene SAUBERE Save startet mit dem Blut des"
             " gestorbenen Runs — CONTINUE ruft re15_wound_reset() nicht");
        /* kein fails++: das IST der erwartete Defekt-Nachweis */
    } else {
        puts("[M3] Kein stale carry-over messbar (Reset-Pfad vorhanden?)");
        fails++;
    }

    remove(CARD);
    printf("\nprobe_save_injured: %s (fails=%d)\n", fails ? "UNERWARTETE ABWEICHUNG" : "alle Messungen wie statisch belegt", fails);
    return fails ? 1 : 0;
}
