/**
 * @file test_audio_vab.c
 * @brief Unit-Tests fuer VAB-Header-Parsing (Validierung der VH-Struktur)
 *
 * Testet die VAB-Header-Validierungslogik mit synthetischen VH-Puffern:
 * - Gueltiger VAB-Header → korrekte Sample-Anzahl (num_vags)
 * - Ungueltiger Magic → Fehlercode ohne Absturz
 *
 * Da die VAB-Parse-Funktion in audio_pc.c statisch und SDL2-abhaengig ist,
 * wird hier eine minimale, testlokale Kopie der Header-Validierungslogik
 * verwendet. Dies testet exakt denselben Algorithmus wie vab_parse_and_decode().
 *
 * Requirements: 6.6, 12.1
 */

/* Engine-Transplant 2026: re15_types.h wurde aus dem oeffentlichen Include
 * entfernt (nur noch in _legacy_minimal/, NICHT im Build). Die aktuellen
 * Engine-Header beziehen die Fixwidth-Integer direkt aus <stdint.h>
 * (siehe re15_port/include/re15_engine.h:18, re15_vab.h:26). Dieser Test
 * braucht von re15_types.h ausschliesslich uint8_t/uint16_t/uint32_t —
 * daher <stdint.h> statt eines _legacy_minimal-Headers. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* =========================================================================
 * Testlokale Kopie der VAB-Header-Struktur und Validierungslogik
 * (identisch zu audio_pc.c — vab_header_t + vab_parse_and_decode Header-Teil)
 * ========================================================================= */

/** VAB Magic "pBAV" in little-endian */
#define VAB_MAGIC 0x56414270u

/** Maximale Samples pro VAB-Bank */
#define VAB_MAX_TONES 128

/** VAB-Header-Struktur (32 Bytes) — identisch zu audio_pc.c */
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t vab_id;
    uint32_t total_size;
    uint16_t reserved;
    uint16_t num_programs;
    uint16_t num_tones;
    uint16_t num_vags;
    uint8_t  master_vol;
    uint8_t  master_pan;
    uint8_t  bank_attr1;
    uint8_t  bank_attr2;
    uint32_t reserved2;
} vab_header_t;

/**
 * Validiert einen VAB-Header und extrahiert die Sample-Anzahl.
 *
 * Repliziert die Header-Validierungslogik aus audio_pc.c::vab_parse_and_decode().
 *
 * @param vh        VH-Header-Daten
 * @param vh_size   Groesse des VH-Puffers in Bytes
 * @param out_num_vags  Ausgabe: Anzahl der Samples (nur bei Erfolg gesetzt)
 * @return          0 bei Erfolg, -1 bei Fehler (zu klein, ungueltig)
 */
static int vab_validate_header(const uint8_t* vh, int vh_size, int* out_num_vags)
{
    const vab_header_t* header;
    int num_vags;

    if (!vh || vh_size < (int)sizeof(vab_header_t)) {
        return -1;
    }

    header = (const vab_header_t*)vh;

    /* Magic pruefen — identisch zu audio_pc.c */
    if (header->magic != VAB_MAGIC) {
        return -1;
    }

    num_vags = (int)header->num_vags;
    if (num_vags <= 0 || num_vags > VAB_MAX_TONES) {
        return -1;
    }

    if (out_num_vags) {
        *out_num_vags = num_vags;
    }
    return 0;
}

/* =========================================================================
 * Hilfsfunktionen
 * ========================================================================= */

/** 32-Bit Little-Endian in Puffer schreiben */
static void write_le32(uint8_t* p, uint32_t val)
{
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)((val >> 8) & 0xFF);
    p[2] = (uint8_t)((val >> 16) & 0xFF);
    p[3] = (uint8_t)((val >> 24) & 0xFF);
}

/** 16-Bit Little-Endian in Puffer schreiben */
static void write_le16(uint8_t* p, uint16_t val)
{
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)((val >> 8) & 0xFF);
}

