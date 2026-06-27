/**
 * @file test_sca_parse.c
 * @brief Unit-Test fuer den SCA-Kollisionsdaten-Parser
 *
 * Testet re15_sca_parse() mit synthetischen Testdaten:
 * - Gueltige Daten (Rechteck, Kreis, Treppen)
 * - Leere Daten (count=0)
 * - Ungueltige Eingaben (NULL, zu kleine Puffer)
 */

#include "re15_collision.h"
#include "re15_error.h"

#include <stdio.h>
#include <string.h>

/* Hilfsfunktion: uint16 LE in Puffer schreiben */
static void write_le16(uint8_t* p, uint16_t val)
{
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)((val >> 8) & 0xFF);
}

/* Hilfsfunktion: int16 LE in Puffer schreiben */
static void write_le16s(uint8_t* p, int16_t val)
{
    write_le16(p, (uint16_t)val);
}

static int test_parse_valid_entries(void)
{
    /*
     * Erstelle 3 Eintraege:
     *   0: Rechteck (type=1, corner_x=-500, corner_z=300, width=200, depth=100, density=0, flags=0)
     *   1: Kreis    (type=3, corner_x=100, corner_z=-200, width=50, depth=0, density=1, flags=0)
     *   2: Treppe   (type=12, corner_x=0, corner_z=0, width=100, depth=50, density=2, flags=0x8000)
     */
    uint8_t buf[4 + 3 * 14];
    re15_sca_data_t sca;
    int result;

    memset(buf, 0, sizeof(buf));

    /* Header: count=3, reserved=0 */
    write_le16(buf + 0, 3);
    write_le16(buf + 2, 0);

    /* Eintrag 0: Rechteck */
    write_le16(buf + 4 + 0,  1);     /* type */
    write_le16s(buf + 4 + 2, -500);  /* corner_x */
    write_le16s(buf + 4 + 4, 300);   /* corner_z */
    write_le16s(buf + 4 + 6, 200);   /* width */
    write_le16s(buf + 4 + 8, 100);   /* depth */
    write_le16(buf + 4 + 10, 0);     /* density */
    write_le16(buf + 4 + 12, 0);     /* flags */

    /* Eintrag 1: Kreis */
    write_le16(buf + 18 + 0,  3);    /* type */
    write_le16s(buf + 18 + 2, 100);  /* corner_x (center) */
    write_le16s(buf + 18 + 4, -200); /* corner_z (center) */
    write_le16s(buf + 18 + 6, 50);   /* width (radius) */
    write_le16s(buf + 18 + 8, 0);    /* depth */
    write_le16(buf + 18 + 10, 1);    /* density */
    write_le16(buf + 18 + 12, 0);    /* flags */

    /* Eintrag 2: Treppe aufwaerts */
    write_le16(buf + 32 + 0,  12);   /* type */
    write_le16s(buf + 32 + 2, 0);    /* corner_x */
    write_le16s(buf + 32 + 4, 0);    /* corner_z */
    write_le16s(buf + 32 + 6, 100);  /* width */
    write_le16s(buf + 32 + 8, 50);   /* depth */
    write_le16(buf + 32 + 10, 2);    /* density */
    write_le16(buf + 32 + 12, 0x8000); /* flags */

    result = re15_sca_parse(buf, sizeof(buf), &sca);
    if (result != RE15_IO_OK) {
        fprintf(stderr, "FAIL: re15_sca_parse returned %d (expected 0)\n", result);
        return 1;
    }

    if (sca.count != 3) {
        fprintf(stderr, "FAIL: count=%u (expected 3)\n", sca.count);
        return 1;
    }

    /* Pruefe Eintrag 0 */
    if (sca.entries[0].type != 1 ||
        sca.entries[0].corner_x != -500 ||
        sca.entries[0].corner_z != 300 ||
        sca.entries[0].width != 200 ||
        sca.entries[0].depth != 100 ||
        sca.entries[0].density != 0) {
        fprintf(stderr, "FAIL: Eintrag 0 (Rechteck) Felder stimmen nicht\n");
        fprintf(stderr, "  type=%u corner_x=%d corner_z=%d width=%d depth=%d density=%u\n",
                sca.entries[0].type, sca.entries[0].corner_x,
                sca.entries[0].corner_z, sca.entries[0].width,
                sca.entries[0].depth, sca.entries[0].density);
        return 1;
    }

    /* Pruefe Eintrag 1 */
    if (sca.entries[1].type != 3 ||
        sca.entries[1].corner_x != 100 ||
        sca.entries[1].corner_z != -200 ||
        sca.entries[1].width != 50 ||
        sca.entries[1].density != 1) {
        fprintf(stderr, "FAIL: Eintrag 1 (Kreis) Felder stimmen nicht\n");
        return 1;
    }

    /* Pruefe Eintrag 2 */
    if (sca.entries[2].type != 12 ||
        sca.entries[2].width != 100 ||
        sca.entries[2].depth != 50 ||
        sca.entries[2].density != 2 ||
        sca.entries[2].flags != 0x8000) {
        fprintf(stderr, "FAIL: Eintrag 2 (Treppe) Felder stimmen nicht\n");
        return 1;
    }

    printf("PASS: test_parse_valid_entries\n");
    return 0;
}

