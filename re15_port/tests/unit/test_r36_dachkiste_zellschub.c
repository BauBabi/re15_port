/*
 * Runde 36 Spur R "dachkiste", Nachbesserung 3 (Abnahme 2: P1, P2, P4) — Pins der MECHANIK.
 * Dossier: analysis/befunde_runde36/R_dachkiste.md, Abschnitt "Nachbesserung 3".
 *
 *   zellschub   P1: Ruhe-Zweig LAB_8002c0b8 des Objekt-Passes (FUN_8003b558(obj,2) @0x8002c0d0 mit Ausschiebung
 *               FUN_8003da78 @0x8003b79c): ROOM2060 Tritt obj 2 (Record @0x00EA6 (-3125,0,2075), Box c(50,-900,0)
 *               h(675,900,600)) beruehrt r0[9] (x -2400..3150 z 2150..5350) an der Kante -> nach dem ERSTEN Bild
 *               x -3143 (Eindringtiefe 0 + 18 @0x8003dab8, kleinerer Betrag @0x8003dc60-94), z 2075; stabil.
 *               Original: room2060_orig.sav Pool 2 (-3143,0,2075) (R_abnahme_2.md A7), orig_2060b_abn2 START.
 *               Dazu die vier Lagecode-Zweige von FUN_8003da78 (@0x8003db14-dc28) synthetisch.
 *   quadrat     P4: Nicht-Quadrat-Zweig @0x8003b684-734 (Objektmitte innerhalb der x-Spanne, ausserhalb der
 *               z-Spanne -> r = hz statt hx): eine Lage in ROOM2060, die mit r = hx (675) gesperrt und mit
 *               r = hz (600) frei ist — re15_zellschub_8003b558 = 0, re15_collision_box_blocked(r = hx) = 1.
 *   ytest       P2: FUN_8002cabc Y-Test @0x8002cb84-cbc8: Leon auf Band 3 (y -5400) in ROOM2050 neben der
 *               Band-0-Kiste obj 1 (2052,0,-5552) wird NICHT ausgeschoben (dy 6030 > hySum 2430), auf Band 0
 *               (y 0) an derselben Stelle wohl (x -> 2952 + 450 = 3402); dasselbe fuer die Aktor-Variante.
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
#include "re15_climb.h"
#include "re15_collision.h"
#include "re15_zellschub_r36.h"

extern scd_vm_t g_scd;

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("ok: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    if (n <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (!b) { fclose(f); return NULL; }
    size_t r = fread(b, 1, (size_t)n, f);
    fclose(f);
    if (r != (size_t)n) { free(b); return NULL; }
    *out_size = (size_t)n;
    return b;
}

static re15_rdt_t    s_rdt;
static re15_actor_t *s_pl;

static int raum(const char *stage, const char *room, uint16_t room_id, int32_t px, int32_t pz, int band)
{
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/%s/%s.RDT", RE15_ASSET_PSX_DIR, stage, room);
    uint8_t *raw = read_file(path, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", path); return 77; }
    if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("SKIP: RDT-Parse %s\n", room); return 77; }
    re15_actor_init();
    scd_vm_init();
    re15_climb_reset();
    g_current_room_id = room_id;
    g_room_change.pending = 0;
    s_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_pl->active = 1; s_pl->type = 0; s_pl->hp = 100;
    s_pl->x = px; s_pl->y = -1800 * band; s_pl->z = pz; s_pl->rot_y = 0;
    scd_room_reenter(&s_rdt, s_pl->x, s_pl->z, 0);
    re15_collision_set_band(band);
    re15_zellschub_install(room_id);
    return 0;
}

static int prop_mit_id(int obj_id)
{
    for (int p = 0; p < (int)g_scd.prop_count && p < RE15_SCD_MAX_PROPS; p++)
        if (g_scd.props[p].active && g_scd.props[p].obj_id == obj_id) return p;
    return -1;
}

/* ---- P1: Ruhe-Zweig in ROOM2060 + FUN_8003da78-Zweige ----------------------------------------------- */
static int test_zellschub(void)
{
    int rc = raum("STAGE2", "ROOM2060", 0x2060, 7600, 3950, 0);   /* 2030-Ankunft slot 0 Band 0 */
    if (rc) return rc;
    const int p = prop_mit_id(2);
    PRUEF(p >= 0, "ROOM2060: Tritt obj 2 installiert (Pool-Platz %d)", p);
    if (p < 0) return 1;
    PRUEF(g_scd.props[p].x == -3125 && g_scd.props[p].z == 2075 && g_scd.props[p].band == 0,
          "ROOM2060 obj 2 vor dem ersten Bild auf dem Record @0x00EA6 (-3125,2075) Band 0: (%d,%d) b%d",
          g_scd.props[p].x, g_scd.props[p].z, (int)g_scd.props[p].band);
    PRUEF(g_scd.props[p].box_cx == 50 && g_scd.props[p].box_hx == 675 && g_scd.props[p].box_hz == 600,
          "ROOM2060 obj 2 Box c(50,%d,%d) h(675,%d,600): c(%d,%d,%d) h(%d,%d,%d)",
          (int)g_scd.props[p].box_cy, (int)g_scd.props[p].box_cz, (int)g_scd.props[p].box_hy,
          (int)g_scd.props[p].box_cx, (int)g_scd.props[p].box_cy, (int)g_scd.props[p].box_cz,
          (int)g_scd.props[p].box_hx, (int)g_scd.props[p].box_hy, (int)g_scd.props[p].box_hz);

    /* Bild 1: Objekt-Pass ohne Spieler-Schub (Pad 0) -> Zaehler 0 != 9 @0x8002bf8c -> LAB_8002c0b8 */
    re15_prop_push_tick(&s_rdt, 0);
    PRUEF(g_scd.props[p].x == -3143 && g_scd.props[p].z == 2075,
          "ROOM2060 obj 2 nach Bild 1: (-3143,2075) = Record - 18 nach Westen (FUN_8003da78 @0x8003dab8/@0x8003dc60-88): (%d,%d)",
          g_scd.props[p].x, g_scd.props[p].z);
    int32_t sx = 0, szz = 0;
    PRUEF(re15_zellschub_spiegel_lesen(p, &sx, &szz) == 0 && sx == -3143 && szz == 2075,
          "ROOM2060 obj 2 Spiegel +0x40/+0x44 nach Bild 1 = (-3143,2075) (@0x8002c0f0-c10c): (%d,%d)", sx, szz);
    /* Bild 2..4: stabil (x-Test -3093 - (-2400-675) = -18 -> (u32) gross -> kein Treffer mehr) */
    re15_prop_push_tick(&s_rdt, 0);
    re15_prop_push_tick(&s_rdt, 0);
    re15_zellschub_nach_pass(&s_rdt);   /* Bild ohne Pass: Haken game_step_common.c */
    PRUEF(g_scd.props[p].x == -3143 && g_scd.props[p].z == 2075,
          "ROOM2060 obj 2 nach Bild 4 unveraendert (-3143,2075): (%d,%d)", g_scd.props[p].x, g_scd.props[p].z);

    /* Alle anderen aktiven Objekte des Raums: Lage nach den vier Bildern gegen die Lage nach Bild 0 — der
     * Ruhe-Zweig darf nur das beruehrende Objekt bewegen. (Zensus ueber alle Raeume: diag.) */
    int bewegt = 0;
    for (int q = 0; q < (int)g_scd.prop_count && q < RE15_SCD_MAX_PROPS; q++) {
        if (!g_scd.props[q].active || q == p) continue;
        int32_t mx, mz;
        if (re15_zellschub_spiegel_lesen(q, &mx, &mz) == 0 && (mx != g_scd.props[q].x || mz != g_scd.props[q].z)) bewegt++;
    }
    PRUEF(bewegt == 0, "ROOM2060: kein weiteres Objekt vom Ruhe-Zweig bewegt (%d)", bewegt);

    /* FUN_8003da78 synthetisch: Zelle r0[9] x -2400 w 5550 z 2150 d 3200, Box c(50,0) r = 675. */
    int32_t x, z, rv;
    /* (a) unbewegt (Spiegel = Lage): Code 0 @0x8003db58 -> dc2c -> gleich -> kleinerer Betrag @0x8003dc60-94:
     *     t0 = (3150+675)-(-3075)+18 = 6918, a0 = (-2400-675)-(-3075)-18 = -18 -> t0 = -18;
     *     a3 = (5350+675)-2075+18 = 3968, az = (2150-675)-2075-18 = -618 -> a3 = -618; |t0| < |a3| -> x += -18 */
    x = -3125; z = 2075;
    rv = re15_zellschub_8003da78(-2400, 5550, 2150, 3200, 50, 0, 675, -3125, 2075, &x, &z);
    PRUEF(rv == 1 && x == -3143 && z == 2075, "FUN_8003da78 Code 0 unbewegt -> x -18 (rv %d, (%d,%d))", (int)rv, x, z);
    /* (b) Code 1 (@0x8003db74-98): dx = 0 -> sign(t0 ^ 0) = 1 (t0 < 0); dz = -100 gleichsinnig zu a3 < 0 -> Bit1 = 0;
     *     |t0| = 18, 18+400 < 801 -> x += t0 (Lage (-3125,1600), Spiegel (-3125,1700); a3 = 1475-1600-18 = -143) */
    x = -3125; z = 1600;
    rv = re15_zellschub_8003da78(-2400, 5550, 2150, 3200, 50, 0, 675, -3125, 1700, &x, &z);
    PRUEF(rv == 1 && x == -3143 && z == 1600, "FUN_8003da78 Code 1 -> x += t0 (rv %d, (%d,%d))", (int)rv, x, z);
    /* (c) Code 2 (@0x8003dba0-c4): dx = -25 gleichsinnig zu t0 -> Bit0 = 0; dz = +100 gegen a3 = -143 -> Bit1 = 1;
     *     |a3| = 143, 143+400 < 801 -> z += a3 */
    x = -3125; z = 1600;
    rv = re15_zellschub_8003da78(-2400, 5550, 2150, 3200, 50, 0, 675, -3100, 1500, &x, &z);
    PRUEF(rv == 1 && x == -3125 && z == 1457, "FUN_8003da78 Code 2 -> z += a3 (rv %d, (%d,%d))", (int)rv, x, z);
    /* (d) Code 3 (@0x8003dbc8-dc28): dx = 0 -> Bit0 = 1; dz = +100 gegen a3 -> Bit1 = 1; kleinerer Betrag 18 < 143 -> x */
    x = -3125; z = 1600;
    rv = re15_zellschub_8003da78(-2400, 5550, 2150, 3200, 50, 0, 675, -3125, 1500, &x, &z);
    PRUEF(rv == 1 && x == -3143 && z == 1600, "FUN_8003da78 Code 3 -> kleinerer Betrag x (rv %d, (%d,%d))", (int)rv, x, z);
    /* (e) Code 0 bewegt (@0x8003dc2c-58 -> @0x8003dc98-cb4): dx = -25 (Bit0 0), dz = -100 (Bit1 0) -> Lage = Spiegel, rv 0 */
    x = -3125; z = 1600;
    rv = re15_zellschub_8003da78(-2400, 5550, 2150, 3200, 50, 0, 675, -3100, 1700, &x, &z);
    PRUEF(rv == 0 && x == -3100 && z == 1700, "FUN_8003da78 Code 0 bewegt -> Rueckstellung auf den Spiegel (rv %d, (%d,%d))", (int)rv, x, z);
    /* (f) Code 2 mit |a3| > 400 (@0x8003dbb8 -> dbc8): Lage (-3125,2075), Spiegel (-3100,1500): dx -25 (Bit0 0), dz 575 gegen
     *     a3 = -618 (Bit1 1) -> 618+400 >= 801 -> kleinerer Betrag |t0| 18 -> x */
    x = -3125; z = 2075;
    rv = re15_zellschub_8003da78(-2400, 5550, 2150, 3200, 50, 0, 675, -3100, 1500, &x, &z);
    PRUEF(rv == 1 && x == -3143 && z == 2075, "FUN_8003da78 Code 2, |a3| > 400 -> kleinerer Betrag x (rv %d, (%d,%d))", (int)rv, x, z);
    return 0;
}

