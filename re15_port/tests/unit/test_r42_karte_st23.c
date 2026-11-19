/* =============================================================================
 * RIEGEL r42/karte: KARTE STAGE2 (Blatt 6) + STAGE3 (Blatt 7) - Leon UND Elza
 * =============================================================================
 * Nutzer 2026-11-18: "Parallel kannst du dich auch mit der Map auseinander setzen, das die
 * fuer STAGE2-6 genauso sauber funktioniert wir bei STAGE1. Das ist aktuell noch nicht der
 * Fall. Inklusive dem - wo befindet sich Leon - Marker." (dazu 2026-11-15 Punkt 5:
 * "Karten STAGE2 fehlen").
 *
 * Gemessen wird ueber den ECHTEN Pfad: Raum laden wie das Spiel (scd_room_reenter), Zone aus
 * der Spielerposition (re15_map_zone_update), Blatt = re15_inv_map_page_shown, Marker =
 * re15_inv_map_marker.
 *
 * ⛔ WAS HIER UNABHAENGIG IST UND WAS NICHT (Pruefbericht r42 Stufe 1 Runde 1, R2): [B] und [C]
 * pruefen die Lage gegen die gemalte Flaeche des eigenen Rechtecks - bei HERGELEITETEN Zeilen ist
 * das per Konstruktion fast immer erfuellt (tools/karte_r42_zeilen.py legt die Kollisions-Bbox in
 * genau diese Flaeche). Sie halten nur fest, dass kein Raum auf einem FREMDEN Rechteck liegt und
 * der Marker sich bewegt. Unabhaengig von der Herleitung sind [E] (Haendigkeit: kein Spiegel),
 * [F] (Tuer -> gemaltes Tuersymbol bzw. ausgelieferte Nachbar-Zeile), [G] (Treppen aus den
 * SCD-Zonen) und [H] (jedes gemalte Tuersymbol hat seine Marke). Die Tuer-Belege in [F] haben nur
 * die DREHUNG gewaehlt (diskret, je Raum mindestens zwei Stimmen), nicht Versatz und Massstab.
 *
 *  [A] jeder erreichbare Raum hat auf seinem Blatt eine Zone mit Rechteck (ROOM2020 fehlte);
 *  [B] der Marker liegt auf den GEMALTEN Texeln des eigenen Rechtecks (+-1 px), je Raum >= 75 %
 *      der Rasterpunkte (vorher STAGE2 40 %: Raeume lagen auf dem Rechteck von ROOM2000);
 *  [C] der Marker laeuft mit: je Zone mindestens die Haelfte verschiedener Positionen
 *      (vorher ROOM2050: 1 Position, ROOM3060/3070: 3..5 von 37/48);
 *  [D] jede Tuer zwischen zwei Raeumen desselben Blatts zeigt ihre Marke (Identitaet: Marke
 *      verbindet die eigene Zone mit einer Zone des Zielraums) - nur der eigene Raum besucht.
 *  [E] DREHUNG STATT SPIEGEL, alle Zonen aller Blaetter: die Abbildung Welt -> Karte hat dieselbe
 *      Haendigkeit wie die ausgelieferten Zeilen (FUN_800473f8 @0x8004741c-0x80047528: +x nach
 *      rechts, +z nach oben, Kreuzprodukt < 0). Runde 1 hatte 30A0/3050/2070 Zone 1 (1,0) und
 *      20A0 (0,1) - im Spiel lief der Punkt in 30A0 seitenverkehrt.
 *  [F] RICHTUNG gegen unabhaengige Belege: geht die Figur von der Zonenmitte zur Tuer, laeuft der
 *      Marker auf das gemalte Tuersymbol / die Projektion der ausgelieferten Nachbar-Zeile zu und
 *      endet dort hoechstens 8 px daneben.
 *  [G] TREPPEN aus den SCD-Zonen Aot_set Typ 12/13 stehen als Marke auf dem Blatt (ROOM3040 mit
 *      seiner AUSGELIEFERTEN Zeile @0x80076A60: (203,75) und (231,76) - in Runde 1 geloescht).
 *  [H] jedes gemalte Tuersymbol auf Blatt 6/7 hat eine Tuermarke hoechstens 4 px neben seiner
 *      Wandlinie (sonst stehen zwei Tuerzeichen auf dem Blatt).
 * Ausnahmen stehen namentlich in s_ausnahme mit Grund.
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

/* Raeume der beiden Blaetter. ROOM20B0 fehlt mit Absicht: keine Tuer im Spiel fuehrt hinein
 * (Tuerliste aller RDTs, re15_port/tools/engine_tueren.txt; in ROOM2000 steht dieselbe Anlage). */
