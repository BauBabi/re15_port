/* =============================================================================
 * Runde 39 Spur G "karten" — Mess-Sonde und Riegel
 * =============================================================================
 * Nutzer 2026-10-17 (analysis/befunde_runde39/AUFTRAG.md, woertlich):
 *   1. "Bei den Maps des Police Departement passt es noch nicht so ganz. [...]
 *      map3F_correct.png, map2F_correct.png, map1F_correct.png, mapB2_correct.png.
 *      Dort ist immer links, wie es bei uns aussieht und rechts wie es eigentlich
 *      aussehen sollte. Keine Doppelwaende, Tuer immer genau auf Wand platziert,
 *      treppen nicht in der Wand platziert usw. Und natuerlich muss der Spielermarker
 *      da entsprechend passen. Der Room B1 den habe ich nicht gemacht. [...] Die Karte
 *      von B1 ist noch ziemlicher schrott."
 *   2. "In ROOM 1090 ist Leons Positionsmarker der Map ausserhalb des aktuellen Raums."
 * Dossier: analysis/befunde_runde39/G_karten.md.
 *
 * Aufruf: probe_r39_karten <teil> ...
 *   ops <seite> <hexraum> <x> <z> [band]   Op-Liste des Kartenschirms (alle Blaetter
 *                                          aufgedeckt) - Messwerkzeug, kein Urteil
 *   marker <hexraum> <x> <z> <band> [y]    Kartenpfad wie im Spiel: Raum betreten, Karte oeffnen,
 *                                          gezeigtes Blatt, Zone, Marker + Kachel-Index darunter
 * ========================================================================== */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_inv_screen.h"
#include "re15_scd.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_esp.h"
#include "re15_msg.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", (name)); g_fail = 1; } \
                               else printf("  PASS: %s\n", (name)); } while (0)

static re15_rdt_t s_rdt;
static uint8_t *s_roh;

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    long sz;
    uint8_t *b;
    if (!f) return 0;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return 0; }
    b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return 0; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return 0; }
    fclose(f); *n = (size_t)sz; return b;
}

