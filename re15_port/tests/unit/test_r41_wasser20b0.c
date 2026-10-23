/*
 * test_r41_wasser20b0.c — Runde 41 Spur A "wasser20b0": Riegel fuer den Wasserschwall der Flut ROOM20B0/20B1.
 * Dossier analysis/befunde_runde41/A_wasser20b0.md, Belege include/re15_wasser20b0_r41.h.
 *
 *   bytes      Spawn-Tabellen der Port-Datei gegen die ROHEN BYTES von STAGE2.BIN (Mini-Walker ueber die vier
 *              Phasenfunktionen 0x8010f178/0x8010f2c0/0x8010f444/0x8010f6b0: addiu/ori/addu a0..a2 + Delay-Slot,
 *              `jal 0x801108cc` = Versatz, `jal 0x80110988` = Spawn, `ori v0,zero,N`+`bne` = Takt, `sltiu` = Takte)
 *              + die Tabellen @0x80118d50/58/88/90 + die Konstanten der INIT/ACTIVE-Worte.
 *   modell     EM024 (CDEMD0.EMS): Teil-0-Matrix der Pose = Original-RAM (flut_a.sav: T = (x, y-3329, z),
 *              Rotation Einheit) -> Anker fuer (200,1800,-1200) = (x+200, -3329, z-1200).
 *   sequencer  grid 0/1/5 am Sequencer mit der echten Raum-Bank 20B0: Takte, Phasen, Spawn-Codes, a1, Anker.
 *   raum       ROOM20B0 am VM: 35 EM024, Zeichenmaske, Drift x/+0x8c byte-gleich zu den Savestates ts_10
 *              (Bild 131: -17166/251), Umbruch (Bild 161: 20000/225), ts_14 (Bild 233: 1172/297);
 *              Partikel 0x0f im Pool; Elza-Variante 20B1 gleich.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_esp.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_wasser20b0_r41.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int s_fails = 0, s_checks = 0;
#define CHECK(cond, ...) do { s_checks++; if (!(cond)) { s_fails++; printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); } \
                              else { printf("ok  : "); printf(__VA_ARGS__); printf("\n"); } } while (0)

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

/* ============================ bytes: STAGE2.BIN-Walker ============================ */
static uint8_t *s_bin = NULL; static size_t s_bin_n = 0;
static uint32_t ov_word(uint32_t addr)
{
    size_t off = (size_t)(addr - 0x80100000u);                  /* Overlay RAW @0x80100000, KEIN Header */
    if (!s_bin || off + 4 > s_bin_n) return 0xFFFFFFFFu;
    return u32(s_bin, off);
}
typedef struct { int takt; int32_t x, y, z; uint32_t skala; int32_t a1; uint32_t sub; } walk_spawn_t;

