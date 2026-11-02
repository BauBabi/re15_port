/*
 * RE1.5 Rebuilt — Runde 41 Spur Q (UEBERGABE 3b, Dossier analysis/befunde_runde41/Q_restpunkte.md Punkt 3): ein Absturz der exe steht im debug.log.
 * Auf dieser Maschine hinterlaesst er keinen WER-Eintrag (auch Abnahme P H1), der Ketten-Riegel endete deshalb "still". Nur Log, die Ausnahme laeuft weiter
 * (EXCEPTION_CONTINUE_SEARCH); kein Spielverhalten. Offsets relativ zur Modulbasis: `gdb -batch -ex "info symbol 0x140000000+<offset>" <exe>`.
 */
#if defined(_WIN32) && !defined(__ANDROID__)
#include <stdio.h>
#include <windows.h>

static LONG WINAPI ausnahme(EXCEPTION_POINTERS *e)
{
    void *f[24];
    const USHORT n = RtlCaptureStackBackTrace(0, 24, f, NULL);
    const char *b = (const char *)GetModuleHandleA(NULL);
    fprintf(stderr, "[flow] AUSNAHME 0x%08lX Modul %p Adresse +0x%llx Ziel %p Stapel", e->ExceptionRecord->ExceptionCode, (void *)b,
            (unsigned long long)((const char *)e->ExceptionRecord->ExceptionAddress - b), (void *)e->ExceptionRecord->ExceptionInformation[1]);
    for (USHORT i = 0; i < n; i++) fprintf(stderr, " +0x%llx", (unsigned long long)((const char *)f[i] - b));
    fprintf(stderr, "\n");
    fflush(stderr);
    return EXCEPTION_CONTINUE_SEARCH;
}

void re15_endgrund_r41_einrichten(void) { SetUnhandledExceptionFilter(ausnahme); }
#else
typedef int re15_endgrund_r41_leer_t;   /* ISO C: keine leere Uebersetzungseinheit */
#endif
