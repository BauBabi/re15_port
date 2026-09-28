/* probe_r30_sicherung_modell.c — RIEGEL fuer das Welt-Modell der Sicherung (Runde 30, Thema H).
 *
 * Dossier: analysis/befunde_runde30/sicherung.md §2.6, §3.3, §3.4, §5.2.
 *
 * WAS ER FESTHAELT
 *   1. Die eingebackenen MD1-Bytes sind GENAU die berichtigte Fassung
 *      (soll/sicherung_zord_normal.md1, sha256 39badaa31dbc2f4c...): Laenge 2532 und
 *      FNV-1a-64 0xA81647DFFC9D76B5. Liegt die Soll-Datei vor, wird zusaetzlich Byte fuer
 *      Byte verglichen.
 *   2. Jedes Viereck steht in Z-ORDNUNG: die Strecken Ecke0-Ecke3 und Ecke1-Ecke2 sind die
 *      DIAGONALEN (sie schneiden sich), Ecke0-Ecke2 und Ecke1-Ecke3 nicht. Original:
 *      FUN_800256b0 reicht die vier MD1-Ecken unveraendert in die vier Ecken des
 *      GPU-Primitivs 0x3C (POLY_GT4: gte_stsxy3_gt3 = x0y0,x1y1,x2y2; gte_stsxy(prim+0x2c)
 *      = x3y3), die GPU teilt in (1,2,3)+(2,3,4).
 *   3. Die Teilung des Ports (0,1,3)+(0,3,2) deckt jedes Viereck GANZ (abgetastet,
 *      Bezug konvexe Huelle). Vor dem Bau: Deckung 0,748 — ein Viertel jeder Flaeche offen.
 *   4. Die Normalen zeigen nach AUSSEN: je Eck-Normale eines Vierecks ist
 *      Normale . (0,y,z) > 0 (Laengsachse = X). Vor dem Bau: 160 von 160 nach innen.
 *      Normalenlaenge 4096 +-1 (Q12).
 *
 * Rueckgabe 0 = alles bestanden.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_md1.h"
#include "re15_sicherung.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fehler = 0;

static void pruefe(const char *was, int ok)
{
    printf("   [%s] %s\n", ok ? "OK  " : "FEHL", was);
    if (!ok) fehler++;
}

static uint64_t fnv1a64(const uint8_t *b, int n)
{
    uint64_t h = 0xcbf29ce484222325ull;
    for (int i = 0; i < n; i++) { h ^= b[i]; h *= 0x100000001b3ull; }
    return h;
}

typedef struct { double x, y; } p2_t;

static double kreuz(p2_t a, p2_t b, p2_t c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

/* echter Schnitt der Strecken ab und cd (im Inneren beider) */
static int schneiden(p2_t a, p2_t b, p2_t c, p2_t d)
{
    return kreuz(a, b, c) * kreuz(a, b, d) < 0 && kreuz(c, d, a) * kreuz(c, d, b) < 0;
}

