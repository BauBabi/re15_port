/* =============================================================================
 * Runde 36 Spur M "karte" — Mess-Sonde und Riegel fuer die sechs Kartenbefunde
 * =============================================================================
 * Nutzer 2026-10-10 (analysis/befunde_runde36/AUFTRAG.md Z. 61-66, woertlich):
 *   1. "Im Treppenhaus 10A0 auf Etage B1 fehlen die Treppen eingezeichnet."
 *   2. "In ROOM 11D0 lande ich auf der Map direkt in der Mitte, sobald ich durch die
 *      Tuer gehe, statt am Tuereingang zu stehen..."
 *   3. "Bei ROOM 1090 auf der Map, gibt es 2 Tueren, eine am Ende des Ganges, wo die
 *      Hunde durch die Luke kommen, und eine wo man die Weste bekommt. Das muss als 2
 *      separate Bereich in der Map eingeblendet sein und sauber getrennt."
 *      (gemessen: das ist ROOM1190 - Hunde/Luke sub13, Weste sub14/sub15; Dossier P3)
 *   4. "Die Tuer von ROOM 11C0 ist auf der Map um 90 Grad verdreht."
 *   5. "Die kleinen Tuerbereiche von ROOM 1110 werden schon als besucht angezeigt,
 *      obwohl ich dort noch garnicht drin war"
 *   6. "Die Fahrstuhl Etagen werden als besucht angezeigt, obwohl ich auf den
 *      verschiedenen Etagen noch nicht im Fahrstuhl war."
 * Dossier: analysis/befunde_runde36/M_karte.md.
 *
 * Gemessen wird der ECHTE Kartenpfad (Muster tests/unit/test_r35_karte.c): Raum laden
 * (scd_room_reenter -> re15_map_zone_update, wie der Raumlade-Punkt im Spiel), Spieler
 * setzen, Kartenschirm oeffnen (Seiten-Setzer je Stage + gezeigte Seite), Zustand jedes
 * Rechtecks/Teils, Marken und Spieler-Marker auslesen.
 *
 * Aufruf: probe_r36_karte <teil>
 *   messung   Mess-Ausgabe (kein Urteil) fuer alle sechs Punkte
 *   treppe | marker11d0 | r1190 | tuer11c0 | teile1110 | fahrstuhl   Riegel je Punkt
 *   kabine    Riegel 7 (Nachbesserung 1, Abnahme 0 M1): Kabine bei UNBEKANNTER Etage
 *             (Debug-Menue-Sprung "108 ELEVATOR" / 4020, v8-Hebung)
 *   bild <bmp> ...   Bildauswertung fuer tests/integration/test_r36_karte.cmake
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
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_karte_r36.h"   /* Bit-Bereiche des Etagen-Felds (Riegel 6) */
#include "re15_debug_menu.h"  /* Riegel 7: Debug-Menue-Sprung (Nachbesserung 1) */

static int g_fail = 0;
extern int re15_map_zone_bit_test(int zonen_index);   /* re15_map_zones.c (Riegel-Zugriff) */
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

/* Spielfortschritt fuer die Raumskripte: scd_vm_init loescht alle Flags (re15_game_state_init),
 * deshalb setzt betrete() diese Liste NACH dem Init und VOR dem Raumskript neu. */
static struct { uint8_t bank, bit; } s_flags[16];
static int s_nflags = 0;
static void flag_dauer(int bank, int bit)
{
    if (s_nflags < 16) { s_flags[s_nflags].bank = (uint8_t)bank; s_flags[s_nflags].bit = (uint8_t)bit; s_nflags++; }
}

/* Dieselbe Kette wie test_map_raum_live.c / test_r35_karte.c betrete(). */
static int betrete(unsigned rid, int32_t px, int32_t pz, int band)
{
    char pfad[600];
    size_t n = 0;
    re15_actor_t *pl;
    int f;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT",
             RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    if (s_roh) { free(s_roh); s_roh = 0; }
    s_roh = slurp(pfad, &n);
    if (!s_roh) return 0;
    if (re15_rdt_parse(s_roh, n, &s_rdt) < 0) return 0;
    scd_vm_init();
    for (f = 0; f < s_nflags; f++) re15_game_flag_set(s_flags[f].bank, s_flags[f].bit, 1);
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

typedef struct {
    int ok, zone, zpage, zrect, zid, page;
    int cur[16], ncur;
    int16_t mx, my;
} mess_t;

/* Kartenschirm oeffnen und messen (wie menu_common.c map_entry()). */
static void miss_hier(unsigned rid, int32_t px, int32_t pz, mess_t *m)
{
    const re15_map_zone_t *zn;
    int i, n;
    memset(m, 0, sizeof *m);
    m->ok = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = px;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = pz;
    re15_map_zone_update(rid, px, pz);
    zn = re15_map_zone_current();
    if (zn) { m->zone = 1; m->zpage = zn->page; m->zrect = zn->rect; m->zid = zn->zid; }
    re15_inv_screen_open();
    g_inv_screen.substate = 1;
    g_inv_screen.item_state = 1;
    re15_inv_map_stage_init((int)((rid >> 12) & 0xfu) - 1, (int)((rid >> 4) & 0xffu));
    g_inv_screen.map_page = re15_inv_map_page_shown();
    m->page = g_inv_screen.map_page;
    n = re15_map_rect_count((unsigned)m->page);
    for (i = 0; i < n && m->ncur < 16; i++)
        if (re15_map_rect_state((unsigned)m->page, (unsigned)i) == RE15_MAP_RECT_CURRENT)
            m->cur[m->ncur++] = i;
    m->mx = m->my = -99;
    re15_inv_map_marker(px, pz, (uint8_t)((rid >> 4) & 0xFF), &m->mx, &m->my);
}

static void miss(unsigned rid, int32_t px, int32_t pz, int band, mess_t *m)
{
    memset(m, 0, sizeof *m);
    if (!betrete(rid, px, pz, band)) return;
    miss_hier(rid, px, pz, m);
}

static void druck(const char *was, unsigned rid, int32_t px, int32_t pz, const mess_t *m)
{
    int i;
    printf("  %-26s ROOM%04X (%6d,%6d): ", was, rid, (int)px, (int)pz);
    if (!m->ok) { printf("RDT fehlt\n"); return; }
    if (m->zone) printf("Zone Blatt %d rect %d zid %d | ", m->zpage, m->zrect, m->zid);
    else         printf("KEINE Zone | ");
    printf("gezeigt Blatt %d | aktuell:", m->page);
    for (i = 0; i < m->ncur; i++) printf(" %d", m->cur[i]);
    if (!m->ncur) printf(" -");
    printf(" | Marker (%d,%d)\n", m->mx, m->my);
}

static const char *zustand(int s)
{
    switch (s) {
    case RE15_MAP_RECT_CURRENT:   return "AKTUELL";
    case RE15_MAP_RECT_VISITED:   return "besucht";
    case RE15_MAP_RECT_UNVISITED: return "unbesucht";
    default:                      return "ohne-Zone";
    }
}

/* Sichtbare Marken eines Rechtecks zaehlen (Treppen = kind >= 4). */
static int marken(int page, int rect, int zid, int nur_treppen, int drucken)
{
    int i, n = re15_map_mark_count(), z = 0;
    for (i = 0; i < n; i++) {
        int pg, rc, mx, my, kind, za, zb, sicht;
        sicht = re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind);
        re15_map_mark_zonen(i, &za, &zb);
        if (pg != page || (rect >= 0 && rc != rect)) continue;
        if (zid >= 0 && za != zid && zb != zid) continue;
        if (nur_treppen && kind < 4) continue;
        if (drucken)
            printf("      Marke #%d Blatt %d rect %d (%d,%d) kind %d zid %d/%d sichtbar %d\n",
                   i, pg, rc, mx, my, kind, za, zb, sicht);
        if (sicht) z++;
    }
    return z;
}

/* ---- Fahrstuhl: die Kabine aus einem Etagenraum betreten ----------------------- */
static void fahrstuhl_rein(unsigned vorraum, int32_t vx, int32_t vz,
                           unsigned kabine, int32_t kx, int32_t kz)
{
    mess_t m;
    re15_game_flag_set(3, 54, 0); re15_game_flag_set(3, 55, 0); re15_game_flag_set(3, 56, 0);
    miss(vorraum, vx, vz, 0, &m);
    miss(kabine, kx, kz, 0, &m);
}

