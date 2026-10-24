/* test_r41_kleinkram_a.c — Runde 41 Spur D "kleinkram_a": Riegel + Werkzeug der Echtlauf-Riegel.
 * Dossier analysis/befunde_runde41/D_kleinkram_a.md.
 *
 * Ohne Argument (unit_r41_kleinkram_a):
 *   A  Lautstaerke: Datei-Parser/Formatierer, Q15-Faktor (100 -> 0x8000, 50 -> 0x4000, 0 -> 0), Schritt/Klemme,
 *      Arbeitskopie-Modell (Eintritt @0x8002f244-4c / Bestaetigen @0x8002f2c0 / Cross @0x8002f27c) mit echter Datei
 *      ueber RE15_OPTIONS_FILE, Rundreise speichern -> laden.
 *   B  Blink: re15_blink_r41_wert(n) == Titelpuls FUN_801028ec nach n Schritten (bytegleich mit title_pulse.c),
 *      Tabelle setzen/aktiv/faktor, Lichtkontext x Wert/0x80 (Spitze 0xBE, Klemme 255), inaktiv unveraendert.
 *   C  Giftkugel: RE2-Gehirn + Spinne als Absender -> 20 HP (@0x800200dc); HP 15 (Danger) -> -5, Zustand 3 (Tod),
 *      re15_player_is_dead(); HP 30 -> 10, Zustand 2, +0x93 Bit 0; Tore (hit-once, HP < 0, kein Spinnen-Absender,
 *      RE1.5-Gehirn -> nicht zustaendig).
 * Werkzeug:
 *   rms <raw>                                  RMS (int16 stereo) ausgeben
 *   rms_vergleich <a.raw> <b.raw> <soll_promille> <toleranz_promille>   RMS(b)/RMS(a)*1000 pruefen
 *   ppm_puls <x0> <x1> <y0> <y1> <min_delta> <max_delta> <datei.ppm>...  Mittelhelligkeit je Bild; max-min in [min,max]
 */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_lautstaerke_r41.h"
#include "re15_blink_r41.h"
#include "re15_giftkugel_r41.h"
#include "re15_title_pulse.h"
#include "re15_engine.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"

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

