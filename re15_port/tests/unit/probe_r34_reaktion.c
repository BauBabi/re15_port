/*
 * probe_r34_reaktion.c — Runde 34 (Granaten), Spur B: Gegnerreaktion aller Typen auf die
 * Explosion (Resolver Art 2/3/4 mit Punkt P) und das Bodenfeuer (re15_re2_gl_apply), gefahren
 * ueber den ECHTEN Spielschritt (re15_game_step: KI, Filter, HP-Stempel, Anim-Vorschub).
 *
 * Dossier: analysis/befunde_runde34_granaten/bau_b.md. Rueckgabe 0 = gruen, sonst die Nummer der
 * ersten fehlgeschlagenen Pruefung.
 *
 * Aufruf: probe_r34_reaktion [teil]   teil = zombie | hund | spinne | re15 | g5 | treppe |
 *                                            zensus | alle (Vorgabe)
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
#include "re15_tim.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static int s_first_fail = 0, s_fails = 0;
#define CHECK(nr, c, ...) do { if (!(c)) { printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); \
    s_fails++; if (!s_first_fail) s_first_fail = (nr); } else { printf("ok   %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); } } while (0)

static re15_rdt_t         s_rdt;
static uint8_t           *s_rdt_buf = NULL;
static int                s_room_id = 0;
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

/* ---- RE2-Baenke (pc_enemy_load-Spiegel, RE2-Zweig), damit Clip-Laengen echt sind ---- */
static const uint8_t *re2_ems_blob(size_t *sz)
{
    static uint8_t *b = NULL; static size_t s = 0; static int tried = 0;
    if (!tried) { tried = 1; b = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s); }
    *sz = s; return b;
}
static void load_re2_bank(uint8_t type)
{
    if (re15_enemy_find(type)) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return;
    size_t sz = 0; const uint8_t *ems = re2_ems_blob(&sz);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (ems && re2_ems_load_bank(ems, sz, type, eb, &tim) == 0) { eb->buf = NULL; eb->ok = 1; return; }
    eb->type = 0;
}

static void frame(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
    re15_game_step(&s_ctx);
}

static int room_load(int room_id, const char *stage)
{
    char path[512];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/%s/ROOM%04X.RDT", stage, room_id);
    free(s_rdt_buf); s_rdt_buf = NULL;
    size_t n = 0;
    s_rdt_buf = slurp(path, &n);
    if (!s_rdt_buf || re15_rdt_parse(s_rdt_buf, n, &s_rdt) != 0) { printf("RDT %s fehlt\n", path); return -1; }
    s_room_id = room_id;
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    return 0;
}

static void bringup(re15_ai_flavor_t flavor)
{
    re15_ai_flavor_set(flavor);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    extern void re15_esp_fx_reset(void);
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    if (flavor != RE15_AI_FLAVOR_RE15)
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && re15_re2_owns_type(g_actors[s].type)) load_re2_bank(g_actors[s].type);
}

static void zensus_print(const char *tag)
{
    printf("[%s] Raum %04X:", tag, s_room_id);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        printf(" [%d t=%02x st=%d/%d/%d hp=%d grid=%02x @(%d,%d,%d) r%d]", s, e->type, e->state,
               e->sub_state_1, e->sub_state_2, e->hp, e->grid_id, e->x, e->y, e->z, e->rot_y);
    }
    printf("\n");
}

static void explosion_bei(const re15_actor_t *e, int32_t dx, uint8_t art)
{
    re15_attack_box_t b;
    b.x = e->x + dx; b.y = e->y - 500; b.z = e->z; b.radius = 500;   /* P = (x, y-500, z) @0x800185a0-ac */
    re15_resolve_attack(&b, art, -1);
}

/* =========================================================================================
 * TEIL "zombie" — B5: Brand-/Saeure-DoT und Leichen-Ausblender (RE2-KI).
 * ========================================================================================= */
