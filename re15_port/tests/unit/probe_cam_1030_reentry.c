/* probe_cam_1030_reentry.c — MESSUNG (kein Fix): Nutzer-Report 2026-08-06
 *   "Wenn ich ROOM1030 erneut betrete, wird die Kamera wieder nicht auf den
 *    SPIELER gelegt, sondern auf die ZOMBIES."
 *
 * Faehrt den ECHTEN Tuer-Pfad (re15_room_apply_pending, room_common.c) mit
 * Datei-Ladern als ARCH-Callbacks:
 *      ROOM1030 (Cutscene gelaufen -> flag(4,15)=1)
 *   -> Tuer slot2 -> ROOM1040 (Eintritts-Cut 0)
 *   -> Tuer slot1 -> ROOM1030 (Eintritts-Cut 8)
 * und protokolliert nach JEDEM Wechsel:
 *   g_scd.cam_id / cam_change_pending / cut_auto_enabled / active_cut_idx /
 *   g_re15_active_cut, die RVD-Zonentabelle (cam_from/cam_to) und die
 *   CAM_SWITCH-AOT-Slots.
 *
 * ORIGINAL-Referenz (ROOM1030.RDT, Datei-Offsets; sub_scd@0x1fd0):
 *   sub00 +0x0018 f0x1FE8  Set(3,116,1)
 *         f0x1FEC  Ifel_ck  / f0x1FF0 Ck(4,15)==1
 *         f0x1FF4  Cut_replace 0 <-> 9
 *         f0x1FF7  Cut_replace 3 <-> 10
 *         f0x1FFA  Cut_replace 4 <-> 11
 *         f0x1FFD  Cut_replace 6 <-> 12
 *         f0x2000  Sca_id_set(2,6,0xf7) / f0x2004 Sca_id_set(3,6,0xf7)
 *         f0x2008  Set(5,20,1)
 *   d.h. der Kamera-Remap wird bei JEDEM Betreten neu angewandt, sobald die
 *   Cutscene einmal lief — das ist byte-true und KEIN Fehler.
 * Tuer-Eintritts-Cuts nach ROOM1030 (Door_aot_set pc[24]):
 *   aus 1040 = cut 8, aus 1050 = cut 2, aus 1070 = cut 7, aus 1031 = cut 6.
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
#include "re15_camera.h"
#include "re15_light.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t          g_scd;
extern re15_aot_state_t  g_aot;
extern int               g_re15_active_cut;

/* --- Port-seitige Sicht auf den Raum (das, was platform/pc/main.c haelt) --- */
static re15_rdt_t                s_rdt;
static int                       s_rdt_ok = 0;
static const re15_camera_cut_t  *s_active_cuts = NULL;
static int                       s_active_cut_count = 0;
static int                       s_active_cut_idx = 0;
static re15_camera_view_t        s_cam_view;

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

/* ARCH-Callback (Gegenstueck zu room_pc.c re15_room_load) */
static uint8_t *s_room_buf = NULL;
static int probe_load_rdt(unsigned room_id)
{
    char path[600];
    size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT",
             RE15_XSTR(RE15_ASSETS_PATH), (room_id >> 12) & 0xF, room_id);
    uint8_t *buf = read_file(path, &sz);
    if (!buf) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return -1; }
    if (re15_rdt_parse(buf, sz, &g_room_rdt) != 0) { free(buf); return -1; }
    if (s_room_buf) free(s_room_buf);
    s_room_buf        = buf;
    g_current_room_id = room_id;
    g_room_rdt_ok     = 1;
    return 0;
}
static void probe_reset_render(void) { }
static int  probe_load_bg(int cut) { (void)cut; return 0; }

