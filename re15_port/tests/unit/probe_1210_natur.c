/* probe_1210_natur.c — GEGENSONDE zur Sound-Behauptung "der Port laesst den SE beim\n * ZUPACKEN aus" (Batch 2026-08-29, ROOM1210).
 *
 * FRAGE, die probe_1210_sound NICHT beantwortet: erreicht ein Spieler, der den Flur
 * WIRKLICH ablaeuft (Kollision an, keine Teleportation quer durch Waende, kein von Hand
 * gesetzter Arm-Sub), ueberhaupt jemals den Greif-Zustand (+0x5 = 4)? probe_1210_sound
 * MESSUNG A setzt den Spieler Bild fuer Bild auf x = -19500 (Flurmitte) OHNE Kollision;
 * MESSUNG B setzt den Arm von Hand auf Sub 2 und den Spieler 900 Einheiten davor.
 *
 * Diese Sonde behauptet nichts. Sie misst:
 *   C) Die ERREICHBARE Bodenmenge (Breitensuche vom Tuer-Ziel des Raums aus, Schrittweite
 *      75 = Gehtempo 0x4B, jeder Schritt durch re15_collision_constrain +
 *      re15_collision_objects) und den kleinsten Abstand dieser Menge zu jedem Arm
 *      (Heimat UND Lunge-Landepunkt).
 *   D) Flur-Durchlaeufe in mehreren Spuren MIT Kollision (an beide Waende gedrueckt).
 *   E) Bestmoegliche Annaeherung: aus der per BFS BELEGT erreichbaren Zelle, die dem
 *      Landepunkt am naechsten liegt, laeuft der Spieler mit Kollision auf den Arm zu.
 *   F) Nachstellung des VORGESCHLAGENEN Fixes ohne Produktivcode: an jedem Eintritt in
 *      +0x5 == 4 wird +0x239 (re2z_cd239) gelesen — nur bei 0 wuerde der vorgeschlagene,
 *      cooldown-gegatete Greif-SE ueberhaupt klingen.
 *
 * KEIN add_test — reine Messsonde. Kein Produktivcode geaendert. */
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

extern int g_test_room_se_log[2048];
extern int g_test_room_se_n;
extern int g_test_room_se_tick;

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

/* ---- BFS ueber den begehbaren Boden, Schritte durch die Kollision des Ports ---------- */
#define GX0 (-31000)
#define GX1 (-7000)
#define GZ0 (-27000)
#define GZ1 (-2000)
#define GS  75
#define GW  ((GX1 - GX0) / GS + 1)
#define GH  ((GZ1 - GZ0) / GS + 1)
static unsigned char s_vis[GW * GH];
static int32_t s_qx[GW * GH], s_qz[GW * GH];

static int cellx(int32_t x) { return (int)((x - GX0 + GS / 2) / GS); }
static int cellz(int32_t z) { return (int)((z - GZ0 + GS / 2) / GS); }

static int bfs(int32_t sx, int32_t sz)
{
    static const int dxs[8] = { 1, -1, 0, 0, 1, 1, -1, -1 };
    static const int dzs[8] = { 0, 0, 1, -1, 1, -1, 1, -1 };
    memset(s_vis, 0, sizeof s_vis);
    int head = 0, tail = 0;
    int cx = cellx(sx), cz = cellz(sz);
    if (cx < 0 || cx >= GW || cz < 0 || cz >= GH) return 0;
    s_vis[cz * GW + cx] = 1; s_qx[tail] = sx; s_qz[tail] = sz; tail++;
    while (head < tail) {
        int32_t px = s_qx[head], pz = s_qz[head]; head++;
        for (int d = 0; d < 8; d++) {
            int32_t nx = px + dxs[d] * GS, nz = pz + dzs[d] * GS;
            re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            if (!re15_collision_on_floor(&s_rdt, nx, nz)) continue;   /* ⛔ ohne diesen Test
                * leckt die Suche aus dem Raum heraus: re15_collision_constrain haelt nur
                * dann fest, wenn der AUSGANGSpunkt begehbar ist. */
            int ax = cellx(nx), az = cellz(nz);
            if (ax < 0 || ax >= GW || az < 0 || az >= GH) continue;
            if (s_vis[az * GW + ax]) continue;
            s_vis[az * GW + ax] = 1;
            if (tail < GW * GH) { s_qx[tail] = nx; s_qz[tail] = nz; tail++; }
        }
    }
    return tail;
}

