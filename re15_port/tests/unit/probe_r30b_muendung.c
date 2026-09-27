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
extern int     re15_player_aim_clip(void);
extern int     re15_player_aim_muzzle_world(int32_t out[3]);
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

/* Raumliste fuer die Suche nach einer LEBENDEN 0x25 — von Teil 1 und Teil 4 geteilt. */
static const struct { const char *sub; int id; } SPINNENRAUM[] = {
    { "STAGE1/ROOM1090.RDT", 0x1090 }, { "STAGE2/ROOM2000.RDT", 0x2000 },
    { "STAGE2/ROOM2010.RDT", 0x2010 }, { "STAGE2/ROOM2020.RDT", 0x2020 },
    { "STAGE2/ROOM2040.RDT", 0x2040 }, { "STAGE2/ROOM2050.RDT", 0x2050 },
    { "STAGE2/ROOM2060.RDT", 0x2060 }, { "STAGE1/ROOM10D0.RDT", 0x10d0 },
    { "STAGE1/ROOM1260.RDT", 0x1260 }, { "STAGE3/ROOM3010.RDT", 0x3010 } };

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

/* ===== TEIL 3: DER BEFUND SELBST — wird der LIEGENDE Hund noch GETROFFEN? ================
 * Gemessen werden ECHTE Treffer (HP-Abzug je Bild), nicht die Tor-Formel. Zwei Laeufe:
 *   "ALT"  = re2_hit_box_set jedes Bild auf 0 zurueckgesetzt -> das Gate ist inert, also
 *            exakt der Code-Pfad von VOR dieser Runde (kein Env-Schalter im Spielcode).
 *   "NEU"  = so, wie der Port jetzt laeuft.
 * Der Hund wird dauerhaft beschossen; gezaehlt wird getrennt nach STEHEND (state != 2),
 * LIEGEND (state == 2, also waehrend der HURT-Kette) und NACH DEM AUFSTEHEN. */
typedef struct { int treffer_stehend, treffer_liegend, treffer_danach;
                 int bilder_liegend, bilder_danach, kette; } befund_t;

