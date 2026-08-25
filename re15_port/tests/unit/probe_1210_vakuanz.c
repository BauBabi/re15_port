/* probe_1210_vakuanz.c - GEGENSONDE zur adversarialen Pruefung des Befunds "1210-reichweite".
 * Frage: messen die Zahlen des Befunds einen Zustand, den das SPIEL einnimmt?
 *   (G1) Ist der gemessene Spieler-Grenzpunkt (-20640 / -18147) vom TUER-Einstieg aus
 *        ueberhaupt erlaufbar? (Flutfuellung ab dem echten AOT-Einstieg, nicht ab -19500.)
 *   (G2) Wie weit kommt der Arm im NATUERLICHEN Ablauf (voller game_step, echte FSM,
 *        echter Klemmer) - gegen den im Befund zitierten "Anschlag" (-22468/-16358),
 *        der aus UNBEGRENZTER Translation stammt?
 *   (G3) Akkumuliert die Lunge ueber mehrere Vorbeilaeufe bis zum Anschlag?
 *   (G4) Die Spitzen-Abstaende aus der NATUERLICHEN Endlage statt aus dem Anschlag.
 * Kein Produktivcode geaendert, kein Test abgeschwaecht.
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
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
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

/* identisch zu probe_1210_geo: Broadphase-Besetzbarkeit (r=450 @0x80073e9a, Maske 1 @0x80031d74) */
static int occupiable(int32_t x, int32_t z)
{
    const int32_t r = 450;
    unsigned zb = (unsigned)(z - (int32_t)(int16_t)s_rdt.ceiling_z) & 0x80000000u;
    unsigned xb = (unsigned)(x - (int32_t)(int16_t)s_rdt.ceiling_x) & 0x80000000u;
    int q = (int)((zb | (xb >> 1)) >> 30);
    int st = 0; for (int i = 0; i < q && i < 5; i++) st += s_rdt.sca_rgn[i];
    int en = st + (q < 5 ? s_rdt.sca_rgn[q] : 0);
    if (en > s_rdt.sca_count) en = s_rdt.sca_count;
    for (int i = st; i < en; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        if ((e->floor >> 4) != 0) continue;
        if ((1u & e->u0) == 0) continue;
        if ((unsigned)(x - ((int32_t)e->x - r)) < (unsigned)((int32_t)e->width   + r*2) &&
            (unsigned)(z - ((int32_t)e->z - r)) < (unsigned)((int32_t)e->density + r*2))
            return 0;
    }
    { int32_t nx = x, nz = z; re15_collision_objects(&nx, &nz);
      if (nx != x || nz != z) return 0; }
    return 1;
}

#define GX0 (-34000)
#define GX1 (-4000)
#define GZ0 (-30000)
#define GZ1 (-2000)
#define GS  25
#define GW  (((GX1)-(GX0))/GS + 1)
#define GH  (((GZ1)-(GZ0))/GS + 1)
static uint8_t g_occ[GW*GH];
static uint8_t g_rea[GW*GH];
static int     g_stack[GW*GH];

static int32_t idist(int32_t dx, int32_t dz)
{ return (int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz)); }

