/* test_re2z_grab_core_se.c — PIN `unit_re2z_grab_core_se`.
 *
 * NUTZER-REPORT 2026-08-24 (RE2-KI-Modus): "die Biss-Sounds des Zombies fehlen" — der Laut,
 * wenn der aufrechte RE2-Zombie Leon packt.
 *
 * BEFUND (Dossier analysis/nutzer_batch_2026-08-25/biss-se.md §3 L1/L2): der BISS-SE des
 * Zombies feuert byte-cadence-korrekt (ENEMSE 3 @0x801028EC -> Mapper -> RE1.5-snd1 SE 3;
 * eigener Beleg-Kommentar an enemy_ai_re2_zombie.c). Was fehlte, ist LEONS Eintritts-Laut der
 * Opfer-/Struggle-Maschine: der Port unterdrueckte ihn im RE2-STEH-Griff (enemy_ai_common.c
 * :710-715 und :2066-2070) mit der Begruendung, die RE2-Steh-Maschine spiele ihn nicht.
 *
 * SOLL — selbst disassembliert:
 *   RE2 STEH-Maschine 0x8010A9B8 (Auswahl @0x8010CF2C[0]/[1]), P0 @0x8010AA50
 *   (info/re2leon/COMMON/BIN/EMOVL10_S0.BIN, RAW @0x80100000):
 *       8010aae0: jal   0x80015fe8     ; rand
 *       8010aae8: lui   a0,0x400
 *       8010aaec: ori   a0,a0,0x1      ; a0 = 0x04000001
 *       8010aaf0: andi  v0,v0,0x1
 *       8010aaf4: sll   v0,v0,17       ; 0 oder 0x20000
 *       8010aaf8: or    a0,v0,a0       ; -> 0x04000001 ODER 0x04020001
 *       8010aafc: jal   0x8005ba28     ; Se_on  => CORE 0 ODER CORE 2  ** MUENZWURF **
 *       8010ab00: addiu a1,s1,56
 *   RE2 KRIECH-Maschine 0x8010AF58 (@0x8010CF34), P0:
 *       8010b05c: ori   a0,a0,0x1      ; a0 = 0x04000001 (FEST, kein Wurf)
 *       8010b060: jal   0x8005ba28
 *   RE1.5 Opfer-Maschine 0x8010A2CC, P0:
 *       8010a3bc: lui   a0,0x400
 *       8010a3e0: ori   a0,a0,0x1      ; a0 = 0x04000001 (FEST, kein Wurf)
 *       8010a3e4: jal   0x80045024
 *   Bank/Record beider Se_on-Zwillinge identisch codiert (RE1.5 FUN_80045024 `srl v1,a0,24`
 *   @0x80045028 + `srl v0,a0,16`/`andi 0xff` @0x80045078-7c; RE2 FUN_8005ba28 @0x8005ba30 +
 *   @0x8005ba7c-80) => 0x04000001 = CORE 0, 0x04020001 = CORE 2.
 *
 * IST vor dem Fix (gemessen, echter game_step, ROOM1140, geladene RE2-Baenke): im gesamten
 * RE2-STEH-Griff NULL CORE-Aufrufe; RE1.5-Griff und RE2-Kriech-Griff je CORE 0.
 *
 * GEPRUEFT WIRD:
 *   (A) END-TO-END RE2, ROOM1140: am Tick des Griff-Eintritts (Opfer-Zustand 0->1) faellt
 *       GENAU EIN CORE-SE, und der ist 0 oder 2.
 *   (B) END-TO-END RE1.5, ROOM1140: derselbe Tick faellt CORE 0 — FEST, nie 2.
 *   (C) MUENZWURF ueber den echten RE2-Shim re15_re2z_victim_begin(behind=0/1): ueber viele
 *       Wuerfe kommen BEIDE Werte vor (0 und 2), und nichts anderes. Rot, sobald der Wurf
 *       @0x8010AAF4 wieder auf eine feste Konstante zusammenfaellt.
 *   (D) KRIECHER (behind=2/3): FEST CORE 0, nie 2 (@0x8010b060 hat keinen Wurf).
 *   (E) REGRESSIONSWACHE: (C) verbraucht Wuerfe aus re15_engine_rand8 — der RE2-Brain-Strom
 *       (re15_re2_rand) darf davon NICHT beruehrt werden.
 * (A) und (C) werden rot, sobald die alte Unterdrueckung (`if (!re2 || variant >= 2)`)
 * zurueckkehrt; (B)/(D) werden rot, sobald der Wurf in die falschen Zweige wandert.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_tim.h"
#include "re15_skeleton.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void     re15_player_aim_reset(void);
extern void     re15_player_set_aim_clip_len(int fc);
extern uint32_t re15_re2_rand(void);
extern uint32_t re15_re2_rand_draws(void);
extern void     re15_re2z_onesave_reset(void);
extern void     re15_player_victim_reset(void);
extern int      g_test_core_se_last, g_test_core_se_count;   /* test_support.c */

