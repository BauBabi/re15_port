/* probe_1220_kriecher.c — MESSUNG (keine Behauptung): was tut der Port in ROOM1220 mit den
 * beiden Sce_em_set-Records, die den Deskriptor 0x81 tragen?
 *
 * NUTZER-BEFUND 2026-08-27 (FINDING E): "im ROOM1220 sollten die Zombies eigentlich kriechen.
 * Auch bei RE2-AI."
 *
 * DATENLAGE (eigener Byte-Walk von ROOM1220.RDT; sub_scd-Tabelle @0x0F1C -> sub00 @0x0F20):
 *   0x0F20  Switch work_vars[0x0A]     (= EINTRITTS-CUT der durchschrittenen Tuer)
 *   0x0F24   Case 0:
 *   0x0F2A     Sce_em_set slot=0 type=0x16 beh=0x81 emflag=0x8A pos=(-25000,0, -6700) dirY=0
 *   0x0F3E     Sce_em_set slot=1 type=0x16 beh=0x81 emflag=0x8B pos=(-23500,0, -9700) dirY=3072
 *   0x0F54   Case 2: slots 2/3  beh=0x02  emflag 0xE4/0xE5
 *   0x0F84   Case 4: slots 4/5  beh=0x02  emflag 0xE6/0xE7
 *   0x0FB4   Case 6: slots 6/7  beh=0x02  emflag 0xE8/0xE9
 *   0x0FE4   Case 8: slots 8/9  beh=0x02  emflag 0xEA/0xEB
 *   0x1014  Eswitch  (KEIN Default -> jeder andere Eintritts-Cut spawnt GAR NICHTS)
 * NUR Case 0 traegt Kriecher; die acht uebrigen sind per Datenstand aufrecht (beh 0x02) —
 * das ist der Auslieferungsstand, kein Defekt.
 * Die Tuer, die Cut 0 liefert: ROOM1210.RDT main00 @0x1CE6 Door_aot_set slot 1 ->
 *   stage 0 / room 0x22 / CUT 0 / Ziel (-22400,0,-6500) — 2598 Einheiten von Kriecher 0,
 *   also innerhalb der Weck-Schwelle 0xBB8 = 3000 (`sltiu v0,v0,0xbb8` @0x80104718).
 *
 * ORIGINAL-KETTE fuer beh 0x81 (STAGE1.BIN, selbst disassembliert):
 *   +0x09 = 0x81            (Sce_em_set FUN_800420a0, Deskriptor -> Entity+0x9)
 *   INIT  @0x80100ca4-b0    Gate `lbu v1,9(v0); andi v0,v1,0x80; beq -> 0x80100e30`
 *   INIT  @0x80100d3c-58    sel {1,3} -> +0x94 = 0x0C
 *   INIT  @0x80100d64-68    sel {1,3} -> +0x05 = 5
 *   Tick  @0x8011f80c[+0x9 & 0xf = 1] = FUN_80101708 = GRID-WURZEL 1 (die Kriech-Maschine)
 *   DECIDE  @0x8011F8E0[5] = 0x8010466C     ANIMATE @0x8011F920[5] = 0x80104808
 *   Kriech-Lokomotion @0x8010371C sb 0x1a,148(v1) / @0x8010374C sb 8,471(v1)
 *   Typ 0x16 haengt an DERSELBEN Wurzel wie 0x10: `sw v0,11268(at)` @0x8011e8a4 ->
 *   0x80072C04 = Tabelle[0x16], v0 = 0x80100424.
 *
 * ERGEBNIS DIESER SONDE (2026-08-27): Fall A zeigt, dass die Kriech-Kette fuer Typ 0x16 in
 * BEIDEN Flavors laeuft — der v0.3.23-Fix wirkt hier. Der eigentliche Defekt lag woanders:
 * Fall D/E belegen, dass der Port den Tod der ROOM1210-WRITHER (Typ 0x1A) persistierte und
 * damit ROOM1220s Kriecher aus dem Spawn nahm (gleiche em-Status-Indizes 0x8A/0x8B).
 * Gefixt in enemy_ai_common.c, gepinnt von tests/unit/test_writher_kill_flag.c.
 *
 * KEIN add_test — reine Messsonde, ctest-Zahl bleibt unveraendert. */
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
#include "re15_ems.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static re15_rdt_t         s_rdt;
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

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

static uint8_t *s_re15_ems = NULL; static size_t s_re15_n = 0;
static uint8_t  s_blob[0x80000];
static int load_bank_re15(uint8_t type)
{
    if (!s_re15_ems) s_re15_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_re15_n);
    if (!s_re15_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_re15_ems, s_re15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob, s_re15_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        eb->type = 0; return 0;
    }
    eb->ok = 1; eb->buf = NULL;
    re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own);
    eb->own_ok = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    return 1;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static int is_zombie(const re15_actor_t *e)
{
    return e->active && re15_re2z_owns_type(e->type);
}

