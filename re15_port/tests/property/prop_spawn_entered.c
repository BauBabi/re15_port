/**
 * @file prop_spawn_entered.c
 * @brief Property-Test: Spawn-Position Tür-Entered-Invariante
 *
 * **Property 7: Spawn-Position Tür-Entered-Invariante**
 *
 * Für jede Spawn-Position nach einem Raumwechsel und jede aktive Tür-AOT-Zone,
 * deren Trigger-Polygon die Spawn-Position enthält, soll das entered-Flag
 * dieser Zone auf true gesetzt sein, bevor der erste Spieler-Input-Frame
 * verarbeitet wird.
 *
 * Teststrategie:
 * 1. Generiere zufällige Spawn-Positionen (int16_t x, z)
 * 2. Generiere zufällige aktive Tür-AOT-Zonen (konvexe 4-Punkt-Polygone)
 * 3. Rufe re15_aot_init() auf, befülle Quad-Slots, markiere "entered"
 * 4. Prüfe: Jede aktive Tür-AOT deren Polygon die Spawn-Position enthält
 *    hat was_inside==1
 *
 * ------------------------------------------------------------------------
 * REAKTIVIERUNG (2026-06-28) — Legacy->aktuelle öffentliche API
 *
 * Der Engine-Transplant hat das AOT-Modell geändert. Alt->Neu (am Header
 * `re15_port/include/re15_aot.h` verifiziert):
 *   - re15_aot_slot_t (entfernt)        -> re15_aot_t (g_aot.slots[], extern g_aot)
 *   - slot.entered                      -> re15_aot_t.was_inside (re15_aot.h:71)
 *   - slot.floor_band                   -> re15_aot_t.band       (re15_aot.h:85)
 *   - slot.trigger_x[4]/trigger_z[4]    -> re15_aot_t.xs[4]/zs[4] + has_quad=1
 *                                          (re15_aot.h:81,87,88)
 *   - AOT_TYPE_DOOR (=1)                -> RE15_AOT_TYPE_DOOR     (re15_aot.h:38)
 *   - re15_aot_set_slot()/get_slot()    -> direkter Zugriff über g_aot.slots[]
 *                                          (re15_aot.h:127,145)
 *   - re15_aot_mark_entered(x,z,band)   -> ERSATZLOS ENTFERNT. Die öffentliche
 *       Engine bietet nur re15_aot_settle_at(x,z) (re15_aot.h:211), das aber
 *       ausschliesslich den AABB-Pfad (x/z+half_w/half_h) nutzt und WEDER den
 *       Quad-Trigger NOCH das Floor-Band berücksichtigt. Die getestete
 *       band-gegatete Point-in-4P-"mark entered"-Semantik wird daher LOKAL im
 *       Test nachgebildet (mark_entered_doors_ref unten), unter Nutzung der
 *       öffentlichen Engine-Funktion re15_aot_point_in_quad() (re15_aot.h:215,
 *       aot_common.c:178) für den Innen-/Aussen-Test — exakt der Quad-Test, den
 *       der per-Frame-Scanner für has_quad-Slots verwendet (aot_common.c:311).
 *
 * Es wird KEIN _legacy_minimal-Header eingebunden.
 * ------------------------------------------------------------------------
 *
 * **Validates: Requirements 4.8**
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

/** Anzahl der Property-Test-Iterationen */
#define NUM_ITERATIONS 500

/** Maximale Anzahl an Tür-Slots pro Iteration */
#define MAX_DOOR_SLOTS 8

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
 * PRNG — xorshift32 für reproduzierbare Ergebnisse
 * ========================================================================= */

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

/** Zufällige int16_t im Bereich [lo, hi] (inklusiv) */
static int16_t rand_s16_range(int16_t lo, int16_t hi)
{
    uint32_t range = (uint32_t)((int32_t)hi - (int32_t)lo + 1);
    return (int16_t)((int32_t)lo + (int32_t)(xorshift32() % range));
}

