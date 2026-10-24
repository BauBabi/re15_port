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
#include "re15_wassersprung_r41.h"
#include "re15_wasser_r38.h"
#include "re15_roy_r37.h"      /* re15_ems_toc / re15_ems_archiv_datei: beide Archive */
#include "re15_enemy_ai.h"
#include "re15_vab.h"          /* Nachbesserung 2: re15_vab_parse / re15_footstep_vag = Ton-Aufloesung wie audio_pc.c */
#include "re15_math.h"         /* Nachbesserung 2: re15_squareroot0 = SquareRoot0 (Abstand der Lage) */

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_R41A_PSX_EXE
#define RE15_R41A_PSX_EXE "../info/Re1.5/PSX.EXE"
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
/* Je Typ ein EIGENER Blob (die Bank zeigt in den Puffer; ein geteilter Puffer wie in test_r38_spinnen.c
 * macht die zuerst geladene Bank beim zweiten Laden zu Datenmuell). */
static int bank_re15(uint8_t type)
{
    if (!s_re15_ems) s_re15_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_re15_n);
    if (!s_re15_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_re15_ems, s_re15_n, idx, &off, &len) != 0) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    uint8_t *blob = (uint8_t *)malloc(len);
    if (!blob) return 0;
    memcpy(blob, s_re15_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; free(blob); return 0; }
    eb->ok = 1; eb->buf = NULL;   /* blob bleibt absichtlich stehen (Lebensdauer = Prozess) */
    return 1;
}

/* Nachbesserung 1: Bank-Lader fuer NPC-Typen aus BEIDEN Archiven (TOC @0x80072f38 Leon / @0x80073178 Elza, roy_r37.c) —
 * Container + OWN-Bank (+0x170/+0x174 = dir[3]/dir[4]) + Loco-Bank in die Registry, wie platform/pc/main.c. */
static uint8_t *s_ems_arch[2] = { NULL, NULL }; static size_t s_ems_arch_n[2] = { 0, 0 };
static int bank_npc(int archiv, uint8_t type)
{
    if (!s_ems_arch[archiv]) {
        char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(archiv));
        s_ems_arch[archiv] = slurp(p, &s_ems_arch_n[archiv]);
    }
    if (!s_ems_arch[archiv]) return 0;
    uint32_t sek = 0, laen = 0;
    if (!re15_ems_toc(archiv, type, &sek, &laen)) return 0;
    size_t off = (size_t)sek * 2048u;
    if (off + laen > s_ems_arch_n[archiv]) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    uint8_t *blob = (uint8_t *)malloc(laen);
    if (!blob) return 0;
    memcpy(blob, s_ems_arch[archiv] + off, laen);
    re15_tim_t tim = (re15_tim_t){0};
    memset(&eb->md1, 0, sizeof eb->md1); memset(&eb->skel, 0, sizeof eb->skel); memset(&eb->anim, 0, sizeof eb->anim);
    if (re15_emd_parse_container(blob, laen, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; free(blob); return 0; }
    eb->ok = 1; eb->buf = NULL;   /* blob bleibt stehen (Lebensdauer = Prozess) */
    eb->own_ok  = (re15_emd_parse_own_bank(blob, laen, &eb->skel_own, &eb->anim_own) == 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(blob, laen, &eb->skel_loco, &eb->anim_loco) == 0);
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

/* Nachbesserung 2 (M1/M4): SPIELT der Schritt-Ton? Dieselbe Aufloesung wie audio_pc.c re15_audio_footstep: EDT-Satz idx der
 * snd0-Bank des Raums (RDT +0x08) -> Programm/Ton -> VAG-Index der VH (RDT +0x0C) ueber re15_footstep_vag (vab_common.c).
 * >= 0 = ein Mischer-Ton (die Abnahme mass an der exe "SE unterdrueckt: type=21 -> vag=-1"). */
static re15_vab_t s_vab_snd0;
static int ton_vag(int idx)
{
    if (idx < 0 || !g_room_rdt.snd_vh[0] || !g_room_rdt.snd_edt[0]) return -1;
    if (re15_vab_parse(g_room_rdt.snd_vh[0], (size_t)g_room_rdt.snd_vh_size[0], &s_vab_snd0) != 0) return -2;
    return re15_footstep_vag(g_room_rdt.snd_edt[0], &s_vab_snd0, idx);
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

/* ============================ Punkt 2: landung / npc (wassersprung_r41.c) ============================ */
static int pool_code(uint8_t id, uint8_t sub, uint16_t skala, int16_t *params, int max)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->rows_base && f->effect_id == id && f->sub_index == sub && f->scale16 == skala) {
            if (params && n < max) params[n] = f->param;
            n++;
        }
    }
    return n;
}

static int teil_landung(void)
{
    printf("\n== landung (ROOM2090, Becken y=0, Stempel -1620 = Zone slot 2/3 Band 0)\n");
    if (!raum("STAGE2/ROOM2090.RDT", 0x2090u, 4500, 0, -15200, 3072, 0)) { s_fails++; return 0; }
    const re15_esp_t *bank = re15_esp_room_bank();
    CHECK(bank && re15_esp_find_id(bank, 0x06) >= 0, "Raum-Bank 2090 traegt Effekt 6 (Wasser-Spritzer)");
    re15_esp_fx_reset(); re15_wassersprung_r41_reset();
    re15_actor_t p; memset(&p, 0, sizeof p);
    p.active = 1; p.type = 0; p.x = 4500; p.z = -16000; p.y = 0; p.rot_y = 3072;
    /* nass: Stempel -1620 < y 0 (PSX y nach unten: Wasserspiegel ueber dem Beckenboden) */
    int n = re15_wassersprung_r41_landung(&p, -1620, -400);
    int16_t par[8]; int np = pool_code(0x06, 0x03, 0x3000, par, 8);
    CHECK(n == 8 && np == 8, "nass: %d Spawn-Aufrufe, %d Plaetze Effekt 6 Sub 3 Skala 0x3000 (erwartet 8/8, `ori s1,zero,0x4` x 2)", n, np);
    const re15_wassersprung_r41_mess_t *m = re15_wassersprung_r41_mess();
    static const int16_t gier_erw[8] = { 3072+512, 3072+512+2048, 3072+256, 3072+256+2048, 3072, 3072+2048, 3072-256, 3072-256+2048 };
    int gier_ok = 1;
    for (int i = 0; i < 8; i++) if (m->letzte_gier[i] != gier_erw[i]) gier_ok = 0;
    CHECK(gier_ok, "Gier-Faecher rot_y + (s1-2)<<8 (+2048): %d %d %d %d %d %d %d %d (@0x80038788-98 / @0x800387c0)",
          m->letzte_gier[0], m->letzte_gier[1], m->letzte_gier[2], m->letzte_gier[3], m->letzte_gier[4], m->letzte_gier[5], m->letzte_gier[6], m->letzte_gier[7]);
    int par_ok = 1;
    for (int i = 0; i < np; i++) { int hit = 0; for (int k = 0; k < 8; k++) if (par[i] == gier_erw[k]) hit = 1; if (!hit) par_ok = 0; }
    CHECK(par_ok, "jeder Platz traegt eine der 8 Gieren als Parameter (slot+0x2e)");
    CHECK(m->letzter_anker[0] == 4500 && m->letzter_anker[1] == -400 && m->letzter_anker[2] == -16000,
          "Anker = Entity-Matrix +0x20 (zuletzt gezeichnete Lage, y vor dem Bodenschreiben): (%d,%d,%d) (erwartet (4500,-400,-16000))",
          m->letzter_anker[0], m->letzter_anker[1], m->letzter_anker[2]);
    /* trocken: Stempel 0 bei y -1800 (0 < -1800 falsch) -> nichts; Stempel -540 auf dem Steg y -1800 (20A0) -> nichts */
    re15_esp_fx_reset();
    p.y = -1800;
    n = re15_wassersprung_r41_landung(&p, 0, -2200);
    int n2 = re15_wassersprung_r41_landung(&p, -540, -2200);
    CHECK(n == 0 && n2 == 0 && pool_code(0x06, 0x03, 0x3000, NULL, 0) == 0 && m->landung_trocken == 2,
          "trocken: Stempel 0 / -540 bei y -1800 -> keine Plaetze (`slt v1,v1,a0` @0x8003874c, beq @0x80038758), trocken=%u", m->landung_trocken);
    return 1;
}

/* Roy (0x47 CDEMD1) Clip 5 (61 Bilder) ohne Marke: Aufsetzer nach der PORT-WAHL-Regel = Bild 19 (Teil 4 erreicht das unterste
 * Zehntel nach dem Schwung, Tiefstlage y -213 bei Bild 21) und Bild 50 (Teil 7, Tiefstlage -214 bei Bild 52); Pose-Reihe Sonde
 * roy_c5.txt, Dossier N1.10d. Ohne Hysterese lieferte die Regel 4 Aufsetzer (Zittern des stehenden Teils 4 um die Schwelle). */
