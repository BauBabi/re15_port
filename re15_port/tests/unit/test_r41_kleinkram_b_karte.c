/*
 * Runde 41 Spur E "kleinkram_b", Punkt 2: Karte B1 — der Marker springt vor der VORDERTUER der "Waffen Armory"
 * (= ROOM1190 Ostkammer, Nutzer "room 1090"; Runde 36 M R0) in den HINTEREN Teil (Halle).
 * Dossier analysis/befunde_runde41/E_kleinkram_b.md. Muster: test_r39_karten.c (betrete), test_r39_johnruf1041.c.
 *
 * MECHANIK (re15_inv_screen.c tuer_anziehen): im Zugbereich einer Tuer wird der Marker auf das Symbol DIESER Tuer
 * gezogen. Das Symbol wurde ueber "irgendeine Zone des ZIELRAUMS" gesucht; ROOM1190 hat zwei Zonen (Halle zid 26,
 * Ostkammer zid 110), s_map_marks fuehrt die Hallen-Marke zuerst -> an der Ostkammer-Tuer zog die HALLEN-Marke.
 * Die Sonde probe_r39_karten tickt die VM nicht (keine Tuer-AOTs, kein Zug) — hier wird getickt wie im Spiel.
 *
 * Aufruf: test_r41_kleinkram_b_karte <teil>
 *   mess      Marker entlang der Annaeherung an beide Gang-Tueren von ROOM1230 (Strom (4,0xF3), Ostkammer-Tuer
 *             (3,0x89)) und an den Ausgaengen in ROOM1190 — nur Ausgabe.
 *   pin       an jedem Annaeherungspunkt der Ostkammer-Tuer bleibt der Marker auf der Ostkammer-Marke (146,109)
 *             +-2 px und naehert sich der Hallen-Marke (127,115) nicht; Hallen-Tuer -> Hallen-Marke; Ausgaenge in
 *             1190 auf der eigenen Marke; ROOM1180 (ohne Strom: Ostkammer-Tuer ist Text) unveraendert.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_esp.h"
#include "re15_inv_screen.h"
/* re15_map_zone_update / re15_inv_map_marker: re15_inv_screen.h, re15_room.h */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static int g_fail = 0, g_n = 0;
#define PRUEF(bed, ...) do { g_n++; int _ok = (bed) ? 1 : 0; if (!_ok) g_fail++; \
    printf("  %s: ", _ok ? "PASS" : "FAIL"); printf(__VA_ARGS__); printf("\n"); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *n = (size_t)sz; return b;
}

static uint8_t   *s_roh = NULL;
static re15_rdt_t s_rdt;

