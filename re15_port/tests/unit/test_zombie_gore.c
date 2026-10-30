/* test_zombie_gore.c — RIEGEL: waffenabhaengige Zombie-Treffer/-Tode (zombiegore_r15.c, STAGE1 0x80100424) und die
 * RE1.5-Teilschicht (re15_teile_r15.h). RE1.5-KI explizit (Default ist RE2).
 *
 *   kopf     Magnum Mitte (DEATH[7][1] = 0x80106edc): entweder +0x6 = 5 (-> kopflos) oder Gore-Teil |= 0x4a mit
 *            Farbwort 0x8f, vy -50 / +3 (@0x801070b0-e4), Teil 7 roter Stumpf, drei Blut-Spawns inkl. Effekt 5.
 *   kopflos  Spalte 5 (0x80107244): laeuft im Gang-Clip, Zusatztreffer (+0x93 & 2) spritzen an Teil 7, Ende -> 6edc
 *            Phase 1 mit Clip 0xb.
 *   schrot   Schrot Mitte (0x80107ee0) ohne Krit -> +0x6 = 6 (Standard-Tod); mit Krit -> Zerfetzen (Teile 10/13 Wort 0)
 *            oder Oberkoerper ab (Teil 7 |= 0x40) und weiter als Kriecher (grid 0x81, hp 30).
 *   beine    Nach unten (0x80108abc): Teile 2/3/5/6 |= 0x4a, Clip-Ende -> Kriecher grid 0x81 / hp 30.
 *   beretta  HURT w5 (0x80106290): Clips 6 -> 7 -> 8, Ausgang 0x201.
 *   magnum   HURT w7 Mitte (0x80106624): Rueckstoss nach hinten (+0x8c 100, -12 ...).
 *   scherben Teilschicht 0x08: Bild-Zaehler bis 0x1d, dann Teil-Wort 0 (@0x8001f208-214).
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
#include "re15_esp.h"
#include "re15_player.h"
#include "re15_room.h"
#include "re15_teile_r15.h"
void re15_aspider_beine_reset(void);

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

#define Z 3
static re15_actor_t *zombie(uint8_t state, uint8_t waffe, uint8_t hoehe, uint8_t bits)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl); pl->active = 1; pl->hp = 100; pl->x = 0; pl->z = -3000;
    re15_actor_t *e = &g_actors[Z];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x10; e->x = 0; e->z = 0; e->hp = -1; e->em_flag_id = 0xFF;
    e->state = state; e->sub_state_1 = waffe; e->sub_state_2 = hoehe; e->sub_state_3 = 0;
    e->hit_react = bits; e->hurt_clip = 2; e->hit_stun = 5;
    re15_enemy_apply_hitbox(e, 0x10);
    re15_teile_reset(Z);
    re15_esp_fx_reset();
    return e;
}
static void takt(void (*f)(int)) { re15_actors_anim_advance(); f(Z); }

static void test_kopf(void)
{
    int kopf = 0, kopflos = 0;
    for (int seed = 1; seed < 40 && (!kopf || !kopflos); seed++) {
        re15_damage_seed_rng((uint32_t)seed);
        re15_actor_t *e = zombie(3, 7, 1, 0x40 | 1);
        re15_enemy_ai_live_death(Z);
        if (e->sub_state_2 == 5) { kopflos = 1; continue; }
        re15_teil_t *t = re15_teile(Z);
        CHECK((t[14].w & 0x4a) == 0x4a && t[14].col == 0x8f, "Kopf-Teil 14 nicht zerplatzt (w 0x%x col 0x%x)", t[14].w, t[14].col);
        CHECK((int16_t)t[14].h94[1] == -50 && (int16_t)t[14].h94[3] == 3, "Kopf vy/inc != -50/3");
        CHECK((t[7].w & 0x80) && re15_teil_tinte1(&t[7]) == 0x80 && re15_teil_tinte2(&t[7]) == 0x20, "Teil 7 kein roter Stumpf");
        CHECK(re15_esp_fx_count() >= 3, "Kopfplatzer: %d Blut-Spawns < 3", re15_esp_fx_count());
        CHECK(re15_teile_an(Z), "Teilschicht nicht markiert");
        kopf = 1;
    }
    CHECK(kopf && kopflos, "Magnum: beide Zweige (kopf %d / kopflos %d) muessen vorkommen", kopf, kopflos);
}

static void test_kopflos(void)
{
    re15_damage_seed_rng(3);
    re15_actor_t *e = zombie(3, 7, 5, 0x40 | 1);
    re15_enemy_ai_live_death(Z);
    CHECK(e->motion == 2 && (re15_teile(Z)[14].w & 0x4a) == 0x4a, "kopflos: Gang-Clip / Kopf-Burst fehlt");
    int spritzer = 0, n;
    for (n = 0; n < 300 && !(e->sub_state_2 == 1); n++) {
        if (n % 10 == 5) e->hit_react |= 2;                       /* weiterer Schuss */
        int vor = re15_esp_fx_count();
        takt(re15_enemy_ai_live_death);
        if (re15_esp_fx_count() > vor) spritzer++;
    }
    CHECK(e->sub_state_2 == 1 && e->sub_state_3 == 1 && e->motion == 0x0b, "kopflos endet nicht in 6edc Phase 1 (c %d ph %d clip 0x%x)",
          e->sub_state_2, e->sub_state_3, e->motion);
    CHECK(spritzer >= 1 && spritzer <= 5, "Halsfontaenen %d nicht in 1..5", spritzer);
    fprintf(stderr, "kopflos: %d Bilder, %d Fontaenen\n", n, spritzer);
}