#define K_ROY_C5_A 19
#define K_ROY_C5_B 50
static int teil_npc(void)
{
    printf("\n== npc (Nachbesserung 1: Aufsetzer der OWN-Bank +0x170/+0x174, beide Archive, PORT-WAHL nur fuer Fortbewegungs-Clips ohne Marke)\n");
    /* Erwartung je Typ (Sonde npc_own.txt, Dossier Nachbesserung 1): Clip 5 = Gehen (30 Bilder), Clip 0 = Laufen (22) */
    static const struct { uint8_t typ; int c5[2]; int f5[2]; int c0[2]; int f0[2]; int portwahl; } k_erw[6] = {
        { 0x40, {7, 22}, {7, 4}, {0, 12}, {4, 4}, 0 },
        { 0x42, {8, 22}, {7, 4}, {0, 11}, {7, 4}, 0 },
        { 0x45, {7, 22}, {7, 4}, {0, 12}, {4, 4}, 0 },
        { 0x49, {7, 22}, {7, 4}, {0, 12}, {4, 4}, 0 },
        { 0x4b, {5, 19}, {7, 4}, {0, 10}, {7, 4}, 1 },   /* PORT-WAHL-Regel: keine Marke in keiner Bank */
        { 0x4d, {8, 22}, {7, 4}, {0, 11}, {7, 4}, 0 },
    };
    for (int arch = 0; arch < 2; arch++) {
        for (int ti = 0; ti < 6; ti++) {
            uint8_t typ = k_erw[ti].typ;
            re15_enemy_reset();   /* Registry (RE15_ENEMY_MAX) je Typ leeren */
            if (!bank_npc(arch, typ)) { CHECK(0, "%s Typ 0x%02x: Bank nicht ladbar", re15_ems_archiv_datei(arch), typ); continue; }
            re15_enemy_bank_t *b = re15_enemy_find(typ);
            CHECK(b && b->ok && b->own_ok && b->anim_own.clip_count >= 6 && b->anim_own.clips[5].frame_count == (typ == 0x4b ? 28 : 30),
                  "%s Typ 0x%02x: OWN-Bank geladen (Clips %d, Clip 5 = %d Bilder)", re15_ems_archiv_datei(arch), typ,
                  b ? b->anim_own.clip_count : -1, (b && b->own_ok) ? b->anim_own.clips[5].frame_count : -1);
            if (!b || !b->own_ok) continue;
            int bi[4] = {0,0,0,0}, fu[4] = {0,0,0,0}, pw = 0;
            int n5 = re15_wassersprung_r41_aufsetzer(&b->anim_own, &b->skel_own, typ, 5, bi, fu, &pw);
            CHECK(n5 == 2 && bi[0] == k_erw[ti].c5[0] && bi[1] == k_erw[ti].c5[1] && fu[0] == k_erw[ti].f5[0] && fu[1] == k_erw[ti].f5[1] && pw == k_erw[ti].portwahl,
                  "%s Typ 0x%02x Clip 5: Aufsetzer Bilder %d/%d Fuesse %d/%d portwahl=%d (erwartet %d/%d, %d/%d, %d)", re15_ems_archiv_datei(arch), typ,
                  bi[0], bi[1], fu[0], fu[1], pw, k_erw[ti].c5[0], k_erw[ti].c5[1], k_erw[ti].f5[0], k_erw[ti].f5[1], k_erw[ti].portwahl);
            int n0 = re15_wassersprung_r41_aufsetzer(&b->anim_own, &b->skel_own, typ, 0, bi, fu, &pw);
            CHECK(n0 == 2 && bi[0] == k_erw[ti].c0[0] && bi[1] == k_erw[ti].c0[1] && fu[0] == k_erw[ti].f0[0] && fu[1] == k_erw[ti].f0[1] && pw == k_erw[ti].portwahl,
                  "%s Typ 0x%02x Clip 0: Aufsetzer Bilder %d/%d Fuesse %d/%d portwahl=%d (erwartet %d/%d, %d/%d, %d)", re15_ems_archiv_datei(arch), typ,
                  bi[0], bi[1], fu[0], fu[1], pw, k_erw[ti].c0[0], k_erw[ti].c0[1], k_erw[ti].f0[0], k_erw[ti].f0[1], k_erw[ti].portwahl);
            /* Regelpruefung: die PORT-WAHL-Regel trifft die Original-Marken der markierten Typen auf <= 4 Bilder */
            if (!k_erw[ti].portwahl && arch == 0) {
                int rb[4] = {0,0,0,0}, rf[4] = {0,0,0,0};
                int nr = re15_wassersprung_r41_regel(&b->anim_own, &b->skel_own, typ, 5, rb, rf);
                int d0 = rb[0] - k_erw[ti].c5[0], d1 = rb[1] - k_erw[ti].c5[1];
                CHECK(nr == 2 && d0 >= -4 && d0 <= 4 && d1 >= -4 && d1 <= 4,
                      "Regel an Typ 0x%02x Clip 5: %d/%d gegen Marken %d/%d (|Abstand| <= 4)", typ, rb[0], rb[1], k_erw[ti].c5[0], k_erw[ti].c5[1]);
            }
            /* Container-Bank Clip 5 (20-Bild-Kniebeuge, 0x4b: 38) traegt KEINE Marke - der Bau-Fehler (falsche Bank). Seit der
             * dritten Fortsetzung: kein Fortbewegungs-Clip (nur OWN c0/c5, LOCO/Container c0/c1 tragen Marken, Sonde
             * npc_marken.txt) -> auch keine PORT-WAHL, also kein Schritt - wie im Original (`andi 0x4000` @0x80051288). */
            int kb[4] = {0,0,0,0}, kf[4] = {0,0,0,0}, kpw = 0;
            int kn = re15_wassersprung_r41_aufsetzer(&b->anim, &b->skel, typ, 5, kb, kf, &kpw);
            CHECK(kn == 0 && kpw == 0, "%s Typ 0x%02x: Container-Clip 5 (%d Bilder) ohne Marke und kein Fortbewegungs-Clip -> %d Aufsetzer, portwahl=%d (erwartet 0/0)",
                  re15_ems_archiv_datei(arch), typ, b->anim.clips[5].frame_count, kn, kpw);
            /* INIT-Pose: LOCO Clip 2 (f314(+0x84,+0x16c) @0x8011d978, Bild 0) traegt keine Marke -> kein Schritt (vorher leitete
             * der Port hier eine PORT-WAHL ab - integration_r37_roy_leon1170 rot an den Fremdzeilen der Ableitung). */
            if (b->loco_ok && b->anim_loco.clip_count > 2) {
                int lb[4] = {0,0,0,0}, lf[4] = {0,0,0,0}, lpw = 0;
                int ln = re15_wassersprung_r41_aufsetzer(&b->anim_loco, &b->skel_loco, typ, 2, lb, lf, &lpw);
                CHECK(ln == 0 && lpw == 0, "%s Typ 0x%02x: LOCO-Clip 2 (INIT-Pose, %d Bilder) -> %d Aufsetzer, portwahl=%d (erwartet 0/0)",
                      re15_ems_archiv_datei(arch), typ, b->anim_loco.clips[2].frame_count, ln, lpw);
            }
            /* Rueckwaerts (Sub 7/8 posieren das LOCO-Paar +0x84/+0x16c @0x80051a20-24 / @0x80051c18-1c, Clip 1; der Port zeichnet
             * dort den Container, Clip 1 mit denselben Marken): Marken 7/22 an den markierten Typen, PORT-WAHL nur 0x4b. */
            {
                int rb[4] = {0,0,0,0}, rf[4] = {0,0,0,0}, rpw = 0;
                int rn = re15_wassersprung_r41_aufsetzer(&b->anim, &b->skel, typ, 1, rb, rf, &rpw);
                CHECK(rpw == k_erw[ti].portwahl && (rpw ? rn >= 1 : (rn == 2 && rb[0] == 7 && rb[1] == 22)),
                      "%s Typ 0x%02x: Container-Clip 1 (Rueckwaerts, %d Bilder) -> Aufsetzer %d/%d (n=%d) portwahl=%d (erwartet Marken 7/22 bzw. PORT-WAHL fuer 0x4b)",
                      re15_ems_archiv_datei(arch), typ, b->anim.clips[1].frame_count, rb[0], rb[1], rn, rpw);
            }
        }
    }
    /* Typ 0x47 (Leon: Elliot, CDEMD0; Elza: Roy, CDEMD1 - re15_typ47_ist_elliot): CDEMD0 OWN c5 Marken 7/22 wie die
     * 0x40-Familie; CDEMD1 OWN c5 OHNE Marke (c0 markiert 0/12, Sonde npc_marken.txt) -> der einzige markierte Typ, dessen
     * Geh-Clip die PORT-WAHL braucht (NUTZER-VORGABE "alle NPCs"). */
    for (int arch = 0; arch < 2; arch++) {
        re15_enemy_reset();
        if (!bank_npc(arch, 0x47)) { CHECK(0, "%s Typ 0x47: Bank nicht ladbar", re15_ems_archiv_datei(arch)); continue; }
        re15_enemy_bank_t *b = re15_enemy_find(0x47);
        if (!b || !b->own_ok) { CHECK(0, "%s Typ 0x47: OWN-Bank fehlt", re15_ems_archiv_datei(arch)); continue; }
        int bi[4] = {0,0,0,0}, fu[4] = {0,0,0,0}, pw = 0;
        int n5 = re15_wassersprung_r41_aufsetzer(&b->anim_own, &b->skel_own, 0x47, 5, bi, fu, &pw);
        printf("     %s Typ 0x47 OWN Clip 5 (%d Bilder): Aufsetzer %d -> Bilder %d/%d Fuesse %d/%d portwahl=%d\n", re15_ems_archiv_datei(arch),
               b->anim_own.clips[5].frame_count, n5, bi[0], bi[1], fu[0], fu[1], pw);
        if (arch == 0)
            CHECK(n5 == 2 && pw == 0 && bi[0] == 7 && bi[1] == 22 && fu[0] == 7 && fu[1] == 4,
                  "CDEMD0 Typ 0x47 (Elliot) Clip 5: Marken 7/22 Fuesse 7/4 (%d/%d %d/%d portwahl=%d)", bi[0], bi[1], fu[0], fu[1], pw);
        else
            CHECK(n5 == 2 && pw == 1 && bi[0] == K_ROY_C5_A && bi[1] == K_ROY_C5_B && fu[0] == 7 && fu[1] == 4,
                  "CDEMD1 Typ 0x47 (Roy) Clip 5 ohne Marke -> PORT-WAHL %d/%d Fuesse %d/%d (erwartet %d/%d, 7/4, portwahl=%d)", bi[0], bi[1], fu[0], fu[1],
                  K_ROY_C5_A, K_ROY_C5_B, pw);
        int n0 = re15_wassersprung_r41_aufsetzer(&b->anim_own, &b->skel_own, 0x47, 0, bi, fu, &pw);
        CHECK(n0 == 2 && pw == 0 && bi[0] == 0 && bi[1] == 12,
              "%s Typ 0x47 Clip 0: Marken 0/12 (%d/%d, portwahl=%d)", re15_ems_archiv_datei(arch), bi[0], bi[1], pw);
        if (b->loco_ok && b->anim_loco.clip_count > 2) {
            int ln = re15_wassersprung_r41_aufsetzer(&b->anim_loco, &b->skel_loco, 0x47, 2, bi, fu, &pw);
            CHECK(ln == 0 && pw == 0, "%s Typ 0x47 LOCO-Clip 2 (INIT-Pose @0x8011d978): %d Aufsetzer, portwahl=%d (erwartet 0/0)",
                  re15_ems_archiv_datei(arch), ln, pw);
        }
    }
    /* Zweige von FUN_80045630 am Riegel-Aktor (wie bisher): Fleck 0x97, Stempel, trocken */
    printf("\n== npc Zweige (ROOM2000: Wasserflecken FLR 0x97 x -5220..-3420 / z -9750..-1650, Trockenfleck 0x17 x -2400..-700)\n");
    if (!raum("STAGE2/ROOM2000.RDT", 0x2000u, 4350, -1800, -20500, 0, 0)) { s_fails++; return 0; }
    (void)bank_npc(0, 0x40);
    const re15_esp_t *bank = re15_esp_room_bank();
    CHECK(bank && re15_esp_find_id(bank, 0x06) >= 0, "Raum-Bank 2000 traegt Effekt 6");
    re15_actor_t *e = &g_actors[3];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x40; e->x = -4300; e->y = -1800; e->z = -5000; e->rot_y = 1500; e->floor = 1; e->motion = 5;
    re15_esp_fx_reset(); re15_wassersprung_r41_reset();
    int boden = re15_rdt_floor_sound(&g_room_rdt, e->x, e->z);
    CHECK(boden == 0x97, "Bodentyp an (-4300,-5000) = %#x (erwartet 0x97, FLR-Satz 3)", boden);
    int idx = re15_wassersprung_r41_npc_schritt(e, 0, 7, &g_room_rdt);
    const re15_wassersprung_r41_mess_t *m = re15_wassersprung_r41_mess();
    int n5 = pool_code(0x06, 0x05, 0x0D00, NULL, 0);
    /* Nachbesserung 2 (M1): Original-Zeile 0x17-8 = 15 (`addiu s3,a0,-8` @0x8004575c) ist in ROOM2000 leer -> stumm im Original;
     * RE2-Retail FUN_8005c040 nimmt a0 + (Boden & 0x7f) = 23 (@0x8005c44c/@0x8005c458), Satz 00007312 -> VAG >= 0. */
    CHECK(m->letzter_ton_orig == (0x97 & 0x7f) - 8 && idx == (0x97 & 0x7f) && m->letzte_quelle == 1 && ton_vag(idx) >= 0
          && m->npc_boden == 1 && n5 >= 1,
          "Wasserboden: Original-Zeile %d (0x17-8 = 15, leer) -> gespielt %d (Retail 0x17 = 23, VAG %d), Zweig boden %u, Plaetze 0x06050D00: %d",
          m->letzter_ton_orig, idx, ton_vag(idx), m->npc_boden, n5);
    re15_esp_fx_reset(); re15_wassersprung_r41_reset();
    e->x = -1500; e->water_y = -1620;
    boden = re15_rdt_floor_sound(&g_room_rdt, e->x, e->z);
    idx = re15_wassersprung_r41_npc_schritt(e, 1, 4, &g_room_rdt);
    int n3 = pool_code(0x06, 0x03, 0x2000, NULL, 0);
    /* Satz 13 = 0000a111: Stimme (0x11 & 0x1f) - 16 + (Fuss 4 != 4) = 1 -> im ORIGINAL hoerbar -> bleibt (Quelle 0) */
    CHECK(boden == 0x17 && idx == 1 + (0x17 - 3) - 8 && m->letzte_quelle == 0 && ton_vag(idx) >= 0 && m->npc_stempel == 1 && n3 >= 1,
          "Stempel: Boden %#x, Ton %d (Original a0 1 + 0x17-3 - 8 = 13, Satz 0000a111 Stimme 1 = hoerbar, Quelle %u, VAG %d), Zweig stempel %u, Plaetze 0x06032000: %d",
          boden, idx, (unsigned)m->letzte_quelle, ton_vag(idx), m->npc_stempel, n3);
    re15_esp_fx_reset(); re15_wassersprung_r41_reset();
    e->water_y = 0;
    idx = re15_wassersprung_r41_npc_schritt(e, 0, 7, &g_room_rdt);
    CHECK(m->letzter_ton_orig == 0x17 - 8 && idx == 0x17 && m->letzte_quelle == 1 && ton_vag(idx) >= 0 && re15_esp_fx_count() == 0,
          "trocken: Original-Zeile %d (0x17-8 leer) -> Retail %d (VAG %d), keine Plaetze (%d)", m->letzter_ton_orig, idx, ton_vag(idx), re15_esp_fx_count());
    /* Takt: Bildwort der OWN-Bank (Geh-Sub 4 -> re15_actor_uses_own_bank): Bild 7 = Marke, Bild 8 = keine */
    re15_esp_fx_reset(); re15_wassersprung_r41_reset();
    e->x = -4300; e->water_y = 0; e->state = 4; e->sub_state_1 = 4; e->motion = 5; e->anim_frame = 7;
    const void *sk = NULL; const void *an = re15_wassersprung_r41_npc_bank(e, &sk);
    re15_enemy_bank_t *b40 = re15_enemy_find(0x40);
    CHECK(an && b40 && an == (const void *)&b40->anim_own, "Takt: Bank des Geh-Subs = OWN-Bank (+0x170/+0x174)");
    int t = re15_wassersprung_r41_npc_takt(e, 0);
    e->anim_frame = 8;
    int t2 = re15_wassersprung_r41_npc_takt(e, 0);
    CHECK(t == 1 && t2 == 0 && m->npc_schritte == 1 && m->npc_boden == 1 && m->letzter_fuss == 7,
          "Takt Typ 0x40 Clip 5: Bild 7 (Wort 0x6xxx, Fuss 7) -> 1 Schritt im Wasser, Bild 8 -> keiner (t=%d t2=%d schritte=%u fuss=%u)",
          t, t2, m->npc_schritte, m->letzter_fuss);
    return 1;
}