static int teil_messung(void)
{
    mess_t m;
    int k, n, i;
    printf("=== Punkt 1: Treppenhaus ROOM10A0 auf B1 (Blatt 0) ===\n");
    re15_map_visited_reset();
    /* Ankunft aus ROOM1180 (Tuer @0x9EE.. slot 1 -> 10A0 Spawn (21200,-7200,25500), Band 4) */
    miss(0x10A0, 21200, 25500, 4, &m);  druck("10A0 B1 (Band 4)", 0x10A0, 21200, 25500, &m);
    printf("    Blatt 0 rect 6 Zustand %s; Treppenmarken (sichtbar) auf Blatt 0: %d\n",
           zustand(re15_map_rect_state(0, 6)), marken(0, -1, 10, 1, 1));
    re15_map_debug_reveal_page(1); re15_map_debug_reveal_page(2);
    printf("    Vergleich: Treppenmarken 10A0 auf B2 (Blatt 1) %d, auf 1F (Blatt 2) %d\n",
           marken(1, 9, 10, 1, 1), marken(2, 6, 10, 1, 1));

    printf("=== Punkt 2: ROOM11D0 durch die Tuer ===\n");
    re15_map_visited_reset();
    /* Gang: im Tuer-Trigger ROOM1230 slot 0 (-6041,-17561 2409x1444) */
    miss(0x1230, -4837, -16839, 0, &m); druck("1230 an der 11D0-Tuer", 0x1230, -4837, -16839, &m);
    /* Ankunft: Spawn der Tuer (-300,0,-17400) */
    miss(0x11D0, -300, -17400, 0, &m);  druck("11D0 Ankunft", 0x11D0, -300, -17400, &m);
    miss_hier(0x11D0, -300, -16700, &m); druck("11D0 im Tuer-Trigger", 0x11D0, -300, -16700, &m);
    miss_hier(0x11D0, 2600, -22000, &m); druck("11D0 an der 1260-Tuer", 0x11D0, 2600, -22000, &m);
    printf("    Tuermarken Blatt 0 rect 5:\n"); marken(0, 5, -1, 0, 1);

    printf("=== Punkt 3: ROOM1190 (Hunde/Luke + Weste) ===\n");
    re15_map_visited_reset();
    miss(0x1190, 3174, -11182, 0, &m);  druck("1190 aus Gang (Hunde)", 0x1190, 3174, -11182, &m);
    printf("    nach NUR Haupthalle: rect 1 %s, rect 2 %s\n",
           zustand(re15_map_rect_state(0, 1)), zustand(re15_map_rect_state(0, 2)));
    re15_map_visited_reset();
    miss(0x1190, 19515, -12109, 0, &m); druck("1190 aus Gang (Weste)", 0x1190, 19515, -12109, &m);
    printf("    nach NUR Ostkammer: rect 1 %s, rect 2 %s\n",
           zustand(re15_map_rect_state(0, 1)), zustand(re15_map_rect_state(0, 2)));
    miss(0x11A0, 6500, -3850, 0, &m);   druck("11A0 (Tuer 2070)", 0x11A0, 6500, -3850, &m);
    miss(0x11A0, 7500, 10650, 3, &m);   druck("11A0 Band 3", 0x11A0, 7500, 10650, &m);

    printf("=== Punkt 4: Tuer ROOM11B0 <-> ROOM11C0 auf B1 ===\n");
    re15_map_visited_reset();
    re15_map_debug_reveal_page(0);
    n = re15_map_mark_count();
    for (i = 0; i < n; i++) {
        int pg, rc, mx, my, kind, za, zb;
        re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind);
        re15_map_mark_zonen(i, &za, &zb);
        if (pg != 0 || !((za == 28 && zb == 29) || (za == 29 && zb == 28) ||
                         za == 29 || zb == 29)) continue;
        printf("    Marke #%d rect %d (%d,%d) kind %d = %s Balken\n", i, rc, mx, my, kind,
               (kind == 0 || kind == 2) ? "WAAGERECHTER" : "SENKRECHTER");
    }
    printf("    gemaltes Original (analysis/kartensymbole/symbolkatalog.csv): Blatt 0 rect 4 "
           "(171,101) 5x5 Wand N waagerecht\n");

    printf("=== Punkt 5: ROOM1110 Teilbereiche ===\n");
    re15_map_visited_reset();
    miss(0x1110, -2400, -5000, 0, &m); druck("1110 Ankunft aus 1100", 0x1110, -2400, -5000, &m);
    for (k = 0; k < re15_map_teil_count(3, 5); k++) {
        int tx, ty, tw, th, ts;
        re15_map_teil_get(3, 5, k, &tx, &ty, &tw, &th, &ts);
        printf("    Teil %d (%d,%d %dx%d): %s\n", k, tx, ty, tw, th, zustand(ts));
    }
    miss_hier(0x1110, -8603, -1243, &m); druck("1110 kleiner Raum oben", 0x1110, -8603, -1243, &m);
    for (k = 0; k < re15_map_teil_count(3, 5); k++) {
        int tx, ty, tw, th, ts;
        re15_map_teil_get(3, 5, k, &tx, &ty, &tw, &th, &ts);
        printf("    Teil %d (%d,%d %dx%d): %s\n", k, tx, ty, tw, th, zustand(ts));
    }

    printf("=== Punkt 6: Fahrstuhl-Etagen ===\n");
    re15_map_visited_reset();
    /* 1F: ROOM1040 Tuer slot 0 -> 1080 (Trigger (-22936,-11000 2300x1400)), Kabine (-13650,-900) */
    fahrstuhl_rein(0x1040, -21786, -10300, 0x1080, -13650, -900);
    printf("    1080 aus 1F (1040): 1F rect 9 %s | 2F rect 4 %s | 3F rect 0 %s\n",
           zustand(re15_map_rect_state(2, 9)), zustand(re15_map_rect_state(3, 4)),
           zustand(re15_map_rect_state(4, 0)));
    miss(0x1120, 1300, 7300, 0, &m);   /* ausgestiegen auf 3F */
    printf("    danach in 1120 (3F): 3F-Kabine rect 0 %s, 2F-Kabine rect 4 %s\n",
           zustand(re15_map_rect_state(4, 0)), zustand(re15_map_rect_state(3, 4)));
    re15_map_visited_reset();
    fahrstuhl_rein(0x4000, 0, 0, 0x4020, -21400, -23400);
    printf("    4020 aus 4000 (Blatt 8): Blatt 8 rect 2 %s | Blatt 9 rect 13 %s | Blatt 10 rect 3 %s\n",
           zustand(re15_map_rect_state(8, 2)), zustand(re15_map_rect_state(9, 13)),
           zustand(re15_map_rect_state(10, 3)));
    return 0;
}

/* ===================================================================================
 * RIEGEL — je Nutzer-Punkt die MECHANIK am echten Kartenpfad
 * =================================================================================== */

/* Kachel der Seite: DATA/MAP0<p+1>.PIX, 256x256 4bpp, 128 B je Zeile, unteres Nibble links. */
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

/* Kachel-Index des Rechtecks am Schirmpunkt (x,y); -1 = ausserhalb. */
static int texel(int seite, int rect, int x, int y)
{
    int rx, ry, rw, rh, u, v;
    if (!pix_laden(seite)) return -1;
    if (!re15_map_rect_geometry((unsigned)seite, (unsigned)rect, &rx, &ry, &rw, &rh)) return -1;
    if (!re15_map_rect_uv((unsigned)seite, (unsigned)rect, &u, &v)) return -1;
    if (x < rx || x >= rx + rw || y < ry || y >= ry + rh) return -1;
    return s_pix[(v + y - ry) * 256 + (u + x - rx)];
}

/* Tuer im geladenen Raum (g_aot nach scd_room_reenter), die in den Raum mit Raum-Byte
 * `ziel_byte` fuehrt; bei mehreren die mit dem Spawn naechst an (sx_hint, sz_hint).
 * Liefert Trigger-Mitte und Spawn - die Zahlen, die das SPIEL beim Tuerdurchgang benutzt. */
static int tuer(unsigned ziel_byte, int32_t sx_hint, int32_t sz_hint,
                int32_t *tx, int32_t *tz, int32_t *sx, int32_t *sz)
{
    int k, best = -1;
    int64_t bd = 0;
    for (k = 0; k < RE15_AOT_MAX; k++) {
        const re15_aot_t *a = &g_aot.slots[k];
        int64_t dx, dz, d;
        if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
        if (g_aot.door_params[k].dest_room != ziel_byte) continue;
        dx = g_aot.door_params[k].spawn_x - sx_hint;
        dz = g_aot.door_params[k].spawn_z - sz_hint;
        d = dx * dx + dz * dz;
        if (best < 0 || d < bd) { best = k; bd = d; }
    }
    if (best < 0) return 0;
    *tx = g_aot.slots[best].x; *tz = g_aot.slots[best].z;
    *sx = g_aot.door_params[best].spawn_x; *sz = g_aot.door_params[best].spawn_z;
    return 1;
}

/* Echter Weg durch eine Tuer: im Quellraum im Tuer-Trigger stehen (Marker messen), dann den
 * Zielraum am Spawn der Tuer laden (wie re15_room_apply_pending). */
