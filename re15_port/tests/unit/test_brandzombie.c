/* test_brandzombie.c — RIEGEL: brennende Zombies (Flag Zone 1/31, include/re15_brandzombie.h).
 *   bank     Effekt 8 liegt in CORE00.ESP (ROOM5120-Raumbank: 5/7/9/16 -> globaler Rueckfall).
 *   init     Flag aus -> nichts; Flag an -> +0x1d8 |= 0x100, +0x1da in [600,1110], acht Flammen 8/3 am Aktor
 *            (Skalen 0x1800,0x2000,0x1000,0x1000,0x1500,0x1500,0x1000,0x1000 @0x80100954ff).
 *   300      nur die Flamme an Teil 0 schaltet auf Routine 22 (s0-Fehler @0x80101290, Bytes @0x8011fbe4 = 0);
 *            ein ESP-Takt spaeter Routine 23 (Aufflammen @0x80017ecc).
 *   tod      DEATH[21] 0x80108d74: alle acht auf Routine 22, Kill-Flag, am Ende +0x4 = 7.
 *   re2      RE2-Gehirn (Gestaltung): Zaehler 0 -> Zustand 3, Zeile 10 (Brand), hp -1. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_rdt.h"
#include "re15_esp.h"
#include "re15_scd.h"
#include "re15_damage.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_brandzombie.h"

uint8_t re15_em_status_zone(void);
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
static uint32_t rd32(const uint8_t *p, size_t o) { return (uint32_t)p[o] | (uint32_t)p[o+1] << 8 | (uint32_t)p[o+2] << 16 | (uint32_t)p[o+3] << 24; }
static re15_esp_t s_bank;
#define Z 3

static int flammen(int routine_min, int routine_max)
{
    int n = 0;
    for (int i = 0; i < 256; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || f->effect_id != 8 || f->sub_index != 3 || f->follow_slot != Z) continue;
        int r = f->row[0] | f->row[1] << 8;
        if (r >= routine_min && r <= routine_max) n++;
    }
    return n;
}
static re15_actor_t *zombie(void)
{
    re15_actor_t *e = &g_actors[Z];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x18; e->hp = 80; e->em_flag_id = 9; e->state = 1;
    return e;
}

int main(void)
{
    size_t n = 0; uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5120.RDT", &n);
    if (!raw) { fprintf(stderr, "SKIP\n"); return 77; }
    CHECK(re15_esp_parse(raw, n, rd32(raw, 0x4C), rd32(raw, 0x50), rd32(raw, 0x54), rd32(raw, 0x58), &s_bank) == 0, "ESP 5120");
    /* Raum-ESP 5120 fuehrt die Ids 5/7/9/16 (RDT+0x4C); Id 8 kommt wie bei den Feuern in 1090 aus CORE00.ESP. */
    {   size_t gn = 0; uint8_t *g = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &gn);
        static re15_esp_t gb;
        CHECK(g && re15_esp_parse_global(g, gn, &gb) == 0, "CORE00.ESP");
        CHECK(re15_esp_find_id(&gb, 8) >= 0, "Effekt 8 fehlt in CORE00.ESP");
        re15_esp_set_global_bank(&gb); }
    re15_esp_set_room_bank(&s_bank);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init();

    /* Flag aus: nichts */
    re15_game_flag_set(1, 31, 0); re15_esp_fx_reset();
    re15_actor_t *e = zombie(); re15_brand_init(e, Z);
    CHECK(!(e->ai_flags & 0x100) && flammen(0, 0xffff) == 0, "ohne Flag darf nichts brennen");

    /* Flag an */
    re15_game_flag_set(1, 31, 1); re15_esp_fx_reset(); re15_damage_seed_rng(77);
    e = zombie(); re15_brand_init(e, Z);
    CHECK(e->ai_flags & 0x100, "+0x1d8 |= 0x100");
    CHECK(e->ai_attack_timer >= 600 && e->ai_attack_timer <= 1110, "+0x1da = %d", e->ai_attack_timer);
    CHECK(flammen(0, 0xffff) == 8, "acht Flammen, gezaehlt %d", flammen(0, 0xffff));
    {   static const uint16_t sk[8] = { 0x1800, 0x2000, 0x1000, 0x1000, 0x1500, 0x1500, 0x1000, 0x1000 };
        int k = 0;
        for (int i = 0; i < 256 && k < 8; i++) {
            const re15_esp_fx_t *f = re15_esp_fx_get(i);
            if (!f || f->effect_id != 8 || f->follow_slot != Z) continue;
            CHECK(f->scale16 == sk[k], "Flamme %d Skala 0x%x", k, f->scale16);
            CHECK(f->follow_part >= 0, "Flamme %d ohne Teil-Anker", k);
            k++;
        }
    }
    if (getenv("BRAND_DBG")) for (int i = 0; i < 256; i++) { const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->effect_id == 8) fprintf(stderr, "fx %d rows %p row0 %d part %d flags %x\n", i, (const void *)f->rows_base, f->row[0], f->follow_part, f->flags); }
    re15_brand_aufflammen_300(e, Z);
    CHECK(flammen(22, 22) == 1, "bei 300 flammt genau die Flamme an Teil 0 auf (%d)", flammen(22, 22));
    re15_esp_fx_tick(&s_bank);
    CHECK(flammen(23, 23) == 1, "Routine 22 -> 23 (%d)", flammen(23, 23));

    /* Tod Zeile 21 */
    e->state = 3; e->sub_state_1 = 0x15; e->sub_state_2 = 0; e->sub_state_3 = 0; e->motion = 0x0b;
    int ok = re15_brand_tod(e, Z);
    CHECK(ok && (e->hit_react & 1) && e->sub_state_3 >= 1, "Phase 0");
    CHECK(flammen(22, 23) == 8, "alle acht flammen auf (%d)", flammen(22, 23));
    CHECK(re15_game_flag_get(re15_em_status_zone(), 9), "Kill-Flag");
    for (int f = 0; f < 400 && e->state != 7; f++) { re15_actors_anim_advance(); re15_brand_tod(e, Z); }
    CHECK(e->state == 7, "am Ende Leiche (+0x4 = 7), ist %d", e->state);

    /* RE2-Gehirn */
    re15_esp_fx_reset();
    e = zombie(); e->ai_flags = 0x100; e->ai_attack_timer = 2; e->state = 1;
    re15_brand_re2_takt(e, Z); CHECK(e->state == 1, "RE2: Zaehler 1 lebt noch");
    re15_brand_re2_takt(e, Z);
    CHECK(e->state == 3 && e->sub_state_1 == 10 && e->hp < 0 && e->re2z_hits1d2 == 1, "RE2: Brand-Tod Zeile 10 (state %d row %d hp %d)",
          e->state, e->sub_state_1, e->hp);

    if (fails) { fprintf(stderr, "%d Fehler\n", fails); return 1; }
    fprintf(stderr, "OK\n"); return 0;
}