static int32_t dist2d(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{
    int64_t dx = ax - bx, dz = az - bz;
    return (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
}

static int32_t nearest_reachable(int32_t tx, int32_t tz, int32_t *ox, int32_t *oz)
{
    int32_t best = 0x7fffffff;
    for (int z = 0; z < GH; z++)
        for (int x = 0; x < GW; x++) {
            if (!s_vis[z * GW + x]) continue;
            int32_t px = GX0 + x * GS, pz = GZ0 + z * GS;
            int32_t d = dist2d(px, pz, tx, tz);
            if (d < best) { best = d; if (ox) *ox = px; if (oz) *oz = pz; }
        }
    return best;
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

    printf("=== ROOM1210 GEGENSONDE: ist der Greif-Zustand im natuerlichen Ablauf erreichbar? ===\n");

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
    printf("  EM01A-Bank: %s\n", load_bank(0x1A, s_blob, sizeof s_blob) ? "ok" : "FEHLT");
    int donor = load_bank(0x10, s_blob_donor, sizeof s_blob_donor);
    re15_enemy_bank_t *db = re15_enemy_find(0x10);
    printf("  EM010-Spenderbank: %s victim_ok=%d\n", donor ? "ok" : "FEHLT", db ? db->victim_ok : -1);
    if (db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
    for (int f = 0; f < 8; f++) frame_step();

    int slots[RE15_ACTOR_MAX], n = 0;
    int32_t hx[RE15_ACTOR_MAX], hz[RE15_ACTOR_MAX], ex[RE15_ACTOR_MAX], ez[RE15_ACTOR_MAX];
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x1A) continue;
        slots[n] = s; hx[n] = e->x; hz[n] = e->z;
        int32_t fs = re15_sin_q12(((int)e->rot_y + 0x400) & 0xfff);
        int32_t fc = re15_cos_q12(((int)e->rot_y + 0x400) & 0xfff);
        ex[n] = e->x + (int32_t)((fs * 2420) >> 12);   /* Landepunkt der Lunge (netto 2420) */
        ez[n] = e->z + (int32_t)((fc * 2420) >> 12);
        n++;
    }
    printf("  %d Arme.\n", n);

    /* ---- MESSUNG C: erreichbare Bodenmenge -------------------------------------------- */
    printf("\n-- MESSUNG C: BFS-Erreichbarkeit vom Tuer-Ziel (-22400,-6500), Schritt 75,\n"
           "   jeder Schritt durch re15_collision_constrain + re15_collision_objects --\n");
    printf("   Tuer-Ziel auf begehbarem Boden? %d\n",
           re15_collision_on_floor(&s_rdt, -22400, -6500));
    /* Startpunkt: die begehbare Zelle, die dem Tuer-Ziel am naechsten liegt. */
    int32_t bsx = 0, bsz = 0; int32_t bsd = 0x7fffffff;
    for (int32_t zz = GZ0; zz <= GZ1; zz += GS)
        for (int32_t xx = GX0; xx <= GX1; xx += GS) {
            if (!re15_collision_on_floor(&s_rdt, xx, zz)) continue;
            int32_t d = dist2d(xx, zz, -22400, -6500);
            if (d < bsd) { bsd = d; bsx = xx; bsz = zz; }
        }
    printf("   BFS-Start: naechste begehbare Zelle (%ld,%ld), %ld vom Tuer-Ziel\n",
           (long)bsx, (long)bsz, (long)bsd);
    {   int cnt = 0;
        for (int32_t zz = GZ0; zz <= GZ1; zz += GS)
            for (int32_t xx = GX0; xx <= GX1; xx += GS)
                if (re15_collision_on_floor(&s_rdt, xx, zz)) cnt++;
        printf("   on_floor-Zellen im Raster: %d\n", cnt);
        int32_t tx, tz;
        for (int d = 0; d < 4; d++) {
            static const int ddx[4] = { 1, -1, 0, 0 }, ddz[4] = { 0, 0, 1, -1 };
            tx = bsx + ddx[d] * GS; tz = bsz + ddz[d] * GS;
            int fl = re15_collision_on_floor(&s_rdt, tx, tz);
            int32_t cx2 = tx, cz2 = tz;
            re15_collision_constrain(&s_rdt, bsx, bsz, &cx2, &cz2);
            int32_t ox2 = cx2, oz2 = cz2;
            re15_collision_objects(&cx2, &cz2);
            printf("   Nachbar (%ld,%ld) on_floor=%d -> constrain (%ld,%ld) -> objects (%ld,%ld)\n",
                   (long)tx, (long)tz, fl, (long)ox2, (long)oz2, (long)cx2, (long)cz2);
        }
    }
    int cells = bfs(bsx, bsz);
    printf("   erreichbare Zellen: %d (Raster 75)\n", cells);
    printf("   Arm | Heimat            | Landepunkt        | minAbstHeimat | minAbstLandepkt | Tor1200?\n");
    int any_gate = 0;
    for (int i = 0; i < n; i++) {
        int32_t rx = 0, rz = 0;
        int32_t dh = nearest_reachable(hx[i], hz[i], NULL, NULL);
        int32_t de = nearest_reachable(ex[i], ez[i], &rx, &rz);
        printf("   %3d | (%7ld,%7ld) | (%7ld,%7ld) | %13ld | %15ld | %s  (naechste Zelle %ld,%ld)\n",
               slots[i], (long)hx[i], (long)hz[i], (long)ex[i], (long)ez[i],
               (long)dh, (long)de, de < 0x4b0 ? "JA" : "nein", (long)rx, (long)rz);
        if (de < 0x4b0) any_gate++;
    }
    printf("   -> %d von %d Armen kommen dem begehbaren Boden auf unter 1200 nahe "
           "(RE2-Griff-Tor sltiu 0x4b0 @0x801018f4)\n", any_gate, n);

    /* ---- MESSUNG D: Flur-Durchlaeufe MIT Kollision, mehrere Spuren -------------------- */
    printf("\n-- MESSUNG D: Flur-Durchlauf MIT Kollision (Schritt 75), Spur an die jeweilige\n"
           "   Wand gedrueckt (Seitendruck 75/Bild). Kein Teleport --\n");
    for (int lane = 0; lane < 3; lane++) {
        int32_t side = (lane == 0) ? -75 : (lane == 1) ? +75 : 0;
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_player_victim_reset();
        re15_damage_seed_rng(0x0badf00du);
        g_current_room_id = 0x1210;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
        re15_collision_set_band(0);
        scd_register_room_events(&s_rdt); scd_room_reenter(&s_rdt, 0, 0, 0);
        for (int f = 0; f < 8; f++) frame_step();
        n = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x1A) slots[n++] = s;
        pl->x = -22400; pl->z = -6500;
        g_test_room_se_n = 0;
        int phase[8]; memset(phase, 0, sizeof phase);
        int32_t mind = 0x7fffffff; int grabs = 0;
        for (int f = 0; f < 400; f++) {
            int32_t nx = pl->x + side, nz = pl->z - 75;
            re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            if (!re15_player_is_grabbed()) { pl->x = nx; pl->z = nz; }
            pl->hp = 100;
            frame_step();
            for (int i = 0; i < n; i++) {
                re15_actor_t *e = &g_actors[slots[i]];
                if (e->sub_state_1 < 8) phase[e->sub_state_1]++;
                if (e->sub_state_1 == 4 && e->sub_state_2 == 0) grabs++;
                int32_t d = dist2d(pl->x, pl->z, e->x, e->z);
                if (d < mind) mind = d;
            }
        }
        printf("   Spur %s: Endposition (%ld,%ld) | Arm-Bilder je Phase:",
               lane == 0 ? "WEST-Wand" : lane == 1 ? "OST-Wand " : "Mitte    ",
               (long)pl->x, (long)pl->z);
        for (int p = 0; p < 8; p++) if (phase[p]) printf(" %d:%d", p, phase[p]);
        printf(" | kleinster Abstand %ld | Griff-Eintritte %d | SE-Aufrufe %d\n",
               (long)mind, grabs, g_test_room_se_n);
    }

    /* ---- MESSUNG E/F: bestmoegliche Annaeherung aus einer BELEGT erreichbaren Zelle ---- */
    printf("\n-- MESSUNG E/F: aus der per BFS erreichbaren Zelle, die dem Landepunkt am naechsten\n"
           "   liegt, laeuft der Spieler MIT Kollision auf den Arm zu (600 Bilder) --\n");
    int narms = n;
    for (int i = 0; i < narms; i++) {
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_player_victim_reset();
        re15_damage_seed_rng(0x0badf00du);
        g_current_room_id = 0x1210;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
        re15_collision_set_band(0);
        scd_register_room_events(&s_rdt); scd_room_reenter(&s_rdt, 0, 0, 0);
        for (int f = 0; f < 8; f++) frame_step();
        int sl = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x1A && g_actors[s].x == hx[i]
                && g_actors[s].z == hz[i]) { sl = s; break; }
        if (sl < 0) { printf("   Arm %d nicht wiedergefunden\n", i); continue; }
        re15_actor_t *e = &g_actors[sl];
        int32_t rx = 0, rz = 0;
        (void)nearest_reachable(ex[i], ez[i], &rx, &rz);
        pl->x = rx; pl->z = rz;
        g_test_room_se_n = 0;
        int32_t mind = 0x7fffffff; int grab_frame = -1, cd_at_grab = -1, phase4 = 0;
        int prev4 = 0;
        for (int f = 0; f < 600; f++) {
            int32_t tx = pl->x, tz = pl->z;
            int32_t ddx = e->x - pl->x, ddz = e->z - pl->z;
            int32_t ad = dist2d(pl->x, pl->z, e->x, e->z);
            if (ad > 0) { tx += (int32_t)((int64_t)ddx * 75 / ad); tz += (int32_t)((int64_t)ddz * 75 / ad); }
            re15_collision_constrain(&s_rdt, pl->x, pl->z, &tx, &tz);
            re15_collision_objects(&tx, &tz);
            if (!re15_player_is_grabbed()) { pl->x = tx; pl->z = tz; }
            pl->hp = 100;
            frame_step();
            int32_t d = dist2d(pl->x, pl->z, e->x, e->z);
            if (d < mind) mind = d;
            if (e->sub_state_1 == 4) {
                phase4++;
                if (!prev4 && grab_frame < 0) { grab_frame = f; cd_at_grab = e->re2z_cd239; }
                prev4 = 1;
            } else prev4 = 0;
        }
        printf("   Arm slot %2d (Start %ld,%ld): kleinster Abstand %5ld | Phase4-Bilder %3d | "
               "erster Griff Bild %4d | +0x239 dabei %3d -> vorgeschlagener SE %s | SE %d\n",
               sl, (long)rx, (long)rz, (long)mind, phase4, grab_frame, cd_at_grab,
               (grab_frame >= 0) ? (cd_at_grab == 0 ? "KLINGT" : "VERSCHLUCKT") : "-",
               g_test_room_se_n);
    }

    /* ---- MESSUNG G: die begehbare HUELLE + Spur-Durchlaeufe aus BELEGT erreichbaren
     * Startzellen. MESSUNG E laesst den Spieler AUF den Arm zulaufen und setzt ihn damit
     * HINTER den Lunge-Landepunkt (die Lunge ueberholt ihn) — Artefakt meiner eigenen
     * Sonde. Hier laeuft er den Flur ab wie ein Spieler. */
    printf("\n-- MESSUNG G1: begehbare Huelle je z-Zeile (aus der BFS-Menge) --\n");
    for (int z = 0; z < GH; z += 4) {
        int lo = -1, hi = -1;
        for (int x = 0; x < GW; x++) if (s_vis[z * GW + x]) { if (lo < 0) lo = x; hi = x; }
        if (lo < 0) continue;
        printf("   z=%7ld : x von %7ld bis %7ld\n", (long)(GZ0 + z * GS),
               (long)(GX0 + lo * GS), (long)(GX0 + hi * GS));
    }

    printf("\n-- MESSUNG G2: Spur-Durchlaeufe x = const (Start = noerdlichste erreichbare Zelle\n"
           "   der Spur), z abwaerts, jede Bewegung durch die Kollision --\n");
    printf("   Spur x | Start z | Bilder | Phasen 0/1/2/4 | min.Abstand | Griffe | SE (Ids)\n");
    for (int cxi = 0; cxi < GW; cxi++) {
        int32_t lane_x = GX0 + cxi * GS;
        int top = -1;
        for (int z = GH - 1; z >= 0; z--) if (s_vis[z * GW + cxi]) { top = z; break; }
        if (top < 0) continue;
        if ((cxi % 4) != 0) continue;                 /* jede 4. Spur = 300 Einheiten Raster */
        int32_t start_z = GZ0 + top * GS;
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_player_victim_reset();
        re15_damage_seed_rng(0x0badf00du);
        g_current_room_id = 0x1210;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
        re15_collision_set_band(0);
        scd_register_room_events(&s_rdt); scd_room_reenter(&s_rdt, 0, 0, 0);
        for (int f = 0; f < 8; f++) frame_step();
        n = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x1A) slots[n++] = s;
        pl->x = lane_x; pl->z = start_z;
        g_test_room_se_n = 0;
        int phase[8]; memset(phase, 0, sizeof phase);
        int32_t mind = 0x7fffffff; int grabs = 0, moved_frames = 0;
        for (int f = 0; f < 320; f++) {
            int32_t nx = lane_x, nz = pl->z - 75;
            int32_t px = pl->x, pz = pl->z;
            re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            if (!re15_player_is_grabbed()) { pl->x = nx; pl->z = nz; }
            if (pl->z != pz || pl->x != px) moved_frames++;
            pl->hp = 100;
            frame_step();
            for (int i = 0; i < n; i++) {
                re15_actor_t *e = &g_actors[slots[i]];
                if (e->sub_state_1 < 8) phase[e->sub_state_1]++;
                if (e->sub_state_1 == 4 && e->sub_state_2 == 0) grabs++;
                int32_t d = dist2d(pl->x, pl->z, e->x, e->z);
                if (d < mind) mind = d;
            }
        }
        printf("   %7ld | %7ld | %6d | %d/%d/%d/%d | %11ld | %6d | %d (",
               (long)lane_x, (long)start_z, moved_frames,
               phase[0], phase[1], phase[2], phase[4], (long)mind, grabs, g_test_room_se_n);
        for (int k = 0; k < g_test_room_se_n && k < 14; k++) printf("%d ", g_test_room_se_log[k]);
        printf(")\n");
    }


    /* ---- MESSUNG H: die Griff-Geometrie, die der Port im natuerlichen Ablauf herstellt ----
     * Der erzwungene Aufbau (test_1210_gitterhaende (4) und probe_1210_sound MESSUNG B)
     * setzt den Arm OHNE die Lunge auf Sub 2 und stellt den Spieler 900 Einheiten vor die
     * HEIMAT-Position. Im natuerlichen Ablauf wird Sub 2 aber erst NACH der Lunge erreicht —
     * der Arm steht dann 2420 Einheiten weiter im Flur. H misst beides:
     *   H1: steht der Punkt "vor dem GELUNGENEN (also ausgefahrenen) Arm" ueberhaupt auf
     *       begehbarem Boden?
     *   H2: der Arm laeuft seinen NATUERLICHEN Weg (Ruhe -> Lunge -> Greifen), der Spieler
     *       steht auf dem naechstgelegenen begehbaren Punkt seiner Reihe. Protokolliert wird
     *       fuer jedes Bild in Sub 2: Abstand und die BEIDEN Halb-Sektor-Tests. */
    printf("\n-- MESSUNG H1: begehbarer Boden vor dem AUSGEFAHRENEN Arm (Heimat + 2420 + d) --\n");
    for (int i = 0; i < narms; i++) {
        re15_actor_t *e0 = &g_actors[slots[i]];
        int32_t fs = re15_sin_q12(((int)e0->rot_y + 0x400) & 0xfff);
        int32_t fc = re15_cos_q12(((int)e0->rot_y + 0x400) & 0xfff);
        printf("   Arm %2d (Heimat %7ld,%7ld yaw=%4d):", slots[i], (long)hx[i], (long)hz[i],
               (int)e0->rot_y);
        for (int k = 0; k < 5; k++) {
            int32_t d = 300 + k * 300;
            int32_t px = hx[i] + (int32_t)((fs * (2420 + d)) >> 12);
            int32_t pz = hz[i] + (int32_t)((fc * (2420 + d)) >> 12);
            printf("  d=%4ld (%7ld,%7ld) Boden=%d", (long)d, (long)px, (long)pz,
                   re15_collision_on_floor(&s_rdt, px, pz));
        }
        printf("\n");
    }

    printf("\n-- MESSUNG H2: natuerlicher Ablauf, Spieler auf dem naechsten BEGEHBAREN Punkt\n"
           "   seiner Reihe; Protokoll jedes Sub-2-Bildes (Abstand + Halb-Sektoren) --\n");
    for (int i = 0; i < narms; i++) {
        /* naechster begehbarer Punkt auf der z-Zeile des Arms, in Richtung Flur */
        re15_actor_t *e0 = &g_actors[slots[i]];
        int32_t fs = re15_sin_q12(((int)e0->rot_y + 0x400) & 0xfff);
        int32_t stand_x = 0, stand_z = hz[i]; int have = 0; int32_t bestd = 0x7fffffff;
        for (int32_t xx = -30000; xx <= -8000; xx += 25) {
            if (!re15_collision_on_floor(&s_rdt, xx, stand_z)) continue;
            /* nur Punkte auf der VORDERSEITE des Arms zaehlen */
            int32_t rel = xx - hx[i];
            if ((fs > 0 && rel <= 0) || (fs < 0 && rel >= 0)) continue;
            int32_t dd = rel < 0 ? -rel : rel;
            if (!have || dd < bestd) { bestd = dd; stand_x = xx; have = 1; }
        }
        if (!have) { printf("   Arm %2d: KEIN begehbarer Punkt auf seiner Vorderseite\n", slots[i]); continue; }

        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_player_victim_reset();
        re15_damage_seed_rng(0x0badf00du);
        g_current_room_id = 0x1210;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
        re15_collision_set_band(0);
        scd_register_room_events(&s_rdt); scd_room_reenter(&s_rdt, 0, 0, 0);
        for (int f = 0; f < 8; f++) frame_step();
        int sl = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x1A && g_actors[s].x == hx[i]
                && g_actors[s].z == hz[i]) { sl = s; break; }
        if (sl < 0) continue;
        re15_actor_t *e = &g_actors[sl];
        pl->x = stand_x; pl->z = stand_z;
        g_test_room_se_n = 0;
        int sub2 = 0, grabs = 0, logged = 0; int32_t mind = 0x7fffffff;
        int cone_ok = 0;
        for (int f = 0; f < 400; f++) {
            int32_t nx = stand_x, nz = stand_z;
            re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            if (!re15_player_is_grabbed()) { pl->x = nx; pl->z = nz; }
            pl->hp = 100;
            frame_step();
            int32_t d = dist2d(pl->x, pl->z, e->x, e->z);
            if (d < mind) mind = d;
            if (e->sub_state_1 == 2) {
                sub2++;
                int bearing = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff;
                int a0 = ((int)e->rot_y + 256) & 0xfff, a1 = ((int)e->rot_y - 256) & 0xfff;
                unsigned t0 = (unsigned)((bearing - a0 + 256) & 0xfff);
                unsigned t1 = (unsigned)((bearing - a1 + 256) & 0xfff);
                int in0 = (t0 < 512), in1 = (t1 < 512);
                if (in0 || in1) cone_ok++;
                if (logged < 3) {
                    printf("      Bild %3d: Arm (%7ld,%7ld) Spieler (%7ld,%7ld) Abstand %5ld "
                           "Kegel %d/%d Tor(<1200)=%d\n", f, (long)e->x, (long)e->z,
                           (long)pl->x, (long)pl->z, (long)d, in0, in1, d < 1200);
                    logged++;
                }
            }
            if (e->sub_state_1 == 4 && e->sub_state_2 == 0) grabs++;
        }
        printf("   Arm %2d: Standpunkt (%7ld,%7ld) | Sub-2-Bilder %3d | Kegel-Treffer %3d | "
               "kleinster Abstand %5ld | GRIFFE %d | SE %d\n",
               sl, (long)stand_x, (long)stand_z, sub2, cone_ok, (long)mind, grabs,
               g_test_room_se_n);
    }


    /* ---- MESSUNG I: FAIRSTE Aufstellung — der Spieler steht GENAU im Frontkegel des
     * ausgefahrenen Arms, in Griff-Entfernung. Wenn der Griff im natuerlichen Ablauf
     * ueberhaupt moeglich ist, dann hier. Der Standpunkt wird NICHT erzwungen: er wird
     * jedes Bild per re15_collision_constrain angestrebt — wo die Kollision den Spieler
     * hinschiebt, da steht er. */
    printf("\n-- MESSUNG I: Spieler im Frontkegel des AUSGEFAHRENEN Arms (Landepunkt + d) --\n");
    printf("   Arm |    d | Wunsch-Standpunkt   | tatsaechlich         | Sub2 | Kegel | minAbst | GRIFFE | SE-Ids\n");
    for (int i = 0; i < narms; i++) {
        for (int k = 0; k < 5; k++) {
            int32_t dstand = 400 + k * 200;
            re15_actor_init(); re15_aot_init(); scd_vm_init();
            re15_enemy_reset(); re15_player_victim_reset();
            re15_damage_seed_rng(0x0badf00du);
            g_current_room_id = 0x1210;
            pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
            re15_collision_set_band(0);
            scd_register_room_events(&s_rdt); scd_room_reenter(&s_rdt, 0, 0, 0);
            for (int f = 0; f < 8; f++) frame_step();
            int sl = -1;
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == 0x1A && g_actors[s].x == hx[i]
                    && g_actors[s].z == hz[i]) { sl = s; break; }
            if (sl < 0) continue;
            re15_actor_t *e = &g_actors[sl];
            int32_t fs = re15_sin_q12(((int)e->rot_y + 0x400) & 0xfff);
            int32_t fc = re15_cos_q12(((int)e->rot_y + 0x400) & 0xfff);
            int32_t stx = hx[i] + (int32_t)((fs * (2420 + dstand)) >> 12);
            int32_t stz = hz[i] + (int32_t)((fc * (2420 + dstand)) >> 12);
            pl->x = stx; pl->z = stz;
            g_test_room_se_n = 0;
            int sub2 = 0, cone = 0, grabs = 0; int32_t mind = 0x7fffffff;
            for (int f = 0; f < 300; f++) {
                int32_t nx = stx, nz = stz;
                re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
                re15_collision_objects(&nx, &nz);
                if (!re15_player_is_grabbed()) { pl->x = nx; pl->z = nz; }
                pl->hp = 100;
                frame_step();
                int32_t d = dist2d(pl->x, pl->z, e->x, e->z);
                if (d < mind) mind = d;
                if (e->sub_state_1 == 2) {
                    sub2++;
                    int bearing = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff;
                    int a0 = ((int)e->rot_y + 256) & 0xfff, a1 = ((int)e->rot_y - 256) & 0xfff;
                    unsigned t0 = (unsigned)((bearing - a0 + 256) & 0xfff);
                    unsigned t1 = (unsigned)((bearing - a1 + 256) & 0xfff);
                    if (t0 < 512 || t1 < 512) cone++;
                }
                if (e->sub_state_1 == 4 && e->sub_state_2 == 0) grabs++;
            }
            printf("   %3d | %4ld | (%7ld,%7ld) | (%7ld,%7ld) | %4d | %5d | %7ld | %6d | ",
                   sl, (long)dstand, (long)stx, (long)stz, (long)pl->x, (long)pl->z,
                   sub2, cone, (long)mind, grabs);
            for (int q = 0; q < g_test_room_se_n && q < 10; q++) printf("%d ", g_test_room_se_log[q]);
            printf("\n");
        }
    }


    /* ---- MESSUNG J: Bild-fuer-Bild-Protokoll EINES Falles aus MESSUNG I (Arm 4, d = 800),
     * um zu sehen, WANN und mit welchem Abstand der Griff im natuerlichen Ablauf zuschlaegt.
     * Der Abstand wird wie im Port gerechnet (re15_enemy_player_dist: int16-Deltas). */
    printf("\n-- MESSUNG J: Bild-Protokoll Arm 4, Standpunkt Landepunkt+800 --\n");
    {
        int i = 3;   /* Arm 4 = Index 3 */
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_player_victim_reset();
        re15_damage_seed_rng(0x0badf00du);
        g_current_room_id = 0x1210;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
        re15_collision_set_band(0);
        scd_register_room_events(&s_rdt); scd_room_reenter(&s_rdt, 0, 0, 0);
        for (int f = 0; f < 8; f++) frame_step();
        int sl = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x1A && g_actors[s].x == hx[i]
                && g_actors[s].z == hz[i]) { sl = s; break; }
        re15_actor_t *e = &g_actors[sl];
        int32_t fs = re15_sin_q12(((int)e->rot_y + 0x400) & 0xfff);
        int32_t fc = re15_cos_q12(((int)e->rot_y + 0x400) & 0xfff);
        int32_t stx = hx[i] + (int32_t)((fs * (2420 + 800)) >> 12);
        int32_t stz = hz[i] + (int32_t)((fc * (2420 + 800)) >> 12);
        pl->x = stx; pl->z = stz;
        g_test_room_se_n = 0;
        int last_se = 0;
        for (int f = 0; f < 70; f++) {
            int32_t nx = stx, nz = stz;
            re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
            re15_collision_objects(&nx, &nz);
            if (!re15_player_is_grabbed()) { pl->x = nx; pl->z = nz; }
            pl->hp = 100;
            frame_step();
            int32_t dx16 = (int32_t)(int16_t)(pl->x - e->x);
            int32_t dz16 = (int32_t)(int16_t)(pl->z - e->z);
            int32_t dport = (int32_t)re15_squareroot0((uint32_t)(dx16 * dx16 + dz16 * dz16));
            int bearing = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff;
            int a0 = ((int)e->rot_y + 256) & 0xfff, a1 = ((int)e->rot_y - 256) & 0xfff;
            unsigned t0 = (unsigned)((bearing - a0 + 256) & 0xfff);
            unsigned t1 = (unsigned)((bearing - a1 + 256) & 0xfff);
            printf("   f=%3d Arm(%7ld,%7ld) +0x5=%u +0x6=%u | Spieler(%7ld,%7ld) | dist_port=%5ld "
                   "Kegel %d/%d | Opfer=%d | SE seit letztem Bild:",
                   f, (long)e->x, (long)e->z, (unsigned)e->sub_state_1, (unsigned)e->sub_state_2,
                   (long)pl->x, (long)pl->z, (long)dport, (t0 < 512), (t1 < 512),
                   re15_player_victim_state());
            for (int q = last_se; q < g_test_room_se_n; q++) printf(" SE%d", g_test_room_se_log[q]);
            last_se = g_test_room_se_n;
            printf("\n");
        }
    }

    free(buf);
    return 0;
}
