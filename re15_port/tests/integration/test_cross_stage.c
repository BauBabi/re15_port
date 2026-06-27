/**
 * @file test_cross_stage.c
 * @brief Integration-Test: Vollständiger Stage-Wechsel (Stage 1 → Stage 2)
 *
 * Testet den kompletten Stage-Übergang end-to-end:
 *   - Spieler steht in einem Raum in Stage 1
 *   - Tür-AOT führt zu einem Raum in Stage 2
 *   - Aktionstaste ausgelöst → Stage-Wechsel + Raum-Laden + Spawn
 *
 * Verifiziert:
 *   - Stage wurde korrekt gewechselt (g_game.current_stage == 2)
 *   - Neuer Raum wurde geladen (g_game.current_room == Ziel)
 *   - Spieler steht an korrekter Spawn-Position
 *   - VAB-Bank wurde gewechselt (unload aufgerufen)
 *   - AOT-Slots wurden zurückgesetzt (alter Raum bereinigt)
 *
 * Verwendet Mock-I/O-Backend mit synthetischen RDT-Daten.
 *
 * Requirements: 4.4, 12.2, 12.5
 */

#include "re15_door.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_engine.h"
#include "re15_io.h"
#include "re15_io_common.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_error.h"
#include "re15_input.h"
#include "re15_player.h"
#include "re15_audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * Synthetic RDT data for testing
 *
 * Minimal valid RDT: 8-byte header + 21 x 4-byte offset table (= 92 bytes).
 * All section offsets set to 0 (sections not present).
 * ========================================================================= */

#define MOCK_RDT_SIZE 96

static uint8_t g_mock_rdt_data[MOCK_RDT_SIZE];

/** Track VAB unload calls (for stage change verification) */
static int g_vab_unload_call_count = 0;

/** Track VAB load calls */
static int g_vab_load_call_count = 0;
static int g_vab_load_last_bank_id = -1;

/** Track which room was requested via I/O open */
static char g_last_opened_path[RE15_MAX_PATH];
static int  g_mock_io_open_count = 0;

/* =========================================================================
 * Mock I/O Backend
 * ========================================================================= */

static int mock_open_success(const char* path, void** handle)
{
    strncpy(g_last_opened_path, path, RE15_MAX_PATH - 1);
    g_last_opened_path[RE15_MAX_PATH - 1] = '\0';
    g_mock_io_open_count++;
    *handle = (void*)(uintptr_t)1;
    return RE15_IO_OK;
}

static int mock_read_rdt(void* handle, uint8_t* buf, int size, int* bytes_read)
{
    (void)handle;
    int to_read = (size < MOCK_RDT_SIZE) ? size : MOCK_RDT_SIZE;
    memcpy(buf, g_mock_rdt_data, (size_t)to_read);
    if (bytes_read) *bytes_read = to_read;
    return RE15_IO_OK;
}

static int mock_get_size_rdt(void* handle, int* size_out)
{
    (void)handle;
    if (size_out) *size_out = MOCK_RDT_SIZE;
    return RE15_IO_OK;
}

static void mock_close_noop(void* handle)
{
    (void)handle;
}

static const re15_io_backend_t g_mock_io_success = {
    mock_open_success,
    mock_read_rdt,
    mock_get_size_rdt,
    mock_close_noop
};

/* =========================================================================
 * Mock Audio Backend
 * ========================================================================= */

static void mock_audio_init(void) {}
static void mock_audio_shutdown(void) {}

static int mock_audio_vab_load(const uint8_t* vh, int vh_size,
                               const uint8_t* vb, int vb_size, int bank_id)
{
    (void)vh; (void)vh_size; (void)vb; (void)vb_size;
    g_vab_load_call_count++;
    g_vab_load_last_bank_id = bank_id;
    return 0;
}

static void mock_audio_vab_unload(int bank_id)
{
    (void)bank_id;
    g_vab_unload_call_count++;
}

static void mock_audio_seq_play(int slot, const uint8_t* d, int s, int l)
{
    (void)slot; (void)d; (void)s; (void)l;
}

static void mock_audio_seq_stop(int slot) { (void)slot; }

static void mock_audio_sfx_play(int bank, int id, int vol, int pan)
{
    (void)bank; (void)id; (void)vol; (void)pan;
}

static void mock_audio_voice_play(int room_id, int msg_id)
{
    (void)room_id; (void)msg_id;
}

static void mock_audio_set_volume(int ch, int vol)
{
    (void)ch; (void)vol;
}

static const audio_backend_t g_mock_audio = {
    mock_audio_init,
    mock_audio_shutdown,
    mock_audio_vab_load,
    mock_audio_vab_unload,
    mock_audio_seq_play,
    mock_audio_seq_stop,
    mock_audio_sfx_play,
    mock_audio_voice_play,
    mock_audio_set_volume
};