static int durch_tuer(unsigned von, int32_t vx, int32_t vz, int band_von, unsigned ziel,
                      int32_t sx_hint, int32_t sz_hint, int band_ziel, mess_t *vorher,
                      mess_t *nachher, int32_t *ax, int32_t *az)
{
    int32_t tx, tz, sx, sz;
    if (!betrete(von, vx, vz, band_von)) return 0;
    if (!tuer((ziel >> 4) & 0xFFu, sx_hint, sz_hint, &tx, &tz, &sx, &sz)) return 0;
    miss_hier(von, tx, tz, vorher);
    if (!betrete(ziel, sx, sz, band_ziel)) return 0;
    miss_hier(ziel, sx, sz, nachher);
    if (ax) *ax = sx;
    if (az) *az = sz;
    return 1;
}

static int nur_aktuell(const mess_t *m, int soll) { return m->ncur == 1 && m->cur[0] == soll; }

/* ---- Riegel 1: Treppen 10A0 auf B1 ------------------------------------------------- */
static int teil_treppe(void)
{
    mess_t vor, m;
    int k, n_band4 = 0, n_marken = 0, alle_auf_kunst = 1, alle_getroffen = 1;
    int32_t ax, az;
    struct { int x, y; } soll[8];
    const re15_map_zone_t *zb1 = re15_map_zone_fuer(0x10A0, 0, 0);
    char t[240];
    printf("=== Riegel 1: Treppen im Treppenhaus 10A0 auf B1 ===\n");
    CHECK("ROOM10A0 hat eine Zeile auf Blatt 0 (B1)", zb1 && zb1->rect == 6);
    if (!zb1) return 1;
    re15_map_visited_reset();
    /* Gegenprobe: nur 1F begangen -> auf B1 KEINE 10A0-Treppe (Gatter am B1-Etagen-Bit) */
    miss(0x10A0, 27050, 24900, 8, &m);
    snprintf(t, sizeof t, "nur 1F begangen: auf B1 keine sichtbare 10A0-Treppe - sind %d",
             marken(0, 6, 10, 1, 0));
    CHECK(t, marken(0, 6, 10, 1, 0) == 0);
    /* ECHTER WEG: Gang ROOM1230 -> Tuer slot 1 -> ROOM10A0 B1 (Spawn y -7200 = Band 4) */
    re15_map_visited_reset();
    if (!durch_tuer(0x1230, -177, -7400, 0, 0x10A0, 21200, 25500, 4, &vor, &m, &ax, &az)) {
        CHECK("Tuer ROOM1230 -> ROOM10A0 gefunden", 0); return 1;
    }
    druck("10A0 Ankunft aus 1230", 0x10A0, ax, az, &m);
    CHECK("Ankunft: die Karte zeigt B1 (Blatt 0), aktuell nur rect 6",
          m.page == 0 && nur_aktuell(&m, 6));
    /* SOLL aus dem geladenen RDT: die STAIR-AOTs mit Band 4 (Aot_set sce 13 @0x00E7C/@0x00E90),
     * ihre Mitten mit der B1-Abbildung der Zone projiziert. */
    for (k = 0; k < RE15_AOT_MAX && n_band4 < 8; k++) {
        const re15_aot_t *a = &g_aot.slots[k];
        int16_t px = 0, py = 0;
        int rx, ry, rw, rh, i, neu = 1;
        if (!a->active || a->type != RE15_AOT_TYPE_STAIR || a->band != 4) continue;
        re15_map_rect_geometry(0, 6, &rx, &ry, &rw, &rh);
        re15_map_zone_marker(zb1, a->x, a->z, rx, ry, rw, rh, &px, &py);
        for (i = 0; i < n_band4; i++) if (soll[i].x == px && soll[i].y == py) neu = 0;
        printf("    STAIR-AOT slot %d Band 4 Mitte (%d,%d) -> B1 (%d,%d)\n", k, (int)a->x,
               (int)a->z, px, py);
        if (neu) { soll[n_band4].x = px; soll[n_band4].y = py; n_band4++; }
    }
    snprintf(t, sizeof t, "das RDT fuehrt zwei Treppenlaeufe auf Band 4 (6<->4, 4<->3) - %d", n_band4);
    CHECK(t, n_band4 == 2);
    {
        int i, nm = re15_map_mark_count();
        for (i = 0; i < nm; i++) {
            int pg, rc, mx, my, kind, za, zbb, j, hit = 0, row;
            if (!re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind)) continue;
            re15_map_mark_zonen(i, &za, &zbb);
            if (pg != 0 || kind < 4 || za != 10) continue;
            n_marken++;
            for (j = 0; j < n_band4; j++)
                if (abs(soll[j].x - mx) <= 1 && abs(soll[j].y - my) <= 1) hit = 1;
            if (!hit) alle_getroffen = 0;
            /* drei Sprossen 5x1 (kind 4) bzw. 1x5 (kind 5), re15_inv_screen.c - alle auf
             * gemaltem Innenraum (Index 1) der B1-Kachel */
            for (row = -2; row <= 2; row += 2) {
                int d;
                for (d = -2; d <= 2; d++) {
                    int x = (kind == 4) ? mx + d : mx + row, y = (kind == 4) ? my + row : my + d;
                    if (texel(0, 6, x, y) != 1) alle_auf_kunst = 0;
                }
            }
            printf("    sichtbare Treppenmarke B1 (%d,%d) kind %d\n", mx, my, kind);
        }
    }
    snprintf(t, sizeof t, "auf B1 sind GENAU die zwei Treppen sichtbar - %d", n_marken);
    CHECK(t, n_marken == 2);
    CHECK("jede B1-Treppe sitzt (<= 1 px) auf der Projektion ihres Band-4-STAIR-AOT", alle_getroffen);
    CHECK("jede Sprosse liegt auf gemaltem Innenraum der B1-Kachel (MAP01.PIX uv(128,16))",
          alle_auf_kunst);
    /* die anderen Etagen unveraendert: je zwei */
    re15_map_debug_reveal_page(1); re15_map_debug_reveal_page(2);
    CHECK("B2 und 1F tragen weiter je zwei 10A0-Treppen",
          marken(1, 9, 10, 1, 0) == 2 && marken(2, 6, 10, 1, 0) == 2);
    return g_fail;
}

/* ---- Riegel 2: Marker ROOM11D0 an der Tuer ------------------------------------------ */
static int teil_marker11d0(void)
{
    mess_t gang, m, unten;
    int32_t ax, az;
    int tx0 = -1, ty0 = -1, i, nm = re15_map_mark_count();
    char t[240];
    printf("=== Riegel 2: ROOM11D0 - der Marker steht nach der Tuer am Eingang ===\n");
    re15_map_visited_reset();
    /* ROOM1230 main00 @0x00AE6-0x00B12: Tuer slot 0 fuehrt erst nach Ck(4,243) ("Reservestrom
     * an", ROOM11F0 sub18 @0x016F6) nach ROOM11D0 (sonst nach ROOM1160). */
    flag_dauer(4, 243);
    if (!durch_tuer(0x1230, -4837, -16300, 0, 0x11D0, -300, -17400, 0, &gang, &m, &ax, &az)) {
        CHECK("Tuer ROOM1230 -> ROOM11D0 gefunden", 0); return 1;
    }
    druck("1230 im Tuer-Trigger", 0x1230, 0, 0, &gang);
    druck("11D0 Ankunft", 0x11D0, ax, az, &m);
    for (i = 0; i < nm; i++) {   /* die Tuermarke 11D0 <-> Gang (zid 30/108) */
        int pg, rc, mx, my, kind, za, zb;
        if (!re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind)) continue;
        re15_map_mark_zonen(i, &za, &zb);
        if (pg == 0 && za == 30 && zb == 108) { tx0 = mx; ty0 = my; }
    }
    printf("    Tuermarke 11D0<->Gang (%d,%d)\n", tx0, ty0);
    CHECK("Ankunft in 11D0: Blatt 0, aktuell nur rect 5", m.page == 0 && nur_aktuell(&m, 5));
    snprintf(t, sizeof t, "Ankunft: Marker (%d,%d) hoechstens 6 px unter der Tuermarke (%d,%d) und "
             "<= 2 px seitlich (vorher (129,151) = Mitte)", m.mx, m.my, tx0, ty0);
    CHECK(t, ty0 > 0 && m.my >= ty0 && m.my - ty0 <= 6 && abs(m.mx - tx0) <= 2);
    snprintf(t, sizeof t, "Sprung beim Durchgang Gang -> 11D0 <= 8 px (Gang (%d,%d), Raum (%d,%d))",
             gang.mx, gang.my, m.mx, m.my);
    CHECK(t, abs(gang.mx - m.mx) + abs(gang.my - m.my) <= 8);
    /* Die Suedwand (Innenkante SCA @0x08A8 z -23050) liegt auf der gemalten Suedwand y172 der
     * Kachel MAP01.PIX uv(128,56); der Spieler kommt bis 450 (Radius) an sie heran. */
    miss_hier(0x11D0, 2600, -22600, &unten);
    snprintf(t, sizeof t, "an der 1260-Tuer (Suedwand, Spielerradius 450 vor der Wand): Marker y %d "
             "<= 4 px ueber der gemalten Suedwand y172", unten.my);
    CHECK(t, unten.my <= 172 && unten.my >= 168 && texel(0, 5, unten.mx, 172) == 4);
    /* Die ANDERE Tuer: aus der Kanalisation ROOM1260 (Tuer slot 3 @0x0118A -> Spawn (3900,-22000)).
     * Vorher (Huellen-Kasten) (141,171); jetzt im unteren Sechstel an der Tuermarke (137,172). */
    re15_map_visited_reset();
    if (durch_tuer(0x1260, 21850, -7300, -1, 0x11D0, 3900, -22000, 0, &gang, &m, &ax, &az)) {
        druck("11D0 Ankunft aus 1260", 0x11D0, ax, az, &m);
        snprintf(t, sizeof t, "Ankunft aus 1260: Marker (%d,%d) an der unteren Tuermarke (137,172): "
                 "<= 4 px seitlich, y 160..172", m.mx, m.my);
        CHECK(t, abs(m.mx - 137) <= 4 && m.my >= 160 && m.my <= 172);
    } else {
        CHECK("Tuer ROOM1260 -> ROOM11D0 gefunden", 0);
    }
    return g_fail;
}

