/*
 * re15_psx_compat.c — Implementierung der Hosted-C-Ersatzfunktionen (nur PSX).
 * Begruendung und Semantik: siehe re15_psx_compat.h.
 */
#include "re15_psx_compat.h"

#include <stdio.h>    /* PSn00bSDK: printf / vsnprintf */
#include <stddef.h>

/* Zwei eindeutige, nie dereferenzierte Adressen als Stream-Handles. */
static struct re15_psx_FILE { int dummy; } s_out, s_err;
FILE *const re15_psx_stdout = &s_out;
FILE *const re15_psx_stderr = &s_err;

FILE *re15_psx_fopen(const char *path, const char *mode)
{
    (void)path; (void)mode;
    return NULL;             /* kein Dateisystem -> Aufrufer nimmt Fehlerzweig */
}

int re15_psx_fclose(FILE *f) { (void)f; return 0; }
int re15_psx_fflush(FILE *f) { (void)f; return 0; }

unsigned int re15_psx_fread(void *p, unsigned int sz, unsigned int n, FILE *f)
{
    (void)p; (void)sz; (void)n; (void)f;
    return 0;
}

unsigned int re15_psx_fwrite(const void *p, unsigned int sz, unsigned int n, FILE *f)
{
    (void)p; (void)sz; (void)n; (void)f;
    return 0;
}

/* Diagnosen sollen NICHT verschwinden: Stream verwerfen, Rest auf die TTY.
 * vsnprintf in einen festen Puffer, dann ein einzelner printf — so bleibt die
 * Ausgabe atomar und der Aufwand klein. */
int re15_psx_fprintf(FILE *f, const char *fmt, ...)
{
    char buf[256];
    va_list ap;
    (void)f;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    /* %s statt buf direkt: der Inhalt kann selbst Prozentzeichen enthalten. */
    printf("%s", buf);
    return 0;
}

char *re15_psx_getenv(const char *name)
{
    (void)name;
    return NULL;             /* keine Umgebung -> alle Debug-Schalter aus */
}