/* Dieselbe Kette wie test_map_raum_live.c / test_r36_karte.c betrete(). */
static int betrete(unsigned rid, int32_t px, int32_t pz, int band)
{
    char pfad[600];
    size_t n = 0;
    re15_actor_t *pl;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT",
             RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    if (s_roh) { free(s_roh); s_roh = 0; }
    s_roh = slurp(pfad, &n);
    if (!s_roh) return 0;
    if (re15_rdt_parse(s_roh, n, &s_rdt) < 0) return 0;
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(1);
    re15_esp_fx_reset();
    g_current_room_id = rid;
    g_room_change.pending = 0;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz;
    re15_collision_reset_band();
    if (band >= 0) re15_collision_set_band(band);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

static re15_inv_op_t s_ops[RE15_INV_MAX_OPS];

/* Kartenschirm fuer <seite> bauen (alle Blaetter aufgedeckt, wie RE15_MAP_SHOT_SWEEP). */
static int baue_seite(int seite)
{
    int p;
    for (p = 0; p < 13; p++) re15_map_debug_reveal_page((unsigned)p);
    re15_inv_screen_open();
    g_inv_screen.substate = 1;
    g_inv_screen.item_state = 1;
    g_inv_screen.map_page = (uint8_t)seite;
    return re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
}

static int teil_ops(int argc, char **argv)
{
    int seite, n, i, band = -1;
    unsigned rid;
    if (argc < 6) { printf("FAIL: ops <seite> <hexraum> <x> <z> [band]\n"); return 2; }
    seite = atoi(argv[2]);
    rid = (unsigned)strtoul(argv[3], NULL, 16);
    if (argc > 6) band = atoi(argv[6]);
    if (!betrete(rid, atoi(argv[4]), atoi(argv[5]), band)) { printf("FAIL: Raum\n"); return 1; }
    re15_map_visited_mark(rid);
    re15_map_zone_update(rid, atoi(argv[4]), atoi(argv[5]));
    n = baue_seite(seite);
    printf("# ops %d seite %d\n", n, seite);
    for (i = 0; i < n; i++) {
        const re15_inv_op_t *o = &s_ops[i];
        printf("op %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", i, o->kind, o->page, o->clut,
               o->abe, o->x, o->y, o->w, o->h, o->u, o->v, o->r, o->g, o->b);
    }
    return 0;
}

/* ---- Kachel der Seite: DATA/MAP0<p+1>.PIX, 256x256 4bpp, unteres Nibble = linkes Pixel ---- */
static uint8_t s_pix[256 * 256];
static int s_pix_seite = -1;
static int pix_laden(int seite)
{
    char pfad[600];
    size_t n = 0;
    uint8_t *roh;
    int y, x;
    if (s_pix_seite == seite) return 1;
    snprintf(pfad, sizeof pfad, "%s/DATA/MAP%02X.PIX", RE15_ASSET_PSX_DIR, seite + 1);
    roh = slurp(pfad, &n);
    if (!roh || n < 256 * 128) { free(roh); return 0; }
    for (y = 0; y < 256; y++)
        for (x = 0; x < 128; x++) {
            s_pix[y * 256 + 2 * x]     = roh[y * 128 + x] & 0xF;
            s_pix[y * 256 + 2 * x + 1] = roh[y * 128 + x] >> 4;
        }
    free(roh);
    s_pix_seite = seite;
    return 1;
}
/* Kachel-Index des Rechtecks am Bildpunkt; -1 = ausserhalb. */
static int texel(int seite, int rect, int x, int y)
{
    int rx, ry, rw, rh, u, v;
    if (!pix_laden(seite)) return -1;
    if (!re15_map_rect_geometry((unsigned)seite, (unsigned)rect, &rx, &ry, &rw, &rh)) return -1;
    if (!re15_map_rect_uv((unsigned)seite, (unsigned)rect, &u, &v)) return -1;
    if (x < rx || x >= rx + rw || y < ry || y >= ry + rh) return -1;
    return s_pix[(v + y - ry) * 256 + (u + x - rx)];
}

typedef struct { int ok, page, zpage, zrect, zid, etage; int16_t mx, my; int tex; } marker_t;

/* Der echte Kartenpfad (menu_common.c map_entry): Zone aus der Lage, Seiten-Setzer der Stage,
 * gezeigtes Blatt, dann re15_inv_map_marker - dieselbe Funktion, die der Schirm zeichnet. */
static void miss_marker(unsigned rid, int32_t x, int32_t y, int32_t z, marker_t *m)
{
    const re15_map_zone_t *zn;
    memset(m, 0, sizeof *m);
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = x;
    g_actors[RE15_ACTOR_SLOT_PLAYER].y = y;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = z;
    re15_map_zone_update(rid, x, z);
    zn = re15_map_zone_current();
    re15_inv_screen_open();
    g_inv_screen.substate = 1;
    g_inv_screen.item_state = 1;
    re15_inv_map_stage_init((int)((rid >> 12) & 0xfu) - 1, (int)((rid >> 4) & 0xffu));
    g_inv_screen.map_page = re15_inv_map_page_shown();
    m->page = g_inv_screen.map_page;
    m->mx = m->my = -1;
    if (zn) {
        int fp, fr;
        m->ok = 1; m->zpage = zn->page; m->zrect = zn->rect; m->zid = zn->zid; m->etage = zn->etage;
        if (re15_map_floor_lookup(zn->room, zn->idx, re15_map_player_band(), &fp, &fr)) {
            m->zpage = fp; m->zrect = fr;
        }
    }
    re15_inv_map_marker(x, z, 0, &m->mx, &m->my);
    m->tex = (m->zrect != 255) ? texel(m->page, m->zrect, m->mx, m->my) : -2;
}

static int teil_marker(int argc, char **argv)
{
    marker_t m;
    unsigned rid;
    int band, y = 0;
    if (argc < 6) { printf("FAIL: marker <hexraum> <x> <z> <band> [y]\n"); return 2; }
    rid = (unsigned)strtoul(argv[2], NULL, 16);
    band = atoi(argv[5]);
    if (argc > 6) y = atoi(argv[6]);
    if (!betrete(rid, atoi(argv[3]), atoi(argv[4]), band)) { printf("FAIL: Raum\n"); return 1; }
    miss_marker(rid, atoi(argv[3]), y, atoi(argv[4]), &m);
    printf("ROOM%04X (%d,%d,%d) Band %d (Spieler-Band %d): Blatt %d, Zone Blatt %d rect %d zid %d "
           "etage %d, Marker (%d,%d), Kachel-Index am Marker %d\n", rid, atoi(argv[3]), y,
           atoi(argv[4]), band, re15_map_player_band(), m.page, m.zpage, m.zrect, m.zid, m.etage,
           m.mx, m.my, m.tex);
    return 0;
}

/* ---- Begehbare Flaeche per ECHTEM Kollisionsloeser -------------------------------------
 * re15_collision_constrain_contact_band (re15_collision.c: Port von FUN_8003b0a4 mit
 * push_rect / push_circle / push_diag2..7): ein Rasterschritt von P nach Q gilt als begehbar,
 * wenn der Loeser Q UNVERAENDERT laesst (Band <band>, Radius <r>, Maske 1 wie der Spieler).
 * Breitensuche ab der Saat, Gitter = SCA-Huelle +- 3000 Einheiten. Messwerkzeug: dieselbe
 * Kollision, an der der Spieler im Spiel haengen bleibt - keine Nachbildung der Zellformen. */
#define FL_MAX 900
static unsigned char s_fl[FL_MAX][FL_MAX];
static int32_t s_flx0, s_flz0;
static int s_flnx, s_flnz, s_flst;
static int s_qi[FL_MAX * FL_MAX];

static int fl_frei(int band, int32_t r, int32_t px, int32_t pz, int32_t qx, int32_t qz)
{
    int32_t x = qx, z = qz;
    re15_collision_constrain_contact_band(&s_rdt, px, pz, &x, &z, r, band, 1u, NULL, NULL);
    return x == qx && z == qz;
}

static int flut(int band, int32_t sx, int32_t sz, int32_t r, int schritt)
{
    int i, kopf = 0, fuss = 0, n = 0;
    int32_t hx0 = 0x7fffffff, hx1 = -0x7fffffff, hz0 = 0x7fffffff, hz1 = -0x7fffffff;
    for (i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        if (e->x < hx0) hx0 = e->x;
        if (e->z < hz0) hz0 = e->z;
        if (e->x + (int32_t)e->width > hx1) hx1 = e->x + (int32_t)e->width;
        if (e->z + (int32_t)e->density > hz1) hz1 = e->z + (int32_t)e->density;
    }
    if (hx0 > hx1) return 0;
    s_flst = schritt;
    s_flx0 = hx0 - 3000; s_flz0 = hz0 - 3000;
    s_flnx = (hx1 + 3000 - s_flx0) / schritt + 1;
    s_flnz = (hz1 + 3000 - s_flz0) / schritt + 1;
    if (s_flnx > FL_MAX || s_flnz > FL_MAX) return 0;
    memset(s_fl, 0, sizeof s_fl);
    {
        int gx = (sx - s_flx0 + schritt / 2) / schritt, gz = (sz - s_flz0 + schritt / 2) / schritt;
        int32_t wx = s_flx0 + gx * schritt, wz = s_flz0 + gz * schritt;
        if (!fl_frei(band, r, wx, wz, wx, wz)) return 0;
        s_fl[gz][gx] = 1; s_qi[fuss++] = gz * FL_MAX + gx; n = 1;
    }
    while (kopf < fuss) {
        static const int dx[4] = { 1, -1, 0, 0 }, dz[4] = { 0, 0, 1, -1 };
        int c = s_qi[kopf++], gz = c / FL_MAX, gx = c % FL_MAX, k;
        for (k = 0; k < 4; k++) {
            int ax = gx + dx[k], az = gz + dz[k];
            if (ax < 0 || az < 0 || ax >= s_flnx || az >= s_flnz || s_fl[az][ax]) continue;
            if (!fl_frei(band, r, s_flx0 + gx * schritt, s_flz0 + gz * schritt,
                         s_flx0 + ax * schritt, s_flz0 + az * schritt)) continue;
            s_fl[az][ax] = 1; s_qi[fuss++] = az * FL_MAX + ax; n++;
        }
    }
    return n;
}

static void flut_bbox(int32_t *x0, int32_t *x1, int32_t *z0, int32_t *z1, int *leck)
{
    int gx, gz;
    *x0 = *z0 = 0x7fffffff; *x1 = *z1 = -0x7fffffff; *leck = 0;
    for (gz = 0; gz < s_flnz; gz++)
        for (gx = 0; gx < s_flnx; gx++) {
            int32_t wx, wz;
            if (!s_fl[gz][gx]) continue;
            wx = s_flx0 + gx * s_flst; wz = s_flz0 + gz * s_flst;
            if (wx < *x0) *x0 = wx;
            if (wx > *x1) *x1 = wx;
            if (wz < *z0) *z0 = wz;
            if (wz > *z1) *z1 = wz;
            if (gx == 0 || gz == 0 || gx == s_flnx - 1 || gz == s_flnz - 1) *leck = 1;
        }
}

/* flut <hexraum> <band> <x> <z> <r> [schritt] [bild.pgm]: Bbox + Zeilen-Laeufe (von +z nach -z). */
static int teil_flut(int argc, char **argv)
{
    unsigned rid; int band, r, st = 100, n, gz, leck;
    int32_t x0, x1, z0, z1;
    char alt[600] = "", neu[600];
    if (argc < 7) { printf("FAIL: flut <hexraum> <band> <x> <z> <r> [schritt] [bild.pgm]\n"); return 2; }
    rid = (unsigned)strtoul(argv[2], NULL, 16); band = atoi(argv[3]); r = atoi(argv[6]);
    if (argc > 7) st = atoi(argv[7]);
    if (!betrete(rid, atoi(argv[4]), atoi(argv[5]), band)) { printf("FAIL: Raum\n"); return 1; }
    n = flut(band, atoi(argv[4]), atoi(argv[5]), r, st);
    if (n <= 0) { printf("FAIL: Saat (%s,%s) Band %d r %d nicht begehbar\n", argv[4], argv[5], band, r); return 1; }
    flut_bbox(&x0, &x1, &z0, &z1, &leck);
    if (argc > 8) {   /* Rasterbild (PGM, Zeile 0 = kleinstes z), Kopf traegt Ursprung + Schritt */
        FILE *f = fopen(argv[8], "wb");
        if (f) {
            fprintf(f, "P5\n# x0 %d z0 %d schritt %d\n%d %d\n255\n", s_flx0, s_flz0, st, s_flnx, s_flnz);
            for (gz = 0; gz < s_flnz; gz++) {
                int gx;
                for (gx = 0; gx < s_flnx; gx++) fputc(s_fl[gz][gx] ? 255 : 0, f);
            }
            fclose(f);
        }
    }
    printf("ROOM%04X Band %d r %d Schritt %d: %d Punkte, Mitten x %d..%d z %d..%d%s\n", rid, band, r, st, n,
           x0, x1, z0, z1, leck ? " LECKT" : "");
    for (gz = s_flnz - 1; gz >= 0; gz--) {
        int gx, lauf = -1, len = 0;
        neu[0] = 0;
        for (gx = 0; gx <= s_flnx; gx++) {
            int an = gx < s_flnx && s_fl[gz][gx];
            if (an && lauf < 0) lauf = gx;
            if (!an && lauf >= 0) {
                len += snprintf(neu + len, sizeof neu - (size_t)len, " [%d..%d]",
                                s_flx0 + lauf * st, s_flx0 + (gx - 1) * st);
                if (len >= (int)sizeof neu - 32) break;
                lauf = -1;
            }
        }
        if (strcmp(neu, alt)) {
            if (neu[0]) printf("   ab z %6d:%s\n", s_flz0 + gz * st, neu);
            strcpy(alt, neu);
        }
    }
    return 0;
}

/* Liegt der Kartenpunkt auf der GEZEICHNETEN Flaeche der Zone? Gemalte Kachel: Index != 0 im
 * eigenen Rechteck (Wand 4 zaehlt mit - der Marker darf auf der Wand stehen, wenn der Spieler
 * an ihr klebt); Schema-Zeichnung: in einer ihrer Zellen. */
static int auf_zone(const marker_t *m, unsigned rid, int32_t x, int32_t z)
{
    const re15_map_zone_t *zn = re15_map_zone_at(rid, x, z);
    int fp, fr;
    if (!zn) return 0;
    if (re15_map_floor_lookup(zn->room, zn->idx, re15_map_player_band(), &fp, &fr)) {
        const re15_map_zone_t *zg = re15_map_zone_fuer(zn->room, zn->idx, (unsigned)fp);
        if (zg) zn = zg;
    }
    if (zn->rect != 255) return m->tex > 0;
    {
        int sx, sy, sw, sh, erste, nz, k, cx, cy, cw, ch;
        if (!re15_map_zone_synth(zn, &sx, &sy, &sw, &sh, &erste, &nz)) return 0;
        for (k = 0; k < nz; k++) {
            if (!re15_map_synth_cell(erste + k, &cx, &cy, &cw, &ch)) continue;
            if (m->mx >= cx && m->mx < cx + cw && m->my >= cy && m->my < cy + ch) return 1;
        }
    }
    return 0;
}

/* markerflaeche <hexraum> <band> <x> <z> <y> [schritt] [dichte]: Flut mit dem Spieler-Radius
 * (PR 450, re15_collision.c) ab der Saat, an jedem <dichte>-ten Rasterpunkt der echte
 * Kartenpfad (miss_marker) - Zahl der Punkte, deren Marker NICHT auf der Zeichnung der eigenen
 * Zone liegt, mit Beispielen. */
static int markerflaeche(unsigned rid, int band, int32_t sx, int32_t sz, int32_t y, int st, int dichte,
                         int laut, int *geprueft)
{
    int n, gx, gz, gepr = 0, aus = 0, gez = 0;
    int mx0 = 999, mx1 = -999, my0 = 999, my1 = -999;
    if (!betrete(rid, sx, sz, band)) { printf("FAIL: Raum %04X\n", rid); return -1; }
    n = flut(band, sx, sz, 450, st);
    if (n <= 0) { printf("FAIL: Saat (%d,%d) ROOM%04X Band %d nicht begehbar\n", sx, sz, rid, band); return -1; }
    for (gz = 0; gz < s_flnz; gz += dichte)
        for (gx = 0; gx < s_flnx; gx += dichte) {
            marker_t m;
            int32_t wx, wz;
            if (!s_fl[gz][gx]) continue;
            wx = s_flx0 + gx * st; wz = s_flz0 + gz * st;
            miss_marker(rid, wx, y, wz, &m);
            gepr++;
            if (m.mx < mx0) mx0 = m.mx;
            if (m.mx > mx1) mx1 = m.mx;
            if (m.my < my0) my0 = m.my;
            if (m.my > my1) my1 = m.my;
            if (!auf_zone(&m, rid, wx, wz)) {
                aus++;
                if (laut && gez < 12) {
                    printf("   AUSSERHALB: Welt (%d,%d) -> Blatt %d rect %d Marker (%d,%d) Index %d\n", wx, wz,
                           m.page, m.zrect, m.mx, m.my, m.tex);
                    gez++;
                }
            }
        }
    printf("ROOM%04X Band %d Saat (%d,%d) y %d: %d begehbare Rasterpunkte, %d geprueft, Marker x %d..%d y %d..%d, "
           "ausserhalb der Zeichnung %d\n", rid, band, sx, sz, y, n, gepr, mx0, mx1, my0, my1, aus);
    if (geprueft) *geprueft = gepr;
    return aus;
}

static int teil_markerflaeche(int argc, char **argv)
{
    int st = 100, dichte = 3;
    if (argc < 7) { printf("FAIL: markerflaeche <hexraum> <band> <x> <z> <y> [schritt] [dichte]\n"); return 2; }
    if (argc > 7) st = atoi(argv[7]);
    if (argc > 8) dichte = atoi(argv[8]);
    return markerflaeche((unsigned)strtoul(argv[2], NULL, 16), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]),
                         atoi(argv[6]), st, dichte, 1, NULL) < 0 ? 1 : 0;
}