/* Mini-Walker: liefert die Spawns einer Phasenfunktion in Aufrufreihenfolge + die Taktzahl (sltiu). */
static int walk_phase(uint32_t start, uint32_t end, walk_spawn_t *out, int max, int *takte_out)
{
    int32_t a[3] = { 0, 0, 0 };      /* a0, a1, a2 */
    int32_t vers[3] = { 0, 0, 0 };
    int takt = 0, n = 0, pending_takt = -1;
    *takte_out = -1;
    for (uint32_t pc = start; pc < end; pc += 4) {
        uint32_t w = ov_word(pc);
        uint32_t op = w >> 26, rs = (w >> 21) & 31, rt = (w >> 16) & 31;
        int16_t imm = (int16_t)(w & 0xffff);
        /* Register-Schreiber: addiu rt,zero,imm (op 9) / ori rt,zero,imm (op 13) / addu rt,zero,zero */
        if (op == 9 && rs == 0 && rt >= 4 && rt <= 6) a[rt - 4] = imm;
        else if (op == 13 && rs == 0 && rt >= 4 && rt <= 6) a[rt - 4] = (int32_t)(w & 0xffff);
        else if (op == 0 && (w & 0x3f) == 0x21 && rs == 0 && rt == 0) { uint32_t rd = (w >> 11) & 31; if (rd >= 4 && rd <= 6) a[rd - 4] = 0; }
        else if (op == 13 && rs == 0 && rt == 2) pending_takt = (int32_t)(w & 0xffff);   /* ori v0,zero,N */
        else if (op == 5 && rs == 3 && rt == 2 && pending_takt >= 0) { takt = pending_takt; pending_takt = -1; } /* bne v1,v0 */
        else if (op == 11 && rs == 2 && rt == 2) *takte_out = (int32_t)(w & 0xffff);     /* sltiu v0,v0,N */
        else if (op == 3) {                                                                /* jal */
            uint32_t ziel = 0x80000000u | ((w & 0x03ffffff) << 2);
            /* Delay-Slot zuerst anwenden */
            uint32_t d = ov_word(pc + 4);
            uint32_t dop = d >> 26, drs = (d >> 21) & 31, drt = (d >> 16) & 31;
            if (dop == 9 && drs == 0 && drt >= 4 && drt <= 6) a[drt - 4] = (int16_t)(d & 0xffff);
            else if (dop == 13 && drs == 0 && drt >= 4 && drt <= 6) a[drt - 4] = (int32_t)(d & 0xffff);
            else if (dop == 0 && (d & 0x3f) == 0x21 && drs == 0 && drt == 0) { uint32_t rd = (d >> 11) & 31; if (rd >= 4 && rd <= 6) a[rd - 4] = 0; }
            if (ziel == 0x801108ccu) { vers[0] = a[0]; vers[1] = a[1]; vers[2] = a[2]; }
            else if (ziel == 0x80110988u && n < max) {
                out[n].takt = takt; out[n].x = vers[0]; out[n].y = vers[1]; out[n].z = vers[2];
                out[n].skala = (uint32_t)a[0] & 0xffff; out[n].a1 = (int16_t)a[1]; out[n].sub = (uint32_t)a[2] & 0xff; n++;
            }
            pc += 4;   /* Delay-Slot verbraucht */
        }
    }
    return n;
}

static void vergleich(const char *name, int grid, int phase, uint32_t start, uint32_t end, int takte_erw)
{
    walk_spawn_t w[16]; int takte = -1;
    int n = walk_phase(start, end, w, 16, &takte);
    int n_tab = 0;
    const re15_wasser20b0_r41_spawn_t *tab = re15_wasser20b0_r41_tabelle(grid, phase, &n_tab);
    CHECK(takte == takte_erw && re15_wasser20b0_r41_phasen_takte(grid, phase) == takte_erw,
          "%s: Takte sltiu=%d Port=%d (erwartet %d)", name, takte, re15_wasser20b0_r41_phasen_takte(grid, phase), takte_erw);
    CHECK(n == n_tab, "%s: %d Spawns in den Bytes, %d in der Port-Tabelle", name, n, n_tab);
    uint32_t sub_erw = (grid == 0) ? RE15_WASSER20B0_R41_SUB_GRID0 : RE15_WASSER20B0_R41_SUB_GRID1;
    for (int i = 0; i < n && i < n_tab; i++) {
        int gleich = w[i].takt == tab[i].takt && w[i].x == tab[i].x && w[i].y == tab[i].y && w[i].z == tab[i].z
                  && w[i].skala == tab[i].skala && w[i].a1 == tab[i].a1 && w[i].sub == sub_erw;
        CHECK(gleich, "%s[%d]: Bytes Takt %d (%d,%d,%d)/0x%04x a1=%d sub=%u | Port Takt %d (%d,%d,%d)/0x%04x a1=%d", name, i,
              w[i].takt, w[i].x, w[i].y, w[i].z, w[i].skala, w[i].a1, w[i].sub,
              tab[i].takt, tab[i].x, tab[i].y, tab[i].z, tab[i].skala, tab[i].a1);
    }
}

