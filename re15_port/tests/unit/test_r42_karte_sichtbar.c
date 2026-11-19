/* =============================================================================
 * RIEGEL r42/karte Stufe 2 Runde 2: DER AKTUELLE RAUM IST AUF DEM BLATT ZU SEHEN - in JEDER Zone.
 * =============================================================================
 * Nutzer 2026-11-18: "Parallel kannst du dich auch mit der Map auseinander setzen, das die
 * fuer STAGE2-6 genauso sauber funktioniert wir bei STAGE1. Das ist aktuell noch nicht der
 * Fall. Inklusive dem - wo befindet sich Leon - Marker."
 *
 * Befund der Pruefung Stufe 2 Runde 1 (pruef_s2_r1/PRUEFUNG.md, ROT 1): im T-Gang 5070/5130
 * (Blatt 9 Rechteck 7) zeigt die Karte KEINEN Raum - nur Punkt und Tuerstriche auf leerem Blau.
 * Ursache: das Rechteck traegt TEILBEREICHE (s_map_teile), und die gehoeren alle dem Zug 5090
 * (zid 84). Ein Rechteck mit Teilen wurde NUR ueber die Teile gezeichnet; deren Zustand haengt
 * am zid 84 -> fuer jede andere Zone auf demselben Rechteck blieb es leer. Auf aaea5f8c2 traf
 * das 5020 (damals ebenfalls auf r7, "ROT FEHLT: ROOM5020" in integration_map_raum_live).
 *
 * unit_r42_karte_st456 [B] prueft nur, dass der Marker auf GEMALTEN Texeln liegt, nicht ob das
 * Feld GEZEICHNET wird - genau der blinde Fleck. Dieser Riegel baut die echte Op-Liste des
 * Kartenschirms (re15_inv_screen_build, derselbe Weg wie unit_map_teilbereich):
 *  [I] fuer jede Zone mit Original-Rechteck (alle Blaetter, Leon UND Elza): steht die Figur in
 *      ihr, liegt unter dem Marker eine Op in der Aktuell-Farbe (SPRT CLUT-Zeile AKTUELL, RE2
 *      `addiu s5,s5,1` @0x8006E648, oder FILL 0x842D = (104,8,8), RE15_KARTE_AKTUELL);
 *  [J] und das eigene Rechteck ist ganz gezeichnet: die Aktuell-Ops (dazu die Wandlinien der
 *      Runde-39-Korrekturen) decken >= 90 % seiner gemalten Texel (DATA/MAP0x.PIX, Kachel-uv @0x800768b0-Blob der Rechteck-Tabelle).
 *      Ausnahme sind nur Rechtecke, die ein Raum MIT Teilbereichen selbst besitzt (ROOM1110,
 *      Zug 5090): dort ist bewusst nur der Teil rot, in dem die Figur steht (Nutzer 2026-09-07,
 *      fehler/howto4.png).
 * [K]/[L] (unten) pruefen die Teil-Rechtecke selbst: Zug 5090 (Blatt 9 r7) und ROOM1110 (Blatt 3 r5).
 * Nicht in [I]/[J]: 5090/5091 (Zugteile, dort gilt [L]).
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_room.h"
#include "re15_inv_screen.h"
#include "re15_collision.h"

extern re15_inv_screen_t g_inv_screen;
extern unsigned g_current_room_id;

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", (name)); g_fail = 1; } \
                               else printf("  PASS: %s\n", (name)); } while (0)

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

static int ist_aktuell(const re15_inv_op_t *q)
{
    if (q->kind == RE15_INV_OP_SPRT && q->page == RE15_INV_PAGE_MAP4 && q->clut == RE15_INV_CLUT_MAP_AKTUELL)
        return 1;
    if (q->kind == RE15_INV_OP_FILL && q->r == 104 && q->g == 8 && q->b == 8) return 1;
    return 0;
}

/* Besitzt ein Raum Teilbereiche auf diesem Rechteck? (dann ist nur sein Teil rot) */
static int teil_besitzer(const re15_map_zone_t *z)
{
    return z->room == 0x1110 || z->room == 0x1111 || z->room == 0x5090 || z->room == 0x5091;
}

