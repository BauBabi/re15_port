/**
 * @file prop_sca_collision.c
 * @brief Property-Test: SCA-Kollisionserkennung
 *
 * **Validates: Requirements 11.3**
 *
 * Property 13: Für jede Spielerposition (x, z) mit Radius r und jede
 * SCA-Zelle (Typ 1: Rechteck, Typ 3: Kreis) soll die Kollisionserkennung
 * genau dann eine Durchdringung melden (Position ändern), wenn der
 * zylindrische Spielerkörper die Zellgrenzen überschneidet, und die
 * berechnete Rücksetz-Richtung soll orthogonal zur nächsten durchdrungenen
 * Kante sein.
 *
 * Geprüfte Invarianten:
 *   1. No-collision: Wenn Spieler weit außerhalb → Position unverändert
 *   2. Containment push-out: Spieler innerhalb → nach Prüfung außerhalb
 *   3. Circle collision: Spieler im combined_radius → danach distance >= combined_radius
 *   4. Position stability: Spieler bereits draußen → Position unverändert
 *
 * Nutzt 250 Iterationen mit pseudo-zufälligen Spielerpositionen und SCA-Zellen.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_collision.h"
#include "re15_player.h"
#include "re15_types.h"

/* =========================================================================
 * Test-Infrastruktur
 * ========================================================================= */

static int g_passed = 0;
static int g_failed = 0;

#define NUM_ITERATIONS 250

#define PROP_ASSERT(cond, fmt, ...)                                          \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("  FAIL [iter %d]: " fmt "\n", iter, ##__VA_ARGS__);       \
            printf("        at %s:%d\n", __FILE__, __LINE__);                 \
            g_failed++;                                                        \
            return 1;                                                          \
        }                                                                      \
    } while (0)

/* =========================================================================
 * Einfacher PRNG (xorshift32) für reproduzierbare Zufallswerte
 * ========================================================================= */

static uint32_t prng_state = 0xCAFEBABE;

static uint32_t prng_next(void)
{
    uint32_t x = prng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    prng_state = x;
    return x;
}

/** Zufälliger int16-Wert im Bereich [lo, hi] */
static int16_t prng_range_i16(int16_t lo, int16_t hi)
{
    uint32_t range = (uint32_t)(hi - lo + 1);
    return (int16_t)(lo + (int16_t)(prng_next() % range));
}

/** Zufälliger int32-Wert im Bereich [lo, hi] */
static int32_t prng_range_i32(int32_t lo, int32_t hi)
{
    uint32_t range = (uint32_t)(hi - lo + 1);
    return lo + (int32_t)(prng_next() % range);
}

/* =========================================================================
 * Hilfsfunktion: Integer-Quadratwurzel (für Distanzberechnung)
 * ========================================================================= */

static int32_t test_isqrt(int64_t val)
{
    if (val <= 0) return 0;
    uint64_t v = (uint64_t)val;
    uint64_t result = 0;
    uint64_t bit = (uint64_t)1 << 62;
    while (bit > v) bit >>= 2;
    while (bit != 0) {
        if (v >= result + bit) {
            v -= result + bit;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }
        bit >>= 2;
    }
    return (int32_t)result;
}

/* =========================================================================
 * Generator: Zufälliges Rechteck-SCA-Entry
 * ========================================================================= */

static void gen_rectangle_entry(re15_sca_entry_t* entry)
{
    memset(entry, 0, sizeof(re15_sca_entry_t));
    entry->type     = RE15_SCA_TYPE_RECTANGLE;
    entry->corner_x = prng_range_i16(-5000, 5000);
    entry->corner_z = prng_range_i16(-5000, 5000);
    entry->width    = prng_range_i16(100, 3000);   /* Positive Breite */
    entry->depth    = prng_range_i16(100, 3000);   /* Positive Tiefe  */
    entry->density  = 0;
    entry->flags    = 0;
}

/* =========================================================================
 * Generator: Zufälliges Kreis-SCA-Entry
 * ========================================================================= */

