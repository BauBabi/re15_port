/* =============================================================================
 * RIEGEL r42/karte Stufe 2: KARTE STAGE4 (Blatt 8), STAGE5 (Blaetter 9-11), STAGE6 (Blatt 12) - Leon UND Elza
 * =============================================================================
 * Nutzer 2026-11-18: "Parallel kannst du dich auch mit der Map auseinander setzen, das die
 * fuer STAGE2-6 genauso sauber funktioniert wir bei STAGE1. Das ist aktuell noch nicht der
 * Fall. Inklusive dem - wo befindet sich Leon - Marker."
 *
 * Derselbe echte Pfad wie unit_r42_karte_st23 (Raum laden wie das Spiel, Zone aus der Lage,
 * Blatt = re15_inv_map_page_shown, Marker = re15_inv_map_marker). Die Zeilen kommen aus
 * tools/karte_r42_zeilen.py 8..12; Rechteck je Raum aus der AUSGELIEFERTEN Zeile (@0x800768b0 + 8*Slot,
 * FUN_800473f8 @0x8004741c-0x80047528: die SCA-Zellen fallen auf genau dieses Rechteck) bzw. der
 * Tuer-Identitaet. Unabhaengig von der Herleitung sind [F] (Tuer -> gemaltes Symbol / ausgelieferte
 * Nachbar-Zeile), [G] (Treppen aus den SCD-Zonen) und [H] (jedes gemalte Tuersymbol hat seine Marke);
 * [B]/[C] halten nur fest, dass kein Raum auf einem FREMDEN Rechteck liegt und der Marker laeuft
 * (vorher: 5040/5120 auf r5 statt r4, 50A0/5140 auf r14 statt r10, 5100 auf r1 statt r0, 4040/4050
 * auf den 16-px-Kacheln r12/r2, 5070/5130 ohne Zone, 5000/5100 ohne Zeile).
 *  [A] jeder erreichbare Raum hat auf seinem Blatt eine Zone (5070/5130 fehlten);
 *  [B] Marker auf den gemalten Texeln des eigenen Rechtecks, je Zone >= 75 %;
 *  [C] Marker laeuft mit, je Zone >= 50 % verschiedene Positionen;
 *  [D] jede Tuer zu einem Raum mit Zone auf demselben Blatt zeigt ihre Marke (Identitaet);
 *  [E] keine Zone auf Blatt 8-12 gespiegelt (nur Drehungen);
 *  [F] Richtung: geht die Figur zur Tuer, laeuft der Marker zum Beleg und endet <= 8 px daneben;
 *  [G] Treppen; [H] jedes gemalte Tuersymbol Blatt 8-12 hat eine Tuermarke <= 4 px neben der Wandlinie.
 * Nicht hier: 5090/5091 (Wagen-Kacheln, U-12 in r42/rest), 6040 (Abspann, kein Kartenschirm).
 * ========================================================================== */
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
static uint8_t *s_roh = NULL;
static uint8_t *s_pix[13];

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