static void test_schrot(void)
{
    re15_damage_seed_rng(1);
    re15_actor_t *e = zombie(3, 8, 1, 1);                         /* kein Krit (>= 3000) */
    re15_enemy_ai_live_death(Z);
    CHECK(e->sub_state_2 == 6 && e->sub_state_3 == 0, "Schrot ohne Krit -> +0x6 6 (ist %d)", e->sub_state_2);
    int fetzen = 0, ab = 0;
    for (int seed = 1; seed < 60 && (!fetzen || !ab); seed++) {
        re15_damage_seed_rng((uint32_t)seed);
        e = zombie(3, 8, 1, 0x40 | 1);
        re15_enemy_ai_live_death(Z);
        re15_teil_t *t = re15_teile(Z);
        if (e->sub_state_3 == 3) {
            CHECK(t[10].w == 0 && t[13].w == 0 && (t[14].w & 0x4a) == 0x4a, "Zerfetzen: Teile 10/13/14 falsch");
            fetzen = 1;
        } else if (t[7].w & 0x40) {
            for (int n = 0; n < 400 && e->grid_id != 0x81; n++) {
                t[7].fest = 1;                                    /* ohne Zeichner: Matrix gilt als uebernommen */
                takt(re15_enemy_ai_live_death);
            }
            CHECK(e->grid_id == 0x81 && e->hp == 30 && e->state == 1, "Oberkoerper ab: kein Kriecher (grid 0x%x hp %d st %d)",
                  e->grid_id, e->hp, e->state);
            CHECK((t[1].w & 0x40) && (t[5].w & 0x40) && !(t[7].w & 0x40), "Beine bleiben nicht liegen / Teil 7 nicht zurueck");
            ab = 1;
        }
    }
    CHECK(fetzen && ab, "Schrot-Krit: beide Ausgaenge (fetzen %d / ab %d)", fetzen, ab);
}

static void test_beine(void)
{
    re15_damage_seed_rng(5);
    re15_actor_t *e = zombie(3, 19, 0, 1);
    re15_enemy_ai_live_death(Z);
    re15_teil_t *t = re15_teile(Z);
    CHECK((t[2].w & 0x4a) == 0x4a && (t[3].w & 0x4a) == 0x4a && (t[5].w & 0x4a) == 0x4a && (t[6].w & 0x4a) == 0x4a,
          "Beine nicht zerplatzt");
    int n;
    for (n = 0; n < 300 && e->grid_id != 0x81; n++) takt(re15_enemy_ai_live_death);
    CHECK(e->grid_id == 0x81 && e->hp == 30, "Beine ab: kein Kriecher (grid 0x%x hp %d)", e->grid_id, e->hp);
}

/* Gestaltung Video 3: Granaten-Tod -> Oberkoerper ab, kriecht weiter (immer, ohne Krit-Bit). */
static void test_granate(void)
{
    re15_damage_seed_rng(11);
    re15_actor_t *e = zombie(3, 9, 1, 1);
    re15_enemy_ai_live_death(Z);
    re15_teil_t *t = re15_teile(Z);
    CHECK((t[7].w & 0x40) && e->sub_state_3 == 1, "Granate: Oberkoerper nicht ab (w7 0x%x ph %d)", t[7].w, e->sub_state_3);
    for (int n = 0; n < 400 && e->grid_id != 0x81; n++) { t[7].fest = 1; takt(re15_enemy_ai_live_death); }
    CHECK(e->grid_id == 0x81 && e->hp == 30, "Granate: kein Kriecher");
}

