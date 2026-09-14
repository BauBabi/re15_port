/* probe_hund_trefferluecke.c - MESSUNG (kein Fix) zu den zwei Nutzer-Befunden 2026-09-14:
 *   (1) "der erste Treffer bei den Zombie Hunden ist irgendwie delayed"
 *   (2) "man kann die Zombie Hunde erst wieder treffen, sobald sie nach dem schiessen wieder
 *        komplett stehen. Vorher sind sie unverwundbar."
 *
 * Gemessen wird NUR, was der Port tut - Bild fuer Bild, mit Dauerfeuer auf einen aufgesetzten
 * Hund (Typ 0x20) bzw. zur Gegenprobe auf einen Zombie (Typ 0x10):
 *   state / sub_state_1,2,3 / hit_react(+0x93) / re2z_self1d3(+0x1D3) / hp / clip / anim_frame
 *   + die Kandidaten-Gates des Resolvers (re15_damage.c, @0x8004712C-30 / @0x80047138-40 /
 *     @0x80047148-50) + das TATSAECHLICHE Ergebnis von re15_player_weapon_fire().
 *
 * Bank-echt: RE2-Flavor laedt die RE2-Bank (shared_assets/RE2/CDEMD0.EMS, re2_ems_load_bank),
 * RE1.5-Flavor die RE1.5-Bank (EMD/CDEMD0.EMS) - ohne Bank ist clip_len 0 und jede
 * Frame-Zahl wertlos.
 *
 * KEIN add_test (reine Messsonde).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_damage.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_esp.h"
#include "re15_math.h"
#include "re15_ai_flavor.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_elevation_for_test(int elev);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static uint8_t *s_ems2  = NULL; static size_t s_ems2_n  = 0;

/* Bank wie das Spiel: RE2-Flavor -> RE2-EMS, sonst RE1.5-EMS. */
static re15_enemy_bank_t *bank_laden(uint8_t type, int re2)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    memset(eb, 0, sizeof *eb); eb->type = type;
    if (re2) {
        if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
        if (s_ems2 && re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) {
            eb->buf = NULL; eb->ok = 1; return eb;
        }
    } else {
        if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
        int idx = s_ems15 ? re15_ems_index_for_type(type) : -1;
        size_t off = 0, len = 0;
        if (idx >= 0 && re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) == 0) {
            uint8_t *blob = (uint8_t *)malloc(len);
            memcpy(blob, s_ems15 + off, len);
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim,
                                                            &eb->anim_victim) == 0);
                return eb;
            }
        }
    }
    eb->type = 0; eb->ok = 0;
    return NULL;
}

static void face(re15_actor_t *e, const re15_actor_t *t)
{
    e->rot_y = (int16_t)(((int)re15_atan2_q12(t->z - e->z, t->x - e->x) - 0x400) & 0xfff);
}

/* Die Kandidaten-Gates des Resolvers, 1:1 aus re15_player_weapon_fire (re15_damage.c:1227-1252).
 * OHNE Hoehenband - das wird separat ueber den Elevations-Sweep getrennt. */
static int gates_basis(const re15_actor_t *e)
{
    if (e->hit_radius_min <= 0) return 0;                                   /* @0x8004712C-30 */
    if (e->state == 7 && !(e->type == 0x25u && re15_ai_re2_for_type(e->type))) return 0;
    if ((e->type == 0x25u || e->type == 0x26u) &&
        re15_ai_re2_for_type(e->type) && e->hp < 0) return 0;               /* @0x80047148-50 */
    if ((e->hit_react & 0x3) == 0x3) return 0;                              /* +0x93 Paar-Riegel */
    return 1;
}

typedef struct {
    int  hits;
    int  erster;
    int  luecken[64];
    int  nluecken;
    int  gestorben;
} ergebnis_t;

static const char *flav_name(int f)
{
    return (f == RE15_AI_FLAVOR_RE2) ? "RE2" : (f == RE15_AI_FLAVOR_MIXED) ? "MIXED" : "RE1.5";
}

