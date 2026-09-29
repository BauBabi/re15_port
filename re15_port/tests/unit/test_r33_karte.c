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
 * RE2 gibt mit dem Hinweis KEIN Blatt frei und kennt KEINEN Zielraum in der normalen Karte
 * (Dossier §2: Etagen-Gatter Bank 35 @0x8006D934/@0x8006D998, Setzer Raumeintritt
 * @0x800693B4 und Kartenaufnahme-Skripte; Zeichner FUN_8006E120 ohne Ziel-Zweig). Beides ist
 * PORT-WAHL auf Nutzerwunsch (engine/src/map_hint_common.c Abschnitt 4), abgeleitet aus dem
 * Szenen-Flag (3,94) (ROOM1150 sub08 @0x01110) und dem Besucht-Bit des Zielorts.
 *
 * EIN Programm, mehrere Modi (argv[1]):
 *   messen      MESSSONDE (kein Riegel): Zustand der normalen Karte nach dem Hinweis —
 *               welche Blaetter erblaetterbar, wohin HOCH/RUNTER fuehrt, wie die Zielkachel
 *               gezeichnet wird.
 *   etage       RIEGEL: ohne Hinweis ueberspringt RUNTER von 3F das nie betretene 2F; nach
 *               dem Hinweis fuehrt RUNTER 3F -> 2F (Se(4,4)) -> 1F, HOCH 1F -> 2F -> 3F; kein
 *               anderes Blatt wird frei; Besucht-/Etagen-Bits bleiben unberuehrt; ohne das
 *               Szenen-Flag (Elzas Lauf: (3,94) setzt kein Elza-Skript) nichts frei.
 *   markierung  RIEGEL: Zielkachel (156,76) 48x40 uv (208,80) auf Blatt 3 in der normalen
 *               Karte, CLUT 502/498 im Takt von RE2s Karten-Pulszaehler (Wechsel bei Schritt
 *               2 + 39k, je VBlank einer, Wanduhr), KEIN Hinweis-Ton, kein fremdes AKTUELL;
 *               im Hinweis-Schirm selbst ziel_aktiv = 0; nach dem Betreten von ROOM10F0 (und
 *               ebenso von Elzas ROOM10F1) aus, die Kachel steht dann in der Besucht-Regel.
 *   speicher    RIEGEL: Rundlauf re15_savedata_capture -> Zustand loeschen -> restore:
 *               Markierung und Freigabe kommen zurueck; nach dem Betreten des Ziels bleibt
 *               sie aus; Speicherformat unveraendert (Version 9, 944 Byte).
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

/* Mitschrift: Beginn der Zielkachel-Anzeige (Wanduhr), Bilder, in denen der Hinweis-Schirm
 * UND die Zielkachel zugleich aktiv waren (muss 0 bleiben). */
static int      g_z_on;
static uint64_t g_z_t0;
static int      g_ziel_im_hinweis;
static uint64_t g_zf_t[64]; static int g_zf_rot[64]; static int g_zf_n; static int g_zf_vor;

static void bild(uint16_t pressed)
{
    const uint64_t t = g_t_us;
    re15_host_clock_set_us(t);
    if (re15_map_hint_pending() >= 0 && re15_menu_request_map_hint(re15_map_hint_pending()))
        re15_map_hint_take();
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen()) re15_menu_fsm_tick(pressed, pressed);
    if (re15_menu_is_open()) re15_inv_screen_ecg_tick();
    re15_fade_tick();
    if (re15_menu_is_open() && g_inv_screen.hint_aktiv && g_inv_screen.ziel_aktiv) g_ziel_im_hinweis++;
    if (!g_z_on && re15_menu_is_open() && g_inv_screen.ziel_aktiv) {
        g_z_on = 1; g_z_t0 = t; g_zf_n = 0; g_zf_vor = g_inv_screen.ziel_rot;
    } else if (g_z_on && g_inv_screen.ziel_aktiv && g_inv_screen.ziel_rot != g_zf_vor) {
        if (g_zf_n < 64) { g_zf_t[g_zf_n] = t - g_z_t0; g_zf_rot[g_zf_n] = g_inv_screen.ziel_rot; g_zf_n++; }
        g_zf_vor = g_inv_screen.ziel_rot;
    }
    if (!(re15_menu_is_open() && g_inv_screen.ziel_aktiv)) g_z_on = 0;
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
static void karte_zu(void) { re15_menu_toggle(); bild(0); zu(); }   /* ein Spielbild nach dem Schliessen */

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

