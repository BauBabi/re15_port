/* probe_advF_ejectcam_1060.c — ADVERSARIALE GEGENPROBE zu LANE F (kein ctest).
 *
 * Zweck: die Kausalbehauptung F-LANEF-01 ("der FINALIZE-Versatz des Ports schluckt
 * die Kamerazone") mit dem ECHTEN Engine-Scan (re15_aot_scan) statt mit einem
 * sonden-lokalen Nachbau von FUN_80014230 pruefen, und einen TRIAL-PATCH fahren:
 *
 *   Modus A  = Port wie ausgeliefert (ein Scan-Punkt pro Tick, inkl. Finalize-Sprung)
 *   Modus B  = Fix-Kandidat: der Eject-Pfad wird zusaetzlich in 100u-Schritten
 *              abgetastet (F-LANEF-01 proposed_fix)
 *   Modus C  = Trial-Patch "KEIN Eject": der Spieler bleibt nach dem Finalize-Tick auf
 *              der Gangart-Endposition (= das, was LAB_80038e50 @0x80038e50..0x80038eec
 *              tut: KEIN Store nach playerX 0x800aca88 / playerZ 0x800aca90) und
 *              laeuft von dort unter Pad-Kontrolle weiter.
 *
 * Trigger-Geometrie: die Treppen-Records tragen flags 0x31 -> NUR der 620u-Vorwaerts-
 * sonden-Test (`ori v0,zero,0x26c` @0x80042bd0, Gate `andi v0,v1,0x20` @0x80042ef8);
 * der Positions-Test haengt an Bit 0x40 @0x80042ea8 und ist AUS. Der Spieler kann die
 * Treppe also bis zu 620u VOR dem Rechteck ausloesen.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_aot.h"
#include "re15_emd.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static re15_rdt_t g_rdt;
static re15_emd_skeleton_t  g_skel;
static re15_emd_animation_t g_anim;
static int g_skel_ok = 0;

static int g_cut = 0;
static int g_chain[128];
static int g_nchain = 0;

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

/* ECHTER Engine-Scan (aot_common.c re15_aot_scan) — kein Sonden-Nachbau. */
static void escan(int32_t x, int32_t z)
{
    g_scd.cut_auto_enabled = 1;
    re15_aot_scan(x, z, (uint8_t)g_cut);
    if (g_scd.cam_change_pending) {
        int nc = (int)g_scd.cam_id;
        g_scd.cam_change_pending = 0;
        if (nc != g_cut) { g_cut = nc; if (g_nchain < 128) g_chain[g_nchain++] = nc; }
    }
}

static void chain_reset(int cut0)
{
    g_cut = cut0; g_nchain = 0; g_chain[g_nchain++] = cut0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
}

static void chain_print(void)
{
    for (int i = 0; i < g_nchain && i < 12; i++) printf("%s%d", i ? "->" : "", g_chain[i]);
    if (g_nchain > 12) printf("->...(%d)", g_nchain);
}

/* Lauf 40u/Frame mit Scan pro Frame (40 << 1000u Zonentiefe).
 * order 0 = X zuerst, dann Z; order 1 = Z zuerst, dann X; order 2 = diagonal. */
static void walk_to(re15_actor_t *pl, int32_t tx, int32_t tz, int order)
{
    for (int leg = 0; leg < 2; leg++) {
        int do_x = (order == 0) ? (leg == 0) : (leg == 1);
        for (int i = 0; i < 600; i++) {
            if (order == 2) {
                int32_t dx = tx - pl->x, dz = tz - pl->z;
                int32_t ax = dx < 0 ? -dx : dx, az = dz < 0 ? -dz : dz;
                if (ax + az == 0) return;
                if (ax > 40) pl->x += (dx > 0 ? 40 : -40); else pl->x += dx;
                if (az > 40) pl->z += (dz > 0 ? 40 : -40); else pl->z += dz;
            } else if (do_x) {
                int32_t dx = tx - pl->x; int32_t ax = dx < 0 ? -dx : dx;
                if (ax == 0) break;
                if (ax > 40) pl->x += (dx > 0 ? 40 : -40); else pl->x += dx;
            } else {
                int32_t dz = tz - pl->z; int32_t az = dz < 0 ? -dz : dz;
                if (az == 0) break;
                if (az > 40) pl->z += (dz > 0 ? 40 : -40); else pl->z += dz;
            }
            escan(pl->x, pl->z);
        }
        if (order == 2) return;
    }
}

