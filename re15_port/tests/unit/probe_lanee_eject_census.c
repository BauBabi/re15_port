/* probe_lanee_eject_census.c — LANE E MESSSONDE (kein ctest)
 *
 * GAME-WEITER ZENSUS: in welchen Treppen-Raeumen kann der PORT-EIGENE
 * Finalize-Versatz (stair_common.c, bis 48 x 100u in EINEM Tick) einen
 * RVD-Kamerastreifen UEBERSPRINGEN?
 *
 * Begruendung, warum der Versatz port-eigen ist — LAB_80038e50, das Finalize
 * des Original-Abstiegs (@0x80038e50 bis `jr ra` @0x80038eec), vollstaendig
 * disassembliert; die EINZIGEN Stores sind:
 *   @0x80038e68  sb v0,-13592(at)   0x800acae8 = 2      (Standby-Clip)
 *   @0x80038e70  sb zero,-13591(at) 0x800acae9 = 0      (Clip-Cursor)
 *   @0x80038e78  sb zero,-13597(at) 0x800acae3 = 0
 *   @0x80038e7c  jal 0x8001f314                          (anim_set)
 *   @0x80038ea8  sb zero,-13735(at) 0x800aca59 = 0      (Spieler-Modus)
 *   @0x80038eb0  sb zero,-13734(at) 0x800aca5a = 0
 *   @0x80038eb4  sb zero,0(s0)      0x800aca5b = 0      (Sub-Phase)
 *   @0x80038ec0  sb v0,-13593(at)   player+0x93 &= 0xfe
 *   @0x80038edc  sb v0,-13610(at)   player+0x82 = -DAT_800acc0e / 0x708
 * KEIN Store auf player+0x34 (X) / +0x38 (Y) / +0x3c (Z).
 * Im Original bewegen den Spieler waehrend des Abstiegs NUR
 *   FUN_800245d8 (@0x80038dc4 jal): +0x34/+0x3c += rot(0,0,speed),
 *                 speed = player+0x8c = 10 (`ori v0,zero,0xa` @0x80038cc0,
 *                 `sh v0,-13600(at)` = 0x800acae0 @0x80038cc8;
 *                 Stores @0x800246e4 / @0x800246f8)                 -> <= 10/Tick
 *   FUN_800390e0 (@0x80038d50 jal, param_1=0): +0x34/+0x38/+0x3c -=
 *                 (Fuss-Weltdelta EINES Animationsframes)           -> ~<= 130/Tick
 * Beides sind Ein-Frame-Inkremente. Der RVD-Scan FUN_80014230 laeuft JEDEN
 * Frame (@0x8001ccec) und testet einen PUNKT (@0x80014298 -> FUN_80014368).
 * Das Original kann einen 1000 Einheiten tiefen Streifen also NICHT
 * ueberspringen — der Port mit seinem Ein-Tick-Versatz schon.
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

/* Streckt die Strecke a->b ab und meldet, wie viele RVD-Zonen sie UEBERSTREICHT,
 * ohne dass einer der beiden Endpunkte in ihnen liegt. */
static int swept_zones(int32_t ax, int32_t az, int32_t bx, int32_t bz,
                       int *first_zone)
{
    int32_t dx = bx - ax, dz = bz - az;
    int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
    if (ad <= 10) return 0;
    int steps = (int)(ad / 10) + 1;
    if (steps > 2000) steps = 2000;
    int n = 0;
    for (int i = 0; i < g_rdt.zone_count; i++) {
        const re15_rdt_zone_t *z = &g_rdt.zones[i];
        int32_t mnx=z->xs[0],mxx=z->xs[0];
        for (int k=1;k<4;k++){ if(z->xs[k]<mnx)mnx=z->xs[k]; if(z->xs[k]>mxx)mxx=z->xs[k]; }
        if (mxx - mnx > 8000) continue;      /* Anker-Zone (ganzer Raum) */
        if (re15_aot_point_in_quad(ax, az, z->xs, z->zs)) continue;
        if (re15_aot_point_in_quad(bx, bz, z->xs, z->zs)) continue;
        for (int s = 1; s < steps; s++) {
            int32_t sx = ax + (int32_t)(((int64_t)dx * s) / steps);
            int32_t sz = az + (int32_t)(((int64_t)dz * s) / steps);
            if (re15_aot_point_in_quad(sx, sz, z->xs, z->zs)) {
                if (n == 0 && first_zone) *first_zone = i;
                n++; break;
            }
        }
    }
    return n;
}

