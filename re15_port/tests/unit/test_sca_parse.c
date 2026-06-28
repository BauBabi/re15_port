/**
 * @file test_sca_parse.c
 * @brief Unit-Test fuer den SCA-Kollisionsdaten-Parser (re15_rdt_parse)
 *
 * REAKTIVIERT 2026-06-28: Der frueher getestete Stand-alone-Parser
 * `re15_sca_parse()` mit eigenem `re15_sca_data_t`/14-Byte-Eintrag und
 * 4-Byte-Header (count/reserved) existiert in der aktuellen Engine NICHT
 * mehr. Die SCA-Kollisionsdaten werden jetzt INLINE von `re15_rdt_parse()`
 * (engine/src/rdt_common.c) aus dem RDT-Puffer geparst:
 *
 *   - RDT-Header: 0x60 Bytes; Adresstabelle bei 0x08..0x5c (4-B-Stride).
 *   - Offset 0x20 (u32 LE): Zeiger auf den SCA-Block (0 = kein SCA).
 *   - SCA-Block-Header (24 B): ceilingX u16, ceilingZ u16, count[5] u32 LE
 *     (5 Quadranten-Partitionen; total = Summe).
 *   - Eintrag (12 B, lib_sca v1.5 / "BH2 prototype", re15_sca_entry_t in
 *     re15_rdt.h:57-66): width u16, density u16, x s16, z s16, type u8,
 *     u0 u8, u1 u8, floor u8.
 *   - `out->sca`/`out->sca_count` werden nur gesetzt, wenn total>0 UND der
 *     Block exakt in den Puffer passt (rdt_common.c:261-264).
 *   - re15_rdt_parse: 0 bei Erfolg, -1 bei NULL-Pointer, -2 bei size<0x60
 *     (rdt_common.c:208-211).
 *
 * Die TEST-INTENTION ist erhalten: korrekte Little-Endian-Feldextraktion der
 * Kollisionseintraege, leerer SCA-Block, ungueltige Eingaben (NULL, zu kleiner
 * Puffer) und Truncation-Verhalten. Das alte 14-B/4-B-Header-Format ist obsolet
 * und durch das aktuelle 12-B/24-B-Header-Format ersetzt.
 */

#include "re15_rdt.h"

#include <stdio.h>
#include <string.h>

/* RDT-Header (rdt_common.c:211): mindestens 0x60 Bytes. */
#define RDT_HEADER_SIZE   0x60
/* SCA-Block-Header (rdt_common.c:250-259): 24 Bytes. */
#define SCA_HEADER_SIZE   24
/* SCA-Eintrag (re15_rdt.h:57-66): 12 Bytes. */
#define SCA_ENTRY_SIZE    12
/* SCA-Zeiger steht im RDT-Header bei Offset 0x20 (rdt_common.c:249). */
#define RDT_OFF_SCA       0x20

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

/* Hilfsfunktion: uint32 LE in Puffer schreiben */
static void write_le32(uint8_t* p, uint32_t val)
{
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)((val >> 8) & 0xFF);
    p[2] = (uint8_t)((val >> 16) & 0xFF);
    p[3] = (uint8_t)((val >> 24) & 0xFF);
}

/*
 * Schreibt einen 12-Byte-SCA-Eintrag in das Layout von re15_sca_entry_t
 * (re15_rdt.h:57-66): width u16, density u16, x s16, z s16, type u8,
 * u0 u8, u1 u8, floor u8.
 */
static void write_sca_entry(uint8_t* p,
                            uint16_t width, uint16_t density,
                            int16_t x, int16_t z,
                            uint8_t type, uint8_t u0, uint8_t u1, uint8_t floor)
{
    write_le16(p + 0, width);
    write_le16(p + 2, density);
    write_le16s(p + 4, x);
    write_le16s(p + 6, z);
    p[8]  = type;
    p[9]  = u0;
    p[10] = u1;
    p[11] = floor;
}

