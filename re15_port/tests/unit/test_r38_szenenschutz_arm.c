/**
 * @file test_r38_szenenschutz_arm.c
 * @brief Runde 38 Spur B "szenenschutz" — Gitterarm 0x1A (RE2-Zellenarm EM2D, ROOM1210/1211) im echten Raum.
 *
 * Nutzer (AUFTRAG.md 2026-10-16): "Das Leon schon in Cutscenes angegriffen werden kann, wo er sich noch nicht mal
 * bewegen kann ist mist." Zensus (Dossier 2.5): ROOM1211 (Elza) hat Szenen-Fenster (2,7)/(1,27) und zehn Gitterarme.
 * Der Arm laeuft in JEDEM Gehirn auf dem RE2-Zellenarm (re15_ai_re2_for_type(0x1A)); im synthetischen Zensus
 * (test_r38_szenenschutz_alle.c) greift er ohne Raum/Bank nicht an, darum hier der Aufbau aus test_p2_1210_arme_re2.c
 * (Raum, Bank, Weckruf Member_set(12,1), REACH, ZUGRIFF) bis zum Griff-Entscheid A3 @0x801009A0:
 *   @0x80100A3C-48 PL+0x1D3 & 0x80 gesetzt -> 0x601 (warten) statt 0x401 (HALTEN).
 * frei:     A3 -> 0x401, Leon gegriffen (Kontrolle).
 * gehalten: g_scd.player_mode = 2 -> die Szenen-Sicht (re15_player_is_grabbed) schliesst dieses Tor: kein 0x401,
 *           kein Griff; nach der Szene greift der Arm wieder.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_ai_flavor.h"
#include "re2_ems.h"
#include "re15_enemy_ai_re2_zellenarm.h"
#include "re15_szenenschutz_r38.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

extern int re15_player_victim_state(void);
extern int re15_player_is_grabbed(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t *s_ems = NULL; static size_t s_ems_n = 0;
static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static int bank_1A(void)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
    if (!eb) eb = re15_enemy_alloc(0x1A);
    if (!eb) return 0;
    re15_tim_t tim = {0};
    if (re2_ems_load_bank(s_ems, s_ems_n, 0x2D, eb, &tim) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return 1;
}
static void bild(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* Raum + Arme wie test_p2_1210_arme_re2.c aufsetzen, Arm 5 bis zum ZUGRIFF (0x301) fuehren. Rueckgabe Slot. */
static int bis_zugriff(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_szenenschutz_zaehler_reset();
    g_scd.player_mode = 0;
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    if (!bank_1A()) return -1;
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) bild();
    int slots[RE15_ACTOR_MAX], n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) slots[n++] = s;
    if (n < 5) return -1;
    int a5 = slots[4];
    re15_actor_t *arm = &g_actors[a5];
    int16_t hyaw; int32_t hx, hz; re15_re2arm_home(a5, &hyaw, &hx, &hz);
    pl->x = hx + 3500; pl->z = hz + 3500;               /* ausser 2500: A0 zieht noch nicht */
    for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;   /* Member_set(12,1) @0x1EDA */
    bild();
    for (int f = 0; f < 20 && arm->sub_state_2 != 2; f++) bild();
    pl->x = hx + 2000; pl->z = hz;                      /* A0 -> 0x101 REACH */
    bild();
    int32_t hand[3]; re15_enemy_bone_world_pos(arm, re15_re2arm_hand_bone(arm), hand);
    pl->x = hand[0] + 300; pl->z = hand[2];             /* < 900 -> A1 -> 0x301 */
    for (int f = 0; f < 30 && arm->sub_state_1 == 1; f++) bild();
    return (arm->state == 1 && arm->sub_state_1 == 3) ? a5 : -1;
}

int main(void)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: ROOM1210.RDT\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    int griff_bild[2] = { -1, -1 }, warte[2] = { 0, 0 };
    for (int halten = 0; halten < 2; halten++) {
        int a5 = bis_zugriff();
        CHECK(a5 > 0, "%s: Arm 5 im ZUGRIFF (0x301, B3 Clip 3)", halten ? "gehalten" : "frei");
        if (a5 <= 0) continue;
        re15_actor_t *arm = &g_actors[a5];
        g_scd.player_mode = halten ? 2 : 0;
        for (int f = 0; f < 60 && griff_bild[halten] < 0; f++) {
            bild();
            if (arm->sub_state_1 == 6) warte[halten]++;
            if (arm->sub_state_1 == 4 || re15_player_victim_state() != 0) griff_bild[halten] = f;
        }
        uint32_t ticks = 0, ab = 0; re15_szenenschutz_zaehler(&ticks, &ab);
        printf("  [arm] %s: Griff (0x401) bei Bild %d, Warte-Bilder (0x601) %d, hp %d, geschuetzte Ticks %u, Netz %u\n",
               halten ? "gehalten" : "frei", griff_bild[halten], warte[halten], pl->hp, ticks, ab);
        if (halten) {
            CHECK(griff_bild[1] < 0 && re15_player_victim_state() == 0 && pl->hp == 100 && ab == 0,
                  "gehalten: A3-Tor @0x80100A3C-48 sieht die Szene -> kein Griff (Bild %d), kein Netz (%u)",
                  griff_bild[1], ab);
            CHECK(warte[1] > 0, "gehalten: der Arm wartet (0x601 @0x80100A4C) statt zu greifen (%d Bilder)", warte[1]);
            g_scd.player_mode = 0;                       /* Szene vorbei */
            int nach = -1;
            for (int f = 0; f < 200 && nach < 0; f++) {
                bild();
                if (arm->sub_state_1 == 4 || re15_player_victim_state() != 0) nach = f;
            }
            printf("  [arm] nach der Szene: Griff bei Bild %d\n", nach);
            CHECK(nach >= 0, "nach der Szene greift der Arm wieder (Bild %d)", nach);
        } else {
            CHECK(griff_bild[0] >= 0, "frei: Kontrolle — A3 greift (0x401 @0x80100A50) bei Bild %d", griff_bild[0]);
        }
    }
    g_scd.player_mode = 0;
    free(buf);
    printf("%s (%d Fehler)\n", fails ? "FAIL" : "OK", fails);
    return fails ? 1 : 0;
}