/* ---- P4: Nicht-Quadrat-Zweig an einer echten Zelle von ROOM2060 --------------------------------------- */
static int test_quadrat(void)
{
    int rc = raum("STAGE2", "ROOM2060", 0x2060, 7600, 3950, 0);
    if (rc) return rc;
    const int p = prop_mit_id(2);
    if (p < 0) { PRUEF(0, "ROOM2060: Tritt obj 2 fehlt"); return 1; }
    const int32_t hx = (int32_t)(uint16_t)g_scd.props[p].box_hx, hz = (int32_t)(uint16_t)g_scd.props[p].box_hz;
    const int32_t cx = g_scd.props[p].box_cx, cz = g_scd.props[p].box_cz;
    PRUEF(hx == 675 && hz == 600, "ROOM2060 obj 2 ist nicht quadratisch: hx %d hz %d", hx, hz);
    /* Suche eine Band-0-Zelle der Klasse 2 (u0 & 2), in deren x-Spanne die Objektmitte liegt; Lage suedlich davon
     * bei czw = z0 - 640: mit r = hx (675) Treffer (35 < d + 1350), mit r = hz (600) frei ((u32)(-40) gross). */
    int gefunden = 0;
    for (int i = 0; i < s_rdt.sca_count && !gefunden; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        if ((e->floor >> 4) != 0 || !(e->u0 & 2)) continue;
        if ((int32_t)e->width < 2 * hx + 100) continue;
        const int32_t cxw = (int32_t)e->x + (int32_t)e->width / 2;
        const int32_t czw = (int32_t)e->z - 640;
        int32_t ox = cxw - cx, oz = czw - cz;
        int32_t tx = ox, tz = oz;
        const int neu = re15_zellschub_8003b558(&s_rdt, p, &tx, &tz, 2u);
        const int alt = re15_collision_box_blocked(&s_rdt, cxw, czw, 0, hx, 2u);
        if (alt == 1 && neu == 0) {
            gefunden = 1;
            printf("ok: Zelle [%d] x %d..%d z %d..%d u0 0x%02x: Objektmitte (%d,%d) — r = hx (675) GESPERRT, r = hz (600) FREI "
                   "(@0x8003b6f8-734 Lagecode x 2 / z 3 -> lhu t0,10(a3))\n", i, (int)e->x, (int)e->x + (int)e->width,
                   (int)e->z, (int)e->z + (int)e->density, (unsigned)e->u0, cxw, czw);
            /* Gegenprobe: 100 Einheiten weiter noerdlich (czw = z0 - 540) sperren beide (Eindringung 60 > 0 bei r = hz). */
            int32_t ux = ox, uz = oz + 100;
            const int neu2 = re15_zellschub_8003b558(&s_rdt, p, &ux, &uz, 2u);
            PRUEF(neu2 == 1, "dieselbe Zelle bei czw = z0 - 540: r = hz sperrt (%d)", neu2);
        }
    }
    PRUEF(gefunden, "ROOM2060: eine Lage gefunden, an der NUR der Nicht-Quadrat-Zweig (r = hz) den Schub freigibt");
    return 0;
}