static int teil_bytes(void)
{
    char p[512]; snprintf(p, sizeof p, "%s/BIN/STAGE2.BIN", RE15_ASSET_PSX_DIR);
    s_bin = slurp(p, &s_bin_n);
    CHECK(s_bin != NULL && s_bin_n == 103216, "STAGE2.BIN geladen (%zu B)", s_bin_n);
    if (!s_bin) return 0;
    /* Tabellen: Zustand @0x80118d50 [0]=INIT [1]=ACTIVE, Sub @0x80118d58, Phasen @0x80118d88 / @0x80118d90 */
    CHECK(ov_word(0x80118d50) == 0x8010ef1cu && ov_word(0x80118d54) == 0x8010f020u, "Zustandstabelle @0x80118d50: INIT 0x8010ef1c, ACTIVE 0x8010f020");
    CHECK(ov_word(0x80118d58) == 0x8010f130u && ov_word(0x80118d5c) == 0x8010f3fcu && ov_word(0x80118d6c) == 0x8011084cu
          && ov_word(0x80118d70) == 0x8011084cu && ov_word(0x80118d74) == 0x8011084cu,
          "Sub-Tabelle @0x80118d58: [0]=0x8010f130 [1]=0x8010f3fc [5..7]=0x8011084c");
    CHECK(ov_word(0x8011084c) == 0x03e00008u, "0x8011084c = jr ra (grid >= 5 ohne Wirkung)");
    CHECK(ov_word(0x80118d88) == 0x8010f178u && ov_word(0x80118d8c) == 0x8010f2c0u, "grid-0-Phasen @0x80118d88: 0x8010f178, 0x8010f2c0");
    CHECK(ov_word(0x80118d90) == 0x8010f444u && ov_word(0x80118d94) == 0x8010f64cu && ov_word(0x80118d98) == 0x8010f6b0u
          && ov_word(0x80118d9c) == 0x8010f8b8u, "grid-1-Phasen @0x80118d90: 0x8010f444, 0x8010f64c, 0x8010f6b0, 0x8010f8b8");
    /* INIT/ACTIVE-Konstanten */
    CHECK(ov_word(0x8010ef64) == 0x34020078u, "INIT +0x8c = 0x78 (`ori v0,zero,0x78` @0x8010ef64)");
    CHECK(ov_word(0x8010efd4) == 0x2403fffeu && ov_word(0x8010efe4) == 0xac820000u, "INIT Zeichenmaske: `addiu v1,zero,-2` @0x8010efd4 / `sw v0,0(a0)` @0x8010efe4");
    CHECK(ov_word(0x8010f100) == 0x28429e58u && ov_word(0x8010f108) == 0x34024e20u && ov_word(0x8010f118) == 0x340200e1u,
          "ACTIVE Drift: `slti v0,v0,-25000` @0x8010f100, 0x4e20 @0x8010f108, 0xe1 @0x8010f118");
    CHECK(ov_word(0x801109b8) == 0x3c020f00u, "0x80110988: `lui v0,0xf00` @0x801109b8 = Effekt-Id 0x0f");
    /* leere Phasen: nur Taktzaehler */
    walk_spawn_t w[4]; int t1 = -1, t3 = -1;
    int n1 = walk_phase(0x8010f64c, 0x8010f6b0, w, 4, &t1);
    int n3 = walk_phase(0x8010f8b8, 0x8010f910, w, 4, &t3);
    CHECK(n1 == 0 && t1 == 11 && n3 == 0 && t3 == 11, "grid 1 Phase 1/3: keine Spawns, 11 Takte (sltiu 0xb @0x8010f67c/@0x8010f8e8): %d/%d, %d/%d", n1, t1, n3, t3);
    vergleich("grid0 Phase0 0x8010f178", 0, 0, 0x8010f178, 0x8010f2c0, 11);
    vergleich("grid0 Phase1 0x8010f2c0", 0, 1, 0x8010f2c0, 0x8010f3fc, 11);
    vergleich("grid1 Phase0 0x8010f444", 1, 0, 0x8010f444, 0x8010f64c, 26);
    vergleich("grid1 Phase2 0x8010f6b0", 1, 2, 0x8010f6b0, 0x8010f8b8, 26);
    return 1;
}

/* ============================ Baenke / Raum (Muster test_r38_spinnen.c) ============================ */
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

static uint8_t *s_rdt_buf = NULL;
static re15_rdt_t s_rdt;
static re15_esp_t s_esp;
static re15_camera_view_t s_cam;
static re15_game_ctx_t s_ctx;

