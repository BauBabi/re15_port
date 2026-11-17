/* stimme_option.c — Runde 41 Stimmen: Port-Option "VOICE  ON|OFF" (Zustand, Datei-Schluessel, Stopp-Hook).
 * Begruendung und Zensus der Stimmen-Quellen: include/re15_stimme_option.h. Kein SDL, keine Datei-Ein-/Ausgabe
 * ausser getenv. */
#include "re15_stimme_option.h"
#include "re15_dining_r38.h"   /* RE15_R38F_RAUM_1141 */
#include "re15_dining_r39.h"   /* RE15_R39J_TUERTON_ID (door_sound.wav) */
#include "re15_johnruf_r39.h"  /* RE15_R39P_RAUM_1041/1131, RE15_R39P_HILFE_ID0/_N (help01..05 in 1041/1131) */
#include "re15_annette_r41.h"  /* RE15_AN_RAUM, RE15_AN_MSG_SCHREI (main41) */
#include "re15_elza_r36.h"     /* RE15_ELZA_RAUM_1011, RE15_ELZA_STOEHNEN_MSG_ID (main16.5) */

#include <stdlib.h>
#include <string.h>

static int  s_an = 1;                 /* Standard ON (heutiger Stand) */
static void (*s_stopp)(void) = 0;

int  re15_stimme_option(void) { return s_an; }

/* NUTZERWUNSCH-Auslegung (17.11.2026): "Voiceover" = gesprochene Dialogzeilen MIT Untertitel. Laute ohne Untertitel
 * sind Hinweise bzw. Geraeusche und bleiben bei OFF hoerbar. Alle anderen (Raum, Id) bleiben bei OFF stumm. */
int re15_stimme_option_ist_geraeusch(unsigned room_id, int voice_id)
{
    const unsigned raum = room_id & 0xFFFFu;
    const int hilfe = voice_id >= RE15_R39J_HILFE_ID0 && voice_id < RE15_R39J_HILFE_ID0 + RE15_R39J_HILFE_N;
    if (raum == RE15_R38F_RAUM_1141)                         /* Tuerton 59, Johns Ruf main42, Hilferufe help01..05 */
        return voice_id == RE15_R39J_TUERTON_ID || voice_id == RE15_R38F_HILFE_STIMME || hilfe;
    if (raum == RE15_R39P_RAUM_1041 || raum == RE15_R39P_RAUM_1131)   /* dieselben help01..05 (Ids wie 1141) */
        return voice_id >= RE15_R39P_HILFE_ID0 && voice_id < RE15_R39P_HILFE_ID0 + RE15_R39P_HILFE_N;
    if (raum == RE15_AN_RAUM)       return voice_id == RE15_AN_MSG_SCHREI;          /* Annettes Schrei main41 */
    if (raum == RE15_ELZA_RAUM_1011) return voice_id == RE15_ELZA_STOEHNEN_MSG_ID;  /* Stoehnen main16.5 (Id 24) */
    return 0;
}

int re15_stimme_option_spielt(unsigned room_id, int voice_id)
{
    return s_an || re15_stimme_option_ist_geraeusch(room_id, voice_id);
}

void re15_stimme_option_stopp_hook(void (*fn)(void)) { s_stopp = fn; }

void re15_stimme_option_set(int an)
{
    an = an ? 1 : 0;
    if (an == s_an) return;
    s_an = an;
    if (!an && s_stopp) s_stopp();    /* laufende Aufnahme endet, der Untertitel bleibt */
}

int re15_stimme_option_parse(const char *text)
{
    int wert = -1;
    const char *p = text;
    if (!p) return -1;
    while (*p) {
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (!*p) break;
        if (!strncmp(p, RE15_STIMME_SCHLUESSEL, sizeof RE15_STIMME_SCHLUESSEL - 1)) {
            const char *w = p + sizeof RE15_STIMME_SCHLUESSEL - 1;
            if (!strncmp(w, "on", 2) || *w == '1') wert = 1;
            else if (!strncmp(w, "off", 3) || *w == '0') wert = 0;
        }
        while (*p && *p != '\n') p++;
    }
    return wert;
}

void re15_stimme_option_aus_text(const char *text)
{
    int w = re15_stimme_option_parse(text);
    const char *e = getenv(RE15_STIMME_ENV);
    if (e && *e) w = (!strcmp(e, "0") || !strcmp(e, "off") || !strcmp(e, "OFF")) ? 0 : 1;
    re15_stimme_option_set(w != 0);   /* fehlt der Schluessel (-1) -> ON */
}
