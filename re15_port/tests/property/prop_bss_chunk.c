/**
 * @file prop_bss_chunk.c
 * @brief Property-Test: BSS-Chunk-Selektion nach Kamera-ID
 *
 * **Validates: Requirements 10.5**
 *
 * Property 16: Für jede gültige Kamera-ID c im Bereich [0, nCut-1] und
 * jede BSS-Datei mit mindestens nCut Chunks à 64 KB soll der geladene
 * Chunk exakt den Bytes an Offset c * 65536 bis (c+1) * 65536 - 1 der
 * BSS-Datei entsprechen.
 *
 * Test-Strategie (kein echtes File-I/O):
 *   1. Synthetischen BSS-Puffer mit N Chunks erzeugen (je 64 KB, eindeutiges
 *      Byte-Muster pro Chunk)
 *   2. Fake-I/O-Backend injizieren, das den Puffer als "Datei" liefert
 *   3. re15_bss_load_chunk für jede camera_id [0, N-1] aufrufen
 *   4. Prüfen: Ergebnis-Chunk == erwartetes Byte-Muster
 *   5. Prüfen: camera_id >= N → Fehler (out of bounds)
 *   6. Zusätzlich: re15_bss_build_path-Format für BSS-Dateien validieren
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_bss.h"
#include "re15_io.h"
#include "re15_io_common.h"
#include "re15_error.h"
#include "re15_engine.h"

/* =========================================================================
 * Globale Definition für g_io (im Test-Kontext benötigt)
 * ========================================================================= */

const re15_io_backend_t* g_io = NULL;

/* =========================================================================
 * Test-Infrastruktur
 * ========================================================================= */

static int g_passed = 0;
static int g_failed = 0;

#define PROP_ASSERT(cond, fmt, ...)                                          \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("  FAIL: " fmt "\n", ##__VA_ARGS__);                       \
            printf("        at %s:%d\n", __FILE__, __LINE__);                 \
            g_failed++;                                                        \
            return 1;                                                          \
        }                                                                      \
    } while (0)

/* =========================================================================
 * Fake-I/O-Backend — liefert synthetischen BSS-Puffer als "Datei"
 * ========================================================================= */

/** Maximale Chunk-Anzahl für den synthetischen BSS-Puffer */
#define MAX_TEST_CHUNKS 8

/** Synthetischer BSS-Puffer: MAX_TEST_CHUNKS * 64 KB */
static uint8_t* g_fake_bss_data = NULL;
static int      g_fake_bss_size = 0;

/** Fake-Handle: Nur ein Marker-Zeiger (nicht-NULL) */
static int g_fake_handle_marker = 42;

static int fake_io_open(const char* path, void** handle)
{
    (void)path;
    if (g_fake_bss_data == NULL) {
        return RE15_IO_NOT_FOUND;
    }
    *handle = &g_fake_handle_marker;
    return RE15_IO_OK;
}

static int fake_io_read(void* handle, uint8_t* buf, int size, int* bytes_read)
{
    (void)handle;
    if (size > g_fake_bss_size) {
        size = g_fake_bss_size;
    }
    memcpy(buf, g_fake_bss_data, (size_t)size);
    *bytes_read = size;
    return RE15_IO_OK;
}

static int fake_io_get_size(void* handle, int* size_out)
{
    (void)handle;
    *size_out = g_fake_bss_size;
    return RE15_IO_OK;
}

static void fake_io_close(void* handle)
{
    (void)handle;
}

static const re15_io_backend_t g_fake_io_backend = {
    .open     = fake_io_open,
    .read     = fake_io_read,
    .get_size = fake_io_get_size,
    .close    = fake_io_close,
};

/* =========================================================================
 * Hilfsfunktionen
 * ========================================================================= */

/**
 * Erzeugt einen synthetischen BSS-Puffer mit num_chunks Chunks.
 * Jeder Chunk wird vollständig mit einem eindeutigen Byte-Muster gefüllt:
 *   Chunk 0 = 0xAA, Chunk 1 = 0xBB, Chunk 2 = 0xCC, usw.
 */
