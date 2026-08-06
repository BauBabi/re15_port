/* probe_camload_1030.c — LANE D MESSUNG (kein Fix):
 *   "Wie stellt der PORT den Kamera-Zustand beim Raum-(Re)Load her —
 *    und was macht ROOM1030s sub00-Cut_replace-Kette dabei?"
 *
 * ORIGINAL-Referenz (belegt, siehe Report):
 *   FUN_800396fc  @0x80039710-30  DAT_800aca3c &= 0xffff0000   (Bit 0x100 = Cut-Freeze AUS)
 *                 @0x8003993c-44  FUN_800142f4((s16)DAT_800b0fe4)  = TUER-Cut anwenden
 *                 @0x80039a00     FUN_8003ef6c  = SCD-Raum-Init (main00 + sub00 laufen HIER)
 *   FUN_800142f4  @0x800142f4     DAT_800afbb5 = cut ; DAT_800ac794 = FUN_80014324(cut)
 *   FUN_80014324  @0x80014324     erste RVD-Zeile mit +2 == cut  (Stride 0x14)
 *   FUN_80014230  @0x80014230     Auto-Scan: ab Zeile+1, ERSTE Treffer-Zeile gewinnt, RETURN
 *   Cut_replace   @0x80040414     Tausch +2/+3 ueber ALLE RVD-Zeilen, danach
 *                 @0x800404ac-cc  if (*(u8*)(DAT_800ac794+2) == a) FUN_800142f4(b)
 *   ROOM1030 sub00 @Datei 0x1ff0  Ck(4,15)==1 -> 4x Cut_replace (0,9)(3,10)(4,11)(6,12)
 *
 * Gemessen wird:
 *   (A) Zonen-Tabelle VOR / NACH scd_room_reenter mit Flag(4,15)=1  (wird getauscht?)
 *   (B) g_scd.cam_id / cam_change_pending / cut_auto_enabled danach
 *   (C) Einfluss des STALE g_re15_active_cut (Port liest den Licht-Index als
 *       "aktueller Cut"; im Original steht dort der frisch gesetzte Tuer-Cut)
 *   (D) welchen Cut der byte-true Auto-Scan aus einer Spieler-Position liefert
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
#include "re15_light.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t g_scd;
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

static void dump_zones(const re15_rdt_t *rdt, const char *tag)
{
    printf("[zones %s]", tag);
    for (int i = 0; i < rdt->zone_count; i++)
        printf(" %d:%u>%u", i, rdt->zones[i].cam_from, rdt->zones[i].cam_to);
    printf("\n");
}

static void dump_cam_aots(const char *tag)
{
    printf("[cam-aots %s]", tag);
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (a->active && a->type == RE15_AOT_TYPE_CAM_SWITCH)
            printf(" s%d:%u>%u", i, a->cam_from_filter, a->event_id);
    }
    printf("\n");
}

static void dump_cam_state(const char *tag)
{
    printf("[cam %-22s] cam_id=%u prev=%u pending=%u auto=%u  g_re15_active_cut=%d\n",
           tag, g_scd.cam_id, g_scd.cam_id_prev, g_scd.cam_change_pending,
           g_scd.cut_auto_enabled, g_re15_active_cut);
}

/* --- Byte-true Nachbau von FUN_80014230 direkt auf der RVD-Tabelle (Referenz-Orakel) ---
 * DAT_800ac794 = erste Zeile mit cam_from == cut (FUN_80014324 @0x80014324).
 * Scan ab Zeile+1, solange cam_from == cut; ERSTE Zeile, in deren Quad der Spieler
 * liegt, gewinnt -> return cam_to. -1 = kein Wechsel. */
