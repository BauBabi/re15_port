/* probe_stair_cam_1060.c — DIAGNOSE (kein ctest): Nutzer-Report 2026-08-06
 * "Im Treppenhaus schwenkt nach dem Runterlaufen der LETZTEN Treppe die Kamera
 *  teilweise nicht richtig um."
 *
 * RAUM-IDENTIFIKATION (Tuer-Bytes, siehe Dossier):
 *   ROOM1120 Door slot0 @0x0c76 -> Raum 0x6 = ROOM1060, Spawn (26000,-14400,25300)
 *   = Band 8 (OBEN), cut 0.  ROOM1060 Door slot2 @0x0d52 band=0 -> ROOM1040.
 *   => ROOM1060 ist das Treppenhaus der aktuellen Raumkette 1120 -> 1060 -> 1040 -> 1030;
 *      man betritt es OBEN (Band 8) und laeuft VIER Treppen hinunter nach Band 0.
 *   LETZTE Treppe = West-Treppe Band 2 -> 0 (Aot_set slots 9/10 @0x0dea/0x0dfe,
 *   rect x[20200..22600] z[20700..25000], side=1, count=2).
 *
 * Die eine Kamera-Umschaltung, die diese letzte Treppe leisten MUSS, ist
 *   RVD[13] cam_from=4 -> cam_to=5, x[19500..23000] z[24000..25000]
 * und die GEGENZONE ist
 *   RVD[15] cam_from=5 -> cam_to=4, x[19500..23000] z[23000..24000].
 * Die beiden Streifen stossen bei z=24000 aneinander.
 *
 * Diese Sonde faehrt die letzte Treppe ueber den ECHTEN Port-Code
 * (re15_stair_try_start/tick mit PL00-Skelett) und laesst pro Frame den ECHTEN
 * RVD-Scan (re15_aot_scan) laufen — wie game_step_common.c:447-457 — und zwar
 * fuer einen SWEEP von Startpositionen innerhalb des Trigger-Rechtecks.
 * Gemessen wird: die vollstaendige Cut-Folge und der End-Cut.
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

static void sweep(const char *label, int32_t px, int rot, int band, int start_cut,
                  int32_t z_lo, int32_t z_hi, int32_t z_step, int expect_cut)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("\n=== %s  (x=%ld rot=%d band=%d cut_in=%d, Soll-Cut danach=%d)\n",
           label, (long)px, rot, band, start_cut, expect_cut);
    printf("  startZ | Frames | Cut-Folge                          | endZ   | endCut | ok\n");
    for (int32_t sz = z_lo; sz <= z_hi; sz += z_step) {
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = px; pl->z = sz; pl->y = -(int32_t)band * 0x708;
        pl->rot_y = (int16_t)rot;
        pl->motion = 0; pl->anim_frame = 0; pl->anim_flags = 0;
        re15_stair_reset();
        re15_collision_set_band(band);
        int cut = start_cut;
        g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 0;
        for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;

        if (!re15_stair_try_start(&g_rdt, 1)) {
            printf("  %6ld | KEIN Stair-Start\n", (long)sz); continue;
        }
        int dn = (pl->motion == (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN);
        char seq[400]; int sl = 0;
        sl += snprintf(seq + sl, sizeof seq - sl, "%d", cut);
        int n = 0, flips = 0;
        int32_t z_pre = pl->z, x_pre = pl->x;
        while (re15_stair_active() && n < 900) {
            z_pre = pl->z; x_pre = pl->x;   /* Zustand VOR dem letzten (= Finalize-)Tick */
            re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
            g_scd.cut_auto_enabled = 1;
            re15_aot_scan(pl->x, pl->z, (uint8_t)cut);
            if (g_scd.cam_change_pending) {
                int nc = (int)g_scd.cam_id; g_scd.cam_change_pending = 0;
                if (nc != cut) {
                    cut = nc; flips++;
                    if (sl < (int)sizeof seq - 12)
                        sl += snprintf(seq + sl, sizeof seq - sl, "->%d(F%d)", cut, n);
                }
            }
            n++;
        }
        /* NACH der Treppe: zur Ausgangstuer (27100,25400) marschieren (40 u/Frame,
         * Probe-Hilfsmittel) und den RVD-Scan weiterlaufen lassen. Die Kamera MUSS
         * dabei ueber RVD[16] 5->7 (x[23500..24500] z[24500..27500]) auf Cut 7
         * springen — das ist der Cut, mit dem ROOM1040s Tuer @0x10d8 hier einsetzt. */
        int32_t z_end = pl->z, x_end = pl->x;   /* Landepunkt NACH der Treppe */
        int door_cut = cut;
        if (expect_cut == 5) {
            int32_t tx2 = 27100, tz2 = 25400;
            for (int i = 0; i < 400; i++) {
                int32_t dx = tx2 - pl->x, dz2 = tz2 - pl->z;
                if ((dx < 0 ? -dx : dx) + (dz2 < 0 ? -dz2 : dz2) <= 40) break;
                pl->x += (dx > 40) ? 40 : ((dx < -40) ? -40 : dx);
                pl->z += (dz2 > 40) ? 40 : ((dz2 < -40) ? -40 : dz2);
                g_scd.cut_auto_enabled = 1;
                re15_aot_scan(pl->x, pl->z, (uint8_t)door_cut);
                if (g_scd.cam_change_pending) {
                    int nc = (int)g_scd.cam_id; g_scd.cam_change_pending = 0;
                    door_cut = nc;
                }
            }
        }
        printf("  %6ld | %s%3d | %-34s | gaitZ=%6ld dz=%+6ld | EJECT %+6ld/%+5ld | %6ld | %6d | %s%s | Tuer-Cut=%d %s\n",
               (long)sz, dn ? "" : "UP ", n, seq, (long)z_pre, (long)(z_pre - sz),
               (long)(z_end - z_pre), (long)(x_end - x_pre), (long)z_end, cut,
               cut == expect_cut ? "JA " : "NEIN",
               flips > 1 ? "  <-- PING-PONG" : "",
               (expect_cut == 5) ? door_cut : -1,
               (expect_cut == 5 && door_cut != 7) ? "<<< KAMERA SCHWENKT NICHT (Soll 7)" : "");
    }
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

    /* Treppe 1 (Ost, Band 8->6, side=0 = unten bei NIEDRIGEM z): abwaerts = -z, rot 1024.
     * Startpunkt muss in der HOHEN Haelfte liegen (z > 19800+2200 = 22000). Soll: 0 -> 1. */
    sweep("Treppe 1  Ost 8->6", 25150, 1024, 8, 0, 22100, 24100, 250, 1);
    /* Treppe 2 (West, Band 6->4, side=1 = unten bei HOHEM z): abwaerts = +z, rot 3072.
     * Startpunkt in der NIEDRIGEN Haelfte (z < 20700+2150 = 22850). Soll: 1 -> 2. */
    sweep("Treppe 2  West 6->4", 21400, 3072, 6, 1, 20800, 22800, 250, 2);
    /* Treppe 3 (Ost, Band 4->2): Soll 2 -> 3 (RVD[7]) und danach 3 -> 4 (RVD[10]). */
    sweep("Treppe 3  Ost 4->2", 25150, 1024, 4, 2, 22100, 24100, 250, 4);
    /* Treppe 4 = LETZTE (West, Band 2->0): Soll 4 -> 5 (RVD[13] z[24000..25000]). */
    sweep("Treppe 4  West 2->0 = LETZTE", 21400, 3072, 2, 4, 20800, 22800, 125, 5);
    /* SYMPTOM-TEST: dieselbe LETZTE Treppe, aber mit einem anderen einlaufenden Cut.
     * Nur cut_in=4 kann RVD[13] (4->5) ausloesen; jeder andere Cut laesst die Kamera
     * ueber die ganze letzte Treppe UND den Weg zur Tuer stehen. */
    for (int ci = 0; ci <= 7; ci++) {
        char lab[80];
        snprintf(lab, sizeof lab, "LETZTE Treppe mit cut_in=%d", ci);
        sweep(lab, 21400, 3072, 2, ci, 21500, 21500, 250, 5);
    }

    /* PORT-EIGENE TRIGGER-ZONE: player_in_zone() (STAIR_REACH 450, stair_common.c:41/92)
     * hat KEINEN PSX-Referenten — das Original testet bei flags 0x31 (Bit 0x20 gesetzt,
     * Bit 0x40 CLEAR) AUSSCHLIESSLICH den 620-Vorwaerts-Punkt gegen das exakte Rechteck
     * (@0x80042ea8 Zentrumstest / @0x80042ef8 Sondentest). z=25300 liegt 300 Einheiten
     * hinter dem Rechteck-Ende (z max 25000); die Sonde bei z+620 = 25920 liegt AUSSERHALB
     * -> Original loest NICHT aus, der Port schon (|25300-22850| = 2450 <= 2150+450). */
    sweep("PORT-ONLY Trigger z=25300 (hinter dem Rechteck), cut_in=5", 21400, 3072, 0, 5, 25300, 25300, 250, 5);
    sweep("PORT-ONLY Trigger z=25300, Blick -z", 21400, 1024, 0, 5, 25300, 25300, 250, 5);

    /* DER ENTSCHEIDENDE FALL: exakt der GEMESSENE Landepunkt der letzten Treppe
     * (21400, 25368), Band 0, Blick = die beim Treppenstart gesnappte Kardinale 3072.
     * Ein Druck auf SQUARE (z.B. um die Ausgangstuer zu oeffnen) darf hier im ORIGINAL
     * NICHTS ausloesen (Vorwaerts-Sonde bei z=25988 liegt ausserhalb des Rechtecks
     * z[20700..25000]; flags 0x31 -> Bit 0x40 CLEAR -> kein Zentrumstest @0x80042ea8).
     * Der Port loest ueber player_in_zone() aus: |25368-22850| = 2518 <= 2150+450. */
    sweep("LANDEPUNKT + SQUARE (Port-Reach 450)", 21400, 3072, 0, 5, 25368, 25368, 250, 5);

    /* Gegenprobe: dieselbe letzte Treppe mit den x-Raendern des Rechtecks. */
    sweep("Treppe 4 LETZTE, x=20300", 20300, 3072, 2, 4, 20800, 22800, 250, 5);
    sweep("Treppe 4 LETZTE, x=22500", 22500, 3072, 2, 4, 20800, 22800, 250, 5);
    return 0;
}
