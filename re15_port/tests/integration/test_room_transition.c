/**
 * @file test_room_transition.c
 * @brief Integration-Tests: Raumwechsel (gleiche Stage, Stage-Wechsel, Same-Room-Teleport)
 *
 * Testet die Zusammenarbeit von AOT-System, Tür-Trigger-Logik und Raum-Wechsel-
 * Anforderung (queued cross-room transition) als integriertes Subsystem.
 *
 * REAKTIVIERUNG / REWRITE (2026-06-28):
 *   Der ursprüngliche Test wurde gegen eine inzwischen ENTFERNTE API geschrieben
 *   (re15_door_trigger_check + re15_player_t + g_game.player/current_rdt/current_stage/
 *   current_room/current_cut + re15_aot_slot_t + re15_aot_set_slot/get_slot + g_io
 *   Mock-IO-Backend + audio_backend_t Mock-Vtable + Q12_FROM_INT/Q12_TO_INT +
 *   RE15_BTN_* + RE15_DOOR_*). Der Engine-Transplant hat dieses gesamte Subsystem
 *   durch ein anderes Modell ersetzt:
 *     - Der Spieler IST jetzt g_actors[RE15_ACTOR_SLOT_PLAYER] (Actor-Pool, RE2-rein;
 *       siehe engine/src/player_common.c:1-17), NICHT mehr ein re15_player_t.
 *     - Türen sind DOOR-AOTs (re15_aot_set_door, re15_aot.h:160), die der per-frame
 *       Scanner re15_aot_scan() auslöst — es gibt KEIN re15_door_trigger_check mehr.
 *     - Der Raumwechsel wird per re15_room_request_change() in g_room_change QUEUED
 *       (re15_room.h:39-52, engine/src/room_common.c:34) und vom Main-Loop später
 *       via re15_room_apply_pending() angewendet.
 *     - Die Aktionstaste wird dem Scanner über das öffentliche Flag g_aot_action_pressed
 *       signalisiert (re15_aot.h:150), nicht über eine re15_input_state_t.
 *
 *   Dieser Rewrite ERHÄLT die Test-INTENTION (Raumwechsel-Mechanik validieren) und
 *   nutzt ausschließlich die AKTUELLE öffentliche API. Die DOOR-Auslöselogik (Vorwärts-
 *   Reach + Band-Gate + Action-Press) ist byte-true in engine/src/aot_common.c:351-417
 *   dokumentiert; die Ziel-Raum-ID-Formel in aot_common.c:463-466.
 *
 * Requirements: 4.2, 4.3, 4.4, 4.7
 */

#include "re15_aot.h"
#include "re15_room.h"
#include "re15_room_list.h"   /* re15_room_ids[] / RE15_ROOM_COUNT (Ziel-Raum-Existenz) */
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_collision.h"
#include "re15_camera.h"      /* re15_sin_q12 / re15_cos_q12 via re15_skeleton (Reach-Mathe) */
#include "re15_skeleton.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * Test-Konstanten — die DOOR-Auslösegeometrie (byte-true aus aot_common.c)
 *
 * Der DOOR-Scanner (aot_common.c:362-368) feuert auf einen "Forward-Reach":
 * ein Punkt 563 Einheiten VOR dem Spieler (in Blickrichtung rot_y) muss
 * innerhalb von ±900 des Tür-ZENTRUMS liegen. Damit die Tür sicher feuert,
 * platzieren wir den Spieler so, dass sein Reach-Punkt exakt im Zentrum landet:
 * Spieler bei rot_y=0 blickt nach +X (Mesh-Konvention, Reach = (cos,-sin)),
 * also Reach-Punkt = (player_x + 563, player_z). Setzen wir das Tür-Zentrum
 * auf (player_x + 563, player_z), trifft der Reach exakt.
 * ========================================================================= */

#define DOOR_REACH_DIST  563   /* aot_common.c:366 — Forward-Reach-Distanz */

/* Hilfs-Setup: Engine-Zustand in einen definierten Ausgangszustand bringen.
 * Leert AOT-Slots, SCD-VM, Actor-Pool, Room-Change-Queue und die
 * Scanner-Gates (player_mode/letterbox/message). */
