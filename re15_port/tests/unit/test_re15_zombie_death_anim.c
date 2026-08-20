/**
 * @file test_re15_zombie_death_anim.c
 * @brief PIN — die RE1.5-Todesanimation des Zombies laeuft MONOTON durch (kein Ruecksprung).
 *
 * Anlass (Nutzer-Spieltest, RE1.5-KI-Modus): "Wenn ich den Zombie erschossen habe, und er stirbt
 * und faellt nach hinten, dass die Animation noch einmal wiederholt wird so ab der Haelfte, bis er
 * dann wirklich umfaellt nach hinten und tot ist." — Wiederholungsfall: derselbe Report kam schon
 * einmal, wurde damals aber nur im RE2-Pfad behoben (f0ece91d, enemy_ai_re2_zombie.c). Der
 * RE1.5-Pfad (enemy_ai_common.c) blieb unangetastet.
 *
 * GEMESSEN VORHER (probe_re15_zdeath, echter Weg game_step + Pad R1/SQUARE, ROOM1140, geladene
 * RE1.5-Bank, Sweep ueber 64 RNG-Seeds): in 4 von 64 Laeufen sprang der GERENDERTE Keyframe des
 * 55-Frame-Sturzclips vom letzten Frame zurueck an den Anfang und der ganze Sturz lief noch einmal:
 *     RUECKSPRUNG f53: mo=11 (0x0b) st=3 fc=55  slot 54 -> 0   (kf 301 -> 274)
 *     RUECKSPRUNG f53: mo=13 (0x0d) st=3 fc=55  slot 54 -> 0   (kf 329 -> 302)
 * Die Zielwerte 0..3 sind exakt das `+0x95 = rand & 3` aus Phase 0 (@0x80106cc4-c8).
 * Quelle war der nachgebaute 1/8-Zweig `if ((rng&7)==0) { +0x6=4; +0x7=0; }` (zitiert auf
 * FUN_80106c18 @0x80106e50-70), der die Phase 0 erneut betrat.
 *
 * DAS ORIGINAL KANN DAS NICHT — vier unabhaengige Belege (Details am Fix in
 * enemy_ai_common.c, re15_enemy_ai_live_death):
 *   (1) @0x80106c40 / @0x80106c44 = 0x00000000 / 0x00000000 — der Eintritt in den
 *       Phase->=2-Block ist weg-genoppt (identisch im Downed-Tod @0x80107cd4/@0x80107cd8).
 *   (2) Xref-Vollscan ueber GANZ STAGE1.BIN: 0 Xrefs auf 0x80106c58 und auf 0x80107cf0.
 *   (3) Ghidra-Decompilat STAGE1_full/FUN_80106c18.c: `if (+0x7 != 0) goto LAB_80106ea0;`.
 *   (4) LIVE-RAM in drei Savestates: dieselben Nullwoerter -> kein Runtime-Patch.
 *   (+) Selbst wenn der Zweig liefe: die Wurzel FUN_80106ba4 loest JEDEN Tick neu ueber
 *       @0x8011feac[+0x5*8 + +0x6] auf, und Spalte 4 ist FUN_80107634 (60..187 Frames Pause,
 *       dann Clip 0x12/0x13) — nie wieder Phase 0 mit dem Sturzclip.
 *
 * DER PIN pinnt den VERLAUF, nicht den Endzustand: ueber einen RNG-SEED-SWEEP (der Fehler trat nur
 * in ~1/8 der Faelle auf — ein Einzellauf haette ihn nicht gefangen, genau deshalb galt der Bug
 * beim ersten Bericht als behoben) und ueber BEIDE Sturzrichtungen (Clip 0x0b und 0x0d) muss der
 * gerenderte Keyframe/Frame-Slot ueber die ganze Todesanimation monoton fortschreiten.
 * Dazu: NEGATIV-KONTROLLE (der zurueckgebaute Zweig wird kuenstlich nachgestellt -> derselbe
 * Detektor MUSS anschlagen) und eine RE2-REGRESSIONSWACHE.
 *
 * MUTATIONSPROBE (durchgefuehrt 2026-08-20, damit der gruene Pin nicht bloss gruen IST):
 * den entfernten Zweig in re15_enemy_ai_live_death wieder einsetzen —
 *   `if (!(e->grid_id & 0x80) && (re15_engine_rand8() & 7) == 0)
 *        { e->sub_state_2 = 4; e->sub_state_3 = 0; return; }`
 * vor dem `e->state = RE15_AI_STATE_CORPSE;` — dann meldet dieser Pin 5 von 48 Laeufen rot,
 * auf BEIDEN Seiten:
 *   FAIL: seed 0x00001000 side 0: 2 Ruecksprung/Neustart ... (Clip 0x0b)
 *   FAIL: seed 0xe377ab90 side 0: 1 ... (Clip 0x0b)
 *   FAIL: seed 0x1fe69f02 side 0: 3 ... (Clip 0x0b)
 *   FAIL: seed 0x36de4be2 side 1: 1 ... (Clip 0x0d)
 *   FAIL: seed 0x4df3ac7f side 1: 1 ... (Clip 0x0d)
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
#include "re15_collision.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_anim_select.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_actor_clip_len(const re15_actor_t *a);
extern int  re15_actor_uses_loco_bank(const re15_actor_t *a);
extern int  re15_actor_toggle_reverse(const re15_actor_t *a);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static re15_rdt_t s_rdt;
static char       s_base[512];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t s_scratch[0x80000];
static void load_bank(uint8_t type)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", s_base);
    uint8_t *ems = slurp(emsp, &ems_size);
    if (!ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0
        && len <= sizeof s_scratch) {
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb && !eb->ok) {
            memcpy(s_scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_scratch, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok  = (re15_emd_parse_own_bank(s_scratch, len, &eb->skel_own, &eb->anim_own) == 0);
            }
        }
    }
    free(ems);
}

/* ==== RENDER-SICHT: derselbe Pfad, den platform/pc/main.c faehrt ==== */
static int rendered_slot(const re15_actor_t *a, int *out_kf, int *out_fc, int *out_loco)
{
    *out_kf = -1; *out_fc = 0; *out_loco = 0;
    re15_anim_banks_t banks; memset(&banks, 0, sizeof banks);
    re15_anim_view_t av;
    re15_actor_anim_select(a, 0, &banks, &av);
    const re15_emd_animation_t *an = av.anim;
    const re15_emd_skeleton_t  *sk = av.skel;
    int co = av.clip_override;
    if (re15_actor_uses_loco_bank(a)) {
        re15_enemy_bank_t *lb = re15_enemy_find(a->type);
        if (lb && lb->loco_ok && (int)a->motion < lb->anim_loco.clip_count) {
            sk = &lb->skel_loco; an = &lb->anim_loco; co = (int)a->motion; *out_loco = 1;
        }
    }
    if (!an || !sk || an->clip_count <= 0) return -1;
    int ci = (co >= 0) ? co : (int)a->motion;
    if (ci < 0) ci = 0;
    if (ci >= an->clip_count) ci = ci % an->clip_count;
    const re15_emd_clip_t *c = &an->clips[ci];
    *out_fc = c->frame_count;
    if (c->frame_count <= 0) return -1;
    *out_kf = re15_compute_actor_kf(an, sk, a, co, (uint32_t)a->anim_frame);
    uint32_t cur = (uint32_t)a->anim_frame;
    int fc = c->frame_count;
    int rev = ((a->anim_flags & 0x80) || re15_actor_toggle_reverse(a)) ? 1 : 0;
    if (a->anim_freeze) return 0;
    if (a->walk_active || co >= 0) {
        uint32_t m = cur % (uint32_t)fc;
        return rev ? (int)((uint32_t)(fc - 1) - m) : (int)m;
    }
    if (rev) {
        uint32_t idx = (cur >= (uint32_t)fc) ? (uint32_t)(fc - 1) : cur;
        return (int)((uint32_t)(fc - 1) - idx);
    }
    if (cur >= (uint32_t)fc) {
        int last = fc - 1;
        while (last > 0 && (an->frames[c->first_frame + last] & 0x8000u)) last--;
        return last;
    }
    return (int)cur;
}

