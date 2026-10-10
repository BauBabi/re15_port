/*
 * test_r36_granate.c — Runde 36 Spur D: Handgranate.
 *   [1] feuer — NUTZER-VORGABE "kein Feuer-Effekt an Zombies bei normalen Granaten, nur Zerplatzen;
 *       Feuer nur bei incendiary": Zaehlung der Flammen-Slots (RE1.5-CORE00-Id 8 = FEUER) nach dem
 *       HE-Tod (Art 2, RE2-Zeile 9 -> 0x80108BEC -> Spreng-Russ FUN_8010640C) = 0, nach dem Brand-Tod
 *       (Art 4, Zeile 10 -> Verkohlung FUN_80106128) > 0; Zerreissen und Russ-Tinten bleiben.
 *   [2] wurf  — "Granate wird manchmal nicht geworfen": N Abzuege = N Granaten (Spielschritt, s.u.).
 *
 * Dossier: analysis/befunde_runde36/D_granate.md. Arena/Spielschritt wie test_r35_granate.c [4].
 * Rueckgabe 0 = gruen, sonst die Nummer der ersten fehlgeschlagenen Pruefung.
 * Aufruf: test_r36_granate [feuer|wurf|alle]
 */
#include "re15_esp.h"
#include "re15_granate_r35.h"
#include "re15_granate_r36.h"
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_scd.h"
#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_tim.h"
#include "re2_ems.h"
#include "re2_fx.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_re2z_last_death_handler(void);

static int s_erste = 0, s_fehler = 0;
#define PRUEF(nr, cond, ...) do { if (!(cond)) { \
        printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); \
        if (!s_erste) s_erste = (nr); s_fehler++; } else { printf("ok   %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); } } while (0)

static re15_esp_t s_core;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- Arena (ROOM1140, RE2-KI, echter Spielschritt) — wie test_r35_granate.c [4] ---------------- */
static re15_rdt_t         s_rdt;
static uint8_t           *s_rdt_buf = NULL;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
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
    re15_esp_fx_tick(re15_esp_room_bank());
}
static int room_load(int room_id, const char *stage)
{
    char path[512];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/%s/ROOM%04X.RDT", stage, room_id);
    free(s_rdt_buf); s_rdt_buf = NULL;
    size_t n = 0;
    s_rdt_buf = slurp(path, &n);
    if (!s_rdt_buf || re15_rdt_parse(s_rdt_buf, n, &s_rdt) != 0) { printf("RDT %s fehlt\n", path); return -1; }
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    g_current_room_id = (uint16_t)room_id;
    return 0;
}
static re15_actor_t *zombie_arena(uint8_t typ)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_room_rdt_ok = 0; g_room_change.pending = 0;
    { extern void scd_register_current_rdt(const re15_rdt_t *rdt); scd_register_current_rdt(NULL); }
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && re15_re2_owns_type(g_actors[s].type)) load_re2_bank(g_actors[s].type);
    for (int f = 0; f < 60; f++) frame();
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) { slot = s; break; }
    if (slot < 0) return NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot];
    e->grid_id = 0; e->re2z_f10e = 0; e->re2z_flags21a &= (uint16_t)~0x12u; e->re2z_self1d3 = 0;
    e->hit_react = 0;
    re15_ai_set_state_word(e, 0x101);
    pl->x = e->x - 6000; pl->z = e->z; pl->y = e->y;
    for (int f = 0; f < 10; f++) frame();
    re15_esp_fx_reset();
    return e;
}

/* Flammen-Slots im Pool: RE1.5-CORE00-Id 8 (FEUER) = das Ziel der RE2-Klasse 5 im Dekoder re2z_gore_fx_ex. */
static int flammen_slots(void)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->effect_id == 8) n++;
    }
    return n;
}
static int fx_slots(void)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) if (re15_esp_fx_get(i)) n++;
    return n;
}

/* ================================================================================================
 * [1] FEUER — HE (Art 2) ohne Flammen, Brand (Art 4) mit Flammen.
 * ================================================================================================ */
