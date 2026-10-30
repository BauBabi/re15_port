/* test_spinne_klettern.c — RIEGEL Runde 42: erwachsene Spinne 0x25 (STAGE2) an Wand, Absatz und Decke, RE1.5-KI.
 * (Unter dem RE2-Gehirn, dem Standard, laeuft 0x25 ueber re15_re2spider_tick — dieser Test setzt RE1.5 explizit.)
 *
 *   absatz   WALK + Wandkontakt vorn (+0x90&3, grid<2, +-512 @0x801118d4-fc) -> ACT 9; Absatz hoch (+0x1ec=1):
 *            Clip 0xe, Bild 0x1a -> +0x1ba -= 1800 / +0xb0 = 0x904, Clip-Ende -> +0x82 += 1, y -= 1800, +0x8c 2800,
 *            danach +0x5/+0x6 = 0 (@0x80112af8).
 *   ausweich ACT 10 (@0x80112e58): Clip 6, +0x1e6 = +-812 (+rng ohne Sicht), +0x8c 200..263, Ende +0x6 = LOS*4, +0x1dc = 0.
 *   klettern Kletterzelle (+0xa & 0xc) vorn -> ACT 12 (+0x1e8/+0x1ea aus der Zelle), Wechsel Boden->Wand: grid 3,
 *            y -= 1488, +0x6c -= 1024, Kasten C (200/1440); dann ACT 13 hoch bis y < -9000, ACT 12 Phase 2 ->
 *            Decke: grid 2, y = -10800, Kasten A, +0x1d6 = 0x3c; DECIDE fuer grid 3 ist `jr ra` (@0x801115bc).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_spinne_r42.h"

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint8_t s_blob[0x80000];
static int bank_laden(uint8_t type)
{
    size_t n = 0, off = 0, len = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems || re15_ems_get_entry(ems, n, re15_ems_index_for_type(type), &off, &len) != 0 || len > sizeof s_blob) { free(ems); return 0; }
    memcpy(s_blob, ems + off, len); free(ems);
    re15_enemy_bank_t *eb = re15_enemy_find(type); if (!eb) eb = re15_enemy_alloc(type);
    re15_tim_t tim = (re15_tim_t){0};
    if (!eb || re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->own_ok  = (re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own) == 0);
    return 1;
}

#define S 3
static re15_sca_entry_t s_zelle[1];

static re15_actor_t *spinne(uint8_t sub1, uint8_t sub2)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl); pl->active = 1; pl->hp = 100; pl->x = 20000; pl->z = 20000;
    re15_actor_t *e = &g_actors[S];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x25; e->hp = 100; e->state = 0; e->grid_id = 0; e->floor = 0;
    e->x = 0; e->z = 0; e->y = 0; e->dog_floor_y = 0;
    re15_enemy_ai_run_all(1);                       /* INIT 0x80110b6c -> ACTIVE */
    e->state = 1; e->sub_state_1 = sub1; e->sub_state_2 = sub2; e->sub_state_3 = 0;
    e->rot_y = 0; e->rot_z = 0; e->aspider_los = 0; e->dog_atk_cd = 0;
    return e;
}

static void test_absatz(void)
{
    re15_actor_t *e = spinne(1, 1);
    e->ai_contact = 0x09;                           /* Richtung 0, Kontakt 8, u1&3 = 1 */
    re15_enemy_ai_run_all(1);
    CHECK(e->sub_state_2 == 9 && e->sub_state_1 == 1, "WALK + Wand vorn -> ACT 9 (ist %d/%d)", e->sub_state_1, e->sub_state_2);
    CHECK(re15_spinne42_feld(S, 0x1ec) == 1 && e->zg_9f == 0x09, "+0x1ec = +0x90&1, +0x9f = +0x90");
    int32_t y0 = e->y; int fl0 = e->floor; int sah_904 = 0, fy = 0;
    for (int f = 0; f < 200 && e->sub_state_2 == 9; f++) {
        re15_enemy_ai_run_all(1);
        if (re15_spinne42_feld(S, 0xb0) == 0x904) { sah_904 = 1; fy = e->dog_floor_y; }
    }
    CHECK(sah_904 && fy == -1800, "Bild 0x1a: +0xb0 = 0x904, +0x1ba -= 1800 (ist %d)", fy);
    CHECK(e->sub_state_2 == 0 && e->sub_state_1 == 0, "Absatz-Ende -> +0x5/+0x6 = 0 (ist %d/%d)", e->sub_state_1, e->sub_state_2);
    CHECK(e->y == y0 - 1800 && e->floor == fl0 + 1, "Absatz hoch: y -1800 (ist %d), +0x82 +1 (ist %d)", e->y - y0, e->floor - fl0);
    CHECK(re15_spinne42_feld(S, 0xb0) == 0, "+0xb0 am Ende 0");
}

