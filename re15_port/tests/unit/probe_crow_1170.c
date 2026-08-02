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
    free(data);
    return 0;
}
