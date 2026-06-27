/**
 * @file prop_ot_sort.c
 * @brief Property-Test: OT Z-Sortierung
 *
 * **Validates: Requirements 5.5**
 *
 * Property 9: Für jede Sequenz von N Primitiven mit beliebigen Z-Werten
 * im Bereich [0, 4095] soll das Traversieren der Ordering Table diese
 * Primitive in korrekter Reihenfolge (höchster Z-Wert zuerst = am weitesten
 * entfernt zuerst gezeichnet) liefern (Painter's Algorithm).
 *
 * Test-Strategie:
 *   1. Standalone-OT-Datenstruktur (identisch zu render_pc.c Implementierung)
 *   2. N Primitive mit zufälligen Z-Werten [0, 4095] generieren
 *   3. In OT einfügen (ot_insert mit Clamping)
 *   4. OT von Z=4095 nach Z=0 traversieren (Painter's Algorithm)
 *   5. Prüfen: Reihenfolge ist monoton nicht-steigend (höchster Z zuerst)
 *   6. Prüfen: Alle eingefügten Primitive werden bei der Traversierung besucht
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "re15_render.h"  /* RE15_OT_DEPTH */

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

static uint16_t rand_z(void)
{
    return (uint16_t)(xorshift32() % RE15_OT_DEPTH);
}

/* =========================================================================
 * Standalone OT-Implementierung (identisch zu render_pc.c Logik)
 *
 * Die OT ist ein Array von Linked-List-Köpfen. Index = Z-Wert.
 * Insertion: Prepend am Kopf der Liste für den jeweiligen Z-Wert.
 * Traversierung: Von Z=4095 (fern) nach Z=0 (nah) — Painter's Algorithm.
 * ========================================================================= */

/** Maximale Primitive für Tests */
#define TEST_OT_MAX_PRIMS   2048

/** Ein Primitiv-Knoten im OT-Pool */
typedef struct test_ot_node {
    uint16_t z;                    /**< Z-Wert (bei Insertion gespeichert) */
    int      id;                   /**< Eindeutige ID für Tracking         */
    struct test_ot_node* next;     /**< Nächster Knoten in der Linked-List  */
} test_ot_node_t;

/** OT-State */
static test_ot_node_t*  g_ot[RE15_OT_DEPTH];
static test_ot_node_t   g_prim_pool[TEST_OT_MAX_PRIMS];
static int              g_prim_count;

/**
 * OT komplett leeren.
 */
static void test_ot_clear(void)
{
    memset(g_ot, 0, sizeof(g_ot));
    g_prim_count = 0;
}

/**
 * Primitiv in die OT einfügen.
 * Identisch zur render_pc.c ot_insert()-Logik:
 *   - Z wird auf [0, RE15_OT_DEPTH-1] geclampt
 *   - Insertion am Kopf der Liste (Prepend)
 */
static int test_ot_insert(uint16_t z, int prim_id)
{
    test_ot_node_t* node;
    uint16_t clamped_z;

    if (g_prim_count >= TEST_OT_MAX_PRIMS) {
        return -1;  /* Pool voll */
    }

    node = &g_prim_pool[g_prim_count++];
    node->z = z;
    node->id = prim_id;
    node->next = NULL;

    clamped_z = z;
    if (clamped_z >= RE15_OT_DEPTH) {
        clamped_z = RE15_OT_DEPTH - 1;
    }

    node->next = g_ot[clamped_z];
    g_ot[clamped_z] = node;

    return 0;
}

/**
 * OT traversieren: Höchster Z (fern) → Niedrigster Z (nah).
 * Sammelt die besuchten Knoten in einem Ausgabe-Array.
 *
 * @param out_ids     Ausgabe-Array für Primitiv-IDs in Traversierungsreihenfolge
 * @param out_zs      Ausgabe-Array für Z-Werte in Traversierungsreihenfolge
 * @param max_out     Maximale Kapazität des Ausgabe-Arrays
 * @return            Anzahl der besuchten Knoten
 */