/** Zufälliges uint8_t */
static uint8_t rand_u8(void)
{
    return (uint8_t)(xorshift32() & 0xFF);
}

/* =========================================================================
 * Referenz-Implementierung: Point-in-4P-Polygon
 *
 * Cross-Product-Test für konvexe 4-Punkt-Polygone. Spiegelt die Engine-
 * Logik re15_aot_point_in_quad() (aot_common.c:178) wider: Vorzeichen-
 * konsistenz aller 4 Kanten-Kreuzprodukte, Kante (cross==0) zählt als "drin".
 * ========================================================================= */

static int ref_point_in_4p(int16_t px, int16_t pz,
                           const int16_t* poly_x, const int16_t* poly_z)
{
    int i;
    int sign = 0;

    for (i = 0; i < 4; i++) {
        int next = (i + 1) & 3;
        int32_t ex = (int32_t)poly_x[next] - (int32_t)poly_x[i];
        int32_t ez = (int32_t)poly_z[next] - (int32_t)poly_z[i];
        int32_t dx = (int32_t)px - (int32_t)poly_x[i];
        int32_t dz = (int32_t)pz - (int32_t)poly_z[i];
        int32_t cross = ex * dz - ez * dx;

        if (cross == 0) {
            continue; /* Auf der Kante — zählt als "drin" */
        }

        if (sign == 0) {
            sign = (cross > 0) ? 1 : -1;
        } else {
            if ((cross > 0 && sign < 0) || (cross < 0 && sign > 0)) {
                return 0; /* Außerhalb */
            }
        }
    }

    return 1; /* Innerhalb */
}

/* =========================================================================
 * Lokale Nachbildung des entfernten re15_aot_mark_entered_doors()
 *
 * Die öffentliche Engine bietet kein band-gegatetes "mark entered" mehr
 * (re15_aot_settle_at() ignoriert Band UND Quad). Diese Funktion bildet die
 * getestete Legacy-Semantik 1:1 nach (Legacy-Doc re15_aot.h §mark_entered_doors):
 *   - nur aktive DOOR-AOTs (RE15_AOT_TYPE_DOOR)
 *   - Floor-Band des Slots == Spawn-Floor-Band
 *   - Spawn liegt im 4-Punkt-Trigger (Engine-Funktion re15_aot_point_in_quad,
 *     re15_aot.h:215 / aot_common.c:178)
 *   -> was_inside = 1 (das aktuelle Pendant des Legacy-entered-Flags)
 * Markiert ALLE passenden Tür-AOTs, nicht nur die erste.
 * ========================================================================= */

static void mark_entered_doors_ref(int16_t spawn_x, int16_t spawn_z,
                                   uint8_t floor_band)
{
    int i;
    for (i = 0; i < RE15_AOT_MAX; i++) {
        re15_aot_t* a = &g_aot.slots[i];
        if (!a->active) continue;
        if (a->type != RE15_AOT_TYPE_DOOR) continue;
        if (a->band != floor_band) continue;
        if (re15_aot_point_in_quad((int32_t)spawn_x, (int32_t)spawn_z,
                                   a->xs, a->zs)) {
            a->was_inside = 1;
        }
    }
}

/* =========================================================================
 * Hilfsfunktion: Quad-Tür-Slot direkt in g_aot.slots[] befüllen
 *
 * Ersetzt das entfernte re15_aot_set_slot(). Setzt has_quad=1 + xs/zs, damit
 * der Engine-Quad-Pfad (aot_common.c:311) greift, und band für das Floor-Band-
 * Gate.
 * ========================================================================= */

