/* probe_re15_zdeath.c — MESSUNG (Nutzer-Report 2026-08-20, RE1.5-KI-Modus):
 *   "Wenn ich den Zombie erschossen habe, und er stirbt und faellt nach hinten, dass die
 *    Animation noch einmal wiederholt wird so ab der Haelfte, bis er dann wirklich umfaellt
 *    nach hinten und tot ist."
 *
 * Diese Sonde BEHAUPTET NICHTS. Sie faehrt den ECHTEN Weg — re15_game_step() mit Pad
 * (R1 halten -> SQUARE), echte ROOM1140-Spawns, RE1.5-Flavor, GELADENE RE1.5-Bank
 * (ohne Bank ist re15_actor_clip_len()==0 und jede clip-getriebene Phase endet schon im
 * Setz-Tick = wertlose Messung) — und protokolliert pro Frame den ANIMATIONS-FORTSCHRITT:
 *   motion/Clip, anim_frame, den daraus GERENDERTEN Keyframe + Frame-Slot (derselbe Pfad
 *   wie platform/pc/main.c), Crossfade-Zaehler +0x8f, state/+0x5/+0x6/+0x7 und eine echte
 *   Bone-Weltposition (re15_enemy_bone_world_pos).
 *
 * Der Detektor meldet jeden RUECKSPRUNG des gerenderten Keyframes innerhalb der
 * Todes-/Leichen-Phase (state 3 oder 7) — genau das, was der Spieler als "die Animation
 * wurde noch einmal wiederholt" sieht. Ein reiner State-Log macht so einen Sprung
 * UNSICHTBAR (Lehre aus dem Aim-Release-Fall).
 *
 * Aufruf: probe_re15_zdeath [seed_hex|sweep] [weapon]
 *   sweep  — 64 RNG-Seeds durchfahren, nur Zusammenfassung + jeder Ruecksprung
 *   <hex>  — ein Lauf mit diesem Seed, VOLLES Frame-Protokoll
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
#include "re15_ems.h"
#include "re15_anim_select.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_actor_clip_len(const re15_actor_t *a);
extern int  re15_actor_uses_loco_bank(const re15_actor_t *a);
extern void re15_enemy_bone_world_pos(const re15_actor_t *e, int bone, int32_t out[3]);
extern int  re15_actor_toggle_reverse(const re15_actor_t *a);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static char               s_base[512];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* ---- RE1.5-Bank aus CDEMD0.EMS (wie probe_stagger_fall_1140) ---- */
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

/* ---- RENDER-SICHT: 1:1 der Pfad aus platform/pc/main.c ---- */
static void render_view(const re15_actor_t *a, const re15_emd_animation_t **out_anim,
                        const re15_emd_skeleton_t **out_skel, int *out_co, int *out_loco)
{
    re15_anim_banks_t banks; memset(&banks, 0, sizeof banks);
    re15_anim_view_t av;
    re15_actor_anim_select(a, 0, &banks, &av);
    const re15_emd_animation_t *an = av.anim;
    const re15_emd_skeleton_t  *sk = av.skel;
    int co = av.clip_override, loco = 0;
    if (re15_actor_uses_loco_bank(a)) {
        re15_enemy_bank_t *lb = re15_enemy_find(a->type);
        if (lb && lb->loco_ok && (int)a->motion < lb->anim_loco.clip_count) {
            sk = &lb->skel_loco; an = &lb->anim_loco; co = (int)a->motion; loco = 1;
        }
    }
    *out_anim = an; *out_skel = sk; *out_co = co; *out_loco = loco;
}

static int rendered_slot(const re15_emd_animation_t *an, const re15_emd_skeleton_t *sk,
                         const re15_actor_t *a, int co, int *out_kf, int *out_fc)
{
    *out_kf = -1; *out_fc = 0;
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
    int slot;
    if (a->anim_freeze) slot = 0;
    else if (a->walk_active || co >= 0) {
        uint32_t m = cur % (uint32_t)fc;
        slot = rev ? (int)((uint32_t)(fc - 1) - m) : (int)m;
    } else if (rev) {
        uint32_t idx = (cur >= (uint32_t)fc) ? (uint32_t)(fc - 1) : cur;
        slot = (int)((uint32_t)(fc - 1) - idx);
    } else if (cur >= (uint32_t)fc) {
        int last = fc - 1;
        while (last > 0 && (an->frames[c->first_frame + last] & 0x8000u)) last--;
        slot = last;
    } else slot = (int)cur;
    return slot;
}

static void bringup(uint32_t seed)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(seed);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static int standing_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) return s;
    return -1;
}

/* Ein Lauf. Gibt die Zahl der gefundenen Ruecksprünge zurueck.
 * hp0 = HP, auf die der Zombie VOR dem Schuss gesetzt wird (1 = der Todesschuss trifft
 * ihn STEHEND, so wie im Nutzer-Fall; 0 = HP nicht antasten).
 * side = 0: Spieler steht in -X vor ihm; 1: Spieler dreht sich, damit das Front/Back-
 * Latch +0x93 & 0x80 kippt (Clip 0x0b statt 0x0d). */
