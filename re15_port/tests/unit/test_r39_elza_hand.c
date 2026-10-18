/* test_r39_elza_hand.c — Runde 39 Spur H "elza", Punkt 1: "Bei Elza, direkt in ROOM 1031 in der 1. Cutscene direkt am
 * Anfang hebt sie die Hand 2x ... Im Original hebt sie die Hand nur einmal und senkt sie dann wieder."
 *
 * MECHANIK (Dossier analysis/befunde_runde39/H_elza.md, Belege include/re15_elza_r39.h): der Port bildet den
 * cmd-0-Handler der Eintritts-Pose (W-Bank Clip 1 @0x80031c10-c24) und die Fall-0-Uebergabe der Idle-FSM
 * (@0x80032074-b4) als zweistufige Latch im Spieler-Tick nach (player_common.c s_entry_pose_step). Faellt der erste
 * Spieler-Tick nach dem Raumwechsel wegen RE15_PAUSE_PLAYER (Dispatcher-Gate @0x80031c54) erst in das Bild, in dem der SCD
 * schon Plc_motion(0,11) gesetzt hat (ROOM1031 sub13 @0x029A4; Handler @0x80041b90: +0x94 := 11 @0x80041ba8, +0x4 := 4
 * @0x80041bb0), ueberschrieb die Latch den Szenen-Clip (gemessen: F451 210, F452 200 statt Clip 11). Im Original laeuft
 * der Dispatcher FUN_80031c44 ab Kommandowort 4 die Tabelle @0x80073f90[4] = 0x80030660 — cmd 0 / Fall 0 nie mehr.
 *
 * RIEGEL: (a) Latch scharf, dann Plc_motion-Wirkung (+0x94 = 11, +0x4 = 4, +0x1c4 = 0x04) VOR dem ersten Spieler-Tick ->
 * zwei Spieler-Ticks lassen Motion 11 stehen (vorher: 210, dann 200). (b) Gegenprobe ohne Plc_motion (Kommandowort 1):
 * die Latch laeuft wie bisher: Tick 1 = 210 (Settle), Tick 2 = 200 (Idle-Uebergabe).
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_scd.h"
#include "re15_elza_r39.h"

static int g_fail = 0, g_checks = 0;
#define PRUEF(c, ...) do { g_checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

extern int re15_player_entry_pose_latch(void);   /* player_common.c:257 — Stand der Latch (0/1/2), kein Header */

enum { MOTION_SETTLE = 210, MOTION_IDLE = 200 };   /* W01 Clip 1 / Clip 3 Sentinels (anim_select_common.c Kopf,
                                                    * re15_player.h RE15_PLAYER_MOTION_IDLE = 200) */

static re15_camera_view_t s_view;

static void frisch(void)
{
    re15_actor_init();
    scd_vm_init();
    memset(&s_view, 0, sizeof s_view);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -13097; pl->y = -1800; pl->z = -21855; pl->rot_y = 1024;   /* ROOM1031 sub13 Member_set @0x02994-9C */
    pl->state = 1;                                                      /* Kommandowort 1 nach dem Raumwechsel (room_common.c) */
    g_scd.player_mode = 2;                                              /* Szene: Set(2,7,1)/Set(1,27,1) @0x02986/@0x0298A */
}

/* Wirkung des Plc_motion-Handlers @0x80041b90 auf den Spieler (scd_vm.c op_plc_motion): +0x94, +0x4 = 4, +0x6 = 0,
 * Flags, Crossfade-Saat; danach Plc_flg(0x04) @0x029A8 (OR auf +0x1c4). */
static void plc_motion_11(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_set_motion(pl, 11);
    pl->state = 4;                              /* @0x80041bb0 */
    pl->sub_state_2 = 0;                        /* @0x80041bb4 */
    pl->anim_frame = 0;
    pl->motion_init_delay = 1;
    pl->anim_frac = 7;
    pl->anim_flags = 0;
    pl->anim_flags |= 0x04;                     /* Plc_flg 0x04 @0x029A8 */
}

