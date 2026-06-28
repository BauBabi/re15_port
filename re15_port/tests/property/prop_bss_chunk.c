/**
 * @file prop_bss_chunk.c
 * @brief Property-Test: BSS-Chunk-Selektion nach Kamera-ID
 *
 * **Validates: Requirements 10.5**
 *
 * Property 16: Für jede gültige Kamera-ID c im Bereich [0, nCut-1] und
 * jede BSS-Datei mit mindestens nCut Chunks à 64 KB soll der ausgewählte
 * Chunk exakt den Bytes an Offset c * 65536 bis (c+1) * 65536 - 1 der
 * BSS-Datei entsprechen.
 *
 * ------------------------------------------------------------------------
 * REAKTIVIERUNG (2026-06-28): API-Transplant.
 *
 * Die ursprüngliche BSS-Schicht (re15_bss_load_chunk / re15_bss_build_path
 * über ein injizierbares I/O-Backend g_io / re15_io_backend_t) wurde im
 * Engine-Transplant ENTFERNT und durch zwei getrennte Schichten ersetzt:
 *
 *   1. CD-Streaming-Loader   re15_cd_load_file()      (re15_cdfs.h)
 *      — lädt eine ganze BSS-Datei per ISO9660-Pfad in einen Puffer.
 *   2. Parse/Decode-Modell   re15_bss_parse_chunk()   (re15_bss.h, AKTUELL)
 *      — parst den 8-Byte-Kopf eines 64-KB-Chunks aus einem Puffer.
 *      + re15_bss_vlc_decode() / re15_bss_mdec_decode().
 *
 * Es gibt KEINE öffentliche Pfad-Bau-Funktion und KEIN injizierbares
 * I/O-Backend mehr (re15_io.h / re15_io_common.h / re15_error.h existieren
 * im aktuellen include/ nicht). Die Chunk-SELEKTIONS-Arithmetik, die früher
 * IN re15_bss_load_chunk lag (dokumentierter Schritt 3:
 *   "Validierung: camera_id * 65536 + 65536 <= file_size"
 *   "64KB-Chunk an Offset camera_id * 65536 nach out_chunk kopieren"
 *   — siehe altes re15_bss.h / _legacy_minimal),
 * ist damit AUFRUFER-Verantwortung geworden.
 *
 * Dieser Test erhält die Intention von Property 16, indem er:
 *   - die Selektions-Arithmetik lokal nachbildet (select_bss_chunk(), unten),
 *     mit zitierter Quelle (RE15_BSS_CHUNK_SIZE aus aktuellem re15_bss.h),
 *   - jeden selektierten Chunk zusätzlich durch die ECHTE Engine-Funktion
 *     re15_bss_parse_chunk() (bss_common.c) führt, sodass der Test an die
 *     LEBENDE öffentliche API gebunden bleibt,
 *   - die früheren Pfad-Bau-Tests (öffentliche Funktion ersatzlos entfernt)
 *     durch einen Kontrakt-Test der aktuellen re15_bss_parse_chunk-Eingabe-
 *     validierung (NULL / Trunkierung < RE15_BSS_HEADER_SIZE) ersetzt.
 * ------------------------------------------------------------------------
 *
 * Test-Strategie (kein echtes File-I/O):
 *   1. Synthetischen BSS-Puffer mit N Chunks erzeugen (je 64 KB, eindeutiges
 *      Byte-Muster pro Chunk, mit erkennbarem 8-Byte-Kopf je Chunk)
 *   2. select_bss_chunk() (lokale Nachbildung der entfernten Selektion) für
 *      jede camera_id [0, N-1] aufrufen
 *   3. Prüfen: Ergebnis-Chunk == erwartetes Byte-Muster (Offset-Arithmetik)
 *   4. Prüfen: re15_bss_parse_chunk() liefert auf dem Chunk die erwarteten
 *      Kopf-Felder (Bindung an die echte Engine-API)
 *   5. Prüfen: camera_id >= N → Selektion schlägt fehl (out of bounds)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_bss.h"   /* AKTUELL: re15_bss_parse_chunk, RE15_BSS_CHUNK_SIZE,
                         *           RE15_BSS_HEADER_SIZE, RE15_BSS_VLC_ID,
                         *           re15_bss_chunk_t */

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
 * Lokale Nachbildung der ENTFERNTEN Chunk-Selektion
 *
 * Bildet die Kern-Logik des früheren re15_bss_load_chunk nach (dokumentierte
 * Schritte 3 + 5 im alten re15_bss.h):
 *   - Offset des Chunks für camera_id  = camera_id * RE15_BSS_CHUNK_SIZE
 *   - Gültigkeit                       = offset + RE15_BSS_CHUNK_SIZE <= size
 *   - Kopie der 64 KB ab diesem Offset nach out_chunk
 * RE15_BSS_CHUNK_SIZE stammt aus dem AKTUELLEN re15_bss.h (= 0x10000).
 *
 * Rückgabe: 0 bei Erfolg, -1 bei out-of-bounds / ungültigen Argumenten.
 * (0 / -1 = die früheren RE15_IO_OK / RE15_IO_NOT_FOUND Semantik-Werte.)
 * ========================================================================= */

