/* test_r38_spinnen.c — Runde 38 Spur R "spinnen" (Stage-2-Spinnen). Belege: include/re15_spinnen_r38.h,
 * engine/src/spinnen_r38.c, Dossier analysis/befunde_runde38/R_spinnen.md (Messungen 1.x, Belege 2.x).
 *
 * Misst die MECHANIK im echten Spielschritt (scd_vm_tick + re15_game_step + ESP-Tick wie
 * platform/pc fx_plattform_pc.c) mit den echten STAGE2-RDTs und Bänken:
 *
 *  A  SCHACHT (Nutzerpunkt 1, ROOM2060, RE1.5- UND RE2-KI): die Schachtspinne (Sce_em_set grid 0x41
 *     floor 2, (5800,-3600,6400)) wartet im Schacht (Zustand 4, Clip 0x10 bzw. RE2 15, Bild 1,
 *     y -3600) bis Cut 1 (work[0x0A] = 1 -> sub01 -> sub02 `Member_set 0x0C 0x42`). Danach die
 *     neun Phasen @0x80114ec8 mit den Original-Abstaenden aus der GDB-Messung
 *     (R_belege/orig_2060_schacht_gift.txt Bild 63..135): Phase 2 nach +30, Phase 3/4 +31/+32,
 *     Phase 5 +38..+40 (Zufall), Phase 7 +70, Phase 8 +71, ACTIVE +72; Phase 1 Bild fuer Bild die
 *     gemessene Original-Bahn (x,z) Bild 63..73 — 11 x 200 auf -z, am Schachtgitter obj 0 (5775,-4860,
 *     5325) von FUN_8002cabc @0x8002be40 ausgeschoben (z 5525, dann x 5975, Teil 4 spinnen_r38.c);
 *     y -2600 + Boden 0 + Neigung 1024 + z 4625 in Phase 2 (Bild 94), Landung y 0 floor 0 auf
 *     (5975, 4625 - 1488 = 3137). RE2: danach RE2-INIT auf dem Boden (+0x222 = 0, y 0).
 *  B  GIFT (Nutzerpunkt 2): die gelandete Spinne spuckt (RE1.5 Unterzustand 7 / RE2 Unterzustand 8):
 *     drei Effektplaetze 0x0d (Raum-ESP), Treffer -> Spieler HP -10 (Resolver Art 0), Zustand 2,
 *     Gift-Bit 0x2; Wirkung alle 121 Bilder HP -2 (@0x80031e44-ecc), nie toedlich (HP 1);
 *     Gegengift 0x23 loescht das Bit (@0x8004af8c).
 *  C  MINISPINNEN (Nutzerpunkt 3, RE2-KI, ROOM2030, Boden -5400): Spinne mit der Schrotflinte
 *     getoetet -> Babys (Typ 0x26) stehen auf dem Boden des Elternteils (y -5400, +0x1C2 -5400,
 *     vorher fielen sie bis y 0) und beissen den Spieler (HP -1 je Biss, Reichweite 450 + 10).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_item_use.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_esp.h"
#include "re2_ems.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_md1.h"
#include "re15_spinnen_r38.h"
#include "re15_math.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static int s_fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); s_fails++; } \
                           else { printf("ok    "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static re15_esp_t         s_esp;
static uint8_t           *s_rdt_buf = NULL;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint32_t u32(const uint8_t *b, size_t o) { return (uint32_t)b[o] | ((uint32_t)b[o+1] << 8) | ((uint32_t)b[o+2] << 16) | ((uint32_t)b[o+3] << 24); }

/* Baenke wie probe_1010_kriecher.c (RE1.5: CDEMD0.EMS + DEFAULT_EMS_ORDER; RE2: re2_ems_load_bank). */
static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}
static uint8_t *s_re15_ems = NULL; static size_t s_re15_n = 0;
static uint8_t  s_blob[0x80000];
static int bank_re15(uint8_t type)
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
    if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return 0; }
    eb->ok = 1; eb->buf = NULL;
    return 1;
}

static int raum(const char *datei, unsigned id, re15_ai_flavor_t fl, int32_t px, int32_t py, int32_t pz,
                int16_t prot, uint8_t cut)
{
    size_t n = 0;
    char p[512];
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, datei);
    free(s_rdt_buf);
    s_rdt_buf = slurp(p, &n);
    if (!s_rdt_buf) { printf("RDT fehlt: %s\n", p); return 0; }
    memset(&g_room_rdt, 0, sizeof g_room_rdt);
    if (re15_rdt_parse(s_rdt_buf, n, &g_room_rdt) != 0) return 0;
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    memset(&s_esp, 0, sizeof s_esp);
    if (re15_esp_parse(s_rdt_buf, n, u32(s_rdt_buf, 0x4C), u32(s_rdt_buf, 0x50), u32(s_rdt_buf, 0x54),
                       u32(s_rdt_buf, 0x58), &s_esp) == 0)
        re15_esp_set_room_bank(&s_esp);
    else
        re15_esp_set_room_bank(NULL);

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    re15_esp_fx_reset();
    re15_spinnen_r38_reset();
    g_current_room_id = id;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->status_flags = 0;
    pl->state = 0; pl->motion = 0; pl->floor = (uint8_t)(-py / 1800); pl->y = py;
    pl->x = px; pl->z = pz; pl->rot_y = prot;
    re15_player_apply_hitbox(pl);
    re15_collision_set_band(pl->floor);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, px, pz, cut);
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = prot;
    if (fl == RE15_AI_FLAVOR_RE2) { (void)bank_re2(0x25); (void)bank_re2(0x26); }
    else                          { (void)bank_re15(0x25); }
    return 1;
}

static void bild(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
    re15_esp_fx_tick(re15_esp_room_bank());      /* fx_plattform_pc.c: hinter Gegnern und Spieler */
}

static re15_actor_t *spinne_bei(int32_t x, int32_t z)
{
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (e->active && e->type == 0x25 && e->x == x && e->z == z) return e;
    }
    return NULL;
}
static int gift_plaetze(void)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->rows_base && f->effect_id == 0x0d) n++;
    }
    return n;
}

