/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Nachbesserung 2 (E-N2): Nachrichten-Takt NACH dem Spiel-Takt
 * (Hauptschleife @0x80020de4 Spiel-Task < @0x80020f3c/@0x80010044 Nachrichten-Takt) und Auswahl-Message_on ohne
 * Parken (Rueckgabe 1 @0x80040520, Faden haelt der Skript-Freeze @0x8003f040-@0x8003f04c). Herleitung und Messung:
 * include/re15_msgtakt_r41.h; Dossier analysis/befunde_runde41/E_kleinkram_b.md ("Nachbesserung 2").
 */
#include "re15_msgtakt_r41.h"

#include "re15_scd.h"   /* g_scd, RE15_PAUSE_SCD, re15_pauseflags_close */

static int      s_aktiv;    /* die Plattform faehrt die Original-Reihenfolge (PC-main) */
static int      s_scharf;   /* gerade im Nachrichten-Takt des Hauptloops */
static int      s_offen;    /* ein Restore ist aufgeschoben */
static int      s_frage;    /* eine Auswahl-Nachricht lief ohne Parken: message_query gehoert dem Restore */
static unsigned s_zahl;

static void restore_jetzt(void)
{
    if (s_offen) {
        s_offen = 0;
        re15_pauseflags_close();   /* `lw DAT_800b853c` -> `sw g_pauseflags` (@0x800285a4/@0x800286cc/@0x8002871c) */
    }
    if (s_frage && !g_scd.message_active) {
        g_scd.message_query = 0;   /* das Port-Tor (msg_block) faellt mit dem Restore, nicht schon im Nachrichten-Takt */
        s_frage = 0;
    }
}

void re15_msgtakt_r41_takt_beginn(void)
{
    restore_jetzt();               /* ein Bild ohne Spiel-Takt (Tuer, Menue) holt ihn hier nach */
    s_aktiv  = 1;
    s_scharf = 1;
}

void re15_msgtakt_r41_takt_ende(void) { s_scharf = 0; }

int re15_msgtakt_r41_schliessen_aufschieben(void)
{
    if (!s_scharf) return 0;
    s_offen = 1;
    s_zahl++;
    return 1;
}

void re15_msgtakt_r41_vor_oeffnen(void) { if (s_offen) { s_offen = 0; re15_pauseflags_close(); } }

void re15_msgtakt_r41_nach_spieltakt(void) { restore_jetzt(); }

int re15_msgtakt_r41_frage_durchlaufen(uint32_t pause_mask)
{
    if (!s_aktiv || !(pause_mask & RE15_PAUSE_SCD)) return 0;
    s_frage = 1;
    return 1;
}

int      re15_msgtakt_r41_aktiv(void)              { return s_aktiv; }
int      re15_msgtakt_r41_restore_offen(void)      { return s_offen; }
unsigned re15_msgtakt_r41_aufgeschoben_zahl(void)  { return s_zahl; }
void     re15_msgtakt_r41_zuruecksetzen(void)      { s_aktiv = s_scharf = s_offen = s_frage = 0; s_zahl = 0; }
