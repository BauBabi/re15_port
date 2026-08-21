/* test_rvd_scan_oracle.c — REGRESSIONSWACHE: der RVD-Kamera-Zonen-Scan des Ports MUSS
 * ueber JEDEN Raum, JEDEN Cut, JEDE Etage und JEDEN Gitterpunkt dasselbe liefern wie das
 * Original FUN_80014230.
 *
 * WARUM ALS ORAKEL UND NICHT ALS EINZELFALL: der Nutzer-Befund "die Kamera wechselt
 * TEILWEISE nicht" ist positionsabhaengig. Ein Einzelfall-Test kann so etwas nicht
 * ausschliessen; ein flaechendeckender Vergleich gegen die nachgebaute Original-Funktion
 * kann es. (Vorlage: analysis/stair_camera_1060.md — dort war "teilweise" ein 253 Einheiten
 * breites Fenster.)
 *
 * DAS ORAKEL IST BYTE-EXAKT NACHGEBAUT (selbst disassembliert aus ghidra1_V2.txt):
 *
 *  FUN_80014230 @0x80014230  (Haupt-Loop-Aufruf @0x8001ccec, a0 = 0; Gate davor
 *                             @0x8001cce0 `andi DAT_800aca3c,0x100` -> gesetzt = Scan AUS)
 *    @0x80014230 lui a0,0x800b / @0x80014234 `lw a0,-0x386c(a0)=>DAT_800ac794`  Gruppenzeiger
 *    @0x8001423c `lbu v0,-0x44b(v0)=>DAT_800afbb5`        ANGEFORDERTER Cut
 *    @0x80014254 `lbu v1,0x16(a0)` / @0x8001425c `bne v1,v0,LAB_800142d8`  -> RAUS
 *    @0x80014260 `addiu s1,a0,0x14`   erster GETESTETER Record (der Gruppen-ANKER wird
 *                                     nie getestet)
 *    @0x80014268 `addiu s0,a0,0x16`
 *    @0x8001426c `lbu v1,-0x1(s0)`    Floor-Byte rec+0x01
 *    @0x80014274 `beq v1,s2(=0xff),LAB_80014290`
 *    @0x80014280 `lbu v0,-0x352a(v0)=>DAT_800acad6` (Spieler-Etage)
 *    @0x80014288 `bne v1,v0,LAB_800142bc`
 *    @0x80014294 `addiu a0,a0,-0x3578` = &DAT_800aca88 (Spielerposition)
 *    @0x80014298 `jal FUN_80014368`   Punkt-in-Quad
 *    @0x800142a0 `beq v0,zero,LAB_800142bc`
 *    @0x800142a8 `lbu a0,0x1(s0)`     = rec+0x03 = Ziel-Cut
 *    @0x800142ac `jal FUN_800142f4` + @0x800142b4 `j LAB_800142d8`   ERSTER TREFFER GEWINNT
 *    @0x800142bc `addiu s0,s0,0x14` / @0x800142d4 `addiu s1,s1,0x14`
 *    @0x800142c0 `lbu v1,0x0(s0)` / @0x800142c8 `lbu v0,DAT_800afbb5` /
 *    @0x800142d0 `beq v1,v0,LAB_8001426c`   -> nur der zusammenhaengende Lauf gleicher cam_from
 *
 *  FUN_80014324 @0x80014324  (Gruppenzeiger = ERSTER Record mit rec+0x02 == Cut)
 *    @0x80014330 `lw v1,0x28(v0)` = RDT-Sektion 0x28 (der RVD-Block)
 *    @0x80014338 `lbu v0,0x2(v1)` / @0x80014340 `beq v0,a0,LAB_80014360`
 *    @0x8001434c-58 Schleife `lbu v0,0x2(v1)` / `bne` / `addiu v1,v1,0x14`
 *    @0x8001435c `addiu v1,v1,-0x14` / @0x80014360 `jr ra` (v0 = v1)
 *
 *  FUN_80014368 @0x80014368  (Punkt-in-Quad, Ecken rec+0x04..0x13 als s16-Paare)
 *    @0x800143a4 `mult` -> `if (a5*a4 <= a2*a1)`   a5=x1-x0 a4=pz-z0 a2=z1-z0 a1=px-x0
 *    @0x800143b4 `if (a6*a4 <  a7*a1) return 0`    a6=x3-x0 a7=z3-z0
 *    dann mit b8=x2-x0 b3=z2-z0:
 *      `(a2-b3)*(a1-b8) <= (a5-b8)*(a4-b3)` UND `(a6-b8)*(a4-b3) <= (a7-b3)*(a1-b8)` -> 1
 *
 * Record = 0x14 Byte: +0x00 ? +0x01 floor +0x02 cam_from +0x03 cam_to +0x04..0x13 4 Ecken.
 * Terminator = 4x 0xFF (rdt_common.c parse_zones).
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

extern scd_vm_t g_scd;

static uint8_t *g_raw   = NULL;
static size_t   g_rawsz = 0;
static uint32_t g_rvd   = 0;

static int g_fail = 0;

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

static int16_t rs16(uint32_t off) { return (int16_t)(uint16_t)(g_raw[off] | (g_raw[off+1] << 8)); }

/* FUN_80014368 @0x80014368 */
static int orig_point_in_quad(int32_t px, int32_t pz, uint32_t rec)
{
    int32_t x0 = rs16(rec + 4),  z0 = rs16(rec + 6);
    int32_t a4 = pz - z0;
    int32_t a5 = rs16(rec + 8)    - x0;
    int32_t a1 = px - x0;
    int32_t a2 = rs16(rec + 10)   - z0;
    int32_t a6 = rs16(rec + 0x10) - x0;
    int32_t a7 = rs16(rec + 0x12) - z0;
    if (a5 * a4 <= a2 * a1) {
        if (a6 * a4 < a7 * a1) return 0;
        int32_t b3 = rs16(rec + 0x0e) - z0;
        int32_t b8 = rs16(rec + 0x0c) - x0;
        if (((a2 - b3) * (a1 - b8) <= (a5 - b8) * (a4 - b3)) &&
            ((a6 - b8) * (a4 - b3) <= (a7 - b3) * (a1 - b8)))
            return 1;
    }
    return 0;
}

