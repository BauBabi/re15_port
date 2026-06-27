/**
 * @file test_room_load.c
 * @brief Unit-Tests fuer das Raum-Lade-System (re15_room_load / re15_room_unload)
 *
 * Testet Error-Paths und Zustandsverwaltung ohne echte RDT-Dateien:
 * - Unload bei leerem Zustand (kein Crash)
 * - Unload setzt g_game-Zustand korrekt zurueck
 * - Load mit nicht-existenter Datei → RE15_IO_NOT_FOUND
 * - Load mit ungueltigen Parametern → RE15_IO_INVALID_ARG
 *
 * Requirements: 10.1, 12.2
 */

#include "re15_room.h"
#include "re15_engine.h"
#include "re15_io.h"
#include "re15_io_common.h"
#include "re15_rdt.h"
#include "re15_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * Mock I/O-Backend: open() gibt immer NOT_FOUND zurueck
 *
 * Wird verwendet um fehlende Dateien zu simulieren ohne echte Assets.
 * ========================================================================= */

static int mock_open_not_found(const char* path, void** handle)
{
    (void)path;
    (void)handle;
    return RE15_IO_NOT_FOUND;
}

static int mock_read_noop(void* handle, uint8_t* buf, int size, int* bytes_read)
{
    (void)handle;
    (void)buf;
    (void)size;
    if (bytes_read) *bytes_read = 0;
    return RE15_IO_READ_ERROR;
}

static int mock_get_size_noop(void* handle, int* size_out)
{
    (void)handle;
    if (size_out) *size_out = 0;
    return RE15_IO_READ_ERROR;
}

static void mock_close_noop(void* handle)
{
    (void)handle;
}

static const re15_io_backend_t g_mock_io_not_found = {
    mock_open_not_found,
    mock_read_noop,
    mock_get_size_noop,
    mock_close_noop
};

/* =========================================================================
 * Globale g_io Definition (extern in re15_io.h)
 *
 * Im Produktionscode setzt die Plattform-Initialisierung diesen Zeiger.
 * Fuer Tests definieren wir ihn hier und setzen ihn pro Test.
 * ========================================================================= */

const re15_io_backend_t* g_io = NULL;

/* =========================================================================
 * Hilfs-RDT-Struktur fuer Tests (current_rdt muss gesetzt sein)
 * ========================================================================= */

static re15_rdt_t g_test_rdt;

/* =========================================================================
 * Test 1: re15_room_unload() bei leerem Zustand — kein Crash
 *
 * Requirement: 10.1 (Raum entladen darf nicht crashen wenn kein Raum geladen)
 * ========================================================================= */

static int test_room_unload_safe(void)
{
    /* Zustand explizit auf "leer" setzen */
    memset(&g_game, 0, sizeof(g_game));
    memset(&g_test_rdt, 0, sizeof(g_test_rdt));
    g_game.current_rdt = &g_test_rdt;
    g_game.rdt_buffer = NULL;
    g_game.rdt_buffer_size = 0;

    /* Soll ohne Crash durchlaufen */
    re15_room_unload();

    /* Zustand muss weiterhin "leer" sein */
    if (g_game.rdt_buffer != NULL) {
        fprintf(stderr, "FAIL: rdt_buffer sollte NULL sein nach unload\n");
        return 1;
    }
    if (g_game.rdt_buffer_size != 0) {
        fprintf(stderr, "FAIL: rdt_buffer_size sollte 0 sein nach unload\n");
        return 1;
    }

    printf("PASS: test_room_unload_safe\n");
    return 0;
}

/* =========================================================================
 * Test 2: re15_room_unload() setzt g_game-Zustand korrekt zurueck
 *
 * Manuell gesetzter Zustand (simuliert geladenen Raum) wird vollstaendig
 * zurueckgesetzt. Speicherfreigabe wird geprueft.
 *
 * Requirement: 10.1, 12.2
 * ========================================================================= */

static int test_room_unload_clears_state(void)
{
    uint8_t* fake_buffer;

    /* Simuliere "Raum geladen" Zustand */
    memset(&g_game, 0, sizeof(g_game));
    memset(&g_test_rdt, 0xAA, sizeof(g_test_rdt));
    g_game.current_rdt = &g_test_rdt;

    /* Allokiere einen Puffer wie room_load es tun wuerde */
    fake_buffer = (uint8_t*)malloc(1024);
    if (fake_buffer == NULL) {
        fprintf(stderr, "FAIL: malloc fehlgeschlagen (test setup)\n");
        return 1;
    }
    memset(fake_buffer, 0xBB, 1024);

    g_game.rdt_buffer = fake_buffer;
    g_game.rdt_buffer_size = 1024;

    /* Unload ausfuehren */
    re15_room_unload();

    /* Puffer muss freigegeben und NULL gesetzt sein */
    if (g_game.rdt_buffer != NULL) {
        fprintf(stderr, "FAIL: rdt_buffer sollte NULL sein nach unload\n");
        return 1;
    }

    /* Groesse muss 0 sein */
    if (g_game.rdt_buffer_size != 0) {
        fprintf(stderr, "FAIL: rdt_buffer_size sollte 0 sein, ist %d\n",
                g_game.rdt_buffer_size);
        return 1;
    }

    /* RDT-Struktur muss genullt sein (alle Pointer auf NULL) */
    if (g_test_rdt.main_scd != NULL) {
        fprintf(stderr, "FAIL: current_rdt->main_scd sollte NULL sein nach unload\n");
        return 1;
    }
    if (g_test_rdt.collision != NULL) {
        fprintf(stderr, "FAIL: current_rdt->collision sollte NULL sein nach unload\n");
        return 1;
    }
    if (g_test_rdt.camera != NULL) {
        fprintf(stderr, "FAIL: current_rdt->camera sollte NULL sein nach unload\n");
        return 1;
    }

    printf("PASS: test_room_unload_clears_state\n");
    return 0;
}

