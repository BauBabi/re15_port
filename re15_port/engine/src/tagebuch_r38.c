/*
 * RE1.5 Rebuilt — Runde 38 Spur G "tagebuch": Roys Diary statt der Code-Nachricht (Elza, ROOM1011), danach die
 * Kartenkette Communications Room -> Prison Room. Herleitung, Belege und alle Konstanten:
 * include/re15_tagebuch_r38.h; Dossier analysis/befunde_runde38/G_tagebuch.md.
 *
 * ABLAUF (je Untersuchen des liegenden Roy, sub07 @0x00E4E):
 *   1. Das Skript erreicht @0x00E64 Message_on 18. Statt die Nachricht zu oeffnen, fordert der Haken den Dokument-
 *      Leser an (re15_menu_request_doc, Runde 30: RE2 FUN_80071ba0) und PARKT den Faden auf dem Opcode.
 *   2. Der Statusschirm laeuft (Spiel samt VM eingefroren, re15_menu_gameplay_frozen). Erstes Mal: Titelseite ->
 *      lesen -> "The Roys Diary has been filed." -> Bit (9,68). Spaeter: nur lesen (menu_common.c, Listenplatz).
 *   3. RUNDE 39 SPUR H (Nutzer: "nach aufnehmen von Roys Diary muss direkt die Karte kommen ... noch bevor das Spiel
 *      zum bewegen wieder zurueckgegeben wird"): wurde das Tagebuch IN DIESEM Lauf abgelegt, fordert
 *      re15_tagebuch_r38_tick die Kartenkette SOFORT an, sobald der Dokument-Schirm samt Rueckblende durch ist —
 *      im selben game_step, in dem die Anforderung angenommen wird (game_step_common.c: tick VOR
 *      re15_map_hint_pending). Der Faden bleibt dabei auf dem Message_on geparkt (die Original-Nachricht msg 18 hielte
 *      mit Maske 0xffff ohnehin alles an, @0x00E64 `2b 12 ff ff`); Elza bleibt gebeugt, keine Steuerung.
 *      Form = RE2-Kartenhinweis (Skript-Opcode 0x84 @0x800591C4: Statusschirm-Modus 4 @0x800591d4, Anforderung
 *      @0x800591e8, Hinweis-Nummer @0x80059210 — ein Skript-Opcode laeuft an der Stelle, an der das Skript ihn ruft,
 *      auch mitten im Ereignis vor Plc_ret). Zeitpunkt = NUTZER-VORGABE (Dossier analysis/befunde_runde39/H_elza.md).
 *      Gemessen vorher: Anforderung erst am Evt_end @0x00E82 -> 41 Bilder sichtbarer Raum (Elza richtet sich auf,
 *      Plc_ret) zwischen Schliessen (F494) und Karte (F535).
 *   4. Ist der Hinweis-Schirm wieder zu, liefert der Haken 1: der PC springt hinter das Message_on, sub07 laeuft
 *      weiter (Evt_next, Elza richtet sich auf @0x00E6A/@0x00E6E, Slot 5 wieder scharf, Evt_end @0x00E82).
 */
#include "re15_tagebuch_r38.h"

#include <stdio.h>
#include <stddef.h>

#include "re15_scd.h"
#include "re15_room.h"
#include "re15_menu.h"
#include "re15_files.h"
#include "re15_map_hint.h"

enum { FREI = 0, LESER = 1, HINWEIS = 2, ZU = 3 };

static int                 s_zustand = FREI;
static const uint8_t      *s_pc      = NULL;   /* das geparkte Message_on                                */
static unsigned            s_raum    = 0;
static int                 s_neu     = 0;      /* 1 = Bit 68 war vor der Anforderung 0 (erstes Lesen)    */

/* Runde 39 Spur H: Uebergang LESER -> HINWEIS / ZU, sobald der Dokument-Schirm durch ist. Laeuft im game_step VOR
 * der Annahme der Hinweis-Anforderung (game_step_common.c), damit die Oeffnen-Blende des Hinweises im selben Bild
 * beginnt, in dem der Dokument-Schirm freigegeben wurde. */