/* ---- P2: Y-Test der Ausschiebung in ROOM2050 ------------------------------------------------------------ */
static int test_ytest(void)
{
    int rc = raum("STAGE2", "ROOM2050", 0x2050, 9100, 4100, 3);   /* 2000-Ankunft slot 4 Band 3 */
    if (rc) return rc;
    const int k = prop_mit_id(1);
    PRUEF(k >= 0 && g_scd.props[k].x == 2052 && g_scd.props[k].y == 0 && g_scd.props[k].z == -5552 && g_scd.props[k].band == 0,
          "ROOM2050 obj 1 (Band-0-Kiste sub00 @0x01DB4) bei (2052,0,-5552) Band 0: Platz %d", k);
    if (k < 0) return 1;
    PRUEF(g_scd.props[k].box_cy == -900 && g_scd.props[k].box_hy == 900 && g_scd.props[k].box_hx == 900,
          "ROOM2050 obj 1 Box c(0,-900,0) h(900,900,900): cy %d hy %d hx %d",
          (int)g_scd.props[k].box_cy, (int)g_scd.props[k].box_hy, (int)g_scd.props[k].box_hx);

    /* Leon auf Band 3 (y -5400) in der x/z-Ueberdeckung der Kiste: objCY -900, actorCY -6930, dy 6030 > 2430 */
    s_pl->x = 3300; s_pl->y = -5400; s_pl->z = -6300;
    re15_collision_set_band(3);
    int32_t x = s_pl->x, z = s_pl->z;
    re15_collision_objects(&x, &z);
    PRUEF(x == 3300 && z == -6300, "Band 3 (y -5400): keine Ausschiebung aus der Band-0-Kiste (@0x8002cbb4-c8, dy 6030 > 2430): (%d,%d)", x, z);
    /* Dieselbe Stelle auf Band 0 (y 0): dy = -900 - (-1530) = 630 <= 2430 -> Ausschiebung nach Osten auf 2952 + 450 */
    s_pl->y = 0; re15_collision_set_band(0);
    x = s_pl->x; z = s_pl->z;
    re15_collision_objects(&x, &z);
    PRUEF(x == 3402 && z == -6300, "Band 0 (y 0): Ausschiebung auf x 3402 = 2952 + 450 (@0x8002cd50-7c): (%d,%d)", x, z);
    /* Aktor-Variante (NPC-Box @0x80121658 = Spieler-Box) */
    x = 3300; z = -6300;
    re15_collision_objects_actor(&x, &z, -5400);
    PRUEF(x == 3300 && z == -6300, "Aktor y -5400: keine Ausschiebung: (%d,%d)", x, z);
    x = 3300; z = -6300;
    re15_collision_objects_actor(&x, &z, 0);
    PRUEF(x == 3402 && z == -6300, "Aktor y 0: Ausschiebung auf 3402: (%d,%d)", x, z);
    /* Schiebe-Modus (a2 = 1, @0x8002cb7c != 2 -> Y-Test): Spieler Band 3 schiebt die Band-0-Kiste nicht */
    int32_t ox = g_scd.props[k].x, oz = g_scd.props[k].z;
    s_pl->y = -5400;
    PRUEF(re15_collision_push_prop(k, 3300, -6300, &ox, &oz) == 0 && ox == 2052,
          "Schiebe-Modus Band 3 gegen Band-0-Kiste: kein Schub (%d, x %d)", (int)ox != 2052, ox);
    s_pl->y = 0;
    PRUEF(re15_collision_push_prop(k, 3300, -6300, &ox, &oz) == 1 && ox == 3300 - 1350,
          "Schiebe-Modus Band 0: Kiste nach Westen auf 3300 - 1350 = 1950 (@0x8002cd20-44): x %d", ox);
    return 0;
}

