/* Verifikations-Sonde (Pruefauftrag tempo-wirkung): misst die ECHTEN Clip-Bildzahlen der
 * RE2-Zombie-Gangclips und vergleicht die Port-Keyframe-Folge Tick fuer Tick mit einem
 * byte-treuen Modell des Originals:
 *   global 0x8002959c -> 0x80029614-Schwanz: 80029b30 addiu +1 / 80029b38 andi 0xff /
 *                        80029b3c sltu fc / 80029b48 sb zero  (Wrap)
 *   Kadenz  @0x80101cd8-d54: Gate, (+0x14D%3)==2, EXTRA-Advance 0x8002a9c8
 *                        (8002a9e8 addiu +1 / 8002aa00 andi 0xff / 8002aa04 sltu fc /
 *                         8002aa18 sb zero)
 * Reihenfolge im Original: 959c (Advance) VOR dem Kadenz-Block; im Port: globaler
 * Advancer (game_step_common.c:1936) VOR re15_enemy_ai_run_all. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_room.h"
#include "re2_ems.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, long *sz)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    if (n <= 0) { fclose(f); return NULL; }
    uint8_t *b = malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f); if (b) *sz = n; return b;
}
static uint8_t *s_ems; static long s_ems_sz;
static void load_re2_bank(const char *base, uint8_t type)
{
    if (!s_ems) { char p[600]; snprintf(p, sizeof p, "%s/../RE2/CDEMD0.EMS", base);
                  s_ems = slurp(p, &s_ems_sz); }
    if (!s_ems) { printf("WARN: CDEMD0.EMS fehlt\n"); return; }
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
}
static void frame_game_order(void) { re15_actors_anim_advance(); re15_enemy_ai_run_all(1); }

int main(void)
{
    const char *base = RE15_ASSET_PSX_DIR;
    char path[600]; long sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", base);
    uint8_t *data = slurp(path, &sz);
    if (!data) { printf("FAIL: %s\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, (size_t)sz, &rdt) != 0) { printf("FAIL: RDT\n"); return 1; }
    re15_actor_init(); re15_aot_init(); scd_vm_init(); re15_enemy_reset();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 30000; pl->y = 0; pl->z = 30000;
    if (rdt.main_scd) scd_thread_start(0, rdt.main_scd);
    scd_thread_start(1, rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_re2_bank(base, 0x10); load_re2_bank(base, 0x11); load_re2_bank(base, 0x16);
    frame_game_order();

    int s11 = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x11) { s11 = s; break; }
    if (s11 < 0) { printf("FAIL: kein Typ-0x11\n"); return 1; }
    re15_actor_t *z = &g_actors[s11];

    /* --- (1) die ECHTEN Bildzahlen --------------------------------------------------- */
    re15_enemy_bank_t *b = re15_enemy_find(0x11);
    printf("== Bank 0x11: ok=%d loco_ok=%d anim.clip_count=%d loco.clip_count=%d\n",
           b?b->ok:-1, b?b->loco_ok:-1, b?b->anim.clip_count:-1, b?b->anim_loco.clip_count:-1);
    for (int c = 0; c < 10 && b && c < b->anim_loco.clip_count; c++)
        printf("   loco clip %d: frame_count=%d first=%d\n", c,
               b->anim_loco.clips[c].frame_count, b->anim_loco.clips[c].first_frame);
    for (int c = 0; c < 10 && b && c < b->anim.clip_count; c++)
        printf("   base clip %d: frame_count=%d first=%d\n", c,
               b->anim.clips[c].frame_count, b->anim.clips[c].first_frame);
    printf("   re2z_walkclip=%d\n", (int)z->re2z_walkclip);

    /* --- (2) Tick-Folge im Port (Gang, EXEC[1]) --------------------------------------- */
    z->re2z_flags21a |= 0x8000u;                 /* Gate an (@0x801008D0) */
    re15_ai_set_state_word(z, 0x101); z->sub_state_2 = 0; z->anim_frame = 0;
    int N = 40;
    int port_af[64], port_mo[64];
    for (int t = 0; t < N; t++) { frame_game_order(); port_af[t] = (int)z->anim_frame;
                                  port_mo[t] = (int)z->motion; }
    int clip = port_mo[0];
    int fc = 0;
    if (b) {
        const re15_emd_animation_t *A = (re15_actor_uses_loco_bank(z) && b->loco_ok)
                                        ? &b->anim_loco : &b->anim;
        if (clip >= 0 && clip < A->clip_count) fc = A->clips[clip].frame_count;
        printf("== Gang-Clip=%d fc=%d (loco_bank=%d)\n", clip, fc,
               re15_actor_uses_loco_bank(z) ? 1 : 0);
    }
    printf("PORT  af: ");
    for (int t = 0; t < N; t++) printf("%d ", port_af[t]);
    printf("\n");

    /* --- (3) byte-treues Modell des Originals ab DEMSELBEN Startwert ------------------ */
    /* Startwert: das +0x14D, das der Port-P0 gesetzt hat (= das Original-Seed rand&0x1f).
     * Original-Tick: 959c-Advance ZUERST, dann Kadenz. */
    if (fc > 0) {
        /* Seed rekonstruieren: der Port liest im P0-Tick den Seed ROH; port_af[0] ist der
         * Wert NACH dem P0-Tick (also seed oder seed+1, je nach Residue). Wir fahren das
         * Modell ab dem Port-Wert von Tick 0 und vergleichen die FOLGE ab Tick 1. */
        int kf = port_af[0];
        printf("ORIG  af: %d ", kf);
        for (int t = 1; t < N; t++) {
            kf = ((kf + 1) & 0xff) < fc ? ((kf + 1) & 0xff) : 0;   /* @0x80029b30-48 */
            if ((kf % 3) == 2) kf = ((kf + 1) & 0xff) < fc ? ((kf + 1) & 0xff) : 0; /* @0x8002a9c8 */
            printf("%d ", kf);
        }
        printf("\n");
        /* Abweichungs-Zaehlung */
        int kf2 = port_af[0], bad = 0;
        for (int t = 1; t < N; t++) {
            kf2 = ((kf2 + 1) & 0xff) < fc ? ((kf2 + 1) & 0xff) : 0;
            if ((kf2 % 3) == 2) kf2 = ((kf2 + 1) & 0xff) < fc ? ((kf2 + 1) & 0xff) : 0;
            if (kf2 != port_af[t]) bad++;
        }
        printf("== Abweichungen Port vs Original-Modell (Tick 1..%d): %d\n", N - 1, bad);
    }

    /* --- (4) EXEC[2] (Bump) Clip 4+walkclip ------------------------------------------ */
    if (b) {
        int c2 = 4 + (int)z->re2z_walkclip;
        const re15_emd_animation_t *A = (re15_actor_uses_loco_bank(z) && b->loco_ok)
                                        ? &b->anim_loco : &b->anim;
        if (c2 < A->clip_count)
            printf("== EXEC[2]-Clip %d: fc=%d  (Seed rand&0x1f = 0..31)\n",
                   c2, A->clips[c2].frame_count);
    }

    /* --- (5) EINTRITTS-TICK (P0): liest der Port den POST-Advance-Wert? ----------------
     * Original @0x80101A40: +0x6==0 -> P0 @0x80101A74 seedet +0x14D = rand&0x1f
     * (@0x80101B0C) und FAELLT DURCH auf @0x80101B24 (= dasselbe Ziel wie der +0x6==1-Zweig
     * @0x80101A64) -> im selben Tick laeuft @0x80101CD0 jal 0x8002959c (Advance, Schwanz
     * @0x80029B30-48), ERST DANN liest der Kadenz-Block @0x80101D00 das Byte.
     * Modelle fuer den Wert am ENDE des P0-Ticks:
     *   f(s) = s   + ((s%3)==2)         (Port: Kadenz sieht den ROHEN Seed)
     *   g(s) = s+1 + (((s+1)%3)==2)     (Original: Kadenz sieht seed+1)
     * f(s)==0 ist nur fuer s==0 moeglich; g(s) ist NIE 0. */
    {
        int hist[70]; memset(hist, 0, sizeof hist);
        int runs = 4000, zero_hits = 0, only_f = 0, only_g = 0;
        int fset[70], gset[70]; memset(fset,0,sizeof fset); memset(gset,0,sizeof gset);
        for (int s = 0; s <= 31; s++) {
            int f = s + (((s % 3) == 2) ? 1 : 0);
            int g1 = s + 1; int g = g1 + (((g1 % 3) == 2) ? 1 : 0);
            if (f < 70) fset[f] = 1;
            if (g < 70) gset[g] = 1;
        }
        for (int r = 0; r < runs; r++) {
            re15_ai_set_state_word(z, 0x101);
            z->sub_state_2 = 0;                 /* +0x6 = 0 -> P0 im naechsten Tick */
            z->re2z_flags21a |= 0x8000u;
            re15_enemy_ai_run_all(1);           /* NUR der KI-Tick (P0 + Kadenz) */
            int v = (int)z->anim_frame;
            if (v >= 0 && v < 70) hist[v]++;
            if (v == 0) zero_hits++;
            if (v < 70 && fset[v] && !gset[v]) only_f++;
            if (v < 70 && gset[v] && !fset[v]) only_g++;
        }
        printf("== P0-Eintritt: %d Laeufe; Werte NUR im Port-Modell f(): %d, NUR im "
               "Original-Modell g(): %d, davon af==0 (g unmoeglich): %d\n",
               runs, only_f, only_g, zero_hits);
        printf("   Histogramm af nach P0-Tick: ");
        for (int v = 0; v < 40; v++) if (hist[v]) printf("%d:%d ", v, hist[v]);
        printf("\n");
    }

    return 0;
}