/* ---- A Lautstaerke ------------------------------------------------------------------------------- */
static void teil_a(void)
{
    printf("\n== A Lautstaerke\n");
    int p = -1, st = -1;
    int n = re15_lautstaerke_r41_parse("volume=70\nsound=mono\n", &p, &st);
    CHECK(n == 2 && p == 70 && st == 0, "Parser: 2 Schluessel, volume 70, mono (n=%d p=%d st=%d)", n, p, st);
    p = -1; st = -1;
    n = re15_lautstaerke_r41_parse("  sound=stereo\r\nvolume=250\r\n", &p, &st);
    CHECK(n == 2 && p == 100 && st == 1, "Parser CRLF + Klemme: volume 250 -> 100, stereo (n=%d p=%d st=%d)", n, p, st);
    n = re15_lautstaerke_r41_parse("unsinn=3\n", &p, &st);
    CHECK(n == 0, "Parser: unbekannter Schluessel zaehlt nicht (n=%d)", n);
    char buf[64];
    int w = re15_lautstaerke_r41_format(buf, sizeof buf, 40, 0);
    CHECK(w > 0 && !strcmp(buf, "volume=40\nsound=mono\n"), "Formatierer: '%s'", buf);

    re15_lautstaerke_r41_reset();
    CHECK(re15_lautstaerke_r41_prozent() == 100 && re15_lautstaerke_r41_q15() == 0x8000,
          "Voreinstellung 100 %% -> Q15 0x8000 (unveraendert)");
    re15_lautstaerke_r41_setzen(50);
    CHECK(re15_lautstaerke_r41_q15() == 0x4000, "50 %% -> Q15 0x4000 (halbe Amplitude): 0x%04X", re15_lautstaerke_r41_q15());
    re15_lautstaerke_r41_setzen(0);
    CHECK(re15_lautstaerke_r41_q15() == 0, "0 %% -> Q15 0");
    re15_lautstaerke_r41_setzen(100);
    re15_lautstaerke_r41_schritt(+1);
    CHECK(re15_lautstaerke_r41_prozent() == 100, "RIGHT bei 100 bleibt 100 (Klemme)");
    re15_lautstaerke_r41_schritt(-1);
    CHECK(re15_lautstaerke_r41_prozent() == 90, "LEFT: 100 -> 90 (Schritt %d)", RE15_LAUT_R41_SCHRITT);
    for (int i = 0; i < 20; i++) re15_lautstaerke_r41_schritt(-1);
    CHECK(re15_lautstaerke_r41_prozent() == 0, "20 x LEFT -> 0 (Klemme)");

    /* Arbeitskopie-Modell mit echter Datei. */
    char pfad[512];
    snprintf(pfad, sizeof pfad, "r41d_optionen_%ld.cfg", (long)rand());
    env_setzen(RE15_LAUT_R41_ENV, pfad);
    remove(pfad);
    re15_lautstaerke_r41_laden();
    CHECK(re15_lautstaerke_r41_prozent() == 100 && re15_lautstaerke_r41_stereo() == 1,
          "Laden ohne Datei: 100 %%, Stereo");
    re15_lautstaerke_r41_schritt(-1); re15_lautstaerke_r41_schritt(-1);          /* 80, nur Arbeitskopie */
    CHECK(re15_lautstaerke_r41_prozent() == 80 && re15_lautstaerke_r41_gespeichert() == 100,
          "LEFT LEFT: Arbeitskopie 80, gespeichert 100");
    re15_lautstaerke_r41_verwerfen();                                              /* Cross @0x8002f27c */
    CHECK(re15_lautstaerke_r41_prozent() == 100, "Cross: Arbeitskopie zurueck auf 100");
    re15_lautstaerke_r41_schritt(-1); re15_lautstaerke_r41_schritt(-1); re15_lautstaerke_r41_schritt(-1);
    int r = re15_lautstaerke_r41_bestaetigen(0);                                   /* Kreis @0x8002f2c0, Mono */
    CHECK(r == 0 && re15_lautstaerke_r41_gespeichert() == 70 && re15_lautstaerke_r41_stereo() == 0,
          "Bestaetigen: gespeichert 70 %%, Mono, Datei geschrieben (r=%d)", r);
    re15_lautstaerke_r41_setzen(10);
    int st2 = re15_lautstaerke_r41_eintritt();                                     /* @0x8002f244-4c */
    CHECK(re15_lautstaerke_r41_prozent() == 70 && st2 == 0, "Eintritt: Arbeitskopie := gespeichert (70), Stereo-Stand 0");
    re15_lautstaerke_r41_reset();
    re15_lautstaerke_r41_laden();
    CHECK(re15_lautstaerke_r41_prozent() == 70 && re15_lautstaerke_r41_stereo() == 0 &&
          re15_lautstaerke_r41_q15() == 70 * 0x8000 / 100, "Rundreise Datei: 70 %%, Mono, Q15 %d", re15_lautstaerke_r41_q15());
    remove(pfad);
    env_setzen(RE15_LAUT_R41_ENV, "");
    re15_lautstaerke_r41_reset();
}

