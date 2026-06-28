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
 *
 * --------------------------------------------------------------------------
 * API-MIGRATION (2026-06-28, Engine-Transplant):
 *
 * Der frühere Test manipulierte direkt einen internen `re15_aot_slot_t`, der
 * im Zuge des Engine-Transplants ENTFERNT wurde. Die Test-INTENTION bleibt
 * unverändert: für den SCD-Opcode `Door_aot_set_4p` (0x68, 40 Bytes) müssen
 * alle bedeutsamen Tür-Felder einen Parse→Speichern→Serialize-Roundtrip
 * unbeschadet überstehen.
 *
 * Statt des entfernten internen Structs nutzt der Test jetzt die ÖFFENTLICHE
 * Setter-API `re15_aot_set_door(...)` (re15_aot.h) und liest danach den
 * öffentlichen Laufzeit-Zustand `g_aot.slots[slot]` + `g_aot.door_params[slot]`
 * zurück. Felder, die der Setter NICHT befüllt (Quad-Ecken, dest_stage,
 * dest_room, floor-Band), werden über die öffentlichen Struct-Member von
 * `g_aot` gesetzt — das sind keine entfernten Internals, sondern Teil der
 * dokumentierten `re15_aot_state_t`.
 *
 * Alte API (entfernt)            ->  Neue öffentliche API (re15_aot.h)
 * -----------------------------------------------------------------------
 * re15_aot_slot_t                ->  re15_aot_t (g_aot.slots[]) +
 *                                    re15_aot_door_params_t (g_aot.door_params[])
 * AOT_TYPE_DOOR                  ->  RE15_AOT_TYPE_DOOR  (Wert weiterhin 1)
 * slot.active / slot.type        ->  re15_aot_set_door() setzt active=1,
 *                                    type=RE15_AOT_TYPE_DOOR (via re15_aot_set)
 * slot.floor_band               ->  g_aot.door_params[].floor (Door_aot_set pc[3])
 * slot.trigger_x[4]/trigger_z[4]->  g_aot.slots[].xs[4]/zs[4] (Quad-Ecken)
 * slot.data.door.dest_stage      ->  g_aot.door_params[].dest_stage
 * slot.data.door.dest_room       ->  g_aot.door_params[].dest_room
 * slot.data.door.dest_cut        ->  g_aot.door_params[].target_cut
 *                                    (kein eigenes dest_cut mehr; target_cut ist
 *                                     das nächstliegende Pendant — Ziel-Cut)
 * slot.data.door.spawn_x/y/z     ->  g_aot.door_params[].spawn_x/y/z (jetzt int32_t)
 * slot.data.door.spawn_rot       ->  g_aot.door_params[].spawn_yaw_4096
 *
 * RE15_AOT_MAX ist von 32 auf 64 gewachsen; der Slot-Index-Generator wird
 * unten passend angepasst.
 * --------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "re15_aot.h"

/* g_aot ist in aot_common.c definiert; hier nur Deklaration aus re15_aot.h
 * (extern re15_aot_state_t g_aot). Für reinen -fsyntax-only/Compile-Test
 * reicht die Deklaration; beim vollen Build linkt aot_common.c die Definition. */

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
 * Parse: Simuliert scd_op_door_aot_set_4p über die ÖFFENTLICHE Setter-API.
 *
 * Liest 40 Bytes und installiert die Tür im AOT-Slot via re15_aot_set_door();
 * Felder, die der Setter nicht abdeckt, werden über die öffentlichen
 * g_aot-Struct-Member gesetzt.
 * ========================================================================= */

