/* probe_crow_1170.c — DIAGNOSE (kein ctest): Nutzer-Report 2026-08-02 nach dem D1-Fix:
 * "KEINE Kraehe greift Leon mehr an — alle kreisen nur."
 * Misst am echten ROOM1170.RDT: (1) welche grid-Werte die gespawnten Kraehen tragen,
 * (2) crow_armed je Slot, (3) ob eine armed-Kraehe je einen Attack-State (10/11/12/15/16)
 * erreicht, wenn der Spieler auf dem Pad steht. Beide Spawn-Zweige (z3/125 clear/set). */
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
#include "re15_collision.h"   /* re15_collision_set_band/reset_band — LIVE setzt Band 4 (room_common.c:218) */
#include "re15_damage.h"      /* re15_player_weapon_fire — Schuss-Reaktions-Probe */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

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

static void run_variant(const re15_rdt_t *rdt, int reentry_flag)
{
    scd_vm_init();
    g_current_room_id = 0x1170;
    if (reentry_flag) re15_game_flag_set(3, 125, 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Spieler mitten auf dem Helipad (Intro-Endposition laut HANDOVER: (2664,-7200,-7336)) */
    pl->active = 1; pl->type = 0; pl->x = 2664; pl->y = -7200; pl->z = -7336; pl->hp = 100;
    pl->hit_react = 0;
    scd_register_room_events(rdt);
    scd_room_reenter((re15_rdt_t *)rdt, 0, 0, 0);

    for (int f = 0; f < 60; f++) { scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

    printf("== z3/125=%d: Kraehen nach 60 Ticks ==\n", reentry_flag);
    int crows = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active || e->type != 0x21) continue;
        crows++;
        printf("  slot %2d: grid=0x%02x armed=%d st=%d ss1=%2d pos=(%ld,%ld,%ld)\n",
               i, e->grid_id, e->crow_armed, e->state, e->sub_state_1,
               (long)e->x, (long)e->y, (long)e->z);
    }
    printf("  Kraehen gesamt: %d\n", crows);
    if (crows == 0) return;

    /* Langlauf: erreicht je eine Kraehe einen Attack-State? Spieler bleibt auf dem Pad,
     * hit_react wird jeden Tick geloescht (der Original-cmd-0-Prolog @0x80031964 tut das). */
    uint32_t seen_ss1[RE15_ACTOR_MAX] = {0};
    int attack_ticks = 0;
    for (int f = 0; f < 3600; f++) {
        pl->hit_react = 0;
        scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            re15_actor_t *e = &g_actors[i];
            if (!e->active || e->type != 0x21 || e->state != 1) continue;
            if (e->sub_state_1 < 32) seen_ss1[i] |= (1u << e->sub_state_1);
            if (e->sub_state_1 == 10 || e->sub_state_1 == 11 || e->sub_state_1 == 12 ||
                e->sub_state_1 == 15 || e->sub_state_1 == 16) attack_ticks++;
        }
    }
    printf("  nach 3600 Ticks (Spieler auf dem Pad, hp=%d):\n", pl->hp);
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active || e->type != 0x21) continue;
        printf("  slot %2d: grid=0x%02x armed=%d ss1-Bitmask=0x%08lx  (Attack-Bits 10/11/12/15/16 = 0x%08lx)\n",
               i, e->grid_id, e->crow_armed, (unsigned long)seen_ss1[i],
               (unsigned long)(seen_ss1[i] & ((1u<<10)|(1u<<11)|(1u<<12)|(1u<<15)|(1u<<16))));
    }
    printf("  Attack-State-Ticks gesamt: %d\n", attack_ticks);
}

/* ===== LIVE-Bedingungs-Variante (crow_shot_attack.md, 2026-08-03) =====================
 * Der D12-Lauf war GRUEN, live greifen die Kraehen trotzdem nie an. Unterschied: live sind
 * g_room_rdt_ok=1 UND s_coll_band gesetzt (room_common.c:218: band_from_y(-7200) = 4) —
 * dann laeuft im LOS-Sensor re15_enemy_los_probe der re15_collision_on_floor-Ray-Stand-in,
 * der auf dem offenen Pad IMMER 0 liefert (1170-Zellen sind Wand-Style; der Spieler laeuft
 * im band-4-FREIEN Komplement, re15_collision.c:13-23) -> s_los_blocked=1 -> parity nie 1
 * -> steer[9]-Commit (@0x8011330c-70: +0x1d2 && +0x1db && dist<10000 && +0x1d6<3) nie.
 * Original: der Ray FUN_8003dcc4 filtert Zellen auf (w5&0xf00)==0x300 UND w5>>12 ==
 * enemy+0x82; die fliegende Kraehe hat +0x82=-(y/1800)=5..7, ROOM1170-Zellen sind max.
 * Band 4 -> KEIN Blocker -> LOS = reiner FOV-Test -> Commit feuert. */