static int raum(const char *datei, unsigned id, int32_t px, int32_t py, int32_t pz, int16_t prot, uint8_t cut)
{
    size_t n = 0; char p[512];
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, datei);
    free(s_rdt_buf); s_rdt_buf = slurp(p, &n);
    if (!s_rdt_buf) { printf("RDT fehlt: %s\n", p); return 0; }
    memset(&g_room_rdt, 0, sizeof g_room_rdt);
    if (re15_rdt_parse(s_rdt_buf, n, &g_room_rdt) != 0) return 0;
    g_room_rdt_ok = 1; s_rdt = g_room_rdt;
    memset(&s_esp, 0, sizeof s_esp);
    if (re15_esp_parse(s_rdt_buf, n, u32(s_rdt_buf, 0x4C), u32(s_rdt_buf, 0x50), u32(s_rdt_buf, 0x54), u32(s_rdt_buf, 0x58), &s_esp) == 0)
        re15_esp_set_room_bank(&s_esp);
    else re15_esp_set_room_bank(NULL);
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    re15_esp_fx_reset();
    re15_wasser20b0_r41_reset();
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
    (void)bank_re15(0x24);
    return 1;
}

static void bild(void)
{
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
    re15_esp_fx_tick(re15_esp_room_bank());
}

static int pool_0f(int *out_max_sub, uint32_t *out_skalen)
{
    int n = 0; if (out_max_sub) *out_max_sub = -1; if (out_skalen) *out_skalen = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->rows_base && f->effect_id == RE15_WASSER20B0_R41_EFFEKT) {
            n++;
            if (out_max_sub && (int)f->sub_index > *out_max_sub) *out_max_sub = f->sub_index;
            if (out_skalen) *out_skalen |= (f->scale16 == 0x4000) ? 1u : (f->scale16 == 0x3000) ? 2u : (f->scale16 == 0x2000) ? 4u : 8u;
        }
    }
    return n;
}

/* ============================ modell ============================ */
static int teil_modell(void)
{
    re15_actor_init(); re15_enemy_reset();
    CHECK(bank_re15(0x24), "EM024 (Typ 0x24) aus CDEMD0.EMS geladen");
    re15_enemy_bank_t *b = re15_enemy_find(0x24);
    if (!b || !b->ok) return 0;
    printf("     EM024: Meshes %d, Knochen %d, Clips %d, Keyframes %d\n", b->md1.mesh_count, b->skel.bone_count, b->anim.clip_count, b->skel.keyframe_count);
    re15_actor_t e; memset(&e, 0, sizeof e);
    e.active = 1; e.type = 0x24; e.x = -3716; e.y = -1800; e.z = -7350; e.rot_y = 0; e.motion = 0; e.anim_frame = 1;
    int32_t null[3] = { 0, 0, 0 }, w[3];
    int rv = re15_wasser20b0_r41_anker(&e, null, w);
    CHECK(rv == 0 && w[0] == -3716 && w[1] == -1800 + RE15_WASSER20B0_R41_KNOCHEN0_Y && w[2] == -7350,
          "Teil-0-Matrix T = (%d,%d,%d) (Original-RAM flut_a.sav Satz 8: (-3716,-5129,-7350) = Lage + Knochen 0 (0,-3329,0)), rv=%d",
          w[0], w[1], w[2], rv);
    int32_t off[3] = { 200, 1800, -1200 };
    rv = re15_wasser20b0_r41_anker(&e, off, w);
    CHECK(rv == 0 && w[0] == -3516 && w[1] == -3329 && w[2] == -8550,
          "Anker fuer Versatz (200,1800,-1200) = (%d,%d,%d) (erwartet (-3516,-3329,-8550): Einheitsrotation, kein Gier)", w[0], w[1], w[2]);
    /* Rotation der Pose muss Einheit sein (Savestate: 4096,0,0 / 0,4096,0 / 0,0,4096) -> Versatz unverdreht */
    int32_t off2[3] = { -2000, 2700, 1200 };
    rv = re15_wasser20b0_r41_anker(&e, off2, w);
    CHECK(rv == 0 && w[0] == -5716 && w[1] == -2429 && w[2] == -6150, "Anker fuer (-2000,2700,1200) = (%d,%d,%d) (erwartet (-5716,-2429,-6150))", w[0], w[1], w[2]);
    return 1;
}

