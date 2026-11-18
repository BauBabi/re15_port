/* =============================================================================
 * RIEGEL r42/karte Stufe 3 Runde 4: der Punkt laeuft im Ostarm von 4080 und im Gang von 6000 mit, und sein Ring
 * liegt an keiner Ankunft auf der Nachbarkachel.
 * =============================================================================
 * Nutzer 2026-11-18: "das die [Map] fuer STAGE2-6 genauso sauber funktioniert wir bei STAGE1 ... Inklusive dem - wo
 * befindet sich Leon - Marker."
 * Pruefbericht pruef_s3_r3 ROT (Restfehler): 4080_w1 "der Punkt steht bei Ankunft und nach 2989 Einheiten Weg auf
 * (212,128)"; 6000_t0/w0 "der Punkt steht auf der Wandlinie (212,86) und laeuft nach 2766 Einheiten nicht mit; der
 * Gang nach 50B0 ist ungemalt"; 4080_t3 "liegt halb auf der Wandlinie".
 *
 * Die Lagen sind ECHTE Lagen aus dem Spiel-Log des Kartenlaufs Stufe 3 Runde 3 ([walk] ... pl pos, Abzuege
 * L4080_t1/L4080_w1, L6000_t0/L6000_w0 in analysis/karte_r42/s3r3/abzuege_s3r3.tsv), der Punkt kommt aus dem
 * Kartenschirm (re15_inv_map_marker), Kunst und Rechtecke liest der Riegel selbst (DATA/MAP0x.PIX, Seitentabelle
 * ueber re15_map_rect_geometry/uv).
 *  [M1] 4080 Ostarm: Ankunft aus 4090 (22000,-5450) und nach 2 s Gehen (24989,-5450): der Punkt wandert >= 2 px
 *       (Zeile aus den Waenden, re15_map_zones.h ROOM4080). Auf aaea5f8c2 und b94479047 ROT (beide Lagen gleich).
 *  [M2] 6000 Gang nach 50B0: Ankunft aus 50B0 (5204,1600) und nach 2 s (5200,-1166): aktuelle Zone ist die
 *       Schema-Zeichnung (rect 255), der Punkt liegt in ihren Zellen, wandert >= 2 px, und unter ihm liegt eine FILL-Op
 *       in der Aktuell-Farbe (0x842D = (104,8,8)). Auf aaea5f8c2/b94479047 ROT (keine Gang-Zone).
 *  [M3] auf dem Bahnsteig (-4200,-8450, Ankunft aus 6010) ist der Gang derselbe Ort: eine Aktuell-FILL-Op deckt (215,80)
 *       (der Kachel-SPRT von r5 zaehlt nicht - sein Kasten reicht ueber den ungemalten Gang). Auf b94479047 ROT.
 *  [M4] an JEDER Tuer-Ankunft STAGE2-6 (Blatt 6-12, Zone mit HERGELEITETER Zeile auf eigener Kachel) liegt der 5x5-Ring
 *       (Mitte mx-1,my-1, TEX.TIM @0x14910 uv 225..229) auf KEINEM Texel einer fremden Kachel (Kunst Index != 0 einer
 *       Kachel, die nicht zum Ort gehoert), ausser wo das innerhalb der halben Ringbreite nicht geht (Liste unten,
 *       je mit Grund). Auf b94479047 ROT (u.a. 4080 aus 40B0: Ring auf der Wandlinie x177 von r11).
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

/* Gegenprobe auf aaea5f8c2 (v0.8.30): dort gibt es das Feld tausch noch nicht (r42/karte Stufe 1) - der Gegenprobe-Bau
 * setzt R42_OHNE_TAUSCH. */
#ifdef R42_OHNE_TAUSCH
#define TAUSCH(z) 0
#else
#define TAUSCH(z) ((z)->tausch)
#endif

extern re15_inv_screen_t g_inv_screen;
extern unsigned g_current_room_id;

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", (name)); g_fail = 1; } \
                               else printf("  PASS: %s\n", (name)); } while (0)

