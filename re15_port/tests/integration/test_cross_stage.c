/**
 * @file test_cross_stage.c
 * @brief Integration-Test: Cross-Stage Tür-Übergang (Stage 1 -> Stage 2/3)
 *
 * Reaktiviert 2026-06-28 gegen die AKTUELLE Engine-API (Engine-Transplant).
 *
 * HINTERGRUND DER NEUFASSUNG
 * ==========================
 * Der ursprüngliche Test rief eine monolithische Funktion
 *   re15_door_trigger_check(player, input)
 * auf, die den GANZEN Raumwechsel synchron erledigte und einen
 * `g_game`-Engine-Zustand (current_stage/current_room/player) mutierte, plus
 * Mock-I/O- und Mock-Audio-VTABLE-Backends (re15_io_backend_t / audio_backend_t).
 *
 * In der aktuellen Engine existiert NICHTS davon mehr:
 *   - `re15_door.h` / re15_door_trigger_check  -> entfernt. Türen sind jetzt
 *     reine DOOR-AOTs; die Tür "feuert" im AOT-Scanner (re15_aot_scan), der bei
 *     Aktion + Forward-Reach EINEN cross-room Transition über
 *     re15_room_request_change() in `g_room_change` QUEUET (room_common.c:34).
 *     Der eigentliche Apply (Stage/Room/Player setzen) läuft danach in
 *     re15_room_apply_pending() über port-spezifische Callbacks — daher ist das
 *     header-prüfbare Observable die GEQUEUETE Transition (g_room_change).
 *   - `g_game` ist jetzt re15_game_state_t (nur Flag-Tabellen, re15_scd.h:340),
 *     KEIN current_stage/current_room/player mehr.
 *   - re15_player_t / re15_input_state_t / re15_io_backend_t / audio_backend_t
 *     existieren nicht mehr (kein `re15_player.h`-Struct, kein I/O-VTABLE,
 *     Audio ist re15_audio_state_t + freie re15_audio_*-Funktionen).
 *   - re15_aot_slot_t (Union door/item/...) -> re15_aot_t (g_aot.slots[]) +
 *     parallele re15_aot_door_params_t (g_aot.door_params[]); öffentliche Setter
 *     re15_aot_set_door() (re15_aot.h:160).
 *
 * Die TEST-INTENTION bleibt erhalten: ein DOOR-AOT, dessen Ziel in einer ANDEREN
 * Stage liegt, löst bei Aktionstaste einen korrekt adressierten cross-stage
 * Raumwechsel mit korrekter Spawn-Position aus; AOT- und SCD-Zustand werden beim
 * Raumwechsel zurückgesetzt. Verifiziert wird das am AKTUELLEN API-Seam
 * (g_room_change + re15_aot_init/scd_vm_init) statt am alten g_game-Zustand.
 *
 * Verwendete aktuelle öffentliche API:
 *   - re15_aot_init / re15_aot_set_door / re15_aot_scan      (re15_aot.h)
 *   - g_aot.slots[] / g_aot.door_params[]                    (re15_aot.h:127-128)
 *   - g_aot_action_pressed                                   (re15_aot.h:150)
 *   - re15_room_request_change / g_room_change / re15_room_change_t (re15_room.h:39-52)
 *   - g_current_room_id                                      (re15_room.h:22)
 *   - g_actors[RE15_ACTOR_SLOT_PLAYER]                       (re15_actor.h:167)
 *   - re15_collision_reset_band                              (re15_collision.h:35)
 *   - scd_vm_init / g_scd / scd_thread_t                     (re15_scd.h)
 *
 * Requirements: 4.4, 12.2, 12.5
 */

#include "re15_aot.h"
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_collision.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* =========================================================================
 * Test-Konstanten
 *
 * Die DOOR-Fire-Geometrie ist byte-true der Forward-Reach (aot_common.c:362-368):
 * ein Punkt 563 Einheiten VOR dem Spieler (in Blickrichtung rot_y) muss
 * innerhalb +-900 des Tür-ZENTRUMS liegen. Mit rot_y == 0 zeigt der Spieler nach
 * +X (forward = (cos0, -sin0) = (1,0)). Steht der Spieler im Ursprung (0,0) und
 * sitzt das Tür-Zentrum ebenfalls im Ursprung, liegt der Forward-Punkt bei
 * (+563, 0): |563| <= 900 und |0| <= 900 -> die Tür feuert.
 * ========================================================================= */

