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
 *   3. Ist der Schirm samt Rueckblende durch, liefert der Haken 1: der PC springt hinter das Message_on, sub07
 *      laeuft weiter (Evt_next, Elza richtet sich auf, Slot 5 wieder scharf, Evt_end @0x00E82).
 *   4. Wurde das Tagebuch IN DIESEM Lauf abgelegt, fordert re15_tagebuch_r38_tick die Kartenkette an, sobald der
 *      Faden das Evt_end hinter sich hat — wie Leons Hinweis am Evt_end von ROOM1150 sub08.
 */
#include "re15_tagebuch_r38.h"

#include <stdio.h>
#include <stddef.h>

#include "re15_scd.h"
#include "re15_room.h"
#include "re15_menu.h"
#include "re15_files.h"
#include "re15_map_hint.h"

enum { FREI = 0, LESER = 1 };

static int                 s_zustand = FREI;
static const uint8_t      *s_pc      = NULL;   /* das geparkte Message_on                                */
static const scd_thread_t *s_faden   = NULL;   /* sein Faden (Kette nach dessen Evt_end)                 */
static unsigned            s_raum    = 0;
static int                 s_neu     = 0;      /* 1 = Bit 68 war vor der Anforderung 0 (erstes Lesen)    */
static int                 s_kette   = 0;      /* 1 = Kartenkette wartet auf das Evt_end des Fadens      */

int re15_tagebuch_r38_message_on(const scd_thread_t *t)
{
    if (!t || g_current_room_id != RE15_TAGEBUCH_R38_RAUM || t->pc[1] != RE15_TAGEBUCH_R38_MSG) return 0;

    if (s_zustand == LESER && s_pc == t->pc && s_raum == g_current_room_id) {
        /* Der Schirm laeuft noch (Anforderung gelatcht, Blenden, Leser, Meldung) -> weiter parken. */
        if (re15_menu_doc_active() || re15_menu_gameplay_frozen()) return 2;
        s_zustand = FREI;
        if (s_neu && re15_game_flag_get(RE15_TAGEBUCH_R38_BANK, RE15_TAGEBUCH_R38_BIT)) {
            s_kette = 1;
            s_faden = t;
        }
        fprintf(stderr, "[tagebuch] Leser zu: Bit (%d,%d)=%d, %s\n", RE15_TAGEBUCH_R38_BANK, RE15_TAGEBUCH_R38_BIT,
                re15_game_flag_get(RE15_TAGEBUCH_R38_BANK, RE15_TAGEBUCH_R38_BIT),
                s_kette ? "Kartenkette nach dem Evt_end" : "kein Hinweis (schon abgelegt)");
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
    s_kette   = 0;
    fprintf(stderr, "[tagebuch] ROOM%04X msg %d -> Roys Diary (Dokument %d, Bildsatz %d, %s)\n",
            (unsigned)g_current_room_id, RE15_TAGEBUCH_R38_MSG, RE15_TAGEBUCH_R38_DOK, RE15_TAGEBUCH_R38_BILDSATZ,
            s_neu ? "erstes Mal: ablegen" : "schon abgelegt: nur lesen");
    return 2;
}

/* Steht der Faden noch zwischen dem Message_on und seinem Evt_end? */
static int faden_im_ereignis(void)
{
    if (!s_faden || !s_pc || !s_faden->active) return 0;
    return s_faden->pc >= s_pc && s_faden->pc <= s_pc + RE15_TAGEBUCH_R38_EVT_END;
}

void re15_tagebuch_r38_tick(void)
{
    if (!s_kette) return;
    if (g_current_room_id != s_raum) { s_kette = 0; return; }   /* Raum verlassen: gehoert nicht mehr hierher */
    if (faden_im_ereignis()) return;
    s_kette = 0;
    {
        const int k = re15_map_hint_eintrag_fuer(RE15_TAGEBUCH_R38_RAUM, RE15_TAGEBUCH_R38_ZIEL1_RAUM);
        int p1 = -1, r1 = -1, p2 = -1, r2 = -1;
        (void)re15_map_hint_ziel(k, &p1, &r1);
        (void)re15_map_hint_ziel(re15_map_hint_folge(k), &p2, &r2);
        fprintf(stderr, "[tagebuch] Evt_end erreicht: Kartenkette Eintrag %d (ROOM%04X Blatt %d Rechteck %d) -> "
                "Folge %d (ROOM%04X Blatt %d Rechteck %d)\n", k, RE15_TAGEBUCH_R38_ZIEL1_RAUM, p1, r1,
                re15_map_hint_folge(k), RE15_TAGEBUCH_R38_ZIEL2_RAUM, p2, r2);
        if (k >= 0) re15_map_hint_request(k);
    }
}

int re15_tagebuch_r38_listenplatz(int doc)
{
    for (int platz = 0; platz < RE15_FILES_SLOTS; platz++)
        if (re15_files_get(platz) == doc) return platz;
    return -1;
}

int re15_tagebuch_r38_hinweis_faellig(void) { return s_kette; }