/* weg <hexraum> <band> <y> <x0> <z0> <x1> <z1> <n>: der Spieler laeuft geradlinig von (x0,z0) nach (x1,z1) in n
 * Schritten (Zone je Schritt per re15_map_zone_update wie game_step_common), je Schritt der echte Kartenpfad.
 * Ausgabe je Schritt: Welt, Blatt, Zone (idx), Marker. Misst "wie man sich bewegt": Pixel je Schritt, Spruenge. */
static int teil_weg(int argc, char **argv)
{
    unsigned rid; int band, n, k;
    int32_t y, x0, z0, x1, z1;
    if (argc < 10) { printf("FAIL: weg <hexraum> <band> <y> <x0> <z0> <x1> <z1> <n>\n"); return 2; }
    rid = (unsigned)strtoul(argv[2], NULL, 16); band = atoi(argv[3]); y = atoi(argv[4]);
    x0 = atoi(argv[5]); z0 = atoi(argv[6]); x1 = atoi(argv[7]); z1 = atoi(argv[8]); n = atoi(argv[9]);
    if (n < 1) n = 1;
    if (!betrete(rid, x0, z0, band)) { printf("FAIL: Raum\n"); return 1; }
    for (k = 0; k <= n; k++) {
        marker_t m;
        int32_t wx = x0 + (int32_t)(((int64_t)(x1 - x0) * k) / n);
        int32_t wz = z0 + (int32_t)(((int64_t)(z1 - z0) * k) / n);
        const re15_map_zone_t *zn;
        miss_marker(rid, wx, y, wz, &m);
        zn = re15_map_zone_current();
        printf("weg %d %d %d %d %d %d %d %d\n", k, wx, wz, m.page, zn ? zn->idx : -1, zn ? zn->zid : -1, m.mx, m.my);
    }
    return 0;
}