/* ---- B Blink --------------------------------------------------------------------------------------- */
static void teil_b(void)
{
    printf("\n== B Blink (Helligkeitspuls FUN_801028ec)\n");
    int gleich = 1, maxw = 0, minw = 999;
    re15_title_pulse_reset();
    for (unsigned n = 0; n <= 130; n++) {
        int w = re15_blink_r41_wert(n);
        if (w != re15_title_pulse_value()) { gleich = 0; printf("   n=%u blink %d titel %d\n", n, w, re15_title_pulse_value()); }
        if (w > maxw) maxw = w;
        if (w < minw) minw = w;
        re15_title_pulse_step();
    }
    CHECK(gleich, "wert(n) == Titelpuls nach n Schritten fuer n = 0..130 (bytegleich title_pulse.c)");
    CHECK(minw == 0x80 && maxw == 0xBE, "Spanne 0x80..0xBE (min 0x%02X max 0x%02X)", minw, maxw);
    CHECK(re15_blink_r41_wert(31) == 0xBE && re15_blink_r41_wert(60) == 0x80 && re15_blink_r41_wert(91) == 0xBE,
          "Spitze bei Takt 31, Reset bei 60 (Periode 0x3c), wieder Spitze bei 91");

    re15_blink_r41_reset();
    CHECK(!re15_blink_r41_aktiv(0x1030, 5), "Tabelle leer: 1030/5 inaktiv");
    CHECK(re15_blink_r41_setzen(0x1030, 5, 1) == 0 && re15_blink_r41_aktiv(0x1030, 5), "setzen 1030/5 an");
    CHECK(!re15_blink_r41_aktiv(0x1031, 5) && !re15_blink_r41_aktiv(0x1030, 4), "andere Raum-/Objekt-Ids bleiben inaktiv");
    CHECK(re15_blink_r41_faktor_q8(0x1030, 5, 0) == 256 && re15_blink_r41_faktor_q8(0x1030, 5, 31) == 0xBE * 2,
          "Faktor Q8: Takt 0 -> 256, Takt 31 -> %d (= 0xBE*256/0x80)", re15_blink_r41_faktor_q8(0x1030, 5, 31));
    re15_actor_lightctx_t ctx; memset(&ctx, 0, sizeof ctx);
    ctx.ambient[0] = 0x80; ctx.ambient[1] = 0xF0; ctx.ambient[2] = 0x10;
    ctx.C[0][0] = 1000; ctx.C[1][1] = -400; ctx.C[2][2] = 64;
    int w = re15_blink_r41_lichtctx(0x1030, 5, 31, &ctx);
    CHECK(w == 0xBE && ctx.ambient[0] == 0xBE && ctx.ambient[1] == 255 && ctx.ambient[2] == 0x10 * 0xBE / 0x80,
          "Lichtkontext Takt 31: ambient 0x80 -> 0xBE, 0xF0 -> 255 (Klemme), 0x10 -> %d", ctx.ambient[2]);
    CHECK(ctx.C[0][0] == 1000 * 0xBE / 0x80 && ctx.C[1][1] == -400 * 0xBE / 0x80 && ctx.C[2][2] == 64 * 0xBE / 0x80,
          "Lichtfarben linear skaliert (%d, %d, %d)", (int)ctx.C[0][0], (int)ctx.C[1][1], (int)ctx.C[2][2]);
    memset(&ctx, 0, sizeof ctx); ctx.ambient[0] = 0x80; ctx.C[0][0] = 1000;
    w = re15_blink_r41_lichtctx(0x1030, 5, 0, &ctx);
    CHECK(w == 0x80 && ctx.ambient[0] == 0x80 && ctx.C[0][0] == 1000, "Takt 0 (Wert 0x80): unveraendert");
    re15_blink_r41_setzen(0x1030, 5, 0);
    ctx.ambient[0] = 0x80;
    w = re15_blink_r41_lichtctx(0x1030, 5, 31, &ctx);
    CHECK(w == 0x80 && ctx.ambient[0] == 0x80, "aus (an = 0): unveraendert");
    re15_blink_r41_reset();
}

/* ---- C Giftkugel ----------------------------------------------------------------------------------- */
static void spieler_und_spinne(int hp, re15_ai_flavor_t fl, unsigned absender_typ)
{
    memset(g_actors, 0, sizeof(re15_actor_t) * RE15_ACTOR_MAX);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->hp = (int16_t)hp; pl->state = 1; pl->x = 1000; pl->y = 0; pl->z = 2000; pl->rot_y = 0;
    re15_actor_t *sp = &g_actors[1];
    sp->active = 1; sp->type = (uint8_t)absender_typ; sp->hp = 100; sp->x = 1000; sp->z = 4000;
    re15_ai_flavor_set(fl);
    re15_giftkugel_r41_reset();
}

