/* test_r41_stimmen.c — Runde 41 Stimmen: Port-Option "VOICE  ON|OFF" (include/re15_stimme_option.h).
 *
 * unit_stimmen_r41:
 *   A  Schalter: Standard ON, Parser voice=on|off (auch CRLF), fehlender Schluessel -> -1, Laden ohne Schluessel -> ON,
 *      Messhaken RE15_VOICE uebersteuert.
 *   B  Options-Datei (re15_options.cfg ueber RE15_OPTIONS_FILE): bei ON ist die Datei Byte fuer Byte die bisherige
 *      ("volume=50\nsound=stereo\n"), bei OFF kommt genau "voice=off\n" dazu (auch hinter ultimate=on), Rundreise
 *      Speichern -> Laden ergibt denselben Zustand; eine Datei ohne voice-Zeile (alle bisherigen) laedt ON.
 *   C  Stopp-Hook: der Wechsel ON -> OFF ruft ihn genau einmal (laufende Aufnahme endet), OFF -> OFF und OFF -> ON nicht.
 *   D  Was stumm wird: bei OFF spielt kein synchro-Dialog mit Untertitel (Beispiele 1170/0, 1240/0, Rekorder 1100/40,
 *      1011/16, 1141/40, 5090/40, 1041/40), wohl aber der Tuerton door_sound.wav (1141/59) und - NUTZERWUNSCH-Auslegung
 *      17.11.2026 - die Laute ohne Untertitel: help01..05 (1141/1041/1131 Id 53..57), Johns Ruf 1141/42, Annettes
 *      Schrei 5090/41, Stoehnen main16.5 (1011/24); bei ON spielt alles.
 */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_stimme_option.h"
#include "re15_ultimate15.h"
#include "re15_lautstaerke_r41.h"
#include "re15_dining_r39.h"

static int s_fails = 0, s_checks = 0;
#define CHECK(cond, ...) do { s_checks++; if (!(cond)) { s_fails++; printf("FAIL: "); } else printf("ok:   "); \
                              printf(__VA_ARGS__); printf("\n"); } while (0)

static void env_setzen(const char *name, const char *wert)
{
#ifdef _WIN32
    char buf[1024]; snprintf(buf, sizeof buf, "%s=%s", name, wert ? wert : ""); _putenv(buf);
#else
    if (wert) setenv(name, wert, 1); else unsetenv(name);
#endif
}

static int datei_lesen(const char *pfad, char *buf, size_t n)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) { buf[0] = 0; return -1; }
    size_t g = fread(buf, 1, n - 1, f); fclose(f); buf[g] = 0;
    return (int)g;
}

static void datei_schreiben(const char *pfad, const char *text)
{
    FILE *f = fopen(pfad, "wb"); if (!f) return; fputs(text, f); fclose(f);
}

/* ---- A Schalter ------------------------------------------------------------------------------------------------ */
static void teil_a(void)
{
    CHECK(re15_stimme_option() == 1, "Standard ON");
    CHECK(re15_stimme_option_parse("volume=50\nsound=stereo\nvoice=off\n") == 0, "Parser: voice=off -> 0");
    CHECK(re15_stimme_option_parse("voice=on\r\nvolume=50\r\n") == 1, "Parser: voice=on (CRLF) -> 1");
    CHECK(re15_stimme_option_parse("volume=50\nsound=stereo\nultimate=on\n") == -1, "Parser: kein Schluessel -> -1");
    CHECK(re15_stimme_option_parse(NULL) == -1, "Parser: NULL -> -1");
    re15_stimme_option_aus_text("volume=50\nvoice=off\n");
    CHECK(re15_stimme_option() == 0, "aus_text voice=off -> OFF");
    re15_stimme_option_aus_text("volume=50\nsound=stereo\n");
    CHECK(re15_stimme_option() == 1, "aus_text ohne Schluessel -> ON");
    env_setzen(RE15_STIMME_ENV, "0");
    re15_stimme_option_aus_text("voice=on\n");
    CHECK(re15_stimme_option() == 0, "Messhaken RE15_VOICE=0 uebersteuert voice=on");
    env_setzen(RE15_STIMME_ENV, "1");
    re15_stimme_option_aus_text("voice=off\n");
    CHECK(re15_stimme_option() == 1, "Messhaken RE15_VOICE=1 uebersteuert voice=off");
    env_setzen(RE15_STIMME_ENV, NULL);
}

