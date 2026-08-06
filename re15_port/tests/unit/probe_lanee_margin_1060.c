/* probe_lanee_margin_1060.c — LANE E MESSSONDE (kein ctest)
 *
 * Die ENTSCHEIDENDE Messung fuer "Original-Quirk oder Port-Fehler?".
 *
 * BEFUND aus probe_lanee_reach_1060: in ROOM1060 schneidet AUF DEN BAENDERN 2
 * UND 6 KEINE EINZIGE RVD-Zone die begehbare Flaeche. Die Umschaltungen
 *   RVD[10] 3->4  x[23000..27500] z[20000..21000]   (waehrend Treppe 3)
 *   RVD[13] 4->5  x[19500..23000] z[24000..25000]   (waehrend Treppe 4)
 * koennen also AUSSCHLIESSLICH WAEHREND DES TREPPEN-ABSTIEGS feuern. Es gibt
 * keinen zweiten Weg, keine Nachhol-Chance durch Herumlaufen.
 *
 * Damit haengt die Kamera-Kette allein an der TRAJEKTORIE des Abstiegs.
 * Diese Sonde misst deshalb:
 *   (1) den echten, per Kollision erreichbaren Ausloese-Punkt (der Spieler wird
 *       mit re15_collision_constrain an die Treppe herangeschoben),
 *   (2) Start-z, Gait-End-z und den PORT-EIGENEN Finalize-Versatz getrennt,
 *   (3) ob der Ziel-Streifen vom GAIT oder erst vom EJECT ueberdeckt wird —
 *       und ob der EJECT ihn UEBERSPRINGT.
 *
 * Warum das die Frage entscheidet: LAB_80038e50 (@0x80038e50-0x80038eec, das
 * Finalize des Abstiegs-Handlers) hat NACHWEISLICH KEINEN Store auf
 * player+0x34/+0x38/+0x3c — disassembliert sind dort nur
 *   0x80038e68 sb 2 -> 0x800acae8 (Clip 2)
 *   0x80038e70 sb 0 -> 0x800acae9 (Cursor)
 *   0x80038e78 sb 0 -> 0x800acae3
 *   0x80038e7c jal 0x8001f314   (anim_set)
 *   0x80038ea8 sb 0 -> 0x800aca59 / 0x80038eb0 sb 0 -> 0x800aca5a / 0x80038eb4 sb 0 -> 0x800aca5b
 *   0x80038ec0 sb (hit_react & 0xfe) -> player+0x93
 *   0x80038edc sb (-DAT_800acc0e / 0x708) -> player.floor (+0x82)
 * Das Original VERSETZT den Spieler im Finalize also NICHT. Jeder XZ-Sprung
 * dort ist eine PORT-Erfindung (stair_common.c, 48 Schritte x 100u).
 * Der RVD-Scan FUN_80014230 ist ein PUNKT-Test (@0x80014298 FUN_80014368) und
 * laeuft jeden Frame (@0x8001ccec) — ein Sprung ueber einen 1000 Einheiten
 * tiefen Streifen laesst die Zone STUMM.
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

static int is_free(int32_t x, int32_t z)
{
    int32_t cx = x, cz = z;
    re15_collision_constrain(&g_rdt, x, z, &cx, &cz);
    return (cx == x && cz == z);
}

/* Aeusserster begehbarer z-Wert bei festem x auf `band`, in Richtung `dir`
 * (dir=-1 -> kleinstes z einer zusammenhaengenden freien Spanne von z0 aus). */
static int32_t edge_z(int32_t x, int32_t z0, int dir, int band)
{
    re15_collision_set_band(band);
    int32_t z = z0;
    /* erst eine freie Zelle finden */
    int found = 0;
    for (int i = 0; i < 400 && !found; i++) {
        if (is_free(x, z0 + (int32_t)dir * 10 * i)) { z = z0 + (int32_t)dir * 10 * i; found = 1; }
    }
    if (!found) return z0;
    while (is_free(x, z + dir * 10)) z += dir * 10;
    return z;
}

