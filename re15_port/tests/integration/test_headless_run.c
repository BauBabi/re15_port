/**
 * @file test_headless_run.c
 * @brief Integration-Test: Headless-Modus — 5+ Räume sequentiell laden
 *
 * Simuliert programmatisch was `--headless --stage 1 --room 0x17 --ticks 60`
 * tut: Räume laden, SCD-Ticks ausführen, entladen, und zum nächsten Raum
 * weiterschalten. Der Test verarbeitet 5+ verschiedene Räume sequentiell
 * und prüft dass keine Abstürze oder Fehler auftreten.
 *
 * Ablauf pro Raum:
 *   1. re15_room_load(stage, room_id, player_id)
 *   2. N SCD-Ticks ausführen (scd_vm_tick)
 *   3. re15_room_unload()
 *   4. Prüfe: Kein Fehler, Zustand konsistent
 *
 * Verwendet Mock-I/O-Backend mit synthetischen RDT-Daten.
 *
 * Requirements: 4.4, 12.2, 12.5
 */

#include "re15_room.h"
#include "re15_engine.h"
#include "re15_io.h"
#include "re15_io_common.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_error.h"
#include "re15_aot.h"
#include "re15_player.h"
#include "re15_audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * Synthetic RDT data for testing
 * ========================================================================= */

#define MOCK_RDT_SIZE 96

static uint8_t g_mock_rdt_data[MOCK_RDT_SIZE];

/** Track I/O operations for verification */
static int  g_mock_io_open_count = 0;
static int  g_mock_io_read_count = 0;
static char g_last_opened_path[RE15_MAX_PATH];

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
    g_mock_io_read_count++;
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
 * Mock Audio Backend (no-op, headless mode does not need audio)
 * ========================================================================= */

static void mock_audio_init(void) {}
static void mock_audio_shutdown(void) {}

static int mock_audio_vab_load(const uint8_t* vh, int vh_size,
                               const uint8_t* vb, int vb_size, int bank_id)
{
    (void)vh; (void)vh_size; (void)vb; (void)vb_size; (void)bank_id;
    return 0;
}

static void mock_audio_vab_unload(int bank_id) { (void)bank_id; }

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
 * Initialisiert den globalen Test-Zustand für den Headless-Test.
 */
static void headless_setup(void)
{
    /* Synthetische RDT: alle Offsets = 0 (Sektionen nicht vorhanden) */
    memset(g_mock_rdt_data, 0, sizeof(g_mock_rdt_data));

    /* Engine-Zustand zurücksetzen */
    memset(&g_game, 0, sizeof(g_game));
    memset(&g_test_rdt, 0, sizeof(g_test_rdt));
    memset(&g_test_player, 0, sizeof(g_test_player));

    g_game.current_rdt = &g_test_rdt;
    g_game.player = &g_test_player;
    g_game.current_stage = 1;
    g_game.current_room = 0x00;
    g_game.current_player = 0;

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
    g_mock_io_open_count = 0;
    g_mock_io_read_count = 0;
    memset(g_last_opened_path, 0, sizeof(g_last_opened_path));
}

/* =========================================================================
 * Test 1: 5 Räume sequentiell laden (gleiche Stage)
 *
 * Simuliert den Headless-Ablauf: Raum laden, Ticks ausführen, entladen.
 * Wiederholt für 5 verschiedene Raum-IDs in Stage 1.
 *
 * Validates: Requirements 12.2, 12.5
 * ========================================================================= */