static int test_parse_empty(void)
{
    uint8_t buf[4];
    re15_sca_data_t sca;
    int result;

    /* Header: count=0, reserved=0 */
    write_le16(buf + 0, 0);
    write_le16(buf + 2, 0);

    result = re15_sca_parse(buf, sizeof(buf), &sca);
    if (result != RE15_IO_OK) {
        fprintf(stderr, "FAIL: empty parse returned %d\n", result);
        return 1;
    }
    if (sca.count != 0) {
        fprintf(stderr, "FAIL: empty parse count=%u (expected 0)\n", sca.count);
        return 1;
    }
    if (sca.entries != NULL) {
        fprintf(stderr, "FAIL: empty parse entries not NULL\n");
        return 1;
    }

    printf("PASS: test_parse_empty\n");
    return 0;
}

static int test_parse_null_input(void)
{
    re15_sca_data_t sca;
    uint8_t buf[4] = {0};
    int result;

    /* NULL data pointer */
    result = re15_sca_parse(NULL, 100, &sca);
    if (result != RE15_IO_INVALID_ARG) {
        fprintf(stderr, "FAIL: NULL data should return INVALID_ARG, got %d\n", result);
        return 1;
    }

    /* NULL output pointer */
    result = re15_sca_parse(buf, sizeof(buf), NULL);
    if (result != RE15_IO_INVALID_ARG) {
        fprintf(stderr, "FAIL: NULL out should return INVALID_ARG, got %d\n", result);
        return 1;
    }

    printf("PASS: test_parse_null_input\n");
    return 0;
}

static int test_parse_buffer_too_small(void)
{
    uint8_t buf[4 + 10]; /* Header OK, aber nicht genug fuer 1 Eintrag (14 Bytes) */
    re15_sca_data_t sca;
    int result;

    /* Header: count=1, reserved=0 */
    write_le16(buf + 0, 1);
    write_le16(buf + 2, 0);
    memset(buf + 4, 0, 10);

    result = re15_sca_parse(buf, sizeof(buf), &sca);
    if (result != RE15_IO_READ_ERROR) {
        fprintf(stderr, "FAIL: small buffer should return READ_ERROR, got %d\n", result);
        return 1;
    }

    printf("PASS: test_parse_buffer_too_small\n");
    return 0;
}

static int test_parse_stair_down(void)
{
    uint8_t buf[4 + 14];
    re15_sca_data_t sca;
    int result;

    memset(buf, 0, sizeof(buf));

    /* Header: count=1, reserved=0 */
    write_le16(buf + 0, 1);
    write_le16(buf + 2, 0);

    /* Eintrag: Treppe abwaerts */
    write_le16(buf + 4 + 0,  13);    /* type = stair_down */
    write_le16s(buf + 4 + 2, -1000); /* corner_x */
    write_le16s(buf + 4 + 4, 2000);  /* corner_z */
    write_le16s(buf + 4 + 6, 150);   /* width */
    write_le16s(buf + 4 + 8, 75);    /* depth */
    write_le16(buf + 4 + 10, 5);     /* density */
    write_le16(buf + 4 + 12, 0);     /* flags */

    result = re15_sca_parse(buf, sizeof(buf), &sca);
    if (result != RE15_IO_OK) {
        fprintf(stderr, "FAIL: stair_down parse returned %d\n", result);
        return 1;
    }

    if (sca.count != 1) {
        fprintf(stderr, "FAIL: stair_down count=%u (expected 1)\n", sca.count);
        return 1;
    }

    if (sca.entries[0].type != 13 ||
        sca.entries[0].corner_x != -1000 ||
        sca.entries[0].corner_z != 2000 ||
        sca.entries[0].width != 150 ||
        sca.entries[0].depth != 75 ||
        sca.entries[0].density != 5) {
        fprintf(stderr, "FAIL: stair_down entry fields incorrect\n");
        return 1;
    }

    printf("PASS: test_parse_stair_down\n");
    return 0;
}

int main(void)
{
    int failures = 0;

    failures += test_parse_valid_entries();
    failures += test_parse_empty();
    failures += test_parse_null_input();
    failures += test_parse_buffer_too_small();
    failures += test_parse_stair_down();

    if (failures == 0) {
        printf("\nALL SCA TESTS PASSED\n");
    } else {
        fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