static void door_to(unsigned room_id, int32_t x, int32_t y, int32_t z,
                    int16_t yaw, int cut)
{
    re15_room_apply_ctx_t rc;
    memset(&rc, 0, sizeof rc);
    rc.rdt              = &s_rdt;
    rc.rdt_ok           = &s_rdt_ok;
    rc.active_cuts      = &s_active_cuts;
    rc.active_cut_count = &s_active_cut_count;
    rc.cam_active_cut   = &s_active_cut_idx;
    rc.cam_view         = &s_cam_view;
    rc.load_rdt         = probe_load_rdt;
    rc.reset_render     = probe_reset_render;
    rc.load_bg_cut      = probe_load_bg;
    re15_room_request_change(room_id, x, y, z, yaw, cut);
    if (!re15_room_apply_pending(&rc))
        fprintf(stderr, "FAIL: Raumwechsel nach %04X nicht angewandt\n", room_id);
}

static void dump_cam(const char *tag)
{
    printf("[cam %-28s] room=%04X active_cut_idx=%d g_re15_active_cut=%d "
           "| g_scd.cam_id=%u prev=%u pending=%d cut_auto=%d\n",
           tag, g_current_room_id, s_active_cut_idx, g_re15_active_cut,
           g_scd.cam_id, g_scd.cam_id_prev, g_scd.cam_change_pending,
           g_scd.cut_auto_enabled);
}

static void dump_zones(const char *tag)
{
    printf("== RVD-Zonen %s (rdt->zones, %d) ==\n  ", tag, s_rdt.zone_count);
    for (int i = 0; i < s_rdt.zone_count; i++) {
        printf("%d:%u>%u ", i, s_rdt.zones[i].cam_from, s_rdt.zones[i].cam_to);
        if ((i % 12) == 11) printf("\n  ");
    }
    printf("\n");
}

static void dump_camswitch_aots(const char *tag)
{
    printf("== CAM_SWITCH-AOTs %s ==\n  ", tag);
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;
        printf("s%d:%u>%u ", i, a->cam_from_filter, a->event_id);
    }
    printf("\n");
}