static int betrete(unsigned rid, int32_t px, int32_t pz, int band)
{
    char pfad[600];
    size_t n = 0;
    re15_actor_t *pl;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    if (s_roh) { free(s_roh); s_roh = NULL; }
    s_roh = slurp(pfad, &n);
    if (!s_roh || n < 0x100) return 0;
    if (re15_rdt_parse(s_roh, n, &s_rdt) < 0) return 0;
    scd_vm_init(); re15_actor_init(); re15_aot_init(); re15_enemy_reset();
    re15_enemy_ai_set_paused(1); re15_esp_fx_reset();
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

/* Texel der Originalkachel: headerlos, 256x256 4 bpp, 128 B je Zeile, unteres Nibble links
 * (DATA/MAP0x.PIX; Seite p -> Datei-Id u16 @0x80074c4c+2p = 12+p -> MAP0<p+1>). */
static int texel(int page, int u, int v)
{
    if (page < 0 || page > 12 || u < 0 || u > 255 || v < 0 || v > 255) return 0;
    if (!s_pix[page]) {
        char pfad[600]; size_t n = 0;
        snprintf(pfad, sizeof pfad, "%s/DATA/MAP%02X.PIX", RE15_ASSET_PSX_DIR, page + 1);
        s_pix[page] = slurp(pfad, &n);
        if (!s_pix[page] || n < 32768) return 0;
    }
    {
        uint8_t b = s_pix[page][v * 128 + u / 2];
        return (u & 1) ? (b >> 4) : (b & 15);
    }
}

static int auf_kachel(int page, int rect, int mx, int my)
{
    int rx, ry, rw, rh, ru, rv, dx, dy;
    if (!re15_map_rect_geometry((unsigned)page, (unsigned)rect, &rx, &ry, &rw, &rh)) return 0;
    if (!re15_map_rect_uv((unsigned)page, (unsigned)rect, &ru, &rv)) return 0;
    for (dx = -1; dx <= 1; dx++) for (dy = -1; dy <= 1; dy++) {
        int x = mx + dx - rx, y = my + dy - ry;
        if (x < 0 || y < 0 || x >= rw || y >= rh) continue;
        if (texel(page, ru + x, rv + y)) return 1;
    }
    return 0;
}

static int in_rvd(int32_t px, int32_t pz)
{
    int q;
    for (q = 0; q < s_rdt.zone_count; q++) {
        const re15_rdt_zone_t *Z = &s_rdt.zones[q];
        int e, s = 0, ok = 1;
        for (e = 0; e < 4 && ok; e++) {
            int64_t x1 = Z->xs[e], z1 = Z->zs[e], x2 = Z->xs[(e + 1) & 3], z2 = Z->zs[(e + 1) & 3];
            int64_t c = (x2 - x1) * (pz - z1) - (z2 - z1) * (px - x1);
            if (c != 0) { if (!s) s = c > 0 ? 1 : -1; else if ((c > 0) != (s > 0)) ok = 0; }
        }
        if (ok) return 1;
    }
    return 0;
}

/* Raeume und ihr Blatt. PORT-WAHL (tools/karte_r42_zeilen.py): 5080 auf Blatt 12 (gemalt r6 an der
 * Westtuer von 6010), 6020 auf Blatt 9 (die zwei gemalten Achtecke r8/r9 an den Osttueren von r7). 4070/50D0/4020
 * zeigen je nach Band ein anderes Blatt (s_map_floors); hier das Blatt der Hauptzeile. */
static const struct { unsigned room; int page; } s_raeume[] = {
    { 0x4000, 8 }, { 0x4010, 8 }, { 0x4020, 8 }, { 0x4030, 8 }, { 0x4040, 8 }, { 0x4050, 8 }, { 0x4070, 8 },
    { 0x4080, 8 }, { 0x4090, 8 }, { 0x40A0, 8 }, { 0x40B0, 8 },
    { 0x5000, 9 }, { 0x5010, 9 }, { 0x5020, 9 }, { 0x5030, 9 }, { 0x5040, 9 }, { 0x5050, 9 }, { 0x5060, 9 },
    { 0x5070, 9 }, { 0x50A0, 9 }, { 0x50B0, 9 }, { 0x5110, 9 }, { 0x5120, 9 }, { 0x5130, 9 }, { 0x5140, 9 },
    { 0x50C0, 10 }, { 0x50D0, 10 }, { 0x50E0, 10 }, { 0x50F0, 11 }, { 0x5100, 11 },
    { 0x5080, 12 }, { 0x6000, 12 }, { 0x6010, 12 }, { 0x6020, 9 }, { 0x6030, 12 },
};

/* Ausnahmen (Raum, Zone, Pruefung), jeweils mit Grund. */
static const struct { unsigned room; int zone; char pruef; const char *grund; } s_ausnahme[] = {
    { 0x6030, 0, 'B', "AUSGELIEFERTE Zeile @0x80076BD8 legt den Zug nach x 249..310, gemalt ist nur r7 (250..265)" },
    { 0x6030, 0, 'C', "dito - der Marker klemmt an r7 (Auslieferungsstand)" },
    /* AUSGELIEFERTE Zeilen: die Kunst deckt nicht die ganze Projektion der Kollision (Auslieferungsstand) */
    { 0x4000, 0, 'B', "AUSGELIEFERT @0x80076AB8: 25/36, Gang r14 und Ostteil ungemalt" },
    { 0x4090, 0, 'B', "AUSGELIEFERT @0x80076B00: 36/49" },
    { 0x5040, 0, 'B', "AUSGELIEFERT @0x80076B38: 11/18" },
    { 0x5120, 0, 'B', "AUSGELIEFERT @0x80076BA8: 14/23" },
    { 0x5140, 0, 'B', "AUSGELIEFERT @0x80076BB8: 15/21" },
    { 0x5100, 0, 'B', "AUSGELIEFERT @0x80076B98: 28/38" },
    /* Etagen-Raeume: ohne Band zeigt das Spiel das Band-0-Blatt (Gast-Zeile Blatt 11), nicht die Hauptzeile */
    { 0x4070, 0, 'B', "Treppenhaus: Blatt haengt am Band (s_map_floors), Band 0 -> Blatt 11 r3" },
    { 0x50D0, 0, 'B', "zwei Stockwerke: Band 0 -> Blatt 11 r1, Band 3 -> Blatt 10 r1" },
    /* Hergeleitet, Kunst passt nicht zur Kollision: seit Stufe 3 Runde 2 klemmt re15_karte_r42_kunst_klemmen den
     * Marker auf die gemalte Kachel (Blatt 6-12, hergeleitete Zeilen) - die frueheren [B]-Ausnahmen 4040/4080 Ring,
     * 5070/5130 T-Gang, 5080 r6 und 6000 r5 sind gestrichen und muessen jetzt bestehen. */
    /* [C] 4080 Zone 1 = Diagonalstueck r13 (16x16, gemalt nur der schmale Streifen): das Bbox-Raster legt 16 der
     * 28 Punkte neben den Streifen (vorher 12/28 auf der Kunst, 14 verschiedene Lagen); die Klemme setzt sie auf
     * den naechsten Streifen-Texel -> 13 verschiedene. Die echten Lagen dort pruefen unit_r42_karte_ankunft und
     * die Abzuege L4080_t0/w0 (Stufe 3 Runde 2). */
    { 0x4080, 1, 'C', "Diagonalstueck r13: Raster ueberwiegend neben dem gemalten Streifen (Stufe 3 Runde 2: 13/28)" },
    /* [C] 4020 Fahrstuhlkabine (Bestand-Zeile 122/87/1760/1760): gemalt ist nur 12x12 der 16x16-Kachel, der
     * Raumkoerper 10x10 (DATA/MAP09.PIX Blatt 8 r2: Wand Index 4 in Zeile 0/11 und Spalte 0/11, rechts/unten 4 px
     * leer). Die Zeile spannt die Kabine ueber die ganze Kachel; Rasterpunkte jenseits der gemalten Wand landen
     * seit Stufe 3 Runde 2 auf dem Innenrand statt auf leerem Blau -> 20/49 verschiedene Lagen. */
    { 0x4020, 0, 'C', "Kabine: gemalt 12x12 von 16x16, Punkte jenseits der Wand auf dem Innenrand (Stufe 3 Runde 2: 20/49)" },
};
static int ausnahme(unsigned room, int zone, char p)
{
    size_t i;
    for (i = 0; i < sizeof s_ausnahme / sizeof s_ausnahme[0]; i++)
        if (s_ausnahme[i].room == (room & ~1u) && s_ausnahme[i].zone == zone && s_ausnahme[i].pruef == p)
            return 1;
    return 0;
}

/* [F] Tuer -> unabhaengiger Kartenpunkt (Wandlinie neben dem GEMALTEN Symbol, Katalog analysis/kartensymbole,
 * Regel W x-1 / O x+w / N y-1 / S y+h; oder Projektion der Gegen-Tuer mit der AUSGELIEFERTEN Nachbar-Zeile). */
static const struct { unsigned room; int zone; int32_t x, z; int mx, my; const char *beleg; } s_richtung[] = {
    { 0x4040, 0,  -4000, -18800, 143, 163, "gemalt (141,163) E auf r4 (->4070)" },
    { 0x4040, 0, -27000,  17250, 102, 104, "gemalt (100,104) E auf r5 (->4050)" },
    { 0x4050, 0,   8850, -11600, 102, 104, "gemalt (100,104) E auf r5" },
    { 0x4050, 1,  -7650, -26800, 110, 119, "gemalt (113,119) W auf r6" },
    { 0x4080, 0,   1950, -25200, 177, 163, "gemalt TUER2 (175,163) E auf r11 (->40B0)" },
    { 0x4080, 0,  21300,  -5400, 209, 128, "gemalt TUER2 (207,128) E auf r9; 4090 @0x80076B00" },
    { 0x5000, 0,  -9600,   7100, 168,  87, "gemalt (165,87) E auf r0; 5010 @0x80076B20 -> (171,90)" },
    { 0x5000, 0, -20050,  -1900, 153,  70, "gemalt TUER2 (153,73) N auf r0 (->5020)" },
    { 0x5000, 0,   -550,  -2100, 153, 105, "5030 @0x80076B30 -> (153,105)" },
    { 0x5020, 0, -12350, -26400, 153,  70, "gemalt TUER2 (153,73) N auf r0" },
    { 0x5050, 0, -26425, -18250,  88, 100, "5040 @0x80076B38 -> (88,100)" },
    { 0x5070, 0,   2550,  -5650, 172, 125, "5030 @0x80076B30 -> (172,125)" },
    { 0x5070, 0, -28900,  13300, 226, 155, "gemalt (223,155) E auf r7" },
    { 0x5070, 0, -28900, -24650, 226,  91, "gemalt (223,91) E auf r7" },
    { 0x5130, 0,   2550,  -5650, 172, 126, "5110 @0x80076BA0 -> (172,126)" },
    { 0x6020, 0,  -4500,   1350, 226, 155, "gemalt (223,155) E auf r7" },
    { 0x6020, 1,  -4050, -17700, 226,  91, "gemalt (223,91) E auf r7" },
    { 0x50B0, 0, -14013, -27000, 196, 169, "5140 @0x80076BB8 -> (196,169); gemalt TUER2 (195,171) N auf r12" },
    { 0x50D0, 0, -24160,  -1959, 124, 114, "gemalt TUER2 (124,117) N auf r1; 50C0 @0x80076B78 -> (126,113)" },
    { 0x6010, 0,  -5215,  -8038, 193, 103, "gemalt (191,103) E auf r0" },
    { 0x6010, 0,  -5215, -20638, 193, 133, "gemalt (191,133) E auf r0" },
};

/* [G] Treppen: SCD Aot_set Typ 12/13 (Raum, Weltmitte, Blatt, Kartenpunkt, Sprossen-kind). */
static const struct { unsigned room; int32_t x, z; int page, mx, my, kind; const char *beleg; } s_treppe[] = {
    { 0x5020, -13538, -17838,  9, 154,  55, 4, "Aot_set sce 13 Baender 0/4 gemittelt" },
    { 0x6010, -22078,  -4638, 12, 160, 100, 5, "Aot_set sce 12 Baender 0/1" },
    { 0x6010, -22078, -24288, 12, 160, 136, 5, "Aot_set sce 12 Baender 0/1" },
    { 0x6010, -18462,  -7430, 12, 167, 105, 4, "Aot_set sce 13 Baender 1/4" },
    { 0x6010, -18487, -20853, 12, 167, 130, 4, "Aot_set sce 13 Baender 1/4" },
    { 0x4070,  -8800, -24950,  8, 150, 170, 5, "Aot_set sce 12 Band 12 (Treppenhaus r7)" },
};

/* [H] gemalte Tuersymbole Blatt 8-12 (Katalog), Wandpunkt. */
static const struct { int page, mx, my; const char *ausnahme; } s_gemalt[] = {
    { 8, 156,  64, 0 }, { 8, 143, 163, 0 }, { 8, 102, 104, 0 }, { 8, 110, 119, 0 }, { 8, 209, 128, 0 },
    { 8, 177, 163, 0 },
    { 9, 153,  70, 0 }, { 9, 168,  87, 0 }, { 9,  94, 137, 0 }, { 9, 226,  91, 0 }, { 9, 226, 155, 0 },
    { 9, 137, 177, 0 }, { 9, 195, 168, 0 },
    { 10, 124, 114, 0 }, { 11, 158, 157, 0 }, { 11, 162, 133, 0 },
    { 12, 193, 103, 0 }, { 12, 193, 133, 0 }, { 12, 139, 132, 0 },
};

static const re15_map_zone_t *zone_von(unsigned room, int idx)
{
    int i;
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        if (z && z->room == room && z->idx == idx && !z->etage) return z;
    }
    return 0;
}

