/* test_r41_kollision.c — Runde 41 Spur H "kollision" (Dossier analysis/befunde_runde41/H_kollision.md).
 *
 * Nutzer: (1) "Die NPCs z.B. Ada und Marvin koennen ineinander rein laufen, das ist falsch, die sind beide fest."
 *         (2) "In ROOM 2050 lassen sich die Kisten in einander schieben, das ist falsch"
 *
 * Teile (Aufruf: test_r41_kollision <teil>):
 *   npc       MECHANIK des NPC-Wurzel-Schwanzes (re15_kollision_r41_npc_schwanz = FUN_8002aec4(&Spieler, NPC)
 *             @0x8011cc3c + FUN_8002b544 @0x8011cc44): exakte Schub-Formel dX*pen/(dist+1) (@0x8002b22c-238),
 *             nur der GESCHOBENE (a1) bewegt sich, Spieler-Schub, Gates (toter Spieler, Leiche, Y-Band
 *             @0x8002b1f0-b228), Radien je Kasten (0x45 500 / 0x4b 300).
 *   raum3070  echte ROOM3070.RDT, sub00 mit (4,215)/(4,236)/(3,180) = 1: Ada (0x42) + Marvin (0x40) spawnen als ESKORTE
 *             (Verhaltensbyte 0x00, sub00 @0x0343C/@0x03450) und laufen zum SELBEN Folgepunkt (FUN_8005070c, 1500
 *             hinter dem Spieler @0x8011ce78). Gemessen ueber re15_enemy_ai_run_all: sie treffen sich (Abstand < 1000
 *             = nicht vakuant), bleiben aber getrennt (Abstand >= 880 nach dem Einschwingen; Rest = Wandklemme NACH dem
 *             Schub @0x8011cc64) — vor Runde 41 gemessen: 14..23.
 *   raum3001  dasselbe fuer Elza: ROOM3001 sub00 @0x015A2/@0x015B6 John (0x49) + Sherry (0x4b), (4,198)/(4,199) = 1.
 *   kiste     FUN_8002cdf4 (@0x8002cdf4-cfd0) an den echten ROOM2050-Kisten (sub00 @0x01DB4/@0x01DD6/@0x01DF8): Grenze
 *             |dX| <= hx+hx inklusiv (@0x8002ce80-8c), Gate Bit 0x40 (@0x8002ce0c-24), Y (@0x8002ced0-e0), vertikaler
 *             Eintritt (@0x8002cf18-3c) und der Durchlauf @0x8002c03c-c0a8 (obj-Nummer 0..nOmodel-1); dazu die
 *             Raetsel-Endlage (drei Kisten in den Kerben slot 3/4/5) ohne gegenseitige Sperre.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_collision.h"
#include "re15_damage.h"          /* re15_enemy_apply_hitbox */
#include "re15_kollision_r41.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void scd_register_current_rdt(const re15_rdt_t *rdt);
extern void scd_thread_start(int slot, const uint8_t *pc);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int32_t abstand(const re15_actor_t *a, const re15_actor_t *b)
{
    int64_t dx = (int64_t)a->x - b->x, dz = (int64_t)a->z - b->z;
    int64_t q = dx * dx + dz * dz, r = 0;
    while ((r + 1) * (r + 1) <= q) r++;
    return (int32_t)r;
}

static void npc_setzen(int slot, uint8_t typ, int32_t x, int32_t y, int32_t z)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = typ; e->state = 1; e->hp = -1;
    e->x = x; e->y = y; e->z = z;
    re15_enemy_apply_hitbox(e, typ);
}