static void door_parse(const uint8_t* p, int* out_slot_idx)
{
    int i;
    int slot = p[1];

    *out_slot_idx = slot;

    /* Tür-spezifische Daten aus dem 40-Byte-Layout. spawn_* sind im Layout
     * s16, passen daher problemlos in die (jetzt int32_t) Setter-Parameter. */
    uint8_t dest_stage   = p[20];
    uint8_t dest_room    = p[21];
    uint8_t target_cut   = p[22];           /* alt: dest_cut -> jetzt target_cut */
    int16_t spawn_x      = test_read_le16s(p + 24);
    int16_t spawn_y      = test_read_le16s(p + 26);
    int16_t spawn_z      = test_read_le16s(p + 28);
    int16_t spawn_rot    = test_read_le16s(p + 30);

    /* Öffentlicher Setter: setzt active=1, type=RE15_AOT_TYPE_DOOR, Rect
     * (cx/cz/half_w/half_h), target_cut + spawn-Daten in door_params[slot].
     * Das Rechteck (cx,cz,half_w,half_h) ist im 40-Byte-Layout nicht direkt
     * kodiert (das Layout trägt nur die 4 Quad-Ecken); wir setzen es auf 0,
     * die Quad-Ecken werden separat unten geschrieben. */
    re15_aot_set_door(slot,
                      /* cx */ 0, /* cz */ 0,
                      /* half_w */ 0, /* half_h */ 0,
                      target_cut,
                      (int32_t)spawn_x, (int32_t)spawn_y, (int32_t)spawn_z,
                      spawn_rot);

    /* Felder außerhalb der Setter-Signatur über öffentliche Member setzen. */
    re15_aot_door_params_t* d = &g_aot.door_params[slot];
    d->dest_stage = dest_stage;
    d->dest_room  = dest_room;
    d->floor      = p[3];   /* alt: slot.floor_band -> door_params.floor (pc[3]) */

    /* Trigger-Polygon: 4 X-Koordinaten (Offset 4..11) -> slots[].xs[4] */
    re15_aot_t* a = &g_aot.slots[slot];
    for (i = 0; i < 4; i++) {
        a->xs[i] = test_read_le16s(p + 4 + i * 2);
    }
    /* Trigger-Polygon: 4 Z-Koordinaten (Offset 12..19) -> slots[].zs[4] */
    for (i = 0; i < 4; i++) {
        a->zs[i] = test_read_le16s(p + 12 + i * 2);
    }
}

/* =========================================================================
 * Serialize: Liest den AOT-Slot aus g_aot zurück in 40 Bytes.
 *
 * Felder, die beim Parsen verloren gehen (pad, reserved),
 * werden als 0 geschrieben.
 * ========================================================================= */

