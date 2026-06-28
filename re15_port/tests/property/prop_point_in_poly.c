/**
 * @file prop_point_in_poly.c
 * @brief Property-Test: Point-in-4P-Polygon Korrektheit
 *
 * **Property 5: Point-in-4P-Polygon Korrektheit**
 *
 * Für jede Spielerposition (x, z) und jedes konvexe 4-Punkt-Polygon
 * (trigger_x[4], trigger_z[4]) soll die Containment-Prüfung genau dann
 * `true` liefern, wenn der Punkt geometrisch innerhalb des Polygons liegt
 * (Cross-Product-Test aller 4 Kanten, konsistentes Vorzeichen).
 *
 * Testansatz:
 * 1. Generiere zufällige achsenparallele Rechtecke (immer konvex)
 * 2. Generiere zufällige konvexe 4-Punkt-Polygone
 * 3. Für jedes Polygon: teste bekannte Innen-/Außen-/Kantenpunkte
 * 4. Verifiziere gegen geometrisches Wissen (Kreuzprodukt-Referenz)
 *
 * Da point_in_4p statisch in aot_common.c ist, testen wir indirekt über
 * re15_aot_check() mit konfiguriertem Slot.
 *
 * **Validates: Requirements 4.2, 11.5, 11.6**
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

/** Anzahl der Iterationen für zufällige Rechteck-Tests */
#define NUM_RECT_ITERATIONS 500

/** Anzahl der Iterationen für zufällige konvexe Quad-Tests */
#define NUM_CONVEX_ITERATIONS 500

/** Floor-Band für alle Tests (beliebiger fester Wert) */
#define TEST_FLOOR_BAND 0

/** AOT-Slot-Index für Tests */
#define TEST_SLOT_IDX 0

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

/** Zufällige int16_t im vollen Bereich */
static int16_t rand_s16(void)
{
    return (int16_t)(xorshift32() & 0xFFFF);
}

/** Zufällige int16_t im Bereich [lo, hi] (inklusiv) */
static int16_t rand_s16_range(int16_t lo, int16_t hi)
{
    uint32_t range = (uint32_t)((int32_t)hi - (int32_t)lo + 1);
    return (int16_t)((int32_t)lo + (int32_t)(xorshift32() % range));
}

/* =========================================================================
 * Referenz-Implementierung: Cross-Product Point-in-Convex-4P
 *
 * Identisch zur Logik in aot_common.c, aber als eigenständige Referenz.
 * Ein Punkt liegt innerhalb eines konvexen Polygons, wenn die Kreuzprodukte
 * (Kante × Punkt-zu-Vertex) für alle 4 Kanten konsistentes Vorzeichen haben.
 * Kreuzprodukt == 0 bedeutet "auf der Kante" und zählt als innerhalb.
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
 * Hilfsfunktion: AOT-Slot konfigurieren und re15_aot_check aufrufen
 *
 * Richtet einen einzelnen aktiven AOT-Slot mit dem gegebenen Polygon ein
 * und prüft, ob der Punkt darin erkannt wird.
 * ========================================================================= */

static int aot_check_point(int16_t px, int16_t pz,
                           const int16_t* poly_x, const int16_t* poly_z)
{
    /* Direkt die ÖFFENTLICHE Point-in-Quad-Funktion testen (aot_common.c,
     * PSX FUN_80014368, int64-Kreuzprodukte). Die alte slot-basierte API
     * (re15_aot_slot_t / re15_aot_set_slot / re15_aot_check) existiert nach
     * dem Engine-Transplant nicht mehr — re15_aot_point_in_quad ist die
     * exakt gleiche „consistent-sign, cross==0 = innen"-Logik wie die
     * Referenz ref_point_in_4p, nur mit int64 statt int32. */
    return re15_aot_point_in_quad((int32_t)px, (int32_t)pz, poly_x, poly_z);
}

/* =========================================================================
 * Property-Test 1: Achsenparallele Rechtecke
 *
 * Generiere zufällige Rechtecke (immer konvex) mit min_x < max_x,
 * min_z < max_z. Teste:
 * - Mittelpunkt: muss innerhalb sein
 * - Ecken: müssen innerhalb sein (auf der Kante)
 * - Punkte weit außerhalb: müssen außerhalb sein
 * - Referenz stimmt mit AOT-Prüfung überein
 * ========================================================================= */

