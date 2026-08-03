/* probe_headlook_10d0.c — DIAGNOSE (kein ctest): Nutzer-Report 2026-08-03:
 * "ROOM10D0 Marvin-Cutscene: Leons Kopf ist komisch verdreht."
 *
 * MESSUNG (Schritt 1 des RE-Gates) fuer analysis/cutscene_headlook.md:
 *   (A) Port-Ist: ROOM10D0 laden, sub21 (Marvin-Szene) feuern, pro Tick Leons
 *       Neck-Zustand dumpen (neck_flags/target/accumulator) + die im Pose-Pfad
 *       tatsaechlich addierten Kopf-Winkel (neck_yaw->vy, neck_pitch->vz).
 *   (B) Gegenprobe: identischer Lauf, aber Port-Neck-Pfad stummgeschaltet
 *       (neck_flags/yaw/pitch vor jeder Pose genullt) -> zeigt, ob der Twist
 *       aus dem Neck-Pfad kommt oder aus den Keyframes/RBJ selbst.
 *   (C) Referenz ROOM1150 sub08 (Irons, gilt als korrekt): gleiche Messung.
 *
 * Original-Sollwerte (zum Vergleich im Log):
 *   Spieler-Klemmen  @0x800319b0/0x800319b8 (LAB_800318f8): yaw +-0x200, pitch +-0x138
 *   Spieler-Steps    @0x800319a4/a8/ac: 0x60/0x60 (nur ohne Plc_neck-Override aktiv)
 *   Plc_neck-Handler @0x80041e98: Ziel-Entity = thread+0x154 (lw v1,0x154(a0) @0x80041e9c),
 *       NICHT fest der Spieler -> Work_set(2,0)+Plc_neck = MARVINS Kopf.
 *   NPC-Neck-Init    @0x8011c790-b0 (FUN_8011c6dc, Typ 0x40 = Marvin): Steps 0x40/0x30,
 *       Klemmen +-0x2c8/+-0x138; +0x1a8=&player @0x8011c738; +0x1b8=0 @0x8011c768.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_player.h"    /* re15_actors_anim_advance */
#include "re15_emd.h"
#include "re15_anim_select.h"
#include "re15_skeleton.h"

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

/* PL00-Skelett + EDD einmal laden (Puffer bleiben resident — skel referenziert sie). */
static re15_emd_animation_t s_anim;
static re15_emd_skeleton_t  s_skel;
static int load_pl00(const char *base)
{
    char p[600]; int ok = 0;
    size_t esz = 0, rsz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", base);
    uint8_t *edd = read_file(p, &esz);
    snprintf(p, sizeof p, "%s/PLD/PL00.EMR", base);
    uint8_t *emr = read_file(p, &rsz);
    if (edd && emr &&
        re15_emd_parse_animation(edd, esz, &s_anim) == 0 &&
        re15_emd_parse_skeleton (emr, rsz, &s_skel) == 0) ok = 1;
    return ok;
}

/* Einen Tick posen wie der Renderer (30Hz-Semantik: anim_frame direkt). */
static void pose_player(re15_actor_t *pl, re15_skel_pose_t poses[RE15_EMD_MAX_BONES])
{
    int kf = re15_compute_actor_kf(&s_anim, &s_skel, pl, -1, pl->anim_frame);
    if (kf < 0) kf = 0;
    g_anim_pose_actor = pl;
    re15_skel_compute_pose(&s_skel, kf, poses);
    g_anim_pose_actor = NULL;
}

/* Kopf-Vorwaertsvektor (Mesh schaut +X bei yaw 0) aus der Bone-8-Weltrotation:
 * fwd = rot8 * (1,0,0); daraus yaw/pitch in 4096er-Einheiten fuer das Log. */
static void head_dir(const re15_skel_pose_t *p8, int *out_dx, int *out_dy, int *out_dz)
{
    *out_dx = (int)p8->rot[0]; /* Spalte 0 = Bild von (1,0,0) */
    *out_dy = (int)p8->rot[3];
    *out_dz = (int)p8->rot[6];
}