/* ============================== A: SCHACHT ============================== */
static re15_actor_t *teil_a(re15_ai_flavor_t fl, const char *name)
{
    const int re2 = (fl == RE15_AI_FLAVOR_RE2);
    printf("\n== A Schacht %s (ROOM2060, Spieler (7600,0,3950) wie Tuer 2030->2060, Cut 0)\n", name);
    if (!raum("STAGE2/ROOM2060.RDT", 0x2060, fl, 7600, 0, 3950, 2048, 0)) { s_fails++; return NULL; }
    re15_actor_t *e = spinne_bei(5800, 6400);
    CHECK(e != NULL, "%s: Schachtspinne (5800,-3600,6400) gespawnt (Sce_em_set @0x0E26 grid 0x41 floor 2)", name);
    if (!e) return NULL;
    CHECK(e->grid_id == 0x41 && e->y == -3600, "%s: grid 0x%02X y %d", name, e->grid_id, e->y);
    for (int f = 0; f < 40; f++) bild();
    const int16_t clip_halt = re2 ? 15 : 0x10;
    CHECK(e->state == 4 && e->sub_state_1 == 1 && e->sub_state_3 == 0 && e->y == -3600 && e->motion == clip_halt
          && e->anim_frame == 1 && e->z == 6400,
          "%s: Halten im Schacht nach 40 Bildern: Zustand %d/%d Phase %d y %d Clip %d Bild %d z %d (erwartet 4/1/0, "
          "-3600, %d, Bild 1 = anim_set @0x80110d60, keine Bewegung @0x80114f14-1c)", name, e->state, e->sub_state_1,
          e->sub_state_3, e->y, e->motion, e->anim_frame, e->z, clip_halt);

    g_scd.work_vars[0x0A] = 1;                      /* angezeigter Cut 1 (DAT_800b0fe4) -> sub01 -> sub02 */
    int T = -1, t2 = -1, t3 = -1, t4 = -1, t5 = -1, t7 = -1, t8 = -1, t_act = -1;
    int32_t z_nach_p1 = 0, x_nach_p1 = 0, y_p2 = 0, gy_p2 = 1, rz_p2 = 0, z_p2 = 0, x_p2 = 0;
    int32_t bahn_x[11], bahn_z[11], land_x = 0, land_y = 1, land_z = 0;
    int clip_p4 = -1, clip_p6 = -1, land_floor = -1;
    for (int f = 0; f < 200; f++) {
        uint8_t ph_alt = e->sub_state_3, st_alt = e->state;
        bild();
        if (T < 0 && e->sub_state_3 == 1) T = f;
        if (T >= 0 && f - T < 11) { bahn_x[f - T] = e->x; bahn_z[f - T] = e->z; }
        if (t2 < 0 && e->sub_state_3 == 2) { t2 = f; z_nach_p1 = e->z; x_nach_p1 = e->x; }
        if (t3 < 0 && e->sub_state_3 == 3) { t3 = f; y_p2 = e->y; gy_p2 = e->dog_floor_y; rz_p2 = e->rot_z; z_p2 = e->z;
                                             x_p2 = e->x; }
        if (t4 < 0 && e->sub_state_3 == 4) { t4 = f; clip_p4 = e->motion; }
        if (t5 < 0 && e->sub_state_3 == 6 && ph_alt != 6) { t5 = f; clip_p6 = e->motion; }
        if (t7 < 0 && e->sub_state_3 == 7) t7 = f;
        if (t8 < 0 && e->sub_state_3 == 8) { t8 = f; land_x = e->x; land_y = e->y; land_z = e->z; land_floor = e->floor; }
        if (t_act < 0 && st_alt == 4 && e->state != 4) t_act = f;
        if (t_act >= 0 && f > t_act + 2) break;
    }
    printf("   Phasen: T=%d p2=%d p3=%d p4=%d p6=%d p7=%d p8=%d aktiv=%d\n", T, t2, t3, t4, t5, t7, t8, t_act);
    CHECK(T >= 0 && e->grid_id != 0x41, "%s: Ausloeser sub02 (grid 0x41 -> 0x42 @RDT 0x1342) startet Phase 1", name);
    CHECK(t2 - T == 30, "%s: Phase 2 nach +%d Bildern (Original Bild 63 -> 93 = +30; Clip 0x10 Bild 31 @0x80114fe4)", name, t2 - T);
    {   /* Original-Bahn Bild 63..73 (R_belege/orig_2060_schacht_gift.txt): 11 x 200 auf -z (`sltiu 0xb` @0x80114fa0,
         * `ori 0xc8` @0x80114fa8), am Schachtgitter obj 0 (5775,-4860,5325, Null-Kasten) von FUN_8002cabc
         * (@0x8002be40, Objekt-Tick nach der Wurzel) ausgeschoben: Bild 67 z 5525 = 5325 + 200, Bild 68 x 5975 = 5775 + 200. */
        static const int32_t orig_x[11] = { 5800, 5800, 5800, 5800, 5800, 5975, 5975, 5975, 5975, 5975, 5975 };
        static const int32_t orig_z[11] = { 6200, 6000, 5800, 5600, 5525, 5325, 5125, 4925, 4725, 4525, 4325 };
        int gleich = 0;
        for (int k = 0; k < 11; k++) {
            if (T >= 0 && bahn_x[k] == orig_x[k] && bahn_z[k] == orig_z[k]) gleich++;
            else if (T >= 0) printf("   Bahn Bild +%d: Port (%d,%d) Original (%d,%d)\n", k, bahn_x[k], bahn_z[k],
                                    orig_x[k], orig_z[k]);
        }
        CHECK(gleich == 11, "%s: Phase-1-Bahn %d/11 Bilder gleich der Original-Messung (Bild 63..73)", name, gleich);
    }
    CHECK(x_nach_p1 == 5975 && z_nach_p1 == 4325, "%s: Phase 1 Ende (%d,%d) = Original Bild 73-92 (5975,4325)", name,
          x_nach_p1, z_nach_p1);
    CHECK(t3 - T == 31 && y_p2 == -2600 && gy_p2 == 0 && rz_p2 == 1024 && z_p2 == 4625 && x_p2 == 5975,
          "%s: Phase 2 (+%d): y %d (+1000 @0x8011501c), Boden %d (`sh zero,442` @0x80115044), Neigung %d (@0x8011505c), "
          "Lage (%d,%d) (300 rueckwaerts @0x80115030; Original Bild 94 (5975,-2600,4625))", name, t3 - T, y_p2, gy_p2, rz_p2,
          x_p2, z_p2);
    CHECK(t4 - T == 32 && clip_p4 == 2, "%s: Phase 3/4 (+%d) Clip %d (= 2, RE2 keyframe-gleich 2)", name, t4 - T, clip_p4);
    CHECK(t5 - T >= 38 && t5 - T <= 41 && clip_p6 == (re2 ? 14 : 0x0d),
          "%s: Phase 6 (+%d, Original +40: y > Boden-1800 @0x80115104) Clip %d", name, t5 - T, clip_p6);
    CHECK(t7 - t5 == 30, "%s: Phase 7 nach Clip 0xd Bild 31 (+%d nach Phase 6, Original 133-103 = 30)", name, t7 - t5);
    CHECK(t_act - T == 72 || t_act - T == 73, "%s: Landung + ACTIVE nach +%d Bildern (Original 63 -> 135 = +72)", name, t_act - T);
    CHECK(land_y == 0 && land_floor == 0 && e->rot_z == 0, "%s: gelandet y %d floor %d Neigung %d (@0x80115264-6c / @0x80115244)",
          name, land_y, land_floor, e->rot_z);
    CHECK(land_x == 5975 && land_z == 3137, "%s: Landestelle (%d,%d) = (5975, 4625 - 1488) (Quadrant 1 @0x801152e4)", name,
          land_x, land_z);
    if (re2) {
        for (int f = 0; f < 3; f++) bild();
        CHECK(e->state == 1 && e->re2s_mode222 == 0 && e->y == 0 && e->re2z_f10e == 0,
              "RE2: Uebergabe an den RE2-INIT auf dem Boden: Zustand %d Modus %d y %d +0x10E %d", e->state,
              e->re2s_mode222, e->y, e->re2z_f10e);
    } else {
        CHECK(e->state == 1 && e->grid_id == 5 && e->hit_react == 0, "RE1.5: ACTIVE mit grid %d (`sb 5` @0x80115254), +0x93 %d",
              e->grid_id, e->hit_react);
    }
    return e;
}

/* A2: die Original-Messung 1:1 nachgestellt (R_belege/orig_2060_schacht_gift.txt): Spieler bei Bild 60 nach
 * (6000,0,3000) Blick 1024, Cut 1 bei Bild 62. Die Spinne landet in Bild 134 auf (5975,0,3137) = IM Spieler —
 * Original danach: Spinne (5740,0,4332), Spieler (6018,0,2906), Bild 135 Spinne (5740,0,4333). */
