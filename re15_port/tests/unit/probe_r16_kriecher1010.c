/* probe_r16_kriecher1010.c — MESSUNG (Runde 16, 2026-09-19), keine Behauptung.
 *
 * NUTZER: "ROOM 1010 - schiesse ich die kriechenden Zombies einmal an, werden sie wieder ein
 *          Stueck zurueck gesetzt (befund_1010_F233_marke10). ... kriechen die im Original nicht
 *          direkt los, sondern stellen sich erst einmal tot!"
 *
 * Gemessen wird am ECHTEN Pfad (ROOM1010.RDT, sub00 Case 4 = Suedtuer, scd_room_reenter +
 * scd_vm_tick + re15_game_step, geladene Baenke, Pistolenschuss ueber das Pad R1/SQUARE):
 *   A) TOTSTELLEN: was tut der Kriecher in den ersten Bildern nach dem Spawn, Spieler auf dem
 *      Tuer-Zielpunkt (3650,-3950) — bewegt er sich sofort, oder wartet er?
 *   B) RUECKVERSATZ: der Kriecher kriecht auf den stehenden Spieler zu, wird EINMAL mit der
 *      Pistole getroffen; PRO BILD um den Treffer herum: x/z/y, dx/dz, Zustandswort, Clip,
 *      Bildnummer, die Wurzel-Momentaufnahme (root_prev_*), +0x144 (re2z_root144).
 *   C) Wurzel-Tabelle des Kriech-Clips 5 (RE2-Bank): sx je Bild, damit ein Sprung
 *      "sx(neu) - sx(alt)" nachgerechnet werden kann.
 * Beide Flavors (RE2 = Nutzer-Default, RE1.5 zum Vergleich).
 * KEIN add_test — reine Messsonde.
 *
 * ERGEBNIS (gemessen 2026-09-19, Build build_r16_kriecher1010, RE2-Bank EM010):
 *   A) RE2 : Kriecher startet in Bild 1 in der Lokomotion (st=1/0/1, Clip 5) und legt in 150
 *            Bildern 1631 Einheiten zurueck — KEIN Totstellen. RE1.5: st=1/5/1, Clip 0x0C,
 *            0 Einheiten in 150 Bildern (dist 3510 > 0xBB8) = wartet, wie das Original.
 *   B) RE2 : Treffer bei f21 (Clip 5 Bild 34, Momentaufnahme rp_x=787). HURT Clip 6 (f21-f36,
 *            keine Bewegung, rp_* unveraendert). f37 P2 -> Wort 1. f38 Lokomotion P0: Bild
 *            rand&0xF = 7, move_root rechnet dx = sx(7) - rp_x = -43 - 787 = **-830**:
 *            x 1598 -> 768 IN EINEM BILD. Das ist der Rueckversatz des Nutzers.
 *            Gerader Schuss (ohne D-Pad) traf den Kriecher NICHT; erst mit DOWN.
 *   C) Clip 5 (50 Bilder) sx: 0,-1,-6,...,-51 (f9/10), 0 (f14), 29, 68, ... 790 (f35/36),
 *            ... 728 (f49). Clip 6 (HURT, 20 Bilder) und Clip 23 (Liegen): alle sx = 0. */
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
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_sin_q12(int), re15_cos_q12(int);

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
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_re2_n);
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

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static int32_t s_px, s_pz;
static int s_latch = 0;   /* Teil B: Ein-Angreifer-Riegel Spieler+0x1D3 Bit 0x80 (0x800CFDCB) setzen,
                           * damit der Kriecher NICHT greift (DECIDE[0] @0x80102F4C-58 kehrt um) —
                           * Isolation des Treffer-Pfades, kein Verhaltens-Nachbau */
static void pin_player(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->hp = 100; pl->x = s_px; pl->z = s_pz; pl->y = 0;
    if (s_latch) pl->re2z_self1d3 |= 0x80u;
}

static void line(const char *tag, int f, int slot, int32_t px0, int32_t pz0)
{
    re15_actor_t *e = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t dx = e->x - px0, dz = e->z - pz0;
    printf("%s f%-4d x=%6ld z=%6ld y=%5ld  dx=%5ld dz=%5ld | st=%u/%2u/%u mo=0x%02X fr=%3u "
           "| rp_mo=%d rp_kf=%d rp_x=%d rp_z=%d r144=%d | grid=0x%02X 10E=0x%04X sca=%u hp=%d "
           "react=0x%02X ry=%d dist=%u plx=%ld plz=%ld\n",
           tag, f, (long)e->x, (long)e->z, (long)e->y, (long)dx, (long)dz,
           e->state, e->sub_state_1, e->sub_state_2, (unsigned)e->motion, (unsigned)e->anim_frame,
           (int)e->root_prev_motion, (int)e->root_prev_kf, (int)e->root_prev_x, (int)e->root_prev_z,
           (int)e->re2z_root144, e->grid_id, (unsigned)e->re2z_f10e, e->sca_mask, (int)e->hp,
           e->hit_react, (int)e->rot_y, (unsigned)e->ai_dist, (long)pl->x, (long)pl->z);
}