/**
 * Erstellt einen synthetischen gueltigen VAB-Header im Puffer.
 *
 * @param buf           Zielpuffer (mindestens 32 Bytes)
 * @param num_programs  Anzahl Programme (typisch 1-128)
 * @param num_tones     Anzahl Tones (typisch 1-128)
 * @param num_vags      Anzahl Samples/VAGs (typisch 1-128)
 */
static void build_valid_vab_header(uint8_t* buf, uint16_t num_programs,
                                   uint16_t num_tones, uint16_t num_vags)
{
    memset(buf, 0, sizeof(vab_header_t));

    /* Magic: "pBAV" = 0x56414270 little-endian */
    write_le32(buf + 0, VAB_MAGIC);
    /* Version */
    write_le32(buf + 4, 0x00000007);
    /* VAB ID */
    write_le32(buf + 8, 0x00000001);
    /* Total size (willkuerlich) */
    write_le32(buf + 12, 0x00010000);
    /* Reserved */
    write_le16(buf + 16, 0x0000);
    /* num_programs */
    write_le16(buf + 18, num_programs);
    /* num_tones */
    write_le16(buf + 20, num_tones);
    /* num_vags */
    write_le16(buf + 22, num_vags);
    /* master_vol, master_pan */
    buf[24] = 127;  /* master volume */
    buf[25] = 64;   /* master pan (center) */
    /* bank_attr1, bank_attr2, reserved2: 0 */
}

/* =========================================================================
 * Test 1: Gueltiger VAB-Header → korrekte Sample-Anzahl
 * ========================================================================= */

static int test_vab_valid_header(void)
{
    uint8_t buf[64];
    int num_vags = 0;
    int result;

    /* Erstelle Header mit 24 Samples */
    build_valid_vab_header(buf, 4, 16, 24);

    result = vab_validate_header(buf, (int)sizeof(vab_header_t), &num_vags);
    if (result != 0) {
        fprintf(stderr, "FAIL: vab_validate_header returned %d (expected 0)\n", result);
        return 1;
    }

    if (num_vags != 24) {
        fprintf(stderr, "FAIL: num_vags=%d (expected 24)\n", num_vags);
        return 1;
    }

    printf("PASS: test_vab_valid_header (24 Samples)\n");
    return 0;
}

/* =========================================================================
 * Test 2: Verschiedene gueltige Sample-Anzahlen
 * ========================================================================= */

static int test_vab_valid_various_counts(void)
{
    uint8_t buf[64];
    int num_vags = 0;
    int result;
    int test_counts[] = { 1, 16, 64, 100, 128 };
    int i;

    for (i = 0; i < 5; i++) {
        build_valid_vab_header(buf, 1, (uint16_t)test_counts[i],
                               (uint16_t)test_counts[i]);

        result = vab_validate_header(buf, (int)sizeof(vab_header_t), &num_vags);
        if (result != 0) {
            fprintf(stderr, "FAIL: vab_validate_header returned %d for num_vags=%d\n",
                    result, test_counts[i]);
            return 1;
        }
        if (num_vags != test_counts[i]) {
            fprintf(stderr, "FAIL: num_vags=%d (expected %d)\n",
                    num_vags, test_counts[i]);
            return 1;
        }
    }

    printf("PASS: test_vab_valid_various_counts\n");
    return 0;
}

/* =========================================================================
 * Test 3: Ungueltiger Magic → Fehlercode ohne Absturz
 * ========================================================================= */

static int test_vab_invalid_magic(void)
{
    uint8_t buf[64];
    int num_vags = -1;
    int result;

    /* Erstelle ansonsten gueltigen Header, aber mit falschem Magic */
    build_valid_vab_header(buf, 4, 16, 24);

    /* Ueberschreibe Magic mit ungueltigem Wert */
    write_le32(buf + 0, 0xDEADBEEF);

    result = vab_validate_header(buf, (int)sizeof(vab_header_t), &num_vags);
    if (result == 0) {
        fprintf(stderr, "FAIL: Invalid magic should return error, got success\n");
        return 1;
    }

    printf("PASS: test_vab_invalid_magic (0xDEADBEEF rejected)\n");
    return 0;
}

