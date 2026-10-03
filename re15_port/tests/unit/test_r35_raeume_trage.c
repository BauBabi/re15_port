/**
 * @file test_r35_raeume_trage.c
 * @brief Runde 35 Spur H, Punkt 3 — ROOM1200: der Zombie von der Bahre kommt auf die Spieler-Ebene.
 *
 * Nutzer: "Bei ROOM 1200 nach dem aufnehmen des Minidisc players steht der Zombie von der Trage auf und
 * laeuft durch die Luft, statt wie im Original danach auf die Spieler Ebene runter zu kommen."
 *
 * Original-Mechanismus = Engine-Schwerkraft FUN_8001bd60(-10, 0x14), gerufen von der Zombie-Wurzel
 * @0x80100514 (Herleitung include/re15_trage1200.h). Gemessen wird:
 *   A  die Funktion an den ECHTEN ROOM1200-Zellen: Absturzkante (Band-1-Zelle 13, Wort 0x1302) ->
 *      ein Band tiefer, +0x1ba 0, Fallfolge y = -1800 -10, +10, +30, ... bis 0 (15 Bilder), Fallwort
 *      danach 0x000F; NICHT auf der Bahre (keine Band-1-Zelle), NICHT am Zellenrand innerhalb des
 *      Radius (Rechteck um 400 verkleinert), NICHT wenn y != -(Band*1800), n = 1 -> zwei Baender.
 *   B  der echte Weg: ROOM1200 laden (main00 spawnt), Liege-Zombie (Sce_em_set id 1, grid 0x87,
 *      Band 1, y -1800) wie sub03 @0x00A26 wecken (Member_set 0x0C = 0x89), Spieler auf Band 0 vor
 *      der Kante; KI + Animation Bild fuer Bild, BEIDE Geschmaecker: der Zombie verlaesst Band 1 und
 *      landet auf y = 0 / Band 0; die Fallfolge ist genau -10 + 20*t.
 *   C  Sce_em_set seedet +0x1ba = -(pc[4]*1800) (@0x80042210) fuer jeden Gegner des Raums.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_climb.h"
#include "re15_trage1200.h"
#include "re15_enemy.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

void scd_register_current_rdt(const re15_rdt_t *rdt);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* -------------------------------------------------------------------------- A: die Funktion */
static void teil_a(void)
{
    re15_climb_reset();                        /* kein Standobjekt: DAT_800aca3c & 0x4000 = 0 */
    re15_actor_t z; memset(&z, 0, sizeof z);
    z.active = 1; z.type = 0x10; z.floor = 1; z.y = -1800; z.dog_floor_y = -1800;

    /* Bahre/Liegeplatz (Spawn id 1 @RDT 0x0086A): keine Band-1-Zelle -> nichts */
    z.x = -24249; z.z = -18579;
    for (int i = 0; i < 5; i++) re15_schwerkraft_8001bd60(&z, -10, 0x14, 400);
    CHECK(z.y == -1800 && z.floor == 1 && z.fall_1c0 == 0, "A: auf der Bahre darf nichts fallen (y=%d b%d)",
          z.y, z.floor);

    /* Zellenrand: Zelle 13 z -17351..-16251; 151 innen < Radius 400 -> nichts (`subu a1,zero,a1`) */
    z.x = -24500; z.z = -17200;
    re15_schwerkraft_8001bd60(&z, -10, 0x14, 400);
    CHECK(z.y == -1800 && z.floor == 1, "A: 151 Einheiten in der Kante (< Radius 400) faellt nicht");

    /* y nicht auf dem Band (-1700) -> keine Erkennung (@0x8001bdec) */
    z.z = -16800; z.y = -1700;
    re15_schwerkraft_8001bd60(&z, -10, 0x14, 400);
    CHECK(z.floor == 1 && z.fall_1c0 == 0, "A: y != -(Band*1800) darf nicht abstuerzen");

    /* echte Kante: Zelle 13 Mitte (z -16800), y = -1800 */
    z.y = -1800;
    int32_t soll = -1800;
    int bilder = 0;
    for (int t = 0; t < 40; t++) {
        re15_schwerkraft_8001bd60(&z, -10, 0x14, 400);
        bilder++;
        int32_t erw = soll + (-10 + 20 * t);
        if (erw > 0) erw = 0;
        CHECK(z.y == erw, "A: Bild %d y=%d, erwartet %d (-10 + 20*t @0x8001be78-a4)", t, z.y, erw);
        soll = erw;
        if (t == 0) {
            CHECK(z.floor == 0 && z.dog_floor_y == 0, "A: ein Band tiefer (+0x82 1->0, +0x1ba -1800->0)");
            CHECK(z.fall_1c0 == 0x8001, "A: Fallwort nach Bild 0 = 0x8001, ist %04x", z.fall_1c0);
        }
        if (!(z.fall_1c0 & 0x8000)) break;
    }
    CHECK(z.y == 0 && z.fall_1c0 == 0x000f && bilder == 15,
          "A: Landung y=%d Fallwort %04x nach %d Bildern (erwartet 0 / 000F / 15)", z.y, z.fall_1c0, bilder);
    /* gelandet auf Band 0: keine Band-0-Absturzzelle in ROOM1200 -> bleibt */
    for (int i = 0; i < 5; i++) re15_schwerkraft_8001bd60(&z, -10, 0x14, 400);
    CHECK(z.y == 0 && z.floor == 0, "A: nach der Landung bleibt der Zombie auf Band 0");
    printf("  [A] Kante Zelle 13: 15 Bilder Sturz -1800 -> 0, Bahre/Rand/Off-Band: kein Sturz\n");
}

