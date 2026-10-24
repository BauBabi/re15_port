/*
 * wasser20b0_r41.c — Runde 41 Spur A "wasser20b0": EM024-Partikel-Sequencer der Flut ROOM20B0/20B1.
 * Belege (Adressen, Original-RAM) im Kopf von include/re15_wasser20b0_r41.h und im Dossier
 * analysis/befunde_runde41/A_wasser20b0.md (1.2/1.3).
 */
#include <stdint.h>
#include <string.h>

#include "re15_wasser20b0_r41.h"
#include "re15_actor.h"
#include "re15_anim_select.h"   /* re15_compute_actor_kf                              */
#include "re15_aot.h"           /* RE15_AOT_TYPE_AUTO_EVENT                           */
#include "re15_enemy.h"         /* re15_enemy_find (Gegnerbank: Skelett + Clips)      */
#include "re15_esp.h"           /* re15_esp_fx_spawn_rows = FUN_80019700              */
#include "re15_skeleton.h"      /* re15_skel_compute_pose / re15_skel_bone_to_world   */

static re15_wasser20b0_r41_mess_t s_mess;

const re15_wasser20b0_r41_mess_t *re15_wasser20b0_r41_mess(void) { return &s_mess; }
void re15_wasser20b0_r41_reset(void) { memset(&s_mess, 0, sizeof s_mess); }

/* ---- grid 0: Phasen @0x80118d88 ----------------------------------------------------------------
 * Phase 0 @0x8010f178 (Takt 0, `lbu v0,464(v0)` / `bne v0,zero` @0x8010f188-90), a1 = 0, sub = 7:
 *   @0x8010f194-b0  (-1000,0xa8c,-1200) / 0x4000     @0x8010f1b4-d0  (-2000,0xa8c,0x4b0) / 0x4000
 *   @0x8010f1d4-f0  (-1000,0xa8c,0)     / 0x3000     @0x8010f1f4-210 (-2000,0x514,-1200) / 0x4000
 *   @0x8010f214-30  (-1000,0x514,0x4b0) / 0x4000     @0x8010f234-50  (-2000,0x514,0)     / 0x4000 */
static const re15_wasser20b0_r41_spawn_t s_g0_p0[6] = {
    { 0, -1000, 2700, -1200, 0x4000, 0 }, { 0, -2000, 2700,  1200, 0x4000, 0 },
    { 0, -1000, 2700,     0, 0x3000, 0 }, { 0, -2000, 1300, -1200, 0x4000, 0 },
    { 0, -1000, 1300,  1200, 0x4000, 0 }, { 0, -2000, 1300,     0, 0x4000, 0 },
};
/* Phase 1 @0x8010f2c0 (Takt 0), a1 = 0, sub = 7, alle 0x3000:
 *   @0x8010f2dc-f8  (-2000,0x9c4,-1200)   @0x8010f2fc-318 (-1000,0x9c4,0x4b0)   @0x8010f31c-338 (-2000,0x9c4,0)
 *   @0x8010f33c-358 (-1000,0x44c,-1200)   @0x8010f35c-378 (-2000,0x44c,0x4b0)   @0x8010f37c-398 (-1000,0x44c,0) */
static const re15_wasser20b0_r41_spawn_t s_g0_p1[6] = {
    { 0, -2000, 2500, -1200, 0x3000, 0 }, { 0, -1000, 2500,  1200, 0x3000, 0 },
    { 0, -2000, 2500,     0, 0x3000, 0 }, { 0, -1000, 1100, -1200, 0x3000, 0 },
    { 0, -2000, 1100,  1200, 0x3000, 0 }, { 0, -1000, 1100,     0, 0x3000, 0 },
};

/* ---- grid 1: Phasen @0x80118d90 ----------------------------------------------------------------
 * Phase 0 @0x8010f444, sub = 0 (`addu a2,zero,zero`), Takt-Vergleiche `ori v0,zero,0x3/0x6/0x9/0xc`
 * @0x8010f490/@0x8010f4e8/@0x8010f560/@0x8010f5b8:
 *   Takt 0  @0x8010f460-7c  (0xc8,0x708,0)       / 0x2000 a1=-200
 *   Takt 3  @0x8010f498-b4  (0x190,0x708,-600)   / 0x2600 a1=-100 ; @0x8010f4b8-d4 (0x190,0x960,0) / 0x2600 a1=-100
 *   Takt 6  @0x8010f4f0-50c (0x258,0x708,-1200)  / 0x2c00 ; @0x8010f510-2c (0x258,0x960,-600) / 0x3000 ;
 *           @0x8010f530-4c  (0x258,0xbb8,0)      / 0x2c00
 *   Takt 9  @0x8010f568-84  (0x320,0x960,-1200)  / 0x3200 ; @0x8010f588-a4 (0x320,0xbb8,-600) / 0x3200
 *   Takt 12 @0x8010f5c0-dc  (0x3e8,0xbb8,-1200)  / 0x3800 */