/* FUN_80014324 @0x80014324 */
static long orig_group_ptr(int cut, int nrec)
{
    for (int i = 0; i < nrec; i++) {
        uint32_t rec = g_rvd + (uint32_t)i * 0x14u;
        if (g_raw[rec + 2] == (uint8_t)cut) return (long)rec;
    }
    return -1;
}

/* FUN_80014230 @0x80014230 — Rueckgabe = neuer Cut, oder -1 (kein Wechsel). */
static int orig_rvd_scan(int cut, int32_t px, int32_t pz, int floor, int nrec)
{
    long grp = orig_group_ptr(cut, nrec);
    if (grp < 0) return -1;
    uint32_t s1 = (uint32_t)grp + 0x14;
    uint32_t s0 = (uint32_t)grp + 0x16;
    if (s0 >= g_rawsz || g_raw[s0] != (uint8_t)cut) return -1;   /* @0x80014254-5c */
    for (;;) {
        if (s1 + 0x14 > g_rawsz) return -1;
        uint8_t fl = g_raw[s0 - 1];                              /* @0x8001426c */
        if (fl == 0xFF || (int)fl == floor) {                    /* @0x80014274 / @0x80014288 */
            if (orig_point_in_quad(px, pz, s1))                  /* @0x80014298 */
                return g_raw[s0 + 1];                            /* @0x800142a8 */
        }
        s0 += 0x14; s1 += 0x14;                                  /* @0x800142bc / @0x800142d4 */
        if (s0 >= g_rawsz || g_raw[s0] != (uint8_t)cut) return -1;/* @0x800142c0-d0 */
    }
}