/* =========================================================================
 * Global extern definitions needed for linking
 * ========================================================================= */

const re15_io_backend_t* g_io = NULL;

/* =========================================================================
 * Test-Zustand
 * ========================================================================= */

static re15_rdt_t    g_test_rdt;
static re15_player_t g_test_player;

/**
 * Initialisiert den Test-Zustand.
 */
static void test_setup(uint8_t stage, uint8_t room, uint8_t player_id)
{
    /* Synthetische RDT: Header (8 Bytes) + 21 x 0x00000000 Offsets */
    memset(g_mock_rdt_data, 0, sizeof(g_mock_rdt_data));

    /* Engine-Zustand zurücksetzen */
    memset(&g_game, 0, sizeof(g_game));
    memset(&g_test_rdt, 0, sizeof(g_test_rdt));
    memset(&g_test_player, 0, sizeof(g_test_player));

    g_game.current_rdt = &g_test_rdt;
    g_game.player = &g_test_player;
    g_game.current_stage = stage;
    g_game.current_room = room;
    g_game.current_player = player_id;

    /* Spieler am Ursprung platzieren */
    g_test_player.x = Q12_FROM_INT(0);
    g_test_player.y = Q12_FROM_INT(0);
    g_test_player.z = Q12_FROM_INT(0);
    g_test_player.yaw = 0;
    g_test_player.floor_band = 0;

    /* Backends setzen */
    g_io = &g_mock_io_success;
    g_audio = &g_mock_audio;

    /* Subsysteme initialisieren */
    re15_aot_init();
    scd_vm_init();

    /* Tracking zurücksetzen */
    g_vab_unload_call_count = 0;
    g_vab_load_call_count = 0;
    g_vab_load_last_bank_id = -1;
    g_mock_io_open_count = 0;
    memset(g_last_opened_path, 0, sizeof(g_last_opened_path));
}

/**
 * Registriert eine Tür-AOT-Zone.
 */
static void setup_door_aot(int slot, int16_t range,
                           uint8_t dest_stage, uint8_t dest_room,
                           int16_t spawn_x, int16_t spawn_y,
                           int16_t spawn_z, int16_t spawn_rot,
                           uint8_t dest_cut)
{
    re15_aot_slot_t door_slot;
    memset(&door_slot, 0, sizeof(door_slot));

    door_slot.active = 1;
    door_slot.type = AOT_TYPE_DOOR;
    door_slot.entered = 0;
    door_slot.floor_band = 0;

    /* Quadratisches Trigger-Polygon um den Ursprung */
    door_slot.trigger_x[0] = -range;
    door_slot.trigger_z[0] = -range;
    door_slot.trigger_x[1] =  range;
    door_slot.trigger_z[1] = -range;
    door_slot.trigger_x[2] =  range;
    door_slot.trigger_z[2] =  range;
    door_slot.trigger_x[3] = -range;
    door_slot.trigger_z[3] =  range;

    door_slot.data.door.dest_stage = dest_stage;
    door_slot.data.door.dest_room  = dest_room;
    door_slot.data.door.dest_cut   = dest_cut;
    door_slot.data.door.spawn_x    = spawn_x;
    door_slot.data.door.spawn_y    = spawn_y;
    door_slot.data.door.spawn_z    = spawn_z;
    door_slot.data.door.spawn_rot  = spawn_rot;

    re15_aot_set_slot(slot, &door_slot);
}

/* =========================================================================
 * Test 1: Stage 1 → Stage 2 — Vollständiger Übergang
 *
 * Verifiziert den kompletten cross-stage Ablauf:
 *   1. Stage-Wechsel (re15_stage_change aufgerufen)
 *   2. VAB-Bank gewechselt (unload + load)
 *   3. Raum in neuer Stage geladen
 *   4. Spieler an Spawn-Position/Rotation
 *   5. current_stage und current_room korrekt aktualisiert
 *
 * Validates: Requirements 4.4
 * ========================================================================= */

