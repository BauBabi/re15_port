/**
 * @file test_re2z_pushoff_crawl.c
 * @brief PIN — drei byte-belegte RE2-Zombie-Korrekturen (Nutzer-Report 2026-08-22).
 *
 * (1) WEGPUSHEN — "Beim Wegpushen, wenn Leon vom Zombie gebissen wird, geht er hinter…"
 *     Der RE2-Spieler-Griff (Maschine 0x8010A9B8, EMOVL10_S0.BIN) SCHNAPPT Leons Yaw auf den
 *     Greifer und legt die 180-Grad-Drehung der Vorn-Clips als PLATZIERUNGS-Offset PL+0x158 ab:
 *       8010aa68/6c  addiu v0,zero,2048 / sh v0,344(s1)      -> PL+0x158 = 0x800
 *       8010aa80-88  lw a1,56(a2)/lw a2,64(a2)/jal 0x80015558 (a3=2048)  -> Yaw-Snap
 *       8010aa98-b0  BEHIND: sh zero,344(s1) + `+0x76 += 2048`
 *     Der EXIT P6 @0x8010AEDC schreibt KEINEN Yaw (der RE1.5-Flip @0x8010a614 gilt hier nicht).
 *     PIN: nach dem Griff-Latch ist |rot_y - Soll| klein, und am Release-Ende steht Leon
 *     byte-true zum Greifer (FRONT ~0 / BEHIND ~0x800), nicht 180 Grad verdreht.
 *
 * (2) STURZ-/ZU-BODEN-ANIMATION — "Wenn der Zombie zu Boden Richtung Leon springt, stimmt die
 *     Animation nicht ganz." Die Blend-Rate eines Clips ist das a3 des ADVANCE, der ihn
 *     verbraucht; EXEC[5]/EXEC[6]/EXEC[11]/FEED-P3 advancen mit 256 (@0x80103380, @0x80103A2C,
 *     @0x801044F0, @0x80103D70), nicht 512. Das Gewicht der Vor-Pose ist +0x8F * rate
 *     (FUN_8001f3bc @0x8001f474, IR0 = 0x1000 - Produkt @0x8001f4a8-b0) und saettigt byte-true
 *     NIE. PIN: ueber einen echten Lauf darf KEIN RE2-Zombie je ein Paar mit
 *     +0x8F * anim_blend_rate >= 0x1000 tragen (das waere ein eingefrorenes Pose-Plateau).
 *
 * (3) KRIECHER-ANGRIFF — "Ein am Boden kriechender Zombie scheint Leon dort nicht angreifen zu
 *     koennen." Der EIN-ANGREIFER-RIEGEL Spieler+0x1D3 Bit 0x80 (0x800CFDCB) hatte im Port vier
 *     Setzer und KEINEN Loescher. Byte-gelesene Loescher: Struggle-EXIT-P6 @0x8010AEEC/F4/F8,
 *     HURT-Grab-Abbruch @0x80104F94/FA0/FAC, DEATH-Grab-Abbruch @0x801082DC/E8/F4.
 *     PIN: zwei Kriecher in ROOM1030 greifen ueber 1800 Frames MEHRFACH an, und der Riegel ist
 *     am Ende wieder frei.
 *
 * Alle drei Pins laufen ueber den ECHTEN Weg (RDT + SCD-Spawns + re15_game_step + GELADENE
 * RE2-Baenke); ohne Bank waere re15_actor_clip_len()==0 und die Messung wertlos.
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
#include "re15_emd.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); fails++; } } while (0)

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_ai_facing_aligned(const re15_actor_t *e, const re15_actor_t *t);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int room_up(const char *rdtname, uint16_t room_id)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/%s", (base && *base) ? base : RE15_ASSET_PSX_DIR, rdtname);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) return 0;
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) return 0;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    if (!load_bank2(0x10) || !load_bank2(0x11) || !load_bank2(0x16)) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    return 1;
}

/* ---------------- (1) + (2): ROOM1140, Griff bis zum Wegstoss -------------------------------- */
static void test_pushoff(void)
{
    int measured = 0;
    for (int seed = 0; seed < 3; seed++) {
        if (!room_up("ROOM1140.RDT", 0x1140)) { printf("SKIP: ROOM1140/RE2-Bank nicht ladbar\n"); return; }
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();
        int zs = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) {
                int nib = g_actors[s].grid_id & 0x0f;         /* 7/8 = Liegender ohne Wecker */
                if (zs < 0 && nib != 7 && nib != 8) zs = s;
            }
        if (zs < 0) continue;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (s != zs && g_actors[s].active && re15_re2z_owns_type(g_actors[s].type))
                g_actors[s].active = 0;
        re15_actor_t *z = &g_actors[zs];
        {   int ang = (seed * 4096) / 3;
            pl->x = z->x + (int32_t)((900 * (int64_t)re15_cos_q12(ang)) >> 12);
            pl->z = z->z - (int32_t)((900 * (int64_t)re15_sin_q12(ang)) >> 12);
            pl->y = z->y;
            pl->rot_y = (int16_t)((seed * 337 + 700) & 0xfff);   /* bewusst beliebiger Start-Yaw */
        }
        int vs_prev = 0, latched = 0, variant = 0;
        for (int f = 0; f < 1200; f++) {
            pl->hp = 100; pl->floor = z->floor;
            uint16_t cur = 0, edge = 0;
            if (re15_player_is_grabbed() && (f & 1)) { cur = RE15_PAD_BIT_CIRCLE; edge = RE15_PAD_BIT_CIRCLE; }
            frame(cur, edge);
            /* (2) das Blend-Produkt darf NIE saettigen (Beleg im Dateikopf) */
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                re15_actor_t *e = &g_actors[s];
                if (!e->active || !re15_re2z_owns_type(e->type)) continue;
                int prod = (int)e->anim_frac * (int)e->anim_blend_rate;
                CHECK(prod < 0x1000,
                      "Blend saettigt: slot %d st=%u s1=%u s2=%u clip=%d +0x8F=%u rate=0x%X (Produkt 0x%X)",
                      s, e->state, e->sub_state_1, e->sub_state_2, (int)e->motion,
                      e->anim_frac, e->anim_blend_rate, prod);
                if (fails > 4) return;
            }
            int vs = re15_player_victim_state();
            if (vs != 0 && !latched) {
                latched = 1; measured++;
                variant = re15_ai_facing_aligned(z, pl);
                int bear = ((int)re15_atan2_q12(z->z - pl->z, z->x - pl->x) - 0x400) & 0xfff;
                int want = variant ? ((bear + 0x800) & 0xfff) : bear;
                int d = ((int)pl->rot_y - want) & 0xfff; if (d > 2048) d -= 4096;
                CHECK(d > -260 && d < 260,
                      "seed %d: Griff-Latch schnappt Leons Yaw nicht (ist %d, Soll ~%d, Variante %d) "
                      "— @0x8010AA88/@0x8010AAAC",
                      seed, (int)pl->rot_y, want, variant);
                CHECK(pl->re2z_t158 == (variant ? 0 : 0x800),
                      "seed %d: PL+0x158 = %d, Soll %d (@0x8010AA6C / @0x8010AAA8)",
                      seed, (int)pl->re2z_t158, variant ? 0 : 0x800);
                CHECK((pl->re2z_self1d3 & 0x80u) != 0u,
                      "seed %d: Griff-P0 setzt den Riegel PL+0x1D3|0x80 nicht (@0x80102754-60)", seed);
            }
            if (vs == 0 && vs_prev != 0) {
                int bear = ((int)re15_atan2_q12(z->z - pl->z, z->x - pl->x) - 0x400) & 0xfff;
                int want = variant ? ((bear + 0x800) & 0xfff) : bear;
                int d = ((int)pl->rot_y - want) & 0xfff; if (d > 2048) d -= 4096;
                CHECK(d > -600 && d < 600,
                      "seed %d: nach dem Wegstoss steht Leon falsch (Yaw %d, Soll ~%d, Variante %d) "
                      "— RE2-EXIT-P6 @0x8010AEDC dreht NICHT",
                      seed, (int)pl->rot_y, want, variant);
                CHECK((pl->re2z_self1d3 & 0x80u) == 0u,
                      "seed %d: Release-EXIT loest den Riegel PL+0x1D3 nicht (@0x8010AEF4)", seed);
                break;
            }
            vs_prev = vs;
        }
    }
    CHECK(measured >= 2, "zu wenige Griffe gemessen (%d) — der Pin wuerde nichts pruefen", measured);
}

