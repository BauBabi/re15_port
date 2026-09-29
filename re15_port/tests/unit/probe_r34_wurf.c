/**
 * @file probe_r34_wurf.c
 * @brief Runde 34 Spur A — SONDE Granate: Wurf, Flug, Abprall, Zuender, Explosion (RE1.5-ESP).
 *
 * Abnahme BAUPLAN §3.1 (analysis/befunde_runde34_granaten/BAUPLAN.md) an der ECHTEN Engine:
 * CORE00.ESP (shared_assets/PSX/DATA) als globale Effektbank, re15_esp_fx_tick, der Port-Resolver
 * re15_resolve_attack. Die Granate wird direkt mit der Spawnhoehe h (Port-Messung, Wurf-Dossier §7)
 * und der Gier gespawnt (re15_esp_granate_spawn = FUN_80019700-Zwilling); SE- und Aufschlag-Haken
 * zeigen auf Spione. Erwartungswerte = unabhaengiger Simulator der Gegenpruefung
 * (re_wurf_gegen_werkzeug/wurf_sim_gegen.py, Tabelle BAUPLAN §1.1).
 *
 * Bild 0 = Spawnbild: der Spawn liegt VOR dem ESP-Tick desselben Bilds (Original: Waffen-FSM
 * `jal 0x80031c44` @0x8001ce0c < ESP-Tick `jal 0x80019e20` @0x8001ce2c), die Sonde ruft daher
 * spawn -> tick.
 *
 * Rueckgabe 0 = gruen, sonst die Nummer der ERSTEN fehlgeschlagenen Pruefung (alle werden gedruckt).
 * Aufruf: probe_r34_wurf [abschnitt]   (ohne Argument: alle Abschnitte)
 */
#include "re15_esp.h"
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_acaec_override_for_test(int on, uint16_t w);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_ready(void);
extern int  re15_player_granate_frame(void);
extern void re15_player_set_hand_world(int32_t x, int32_t y, int32_t z);
extern void re15_player_set_hand_rot(const int32_t r[9]);

static int s_erste = 0, s_fehler = 0;
#define PRUEF(nr, cond, ...) do { if (!(cond)) { \
        fprintf(stderr, "FAIL %d: ", (nr)); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); \
        if (!s_erste) s_erste = (nr); s_fehler++; } } while (0)

static re15_esp_t s_core;

/* ---- Spione ------------------------------------------------------------------------------ */
#define MAX_EV 64
static int      s_bild = 0;
static int      s_se_n = 0;
static uint32_t s_se_code[MAX_EV];
static int      s_se_bild[MAX_EV];
static int32_t  s_se_pos[MAX_EV][3];
static int      s_auf_n = 0, s_auf_art = -1, s_auf_bild = -1;
static int32_t  s_auf_q[3];
static int16_t  s_auf_gier = 0;

static void spy_se(uint32_t code, const int32_t pos[3])
{
    if (s_se_n < MAX_EV) {
        s_se_code[s_se_n] = code; s_se_bild[s_se_n] = s_bild;
        s_se_pos[s_se_n][0] = pos[0]; s_se_pos[s_se_n][1] = pos[1]; s_se_pos[s_se_n][2] = pos[2];
    }
    s_se_n++;
}
static void spy_auf(int re2_art, const int32_t q[3], int16_t gier)
{
    s_auf_n++; s_auf_art = re2_art; s_auf_bild = s_bild;
    s_auf_q[0] = q[0]; s_auf_q[1] = q[1]; s_auf_q[2] = q[2]; s_auf_gier = gier;
}
static void spione_reset(void)
{
    s_se_n = 0; s_auf_n = 0; s_auf_art = -1; s_auf_bild = -1;
    re15_esp_se_hook = spy_se;
    re15_esp_aufschlag_hook = spy_auf;
    g_re15_licht_latch = 0;
}

static uint16_t ru16(const re15_esp_fx_t *f, int off) { return (uint16_t)(f->row[off] | (f->row[off + 1] << 8)); }

/* ---- Weltaufbau ---------------------------------------------------------------------------- */
#define X0 (-6851)       /* Port-Spawnpunkt MITTE, Wurf-Dossier §7 (x/z fuer alle Faelle gleich) */
#define Z0 (-18279)

static void welt_leer(void)
{
    re15_actor_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    re15_player_apply_hitbox(pl);
    pl->x = 30000; pl->y = 0; pl->z = 30000;            /* weit weg (Negativ-Kontrolle Spieler) */
    g_re15_pauseflags = 0;
}
static re15_actor_t *dummy(int slot, uint8_t typ, int16_t hp, int32_t x, int32_t y, int32_t z)
{
    re15_actor_t *e = &g_actors[slot];
    e->active = 1; e->type = typ; e->hp = hp; e->state = 1; e->hit_react = 0;
    e->x = x; e->y = y; e->z = z; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, typ);
    return e;
}

/* ---- ein Wurf ---------------------------------------------------------------------------- */
typedef struct {
    int L, X, Z2, frei;                 /* Liegen / Explosion / Zuender 2 / Platz frei (Bild) */
    int kontakte;                       /* SE-Aufrufe 0x010Axx01 */
    int32_t xl[3];                      /* xlat beim Liegen */
    int16_t wl[3];                      /* wpos beim Liegen */
    int     slot;                       /* Granatenplatz */
    unsigned res0;                      /* Resolver-Zaehler vor dem Wurf */
} wurf_t;

static const re15_esp_fx_t *platz(int i)
{
    return re15_esp_fx_get(i);
}
/* Index eines (aktiven) Platzes: re15_esp_fx_get(i) liefert fuer freie Plaetze NULL. */
static int slot_von(const re15_esp_fx_t *g)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) if (re15_esp_fx_get(i) == g) return i;
    return -1;
}