/* ---- Riegel 3: ROOM1190 zwei Bereiche --------------------------------------------- */
static int teil_r1190(void)
{
    mess_t gang, m;
    int32_t ax, az;
    const re15_map_zone_t *zh, *zo;
    char t[240];
    printf("=== Riegel 3: ROOM1190 Haupthalle (Hunde) und Ostkammer (Weste) getrennt ===\n");
    zh = re15_map_zone_at(0x1190, 3174, -11182);
    zo = re15_map_zone_at(0x1190, 19515, -12109);
    CHECK("zwei verschiedene Zonen mit verschiedenen Besucht-Bits",
          zh && zo && zh != zo && zh->zid != zo->zid &&
          re15_map_zone_bit_test((int)(zh - re15_map_zone_by_index(0))) !=
          re15_map_zone_bit_test((int)(zo - re15_map_zone_by_index(0))));
    /* ECHTER WEG 1: Gang -> Haupthallen-Tuer (ROOM1230 slot 2 @0x00B54) */
    re15_map_visited_reset();
    if (!durch_tuer(0x1230, -8118, 10300, 0, 0x1190, 3174, -11182, 0, &gang, &m, &ax, &az)) {
        CHECK("Tuer ROOM1230 -> 1190 Haupthalle", 0); return 1;
    }
    druck("1190 Haupthalle", 0x1190, ax, az, &m);
    CHECK("Haupthalle: Blatt 0, aktuell NUR rect 1", m.page == 0 && nur_aktuell(&m, 1));
    CHECK("Haupthalle: Ostkammer (rect 2) noch UNBESUCHT",
          re15_map_rect_state(0, 2) == RE15_MAP_RECT_UNVISITED);
    snprintf(t, sizeof t, "Haupthalle: Marker (%d,%d) auf gemalter Flaeche von rect 1", m.mx, m.my);
    CHECK(t, texel(0, 1, m.mx, m.my) > 0);
    /* Hinten an den Zielscheiben (Haupthalle, Westteil) bleibt es rect 1 */
    miss_hier(0x1190, -20000, -20000, &m);
    CHECK("Haupthalle an den Zielscheiben (-20000,-20000): weiter NUR rect 1", nur_aktuell(&m, 1));
    /* ECHTER WEG 2: Gang -> Ostkammer-Tuer (ROOM1230 slot 4 @0x00CE6 - steht im Else-Zweig von
     * Ck(3,0x89) @0x00B98, vorher ist der Platz eine Meldung) */
    flag_dauer(3, 0x89);
    if (!durch_tuer(0x1230, 96, 12400, 0, 0x1190, 19515, -12109, 0, &gang, &m, &ax, &az)) {
        CHECK("Tuer ROOM1230 -> 1190 Ostkammer", 0); return 1;
    }
    druck("1190 Ostkammer", 0x1190, ax, az, &m);
    CHECK("Ostkammer: Blatt 0, aktuell NUR rect 2", m.page == 0 && nur_aktuell(&m, 2));
    CHECK("Ostkammer: Haupthalle (rect 1) jetzt BESUCHT",
          re15_map_rect_state(0, 1) == RE15_MAP_RECT_VISITED);
    snprintf(t, sizeof t, "Ostkammer: Marker (%d,%d) an der gemalten Tuernische (152..156,70..74)",
             m.mx, m.my);
    CHECK(t, m.mx >= 152 && m.mx <= 156 && m.my >= 70 && m.my <= 74);
    miss_hier(0x1190, 19700, -21000, &m);   /* an der Weste (Aot slot 16 @0x02D90) */
    CHECK("an der Weste: weiter NUR rect 2", nur_aktuell(&m, 2));
    /* ROOM11A0 liegt nicht mehr auf B1 */
    re15_map_visited_reset();
    miss(0x11A0, 7500, 10650, 3, &m);
    CHECK("ROOM11A0 (Band 3) zeigt Blatt 6, nicht B1", m.page == 6 && nur_aktuell(&m, 2));
    CHECK("ROOM11A0 begangen: B1 rect 1 bleibt UNBESUCHT",
          re15_map_rect_state(0, 1) == RE15_MAP_RECT_UNVISITED);
    /* Wortlaut des Nutzers "ROOM 1090": dessen zwei Bereiche (Hof Band 1 mit Tuer -> 1050
     * @0x0211A, Dachterrasse Band 6 mit Tuer -> 1100 @0x0213A) liegen schon getrennt auf 1F/2F -
     * festgehalten, damit die Zuordnung zu ROOM1190 (Hunde/Luke/Weste, Dossier R0) pruefbar bleibt. */
    re15_map_visited_reset();
    miss(0x1090, -10080, 4000, 1, &m);
    druck("1090 Hof (Band 1)", 0x1090, -10080, 4000, &m);
    CHECK("ROOM1090 Hof: Blatt 2 (1F), NUR rect 5", m.page == 2 && nur_aktuell(&m, 5));
    miss(0x1090, -5820, -17500, 6, &m);
    druck("1090 Dach (Band 6)", 0x1090, -5820, -17500, &m);
    CHECK("ROOM1090 Dach: Blatt 3 (2F), NUR rect 7", m.page == 3 && nur_aktuell(&m, 7));
    return g_fail;
}

/* ---- Riegel 4: Tuer 11B0 <-> 11C0 waagerecht --------------------------------------- */
static int teil_tuer11c0(void)
{
    int i, nm = re15_map_mark_count(), gefunden = 0;
    char t[240];
    printf("=== Riegel 4: Tuer ROOM11B0 <-> ROOM11C0 liegt WAAGERECHT in der Wand ===\n");
    re15_map_visited_reset();
    re15_map_debug_reveal_page(0);
    for (i = 0; i < nm; i++) {
        int pg, rc, mx, my, kind, za, zb, d, wand = 1;
        if (!re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind)) continue;
        re15_map_mark_zonen(i, &za, &zb);
        if (pg != 0 || kind > 3 || !((za == 28 && zb == 29) || (za == 29 && zb == 28))) continue;
        gefunden++;
        printf("    Marke (%d,%d) kind %d\n", mx, my, kind);
        snprintf(t, sizeof t, "der Balken ist WAAGERECHT (kind 0/2) - kind %d", kind);
        CHECK(t, kind == 0 || kind == 2);
        /* jeder der 5 Balkenpunkte liegt auf der gemeinsamen Wandzeile: Unterkante der
         * 11B0-Kachel (rect 3) und Oberkante der 11C0-Kachel (rect 4) = Index 4 */
        for (d = -2; d <= 2; d++)
            if (texel(0, 3, mx + d, my) != 4 || texel(0, 4, mx + d, my) != 4) wand = 0;
        snprintf(t, sizeof t, "alle 5 Balkenpunkte (%d..%d,%d) liegen auf der gemalten Wand "
                 "zwischen 11B0 und 11C0", mx - 2, mx + 2, my);
        CHECK(t, wand);
        /* ueber der gemalten Tuernische der 11C0-Kachel: Nischenwaende bei x171/x175 (Index 4)
         * in den Zeilen y101..104, dazwischen Innenraum (Index 1) */
        snprintf(t, sizeof t, "Balkenmitte x%d ueber der gemalten Nische x171..175", mx);
        CHECK(t, mx >= 172 && mx <= 174 && texel(0, 4, 171, 102) == 4 &&
                 texel(0, 4, 175, 102) == 4 && texel(0, 4, 173, 102) == 1);
    }
    snprintf(t, sizeof t, "genau eine Marke 11B0<->11C0 - %d", gefunden);
    CHECK(t, gefunden == 1);
    return g_fail;
}