/*
 * Baut einen minimalen RDT-Puffer mit einem SCA-Block direkt hinter dem
 * 0x60-Byte-Header. `entry_count` Eintraege werden in EINE Region (count[0])
 * gelegt; ceilingX/ceilingZ sind frei waehlbar. Der SCA-Zeiger (0x20) zeigt
 * auf RDT_HEADER_SIZE. Liefert die belegte Gesamtgroesse.
 *
 * `sca_pointer_override` != 0 ueberschreibt den geschriebenen SCA-Zeiger
 * (fuer "kein SCA"-Faelle 0). Bei -1 wird RDT_HEADER_SIZE verwendet.
 */
static int build_rdt_with_sca(uint8_t* buf, int buf_cap,
                              const uint8_t* entries, int entry_count,
                              uint16_t ceiling_x, uint16_t ceiling_z,
                              long sca_pointer_override)
{
    int sca_off    = RDT_HEADER_SIZE;
    int entry_bytes = entry_count * SCA_ENTRY_SIZE;
    int total      = sca_off + SCA_HEADER_SIZE + entry_bytes;
    if (total > buf_cap) return -1;

    memset(buf, 0, (size_t)total);

    /* SCA-Zeiger bei Offset 0x20. */
    uint32_t ptr = (sca_pointer_override < 0)
                       ? (uint32_t)sca_off
                       : (uint32_t)sca_pointer_override;
    write_le32(buf + RDT_OFF_SCA, ptr);

    /* SCA-Block-Header: ceilingX, ceilingZ, count[5]. */
    uint8_t* h = buf + sca_off;
    write_le16(h + 0, ceiling_x);
    write_le16(h + 2, ceiling_z);
    write_le32(h + 4,  (uint32_t)entry_count); /* count[0] = alle Eintraege */
    write_le32(h + 8,  0);
    write_le32(h + 12, 0);
    write_le32(h + 16, 0);
    write_le32(h + 20, 0);

    /* Eintraege. */
    if (entry_bytes > 0 && entries != NULL) {
        memcpy(buf + sca_off + SCA_HEADER_SIZE, entries, (size_t)entry_bytes);
    }

    return total;
}

