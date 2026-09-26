/* probe_elevator_4020_full.c — DER ZWEITE FAHRSTUHL (ROOM4020), ganzer Weg bis in den
 * GELADENEN Raum. Spiegel von probe_elevator_1080_full.c.
 *
 * WARUM
 * -----
 * Der Zensus aus analysis/befunde_2026-09-26/messung-fahrstuhl.md (27 Null-Rect-
 * Cross-Room-Tueren in 19 Raeumen, vier Raeume mit mehr als einer) nennt ROOM4020/4021
 * als ZWEITEN Raum mit derselben Fehlerklasse. Hier wird sie gemessen statt behauptet.
 *
 * Selbst nachgelesen (ROOM4020.RDT, Datei-Byte-Offsets):
 *   Sub-Tabelle @0x069C (RDT-Header +0x44), 13 Subs.
 *   Tuer-Saetze (Door_aot_set, Opcode 0x3B, sce=2, flags=0x31), alle NULL-Rechteck
 *   (Bytes +4..+13 == 0):
 *     @0x063A slot0  +22=stage 0x03, +23=room 0x00, +24=cut 0x02  -> 0x4000
 *     @0x065A slot1  +22=stage 0x04, +23=room 0x00, +24=cut 0x05  -> 0x5000
 *     @0x067A slot2  +22=stage 0x04, +23=room 0x0C, +24=cut 0x00  -> 0x50C0
 *   Fahrt-Subs (jeweils mit Set(2,7,1) `22 02 07 01` + Set(1,27,1) `22 01 1b 01`,
 *   also Cinematic AN, und als LETZTE Opcodes `47 0N` Aot_on + `01 00` Evt_end):
 *     sub07 @0x08FE .. Aot_on 0 @0x0990   -> 1F  0x4000
 *     sub08 @0x0994 .. Aot_on 1 @0x0A36   -> 2F  0x5000
 *     sub09 @0x0A3A .. Aot_on 2 @0x0ACC   -> 3F  0x50C0
 *   Das ist Opcode fuer Opcode dieselbe Bauart wie ROOM1080 sub07/08/09.
 *
 * UNTERSCHIED ZUR 1080-SONDE (bewusst, und hier benannt): diese Sonde faehrt NICHT die
 * Panel-Oberflaeche (Cursor-Zellen, Member_cmp-Stempel) — die ist raumspezifisch und
 * NICHT der Gegenstand des Defekts. Sie startet die Fahrt-Subs direkt ueber
 * scd_event_fire(), also genau das, was der Panel-Evt_exec im Echtlauf tut
 * (Protokollzeile "EVT_EXEC sub=8 cond=0xff -> slot 10"). Gemessen wird der Weg DANACH:
 * Aot_on N -> Tuer-Feuer -> re15_room_request_change -> re15_room_apply_pending.
 *
 * DEFEKT, den sie faengt: der port-eigene Zweig `is_auto_door` (aot_common.c) liess jede
 * Null-Rect-Tuer OHNE Tastendruck feuern, sobald Cinematic lief und die SCD-Threads leer
 * waren; die Scan-Schleife ab i=0 waehlte dabei immer Slot 0. Das Original kann das nicht:
 * der druckfreie AUTO-Pass ruft den Scanner mit Pool=OBJEKT (@0x80043778 `ori a1,zero,0x4`)
 * und ACTION=0 (@0x80043790 `addu a2,zero,zero` im Delay-Slot von @0x8004378C `jal
 * 0x80042bac`), und der Scanner wirft den Satz an zwei Gates hinaus, BEVOR er den sce-Typ
 * ueberhaupt liest: @0x80042C8C `and v0,v0,a3` + @0x80042C90 `beq v0,zero` (0x31 & 0x04 = 0)
 * und @0x80042CA0 `andi v0,v0,0x10` + @0x80042CA4 `bne v0,s6` (0x10 != 0). Der sce-Typ wird
 * erst @0x80042CAC `lbu v0,2(s0)` gelesen.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_camera.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t g_scd;
extern re15_aot_state_t g_aot;
extern uint16_t g_scd_pad_held;
extern uint16_t g_scd_pad_edge;
extern uint8_t  g_aot_action_pressed;
extern int      re15_cine_active(void);

static int g_fail = 0;

static re15_rdt_t               s_rdt;
static int                      s_rdt_ok = 0;
static const re15_camera_cut_t *s_active_cuts = NULL;
static int                      s_active_cut_count = 0;
static int                      s_active_cut_idx = 0;
static re15_camera_view_t       s_cam_view;

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

static void make_ctx_room(re15_room_apply_ctx_t *rc)
{
    memset(rc, 0, sizeof *rc);
    rc->rdt              = &s_rdt;
    rc->rdt_ok           = &s_rdt_ok;
    rc->active_cuts      = &s_active_cuts;
    rc->active_cut_count = &s_active_cut_count;
    rc->cam_active_cut   = &s_active_cut_idx;
    rc->cam_view         = &s_cam_view;
    rc->load_rdt         = probe_load_rdt;
    rc->reset_render     = probe_reset_render;
    rc->load_bg_cut      = probe_load_bg;
}

static int s_ticks = 0, s_applies = 0, s_cine_frames = 0, s_rides = 0;

static void tick(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* player_mode-Proxy des Spiels (platform/pc/main.c, cine_active = flag(1,27)||flag(2,7)).
     * Ohne ihn ist in_cinematic in aot_common.c false und der Defekt unerreichbar. */
    if (re15_cine_active()) {
        g_scd.player_mode         = 2;
        g_scd.letterbox_countdown = -1;
        s_cine_frames++;
    }
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    re15_actor_step_all_walkers();
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
    re15_aot_stamp_entities();
    re15_object_notch_update();
    g_aot_action_pressed = 0;
    g_scd_pad_edge = 0;
    { re15_room_apply_ctx_t rc; make_ctx_room(&rc);
      if (re15_room_apply_pending(&rc)) s_applies++; }
    s_ticks++;
}

