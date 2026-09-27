/* ============================================================================================
 * probe_r30b_muendung - Runde 30 / Welle 2: DIE MUENDUNGSHOEHE, gemessen
 *
 * Welle 1 (analysis/befunde_2026-09-27/zielfenster-messung.md) hat das fuenfte Gate von
 * FUN_800470C0 @0x8004716C-A4 gelesen und den Einbau ABGELEHNT, weil dem Port die Zielhoehe
 * fehlte: re15_player_gunbone_world haengt am PC-Renderer und war headless 0/1200 gueltig.
 *
 * Diese Sonde misst das neue ENGINE-Gegenstueck re15_player_muzzle_world (re15_damage.c) -
 * Bone 11 aus Leons eigener PL00-Bank, ueber re15_skel_bone_to_world in die Welt gedreht,
 * dieselbe Kette wie @0x80042E60-94 (0 -> 9 -> 10 -> 11, Stride 0xAC) mit t[1] @sp+56.
 *
 * Gemessen wird auf dem ECHTEN Weg (re15_game_step + Pad, echte RDTs, RE2-KI-Geschmack,
 * RE2-Bank shared_assets/RE2/CDEMD0.EMS geladen - ohne sie haben alle Clips Laenge 0).
 * ============================================================================================ */
#include "re15_emd.h"
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
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void    re15_player_aim_reset(void);
extern void    re15_player_set_aim_clip_len(int fc);
extern int     re15_player_aim_ready(void);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);
extern void    re15_esp_fx_reset(void);
extern int     re15_player_gunbone_world(int32_t ox, int32_t oy, int32_t oz, int32_t out[3]);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_room_id = 0x1140;
static int                s_fire_sub = -1;
static uint8_t           *s_keep = NULL;

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

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (s_fire_sub >= 0 && s_rdt.sub_scd_count > s_fire_sub && s_rdt.sub_scd[s_fire_sub])
        scd_thread_start(2, s_rdt.sub_scd[s_fire_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static int load_room(const char *sub, int room_id, int fire_sub)
{
    char path[600];
    const char *base = getenv("RE15_ASSET_DIR");
    snprintf(path, sizeof path, "%s/%s", (base && *base) ? base : RE15_ASSET_PSX_DIR, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) return 0;
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { free(buf); return 0; }
    s_room_id = room_id; s_fire_sub = fire_sub;
    if (s_keep) free(s_keep);
    s_keep = buf;
    return 1;
}

static void track(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int32_t dx = e->x - pl->x, dz = e->z - pl->z;
    int64_t q  = (int64_t)dx*dx + (int64_t)dz*dz;
    double  d  = q > 0 ? __builtin_sqrt((double)q) : 0.0;
    if (d > 1.0) {
        pl->x = e->x - (int32_t)((double)dx / d * back);
        pl->z = e->z - (int32_t)((double)dz / d * back);
    } else { pl->x = e->x - back; pl->z = e->z; }
    /* GLEICHE EBENE wie in probe_r29: der Spieler wird auf die Bodenhoehe des Gegners
     * gesetzt. OHNE diese Zeile bleibt pl->y auf 0, waehrend die RE2-Gegner je nach Raum
     * auf -530..-5400 stehen — dann misst die Sonde den Teleport-Versatz der Sonde selbst
     * und nicht die Spielgroesse (erster Lauf 2026-09-27 lief genau da hinein). */
    pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

static uint16_t elev_pad_for(const re15_actor_t *e)
{
    if (e->type == 0x21) {
        if (e->aim_band == 4) return RE15_PAD_BIT_UP;
        if (e->aim_band == 1) return RE15_PAD_BIT_DOWN;
        return 0;
    }
    if (e->grid_id & 0x80) return RE15_PAD_BIT_DOWN;
    return 0;
}

/* RE2-Bank (CDEMD0.EMS) — OHNE sie haben alle Clips Laenge 0 und jede Animation ist nach
 * EINEM Bild "zu Ende" (Falle aus Runde 29). */
static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (s_ems2 && re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
}

static int find_type(uint8_t type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active || g_actors[s].type != type) continue;
        if (type >= 0x10 && type <= 0x18 && (g_actors[s].grid_id & 0x80)) continue;
        return s;
    }
    return -1;
}

static int setup_target(uint8_t type, int weapon, int baby_force)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    load_re2_bank(type);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    if (baby_force) {
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x26)
                g_actors[s].re2s_baby_spawned = 1;
    }
    for (int f = 0; f < 60; f++) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        if (baby_force)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == 0x26)
                    g_actors[s].re2s_baby_spawned = 1;
        frame(0, 0);
    }
    int slot = find_type(type);
    if (slot < 0) return -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    if (type == 0x20 && g_actors[slot].state == 4 && g_actors[slot].grid_id == 0x40)
        g_actors[slot].grid_id = 0x43;
    return slot;
}