static void teil_a2(re15_ai_flavor_t fl, const char *name)
{
    printf("\n== A2 Landung neben dem Spieler %s (Original-Messung Bild 60..135 nachgestellt)\n", name);
    if (!raum("STAGE2/ROOM2060.RDT", 0x2060, fl, 7600, 0, 3950, 2048, 0)) { s_fails++; return; }
    re15_actor_t *e = spinne_bei(5800, 6400);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!e) { CHECK(0, "%s: Schachtspinne fehlt", name); return; }
    for (int f = 0; f < 60; f++) bild();
    pl->x = 6000; pl->z = 3000; pl->rot_y = 1024;              /* Teleport wie gdb_spinne.py --pl 6000,3000,1024 */
    bild(); bild();
    g_scd.work_vars[0x0A] = 1;
    int32_t sx = 0, sz = 0, px = 0, pz = 0, sx2 = 0, sz2 = 0, px2 = 0, pz2 = 0;
    int land = -1;
    for (int f = 0; f < 120; f++) {
        bild();
        if (land < 0 && e->sub_state_3 == 8) { land = f; sx = e->x; sz = e->z; px = pl->x; pz = pl->z; continue; }
        if (land >= 0) { sx2 = e->x; sz2 = e->z; px2 = pl->x; pz2 = pl->z; break; }
    }
    printf("   Landebild: Spinne (%d,%d) Spieler (%d,%d); naechstes Bild Spinne (%d,%d) Spieler (%d,%d)\n", sx, sz, px, pz,
           sx2, sz2, px2, pz2);
    /* Spinne: aec4(Spieler, Spinne) in der Wurzel (`jal 0x8002aec4` @0x80110a6c) schiebt sie im Landebild aus dem
     * Spieler — Bild 134 (5740,4332), Bild 135 (5740,4333), beides gemessen. */
    CHECK(sx == 5740 && sz == 4332 && sx2 == 5740 && sz2 == 4333,
          "%s: Spinne Landebild (%d,%d) / Folgebild (%d,%d) = Original Bild 134/135 (5740,4332)/(5740,4333)", name, sx, sz,
          sx2, sz2);
    /* Spieler: der Rest-Schub FUN_8002b544 @0x80031cbc (Spieler-FSM @0x8001ce0c NACH der Gegner-Schleife @0x8001ce04)
     * -> Original Bild 134 (6018,2906). Der Port ruft re15_body_push_player im Spieler-Block VOR re15_enemy_ai_run_all
     * (game_step_common.c) -> derselbe Wert ein Bild spaeter (allgemeine Port-Reihenfolge, Dossier 6). */
    CHECK(px == 6000 && pz == 3000 && px2 == 6018 && pz2 == 2906,
          "%s: Spieler (%d,%d) -> (%d,%d) = Original-Endlage (6018,2906), im Port ein Bild spaeter", name, px, pz, px2, pz2);
}

/* ============================== B: GIFT ============================== */
static void teil_b(re15_actor_t *e, int re2, const char *name)
{
    printf("\n== B Gift %s\n", name);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
        if (&g_actors[s] != e) g_actors[s].active = 0;
    /* Spieler 2000 vor die Spinne (Blick 1024 = -z), Blick zur Spinne */
    pl->x = e->x; pl->z = e->z - 2000; pl->y = 0; pl->rot_y = 3072; pl->hp = 100; pl->state = 1;
    pl->status_flags = 0; pl->hit_react = 0;
    re15_spinnen_r38_reset();
    e->rot_y = 1024;
    if (re2) { e->state = 1; e->sub_state_1 = 8; e->sub_state_2 = 0; e->sub_state_3 = 0; }   /* RE2 Zustandswort 0x801 */
    else     { e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 7; e->sub_state_3 = 0; }  /* RE1.5 Unterzustand 7 */
    int max_plaetze = 0, f_treffer = -1;
    for (int f = 0; f < 60; f++) {
        bild();
        int n = gift_plaetze();
        if (n > max_plaetze) max_plaetze = n;
        if (f_treffer < 0 && pl->hp < 100) { f_treffer = f; break; }
    }
    CHECK(max_plaetze == 3, "%s: Spucken spawnt %d Gift-Plaetze (Effekt 0x0d sub 0 = 3 Stroeme)", name, max_plaetze);
    CHECK(f_treffer >= 0 && pl->hp == 90, "%s: Treffer nach %d Bildern, HP %d (= 100 - 10, Resolver Art 0 @0x80018004, "
          "DAT_8006f418[0] = 10)", name, f_treffer, pl->hp);
    CHECK(pl->state == 2, "%s: Spieler Zustand %d (= 2 getroffen @0x80012ebc)", name, pl->state);
    CHECK((pl->status_flags & 2u) && re15_spinnen_r38_gift_treffer() >= 1,
          "%s: GIFT gesetzt (status 0x%04X, Treffer %u) — RE2 Op 32 `sh 5400,0x800cfe10` @0x80020130", name,
          pl->status_flags, re15_spinnen_r38_gift_treffer());
}

/* B2: derselbe Effekt 0x0d OHNE Spinne als Absender (Effekt steht in 41 Raum-ESPs, u.a. Birkins Hieb 0x0d002000
 * @0x801146d4-ec): Routine 25 trifft wie im Original (10 HP, Resolver Art 0), vergiftet aber NICHT — RE2 vergiftet nur
 * fuer Spinnen-sub 8..11 (`andi v0,v0,0x8` @0x80020118). */