static void pass_befund(befund_t *r, int alt, int budget)
{
    memset(r, 0, sizeof *r); r->kette = -1;
    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) { printf("  FEHLLAUF: ROOM1190 fehlt\n"); return; }
    int slot = setup_target(0x20, 3, 0);
    if (slot < 0) { printf("  FEHLLAUF: kein Hund 0x20 in 1190 - sagt NICHTS\n"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    aim_up(slot, 2000);
    e->re2z_self1d3 = 0;
    int getroffen = -1;          /* Bild, in dem der Hund niedergeht (state -> 2) */
    int steht_wieder = -1;       /* erstes Bild nach der Kette mit state != 2 */
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        e->hp = 30000;                         /* der Hund soll die ganze Kette ueberleben */
        if (!e->active) break;
        /* ⛔ ALT-HEBEL, RUNDE 34 NACHGEZOGEN (Harnisch, keine Schranke).
         * Bis Runde 33 sperrte den liegenden Hund das FUENFTE TOR ueber +0x98/+0x9E; ALT
         * machte es inert, indem es re2_hit_box_set nullte. Seit Runde 34 sperrt ihn die
         * HALTUNGSKLASSE (word0>>26)&7 im Applier-Zwilling re15_re2_gun_probe — das alte
         * Nullen wirkt dort nicht mehr, und ALT und NEU lieferten gemessen dieselbe Zeile
         * (beide 0 Treffer im Liegen). Der Hebel muss also am NEUEN Mechanismus angreifen:
         * ALT = die Klasse bleibt jedes Bild auf 3 (`lui v1,0xc00` @0x801040CC, der
         * "volle Box"-Zweig), FUN_80104088(0) mit seinem `and 0xE7FFFFFF` @0x80104090-B4
         * wird also zurueckgenommen. Das ist exakt der Zustand von VOR der Sperre.
         * Zusaetzlich bleibt das alte Nullen stehen — damit misst ALT beide Wege inert. */
        if (alt) { e->re2_hit_box_set = 0; e->re2z_parts = 3u; }
        track(slot, 2000);
        /* Erst ab Bild 10 schiessen, damit die STEHENDE Phase messbar ist (Gegenprobe
         * gegen Ueberkorrektur: der stehende Hund muss weiter getroffen werden). */
        int schuss = (f >= 10);
        frame((uint16_t)(RE15_PAD_BIT_R1 | (schuss ? RE15_PAD_BIT_SQUARE : 0u)),
              (uint16_t)((schuss && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        int hit = (e->hp < 30000);
        if (getroffen < 0) {
            /* Der Niederschlag-Treffer zaehlt als Treffer am STEHENDEN Hund — genau er ist
             * die Gegenprobe gegen Ueberkorrektur (der Hund stand, als der Schuss kam). */
            if (hit) r->treffer_stehend++;
            if (e->state == 2) getroffen = f;
            continue;
        }
        if (steht_wieder < 0 && e->state != 2) steht_wieder = f;
        if (steht_wieder < 0) { r->bilder_liegend++; if (hit) r->treffer_liegend++; }
        else                  { r->bilder_danach++;  if (hit) r->treffer_danach++;
                                if (r->bilder_danach >= 60) break; }
    }
    if (getroffen >= 0 && steht_wieder >= 0) r->kette = steht_wieder - getroffen;
    printf("  [%s] Treffer am STEHENDEN Hund: %d | Niederschlag bei f%d, "
           "wieder auf den Beinen nach %d Bildern | "
           "TREFFER waehrend er LIEGT: %d in %d Bildern | nach dem Aufstehen: %d in %d Bildern\n",
           alt ? "ALT" : "NEU", r->treffer_stehend, getroffen, r->kette,
           r->treffer_liegend, r->bilder_liegend, r->treffer_danach, r->bilder_danach);
}

/* ===== TEIL 4: KEIN TYP DAUERHAFT UNTREFFBAR ============================================
 * Fuer JEDEN der fuenf RE2-Typen: faellt mit dem neuen Gate noch Schaden? */
static int pass_treffbar(const char *tag, uint8_t type, const char *room, int room_id,
                         int fire_sub, int weapon, int baby_force, int budget)
{
    if (!load_room(room, room_id, fire_sub)) {
        printf("  [%-12s] FEHLLAUF: Raum %s fehlt - sagt NICHTS\n", tag, room); return -1; }
    int slot = setup_target(type, weapon, baby_force);
    if (slot < 0) {
        printf("  [%-12s] FEHLLAUF: kein Gegner Typ 0x%02X in %04X - sagt NICHTS\n",
               tag, type, room_id); return -1; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;
    aim_up(slot, back);
    int treffer = 0, box_set = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100; e->hp = 30000;
        if (!e->active) break;
        track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e) | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        if (e->re2_hit_box_set) box_set = 1;
        if (e->hp < 30000) treffer++;
    }
    printf("  [%-12s] Treffer %3d in %3d Bildern | re2_hit_box_set=%d -> %s\n",
           tag, treffer, budget, box_set,
           treffer > 0 ? "treffbar" : "⛔ NIE GETROFFEN");
    return treffer;
}

/* ===== TEIL 5: OHNE Y-KLAMMER — steht der Spieler im Raum von selbst auf der Hundeebene? =
 * ⛔ Alle Messungen oben setzen `pl->y = e->y` (track()). Das ist noetig, damit die Sonde den
 * Gegner ueberhaupt trifft, aber es KOENNTE den Befund erzeugen, statt ihn zu messen. Dieser
 * Durchgang klammert nur X/Z und laesst pl->y in Ruhe — so, wie der Spieler im Raum steht. */
static void pass_ohne_yklammer(int budget)
{
    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) { printf("  FEHLLAUF: ROOM1190 fehlt\n"); return; }
    int slot = setup_target(0x20, 3, 0);
    if (slot < 0) { printf("  FEHLLAUF: kein Hund 0x20 in 1190 - sagt NICHTS\n"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    aim_up(slot, 2000);
    int32_t hg_min = 0x7fffffff, hg_max = -0x7fffffff;
    int32_t dy_min = 0x7fffffff, dy_max = -0x7fffffff;
    int durch_steh = 0, gueltig = 0, treffer = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100; e->hp = 30000;
        if (!e->active) break;
        /* NUR X/Z klammern — pl->y bleibt, wo der Raum ihn hat. */
        {   int32_t dx = e->x - pl->x, dz = e->z - pl->z;
            int64_t q = (int64_t)dx*dx + (int64_t)dz*dz;
            double d = q > 0 ? __builtin_sqrt((double)q) : 0.0;
            if (d > 1.0) { pl->x = e->x - (int32_t)((double)dx / d * 2000);
                           pl->z = e->z - (int32_t)((double)dz / d * 2000); }
            pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff); }
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        int32_t m[3];
        if (e->y - pl->y < dy_min) dy_min = e->y - pl->y;
        if (e->y - pl->y > dy_max) dy_max = e->y - pl->y;
        if (!re15_player_muzzle_world(m)) continue;
        gueltig++;
        int32_t hg = e->y - m[1];
        if (hg < hg_min) hg_min = hg;
        if (hg > hg_max) hg_max = hg;
        if (tor(e->y, -1000, 1000, m[1])) durch_steh++;       /* STEHENDE Box */
        if (e->hp < 30000) treffer++;
    }
    printf("  OHNE Y-Klammer: eY-plY %d..%d | Hgun %d..%d | Tor(stehende Box) DURCH %d/%d | "
           "echte Treffer %d\n", dy_min, dy_max, hg_min, hg_max, durch_steh, gueltig, treffer);
}

/* ======================================================================================== */
static re15_emd_skeleton_t  s_pl00_skel;
static re15_emd_animation_t s_pl00_anim;
/* Runde 32: die AKTIVE Waffen-Bank. Alle Messlaeufe dieser Sonde fahren Waffe 3, also
 * PL00W03 — dieselbe Bank, die der PC-Renderer beim Zielen aufsetzt (main.c:7423-7431).
 * OHNE sie posiert die Engine die Bindpose, und die Sonde misst 1666 statt der Zielpose. */
static re15_emd_skeleton_t  s_w03_skel;
static re15_emd_animation_t s_w03_anim;

/* MESSUNG: Muendungshoehe ueber den Fuessen je Zielband (HOCH / EBEN / TIEF), auf dem
 * echten Weg gefahren. Das Band kommt aus den Pad-Bits (player_common.c:1034), die Pose
 * aus der W-Bank ueber re15_player_aim_clip(). */
static int32_t s_bandhoehe[3] = { -1, -1, -1 };   /* HOCH / EBEN / TIEF, gemessen */
static void pass_bandhoehen(void)
{
    static const struct { const char *tag; uint16_t bit; } BAND[3] = {
        { "HOCH", RE15_PAD_BIT_UP }, { "EBEN", 0 }, { "TIEF", RE15_PAD_BIT_DOWN } };
    if (!load_room("STAGE1/ROOM1140.RDT", 0x1140, -1)) {
        printf("  FEHLLAUF: ROOM1140 fehlt - sagt NICHTS\n"); return; }
    for (int i = 0; i < 3; i++) {
        int slot = setup_target(0x10, 3, 0);
        if (slot < 0) { printf("  FEHLLAUF: kein 0x10 in 1140\n"); return; }
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->hp = 100;
        re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
        track(slot, 2000);
        int32_t lo = 0x7fffffff, hi = -0x7fffffff, halten = -1; int n = 0, clip = -1;
        for (int f = 0; f < 90; f++) {
            pl->hp = 100; track(slot, 2000);
            frame((uint16_t)(RE15_PAD_BIT_R1 | BAND[i].bit), 0);
            int32_t m[3];
            if (!re15_player_aim_muzzle_world(m)) continue;
            int32_t hoehe = pl->y - m[1];             /* Hoehe ueber den Fuessen */
            if (f >= 40) { if (hoehe < lo) lo = hoehe; if (hoehe > hi) hi = hoehe;
                           halten = hoehe; clip = re15_player_aim_clip(); n++; }
        }
        s_bandhoehe[i] = halten;
        printf("  [%s] Clip %2d | Muendung ueber den Fuessen %d..%d (haltend %d) | %d Bilder\n",
               BAND[i].tag, clip, lo, hi, halten, n);
    }
}

/* MESSUNG: wuerde die ZIELPOSE die heute ungegateten Typen oeffnen? Reine Arithmetik des
 * fuenften Tores @0x8004717C-A4 gegen die drei gemessenen Halte-Hoehen. Kein Einbau. */
static void pass_zielpose_urteil(void)
{
    static const char *TAG[3] = { "HOCH", "EBEN", "TIEF" };
    printf("  %-24s %-20s %s\n", "Box", "Fenster Hgun", "DURCH bei HOCH/EBEN/TIEF");
    for (unsigned i = 0; i < sizeof BOXEN / sizeof BOXEN[0]; i++) {
        const box_t *B = &BOXEN[i];
        char urteil[80]; urteil[0] = 0;
        for (int k = 0; k < 3; k++) {
            int d = (s_bandhoehe[k] > 0) ? tor(0, B->b, B->h, -s_bandhoehe[k]) : -1;
            snprintf(urteil + strlen(urteil), sizeof urteil - strlen(urteil), "%s%s=%s",
                     k ? " " : "", TAG[k], d < 0 ? "?" : (d ? "JA" : "nein"));
        }
        printf("  %-24s [%5d,%5d)        %s\n", B->name,
               -(B->b + B->h + 100), B->h - B->b + 100, urteil);
    }
}

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
    {   size_t we = 0, wr = 0;
        uint8_t *wedd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00W03.EDD", &we);
        uint8_t *wemr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00W03.EMR", &wr);
        static re15_emd_skeleton_t w03_raw;
        if (wedd && wemr &&
            re15_emd_parse_animation(wedd, we, &s_w03_anim) == 0 &&
            re15_emd_parse_skeleton (wemr, wr, &w03_raw) == 0) {
            /* Die W-EMR traegt KEINE Hierarchie und KEINE Bind-Offsets (bones_table = 0,
             * reiner Keyframe-Strom). Die Knochen kommen aus PL00, die Keyframes aus der
             * W-Bank - exakt die Komposition, die der PC-Renderer baut
             * (platform/pc/main.c:3534-3538) und die das Original im Teile-Pool +0x198
             * stehen hat. Ohne sie posiert man PL00-Bind mit W-Keyframes falsch gepaart
             * und misst Unsinn (erster Lauf 2026-09-27: Muendung 15821 UNTER den Fuessen). */
            s_w03_skel = s_pl00_skel;
            s_w03_skel.keyframe_data       = w03_raw.keyframe_data;
            s_w03_skel.keyframe_data_size  = w03_raw.keyframe_data_size;
            s_w03_skel.keyframe_count      = w03_raw.keyframe_count;
            s_w03_skel.keyframe_size_bytes = w03_raw.keyframe_size_bytes;
            s_ctx.w_skel = &s_w03_skel;
            s_ctx.w_anim = &s_w03_anim;
            printf("PL00W03: %d Bones (aus PL00), %d Clips, %d Keyframes (Zielpose-Bank, Waffe 3)\n",
                   s_w03_skel.bone_count, s_w03_anim.clip_count, s_w03_skel.keyframe_count);
        } else {
            printf("WARN: PL00W03 fehlt - die Sonde misst dann die BINDPOSE, nicht die Zielpose\n");
        } }
    printf("PL00: %d Bones, %d Clips (Bone 11 = Waffen-Bone, Kettenende @0x80042E94)\n",
           s_pl00_skel.bone_count, s_pl00_anim.clip_count);
    if (s_pl00_skel.bone_count <= 11) { printf("FAIL: PL00 hat keinen Bone 11\n"); return 1; }

    printf("\n=== TEIL 0: MUENDUNGSHOEHE JE ZIELBAND (die Zielpose) ===\n");
    pass_bandhoehen();

    printf("\n=== TEIL 0b: WUERDE DIE ZIELPOSE DIE UNGEGATETEN TYPEN OEFFNEN? ===\n");
    pass_zielpose_urteil();

    printf("\n=== TEIL 1: MUENDUNGSHOEHE + TOR-URTEIL JE TYP (echter Weg) ===\n");
    mess_t m[6];
    pass_muendung(&m[0], "ZOMBIE 0x10", 0x10, -1500, 1500, "STAGE1/ROOM1140.RDT", 0x1140, -1, 3, 0, 240);
    pass_muendung(&m[1], "HUND 0x20",   0x20, -1000, 1000, "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0, 240);
    pass_muendung(&m[2], "KRAEHE 0x21", 0x21,  -350,  530, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 3, 0, 240);
    /* Runde 31: ROOM2000 traegt im Port KEINE lebende 0x25 — Teil 1 meldete dort nur
     * "FEHLLAUF ... sagt NICHTS", weshalb die 0x25-Zeile im Welle-2-Dossier FEHLTE.
     * Teil 4 sucht die Spinne ueber eine Raumliste; hier wird DIESELBE Suche gefahren
     * und der gefundene Raum ausgegeben, damit die Muendungshoehe fuer 0x25 endlich
     * gemessen ist. Reine MESS-Aenderung, keine Spiellogik. */
    {   const char *sp = NULL; int spid = 0;
        for (unsigned i = 0; i < sizeof SPINNENRAUM / sizeof SPINNENRAUM[0]; i++) {
            if (!load_room(SPINNENRAUM[i].sub, SPINNENRAUM[i].id, -1)) continue;
            if (setup_target(0x25, 3, 0) >= 0) { sp = SPINNENRAUM[i].sub; spid = SPINNENRAUM[i].id; break; } }
        if (sp) { printf("  (0x25 gefunden in %s)\n", sp);
                  pass_muendung(&m[3], "SPINNE 0x25", 0x25, 0, 1400, sp, spid, -1, 3, 0, 240); }
        else      printf("  [SPINNE 0x25 ] FEHLLAUF: kein Raum mit lebender 0x25 - sagt NICHTS\n"); }
    pass_muendung(&m[4], "BABY 0x26",   0x26,   -10,   10, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 1, 240);

    printf("\n=== TEIL 2: DIE HUNDE-KETTE, TOR GEGEN DIE ECHTE MUENDUNG ===\n");
    pass_dog_chain(400);

    printf("\n=== TEIL 3: DER BEFUND — ECHTE TREFFER AM LIEGENDEN HUND ===\n");
    befund_t alt, neu;
    pass_befund(&alt, 1, 400);
    pass_befund(&neu, 0, 400);

    printf("\n=== TEIL 5: GEGENPROBE OHNE Y-KLAMMER ===\n");
    pass_ohne_yklammer(200);

    printf("\n=== TEIL 4: KEIN TYP DAUERHAFT UNTREFFBAR ===\n");
    int tb[5];
    tb[0] = pass_treffbar("ZOMBIE 0x10", 0x10, "STAGE1/ROOM1140.RDT", 0x1140, -1, 3, 0, 200);
    tb[1] = pass_treffbar("HUND 0x20",   0x20, "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0, 200);
    tb[2] = pass_treffbar("KRAEHE 0x21", 0x21, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 3, 0, 200);
    {   const char *sp = NULL; int spid = 0;
        for (unsigned i = 0; i < sizeof SPINNENRAUM / sizeof SPINNENRAUM[0]; i++) {
            if (!load_room(SPINNENRAUM[i].sub, SPINNENRAUM[i].id, -1)) continue;
            if (setup_target(0x25, 3, 0) >= 0) { sp = SPINNENRAUM[i].sub; spid = SPINNENRAUM[i].id; break; } }
        tb[3] = sp ? pass_treffbar("SPINNE 0x25", 0x25, sp, spid, -1, 3, 0, 200) : -1;
        if (!sp) printf("  [SPINNE 0x25 ] FEHLLAUF: kein Raum mit lebender 0x25 - sagt NICHTS\n"); }
    tb[4] = pass_treffbar("BABY 0x26",   0x26, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 1, 200);

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
    /* ===== RIEGEL RUNDE 32: DIE ZIELPOSE IST GEBAUT UND FESTGENAGELT ======================
     * re15_player_aim_muzzle_world posiert die aktive Waffen-Bank mit dem Clip des Bandes
     * +0x154 (Tabelle @0x80011010 = {0,14,10,0,12}, `srl v0,v0,13` @0x80042D08) ueber die
     * Kette @0x80042E60-94. Die drei Halte-Hoehen sind damit keine freien Zahlen mehr:
     * 2751 / 2500 / 1988 kommen unabhaengig auch aus der Vorwaertskinematik ausserhalb des
     * Ports (analysis/befunde_2026-09-27/zielpose_fk.py auf PL00.EMR + PL00W03), und RE2s
     * eigene Bank PL00W02 liegt mit 2805 / 2504 / 1921 um 54 / 4 / 67 daneben.
     * Faellt einer dieser Werte, ist entweder die Bank-Wahl oder die Clip-Wahl kaputt. */
    {   static const struct { const char *tag; int32_t soll; } SOLL[3] = {
            { "HOCH", 2751 }, { "EBEN", 2500 }, { "TIEF", 1988 } };
        for (int i = 0; i < 3; i++)
            if (s_bandhoehe[i] != SOLL[i].soll) {
                printf("RIEGEL-FAIL: Zielpose %s = %d, erwartet %d (Bank/Clip-Wahl kaputt)\n",
                       SOLL[i].tag, s_bandhoehe[i], SOLL[i].soll); fail = 1; }
        if (s_bandhoehe[1] <= s_bandhoehe[2] || s_bandhoehe[0] <= s_bandhoehe[1]) {
            printf("RIEGEL-FAIL: Zielpose nicht monoton HOCH>EBEN>TIEF (%d/%d/%d)\n",
                   s_bandhoehe[0], s_bandhoehe[1], s_bandhoehe[2]); fail = 1; }
    }
    if (!mues_ok) { printf("RIEGEL-FAIL: Muendung headless nicht verfuegbar\n"); fail = 1; }
    if (s_kette < 0 || s_pause < 0) { printf("RIEGEL-FAIL: Hunde-Kette nicht gemessen\n"); fail = 1; }
    /* DER BEFUND: am ALTEN Stand wird der liegende Hund getroffen, am NEUEN nicht mehr. */
    if (alt.treffer_liegend <= 0) {
        printf("RIEGEL-FAIL: ALT-Lauf traf den liegenden Hund gar nicht - der Riegel waere\n"
               "             am alten Stand nicht rot gewesen und beweist nichts\n"); fail = 1; }
    if (neu.treffer_liegend != 0) {
        printf("RIEGEL-FAIL: der liegende Hund ist immer noch treffbar (%d Treffer)\n",
               neu.treffer_liegend); fail = 1; }
    if (neu.treffer_stehend <= 0) {
        printf("RIEGEL-FAIL: der STEHENDE Hund wird nicht mehr getroffen - Ueberkorrektur\n");
        fail = 1; }
    if (neu.treffer_danach <= 0) {
        printf("RIEGEL-FAIL: nach dem Aufstehen wieder treffbar? NEIN (%d) - das waere die\n"
               "             Runde-14-Dauersperre\n", neu.treffer_danach); fail = 1; }
    /* GEGEN-UEBERKORREKTUR: kein RE2-Typ darf dauerhaft untreffbar werden. */
    for (int i = 0; i < 5; i++)
        if (tb[i] == 0) { printf("RIEGEL-FAIL: Typ #%d ist NIE getroffen worden\n", i); fail = 1; }
    /* Das Tor darf den STEHENDEN Hund nicht aussperren. */
    if (m[1].ok && m[1].durch_s != m[1].gueltig) {
        printf("RIEGEL-FAIL: stehender Hund faellt aus dem Fenster (%d/%d)\n",
               m[1].durch_s, m[1].gueltig); fail = 1; }
    printf(fail ? "PROBE-FAIL\n" : "PROBE-OK\n");
    return fail;
}