static void test_ausweichen(void)
{
    re15_actor_t *e = spinne(1, 10);
    e->aspider_los = 1; e->ai_target_x = 40;
    re15_enemy_ai_run_all(1);
    CHECK(e->motion == 6 && (e->speed_h == 812 || e->speed_h == -812), "ACT 10: Clip 6, +0x1e6 = +-812 (ist %d/%d)", e->motion, e->speed_h);
    CHECK(e->crow_speed >= 200 && e->crow_speed <= 263, "+0x8c = (rng&0x3f)+200 (ist %d)", e->crow_speed);
    for (int f = 0; f < 300 && e->sub_state_2 == 10; f++) re15_enemy_ai_run_all(1);
    CHECK(e->sub_state_2 == 4 && e->ai_target_x == 0, "Ende: +0x6 = LOS*4, +0x1dc = 0 (ist %d / %d)", e->sub_state_2, e->ai_target_x);
}

static void test_klettern(void)
{
    s_zelle[0].x = 2000; s_zelle[0].z = 1000; s_zelle[0].width = 300; s_zelle[0].density = 500;
    s_zelle[0].type = 1; s_zelle[0].u0 = 4; s_zelle[0].u1 = 0x04; s_zelle[0].floor = 0;
    g_room_rdt.sca = s_zelle; g_room_rdt.sca_count = 1; g_room_rdt_ok = 0;   /* Klemme aus, Zelle lesbar */
    re15_actor_t *e = spinne(1, 1);
    e->ai_contact = 0x08; e->coll_cell = 0;         /* Kontakt ohne u1&3, Zelle kletterbar (u1 & 0xc) */
    re15_enemy_ai_run_all(1);
    CHECK(e->sub_state_2 == 12, "Kletterzelle vorn -> ACT 12 (ist %d)", e->sub_state_2);
    CHECK(re15_spinne42_feld(S, 0x1e8) == 1000 && re15_spinne42_feld(S, 0x1ea) == 1500 && re15_spinne42_feld(S, 0x1ee) == 0,
          "+0x1e8/+0x1ea = z / z+Tiefe (ist %d/%d)", re15_spinne42_feld(S, 0x1e8), re15_spinne42_feld(S, 0x1ea));
    int f;
    for (f = 0; f < 400 && !(e->sub_state_2 == 13); f++) re15_enemy_ai_run_all(1);
    CHECK(e->grid_id == 3 && e->y == -1488 && e->rot_z == -1024, "Boden -> Wand: grid 3, y -1488, +0x6c -1024 (ist %d/%d/%d)",
          e->grid_id, e->y, e->rot_z);
    CHECK(e->hit_radius_min == 200 && e->hit_height == 1440, "Wandkasten C (ist %u/%u)", e->hit_radius_min, e->hit_height);
    re15_enemy_ai_run_all(1);
    CHECK(e->sub_state_2 == 13 || e->sub_state_2 == 12, "grid 3: kein DECIDE, ACT 13 (ist %d)", e->sub_state_2);
    for (f = 0; f < 2000 && e->grid_id != 2; f++) re15_enemy_ai_run_all(1);
    CHECK(e->grid_id == 2 && e->y == -10800, "Wand -> Decke: grid 2, y -10800 (ist %d/%d)", e->grid_id, e->y);
    CHECK(e->hit_radius_min == 1000 && e->dog_atk_cd >= 0x3b, "Deckenkasten A, +0x1d6 = 0x3c (ist %u/%d)", e->hit_radius_min, e->dog_atk_cd);
    g_room_rdt.sca = NULL; g_room_rdt.sca_count = 0;
}

int main(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    g_current_room_id = 0x2030u;
    re15_actor_init(); re15_enemy_reset();
    CHECK(bank_laden(0x25), "EM025-Bank");
    if (fails) return 1;
    re15_damage_seed_rng(0x1234567u);
    test_absatz();
    test_ausweichen();
    test_klettern();
    if (fails) { fprintf(stderr, "%d Fehler\n", fails); return 1; }
    fprintf(stderr, "OK\n");
    return 0;
}
