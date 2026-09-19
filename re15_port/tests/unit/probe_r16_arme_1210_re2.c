/* probe_r16_arme_1210_re2.c — MESSSONDE (kein Test, kein add_test), Runde 16, Thema
 * "arme-1210-re2": Was tun die Gitterhaende in ROOM1210 heute (Port-Ist), Bild fuer Bild,
 * waehrend der Spieler den Flur entlanglaeuft — und wie ist die Geometrie des RE2-Vorbilds
 * EM2D (CDEMD0.EMS kind 0x2D, ROOM2050) im Vergleich?
 *
 * Teil A  Port-Ist ROOM1210 (RE2-Flavor = Nutzer-Default): Bahn ueber re15_collision_constrain,
 *         pro Bild alle 10 Arme: state/sub1/sub2/motion/frame/x/z, Hand-Weltpunkt (Bone 3),
 *         Sichtbarkeit im aktiven Cut-Viereck, Spieler-Griff-Ereignisse.
 * Teil B  Geometrie ROOM1210: begehbare Flurkante je Arm-z, Abstand Ursprung/Lunge-Ende/Hand.
 * Teil C  RE2 EM2D-Bank: Clips/Bones, Hand-Reichweite (Bone 3 bzw. 10) je Clip/Frame relativ
 *         zum Ursprung — in Modellkoordinaten (Y PSX-abwaerts).
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
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
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

/* Push-out-Grenze: von der Flurmitte x0 in Richtung dir (25er-Schritte) bis der Constrain sperrt. */
static int32_t walk_edge(int32_t x0, int32_t z, int dir)
{
    int32_t cx = x0, cz = z;
    for (int k = 0; k < 800; k++) {
        re15_collision_set_band(0);
        int32_t nx = cx + dir * 25, nz = cz;
        re15_collision_constrain(&s_rdt, cx, cz, &nx, &nz);
        if (nx == cx) break;
        cx = nx; cz = nz;
    }
    return cx;
}