/* Faellt der Platz `slot` in diesem Bild auf "liegt" (A 31)? -> L. */
static int wurf(uint8_t art, uint16_t acaec, int16_t h, int16_t gier, int bilder, wurf_t *w,
                void (*je_bild)(int k, const wurf_t *w))
{
    memset(w, 0, sizeof *w);
    w->L = w->X = w->Z2 = w->frei = -1;
    re15_esp_fx_reset();
    spione_reset();
    re15_player_acaec_override_for_test(1, acaec);
    w->res0 = re15_esp_granate_resolver_calls();
    s_bild = 0;
    re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, art, X0, h, Z0, gier);
    if (!g) return -1;
    w->slot = slot_von(g);
    int alt_B = 29, alt_z = 42;
    for (int k = 0; k < bilder; k++) {
        s_bild = k;
        re15_esp_fx_tick(NULL);
        const re15_esp_fx_t *f = platz(w->slot);
        int lebt = f && f->granate_art;
        if (lebt && w->L < 0 && ru16(f, 0x00) == 31 && alt_B == 29) {
            w->L = k;
            w->xl[0] = f->xlat_x; w->xl[1] = f->xlat_y; w->xl[2] = f->xlat_z;
            w->wl[0] = f->wpos[0]; w->wl[1] = f->wpos[1]; w->wl[2] = f->wpos[2];
        }
        if (lebt && ru16(f, 0x00) == 31) {
            int z = ru16(f, 0x1e);
            if (alt_z == 7 && z == 6) w->X = k;
            if (alt_z == 2 && z == 1) w->Z2 = k;
            alt_z = z;
        }
        if (!lebt && w->frei < 0 && w->L >= 0) w->frei = k;
        if (lebt) alt_B = ru16(f, 0x02);
        if (je_bild) je_bild(k, w);
        if (w->frei >= 0 && k > w->frei + 30) break;
    }
    for (int i = 0; i < s_se_n && i < MAX_EV; i++)
        if ((s_se_code[i] & 0xffff00ffu) == 0x010A0001u) w->kontakte++;
    return 0;
}

static int zaehle(uint8_t id, uint8_t sub, int16_t scale_or_neg)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || f->effect_id != id || f->sub_index != sub) continue;
        if (scale_or_neg >= 0 && f->scale16 != (uint16_t)scale_or_neg) continue;
        n++;
    }
    return n;
}

/* ================================================================================================
 * Abschnitt 1: MITTE gesund (a = 0x4000, h = -2474, Gier 0) — voll mit Ziel-Dummy und Spieler
 * ================================================================================================ */
static re15_actor_t *s_dum;
static int s_lat_bild = -1, s_lat_n = 0;
static int s_fb1 = -1;          /* Platz des ersten Feuerballs */
static int s_kind_x_ok = 0, s_kind_z2_ok = 0, s_kind_frei_ok = 0;
static int s_dum_vor_x_ok = 1, s_res_vor_x_ok = 1;
static int s_fb1_weg_bild = -1, s_r1_weg_bild = -1;

static void mitte_je_bild(int k, const wurf_t *w)
{
    if (g_re15_licht_latch) { s_lat_n++; s_lat_bild = k; g_re15_licht_latch = 0; }  /* Leser C3 */
    /* vor der Explosion: niemand beschaedigt, kein Resolver */
    if (w->X < 0) {
        if (s_dum->hp != 180) s_dum_vor_x_ok = 0;
        if (re15_esp_granate_resolver_calls() != w->res0) s_res_vor_x_ok = 0;
    }
    if (k == 109) {
        /* Kind 0x03195000 im SELBEN Bild initialisiert (Flags 0x13 nach Routine 10) */
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
            const re15_esp_fx_t *f = re15_esp_fx_get(i);
            if (f && f->effect_id == 3 && f->sub_index == 0x19 && f->flags == 0x13 &&
                f->frame == 10 && f->scale16 == 0x5000 && re15_esp_fx_visible(f) &&
                f->x == w->wl[0] && f->y == w->wl[1] - 500 && f->z == w->wl[2] &&
                f->clut == 0x78D1 && f->tpage == 0x001E &&
                f->wpos[0] == w->wl[0] && f->wpos[1] == w->wl[1] - 500 && f->wpos[2] == w->wl[2])
                { s_kind_x_ok = 1; s_fb1 = i; }
        }
    }
    if (k == 114) {
        /* Zuender 2: Feuerball #2 + Rauch #1 NEU (fb1 lebt noch: 13 Bilder X..X+12) */
        int fb = zaehle(3, 0x19, 0x5000), r1 = zaehle(3, 0x0B, 0x5400);
        int r1_ok = 0;
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
            const re15_esp_fx_t *f = re15_esp_fx_get(i);
            if (f && f->effect_id == 3 && f->sub_index == 0x0B && f->scale16 == 0x5400 &&
                f->flags == 0x13 && f->frame == 8 && f->clut == 0x7851 && f->tpage == 0x005E &&
                f->xlat_y == -135 && f->drift_y == -130)
                r1_ok = 1;
        }
        s_kind_z2_ok = (fb == 2 && r1 == 1 && r1_ok);
    }
    if (k == 116) {
        /* Zuender 0: Granatenplatz frei, Rauch #2 (0x5800) — darf den Granatenplatz belegen */
        const re15_esp_fx_t *f = re15_esp_fx_get(w->slot);
        s_kind_frei_ok = (f && f->effect_id == 3 && f->sub_index == 0x0B && f->scale16 == 0x5800 &&
                          f->granate_art == 0 && f->flags == 0x13);
    }
    if (s_fb1 >= 0 && s_fb1_weg_bild < 0 && k > 109) {
        const re15_esp_fx_t *f = re15_esp_fx_get(s_fb1);
        if (!f || f->effect_id != 3 || f->sub_index != 0x19 || f->scale16 != 0x5000) s_fb1_weg_bild = k;
    }
    if (k > 114 && s_r1_weg_bild < 0 && zaehle(3, 0x0B, 0x5400) == 0) s_r1_weg_bild = k;
}

