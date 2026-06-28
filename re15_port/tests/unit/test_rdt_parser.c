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
 *
 * ---------------------------------------------------------------------------
 * API-MIGRATION (Engine-Transplant, re15_rdt.h / rdt_common.c, 2026-06):
 * ---------------------------------------------------------------------------
 * Der frühere Parser exportierte eine UNIFORME 21-Einträge-Adresstabelle, bei
 * der jeder Offset 0x08..0x5C 1:1 auf ein flaches struct-Pointer-Feld abbildete
 * (snd0_edt … animation) und die Konstante RE15_RDT_SECTION_COUNT (=21) sowie
 * die re15_error.h-Codes RE15_IO_OK / RE15_IO_READ_ERROR / RE15_IO_INVALID_ARG
 * definierte. Diese sind im aktuellen Engine-Stand ALLE ENTFERNT.
 *
 * Aktuelle öffentliche API (re15_rdt.h / rdt_common.c):
 *   int re15_rdt_parse(const uint8_t *data, size_t size, re15_rdt_t *out);
 *     - Rückgabe  0  bei Erfolg                       (rdt_common.c:342)
 *     - Rückgabe -1  bei NULL data ODER NULL out      (rdt_common.c:210)
 *     - Rückgabe -2  bei size < 0x60                   (rdt_common.c:211)
 *   re15_rdt_t hat KEINE flache 21:1-Pointer-Tabelle mehr, sondern eine
 *   sektions-SPEZIFISCH semantisch geparste Struktur (cuts, zones[], sca,
 *   main_scd, sub_scd[], lights, messages, props, snd_*, flr, …).
 *
 * Erhaltene Test-INTENTION: die ursprüngliche „Null-Offset → NULL /
 * gültiger Offset → buffer+offset / OOB-Offset → NULL"-Garantie ist im
 * aktuellen Parser EXAKT erhalten für die 7 Sektionen, die immer noch als
 * reines `data + offset` aliasen:
 *   snd_edt[0/1], snd_vh[0/1], snd_vb[0/1]  (RDT-Offsets 0x08..0x1c)
 *       Quelle: parse_audio() — `out->snd_*[b] = data + off`
 *               GATE: `off != 0 && (size_t)off < size`   (rdt_common.c:92-102)
 *   flr  (RDT-Offset 0x34)
 *       Quelle: re15_rdt_parse — `out->flr = data + flr_start`
 *               GATE: `flr_start != 0 && (size_t)flr_start + 2 <= size`
 *                                                          (rdt_common.c:320-323)
 * Alle 5 Tests sind auf diese 7 öffentlichen Pointer + die Rückgabe-Codes
 * umgestellt; KEIN _legacy_minimal-Header wird eingebunden.
 *
 * Diese Datei spiegelt das bereits reaktivierte Property-Pendant
 * tests/property/prop_rdt_null_offset.c (gleiche 7 Probes, gleiche Gates).
 */

#include "re15_rdt.h"

#include <stdint.h>
#include <stddef.h>
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

/* -------------------------------------------------------------------------
 * Die 7 Sektionen, deren "Null-Offset → NULL / gültiger Offset → buffer+offset
 * / OOB-Offset → NULL"-Property der aktuelle Parser exakt erhält. `rdt_off` =
 * Byte-Offset des u32 in der RDT-Adresstabelle (0x08..0x5c). Die Pointer liegen
 * in den snd_*-Arrays bzw. dem flr-Skalar; collect_sections() liest sie in der
 * gleichen Reihenfolge wie SECTIONS[] aus dem geparsten re15_rdt_t.
 * ------------------------------------------------------------------------- */
#define NUM_SECTIONS 7

static const uint32_t SECTION_RDT_OFF[NUM_SECTIONS] = {
    0x08, /* snd_edt[0] */
    0x0c, /* snd_vh[0]  */
    0x10, /* snd_vb[0]  */
    0x14, /* snd_edt[1] */
    0x18, /* snd_vh[1]  */
    0x1c, /* snd_vb[1]  */
    0x34, /* flr        */
};