/* ------------------------------------------------------------------- B: der echte Weg (KI) */
static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

static int raum_hoch(const re15_rdt_t *r)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x1200u);
    re15_climb_reset();
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = 0x1200;
    scd_register_current_rdt(r);
    if (r->main_scd) scd_thread_start(0, r->main_scd);
    for (int i = 0; i < 30; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 200; pl->x = -25900; pl->y = 0; pl->z = -16500;
    pl->floor = 0; pl->state = 1; pl->motion = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (e->active && e->type == 0x10 && e->x == -24249 && e->z == -18579) return s;
    }
    return -1;
}

/* Zombie-Bank wie im Spiel: RE1.5 EM10 aus PSX/EMD/CDEMD0.EMS (Muster probe_10d0_knockse.c),
 * RE2 EM010 aus RE2/CDEMD0.EMS (Muster test_re2_room1190_ab.c) — Clip-Laengen + Wurzelschritte. */
static uint8_t s_b10[0x80000];
static int bank_laden(int re2)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x10);
    if (!eb) eb = re15_enemy_alloc(0x10);
    if (!eb) return 0;
    long sz = 0;
    if (re2) {
        static uint8_t *ems = NULL; static long ems_sz = 0;
        if (!ems) ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_sz);
        if (!ems) return 0;
        if (re2_ems_load_bank(ems, (size_t)ems_sz, 0x10, eb, NULL) != 0) return 0;
        eb->buf = NULL; eb->ok = 1;
        return 1;
    }
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &sz);
    if (!ems) return 0;
    int ok = 0;
    int idx = re15_ems_index_for_type(0x10);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, (size_t)sz, idx, &off, &len) == 0 && len <= sizeof s_b10) {
        memcpy(s_b10, ems + off, len);
        re15_tim_t tim = (re15_tim_t){0};
        if (re15_emd_parse_container(s_b10, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
            eb->ok = 1; eb->buf = NULL;
            eb->loco_ok = (re15_emd_parse_loco_bank(s_b10, len, &eb->skel_loco, &eb->anim_loco) == 0);
            eb->own_ok  = (re15_emd_parse_own_bank(s_b10, len, &eb->skel_own, &eb->anim_own) == 0);
            ok = 1;
        }
    }
    free(ems);
    return ok;
}