int main(void)
{
    static re15_inv_op_t ops[RE15_INV_MAX_OPS];
    static unsigned char gedeckt[256][512];   /* Schirm 320 breit (Kacheln bis x 309) */
    int i, geprueft = 0, i_fehlt = 0, j_fehlt = 0, s456 = 0;
    char li[2048] = "", lj[2048] = "";

    printf("=== r42 Karte: der aktuelle Raum ist gezeichnet (Op-Liste des Kartenschirms) ===\n");
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        int rx, ry, rw, rh, ru, rv, gx, gz, punkte = 0, rot = 0, nops = 0, o, x, y, bemalt = 0, deck = 0;
        int16_t mx = 0, my = 0;
        int32_t px = 0, pz = 0;
        int band;
        if (!z || z->etage || z->rect == 255) continue;
        if (z->room == 0x5090 || z->room == 0x5091) continue;          /* U-12 (r42/rest) */
        if (!re15_map_rect_geometry(z->page, z->rect, &rx, &ry, &rw, &rh)) continue;
        if (!re15_map_rect_uv(z->page, z->rect, &ru, &rv)) continue;
        /* Etagen-Raeume (s_map_floors): das Rechteck ist nur auf SEINEM Band aktuell (re15_map_rect_state,
         * Zweig hat_etage). Band suchen, dessen Etagenzeile genau dieses Rechteck nennt; hat der Raum
         * Etagenzeilen, aber keine fuer dieses Rechteck, zeigt das Spiel ihn hier nie als aktuell. */
        {
            int b, fp, fr, hat = 0;
            band = -1;
            for (b = 0; b < 32; b++) {
                if (!re15_map_floor_lookup(z->room, z->idx, b, &fp, &fr)) continue;
                hat = 1;
                if (fp == (int)z->page && fr == (int)z->rect) { band = b; break; }
            }
            if (hat && band < 0) continue;
        }
        /* bis zu 9x9 Rasterpunkte der Zone, nur wo die Zone selbst die aktuelle ist */
        for (gx = 0; gx < 9; gx++) for (gz = 0; gz < 9; gz++) {
            int32_t qx = z->wx0 + (int32_t)((int64_t)(z->wx1 - z->wx0) * (2 * gx + 1) / 18);
            int32_t qz = z->wz0 + (int32_t)((int64_t)(z->wz1 - z->wz0) * (2 * gz + 1) / 18);
            const re15_map_zone_t *zc;
            int16_t ax = 0, ay = 0;
            int treffer = 0;
            re15_map_visited_reset();
            re15_collision_reset_band();
            if (band >= 0) re15_collision_set_band(band);
            g_current_room_id = z->room;
            g_actors[RE15_ACTOR_SLOT_PLAYER].x = qx;
            g_actors[RE15_ACTOR_SLOT_PLAYER].z = qz;
            re15_map_zone_update(z->room, qx, qz);
            zc = re15_map_zone_current();
            if (zc != z) continue;
            memset(&g_inv_screen, 0, sizeof g_inv_screen);
            g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
            g_inv_screen.map_page = (uint8_t)z->page;
            re15_inv_map_marker(qx, qz, 0, &ax, &ay);
            nops = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
            for (o = 0; o < nops; o++) {
                const re15_inv_op_t *q = &ops[o];
                if (!ist_aktuell(q)) continue;
                if (ax >= q->x - 1 && ax <= q->x + q->w && ay >= q->y - 1 && ay <= q->y + q->h) treffer = 1;
            }
            punkte++;
            if (treffer) rot++;
            if (punkte == 1) { px = qx; pz = qz; mx = ax; my = ay; }
        }
        if (punkte == 0) continue;
        geprueft++;
        if (z->page >= 8) s456++;
        /* [J] Deckung der gemalten Texel durch Aktuell-Ops, Figur am ersten Rasterpunkt */
        re15_map_visited_reset();
        re15_collision_reset_band();
        if (band >= 0) re15_collision_set_band(band);
        g_current_room_id = z->room;
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = px;
        g_actors[RE15_ACTOR_SLOT_PLAYER].z = pz;
        re15_map_zone_update(z->room, px, pz);
        memset(&g_inv_screen, 0, sizeof g_inv_screen);
        g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
        g_inv_screen.map_page = (uint8_t)z->page;
        nops = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
        memset(gedeckt, 0, sizeof gedeckt);
        for (o = 0; o < nops; o++) {
            const re15_inv_op_t *q = &ops[o];
            if (getenv("RE15_R42_SICHT_DUMP") && strtoul(getenv("RE15_R42_SICHT_DUMP"), 0, 16) == z->room)
                printf("      op %4d k%d pg%d clut%2d (%d,%d) %dx%d uv(%d,%d) rgb(%d,%d,%d)%s\n", o, q->kind, q->page,
                       q->clut, q->x, q->y, q->w, q->h, q->u, q->v, q->r, q->g, q->b, ist_aktuell(q) ? " AKTUELL" : "");
            /* [J] zaehlt auch die deckende Wandlinie der Runde-39-Korrekturen (karte_r39.c s_korr: FILL
             * RE15_KARTE_WAND 0x5AD6 = TEX.TIM @0x055C, (176,176,176)) - sie ersetzt Kachel-Texel, z.B. ROOM1130
             * Blatt 4 r4 Spalten 1/9; in jedem Zustand dieselbe Farbe, also KEIN Beleg fuer "aktuell". */
            if (!ist_aktuell(q) && !(q->kind == RE15_INV_OP_FILL && q->r == 176 && q->g == 176 && q->b == 176))
                continue;
            for (y = q->y; y < q->y + q->h; y++) for (x = q->x; x < q->x + q->w; x++)
                if (x >= 0 && x < 512 && y >= 0 && y < 256) gedeckt[y][x] = 1;
        }
        for (y = 0; y < rh; y++) for (x = 0; x < rw; x++) {
            if (!texel(z->page, ru + x, rv + y)) continue;
            bemalt++;
            if (rx + x < 512 && ry + y < 256 && gedeckt[ry + y][rx + x]) deck++;
        }
        if (rot * 2 < punkte) {
            char e[64]; i_fehlt++;
            snprintf(e, sizeof e, " %04X.z%d(B%d r%d %d/%d)", z->room, z->idx, z->page, z->rect, rot, punkte);
            strncat(li, e, sizeof li - strlen(li) - 1);
        }
        if (!teil_besitzer(z) && bemalt > 0 && deck * 10 < bemalt * 9) {
            char e[64]; j_fehlt++;
            snprintf(e, sizeof e, " %04X.z%d(B%d r%d %d/%d)", z->room, z->idx, z->page, z->rect, deck, bemalt);
            strncat(lj, e, sizeof lj - strlen(lj) - 1);
        }
        if (z->page >= 8)
            printf("    %04X.z%d Blatt %2u r%-2u: Marker auf Rot %2d/%2d, gemalte Texel rot %4d/%4d (Marker %d,%d)\n",
                   z->room, z->idx, z->page, z->rect, rot, punkte, deck, bemalt, mx, my);
    }
    /* [K] Gegenrichtung auf demselben Rechteck: im Zug 5090 nach einem Besuch im T-Gang 5070 (Blatt 9 r7).
     * [L] (Stufe 3 Runde 3, Pruefbericht pruef_s3_r2 ROT 1): KEIN Pixel eines Aktuell-Teils liegt auf einem
     *     anders gefaerbten Stueck DESSELBEN Rechtecks. Die Aktuell-Zeile 502 traegt 680808 MIT STP (RE2
     *     `addiu s5,s5,1` @0x8006E648) - auf dem gruenen Grund des T-Gangs wurde der rote Zugteil braun
     *     (56,32,24) statt (48,8,48) wie auf aaea5f8c2 und wie die Teile von ROOM1110 (Blatt 3 r5). Fuer
     *     jeden der fuenf Zugteile (Mitte seiner Weltbox, s_map_teile) und fuer 1110 (drei Teile), je ohne
     *     und mit besuchtem Nachbarn auf demselben Rechteck. Dazu: die gemalten Texel des Rechtecks sind
     *     zu >= 90 % gezeichnet (rot oder gruen) - der T-Gang bleibt also sichtbar (Stufe 2 Runde 2).
     *     Vorher [K] verlangte ein GANZES gruenes Rechteck unter dem roten Teil - genau der Defekt.
     *     Gegenprobe: aaea5f8c2 ROT (T-Gang nicht gezeichnet, Deckung), 821a8b120 ROT (Ueberlappung). */
    {
        static const struct { unsigned room, nach; int page, rect, nach_x, nach_z; } fall[] = {
            { 0x5090, 0x5070, 9, 7, -14000, -6000 },
            { 0x5090, 0x5130, 9, 7, -14000, -6000 },
            { 0x5090, 0,      9, 7, 0, 0 },
            { 0x1110, 0,      3, 5, 0, 0 },
        };
        static unsigned char farbe[256][512];
        int f, k, l_fehl = 0, l_fall = 0, k_ok = 1;
        char ll[2048] = "";
        for (f = 0; f < (int)(sizeof fall / sizeof fall[0]); f++)
        for (k = 0; k < re15_map_teil_count((unsigned)fall[f].page, (unsigned)fall[f].rect); k++) {
            int tx, ty, tw, th, ts, nops, o, x, y, rx, ry, rw, rh, ru, rv, ueber = 0, bemalt = 0, deck = 0, rot_op = -1, gruen_op = -1;
            int32_t qx, qz;
            /* Mitte der Weltbox des k-ten Teils (re15_map_zones.h s_map_teile, gleiche Reihenfolge wie
             * re15_map_teil_get; hier abgeschrieben, damit der Riegel auch auf aaea5f8c2 baut) */
            static const int32_t welt9[5][4] = {
                {  -5683, -13414, 26019,  -4716 }, { 20532,    850, 29067, 11288 },
                {  20532, -26982, 29067, -18632 }, { 20532,  -3672, 26019,   155 },
                {  20532, -17589, 26019, -14458 } };
            static const int32_t welt3[3][4] = {
                {  -2909,  -7522, 14950,   7087 }, { -13521, -3806, -3686, 1320 },
                { -13521,   1961, -3686,   7087 } };
            const int32_t *w = (fall[f].page == 9) ? (k < 5 ? welt9[k] : 0) : (k < 3 ? welt3[k] : 0);
            if (!w) continue;
            qx = (w[0] + w[2]) / 2; qz = (w[1] + w[3]) / 2;
            re15_map_visited_reset();
            re15_collision_reset_band();
            if (fall[f].nach) re15_map_visited_mark_at(fall[f].nach, fall[f].nach_x, fall[f].nach_z);
            if (fall[f].nach == 0x5130) re15_map_visited_mark_at(0x5070, -14000, -6000);
            g_current_room_id = fall[f].room;
            g_actors[RE15_ACTOR_SLOT_PLAYER].x = qx;
            g_actors[RE15_ACTOR_SLOT_PLAYER].z = qz;
            re15_map_zone_update(fall[f].room, qx, qz);
            re15_map_teil_get((unsigned)fall[f].page, (unsigned)fall[f].rect, k, &tx, &ty, &tw, &th, &ts);
            if (ts != RE15_MAP_RECT_CURRENT) continue;   /* Mitte ausserhalb des Teils (Rand) - nicht messbar */
            memset(&g_inv_screen, 0, sizeof g_inv_screen);
            g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
            g_inv_screen.map_page = (uint8_t)fall[f].page;
            re15_map_rect_geometry((unsigned)fall[f].page, (unsigned)fall[f].rect, &rx, &ry, &rw, &rh);
            re15_map_rect_uv((unsigned)fall[f].page, (unsigned)fall[f].rect, &ru, &rv);
            nops = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
            memset(farbe, 0, sizeof farbe);
            for (o = 0; o < nops; o++) {
                const re15_inv_op_t *q = &ops[o];
                int bit;
                if (q->kind != RE15_INV_OP_SPRT || q->page != RE15_INV_PAGE_MAP4) continue;
                if (q->x < rx || q->y < ry || q->x + q->w > rx + rw || q->y + q->h > ry + rh) continue;
                /* nur Stuecke DIESER Kachel (uv im Kachel-Fenster, gleicher Versatz) */
                if (q->u - ru != q->x - rx || q->v - rv != q->y - ry) continue;
                bit = (q->clut == RE15_INV_CLUT_MAP_AKTUELL) ? 1 : 2;
                if (bit == 1 && rot_op < 0) rot_op = o;
                if (bit == 2) gruen_op = o;
                for (y = q->y; y < q->y + q->h; y++) for (x = q->x; x < q->x + q->w; x++) farbe[y][x] |= (unsigned char)bit;
            }
            for (y = 0; y < rh; y++) for (x = 0; x < rw; x++) {
                if (farbe[ry + y][rx + x] == 3) ueber++;
                if (!texel(fall[f].page, ru + x, rv + y)) continue;
                bemalt++;
                if (farbe[ry + y][rx + x]) deck++;
            }
            l_fall++;
            if (ueber > 0 || rot_op < 0 || (fall[f].nach && deck * 10 < bemalt * 9)) {
                char e[96]; l_fehl++;
                snprintf(e, sizeof e, " %04X-Teil%d(nach %04X: %d px rot auf anderem Stueck, gezeichnet %d/%d)",
                         fall[f].room, k, fall[f].nach, ueber, deck, bemalt);
                strncat(ll, e, sizeof ll - strlen(ll) - 1);
            }
            /* [K] 5090 Teil 0 nach 5070: rot liegt OBEN (frueher eingetragen) ueber dem gruenen T-Gang */
            if (f == 0 && k == 0) {
                printf("  [K] im Zug 5090 nach 5070: 5090-Teil rot Op %d, T-Gang gruen bis Op %d\n", rot_op, gruen_op);
                k_ok = rot_op >= 0 && gruen_op >= 0 && rot_op < gruen_op;
            }
        }
        printf("  [L] %d Faelle (Zugteile 5090, Teile 1110):%s\n", l_fall, ll[0] ? ll : " kein Pixel rot auf anderem Stueck");
        CHECK("[K] im Zug 5090 bleibt sein Teil rot und liegt ueber dem gruenen T-Gang", k_ok);
        CHECK("[L] genug Teil-Faelle gemessen", l_fall >= 12);
        CHECK("[L] ein aktueller Teil liegt auf keinem anders gefaerbten Stueck seines Rechtecks (STP-Rot bleibt rot)",
              l_fehl == 0);
    }
    re15_map_visited_reset();
    re15_collision_reset_band();
    printf("  %d Zonen geprueft (davon %d auf Blatt 8-12)\n", geprueft, s456);
    printf("  [I] Marker nicht auf Rot (weniger als die Haelfte der Punkte):%s\n", li[0] ? li : " keine");
    printf("  [J] eigenes Rechteck nicht ganz rot (< 90 %% der gemalten Texel):%s\n", lj[0] ? lj : " keine");
    CHECK("genug Zonen geprueft (alle Blaetter)", geprueft > 150 && s456 > 50);
    CHECK("[I] in jeder Zone steht der Marker auf dem rot gezeichneten Raum", i_fehlt == 0);
    CHECK("[J] das Rechteck des aktuellen Raums ist ganz gezeichnet", j_fehlt == 0);
    printf(g_fail ? "\nFEHLER\n" : "\nOK\n");
    return g_fail;
}
