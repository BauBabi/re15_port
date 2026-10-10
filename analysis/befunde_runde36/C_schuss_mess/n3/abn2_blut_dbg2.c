/* Abnahme 2 Spur C — unabhaengige Blut-Lage-Sonde mit WOERTLICHEM Renderer-Zwilling (nicht im Repo).
 *
 * Unterschied zu tests/unit/probe_r36_blut.c (Bau-Agent) und abn1_blut.c (Abnahme 1):
 *  - Bank wie die exe: RE2-Bank aus shared_assets/RE2/CDEMD0.EMS + HYBRID (re2_hybrid_apply mit der RE1.5-EMD aus
 *    EMD/CDEMD0.EMS, re2_rig = 1, Typ 0x10 Sitz-Import) = main.c pc_enemy_load_re2_kind + pc_enemy_hybrid_re15_models.
 *  - "Der Zeichner" ist main.c 9791-10341 Zeile fuer Zeile (re15_actor_anim_select, Loco-Bank, Kanal-Kette, ncur bei 30 fps,
 *    g_anim_pose_actor = npc und NICHT zurueckgesetzt wie in main.c, Bone-Zahl/Remap-Regeln, gore_resolve,
 *    Zellenarm-Maske, Zeichen-Skip fuer !gore_draw (main.c:10172: ein nicht gezeichnetes Teil bekommt KEINEN
 *    Physikschritt), Unterhaelfte, Ragdoll-Anker, re15_re2z_gore_part_matrix mit Physik), danach frame_count++.
 *  - JEDER Spawn des Mess-Rings wird gegen die im SELBEN Bild gezeichnete Matrix seines Parts gemessen (Spawn im
 *    Spielschritt -> Zeichnung nach dem Schritt; Spawn waehrend des Zeichnens -> Matrix nach dem Physikschritt), dazu
 *    gegen die Zeichnung des Vorbilds (T-1, Original-Zeitpunkt, nur Information).
 * Aufruf: abn2_blut <waffe> <px|pz|nx|nz> <eben|tief> [platz=2] [abstand=1800] [alle=0] [saat=0x1234567] */
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
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_engine.h"
#include "re15_enemy_ai_re2_zellenarm.h"
#include "re2_ems.h"
#include "re15_schuss_r36.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern re15_actor_t g_actors[];
extern re15_enemy_bank_t *re15_enemy_alloc(uint8_t type);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_compute_actor_kf(const re15_emd_animation_t *anim, const re15_emd_skeleton_t *skel,
                                  const re15_actor_t *a, int clip_override, uint32_t cur);
#define MAXB RE15_EMD_MAX_BONES