static void teil_b2(void)
{
    printf("\n== B2 Effekt 0x0d ohne Spinne (kein Gift)\n");
    /* ROOM2060 an der Landestelle der Schachtspinne (Teil A/B: Spinne (5975,0,3137), Spieler 2000 davor), aber ohne
     * Spinne: Klumpen an der Spuck-Hoehe, Blick 1024 (-z) auf Leon. */
    if (!raum("STAGE2/ROOM2060.RDT", 0x2060, RE15_AI_FLAVOR_RE15, 5975, 0, 1137, 3072, 1)) { s_fails++; return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
    pl->hp = 100; pl->status_flags = 0; pl->state = 1;
    int n = re15_esp_fx_spawn_rows(re15_esp_room_bank(), 0x0d, 0x00, 0x2000, 5975, -700, 2637, INT32_MAX, 1024);
    int getroffen = -1;
    for (int f = 0; f < 60 && getroffen < 0; f++) { bild(); if (pl->hp < 100) getroffen = f; }
    CHECK(n > 0 && getroffen >= 0 && pl->hp == 90, "Klumpen ohne Absender trifft: %d Plaetze, Bild %d, HP %d (Routine 25)",
          n, getroffen, pl->hp);
    CHECK(!(pl->status_flags & 2u) && re15_spinnen_r38_gift_treffer() == 0,
          "kein Gift ohne Spinne als Absender (status 0x%04X, Gift-Treffer %u)", pl->status_flags,
          re15_spinnen_r38_gift_treffer());
}

/* B3 (Fortsetzung 2): Gift nur am ERREICHBAREN Spieler. RE2 Op 32 vergiftet nur, wenn der Applier FUN_80047664 den
 * Spieler zurueckgibt (`beq v0,zero` @0x800200e8); der ueberspringt ihn bei +0x1D3 != 0 (@0x800476ec-f4) und HP < 0
 * (@0x800476fc-704). Port-Tor = der Schutz des Spielerzweigs von FUN_80012d60 (+0x93 & 1 @0x80012e24-30). Der Klumpen
 * einer echten Spinne (re15_spinnen_r38_spucken, KI angehalten) trifft einmal den geschuetzten und einmal den freien
 * Spieler. Bild-Schritt wie bild(), der Schutz wird direkt VOR dem ESP-Takt gesetzt (der Spieler-Takt loescht ihn). */
static int b3_lauf(int schutz)
{
    if (!raum("STAGE2/ROOM2060.RDT", 0x2060, RE15_AI_FLAVOR_RE15, 5975, 0, 1137, 3072, 1)) { s_fails++; return -1; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *sp = NULL;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        if (g_actors[s].active && g_actors[s].type == 0x25 && !sp) sp = &g_actors[s];
        else g_actors[s].active = 0;
    }
    if (!sp) { printf("FAIL: B3 keine Spinne\n"); s_fails++; return -1; }
    re15_enemy_ai_set_paused(1);
    sp->x = 5975; sp->y = 0; sp->z = 3137; sp->rot_y = 1024; sp->state = 1; sp->grid_id = 5;
    pl->hp = 100; pl->status_flags = 0; pl->state = 1; pl->hit_react = 0;
    re15_spinnen_r38_spucken(sp, 0);
    for (int f = 0; f < 60; f++) {
        const unsigned char *raw; int len, id;
        re15_msg_tick(&raw, &len, &id);
        s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
        scd_vm_tick();
        re15_game_step(&s_ctx);
        if (schutz) pl->hit_react |= 0x1u;
        re15_esp_fx_tick(re15_esp_room_bank());
    }
    re15_enemy_ai_set_paused(0);
    return pl->hp;
}
static void teil_b3(void)
{
    printf("\n== B3 Gift nur am erreichbaren Spieler (Schutz +0x93 & 1 @0x80012e24-30 / RE2 +0x1D3 @0x800476ec-f4)\n");
    int hp = b3_lauf(1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    CHECK(hp == 100 && !(pl->status_flags & 2u) && re15_spinnen_r38_gift_treffer() == 0,
          "geschuetzter Spieler: HP %d, status 0x%04X, Gift-Treffer %u (kein Schaden, kein Gift)", hp, pl->status_flags,
          re15_spinnen_r38_gift_treffer());
    hp = b3_lauf(0);
    CHECK(hp == 88 && (pl->status_flags & 2u) && re15_spinnen_r38_gift_treffer() >= 1,
          "Gegenprobe freier Spieler: HP %d = 100 - 10 (Treffer) - 2 (Gift sofort, Zaehler 0 @0x80031e74), status 0x%04X, "
          "Gift-Treffer %u", hp, pl->status_flags, re15_spinnen_r38_gift_treffer());
}

/* B5 (Nachbesserung 1, Mangel 3 — Nachtrag Integration 2026-10-17 07:36, Spur B): der Giftklumpen laeuft im ESP-Takt
 * AUSSERHALB von re15_enemy_ai_run_all, Spur Bs Sicht (je geschuetztem Gegner-Tick) erfasst ihn nicht. Haelt eine Szene
 * den Spieler (g_scd.player_mode == 2 = re15_szenenschutz_haelt()), sieht Routine 25 den Spieler durch das Original-Tor
 * +0x93 Bit 0 als nicht angreifbar: der Resolver FUN_80012d60 ueberspringt ihn (`lbu v0,player.hit_react` @0x80012e24 /
 * `andi v0,v0,0x1` / `bne` @0x80012e30), das Gift-Tor `frei` (RE2 FUN_80047664 +0x1D3 @0x800476ec-f4) ebenso.
 * Erwartet: Szene haelt -> keine HP, kein Gift; Gegenprobe frei -> 10 HP (Treffer) + 2 HP (Takt sofort) und Gift. */
static int s_b5_hr_falsch = 0;
static int b5_lauf(int szene)
{
    if (!raum("STAGE2/ROOM2060.RDT", 0x2060, RE15_AI_FLAVOR_RE15, 5975, 0, 1137, 3072, 1)) { s_fails++; return -1; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *sp = NULL;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        if (g_actors[s].active && g_actors[s].type == 0x25 && !sp) sp = &g_actors[s];
        else g_actors[s].active = 0;
    }
    if (!sp) { printf("FAIL: B5 keine Spinne\n"); s_fails++; return -1; }
    re15_enemy_ai_set_paused(1);
    sp->x = 5975; sp->y = 0; sp->z = 3137; sp->rot_y = 1024; sp->state = 1; sp->grid_id = 5;
    pl->hp = 100; pl->status_flags = 0; pl->state = 1; pl->hit_react = 0;
    re15_spinnen_r38_spucken(sp, 0);
    int hr_falsch = 0;
    for (int f = 0; f < 60; f++) {
        const unsigned char *raw; int len, id;
        re15_msg_tick(&raw, &len, &id);
        s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
        g_scd.player_mode = szene ? 2 : 0;
        scd_vm_tick();
        re15_game_step(&s_ctx);
        g_scd.player_mode = szene ? 2 : 0;                 /* die Szene haelt den Spieler auch im ESP-Takt */
        uint8_t hr = pl->hit_react;
        re15_esp_fx_tick(re15_esp_room_bank());
        if (szene && pl->hit_react != hr) hr_falsch++;     /* die Sicht darf den echten +0x93 nicht verstellen */
    }
    g_scd.player_mode = 0;
    re15_enemy_ai_set_paused(0);
    s_b5_hr_falsch = hr_falsch;
    return pl->hp;
}
static void teil_b5(void)
{
    printf("\n== B5 Giftklumpen, waehrend eine Szene den Spieler haelt (player_mode 2, Spur B re15_szenenschutz_haelt)\n");
    int hp = b5_lauf(1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    CHECK(hp == 100 && !(pl->status_flags & 2u) && re15_spinnen_r38_gift_treffer() == 0 && s_b5_hr_falsch == 0,
          "Szene haelt: HP %d, status 0x%04X, Gift-Treffer %u, +0x93 vom ESP-Takt veraendert %d mal (kein Schaden, "
          "kein Gift; vorher 88 + Gift)", hp, pl->status_flags, re15_spinnen_r38_gift_treffer(), s_b5_hr_falsch);
    hp = b5_lauf(0);
    CHECK(hp == 88 && (pl->status_flags & 2u) && re15_spinnen_r38_gift_treffer() >= 1,
          "Gegenprobe frei (player_mode 0): HP %d = 100 - 10 - 2, status 0x%04X, Gift-Treffer %u", hp, pl->status_flags,
          re15_spinnen_r38_gift_treffer());
}

/* B4 (Nachbesserung 1, Mangel 1): der Gift-Takt im ECHTEN Spielschritt (re15_game_step mit rdt_ok, ESP-Takt wie bild())
 * ist KEIN Treffer. Original RE1.5 @0x80031e44-ec8: Stores nur auf den Zaehler 0x800acc27 (@0x80031e70/80/94) und die HP
 * (@0x80031eac/ec8), kein `jal`, kein Kommandowort 0x800aca58, kein +0x93; RE2 @0x8003c12c-180 ebenso. Die Treffer-Reaktion
 * (Clip 8/9/0xa, Rueckstoss 0xc8, SE CORE 1/2, Zielabbruch) schreibt im Original nur ein Kommando-2-Schreiber (z.B.
 * `sb v0(=2),4(s1)` @0x80012ebc). Vorher (Stand e7131aca) loeste jeder Takt ueber den Port-Detektor `hp < s_prev_hp`
 * (game_step_common.c) re15_player_stagger_cmd2 aus: Periode 143 statt 121, Zielabbruch (R0/p2b, unit_r34_wurf 119-121). */
extern int g_test_core_se_last, g_test_core_se_count;   /* tests/test_support.c (re15_audio_core_se) */
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
static void teil_b4(void)
{
    printf("\n== B4 Gift-Takt im Spielschritt: nur HP und Zaehler (@0x80031e44-ec8), keine Treffer-Reaktion\n");
    /* (a) Spieler steht (Kommando 1, kein Pad), vergiftet, keine Gegner: 4 Takte */
    if (!raum("STAGE2/ROOM2060.RDT", 0x2060, RE15_AI_FLAVOR_RE15, 5975, 0, 1137, 3072, 1)) { s_fails++; return; }
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 5; f++) bild();                       /* Spieler im Kommando 1 einschwingen */
    pl->hp = 100; pl->status_flags = 2; pl->hit_react = 0;
    re15_spinnen_r38_reset();
    const int32_t x0 = pl->x, z0 = pl->z;
    const int se0 = g_test_core_se_count;
    int takt[4] = { -1, -1, -1, -1 }, nt = 0, abzug_falsch = 0, flinch = 0, clip = 0, weg = 0;
    for (int f = 0; f < 400; f++) {
        int16_t vor = pl->hp;
        bild();
        if (pl->hp != vor) { if (nt < 4) takt[nt] = f; nt++; if (vor - pl->hp != 2) abzug_falsch++; }
        if (re15_player_hit_flinch_active()) flinch++;
        if (pl->motion == 8 || pl->motion == 9 || pl->motion == 0x0a) clip++;
        int w = abs(pl->x - x0) + abs(pl->z - z0);
        if (w > weg) weg = w;
    }
    CHECK(nt == 4 && abzug_falsch == 0 && takt[1] - takt[0] == 121 && takt[2] - takt[1] == 121 && takt[3] - takt[2] == 121,
          "stehend: %d Takte zu -2 HP in Bild %d/%d/%d/%d = Periode 121 (Zaehler 0x78 + 1 @0x80031e78; vorher 143)",
          nt, takt[0], takt[1], takt[2], takt[3]);
    CHECK(flinch == 0 && clip == 0 && weg == 0 && g_test_core_se_count == se0,
          "stehend: keine Treffer-Reaktion (Flinch-Bilder %d, Clip 8/9/0xa %d, Rueckstoss %d, SE CORE %d; vorher je Takt "
          "22 Bilder Clip 8/9 + 500 Rueckstoss)", flinch, clip, weg, g_test_core_se_count - se0);

    /* (c) Kommando 0 (Raumwechsel / Spieler-Laden / Inventar-Ende -> re15_player_cmd_zero) nullt den Zaehler:
     * `sb zero,-13273(at)` @0x8003196c im cmd-0-Handler 0x800318f8 -> der naechste Takt kommt SOFORT (@0x80031e74).
     * Vorher (Stand e7131aca) ueberlebte der Zaehler jeden Raumwechsel/jedes Laden (Abnahme 0, Hinweis 1). */
    {
        extern void re15_player_cmd_zero(void);              /* game_step_common.c */
        unsigned z_vor = re15_spinnen_r38_gift_zaehler();
        re15_player_cmd_zero();
        unsigned z_nach = re15_spinnen_r38_gift_zaehler();
        int16_t vor = pl->hp;
        bild();
        CHECK(z_vor > 0 && z_nach == 0 && pl->hp == vor - 2 && re15_spinnen_r38_gift_zaehler() == 0x78,
              "Kommando 0: Zaehler %u -> %u (@0x8003196c), naechstes Bild Takt HP %d -> %d, Zaehler %u (0x78)",
              z_vor, z_nach, vor, pl->hp, re15_spinnen_r38_gift_zaehler());
    }

    /* (b) Spieler ZIELT (R1 gehalten, Zielbereit), dann vergiftet: der erste Takt kommt sofort (Zaehler 0 @0x80031e74)
     * und darf das Zielen nicht abbrechen (re15_player_aim_interrupt sitzt nur im Kommando-2-Arm). */
    if (!raum("STAGE2/ROOM2060.RDT", 0x2060, RE15_AI_FLAVOR_RE15, 5975, 0, 1137, 3072, 1)) { s_fails++; return; }
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
    re15_player_aim_reset();
    {   uint16_t fcs[16];                                     /* Cliplaengen-Mock wie probe_r34_wurf (PLW nicht geladen) */
        for (int i = 0; i < 16; i++) fcs[i] = 12;
        re15_player_set_aim_clip_lens(fcs, 16); }
    g_inv.slots[3].id = 9; g_inv.slots[3].qty = 5; g_inv.slots[3].flags = 0;
    re15_player_set_equipped_weapon(9);
    int bereit = -1;
    for (int f = 0; f < 80 && bereit < 0; f++) {
        const unsigned char *raw; int len, id;
        re15_msg_tick(&raw, &len, &id);
        s_ctx.pad_current = RE15_PAD_BIT_R1; s_ctx.pad_pressed = (f == 0) ? RE15_PAD_BIT_R1 : 0;
        scd_vm_tick(); re15_game_step(&s_ctx); re15_esp_fx_tick(re15_esp_room_bank());
        if (re15_player_aim_ready()) bereit = f;
    }
    pl->hp = 100; pl->status_flags = 2; pl->hit_react = 0;
    re15_spinnen_r38_reset();
    int takt_b = -1, nicht_bereit = 0, flinch_b = 0;
    for (int f = 0; f < 60; f++) {
        const unsigned char *raw; int len, id;
        int16_t vor = pl->hp;
        re15_msg_tick(&raw, &len, &id);
        s_ctx.pad_current = RE15_PAD_BIT_R1; s_ctx.pad_pressed = 0;
        scd_vm_tick(); re15_game_step(&s_ctx); re15_esp_fx_tick(re15_esp_room_bank());
        if (takt_b < 0 && pl->hp == vor - 2) takt_b = f;
        if (!re15_player_aim_ready()) nicht_bereit++;
        if (re15_player_hit_flinch_active()) flinch_b++;
    }
    CHECK(bereit >= 0 && takt_b >= 0 && pl->hp == 98 && nicht_bereit == 0 && flinch_b == 0,
          "zielend: Zielbereit nach %d Bildern, Gift-Takt in Bild %d (HP %d), danach %d Bilder NICHT zielbereit, %d Flinch-"
          "Bilder (vorher: Zielabbruch im ersten Takt, unit_r34_wurf 119-121)", bereit, takt_b, pl->hp, nicht_bereit, flinch_b);
    re15_player_aim_reset();
}

static void teil_b_wirkung(void)
{
    printf("\n== B Gift-Wirkung (@0x80031e44-ecc) und Gegengift (@0x8004af8c)\n");
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_spinnen_r38_reset();
    pl->hp = 100; pl->status_flags = 2; pl->state = 1;
    int drains[4] = { -1, -1, -1, -1 }, nd = 0;
    for (int f = 0; f < 400 && nd < 4; f++) {
        int16_t vor = pl->hp;
        re15_spinnen_r38_gift_tick(pl);
        if (pl->hp != vor) { if (nd < 4) drains[nd] = f; nd++; CHECK(vor - pl->hp == 2, "Abzug %d HP", vor - pl->hp); }
    }
    CHECK(drains[0] == 0 && drains[1] == 121 && drains[2] == 242,
          "Abzug in Bild %d/%d/%d (Zaehler 0 -> sofort, dann 0x78 + 1 = 121 Bilder Periode)", drains[0], drains[1], drains[2]);
    pl->hp = 1;
    re15_spinnen_r38_reset();
    re15_spinnen_r38_gift_tick(pl);
    CHECK(pl->hp == 1, "nie toedlich: HP 1 - 2 -> %d (`ori v0,zero,0x1` @0x80031ec0)", pl->hp);
    pl->hp = 50; pl->status_flags = 2;
    re15_item_use_apply(0x23);
    CHECK(!(pl->status_flags & 2u) && pl->hp == 75, "Gegengift 0x23: Bit geloescht (0x%04X), HP +25 = %d", pl->status_flags, pl->hp);
    re15_spinnen_r38_reset();
    int16_t h0 = pl->hp;
    for (int f = 0; f < 300; f++) re15_spinnen_r38_gift_tick(pl);
    CHECK(pl->hp == h0, "ohne Gift kein Abzug (HP %d)", pl->hp);
}

/* ============================== C: MINISPINNEN ============================== */
/* Tiefster GEZEICHNETER Punkt eines Aktors relativ zu seiner Lage-Y (PSX-Y nach unten: groesser = tiefer), exakt
 * die Zeichenkette des NPC-Renderers (platform/pc/main.c: re15_compute_actor_kf -> re15_skel_compute_pose ->
 * Mesh m an Pose m (m >= Bones -> Pose 0) -> rot*v>>12 + trans). 0 = steht auf seiner Lage; negativ = schwebt. */
static int32_t gezeichnet_tiefster(const re15_actor_t *e, int *ok)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    *ok = 0;
    if (!b || !b->ok || b->skel.bone_count <= 0) return 0;
    int kf = re15_compute_actor_kf(&b->anim, &b->skel, e, -1, e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(&b->skel, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    int32_t tief = -0x7fffffff;
    for (int m = 0; m < b->md1.mesh_count; m++) {
        const re15_skel_pose_t *p = &poses[m < b->skel.bone_count ? m : 0];
        const re15_md1_mesh_t *me = &b->md1.meshes[m];
        for (int pass = 0; pass < 2; pass++) {
            const re15_md1_vertex_t *vs = pass ? me->quad_vertices : me->tri_vertices;
            int nv = pass ? me->quad_vertex_count : me->tri_vertex_count;
            for (int i = 0; i < nv; i++) {
                int32_t y = (int32_t)(((int64_t)p->rot[3] * vs[i].x + (int64_t)p->rot[4] * vs[i].y
                                      + (int64_t)p->rot[5] * vs[i].z) >> 12) + p->trans[1];
                if (y > tief) tief = y;
            }
        }
    }
    *ok = (tief != -0x7fffffff);
    return tief;
}

static void teil_c(void)
{
    printf("\n== C Minispinnen (RE2-KI, ROOM2030, Boden -5400)\n");
    if (!raum("STAGE2/ROOM2030.RDT", 0x2030, RE15_AI_FLAVOR_RE2, 6550, -5400, 2000, 3072, 0)) { s_fails++; return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e = spinne_bei(6550, 5250);
    CHECK(e != NULL, "Spinne 1 (6550,-5400,5250) gespawnt (ROOM2030 Case 0 @0x1C18)");
    if (!e) return;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
        if (&g_actors[s] != e) g_actors[s].active = 0;   /* nur diese Spinne (die anderen spucken sonst) */
    for (int f = 0; f < 5; f++) { pl->hp = 100; bild(); }
    CHECK(e->y == -5400 && e->dog_floor_y == -5400, "Elternteil auf dem Boden y %d, +0x1C2 %d", e->y, e->dog_floor_y);
    /* Schrotflinte bis zum Tod (der echte Trefferweg re15_player_weapon_fire = FUN_80011f50) */
    re15_inv_init(); re15_inv_grant(8, 30); re15_player_set_equipped_weapon(8);
    int tot = 0;
    for (int k = 0; k < 30 && !tot; k++) {
        pl->x = e->x; pl->z = e->z - 2500; pl->y = -5400; pl->rot_y = 3072; pl->hp = 100; pl->status_flags = 0;
        (void)re15_player_weapon_fire(8);
        for (int f = 0; f < 20; f++) { pl->hp = 100; bild(); }
        int babys = 0;
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x26) babys++;
        if (babys > 0) tot = 1;
    }
    int nb = 0, auf_boden = 0, floor_ok = 0;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *b = &g_actors[s];
        if (!b->active || b->type != 0x26) continue;
        nb++;
        if (b->dog_floor_y == -5400) floor_ok++;
    }
    CHECK(nb > 0, "Babys gespawnt: %d (RE2-Spawner FUN_80105D38, Typ 0x26 @0x80105DE8)", nb);
    CHECK(floor_ok == nb, "alle %d Babys tragen +0x1C2 = -5400 (Boden des Elternteils; vorher 0 = 5400 unter dem Boden)", floor_ok);
    /* Spieler mitten zwischen die Babys; 600 Bilder: Boden halten, Bisse zaehlen */
    int32_t bx = 0, bz = 0, k = 0;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x26) { bx += g_actors[s].x; bz += g_actors[s].z; k++; }
    if (k) { pl->x = bx / k; pl->z = bz / k + 300; }
    pl->y = -5400; pl->hp = 100; pl->status_flags = 0; pl->state = 1;
    int bisse = 0, unter_boden = 0, max_hoch = 0;
    int32_t zeichen_min = 0x7fffffff, zeichen_max = -0x7fffffff;   /* gezeichneter tiefster Punkt - Lage-Y */
    int zeichen_n = 0;
    /* Fortsetzung 2 (2026-10-17): UNABHAENGIGE Bodenmessung statt `y == +0x1C2` (das Feld setzt der Port selbst):
     * bu = hoechste SCA-Bodenflaeche auf/unter dem Baby (re15_spinnen_r38_boden_unter = FUN_8001c6e8 ab dem
     * eigenen Band). Am Boden (Zustand 1 Unterzustand 1, nicht Sprung 3 / Fall 2) muss y == bu sein; vorher lagen
     * 72 % der Baby-Bilder 5400 ueber dem Boden (Dossier 1.6, ohne Wandpass @0x801000c0). */
    int boden_bilder = 0, schwebt = 0, steg_weg = 0;
    int32_t schwebt_x = 0, schwebt_z = 0, schwebt_bu = 0;
    int32_t min_abstand = 0x7fffffff;                              /* Baby am Boden <-> Spieler (Kugel-Schub) */
    for (int f = 0; f < 600; f++) {
        int16_t vor = pl->hp;
        bild();
        if (vor - pl->hp == 1) bisse++;
        if (pl->hp < 50) pl->hp = 100;
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *b = &g_actors[s];
            if (!b->active || b->type != 0x26) continue;
            if (b->state == 1 && b->sub_state_1 == 1) {
                const int32_t bu = re15_spinnen_r38_boden_unter(b);
                boden_bilder++;
                if (b->y < bu - 100) { schwebt++; schwebt_x = b->x; schwebt_z = b->z; schwebt_bu = bu; }
                if (bu != -5400) steg_weg++;
                const int64_t ddx = b->x - pl->x, ddz = b->z - pl->z;
                const int32_t d = (int32_t)re15_squareroot0((uint32_t)(ddx * ddx + ddz * ddz));
                if (d < min_abstand) min_abstand = d;
            }
            if (b->y > -5400) unter_boden++;
            if (-5400 - b->y > max_hoch) max_hoch = -5400 - b->y;
            int ok = 0;
            int32_t t = gezeichnet_tiefster(b, &ok);
            if (ok) { zeichen_n++; if (t < zeichen_min) zeichen_min = t; if (t > zeichen_max) zeichen_max = t; }
        }
    }
    {   /* GEZEICHNET (Nutzerpunkt 3 "fliegen in der Luft"): RE2 spielt das Baby auf Paar 1 Clip 0 (Binder
         * @0x8001abb0/c0, EMS26 `lw a1,264(s0)` / `lw a2,380(s0)` @0x8010005c/60, Clip-Wort 0x000F0000 @0x80100120):
         * 2 Bilder, Wurzel (0,0,0), Vertex-Unterkante 0 = auf der Lage. Vorher (Paar 3): -1797..-1166. */
        re15_enemy_bank_t *bb = re15_enemy_find(0x26);
        CHECK(bb && bb->ok && bb->anim.clip_count == 1 && bb->anim.clips[0].frame_count == 2,
              "Babybank = Paar 1: %d Clip(s), Clip 0 mit %d Bildern (RE2 dir[1]/dir[2], vorher Paar 3 mit 3 Clips)",
              bb ? bb->anim.clip_count : -1, bb ? bb->anim.clips[0].frame_count : -1);
        printf("   gezeichnete Unterkante der Babys relativ zur Lage: %d .. %d (%d Messungen)\n", zeichen_min,
               zeichen_max, zeichen_n);
        CHECK(zeichen_n > 0 && zeichen_min >= -10 && zeichen_max <= 10,
              "Babys GEZEICHNET auf ihrer Lage: Unterkante %d..%d (vorher -1797..-1166 = schwebend)", zeichen_min,
              zeichen_max);
    }
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x26) auf_boden++;
    printf("   Babys aktiv %d, Bisse %d, Bilder unter dem Boden %d, hoechster Sprung %d\n", auf_boden, bisse, unter_boden,
           max_hoch);
    CHECK(unter_boden == 0, "kein Baby unter dem Boden (y > -5400) in 600 Bildern (%d)", unter_boden);
    printf("   Babys am Boden: %d Bilder, davon ueber dem SCA-Boden %d, nicht mehr auf dem Steg %d%s\n", boden_bilder,
           schwebt, steg_weg, schwebt ? " (letztes: siehe naechste Zeile)" : "");
    if (schwebt) printf("   schwebend zuletzt bei (%d,%d), Boden darunter %d\n", schwebt_x, schwebt_z, schwebt_bu);
    CHECK(boden_bilder > 0 && schwebt == 0 && steg_weg == 0,
          "Babys bleiben auf dem Steg: 0 von %d Boden-Bildern ueber dem SCA-Boden (Wandpass FUN_8003567C(self,0x400) "
          "@0x801000c0, Radius +0x90 = 10 @0x80100180)", boden_bilder);
    CHECK(min_abstand >= 400,
          "Kugel-Schub FUN_80035530 @0x801000b4: kein Baby am Boden naeher als 400 am Spieler (min %d; Kugeln 450 + 10 "
          "@0x8003bdc0 / @0x80100174, ohne Schub bis 0)", min_abstand);
    CHECK(bisse > 0, "Babys beissen: %d Bisse a 1 HP (`addiu a0,zero,1` @0x80100EA8, Reichweite Ziel+0x9A 450 + 10)", bisse);
    /* Sprung @0x80100880: vy = -300 (@0x801008C4), y += vy, vy += 40 (@0x801008FC-904) -> Scheitel nach 8 Bildern
     * 300+260+220+180+140+100+60+20 = 1280 ueber dem Absprung (Boden). */
    CHECK(max_hoch <= 1280, "Sprung im RE2-Rahmen: hoechster Punkt %d ueber dem Boden (<= 1280 = Scheitel vy -300/+40)",
          max_hoch);
}