/* Ein Messlauf: Gegner aufsetzen, JEDES Bild feuern, alles protokollieren. */
static ergebnis_t lauf(const char *titel, uint8_t type, int flavor, int weapon,
                       int elev, int32_t dist, int frames, int hp_pin, int ausfuehrlich)
{
    ergebnis_t R; memset(&R, 0, sizeof R); R.erster = -1; R.gestorben = -1;

    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_enemy_reset();
    re15_actor_init();
    re15_esp_fx_reset();
    re15_player_aim_reset();

    int re2 = re15_ai_re2_for_type(type);
    re15_enemy_bank_t *eb = bank_laden(type, re2);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = dist; pl->hp = 100;

    const int SL = 1;
    re15_actor_t *e = &g_actors[SL];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = type; e->state = 0; e->x = 0; e->y = 0; e->z = 0;
    e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, type);
    face(e, pl); face(pl, e);

    re15_enemy_ai_run_all(1);                 /* INIT -> ACTIVE */
    e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
    re15_enemy_ai_run_all(1);                 /* 1 ACTIVE-Tick: erst er stempelt aim_band */
    e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);

    re15_player_set_aim_elevation_for_test(elev);
    int hp_start = (hp_pin > 0) ? hp_pin : (int)e->hp;
    e->hp = (int16_t)hp_start;

    printf("\n--- %s ---\n", titel);
    printf("    Typ 0x%02X  Flavor %s (typ-bezogen RE2=%d)  Waffe %d  Elevation %d  Distanz %d\n",
           type, flav_name(flavor), re2, weapon, elev, (int)dist);
    printf("    Bank: %s  clips=%d", (eb && eb->ok) ? "OK" : "FEHLT",
           (eb && eb->ok) ? eb->anim.clip_count : 0);
    if (eb && eb->ok) {
        printf("  Laengen:");
        for (int c = 0; c < eb->anim.clip_count && c < 32; c++)
            printf(" %d:%d", c, eb->anim.clips[c].frame_count);
    }
    printf("\n    Start-HP %d  hitbox r=%d\n", (int)e->hp, (int)e->hit_radius_min);

    int letzter_treffer = -1;
    uint8_t prev_hr = e->hit_react;
    for (int t = 0; t < frames; t++) {
        e->x = 0; e->z = 0; e->y = 0;
        face(e, pl); face(pl, e);

        int g    = gates_basis(e);
        int frei = !(e->hit_react & 1u);
        int16_t hp_vor = e->hp;
        uint8_t st_vor = e->state, s1v = e->sub_state_1, s2v = e->sub_state_2, s3v = e->sub_state_3;
        int16_t mo_vor = e->motion; int32_t af_vor = e->anim_frame;
        uint8_t hr_vor = e->hit_react, d3_vor = e->re2z_self1d3;

        int ret = re15_player_weapon_fire(weapon);
        int hit = (ret == SL + 1) && (e->hp != hp_vor);
        if (hit) {
            R.hits++;
            if (R.erster < 0) R.erster = t;
            if (letzter_treffer >= 0 && R.nluecken < 64) R.luecken[R.nluecken++] = t - letzter_treffer;
            letzter_treffer = t;
        }
        if (ausfuehrlich && (hit || t < 4 || (prev_hr & 1u) != (e->hit_react & 1u) ||
                             st_vor != e->state))
            printf("      f%-4d st=%d/%d/%d/%d clip=%d fr=%d hp=%d +93=0x%02x +1D3=0x%02x band=%d "
                   "| gates=%d riegel_frei=%d fire=%d%s\n",
                   t, st_vor, s1v, s2v, s3v, (int)mo_vor, (int)af_vor, (int)hp_vor,
                   hr_vor, d3_vor, (int)e->aim_band, g, frei, ret, hit ? "  <<< TREFFER" : "");
        prev_hr = e->hit_react;

        re15_enemy_ai_run_all(1);
        if (R.gestorben < 0 && (e->state == 3 || e->state == 7)) R.gestorben = t;
        pl->hit_react = 0;
        pl->hp = 100;
    }

    printf("    ERGEBNIS: %d Treffer in %d Bildern; erster Treffer Bild %d; Tod/Leiche ab Bild %d\n",
           R.hits, frames, R.erster, R.gestorben);
    printf("    LUECKEN zwischen aufeinanderfolgenden Treffern (Bilder):");
    if (R.nluecken == 0) printf(" -");
    for (int i = 0; i < R.nluecken; i++) printf(" %d", R.luecken[i]);
    if (R.nluecken) {
        int s = 0, mn = 99999, mx = 0;
        for (int i = 0; i < R.nluecken; i++) {
            s += R.luecken[i];
            if (R.luecken[i] < mn) mn = R.luecken[i];
            if (R.luecken[i] > mx) mx = R.luecken[i];
        }
        printf("   [min %d  max %d  mittel %.1f = %.2f s @30Hz]", mn, mx,
               (double)s / R.nluecken, (double)s / R.nluecken / 30.0);
    }
    printf("\n");
    return R;
}