/* Eine Taste im offenen Kartenschirm; liefert das Blatt danach und ob ein CORE-Ton fiel. */
static int taste(uint16_t t, int *ton)
{
    int c0 = g_test_core_se_count;
    bild(t); bild(0);
    if (ton) *ton = (g_test_core_se_count != c0) ? g_test_core_se_last : -1;
    return g_inv_screen.map_page;
}

/* ============================================================================================ */
static int riegel_etage(void)
{
    uint8_t vis0[32], vis1[32], fl0[16], fl1[16];
    int ton = -1, p;
    grundstellung();
    int k = weg_ohne_2f();
    spieler_1150();
    CHECK(k > 0 && !re15_map_page_known(3) && re15_map_page_known(2) && re15_map_page_known(4),
          "Weg ohne 2F: %d Zeilen besucht; bekannt 1F=%d 2F=%d 3F=%d (Soll 1/0/1)", k,
          re15_map_page_known(2), re15_map_page_known(3), re15_map_page_known(4));

    /* vorher: 2F nicht waehlbar — RUNTER springt von 3F nach 1F (Messung [M3]) */
    karte_auf();
    CHECK(re15_menu_substate() == 1 && g_inv_screen.map_page == 4, "Karte auf, Blatt %d (Soll 4 = 3F)",
          g_inv_screen.map_page);
    p = taste(RE15_PAD_BIT_DOWN, &ton);
    CHECK(p == 2, "OHNE Hinweis: RUNTER 3F -> Blatt %d (Soll 2 = 1F, 2F nie betreten)", p);
    karte_zu();
    CHECK(!re15_map_blatt_waehlbar(3) && !re15_map_ziel_blatt_frei(3), "ohne Hinweis ist Blatt 3 nicht frei");

    re15_map_visited_export(vis0);
    re15_map_visited_floor_export(fl0);
    if (!hinweis_ablaufen()) return 77;
    re15_map_visited_export(vis1);
    re15_map_visited_floor_export(fl1);
    CHECK(memcmp(vis0, vis1, 32) == 0 && memcmp(fl0, fl1, 16) == 0,
          "Hinweis laesst Besucht- und Etagen-Bits unberuehrt (die Freigabe ist KEIN Besuch)");
    CHECK(!re15_map_page_known(3) && re15_map_ziel_blatt_frei(3) && re15_map_blatt_waehlbar(3),
          "nach dem Hinweis: Blatt 3 bekannt %d, frei %d, waehlbar %d (Soll 0/1/1)",
          re15_map_page_known(3), re15_map_ziel_blatt_frei(3), re15_map_blatt_waehlbar(3));
    {
        int ok = 1;
        for (int q = 0; q < 14; q++)
            if (q != 3 && re15_map_blatt_waehlbar((unsigned)q) != re15_map_page_known((unsigned)q)) ok = 0;
        CHECK(ok, "kein anderes Blatt wird frei (waehlbar == bekannt fuer alle Blaetter ausser 3)");
    }

    karte_auf();
    CHECK(g_inv_screen.map_page == 4, "Karte oeffnet weiter auf dem Blatt des Spielers (%d; RE2 @0x8006D6B8)",
          g_inv_screen.map_page);
    p = taste(RE15_PAD_BIT_DOWN, &ton);
    CHECK(p == 3 && ton == 4, "RUNTER 3F -> Blatt %d, Ton %d (Soll 3 = 2F, Se(4,4))", p, ton);
    p = taste(RE15_PAD_BIT_DOWN, &ton);
    CHECK(p == 2, "RUNTER 2F -> Blatt %d (Soll 2 = 1F)", p);
    p = taste(RE15_PAD_BIT_UP, &ton);
    CHECK(p == 3, "HOCH 1F -> Blatt %d (Soll 3 = 2F)", p);
    p = taste(RE15_PAD_BIT_UP, &ton);
    CHECK(p == 4, "HOCH 2F -> Blatt %d (Soll 4 = 3F)", p);
    karte_zu();

    /* Elzas Lauf: ROOM1151 hat die Szene nicht (kein sub08, Runde 30 §2.4), (3,94) setzt
     * kein RDT ausser ROOM1150 (Zensus r33_re15_flag_zensus.py). Ohne das Flag: nichts frei. */
    grundstellung();
    (void)weg_ohne_2f();
    spieler_in(0x1151, -17370, -11900);
    CHECK(re15_game_flag_get(3, 94) == 0 && !re15_map_blatt_waehlbar(3) && !re15_map_ziel_aktiv(NULL, NULL),
          "Elza/ohne Szenen-Flag: Blatt 3 nicht frei, keine Markierung");
    free(s_raw1150); s_raw1150 = NULL;
    return g_fail;
}