static void teil_zombie(void)
{
    printf("== zombie (B5)\n");
    if (room_load(0x1140, "STAGE1") != 0) { CHECK(100, 0, "ROOM1140 fehlt"); return; }
    const uint8_t art[2] = { 4, 3 };                 /* 4 = Brand (RE2-Zeile 10), 3 = Saeure (11) */
    for (int lauf = 0; lauf < 2; lauf++) {
        bringup(RE15_AI_FLAVOR_RE2);
        for (int f = 0; f < 60; f++) frame();
        if (lauf == 0) zensus_print("1140 RE2");
        /* Brad 0x11 (HP 250 @0x801008C8) aus der Fress-Pose holen: stehend, gehend (EXEC[1]).
         * Harness-Hilfe (Zustand gesetzt, Treffer + KI laufen echt); Arena = nur Brad. */
        int slot = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x11) { slot = s; break; }
        if (slot < 0) { CHECK(100, 0, "kein Brad 0x11 in ROOM1140"); return; }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
        re15_actor_t *e = &g_actors[slot];
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        e->grid_id = 0; e->re2z_f10e = 0; e->re2z_flags21a &= (uint16_t)~0x12u; e->re2z_self1d3 = 0;
        e->hit_react = 0;
        re15_ai_set_state_word(e, 0x101);
        pl->x = e->x - 6000; pl->z = e->z; pl->y = e->y;
        for (int f = 0; f < 10; f++) frame();
        int16_t hp0 = e->hp;
        explosion_bei(e, 300, art[lauf]);
        CHECK(101 + 10 * lauf, e->hp == hp0 - 200 && e->state == 2 &&
                               e->sub_state_1 == (lauf == 0 ? 10 : 11) && e->re2z_hits1d2 == 0,
              "Brad %s: hp %d -> %d (-200), st=%d, +5=%d, 1D2=%d", lauf == 0 ? "Brand" : "Saeure",
              hp0, e->hp, e->state, e->sub_state_1, e->re2z_hits1d2);
        /* Verkohlung (+0x10E |= 0x80, +0x21A |= 0x800 — FUN_80106128 @0x8010613C-48 + @0x80105df0-fc)
         * bzw. Aetzung (+0x21A |= 0x1800 @0x8010632C-38) aus Stagger-P0 (@0x80105DC4-F18). */
        int el = 0;
        for (int f = 0; f < 5 && !el; f++) {
            frame();
            el = (lauf == 0) ? ((e->re2z_f10e & 0x80u) && (e->re2z_flags21a & 0x800u))
                             : ((e->re2z_flags21a & 0x1800u) == 0x1800u);
        }
        CHECK(102 + 10 * lauf, el, "Element-Bits nach dem Treffer: f10e=0x%04X 21a=0x%04X",
              e->re2z_f10e, e->re2z_flags21a);
        /* DoT: HP -1 genau dann, wenn beim Gang-Executor (+0x236 & 7) == 0 war; der Zaehler
         * steigt NACH dem Dispatch (@0x801004F8-508) -> nach dem Bild gilt (+0x236 & 7) == 1. */
        int drops = 0, bad = 0, walk_frames = 0, last_drop = -1, gap_bad = 0, died = -1;
        uint8_t tod_zeile = 0, tod_1d2 = 0, tod_1d3 = 0; uint16_t tod_21a = 0;
        for (int f = 0; f < 1600 && died < 0; f++) {
            int16_t hp_vor = e->hp; uint8_t st_vor = e->state; uint8_t sub_vor = e->sub_state_1;
            frame();
            int walk = (st_vor == 1 && (sub_vor == 1 || sub_vor == 2));
            if (walk) walk_frames++;
            else last_drop = -1;                 /* Abstand nur ueber durchgehendes Gehen messen */
            if (e->hp == hp_vor - 1 && walk) {
                drops++;
                if ((e->re2z_c236 & 7u) != 1u) bad++;
                if (last_drop >= 0 && f - last_drop != 8) gap_bad++;
                last_drop = f;
            }
            if (e->state == 3 && st_vor != 3) {
                died = f; tod_zeile = e->sub_state_1; tod_1d2 = e->re2z_hits1d2; tod_1d3 = e->re2z_self1d3;
                tod_21a = e->re2z_flags21a;
            }
        }
        CHECK(103 + 10 * lauf, drops >= 40 && bad == 0 && gap_bad == 0,
              "DoT: %d HP-Stufen in %d Gang-Bildern, Takt-Fehler %d, Abstand != 8: %d", drops,
              walk_frames, bad, gap_bad);
        CHECK(104 + 10 * lauf, died >= 0 && tod_zeile == (lauf == 0 ? 0x0A : 0x0B) && tod_1d2 == 4 &&
                               (tod_1d3 & 0x80u) && (tod_21a & 0x2000u),
              "Tod am Element: Bild %d, +5=0x%02X, 1D2=%d, 1D3=0x%02X, 21a=0x%04X", died, tod_zeile,
              tod_1d2, tod_1d3, tod_21a);
        /* Leichen-Ausblender (@0x8010a810-868, nur verkohlt): Part 0 R-Byte sinkt alle 4 Bilder
         * um 1, Stopp bei 16. */
        if (lauf == 0) {
            int corpse = -1;
            for (int f = 0; f < 600 && corpse < 0; f++) { frame(); if (e->state == 7) corpse = f; }
            uint32_t r0 = e->re2z_part_tint[0] & 0xffu;
            int stufen = 0, abstand_ok = 1, last = -1;
            for (int f = 0; f < 400; f++) {
                uint32_t vor = e->re2z_part_tint[0] & 0xffu;
                frame();
                uint32_t nach = e->re2z_part_tint[0] & 0xffu;
                if (nach == vor - 1u) { if (last >= 0 && f - last != 4) abstand_ok = 0; last = f; stufen++; }
            }
            uint32_t r1 = e->re2z_part_tint[0] & 0xffu;
            CHECK(105, corpse >= 0 && stufen > 0 && abstand_ok && (r1 == 16u || r1 < r0),
                  "Leichen-Ausblender: Leiche ab Bild %d, R %u -> %u in %d Stufen (je 4 Bilder: %d)",
                  corpse, r0, r1, stufen, abstand_ok);
            CHECK(106, r1 == 16u, "Ausblender stoppt bei R = 16 (@0x8010a830-34): R = %u", r1);
        }
    }
    /* (107) NEGATIV-KONTROLLE: ein gehender Brad OHNE Element-Bits verliert in 400 Bildern keine HP. */
    {
        bringup(RE15_AI_FLAVOR_RE2);
        for (int f = 0; f < 60; f++) frame();
        int slot = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x11) { slot = s; break; }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
        re15_actor_t *e = &g_actors[slot];
        e->grid_id = 0; e->re2z_f10e = 0; e->re2z_flags21a &= (uint16_t)~0x12u; e->re2z_self1d3 = 0;
        re15_ai_set_state_word(e, 0x101);
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = e->x - 6000;
        int16_t hp0 = e->hp;
        for (int f = 0; f < 400; f++) frame();
        CHECK(107, e->hp == hp0, "ohne Element: hp %d -> %d", hp0, e->hp);
    }
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    int alle = (strcmp(teil, "alle") == 0);
    if (alle || !strcmp(teil, "zombie")) teil_zombie();
    printf("probe_r34_reaktion %s: %d Fehler (erste Pruefung %d)\n", teil, s_fails, s_first_fail);
    return s_first_fail;
}