/* ---- Riegel 5: Teilbereiche ROOM1110 erst nach Betreten besucht --------------------- */
static void teile_lesen(int st[3])
{
    int k;
    for (k = 0; k < 3; k++) {
        int tx, ty, tw, th, ts = -1;
        re15_map_teil_get(3, 5, k, &tx, &ty, &tw, &th, &ts);
        st[k] = ts;
    }
}

static int teil_teile1110(void)
{
    mess_t vor, m;
    int32_t ax, az;
    int st[3];
    uint8_t zb[32], fb[16];
    char t[240];
    printf("=== Riegel 5: ROOM1110 - kleine Raeume erst besucht, wenn man drin war ===\n");
    re15_map_visited_reset();
    /* ROOM1100 sub00: ohne Ck(3,0x67) @0x00AC8 bzw. Ck(4,0xE8) @0x00B06 setzt Aot_reset slot 1
     * sce 0 (@0x00ACC / @0x00B0A) die Tuer nach ROOM1110 still - Spielfortschritt setzen. */
    flag_dauer(3, 0x67);
    flag_dauer(4, 0xE8);
    /* ECHTER WEG: ROOM1100 Tuer slot 1 @0x009BA -> ROOM1110 Spawn (-2400,-5000) */
    if (!durch_tuer(0x1100, -26130, -11036, 0, 0x1110, -2400, -5000, 0, &vor, &m, &ax, &az)) {
        CHECK("Tuer ROOM1100 -> ROOM1110", 0); return 1;
    }
    teile_lesen(st);
    printf("    Ankunft (%d,%d): Teil 0 %s | 1 %s | 2 %s\n", (int)ax, (int)az, zustand(st[0]),
           zustand(st[1]), zustand(st[2]));
    CHECK("Ankunft im Evidence Room: Teil 0 AKTUELL",
          st[0] == RE15_MAP_RECT_CURRENT);
    CHECK("Ankunft: die kleinen Raeume (Teil 1/2) sind UNBESUCHT (der gemeldete Fehler: besucht)",
          st[1] == RE15_MAP_RECT_UNVISITED && st[2] == RE15_MAP_RECT_UNVISITED);
    /* Selbst-Tuer slot 1 @0x00A6E -> kleiner Raum oben (Spawn (-4550,-650)) */
    {
        int32_t tx, tz, sx, sz;
        if (!tuer(0x11, -4550, -650, &tx, &tz, &sx, &sz)) { CHECK("Selbst-Tuer oben", 0); return 1; }
        miss_hier(0x1110, tx, tz, &m);
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = sx; g_actors[RE15_ACTOR_SLOT_PLAYER].z = sz;
        scd_room_reenter(&s_rdt, sx, sz, 0);
        miss_hier(0x1110, sx, sz, &m);
        teile_lesen(st);
        printf("    kleiner Raum oben (%d,%d): Teil 0 %s | 1 %s | 2 %s\n", (int)sx, (int)sz,
               zustand(st[0]), zustand(st[1]), zustand(st[2]));
        CHECK("im kleinen Raum oben: Teil 1 AKTUELL, Evidence Room besucht, unterer Raum UNBESUCHT",
              st[1] == RE15_MAP_RECT_CURRENT && st[0] == RE15_MAP_RECT_VISITED &&
              st[2] == RE15_MAP_RECT_UNVISITED);
    }
    /* zurueck in den Evidence Room (Selbst-Tuer slot 2 @0x00A8E -> (-2500,-650)) */
    miss_hier(0x1110, -2500, -650, &m);
    teile_lesen(st);
    CHECK("zurueck im Evidence Room: oberer Raum BESUCHT, unterer weiter UNBESUCHT",
          st[0] == RE15_MAP_RECT_CURRENT && st[1] == RE15_MAP_RECT_VISITED &&
          st[2] == RE15_MAP_RECT_UNVISITED);
    /* SPEICHERN/LADEN: die Teil-Bits stehen im Spielstand (visited_floor, v9) */
    re15_map_visited_export(zb);
    re15_map_visited_floor_export(fb);
    re15_map_visited_reset();
    re15_map_visited_import(zb);
    re15_map_visited_floor_import(fb);
    re15_map_ohne_aktuell(1);
    teile_lesen(st);
    re15_map_ohne_aktuell(0);
    snprintf(t, sizeof t, "nach Speichern/Laden: Teil 0 %s, 1 %s, 2 %s (soll besucht/besucht/"
             "unbesucht)", zustand(st[0]), zustand(st[1]), zustand(st[2]));
    CHECK(t, st[0] == RE15_MAP_RECT_VISITED && st[1] == RE15_MAP_RECT_VISITED &&
             st[2] == RE15_MAP_RECT_UNVISITED);
    /* ALTER Spielstand (nur das Zonen-Bit, keine Teil-Bits): der EINGANG (Evidence Room) ist
     * besucht, die kleinen Raeume nicht */
    re15_map_visited_reset();
    re15_map_visited_mark(0x1110);
    re15_map_ohne_aktuell(1);
    teile_lesen(st);
    re15_map_ohne_aktuell(0);
    CHECK("alter Stand (nur Zonen-Bit): Evidence Room besucht, kleine Raeume unbesucht",
          st[0] == RE15_MAP_RECT_VISITED && st[1] == RE15_MAP_RECT_UNVISITED &&
          st[2] == RE15_MAP_RECT_UNVISITED);
    return g_fail;
}