/* ============================ npc_wasser: der ECHTE Geh-Sub (state 4 / sub 4) durch den Wasserfleck ============================ */
static int npc_wasser_einer(int archiv, uint8_t typ, int erw_portwahl)
{
    if (!raum("STAGE2/ROOM2000.RDT", 0x2000u, 4350, -1800, -20500, 0, 0)) { s_fails++; return 0; }
    if (!bank_npc(archiv, typ)) { CHECK(0, "%s Typ 0x%02x: Bank nicht ladbar", re15_ems_archiv_datei(archiv), typ); return 0; }
    re15_actor_t *e = &g_actors[3];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = typ; e->hp = 100; e->x = -4300; e->y = -1800; e->z = -3000; e->rot_y = 2048; e->floor = 1;
    /* Plc_dest(mode 4) @0x80041c14-24: State 4, Sub 4, Ziel +0x1bc/+0x1be, Phase 0 (scd_vm.c op_plc_dest) */
    e->state = 4; e->sub_state_1 = 4; e->sub_state_2 = 0; e->steer_x = -4300; e->steer_z = -9000;
    e->walk_dest_x = -4300; e->walk_dest_z = -9000; e->walk_flag_bit = 0; e->anim_flags = 0; e->motion = 5;
    re15_esp_fx_reset(); re15_wassersprung_r41_reset();
    int32_t z0 = e->z; int max_pool = 0;
    for (int f = 0; f < 150; f++) {
        bild();
        int n = pool_code(0x06, 0x05, 0x0D00, NULL, 0);
        if (n > max_pool) max_pool = n;
        if (e->state != 4 || e->sub_state_1 != 4) break;
    }
    const re15_wassersprung_r41_mess_t *m = re15_wassersprung_r41_mess();
    int boden = re15_rdt_floor_sound(&g_room_rdt, e->x, e->z);
    printf("     %s Typ 0x%02x: z %d -> %d, Boden %#x, Schritte %u (boden %u, stempel %u, portwahl %u), Plaetze %u, max gleichzeitig %d, letzter Ton %d, letztes Bild %u Clip %u\n",
           re15_ems_archiv_datei(archiv), typ, z0, e->z, boden, m->npc_schritte, m->npc_boden, m->npc_stempel, m->portwahl_schritte, m->plaetze, max_pool, m->letzter_ton,
           m->letztes_bild, m->letzter_clip);
    CHECK(e->z < z0 - 2000, "%s Typ 0x%02x: der Geh-Sub bewegt den NPC durch den Fleck (z %d -> %d)", re15_ems_archiv_datei(archiv), typ, z0, e->z);
    /* Roy (0x47 aus CDEMD1): OWN-Clip 5 ohne Marke -> PORT-WAHL 2 Aufsetzer je Umlauf des laengeren Clips (N1.10d) -> >= 2 */
    const unsigned min_s = (typ == 0x47 && archiv == 1) ? 2u : 4u;
    CHECK(m->npc_schritte >= min_s && m->npc_boden >= min_s, "%s Typ 0x%02x: >= %u Schritte im Wasser (FLR 0x97 -> Zweig boden): %u/%u", re15_ems_archiv_datei(archiv), typ, min_s, m->npc_schritte, m->npc_boden);
    CHECK(m->plaetze >= min_s && max_pool >= 1, "%s Typ 0x%02x: Wasser-Spritzer 0x06050D00 gespawnt (%u Plaetze, max %d gleichzeitig)", re15_ems_archiv_datei(archiv), typ, m->plaetze, max_pool);
    /* Nachbesserung 2 (M1/M4): nicht nur der Index - der Ton muss SPIELEN (VAG >= 0) und KEIN Schritt darf stumm sein */
    CHECK(m->letzter_ton_orig == (0x97 & 0x7f) - 8 && m->letzter_ton == (0x97 & 0x7f) && m->npc_ton_stumm == 0
          && m->npc_ton_retail == m->npc_schritte && ton_vag(m->letzter_ton) >= 0,
          "%s Typ 0x%02x: Schritt-Ton gespielt: Original 0x17-8 = 15 (a0 = 0 @0x80051290, leer) -> Retail %d (VAG %d), %u/%u Schritte Retail, %u stumm",
          re15_ems_archiv_datei(archiv), typ, m->letzter_ton, ton_vag(m->letzter_ton), m->npc_ton_retail, m->npc_schritte, m->npc_ton_stumm);
    CHECK((m->portwahl_schritte > 0) == (erw_portwahl != 0), "%s Typ 0x%02x: Schritte aus der PORT-WAHL-Regel %u (erwartet %s)", re15_ems_archiv_datei(archiv), typ, m->portwahl_schritte, erw_portwahl ? ">0" : "0");
    return 1;
}
static int teil_npc_wasser(void)
{
    printf("\n== npc_wasser (ROOM2000, Geh-Sub 4 von (-4300,-3000) nach (-4300,-9000) durch den Fleck FLR 0x97)\n");
    npc_wasser_einer(0, 0x40, 0);   /* Ada    (Leon-Archiv)  */
    npc_wasser_einer(0, 0x42, 0);   /* Marvin (Leon-Archiv)  */
    npc_wasser_einer(0, 0x45, 0);
    npc_wasser_einer(1, 0x49, 0);   /* Elza-Archiv           */
    npc_wasser_einer(1, 0x4b, 1);   /* Kind: PORT-WAHL-Regel */
    npc_wasser_einer(1, 0x4d, 0);
    npc_wasser_einer(0, 0x47, 0);   /* Elliot (Leon-Archiv): OWN c5 Marken 7/22 - Nachbesserung 2, M4 */
    npc_wasser_einer(1, 0x47, 1);   /* Roy (Elza-Archiv): OWN c5 ohne Marke -> PORT-WAHL */
    return 1;
}

