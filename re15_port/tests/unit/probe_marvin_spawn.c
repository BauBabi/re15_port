/* probe_marvin_spawn.c — REGRESSION (ctest): Nutzer-Report #3 2026-08-03:
 * "Marvins SPAWN + START-ANIMATION in ROOM10D0 sind immer noch falsch."
 *
 * ORIGINAL-GROUND-TRUTH (DuckStation-Savestate r10d0_walk1.sav, PRE-Cutscene):
 *   enemy[0] @0x800acc2c: type=0x40 beh=0x40, word+0x4=0x00030604
 *     -> state 4 / sub 6 / phase 3, +0x94=2 +0x95=39, +0x1c4=0x20, yaw 1142,
 *     +0x170=0x8013d040 +0x174=0x8013cc58 = CDEMD0.EMS blob18 dir[4]/dir[3]
 *     (= BANK 1: 14 Clips {22,16,52,1,50,30,10,16,1,17,1,17,1,1}),
 *     +0x84=0x801359e8 +0x16c=0x801353d4 = dir[2]/dir[1] (= BANK 0: 24 Clips).
 *   Kanal-Loader FUN_80022300: +0x16c=dir[1] @0x80022498, +0x84=dir[2] @0x800224a8,
 *     +0x174=dir[3] @0x800224b8, +0x170=dir[4] @0x800224c8 — BY POSITION, keine Groesse.
 *   INIT FUN_8011c6dc: +0x94=2 @0x8011c7bc-c0, f314(+0x84,+0x16c,0,0x200) @0x8011c800-08,
 *     beh&0x40 -> state=4 @0x8011c860, +0x5=6.
 *   Sub 6 @0x800517f0: Phase 0 Clip 1 (@0x80051844-74), f314(+0x170,+0x174) @0x80051884-90,
 *     Phase 2 Clip 2 (@0x800518b4-d8), Phase-3-Loop f314 @0x800518e4-f4.
 *   => SOLL: Clip 1 der BANK 1 (16 Frames) einmal, dann Clip 2 der BANK 1 (52 Frames) Loop.
 *
 * PORT-HYPOTHESE: re15_emd_parse_container waehlt die Bank mit den MEISTEN Clips
 * (bank0 = dir[1], 24 Clips) als eb->anim -> der Executor-Kanal +0x170/+0x174 spielt
 * fuer EM040 die FALSCHE Bank (Clip 1 = 32 statt 16 Frames, Clip 2 = 50 statt 52;
 * andere Posen). Messbar am Frame-Wrap. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_room.h"

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

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10D0.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x10D0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 7650; pl->y = 0; pl->z = 11400; pl->rot_y = 2048;   /* Tuer-Spawn, WEIT weg von der AOT-Zone */
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    /* Wie live: EM040 aus CDEMD0.EMS laden (pc_enemy_load-Aequivalent) + RBJ registrieren */
    {
        char emsp[600]; size_t ems_size = 0;
        snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
        uint8_t *ems = read_file(emsp, &ems_size);
        if (ems) {
            int idx = re15_ems_index_for_type(0x40);
            size_t off = 0, len = 0;
            if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0) {
                re15_enemy_bank_t *eb = re15_enemy_alloc(0x40);
                if (eb) {
                    static uint8_t s_em40[0x60000];
                    if (len <= sizeof s_em40) {
                        memcpy(s_em40, ems + off, len);
                        re15_tim_t tim = (re15_tim_t){0};
                        if (re15_emd_parse_container(s_em40, len, &eb->md1, &eb->skel,
                                                     &eb->anim, &tim) == 0) {
                            eb->ok = 1; eb->buf = NULL;
                            eb->loco_ok = (re15_emd_parse_loco_bank(s_em40, len,
                                              &eb->skel_loco, &eb->anim_loco) == 0);
                            eb->own_ok = (re15_emd_parse_own_bank(s_em40, len,
                                              &eb->skel_own, &eb->anim_own) == 0);
                            printf("EM040 eb->anim: %d clips, fc[0..9]=", eb->anim.clip_count);
                            for (int c = 0; c < 10 && c < eb->anim.clip_count; c++)
                                printf("%d ", eb->anim.clips[c].frame_count);
                            printf("\n  eigene BANK 1 (own_ok=%d): %d clips, fc[0..9]=",
                                   eb->own_ok, eb->own_ok ? eb->anim_own.clip_count : 0);
                            for (int c = 0; eb->own_ok && c < 10 && c < eb->anim_own.clip_count; c++)
                                printf("%d ", eb->anim_own.clips[c].frame_count);
                            printf("\n  SOLL fuer den +0x170-Kanal (BANK1 dir[3] @0x2d5090): 22 16 52 1 50 30 10 16 1 17 (14 clips)\n");
                            /* Bank-1-Parser gegen die byte-dekodierten dir[3]-Laengen pruefen */
                            static const uint8_t soll[14] = {22,16,52,1,50,30,10,16,1,17,1,17,1,1};
                            CHECK(eb->own_ok && eb->anim_own.clip_count == 14,
                                  "own-Bank: 14 Clips (dir[3] @0x2d5090), got ok=%d n=%d",
                                  eb->own_ok, eb->own_ok ? eb->anim_own.clip_count : -1);
                            for (int c = 0; eb->own_ok && c < 14 && c < eb->anim_own.clip_count; c++)
                                CHECK(eb->anim_own.clips[c].frame_count == soll[c],
                                      "own-Bank Clip %d = %d Frames (dir[3]-EDD), got %d",
                                      c, soll[c], eb->anim_own.clips[c].frame_count);
                        }
                    }
                }
            }
            free(ems);
        }
        if (rdt.animation && rdt.animation_size > 0)
            re15_rbj_bind_room(rdt.animation, rdt.animation_size);
    }

    for (int f = 0; f < 8; f++) scd_vm_tick();
    printf("== Spawn-Roster nach sub00 ==\n");
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (a->active)
            printf("  slot=%d type=0x%02X grid=0x%02X pos=(%ld,%ld,%ld) rot=%d st=%u s1=%u mo=%d\n",
                   s, a->type, a->grid_id, (long)a->x, (long)a->y, (long)a->z,
                   (int)a->rot_y, a->state, a->sub_state_1, (int)a->motion);
    }

    /* KEIN sub21-Start: reine Spawn-/Idle-Phase, wie beim Betreten fern der AOT-Zone.
     * SOLL (Original): st=4 s1=6, Clip 1 (16f, Bank1) einmal -> Clip 2 (52f, Bank1) Loop.
     * Frame-Wrap verraet die Bank: Wrap bei 16/52 = BANK1 (richtig), 32/50 = BANK0 (falsch). */
    const re15_actor_t *mv = &g_actors[1];
    int last_mo = -1, last_fr = -1;
    int wrap_seen[8]; int nwrap = 0;
    for (int tick = 0; tick < 260; tick++) {
        scd_vm_tick();
        re15_actor_step_all_walkers();
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);
        if ((int)mv->motion != last_mo)
            printf("%5d MARVIN st=%u s1=%u s2=%u mo=%d fr=%u af=0x%04x rot=%d\n",
                   tick, mv->state, mv->sub_state_1, mv->sub_state_2,
                   (int)mv->motion, mv->anim_frame, mv->anim_flags, (int)mv->rot_y);
        if (last_fr >= 0 && (int)mv->anim_frame < last_fr && (int)mv->motion == last_mo) {
            printf("%5d MARVIN WRAP: mo=%d frame %d -> %u  (Cliplaenge im Port = %d)\n",
                   tick, (int)mv->motion, last_fr, mv->anim_frame, last_fr + 1);
            if (nwrap < 8) wrap_seen[nwrap++] = last_fr + 1;
        }
        last_mo = (int)mv->motion; last_fr = (int)mv->anim_frame;
    }
    printf("\n== BEFUND ==\n");
    printf("Ende: st=%u s1=%u s2=%u mo=%d fr=%u\n", mv->state, mv->sub_state_1,
           mv->sub_state_2, (int)mv->motion, mv->anim_frame);
    printf("Wraps: "); for (int i = 0; i < nwrap; i++) printf("%d ", wrap_seen[i]); printf("\n");
    printf("SOLL (Savestate r10d0_walk1): Clip1 16f einmal -> Clip2 52f Loop (BANK1);\n");
    printf("FALSCH waere: Clip1 32f / Clip2 50f (BANK0 via parse_container 'largest bank').\n");

    /* SOLL aus dem Original (Savestate r10d0_walk1 + Sub-6-Disasm @0x800517f0):
     * st=4 s1=6, Clip-2-Loop; jeder beobachtete Wrap ist eine BANK-1-Laenge (Clip 2 = 52f;
     * ein frueher Clip-1-Wrap waere 16f). BANK-0-Wraps (32/50) = der Bug. */
    CHECK(mv->active && mv->type == 0x40, "Marvin slot 1 aktiv (Sce_em_set), got type=0x%02x", mv->type);
    CHECK(mv->state == 4 && mv->sub_state_1 == 6,
          "Ende in state 4 / sub 6 (EVENT-REACH @0x800517f0), got st=%u s1=%u",
          mv->state, mv->sub_state_1);
    CHECK(mv->motion == 2, "Idle-Loop auf Clip 2 (@0x800518b4-d8), got mo=%d", (int)mv->motion);
    CHECK(nwrap > 0, "mindestens ein Frame-Wrap in 260 Ticks (Clip 2 = 52f)");
    for (int i = 0; i < nwrap; i++)
        CHECK(wrap_seen[i] == 52 || wrap_seen[i] == 16,
              "Wrap %d = BANK-1-Laenge (16/52), got %d (32/50 = BANK 0 = largest-bank-Bug)",
              i, wrap_seen[i]);

    free(data);
    if (fails) { printf("MARVIN SPAWN: %d FAIL\n", fails); return 1; }
    printf("MARVIN SPAWN: all checks passed\n");
    return 0;
}