static void abschnitt_mitte(void)
{
    welt_leer();
    /* Ziel-Dummy 0x27 (Kasten {0,-1440,0,1600,1440,1600} @0x80121350) 300 neben der Liegestelle
     * (Simulator: Liegen bei lokal x 11929 / z 1752, Welt-y 9), Spieler 900 waagrecht entfernt. */
    int32_t xL = X0 + 11929, zL = Z0 + 1752;
    s_dum = dummy(1, 0x27, 180, xL + 300, 0, zL);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = xL - 900; pl->y = 0; pl->z = zL;
    wurf_t w;
    s_lat_bild = -1; s_lat_n = 0; s_fb1 = -1; s_kind_x_ok = s_kind_z2_ok = s_kind_frei_ok = 0;
    s_dum_vor_x_ok = 1; s_res_vor_x_ok = 1; s_fb1_weg_bild = s_r1_weg_bild = -1;

    /* Bild 0 einzeln pruefen (Routine 30 im Spawnbild) */
    re15_esp_fx_reset(); spione_reset();
    re15_player_acaec_override_for_test(1, 0x4000);
    s_bild = 0;
    re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, 2, X0, -2474, Z0, 0);
    PRUEF(11, g != NULL && g->granate_art == 2 && g->flags == 0x03 && g->effect_id == 4 &&
               g->sub_index == 0x0d && g->scale16 == 0x1000 && g->clut == 0x7B11 && g->tpage == 0x001F &&
               ru16(g, 0x00) == 30,
          "Spawn: slot=%p art=%u fl=%02x id=%u sub=%u clut=%04x tpage=%04x A=%u", (void *)g,
          g ? g->granate_art : 0, g ? g->flags : 0, g ? g->effect_id : 0, g ? g->sub_index : 0,
          g ? g->clut : 0, g ? g->tpage : 0, g ? ru16(g, 0) : 0);
    if (g) {
        re15_esp_fx_tick(NULL);
        PRUEF(12, g->xlat_x == 280 && g->xlat_y == -50 && g->xlat_z == 24 &&
                   g->drift_x == 279 && g->drift_y == -40 && g->drift_z == 24 &&
                   g->accel_x == -1 && g->accel_y == 10 && g->accel_z == 0,
              "Bild 0: xlat (%d,%d,%d) vel (%d,%d,%d) acc (%d,%d,%d) (soll xlat (280,-50,24) vel (279,-40,24) acc (-1,10,0))",
              g->xlat_x, g->xlat_y, g->xlat_z, g->drift_x, g->drift_y, g->drift_z, g->accel_x, g->accel_y, g->accel_z);
        PRUEF(13, ru16(g, 0x1e) == 42 && ru16(g, 0x26) == 7 && ru16(g, 0x00) == 0 && ru16(g, 0x02) == 29 &&
                   g->frame == 23 && g->flags == 0x03 && re15_esp_fx_visible(g),
              "Bild 0: zuender %u zaehler %u A %u B %u satz %d fl %02x (soll 42/7/0/29/23/03)",
              ru16(g, 0x1e), ru16(g, 0x26), ru16(g, 0x00), ru16(g, 0x02), g->frame, g->flags);
        PRUEF(14, g->wpos[0] == X0 && g->wpos[1] == -2474 && g->wpos[2] == Z0,
              "Bild 0: wpos (%d,%d,%d) soll Spawnpunkt", g->wpos[0], g->wpos[1], g->wpos[2]);
        re15_esp_fx_tick(NULL);
        PRUEF(15, g->frame == 24 && g->wpos[0] == X0 + 280 && g->wpos[1] == -2474 - 50 && g->wpos[2] == Z0 + 24,
              "Bild 1: satz %d wpos (%d,%d,%d) (soll 24, Spawn + (280,-50,24))", g->frame, g->wpos[0], g->wpos[1], g->wpos[2]);
    }

    /* ganzer Wurf */
    if (wurf(2, 0x4000, -2474, 0, 200, &w, mitte_je_bild) != 0) { PRUEF(16, 0, "Spawn fehlgeschlagen"); return; }
    PRUEF(17, w.L == 73 && w.X == 109 && w.Z2 == 114 && w.frei == 116,
          "Zeitlinie L %d X %d Z2 %d frei %d (soll 73/109/114/116)", w.L, w.X, w.Z2, w.frei);
    PRUEF(18, w.xl[0] == 11929 && w.xl[1] == 2483 && w.xl[2] == 1752,
          "Weg bis L xlat (%d,%d,%d) (soll 11929/2483/1752)", w.xl[0], w.xl[1], w.xl[2]);
    PRUEF(19, w.wl[0] == X0 + 11929 && w.wl[1] == 9 && w.wl[2] == Z0 + 1752,
          "Liegestelle wpos (%d,%d,%d) (soll (%d,9,%d))", w.wl[0], w.wl[1], w.wl[2], X0 + 11929, Z0 + 1752);
    {
        static const uint32_t soll_code[8] = { 0x010A0601u, 0x010A0501u, 0x010A0401u, 0x010A0301u,
                                               0x010A0201u, 0x010A0101u, 0x010A0001u, 0x010A0001u };
        static const int soll_bild[8] = { 29, 47, 55, 60, 64, 67, 70, 73 };
        static const int soll_tiefe[8] = { 136, 90, 16, 25, 16, 3, 9, 9 };
        int ok = (s_se_n == 9);
        for (int i = 0; i < 8 && i < s_se_n; i++)
            if (s_se_code[i] != soll_code[i] || s_se_bild[i] != soll_bild[i] || s_se_pos[i][1] != soll_tiefe[i])
                ok = 0;
        PRUEF(20, ok, "SE-Folge: n=%d; erste: %08x@%d y=%d", s_se_n,
              s_se_n ? s_se_code[0] : 0, s_se_n ? s_se_bild[0] : -1, s_se_n ? s_se_pos[0][1] : 0);
        for (int i = 0; i < s_se_n && i < 9; i++)
            printf("  SE %d: %08x Bild %d pos (%d,%d,%d)\n", i, s_se_code[i], s_se_bild[i],
                   s_se_pos[i][0], s_se_pos[i][1], s_se_pos[i][2]);
        PRUEF(21, s_se_n == 9 && s_se_code[8] == 0x04080001u && s_se_bild[8] == 109 &&
                   s_se_pos[8][0] == w.wl[0] && s_se_pos[8][1] == w.wl[1] - 500 && s_se_pos[8][2] == w.wl[2],
              "Explosions-SE: n=%d code %08x Bild %d pos (%d,%d,%d)", s_se_n, s_se_n > 8 ? s_se_code[8] : 0,
              s_se_n > 8 ? s_se_bild[8] : -1, s_se_n > 8 ? s_se_pos[8][0] : 0, s_se_n > 8 ? s_se_pos[8][1] : 0,
              s_se_n > 8 ? s_se_pos[8][2] : 0);
    }
    PRUEF(22, re15_esp_granate_resolver_calls() == w.res0 + 1 && s_res_vor_x_ok,
          "Resolver-Aufrufe %u (soll genau 1, erst im Bild 109)", re15_esp_granate_resolver_calls() - w.res0);
    PRUEF(23, s_lat_n == 1 && s_lat_bild == 109, "Licht-Latch %d-mal, Bild %d (soll 1x, Bild 109)", s_lat_n, s_lat_bild);
    PRUEF(24, s_dum_vor_x_ok && s_dum->hp == 180 - 1000 && s_dum->state == 3 &&
               s_dum->sub_state_1 == 9 && s_dum->sub_state_2 == 1,
          "Dummy 0x27: hp %d state %u +5 %u +6 %u (soll -820/3/9/1)", s_dum->hp, s_dum->state,
          s_dum->sub_state_1, s_dum->sub_state_2);
    PRUEF(25, g_actors[0].hp == 100 - 1000 && g_actors[0].state == 3,
          "Spieler 900 entfernt: hp %d state %u (soll -900/3)", g_actors[0].hp, g_actors[0].state);
    PRUEF(26, s_kind_x_ok, "Kind 0x03195000 im Explosionsbild nicht sichtbar/initialisiert (Flags 0x13, Satz 10, an P)");
    PRUEF(27, s_kind_z2_ok, "Zuender 2 (Bild 114): Feuerball #2 + Rauch #1 (0x030B5400) fehlen/falsch");
    PRUEF(28, s_kind_frei_ok, "Zuender 0 (Bild 116): Granatenplatz nicht frei bzw. Rauch #2 nicht auf dem Platz");
    PRUEF(29, s_fb1_weg_bild == 109 + 13, "Feuerball #1 lebt bis Bild %d (soll X..X+12, weg in 122)", s_fb1_weg_bild);
    PRUEF(30, s_r1_weg_bild == 114 + 15, "Rauch #1 weg in Bild %d (soll X+5..X+19, weg in 129)", s_r1_weg_bild);
}

