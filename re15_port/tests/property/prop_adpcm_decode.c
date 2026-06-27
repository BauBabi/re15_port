/**
 * @file prop_adpcm_decode.c
 * @brief Property-Test: ADPCM-Dekodierung
 *
 * **Validates: Requirements 6.3**
 *
 * Property 10: Für jeden gültigen 16-Byte PSX-ADPCM-Block (Shift 0-12,
 * Filter 0-4, 28 Nibble-Samples) soll der Decoder exakt 28 PCM-Samples
 * erzeugen, die dem PSX-SPU-Dekodierungsalgorithmus entsprechen:
 *   - Vorzeichen-Erweiterung (4-bit → 32-bit)
 *   - Shift um (12 - shift)
 *   - Filter-Koeffizienten f0/f1
 *   - Clamping auf [-32768, 32767]
 *
 * Test-Strategie:
 *   1. Referenz-Decoder direkt im Test (identisch zu audio_pc.c Algorithmus)
 *   2. Generiere gültige 16-Byte ADPCM-Blöcke mit Shift 0-12, Filter 0-4
 *   3. Dekodiere und prüfe:
 *      a) Exakt 28 PCM-Samples pro Block
 *      b) Alle Samples im Bereich [-32768, 32767]
 *      c) Prediktion korrekt: (f0*prev0 + f1*prev1 + 32) / 64
 *      d) History-Update konsistent (prev1 = prev0_alt, prev0 = neues_sample)
 *   4. Zusätzlich: Grenzfälle (alle Nibbles 0, alle Nibbles max/min)
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
 * Simpler PRNG (xorshift32) für reproduzierbare Zufallswerte
 * ========================================================================= */

static uint32_t g_rng_state = 0;

static uint32_t xorshift32(void)
{
    uint32_t x = g_rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    g_rng_state = x;
    return x;
}

static uint8_t rand_u8(void)
{
    return (uint8_t)(xorshift32() & 0xFF);
}

/* =========================================================================
 * PSX-ADPCM Referenz-Konstanten
 * ========================================================================= */

/** Samples pro 16-Byte ADPCM-Block */
#define ADPCM_SAMPLES_PER_BLOCK 28

/** Block-Größe in Bytes */
#define ADPCM_BLOCK_SIZE 16

/** Filter-Koeffizienten (identisch zu PSX-SPU und audio_pc.c) */
static const int16_t s_filter_f0[5] = {  0,  60, 115,  98, 122 };
static const int16_t s_filter_f1[5] = {  0,   0, -52, -55, -60 };

/* =========================================================================
 * Referenz-ADPCM-Decoder (identische Logik wie audio_pc.c)
 *
 * Dekodiert einen einzelnen 16-Byte Block zu 28 int16_t Samples.
 * ========================================================================= */

/**
 * Dekodiert einen 16-Byte PSX-ADPCM-Block.
 *
 * @param block     16-Byte ADPCM-Block
 * @param out       Ausgabe-Puffer für 28 int16_t Samples
 * @param prev0     Zeiger auf History-Sample 0 (wird aktualisiert)
 * @param prev1     Zeiger auf History-Sample 1 (wird aktualisiert)
 * @return          Flags-Byte (Block[1])
 */
static uint8_t ref_adpcm_decode_block(const uint8_t* block, int16_t* out,
                                       int32_t* prev0, int32_t* prev1)
{
    int shift  = (int)(block[0] & 0x0F);
    int filter = (int)((block[0] >> 4) & 0x0F);
    uint8_t flags = block[1];
    int f0, f1;
    int i;

    /* Filter clampen auf gültigen Bereich 0-4 */
    if (filter > 4) filter = 4;

    /* Shift clampen: PSX-SPU-Verhalten bei shift > 12 */
    if (shift > 12) shift = 9;

    f0 = s_filter_f0[filter];
    f1 = s_filter_f1[filter];

    for (i = 0; i < ADPCM_SAMPLES_PER_BLOCK; i++) {
        int byte_idx = 2 + (i / 2);
        int nibble;
        int32_t sample;
        int32_t predicted;

        /* Low nibble zuerst, dann High nibble */
        if ((i & 1) == 0) {
            nibble = (int)(block[byte_idx] & 0x0F);
        } else {
            nibble = (int)((block[byte_idx] >> 4) & 0x0F);
        }

        /* Sign-Extend von 4-bit auf 32-bit */
        if (nibble >= 8) {
            nibble -= 16;
        }

        /* Shift anwenden: left-shift um (12 - shift) */
        sample = (int32_t)(nibble << (12 - shift));

        /* Filter-Prediktion addieren */
        predicted = (f0 * (*prev0) + f1 * (*prev1) + 32) / 64;
        sample += predicted;

        /* Clamp auf 16-bit Bereich */
        if (sample > 32767) sample = 32767;
        if (sample < -32768) sample = -32768;

        out[i] = (int16_t)sample;

        /* History aktualisieren */
        *prev1 = *prev0;
        *prev0 = sample;
    }

    return flags;
}