static const unsigned s_raeume[] = {
    0x2000, 0x2010, 0x2020, 0x2030, 0x2040, 0x2050, 0x2060, 0x2070, 0x2080, 0x2090, 0x20A0,
    0x3000, 0x3010, 0x3020, 0x3030, 0x3040, 0x3050, 0x3060, 0x3070, 0x3080, 0x3090,
    0x30A0, 0x30B0, 0x30C0, 0x30D0, 0x30E0,
};

/* Ausnahmen (Raum, Zone, Pruefung), jeweils mit Grund. */
static const struct { unsigned room; int zone; char pruef; const char *grund; } s_ausnahme[] = {
    { 0x3030, 0, 'C', "r3 ist 8 px breit gemalt - die 4-px-Klemmung laesst waagerecht 1 Spalte" },
    { 0x3080, 0, 'C', "r8 ist 8x9 px gemalt (Original-Zeile @0x80076A80 trifft genau dorthin)" },
    { 0x30E0, 0, 'B', "30E0 Zone 0 = L-Gang, gemalt als Suedband von r4 (y93..103); der Nordarm des L hat keine eigene Kunst - OFFEN" },
    { 0x2070, 2, 'B', "2070 Zone 2 (Gang zu 11A0) ist nicht gemalt - OFFEN (Schema-Kasten)" },
    { 0x2070, 2, 'C', "dito" },
    { 0x2070, 1, 'D', "2070 Zone 1 ist eine Kopie des Gangs, in die keine Tuer einen Spawn setzt (unerreichbar)" },
    { 0x3090, 0, 'B', "AUSGELIEFERTE Zeile @0x80076A88: die 3090-Bbox umschliesst r10/r11 (= 30A0/30B0), 68 % auf r9" },
    { 0x20A0, 0, 'C', "PORT-WAHL r10 ist 11x8 px gemalt (Original: Blatt 0xD ohne Kunst, Datei-Id @0x80074c66 = 0)" },
};
static int ausnahme(unsigned room, int zone, char p)
{
    size_t i;
    for (i = 0; i < sizeof s_ausnahme / sizeof s_ausnahme[0]; i++)
        if (s_ausnahme[i].room == (room & ~1u) && s_ausnahme[i].zone == zone && s_ausnahme[i].pruef == p)
            return 1;
    return 0;
}

/* [F] Tuer -> unabhaengiger Kartenpunkt (Raum, Zone, Trigger-Mitte aus dem RDT, Kartenpunkt, Herkunft).
 * Kartenpunkt = Wandlinie neben dem GEMALTEN Symbol (Katalog analysis/kartensymbole, STAGE1-Regel
 * W x-1 / O x+w / N y-1 / S y+h) oder Projektion der Gegen-Tuer mit der AUSGELIEFERTEN Nachbar-Zeile. */
static const struct { unsigned room; int zone; int32_t x, z; int mx, my; const char *beleg; } s_richtung[] = {
    { 0x2010, 0,  -5300,  -4925,  90, 131, "gemalt (88,131) E; 2000 @0x800769E0 -> (91,131)" },
    { 0x2020, 0,  11400,  -3850, 125, 100, "gemalt (125,98) S an r2" },
    { 0x2020, 0, -14200,   3300, 182, 117, "gemalt (182,120) N an r6" },
    { 0x2030, 0,   -450, -10400, 182, 117, "gemalt (182,120) N" },
    { 0x2030, 0,  -7700,  -7250, 198, 126, "gemalt (201,126) W an r7" },
    { 0x2050, 0,  -5150,   4450, 125, 100, "gemalt (125,98) S" },
    { 0x2050, 0,   9100,   5700,  91, 103, "2000 @0x800769E0 Tuer ->2050" },
    { 0x2060, 0,   8450,   3950, 198, 126, "gemalt (201,126) W" },
    { 0x3020, 0, -26750, -22500, 167, 174, "gemalt (170,174) W" },
    { 0x3050, 0, -23951,   2183, 191,  54, "gemalt (194,54) W; 3040 @0x80076A60 -> (194,57)" },
    { 0x3050, 0,  -3526,   2083, 190,  98, "gemalt (187,98) E" },
    { 0x30A0, 0, -26550,  11800, 242, 155, "gemalt (242,158) N; 3090 @0x80076A88 -> (244,155)" },
    { 0x30B0, 0, -23850, -24400, 220, 191, "gemalt (218,191) E; 3090 @0x80076A88 -> (222,189)" },
    { 0x30C0, 0,  -5750,   -300, 215, 132, "gemalt (218,132) W; 3090 @0x80076A88 -> (217,137)" },
    { 0x30C0, 0, -11800,   1050, 208, 128, "gemalt (208,126) S; 3030 @0x80076A58 -> (208,127)" },
    { 0x30E0, 0,   3700,   4625, 190,  98, "gemalt (187,98) E an r5" },
    { 0x30E0, 0,   7125,   2200, 208, 104, "gemalt (208,107) N; 3030 @0x80076A58 -> (208,106)" },
};

