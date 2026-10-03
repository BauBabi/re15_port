/* =============================================================================
 * Runde 35 Spur G "karte" — Mess-Sonde und Riegel fuer die vier Kartenbefunde
 * =============================================================================
 * Nutzer 2026-10-03 (analysis/befunde_runde35/AUFTRAG.md, woertlich):
 *   1. "Beim Elevator ROOM 1080 bewegt sich auf der Map der Player Cursor nicht."
 *   2. "In ROOM 11F0 taucht nicht auf der Karte auf, wenn man drin ist."
 *      "In ROOM 1200 taucht nicht auf der Karte auf, wenn man drin ist."
 *   3. "In ROOM 1230 bekomme ich die Map von ROOM 11E0."
 *   4. "In ROOM 1210 ist der Korridor falsch und so gut wie alle Tueren fehlen"
 * Dossier: analysis/befunde_runde35/G_karte.md.
 *
 * Gemessen wird der ECHTE Pfad, den das Spiel beim Oeffnen der Karte faehrt (wie
 * tests/integration/test_map_raum_live.c): Raum laden (scd_room_reenter), Spieler
 * setzen, Zone nachfuehren, Kartenschirm oeffnen, gezeigte Seite + Zustand jedes
 * Rechtecks + Spieler-Marker auslesen und die echte Op-Liste rastern.
 *
 * Aufruf: test_r35_karte <teil>
 *   messung   Mess-Ausgabe (kein Urteil) fuer alle vier Punkte
 *   fahrstuhl Riegel Punkt 1   b2  Riegel Punkt 2   r1230  Riegel Punkt 3
 *   r1210     Riegel Punkt 4
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

/* Dieselbe Kette wie test_map_raum_live.c betrete(). */
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
    int ok;              /* Raum geladen */
    int zone;            /* 1 = es gibt eine aktuelle Zone */
    int zpage, zrect, zid;
    int page;            /* gezeigte Seite */
    int cur[16];         /* Rechtecke der gezeigten Seite im Zustand CURRENT */
    int ncur;
    int16_t mx, my;      /* Spieler-Marker */
} mess_t;

/* Oeffnet die Karte im Raum rid an (px,pz) und misst. Die Position wird NUR ueber
 * re15_map_zone_update nachgefuehrt (wie game_step_common je Bild), nicht neu geladen. */
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
    /* wie menu_common.c map_entry(): Seiten-Setzer je Stage (@0x80074c0c), dann die
     * gezeigte Seite */
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
    printf("  %-22s ROOM%04X (%6d,%6d): ", was, rid, (int)px, (int)pz);
    if (!m->ok) { printf("RDT fehlt\n"); return; }
    if (m->zone) printf("Zone Blatt %d rect %d zid %d | ", m->zpage, m->zrect, m->zid);
    else         printf("KEINE Zone | ");
    printf("gezeigt Blatt %d | aktuell:", m->page);
    for (i = 0; i < m->ncur; i++) printf(" %d", m->cur[i]);
    if (!m->ncur) printf(" -");
    printf(" | Marker (%d,%d)\n", m->mx, m->my);
}

/* ---- Punkt 1: Fahrstuhl -------------------------------------------------------- */
/* Kabinen-Innenraum aus der SCA von ROOM1080.RDT: Waende x -16750..-15750 / -11550..-10550,
 * z -5150..-4150 / -50..950 -> begehbar x -15750..-11550, z -4150..-50; Ankunft aller drei
 * Etagen-Tueren bei (-13650,-900) (ROOM1040 @0x1096, ROOM10C0 @0xE82, ROOM1120 @0xCB6). */
static const int32_t s_kabine[][2] = {
    { -13650,  -900 }, { -15500, -3900 }, { -11800, -3900 },
    { -15500,  -300 }, { -11800,  -300 }, { -13650, -2100 },
};

/* Etage der Kabine: Bank 3 Bit 54/55/56 (ROOM1040 @0x15D6 `22 03 36 01`,
 * ROOM10C0 @0x0FEE `22 03 37 01`, ROOM1120 @0x0D6C `22 03 38 01`). */
static void kabine_auf(int etage)
{
    re15_game_flag_set(3, 54, etage == 1);
    re15_game_flag_set(3, 55, etage == 2);
    re15_game_flag_set(3, 56, etage == 3);
}