/* Eine Treppe. mode: 0=A(Port), 1=B(Eject-Pfad mitscannen), 2=C(kein Eject). */
static int do_stair(re15_actor_t *pl, int band, int32_t cx, int32_t sz,
                    int down_plus, int mode, int32_t *out_fin,
                    int32_t *out_pre, int32_t *out_post)
{
    pl->x = cx; pl->z = sz;
    pl->y = -(int32_t)band * 0x708;
    pl->rot_y = (int16_t)(down_plus ? 3072 : 1024);
    re15_collision_set_band(band);
    re15_stair_reset();
    escan(pl->x, pl->z);
    if (!re15_stair_try_start(&g_rdt, 1)) return 0;
    while (re15_stair_active()) {
        int32_t px = pl->x, pz = pl->z;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        int last = !re15_stair_active();
        if (last) {
            int32_t dx = pl->x - px, dz = pl->z - pz;
            int32_t d = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            if (out_fin) *out_fin = d;
            if (out_pre) *out_pre = pz;
            if (out_post) *out_post = pl->z;
            if (mode == 2) { pl->x = px; pl->z = pz; escan(pl->x, pl->z); break; }
            if (mode == 1) {
                int32_t adx = dx < 0 ? -dx : dx, adz = dz < 0 ? -dz : dz;
                int steps = (int)((adx + adz) / 100) + 1;
                for (int s = 1; s <= steps; s++)
                    escan(px + (int32_t)((int64_t)dx * s / steps),
                          pz + (int32_t)((int64_t)dz * s / steps));
                break;
            }
        }
        escan(pl->x, pl->z);
    }
    return 1;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    {
        char p2[600]; size_t s1 = 0, s2 = 0;
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EDD", base);
        uint8_t *edd = read_file(p2, &s1);
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EMR", base);
        uint8_t *emr = read_file(p2, &s2);
        if (edd && emr && re15_emd_parse_animation(edd, s1, &g_anim) == 0 &&
            re15_emd_parse_skeleton(emr, s2, &g_skel) == 0) g_skel_ok = 1;
    }
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("FAIL: %s\n", path); return 1; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { printf("FAIL parse\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 25150; pl->z = 24000; pl->y = -8 * 0x708;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    const char *mname[3] = { "A Port", "B Fix(Eject-Pfad scannen)", "C Trial(kein Eject)" };

    /* ================= TEIL 1: NUR die letzte Treppe, Start-Cut 4 =============
     * West-Treppe (side=1, corner 20700, extent 4300), abwaerts +Z, Band 2 -> 0.
     * Danach Lauf zur Tuer (24000,26000): RVD[16] @0x02a0 cam_from=5 -> 7. */
    printf("=== TEIL 1: LETZTE Treppe (Band2->0), Start-Cut 4, ECHTER Engine-Scan ===\n");
    printf("    Einstieg 20080..21500 (620u-Sonde @0x80042bd0 laesst Start VOR dem Rect zu)\n");
    int failA = 0, failB = 0, failC = 0, nstart = 0;
    int32_t winlo = 0, winhi = 0;
    for (int32_t sz = 20080; sz <= 21500; sz += 10) {
        int res[3]; int32_t fin = 0, pre = 0, post = 0;
        int started = 1;
        for (int mode = 0; mode < 3; mode++) {
            chain_reset(4);
            if (!do_stair(pl, 2, 21400, sz, 1, mode, &fin, &pre, &post)) { started = 0; break; }
            walk_to(pl, 24000, 26000, 0);
            res[mode] = g_cut;
        }
        if (!started) continue;
        nstart++;
        if (res[0] != 7) { failA++; if (!winlo) winlo = sz; winhi = sz; }
        if (res[1] != 7) failB++;
        if (res[2] != 7) failC++;
        if (res[0] != res[1] || res[0] != res[2] || res[0] != 7)
            printf("   z=%5ld  FIN=%4ld (z %ld->%ld)  A=%d  B=%d  C=%d %s\n",
                   (long)sz, (long)fin, (long)pre, (long)post, res[0], res[1], res[2],
                   res[0] != 7 ? "<<< A FALSCH" : "");
    }
    printf("   -> gestartete Einstiege: %d ; A falsch: %d (Fenster z=%ld..%ld) ; B falsch: %d ; C falsch: %d\n\n",
           nstart, failA, (long)winlo, (long)winhi, failB, failC);

    /* ================= TEIL 2: volle Kette, zwei Laufwege ==================== */
    printf("=== TEIL 2: volle Kette 4 Treppen + Laufwege (Laufweg-Reihenfolge variiert) ===\n");
    struct { int band; int32_t cx, top; int down_plus; } S[4] = {
        { 8, 25150, 24200, 0 }, { 6, 21400, 20700, 1 },
        { 4, 25150, 24200, 0 }, { 2, 21400, 20700, 1 },
    };
    for (int order = 0; order <= 1; order++) {
        printf("  Laufweg-Reihenfolge = %s\n", order ? "Z zuerst, dann X" : "X zuerst, dann Z");
        for (int32_t off = -600; off <= 600; off += 100) {
            printf("   Einstiegs-Offset %+5ld u:", (long)off);
            for (int mode = 0; mode < 3; mode++) {
                chain_reset(0);
                int ok = 1;
                for (int s = 0; s < 4 && ok; s++) {
                    int32_t sz = S[s].top + (S[s].down_plus ? +off : -off);
                    ok = do_stair(pl, S[s].band, S[s].cx, sz, S[s].down_plus, mode, NULL, NULL, NULL);
                    if (ok && s < 3) {
                        int32_t nz = S[s+1].top + (S[s+1].down_plus ? +off : -off);
                        walk_to(pl, S[s+1].cx, nz, order);
                    }
                }
                if (!ok) { printf("  %s=<kein Start>", mname[mode]); continue; }
                walk_to(pl, 24000, 26000, order);
                printf("  %s=%d", mname[mode], g_cut);
            }
            printf("\n");
        }
    }
    return 0;
}