static void door_serialize(int slot_idx, uint8_t* out)
{
    int i;
    const re15_aot_t* a            = &g_aot.slots[slot_idx];
    const re15_aot_door_params_t* d = &g_aot.door_params[slot_idx];

    memset(out, 0, DOOR_AOT_SET_4P_SIZE);

    out[0] = DOOR_AOT_SET_4P_OPCODE;
    out[1] = (uint8_t)slot_idx;
    out[2] = a->type;          /* Wird vom Setter als RE15_AOT_TYPE_DOOR gesetzt */
    out[3] = d->floor;

    /* Trigger-Polygon X (Offset 4..11) <- slots[].xs[4] */
    for (i = 0; i < 4; i++) {
        test_write_le16s(out + 4 + i * 2, a->xs[i]);
    }

    /* Trigger-Polygon Z (Offset 12..19) <- slots[].zs[4] */
    for (i = 0; i < 4; i++) {
        test_write_le16s(out + 12 + i * 2, a->zs[i]);
    }

    /* Door-Daten */
    out[20] = d->dest_stage;
    out[21] = d->dest_room;
    out[22] = d->target_cut;   /* alt: dest_cut */
    /* out[23] = pad — nicht im Slot, bleibt 0 */

    /* Spawn-Position (int32_t -> s16, Werte stammen aus s16 und passen exakt) */
    test_write_le16s(out + 24, (int16_t)d->spawn_x);
    test_write_le16s(out + 26, (int16_t)d->spawn_y);
    test_write_le16s(out + 28, (int16_t)d->spawn_z);
    test_write_le16s(out + 30, d->spawn_yaw_4096);

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
 * byte[0] = 0x68, byte[1] wird auf [0..RE15_AOT_MAX-1] begrenzt (gültiger
 * Slot-Index). Alle anderen Bytes sind zufällig.
 */
static void generate_random_door_bytes(uint8_t* buf)
{
    int i;

    buf[0] = DOOR_AOT_SET_4P_OPCODE;
    /* Slot-Index 0..RE15_AOT_MAX-1 (RE15_AOT_MAX ist 64 = Zweierpotenz). */
    buf[1] = (uint8_t)(rand_byte() % RE15_AOT_MAX);

    for (i = 2; i < DOOR_AOT_SET_4P_SIZE; i++) {
        buf[i] = rand_byte();
    }
}

/* =========================================================================
 * Vergleichs-Hilfsfunktion: Nur bedeutsame Felder vergleichen
 *
 * Ignoriert:
 *   - Offset 2  (type): beim Parsen wird immer RE15_AOT_TYPE_DOOR gesetzt,
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

    /* Byte 2: Type — Parser setzt immer RE15_AOT_TYPE_DOOR, daher prüfen wir
     * dass die Ausgabe RE15_AOT_TYPE_DOOR (1) enthält (unabhängig von der Eingabe) */
    if (output[2] != RE15_AOT_TYPE_DOOR) { *mismatch_offset = 2; return 1; }

    /* Byte 3: Floor-Band */
    if (input[3] != output[3]) { *mismatch_offset = 3; return 1; }

    /* Bytes 4..11: xs[4] (Quad-X, 4 × s16 LE) */
    for (i = 4; i < 12; i++) {
        if (input[i] != output[i]) { *mismatch_offset = i; return 1; }
    }

    /* Bytes 12..19: zs[4] (Quad-Z, 4 × s16 LE) */
    for (i = 12; i < 20; i++) {
        if (input[i] != output[i]) { *mismatch_offset = i; return 1; }
    }

    /* Bytes 20..21: dest_stage, dest_room */
    if (input[20] != output[20]) { *mismatch_offset = 20; return 1; }
    if (input[21] != output[21]) { *mismatch_offset = 21; return 1; }

    /* Byte 22: target_cut (vormals dest_cut, jetzt im door_params gespeichert) */
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
    int slot_idx;
    int mismatch_offset;
    int i;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        generate_random_door_bytes(input);

        /* Parse → installiert die Tür im öffentlichen g_aot-Zustand */
        door_parse(input, &slot_idx);

        /* Serialize → liest aus g_aot zurück */
        door_serialize(slot_idx, output);

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
 * - Byte 2 = RE15_AOT_TYPE_DOOR (1)
 * - Bytes 23, 32..39 = 0 (nicht gespeicherte Felder)
 * ========================================================================= */

static int prop_serialize_invariants(void)
{
    uint8_t input[DOOR_AOT_SET_4P_SIZE];
    uint8_t output[DOOR_AOT_SET_4P_SIZE];
    int slot_idx;
    int i, j;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        generate_random_door_bytes(input);
        door_parse(input, &slot_idx);
        door_serialize(slot_idx, output);

        PROP_ASSERT(output[0] == DOOR_AOT_SET_4P_OPCODE,
            "Iteration %d: output[0]=0x%02X (expected 0x68)", i, output[0]);

        PROP_ASSERT(output[2] == RE15_AOT_TYPE_DOOR,
            "Iteration %d: output[2]=0x%02X (expected RE15_AOT_TYPE_DOOR=%d)",
            i, output[2], RE15_AOT_TYPE_DOOR);

        /* target_cut (vormals dest_cut) wird im door_params gespeichert — Roundtrip prüfen */
        PROP_ASSERT(output[22] == input[22],
            "Iteration %d: output[22]=0x%02X (expected 0x%02X, target_cut roundtrip)",
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

    /* AOT-Subsystem in einen sauberen Anfangszustand bringen. */
    re15_aot_init();

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
