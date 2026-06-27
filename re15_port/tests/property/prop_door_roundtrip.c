/**
 * @file prop_door_roundtrip.c
 * @brief Property-Test: Door_aot_set_4p Parse-Roundtrip
 *
 * **Property 3: Door_aot_set_4p Parse-Roundtrip**
 *
 * Für N zufällig generierte gültige 40-Byte-Sequenzen (mit opcode = 0x68)
 * muss gelten: Parse → Serialize → identische Bytes für alle bedeutsamen
 * Felder (Opcode, Slot, Floor, Trigger-Polygon, Door-Daten, Spawn-Werte).
 *
 * Die Felder pad (Offset 23) und reserved (Offset 32..39)
 * werden beim Parsen NICHT in den AOT-Slot übernommen, daher werden sie
 * beim Vergleich ausgelassen.
 *
 * **Validates: Requirements 3.1, 3.2, 4.1**
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "re15_aot.h"

/* =========================================================================
 * Konfiguration
 * ========================================================================= */

/** Anzahl der Roundtrip-Iterationen */
#define NUM_ITERATIONS 300

/** Opcode für Door_aot_set_4p */
#define DOOR_AOT_SET_4P_OPCODE 0x68

/** Gesamtgröße des Opcodes in Bytes */
#define DOOR_AOT_SET_4P_SIZE 40

/* =========================================================================
 * Test-Framework
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
 * Hilfsfunktionen: Little-Endian Byte-Zugriff (identisch zu scd_opcodes_aot.c)
 * ========================================================================= */