/* ================================================================================================
 * Abschnitt 2: Zeitlinien HOCH / TIEF / vergiftet (Tabelle BAUPLAN §1.1)
 * ================================================================================================ */
static void abschnitt_zeitlinien(void)
{
    struct { const char *name; uint16_t a; int16_t h; int L, X, Z2, frei, kontakte; int32_t xl[3]; int nr; } F[] = {
        { "HOCH gesund",     0x8000, -3171, 88, 124, 129, 131,  8, {18760, 3180, 1848}, 31 },
        { "TIEF gesund",     0x2000,  -772, 40,  76,  81,  83,  6, { 1543,  792,   40}, 36 },
        { "HOCH vergiftet",  0x8002, -3171, 94, 130, 135, 137, 10, {18721, 3180, 1974}, 41 },
        { "MITTE vergiftet", 0x4002, -2474, 79, 115, 120, 122, 10, {11938, 2483, 1896}, 46 },
    };
    for (unsigned i = 0; i < sizeof F / sizeof F[0]; i++) {
        welt_leer();
        wurf_t w;
        if (wurf(2, F[i].a, F[i].h, 0, 220, &w, NULL) != 0) { PRUEF(F[i].nr, 0, "%s: Spawn", F[i].name); continue; }
        PRUEF(F[i].nr + 1, w.L == F[i].L && w.X == F[i].X && w.Z2 == F[i].Z2 && w.frei == F[i].frei,
              "%s: L %d X %d Z2 %d frei %d (soll %d/%d/%d/%d)", F[i].name, w.L, w.X, w.Z2, w.frei,
              F[i].L, F[i].X, F[i].Z2, F[i].frei);
        PRUEF(F[i].nr + 2, w.kontakte == F[i].kontakte, "%s: Kontakt-SEs %d (soll %d)", F[i].name, w.kontakte, F[i].kontakte);
        PRUEF(F[i].nr + 3, w.xl[0] == F[i].xl[0] && w.xl[1] == F[i].xl[1] && w.xl[2] == F[i].xl[2],
              "%s: xlat bei L (%d,%d,%d) (soll %d/%d/%d)", F[i].name, w.xl[0], w.xl[1], w.xl[2],
              F[i].xl[0], F[i].xl[1], F[i].xl[2]);
        printf("  %-16s L %d X %d Z2 %d frei %d Kontakte %d xlat (%d,%d,%d)\n", F[i].name, w.L, w.X, w.Z2,
               w.frei, w.kontakte, w.xl[0], w.xl[1], w.xl[2]);
    }
}

/* ================================================================================================
 * Abschnitt 3: Gier 1024 — Endlage = RotY(1024) * (11929, ., 1752) um den Spawnpunkt
 * ================================================================================================ */
static void abschnitt_gier(void)
{
    welt_leer();
    wurf_t w;
    if (wurf(2, 0x4000, -2474, 1024, 200, &w, NULL) != 0) { PRUEF(51, 0, "Spawn"); return; }
    /* RotY(1024): cos = 0, sin = 4096 (Tabelle 0x800794c4[1024] = 0x00001000) ->
     * x' = (c*vx + s*vz) >> 12 = vz, z' = (-s*vx + c*vz) >> 12 = -vx (RotMatrix @0x8006816c/20c/2a4) */
    PRUEF(52, w.L == 73 && w.X == 109, "Gier 1024: L %d X %d (Zeitlinie muss gleich bleiben)", w.L, w.X);
    PRUEF(53, w.xl[0] == 11929 && w.xl[2] == 1752, "Gier 1024: lokales xlat (%d,%d)", w.xl[0], w.xl[2]);
    PRUEF(54, w.wl[0] == X0 + 1752 && w.wl[1] == 9 && w.wl[2] == Z0 - 11929,
          "Gier 1024: Liegestelle (%d,%d,%d) (soll (%d,9,%d))", w.wl[0], w.wl[1], w.wl[2], X0 + 1752, Z0 - 11929);
    /* Negativ-Kontrolle: Gier 0 liegt woanders */
    welt_leer();
    wurf_t w0;
    wurf(2, 0x4000, -2474, 0, 90, &w0, NULL);
    PRUEF(55, w0.wl[0] != w.wl[0] && w0.wl[2] != w.wl[2], "Gier 0 und 1024 enden an derselben Stelle");
}