static void do_room(const char *base, const char *stage, const char *room, uint16_t rid)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/%s/%s.RDT", base, stage, room);
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("  %s: FEHLT\n", room); return; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { printf("  %s: PARSE-FEHLER\n", room); return; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = rid;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 0; pl->y = 0; pl->z = 0;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    int nstair = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_STAIR) nstair++;
    printf("\n--- %s (%d RVD-Zonen, %d Treppen-AOTs) ---\n", room, g_rdt.zone_count, nstair);
    if (!nstair) return;

    int worst = 0, skips = 0, runs = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_STAIR) continue;
        const re15_aot_stair_params_t *sp = &g_aot.stair_params[i];
        int band = (int)a->band;
        int zaxis = (sp->axis == 13);
        /* Vier Anlaufrichtungen probieren; nur die, die einen ABSTIEG ausloest,
         * wird gewertet. Startpunkt = Rechteckmitte +- (Halbmass - 100). */
        static const int rots[4] = { 0, 1024, 2048, 3072 };
        for (int r = 0; r < 4; r++) {
            int rot = rots[r];
            /* Startpunkt so waehlen, dass die 620-Sonde ins Rechteck zeigt:
             * am rueckwaertigen Rand des Rechtecks, entgegen der Blickrichtung. */
            /* Der Ausloese-Punkt liegt nirgends fest: der Spieler kann ueberall
             * stehen, wo die 620-Sonde ins Rechteck faellt. Darum den ganzen
             * moeglichen Bereich in 100er-Schritten abtasten. */
            int32_t half = zaxis ? a->half_h : a->half_w;
            int32_t ctr  = zaxis ? a->z : a->x;
            int32_t neg  = (rot == 1024 /* -z */ || rot == 2048 /* -x? */) ? 1 : 0;
            (void)neg;
            int hits_here = 0;
            for (int32_t off = -half - 600; off <= half + 600; off += 100) {
                int32_t sx = a->x, sz = a->z;
                if (zaxis) sz = ctr + off; else sx = ctr + off;
                pl->x = sx; pl->z = sz; pl->y = -(int32_t)band * 0x708;
                pl->rot_y = (int16_t)rot; pl->motion = 0; pl->anim_frame = 0; pl->anim_flags = 0;
                re15_collision_set_band(band);
                re15_stair_reset();
                if (!re15_stair_try_start(&g_rdt, 1)) continue;
                if (pl->motion != (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN) { re15_stair_reset(); continue; }
                runs++;
                int n = 0; int32_t gx = pl->x, gz = pl->z;
                while (re15_stair_active() && n < 900) {
                    gx = pl->x; gz = pl->z;
                    re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
                    n++;
                }
                int32_t ex = pl->x - gx, ez = pl->z - gz;
                int32_t emag = (ex < 0 ? -ex : ex) + (ez < 0 ? -ez : ez);
                int fz = -1;
                int nz = swept_zones(gx, gz, pl->x, pl->z, &fz);
                if (emag > worst) worst = emag;
                if (nz) {
                    skips++;
                    if (hits_here < 2) {
                        printf("    Slot%-2d Band%-2d rot=%-4d start=%-6ld GAIT-Ende=%-6ld"
                               " EJECT %+ld/%+ld (|%ld|) UEBERSPRINGT %d RVD-Zone(n),"
                               " zuerst RVD[%d] %d->%d\n",
                               i, band, rot, (long)(zaxis ? sz : sx), (long)(zaxis ? gz : gx),
                               (long)ex, (long)ez, (long)emag,
                               nz, fz, (int)g_rdt.zones[fz].cam_from, (int)g_rdt.zones[fz].cam_to);
                    }
                    hits_here++;
                }
            }
        }
    }
    printf("    -> %d Abstiege gefahren, groesster Ein-Tick-Finalize-Versatz = %ld,"
           " %d davon ueberspringen mindestens eine RVD-Zone\n", runs, (long)worst, skips);
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
        printf("PL00 %s\n", g_skel_ok ? "geladen" : "FEHLT!");
    }
    struct { const char *stage, *room; uint16_t rid; } rooms[] = {
        {"STAGE1","ROOM1060",0x1060}, {"STAGE1","ROOM10A0",0x10A0}, {"STAGE1","ROOM1170",0x1170},
        {"STAGE2","ROOM2030",0x2030}, {"STAGE2","ROOM2040",0x2040}, {"STAGE2","ROOM2080",0x2080},
        {"STAGE3","ROOM3010",0x3010}, {"STAGE3","ROOM3040",0x3040}, {"STAGE3","ROOM30C0",0x30C0},
        {"STAGE3","ROOM30D0",0x30D0}, {"STAGE4","ROOM4070",0x4070}, {"STAGE5","ROOM5020",0x5020},
        {"STAGE6","ROOM6010",0x6010}, {"STAGE6","ROOM6030",0x6030},
    };
    for (unsigned i = 0; i < sizeof rooms / sizeof rooms[0]; i++)
        do_room(base, rooms[i].stage, rooms[i].room, rooms[i].rid);
    return 0;
}