/* ============================== D: ZENSUS "keine Spinne schwebt" ============================== */
/* Nutzerpunkt 1 ("die Spinnen fliegen FAST ALLE in der Luft"): jede Sce_em_set-Spinne (Typ 0x25) ALLER Stage-2-Raeume,
 * beide KI-Geschmaecker, 300 Bilder am Sprungpunkt des Debug-Menues (DEBUG.BIN-Tabelle, gemessen in der exe) bzw. an der
 * Tuerlage 2060 -> 2030 (Cut 13, oberer Steg). Erlaubt sind genau zwei Lagen: (a) auf dem SCA-Boden unter der Spinne
 * (|y - bu| <= 100, bu = re15_spinnen_r38_boden_unter = FUN_8001c6e8 ab dem eigenen Band — UNABHAENGIG vom eigenen Feld
 * +0x1ba, das Sce_em_set selbst setzt (`sh v0,442(s0)` @0x80042210); Fortsetzung 2 ersetzt damit die
 * selbstbestaetigende Pruefung `y == +0x1ba`) oder (b) wartend im Schacht (grid 0x41, Zustand 4/1, y = Spawnhoehe;
 * Ausloeser erst Cut 1/5 bzw. 4/5, Teil A). Vorher (Stand e2cc7673) liefen die Schachtspinnen unter der RE2-KI im
 * Bodenmodus frei auf der Schachthoehe herum (Dossier 1.2). */