static int run_one(uint32_t seed, int weapon, int verbose, int budget, int hp0, int side)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    bringup(seed);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 999; }

    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    load_bank(0x16); load_bank(0x10); load_bank(0x11);

    int slot = standing_zombie();
    if (slot < 0) { printf("FAIL: kein stehender Zombie\n"); return -1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - ((weapon < 3) ? 1200 : 2600);
    pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    if (verbose) {
        re15_enemy_bank_t *b = re15_enemy_find(e->type);
        printf("== Seed 0x%08x  Zombie slot=%d typ=0x%02X hp=%d grid=0x%02X  Bank: ACT=%d LOCO=%d\n",
               seed, slot, e->type, e->hp, e->grid_id,
               b ? b->anim.clip_count : -1, (b && b->loco_ok) ? b->anim_loco.clip_count : -1);
        if (b) printf("   Clip-Laengen: 0x0b=%d 0x0d=%d 0x12=%d 0x13=%d 0x1f=%d\n",
                      b->anim.clips[0x0b].frame_count, b->anim.clips[0x0d].frame_count,
                      b->anim.clips[0x12].frame_count, b->anim.clips[0x13].frame_count,
                      b->anim.clips[0x1f].frame_count);
    }

    if (hp0 > 0) e->hp = (int16_t)hp0;   /* der TODESSCHUSS trifft ihn stehend */
    if (side) e->rot_y = (int16_t)((pl->rot_y + 2048) & 0xfff);

    int hp_last = e->hp, dead_at = -1, back = 0;
    int prev_kf = -1, prev_slot = -1, prev_mo = -1, prev_st = -1, prev_loco = -1;
    int post = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
        /* SCHUSS-FAHRPLAN: alle 12 Frames EIN Schuss, aber KEIN Schuss mehr, sobald der
         * Zombie stirbt — der Nach-Tod-Beschuss ist ein SEPARATER Fall (RE2-Report (e)). */
        if (e->state != 3 && e->state != 7 && (f % 12) == 0) {
            cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE;
        }
        frame(cur, edge);

        int hit = (e->hp < hp_last); hp_last = e->hp;
        if ((e->state == 3 || e->state == 7) && dead_at < 0) dead_at = f;

        const re15_emd_animation_t *an; const re15_emd_skeleton_t *sk; int co, loco, kf, fc;
        render_view(e, &an, &sk, &co, &loco);
        int sl = rendered_slot(an, sk, e, co, &kf, &fc);
        int32_t b0[3]; re15_enemy_bone_world_pos(e, 0, b0);

        int is_death = (e->state == 3 || e->state == 7);
        /* RUECKSPRUNG-Detektor: gleicher Clip, gleiche Bank, gleicher State — und der
         * gerenderte Slot faellt zurueck. (fc-1 -> 0 waere der byte-true Loop; im
         * Todes-Clip gibt es keinen.) */
        if (is_death && prev_kf >= 0 && sl >= 0 && prev_slot >= 0 &&
            prev_mo == (int)e->motion && prev_st == e->state && prev_loco == loco &&
            sl < prev_slot) {
            printf("  >>> RUECKSPRUNG f%d: mo=%d st=%d fc=%d  slot %d -> %d (kf %d -> %d)\n",
                   f, (int)e->motion, e->state, fc, prev_slot, sl, prev_kf, kf);
            back++;
        }
        /* Zusaetzlich: ein NEU GESETZTER Todes-Clip (motion wechselt zurueck auf 0x0b/0x0d,
         * waehrend wir schon in state 3 sind) ist ebenfalls eine Wiederholung. */
        if (is_death && prev_mo >= 0 && (int)e->motion != prev_mo &&
            ((int)e->motion == 0x0b || (int)e->motion == 0x0d) && prev_st == 3) {
            printf("  >>> CLIP-NEUSTART f%d: motion %d -> %d (af=%d) in state %d\n",
                   f, prev_mo, (int)e->motion, (int)e->anim_frame, e->state);
            back++;
        }

        if (verbose && (dead_at >= 0 ? 1 : (f > dead_at))) {
            if (dead_at >= 0 || hit || (f % 4) == 0)
                printf("f%-4d %s st=%u s1=0x%02x s2=%u s3=%u mo=%3d af=%3u frac=%2u flg=%04x "
                       "grid=%02x hp=%5d | %s co=%3d fc=%3d slot=%3d kf=%4d clk=%3d | b0=(%d,%d,%d)\n",
                       f, hit ? "HIT " : "    ", e->state, e->sub_state_1, e->sub_state_2,
                       e->sub_state_3, (int)e->motion, e->anim_frame, e->anim_frac,
                       e->anim_flags, e->grid_id, (int)e->hp,
                       loco ? "LOCO" : "ACT ", co, fc, sl, kf, re15_actor_clip_len(e),
                       b0[0], b0[1], b0[2]);
        }

        prev_kf = kf; prev_slot = sl; prev_mo = (int)e->motion;
        prev_st = e->state; prev_loco = loco;
        if (dead_at >= 0 && ++post > 260) break;
    }
    if (verbose)
        printf("== Ende: tot ab f%d, state=%d s1=0x%02x s2=%u s3=%u mo=%d, %d Ruecksprünge\n",
               dead_at, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
               (int)e->motion, back);
    return back;
}

int main(int argc, char **argv)
{
    const char *arg  = (argc > 1) ? argv[1] : "sweep";
    int weapon       = (argc > 2) ? atoi(argv[2]) : 3;
    const char *base = getenv("RE15_ASSET_DIR");
    snprintf(s_base, sizeof s_base, "%s", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", s_base);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    int side = (argc > 3) ? atoi(argv[3]) : 0;
    if (strcmp(arg, "sweep") == 0) {
        int total = 0, runs = 0;
        for (int i = 0; i < 64; i++) {
            uint32_t seed = 0x1000u + (uint32_t)i * 0x9e3779b9u;
            printf("-- seed 0x%08x\n", seed);
            int b = run_one(seed, weapon, 0, 700, 1, side);
            if (b > 0) { total += b; runs++; }
        }
        printf("\n=== SWEEP: %d von 64 Laeufen mit Ruecksprung/Neustart, %d Ereignisse ===\n",
               runs, total);
        return 0;
    }
    uint32_t seed = (uint32_t)strtoul(arg, NULL, 0);
    run_one(seed, weapon, 1, 700, 1, side);
    free(buf);
    return 0;
}
