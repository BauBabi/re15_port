/* probe_r30_karten-marken_rundlauf.c - Runde 30, Thema F (Fortsetzung): MESS-WERKZEUG
 * (kein Pin, kein add_test). Dossier: analysis/befunde_runde30/karten-marken.md
 *
 *   F  RUNDLAUF JE ORT   jede Haupt-Zonenzeile EINZELN begehen (auf jedem ihrer
 *                        Baender) -> Spieler in einen neutralen Speicherraum ->
 *                        capture -> Karte -> Null-Zustand -> load -> restore ->
 *                        Vergleich: Zonen-Bits, Etagen-Bits, Rechtecke, Marken
 *   G  NUTZER-KARTE      Slot <n> der Karte des Nutzers laden (frischer Zustand) und
 *                        ausgeben, was der Zeichner danach je Blatt zeigt; je gesetztem
 *                        Besucht-Bit jede Zeile des Ortes mit ihrem Rechteck-Zustand
 *   H  MARKEN-ZENSUS     alle Blaetter, alles begangen: jede SICHTBARE Marke, deren
 *                        Balken samt 1-Punkt-Saum auf KEINEM bemalten Texel eines
 *                        gezeichneten Rechtecks und in KEINER Schema-Zelle liegt
 *
 * Aufruf: probe_r30_karten-marken_rundlauf <arbeitskarte.mcr> <nutzerkarte.mcr> <slot>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "re15_room.h"
#include "re15_room_list.h"
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_inv_screen.h"
#include "re15_map_owned.h"
#include "re15_map_zones.h"      /* engine/src: dieselben Tabellen, die die Engine fuehrt */

extern int re15_map_zone_bit_test(int zonen_index);
extern int re15_map_floor_row_visited(unsigned room, int zone, int band);

#define NZ ((int)(sizeof s_map_zones  / sizeof s_map_zones[0]))
#define NM ((int)(sizeof s_map_marks  / sizeof s_map_marks[0]))
#define NF ((int)(sizeof s_map_floors / sizeof s_map_floors[0]))
#define NS ((int)(sizeof s_map_synth  / sizeof s_map_synth[0]))
#define SEITEN 13
#define RMAX   32

static unsigned char g_px[SEITEN][256][256];
static int g_px_ok[SEITEN];
static void blaetter_laden(void)
{
    int p, x, y;
    static unsigned char roh[256 * 128];
    for (p = 0; p < SEITEN; p++) {
        char pfad[700]; FILE *f;
        snprintf(pfad, sizeof pfad, "%s/DATA/MAP%02X.PIX", RE15_ASSET_PSX_DIR, p + 1);
        f = fopen(pfad, "rb");
        if (!f) continue;
        if (fread(roh, 1, sizeof roh, f) == sizeof roh) {
            for (y = 0; y < 256; y++)
                for (x = 0; x < 256; x++)
                    g_px[p][y][x] = (x & 1) ? (unsigned char)(roh[y * 128 + (x >> 1)] >> 4)
                                            : (unsigned char)(roh[y * 128 + (x >> 1)] & 15);
            g_px_ok[p] = 1;
        }
        fclose(f);
    }
}
static int kachel_index(int page, int rect, int sx, int sy)
{
    int rx, ry, rw, rh, u, v;
    if (page < 0 || page >= SEITEN || !g_px_ok[page]) return -1;
    if (!re15_map_rect_geometry((unsigned)page, (unsigned)rect, &rx, &ry, &rw, &rh)) return -1;
    if (!re15_map_rect_uv((unsigned)page, (unsigned)rect, &u, &v)) return -1;
    if (sx < rx || sx >= rx + rw || sy < ry || sy >= ry + rh) return -1;
    return g_px[page][(v + (sy - ry)) & 255][(u + (sx - rx)) & 255];
}