static void aim_up(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, back);
    for (int f = 0; f < 60 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);
    }
}

/* ===== Das Tor als reine Rechnung (kein Einbau) ========================================== */
/* DURCH <=> (uint32)(eY + b + 100 + h - zielY) < (uint32)(2*(h+100))   @0x8004717C-A4 */
static int tor(int32_t eY, int b, int h, int32_t zielY)
{
    uint32_t lhs = (uint32_t)(eY + b + 100 + h - zielY);
    uint32_t rhs = (uint32_t)(2 * (h + 100));
    return lhs < rhs;
}

typedef struct { const char *name; uint8_t type; int b, h; const char *quelle; } box_t;
static const box_t BOXEN[] = {
  { "ZOMBIE 0x10 stehend", 0x10, -1500, 1500, "EMZ0 INIT @0x8010095C/64" },
  { "ZOMBIE 0x10 Kriecher",0x10,  -350,  350, "EMZ0 @0x80100B14-20 / @0x80103460-6C" },
  { "HUND   0x20 stehend", 0x20, -1000, 1000, "EMD0G INIT @0x8010028C-9C / F80104088(1) @0x801040B8-C4" },
  { "HUND   0x20 liegend", 0x20,  -500,  500, "FUN_80104088(0) @0x80104098-A8" },
  { "KRAEHE 0x21",         0x21,  -350,  530, "EMOVL21_S0 @0x801003B8-DC" },
  { "SPINNE 0x25",         0x25,     0, 1400, "EMS25 Tab @0x801063A0+20/24, +0x98=0 @0x8010276C" },
  { "BABY   0x26",         0x26,   -10,   10, "EMS26 @0x80100168-78 (einziger Schreiber)" },
};


/* ===== TEIL 1: Muendungshoehe + Tor-Urteil je Typ auf dem echten Weg ====================== */
typedef struct {
    const char *tag; int ok, seen, gueltig;
    int32_t mY_min, mY_max;
    int32_t hg_min, hg_max;
    int     durch_s;
    int     b, h;
} mess_t;

static void pass_muendung(mess_t *r, const char *tag, uint8_t type, int b, int h,
                          const char *room, int room_id, int fire_sub, int weapon,
                          int baby_force, int budget)
{
    memset(r, 0, sizeof *r); r->tag = tag; r->b = b; r->h = h;
    r->mY_min = r->hg_min =  0x7fffffff;
    r->mY_max = r->hg_max = -0x7fffffff;
    if (!load_room(room, room_id, fire_sub)) {
        printf("  [%s] FEHLLAUF: Raum %s fehlt - sagt NICHTS\n", tag, room); return; }
    int slot = setup_target(type, weapon, baby_force);
    if (slot < 0) {
        printf("  [%s] FEHLLAUF: kein Gegner Typ 0x%02X in %04X - sagt NICHTS\n",
               tag, type, room_id); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;
    aim_up(slot, back);
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 400) e->hp = 30000;
        if (!e->active) break;
        track(slot, back);
        int schuss = (f >= 20);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e) | (schuss ? RE15_PAD_BIT_SQUARE : 0u)),
              (uint16_t)((schuss && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        r->seen++;
        int32_t m[3];
        if (!re15_player_muzzle_world(m)) continue;
        r->gueltig++;
        if (m[1] < r->mY_min) r->mY_min = m[1];
        if (m[1] > r->mY_max) r->mY_max = m[1];
        int32_t hg = e->y - m[1];
        if (hg < r->hg_min) r->hg_min = hg;
        if (hg > r->hg_max) r->hg_max = hg;
        if (tor(e->y, b, h, m[1])) r->durch_s++;
    }
    r->ok = (r->seen > 0);
    printf("  [%-12s] %3d Bilder | Muendung gueltig %3d/%3d | MuendungY %7d..%7d | "
           "Hgun %6d..%6d | Tor(b=%d,h=%d) DURCH %3d/%3d\n",
           tag, r->seen, r->gueltig, r->seen, r->mY_min, r->mY_max,
           r->hg_min, r->hg_max, b, h, r->durch_s, r->gueltig);
}