/* ============================ stempel: WATER-Stempel (sce 8) auch unter player_mode 2 (M3) ============================ */
static int teil_stempel(void)
{
    printf("\n== stempel (ROOM20B0: Aot_set sce 8 Slots 5/6, Flags 0x47 = alle Pools; FUN_800436a8 ohne Szenen-Gate)\n");
    if (!raum("STAGE2/ROOM20B0.RDT", 0x20B0u, 4350, -1800, -20500, 0, 0)) { s_fails++; return 0; }
    (void)bank_npc(0, 0x40);
    int wi = -1;
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_WATER) { wi = i; break; }
    CHECK(wi >= 0, "20B0 traegt einen WATER-Satz (Slot %d)", wi);
    if (wi < 0) return 0;
    const re15_aot_t *a = &g_aot.slots[wi];
    int16_t p0 = g_aot.env_params[wi].p0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e = &g_actors[3];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x40; e->hp = 100; e->x = a->x; e->z = a->z; e->y = -1800; e->floor = (uint8_t)(a->band & 0x7f); e->state = 4; e->motion = 2;
    pl->x = a->x; pl->z = a->z; pl->floor = e->floor;
    pl->water_y = 0; e->water_y = 0;
    g_scd.player_mode = 2;                       /* Szene (Pad-Sperre) -> in_cinematic */
    re15_aot_scan(pl->x, pl->z, 0);
    CHECK(e->water_y == p0 && p0 != 0, "NPC im WATER-Satz %d unter player_mode 2: +0x88 = %d (p0 %d)", wi, (int)e->water_y, (int)p0);
    CHECK(pl->water_y == p0, "Spieler im WATER-Satz %d unter player_mode 2: +0x88 = %d (p0 %d)", wi, (int)pl->water_y, (int)p0);
    g_scd.player_mode = 0;
    pl->water_y = 0; e->water_y = 0;
    re15_aot_scan(pl->x, pl->z, 0);
    CHECK(e->water_y == p0 && pl->water_y == p0, "dasselbe im freien Lauf (player_mode 0): NPC %d, Spieler %d", (int)e->water_y, (int)pl->water_y);

    /* PRO-BILD-STEMPEL (Fortsetzung 2026-10-24): `sh zero,136(v0)` @0x8001a59c loescht +0x88 je Entity nach dem Handler, der
     * AUTO-Pass stempelt neu. Takt A: NPC im WATER-Satz (Bild ohne Marke) -> gestempelt. Takt B: derselbe NPC AUSSERHALB jedes
     * WATER-Satzes auf trockenem Boden, gezeichnetes Bild 7 (Marke) -> der Schritt sieht KEINEN Stempel mehr -> Zweig trocken. */
    if (!bank_npc(0, 0x40)) { CHECK(0, "Bank 0x40 nicht ladbar"); return 0; }
    const re15_wassersprung_r41_mess_t *m = re15_wassersprung_r41_mess();
    int32_t ax = 0, az = 0, gefunden = 0;
    for (int32_t dx = 1000; dx <= 12000 && !gefunden; dx += 500)
        for (int sg = -1; sg <= 1 && !gefunden; sg += 2) {
            int32_t x = a->x + sg * (a->half_w + dx), z = a->z;
            int fl = re15_rdt_floor_sound(&g_room_rdt, x, z);
            if (re15_aot_water_at(x, z) == 0 && !(fl & 0x80)) { ax = x; az = z; gefunden = 1; }
        }
    CHECK(gefunden, "trockener Punkt ausserhalb aller WATER-Saetze neben Satz %d: (%d,%d)", wi, (int)ax, (int)az);
    if (!gefunden) return 0;
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x40; e->hp = 100; e->x = a->x; e->z = a->z; e->y = -1800; e->floor = (uint8_t)(a->band & 0x7f);
    e->state = 4; e->sub_state_1 = 9; e->sub_state_2 = 1; e->motion = 5; e->anim_frame = 0;
    e->steer_x = (int16_t)(e->x - 3000); e->steer_z = (int16_t)e->z; e->rot_y = 0;   /* Ziel hinter dem NPC: Sub 9 dreht weiter */
    re15_esp_fx_reset(); re15_wassersprung_r41_reset();
    bild();
    int16_t st_a = e->water_y;
    unsigned sch_a = m->npc_schritte;
    e->x = ax; e->z = az; e->motion = 5; e->anim_frame = 7; e->sub_state_1 = 9; e->sub_state_2 = 1; e->state = 4;
    bild();
    CHECK(st_a == p0 && sch_a == 0, "Takt A im Satz %d: Stempel %d (p0 %d), kein Schritt (%u)", wi, (int)st_a, (int)p0, sch_a);
    CHECK(m->npc_schritte == 1 && m->npc_stempel == 0 && m->npc_boden == 0 && e->water_y == 0,
          "Takt B ausserhalb: 1 Schritt, Zweig trocken (stempel %u, boden %u), +0x88 = %d (Pro-Bild-Wert @0x8001a59c)",
          m->npc_stempel, m->npc_boden, (int)e->water_y);
    return 1;
}