static void stehe(unsigned room, int32_t x, int32_t z, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_current_room_id = room;
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = x; pl->z = z;
    pl->y = -(int32_t)band * 0x708;                 /* band_from_y = -(y/0x708) */
    re15_collision_set_band(band);
    re15_map_zone_update(room, x, z);
}

static int punkt_in_zone(const re15_map_zone_t *zn, int32_t *px, int32_t *pz)
{
    int gx, gy, ring;
    for (ring = 0; ring <= 8; ring++)
        for (gy = 8 - ring; gy <= 8 + ring; gy++)
            for (gx = 8 - ring; gx <= 8 + ring; gx++) {
                int32_t x, z; const re15_map_zone_t *t;
                if (gx < 1 || gx > 15 || gy < 1 || gy > 15) continue;
                x = zn->wx0 + (int32_t)(((int64_t)(zn->wx1 - zn->wx0) * gx) / 16);
                z = zn->wz0 + (int32_t)(((int64_t)(zn->wz1 - zn->wz0) * gy) / 16);
                t = re15_map_zone_at(zn->room, x, z);
                if (t && t->room == zn->room && t->idx == zn->idx) { *px = x; *pz = z; return 1; }
            }
    return 0;
}

/* Eine Haupt-Zonenzeile auf JEDEM ihrer Baender begehen. Rueckgabe: Zahl der Baender. */
static int begehe_zeile(int i)
{
    const re15_map_zone_t *zn = &s_map_zones[i];
    int32_t x = 0, z = 0; int baender[16], nb = 0, j;
    if (!punkt_in_zone(zn, &x, &z)) return -1;
    for (j = 0; j < NF; j++) {
        int k, da = 0;
        if (s_map_floors[j].room != zn->room || s_map_floors[j].zone != zn->idx) continue;
        for (k = 0; k < nb; k++) if (baender[k] == s_map_floors[j].band) da = 1;
        if (!da && nb < 16) baender[nb++] = s_map_floors[j].band;
    }
    if (nb == 0) baender[nb++] = 0;
    for (j = 0; j < nb; j++) stehe(zn->room, x, z, baender[j]);
    return nb;
}

typedef struct {
    unsigned char rect[SEITEN][RMAX];
    unsigned char marke[512];
    unsigned char etage[256];
    unsigned char bits[32];
} aufnahme_t;

static void aufnehmen(aufnahme_t *a)
{
    int p, r, i;
    memset(a, 0, sizeof *a);
    for (p = 0; p < SEITEN; p++) {
        int cnt = re15_map_rect_count((unsigned)p);
        for (r = 0; r < cnt && r < RMAX; r++)
            a->rect[p][r] = (unsigned char)re15_map_rect_state((unsigned)p, (unsigned)r);
    }
    for (i = 0; i < NM && i < 512; i++) {
        int pg, rc, mx, my, kind;
        a->marke[i] = (unsigned char)re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind);
    }
    for (i = 0; i < NF && i < 256; i++)
        a->etage[i] = (unsigned char)re15_map_floor_row_visited(s_map_floors[i].room,
                          s_map_floors[i].zone, s_map_floors[i].band);
    re15_map_visited_export(a->bits);
}

static const char *zname(int s)
{
    return s == RE15_MAP_RECT_CURRENT ? "AKTUELL" : s == RE15_MAP_RECT_VISITED ? "BESUCHT"
         : s == RE15_MAP_RECT_UNVISITED ? "unbesucht" : "ohne-Zone";
}

static int bitzahl(const unsigned char *b)
{
    int k, n = 0, j;
    for (k = 0; k < 32; k++) for (j = 0; j < 8; j++) n += (b[k] >> j) & 1;
    return n;
}

/* Ist der Schirmpunkt von einem GEZEICHNETEN Rechteck bemalt (Texel-Index != 0) oder
 * liegt er in einer gezeichneten Schema-Zelle? */
