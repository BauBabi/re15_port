/* test_r38_rolltor20a0_windung.c — Runde 38 Spur W: Nebenwirkungs-Riegel des NCLIP im PC-Prop-Zeichenweg.
 *
 * Mit dem Fix (engine/src/objpoly_r38.c, Haken in main.c) zeichnet der PC jedes Raum-Objekt-Polygon nur noch, wenn
 * NCLIP >= 0 (Original FUN_80023098 `bgez` @0x80023190, FUN_800256b0 @0x800257c0, FUN_80022f0c `bltz` @0x80023010,
 * FUN_800254a0 @0x800255a8). Vorher zeichnete der PC auch Rueckseiten. Ein Modell, dessen Windung zu seinen eigenen
 * Normalen VERKEHRT herum laeuft, war bisher trotzdem sichtbar und waere jetzt weg. Fuer die Original-Modelle ist
 * das ausgeschlossen (das Original zeichnet sie mit demselben NCLIP); fuer die PORT-EIGENEN Modelle (eingebackene
 * MD1: Sicherung, Irons' Tagebuch/Karte, Granate, Hebetisch-Cursor, Dokumente, Inhalt R35, Schluessel R36,
 * Kartenitems R36, Feuerloescher R37) nicht.
 *
 * MESSGROESSE je Polygon (Objektraum): w = (v1-v0) x (v2-v0) (Windung der ersten drei Scheitel = was NCLIP auf dem
 * Schirm prueft) gegen n = Summe der gespeicherten Scheitelnormalen (Licht, zeigt nach aussen). Vorzeichen von w.n.
 *   O1  Zensus ueber ALLE Objekt-Modelle aller ausgelieferten RDTs (RDT+0x30, nOmodel): das vorherrschende Vorzeichen
 *       s (Original-Konvention, bei der NCLIP die Aussenseite zeigt) — eindeutig (>= 99 %).
 *   O2  Zensus der port-eigenen Modelle (nur Ausgabe). ERGEBNIS 2026-10-16: alle folgen s, AUSSER dem Feuerloescher
 *       R37 (36 von 148 Polygonen in s, 112 verkehrt; gen/feuerloescher_r37.inc, PORT-WAHL-Modell der Runde 37). Mit
 *       NCLIP waere er zerrissen. Deshalb gilt NCLIP im PC-Weg NUR fuer Modelle aus der RDT (main.c: md1 im RDT-Puffer);
 *       Port-Modelle bleiben zweiseitig wie bisher (PORT-WAHL, re15_objpoly_r38.h).
 *   O3  Der Rolladen (ROOM20A0 obj 1) folgt s (Gegenprobe: Original-Messung zeigt ihn in Cut 12 von vorn).
 *   O4  Der Schalter nclip: ein Rueckseiten-Viereck wird mit nclip = 1 verworfen und mit nclip = 0 gezeichnet.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_room_list.h"
#include "re15_dokumente.h"
#include "re15_feuerloescher_r37.h"
#include "re15_granate.h"
#include "re15_hebetisch_cursor.h"
#include "re15_inhalt_r35.h"
#include "re15_irons_tisch.h"
#include "re15_kartenitems_r36.h"
#include "re15_schluessel_r36.h"
#include "re15_sicherung.h"
#include "re15_objpoly_r38.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int g_fail = 0;
#define CHECK(cond, ...) do { \
    if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
} while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b && out_size) *out_size = (size_t)sz;
    return b;
}

/* Vorzeichen-Zaehlung eines Modells: plus/minus = Polygone mit w.n > 0 / < 0. */
typedef struct { long plus, minus; } zaehl_t;

static int64_t punkt(const re15_md1_vertex_t *a, const re15_md1_vertex_t *b, const re15_md1_vertex_t *c,
                     const re15_md1_vertex_t *const *n, int nn)
{
    const int64_t ux = b->x - a->x, uy = b->y - a->y, uz = b->z - a->z;
    const int64_t vx = c->x - a->x, vy = c->y - a->y, vz = c->z - a->z;
    const int64_t wx = uy * vz - uz * vy, wy = uz * vx - ux * vz, wz = ux * vy - uy * vx;
    int64_t sx = 0, sy = 0, sz = 0;
    for (int k = 0; k < nn; k++) { sx += n[k]->x; sy += n[k]->y; sz += n[k]->z; }
    return wx * sx + wy * sy + wz * sz;
}

