/* test_r38_griff_alle.c — Runde 38 Spur T, Punkt 2 GENERELL: "Sowas darf GARNICHT passieren, bei keinen Gegner."
 * (Dossier analysis/befunde_runde38/T_spiderman.md, Abschnitt 2b Zensus.)
 *
 * Ein Riegel ueber die Griffe ALLER Greifer, die den Spieler absolut platzieren (generische Opfer-Maschine
 * re15_victim_place = FUN_8001ad68 / RE2 0x80015CB8, bzw. eigene Platzierung). Jeder Fall laeuft am echten Raum mit
 * dem echten re15_game_step, und vor dem Griff ist der Spieler-Anker VERGIFTET (ein Punkt weit ausserhalb des
 * Raums = der Anker eines frueheren Griffs). Ein Greifer, der seinen Anker nicht auf den Spieler kopiert (die
 * Fehlerklasse von 585c239a Hund, 9661e369 Arme, Runde 38 Spiderman), stellt den Spieler dorthin.
 *
 * Invariante je Griff-/Kollapsbild: (a) innerhalb der Huelle aller SCA-Zellen des Raums; (b) die Strecke von der
 * Lage vor dem Griff zur Lage jetzt schneidet keine solide Rechteckzelle (Typ 1, u0 & 1 = Spieler-Maske) des
 * Spielerbandes — kein Griff setzt den Spieler hinter eine Wand (ROOM1220, Runde 37 J_bissarm OFFEN 1).
 *
 * Aufruf: test_r38_griff_alle <fall>   (Faelle s. k_faelle). Rueckgabe 0 gruen, 1 rot, 77 SKIP (Assets). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
#include "re15_md1.h"
#include "re15_tim.h"
#include "re2_ems.h"
#include "re15_skeleton.h"
#include "re15_ems.h"
#include "re15_roy_r37.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t *s_ems15[2];  static size_t s_ems15_n[2];
static uint8_t *s_ems2;      static size_t s_ems2_n;
static int s_fail = 0;

#define CHECK(c, ...) do { if (!(c)) { s_fail++; printf("  FAIL: "); printf(__VA_ARGS__); printf("\n"); } \
                           else { printf("  ok:   "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* RE1.5-Bank aus dem Archiv des Szenarios (Leon CDEMD0 / Elza CDEMD1, @0x8002233c/@0x800223ac). */
static int bank15(uint8_t typ, int archiv)
{
    re15_enemy_bank_t *eb = re15_enemy_find(typ);
    if (eb && eb->ok) return 1;
    if (!s_ems15[archiv]) {
        char p[600]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(archiv));
        s_ems15[archiv] = slurp(p, &s_ems15_n[archiv]);
    }
    uint32_t sek = 0, len = 0;
    if (!s_ems15[archiv] || !re15_ems_toc(archiv, typ, &sek, &len)) return 0;
    const uint8_t *blob = s_ems15[archiv] + (size_t)sek * 2048u;
    if (!eb) eb = re15_enemy_alloc(typ);
    if (!eb) return 0;
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return 0; }
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(blob, len, &eb->skel_own, &eb->anim_own) == 0);
    return 1;
}

/* RE2-Bank (shared_assets/RE2/CDEMD0.EMS) + der Hybrid der exe (RE1.5-Netz/-Bindlaengen), wie test_r37_bissarm. */
static int bank2(uint8_t typ)
{
    re15_enemy_bank_t *eb = re15_enemy_find(typ);
    if (eb && eb->ok) return 1;
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems2) return 0;
    if (!eb) eb = re15_enemy_alloc(typ);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)typ, eb, NULL) != 0) { eb->type = 0; return 0; }
    eb->buf = NULL; eb->ok = 1;
    if (!s_ems15[0]) {
        char p[600]; snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
        s_ems15[0] = slurp(p, &s_ems15_n[0]);
    }
    int idx = re15_ems_index_for_type(typ); size_t off = 0, len = 0;
    if (s_ems15[0] && idx >= 0 && re15_ems_get_entry(s_ems15[0], s_ems15_n[0], idx, &off, &len) == 0) {
        static re15_md1_t md15; static re15_emd_skeleton_t sk15; static re15_emd_animation_t an15;
        uint8_t *blob = (uint8_t *)malloc(len); memcpy(blob, s_ems15[0] + off, len);
        if (re15_emd_parse_container(blob, len, &md15, &sk15, &an15, NULL) == 0) {
            int um = -1; (void)re2_hybrid_apply(eb, (int)typ, &md15, &sk15, &um);
        }
    }
    return 1;
}

