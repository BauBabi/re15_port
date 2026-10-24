/*
 * Runde 37 Spur P (masken36): Generator-Masken (R15M ohne eigenen Atlas) nur dort zeichnen, wo sie keine Figur VOR jedem
 * Objekt uebermalen. Dossier analysis/befunde_runde37/P_masken36.md (§3, §4).
 *
 * Seit Spur P zeichnet der Lader die Generator-Masken vom 2026-09-03 wieder mit dem BSS-Atlas des Cuts
 * (re15_pri_atlas_quelle, include/re15_masken36_r37.h). Gemessen an der exe (Dossier §4): in einem Teil der Cuts verdecken
 * einzelne Rechtecke Leon, obwohl zwischen Kamera und Leon NICHTS steht — Bodenflaechen in der Auswahl (ROOM30E0 Cut 7,
 * ROOM3050 Cut 6, ROOM5050 Cut 2), Hintergrund in der Auswahl (ROOM3050 Cut 5: Leon frei im Gang bis auf den Kopf
 * geloescht) und das Tiefenband des Generators (DEPTH_SAFETY 0.90: die Maske liegt um 10 % naeher als der Bodenkontakt).
 * Das ist die Fehlerklasse, die der Nutzer am 2026-09-03 gemeldet hat ("Leon verschwindet fast vollstaendig", Commit
 * 6c29d834: "Eine fehlende Maske laesst alles wie bisher, eine falsche uebermalt den Spieler").
 *
 * Diese Tabelle nimmt genau die Rechtecke heraus, die an einem erreichbaren Stehplatz einen Figurpunkt verdecken, ohne dass
 * zwischen Kamera und Figurpunkt eine solide Kollisionszelle des Bands liegt (SCA-Zellen sind die Waende/Gegenstaende,
 * re15_collision.c Kopf; Spielerradius 450 = DAT_80073e94[6]). Erzeugt von tests/unit/test_r37_masken36.c ("erzeuge"),
 * der Riegel unit_r37_masken36_ausschluss rechnet sie aus den Dateien nach. Alle uebrigen Rechtecke zeichnet der Lader wie
 * am 2026-09-03 gebaut. MASKS/ * bleibt unveraendert.
 *
 * Nachbesserung 2 (Abnahme 1, Dossier "Nachbesserung 2"): die Figur wird pixelgenau abgetastet (Kopfoberkante bis Fuss;
 * vorher 9 x 9 Zeilenmitten - ROOM30A0 Cut 0 schnitt Leon das Haar ab), und eine Sektion OHNE Stehplatz ist ganz aus
 * (ROOM3050/3051 Cut 15: die Masken uebermalten den Konsolen-Cursor; das Original hat dort die NULL-Sektion FF FF FF FF,
 * FUN_800392d4 @0x80039324 lw / @0x8003932c bne -1 / @0x80039338 sb zero = keine Maske).
 */
#include <string.h>
#include "re15_pri.h"
#include "re15_masken36_r37.h"

typedef struct {
    uint16_t raum;        /* Raum-Id der MSK-Datei (re15_pri_masken_raum) */
    uint8_t  cut;
    uint8_t  anzahl;      /* Zahl der ausgeschlossenen Rechtecke (Kontrolle) */
    uint32_t bits[4];     /* Bit i = Rechteck i der Sektion (Reihenfolge wie re15_pri_parse_section) */
} re15_masken36_zeile_t;

#include "gen/masken36_ausschluss_r37.inc"

int re15_masken36_ausgeschlossen(unsigned masken_raum, int cut, int idx)
{
    size_t i;
    if (cut < 0 || idx < 0 || idx >= 128) return 0;
    for (i = 0; i < sizeof re15_masken36_zeilen / sizeof re15_masken36_zeilen[0]; i++) {
        const re15_masken36_zeile_t *z = &re15_masken36_zeilen[i];
        if (z->raum == (uint16_t) masken_raum && z->cut == (uint8_t) cut)
            return (int) ((z->bits[idx >> 5] >> (idx & 31)) & 1u);
    }
    return 0;
}

int re15_masken36_filter(unsigned masken_raum, int cut, re15_pri_cut_t *pri)
{
    int i, n = 0, d = 0;
    if (!pri) return 0;
    for (i = 0; i < pri->mask_count; i++) {
        if (re15_masken36_ausgeschlossen(masken_raum, cut, i)) continue;
        if (i < pri->draw_count) d++;
        if (n != i) pri->masks[n] = pri->masks[i];
        n++;
    }
    pri->mask_count = n;
    pri->draw_count = d;
    return n;
}

int re15_masken36_zeilen_zahl(void)
{
    return (int) (sizeof re15_masken36_zeilen / sizeof re15_masken36_zeilen[0]);
}
