/* probe_laneE_1030_visible.c — LANE E MESSUNG (kein Fix).
 *
 * Nutzer-Report ROOM1030: "Im ORIGINAL sind nur SECHS Zombies sichtbar, bei uns Dutzende."
 *
 * Diese Sonde misst die SICHTBARKEITS-Seite:
 *   1) die 20 Sce_em_set-Records roh aus ROOM1030.RDT (Datei-Offset 0x1de6, Stride 20)
 *   2) die RVD-Zonen-Tabelle des Raums (cam_from/cam_to + 4-Ecken-Quad)
 *   3) pro CUT: das Region-Quad, das das ORIGINAL fuer den Zeichen-Cull benutzt
 *      (FUN_80014324 = ERSTER RVD-Record mit rec[+2]==cut; DAT_800ac790)
 *      -> und fuer jede der 20 Spawn-Positionen: drin/draussen
 *      -> BEIDE Tests: byte-true FUN_80014368 UND der Port-Test re15_aot_point_in_quad
 *   4) LIVE: scd_room_reenter(ROOM1030) -> wieviele Aktoren spawnt der PORT wirklich?
 *
 * ORIGINAL-BELEGE (im Chat/Bericht zitiert):
 *   FUN_8001e8c8 (RE_15_Quellcode_V2/FUN_8001e8c8.c)  — der Entity-Zeichen-Einstieg:
 *       iVar2 = FUN_80014368(DAT_800ac784 + 0x34, DAT_800ac790);
 *       if (iVar2 == 0)  -> Teile via FUN_8001ef54  (KEIN Mesh-Draw)
 *       else             -> Teile via FUN_8001e9ec  (Mesh-Draw)
 *   Aufrufer-Schleife @0x8001d0e8-0x8001d168 (ghidra1_V2.txt:94569-94586):
 *       s0 = DAT_800aca4e (Spawn-Zaehler); ent = DAT_800acc2c; Stride 0x1f4;
 *       `andi v0,v0,0x1` @0x8001d0fc  -> nur ent+0x00 bit0 wird gezeichnet
 *   FUN_80014324 (Region-Lookup) — erster RVD-Record mit +2 == cut.
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
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t g_scd;

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

/* ---- byte-true Nachbau FUN_80014368 (RE_15_Quellcode_V2/FUN_80014368.c) ----
 * param_1 = &pos (int x, int y, int z)  -> nutzt [0]=x und [2]=z
 * param_2 = RVD-Record; Ecken als s16 bei +4/+6, +8/+10, +0xc/+0xe, +0x10/+0x12 */
static int fun_80014368(int32_t px, int32_t pz, const int16_t xs[4], const int16_t zs[4])
{
    int iVar8 = xs[0];
    int iVar3 = zs[0];
    int iVar4 = (int)pz - iVar3;
    int iVar5 = (int)xs[1] - iVar8;
    int iVar1 = (int)px - iVar8;
    int iVar2 = (int)zs[1] - iVar3;
    int iVar6 = (int)xs[3] - iVar8;
    int iVar7 = (int)zs[3] - iVar3;
    if (iVar5 * iVar4 <= iVar2 * iVar1) {
        if (iVar6 * iVar4 < iVar7 * iVar1) return 0;
        iVar3 = (int)zs[2] - iVar3;
        iVar8 = (int)xs[2] - iVar8;
        if (((iVar2 - iVar3) * (iVar1 - iVar8) <= (iVar5 - iVar8) * (iVar4 - iVar3)) &&
            ((iVar6 - iVar8) * (iVar4 - iVar3) <= (iVar7 - iVar3) * (iVar1 - iVar8)))
            return 1;
    }
    return 0;
}