/* =====================================================================================================
 * RIEGEL (Runde 39 Spur G)
 * ===================================================================================================== */
#include "r39_karte_b1.h"
#include "r39_karten_soll.inc"

/* ---- Punkt 2: ROOM1090 - der Marker steht in jeder begehbaren Lage auf der Zeichnung der Ebene ---------------
 * Vorher (Stand 10876ebb^): obere Ebene 160 von 1225 Rasterpunkten neben dem gemalten Gamma (Dossier P2). */
static int teil_pruef_1090(void)
{
    static const unsigned R[2] = { 0x1090, 0x1091 };
    char t[200];
    int r, gepr;
    marker_t m;
    printf("=== Punkt 2: ROOM1090 Marker im Raum (Leon + Elza, beide Ebenen) ===\n");
    for (r = 0; r < 2; r++) {
        int aus = markerflaeche(R[r], 5, -13600, 1300, -9000, 100, 3, 0, &gepr);
        snprintf(t, sizeof t, "ROOM%04X obere Ebene (Band 5, Ankunft aus 10F0): %d von %d Rasterpunkten neben dem Gamma",
                 R[r], aus, gepr);
        CHECK(t, aus == 0 && gepr > 1000);
        aus = markerflaeche(R[r], 1, -10000, 6000, -1800, 100, 3, 0, &gepr);
        snprintf(t, sizeof t, "ROOM%04X Hof (Band 1): %d von %d Rasterpunkten neben der Kachel", R[r], aus, gepr);
        CHECK(t, aus == 0 && gepr > 500);
    }
    /* der Punkt des Nutzers (befund.log F1126..F1351, R1090 pos=(-12528,-9000,959)) */
    if (betrete(0x1090, -12528, 959, 5)) {
        miss_marker(0x1090, -12528, -9000, 959, &m);
        snprintf(t, sizeof t, "Nutzerpunkt (-12528,-9000,959): Blatt %d rect %d, Marker (%d,%d) auf Index %d (vorher (187,84) "
                 "Index 0)", m.page, m.zrect, m.mx, m.my, m.tex);
        CHECK(t, m.page == 3 && m.zrect == 7 && m.tex > 0);
    } else CHECK("ROOM1090 geladen", 0);
    /* die Tuer -> ROOM1050 (ROOM1090.RDT slot 0, Mitte (-10080,3060)) auf ihrem gemalten Tuerblatt / der Marke (191,69) */
    if (betrete(0x1090, -10080, 4200, 1)) {
        miss_marker(0x1090, -10080, -1800, 3060, &m);
        snprintf(t, sizeof t, "Hof: Tuer-Mitte -> ROOM1050 projiziert (%d,%d), Marke (191,69): <= 2 px", m.mx, m.my);
        CHECK(t, abs(m.mx - 191) <= 2 && abs(m.my - 69) <= 2);
    }
    return g_fail;
}

