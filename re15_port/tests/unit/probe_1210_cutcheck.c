/* probe_1210_cutcheck.c - GEGENPRUEFUNG (kein Test, kein add_test).
 * Der zu pruefende Befund behauptet: die zehn EM01A liegen "in NULL von neun
 * Kamera-Region-Vierecken" und werden deshalb GAR NICHT gezeichnet; erst die
 * Original-Translation von 2420 Einheiten schiebe sie "in 1 bis 3 Cut-Vierecke".
 *
 * Der Port-Cull (platform/pc/main.c:6609) testet aber NUR das Viereck des AKTIVEN
 * Cuts (cam_has_region/cam_region_xs aus re15_rdt_get_region_quad(active_cut_idx)).
 * "in irgendeinem der 9 Vierecke" ist also NICHT die Groesse, die ueber Sichtbarkeit
 * entscheidet. Diese Sonde misst deshalb PRO BILD den AKTIVEN Cut (g_scd.cam_id, vom
 * AOT-Scan gesetzt) und testet gegen DESSEN Viereck.
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
                eb->ok = 1; eb->buf = NULL; ok = 1;
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
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &sz);
    if (!buf) { printf("FAIL: RDT nicht lesbar\n"); return 1; }
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
    load_bank(0x1A, s_blob1a, sizeof s_blob1a);
    for (int f = 0; f < 8; f++) frame_step();

    int arms[16]; int na = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && na < 16; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) arms[na++] = s;
    printf("Arme: %d | nCut(RDT-Header)=%d | zone_count=%d\n", na, (int)buf[1], s_rdt.zone_count);

    for (int c = 0; c < (int)buf[1]; c++) {
        int16_t rx[4], rz[4];
        printf("  Cut %d: Anker-Viereck %s\n", c,
               re15_rdt_get_region_quad(&s_rdt, c, rx, rz) ? "JA" : "NEIN -> cam_has_region=0");
    }

    int32_t lx[16], lz[16];
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        int t = (int)e->rot_y & 0xfff;
        lx[i] = e->x + (int32_t)(((int64_t)2420 * re15_cos_q12(t)) >> 12);
        lz[i] = e->z + (int32_t)(((int64_t)(-2420) * re15_sin_q12(t)) >> 12);
    }

    printf("\n=== Lauf ueber den begehbaren Flur, AKTIVER Cut pro Bild ===\n");
    int cut_hist[32]; memset(cut_hist, 0, sizeof cut_hist);
    int pl_out = 0, frames = 0;
    int spawn_in_active[16]; memset(spawn_in_active, 0, sizeof spawn_in_active);
    int lunge_in_active[16]; memset(lunge_in_active, 0, sizeof lunge_in_active);
    int nearest_cut[16]; int32_t nearest_d[16];
    for (int i = 0; i < na; i++) { nearest_cut[i] = -1; nearest_d[i] = 0x7fffffff; }

    int32_t zmin = 0x7fffffff, zmax = -0x7fffffff;
    for (int i = 0; i < na; i++) {
        if (g_actors[arms[i]].z < zmin) zmin = g_actors[arms[i]].z;
        if (g_actors[arms[i]].z > zmax) zmax = g_actors[arms[i]].z;
    }
    int last_cut = -1;
    const char *mode = getenv("ROUTE") ? getenv("ROUTE") : "mitte";
    int route = (mode[0] == 'r');
    printf("  (Pfad: %s)\n", route ? "ECHTE Route durch die RVD-Schaltzonen" : "Flurmitte wie im Befund");
    for (int32_t z = (route ? 300 : zmax + 2000); z > zmin - 2000; z -= 75) {
        int32_t lo = 0, hi = 0; int have = 0;
        for (int32_t x = -34000; x <= -6000; x += 25)
            if (re15_collision_on_floor(&s_rdt, x, z)) { if (!have) { lo = x; have = 1; } hi = x; }
        if (!have) continue;
        if (route) {
            int32_t want = (z > -16000) ? -21700 : -19500;
            if (want < lo) want = lo;
            if (want > hi) want = hi;
            pl->x = want;
        } else {
            pl->x = (lo + hi) / 2;
        }
        pl->z = z; pl->hp = 100;
        frame_step();
        frames++;
        int cut = (int)g_scd.cam_id;
        if (cut >= 0 && cut < 32) cut_hist[cut]++;
        int16_t rx[4], rz[4];
        int has = re15_rdt_get_region_quad(&s_rdt, cut, rx, rz);
        int pin = has ? re15_aot_point_in_quad(pl->x, pl->z, rx, rz) : 1;
        if (!pin) pl_out++;
        if (cut != last_cut) {
            printf("  f%3d z=%7ld -> AKTIVER CUT %d (Viereck %s, Spieler %s)\n",
                   frames, (long)z, cut, has ? "ja" : "NEIN", pin ? "DRIN" : "DRAUSSEN(!)");
            last_cut = cut;
        }
        for (int i = 0; i < na; i++) {
            re15_actor_t *e = &g_actors[arms[i]];
            if (has) {
                if (re15_aot_point_in_quad(e->x, e->z, rx, rz)) spawn_in_active[i]++;
                if (re15_aot_point_in_quad(lx[i], lz[i], rx, rz)) lunge_in_active[i]++;
            }
            int32_t dx = pl->x - e->x, dz = pl->z - e->z;
            int32_t dd = (int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz));
            if (dd < nearest_d[i]) { nearest_d[i] = dd; nearest_cut[i] = cut; }
        }
    }
    printf("  Bilder %d | Spieler in %d Bildern AUSSERHALB seines eigenen Cut-Vierecks\n", frames, pl_out);
    printf("  Cut-Histogramm:");
    for (int c = 0; c < 32; c++) if (cut_hist[c]) printf(" [%d]=%d", c, cut_hist[c]);
    printf("\n");
    printf("  -- pro Arm: in wievielen der %d Bilder liegt er im Viereck des AKTIVEN Cuts? --\n", frames);
    for (int i = 0; i < na; i++)
        printf("     slot %2d spawn(%7ld,%7ld): SPAWN %4d Bilder DRIN | LUNGE(%7ld,%7ld) %4d Bilder DRIN | naechster Abstand %ld bei Cut %d\n",
               arms[i], (long)g_actors[arms[i]].x, (long)g_actors[arms[i]].z,
               spawn_in_active[i], (long)lx[i], (long)lz[i], lunge_in_active[i],
               (long)nearest_d[i], nearest_cut[i]);

    free(buf);
    printf("\nprobe_1210_cutcheck: FERTIG\n");
    return 0;
}
