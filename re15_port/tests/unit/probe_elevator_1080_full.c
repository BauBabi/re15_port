/* probe_elevator_1080_full.c — FAHRSTUHL ROOM1080, GANZER WEG bis in den geladenen Raum.
 *
 * WARUM ES DIESE ZWEITE SONDE GIBT
 * --------------------------------
 * `probe_elevator_1080.c` misst nur die halbe Kette: sie bricht bei Zeile 174 ab, sobald
 * `g_room_change.pending` steht, und ruft `re15_room_apply_pending` NIE. Genau zwischen
 * diesen beiden Punkten sitzt der Defekt — im ECHTEN Spiel gemessen (Lauf 2026-09-26,
 * analysis/befunde_2026-09-26/messung-fahrstuhl.md):
 *
 *   AOT_ON slot=1 typ=1                              <- Knopf 2F, richtig
 *   FIRE_DOOR slot=1 dest=st0 rm0x0C cut=0
 *   REQ  room=0x10C0 ...                             <- richtiger Zielraum in der Anforderung
 *   SCAN-FIRE-DOOR slot=0 action=0 inside=0          <- SELBES BILD: der AOT-Scan feuert Slot 0
 *   FIRE_DOOR slot=0 dest=st0 rm0x04 cut=5
 *   REQ  room=0x1040 ...                             <- ueberschreibt die Anforderung (1F)
 *   APPLY-ENDE  raum=0x1040                          <- geladen wird 1F
 *
 * Der zweite Schuss kommt aus dem Port-eigenen Sonderzweig `is_auto_door`
 * (engine/src/aot_common.c:943-956 -> Feuer-Bedingung :1268): eine Tuer mit
 * Null-Rechteck feuert dort OHNE Action-Druck und OHNE Rechteck-Treffer, sobald
 *   in_cinematic (aot_common.c:882 = player_mode==2 || letterbox_countdown!=0)
 *   && scd_idle (:897)  && scd_ran (:900, tick_count>90).
 * Alle drei Fahrstuhl-Tueren von ROOM1080 sind Null-Rect (ROOM1080.RDT @Datei 0x0482/
 * 0x04A2/0x04C2: `3b 0N 02 31 00 00 00 00 00 00 00 00 00 00 ...`), die Schleife laeuft ab
 * i=0 (aot_common.c:913), also gewinnt IMMER Slot 0 = ROOM1040 = 1F.
 *
 * Die alte Sonde kann das prinzipiell nicht sehen, weil sie `g_scd.player_mode` nie auf 2
 * setzt (das leitet im Spiel platform/pc/main.c:4566-4578 jedes Bild aus
 * flag(1,27)||flag(2,7) ab) — `in_cinematic` bleibt dort false.
 *
 * DIESE SONDE
 * -----------
 *  - faehrt denselben echten UI-Pfad (Panel-Action, D-Pad-Cursor, SQUARE-Confirm),
 *  - zieht den player_mode-Proxy des Spiels nach (main.c:4566-4578),
 *  - ruft `re15_room_apply_pending` mit einem Datei-RDT-Lader,
 *  - und pinnt den RAUM, IN DEM DER SPIELER WIRKLICH LANDET — nicht die Warteschlange.
 *
 * Soll (ROOM1080.RDT main00-Payload, Warp FUN_8001d600 @0x8001d94c lbu 9(a0) = Raum):
 *   Knopf 1F (Zelle 4) -> sub07 -> Aot_on 0 -> ROOM1040
 *   Knopf 2F (Zelle 5) -> sub08 -> Aot_on 1 -> ROOM10C0
 *   Knopf 3F (Zelle 6) -> sub09 -> Aot_on 2 -> ROOM1120
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
extern uint16_t re15_pad_virtual_word(uint16_t phys);
extern int      re15_cine_active(void);

static int g_fail = 0;

/* --- Port-seitige Sicht auf den Raum (wie platform/pc/main.c) --- */
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

/* Abdeckungszaehler — eine Sonde, die nichts misst, luegt. */
static int s_ticks = 0, s_applies = 0, s_cine_frames = 0;

/* Ein Bild. Reihenfolge wie re15_game_step + die Hauptschleife:
 *   player_mode-Proxy (main.c:4566-4578) -> SCD -> AOT-Scan -> Aktoren
 *   -> Stempel -> re15_room_apply_pending (main.c:6840). */
static void tick(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* DER FEHLENDE ZUSTAND DER ALTEN SONDE: das Spiel leitet player_mode jeden Frame
     * NEU aus dem Cinematic-Latch ab (platform/pc/main.c:4566-4578,
     * `cine_active = flag(1,27) || flag(2,7)`). Ohne das ist `in_cinematic`
     * (aot_common.c:882) immer false und der Defekt unerreichbar. */
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

static int find_cursor_prop(void)
{
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_id == 0) return i;
    return -1;
}