static void test_beretta(void)
{
    re15_damage_seed_rng(7);
    re15_actor_t *e = zombie(2, 5, 1, 0);
    int clips = 0, letzter = -1, n;
    for (n = 0; n < 400 && e->state == 2; n++) {
        takt(re15_enemy_ai_live_hurt);
        if (e->state == 2 && e->motion != letzter) { clips = clips * 16 + e->motion; letzter = e->motion; }
    }
    CHECK(clips == 0x678, "Beretta-Clips 0x%x != 6,7,8", clips);
    CHECK(e->state == 1 && (e->sub_state_1 == 2 || e->sub_state_1 == 7 || e->sub_state_1 == 8), "Ausgang 0x201 (st %d sub %d)",
          e->state, e->sub_state_1);
}

static void test_magnum_treffer(void)
{
    re15_damage_seed_rng(9);
    re15_actor_t *e = zombie(2, 7, 1, 0);                         /* +0x93 Bit 7 = 0 -> rueckwaerts */
    e->rot_y = 0x400;                                             /* Blick nach -z (Spieler) */
    takt(re15_enemy_ai_live_hurt);
    CHECK(e->motion == 0x10 && e->speed_h == 88, "Rueckstoss: Clip 0x%x / +0x8c %d (100-12)", e->motion, e->speed_h);
    CHECK(e->z > 0, "Rueckstoss schiebt nicht nach hinten (z %d)", e->z);
}

/* Spinne 0x25 (STAGE2 0x80113aa4): Krit-Treffer -> ein Beinpaar platzt gruen (0x207f60); Tod mit Krit -> Teile 0/19. */
static void test_spinne(void)
{
    re15_aspider_beine_reset();
    re15_actor_t *e = &g_actors[Z];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x25; e->hp = 100; e->em_flag_id = 0xFF;
    re15_teile_reset(Z);
    e->state = 2; e->sub_state_3 = 0; e->hit_react = 0x40 | 1;
    re15_damage_seed_rng(2);
    re15_enemy_ai_run_all(1);
    re15_teil_t *t = re15_teile(Z);
    int paar = 0;
    for (int k = 3; k <= 17; k += 2) if ((t[k].w & 0x4a) == 0x4a && (t[k + 1].w & 0x4a) == 0x4a && t[k].col == 0x207f60u) paar++;
    CHECK(paar == 1, "Spinne: %d Beinpaare geplatzt statt 1", paar);
    re15_teile_reset(Z);
    e->state = 3; e->sub_state_3 = 0; e->hit_react = 0x40 | 1;
    re15_enemy_ai_run_all(1);
    CHECK((t[0].w & 0x4a) == 0x4a && (t[19].w & 0x4a) == 0x4a, "Spinne: Koerper (Teile 0/19) nicht geplatzt");
}

static void test_scherben(void)
{
    re15_teile_reset(Z);
    re15_teil_t *t = &re15_teile(Z)[3];
    t->w |= 0x4a; t->col = 0x8f;
    int32_t r[9] = { 0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000 }, p[3] = { 0, -1000, 0 };
    int n;
    for (n = 0; n < 40 && t->w; n++) {
        int32_t rr[9], pp[3]; memcpy(rr, r, sizeof r); memcpy(pp, p, sizeof p);
        int rc = re15_teil_stufe(Z, 3, -1, rr, pp, 0, 1);
        if (rc == 2) re15_teil_scherben_nach(Z, 3, 1);
    }
    CHECK(n == 0x1d, "Scherben leben %d Bilder statt 29", n);
}

int main(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    g_current_room_id = 0x1140u;
    re15_actor_init(); re15_enemy_reset();
    CHECK(bank_laden(0x10), "EM10-Bank");
    if (fails) return 1;
    test_kopf();
    test_kopflos();
    test_schrot();
    test_beine();
    test_granate();
    test_beretta();
    test_magnum_treffer();
    test_spinne();
    test_scherben();
    if (fails) { fprintf(stderr, "%d Fehler\n", fails); return 1; }
    fprintf(stderr, "OK\n");
    return 0;
}
