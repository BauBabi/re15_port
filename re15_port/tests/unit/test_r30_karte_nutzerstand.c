/* Karte: DER STAND DES NUTZERS VOM 2026-09-27 - drei Marken und der Verlust beim Laden.
 *
 * NUTZER-BEFUND (Runde 30, Thema F, analysis/befunde_runde30/AUFTRAG.md):
 *   "3 Marker gesetzt: Roof ist irgendwie die Wand unten blau... 2F ist jetzt unten eine
 *    Tuer eingezeichnet auf der Karte die es nicht gibt.... und ROOM 1000 ist irgendwie
 *    jetzt blau eingezeichnet.... Ausserdem glaube ich, das wenn das spiel Gespeichert
 *    und dann geladen wird, Teile der Karte die ich bereits freigeschaltet habe verloren
 *    gegangen sind...."
 *
 * FIXTURE: seine Speicherkarte, UNVERAENDERT (probes/r30_karte_nutzer_2026-09-27.mcr,
 * bytegleich zu analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr).
 * Platz 2 (Block 3, Datei 0x06100): Version 8, ROOM1070 cut 2, pos (15392,0,6583),
 * 20 Besucht-Bits. Aus genau diesem Stand stammen seine drei Abzuege.
 *
 * GEPRUEFT WIRD DIE OP-LISTE DES ECHTEN ZEICHNERS (re15_inv_screen_build), nicht eine
 * Tabelle - Memory reai-v2-tabelle-vs-bild. Stand VOR der Korrektur (d98e9639, Sonde
 * probe_r30_karten-marken_rundlauf):
 *     Nutzer-Stand     2 Ops in RE2-Blau (16,64,176), 9 von 41 Marken ohne Traeger,
 *                      0 von 50 Etagen-Bits, 4 von 20 Orten ohne Zeichnung
 *     alles begangen  13 Ops in RE2-Blau, 10 von 186 Marken ohne Traeger
 *
 * Die Farben werden hier NICHT als Zahlen wiederholt, sondern aus denselben
 * Palettenwoertern dekodiert wie im Zeichner (RE15_KARTE_BESUCHT 0x81A4 = TEX.TIM
 * @0x0556, RE15_KARTE_AKTUELL 0x842D = ST0.TIM @0x109B6, RE15_KARTE_WAND 0x5AD6 =
 * TEX.TIM @0x055C); dass die Woerter in den Dateien stehen, prueft unit_karte_besitz.
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

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_R30_NUTZERKARTE
#define RE15_R30_NUTZERKARTE "probes/r30_karte_nutzer_2026-09-27.mcr"
#endif

#define NZ ((int)(sizeof s_map_zones  / sizeof s_map_zones[0]))
#define NM ((int)(sizeof s_map_marks  / sizeof s_map_marks[0]))
#define NF ((int)(sizeof s_map_floors / sizeof s_map_floors[0]))
#define NC ((int)(sizeof s_map_synth_cells / sizeof s_map_synth_cells[0]))
#define NW ((int)(sizeof s_map_walls  / sizeof s_map_walls[0]))
#define SEITEN 13
#define RMAX   32
#define OPMAX  1024

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

typedef struct { int r, g, b, stp; } farbe_t;
static farbe_t wort(uint16_t w)
{
    farbe_t f;
    f.r = (w & 31) << 3; f.g = ((w >> 5) & 31) << 3; f.b = ((w >> 10) & 31) << 3;
    f.stp = (w >> 15) & 1;
    return f;
}
static int op_hat(const re15_inv_op_t *o, farbe_t f)
{
    return o->r == f.r && o->g == f.g && o->b == f.b;
}

/* ---- Kartenkunst: DATA/MAPxx.PIX, headerlos 256x256 4bpp, unteres Nibble = links ---- */
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

/* Eine Haupt-Zonenzeile auf JEDEM ihrer Baender begehen. */
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

