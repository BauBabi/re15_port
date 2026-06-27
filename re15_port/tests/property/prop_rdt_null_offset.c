/**
 * @file prop_rdt_null_offset.c
 * @brief Property-Test: RDT Null-Offset-Sektionsbehandlung
 *
 * **Validates: Requirements 10.8**
 *
 * Property 15: Für jede RDT-Datei mit beliebiger Kombination von Null-Offsets
 * (0x00000000) in der Adresstabelle soll der Parser diese Sektionen als nicht
 * vorhanden behandeln (Pointer = NULL setzen) und alle übrigen Sektionen
 * korrekt parsen, ohne Absturz oder undefiniertes Verhalten.
 *
 * Teststrategie:
 * - Generiere synthetische RDT-Buffer (mindestens 0x60 + Padding für Daten)
 * - Für N Iterationen (200): setze zufällig einige der 21 Offsets auf 0x00000000,
 *   andere auf gültige Offsets innerhalb des Puffers
 * - Prüfe: Null-Offset → Pointer == NULL
 * - Prüfe: Gültiger Offset → Pointer == buffer + offset
 * - Prüfe: Kein Absturz bei beliebigen Null-Mustern
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "re15_rdt.h"

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
 * PRNG — Einfacher xorshift32 für reproduzierbare Zufallszahlen
 * ========================================================================= */

static uint32_t prng_state = 0;

static void prng_seed(uint32_t seed)
{
    prng_state = seed ? seed : 1;
}

static uint32_t prng_next(void)
{
    uint32_t x = prng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    prng_state = x;
    return x;
}


/* =========================================================================
 * Hilfsfunktion: Little-Endian 32-Bit-Wert in Buffer schreiben
 * ========================================================================= */

static void write_le32(uint8_t* p, uint32_t val)
{
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)((val >> 8) & 0xFF);
    p[2] = (uint8_t)((val >> 16) & 0xFF);
    p[3] = (uint8_t)((val >> 24) & 0xFF);
}

/* =========================================================================
 * Konstanten
 * ========================================================================= */

/** Puffergröße: 0x60 Bytes Header/Tabelle + 256 Bytes Datenbereich */
#define TEST_BUFFER_SIZE  (0x60 + 256)

/** Beginn des Datenbereichs (nach der Adresstabelle) */
#define DATA_AREA_START   0x60

/** Anzahl der Property-Test-Iterationen */
#define NUM_ITERATIONS    200

/* =========================================================================
 * Property-Test: Eine Iteration mit zufälligem Null-Offset-Muster
 * ========================================================================= */

/**
 * Führt eine Iteration durch:
 * 1. Erstellt einen synthetischen RDT-Buffer
 * 2. Setzt zufällig einige Offsets auf 0, andere auf gültige Werte
 * 3. Parst den Buffer
 * 4. Verifiziert alle 21 Sektions-Pointer
 *
 * @param iteration  Aktuelle Iterationsnummer (für Fehlerausgabe)
 * @param null_mask  Bitmaske: Bit i gesetzt → Offset i wird auf 0 gesetzt
 * @return 0 bei Erfolg, 1 bei Fehler
 */
