/**
 * @file prop_savestate_cmp.c
 * @brief Property-Test: Savestate-Vergleichstoleranz
 *
 * **Validates: Requirements 12.3**
 *
 * Property 17: Für jede zwei Positionswerte p1 und p2 (Fixkomma) soll der
 * Vergleichsmodus diese als "übereinstimmend" melden, wenn |p1 - p2| <= 1,
 * und als "abweichend" mit Soll/Ist-Ausgabe, wenn |p1 - p2| > 1.
 *
 * Getestet wird die Funktion re15_savestate_pos_match(expected, actual):
 *   - Gibt 1 zurück wenn |expected - actual| <= RE15_SAVESTATE_POS_TOLERANCE (= 1)
 *   - Gibt 0 zurück wenn |expected - actual| > RE15_SAVESTATE_POS_TOLERANCE
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "re15_savestate.h"

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

/** Generiert einen zufälligen int32_t im vollen Wertebereich */
static int32_t rand_i32(void)
{
    return (int32_t)xorshift32();
}

/** Generiert einen zufälligen int32_t im Bereich [min, max] (inklusive) */
static int32_t rand_i32_range(int32_t min, int32_t max)
{
    uint32_t range = (uint32_t)(max - min + 1);
    return min + (int32_t)(xorshift32() % range);
}

/* =========================================================================
 * Property-Verifikation
 * ========================================================================= */

/**
 * Verifiziert die Toleranzlogik für ein einzelnes (p1, p2) Paar.
 *
 * Prüft:
 * - |p1 - p2| <= 1 → re15_savestate_pos_match gibt 1 zurück ("übereinstimmend")
 * - |p1 - p2| > 1  → re15_savestate_pos_match gibt 0 zurück ("abweichend")
 */