static int test_parse_valid_entries(void)
{
    /*
     * Erstelle 3 Eintraege (12 B Layout):
     *   0: Rechteck (type=1, x=-500, z=300, width=200, density=100, floor=0)
     *   1: Kreis    (type=3, x=100,  z=-200, width=50, density=0,  floor=1)
     *   2: Treppe   (type=12, x=0,   z=0,   width=100, density=50, floor=2,
     *               u0=0x80 als Flag-Beispiel)
     */
    uint8_t entries[3 * SCA_ENTRY_SIZE];
    uint8_t buf[RDT_HEADER_SIZE + SCA_HEADER_SIZE + 3 * SCA_ENTRY_SIZE];
    re15_rdt_t rdt;
    int total, result;

    write_sca_entry(entries + 0 * SCA_ENTRY_SIZE,
                    200, 100, -500, 300, 1, 0x00, 0x00, 0);
    write_sca_entry(entries + 1 * SCA_ENTRY_SIZE,
                    50, 0, 100, -200, 3, 0x00, 0x00, 1);
    write_sca_entry(entries + 2 * SCA_ENTRY_SIZE,
                    100, 50, 0, 0, 12, 0x80, 0x00, 2);

    total = build_rdt_with_sca(buf, (int)sizeof(buf), entries, 3,
                               /*ceiling_x=*/0x1234, /*ceiling_z=*/0x5678, -1);
    if (total < 0) {
        fprintf(stderr, "FAIL: build_rdt_with_sca overflow\n");
        return 1;
    }

    result = re15_rdt_parse(buf, (size_t)total, &rdt);
    if (result != 0) {
        fprintf(stderr, "FAIL: re15_rdt_parse returned %d (expected 0)\n", result);
        return 1;
    }

    if (rdt.sca == NULL) {
        fprintf(stderr, "FAIL: rdt.sca is NULL\n");
        return 1;
    }
    if (rdt.sca_count != 3) {
        fprintf(stderr, "FAIL: sca_count=%d (expected 3)\n", rdt.sca_count);
        return 1;
    }
    if (rdt.ceiling_x != 0x1234 || rdt.ceiling_z != 0x5678) {
        fprintf(stderr, "FAIL: ceiling=(0x%X,0x%X) (expected 0x1234,0x5678)\n",
                rdt.ceiling_x, rdt.ceiling_z);
        return 1;
    }

    /* Pruefe Eintrag 0 (Rechteck) */
    if (rdt.sca[0].type != 1 ||
        rdt.sca[0].x != -500 ||
        rdt.sca[0].z != 300 ||
        rdt.sca[0].width != 200 ||
        rdt.sca[0].density != 100 ||
        rdt.sca[0].floor != 0) {
        fprintf(stderr, "FAIL: Eintrag 0 (Rechteck) Felder stimmen nicht\n");
        fprintf(stderr, "  type=%u x=%d z=%d width=%u density=%u floor=%u\n",
                rdt.sca[0].type, rdt.sca[0].x, rdt.sca[0].z,
                rdt.sca[0].width, rdt.sca[0].density, rdt.sca[0].floor);
        return 1;
    }

    /* Pruefe Eintrag 1 (Kreis) */
    if (rdt.sca[1].type != 3 ||
        rdt.sca[1].x != 100 ||
        rdt.sca[1].z != -200 ||
        rdt.sca[1].width != 50 ||
        rdt.sca[1].floor != 1) {
        fprintf(stderr, "FAIL: Eintrag 1 (Kreis) Felder stimmen nicht\n");
        return 1;
    }

    /* Pruefe Eintrag 2 (Treppe) */
    if (rdt.sca[2].type != 12 ||
        rdt.sca[2].width != 100 ||
        rdt.sca[2].density != 50 ||
        rdt.sca[2].floor != 2 ||
        rdt.sca[2].u0 != 0x80) {
        fprintf(stderr, "FAIL: Eintrag 2 (Treppe) Felder stimmen nicht\n");
        return 1;
    }

    printf("PASS: test_parse_valid_entries\n");
    return 0;
}

static int test_parse_empty(void)
{
    /* SCA-Block mit count[5]=0 -> total=0 -> sca bleibt NULL, sca_count=0. */
    uint8_t buf[RDT_HEADER_SIZE + SCA_HEADER_SIZE];
    re15_rdt_t rdt;
    int total, result;

    total = build_rdt_with_sca(buf, (int)sizeof(buf), NULL, 0, 0, 0, -1);
    if (total < 0) {
        fprintf(stderr, "FAIL: build_rdt_with_sca overflow (empty)\n");
        return 1;
    }

    result = re15_rdt_parse(buf, (size_t)total, &rdt);
    if (result != 0) {
        fprintf(stderr, "FAIL: empty parse returned %d\n", result);
        return 1;
    }
    if (rdt.sca_count != 0) {
        fprintf(stderr, "FAIL: empty parse sca_count=%d (expected 0)\n",
                rdt.sca_count);
        return 1;
    }
    if (rdt.sca != NULL) {
        fprintf(stderr, "FAIL: empty parse rdt.sca not NULL\n");
        return 1;
    }

    printf("PASS: test_parse_empty\n");
    return 0;
}

static int test_parse_no_sca_pointer(void)
{
    /* SCA-Zeiger (0x20) = 0 -> kein SCA-Block (rdt_common.c:250). */
    uint8_t buf[RDT_HEADER_SIZE];
    re15_rdt_t rdt;
    int result;

    memset(buf, 0, sizeof(buf));
    /* Offset 0x20 bleibt 0 -> kein SCA. */

    result = re15_rdt_parse(buf, sizeof(buf), &rdt);
    if (result != 0) {
        fprintf(stderr, "FAIL: no-sca parse returned %d\n", result);
        return 1;
    }
    if (rdt.sca != NULL || rdt.sca_count != 0) {
        fprintf(stderr, "FAIL: no-sca parse: sca=%p count=%d (expected NULL/0)\n",
                (const void*)rdt.sca, rdt.sca_count);
        return 1;
    }

    printf("PASS: test_parse_no_sca_pointer\n");
    return 0;
}