static const re15_wasser20b0_r41_spawn_t s_g1_p0[9] = {
    {  0,  200, 1800,     0, 0x2000, -200 },
    {  3,  400, 1800,  -600, 0x2600, -100 }, {  3,  400, 2400,     0, 0x2600, -100 },
    {  6,  600, 1800, -1200, 0x2c00,    0 }, {  6,  600, 2400,  -600, 0x3000,    0 }, {  6,  600, 3000,     0, 0x2c00, 0 },
    {  9,  800, 2400, -1200, 0x3200,    0 }, {  9,  800, 3000,  -600, 0x3200,    0 },
    { 12, 1000, 3000, -1200, 0x3800,    0 },
};
/* Phase 2 @0x8010f6b0, sub = 0:
 *   Takt 0  @0x8010f6cc-e8  (0xc8,0x708,-1200)   / 0x2000
 *   Takt 3  @0x8010f704-20  (0x190,0x708,-600)   / 0x2600 a1=0x64 ; @0x8010f724-40 (0x190,0x960,-1200) / 0x2600
 *   Takt 6  @0x8010f75c-78  (0x258,0x708,0)      / 0x2000 a1=0xc8 ; @0x8010f77c-98 (0x258,0x960,-600) / 0x3000 ;
 *           @0x8010f79c-b8  (0x258,0xbb8,-1200)  / 0x2c00
 *   Takt 9  @0x8010f7d4-f0  (0x320,0x258,0)      / 0x3200 a1=0x64 ; @0x8010f7f4-810 (0x320,0xbb8,-600) / 0x3200
 *   Takt 12 @0x8010f82c-48  (0x3e8,0xbb8,0)      / 0x3800 */
static const re15_wasser20b0_r41_spawn_t s_g1_p2[9] = {
    {  0,  200, 1800, -1200, 0x2000,   0 },
    {  3,  400, 1800,  -600, 0x2600, 100 }, {  3,  400, 2400, -1200, 0x2600,   0 },
    {  6,  600, 1800,     0, 0x2000, 200 }, {  6,  600, 2400,  -600, 0x3000,   0 }, {  6,  600, 3000, -1200, 0x2c00, 0 },
    {  9,  800,  600,     0, 0x3200, 100 }, {  9,  800, 3000,  -600, 0x3200,   0 },
    { 12, 1000, 3000,     0, 0x3800,   0 },
};

const re15_wasser20b0_r41_spawn_t *re15_wasser20b0_r41_tabelle(int grid, int phase, int *out_n)
{
    if (out_n) *out_n = 0;
    if (grid == 0 && phase == 0) { if (out_n) *out_n = 6; return s_g0_p0; }
    if (grid == 0 && phase == 1) { if (out_n) *out_n = 6; return s_g0_p1; }
    if (grid == 1 && phase == 0) { if (out_n) *out_n = 9; return s_g1_p0; }
    if (grid == 1 && phase == 2) { if (out_n) *out_n = 9; return s_g1_p2; }
    return NULL;
}

int re15_wasser20b0_r41_phasen_takte(int grid, int phase)
{
    if (grid == 0) return (phase == 0 || phase == 1) ? RE15_WASSER20B0_R41_TAKTE_KURZ : -1;
    if (grid == 1) {
        if (phase == 0 || phase == 2) return RE15_WASSER20B0_R41_TAKTE_LANG;
        if (phase == 1 || phase == 3) return RE15_WASSER20B0_R41_TAKTE_KURZ;
    }
    return -1;
}

void re15_wasser20b0_r41_init(re15_actor_t *e)
{
    if (!e) return;
    e->no_draw    = 1;   /* `lw a0,392(v0)` / `addiu v1,zero,-2` / `and v0,v0,v1` / `sw v0,0(a0)` @0x8010efc8-e4 */
    e->anim_frame = 1;   /* +0x95 = 1 (Original-RAM ts_10/12/14/16/flut_a; kein Vorschub)               */
    e->sub_state_3 = 0;  /* +0x1d0 = 0 (`sw zero,464(v0)` @0x8010efa8, erstes der 8 Worte)              */
}