static void bringup(re15_ai_flavor_t fl, uint8_t cut, int32_t px, int32_t pz)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1010;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; s_px = px; s_pz = pz;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    if (fl == RE15_AI_FLAVOR_RE2) { (void)load_bank_re2(0x10);  (void)load_bank_re2(0x11); }
    else                          { (void)load_bank_re15(0x10); (void)load_bank_re15(0x11); }
    re15_player_set_aim_clip_len(12);
}

/* C) Wurzel-Tabelle eines Clips der geladenen Bank (Haupt-Bank = die, die der Kriecher faehrt) */
static void dump_root_table(uint8_t type, int clip, const char *name)
{
    re15_enemy_bank_t *b = re15_enemy_find(type);
    if (!b || !b->ok) { printf("  (%s: keine Bank)\n", name); return; }
    const re15_emd_animation_t *an = &b->anim; const re15_emd_skeleton_t *sk = &b->skel;
    if (clip >= an->clip_count) { printf("  (%s: clip %d > %d)\n", name, clip, an->clip_count); return; }
    int fc = an->clips[clip].frame_count;
    printf("  %s Clip %d, %d Bilder — Wurzel sx/sz je Bild (Keyframe-Offset, unrotiert):\n", name, clip, fc);
    for (int f = 0; f < fc; f++) {
        int kf = (int)(re15_emd_get_frame_entry(an, clip, f) & 0xFFFu);
        int16_t sx = 0, sy = 0, sz = 0;
        re15_emd_get_keyframe_speed(sk, kf, &sx, &sy, &sz);
        printf("    f%2d kf=%3d sx=%5d sy=%5d sz=%5d\n", f, kf, sx, sy, sz);
    }
}

/* A) Totstellen: Spieler am Tuer-Zielpunkt, 150 Bilder, Bewegung des Kriechers? */
static void part_a(re15_ai_flavor_t fl, const char *name)
{
    printf("\n===== A) %s  Spawn Cut 4, Spieler am Tuer-Ziel (3650,-3950): bewegt sich der Kriecher? =====\n", name);
    bringup(fl, 4, 3650, -3950);
    int slots[8], n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && n < 8; s++) if (g_actors[s].active) slots[n++] = s;
    for (int i = 0; i < n; i++) {
        re15_actor_t *e = &g_actors[slots[i]];
        printf("  spawn slot%d type=0x%02X grid=0x%02X pos=(%ld,%ld) ry=%d st=%u/%u/%u mo=0x%02X 10E=0x%04X\n",
               slots[i], e->type, e->grid_id, (long)e->x, (long)e->z, (int)e->rot_y,
               e->state, e->sub_state_1, e->sub_state_2, (unsigned)e->motion, (unsigned)e->re2z_f10e);
    }
    int32_t x0[8], z0[8];
    for (int i = 0; i < n; i++) { x0[i] = g_actors[slots[i]].x; z0[i] = g_actors[slots[i]].z; }
    for (int f = 0; f < 150; f++) {
        pin_player(); frame(0, 0);
        if (f < 3 || f == 10 || f == 30 || f == 60 || f == 149)
            for (int i = 0; i < n; i++) line("   ", f, slots[i], x0[i], z0[i]);
    }
}