static void bild(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

typedef struct {
    const char *name;
    unsigned room; int flavor; int archiv;
    uint8_t typ_lo, typ_hi;          /* Greifer-Typ-Bereich */
    int32_t px, pz; int rot;         /* Spieler */
    int abst; int hinten;            /* Greifer abst vor (hinten: hinter) dem Spieler, Blick zum Spieler */
    int zwang_sub;                   /* >= 0: nach INIT Zustand 1, +0x5 = zwang_sub, +0x6 = 0 */
    int hp;                          /* Spieler-hp bis zum Griff (darunter auffuellen) */
    int nflags; uint8_t flags[4][2];
} fall_t;

/* Positionen: 1140 frei (-1000,-19600) wie unit_r37_bissarm_klemme_1140; 1220 duenne Wand wie J_bissarm OFFEN 1;
 * 11C0 Kampf-Layout (Ck(4,0x40)=1, Ck(3,0x43)=0: Gorillas grid 0x10, J_affen.md); 4001 Spiderman draussen. */
static const fall_t k_faelle[] = {
    { "zombie15_vorn",   0x1140, RE15_AI_FLAVOR_RE15, 0, 0x10, 0x18, -1000, -19600, 0, 700, 0,  3, 100, 0, {{0}} },
    { "zombie15_hinten", 0x1140, RE15_AI_FLAVOR_RE15, 0, 0x10, 0x18, -1000, -19600, 0, 700, 1,  4, 100, 0, {{0}} },
    { "zombie2_vorn",    0x1140, RE15_AI_FLAVOR_RE2,  0, 0x10, 0x18, -1000, -19600, 0, 700, 0, -1, 100, 0, {{0}} },
    { "zombie2_hinten",  0x1140, RE15_AI_FLAVOR_RE2,  0, 0x10, 0x18, -1000, -19600, 0, 700, 1, -1, 100, 0, {{0}} },
    { "kriecher2_1220",  0x1220, RE15_AI_FLAVOR_RE2,  0, 0x10, 0x18, -21000, -5500, 0, 700, 0, -1, 100, 0, {{0}} },
    { "hund15",          0x1140, RE15_AI_FLAVOR_RE15, 0, 0x20, 0x20, -1000, -19600, 0, 700, 0,  9, 100, 0, {{0}} },
    { "hund2",           0x1140, RE15_AI_FLAVOR_RE2,  0, 0x20, 0x20, -1000, -19600, 0, 1500, 0, -1, 5, 0, {{0}} },
    { "gorilla",         0x11C0, RE15_AI_FLAVOR_RE2,  0, 0x27, 0x27, -9013, -13961, 1024, 1500, 0, 15, 100, 1, {{4, 0x40}} },
    { "spiderman_vorn",  0x4001, RE15_AI_FLAVOR_RE2,  1, 0x2B, 0x2B, -12000, -9000, 1024, 1500, 0, 5, 40, 4,
                          {{4, 20}, {3, 0x21}, {3, 0x91}, {3, 0xAF}} },
    { "spiderman_hinten",0x4001, RE15_AI_FLAVOR_RE2,  1, 0x2B, 0x2B, -12000, -9000, 1024, 1500, 1, 9, 40, 4,
                          {{4, 20}, {3, 0x21}, {3, 0x91}, {3, 0xAF}} },
    /* Runde 38 Integration (Spur S Rueckstoss): der kleine Affe 0x29 (Spur S) greift von der Decke, nur von HINTEN
     * (A[13]: a780(Spieler) != 0 @0x80113a24-a94) -> B[15] 0x801142c4 P0: Anker = Spieler-Lage (`sh` acaf4/acaf6
     * @0x801143b0-b8), aca58 = 5 (@0x8011440c); der Spieler-Haken 0x80115d70 platziert absolut (FUN_8001ad68
     * @0x80115e1c/@0x80115e74/@0x80115f34) — dieselbe Fehlerklasse wie oben, deshalb hier. ROOM3040 Band 0 im
     * Westteil (Leon (-25600,-1350) Blick +z, Westwand Zelle 0 bei x -26800, Nordwand Zelle 3 bei z 200). */
    { "affe3_hinten",    0x3040, RE15_AI_FLAVOR_RE2,  0, 0x29, 0x29, -25600, -1350, 3072, 1000, 1, 15, 100, 0, {{0}} },
};

static int32_t s_hx0, s_hx1, s_hz0, s_hz1;
static void huelle(void)
{
    s_hx0 = s_hz0 = 0x7fffffff; s_hx1 = s_hz1 = -0x7fffffff;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        int32_t x0 = c->x, z0 = c->z, x1 = x0 + (int32_t)c->width, z1 = z0 + (int32_t)c->density;
        if (x0 < s_hx0) s_hx0 = x0; if (z0 < s_hz0) s_hz0 = z0;
        if (x1 > s_hx1) s_hx1 = x1; if (z1 > s_hz1) s_hz1 = z1;
    }
}
static int im_raum(int32_t x, int32_t z) { return x >= s_hx0 && x <= s_hx1 && z >= s_hz0 && z <= s_hz1; }

