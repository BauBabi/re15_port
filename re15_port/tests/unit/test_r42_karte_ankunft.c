/* =============================================================================
 * RIEGEL r42/karte Stufe 3 Runde 2: an JEDER ECHTEN ANKUNFTSLAGE steht der Punkt auf der eigenen Kachel.
 * =============================================================================
 * Nutzer 2026-11-18: "das die [Map] fuer STAGE2-6 genauso sauber funktioniert wir bei STAGE1 ...
 * Inklusive dem - wo befindet sich Leon - Marker."
 *
 * Pruefbericht Stufe 3 Runde 1 (pruef_s3_r1/PRUEFUNG.md): die Messlagen waren Mitten von Kamerabereichen,
 * teils Orte, an denen man nicht stehen kann ((31500,31500), (-5000,-6500)); und "richtig" hiess nur
 * "so, wie die Engine rechnet". Dieser Riegel nimmt dagegen nur Lagen, an denen das SPIEL die Figur
 * hinstellt: die Ankunft jeder Tuer (Door_aot_set next_pos, door_params.spawn_x/z), und prueft gegen
 * eine Groesse, die der Marker-Code nicht kennt: die GEMALTEN Texel der eigenen Kachel, direkt aus
 * DATA/MAP0x.PIX gelesen (Seite p -> MAP0<p+1>, Kachel-uv aus re15_map_rect_uv).
 *
 *  [A] Blatt 6..12 (STAGE2-6), Zone mit HERGELEITETER Zeile (Port-Ergaenzung): der Punkt liegt auf einem
 *      Texel des RAUMKOERPERS der eigenen Kachel (Index != 0 und != 4 = nicht auf der Wandlinie). Auf aaea5f8c2 / 6e0f3db61 ROT: 6000 aus 50B0 (8 px ueber r5, der
 *      Gang ist nicht gemalt), 4040 aus 4050, 4050 (2 Lagen), 4080 aus 4090, 11A0 aus 3000 (Blatt 6).
 *  [B] Blatt 6..12, Zone mit AUSGELIEFERTER Zeile (@0x800768b0): nur gezaehlt und gemeldet (byte-true,
 *      der Marker wird dort nicht geklemmt; 2000/3090/4030 liegen an der Tuer 1 px auf der Nachbarkachel).
 *  [C] Blatt 6..12: Zonen DESSELBEN Raums mit DERSELBEN Zeile auf demselben Blatt (4040 r4+r12, 4080 r8+r13,
 *      5140 r10+r12) sind ein Ort - steht die Figur in einer, sind die Kacheln aller aktuell (re15_map_rect_state).
 *      Auf 6e0f3db61 ROT (4080 aus 4030: nur der Streifen r13 rot, Pruefbericht s3 r1).
 * Nicht hier: 5090/5091 (U-12, r42/rest), Raeume ohne Zone (1240/1260/6040, Szenen- bzw. Seitenlose Raeume).
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

static int gemalt(int page, int rect, int x, int y)
{
    int rx, ry, rw, rh, ru, rv;
    if (!re15_map_rect_geometry((unsigned)page, (unsigned)rect, &rx, &ry, &rw, &rh)) return 0;
    if (!re15_map_rect_uv((unsigned)page, (unsigned)rect, &ru, &rv)) return 0;
    if (x < rx || y < ry || x >= rx + rw || y >= ry + rh) return 0;
    {   /* Raumkoerper: Index 0 = transparent, Index 4 = Wandlinie (CLUT 0x5ad6) - auf der Linie steht der Punkt halb draussen */
        int t = texel(page, ru + x - rx, rv + y - ry);
        return t != 0 && t != 4;
    }
}

/* Zeile {s16 ox, s16 oy, u16 sx, u16 sy} @0x800768b0 + 8*(Basis+Raum) DIREKT aus PSX.EXE (Datei-Offset
 * 0x800 + va - 0x80010000), Basen 0/38/50/65/77/98 (Seiten-Setzer FUN_8004b568). */
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
    return z->ox == ox && z->oy == oy && z->sx == sx && z->sy == sy && !z->flip_x && !z->flip_z && !z->tausch;
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

#define MAXANK 1200
static struct { unsigned dest, von; int32_t x, z; } s_ank[MAXANK];
static int s_nank = 0;