/* ---- Riegel 6: Fahrstuhl-Kabine nur auf der Einstiegs-Etage besucht ----------------- */
static int teil_fahrstuhl(void)
{
    mess_t vor, m;
    int32_t ax, az;
    uint8_t zb[32], fb[16];
    char t[240];
    printf("=== Riegel 6: Fahrstuhl-Etagen ===\n");
    re15_map_visited_reset();
    /* ROOM1040 main00 @0x0107A Ck(4,243): die Fahrstuhltuer slot 0 @0x01096 steht im Else-Zweig */
    flag_dauer(4, 243);
    /* ECHTER WEG: 1F ROOM1040 Tuer slot 0 @0x01096 -> Kabine */
    if (!durch_tuer(0x1040, -21786, -10300, 0, 0x1080, -13650, -900, 0, &vor, &m, &ax, &az)) {
        CHECK("Tuer ROOM1040 -> ROOM1080", 0); return 1;
    }
    snprintf(t, sizeof t, "aus 1F eingestiegen: 1F-Kabine %s, 2F %s, 3F %s",
             zustand(re15_map_rect_state(2, 9)), zustand(re15_map_rect_state(3, 4)),
             zustand(re15_map_rect_state(4, 0)));
    CHECK(t, m.page == 2 && re15_map_rect_state(2, 9) == RE15_MAP_RECT_CURRENT &&
             re15_map_rect_state(3, 4) == RE15_MAP_RECT_UNVISITED &&
             re15_map_rect_state(4, 0) == RE15_MAP_RECT_UNVISITED);
    /* LADEN ist keine Ankunft: Stand "nur 1F" sichern, wieder laden und im 3F-Etagenraum stehen
     * (der Raum der vorigen Sitzung war die Kabine) - die 3F-Kabine bleibt unbesucht. */
    {
        uint8_t z1[32], f1[16];
        re15_map_visited_export(z1); re15_map_visited_floor_export(f1);
        re15_map_visited_import(z1); re15_map_visited_floor_import(f1);
        miss(0x1120, 1300, 7300, 0, &m);
        snprintf(t, sizeof t, "Stand aus der Kabine geladen, im 3F-Raum: 3F-Kabine %s (keine Fahrt)",
                 zustand(re15_map_rect_state(4, 0)));
        CHECK(t, re15_map_rect_state(4, 0) == RE15_MAP_RECT_UNVISITED);
    }
    /* Fahrt nach 3F: aus der Kabine (1F) in ROOM1120 aussteigen (Kabinen-Tuer 2 @0x04C2 -> Spawn
     * (1300,0,7300)) - wer mit dem Fahrstuhl ankommt, WAR auf 3F im Fahrstuhl. */
    if (!durch_tuer(0x1040, -21786, -10300, 0, 0x1080, -13650, -900, 0, &vor, &m, &ax, &az)) {
        CHECK("Tuer ROOM1040 -> ROOM1080 (2)", 0); return 1;
    }
    miss(0x1120, 1300, 7300, 0, &m);
    snprintf(t, sizeof t, "mit dem Fahrstuhl auf 3F angekommen: 3F-Kabine %s, 2F-Kabine %s (nie dort)",
             zustand(re15_map_rect_state(4, 0)), zustand(re15_map_rect_state(3, 4)));
    CHECK(t, re15_map_rect_state(4, 0) == RE15_MAP_RECT_VISITED &&
             re15_map_rect_state(3, 4) == RE15_MAP_RECT_UNVISITED &&
             re15_map_rect_state(2, 9) == RE15_MAP_RECT_VISITED);
    /* zu Fuss weiter auf 2F (ROOM10C0, Vorraum 1120 = keine Kabine): 2F-Kabine bleibt unbesucht */
    miss(0x10C0, -6700, -2100, 0, &m);
    snprintf(t, sizeof t, "zu Fuss auf 2F: 2F-Kabine %s", zustand(re15_map_rect_state(3, 4)));
    CHECK(t, m.page == 3 && re15_map_rect_state(3, 4) == RE15_MAP_RECT_UNVISITED);
    /* auf 3F wieder einsteigen: jetzt (und nur jetzt) wird die 3F-Kabine besucht */
    if (!durch_tuer(0x1120, 1300, 6400, 0, 0x1080, -13650, -900, 0, &vor, &m, &ax, &az)) {
        CHECK("Tuer ROOM1120 -> ROOM1080", 0); return 1;
    }
    CHECK("aus 3F eingestiegen: Blatt 4, 3F-Kabine AKTUELL, 2F weiter UNBESUCHT",
          m.page == 4 && re15_map_rect_state(4, 0) == RE15_MAP_RECT_CURRENT &&
          re15_map_rect_state(3, 4) == RE15_MAP_RECT_UNVISITED);
    /* Speichern/Laden */
    re15_map_visited_export(zb); re15_map_visited_floor_export(fb);
    re15_map_visited_reset();
    re15_map_visited_import(zb); re15_map_visited_floor_import(fb);
    miss(0x1120, 1300, 7300, 0, &m);
    CHECK("nach Speichern/Laden: 1F + 3F besucht, 2F unbesucht",
          re15_map_rect_state(2, 9) == RE15_MAP_RECT_VISITED &&
          re15_map_rect_state(4, 0) == RE15_MAP_RECT_VISITED &&
          re15_map_rect_state(3, 4) == RE15_MAP_RECT_UNVISITED);
    /* ALTER SPIELSTAND: die frueheren Kabinen-Bits 3/4/5 (immer gemeinsam gesetzt) zaehlen nicht
     * mehr - die Kabine erscheint erst wieder, wenn man auf der Etage einsteigt. */
    {
        uint8_t alt[16];
        memset(alt, 0, sizeof alt);
        alt[0] = (uint8_t)((1u << 3) | (1u << 4) | (1u << 5));
        re15_map_visited_reset();
        re15_map_visited_mark(0x1080);
        re15_map_visited_floor_import(alt);
        miss(0x1120, 1300, 7300, 0, &m);
        snprintf(t, sizeof t, "alter Stand (alte Kabinen-Bits 3/4/5): 1F %s, 2F %s, 3F %s - keine "
                 "Kabine als besucht", zustand(re15_map_rect_state(2, 9)),
                 zustand(re15_map_rect_state(3, 4)), zustand(re15_map_rect_state(4, 0)));
        CHECK(t, re15_map_rect_state(2, 9) != RE15_MAP_RECT_VISITED &&
                 re15_map_rect_state(3, 4) != RE15_MAP_RECT_VISITED &&
                 re15_map_rect_state(4, 0) != RE15_MAP_RECT_VISITED);
    }
    /* Feld-Belegung: Tabellen-Bits < 96 <= Teil-Bits < 112 <= Fahrstuhl-Bits < 128 */
    snprintf(t, sizeof t, "Etagen-Tabelle (%d Bits) bleibt unter den Teil-Bits (%d), Teil-Tabelle "
             "(%d) unter den Fahrstuhl-Bits (%d), Fahrstuhl-Tabelle (%d) im Feld (%d)",
             re15_map_floor_bit_count(), RE15_KARTE_R36_TEIL_BASIS, re15_karte_r36_teil_bit_count(),
             RE15_KARTE_R36_FAHRSTUHL_BASIS, re15_karte_r36_fahrstuhl_bit_count(),
             re15_map_floor_bit_kapazitaet());
    CHECK(t, re15_map_floor_bit_count() <= RE15_KARTE_R36_TEIL_BASIS &&
             RE15_KARTE_R36_TEIL_BASIS + re15_karte_r36_teil_bit_count() <= RE15_KARTE_R36_FAHRSTUHL_BASIS &&
             RE15_KARTE_R36_FAHRSTUHL_BASIS + re15_karte_r36_fahrstuhl_bit_count() <=
                 re15_map_floor_bit_kapazitaet());
    /* Der zweite Fahrstuhl ROOM4020 (Blatt 8/9/10): aus ROOM5000 (Tuer slot 3 @0x008A4) */
    re15_map_visited_reset();
    re15_game_flag_set(3, 0x93, 0); re15_game_flag_set(3, 0x94, 0); re15_game_flag_set(3, 0x95, 0);
    if (!durch_tuer(0x5000, -10550, -15600, 0, 0x4020, -21400, -23400, 0, &vor, &m, &ax, &az)) {
        CHECK("Tuer ROOM5000 -> ROOM4020", 0); return 1;
    }
    snprintf(t, sizeof t, "4020 aus ROOM5000: Blatt %d, Blatt 9 %s, Blatt 8 %s, Blatt 10 %s",
             m.page, zustand(re15_map_rect_state(9, 13)), zustand(re15_map_rect_state(8, 2)),
             zustand(re15_map_rect_state(10, 3)));
    CHECK(t, m.page == 9 && re15_map_rect_state(9, 13) == RE15_MAP_RECT_CURRENT &&
             re15_map_rect_state(8, 2) == RE15_MAP_RECT_UNVISITED &&
             re15_map_rect_state(10, 3) == RE15_MAP_RECT_UNVISITED);
    return g_fail;
}

/* ---- Riegel 7 (Nachbesserung 1, Abnahme 0 M1): Kabine bei UNBEKANNTER Etage --------------------
 * Abnahme 0: Debug-Menue JUMP "108 ELEVATOR" (RE15_DEBUG_JUMP=1080@gp) aus ROOM1100 ohne
 * Bank-3-Bits 54/55/56 -> die Karte zeigte die Kabine auf 2F UND 3F als besucht, das 3F-Blatt wurde
 * dadurch anwaehlbar. Hier derselbe Weg: Spawn aus dem DEBUG.BIN-Tabellensatz
 * (re15_debug_menu_jump_spawn = JUMP-Executor FUN_8001d600, X @0x8001d6a8 / Z @0x8001d720 /
 * Band @0x8001d798), davor ein Raum, der KEIN Etagenraum der Kabine ist, keine Etagen-Bits des Spiels
 * (scd_vm_init in betrete() loescht alle Flags). Soll: die Kabine steht NUR auf dem Blatt des
 * Seiten-Setzers (1080: Blatt 2, `ori v0,zero,0x2` @0x8004b684 -> `sb v0,0x260e` @0x8004b88c;
 * 4020: Blatt 8, `ori v1,zero,0x8` @0x8004bdb4 -> `sb v1,0x260e` @0x8004bdbc), die anderen Etagen
 * bleiben unbesucht und als Blatt unbekannt; ein v8-Stand (Zonen-Bit ohne Etage) setzt kein
 * Kabinen-Etagen-Bit. */
static int debug_sprung(unsigned ziel, unsigned von, int32_t vx, int32_t vz, mess_t *m,
                        int32_t *sx, int32_t *sz)
{
    static uint8_t *dbin;
    static size_t dbsz;
    int band = 0;
    if (!dbin) {
        char p[600];
        snprintf(p, sizeof p, "%s/BIN/DEBUG.BIN", RE15_ASSET_PSX_DIR);
        dbin = slurp(p, &dbsz);
        if (!dbin) return 0;
        re15_debug_menu_reset();
        re15_debug_menu_set_bin(dbin, (unsigned)dbsz);
    }
    if (!re15_debug_menu_point_at(ziel) || !re15_debug_menu_jump_spawn(sx, sz, &band)) return 0;
    if (!betrete(von, vx, vz, 0)) return 0;
    miss_hier(von, vx, vz, m);
    if (!betrete(ziel, *sx, *sz, band)) return 0;
    miss_hier(ziel, *sx, *sz, m);
    return 1;
}

static int floor_bit_an(int b)
{
    uint8_t f[16];
    if (b < 0) return -1;
    re15_map_visited_floor_export(f);
    return (f[b >> 3] >> (b & 7)) & 1;
}

static uint32_t byte_summe(const void *p, size_t n)
{
    const uint8_t *b = (const uint8_t *)p;
    uint32_t s = 0;
    while (n--) s += *b++;
    return s;
}

/* Bit der HAUPT-Zonenzeile (ohne Etagen-Marke) eines Raums, -1 = keine. */
static int haupt_zonenbit(unsigned room)
{
    int i;
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(i);
        if (zn && zn->room == room && !zn->etage) return re15_map_zone_bit_test(i);
    }
    return -1;
}