static int verify_tolerance(int32_t p1, int32_t p2)
{
    int result = re15_savestate_pos_match(p1, p2);
    int64_t diff64 = (int64_t)p1 - (int64_t)p2;
    int64_t abs_diff = diff64 < 0 ? -diff64 : diff64;
    int expected_match = (abs_diff <= RE15_SAVESTATE_POS_TOLERANCE) ? 1 : 0;

    PROP_ASSERT(result == expected_match,
        "p1=%d, p2=%d: diff=%lld, got match=%d, expected match=%d",
        (int)p1, (int)p2, (long long)abs_diff, result, expected_match);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Gezielte Grenzfall-Tests
 * ========================================================================= */

/**
 * Testet spezifische Grenzfälle der Toleranzprüfung.
 */
static int test_boundary_cases(void)
{
    /* Delta = 0: Exakte Übereinstimmung (immer match) */
    if (verify_tolerance(0, 0) != 0) return 1;
    if (verify_tolerance(1000, 1000) != 0) return 1;
    if (verify_tolerance(-1000, -1000) != 0) return 1;

    /* Delta = 1: Grenzfall (noch match, da Toleranz = 1) */
    if (verify_tolerance(100, 101) != 0) return 1;
    if (verify_tolerance(101, 100) != 0) return 1;
    if (verify_tolerance(-100, -99) != 0) return 1;
    if (verify_tolerance(-99, -100) != 0) return 1;
    if (verify_tolerance(0, 1) != 0) return 1;
    if (verify_tolerance(1, 0) != 0) return 1;
    if (verify_tolerance(0, -1) != 0) return 1;
    if (verify_tolerance(-1, 0) != 0) return 1;

    /* Delta = 2: Knapp außerhalb der Toleranz (mismatch) */
    if (verify_tolerance(100, 102) != 0) return 1;
    if (verify_tolerance(102, 100) != 0) return 1;
    if (verify_tolerance(-100, -98) != 0) return 1;
    if (verify_tolerance(-98, -100) != 0) return 1;
    if (verify_tolerance(0, 2) != 0) return 1;
    if (verify_tolerance(2, 0) != 0) return 1;
    if (verify_tolerance(0, -2) != 0) return 1;
    if (verify_tolerance(-2, 0) != 0) return 1;

    /* Vorzeichen-Wechsel um die Null herum */
    if (verify_tolerance(-1, 1) != 0) return 1;  /* diff = 2 → mismatch */
    if (verify_tolerance(1, -1) != 0) return 1;  /* diff = 2 → mismatch */
    if (verify_tolerance(-1, 0) != 0) return 1;  /* diff = 1 → match */
    if (verify_tolerance(0, -1) != 0) return 1;  /* diff = 1 → match */

    /* Extremwerte (INT32_MAX / INT32_MIN Bereich) */
    if (verify_tolerance(INT32_MAX, INT32_MAX) != 0) return 1;
    if (verify_tolerance(INT32_MAX, INT32_MAX - 1) != 0) return 1;
    if (verify_tolerance(INT32_MIN, INT32_MIN) != 0) return 1;
    if (verify_tolerance(INT32_MIN, INT32_MIN + 1) != 0) return 1;

    /* Große Differenzen */
    if (verify_tolerance(INT32_MAX, 0) != 0) return 1;
    if (verify_tolerance(0, INT32_MIN) != 0) return 1;

    return 0;
}

/* =========================================================================
 * Systematischer Delta-Test
 * ========================================================================= */

/**
 * Testet systematisch Deltas von 0 bis 10 um verschiedene Basiswerte.
 * Stellt sicher dass die Toleranzgrenze (1) exakt eingehalten wird.
 */
static int test_systematic_deltas(void)
{
    int32_t bases[] = {0, 100, -100, 5000, -5000, 32767, -32768, 1000000, -1000000};
    int num_bases = (int)(sizeof(bases) / sizeof(bases[0]));
    int b, d;

    for (b = 0; b < num_bases; b++) {
        int32_t base = bases[b];

        for (d = 0; d <= 10; d++) {
            /* Positives Delta */
            int32_t p2_pos = base + d;
            /* Overflow-Check: Überspringe wenn Überlauf auftreten würde */
            if ((int64_t)base + d <= INT32_MAX) {
                if (verify_tolerance(base, p2_pos) != 0) return 1;
            }

            /* Negatives Delta */
            int32_t p2_neg = base - d;
            if ((int64_t)base - d >= INT32_MIN) {
                if (verify_tolerance(base, p2_neg) != 0) return 1;
            }
        }
    }

    return 0;
}

/* =========================================================================
 * Symmetrie-Test
 * ========================================================================= */

/**
 * Prüft dass pos_match symmetrisch ist: match(p1, p2) == match(p2, p1).
 */
static int test_symmetry(void)
{
    int i;

    for (i = 0; i < 1000; i++) {
        int32_t p1 = rand_i32();
        int32_t p2 = rand_i32();
        int r1 = re15_savestate_pos_match(p1, p2);
        int r2 = re15_savestate_pos_match(p2, p1);

        PROP_ASSERT(r1 == r2,
            "Symmetrie verletzt: match(%d, %d)=%d aber match(%d, %d)=%d",
            (int)p1, (int)p2, r1, (int)p2, (int)p1, r2);

        g_passed++;
    }

    return 0;
}

/* =========================================================================
 * Main — Randomisierter Property-Test mit N Iterationen
 * ========================================================================= */

#define NUM_RANDOM_ITERATIONS 10000

int main(void)
{
    int i;
    uint32_t seed;

    printf("=== Property-Test: Savestate-Vergleichstoleranz ===\n");
    printf("    Validates: Requirements 12.3\n");
    printf("    Toleranz: RE15_SAVESTATE_POS_TOLERANCE = %d\n\n",
           RE15_SAVESTATE_POS_TOLERANCE);

    /* Seed initialisieren (reproduzierbar via Seed-Ausgabe) */
    seed = (uint32_t)time(NULL);
    g_rng_state = seed;
    printf("    PRNG Seed: %u\n\n", seed);

    /* Phase 1: Gezielte Grenzfälle */
    printf("[1/4] Teste Grenzfaelle...\n");
    if (test_boundary_cases() != 0) {
        printf("\n=== ABBRUCH bei Grenzfaellen ===\n");
        printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
               g_passed, g_failed);
        return 1;
    }
    printf("    Alle Grenzfaelle bestanden.\n\n");

    /* Phase 2: Systematische Deltas */
    printf("[2/4] Teste systematische Deltas (0-10 um diverse Basiswerte)...\n");
    if (test_systematic_deltas() != 0) {
        printf("\n=== ABBRUCH bei systematischen Deltas ===\n");
        printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
               g_passed, g_failed);
        return 1;
    }
    printf("    Alle systematischen Delta-Tests bestanden.\n\n");

    /* Phase 3: Symmetrie-Test */
    printf("[3/4] Teste Symmetrie (1000 zufaellige Paare)...\n");
    if (test_symmetry() != 0) {
        printf("\n=== ABBRUCH bei Symmetrie-Test ===\n");
        printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
               g_passed, g_failed);
        return 1;
    }
    printf("    Alle Symmetrie-Tests bestanden.\n\n");

    /* Phase 4: Randomisierte Tests mit gezieltem Delta */
    printf("[4/4] Teste %d zufaellige Positionspaare mit kontrolliertem Delta...\n",
           NUM_RANDOM_ITERATIONS);

    for (i = 0; i < NUM_RANDOM_ITERATIONS; i++) {
        int32_t p1 = rand_i32();
        int32_t delta;
        int32_t p2;

        /* Generiere Delta: Mix aus kleinen (Toleranzgrenze) und großen Deltas */
        if (i % 3 == 0) {
            /* Delta 0 oder 1 — sollte matchen */
            delta = rand_i32_range(0, 1);
        } else if (i % 3 == 1) {
            /* Delta 2 bis 100 — sollte nicht matchen */
            delta = rand_i32_range(2, 100);
        } else {
            /* Beliebiges Delta (voller Wertebereich) */
            delta = rand_i32();
        }

        /* Zufällig Vorzeichen des Deltas wählen */
        if (xorshift32() & 1) {
            delta = -delta;
        }

        /* Overflow-safe Berechnung von p2 */
        int64_t p2_64 = (int64_t)p1 + (int64_t)delta;
        if (p2_64 > INT32_MAX || p2_64 < INT32_MIN) {
            /* Overflow: Überspringe diese Iteration */
            continue;
        }
        p2 = (int32_t)p2_64;

        if (verify_tolerance(p1, p2) != 0) {
            printf("\n=== ABBRUCH bei Iteration %d (seed=%u) ===\n", i, seed);
            printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
                   g_passed, g_failed);
            return 1;
        }
    }

    printf("    Alle %d zufaelligen Tests bestanden.\n\n", NUM_RANDOM_ITERATIONS);

    /* Zusammenfassung */
    printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