/* Sperr-Anatomie: EIN Schuss, danach nur ticken - WANN und WOBEI faellt +0x93 Bit 0? */
static void anatomie(const char *titel, uint8_t type, int flavor, int weapon, int32_t dist)
{
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_enemy_reset(); re15_actor_init(); re15_esp_fx_reset(); re15_player_aim_reset();
    int re2 = re15_ai_re2_for_type(type);
    re15_enemy_bank_t *eb = bank_laden(type, re2); (void)eb;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->type = 0; pl->z = dist; pl->hp = 100;
    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = type; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, type);
    face(e, pl); face(pl, e);
    re15_enemy_ai_run_all(1);
    e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
    re15_enemy_ai_run_all(1);
    e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
    re15_player_set_aim_elevation_for_test(0);
    e->hp = 1000;

    printf("\n--- ANATOMIE: %s (Typ 0x%02X, %s, Waffe %d) ---\n",
           titel, type, flav_name(flavor), weapon);
    int ret = re15_player_weapon_fire(weapon);
    printf("    Schuss: fire=%d  -> st=%d/%d/%d/%d clip=%d hp=%d +93=0x%02x +1D3=0x%02x\n",
           ret, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
           (int)e->motion, (int)e->hp, e->hit_react, e->re2z_self1d3);
    if (ret != 2) { printf("    (kein Treffer - Anatomie entfaellt)\n"); return; }

    for (int t = 1; t <= 240; t++) {
        uint8_t hr_vor = e->hit_react;
        e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
        re15_enemy_ai_run_all(1);
        pl->hit_react = 0; pl->hp = 100;
        if ((hr_vor & 1u) && !(e->hit_react & 1u)) {
            printf("    Bild %d nach dem Treffer: +0x93 Bit0 FAELLT -> st=%d/%d/%d/%d clip=%d fr=%d "
                   "+1D3=0x%02x  (= %.2f s @30Hz)\n",
                   t, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                   (int)e->motion, (int)e->anim_frame, e->re2z_self1d3, t / 30.0);
            for (int k = 0; k < 3; k++) {
                e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
                re15_enemy_ai_run_all(1); pl->hit_react = 0; pl->hp = 100;
                printf("       +%d: st=%d/%d/%d/%d clip=%d fr=%d +93=0x%02x\n", k + 1,
                       e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                       (int)e->motion, (int)e->anim_frame, e->hit_react);
            }
            return;
        }
        if ((t % 10) == 0)
            printf("      .. f%-3d st=%d/%d/%d/%d clip=%d fr=%d +93=0x%02x +1D3=0x%02x\n", t,
                   e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                   (int)e->motion, (int)e->anim_frame, e->hit_react, e->re2z_self1d3);
    }
    printf("    +0x93 Bit0 faellt in 240 Bildern NICHT.\n");
}

/* EINZELSCHUSS mit vorgegebener HURT-Zeile (re2z_prev_sub) - trennt die "Zeile-0-Vorlaufzeit"
 * (Schlaf-Treffer-Lane 0x8010321C) vom eigentlichen generischen Treffer-Zyklus P0->P2. */
static void einzel_zeile(const char *titel, int flavor, int weapon, int prev_sub_forciert)
{
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_enemy_reset(); re15_actor_init(); re15_esp_fx_reset(); re15_player_aim_reset();
    int re2 = re15_ai_re2_for_type(0x20);
    bank_laden(0x20, re2);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->type = 0; pl->z = 2000; pl->hp = 100;
    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x20; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x20);
    face(e, pl); face(pl, e);
    for (int i = 0; i < 2; i++) {
        re15_enemy_ai_run_all(1);
        e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
    }
    re15_player_set_aim_elevation_for_test(0);
    e->hp = 1000;
    if (prev_sub_forciert >= 0) e->re2z_prev_sub = (uint8_t)prev_sub_forciert;

    int ret = re15_player_weapon_fire(weapon);
    printf("\n--- %s (Zeile/prev_sub=%d) ---\n", titel, (int)e->re2z_prev_sub);
    if (ret != 2) { printf("    kein Treffer (fire=%d)\n", ret); return; }
    int t;
    for (t = 1; t <= 400; t++) {
        uint8_t hr = e->hit_react;
        e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
        re15_enemy_ai_run_all(1);
        pl->hit_react = 0; pl->hp = 100;
        if ((hr & 1u) && !(e->hit_react & 1u)) {
            printf("    Sperre endet nach %d Bildern (%.2f s @30Hz) -> st=%d/%d/%d/%d clip=%d\n",
                   t, t / 30.0, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                   (int)e->motion);
            return;
        }
    }
    printf("    Sperre haelt >400 Bilder\n");
}