static int teil_messung(void)
{
    mess_t m;
    int e, k;
    printf("=== Punkt 1: Fahrstuhl ROOM1080 ===\n");
    for (e = 1; e <= 3; e++) {
        char was[64];
        if (!betrete(0x1080, s_kabine[0][0], s_kabine[0][1], 0)) { printf("  RDT fehlt\n"); break; }
        kabine_auf(e);
        for (k = 0; k < (int)(sizeof s_kabine / sizeof s_kabine[0]); k++) {
            snprintf(was, sizeof was, "Kabine %dF Punkt %d", e, k);
            miss_hier(0x1080, s_kabine[k][0], s_kabine[k][1], &m);
            druck(was, 0x1080, s_kabine[k][0], s_kabine[k][1], &m);
        }
    }
    printf("=== Punkt 2: ROOM11F0 / ROOM1200 (Blatt 1 = B2) ===\n");
    miss(0x11F0, 250, 250, 0, &m);       druck("11F0 Ankunft", 0x11F0, 250, 250, &m);
    miss(0x11F0, 7000, -12000, 0, &m);   druck("11F0 Mitte", 0x11F0, 7000, -12000, &m);
    miss(0x1200, -20154, -25245, 0, &m); druck("1200 Ankunft", 0x1200, -20154, -25245, &m);
    miss(0x1200, -22000, -15000, 0, &m); druck("1200 Mitte", 0x1200, -22000, -15000, &m);
    miss(0x11E0, -24707, -9442, 0, &m);  druck("11E0 (von 11F0)", 0x11E0, -24707, -9442, &m);
    printf("=== Punkt 3: ROOM1230 / ROOM1180 ===\n");
    miss(0x1230, -8133, 10226, 0, &m);   druck("1230 (von 1190 s4)", 0x1230, -8133, 10226, &m);
    miss(0x1230, 4725, 28650, 0, &m);    druck("1230 (von 11B0)", 0x1230, 4725, 28650, &m);
    miss(0x1230, -4729, -16018, 0, &m);  druck("1230 (von 11D0)", 0x1230, -4729, -16018, &m);
    miss(0x1180, -8133, 10226, 0, &m);   druck("1180 (von 1190 s4)", 0x1180, -8133, 10226, &m);
    miss(0x1180, 4725, 28650, 0, &m);    druck("1180 (von 11B0)", 0x1180, 4725, 28650, &m);
    miss(0x1180, -4729, -16018, 0, &m);  druck("1180 (von 11D0)", 0x1180, -4729, -16018, &m);
    printf("=== Punkt 4: ROOM1210 / ROOM1220 ===\n");
    miss(0x1210, -26400, -2200, 0, &m);  druck("1210 (von 11E0)", 0x1210, -26400, -2200, &m);
    miss(0x1210, -20890, -6560, 0, &m);  druck("1210 (von 1220 s0)", 0x1210, -20890, -6560, &m);
    miss(0x1210, -18100, -25600, 0, &m); druck("1210 (von 1220 s4)", 0x1210, -18100, -25600, &m);
    miss(0x1220, -22400, -6500, 0, &m);  druck("1220 Zelle s1", 0x1220, -22400, -6500, &m);
    miss(0x1220, -16600, -9900, 0, &m);  druck("1220 Zelle s2", 0x1220, -16600, -9900, &m);
    miss(0x1220, -22400, -14000, 0, &m); druck("1220 Zelle s3", 0x1220, -22400, -14000, &m);
    miss(0x1220, -16600, -17900, 0, &m); druck("1220 Zelle s4", 0x1220, -16600, -17900, &m);
    miss(0x1220, -16600, -25050, 0, &m); druck("1220 Zelle s5", 0x1220, -16600, -25050, &m);
    {   /* Tuermarken auf Blatt 1 (sichtbar = re15_map_mark_get liefert 1) */
        int i, n = re15_map_mark_count();
        printf("  Marken Blatt 1 (alle Blaetter aufgedeckt):\n");
        re15_map_debug_reveal_page(1);
        for (i = 0; i < n; i++) {
            int pg, rc, mx, my, kind, za, zb;
            int sicht = re15_map_mark_get(i, &pg, &rc, &mx, &my, &kind);
            if (pg != 1) continue;
            re15_map_mark_zonen(i, &za, &zb);
            printf("    #%d rect %d (%d,%d) kind %d zid %d/%d sichtbar %d\n",
                   i, rc, mx, my, kind, za, zb, sicht);
        }
    }
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