static void teil_c(void)
{
    printf("\n== C Giftkugel (RE2 Op 32: 20 HP @0x800200dc, Kommando 2/3 @0x800477b8/@0x8004780c)\n");
    re15_attack_box_t box; memset(&box, 0, sizeof box);
    box.x = 1000; box.y = 0; box.z = 2000; box.radius = 500;        /* Routine 25: `ori a0,zero,0x1f4` @0x80017ffc */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    spieler_und_spinne(15, RE15_AI_FLAVOR_RE2, 0x25);
    int r = re15_giftkugel_r41_treffer(pl, &box, 1);
    CHECK(r == 2 && pl->hp == -5 && pl->state == 3 && (pl->re2z_self1d3 & 0x80u) && re15_player_is_dead(),
          "Danger HP 15 + Treffer -> HP %d, Zustand %d (= 3 Tod, FUN_800401d4 Todeszweig), is_dead %d, r=%d",
          (int)pl->hp, (int)pl->state, re15_player_is_dead(), r);
    CHECK(re15_giftkugel_r41_tode() == 1 && re15_giftkugel_r41_treffer_zaehler() == 1, "Zaehler: 1 Treffer, 1 Tod");
    r = re15_giftkugel_r41_treffer(pl, &box, 1);
    CHECK(r == 0 && pl->hp == -5, "toter Spieler (HP < 0 @0x800476fc-704): kein zweiter Abzug (r=%d, HP %d)", r, (int)pl->hp);

    spieler_und_spinne(19, RE15_AI_FLAVOR_RE2, 0x25);
    r = re15_giftkugel_r41_treffer(pl, &box, 1);
    CHECK(r == 2 && pl->hp == -1 && pl->state == 3, "Danger-Obergrenze HP 19 -> -1 = Tod (r=%d)", r);

    spieler_und_spinne(30, RE15_AI_FLAVOR_RE2, 0x25);
    r = re15_giftkugel_r41_treffer(pl, &box, 1);
    CHECK(r == 1 && pl->hp == 10 && pl->state == 2 && (pl->hit_react & 1u) && !re15_player_is_dead(),
          "HP 30 -> %d, Zustand %d (= 2 Treffer), +0x93 Bit 0 gesetzt, lebt (r=%d)", (int)pl->hp, (int)pl->state, r);
    r = re15_giftkugel_r41_treffer(pl, &box, 1);
    CHECK(r == 0 && pl->hp == 10, "hit-once +0x93 (@0x80012e24-30): kein zweiter Abzug im Schutzfenster");
    pl->hit_react = 0;
    r = re15_giftkugel_r41_treffer(pl, &box, 1);
    CHECK(r == 2 && pl->hp == -10 && pl->state == 3, "Schutz vorbei, HP 10 (Danger) + Treffer -> -10 = Tod (r=%d)", r);

    spieler_und_spinne(20, RE15_AI_FLAVOR_RE2, 0x25);
    r = re15_giftkugel_r41_treffer(pl, &box, 1);
    CHECK(r == 1 && pl->hp == 0 && pl->state == 2, "HP 20 (Caution, nicht Danger) -> 0, lebt (Danger-Schwelle 20 @0x800112b5)");

    spieler_und_spinne(15, RE15_AI_FLAVOR_RE15, 0x25);
    r = re15_giftkugel_r41_treffer(pl, &box, 1);
    CHECK(r == 0 && pl->hp == 15, "RE1.5-Gehirn: nicht zustaendig (Beta-Weg Resolver Art 0 = 10 bleibt), r=%d", r);

    spieler_und_spinne(15, RE15_AI_FLAVOR_RE2, 0x27);
    r = re15_giftkugel_r41_treffer(pl, &box, 1);
    CHECK(r == 0 && pl->hp == 15, "Absender keine Spinne (Typ 0x27): nicht zustaendig");

    spieler_und_spinne(15, RE15_AI_FLAVOR_RE2, 0x25);
    box.x = 5000;
    r = re15_giftkugel_r41_treffer(pl, &box, 1);
    CHECK(r == 0 && pl->hp == 15, "Kasten trifft nicht: kein Abzug");
    box.x = 1000;
    r = re15_giftkugel_r41_treffer(pl, &box, 0);
    CHECK(r == 0 && pl->hp == 15, "Absender Slot 0 (kein Gegner): nicht zustaendig");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
}