static void dump(const char *tag, int frame)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!is_zombie(e)) continue;
        printf("   %s f%-4d slot%-2d type=0x%02X  +04state=%u +05=%2u +06=%2u  +09grid=0x%02X"
               "  +94clip=0x%02X +95=%3u  +1D7sca=%u  +1C4=0x%04X  +10E=0x%04X  hp=%d"
               "  pos=(%6ld,%6ld) ry=%4d  dist=%u\n",
               tag, frame, s, e->type, e->state, e->sub_state_1, e->sub_state_2,
               e->grid_id, e->motion, (unsigned)e->anim_frame, e->sca_mask,
               (unsigned)e->anim_flags, (unsigned)e->re2z_f10e, (int)e->hp,
               (long)e->x, (long)e->z, (int)e->rot_y, (unsigned)e->ai_dist);
    }
}

/* Wenn gesetzt: VOR scd_room_reenter die em-Status-Flags dieser Indizes (Zone 7) setzen —
 * das ist genau der Zustand, den der Port herstellt, sobald der Spieler die beiden
 * ROOM1210-Writher (Typ 0x1A, kill-Flags 0x8A/0x8B, ROOM1210.RDT main00 @0x1D86/@0x1D9A)
 * erschossen hat. ROOM1220s Kriecher tragen DIESELBEN Indizes (@Datei 0x0F31 / 0x0F45). */
static int s_preset_flags[8];
static int s_preset_n = 0;

static void run(re15_ai_flavor_t fl, const char *name, uint8_t entry_cut,
                int32_t px, int32_t pz, int frames)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1220;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz;
    re15_collision_set_band(0);
    re15_inv_load_briefing();

    for (int k = 0; k < s_preset_n; k++)
        re15_game_flag_set(7, (uint8_t)s_preset_flags[k], 1);   /* em-Status-Zone 7 (Stage 0..2) */

    scd_room_reenter(&s_rdt, pl->x, pl->z, entry_cut);

    if (fl == RE15_AI_FLAVOR_RE2) { (void)load_bank_re2(0x16);  (void)load_bank_re2(0x10); }
    else                          { (void)load_bank_re15(0x16); (void)load_bank_re15(0x10); }

    printf("\n=== %s  Eintritts-Cut=%u  Spieler=(%ld,%ld) ===\n",
           name, (unsigned)entry_cut, (long)px, (long)pz);
    printf("  -- direkt nach dem Spawn (vor jedem Tick):\n");
    dump(name, -1);

    int marks[] = { 0, 1, 2, 5, 30, 120, 300, 600 };
    int mi = 0;
    for (int f = 0; f < frames; f++) {
        frame_step();
        if (mi < (int)(sizeof marks / sizeof marks[0]) && f == marks[mi]) {
            dump(name, f);
            mi++;
        }
    }
    printf("  -- Endstand nach %d Frames:\n", frames);
    dump(name, frames);
}