static int punkt_getragen(int page, int sx, int sy)
{
    int r, cnt = re15_map_rect_count((unsigned)page), i;
    for (r = 0; r < cnt && r < RMAX; r++) {
        int st = re15_map_rect_state((unsigned)page, (unsigned)r);
        int ki;
        /* Gezeichnet wird wie im Zeichner re15_inv_screen.c:2317-2341: BESUCHT/AKTUELL
         * immer, UNBESUCHT nur mit der Karte des Blatts, OHNE-ZONE (UNMAPPED) nie. */
        if (st == RE15_MAP_RECT_UNMAPPED) continue;
        if (st == RE15_MAP_RECT_UNVISITED && !re15_map_owned_page((unsigned)page)) continue;
        ki = kachel_index(page, r, sx, sy);
        if (ki > 0) return 1;
    }
    for (i = 0; i < NZ; i++) {
        const re15_map_zone_t *zn = &s_map_zones[i];
        const re15_map_synth_t *s;
        if (!zn->synth || zn->page != page || (zn->room & 1)) continue;
        if (!re15_map_zone_visited(re15_map_zone_by_index(i))) continue;
        s = &s_map_synth[zn->synth - 1];
        if (sx >= s->x && sx < s->x + s->w && sy >= s->y && sy < s->y + s->h) return 1;
    }
    return 0;
}

/* Balken der Marke: kind 0/2 (Nord/Sued) waagerecht 5x1, 1/3 (Ost/West) senkrecht 1x5,
 * Treppen 4/5 als 5x5 behandelt - gemessen an den Nutzer-Abzuegen (188,178..182) kind 3
 * und (172..176,101) kind 0. */
static int marke_getragen(int page, int mx, int my, int kind, int saum)
{
    int hx = (kind == 1 || kind == 3) ? 0 : 2;
    int hy = (kind == 0 || kind == 2) ? 0 : 2;
    int x, y;
    for (y = my - hy - saum; y <= my + hy + saum; y++)
        for (x = mx - hx - saum; x <= mx + hx + saum; x++)
            if (punkt_getragen(page, x, y)) return 1;
    return 0;
}

static void zeilen_des_ortes(unsigned room, int idx)
{
    int i, j;
    for (i = 0; i < NZ; i++) {
        const re15_map_zone_t *zn = &s_map_zones[i];
        int st;
        if (zn->room != room || zn->idx != idx) continue;
        st = (zn->rect == 255) ? -1 : re15_map_rect_state(zn->page, zn->rect);
        printf("        Zeile %3d %s Blatt %2d rect %3d%s -> %s", i,
               zn->etage ? "GAST " : "HAUPT", zn->page, zn->rect,
               zn->synth ? " (Schema)" : "",
               zn->rect == 255 ? (zn->synth ? (re15_map_zone_visited(re15_map_zone_by_index(i)) ? "Schema GEZEICHNET" : "Schema nicht gezeichnet")
                                            : "KEINE ZEICHNUNG (rect 255, kein Schema)")
                               : zname(st));
        for (j = 0; j < NF; j++) {
            if (s_map_floors[j].room != room || s_map_floors[j].zone != idx) continue;
            if (s_map_floors[j].page != zn->page || s_map_floors[j].rect != zn->rect) continue;
            printf("  [Etagenzeile %d Band %d: Bit %s]", j, s_map_floors[j].band,
                   re15_map_floor_row_visited(room, idx, s_map_floors[j].band) ? "gesetzt" : "FEHLT");
        }
        printf("\n");
    }
}

/* ---- I: Zensus der Op-Liste des Kartenschirms ------------------------------------
 * Baut je Blatt die Op-Liste des echten Zeichners (re15_inv_screen_build, Muster aus
 * test_map_synth.c) und zaehlt die FILL-Ops in RE2-Blau (16,64,176) - getrennt nach
 * Schema-Zelle (deckt sich mit einer Zeile aus s_map_synth_cells) und Linie (w==1
 * oder h==1 = Innenwand). */