typedef struct { int slot, type, beh, band, p5, p6, kill; int x, y, z, anim, rot, init; } emrec_t;

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    size_t sz = 0;

    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    printf("ROOM1030: cuts=%d zones=%d sub_scd=%d  RE15_ACTOR_MAX=%d\n\n",
           rdt.cut_count, rdt.zone_count, rdt.sub_scd_count, RE15_ACTOR_MAX);

    /* ---------- 1) Sce_em_set-Records roh ---------- */
    emrec_t rec[24]; int nrec = 0;
    const size_t EM_OFF = 0x1de6;
    for (int i = 0; i < 22; i++) {
        const uint8_t *p = dat + EM_OFF + (size_t)i * 20;
        if (p[0] != 0x44) break;
        emrec_t *r = &rec[nrec++];
        r->slot = p[1]; r->type = p[2]; r->beh = p[3]; r->band = p[4];
        r->p5 = p[5]; r->p6 = p[6]; r->kill = p[7];
        r->x    = (int16_t)(p[8]  | (p[9]  << 8));
        r->y    = (int16_t)(p[10] | (p[11] << 8));
        r->z    = (int16_t)(p[12] | (p[13] << 8));
        r->anim = (int16_t)(p[14] | (p[15] << 8));
        r->rot  = (int16_t)(p[16] | (p[17] << 8));
        r->init = (int16_t)(p[18] | (p[19] << 8));
    }
    printf("== 1) Sce_em_set-Records (Datei 0x%04zx, Stride 20) : %d Stueck ==\n", EM_OFF, nrec);
    for (int i = 0; i < nrec; i++) {
        emrec_t *r = &rec[i];
        printf("  #%2d slot=%2d type=0x%02X beh=0x%02X band=%d p5=%d p6=%d kill=%2d "
               "pos=(%6d,%6d,%6d) anim=%d rot=%5d init=%d  floorY(-1800*band)=%d\n",
               i, r->slot, r->type, r->beh, r->band, r->p5, r->p6, r->kill,
               r->x, r->y, r->z, r->anim, r->rot, r->init, -1800 * r->band);
    }

    /* ---------- 2) RVD-Zonen ---------- */
    printf("\n== 2) RVD-Zonen (Port-Parse; Original: Stride 0x14, +2=from, +3=to) ==\n");
    for (int i = 0; i < rdt.zone_count; i++) {
        const re15_rdt_zone_t *z = &rdt.zones[i];
        printf("  z%2d from=%3u to=%3u floor=0x%02X quad=(%d,%d)(%d,%d)(%d,%d)(%d,%d)"
               "  aabb x[%ld..%ld] z[%ld..%ld]\n",
               i, z->cam_from, z->cam_to, z->floor,
               z->xs[0], z->zs[0], z->xs[1], z->zs[1],
               z->xs[2], z->zs[2], z->xs[3], z->zs[3],
               (long)(z->cx - z->half_w), (long)(z->cx + z->half_w),
               (long)(z->cz - z->half_h), (long)(z->cz + z->half_h));
    }

    /* ---------- 3) Pro Cut: wer waere sichtbar? ---------- */
    printf("\n== 3) Zeichen-Cull pro CUT (Original FUN_8001e8c8 -> FUN_80014368(ent+0x34, DAT_800ac790)) ==\n");
    printf("   ORIG = byte-true FUN_80014368, PORT = re15_aot_point_in_quad\n");
    for (int c = 0; c < rdt.cut_count; c++) {
        int16_t qx[4], qz[4];
        int has = re15_rdt_get_region_quad(&rdt, c, qx, qz);
        if (!has) {
            printf("  cut %2d: KEIN RVD-Record -> Port cam_has_region=0 => ES WIRD NICHTS GECULLT "
                   "(alle %d Zombies gezeichnet!)   [Original: FUN_80014324 wuerde hier ins Leere laufen]\n",
                   c, nrec);
            continue;
        }
        int no = 0, np = 0;
        char so[64] = {0}, sp[64] = {0};
        for (int i = 0; i < nrec; i++) {
            int o = fun_80014368(rec[i].x, rec[i].z, qx, qz);
            int p = re15_aot_point_in_quad(rec[i].x, rec[i].z, qx, qz);
            if (o) { no++; if (strlen(so) < 55) snprintf(so + strlen(so), 8, "%d ", i); }
            if (p) { np++; if (strlen(sp) < 55) snprintf(sp + strlen(sp), 8, "%d ", i); }
        }
        printf("  cut %2d: quad=(%d,%d)(%d,%d)(%d,%d)(%d,%d)\n", c,
               qx[0], qz[0], qx[1], qz[1], qx[2], qz[2], qx[3], qz[3]);
        printf("          ORIG sichtbar=%2d  [%s]\n", no, so);
        printf("          PORT sichtbar=%2d  [%s]%s\n", np, sp,
               (np == no) ? "" : "   <<< TEST-DIVERGENZ ORIG vs PORT");
    }

    /* ---------- 4) LIVE: was spawnt der Port wirklich? ---------- */
    printf("\n== 4) LIVE Port: scd_room_reenter(ROOM1030) ==\n");
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -25000; pl->y = 0; pl->z = -20000; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    for (int t = 0; t < 8; t++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }

    int nact = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active) continue;
        nact++;
        printf("  actor%2d type=0x%02X grid(+0x9)=0x%02X st=%u sub=%u pos=(%6ld,%6ld,%6ld) "
               "rot=%5d floor=%u hp=%d emflag=%u\n",
               s, a->type, a->grid_id, a->state, a->sub_state_1,
               (long)a->x, (long)a->y, (long)a->z, (int)a->rot_y, a->floor, a->hp, a->em_flag_id);
    }
    printf("  -> AKTIVE GEGNER IM PORT: %d   (RDT-Records: %d)  VERLOREN: %d "
           "(RE15_ACTOR_MAX=%d, slot->actor = slot+1)\n",
           nact, nrec, nrec - nact, RE15_ACTOR_MAX);

    /* ---------- 5) LIVE-Sichtbarkeit mit dem tatsaechlichen Startcut ---------- */
    printf("\n== 5) LIVE: aktueller cam_id=%u -> Port-Zeichen-Cull auf den LIVE-Aktoren ==\n",
           g_scd.cam_id);
    {
        int16_t qx[4], qz[4];
        int has = re15_rdt_get_region_quad(&rdt, g_scd.cam_id, qx, qz);
        int drawn = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *a = &g_actors[s];
            if (!a->active) continue;
            int vis = !has || re15_aot_point_in_quad(a->x, a->z, qx, qz);
            if (vis) drawn++;
        }
        printf("  has_region=%d -> gezeichnete Gegner = %d\n", has, drawn);
    }

    /* ---------- 6) LIVE-AI: wandern die Gegner IN das Cut-0-Quad hinein? ----------
     * DAS ist die eigentliche Lane-E-Frage: der Region-Cull ist byte-true, aber wenn
     * der Port die 15 Zombies laufen laesst, sammeln sie sich vor der Kamera. */
    {
        /* Enemy-Bank fuer Typ 0x16 laden — ohne sie tickt die AI ins Leere
         * (LEHRE aus der letzten Runde: Aufbau muss den LIVE-Zustand reproduzieren). */
        char emsp[600]; size_t ems_size = 0;
        snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
        uint8_t *ems = read_file(emsp, &ems_size);
        static uint8_t scratch[0x80000];
        int bank_ok = 0;
        if (ems) {
            int idx = re15_ems_index_for_type(0x16);
            size_t off = 0, len = 0;
            if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 &&
                len <= sizeof scratch) {
                re15_enemy_bank_t *eb = re15_enemy_alloc(0x16);
                if (eb) {
                    memcpy(scratch, ems + off, len);
                    re15_tim_t tim = (re15_tim_t){0};
                    if (re15_emd_parse_container(scratch, len, &eb->md1, &eb->skel,
                                                 &eb->anim, &tim) == 0) {
                        eb->ok = 1; eb->buf = NULL; bank_ok = 1;
                    }
                }
            }
            free(ems);
        }
        printf("\n== 6) LIVE-AI (Bank EM16 geladen=%d): wandern sie ins Bild? ==\n", bank_ok);
        /* Spieler dort, wo ihn die Cutscene-Kette erwartet: AOT3-Rect (Flag 5,32). */
        pl->x = -19900; pl->z = -9200;
        int16_t q0x[4], q0z[4], q6x[4], q6z[4];
        int h0 = re15_rdt_get_region_quad(&rdt, 0, q0x, q0z);
        int h6 = re15_rdt_get_region_quad(&rdt, 6, q6x, q6z);
        printf("  t   aktiv  in-cut0  in-cut6   (drawn = im aktiven Quad)\n");
        for (int t = 0; t <= 600; t++) {
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
            if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
            re15_actor_step_all_walkers();
            re15_actors_anim_advance();
            re15_enemy_ai_run_all(0);
            if (t % 60 == 0) {
                int na = 0, c0 = 0, c6 = 0;
                for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                    const re15_actor_t *a = &g_actors[s];
                    if (!a->active) continue;
                    na++;
                    if (h0 && re15_aot_point_in_quad(a->x, a->z, q0x, q0z)) c0++;
                    if (h6 && re15_aot_point_in_quad(a->x, a->z, q6x, q6z)) c6++;
                }
                printf("  %3d   %3d     %3d      %3d\n", t, na, c0, c6);
            }
        }
        printf("  Endpositionen:\n");
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *a = &g_actors[s];
            if (!a->active) continue;
            printf("    actor%2d st=%u s1=%u grid=0x%02X mo=%3d pos=(%6ld,%6ld) "
                   "in0=%d in6=%d\n",
                   s, a->state, a->sub_state_1, a->grid_id, (int)a->motion,
                   (long)a->x, (long)a->z,
                   h0 ? re15_aot_point_in_quad(a->x, a->z, q0x, q0z) : -1,
                   h6 ? re15_aot_point_in_quad(a->x, a->z, q6x, q6z) : -1);
        }
    }
    return 0;
}