static void alle_besucht(void)
{
    int i;
    uint8_t alle_etagen[16];
    re15_map_visited_reset();
    memset(alle_etagen, 0xFF, sizeof alle_etagen);     /* auch jede Etagen-Zeile (Treppenhaus 4070 Band 12) */
    re15_map_visited_floor_import(alle_etagen);
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        if (z && !z->etage)
            re15_map_visited_mark_at(z->room, (z->wx0 + z->wx1) / 2, (z->wz0 + z->wz1) / 2);
    }
}

static int marke_bei(int page, int mx, int my, int tol, int treppe, int kind)
{
    int k, n = re15_map_mark_count(), best = 9999;
    for (k = 0; k < n; k++) {
        int pg, rc, x, y, kd, d;
        if (!re15_map_mark_get(k, &pg, &rc, &x, &y, &kd)) continue;
        if (pg != page || (treppe ? kd < 4 : kd > 3)) continue;
        if (treppe && kind >= 0 && kd != kind) continue;
        d = abs(x - mx) > abs(y - my) ? abs(x - mx) : abs(y - my);
        if (d < best) best = d;
    }
    return best <= tol ? best : -1;
}

static int cheb(int ax, int ay, int bx, int by)
{
    return abs(ax - bx) > abs(ay - by) ? abs(ax - bx) : abs(ay - by);
}