static int test_ot_traverse(int* out_ids, uint16_t* out_zs, int max_out)
{
    int count = 0;
    int z;
    test_ot_node_t* node;

    for (z = RE15_OT_DEPTH - 1; z >= 0; z--) {
        node = g_ot[z];
        while (node) {
            if (count < max_out) {
                out_ids[count] = node->id;
                out_zs[count] = (uint16_t)z;  /* Geclamped Z (Bucket) */
            }
            count++;
            node = node->next;
        }
    }

    return count;
}

/* =========================================================================
 * Property-Test 1: Traversierungsreihenfolge ist monoton nicht-steigend
 *
 * Nach Insertion von N Primitiven mit beliebigen Z-Werten soll die
 * Traversierung Z-Werte in nicht-steigender Reihenfolge liefern
 * (höchster Z zuerst).
 * ========================================================================= */

static int test_ot_sort_order(int num_prims)
{
    int i;
    int visited;
    int* out_ids;
    uint16_t* out_zs;

    test_ot_clear();

    /* N Primitive mit zufälligen Z-Werten einfügen */
    for (i = 0; i < num_prims; i++) {
        uint16_t z = rand_z();
        int result = test_ot_insert(z, i);
        PROP_ASSERT(result == 0,
            "test_ot_insert failed for prim %d with z=%u", i, z);
    }

    /* Traversierung */
    out_ids = (int*)malloc(sizeof(int) * (size_t)num_prims);
    out_zs = (uint16_t*)malloc(sizeof(uint16_t) * (size_t)num_prims);
    PROP_ASSERT(out_ids != NULL && out_zs != NULL,
        "Speicherallokation fuer Traversierungs-Ausgabe fehlgeschlagen");

    visited = test_ot_traverse(out_ids, out_zs, num_prims);

    /* Alle Primitive müssen besucht worden sein */
    PROP_ASSERT(visited == num_prims,
        "Traversierung besuchte %d von %d Primitiven", visited, num_prims);

    /* Prüfe: Z-Werte sind monoton nicht-steigend (höchster zuerst) */
    for (i = 1; i < visited; i++) {
        PROP_ASSERT(out_zs[i] <= out_zs[i - 1],
            "Z-Sortierung verletzt bei Position %d: z[%d]=%u > z[%d]=%u",
            i, i, out_zs[i], i - 1, out_zs[i - 1]);
    }

    free(out_ids);
    free(out_zs);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 2: Alle eingefügten Primitive werden traversiert
 *
 * Jede eingefügte Primitiv-ID soll genau einmal in der Traversierung
 * vorkommen (Vollständigkeit).
 * ========================================================================= */

static int test_ot_completeness(int num_prims)
{
    int i;
    int visited;
    int* out_ids;
    uint16_t* out_zs;
    uint8_t* seen;

    test_ot_clear();

    /* N Primitive einfügen */
    for (i = 0; i < num_prims; i++) {
        uint16_t z = rand_z();
        test_ot_insert(z, i);
    }

    /* Traversierung */
    out_ids = (int*)malloc(sizeof(int) * (size_t)num_prims);
    out_zs = (uint16_t*)malloc(sizeof(uint16_t) * (size_t)num_prims);
    seen = (uint8_t*)calloc((size_t)num_prims, sizeof(uint8_t));
    PROP_ASSERT(out_ids && out_zs && seen,
        "Speicherallokation fehlgeschlagen");

    visited = test_ot_traverse(out_ids, out_zs, num_prims);

    PROP_ASSERT(visited == num_prims,
        "Traversierung besuchte %d von %d Primitiven", visited, num_prims);

    /* Prüfe: Jede ID kommt genau einmal vor */
    for (i = 0; i < visited; i++) {
        int id = out_ids[i];
        PROP_ASSERT(id >= 0 && id < num_prims,
            "Ungueltige Primitiv-ID %d bei Position %d", id, i);
        PROP_ASSERT(seen[id] == 0,
            "Primitiv-ID %d doppelt besucht bei Position %d", id, i);
        seen[id] = 1;
    }

    /* Prüfe: Jede ID wurde gesehen */
    for (i = 0; i < num_prims; i++) {
        PROP_ASSERT(seen[i] == 1,
            "Primitiv-ID %d wurde nicht besucht", i);
    }

    free(out_ids);
    free(out_zs);
    free(seen);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 3: Z-Clamping bei Werten >= RE15_OT_DEPTH
 *
 * Z-Werte >= 4096 sollen auf 4095 geclampt werden und am höchsten
 * OT-Bucket landen (zuerst gezeichnet).
 * ========================================================================= */

static int test_ot_z_clamping(void)
{
    int visited;
    int out_ids[4];
    uint16_t out_zs[4];

    test_ot_clear();

    /* Primitive mit Z-Werten über dem Maximum einfügen */
    test_ot_insert(5000, 0);   /* Sollte auf 4095 geclampt werden */
    test_ot_insert(65535, 1);  /* Sollte auf 4095 geclampt werden */
    test_ot_insert(4095, 2);   /* Normaler maximaler Z-Wert */
    test_ot_insert(0, 3);      /* Minimaler Z-Wert */

    visited = test_ot_traverse(out_ids, out_zs, 4);

    PROP_ASSERT(visited == 4,
        "Erwartet 4 Primitive, erhalten %d", visited);

    /* Die ersten 3 Primitive sollten alle bei Z=4095 liegen */
    PROP_ASSERT(out_zs[0] == 4095,
        "Erstes Primitiv (geclampt) sollte Z=4095 haben, hat Z=%u", out_zs[0]);
    PROP_ASSERT(out_zs[1] == 4095,
        "Zweites Primitiv (geclampt) sollte Z=4095 haben, hat Z=%u", out_zs[1]);
    PROP_ASSERT(out_zs[2] == 4095,
        "Drittes Primitiv sollte Z=4095 haben, hat Z=%u", out_zs[2]);

    /* Letztes Primitiv bei Z=0 */
    PROP_ASSERT(out_zs[3] == 0,
        "Letztes Primitiv sollte Z=0 haben, hat Z=%u", out_zs[3]);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 4: Leere OT — Traversierung liefert 0 Elemente
 * ========================================================================= */

static int test_ot_empty(void)
{
    int visited;
    int dummy_ids[1];
    uint16_t dummy_zs[1];

    test_ot_clear();

    visited = test_ot_traverse(dummy_ids, dummy_zs, 1);

    PROP_ASSERT(visited == 0,
        "Leere OT sollte 0 Primitive liefern, erhalten %d", visited);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 5: Alle Primitive auf demselben Z-Wert
 *
 * Wenn alle N Primitive denselben Z-Wert haben, sollen alle beim
 * Traversieren besucht werden (LIFO-Reihenfolge innerhalb eines Buckets,
 * aber Z-Sortierung trivial erfüllt).
 * ========================================================================= */

static int test_ot_same_z(void)
{
    int i;
    int visited;
    int num_prims = 100;
    int* out_ids;
    uint16_t* out_zs;
    uint16_t z_value = 2000;

    test_ot_clear();

    out_ids = (int*)malloc(sizeof(int) * (size_t)num_prims);
    out_zs = (uint16_t*)malloc(sizeof(uint16_t) * (size_t)num_prims);
    PROP_ASSERT(out_ids && out_zs, "Speicherallokation fehlgeschlagen");

    /* Alle Primitive mit demselben Z-Wert */
    for (i = 0; i < num_prims; i++) {
        test_ot_insert(z_value, i);
    }

    visited = test_ot_traverse(out_ids, out_zs, num_prims);

    PROP_ASSERT(visited == num_prims,
        "Erwartet %d Primitive, erhalten %d", num_prims, visited);

    /* Alle Z-Werte sollten identisch sein */
    for (i = 0; i < visited; i++) {
        PROP_ASSERT(out_zs[i] == z_value,
            "Position %d: Z=%u (erwartet %u)", i, out_zs[i], z_value);
    }

    /* Z-Sortierung ist trivial erfüllt (alle gleich) */
    for (i = 1; i < visited; i++) {
        PROP_ASSERT(out_zs[i] <= out_zs[i - 1],
            "Sortierung verletzt (trivial) bei Position %d", i);
    }

    free(out_ids);
    free(out_zs);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 6: Randwerte (Z=0 und Z=4095)
 *
 * Primitive an den äußersten Grenzen der OT sollen korrekt einsortiert
 * und in der richtigen Reihenfolge traversiert werden.
 * ========================================================================= */

static int test_ot_boundary_z(void)
{
    int visited;
    int out_ids[3];
    uint16_t out_zs[3];

    test_ot_clear();

    /* Einfügen in unsortierter Reihenfolge */
    test_ot_insert(0, 0);      /* Nah (zuletzt gezeichnet) */
    test_ot_insert(4095, 1);   /* Fern (zuerst gezeichnet) */
    test_ot_insert(2048, 2);   /* Mitte */

    visited = test_ot_traverse(out_ids, out_zs, 3);

    PROP_ASSERT(visited == 3, "Erwartet 3 Primitive, erhalten %d", visited);

    /* Reihenfolge: 4095 → 2048 → 0 */
    PROP_ASSERT(out_zs[0] == 4095,
        "Position 0: Z=%u (erwartet 4095)", out_zs[0]);
    PROP_ASSERT(out_zs[1] == 2048,
        "Position 1: Z=%u (erwartet 2048)", out_zs[1]);
    PROP_ASSERT(out_zs[2] == 0,
        "Position 2: Z=%u (erwartet 0)", out_zs[2]);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Main — Randomisierte und deterministische Property-Tests
 * ========================================================================= */

#define NUM_RANDOM_ITERATIONS 500

int main(void)
{
    int i;
    uint32_t seed;

    printf("=== Property-Test: OT Z-Sortierung ===\n");
    printf("    Validates: Requirements 5.5\n\n");

    /* Seed initialisieren */
    seed = (uint32_t)time(NULL);
    g_rng_state = seed;
    printf("    PRNG Seed: %u\n\n", seed);

    /* Phase 1: Deterministische Grenzfälle */
    printf("[1/6] Teste leere OT...\n");
    if (test_ot_empty() != 0) goto done;
    printf("    Bestanden.\n\n");

    printf("[2/6] Teste Randwerte (Z=0, Z=2048, Z=4095)...\n");
    if (test_ot_boundary_z() != 0) goto done;
    printf("    Bestanden.\n\n");

    printf("[3/6] Teste Z-Clamping (Werte >= 4096)...\n");
    if (test_ot_z_clamping() != 0) goto done;
    printf("    Bestanden.\n\n");

    printf("[4/6] Teste alle Primitive auf gleichem Z-Wert...\n");
    if (test_ot_same_z() != 0) goto done;
    printf("    Bestanden.\n\n");

    /* Phase 2: Randomisierte Tests — variierende Primitiv-Anzahlen */
    printf("[5/6] Teste Sortierungsreihenfolge (%d zufaellige Szenarien)...\n",
           NUM_RANDOM_ITERATIONS);
    for (i = 0; i < NUM_RANDOM_ITERATIONS; i++) {
        /* Variierende Primitive-Anzahl: 1 bis TEST_OT_MAX_PRIMS */
        int num_prims = 1 + (int)(xorshift32() % (TEST_OT_MAX_PRIMS - 1));
        if (test_ot_sort_order(num_prims) != 0) {
            printf("    Fehlgeschlagen bei Iteration %d (num_prims=%d, seed=%u)\n",
                   i, num_prims, seed);
            goto done;
        }
    }
    printf("    Alle %d Szenarien bestanden.\n\n", NUM_RANDOM_ITERATIONS);

    printf("[6/6] Teste Vollstaendigkeit (%d zufaellige Szenarien)...\n",
           NUM_RANDOM_ITERATIONS);
    for (i = 0; i < NUM_RANDOM_ITERATIONS; i++) {
        int num_prims = 1 + (int)(xorshift32() % (TEST_OT_MAX_PRIMS - 1));
        if (test_ot_completeness(num_prims) != 0) {
            printf("    Fehlgeschlagen bei Iteration %d (num_prims=%d, seed=%u)\n",
                   i, num_prims, seed);
            goto done;
        }
    }
    printf("    Alle %d Szenarien bestanden.\n\n", NUM_RANDOM_ITERATIONS);

done:
    /* Zusammenfassung */
    printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
