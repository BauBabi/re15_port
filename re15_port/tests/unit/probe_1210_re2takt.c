/* probe_1210_re2takt.c - MESSUNG (kein Test, kein add_test).
 *
 * Frage: Wann faehrt der ROOM1210-Arm heute aus, gemessen an einem ECHTEN Lauf
 * durch den begehbaren Flur (re15_collision_constrain, NICHT on_floor), und wie
 * verhaelt sich das zu RE2s Fenster-Greifer (Tor 1300 @0x80102f3c / Griff 1200
 * @0x801018f4, kein Ausfahren, Griff im selben Bild)?
 *
 * Gemessen wird pro Arm:
 *   - Bild + |dz| + Hand-Abstand beim Uebergang Ruhe -> AUSFAHREN (sub1 0 -> 1)
 *   - Bild, in dem das Ausfahren fertig ist (sub1 -> 2 = Griff-Tor scharf)
 *   - Bild des Gleichstands (kleinstes |dz|) und der kleinste Hand-Abstand
 *   - Bild des ersten Griffs (sub1 -> 4)
 *   - wie viele Arme gleichzeitig nicht in Ruhe sind
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
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_math.h"

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
static uint8_t            s_blob1a[0x80000];
static uint8_t            s_blob10[0x80000];

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

#define MESH_REACH 1671
#define LUNGE_NET  2420

static int32_t hand_dist_of(const re15_actor_t *e, const re15_actor_t *pl, int32_t extra)
{
    int t = (int)e->rot_y & 0xfff;
    int32_t hx = e->x + (int32_t)(((int64_t)(MESH_REACH + extra) * re15_cos_q12(t)) >> 12);
    int32_t hz = e->z - (int32_t)(((int64_t)(MESH_REACH + extra) * re15_sin_q12(t)) >> 12);
    int32_t dx = pl->x - hx, dz = pl->z - hz;
    return (int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz));
}


/* ------------------------------------------------------------------------------
 * (D) VORSCHLAGS-SIMULATION - beruehrt keinen Produktivcode.
 * Regel wie RE2s verankerter Fenster-Greifer:
 *   Tor    = RADIUS statt Rechteck. Abstand Spieler <-> PROJIZIERTE Hand
 *            (Ursprung + LUNGE_NET 2420 + MESH_REACH 1671 entlang Yaw)
 *            < T_out, plus RE2s Halb-Sektor-Paar (Yaw +-256, Halbwinkel 256).
 *   Griff  = derselbe Sektor + Abstand zur ECHTEN Hand < T_grab, und zwar SCHON
 *            waehrend des Ausfahrens, sobald die Hand draussen ist (Bild 4).
 * T_out / T_grab werden hier durchgesweept.
 * ---------------------------------------------------------------------------- */