static void pruefe_efgh(void)
{
    int i, spiegel = 0, f_schlecht = 0, g_fehlt = 0, h_fehlt = 0, n_e = 0;
    char le[1024] = "", lf[1024] = "", lg[512] = "", lh[512] = "";
    /* [E] alle Zonen, grosses Klemm-Rechteck */
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        int16_t x0, y0, x1, y1, x2, y2;
        int32_t cx, cz;
        long kreuz;
        if (!z || z->rect == 255 || z->page < 8) continue;
        cx = (z->wx0 + z->wx1) / 2; cz = (z->wz0 + z->wz1) / 2;
        if (!re15_map_zone_marker(z, cx, cz, -30000, -30000, 60000, 60000, &x0, &y0)) continue;
        re15_map_zone_marker(z, cx + 4000, cz, -30000, -30000, 60000, 60000, &x1, &y1);
        re15_map_zone_marker(z, cx, cz + 4000, -30000, -30000, 60000, 60000, &x2, &y2);
        kreuz = (long)(x1 - x0) * (y2 - y0) - (long)(y1 - y0) * (x2 - x0);
        n_e++;
        if (kreuz >= 0) {
            char e[48]; spiegel++;
            snprintf(e, sizeof e, " %04X.z%d(%d,%d)", z->room, z->idx, z->flip_x, z->flip_z);
            strncat(le, e, sizeof le - strlen(le) - 1);
        }
    }
    /* [F] */
    for (i = 0; i < (int)(sizeof s_richtung / sizeof s_richtung[0]); i++) {
        unsigned var;
        for (var = 0; var < 2; var++) {
            const re15_map_zone_t *z = zone_von(s_richtung[i].room | var, s_richtung[i].zone);
            int rx, ry, rw, rh, dm, dd;
            int16_t mx, my, cx, cy;
            if (!z || !re15_map_rect_geometry(z->page, z->rect, &rx, &ry, &rw, &rh)) {
                char e[48]; f_schlecht++;
                snprintf(e, sizeof e, " %04X.z%d(keine Zone)", s_richtung[i].room | var, s_richtung[i].zone);
                strncat(lf, e, sizeof lf - strlen(lf) - 1);
                continue;
            }
            re15_map_zone_marker(z, s_richtung[i].x, s_richtung[i].z, rx, ry, rw, rh, &mx, &my);
            re15_map_zone_marker(z, (z->wx0 + z->wx1) / 2, (z->wz0 + z->wz1) / 2, rx, ry, rw, rh, &cx, &cy);
            dm = cheb(mx, my, s_richtung[i].mx, s_richtung[i].my);
            dd = cheb(cx, cy, s_richtung[i].mx, s_richtung[i].my);
            if (var == 0)
                printf("    [F] %04X.z%d Tuer (%d,%d): Marker (%d,%d), Beleg (%d,%d) %s -> %d px (Mitte %d px)\n",
                       s_richtung[i].room, s_richtung[i].zone, (int)s_richtung[i].x, (int)s_richtung[i].z, mx, my,
                       s_richtung[i].mx, s_richtung[i].my, s_richtung[i].beleg, dm, dd);
            if (dm > 8 || dm >= dd) {
                char e[64]; f_schlecht++;
                snprintf(e, sizeof e, " %04X.z%d->(%d,%d)=%dpx", s_richtung[i].room | var, s_richtung[i].zone,
                         s_richtung[i].mx, s_richtung[i].my, dm);
                strncat(lf, e, sizeof lf - strlen(lf) - 1);
            }
        }
    }
    /* [G]/[H]: alle Zonen besucht, damit jede Marke sichtbar ist */
    alle_besucht();
    for (i = 0; i < (int)(sizeof s_treppe / sizeof s_treppe[0]); i++) {
        const re15_map_zone_t *z = re15_map_zone_at(s_treppe[i].room, s_treppe[i].x, s_treppe[i].z);
        int16_t mx = 0, my = 0;
        int rx, ry, rw, rh, d;
        if (z && re15_map_rect_geometry(z->page, z->rect, &rx, &ry, &rw, &rh))
            re15_map_zone_marker(z, s_treppe[i].x, s_treppe[i].z, rx, ry, rw, rh, &mx, &my);
        d = marke_bei(s_treppe[i].page, s_treppe[i].mx, s_treppe[i].my, 1, 1, s_treppe[i].kind);
        printf("    [G] ROOM%04X Treppe (%d,%d) %s: Marke (%d,%d) %s, Marker dort (%d,%d)\n", s_treppe[i].room,
               (int)s_treppe[i].x, (int)s_treppe[i].z, s_treppe[i].beleg, s_treppe[i].mx, s_treppe[i].my,
               d >= 0 ? "da" : "FEHLT", mx, my);
        if (d < 0 || abs(mx - s_treppe[i].mx) > 1 || abs(my - s_treppe[i].my) > 1) {
            char e[48]; g_fehlt++;
            snprintf(e, sizeof e, " %04X(%d,%d)", s_treppe[i].room, s_treppe[i].mx, s_treppe[i].my);
            strncat(lg, e, sizeof lg - strlen(lg) - 1);
        }
    }
    for (i = 0; i < (int)(sizeof s_gemalt / sizeof s_gemalt[0]); i++) {
        int d = marke_bei(s_gemalt[i].page, s_gemalt[i].mx, s_gemalt[i].my, 4, 0, -1);
        if (s_gemalt[i].ausnahme) { printf("    [H] Ausnahme: %s\n", s_gemalt[i].ausnahme); continue; }
        if (d < 0) {
            char e[48]; h_fehlt++;
            snprintf(e, sizeof e, " B%d(%d,%d)", s_gemalt[i].page, s_gemalt[i].mx, s_gemalt[i].my);
            strncat(lh, e, sizeof lh - strlen(lh) - 1);
        }
    }
    re15_map_visited_reset();
    printf("  [E] %d Zonen, gespiegelt:%s\n", n_e, le[0] ? le : " keine");
    printf("  [F] Tueren neben/gegen ihren Beleg:%s\n", lf[0] ? lf : " keine");
    printf("  [G] Treppen ohne Marke:%s\n", lg[0] ? lg : " keine");
    printf("  [H] gemalte Tuersymbole ohne Marke (4 px):%s\n", lh[0] ? lh : " keine");
    CHECK("[E] keine Zone auf Blatt 8-12 ist gespiegelt (nur Drehungen)", spiegel == 0 && n_e > 60);
    CHECK("[F] Marker laeuft auf die belegte Tuer zu und endet <= 8 px daneben", f_schlecht == 0);
    CHECK("[G] Treppen aus den SCD-Zonen stehen auf dem Blatt", g_fehlt == 0);
    CHECK("[H] jedes gemalte Tuersymbol hat seine Marke", h_fehlt == 0);
}

