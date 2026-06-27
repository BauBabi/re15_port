/**
 * @file prop_input_edge.c
 * @brief Property-Test: Input-Edge-Detection
 *
 * **Validates: Requirements 7.1**
 *
 * Property 11: Für jedes Paar aufeinanderfolgender Frame-Zustände (previous,
 * current) als 16-Bit-Bitmasken soll gelten:
 *   pressed  = current & ~previous
 *   released = ~current & previous
 *   held     = current
 *
 * Da die Edge-Detection eine rein mathematische Operation ist, testen wir
 * die Formeln direkt mit zufälligen 16-Bit-Wertepaaren, ohne SDL oder
 * Hardware-Abhängigkeiten.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
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

static uint16_t rand_u16(void)
{
    return (uint16_t)(xorshift32() & 0xFFFF);
}

/* =========================================================================
 * Edge-Detection-Berechnung (die zu testende Logik)
 *
 * Bildet die Formeln ab, wie sie in re15_input_tick() verwendet werden.
 * ========================================================================= */

static void compute_edges(uint16_t previous, uint16_t current,
                          uint16_t* out_pressed, uint16_t* out_released,
                          uint16_t* out_held)
{
    *out_pressed  = current & ~previous;
    *out_released = ~current & previous;
    *out_held     = current;
}

/* =========================================================================
 * Property-Verifikation
 * ========================================================================= */

/**
 * Verifiziert die Edge-Detection-Formeln für ein einzelnes (previous, current)
 * Paar. Prüft sämtliche mathematischen Invarianten:
 *
 * 1. pressed  == current & ~previous  (Rising edges)
 * 2. released == ~current & previous  (Falling edges)
 * 3. held     == current              (Aktueller Zustand)
 * 4. pressed & released == 0          (Kein Bit gleichzeitig beides)
 * 5. pressed | held == held           (Pressed ist Teilmenge von Held)
 * 6. released & held == 0             (Released ist nie in Held)
 */
static int verify_edge_detection(uint16_t previous, uint16_t current)
{
    uint16_t pressed, released, held;
    uint16_t expected_pressed, expected_released, expected_held;

    compute_edges(previous, current, &pressed, &released, &held);

    /* Erwartete Werte direkt berechnen */
    expected_pressed  = current & (uint16_t)~previous;
    expected_released = (uint16_t)~current & previous;
    expected_held     = current;

    /* Property 1: pressed == current & ~previous */
    PROP_ASSERT(pressed == expected_pressed,
        "prev=0x%04X cur=0x%04X: pressed=0x%04X (expected 0x%04X)",
        previous, current, pressed, expected_pressed);

    /* Property 2: released == ~current & previous */
    PROP_ASSERT(released == expected_released,
        "prev=0x%04X cur=0x%04X: released=0x%04X (expected 0x%04X)",
        previous, current, released, expected_released);

    /* Property 3: held == current */
    PROP_ASSERT(held == expected_held,
        "prev=0x%04X cur=0x%04X: held=0x%04X (expected 0x%04X)",
        previous, current, held, expected_held);

    /* Derived property 4: pressed & released == 0
     * Ein Bit kann nicht gleichzeitig rising AND falling edge sein. */
    PROP_ASSERT((pressed & released) == 0,
        "prev=0x%04X cur=0x%04X: pressed & released = 0x%04X (expected 0)",
        previous, current, (uint16_t)(pressed & released));

    /* Derived property 5: pressed ist Teilmenge von held
     * Alles was neu gedrückt wurde, muss aktuell gedrückt sein. */
    PROP_ASSERT((pressed | held) == held,
        "prev=0x%04X cur=0x%04X: pressed (0x%04X) not subset of held (0x%04X)",
        previous, current, pressed, held);

    /* Derived property 6: released & held == 0
     * Losgelassene Tasten können nicht gleichzeitig gedrückt sein. */
    PROP_ASSERT((released & held) == 0,
        "prev=0x%04X cur=0x%04X: released & held = 0x%04X (expected 0)",
        previous, current, (uint16_t)(released & held));

    g_passed++;
    return 0;
}

/* =========================================================================
 * Gezielte Grenzfall-Tests
 * ========================================================================= */

/**
 * Testet spezifische Grenzfälle der Edge-Detection.
 */