/* ============================ npc_subs: ALLE NPC-Sub-Aufrufe von FUN_80045630 + gezeichneter Zustand (Fortsetzung) ============================
 * re15_wassersprung_r41.h Kopf (d): Sub 9 @0x80051e88, Kante [1] @0x800525d4, Schub [1] @0x80052930, Sub 4/5 Phase 2 VOR
 * Kurs/Vorschub (@0x800512d4 / @0x80051640), Bildwort *(+0x168) und Part (+0x188) des GEZEICHNETEN Bilds (Sub 5 Phase 1 setzt
 * +0x94/+0x95 @0x8005157c-8c VOR dem Test @0x800515e4). Typ 0x40 (CDEMD0), OWN-Clip 5 Marken Bild 7 (Fuss 7) / 22 (Fuss 4),
 * OWN-Clip 0 Marken Bild 0 / 12 (Tabelle Dossier N1.3). Ort: Wasserfleck FLR 0x97 in ROOM2000 -> Zweig boden, Ton 0x17-8 = 15. */
/* Nachbesserung 2 (M1/M4): Wasserfleck 0x97, a0 = 0: Original-Zeile 0x17 - 8 = 15 ist in ROOM2000 leer (EDT Satz 15 = 00000000,
 * im Original stumm) -> RE2-Retail-Zeile 0x17 = 23 (FUN_8005c040 @0x8005c44c/@0x8005c458) - und die SPIELT (VAG >= 0). */
#define TON_WASSER(m) ((m)->letzter_ton_orig == 15 && (m)->letzter_ton == 23 && (m)->letzte_quelle == 1 && ton_vag((m)->letzter_ton) >= 0)

static re15_actor_t *npc_sub_setzen(uint8_t typ, int sub, int phase, int clip, int bild_, int32_t x, int32_t z, int16_t rot,
                                     int32_t zx, int32_t zz)
{
    re15_actor_t *e = &g_actors[3];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = typ; e->hp = 100; e->x = x; e->y = -1800; e->z = z; e->rot_y = rot; e->floor = 1;
    e->state = 4; e->sub_state_1 = (uint8_t)sub; e->sub_state_2 = (uint8_t)phase;
    e->steer_x = (int16_t)zx; e->steer_z = (int16_t)zz; e->walk_dest_x = (int16_t)zx; e->walk_dest_z = (int16_t)zz;
    e->motion = (int16_t)clip; e->anim_frame = bild_;
    re15_esp_fx_reset(); re15_wassersprung_r41_reset();
    return e;
}

static int teil_npc_subs(void)
{
    printf("\n== npc_subs (ROOM2000 Wasserfleck FLR 0x97, Typ 0x40 OWN-Bank: Sub 9 / Kante / Schub / Sub 5 Phase 1 / Sub 4 Phase 2)\n");
    if (!raum("STAGE2/ROOM2000.RDT", 0x2000u, 4350, -1800, -20500, 0, 0)) { s_fails++; return 0; }
    if (!bank_npc(0, 0x40)) { CHECK(0, "Bank 0x40 nicht ladbar"); return 0; }
    re15_enemy_bank_t *b = re15_enemy_find(0x40);
    CHECK(b && b->own_ok && (re15_emd_get_frame_entry(&b->anim_own, 5, 7) & 0x4000u) && !(re15_emd_get_frame_entry(&b->anim_own, 5, 0) & 0x4000u)
          && (re15_emd_get_frame_entry(&b->anim_own, 0, 0) & 0x4000u),
          "Vorbedingung OWN 0x40: Clip 5 Bild 7 Marke, Clip 5 Bild 0 ohne, Clip 0 Bild 0 Marke");
    if (!b || !b->own_ok) return 0;
    const re15_wassersprung_r41_mess_t *m = re15_wassersprung_r41_mess();
    const int32_t wx = -4300, wz = -5000;
    CHECK(re15_rdt_floor_sound(&g_room_rdt, wx, wz) == 0x97, "Wasserfleck (%d,%d) = FLR 0x97", (int)wx, (int)wz);

    /* (1) Sub 9 DREHEN, Phase 0: INIT setzt +0x94 = 5 / +0x95 = 0 (@0x80051d3c/4c), der Test liest das GEZEICHNETE Bild
     *     (Clip 5 Bild 7 = Marke) NACH dem Kurs @0x80051e44; Part = gezeichnete Lage/Gier (vor dem Kurs). */
    re15_actor_t *e = npc_sub_setzen(0x40, 9, 0, 5, 7, wx, wz, 0, wx - 3000, wz);   /* Ziel hinter dem NPC -> nicht ausgerichtet */
    re15_actor_t vor = *e;
    bild();
    int32_t erw[3] = { 0, 0, 0 };
    int rv = re15_wasser_r38_anker(&b->skel_own, &b->anim_own, &vor, -1, 7u, 7, erw);
    CHECK(m->npc_schritte == 1 && m->npc_boden == 1 && TON_WASSER(m) && m->letzter_fuss == 7,
          "Sub 9 (Drehen, @0x80051e88): 1 Schritt aus dem gezeichneten Bild 7, Zweig boden, Ton %d (Original 15 leer -> Retail 23; a0 0 @0x80051e70), Fuss %u (schritte %u)",
          m->letzter_ton, (unsigned)m->letzter_fuss, m->npc_schritte);
    CHECK(rv == 0 && e->rot_y != vor.rot_y && m->letzter_anker[0] == erw[0] && m->letzter_anker[1] == erw[1] && m->letzter_anker[2] == erw[2],
          "Sub 9: Anker = Part 7 der GEZEICHNETEN Pose (Gier %d, nach dem Kurs %d): (%d,%d,%d) vs (%d,%d,%d)", (int)vor.rot_y, (int)e->rot_y,
          (int)m->letzter_anker[0], (int)m->letzter_anker[1], (int)m->letzter_anker[2], (int)erw[0], (int)erw[1], (int)erw[2]);

    /* (2) Kante 0x12 Phase 1 (@0x80052598): Schritt vor anim_set @0x800525f0 */
    e = npc_sub_setzen(0x40, 0x12, 1, 5, 7, wx, wz, 0x200, wx, wz - 3000);
    bild();
    CHECK(m->npc_schritte == 1 && m->npc_boden == 1 && TON_WASSER(m),
          "Kante 0x12 Phase 1 (@0x800525d4): 1 Schritt, Zweig boden, Ton %d (Original 15 leer -> Retail 23; a0 0 @0x800525bc), schritte %u", m->letzter_ton, m->npc_schritte);

    /* (3) Schub 0x13 Phase 1 (@0x800528f4): Schritt vor anim_set @0x8005294c */
    e = npc_sub_setzen(0x40, 0x13, 1, 5, 7, wx, wz, 0x200, wx, wz - 3000);
    bild();
    CHECK(m->npc_schritte == 1 && m->npc_boden == 1 && TON_WASSER(m),
          "Schub 0x13 Phase 1 (@0x80052930): 1 Schritt, Zweig boden, Ton %d (Original 15 leer -> Retail 23; a0 0 @0x80052918), schritte %u", m->letzter_ton, m->npc_schritte);

    /* (4) Sub 5 LAUFEN aus dem Stand, Ziel geradeaus (sofort ausgerichtet): Takt 1 setzt +0x94 = 0 / +0x95 = 0 (@0x8005157c-8c)
     *     VOR dem Test @0x800515e4 -> gelesen wird das gezeichnete Clip-5-Bild 0 (keine Marke) -> KEIN Schritt (der Port las
     *     vorher Clip 0 Bild 0 = Marke -> Doppelschritt am Laufbeginn, gemessen 11C0 F1071/F1072). Takt 2 (Phase 2): das in
     *     Takt 1 posierte Clip-0-Bild 0 traegt die Marke -> genau EIN Schritt mit a0 = 0 (@0x8005165c) -> Ton 15. */
    e = npc_sub_setzen(0x40, 5, 0, 5, 0, wx, wz, 1024, wx, wz - 6000);   /* Gier 1024: Vorschub nach -z (pos_advance) */
    bild();
    unsigned s1 = m->npc_schritte;
    int m1 = (int)e->motion;
    bild();
    CHECK(s1 == 0 && m1 == 0 && m->npc_schritte == 1 && TON_WASSER(m),
          "Sub 5 Laufbeginn: Takt 1 %u Schritte (Clip danach %d), Takt 2 gesamt %u, Ton %d (erwartet 0 / 0 / 1 / 15)", s1, m1, m->npc_schritte, m->letzter_ton);

    /* (5) Sub 4 Phase 2: der Schritt liegt VOR Kurs und Vorschub (@0x800512d4-308 vor @0x8005133c/@0x80051344). Lage am
     *     Rand des Flecks, so dass die Lage VOR dem Vorschub im Wasser (0x97) und die DANACH trocken ist. */
    int32_t zr = 0, zt = 0;
    for (int32_t z = wz; z > wz - 8000; z -= 10)
        if (re15_rdt_floor_sound(&g_room_rdt, wx, z) != 0x97) { zr = z + 10; zt = z; break; }
    int trocken = (zt != 0) ? re15_rdt_floor_sound(&g_room_rdt, wx, zt - 300) : -1;
    CHECK(zr != 0 && trocken >= 0 && !(trocken & 0x80), "Fleckrand bei z %d (dahinter Boden %#x)", (int)zr, trocken);
    e = npc_sub_setzen(0x40, 4, 2, 5, 7, wx, zr, 1024, wx, zr - 6000);
    e->crow_speed = 400;                                     /* ein Vorschub ueber den Rand (pos_advance +0x8c) */
    bild();
    CHECK(m->npc_schritte == 1 && m->npc_boden == 1 && TON_WASSER(m) && e->z < zr - 300,
          "Sub 4 Phase 2 am Fleckrand: Zweig boden %u, Ton %d (Original 15 leer -> Retail 23; Boden VOR dem Vorschub; danach trocken %#x), z %d -> %d",
          m->npc_boden, m->letzter_ton, trocken, (int)zr, (int)e->z);

    /* (6) RUECKWAERTS-Subs 7/8 (Nachbesserung 2, M4): INIT +0x94 = 1 (`sb v1,148(v0)` @0x8005196c / @0x80051b64), dann je Takt Kurs
     *     (`jal 0x8001aac4` @0x800519c0 / @0x80051bb8), Vorschub pos_advance(0x800) (@0x800519c8 / @0x80051bc0) und ERST DANACH der
     *     Bildwort-Test (@0x800519dc / @0x80051bd4) mit Schritt (@0x80051a0c / @0x80051c04, a0 = 0 @0x800519f8); Bank = LOCO-Paar
     *     +0x84/+0x16c (@0x80051a20-24 / @0x80051c18-1c; Port: Container, Clip 1, Marken 7/22). Lauf rueckwaerts durch den
     *     Wasserfleck: Blick +z (Gier 3072), Ziel -z. */
    for (int sub = 7; sub <= 8; sub++) {
        e = npc_sub_setzen(0x40, sub, 0, 1, 0, wx, wz + 2500, 3072, wx, wz - 6000);
        const int32_t z0 = e->z;
        int takte = 0;
        for (; takte < 90; takte++) {
            bild();
            if (e->state != 4 || e->sub_state_1 != (uint8_t)sub) break;
        }
        printf("     Sub %d: %d Takte, z %d -> %d, Clip %d, Schritte %u (boden %u), letzter Clip %u Bild %u Fuss %u, Ton %d (Original %d), Lage %d/%d\n",
               sub, takte, (int)z0, (int)e->z, (int)e->motion, m->npc_schritte, m->npc_boden, (unsigned)m->letzter_clip,
               (unsigned)m->letztes_bild, (unsigned)m->letzter_fuss, m->letzter_ton, m->letzter_ton_orig, m->letzte_lage[0], m->letzte_lage[1]);
        CHECK(e->z < z0 - 500 && m->npc_schritte >= 2 && m->npc_boden == m->npc_schritte && m->letzter_clip == 1
              && (m->letztes_bild == 7 || m->letztes_bild == 22) && TON_WASSER(m) && m->npc_ton_stumm == 0,
              "Sub %d rueckwaerts (Schritt @0x%08x NACH dem Vorschub): z %d -> %d, %u Schritte Clip %u (Bild %u), Zweig boden %u, Ton %d gespielt",
              sub, sub == 7 ? 0x80051a0cu : 0x80051c04u, (int)z0, (int)e->z, m->npc_schritte, (unsigned)m->letzter_clip,
              (unsigned)m->letztes_bild, m->npc_boden, m->letzter_ton);
    }
    return 1;
}