static int teil_kabine(void)
{
    mess_t m;
    int32_t sx = 0, sz = 0;
    char t[320];
    const int k1 = re15_karte_r36_fahrstuhl_bit(0x1080, 2), k2 = re15_karte_r36_fahrstuhl_bit(0x1080, 3),
              k3 = re15_karte_r36_fahrstuhl_bit(0x1080, 4), l8 = re15_karte_r36_fahrstuhl_bit(0x4020, 8),
              l9 = re15_karte_r36_fahrstuhl_bit(0x4020, 9), l10 = re15_karte_r36_fahrstuhl_bit(0x4020, 10);
    printf("=== Riegel 7: Kabine bei unbekannter Etage (Debug-Sprung, v8-Hebung; Abnahme 0 M1) ===\n");
    s_nflags = 0;

    /* (a) JUMP "108 ELEVATOR" aus ROOM1100 (2F-Gang, Blatt 3 - kein Etagenraum der Kabine) */
    re15_map_visited_reset();
    if (!debug_sprung(0x1080, 0x1100, -26130, -11036, &m, &sx, &sz)) {
        CHECK("Debug-Sprung ROOM1100 -> ROOM1080 (DEBUG.BIN-Satz)", 0); return 1;
    }
    snprintf(t, sizeof t, "JUMP 108 ELEVATOR aus ROOM1100, Spawn DEBUG.BIN (%d,%d): gezeigt Blatt %d, "
             "1F-Kabine %s, 2F-Kabine %s, 3F-Kabine %s", (int)sx, (int)sz, m.page,
             zustand(re15_map_rect_state(2, 9)), zustand(re15_map_rect_state(3, 4)),
             zustand(re15_map_rect_state(4, 0)));
    CHECK(t, m.page == 2 && re15_map_rect_state(2, 9) == RE15_MAP_RECT_CURRENT &&
             re15_map_rect_state(3, 4) == RE15_MAP_RECT_UNVISITED &&
             re15_map_rect_state(4, 0) == RE15_MAP_RECT_UNVISITED);
    snprintf(t, sizeof t, "Kabinen-Etagen-Bits nach dem Sprung: 1F(%d)=%d 2F(%d)=%d 3F(%d)=%d - nur das "
             "Blatt des Seiten-Setzers (Blatt 2 @0x8004b684)", k1, floor_bit_an(k1), k2, floor_bit_an(k2),
             k3, floor_bit_an(k3));
    CHECK(t, floor_bit_an(k1) == 1 && floor_bit_an(k2) == 0 && floor_bit_an(k3) == 0);
    snprintf(t, sizeof t, "3F-Blatt (4) nach dem Sprung NICHT anwaehlbar (nie auf 3F) - bekannt %d",
             re15_map_page_known(4));
    CHECK(t, !re15_map_page_known(4));
    /* Fahrt nach 3F: aus der Kabine in ROOM1120 (Kabinen-Tuer 2 @0x04C2 -> Spawn (1300,0,7300)) */
    miss(0x1120, 1300, 7300, 0, &m);
    snprintf(t, sizeof t, "danach mit dem Fahrstuhl auf 3F angekommen: 3F-Kabine %s, 2F-Kabine %s, "
             "1F-Kabine %s", zustand(re15_map_rect_state(4, 0)), zustand(re15_map_rect_state(3, 4)),
             zustand(re15_map_rect_state(2, 9)));
    CHECK(t, re15_map_rect_state(4, 0) == RE15_MAP_RECT_VISITED &&
             re15_map_rect_state(3, 4) == RE15_MAP_RECT_UNVISITED &&
             re15_map_rect_state(2, 9) == RE15_MAP_RECT_VISITED);

    /* (b) der zweite Fahrstuhl: JUMP nach ROOM4020 aus ROOM4010 (Blatt 8, kein Etagenraum) */
    re15_map_visited_reset();
    re15_game_flag_set(3, 0x93, 0); re15_game_flag_set(3, 0x94, 0); re15_game_flag_set(3, 0x95, 0);
    if (!debug_sprung(0x4020, 0x4010, -22574, -21278, &m, &sx, &sz)) {
        CHECK("Debug-Sprung ROOM4010 -> ROOM4020 (DEBUG.BIN-Satz)", 0); return 1;
    }
    snprintf(t, sizeof t, "JUMP 4020 aus ROOM4010, Spawn DEBUG.BIN (%d,%d): gezeigt Blatt %d, Blatt 8 %s, "
             "Blatt 9 %s, Blatt 10 %s", (int)sx, (int)sz, m.page, zustand(re15_map_rect_state(8, 2)),
             zustand(re15_map_rect_state(9, 13)), zustand(re15_map_rect_state(10, 3)));
    CHECK(t, m.page == 8 && re15_map_rect_state(8, 2) == RE15_MAP_RECT_CURRENT &&
             re15_map_rect_state(9, 13) == RE15_MAP_RECT_UNVISITED &&
             re15_map_rect_state(10, 3) == RE15_MAP_RECT_UNVISITED);
    snprintf(t, sizeof t, "4020-Etagen-Bits: Blatt 8 (%d)=%d, 9 (%d)=%d, 10 (%d)=%d - nur Blatt 8 "
             "(@0x8004bdb4); Blatt 9/10 bekannt %d/%d", l8, floor_bit_an(l8), l9, floor_bit_an(l9), l10,
             floor_bit_an(l10), re15_map_page_known(9), re15_map_page_known(10));
    CHECK(t, floor_bit_an(l8) == 1 && floor_bit_an(l9) == 0 && floor_bit_an(l10) == 0 &&
             !re15_map_page_known(9) && !re15_map_page_known(10));

    /* (c) v8-Stand (Zonen-Bits ohne Etagen-Feld): die Kabinen waren besucht, die Etage steht
     * nirgends - kein Kabinen-Etagen-Bit (wie die alten v9-Bits 3/4/5, Riegel 6). */
    {
        static uint8_t roh[sizeof(re15_savedata_t)];
        re15_savedata_t sd;
        uint32_t ck;
        size_t off = offsetof(re15_savedata_t, visited_floor);
        int z80 = haupt_zonenbit(0x1080), z20 = haupt_zonenbit(0x4020), b, n = 0;
        memset(&sd, 0, sizeof sd);
        sd.magic = RE15_SAVE_MAGIC; sd.version = 8; sd.room = 0x1150; sd.player_hp = 100;
        if (z80 >= 0) sd.visited[z80 >> 3] |= (uint8_t)(1u << (z80 & 7));
        if (z20 >= 0) sd.visited[z20 >> 3] |= (uint8_t)(1u << (z20 & 7));
        memset(roh, 0xA5, sizeof roh);           /* Alt-Block: Pruefwort bei visited_floor */
        memcpy(roh, &sd, off);
        ck = byte_summe(roh, off);
        memcpy(roh + off, &ck, sizeof ck);
        memcpy(&sd, roh, sizeof sd);
        CHECK("v8-Stand mit Kabinen-Zonen-Bits (1080/4020) wird angenommen und gehoben",
              z80 >= 0 && z20 >= 0 && re15_savedata_validate(&sd) == 0 &&
              sd.version == RE15_SAVE_VERSION);
        for (b = RE15_KARTE_R36_FAHRSTUHL_BASIS;
             b < RE15_KARTE_R36_FAHRSTUHL_BASIS + re15_karte_r36_fahrstuhl_bit_count(); b++)
            if ((sd.visited_floor[b >> 3] >> (b & 7)) & 1) n++;
        snprintf(t, sizeof t, "v8-Hebung: Kabinen-Etagen-Bits %d..%d gesetzt %d (vorher alle 6)",
                 RE15_KARTE_R36_FAHRSTUHL_BASIS,
                 RE15_KARTE_R36_FAHRSTUHL_BASIS + re15_karte_r36_fahrstuhl_bit_count() - 1, n);
        CHECK(t, n == 0);
    }
    return g_fail;
}

/* ===================================================================================
 * WERKZEUG fuer den echten Lauf (tests/integration/test_r36_karte.cmake)
 * =================================================================================== */

/* karte <mcr> <hexraum> <x> <y> <z> <rot> [besucht:<hexraum>:<x>:<z>[:<band>] ...] [flag:<bank>:<bit>]
 *   Spielstand Slot 0: Spieler im Raum an (x,y,z) mit Blick rot; die genannten Orte werden der
 *   Reihe nach betreten (re15_map_zone_update wie beim Raumladen - die Reihenfolge zaehlt fuer
 *   die Fahrstuhl-Etage). */