static int sector_ok(const re15_actor_t *e, const re15_actor_t *pl, int half)
{
    int bearing = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff;
    int c0 = ((int)e->rot_y + half) & 0xfff, c1 = ((int)e->rot_y - half) & 0xfff;
    unsigned t0 = (unsigned)((bearing - c0 + half) & 0xfff);
    unsigned t1 = (unsigned)((bearing - c1 + half) & 0xfff);
    return (t0 < (unsigned)(half << 1)) || (t1 < (unsigned)(half << 1));
}

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    for (int lane = 0; lane < 3; lane++) {
    for (int gait = 0; gait < 2; gait++) {
        const int step = gait ? 200 : 75;         /* 0x4B @0x80030b20 / 0xC8 @0x80030d58 */
        memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

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
        load_bank(0x1A, s_blob1a, sizeof s_blob1a);
        int d10 = load_bank(0x10, s_blob10, sizeof s_blob10);
        re15_enemy_bank_t *db = re15_enemy_find(0x10);
        if (d10 && db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
        for (int f = 0; f < 8; f++) frame_step();

        int arms[16]; int na = 0;
        for (int s = 1; s < RE15_ACTOR_MAX && na < 16; s++)
            if (g_actors[s].active && g_actors[s].type == 0x1A) arms[na++] = s;

        int32_t zmin = 0x7fffffff, zmax = -0x7fffffff;
        for (int i = 0; i < na; i++) {
            re15_actor_t *e = &g_actors[arms[i]];
            if (e->z < zmin) zmin = e->z;
            if (e->z > zmax) zmax = e->z;
        }

        /* Startpunkt: begehbarer Flur, nur per constrain gesucht. */
        int32_t px = -19400, pz = zmax + 1500;
        if (lane != 1) {
            int32_t dir = (lane == 0) ? -25 : 25;
            for (int k = 0; k < 400; k++) {
                int32_t nx = px + dir, nz = pz;
                re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
                if (nx == px && nz == pz) break;
                px = nx; pz = nz;
            }
        }
        printf("\n===== SPUR %s, TEMPO %d/Bild =====\n",
               lane == 0 ? "West-Wand" : lane == 1 ? "Mitte" : "Ost-Wand", step);
        printf("  Start (%ld,%ld); Arme %d, z-Bereich %ld..%ld\n",
               (long)px, (long)pz, na, (long)zmin, (long)zmax);

        int   f_ext[16], f_ready[16], f_grab[16], f_near[16];
        int32_t dz_ext[16], hd_ext[16], dz_ready[16], hd_min[16], dz_min[16];
        uint8_t prev_sub[16];
        for (int i = 0; i < 16; i++) {
            f_ext[i] = f_ready[i] = f_grab[i] = f_near[i] = -1;
            dz_ext[i] = hd_ext[i] = dz_ready[i] = 0;
            hd_min[i] = 0x7fffffff; dz_min[i] = 0x7fffffff;
            prev_sub[i] = 0;
        }
        int max_busy = 0, busy_frame = -1;
        int frames = 0;
        for (int f = 0; f < 1200; f++) {
            if (!re15_player_is_grabbed()) {
                int32_t nx = px, nz = pz - step;
                re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
                if (nz == pz && nx == px) break;
                px = nx; pz = nz;
                if (lane != 1) {
                    int32_t dir = (lane == 0) ? -25 : 25;
                    for (int k = 0; k < 200; k++) {
                        int32_t ax = px + dir, az = pz;
                        re15_collision_constrain(&s_rdt, px, pz, &ax, &az);
                        if (ax == px && az == pz) break;
                        px = ax; pz = az;
                    }
                }
                pl->x = px; pl->z = pz;
            }
            pl->hp = 100;
            frame_step();
            frames = f;
            int busy = 0;
            for (int i = 0; i < na; i++) {
                re15_actor_t *e = &g_actors[arms[i]];
                int32_t dz = pl->z - e->z; if (dz < 0) dz = -dz;
                int32_t hd = hand_dist_of(e, pl, 0);
                if (e->sub_state_1 != 0) busy++;
                if (prev_sub[i] == 0 && e->sub_state_1 == 1 && f_ext[i] < 0) {
                    f_ext[i] = f; dz_ext[i] = dz; hd_ext[i] = hd;
                }
                if (e->sub_state_1 == 2 && f_ready[i] < 0) { f_ready[i] = f; dz_ready[i] = dz; }
                if (e->sub_state_1 == 4 && f_grab[i] < 0) f_grab[i] = f;
                if (dz < dz_min[i]) { dz_min[i] = dz; f_near[i] = f; }
                if (hd < hd_min[i]) hd_min[i] = hd;
                prev_sub[i] = e->sub_state_1;
            }
            if (busy > max_busy) { max_busy = busy; busy_frame = f; }
            if (pz < zmin - 2500) break;
        }
        printf("  Bilder gelaufen: %d; hoechste Zahl gleichzeitig aktiver Arme: %d (Bild %d)\n",
               frames, max_busy, busy_frame);
        printf("  Arm  (x,z)            yaw | AUSFAHR-Bild |dz| Hand | FERTIG-Bild |dz| (Vorlauf) | Gleichstand | minHand | GRIFF\n");
        for (int i = 0; i < na; i++) {
            re15_actor_t *e = &g_actors[arms[i]];
            printf("  %2d (%7ld,%7ld) %4d | %6d %6ld %6ld | %6d %6ld (%4d B) | %6d | %6ld | %6d\n",
                   arms[i], (long)e->x, (long)e->z, (int)e->rot_y,
                   f_ext[i], (long)dz_ext[i], (long)hd_ext[i],
                   f_ready[i], (long)dz_ready[i],
                   (f_ready[i] >= 0 && f_near[i] >= 0) ? (f_near[i] - f_ready[i]) : -999,
                   f_near[i], (long)hd_min[i], f_grab[i]);
        }
    }
    }

    /* ================= (D) VORSCHLAG SIMULIEREN ================= */
    {
        static const int32_t T_OUT[]  = { 1300, 1600, 2100, 2600, 3200 };
        static const int32_t T_GRAB[] = { 1200, 1300 };
        printf("\n=== (D) Vorschlags-Simulation (Radius + Sektor, Griff schon ab Bild 4) ===\n");
        for (int lane = 0; lane < 3; lane++)
        for (int gait = 0; gait < 2; gait++)
        for (unsigned ti = 0; ti < sizeof T_OUT / sizeof T_OUT[0]; ti++)
        for (unsigned gi = 0; gi < sizeof T_GRAB / sizeof T_GRAB[0]; gi++) {
            const int step = gait ? 200 : 75;
            memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
            s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
            re15_actor_init(); re15_aot_init(); scd_vm_init();
            re15_enemy_reset(); re15_enemy_ai_set_paused(1);
            re15_player_victim_reset();
            re15_damage_seed_rng(0x0badf00du);
            g_current_room_id = 0x1210;
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
            re15_collision_set_band(0);
            scd_register_room_events(&s_rdt);
            scd_room_reenter(&s_rdt, 0, 0, 0);
            for (int f = 0; f < 8; f++) frame_step();
            int arms[16]; int na = 0;
            for (int s = 1; s < RE15_ACTOR_MAX && na < 16; s++)
                if (g_actors[s].active && g_actors[s].type == 0x1A) arms[na++] = s;
            int32_t zmax = -0x7fffffff;
            for (int i = 0; i < na; i++) if (g_actors[arms[i]].z > zmax) zmax = g_actors[arms[i]].z;
            int32_t px = -19400, pz = zmax + 1500;
            if (lane != 1) {
                int32_t dir = (lane == 0) ? -25 : 25;
                for (int k = 0; k < 400; k++) {
                    int32_t nx = px + dir, nz = pz;
                    re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
                    if (nx == px && nz == pz) break;
                    px = nx; pz = nz;
                }
            }
            int st[16], ext_f[16], grabbed[16], nout = 0, maxsim = 0;
            int32_t dz_trig[16];
            for (int i = 0; i < 16; i++) { st[i] = 0; ext_f[i] = 0; grabbed[i] = -1; dz_trig[i] = -1; }
            for (int f = 0; f < 1200; f++) {
                int32_t nx = px, nz = pz - step;
                re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
                if (nz == pz && nx == px) break;
                px = nx; pz = nz;
                if (lane != 1) {
                    int32_t dir = (lane == 0) ? -25 : 25;
                    for (int k = 0; k < 200; k++) {
                        int32_t ax = px + dir, az = pz;
                        re15_collision_constrain(&s_rdt, px, pz, &ax, &az);
                        if (ax == px && az == pz) break;
                        px = ax; pz = az;
                    }
                }
                pl->x = px; pl->z = pz;
                int busy = 0;
                for (int i = 0; i < na; i++) {
                    re15_actor_t *e = &g_actors[arms[i]];
                    int32_t dout = hand_dist_of(e, pl, LUNGE_NET);
                    if (st[i] == 0) {
                        if (dout < T_OUT[ti] && sector_ok(e, pl, 256)) {
                            st[i] = 1; ext_f[i] = 0; nout++;
                            dz_trig[i] = pl->z - e->z; if (dz_trig[i] < 0) dz_trig[i] = -dz_trig[i];
                        }
                    } else {
                        ext_f[i]++;
                        int32_t adv = ext_f[i] <= 3 ? 800 * ext_f[i] : LUNGE_NET;
                        int32_t hd = hand_dist_of(e, pl, adv);
                        if (ext_f[i] >= 4 && hd < T_GRAB[gi] && sector_ok(e, pl, 256) && grabbed[i] < 0)
                            grabbed[i] = f;
                        if (ext_f[i] > 39 && dout > T_OUT[ti] + 400) st[i] = 0;
                        busy++;
                    }
                }
                if (busy > maxsim) maxsim = busy;
            }
            int ng = 0; int32_t dzmax = 0;
            for (int i = 0; i < na; i++) { if (grabbed[i] >= 0) ng++; if (dz_trig[i] > dzmax) dzmax = dz_trig[i]; }
            printf("  Spur %-5s Tempo %3d  T_out %4ld  T_grab %4ld : ausfahrende Arme %2d, max gleichzeitig %d, GRIFFE %d, groesstes |dz| beim Ausloesen %ld\n",
                   lane == 0 ? "West" : lane == 1 ? "Mitte" : "Ost", step,
                   (long)T_OUT[ti], (long)T_GRAB[gi], nout, maxsim, ng, (long)dzmax);
        }
    }

    free(buf);
    printf("\nprobe_1210_re2takt: MESSUNG FERTIG\n");
    return 0;
}