static const char *SECTION_NAME[NUM_SECTIONS] = {
    "snd_edt[0]", "snd_vh[0]", "snd_vb[0]",
    "snd_edt[1]", "snd_vh[1]", "snd_vb[1]", "flr",
};

/* Liest die 7 geprobten Pointer aus dem geparsten rdt in derselben Reihenfolge
 * wie SECTION_RDT_OFF[]. */
static void collect_sections(const re15_rdt_t *rdt, const uint8_t *out[NUM_SECTIONS])
{
    out[0] = rdt->snd_edt[0];
    out[1] = rdt->snd_vh[0];
    out[2] = rdt->snd_vb[0];
    out[3] = rdt->snd_edt[1];
    out[4] = rdt->snd_vh[1];
    out[5] = rdt->snd_vb[1];
    out[6] = rdt->flr;
}

/* Setzt einen sauberen 0x60-Header: alle Header-Zähler und die GESAMTE
 * Adresstabelle (0x08..0x5c) auf 0, damit count-gegatete bzw. nicht geprobte
 * Sektionen (cuts/sca/light/scd/props) den Test nicht stören. */
static void init_empty_header(uint8_t *buf)
{
    int i;
    for (i = 0x00; i < 0x08; i++) buf[i] = 0x00;          /* Zähler */
    for (i = 0x08; i <= 0x5c; i += 4) write_le32(buf + i, 0x00000000);
}

/* =========================================================================
 * Test 1: Gueltige Offsets — die 7 buffer+offset-erhaltenden Sektionen
 *         zeigen korrekt in den Puffer (data + offset).
 * ========================================================================= */