/* B) Rueckversatz: Kriecher slot A kriecht auf den Spieler zu, ein Pistolentreffer, Log je Bild */
static void part_b(re15_ai_flavor_t fl, const char *name, int pre_frames, int use_down)
{
    printf("\n===== B) %s  Treffer auf den kriechenden Zombie (pre=%d, aim-down=%d) =====\n",
           name, pre_frames, use_down);
    /* Spieler VOR Kriecher A (950,-1700, ry=0 = Blick +X): 1500 Einheiten auf +X */
    bringup(fl, 4, 2450, -1700);
    int a = -1, bslot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active) continue;
        if (g_actors[s].x == 950 && a < 0) a = s; else if (bslot < 0) bslot = s;
    }
    if (a < 0) { printf("  FAIL: Kriecher A nicht gefunden\n"); return; }
    if (bslot >= 0) { g_actors[bslot].x = 30000; g_actors[bslot].z = 30000; }   /* isolieren */
    re15_actor_t *e = &g_actors[a];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
    re15_player_set_equipped_weapon(3);                       /* Pistole = RE1.5-Waffe 3 */
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

    int32_t lx = e->x, lz = e->z;
    s_latch = (fl == RE15_AI_FLAVOR_RE2);
    /* Vorlauf: der Kriecher kriecht, der Spieler steht — bis dist < 1200 (Pistolen-Reichweite
     * 1000 + Trefferradius) oder pre_frames erreicht */
    int f = 0;
    for (; f < pre_frames; f++) {
        pin_player(); frame(0, 0);
        if (f % 10 == 0) line(" vor", f, a, lx, lz);
        lx = e->x; lz = e->z;
        if (e->ai_dist < 1200u && f > 5) { line(" vor", f, a, lx, lz); f++; break; }
    }
    /* Zielen: R1 halten, bis aim_ready */
    for (int k = 0; k < 40 && !re15_player_aim_ready(); k++, f++) {
        pin_player(); frame((uint16_t)(RE15_PAD_BIT_R1 | (use_down ? RE15_PAD_BIT_DOWN : 0)), 0);
        lx = e->x; lz = e->z;
    }
    printf("  aim_ready=%d nach f%d, dist=%u, Kriecher st=%u/%u/%u mo=0x%02X fr=%u\n",
           re15_player_aim_ready(), f, (unsigned)e->ai_dist, e->state, e->sub_state_1,
           e->sub_state_2, (unsigned)e->motion, (unsigned)e->anim_frame);
    /* Schuss + 90 Bilder je Bild loggen */
    int hp_last = e->hp, hit_f = -1;
    int32_t hx = 0, hz = 0;
    for (int k = 0; k < 90; k++, f++) {
        pin_player();
        uint16_t cur = (uint16_t)(RE15_PAD_BIT_R1 | (use_down ? RE15_PAD_BIT_DOWN : 0) |
                                  (k == 0 ? RE15_PAD_BIT_SQUARE : 0));
        frame(cur, (uint16_t)(k == 0 ? RE15_PAD_BIT_SQUARE : 0));
        const char *tag = "    ";
        if (e->hp < hp_last && hit_f < 0) { hit_f = f; hx = e->x; hz = e->z; tag = "HIT!"; }
        hp_last = e->hp;
        line(tag, f, a, lx, lz);
        lx = e->x; lz = e->z;
        if (e->hp < 0) { printf("  (tot)\n"); break; }
    }
    if (hit_f >= 0) {
        int32_t dx = e->x - hx, dz = e->z - hz;
        printf("  TREFFER bei f%d an (%ld,%ld); 90 Bilder spaeter (%ld,%ld): Netto dx=%ld dz=%ld "
               "(Spieler bei x=%ld: positiv dx = auf den Spieler zu)\n",
               hit_f, (long)hx, (long)hz, (long)e->x, (long)e->z, (long)dx, (long)dz, (long)pl->x);
    } else printf("  KEIN TREFFER registriert\n");
}

int main(void)
{
    printf("== probe_r16_kriecher1010: Totstellen + Rueckversatz bei Treffer ==\n");
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1010.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1010.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;

    const char *only = getenv("PROBE_ONLY");
    if (!only || strstr(only, "A")) {
        part_a(RE15_AI_FLAVOR_RE2,  "RE2 ");
        part_a(RE15_AI_FLAVOR_RE15, "RE15");
    }
    if (!only || strstr(only, "C")) {
        printf("\n===== C) Wurzel-Tabellen =====\n");
        re15_enemy_reset(); (void)load_bank_re2(0x10);
        dump_root_table(0x10, 5,    "RE2  EM010 Kriech-Clip");
        dump_root_table(0x10, 6,    "RE2  EM010 Kriech-HURT-Clip");
        dump_root_table(0x10, 0x17, "RE2  EM010 Liege-Clip");
    }
    if (!only || strstr(only, "B")) {
        part_b(RE15_AI_FLAVOR_RE2,  "RE2 ", 200, 0);
        part_b(RE15_AI_FLAVOR_RE2,  "RE2 ", 200, 1);
        part_b(RE15_AI_FLAVOR_RE15, "RE15", 200, 0);
    }
    printf("\n(Sonde: reine Messung, kein PASS/FAIL)\n");
    return 0;
}