/* Cross-room dest-id-Formel (byte-true aot_common.c:464):
 *   dest_id = ((dest_stage+1) << 12) | (dest_room << 4) | (g_current_room_id & 0xF)
 * dest_stage ist 0-basiert (0 = STAGE1). g_current_room_id bootet als 0x1170
 * (room_common.c:31), low nibble = 0. */
static unsigned expected_dest_id(uint8_t dest_stage, uint8_t dest_room)
{
    return (((unsigned)dest_stage + 1u) << 12)
         | ((unsigned)dest_room << 4)
         | (g_current_room_id & 0x000Fu);
}

/* =========================================================================
 * Test-Setup
 * ========================================================================= */

/**
 * Setzt Engine-Zustand für einen frischen Frame zurück.
 *
 * Anders als die Legacy-Variante mutieren wir KEINEN globalen Spielzustand
 * (es gibt keinen current_stage/current_room mehr). Wir positionieren den
 * Spieler-Actor (g_actors[0]) im Ursprung mit rot_y==0 und stellen den
 * AOT-/SCD-/Room-Change-Zustand zurück.
 */
static void test_setup(void)
{
    /* Spieler = Actor-Slot 0 (RE2-pure, re15_actor.h:23). */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof(*pl));
    pl->active = 1;
    pl->x      = 0;
    pl->y      = 0;
    pl->z      = 0;
    pl->rot_y  = 0;        /* facing +X -> forward-reach point at (+563, 0) */

    /* Floor-Band auf -1 (unbekannt) -> der Door-Band-Gate (aot_common.c:380)
     * wird übersprungen (`if (pb >= 0 && ...)`), die Tür feuert bandunabhängig. */
    re15_collision_reset_band();

    /* Subsysteme initialisieren. */
    re15_aot_init();
    scd_vm_init();

    /* Room-Change-Queue + Cinematic/Message-Gate sicher klar. */
    memset(&g_room_change, 0, sizeof(g_room_change));
    g_scd.player_mode          = 0;   /* nicht scripted -> in_cinematic=0 */
    g_scd.letterbox_countdown  = 0;
    g_scd.message_query        = 0;
    g_scd.message_display_frames = 0;
    g_aot_action_pressed       = 0;
}

/**
 * Registriert eine DOOR-AOT in `slot` mit Ziel-Stage/Room + Spawn.
 *
 * re15_aot_set_door() (re15_aot.h:160) setzt Rect + target_cut + Spawn; die
 * Ziel-Stage/Room liegen in g_aot.door_params[] (re15_aot.h:99-101), die der
 * SCD-Opcode Door_aot_set normalerweise füllt — hier direkt geschrieben, da
 * `g_aot` öffentlich (extern, re15_aot.h:145) ist.
 *
 * Rect-Zentrum = Ursprung, half-extents = `range` (echtes Rechteck, KEIN
 * (0,0)-Sentinel -> normale action-gated Lauftür, nicht die Auto-Advance-Tür).
 */
static void setup_door_aot(int slot, int32_t range,
                           uint8_t dest_stage, uint8_t dest_room,
                           int32_t spawn_x, int32_t spawn_y,
                           int32_t spawn_z, int16_t spawn_yaw,
                           uint8_t dest_cut)
{
    re15_aot_set_door(slot,
                      /* cx, cz   */ 0, 0,
                      /* half_w/h */ range, range,
                      /* target_cut */ dest_cut,
                      spawn_x, spawn_y, spawn_z, spawn_yaw);

    /* Ziel-Stage/Room (Door_aot_set pc[22]/pc[23]). */
    g_aot.door_params[slot].dest_stage = dest_stage;
    g_aot.door_params[slot].dest_room  = dest_room;

    /* was_inside auf 0 lassen (frisch via re15_aot_set -> re15_aot_set_door). */
}