static int s_fail = 0;
#define CHECK(cond, ...) do { \
    if (cond) { printf("  OK   " __VA_ARGS__); printf("\n"); } \
    else      { printf("  FAIL " __VA_ARGS__); printf("\n"); s_fail++; } \
} while (0)

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

/* RE1.5-Baenke aus dem eigenen EMS-Container (fuer den RE1.5-Gegenprobe-Lauf) — inkl.
 * Opfer-Bank (dir[3]), ohne die der Opfer-Latch gar nicht erst einsteigt. */
static uint8_t s_blob15[0x80000];
static int load_bank15(uint8_t type)
{
    static uint8_t *ems15 = NULL; static size_t ems15_n = 0;
    if (!ems15) ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &ems15_n);
    if (!ems15) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems15, ems15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob15) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob15, ems15 + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob15, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        eb->type = 0; return 0;
    }
    eb->ok = 1; eb->buf = NULL;
    re15_emd_parse_own_bank(s_blob15, len, &eb->skel_own, &eb->anim_own);
    eb->own_ok  = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob15, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_blob15, len,
                                                &eb->skel_victim, &eb->anim_victim) == 0);
    return 1;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

/* Ein Raum-Setup + Lauf bis zum ersten Griff-Eintritt. Gibt den CORE-SE dieses Ticks zurueck
 * (-1 = kein SE gefallen, -2 = kein Griff im Budget, -3 = Setup fehlgeschlagen).
 * out_delta = Anzahl CORE-SEs GENAU an diesem Tick. */
static int run_until_grab(int re2, int seed, int budget, int *out_delta)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_player_victim_reset();
    re15_re2z_onesave_reset();
    re15_damage_seed_rng(0x0badf00du + (uint32_t)seed * 0x9E3779B9u);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    if (re2) { if (!load_bank2(0x10)) return -3; load_bank2(0x11); load_bank2(0x16); }
    else     { if (!load_bank15(0x10)) return -3; load_bank15(0x11); load_bank15(0x16); }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();

    int zs = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x16) {
            int nib = g_actors[s].grid_id & 0x0f;
            if (zs < 0 && nib != 7 && nib != 8) zs = s;
        }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s != zs && g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x16)
            g_actors[s].active = 0;
    if (zs < 0) return -3;
    re15_actor_t *z = &g_actors[zs];
    {   int ang = (seed * 4096) / 8;
        pl->x = z->x + (int32_t)((900 * (int64_t)re15_cos_q12(ang)) >> 12);
        pl->z = z->z - (int32_t)((900 * (int64_t)re15_sin_q12(ang)) >> 12);
        pl->y = z->y;
        pl->rot_y = (int16_t)((seed * 337 + 700) & 0xfff);
    }

    for (int f = 0; f < budget; f++) {
        pl->floor = z->floor;
        pl->hp = 100;                       /* nur der GRIFF-Eintritt interessiert, kein Tod */
        int vs_prev = re15_player_victim_state();
        int c0 = g_test_core_se_count;
        frame_step();
        int vs = re15_player_victim_state();
        if (vs == 1 && vs_prev != 1) {
            *out_delta = g_test_core_se_count - c0;
            return (*out_delta > 0) ? g_test_core_se_last : -1;
        }
    }
    *out_delta = 0;
    return -2;
}

/* Direkter Wurf-Test ueber den ECHTEN RE2-Shim (der Pfad, den der RE2-Grab P0 nimmt). */
static int shim_core_se(re15_actor_t *z, re15_actor_t *pl, int behind)
{
    re15_player_victim_reset();
    int c0 = g_test_core_se_count;
    re15_re2z_victim_begin(z, pl, behind);
    if (g_test_core_se_count == c0) return -1;
    return g_test_core_se_last;
}