static uint8_t chunk_pattern(int chunk_index)
{
    /* Muster: 0xAA + chunk_index, wraparound bei Überlauf */
    return (uint8_t)(0xAA + chunk_index);
}

static int setup_fake_bss(int num_chunks)
{
    int i;

    g_fake_bss_size = num_chunks * RE15_BSS_CHUNK_SIZE;
    g_fake_bss_data = (uint8_t*)malloc((size_t)g_fake_bss_size);
    if (!g_fake_bss_data) {
        return -1;
    }

    for (i = 0; i < num_chunks; i++) {
        memset(g_fake_bss_data + i * RE15_BSS_CHUNK_SIZE,
               chunk_pattern(i),
               RE15_BSS_CHUNK_SIZE);
    }

    return 0;
}

static void teardown_fake_bss(void)
{
    free(g_fake_bss_data);
    g_fake_bss_data = NULL;
    g_fake_bss_size = 0;
}

/* =========================================================================
 * Property-Test 1: BSS-Pfad-Konstruktion (re15_bss_build_path)
 * ========================================================================= */

/**
 * Prüft dass re15_bss_build_path das korrekte Format liefert:
 * STAGE{N}/ROOM{Stage}{RoomHex}.BSS
 */
static int test_bss_build_path_valid(void)
{
    char buf[RE15_MAX_PATH];
    int result;
    int stage, room_hex;

    printf("[1/4] Teste re15_bss_build_path fuer gueltige Eingaben...\n");

    for (stage = 1; stage <= 6; stage++) {
        for (room_hex = 0x00; room_hex <= 0x27; room_hex++) {
            char expected[64];
            memset(buf, 0xCC, sizeof(buf));

            result = re15_bss_build_path(stage, room_hex, buf, sizeof(buf));

            PROP_ASSERT(result == 0,
                "re15_bss_build_path(%d, 0x%02X) returned %d (expected 0)",
                stage, room_hex, result);

            /* Erwartetes Format: STAGE{N}/ROOM{Stage}{RoomHex}.BSS */
            snprintf(expected, sizeof(expected),
                     "STAGE%d/ROOM%d%02X.BSS", stage, stage, room_hex);

            PROP_ASSERT(strcmp(buf, expected) == 0,
                "stage=%d room=0x%02X: got '%s' (expected '%s')",
                stage, room_hex, buf, expected);

            g_passed++;
        }
    }

    printf("    240 Kombinationen (6 Stages x 40 Rooms) bestanden.\n\n");
    return 0;
}

/**
 * Prüft dass ungültige Parameter von re15_bss_build_path abgelehnt werden.
 */
static int test_bss_build_path_invalid(void)
{
    char buf[RE15_MAX_PATH];
    int result;

    printf("[2/4] Teste re15_bss_build_path fuer ungueltige Eingaben...\n");

    /* Stage außerhalb des gültigen Bereichs */
    result = re15_bss_build_path(0, 0x10, buf, sizeof(buf));
    PROP_ASSERT(result == -1, "stage=0 should return -1, got %d", result);

    result = re15_bss_build_path(7, 0x10, buf, sizeof(buf));
    PROP_ASSERT(result == -1, "stage=7 should return -1, got %d", result);

    /* room_hex außerhalb des gültigen Bereichs */
    result = re15_bss_build_path(1, 0x28, buf, sizeof(buf));
    PROP_ASSERT(result == -1, "room_hex=0x28 should return -1, got %d", result);

    result = re15_bss_build_path(1, -1, buf, sizeof(buf));
    PROP_ASSERT(result == -1, "room_hex=-1 should return -1, got %d", result);

    /* NULL-Pointer */
    result = re15_bss_build_path(1, 0x10, NULL, sizeof(buf));
    PROP_ASSERT(result == -1, "out_path=NULL should return -1, got %d", result);

    /* Puffer zu klein */
    result = re15_bss_build_path(1, 0x10, buf, 5);
    PROP_ASSERT(result == -1, "max_len=5 should return -1, got %d", result);

    result = re15_bss_build_path(1, 0x10, buf, 0);
    PROP_ASSERT(result == -1, "max_len=0 should return -1, got %d", result);

    g_passed += 7;
    printf("    7 Ungueltigkeits-Tests bestanden.\n\n");
    return 0;
}