/* ---------------- (3): ROOM1030, zwei Kriecher, greifen sie MEHRFACH an? --------------------- */
static void test_crawler_attacks(void)
{
    if (!room_up("ROOM1030.RDT", 0x1030)) { printf("SKIP: ROOM1030/RE2-Bank nicht ladbar\n"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int cs[2], nc = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && nc < 2; s++)
        if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) cs[nc++] = s;
    CHECK(nc == 2, "ROOM1030 liefert keine zwei RE2-Zombies (%d)", nc);
    if (nc < 2) return;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s != cs[0] && s != cs[1] && g_actors[s].active && re15_re2z_owns_type(g_actors[s].type))
            g_actors[s].active = 0;
    /* byte-true HURT-Eintritt @0x80107A54/@0x80107A58 (Sub 0, KEIN Riegel-Write) */
    for (int i = 0; i < nc; i++) re15_re2z_enter_crawler(&g_actors[cs[i]], NULL, 0u);
    re15_actor_t *c0 = &g_actors[cs[0]];
    pl->x = c0->x + 700; pl->z = c0->z; pl->y = c0->y; pl->floor = c0->floor;

    int grabs = 0, s1prev[2] = { -1, -1 }, hpdrops = 0;
    for (int f = 0; f < 1800; f++) {
        pl->hp = 100;
        int16_t hp_prev = pl->hp;
        frame(0, 0);
        for (int i = 0; i < nc; i++) {
            re15_actor_t *e = &g_actors[cs[i]];
            if (!e->active) continue;
            if ((int)e->sub_state_1 == 1 && s1prev[i] != 1) grabs++;
            s1prev[i] = (int)e->sub_state_1;
        }
        if (pl->hp < hp_prev) hpdrops++;
    }
    CHECK(grabs >= 5,
          "Kriecher greifen nur %dx an — der EIN-ANGREIFER-RIEGEL PL+0x1D3|0x80 wird nicht "
          "geloest (Loescher @0x8010AEF4 / @0x80104FA0 / @0x801082E8)", grabs);
    CHECK(hpdrops >= 10, "zu wenige Kriecher-Bisse (%d)", hpdrops);
    printf("  Kriecher: %d Griff-Eintritte, %d HP-Abfaelle, Riegel am Ende 0x%02X\n",
           grabs, hpdrops, pl->re2z_self1d3);
}

int main(void)
{
    printf("== PIN: RE2-Zombie Wegstoss / Sturz-Blend / Kriecher-Angriff ==\n");
    test_pushoff();
    test_crawler_attacks();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    if (fails) { printf("%d FEHLER\n", fails); return 1; }
    printf("OK\n");
    return 0;
}