/* ------------------------------------------------------------------------------------------------ npc -- */
static void teil_npc(void)
{
    printf("=== (1) NPC-Wurzel-Schwanz: FUN_8002aec4(&Spieler,NPC) @0x8011cc3c + FUN_8002b544 @0x8011cc44 ===\n");
    re15_actor_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 50000; pl->y = 0; pl->z = 50000;
    re15_player_apply_hitbox(pl);

    /* (a) Ada (Slot 1) bei (0,0), Marvin (Slot 2) bei (500,0): R = 450+450 (Kasten @0x80121658 +6), dist 500 =
     *     SquareRoot0(250000) @0x8002b1d8, pen 400 (@0x8002b1e4), Schub dX*pen/(dist+1) = 500*400/501 = 399 (div
     *     @0x8002b238 schneidet zur Null) -> Marvin x 899; Ada bleibt (sie ist in Marvins Tick der SCHIEBER). */
    npc_setzen(1, 0x42, 0, 0, 0);
    npc_setzen(2, 0x40, 500, 0, 0);
    CHECK(g_actors[1].hit_radius_min == 450 && g_actors[2].hit_radius_min == 450,
          "Kasten +6 = 450 fuer 0x42/0x40 (@0x80121658 `c2 01`)");
    re15_kollision_r41_npc_schwanz(2);
    CHECK(g_actors[2].x == 899 && g_actors[2].z == 0,
          "Marvin aus Ada geschoben: x 500 -> %d (erwartet 500 + 500*400/501 = 899), z %d", (int)g_actors[2].x,
          (int)g_actors[2].z);
    CHECK(g_actors[1].x == 0 && g_actors[1].z == 0, "Ada unbewegt (%d,%d) — FUN_8002aec4 bewegt nur a1",
          (int)g_actors[1].x, (int)g_actors[1].z);

    /* (b) Spieler-Schub: Ada bei (0,0), Spieler bei (0,600): pen 300, dz -600 -> -600*300/601 = -299. Marvin steht
     *     bei 899 (pen 1 -> -899*1/900 = 0, kein Versatz). */
    pl->x = 0; pl->z = 600;
    re15_kollision_r41_npc_schwanz(1);
    CHECK(g_actors[1].x == 0 && g_actors[1].z == -299,
          "Ada aus dem SPIELER geschoben (aec4 a0 = 0x800aca54 @0x8011cc40): (%d,%d), erwartet (0,-299)",
          (int)g_actors[1].x, (int)g_actors[1].z);
    CHECK(pl->x == 0 && pl->z == 600, "der Spieler bleibt im NPC-Tick stehen (%d,%d)", (int)pl->x, (int)pl->z);

    /* (c) Gates: toter Spieler (Port-Abbild der Bit-Gates @0x8002af04-30, wie re15_enemy_selfpush_from_player). */
    g_actors[1].x = 0; g_actors[1].z = 0; pl->hp = -1;
    g_actors[2].x = 40000; g_actors[2].z = 40000;
    re15_kollision_r41_npc_schwanz(1);
    CHECK(g_actors[1].x == 0 && g_actors[1].z == 0, "toter Spieler schiebt nicht (%d,%d)", (int)g_actors[1].x,
          (int)g_actors[1].z);
    pl->hp = 100; pl->x = 50000; pl->z = 50000;

    /* (d) Leiche (Zustand 7) schiebt nicht; (e) Y-Band: |dy| >= hA+hB (1530+1530) -> kein Schub (@0x8002b1f0-b228). */
    g_actors[2].x = 500; g_actors[2].z = 0; g_actors[2].state = 7;
    re15_kollision_r41_npc_schwanz(1);
    CHECK(g_actors[1].x == 0, "Leiche (Zustand 7) schiebt nicht (x %d)", (int)g_actors[1].x);
    g_actors[2].state = 1; g_actors[2].y = -3060;
    re15_kollision_r41_npc_schwanz(1);
    CHECK(g_actors[1].x == 0, "Y-Band: dy 3060 = hA+hB -> kein Schub (strikt @0x8002b21c-228), x %d", (int)g_actors[1].x);
    g_actors[2].y = -3059;
    re15_kollision_r41_npc_schwanz(1);
    CHECK(g_actors[1].x < 0, "dy 3059 < 3060 -> Schub (x %d)", (int)g_actors[1].x);

    /* (f) Eigene Kaesten: John 0x49 (450) gegen Sherry 0x4b (300 @0x801218c8): Summe 750, dist 600, pen 150 ->
     *     Sherry x 600 + 600*150/601 = 749. */
    re15_actor_init();
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->hp = 100; pl->x = 50000; pl->z = 50000; re15_player_apply_hitbox(pl);
    npc_setzen(1, 0x49, 0, 0, 0);
    npc_setzen(2, 0x4b, 600, 0, 0);
    CHECK(g_actors[2].hit_radius_min == 300, "Sherry-Kasten +6 = 300 (@0x801218c8 `2c 01`)");
    re15_kollision_r41_npc_schwanz(2);
    CHECK(g_actors[2].x == 749, "Sherry aus John geschoben: x %d (erwartet 600 + 600*150/601 = 749)", (int)g_actors[2].x);
}