/* ---- B Options-Datei ------------------------------------------------------------------------------------------- */
static void teil_b(void)
{
    const char *pfad = "stimmen_r41_opt.cfg";
    char txt[256];
    remove(pfad);
    env_setzen(RE15_LAUT_R41_ENV, pfad);
    env_setzen(RE15_U15_ENV, NULL);
    env_setzen(RE15_STIMME_ENV, NULL);

    re15_stimme_option_set(0);
    re15_lautstaerke_r41_laden();                  /* Datei fehlt -> Standard */
    CHECK(re15_stimme_option() == 1, "Laden ohne Datei: ON");
    CHECK(re15_lautstaerke_r41_speichern() == 0, "Speichern bei ON");
    datei_lesen(pfad, txt, sizeof txt);
    CHECK(!strcmp(txt, "volume=50\nsound=stereo\n"), "Datei bei ON = bisheriges Format, bitgleich: '%s'", txt);

    re15_stimme_option_set(0);
    CHECK(re15_lautstaerke_r41_speichern() == 0, "Speichern bei OFF");
    datei_lesen(pfad, txt, sizeof txt);
    CHECK(!strcmp(txt, "volume=50\nsound=stereo\nvoice=off\n"), "Datei bei OFF: '%s'", txt);
    re15_lautstaerke_r41_reset();
    CHECK(re15_stimme_option() == 1, "reset: ON");
    re15_lautstaerke_r41_laden();
    CHECK(re15_stimme_option() == 0, "Rundreise: Laden -> OFF");

    re15_ultimate15_set(1);
    CHECK(re15_lautstaerke_r41_speichern() == 0, "Speichern bei ULTIMATE an + VOICE OFF");
    datei_lesen(pfad, txt, sizeof txt);
    CHECK(!strcmp(txt, "volume=50\nsound=stereo\nultimate=on\nvoice=off\n"), "Datei mit beiden Port-Zeilen: '%s'", txt);
    re15_lautstaerke_r41_laden();
    CHECK(re15_stimme_option() == 0 && re15_ultimate15() == 1, "Rundreise: OFF und ULTIMATE an");

    re15_stimme_option_set(1);
    re15_ultimate15_set(0);
    CHECK(re15_lautstaerke_r41_speichern() == 0, "Speichern zurueck auf ON");
    datei_lesen(pfad, txt, sizeof txt);
    CHECK(!strcmp(txt, "volume=50\nsound=stereo\n"), "zurueck auf ON: Datei wieder bitgleich: '%s'", txt);

    datei_schreiben(pfad, "volume=70\nsound=mono\nultimate=on\n");   /* eine Datei der Runde ULTIMATE (ohne voice) */
    re15_stimme_option_set(0);
    re15_lautstaerke_r41_laden();
    CHECK(re15_stimme_option() == 1 && re15_lautstaerke_r41_prozent() == 70 && re15_ultimate15() == 1,
          "bisherige Datei ohne voice-Zeile: ON, Lautstaerke/ULTIMATE wie gespeichert");

    env_setzen(RE15_STIMME_ENV, "0");
    re15_lautstaerke_r41_laden();
    CHECK(re15_stimme_option() == 0, "Messhaken RE15_VOICE=0 beim Laden");
    env_setzen(RE15_STIMME_ENV, NULL);
    re15_ultimate15_set(0);
    re15_stimme_option_set(1);
    remove(pfad);
    env_setzen(RE15_LAUT_R41_ENV, NULL);
}

/* ---- C Stopp-Hook ---------------------------------------------------------------------------------------------- */
static int s_stopps = 0;
static void stopp_zaehlen(void) { s_stopps++; }