static int64_t cross(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{ return (int64_t)ax * (int64_t)bz - (int64_t)bz * 0; }

static int point_in_quad_ref(int32_t px, int32_t pz, const int16_t *xs, const int16_t *zs)
{
    /* gleiche Vorzeichenkonvention wie re15_aot_point_in_quad (FUN_80014368) */
    return re15_aot_point_in_quad(px, pz, xs, zs);
}

static int autoscan_ref(const re15_rdt_t *rdt, int cut, int32_t px, int32_t pz, int band)
{
    int anchor = -1;
    for (int i = 0; i < rdt->zone_count; i++)
        if (rdt->zones[i].cam_from == cut) { anchor = i; break; }
    if (anchor < 0) return -2;                       /* Cut hat keine Zeile */
    for (int i = anchor + 1; i < rdt->zone_count; i++) {
        if (rdt->zones[i].cam_from != cut) break;    /* Gruppe zu Ende */
        const re15_rdt_zone_t *z = &rdt->zones[i];
        if (!(z->floor == 0xFF || (int)z->floor == band)) continue;
        if (point_in_quad_ref(px, pz, z->xs, z->zs)) return z->cam_to;
    }
    return -1;
}

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

    printf("ROOM1030: cuts=%d zones=%d\n", rdt.cut_count, rdt.zone_count);
    dump_zones(&rdt, "frisch geparst");

    /* ---------- A: ERST-BESUCH (Flag(4,15)=0) ---------- */
    printf("\n##### A: ERST-BESUCH — Flag(4,15)=0 #####\n");
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -25000; pl->y = 0; pl->z = -20000; pl->rot_y = 2048;
    g_re15_active_cut = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    dump_cam_state("A nach reenter");
    dump_zones(&rdt, "A nach reenter");
    dump_cam_aots("A");

    /* ---------- B: RE-ENTRY nach gesehener Cutscene (Flag(4,15)=1) ---------- */
    /* Der Port re-parst beim echten Raumwechsel neu (room_pc.c:68) — hier explizit
     * nachstellen, damit die Zonen wie im Original frisch von Platte kommen. */
    for (int stale = 0; stale <= 12; stale++) {
        static re15_rdt_t r2;
        if (re15_rdt_parse(dat, sz, &r2) != 0) { fprintf(stderr, "FAIL: reparse\n"); return 1; }
        re15_actor_init();
        scd_vm_init();
        g_current_room_id = 0x1030;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = -25000; pl->y = 0; pl->z = -20000; pl->rot_y = 2048;
        re15_game_flag_set(3, 116, 1);
        re15_game_flag_set(4, 15, 1);          /* Cutscene bereits gesehen */
        g_re15_active_cut = stale;             /* = Cut des VORRAUMS (Port liest den!) */
        scd_room_reenter(&r2, pl->x, pl->z, 0);
        printf("\n-- B stale g_re15_active_cut=%2d --\n", stale);
        dump_cam_state("B nach reenter");
        if (stale == 0) { dump_zones(&r2, "B nach reenter"); dump_cam_aots("B"); }
    }

    /* ---------- C: Auto-Scan-Orakel auf der GETAUSCHTEN Tabelle ---------- */
    printf("\n##### C: Auto-Scan (byte-true FUN_80014230) auf der getauschten Tabelle #####\n");
    {
        static re15_rdt_t r3;
        re15_rdt_parse(dat, sz, &r3);
        re15_actor_init(); scd_vm_init();
        g_current_room_id = 0x1030;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = -25000; pl->y = 0; pl->z = -20000;
        re15_game_flag_set(3, 116, 1);
        re15_game_flag_set(4, 15, 1);
        g_re15_active_cut = 99;                /* neutral: Tail feuert nicht */
        scd_room_reenter(&r3, pl->x, pl->z, 0);
        dump_zones(&r3, "C getauscht");
        /* Fuer jeden moeglichen TUER-Eintritts-Cut: hat er nach dem Tausch ueberhaupt
         * noch Schaltzonen? (Gruppe mit >1 Zeile) */
        for (int cut = 0; cut < r3.cut_count; cut++) {
            int anchor = -1, n = 0;
            for (int i = 0; i < r3.zone_count; i++)
                if (r3.zones[i].cam_from == cut) { if (anchor < 0) anchor = i; n++; }
            printf("  cut %2d: anchor=%2d zeilen=%d -> schaltzonen=%d%s\n",
                   cut, anchor, n, n > 0 ? n - 1 : 0,
                   (n <= 1) ? "   <== SACKGASSE (Auto-Scan tot)" : "");
        }
        /* Konkreter Punkt: Spieler in Zone[20] (from=5 -> to=4/11) */
        printf("  autoscan(cut=5, p=(-15000,-12000)) = %d  (0=Cut0 .. -1=kein Wechsel)\n",
               autoscan_ref(&r3, 5, -15000, -12000, 0));
    }

    /* ---------- D: welchen EINTRITTS-CUT geben die Tueren nach ROOM1030 vor? ----------
     * Original: FUN_8001d600 @0x8001d930-48 liest Tuer-Record +0x0A -> DAT_800afbb5/
     * DAT_800b0fe4; der Raumlader wendet ihn @0x80039944 an. Port: op_door_aot_set
     * pc[24] -> door_params[].target_cut. */
    printf("\n##### D: Tueren mit Ziel ROOM1030 (dest_stage=0 dest_room=3) #####\n");
    for (unsigned rid = 0x1000; rid <= 0x1270; rid += 0x10) {
        char p2[600]; size_t s2 = 0;
        snprintf(p2, sizeof p2, "%s/STAGE1/ROOM%04X.RDT", base, rid);
        uint8_t *d2 = read_file(p2, &s2);
        if (!d2) continue;
        static re15_rdt_t r4;
        if (re15_rdt_parse(d2, s2, &r4) != 0) { free(d2); continue; }
        re15_actor_init(); scd_vm_init();
        g_current_room_id = rid;
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p->active = 1; p->type = 0; p->hp = 100;
        scd_room_reenter(&r4, 0, 0, 0);
        for (int i = 0; i < RE15_AOT_MAX; i++) {
            if (!g_aot.slots[i].active || g_aot.slots[i].type != RE15_AOT_TYPE_DOOR) continue;
            if (g_aot.door_params[i].dest_stage != 0 || g_aot.door_params[i].dest_room != 3) continue;
            /* Welcher Cut deckt die Tuer im QUELL-Raum ab? = die Anker-Zone (cam_from-Gruppen-
             * Erste), in deren Quad die Tuer-Rect-Mitte liegt. Das ist der Wert, den
             * g_re15_active_cut beim Verlassen des Quellraums traegt. */
            int door_cut = -1;
            for (int zi = 0; zi < r4.zone_count; zi++) {
                const re15_rdt_zone_t *z = &r4.zones[zi];
                if (zi > 0 && r4.zones[zi - 1].cam_from == z->cam_from) continue;  /* nur Anker */
                if (re15_aot_point_in_quad(g_aot.slots[i].x, g_aot.slots[i].z, z->xs, z->zs)) {
                    door_cut = z->cam_from; break;
                }
            }
            printf("  ROOM%04X slot%-2d -> ROOM1030  entry_cut=%u  tuer@(%ld,%ld) liegt in Cut %d  spawn=(%ld,%ld,%ld)\n",
                   rid, i, g_aot.door_params[i].target_cut,
                   (long)g_aot.slots[i].x, (long)g_aot.slots[i].z, door_cut,
                   (long)g_aot.door_params[i].spawn_x,
                   (long)g_aot.door_params[i].spawn_y,
                   (long)g_aot.door_params[i].spawn_z);
        }
        free(d2);
    }

    free(dat);
    (void)cross;
    return 0;
}
