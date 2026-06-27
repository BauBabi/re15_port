/**
 * @file test_room_transition.c
 * @brief Integration-Tests: Raumwechsel (gleiche Stage, Stage-Wechsel, Same-Room-Teleport)
 *
 * Testet die Zusammenarbeit von AOT-System, Tür-Trigger-Logik, Raum-Lade-System
 * und Stage-Wechsel-Logik als integriertes Subsystem.
 *
 * Verwendet Mock-I/O-Backend mit synthetischen RDT-Daten, um:
 *   - Gleiche-Stage Raumwechsel (Entladen/Laden/Spawn korrekt)
 *   - Stage-Wechsel (Overlay + VAB-Bank wechseln)
 *   - Same-Room-Teleport (kein Neuladen, nur Position + Kamera)
 * zu validieren.
 *
 * Requirements: 4.2, 4.3, 4.4, 4.7
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

/** Track how many times stage_change was called and with what parameter */
static uint8_t g_stage_change_called_with = 0;
static int     g_stage_change_call_count = 0;

/** Track VAB unload calls (for stage change verification) */
static int     g_vab_unload_call_count = 0;

/** Track which room was requested via I/O open */
static char    g_last_opened_path[RE15_MAX_PATH];
static int     g_mock_io_open_count = 0;

/* =========================================================================
 * Mock I/O Backend: Returns synthetic RDT data for any open/read request
 * ========================================================================= */

