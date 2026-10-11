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
 *   bild <bmp> ...   Bildauswertung fuer tests/integration/test_r36_karte.cmake
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
#include "re15_savedata.h"
#include "re15_memcard.h"

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

/* Dieselbe Kette wie test_map_raum_live.c / test_r35_karte.c betrete(). */
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

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "messung";
    re15_map_visited_reset();
    re15_inv_map_stage_init(0, 6);
    if (!strcmp(teil, "messung")) return teil_messung();
    printf("unbekannter Teil %s\n", teil);
    return 2;
}