#define SELECT_OK         0
#define SELECT_OOB       (-1)

static int select_bss_chunk(const uint8_t* file_data, int file_size,
                            uint8_t camera_id, uint8_t* out_chunk)
{
    /* Offset-Arithmetik wie im entfernten re15_bss_load_chunk. In int gerechnet
     * würde camera_id * 65536 ab camera_id >= 32768 überlaufen — hier mit
     * camera_id (uint8_t, max 255) unkritisch (255*65536 = ~16 MB < INT_MAX). */
    long offset = (long)camera_id * RE15_BSS_CHUNK_SIZE;

    if (file_data == NULL || out_chunk == NULL) {
        return SELECT_OOB;
    }
    /* Validierung: camera_id * 65536 + 65536 <= file_size */
    if (offset + RE15_BSS_CHUNK_SIZE > (long)file_size) {
        return SELECT_OOB;
    }

    memcpy(out_chunk, file_data + offset, RE15_BSS_CHUNK_SIZE);
    return SELECT_OK;
}

/* =========================================================================
 * Synthetischer BSS-Puffer
 * ========================================================================= */

/** Maximale Chunk-Anzahl für den synthetischen BSS-Puffer */
#define MAX_TEST_CHUNKS 8

/** Synthetischer BSS-Puffer: bis zu MAX_TEST_CHUNKS * 64 KB */
static uint8_t* g_fake_bss_data = NULL;
static int      g_fake_bss_size = 0;

/**
 * Eindeutiges Füll-Byte je Chunk:
 *   Chunk 0 = 0xAA, Chunk 1 = 0xBB, Chunk 2 = 0xCC, usw. (wraparound).
 */
static uint8_t chunk_pattern(int chunk_index)
{
    return (uint8_t)(0xAA + chunk_index);
}

/**
 * Schreibt einen erkennbaren 8-Byte-Kopf an den Anfang eines Chunks, sodass
 * re15_bss_parse_chunk() verifizierbare Felder liefert. Layout (LE) wie in
 * re15_bss.h / bss_common.c:
 *   +0 run_length_words, +2 id, +4 quant, +6 version.
 * id wird auf RE15_BSS_VLC_ID gesetzt (= "Video-Chunk").
 */
static void write_chunk_header(uint8_t* chunk, uint16_t run_len,
                               uint16_t id, uint16_t quant, uint16_t version)
{
    chunk[0] = (uint8_t)(run_len & 0xFF);
    chunk[1] = (uint8_t)(run_len >> 8);
    chunk[2] = (uint8_t)(id & 0xFF);
    chunk[3] = (uint8_t)(id >> 8);
    chunk[4] = (uint8_t)(quant & 0xFF);
    chunk[5] = (uint8_t)(quant >> 8);
    chunk[6] = (uint8_t)(version & 0xFF);
    chunk[7] = (uint8_t)(version >> 8);
}