/* ---- Punkt 1, B1: die Schema-Zeichnung passt zu dem, wie man sich bewegt ------------------------------------ */
typedef struct { const char *was; unsigned raum; int band; int32_t x, z, y; int synth; } b1_ort_t;
static const b1_ort_t k_b1_orte[] = {
    { "Gang 1180", 0x1180, 0, -358, -6536, 0, R39B1_GANG },     { "Gang 1230", 0x1230, 0, -358, -6536, 0, R39B1_GANG },
    { "Gang 1181 (Elza)", 0x1181, 0, -358, -6536, 0, R39B1_GANG },
    { "1190 Halle", 0x1190, 0, 3174, -11182, 0, R39B1_HALLE }, { "1190 Ostkammer", 0x1190, 0, 19515, -12109, 0, R39B1_OST },
    { "1191 Ostkammer (Elza)", 0x1191, 0, 19515, -12109, 0, R39B1_OST },
    { "11B0 West", 0x11B0, 0, -26483, -18094, 0, R39B1_11B0 }, { "11B0 Werkstatt", 0x11B0, 0, -10000, -20000, 0, R39B1_11B0 },
    { "11C0 Ring", 0x11C0, 0, -25279, 16000, 0, R39B1_11C0 },  { "11C1 Ring (Elza)", 0x11C1, 0, -25279, 16000, 0, R39B1_11C0 },
    { "11D0 Zwinger", 0x11D0, 0, -300, -17400, 0, R39B1_11D0 }, { "1160 Zwinger ohne Strom", 0x1160, 0, -300, -17400, 0, R39B1_11D0 },
    { "11D0 Leiter-Kammer", 0x11D0, 0, 3900, -22000, 0, R39B1_KAMMER },
    { "10A0 B1-Absatz", 0x10A0, 4, 21200, 25500, -7200, R39B1_10A0 },
};