static int test_iteration(int iteration, uint32_t null_mask)
{
    uint8_t buffer[TEST_BUFFER_SIZE];
    re15_rdt_t rdt;
    int result;
    int i;
    uint32_t offsets[RE15_RDT_SECTION_COUNT];
    uint8_t** rdt_sections[RE15_RDT_SECTION_COUNT];

    /* Buffer mit nicht-null Wert füllen (erleichtert Debugging) */
    memset(buffer, 0xAB, TEST_BUFFER_SIZE);

    /* Header-Bytes (0x00-0x07): beliebig, Parser liest sie nicht */
    buffer[0] = 0x52; /* 'R' */
    buffer[1] = 0x44; /* 'D' */
    buffer[2] = 0x54; /* 'T' */
    buffer[3] = 0x00;
    buffer[4] = 0x00;
    buffer[5] = 0x00;
    buffer[6] = 0x00;
    buffer[7] = 0x00;

    /* 21 Offsets in der Adresstabelle setzen (0x08 bis 0x5C) */
    for (i = 0; i < RE15_RDT_SECTION_COUNT; i++) {
        if (null_mask & (1u << i)) {
            /* Dieser Offset ist 0 → Sektion nicht vorhanden */
            offsets[i] = 0x00000000;
        } else {
            /* Gültiger Offset: zeigt in den Datenbereich
             * Verteile die Offsets über den Datenbereich */
            offsets[i] = DATA_AREA_START + (uint32_t)(i * 12);
        }
        write_le32(buffer + 0x08 + (i * 4), offsets[i]);
    }

    /* Parser aufrufen */
    result = re15_rdt_parse(buffer, TEST_BUFFER_SIZE, &rdt);

    PROP_ASSERT(result == 0,
        "Iteration %d (mask=0x%08X): re15_rdt_parse returned %d (expected 0)",
        iteration, null_mask, result);

    /* Pointer-Array auf die struct-Felder aufbauen (gleiche Reihenfolge wie Parser) */
    rdt_sections[0]  = &rdt.snd0_edt;
    rdt_sections[1]  = &rdt.snd0_vh;
    rdt_sections[2]  = &rdt.snd0_vb;
    rdt_sections[3]  = &rdt.snd1_edt;
    rdt_sections[4]  = &rdt.snd1_vh;
    rdt_sections[5]  = &rdt.snd1_vb;
    rdt_sections[6]  = &rdt.collision;
    rdt_sections[7]  = &rdt.camera;
    rdt_sections[8]  = &rdt.zone;
    rdt_sections[9]  = &rdt.light;
    rdt_sections[10] = &rdt.md1_ptr;
    rdt_sections[11] = &rdt.floor;
    rdt_sections[12] = &rdt.block;
    rdt_sections[13] = &rdt.message;
    rdt_sections[14] = &rdt.main_scd;
    rdt_sections[15] = &rdt.sub_scd;
    rdt_sections[16] = &rdt.extra_scd;
    rdt_sections[17] = &rdt.effect;
    rdt_sections[18] = &rdt.esp_tim;
    rdt_sections[19] = &rdt.model_tim;
    rdt_sections[20] = &rdt.animation;

    /* Verifikation: Jede Sektion prüfen */
    for (i = 0; i < RE15_RDT_SECTION_COUNT; i++) {
        if (null_mask & (1u << i)) {
            /* Null-Offset → Pointer muss NULL sein */
            PROP_ASSERT(*rdt_sections[i] == NULL,
                "Iteration %d (mask=0x%08X): Sektion %d hat Null-Offset "
                "aber Pointer ist nicht NULL (ptr=%p)",
                iteration, null_mask, i, (void*)*rdt_sections[i]);
        } else {
            /* Gültiger Offset → Pointer muss buffer + offset sein */
            uint8_t* expected = buffer + offsets[i];
            PROP_ASSERT(*rdt_sections[i] == expected,
                "Iteration %d (mask=0x%08X): Sektion %d hat Offset 0x%08X "
                "aber Pointer=%p (expected %p, buffer=%p)",
                iteration, null_mask, i, offsets[i],
                (void*)*rdt_sections[i], (void*)expected, (void*)buffer);
        }
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test: Spezialfälle (Extremwerte)
 * ========================================================================= */

/**
 * Teste: Alle Offsets sind NULL → alle 21 Pointer müssen NULL sein.
 */
static int test_all_null(void)
{
    uint32_t mask = 0x001FFFFF; /* Bits 0-20 gesetzt = alle 21 Sektionen */
    return test_iteration(-1, mask);
}

/**
 * Teste: Kein Offset ist NULL → alle 21 Pointer müssen gültig sein.
 */
static int test_none_null(void)
{
    return test_iteration(-2, 0x00000000);
}

/**
 * Teste: Nur jeweils ein einzelner Offset ist NULL (21 Tests).
 */
static int test_single_null(void)
{
    int i;
    for (i = 0; i < RE15_RDT_SECTION_COUNT; i++) {
        uint32_t mask = 1u << i;
        if (test_iteration(-100 - i, mask) != 0) {
            return 1;
        }
    }
    return 0;
}

/* =========================================================================
 * Main — Randomisierter Property-Test
 * ========================================================================= */

int main(void)
{
    int i;
    uint32_t seed;
    uint32_t null_mask;

    printf("=== Property-Test: RDT Null-Offset-Sektionsbehandlung ===\n");
    printf("    Validates: Requirements 10.8\n\n");

    /* Deterministischer Seed (reproduzierbar), kann via Env überschrieben werden */
    seed = (uint32_t)time(NULL);
    printf("    PRNG Seed: %u\n\n", seed);
    prng_seed(seed);

    /* --- Phase 1: Spezialfälle --- */
    printf("[1/4] Teste Spezialfall: Alle Offsets NULL...\n");
    if (test_all_null() != 0) {
        printf("    FEHLER!\n");
        goto done;
    }
    printf("    OK.\n\n");

    printf("[2/4] Teste Spezialfall: Kein Offset NULL...\n");
    if (test_none_null() != 0) {
        printf("    FEHLER!\n");
        goto done;
    }
    printf("    OK.\n\n");

    printf("[3/4] Teste Spezialfall: Jeweils ein einzelner Offset NULL...\n");
    if (test_single_null() != 0) {
        printf("    FEHLER!\n");
        goto done;
    }
    printf("    OK (21 Einzeltests bestanden).\n\n");

    /* --- Phase 2: Randomisierte Tests --- */
    printf("[4/4] Teste %d zufaellige Null-Offset-Kombinationen...\n", NUM_ITERATIONS);
    for (i = 0; i < NUM_ITERATIONS; i++) {
        /* Generiere zufällige Bitmaske für 21 Bits */
        null_mask = prng_next() & 0x001FFFFF;
        if (test_iteration(i, null_mask) != 0) {
            printf("    FEHLER bei Iteration %d (mask=0x%08X)!\n", i, null_mask);
            goto done;
        }
    }
    printf("    %d Iterationen bestanden.\n\n", NUM_ITERATIONS);

done:
    /* Zusammenfassung */
    printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