/* ================================================================================================
 * Abschnitt 4: 0x0A / 0x0B — gleicher Flug, Resolver-Art 3/4, Aufschlag-Haken, keine HE-Inhalte
 * ================================================================================================ */
static void abschnitt_saeure_brand(void)
{
    for (int k = 0; k < 2; k++) {
        uint8_t art = (uint8_t)(3 + k);
        int nr = 55 + 5 * k;
        welt_leer();
        int32_t xL = X0 + 11929, zL = Z0 + 1752;
        re15_actor_t *d = dummy(1, 0x27, 180, xL + 300, 0, zL);
        int lat = 0, kinder = 0, kind_nach_frei = 0;
        /* eigener Bildlauf, um HE-Kinder und Latch zu sehen */
        re15_esp_fx_reset(); spione_reset();
        re15_player_acaec_override_for_test(1, 0x4000);
        unsigned r0 = re15_esp_granate_resolver_calls();
        re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, art, X0, -2474, Z0, 777);
        if (!g) { PRUEF(nr + 1, 0, "Art %u: Spawn", art); continue; }
        int slot = slot_von(g);
        int X = -1, frei = -1, hp_bild = -1;
        int16_t wl[3] = {0, 0, 0};
        for (int b = 0; b < 160; b++) {
            s_bild = b;
            re15_esp_fx_tick(NULL);
            if (g_re15_licht_latch) { lat++; g_re15_licht_latch = 0; }
            const re15_esp_fx_t *f = re15_esp_fx_get(slot);
            /* Gier 777 dreht die Flugbahn: den Dummy im Liegebild 300 neben die ECHTE Liegestelle
             * setzen (Weltlage = Anker + RotY(777) * xlat, @0x8001a118-2a4). */
            if (f && f->granate_art && ru16(f, 0x00) == 31 && ru16(f, 0x1e) == 42) {
                d->x = f->wpos[0] + 300; d->y = 0; d->z = f->wpos[2];
            }
            if (f && f->granate_art && ru16(f, 0x00) == 31 && ru16(f, 0x1e) == 6 && X < 0) {
                X = b; wl[0] = f->wpos[0]; wl[1] = f->wpos[1]; wl[2] = f->wpos[2];
            }
            if (d->hp != 180 && hp_bild < 0) hp_bild = b;
            if (frei < 0 && X >= 0 && !(f && f->granate_art)) {
                frei = b;
                kind_nach_frei = zaehle(3, 0x0B, -1);
            }
        }
        kinder = zaehle(3, 0x19, -1) + zaehle(3, 0x0B, -1);
        PRUEF(nr + 1, X == 109 && frei == 116, "Art %u: X %d frei %d (soll 109/116)", art, X, frei);
        PRUEF(nr + 2, re15_esp_granate_resolver_calls() == r0 + 1 && hp_bild == 109,
              "Art %u: Resolver %u-mal, Schaden im Bild %d (soll 1x, 109)", art, re15_esp_granate_resolver_calls() - r0, hp_bild);
        PRUEF(nr + 3, d->hp == 180 - 1000 && d->state == 3 && d->sub_state_1 == (art == 3 ? 10 : 11) && d->sub_state_2 == 1,
              "Art %u: Dummy hp %d state %u +5 %u +6 %u (soll -820/3/%d/1)", art, d->hp, d->state,
              d->sub_state_1, d->sub_state_2, art == 3 ? 10 : 11);
        PRUEF(nr + 4, s_auf_n == 1 && s_auf_bild == 109 && s_auf_art == (art == 3 ? 2 : 1) &&
                      s_auf_q[0] == wl[0] && s_auf_q[1] == wl[1] && s_auf_q[2] == wl[2] && s_auf_gier == 777,
              "Art %u: Aufschlag n=%d Bild %d re2_art %d q (%d,%d,%d) gier %d (soll 1x/109/%d/wpos/777)",
              art, s_auf_n, s_auf_bild, s_auf_art, s_auf_q[0], s_auf_q[1], s_auf_q[2], s_auf_gier, art == 3 ? 2 : 1);
        {
            int he_se = 0;
            for (int i = 0; i < s_se_n && i < MAX_EV; i++) if (s_se_code[i] == 0x04080001u) he_se++;
            PRUEF(nr + 5, he_se == 0 && lat == 0 && kinder == 0 && kind_nach_frei == 0 && s_se_n == 8,
                  "Art %u: HE-Inhalte: SE 0x04080001 %d-mal, Latch %d, Kinder %d/%d, SEs %d (soll 0/0/0/0, 8 Kontakt-SEs)",
                  art, he_se, lat, kinder, kind_nach_frei, s_se_n);
        }
    }
}

/* ================================================================================================
 * Abschnitt 5: Pool voll, Raumwechsel, Negativ-Kontrollen
 * ================================================================================================ */