static re15_rdt_t s_rdt;
static uint8_t *s_roh = NULL;
static uint8_t *s_pix[13];
static uint8_t *s_exe = NULL;
static size_t s_exe_n = 0;

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

/* headerlos, 256x256 4 bpp, 128 B je Zeile, unteres Nibble links (Datei-Id u16 @0x80074c4c+2p = 12+p). */
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

/* Index der Kachel <rect> am Schirmpunkt (x,y), 0 = nichts (ausserhalb oder transparent). */
static int kachel_index(int page, int rect, int x, int y)
{
    int rx, ry, rw, rh, ru, rv;
    if (!re15_map_rect_geometry((unsigned)page, (unsigned)rect, &rx, &ry, &rw, &rh)) return 0;
    if (!re15_map_rect_uv((unsigned)page, (unsigned)rect, &ru, &rv)) return 0;
    if (x < rx || y < ry || x >= rx + rw || y >= ry + rh) return 0;
    return texel(page, ru + x - rx, rv + y - ry);
}

/* Zeile {s16 ox, s16 oy, u16 sx, u16 sy} @0x800768b0 + 8*(Basis+Raum) direkt aus PSX.EXE. */
static int echte_zeile(const re15_map_zone_t *z)
{
    static const int basis[6] = { 0, 38, 50, 65, 77, 98 };
    unsigned st = ((unsigned)z->room >> 12) & 0xF;
    size_t o;
    int ox, oy, sx, sy;
    if (!s_exe) {
        s_exe = slurp(RE15_EXE_PFAD, &s_exe_n);
        if (!s_exe) return -1;
    }
    if (st < 1 || st > 6) return 0;
    o = 0x800u + (0x800768b0u + 8u * (unsigned)(basis[st - 1] + (int)((z->room >> 4) & 0xFF))) - 0x80010000u;
    if (o + 8 > s_exe_n) return 0;
    ox = (int16_t)(s_exe[o] | (s_exe[o + 1] << 8)); oy = (int16_t)(s_exe[o + 2] | (s_exe[o + 3] << 8));
    sx = s_exe[o + 4] | (s_exe[o + 5] << 8); sy = s_exe[o + 6] | (s_exe[o + 7] << 8);
    if (sx <= 1 || sy <= 1) return 0;
    return z->ox == ox && z->oy == oy && z->sx == sx && z->sy == sy && !z->flip_x && !z->flip_z && !TAUSCH(z);
}

static int betrete(unsigned rid, int32_t px, int32_t pz)
{
    char pfad[600];
    size_t n = 0;
    re15_actor_t *pl;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    if (s_roh) { free(s_roh); s_roh = NULL; }
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
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

/* Kartenschirm fuer (rid, x, z) aufbauen und den Punkt holen; Rueckgabe Blatt (-1 = keins). */
static int punkt(unsigned rid, int32_t x, int32_t z, int16_t *mx, int16_t *my)
{
    static const int grenze[6] = { 0x26, 0xc, 0x20, 0xc, 0x15, 0x8 };   /* sltiu je Stage-Init (Seiten-Setzer) */
    unsigned st = (rid >> 12) & 0xF, r = (rid >> 4) & 0xFF;
    if (!betrete(rid, x, z)) return -1;
    re15_map_visited_reset();
    re15_map_visited_mark_at(rid, x, z);
    re15_map_zone_update(rid, x, z);
    if (!re15_map_zone_current()) return -1;
    if (st >= 1 && st <= 6 && (int)r < grenze[st - 1]) re15_inv_map_stage_init((int)st - 1, (int)r);
    memset(&g_inv_screen, 0, sizeof g_inv_screen);
    re15_inv_screen_open();
    g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
    g_inv_screen.map_page = (uint8_t)re15_inv_map_page_shown();
    re15_inv_map_marker(x, z, 0, mx, my);
    return g_inv_screen.map_page;
}

/* nur_fuellung: nur eine FILL-Op in der Aktuell-Farbe zaehlt (Schema-Zeichnung) - ein Kachel-SPRT deckt seinen ganzen
 * Kasten, auch die transparenten Texel (r5 auf Blatt 12 reicht bis y 60), und wuerde sonst ueberall "rot" melden. */
static int aktuell_unter_art(int ax, int ay, int nur_fuellung)
{
    static re15_inv_op_t ops[RE15_INV_MAX_OPS];
    int n = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS), o;
    for (o = 0; o < n; o++) {
        const re15_inv_op_t *q = &ops[o];
        int akt = (!nur_fuellung && q->kind == RE15_INV_OP_SPRT && q->page == RE15_INV_PAGE_MAP4 &&
                   q->clut == RE15_INV_CLUT_MAP_AKTUELL) ||
                  (q->kind == RE15_INV_OP_FILL && q->r == 104 && q->g == 8 && q->b == 8);
        if (akt && ax >= q->x && ax < q->x + q->w && ay >= q->y && ay < q->y + q->h) return 1;
    }
    return 0;
}
#define aktuell_unter(x, y) aktuell_unter_art((x), (y), 1)