/* =========================================================================
 * Property-Test 2: Chunk-Selektion — korrekter Offset pro Kamera-ID
 * ========================================================================= */

/**
 * Erstellt einen synthetischen BSS-Puffer mit N Chunks, ruft
 * re15_bss_load_chunk für jede camera_id [0, N-1] auf und prüft,
 * dass der geladene Chunk exakt dem erwarteten Byte-Muster entspricht.
 *
 * Dies verifiziert die Kern-Property:
 *   Geladener Chunk == Bytes an Offset c * 65536 bis (c+1)*65536-1
 */
static int test_chunk_selection_valid(void)
{
    int num_chunks = 5;
    int cam;
    uint8_t out_chunk[RE15_BSS_CHUNK_SIZE];
    int result;
    int j;

    printf("[3/4] Teste Chunk-Selektion fuer %d Chunks...\n", num_chunks);

    /* BSS-Puffer aufbauen */
    if (setup_fake_bss(num_chunks) != 0) {
        printf("  FAIL: Speicherallokation fuer synthetischen BSS-Puffer\n");
        g_failed++;
        return 1;
    }

    /* Fake-I/O-Backend injizieren */
    g_io = &g_fake_io_backend;

    /* Für jede gültige camera_id testen */
    for (cam = 0; cam < num_chunks; cam++) {
        uint8_t expected_byte = chunk_pattern(cam);

        memset(out_chunk, 0x00, RE15_BSS_CHUNK_SIZE);

        result = re15_bss_load_chunk(1, 0x00, (uint8_t)cam, out_chunk);

        PROP_ASSERT(result == RE15_IO_OK,
            "re15_bss_load_chunk(1, 0x00, %d) returned %d (expected 0)",
            cam, result);

        /* Prüfe: Jedes Byte im Chunk muss dem erwarteten Muster entsprechen.
         * Stichproben-Prüfung an Anfang, Mitte und Ende des Chunks. */
        PROP_ASSERT(out_chunk[0] == expected_byte,
            "cam=%d: out_chunk[0]=0x%02X (expected 0x%02X)",
            cam, out_chunk[0], expected_byte);

        PROP_ASSERT(out_chunk[RE15_BSS_CHUNK_SIZE / 2] == expected_byte,
            "cam=%d: out_chunk[%d]=0x%02X (expected 0x%02X)",
            cam, RE15_BSS_CHUNK_SIZE / 2,
            out_chunk[RE15_BSS_CHUNK_SIZE / 2], expected_byte);

        PROP_ASSERT(out_chunk[RE15_BSS_CHUNK_SIZE - 1] == expected_byte,
            "cam=%d: out_chunk[%d]=0x%02X (expected 0x%02X)",
            cam, RE15_BSS_CHUNK_SIZE - 1,
            out_chunk[RE15_BSS_CHUNK_SIZE - 1], expected_byte);

        /* Vollständige Prüfung: alle 65536 Bytes müssen übereinstimmen */
        for (j = 0; j < RE15_BSS_CHUNK_SIZE; j++) {
            PROP_ASSERT(out_chunk[j] == expected_byte,
                "cam=%d: out_chunk[%d]=0x%02X (expected 0x%02X)",
                cam, j, out_chunk[j], expected_byte);
        }

        g_passed++;
    }

    printf("    %d Chunks korrekt selektiert (Offset-Arithmetik validiert).\n\n",
           num_chunks);

    teardown_fake_bss();
    g_io = NULL;
    return 0;
}

/* =========================================================================
 * Property-Test 3: Chunk-Selektion — camera_id out-of-bounds
 * ========================================================================= */

/**
 * Prüft: Wenn camera_id >= Anzahl verfügbarer Chunks, soll
 * re15_bss_load_chunk einen Fehlercode zurückgeben (nicht 0).
 */