/** Erwarteter run_length_words-Wert je Chunk (eindeutig, verifizierbar). */
static uint16_t chunk_run_len(int chunk_index)
{
    return (uint16_t)(0x1000 + chunk_index);
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
        uint8_t* chunk = g_fake_bss_data + i * RE15_BSS_CHUNK_SIZE;
        /* Ganzen Chunk mit eindeutigem Muster füllen ... */
        memset(chunk, chunk_pattern(i), RE15_BSS_CHUNK_SIZE);
        /* ... dann den erkennbaren 8-Byte-Kopf darüberschreiben. */
        write_chunk_header(chunk,
                           chunk_run_len(i),
                           RE15_BSS_VLC_ID,
                           (uint16_t)(i + 1),   /* quant   */
                           3);                   /* version */
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
 * Property-Test 1: re15_bss_parse_chunk — Eingabe-Validierung (Kontrakt)
 *
 * Ersetzt die früheren re15_bss_build_path-Tests (öffentliche Funktion
 * ersatzlos entfernt). Prüft die Kontrakt-Garantien der AKTUELLEN API:
 *   - NULL-Puffer / NULL-out  → 0 (Fehler)
 *   - size < RE15_BSS_HEADER_SIZE → 0 (trunkiert)
 *   - size >= RE15_BSS_HEADER_SIZE → 1 (Erfolg)
 * ========================================================================= */

static int test_parse_chunk_contract(void)
{
    re15_bss_chunk_t out;
    uint8_t hdr[RE15_BSS_HEADER_SIZE];
    int i;

    printf("[1/4] Teste re15_bss_parse_chunk Eingabe-Validierung...\n");

    memset(hdr, 0, sizeof(hdr));

    /* NULL-Puffer → 0 */
    PROP_ASSERT(re15_bss_parse_chunk(NULL, RE15_BSS_HEADER_SIZE, &out) == 0,
        "parse_chunk(NULL buf) should return 0");
    g_passed++;

    /* NULL-out → 0 */
    PROP_ASSERT(re15_bss_parse_chunk(hdr, RE15_BSS_HEADER_SIZE, NULL) == 0,
        "parse_chunk(NULL out) should return 0");
    g_passed++;

    /* Trunkierte Größen (< Header) → 0 */
    for (i = 0; i < RE15_BSS_HEADER_SIZE; i++) {
        PROP_ASSERT(re15_bss_parse_chunk(hdr, (size_t)i, &out) == 0,
            "parse_chunk(size=%d < %d) should return 0", i, RE15_BSS_HEADER_SIZE);
        g_passed++;
    }

    /* Genau Header-Größe → 1 (Erfolg, leerer Payload) */
    PROP_ASSERT(re15_bss_parse_chunk(hdr, RE15_BSS_HEADER_SIZE, &out) == 1,
        "parse_chunk(size=%d) should return 1", RE15_BSS_HEADER_SIZE);
    PROP_ASSERT(out.vlc_payload_size == 0,
        "header-only chunk should have payload size 0, got %zu", out.vlc_payload_size);
    g_passed++;

    printf("    parse_chunk-Kontrakt (NULL / Trunkierung / Header) bestanden.\n\n");
    return 0;
}

/* =========================================================================
 * Property-Test 2: parse_chunk Feld-Dekodierung (LE-Kopf)
 *
 * Verifiziert dass re15_bss_parse_chunk die 4 LE-uint16-Felder korrekt
 * dekodiert und den Payload-Zeiger auf Offset +RE15_BSS_HEADER_SIZE setzt.
 * ========================================================================= */

static int test_parse_chunk_fields(void)
{
    uint8_t buf[RE15_BSS_HEADER_SIZE + 4];
    re15_bss_chunk_t out;
    int rc;

    printf("[2/4] Teste re15_bss_parse_chunk Feld-Dekodierung...\n");

    write_chunk_header(buf, 0x1234, RE15_BSS_VLC_ID, 0x0007, 0x0003);
    buf[RE15_BSS_HEADER_SIZE + 0] = 0xDE;
    buf[RE15_BSS_HEADER_SIZE + 1] = 0xAD;
    buf[RE15_BSS_HEADER_SIZE + 2] = 0xBE;
    buf[RE15_BSS_HEADER_SIZE + 3] = 0xEF;

    rc = re15_bss_parse_chunk(buf, sizeof(buf), &out);
    PROP_ASSERT(rc == 1, "parse_chunk should succeed, got %d", rc);

    PROP_ASSERT(out.run_length_words == 0x1234,
        "run_length_words = 0x%04X (expected 0x1234)", out.run_length_words);
    PROP_ASSERT(out.id == RE15_BSS_VLC_ID,
        "id = 0x%04X (expected 0x%04X)", out.id, RE15_BSS_VLC_ID);
    PROP_ASSERT(out.quant == 0x0007,
        "quant = 0x%04X (expected 0x0007)", out.quant);
    PROP_ASSERT(out.version == 0x0003,
        "version = 0x%04X (expected 0x0003)", out.version);
    PROP_ASSERT(out.vlc_payload == buf + RE15_BSS_HEADER_SIZE,
        "vlc_payload pointer should be buf + %d", RE15_BSS_HEADER_SIZE);
    PROP_ASSERT(out.vlc_payload_size == 4,
        "vlc_payload_size = %zu (expected 4)", out.vlc_payload_size);

    /* Video-Chunk-Erkennung */
    PROP_ASSERT(re15_bss_chunk_has_video(&out) == 1,
        "chunk with id==VLC_ID should be flagged as video");

    g_passed++;
    printf("    LE-Kopf-Dekodierung + Video-Flag bestanden.\n\n");
    return 0;
}

/* =========================================================================
 * Property-Test 3: Chunk-Selektion — korrekter Offset pro Kamera-ID
 *
 * Erstellt einen synthetischen BSS-Puffer mit N Chunks, ruft die lokale
 * Selektions-Nachbildung select_bss_chunk() für jede camera_id [0, N-1] auf
 * und prüft, dass der selektierte Chunk exakt dem erwarteten Byte-Muster
 * entspricht.
 *
 * Verifiziert die Kern-Property:
 *   Selektierter Chunk == Bytes an Offset c * 65536 bis (c+1)*65536-1
 *
 * Zusätzlich wird jeder selektierte Chunk durch die ECHTE Engine-Funktion
 * re15_bss_parse_chunk() geführt → Bindung an die lebende API.
 * ========================================================================= */

static int test_chunk_selection_valid(void)
{
    int num_chunks = 5;
    int cam;
    uint8_t out_chunk[RE15_BSS_CHUNK_SIZE];
    int result;
    int j;

    printf("[3/4] Teste Chunk-Selektion fuer %d Chunks...\n", num_chunks);

    if (setup_fake_bss(num_chunks) != 0) {
        printf("  FAIL: Speicherallokation fuer synthetischen BSS-Puffer\n");
        g_failed++;
        return 1;
    }

    for (cam = 0; cam < num_chunks; cam++) {
        uint8_t expected_byte = chunk_pattern(cam);
        re15_bss_chunk_t parsed;
        int parse_ok;

        memset(out_chunk, 0x00, RE15_BSS_CHUNK_SIZE);

        result = select_bss_chunk(g_fake_bss_data, g_fake_bss_size,
                                  (uint8_t)cam, out_chunk);

        PROP_ASSERT(result == SELECT_OK,
            "select_bss_chunk(cam=%d) returned %d (expected 0)", cam, result);

        /* Stichproben: Anfang (NACH dem 8-Byte-Kopf), Mitte, Ende */
        PROP_ASSERT(out_chunk[RE15_BSS_HEADER_SIZE] == expected_byte,
            "cam=%d: out_chunk[%d]=0x%02X (expected 0x%02X)",
            cam, RE15_BSS_HEADER_SIZE, out_chunk[RE15_BSS_HEADER_SIZE], expected_byte);

        PROP_ASSERT(out_chunk[RE15_BSS_CHUNK_SIZE / 2] == expected_byte,
            "cam=%d: out_chunk[%d]=0x%02X (expected 0x%02X)",
            cam, RE15_BSS_CHUNK_SIZE / 2,
            out_chunk[RE15_BSS_CHUNK_SIZE / 2], expected_byte);

        PROP_ASSERT(out_chunk[RE15_BSS_CHUNK_SIZE - 1] == expected_byte,
            "cam=%d: out_chunk[%d]=0x%02X (expected 0x%02X)",
            cam, RE15_BSS_CHUNK_SIZE - 1,
            out_chunk[RE15_BSS_CHUNK_SIZE - 1], expected_byte);

        /* Vollständige Prüfung des Daten-Bereichs (alle Bytes nach dem Kopf) */
        for (j = RE15_BSS_HEADER_SIZE; j < RE15_BSS_CHUNK_SIZE; j++) {
            PROP_ASSERT(out_chunk[j] == expected_byte,
                "cam=%d: out_chunk[%d]=0x%02X (expected 0x%02X)",
                cam, j, out_chunk[j], expected_byte);
        }

        /* Bindung an die echte Engine-API: parsen + Kopf-Felder prüfen.
         * Der selektierte Chunk muss den für Chunk `cam` geschriebenen
         * Kopf tragen → bestätigt, dass der RICHTIGE Chunk selektiert wurde. */
        parse_ok = re15_bss_parse_chunk(out_chunk, RE15_BSS_CHUNK_SIZE, &parsed);
        PROP_ASSERT(parse_ok == 1,
            "cam=%d: re15_bss_parse_chunk failed on selected chunk", cam);
        PROP_ASSERT(parsed.run_length_words == chunk_run_len(cam),
            "cam=%d: parsed run_length_words=0x%04X (expected 0x%04X) "
            "-> wrong chunk selected",
            cam, parsed.run_length_words, chunk_run_len(cam));
        PROP_ASSERT(parsed.id == RE15_BSS_VLC_ID,
            "cam=%d: parsed id=0x%04X (expected 0x%04X)",
            cam, parsed.id, RE15_BSS_VLC_ID);
        PROP_ASSERT(parsed.quant == (uint16_t)(cam + 1),
            "cam=%d: parsed quant=0x%04X (expected 0x%04X)",
            cam, parsed.quant, (uint16_t)(cam + 1));

        g_passed++;
    }

    printf("    %d Chunks korrekt selektiert (Offset-Arithmetik validiert,\n"
           "    je Chunk durch re15_bss_parse_chunk bestaetigt).\n\n",
           num_chunks);

    teardown_fake_bss();
    return 0;
}

/* =========================================================================
 * Property-Test 4: Chunk-Selektion — camera_id out-of-bounds
 *
 * Prüft: Wenn camera_id >= Anzahl verfügbarer Chunks, soll
 * select_bss_chunk einen Fehlercode zurückgeben (nicht 0).
 * ========================================================================= */

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

    /* camera_id == num_chunks (genau 1 über dem Maximum) */
    memset(out_chunk, 0x00, RE15_BSS_CHUNK_SIZE);
    result = select_bss_chunk(g_fake_bss_data, g_fake_bss_size,
                              (uint8_t)num_chunks, out_chunk);
    PROP_ASSERT(result != SELECT_OK,
        "cam=%d (out of bounds): expected error, got %d", num_chunks, result);
    g_passed++;

    /* camera_id = num_chunks + 1 (deutlich über dem Maximum) */
    result = select_bss_chunk(g_fake_bss_data, g_fake_bss_size,
                              (uint8_t)(num_chunks + 1), out_chunk);
    PROP_ASSERT(result != SELECT_OK,
        "cam=%d (out of bounds): expected error, got %d", num_chunks + 1, result);
    g_passed++;

    /* camera_id = 255 (Maximum für uint8_t) bei nur 3 Chunks */
    result = select_bss_chunk(g_fake_bss_data, g_fake_bss_size, 255, out_chunk);
    PROP_ASSERT(result != SELECT_OK,
        "cam=255 (out of bounds): expected error, got %d", result);
    g_passed++;

    /* Zusätzlich: Verschiedene Chunk-Anzahlen testen — letzter gültiger Chunk
     * gelingt, erster ungültiger Chunk schlägt fehl (Grenzwert-Verhalten). */
    teardown_fake_bss();

    for (cam = 1; cam <= MAX_TEST_CHUNKS; cam++) {
        if (setup_fake_bss(cam) != 0) {
            g_failed++;
            return 1;
        }

        /* Gültiger letzter Chunk */
        memset(out_chunk, 0x00, RE15_BSS_CHUNK_SIZE);
        result = select_bss_chunk(g_fake_bss_data, g_fake_bss_size,
                                  (uint8_t)(cam - 1), out_chunk);
        PROP_ASSERT(result == SELECT_OK,
            "num_chunks=%d, cam=%d (last valid): expected OK, got %d",
            cam, cam - 1, result);

        /* Erster ungültiger Chunk */
        result = select_bss_chunk(g_fake_bss_data, g_fake_bss_size,
                                  (uint8_t)cam, out_chunk);
        PROP_ASSERT(result != SELECT_OK,
            "num_chunks=%d, cam=%d (first invalid): expected error, got %d",
            cam, cam, result);

        g_passed += 2;
        teardown_fake_bss();
    }

    printf("    Out-of-Bounds-Erkennung fuer alle Chunk-Groessen bestanden.\n\n");
    return 0;
}

/* =========================================================================
 * Main
 * ========================================================================= */

int main(void)
{
    printf("=== Property-Test: BSS-Chunk-Selektion nach Kamera-ID ===\n");
    printf("    Validates: Requirements 10.5\n\n");

    /* Test 1: parse_chunk Eingabe-Validierung (ersetzt build_path-Tests) */
    if (test_parse_chunk_contract() != 0) {
        printf("\n=== ABBRUCH bei parse_chunk-Kontrakt ===\n");
        goto done;
    }

    /* Test 2: parse_chunk Feld-Dekodierung */
    if (test_parse_chunk_fields() != 0) {
        printf("\n=== ABBRUCH bei parse_chunk-Feld-Dekodierung ===\n");
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
    printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