/* Fresh-Parse der RDT-Zonen (Referenz: was der Raum-Load LIEFERN muesste) */
static void dump_fresh_zones(const char *path)
{
    size_t sz = 0;
    uint8_t *b = read_file(path, &sz);
    if (!b) return;
    static re15_rdt_t fresh;
    if (re15_rdt_parse(b, sz, &fresh) == 0) {
        printf("== RVD-Zonen FRISCH aus Datei (%d) ==\n  ", fresh.zone_count);
        for (int i = 0; i < fresh.zone_count; i++) {
            printf("%d:%u>%u ", i, fresh.zones[i].cam_from, fresh.zones[i].cam_to);
            if ((i % 12) == 11) printf("\n  ");
        }
        printf("\n");
    }
    free(b);
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char p1030[600];
    snprintf(p1030, sizeof p1030, "%s/STAGE1/ROOM1030.RDT", base);

    re15_actor_init();
    scd_vm_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;

    /* ============ PHASE 1: ERST-Betretung ROOM1030 (flag(4,15)=0) ============ */
    printf("##### PHASE 1: ROOM1030 ERSTMALS betreten (Cutscene noch nicht gelaufen) #####\n");
    door_to(0x1030, -13000, 0, -23000, 2048, 8);   /* Eintritts-Cut wie aus ROOM1040 */
    dump_cam("1030 erste Betretung");
    printf("  flag(4,15)=%d flag(5,20)=%d\n",
           re15_game_flag_get(4, 15), re15_game_flag_get(5, 20));
    dump_zones("1030 #1");
    dump_camswitch_aots("1030 #1");
    dump_fresh_zones(p1030);

    /* ============ PHASE 2: Cutscene als GELAUFEN markieren ==================
     * Genau die zwei Flags, die sub01 f0x2198 (Set(4,15)=1) und sub08 f0x27d8
     * (Set(5,20)=1) hinterlassen; (3,116) setzt sub00 f0x1FE8 ohnehin. */
    printf("\n##### PHASE 2: Cutscene gelaufen -> flag(4,15)=1 #####\n");
    re15_game_flag_set(4, 15, 1);

    /* ============ PHASE 3: Raum verlassen -> ROOM1040 (Eintritts-Cut 0) ===== */
    printf("\n##### PHASE 3: Tuer ROOM1030 slot2 -> ROOM1040 (Door_aot_set pc[24]=cut 0) #####\n");
    door_to(0x1040, 5000, 0, -12000, 2048, 0);
    dump_cam("1040 betreten");

    /* ============ PHASE 4: zurueck nach ROOM1030 (Eintritts-Cut 8) ========== */
    printf("\n##### PHASE 4: Tuer ROOM1040 slot1 -> ROOM1030 (Door_aot_set pc[24]=cut 8) #####\n");
    door_to(0x1030, -13000, 0, -23000, 2048, 8);
    dump_cam("1030 WIEDERbetreten");
    printf("  flag(4,15)=%d flag(5,20)=%d\n",
           re15_game_flag_get(4, 15), re15_game_flag_get(5, 20));
    dump_zones("1030 #2 (nach Cut_replace)");
    dump_camswitch_aots("1030 #2");
    dump_fresh_zones(p1030);

    printf("\nBEFUND PHASE 4: active_cut_idx=%d (Tuer wollte 8) cut_auto=%d "
           "cam_change_pending=%d\n",
           s_active_cut_idx, g_scd.cut_auto_enabled, g_scd.cam_change_pending);

    /* ============ PHASE 5: Sweep — welcher VORHERIGE Cut loest es aus? ======
     * g_re15_active_cut wird von room_common.c erst in Schritt (10) gesetzt
     * (re15_light_apply_cut), also NACH scd_room_reenter in Schritt (7). Beim
     * Cut_replace in sub00 steht dort noch der Cut des VORHERIGEN Raums. */
    printf("\n##### PHASE 5: Sweep ueber den Rest-Cut des Vorraums #####\n");
    for (int prev = 0; prev < 13; prev++) {
        re15_actor_init();
        scd_vm_init();
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100;
        re15_game_flag_set(4, 15, 1);
        g_re15_active_cut = prev;                /* Rest des Vorraums */
        door_to(0x1030, -13000, 0, -23000, 2048, 8);
        printf("  vorheriger_cut=%2d -> active_cut_idx=%2d cam_id=%2u pending=%d cut_auto=%d %s\n",
               prev, s_active_cut_idx, g_scd.cam_id, g_scd.cam_change_pending,
               g_scd.cut_auto_enabled,
               (s_active_cut_idx != 8 || !g_scd.cut_auto_enabled) ? "  <== DIVERGENZ" : "");
    }

    /* REGRESSIONS-GATE (Nutzer-Report 2026-08-06: "beim Wiederbetreten liegt die Kamera auf den
     * Zombies statt auf dem Spieler"). Der Tuer-Eintritts-Cut (8) MUSS sich gegen JEDEN Rest-Cut des
     * Vorraums durchsetzen. Vorher kaperten die Cuts 0/3/4/6 die Ansicht nach 9/10/11/12 — Cut 9 ist
     * byte-gleich zu Cut 0 (die Halle mit den 20 Zombies). Ursache war der Schwanz von op_cut_replace,
     * der aus dem Zeiger-Fixup LAB_80040414 @0x800404ac-cc einen echten Kamerawechsel machte. */
    {
        int bad = 0;
        for (int prev = 0; prev < 13; prev++) {
            re15_actor_init();
            scd_vm_init();
            pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->active = 1; pl->type = 0; pl->hp = 100;
            re15_game_flag_set(4, 15, 1);
            g_re15_active_cut = prev;
            door_to(0x1030, -13000, 0, -23000, 2048, 8);
            if (s_active_cut_idx != 8 || !g_scd.cut_auto_enabled) {
                printf("FAIL: Vorraum-Cut %d kapert den Tuer-Cut: active=%d cut_auto=%d\n",
                       prev, s_active_cut_idx, g_scd.cut_auto_enabled);
                bad = 1;
            }
        }
        if (bad) { printf("\nERGEBNIS: ROOM1030-Wiederbetreten DEFEKT\n"); return 1; }
        printf("\nERGEBNIS: Tuer-Cut 8 setzt sich gegen alle 13 Vorraum-Cuts durch\n");
    }

    if (argc > 1) (void)argv;
    return 0;
}