static void abschnitt_feuer(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(100, 0, "ROOM1140 fehlt"); return; }
    const int LAEUFE = 4;
    int he_ok = 0, he_gore = 0, he_tint = 0, he_gate = 0, brand_ok = 0, brand_tot = 0;
    int he_max = 0, brand_min = 9999;
    for (int lauf = 0; lauf < LAEUFE; lauf++) {
        /* ---- HE (Art 2): Zeile 9 -> Zerreissen 0x80108BEC -> Spreng-Russ FUN_8010640C ---- */
        re15_actor_t *e = zombie_arena(0x10);
        if (!e) { PRUEF(101, 0, "kein Zombie 0x10 in ROOM1140"); return; }
        re15_damage_seed_rng(0x2000u + (uint32_t)lauf * 7919u);
        re15_granate_r36_zaehler_reset();
        const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
        int n = re15_granate_r35_explosion(p, 0, 2);
        if (n != 1 || e->state != 3 || e->sub_state_1 != 9u) {
            PRUEF(101, 0, "HE Lauf %d: Treffer %d Zustand %u Zeile %u (1/3/9)", lauf, n, e->state, e->sub_state_1); return;
        }
        int fl_max = 0, fx_max = 0, teile = 0;
        for (int f = 0; f < 90; f++) {
            frame();
            int fl = flammen_slots(); if (fl > fl_max) fl_max = fl;
            int fx = fx_slots();      if (fx > fx_max) fx_max = fx;
        }
        for (int pp = 0; pp < 20; pp++) if (e->re2z_part_flags[pp] & 0x4Au) teile++;
        int hdl = re15_re2z_last_death_handler();
        unsigned gate = re15_granate_r36_russ_unterdrueckt();
        printf("   HE Lauf %d: Flammen max %d, FX max %d, Handler %d, Teile %d, Tinte[0]=0x%06x, Gate %u, Zustand %u\n",
               lauf, fl_max, fx_max, hdl, teile, (unsigned)e->re2z_part_tint[0], gate, e->state);
        if (fl_max > he_max) he_max = fl_max;
        if (fl_max == 0) he_ok++;
        if ((hdl == 5 || hdl == 6) && teile >= 1 && fx_max > 0) he_gore++;
        if (e->re2z_part_tint[0] == 0x00404040u) he_tint++;     /* Russ-Tinte Part 0 `sw a0,112` @0x8010648c */
        if (gate >= 1u) he_gate++;

        /* ---- BRAND (Art 4): Zeile 10 -> Verkohlung FUN_80106128 -> Flammen (unveraendert) ---- */
        e = zombie_arena(0x10);
        if (!e) { PRUEF(102, 0, "kein Zombie 0x10 in ROOM1140 (Brand)"); return; }
        re15_damage_seed_rng(0x3000u + (uint32_t)lauf * 7919u);
        const int32_t pb[3] = { e->x + 300, e->y - 500, e->z };
        n = re15_granate_r35_explosion(pb, 0, 4);
        if (n != 1 || e->sub_state_1 != 10u) {
            PRUEF(102, 0, "Brand Lauf %d: Treffer %d Zeile %u (1/10)", lauf, n, e->sub_state_1); return;
        }
        fl_max = 0;
        for (int f = 0; f < 90; f++) {
            frame();
            int fl = flammen_slots(); if (fl > fl_max) fl_max = fl;
        }
        printf("   Brand Lauf %d: Flammen max %d, Zustand %u, Zeile %u, f10e 0x%04x, Tinte[0]=0x%06x\n",
               lauf, fl_max, e->state, e->sub_state_1, (unsigned)e->re2z_f10e, (unsigned)e->re2z_part_tint[0]);
        if (fl_max < brand_min) brand_min = fl_max;
        if (fl_max > 0) brand_ok++;
        if (e->state == 7 || e->state == 3) brand_tot++;
    }
    PRUEF(103, he_ok == LAEUFE, "HE (Art 2, Zeile 9): Flammen-Slots (Id 8) nach dem Tod = 0 in %d/%d Laeufen (max %d; vorher 2 je Zombie)",
          he_ok, LAEUFE, he_max);
    PRUEF(104, he_gate == LAEUFE, "HE: der Spreng-Russ FUN_8010640C wurde erreicht und seine Flammen @0x80106418-44/@0x80106450-74 unterdrueckt (%d/%d)",
          he_gate, LAEUFE);
    PRUEF(105, he_gore == LAEUFE, "HE: Zerreissen bleibt — Handler 0x80108BEC/0x80109610, abplatzende Teile (0x4A), Effekte im Pool (%d/%d)",
          he_gore, LAEUFE);
    PRUEF(106, he_tint == LAEUFE, "HE: Russ-Tinte Part 0 = 0x404040 (@0x8010648c) bleibt (%d/%d)", he_tint, LAEUFE);
    PRUEF(107, brand_ok == LAEUFE, "Brand (Art 4, Zeile 10): Flammen-Slots (Id 8) > 0 in %d/%d Laeufen (min %d) — Feuer unveraendert",
          brand_ok, LAEUFE, brand_min);
    PRUEF(108, re15_granate_r36_russ_flammen(9) == 0 && re15_granate_r36_russ_flammen(17) == 0,
          "Gate: Zeile 9 (HE/GL-Explosiv) und 17 (Rakete) ohne Flammen (NUTZER-VORGABE)");
}

/* ================================================================================================
 * [2] WURF — "Manchmal selten loest die Granate nicht aus / wird nicht geworfen" (s. Dossier §3).
 * ================================================================================================ */
static void abschnitt_wurf(void)
{
    printf("   (Abschnitt wurf: siehe Dossier §3 — wird mit der Ursache ergaenzt)\n");
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    int alle = (strcmp(teil, "alle") == 0);
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &n);
    if (!buf) { fprintf(stderr, "CORE00.ESP fehlt\n"); return 1; }
    if (re15_esp_parse_global(buf, n, &s_core) != 0) { fprintf(stderr, "CORE00.ESP Parse\n"); return 1; }
    re15_esp_set_global_bank(&s_core);
    re15_esp_set_room_bank(NULL);
    re15_esp_aufschlag_hook = NULL;

    if (alle || strstr(teil, "feuer")) { printf("[1] Feuer\n"); abschnitt_feuer(); }
    if (alle || strstr(teil, "wurf"))  { printf("[2] Wurf\n");  abschnitt_wurf(); }

    free(buf);
    if (s_fehler) { printf("test_r36_granate: %d Fehler, erste Pruefung %d\n", s_fehler, s_erste); return s_erste; }
    printf("test_r36_granate: ALLE PRUEFUNGEN GRUEN\n");
    return 0;
}