int re15_wasser20b0_r41_anker(const re15_actor_t *e, const int32_t off[3], int32_t out[3])
{
    int32_t v[3] = { off[0], off[1], off[2] };
    int rv = -1;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (b && b->ok && b->skel.bone_count > 0) {
        /* Teil-0-Matrix = Pose des zuletzt gebundenen Bilds (Clip +0x94 = 0, Bild +0x95 = 1). */
        int kf = re15_compute_actor_kf(&b->anim, &b->skel, e, 0, 1);
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        void *save = g_anim_pose_actor;
        g_anim_pose_actor = NULL;                       /* Abfrage: Ueberblendung nicht anfassen */
        int ok = re15_skel_compute_pose(&b->skel, kf, poses);
        g_anim_pose_actor = save;
        if (ok == 0) {
            for (int i = 0; i < 3; i++) {
                int64_t s = (int64_t)poses[0].rot[i * 3 + 0] * off[0]
                          + (int64_t)poses[0].rot[i * 3 + 1] * off[1]
                          + (int64_t)poses[0].rot[i * 3 + 2] * off[2];
                v[i] = (int32_t)(s >> 12) + poses[0].trans[i];
            }
            rv = 0;
        }
    }
    re15_skel_bone_to_world(v, e->rot_y, e->x, e->y, e->z, out);
    return rv;
}

/* 0x801108cc + 0x80110988: Versatz -> Weltlage, a0 = 0x0f000000 | sub<<16 | skala, jal 0x80019700. */
static int spawn(re15_actor_t *e, const re15_wasser20b0_r41_spawn_t *s, uint8_t sub, int grid)
{
    int32_t off[3] = { s->x, s->y, s->z }, w[3];
    (void)re15_wasser20b0_r41_anker(e, off, w);
    s_mess.letzter_anker[0] = w[0]; s_mess.letzter_anker[1] = w[1]; s_mess.letzter_anker[2] = w[2];
    s_mess.letzter_code = ((uint32_t)RE15_WASSER20B0_R41_EFFEKT << 24) | ((uint32_t)sub << 16) | s->skala;
    s_mess.letztes_a1   = s->a1;
    if (grid == 0) s_mess.spawns_grid0++; else s_mess.spawns_grid1++;
    const re15_esp_t *bank = re15_esp_room_bank();
    if (!bank || re15_esp_find_id(bank, RE15_WASSER20B0_R41_EFFEKT) < 0) { s_mess.ohne_bank++; return 0; }
    /* Boden: FUN_80019e20 klemmt Zeile A=0/B=0 nicht am Boden (wie spinnen_r38: INT32_MAX). */
    int n = re15_esp_fx_spawn_rows(bank, RE15_WASSER20B0_R41_EFFEKT, sub, s->skala, w[0], w[1], w[2],
                                   INT32_MAX, s->a1);
    if (n > 0) s_mess.plaetze += (unsigned)n;
    return n > 0 ? n : 0;
}

int re15_wasser20b0_r41_sub(re15_actor_t *e)
{
    if (!e) return 0;
    int grid = e->sub_state_1;                       /* +0x5 (= grid @0x8010f00c)                 */
    if (grid != 0 && grid != 1) return 0;            /* [2..4] OFFEN (nie gespawnt), [5..7] jr ra */
    int phase = e->sub_state_2;                      /* +0x6                                      */
    int takt  = e->sub_state_3;                      /* +0x1d0 (lbu)                              */
    int takte = re15_wasser20b0_r41_phasen_takte(grid, phase);
    if (takte < 0) { e->sub_state_2 = 0; return 0; } /* Phase ausser Tabelle: nur im Port moeglich */
    int n_s = 0, spawned = 0;
    const re15_wasser20b0_r41_spawn_t *tab = re15_wasser20b0_r41_tabelle(grid, phase, &n_s);
    uint8_t sub = (grid == 0) ? RE15_WASSER20B0_R41_SUB_GRID0 : RE15_WASSER20B0_R41_SUB_GRID1;
    for (int i = 0; i < n_s; i++)
        if (tab[i].takt == takt) spawned += spawn(e, &tab[i], sub, grid);
    /* Takt++ (`addiu v1,v1,1` / `sb v1,464(v0)` @0x8010f268-6c), Ende -> Takt 0, Phase++ bzw. 0. */
    takt++;
    if (takt >= takte) {
        takt = 0;
        int letzte = (grid == 0) ? 1 : 3;            /* @0x8010f3e8 `sb zero,6` / @0x8010f904     */
        phase = (phase >= letzte) ? 0 : phase + 1;   /* @0x8010f2ac / @0x8010f638 / @0x8010f6a4 / @0x8010f8a4 */
    }
    e->sub_state_3 = (uint8_t)takt;
    e->sub_state_2 = (uint8_t)phase;
    return spawned;
}

int re15_wasser20b0_r41_auto_aot(unsigned room_id, uint8_t aot_type)
{
    /* FUN_800436a8 @0x8001ce1c: AUTO-Pass ohne Gate. Begrenzt auf die Flut-Raeume (Spur A); die
     * allgemeine Aufhebung der Port-Sperre in_cinematic bleibt OFFEN (Dossier). */
    return (room_id == 0x20B0u || room_id == 0x20B1u) && aot_type == RE15_AOT_TYPE_AUTO_EVENT;
}