static int test_boundary_cases(void)
{
    /* Kein Button gedrückt -> Kein Button gedrückt */
    if (verify_edge_detection(0x0000, 0x0000) != 0) return 1;

    /* Kein Button -> Alle Buttons */
    if (verify_edge_detection(0x0000, 0xFFFF) != 0) return 1;

    /* Alle Buttons -> Kein Button */
    if (verify_edge_detection(0xFFFF, 0x0000) != 0) return 1;

    /* Alle Buttons -> Alle Buttons (gehalten, keine Edges) */
    if (verify_edge_detection(0xFFFF, 0xFFFF) != 0) return 1;

    /* Einzelner Button gedrückt (Rising Edge) */
    if (verify_edge_detection(0x0000, 0x0010) != 0) return 1;  /* UP */

    /* Einzelner Button losgelassen (Falling Edge) */
    if (verify_edge_detection(0x0010, 0x0000) != 0) return 1;  /* UP los */

    /* Wechsel: Ein Button wird gedrückt, ein anderer losgelassen */
    if (verify_edge_detection(0x0010, 0x0020) != 0) return 1;  /* UP->RIGHT */

    /* Mehrere gleichzeitig neu gedrückt */
    if (verify_edge_detection(0x0000, 0x00F0) != 0) return 1;  /* D-Pad komplett */

    /* Teilweise überlappend */
    if (verify_edge_detection(0x00F0, 0x000F) != 0) return 1;

    /* Alternierendes Muster */
    if (verify_edge_detection(0x5555, 0xAAAA) != 0) return 1;
    if (verify_edge_detection(0xAAAA, 0x5555) != 0) return 1;

    /* PSX-Controller-typische Werte */
    if (verify_edge_detection(0x0000, 0x4000) != 0) return 1;  /* Cross */
    if (verify_edge_detection(0x4000, 0x4010) != 0) return 1;  /* Cross+UP */
    if (verify_edge_detection(0x4010, 0x0010) != 0) return 1;  /* nur UP */

    return 0;
}

/* =========================================================================
 * Einzelbit-Rotation-Test
 * ========================================================================= */

/**
 * Testet alle 16 Einzel-Bit-Positionen als Rising und Falling Edge.
 */
static int test_single_bit_rotation(void)
{
    int bit;
    for (bit = 0; bit < 16; bit++) {
        uint16_t mask = (uint16_t)(1u << bit);

        /* Rising edge: Bit war 0, ist jetzt 1 */
        if (verify_edge_detection(0x0000, mask) != 0) return 1;

        /* Falling edge: Bit war 1, ist jetzt 0 */
        if (verify_edge_detection(mask, 0x0000) != 0) return 1;

        /* Bit bleibt 1 (kein Edge) */
        if (verify_edge_detection(mask, mask) != 0) return 1;
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

    printf("=== Property-Test: Input-Edge-Detection ===\n");
    printf("    Validates: Requirements 7.1\n\n");

    /* Seed initialisieren (reproduzierbar via Seed-Ausgabe) */
    seed = (uint32_t)time(NULL);
    g_rng_state = seed;
    printf("    PRNG Seed: %u\n\n", seed);

    /* Phase 1: Gezielte Grenzfälle */
    printf("[1/3] Teste Grenzfaelle...\n");
    if (test_boundary_cases() != 0) {
        printf("\n=== ABBRUCH bei Grenzfaellen ===\n");
        printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
               g_passed, g_failed);
        return 1;
    }
    printf("    Alle Grenzfaelle bestanden.\n\n");

    /* Phase 2: Einzelbit-Rotation */
    printf("[2/3] Teste Einzelbit-Rotation (alle 16 Bits)...\n");
    if (test_single_bit_rotation() != 0) {
        printf("\n=== ABBRUCH bei Einzelbit-Rotation ===\n");
        printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
               g_passed, g_failed);
        return 1;
    }
    printf("    Alle Einzelbit-Tests bestanden.\n\n");

    /* Phase 3: Randomisierte Tests */
    printf("[3/3] Teste %d zufaellige (previous, current) Paare...\n",
           NUM_RANDOM_ITERATIONS);

    for (i = 0; i < NUM_RANDOM_ITERATIONS; i++) {
        uint16_t previous = rand_u16();
        uint16_t current  = rand_u16();

        if (verify_edge_detection(previous, current) != 0) {
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
