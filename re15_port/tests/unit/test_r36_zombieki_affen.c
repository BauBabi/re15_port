/*
 * Runde 36 Spur F "zombieki", Punkt 3 — "Bei den Monkeys in ROOM 11C0 laeuft das Blut nicht aus wenn sie tot
 * sind." Dossier: analysis/befunde_runde36/F_zombieki.md, Punkt 3; Belege include/re15_zombieki_r36.h.
 *
 * Echter Todesweg in ROOM11C0 (Raum-Harness wie test_r35_affen.c): Gorilla auf 1 HP, Leon mit Handfeuerwaffe
 * (Waffe 3) 2500 davor, ein Treffer -> DEATH (Zustand 3) -> LEICHE (Zustand 7, FUN_8011be54). Gemessen je Takt:
 *   +0x7 (sub_state_3), +0x9e (grab_kill_ctr), Lachenfarbe (crow_pool), Halbmasse +0xBC/+0xBE (crow_shadow_w/h).
 * Soll (STAGE1.BIN): INIT +0xBC/+0xBE = 1700/1800 (@0x801171d8-f0 ueber Record 0x80121350[+6] = 1600);
 *   Leiche +0x7 0 -> 1 im ersten Takt (+0x9e = 0x5a @0x8011be80-84), dann je Takt Tinte 0x00ffff38 und
 *   +0xBC/+0xBE += 8 (@0x8011bed0-bf14), +0x9e-- bis alt == 0 -> +0x7 = 2 (@0x8011bf24-44): 91 Takte mit
 *   Wachstum (Takt 0 mit Durchfall + 90 weitere) -> 1700 + 91*8 = 2428 / 1800 + 91*8 = 2528, danach still.
 * Gegenprobe: mit abgeschalteten Leichen-Haken (re15_r36_affen_lache_enable(0)) bleibt crow_pool 0 und
 *   +0xBC/+0xBE bei 1700/1800 = der Befund des Nutzers (Stand v0.8.23 zusaetzlich 1000/500 Hunde-Werte).
 * Ohne Argument: Riegel. "mess": Zeitlinie der Leiche.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_zombieki_r36.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
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

static uint8_t          *s_raw = NULL; static size_t s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static uint8_t            s_blob27[600000];

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int bank_laden_27(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x27);
    if (eb && eb->ok) return 1;
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x27);
    size_t off = 0, len = 0;
    int ok = (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob27);
    if (ok) { memcpy(s_blob27, ems + off, len); if (!eb) eb = re15_enemy_alloc(0x27); ok = (eb != NULL); }
    if (ok) {
        re15_tim_t tim = (re15_tim_t){0};
        ok = (re15_emd_parse_container(s_blob27, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0);
        if (ok) { eb->ok = 1; eb->buf = NULL;
                  eb->victim_ok = (re15_emd_parse_victim_bank(s_blob27, len, &eb->skel_victim, &eb->anim_victim) == 0); }
        else eb->type = 0;
    }
    free(ems);
    return ok;
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut, int bilder)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_player_victim_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    for (int f = 0; f < bilder; f++) frame(0, 0);
    return 0;
}

static re15_actor_t *aktor_vom_typ(uint8_t typ, int nth)
{
    int k = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) { if (k == nth) return &g_actors[s]; k++; }
    return NULL;
}

typedef struct {
    int tod_bild;            /* erstes Bild mit Zustand 7 */
    uint16_t w0, h0;         /* +0xBC/+0xBE vor dem Tod (INIT-Wert) */
    uint16_t w_ende, h_ende; /* nach 150 Takten Leiche */
    int pool_erst;           /* erstes Leichenbild mit crow_pool = 1 (0 = nie) */
    int takte_wachstum;      /* Anzahl Takte mit += 8 */
    int phase2_bild;         /* Bild, in dem +0x7 = 2 wird (relativ zum Tod) */
    int monoton;             /* Wachstum je Takt genau 0 oder 8, nie anders */
} lache_t;