int main(void)
{
    static re15_aot_t sn_a[RE15_AOT_MAX];
    static re15_aot_door_params_t sn_d[RE15_AOT_MAX];
    static const int grenze[6] = { 0x26, 0xc, 0x20, 0xc, 0x15, 0x8 };   /* sltiu je Stage-Init (Seiten-Setzer) */
    unsigned st, r, v;
    int i, a_n = 0, a_ok = 0, b_n = 0, b_auf = 0;
    char la[3000] = "", lb[3000] = "";

    printf("=== r42 Karte: Punkt an echten Ankunftslagen auf der eigenen gemalten Kachel ===\n");
    for (st = 1; st <= 6; st++) for (r = 0; r < 0x40; r++) for (v = 0; v < 2; v++) {
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
    printf("  %d Ankunftslagen (Tuer-Datensaetze aller Raeume STAGE1-6, Leon und Elza)\n", s_nank);

    for (i = 0; i < s_nank; i++) {
        unsigned rid = s_ank[i].dest, st2 = (rid >> 12) & 0xF, r2 = (rid >> 4) & 0xFF;
        const re15_map_zone_t *zn, *zb;
        int seite, rc;
        int16_t mx = 0, my = 0;
        if (rid == 0x5090 || rid == 0x5091) continue;                 /* U-12 (r42/rest) */
        if (!betrete(rid, s_ank[i].x, s_ank[i].z)) continue;
        re15_map_visited_reset();
        re15_map_visited_mark_at(rid, s_ank[i].x, s_ank[i].z);
        re15_map_zone_update(rid, s_ank[i].x, s_ank[i].z);
        zn = re15_map_zone_current();
        if (!zn) continue;                                            /* kein Kartenort (1240/1260/6040) */
        if (st2 >= 1 && st2 <= 6 && (int)r2 < grenze[st2 - 1]) re15_inv_map_stage_init((int)st2 - 1, (int)r2);
        memset(&g_inv_screen, 0, sizeof g_inv_screen);
        re15_inv_screen_open();
        g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
        g_inv_screen.map_page = (uint8_t)re15_inv_map_page_shown();
        seite = g_inv_screen.map_page;
        if (seite < 6 || seite > 12) continue;
        zb = re15_map_zone_fuer(zn->room, zn->idx, (unsigned)seite);
        if (!zb) zb = zn;
        rc = zb->rect;
        if (rc == 255) continue;
        re15_inv_map_marker(s_ank[i].x, s_ank[i].z, 0, &mx, &my);
        if (echte_zeile(zb) == 1) {
            char e[80];
            b_n++;
            if (gemalt(seite, rc, mx, my)) b_auf++;
            else {
                snprintf(e, sizeof e, " %04X<-%04X(B%d r%d %d,%d)", rid, s_ank[i].von, seite, rc, mx, my);
                strncat(lb, e, sizeof lb - strlen(lb) - 1);
            }
            continue;
        }
        a_n++;
        if (gemalt(seite, rc, mx, my)) a_ok++;
        else {
            char e[80];
            snprintf(e, sizeof e, " %04X<-%04X(B%d r%d %d,%d)", rid, s_ank[i].von, seite, rc, mx, my);
            strncat(la, e, sizeof la - strlen(la) - 1);
        }
    }
    /* [C] ein Ort auf zwei Kacheln */
    {
        int c_n = 0, c_ok = 0, a2, b2;
        char lc[1000] = "";
        for (a2 = 0; a2 < re15_map_zone_count(); a2++) {
            const re15_map_zone_t *za = re15_map_zone_by_index(a2);
            int32_t cx, cz;
            if (!za || za->etage || za->page < 6 || za->page > 12 || za->rect == 255) continue;
            cx = (za->wx0 + za->wx1) / 2; cz = (za->wz0 + za->wz1) / 2;
            for (b2 = 0; b2 < re15_map_zone_count(); b2++) {
                const re15_map_zone_t *zb2 = re15_map_zone_by_index(b2);
                const re15_map_zone_t *zc;
                if (!zb2 || zb2 == za || zb2->room != za->room || zb2->idx == za->idx || zb2->etage) continue;
                if (zb2->page != za->page || zb2->rect == 255 || zb2->rect == za->rect) continue;
                if (zb2->ox != za->ox || zb2->oy != za->oy || zb2->sx != za->sx || zb2->sy != za->sy) continue;
                if (zb2->flip_x != za->flip_x || zb2->flip_z != za->flip_z || zb2->tausch != za->tausch) continue;
                if ((za->sx | za->sy) == 0) continue;
                re15_map_visited_reset();
                re15_collision_reset_band();
                g_current_room_id = za->room;
                g_actors[RE15_ACTOR_SLOT_PLAYER].x = cx; g_actors[RE15_ACTOR_SLOT_PLAYER].z = cz;
                re15_map_zone_update(za->room, cx, cz);
                zc = re15_map_zone_current();
                if (!zc || zc->idx != za->idx) continue;
                c_n++;
                if (re15_map_rect_state(za->page, za->rect) == RE15_MAP_RECT_CURRENT &&
                    re15_map_rect_state(zb2->page, zb2->rect) == RE15_MAP_RECT_CURRENT) c_ok++;
                else {
                    char e[64];
                    snprintf(e, sizeof e, " %04X.z%d->r%d", za->room, za->idx, zb2->rect);
                    strncat(lc, e, sizeof lc - strlen(lc) - 1);
                }
            }
        }
        re15_map_visited_reset();
        printf("  [C] ein Ort auf zwei Kacheln: %d/%d Paare beide rot%s%s\n", c_ok, c_n, lc[0] ? ", nicht:" : "", lc);
        CHECK("[C] gleiche Zeile, gleiches Blatt: beide Kacheln des Raums sind aktuell (4040/4080/5140, Leon+Elza)",
              c_n >= 12 && c_ok == c_n);
    }
    printf("  [A] hergeleitete Zeile, Blatt 6-12: %d/%d Ankunftslagen auf der eigenen gemalten Kachel\n", a_ok, a_n);
    printf("      daneben:%s\n", la[0] ? la : " keine");
    printf("  [B] ausgelieferte Zeile (byte-true, nur gemeldet): %d/%d auf der Kachel\n", b_auf, b_n);
    printf("      daneben:%s\n", lb[0] ? lb : " keine");
    CHECK("genug Ankunftslagen auf Blatt 6-12 geprueft", a_n > 100);
    CHECK("[A] jede Ankunft (hergeleitete Zeile, STAGE2-6) steht auf der eigenen gemalten Kachel", a_ok == a_n);
    if (s_roh) free(s_roh);
    printf(g_fail ? "\nFEHLER\n" : "\nOK\n");
    return g_fail;
}