static int test_rdt_parse_valid(void)
{
    /*
     * Synthetischer RDT-Puffer:
     * - 0x60 Header (Zähler 0, Adresstabelle 0)
     * - Datenbereich ab 0x60; jede der 7 geprobten Sektionen bekommt einen
     *   gültigen Offset 0x60 + i*16 (strikt < size, weit innerhalb).
     * Gesamtgroesse: 0x60 + 7*16 = 208 Bytes.
     */
    uint8_t buf[0x60 + NUM_SECTIONS * 16];
    re15_rdt_t rdt;
    const uint8_t *sections[NUM_SECTIONS];
    int result;
    int i;

    memset(buf, 0xAB, sizeof(buf)); /* Fuellmuster */
    init_empty_header(buf);

    /* Header-Magie nur als Marker (vom Parser ignoriert) */
    buf[0] = 'R'; buf[1] = 'D'; buf[2] = 'T'; buf[3] = '0';

    /* Jede geprobte Sektion bekommt einen gültigen Offset in den Datenbereich */
    for (i = 0; i < NUM_SECTIONS; i++) {
        uint32_t offset = 0x60 + (uint32_t)(i * 16);
        write_le32(buf + SECTION_RDT_OFF[i], offset);
    }

    result = re15_rdt_parse(buf, sizeof(buf), &rdt);
    if (result != 0) {
        fprintf(stderr, "FAIL: re15_rdt_parse returned %d (expected 0)\n", result);
        return 1;
    }

    /* Alle 7 Pointer muessen korrekt in den Puffer zeigen (data + offset) */
    collect_sections(&rdt, sections);
    for (i = 0; i < NUM_SECTIONS; i++) {
        const uint8_t* expected_ptr = buf + 0x60 + i * 16;
        if (sections[i] != expected_ptr) {
            fprintf(stderr, "FAIL: Sektion %d (%s): Pointer mismatch\n",
                    i, SECTION_NAME[i]);
            fprintf(stderr, "  expected: %p, actual: %p\n",
                    (void*)expected_ptr, (void*)sections[i]);
            return 1;
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
     * Puffer mit gemischten Null-/Gueltig-Offsets.
     * Sektionen 0, 3, 6 haben Offset=0 → Pointer muss NULL sein.
     * Alle anderen haben gueltige Offsets → Pointer muss non-NULL sein.
     */
    uint8_t buf[0x60 + NUM_SECTIONS * 16];
    re15_rdt_t rdt;
    const uint8_t *sections[NUM_SECTIONS];
    int result;
    int i;

    memset(buf, 0xCC, sizeof(buf));
    init_empty_header(buf);

    buf[0] = 'R'; buf[1] = 'D'; buf[2] = 'T'; buf[3] = '0';

    for (i = 0; i < NUM_SECTIONS; i++) {
        if (i == 0 || i == 3 || i == 6) {
            /* Null-Offset: Sektion nicht vorhanden */
            write_le32(buf + SECTION_RDT_OFF[i], 0x00000000);
        } else {
            /* Gueltiger Offset */
            uint32_t offset = 0x60 + (uint32_t)(i * 16);
            write_le32(buf + SECTION_RDT_OFF[i], offset);
        }
    }

    result = re15_rdt_parse(buf, sizeof(buf), &rdt);
    if (result != 0) {
        fprintf(stderr, "FAIL: re15_rdt_parse returned %d (expected 0)\n", result);
        return 1;
    }

    collect_sections(&rdt, sections);
    for (i = 0; i < NUM_SECTIONS; i++) {
        if (i == 0 || i == 3 || i == 6) {
            /* Muss NULL sein */
            if (sections[i] != NULL) {
                fprintf(stderr, "FAIL: Sektion %d (%s) sollte NULL sein, ist %p\n",
                        i, SECTION_NAME[i], (void*)sections[i]);
                return 1;
            }
        } else {
            /* Muss non-NULL sein */
            if (sections[i] == NULL) {
                fprintf(stderr, "FAIL: Sektion %d (%s) sollte non-NULL sein\n",
                        i, SECTION_NAME[i]);
                return 1;
            }
        }
    }

    printf("PASS: test_rdt_parse_null_offsets\n");
    return 0;
}

/* =========================================================================
 * Test 3: Puffer zu klein (<0x60 Bytes) → Fehler (negativer Code)
 *
 * Aktuell: re15_rdt_parse gibt -2 zurueck wenn size < 0x60 (rdt_common.c:211).
 * (Frueher RE15_IO_READ_ERROR == -2 — Wert + Vorzeichen identisch.)
 * ========================================================================= */

static int test_rdt_parse_buffer_too_small(void)
{
    uint8_t buf[0x60]; /* gross genug; wir uebergeben absichtlich kleinere size */
    re15_rdt_t rdt;
    int result;

    memset(buf, 0, sizeof(buf));

    /* size = 0x50 < 0x60 */
    result = re15_rdt_parse(buf, 0x50, &rdt);
    if (result >= 0) {
        fprintf(stderr, "FAIL: small buffer (0x50) should return negative, got %d\n",
                result);
        return 1;
    }

    /* Auch exakt 0x5F (knapp unter Minimum) */
    result = re15_rdt_parse(buf, 0x5F, &rdt);
    if (result >= 0) {
        fprintf(stderr, "FAIL: buffer 0x5F should return negative, got %d\n", result);
        return 1;
    }

    /* Genau 0x60 (Minimum) ist gueltig → 0 */
    result = re15_rdt_parse(buf, 0x60, &rdt);
    if (result != 0) {
        fprintf(stderr, "FAIL: buffer 0x60 (min) should return 0, got %d\n", result);
        return 1;
    }

    printf("PASS: test_rdt_parse_buffer_too_small\n");
    return 0;
}

/* =========================================================================
 * Test 4: NULL-Argumente → Fehler (negativer Code)
 *
 * Aktuell: re15_rdt_parse gibt -1 zurueck wenn data==NULL || out==NULL
 * (rdt_common.c:210). (Frueher RE15_IO_INVALID_ARG == -4.)
 * ========================================================================= */

static int test_rdt_parse_null_args(void)
{
    uint8_t buf[0x60];
    re15_rdt_t rdt;
    int result;

    memset(buf, 0, sizeof(buf));

    /* NULL buffer */
    result = re15_rdt_parse(NULL, 0x60, &rdt);
    if (result >= 0) {
        fprintf(stderr, "FAIL: NULL buffer should return negative, got %d\n", result);
        return 1;
    }

    /* NULL output */
    result = re15_rdt_parse(buf, 0x60, NULL);
    if (result >= 0) {
        fprintf(stderr, "FAIL: NULL output should return negative, got %d\n", result);
        return 1;
    }

    /* Both NULL */
    result = re15_rdt_parse(NULL, 0x60, NULL);
    if (result >= 0) {
        fprintf(stderr, "FAIL: both NULL should return negative, got %d\n", result);
        return 1;
    }

    printf("PASS: test_rdt_parse_null_args\n");
    return 0;
}

/* =========================================================================
 * Test 5: Out-of-Bounds-Offset → NULL-Pointer (als fehlend behandelt)
 *
 * Der semantische Parser gated jede der 7 geprobten Sektionen gegen die
 * Puffergroesse:
 *   Audio (snd_*):  Offset gueltig gdw. `off != 0 && off <  size`  (strikt)
 *   FLR:            Offset gueltig gdw. `off != 0 && off + 2 <= size`
 * Out-of-Bounds-Offsets muessen also NULL liefern, ohne Absturz.
 * ========================================================================= */

static int test_rdt_parse_out_of_bounds_offset(void)
{
    uint8_t buf[0x80]; /* 128 Bytes */
    re15_rdt_t rdt;
    int result;

    memset(buf, 0xDD, sizeof(buf));
    init_empty_header(buf);

    buf[0] = 'R'; buf[1] = 'D'; buf[2] = 'T'; buf[3] = '0';

    /* snd_edt[0] (0x08): Offset weit jenseits des Puffers → NULL */
    write_le32(buf + 0x08, 0x00010000); /* 64KB — weit out-of-bounds */

    /* snd_vh[0] (0x0c): Offset genau == size (audio-Gate ist strikt off<size) → NULL */
    write_le32(buf + 0x0c, 0x80); /* == sizeof(buf) */

    /* snd_vb[0] (0x10): Gueltiger Offset (innerhalb) → non-NULL, = buf+0x60 */
    write_le32(buf + 0x10, 0x60);

    /* snd_edt[1] (0x14): Null-Offset → NULL */
    write_le32(buf + 0x14, 0x00000000);

    /* snd_vh[1] (0x18): Offset knapp innerhalb (size-1) → non-NULL, = buf+0x7F */
    write_le32(buf + 0x18, 0x7F);

    /* snd_vb[1] (0x1c) + flr (0x34): von init_empty_header bereits 0 → NULL */

    result = re15_rdt_parse(buf, sizeof(buf), &rdt);
    if (result != 0) {
        fprintf(stderr, "FAIL: re15_rdt_parse returned %d (expected 0)\n", result);
        return 1;
    }

    /* snd_edt[0]: Out-of-bounds → NULL */
    if (rdt.snd_edt[0] != NULL) {
        fprintf(stderr, "FAIL: snd_edt[0] (OOB 0x10000) sollte NULL sein, ist %p\n",
                (void*)rdt.snd_edt[0]);
        return 1;
    }

    /* snd_vh[0]: Offset == size → out-of-bounds (strikt) → NULL */
    if (rdt.snd_vh[0] != NULL) {
        fprintf(stderr, "FAIL: snd_vh[0] (offset==size) sollte NULL sein, ist %p\n",
                (void*)rdt.snd_vh[0]);
        return 1;
    }

    /* snd_vb[0]: Gueltiger Offset → non-NULL, zeigt auf buf+0x60 */
    if (rdt.snd_vb[0] != buf + 0x60) {
        fprintf(stderr, "FAIL: snd_vb[0] sollte buf+0x60 sein\n");
        fprintf(stderr, "  expected: %p, actual: %p\n",
                (void*)(buf + 0x60), (void*)rdt.snd_vb[0]);
        return 1;
    }

    /* snd_edt[1]: Null-Offset → NULL */
    if (rdt.snd_edt[1] != NULL) {
        fprintf(stderr, "FAIL: snd_edt[1] (null offset) sollte NULL sein\n");
        return 1;
    }

    /* snd_vh[1]: Offset 0x7F (innerhalb, < size) → non-NULL, buf+0x7F */
    if (rdt.snd_vh[1] != buf + 0x7F) {
        fprintf(stderr, "FAIL: snd_vh[1] sollte buf+0x7F sein\n");
        fprintf(stderr, "  expected: %p, actual: %p\n",
                (void*)(buf + 0x7F), (void*)rdt.snd_vh[1]);
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