/* =========================================================================
 * Test 4: Magic komplett null → Fehlercode
 * ========================================================================= */

static int test_vab_zero_magic(void)
{
    uint8_t buf[64];
    int result;

    build_valid_vab_header(buf, 4, 16, 24);
    write_le32(buf + 0, 0x00000000);

    result = vab_validate_header(buf, (int)sizeof(vab_header_t), NULL);
    if (result == 0) {
        fprintf(stderr, "FAIL: Zero magic should return error\n");
        return 1;
    }

    printf("PASS: test_vab_zero_magic\n");
    return 0;
}

/* =========================================================================
 * Test 5: Puffer zu klein (< 32 Bytes) → Fehler ohne Absturz
 * ========================================================================= */

static int test_vab_buffer_too_small(void)
{
    uint8_t buf[16];
    int result;

    memset(buf, 0, sizeof(buf));
    write_le32(buf + 0, VAB_MAGIC); /* Magic korrekt, aber Puffer zu klein */

    result = vab_validate_header(buf, 16, NULL);
    if (result == 0) {
        fprintf(stderr, "FAIL: Buffer 16 bytes should return error\n");
        return 1;
    }

    /* Auch exakt 31 Bytes (knapp unter Minimum) */
    result = vab_validate_header(buf, 31, NULL);
    if (result == 0) {
        fprintf(stderr, "FAIL: Buffer 31 bytes should return error\n");
        return 1;
    }

    printf("PASS: test_vab_buffer_too_small\n");
    return 0;
}

/* =========================================================================
 * Test 6: NULL-Zeiger → Fehler ohne Absturz
 * ========================================================================= */

static int test_vab_null_pointer(void)
{
    int result;

    result = vab_validate_header(NULL, 64, NULL);
    if (result == 0) {
        fprintf(stderr, "FAIL: NULL pointer should return error\n");
        return 1;
    }

    printf("PASS: test_vab_null_pointer\n");
    return 0;
}

/* =========================================================================
 * Test 7: num_vags ausserhalb gueltiger Grenzen (0, >128) → Fehler
 * ========================================================================= */

static int test_vab_invalid_vag_count(void)
{
    uint8_t buf[64];
    int result;

    /* num_vags = 0 → ungueltig */
    build_valid_vab_header(buf, 4, 16, 0);
    result = vab_validate_header(buf, (int)sizeof(vab_header_t), NULL);
    if (result == 0) {
        fprintf(stderr, "FAIL: num_vags=0 should return error\n");
        return 1;
    }

    /* num_vags = 129 → ueber Maximum (128) */
    build_valid_vab_header(buf, 4, 16, 129);
    result = vab_validate_header(buf, (int)sizeof(vab_header_t), NULL);
    if (result == 0) {
        fprintf(stderr, "FAIL: num_vags=129 should return error\n");
        return 1;
    }

    /* num_vags = 0xFFFF → weit ueber Maximum */
    build_valid_vab_header(buf, 4, 16, 0xFFFF);
    result = vab_validate_header(buf, (int)sizeof(vab_header_t), NULL);
    if (result == 0) {
        fprintf(stderr, "FAIL: num_vags=0xFFFF should return error\n");
        return 1;
    }

    printf("PASS: test_vab_invalid_vag_count\n");
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(void)
{
    int failures = 0;

    printf("=== VAB-Parsing Unit Tests ===\n\n");

    failures += test_vab_valid_header();
    failures += test_vab_valid_various_counts();
    failures += test_vab_invalid_magic();
    failures += test_vab_zero_magic();
    failures += test_vab_buffer_too_small();
    failures += test_vab_null_pointer();
    failures += test_vab_invalid_vag_count();

    if (failures == 0) {
        printf("\nALL VAB-PARSING TESTS PASSED\n");
    } else {
        fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