static void teil_c(void)
{
    re15_stimme_option_set(1);
    re15_stimme_option_stopp_hook(stopp_zaehlen);
    s_stopps = 0;
    re15_stimme_option_set(0);
    CHECK(s_stopps == 1, "ON -> OFF ruft den Stopp-Hook einmal (%d)", s_stopps);
    re15_stimme_option_set(0);
    CHECK(s_stopps == 1, "OFF -> OFF ruft ihn nicht erneut (%d)", s_stopps);
    re15_stimme_option_set(1);
    CHECK(s_stopps == 1, "OFF -> ON ruft ihn nicht (%d)", s_stopps);
    re15_stimme_option_aus_text("voice=off\n");
    CHECK(s_stopps == 2, "Laden einer OFF-Datei waehrend ON: Stopp-Hook (%d)", s_stopps);
    re15_stimme_option_stopp_hook(NULL);
    re15_stimme_option_set(1);
}

/* ---- D Was stumm wird ------------------------------------------------------------------------------------------ */
static void teil_d(void)
{
    static const struct { unsigned raum; int id; const char *was; } dialog[] = {
        { 0x1170, 0,  "Helipad-Intro main00" },
        { 0x1240, 0,  "Vorspann 1240 main00" },
        { 0x1100, 40, "Irons' Rekorder (Irons_Rekorder_Sound.wav)" },
        { 0x1011, 16, "1011 main16 (Dialogzeile neben dem Stoehnen)" },
        { 0x1141, 40, "1141 main40 (Dialogzeile neben Johns Ruf)" },
        { 0x1141, 52, "1141 Id 52 (vor help01)" },
        { 0x1141, 58, "1141 Id 58 (nach help05)" },
        { 0x5090, 40, "Annette main40 'W-William! You're alive!'" },
        { 0x5090, 42, "5090 main42 'Leon: Annette!'" },
        { 0x1041, 40, "Elza 1041 main40 'Oh, no, this is John!'" },
        { 0x1041, 42, "1041 Id 42 (Johns Ruf nur in 1141)" },
        { 0x5091, 41, "5091 Id 41 (Schrei nur in 5090)" },
        { 0x1010, 24, "1010 Id 24 (Stoehnen nur in 1011)" },
    };
    /* NUTZERWUNSCH-Auslegung 17.11.2026: Laute ohne Untertitel = Hinweise/Geraeusche, bleiben bei OFF hoerbar. */
    static const struct { unsigned raum; int id; const char *was; } laut[] = {
        { 0x1141, 53, "Hilferuf help01 1141" }, { 0x1141, 57, "Hilferuf help05 1141" },
        { 0x1041, 53, "Hilferuf help01 1041" }, { 0x1041, 57, "Hilferuf help05 1041" },
        { 0x1131, 54, "Hilferuf help02 1131" }, { 0x1131, 57, "Hilferuf help05 1131" },
        { 0x1141, 42, "Johns Ruf main42 (re15_audio_stimme)" },
        { 0x5090, 41, "Annettes Schrei main41 (re15_audio_stimme)" },
        { 0x1011, 24, "Stoehnen main16.5 (1011 Id 24)" },
    };
    re15_stimme_option_set(1);
    for (size_t i = 0; i < sizeof dialog / sizeof dialog[0]; i++)
        CHECK(re15_stimme_option_spielt(dialog[i].raum, dialog[i].id) == 1, "ON: %s spielt", dialog[i].was);
    for (size_t i = 0; i < sizeof laut / sizeof laut[0]; i++)
        CHECK(re15_stimme_option_spielt(laut[i].raum, laut[i].id) == 1, "ON: %s spielt", laut[i].was);
    CHECK(re15_stimme_option_spielt(0x1141, RE15_R39J_TUERTON_ID) == 1, "ON: Tuerton door_sound.wav spielt");
    re15_stimme_option_set(0);
    for (size_t i = 0; i < sizeof dialog / sizeof dialog[0]; i++)
        CHECK(re15_stimme_option_spielt(dialog[i].raum, dialog[i].id) == 0, "OFF: %s stumm", dialog[i].was);
    for (size_t i = 0; i < sizeof laut / sizeof laut[0]; i++)
        CHECK(re15_stimme_option_spielt(laut[i].raum, laut[i].id) == 1, "OFF: %s bleibt hoerbar (Laut ohne Untertitel)",
              laut[i].was);
    CHECK(re15_stimme_option_spielt(0x1141, RE15_R39J_TUERTON_ID) == 1,
          "OFF: Tuerton door_sound.wav (1141/%d) spielt weiter - Geraeusch", RE15_R39J_TUERTON_ID);
    CHECK(re15_stimme_option_spielt(0x1041, RE15_R39J_TUERTON_ID) == 0, "OFF: Id 59 in einem anderen Raum ist kein Tuerton");
    re15_stimme_option_set(1);
}