static void abschnitt_rand(void)
{
    /* Pool voll: 96 Plaetze belegt (Blut-Effekt 0 als Altplatz) -> kein Spawn, kein Absturz */
    welt_leer();
    re15_esp_fx_reset();
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) re15_esp_fx_spawn_ex(&s_core, 0x03, 0x19, 0x1000, 0, 0, 0, 0);
    PRUEF(71, re15_esp_fx_count() == RE15_ESP_FX_MAX, "Pool nicht voll: %d", re15_esp_fx_count());
    re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, 2, X0, -2474, Z0, 0);
    PRUEF(72, g == NULL, "Pool voll: Granate trotzdem gespawnt");
    for (int b = 0; b < 30; b++) re15_esp_fx_tick(NULL);
    {
        int gr = 0;
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) { const re15_esp_fx_t *f = re15_esp_fx_get(i); if (f && f->granate_art) gr++; }
        PRUEF(73, gr == 0, "Pool voll: %d Granatenplaetze", gr);
    }

    /* Raumwechsel: liegende Granate verschwindet ohne Explosion/Schaden (FUN_80019354 `sb zero`
     * @0x80019378, gerufen @0x8003996c -> Port re15_esp_fx_reset) */
    welt_leer();
    int32_t xL = X0 + 11929, zL = Z0 + 1752;
    re15_actor_t *d = dummy(1, 0x27, 180, xL + 300, 0, zL);
    re15_esp_fx_reset(); spione_reset();
    re15_player_acaec_override_for_test(1, 0x4000);
    unsigned r0 = re15_esp_granate_resolver_calls();
    g = re15_esp_granate_spawn(&s_core, 2, X0, -2474, Z0, 0);
    for (int b = 0; b < 90 && g; b++) re15_esp_fx_tick(NULL);   /* liegt seit Bild 73 */
    PRUEF(74, g && g->active && ru16(g, 0x00) == 31, "Raumwechsel: Granate liegt nicht (A %u)", g ? ru16(g, 0) : 0);
    re15_esp_fx_reset();
    for (int b = 0; b < 120; b++) re15_esp_fx_tick(NULL);
    PRUEF(75, re15_esp_granate_resolver_calls() == r0 && d->hp == 180 && re15_esp_fx_count() == 0,
          "Raumwechsel: Resolver %u, Dummy hp %d, Pool %d (soll 0/180/0)",
          re15_esp_granate_resolver_calls() - r0, d->hp, re15_esp_fx_count());

    /* Negativ-Kontrolle Zielbits: a ohne 0x8000/0x4000/0x2000 -> Routine 30 schreibt keine
     * Geschwindigkeit und keinen Zaehler (@0x8001848c/b4/510 alle beq) -> senkrechter Fall,
     * Zaehler 0 der Zeile -> liegt beim ersten Kontakt */
    welt_leer();
    wurf_t w;
    wurf(2, 0x0000, -2474, 0, 120, &w, NULL);
    PRUEF(76, w.kontakte == 1 && w.xl[0] == 0 && w.xl[2] == 0, "a=0: Kontakte %d xlat (%d,%d) (soll 1, 0/0)",
          w.kontakte, w.xl[0], w.xl[2]);

    /* Negativ-Kontrolle Spieler 1000 entfernt: unveraendert */
    welt_leer();
    g_actors[0].x = xL - 1000; g_actors[0].y = 0; g_actors[0].z = zL;
    wurf(2, 0x4000, -2474, 0, 130, &w, NULL);
    PRUEF(77, g_actors[0].hp == 100, "Spieler 1000 entfernt: hp %d (soll 100)", g_actors[0].hp);
    /* und 949 entfernt: getroffen (R = 450 + 500 = 950, streng <) */
    welt_leer();
    g_actors[0].x = xL - 949; g_actors[0].y = 0; g_actors[0].z = zL;
    wurf(2, 0x4000, -2474, 0, 130, &w, NULL);
    PRUEF(78, g_actors[0].hp == -900, "Spieler 949 entfernt: hp %d (soll -900)", g_actors[0].hp);

    /* Sammel-Bodenklemme gilt weiter fuer andere Effekte (Negativ-Kontrolle A6): Huelse id 4 sub 0 */
    welt_leer();
    re15_esp_fx_reset();
    re15_esp_fx_spawn_rows(&s_core, 4, 0, 0x0800, 0, 0, 0, 60, 0);
    {
        const re15_esp_fx_t *h = re15_esp_fx_get(0);
        PRUEF(79, h && h->floor_y == 60, "Huelse: floor_y %d (soll 60 = Klemme aktiv)", h ? h->floor_y : -1);
    }
}

/* ================================================================================================
 * Abschnitt 6: SPIELSCHRITT (re15_game_step, ROOM1140) — Abzug, Spawn, R1 los, Drehen, 9/10/11
 * Reihenfolge je Bild wie das Original: Spielschritt (Spieler-FSM @0x8001ce0c) -> ESP-Tick
 * (@0x8001ce2c); die Sonde ruft den ESP-Tick selbst (E10 — die Plattform-Schleife gehoert Spur C).
 * ================================================================================================ */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_rdt_buf = NULL;
static int32_t            s_hand[3];     /* Waffenknochen-T (fest, Einheitsrotation) */

static void sp_bild(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    re15_esp_fx_tick(re15_esp_room_bank());
    g_re15_licht_latch = 0;
}

static int sp_bringup(int waffe, int menge)
{
    if (!s_rdt_buf) {
        char pfad[600];
        snprintf(pfad, sizeof pfad, "%s/STAGE1/ROOM1140.RDT", RE15_ASSET_PSX_DIR);
        FILE *fp = fopen(pfad, "rb");
        if (!fp) return -1;
        fseek(fp, 0, SEEK_END); long n = ftell(fp); fseek(fp, 0, SEEK_SET);
        s_rdt_buf = (uint8_t *)malloc((size_t)n);
        if (!s_rdt_buf || fread(s_rdt_buf, 1, (size_t)n, fp) != (size_t)n) { fclose(fp); return -1; }
        fclose(fp);
        if (re15_rdt_parse(s_rdt_buf, (size_t)n, &s_rdt) != 0) return -1;
    }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
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
    {   /* W09-Baenke: Clip 7/9/11 = 35/40/40 Bilder (PLD/PL00W09.PLW @0x24/0x2c/0x34, K3); die
         * uebrigen Clips (Heben 6, Halten 8/10/12) als Sonden-Mock 12 wie probe_abzug_takt. */
        uint16_t fcs[16];
        for (int i = 0; i < 16; i++) fcs[i] = 12;
        fcs[7] = 35; fcs[9] = 40; fcs[11] = 40;
        re15_player_set_aim_clip_lens(fcs, 16);
    }
    re15_inv_load_briefing();
    g_inv.slots[3].id = (uint8_t)waffe; g_inv.slots[3].qty = (uint8_t)menge; g_inv.slots[3].flags = 0;
    re15_player_set_equipped_weapon(waffe);
    {   /* Waffenknochen = Einheitsmatrix an (x, y-1500, z) -> Spawnpunkt = Knochen + Versatz */
        int32_t r[9] = { 4096, 0, 0, 0, 4096, 0, 0, 0, 4096 };
        re15_player_set_hand_rot(r);
        s_hand[0] = pl->x; s_hand[1] = pl->y - 1500; s_hand[2] = pl->z;
        re15_player_set_hand_world(s_hand[0], s_hand[1], s_hand[2]);
    }
    return 0;
}