/* Strecke gegen das INNERE solider Rechteckzellen (Typ 1, u0 & 1) des Bandes — unabhaengig vom Port-Waechter
 * gerechnet (eigener Slab-Test), damit der Riegel nicht den Pruefling mit sich selbst vergleicht. */
static int durch_wand(int32_t x0, int32_t z0, int32_t x1, int32_t z1, int band)
{
    if (x0 == x1 && z0 == z1) return 0;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if ((c->floor >> 4) != band || !(c->u0 & 1u) || c->type != 1) continue;
        double ax = c->x, az = c->z, bx = ax + c->width, bz = az + c->density;
        double lo = 0, hi = 1, px = x0, pz = z0, dx = (double)x1 - x0, dz = (double)z1 - z0;
        int ok = 1;
        if (dx == 0) { if (px <= ax || px >= bx) ok = 0; }
        else { double t0 = (ax - px) / dx, t1 = (bx - px) / dx; if (t0 > t1) { double t = t0; t0 = t1; t1 = t; }
               if (t0 > lo) lo = t0; if (t1 < hi) hi = t1; if (!(lo < hi)) ok = 0; }
        if (ok) {
            if (dz == 0) { if (pz <= az || pz >= bz) ok = 0; }
            else { double t0 = (az - pz) / dz, t1 = (bz - pz) / dz; if (t0 > t1) { double t = t0; t0 = t1; t1 = t; }
                   if (t0 > lo) lo = t0; if (t1 < hi) hi = t1; if (!(lo < hi)) ok = 0; }
        }
        if (ok) return 1;
    }
    return 0;
}