/* ============================================================================================ */
static int riegel_markierung(void)
{
    int pg = -1, rc = -1, x, y, w, h, u, v, ton = -1;
    grundstellung();
    (void)weg_ohne_2f();
    spieler_1150();
    CHECK(!re15_map_ziel_aktiv(NULL, NULL), "vor dem Hinweis keine Markierung");
    g_ziel_im_hinweis = 0;
    if (!hinweis_ablaufen()) return 77;
    CHECK(g_ziel_im_hinweis == 0, "im Hinweis-Schirm selbst ist ziel_aktiv nie gesetzt (%d Bilder)", g_ziel_im_hinweis);
    CHECK(re15_map_ziel_aktiv(&pg, &rc) && pg == 3 && rc == 9,
          "nach dem Hinweis markiert: Blatt %d Rechteck %d (Soll 3 / 9 = ROOM10F0, Hauptzeile)", pg, rc);
    re15_map_rect_geometry(3, 9, &x, &y, &w, &h);
    re15_map_rect_uv(3, 9, &u, &v);
    CHECK(x == 156 && y == 76 && w == 48 && h == 40 && u == 208 && v == 80,
          "Zielkachel (%d,%d) %dx%d uv (%d,%d) (Soll (156,76) 48x40 uv (208,80))", x, y, w, h, u, v);
    CHECK(re15_map_rect_state(3, 9) == RE15_MAP_RECT_UNVISITED && !re15_map_owned_page(3),
          "Ziel unbesucht (%d), Blatt 3 nicht im Besitz (%d) — die normale Regel zoege sie nicht",
          re15_map_rect_state(3, 9), re15_map_owned_page(3));

    /* Blatt 4 (eigenes Blatt): keine Zielkachel, Irons' Buero AKTUELL wie immer */
    karte_auf();
    {
        int n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
        int clut = -1, ib = kachel_op(s_ops, n, 4, 2, &clut);
        CHECK(g_inv_screen.map_page == 4 && ib >= 0 && clut == RE15_INV_CLUT_MAP_AKTUELL,
              "Blatt 4: Irons' Buero (4/2) CLUT 0x%04X (Soll AKTUELL 0x%04X)", clut, RE15_INV_CLUT_MAP_AKTUELL);
    }
    int p = taste(RE15_PAD_BIT_DOWN, &ton);
    CHECK(p == 3, "RUNTER -> Blatt %d (Soll 3)", p);

    /* 4 s Wanduhr auf Blatt 3: Zielkachel IMMER da, CLUT folgt der Phase, Wechsel im Takt */
    {
        int h0 = g_test_hint_se_count, c0 = g_test_core_se_count, ok = 1;
        int gesehen_rot = 0, gesehen_umriss = 0, flip_ok = 1, fremd = 0;
        CHECK(g_z_on, "Zielkachel-Anzeige laeuft seit t0=%llu us (Beginn der Kartenansicht)",
              (unsigned long long)g_z_t0);
        while (g_t_us - g_z_t0 < 4000000ull) {
            bild(0);
            int n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
            int clut = -1, zi = kachel_op(s_ops, n, 3, 9, &clut);
            int soll = g_inv_screen.ziel_rot ? RE15_INV_CLUT_MAP_AKTUELL : RE15_INV_CLUT_MAP_UNBESUCHT;
            if (zi < 0 || clut != soll) ok = 0;
            for (int i = 0; i < n; i++)
                if (i != zi && s_ops[i].kind == RE15_INV_OP_SPRT && s_ops[i].clut == RE15_INV_CLUT_MAP_AKTUELL) fremd++;
            if (g_inv_screen.ziel_rot) gesehen_rot++; else gesehen_umriss++;
        }
        /* Wechsel ab dem Beginn der Kartenansicht (auch die auf Blatt 4 vor dem RUNTER):
         * Schritt 2 (rot), 41 (Umriss), 80 (rot) ... = RE2s Zaehler von 10/1 aus. */
        for (int k = 0; k < g_zf_n; k++) {
            uint64_t s = 2 + 39 * (uint64_t)k;
            uint64_t sollt = (s * 1000000000ull + 59825) / 59826;
            int soll_rot = (k % 2) == 0;
            if (!(g_zf_t[k] >= sollt && g_zf_t[k] < sollt + g_dt_us) || g_zf_rot[k] != soll_rot) flip_ok = 0;
            if (k < 4)
                printf("     Wechsel %d: t=%llu us -> %s (Soll ab %llu us)\n", k, (unsigned long long)g_zf_t[k],
                       g_zf_rot[k] ? "rot" : "umriss", (unsigned long long)sollt);
        }
        CHECK(ok, "4 s auf Blatt 3: Zielkachel in jedem Bild, CLUT = ziel_rot ? 502 : 498");
        CHECK(gesehen_rot > 0 && gesehen_umriss > 0, "beide Phasen (rot %d / Umriss %d Bilder)", gesehen_rot, gesehen_umriss);
        CHECK(g_zf_n >= 6 && flip_ok, "%d Wechsel, je bei Schritt 2+39k / 59,826 Hz auf ein Bild genau (RE2 @0x8006D87C-D4)",
              g_zf_n);
        CHECK(g_test_hint_se_count == h0, "KEIN Hinweis-Ton Se(2,0x2B) in der normalen Karte (%d)", g_test_hint_se_count - h0);
        CHECK(g_test_core_se_count == c0, "keine CORE-Toene im Lauf (%d)", g_test_core_se_count - c0);
        CHECK(fremd == 0, "keine andere Kachel in AKTUELL auf Blatt 3 (%d)", fremd);
    }
    karte_zu();

    /* Bildrate: dieselben Wanduhr-Zeiten bei 60 und 144 Bildern je s */
    {
        static const uint64_t dts[2] = { 16667ull, 6944ull };
        for (int d = 0; d < 2; d++) {
            uint64_t alt = g_dt_us; g_dt_us = dts[d];
            karte_auf(); (void)taste(RE15_PAD_BIT_DOWN, NULL);
            while (g_t_us - g_z_t0 < 1500000ull) bild(0);
            uint64_t s1 = (2ull * 1000000000ull + 59825) / 59826, s2 = (41ull * 1000000000ull + 59825) / 59826;
            CHECK(g_zf_n >= 2 && g_zf_t[0] >= s1 && g_zf_t[0] < s1 + dts[d] && g_zf_t[1] >= s2 && g_zf_t[1] < s2 + dts[d],
                  "%s Bilder/s: Wechsel bei t=%llu / %llu us (Soll ab %llu / %llu us)", d ? "144" : "60",
                  (unsigned long long)g_zf_t[0], (unsigned long long)g_zf_t[1],
                  (unsigned long long)s1, (unsigned long long)s2);
            karte_zu();
            g_dt_us = alt;
        }
    }

    /* Ziel BETRETEN (Tuer aus ROOM10D0 setzt auf (8400,-350), ROOM10D0 main00 @0x01052) */
    spieler_in(0x10F0, 8400, -350);
    CHECK(!re15_map_ziel_aktiv(NULL, NULL) && re15_map_blatt_waehlbar(3) && re15_map_page_known(3),
          "ROOM10F0 betreten: Markierung aus, Blatt 3 bekannt");
    karte_auf();
    {
        int konst = 1, clut0 = -1;
        for (int f = 0; f < 60; f++) {
            bild(0);
            int n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
            int clut = -1, zi = kachel_op(s_ops, n, 3, 9, &clut);
            if (zi < 0 || g_inv_screen.ziel_aktiv) konst = 0;
            if (clut0 < 0) clut0 = clut; else if (clut != clut0) konst = 0;
        }
        CHECK(g_inv_screen.map_page == 3 && konst && clut0 == RE15_INV_CLUT_MAP_AKTUELL,
              "im Funkraum: Karte auf Blatt %d, Kachel 3/9 stetig CLUT 0x%04X (Soll AKTUELL, kein Blinken)",
              g_inv_screen.map_page, clut0);
    }
    karte_zu();
    spieler_1150();
    karte_auf(); (void)taste(RE15_PAD_BIT_DOWN, NULL);
    {
        int n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
        int clut = -1, zi = kachel_op(s_ops, n, 3, 9, &clut);
        CHECK(g_inv_screen.map_page == 3 && zi >= 0 && clut == RE15_INV_CLUT_MAP_BESUCHT && !g_inv_screen.ziel_aktiv,
              "zurueck in ROOM1150: Blatt 3, Kachel 3/9 BESUCHT (0x%04X), keine Markierung", clut);
    }
    karte_zu();

    /* Elzas Variante: ROOM10F1 teilt das Besucht-Bit (zone_bit maskiert room & ~1) */
    grundstellung();
    (void)weg_ohne_2f();
    spieler_1150();
    re15_game_flag_set(3, 94, 1);
    CHECK(re15_map_ziel_aktiv(NULL, NULL), "Flag gesetzt, Ziel unbesucht: markiert");
    spieler_in(0x10F1, 8400, -350);
    CHECK(!re15_map_ziel_aktiv(NULL, NULL), "ROOM10F1 (Elzas Funkraum) betreten: Markierung ebenfalls aus");
    free(s_raw1150); s_raw1150 = NULL;
    return g_fail;
}