static void teil_d(void)
{
    static const struct { const char *datei; unsigned id; int32_t x, y, z; int16_t rot; uint8_t cut; } R[] = {
        { "STAGE2/ROOM2030.RDT", 0x2030,  6550,  -5400,   -100, 2048,  0 },
        { "STAGE2/ROOM2030.RDT", 0x2030, -6950, -16200,  -7200,    0, 13 },   /* Tuer 2060 -> 2030 (Datei 0x0D9A) */
        { "STAGE2/ROOM2050.RDT", 0x2050,  9100,  -5400,   4100, 2048,  0 },
        { "STAGE2/ROOM2060.RDT", 0x2060,  7600,      0,   3950, 2048,  0 },
        { "STAGE2/ROOM2070.RDT", 0x2070,  4851,      0,  -6250, 2048,  0 },
        { "STAGE2/ROOM2090.RDT", 0x2090, -8200,      0, -25550, 2048,  0 },
        { "STAGE2/ROOM20A0.RDT", 0x20A0, -2800,  -3600, -22200, 2048,  0 },
    };
    printf("\n== D Zensus: Lage aller Stage-2-Spinnen (beide KI, 300 Bilder je Raum)\n");
    for (int fl = 0; fl < 2; fl++) {
        re15_ai_flavor_t flv = fl ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15;
        for (size_t r = 0; r < sizeof R / sizeof R[0]; r++) {
            if (!raum(R[r].datei, R[r].id, flv, R[r].x, R[r].y, R[r].z, R[r].rot, R[r].cut)) { s_fails++; continue; }
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            int n_sp = 0, boden = 0, schacht = 0, schwebt = 0, im_block = 0;
            int32_t schwebt_y = 0, schwebt_gy = 0;
            for (int f = 0; f < 300; f++) {
                pl->hp = 100;                                   /* Messlauf: Leon bleibt stehen, stirbt nicht */
                bild();
                for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
                    const re15_actor_t *e = &g_actors[s];
                    if (!e->active || e->type != 0x25) continue;
                    if (f == 0) n_sp++;
                    /* grid 0x41 = Schachtspinne: solange das Skript grid nicht aendert, MUSS sie im Schacht warten
                     * (Zustand 4/1 @0x80110d14, Phase 0 `beq` @0x80114f18-1c). Eine 0x41-Spinne in einem anderen
                     * Zustand laeuft frei auf Schachthoehe herum (Stand e2cc7673 unter der RE2-KI: Zustand 1, Modus 0,
                     * y -3600 ueber dem Boden 0) — y == +0x1ba ist dort KEIN Bodenbeweis, weil Sce_em_set +0x1ba aus
                     * floor 2 setzt (-3600). */
                    if (e->grid_id == 0x41) {
                        if (e->state == 4 && e->sub_state_1 == 1) schacht++;
                        else { schwebt++; schwebt_y = e->y; schwebt_gy = e->dog_floor_y; }
                    }
                    else {
                        const int32_t bu = re15_spinnen_r38_boden_unter(e);
                        if (e->y < bu - 100)      { schwebt++; schwebt_y = e->y; schwebt_gy = bu; }
                        else if (e->y > bu + 100) im_block++;
                        else                      boden++;
                    }
                }
            }
            CHECK(n_sp > 0 && schwebt == 0 && im_block == 0,
                  "%s ROOM%04X Cut %u: %d Spinnen, Spinnen-Bilder am SCA-Boden %d, im Schacht wartend %d, schwebend %d, "
                  "im Block %d%s", fl ? "RE2" : "RE1.5", R[r].id, (unsigned)R[r].cut, n_sp, boden, schacht, schwebt,
                  im_block, schwebt ? " (letzte: y/Boden siehe naechste Zeile)" : "");
            if (schwebt) printf("   schwebend zuletzt y %d Boden %d\n", schwebt_y, schwebt_gy);
        }
    }
}