/* ------------------------------------------------------------------------------------------ raum ------ */
static int raum_laden(const char *rel, unsigned id, const int (*flags)[2], int nflags, uint8_t **buf_out)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/%s", rel);
    size_t sz = 0;
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); fails++; return -1; }
    if (re15_rdt_parse(buf, sz, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse %s\n", path); fails++; free(buf); return -1; }
    g_room_rdt_ok = 1;
    g_current_room_id = id;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_collision_set_band(0);
    for (int i = 0; i < nflags; i++) re15_game_flag_set((uint8_t)flags[i][0], (uint8_t)flags[i][1], 1);
    scd_register_current_rdt(&g_room_rdt);
    if (g_room_rdt.main_scd) scd_thread_start(0, g_room_rdt.main_scd);
    if (g_room_rdt.sub_scd[0]) scd_thread_start(1, g_room_rdt.sub_scd[0]);
    for (int i = 0; i < 60; i++) scd_vm_tick();
    *buf_out = buf;
    return 0;
}

static void teil_raum(const char *rel, unsigned id, const int (*flags)[2], int nflags, uint8_t ta, uint8_t tb,
                      int32_t px, int32_t pz, int16_t prot, int32_t summe, int32_t min_soll)
{
    printf("=== (1) %s: zwei Eskorten (Typ 0x%02x + 0x%02x) folgen demselben Punkt ===\n", rel, ta, tb);
    uint8_t *buf = NULL;
    if (raum_laden(rel, id, flags, nflags, &buf) != 0) return;
    int sa = -1, sb = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active) continue;
        if (g_actors[s].type == ta && sa < 0) sa = s;
        else if (g_actors[s].type == tb && sb < 0) sb = s;
    }
    CHECK(sa > 0 && sb > 0, "beide NPC gespawnt (Slots %d/%d)", sa, sb);
    if (sa < 0 || sb < 0) { free(buf); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = prot;
    re15_player_apply_hitbox(pl);
    re15_actor_t *a = &g_actors[sa], *b = &g_actors[sb];
    printf("  Spawn: 0x%02x (%d,%d) / 0x%02x (%d,%d), Spieler (%d,%d) Blick %d\n", a->type, (int)a->x, (int)a->z,
           b->type, (int)b->x, (int)b->z, (int)px, (int)pz, (int)prot);
    int32_t ax0 = a->x, az0 = a->z, bx0 = b->x, bz0 = b->z;
    int32_t dmin = 0x7fffffff, dmin_f = -1, kontakt = 0, dend = 0;
    int eskorte = 0;
    for (int f = 0; f < 900; f++) {
        re15_enemy_ai_run_all(0);
        if (a->state == 1 && b->state == 1) eskorte++;
        int32_t d = abstand(a, b);
        if (f >= 30) {
            if (d < dmin) { dmin = d; dmin_f = f; }
            if (d < summe + 100) kontakt++;
        }
        dend = d;
    }
    int32_t wa = abstand(&(re15_actor_t){ .x = ax0, .z = az0 }, a);
    int32_t wb = abstand(&(re15_actor_t){ .x = bx0, .z = bz0 }, b);
    printf("  900 Bilder: Eskorte-Bilder %d, Weg %d / %d, min. Abstand %d (Bild %d), Ende %d, Bilder < Summe+100: %d\n",
           eskorte, (int)wa, (int)wb, (int)dmin, (int)dmin_f, (int)dend, (int)kontakt);
    CHECK(eskorte >= 850, "beide laufen als ESKORTE (Zustand 1, Verhaltensbyte 0x00 -> INIT `sb 1,4(v1)` @0x8011ccc0)");
    CHECK(wa > 1000 && wb > 1000, "beide bewegen sich (Weg %d / %d) — sonst waere die Trennung trivial", (int)wa, (int)wb);
    CHECK(kontakt > 0, "sie kommen sich bis auf Radien-Summe + 100 nahe (%d Bilder) — derselbe Folgepunkt "
          "(`ori a0,zero,0x5dc` @0x8011ce78), die Messung ist nicht vakuant", (int)kontakt);
    CHECK(dmin >= min_soll, "und laufen NICHT ineinander: min. Abstand %d >= %d (Radien-Summe %d; vor Runde 41 "
          "gemessen 14 (3070) / 4 (3001))", (int)dmin, (int)min_soll, (int)summe);
    CHECK(dend >= summe - 2, "am Ende getrennt: %d >= %d", (int)dend, (int)(summe - 2));
    free(buf);
    g_room_rdt_ok = 0;
}

