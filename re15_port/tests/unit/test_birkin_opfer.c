/* test_birkin_opfer.c — RIEGEL: Leons Reaktion auf Birkin-Treffer (Spieler-Haken 0x8011afd8 STAGE5, birkin_opfer.c).
 *
 * Nutzer-Report v0.8.29 (ROOM5080, zweiter Birkin-Kampf): "stuerzt sogar Leon's animation ab, wenn man von birkin
 * getroffen wird". Vorher setzte jeder Treffer nur den Pin — Leon blieb in der Steh-Pose eingefroren.
 *
 *   vorn    Haken [0]: Clip 0xd ab Bild 6 (@0x8011b06c-80), Tempo 0x4b0 -150/Bild, Boden 400 (@0x8011b104-130),
 *           Rueckstoss entgegen der Gier (FUN_800245d8(0x800) @0x8011b3f0), dann Clip 0xe (Tempo -50), Loesen.
 *   wand    Haken [0] mit Wand-Bit (@0x8011b140): Clip 0xf (@0x8011b230), Ringkampf in Opfer-Clip 0 mit Zaehler
 *           0x4b (@0x8011b288), Aufstehen 0x10 -> 0xb rueckwaerts, Loesen.
 *   hinten  Haken [1]: Clip 0xc (@0x8011b458), Tempo 0x190 -40, Ringkampf Zaehler 0x19 (@0x8011b55c), 0x10, 0xb.
 *   biss    Haken [2]: Opfer-Clip 2 zweimal (acaf2 = 2 @0x8011b754), Ringkampf Zaehler 0x32 (@0x8011b7f4), 0x10, 0xb.
 *   wurf3   STAGE3-Wurf (cmd 6 -> Stub 0x8011b168 -> 0x8011b1a8): Opfer-Clip 1 (@0x8011b260) ganz, Endpose
 *           gehalten, dann cmd 7 (@0x8011b598). In STAGE5 laeuft derselbe Wurf ueber [0] (@0x8011defc).
 *   ki      echte Birkin-KI (Typ 0x30) greift einen stehenden Spieler an: der Haken startet, Leon spielt eigene
 *           Clips statt der Steh-Pose, und er kommt wieder frei.
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
#include "re15_damage.h"
#include "re15_player.h"
#include "re15_room.h"
#include "re15_birkin_opfer.h"

void re15_player_wand_bit_test(int an);   /* game_step_common.c (Testzugang) */
void re15_player_victim_haken(const re15_actor_t *greifer, int an);   /* enemy_ai_common.c */
static void re15_player_victim_haken_reset_test(void) { re15_player_victim_haken(NULL, 0); }

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}