int main(void)
{
    printf("== PIN: RE2-Griff-Eintritts-SE — Muenzwurf CORE 0/2 @0x8010AAE0-AAFC ==\n");

    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) {
        printf("SKIP: ROOM1140.RDT fehlt\n"); return 77;
    }

    /* ---- (A) END-TO-END, RE2 ---------------------------------------------------------- */
    int grabs = 0, bad = 0, seen0 = 0, seen2 = 0, silent = 0, setup_bad = 0;
    for (int seed = 0; seed < 8; seed++) {
        int delta = 0;
        int se = run_until_grab(1, seed, 900, &delta);
        if (se == -3) { setup_bad++; continue; }
        if (se == -2) continue;                       /* kein Griff in diesem Seed */
        grabs++;
        if (se < 0) { silent++; continue; }
        if (se == 0) seen0++; else if (se == 2) seen2++; else bad++;
        if (delta != 1) bad++;
        printf("       (A) seed %d: Griff-Eintritt -> CORE se=%d (SEs an diesem Tick: %d)\n",
               seed, se, delta);
    }
    if (setup_bad == 8) { printf("SKIP: RE2-Bank EM010 / ROOM1140-Spawns fehlen\n"); return 77; }
    CHECK(grabs > 0, "(A) RE2-Steh-Griffe erreicht: %d", grabs);
    CHECK(grabs > 0 && silent == 0,
          "(A) JEDER RE2-Steh-Griff-Eintritt spielt einen CORE-SE: %d von %d stumm "
          "(vor dem Fix: ALLE stumm) [jal 0x8005ba28 @0x8010AAFC]", silent, grabs);
    CHECK(bad == 0, "(A) CORE-SE ist ausschliesslich 0 oder 2, genau einer pro Eintritt "
                    "(%d Verstoesse) [a0 = 0x04000001 | ((rand&1)<<17) @0x8010AAE8-AAF8]", bad);

    /* ---- (B) END-TO-END, RE1.5-Gegenprobe: FEST CORE 0 -------------------------------- */
    int g15 = 0, bad15 = 0, silent15 = 0;
    for (int seed = 0; seed < 6; seed++) {
        int delta = 0;
        int se = run_until_grab(0, seed, 900, &delta);
        if (se == -3 || se == -2) continue;
        g15++;
        if (se < 0) { silent15++; continue; }
        if (se != 0) bad15++;
    }
    CHECK(g15 == 0 || silent15 == 0,
          "(B) RE1.5-Griff-Eintritt spielt CORE 0: %d von %d stumm", silent15, g15);
    CHECK(bad15 == 0,
          "(B) RE1.5 spielt FEST CORE 0, nie CORE 2 (%d Verstoesse) "
          "[`ori a0,a0,0x1` @0x8010A3E0 / `jal 0x80045024` @0x8010A3E4 — KEIN Wurf]", bad15);

    /* ---- (C)/(D)/(E) der Shim direkt: Wurf steh vs. fest kriechend ------------------- */
    {
        /* Ein Setup reicht — geprueft wird der SE, nicht die Bewegung. */
        int delta = 0; (void)run_until_grab(1, 0, 1, &delta);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        re15_actor_t *z  = NULL;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) { z = &g_actors[s]; break; }
        if (!z) { printf("SKIP: kein RE2-Zombie fuer den Shim-Test\n"); return 77; }

        re15_damage_seed_rng(0x13570246u);
        uint32_t re2_draws_before = re15_re2_rand_draws();   /* (E) RE2-Brain-Zugzaehler */

        int st0 = 0, st2 = 0, stx = 0, st_silent = 0;
        for (int i = 0; i < 64; i++) {
            int se = shim_core_se(z, pl, i & 1);     /* 0/1 = STEH (front/hinten) */
            if (se < 0) st_silent++;
            else if (se == 0) st0++;
            else if (se == 2) st2++;
            else stx++;
        }
        int cr0 = 0, cr2 = 0, crx = 0, cr_silent = 0;
        for (int i = 0; i < 64; i++) {
            int se = shim_core_se(z, pl, 2 + (i & 1));  /* 2/3 = KRIECHER */
            if (se < 0) cr_silent++;
            else if (se == 0) cr0++;
            else if (se == 2) cr2++;
            else crx++;
        }
        printf("       (C) STEH   : CORE0 %d  CORE2 %d  andere %d  stumm %d\n",
               st0, st2, stx, st_silent);
        printf("       (D) KRIECH : CORE0 %d  CORE2 %d  andere %d  stumm %d\n",
               cr0, cr2, crx, cr_silent);

        CHECK(st_silent == 0 && stx == 0 && st0 > 0 && st2 > 0,
              "(C) STEH-Griff wuerfelt BEIDE Laute: CORE0 %d / CORE2 %d von 64 "
              "[`jal 0x80015fe8` @0x8010AAE0, `sll v0,v0,17` @0x8010AAF4]", st0, st2);
        CHECK(cr_silent == 0 && crx == 0 && cr2 == 0 && cr0 == 64,
              "(D) KRIECH-Griff spielt FEST CORE 0 (0:%d 2:%d) "
              "[`ori a0,a0,0x1` @0x8010B05C / `jal 0x8005ba28` @0x8010B060 — kein Wurf]",
              cr0, cr2);

        /* (E) der RE2-Brain-Strom darf von den 128 Wuerfen unberuehrt sein */
        uint32_t re2_draws_after = re15_re2_rand_draws();
        CHECK(re2_draws_after == re2_draws_before,
              "(E) RE2-Brain-Strom unberuehrt: %u Zuege vor / %u nach 128 Griff-Eintritten — "
              "der Wurf laeuft bewusst ueber re15_engine_rand8 (Port-Mapping, s. "
              "re15_victim_grab_core_se)",
              (unsigned)re2_draws_before, (unsigned)re2_draws_after);
    }

    re15_player_victim_reset();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf(s_fail ? "\nFAIL: %d Pruefungen rot\n" : "\nOK: alle Pruefungen gruen\n", s_fail);
    return s_fail ? 1 : 0;
}