static zaehl_t zaehle_md1(const uint8_t *data, int size)
{
    zaehl_t z = { 0, 0 };
    re15_md1_t m;
    if (!data || size <= 0 || re15_md1_parse(data, size, &m) != 0) return z;
    for (int i = 0; i < m.mesh_count; i++) {
        const re15_md1_mesh_t *h = &m.meshes[i];
        for (int t = 0; t < h->triangle_count; t++) {
            const re15_md1_triangle_t *f = &h->triangles[t];
            if (f->v0 >= (unsigned)h->tri_vertex_count || f->v1 >= (unsigned)h->tri_vertex_count ||
                f->v2 >= (unsigned)h->tri_vertex_count || !h->tri_normals ||
                f->n0 >= (unsigned)h->tri_normal_count || f->n1 >= (unsigned)h->tri_normal_count ||
                f->n2 >= (unsigned)h->tri_normal_count) continue;
            const re15_md1_vertex_t *n[3] = { &h->tri_normals[f->n0], &h->tri_normals[f->n1], &h->tri_normals[f->n2] };
            const int64_t d = punkt(&h->tri_vertices[f->v0], &h->tri_vertices[f->v1], &h->tri_vertices[f->v2], n, 3);
            if (d > 0) z.plus++; else if (d < 0) z.minus++;
        }
        for (int q = 0; q < h->quad_count; q++) {
            const re15_md1_quad_t *f = &h->quads[q];
            if (f->v0 >= (unsigned)h->quad_vertex_count || f->v1 >= (unsigned)h->quad_vertex_count ||
                f->v2 >= (unsigned)h->quad_vertex_count || f->v3 >= (unsigned)h->quad_vertex_count ||
                !h->quad_normals || f->n0 >= (unsigned)h->quad_normal_count ||
                f->n1 >= (unsigned)h->quad_normal_count || f->n2 >= (unsigned)h->quad_normal_count ||
                f->n3 >= (unsigned)h->quad_normal_count) continue;
            const re15_md1_vertex_t *n[4] = { &h->quad_normals[f->n0], &h->quad_normals[f->n1],
                                              &h->quad_normals[f->n2], &h->quad_normals[f->n3] };
            const int64_t d = punkt(&h->quad_vertices[f->v0], &h->quad_vertices[f->v1], &h->quad_vertices[f->v2], n, 4);
            if (d > 0) z.plus++; else if (d < 0) z.minus++;
        }
    }
    return z;
}

static int s_port_verkehrt = 0;   /* Port-Modelle mit < 90 % Original-Windung */
static int s_loescher_verkehrt = 0;

static int pruefe_port(const char *name, const uint8_t *data, int size, int s)
{
    if (!data || size <= 0) return 0;
    const zaehl_t z = zaehle_md1(data, size);
    const long gut = (s > 0) ? z.plus : z.minus, schlecht = (s > 0) ? z.minus : z.plus;
    const int verkehrt = gut * 10 < (gut + schlecht) * 9;
    printf("  Port-Modell %-34s Polygone %4ld: Original-Windung %4ld, verkehrt %4ld%s\n", name, gut + schlecht, gut,
           schlecht, verkehrt ? "  <- mit NCLIP zerrissen (darum: Port-Modelle ohne NCLIP)" : "");
    CHECK(gut + schlecht > 0, "O2: %s ohne auswertbare Polygone", name);
    s_port_verkehrt += verkehrt;
    if (verkehrt && strstr(name, "Feuerloescher")) s_loescher_verkehrt++;
    return 1;
}