/* ===== TEIL 2: die Hunde-Kette, Tor je Bild gegen die ECHTE Muendung ====================== */
static int s_kette = -1, s_pause = -1, s_sperr_von = -1, s_sperr_bis = -1, s_durch_vor = 0;
static int s_stand_durch = 0, s_stand_bilder = 0, s_nach_durch = 0, s_nach_bilder = 0;

static void pass_dog_chain(int budget)
{
    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) { printf("  FEHLLAUF: ROOM1190 fehlt\n"); return; }
    int slot = setup_target(0x20, 3, 0);
    if (slot < 0) { printf("  FEHLLAUF: kein Hund 0x20 in 1190 - sagt NICHTS\n"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    aim_up(slot, 2000);
    e->re2z_self1d3 = 0;
    int getroffen = -1;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 400) e->hp = 30000;
        if (!e->active) break;
        track(slot, 2000);
        int schuss = (getroffen < 0);
        frame((uint16_t)(RE15_PAD_BIT_R1 | (schuss ? RE15_PAD_BIT_SQUARE : 0u)),
              (uint16_t)((schuss && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        int32_t m[3]; int mok = re15_player_muzzle_world(m);
        int low = e->re2z_self1d3 & 0x7f;
        int durch = mok ? tor(e->y, e->re2_hit_b98, (int)e->re2_hit_h9e, m[1]) : -1;
        if (getroffen < 0) {
            s_stand_bilder++; if (durch == 1) s_stand_durch++;
            if (low > 0) { getroffen = f; s_durch_vor = s_stand_durch;
                printf("      TREFFER f%d: 1D3=%d st=%d clip=%d eY=%d MuendungY=%d Hgun=%d Box %d/%d Tor=%s\n",
                       f, low, e->state, e->motion, e->y, mok ? m[1] : 0,
                       mok ? (e->y - m[1]) : 0, e->re2_hit_b98, (int)e->re2_hit_h9e,
                       durch == 1 ? "DURCH" : durch == 0 ? "SPERRT" : "n/a"); }
            continue; }
        int k = f - getroffen;
        if (durch == 0) { if (s_sperr_von < 0) s_sperr_von = k; s_sperr_bis = k; }
        if (s_kette >= 0) { s_nach_bilder++; if (durch == 1) s_nach_durch++; }
        if (k <= 80)
            printf("      +%-2d 1D3=%2d | st=%d/%u | clip=%2d f=%2d | Box %d/%d | Hgun %6d | Tor=%s%s%s\n",
                   k, low, e->state, e->sub_state_1, e->motion, e->anim_frame,
                   e->re2_hit_b98, (int)e->re2_hit_h9e, mok ? (e->y - m[1]) : 0,
                   durch == 1 ? "DURCH " : durch == 0 ? "SPERRT" : "n/a",
                   (low == 0 && s_pause < 0) ? "   <== PAUSE ENDE" : "",
                   (e->state != 2 && s_kette < 0) ? "   <== WIEDER ACTIVE" : "");
        if (low == 0 && s_pause < 0) s_pause = k;
        if (e->state != 2 && s_kette < 0) s_kette = k;
        if (s_pause >= 0 && s_kette >= 0 && k > s_kette + 20) break;
    }
    printf("  HUND: Pause endete +%d | HURT-Kette endete +%d | Tor SPERRT von +%d bis +%d\n",
           s_pause, s_kette, s_sperr_von, s_sperr_bis);
    printf("  STEHEND VOR DEM TREFFER: Tor DURCH in %d/%d Bildern\n", s_durch_vor, s_stand_bilder);
    printf("  NACH DEM AUFSTEHEN:      Tor DURCH in %d/%d Bildern\n", s_nach_durch, s_nach_bilder);
}

/* ======================================================================================== */
static re15_emd_skeleton_t  s_pl00_skel;
static re15_emd_animation_t s_pl00_anim;

int main(int argc, char **argv)
{
    const int riegel = (argc > 1 && strcmp(argv[1], "riegel") == 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    size_t esz = 0, rsz = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    if (!edd || !emr) { printf("SKIP: PL00.EDD/EMR fehlt\n"); return 77; }
    if (re15_emd_parse_animation(edd, esz, &s_pl00_anim) != 0 ||
        re15_emd_parse_skeleton (emr, rsz, &s_pl00_skel) != 0) {
        printf("FAIL: PL00-Parse\n"); return 1; }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    printf("PL00: %d Bones, %d Clips (Bone 11 = Waffen-Bone, Kettenende @0x80042E94)\n",
           s_pl00_skel.bone_count, s_pl00_anim.clip_count);
    if (s_pl00_skel.bone_count <= 11) { printf("FAIL: PL00 hat keinen Bone 11\n"); return 1; }

    printf("\n=== TEIL 1: MUENDUNGSHOEHE + TOR-URTEIL JE TYP (echter Weg) ===\n");
    mess_t m[6];
    pass_muendung(&m[0], "ZOMBIE 0x10", 0x10, -1500, 1500, "STAGE1/ROOM1140.RDT", 0x1140, -1, 3, 0, 240);
    pass_muendung(&m[1], "HUND 0x20",   0x20, -1000, 1000, "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0, 240);
    pass_muendung(&m[2], "KRAEHE 0x21", 0x21,  -350,  530, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 3, 0, 240);
    pass_muendung(&m[3], "SPINNE 0x25", 0x25,     0, 1400, "STAGE2/ROOM2000.RDT", 0x2000, -1, 3, 0, 240);
    pass_muendung(&m[4], "BABY 0x26",   0x26,   -10,   10, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 1, 240);

    printf("\n=== TEIL 2: DIE HUNDE-KETTE, TOR GEGEN DIE ECHTE MUENDUNG ===\n");
    pass_dog_chain(400);

    printf("\n=== URTEIL ===\n");
    int mues_ok = (m[1].gueltig > 0);
    printf("  Muendung headless verfuegbar: %s (%d/%d Bilder beim Hund)\n",
           mues_ok ? "JA" : "NEIN", m[1].gueltig, m[1].seen);
    printf("  Hgun beim Hund: %d..%d  (Fenster stehend [-100,2100), liegend [-100,1100))\n",
           m[1].hg_min, m[1].hg_max);
    for (int i = 0; i < 5; i++)
        if (m[i].ok)
            printf("  %-12s Tor DURCH %3d/%3d -> %s\n", m[i].tag, m[i].durch_s, m[i].gueltig,
                   m[i].durch_s == 0 ? "DAUERHAFT GESPERRT (Gate fuer diesen Typ NICHT bauen)"
                                     : "Kandidat bleibt erreichbar");
    if (!riegel) return 0;

    int fail = 0;
    if (!mues_ok) { printf("RIEGEL-FAIL: Muendung headless nicht verfuegbar\n"); fail = 1; }
    if (s_kette < 0 || s_pause < 0) { printf("RIEGEL-FAIL: Hunde-Kette nicht gemessen\n"); fail = 1; }
    printf(fail ? "PROBE-FAIL\n" : "PROBE-OK\n");
    return fail;
}