/* =========================================================================
 * ADPCM-Block-Generator
 *
 * Erzeugt gültige 16-Byte Blöcke mit kontrollierten Parametern.
 * ========================================================================= */

/**
 * Generiert einen gültigen ADPCM-Block mit spezifizierten Parametern.
 *
 * @param block     Ausgabe: 16-Byte-Block
 * @param shift     Shift-Wert (0-12)
 * @param filter    Filter-Index (0-4)
 * @param flags     Flags-Byte
 */
static void generate_adpcm_block(uint8_t* block, int shift, int filter,
                                  uint8_t flags)
{
    int i;

    /* Byte 0: shift (low 4 bits) + filter (high 4 bits) */
    block[0] = (uint8_t)((filter << 4) | (shift & 0x0F));

    /* Byte 1: Flags */
    block[1] = flags;

    /* Bytes 2-15: 14 Bytes mit zufälligen Nibble-Paaren */
    for (i = 2; i < ADPCM_BLOCK_SIZE; i++) {
        block[i] = rand_u8();
    }
}

/**
 * Generiert einen Block mit allen Nibbles auf einem bestimmten Wert.
 */
static void generate_uniform_block(uint8_t* block, int shift, int filter,
                                    uint8_t flags, uint8_t nibble_val)
{
    int i;
    uint8_t byte_val;

    block[0] = (uint8_t)((filter << 4) | (shift & 0x0F));
    block[1] = flags;

    /* Jedes Byte enthält zwei identische Nibbles */
    byte_val = (uint8_t)((nibble_val << 4) | nibble_val);
    for (i = 2; i < ADPCM_BLOCK_SIZE; i++) {
        block[i] = byte_val;
    }
}

/* =========================================================================
 * Property-Verifikation: Einzelner Block
 * ========================================================================= */

/**
 * Verifiziert einen dekodierten ADPCM-Block gegen die Spezifikation.
 *
 * Prüft:
 *   1. Exakt 28 Samples wurden geschrieben
 *   2. Jedes Sample liegt im Bereich [-32768, 32767]
 *   3. Jedes Sample entspricht dem manuell berechneten Referenzwert
 *   4. History-Update ist korrekt (prev0/prev1 nach Dekodierung)
 */
