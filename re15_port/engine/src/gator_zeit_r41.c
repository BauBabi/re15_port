/*
 * gator_zeit_r41.c - Runde 41 Spur G2 Nachbesserung 9 Punkt 2: MESSHAKEN Rechenzeit des Alligator-Takts (kein Spielverhalten).
 * Dossier analysis/befunde_runde41/G2_gatorfix.md "Nachbesserung 9".
 *
 * NB8 mass die Biegung mit clock() (Millisekunden-Raster, Wanduhr) - eine Spitze war damit nicht von einer Verdraengung des Fadens durch das
 * Betriebssystem zu trennen. Hier:
 *  - Wanduhr in Nanosekunden: Windows QueryPerformanceCounter, sonst clock_gettime(CLOCK_MONOTONIC);
 *  - Rechenzeit des Fadens: Windows QueryThreadCycleTime (Zyklen, nur wenn der Faden laeuft), sonst clock_gettime(CLOCK_THREAD_CPUTIME_ID) in ns.
 *    Die Zyklen rechnet der Auswerter mit dem im Lauf gemessenen Verhaeltnis TSC/Wanduhr in Mikrosekunden um (re15_g2_zeit_zyk_je_us).
 * Die Funktionen liegen in einer eigenen Datei, damit windows.h nur hier eingebunden wird.
 * env RE15_G2_ZEIT_LOG=1: je Boss-Takt eine Zeile in gator_zeit.log (Laufordner): Bild, Phase, Entwurf-F-Modus, Biegung Wand/Faden, ganzer Takt Wand/Faden.
 */
/* Linux (glibc, -std=c11 ohne Erweiterungen): clock_gettime, CLOCK_MONOTONIC, CLOCK_THREAD_CPUTIME_ID sind nur mit Feature-Makro deklariert - Muster
 * platform/pc/src/asset_root_pc.c:8-9 (Release-Abbruch v0.8.30 Schritt linux). Bau-Umgebung, kein Spielverhalten. */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE 1
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef RE15_PLATFORM_PC
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
int64_t re15_g2_zeit_ns(void)
{
    static LARGE_INTEGER f;
    LARGE_INTEGER c;
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (int64_t)(c.QuadPart / f.QuadPart) * 1000000000LL + (int64_t)((c.QuadPart % f.QuadPart) * 1000000000LL / f.QuadPart);
}
int64_t re15_g2_faden_zyk(void)
{
    ULONG64 z = 0;
    QueryThreadCycleTime(GetCurrentThread(), &z);
    return (int64_t)z;
}
/* Zyklen je Mikrosekunde: TSC-Zuwachs gegen die Wanduhr seit dem ersten Aufruf (QueryThreadCycleTime zaehlt im TSC-Takt) */
double re15_g2_zeit_zyk_je_us(void)
{
    static int64_t t0 = 0; static uint64_t z0 = 0;
    if (!t0) { t0 = re15_g2_zeit_ns(); z0 = __rdtsc(); return 0.0; }
    const int64_t dt = re15_g2_zeit_ns() - t0;
    return dt > 0 ? (double)(__rdtsc() - z0) * 1000.0 / (double)dt : 0.0;
}
#else
#include <time.h>
int64_t re15_g2_zeit_ns(void) { struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts); return (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec; }
int64_t re15_g2_faden_zyk(void) { struct timespec ts; clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts); return (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec; }
double re15_g2_zeit_zyk_je_us(void) { return 1000.0; }   /* Faden-Zeit in ns */
#endif

void re15_g2_zeit_takt(unsigned bild, int phase, int modus, int64_t takt_ns, int64_t takt_zyk, int64_t biege_ns, int64_t biege_zyk, const int64_t *teile)
{
    static int s_an = -1; static FILE *s_f = NULL; static unsigned s_n = 0;
    if (s_an < 0) { s_an = getenv("RE15_G2_ZEIT_LOG") != NULL; if (s_an) { s_f = fopen("gator_zeit.log", "w"); (void)re15_g2_zeit_zyk_je_us(); } }
    if (!s_an || !s_f) return;
    fprintf(s_f, "Z F%u ph=%d f=%d biege_ns=%lld biege_zyk=%lld takt_ns=%lld takt_zyk=%lld", bild, phase, modus, (long long)biege_ns, (long long)biege_zyk,
            (long long)takt_ns, (long long)takt_zyk);
    /* NB10: Abschnitte des Entwurf-F-Takts (re15_gr2_anlauf_zeiten): Huellen-Bau, Freikarten, Planung, Dijkstra, Bahnwahl, Ausfall, Zwischen-Bahn, Kletter-Bahnen */
    if (teile) fprintf(s_f, " teile=%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld", (long long)teile[0], (long long)teile[1], (long long)teile[2], (long long)teile[3],
                       (long long)teile[4], (long long)teile[5], (long long)teile[6], (long long)teile[7]);
    fputc('\n', s_f);
    if ((++s_n % 300u) == 0) { fprintf(s_f, "K zyk_je_us=%.3f\n", re15_g2_zeit_zyk_je_us()); fflush(s_f); }
}
#else
int64_t re15_g2_zeit_ns(void) { return 0; }
int64_t re15_g2_faden_zyk(void) { return 0; }
double re15_g2_zeit_zyk_je_us(void) { return 0.0; }
void re15_g2_zeit_takt(unsigned bild, int phase, int modus, int64_t takt_ns, int64_t takt_zyk, int64_t biege_ns, int64_t biege_zyk, const int64_t *teile)
{ (void)teile; (void)bild; (void)phase; (void)modus; (void)takt_ns; (void)takt_zyk; (void)biege_ns; (void)biege_zyk; }
#endif