static int prop_rect_containment(void)
{
    int iter;

    for (iter = 0; iter < NUM_RECT_ITERATIONS; iter++) {
        int16_t x0, x1, z0, z1;
        int16_t mid_x, mid_z;
        int16_t poly_x[4], poly_z[4];
        int ref_result, aot_result;
        int16_t test_px, test_pz;

        /* Generiere Rechteck mit Mindestgröße 4 in jeder Dimension */
        x0 = rand_s16_range(-16000, 16000);
        x1 = rand_s16_range(x0 + 4, (int16_t)(x0 + 4 + (int16_t)(xorshift32() % 2000)));
        z0 = rand_s16_range(-16000, 16000);
        z1 = rand_s16_range(z0 + 4, (int16_t)(z0 + 4 + (int16_t)(xorshift32() % 2000)));

        /* Clamp auf int16_t-Bereich */
        if (x1 > 32000) x1 = 32000;
        if (z1 > 32000) z1 = 32000;

        /* Rechteck als 4-Punkt-Polygon (CCW-Reihenfolge) */
        poly_x[0] = x0; poly_z[0] = z0;  /* unten-links  */
        poly_x[1] = x1; poly_z[1] = z0;  /* unten-rechts */
        poly_x[2] = x1; poly_z[2] = z1;  /* oben-rechts  */
        poly_x[3] = x0; poly_z[3] = z1;  /* oben-links   */

        /* Test 1: Mittelpunkt muss drin sein */
        mid_x = (int16_t)(((int32_t)x0 + (int32_t)x1) / 2);
        mid_z = (int16_t)(((int32_t)z0 + (int32_t)z1) / 2);

        ref_result = ref_point_in_4p(mid_x, mid_z, poly_x, poly_z);
        aot_result = aot_check_point(mid_x, mid_z, poly_x, poly_z);

        PROP_ASSERT(ref_result == 1,
            "Iter %d: Mittelpunkt (%d,%d) nicht in Referenz als 'drin' erkannt "
            "(Rect [%d,%d]-[%d,%d])",
            iter, mid_x, mid_z, x0, z0, x1, z1);

        PROP_ASSERT(aot_result == ref_result,
            "Iter %d: AOT (%d) != Ref (%d) für Mittelpunkt (%d,%d) "
            "(Rect [%d,%d]-[%d,%d])",
            iter, aot_result, ref_result, mid_x, mid_z, x0, z0, x1, z1);

        /* Test 2: Punkt klar außerhalb (links vom Rechteck) */
        test_px = (int16_t)(x0 - 100);
        test_pz = mid_z;
        if (test_px > -32000) { /* Überlauf vermeiden */
            ref_result = ref_point_in_4p(test_px, test_pz, poly_x, poly_z);
            aot_result = aot_check_point(test_px, test_pz, poly_x, poly_z);

            PROP_ASSERT(ref_result == 0,
                "Iter %d: Punkt außerhalb (%d,%d) fälschlich als 'drin' (Ref) "
                "(Rect [%d,%d]-[%d,%d])",
                iter, test_px, test_pz, x0, z0, x1, z1);

            PROP_ASSERT(aot_result == ref_result,
                "Iter %d: AOT (%d) != Ref (%d) für außerhalb (%d,%d) "
                "(Rect [%d,%d]-[%d,%d])",
                iter, aot_result, ref_result, test_px, test_pz, x0, z0, x1, z1);
        }

        /* Test 3: Zufälliger Punkt — Referenz und AOT müssen übereinstimmen */
        test_px = rand_s16();
        test_pz = rand_s16();
        ref_result = ref_point_in_4p(test_px, test_pz, poly_x, poly_z);
        aot_result = aot_check_point(test_px, test_pz, poly_x, poly_z);

        PROP_ASSERT(aot_result == ref_result,
            "Iter %d: AOT (%d) != Ref (%d) für zufälligen Punkt (%d,%d) "
            "(Rect [%d,%d]-[%d,%d])",
            iter, aot_result, ref_result, test_px, test_pz, x0, z0, x1, z1);
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 2: Zufällige konvexe 4-Punkt-Polygone
 *
 * Generierung konvexer Quads:
 * - Starte mit einem Zentrum und 4 Richtungen (Quadranten)
 * - Erzeuge 4 Punkte in je einem Quadranten → garantiert konvex
 * - Teste zufällige Punkte: AOT und Referenz müssen übereinstimmen
 * ========================================================================= */

/**
 * Prüft ob ein 4-Punkt-Polygon konvex ist (alle Kreuzprodukte
 * aufeinanderfolgender Kanten haben konsistentes Vorzeichen).
 */
static int is_convex_quad(const int16_t* poly_x, const int16_t* poly_z)
{
    int i;
    int sign = 0;

    for (i = 0; i < 4; i++) {
        int next  = (i + 1) & 3;
        int next2 = (i + 2) & 3;
        int32_t e1x = (int32_t)poly_x[next]  - (int32_t)poly_x[i];
        int32_t e1z = (int32_t)poly_z[next]  - (int32_t)poly_z[i];
        int32_t e2x = (int32_t)poly_x[next2] - (int32_t)poly_x[next];
        int32_t e2z = (int32_t)poly_z[next2] - (int32_t)poly_z[next];
        int32_t cross = e1x * e2z - e1z * e2x;

        if (cross == 0) continue; /* Kollineare Kanten → degeneriert */

        if (sign == 0) {
            sign = (cross > 0) ? 1 : -1;
        } else {
            if ((cross > 0 && sign < 0) || (cross < 0 && sign > 0)) {
                return 0; /* Nicht konvex */
            }
        }
    }

    return 1; /* Konvex */
}

/**
 * Generiert ein konvexes 4-Punkt-Polygon.
 *
 * Strategie: Wähle ein Zentrum (cx, cz). Generiere 4 Punkte,
 * jeweils einen in jedem Quadranten relativ zum Zentrum.
 * Ordne die Punkte CCW (Quadrant I, II, III, IV → ergibt CCW-Polygon).
 *
 * Quadrant-Zuordnung (relativ zum Zentrum):
 *   0: (+x, +z) → oben-rechts
 *   1: (-x, +z) → oben-links
 *   2: (-x, -z) → unten-links
 *   3: (+x, -z) → unten-rechts
 *
 * CCW-Reihenfolge: 3, 0, 1, 2 (unten-rechts → oben-rechts → oben-links → unten-links)
 */
static void generate_convex_quad(int16_t* poly_x, int16_t* poly_z,
                                 int16_t* out_cx, int16_t* out_cz)
{
    int16_t cx, cz;
    int16_t dx[4], dz[4];

    do {
        /* Zentrum im sicheren Bereich wählen */
        cx = rand_s16_range(-10000, 10000);
        cz = rand_s16_range(-10000, 10000);

        /* Offsets für jeden Quadranten (mindestens 2, maximal 5000) */
        /* Quadrant 0: (+x, +z) */
        dx[0] = rand_s16_range(2, 5000);
        dz[0] = rand_s16_range(2, 5000);

        /* Quadrant 1: (-x, +z) */
        dx[1] = rand_s16_range(-5000, -2);
        dz[1] = rand_s16_range(2, 5000);

        /* Quadrant 2: (-x, -z) */
        dx[2] = rand_s16_range(-5000, -2);
        dz[2] = rand_s16_range(-5000, -2);

        /* Quadrant 3: (+x, -z) */
        dx[3] = rand_s16_range(2, 5000);
        dz[3] = rand_s16_range(-5000, -2);

        /* CCW-Reihenfolge: 3 (unten-rechts), 0 (oben-rechts), 1 (oben-links), 2 (unten-links) */
        poly_x[0] = (int16_t)(cx + dx[3]); poly_z[0] = (int16_t)(cz + dz[3]);
        poly_x[1] = (int16_t)(cx + dx[0]); poly_z[1] = (int16_t)(cz + dz[0]);
        poly_x[2] = (int16_t)(cx + dx[1]); poly_z[2] = (int16_t)(cz + dz[1]);
        poly_x[3] = (int16_t)(cx + dx[2]); poly_z[3] = (int16_t)(cz + dz[2]);
    } while (!is_convex_quad(poly_x, poly_z));

    *out_cx = cx;
    *out_cz = cz;
}

static int prop_convex_quad_containment(void)
{
    int iter;

    for (iter = 0; iter < NUM_CONVEX_ITERATIONS; iter++) {
        int16_t poly_x[4], poly_z[4];
        int16_t cx, cz;
        int16_t test_px, test_pz;
        int ref_result, aot_result;

        generate_convex_quad(poly_x, poly_z, &cx, &cz);

        /* Test 1: Zentrum muss drin sein (Zentrum ist per Konstruktion innen) */
        ref_result = ref_point_in_4p(cx, cz, poly_x, poly_z);
        aot_result = aot_check_point(cx, cz, poly_x, poly_z);

        PROP_ASSERT(ref_result == 1,
            "Iter %d: Zentrum (%d,%d) nicht als 'drin' erkannt (Ref) "
            "Poly=[(%d,%d),(%d,%d),(%d,%d),(%d,%d)]",
            iter, cx, cz,
            poly_x[0], poly_z[0], poly_x[1], poly_z[1],
            poly_x[2], poly_z[2], poly_x[3], poly_z[3]);

        PROP_ASSERT(aot_result == ref_result,
            "Iter %d: AOT (%d) != Ref (%d) für Zentrum (%d,%d)",
            iter, aot_result, ref_result, cx, cz);

        /* Test 2: Zufälliger Punkt — AOT und Referenz müssen übereinstimmen */
        test_px = rand_s16();
        test_pz = rand_s16();
        ref_result = ref_point_in_4p(test_px, test_pz, poly_x, poly_z);
        aot_result = aot_check_point(test_px, test_pz, poly_x, poly_z);

        PROP_ASSERT(aot_result == ref_result,
            "Iter %d: AOT (%d) != Ref (%d) für Punkt (%d,%d) "
            "Poly=[(%d,%d),(%d,%d),(%d,%d),(%d,%d)]",
            iter, aot_result, ref_result, test_px, test_pz,
            poly_x[0], poly_z[0], poly_x[1], poly_z[1],
            poly_x[2], poly_z[2], poly_x[3], poly_z[3]);

        /* Test 3: Punkt sehr weit außerhalb (großer Abstand vom Zentrum) */
        test_px = (int16_t)(cx + 20000);
        test_pz = (int16_t)(cz + 20000);
        /* Nur wenn kein Überlauf */
        if ((int32_t)cx + 20000 < 32767 && (int32_t)cz + 20000 < 32767) {
            ref_result = ref_point_in_4p(test_px, test_pz, poly_x, poly_z);
            aot_result = aot_check_point(test_px, test_pz, poly_x, poly_z);

            PROP_ASSERT(ref_result == 0,
                "Iter %d: Weit-außen-Punkt (%d,%d) fälschlich als 'drin' (Ref)",
                iter, test_px, test_pz);

            PROP_ASSERT(aot_result == ref_result,
                "Iter %d: AOT (%d) != Ref (%d) für Weit-außen (%d,%d)",
                iter, aot_result, ref_result, test_px, test_pz);
        }
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 3: Kantenpunkte (On-Edge) zählen als "drin"
 *
 * Punkte exakt auf einer Polygon-Kante sollen als innerhalb erkannt
 * werden (cross == 0 → continue, kein Vorzeichenwechsel).
 * ========================================================================= */

static int prop_edge_points_inside(void)
{
    int iter;

    for (iter = 0; iter < NUM_RECT_ITERATIONS; iter++) {
        int16_t x0, x1, z0, z1;
        int16_t poly_x[4], poly_z[4];
        int16_t edge_x, edge_z;
        int ref_result, aot_result;
        int32_t t_num;

        /* Generiere Rechteck */
        x0 = rand_s16_range(-16000, 16000);
        x1 = rand_s16_range((int16_t)(x0 + 10), (int16_t)(x0 + 2000));
        z0 = rand_s16_range(-16000, 16000);
        z1 = rand_s16_range((int16_t)(z0 + 10), (int16_t)(z0 + 2000));

        if (x1 > 32000) x1 = 32000;
        if (z1 > 32000) z1 = 32000;

        /* Rechteck CCW */
        poly_x[0] = x0; poly_z[0] = z0;
        poly_x[1] = x1; poly_z[1] = z0;
        poly_x[2] = x1; poly_z[2] = z1;
        poly_x[3] = x0; poly_z[3] = z1;

        /* Punkt auf unterer Kante (z = z0, x zwischen x0 und x1) */
        t_num = (int32_t)(xorshift32() % (uint32_t)(x1 - x0));
        edge_x = (int16_t)(x0 + t_num);
        edge_z = z0;

        ref_result = ref_point_in_4p(edge_x, edge_z, poly_x, poly_z);
        aot_result = aot_check_point(edge_x, edge_z, poly_x, poly_z);

        PROP_ASSERT(ref_result == 1,
            "Iter %d: Kantenpunkt (%d,%d) nicht als 'drin' (Ref) "
            "(Rect [%d,%d]-[%d,%d])",
            iter, edge_x, edge_z, x0, z0, x1, z1);

        PROP_ASSERT(aot_result == ref_result,
            "Iter %d: AOT (%d) != Ref (%d) für Kantenpunkt (%d,%d)",
            iter, aot_result, ref_result, edge_x, edge_z);

        /* Punkt auf rechter Kante (x = x1, z zwischen z0 und z1) */
        t_num = (int32_t)(xorshift32() % (uint32_t)(z1 - z0));
        edge_x = x1;
        edge_z = (int16_t)(z0 + t_num);

        ref_result = ref_point_in_4p(edge_x, edge_z, poly_x, poly_z);
        aot_result = aot_check_point(edge_x, edge_z, poly_x, poly_z);

        PROP_ASSERT(ref_result == 1,
            "Iter %d: Kantenpunkt (%d,%d) nicht als 'drin' (Ref) rechte Kante",
            iter, edge_x, edge_z);

        PROP_ASSERT(aot_result == ref_result,
            "Iter %d: AOT (%d) != Ref (%d) für Kantenpunkt rechts (%d,%d)",
            iter, aot_result, ref_result, edge_x, edge_z);
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 4: Eckpunkte des Polygons zählen als "drin"
 *
 * Alle 4 Vertices des Polygons selbst liegen auf mindestens zwei Kanten
 * und müssen als "innerhalb" erkannt werden.
 * ========================================================================= */

static int prop_vertices_inside(void)
{
    int iter;

    for (iter = 0; iter < NUM_CONVEX_ITERATIONS; iter++) {
        int16_t poly_x[4], poly_z[4];
        int16_t cx, cz;
        int v;

        generate_convex_quad(poly_x, poly_z, &cx, &cz);

        for (v = 0; v < 4; v++) {
            int ref_result = ref_point_in_4p(poly_x[v], poly_z[v], poly_x, poly_z);
            int aot_result = aot_check_point(poly_x[v], poly_z[v], poly_x, poly_z);

            PROP_ASSERT(ref_result == 1,
                "Iter %d: Vertex %d (%d,%d) nicht als 'drin' (Ref)",
                iter, v, poly_x[v], poly_z[v]);

            PROP_ASSERT(aot_result == ref_result,
                "Iter %d: AOT (%d) != Ref (%d) für Vertex %d (%d,%d)",
                iter, aot_result, ref_result, v, poly_x[v], poly_z[v]);
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
    printf("=== Property-Test: Point-in-4P-Polygon Korrektheit ===\n");
    printf("    Validates: Requirements 4.2, 11.5, 11.6\n");
    printf("    PRNG Seed: %u\n", xorshift_state);
    printf("    Iterationen: %d (Rect) + %d (Convex)\n\n",
           NUM_RECT_ITERATIONS, NUM_CONVEX_ITERATIONS);

    /* Test 1: Achsenparallele Rechtecke */
    printf("[1/4] prop_rect_containment (%d Iterationen) ... ",
           NUM_RECT_ITERATIONS);
    if (prop_rect_containment() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test 2: Zufällige konvexe Quads */
    printf("[2/4] prop_convex_quad_containment (%d Iterationen) ... ",
           NUM_CONVEX_ITERATIONS);
    if (prop_convex_quad_containment() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test 3: Kantenpunkte zählen als "drin" */
    printf("[3/4] prop_edge_points_inside (%d Iterationen) ... ",
           NUM_RECT_ITERATIONS);
    if (prop_edge_points_inside() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test 4: Vertices des Polygons zählen als "drin" */
    printf("[4/4] prop_vertices_inside (%d Iterationen) ... ",
           NUM_CONVEX_ITERATIONS);
    if (prop_vertices_inside() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Summary */
    printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