static int fall_laufen(const fall_t *F)
{
    printf("== %s: ROOM%04X, KI %s ==\n", F->name, F->room,
           F->flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : (F->flavor == RE15_AI_FLAVOR_RE15 ? "RE1.5" : "MIXED"));
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (F->room >> 12) & 0xFu, F->room);
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("SKIP: %s fehlt\n", path); return 77; }
    memset(&g_room_rdt, 0, sizeof g_room_rdt);
    if (re15_rdt_parse(buf, sz, &g_room_rdt) != 0) { printf("FAIL: RDT\n"); return 1; }
    g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &g_room_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(F->flavor);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = F->room;
    for (int i = 0; i < F->nflags; i++) re15_game_flag_set(F->flags[i][0], F->flags[i][1], 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0;
    pl->floor = 0; pl->y = 0; pl->x = F->px; pl->z = F->pz; pl->rot_y = (int16_t)F->rot;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    scd_register_room_events(&g_room_rdt);
    scd_room_reenter(&g_room_rdt, F->px, F->pz, 0);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type < 0x10 || e->type >= 0x40) continue;
        /* wie der exe-Lader (platform/pc/main.c pc_enemy_load: RE2-Bank nur bei re2_for_type && re2_owns_type) */
        if (re15_ai_re2_for_type(e->type) && re15_re2_owns_type(e->type)) (void)bank2(e->type);
        else (void)bank15(e->type, F->archiv);
    }
    for (int f = 0; f < 4; f++) bild();
    /* Greifer: der naechste aktive Gegner im Typbereich; alle anderen Gegner aus. */
    int g = -1; long best = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type < F->typ_lo || e->type > F->typ_hi) continue;
        long dx = e->x - F->px, dz = e->z - F->pz, d = dx * dx + dz * dz;
        if (best < 0 || d < best) { best = d; g = s; }
    }
    if (g < 0) {
        /* Hund: in ROOM1140 gibt es keinen — wie probe_dog_devour von Hand setzen. */
        if (F->typ_lo == 0x20) {
            for (int s = 1; s < RE15_ACTOR_MAX && g < 0; s++) if (!g_actors[s].active) g = s;
            re15_actor_t *d = &g_actors[g];
            memset(d, 0, sizeof *d);
            d->active = 1; d->type = 0x20; d->state = 0; d->em_flag_id = 0xFF;
            re15_enemy_apply_hitbox(d, 0x20);
            if (re15_ai_re2_for_type(0x20) && re15_re2_owns_type(0x20)) (void)bank2(0x20);
            else (void)bank15(0x20, F->archiv);
        } else { printf("FAIL: kein Greifer 0x%02X..0x%02X im Raum\n", F->typ_lo, F->typ_hi); return 1; }
    }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s != g && g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x40) g_actors[s].active = 0;
    re15_actor_t *z = &g_actors[g];
    re15_enemy_bank_t *zb = re15_enemy_find(z->type);
    if (!zb || !zb->ok) { printf("SKIP: Bank Typ 0x%02X fehlt\n", z->type); return 77; }
    int32_t sn = re15_sin_q12(F->rot), cs = re15_cos_q12(F->rot);
    int vz = F->hinten ? -1 : 1;
    pl->x = F->px; pl->z = F->pz; pl->rot_y = (int16_t)F->rot; pl->hp = (int16_t)F->hp;
    z->x = F->px + vz * (int32_t)(((int64_t)cs * F->abst) >> 12);
    z->z = F->pz - vz * (int32_t)(((int64_t)sn * F->abst) >> 12);
    z->y = 0;
    z->rot_y = (int16_t)((F->rot + (F->hinten ? 0 : 2048)) & 0x0fff);
    if (F->zwang_sub >= 0) {
        if (z->state == 0) bild();                              /* INIT -> ACTIVE */
        z->x = F->px + vz * (int32_t)(((int64_t)cs * F->abst) >> 12);
        z->z = F->pz - vz * (int32_t)(((int64_t)sn * F->abst) >> 12);
        z->rot_y = (int16_t)((F->rot + (F->hinten ? 0 : 2048)) & 0x0fff);
        z->state = 1; z->grid_id = 0; z->sub_state_1 = (uint8_t)F->zwang_sub; z->sub_state_2 = 0; z->sub_state_3 = 0;
        if (z->hp <= 0) z->hp = 100;
    }
    huelle();
    const int32_t gift_x = s_hx1 + 20000, gift_z = s_hz1 + 20000;   /* Anker eines frueheren Griffs, ausserhalb */
    pl->anchor_x = gift_x; pl->anchor_z = gift_z;
    {   extern void re15_victim_anchor_calibrate(int32_t, int32_t);
        re15_victim_anchor_calibrate(gift_x, gift_z); }   /* auch der "letzte begehbare Standpunkt" stammt aus einem frueheren Griff */
    printf("  Greifer slot %d Typ 0x%02X (%d,%d) r%d, Spieler (%d,%d) r%d, Huelle (%d,%d)..(%d,%d), Gift-Anker (%d,%d)\n",
           g, z->type, (int)z->x, (int)z->z, (int)z->rot_y, (int)pl->x, (int)pl->z, (int)pl->rot_y,
           (int)s_hx0, (int)s_hz0, (int)s_hx1, (int)s_hz1, (int)gift_x, (int)gift_z);

    int latch = -1, griff = 0, aus = 0, durch = 0, anker_kopie = -1;
    int32_t vor_x = pl->x, vor_z = pl->z, maxd = 0, maxsprung = 0, lx = pl->x, lz = pl->z;
    int band = re15_collision_debug_band(); if (band < 0) band = 0;
    for (int f = 0; f < 1500; f++) {
        if (latch < 0) { extern void re15_victim_anchor_calibrate(int32_t, int32_t); re15_victim_anchor_calibrate(gift_x, gift_z); }
        int v0 = re15_player_victim_state();
        if (latch < 0 && v0 == 0 && !(pl->hit_react & 1) && F->zwang_sub < 0) {
            pl->x = F->px; pl->z = F->pz; pl->rot_y = (int16_t)F->rot;  /* bis zum Griff festhalten (bissarm) */
        }
        if (latch < 0 && pl->hp < F->hp) pl->hp = (int16_t)F->hp;
        if (latch < 0) { pl->anchor_x = gift_x; pl->anchor_z = gift_z; }   /* Gift bis zum Latch erneuern */
        int32_t ox = pl->x, oz = pl->z;
        bild();
        int v = re15_player_victim_state();
        if (latch < 0 && v != 0) {
            latch = f; vor_x = ox; vor_z = oz; lx = ox; lz = oz;
            anker_kopie = !(pl->anchor_x == gift_x && pl->anchor_z == gift_z);
            printf("  Latch Bild %d: Opferzustand %d, Greifer +0x5 %d, Spieler-Anker (%d,%d), Greifer-Anker (%d,%d)\n", f, v,
                   z->sub_state_1, (int)pl->anchor_x, (int)pl->anchor_z, (int)z->anchor_x, (int)z->anchor_z);
        }
        if (latch >= 0 && v != 0) {
            griff++;
            if (!im_raum(pl->x, pl->z)) aus++;
            if (durch_wand(vor_x, vor_z, pl->x, pl->z, band)) {
                if (durch < 3) printf("  Bild %d: Spieler (%d,%d) hinter einer Wand (vor dem Griff (%d,%d))\n", f,
                                      (int)pl->x, (int)pl->z, (int)vor_x, (int)vor_z);
                durch++;
            }
            int32_t d = re15_enemy_player_dist(z, pl); if (d > maxd) maxd = d;
            int32_t sp = (int32_t)sqrt((double)(pl->x - lx) * (pl->x - lx) + (double)(pl->z - lz) * (pl->z - lz));
            if (sp > maxsprung) maxsprung = sp;
            if (getenv("R38_GRIFF_LOG") && (sp > 1000 || f < latch + 3 || (f > 85 && f < 95)))
                printf("    Bild %d: Spieler (%d,%d) a(%d,%d) r%d mo %d af %d vs %d | Greifer (%d,%d) r%d +0x5 %d +0x6 %d mo %d af %d\n", f,
                       (int)pl->x, (int)pl->z, (int)pl->anchor_x, (int)pl->anchor_z, (int)pl->rot_y, (int)pl->motion, (int)pl->anim_frame, v, (int)z->x,
                       (int)z->z, (int)z->rot_y, z->sub_state_1, z->sub_state_2, (int)z->motion, (int)z->anim_frame);
        }
        lx = pl->x; lz = pl->z;
        if (latch >= 0 && (f > latch + 300 || (v == 0 && f > latch + 5))) break;
    }
    CHECK(latch >= 0, "Griff kommt zustande (Bild %d)", latch);
    if (latch < 0) return s_fail ? 1 : 0;
    printf("  info: %d Griffbilder, max Abstand zum Greifer %d, groesster Bild-zu-Bild-Sprung %d, Anker im Latch %s\n",
           griff, (int)maxd, (int)maxsprung, anker_kopie ? "neu gesetzt" : "UNVERAENDERT (Gift)");
    CHECK(aus == 0, "Spieler in jedem Griffbild in der Raumhuelle: %d ausserhalb", aus);
    CHECK(durch == 0, "kein Griffbild hinter einer Wand: %d", durch);
    return s_fail ? 1 : 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) { for (size_t i = 0; i < sizeof k_faelle / sizeof k_faelle[0]; i++) printf("%s\n", k_faelle[i].name); return 2; }
    for (size_t i = 0; i < sizeof k_faelle / sizeof k_faelle[0]; i++)
        if (!strcmp(argv[1], k_faelle[i].name)) {
            int rc = fall_laufen(&k_faelle[i]);
            printf("test_r38_griff_alle %s: %s\n", argv[1], rc == 77 ? "SKIP" : (rc ? "FAIL" : "OK"));
            return rc;
        }
    printf("unbekannter Fall %s\n", argv[1]);
    return 2;
}