/* ============================================================================================ */
static int riegel_speicher(void)
{
    static re15_savedata_t sd, sd2;
    uint16_t raum = 0;
    int pg = -1, rc = -1;
    CHECK(RE15_SAVE_VERSION == 9 && sizeof(re15_savedata_t) == 944,
          "Speicherformat unveraendert: Version %d, %u Byte (Runde 30: 9 / 944)", RE15_SAVE_VERSION,
          (unsigned)sizeof(re15_savedata_t));
    grundstellung();
    (void)weg_ohne_2f();
    spieler_1150();
    if (!hinweis_ablaufen()) return 77;
    CHECK(re15_map_ziel_aktiv(NULL, NULL) && re15_map_blatt_waehlbar(3), "vor dem Speichern: markiert, Blatt 3 frei");
    re15_savedata_capture(&sd, 4321, 7);

    /* laufenden Zustand loeschen (wie ein frischer Start) */
    memset(g_game.flags, 0, sizeof g_game.flags);
    re15_map_visited_reset();
    CHECK(!re15_map_ziel_aktiv(NULL, NULL) && !re15_map_blatt_waehlbar(3), "geloescht: keine Markierung, Blatt 3 zu");

    CHECK(re15_savedata_restore(&sd, &raum) == 0 && raum == 0x1150, "Laden rc=0, Raum %04X", raum);
    spieler_1150();
    CHECK(re15_map_ziel_aktiv(&pg, &rc) && pg == 3 && rc == 9 && re15_map_blatt_waehlbar(3) &&
          !re15_map_page_known(3),
          "nach dem Laden: markiert (%d/%d), Blatt 3 frei, nicht bekannt", pg, rc);
    karte_auf();
    {
        int p = taste(RE15_PAD_BIT_DOWN, NULL);
        for (int f = 0; f < 5; f++) bild(0);
        int n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
        int clut = -1, zi = kachel_op(s_ops, n, 3, 9, &clut);
        CHECK(p == 3 && zi >= 0 && g_inv_screen.ziel_aktiv, "geladener Stand: RUNTER -> Blatt %d, Zielkachel da (CLUT 0x%04X)", p, clut);
    }
    karte_zu();

    /* Ziel betreten, speichern, loeschen, laden: aus bleibt aus */
    spieler_in(0x10F0, 8400, -350);
    re15_savedata_capture(&sd2, 4400, 8);
    memset(g_game.flags, 0, sizeof g_game.flags);
    re15_map_visited_reset();
    CHECK(re15_savedata_restore(&sd2, &raum) == 0, "zweiter Stand geladen");
    spieler_1150();
    CHECK(!re15_map_ziel_aktiv(NULL, NULL) && re15_map_page_known(3) && re15_game_flag_get(3, 94) == 1,
          "Stand nach dem Betreten: keine Markierung, Blatt 3 bekannt, (3,94) gesetzt");
    free(s_raw1150); s_raw1150 = NULL;
    return g_fail;
}

int main(int argc, char **argv)
{
    const char *m = argc > 1 ? argv[1] : "messen";
    if (!strcmp(m, "messen")) return sonde_messen();
    if (!strcmp(m, "etage")) return riegel_etage();
    if (!strcmp(m, "markierung")) return riegel_markierung();
    if (!strcmp(m, "speicher")) return riegel_speicher();
    printf("unbekannter Modus %s\n", m);
    return 2;
}