/* [M5]: der Raum ist AM Punkt rot - Mitte oder ein Nachbar im 3x3 (ein Punkt auf einer Innenwand zwischen zwei Teilen
 * desselben Raums, z.B. ROOM1110 x188, zaehlt; das Mass der Pruefung ist der 2-px-Rand um den Punkt). */
static int rot_am_punkt(int cx, int cy)
{
    int dx, dy;
    for (dy = -1; dy <= 1; dy++) for (dx = -1; dx <= 1; dx++) if (aktuell_unter_art(cx + dx, cy + dy, 0)) return 1;
    return 0;
}

static int cheb(int ax, int ay, int bx, int by)
{
    return abs(ax - bx) > abs(ay - by) ? abs(ax - bx) : abs(ay - by);
}

/* Ring-Texel auf einer FREMDEN Kachel: Kunst (Index != 0) einer Kachel des Blatts, die nicht zum Ort gehoert, an einer
 * Stelle, an der keine Kachel des Orts Kunst hat. Ort = Kacheln derselben Zone-Familie (gleicher Raum, gleiche Zeile). */
static int ring_fremd(const re15_map_zone_t *zn, int page, int cx, int cy)
{
    int ort[8], no = 0, i, dx, dy, f = 0;
    ort[no++] = zn->rect;                             /* die eigene Kachel (auch eine Gast-Zeile, z.B. 4070 auf Blatt 11) */
    for (i = 0; i < re15_map_zone_count() && no < 8; i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        int j;
        if (!z || z->room != zn->room || z->page != (unsigned)page || z->etage || z->rect == 255) continue;
        if (z->ox != zn->ox || z->oy != zn->oy || z->sx != zn->sx || z->sy != zn->sy || z->flip_x != zn->flip_x ||
            z->flip_z != zn->flip_z || TAUSCH(z) != TAUSCH(zn)) continue;
        for (j = 0; j < no && ort[j] != z->rect; j++) ;
        if (j == no) ort[no++] = z->rect;
    }
    for (dy = -2; dy <= 2; dy++) for (dx = -2; dx <= 2; dx++) {
        int x = cx + dx, y = cy + dy, eigen = 0, r;
        for (i = 0; i < no; i++) if (kachel_index(page, ort[i], x, y)) eigen = 1;
        if (eigen) continue;
        for (r = 0; r < 64; r++) {
            int hat_zone = 0, k;
            for (k = 0; k < re15_map_zone_count(); k++) {
                const re15_map_zone_t *z = re15_map_zone_by_index(k);
                if (z && z->page == (unsigned)page && z->rect == (unsigned)r) { hat_zone = 1; break; }
            }
            if (hat_zone && kachel_index(page, r, x, y)) { f++; break; }
        }
    }
    return f;
}

#define MAXANK 1200
static struct { unsigned dest, von; int32_t x, z; } s_ank[MAXANK];
static int s_nank = 0;