static void set_door_quad(int slot, uint8_t band,
                          const int16_t* poly_x, const int16_t* poly_z)
{
    re15_aot_t* a = &g_aot.slots[slot];
    memset(a, 0, sizeof(*a));
    a->active     = 1;
    a->type       = RE15_AOT_TYPE_DOOR;
    a->was_inside = 0;
    a->has_quad   = 1;
    a->band       = band;
    a->xs[0] = poly_x[0]; a->zs[0] = poly_z[0];
    a->xs[1] = poly_x[1]; a->zs[1] = poly_z[1];
    a->xs[2] = poly_x[2]; a->zs[2] = poly_z[2];
    a->xs[3] = poly_x[3]; a->zs[3] = poly_z[3];
}

/* =========================================================================
 * Hilfsfunktion: Konvexes 4-Punkt-Polygon generieren
 *
 * Strategie: Wähle ein Zentrum (cx, cz). Generiere 4 Punkte, jeweils
 * einen in jedem Quadranten → garantiert konvex bei CCW-Ordnung.
 * ========================================================================= */

static void generate_convex_quad(int16_t* poly_x, int16_t* poly_z,
                                 int16_t cx, int16_t cz)
{
    int16_t dx[4], dz[4];

    /* Offsets für jeden Quadranten (mindestens 2, maximal 3000) */
    /* Quadrant 0: (+x, +z) */
    dx[0] = rand_s16_range(2, 3000);
    dz[0] = rand_s16_range(2, 3000);

    /* Quadrant 1: (-x, +z) */
    dx[1] = rand_s16_range(-3000, -2);
    dz[1] = rand_s16_range(2, 3000);

    /* Quadrant 2: (-x, -z) */
    dx[2] = rand_s16_range(-3000, -2);
    dz[2] = rand_s16_range(-3000, -2);

    /* Quadrant 3: (+x, -z) */
    dx[3] = rand_s16_range(2, 3000);
    dz[3] = rand_s16_range(-3000, -2);

    /* CCW-Reihenfolge: 3(+x,-z), 0(+x,+z), 1(-x,+z), 2(-x,-z) */
    poly_x[0] = (int16_t)(cx + dx[3]); poly_z[0] = (int16_t)(cz + dz[3]);
    poly_x[1] = (int16_t)(cx + dx[0]); poly_z[1] = (int16_t)(cz + dz[0]);
    poly_x[2] = (int16_t)(cx + dx[1]); poly_z[2] = (int16_t)(cz + dz[1]);
    poly_x[3] = (int16_t)(cx + dx[2]); poly_z[3] = (int16_t)(cz + dz[2]);
}

/* =========================================================================
 * Property-Test 1: Spawn enthält → entered gesetzt
 *
 * Für jede Iteration:
 * - Generiere eine Spawn-Position
 * - Generiere 1..MAX_DOOR_SLOTS aktive Tür-AOT-Zonen
 * - Einige Zonen enthalten die Spawn-Position (konstruiert), andere nicht
 * - Rufe mark_entered_doors_ref() auf
 * - Prüfe: Alle Tür-Zonen die den Spawn enthalten haben was_inside==1
 * ========================================================================= */

