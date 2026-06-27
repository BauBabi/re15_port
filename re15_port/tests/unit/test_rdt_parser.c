/**
 * @file test_rdt_parser.c
 * @brief Unit-Tests fuer den RDT-Parser (re15_rdt_parse)
 *
 * Testet mit synthetischen Puffern:
 * - Gueltige Offsets → korrekte Pointer
 * - Null-Offsets → NULL-Pointer
 * - Zu kleiner Puffer → Fehler
 * - NULL-Argumente → Fehler
 * - Out-of-Bounds-Offsets → NULL-Pointer
 *
 * Requirements: 10.1, 10.8, 12.1
 */

#include "re15_rdt.h"
#include "re15_error.h"

#include <stdio.h>
#include <string.h>

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

/* =========================================================================
 * Test 1: Gueltige Offsets — alle 21 Sektionen zeigen in den Puffer
 * ========================================================================= */

static int test_rdt_parse_valid(void)
{
    /*
     * Erstelle einen synthetischen RDT-Puffer:
     * - 8 Byte Header (ignoriert vom Parser)
     * - 21 x 4 Byte Adresstabelle (0x08-0x5C)
     * - Datenbereiche ab 0x60
     *
     * Gesamtgroesse: 0x60 (Header+Tabelle) + 21*16 (Daten) = 432 Bytes
     */
    uint8_t buf[0x60 + 21 * 16];
    re15_rdt_t rdt;
    int result;
    int i;

    memset(buf, 0xAB, sizeof(buf)); /* Fuellmuster */

    /* Header-Bytes (0x00-0x07): willkuerlich */
    buf[0] = 'R'; buf[1] = 'D'; buf[2] = 'T'; buf[3] = '0';
    buf[4] = 0x00; buf[5] = 0x00; buf[6] = 0x00; buf[7] = 0x00;

    /* Adresstabelle: Jede Sektion bekommt einen gültigen Offset */
    for (i = 0; i < RE15_RDT_SECTION_COUNT; i++) {
        uint32_t offset = 0x60 + (uint32_t)(i * 16);
        write_le32(buf + 0x08 + i * 4, offset);
    }

    result = re15_rdt_parse(buf, (int)sizeof(buf), &rdt);
    if (result != RE15_IO_OK) {
        fprintf(stderr, "FAIL: re15_rdt_parse returned %d (expected 0)\n", result);
        return 1;
    }

    /* Alle 21 Pointer muessen korrekt in den Puffer zeigen */
    {
        uint8_t** sections[RE15_RDT_SECTION_COUNT];
        sections[0]  = &rdt.snd0_edt;
        sections[1]  = &rdt.snd0_vh;
        sections[2]  = &rdt.snd0_vb;
        sections[3]  = &rdt.snd1_edt;
        sections[4]  = &rdt.snd1_vh;
        sections[5]  = &rdt.snd1_vb;
        sections[6]  = &rdt.collision;
        sections[7]  = &rdt.camera;
        sections[8]  = &rdt.zone;
        sections[9]  = &rdt.light;
        sections[10] = &rdt.md1_ptr;
        sections[11] = &rdt.floor;
        sections[12] = &rdt.block;
        sections[13] = &rdt.message;
        sections[14] = &rdt.main_scd;
        sections[15] = &rdt.sub_scd;
        sections[16] = &rdt.extra_scd;
        sections[17] = &rdt.effect;
        sections[18] = &rdt.esp_tim;
        sections[19] = &rdt.model_tim;
        sections[20] = &rdt.animation;

        for (i = 0; i < RE15_RDT_SECTION_COUNT; i++) {
            uint8_t* expected_ptr = buf + 0x60 + i * 16;
            if (*sections[i] != expected_ptr) {
                fprintf(stderr, "FAIL: Sektion %d: Pointer mismatch\n", i);
                fprintf(stderr, "  expected: %p, actual: %p\n",
                        (void*)expected_ptr, (void*)*sections[i]);
                return 1;
            }
        }
    }

    printf("PASS: test_rdt_parse_valid\n");
    return 0;
}

/* =========================================================================
 * Test 2: Null-Offsets → NULL-Pointer
 * ========================================================================= */