static void test_reset_engine(uint8_t player_band)
{
    re15_aot_init();           /* memset g_aot (alle Slots inaktiv) */
    scd_vm_init();             /* SCD-VM zurücksetzen (threads idle, gates 0) */

    memset(g_actors, 0, sizeof(re15_actor_t) * RE15_ACTOR_MAX);
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].type   = 0;   /* Spieler */

    /* Room-Change-Queue leeren (frühere Tests könnten sie gesetzt haben). */
    memset(&g_room_change, 0, sizeof(g_room_change));

    /* Scanner-Gates explizit auf "frei" (kein Cinematic, keine Message). */
    g_scd.player_mode            = 0;
    g_scd.letterbox_countdown    = 0;
    g_scd.message_query          = 0;
    g_scd.message_display_frames = 0;
    g_scd.cam_change_pending     = 0;
    g_scd.cam_id                 = 0xFF;
    /* tick_count > 90 ist NUR für die Auto-Advance-Tür relevant (is_auto_door);
     * normale Rechteck-Türen brauchen das nicht. Wir lassen es bei 0. */

    /* Aktueller Raum = STAGE1 ROOM1170 (Boot-Raum). Die Ziel-Raum-ID-Formel
     * (aot_common.c:463-466) zieht das Varianten-Nibble aus dieser ID. */
    g_current_room_id = RE15_BOOT_ROOM;   /* 0x1170 */

    /* Spieler-Floor-Band setzen (DOOR-Band-Gate, aot_common.c:369-381). Der
     * Door-Param-Band (von re15_aot_set_door NICHT gesetzt → 0) muss matchen,
     * sonst feuert die Tür nicht. Wir setzen Player- UND Door-Band auf 0. */
    re15_collision_set_band((int)player_band);

    g_aot_action_pressed = 0;
}

/**
 * Registriert eine DOOR-AOT im Slot, deren Forward-Reach-Zentrum am Spieler-
 * Reach-Punkt liegt. Der Spieler steht bei (px,pz) mit rot_y=0 (blickt +X).
 * Setzt zusätzlich den Door-Param-Band auf 0 (matcht das Player-Band 0).
 */
static void setup_door_aot(int slot,
                           int32_t px, int32_t pz,
                           uint8_t dest_stage, uint8_t dest_room,
                           int32_t spawn_x, int32_t spawn_y,
                           int32_t spawn_z, int16_t spawn_yaw,
                           uint8_t dest_cut)
{
    /* Tür-Zentrum = Reach-Punkt des nach +X blickenden Spielers. */
    int32_t door_cx = px + DOOR_REACH_DIST;
    int32_t door_cz = pz;

    /* half_w/half_h > 0 → echte Rechteck-Tür (KEINE Auto-Advance-Tür). */
    re15_aot_set_door(slot, door_cx, door_cz, 500, 500,
                      dest_cut, spawn_x, spawn_y, spawn_z, spawn_yaw);

    /* Door-Param-Felder, die re15_aot_set_door nicht abdeckt, direkt setzen:
     * Ziel-Stage/-Raum (Cross-Room-Auflösung) + Band-Gate-Match. */
    g_aot.door_params[slot].dest_stage = dest_stage;
    g_aot.door_params[slot].dest_room  = dest_room;
    g_aot.door_params[slot].band       = 0;   /* matcht Player-Band 0 */
}

/* Erwartete Ziel-Raum-ID, byte-true aus aot_common.c:464-466. */
static unsigned expected_dest_id(uint8_t dest_stage, uint8_t dest_room)
{
    return (((unsigned)dest_stage + 1u) << 12)
         | ((unsigned)dest_room << 4)
         | (g_current_room_id & 0x000Fu);
}

/* Spieler an (px,pz), Blickrichtung +X (rot_y=0). */
static void place_player(int32_t px, int32_t pz)
{
    g_actors[RE15_ACTOR_SLOT_PLAYER].x     = px;
    g_actors[RE15_ACTOR_SLOT_PLAYER].y     = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z     = pz;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 0;   /* blickt +X (Reach = (cos,-sin)) */
}