static re15_rdt_t s_rdt; static re15_camera_view_t s_cam; static re15_game_ctx_t s_ctx;
static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
/* main.c pc_enemy_load_re2_kind + pc_enemy_hybrid_re15_models (ohne TIM-Upload) */
static int s_hybrid_ok[256];
static void load_bank_hybrid(uint8_t type)
{
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
    if (!s_ems2) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) != 0) { eb->type = 0; return; }
    eb->buf = NULL; eb->re2_rig = 1; eb->ok = 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (!s_ems15 || idx < 0 || re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) != 0) return;
    uint8_t *buf = (uint8_t *)malloc(len); memcpy(buf, s_ems15 + off, len);
    static re15_md1_t s_md15; static re15_emd_skeleton_t s_sk15; static re15_emd_animation_t s_an15;
    if (re15_emd_parse_container(buf, len, &s_md15, &s_sk15, &s_an15, NULL) != 0) { free(buf); return; }
    int unm = -1;
    if (re2_hybrid_apply(eb, (int)type, &s_md15, &s_sk15, &unm) != 0) { free(buf); return; }
    eb->buf = buf;
    if (type == 0x10) {
        static re15_emd_skeleton_t s_sk; static re15_emd_animation_t s_an;
        s_sk = s_sk15; s_an = s_an15; re15_re2z_set_re15_pose_bank(&s_sk, &s_an);
    }
    s_hybrid_ok[type] = 1;
}
static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* ---- woertlicher Zwilling main.c 9791-10341 fuer EINEN Aktor ---- */
typedef struct { int ok, n, bank; int32_t R[MAXB][9], T[MAXB][3]; uint8_t gez[MAXB]; uint8_t flieg[MAXB]; } zt_t;
static void zeichne_wie_main(re15_actor_t *npc, zt_t *d)
{
    memset(d, 0, sizeof *d);
    re15_anim_banks_t banks; memset(&banks, 0, sizeof banks);   /* Leon-/W01-/PL00-Baenke: fuer Gegner ohne Wirkung */
    re15_anim_view_t av;
    re15_actor_anim_select(npc, 0, &banks, &av);                 /* main.c:9799 */
    const re15_md1_t *npc_md1 = av.mesh;
    const re15_emd_skeleton_t *npc_skel = av.skel;
    const re15_emd_animation_t *npc_anim = av.anim;
    const int8_t *npc_remap = NULL;
    { re15_enemy_bank_t *hb = re15_enemy_find(npc->type);       /* main.c:9811-9817 */
      if (hb && hb->remap_ok && npc_md1 == &hb->md1) npc_remap = hb->mesh_remap; }
    if (!npc_skel || !npc_anim) return;
    if (re15_actor_uses_loco_bank(npc)) {                        /* main.c:9834-9844 */
        re15_enemy_bank_t *lb = re15_enemy_find(npc->type);
        if (lb && lb->loco_ok && (int)npc->motion < lb->anim_loco.clip_count) {
            npc_skel = &lb->skel_loco; npc_anim = &lb->anim_loco; av.clip_override = (int)npc->motion; d->bank = 1;
        }
    }
    if (npc->state == 4 && npc->sub_state_1 == 0 && !npc->walk_active) {          /* main.c:9853-9925 */
        const re15_emd_skeleton_t *rs = re15_actor_rbj_skel((int)(npc - g_actors));
        const re15_emd_animation_t *ra = re15_actor_rbj_anim((int)(npc - g_actors));
        if (rs && ra && (int)npc->motion < ra->clip_count) { npc_skel = rs; npc_anim = ra; av.clip_override = (int)npc->motion; d->bank = 3; }
    } else if (npc->state == 4 && !npc->walk_active &&
               (npc->sub_state_1 == 1 || npc->sub_state_1 == 7 || npc->sub_state_1 == 8)) {
        re15_enemy_bank_t *lb = re15_enemy_find(npc->type);
        if (lb && lb->loco_ok && (int)npc->motion < lb->anim_loco.clip_count) {
            npc_skel = &lb->skel_loco; npc_anim = &lb->anim_loco; av.clip_override = (int)npc->motion; d->bank = 4; }
    } else if (npc->state == 4 && !npc->walk_active && npc->sub_state_1 == 3) {
        re15_enemy_bank_t *lb = re15_enemy_find(npc->type);
        if (lb && lb->victim_ok && (int)npc->motion < lb->anim_victim.clip_count) {
            npc_skel = &lb->skel_victim; npc_anim = &lb->anim_victim; av.clip_override = (int)npc->motion; d->bank = 5; }
    } else if (re15_actor_uses_own_bank(npc)) {
        re15_enemy_bank_t *lb = re15_enemy_find(npc->type);
        if (lb && lb->own_ok && (int)npc->motion < lb->anim_own.clip_count) {
            npc_skel = &lb->skel_own; npc_anim = &lb->anim_own; av.clip_override = (int)npc->motion; d->bank = 6; }
    } else if (npc->re2z_re15_pose) {
        const re15_emd_skeleton_t *rs15 = re15_re2z_re15_pose_skel();
        const re15_emd_animation_t *ra15 = re15_re2z_re15_pose_anim();
        if (rs15 && ra15 && (int)npc->motion < ra15->clip_count) {
            npc_skel = rs15; npc_anim = ra15; av.clip_override = (int)npc->motion; npc_remap = NULL; d->bank = 2; }
    }
    if (npc_anim->clip_count <= 0 || npc_skel->keyframe_count <= 0) return;  /* main.c:9952 */
    uint32_t ncur = (uint32_t)npc->anim_frame;                                /* main.c:9954 (30 fps) */
    int npc_kf = re15_compute_actor_kf(npc_anim, npc_skel, npc, av.clip_override, ncur);
    static re15_skel_pose_t npc_poses[MAXB];
    g_anim_pose_actor = npc;                                                   /* main.c:9960, NICHT zurueckgesetzt */
    if (re15_skel_compute_pose(npc_skel, npc_kf, npc_poses) != 0) return;
    if (npc->no_draw) return;                                                  /* main.c:10058 */
    int npc_bones = npc_skel->bone_count;                                      /* main.c:10060-10084 */
    if (!npc_remap && npc_md1 && npc_bones > npc_md1->mesh_count) npc_bones = npc_md1->mesh_count;
    if (npc_bones > MAXB) npc_bones = MAXB;
    int npc_zeichen_n = npc_bones;
    if (!npc_remap && npc_md1 && npc_md1->mesh_count > npc_bones) { npc_zeichen_n = npc_md1->mesh_count; if (npc_zeichen_n > MAXB) npc_zeichen_n = MAXB; }
    uint8_t gore_draw[MAXB]; uint32_t gore_tint[MAXB]; uint8_t gore_mesh[MAXB];
    for (int gi = 0; gi < MAXB; gi++) { gore_draw[gi] = 1u; gore_tint[gi] = 0x00808080u; gore_mesh[gi] = (uint8_t)gi; }
    int gore_on = re15_re2z_gore_resolve(npc, npc_skel->bone_parent, npc_bones, gore_draw, gore_tint, gore_mesh);
    const uint16_t arm_hide = re15_re2arm_part_hide_mask(npc);
    int32_t nfs = re15_sin_q12((int)npc->rot_y), nfc = re15_cos_q12((int)npc->rot_y);
    int32_t nyaw[9] = { nfc, 0, nfs, 0, 0x1000, 0, -nfs, 0, nfc };
    if (npc->render_scale_q12) for (int k = 0; k < 9; k++) nyaw[k] = (nyaw[k] * (int32_t)npc->render_scale_q12) >> 12;
    static re15_skel_pose_t s_low[MAXB];
    d->n = npc_zeichen_n;
    for (int nbi = 0; nbi < npc_zeichen_n; nbi++) {
        if (arm_hide & (1u << nbi)) continue;
        if (gore_on && nbi < npc_bones && !gore_draw[nbi]) continue;            /* main.c:10172 */
        const re15_skel_pose_t *np = &npc_poses[nbi < npc_bones ? nbi : 0];
        if (npc->re2z_rag_anchor_on && nbi >= 1 && nbi <= 6 && nbi < npc_bones) {   /* main.c:10198-10217 */
            const re15_emd_skeleton_t *lskel = NULL;
            int lkf = re15_re2z_lower_kf(npc, &lskel);
            if (!lskel) lskel = npc_skel;
            void *sav = g_anim_pose_actor; g_anim_pose_actor = NULL;
            int ok = (re15_skel_compute_pose(lskel, lkf, s_low) == 0);
            g_anim_pose_actor = sav;
            if (ok) np = &s_low[nbi];
        }
        int32_t rot[9], tr[3];
        for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++) {
            int64_t s = 0; for (int k = 0; k < 3; k++) s += (int64_t)nyaw[r*3+k] * (int64_t)np->rot[k*3+c];
            rot[r*3+c] = (int32_t)(s >> 12);
        }
        for (int r = 0; r < 3; r++) { int64_t s = 0; for (int k = 0; k < 3; k++) s += (int64_t)nyaw[r*3+k] * (int64_t)np->trans[k]; tr[r] = (int32_t)(s >> 12); }
        int32_t w[3] = { tr[0] + npc->x, tr[1] + npc->y, tr[2] + npc->z };
        if (npc->re2z_rag_anchor_on) {                                           /* main.c:10316-10329 */
            const re15_skel_pose_t *p0 = &npc_poses[0]; int32_t p0w[3];
            for (int rr = 0; rr < 3; rr++) { int64_t s0 = 0; for (int kk = 0; kk < 3; kk++) s0 += (int64_t)nyaw[rr*3+kk] * (int64_t)p0->trans[kk]; p0w[rr] = (int32_t)(s0 >> 12); }
            p0w[0] += npc->x; p0w[1] += npc->y; p0w[2] += npc->z;
            re15_re2z_ragdoll_part_anchor(npc, nbi, p0w, w);
        }
        if (gore_on) d->flieg[nbi] = (uint8_t)re15_re2z_gore_part_matrix(npc, nbi, (uint32_t)g_engine.frame_count, rot, w);
        memcpy(d->R[nbi], rot, sizeof rot); memcpy(d->T[nbi], w, sizeof w); d->gez[nbi] = 1;
    }
    d->ok = 1;
}
static void appl(const int32_t R[9], const int32_t T[3], const int32_t a[3], int32_t o[3])
{
    for (int r = 0; r < 3; r++) o[r] = T[r] + (int32_t)(((int64_t)R[r*3+0]*a[0] + (int64_t)R[r*3+1]*a[1] + (int64_t)R[r*3+2]*a[2]) >> 12);
}
static int32_t abst(const int32_t a[3], const int32_t b[3])
{
    int64_t dx = a[0]-b[0], dy = a[1]-b[1], dz = a[2]-b[2]; int64_t q = dx*dx+dy*dy+dz*dz; int32_t r = 0;
    while ((int64_t)(r+1)*(r+1) <= q) r++;
    return r;
}