static void flood(int32_t sx, int32_t sz)
{
    memset(g_rea, 0, sizeof g_rea);
    int gx = (int)((sx - GX0)/GS), gz = (int)((sz - GZ0)/GS);
    if (gx < 0 || gx >= GW || gz < 0 || gz >= GH) { printf("  (Saat ausserhalb des Gitters)\n"); return; }
    if (!g_occ[gz*GW+gx]) { printf("  (Saat NICHT besetzbar)\n"); return; }
    int sp = 0; g_rea[gz*GW+gx] = 1; g_stack[sp++] = gz*GW+gx;
    const int dxs[4] = {1,-1,0,0}, dzs[4] = {0,0,1,-1};
    while (sp > 0) {
        int c = g_stack[--sp]; int cx = c % GW, cz = c / GW;
        for (int k = 0; k < 4; k++) {
            int nx = cx + dxs[k], nz = cz + dzs[k];
            if (nx < 0 || nx >= GW || nz < 0 || nz >= GH) continue;
            if (g_rea[nz*GW+nx] || !g_occ[nz*GW+nx]) continue;
            g_rea[nz*GW+nx] = 1; g_stack[sp++] = nz*GW+nx;
        }
    }
}
static int reached(int32_t x, int32_t z)
{
    int gx = (int)((x - GX0 + GS/2)/GS), gz = (int)((z - GZ0 + GS/2)/GS);
    if (gx < 0 || gx >= GW || gz < 0 || gz >= GH) return -1;
    return g_rea[gz*GW+gx];
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
    pl->x = 0; pl->z = 0;                 /* weit weg -> kein reach */
    re15_collision_set_band(0);
    /* ⛔ HARNESS-FIDELITAET: re15_writher_step klemmt gegen g_room_rdt (enemy_ai_common.c),
     * NICHT gegen die lokale Kopie. Ohne das ist der Arm-Klemmer im Sondenlauf ein NO-OP
     * (collision_constrain_impl: `if (!rdt || !rdt->sca ...) return 0;`) — das echte Spiel
     * fuellt g_room_rdt im Raum-Loader. */
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    printf("EM01A-Bank: %s\n", load_bank(0x1A, s_blob1a, sizeof s_blob1a) ? "ok" : "FEHLT");
    for (int f = 0; f < 8; f++) frame_step();

    int arms[16]; int na = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && na < 16; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) arms[na++] = s;
    printf("Arme: %d\n", na);

    for (int gz = 0; gz < GH; gz++)
        for (int gx = 0; gx < GW; gx++)
            g_occ[gz*GW+gx] = (uint8_t)occupiable(GX0 + gx*GS, GZ0 + gz*GS);

    /* ---------- (G1) AOTs: wo betritt/steht der Spieler wirklich? ---------- */
    printf("\n=== (G1) AOT-Slots von ROOM1210 (Einstiegs-/Ereigniszonen) ===\n");
    int32_t seedx = 0, seedz = 0; int have_seed = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        if (!g_aot.slots[i].active) continue;
        int occ = occupiable(g_aot.slots[i].x, g_aot.slots[i].z);
        printf("  AOT#%2d typ=%d  (%7ld,%7ld)  besetzbar=%d\n",
               i, g_aot.slots[i].type, (long)g_aot.slots[i].x, (long)g_aot.slots[i].z, occ);
        if (!have_seed && occ && g_aot.slots[i].type == RE15_AOT_TYPE_DOOR) {
            seedx = g_aot.slots[i].x; seedz = g_aot.slots[i].z; have_seed = 1;
        }
    }
    if (!have_seed) {
        for (int i = 0; i < RE15_AOT_MAX && !have_seed; i++) {
            if (!g_aot.slots[i].active) continue;
            if (!occupiable(g_aot.slots[i].x, g_aot.slots[i].z)) continue;
            seedx = g_aot.slots[i].x; seedz = g_aot.slots[i].z; have_seed = 1;
        }
    }
    if (!have_seed) { seedx = -19500; seedz = -14000; printf("  (kein besetzbarer AOT -> Saat Flurmitte)\n"); }
    printf("  Saat fuer die Flutfuellung: (%ld,%ld)\n", (long)seedx, (long)seedz);
    flood(seedx, seedz);
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        /* NICHT den Grenzwert selbst abfragen: reached() rundet aufs 25er-Raster,
         * -20640 rundet auf -20650 = BLOCKIERT (Artefakt meiner eigenen Sonde).
         * Naechster besetzbarer RASTERPUNKT: West -20625, Ost -18150. */
        int32_t wx = (e->x < -19500) ? -20625 : -18150;
        printf("  Arm slot %2d z=%7ld: Rasterpunkt (%7ld,%7ld) besetzbar=%d erreichbar=%d\n",
               arms[i], (long)e->z, (long)wx, (long)e->z, occupiable(wx,e->z), reached(wx, e->z));
    }

    /* ---------- (G2/G3/G4) NATUERLICHER ABLAUF: voller game_step ---------- */
    printf("\n=== (G2) Arm-Auslenkung im NATUERLICHEN Ablauf (voller game_step, echte FSM) ===\n");
    printf("    Spieler laeuft ab Flurmitte an die Wand (Klemmer+Objekte wie game_step_common.c:1287-98),\n");
    printf("    steht dort, AI tickt. 3 Vorbeilaeufe (raus aus reach / rein) je Arm.\n");
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        int32_t home_x = e->x, home_z = e->z;
        int yaw = (int)e->rot_y & 0xfff;
        int32_t wallx = (e->x < -19500) ? -20640 : -18147;
        int dirsign = (e->x < -19500) ? -1 : 1;   /* Laufrichtung des Spielers zur Arm-Seite */
        int32_t best_fwd = e->x;
        int32_t best_z = e->z;
        int32_t mind = 0x7fffffff;
        int grabbed = 0;
        int32_t px = -19500, pz = e->z;
        for (int pass = 0; pass < 3; pass++) {
            px = -19500; pz = e->z;
            for (int f = 0; f < 400; f++) {
                int32_t nx = px + (dirsign < 0 ? -75 : 75), nz = pz;
                re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
                re15_collision_objects(&nx, &nz);
                pl->x = nx; pl->z = nz;
                frame_step();
                px = pl->x; pz = pl->z;
                int32_t d = idist(px - e->x, pz - e->z);
                if (d < mind) mind = d;
                if ((dirsign < 0 && e->x > best_fwd) || (dirsign > 0 && e->x < best_fwd)) { best_fwd = e->x; best_z = e->z; }
                if (re15_player_is_grabbed()) grabbed = 1;
            }
            for (int f = 0; f < 200; f++) {
                pl->x = px; pl->z = pz;
                frame_step();
                int32_t d = idist(pl->x - e->x, pl->z - e->z);
                if (d < mind) mind = d;
                if ((dirsign < 0 && e->x > best_fwd) || (dirsign > 0 && e->x < best_fwd)) { best_fwd = e->x; best_z = e->z; }
                if (re15_player_is_grabbed()) grabbed = 1;
            }
            for (int f = 0; f < 200; f++) {   /* raus aus reach: dz > 1700 */
                pl->x = px; pl->z = e->z + 6000;
                frame_step();
            }
        }
        int32_t tip = best_fwd + (int32_t)(((int64_t)1671 * re15_cos_q12(yaw)) >> 12);
        int32_t dtip = wallx > tip ? wallx - tip : tip - wallx;
        int32_t dorg = wallx > best_fwd ? wallx - best_fwd : best_fwd - wallx;
        printf("  [radien] slot %2d hit_radius_min=%d hit_radius_max=%d y=%ld\n",
               arms[i], (int)e->hit_radius_min, (int)e->hit_radius_max, (long)e->y);
        printf("  slot %2d Heim(%7ld,%7ld) yaw=%4d | NATUERLICH weiteste Lage x=%7ld z=%7ld"
               " | Ursprungs-Abstand zur Wandgrenze %5ld | Mesh-Spitze %7ld -> Abstand %5ld"
               " | min gemessener Ursprungs-Abstand %5ld | gegriffen=%d | Endlage x=%7ld sub=%d\n",
               arms[i], (long)home_x, (long)home_z, yaw, (long)best_fwd, (long)best_z,
               (long)dorg, (long)tip, (long)dtip, (long)mind, grabbed,
               (long)e->x, (int)e->sub_state_1);
    }

    free(buf);
    printf("\nprobe_1210_vakuanz: MESSUNG FERTIG\n");
    return 0;
}