/* =========================================================================
 * Test 1: Gleiche-Stage Raumwechsel
 *
 * Szenario: Spieler steht vor Tür-Zone, Aktionstaste gedrückt.
 * Tür führt zu anderem Raum in gleicher Stage.
 * Erwartung:
 *   - re15_room_request_change wird ausgelöst → g_room_change.pending = 1
 *   - g_room_change.room_id = Ziel-Raum-ID (gleiche Stage, anderer Raum)
 *   - Spawn-Position/Rotation/Cut korrekt in der Queue
 *   - KEIN Same-Room-Teleport (Actor wird NICHT direkt versetzt)
 *
 * Validates: Requirements 4.2, 4.3
 * ========================================================================= */

static int test_same_stage_room_change(void)
{
    printf("  test_same_stage_room_change... ");

    test_reset_engine(0);

    /* Aktueller Raum STAGE1/ROOM1170 (0x1170). Ziel: gleiche Stage (dest_stage=0,
     * 0-basiert = STAGE1), anderer Raum (dest_room=0x14). */
    uint8_t dest_stage = 0;     /* STAGE1 (0-basiert, aot_common.c:451-455) */
    uint8_t dest_room  = 0x14;
    unsigned want_id   = expected_dest_id(dest_stage, dest_room);

    place_player(0, 0);
    setup_door_aot(0, 0, 0, dest_stage, dest_room,
                   100, -50, 200, 2048, 3);

    g_aot_action_pressed = 1;
    re15_aot_scan(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                  g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0xFF);

    if (!g_room_change.pending) {
        fprintf(stderr, "FAIL: room change should be pending\n");
        return 1;
    }
    if (g_room_change.room_id != want_id) {
        fprintf(stderr, "FAIL: room_id should be 0x%04X, got 0x%04X\n",
                want_id, g_room_change.room_id);
        return 1;
    }
    /* Stage muss gleich bleiben: high nibble der Ziel-ID == high nibble der aktuellen. */
    if ((g_room_change.room_id & 0xF000) != (g_current_room_id & 0xF000)) {
        fprintf(stderr, "FAIL: stage (high nibble) should be unchanged\n");
        return 1;
    }
    if (g_room_change.x != 100 || g_room_change.y != -50 || g_room_change.z != 200) {
        fprintf(stderr, "FAIL: spawn pos should be (100,-50,200), got (%d,%d,%d)\n",
                (int)g_room_change.x, (int)g_room_change.y, (int)g_room_change.z);
        return 1;
    }
    if (g_room_change.yaw_4096 != 2048) {
        fprintf(stderr, "FAIL: spawn yaw should be 2048, got %d\n",
                (int)g_room_change.yaw_4096);
        return 1;
    }
    if (g_room_change.target_cut != 3) {
        fprintf(stderr, "FAIL: target_cut should be 3, got %d\n",
                g_room_change.target_cut);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 2: Stage-Wechsel (andere Stage)
 *
 * Szenario: Tür führt zu einem Raum in einer ANDEREN Stage.
 * Erwartung:
 *   - g_room_change.pending = 1
 *   - g_room_change.room_id trägt die NEUE Stage im high nibble
 *     (byte-true Cross-Stage-Formel, aot_common.c:451-466)
 *   - Spawn-Position/Rotation korrekt
 *
 * Validates: Requirements 4.4
 * ========================================================================= */

static int test_stage_change_transition(void)
{
    printf("  test_stage_change_transition... ");

    test_reset_engine(0);

    /* Ziel: STAGE3 (dest_stage=2, 0-basiert) → high nibble der ID wird (2+1)=3. */
    uint8_t dest_stage = 2;     /* STAGE3 */
    uint8_t dest_room  = 0x05;
    unsigned want_id   = expected_dest_id(dest_stage, dest_room);

    place_player(0, 0);
    setup_door_aot(0, 0, 0, dest_stage, dest_room,
                   -300, 0, 500, 1024, 1);

    g_aot_action_pressed = 1;
    re15_aot_scan(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                  g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0xFF);

    if (!g_room_change.pending) {
        fprintf(stderr, "FAIL: room change should be pending for stage change\n");
        return 1;
    }
    if (g_room_change.room_id != want_id) {
        fprintf(stderr, "FAIL: room_id should be 0x%04X, got 0x%04X\n",
                want_id, g_room_change.room_id);
        return 1;
    }
    /* Stage MUSS sich geändert haben (high nibble != aktueller Raum). */
    if ((g_room_change.room_id & 0xF000) == (g_current_room_id & 0xF000)) {
        fprintf(stderr, "FAIL: stage (high nibble) should have changed\n");
        return 1;
    }
    /* Konkret: high nibble == (dest_stage+1) << 12. */
    if ((g_room_change.room_id & 0xF000) != (unsigned)((dest_stage + 1) << 12)) {
        fprintf(stderr, "FAIL: stage high nibble should be %u, got %u\n",
                (unsigned)(dest_stage + 1),
                (g_room_change.room_id >> 12) & 0xF);
        return 1;
    }
    if (g_room_change.x != -300 || g_room_change.z != 500) {
        fprintf(stderr, "FAIL: spawn pos should be (-300,_,500), got (%d,_,%d)\n",
                (int)g_room_change.x, (int)g_room_change.z);
        return 1;
    }
    if (g_room_change.yaw_4096 != 1024) {
        fprintf(stderr, "FAIL: spawn yaw should be 1024, got %d\n",
                (int)g_room_change.yaw_4096);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 3: Same-Room-Teleport (kein Raum-Neuladen)
 *
 * Szenario: DOOR-AOT mit dest_room == 0 → Same-Room/Self-Teleport
 * (aot_common.c:463 "dest_room 0 -> self/same-room teleport (falls through)").
 * Erwartung:
 *   - KEIN Raumwechsel angefordert (g_room_change.pending == 0)
 *   - Spieler-Actor wird DIREKT an Spawn-Position teleportiert
 *   - Kamera wird auf dest_cut umgeschaltet (g_scd.cam_id + cam_change_pending)
 *
 * Validates: Requirements 4.7
 * ========================================================================= */

static int test_same_room_teleport(void)
{
    printf("  test_same_room_teleport... ");

    test_reset_engine(0);

    place_player(0, 0);

    /* DOOR-AOT mit dest_room=0 → Same-Room-Teleport. Spawn (400,0,-200),
     * yaw 3072, Cut 5. Spawn darf NICHT (0,0,0) sein (aot_common.c:436 Guard). */
    setup_door_aot(0, 0, 0, /*dest_stage*/0, /*dest_room*/0,
                   400, 0, -200, 3072, 5);

    g_aot_action_pressed = 1;
    re15_aot_scan(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                  g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0xFF);

    /* KEIN Raumwechsel: Same-Room-Teleport lädt keinen Raum. */
    if (g_room_change.pending) {
        fprintf(stderr, "FAIL: same-room teleport should NOT queue a room change\n");
        return 1;
    }

    /* Spieler-Actor muss an Spawn liegen. */
    if (g_actors[RE15_ACTOR_SLOT_PLAYER].x != 400) {
        fprintf(stderr, "FAIL: player.x should be 400, got %d\n",
                (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x);
        return 1;
    }
    if (g_actors[RE15_ACTOR_SLOT_PLAYER].z != -200) {
        fprintf(stderr, "FAIL: player.z should be -200, got %d\n",
                (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        return 1;
    }
    if (g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y != 3072) {
        fprintf(stderr, "FAIL: player.rot_y should be 3072, got %d\n",
                (int)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y);
        return 1;
    }

    /* Kamera muss auf dest_cut (5) umgeschaltet sein. */
    if (g_scd.cam_id != 5) {
        fprintf(stderr, "FAIL: g_scd.cam_id should be 5, got %d\n", g_scd.cam_id);
        return 1;
    }
    if (!g_scd.cam_change_pending) {
        fprintf(stderr, "FAIL: g_scd.cam_change_pending should be set\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 4: Kein Trigger ohne Aktionstaste
 *
 * Szenario: Spieler steht vor Tür-Zone, drückt KEINE Aktionstaste.
 * Erwartung: Kein Raumwechsel ausgelöst (DOOR-Fire braucht g_aot_action_pressed,
 * aot_common.c:413).
 *
 * Validates: Requirements 4.2 (nur bei Aktionstaste)
 * ========================================================================= */

static int test_no_trigger_without_action_button(void)
{
    printf("  test_no_trigger_without_action_button... ");

    test_reset_engine(0);

    place_player(0, 0);
    setup_door_aot(0, 0, 0, 0, 0x14, 100, 0, 200, 2048, 3);

    /* KEINE Aktionstaste. */
    g_aot_action_pressed = 0;
    re15_aot_scan(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                  g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0xFF);

    if (g_room_change.pending) {
        fprintf(stderr, "FAIL: no room change without action button\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 5: Band-Mismatch verhindert Trigger
 *
 * Szenario: Spieler steht auf Band 4, die Tür gehört zu Band 0 (Pit).
 * Erwartung: DOOR feuert NICHT — der Band-Gate (aot_common.c:369-381,
 * byte-true FUN_8002bd44 @0x8002bf38) überspringt Türen mit Band-Mismatch.
 * Das ersetzt den alten "entered-Flag verhindert Re-Trigger"-Test durch
 * den AKTUELLEN Re-Trigger-/Erreichbarkeits-Schutz der Engine.
 *
 * Validates: Requirements 4.7 (Re-Trigger-/Erreichbarkeits-Schutz)
 * ========================================================================= */

static int test_band_mismatch_blocks_trigger(void)
{
    printf("  test_band_mismatch_blocks_trigger... ");

    /* Spieler-Band = 4 (Courtyard). */
    test_reset_engine(4);

    place_player(0, 0);
    /* setup_door_aot setzt door_params.band = 0 (Pit). Player-Band 4 != 0 → Gate. */
    setup_door_aot(0, 0, 0, 0, 0x14, 100, 0, 200, 2048, 3);

    g_aot_action_pressed = 1;
    re15_aot_scan(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                  g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0xFF);

    if (g_room_change.pending) {
        fprintf(stderr, "FAIL: band mismatch should block the door trigger\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 6: Forward-Reach-Geometrie — außerhalb der Reichweite kein Trigger
 *
 * Szenario: Spieler steht zu weit von der Tür entfernt (Reach-Punkt > ±900
 * vom Tür-Zentrum). Erwartung: kein Trigger (aot_common.c:368).
 *
 * Validates: Requirements 4.2 (Tür nur bei Erreichbarkeit)
 * ========================================================================= */

static int test_out_of_reach_no_trigger(void)
{
    printf("  test_out_of_reach_no_trigger... ");

    test_reset_engine(0);

    /* Tür-Zentrum für einen Spieler bei (0,0) (Reach-Punkt (563,0)). */
    place_player(0, 0);
    setup_door_aot(0, 0, 0, 0, 0x14, 100, 0, 200, 2048, 3);

    /* Spieler jetzt weit nach -X schieben: Reach-Punkt wandert mit, liegt
     * deutlich außerhalb von ±900 des Tür-Zentrums (563,0). */
    place_player(-5000, 0);

    g_aot_action_pressed = 1;
    re15_aot_scan(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                  g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0xFF);

    if (g_room_change.pending) {
        fprintf(stderr, "FAIL: out-of-reach door should NOT trigger\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(void)
{
    int failures = 0;

    printf("=== Integration Tests: Raumwechsel ===\n\n");

    failures += test_same_stage_room_change();
    failures += test_stage_change_transition();
    failures += test_same_room_teleport();
    failures += test_no_trigger_without_action_button();
    failures += test_band_mismatch_blocks_trigger();
    failures += test_out_of_reach_no_trigger();

    printf("\n");
    if (failures == 0) {
        printf("ALL ROOM TRANSITION INTEGRATION TESTS PASSED (%d tests)\n", 6);
    } else {
        fprintf(stderr, "%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