static void leser_zu_pruefen(void)
{
    if (s_zustand != LESER) return;
    if (re15_menu_doc_active() || re15_menu_gameplay_frozen()) return;   /* Anforderung gelatcht, Blenden, Leser, Meldung */
    if (s_neu && re15_game_flag_get(RE15_TAGEBUCH_R38_BANK, RE15_TAGEBUCH_R38_BIT)) {
        const int k = re15_map_hint_eintrag_fuer(RE15_TAGEBUCH_R38_RAUM, RE15_TAGEBUCH_R38_ZIEL1_RAUM);
#ifdef RE15_PLATFORM_PC
        int p1 = -1, r1 = -1, p2 = -1, r2 = -1;
        (void)re15_map_hint_ziel(k, &p1, &r1);
        (void)re15_map_hint_ziel(re15_map_hint_folge(k), &p2, &r2);
        fprintf(stderr, "[tagebuch] Leser zu: Bit (%d,%d)=1, Kartenkette sofort (Runde 39 Spur H): Eintrag %d (ROOM%04X "
                "Blatt %d Rechteck %d) -> Folge %d (ROOM%04X Blatt %d Rechteck %d)\n", RE15_TAGEBUCH_R38_BANK,
                RE15_TAGEBUCH_R38_BIT, k, RE15_TAGEBUCH_R38_ZIEL1_RAUM, p1, r1, re15_map_hint_folge(k),
                RE15_TAGEBUCH_R38_ZIEL2_RAUM, p2, r2);
#endif
        if (k >= 0) { re15_map_hint_request(k); s_zustand = HINWEIS; return; }
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[tagebuch] Leser zu: Bit (%d,%d)=%d, kein Hinweis (schon abgelegt)\n", RE15_TAGEBUCH_R38_BANK,
            RE15_TAGEBUCH_R38_BIT, re15_game_flag_get(RE15_TAGEBUCH_R38_BANK, RE15_TAGEBUCH_R38_BIT));
#endif
    s_zustand = ZU;
}

int re15_tagebuch_r38_message_on(const scd_thread_t *t)
{
    if (!t || g_current_room_id != RE15_TAGEBUCH_R38_RAUM || t->pc[1] != RE15_TAGEBUCH_R38_MSG) return 0;

    if (s_zustand != FREI && s_pc == t->pc && s_raum == g_current_room_id) {
        if (s_zustand == LESER) leser_zu_pruefen();          /* falls der tick in diesem Bild noch nicht lief */
        if (s_zustand == LESER) return 2;                    /* der Schirm laeuft noch -> weiter parken */
        if (s_zustand == HINWEIS) {
            /* Runde 39 Spur H: parken, bis der Hinweis-Schirm angenommen UND wieder zu ist (Anforderung offen,
             * Hinweis-Task lebt, Blenden). */
            if (re15_map_hint_pending() >= 0 || re15_menu_map_hint_active() || re15_menu_gameplay_frozen()) return 2;
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[tagebuch] Hinweis zu: Faden laeuft hinter dem Message_on weiter (Elza richtet sich auf)\n");
#endif
        }
        s_zustand = FREI;
        return 1;
    }

    /* Anfordern. Ohne Dokument-Eintrag oder bei belegtem Menue nimmt re15_menu_request_doc nichts an -> parken und
     * im naechsten Bild erneut (der Faden steht wie bei der Original-Nachricht, die mit Maske 0xffff ohnehin
     * alles anhaelt). Kein Aufhebe-Slot (-1): Slot 5 legt sub07 selbst still und wieder an (@0x00E4E/@0x00E78);
     * kein Weltmodell (-1, NUTZER-VORGABE "Muss nicht unbedingt ein Welt Model haben"). */
    s_neu = !re15_game_flag_get(RE15_TAGEBUCH_R38_BANK, RE15_TAGEBUCH_R38_BIT);
    re15_menu_request_doc(RE15_TAGEBUCH_R38_DOK, RE15_TAGEBUCH_R38_BIT, -1, -1);
    if (!re15_menu_doc_active()) return 2;
    s_zustand = LESER;
    s_pc      = t->pc;
    s_raum    = g_current_room_id;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[tagebuch] ROOM%04X msg %d -> Roys Diary (Dokument %d, Bildsatz %d, %s)\n",
            (unsigned)g_current_room_id, RE15_TAGEBUCH_R38_MSG, RE15_TAGEBUCH_R38_DOK, RE15_TAGEBUCH_R38_BILDSATZ,
            s_neu ? "erstes Mal: ablegen" : "schon abgelegt: nur lesen");
#endif
    return 2;
}

void re15_tagebuch_r38_tick(void)
{
    /* Raumwechsel: Zustand und Kette gehoeren zum Raum, in dem sie entstanden (der Statusschirm haelt das Spiel an,
     * waehrend der Leser laeuft — ein Raumwechsel dazwischen ist ein neuer Raumaufbau). */
    if (g_current_room_id != s_raum) { s_zustand = FREI; s_raum = g_current_room_id; return; }
    leser_zu_pruefen();   /* Runde 39 Spur H: Kartenkette sofort nach dem Dokument-Schirm (vor re15_map_hint_pending) */
}

int re15_tagebuch_r38_listenplatz(int doc)
{
    for (int platz = 0; platz < RE15_FILES_SLOTS; platz++)
        if (re15_files_get(platz) == doc) return platz;
    return -1;
}

/* 1 = die Kartenkette ist angefordert und der Faden wartet noch auf den Hinweis-Schirm (Runde 39 Spur H: statt der
 * frueheren Evt_end-Kette). */
int re15_tagebuch_r38_hinweis_faellig(void) { return s_zustand == HINWEIS; }