/* Ist der Schirmpunkt von einem GEZEICHNETEN Rechteck bemalt (Texel-Index != 0) oder
 * liegt er in einer gezeichneten Schema-Zelle? "Gezeichnet" wie im Zeichner
 * (re15_inv_screen.c, Kachel-Schleife): BESUCHT/AKTUELL immer, UNBESUCHT nur mit dem
 * Plan des Blatts, OHNE ZONE nie. */
static int punkt_getragen(int page, int sx, int sy)
{
    int r, cnt = re15_map_rect_count((unsigned)page), i;
    for (r = 0; r < cnt && r < RMAX; r++) {
        int st = re15_map_rect_state((unsigned)page, (unsigned)r);
        if (st == RE15_MAP_RECT_UNMAPPED) continue;
        if (st == RE15_MAP_RECT_UNVISITED && !re15_map_owned_page((unsigned)page)) continue;
        if (kachel_index(page, r, sx, sy) > 0) return 1;
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

/* Balken der Marke: kind 0/2 waagerecht 5x1, 1/3 senkrecht 1x5, Treppen 4/5 als 5x5;
 * dazu 1 Punkt Saum. Gemessen an den Nutzer-Abzuegen: (188,178..182) kind 3. */
static int marke_getragen(int page, int mx, int my, int kind)
{
    int hx = (kind == 1 || kind == 3) ? 0 : 2;
    int hy = (kind == 0 || kind == 2) ? 0 : 2;
    int x, y;
    for (y = my - hy - 1; y <= my + hy + 1; y++)
        for (x = mx - hx - 1; x <= mx + hx + 1; x++)
            if (punkt_getragen(page, x, y)) return 1;
    return 0;
}

static int bitzahl(const unsigned char *b, int n)
{
    int k, z = 0, j;
    for (k = 0; k < n; k++) for (j = 0; j < 8; j++) z += (b[k] >> j) & 1;
    return z;
}

static re15_inv_op_t g_ops[OPMAX];
static int blatt_bauen(int p)
{
    re15_inv_map_stage_init(0, 13);
    re15_inv_screen_open();
    g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
    g_inv_screen.map_page = (uint8_t)p;
    return re15_inv_screen_build(&g_inv_screen, g_ops, OPMAX);
}

/* Zahl der Ops in RE2s Besucht-Blau ueber alle 13 Blaetter. */
static int blau_zensus(void)
{
    farbe_t blau = wort(RE15_KARTE_BESUCHT_RE2);
    int p, k, n = 0;
    for (p = 0; p < SEITEN; p++) {
        int nops = blatt_bauen(p);
        for (k = 0; k < nops; k++)
            if (op_hat(&g_ops[k], blau)) {
                printf("     Blatt %2d Op %3d: kind %d (%d,%d) %dx%d abe %d in RE2-Blau\n",
                       p, k, g_ops[k].kind, g_ops[k].x, g_ops[k].y, g_ops[k].w, g_ops[k].h,
                       g_ops[k].abe);
                n++;
            }
    }
    return n;
}

/* Ist diese Op der Balken einer SICHTBAREN Tuermarke des Blattes? Geometrie wie im
 * Zeichner: Nord/Sued (kind 0/2) 5x1 ab (mx-2,my), Ost/West (kind 1/3) 1x5 ab (mx,my-2). */
static int ist_markenbalken(int page, const re15_inv_op_t *o)
{
    int i;
    for (i = 0; i < NM; i++) {
        int pg, rc, mx, my, kind, laengs_x;
        if (!re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind)) continue;
        if (pg != page || kind > 3) continue;
        laengs_x = (kind == 0 || kind == 2);
        if (o->x == (laengs_x ? mx - 2 : mx) && o->y == (laengs_x ? my : my - 2) &&
            o->w == (laengs_x ? 5 : 1) && o->h == (laengs_x ? 1 : 5)) return 1;
    }
    return 0;
}

/* Die Marken der Klasse C (karten-marken.md §2.8): ihr EIGENES Rechteck ist gezeichnet,
 * die Lage sitzt aber neben der Kunst. Ursache nicht untersucht, NICHT Teil der Runde 30.
 * ⛔ BENANNTE LISTE, KEINE ZAHL "<= 4": eine fuenfte faellt damit auf. Gefuehrt ueber
 * (Blatt, Lage), weil sich die Tabellen-Nummer bei jeder Streichung verschiebt. */
static const struct { int page, mx, my; } KLASSE_C[] = {
    { 6, 129, 153 }, { 6, 139, 153 }, { 7, 224, 175 }, { 9, 190, 91 },
};
static int ist_klasse_c(int page, int mx, int my)
{
    int k;
    for (k = 0; k < (int)(sizeof KLASSE_C / sizeof KLASSE_C[0]); k++)
        if (KLASSE_C[k].page == page && KLASSE_C[k].mx == mx && KLASSE_C[k].my == my) return 1;
    return 0;
}

/* Sichtbare Marken ohne Traeger. *unbekannt zaehlt die, die NICHT zur Klasse C gehoeren;
 * *klasse_c die der benannten Liste. */
static int marken_zensus(int *unbekannt, int *klasse_c)
{
    int i, sicht = 0;
    *unbekannt = 0; *klasse_c = 0;
    for (i = 0; i < NM; i++) {
        int pg, rc, mx, my, kind;
        if (!re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind)) continue;
        sicht++;
        if (marke_getragen(pg, mx, my, kind)) continue;
        if (ist_klasse_c(pg, mx, my)) { (*klasse_c)++; continue; }
        (*unbekannt)++;
        printf("     Marke %3d Blatt %2d rect %3d (%3d,%3d) kind %d zid %d: sichtbar, aber "
               "OHNE gezeichneten Traeger\n", i, pg, rc, mx, my, kind, s_map_marks[i].zid);
    }
    return sicht;
}