/* ============================ sequencer ============================ */
static int teil_sequencer(void)
{
    if (!raum("STAGE2/ROOM20B0.RDT", 0x20B0u, 4350, -1800, -20500, 0, 0)) { s_fails++; return 0; }
    re15_esp_fx_reset(); re15_wasser20b0_r41_reset();
    const re15_esp_t *bank = re15_esp_room_bank();
    CHECK(bank && re15_esp_find_id(bank, 0x0f) >= 0, "Raum-Bank 20B0 traegt Effekt 0x0f (ids 05 06 07 0b 0c 0d 0f)");

    /* grid 0: 2 Phasen x 11 Takte, je 6 Spawns bei Takt 0 (sub 7, 0x4000/0x3000 bzw. alle 0x3000) */
    re15_actor_t e; memset(&e, 0, sizeof e);
    e.active = 1; e.type = 0x24; e.x = 7200; e.y = -1800; e.z = -6450; e.grid_id = 0; e.sub_state_1 = 0;
    re15_wasser20b0_r41_init(&e);
    CHECK(e.no_draw == 1 && e.anim_frame == 1 && e.sub_state_3 == 0, "init: no_draw=1 (Zeichenmaske @0x8010efe4), Bild 1, Takt 0");
    int je_takt[48]; int summe = 0; int phasen_ok = 1;
    for (int t = 0; t < 44; t++) {
        int ph_vor = e.sub_state_2;
        je_takt[t] = re15_wasser20b0_r41_sub(&e); summe += je_takt[t];
        int ph_erw = (t / 11) & 1;
        if (ph_vor != ph_erw) phasen_ok = 0;
    }
    CHECK(je_takt[0] == 6 && je_takt[11] == 6 && je_takt[22] == 6 && je_takt[33] == 6 && summe == 24,
          "grid 0: 6 Spawns bei Takt 0/11/22/33 (%d/%d/%d/%d), Summe %d (erwartet 24 = 4 Phasen x 6)", je_takt[0], je_takt[11], je_takt[22], je_takt[33], summe);
    CHECK(phasen_ok && e.sub_state_2 == 0 && e.sub_state_3 == 0, "grid 0: Phase 0,1,0,1 je 11 Takte (@0x8010f284/@0x8010f3cc), danach Phase %d Takt %d", e.sub_state_2, e.sub_state_3);
    const re15_wasser20b0_r41_mess_t *m = re15_wasser20b0_r41_mess();
    CHECK(m->spawns_grid0 == 24 && m->ohne_bank == 0 && m->plaetze == 24, "grid 0: 24 Spawns, 0 ohne Bank, %u Plaetze (je 1 Strom)", m->plaetze);
    CHECK(m->letzter_code == 0x0f073000u && m->letztes_a1 == 0, "grid 0 letzter Code 0x%08x (erwartet 0x0f073000 = Id 0x0f, sub 7, Skala 0x3000), a1 %d", m->letzter_code, m->letztes_a1);
    /* Anker des letzten Spawns (Phase 1 Satz 6: (-1000,1100,0)) relativ zur Lage (7200,-1800,-6450) */
    CHECK(m->letzter_anker[0] == 6200 && m->letzter_anker[1] == -1800 - 3329 + 1100 && m->letzter_anker[2] == -6450,
          "grid 0 letzter Anker (%d,%d,%d) (erwartet (6200,-4029,-6450))", m->letzter_anker[0], m->letzter_anker[1], m->letzter_anker[2]);
    int max_sub = -1; uint32_t sk = 0;
    int n0f = pool_0f(&max_sub, &sk);
    CHECK(n0f == 24 && max_sub == 7 && (sk & 3u) == 3u, "Pool: %d Plaetze 0x0f, sub max %d (7), Skalen 0x4000+0x3000 (Maske %u)", n0f, max_sub, sk);

    /* grid 1: Phasen 26/11/26/11, Spawns Takt 0/3/6/9/12 in Phase 0 und 2 */
    re15_esp_fx_reset(); re15_wasser20b0_r41_reset();
    memset(&e, 0, sizeof e);
    e.active = 1; e.type = 0x24; e.x = 7200; e.y = -1800; e.z = -7350; e.grid_id = 1; e.sub_state_1 = 1;
    re15_wasser20b0_r41_init(&e);
    int folge[80]; summe = 0;
    for (int t = 0; t < 74; t++) { folge[t] = re15_wasser20b0_r41_sub(&e); summe += folge[t]; }
    CHECK(folge[0] == 1 && folge[3] == 2 && folge[6] == 3 && folge[9] == 2 && folge[12] == 1 && folge[1] == 0 && folge[13] == 0 && folge[25] == 0,
          "grid 1 Phase 0: Spawns je Takt 0/3/6/9/12 = %d/%d/%d/%d/%d (erwartet 1/2/3/2/1), dazwischen 0", folge[0], folge[3], folge[6], folge[9], folge[12]);
    int leer = 0; for (int t = 26; t < 37; t++) leer += folge[t];
    CHECK(leer == 0, "grid 1 Phase 1 (Takt 26..36): keine Spawns (%d)", leer);
    CHECK(folge[37] == 1 && folge[40] == 2 && folge[43] == 3 && folge[46] == 2 && folge[49] == 1,
          "grid 1 Phase 2: Spawns bei Takt 37/40/43/46/49 = %d/%d/%d/%d/%d (erwartet 1/2/3/2/1)", folge[37], folge[40], folge[43], folge[46], folge[49]);
    leer = 0; for (int t = 63; t < 74; t++) leer += folge[t];
    CHECK(leer == 0 && summe == 18 && e.sub_state_2 == 0 && e.sub_state_3 == 0, "grid 1 Phase 3 leer, Summe %d (18), danach Phase %d Takt %d", summe, e.sub_state_2, e.sub_state_3);
    m = re15_wasser20b0_r41_mess();
    CHECK(m->spawns_grid1 == 18 && m->letzter_code == 0x0f003800u && m->letztes_a1 == 0, "grid 1: 18 Spawns, letzter Code 0x%08x (0x0f003800: sub 0, Skala 0x3800), a1 %d", m->letzter_code, m->letztes_a1);
    n0f = pool_0f(&max_sub, &sk);
    CHECK(n0f == 18 && max_sub == 0, "Pool: %d Plaetze 0x0f (18), alle sub 0", n0f);
    /* a1 der Zeile: Phase 0 Takt 0 = -200 -> Platz-param -200 (slot+0x2e) */
    int a1_gesehen = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) { const re15_esp_fx_t *f = re15_esp_fx_get(i); if (f && f->rows_base && f->effect_id == 0x0f && f->param == -200) a1_gesehen++; }
    CHECK(a1_gesehen == 1, "ein Platz mit a1=-200 (Phase 0 Takt 0, `addiu a1,zero,-200` @0x8010f474): %d", a1_gesehen);

    /* grid 5: jr ra -> nichts */
    re15_esp_fx_reset(); re15_wasser20b0_r41_reset();
    memset(&e, 0, sizeof e);
    e.active = 1; e.type = 0x24; e.x = 7200; e.y = -1800; e.z = -8150; e.grid_id = 5; e.sub_state_1 = 5;
    re15_wasser20b0_r41_init(&e);
    summe = 0; for (int t = 0; t < 100; t++) summe += re15_wasser20b0_r41_sub(&e);
    CHECK(summe == 0 && pool_0f(NULL, NULL) == 0 && e.sub_state_2 == 0, "grid 5: 0 Spawns in 100 Takten (0x8011084c jr ra)");
    return 1;
}

