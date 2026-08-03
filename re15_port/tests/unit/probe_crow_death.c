/* probe_crow_death.c — REGRESSION (ctest): Nutzer-Reports 2026-08-03:
 * (a) abgeschossene Kraehen ANIMIEREN im Port weiter, wenn sie tot am Boden liegen
 *     (Original: state 7 hat KEINEN f314-Call -> Pose friert am FINISH-Ende ein,
 *     Freeze-Frame 12 des Land-Clips 0x0a; crow_death_pool.md §1.2/1.3),
 * (b) die Blutlache unter der abgeschossenen Kraehe fehlt (Original: Corpse-Handler
 *     0x80115830 faerbt den Boden-Schatten um (0x00ffff38 @0x80115880-c8) und waechst
 *     +0xbc/+0xbe um +10/Tick fuer 51 Ticks @0x8011589c-ac/@0x8011585c-60).
 * Messung: ROOM1170 Re-Entry-Roster, naechste Kraehe erschiessen —
 *   Lauf A: weapon 3 (Normal-Lane 0x80114738) -> state 7 sub 0, frame==12 eingefroren,
 *           Lache waechst Basis+510 (51 Ticks x +10), danach konstant, crow_pool=1;
 *   Lauf B: weapon 7 (GIB-Lane 0x801149c4)   -> state 7 sub 1 (Pool-WIPE): Schatten 1x1,
 *           kein Pool, Koerper verborgen (crow_hide), flags|=0x42 (@0x80114acc/ae8). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_damage.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static void run_death(const re15_rdt_t *rdt, int weapon)
{
    scd_vm_init();
    g_current_room_id = 0x1170;
    re15_game_flag_set(3, 125, 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 530; pl->y = -7200; pl->z = -5150; pl->hp = 100;
    pl->rot_y = 0; pl->hit_react = 0;
    memcpy((void *)&g_room_rdt, rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(4);
    scd_register_room_events(rdt);
    scd_room_reenter((re15_rdt_t *)rdt, 0, 0, 0);

    for (int f = 0; f < 60; f++) { scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

    /* naechste State-1-Kraehe suchen und erschiessen */
    int tgt = -1; uint32_t bd = 0xffffffffu;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active || e->type != 0x21 || e->state != 1) continue;
        int32_t dx = e->x - pl->x, dz = e->z - pl->z;
        uint32_t d2 = (uint32_t)(dx * dx + dz * dz);
        if (d2 < bd) { bd = d2; tgt = i; }
    }
    if (tgt < 0) { printf("== weapon %d: keine Kraehe gefunden\n", weapon); return; }
    re15_actor_t *e = &g_actors[tgt];
    pl->rot_y = (int16_t)((re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);
    int hit = re15_player_weapon_fire(weapon);
    printf("== weapon %d: fire=%d slot=%d -> hp=%d st=%d ss1=%d ss3=%d\n",
           weapon, hit, tgt, (int)e->hp, e->state, e->sub_state_1, e->sub_state_3);

    /* Death-Lauf bis state 7 (max 300 Ticks) */
    int dt = 0;
    while (e->state == 3 && dt < 300) {
        scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance(); dt++;
    }
    printf("   state 7 nach %d Ticks: ss1=%d ss2=%d ss3=%d motion=%d frame=%d y=%ld timer=%d shadow=%u/%u\n",
           dt, e->sub_state_1, e->sub_state_2, e->sub_state_3, e->motion,
           e->anim_frame, (long)e->y, e->crow_timer, e->crow_shadow_w, e->crow_shadow_h);
    CHECK(e->state == 7, "(w%d) state 7 erreicht, got %d", weapon, e->state);
    uint16_t base_w = e->crow_shadow_w;   /* letzter ACTIVE-Tail-Wert = Lachen-Basis (§1.4) */

    /* 300 Corpse-Ticks: motion/anim_frame-Verlauf. Original: state 7 enthaelt KEINEN
     * f314-Call -> frame bleibt konstant (eingefroren am FINISH-Ende, Frame 12). */
    int changes = 0, minf = 9999, maxf = -1; int f_prev = e->anim_frame;
    for (int f = 0; f < 300; f++) {
        scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        if (e->anim_frame != f_prev) { changes++; f_prev = e->anim_frame; }
        if (e->anim_frame < minf) minf = e->anim_frame;
        if (e->anim_frame > maxf) maxf = e->anim_frame;
        if (f < 4 || (f % 50) == 0)
            printf("   corpse t=%3d: st=%d ss1=%d ss2=%d motion=%d frame=%d shadow=%u/%u pool=%d\n",
                   f, e->state, e->sub_state_1, e->sub_state_2, e->motion, e->anim_frame,
                   e->crow_shadow_w, e->crow_shadow_h, e->crow_pool);
    }
    printf("   nach 300 Corpse-Ticks: frame-CHANGES=%d, frame-Spanne %d..%d, motion=%d ss1=%d shadow=%u/%u pool=%d hide=%d\n",
           changes, minf, maxf, e->motion, e->sub_state_1,
           e->crow_shadow_w, e->crow_shadow_h, e->crow_pool, e->crow_hide);

    /* Erwartungen aus dem Original (crow_death_pool.md §3-Verifikation) */
    CHECK(changes == 0, "(w%d) Corpse friert ein — KEIN f314 in state 7, got %d frame-CHANGES",
          weapon, changes);
    if (weapon == 3) {
        CHECK(e->sub_state_1 == 0, "(w3) Normal-Lane endet sub 0 (a0=0 @0x80114964), got %d",
              e->sub_state_1);
        CHECK(e->motion == 0x0a && e->anim_frame == 12,
              "(w3) Freeze auf Land-Clip 0x0a Frame 12 (FINISH-Bilanz §1.2), got motion=%d frame=%d",
              e->motion, e->anim_frame);
        CHECK(e->rot_z == 0, "(w3) rot_z=0 beim Land (@0x801148f8), got %d", e->rot_z);
        CHECK(e->crow_pool == 1, "(w3) Lache aktiv (0x00ffff38 @0x80115880-c8)");
        CHECK(e->crow_shadow_w == (uint16_t)(base_w + 510) &&
              e->crow_shadow_h == (uint16_t)(base_w + 510),
              "(w3) Lache = Basis(%u)+510 (51 Ticks x +10 @0x8011589c-ac), got %u/%u",
              base_w, e->crow_shadow_w, e->crow_shadow_h);
        CHECK((e->flags & 0x42) == 0x42,
              "(w3) word0-Bits 0x2|0x40 (@0x80114990/@0x801149ac), got flags=%02x", e->flags);
        CHECK(e->crow_hide == 0, "(w3) Normal-Leiche bleibt sichtbar");
    } else {
        CHECK(e->sub_state_1 == 1, "(w%d) GIB endet sub 1 = Pool-WIPE (a0=1 @0x80114b7c), got %d",
              weapon, e->sub_state_1);
        CHECK(e->crow_shadow_w == 1 && e->crow_shadow_h == 1,
              "(w%d) Wipe: Schatten 1x1 (@0x80115938-3c), got %u/%u",
              weapon, e->crow_shadow_w, e->crow_shadow_h);
        CHECK(e->crow_pool == 0, "(w%d) GIB-Leiche hat KEINE Lache (@0x80115940-54)", weapon);
        CHECK(e->crow_hide == 1, "(w%d) Koerper verborgen (Scatter @0x80114a50-aa4/@0x80114b78)",
              weapon);
        CHECK((e->flags & 0x42) == 0x42,
              "(w%d) word0-Bits 0x2|0x40 (@0x80114acc/@0x80114ae8), got flags=%02x",
              weapon, e->flags);
    }
    g_room_rdt_ok = 0; re15_collision_reset_band();
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1170.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    re15_actor_init();
    run_death(&rdt, 3);   /* Normal-Lane (Original: state 7 sub 0 -> Blutlache) */
    re15_actor_init();
    run_death(&rdt, 7);   /* GIB-Lane (Original: state 7 sub 1 -> Pool-WIPE, keine Lache) */
    free(data);
    if (fails) { printf("CROW DEATH: %d FAIL\n", fails); return 1; }
    printf("CROW DEATH: all checks passed\n");
    return 0;
}
