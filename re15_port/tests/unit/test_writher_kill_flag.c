/* test_writher_kill_flag.c — PIN: der WRITHER (Typ 0x1A) darf sein em-Status-Kill-Flag
 * NICHT persistieren; jeder Typ, der es im Original tut, muss es weiter tun.
 *
 * ⛔ NUTZER-BEFUND 2026-08-27: "im ROOM1220 sollten die Zombies eigentlich kriechen.
 * Auch bei RE2-AI."
 *
 * Die Kriech-Kette selbst war in Ordnung (v0.3.23). Der Raum war LEER: ROOM1220s beide
 * Kriecher tragen die em-Status-Indizes 0x8A/0x8B — und dieselben Indizes tragen die
 * WRITHER im Vorraum ROOM1210. Byte-Beleg (eigener Dump der RDTs):
 *     ROOM1220.RDT @0x0F2A  44 00 16 81 00 00 00 8a   slot0 Typ 0x16 beh 0x81 Flag 0x8A
 *     ROOM1220.RDT @0x0F3E  44 01 16 81 00 00 01 8b   slot1 Typ 0x16 beh 0x81 Flag 0x8B
 *     ROOM1210.RDT @0x1D86  44 00 1a 00 00 00 00 8a   slot0 Typ 0x1A beh 0x00 Flag 0x8A
 *     ROOM1210.RDT @0x1D96  44 01 1a 00 00 00 01 8b   slot1 Typ 0x1A beh 0x00 Flag 0x8B
 * Der Port persistierte den Tod JEDES Aktors, der CORPSE erreicht — also auch den des
 * Writhers. Wer in ROOM1210 aufraeumte, loeschte damit ROOM1220s Kriecher.
 *
 * DAS ORIGINAL TUT DAS NICHT. Der Flag-Setzer ist `jal 0x8004ef90` (Wort 0x0C013BE4).
 * Eigener Byte-Scan der GANZEN info/Re1.5/PSX/BIN/STAGE1.BIN: 17 Treffer —
 *   Zombie-Familie 0x80106E0C 0x8010716C 0x801074B0 0x801076EC 0x80107DC4
 *                  0x801082A4 0x801082C4 0x80109100 0x801096FC
 *   Hund           0x80110FBC 0x801111D4
 *   Kraehe         0x801146EC 0x801152C4
 *   Typ 0x26       0x80116AC8
 *   Gorilla 0x27   0x8011B8F4 0x8011BAFC 0x8011BD24
 * und KEIN EINZIGER im Writher-Bereich 0x8010C1EC..0x8010D770. Der Bereich ist ueber die
 * Zustandstabelle @0x8012093C sauber abgegrenzt (selbst gedumpt):
 *   [0] 0x8010C33C INIT  [1] 0x8010C488 ACTIVE  [2] 0x8010D0F8 HURT  [3] 0x8010D474 DEATH
 *   [4] 0x8010D768        [5] 0            [6] 0                 [7] 0x8010D770 CORPSE
 * Sein Todes-Commit schreibt nur `sw v0,4(a0)` @0x8010d5bc — kein Flag-Setzer.
 *
 * DIE WACHE IST NICHT VAKUANT: sie prueft BEIDE Richtungen an echten Raumdaten —
 *   (1) die Flag-KOLLISION muss in den ausgelieferten RDTs wirklich existieren, sonst
 *       misst der Test ein Phantom;
 *   (2) ein Writher, der CORPSE erreicht, darf sein Flag NICHT setzen;
 *   (3) ein ZOMBIE, der CORPSE erreicht, MUSS sein Flag setzen (Positiv-Kontrolle — sonst
 *       koennte man den Test bestehen, indem man die Persistenz ganz abschaltet).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* Raum laden + hochfahren; liefert den RDT-Puffer (Aufrufer gibt frei) oder NULL. */
static uint8_t *enter(unsigned rid, uint8_t entry_cut, int32_t px, int32_t pz)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM%04X.RDT", rid);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) return NULL;
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { free(buf); return NULL; }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = rid;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz;
    re15_collision_set_band(0);
    re15_inv_load_briefing();

    scd_room_reenter(&s_rdt, pl->x, pl->z, entry_cut);
    return buf;
}

/* Aktor mit (Typ, Flag) suchen, in den Todesast schicken, N Frames ticken.
 * Liefert 1, wenn er CORPSE (state 7) erreicht hat. */
static int kill_actor(uint8_t type, uint8_t flag, int frames)
{
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == type && g_actors[s].em_flag_id == flag)
        { slot = s; break; }
    if (slot < 0) return -1;
    g_actors[slot].hp = -1;
    g_actors[slot].state = 3;                 /* DEATH — der Commit setzt +0x4 = 7 */
    g_actors[slot].sub_state_3 = 0;
    for (int f = 0; f < frames; f++) frame_step();
    return (g_actors[slot].state == 7) ? 1 : 0;
}