static void run_live_variant(const re15_rdt_t *rdt, int with_band)
{
    scd_vm_init();
    g_current_room_id = 0x1170;
    re15_game_flag_set(3, 125, 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Spieler NAH an der armed Pad-Kraehe (Spawn 5: grid 0x00, pos 530,-7200,-6650) */
    pl->active = 1; pl->type = 0; pl->x = 530; pl->y = -7200; pl->z = -5150; pl->hp = 100;
    pl->rot_y = 0; pl->hit_react = 0;
    memcpy((void *)&g_room_rdt, rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    if (with_band) re15_collision_set_band(4);   /* wie live: band_from_y(-7200)=4 */
    else           re15_collision_reset_band();  /* wie der alte D12-Lauf (maskiert) */
    scd_register_room_events(rdt);
    scd_room_reenter((re15_rdt_t *)rdt, 0, 0, 0);

    long parity1 = 0, commit_ticks = 0; uint32_t seen = 0;
    int armed_slot = -1;
    for (int f = 0; f < 2400; f++) {
        pl->hit_react = 0;
        scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            re15_actor_t *e = &g_actors[i];
            if (!e->active || e->type != 0x21 || (e->grid_id & 0x10)) continue;  /* nur armed (grid 0x00) */
            if (armed_slot < 0) armed_slot = i;
            if (i != armed_slot) continue;
            parity1 += e->crow_parity;
            if (e->state == 1 && e->sub_state_1 < 32) seen |= (1u << e->sub_state_1);
            if (e->state == 1 && (e->sub_state_1 == 10 || e->sub_state_1 == 11 ||
                e->sub_state_1 == 12 || e->sub_state_1 == 15 || e->sub_state_1 == 16))
                commit_ticks++;
        }
    }
    re15_actor_t *ac = (armed_slot > 0) ? &g_actors[armed_slot] : NULL;
    printf("== LIVE-Variante band=%s: armed slot %d nach 2400 Ticks:\n",
           with_band ? "4 (wie live)" : "unset (wie D12-Lauf)", armed_slot);
    if (ac)
        printf("   parity1-Ticks=%ld  Attack-Ticks=%ld  ss1-Maske=0x%08lx  ss1=%d dist=%u y=%ld hp=%d player-hp=%d\n",
               parity1, commit_ticks, (unsigned long)seen, ac->sub_state_1,
               (unsigned)ac->crow_dist, (long)ac->y, (int)ac->hp, (int)pl->hp);

    /* --- Schuss-Reaktions-Probe: naechste Kraehe anvisieren, weapon 3 (Gun-Strip, dmg-Row
     * 0x21 = 2 @0x8006ec28) -> Original-Erwartung: hp 0-2 = -2 < 0 -> +0x4=3 DEATH
     * (@0x80012520), Lane = +0x5 = weapon_id (@0x800124bc, Dispatch @0x80114700-24). */
    int tgt = -1; uint32_t bd = 0xffffffffu;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active || e->type != 0x21 || e->state != 1) continue;
        int32_t dx = e->x - pl->x, dz = e->z - pl->z;
        uint32_t d2 = (uint32_t)(dx * dx + dz * dz);
        if (d2 < bd) { bd = d2; tgt = i; }
    }
    if (tgt > 0) {
        re15_actor_t *e = &g_actors[tgt];
        pl->rot_y = (int16_t)((re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);
        printf("   Schuss auf slot %d (hp=%d vert=%ld): ", tgt, (int)e->hp, (long)(pl->y - e->y));
        int hit = re15_player_weapon_fire(3);
        printf("fire(3)=%d -> hp=%d st=%d ss1=%d ss3=%d\n", hit, (int)e->hp, e->state,
               e->sub_state_1, e->sub_state_3);
        for (int f = 0; f < 300 && e->state == 3; f++) {
            scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        }
        printf("   nach Death-Lauf: st=%d ss1=%d ss3=%d y=%ld motion=%d hp=%d\n",
               e->state, e->sub_state_1, e->sub_state_3, (long)e->y, e->motion, (int)e->hp);
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
    run_variant(&rdt, 0);
    run_variant(&rdt, 1);
    re15_actor_init();
    run_live_variant(&rdt, 1);   /* LIVE-Bedingungen: Band 4 -> LOS-Stand-in blockt */
    re15_actor_init();
    run_live_variant(&rdt, 0);   /* Gegenprobe: Band unset -> Stand-in maskiert (D12-Gruen) */
    free(data);
    return 0;
}