/* ============================ raum ============================ */
static int em024_zensus(int *grid0, int *grid1, int *nodraw)
{
    int n = 0; *grid0 = 0; *grid1 = 0; *nodraw = 0;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x24) continue;
        n++;
        if (e->grid_id == 0) (*grid0)++;
        if (e->grid_id == 1) (*grid1)++;
        if (e->no_draw) (*nodraw)++;
    }
    return n;
}
static const re15_actor_t *em024_erster(void)
{
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x24) return &g_actors[s];
    return NULL;
}
static int drift_gleich(int32_t x, int16_t timer)
{
    int ok = 1;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (e->active && e->type == 0x24 && (e->x != x || e->ai_timer != timer)) ok = 0;
    }
    return ok;
}

static int teil_raum_einer(const char *datei, unsigned id, const char *name)
{
    printf("\n== raum %s (%s, Debug-Sprung-Lage (4350,-1800,-20500), Flag (3,0xEB)=0)\n", name, datei);
    if (!raum(datei, id, 4350, -1800, -20500, 0, 0)) { s_fails++; return 0; }
    int g0, g1, nd;
    int n = em024_zensus(&g0, &g1, &nd);
    CHECK(n == 35 && g0 == 1 && g1 == 2, "%s: 35 EM024 gespawnt (grid 0: %d, grid 1: %d) — sub00 @0x1AAE..0x1D56", name, g0, g1);
    const re15_actor_t *e0 = em024_erster();
    CHECK(e0 && e0->x == 7200 && e0->y == -1800 && e0->z == -8150, "%s: Satz 0 bei (7200,-1800,-8150)", name);
    bild();                                   /* INIT-Takt */
    n = em024_zensus(&g0, &g1, &nd);
    CHECK(nd == 35, "%s: nach dem INIT-Takt alle 35 ohne Zeichnung (no_draw, @0x8010efe4): %d", name, nd);
    int max_pool = 0, bilder_mit_partikeln = 0, bild_ts10 = 0, bild_wrap = 0, bild_ts14 = 0;
    for (int f = 1; f <= 233; f++) {
        bild();
        int np = pool_0f(NULL, NULL);
        if (np > max_pool) max_pool = np;
        if (np > 0) bilder_mit_partikeln++;
        if (f == 131) bild_ts10 = drift_gleich(-17166, 251);
        if (f == 161) bild_wrap = drift_gleich(20000, 225);
        if (f == 233) bild_ts14 = drift_gleich(1172, 297);
    }
    CHECK(bild_ts10, "%s: Bild 131: alle 35 bei x=-17166 / +0x8c=251 (Original ts_10.sav)", name);
    CHECK(bild_wrap, "%s: Bild 161: Umbruch x=20000 / +0x8c=225 (@0x8010f108/@0x8010f118)", name);
    CHECK(bild_ts14, "%s: Bild 233: x=1172 / +0x8c=297 (Original ts_14.sav)", name);
    const re15_wasser20b0_r41_mess_t *m = re15_wasser20b0_r41_mess();
    /* 234 Takte: grid 0 -> Takte 0,11,...,231 = 22 Phasen x 6 = 132; grid 1 (2 Entities) -> 3 volle Umlaeufe (222 Takte) x 18 + Takte 222..233 = Phase 0 Takt 0..11 -> 1+2+3+2 = 8 -> (54+8) x 2 = 124 */
    CHECK(m->spawns_grid0 == 132 && m->spawns_grid1 == 124 && m->ohne_bank == 0,
          "%s: Spawns grid 0 = %u (erwartet 132), grid 1 = %u (erwartet 124), ohne Bank %u", name, m->spawns_grid0, m->spawns_grid1, m->ohne_bank);
    CHECK(max_pool >= 3 && bilder_mit_partikeln >= 200, "%s: Partikel 0x0f im Pool (max %d gleichzeitig, %d von 233 Bildern belegt; Original ts_10..flut_a: 3..14)", name, max_pool, bilder_mit_partikeln);
    CHECK(m->letzter_anker[1] < -1800, "%s: letzter Anker y=%d ueber dem Boden (-1800)", name, m->letzter_anker[1]);
    return 1;
}

static int teil_raum(void)
{
    int a = teil_raum_einer("STAGE2/ROOM20B0.RDT", 0x20B0u, "Leon 20B0");
    int b = teil_raum_einer("STAGE2/ROOM20B1.RDT", 0x20B1u, "Elza 20B1");
    return a && b;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "bytes"))     teil_bytes();
    if (alle || !strcmp(teil, "modell"))    teil_modell();
    if (alle || !strcmp(teil, "sequencer")) teil_sequencer();
    if (alle || !strcmp(teil, "raum"))      teil_raum();
    printf("\n%s: %d Pruefungen, %d Fehler\n", teil, s_checks, s_fails);
    return s_fails ? 1 : 0;
}