int main(void)
{
    printf("== ROOM1220: was macht der Port aus den beh-0x81-Records (Case 0)? ==\n");

    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1220.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1220.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;

    /* A: DER FALL DES NUTZERS — Eintritt ueber CUT 0 (ROOM1210 main00 @0x1CE6 Door slot 1,
     * Ziel (-22400,0,-6500)). ROOM1220 sub00 Case 0 @Datei 0x0F24 spawnt die BEIDEN
     * Sce_em_set-Records mit beh 0x81 (Datei 0x0F2D / 0x0F41), type 0x16.
     * Kriecher 0 steht auf (-25000,-6700): Abstand zum Tuerpunkt ~2608 < 0xBB8 = 3000. */
    run(RE15_AI_FLAVOR_RE15, "RE1.5-cut0", 0, -22400, -6500, 700);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2,  "RE2-cut0",   0, -22400, -6500, 700);

    /* B: Kontrolle — Eintritt ueber CUT 2 (ROOM1210 @0x1D06, Ziel (-16600,0,-9900)).
     * Case 2 spawnt beh 0x02 = AUFRECHT. Zeigt, dass der Switch-Pfad greift. */
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE15, "RE1.5-cut2", 2, -16600, -9900, 300);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2,  "RE2-cut2",   2, -16600, -9900, 300);

    /* C: Spieler DIREKT VOR Kriecher 0. Der Record @0x0F2A hat dirY=0; im Port ist
     * rot_y=0 = Blick auf +X, also den Spieler auf +X davorsetzen, damit er im
     * Original-Arc +-0x200 (@0x8010472C) liegt. */
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE15, "RE1.5-vorne", 0, -24000, -6700, 700);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2,  "RE2-vorne",   0, -24000, -6700, 700);

    /* D: DER VERDACHT — der Spieler hat in ROOM1210 die beiden Writher (Typ 0x1A) mit den
     * kill-Flags 0x8A/0x8B erschossen. Der Port setzt das em-Status-Flag fuer JEDEN Aktor,
     * der Zustand 7 (CORPSE) erreicht (enemy_ai_common.c:11502-11503) — auch fuer den
     * Writher, dessen Original-Todesast (@0x8010d474/@0x8010d4c4, Commit `sw 7 -> +0x4`
     * @0x8010d5bc) NIRGENDS `jal 0x8004ef90` enthaelt (eigener Byte-Scan ueber die ganze
     * STAGE1.BIN: 17 Treffer, KEINER im Writher-Bereich 0x8010c16c..0x8010d7f8).
     * Erwartung, wenn der Verdacht stimmt: ROOM1220 Case 0 spawnt GAR NICHTS mehr. */
    re15_enemy_reset();
    s_preset_flags[0] = 0x8A; s_preset_flags[1] = 0x8B; s_preset_n = 2;
    run(RE15_AI_FLAVOR_RE15, "RE1.5-cut0-flags8A8B", 0, -22400, -6500, 60);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2,  "RE2-cut0-flags8A8B",   0, -22400, -6500, 60);
    s_preset_n = 0;

    /* E: setzt der Port das Flag beim Writher-Tod wirklich? ROOM1210 laden, den Writher in
     * Slot 1 (kill-Flag 0x8A) toeten und danach Flag 0x8A in Zone 7 lesen. */
    {
        size_t n2 = 0;
        uint8_t *b2 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &n2);
        static re15_rdt_t rdt2;
        if (b2 && re15_rdt_parse(b2, n2, &rdt2) == 0) {
            memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
            s_ctx.rdt = &rdt2; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
            g_room_rdt = rdt2; g_room_rdt_ok = 1; s_rdt = rdt2;
            re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
            re15_actor_init(); re15_aot_init(); scd_vm_init();
            re15_enemy_reset(); re15_enemy_ai_set_paused(0);
            re15_player_cmd_reset(); re15_damage_seed_rng(0x2545f491u);
            g_current_room_id = 0x1210;
            re15_actor_t *pl2 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl2->active = 1; pl2->type = 0; pl2->hp = 100; pl2->hit_react = 0;
            pl2->state = 0; pl2->motion = 0; pl2->floor = 0; pl2->y = 0;
            pl2->x = -25000; pl2->z = -8000;
            re15_collision_set_band(0);
            scd_room_reenter(&rdt2, pl2->x, pl2->z, 0);
            printf("\n=== E: ROOM1210 Writher-Tod setzt em-Status-Flag? ===\n");
            printf("  Flag(7,0x8A) vor allem: %d   Flag(7,0x8B): %d\n",
                   re15_game_flag_get(7, 0x8A), re15_game_flag_get(7, 0x8B));
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                re15_actor_t *e = &g_actors[s];
                if (!e->active) continue;
                printf("  slot%-2d type=0x%02X em_flag_id=0x%02X hp=%d pos=(%ld,%ld)\n",
                       s, e->type, e->em_flag_id, (int)e->hp, (long)e->x, (long)e->z);
            }
            /* den Writher mit em_flag_id 0x8A in den Todesast schicken (Zustand 3 = DEATH
             * 0x8010d474; sein Commit setzt +0x4 = 7 = CORPSE @0x8010d5bc). */
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                re15_actor_t *e = &g_actors[s];
                if (e->active && e->type == 0x1A && e->em_flag_id == 0x8A) {
                    e->hp = -1; e->state = 3; e->sub_state_3 = 0;
                    printf("  -> slot%d (Typ 0x1A, Flag 0x8A) in Zustand 3 (DEATH) versetzt\n", s);
                    break;
                }
            }
            for (int f = 0; f < 240; f++) frame_step();
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                re15_actor_t *e = &g_actors[s];
                if (e->active && e->type == 0x1A && e->em_flag_id == 0x8A)
                    printf("  nach 240 Frames: slot%d state=%u  Flag(7,0x8A)=%d\n",
                           s, e->state, re15_game_flag_get(7, 0x8A));
            }
            printf("  ERGEBNIS Flag(7,0x8A) = %d  (1 = der Port persistiert den Writher-Tod)\n",
                   re15_game_flag_get(7, 0x8A));
        } else printf("\n(E: ROOM1210.RDT nicht ladbar)\n");
    }

    printf("\n(Sonde: reine Messung, kein PASS/FAIL)\n");
    return 0;
}