/* =========================================================================
 * Test 1: Stage 1 -> Stage 2 — cross-stage Tür queued korrekt
 *
 * Verifiziert: nach einem Aktions-Press im Tür-Forward-Reach queuet
 * re15_aot_scan einen Raumwechsel (g_room_change.pending) mit
 *   - korrekter Ziel-Raum-ID (Stage 2 / Room 0x03 -> 0x2030)
 *   - korrekter Spawn-Position/Rotation/Cut.
 *
 * Validates: Requirements 4.4
 * ========================================================================= */
static int test_stage1_to_stage2_full(void)
{
    printf("  test_stage1_to_stage2_full... ");

    test_setup();

    /* Tür -> Stage 2 (dest_stage=1), Room 0x03; Spawn (-1200,0,3400), yaw 2048, cut 2 */
    setup_door_aot(0, 500, /*dest_stage*/1, /*dest_room*/0x03,
                   -1200, 0, 3400, 2048, /*dest_cut*/2);

    /* Spieler im Ursprung, Aktionstaste gedrückt. */
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = 0;
    g_aot_action_pressed = 1;

    /* AOT-Scan (active_cut beliebig — DOOR ignoriert cam_from-Filter). */
    re15_aot_scan(0, 0, /*active_cut*/0);

    /* --- Assertions: cross-room Transition wurde QUEUED --- */
    if (!g_room_change.pending) {
        fprintf(stderr, "FAIL: door scan should have queued a room change\n");
        return 1;
    }

    unsigned want = expected_dest_id(1, 0x03);
    if (g_room_change.room_id != want) {
        fprintf(stderr, "FAIL: dest room_id should be 0x%04X (stage2/room0x03), got 0x%04X\n",
                want, g_room_change.room_id);
        return 1;
    }

    if (g_room_change.x != -1200) {
        fprintf(stderr, "FAIL: spawn x should be -1200, got %d\n", (int)g_room_change.x);
        return 1;
    }
    if (g_room_change.y != 0) {
        fprintf(stderr, "FAIL: spawn y should be 0, got %d\n", (int)g_room_change.y);
        return 1;
    }
    if (g_room_change.z != 3400) {
        fprintf(stderr, "FAIL: spawn z should be 3400, got %d\n", (int)g_room_change.z);
        return 1;
    }
    if (g_room_change.yaw_4096 != 2048) {
        fprintf(stderr, "FAIL: spawn yaw should be 2048, got %d\n", (int)g_room_change.yaw_4096);
        return 1;
    }
    if (g_room_change.target_cut != 2) {
        fprintf(stderr, "FAIL: target_cut should be 2, got %d\n", g_room_change.target_cut);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 2: cross-stage zu Stage 3 (anderer Pfad)
 *
 * Gleiche Logik, aber dest_stage=2 (=Stage 3), Room 0x01. Stellt sicher dass
 * die Ziel-Raum-ID korrekt aus dest_stage konstruiert wird (NICHT auf Stage 1
 * hardcoded — die alte Engine-Bug-Klasse, aot_common.c:453-456).
 *
 * Validates: Requirements 4.4
 * ========================================================================= */
static int test_cross_stage_elza(void)
{
    printf("  test_cross_stage_elza... ");

    test_setup();

    /* Tür -> Stage 3 (dest_stage=2), Room 0x01; Spawn (500,-100,-800), yaw 3072, cut 0 */
    setup_door_aot(0, 300, /*dest_stage*/2, /*dest_room*/0x01,
                   500, -100, -800, 3072, /*dest_cut*/0);

    g_actors[RE15_ACTOR_SLOT_PLAYER].x = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = 0;
    g_aot_action_pressed = 1;

    re15_aot_scan(0, 0, 0);

    if (!g_room_change.pending) {
        fprintf(stderr, "FAIL: door scan should have queued a room change\n");
        return 1;
    }

    unsigned want = expected_dest_id(2, 0x01);   /* (3<<12)|(1<<4)|0 = 0x3010 */
    if (g_room_change.room_id != want) {
        fprintf(stderr, "FAIL: dest room_id should be 0x%04X (stage3/room0x01), got 0x%04X\n",
                want, g_room_change.room_id);
        return 1;
    }

    if (g_room_change.x != 500) {
        fprintf(stderr, "FAIL: spawn x should be 500, got %d\n", (int)g_room_change.x);
        return 1;
    }
    if (g_room_change.y != -100) {
        fprintf(stderr, "FAIL: spawn y should be -100, got %d\n", (int)g_room_change.y);
        return 1;
    }
    if (g_room_change.z != -800) {
        fprintf(stderr, "FAIL: spawn z should be -800, got %d\n", (int)g_room_change.z);
        return 1;
    }
    if (g_room_change.yaw_4096 != 3072) {
        fprintf(stderr, "FAIL: spawn yaw should be 3072, got %d\n", (int)g_room_change.yaw_4096);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 3: AOT-Slots werden beim Raum-Init zurückgesetzt
 *
 * Beim Raumwechsel re-initialisiert die Engine die AOTs des alten Raums
 * (re15_aot_init -> alle Slots active=0). Verifiziert die Reset-Invariante:
 * mehrere belegte Slots -> nach re15_aot_init keiner mehr aktiv.
 *
 * Validates: Requirements 4.4
 * ========================================================================= */
static int test_aot_reset_after_stage_change(void)
{
    printf("  test_aot_reset_after_stage_change... ");

    test_setup();

    /* Mehrere AOT-Slots belegen (simuliert alten Raum mit Triggern). */
    setup_door_aot(0, 500, 1, 0x02, 100, 0, 200, 1024, 1);
    setup_door_aot(1, 300, 0, 0x07, -50, 0, -100, 512, 0);
    setup_door_aot(2, 200, 0, 0x08, 300, 0, 400, 0, 2);

    /* Sanity: mindestens ein Slot ist jetzt aktiv. */
    int active_before = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active) { active_before = 1; break; }
    if (!active_before) {
        fprintf(stderr, "FAIL: setup should have left at least one AOT active\n");
        return 1;
    }

    /* Raumwechsel-Reset: re15_aot_init löscht alle Slots des alten Raums. */
    re15_aot_init();

    for (int i = 0; i < RE15_AOT_MAX; i++) {
        if (g_aot.slots[i].active) {
            fprintf(stderr, "FAIL: AOT slot %d still active after re15_aot_init\n", i);
            return 1;
        }
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 4: SCD-VM wird beim Raumwechsel reinitialisiert
 *
 * Verifiziert dass scd_vm_init() einen sauberen Zustand herstellt (alle
 * Threads inaktiv, tick_count == 0) — das tut die Engine beim Raumwechsel.
 *
 * Validates: Requirements 4.4
 * ========================================================================= */
static int test_scd_reset_after_stage_change(void)
{
    printf("  test_scd_reset_after_stage_change... ");

    test_setup();

    /* Simuliere aktive SCD-Threads + tick_count (alter Raum). */
    g_scd.threads[0].active = 1;
    g_scd.threads[1].active = 1;
    g_scd.threads[5].active = 1;
    g_scd.tick_count        = 42;

    /* Raumwechsel-Reset. */
    scd_vm_init();

    for (int i = 0; i < SCD_THREAD_COUNT; i++) {
        if (g_scd.threads[i].active) {
            fprintf(stderr, "FAIL: SCD thread %d should be inactive after scd_vm_init\n", i);
            return 1;
        }
    }
    if (g_scd.tick_count != 0) {
        fprintf(stderr, "FAIL: SCD tick_count should be 0 after scd_vm_init, got %u\n",
                g_scd.tick_count);
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

    printf("=== Integration Tests: Cross-Stage Transition ===\n\n");

    failures += test_stage1_to_stage2_full();
    failures += test_cross_stage_elza();
    failures += test_aot_reset_after_stage_change();
    failures += test_scd_reset_after_stage_change();

    printf("\n");
    if (failures == 0) {
        printf("ALL CROSS-STAGE INTEGRATION TESTS PASSED (%d tests)\n", 4);
    } else {
        fprintf(stderr, "%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