/* [G] Treppen: SCD Aot_set Typ 12/13 (Raum, Weltmitte des Paars bzw. Endes, Kartenpunkt, Sprossen-kind). */
static const struct { unsigned room; int32_t x, z; int page, mx, my, kind; const char *beleg; } s_treppe[] = {
    { 0x3040, -22725, -13200, 7, 203,  75, 5, "Aot_set sce 12 Baender 0/1, AUSGELIEFERTE Zeile @0x80076A60" },
    { 0x3040,  -9588, -13575, 7, 231,  76, 5, "Aot_set sce 12 Baender 0/1, AUSGELIEFERTE Zeile @0x80076A60" },
    { 0x3010, -21520, -18070, 7, 135, 188, 4, "Aot_set sce 13, AUSGELIEFERTE Zeile @0x80076A48" },
    { 0x30C0, -17500,    300, 7, 189, 131, 5, "Aot_set sce 12 Baender 0/4 (count 4) gemittelt" },
    { 0x2030,  -4200,  -7700, 6, 187, 124, 5, "Aot_set sce 12" },
    { 0x2080, -19550, -16700, 6, 226, 155, 5, "Aot_set sce 12 zwischen den beiden Zonen" },
};

/* [H] gemalte Tuersymbole Blatt 6/7 (Katalog), Wandpunkt. Ausnahme mit Grund. */
static const struct { int page, mx, my; const char *ausnahme; } s_gemalt[] = {
    { 6,  90, 131, 0 }, { 6, 107, 141, 0 }, { 6, 125, 100, 0 }, { 6, 164, 141, 0 }, { 6, 182, 117, 0 },
    { 6, 198, 126, 0 },
    { 7, 128, 114, 0 }, { 7, 121, 188, 0 }, { 7, 167, 174, 0 }, { 7, 208, 104, 0 }, { 7, 208, 128, 0 },
    { 7, 191,  54, 0 }, { 7, 190,  98, 0 }, { 7, 220, 104, 0 }, { 7, 167, 115, 0 }, { 7, 220, 128, 0 },
    { 7, 104, 180, 0 }, { 7, 215, 132, 0 }, { 7, 242, 155, 0 }, { 7, 220, 191, 0 },
    { 7, 198, 159, "(198,163) N an r2/r12: kein Tuer-Datensatz von 3020/30D0/30C0 fuehrt dorthin - OFFEN" },
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
    re15_map_visited_reset();
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
        if (!z || z->rect == 255) continue;
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
    CHECK("[E] keine Zone ist gespiegelt (nur Drehungen)", spiegel == 0 && n_e > 100);
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

    printf("=== r42 Karte STAGE2/STAGE3: Zone, Kachel, Mitlaufen, Tuermarken, Drehung, Richtung, Treppen, gemalte Tueren ===\n");
    for (ri = 0; ri < sizeof s_raeume / sizeof s_raeume[0]; ri++)
    for (var = 0; var < 2; var++) {
        unsigned rid = s_raeume[ri] | (unsigned)var;
        int i, k, zonen = 0, seite_soll = ((rid >> 12) == 2) ? 6 : 7;
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
            if (zz->page != seite_soll || zz->rect == 255) continue;
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
            if (ziel == rid || (ziel >> 12) != (rid >> 12) || (ziel & ~1u) == 0x20B0u) continue;
            if (!betrete(rid, a->x, a->z, (int)d->band)) continue;
            re15_map_visited_reset();
            re15_map_visited_mark_at(rid, a->x, a->z);
            re15_map_zone_update(rid, a->x, a->z);
            zn = re15_map_zone_current();
            if (!zn) continue;
            seite = re15_inv_map_page_shown();
            if (seite != seite_soll || ausnahme(rid, zn->idx, 'B') || ausnahme(rid, zn->idx, 'D')) continue;
            {   /* Ziel-Zone selbst offen (30E0 Zone 0)? Dann gibt es dort keine belastbare Lage. */
                const re15_map_zone_t *zt = re15_map_zone_at(ziel, d->spawn_x, d->spawn_z);
                if (zt && ausnahme(ziel, zt->idx, 'B')) continue;
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
    CHECK("[D] Tueren zeigen ihre Marke", tueren >= 40 && ohne_marke == 0);
    pruefe_efgh();
    if (s_roh) free(s_roh);
    printf(g_fail ? "=== FAIL ===\n" : "=== PASS ===\n");
    return g_fail;
}
