/* probe_r16_sk_arme_1210_re2.c — GEGEN-SONDE des Skeptikers (kein Test, kein add_test),
 * Runde 16, Thema "arme-1210-re2". Prueft die Dossier-Behauptungen, die sich NICHT aus dem
 * mitgelieferten Sondenlog belegen lassen, weil dort die EM01A-Bank in den Bahnen fehlt
 * (re15_enemy_reset() vor jeder Bahn -> Bone1-Welt faellt auf den Ursprung zurueck, y=0;
 * der EM01A-Reichweitenblock am Ende fehlt ganz):
 *   T1  Welt-y von EM01A-Bone 0..3 beim Spawn (Dossier: "Bone 1 bei -2513")
 *   T2  EM01A: groesste Bone-Auslenkung (Dossier: 142) und Mesh-Reichweite (Port: 1671/1530)
 *   T3  Westwand hin UND ZURUECK: sind die Arme 1-4 "konstruktionsbedingt tot"?
 *   T4  Spieler PARKT an der Westwand auf Arm-5-Hoehe: kommt ein Griff zustande?
 *   T5  Spieler parkt in der Flurmitte auf Arm-5-Hoehe: passiert etwas?
 * Dossier: analysis/befunde_2026-09-19/arme-1210-re2.md
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_math.h"
#include "re15_ai_flavor.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t            s_blob[0x80000];
static uint8_t            s_blob_donor[0x80000];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank(uint8_t type, uint8_t *blob, size_t blobcap)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= blobcap) {
        memcpy(blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok   = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
                ok = 1;
            }
        }
    }
    free(ems);
    return ok;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static const char *sub_name(int s)
{
    switch (s) { case 0: return "RUHE"; case 1: return "LUNGE"; case 2: return "GREIFEN";
                 case 3: return "ZURUECK"; case 4: return "HALTEN"; case 5: return "ABWERFEN";
                 case 6: return "ABTAUCHEN"; case 7: return "VERGRABEN"; default: return "?"; }
}

/* Raum frisch aufsetzen, Baenke NACH dem Reset laden (anders als die Dossier-Sonde). */
static re15_actor_t *room_setup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    int armbank = load_bank(0x1A, s_blob, sizeof s_blob);
    int donor = load_bank(0x10, s_blob_donor, sizeof s_blob_donor);
    re15_enemy_bank_t *db = re15_enemy_find(0x10);
    if (donor && db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
    (void)armbank;
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();
    return pl;
}

static int collect(int *slots)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x1A) continue;
        slots[n++] = s;
    }
    return n;
}

/* Eine Bahn: Spieler faehrt je Bild um (dx,dz) mit Constrain, Ziel-x lane_x; Schrittfolge aus
 * einer Liste von (z_ziel, x_ziel) Etappen; nach Erreichen der letzten Etappe optional parken. */
typedef struct { int32_t zx; int32_t zz; int park; } etappe_t;

