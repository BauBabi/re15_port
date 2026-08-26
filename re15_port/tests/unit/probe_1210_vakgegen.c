/* probe_1210_vakgegen.c - GEGENSONDE zur VAKUANZ-Pruefung des Befunds "1210-re2-verhalten".
 *
 * Drei Fragen, die die Vorlage-Sonde (probe_1210_re2takt) NICHT beantwortet:
 *  (1) Ist der Wand-Pfad der Vorlage ueberhaupt LAUFBAR? Sie schiebt den Spieler pro Bild
 *      bis zu 200x25 = 5000 Einheiten quer, waehrend er 75 bzw. 200 vorwaerts geht.
 *      Hier wird derselbe Lauf mit HARTER Tempo-Deckelung (Euklid <= step) wiederholt.
 *  (2) Bleibt "GRIFF = -1" auch auf dem gedeckelten Pfad?
 *  (3) Was macht der VORGESCHLAGENE Ausdruck, wenn er - wie im Produktivcode - JEDES Bild
 *      ausgewertet wird, also auch waehrend der Arm schon ausgefahren ist?
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

static int32_t pt_dist(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{
    int32_t dx = ax - bx, dz = az - bz;
    return (int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz));
}

static void proj_pt(const re15_actor_t *e, int32_t reach, int32_t *ox, int32_t *oz)
{
    int t = (int)e->rot_y & 0xfff;
    *ox = e->x + (int32_t)(((int64_t)reach * re15_cos_q12(t)) >> 12);
    *oz = e->z - (int32_t)(((int64_t)reach * re15_sin_q12(t)) >> 12);
}

static void setup(int paused)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(paused);
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
}

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    for (int lane = 0; lane < 3; lane++)
    for (int gait = 0; gait < 2; gait++) {
        const int step = gait ? 200 : 75;
        setup(0);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int arms[16]; int na = 0;
        for (int s = 1; s < RE15_ACTOR_MAX && na < 16; s++)
            if (g_actors[s].active && g_actors[s].type == 0x1A) arms[na++] = s;
        int32_t hx[16], hz[16];
        int32_t zmax = -0x7fffffff;
        for (int i = 0; i < na; i++) {
            hx[i] = g_actors[arms[i]].x; hz[i] = g_actors[arms[i]].z;
            if (g_actors[arms[i]].z > zmax) zmax = g_actors[arms[i]].z;
        }

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
        printf("\n===== GEDECKELT: SPUR %s, TEMPO %d/Bild =====\n",
               lane == 0 ? "West-Wand" : lane == 1 ? "Mitte" : "Ost-Wand", step);
        printf("  Start (%ld,%ld), %d Arme\n", (long)px, (long)pz, na);

        int   f_grab[16], f_ready2[16];
        int32_t dcur_at_ready[16], dhome_at_ready[16], minhand[16];
        int32_t dcur_at_min[16], ext_at_min[16]; int sub1_at_min[16];
        int32_t ext_at_sub2ge1[16]; int32_t dcur_at_sub2ge1[16];
        uint8_t seen_sub2[16];
        for (int i = 0; i < 16; i++) {
            f_grab[i] = f_ready2[i] = -1;
            dcur_at_ready[i] = dhome_at_ready[i] = -1; minhand[i] = 0x7fffffff;
            dcur_at_min[i] = ext_at_min[i] = -1; sub1_at_min[i] = -1;
            ext_at_sub2ge1[i] = dcur_at_sub2ge1[i] = -1; seen_sub2[i] = 0;
        }
        int32_t maxmove = 0;
        int grabs = 0;
        for (int f = 0; f < 1400; f++) {
            if (!re15_player_is_grabbed()) {
                int32_t ox = px, oz = pz;
                int32_t nx = px, nz = pz - step;
                re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
                px = nx; pz = nz;
                if (lane != 1) {
                    int32_t used = pt_dist(px, pz, ox, oz);
                    int32_t budget = step - used; if (budget < 0) budget = 0;
                    int32_t dir = (lane == 0) ? -25 : 25;
                    int steps = (int)(budget / 25);
                    for (int k = 0; k < steps; k++) {
                        int32_t ax = px + dir, az = pz;
                        re15_collision_constrain(&s_rdt, px, pz, &ax, &az);
                        if (ax == px && az == pz) break;
                        px = ax; pz = az;
                    }
                }
                int32_t mv = pt_dist(px, pz, ox, oz);
                if (mv > maxmove) maxmove = mv;
                if (mv == 0) break;
                pl->x = px; pl->z = pz;
            }
            pl->hp = 100;
            frame_step();
            for (int i = 0; i < na; i++) {
                re15_actor_t *e = &g_actors[arms[i]];
                int32_t ax, az;
                proj_pt(e, MESH_REACH, &ax, &az);
                int32_t hd = pt_dist(pl->x, pl->z, ax, az);
                if (hd < minhand[i]) {
                    minhand[i] = hd;
                    { int32_t bx, bz; proj_pt(e, LUNGE_NET + MESH_REACH, &bx, &bz);
                      dcur_at_min[i] = pt_dist(pl->x, pl->z, bx, bz); }
                    ext_at_min[i]  = pt_dist(e->x, e->z, hx[i], hz[i]);
                    sub1_at_min[i] = (int)e->sub_state_1;
                }
                if (!seen_sub2[i] && e->sub_state_1 == 1 && e->sub_state_2 >= 1) {
                    seen_sub2[i] = 1;
                    ext_at_sub2ge1[i] = pt_dist(e->x, e->z, hx[i], hz[i]);
                    { int32_t bx, bz; proj_pt(e, MESH_REACH, &bx, &bz);
                      dcur_at_sub2ge1[i] = pt_dist(pl->x, pl->z, bx, bz); }
                }
                if (e->sub_state_1 == 2 && f_ready2[i] < 0) {
                    f_ready2[i] = f;
                    proj_pt(e, LUNGE_NET + MESH_REACH, &ax, &az);
                    dcur_at_ready[i] = pt_dist(pl->x, pl->z, ax, az);
                    {   int t = (int)e->rot_y & 0xfff;
                        int32_t bx = hx[i] + (int32_t)(((int64_t)(LUNGE_NET+MESH_REACH) * re15_cos_q12(t)) >> 12);
                        int32_t bz = hz[i] - (int32_t)(((int64_t)(LUNGE_NET+MESH_REACH) * re15_sin_q12(t)) >> 12);
                        dhome_at_ready[i] = pt_dist(pl->x, pl->z, bx, bz);
                    }
                }
                if (e->sub_state_1 == 4 && f_grab[i] < 0) { f_grab[i] = f; grabs++; }
            }
            if (pz < -25000) break;
        }
        printf("  groesste Spieler-Verschiebung in EINEM Bild: %ld (Deckel %d)\n",
               (long)maxmove, step);
        printf("  GRIFFE gesamt: %d\n", grabs);
        printf("  Arm | sub1==2 ab Bild | d_cur LIVE | d_home wie simuliert | minHand | Griff\n");
        for (int i = 0; i < na; i++)
            printf("  %2d  | %6d          | %8ld   | %8ld             | %6ld  | %4d\n",
                   arms[i], f_ready2[i], (long)dcur_at_ready[i], (long)dhome_at_ready[i],
                   (long)minhand[i], f_grab[i]);
        printf("  --- BEI GROESSTER NAEHE (der Moment, in dem der Vorschlag greifen soll) ---\n");
        printf("  Arm | minHand | sub1 | Ausfahrweite | d_cur(Vorschlag,LIVE) | reach<1300?\n");
        for (int i = 0; i < na; i++)
            printf("  %2d  | %6ld  | %4d | %8ld     | %8ld              | %s\n",
                   arms[i], (long)minhand[i], sub1_at_min[i], (long)ext_at_min[i],
                   (long)dcur_at_min[i],
                   (dcur_at_min[i] >= 0 && dcur_at_min[i] < 1300) ? "JA" : "NEIN");
        printf("  --- ARMIERUNGS-PUNKT des Vorschlags 2 (sub1==1 && sub2>=1) ---\n");
        printf("  Arm | Ausfahrweite dort | Hand-Abstand dort (Tor 1300)\n");
        for (int i = 0; i < na; i++)
            printf("  %2d  | %8ld          | %8ld\n",
                   arms[i], (long)ext_at_sub2ge1[i], (long)dcur_at_sub2ge1[i]);
    }

    free(buf);
    printf("\nprobe_1210_vakgegen: MESSUNG FERTIG\n");
    return 0;
}