/* ============================ ton: Schritt-Ton aller NPC-Raeume ueber den ECHTEN Schritt (Nachbesserung 2, M1/M4) ============================
 * Die 52 Raeume mit NPC-Sce_em_set (Typ 0x40..0x4f; Zensus Abnahme 1 A41abn1/npc_ton_zensus.txt). Je Raum jeder FLR-Bodentyp an einer
 * Stelle dieses Typs, Zweig ohne/mit Stempel (Boden - 3 @0x800456ec), a0 0/1, Fuss 7/4 -> re15_wassersprung_r41_npc_schritt. Erwartung
 * UNABHAENGIG aus den EDT-Bytes + der Ton-Aufloesung von audio_pc.c (re15_footstep_vag): Original-Zeile (Boden & 0x7f) + a0 - 8
 * hoerbar, wenn Stimme (e[3] & 0x1f) - 16 + (Fuss != 4) in 0..7 (@0x800457a0-b4, 8 Saetze @0x800458dc) UND der Ton einen VAG hat
 * (SsUtKeyOnV keyt nur vag != 0: `lhu a0,22(v0)` @0x80059f2c, `bne a0,zero` @0x80059f8c); sonst Retail-Zeile (Boden & 0x7f) + a0
 * (FUN_8005c040 @0x8005c44c/@0x8005c458) mit VAG; sonst stumm. Nur erreichbare Bodentypen (FLR-Suche mit Ende x == 0 wie
 * FUN_800437d4 / re15_rdt_floor_sound: ROOM10D0 Boden 0x1d liegt hinter dem Ende). Zaehlung: N2.4 im Dossier. */
static const char *const k_npc_raeume[52] = {
    "1011","1021","1050","1090","10B1","10D0","1141","1150","1151","1170","1171","11A0","11A1","11B0","11B1","11C0","1211",
    "1260","1261","2000","2001","2011","3000","3001","3010","3011","3020","3021","3060","3061","3070","3071","3080","3081",
    "3091","30C1","30E0","30E1","4000","4001","4010","4031","5011","5021","5031","5090","50B0","6000","6020","6021","6030","6031" };

/* Zaehlung (je Fall unabhaengig erwartet, s.o.; dieselbe Zahl liefert A_nb2/npc_ton_retail2.py ohne ROOM10D0 0x1d): 35 Original /
 * 387 Retail / 114 stumm; ohne Stempel-Zweig (Gehen auf trockenem Boden / Wasserboden 0x80): 15 / 257 / 8, die 8 stummen = ROOM6030/6031
 * Boden 0x17 (Zeilen 15/16 und 23/24 ohne Ton - dort ist auch der Spieler stumm). Original vorher: 15 von 280 hoerbar. */
#define K_TON_ORIG   35
#define K_TON_RETAIL 387
#define K_TON_STUMM  114
#define K_TON_STUMM_OHNE_STEMPEL 8
static int flr_stelle(uint8_t typ, int32_t *x, int32_t *z)
{
    const uint8_t *f = g_room_rdt.flr;
    if (!f) return 0;
    const int n = f[0] | (f[1] << 8);
    static const int fr[5][2] = { {1,2}, {1,4}, {3,4}, {1,8}, {7,8} };
    for (int k = 0; k < n; k++) {
        const uint8_t *e = f + 2 + 12 * k;
        if (e[8] != typ) continue;
        const int32_t ex = (int16_t)(e[0] | (e[1] << 8)), ez = (int16_t)(e[2] | (e[3] << 8));
        const int32_t w  = (int16_t)(e[4] | (e[5] << 8)), d  = (int16_t)(e[6] | (e[7] << 8));
        for (int a = 0; a < 5; a++) for (int b = 0; b < 5; b++) {
            const int32_t px = ex + w * fr[a][0] / fr[a][1], pz = ez + d * fr[b][0] / fr[b][1];
            if (re15_rdt_floor_sound(&g_room_rdt, px, pz) == typ) { *x = px; *z = pz; return 1; }
        }
    }
    return 0;
}