/* ---- Werkzeuge ------------------------------------------------------------------------------------- */
static double rms_datei(const char *pfad, long *out_samples)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) { fprintf(stderr, "rms: %s nicht lesbar\n", pfad); return -1.0; }
    double acc = 0.0; long n = 0;
    int16_t buf[4096];
    size_t got;
    while ((got = fread(buf, sizeof(int16_t), 4096, f)) > 0) {
        for (size_t i = 0; i < got; i++) { double v = buf[i]; acc += v * v; }
        n += (long)got;
    }
    fclose(f);
    if (out_samples) *out_samples = n;
    return n > 0 ? sqrt(acc / (double)n) : 0.0;
}

static int ppm_mittel(const char *pfad, int x0, int x1, int y0, int y1, double *out)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) { fprintf(stderr, "ppm: %s nicht lesbar\n", pfad); return -1; }
    char magic[3] = {0}; int w = 0, h = 0, maxv = 0;
    if (fscanf(f, "%2s %d %d %d", magic, &w, &h, &maxv) != 4 || strcmp(magic, "P6") != 0) { fclose(f); return -1; }
    fgetc(f);
    unsigned char *px = (unsigned char *)malloc((size_t)w * (size_t)h * 3);
    if (!px) { fclose(f); return -1; }
    size_t got = fread(px, 1, (size_t)w * (size_t)h * 3, f);
    fclose(f);
    if (got != (size_t)w * (size_t)h * 3) { free(px); return -1; }
    double acc = 0.0; long n = 0;
    for (int y = y0; y <= y1; y++)
        for (int x = x0; x <= x1; x++) {
            if (x < 0 || y < 0 || x >= w || y >= h) continue;
            const unsigned char *p = px + ((size_t)y * (size_t)w + (size_t)x) * 3;
            acc += (p[0] + p[1] + p[2]) / 3.0; n++;
        }
    free(px);
    if (n == 0) return -1;
    *out = acc / (double)n;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc >= 3 && !strcmp(argv[1], "rms")) {
        long n = 0; double r = rms_datei(argv[2], &n);
        printf("rms=%.3f samples=%ld\n", r, n);
        return r >= 0.0 ? 0 : 1;
    }
    if (argc >= 6 && !strcmp(argv[1], "rms_vergleich")) {
        long na = 0, nb = 0;
        double a = rms_datei(argv[2], &na), b = rms_datei(argv[3], &nb);
        long soll = atol(argv[4]), tol = atol(argv[5]);
        if (a <= 0.0 || b < 0.0) { printf("rms_vergleich: a=%.3f b=%.3f (kein Ton?)\n", a, b); return 1; }
        long promille = (long)(b / a * 1000.0 + 0.5);
        printf("rms_vergleich: a=%.3f (%ld Samples) b=%.3f (%ld Samples) -> %ld Promille (soll %ld +- %ld)\n",
               a, na, b, nb, promille, soll, tol);
        return (labs(promille - soll) <= tol && na == nb) ? 0 : 1;
    }
    if (argc >= 9 && !strcmp(argv[1], "ppm_puls")) {
        int x0 = atoi(argv[2]), x1 = atoi(argv[3]), y0 = atoi(argv[4]), y1 = atoi(argv[5]);
        double min_delta = atof(argv[6]), max_delta = atof(argv[7]);
        double mn = 1e9, mx = -1e9; int k = 0;
        for (int i = 8; i < argc; i++) {
            double m = 0.0;
            if (ppm_mittel(argv[i], x0, x1, y0, y1, &m) != 0) { printf("ppm_puls: %s unlesbar\n", argv[i]); return 1; }
            printf("  %s: Mittel %.2f\n", argv[i], m);
            if (m < mn) mn = m;
            if (m > mx) mx = m;
            k++;
        }
        double d = mx - mn;
        printf("ppm_puls: %d Bilder, Mittel min %.2f max %.2f, Hub %.2f (soll %.2f..%.2f)\n", k, mn, mx, d, min_delta, max_delta);
        return (k > 0 && d >= min_delta && d <= max_delta) ? 0 : 1;
    }

    srand(12345);
    teil_a();
    teil_b();
    teil_c();
    printf("\n%s (%d Pruefungen, %d FAIL)\n", s_fails ? "FAIL" : "ALLE PRUEFUNGEN GRUEN", s_checks, s_fails);
    return s_fails ? 1 : 0;
}