static uint8_t s_blob[0x80000];
static re15_enemy_bank_t *s_vb = NULL;
static int bank_laden(void)
{
    size_t esz = 0, off = 0, len = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    if (!ems || re15_ems_get_entry(ems, esz, re15_ems_index_for_type(0x30), &off, &len) != 0 || len > sizeof s_blob) {
        free(ems); return 0;
    }
    memcpy(s_blob, ems + off, len); free(ems);
    re15_enemy_bank_t *eb = re15_enemy_find(0x30);
    if (!eb) eb = re15_enemy_alloc(0x30);
    if (!eb || re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    eb->victim_ok = (re15_emd_parse_victim_bank(s_blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
    s_vb = eb;
    return eb->victim_ok;
}

static re15_emd_animation_t s_pl00_anim; static re15_emd_skeleton_t s_pl00_skel;
static int pl00_laden(void)
{
    size_t esz = 0, rsz = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    if (!edd || !emr) return 0;
    if (re15_emd_parse_animation(edd, esz, &s_pl00_anim) != 0 || re15_emd_parse_skeleton(emr, rsz, &s_pl00_skel) != 0) return 0;
    re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);
    return 1;                                   /* edd/emr bleiben fuer die Bank-Zeiger am Leben */
}

static int s_birkin = 0;
static void birkin_setzen(int32_t x, int32_t z)
{
    re15_actor_t *a = &g_actors[s_birkin];
    memset(a, 0, sizeof *a);
    a->active = 1; a->type = 0x30; a->x = x; a->z = z; a->hp = 400; a->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(a, 0x30);
}

static re15_actor_t *spieler(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->hp = 100; pl->x = 0; pl->z = 0; pl->rot_y = 0;
    return pl;
}

/* Ein Spieler-Bild: der Haken (re15_player_victim_tick ruft re15_bk_opfer_tick). */
static void bild(void) { re15_player_victim_tick(); }

static void test_vorn(void)
{
    re15_bk_opfer_reset(); re15_player_wand_bit_test(0);
    re15_actor_t *pl = spieler();
    birkin_setzen(0, 2000);
    CHECK(re15_bk_opfer_start(&g_actors[s_birkin], 5, 0), "Start [0]");
    int fc_d = re15_player_pl00_clip_frames(0x0d), fc_e = re15_player_pl00_clip_frames(0x0e);
    bild();
    CHECK(pl->motion == 0x0d && pl->anim_frame == 7, "P0->P1: Clip 0xd Bild 6+1 (motion 0x%x Bild %d)", pl->motion, pl->anim_frame);
    CHECK(pl->hit_react & 1, "+0x93 |= 1");
    int32_t schritt0 = pl->x * pl->x + pl->z * pl->z;
    CHECK(schritt0 > 0, "kein Rueckstoss im ersten Bild");
    int n = 1, sah_e = 0, frei = -1;
    for (; n < 400 && re15_bk_opfer_cmd(); n++) {
        bild();
        if (pl->motion == 0x0e) sah_e = 1;
    }
    frei = n;
    CHECK(!re15_bk_opfer_cmd() && re15_player_victim_state() == 0, "Haken endet nicht");
    CHECK(sah_e, "Clip 0xe nicht gespielt");
    CHECK((pl->hit_react & 1) == 0, "+0x93 nach dem Loesen noch gesetzt");
    /* Dauer: 0xd ab 6 -> (fc_d - 6) Bilder, 0xe -> fc_e Bilder, Loesen im Bild danach */
    CHECK(frei == (fc_d - 6) + fc_e + 1, "Dauer %d != %d (0xd %d, 0xe %d)", frei, (fc_d - 6) + fc_e + 1, fc_d, fc_e);
    /* Gier 0 = Blick nach +x (cos), FUN_800245d8(0x800) schiebt nach -x */
    CHECK(pl->x < -2000, "Rueckstoss nicht entgegen der Gier (x %d)", pl->x);
    fprintf(stderr, "vorn: frei nach %d Bildern, x %d z %d\n", frei, pl->x, pl->z);
}

static void test_wand(void)
{
    re15_bk_opfer_reset(); re15_player_wand_bit_test(1);
    re15_actor_t *pl = spieler();
    birkin_setzen(0, 2000);
    re15_bk_opfer_start(&g_actors[s_birkin], 5, 0);
    bild();                                      /* P0 -> P1 -> Wand -> aca5a 5 */
    CHECK(re15_bk_opfer_phase() == 5, "Wand-Bit -> Phase 5 (ist %d)", re15_bk_opfer_phase());
    re15_player_wand_bit_test(0);
    bild();
    CHECK(pl->motion == 0x0f && re15_bk_opfer_eigenbank(), "Clip 0xf aus Leons Bank (0x%x)", pl->motion);
    int ring = 0, auf = 0, rueck = 0, n;
    for (n = 0; n < 600 && re15_bk_opfer_cmd(); n++) {
        bild();
        if (re15_bk_opfer_phase() == 8 && pl->motion == 0 && !re15_bk_opfer_eigenbank()) ring++;
        if (pl->motion == 0x10) auf = 1;
        if (pl->motion == 0x0b && (pl->anim_flags & 0x80)) rueck = 1;
    }
    /* Zaehler 0x4b, ohne Druck -4/Bild: 75 -> < 0 im 19. Ringkampf-Bild (dort aca5a = 9) -> 18 Bilder mit Phase 8 */
    CHECK(ring == 18, "Ringkampf %d Bilder != 18", ring);
    CHECK(auf && rueck, "Aufstehen 0x10 / 0xb rueckwaerts fehlt (%d %d)", auf, rueck);
    CHECK(!re15_bk_opfer_cmd(), "Wand-Haken endet nicht");
}

static void test_hinten(void)
{
    re15_bk_opfer_reset(); re15_player_wand_bit_test(0);
    re15_actor_t *pl = spieler();
    birkin_setzen(0, 2000);
    re15_bk_opfer_start(&g_actors[s_birkin], 5, 1);
    bild();
    CHECK(pl->motion == 0x0c, "Clip 0xc (0x%x)", pl->motion);
    CHECK(pl->x > 0, "Sturz nach vorn (FUN_800245d8(0)) x %d", pl->x);
    int ring = 0, n;
    for (n = 0; n < 600 && re15_bk_opfer_cmd(); n++) {
        bild();
        if (re15_bk_opfer_phase() == 3) ring++;
    }
    CHECK(ring == 6, "Ringkampf %d Bilder != 6 (0x19: < 0 im 7. Bild)", ring);
    CHECK(!re15_bk_opfer_cmd(), "Haken [1] endet nicht");
}

static void test_biss(void)
{
    re15_bk_opfer_reset();
    re15_actor_t *pl = spieler();
    birkin_setzen(0, 2000);
    re15_bk_opfer_start(&g_actors[s_birkin], 5, 2);
    int fc2 = s_vb->anim_victim.clips[2].frame_count;
    int biss = 0, ring = 0, n;
    for (n = 0; n < 600 && re15_bk_opfer_cmd(); n++) {
        bild();
        if (pl->motion == 2 && !re15_bk_opfer_eigenbank()) biss++;
        if (re15_bk_opfer_phase() == 3 && pl->motion == 0) ring++;
    }
    CHECK(biss == 2 * fc2, "Biss %d Bilder != 2 * %d", biss, fc2);
    CHECK(ring == 12, "Ringkampf %d Bilder != 12 (0x32: < 0 im 13. Bild)", ring);
    CHECK(pl->x == 0 && pl->z == 0, "Biss schiebt Leon (kein FUN_800245d8 in [2])");
    CHECK(!re15_bk_opfer_cmd(), "Haken [2] endet nicht");
}

static void test_wurf3(void)
{
    re15_bk_opfer_reset();
    re15_actor_t *pl = spieler();
    birkin_setzen(0, 2000);
    g_current_room_id = 0x3070u;
    CHECK(re15_bk_opfer_start(&g_actors[s_birkin], 6, 0), "Start Wurf STAGE3");
    CHECK(re15_bk_opfer_variante() == 3, "STAGE3-Wurf nicht Variante 3 (%d)", re15_bk_opfer_variante());
    int fc1 = s_vb->anim_victim.clips[1].frame_count, n, clip1 = 0;
    for (n = 0; n < 200 && pl->state != 7; n++) {
        bild();
        if (pl->motion == 1 && !re15_bk_opfer_eigenbank()) clip1++;
    }
    CHECK(pl->state == 7 && pl->hp < 0, "kein cmd 7 nach dem Wurf (state %d hp %d)", pl->state, pl->hp);
    CHECK(clip1 == fc1 + 1, "Opfer-Clip 1: %d Bilder != %d + Tod-Bild", clip1, fc1);
    CHECK(pl->anim_frame == fc1 - 1, "Endpose nicht gehalten (Bild %d)", pl->anim_frame);
    for (int k = 0; k < 10; k++) bild();
    CHECK(pl->anim_frame == fc1 - 1, "cmd 7 animiert weiter");
    re15_bk_opfer_reset(); re15_player_victim_haken_reset_test();
    g_current_room_id = 0x5080u;
    pl = spieler();
    CHECK(re15_bk_opfer_start(&g_actors[s_birkin], 6, 0) && re15_bk_opfer_variante() == 0, "STAGE5-Wurf nicht [0]");
    re15_bk_opfer_reset(); re15_player_victim_haken_reset_test();
    g_current_room_id = 0x1100u;
    CHECK(!re15_bk_opfer_start(&g_actors[s_birkin], 5, 0), "Haken ausserhalb STAGE3/5 gestartet");
    g_current_room_id = 0x5080u;
}

static void test_ki(void)
{
    re15_bk_opfer_reset(); re15_player_wand_bit_test(0);
    re15_actor_t *pl = spieler();
    pl->rot_y = 0x400;                           /* Gier 0x400 = Blick nach -z (sin), Birkin steht davor */
    birkin_setzen(0, -2600);
    int start = -1, var = -1, eigen = 0, steh = 0, n;
    for (n = 0; n < 900; n++) {
        re15_enemy_ai_run_all(1);
        bild();
        if (start < 0 && re15_bk_opfer_cmd()) { start = n; var = re15_bk_opfer_variante(); }
        if (start >= 0 && re15_bk_opfer_cmd()) {
            if (re15_bk_opfer_eigenbank() && pl->motion >= 0x0b) eigen = 1;
            if (pl->motion == 0) steh += re15_bk_opfer_eigenbank();
        }
        if (start >= 0 && !re15_bk_opfer_cmd()) break;
    }
    CHECK(start >= 0, "Birkin-KI startet keinen Haken (sub %d, Abstand ~2600)", g_actors[s_birkin].sub_state_1);
    CHECK(eigen, "Leon spielt im Haken keine Reaktions-Clips");
    CHECK(steh == 0, "Leon steht im Haken (Clip 0 aus der eigenen Bank)");
    CHECK(start < 0 || !re15_bk_opfer_cmd(), "Haken endet nicht (Phase %d)", re15_bk_opfer_phase());
    fprintf(stderr, "ki: Haken ab Bild %d, Variante %d, Ende Bild %d, hp %d\n", start, var, n, pl->hp);
}

int main(void)
{
    g_current_room_id = 0x5080u;
    re15_enemy_reset();
    re15_actor_init();
    CHECK(pl00_laden(), "PL00.EDD/EMR");
    CHECK(bank_laden(), "EM30-Bank mit Opfer-Paar (dir[5]/[6])");
    if (fails) return 1;
    CHECK(s_vb->anim_victim.clip_count == 3, "EM30-Opferbank %d Clips != 3", s_vb->anim_victim.clip_count);
    s_birkin = 5;
    test_vorn();
    test_wand();
    test_hinten();
    test_biss();
    test_wurf3();
    test_ki();
    if (fails) { fprintf(stderr, "%d Fehler\n", fails); return 1; }
    fprintf(stderr, "OK\n");
    return 0;
}