static int teil_ton(void)
{
    printf("\n== ton (Nachbesserung 2: Schritt-Ton der NPCs in allen 52 NPC-Raeumen, Original oder RE2-Retail, und er SPIELT)\n");
    unsigned n_orig = 0, n_retail = 0, n_stumm = 0, n_falsch = 0, n_vag = 0, n_typ_fehlt = 0, n_faelle = 0, n_stumm_trocken = 0;
    g_scd.work_vars[0x0A] = 0;
    for (int ri = 0; ri < 52; ri++) {
        char p[512]; size_t n = 0;
        snprintf(p, sizeof p, "%s/STAGE%c/ROOM%s.RDT", RE15_ASSET_PSX_DIR, k_npc_raeume[ri][0], k_npc_raeume[ri]);
        free(s_rdt_buf); s_rdt_buf = slurp(p, &n);
        memset(&g_room_rdt, 0, sizeof g_room_rdt);
        if (!s_rdt_buf || re15_rdt_parse(s_rdt_buf, n, &g_room_rdt) != 0) { CHECK(0, "ROOM%s nicht ladbar", k_npc_raeume[ri]); continue; }
        g_room_rdt_ok = 1;
        re15_esp_set_room_bank(NULL);
        const uint8_t *edt = g_room_rdt.snd_edt[0];
        const int nedt = g_room_rdt.snd_edt_size[0] / 4;
        uint8_t typen[64]; int nt = 0;
        const uint8_t *f = g_room_rdt.flr;
        const int nf = f ? (f[0] | (f[1] << 8)) : 0;
        for (int k = 0; k < nf; k++) {
            const uint8_t t = f[2 + 12 * k + 8];
            int da = 0; for (int j = 0; j < nt; j++) if (typen[j] == t) da = 1;
            if (!da && nt < 64) typen[nt++] = t;
        }
        unsigned r_orig = 0, r_retail = 0, r_stumm = 0;
        for (int ti = 0; ti < nt; ti++) {
            const uint8_t t = typen[ti];
            int32_t x = 0, z = 0;
            if (!flr_stelle(t, &x, &z)) { n_typ_fehlt++; printf("     ROOM%s: keine Stelle fuer Boden 0x%02x\n", k_npc_raeume[ri], t); continue; }
            for (int stempel = 0; stempel < 2; stempel++) {
                if (stempel && (t & 0x80)) continue;
                for (int a0 = 0; a0 < 2; a0++) for (int fi = 0; fi < 2; fi++) {
                    const int fuss = fi ? 4 : 7;
                    re15_actor_t *e = &g_actors[3];
                    memset(e, 0, sizeof *e);
                    e->active = 1; e->type = 0x40; e->hp = 100; e->x = x; e->y = -1800; e->z = z; e->floor = 1;
                    e->water_y = stempel ? -900 : 0;
                    re15_wassersprung_r41_reset();
                    const int idx = re15_wassersprung_r41_npc_schritt(e, a0, fuss, &g_room_rdt);
                    const re15_wassersprung_r41_mess_t *m = re15_wassersprung_r41_mess();
                    /* unabhaengige Erwartung aus den Bytes */
                    const int boden = (t & 0x7f) - (stempel ? 3 : 0);
                    const int io = boden + a0 - 8, ir = boden + a0;
                    int q, soll;
                    int hoerbar = 0;
                    if (io >= 0 && io < nedt) { const int v = (edt[io * 4 + 3] & 0x1f) - 16 + (fuss != 4); hoerbar = (v >= 0 && v <= 7) && ton_vag(io) >= 0; }
                    if (hoerbar) { q = 0; soll = io; }
                    else if (ir >= 0 && ir < nedt && ton_vag(ir) >= 0) { q = 1; soll = ir; }
                    else { q = 2; soll = -1; }
                    n_faelle++;
                    if (q == 0) { n_orig++; r_orig++; } else if (q == 1) { n_retail++; r_retail++; } else { n_stumm++; r_stumm++; }
                    if (q == 2 && !stempel) {
                        n_stumm_trocken++;
                        printf("     stumm ohne Stempel: ROOM%s Boden 0x%02x a0=%d Fuss %d (Zeilen %d / %d ohne Ton)\n", k_npc_raeume[ri], t, a0, fuss, io, ir);
                    }
                    if (idx != soll || m->letzte_quelle != q || m->letzter_ton_orig != io) {
                        n_falsch++;
                        printf("     FALSCH ROOM%s Boden 0x%02x%s a0=%d Fuss %d: Port %d (Quelle %u, Original %d), Soll %d (Quelle %d)\n",
                               k_npc_raeume[ri], t, stempel ? "/Stempel" : "", a0, fuss, idx, (unsigned)m->letzte_quelle, m->letzter_ton_orig, soll, q);
                    }
                    if (q != 2) {
                        const int vag = ton_vag(idx);
                        if (vag < 0) { n_vag++; printf("     KEIN VAG ROOM%s Boden 0x%02x Zeile %d -> %d\n", k_npc_raeume[ri], t, idx, vag); }
                    }
                }
            }
        }
        printf("     ROOM%s: %d Bodentypen, Original %u / Retail %u / stumm %u\n", k_npc_raeume[ri], nt, r_orig, r_retail, r_stumm);
    }
    CHECK(n_falsch == 0, "Zeile + Quelle des Ports = Erwartung aus den EDT-Bytes in allen %u Faellen (%u falsch)", n_faelle, n_falsch);
    CHECK(n_vag == 0, "jeder nicht stumme NPC-Schritt hat einen VAG (der Ton spielt): %u ohne", n_vag);
    CHECK(n_typ_fehlt == 1 && n_orig == K_TON_ORIG && n_retail == K_TON_RETAIL && n_stumm == K_TON_STUMM,
          "Zaehlung 52 Raeume: Original %u / Retail %u / stumm %u (Soll %d / %d / %d, Dossier N2.4), unerreichbare Bodentypen %u (Soll 1: ROOM10D0 0x1d)",
          n_orig, n_retail, n_stumm, K_TON_ORIG, K_TON_RETAIL, K_TON_STUMM, n_typ_fehlt);
    CHECK(n_stumm_trocken == K_TON_STUMM_OHNE_STEMPEL,
          "ohne Stempel-Zweig stumm: %u (Soll %d = ROOM6030/6031 Boden 0x17, wo auch der Spieler keinen Ton hat)", n_stumm_trocken, K_TON_STUMM_OHNE_STEMPEL);
    /* Einzelstiche (Dossier N2.1): ROOM2000 Boden 0x1d a0 = 1 Fuss 7 -> Original 22 (0000b111, Stimme 2); ROOM6020 0x1d a0 = 0 -> Satz
     * 21 = 86001f0e hat Stimme -2/-1 (im Original stumm) -> Retail 29 */
    return 1;
}