/* liegt (x,y) im Dreieck abc? (Rand zaehlt mit) */
static int im_dreieck(p2_t a, p2_t b, p2_t c, double x, double y)
{
    double d = (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
    if (d > -1e-9 && d < 1e-9) return 0;
    double w0 = ((b.x - x) * (c.y - y) - (c.x - x) * (b.y - y)) / d;
    double w1 = ((c.x - x) * (a.y - y) - (a.x - x) * (c.y - y)) / d;
    return w0 >= 0 && w1 >= 0 && (1.0 - w0 - w1) >= 0;
}

/* Anteil der Vierecksflaeche, den die Port-Teilung (0,1,3)+(0,3,2) WIRKLICH bedeckt —
 * abgetastet, dieselbe Messung wie sicherung_werkzeug/md1_zordnung.py. Bezug ist die
 * konvexe Huelle = Vereinigung aller vier Eck-Dreiecke. Eine Flaechen-SUMME taugt hier
 * nicht: bei einem Umlauf-Viereck ueberlappen die beiden Dreiecke, die Summe stimmt
 * trotzdem, und ein Viertel der Flaeche bleibt offen. */
static double deckung(const p2_t Q[4])
{
    double lox = Q[0].x, hix = Q[0].x, loy = Q[0].y, hiy = Q[0].y;
    for (int k = 1; k < 4; k++) {
        if (Q[k].x < lox) lox = Q[k].x;
        if (Q[k].x > hix) hix = Q[k].x;
        if (Q[k].y < loy) loy = Q[k].y;
        if (Q[k].y > hiy) hiy = Q[k].y;
    }
    const int N = 120;
    long port = 0, huelle = 0;
    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++) {
            double x = lox + (hix - lox) * (double)i / (double)(N - 1);
            double y = loy + (hiy - loy) * (double)j / (double)(N - 1);
            int h = im_dreieck(Q[0], Q[1], Q[2], x, y) || im_dreieck(Q[0], Q[1], Q[3], x, y) ||
                    im_dreieck(Q[0], Q[2], Q[3], x, y) || im_dreieck(Q[1], Q[2], Q[3], x, y);
            int pt = im_dreieck(Q[0], Q[1], Q[3], x, y) || im_dreieck(Q[0], Q[3], Q[2], x, y);
            huelle += h;
            port += (h && pt);
        }
    return huelle ? (double)port / (double)huelle : 0.0;
}