static void tick_once(void)
{
    re15_aot_scan(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0);
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
}

/* Ein Todes-Durchlauf. Rueckgabe: Zahl der Ruecksprünge im gerenderten Frame-Slot waehrend
 * der Todesanimation (state 3), inkl. eines mitten in state 3 NEU gesetzten Sturzclips.
 * mutate != 0 = NEGATIV-KONTROLLE: der entfernte 1/8-Zweig wird von aussen nachgestellt
 * (+0x6 = 4 / +0x7 = 0 am Clip-Ende) — der Detektor MUSS dann anschlagen. */
static int death_run(uint32_t seed, int side, int mutate, int *out_first_clip,
                     int *out_reached_last, int *out_entered_death)
{
    *out_first_clip = -1; *out_reached_last = 0; *out_entered_death = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_damage_seed_rng(seed);
    re15_actor_init(); scd_vm_init(); re15_enemy_reset();
    re15_enemy_ai_set_paused(0);
    re15_aot_init();
    g_current_room_id = 0x1140;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0;
    re15_collision_set_band(0);
    /* Wiedereintritts-Zweig von sub00 -> STEHENDE Zombies (der Erstbesuch hat nur liegende) */
    re15_game_flag_set(3, 0xD2, 1);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    load_bank(0x16); load_bank(0x10); load_bank(0x11);
    for (int f = 0; f < 8; f++) tick_once();

    re15_actor_t *z = NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *c = &g_actors[s];
        if (c->active && (c->type == 0x10 || c->type == 0x11 || c->type == 0x16)
            && !(c->grid_id & 0x80)) { z = c; break; }
    }
    if (!z) return -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (&g_actors[s] != z && g_actors[s].active) g_actors[s].active = 0;

    for (int f = 0; f < 4; f++) tick_once();

    /* Schussposition ERST JETZT setzen (der Zombie laeuft waehrend der Vorlauf-Ticks):
     * Spieler 2600 Einheiten in -X vor ihm, Blickrichtung +X (rot_y = 0 -> cos=4096/sin=0,
     * der Schuss-Streifen zeigt nach +X). */
    pl->x = z->x - 2600; pl->z = z->z; pl->y = z->y; pl->rot_y = 0;
    /* Front/Back-Latch +0x93 & 0x80 (@0x8001a788-94 ueber die HEADINGS) kippen ->
     * Clip 0x0b statt 0x0d (@0x80106ca0-ac). */
    z->rot_y = (int16_t)(side ? ((pl->rot_y + 2048) & 0xfff) : pl->rot_y);

    z->hp = 1; z->hit_react = 0;              /* der naechste Treffer ist der Todesschuss */
    int fired = re15_player_weapon_fire(3);   /* ECHTER Waffenpfad (setzt +0x5/+0x6/+0x7/+0x93) */
    if (!fired) {
        if (getenv("RE15_DEATHPIN_DEBUG"))
            printf("  [dbg] kein Treffer: z typ=0x%02x st=%d s1=%d grid=%02x r=%d hitreact=%02x "
                   "pos(%d,%d,%d) pl(%d,%d,%d) rot=%d\n", z->type, z->state, z->sub_state_1,
                   z->grid_id, (int)z->hit_radius_min, z->hit_react,
                   (int)z->x, (int)z->y, (int)z->z, (int)pl->x, (int)pl->y, (int)pl->z,
                   (int)pl->rot_y);
        return -2;
    }
    if (z->state != 3) return -3;
    *out_entered_death = 1;

    int back = 0, prev_slot = -1, prev_mo = -1, prev_st = -1, prev_loco = -1;
    for (int f = 0; f < 400; f++) {
        tick_once();
        int kf, fc, loco;
        int sl = rendered_slot(z, &kf, &fc, &loco);
        int mo   = (int)z->motion;
        int dcl  = (mo == 0x0b || mo == 0x0d);                 /* Sturzclip? */
        int dphase     = (z->state == 3 || z->state == 7);     /* Tod / Leiche */
        int prev_dphase = (prev_st == 3 || prev_st == 7);
        (void)kf;

        if (*out_first_clip < 0 && dphase && dcl) *out_first_clip = mo;
        if (dcl && fc > 0 && sl >= fc - 1) *out_reached_last = 1;

        /* (a) RUECKSPRUNG im gerenderten Frame-Slot, gleicher Sturzclip, gleiche Bank.
         *     Der Leichen-Settle (motion 21, fc 5) ist per `dcl` ausgeschlossen — der loopt
         *     byte-true und ist NICHT gemeint. */
        if (dcl && dphase && prev_dphase && prev_mo == mo && prev_loco == loco
            && prev_slot >= 0 && sl >= 0 && sl < prev_slot)
            back++;
        /* (b) der Sturzclip wird waehrend Tod/Leiche ERNEUT gesetzt = Wiederholung */
        if (dcl && dphase && prev_dphase && prev_mo >= 0 && prev_mo != mo)
            back++;

        /* NEGATIV-KONTROLLE: exakt das nachstellen, was der entfernte Zweig tat — statt
         * `+0x4 = 7` schrieb er `+0x6 = 4` / `+0x7 = 0` (@0x80106e60-70) und kehrte zurueck,
         * d.h. der naechste Tick lief wieder in Phase 0 und setzte den Sturzclip neu. */
        if (mutate && z->state == 7 && prev_st == 3) {
            z->state = 3; z->sub_state_2 = 4; z->sub_state_3 = 0; mutate = 0;
        }
        prev_slot = sl; prev_mo = mo; prev_st = z->state; prev_loco = loco;
    }
    return back;
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    snprintf(s_base, sizeof s_base, "%s", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", s_base);
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    /* ---- 1) VERLAUFS-PIN: RNG-SEED-SWEEP x BEIDE STURZRICHTUNGEN ---------------------------
     * Der Fehler war 1-aus-8. Ein einzelner Lauf haette ihn verfehlt — deshalb der Sweep. */
    int runs = 0, seen_0b = 0, seen_0d = 0, reached_last = 0;
    for (int side = 0; side <= 1; side++) {
        for (int i = 0; i < 24; i++) {
            uint32_t seed = 0x1000u + (uint32_t)(side * 1000 + i) * 0x9e3779b9u;
            int first = -1, last = 0, entered = 0;
            int back = death_run(seed, side, 0, &first, &last, &entered);
            CHECK(back >= 0, "Sweep seed 0x%08x side %d: Aufbau fehlgeschlagen (%d)", seed, side, back);
            if (back < 0) continue;
            runs++;
            CHECK(entered == 1, "seed 0x%08x side %d: DEATH (state 3) nicht erreicht", seed, side);
            CHECK(first == 0x0b || first == 0x0d,
                  "seed 0x%08x side %d: Todes-Clip ist %d, erwartet 0x0b oder 0x0d (@0x80106ca0-ac)",
                  seed, side, first);
            /* DAS IST DER PIN: kein Ruecksprung im GERENDERTEN Frame-Slot, kein zweites
             * Setzen des Sturzclips waehrend state 3. */
            CHECK(back == 0,
                  "seed 0x%08x side %d: %d Ruecksprung/Neustart in der Todesanimation "
                  "(Clip 0x%02x) — die Animation wird wiederholt", seed, side, back, first);
            if (first == 0x0b) seen_0b++;
            if (first == 0x0d) seen_0d++;
            if (last) reached_last++;
        }
    }
    CHECK(runs >= 40, "zu wenige gueltige Laeufe: %d", runs);
    /* Beide Richtungen muessen im Sweep wirklich vorgekommen sein, sonst pinnt der Test nur eine. */
    CHECK(seen_0b > 0, "Sturzclip 0x0b (Front/Back-Latch +0x93&0x80 gesetzt) kam im Sweep nie vor");
    CHECK(seen_0d > 0, "Sturzclip 0x0d kam im Sweep nie vor");
    /* Die Animation muss auch WIRKLICH bis zum letzten Frame gelaufen sein — sonst waere
     * "kein Ruecksprung" trivial erfuellt (z.B. bei clip_len == 0 ohne geladene Bank). */
    CHECK(reached_last == runs,
          "nur %d von %d Laeufen haben den letzten Frame des Sturzclips erreicht", reached_last, runs);
    printf("Sweep: %d Laeufe, Clip 0x0b %dx, Clip 0x0d %dx, letzter Frame erreicht %dx\n",
           runs, seen_0b, seen_0d, reached_last);

    /* ---- 2) NEGATIV-KONTROLLE: der entfernte Zweig, kuenstlich nachgestellt --------------
     * Stellt sicher, dass der Detektor oben den Fehler ueberhaupt sehen KANN (sonst waere
     * der gruene Pin wertlos). Nachgestellt wird exakt das, was der entfernte Code tat:
     * am Clip-Ende +0x6 = 4 / +0x7 = 0 -> Phase-0-Wiedereintritt -> Sturzclip neu. */
    {
        int first = -1, last = 0, entered = 0;
        int back = death_run(0x1000u, 0, 1, &first, &last, &entered);
        CHECK(back > 0,
              "NEGATIV-KONTROLLE: der nachgestellte 1/8-Zweig (+0x6=4/+0x7=0 @0x80106e60-70) "
              "wurde NICHT als Ruecksprung erkannt (back=%d) — der Pin waere blind", back);
        printf("Negativ-Kontrolle: %d Ruecksprung/Neustart erkannt (erwartet > 0)\n", back);
    }

    /* ---- 3) RE2-REGRESSIONSWACHE ---------------------------------------------------------
     * Im RE2-Modus faehrt der Zombie das RE2-Gehirn (re15_re2z_tick, enemy_ai_re2_zombie.c);
     * die RE1.5-Todesphase darf dort gar nicht laufen. Belegt, dass dieser Fix den RE2-Pfad
     * nicht beruehrt. */
    {
        re15_actor_init(); re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        CHECK(re15_re2z_owns_type(0x10) != 0, "RE2-Modus: Typ 0x10 gehoert nicht dem RE2-Gehirn");
        re15_actor_t *e = &g_actors[2];
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x10; e->hp = -1;
        e->state = 3;                 /* DEATH */
        e->sub_state_3 = 0;           /* Phase 0 — die RE1.5-Phase, die den Sturzclip setzt */
        e->motion = 99;               /* Sentinel: die RE1.5-Phase 0 wuerde 0x0b/0x0d schreiben */
        e->hit_react = 0;
        re15_enemy_ai_live_tick(2);
        CHECK(e->motion != 0x0b && e->motion != 0x0d,
              "RE2-Modus: die RE1.5-Todes-Phase 0 hat den Sturzclip gesetzt (motion=%d) — "
              "der RE2-Pfad wurde beruehrt", (int)e->motion);
        /* und umgekehrt: im RE1.5-Modus MUSS derselbe Aufruf die Phase 0 fahren
         * (Positiv-Kontrolle der Wache). */
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x10; e->hp = -1;
        e->state = 3; e->sub_state_3 = 0; e->motion = 99;
        re15_enemy_ai_live_tick(2);
        CHECK(e->motion == 0x0b || e->motion == 0x0d,
              "RE1.5-Modus: Phase 0 hat den Sturzclip NICHT gesetzt (motion=%d)", (int)e->motion);
        CHECK(e->sub_state_3 == 1, "RE1.5-Modus: +0x7 nach Phase 0 ist %d, erwartet 1 (@0x80106c88)",
              (int)e->sub_state_3);
    }

    free(buf);
    if (fails) { printf("%d FAILS\n", fails); return 1; }
    printf("OK — RE1.5-Todesanimation laeuft monoton durch (Sweep + Negativ-Kontrolle + RE2-Wache)\n");
    return 0;
}
