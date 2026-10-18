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

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "";
    if (!strcmp(teil, "ops")) return teil_ops(argc, argv);
    if (!strcmp(teil, "marker")) return teil_marker(argc, argv);
    if (!strcmp(teil, "flut")) return teil_flut(argc, argv);
    if (!strcmp(teil, "weg")) return teil_weg(argc, argv);
    if (!strcmp(teil, "markerflaeche")) return teil_markerflaeche(argc, argv);
    printf("FAIL: unbekannter Teil '%s'\n", teil);
    (void)g_fail;
    return 2;
}