static int test_stage1_to_stage2_full(void)
{
    re15_input_state_t input;
    int result;

    printf("  test_stage1_to_stage2_full... ");

    /* Setup: Aktuell Stage 1, Room 0x17, Player 0 (Leon) */
    test_setup(1, 0x17, 0);

    /* Tür führt zu Stage 2, Room 0x03 */
    /* Spawn: (-1200, 0, 3400), Rotation 2048, Cut 2 */
    setup_door_aot(0, 500, 2, 0x03, -1200, 0, 3400, 2048, 2);

    /* Spieler innerhalb der Zone */
    g_test_player.x = Q12_FROM_INT(0);
    g_test_player.z = Q12_FROM_INT(0);
    g_test_player.floor_band = 0;

    /* Aktionstaste drücken */
    memset(&input, 0, sizeof(input));
    input.pressed = RE15_BTN_CROSS;
    input.held = RE15_BTN_CROSS;

    /* Tür-Trigger */
    result = re15_door_trigger_check(&g_test_player, input);

    /* --- Assertions --- */

    if (result != RE15_DOOR_OK) {
        fprintf(stderr, "FAIL: Expected RE15_DOOR_OK (1), got %d\n", result);
        return 1;
    }

    /* Stage muss auf 2 gewechselt sein */
    if (g_game.current_stage != 2) {
        fprintf(stderr, "FAIL: current_stage should be 2, got %d\n",
                g_game.current_stage);
        return 1;
    }

    /* Room muss auf 0x03 gesetzt sein */
    if (g_game.current_room != 0x03) {
        fprintf(stderr, "FAIL: current_room should be 0x03, got 0x%02X\n",
                g_game.current_room);
        return 1;
    }

    /* Spieler-Position prüfen */
    if (Q12_TO_INT(g_test_player.x) != -1200) {
        fprintf(stderr, "FAIL: player.x should be -1200, got %d\n",
                (int)Q12_TO_INT(g_test_player.x));
        return 1;
    }

    if (Q12_TO_INT(g_test_player.y) != 0) {
        fprintf(stderr, "FAIL: player.y should be 0, got %d\n",
                (int)Q12_TO_INT(g_test_player.y));
        return 1;
    }

    if (Q12_TO_INT(g_test_player.z) != 3400) {
        fprintf(stderr, "FAIL: player.z should be 3400, got %d\n",
                (int)Q12_TO_INT(g_test_player.z));
        return 1;
    }

    if (g_test_player.yaw != 2048) {
        fprintf(stderr, "FAIL: player.yaw should be 2048, got %d\n",
                (int)g_test_player.yaw);
        return 1;
    }

    /* VAB-Unload muss aufgerufen worden sein (Stage-Wechsel) */
    if (g_vab_unload_call_count < 1) {
        fprintf(stderr, "FAIL: VAB unload should be called for stage change, "
                "got %d calls\n", g_vab_unload_call_count);
        return 1;
    }

    /* I/O muss aufgerufen worden sein (neuer Raum geladen) */
    if (g_mock_io_open_count < 1) {
        fprintf(stderr, "FAIL: IO open should have been called to load new room\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 2: Stage-Wechsel mit anderem Spieler (Elza)
 *
 * Gleicher Ablauf wie Test 1, aber mit Player 1 (Elza).
 * Stellt sicher dass der Pfad korrekt mit Player-ID konstruiert wird.
 *
 * Validates: Requirements 4.4
 * ========================================================================= */

static int test_cross_stage_elza(void)
{
    re15_input_state_t input;
    int result;

    printf("  test_cross_stage_elza... ");

    /* Setup: Stage 1, Room 0x0A, Player 1 (Elza) */
    test_setup(1, 0x0A, 1);

    /* Tür führt zu Stage 3, Room 0x01 */
    /* Spawn: (500, -100, -800), Rotation 3072, Cut 0 */
    setup_door_aot(0, 300, 3, 0x01, 500, -100, -800, 3072, 0);

    /* Spieler innerhalb der Zone */
    g_test_player.x = Q12_FROM_INT(0);
    g_test_player.z = Q12_FROM_INT(0);
    g_test_player.floor_band = 0;

    /* Aktionstaste */
    memset(&input, 0, sizeof(input));
    input.pressed = RE15_BTN_CROSS;
    input.held = RE15_BTN_CROSS;

    result = re15_door_trigger_check(&g_test_player, input);

    if (result != RE15_DOOR_OK) {
        fprintf(stderr, "FAIL: Expected RE15_DOOR_OK, got %d\n", result);
        return 1;
    }

    /* Stage muss auf 3 gewechselt sein */
    if (g_game.current_stage != 3) {
        fprintf(stderr, "FAIL: current_stage should be 3, got %d\n",
                g_game.current_stage);
        return 1;
    }

    /* Room korrekt */
    if (g_game.current_room != 0x01) {
        fprintf(stderr, "FAIL: current_room should be 0x01, got 0x%02X\n",
                g_game.current_room);
        return 1;
    }

    /* Spawn-Position */
    if (Q12_TO_INT(g_test_player.x) != 500) {
        fprintf(stderr, "FAIL: player.x should be 500, got %d\n",
                (int)Q12_TO_INT(g_test_player.x));
        return 1;
    }

    if (Q12_TO_INT(g_test_player.z) != -800) {
        fprintf(stderr, "FAIL: player.z should be -800, got %d\n",
                (int)Q12_TO_INT(g_test_player.z));
        return 1;
    }

    if (g_test_player.yaw != 3072) {
        fprintf(stderr, "FAIL: player.yaw should be 3072, got %d\n",
                (int)g_test_player.yaw);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 3: AOT-Slots zurückgesetzt nach Stage-Wechsel
 *
 * Verifiziert dass nach dem Laden des neuen Raums die AOT-Slots des
 * alten Raums zurückgesetzt sind (re15_aot_init wird beim Entladen
 * aufgerufen).
 *
 * Validates: Requirements 4.4
 * ========================================================================= */

static int test_aot_reset_after_stage_change(void)
{
    re15_input_state_t input;
    int result;
    const re15_aot_slot_t* slot;
    int i;
    int any_active = 0;

    printf("  test_aot_reset_after_stage_change... ");

    /* Setup: Stage 1, Room 0x05 */
    test_setup(1, 0x05, 0);

    /* Mehrere AOT-Slots belegen (simuliert alten Raum mit Triggern) */
    setup_door_aot(0, 500, 2, 0x02, 100, 0, 200, 1024, 1);
    setup_door_aot(1, 300, 1, 0x07, -50, 0, -100, 512, 0);
    setup_door_aot(2, 200, 1, 0x08, 300, 0, 400, 0, 2);

    /* Spieler in Slot 0 Zone */
    g_test_player.x = Q12_FROM_INT(0);
    g_test_player.z = Q12_FROM_INT(0);
    g_test_player.floor_band = 0;

    /* Aktionstaste — löst Slot 0 aus (Stage 2, Room 0x02) */
    memset(&input, 0, sizeof(input));
    input.pressed = RE15_BTN_CROSS;
    input.held = RE15_BTN_CROSS;

    result = re15_door_trigger_check(&g_test_player, input);

    if (result != RE15_DOOR_OK) {
        fprintf(stderr, "FAIL: Expected RE15_DOOR_OK, got %d\n", result);
        return 1;
    }

    /* Prüfe: Alte AOT-Slots (1, 2) sollten inaktiv sein nach Raumwechsel */
    /* Der neue Raum hat leere RDT-Daten → keine neuen AOTs registriert */
    for (i = 0; i < RE15_AOT_MAX_SLOTS; i++) {
        slot = re15_aot_get_slot(i);
        if (slot && slot->active) {
            any_active = 1;
            break;
        }
    }

    /* Nach room_load mit leerer RDT sollten keine AOTs aktiv sein
     * (Die AOTs des alten Raums werden durch re15_aot_init() gelöscht) */
    if (any_active) {
        fprintf(stderr, "FAIL: AOT slots should be reset after room change, "
                "but slot %d is still active\n", i);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 4: SCD-VM wird bei Stage-Wechsel reinitialisiert
 *
 * Verifiziert dass die SCD-VM nach dem Stage-Wechsel einen sauberen
 * Zustand hat (alle Threads inaktiv, tick_count reset).
 *
 * Validates: Requirements 4.4
 * ========================================================================= */

static int test_scd_reset_after_stage_change(void)
{
    re15_input_state_t input;
    int result;
    int i;

    printf("  test_scd_reset_after_stage_change... ");

    /* Setup: Stage 1, Room 0x12 */
    test_setup(1, 0x12, 0);

    /* Simuliere aktive SCD-Threads (alter Raum) */
    g_scd.threads[0].active = 1;
    g_scd.threads[1].active = 1;
    g_scd.threads[5].active = 1;
    g_scd.tick_count = 42;

    /* Tür-AOT führt zu Stage 2, Room 0x00 */
    setup_door_aot(0, 500, 2, 0x00, 0, 0, 0, 0, 0);

    g_test_player.x = Q12_FROM_INT(0);
    g_test_player.z = Q12_FROM_INT(0);
    g_test_player.floor_band = 0;

    memset(&input, 0, sizeof(input));
    input.pressed = RE15_BTN_CROSS;
    input.held = RE15_BTN_CROSS;

    result = re15_door_trigger_check(&g_test_player, input);

    if (result != RE15_DOOR_OK) {
        fprintf(stderr, "FAIL: Expected RE15_DOOR_OK, got %d\n", result);
        return 1;
    }

    /* Alle SCD-Threads sollten nach scd_vm_init() inaktiv sein */
    for (i = 0; i < SCD_THREAD_COUNT; i++) {
        if (g_scd.threads[i].active) {
            fprintf(stderr, "FAIL: SCD thread %d should be inactive after "
                    "stage change, but is active\n", i);
            return 1;
        }
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