/* R1 halten bis ZIELBEREIT (Heben Clip 6 laeuft ab). */
static int sp_zielen(void)
{
    int n = 0;
    for (; n < 80 && !re15_player_aim_ready(); n++) sp_bild(RE15_PAD_BIT_R1, n == 0 ? RE15_PAD_BIT_R1 : 0);
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

static void abschnitt_spielschritt(void)
{
    /* 6a — Abzug mit Gegner 1299 vor Leon: KEIN Schaden im Abzugsbild (P1: ENT[9].resolve = 0,
     * Handler 0x80033B38 nur `jal 0x8004eae4` @0x80033b40); Spawn im Clipbild 22 (MITTE,
     * `ori v0,zero,0x16` @0x800336a4/@0x800336fc) mit Art 2 und Gier = rot_y. */
    for (int wi = 0; wi < 3; wi++) {
        int waffe = 9 + wi;
        int nr = 91 + 6 * wi;
        if (sp_bringup(waffe, 5) != 0) { PRUEF(nr, 0, "ROOM1140/Bringup"); return; }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        re15_actor_t *e = dummy(1, 0x27, 180, pl->x + 1299, 0, pl->z);   /* rot_y 0 = +x vor Leon */
        int z = sp_zielen();
        PRUEF(nr, z > 0, "Waffe %d: nicht zielbereit (%d)", waffe, z);
        pl->rot_y = 0;
        int abzug = -1, spawn = -1, spawn_gier = -1, spawn_art = -1, erster_schaden = -1;
        int32_t spawn_pos[3] = {0, 0, 0};
        int menge_nach_abzug = -1;
        for (int b = 0; b < 60; b++) {
            int vor_rec = re15_player_granate_frame();
            sp_bild((uint16_t)(RE15_PAD_BIT_R1 | (b == 0 ? RE15_PAD_BIT_SQUARE : 0)),
                    (uint16_t)(b == 0 ? RE15_PAD_BIT_SQUARE : 0));
            if (abzug < 0 && vor_rec < 0 && re15_player_granate_frame() >= 0) {
                abzug = b; menge_nach_abzug = g_inv.slots[3].qty;
            }
            if (erster_schaden < 0 && e->hp != 180) erster_schaden = b;
            int gp = granate_platz();
            if (spawn < 0 && gp >= 0) {
                const re15_esp_fx_t *g = re15_esp_fx_get(gp);
                spawn = b; spawn_gier = g->param; spawn_art = g->granate_art;
                spawn_pos[0] = g->x; spawn_pos[1] = g->y; spawn_pos[2] = g->z;
            }
        }
        printf("  Waffe %d: zielbereit nach %d, Abzug Bild %d, Menge %d, Spawn Bild %d (A+%d) art %d gier %d "
               "anker (%d,%d,%d), erster Schaden %d\n", waffe, z, abzug, menge_nach_abzug, spawn,
               spawn - abzug, spawn_art, spawn_gier, spawn_pos[0], spawn_pos[1], spawn_pos[2], erster_schaden);
        PRUEF(nr + 1, abzug == 0 && menge_nach_abzug == 4, "Waffe %d: Abzug Bild %d, Menge %d (soll 0 / 4)",
              waffe, abzug, menge_nach_abzug);
        PRUEF(nr + 2, erster_schaden < 0, "Waffe %d: Gegner 1299 vor Leon beschaedigt im Bild %d (Bruecke!)",
              waffe, erster_schaden);
        PRUEF(nr + 3, spawn == abzug + 22, "Waffe %d: Spawn im Bild A+%d (soll A+22, Clipbild 0x16)",
              waffe, spawn - abzug);
        PRUEF(nr + 4, spawn_art == 2 + wi && spawn_gier == 0,
              "Waffe %d: Art %d Gier %d (soll %d / 0)", waffe, spawn_art, spawn_gier, 2 + wi);
        /* Anker = Knochen-R * Versatz + Knochen-T (MITTE {0,0,0x1f4} @0x80033738-44) */
        PRUEF(nr + 5, spawn_pos[0] == s_hand[0] && spawn_pos[1] == s_hand[1] && spawn_pos[2] == s_hand[2] + 0x1f4,
              "Waffe %d: Anker (%d,%d,%d) (soll Knochen + {0,0,0x1f4} = (%d,%d,%d))", waffe,
              spawn_pos[0], spawn_pos[1], spawn_pos[2], s_hand[0], s_hand[1], s_hand[2] + 0x1f4);
    }

    /* 6b — R1 los nach Clipbild 10 -> kein Spawn, Munition trotzdem -1 (Byte2 0x0a @0x800740ba,
     * `sltu v0,v0,a0` @0x8003363c, 0x800aca5a := 3 @0x8003364c) */
    {
        if (sp_bringup(9, 5) != 0) { PRUEF(109, 0, "Bringup 6b"); return; }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
        sp_zielen();
        int spawn = -1;
        for (int b = 0; b < 60; b++) {
            uint16_t pad = (b <= 14) ? RE15_PAD_BIT_R1 : 0;           /* R1 los im Bild 15 */
            if (b == 0) pad |= RE15_PAD_BIT_SQUARE;
            sp_bild(pad, (uint16_t)(b == 0 ? RE15_PAD_BIT_SQUARE : 0));
            if (spawn < 0 && granate_platz() >= 0) spawn = b;
        }
        PRUEF(109, spawn < 0 && g_inv.slots[3].qty == 4, "R1 los: Spawn %d, Menge %d (soll kein Spawn, 4)",
              spawn, g_inv.slots[3].qty);
        /* Negativ-Kontrolle: R1 nur in den Clipbildern 3..8 los (<= 10) und danach wieder
         * gehalten -> KEIN Bruch (Schwelle `sltu v0,v0,a0` = Bild > 10, @0x8003363c), Spawn 22 */
        if (sp_bringup(9, 5) != 0) return;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
        sp_zielen();
        spawn = -1;
        for (int b = 0; b < 60; b++) {
            uint16_t pad = (b >= 3 && b <= 8) ? 0 : RE15_PAD_BIT_R1;
            if (b == 0) pad |= RE15_PAD_BIT_SQUARE;
            sp_bild(pad, (uint16_t)(b == 0 ? RE15_PAD_BIT_SQUARE : 0));
            if (spawn < 0 && granate_platz() >= 0) spawn = b;
        }
        PRUEF(110, spawn == 22, "R1 los in den Clipbildern 3..8: Spawn Bild %d (soll 22 — Bruch erst ab Bild > 10)", spawn);
    }

    /* 6c — Drehen im Wurf: LINKS (virtuell 0x8) -> Gier -= 24 je Bild, RECHTS (0x2) -> += 24
     * (@0x8003355c-0x800335fc: `lbu` 0x80074091[(w-1)*5] = 0x30, `srl v0,v0,1`, subu/addu auf
     * 0x800acabe); die Spawn-Gier ist die gedrehte (@0x800336cc nach dem Drehen). */
    for (int dir = 0; dir < 2; dir++) {
        if (sp_bringup(9, 5) != 0) { PRUEF(111, 0, "Bringup 6c"); return; }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
        sp_zielen();
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->rot_y = 1000;
        uint16_t taste = dir ? RE15_PAD_BIT_RIGHT : RE15_PAD_BIT_LEFT;
        int soll_d = dir ? 24 : -24;
        int bad = 0, spawn = -1, spawn_gier = -1, gier_im_spawnbild = -1;
        int deltas[40];
        for (int b = 0; b < 30; b++) {
            int vor = pl->rot_y;
            uint16_t pad = RE15_PAD_BIT_R1 | (b >= 1 ? taste : 0);
            if (b == 0) pad |= RE15_PAD_BIT_SQUARE;
            sp_bild(pad, (uint16_t)(b == 0 ? RE15_PAD_BIT_SQUARE : 0));
            int d = (((int)pl->rot_y - vor + 0x800) & 0xfff) - 0x800;
            deltas[b] = d;
            if (b >= 1 && d != soll_d) bad++;
            if (spawn < 0 && granate_platz() >= 0) {
                spawn = b; spawn_gier = re15_esp_fx_get(granate_platz())->param; gier_im_spawnbild = pl->rot_y;
            }
        }
        printf("  Drehen %s: Deltas", dir ? "RECHTS" : "LINKS");
        for (int b = 0; b < 8; b++) printf(" %d", deltas[b]);
        printf(" ... Spawn Bild %d Gier %d (rot_y %d)\n", spawn, spawn_gier, gier_im_spawnbild);
        PRUEF(111 + 2 * dir, bad == 0, "Drehen %s: %d Bilder mit Delta != %d (Bild1 %d, Bild2 %d)",
              dir ? "RECHTS" : "LINKS", bad, soll_d, deltas[1], deltas[2]);
        PRUEF(112 + 2 * dir, spawn == 22 && spawn_gier == gier_im_spawnbild &&
                             spawn_gier == 1000 + 22 * soll_d,
              "Drehen %s: Spawn %d Gier %d (soll Bild 22, Gier %d = 1000 + 22 Bilder x %d)", dir ? "RECHTS" : "LINKS",
              spawn, spawn_gier, 1000 + 22 * soll_d, soll_d);
    }
}

int main(int argc, char **argv)
{
    const char *nur = (argc > 1) ? argv[1] : NULL;
    char pfad[600];
    snprintf(pfad, sizeof pfad, "%s/DATA/CORE00.ESP", RE15_ASSET_PSX_DIR);
    FILE *fp = fopen(pfad, "rb");
    if (!fp) { fprintf(stderr, "CORE00.ESP fehlt: %s\n", pfad); return 1; }
    fseek(fp, 0, SEEK_END); long n = ftell(fp); fseek(fp, 0, SEEK_SET);
    uint8_t *buf = (uint8_t *)malloc((size_t)n);
    if (!buf || fread(buf, 1, (size_t)n, fp) != (size_t)n) { fclose(fp); return 1; }
    fclose(fp);
    if (re15_esp_parse_global(buf, (size_t)n, &s_core) != 0) { fprintf(stderr, "CORE00.ESP Parse\n"); return 1; }
    re15_esp_set_global_bank(&s_core);
    re15_esp_set_room_bank(NULL);

    if (!nur || !strcmp(nur, "mitte"))      { printf("[1] MITTE gesund\n");        abschnitt_mitte(); }
    if (!nur || !strcmp(nur, "zeit"))       { printf("[2] Zeitlinien\n");          abschnitt_zeitlinien(); }
    if (!nur || !strcmp(nur, "gier"))       { printf("[3] Gier 1024\n");           abschnitt_gier(); }
    if (!nur || !strcmp(nur, "saeure"))     { printf("[4] 0x0A / 0x0B\n");         abschnitt_saeure_brand(); }
    if (!nur || !strcmp(nur, "rand"))       { printf("[5] Rand/Negativ\n");        abschnitt_rand(); }
    if (!nur || !strcmp(nur, "schritt"))    { printf("[6] Spielschritt\n");        abschnitt_spielschritt(); }

    re15_player_acaec_override_for_test(0, 0);
    re15_esp_se_hook = NULL; re15_esp_aufschlag_hook = NULL;
    free(buf);
    if (s_fehler) { fprintf(stderr, "probe_r34_wurf: %d Fehler, erste Pruefung %d\n", s_fehler, s_erste); return s_erste; }
    printf("probe_r34_wurf: ALLE PRUEFUNGEN GRUEN\n");
    return 0;
}