static int test_parse_null_input(void)
{
    re15_rdt_t rdt;
    uint8_t buf[RDT_HEADER_SIZE] = {0};
    int result;

    /* NULL data pointer -> -1 (rdt_common.c:210) */
    result = re15_rdt_parse(NULL, sizeof(buf), &rdt);
    if (result != -1) {
        fprintf(stderr, "FAIL: NULL data should return -1, got %d\n", result);
        return 1;
    }

    /* NULL output pointer -> -1 (rdt_common.c:210) */
    result = re15_rdt_parse(buf, sizeof(buf), NULL);
    if (result != -1) {
        fprintf(stderr, "FAIL: NULL out should return -1, got %d\n", result);
        return 1;
    }

    printf("PASS: test_parse_null_input\n");
    return 0;
}

static int test_parse_buffer_too_small(void)
{
    /* Puffer kleiner als der RDT-Header (0x60) -> -2 (rdt_common.c:211). */
    uint8_t buf[RDT_HEADER_SIZE - 1];
    re15_rdt_t rdt;
    int result;

    memset(buf, 0, sizeof(buf));

    result = re15_rdt_parse(buf, sizeof(buf), &rdt);
    if (result != -2) {
        fprintf(stderr, "FAIL: small buffer should return -2, got %d\n", result);
        return 1;
    }

    printf("PASS: test_parse_buffer_too_small\n");
    return 0;
}

static int test_parse_sca_truncated(void)
{
    /*
     * count[0] verspricht 2 Eintraege, aber der Puffer reicht nur fuer 1.
     * Erwartung (rdt_common.c:261): der Bounds-Check (total*12 passt nicht)
     * schlaegt fehl -> sca bleibt NULL, sca_count=0, ABER der Header (ceiling/
     * sca_rgn) wurde bereits gelesen und re15_rdt_parse liefert weiterhin 0.
     */
    uint8_t buf[RDT_HEADER_SIZE + SCA_HEADER_SIZE + 1 * SCA_ENTRY_SIZE];
    re15_rdt_t rdt;
    int result;
    int sca_off = RDT_HEADER_SIZE;

    memset(buf, 0, sizeof(buf));
    write_le32(buf + RDT_OFF_SCA, (uint32_t)sca_off);

    /* Header: ceiling=0, count[0]=2 (verspricht 2 Eintraege). */
    write_le16(buf + sca_off + 0, 0);
    write_le16(buf + sca_off + 2, 0);
    write_le32(buf + sca_off + 4, 2);  /* count[0]=2, aber nur Platz fuer 1 */

    /* Genau 1 Eintrag passt physisch -> 2. wuerde ueberlaufen. */
    write_sca_entry(buf + sca_off + SCA_HEADER_SIZE,
                    10, 10, 0, 0, 1, 0, 0, 0);

    result = re15_rdt_parse(buf, sizeof(buf), &rdt);
    if (result != 0) {
        fprintf(stderr, "FAIL: truncated SCA parse returned %d (expected 0)\n",
                result);
        return 1;
    }
    if (rdt.sca != NULL || rdt.sca_count != 0) {
        fprintf(stderr,
                "FAIL: truncated SCA should reject block: sca=%p count=%d\n",
                (const void*)rdt.sca, rdt.sca_count);
        return 1;
    }

    printf("PASS: test_parse_sca_truncated\n");
    return 0;
}

int main(void)
{
    int failures = 0;

    failures += test_parse_valid_entries();
    failures += test_parse_empty();
    failures += test_parse_no_sca_pointer();
    failures += test_parse_null_input();
    failures += test_parse_buffer_too_small();
    failures += test_parse_sca_truncated();

    if (failures == 0) {
        printf("\nALL SCA TESTS PASSED\n");
    } else {
        fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