int main(void)
{
    /* O1: Zensus aller Objekt-Modelle aller RDTs */
    long plus = 0, minus = 0, modelle = 0, raeume = 0;
    for (int r = 0; r < RE15_ROOM_COUNT; r++) {
        const unsigned id = re15_room_ids[r];
        char path[512];
        snprintf(path, sizeof path, "%s/STAGE%X/ROOM%04X.RDT", RE15_XSTR(RE15_ASSETS_PATH), (id >> 12) & 0xF, id);
        size_t sz = 0;
        uint8_t *buf = read_file(path, &sz);
        if (!buf) continue;
        re15_rdt_t rdt;
        memset(&rdt, 0, sizeof rdt);
        if (re15_rdt_parse(buf, sz, &rdt) == 0) {
            raeume++;
            for (int k = 0; k < rdt.prop_count; k++) {
                if (!rdt.prop_md1[k]) continue;
                const zaehl_t z = zaehle_md1(rdt.prop_md1[k], rdt.prop_md1_size[k]);
                if (z.plus + z.minus == 0) continue;
                modelle++; plus += z.plus; minus += z.minus;
            }
        }
        free(buf);
    }
    const int s = (plus >= minus) ? 1 : -1;
    printf("O1 Zensus: %ld Raeume, %ld Objekt-Modelle, Polygone w.n > 0: %ld, < 0: %ld -> Original-Windung s = %+d\n",
           raeume, modelle, plus, minus, s);
    CHECK(raeume >= 200 && modelle >= 400, "O1: Zensus zu klein (%ld Raeume, %ld Modelle)", raeume, modelle);
    CHECK((s > 0 ? plus : minus) * 100 >= (plus + minus) * 99, "O1: Original-Windung nicht eindeutig (%ld/%ld)", plus,
          minus);

    /* O3: der Rolladen */
    {
        char path[512];
        snprintf(path, sizeof path, "%s/STAGE2/ROOM20A0.RDT", RE15_XSTR(RE15_ASSETS_PATH));
        size_t sz = 0;
        uint8_t *buf = read_file(path, &sz);
        re15_rdt_t rdt;
        memset(&rdt, 0, sizeof rdt);
        CHECK(buf && re15_rdt_parse(buf, sz, &rdt) == 0 && rdt.prop_count > 1, "O3: ROOM20A0.RDT");
        if (buf && rdt.prop_count > 1) {
            const zaehl_t z = zaehle_md1(rdt.prop_md1[1], rdt.prop_md1_size[1]);
            printf("O3 Rolladen ROOM20A0 obj 1: w.n > 0: %ld, < 0: %ld\n", z.plus, z.minus);
            CHECK((s > 0 ? z.plus : z.minus) == z.plus + z.minus, "O3: Rolladen nicht durchgehend in Original-Windung");
        }
        free(buf);
    }

    /* O2: port-eigene Modelle */
    int n = 0, sz = 0;
    const uint8_t *d;
    d = re15_sicherung_md1_bytes(&sz);          n += pruefe_port("Sicherung (1150)", d, sz, s);
    d = re15_granate_md1_bytes(&sz);            n += pruefe_port("Granate (1150)", d, sz, s);
    d = re15_hebetisch_cursor_md1_bytes(&sz);   n += pruefe_port("Hebetisch-Cursor (1150)", d, sz, s);
    for (int oid = 0; oid < 32; oid++) {
        d = re15_irons_tisch_md1_bytes(oid, &sz);
        if (d && sz > 0) { char t[64]; snprintf(t, sizeof t, "Irons-Tisch obj %d", oid); n += pruefe_port(t, d, sz, s); }
    }
    {
        const uint8_t *gesehen[64]; int ng = 0;
        for (int r = 0; r < RE15_ROOM_COUNT; r++) {
            const uint16_t id = (uint16_t)re15_room_ids[r];
            const struct { const char *name; const uint8_t *(*f)(uint16_t, int *); } quellen[] = {
                { "Dokument", re15_dokumente_md1_bytes },
                { "Inhalt R35", re15_inhalt_r35_md1_bytes },
                { "Schluessel R36", re15_schluessel_r36_md1_bytes },
                { "Kartenitem R36", re15_kartenitems_r36_md1_bytes },
                { "Feuerloescher R37", re15_feuerloescher_r37_md1_bytes },
            };
            for (int q = 0; q < 5; q++) {
                d = quellen[q].f(id, &sz);
                if (!d || sz <= 0) continue;
                int neu = 1;
                for (int g = 0; g < ng; g++) if (gesehen[g] == d) neu = 0;
                if (!neu || ng >= 64) continue;
                gesehen[ng++] = d;
                char t[64]; snprintf(t, sizeof t, "%s (ROOM%04X)", quellen[q].name, (unsigned)id);
                n += pruefe_port(t, d, sz, s);
            }
        }
    }
    printf("O2: %d port-eigene Modelle geprueft, %d davon nicht in Original-Windung (Feuerloescher-Lagen: %d)\n", n,
           s_port_verkehrt, s_loescher_verkehrt);
    CHECK(n >= 5, "O2: zu wenige port-eigene Modelle gefunden (%d)", n);

    /* O4: der Schalter nclip (main.c: 1 fuer RDT-Modelle, 0 fuer Port-Modelle) */
    {
        /* Viereck 2000 vor der Kamera in Z-Ordnung; "hinten" = dieselben Ecken gespiegelt (OPZ < 0). */
        const int32_t vorn[4][3]   = { { -500, -500, 2000 }, { 500, -500, 2000 }, { -500, 500, 2000 }, { 500, 500, 2000 } };
        const int32_t hinten[4][3] = { { 500, -500, 2000 }, { -500, -500, 2000 }, { 500, 500, 2000 }, { -500, 500, 2000 } };
        re15_objpoly_t p;
        const int v1 = re15_objpoly_r38_quad(vorn, 208, 160, 120, 1, &p);
        const int opz_vorn = (int)p.opz;
        const int h1 = re15_objpoly_r38_quad(hinten, 208, 160, 120, 1, &p);
        const int opz_hinten = (int)p.opz;
        const int h0 = re15_objpoly_r38_quad(hinten, 208, 160, 120, 0, &p);
        printf("O4 nclip: Vorderseite opz %d -> %d, Rueckseite opz %d -> nclip=1: %d, nclip=0: %d\n", opz_vorn, v1,
               opz_hinten, h1, h0);
        CHECK(opz_vorn >= 0 && opz_hinten < 0, "O4: Testvierecke falsch orientiert (%d/%d)", opz_vorn, opz_hinten);
        CHECK(v1 == 1 && h1 == 0 && h0 == 1, "O4: nclip-Schalter wirkt nicht (vorn %d, hinten nclip=1 %d, nclip=0 %d)",
              v1, h1, h0);
    }

    if (g_fail) { printf("test_r38_rolltor20a0_windung: %d FEHLER\n", g_fail); return 1; }
    printf("test_r38_rolltor20a0_windung: OK\n");
    return 0;
}