static void teil_b(const re15_rdt_t *r, int flavor, const char *name)
{
    re15_ai_flavor_set(flavor);
    CHECK(bank_laden(flavor == RE15_AI_FLAVOR_RE2), "[%s] Zombie-Bank nicht ladbar", name);
    const int s = raum_hoch(r);
    CHECK(s > 0, "[%s] Liege-Zombie (id 1 @RDT 0x0086A) nicht gespawnt", name);
    if (s <= 0) return;
    re15_actor_t *e = &g_actors[s];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    CHECK(e->y == -1800 && e->floor == 1 && e->dog_floor_y == -1800,
          "[%s] Spawn y=%d b%d +0x1ba=%d (erwartet -1800/1/-1800, Seed @0x80042210)",
          name, e->y, e->floor, (int)e->dog_floor_y);
    /* C: alle Gegner des Raums geseedet */
    for (int k = 1; k < RE15_ACTOR_MAX; k++) {
        const re15_actor_t *g = &g_actors[k];
        if (!g->active || g->type != 0x10) continue;
        CHECK((int32_t)g->dog_floor_y == -(int32_t)g->floor * 1800,
              "[%s] C: slot %d +0x1ba=%d != -(b%d*1800)", name, k, (int)g->dog_floor_y, g->floor);
    }
    frame();
    e->grid_id = 0x89;                          /* sub03 @0x00A26 `34 0c 89 00` (id 1) */
    int sturz_start = -1, landung = -1, folge_ok = 1, luft_band1 = 0;
    int32_t y_vor = e->y;
    for (int f = 0; f < 1500; f++) {
        pl->hp = 200;                           /* Messung des Gegners, nicht des Kampfes */
        /* Spieler laeuft (Bewegungsclip 100 = rennen) auf Band 0 hinter der Kante hin und her, wie im
         * Spiel nach dem Aufnehmen — der RE2-Zombie verliert sonst das Interesse (Leerlauf Sub 0);
         * steht der Spieler DIREKT unter der Kante, beisst der RE2-Zombie von oben (Sub 14, 2D-Abstand)
         * statt weiterzugehen — darum 4500 hinter der Kante (Zelle 13 z -17351..-16251). */
        pl->motion = 100; pl->x = -24300 + ((f / 60) & 1 ? 600 : -600); pl->z = -12500;
        frame();
        if (e->floor == 1 && e->y == -1800) luft_band1++;
        if (sturz_start < 0 && (e->fall_1c0 & 0x8000)) sturz_start = f;
        if (sturz_start >= 0 && landung < 0) {
            const int t = (int)(e->fall_1c0 & 0x1fff) - 1;    /* t dieses Bildes */
            int32_t erw = y_vor + (-10 + 20 * t);
            if (erw > (int32_t)e->dog_floor_y) erw = e->dog_floor_y;
            if (t >= 0 && e->y != erw) folge_ok = 0;
            if (!(e->fall_1c0 & 0x8000)) landung = f;
        }
        y_vor = e->y;
        if (getenv("R35_TRAGE_DBG") && (f % 50) == 0)
            printf("    f%d st=%d/%d/%d g=%02x mo=%d af=%d @(%d,%d,%d) b%d\n", f, e->state, e->sub_state_1,
                   e->sub_state_2, e->grid_id, e->motion, e->anim_frame, e->x, e->y, e->z, e->floor);
        if (landung >= 0 && f > landung + 30) break;
    }
    printf("  [B %s] slot %d: Sturz ab Bild %d, Landung Bild %d, danach y=%d b%d +0x1ba=%d\n",
           name, s, sturz_start, landung, e->y, e->floor, (int)e->dog_floor_y);
    CHECK(sturz_start >= 0, "[%s] der Zombie ist nie von der Bahre gefallen (FUN_8001bd60)", name);
    CHECK(landung >= 0 && e->y == 0 && e->floor == 0,
          "[%s] keine Landung auf der Spieler-Ebene (y=%d b%d)", name, e->y, e->floor);
    CHECK(folge_ok, "[%s] Fallfolge weicht von -10 + 20*t ab", name);
    (void)luft_band1;
}

int main(void)
{
    long sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1200.RDT", &sz);
    if (!buf) { printf("FAIL: ROOM1200.RDT fehlt\n"); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0) { printf("FAIL: parse\n"); free(buf); return 1; }
    g_room_rdt = rdt; g_room_rdt_ok = 1;

    teil_a();

    /* n = 1 (zwei Baender): synthetische Zelle im selben Raum — Wort 0x1306 an Zelle 13 */
    {
        re15_rdt_t r2 = rdt;
        static re15_sca_entry_t zellen[256];
        int n = rdt.sca_count < 256 ? rdt.sca_count : 256;
        memcpy(zellen, rdt.sca, (size_t)n * sizeof zellen[0]);
        for (int i = 0; i < n; i++)
            if ((zellen[i].floor >> 4) == 1 && (zellen[i].u1 & 2) &&
                zellen[i].z == -17351) zellen[i].u1 = 0x06;    /* (r & 0xc) >> 2 = 1 */
        r2.sca = zellen;
        g_room_rdt = r2;
        re15_actor_t z; memset(&z, 0, sizeof z);
        z.active = 1; z.type = 0x10; z.floor = 2; z.y = -3600; z.dog_floor_y = -3600;
        z.x = -24500; z.z = -16800;
        /* Band 2 hat keine Zelle -> erst auf Band 1 stellen, dann Wort 0x1306 lesen */
        z.floor = 1; z.y = -1800; z.dog_floor_y = -1800;
        re15_schwerkraft_8001bd60(&z, -10, 0x14, 400);
        CHECK(z.dog_floor_y == 1800 && z.floor == (uint8_t)0xff,
              "n=1: zwei Baender (+0x1ba +3600, +0x82 -2), ist %d / %d", (int)z.dog_floor_y, z.floor);
        CHECK(z.fall_1c0 == (0x8000u | (1u << 13)) + 1u, "n=1: Fallwort %04x", z.fall_1c0);
        g_room_rdt = rdt;
    }

    teil_b(&rdt, RE15_AI_FLAVOR_RE15, "RE1.5");
    teil_b(&rdt, RE15_AI_FLAVOR_RE2,  "RE2");

    free(buf);
    if (fails) { printf("test_r35_raeume_trage: %d FAIL\n", fails); return 1; }
    printf("test_r35_raeume_trage: OK\n");
    return 0;
}