static void run_lane(const char *name, const etappe_t *et, int net, int maxf)
{
    printf("\n--- %s ---\n", name);
    re15_actor_t *pl = room_setup();
    int slots[RE15_ACTOR_MAX]; int n = collect(slots);
    int32_t ax[RE15_ACTOR_MAX], az[RE15_ACTOR_MAX];
    for (int i = 0; i < n; i++) { ax[i] = g_actors[slots[i]].x; az[i] = g_actors[slots[i]].z; }
    int32_t px = -19500, pz = -3500;
    int last_sub[RE15_ACTOR_MAX]; memset(last_sub, -1, sizeof last_sub);
    int active_frames[RE15_ACTOR_MAX]; memset(active_frames, 0, sizeof active_frames);
    int grabbed_prev = 0, grab_events = 0, grab_frames = 0, sub02_f = -1, e_i = 0;
    int32_t maxdrift[RE15_ACTOR_MAX]; memset(maxdrift, 0, sizeof maxdrift);
    for (int f = 0; f < maxf; f++) {
        re15_collision_set_band(0);
        if (!re15_player_is_grabbed()) {
            const etappe_t *cur = &et[e_i];
            int32_t nx = px, nz = pz;
            if (!cur->park) {
                nx = (px < cur->zx) ? px + 75 : (px > cur->zx) ? px - 75 : px;
                if ((px < cur->zx && nx > cur->zx) || (px > cur->zx && nx < cur->zx)) nx = cur->zx;
                nz = (pz < cur->zz) ? pz + 75 : (pz > cur->zz) ? pz - 75 : pz;
                if ((pz < cur->zz && nz > cur->zz) || (pz > cur->zz && nz < cur->zz)) nz = cur->zz;
            }
            re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
            /* Etappe erreicht, wenn z am Ziel ist (x kann an der Wand klemmen). */
            if (!cur->park && nz == cur->zz && e_i + 1 < net) {
                e_i++;
                printf("  f%3d Etappe %d erreicht pl=(%ld,%ld)\n", f, e_i, (long)nx, (long)nz);
            }
            px = nx; pz = nz; pl->x = px; pl->z = pz;
        } else { px = pl->x; pz = pl->z; }
        pl->hp = 100;
        frame_step();
        if (sub02_f < 0)
            for (int i = 0; i < n; i++)
                if ((g_actors[slots[i]].grid_id & 0x1f) == 1) {
                    sub02_f = f; printf("  f%3d pl=(%ld,%ld) sub02: grid_id=1\n", f, (long)px, (long)pz); break; }
        int g = re15_player_is_grabbed();
        if (g && !grabbed_prev) {
            grab_events++;
            printf("  f%3d GRIFF beginnt: pl=(%ld,%ld) victim_state=%d victim_type=0x%02x\n", f,
                   (long)pl->x, (long)pl->z, re15_player_victim_state(), re15_player_victim_type());
        }
        if (!g && grabbed_prev)
            printf("  f%3d GRIFF endet:   pl=(%ld,%ld)\n", f, (long)pl->x, (long)pl->z);
        if (g) grab_frames++;
        grabbed_prev = g;
        for (int i = 0; i < n; i++) {
            re15_actor_t *e = &g_actors[slots[i]];
            if (e->sub_state_1 != 0) active_frames[i]++;
            {   int32_t d = labs(e->x - ax[i]); if (d > maxdrift[i]) maxdrift[i] = d; }
            if (e->sub_state_1 != last_sub[i]) {
                int32_t b1[3]; re15_enemy_bone_world_pos(e, 1, b1);
                int32_t dx = pl->x - ax[i], dz = pl->z - az[i];
                printf("  f%3d arm%2d sub=%d(%-8s) ph=%d clip=%d pos=(%ld,%ld) Bone1-Welt=(%ld,%ld,%ld) pl=(%ld,%ld) d(Heimat)=%ld\n",
                       f, slots[i], e->sub_state_1, sub_name(e->sub_state_1), e->sub_state_2, e->motion,
                       (long)e->x, (long)e->z, (long)b1[0], (long)b1[1], (long)b1[2],
                       (long)pl->x, (long)pl->z,
                       (long)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz)));
                last_sub[i] = e->sub_state_1;
            }
        }
    }
    printf("  Zusammenfassung: Griff-Ereignisse=%d, Bilder im Griff=%d, Ende pl=(%ld,%ld)\n",
           grab_events, grab_frames, (long)pl->x, (long)pl->z);
    printf("  slot aktive-Bilder max-x-Drift\n");
    for (int i = 0; i < n; i++)
        printf("  %2d   %4d   %5ld\n", slots[i], active_frames[i], (long)maxdrift[i]);
}

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    printf("=== probe_r16_sk_arme_1210_re2 (Skeptiker) Flavor=%d ===\n", (int)re15_ai_flavor());

    /* ---------------- T1: Bone-Welt-y beim Spawn, Bank GELADEN ---------------- */
    printf("\n--- T1: EM01A Bone-Weltpunkte beim Spawn (Bank geladen) ---\n");
    room_setup();
    int slots[RE15_ACTOR_MAX]; int n = collect(slots);
    re15_enemy_bank_t *ab = re15_enemy_find(0x1A);
    printf("  Bank 0x1A ok=%d bones=%d meshes=%d clips=%d\n", ab ? ab->ok : -1,
           ab ? ab->skel.bone_count : -1, ab ? ab->md1.mesh_count : -1, ab ? ab->anim.clip_count : -1);
    for (int i = 0; i < n && i < 10; i++) {
        re15_actor_t *e = &g_actors[slots[i]];
        printf("  arm%2d actor=(%ld,%ld,%ld) yaw=%d motion=%d frame=%d:", slots[i],
               (long)e->x, (long)e->y, (long)e->z, (int)e->rot_y, e->motion, (int)e->anim_frame);
        for (int b = 0; b < 4; b++) {
            int32_t w[3]; re15_enemy_bone_world_pos(e, b, w);
            printf(" B%d=(%ld,%ld,%ld)", b, (long)w[0], (long)w[1], (long)w[2]);
        }
        printf("\n");
    }

    /* ---------------- T2: EM01A Bone- und Mesh-Reichweite je Clip ---------------- */
    printf("\n--- T2: EM01A Reichweite (Modellraum, x = vorwaerts) ---\n");
    if (ab && ab->ok) {
        g_anim_pose_actor = NULL;
        static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        printf("  Bone-Eltern:");
        for (int b = 0; b < ab->skel.bone_count; b++)
            printf(" %d:%d(mesh %d)", b, ab->skel.bone_parent[b], ab->skel.bone_mesh_index[b]);
        printf("\n");
        for (int c = 0; c < ab->anim.clip_count; c++) {
            int32_t bmax = -99999, mmax = -99999, mmin = 99999, ymin = 99999, ymax = -99999;
            int bf = 0, mf = 0;
            for (int fr = 0; fr < ab->anim.clips[c].frame_count; fr++) {
                int kf = (int)(re15_emd_get_frame_entry(&ab->anim, c, fr) & 0xfffu);
                if (re15_skel_compute_pose(&ab->skel, kf, poses) != 0) continue;
                for (int b = 0; b < ab->skel.bone_count; b++) {
                    if (poses[b].trans[0] > bmax) { bmax = poses[b].trans[0]; bf = fr; }
                    int m = ab->skel.bone_mesh_index[b];
                    if (m < 0 || m >= ab->md1.mesh_count) continue;
                    const re15_md1_mesh_t *mm = &ab->md1.meshes[m];
                    for (int pass = 0; pass < 2; pass++) {
                        const re15_md1_vertex_t *vv = pass ? mm->quad_vertices : mm->tri_vertices;
                        int cnt = pass ? mm->quad_vertex_count : mm->tri_vertex_count;
                        for (int v = 0; v < cnt; v++) {
                            int64_t vx = vv[v].x, vy = vv[v].y, vz = vv[v].z;
                            int32_t wx = (int32_t)((poses[b].rot[0]*vx + poses[b].rot[1]*vy + poses[b].rot[2]*vz) >> 12) + poses[b].trans[0];
                            int32_t wy = (int32_t)((poses[b].rot[3]*vx + poses[b].rot[4]*vy + poses[b].rot[5]*vz) >> 12) + poses[b].trans[1];
                            if (wx > mmax) { mmax = wx; mf = fr; }
                            if (wx < mmin) mmin = wx;
                            if (wy < ymin) ymin = wy;
                            if (wy > ymax) ymax = wy;
                        }
                    }
                }
            }
            printf("  clip %d (%d Bilder): max Bone-x %ld (Bild %d) | Mesh-x %ld..%ld (max bei Bild %d) | Mesh-y %ld..%ld\n",
                   c, ab->anim.clips[c].frame_count, (long)bmax, bf, (long)mmin, (long)mmax, mf, (long)ymin, (long)ymax);
        }
    }

    /* ---------------- T3: Westwand hin und zurueck ---------------- */
    {
        static const etappe_t et[] = { { -23000, -14000, 0 }, { -23000, -3500, 0 }, { -23000, -3500, 1 } };
        run_lane("T3: Westwand suedwaerts bis z=-14000 (sub02), dann ZURUECK nach z=-3500", et, 3, 420);
    }
    /* ---------------- T4: an der Westwand auf Arm-5-Hoehe parken ---------------- */
    {
        static const etappe_t et[] = { { -23000, -14000, 0 }, { -23000, -15747, 0 }, { -23000, -15747, 1 } };
        run_lane("T4: Westwand bis z=-14000, dann auf z=-15747 (Arm 5) PARKEN", et, 3, 420);
    }
    /* ---------------- T5: Flurmitte auf Arm-5-Hoehe parken ---------------- */
    {
        static const etappe_t et[] = { { -19393, -14000, 0 }, { -19393, -15747, 0 }, { -19393, -15747, 1 } };
        run_lane("T5: Flurmitte bis z=-14000, dann auf z=-15747 PARKEN", et, 3, 420);
    }
    printf("\nOK\n");
    return 0;
}