int main(void)
{
    int n = 0;
    const uint8_t *d = re15_sicherung_md1_bytes(&n);

    printf("== 1. Bytes ==\n");
    printf("   MD1 %d B, FNV-1a-64 0x%016llX\n", n, (unsigned long long)fnv1a64(d, n));
    pruefe("Laenge 2532", n == 2532);
    pruefe("FNV-1a-64 = 0xA81647DFFC9D76B5 (= soll/sicherung_zord_normal.md1)",
           fnv1a64(d, n) == 0xA81647DFFC9D76B5ull);
#ifdef RE15_R30_SOLL_DIR
    {
        char p[700];
        snprintf(p, sizeof p, "%s/sicherung_zord_normal.md1", RE15_XSTR(RE15_R30_SOLL_DIR));
        FILE *f = fopen(p, "rb");
        if (f) {
            static uint8_t soll[4096];
            size_t m = fread(soll, 1, sizeof soll, f);
            fclose(f);
            pruefe("bytegleich mit der Soll-Datei",
                   (int)m == n && memcmp(soll, d, (size_t)n) == 0);
        } else {
            printf("   [----] Soll-Datei nicht vorhanden (%s) - der FNV-Riegel gilt\n", p);
        }
    }
#endif

    static re15_md1_t md1;
    if (re15_md1_parse(d, n, &md1) != 0 || md1.mesh_count < 1) {
        printf("FEHL: MD1 nicht lesbar\n");
        return 1;
    }
    const re15_md1_mesh_t *m = &md1.meshes[0];
    printf("\n== 2./3. Eckenfolge der %d Vierecke ==\n", (int)m->quad_count);
    pruefe("40 Vierecke, 16 Dreiecke, 50 Punkte",
           m->quad_count == 40 && m->triangle_count == 16 && m->quad_vertex_count == 50);

    int z_ord = 0, umlauf = 0, entartet = 0, voll = 0;
    double deck_min = 9.0, deck_sum = 0;
    int aussen = 0, innen = 0, quer = 0, laenge_ok = 1;
    for (int qi = 0; qi < (int)m->quad_count; qi++) {
        const re15_md1_quad_t *q = &m->quads[qi];
        uint16_t vi[4] = { q->v0, q->v1, q->v2, q->v3 };
        uint16_t ni[4] = { q->n0, q->n1, q->n2, q->n3 };
        double P[4][3];
        for (int k = 0; k < 4; k++) {
            P[k][0] = m->quad_vertices[vi[k]].x;
            P[k][1] = m->quad_vertices[vi[k]].y;
            P[k][2] = m->quad_vertices[vi[k]].z;
        }
        /* Flaechennormale -> Hauptachse wegwerfen -> 2D */
        double u[3] = { P[1][0] - P[0][0], P[1][1] - P[0][1], P[1][2] - P[0][2] };
        double v[3] = { P[2][0] - P[0][0], P[2][1] - P[0][1], P[2][2] - P[0][2] };
        double nn[3] = { u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2],
                         u[0] * v[1] - u[1] * v[0] };
        int ax = 0;
        for (int k = 1; k < 3; k++)
            if ((nn[k] < 0 ? -nn[k] : nn[k]) > (nn[ax] < 0 ? -nn[ax] : nn[ax])) ax = k;
        int a0 = (ax + 1) % 3, a1 = (ax + 2) % 3;
        p2_t Q[4];
        for (int k = 0; k < 4; k++) { Q[k].x = P[k][a0]; Q[k].y = P[k][a1]; }

        if (schneiden(Q[0], Q[3], Q[1], Q[2]))      z_ord++;
        else if (schneiden(Q[0], Q[2], Q[1], Q[3])) umlauf++;
        else                                        entartet++;

        double deck = deckung(Q);
        deck_sum += deck;
        if (deck < deck_min) deck_min = deck;
        if (deck > 0.95) voll++;     /* Abtastung: schmale Ringflaechen messen 0,990 */

        for (int k = 0; k < 4; k++) {
            const re15_md1_vertex_t *N = &m->quad_normals[ni[k]];
            double s = (double)N->y * P[k][1] + (double)N->z * P[k][2];
            if (s > 1e-6) aussen++; else if (s < -1e-6) innen++; else quer++;
            long l2 = (long)N->x * N->x + (long)N->y * N->y + (long)N->z * N->z;
            if (l2 < 4095L * 4095L || l2 > 4097L * 4097L) laenge_ok = 0;
        }
    }
    printf("   Z-Ordnung %d | Umlauf %d | entartet %d ; Deckung Mittel %.3f, kleinste %.3f\n",
           z_ord, umlauf, entartet, deck_sum / (double)m->quad_count, deck_min);
    pruefe("alle 40 Vierecke in Z-Ordnung (Diagonalen 0-3 und 1-2 schneiden sich)",
           z_ord == 40 && umlauf == 0 && entartet == 0);
    pruefe("die Teilung (0,1,3)+(0,3,2) deckt jedes Viereck ganz (> 0,95; Umlauf = 0,75)", voll == 40);

    printf("\n== 4. Normalen ==\n");
    printf("   Eck-Normalen der Vierecke: nach AUSSEN %d | nach INNEN %d | quer %d\n",
           aussen, innen, quer);
    pruefe("160 von 160 Eck-Normalen zeigen nach aussen", aussen == 160 && innen == 0);
    pruefe("Normalenlaenge 4096 +-1", laenge_ok);

    /* Kappen: die Mittelpunkte der beiden Deckel liegen auf der Achse; ihre Normale zeigt
     * entlang X vom Koerper WEG. */
    int kappen = 0, kappen_ok = 0;
    for (int i = 0; i < (int)m->quad_vertex_count; i++) {
        const re15_md1_vertex_t *V = &m->quad_vertices[i];
        if (V->y == 0 && V->z == 0) {
            const re15_md1_vertex_t *N = &m->quad_normals[i];
            kappen++;
            printf("   Kappenmitte (%d,0,0) Normale (%d,%d,%d)\n", V->x, N->x, N->y, N->z);
            if ((long)N->x * V->x > 0 && N->y == 0 && N->z == 0) kappen_ok++;
        }
    }
    pruefe("beide Kappenmitten tragen die Normale von der Mitte weg", kappen == 2 && kappen_ok == 2);

    printf("\n%s - %d Pruefung(en) gerissen\n", fehler ? "FEHLGESCHLAGEN" : "ALLES BESTANDEN", fehler);
    return fehler ? 1 : 0;
}