int main(void)
{
    static re15_aot_t sn_a[RE15_AOT_MAX];
    static re15_aot_door_params_t sn_d[RE15_AOT_MAX];
    size_t ri;
    int fehlt = 0, schlecht_b = 0, schlecht_c = 0, tueren = 0, ohne_marke = 0, var;
    char liste_a[512] = "", liste_b[1024] = "", liste_c[1024] = "", liste_d[1024] = "";

    printf("=== r42 Karte STAGE4-6: Zone, Kachel, Mitlaufen, Tuermarken, Drehung, Richtung, Treppen, gemalte Tueren ===\n");
    for (ri = 0; ri < sizeof s_raeume / sizeof s_raeume[0]; ri++)
    for (var = 0; var < 2; var++) {
        unsigned rid = s_raeume[ri].room | (unsigned)var;
        int i, k, zonen = 0, seite_soll = s_raeume[ri].page;
        if (!betrete(rid, 0, 0, -1)) { fehlt++; continue; }
        memcpy(sn_a, g_aot.slots, sizeof sn_a);
        memcpy(sn_d, g_aot.door_params, sizeof sn_d);
        /* ---- [A] Zone + [B] Kachel + [C] Mitlaufen, Raster 7x7 je Zone ---- */
        for (i = 0; i < re15_map_zone_count(); i++) {
            const re15_map_zone_t *zz = re15_map_zone_by_index(i);
            int gx, gz, n = 0, auf = 0, versch = 0, j;
            int16_t pos[49][2];
            if (!zz || zz->room != (unsigned short)rid || zz->etage) continue;
            zonen++;
            if (zz->page != (unsigned)seite_soll || zz->rect == 255) continue;
            for (gx = 0; gx < 7; gx++) for (gz = 0; gz < 7; gz++) {
                int32_t px = zz->wx0 + (int32_t)((int64_t)(zz->wx1 - zz->wx0) * (2 * gx + 1) / 14);
                int32_t pz = zz->wz0 + (int32_t)((int64_t)(zz->wz1 - zz->wz0) * (2 * gz + 1) / 14);
                const re15_map_zone_t *zc;
                int16_t mx = 0, my = 0;
                if (!in_rvd(px, pz)) continue;
                g_actors[RE15_ACTOR_SLOT_PLAYER].x = px;
                g_actors[RE15_ACTOR_SLOT_PLAYER].z = pz;
                re15_map_zone_update(rid, px, pz);
                zc = re15_map_zone_current();
                if (!zc || zc->idx != zz->idx) continue;      /* Punkt gehoert einer anderen Zone */
                g_inv_screen.map_page = re15_inv_map_page_shown();
                re15_inv_map_marker(px, pz, 0, &mx, &my);
                if (g_inv_screen.map_page == seite_soll && auf_kachel(seite_soll, zz->rect, mx, my)) auf++;
                for (j = 0; j < n; j++) if (pos[j][0] == mx && pos[j][1] == my) break;
                if (j == n) versch++;
                pos[n][0] = mx; pos[n][1] = my; n++;
            }
            if (n == 0) continue;
            if (auf * 4 < n * 3 && !ausnahme(rid, zz->idx, 'B')) {
                char e[64]; schlecht_b++;
                snprintf(e, sizeof e, " %04X.z%d(%d/%d)", rid, zz->idx, auf, n);
                strncat(liste_b, e, sizeof liste_b - strlen(liste_b) - 1);
            }
            if (n >= 8 && versch * 2 < n && !ausnahme(rid, zz->idx, 'C')) {
                char e[64]; schlecht_c++;
                snprintf(e, sizeof e, " %04X.z%d(%d/%d)", rid, zz->idx, versch, n);
                strncat(liste_c, e, sizeof liste_c - strlen(liste_c) - 1);
            }
        }
        if (zonen == 0) {
            char e[16]; fehlt++;
            snprintf(e, sizeof e, " %04X", rid);
            strncat(liste_a, e, sizeof liste_a - strlen(liste_a) - 1);
        }
        /* ---- [D] Tuermarken ueber die Identitaet ---- */
        for (k = 0; k < RE15_AOT_MAX; k++) {
            const re15_aot_t *a = &sn_a[k];
            const re15_aot_door_params_t *d = &sn_d[k];
            const re15_map_zone_t *zn;
            unsigned ziel;
            int seite, mk, gef = 0, ix;
            if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
            if (a->half_w == 0 && a->half_h == 0) continue;               /* Skript-Warp */
            ziel = (((unsigned)d->dest_stage + 1u) << 12) | ((unsigned)d->dest_room << 4) | (rid & 0xFu);
            if (ziel == rid || (ziel & ~1u) == 0x5090u || (ziel & ~1u) == 0x6040u) continue;   /* U-12 / Abspann */
            if (!betrete(rid, a->x, a->z, (int)d->band)) continue;
            re15_map_visited_reset();
            re15_map_visited_mark_at(rid, a->x, a->z);
            re15_map_zone_update(rid, a->x, a->z);
            zn = re15_map_zone_current();
            if (!zn) continue;
            seite = re15_inv_map_page_shown();
            if (seite != seite_soll || ausnahme(rid, zn->idx, 'D')) continue;   /* [B]-Ausnahmen gelten hier NICHT */
            {   /* Ziel-Zone selbst offen (30E0 Zone 0)? Dann gibt es dort keine belastbare Lage. */
                const re15_map_zone_t *zt = re15_map_zone_at(ziel, d->spawn_x, d->spawn_z);
                if (zt && ausnahme(ziel, zt->idx, 'D')) continue;
            }
            /* nur Ziele mit Zone auf DIESEM Blatt */
            for (ix = 0, gef = -1; ix < 8; ix++) if (re15_map_zone_fuer(ziel, ix, (unsigned)seite)) gef = 0;
            if (gef < 0) continue;
            tueren++;
            for (mk = 0; mk < re15_map_mark_count() && !gef; mk++) {
                int pg, rc, sx, sy, kd, za, zb, andere;
                if (!re15_map_mark_get(mk, &pg, &rc, &sx, &sy, &kd)) continue;
                if (pg != seite || kd >= 4 || !re15_map_mark_zonen(mk, &za, &zb)) continue;
                if (za != zn->zid && zb != zn->zid) continue;
                andere = (za == zn->zid) ? zb : za;
                for (ix = 0; ix < 8 && !gef; ix++) {
                    const re15_map_zone_t *zt = re15_map_zone_fuer(ziel, ix, (unsigned)seite);
                    if (zt && zt->zid == andere) gef = 1;
                }
            }
            if (!gef) {
                char e[48]; ohne_marke++;
                snprintf(e, sizeof e, " %04X->%04X", rid, ziel);
                strncat(liste_d, e, sizeof liste_d - strlen(liste_d) - 1);
            }
        }
        betrete(rid, 0, 0, -1);
    }
    printf("  [A] Raeume ohne Zone:%s\n", liste_a[0] ? liste_a : " keine");
    printf("  [B] Marker neben der Kachel (<75%%):%s\n", liste_b[0] ? liste_b : " keine");
    printf("  [C] Marker steht (<50%% verschiedene Positionen):%s\n", liste_c[0] ? liste_c : " keine");
    printf("  [D] %d Tueren, ohne eigene Marke:%s\n", tueren, liste_d[0] ? liste_d : " keine");
    CHECK("[A] jeder erreichbare Raum (Leon+Elza) hat eine Zone", fehlt == 0);
    CHECK("[B] Marker auf den gemalten Texeln des eigenen Rechtecks", schlecht_b == 0);
    CHECK("[C] Marker laeuft mit der Figur mit", schlecht_c == 0);
    CHECK("[D] Tueren zeigen ihre Marke", tueren >= 100 && ohne_marke == 0);
    pruefe_efgh();
    if (s_roh) free(s_roh);
    printf(g_fail ? "=== FAIL ===\n" : "=== PASS ===\n");
    return g_fail;
}
