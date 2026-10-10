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
 * [2] WURF — "Manchmal selten loest die Granate nicht aus / wird nicht geworfen" (Dossier §3).
 * Gemessen (r1_sweep): R1 innerhalb von 21 Bildern nach dem Abzug losgelassen -> Rueckstoss-Abbruch ab
 * Cursor 11 (`sltu v0,v0,a0` @0x8003363c mit Byte 2 = 10 @0x800740ba, `j 0x800337ac` @0x80033650 vor dem
 * Spawn @0x80033680-7a8), Granate weg (Abzug Bild 0 @0x80033b40), kein Wurf. Vorgabe: jeder Abzug = ein
 * Wurf — Schwelle der Granate = Spawnbild 19/22/24 (@0x80033690/@0x800336a4/@0x80033758).
 * Spielschritt-Sandkasten wie probe_r34_wurf.c Abschnitt 6 (ROOM1140, keine Gegner, Knochen-Mock).
 * ================================================================================================ */
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_ready(void);
extern int  re15_player_aim_phase_debug(void);   /* Phase | 0x10 = Rueckstoss (player_common.c) */
extern int  re15_player_granate_frame(void);
extern void re15_player_set_hand_world(int32_t x, int32_t y, int32_t z);
extern void re15_player_set_hand_rot(const int32_t r[9]);
extern void re15_player_cmd_reset(void);

static void sp_bild(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    re15_esp_fx_tick(re15_esp_room_bank());
}
static int sp_bringup(int waffe, int menge)
{
    if (room_load(0x1140, "STAGE1") != 0) return -1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_esp_fx_reset();
    g_current_room_id = 0x1140;
    g_re15_pauseflags = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = -7600; pl->y = 0; pl->z = -17600; pl->rot_y = 0;
    re15_player_apply_hitbox(pl);
    re15_collision_set_band(0);
    {   /* W09: Clip 7/9/11 = 35/40/40 Bilder (PLD/PL00W09.PLW @0x24/0x2c/0x34); Rest Mock 12 */
        uint16_t fcs[16];
        for (int i = 0; i < 16; i++) fcs[i] = 12;
        fcs[7] = 35; fcs[9] = 40; fcs[11] = 40;
        re15_player_set_aim_clip_lens(fcs, 16);
    }
    re15_inv_load_briefing();
    g_inv.slots[3].id = (uint8_t)waffe; g_inv.slots[3].qty = (uint8_t)menge; g_inv.slots[3].flags = 0;
    re15_player_set_equipped_weapon(waffe);
    {   int32_t r[9] = { 4096, 0, 0, 0, 4096, 0, 0, 0, 4096 };
        re15_player_set_hand_rot(r);
        re15_player_set_hand_world(pl->x, pl->y - 1500, pl->z);
    }
    return 0;
}
/* R1 (+ Zielhoehe) halten bis ZIELBEREIT; danach die Zielhoehe ein Bild lang anlegen (HOLD-Wechsel 8/10/12). */
static int sp_zielen(int elev)
{
    int n = 0;
    for (; n < 80 && !re15_player_aim_ready(); n++) sp_bild(RE15_PAD_BIT_R1, n == 0 ? RE15_PAD_BIT_R1 : 0);
    if (!re15_player_aim_ready()) return -1;
    uint16_t eb = (elev > 0) ? RE15_PAD_BIT_UP : (elev < 0) ? RE15_PAD_BIT_DOWN : 0;
    for (int k = 0; k < 3; k++) sp_bild((uint16_t)(RE15_PAD_BIT_R1 | eb), 0);
    return re15_player_aim_ready() ? n : -1;
}
static int granate_platz(void)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->granate_art) return i;
    }
    return -1;
}
/* Ein Wurf: Abzug im Bild 0 (SQUARE 6 Bilder wie das Skript MA0.2), R1 bis einschliesslich Bild `r1_bis`
 * gehalten (danach 0; -1 = nie loslassen). Liefert das Spawnbild (-1 = keiner), *lower = erstes Bild der
 * Phase LOWER (4) nach dem Abzug (-1 = nie), *menge = Menge nach 60 Bildern. */
