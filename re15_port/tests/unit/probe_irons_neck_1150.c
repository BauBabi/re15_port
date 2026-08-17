/* probe_irons_neck_1150.c — DIAGNOSE (kein ctest): Nutzer-Report (WIEDERHOLT)
 * "Irons schaut im Original etwas diagonaler in Irons' Office."
 *
 * MESSUNG (Schritt 1 des RE-Gates): ROOM1150 ueber die ECHTE Kette laden
 * (ROOM10D0 -> ROOM1150), Irons (NPC-Typ 0x45, Spawn `44 00 45 00 …` @main00+0x012c)
 * finden und pro Tick seinen Neck-Zustand + die GERENDERTE Kopf-Orientierung
 * (Yaw und Pitch GETRENNT aus der Bone-8-Weltmatrix) dumpen.
 *
 * ORIGINAL-SOLL (alle selbst disassembliert, STAGE1.BIN / PSX.EXE):
 *   Sce_em_set @0x800420a0: `sw zero,4(s0)` @0x800421e0 (state=0) …
 *       `sw s0,-14460(at)` @0x80042578 (DAT_800ac784 = neue Entity)
 *       `lbu v0,8(s0); sll 2; addiu at,at,11180 (=0x80072bac); lw; jalr v0` @0x8004257c-9c
 *       -> der TYP-ROOT laeuft SOFORT beim Spawn, mit state == 0 -> Tabelle[0] = INIT.
 *       Danach `lh v0,16(s2)` @0x800425a4: ROOM1150 hat dort 0x0000 -> KEIN state=4-Override.
 *       `andi v0,0x40` @0x8004260c: entity+9 == 0x00 (Operand pc[3]) -> KEIN +0x1b8=0x12.
 *   Root 0x45 = FUN_8011d140, Tabelle @0x80121738[0] = 0x8011d2b8 (INIT), [4] = 0x80050be8.
 *   INIT 0x8011d2b8: +0x1a8 = &player @0x8011d314 · +0x9a = -1 @0x8011d324 ·
 *       +0x9e = 0x78 @0x8011d334 · +0x1b8 = 0 @0x8011d344 (= ENTITY-TRACKING) ·
 *       +0x1b9 = 8 @0x8011d354 · part8+0x98 = 0x40 @0x8011d36c · +0x9a = 0x30 @0x8011d374 ·
 *       +0x9c = 0x2c8 @0x8011d37c · +0x94/+0x96 = 0 @0x8011d384/88 · +0x9e = 0x138 @0x8011d38c ·
 *       +0x94 (clip) = 2 @0x8011d39c · state = 1 @0x8011d2cc.
 *   ROOM1150 sub02 danach: Work_set(2,0) + Plc_motion(0,3) @0x80041b90 -> state=4 (@0x80041bb0),
 *       +0x94 = 3, +0x5 = 0. Plc_motion fasst +0x1b8/+0x1b9 NICHT an -> die Neck-Konfiguration
 *       des INIT bleibt stehen: Irons TRACKT LEON PERMANENT MIT DEM KOPF.
 *   Der Root ruft die Neck-FSM jeden Tick: `jal 0x80037358` @0x8011d278.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_player.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_anim_select.h"
#include "re15_skeleton.h"
#include "re15_game_step.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static re15_emd_skeleton_t s_sk45; static re15_emd_animation_t s_an45;
static uint8_t s_em45[0x80000];

static int load_em045(const char *base)
{
    char p[600]; size_t esz = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(p, &esz);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x45);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, esz, idx, &off, &len) != 0 || len > sizeof s_em45) {
        free(ems); return 0;
    }
    memcpy(s_em45, ems + off, len);
    free(ems);
    return re15_emd_parse_own_bank(s_em45, len, &s_sk45, &s_an45) == 0;
}

/* Kopf-Orientierung: fwd = rot8 * (1,0,0) (Mesh schaut +X bei yaw 0).
 * yaw   = atan2(dz, dx) in 4096er-Einheiten (0 = +X)
 * pitch = atan2(-dy, sqrt(dx^2+dz^2))  (PSX: -Y = oben) */
static void head_yaw_pitch(const re15_skel_pose_t *p8, int *yaw, int *pitch)
{
    double dx = (double)p8->rot[0], dy = (double)p8->rot[3], dz = (double)p8->rot[6];
    double k = 4096.0 / (2.0 * 3.14159265358979);
    *yaw   = (int)(atan2(dz, dx) * k);
    *pitch = (int)(atan2(-dy, sqrt(dx*dx + dz*dz)) * k);
}