static void hold(uint16_t phys_mask, int ticks)
{
    for (int t = 0; t < ticks; t++) { g_scd_pad_held = re15_pad_virtual_word(phys_mask); tick(); }
    g_scd_pad_held = 0;
}

/* Raum 1080 frisch betreten (ueber den ECHTEN Weg: Anforderung + apply_pending) und
 * das Panel per Action-Druck oeffnen. */
static int enter_1080_and_open_panel(void)
{
    re15_actor_init();
    scd_vm_init();
    g_scd.player_mode = 0;
    g_scd.letterbox_countdown = 0;
    /* Eintritts-Spawn der 2F-Tuer aus ROOM10C0 main00 @Datei 0x0E82: (-13650,0,-900). */
    re15_room_request_change(0x1080, -13650, 0, -900, 0, 0);
    { re15_room_apply_ctx_t rc; make_ctx_room(&rc);
      if (!re15_room_apply_pending(&rc)) { printf("   FAIL: ROOM1080 nicht geladen\n"); return 0; }
      s_applies++; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    for (int t = 0; t < 8; t++) tick();
    /* Vor das Panel (Rect-Zentrum (-12050,-50), FORWARD-620 bei Yaw 3072). */
    pl->x = -12050; pl->z = -670; pl->rot_y = 3072;
    g_aot_action_pressed = 1;
    tick(); tick(); tick();
    int mode_bits = 0;
    for (int b = 0; b <= 8; b++) mode_bits += re15_game_flag_get(5, (uint8_t)b);
    if (mode_bits != 9) { printf("   FAIL: Panel-Modus nicht aktiv (%d/9)\n", mode_bits); return 0; }
    return 1;
}

/* Eine Etagenfahrt bis in den GELADENEN Raum. */
static unsigned ride(const char *name, uint16_t dpad, int dticks,
                     int expect_notch, unsigned expect_room)
{
    printf("== %s ==\n", name);
    if (!enter_1080_and_open_panel()) { g_fail = 1; return 0; }
    int cp = find_cursor_prop();
    if (cp < 0) { printf("   FAIL: kein Cursor-Prop\n"); g_fail = 1; return 0; }
    if (dticks) hold(dpad, dticks);
    tick();
    cp = find_cursor_prop();
    unsigned notch = cp >= 0 ? g_scd.props[cp].member_0b : 0u;
    printf("   Cursor=(%ld,%ld) notch=%u (Soll %d)\n",
           cp >= 0 ? (long)g_scd.props[cp].x : 0, cp >= 0 ? (long)g_scd.props[cp].z : 0,
           notch, expect_notch);
    if ((int)notch != expect_notch) { printf("   FAIL: Cursor-Stempel falsch\n"); g_fail = 1; }
    hold(RE15_PAD_BIT_SQUARE, 1);
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
    printf("=== Fahrstuhl ROOM1080 — GANZER WEG bis in den geladenen Raum ===\n");
    unsigned r1 = ride("Knopf 1F (Zelle 4, 4x DOWN)",  RE15_PAD_BIT_DOWN, 4,  4, 0x1040);
    unsigned r2 = ride("Knopf 2F (Zelle 5, 4x UP)",    RE15_PAD_BIT_UP,   4,  5, 0x10C0);
    unsigned r3 = ride("Knopf 3F (Zelle 6, 17x UP)",   RE15_PAD_BIT_UP,   17, 6, 0x1120);

    printf("\n===== ETAGEN-TABELLE (geladener Raum) =====\n");
    printf("   1F: Soll 0x1040  Ist 0x%04X  %s\n", r1, r1 == 0x1040 ? "OK" : "FALSCH");
    printf("   2F: Soll 0x10C0  Ist 0x%04X  %s\n", r2, r2 == 0x10C0 ? "OK" : "FALSCH");
    printf("   3F: Soll 0x1120  Ist 0x%04X  %s\n", r3, r3 == 0x1120 ? "OK" : "FALSCH");
    printf("ABDECKUNG: %d Bilder getickt, %d Raumwechsel angewandt, %d Bilder im Cinematic.\n",
           s_ticks, s_applies, s_cine_frames);
    if (s_ticks == 0 || s_applies == 0) {
        printf("FEHLLAUF — die Sonde hat nichts gefahren, das Ergebnis sagt NICHTS.\n");
        return 1;
    }
    printf("\n===== BEFUND: %s =====\n", g_fail ? "FAIL" : "OK");
    return g_fail;
}
