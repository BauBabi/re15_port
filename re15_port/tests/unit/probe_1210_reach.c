/* probe_1210_reach.c - MESSUNG (kein Test, kein add_test):
 * Warum sieht der Nutzer in ROOM1210 keine Haende aus dem Gitter kommen?
 * Gemessen wird:
 *   (1) Arm-Positionen/Yaw + der begehbare Flur (re15_collision_on_floor)
 *   (2) Wohin die ORIGINAL-Translation (FUN_800245d8, +0x8c) den Arm bringt
 *   (3) Traegt EM01A ROOT-TRANSLATION in den Keyframes? (bytes 0..5 und 6..11)
 *   (4) Port-Lauf: Spieler geht den begehbaren Flur ab, pro Bild alle Arme
 *   (5) Wie weit wandert der Hand-Bone in Weltkoordinaten (Clip 0 vs Clip 2)?
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

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
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
    printf("EM01A-Bank: %s\n", load_bank(0x1A, s_blob1a, sizeof s_blob1a) ? "ok" : "FEHLT");
    int d10 = load_bank(0x10, s_blob10, sizeof s_blob10);
    re15_enemy_bank_t *db = re15_enemy_find(0x10);
    if (d10 && db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);

    for (int f = 0; f < 8; f++) frame_step();

    int arms[16]; int na = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && na < 16; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) arms[na++] = s;
    printf("Arme: %d\n", na);

    /* ---------- (1) Arme + Flur ---------- */
    printf("\n=== (1) Arme, Yaw, naechster begehbarer Punkt, Flurband auf ihrer z-Linie ===\n");
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        int32_t lo = 0, hi = 0; int have = 0;
        for (int32_t x = -34000; x <= -6000; x += 25)
            if (re15_collision_on_floor(&s_rdt, x, e->z)) { if (!have) { lo = x; have = 1; } hi = x; }
        int32_t bd = 0x7fffffff, bx = 0, bz = 0;
        for (int32_t x = e->x - 8000; x <= e->x + 8000; x += 50)
            for (int32_t z = e->z - 8000; z <= e->z + 8000; z += 50) {
                if (!re15_collision_on_floor(&s_rdt, x, z)) continue;
                int32_t dx = x - e->x, dz = z - e->z;
                int32_t dd = (int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz));
                if (dd < bd) { bd = dd; bx = x; bz = z; }
            }
        printf("  slot %2d @(%7ld,%7ld) yaw=%4d | Flur auf z: %s x %7ld..%7ld | naechster Boden (%7ld,%7ld) d=%5ld\n",
               arms[i], (long)e->x, (long)e->z, (int)e->rot_y,
               have ? "" : "KEINER", (long)lo, (long)hi, (long)bx, (long)bz, (long)bd);
    }

    /* ---------- (2) ORIGINAL-Translation nachrechnen ---------- */
    /* FUN_800245d8 @0x800245d8: v=(+0x8c,0,0); RotMatrixY(+0x6a + param); pos.x += v'.x ; pos.z += v'.z
     * RotMatrixY(t): x' = x*cos(t) + z*sin(t) ; z' = -x*sin(t) + z*cos(t)   -> mit z=0:
     *   dx =  S*cos(t) ; dz = -S*sin(t)
     * FUN_8010c714: 3 Bilder S=800 (@0x8010c7b4/b8) + 1 Bild S=20 (@0x8010c7f4/f8) vorwaerts,
     *               dann 30 Bilder S=20 rueckwaerts (param=0x800 @0x8010c8b4) + 1 Bild S=200
     *               (@0x8010c85c/60) rueckwaerts, dann 4 Bilder S=200 vorwaerts (param=0 @0x8010c914). */
    printf("\n=== (2) ORIGINAL-Vorwaertsbewegung: wohin bringt +0x8c den Arm? ===\n");
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        int32_t px = e->x, pz = e->z;
        int32_t peakx = px, peakz = pz; int32_t peak_adv = 0;
        struct { int frames; int S; int rev; } seq[] = {
            {3, 800, 0}, {1, 20, 0}, {30, 20, 1}, {1, 200, 1}, {4, 200, 0}
        };
        int32_t adv = 0;
        for (unsigned k = 0; k < sizeof seq / sizeof seq[0]; k++)
            for (int f = 0; f < seq[k].frames; f++) {
                int t = ((int)e->rot_y + (seq[k].rev ? 0x800 : 0)) & 0xfff;
                int32_t dx = (int32_t)(((int64_t)seq[k].S * re15_cos_q12(t)) >> 12);
                int32_t dz = (int32_t)(((int64_t)(-seq[k].S) * re15_sin_q12(t)) >> 12);
                px += dx; pz += dz;
                int32_t ddx = px - e->x, ddz = pz - e->z;
                int32_t da = (int32_t)re15_squareroot0((uint32_t)((int64_t)ddx*ddx + (int64_t)ddz*ddz));
                if (da > peak_adv) { peak_adv = da; peakx = px; peakz = pz; }
                adv = da;
            }
        printf("  slot %2d yaw=%4d: Spitze (%7ld,%7ld) nach %5ld Einheiten -> Boden dort? %s | Endpunkt (%7ld,%7ld) d=%5ld Boden? %s\n",
               arms[i], (int)e->rot_y, (long)peakx, (long)peakz, (long)peak_adv,
               re15_collision_on_floor(&s_rdt, peakx, peakz) ? "JA" : "nein",
               (long)px, (long)pz, (long)adv,
               re15_collision_on_floor(&s_rdt, px, pz) ? "JA" : "nein");
    }

    /* ---------- (3) Root-Translation in den EM01A-Keyframes? ---------- */
    printf("\n=== (3) EM01A-Keyframes: Root-Pose (byte 0..5) und SPEED (byte 6..11) ===\n");
    {
        re15_enemy_bank_t *ab = re15_enemy_find(0x1A);
        if (ab && ab->ok) {
            for (int c = 0; c < ab->anim.clip_count && c < 8; c++) {
                int32_t sxsum = 0, sysum = 0, szsum = 0;
                int32_t rx0 = 0, rz0 = 0, rxN = 0, rzN = 0;
                int32_t maxabs = 0;
                int n = ab->anim.clips[c].frame_count;
                for (int f = 0; f < n; f++) {
                    int kf = (int)(ab->anim.frames[ab->anim.clips[c].first_frame + f] & 0xfffu);
                    int16_t sx = 0, sy = 0, sz2 = 0;
                    if (re15_emd_get_keyframe_speed(&ab->skel, kf, &sx, &sy, &sz2) == 0) {
                        sxsum += sx; sysum += sy; szsum += sz2;
                        int32_t a = sx < 0 ? -sx : sx; if (a > maxabs) maxabs = a;
                        a = sz2 < 0 ? -sz2 : sz2; if (a > maxabs) maxabs = a;
                    }
                    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
                    if (re15_skel_compute_pose(&ab->skel, kf, poses) == 0) {
                        if (f == 0)   { rx0 = poses[0].trans[0]; rz0 = poses[0].trans[2]; }
                        if (f == n-1) { rxN = poses[0].trans[0]; rzN = poses[0].trans[2]; }
                    }
                }
                printf("  Clip %d (%3d Bilder): SPEED-Summe (%6ld,%6ld,%6ld) maxabs=%ld | Root-Pose x/z: erstes (%ld,%ld) letztes (%ld,%ld) delta (%ld,%ld)\n",
                       c, n, (long)sxsum, (long)sysum, (long)szsum, (long)maxabs,
                       (long)rx0, (long)rz0, (long)rxN, (long)rzN, (long)(rxN-rx0), (long)(rzN-rz0));
            }
        } else printf("  (Bank fehlt)\n");
    }

    /* ---------- (5) Hand-Bone-Wanderung Clip 0 vs Clip 2 vs Clip 1 ---------- */
    printf("\n=== (5) Bone-Weltpositionen je Clip (Arm auf (0,0), yaw 0) ===\n");
    {
        re15_enemy_bank_t *ab = re15_enemy_find(0x1A);
        if (ab && ab->ok) {
            printf("  Bones: %d\n", ab->skel.bone_count);
            for (int c = 0; c < ab->anim.clip_count && c < 8; c++) {
                for (int bn = 0; bn < ab->skel.bone_count; bn++) {
                    int32_t minx = 0x7fffffff, maxx = -0x7fffffff, miny = 0x7fffffff, maxy = -0x7fffffff;
                    for (int f = 0; f < ab->anim.clips[c].frame_count; f++) {
                        int kf = (int)(ab->anim.frames[ab->anim.clips[c].first_frame + f] & 0xfffu);
                        static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
                        if (re15_skel_compute_pose(&ab->skel, kf, poses) != 0) continue;
                        int32_t w[3];
                        re15_skel_bone_to_world(poses[bn].trans, 0, 0, 0, 0, w);
                        if (w[0] < minx) minx = w[0];
                        if (w[0] > maxx) maxx = w[0];
                        if (w[1] < miny) miny = w[1];
                        if (w[1] > maxy) maxy = w[1];
                    }
                    printf("    Clip %d Bone %d: X %6ld..%6ld (Hub %5ld)  Y %6ld..%6ld\n",
                           c, bn, (long)minx, (long)maxx, (long)(maxx-minx), (long)miny, (long)maxy);
                }
            }
        }
    }

    /* ---------- (4) Port-Lauf ueber den BEGEHBAREN Flur ---------- */
    printf("\n=== (4) Port: Spieler geht den begehbaren Flur ab (75/Bild) ===\n");
    {
        int32_t zmin = 0x7fffffff, zmax = -0x7fffffff;
        for (int i = 0; i < na; i++) {
            re15_actor_t *e = &g_actors[arms[i]];
            if (e->z < zmin) zmin = e->z;
            if (e->z > zmax) zmax = e->z;
        }
        printf("  Arm-z-Bereich %ld .. %ld\n", (long)zmin, (long)zmax);
        uint8_t seen_sub[16]; memset(seen_sub, 0, sizeof seen_sub);
        int mask[16]; memset(mask, 0, sizeof mask);
        int32_t start_x[16], start_z[16];
        for (int i = 0; i < na; i++) { start_x[i] = g_actors[arms[i]].x; start_z[i] = g_actors[arms[i]].z; }
        int frame = 0;
        for (int32_t z = zmax + 2000; z > zmin - 2000; z -= 75) {
            int32_t lo = 0, hi = 0; int have = 0;
            for (int32_t x = -34000; x <= -6000; x += 25)
                if (re15_collision_on_floor(&s_rdt, x, z)) { if (!have) { lo = x; have = 1; } hi = x; }
            if (!have) continue;
            pl->x = (lo + hi) / 2; pl->z = z; pl->hp = 100;
            frame_step();
            frame++;
            for (int i = 0; i < na; i++) {
                re15_actor_t *e = &g_actors[arms[i]];
                mask[i] |= (1 << (e->sub_state_1 & 7));
                if (e->sub_state_1 > seen_sub[i]) seen_sub[i] = e->sub_state_1;
            }
            if ((frame % 10) == 0) {
                printf("  f%3d PL(%7ld,%7ld) Flur x %7ld..%7ld |", frame,
                       (long)pl->x, (long)pl->z, (long)lo, (long)hi);
                for (int i = 0; i < na; i++) {
                    re15_actor_t *e = &g_actors[arms[i]];
                    int32_t dx = pl->x - e->x, dz = pl->z - e->z;
                    int32_t dd = (int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz));
                    printf(" [%d s%u c%u f%u d%ld]", arms[i], e->sub_state_1, e->motion, e->anim_frame, (long)dd);
                }
                printf("\n");
            }
        }
        printf("  -- Zusammenfassung --\n");
        for (int i = 0; i < na; i++) {
            re15_actor_t *e = &g_actors[arms[i]];
            printf("   slot %2d: erreichte sub-Zustaende Maske 0x%02x, hoechster %u | Position verschoben? dx=%ld dz=%ld\n",
                   arms[i], mask[i], seen_sub[i], (long)(e->x - start_x[i]), (long)(e->z - start_z[i]));
        }
        printf("  Bilder gesamt: %d\n", frame);
    }

    /* ---------- (6) SICHTBARKEIT: Region-Cull je Kamera-Cut ----------
     * Der PC-Port verwirft jeden Gegner ausserhalb des Region-Vierecks des aktiven Cuts
     * (platform/pc/main.c: re15_rdt_get_region_quad + re15_aot_point_in_quad, PSX
     * FUN_8002c18c -> FUN_80014368). Also: liegt der Arm in IRGENDEINEM Cut-Viereck --
     * an seiner Spawn-Position und nach der Original-Translation von 2415 Einheiten? */
    printf("\n=== (6) Region-Cull: liegt der Arm im Viereck eines Cuts? ===\n");
    {
        int16_t rxs[4], rzs[4];
        int ncuts = 0;
        for (int c = 0; c < 32; c++) if (re15_rdt_get_region_quad(&s_rdt, c, rxs, rzs)) ncuts = c + 1;
        printf("  Cuts mit Region-Viereck: bis %d\n", ncuts);
        for (int c = 0; c < ncuts; c++) {
            if (!re15_rdt_get_region_quad(&s_rdt, c, rxs, rzs)) { printf("  Cut %2d: kein Viereck\n", c); continue; }
            printf("  Cut %2d Viereck (%d,%d) (%d,%d) (%d,%d) (%d,%d):\n", c,
                   rxs[0], rzs[0], rxs[1], rzs[1], rxs[2], rzs[2], rxs[3], rzs[3]);
            for (int i = 0; i < na; i++) {
                re15_actor_t *e = &g_actors[arms[i]];
                int in0 = re15_aot_point_in_quad(e->x, e->z, rxs, rzs);
                int t = (int)e->rot_y & 0xfff;
                int32_t lx = e->x + (int32_t)(((int64_t)2415 * re15_cos_q12(t)) >> 12);
                int32_t lz = e->z + (int32_t)(((int64_t)(-2415) * re15_sin_q12(t)) >> 12);
                int in1 = re15_aot_point_in_quad(lx, lz, rxs, rzs);
                if (in0 || in1)
                    printf("     slot %2d: spawn %s | nach 2415 (%ld,%ld) %s\n", arms[i],
                           in0 ? "DRIN" : "draussen", (long)lx, (long)lz, in1 ? "DRIN" : "draussen");
            }
        }
        printf("  -- pro Arm: in wie vielen Cut-Vierecken? --\n");
        for (int i = 0; i < na; i++) {
            re15_actor_t *e = &g_actors[arms[i]];
            int t = (int)e->rot_y & 0xfff;
            int32_t lx = e->x + (int32_t)(((int64_t)2415 * re15_cos_q12(t)) >> 12);
            int32_t lz = e->z + (int32_t)(((int64_t)(-2415) * re15_sin_q12(t)) >> 12);
            int n0 = 0, n1 = 0;
            for (int c = 0; c < ncuts; c++) {
                if (!re15_rdt_get_region_quad(&s_rdt, c, rxs, rzs)) continue;
                n0 += re15_aot_point_in_quad(e->x, e->z, rxs, rzs) ? 1 : 0;
                n1 += re15_aot_point_in_quad(lx, lz, rxs, rzs) ? 1 : 0;
            }
            printf("     slot %2d: spawn in %d Cuts, nach der Translation in %d Cuts | y=%ld\n",
                   arms[i], n0, n1, (long)e->y);
        }
    }

    /* ---------- (7) Haelt der Wand-Klemmer (FUN_8003b0a4, ROOT @0x8010c324) die Lunge auf? ----
     * Der Writher-ROOT ruft jeden Bild func_0x8003b0a4(+0x34, *(+0x78)+6, 4) — wenn der Port-
     * Zwilling re15_collision_constrain_enemy den Arm zurueckdrueckt, waere die Translation
     * wirkungslos. Also die 3x800-Schritte durch den Klemmer schicken und messen. */
    printf("\n=== (7) Wand-Klemmer gegen die Lunge (Radius 300, +0x78 @0x8012091c) ===\n");
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        int32_t px = e->x, pz = e->z;
        int t = (int)e->rot_y & 0xfff;
        int blocked = 0;
        struct { int frames; int S; int rev; } seq[] = {
            {3, 800, 0}, {1, 20, 0}, {30, 20, 1}, {1, 200, 1}, {4, 200, 0}
        };
        for (unsigned k = 0; k < sizeof seq / sizeof seq[0]; k++)
            for (int f = 0; f < seq[k].frames; f++) {
                int tt = (t + (seq[k].rev ? 0x800 : 0)) & 0xfff;
                int32_t nx = px + (int32_t)(((int64_t)seq[k].S * re15_cos_q12(tt)) >> 12);
                int32_t nz = pz + (int32_t)(((int64_t)(-seq[k].S) * re15_sin_q12(tt)) >> 12);
                int32_t wx = nx, wz = nz;
                re15_collision_constrain_enemy(&s_rdt, px, pz, &wx, &wz, 300, e->y, 4);
                if (wx != nx || wz != nz) blocked++;
                px = wx; pz = wz;
            }
        printf("  slot %2d: Endpunkt mit Klemmer (%7ld,%7ld) | ohne Klemmer waere x=%7ld | geklemmte Bilder %d\n",
               arms[i], (long)px, (long)pz,
               (long)(e->x + (int32_t)(((int64_t)2420 * re15_cos_q12(t)) >> 12)), blocked);
    }

    free(buf);
    printf("\nprobe_1210_reach: MESSUNG FERTIG\n");
    return 0;
}