static int wurf(int elev, int r1_bis, int *lower, int *menge)
{
    int spawn = -1; *lower = -1;
    uint16_t eb = (elev > 0) ? RE15_PAD_BIT_UP : (elev < 0) ? RE15_PAD_BIT_DOWN : 0;
    for (int b = 0; b < 60; b++) {
        uint16_t pad = 0;
        if (r1_bis < 0 || b <= r1_bis) pad = (uint16_t)(RE15_PAD_BIT_R1 | eb);
        if (b < 6) pad |= RE15_PAD_BIT_SQUARE;
        sp_bild(pad, (uint16_t)(b == 0 ? RE15_PAD_BIT_SQUARE : 0));
        if (spawn < 0 && granate_platz() >= 0) spawn = b;
        if (*lower < 0 && (re15_player_aim_phase_debug() & 0x0f) == 4) *lower = b;
    }
    *menge = g_inv.slots[3].qty;
    return spawn;
}
static void abschnitt_wurf(void)
{
    static const int k_spawn[3] = { 19, 22, 24 };     /* HOCH / MITTE / TIEF */
    static const int k_elev [3] = { 1, 0, -1 };
    static const char *k_name[3] = { "HOCH", "MITTE", "TIEF" };
    int gesamt_abzuege = 0, gesamt_wuerfe = 0, gesamt_menge_ok = 0, gesamt_lower_ok = 0, gesamt_spawn_ok = 0;
    for (int ei = 0; ei < 3; ei++) {
        int abz = 0, wu = 0, menge_ok = 0, lower_ok = 0, spawn_ok = 0;
        int erster_fehl = -1;
        /* R1 losgelassen in JEDEM Bild r = 0..30 nach dem Abzug, dazu r = -1 (nie) */
        for (int r = -1; r <= 30; r++) {
            if (sp_bringup(9, 5) != 0) { PRUEF(200, 0, "Bringup wurf"); return; }
            for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
            if (sp_zielen(k_elev[ei]) < 0) { PRUEF(200, 0, "%s: nicht zielbereit", k_name[ei]); return; }
            int lower = -1, menge = -1;
            int spawn = wurf(k_elev[ei], r, &lower, &menge);
            abz++;
            if (spawn >= 0) wu++; else if (erster_fehl < 0) erster_fehl = r;
            if (spawn == k_spawn[ei]) spawn_ok++;
            if (menge == 4) menge_ok++;
            /* R1 vor dem Spawnbild losgelassen: LOWER fruehestens im Bild nach dem Spawn, spaetestens 2 danach
             * (Schwelle = Spawnbild, `Cursor > Schwelle`); R1 danach oder nie: LOWER erst nach dem Loslassen */
            if (r >= 0 && r < k_spawn[ei]) { if (lower > k_spawn[ei] && lower <= k_spawn[ei] + 2) lower_ok++; }
            else if (r < 0)                 { if (lower < 0) lower_ok++; }
            else                            { if (lower > r && lower <= r + 2) lower_ok++; }
            if (r == 15 || r == -1 || r == k_spawn[ei] - 1)
                printf("   %s r1_bis=%d: Spawn %d, LOWER %d, Menge %d\n", k_name[ei], r, spawn, lower, menge);
        }
        printf("   %s: %d Abzuege, %d Wuerfe (erster Fehlwurf r=%d), Spawnbild %d ok %d, Menge ok %d, LOWER ok %d\n",
               k_name[ei], abz, wu, erster_fehl, k_spawn[ei], spawn_ok, menge_ok, lower_ok);
        gesamt_abzuege += abz; gesamt_wuerfe += wu; gesamt_menge_ok += menge_ok; gesamt_lower_ok += lower_ok;
        gesamt_spawn_ok += spawn_ok;
    }
    PRUEF(201, gesamt_wuerfe == gesamt_abzuege, "N Abzuege = N Granaten: %d Wuerfe bei %d Abzuegen (R1 los in Bild 0..30 nach dem Abzug, HOCH/MITTE/TIEF; vorher: r <= 18/21/23 ohne Wurf)",
          gesamt_wuerfe, gesamt_abzuege);
    PRUEF(202, gesamt_spawn_ok == gesamt_abzuege, "Spawnbild bleibt 19/22/24 (@0x80033690/@0x800336a4/@0x80033758) in %d/%d", gesamt_spawn_ok, gesamt_abzuege);
    PRUEF(203, gesamt_menge_ok == gesamt_abzuege, "je Abzug genau eine Granate abgezogen (Bild 0 @0x80033b40) in %d/%d", gesamt_menge_ok, gesamt_abzuege);
    PRUEF(204, gesamt_lower_ok == gesamt_abzuege, "R1-Loslassen senkt den Arm (LOWER) im Bild nach dem Abgang bzw. nach dem Loslassen in %d/%d", gesamt_lower_ok, gesamt_abzuege);
    PRUEF(205, re15_granate_r36_recoil_break(9, 1) == 19 && re15_granate_r36_recoil_break(9, 0) == 22 &&
               re15_granate_r36_recoil_break(9, -1) == 24 && re15_granate_r36_recoil_break(10, 0) == 22 &&
               re15_granate_r36_recoil_break(11, 0) == 22 && re15_granate_r36_recoil_break(3, 0) == -1 &&
               re15_granate_r36_recoil_break(15, 0) == -1,
          "Schwelle: 9/10/11 -> 19/22/24 je Zielhoehe, Schusswaffen und Werfer-Klasse unveraendert (-1)");
    /* Negativ-Kontrolle: Schusswaffe 3 (Browning, Byte 2 = 7 @0x8007409c): R1 los im Bild 8 -> LOWER im
     * Bild 9 (unveraendert) */
    {
        if (sp_bringup(3, 15) != 0) { PRUEF(206, 0, "Bringup Browning"); return; }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
        {   uint16_t fcs[16]; for (int i = 0; i < 16; i++) fcs[i] = 23; fcs[6] = 10; fcs[8] = 1; fcs[10] = 1; fcs[12] = 1;
            re15_player_set_aim_clip_lens(fcs, 16); }
        if (sp_zielen(0) < 0) { PRUEF(206, 0, "Browning: nicht zielbereit"); return; }
        int lower = -1;
        for (int b = 0; b < 40; b++) {
            uint16_t pad = (b <= 8) ? RE15_PAD_BIT_R1 : 0;
            if (b == 0) pad |= RE15_PAD_BIT_SQUARE;
            sp_bild(pad, (uint16_t)(b == 0 ? RE15_PAD_BIT_SQUARE : 0));
            if (lower < 0 && (re15_player_aim_phase_debug() & 0x0f) == 4) lower = b;
        }
        PRUEF(206, lower == 9, "Browning (Schwelle 7): R1 los im Bild 8 -> LOWER im Bild %d (soll 9, unveraendert)", lower);
    }
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