/* ROOM4020 frisch betreten — ueber den ECHTEN Weg (Anforderung + apply_pending). */
static int enter_4020(void)
{
    re15_actor_init();
    scd_vm_init();
    g_scd.player_mode = 0;
    g_scd.letterbox_countdown = 0;
    /* Kabinen-Mitte: Rechteck-freier Standpunkt, die Fahrt wird per Sub gestartet. */
    re15_room_request_change(0x4020, 0, 0, 0, 0, 0);
    { re15_room_apply_ctx_t rc; make_ctx_room(&rc);
      if (!re15_room_apply_pending(&rc)) { printf("   FAIL: ROOM4020 nicht geladen\n"); return 0; }
      s_applies++; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* > 90 Bilder: erst dann ist scd_ran (aot_common.c) wahr — der Zustand, in dem der
     * alte is_auto_door-Zweig feuerte. Weniger zu ticken wuerde den Defekt VERSTECKEN. */
    for (int t = 0; t < 120; t++) tick();
    return 1;
}

static unsigned ride(const char *name, uint8_t sub, unsigned expect_room)
{
    printf("== %s (sub%02u) ==\n", name, sub);
    if (!enter_4020()) { g_fail = 1; return 0; }
    if (!scd_event_fire(sub)) { printf("   FAIL: sub%02u nicht gestartet\n", sub); g_fail = 1; return 0; }
    s_rides++;
    unsigned start_room = g_current_room_id;
    int t = 0;
    for (; t < 2000; t++) { tick(); if (g_current_room_id != start_room) break; }
    printf("   GELADENER Raum nach %d Bildern: 0x%04X (Soll 0x%04X) %s\n",
           t, g_current_room_id, expect_room,
           g_current_room_id == expect_room ? "OK" : "FALSCH");
    if (g_current_room_id != expect_room) g_fail = 1;
    return g_current_room_id;
}

int main(void)
{
    printf("=== Fahrstuhl ROOM4020 (der ZWEITE) — bis in den geladenen Raum ===\n");
    unsigned r1 = ride("1F", 7, 0x4000);
    unsigned r2 = ride("2F", 8, 0x5000);
    unsigned r3 = ride("3F", 9, 0x50C0);

    printf("\n===== ETAGEN-TABELLE (geladener Raum) =====\n");
    printf("   1F: Soll 0x4000  Ist 0x%04X  %s\n", r1, r1 == 0x4000 ? "OK" : "FALSCH");
    printf("   2F: Soll 0x5000  Ist 0x%04X  %s\n", r2, r2 == 0x5000 ? "OK" : "FALSCH");
    printf("   3F: Soll 0x50C0  Ist 0x%04X  %s\n", r3, r3 == 0x50C0 ? "OK" : "FALSCH");
    printf("ABDECKUNG: %d Bilder getickt, %d Fahrten gestartet, %d Raumwechsel angewandt, "
           "%d Bilder im Cinematic.\n", s_ticks, s_rides, s_applies, s_cine_frames);
    if (s_ticks == 0 || s_rides != 3 || s_applies == 0) {
        printf("FEHLLAUF — die Sonde hat nicht alle drei Fahrten gefahren, das Ergebnis sagt NICHTS.\n");
        return 1;
    }
    printf("\n===== BEFUND: %s =====\n", g_fail ? "FAIL" : "OK");
    return g_fail;
}