/* Ein Abstieg mit voller Buchfuehrung. */
static void run_stair(const char *label, int32_t sx, int32_t sz, int rot, int band,
                      int cut_in, int zone_idx)
{
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const re15_rdt_zone_t *Z = &g_rdt.zones[zone_idx];
    int32_t zmn = Z->zs[0], zmx = Z->zs[0];
    for (int k = 1; k < 4; k++) { if (Z->zs[k] < zmn) zmn = Z->zs[k]; if (Z->zs[k] > zmx) zmx = Z->zs[k]; }

    p->active = 1; p->type = 0; p->hp = 100;
    p->x = sx; p->z = sz; p->y = -(int32_t)band * 0x708; p->rot_y = (int16_t)rot;
    p->motion = 0; p->anim_frame = 0; p->anim_flags = 0;
    re15_collision_set_band(band);
    re15_stair_reset();
    int cut = cut_in;
    g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;

    if (!re15_stair_try_start(&g_rdt, 1)) {
        printf("  %-42s startZ=%6ld  KEIN STAIR-START\n", label, (long)sz);
        return;
    }
    int gait_in_zone = 0, eject_over_zone = 0, n = 0, fired = 0, fired_frame = -1;
    int32_t gz = p->z, gx = p->x;
    while (re15_stair_active() && n < 900) {
        int fin_before = !re15_stair_active();
        (void)fin_before;
        gz = p->z; gx = p->x;
        int32_t bz = p->z;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        int32_t az = p->z;
        int inside = re15_aot_point_in_quad(p->x, p->z, Z->xs, Z->zs);
        int still = re15_stair_active();
        if (inside && still) gait_in_zone++;
        /* Sprung ueber den Streifen? (Strecke bz->az deckt den Streifen ab,
         * beide Endpunkte aber ausserhalb) */
        int32_t lo = bz < az ? bz : az, hi = bz < az ? az : bz;
        if (!inside && lo < zmn && hi > zmx) {
            if (!still) eject_over_zone = 1;
        }
        g_scd.cut_auto_enabled = 1;
        re15_aot_scan(p->x, p->z, (uint8_t)cut);
        if (g_scd.cam_change_pending) {
            int nc = (int)g_scd.cam_id; g_scd.cam_change_pending = 0;
            if (nc != cut) { cut = nc; if (!fired) { fired = 1; fired_frame = n; } }
        }
        n++;
    }
    int32_t ejz = p->z - gz, ejx = p->x - gx;
    printf("  %-42s startZ=%6ld gaitEnd=%6ld (dz=%+6ld) EJECT=%+6ld/%+5ld end=%6ld | "
           "Zone[%d] z[%ld..%ld]: gait-frames drin=%d %s | cut %d->%d %s\n",
           label, (long)sz, (long)gz, (long)(gz - sz), (long)ejz, (long)ejx, (long)p->z,
           zone_idx, (long)zmn, (long)zmx, gait_in_zone,
           eject_over_zone ? "EJECT SPRINGT DRUEBER!" : "",
           cut_in, cut,
           (cut == (int)Z->cam_to) ? "OK" : "<<< KAMERA SCHALTET NICHT");
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t size = 0;
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
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s\n", path); return 1; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { fprintf(stderr, "FAIL parse\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 26000; pl->y = -8 * 0x708; pl->z = 25300;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    /* --- Wo steht der Spieler wirklich, wenn er die Treppe ausloest? --- */
    printf("\n== Erreichbare Ausloese-Punkte (Kollisions-Anlauf) ==\n");
    struct { const char *n; int32_t x, z0; int dir; int band; } app[] = {
        { "Band 8 Ost,  Rand -z (Treppe 1)", 25150, 26400, -1, 8 },
        { "Band 6 West, Rand -z (Treppe 2)", 21400, 20200, -1, 6 },
        { "Band 4 Ost,  Rand -z (Treppe 3)", 25150, 26400, -1, 4 },
        { "Band 2 West, Rand +z (Treppe 4)", 21400, 19000, +1, 2 },
    };
    int32_t trig_z[4];
    for (int i = 0; i < 4; i++) {
        trig_z[i] = edge_z(app[i].x, app[i].z0, app[i].dir, app[i].band);
        printf("  %-36s -> aeusserste begehbare Zelle z=%ld  (Sonde 620 voraus -> z=%ld)\n",
               app[i].n, (long)trig_z[i],
               (long)(trig_z[i] + (app[i].dir < 0 ? -620 : 620)));
    }

    /* --- Wo liegt die begehbare Flaeche des ZIEL-Bandes? Daraus folgt, wie weit
     *     der Abstieg im ORIGINAL tragen MUSS (das Original versetzt im Finalize
     *     nicht: LAB_80038e50 @0x80038e50-0x80038eec hat keinen Store auf
     *     player+0x34/+0x38/+0x3c). --- */
    printf("\n== Soll-Reichweite der Abstiege (Start-Rand -> Ziel-Rand) ==\n");
    {
        struct { const char *n; int32_t x; int32_t from_z0; int from_dir; int from_band;
                 int32_t to_z0; int to_dir; int to_band; } q[] = {
          { "T1 Ost  8->6", 25150, 26400, -1, 8, 16000, +1, 6 },
          { "T2 West 6->4", 21400, 19000, +1, 6, 26400, -1, 4 },
          { "T3 Ost  4->2", 25150, 26400, -1, 4, 16000, +1, 2 },
          { "T4 West 2->0 (LETZTE)", 21400, 19000, +1, 2, 26400, -1, 0 },
        };
        for (unsigned i = 0; i < sizeof q / sizeof q[0]; i++) {
            int32_t a = edge_z(q[i].x, q[i].from_z0, q[i].from_dir, q[i].from_band);
            int32_t b = edge_z(q[i].x, q[i].to_z0,   q[i].to_dir,   q[i].to_band);
            int32_t need = b - a; if (need < 0) need = -need;
            printf("  %-22s Start-Rand z=%6ld (Band %d) -> Ziel-Rand z=%6ld (Band %d)"
                   "  SOLL=%ld   PORT-Gait=3761   Fehlbetrag=%ld\n",
                   q[i].n, (long)a, q[i].from_band, (long)b, q[i].to_band,
                   (long)need, (long)(need - 3761));
        }
    }

    /* Ausloese-Fenster: der 620-Sondenpunkt (STAIR_FWDPROBE, `ori v0,zero,0x26c`
     * @0x80042bd0) muss im Record-Rechteck liegen UND der Spieler auf einer
     * begehbaren Zelle des Bandes stehen.
     *   Ost-Rechteck (slots 3/4/7/8): z 19800..24200, Abstieg -z -> z in [20420,24820]
     *   West-Rechteck (slots 5/6/9/10): z 20700..25000, Abstieg +z -> z in [20080,24380]
     * geschnitten mit der begehbaren Spanne des jeweiligen Bandes. */

    printf("\n== Treppe 1 Ost Band 8->6, cut_in=0, Zielzone RVD[1] 0->1 z[21500..22500] ==\n");
    for (int32_t z = trig_z[0]; z <= 24820; z += 20) {
        char lab[80]; snprintf(lab, sizeof lab, "T1 z=%ld", (long)z);
        run_stair(lab, 25150, z, 1024, 8, 0, 1);
    }

    printf("\n== Treppe 2 West Band 6->4, cut_in=1, Zielzone RVD[4] 1->2 z[22000..23000] ==\n");
    for (int32_t z = 20080; z <= trig_z[1]; z += 20) {
        char lab[80]; snprintf(lab, sizeof lab, "T2 z=%ld", (long)z);
        run_stair(lab, 21400, z, 3072, 6, 1, 4);
    }

    printf("\n== Treppe 3 Ost Band 4->2, cut_in=3, Zielzone RVD[10] 3->4 z[20000..21000] ==\n");
    printf("   (RVD[10] schneidet die begehbare Flaeche von Band 2 NICHT -> nur der Abstieg trifft sie)\n");
    for (int32_t z = trig_z[2]; z <= 24820; z += 20) {
        char lab[80]; snprintf(lab, sizeof lab, "T3 z=%ld", (long)z);
        run_stair(lab, 25150, z, 1024, 4, 3, 10);
    }

    printf("\n== Treppe 4 West Band 2->0 (LETZTE), cut_in=4, Zielzone RVD[13] 4->5 z[24000..25000] ==\n");
    printf("   (RVD[13] schneidet die begehbare Flaeche von Band 0 NICHT -> nur der Abstieg trifft sie)\n");
    for (int32_t z = 20080; z <= trig_z[3]; z += 20) {
        char lab[80]; snprintf(lab, sizeof lab, "T4 z=%ld", (long)z);
        run_stair(lab, 21400, z, 3072, 2, 4, 13);
    }
    return 0;
}