static int teil_pruef_b1(void)
{
    char t[260];
    int i, gepr;
    printf("=== Punkt 1, B1: Schema-Zeichnung aus Kollision + Tueren ===\n");
    /* (a) in JEDER begehbaren Lage steht der Marker auf der Zeichnung des eigenen Orts */
    for (i = 0; i < (int)(sizeof k_b1_orte / sizeof k_b1_orte[0]); i++) {
        const b1_ort_t *o = &k_b1_orte[i];
        int aus = markerflaeche(o->raum, o->band, o->x, o->z, o->y, 100, 3, 0, &gepr);
        const re15_map_zone_t *zn = re15_map_zone_current();
        snprintf(t, sizeof t, "%s: %d von %d begehbaren Rasterpunkten neben der Zeichnung, Zone synth %d (soll %d)",
                 o->was, aus, gepr, zn ? zn->synth : -1, o->synth);
        CHECK(t, aus == 0 && gepr > 30);   /* kleinster Ort: die 11D0-Leiterkammer, 44 Punkte */
    }
    /* (b) jede B1-Tuermarke liegt auf der GEMEINSAMEN Wandlinie beider Zeichnungen (Rand beider, Innen keiner);
     *     Treppen im Innenraum des Absatzes; keine Marke mehr auf einer gemalten Kachel von Blatt 0 */
    {
        int n = re15_map_mark_count(), k, tueren = 0, treppen = 0, kachel = 0;
        for (k = 0; k < n; k++) {
            int pg, rc, mx, my, kind, za, zb, d, ok = 1, sa = 0, sb = 0, j;
            re15_map_mark_get(k, &pg, &rc, &mx, &my, &kind);
            if (pg != 0) continue;
            if (rc != 255) { kachel++; continue; }
            re15_map_mark_zonen(k, &za, &zb);
            for (j = 0; j < re15_map_zone_count(); j++) {
                const re15_map_zone_t *z = re15_map_zone_by_index(j);
                if (z && z->page == 0 && z->zid == za) sa = z->synth;
                if (z && z->page == 0 && z->zid == zb) sb = z->synth;
            }
            if (kind >= 4) {
                treppen++;
                for (d = -2; d <= 2; d++) {
                    int r2;
                    for (r2 = -2; r2 <= 2; r2 += 2)
                        if (!r39b1_in(sa, kind == 4 ? mx + d : mx + r2, kind == 4 ? my + r2 : my + d, 1)) ok = 0;
                }
                snprintf(t, sizeof t, "Treppe (%d,%d) kind %d: alle Sprossen im Innenraum der Zeichnung synth %d", mx, my,
                         kind, sa);
                CHECK(t, ok && sa == R39B1_10A0);
                continue;
            }
            tueren++;
            for (d = -2; d <= 2; d++) {
                int x = (kind == 0 || kind == 2) ? mx + d : mx, y = (kind == 0 || kind == 2) ? my : my + d;
                if (!r39b1_in(sa, x, y, 0) || r39b1_in(sa, x, y, 1)) ok = 0;
                if (zb != 255 && (!r39b1_in(sb, x, y, 0) || r39b1_in(sb, x, y, 1))) ok = 0;
            }
            snprintf(t, sizeof t, "Tuer (%d,%d) zid %d/%d: Balken auf der Wandlinie %s", mx, my, za, zb,
                     zb != 255 ? "BEIDER Zeichnungen (eine Linie, keine Doppelwand)" : "seiner Zeichnung");
            CHECK(t, ok);
        }
        snprintf(t, sizeof t, "B1: %d Tueren, %d Treppen, %d Marken auf alten Kacheln (soll 6/2/0)", tueren, treppen, kachel);
        CHECK(t, tueren == 6 && treppen == 2 && kachel == 0);
    }
    /* (c) WIE MAN SICH BEWEGT: geradlinig durch jeden Gang-Abschnitt - gleichmaessig 1 px je 449 Einheiten (Affine 146,
     *     RE2 FUN_8006e120 /450), kein Querversatz, keine Spruenge, EINE Zone. Vorher (5 gestreckte Abschnitte): s. Dossier. */
    {
        static const struct { const char *was; int32_t x0, z0, x1, z1; } W[] = {
            { "Suedbein nach Norden", -4142, -15900, -4142, -7800 }, { "Versatz nach Osten", -5100, -5777, 5000, -5777 },
            { "Laengsgang nach Norden", 3858, -3800, 3858, 30500 },   { "Querarm nach Westen", 5000, 13723, -9000, 13723 },
            { "Westbein nach Sueden", -7942, 13700, -7942, 9950 },
        };
        int w;
        if (!betrete(0x1180, -358, -6536, 0)) { CHECK("ROOM1180 geladen", 0); return g_fail; }
        for (w = 0; w < (int)(sizeof W / sizeof W[0]); w++) {
            int k, n = (int)((abs(W[w].x1 - W[w].x0) + abs(W[w].z1 - W[w].z0)) / 100), maxs = 0, qmin = 999, qmax = -999,
                zone0 = -1, zwechsel = 0, l0 = 0, l1 = 0;
            int16_t px = 0, py = 0;
            for (k = 0; k <= n; k++) {
                marker_t m;
                int32_t wx = W[w].x0 + (W[w].x1 - W[w].x0) * k / n, wz = W[w].z0 + (W[w].z1 - W[w].z0) * k / n;
                const re15_map_zone_t *zn;
                int laengs, quer;
                miss_marker(0x1180, wx, 0, wz, &m);
                zn = re15_map_zone_current();
                if (zone0 < 0) zone0 = zn ? zn->idx : -2; else if (!zn || zn->idx != zone0) zwechsel++;
                if (k && abs(m.mx - px) + abs(m.my - py) > maxs) maxs = abs(m.mx - px) + abs(m.my - py);
                px = m.mx; py = m.my;
                if (W[w].z0 == W[w].z1) { laengs = m.mx; quer = m.my; } else { laengs = -m.my; quer = m.mx; }
                if (k == 0) l0 = laengs;
                l1 = laengs;
                if (quer < qmin) qmin = quer;
                if (quer > qmax) qmax = quer;
            }
            {
                int welt = abs(W[w].x1 - W[w].x0) + abs(W[w].z1 - W[w].z0);
                int pxw = abs(l1 - l0);
                double je1000 = 1000.0 * pxw / welt;
                snprintf(t, sizeof t, "Gang %s: %d Einheiten -> %d px (%.2f px je 1000, soll 1000*146/65536 = 2,23 +-0,15), "
                         "Querversatz %d px, groesster Schritt %d px, Zonenwechsel %d", W[w].was, welt, pxw, je1000,
                         qmax - qmin, maxs, zwechsel);
                CHECK(t, je1000 >= 2.08 && je1000 <= 2.38 && qmax - qmin <= 1 && maxs <= 2 && zwechsel == 0);
            }
        }
    }
    /* (d) Tueren: der Marker springt beim Durchgang hoechstens 3 px (Tuer-Mitte in A gegen Ankunft in B) */
    {
        static const struct { const char *was; unsigned ra; int32_t ax, az; int ba; unsigned rb; int32_t bx, bz; int bb; } T[] = {
            { "Gang -> 1190 Halle", 0x1180, -8118, 9355, 0, 0x1190, 3174, -11182, 0 },
            { "Gang -> 1190 Ostkammer", 0x1230, 96, 12161, 0, 0x1190, 19515, -12109, 0 },
            { "Gang -> 11B0", 0x1180, 5706, 28874, 0, 0x11B0, -26483, -18094, 0 },
            { "Gang -> 10A0 B1", 0x1180, -177, -7772, 0, 0x10A0, 21200, 25500, 4 },
            { "Gang -> 11D0", 0x1180, -4837, -16839, 0, 0x11D0, -300, -17400, 0 },
            { "11B0 -> 11C0", 0x11B0, -25480, -28190, 0, 0x11C0, -25279, 17268, 0 },
        };
        int k;
        for (k = 0; k < (int)(sizeof T / sizeof T[0]); k++) {
            marker_t a, b;
            memset(&a, 0, sizeof a); memset(&b, 0, sizeof b);
            if (betrete(T[k].ra, T[k].ax, T[k].az, T[k].ba)) miss_marker(T[k].ra, T[k].ax, T[k].ba == 4 ? -7200 : 0, T[k].az, &a);
            if (betrete(T[k].rb, T[k].bx, T[k].bz, T[k].bb)) miss_marker(T[k].rb, T[k].bx, T[k].bb == 4 ? -7200 : 0, T[k].bz, &b);
            snprintf(t, sizeof t, "Tuer %s: Marker (%d,%d) -> (%d,%d) auf Blatt %d/%d, Sprung %d px (<= 3)", T[k].was, a.mx,
                     a.my, b.mx, b.my, a.page, b.page, abs(a.mx - b.mx) + abs(a.my - b.my));
            CHECK(t, a.page == 0 && b.page == 0 && abs(a.mx - b.mx) + abs(a.my - b.my) <= 3);
        }
    }
    return g_fail;
}