static const char *sub_name(int s)
{
    switch (s) { case 0: return "RUHE"; case 1: return "LUNGE"; case 2: return "GREIFEN";
                 case 3: return "ZURUECK"; case 4: return "HALTEN"; case 5: return "ABWERFEN";
                 case 6: return "ABTAUCHEN"; case 7: return "VERGRABEN"; default: return "?"; }
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
    printf("=== probe_r16_arme_1210_re2 — Port-Ist der Gitterhaende, Flavor=%d ===\n", (int)re15_ai_flavor());

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
    re15_enemy_bank_t *ab = re15_enemy_find(0x1A);
    printf("  EM01A-Bank geladen=%d: meshes=%d bones=%d clips=%d victim_ok=%d | Donor 0x10 victim_ok=%d\n",
           armbank, ab ? ab->md1.mesh_count : -1, ab ? ab->skel.bone_count : -1,
           ab ? ab->anim.clip_count : -1, ab ? ab->victim_ok : -1, (donor && db) ? db->victim_ok : -1);
    if (ab && ab->ok) {
        for (int c = 0; c < ab->anim.clip_count; c++)
            printf("    EM01A clip %d: %d Bilder\n", c, ab->anim.clips[c].frame_count);
    }

    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();

    int slots[RE15_ACTOR_MAX], n = 0;
    int32_t ax[RE15_ACTOR_MAX], az[RE15_ACTOR_MAX];
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x1A) continue;
        slots[n] = s; ax[n] = e->x; az[n] = e->z; n++;
    }
    printf("\n--- Teil B: Geometrie ROOM1210 (%d Arme) ---\n", n);
    printf("  slot  Spawn(x,z)         yaw   Flurkante(x) Abstand Ursprung->Kante  Lunge-Ende->Kante  Hand(+1671)->Kante  HitR\n");
    for (int i = 0; i < n; i++) {
        re15_actor_t *e = &g_actors[slots[i]];
        int dir = (ax[i] > -19393) ? 1 : -1;
        int32_t edge = walk_edge(-19393, az[i], dir);
        int32_t hc = re15_cos_q12((int)(e->rot_y & 0xfff));
        int32_t lx = ax[i] + (int32_t)((hc * 2420) >> 12);
        int32_t hx = ax[i] + (int32_t)((hc * (2420 + 1671)) >> 12);
        printf("  %2d    (%6ld,%6ld)  %4d   %7ld      %6ld                 %6ld            %6ld            %u\n",
               slots[i], (long)ax[i], (long)az[i], (int)e->rot_y, (long)edge,
               (long)labs(ax[i] - edge), (long)labs(lx - edge), (long)labs(hx - edge),
               (unsigned)e->hit_radius_min);
    }
    printf("  SCA-Zellen (Band 0):\n");
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &s_rdt.sca[i];
        printf("    [%2d] x %7ld..%7ld  z %7ld..%7ld  type %u\n", i, (long)c->x,
               (long)((int32_t)c->x + c->width), (long)c->z, (long)((int32_t)c->z + c->density), c->type);
    }

    /* ---------------- Teil A: der Lauf (drei Bahnen: Mitte / Westwand / Ostwand) ---------------- */
    for (int lane = 0; lane < 3; lane++) {
    const int32_t lane_x = (lane == 0) ? -19500 : (lane == 1) ? -23000 : -16000;
    printf("\n--- Teil A%d: Lauf durch den Flur, Ziel-x %ld (%s), z -3500 -> -25000, 75/Bild, Constrain ---\n",
           lane, (long)lane_x, lane == 0 ? "Flurmitte" : lane == 1 ? "an der WESTWAND" : "an der OSTWAND");
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    if (donor && db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();
    n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x1A) continue;
        slots[n] = s; ax[n] = e->x; az[n] = e->z; n++;
    }
    int32_t px = -19500, pz = getenv("START_Z") ? (int32_t)atoi(getenv("START_Z")) : -3500;
    int last_sub[RE15_ACTOR_MAX]; memset(last_sub, -1, sizeof last_sub);
    int last_motion[RE15_ACTOR_MAX]; memset(last_motion, -1, sizeof last_motion);
    int grabbed_prev = 0, grab_events = 0, grab_frames = 0;
    int sub02_f = -1, last_cut = -1;
    int invisible_frames[RE15_ACTOR_MAX]; memset(invisible_frames, 0, sizeof invisible_frames);
    int active_frames[RE15_ACTOR_MAX]; memset(active_frames, 0, sizeof active_frames);
    int32_t maxdrift[RE15_ACTOR_MAX]; memset(maxdrift, 0, sizeof maxdrift);
    for (int f = 0; f < 520; f++) {
        re15_collision_set_band(0);
        if (!re15_player_is_grabbed()) {
            int32_t nx = (px < lane_x) ? px + 75 : (px > lane_x) ? px - 75 : px, nz = pz - 75;
            if (nx < lane_x && px < lane_x && nx > lane_x) nx = lane_x;
            re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
            px = nx; pz = nz;
            pl->x = px; pl->z = pz;
        } else { px = pl->x; pz = pl->z; }
        pl->hp = 100;
        frame_step();

        int cut = (int)g_scd.cam_id;
        int16_t rx[4], rz[4];
        int has = re15_rdt_get_region_quad(&s_rdt, cut, rx, rz);
        if (cut != last_cut) {
            printf("  f%3d pl=(%6ld,%6ld) CUT %d (Viereck %s)\n", f, (long)px, (long)pz, cut, has ? "ja" : "nein");
            last_cut = cut;
        }
        if (sub02_f < 0)
            for (int i = 0; i < n; i++)
                if ((g_actors[slots[i]].grid_id & 0x1f) == 1) { sub02_f = f; printf("  f%3d pl=(%6ld,%6ld) sub02: grid_id=1 auf allen Armen\n", f, (long)px, (long)pz); break; }
        int g = re15_player_is_grabbed();
        if (g && !grabbed_prev) {
            grab_events++;
            printf("  f%3d GRIFF beginnt: pl=(%6ld,%6ld) victim_state=%d victim_type=0x%02x\n", f,
                   (long)pl->x, (long)pl->z, re15_player_victim_state(), re15_player_victim_type());
        }
        if (!g && grabbed_prev)
            printf("  f%3d GRIFF endet:   pl=(%6ld,%6ld) hp=%d\n", f, (long)pl->x, (long)pl->z, pl->hp);
        if (g) grab_frames++;
        grabbed_prev = g;

        for (int i = 0; i < n; i++) {
            re15_actor_t *e = &g_actors[slots[i]];
            int32_t hp3[3] = {0,0,0};
            re15_enemy_bone_world_pos(e, 1, hp3);
            int origin_in = has ? re15_aot_point_in_quad(e->x, e->z, rx, rz) : 1;
            int hand_in   = has ? re15_aot_point_in_quad(hp3[0], hp3[2], rx, rz) : 1;
            if (e->sub_state_1 != 0) active_frames[i]++;
            if (e->sub_state_1 != 0 && !origin_in && !hand_in) invisible_frames[i]++;
            {   int32_t d = labs(e->x - ax[i]); if (d > maxdrift[i]) maxdrift[i] = d; }
            if (e->sub_state_1 != last_sub[i] || e->state != 1 || e->motion != last_motion[i]) {
                int32_t dx = pl->x - e->x, dz = pl->z - e->z;
                printf("  f%3d arm%2d st=%d sub=%d(%-9s) ph=%d clip=%d fr=%3d pos=(%6ld,%6ld) drift=%5ld  Bone1-Welt=(%6ld,%6ld,%6ld) origin%s hand%s  pl=(%6ld,%6ld) d=%ld\n",
                       f, slots[i], e->state, e->sub_state_1, sub_name(e->sub_state_1), e->sub_state_2,
                       e->motion, (int)e->anim_frame, (long)e->x, (long)e->z, (long)(e->x - ax[i]),
                       (long)hp3[0], (long)hp3[1], (long)hp3[2],
                       origin_in ? "+" : "-", hand_in ? "+" : "-",
                       (long)pl->x, (long)pl->z,
                       (long)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz)));
                last_sub[i] = e->sub_state_1; last_motion[i] = e->motion;
            }
        }
        if (pz < -25500) break;
    }
    printf("\n  Zusammenfassung Lauf: Griff-Ereignisse=%d, Bilder im Griff=%d\n", grab_events, grab_frames);
    printf("  slot  aktive Bilder  davon Ursprung+Hand ausserhalb Cut-Viereck  max. x-Drift\n");
    for (int i = 0; i < n; i++)
        printf("  %2d    %4d           %4d                                       %5ld\n",
               slots[i], active_frames[i], invisible_frames[i], (long)maxdrift[i]);
    }

    /* ---------------- Teil C: RE2 EM2D ---------------- */
    printf("\n--- Teil C: RE2 EM2D (CDEMD0.EMS kind 0x2D) ---\n");
    {
        size_t en = 0;
        uint8_t *ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &en);
        if (!ems) { printf("  RE2 CDEMD0.EMS fehlt (%s)\n", RE15_ASSET_RE2_DIR); }
        else {
            static re15_enemy_bank_t eb; memset(&eb, 0, sizeof eb);
            re15_tim_t tim = {0};
            int r = re2_ems_load_bank(ems, en, 0x2D, &eb, &tim);
            printf("  re2_ems_load_bank(0x2D)=%d meshes=%d bones=%d clips(main)=%d loco=%d/%d victim=%d/%d tim=%dx%d\n",
                   r, eb.md1.mesh_count, eb.skel.bone_count, eb.anim.clip_count,
                   eb.loco_ok, eb.anim_loco.clip_count, eb.victim_ok, eb.anim_victim.clip_count,
                   tim.width, tim.height);
            if (r == 0) {
                for (int c = 0; c < eb.anim.clip_count; c++)
                    printf("    EM2D clip %d: %d Bilder\n", c, eb.anim.clips[c].frame_count);
                for (int c = 0; c < eb.anim_victim.clip_count; c++)
                    printf("    EM2D Opfer-Clip %d: %d Bilder\n", c, eb.anim_victim.clips[c].frame_count);
                printf("  Bone-Eltern:");
                for (int b = 0; b < eb.skel.bone_count; b++) printf(" %d:%d", b, eb.skel.bone_parent[b]);
                printf("\n");
                /* Hand-Reichweite: Bone 3 (Variante 0) / Bone 10 (Variante 1) und die
                 * Fingerspitzen-Bones 7 / 14, Modellraum (x vor, y abwaerts). */
                g_anim_pose_actor = NULL;
                static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
                for (int c = 0; c < eb.anim.clip_count; c++) {
                    int fc = eb.anim.clips[c].frame_count;
                    int32_t maxx3 = -99999, maxx7 = -99999, minx7 = 99999, y7 = 0, maxx14 = -99999;
                    int argf = 0;
                    for (int fr = 0; fr < fc; fr++) {
                        int kf = (int)(re15_emd_get_frame_entry(&eb.anim, c, fr) & 0xfffu);
                        if (re15_skel_compute_pose(&eb.skel, kf, poses) != 0) continue;
                        if (poses[3].trans[0] > maxx3) maxx3 = poses[3].trans[0];
                        if (poses[7].trans[0] > maxx7) { maxx7 = poses[7].trans[0]; argf = fr; y7 = poses[7].trans[1]; }
                        if (poses[7].trans[0] < minx7) minx7 = poses[7].trans[0];
                        if (poses[14].trans[0] > maxx14) maxx14 = poses[14].trans[0];
                    }
                    int kf0 = (int)(re15_emd_get_frame_entry(&eb.anim, c, 0) & 0xfffu);
                    re15_skel_compute_pose(&eb.skel, kf0, poses);
                    printf("    clip %d: Bild0 Bone3=(%ld,%ld,%ld) Bone7=(%ld,%ld,%ld) Bone10=(%ld,%ld,%ld) Bone14=(%ld,%ld,%ld) | max x: Bone3 %ld, Bone7 %ld (Bild %d, y %ld), min x Bone7 %ld, Bone14 %ld\n",
                           c, (long)poses[3].trans[0], (long)poses[3].trans[1], (long)poses[3].trans[2],
                           (long)poses[7].trans[0], (long)poses[7].trans[1], (long)poses[7].trans[2],
                           (long)poses[10].trans[0], (long)poses[10].trans[1], (long)poses[10].trans[2],
                           (long)poses[14].trans[0], (long)poses[14].trans[1], (long)poses[14].trans[2],
                           (long)maxx3, (long)maxx7, argf, (long)y7, (long)minx7, (long)maxx14);
                }
                /* MD1: Ausdehnung der Meshes je Bone (Vertex-Bounds im Bone-Raum). */
                for (int m = 0; m < eb.md1.mesh_count && m < 16; m++) {
                    const re15_md1_mesh_t *mm = &eb.md1.meshes[m];
                    int32_t bx[2] = {99999,-99999}, by[2] = {99999,-99999}, bz[2] = {99999,-99999};
                    for (int v = 0; v < mm->tri_vertex_count; v++) {
                        const re15_md1_vertex_t *vv = &mm->tri_vertices[v];
                        if (vv->x < bx[0]) bx[0] = vv->x; if (vv->x > bx[1]) bx[1] = vv->x;
                        if (vv->y < by[0]) by[0] = vv->y; if (vv->y > by[1]) by[1] = vv->y;
                        if (vv->z < bz[0]) bz[0] = vv->z; if (vv->z > bz[1]) bz[1] = vv->z;
                    }
                    printf("    mesh %2d: %3d tri-verts  x %ld..%ld  y %ld..%ld  z %ld..%ld\n", m,
                           mm->tri_vertex_count, (long)bx[0], (long)bx[1], (long)by[0], (long)by[1], (long)bz[0], (long)bz[1]);
                }
            }
            free(ems);
        }
    }
    /* EM01A zum Vergleich: Hand-Reichweite ueber alle Clips (Bone 3 = letzter Bone). */
    if (ab && ab->ok) {
        g_anim_pose_actor = NULL;
        static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        printf("  EM01A Bone-Eltern:");
        for (int b = 0; b < ab->skel.bone_count; b++) printf(" %d:%d", b, ab->skel.bone_parent[b]);
        printf("\n");
        for (int c = 0; c < ab->anim.clip_count; c++) {
            int32_t mx = -99999; int argf = 0;
            for (int fr = 0; fr < ab->anim.clips[c].frame_count; fr++) {
                int kf = (int)(re15_emd_get_frame_entry(&ab->anim, c, fr) & 0xfffu);
                if (re15_skel_compute_pose(&ab->skel, kf, poses) != 0) continue;
                for (int b = 0; b < ab->skel.bone_count; b++)
                    if (poses[b].trans[0] > mx) { mx = poses[b].trans[0]; argf = fr; }
            }
            printf("    EM01A clip %d: max Bone-x %ld (Bild %d)\n", c, (long)mx, argf);
        }
    }
    printf("\nOK\n");
    return 0;
}