/* ============================ lage: FUN_80045a64 + SsUtKeyOnV/SpuVmKeyOnNow (Nachbesserung 2, M2 / Entscheid A-E4) ============================ */
static int teil_lage(void)
{
    printf("\n== lage (Nachbesserung 2, M2: Lage des NPC-Schritts nach FUN_80045a64 @0x80045a64, Pegel nach SsUtKeyOnV @0x80059d3c / SpuVmKeyOnNow @0x80055d10)\n");
    /* (1) Bytes: PAN @0x80074728 / ATT @0x800747a8 und die CORDIC-Winkel @0x80078c90 = PSX.EXE (Ladeadresse = Kopf +0x18, Daten ab 0x800) */
    size_t n = 0; uint8_t *exe = slurp(RE15_R41A_PSX_EXE, &n);
    CHECK(exe && n > 0x800, "PSX.EXE geladen (%s, %zu B)", RE15_R41A_PSX_EXE, n);
    if (exe) {
        const uint32_t t_addr = u32(exe, 0x18);
        const size_t ot = 0x80074728u - t_addr + 0x800u, ow = 0x80078c90u - t_addr + 0x800u;
        CHECK(ot + 256 <= n && memcmp(exe + ot, re15_wassersprung_r41_lage_tabelle(), 256) == 0,
              "PAN/ATT der Nachbildung = PSX.EXE @0x80074728..0x80074827 (t_addr 0x%08x)", t_addr);
        static const uint32_t W[12] = { 511, 302, 159, 81, 41, 20, 10, 5, 3, 1, 0, 0 };
        int ok = (ow + 48 <= n);
        for (int i = 0; ok && i < 12; i++) ok = (u32(exe, ow + 4 * (size_t)i) == W[i]);
        CHECK(ok, "CORDIC-Winkel @0x80078c90 = 511,302,159,81,41,20,10,5,3,1,0,0 (catan @0x800658fc)");
        free(exe);
    }
    /* (2) Von Hand aus dem Disasm gerechnet: Kamera (0,-2000,0) -> Ziel (0,-2000,10000) (Blick +z), Satz *(RDT+0x24) + Cut*0x20. */
    static re15_camera_cut_t cut[1];
    memset(cut, 0, sizeof cut);
    cut[0].pos_x = 0; cut[0].pos_y = -2000; cut[0].pos_z = 0; cut[0].target_x = 0; cut[0].target_y = -2000; cut[0].target_z = 10000;
    re15_rdt_t fake; memset(&fake, 0, sizeof fake); fake.cuts = cut; fake.cut_count = 1;
    g_scd.work_vars[0x0A] = 0;
    int lage[2] = { -1, -1 }, ab = -1;
    /* Abstand mit der BIOS-Wurzel SquareRoot0 (`jal 0x80065f60` @0x80045b04/@0x80045b10/@0x80045b34; re15_squareroot0 = dieselbe
     * Rechnung, sie unterschaetzt die Wurzel): r = SR0((Kamera.y - |Kamera.y - y|)^2 + SR0(dx^2+dz^2)^2), /250. */
#define R41A_ABSTAND(dxz2, dyt) ((int)(re15_squareroot0((uint32_t)((dyt) * (dyt)) + re15_squareroot0(dxz2) * re15_squareroot0(dxz2)) / 250u))
    const int ab_a = R41A_ABSTAND(100000000u, -4000), ab_b = R41A_ABSTAND(125000000u, -4000), ab_c = R41A_ABSTAND(625000000u, -4000);
    printf("     SquareRoot0: SR0(1e8) = %u, Abstaende a/b/c = %d/%d/%d (exakte Wurzel: 43/47/101)\n", re15_squareroot0(100000000u), ab_a, ab_b, ab_c);
    /* (a) geradeaus (0,0,10000): exakt sqrt(4000^2 + 10000^2) = 10770 -> 43, mit SquareRoot0 42; ATT[42] = ATT[43] = 34 (ATT[k] = 26 +
     *     (k-26)/2 ab k 26, Bytes @0x800747a8); Richtung = Blick (FUN_80045d6c dx 0 -> 0xc00 beide) -> rel 0 -> beide 0x89 (@0x80045c0c-1c)
     *     -> (0x89 - 34) & 0x7f = 103. */
    const int32_t pa[3] = { 0, 0, 10000 };
    int rv = re15_wassersprung_r41_lage(&fake, pa, lage, &ab);
    CHECK(rv == 1 && ab == ab_a && (ab == 42 || ab == 43) && lage[0] == 103 && lage[1] == 103,
          "geradeaus 10000 vor der Kamera: Abstand %d (SquareRoot0: %d), voll/volr %d/%d (103/103)", ab, ab_a, lage[0], lage[1]);
    /* (b) seitlich (5000,0,10000): (10000 << 12) / 5000 = 8192 -> CORDIC 721 -> Richtung (0x1000 - 721) & 0xfff = 3375; Blick 0xc00,
     *     Ursprung 0x400 -> rel = 2351 - 2048 = 303, Bit 0x800 frei, kein Spiegel -> i = 303 >> 3 = 37, PAN[37] = 119: voll = PAN
     *     (`sh a0` @0x80045ce4), volr = 0x89 (@0x80045cd4); Abstand sqrt(4000^2 + 11180^2) = 11874 -> 47, ATT[47] = 36 -> 83 / 101. */
    const int32_t pb[3] = { 5000, 0, 10000 };
    rv = re15_wassersprung_r41_lage(&fake, pb, lage, &ab);
    {   const int att = 26 + (ab_b - 26) / 2;   /* ATT[46] = ATT[47] = 36 */
        CHECK(rv == 1 && ab == ab_b && att == 36 && lage[0] == 119 - att && lage[1] == 0x89 - att,
              "seitlich +x: Abstand %d (SquareRoot0: %d, ATT %d), voll/volr %d/%d (PAN[37] 119 - 36 = 83 / 0x89 - 36 = 101)", ab, ab_b, att, lage[0], lage[1]); }
    /* (c) weiter weg geradeaus (0,0,25000): sqrt(4000^2 + 25000^2) = 25317 -> 101, ATT[101] = 26 + (101-26)/2 = 63 -> 0x89 - 63 = 74/74 (leiser) */
    const int32_t pc[3] = { 0, 0, 25000 };
    rv = re15_wassersprung_r41_lage(&fake, pc, lage, &ab);
    {   const int att = 26 + (ab_c - 26) / 2;   /* ATT[100] = ATT[101] = 63 */
        CHECK(rv == 1 && ab == ab_c && att == 63 && lage[0] == 0x89 - att && lage[1] == 0x89 - att,
              "geradeaus 25000: Abstand %d (SquareRoot0: %d, ATT %d), voll/volr %d/%d (74/74) - leiser als 103", ab, ab_c, att, lage[0], lage[1]); }
    /* (d) ohne Kamera (Cut ausserhalb) -> keine Lage */
    g_scd.work_vars[0x0A] = 5;
    rv = re15_wassersprung_r41_lage(&fake, pa, lage, &ab);
    CHECK(rv == 0, "Cut 5 bei 1 Kamerasatz -> keine Lage (%d)", rv);
    g_scd.work_vars[0x0A] = 0;
    /* (3) Pegel (Port-Q15 des Tons bei Tastenpegel 0x7f = 8192): SsUtKeyOnV volr > voll -> Pegel volr, Pan 0x7f - (voll<<6)/volr
     *     (@0x80059e78-bc) = 127 - 5312/101 = 75 -> links * (0x7f - 75) / 63 (@0x80055f8c), rechts voll. 8192*101/127 = 6514 -> L 5376 R 6514. */
    int vol = 8192; const int l1[2] = { 83, 101 };
    int r = re15_wassersprung_r41_lage_pegel(&vol, l1, 0);
    CHECK(vol == 5376 && r == 6514, "Pegel voll/volr 83/101 -> L/R %d/%d (5376/6514)", vol, r);
    vol = 8192; const int l2[2] = { 101, 83 };
    r = re15_wassersprung_r41_lage_pegel(&vol, l2, 0);
    CHECK(vol == 6514 && r == 5376, "Pegel voll/volr 101/83 (Pan (83<<6)/101 = 52 @0x80059e30-6c) -> L/R %d/%d (6514/5376)", vol, r);
    vol = 8192; const int l3[2] = { 103, 103 };
    r = re15_wassersprung_r41_lage_pegel(&vol, l3, 0);
    CHECK(vol == 6643 && r == 6643, "Pegel 103/103 (Pan 0x40 @0x80059e10) -> L/R %d/%d (6643/6643)", vol, r);
    vol = 8192; const int l4[2] = { 74, 74 };
    r = re15_wassersprung_r41_lage_pegel(&vol, l4, 0);
    CHECK(vol == 4773 && r == 4773, "Pegel 74/74 (weiter weg) -> L/R %d/%d (4773/4773, leiser als 6643)", vol, r);
    /* (4) Im Raum: NPC-Schritt in ROOM2000 traegt die Lage (Messzeile) und gibt sie NICHT an einen spaeteren Schritt weiter */
    if (!raum("STAGE2/ROOM2000.RDT", 0x2000u, 4350, -1800, -20500, 0, 0)) { s_fails++; return 0; }
    re15_actor_t *e = &g_actors[3];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x40; e->hp = 100; e->x = -4300; e->y = -1800; e->z = -5000; e->floor = 1;
    re15_wassersprung_r41_reset();
    const int cut_raum = (int)g_scd.work_vars[0x0A];
    (void)re15_wassersprung_r41_npc_schritt(e, 0, 7, &g_room_rdt);
    const re15_wassersprung_r41_mess_t *m = re15_wassersprung_r41_mess();
    int erw[2] = { -1, -1 }, erw_ab = -1;
    const int32_t pe[3] = { e->x, e->y, e->z };
    (void)re15_wassersprung_r41_lage(&g_room_rdt, pe, erw, &erw_ab);
    int rest[2];
    CHECK(m->letzte_lage[0] >= 0 && m->letzte_lage[0] == erw[0] && m->letzte_lage[1] == erw[1] && m->letzter_abstand == erw_ab
          && re15_wassersprung_r41_lage_nehmen(rest) == 0,
          "ROOM2000 Cut %d: Schritt bei (-4300,-1800,-5000) -> Lage %d/%d Abstand %d; danach keine haengende Lage",
          cut_raum, m->letzte_lage[0], m->letzte_lage[1], m->letzter_abstand);
    return 1;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "bytes"))     teil_bytes();
    if (alle || !strcmp(teil, "modell"))    teil_modell();
    if (alle || !strcmp(teil, "sequencer")) teil_sequencer();
    if (alle || !strcmp(teil, "raum"))      teil_raum();
    if (alle || !strcmp(teil, "landung"))   teil_landung();
    if (alle || !strcmp(teil, "npc"))       teil_npc();
    if (alle || !strcmp(teil, "npc_wasser")) teil_npc_wasser();
    if (alle || !strcmp(teil, "stempel"))   teil_stempel();
    if (alle || !strcmp(teil, "npc_subs"))  teil_npc_subs();
    if (alle || !strcmp(teil, "ton"))       teil_ton();
    if (alle || !strcmp(teil, "lage"))      teil_lage();
    printf("\n%s: %d Pruefungen, %d Fehler\n", teil, s_checks, s_fails);
    return s_fails ? 1 : 0;
}