/* Raum wie der Spielweg laden: Flags VOR dem Raumskript, dann main00/sub00 im Init-Lauf TICKEN (Tuer-AOTs). */
static int betrete(unsigned rid, int32_t px, int32_t pz, const int *flags)
{
    char pfad[600]; size_t n = 0;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    if (s_roh) { free(s_roh); s_roh = 0; }
    s_roh = slurp(pfad, &n);
    if (!s_roh) return 0;
    if (re15_rdt_parse(s_roh, n, &s_rdt) < 0) return 0;
    scd_vm_init();
    for (int i = 0; flags && flags[i] >= 0; i += 2) re15_game_flag_set((uint8_t)flags[i], (uint8_t)flags[i + 1], 1);
    re15_actor_init(); re15_aot_init(); re15_enemy_reset(); re15_enemy_ai_set_paused(1); re15_esp_fx_reset();
    g_current_room_id = rid; g_room_change.pending = 0;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = px; pl->y = 0; pl->z = pz;
    re15_collision_reset_band(); re15_collision_set_band(0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    scd_vm_tick();                      /* Init-Lauf: Door_aot_set der Gang-Tueren wird wirksam */
    return 1;
}

static int tuer_aktiv(int slot) { return g_aot.slots[slot].active && g_aot.slots[slot].type == RE15_AOT_TYPE_DOOR; }

/* Karte oeffnen und den Marker fuer (px,pz) im geladenen Raum lesen (Muster test_r35_karte.c miss_hier). */
static void marker(unsigned rid, int32_t px, int32_t pz, int16_t *mx, int16_t *my)
{
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = px; g_actors[RE15_ACTOR_SLOT_PLAYER].z = pz;
    re15_map_zone_update(rid, px, pz);
    re15_inv_screen_open();
    g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
    re15_inv_map_stage_init((int)((rid >> 12) & 0xfu) - 1, (int)((rid >> 4) & 0xffu));
    g_inv_screen.map_page = re15_inv_map_page_shown();
    *mx = *my = -99;
    re15_inv_map_marker(px, pz, (uint8_t)((rid >> 4) & 0xFF), mx, my);
}

/* Marken (re15_map_zones.h, Blatt 0, Runde 39 Spur G): exakte Projektionen der AOT-Mitten */
#define M_HALLE_X 127
#define M_HALLE_Y 115
#define M_OSTK_X  146
#define M_OSTK_Y  109
static int abs_i(int v) { return v < 0 ? -v : v; }

/* Annaeherung im Gang (ROOM1230, Strom + (3,0x89)): Ostkammer-Tuer Slot 4 Mitte (96,12161) hh 467, Zugbereich
 * hh*2+450 = 1384 -> z 12161..13545; begehbar bis etwa z 13078 (Trigger-Nordkante 12628 + Radius 450).
 * Hallen-Tuer Slot 2 Mitte (-8118,9355), Zug z 9355..10739, begehbar bis etwa 10272. */
static const int32_t k_ostk_z[]  = { 14000, 13700, 13500, 13300, 13100, 13078 };
static const int32_t k_halle_z[] = { 11200, 10900, 10700, 10500, 10300, 10272 };
static const int k_flags_1230[] = { 4, 0xF3, 3, 0x89, -1 };

static void teil_mess(void)
{
    int16_t mx, my;
    printf("== mess ==\n");
    if (!betrete(0x1230, 96, 14000, k_flags_1230)) { printf("FAIL: ROOM1230\n"); g_fail++; return; }
    printf("  ROOM1230: Slot 4 Tuer=%d (Ostkammer, (3,0x89)), Slot 2 Tuer=%d (Halle)\n", tuer_aktiv(4), tuer_aktiv(2));
    for (unsigned i = 0; i < sizeof k_ostk_z / sizeof k_ostk_z[0]; i++) {
        marker(0x1230, 96, k_ostk_z[i], &mx, &my);
        printf("  Gang -> Ostkammer-Tuer  (96,%5d): Marker (%d,%d)  [Ostk-Marke (%d,%d), Hallen-Marke (%d,%d)]\n",
               (int)k_ostk_z[i], mx, my, M_OSTK_X, M_OSTK_Y, M_HALLE_X, M_HALLE_Y);
    }
    for (unsigned i = 0; i < sizeof k_halle_z / sizeof k_halle_z[0]; i++) {
        marker(0x1230, -8118, k_halle_z[i], &mx, &my);
        printf("  Gang -> Hallen-Tuer  (-8118,%5d): Marker (%d,%d)\n", (int)k_halle_z[i], mx, my);
    }
    if (betrete(0x1190, 19515, -12109, k_flags_1230)) {
        marker(0x1190, 19515, -12109, &mx, &my); printf("  1190 Ostkammer-Ankunft (19515,-12109): Marker (%d,%d)\n", mx, my);
        marker(0x1190, 3174, -11182, &mx, &my);  printf("  1190 Hallen-Ankunft (3174,-11182): Marker (%d,%d)\n", mx, my);
    }
}

static void teil_pin(void)
{
    int16_t mx, my;
    printf("== pin ==\n");
    if (!betrete(0x1230, 96, 14000, k_flags_1230)) { PRUEF(0, "ROOM1230 ladbar"); return; }
    PRUEF(tuer_aktiv(4) && tuer_aktiv(2), "ROOM1230 (Strom, (3,0x89)): Slot 4 = Ostkammer-Tuer (@0x00CE6), Slot 2 = Hallen-Tuer (@0x00B54)");
    for (unsigned i = 0; i < sizeof k_ostk_z / sizeof k_ostk_z[0]; i++) {
        marker(0x1230, 96, k_ostk_z[i], &mx, &my);
        int d_ostk = abs_i(mx - M_OSTK_X) + abs_i(my - M_OSTK_Y);
        int d_halle = abs_i(mx - M_HALLE_X) + abs_i(my - M_HALLE_Y);
        PRUEF(d_ostk <= 4 && d_halle > d_ostk, "vor der Ostkammer-Tuer (96,%d): Marker (%d,%d) auf der Ostkammer-Marke (d=%d), nicht zur Halle (d=%d)",
              (int)k_ostk_z[i], mx, my, d_ostk, d_halle);
    }
    for (unsigned i = 0; i < sizeof k_halle_z / sizeof k_halle_z[0]; i++) {
        marker(0x1230, -8118, k_halle_z[i], &mx, &my);
        int d_halle = abs_i(mx - M_HALLE_X) + abs_i(my - M_HALLE_Y);
        PRUEF(d_halle <= 3, "vor der Hallen-Tuer (-8118,%d): Marker (%d,%d) auf der Hallen-Marke (d=%d)", (int)k_halle_z[i], mx, my, d_halle);
    }
    /* Ausgaenge in ROOM1190: Ostkammer Slot 6 -> Gang, Halle Slot 4 -> Gang */
    if (betrete(0x1190, 19515, -12109, k_flags_1230)) {
        marker(0x1190, 19515, -12109, &mx, &my);
        PRUEF(abs_i(mx - M_OSTK_X) + abs_i(my - M_OSTK_Y) <= 3, "1190 Ostkammer-Ankunft: Marker (%d,%d) auf der Ostkammer-Marke", mx, my);
        marker(0x1190, 3174, -11182, &mx, &my);
        PRUEF(abs_i(mx - M_HALLE_X) + abs_i(my - M_HALLE_Y) <= 3, "1190 Hallen-Ankunft: Marker (%d,%d) auf der Hallen-Marke", mx, my);
    } else PRUEF(0, "ROOM1190 ladbar");
    /* ROOM1180 (ohne Strom): Ostkammer-Tuer ist Text (Slot 4 sce 0 @0x00A94) - kein Zug dorthin, Hallen-Tuer zieht */
    {
        static const int k_1180[] = { -1 };
        if (betrete(0x1180, 96, 14000, k_1180)) {
            PRUEF(!tuer_aktiv(4) && tuer_aktiv(2), "ROOM1180 ohne Strom: Slot 4 keine Tuer, Slot 2 Hallen-Tuer");
            marker(0x1180, -8118, 10272, &mx, &my);
            PRUEF(abs_i(mx - M_HALLE_X) + abs_i(my - M_HALLE_Y) <= 3, "ROOM1180 vor der Hallen-Tuer: Marker (%d,%d) auf der Hallen-Marke", mx, my);
        } else PRUEF(0, "ROOM1180 ladbar");
    }
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "pin";
    if (!strcmp(teil, "mess")) teil_mess();
    else teil_pin();
    printf("%s: %d/%d bestanden\n", g_fail ? "FAIL" : "OK", g_n - g_fail, g_n);
    return g_fail ? 1 : 0;
}