/* =========================================================================
 * Test 3: re15_room_load() mit nicht-existenter Datei → RE15_IO_NOT_FOUND
 *
 * I/O-Backend wird auf Mock gesetzt der immer NOT_FOUND liefert.
 * Gueltige Parameter (Stage 1, Room 0x00, Player 0) damit die
 * Parameter-Validierung durchlaeuft.
 *
 * Requirement: 12.2
 * ========================================================================= */

static int test_room_load_missing_file(void)
{
    int result;

    /* Setup: sauberer Zustand, Mock-IO-Backend */
    memset(&g_game, 0, sizeof(g_game));
    memset(&g_test_rdt, 0, sizeof(g_test_rdt));
    g_game.current_rdt = &g_test_rdt;
    g_game.rdt_buffer = NULL;
    g_game.rdt_buffer_size = 0;

    g_io = &g_mock_io_not_found;

    /* Load mit gueltigen Parametern — Datei existiert nicht */
    result = re15_room_load(1, 0x00, 0);

    if (result != RE15_IO_NOT_FOUND) {
        fprintf(stderr, "FAIL: Expected RE15_IO_NOT_FOUND (%d), got %d\n",
                RE15_IO_NOT_FOUND, result);
        g_io = NULL;
        return 1;
    }

    /* Zustand muss sauber bleiben (kein halber Load) */
    if (g_game.rdt_buffer != NULL) {
        fprintf(stderr, "FAIL: rdt_buffer sollte NULL bleiben bei fehlender Datei\n");
        g_io = NULL;
        return 1;
    }

    g_io = NULL;
    printf("PASS: test_room_load_missing_file\n");
    return 0;
}

/* =========================================================================
 * Test 4: re15_room_load() mit ungueltigen Parametern → RE15_IO_INVALID_ARG
 *
 * Stage, Room und Player ausserhalb gueltigem Bereich muessen frueh
 * abgelehnt werden.
 *
 * Requirement: 12.2
 * ========================================================================= */

static int test_room_load_invalid_params(void)
{
    int result;

    /* Setup: sauberer Zustand, Mock-IO-Backend (sollte nicht erreicht werden) */
    memset(&g_game, 0, sizeof(g_game));
    memset(&g_test_rdt, 0, sizeof(g_test_rdt));
    g_game.current_rdt = &g_test_rdt;
    g_game.rdt_buffer = NULL;
    g_game.rdt_buffer_size = 0;

    g_io = &g_mock_io_not_found;

    /* Stage = 0 (unterhalb gueltigem Bereich 1-6) */
    result = re15_room_load(0, 0x00, 0);
    if (result != RE15_IO_INVALID_ARG) {
        fprintf(stderr, "FAIL: stage=0 should return RE15_IO_INVALID_ARG (%d), got %d\n",
                RE15_IO_INVALID_ARG, result);
        g_io = NULL;
        return 1;
    }

    /* Stage = 7 (oberhalb gueltigem Bereich) */
    result = re15_room_load(7, 0x00, 0);
    if (result != RE15_IO_INVALID_ARG) {
        fprintf(stderr, "FAIL: stage=7 should return RE15_IO_INVALID_ARG (%d), got %d\n",
                RE15_IO_INVALID_ARG, result);
        g_io = NULL;
        return 1;
    }

    /* Room = 0x28 (oberhalb gueltigem Bereich 0x00-0x27) */
    result = re15_room_load(1, 0x28, 0);
    if (result != RE15_IO_INVALID_ARG) {
        fprintf(stderr, "FAIL: room=0x28 should return RE15_IO_INVALID_ARG (%d), got %d\n",
                RE15_IO_INVALID_ARG, result);
        g_io = NULL;
        return 1;
    }

    /* Player = 2 (nur 0 und 1 gueltig) */
    result = re15_room_load(1, 0x00, 2);
    if (result != RE15_IO_INVALID_ARG) {
        fprintf(stderr, "FAIL: player=2 should return RE15_IO_INVALID_ARG (%d), got %d\n",
                RE15_IO_INVALID_ARG, result);
        g_io = NULL;
        return 1;
    }

    g_io = NULL;
    printf("PASS: test_room_load_invalid_params\n");
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(void)
{
    int failures = 0;

    printf("=== Room Load Unit Tests ===\n\n");

    failures += test_room_unload_safe();
    failures += test_room_unload_clears_state();
    failures += test_room_load_missing_file();
    failures += test_room_load_invalid_params();

    if (failures == 0) {
        printf("\nALL ROOM LOAD TESTS PASSED\n");
    } else {
        fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