static int lauf(int haken_an, int zeitlinie, lache_t *r)
{
    memset(r, 0, sizeof *r);
    re15_r36_affen_lache_enable(haken_an);
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -6729, -12800, 647, 5, 3) != 0) { printf("ROOM11C0 nicht ladbar\n"); return 0; }
    if (!bank_laden_27()) { printf("EM027-Bank fehlt\n"); return 0; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *a = aktor_vom_typ(0x27, 0), *b = aktor_vom_typ(0x27, 1);
    if (!a) { printf("kein Gorilla\n"); return 0; }
    if (b) b->active = 0;                                   /* nur ein Gorilla in der Szene */
    /* Gorilla stehend vor Leon, 1 HP; Leon zielt mit der Handfeuerwaffe */
    a->x = pl->x + 2500; a->z = pl->z; a->y = 0; a->rot_y = 2048; a->floor = 0; a->grid_id = 0x10;
    a->state = 1; a->sub_state_1 = 0; a->sub_state_2 = 0; a->sub_state_3 = 0; a->hp = 1;
    pl->rot_y = 0; pl->hp = 100;
    r->w0 = a->crow_shadow_w; r->h0 = a->crow_shadow_h;
    re15_inv_load_briefing();
    re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    re15_player_set_equipped_weapon(3);
    { int es = re15_inv_equipped_slot(); if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    uint16_t lw = a->crow_shadow_w; int takt_leiche = 0; r->monoton = 1;
    for (int f = 1; f <= 600; f++) {
        uint16_t held = 0, edge = 0;
        if (!r->tod_bild) {
            a->hp = (a->state == 3 || a->state == 7) ? a->hp : 1;
            pl->rot_y = (int16_t)(((int)re15_atan2_q12(a->z - pl->z, a->x - pl->x) - 0x400) & 0x0fff);
            held = RE15_PAD_BIT_R1;
            if (re15_player_aim_ready() && (f % 20) == 0) { held |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE; }
        }
        frame(held, edge);
        if (zeitlinie && !r->tod_bild && (f % 20) == 1)
            printf("  F%d gorilla st=%d/%d/%d hp=%d @(%d,%d) dist=%d | PL hp=%d aim=%d\n",
                   f, a->state, a->sub_state_1, a->sub_state_2, (int)a->hp, (int)a->x, (int)a->z,
                   (int)re15_enemy_player_dist(a, pl), (int)pl->hp, re15_player_aim_ready());
        if (!r->tod_bild && a->state == 7) r->tod_bild = f;
        if (r->tod_bild) {
            int d = (int)a->crow_shadow_w - (int)lw;
            if (d != 0 && d != 8) r->monoton = 0;
            if (d == 8) r->takte_wachstum++;
            if (a->crow_pool && !r->pool_erst) r->pool_erst = f;
            if (a->sub_state_3 == 2 && !r->phase2_bild) r->phase2_bild = f - r->tod_bild;
            if (zeitlinie && (takt_leiche < 4 || d != 0 || a->sub_state_3 == 2))
                printf("  Leiche Takt %3d: +0x7=%d +0x9e=%d pool=%d +0xBC=%u +0xBE=%u clip=%d\n", takt_leiche,
                       a->sub_state_3, (int)a->grab_kill_ctr, a->crow_pool, a->crow_shadow_w, a->crow_shadow_h, (int)a->motion);
            takt_leiche++;
            if (takt_leiche >= 150) break;
        }
        lw = a->crow_shadow_w;
    }
    r->w_ende = a->crow_shadow_w; r->h_ende = a->crow_shadow_h;
    printf("%s: Tod Bild %d, INIT +0xBC/+0xBE %u/%u, pool ab Leichenbild %d, %d Takte Wachstum, Phase 2 nach %d Takten, Ende %u/%u, monoton %d\n",
           haken_an ? "NACHHER (Haken an)" : "VORHER (Haken aus)", r->tod_bild, r->w0, r->h0,
           r->pool_erst ? r->pool_erst - r->tod_bild : -1, r->takte_wachstum, r->phase2_bild, r->w_ende, r->h_ende, r->monoton);
    return 1;
}

int main(int argc, char **argv)
{
    const int mess = (argc > 1 && strcmp(argv[1], "mess") == 0);
    lache_t vor, nach;
    if (!lauf(0, mess, &vor)) return 1;
    if (!lauf(1, mess, &nach)) return 1;
    re15_r36_affen_lache_enable(1);
    if (mess) return 0;

    CHECK(nach.tod_bild > 0, "Gorilla stirbt am echten Schusspfad (Zustand 7 ab Bild %d)", nach.tod_bild);
    CHECK(nach.w0 == RE15_R36_AFFEN_SCHATTEN_X && nach.h0 == RE15_R36_AFFEN_SCHATTEN_Z,
          "INIT-Schattenrecord +0xBC/+0xBE = %u/%u (1600+100 / 1600+200 @0x801171e8/f0)", nach.w0, nach.h0);
    /* tod_bild = Bild, in dem der Todes-Commit (Zustand 7) geschrieben wurde; der Leichen-Root laeuft erstmals im
     * Bild danach und faellt dort von +0x7 0 nach 1 durch (@0x8011be68 -> @0x8011bed0, kein Return) */
    CHECK(nach.pool_erst == nach.tod_bild + 1, "Lachenfarbe 0x00ffff38 ab dem ERSTEN Leichen-Root-Takt (Durchfall +0x7 0 -> 1 @0x8011be68/bed0): Bild %d",
          nach.pool_erst - nach.tod_bild);
    CHECK(nach.takte_wachstum == 91 && nach.monoton, "+0xBC/+0xBE wachsen in genau 91 Takten um je 8 (@0x8011bf08-14, +0x9e 0x5a..0 @0x8011bf24-44): %d Takte, monoton %d",
          nach.takte_wachstum, nach.monoton);
    CHECK(nach.phase2_bild == 91, "+0x7 = 2 (inert) nach 91 Takten (alt == 0 @0x8011bf30-44): %d", nach.phase2_bild);
    CHECK(nach.w_ende == RE15_R36_AFFEN_SCHATTEN_X + 91 * 8 && nach.h_ende == RE15_R36_AFFEN_SCHATTEN_Z + 91 * 8,
          "Endmasse %u/%u = 2428/2528", nach.w_ende, nach.h_ende);
    /* Gegenprobe = Befund: ohne die Haken keine Lache, keine Vergroesserung */
    CHECK(vor.pool_erst == 0 && vor.takte_wachstum == 0 && vor.w_ende == vor.w0,
          "GEGENPROBE ohne Haken: keine Lache (pool %d), kein Wachstum (%d), Ende %u = Anfang %u", vor.pool_erst, vor.takte_wachstum, vor.w_ende, vor.w0);
    {   /* Renderer-Haken: Typ 0x27 liefert Halbmasse + Farbe, andere Typen nicht */
        int32_t nx = 500, nz = 600; int pool = 0; uint8_t rr = 0, gg = 0, bb = 0;
        re15_actor_t t; memset(&t, 0, sizeof t); t.type = 0x27; t.crow_shadow_w = 2428; t.crow_shadow_h = 2528; t.crow_pool = 1;
        int ok = re15_r36_affen_lache_render(&t, &nx, &nz, &pool, &rr, &gg, &bb);
        CHECK(ok == 1 && nx == 2428 && nz == 2528 && pool == 1 && rr == 0x38 && gg == 0xff && bb == 0xff,
              "Renderer-Haken 0x27: Halbmasse %d/%d, Lache %d, Farbe %02x/%02x/%02x (0x00ffff38)", (int)nx, (int)nz, pool, rr, gg, bb);
        t.type = 0x20; nx = 500; pool = 0;
        CHECK(re15_r36_affen_lache_render(&t, &nx, &nz, &pool, &rr, &gg, &bb) == 0 && nx == 500 && pool == 0, "Renderer-Haken: Hund 0x20 nicht zustaendig");
    }
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