/* BAND-BEWEIS: derselbe Tief-Schuss einmal mit dem Port-Stand (grid_id ohne 0x80 -> generischer
 * LEVEL-Stempel in re15_damage.c) und einmal mit grid_id|0x80 (-> DOWN-Stempel). */
static void band_beweis(void)
{
    for (int variante = 0; variante < 2; variante++) {
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_enemy_reset(); re15_actor_init(); re15_esp_fx_reset(); re15_player_aim_reset();
        bank_laden(0x20, 1);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        memset(pl, 0, sizeof *pl);
        pl->active = 1; pl->type = 0; pl->z = 2000; pl->hp = 100;
        re15_actor_t *e = &g_actors[1];
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x20; e->state = 0; e->em_flag_id = 0xFF;
        re15_enemy_apply_hitbox(e, 0x20);
        face(e, pl); face(pl, e);
        for (int i = 0; i < 2; i++) { re15_enemy_ai_run_all(1);
            e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e); }
        e->hp = 1000;
        if (variante) e->grid_id |= 0x80u;      /* nur um zu zeigen, WELCHES Gate blockt */
        re15_player_set_aim_elevation_for_test(-1);
        int treffer = 0;
        for (int t = 0; t < 60; t++) {
            e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
            int16_t hp0 = e->hp;
            if (re15_player_weapon_fire(3) == 2 && e->hp != hp0) treffer++;
            re15_enemy_ai_run_all(1); pl->hit_react = 0; pl->hp = 100;
        }
        printf("    TIEF-Schuss (elev -1), RE2-Hund, grid_id=0x%02x -> %d Treffer in 60 Bildern\n",
               e->grid_id, treffer);
    }
}

/* SKRIPT-SPAWN (ROOM1190: die drei Hunde stehen als grid 0x40/0x41 in der Luft und fallen
 * ueber die RE1.5-Maschine 4/5/6 herunter). Wird auf den FALLENDEN Hund ueberhaupt getroffen? */
static void skriptspawn(const char *titel, int flavor, uint8_t grid, int32_t y0, int elev)
{
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_enemy_reset(); re15_actor_init(); re15_esp_fx_reset(); re15_player_aim_reset();
    bank_laden(0x20, re15_ai_re2_for_type(0x20));
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->type = 0; pl->z = 2500; pl->hp = 100; pl->floor = 0;
    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x20; e->state = 0; e->em_flag_id = 0xFF;
    e->grid_id = grid; e->y = y0; e->floor = 0;
    re15_enemy_apply_hitbox(e, 0x20);
    face(e, pl); face(pl, e);
    re15_enemy_ai_run_all(1);                  /* INIT -> Skript-Zustand 4 */
    re15_player_set_aim_elevation_for_test(elev);
    e->hp = 2000;
    int erster = -1, treffer = 0, erst_state = -1; int32_t erst_y = 0;
    for (int t = 0; t < 240; t++) {
        e->x = 0; e->z = 0; face(e, pl); face(pl, e);
        int16_t hp0 = e->hp;
        if (re15_player_weapon_fire(3) == 2 && e->hp != hp0) {
            treffer++;
            if (erster < 0) { erster = t; erst_state = e->state; erst_y = e->y; }
        }
        re15_enemy_ai_run_all(1); pl->hit_react = 0; pl->hp = 100;
    }
    printf("    %s: grid 0x%02x y0=%d elev=%d -> erster Treffer Bild %d (state %d, y=%d), "
           "gesamt %d in 240 Bildern\n",
           titel, grid, (int)y0, elev, erster, erst_state, (int)erst_y, treffer);
}