/* ---------------------------------------------------------------------------------------------- kiste -- */
static int prop_von_obj(int obj)
{
    for (int k = 0; k < RE15_SCD_MAX_PROPS && k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].active && (int)g_scd.props[k].obj_id == obj) return k;
    return -1;
}

static void teil_kiste(void)
{
    printf("=== (2) ROOM2050: Objekt-gegen-Objekt-Pass FUN_8002bd44 @0x8002c03c-c0a8 / FUN_8002cdf4 ===\n");
    uint8_t *buf = NULL;
    if (raum_laden("STAGE2/ROOM2050.RDT", 0x2050, NULL, 0, &buf) != 0) return;
    int p1 = prop_von_obj(1), p2 = prop_von_obj(2), p3 = prop_von_obj(3);
    CHECK(p1 >= 0 && p2 >= 0 && p3 >= 0, "Raetsel-Kisten obj 1/2/3 installiert (Pool-Plaetze %d/%d/%d)", p1, p2, p3);
    if (p1 < 0 || p2 < 0 || p3 < 0) { free(buf); return; }
    CHECK(g_scd.props[p3].x == 4248 && g_scd.props[p3].z == 1400 && g_scd.props[p3].box_hx == 900 &&
          g_scd.props[p3].box_cy == -900, "obj 3 = Record @0x01DF8 (4248,0,1400) Kasten c(0,-900,0) h(900,..)");
    re15_kollision_r41_objspiegel();

    /* Grenze inklusiv: obj 2 (z -64) gegen obj 3 (4248, z 1400): dZ -1464; dX = bx - 4248. */
    int32_t ny = 0;
    CHECK(re15_kollision_r41_8002cdf4(p3, p2, 2448, -64, 0, 0, &ny) == 1,
          "|dX| = 1800 = hx+hx -> UEBERLAPPUNG (sltu (2*sum) < (d+sum) @0x8002ce88 ist falsch fuer d+sum = 0)");
    CHECK(re15_kollision_r41_8002cdf4(p3, p2, 2447, -64, 0, 0, &ny) == 0, "|dX| = 1801 -> frei");
    CHECK(re15_kollision_r41_kisten_pass(&g_room_rdt, p2, 2448, -64) == 1, "Durchlauf: Ziel x 2448 GESPERRT");
    CHECK(re15_kollision_r41_kisten_pass(&g_room_rdt, p2, 2447, -64) == 0, "Durchlauf: Ziel x 2447 frei");
    CHECK(re15_kollision_r41_kisten_pass(&g_room_rdt, p2, -1256, -64) == 0, "Startlage obj 2 trifft nichts");

    /* Gate Bit 0x40 im Wort 0 (`or`/`andi 0x40`/`bne` @0x8002ce0c-24). */
    uint16_t f3 = g_scd.props[p3].flags;
    g_scd.props[p3].flags = (uint16_t)(f3 | 0x40);
    CHECK(re15_kollision_r41_kisten_pass(&g_room_rdt, p2, 2448, -64) == 0, "Bit 0x40 am anderen Objekt -> kein Test");
    g_scd.props[p3].flags = f3;

    /* Y: obj 3 auf Band 3 (y -5400): dY = 5400 > 1800 -> keine Ueberlappung (@0x8002ced0-e0). */
    int32_t y3 = g_scd.props[p3].y;
    g_scd.props[p3].y = -5400;
    CHECK(re15_kollision_r41_8002cdf4(p3, p2, 2448, -64, -5400, 0, &ny) == 0, "anderes Band (dY 5400) -> frei");
    /* Vertikaler Eintritt: jetzt Y-Ueberlappung (obj 3 y 0), im Spiegel +0x42 nicht (y -5400) -> b.y = a-Mitte + sy + 1
     * - b.cy = -900 + 1801 + 900 = 1801 (`bgez t6` @0x8002cf18 mit t6 = 0 -> @0x8002cf30-3c). */
    g_scd.props[p3].y = y3;
    ny = 12345;
    CHECK(re15_kollision_r41_8002cdf4(p3, p2, 2448, -64, -5400, 0, &ny) == 1 && ny == 1801,
          "vertikaler Eintritt (Vor-y aus +0x42 ausserhalb): Rueckgabe 1, neue y %d (erwartet 1801)", (int)ny);

    /* Raetsel-Endlage (gemessen an der exe, H_belege/raetsel_leon): obj 1 (-6636,-3718), obj 2 (-6614,-64), obj 3
     * (-6644,-1887) — jede Kiste in ihrer Kerbe (Aot slot 3/4/5 @0x01CAA/@0x01CBE/@0x01CD2, x -7600..-5550) und
     * keine sperrt die andere (|dZ| 1831 / 1823 > 1800). */
    g_scd.props[p1].x = -6636; g_scd.props[p1].z = -3718;
    g_scd.props[p2].x = -6614; g_scd.props[p2].z = -64;
    g_scd.props[p3].x = -6644; g_scd.props[p3].z = -1887;
    re15_kollision_r41_objspiegel();
    CHECK(re15_kollision_r41_kisten_pass(&g_room_rdt, p1, -6636, -3718) == 0 &&
          re15_kollision_r41_kisten_pass(&g_room_rdt, p2, -6614, -64) == 0 &&
          re15_kollision_r41_kisten_pass(&g_room_rdt, p3, -6644, -1887) == 0,
          "Raetsel-Endlage: drei Kisten in den Kerben ohne gegenseitige Sperre (Raetsel bleibt loesbar)");
    CHECK(re15_kollision_r41_kisten_pass(&g_room_rdt, p1, -6636, -3687) == 1,
          "aber obj 1 bei z -3687 (|dZ| 1800 zu obj 3) waere gesperrt — die Kerben lassen 1900/1850 Raster");
    free(buf);
    g_room_rdt_ok = 0;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "npc";
    if (!strcmp(teil, "npc")) teil_npc();
    else if (!strcmp(teil, "raum3070")) {
        static const int fl[3][2] = { {4, 215}, {4, 236}, {3, 180} };
        teil_raum("STAGE3/ROOM3070.RDT", 0x3070, fl, 3, 0x42, 0x40, -9701, -17922, 1440, 900, 880);
    } else if (!strcmp(teil, "raum3001")) {
        static const int fl[2][2] = { {4, 198}, {4, 199} };
        teil_raum("STAGE3/ROOM3001.RDT", 0x3001, fl, 2, 0x49, 0x4b, -23000, -16000, 0, 750, 730);
    } else if (!strcmp(teil, "kiste")) teil_kiste();
    else { printf("unbekannter Teil '%s'\n", teil); return 2; }
    if (fails) { printf("\nR41 KOLLISION [%s]: FAIL (%d)\n", teil, fails); return 1; }
    printf("\nR41 KOLLISION [%s]: OK\n", teil);
    return 0;
}