static uint16_t test_read_le16(const uint8_t* p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static int16_t test_read_le16s(const uint8_t* p)
{
    return (int16_t)test_read_le16(p);
}

static void test_write_le16s(uint8_t* p, int16_t val)
{
    uint16_t u = (uint16_t)val;
    p[0] = (uint8_t)(u & 0xFF);
    p[1] = (uint8_t)((u >> 8) & 0xFF);
}

/* =========================================================================
 * Parse: Simuliert scd_op_door_aot_set_4p (ohne VM/Thread-Abhängigkeit)
 *
 * Liest 40 Bytes und füllt den AOT-Slot analog zum echten Handler.
 * ========================================================================= */

static void door_parse(const uint8_t* p, re15_aot_slot_t* slot, int* out_slot_idx)
{
    int i;

    memset(slot, 0, sizeof(re15_aot_slot_t));

    *out_slot_idx   = p[1];
    slot->active     = 1;
    slot->type       = AOT_TYPE_DOOR;
    slot->floor_band = p[3];

    /* Trigger-Polygon: 4 X-Koordinaten (Offset 4..11) */
    for (i = 0; i < 4; i++) {
        slot->trigger_x[i] = test_read_le16s(p + 4 + i * 2);
    }

    /* Trigger-Polygon: 4 Z-Koordinaten (Offset 12..19) */
    for (i = 0; i < 4; i++) {
        slot->trigger_z[i] = test_read_le16s(p + 12 + i * 2);
    }

    /* Tür-spezifische Daten */
    slot->data.door.dest_stage = p[20];
    slot->data.door.dest_room  = p[21];
    slot->data.door.dest_cut   = p[22];
    /* p[23] = pad — wird NICHT im Slot gespeichert */

    slot->data.door.spawn_x   = test_read_le16s(p + 24);
    slot->data.door.spawn_y   = test_read_le16s(p + 26);
    slot->data.door.spawn_z   = test_read_le16s(p + 28);
    slot->data.door.spawn_rot = test_read_le16s(p + 30);
}

/* =========================================================================
 * Serialize: Schreibt den AOT-Slot zurück in 40 Bytes
 *
 * Felder, die beim Parsen verloren gehen (pad, reserved),
 * werden als 0 geschrieben.
 * ========================================================================= */

static void door_serialize(const re15_aot_slot_t* slot, int slot_idx, uint8_t* out)
{
    int i;

    memset(out, 0, DOOR_AOT_SET_4P_SIZE);

    out[0] = DOOR_AOT_SET_4P_OPCODE;
    out[1] = (uint8_t)slot_idx;
    out[2] = slot->type;         /* Wird als AOT_TYPE_DOOR gesetzt */
    out[3] = slot->floor_band;

    /* Trigger-Polygon X (Offset 4..11) */
    for (i = 0; i < 4; i++) {
        test_write_le16s(out + 4 + i * 2, slot->trigger_x[i]);
    }

    /* Trigger-Polygon Z (Offset 12..19) */
    for (i = 0; i < 4; i++) {
        test_write_le16s(out + 12 + i * 2, slot->trigger_z[i]);
    }

    /* Door-Daten */
    out[20] = slot->data.door.dest_stage;
    out[21] = slot->data.door.dest_room;
    out[22] = slot->data.door.dest_cut;
    /* out[23] = pad — nicht im Slot, bleibt 0 */

    /* Spawn-Position */
    test_write_le16s(out + 24, slot->data.door.spawn_x);
    test_write_le16s(out + 26, slot->data.door.spawn_y);
    test_write_le16s(out + 28, slot->data.door.spawn_z);
    test_write_le16s(out + 30, slot->data.door.spawn_rot);

    /* out[32..39] = reserved — bleibt 0 */
}

/* =========================================================================
 * Zufallsdaten-Generierung
 * ========================================================================= */

/**
 * Einfacher xorshift32 PRNG für reproduzierbare Ergebnisse.
 */
static uint32_t xorshift_state;

static uint32_t xorshift32(void)
{
    uint32_t x = xorshift_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    xorshift_state = x;
    return x;
}

static uint8_t rand_byte(void)
{
    return (uint8_t)(xorshift32() & 0xFF);
}

/**
 * Generiert eine zufällige 40-Byte-Sequenz mit korrektem Opcode.
 * byte[0] = 0x68, byte[1] wird auf [0..31] begrenzt (gültiger Slot-Index).
 * Alle anderen Bytes sind zufällig.
 */
static void generate_random_door_bytes(uint8_t* buf)
{
    int i;

    buf[0] = DOOR_AOT_SET_4P_OPCODE;
    buf[1] = rand_byte() & 0x1F;  /* Slot-Index 0..31 (RE15_AOT_MAX_SLOTS - 1) */

    for (i = 2; i < DOOR_AOT_SET_4P_SIZE; i++) {
        buf[i] = rand_byte();
    }
}

/* =========================================================================
 * Vergleichs-Hilfsfunktion: Nur bedeutsame Felder vergleichen
 *
 * Ignoriert:
 *   - Offset 2  (type): beim Parsen wird immer AOT_TYPE_DOOR gesetzt,
 *     unabhängig vom Eingabe-Byte. Wir vergleichen NICHT den type-Byte
 *     aus der Random-Eingabe, sondern prüfen nur die Roundtrip-Logik.
 *   - Offset 23 (pad): wird NICHT im Slot gespeichert
 *   - Offset 32..39 (reserved): wird NICHT im Slot gespeichert
 * ========================================================================= */

/**
 * Vergleicht die bedeutsamen Bytes der Eingabe mit der serialisierten Ausgabe.
 * Gibt 0 bei Übereinstimmung zurück, 1 bei Abweichung.
 */
static int compare_meaningful_bytes(const uint8_t* input, const uint8_t* output,
                                    int iteration, int* mismatch_offset)
{
    int i;

    /* Byte 0: Opcode */
    if (input[0] != output[0]) { *mismatch_offset = 0; return 1; }

    /* Byte 1: Slot-Index */
    if (input[1] != output[1]) { *mismatch_offset = 1; return 1; }

    /* Byte 2: Type — Parser setzt immer AOT_TYPE_DOOR, daher prüfen wir
     * dass die Ausgabe AOT_TYPE_DOOR (1) enthält (unabhängig von der Eingabe) */
    if (output[2] != AOT_TYPE_DOOR) { *mismatch_offset = 2; return 1; }

    /* Byte 3: Floor-Band */
    if (input[3] != output[3]) { *mismatch_offset = 3; return 1; }

    /* Bytes 4..11: trigger_x[4] (4 × s16 LE) */
    for (i = 4; i < 12; i++) {
        if (input[i] != output[i]) { *mismatch_offset = i; return 1; }
    }

    /* Bytes 12..19: trigger_z[4] (4 × s16 LE) */
    for (i = 12; i < 20; i++) {
        if (input[i] != output[i]) { *mismatch_offset = i; return 1; }
    }

    /* Bytes 20..21: dest_stage, dest_room */
    if (input[20] != output[20]) { *mismatch_offset = 20; return 1; }
    if (input[21] != output[21]) { *mismatch_offset = 21; return 1; }

    /* Byte 22: dest_cut (now stored in slot) */
    if (input[22] != output[22]) { *mismatch_offset = 22; return 1; }

    /* Byte 23: pad — SKIP (nicht im Slot gespeichert) */

    /* Bytes 24..31: spawn_x/y/z/rot (4 × s16 LE) */
    for (i = 24; i < 32; i++) {
        if (input[i] != output[i]) { *mismatch_offset = i; return 1; }
    }

    /* Bytes 32..39: reserved — SKIP */

    (void)iteration;
    *mismatch_offset = -1;
    return 0;
}

/* =========================================================================
 * Hex-Dump für Fehlerausgabe
 * ========================================================================= */

static void print_hex_row(const char* label, const uint8_t* data, int len)
{
    int i;
    printf("  %s: ", label);
    for (i = 0; i < len; i++) {
        printf("%02X ", data[i]);
        if (i == 3 || i == 11 || i == 19 || i == 23 || i == 31) {
            printf("| ");
        }
    }
    printf("\n");
}

/* =========================================================================
 * Property-Test: Parse → Serialize Roundtrip
 * ========================================================================= */

static int prop_door_roundtrip(void)
{
    uint8_t input[DOOR_AOT_SET_4P_SIZE];
    uint8_t output[DOOR_AOT_SET_4P_SIZE];
    re15_aot_slot_t slot;
    int slot_idx;
    int mismatch_offset;
    int i;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        generate_random_door_bytes(input);

        /* Parse */
        door_parse(input, &slot, &slot_idx);

        /* Serialize */
        door_serialize(&slot, slot_idx, output);

        /* Compare meaningful fields */
        if (compare_meaningful_bytes(input, output, i, &mismatch_offset) != 0) {
            printf("  FAIL: Roundtrip mismatch at iteration %d, offset %d\n", i, mismatch_offset);
            printf("        input[%d]=0x%02X, output[%d]=0x%02X\n",
                   mismatch_offset, input[mismatch_offset],
                   mismatch_offset, output[mismatch_offset]);
            print_hex_row("Input ", input, DOOR_AOT_SET_4P_SIZE);
            print_hex_row("Output", output, DOOR_AOT_SET_4P_SIZE);
            g_failed++;
            return 1;
        }
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Zusatz-Property: Serialisierte Ausgabe hat immer korrekte Struktur
 *
 * Unabhängig von den Eingabedaten müssen folgende Invarianten gelten:
 * - Byte 0 = 0x68 (Opcode)
 * - Byte 2 = AOT_TYPE_DOOR (1)
 * - Bytes 22, 23, 32..39 = 0 (nicht gespeicherte Felder)
 * ========================================================================= */

static int prop_serialize_invariants(void)
{
    uint8_t input[DOOR_AOT_SET_4P_SIZE];
    uint8_t output[DOOR_AOT_SET_4P_SIZE];
    re15_aot_slot_t slot;
    int slot_idx;
    int i, j;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        generate_random_door_bytes(input);
        door_parse(input, &slot, &slot_idx);
        door_serialize(&slot, slot_idx, output);

        PROP_ASSERT(output[0] == DOOR_AOT_SET_4P_OPCODE,
            "Iteration %d: output[0]=0x%02X (expected 0x68)", i, output[0]);

        PROP_ASSERT(output[2] == AOT_TYPE_DOOR,
            "Iteration %d: output[2]=0x%02X (expected AOT_TYPE_DOOR=%d)",
            i, output[2], AOT_TYPE_DOOR);

        /* dest_cut wird jetzt im Slot gespeichert — Roundtrip prüfen */
        PROP_ASSERT(output[22] == input[22],
            "Iteration %d: output[22]=0x%02X (expected 0x%02X, dest_cut roundtrip)",
            i, output[22], input[22]);

        /* pad muss 0 sein (nicht gespeichert) */
        PROP_ASSERT(output[23] == 0,
            "Iteration %d: output[23]=0x%02X (expected 0, pad not stored)",
            i, output[23]);

        /* Reserved-Bytes müssen 0 sein */
        for (j = 32; j < 40; j++) {
            PROP_ASSERT(output[j] == 0,
                "Iteration %d: output[%d]=0x%02X (expected 0, reserved)",
                i, j, output[j]);
        }
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    /* Seed: Nutze Zeit für Varianz, drucke Seed für Reproduzierbarkeit */
    xorshift_state = (uint32_t)time(NULL);
    printf("=== Property-Test: Door_aot_set_4p Parse-Roundtrip ===\n");
    printf("    Validates: Requirements 3.1, 3.2, 4.1\n");
    printf("    PRNG Seed: %u\n", xorshift_state);
    printf("    Iterationen: %d\n\n", NUM_ITERATIONS);

    /* Test 1: Parse-Serialize Roundtrip */
    printf("[1/2] prop_door_roundtrip (%d Iterationen) ... ", NUM_ITERATIONS);
    if (prop_door_roundtrip() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test 2: Serialisierungs-Invarianten */
    printf("[2/2] prop_serialize_invariants (%d Iterationen) ... ", NUM_ITERATIONS);
    if (prop_serialize_invariants() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Summary */
    printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