/* ---- Zensus (diag): alle Raeume laden, ein Bild Ruhe-Zweig, bewegte Objekte ausgeben --------------------- */
static void zensus_alle(void)
{
    static const char *stages[6] = { "STAGE1", "STAGE2", "STAGE3", "STAGE4", "STAGE5", "STAGE6" };
    int n_raeume = 0, n_obj = 0, n_bewegt = 0;
    for (int s = 0; s < 6; s++) {
        for (int rv = 0; rv < 512; rv++) {
            const int r = rv >> 1, v = rv & 1;            /* ROOM<stage><xx><variante>, z. B. ROOM1090 / ROOM1091 */
            char room[16]; snprintf(room, sizeof room, "ROOM%d%02X%d", s + 1, r, v);
            char path[600]; snprintf(path, sizeof path, "%s/%s/%s.RDT", RE15_ASSET_PSX_DIR, stages[s], room);
            FILE *f = fopen(path, "rb"); if (!f) continue; fclose(f);
            if (raum(stages[s], room, (uint16_t)(((s + 1) << 12) | (r << 4) | v), 0, 0, 0)) continue;
            n_raeume++;
            int32_t vor_x[RE15_SCD_MAX_PROPS], vor_z[RE15_SCD_MAX_PROPS];
            for (int p = 0; p < RE15_SCD_MAX_PROPS; p++) { vor_x[p] = g_scd.props[p].x; vor_z[p] = g_scd.props[p].z; }
            re15_prop_push_tick(&s_rdt, 0);
            for (int p = 0; p < (int)g_scd.prop_count && p < RE15_SCD_MAX_PROPS; p++) {
                if (!g_scd.props[p].active) continue;
                n_obj++;
                if (vor_x[p] != g_scd.props[p].x || vor_z[p] != g_scd.props[p].z) {
                    n_bewegt++;
                    printf("BEWEGT %s p=%d obj=%d typ=%d band=%d flags=0x%04x box c(%d,%d,%d) h(%d,%d,%d): (%d,%d) -> (%d,%d)\n",
                           room, p, (int)g_scd.props[p].obj_id, (int)g_scd.props[p].obj_type, (int)g_scd.props[p].band,
                           (unsigned)g_scd.props[p].flags, (int)g_scd.props[p].box_cx, (int)g_scd.props[p].box_cy,
                           (int)g_scd.props[p].box_cz, (int)g_scd.props[p].box_hx, (int)g_scd.props[p].box_hy,
                           (int)g_scd.props[p].box_hz, vor_x[p], vor_z[p], g_scd.props[p].x, g_scd.props[p].z);
                }
            }
        }
    }
    printf("ZENSUS: %d Raeume, %d aktive Objekte, %d vom Ruhe-Zweig im ersten Bild bewegt\n", n_raeume, n_obj, n_bewegt);
}

int main(int argc, char **argv)
{
    const char *was = argc > 1 ? argv[1] : "pin";
    if (!strcmp(was, "diag")) { zensus_alle(); return 0; }
    int rc;
    if ((rc = test_zellschub()) == 77) return 77;
    if ((rc = test_quadrat()) == 77) return 77;
    if ((rc = test_ytest()) == 77) return 77;
    printf(g_fail ? "FAIL: %d\n" : "OK (%d Fehler)\n", g_fail);
    return g_fail ? 1 : 0;
}