/* ============================== pruef_log: Auswerter fuer den Echtlauf ==============================
 * Aufruf `test_r38_spinnen pruef_log <spinnen.log> <raum-hex>` (integration_r38_spinnen_echtlauf). Liest die Messschiene
 * RE15_SPINNEN_LOG der echten exe (spinnen_r38.c re15_spinnen_r38_log_frame) und prueft je Nutzerpunkt:
 *   Punkt 3  Babys (Typ 0x26) entstehen; in keinem Bild am Boden (Zustand 1 Unterzustand 1) liegt ein Baby mehr als 100
 *            UEBER dem SCA-Boden bu (Wandpass @0x801000c0); Bisse (HP -1) > 0
 *   Punkt 1  keine Adultspinne (Typ 0x25) ausserhalb der Schacht-Choreografie (Zustand 4/1) mehr als 100 ueber bu
 *   Punkt 2  der Spieler traegt mindestens einmal das Gift-Bit 0x2 (Status-Wort ps)
 *   Punkt 2  (Nachbesserung 1) jeder Gift-Takt (gz 0 -> 0x78/8 mit HP -2, @0x80031e78-ac) OHNE fremden HP-Abzug im Fenster
 *            +-25 Bilder loest KEINE Treffer-Reaktion aus (phf / Clip 8/9/0xa in den 25 Bildern danach), und zwei Takte
 *            ohne fremden Abzug dazwischen liegen genau 121 Bilder auseinander (Zaehler 0x78 + 1). Original: der Takt
 *            schreibt nur HP und Zaehler (@0x80031e44-ec8), die Reaktion nur ein Kommando-2-Schreiber (@0x80012ebc).
 *   Spieler  |y - bu| <= 100 in jedem Bild (Gegenprobe der Messgroesse). */
