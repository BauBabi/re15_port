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
#include "re15_spinnen_r38.h"

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
    for (int f = 0; f < 600; f++) {
        int16_t vor = pl->hp;
        bild();
        if (vor - pl->hp == 1) bisse++;
        if (pl->hp < 50) pl->hp = 100;
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *b = &g_actors[s];
            if (!b->active || b->type != 0x26) continue;
            if (b->y > -5400) unter_boden++;
            if (-5400 - b->y > max_hoch) max_hoch = -5400 - b->y;
        }
    }
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x26) auf_boden++;
    printf("   Babys aktiv %d, Bisse %d, Bilder unter dem Boden %d, hoechster Sprung %d\n", auf_boden, bisse, unter_boden,
           max_hoch);
    CHECK(unter_boden == 0, "kein Baby unter dem Boden (y > -5400) in 600 Bildern (%d)", unter_boden);
    CHECK(bisse > 0, "Babys beissen: %d Bisse a 1 HP (`addiu a0,zero,1` @0x80100EA8, Reichweite Ziel+0x9A 450 + 10)", bisse);
    /* Sprung @0x80100880: vy = -300 (@0x801008C4), y += vy, vy += 40 (@0x801008FC-904) -> Scheitel nach 8 Bildern
     * 300+260+220+180+140+100+60+20 = 1280 ueber dem Absprung (Boden). */
    CHECK(max_hoch <= 1280, "Sprung im RE2-Rahmen: hoechster Punkt %d ueber dem Boden (<= 1280 = Scheitel vy -300/+40)",
          max_hoch);
}

int main(void)
{
    re15_actor_t *a15 = teil_a(RE15_AI_FLAVOR_RE15, "RE1.5");
    if (a15) teil_b(a15, 0, "RE1.5");
    teil_b_wirkung();
    teil_a2(RE15_AI_FLAVOR_RE15, "RE1.5");
    re15_actor_t *a2 = teil_a(RE15_AI_FLAVOR_RE2, "RE2");
    if (a2) teil_b(a2, 1, "RE2");
    teil_c();
    printf("\ntest_r38_spinnen: %d FAIL\n", s_fails);
    return s_fails ? 1 : 0;
}