static void op_zensus(const char *titel)
{
    static re15_inv_op_t ops[1024];
    int p, k, c, ges_blau = 0, ges_px = 0;
    printf("  --- %s ---\n", titel);
    for (p = 0; p < SEITEN; p++) {
        int nops, n_schema = 0, n_linie = 0, n_sonst = 0, px = 0, abe1 = 0;
        re15_inv_map_stage_init(0, 13);
        re15_inv_screen_open();
        g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
        g_inv_screen.map_page = (uint8_t)p;
        nops = re15_inv_screen_build(&g_inv_screen, ops, 1024);
        for (k = 0; k < nops; k++) {
            int zelle = 0;
            if (ops[k].kind != RE15_INV_OP_FILL) continue;
            if (ops[k].r != 16 || ops[k].g != 64 || ops[k].b != 176) continue;
            for (c = 0; c < (int)(sizeof s_map_synth_cells / sizeof s_map_synth_cells[0]); c++)
                if (ops[k].x == s_map_synth_cells[c].x && ops[k].y == s_map_synth_cells[c].y &&
                    ops[k].w == s_map_synth_cells[c].w && ops[k].h == s_map_synth_cells[c].h) zelle = 1;
            if (zelle) n_schema++;
            else if (ops[k].w == 1 || ops[k].h == 1) n_linie++;
            else n_sonst++;
            if (ops[k].abe) abe1++;
            px += ops[k].w * ops[k].h;
            printf("      Blatt %2d Op %3d: FILL (%d,%d) %dx%d abe %d rgb (%d,%d,%d) -> %s\n", p, k,
                   ops[k].x, ops[k].y, ops[k].w, ops[k].h, ops[k].abe, ops[k].r, ops[k].g, ops[k].b,
                   zelle ? "SCHEMA-ZELLE" : (ops[k].w == 1 || ops[k].h == 1) ? "LINIE (Innenwand)" : "sonst");
        }
        if (n_schema + n_linie + n_sonst)
            printf("    Blatt %2d: %d Ops gesamt, blau: Schema %d, Linie %d, sonst %d, Flaeche %d Punkte, "
                   "davon abe=1: %d\n", p, nops, n_schema, n_linie, n_sonst, px, abe1);
        ges_blau += n_schema + n_linie + n_sonst; ges_px += px;
    }
    printf("    SUMME %s: %d blaue FILL-Ops, %d Punkte Flaeche (vor Ueberdeckung)\n", titel, ges_blau, ges_px);
}