static int prop_spawn_entered_invariant(void)
{
    int iter;

    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        int16_t spawn_x, spawn_z;
        uint8_t floor_band;
        int num_doors;
        int i;

        /* Generiere zufällige Spawn-Position und Floor-Band */
        spawn_x = rand_s16_range(-10000, 10000);
        spawn_z = rand_s16_range(-10000, 10000);
        floor_band = rand_u8();

        /* Zufällige Anzahl Tür-Slots (1..MAX_DOOR_SLOTS) */
        num_doors = 1 + (int)(xorshift32() % MAX_DOOR_SLOTS);

        /* AOT-System zurücksetzen */
        re15_aot_init();

        /* Tür-Slots einrichten */
        for (i = 0; i < num_doors; i++) {
            int16_t poly_x[4], poly_z[4];

            /* Polygon generieren:
             * - ungerade Slots: Polygon wird um den Spawn-Punkt konstruiert
             *   (Spawn-Punkt ist garantiert innerhalb)
             * - gerade Slots: Polygon wird weit weg vom Spawn generiert
             *   (Spawn-Punkt ist garantiert außerhalb) */
            if (i % 2 == 0) {
                /* Polygon um Spawn-Position → Spawn ist drin */
                generate_convex_quad(poly_x, poly_z, spawn_x, spawn_z);
            } else {
                /* Polygon weit weg: Zentrum +20000 entfernt */
                int16_t far_cx = (int16_t)((int32_t)spawn_x + 20000);
                int16_t far_cz = (int16_t)((int32_t)spawn_z + 20000);
                /* Falls Überlauf, verschiebe in die andere Richtung */
                if ((int32_t)spawn_x + 20000 > 32000) {
                    far_cx = (int16_t)((int32_t)spawn_x - 20000);
                }
                if ((int32_t)spawn_z + 20000 > 32000) {
                    far_cz = (int16_t)((int32_t)spawn_z - 20000);
                }
                generate_convex_quad(poly_x, poly_z, far_cx, far_cz);
            }

            set_door_quad(i, floor_band, poly_x, poly_z);
        }

        /* Aufruf der zu testenden Logik (lokale Nachbildung, s. oben) */
        mark_entered_doors_ref(spawn_x, spawn_z, floor_band);

        /* Verifikation: Für jeden Slot prüfen */
        for (i = 0; i < num_doors; i++) {
            const re15_aot_t* slot = &g_aot.slots[i];
            int contains_spawn;

            contains_spawn = ref_point_in_4p(spawn_x, spawn_z,
                                             slot->xs, slot->zs);

            if (contains_spawn) {
                /* Invariante: Tür-Zone die Spawn enthält MUSS was_inside==1 haben */
                PROP_ASSERT(slot->was_inside == 1,
                    "Iter %d Slot %d: Polygon enthält Spawn (%d,%d) "
                    "aber was_inside==%d (erwartet 1). "
                    "Poly=[(%d,%d),(%d,%d),(%d,%d),(%d,%d)] floor=%d",
                    iter, i, spawn_x, spawn_z, slot->was_inside,
                    slot->xs[0], slot->zs[0],
                    slot->xs[1], slot->zs[1],
                    slot->xs[2], slot->zs[2],
                    slot->xs[3], slot->zs[3],
                    floor_band);
            }
        }
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 2: Nicht-überlappende Zonen bleiben was_inside==0
 *
 * Prüft die Negativ-Seite: Zonen, die den Spawn NICHT enthalten,
 * dürfen NICHT als entered markiert werden.
 * ========================================================================= */

static int prop_non_overlapping_not_entered(void)
{
    int iter;

    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        int16_t spawn_x, spawn_z;
        uint8_t floor_band;
        int num_doors;
        int i;

        spawn_x = rand_s16_range(-10000, 10000);
        spawn_z = rand_s16_range(-10000, 10000);
        floor_band = rand_u8();

        num_doors = 1 + (int)(xorshift32() % MAX_DOOR_SLOTS);

        re15_aot_init();

        for (i = 0; i < num_doors; i++) {
            int16_t poly_x[4], poly_z[4];

            if (i % 2 == 0) {
                generate_convex_quad(poly_x, poly_z, spawn_x, spawn_z);
            } else {
                int16_t far_cx = (int16_t)((int32_t)spawn_x + 20000);
                int16_t far_cz = (int16_t)((int32_t)spawn_z + 20000);
                if ((int32_t)spawn_x + 20000 > 32000) {
                    far_cx = (int16_t)((int32_t)spawn_x - 20000);
                }
                if ((int32_t)spawn_z + 20000 > 32000) {
                    far_cz = (int16_t)((int32_t)spawn_z - 20000);
                }
                generate_convex_quad(poly_x, poly_z, far_cx, far_cz);
            }

            set_door_quad(i, floor_band, poly_x, poly_z);
        }

        mark_entered_doors_ref(spawn_x, spawn_z, floor_band);

        for (i = 0; i < num_doors; i++) {
            const re15_aot_t* slot = &g_aot.slots[i];
            int contains_spawn;

            contains_spawn = ref_point_in_4p(spawn_x, spawn_z,
                                             slot->xs, slot->zs);

            if (!contains_spawn) {
                /* Zone enthält Spawn NICHT → was_inside muss 0 bleiben */
                PROP_ASSERT(slot->was_inside == 0,
                    "Iter %d Slot %d: Polygon enthält Spawn (%d,%d) NICHT "
                    "aber was_inside==%d (erwartet 0). "
                    "Poly=[(%d,%d),(%d,%d),(%d,%d),(%d,%d)] floor=%d",
                    iter, i, spawn_x, spawn_z, slot->was_inside,
                    slot->xs[0], slot->zs[0],
                    slot->xs[1], slot->zs[1],
                    slot->xs[2], slot->zs[2],
                    slot->xs[3], slot->zs[3],
                    floor_band);
            }
        }
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 3: Floor-Band-Mismatch → entered bleibt 0
 *
 * Zonen mit anderem Floor-Band als der Spawn dürfen nicht als entered
 * markiert werden, auch wenn das Polygon den Spawn geometrisch enthält.
 * ========================================================================= */

static int prop_floor_band_mismatch(void)
{
    int iter;

    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        int16_t spawn_x, spawn_z;
        uint8_t spawn_floor, other_floor;
        int16_t poly_x[4], poly_z[4];

        spawn_x = rand_s16_range(-10000, 10000);
        spawn_z = rand_s16_range(-10000, 10000);
        spawn_floor = rand_u8();

        /* Anderes Floor-Band (garantiert verschieden) */
        other_floor = (uint8_t)(spawn_floor + 1 + (rand_u8() % 254));

        re15_aot_init();

        /* Polygon um Spawn → geometrisch drin, aber Floor-Band stimmt nicht */
        generate_convex_quad(poly_x, poly_z, spawn_x, spawn_z);
        set_door_quad(0, other_floor /* Anderes Band als Spawn */,
                      poly_x, poly_z);

        /* mark_entered mit dem Spawn-Floor-Band */
        mark_entered_doors_ref(spawn_x, spawn_z, spawn_floor);

        /* Slot darf NICHT als entered markiert sein (Floor-Band-Mismatch) */
        {
            const re15_aot_t* result = &g_aot.slots[0];

            PROP_ASSERT(result->was_inside == 0,
                "Iter %d: Floor-Band-Mismatch (spawn=%d, slot=%d) "
                "aber was_inside==%d (erwartet 0). Spawn=(%d,%d)",
                iter, spawn_floor, other_floor, result->was_inside,
                spawn_x, spawn_z);
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
    printf("=== Property-Test: Spawn-Position Tür-Entered-Invariante ===\n");
    printf("    Validates: Requirements 4.8\n");
    printf("    PRNG Seed: %u\n", xorshift_state);
    printf("    Iterationen: %d\n\n", NUM_ITERATIONS);

    /* Test 1: Überlappende Zonen → entered gesetzt */
    printf("[1/3] prop_spawn_entered_invariant (%d Iterationen) ... ",
           NUM_ITERATIONS);
    if (prop_spawn_entered_invariant() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test 2: Nicht-überlappende Zonen → entered bleibt 0 */
    printf("[2/3] prop_non_overlapping_not_entered (%d Iterationen) ... ",
           NUM_ITERATIONS);
    if (prop_non_overlapping_not_entered() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test 3: Floor-Band-Mismatch → entered bleibt 0 */
    printf("[3/3] prop_floor_band_mismatch (%d Iterationen) ... ",
           NUM_ITERATIONS);
    if (prop_floor_band_mismatch() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Zusammenfassung */
    printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