int main(void)
{
    char t[260];
    farbe_t besucht = wort(RE15_KARTE_BESUCHT);
    farbe_t aktuell = wort(RE15_KARTE_AKTUELL);
    farbe_t wand    = wort(RE15_KARTE_WAND);

    setvbuf(stdout, NULL, _IONBF, 0);
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_map_stock_set(0);
    blaetter_laden();

    printf("=== Karte: der Stand des Nutzers vom 2026-09-27 ===\n");
    printf("  Palette: besucht (%d,%d,%d) stp%d | aktuell (%d,%d,%d) stp%d | wand (%d,%d,%d) stp%d\n",
           besucht.r, besucht.g, besucht.b, besucht.stp, aktuell.r, aktuell.g, aktuell.b,
           aktuell.stp, wand.r, wand.g, wand.b, wand.stp);
    CHECK("ABDECKUNG: alle 13 Kartenblaetter gelesen",
          g_px_ok[0] && g_px_ok[2] && g_px_ok[3] && g_px_ok[5] && g_px_ok[12]);

    /* =================== A. DER STAND DES NUTZERS (Alt-Stand v8) =================== */
    {
        re15_savedata_t sd; uint16_t rr = 0; unsigned char bits[32], etage[16];
        int p, k, n;
        memset(&sd, 0, sizeof sd);
        re15_map_visited_reset();
        if (re15_memcard_load(RE15_R30_NUTZERKARTE, 2, &sd) != 0) {
            printf("  FAIL: Karte %s Platz 2 nicht lesbar\n", RE15_R30_NUTZERKARTE);
            return 1;
        }
        printf("  Platz 2: Version %u, Raum %04X, pos (%d,%d,%d), hp %d\n", (unsigned)sd.version,
               (unsigned)sd.room, (int)sd.player_x, (int)sd.player_y, (int)sd.player_z,
               (int)sd.player_hp);
        CHECK("A1 der v8-Stand wird beim Lesen auf die aktuelle Version gehoben",
              sd.version == RE15_SAVE_VERSION);
        CHECK("A1 Raum ROOM1070, pos (15392,0,6583), hp 85 - der Stand seiner Abzuege",
              sd.room == 0x1070 && sd.player_x == 15392 && sd.player_z == 6583 &&
              sd.player_hp == 85);
        snprintf(t, sizeof t, "A1 20 Besucht-Bits im Block - sind %d", bitzahl(sd.visited, 32));
        CHECK(t, bitzahl(sd.visited, 32) == 20);
        {   int leer = 1;
            for (k = 0; k < (int)sizeof sd.files; k++) if (sd.files[k] != 0xFF) leer = 0;
            CHECK("A1 die FILE-Liste eines Alt-Stands ist leer (24 x 0xFF)", leer); }

        CHECK("A2 restore nimmt den gehobenen Stand an", re15_savedata_restore(&sd, &rr) == 0);
        stehe(rr, g_actors[0].x, g_actors[0].z, re15_collision_band_from_y(g_actors[0].y));
        re15_map_visited_export(bits);
        re15_map_visited_floor_export(etage);
        CHECK("A2 die Zonen-Bits kommen bytegleich an", memcmp(bits, sd.visited, 32) == 0);
        printf("  Etagen-Bits nach dem Laden: %d\n", bitzahl(etage, 16));

        /* ---- Befund d: die vier Orte, die nach dem Laden fehlten ------------------ */
        CHECK("A3 ROOM1060 (Treppenhaus) ist auf 1F wieder gezeichnet (Blatt 2 rect 10)",
              re15_map_rect_state(2, 10) == RE15_MAP_RECT_VISITED);
        CHECK("A3 ROOM1090 ist auf 1F wieder gezeichnet (Blatt 2 rect 5)",
              re15_map_rect_state(2, 5) == RE15_MAP_RECT_VISITED);
        CHECK("A3 ROOM10A0 ist auf 1F wieder gezeichnet (Blatt 2 rect 6)",
              re15_map_rect_state(2, 6) == RE15_MAP_RECT_VISITED);
        CHECK("A3 ROOM1170/z1 ist auf ROOF wieder gezeichnet (Blatt 5 rect 0)",
              re15_map_rect_state(5, 0) == RE15_MAP_RECT_VISITED);
        /* Die Hebung zeigt KEINE Gast-Zeile: welche Etage begangen war, steht in keinem
         * v8-Block (Port-Entscheidung, Begruendung an re15_map_visited_floor_heben). */
        CHECK("A3 die Hebung zeigt KEINE Gast-Zeile: ROOM1060 auf 2F/3F bleibt verborgen",
              re15_map_rect_state(3, 1) == RE15_MAP_RECT_UNVISITED &&
              re15_map_rect_state(4, 1) == RE15_MAP_RECT_UNVISITED);
        CHECK("A3 ... ebenso ROOM1090 oben (3/7), ROOM10A0 auf B2 (1/9), ROOM1170/z1 auf 3F (4/3)",
              re15_map_rect_state(3, 7) == RE15_MAP_RECT_UNVISITED &&
              re15_map_rect_state(1, 9) == RE15_MAP_RECT_UNVISITED &&
              re15_map_rect_state(4, 3) == RE15_MAP_RECT_UNVISITED);
        snprintf(t, sizeof t, "A3 genau 4 Etagen-Bits gehoben (je Ort die Haupt-Zeile) - sind %d",
                 bitzahl(etage, 16));
        CHECK(t, bitzahl(etage, 16) == 4);

        /* ---- Befund a + c: kein RE2-Blau mehr -------------------------------------- */
        n = blau_zensus();
        snprintf(t, sizeof t, "A4 KEINE Op in RE2-Blau auf den 13 Blaettern (vorher 2) - sind %d", n);
        CHECK(t, n == 0);

        /* ---- Befund c: der Schema-Kasten von ROOM1000/z0 --------------------------- */
        {
            int nops = blatt_bauen(2), da = 0, ok = 0;
            for (k = 0; k < nops; k++) {
                if (g_ops[k].kind != RE15_INV_OP_FILL) continue;
                if (g_ops[k].x != s_map_synth_cells[0].x || g_ops[k].y != s_map_synth_cells[0].y ||
                    g_ops[k].w != s_map_synth_cells[0].w || g_ops[k].h != s_map_synth_cells[0].h)
                    continue;
                da++;
                printf("  Blatt 2 Op %d: FILL (%d,%d) %dx%d abe %d rgb (%d,%d,%d)\n", k, g_ops[k].x,
                       g_ops[k].y, g_ops[k].w, g_ops[k].h, g_ops[k].abe, g_ops[k].r, g_ops[k].g,
                       g_ops[k].b);
                if (op_hat(&g_ops[k], besucht) && g_ops[k].abe == 1) ok++;
            }
            CHECK("A5 ABDECKUNG: der Schema-Kasten (207,89) 15x33 wird gezeichnet", da == 1);
            CHECK("A5 er traegt RE1.5s Besucht-Farbe (TEX.TIM @0x0556) und ist halbtransparent",
                  ok == 1 && besucht.stp == 1);
        }

        /* ---- Befund a: die Dach-Wandzeile ------------------------------------------ */
        {
            int nops = blatt_bauen(5), auf_wand = 0;
            for (k = 0; k < nops; k++) {
                if (g_ops[k].kind != RE15_INV_OP_FILL) continue;
                if (g_ops[k].h == 1 && g_ops[k].y == 155 &&
                    g_ops[k].x <= 182 && g_ops[k].x + g_ops[k].w - 1 >= 148) {
                    auf_wand++;
                    printf("     Blatt 5 Op %d: FILL (%d,%d) %dx%d rgb (%d,%d,%d) auf y=155\n", k,
                           g_ops[k].x, g_ops[k].y, g_ops[k].w, g_ops[k].h, g_ops[k].r,
                           g_ops[k].g, g_ops[k].b);
                }
            }
            CHECK("A6 ROOF: KEINE gezeichnete Linie auf y=155 x 148..182 (die Kachel malt "
                  "die Suedwand selbst)", auf_wand == 0);
            CHECK("A6 ABDECKUNG: dort traegt die Kachel Index 4 (Wandlinie)",
                  kachel_index(5, 1, 148, 155) == 4 && kachel_index(5, 1, 182, 155) == 4);
        }

        /* ---- Befund b: die schwebende Tuermarke auf 2F ----------------------------- */
        {
            int nops = blatt_bauen(3), da = 0;
            for (k = 0; k < nops; k++) {
                if (g_ops[k].kind != RE15_INV_OP_FILL) continue;
                if (g_ops[k].w * g_ops[k].h > 25) continue;          /* nur Marken-Groesse */
                if (188 >= g_ops[k].x && 188 < g_ops[k].x + g_ops[k].w &&
                    180 >= g_ops[k].y && 180 < g_ops[k].y + g_ops[k].h) {
                    da++;
                    printf("     Blatt 3 Op %d: FILL (%d,%d) %dx%d ueberdeckt (188,180)\n", k,
                           g_ops[k].x, g_ops[k].y, g_ops[k].w, g_ops[k].h);
                }
            }
            CHECK("A7 2F: KEINE Marke bei (188,180)", da == 0);
        }

        /* ---- keine sichtbare Marke ohne Traeger ------------------------------------ */
        {
            int unb, kc, sicht = marken_zensus(&unb, &kc);
            printf("  sichtbare Marken %d, ohne Traeger %d (+ %d der Klasse C)\n", sicht, unb, kc);
            CHECK("A8 ABDECKUNG: es sind Marken sichtbar", sicht > 20);
            snprintf(t, sizeof t, "A8 KEINE sichtbare Marke ohne gezeichneten Traeger "
                     "(vorher 9 von 41) - sind %d", unb + kc);
            CHECK(t, unb == 0 && kc == 0);
        }
        (void)p;
    }

    /* =================== B. ALLES BEGANGEN, ALLE 13 BLAETTER ======================= */
    {
        int i, n, unb, kc, sicht;
        re15_map_visited_reset();
        for (i = 0; i < NZ; i++) {
            const re15_map_zone_t *zn = &s_map_zones[i];
            if (zn->etage || (zn->room & 1)) continue;
            begehe_zeile(i);
        }
        {   const re15_map_zone_t *s0 = NULL; int32_t px = 0, pz = 0; int q;
            for (q = 0; q < NZ; q++)
                if (s_map_zones[q].room == 0x1150 && !s_map_zones[q].etage) { s0 = &s_map_zones[q]; break; }
            if (s0 && punkt_in_zone(s0, &px, &pz)) stehe(0x1150, px, pz, 0); }

        n = blau_zensus();
        snprintf(t, sizeof t, "B1 alles begangen: KEINE Op in RE2-Blau (vorher 13) - sind %d", n);
        CHECK(t, n == 0);

        sicht = marken_zensus(&unb, &kc);
        printf("  sichtbare Marken %d von %d, ohne Traeger %d (+ %d der Klasse C)\n",
               sicht, NM, unb, kc);
        snprintf(t, sizeof t, "B2 alles begangen: JEDE Marke der Tabelle ist sichtbar - %d von %d",
                 sicht, NM);
        CHECK(t, sicht == NM);
        snprintf(t, sizeof t, "B2 KEINE Marke ohne Traeger ausser der benannten Klasse C "
                 "(vorher 10) - sind %d", unb);
        CHECK(t, unb == 0);
        snprintf(t, sizeof t, "B2 die Klasse C ist genau die benannte Liste (4) - sind %d", kc);
        CHECK(t, kc == 4);

        /* Schema-Fuellungen und Innenwaende tragen die dekodierten Palettenwerte. */
        {
            int p, k, c, fuell = 0, fuell_ok = 0, linie = 0, linie_ok = 0;
            for (p = 0; p < SEITEN; p++) {
                int nops = blatt_bauen(p);
                for (k = 0; k < nops; k++) {
                    int w;
                    if (g_ops[k].kind != RE15_INV_OP_FILL) continue;
                    for (c = 0; c < NC; c++)
                        if (g_ops[k].x == s_map_synth_cells[c].x &&
                            g_ops[k].y == s_map_synth_cells[c].y &&
                            g_ops[k].w == s_map_synth_cells[c].w &&
                            g_ops[k].h == s_map_synth_cells[c].h) {
                            fuell++;
                            if (op_hat(&g_ops[k], besucht) && g_ops[k].abe == 1) fuell_ok++;
                        }
                    /* Innenwand: eine Linie, die vollstaendig auf einer Zeile von
                     * s_map_walls dieses Blattes liegt. */
                    if (g_ops[k].w != 1 && g_ops[k].h != 1) continue;
                    for (w = 0; w < NW; w++) {
                        const re15_map_wall_t *mw = &s_map_walls[w];
                        int x0 = mw->x0 < mw->x1 ? mw->x0 : mw->x1;
                        int x1 = mw->x0 < mw->x1 ? mw->x1 : mw->x0;
                        int y0 = mw->y0 < mw->y1 ? mw->y0 : mw->y1;
                        int y1 = mw->y0 < mw->y1 ? mw->y1 : mw->y0;
                        if (mw->page != p) continue;
                        if (g_ops[k].x < x0 || g_ops[k].x + g_ops[k].w - 1 > x1) continue;
                        if (g_ops[k].y < y0 || g_ops[k].y + g_ops[k].h - 1 > y1) continue;
                        /* Ein Tuerbalken IN der Wand ist keine Wand - erkannt an der
                         * Marken-Tabelle (Balken 5x1 bzw. 1x5 um die Lage), nicht an
                         * einer Farbe. */
                        if (ist_markenbalken(p, &g_ops[k])) continue;
                        linie++;
                        if (op_hat(&g_ops[k], wand) && g_ops[k].abe == 0) linie_ok++;
                        else printf("     Blatt %2d Op %3d: Innenwand (%d,%d) %dx%d abe %d rgb "
                                    "(%d,%d,%d)\n", p, k, g_ops[k].x, g_ops[k].y, g_ops[k].w,
                                    g_ops[k].h, g_ops[k].abe, g_ops[k].r, g_ops[k].g, g_ops[k].b);
                        break;
                    }
                }
            }
            snprintf(t, sizeof t, "B3 ABDECKUNG: die drei Schema-Fuellungen werden gezeichnet - %d",
                     fuell);
            CHECK(t, fuell == NC);
            snprintf(t, sizeof t, "B3 jede traegt die Besucht-Farbe der Palette, halbtransparent "
                     "- %d von %d", fuell_ok, fuell);
            CHECK(t, fuell_ok == fuell);
            snprintf(t, sizeof t, "B4 ABDECKUNG: Innenwand-Linien werden gezeichnet - %d", linie);
            CHECK(t, linie >= NW);
            snprintf(t, sizeof t, "B4 jede traegt die Wandfarbe der Palette (TEX.TIM @0x055C), "
                     "deckend, zustandsfrei - %d von %d", linie_ok, linie);
            CHECK(t, linie_ok == linie);
        }
    }

    /* =================== C. DER SPIELER STEHT IN ROOM1000 ========================== */
    {
        int q, k, nops, da = 0, ok = 0;
        const re15_map_zone_t *z0 = NULL; int32_t px = 0, pz = 0;
        re15_map_visited_reset();
        for (q = 0; q < NZ; q++)
            if (s_map_zones[q].room == 0x1000 && s_map_zones[q].idx == 0 && !s_map_zones[q].etage) {
                z0 = &s_map_zones[q]; break; }
        if (z0 && punkt_in_zone(z0, &px, &pz)) stehe(0x1000, px, pz, 0);
        printf("  Spieler in ROOM1000/z0 bei (%d,%d), aktuelle Zone: %s\n", (int)px, (int)pz,
               (re15_map_zone_current() && re15_map_zone_current()->room == 0x1000 &&
                re15_map_zone_current()->idx == 0) ? "ROOM1000/z0" : "ANDERE");
        nops = blatt_bauen(2);
        for (k = 0; k < nops; k++) {
            if (g_ops[k].kind != RE15_INV_OP_FILL) continue;
            if (g_ops[k].x != s_map_synth_cells[0].x || g_ops[k].y != s_map_synth_cells[0].y ||
                g_ops[k].w != s_map_synth_cells[0].w || g_ops[k].h != s_map_synth_cells[0].h)
                continue;
            da++;
            printf("  Blatt 2 Op %d: FILL (%d,%d) %dx%d abe %d rgb (%d,%d,%d)\n", k, g_ops[k].x,
                   g_ops[k].y, g_ops[k].w, g_ops[k].h, g_ops[k].abe, g_ops[k].r, g_ops[k].g,
                   g_ops[k].b);
            if (op_hat(&g_ops[k], aktuell) && g_ops[k].abe == 1) ok++;
        }
        CHECK("C1 ABDECKUNG: der Ostraum von ROOM1000 wird als aktueller Raum gezeichnet", da == 1);
        CHECK("C1 aktueller Schema-Raum: RE2s Aktuell-Farbe (ST0.TIM @0x109B6), halbtransparent",
              ok == 1 && aktuell.stp == 1);
    }

    printf(g_fail ? "\nFEHLER\n" : "\nOK\n");
    return g_fail;
}