/* ---- Werkzeug fuer integration_stimmen_r41_szene -----------------------------------------------------------------
 * cap_diff <a.raw> <b.raw>: zwei RE15_AUDIO_CAP_SYNC-Mitschnitte (je Spielbild 44100/30 = 1470 Stereo-Frames int16 =
 * 5880 Byte) Bild fuer Bild vergleichen. Ausgabe: Laengen, erstes/letztes abweichendes Bild, Zahl abweichender Bilder,
 * Summe |a-b| und die Energie (Summe |x|) beider Mitschnitte. Exit 0. */
#define CAP_TICK_B (1470 * 2 * 2)
static unsigned char *roh_lesen(const char *pfad, long *n)
{
    FILE *f = fopen(pfad, "rb"); if (!f) { *n = 0; return NULL; }
    fseek(f, 0, SEEK_END); long g = ftell(f); fseek(f, 0, SEEK_SET);
    unsigned char *b = (unsigned char *)malloc((size_t)g + 1);
    *n = (long)fread(b, 1, (size_t)g, f); fclose(f);
    return b;
}
static int cap_diff(const char *pa, const char *pb)
{
    long na = 0, nb = 0;
    unsigned char *a = roh_lesen(pa, &na), *b = roh_lesen(pb, &nb);
    if (!a || !b) { printf("cap_diff: Datei fehlt (%s / %s)\n", pa, pb); return 2; }
    long ta = na / CAP_TICK_B, tb = nb / CAP_TICK_B, t = ta < tb ? ta : tb;
    long erste = -1, letzte = -1, anz = 0;
    double summe = 0, ea = 0, eb = 0;
    for (long k = 0; k < t; k++) {
        const short *sa = (const short *)(a + k * CAP_TICK_B), *sb = (const short *)(b + k * CAP_TICK_B);
        int anders = 0;
        for (int i = 0; i < CAP_TICK_B / 2; i++) {
            int d = sa[i] - sb[i];
            ea += sa[i] < 0 ? -sa[i] : sa[i];
            eb += sb[i] < 0 ? -sb[i] : sb[i];
            if (d) { anders = 1; summe += d < 0 ? -d : d; }
        }
        if (anders) { if (erste < 0) erste = k; letzte = k; anz++; }
    }
    printf("cap_diff: Bilder a=%ld b=%ld verglichen=%ld erste_abweichung=%ld letzte_abweichung=%ld abweichend=%ld "
           "summe_diff=%.0f energie_a=%.0f energie_b=%.0f\n", ta, tb, t, erste, letzte, anz, summe, ea, eb);
    free(a); free(b);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 4 && !strcmp(argv[1], "cap_diff")) return cap_diff(argv[2], argv[3]);
    printf("== A Schalter\n");       teil_a();
    printf("== B Options-Datei\n");  teil_b();
    printf("== C Stopp-Hook\n");     teil_c();
    printf("== D Was stumm wird\n"); teil_d();
    printf("unit_stimmen_r41: %d/%d ok\n", s_checks - s_fails, s_checks);
    return s_fails ? 1 : 0;
}