#define PRUEF_MAX_BILDER 20000
static int16_t s_pl_hp[PRUEF_MAX_BILDER];
static uint8_t s_pl_gz[PRUEF_MAX_BILDER], s_pl_reakt[PRUEF_MAX_BILDER];
static unsigned s_pl_f[PRUEF_MAX_BILDER];
static int pruef_log(const char *pfad, unsigned raum)
{
    FILE *f = fopen(pfad, "r");
    if (!f) { printf("FAIL: pruef_log: %s fehlt\n", pfad); return 1; }
    static char z[65536];
    long bilder = 0, baby_boden = 0, baby_schwebt = 0, adult = 0, adult_schwebt = 0, pl_falsch = 0;
    int bisse = 0, gift = 0, babys_max = 0, vor_hp = -100000, n_pl = 0, mit_reakt_feld = 0;
    while (fgets(z, sizeof z, f)) {
        unsigned fr = 0, r = 0, ps = 0, fl = 0, gz = 0;
        int px = 0, py = 0, pz = 0, hp = 0, st = 0, bo = 0, bu = 0;
        if (sscanf(z, "F%u R%X PL(%d,%d,%d hp=%d st=%d ps=%x fl=%u gz=%u bo=%d bu=%d)", &fr, &r, &px, &py, &pz, &hp,
                   &st, &ps, &fl, &gz, &bo, &bu) != 12) continue;
        if (r != raum) continue;
        if (n_pl < PRUEF_MAX_BILDER) {
            int pmo = -1, phf = 0;
            const char *q = strstr(z, ") pmo=");
            if (q && sscanf(q, ") pmo=%d phf=%d", &pmo, &phf) == 2) mit_reakt_feld = 1;
            s_pl_hp[n_pl] = (int16_t)hp; s_pl_gz[n_pl] = (uint8_t)gz; s_pl_f[n_pl] = fr;
            s_pl_reakt[n_pl] = (uint8_t)(phf != 0 || pmo == 8 || pmo == 9 || pmo == 0x0a);
            n_pl++;
        }
        bilder++;
        if (py - bu > 100 || bu - py > 100) pl_falsch++;
        if (ps & 0x2u) gift = 1;
        if (vor_hp != -100000 && vor_hp - hp == 1) bisse++;
        vor_hp = hp;
        int babys = 0;
        for (const char *q = strchr(z, '['); q; q = strchr(q + 1, '[')) {
            int slot, t, est, s1, s2, s3, g, efl, m, gy, ebo, ebu, mo, af, x, y, zz;
            if (sscanf(q, "[%d t=%x st=%d s=%d.%d.%d g=%x fl=%d m=%d gy=%d bo=%d bu=%d mo=%d af=%d @(%d,%d,%d)", &slot, &t,
                       &est, &s1, &s2, &s3, &g, &efl, &m, &gy, &ebo, &ebu, &mo, &af, &x, &y, &zz) != 17) continue;
            if (t == 0x26) {
                babys++;
                if (est == 1 && s1 == 1) { baby_boden++; if (y < ebu - 100) baby_schwebt++; }
            } else if (t == 0x25) {
                if (est == 4 && s1 == 1) continue;                     /* Schacht-Choreografie @0x80114ec8 */
                if (est == 0 && g == 0x41) continue;                   /* Schachtspinne vor dem ersten Takt */
                adult++;
                if (y < ebu - 100) adult_schwebt++;
            }
        }
        if (babys > babys_max) babys_max = babys;
    }
    fclose(f);
    printf("   pruef_log ROOM%04X: %ld Bilder, Babys bis %d, Baby-Boden-Bilder %ld (schwebend %ld), Adult-Bilder %ld "
           "(schwebend %ld), Bisse %d, Gift %d, Spieler |y-bu|>100 in %ld\n", raum, bilder, babys_max, baby_boden,
           baby_schwebt, adult, adult_schwebt, bisse, gift, pl_falsch);
    CHECK(bilder > 300, "Lauf in ROOM%04X: %ld Bilder", raum, bilder);
    CHECK(pl_falsch == 0, "Messgroesse bu am Spieler gegengeprueft (|y - bu| <= 100 in jedem Bild, falsch %ld)", pl_falsch);
    CHECK(babys_max > 0 && baby_boden > 0, "Punkt 3: Babys entstehen (bis %d gleichzeitig, %ld Boden-Bilder)", babys_max,
          baby_boden);
    CHECK(baby_schwebt == 0, "Punkt 3: kein Baby am Boden ueber dem SCA-Boden (%ld von %ld; Wandpass @0x801000c0)",
          baby_schwebt, baby_boden);
    CHECK(bisse > 0, "Punkt 3: Babys beissen (%d x 1 HP, `addiu a0,zero,1` @0x80100EA8)", bisse);
    CHECK(adult > 0 && adult_schwebt == 0, "Punkt 1: keine Adultspinne ueber dem SCA-Boden (%ld von %ld)", adult_schwebt,
          adult);
    CHECK(gift, "Punkt 2: der Spieler wird vergiftet (Status-Bit 0x2, Routine 25 / RE2 Op 32 @0x80020130)");

    /* Nachbesserung 1, Mangel 1: Gift-Takte in der echten exe */
    int takte = 0, sauber = 0, reakt = 0, perioden = 0, perioden_falsch = 0, letzter = -1, takt_bild[64];
    for (int i = 1; i < n_pl; i++) {
        if (!(s_pl_gz[i - 1] == 0 && (s_pl_gz[i] == 0x78 || s_pl_gz[i] == 8) && s_pl_hp[i - 1] - s_pl_hp[i] == 2)) continue;
        if (takte < 64) takt_bild[takte] = i;
        takte++;
        int fremd = 0;                                   /* fremder HP-Abzug (Biss/Spucken) im Fenster +-25 */
        for (int j = i - 25; j <= i + 25; j++)
            if (j >= 1 && j < n_pl && j != i && s_pl_hp[j] < s_pl_hp[j - 1]) fremd = 1;
        if (!fremd) {
            sauber++;
            for (int j = i + 1; j <= i + 25 && j < n_pl; j++) if (s_pl_reakt[j]) { reakt++; break; }
        }
        if (letzter >= 0) {                              /* Periode nur ohne fremden Abzug dazwischen */
            int fremd_dazw = 0;
            for (int j = letzter + 1; j < i; j++) if (s_pl_hp[j] < s_pl_hp[j - 1]) fremd_dazw = 1;
            if (!fremd_dazw) { perioden++; if (i - letzter != 121) perioden_falsch++; }
        }
        letzter = i;
    }
    printf("   Gift-Takte %d (Bilder", takte);
    for (int k = 0; k < takte && k < 64; k++) printf(" F%u", s_pl_f[takt_bild[k]]);
    printf("), ohne fremden Abzug %d, davon mit Treffer-Reaktion %d; Perioden ohne Fremdtreffer %d, davon != 121: %d\n",
           sauber, reakt, perioden, perioden_falsch);
    CHECK(mit_reakt_feld, "Punkt 2: Messschiene traegt pmo/phf (Spieler-Clip, Flinch)");
    CHECK(sauber > 0 && reakt == 0, "Punkt 2: Gift-Takt ohne Treffer-Reaktion (%d von %d sauberen Takten reagierten; "
          "Original @0x80031e44-ec8 nur HP + Zaehler)", reakt, sauber);
    CHECK(perioden > 0 && perioden_falsch == 0, "Punkt 2: Gift-Periode 121 Bilder (%d Perioden, %d abweichend; vorher 143)",
          perioden, perioden_falsch);
    return s_fails ? 1 : 0;
}

int main(int argc, char **argv)
{
    if (argc >= 4 && strcmp(argv[1], "pruef_log") == 0)
        return pruef_log(argv[2], (unsigned)strtoul(argv[3], NULL, 16));
    re15_actor_t *a15 = teil_a(RE15_AI_FLAVOR_RE15, "RE1.5");
    if (a15) teil_b(a15, 0, "RE1.5");
    teil_b_wirkung();
    teil_b2();
    teil_b3();
    teil_b4();
    teil_b5();
    teil_a2(RE15_AI_FLAVOR_RE15, "RE1.5");
    re15_actor_t *a2 = teil_a(RE15_AI_FLAVOR_RE2, "RE2");
    if (a2) teil_b(a2, 1, "RE2");
    teil_c();
    teil_d();
    printf("\ntest_r38_spinnen: %d FAIL\n", s_fails);
    return s_fails ? 1 : 0;
}