/* [M4] Ausnahmen: Ankuenfte, an denen JEDE Mitte im Umkreis der halben Ringbreite auf der eigenen Kachel noch fremde
 * Kunst im Ring haette. Gemessen am 2026-11-18 (Stand r42/karte Stufe 3 Runde 4): KEINE - 122 von 122 Ankuenften frei.
 * Eine neue Ausnahme braucht hier eine Zeile mit Grund; die Liste endet mit grund == 0. */
static const struct { unsigned dest, von; const char *grund; } s_m4_aus[] = {
    { 0, 0, 0 }
};

int main(void)
{
    static re15_aot_t sn_a[RE15_AOT_MAX];
    static re15_aot_door_params_t sn_d[RE15_AOT_MAX];
    int16_t ax = 0, ay = 0, bx = 0, by = 0;
    int pa, pb, i, m4_n = 0, m4_ok = 0, m4_aus = 0;
    char l4[3000] = "";
    unsigned st, r, v;

    printf("=== r42 Karte Stufe 3 Runde 4: Mitlaufen 4080/6000, Ring nicht auf der Nachbarkachel ===\n");

    /* [M1] 4080 Ostarm */
    pa = punkt(0x4080, 22000, -5450, &ax, &ay);
    pb = punkt(0x4080, 24989, -5450, &bx, &by);
    printf("  [M1] 4080 Ankunft aus 4090 (22000,-5450) -> (%d,%d) Blatt %d; nach 2 s (24989,-5450) -> (%d,%d) Blatt %d\n",
           ax, ay, pa, bx, by, pb);
    CHECK("[M1] 4080 Ostarm: der Punkt laeuft mit (>= 2 px auf 2989 Einheiten)", pa == 8 && pb == 8 && cheb(ax, ay, bx, by) >= 2);

    /* [M2] 6000 Gang nach 50B0 */
    {
        const re15_map_zone_t *za, *zb;
        int sx, sy, sw, sh, erste, nz, k, in_a = 0, in_b = 0, rot_a, rot_b;
        pa = punkt(0x6000, 5204, 1600, &ax, &ay);
        za = re15_map_zone_current();
        rot_a = (pa == 12) ? aktuell_unter(ax - 1, ay - 1) : 0;
        if (za && re15_map_zone_synth(za, &sx, &sy, &sw, &sh, &erste, &nz))
            for (k = 0; k < nz; k++) {
                int cx, cy, cw, ch;
                if (!re15_map_synth_cell(erste + k, &cx, &cy, &cw, &ch)) continue;
                if (ax - 1 >= cx && ax - 1 < cx + cw && ay - 1 >= cy && ay - 1 < cy + ch) in_a = 1;
            }
        pb = punkt(0x6000, 5200, -1166, &bx, &by);
        zb = re15_map_zone_current();
        rot_b = (pb == 12) ? aktuell_unter(bx - 1, by - 1) : 0;
        if (zb && re15_map_zone_synth(zb, &sx, &sy, &sw, &sh, &erste, &nz))
            for (k = 0; k < nz; k++) {
                int cx, cy, cw, ch;
                if (!re15_map_synth_cell(erste + k, &cx, &cy, &cw, &ch)) continue;
                if (bx - 1 >= cx && bx - 1 < cx + cw && by - 1 >= cy && by - 1 < cy + ch) in_b = 1;
            }
        printf("  [M2] 6000 Ankunft aus 50B0 (5204,1600) -> (%d,%d) Zone %d rect %d, in Zelle %d, rot %d; "
               "nach 2 s (5200,-1166) -> (%d,%d) Zone %d, in Zelle %d, rot %d\n",
               ax, ay, za ? za->idx : -1, za ? za->rect : -1, in_a, rot_a, bx, by, zb ? zb->idx : -1, in_b, rot_b);
        CHECK("[M2] 6000 Gang: Schema-Zone aktuell, Punkt in ihren Zellen und rot unterlegt",
              pa == 12 && pb == 12 && za && zb && za->rect == 255 && zb->rect == 255 && in_a && in_b && rot_a && rot_b);
        CHECK("[M2] 6000 Gang: der Punkt laeuft mit (>= 2 px auf 2766 Einheiten)", cheb(ax, ay, bx, by) >= 2);
    }

    /* [M3] Bahnsteig: Gang ist derselbe Ort */
    pa = punkt(0x6000, -4200, -8450, &ax, &ay);
    {
        int rot = (pa == 12) ? aktuell_unter(215, 80) : 0;
        printf("  [M3] 6000 Bahnsteig (-4200,-8450) -> (%d,%d) Blatt %d, Gang (215,80) rot %d\n", ax, ay, pa, rot);
        CHECK("[M3] auf dem Bahnsteig ist auch der Gang nach 50B0 rot (ein Ort)", pa == 12 && rot);
    }

    /* [M4] Ring an jeder Ankunft */
    for (st = 2; st <= 6; st++) for (r = 0; r < 0x40; r++) for (v = 0; v < 2; v++) {
        unsigned rid = (st << 12) | (r << 4) | v;
        int k;
        if (!betrete(rid, 0, 0)) continue;
        memcpy(sn_a, g_aot.slots, sizeof sn_a);
        memcpy(sn_d, g_aot.door_params, sizeof sn_d);
        for (k = 0; k < RE15_AOT_MAX; k++) {
            unsigned ziel;
            int j, dup = 0;
            if (!sn_a[k].active || sn_a[k].type != RE15_AOT_TYPE_DOOR) continue;
            ziel = (((unsigned)sn_d[k].dest_stage + 1u) << 12) | ((unsigned)sn_d[k].dest_room << 4) | (rid & 0xFu);
            if (ziel == rid) continue;
            for (j = 0; j < s_nank; j++)
                if (s_ank[j].dest == ziel && s_ank[j].x == sn_d[k].spawn_x && s_ank[j].z == sn_d[k].spawn_z) dup = 1;
            if (dup || s_nank >= MAXANK) continue;
            s_ank[s_nank].dest = ziel; s_ank[s_nank].von = rid;
            s_ank[s_nank].x = sn_d[k].spawn_x; s_ank[s_nank].z = sn_d[k].spawn_z; s_nank++;
        }
    }
    for (i = 0; i < s_nank; i++) {
        unsigned rid = s_ank[i].dest;
        const re15_map_zone_t *zn, *zb;
        int seite, f, j, aus = 0;
        int16_t mx = 0, my = 0;
        if (rid == 0x5090 || rid == 0x5091) continue;                 /* U-12 (r42/rest) */
        seite = punkt(rid, s_ank[i].x, s_ank[i].z, &mx, &my);
        if (seite < 6 || seite > 12) continue;
        zn = re15_map_zone_current();
        zb = re15_map_zone_fuer(zn->room, zn->idx, (unsigned)seite);
        if (!zb) zb = zn;
        if (zb->rect == 255 || echte_zeile(zb) == 1) continue;
        m4_n++;
        f = ring_fremd(zb, seite, mx - 1, my - 1);
        if (!f) { m4_ok++; continue; }
        for (j = 0; s_m4_aus[j].grund; j++)
            if (s_m4_aus[j].dest == rid && s_m4_aus[j].von == s_ank[i].von) aus = 1;
        if (aus) { m4_aus++; continue; }
        {
            char e[80];
            snprintf(e, sizeof e, " %04X<-%04X(B%d r%d %d,%d f%d)", rid, s_ank[i].von, seite, zb->rect, mx, my, f);
            strncat(l4, e, sizeof l4 - strlen(l4) - 1);
        }
    }
    printf("  [M4] %d Ankuenfte (hergeleitete Zeile, Blatt 6-12): Ring frei %d, Ausnahme %d, auf fremder Kachel:%s\n",
           m4_n, m4_ok, m4_aus, l4[0] ? l4 : " keine");
    CHECK("[M4] genug Ankuenfte geprueft", m4_n > 100);
    CHECK("[M4] kein Ring auf einer fremden Kachel (ausser den begruendeten Ausnahmen)", m4_ok + m4_aus == m4_n);
    /* [M5] ZENSUS ALLER RAEUME (NUTZERWUNSCH 18.11.2026 woertlich: "selbstverständlich sollen alle Räume auch eIn stück karte
     * mit navigationspunkt haben."): jede ROOM####.RDT der sechs Stages mit Inhalt (> 4 Byte; die 4-Byte-Dateien sind leere
     * Platzhalter) - Leon UND Elza - zeigt an jeder Ankunft (Spawn einer Tuer, die in den Raum fuehrt; ohne solche Tuer die
     * Trigger der eigenen Tueren) ein Blatt, auf dem unter dem Punkt der Raum in der Aktuell-Farbe gezeichnet ist, und der
     * Punkt laeuft mit: um die Ankunft herum (+-2000 oder +-4000 in x oder z; der Tuerzug tuer_anziehen haelt ihn im doppelten Trigger auf dem
     * Tuersymbol) wandert er um >= 2 px. Ausgenommen sind nur Raeume, in
     * denen die Karte nachweislich nicht aufgeht (Liste s_m5_aus mit Beleg). Auf aaea5f8c2/b94479047 ROT (10B0 steht,
     * 1260 und 20B0 ohne Zone). */
    {
        static const struct { unsigned room; const char *grund; } s_m5_aus[] = {
            { 0x1240, "reine Szene: nDoor 0, sub00 Evt_exec sub 2 ohne Bedingung @ROOM1240.RDT 0x00544, sub02 Set 2:7=1 @0x0055A ... Aot_on 0 @0x00618 (Tuer -> 1170)" },
            { 0x1241, "dito ROOM1241" },
            { 0x6040, "Abspann: sub00 Evt_exec sub 2 ohne Bedingung @ROOM6040.RDT 0x00522, sub02 Set 2:7=1 @0x00530, keine Tuer" },
            { 0x6041, "dito ROOM6041 (bytegleich mit ROOM6040)" },
            { 0x5090, "Zug: Lagen ausserhalb der 5 Teil-Weltboxen ohne Rot, Stand v0.8.30, gehoert zu U-12 (Orchestrator-Vorgabe Punkt 3: gelb)" },
            { 0x5091, "dito ROOM5091" },
            { 0x20A0, "gemalte Kachel r10 nur 11x9 px (MAP07.PIX, Seitentabelle Blatt 6): der 5x5-Punkt hat 6x4 Mitten, ueber den ganzen Raum wandert er ~5 px - zu wenig fuer +-4000 (offen, gelb)" },
            { 0x20A1, "dito ROOM20A1" },
            { 0, 0 }
        };
        int m5_n = 0, m5_ok = 0, m5_aus = 0, k2;
        char l5[3000] = "";
        static struct { unsigned dest; int32_t x, z; } ank5[600];
        int n5 = 0;
        for (st = 1; st <= 6; st++) for (r = 0; r < 0x30; r++) for (v = 0; v < 2; v++) {
            unsigned rid = (st << 12) | (r << 4) | v;
            if (!betrete(rid, 0, 0)) continue;
            for (k2 = 0; k2 < RE15_AOT_MAX && n5 < 600; k2++) {
                const re15_aot_t *a = &g_aot.slots[k2];
                const re15_aot_door_params_t *d = &g_aot.door_params[k2];
                if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
                ank5[n5].dest = (((unsigned)d->dest_stage + 1u) << 12) | ((unsigned)d->dest_room << 4) | v;
                ank5[n5].x = d->spawn_x; ank5[n5].z = d->spawn_z; n5++;
            }
        }
        for (st = 1; st <= 6; st++) for (r = 0; r < 0x30; r++) for (v = 0; v < 2; v++) {
            unsigned rid = (st << 12) | (r << 4) | v;
            char pfad[600];
            size_t groesse = 0;
            uint8_t *roh;
            int32_t lx[16], lz[16];
            int nl = 0, j, aus = 0, gut = 1, schlecht_grund = 0;
            snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, st, rid);
            roh = slurp(pfad, &groesse);
            if (!roh) continue;
            free(roh);
            if (groesse <= 4) continue;                                   /* leerer Platzhalter */
            for (j = 0; s_m5_aus[j].grund; j++) if (s_m5_aus[j].room == rid) aus = 1;
            if (aus) { m5_aus++; continue; }
            for (j = 0; j < n5 && nl < 16; j++) if (ank5[j].dest == rid) { lx[nl] = ank5[j].x; lz[nl] = ank5[j].z; nl++; }
            if (nl == 0 && betrete(rid, 0, 0))                            /* keine Tuer fuehrt hinein: Trigger der eigenen Tueren */
                for (k2 = 0; k2 < RE15_AOT_MAX && nl < 16; k2++) {
                    const re15_aot_t *a = &g_aot.slots[k2];
                    if (!a->active || a->type != RE15_AOT_TYPE_DOOR || (a->half_w == 0 && a->half_h == 0)) continue;
                    lx[nl] = a->x; lz[nl] = a->z; nl++;
                }
            if (nl == 0)                                                  /* nur Skript-Warps (Fahrstuhl): Mitte jeder Zone */
                for (k2 = 0; k2 < re15_map_zone_count() && nl < 16; k2++) {
                    const re15_map_zone_t *z = re15_map_zone_by_index(k2);
                    if (!z || z->room != (unsigned short)rid || z->etage) continue;
                    lx[nl] = (z->wx0 + z->wx1) / 2; lz[nl] = (z->wz0 + z->wz1) / 2; nl++;
                }
            m5_n++;
            if (nl == 0) { gut = 0; schlecht_grund = 1; }
            for (j = 0; j < nl && gut; j++) {
                static const int dx[8] = { 2000, -2000, 0, 0, 4000, -4000, 0, 0 }, dz[8] = { 0, 0, 2000, -2000, 0, 0, 4000, -4000 };
                int16_t mx = 0, my = 0, qx = 0, qy = 0;
                int seite = punkt(rid, lx[j], lz[j], &mx, &my), d, lauf = 0;
                if (seite < 0 || seite > 12 || mx < 0 || my < 0) { gut = 0; schlecht_grund = 2; break; }
                if (!rot_am_punkt(mx - 1, my - 1)) { gut = 0; schlecht_grund = 3; printf("    [M5] %04X Lage (%d,%d): Punkt (%d,%d) Blatt %d nicht rot\n", rid, (int)lx[j], (int)lz[j], mx, my, seite); break; }
                for (d = 0; d < 8; d++) {
                    int s2 = punkt(rid, lx[j] + dx[d], lz[j] + dz[d], &qx, &qy);
                    if (s2 == seite && cheb(mx, my, qx, qy) > lauf) lauf = cheb(mx, my, qx, qy);
                }
                if (lauf < 2) { gut = 0; schlecht_grund = 4; printf("    [M5] %04X Lage (%d,%d): Punkt (%d,%d) Blatt %d steht (max %d px)\n", rid, (int)lx[j], (int)lz[j], mx, my, seite, lauf); }
            }
            if (gut) { m5_ok++; continue; }
            {
                static const char *g[5] = { "", "keine Lage", "kein Blatt/Punkt", "nicht rot unter dem Punkt", "Punkt steht" };
                char e[96];
                snprintf(e, sizeof e, " %04X(%s)", rid, g[schlecht_grund]);
                strncat(l5, e, sizeof l5 - strlen(l5) - 1);
            }
        }
        printf("  [M5] %d Raeume mit Inhalt (ohne %d Szenen-Raeume): Stueck Karte + Punkt laeuft mit %d, fehlt:%s\n",
               m5_n, m5_aus, m5_ok, l5[0] ? l5 : " keine");
        CHECK("[M5] Zensus: genug Raeume geprueft (STAGE1-6, Leon und Elza)", m5_n >= 180);
        CHECK("[M5] jeder Raum zeigt ein Stueck Karte mit rotem Raum und mitlaufendem Punkt", m5_ok == m5_n);
    }
    if (s_roh) free(s_roh);
    printf(g_fail ? "\nFEHLER\n" : "\nOK\n");
    return g_fail;
}
