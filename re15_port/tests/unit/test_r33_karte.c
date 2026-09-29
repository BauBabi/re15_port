/* test_r33_karte.c — Runde 33, Thema K: die Karte NACH dem Irons-Hinweis.
 *
 * Nutzer-Auftrag (analysis/befunde_runde33/AUFTRAG.md, woertlich): "Nach der Cutscene mit
 * Irons und dem Anzeigen des Communication Room, wo man hin soll, muss man hinterher noch in
 * der Lage sein bei der Map zu 2F zu wechseln, um den Raum zu sehen, auch wenn man noch nicht
 * auf 2F war. Ausserdem muss der Raum irgendwie angezeigt bleiben, wie bei Resident Evil 2 bei
 * Zielraeumen auch."
 *
 * Dossier: analysis/befunde_runde33/karte_zielraum.md.
 *
 * EIN Programm, mehrere Modi (argv[1]):
 *   messen    MESSSONDE (kein Riegel): Zustand der normalen Karte nach dem Hinweis —
 *             welche Blaetter erblaetterbar, wohin HOCH/RUNTER fuehrt, wie die Zielkachel
 *             gezeichnet wird.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_menu.h"
#include "re15_fade.h"
#include "re15_inventory.h"
#include "re15_inv_screen.h"
#include "re15_map_owned.h"
#include "re15_map_hint.h"
#include "re15_collision.h"
#include "re15_savedata.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;
extern int g_test_core_se_last, g_test_core_se_count;
extern int g_test_hint_se_last, g_test_hint_se_count;

static int g_fail = 0;
#define CHECK(c, ...) do { if (c) { printf("  PASS: "); printf(__VA_ARGS__); printf("\n"); } \
                           else { printf("  FAIL: "); printf(__VA_ARGS__); printf("\n"); g_fail = 1; } } while (0)

#define EVT_END   0x12ECu   /* ROOM1150.RDT: 01 00 Evt_end von sub08 = Anker des Hinweises */

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return NULL; }
    fclose(f);
    *out_size = (size_t)sz;
    return buf;
}

static uint8_t *load_room(unsigned room, size_t *sz, re15_rdt_t *rdt)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (room >> 12) & 0xF, room);
    uint8_t *raw = read_file(p, sz);
    if (!raw) { printf("  SKIP: %s fehlt\n", p); return NULL; }
    if (re15_rdt_parse(raw, *sz, rdt) < 0) { printf("  FAIL: RDT-Parse %s\n", p); g_fail = 1; free(raw); return NULL; }
    return raw;
}

/* Spieler in ROOM1150 an den Debug-JUMP-Spawn (DEBUG.BIN @0x0285C: X=-17370 Z=-11900) */
static void spieler_in(unsigned room, int32_t x, int32_t z)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_current_room_id = room;
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = x; pl->y = 0; pl->z = z; pl->rot_y = 0;
    re15_collision_set_band(0);
    re15_map_zone_update(room, pl->x, pl->z);
}
static void spieler_1150(void) { spieler_in(0x1150, -17370, -11900); }

/* Den Weg bis Irons' Buero nachstellen, OHNE 2F zu betreten: alle Haupt-Zeilen (etage == 0)
 * der Blaetter 2 (1F), 4 (3F) und 5 (Dach) werden besucht, ausser Orten, die irgendeine Zeile
 * auf Blatt 3 (2F) fuehren (sonst waere 2F ueber das geteilte Zonen-Bit bekannt). */
static int ort_auf_blatt(unsigned room, unsigned idx, unsigned page)
{
    int n = re15_map_zone_count();
    for (int i = 0; i < n; i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        if (z && (z->room & ~1u) == (room & ~1u) && z->idx == idx && z->page == page) return 1;
    }
    return 0;
}
static int weg_ohne_2f(void)
{
    int n = re15_map_zone_count(), k = 0;
    for (int i = 0; i < n; i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        if (!z || z->etage) continue;
        if (z->page != 2 && z->page != 4 && z->page != 5) continue;
        if (ort_auf_blatt(z->room, z->idx, 3)) continue;
        if (z->room & 1u) continue;                          /* Leons Raeume */
        re15_map_visited_mark_at(z->room, (z->wx0 + z->wx1) / 2, (z->wz0 + z->wz1) / 2);
        k++;
    }
    return k;
}

/* ---- Bild-Modell wie test_r30_hinweis.c (Spiel-Haken -> START-Poll -> FSM-Tick) ----------- */
static uint64_t g_t_us = 5000000ull;
static uint64_t g_dt_us = 33333ull;

static void bild(uint16_t pressed)
{
    re15_host_clock_set_us(g_t_us);
    if (re15_map_hint_pending() >= 0 && re15_menu_request_map_hint(re15_map_hint_pending()))
        re15_map_hint_take();
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen()) re15_menu_fsm_tick(pressed, pressed);
    if (re15_menu_is_open()) re15_inv_screen_ecg_tick();
    re15_fade_tick();
    g_t_us += g_dt_us;
}

static re15_rdt_t s_rdt1150;
static uint8_t   *s_raw1150;
static int anfordern(void)
{
    if (!s_raw1150) {
        size_t sz = 0;
        s_raw1150 = load_room(0x1150, &sz, &s_rdt1150);
        if (!s_raw1150) return 0;
    }
    g_current_room_id = 0x1150;
    re15_map_hint_room_scan(s_raw1150, (int)s_rdt1150.raw_size, 0x1150);
    re15_map_hint_pc(s_raw1150 + EVT_END);
    return re15_map_hint_pending() == 0;
}