/* ---- Punkt 1, 1F/2F/3F/B2: Abzug der echten exe gegen die Soll-Haelfte der Nutzer-Vorlage ---------------------
 * sollbild <bmp> <seite>: Klassenbild (Farben wie komp.py farbe_klasse, aktuell = Koerper) gegen r39_karten_soll.inc;
 * gezaehlt werden Pixel, an denen BEIDE eine Kartenklasse tragen und verschieden sind (Spieler-Ring = Sonstiges). */
static int klasse_rgb(int r, int g, int b)
{
    if (r == 0 && g == 16 && (b == 88 || b == 120)) return 0;
    if (r == 16 && g == 56 && (b == 40 || b == 56)) return 1;
    if (r == 176 && g == 176 && b == 176) return 2;
    if (r == 104 && g == 136 && b == 88) return 3;
    if (r == 224 && g == 168 && b == 40) return 4;
    if (r == 240 && g == 240 && b == 216) return 5;
    if (r == 48 && g == 8 && (b == 48 || b == 64)) return 1;   /* aktuell zaehlt als Koerper */
    return 7;
}

static int teil_sollbild(int argc, char **argv)
{
    FILE *f;
    unsigned char hdr[54], *px;
    int w, h, x, y, seite, i, k, diff = 0, gez = 0, gepr = 0;
    long off;
    const unsigned short *rle = NULL;
    int nrle = 0;
    static unsigned char soll[240][320];
    if (argc < 4) { printf("FAIL: sollbild <bmp> <seite>\n"); return 2; }
    seite = atoi(argv[3]);
    for (i = 0; i < (int)(sizeof k_soll / sizeof k_soll[0]); i++)
        if (k_soll[i].seite == seite) { rle = k_soll[i].rle; nrle = k_soll[i].n; }
    if (!rle) { printf("FAIL: kein Soll fuer Blatt %d\n", seite); return 2; }
    for (y = 0, x = 0, k = 0; k + 1 < nrle && y < 240; k += 2) {
        int c = rle[k], n = rle[k + 1];
        while (n-- > 0 && y < 240) { soll[y][x] = (unsigned char)c; if (++x == 320) { x = 0; y++; } }
    }
    f = fopen(argv[2], "rb");
    if (!f || fread(hdr, 1, 54, f) != 54) { printf("FAIL: %s nicht lesbar\n", argv[2]); if (f) fclose(f); return 1; }
    off = hdr[10] | (hdr[11] << 8) | (hdr[12] << 16) | ((long)hdr[13] << 24);
    w = hdr[18] | (hdr[19] << 8); h = hdr[22] | (hdr[23] << 8);
    if (w != 320 || h != 240) { printf("FAIL: %dx%d statt 320x240\n", w, h); fclose(f); return 1; }
    px = (unsigned char *)malloc((size_t)w * (size_t)h * 3u);
    fseek(f, off, SEEK_SET);
    for (y = 0; y < h; y++) {
        int stride = (w * 3 + 3) & ~3;
        unsigned char *zeile = px + (size_t)(h - 1 - y) * (size_t)w * 3u;
        if (fread(zeile, 1, (size_t)w * 3u, f) != (size_t)w * 3u) break;
        fseek(f, stride - w * 3, SEEK_CUR);
    }
    fclose(f);
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
            unsigned char *p = px + ((size_t)y * (size_t)w + (size_t)x) * 3u;   /* B,G,R */
            int ist = klasse_rgb(p[2], p[1], p[0]), s = soll[y][x];
            if (ist == 7 || s == 7) continue;
            gepr++;
            if (ist != s) {
                diff++;
                if (gez < 10) { printf("   Abweichung (%d,%d): ist %d soll %d\n", x, y, ist, s); gez++; }
            }
        }
    free(px);
    printf("Blatt %d (%s): %d Kartenpixel verglichen, %d Abweichungen gegen das Soll der Nutzer-Vorlage\n", seite, argv[2],
           gepr, diff);
    if (diff != 0 || gepr < 20000) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}