static void pose_actor(re15_actor_t *a, re15_emd_skeleton_t *sk, re15_emd_animation_t *an,
                       re15_skel_pose_t poses[RE15_EMD_MAX_BONES])
{
    int kf = re15_compute_actor_kf(an, sk, a, a->motion, a->anim_frame);
    if (kf < 0) kf = 0;
    g_anim_pose_actor = a;
    re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = NULL;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t size = 0;
    static re15_rdt_t rdt10d0, rdt1150;

    if (!load_em045(base)) { fprintf(stderr, "FAIL: EM045\n"); return 1; }
    printf("EM045 own-Bank: %d clips, %d bones, %d keyframes\n",
           s_an45.clip_count, s_sk45.bone_count, s_sk45.keyframe_count);

    /* --- Kette Teil 1: ROOM10D0 (Marvin) --- */
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10D0.RDT", base);
    uint8_t *d10 = read_file(path, &size);
    if (!d10 || re15_rdt_parse(d10, size, &rdt10d0) != 0) { fprintf(stderr, "FAIL 10D0\n"); return 1; }
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x10D0;
    {
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->hp = 100; pl->x = 5100; pl->y = 0; pl->z = 19800; pl->rot_y = 0;
    }
    scd_register_room_events(&rdt10d0);
    scd_room_reenter(&rdt10d0, 0, 0, 0);
    for (int f = 0; f < 60; f++) { scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }
    scd_event_fire(21);
    for (int f = 0; f < 300; f++) { scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }
    {
        int m = -1;
        for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type >= 0x40) { m = i; break; }
        if (m >= 0)
            printf("[10D0] Marvin slot=%d type=0x%02x neck: bone=%d fl=0x%02x tgt=%d acc=(%d,%d) "
                   "step=(%d,%d) clamp=(0x%x,0x%x) spd=0x%04x\n",
                   m, g_actors[m].type, g_actors[m].neck_bone, g_actors[m].neck_flags,
                   (int)g_actors[m].neck_target_slot, g_actors[m].neck_yaw, g_actors[m].neck_pitch,
                   g_actors[m].neck_step_yaw, g_actors[m].neck_step_pitch,
                   (unsigned)g_actors[m].neck_clamp_yaw, (unsigned)g_actors[m].neck_clamp_pitch,
                   (unsigned)(uint16_t)g_actors[m].neck_speed);
        else printf("[10D0] kein NPC gefunden\n");
    }

    /* --- Kette Teil 2: ROOM1150 (Irons' Office) --- */
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1150.RDT", base);
    uint8_t *d15 = read_file(path, &size);
    if (!d15 || re15_rdt_parse(d15, size, &rdt1150) != 0) { fprintf(stderr, "FAIL 1150\n"); return 1; }
    g_current_room_id = 0x1150;
    re15_actor_init();
    scd_vm_init();
    { extern void scd_register_current_rdt(const re15_rdt_t *); scd_register_current_rdt(&rdt1150); }
    scd_register_room_events(&rdt1150);
    if (rdt1150.main_scd)   scd_thread_start(0, rdt1150.main_scd);
    if (rdt1150.sub_scd[0]) scd_thread_start(1, rdt1150.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    {   /* Leon steht dort, wo sub03 ihn per Pos_set hinsetzt: (0xafc6, 0, 0x9dc5) @sub03+0x0050 */
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->hp = 100;
        pl->x = (int16_t)0xafc6; pl->y = 0; pl->z = (int16_t)0x9dc5; pl->rot_y = 0x05dc;
        pl->neck_bone = 8; pl->neck_step_yaw = 96; pl->neck_step_pitch = 96;
        pl->neck_clamp_yaw = 0x200; pl->neck_clamp_pitch = 0x138; pl->neck_target_slot = -1;
    }

    int ir = -1;
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    /* LANE B (echte Frame-Reihenfolge): main00 spawnt Irons (state 0), im SELBEN Frame laeuft
     * sub00 -> Evt_exec(sub02) -> Work_set(2,0) + Plc_motion(0,3) = state 4 (@0x80041bb0),
     * und ERST DANACH tickt die Entity-Schleife. Im Original ist das egal (Sce_em_set hat den
     * INIT schon inline gefahren, @0x8004259c); im Port entscheidet es alles.
     * RE15_IRONS_LANE=B schaltet diese Reihenfolge scharf. */
    const char *lane = getenv("RE15_IRONS_LANE");
    int lane_b = (lane && (*lane == 'B' || *lane == 'b'));
    if (lane_b) { scd_vm_tick(); scd_event_fire(2); scd_vm_tick(); }
    for (int f = 0; f < 200; f++) {
        scd_vm_tick();
        { re15_game_ctx_t gctx; memset(&gctx, 0, sizeof gctx); gctx.rdt_ok = 0;
          re15_game_step(&gctx); }        /* <- enthaelt re15_npc_neck_spawn_init() */
        re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        if (ir < 0)
            for (int i = 1; i < RE15_ACTOR_MAX; i++)
                if (g_actors[i].active && g_actors[i].type == 0x45) { ir = i; break; }
        if (ir >= 0) {
            re15_actor_t *a = &g_actors[ir];
            pose_actor(a, &s_sk45, &s_an45, poses);
            if (f < 6 || (f % 20) == 0 || f == 199) {
                int hy = 0, hp = 0; head_yaw_pitch(&poses[8], &hy, &hp);
                printf("[1150] t=%3d slot=%d st=%d/%d mo=%d pos=(%ld,%ld,%ld) roty=%d | "
                       "neck bone=%d fl=0x%02x tgt=%d acc=(%d,%d) step=(%d,%d) clamp=(0x%x,0x%x) "
                       "| HEAD yaw=%d pitch=%d\n",
                       f, ir, a->state, a->sub_state_1, a->motion,
                       (long)a->x, (long)a->y, (long)a->z, a->rot_y,
                       a->neck_bone, a->neck_flags, (int)a->neck_target_slot,
                       a->neck_yaw, a->neck_pitch, a->neck_step_yaw, a->neck_step_pitch,
                       (unsigned)a->neck_clamp_yaw, (unsigned)a->neck_clamp_pitch, hy, hp);
            }
        }
    }
    if (ir < 0) { printf("[1150] KEIN Irons (Typ 0x45) gespawnt!\n"); return 1; }

    /* --- SOLL-Rechnung: was WUERDE die FSM liefern, wenn die INIT-Defaults stuenden?
     * (Nur zur Orientierung; die echte Verifikation macht der Pin.) --- */
    {
        re15_actor_t *a = &g_actors[ir];
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        printf("\nGEOMETRIE: Irons(%ld,%ld,%ld) roty=%d  Leon(%ld,%ld,%ld)  d=(%ld,%ld)\n",
               (long)a->x, (long)a->y, (long)a->z, a->rot_y,
               (long)pl->x, (long)pl->y, (long)pl->z,
               (long)(pl->x - a->x), (long)(pl->z - a->z));
    }
    /* --- REGRESSIONS-SWEEP: die anderen NPC-Raeume. Zeigt, ob der Spawn-Neck-INIT dort
     * etwas VERAENDERT (er darf nur greifen, wo der Zustands-Dispatch-INIT nie lief). --- */
    printf("\n== Sweep andere NPC-Raeume ==\n");
    static const char *rooms[] = { "ROOM10D0", "ROOM1170", "ROOM11B0", "ROOM11B1", "ROOM1211" };
    for (unsigned r = 0; r < sizeof rooms / sizeof rooms[0]; r++) {
        snprintf(path, sizeof path, "%s/STAGE1/%s.RDT", base, rooms[r]);
        size_t rs = 0; uint8_t *rd = read_file(path, &rs);
        static re15_rdt_t rr;
        if (!rd || re15_rdt_parse(rd, rs, &rr) != 0) { printf("  %s: nicht ladbar\n", rooms[r]); free(rd); continue; }
        re15_actor_init(); scd_vm_init();
        { extern void scd_register_current_rdt(const re15_rdt_t *); scd_register_current_rdt(&rr); }
        scd_register_room_events(&rr);
        if (rr.main_scd)   scd_thread_start(0, rr.main_scd);
        if (rr.sub_scd[0]) scd_thread_start(1, rr.sub_scd[0]);
        g_scd.work_vars[10] = 0;
        g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1; g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        for (int f = 0; f < 90; f++) {
            scd_vm_tick();
            { re15_game_ctx_t g; memset(&g, 0, sizeof g); g.rdt_ok = 0; re15_game_step(&g); }
            re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        }
        printf("  %s:", rooms[r]);
        int any = 0;
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            re15_actor_t *n = &g_actors[i];
            if (!n->active) continue;
            if (n->type != 0x40 && n->type != 0x42 && n->type != 0x45 && n->type != 0x47 &&
                n->type != 0x49 && n->type != 0x4b && n->type != 0x4d) continue;
            any = 1;
            printf(" [slot%d t=0x%02x grid=0x%02x st=%d/%d bone=%d fl=0x%02x acc=(%d,%d)]",
                   i, n->type, n->grid_id, n->state, n->sub_state_1,
                   n->neck_bone, n->neck_flags, n->neck_yaw, n->neck_pitch);
        }
        if (!any) printf(" (keine NPCs)");
        printf("\n");
        free(rd);
    }

    free(d10); free(d15);
    return 0;
}
