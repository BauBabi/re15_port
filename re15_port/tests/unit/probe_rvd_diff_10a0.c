/* probe_rvd_diff_10a0.c — MESSUNG (kein Fix): Nutzer-Report 2026-08-06
 *   "Im Treppenhaus schwenkt nach dem Runterlaufen der letzten Treppe die
 *    Kamera teilweise nicht richtig um."
 *
 * DIFFERENTIAL: fuer JEDEN Cut und ein dichtes XZ-Gitter ueber den Raum wird
 * verglichen, welchen Ziel-Cut
 *   (A) das ORIGINAL waehlt — FUN_80014230 @0x80014230, direkt aus den
 *       RDT-RVD-Bytes nachgebildet:
 *         anchor = erster Eintrag mit cam_from == cur      (FUN_80014324 @0x80014324)
 *         @0x80014254  lbu v1,22(anchor)  ; = (anchor+20)->cam_from
 *         @0x8001425c  bne v1,cur -> RAUS (Gruppe hat KEINE Transition)
 *         Schleife ab anchor+20:
 *           @0x8001426c  v1 = e[1] (floor);  ==0xFF -> Quad-Test
 *           @0x80014280-88 sonst muss v1 == player+0x82 (Band) sein
 *           @0x80014298  FUN_80014368(player, e)  ; Punkt-in-Quad e+4..e+19
 *           @0x800142a8  a0 = e[3] (cam_to) -> FUN_800142f4  ; ERSTER Treffer gewinnt
 *           @0x800142bc-d0  e += 20; weiter solange e[2] == cur
 *   (B) der PORT waehlt — re15_aot_scan (aot_common.c), also die als CAM_SWITCH
 *       installierten AOT-Slots inkl. der "letzter Treffer gewinnt"-Regel.
 *
 * Beide Seiten benutzen DENSELBEN Quad-Test (re15_aot_point_in_quad), es wird
 * also ausschliesslich die AUSWAHL-/Installations-Logik geprueft, nicht die
 * Geometrie.
 *
 * ROOM10A0-Daten (Datei-Offsets): RVD @0x1a0, 28 Eintraege a 20 B, 10 Cuts.
 * Treppen (main00 @0xdb0): A slots3/4 band8<->6, B slots5/6 band6<->4,
 * C slots7/8 band4<->3, D slots9/10 band3<->1 (die LETZTE Treppe abwaerts).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* --- (A) ORIGINAL FUN_80014230, direkt auf den RVD-Bytes ---------------- */
static const uint8_t *s_rvd = NULL;
static int            s_rvd_n = 0;

static int orig_pick(int cur, int32_t px, int32_t pz, int player_band)
{
    int anchor = -1;
    for (int i = 0; i < s_rvd_n; i++)                       /* FUN_80014324 */
        if (s_rvd[i * 20 + 2] == (uint8_t)cur) { anchor = i; break; }
    if (anchor < 0) return -1;
    if (anchor + 1 >= s_rvd_n) return -1;
    if (s_rvd[(anchor + 1) * 20 + 2] != (uint8_t)cur) return -1;   /* @0x8001425c */
    for (int i = anchor + 1; i < s_rvd_n; i++) {
        const uint8_t *e = s_rvd + i * 20;
        if (e[2] != (uint8_t)cur) break;                     /* @0x800142d0 */
        uint8_t fl = e[1];
        if (fl != 0xFF && player_band >= 0 && fl != (uint8_t)player_band) continue;
        int16_t xs[4], zs[4];
        for (int k = 0; k < 4; k++) {
            xs[k] = (int16_t)(e[4 + k * 4] | (e[5 + k * 4] << 8));
            zs[k] = (int16_t)(e[6 + k * 4] | (e[7 + k * 4] << 8));
        }
        if (re15_aot_point_in_quad(px, pz, xs, zs)) return e[3];   /* ERSTER Treffer */
    }
    return -1;
}