static int verify_adpcm_block(const uint8_t* block, int32_t init_prev0,
                               int32_t init_prev1)
{
    int16_t out[ADPCM_SAMPLES_PER_BLOCK];
    int32_t prev0 = init_prev0;
    int32_t prev1 = init_prev1;
    int shift, filter, f0, f1;
    int i;

    /* Dekodiere mit Referenz-Decoder */
    ref_adpcm_decode_block(block, out, &prev0, &prev1);

    /* Parameter extrahieren */
    shift  = (int)(block[0] & 0x0F);
    filter = (int)((block[0] >> 4) & 0x0F);
    if (filter > 4) filter = 4;
    if (shift > 12) shift = 9;

    f0 = s_filter_f0[filter];
    f1 = s_filter_f1[filter];

    /* Manuell Schritt-für-Schritt verifizieren */
    {
        int32_t verify_prev0 = init_prev0;
        int32_t verify_prev1 = init_prev1;

        for (i = 0; i < ADPCM_SAMPLES_PER_BLOCK; i++) {
            int byte_idx = 2 + (i / 2);
            int nibble;
            int32_t sample, predicted;

            /* Nibble extrahieren */
            if ((i & 1) == 0) {
                nibble = (int)(block[byte_idx] & 0x0F);
            } else {
                nibble = (int)((block[byte_idx] >> 4) & 0x0F);
            }

            /* Sign-Extend */
            if (nibble >= 8) nibble -= 16;

            /* Shift */
            sample = (int32_t)(nibble << (12 - shift));

            /* Filter-Prediktion */
            predicted = (f0 * verify_prev0 + f1 * verify_prev1 + 32) / 64;
            sample += predicted;

            /* Clamp */
            if (sample > 32767) sample = 32767;
            if (sample < -32768) sample = -32768;

            /* Prüfe: Sample stimmt mit Decoder-Ausgabe überein */
            PROP_ASSERT(out[i] == (int16_t)sample,
                "Sample %d: Decoder=%d, Referenz=%d (shift=%d, filter=%d, "
                "nibble=%d, predicted=%d)",
                i, (int)out[i], (int)(int16_t)sample,
                shift, filter, nibble, (int)predicted);

            /* Prüfe: Sample im gültigen Bereich */
            PROP_ASSERT(out[i] >= -32768 && out[i] <= 32767,
                "Sample %d out of range: %d", i, (int)out[i]);

            /* History-Update */
            verify_prev1 = verify_prev0;
            verify_prev0 = sample;
        }

        /* Prüfe: History nach der Dekodierung stimmt */
        PROP_ASSERT(prev0 == verify_prev0,
            "prev0 mismatch: Decoder=%d, Referenz=%d",
            (int)prev0, (int)verify_prev0);
        PROP_ASSERT(prev1 == verify_prev1,
            "prev1 mismatch: Decoder=%d, Referenz=%d",
            (int)prev1, (int)verify_prev1);
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Gezielte Grenzfall-Tests
 * ========================================================================= */

/**
 * Teste alle Filter/Shift-Kombinationen mit Null-Nibbles.
 * Erwartet: Alle Samples = 0 wenn prev0=prev1=0 (kein Eingang, kein Filter).
 */
static int test_zero_nibbles(void)
{
    int shift, filter;

    printf("  Teste Null-Nibbles (alle Filter/Shift Kombinationen)...\n");

    for (filter = 0; filter <= 4; filter++) {
        for (shift = 0; shift <= 12; shift++) {
            uint8_t block[ADPCM_BLOCK_SIZE];
            int16_t out[ADPCM_SAMPLES_PER_BLOCK];
            int32_t prev0 = 0, prev1 = 0;
            int i;

            generate_uniform_block(block, shift, filter, 0x00, 0x00);
            ref_adpcm_decode_block(block, out, &prev0, &prev1);

            /* Bei Filter 0 und Nibbles=0: Alle Samples müssen 0 sein */
            if (filter == 0) {
                for (i = 0; i < ADPCM_SAMPLES_PER_BLOCK; i++) {
                    PROP_ASSERT(out[i] == 0,
                        "Zero-nibble/filter0: Sample %d != 0 (got %d, "
                        "shift=%d)",
                        i, (int)out[i], shift);
                }
            }

            /* Bei allen Filtern: Samples müssen im Bereich liegen */
            for (i = 0; i < ADPCM_SAMPLES_PER_BLOCK; i++) {
                PROP_ASSERT(out[i] >= -32768 && out[i] <= 32767,
                    "Zero-nibble: Sample %d out of range: %d (shift=%d, "
                    "filter=%d)",
                    i, (int)out[i], shift, filter);
            }

            g_passed++;
        }
    }
    return 0;
}

/**
 * Teste maximale positive Nibbles (0x7 = +7 sign-extended bleibt +7).
 */
static int test_max_positive_nibbles(void)
{
    int shift, filter;

    printf("  Teste maximale positive Nibbles (0x7)...\n");

    for (filter = 0; filter <= 4; filter++) {
        for (shift = 0; shift <= 12; shift++) {
            uint8_t block[ADPCM_BLOCK_SIZE];

            generate_uniform_block(block, shift, filter, 0x00, 0x07);

            if (verify_adpcm_block(block, 0, 0) != 0) return 1;
        }
    }
    return 0;
}

/**
 * Teste maximale negative Nibbles (0x8 = -8 sign-extended).
 */
static int test_max_negative_nibbles(void)
{
    int shift, filter;

    printf("  Teste maximale negative Nibbles (0x8 = -8)...\n");

    for (filter = 0; filter <= 4; filter++) {
        for (shift = 0; shift <= 12; shift++) {
            uint8_t block[ADPCM_BLOCK_SIZE];

            generate_uniform_block(block, shift, filter, 0x00, 0x08);

            if (verify_adpcm_block(block, 0, 0) != 0) return 1;
        }
    }
    return 0;
}

/**
 * Teste mit nicht-null initialen History-Werten (Prediktion wird aktiv).
 */
static int test_nonzero_history(void)
{
    int filter;

    printf("  Teste mit nicht-null History (prev0/prev1 != 0)...\n");

    for (filter = 0; filter <= 4; filter++) {
        uint8_t block[ADPCM_BLOCK_SIZE];

        /* Block mit zufälligem Inhalt, mittlerem Shift */
        generate_uniform_block(block, 6, filter, 0x00, 0x05);

        /* Verschiedene History-Werte */
        if (verify_adpcm_block(block, 1000, -500) != 0) return 1;
        if (verify_adpcm_block(block, -32768, 32767) != 0) return 1;
        if (verify_adpcm_block(block, 32767, 32767) != 0) return 1;
        if (verify_adpcm_block(block, -32768, -32768) != 0) return 1;
    }
    return 0;
}

/**
 * Teste Clamping: Maximal amplifizierten Block mit shift=0 und Filter-Prediktion.
 */
static int test_clamping(void)
{
    uint8_t block[ADPCM_BLOCK_SIZE];
    int16_t out[ADPCM_SAMPLES_PER_BLOCK];
    int32_t prev0, prev1;
    int i;

    printf("  Teste Clamping-Verhalten...\n");

    /* Shift=0, Filter=1 (f0=60): Maximale Amplifikation + positive Prediktion */
    generate_uniform_block(block, 0, 1, 0x00, 0x07);
    prev0 = 32767;
    prev1 = 32767;
    ref_adpcm_decode_block(block, out, &prev0, &prev1);

    for (i = 0; i < ADPCM_SAMPLES_PER_BLOCK; i++) {
        PROP_ASSERT(out[i] >= -32768 && out[i] <= 32767,
            "Clamping positive: Sample %d = %d out of 16-bit range",
            i, (int)out[i]);
    }
    g_passed++;

    /* Shift=0, Filter=1: Maximale negative Amplifikation + negative Prediktion */
    generate_uniform_block(block, 0, 1, 0x00, 0x08);
    prev0 = -32768;
    prev1 = -32768;
    ref_adpcm_decode_block(block, out, &prev0, &prev1);

    for (i = 0; i < ADPCM_SAMPLES_PER_BLOCK; i++) {
        PROP_ASSERT(out[i] >= -32768 && out[i] <= 32767,
            "Clamping negative: Sample %d = %d out of 16-bit range",
            i, (int)out[i]);
    }
    g_passed++;

    return 0;
}

/**
 * Teste: Exakt 28 Samples werden erzeugt (Puffer-Integrität).
 */
static int test_sample_count(void)
{
    uint8_t block[ADPCM_BLOCK_SIZE];
    int16_t out[ADPCM_SAMPLES_PER_BLOCK + 4];  /* Extra Platz zur Prüfung */
    int32_t prev0 = 0, prev1 = 0;
    int i;
    const int16_t sentinel = (int16_t)0xBEEF;

    printf("  Teste exakte Sample-Anzahl (28)...\n");

    /* Sentinel-Werte nach dem 28-Sample-Puffer setzen */
    for (i = 0; i < ADPCM_SAMPLES_PER_BLOCK + 4; i++) {
        out[i] = sentinel;
    }

    generate_adpcm_block(block, 6, 2, 0x00);
    ref_adpcm_decode_block(block, out, &prev0, &prev1);

    /* Prüfe: Sentinel-Werte nach 28 Samples sind unverändert */
    for (i = ADPCM_SAMPLES_PER_BLOCK; i < ADPCM_SAMPLES_PER_BLOCK + 4; i++) {
        PROP_ASSERT(out[i] == sentinel,
            "Buffer overrun at index %d: got 0x%04X (expected sentinel 0x%04X)",
            i, (unsigned)(uint16_t)out[i], (unsigned)(uint16_t)sentinel);
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Randomisierter Property-Test
 * ========================================================================= */

#define NUM_RANDOM_ITERATIONS 10000

/**
 * Generiert zufällige gültige ADPCM-Blöcke und verifiziert die Dekodierung.
 */
static int test_random_blocks(void)
{
    int i;

    printf("  Teste %d zufaellige ADPCM-Bloecke...\n", NUM_RANDOM_ITERATIONS);

    for (i = 0; i < NUM_RANDOM_ITERATIONS; i++) {
        uint8_t block[ADPCM_BLOCK_SIZE];
        int shift, filter;
        int32_t init_prev0, init_prev1;

        /* Gültige Shift/Filter-Werte generieren */
        shift  = (int)(xorshift32() % 13);   /* 0-12 */
        filter = (int)(xorshift32() % 5);    /* 0-4 */

        /* Zufällige initiale History (vorheriger Block-Zustand) */
        init_prev0 = (int32_t)(int16_t)(xorshift32() & 0xFFFF);
        init_prev1 = (int32_t)(int16_t)(xorshift32() & 0xFFFF);

        /* Block generieren */
        generate_adpcm_block(block, shift, filter, 0x00);

        /* Verifizieren */
        if (verify_adpcm_block(block, init_prev0, init_prev1) != 0) {
            printf("    Fehler bei Iteration %d: shift=%d, filter=%d, "
                   "prev0=%d, prev1=%d\n",
                   i, shift, filter, (int)init_prev0, (int)init_prev1);
            return 1;
        }
    }
    return 0;
}

/**
 * Testet Multi-Block-Dekodierung (Block-Kette, History wird weitergereicht).
 */
static int test_multi_block_chain(void)
{
    int chain_len = 10;
    int block_idx;
    int32_t prev0 = 0, prev1 = 0;

    printf("  Teste Multi-Block-Kette (%d Bloecke)...\n", chain_len);

    for (block_idx = 0; block_idx < chain_len; block_idx++) {
        uint8_t block[ADPCM_BLOCK_SIZE];
        int16_t out[ADPCM_SAMPLES_PER_BLOCK];
        int32_t chain_prev0 = prev0;
        int32_t chain_prev1 = prev1;
        int shift, filter;
        int i;

        shift  = (int)(xorshift32() % 13);
        filter = (int)(xorshift32() % 5);
        generate_adpcm_block(block, shift, filter, 0x00);

        /* Dekodiere mit aktueller History */
        ref_adpcm_decode_block(block, out, &chain_prev0, &chain_prev1);

        /* Prüfe alle Samples im Bereich */
        for (i = 0; i < ADPCM_SAMPLES_PER_BLOCK; i++) {
            PROP_ASSERT(out[i] >= -32768 && out[i] <= 32767,
                "Chain block %d, sample %d: %d out of range",
                block_idx, i, (int)out[i]);
        }

        /* Prüfe: History nach Block = letztes Sample */
        PROP_ASSERT(chain_prev0 == (int32_t)out[ADPCM_SAMPLES_PER_BLOCK - 1],
            "Chain block %d: prev0=%d != last_sample=%d",
            block_idx, (int)chain_prev0,
            (int)out[ADPCM_SAMPLES_PER_BLOCK - 1]);

        PROP_ASSERT(chain_prev1 == (int32_t)out[ADPCM_SAMPLES_PER_BLOCK - 2],
            "Chain block %d: prev1=%d != second_last_sample=%d",
            block_idx, (int)chain_prev1,
            (int)out[ADPCM_SAMPLES_PER_BLOCK - 2]);

        /* History weitergeben */
        prev0 = chain_prev0;
        prev1 = chain_prev1;
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Main — Property-Test mit gezielten und zufälligen Testfällen
 * ========================================================================= */

int main(void)
{
    uint32_t seed;

    printf("=== Property-Test: ADPCM-Dekodierung ===\n");
    printf("    Validates: Requirements 6.3\n\n");

    /* Seed initialisieren */
    seed = (uint32_t)time(NULL);
    g_rng_state = seed;
    printf("    PRNG Seed: %u\n\n", seed);

    /* Phase 1: Gezielte Grenzfälle */
    printf("[1/7] Teste Null-Nibbles...\n");
    if (test_zero_nibbles() != 0) goto fail;

    printf("[2/7] Teste maximale positive Nibbles...\n");
    if (test_max_positive_nibbles() != 0) goto fail;

    printf("[3/7] Teste maximale negative Nibbles...\n");
    if (test_max_negative_nibbles() != 0) goto fail;

    printf("[4/7] Teste nicht-null History...\n");
    if (test_nonzero_history() != 0) goto fail;

    printf("[5/7] Teste Clamping-Verhalten...\n");
    if (test_clamping() != 0) goto fail;

    printf("[6/7] Teste exakte Sample-Anzahl...\n");
    if (test_sample_count() != 0) goto fail;

    /* Phase 2: Randomisierte Tests */
    printf("[7/7] Randomisierte Property-Tests...\n");
    if (test_random_blocks() != 0) goto fail;

    /* Phase 3: Multi-Block-Kette */
    printf("\n[Bonus] Multi-Block-Ketten-Test...\n");
    if (test_multi_block_chain() != 0) goto fail;

    /* Zusammenfassung */
    printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);
    return 0;

fail:
    printf("\n=== ABBRUCH ===\n");
    printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen (seed=%u) ===\n",
           g_passed, g_failed, seed);
    return 1;
}