static int test_5_rooms_sequential_same_stage(void)
{
    static const uint8_t rooms[] = { 0x00, 0x05, 0x0A, 0x14, 0x17 };
    static const int num_rooms = 5;
    static const int ticks_per_room = 60;
    int i, t, result;

    printf("  test_5_rooms_sequential_same_stage... ");

    headless_setup();

    for (i = 0; i < num_rooms; i++) {
        /* Raum laden */
        result = re15_room_load(1, rooms[i], 0);
        if (result != 0) {
            fprintf(stderr, "FAIL: room_load(1, 0x%02X, 0) returned %d\n",
                    rooms[i], result);
            return 1;
        }

        /* SCD-Ticks ausführen (simuliert Headless-Ticks) */
        for (t = 0; t < ticks_per_room; t++) {
            scd_vm_tick();
        }

        /* Raum entladen */
        re15_room_unload();

        /* Prüfe Zustand nach Entladen: Buffer muss freigegeben sein */
        if (g_game.rdt_buffer != NULL) {
            fprintf(stderr, "FAIL: rdt_buffer should be NULL after unload "
                    "(room 0x%02X)\n", rooms[i]);
            return 1;
        }

        if (g_game.rdt_buffer_size != 0) {
            fprintf(stderr, "FAIL: rdt_buffer_size should be 0 after unload "
                    "(room 0x%02X)\n", rooms[i]);
            return 1;
        }
    }

    /* Alle 5 Räume müssen via I/O geladen worden sein */
    if (g_mock_io_open_count < num_rooms) {
        fprintf(stderr, "FAIL: Expected at least %d IO opens, got %d\n",
                num_rooms, g_mock_io_open_count);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 2: 6 Räume sequentiell — verschiedene Stages
 *
 * Simuliert Headless-Modus über Stage-Grenzen hinweg:
 *   Stage 1, Room 0x17  →  Stage 2, Room 0x03  →  Stage 3, Room 0x00
 *   → Stage 4, Room 0x01  →  Stage 5, Room 0x02  →  Stage 6, Room 0x00
 *
 * Validates: Requirements 4.4, 12.2
 * ========================================================================= */

typedef struct {
    uint8_t stage;
    uint8_t room;
} room_spec_t;

static int test_6_rooms_cross_stage(void)
{
    static const room_spec_t rooms[] = {
        { 1, 0x17 },
        { 2, 0x03 },
        { 3, 0x00 },
        { 4, 0x01 },
        { 5, 0x02 },
        { 6, 0x00 },
    };
    static const int num_rooms = 6;
    static const int ticks_per_room = 30;
    int i, t, result;

    printf("  test_6_rooms_cross_stage... ");

    headless_setup();

    for (i = 0; i < num_rooms; i++) {
        /* Stage-Wechsel wenn nötig */
        if (rooms[i].stage != g_game.current_stage) {
            result = re15_stage_change(rooms[i].stage);
            if (result != 0) {
                fprintf(stderr, "FAIL: stage_change(%d) returned %d\n",
                        rooms[i].stage, result);
                return 1;
            }
        }

        /* Raum laden */
        result = re15_room_load(rooms[i].stage, rooms[i].room, 0);
        if (result != 0) {
            fprintf(stderr, "FAIL: room_load(%d, 0x%02X, 0) returned %d\n",
                    rooms[i].stage, rooms[i].room, result);
            return 1;
        }

        /* SCD-Ticks */
        for (t = 0; t < ticks_per_room; t++) {
            scd_vm_tick();
        }

        /* Raum entladen */
        re15_room_unload();
    }

    /* Letzte Stage muss 6 sein */
    if (g_game.current_stage != 6) {
        fprintf(stderr, "FAIL: current_stage should be 6 after traversal, "
                "got %d\n", g_game.current_stage);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 3: Headless — Double-Load ohne Crash
 *
 * Ruft room_load zweimal nacheinander auf ohne explizites unload dazwischen.
 * Der Room-Manager muss intern unloaden bevor er den neuen Raum lädt.
 * Prüft dass kein Absturz, Speicherleck oder undefiniertes Verhalten auftritt.
 *
 * Validates: Requirements 12.2
 * ========================================================================= */

static int test_double_load_no_crash(void)
{
    int result;
    int t;

    printf("  test_double_load_no_crash... ");

    headless_setup();

    /* Erster Raum laden */
    result = re15_room_load(1, 0x00, 0);
    if (result != 0) {
        fprintf(stderr, "FAIL: First room_load returned %d\n", result);
        return 1;
    }

    /* Ein paar Ticks */
    for (t = 0; t < 10; t++) {
        scd_vm_tick();
    }

    /* Zweiter Raum laden OHNE explizites unload — room_load muss intern entladen */
    result = re15_room_load(1, 0x05, 0);
    if (result != 0) {
        fprintf(stderr, "FAIL: Second room_load returned %d\n", result);
        return 1;
    }

    /* Ticks auf neuem Raum */
    for (t = 0; t < 10; t++) {
        scd_vm_tick();
    }

    /* Aufräumen */
    re15_room_unload();

    if (g_game.rdt_buffer != NULL) {
        fprintf(stderr, "FAIL: rdt_buffer should be NULL after final unload\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 4: Headless — Unload auf leerem Zustand (no-op)
 *
 * re15_room_unload() darf nicht abstürzen wenn kein Raum geladen ist.
 *
 * Validates: Requirements 12.2
 * ========================================================================= */

static int test_unload_empty_no_crash(void)
{
    printf("  test_unload_empty_no_crash... ");

    headless_setup();

    /* Unload ohne vorheriges Load — muss no-op sein */
    re15_room_unload();
    re15_room_unload(); /* Doppelt — trotzdem kein Crash */

    /* Buffer sollte NULL bleiben */
    if (g_game.rdt_buffer != NULL) {
        fprintf(stderr, "FAIL: rdt_buffer should remain NULL\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 5: Headless — 8 Räume mit mehr Ticks (Stress-Test)
 *
 * Lädt 8 verschiedene Räume mit je 120 SCD-Ticks. Validiert dass auch
 * bei längerer Ausführung kein Fehler auftritt.
 *
 * Validates: Requirements 12.2, 12.5
 * ========================================================================= */

static int test_8_rooms_stress(void)
{
    static const uint8_t rooms[] = {
        0x00, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x20
    };
    static const int num_rooms = 8;
    static const int ticks_per_room = 120;
    int i, t, result;

    printf("  test_8_rooms_stress... ");

    headless_setup();

    for (i = 0; i < num_rooms; i++) {
        result = re15_room_load(1, rooms[i], 0);
        if (result != 0) {
            fprintf(stderr, "FAIL: room_load(1, 0x%02X, 0) returned %d "
                    "at iteration %d\n", rooms[i], result, i);
            return 1;
        }

        for (t = 0; t < ticks_per_room; t++) {
            scd_vm_tick();
        }

        re15_room_unload();
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

    printf("=== Integration Tests: Headless Run ===\n\n");

    failures += test_5_rooms_sequential_same_stage();
    failures += test_6_rooms_cross_stage();
    failures += test_double_load_no_crash();
    failures += test_unload_empty_no_crash();
    failures += test_8_rooms_stress();

    printf("\n");
    if (failures == 0) {
        printf("ALL HEADLESS RUN INTEGRATION TESTS PASSED (%d tests)\n", 5);
    } else {
        fprintf(stderr, "%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