static int test_chunk_selection_out_of_bounds(void)
{
    int num_chunks = 3;
    uint8_t out_chunk[RE15_BSS_CHUNK_SIZE];
    int result;
    int cam;

    printf("[4/4] Teste Chunk-Selektion Out-of-Bounds...\n");

    if (setup_fake_bss(num_chunks) != 0) {
        printf("  FAIL: Speicherallokation fuer synthetischen BSS-Puffer\n");
        g_failed++;
        return 1;
    }

    g_io = &g_fake_io_backend;

    /* camera_id == num_chunks (genau 1 über dem Maximum) */
    memset(out_chunk, 0x00, RE15_BSS_CHUNK_SIZE);
    result = re15_bss_load_chunk(1, 0x00, (uint8_t)num_chunks, out_chunk);
    PROP_ASSERT(result != RE15_IO_OK,
        "cam=%d (out of bounds): expected error, got %d",
        num_chunks, result);
    g_passed++;

    /* camera_id = num_chunks + 1 (deutlich über dem Maximum) */
    result = re15_bss_load_chunk(1, 0x00, (uint8_t)(num_chunks + 1), out_chunk);
    PROP_ASSERT(result != RE15_IO_OK,
        "cam=%d (out of bounds): expected error, got %d",
        num_chunks + 1, result);
    g_passed++;

    /* camera_id = 255 (Maximum für uint8_t) bei nur 3 Chunks */
    result = re15_bss_load_chunk(1, 0x00, 255, out_chunk);
    PROP_ASSERT(result != RE15_IO_OK,
        "cam=255 (out of bounds): expected error, got %d", result);
    g_passed++;

    /* Zusätzlich: Verschiedene Chunk-Anzahlen testen */
    teardown_fake_bss();

    for (cam = 1; cam <= MAX_TEST_CHUNKS; cam++) {
        if (setup_fake_bss(cam) != 0) {
            g_failed++;
            return 1;
        }

        /* Gültiger letzter Chunk */
        memset(out_chunk, 0x00, RE15_BSS_CHUNK_SIZE);
        result = re15_bss_load_chunk(1, 0x00, (uint8_t)(cam - 1), out_chunk);
        PROP_ASSERT(result == RE15_IO_OK,
            "num_chunks=%d, cam=%d (last valid): expected OK, got %d",
            cam, cam - 1, result);

        /* Erster ungültiger Chunk */
        result = re15_bss_load_chunk(1, 0x00, (uint8_t)cam, out_chunk);
        PROP_ASSERT(result != RE15_IO_OK,
            "num_chunks=%d, cam=%d (first invalid): expected error, got %d",
            cam, cam, result);

        g_passed += 2;
        teardown_fake_bss();
    }

    printf("    Out-of-Bounds-Erkennung fuer alle Chunk-Groessen bestanden.\n\n");

    g_io = NULL;
    return 0;
}

/* =========================================================================
 * Main
 * ========================================================================= */

int main(void)
{
    printf("=== Property-Test: BSS-Chunk-Selektion nach Kamera-ID ===\n");
    printf("    Validates: Requirements 10.5\n\n");

    /* Setze RE15_ASSETS_PATH damit re15_assets_get_root() nicht NULL liefert.
     * Der konkrete Pfad ist irrelevant — das Fake-I/O-Backend ignoriert ihn. */
#ifdef _WIN32
    _putenv("RE15_ASSETS_PATH=C:\\tmp");
#else
    setenv("RE15_ASSETS_PATH", "/tmp", 1);
#endif

    /* Test 1: BSS-Pfad-Konstruktion */
    if (test_bss_build_path_valid() != 0) {
        printf("\n=== ABBRUCH bei BSS-Pfad-Konstruktion ===\n");
        goto done;
    }

    /* Test 2: BSS-Pfad ungültige Eingaben */
    if (test_bss_build_path_invalid() != 0) {
        printf("\n=== ABBRUCH bei BSS-Pfad Ungueltigkeits-Tests ===\n");
        goto done;
    }

    /* Test 3: Chunk-Selektion gültig */
    if (test_chunk_selection_valid() != 0) {
        printf("\n=== ABBRUCH bei Chunk-Selektion ===\n");
        goto done;
    }

    /* Test 4: Chunk-Selektion out-of-bounds */
    if (test_chunk_selection_out_of_bounds() != 0) {
        printf("\n=== ABBRUCH bei Out-of-Bounds-Test ===\n");
        goto done;
    }

done:
    /* Zusammenfassung */
    printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