int main(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    PRUEF(re15_elza_r39_latch_hinfaellig(4) == 1 && re15_elza_r39_latch_hinfaellig(1) == 0 &&
          re15_elza_r39_latch_hinfaellig(0) == 0, "Kommandowort 4 (@0x80041bb0) = Latch hinfaellig; 0/1 nicht");

    /* (a) der gemessene Fall: Latch scharf (Raumwechsel), Spieler-Tick pausiert, SCD-Plc_motion kommt zuerst */
    frisch();
    re15_player_room_entry_pose();
    PRUEF(pl->motion == MOTION_SETTLE && re15_player_entry_pose_latch() == 1,
          "Raumwechsel: Eintritts-Pose %d (W-Bank Clip 1 @0x80031c10), Latch 1 scharf", (int)pl->motion);
    plc_motion_11();
    PRUEF(pl->motion == 11 && pl->state == 4, "Plc_motion(0,11): +0x94 = %d, Kommandowort %d", (int)pl->motion, (int)pl->state);
    re15_player_tick(&s_view, 0);
    const int m1 = (int)pl->motion, l1 = re15_player_entry_pose_latch();
    re15_player_tick(&s_view, 0);
    const int m2 = (int)pl->motion, l2 = re15_player_entry_pose_latch();
    PRUEF(m1 == 11 && m2 == 11 && l1 == 0 && l2 == 0,
          "zwei Spieler-Ticks nach dem Plc_motion: Motion %d / %d (soll 11 / 11, vorher 210 / 200), Latch %d / %d (soll 0)",
          m1, m2, l1, l2);
    PRUEF(pl->state == 4 && (pl->anim_flags & 0x04u), "Kommandowort bleibt 4, Schleifen-Flag 0x04 steht (Haltepose Clip 11)");

    /* (b) Gegenprobe: Szene (player_mode 2) OHNE Plc_motion -> die Latch laeuft byte-true weiter (cmd 0 -> Fall 0);
     * der pm-2-Zweig ersetzt die Pose nicht (holt nur 105/100 zurueck, player_common.c). */
    frisch();
    re15_player_room_entry_pose();
    re15_player_tick(&s_view, 0);
    const int g1 = (int)pl->motion;
    re15_player_tick(&s_view, 0);
    const int g2 = (int)pl->motion;
    PRUEF(g1 == MOTION_SETTLE && g2 == MOTION_IDLE && re15_player_entry_pose_latch() == 0,
          "Szene ohne Plc_motion: Tick 1 = %d (Settle, cmd 0), Tick 2 = %d (Idle-Uebergabe Fall 0 @0x80032088), Latch zu",
          g1, g2);
    /* (c) gewoehnliches Spiel (player_mode 0): der Pad-/Idle-Zweig ersetzt Settle schon im Latch-1-Bild durch den
     * Fall-0-Zustand (200); die Latch schliesst ohne zweiten Store (probe_r16_tuer1040, player_common.c Latch 2). */
    frisch();
    g_scd.player_mode = 0;
    re15_player_room_entry_pose();
    re15_player_tick(&s_view, 0);
    const int h1 = (int)pl->motion;
    re15_player_tick(&s_view, 0);
    const int h2 = (int)pl->motion;
    PRUEF(h1 == MOTION_IDLE && h2 == MOTION_IDLE && re15_player_entry_pose_latch() == 0 && pl->state != 4,
          "Spiel ohne Plc_motion: Tick 1 = %d, Tick 2 = %d (Idle-Zweig), Latch zu, Kommandowort %d", h1, h2, (int)pl->state);

    /* (d) PUNKT 1b — Sub-Init der Spieler-Walk-Subs (Cut_chg(6) @0x029CA + Plc_dest(0,9,0x20,..) @0x029CC nach dem
     * Senken): der Walker setzt am Init (walk_fsm 0) sofort Clip 5 der PLW-Bank (Sentinel 105), Bild 0, Blende 7
     * (TURN @0x800313a4-b8, WALK @0x80030b58-6c, RUN @0x80030d90-a4) — vorher lief Clip 0 vorwaerts weiter (slot
     * 10..17 = zweiter Hub). Hier: Spieler nach dem rueckwaerts gehaltenen Clip 0 (Plc_flg 0x80), dann Plc_dest-Wirkung
     * (op_plc_dest-Spielerpfad: state 4, walk_mode, +0x1c4 := 0, walk_active, walk_fsm 0) und EIN Walker-Tick. */
    static const int modi[3] = { 0x09, 0x04, 0x05 };
    for (int i = 0; i < 3; i++) {
        frisch();
        pl->state = 4; pl->sub_state_2 = 0;
        re15_actor_set_motion(pl, 0);
        pl->anim_flags = 0x80; pl->anim_frame = 29; pl->anim_frac = 0;   /* Clip 0 rueckwaerts, Ende gehalten (F520) */
        pl->walk_dest_x = (int16_t)(pl->x + 4000); pl->walk_dest_z = (int16_t)pl->z;   /* Ziel quer zur Blickrichtung */
        pl->walk_mode = (uint8_t)modi[i]; pl->walk_flag_bit = 0x20; pl->anim_flags = 0;   /* +0x1c4 := 0 @0x80041c4c */
        pl->walk_active = 1; pl->walk_fsm = 0;
        re15_actor_step_walk(pl);
        PRUEF(pl->motion == 105 && pl->anim_frame == 0 && pl->anim_frac == 7 && pl->walk_fsm >= 1 && pl->walk_active,
              "Plc_dest Modus 0x%02X, erster Walker-Tick: Motion %d (soll 105 = PLW Clip 5, +0x94 := 5), Bild %d (+0x95 := 0), "
              "Blende %d (+0x8f := 7), walk_fsm %d (Align) — kein Clip 0 vorwaerts mehr",
              modi[i], (int)pl->motion, (int)pl->anim_frame, (int)pl->anim_frac, (int)pl->walk_fsm);
    }
    /* Gegenprobe: ein Nicht-Spieler-Aktor bleibt unveraendert (NPC-Clips kommen aus der Tabelle @0x80076ca0, nicht
     * aus @0x80073e30) */
    {
        re15_actor_t *n = &g_actors[3];
        memset(n, 0, sizeof *n);
        n->active = 1; n->type = 0x47; n->motion = 2; n->walk_mode = 0x09; n->walk_active = 1; n->walk_fsm = 0;
        n->walk_dest_x = 4000; n->walk_dest_z = 0; n->rot_y = 2048;
        PRUEF(re15_elza_r39_plc_walk_init(n) == 0 && n->motion == 2, "Nicht-Spieler: Sub-Init greift nicht (Motion bleibt %d)", (int)n->motion);
    }

    printf("hand: %d Pruefungen, %d FAIL\n", g_checks, g_fail);
    return g_fail ? 1 : 0;
}