static int teil_karte(int argc, char **argv)
{
    re15_savedata_t sd, back;
    re15_actor_t *pl;
    unsigned room;
    uint16_t rr = 0;
    int a;
    if (argc < 8) { printf("FAIL: karte <mcr> <raum> <x> <y> <z> <rot> ...\n"); return 2; }
    room = (unsigned)strtoul(argv[3], NULL, 16);
    scd_vm_init(); re15_actor_init(); re15_aot_init();
    re15_map_visited_reset();
    for (a = 8; a < argc; a++) {
        unsigned r2 = 0; int x2 = 0, z2 = 0, b2 = 0, b = 0, i = 0;
        int n = sscanf(argv[a], "besucht:%x:%d:%d:%d", &r2, &x2, &z2, &b2);
        if (n >= 3) {
            re15_collision_reset_band();
            re15_collision_set_band(n == 4 ? b2 : 0);
            g_actors[RE15_ACTOR_SLOT_PLAYER].x = x2; g_actors[RE15_ACTOR_SLOT_PLAYER].z = z2;
            re15_map_zone_update(r2, x2, z2);
            continue;
        }
        if (sscanf(argv[a], "flag:%d:%d", &b, &i) == 2) {
            re15_game_flag_set((uint8_t)b, (uint8_t)i, 1);
            continue;
        }
        printf("FAIL: unbekanntes Argument '%s'\n", argv[a]);
        return 2;
    }
    g_current_room_id = room;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = atoi(argv[4]); pl->y = atoi(argv[5]); pl->z = atoi(argv[6]);
    pl->rot_y = (int16_t)atoi(argv[7]);
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(argv[2], 0, &sd, "LEON  R36 KARTE") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", argv[2]);
        return 1;
    }
    if (re15_memcard_load(argv[2], 0, &back) != 0 || re15_savedata_restore(&back, &rr) != 0 ||
        rr != (uint16_t)room) {
        printf("FAIL: Ruecklesen (Raum 0x%04X)\n", (unsigned)rr);
        return 1;
    }
    printf("Karte %s: Raum 0x%04X an (%d,%d,%d) rot %d\n", argv[2], room, (int)pl->x, (int)pl->y,
           (int)pl->z, (int)pl->rot_y);
    return 0;
}

/* BMP (24 Bit, RE15_INV_FB_SHOT) laden: B,G,R je Pixel, Zeile 0 oben. */
static unsigned char *bmp_laden(const char *pfad, int *w, int *h)
{
    FILE *f = fopen(pfad, "rb");
    unsigned char hdr[54], *px;
    long off;
    int y;
    if (!f || fread(hdr, 1, 54, f) != 54) { if (f) fclose(f); return 0; }
    off = hdr[10] | (hdr[11] << 8) | (hdr[12] << 16) | ((long)hdr[13] << 24);
    *w = hdr[18] | (hdr[19] << 8); *h = hdr[22] | (hdr[23] << 8);
    if (*h < 0) *h = -*h;
    px = (unsigned char *)malloc((size_t)(*w) * (size_t)(*h) * 3u + 4u * (size_t)(*h));
    fseek(f, off, SEEK_SET);
    for (y = 0; y < *h; y++) {
        int stride = (*w * 3 + 3) & ~3;
        unsigned char *zeile = px + (size_t)(*h - 1 - y) * (size_t)(*w) * 3u;
        if (fread(zeile, 1, (size_t)(*w) * 3u, f) != (size_t)(*w) * 3u) break;
        fseek(f, stride - *w * 3, SEEK_CUR);
    }
    fclose(f);
    return px;
}

/* Farbklassen des Kartenabzugs (gemessen in den Runde-35-Abzuegen und hier im Sweep 2026-10-11):
 *   rot     aktuell: 0x680808 halbtransparent auf dem Grund -> (48,8,48)/(48,8,64)
 *   besucht (16,56,40)/(16,56,56)   wand (176,176,176)   gelb Tuerbalken (224,168,40)
 *   sprosse Treppe (240,240,216)    grund (0,16,88)/(0,16,120) */
static int klasse(const unsigned char *p)
{
    int r = p[2], g = p[1], b = p[0];
    if (r >= 40 && r <= 56 && g <= 12 && b >= 40 && b <= 72) return 1;          /* rot */
    if (r == 16 && g == 56 && (b == 40 || b == 56)) return 2;                    /* besucht */
    if (r == 224 && g == 168 && b == 40) return 3;                               /* gelb */
    if (r == 240 && g == 240 && b == 216) return 4;                              /* sprosse */
    if (r == 176 && g == 176 && b == 176) return 5;                              /* wand */
    return 0;
}

/* farben <bmp> <x0> <y0> <x1> <y1>  -> "FARBEN rot=.. besucht=.. gelb=.. sprosse=.. wand=.." */
static int teil_farben(int argc, char **argv)
{
    int w, h, x, y, x0, y0, x1, y1, n[6] = {0};
    unsigned char *px;
    if (argc < 7) { printf("FAIL: farben <bmp> x0 y0 x1 y1\n"); return 2; }
    px = bmp_laden(argv[2], &w, &h);
    if (!px) { printf("FAIL: %s nicht lesbar\n", argv[2]); return 1; }
    x0 = atoi(argv[3]); y0 = atoi(argv[4]); x1 = atoi(argv[5]); y1 = atoi(argv[6]);
    for (y = y0; y <= y1 && y < h; y++)
        for (x = x0; x <= x1 && x < w; x++)
            n[klasse(px + ((size_t)y * (size_t)w + (size_t)x) * 3u)]++;
    free(px);
    printf("FARBEN %dx%d (%d,%d)-(%d,%d) rot=%d besucht=%d gelb=%d sprosse=%d wand=%d\n", w, h,
           x0, y0, x1, y1, n[1], n[2], n[3], n[4], n[5]);
    return 0;
}

/* marker <bmp> <x0> <y0> <x1> <y1>: der Spieler-Marker = jedes Pixel ohne Kartenfarbe (Muster
 * probe_r35_karte marker: Ring 5x5 aus uv(224,128), modulierte Pulsfarbe) -> "MARKER mx my n" */
static int teil_marker(int argc, char **argv)
{
    int w, h, x, y, x0, y0, x1, y1, n = 0, bx0 = 9999, by0 = 9999, bx1 = -1, by1 = -1;
    unsigned char *px;
    if (argc < 7) { printf("FAIL: marker <bmp> x0 y0 x1 y1\n"); return 2; }
    px = bmp_laden(argv[2], &w, &h);
    if (!px) { printf("FAIL: %s nicht lesbar\n", argv[2]); return 1; }
    x0 = atoi(argv[3]); y0 = atoi(argv[4]); x1 = atoi(argv[5]); y1 = atoi(argv[6]);
    for (y = y0; y <= y1 && y < h; y++)
        for (x = x0; x <= x1 && x < w; x++) {
            const unsigned char *p = px + ((size_t)y * (size_t)w + (size_t)x) * 3u;
            if (klasse(p)) continue;
            if (p[2] == 0 && p[1] == 16 && (p[0] == 88 || p[0] == 120)) continue;   /* Grund */
            n++;
            if (x < bx0) bx0 = x;
            if (x > bx1) bx1 = x;
            if (y < by0) by0 = y;
            if (y > by1) by1 = y;
        }
    free(px);
    if (!n) { printf("MARKER fehlt im Fenster (%d,%d)-(%d,%d)\n", x0, y0, x1, y1); return 1; }
    printf("MARKER %d %d %d bbox (%d,%d)-(%d,%d)\n", (bx0 + bx1) / 2, (by0 + by1) / 2, n,
           bx0, by0, bx1, by1);
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "messung";
    if (!strcmp(teil, "karte")) return teil_karte(argc, argv);
    if (!strcmp(teil, "farben")) return teil_farben(argc, argv);
    if (!strcmp(teil, "marker")) return teil_marker(argc, argv);
    re15_map_visited_reset();
    re15_inv_map_stage_init(0, 6);
    if (!strcmp(teil, "messung")) return teil_messung();
    if (!strcmp(teil, "aots") && argc > 2) {   /* Diagnose: Tuer-AOTs eines Raums nach dem Laden */
        unsigned rid = (unsigned)strtoul(argv[2], NULL, 16);
        int k;
        if (!betrete(rid, 0, 0, 0)) { printf("RDT fehlt\n"); return 1; }
        for (k = 0; k < RE15_AOT_MAX; k++)
            if (g_aot.slots[k].active)
                printf("slot %2d typ %d (%d,%d) halb (%d,%d) ziel %02X/%02X spawn (%d,%d)\n", k,
                       g_aot.slots[k].type, (int)g_aot.slots[k].x, (int)g_aot.slots[k].z,
                       (int)g_aot.slots[k].half_w, (int)g_aot.slots[k].half_h,
                       g_aot.door_params[k].dest_stage, g_aot.door_params[k].dest_room,
                       (int)g_aot.door_params[k].spawn_x, (int)g_aot.door_params[k].spawn_z);
        return 0;
    }
    if (!strcmp(teil, "treppe")) return teil_treppe();
    if (!strcmp(teil, "marker11d0")) return teil_marker11d0();
    if (!strcmp(teil, "r1190")) return teil_r1190();
    if (!strcmp(teil, "tuer11c0")) return teil_tuer11c0();
    if (!strcmp(teil, "teile1110")) return teil_teile1110();
    if (!strcmp(teil, "fahrstuhl")) return teil_fahrstuhl();
    if (!strcmp(teil, "kabine")) return teil_kabine();
    printf("unbekannter Teil %s\n", teil);
    return 2;
}
