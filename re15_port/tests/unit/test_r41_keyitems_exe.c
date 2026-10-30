/*
 * Runde 41 Spur O "keyitems": Auswerter des Integrationstests tests/integration/test_r41_keyitems.cmake.
 *   kurbel <state.txt>   letzter Lauf-Abschnitt von RE15_STATE_LOG: das erste Bild an der Kurbel (x = 3650) ist schon das
 *                        Kurbeln (Kommando 4, Clip 0, Gier 1024, kein Text), ab dort jedes Bild an der Kurbel bis Plc_ret,
 *                        Mindestlaenge der Szene.
 * Keine Engine-Abhaengigkeit (reiner Text-Auswerter), Linux-fest.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int f, x, z, rot, pst, mo, msg; } zeile_t;
#define MAX_Z 200000
static zeile_t *s_z;
static int s_n;

static int lies(const char *pfad)
{
    FILE *f = fopen(pfad, "r");
    if (!f) { printf("FEHLER: %s fehlt\n", pfad); return -1; }
    static char b[1024];
    s_n = 0;
    while (fgets(b, sizeof b, f)) {
        zeile_t z; int hp, a = 0;
        const char *pl = strstr(b, "PL("), *pst = strstr(b, " pst="), *mo = strstr(b, " mo="), *m = strstr(b, "msg(a=");
        if (b[0] != 'F' || !pl || !pst || !mo) continue;
        z.f = atoi(b + 1);
        if (sscanf(pl, "PL(%d,%d,rot=%d,hp=%d)", &z.x, &z.z, &z.rot, &hp) != 4) continue;
        z.pst = atoi(pst + 5); z.mo = atoi(mo + 4);
        if (m) a = atoi(m + 6);
        z.msg = a;
        if (z.f == 0) s_n = 0;          /* neuer Raum: der letzte Abschnitt zaehlt */
        if (s_n < MAX_Z) s_z[s_n++] = z;
    }
    fclose(f);
    return s_n > 0 ? 0 : -1;
}

static int kurbel(const char *pfad)
{
    if (lies(pfad) != 0) return 1;
    int i0 = -1, fehler = 0;
    for (int i = 0; i < s_n; i++) if (s_z[i].x == 3650) { i0 = i; break; }
    if (i0 <= 0) { printf("FAIL: nie an der Kurbel (%d Zeilen)\n", s_n); return 1; }
    const zeile_t *v = &s_z[i0 - 1], *e = &s_z[i0];
    printf("vorher F%d (%d,%d) Gier %d, erstes Kurbelbild F%d (%d,%d) Gier %d pst=%d mo=%d\n",
           v->f, v->x, v->z, v->rot, e->f, e->x, e->z, e->rot, e->pst, e->mo);
    if (e->pst != 4 || e->mo != 0) { printf("FAIL: erstes Bild an der Kurbel ist nicht das Kurbeln\n"); fehler++; }
    if (e->rot != 1024) { printf("FAIL: Gier\n"); fehler++; }
    if (e->msg) { printf("FAIL: Text offen\n"); fehler++; }
    int ende = -1;
    for (int i = i0; i < s_n; i++) {
        if (s_z[i].pst != 4) { ende = i; break; }
        if (s_z[i].x != 3650 || s_z[i].z != e->z) { printf("FAIL: F%d Lage (%d,%d)\n", s_z[i].f, s_z[i].x, s_z[i].z); fehler++; }
    }
    const int lang = (ende < 0 ? s_n : ende) - i0;
    printf("Szene %d Bilder bis Plc_ret\n", lang);
    if (lang < 120) { printf("FAIL: Szene zu kurz\n"); fehler++; }
    return fehler ? 1 : 0;
}

int main(int argc, char **argv)
{
    s_z = (zeile_t *)malloc(sizeof(zeile_t) * MAX_Z);
    if (!s_z) return 2;
    if (argc >= 3 && !strcmp(argv[1], "kurbel")) return kurbel(argv[2]);
    printf("Aufruf: kurbel <state.txt>\n");
    return 2;
}