static int mock_open_success(const char* path, void** handle)
{
    /* Record the path for verification */
    strncpy(g_last_opened_path, path, RE15_MAX_PATH - 1);
    g_last_opened_path[RE15_MAX_PATH - 1] = '\0';
    g_mock_io_open_count++;

    /* Return a non-NULL opaque handle */
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
 * Mock I/O Backend: Fails on open (for error-path testing)
 * ========================================================================= */

static int mock_open_fail(const char* path, void** handle)
{
    (void)path;
    (void)handle;
    return RE15_IO_NOT_FOUND;
}

static const re15_io_backend_t g_mock_io_fail = {
    mock_open_fail,
    mock_read_rdt,
    mock_get_size_rdt,
    mock_close_noop
};

/* =========================================================================
 * Mock Audio Backend (tracks VAB unload calls for stage-change test)
 * ========================================================================= */

static void mock_audio_init(void) {}
static void mock_audio_shutdown(void) {}

static int mock_audio_vab_load(const uint8_t* vh, int vh_size,
                               const uint8_t* vb, int vb_size, int bank_id)
{
    (void)vh; (void)vh_size; (void)vb; (void)vb_size; (void)bank_id;
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
 *
 * g_io is not defined in the engine library (platform-specific backends define
 * it), so we provide it here. g_audio is defined in engine/src/stub.c, so we
 * only reference it via extern (already declared in re15_audio.h).
 * ========================================================================= */

const re15_io_backend_t* g_io = NULL;

/* =========================================================================
 * Test helper: RDT-Struktur und Spieler-Entität für Tests
 * ========================================================================= */

static re15_rdt_t    g_test_rdt;
static re15_player_t g_test_player;

/**
 * Initialisiert Mock-RDT-Daten (alle Sektions-Offsets = 0 → NULL-Pointer).
 * Setzt den Engine-Zustand auf einen definierten Ausgangszustand.
 */
static void test_setup(uint8_t stage, uint8_t room, uint8_t player_id)
{
    /* Synthetische RDT: Header (8 Bytes) + 21 x 0x00000000 Offsets */
    memset(g_mock_rdt_data, 0, sizeof(g_mock_rdt_data));
    /* Minimal RDT header identifier (first 8 bytes) */
    g_mock_rdt_data[0] = 0x00; /* Keine spezielle Kennung nötig für den Parser */

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

    /* I/O auf Success-Mock setzen */
    g_io = &g_mock_io_success;

    /* Audio-Mock setzen */
    g_audio = &g_mock_audio;

    /* AOT-System und SCD-VM initialisieren */
    re15_aot_init();
    scd_vm_init();

    /* Tracking-Variablen zurücksetzen */
    g_stage_change_called_with = 0;
    g_stage_change_call_count = 0;
    g_vab_unload_call_count = 0;
    g_mock_io_open_count = 0;
    memset(g_last_opened_path, 0, sizeof(g_last_opened_path));
}

/**
 * Registriert eine Tür-AOT-Zone im Slot-System.
 * Die Zone ist ein Quadrat um den Ursprung (±range).
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
 * Test 1: Gleiche-Stage Raumwechsel
 *
 * Szenario: Spieler steht in Tür-Zone, drückt Aktionstaste.
 * Tür führt zu anderem Raum in gleicher Stage.
 * Erwartung:
 *   - Aktueller Raum wird entladen (rdt_buffer freigegeben)
 *   - Zielraum wird geladen (I/O wird aufgerufen)
 *   - Spieler wird an Spawn-Position/Rotation gesetzt
 *   - current_stage bleibt unverändert
 *   - current_room wird auf Zielraum aktualisiert
 *
 * Validates: Requirements 4.2, 4.3
 * ========================================================================= */

static int test_same_stage_room_change(void)
{
    re15_input_state_t input;
    int result;

    printf("  test_same_stage_room_change... ");

    /* Setup: Stage 1, Room 0x17, Player 0 */
    test_setup(1, 0x17, 0);

    /* Tür-AOT registrieren: führt zu Stage 1, Room 0x14 */
    /* Spawn bei (100, -50, 200), Rotation 2048, Cut 3 */
    setup_door_aot(0, 500, 1, 0x14, 100, -50, 200, 2048, 3);

    /* Spieler steht am Ursprung (innerhalb der Tür-Zone) */
    g_test_player.x = Q12_FROM_INT(0);
    g_test_player.z = Q12_FROM_INT(0);
    g_test_player.floor_band = 0;

    /* Input: Aktionstaste (Cross) gedrückt */
    memset(&input, 0, sizeof(input));
    input.pressed = RE15_BTN_CROSS;
    input.held = RE15_BTN_CROSS;

    /* Tür-Trigger ausführen */
    result = re15_door_trigger_check(&g_test_player, input);

    /* Prüfungen */
    if (result != RE15_DOOR_OK) {
        fprintf(stderr, "FAIL: Expected RE15_DOOR_OK (1), got %d\n", result);
        return 1;
    }

    /* Stage muss unverändert sein (gleicher Stage-Wechsel) */
    if (g_game.current_stage != 1) {
        fprintf(stderr, "FAIL: current_stage should be 1, got %d\n",
                g_game.current_stage);
        return 1;
    }

    /* Room muss auf Zielraum 0x14 aktualisiert sein */
    if (g_game.current_room != 0x14) {
        fprintf(stderr, "FAIL: current_room should be 0x14, got 0x%02X\n",
                g_game.current_room);
        return 1;
    }

    /* Spieler-Position muss an Spawn-Werten liegen */
    if (Q12_TO_INT(g_test_player.x) != 100) {
        fprintf(stderr, "FAIL: player.x should be 100, got %d\n",
                (int)Q12_TO_INT(g_test_player.x));
        return 1;
    }

    if (Q12_TO_INT(g_test_player.y) != -50) {
        fprintf(stderr, "FAIL: player.y should be -50, got %d\n",
                (int)Q12_TO_INT(g_test_player.y));
        return 1;
    }

    if (Q12_TO_INT(g_test_player.z) != 200) {
        fprintf(stderr, "FAIL: player.z should be 200, got %d\n",
                (int)Q12_TO_INT(g_test_player.z));
        return 1;
    }

    if (g_test_player.yaw != 2048) {
        fprintf(stderr, "FAIL: player.yaw should be 2048, got %d\n",
                (int)g_test_player.yaw);
        return 1;
    }

    /* I/O-Backend muss aufgerufen worden sein (Datei geladen) */
    if (g_mock_io_open_count < 1) {
        fprintf(stderr, "FAIL: IO open should have been called\n");
        return 1;
    }

    /* VAB-Unload sollte NICHT aufgerufen werden (gleiche Stage) */
    if (g_vab_unload_call_count != 0) {
        fprintf(stderr, "FAIL: VAB unload should not be called for same-stage, "
                "got %d calls\n", g_vab_unload_call_count);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 2: Stage-Wechsel (Overlay + VAB-Bank wechseln)
 *
 * Szenario: Spieler steht in Tür-Zone, Tür führt zu anderer Stage.
 * Erwartung:
 *   - re15_stage_change() wird aufgerufen (Overlay laden, VAB wechseln)
 *   - VAB-Bank wird entladen (unload aufgerufen)
 *   - Zielraum in neuer Stage wird geladen
 *   - current_stage wird auf Ziel-Stage aktualisiert
 *   - Spieler bei korrekter Spawn-Position
 *
 * Validates: Requirements 4.4
 * ========================================================================= */

static int test_stage_change_transition(void)
{
    re15_input_state_t input;
    int result;

    printf("  test_stage_change_transition... ");

    /* Setup: Aktuell in Stage 1, Room 0x00, Player 0 */
    test_setup(1, 0x00, 0);

    /* Tür-AOT: führt zu Stage 2, Room 0x05 */
    /* Spawn bei (-300, 0, 500), Rotation 1024, Cut 1 */
    setup_door_aot(0, 500, 2, 0x05, -300, 0, 500, 1024, 1);

    /* Spieler am Ursprung (innerhalb der Zone) */
    g_test_player.x = Q12_FROM_INT(0);
    g_test_player.z = Q12_FROM_INT(0);
    g_test_player.floor_band = 0;

    /* Input: Cross gedrückt */
    memset(&input, 0, sizeof(input));
    input.pressed = RE15_BTN_CROSS;
    input.held = RE15_BTN_CROSS;

    /* Tür-Trigger ausführen */
    result = re15_door_trigger_check(&g_test_player, input);

    /* Prüfungen */
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

    /* Room muss auf 0x05 gesetzt sein */
    if (g_game.current_room != 0x05) {
        fprintf(stderr, "FAIL: current_room should be 0x05, got 0x%02X\n",
                g_game.current_room);
        return 1;
    }

    /* VAB-Unload muss aufgerufen worden sein (Stage-Wechsel → alte Bank entladen) */
    if (g_vab_unload_call_count < 1) {
        fprintf(stderr, "FAIL: VAB unload should have been called for stage change, "
                "got %d calls\n", g_vab_unload_call_count);
        return 1;
    }

    /* Spieler-Position prüfen */
    if (Q12_TO_INT(g_test_player.x) != -300) {
        fprintf(stderr, "FAIL: player.x should be -300, got %d\n",
                (int)Q12_TO_INT(g_test_player.x));
        return 1;
    }

    if (Q12_TO_INT(g_test_player.z) != 500) {
        fprintf(stderr, "FAIL: player.z should be 500, got %d\n",
                (int)Q12_TO_INT(g_test_player.z));
        return 1;
    }

    if (g_test_player.yaw != 1024) {
        fprintf(stderr, "FAIL: player.yaw should be 1024, got %d\n",
                (int)g_test_player.yaw);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 3: Same-Room-Teleport (kein Neuladen)
 *
 * Szenario: Tür führt zum selben Raum in gleicher Stage.
 * Erwartung:
 *   - KEIN Raum-Neuladen (I/O nicht aufgerufen für room_load)
 *   - Spieler wird an Spawn-Position teleportiert
 *   - Kamera wird auf dest_cut umgeschaltet
 *   - Überlappende Tür-AOTs am Spawn als "entered" markiert
 *
 * Validates: Requirements 4.7
 * ========================================================================= */

static int test_same_room_teleport(void)
{
    re15_input_state_t input;
    int result;
    const re15_aot_slot_t* slot_after;

    printf("  test_same_room_teleport... ");

    /* Setup: Stage 1, Room 0x10, Player 0 */
    test_setup(1, 0x10, 0);

    /* Tür-AOT in Slot 0: führt zum SELBEN Raum (Stage 1, Room 0x10) */
    /* Spawn bei (400, 0, -200), Rotation 3072, Cut 5 */
    setup_door_aot(0, 500, 1, 0x10, 400, 0, -200, 3072, 5);

    /*
     * Zweite Tür-AOT in Slot 1: Polygon enthält den Spawn-Punkt (400, -200).
     * Diese Tür soll nach dem Teleport als "entered" markiert sein (Req 4.8).
     */
    {
        re15_aot_slot_t spawn_door;
        memset(&spawn_door, 0, sizeof(spawn_door));
        spawn_door.active = 1;
        spawn_door.type = AOT_TYPE_DOOR;
        spawn_door.entered = 0;
        spawn_door.floor_band = 0;
        /* Polygon um den Spawn-Punkt (400, -200) */
        spawn_door.trigger_x[0] = 300;
        spawn_door.trigger_z[0] = -300;
        spawn_door.trigger_x[1] = 500;
        spawn_door.trigger_z[1] = -300;
        spawn_door.trigger_x[2] = 500;
        spawn_door.trigger_z[2] = -100;
        spawn_door.trigger_x[3] = 300;
        spawn_door.trigger_z[3] = -100;
        spawn_door.data.door.dest_stage = 1;
        spawn_door.data.door.dest_room = 0x12;
        spawn_door.data.door.spawn_x = 0;
        spawn_door.data.door.spawn_y = 0;
        spawn_door.data.door.spawn_z = 0;
        spawn_door.data.door.spawn_rot = 0;
        re15_aot_set_slot(1, &spawn_door);
    }

    /* Spieler am Ursprung (innerhalb der ersten Tür-Zone) */
    g_test_player.x = Q12_FROM_INT(0);
    g_test_player.z = Q12_FROM_INT(0);
    g_test_player.floor_band = 0;

    /* Reset I/O open counter — any calls after this are from the trigger */
    g_mock_io_open_count = 0;

    /* Input: Cross gedrückt */
    memset(&input, 0, sizeof(input));
    input.pressed = RE15_BTN_CROSS;
    input.held = RE15_BTN_CROSS;

    /* Tür-Trigger ausführen */
    result = re15_door_trigger_check(&g_test_player, input);

    /* Prüfungen */
    if (result != RE15_DOOR_OK) {
        fprintf(stderr, "FAIL: Expected RE15_DOOR_OK (1), got %d\n", result);
        return 1;
    }

    /* KEIN I/O-Aufruf: Same-Room-Teleport darf keinen Raum laden */
    if (g_mock_io_open_count != 0) {
        fprintf(stderr, "FAIL: Same-room teleport should NOT call IO open, "
                "but got %d calls\n", g_mock_io_open_count);
        return 1;
    }

    /* Spieler-Position muss an Spawn liegen */
    if (Q12_TO_INT(g_test_player.x) != 400) {
        fprintf(stderr, "FAIL: player.x should be 400, got %d\n",
                (int)Q12_TO_INT(g_test_player.x));
        return 1;
    }

    if (Q12_TO_INT(g_test_player.z) != -200) {
        fprintf(stderr, "FAIL: player.z should be -200, got %d\n",
                (int)Q12_TO_INT(g_test_player.z));
        return 1;
    }

    if (g_test_player.yaw != 3072) {
        fprintf(stderr, "FAIL: player.yaw should be 3072, got %d\n",
                (int)g_test_player.yaw);
        return 1;
    }

    /* Kamera muss auf dest_cut (5) umgeschaltet sein */
    if (g_game.current_cut != 5) {
        fprintf(stderr, "FAIL: current_cut should be 5, got %d\n",
                (int)g_game.current_cut);
        return 1;
    }

    /* Tür-AOT in Slot 1 (am Spawn-Punkt) muss als "entered" markiert sein */
    slot_after = re15_aot_get_slot(1);
    if (slot_after == NULL) {
        fprintf(stderr, "FAIL: AOT slot 1 should exist\n");
        return 1;
    }
    if (!slot_after->entered) {
        fprintf(stderr, "FAIL: AOT slot 1 at spawn should be marked 'entered'\n");
        return 1;
    }

    /* Stage und Room bleiben unverändert */
    if (g_game.current_stage != 1 || g_game.current_room != 0x10) {
        fprintf(stderr, "FAIL: Stage/Room should remain unchanged for teleport\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 4: Raumwechsel-Fehlerfall — Ziel-RDT nicht ladbar
 *
 * Szenario: Tür führt zu einem Raum, dessen RDT nicht geladen werden kann.
 * Erwartung:
 *   - re15_door_trigger_check gibt RE15_DOOR_LOAD_FAIL zurück
 *   - Spieler bleibt an aktueller Position (kein Teleport)
 *
 * Validates: Requirements 4.3 (Fehlerfall)
 * ========================================================================= */

static int test_room_change_load_failure(void)
{
    re15_input_state_t input;
    int result;
    int32_t orig_x, orig_z;

    printf("  test_room_change_load_failure... ");

    /* Setup: Stage 1, Room 0x17, Player 0 */
    test_setup(1, 0x17, 0);

    /* I/O-Backend auf Fail setzen NACH dem setup (setup setzt success) */
    g_io = &g_mock_io_fail;

    /* Tür-AOT: führt zu Stage 1, Room 0x20 (Datei nicht ladbar) */
    setup_door_aot(0, 500, 1, 0x20, 999, 0, 888, 512, 0);

    /* Spieler am Ursprung */
    g_test_player.x = Q12_FROM_INT(50);
    g_test_player.z = Q12_FROM_INT(75);
    orig_x = g_test_player.x;
    orig_z = g_test_player.z;
    g_test_player.floor_band = 0;

    /* Input: Cross gedrückt */
    memset(&input, 0, sizeof(input));
    input.pressed = RE15_BTN_CROSS;
    input.held = RE15_BTN_CROSS;

    /* Tür-Trigger ausführen */
    result = re15_door_trigger_check(&g_test_player, input);

    /* Muss LOAD_FAIL zurückgeben */
    if (result != RE15_DOOR_LOAD_FAIL) {
        fprintf(stderr, "FAIL: Expected RE15_DOOR_LOAD_FAIL (-1), got %d\n",
                result);
        return 1;
    }

    /* Spieler muss an Original-Position bleiben */
    if (g_test_player.x != orig_x) {
        fprintf(stderr, "FAIL: player.x should remain at original position\n");
        return 1;
    }
    if (g_test_player.z != orig_z) {
        fprintf(stderr, "FAIL: player.z should remain at original position\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 5: Kein Trigger ohne Aktionstaste
 *
 * Szenario: Spieler steht in Tür-Zone, drückt KEINE Aktionstaste.
 * Erwartung: Kein Raumwechsel ausgelöst.
 *
 * Validates: Requirements 4.2 (nur bei Aktionstaste)
 * ========================================================================= */

static int test_no_trigger_without_action_button(void)
{
    re15_input_state_t input;
    int result;

    printf("  test_no_trigger_without_action_button... ");

    /* Setup: Stage 1, Room 0x17, Player 0 */
    test_setup(1, 0x17, 0);

    /* Tür-AOT registrieren */
    setup_door_aot(0, 500, 1, 0x14, 100, 0, 200, 2048, 3);

    /* Spieler steht in der Zone */
    g_test_player.x = Q12_FROM_INT(0);
    g_test_player.z = Q12_FROM_INT(0);
    g_test_player.floor_band = 0;

    /* Input: KEINE Aktionstaste — nur D-Pad */
    memset(&input, 0, sizeof(input));
    input.held = RE15_BTN_UP;
    input.pressed = RE15_BTN_UP;

    /* Tür-Trigger prüfen */
    result = re15_door_trigger_check(&g_test_player, input);

    /* Kein Trigger ausgelöst */
    if (result != RE15_DOOR_NONE) {
        fprintf(stderr, "FAIL: Expected RE15_DOOR_NONE (0), got %d\n", result);
        return 1;
    }

    /* Room/Stage unverändert */
    if (g_game.current_room != 0x17) {
        fprintf(stderr, "FAIL: current_room should remain 0x17\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 6: Entered-Markierung verhindert Re-Trigger
 *
 * Szenario: Tür ist als "entered" markiert. Spieler drückt Aktionstaste.
 * Erwartung: Kein Raumwechsel (entered-Flag verhindert Auslösung).
 *
 * Validates: Requirements 4.7 (Re-Trigger-Schutz)
 * ========================================================================= */

static int test_entered_prevents_retrigger(void)
{
    re15_input_state_t input;
    int result;

    printf("  test_entered_prevents_retrigger... ");

    /* Setup: Stage 1, Room 0x17, Player 0 */
    test_setup(1, 0x17, 0);

    /* Tür-AOT registrieren und als "entered" markieren */
    setup_door_aot(0, 500, 1, 0x14, 100, 0, 200, 2048, 3);
    /* Manuell entered-Flag setzen (simuliert Spawn-in-Zone) */
    {
        re15_aot_slot_t entered_door;
        const re15_aot_slot_t* existing = re15_aot_get_slot(0);
        entered_door = *existing;
        entered_door.entered = 1;
        re15_aot_set_slot(0, &entered_door);
    }

    /* Spieler steht in der Zone */
    g_test_player.x = Q12_FROM_INT(0);
    g_test_player.z = Q12_FROM_INT(0);
    g_test_player.floor_band = 0;

    /* Input: Cross gedrückt */
    memset(&input, 0, sizeof(input));
    input.pressed = RE15_BTN_CROSS;
    input.held = RE15_BTN_CROSS;

    /* Tür-Trigger prüfen — entered-Flag muss den Trigger verhindern */
    result = re15_door_trigger_check(&g_test_player, input);

    if (result != RE15_DOOR_NONE) {
        fprintf(stderr, "FAIL: Expected RE15_DOOR_NONE (0) due to entered flag, "
                "got %d\n", result);
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
    failures += test_room_change_load_failure();
    failures += test_no_trigger_without_action_button();
    failures += test_entered_prevents_retrigger();

    printf("\n");
    if (failures == 0) {
        printf("ALL ROOM TRANSITION INTEGRATION TESTS PASSED (%d tests)\n", 6);
    } else {
        fprintf(stderr, "%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