static int test_rdt_parse_null_offsets(void)
{
    /*
     * Erstelle einen Puffer mit einigen Null-Offsets.
     * Sektionen 0, 5, 10, 15, 20 haben Offset=0 → NULL
     * Alle anderen haben gueltige Offsets.
     */
    uint8_t buf[0x60 + 16 * 16]; /* genug Platz fuer 16 gueltige Sektionen */
    re15_rdt_t rdt;
    int result;
    int i;
    int data_idx;

    memset(buf, 0xCC, sizeof(buf));

    /* Header */
    buf[0] = 'R'; buf[1] = 'D'; buf[2] = 'T'; buf[3] = '0';
    buf[4] = 0x00; buf[5] = 0x00; buf[6] = 0x00; buf[7] = 0x00;

    /* Adresstabelle */
    data_idx = 0;
    for (i = 0; i < RE15_RDT_SECTION_COUNT; i++) {
        if (i == 0 || i == 5 || i == 10 || i == 15 || i == 20) {
            /* Null-Offset: Sektion nicht vorhanden */
            write_le32(buf + 0x08 + i * 4, 0x00000000);
        } else {
            /* Gueltiger Offset */
            uint32_t offset = 0x60 + (uint32_t)(data_idx * 16);
            write_le32(buf + 0x08 + i * 4, offset);
            data_idx++;
        }
    }

    result = re15_rdt_parse(buf, (int)sizeof(buf), &rdt);
    if (result != RE15_IO_OK) {
        fprintf(stderr, "FAIL: re15_rdt_parse returned %d (expected 0)\n", result);
        return 1;
    }

    /* Pruefe: Null-Offset-Sektionen muessen NULL sein */
    {
        uint8_t** sections[RE15_RDT_SECTION_COUNT];
        sections[0]  = &rdt.snd0_edt;
        sections[1]  = &rdt.snd0_vh;
        sections[2]  = &rdt.snd0_vb;
        sections[3]  = &rdt.snd1_edt;
        sections[4]  = &rdt.snd1_vh;
        sections[5]  = &rdt.snd1_vb;
        sections[6]  = &rdt.collision;
        sections[7]  = &rdt.camera;
        sections[8]  = &rdt.zone;
        sections[9]  = &rdt.light;
        sections[10] = &rdt.md1_ptr;
        sections[11] = &rdt.floor;
        sections[12] = &rdt.block;
        sections[13] = &rdt.message;
        sections[14] = &rdt.main_scd;
        sections[15] = &rdt.sub_scd;
        sections[16] = &rdt.extra_scd;
        sections[17] = &rdt.effect;
        sections[18] = &rdt.esp_tim;
        sections[19] = &rdt.model_tim;
        sections[20] = &rdt.animation;

        for (i = 0; i < RE15_RDT_SECTION_COUNT; i++) {
            if (i == 0 || i == 5 || i == 10 || i == 15 || i == 20) {
                /* Muss NULL sein */
                if (*sections[i] != NULL) {
                    fprintf(stderr, "FAIL: Sektion %d sollte NULL sein, ist %p\n",
                            i, (void*)*sections[i]);
                    return 1;
                }
            } else {
                /* Muss non-NULL sein */
                if (*sections[i] == NULL) {
                    fprintf(stderr, "FAIL: Sektion %d sollte non-NULL sein\n", i);
                    return 1;
                }
            }
        }
    }

    printf("PASS: test_rdt_parse_null_offsets\n");
    return 0;
}

/* =========================================================================
 * Test 3: Puffer zu klein (<0x60 Bytes) → Fehler
 * ========================================================================= */

static int test_rdt_parse_buffer_too_small(void)
{
    uint8_t buf[0x50]; /* kleiner als RDT_MIN_SIZE (0x60) */
    re15_rdt_t rdt;
    int result;

    memset(buf, 0, sizeof(buf));

    result = re15_rdt_parse(buf, (int)sizeof(buf), &rdt);
    if (result != RE15_IO_READ_ERROR) {
        fprintf(stderr, "FAIL: small buffer should return RE15_IO_READ_ERROR (%d), got %d\n",
                RE15_IO_READ_ERROR, result);
        return 1;
    }

    /* Auch exakt 0x5F (knapp unter Minimum) */
    result = re15_rdt_parse(buf, 0x5F, &rdt);
    if (result != RE15_IO_READ_ERROR) {
        fprintf(stderr, "FAIL: buffer 0x5F should return RE15_IO_READ_ERROR, got %d\n", result);
        return 1;
    }

    printf("PASS: test_rdt_parse_buffer_too_small\n");
    return 0;
}

/* =========================================================================
 * Test 4: NULL-Argumente → RE15_IO_INVALID_ARG
 * ========================================================================= */

static int test_rdt_parse_null_args(void)
{
    uint8_t buf[0x60];
    re15_rdt_t rdt;
    int result;

    memset(buf, 0, sizeof(buf));

    /* NULL buffer */
    result = re15_rdt_parse(NULL, 0x60, &rdt);
    if (result != RE15_IO_INVALID_ARG) {
        fprintf(stderr, "FAIL: NULL buffer should return RE15_IO_INVALID_ARG (%d), got %d\n",
                RE15_IO_INVALID_ARG, result);
        return 1;
    }

    /* NULL output */
    result = re15_rdt_parse(buf, 0x60, NULL);
    if (result != RE15_IO_INVALID_ARG) {
        fprintf(stderr, "FAIL: NULL output should return RE15_IO_INVALID_ARG (%d), got %d\n",
                RE15_IO_INVALID_ARG, result);
        return 1;
    }

    /* Both NULL */
    result = re15_rdt_parse(NULL, 0x60, NULL);
    if (result != RE15_IO_INVALID_ARG) {
        fprintf(stderr, "FAIL: both NULL should return RE15_IO_INVALID_ARG, got %d\n", result);
        return 1;
    }

    printf("PASS: test_rdt_parse_null_args\n");
    return 0;
}