static int teil_pruef(int argc, char **argv)
{
    const char *was = argc > 2 ? argv[2] : "";
    g_fail = 0;
    if (!strcmp(was, "1090")) teil_pruef_1090();
    else if (!strcmp(was, "b1")) teil_pruef_b1();
    else { printf("FAIL: pruef 1090|b1\n"); return 2; }
    printf(g_fail ? "FEHLER\n" : "OK\n");
    return g_fail;
}

/* doppelwand <bmp>: "Keine Doppelwaende" (Nutzer 2026-10-17) - zaehlt zwei nebeneinander liegende Wandlinien zwischen
 * zwei Raumkoerpern (Koerper|Wand|Wand|Koerper waagerecht oder senkrecht). Die Soll-Haelften der vier Vorlagen haben 0,
 * unser Stand 1038dc15 hatte 1F 188 / B2 30. */
static int teil_doppelwand(int argc, char **argv)
{
    FILE *f;
    unsigned char hdr[54], *px;
    static unsigned char k[240][320];
    int w, h, x, y, n = 0;
    long off;
    if (argc < 3) { printf("FAIL: doppelwand <bmp>\n"); return 2; }
    f = fopen(argv[2], "rb");
    if (!f || fread(hdr, 1, 54, f) != 54) { printf("FAIL: %s nicht lesbar\n", argv[2]); if (f) fclose(f); return 1; }
    off = hdr[10] | (hdr[11] << 8) | (hdr[12] << 16) | ((long)hdr[13] << 24);
    w = hdr[18] | (hdr[19] << 8); h = hdr[22] | (hdr[23] << 8);
    if (w != 320 || h != 240) { printf("FAIL: %dx%d\n", w, h); fclose(f); return 1; }
    px = (unsigned char *)malloc((size_t)w * (size_t)h * 3u);
    fseek(f, off, SEEK_SET);
    for (y = 0; y < h; y++) {
        int stride = (w * 3 + 3) & ~3;
        if (fread(px + (size_t)(h - 1 - y) * (size_t)w * 3u, 1, (size_t)w * 3u, f) != (size_t)w * 3u) break;
        fseek(f, stride - w * 3, SEEK_CUR);
    }
    fclose(f);
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
            unsigned char *p = px + ((size_t)y * (size_t)w + (size_t)x) * 3u;
            k[y][x] = (unsigned char)klasse_rgb(p[2], p[1], p[0]);
        }
    free(px);
#define R39_KOERPER(c) ((c) == 1 || (c) == 3)
    for (y = 0; y < 240; y++)
        for (x = 1; x < 318; x++)
            if (k[y][x] == 2 && k[y][x + 1] == 2 && R39_KOERPER(k[y][x - 1]) && R39_KOERPER(k[y][x + 2])) {
                if (n < 8) printf("   Doppelwand waagerecht bei (%d,%d)\n", x, y);
                n++;
            }
    for (y = 1; y < 238; y++)
        for (x = 0; x < 320; x++)
            if (k[y][x] == 2 && k[y + 1][x] == 2 && R39_KOERPER(k[y - 1][x]) && R39_KOERPER(k[y + 2][x])) {
                if (n < 8) printf("   Doppelwand senkrecht bei (%d,%d)\n", x, y);
                n++;
            }
#undef R39_KOERPER
    printf("%s: %d Doppelwand-Pixel\n", argv[2], n);
    printf(n ? "FAIL\n" : "OK\n");
    return n ? 1 : 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "";
    if (!strcmp(teil, "ops")) return teil_ops(argc, argv);
    if (!strcmp(teil, "marker")) return teil_marker(argc, argv);
    if (!strcmp(teil, "flut")) return teil_flut(argc, argv);
    if (!strcmp(teil, "weg")) return teil_weg(argc, argv);
    if (!strcmp(teil, "pruef")) return teil_pruef(argc, argv);
    if (!strcmp(teil, "sollbild")) return teil_sollbild(argc, argv);
    if (!strcmp(teil, "doppelwand")) return teil_doppelwand(argc, argv);
    if (!strcmp(teil, "markerflaeche")) return teil_markerflaeche(argc, argv);
    printf("FAIL: unbekannter Teil '%s'\n", teil);
    (void)g_fail;
    return 2;
}