int main(int argc, char **argv)
{
    const char *karte  = (argc > 1) ? argv[1] : "r30_rundlauf.mcr";
    const char *nutzer = (argc > 2) ? argv[2] : NULL;
    int slot = (argc > 3) ? atoi(argv[3]) : 2;
    int i;

    setvbuf(stdout, NULL, _IONBF, 0);
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_map_stock_set(0);
    blaetter_laden();

    /* =============================== F RUNDLAUF ============================= */
    printf("=== F. RUNDLAUF JE ORT (jede Haupt-Zonenzeile einzeln) ===\n");
    {
        int orte = 0, ohne_punkt = 0, bit_verlust = 0, etage_verlust_orte = 0;
        int rect_verlust_ges = 0, marken_verlust_ges = 0, etagen_bits_ges = 0, sauber = 0;
        for (i = 0; i < NZ; i++) {
            const re15_map_zone_t *zn = &s_map_zones[i];
            aufnahme_t vor, nach; re15_savedata_t sd, zur; uint16_t rr = 0;
            unsigned sraum; int32_t sx, sz; int nb, p, r, k, bd = 0, ev = 0, rv = 0, mv = 0, rg = 0, mg = 0;
            if (zn->etage || (zn->room & 1)) continue;
            orte++;
            re15_map_visited_reset();
            nb = begehe_zeile(i);
            if (nb < 0) { ohne_punkt++;
                printf("  [!] ROOM%04X/z%d: kein Punkt trifft diese Zone - uebersprungen\n",
                       zn->room, zn->idx); continue; }
            /* neutraler Speicherraum: ROOM1150 (Spawn der Raumliste), fuer ROOM1150 selbst ROOM1130 */
            if (zn->room == 0x1150) { sraum = 0x1130; } else { sraum = 0x1150; }
            { const re15_map_zone_t *sz0 = re15_map_zone_fuer(sraum, 0, sraum == 0x1150 ? 4 : 4);
              int32_t px = 0, pz = 0;
              if (!sz0) { int q; for (q = 0; q < NZ; q++) if (s_map_zones[q].room == sraum && !s_map_zones[q].etage) { sz0 = &s_map_zones[q]; break; } }
              if (!sz0 || !punkt_in_zone(sz0, &px, &pz)) { printf("  FAIL: Speicherraum %04X\n", sraum); return 1; }
              sx = px; sz = pz; }
            stehe(sraum, sx, sz, 0);
            aufnehmen(&vor);
            re15_savedata_capture(&sd, 1000, 0);
            remove(karte);
            if (re15_memcard_save(karte, 0, &sd, "R30 RUNDLAUF") != 0) { printf("  FAIL: schreiben\n"); return 1; }
            re15_map_visited_reset();                       /* frischer Prozess */
            memset(&zur, 0, sizeof zur);
            if (re15_memcard_load(karte, 0, &zur) != 0) { printf("  FAIL: lesen\n"); return 1; }
            if (re15_savedata_restore(&zur, &rr) != 0) { printf("  FAIL: restore\n"); return 1; }
            stehe(rr, g_actors[0].x, g_actors[0].z, re15_collision_band_from_y(g_actors[0].y));
            aufnehmen(&nach);
            for (k = 0; k < 32; k++) { unsigned char d = (unsigned char)(vor.bits[k] ^ nach.bits[k]); int b;
                for (b = 0; b < 8; b++) bd += (d >> b) & 1; }
            for (k = 0; k < NF; k++) if (vor.etage[k] && !nach.etage[k]) ev++;
            for (p = 0; p < SEITEN; p++) { int cnt = re15_map_rect_count((unsigned)p);
                for (r = 0; r < cnt && r < RMAX; r++) {
                    if (vor.rect[p][r] > nach.rect[p][r]) rv++;
                    else if (vor.rect[p][r] < nach.rect[p][r]) rg++; } }
            for (k = 0; k < NM; k++) { if (vor.marke[k] && !nach.marke[k]) mv++;
                                       if (!vor.marke[k] && nach.marke[k]) mg++; }
            if (bd) bit_verlust++;
            if (ev || rv || mv || rg || mg || bd) {
                etage_verlust_orte++;
                printf("  ROOM%04X/z%d (Zeile %3d, Bit %3d, %d Band/Baender, Blatt %d rect %d): "
                       "Zonen-Bits %d anders, Etagen-Bits -%d, Rechtecke -%d/+%d, Marken -%d/+%d\n",
                       zn->room, zn->idx, i, re15_map_zone_bit_test(i), nb, zn->page, zn->rect,
                       bd, ev, rv, rg, mv, mg);
                for (p = 0; p < SEITEN; p++) { int cnt = re15_map_rect_count((unsigned)p);
                    for (r = 0; r < cnt && r < RMAX; r++)
                        if (vor.rect[p][r] != nach.rect[p][r])
                            printf("        Blatt %2d rect %2d: %s -> %s\n", p, r,
                                   zname(vor.rect[p][r]), zname(nach.rect[p][r])); }
                for (k = 0; k < NM; k++) if (vor.marke[k] != nach.marke[k])
                    printf("        Marke %3d Blatt %d (%d,%d): %s -> %s\n", k, s_map_marks[k].page,
                           s_map_marks[k].mx, s_map_marks[k].my,
                           vor.marke[k] ? "sichtbar" : "weg", nach.marke[k] ? "sichtbar" : "weg");
            } else sauber++;
            rect_verlust_ges += rv; marken_verlust_ges += mv; etagen_bits_ges += ev;
        }
        printf("  SUMME: %d Orte (Haupt-Zeilen, Leon), %d ohne treffbaren Punkt, %d verlustfrei, "
               "%d mit Abweichung\n", orte, ohne_punkt, sauber, etage_verlust_orte);
        printf("         Orte mit veraendertem ZONEN-Bit: %d | verlorene Etagen-Bits %d, "
               "Rechtecke %d, Marken %d\n", bit_verlust, etagen_bits_ges, rect_verlust_ges,
               marken_verlust_ges);
        re15_map_visited_reset();
    }

    /* =============================== G NUTZER-KARTE ========================= */
    printf("=== G. NUTZER-KARTE ===\n");
    if (nutzer) {
        re15_savedata_t sd; uint16_t rr = 0; unsigned char bits[32]; int p, r, b;
        memset(&sd, 0, sizeof sd);
        re15_map_visited_reset();
        if (re15_memcard_load(nutzer, slot, &sd) != 0) { printf("  FAIL: Karte %s Slot %d\n", nutzer, slot); return 1; }
        printf("  Karte %s\n  Slot %d: Version %u, Raum %04X, cut %u, pos (%d,%d,%d), hp %d, save_count %u\n",
               nutzer, slot, (unsigned)sd.version, (unsigned)sd.room, (unsigned)sd.camera_cut,
               (int)sd.player_x, (int)sd.player_y, (int)sd.player_z, (int)sd.player_hp,
               (unsigned)sd.save_count);
        printf("  visited[] im Block: %d Bits gesetzt\n", bitzahl(sd.visited));
        if (re15_savedata_restore(&sd, &rr) != 0) { printf("  FAIL: restore\n"); return 1; }
        stehe(rr, g_actors[0].x, g_actors[0].z, re15_collision_band_from_y(g_actors[0].y));
        re15_map_visited_export(bits);
        printf("  nach restore + erstem Zonen-Update: %d Bits gesetzt, visited[] bytegleich zum Block: %s\n",
               bitzahl(bits), memcmp(bits, sd.visited, 32) == 0 ? "ja" : "NEIN");
        { int ne = 0; for (i = 0; i < NF; i++) ne += re15_map_floor_row_visited(s_map_floors[i].room,
                          s_map_floors[i].zone, s_map_floors[i].band) ? 1 : 0;
          printf("  Etagen-Bits nach dem Laden: %d von %d Zeilen\n", ne, NF);
          for (i = 0; i < NF; i++) if (re15_map_floor_row_visited(s_map_floors[i].room,
                          s_map_floors[i].zone, s_map_floors[i].band))
              printf("      Etagenzeile %d: ROOM%04X/z%d Band %d -> Blatt %d rect %d\n", i,
                     s_map_floors[i].room, s_map_floors[i].zone, s_map_floors[i].band,
                     s_map_floors[i].page, s_map_floors[i].rect); }
        printf("  --- G1: je gesetztem Bit der Ort und ALLE seine Zeilen ---\n");
        for (b = 0; b < 256; b++) {
            int seen = 0;
            if (!((sd.visited[b >> 3] >> (b & 7)) & 1)) continue;
            for (i = 0; i < NZ; i++) {
                const re15_map_zone_t *zn = &s_map_zones[i];
                if (zn->etage || (zn->room & 1)) continue;
                if (re15_map_zone_bit_test(i) != b) continue;
                printf("    Bit %3d = ROOM%04X/z%d\n", b, zn->room, zn->idx);
                zeilen_des_ortes(zn->room, zn->idx);
                seen = 1;
            }
            if (!seen) printf("    Bit %3d = KEINE Zonenzeile traegt dieses Bit\n", b);
        }
        printf("  --- G2: gezeichnete Rechtecke je Blatt (BESUCHT/AKTUELL) ---\n");
        for (p = 0; p < SEITEN; p++) {
            int cnt = re15_map_rect_count((unsigned)p), n = 0;
            for (r = 0; r < cnt && r < RMAX; r++) {
                int st = re15_map_rect_state((unsigned)p, (unsigned)r);
                if (st == RE15_MAP_RECT_VISITED || st == RE15_MAP_RECT_CURRENT) {
                    if (!n) printf("    Blatt %2d:", p);
                    printf(" r%d=%s", r, zname(st)); n++;
                }
            }
            if (n) printf("\n");
        }
        printf("  --- G3: sichtbare Marken, die NICHT getragen sind (Balken + 1 Punkt Saum) ---\n");
        { int n = 0, sicht = 0;
          for (i = 0; i < NM; i++) {
            int pg, rc, mx, my, kind;
            if (!re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind)) continue;
            sicht++;
            if (marke_getragen(pg, mx, my, kind, 1)) continue;
            printf("    Marke %3d Blatt %d rect %3d (%d,%d) kind %d zid %d zid2 %d\n", i, pg, rc, mx, my,
                   kind, s_map_marks[i].zid, s_map_marks[i].zid2); n++;
          }
          printf("    sichtbar %d, davon ohne Traeger %d\n", sicht, n); }
        printf("=== I1. OP-LISTE, Stand des Nutzers ===\n");
        op_zensus("Nutzer-Karte geladen");
        re15_map_visited_reset();
    } else printf("  (keine Nutzer-Karte angegeben)\n");

    /* =============================== H MARKEN-ZENSUS ======================== */
    printf("=== H. MARKEN-ZENSUS, alles begangen, alle Blaetter ===\n");
    {
        int je_blatt[SEITEN], sicht_blatt[SEITEN], n = 0, sicht = 0, p;
        memset(je_blatt, 0, sizeof je_blatt); memset(sicht_blatt, 0, sizeof sicht_blatt);
        re15_map_visited_reset();
        for (i = 0; i < NZ; i++) {
            const re15_map_zone_t *zn = &s_map_zones[i];
            if (zn->etage || (zn->room & 1)) continue;
            begehe_zeile(i);
        }
        { const re15_map_zone_t *s0 = NULL; int32_t px = 0, pz = 0; int q;
          for (q = 0; q < NZ; q++) if (s_map_zones[q].room == 0x1150 && !s_map_zones[q].etage) { s0 = &s_map_zones[q]; break; }
          if (s0 && punkt_in_zone(s0, &px, &pz)) stehe(0x1150, px, pz, 0); }
        for (i = 0; i < NM; i++) {
            int pg, rc, mx, my, kind, eig = 0, q;
            if (!re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind)) continue;
            sicht++; if (pg < SEITEN) sicht_blatt[pg]++;
            if (marke_getragen(pg, mx, my, kind, 1)) continue;
            for (q = 0; q < NZ; q++) if (s_map_zones[q].zid == s_map_marks[i].zid &&
                                         s_map_zones[q].page == pg) eig = 1;
            printf("  Marke %3d Blatt %2d rect %3d (%3d,%3d) kind %d zid %3d zid2 %3d auf_partner %d | "
                   "zid hat Zeile auf diesem Blatt: %s | Mittelpunkt-Texel:", i, pg, rc, mx, my, kind,
                   s_map_marks[i].zid, s_map_marks[i].zid2, s_map_marks[i].auf_partner, eig ? "ja" : "NEIN");
            { int r, cnt = re15_map_rect_count((unsigned)pg), da = 0;
              for (r = 0; r < cnt && r < RMAX; r++) { int ki = kachel_index(pg, r, mx, my);
                  if (ki >= 0) { printf(" r%d=%d", r, ki); da = 1; } }
              if (!da) printf(" in KEINEM Rechteck"); }
            printf("\n");
            n++; if (pg < SEITEN) je_blatt[pg]++;
        }
        printf("  SUMME: sichtbar %d von %d Marken, ohne Traeger %d\n", sicht, NM, n);
        printf("=== I2. OP-LISTE, alles begangen ===\n");
        op_zensus("alles begangen");
        for (p = 0; p < SEITEN; p++)
            printf("    Blatt %2d: sichtbar %3d, ohne Traeger %d\n", p, sicht_blatt[p], je_blatt[p]);
    }
    /* =============================== J GEGENPROBE TUER 1090<->1100 ========== */
    printf("=== J. GEGENPROBE: Tuer ROOM1090(oben) <-> ROOM1100 von BEIDEN Seiten ===\n");
    {
        static const struct { unsigned room; int page; int32_t x, z; const char *was; } pt[] = {
            { 0x1090, 3,  -5820, -18690, "ROOM1090 Tuer #1 Trigger-Mitte (ROOM1090.RDT @0x0213A)" },
            { 0x1100, 3, -20900, -10336, "Ankunft in ROOM1100 aus ROOM1090 (ROOM1090.RDT @0x02148..)" },
            { 0x1100, 3, -26880, -10936, "ROOM1100 Tuer #1 -> ROOM1110 Trigger-Mitte (ROOM1100.RDT @0x009BA)" },
            { 0x1100, 3, -21330, -24790, "ROOM1100 Tuer #0 -> ROOM10D0 Trigger-Mitte (ROOM1100.RDT @0x0099A)" },
            { 0x1090, 3, -13600,   1300, "Ankunft in ROOM1090(oben) aus ROOM10F0 (ROOM10F0.RDT @0x00F52)" },
            { 0x10F0, 3,  -4750,  -1750, "ROOM10F0 Tuer #1 -> ROOM1090 Trigger-Mitte (ROOM10F0.RDT @0x00F52)" },
        };
        int k;
        for (k = 0; k < (int)(sizeof pt / sizeof pt[0]); k++) {
            const re15_map_zone_t *zn = re15_map_zone_fuer(pt[k].room, 0, (unsigned)pt[k].page);
            int rx, ry, rw, rh, r, cnt; int16_t mx = 0, my = 0;
            if (!zn || !re15_map_rect_geometry((unsigned)pt[k].page, zn->rect, &rx, &ry, &rw, &rh)) {
                printf("  %s: keine Zeile auf Blatt %d\n", pt[k].was, pt[k].page); continue; }
            re15_map_zone_marker(zn, pt[k].x, pt[k].z, rx, ry, rw, rh, &mx, &my);
            printf("  %s\n      Welt (%d,%d) -> Blatt %d rect %d (%d,%d) %dx%d flip %d/%d -> Karte (%d,%d); Texel je Rechteck:",
                   pt[k].was, (int)pt[k].x, (int)pt[k].z, pt[k].page, zn->rect, rx, ry, rw, rh,
                   zn->flip_x, zn->flip_z, mx, my);
            cnt = re15_map_rect_count((unsigned)pt[k].page);
            for (r = 0; r < cnt && r < RMAX; r++) { int ki = kachel_index(pt[k].page, r, mx, my);
                if (ki >= 0) printf(" r%d=%d", r, ki); }
            printf("\n");
        }
        /* Wandzeile y=114 beider Kacheln: wo ist sie in rect 7 UND rect 6 bemalt (Index 4)? */
        { int x, a = -1;
          printf("  Zeile y=114, Index 4 in rect 7 UND rect 6 zugleich: x =");
          for (x = 170; x <= 232; x++) {
              int beide = (kachel_index(3, 7, x, 114) == 4 && kachel_index(3, 6, x, 114) == 4);
              if (beide && a < 0) a = x;
              if (!beide && a >= 0) { printf(" %d..%d", a, x - 1); a = -1; }
          }
          printf("\n"); }
    }
    return 0;
}