/* Ein Raum: Port-Scan gegen das Orakel, Gitter ueber die Ausdehnung aller RVD-Quads. */
static void check_room(const char *path, const char *name)
{
    size_t sz = 0;
    g_raw = read_file(path, &sz);
    if (!g_raw) return;
    g_rawsz = sz;
    re15_rdt_t rdt;
    if (re15_rdt_parse(g_raw, sz, &rdt) < 0) { free(g_raw); g_raw = NULL; return; }
    g_rvd = (uint32_t)(g_raw[0x28] | (g_raw[0x29]<<8) | (g_raw[0x2A]<<16) | (g_raw[0x2B]<<24));
    if (g_rvd == 0 || g_rvd + 20 > sz) { free(g_raw); g_raw = NULL; return; }

    int nrec = 0;
    for (uint32_t o = g_rvd; o + 20 <= sz; o += 20) {
        if (g_raw[o] == 0xFF && g_raw[o+1] == 0xFF && g_raw[o+2] == 0xFF && g_raw[o+3] == 0xFF) break;
        nrec++;
    }
    if (nrec == 0) { free(g_raw); g_raw = NULL; return; }

    /* Gitter = Bounding-Box aller Quads, 150 Einheiten Raster (feiner als jede
     * ausgelieferte Zonentiefe/2 — die duennsten Baender sind ~900 tief). */
    int32_t minx = 32767, maxx = -32768, minz = 32767, maxz = -32768;
    int max_cut = 0, max_floor = 0;
    for (int i = 0; i < nrec; i++) {
        uint32_t r = g_rvd + (uint32_t)i * 0x14u;
        for (int k = 0; k < 4; k++) {
            int32_t x = rs16(r + 4 + k*4), z = rs16(r + 6 + k*4);
            if (x < minx) minx = x; if (x > maxx) maxx = x;
            if (z < minz) minz = z; if (z > maxz) maxz = z;
        }
        if (g_raw[r+2] > max_cut)   max_cut   = g_raw[r+2];
        if (g_raw[r+1] != 0xFF && g_raw[r+1] > max_floor) max_floor = g_raw[r+1];
    }

    re15_actor_init(); scd_vm_init(); re15_aot_init();
    re15_collision_reset_band();
    g_current_room_id = 0x1000; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    /* Nur die Zonen installieren (kein SCD-Lauf) — verglichen wird der KAMERA-Scan;
     * andere AOT-Klassen wuerden feuern und den Scan vorzeitig beenden. */
    re15_rdt_apply_zones_as_aots(&rdt, 0);

    long tested = 0, diff = 0;
    int shown = 0;
    for (int floor = 0; floor <= max_floor; floor++) {
        re15_collision_set_band(floor);
        for (int cut = 0; cut <= max_cut; cut++) {
            for (int32_t x = minx - 150; x <= maxx + 150; x += 150) {
                for (int32_t z = minz - 150; z <= maxz + 150; z += 150) {
                    int want = orig_rvd_scan(cut, x, z, floor, nrec);
                    if (want == cut) want = -1;
                    g_scd.cam_id             = (uint8_t)cut;
                    g_scd.cam_change_pending = 0;
                    g_scd.cut_auto_enabled   = 1;
                    pl->x = x; pl->z = z;
                    re15_aot_scan(x, z, (uint8_t)cut);
                    int got = ((int)g_scd.cam_id == cut) ? -1 : (int)g_scd.cam_id;
                    tested++;
                    if (want != got) {
                        diff++;
                        if (shown < 8) {
                            shown++;
                            printf("FAIL: %s floor=%d cut=%2d (%6ld,%6ld): "
                                   "FUN_80014230 -> %-3d, Port -> %d\n",
                                   name, floor, cut, (long)x, (long)z, want, got);
                        }
                    }
                }
            }
        }
    }
    if (diff) { g_fail = 1; printf("FAIL: %s — %ld/%ld Gitterpunkte weichen ab\n", name, diff, tested); }
    else      printf("  ok %-9s %2d Records, %ld Punkte\n", name, nrec, tested);
    free(g_raw); g_raw = NULL;
}

int main(void)
{
    const char *base = RE15_ASSET_PSX_DIR;
    /* Die STAGE1-Raeume der Kampagne + der Generator-Raum des Nutzer-Befunds. */
    static const char *rooms[] = {
        "ROOM1030","ROOM1040","ROOM1060","ROOM1070","ROOM1090","ROOM10A0","ROOM10C0",
        "ROOM10D0","ROOM10F0","ROOM1120","ROOM1130","ROOM1140","ROOM1150","ROOM1170",
        "ROOM1190","ROOM11B0","ROOM11C0","ROOM11E0","ROOM11F0","ROOM1230","ROOM1240",
    };
    int n = (int)(sizeof rooms / sizeof rooms[0]);
    int found = 0;
    for (int i = 0; i < n; i++) {
        char p[600];
        snprintf(p, sizeof p, "%s/STAGE1/%s.RDT", base, rooms[i]);
        FILE *f = fopen(p, "rb");
        if (!f) continue;
        fclose(f);
        found++;
        check_room(p, rooms[i]);
    }
    if (!found) { printf("SKIP: keine STAGE1-RDTs unter %s\n", base); return 77; }
    printf(g_fail ? "RVD-ORAKEL: ABWEICHUNG\n" : "RVD-ORAKEL: OK\n");
    return g_fail;
}