int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("=== HUNDE-TREFFERLUECKE - Dauerfeuer-Messung (Port) ===\n");
    printf("    Bilder = Ticks von re15_enemy_ai_run_all; 30 Hz.\n");

    /* --- A) HUND, beide Geschmaecker, beide Waffen, Elevation LEVEL --------------------- */
    lauf("A1 Hund 0x20 / RE2-KI / Pistole (w3) / LEVEL",   0x20, RE15_AI_FLAVOR_RE2,  3,  0, 2000, 400, 1000, 1);
    lauf("A2 Hund 0x20 / RE1.5-KI / Pistole (w3) / LEVEL", 0x20, RE15_AI_FLAVOR_RE15, 3,  0, 2000, 400, 1000, 1);
    lauf("A3 Hund 0x20 / RE2-KI / Messer (w1) / LEVEL",    0x20, RE15_AI_FLAVOR_RE2,  1,  0,  900, 400, 1000, 0);
    lauf("A4 Hund 0x20 / RE1.5-KI / Messer (w1) / LEVEL",  0x20, RE15_AI_FLAVOR_RE15, 1,  0,  900, 400, 1000, 0);

    /* --- B) Elevations-Sweep ------------------------------------------------------------ */
    lauf("B1 Hund / RE2-KI / Pistole / UNTEN (-1)",   0x20, RE15_AI_FLAVOR_RE2,  3, -1, 2000, 300, 1000, 0);
    lauf("B2 Hund / RE2-KI / Pistole / OBEN (+1)",    0x20, RE15_AI_FLAVOR_RE2,  3, +1, 2000, 300, 1000, 0);
    lauf("B3 Hund / RE1.5-KI / Pistole / UNTEN (-1)", 0x20, RE15_AI_FLAVOR_RE15, 3, -1, 2000, 300, 1000, 0);
    lauf("B4 Hund / RE1.5-KI / Pistole / OBEN (+1)",  0x20, RE15_AI_FLAVOR_RE15, 3, +1, 2000, 300, 1000, 0);

    /* --- C) Gegenprobe ZOMBIE 0x10 ------------------------------------------------------ */
    lauf("C1 Zombie 0x10 / RE2-KI / Pistole / LEVEL",   0x10, RE15_AI_FLAVOR_RE2,  3, 0, 2000, 400, 1000, 0);
    lauf("C2 Zombie 0x10 / RE1.5-KI / Pistole / LEVEL", 0x10, RE15_AI_FLAVOR_RE15, 3, 0, 2000, 400, 1000, 0);

    /* --- D) natuerliche HP -------------------------------------------------------------- */
    lauf("D1 Hund / RE2-KI / Pistole / natuerliche HP",   0x20, RE15_AI_FLAVOR_RE2,  3, 0, 2000, 600, 0, 0);
    lauf("D2 Hund / RE1.5-KI / Pistole / natuerliche HP", 0x20, RE15_AI_FLAVOR_RE15, 3, 0, 2000, 600, 0, 0);

    /* --- E) Anatomie der Sperre --------------------------------------------------------- */
    anatomie("Hund RE2-KI",     0x20, RE15_AI_FLAVOR_RE2,  3, 2000);
    anatomie("Hund RE1.5-KI",   0x20, RE15_AI_FLAVOR_RE15, 3, 2000);
    anatomie("Zombie RE2-KI",   0x10, RE15_AI_FLAVOR_RE2,  3, 2000);
    anatomie("Zombie RE1.5-KI", 0x10, RE15_AI_FLAVOR_RE15, 3, 2000);

    /* --- F) Zeilen-Zerlegung der RE2-Sperre ---------------------------------------------- */
    einzel_zeile("F1 RE2-Hund, HURT-Zeile 0 (Port-Stand: prev_sub=0)",   RE15_AI_FLAVOR_RE2, 3, 0);
    einzel_zeile("F2 RE2-Hund, HURT-Zeile 2 (generische Zeile)",         RE15_AI_FLAVOR_RE2, 3, 2);
    einzel_zeile("F3 RE2-Hund, HURT-Zeile 3 (= Waffen-Id 3 wie RE2)",    RE15_AI_FLAVOR_RE2, 3, 3);

    /* --- G) Welches Gate blockt den Tief-Schuss? ------------------------------------------ */
    printf("\n--- G Band-Beweis (RE2-Hund, Elevation UNTEN) ---\n");
    band_beweis();

    /* --- H) Skript-Spawn (ROOM1190-Muster: Hund haengt in der Luft und faellt) ----------- */
    printf("\n--- H Skript-Spawn (Park-Hoehe, faellt) ---\n");
    skriptspawn("H1 RE2-KI  grid 0x41", RE15_AI_FLAVOR_RE2,  0x41, -10000, 0);
    skriptspawn("H2 RE2-KI  grid 0x40", RE15_AI_FLAVOR_RE2,  0x40,  -3600, 0);
    skriptspawn("H3 RE15-KI grid 0x41", RE15_AI_FLAVOR_RE15, 0x41, -10000, 0);
    skriptspawn("H4 RE15-KI grid 0x40", RE15_AI_FLAVOR_RE15, 0x40,  -3600, 0);
    skriptspawn("H5 RE15-KI grid 0x41 / elev UNTEN", RE15_AI_FLAVOR_RE15, 0x41, -10000, -1);

    printf("\n=== ENDE ===\n");
    return 0;
}