static void run_scene(const re15_rdt_t *rdt, uint16_t room_id, int event_id,
                      int32_t px, int32_t pz, int16_t prot,
                      int neck_off, int ticks)
{
    scd_vm_init();
    memset(g_actors, 0, sizeof g_actors);
    g_current_room_id = room_id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = prot;
    /* Spieler-Neck-Defaults (byte-true State-0-INIT LAB_800318f8 — der manuelle memset hier
     * umgeht re15_actor_init, das sie sonst setzt): Bone 8 @0x80031938, Steps 96/96
     * @0x800319a4-ac, Klemmen ±0x200/±0x138 @0x800319b0-c4. Ohne neck_bone laeuft die
     * FSM (skeleton_common) fuer den Slot gar nicht. */
    pl->neck_bone = 8; pl->neck_step_yaw = 96; pl->neck_step_pitch = 96;
    pl->neck_clamp_yaw = 0x200; pl->neck_clamp_pitch = 0x138; pl->neck_target_slot = -1;

    scd_register_room_events(rdt);
    scd_room_reenter((re15_rdt_t *)rdt, 0, 0, 0);
    for (int f = 0; f < 4; f++) { scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

    int marvin = -1;
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type >= 0x40) { marvin = i; break; }
    printf("== room %04X event %d neck_off=%d: NPC-slot=%d (type=0x%02x)\n",
           room_id, event_id, neck_off, marvin,
           marvin >= 0 ? g_actors[marvin].type : 0);

    if (scd_event_fire((uint8_t)event_id) < 0) { printf("  !! event_fire fehlgeschlagen\n"); return; }

    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int16_t last_fl = -1, last_ny = 0, last_np = 0;
    int max_ay = 0, max_ap = 0, over_yaw = 0, over_pit = 0;
    for (int f = 0; f < ticks; f++) {
        scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        if (neck_off) { pl->neck_flags = 0; pl->neck_yaw = 0; pl->neck_pitch = 0; }
        pose_player(pl, poses);

        int ay = pl->neck_yaw < 0 ? -pl->neck_yaw : pl->neck_yaw;
        int ap = pl->neck_pitch < 0 ? -pl->neck_pitch : pl->neck_pitch;
        if (ay > max_ay) max_ay = ay;
        if (ap > max_ap) max_ap = ap;
        if (ay > 0x200) over_yaw++;   /* Spieler-Yaw-Klemme @0x800319b0 */
        if (ap > 0x138) over_pit++;   /* Spieler-Pitch-Klemme @0x800319b8 */

        int chg = (pl->neck_flags != last_fl) ||
                  (pl->neck_yaw - last_ny > 64 || last_ny - pl->neck_yaw > 64) ||
                  (pl->neck_pitch - last_np > 64 || last_np - pl->neck_pitch > 64);
        if (chg) {
            long dx = (long)pl->neck_tx - pl->x, dz = (long)pl->neck_tz - pl->z;
            int fx, fy, fz; head_dir(&poses[8], &fx, &fy, &fz);
            printf("  t=%4d fl=0x%02x tgt=(%d,%d,%d) dXZ=(%ld,%ld) spd=0x%04x  "
                   "acc(yaw=%d,pit=%d)  headfwd=(%d,%d,%d)  pl=(%ld,%ld) rot=%d mv=%d\n",
                   f, pl->neck_flags, pl->neck_tx, pl->neck_ty, pl->neck_tz, dx, dz,
                   (unsigned)(uint16_t)pl->neck_speed,
                   pl->neck_yaw, pl->neck_pitch, fx, fy, fz,
                   (long)pl->x, (long)pl->z, pl->rot_y,
                   marvin >= 0 ? g_actors[marvin].motion : -1);
            last_fl = (int16_t)pl->neck_flags; last_ny = pl->neck_yaw; last_np = pl->neck_pitch;
        }
    }
    printf("  SUMMARY: max|neck_yaw|=%d (Klemme 0x200=512)  max|neck_pitch|=%d (Klemme 0x138=312)\n"
           "           Ticks ueber Yaw-Klemme=%d  ueber Pitch-Klemme=%d\n",
           max_ay, max_ap, over_yaw, over_pit);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];

    if (!load_pl00(base)) { fprintf(stderr, "FAIL: PL00.EDD/EMR\n"); return 1; }
    printf("PL00: %d bones, %d clips, %d keyframes (Kopf=Bone 8, wk+0x1b9 @0x80031938)\n",
           s_skel.bone_count, s_anim.clip_count, s_skel.keyframe_count);

    /* --- ROOM10D0, Marvin-Szene sub21. Spieler in der Trigger-AOT slot 0x12
     * (Aot_set @sub00+0x36: x=4700 z=19400 w=800 h=800, Event 0x15=21). --- */
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10D0.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s\n", path); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse 10D0\n"); return 1; }
    re15_actor_init();
    /* Leon betritt den Raum von Sueden, laeuft in die AOT; rot_y aus Tuer-Spawn-Naehe.
     * (5100,19800) liegt mittig in der Trigger-AOT. */
    run_scene(&rdt, 0x10D0, 21, 5100, 19800, 0, /*neck_off*/0, 900);
    run_scene(&rdt, 0x10D0, 21, 5100, 19800, 0, /*neck_off*/1, 900);
    free(data);

    /* --- Referenz ROOM1150 sub08 (Irons-Kneel, gilt als korrekt):
     * Plc_neck(1,-22000,-720,-26131,...) — Ziel WEIT weg vom Spieler. --- */
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1150.RDT", base);
    data = read_file(path, &size);
    if (data && re15_rdt_parse(data, size, &rdt) == 0) {
        re15_actor_init();
        run_scene(&rdt, 0x1150, 8, -20000, -24000, 0, /*neck_off*/0, 600);
        free(data);
    } else {
        printf("(ROOM1150-Referenz uebersprungen — RDT nicht ladbar)\n");
    }
    return 0;
}
