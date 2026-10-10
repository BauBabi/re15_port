/*
 * Runde 36 Spur F "zombieki" — Gorilla-Schattenrecord und Blutlache der Leiche (Belege: re15_zombieki_r36.h).
 */
#include "re15_zombieki_r36.h"
#include "re15_room.h"
#include "re15_ai_flavor.h"
#include "re15_collision.h"
#include "re15_skeleton.h"   /* re15_cos_q12 / re15_sin_q12 */
#include <string.h>

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

/* ----- Punkt 2: Pad-Suche des Gorillas in der blinden Jagd (PORT-WAHL, Belege: re15_zombieki_r36.h) ----- */
static int     s_r36_padsuche_an = 1;              /* NUR Sonden: 0 = Original-Verhalten (Gegenprobe) */
static uint8_t s_r36_padsuche_aktiv[RE15_ACTOR_MAX];   /* 0 = aus, 1 = zum Pad, 2 = auf dem Pad in Pad-Richtung drehen */
static int16_t s_r36_padsuche_dir[RE15_ACTOR_MAX];
void re15_r36_affen_padsuche_enable(int an) { s_r36_padsuche_an = an ? 1 : 0; memset(s_r36_padsuche_aktiv, 0, sizeof s_r36_padsuche_aktiv); }
int  re15_r36_affen_padsuche_aktiv(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_r36_padsuche_aktiv[slot] : 0; }

/* Mitte des naechsten Sprungpads: SCA-Zelle mit u1 == 0x10 auf dem Band des Gorillas (dieselbe Zellmenge, die
 * FUN_8003b93c(attr 0x10) in A[4] @0x80117ee0-f00 abfragt). 0 = kein Pad im Raum. */
static int r36_pad_naechstes(const re15_actor_t *e, int32_t *cx, int32_t *cz)
{
    if (!g_room_rdt_ok || !g_room_rdt.sca) return 0;
    long long best = -1;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if ((int)(c->floor >> 4) != (int)e->floor || c->u1 != 0x10u) continue;
        int32_t mx = (int32_t)c->x + (int32_t)c->width / 2, mz = (int32_t)c->z + (int32_t)c->density / 2;
        long long dx = mx - e->x, dz = mz - e->z, d2 = dx * dx + dz * dz;
        if (best < 0 || d2 < best) { best = d2; *cx = mx; *cz = mz; }
    }
    return best >= 0;
}

/* Nur aus A[3] (sub == 3) gehakt. Rueckgabe 1 = der 0x10-Sprung des Originals wurde ausgeloest. */
int re15_r36_affen_padsuche_decide(re15_actor_t *e, int slot, int sub)
{
    if (!s_r36_padsuche_an || !e || slot < 0 || slot >= RE15_ACTOR_MAX || sub != 3) return 0;
    if (e->dog_flags & 1) { s_r36_padsuche_aktiv[slot] = 0; return 0; }          /* Sicht -> Original-Jagd */
    if (!s_r36_padsuche_aktiv[slot]) {
        if (e->sca_wall_hit == 0) return 0;                                       /* nur blind UND an der Klemme (+0x1d6) */
        int32_t cx, cz;
        if (!r36_pad_naechstes(e, &cx, &cz)) return 0;                            /* Raum ohne Pads: Original */
        s_r36_padsuche_aktiv[slot] = 1;
    }
    uint8_t zc = 0;                                                               /* auf dem Pad? (wie A[4] @0x80117ee0-f00) */
    if (re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x10u, &zc)) {
        int win = ((((int)(zc & 0xf0u)) << 4) - (int)e->rot_y + 512) & 0xfff;   /* Yaw-Fenster des Originals @0x80117f18-38 */
        if (win < 1024) {                                                         /* der 0x10-Sprung des Originals (A[4] @0x80117f40-74) */
            s_r36_padsuche_aktiv[slot] = 0;
            e->dog_aux9f = (int8_t)zc;                                            /* +0x9f = +0x90 @0x80117f40 */
            e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 1;          /* +0x5 = 7, +0x6 = 0, +0x7 = 1 @0x80117f50-74 */
            return 1;
        }
        s_r36_padsuche_dir[slot] = (int16_t)(((int)(zc & 0xf0u)) << 4);          /* auf dem Pad, falsch gedreht: in Pad-Richtung drehen */
        s_r36_padsuche_aktiv[slot] = 2;
        return 0;
    }
    s_r36_padsuche_aktiv[slot] = 1;
    return 0;
}

int re15_r36_affen_padsuche_ziel(const re15_actor_t *e, int slot, int sub, int32_t plx, int32_t plz, int32_t *tx, int32_t *tz)
{
    *tx = plx; *tz = plz;
    if (!s_r36_padsuche_an || !e || slot < 0 || slot >= RE15_ACTOR_MAX || sub != 3 || !s_r36_padsuche_aktiv[slot]) return 0;
    if (s_r36_padsuche_aktiv[slot] == 2) {                                        /* Punkt in Pad-Richtung (Vorwaerts = (cos, -sin) wie FUN_800152C8) */
        int d = s_r36_padsuche_dir[slot] & 0xfff;
        *tx = e->x + (int32_t)(((int64_t)re15_cos_q12(d) * 3000) >> 12);
        *tz = e->z - (int32_t)(((int64_t)re15_sin_q12(d) * 3000) >> 12);
        return 1;
    }
    int32_t cx, cz;
    if (!r36_pad_naechstes(e, &cx, &cz)) return 0;
    *tx = cx; *tz = cz;
    return 1;
}