static void grundstellung(void)
{
    re15_actor_init(); scd_vm_init(); re15_aot_init();
    re15_inv_init();
    re15_fade_init();
    re15_map_visited_reset();
    memset(g_game.flags, 0, sizeof g_game.flags);
    spieler_1150();
}

static void zu(void)
{
    for (int f = 0; f < 200 && (re15_menu_is_open() || re15_menu_stage() != 0); f++) bild(0);
}

/* Den Hinweis so ablaufen lassen, wie ihn das Spiel zeigt: sub08 setzt (3,94) an seinem
 * ANFANG (ROOM1150.RDT @0x01110 `22 03 5e 01`), der Hinweis kommt an seinem Evt_end. */
static int hinweis_ablaufen(void)
{
    re15_game_flag_set(3, 94, 1);
    if (!anfordern()) return 0;
    for (int f = 0; f < 200 && !(re15_menu_is_open() && re15_menu_phase() == 1); f++) bild(0);
    for (int f = 0; f < 60; f++) bild(0);
    bild(RE15_PAD_BIT_START);
    zu();
    return 1;
}

/* Normale Karte oeffnen: Statusschirm + L1 (wie riegel_spurlos in test_r30_hinweis.c). */
static void karte_auf(void)
{
    re15_menu_toggle();
    bild(RE15_PAD_BIT_L1);
    for (int f = 0; f < 40; f++) bild(0);
}
static void karte_zu(void) { re15_menu_toggle(); zu(); }

static re15_inv_op_t s_ops[RE15_INV_MAX_OPS];
static int kachel_op(const re15_inv_op_t *ops, int n, int page, int rect, int *clut)
{
    int x, y, w, h, u, v;
    if (!re15_map_rect_geometry((unsigned)page, (unsigned)rect, &x, &y, &w, &h)) return -1;
    if (!re15_map_rect_uv((unsigned)page, (unsigned)rect, &u, &v)) return -1;
    for (int i = 0; i < n; i++) {
        const re15_inv_op_t *o = &ops[i];
        if (o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_MAP4 &&
            o->x == x && o->y == y && o->w == w && o->h == h && o->u == u && o->v == v) {
            if (clut) *clut = o->clut;
            return i;
        }
    }
    return -1;
}
static const char *zname(int s)
{
    return s == RE15_MAP_RECT_CURRENT ? "CURRENT" : s == RE15_MAP_RECT_VISITED ? "VISITED"
         : s == RE15_MAP_RECT_UNVISITED ? "UNVISITED" : "UNMAPPED";
}

/* ============================================================================================ */
static int sonde_messen(void)
{
    grundstellung();
    int k = weg_ohne_2f();
    spieler_1150();
    int pg = -1, rc = -1;
    re15_map_hint_ziel(0, &pg, &rc);
    printf("[M0] Weg ohne 2F: %d Haupt-Zeilen besucht (Blaetter 2/4/5); Ziel = Blatt %d Rechteck %d\n", k, pg, rc);
    printf("[M1] vor dem Hinweis: flag(3,94)=%d  bekannt:", re15_game_flag_get(3, 94));
    for (int p = 0; p < 13; p++) printf(" %d:%d", p, re15_map_page_known((unsigned)p));
    printf("\n");
    if (!hinweis_ablaufen()) return 77;
    printf("[M2] nach dem Hinweis: flag(3,94)=%d  offen=%d  bekannt:", re15_game_flag_get(3, 94), re15_menu_is_open());
    for (int p = 0; p < 13; p++) printf(" %d:%d", p, re15_map_page_known((unsigned)p));
    printf("\n");
    printf("[M2] Ziel Blatt %d Rechteck %d: Zustand %s, Blatt im Besitz %d\n", pg, rc,
           zname(re15_map_rect_state((unsigned)pg, (unsigned)rc)), re15_map_owned_page((unsigned)pg));

    karte_auf();
    printf("[M3] normale Karte: substate %d, Blatt %d\n", re15_menu_substate(), g_inv_screen.map_page);
    const uint16_t tasten[4] = { RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN, RE15_PAD_BIT_UP, RE15_PAD_BIT_UP };
    const char *tn[4] = { "RUNTER", "RUNTER", "HOCH", "HOCH" };
    for (int t = 0; t < 4; t++) {
        int c0 = g_test_core_se_count;
        bild(tasten[t]); bild(0);
        int n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
        int clut = -1, zi = kachel_op(s_ops, n, pg, rc, &clut);
        printf("[M3] %-6s -> Blatt %d (Ton %s)  Zielkachel in der Op-Liste: %s (CLUT 0x%04X)\n", tn[t],
               g_inv_screen.map_page, g_test_core_se_count != c0 ? "ja" : "nein",
               (g_inv_screen.map_page == pg) ? (zi >= 0 ? "ja" : "NEIN") : "-", zi >= 0 ? clut : 0);
    }
    karte_zu();
    free(s_raw1150); s_raw1150 = NULL;
    return 0;
}

int main(int argc, char **argv)
{
    const char *m = argc > 1 ? argv[1] : "messen";
    if (!strcmp(m, "messen")) return sonde_messen();
    printf("unbekannter Modus %s\n", m);
    return 2;
}