/* --- (B) PORT: re15_aot_scan, Ergebnis aus g_scd ------------------------ */
static int port_pick(int cur, int32_t px, int32_t pz)
{
    g_scd.cam_id             = (uint8_t)cur;
    g_scd.cam_change_pending = 0;
    g_scd.cut_auto_enabled   = 1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = px; pl->z = pz;
    re15_aot_scan(px, pz, (uint8_t)cur);
    if (!g_scd.cam_change_pending) return -1;
    return (int)g_scd.cam_id;
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    unsigned room = (argc > 1) ? (unsigned)strtoul(argv[1], NULL, 16) : 0x10A0;
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT", base, (room >> 12) & 0xF, room);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    uint32_t rvd_off = (uint32_t)(data[0x28] | (data[0x29] << 8) |
                                  (data[0x2a] << 16) | (data[0x2b] << 24));
    s_rvd = data + rvd_off;
    s_rvd_n = 0;
    while (rvd_off + (uint32_t)(s_rvd_n + 1) * 20 <= size &&
           s_rvd[s_rvd_n * 20 + 2] != 0xFF) s_rvd_n++;
    printf("ROOM%04X: cuts=%d RVD@0x%x eintraege=%d (rdt->zone_count=%d)\n",
           room, rdt.cut_count, rvd_off, s_rvd_n, rdt.zone_count);

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    scd_room_reenter(&rdt, 0, 0, 0);

    int n_cs = 0;
    printf("== als CAM_SWITCH installierte Slots ==\n  ");
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (a->active && a->type == RE15_AOT_TYPE_CAM_SWITCH) {
            printf("s%d:%u>%u ", i, a->cam_from_filter, a->event_id);
            n_cs++;
        }
    }
    printf("\n  installiert=%d  (RVD-Transitionen im Original = Eintraege - Gruppen)\n", n_cs);

    /* Gruppen/Transitionen im Original zaehlen */
    int groups = 0, trans = 0;
    for (int i = 0; i < s_rvd_n; i++) {
        if (i == 0 || s_rvd[(i - 1) * 20 + 2] != s_rvd[i * 20 + 2]) groups++;
        else trans++;
    }
    printf("  ORIGINAL: gruppen=%d transitionen=%d\n", groups, trans);

    /* Bounding box aus allen Zonen */
    int32_t minx = 1 << 30, maxx = -(1 << 30), minz = 1 << 30, maxz = -(1 << 30);
    for (int i = 0; i < s_rvd_n; i++) {
        const uint8_t *e = s_rvd + i * 20;
        for (int k = 0; k < 4; k++) {
            int16_t x = (int16_t)(e[4 + k * 4] | (e[5 + k * 4] << 8));
            int16_t z = (int16_t)(e[6 + k * 4] | (e[7 + k * 4] << 8));
            if (x < minx) minx = x; if (x > maxx) maxx = x;
            if (z < minz) minz = z; if (z > maxz) maxz = z;
        }
    }
    printf("  Gitter x[%ld..%ld] z[%ld..%ld] step=50\n\n",
           (long)minx, (long)maxx, (long)minz, (long)maxz);

    re15_collision_set_band(-1);   /* Band-Gate neutral (alle Zonen floor=0xFF) */

    long diffs = 0, tested = 0, hits = 0;
    for (int cur = 0; cur < rdt.cut_count; cur++) {
        long dcut = 0;
        int32_t fx = 0, fz = 0; int fo = -2, fp = -2;
        for (int32_t z = minz; z <= maxz; z += 50) {
            for (int32_t x = minx; x <= maxx; x += 50) {
                int o = orig_pick(cur, x, z, -1);
                int p = port_pick(cur, x, z);
                tested++;
                if (o >= 0) hits++;
                if (o != p) {
                    if (dcut == 0) { fx = x; fz = z; fo = o; fp = p; }
                    dcut++; diffs++;
                }
            }
        }
        printf("  cur_cut=%-2d  Abweichungen=%ld%s\n", cur, dcut,
               dcut ? "" : "  ok");
        if (dcut)
            printf("      erste: pos=(%ld,%ld) original=%d port=%d\n",
                   (long)fx, (long)fz, fo, fp);
    }
    printf("\nSUMME: getestet=%ld  original-treffer=%ld  ABWEICHUNGEN=%ld\n",
           tested, hits, diffs);

    free(data);
    return diffs ? 2 : 0;
}