int main(int argc, char **argv)
{
    int waffe = argc > 1 ? atoi(argv[1]) : 3;
    const char *seite = argc > 2 ? argv[2] : "px";
    int tief = argc > 3 && strcmp(argv[3], "tief") == 0;
    int platz = argc > 4 ? atoi(argv[4]) : 2;
    int D = argc > 5 ? atoi(argv[5]) : 1800;
    int alle = argc > 6 ? atoi(argv[6]) : 0;
    unsigned saat = argc > 7 ? (unsigned)strtoul(argv[7], NULL, 0) : 0x1234567u;
    static const int PX[6] = { 0, -800, -1800, -1800, 200, 200 }, PZ[6] = { 0, -20600, -19600, -21600, -21600, -19600 };
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL RDT\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(saat);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) load_bank_hybrid(g_actors[s].type);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_inv_grant(waffe, 250);
    re15_player_set_equipped_weapon(waffe);
    { int es = re15_inv_equipped_slot(); if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    int zs = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].x == PX[platz] && g_actors[s].z == PZ[platz]) zs = s;
    if (zs < 0) { printf("FAIL Platz %d\n", platz); return 1; }
    re15_actor_t *z = &g_actors[zs];
    if (!alle) for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != zs) g_actors[s].active = 0;
    if (!strcmp(seite, "px")) { pl->x = z->x + D; pl->z = z->z; pl->rot_y = 2048; }
    else if (!strcmp(seite, "nx")) { pl->x = z->x - D; pl->z = z->z; pl->rot_y = 0; }
    else if (!strcmp(seite, "nz")) { pl->x = z->x; pl->z = z->z - D; pl->rot_y = 3072; }
    else { pl->x = z->x; pl->z = z->z + D; pl->rot_y = 1024; }
    re15_enemy_bank_t *bk = re15_enemy_find(z->type);
    printf("abn2_blut: waffe %d seite %s %s platz %d D %d alle %d saat 0x%x slot %d typ 0x%02x bank %s hybrid %d remap %d Leon (%d,%d) rot %d\n",
           waffe, seite, tief ? "tief" : "eben", platz, D, alle, saat, zs, z->type, (bk && bk->ok) ? "ok" : "FEHLT",
           s_hybrid_ok[z->type], bk ? bk->remap_ok : -1, pl->x, pl->z, pl->rot_y);
    int autow = (waffe == 12 || waffe == 19);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; frame(RE15_PAD_BIT_R1, 0);
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) { static zt_t tmp; zeichne_wie_main(&g_actors[s], &tmp); }
        g_engine.frame_count++;
    }
    static zt_t prev, cur, tmp;
    memset(&prev, 0, sizeof prev); memset(&cur, 0, sizeof cur);
    int tot_ab = -1, steht = -1, n_spawn = 0, n_ohne = 0, n_z = 0, n_gt40 = 0, maxd = 0, max_ohne = 0, maxd_z = 0, n_ungez = 0;
    int max_t1 = 0;
    for (int f = 0; f < 3000; f++) {
        pl->hp = 100;
        uint16_t cr = RE15_PAD_BIT_R1 | (tief ? RE15_PAD_BIT_DOWN : 0), ed = 0;
        if (steht < 0 && z->state == 1 && z->sub_state_1 != 8) steht = f;
        if (tot_ab < 0 && steht >= 0 && f >= steht + 20) {
            if (autow) { cr |= RE15_PAD_BIT_SQUARE; if (((f - steht) % 30) == 0) ed = RE15_PAD_BIT_SQUARE; }
            else if (((f - steht - 20) % 12) == 0) { cr |= RE15_PAD_BIT_SQUARE; ed = RE15_PAD_BIT_SQUARE; }
        }
        int32_t hp_vor = z->hp; unsigned st_vor = z->state, s1v = z->sub_state_1, s2v = z->sub_state_2;
        int32_t vx = z->x, vz = z->z; int vr = z->rot_y; unsigned vmo = z->motion, vaf = z->anim_frame, vax = z->anchor_x, vaz = z->anchor_z;
        unsigned n0 = re15_schuss_r36_fx_zahl();
        frame(cr, ed);
        unsigned n1 = re15_schuss_r36_fx_zahl();
        if (getenv("DBG_POS") && (n1 != n0 || (f >= atoi(getenv("DBG_POS")) - 2 && f <= atoi(getenv("DBG_POS")) + 1)))
            printf("  POS F%d vor (%d,%d) r%d mo %u af %u anker (%d,%d) | nach (%d,%d) r%d mo %u af %u anker (%d,%d) | spawns %u\n", f, vx, vz, vr, vmo, vaf, (int)vax, (int)vaz,
                   z->x, z->z, z->rot_y, (unsigned)z->motion, (unsigned)z->anim_frame, (int)z->anchor_x, (int)z->anchor_z, n1 - n0);
        /* der Renderer zeichnet ALLE aktiven Aktoren in Slot-Reihenfolge (main.c NPC-Schleife); gemessen wird z */
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!g_actors[s].active) continue;
            if (&g_actors[s] == z) zeichne_wie_main(z, &cur); else zeichne_wie_main(&g_actors[s], &tmp);
        }
        unsigned n2 = re15_schuss_r36_fx_zahl();
        g_engine.frame_count++;                                                  /* main.c:11483 */
        int died = (hp_vor >= 0 && z->hp < 0) || ((st_vor != 3u && st_vor != 7u) && (z->state == 3u || z->state == 7u));
        { static int v0 = -2, v1 = -2; if (v0 == -2) { const char *ev = getenv("ABN2_VERB"); v0 = -1; if (ev) sscanf(ev, "%d-%d", &v0, &v1); }
          if (v0 >= 0 && f >= v0 && f <= v1)
            printf("  V F%-4d st %u/%u/%u mo %d af %d frac %d hp %d @(%d,%d,%d) r%d | pfl0 %04x pfl8 %04x seeded %04x | Leon (%d,%d) hp %d st %u | 10E %04x pf1-7 %04x %04x %04x %04x %04x %04x %04x\n",
                   f, z->state, z->sub_state_1, z->sub_state_2, (int)z->motion, (int)z->anim_frame, (int)z->anim_frac, z->hp, z->x, z->y, z->z, z->rot_y,
                   z->re2z_part_flags[0], z->re2z_part_flags[8], z->re2z_part_seeded, pl->x, pl->z, pl->hp, pl->state,
                   z->re2z_f10e, z->re2z_part_flags[1], z->re2z_part_flags[2], z->re2z_part_flags[3], z->re2z_part_flags[4],
                   z->re2z_part_flags[5], z->re2z_part_flags[6], z->re2z_part_flags[7]); }
        for (unsigned nr = n0; nr < n2; nr++) {
            int part, mit; int32_t fx[3], a3[3], ref[3], ref1[3];
            if (!re15_schuss_r36_fx_holen(nr, &part, fx, a3, &mit)) continue;
            n_spawn++;
            int z_ = (nr >= n1);
            int32_t null3[3] = {0,0,0};
            const int32_t *aa = mit ? a3 : null3;
            int d = -1, d1 = -1;
            if (cur.ok && part >= 0 && part < cur.n && cur.gez[part]) { appl(cur.R[part], cur.T[part], aa, ref); d = abst(fx, ref); }
            if (prev.ok && part >= 0 && part < prev.n && prev.gez[part]) { appl(prev.R[part], prev.T[part], aa, ref1); d1 = abst(fx, ref1); }
            if (d < 0) n_ungez++;
            if (!mit && d > 0 && cur.ok && part >= 0 && part < cur.n && cur.gez[part] && !z_ && !cur.flieg[part]) {
                /* Welt-Versatz-Spawn: ofs = fx - T; in Original-Semantik (a3 im Anker-Rahmen) laege er bei T + R*ofs */
                int32_t ofs[3] = { fx[0] - cur.T[part][0], fx[1] - cur.T[part][1], fx[2] - cur.T[part][2] }, oo[3];
                appl(cur.R[part], cur.T[part], ofs, oo);
                printf("    -> Welt-Versatz (%d,%d,%d) an Part %d; mit a3-Semantik (T + R*ofs) laege der Spawn bei (%d,%d,%d): Abstand %d\n",
                       ofs[0], ofs[1], ofs[2], part, oo[0], oo[1], oo[2], abst(fx, oo));
            }
            if (!mit && d > 0 && cur.ok && part >= 0 && part < cur.n && cur.gez[part] && !z_ && prev.ok && prev.flieg[part]) {
                int32_t ofs[3] = { fx[0] - prev.T[part][0], fx[1] - prev.T[part][1], fx[2] - prev.T[part][2] }, oo[3];
                appl(prev.R[part], prev.T[part], ofs, oo);
                printf("    -> fliegendes Teil: Versatz zur Matrix des Vorbilds (%d,%d,%d); mit a3-Semantik laege er bei (%d,%d,%d): Abstand %d\n",
                       ofs[0], ofs[1], ofs[2], oo[0], oo[1], oo[2], abst(fx, oo));
            }
            if (mit && part == 0 && a3[0] == 0 && a3[1] == -600 && a3[2] == 0) {
                int32_t Rp[9], Tp[3], o2[3]; int rv = re15_schuss_r36_gezeichnet(z, 0, Rp, Tp); re15_schuss_r36_blut_anker(z, 0, a3, o2);
                printf("  DBG F%d rv %d post-Anker T=(%d,%d,%d) R0=(%d,%d,%d) | gezeichnet T=(%d,%d,%d) R0=(%d,%d,%d) | anker(a3) post=(%d,%d,%d) fx=(%d,%d,%d)\n",
                       f, rv, Tp[0],Tp[1],Tp[2], Rp[0],Rp[3],Rp[6], cur.T[0][0],cur.T[0][1],cur.T[0][2], cur.R[0][0],cur.R[0][3],cur.R[0][6], o2[0],o2[1],o2[2], fx[0],fx[1],fx[2]);
                printf("  DBG   mo %d af %d fl %02x st %u/%u/%u walk %d loco %d own %d re15pose %d bank %d frac %d pv %d\n", (int)z->motion, (int)z->anim_frame,
                       (unsigned)z->anim_flags, z->state, z->sub_state_1, z->sub_state_2, (int)z->walk_active, re15_actor_uses_loco_bank(z), re15_actor_uses_own_bank(z),
                       (int)z->re2z_re15_pose, cur.bank, (int)z->anim_frac, (int)z->anim_prev_valid);
            }
            printf("F%-4d %s%s st %u/%u/%u->%u/%u/%u hp %d->%d rag %u bank %d | part %2d %s fx=(%d,%d,%d) a3=%s(%d,%d,%d) | d(gezeichnet T)=%d | d(T-1)=%d\n",
                   f, z_ ? "Z " : "  ", died ? "TOD " : (z->hp < hp_vor ? "TREF" : "    "), st_vor, s1v, s2v, z->state, z->sub_state_1,
                   z->sub_state_2, hp_vor, z->hp, z->re2z_rag_anchor_on, cur.bank, part,
                   (cur.ok && part >= 0 && part < MAXB && cur.flieg[part]) ? "fliegt" : "      ",
                   fx[0], fx[1], fx[2], mit ? "" : "ohne ", a3[0], a3[1], a3[2], d, d1);
            if (!mit || (a3[0] == 0 && a3[1] == 0 && a3[2] == 0)) { n_ohne++; if (d > max_ohne) max_ohne = d; }
            if (z_) { n_z++; if (d > maxd_z) maxd_z = d; }
            if (d > maxd) maxd = d;
            if (d1 > max_t1) max_t1 = d1;
            if (d > 40) n_gt40++;
        }
        prev = cur;
        if (tot_ab < 0 && died) tot_ab = f;
        if (tot_ab >= 0 && f > tot_ab + 150) break;
    }
    printf("ERGEBNIS waffe %d %s %s platz %d alle %d: Tod F%d, Spawns %d (ohne a3/Null %d, beim Zeichnen %d, Part nicht gezeichnet %d) | "
           "max d(T) %d, ohne a3 %d, beim Zeichnen %d, d>40: %d | max d(T-1) %d\n", waffe, seite, tief ? "tief" : "eben", platz, alle,
           tot_ab, n_spawn, n_ohne, n_z, n_ungez, maxd, max_ohne, maxd_z, n_gt40, max_t1);
    return 0;
}