/* (Typ, Flag)-Paare eines Raums einsammeln. */
static int collect(unsigned rid, uint8_t entry_cut, int32_t px, int32_t pz,
                   uint8_t *types, uint8_t *flags, int max)
{
    uint8_t *buf = enter(rid, entry_cut, px, pz);
    if (!buf) return -1;
    for (int f = 0; f < 4; f++) frame_step();
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && n < max; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->em_flag_id == 0xFF) continue;
        types[n] = e->type; flags[n] = e->em_flag_id; n++;
    }
    free(buf);
    return n;
}

int main(void)
{
    printf("=== WRITHER-Kill-Flag: kein `jal 0x8004ef90` in 0x8010C1EC..0x8010D770 ===\n");

    /* --- (1) Die Kollision muss in den echten Raumdaten existieren --------------------- */
    uint8_t t1210[32], f1210[32], t1220[32], f1220[32];
    int n1210 = collect(0x1210, 0, -22000, -6000, t1210, f1210, 32);
    int n1220 = collect(0x1220, 0, -22400, -6500, t1220, f1220, 32);
    printf("  ROOM1210: %d Aktoren mit em-Flag | ROOM1220 (Cut 0): %d\n", n1210, n1220);

    int collisions = 0, writhers = 0;
    for (int a = 0; a < n1210; a++) {
        if (t1210[a] == 0x1A) writhers++;
        for (int b = 0; b < n1220; b++)
            if (f1210[a] == f1220[b] && t1210[a] == 0x1A && t1220[b] != 0x1A) {
                printf("    Kollision: Flag 0x%02X — ROOM1210 Typ 0x%02X vs ROOM1220 Typ 0x%02X\n",
                       f1210[a], t1210[a], t1220[b]);
                collisions++;
            }
    }
    CHECK(writhers > 0,
          "ROOM1210 liefert %d Writher (Typ 0x1A) — ohne sie misst die Wache nichts", writhers);
    CHECK(collisions > 0,
          "%d Flag-Kollisionen Writher<->ROOM1220 gefunden (ROOM1210.RDT @0x1D86/@0x1D96 vs "
          "ROOM1220.RDT @0x0F2A/@0x0F3E, beide 0x8A/0x8B) — ohne Kollision waere der Fehler "
          "gar nicht ausloesbar und der Test ein Phantom", collisions);

    /* --- (2) Writher-Tod darf NICHT persistieren --------------------------------------- */
    {   uint8_t *buf = enter(0x1210, 0, -22000, -6000);
        if (!buf) { printf("FAIL: ROOM1210.RDT nicht ladbar\n"); return 1; }
        for (int f = 0; f < 4; f++) frame_step();
        int before = re15_game_flag_get(7, 0x8A);
        int died   = kill_actor(0x1A, 0x8A, 240);
        int after  = re15_game_flag_get(7, 0x8A);
        printf("  Writher 0x8A: vorher=%d  CORPSE erreicht=%d  nachher=%d\n",
               before, died, after);
        CHECK(died == 1,
              "der Writher muss CORPSE erreichen (Commit `sw v0,4(a0)` @0x8010d5bc), "
              "gemessen died=%d — sonst prueft (2) nichts", died);
        CHECK(before == 0 && after == 0,
              "Flag(7,0x8A) %d -> %d: der Writher-Tod darf NICHT persistieren "
              "(kein `jal 0x8004ef90` in 0x8010C1EC..0x8010D770, Zustandstabelle @0x8012093C)",
              before, after);
        free(buf);
    }

    /* --- (3) Positiv-Kontrolle: der ZOMBIE persistiert weiterhin ------------------------ */
    {   uint8_t *buf = enter(0x1220, 0, -22400, -6500);
        if (!buf) { printf("FAIL: ROOM1220.RDT nicht ladbar\n"); return 1; }
        for (int f = 0; f < 4; f++) frame_step();
        int before = re15_game_flag_get(7, 0x8A);
        int died   = kill_actor(0x16, 0x8A, 240);
        int after  = re15_game_flag_get(7, 0x8A);
        printf("  Zombie 0x16 Flag 0x8A: vorher=%d  CORPSE erreicht=%d  nachher=%d\n",
               before, died, after);
        CHECK(died == 1,
              "der ROOM1220-Zombie muss CORPSE erreichen, gemessen died=%d — sonst prueft "
              "die Positiv-Kontrolle nichts", died);
        CHECK(before == 0 && after == 1,
              "Flag(7,0x8A) %d -> %d: der Zombie-Tod MUSS persistieren "
              "(`jal 0x8004ef90` @0x801096FC / @0x8010716C) — sonst waere die Persistenz "
              "insgesamt abgeschaltet statt typ-gebunden", before, after);
        free(buf);
    }

    if (fails) { printf("\nWRITHER KILL FLAG: FAIL (%d)\n", fails); return 1; }
    printf("\nWRITHER KILL FLAG: Persistenz ist typ-gebunden\n");
    return 0;
}