/* =========================================================================
 * Test 5: Out-of-Bounds-Offset → NULL-Pointer (als fehlend behandelt)
 * ========================================================================= */

static int test_rdt_parse_out_of_bounds_offset(void)
{
    /*
     * Puffer: genau 0x60 Bytes (Minimum).
     * Setze einige Offsets auf Werte >= buffer size → muessen NULL werden.
     * Setze andere auf 0 (auch NULL).
     * Ein Offset genau am Rand (0x5F) ist gueltig (zeigt auf letztes Byte).
     */
    uint8_t buf[0x80]; /* 128 Bytes */
    re15_rdt_t rdt;
    int result;
    int i;

    memset(buf, 0xDD, sizeof(buf));

    /* Header */
    buf[0] = 'R'; buf[1] = 'D'; buf[2] = 'T'; buf[3] = '0';
    buf[4] = 0x00; buf[5] = 0x00; buf[6] = 0x00; buf[7] = 0x00;

    /* Sektion 0: Offset weit jenseits des Puffers */
    write_le32(buf + 0x08, 0x00010000); /* 64KB — weit out-of-bounds */

    /* Sektion 1: Offset genau == size (out-of-bounds, >= size) */
    write_le32(buf + 0x0C, 0x80); /* == sizeof(buf) */

    /* Sektion 2: Gueltiger Offset (innerhalb des Puffers) */
    write_le32(buf + 0x10, 0x60); /* zeigt auf Datenbereich */

    /* Sektion 3: Null-Offset */
    write_le32(buf + 0x14, 0x00000000);

    /* Sektion 4: Offset knapp innerhalb (size-1) */
    write_le32(buf + 0x18, 0x7F); /* letztes Byte im Puffer */

    /* Restliche Sektionen: Null-Offset */
    for (i = 5; i < RE15_RDT_SECTION_COUNT; i++) {
        write_le32(buf + 0x08 + i * 4, 0x00000000);
    }

    result = re15_rdt_parse(buf, (int)sizeof(buf), &rdt);
    if (result != RE15_IO_OK) {
        fprintf(stderr, "FAIL: re15_rdt_parse returned %d (expected 0)\n", result);
        return 1;
    }

    /* Sektion 0: Out-of-bounds → NULL */
    if (rdt.snd0_edt != NULL) {
        fprintf(stderr, "FAIL: Sektion 0 (OOB 0x10000) sollte NULL sein, ist %p\n",
                (void*)rdt.snd0_edt);
        return 1;
    }

    /* Sektion 1: Offset == size → out-of-bounds → NULL */
    if (rdt.snd0_vh != NULL) {
        fprintf(stderr, "FAIL: Sektion 1 (offset==size) sollte NULL sein, ist %p\n",
                (void*)rdt.snd0_vh);
        return 1;
    }

    /* Sektion 2: Gueltiger Offset → non-NULL, zeigt auf buf+0x60 */
    if (rdt.snd0_vb != buf + 0x60) {
        fprintf(stderr, "FAIL: Sektion 2 sollte buf+0x60 sein\n");
        fprintf(stderr, "  expected: %p, actual: %p\n",
                (void*)(buf + 0x60), (void*)rdt.snd0_vb);
        return 1;
    }

    /* Sektion 3: Null-Offset → NULL */
    if (rdt.snd1_edt != NULL) {
        fprintf(stderr, "FAIL: Sektion 3 (null offset) sollte NULL sein\n");
        return 1;
    }

    /* Sektion 4: Offset 0x7F (innerhalb) → non-NULL */
    if (rdt.snd1_vh != buf + 0x7F) {
        fprintf(stderr, "FAIL: Sektion 4 sollte buf+0x7F sein\n");
        fprintf(stderr, "  expected: %p, actual: %p\n",
                (void*)(buf + 0x7F), (void*)rdt.snd1_vh);
        return 1;
    }

    printf("PASS: test_rdt_parse_out_of_bounds_offset\n");
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(void)
{
    int failures = 0;

    printf("=== RDT Parser Unit Tests ===\n\n");

    failures += test_rdt_parse_valid();
    failures += test_rdt_parse_null_offsets();
    failures += test_rdt_parse_buffer_too_small();
    failures += test_rdt_parse_null_args();
    failures += test_rdt_parse_out_of_bounds_offset();

    if (failures == 0) {
        printf("\nALL RDT PARSER TESTS PASSED\n");
    } else {
        fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
