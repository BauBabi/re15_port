/*
 * Runde 36 Spur F "zombieki" — Gorilla-Schattenrecord und Blutlache der Leiche (Belege: re15_zombieki_r36.h).
 */
#include "re15_zombieki_r36.h"
#include "re15_room.h"
#include "re15_ai_flavor.h"

/* ----- Punkt 1: Sicht-Bit des RE2-Zombies (Belege: re15_zombieki_r36.h) ----- */
static int s_r36_zombie_sicht_an = 1;   /* NUR Sonden: 0 = alter Weg re15_re2_los_clear (Slab-Test) */
void re15_r36_zombie_sicht_enable(int an) { s_r36_zombie_sicht_an = an ? 1 : 0; }

int re15_r36_zombie_sicht_frei(re15_actor_t *e, re15_actor_t *pl)
{
    if (!s_r36_zombie_sicht_an) return re15_re2_los_clear(e, pl) ? 1 : 0;
    if (!g_room_rdt_ok) return 1;                                   /* keine Zellen -> frei (wie bisher) */
    return re15_r36_los_ray_nib3(e, pl->x, pl->z) ? 0 : 1;          /* FUN_8003dcc4(0xf00, 0x300) @0x8001b8d4/8ec */
}

/* NUR FUER SONDEN (Gegenprobe "vorher" im selben Lauf): 0 = die Leichen-Haken schreiben nichts. Das Spiel ruft das nie. */
static int s_r36_affen_lache_an = 1;
void re15_r36_affen_lache_enable(int an) { s_r36_affen_lache_an = an ? 1 : 0; }

void re15_r36_affen_lache_init(re15_actor_t *e)
{
    if (!e) return;
    e->crow_shadow_w = (uint16_t)RE15_R36_AFFEN_SCHATTEN_X;   /* +0xBC = 1600 + 100 @0x801171e8 -> sh @0x8001b030 */
    e->crow_shadow_h = (uint16_t)RE15_R36_AFFEN_SCHATTEN_Z;   /* +0xBE = 1600 + 200 @0x801171f0 -> sh @0x8001b038 */
    e->crow_pool     = 0;                                     /* Farbe des Records: 0x80808080 @0x801171bc (grau) */
}

void re15_r36_affen_lache_tick(re15_actor_t *e)
{
    if (!e || !s_r36_affen_lache_an) return;
    e->crow_pool     = 1;                                                              /* +0xC4/+0xEC = (alt & 0xff000000) | 0x00ffff38 @0x8011bed0-f00 */
    e->crow_shadow_w = (uint16_t)(e->crow_shadow_w + RE15_R36_AFFEN_LACHE_SCHRITT);   /* +0xBC += 8 @0x8011bf08/bf10 */
    e->crow_shadow_h = (uint16_t)(e->crow_shadow_h + RE15_R36_AFFEN_LACHE_SCHRITT);   /* +0xBE += 8 @0x8011bf0c/bf14 */
}

int re15_r36_affen_lache_render(const re15_actor_t *npc, int32_t *nhx, int32_t *nhz, int *pool,
                                uint8_t *sh_r, uint8_t *sh_g, uint8_t *sh_b)
{
    if (!npc || npc->type != 0x27u || npc->crow_shadow_w == 0) return 0;
    *nhx = npc->crow_shadow_w; *nhz = npc->crow_shadow_h;
    if (npc->crow_pool) {
        *pool = 1;
        *sh_r = 0x38; *sh_g = 0xff; *sh_b = 0xff;   /* 0x00ffff38 (B,G,R-Bytes) @0x8011bed0 `ori a2,a2,0xff38` + `lui a2,0xff` @0x8011be6c */
    }
    return 1;
}