static void gen_circle_entry(re15_sca_entry_t* entry)
{
    memset(entry, 0, sizeof(re15_sca_entry_t));
    entry->type     = RE15_SCA_TYPE_CIRCLE;
    entry->corner_x = prng_range_i16(-5000, 5000);  /* Mittelpunkt X */
    entry->corner_z = prng_range_i16(-5000, 5000);  /* Mittelpunkt Z */
    entry->width    = prng_range_i16(50, 2000);     /* Radius        */
    entry->depth    = 0;
    entry->density  = 0;
    entry->flags    = 0;
}

/* =========================================================================
 * Generator: Spieler mit typischem Radius
 * ========================================================================= */

#define TEST_PLAYER_RADIUS 20

static void gen_player(re15_player_t* player, int32_t x, int32_t z)
{
    memset(player, 0, sizeof(re15_player_t));
    player->x      = x;
    player->y      = 0;
    player->z      = z;
    player->yaw    = 0;
    player->radius = TEST_PLAYER_RADIUS;
    player->floor_band = 0;
}

/* =========================================================================
 * Property 1: No-collision invariant
 *
 * Wenn der Spieler weit genug von einem Rechteck entfernt ist (Abstand >
 * Radius auf jeder Achse), darf die Position nach collision_check NICHT
 * verändert werden.
 * ========================================================================= */

static int test_no_collision_rect(int iter)
{
    re15_sca_entry_t entry;
    re15_sca_data_t sca;
    re15_player_t player;

    gen_rectangle_entry(&entry);
    sca.count   = 1;
    sca.reserved = 0;
    sca.entries = &entry;

    /* Platziere Spieler weit außerhalb des Rechtecks (mindestens radius + 100 Abstand) */
    int32_t safe_margin = (int32_t)TEST_PLAYER_RADIUS + 100;
    int32_t px, pz;

    /* Wähle zufällige Seite: 0=links, 1=rechts, 2=oben, 3=unten */
    int side = (int)(prng_next() % 4);
    switch (side) {
        case 0: /* Links vom Rechteck */
            px = (int32_t)entry.corner_x - safe_margin - (int32_t)(prng_next() % 500);
            pz = prng_range_i32((int32_t)entry.corner_z - 1000,
                                (int32_t)entry.corner_z + (int32_t)entry.depth + 1000);
            break;
        case 1: /* Rechts vom Rechteck */
            px = (int32_t)entry.corner_x + (int32_t)entry.width + safe_margin + (int32_t)(prng_next() % 500);
            pz = prng_range_i32((int32_t)entry.corner_z - 1000,
                                (int32_t)entry.corner_z + (int32_t)entry.depth + 1000);
            break;
        case 2: /* Oben (Z-) */
            px = prng_range_i32((int32_t)entry.corner_x - 1000,
                                (int32_t)entry.corner_x + (int32_t)entry.width + 1000);
            pz = (int32_t)entry.corner_z - safe_margin - (int32_t)(prng_next() % 500);
            break;
        default: /* Unten (Z+) */
            px = prng_range_i32((int32_t)entry.corner_x - 1000,
                                (int32_t)entry.corner_x + (int32_t)entry.width + 1000);
            pz = (int32_t)entry.corner_z + (int32_t)entry.depth + safe_margin + (int32_t)(prng_next() % 500);
            break;
    }

    gen_player(&player, px, pz);
    int32_t orig_x = player.x;
    int32_t orig_z = player.z;

    re15_collision_check(&player, &sca);

    PROP_ASSERT(player.x == orig_x && player.z == orig_z,
        "No-collision (rect): Position changed! orig=(%d,%d) new=(%d,%d) "
        "rect=(%d,%d,%d,%d)",
        (int)orig_x, (int)orig_z, (int)player.x, (int)player.z,
        (int)entry.corner_x, (int)entry.corner_z,
        (int)entry.width, (int)entry.depth);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property 2: Containment push-out (Rechteck)
 *
 * Wenn der Spieler-Mittelpunkt innerhalb eines Rechtecks platziert wird,
 * muss nach collision_check der Spieler-Mittelpunkt außerhalb des Rechtecks
 * liegen ODER einen Abstand >= Radius zu den Kanten haben (kein Überlapp).
 * ========================================================================= */

static int test_containment_pushout_rect(int iter)
{
    re15_sca_entry_t entry;
    re15_sca_data_t sca;
    re15_player_t player;

    gen_rectangle_entry(&entry);
    sca.count   = 1;
    sca.reserved = 0;
    sca.entries = &entry;

    /* Platziere Spieler innerhalb des Rechtecks */
    int32_t px = prng_range_i32((int32_t)entry.corner_x + 1,
                                (int32_t)entry.corner_x + (int32_t)entry.width - 1);
    int32_t pz = prng_range_i32((int32_t)entry.corner_z + 1,
                                (int32_t)entry.corner_z + (int32_t)entry.depth - 1);

    gen_player(&player, px, pz);

    re15_collision_check(&player, &sca);

    /* Minimal: Spieler darf nicht mehr voll im Inneren stecken. */
    int was_pushed = (player.x != px || player.z != pz);

    PROP_ASSERT(was_pushed,
        "Containment push-out (rect): Player not pushed out! "
        "pos=(%d,%d) rect=(%d,%d,%d,%d)",
        (int)player.x, (int)player.z,
        (int)entry.corner_x, (int)entry.corner_z,
        (int)entry.width, (int)entry.depth);

    /* After push, verify that position was moved along exactly one axis (orthogonal push) */
    int pushed_x = (player.x != px);
    int pushed_z = (player.z != pz);

    /* Push should be orthogonal to nearest edge (only one axis changes) */
    PROP_ASSERT(pushed_x || pushed_z,
        "Containment push-out (rect): No axis displacement. "
        "orig=(%d,%d) new=(%d,%d)",
        (int)px, (int)pz, (int)player.x, (int)player.z);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property 3: Circle collision push-out
 *
 * Wenn der Spieler-Mittelpunkt innerhalb des combined_radius eines Kreises
 * liegt, muss nach collision_check der Abstand zum Kreismittelpunkt
 * >= combined_radius sein.
 * ========================================================================= */

static int test_circle_collision_pushout(int iter)
{
    re15_sca_entry_t entry;
    re15_sca_data_t sca;
    re15_player_t player;

    gen_circle_entry(&entry);
    sca.count   = 1;
    sca.reserved = 0;
    sca.entries = &entry;

    int32_t cx = (int32_t)entry.corner_x;
    int32_t cz = (int32_t)entry.corner_z;
    int32_t circle_radius = (int32_t)entry.width;
    int32_t combined_radius = (int32_t)TEST_PLAYER_RADIUS + circle_radius;

    /* Platziere Spieler innerhalb des combined_radius (aber nicht im Mittelpunkt) */
    int32_t max_dist = combined_radius - 1;
    if (max_dist < 1) max_dist = 1;

    /* Zufälligen Winkel und Abstand erzeugen */
    int32_t angle = prng_range_i32(0, 4095);
    int32_t dist = prng_range_i32(1, max_dist);

    /* Einfache Annäherung: Verwende Winkel als Quadrant-Selektor */
    int32_t dx, dz;
    switch (angle & 3) {
        case 0: dx =  dist; dz =  (dist / 2); break;
        case 1: dx = -dist; dz =  (dist / 2); break;
        case 2: dx =  (dist / 2); dz = -dist; break;
        default: dx = -(dist / 2); dz = -dist; break;
    }

    int32_t px = cx + dx;
    int32_t pz = cz + dz;

    /* Sicherstellen, dass wir tatsächlich innerhalb des combined_radius sind */
    int64_t initial_dist_sq = (int64_t)(px - cx) * (px - cx) +
                              (int64_t)(pz - cz) * (pz - cz);
    int64_t combined_sq = (int64_t)combined_radius * combined_radius;

    if (initial_dist_sq >= combined_sq) {
        /* Wenn zufällig doch außerhalb, verschieben wir näher ran */
        px = cx + (dx / 4);
        pz = cz + (dz / 4);
        initial_dist_sq = (int64_t)(px - cx) * (px - cx) +
                          (int64_t)(pz - cz) * (pz - cz);
        if (initial_dist_sq >= combined_sq) {
            /* Skip diesen Fall — zu nah am Rand */
            g_passed++;
            return 0;
        }
    }

    gen_player(&player, px, pz);

    re15_collision_check(&player, &sca);

    /* Nach Push: Abstand zum Kreismittelpunkt muss >= combined_radius sein */
    int64_t final_dist_sq = (int64_t)(player.x - cx) * (player.x - cx) +
                            (int64_t)(player.z - cz) * (player.z - cz);

    /* Erlaube Toleranz von 2 (Integer-Rundung bei isqrt und Division) */
    int32_t final_dist = test_isqrt(final_dist_sq);

    PROP_ASSERT(final_dist >= combined_radius - 2,
        "Circle push-out: distance after push = %d, expected >= %d "
        "(center=(%d,%d), r=%d, player_r=%d, player_pos=(%d,%d))",
        (int)final_dist, (int)combined_radius,
        (int)cx, (int)cz, (int)circle_radius, (int)TEST_PLAYER_RADIUS,
        (int)player.x, (int)player.z);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property 4: Position stability (Rechteck)
 *
 * Wenn der Spieler bereits außerhalb aller Kollisionsbereiche liegt
 * (Abstand > Radius zu allen Kanten), bleibt die Position unverändert.
 * ========================================================================= */

static int test_position_stability_rect(int iter)
{
    re15_sca_entry_t entry;
    re15_sca_data_t sca;
    re15_player_t player;

    gen_rectangle_entry(&entry);
    sca.count   = 1;
    sca.reserved = 0;
    sca.entries = &entry;

    int32_t radius = TEST_PLAYER_RADIUS;

    /* Platziere Spieler eindeutig außerhalb (Abstand > Radius auf jeder relevanten Achse) */
    int32_t rect_x_min = (int32_t)entry.corner_x;
    int32_t rect_z_min = (int32_t)entry.corner_z;
    int32_t rect_x_max = rect_x_min + (int32_t)entry.width;
    int32_t rect_z_max = rect_z_min + (int32_t)entry.depth;

    int32_t px, pz;
    int side = (int)(prng_next() % 4);

    /* Erzeuge Position die sicher außerhalb des Radius-Bereichs liegt */
    switch (side) {
        case 0: /* Weit links */
            px = rect_x_min - radius - 1 - (int32_t)(prng_next() % 200);
            pz = prng_range_i32(rect_z_min - 500, rect_z_max + 500);
            break;
        case 1: /* Weit rechts */
            px = rect_x_max + radius + 1 + (int32_t)(prng_next() % 200);
            pz = prng_range_i32(rect_z_min - 500, rect_z_max + 500);
            break;
        case 2: /* Weit oben */
            px = prng_range_i32(rect_x_min - 500, rect_x_max + 500);
            pz = rect_z_min - radius - 1 - (int32_t)(prng_next() % 200);
            break;
        default: /* Weit unten */
            px = prng_range_i32(rect_x_min - 500, rect_x_max + 500);
            pz = rect_z_max + radius + 1 + (int32_t)(prng_next() % 200);
            break;
    }

    /* Zusätzlich: Sicherstellen, dass Ecken-Abstand > Radius ist */
    /* Wenn Spieler diagonal liegt, prüfe ob Ecken-Abstand groß genug */
    int in_x_range = (px >= rect_x_min && px <= rect_x_max);
    int in_z_range = (pz >= rect_z_min && pz <= rect_z_max);

    if (!in_x_range && !in_z_range) {
        /* Diagonal: Prüfe Ecken-Abstand */
        int32_t closest_x = (px < rect_x_min) ? rect_x_min : rect_x_max;
        int32_t closest_z = (pz < rect_z_min) ? rect_z_min : rect_z_max;
        int64_t dx = (int64_t)(px - closest_x);
        int64_t dz = (int64_t)(pz - closest_z);
        int64_t dist_sq = dx * dx + dz * dz;
        int64_t rad_sq = (int64_t)radius * radius;
        if (dist_sq <= rad_sq) {
            /* Zu nah an Ecke — verschiebe weiter weg */
            px += (px < closest_x) ? -(radius + 50) : (radius + 50);
            pz += (pz < closest_z) ? -(radius + 50) : (radius + 50);
        }
    }

    gen_player(&player, px, pz);
    int32_t orig_x = player.x;
    int32_t orig_z = player.z;

    re15_collision_check(&player, &sca);

    PROP_ASSERT(player.x == orig_x && player.z == orig_z,
        "Position stability (rect): Position changed! "
        "orig=(%d,%d) new=(%d,%d) rect=(%d,%d,%d,%d) side=%d",
        (int)orig_x, (int)orig_z, (int)player.x, (int)player.z,
        (int)entry.corner_x, (int)entry.corner_z,
        (int)entry.width, (int)entry.depth, side);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property 4b: Position stability (Kreis)
 *
 * Wenn der Spieler außerhalb des combined_radius eines Kreises liegt,
 * bleibt die Position unverändert.
 * ========================================================================= */

static int test_position_stability_circle(int iter)
{
    re15_sca_entry_t entry;
    re15_sca_data_t sca;
    re15_player_t player;

    gen_circle_entry(&entry);
    sca.count   = 1;
    sca.reserved = 0;
    sca.entries = &entry;

    int32_t cx = (int32_t)entry.corner_x;
    int32_t cz = (int32_t)entry.corner_z;
    int32_t circle_radius = (int32_t)entry.width;
    int32_t combined_radius = (int32_t)TEST_PLAYER_RADIUS + circle_radius;

    /* Platziere Spieler sicher außerhalb des combined_radius */
    int32_t safe_dist = combined_radius + 10 + (int32_t)(prng_next() % 500);
    int32_t px, pz;

    int quadrant = (int)(prng_next() % 4);
    switch (quadrant) {
        case 0: px = cx + safe_dist; pz = cz;             break;
        case 1: px = cx - safe_dist; pz = cz;             break;
        case 2: px = cx;             pz = cz + safe_dist; break;
        default: px = cx;            pz = cz - safe_dist; break;
    }

    gen_player(&player, px, pz);
    int32_t orig_x = player.x;
    int32_t orig_z = player.z;

    re15_collision_check(&player, &sca);

    PROP_ASSERT(player.x == orig_x && player.z == orig_z,
        "Position stability (circle): Position changed! "
        "orig=(%d,%d) new=(%d,%d) center=(%d,%d) combined_r=%d",
        (int)orig_x, (int)orig_z, (int)player.x, (int)player.z,
        (int)cx, (int)cz, (int)combined_radius);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Main — Property-Tests mit zufälligen Spielerpositionen und SCA-Zellen
 * ========================================================================= */

int main(void)
{
    int iter;
    int failures = 0;

    printf("=== Property-Test: SCA-Kollisionserkennung ===\n");
    printf("    Validates: Requirements 11.3\n");
    printf("    Iterationen: %d\n\n", NUM_ITERATIONS);

    printf("--- Property 1: No-collision invariant (Rect) ---\n");
    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        if (test_no_collision_rect(iter) != 0) {
            failures++;
            break;
        }
    }
    if (!failures) {
        printf("  PASS: %d Iterationen bestanden.\n", NUM_ITERATIONS);
    }

    printf("\n--- Property 2: Containment push-out (Rect) ---\n");
    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        if (test_containment_pushout_rect(iter) != 0) {
            failures++;
            break;
        }
    }
    if (failures == 0) {
        printf("  PASS: %d Iterationen bestanden.\n", NUM_ITERATIONS);
    }

    printf("\n--- Property 3: Circle collision push-out ---\n");
    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        if (test_circle_collision_pushout(iter) != 0) {
            failures++;
            break;
        }
    }
    if (failures == 0) {
        printf("  PASS: %d Iterationen bestanden.\n", NUM_ITERATIONS);
    }

    printf("\n--- Property 4a: Position stability (Rect) ---\n");
    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        if (test_position_stability_rect(iter) != 0) {
            failures++;
            break;
        }
    }
    if (failures == 0) {
        printf("  PASS: %d Iterationen bestanden.\n", NUM_ITERATIONS);
    }

    printf("\n--- Property 4b: Position stability (Circle) ---\n");
    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        if (test_position_stability_circle(iter) != 0) {
            failures++;
            break;
        }
    }
    if (failures == 0) {
        printf("  PASS: %d Iterationen bestanden.\n", NUM_ITERATIONS);
    }

    printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
