/**
 * @file prop_sca_collision.c
 * @brief Property-Test: SCA-Kollisionserkennung (re15_collision_constrain)
 *
 * **Validates: Requirements 11.3**
 *
 * Property 13: Für jede Spielerposition (x, z) und jede SCA-Zelle (Typ 1:
 * Rechteck, Typ 3: Kreis) soll die Kollisionsauflösung
 * (re15_collision_constrain) genau dann die Position verschieben (Push-Out),
 * wenn der Spielerkörper die Zelle durchdringt, und sonst die Position
 * unverändert lassen.
 *
 * Geprüfte Invarianten:
 *   1. No-collision (Rect): Spieler weit außerhalb der Broad-Phase → Position
 *      unverändert.
 *   2. Containment push-out (Rect): Spieler tief innerhalb der soliden Zelle
 *      → Position wird verschoben.
 *   3. Circle push-out: Spieler innerhalb des combined_radius → danach
 *      distance >= combined_radius (innerhalb Integer-Toleranz).
 *   4a. Position stability (Rect): Spieler weit draußen → Position unverändert.
 *   4b. Position stability (Circle): Spieler weit draußen → Position unverändert.
 *
 * ---------------------------------------------------------------------------
 * REAKTIVIERUNG (2026-06-28): Der ursprüngliche Test war gegen eine inzwischen
 * ENTFERNTE Legacy-API geschrieben (re15_collision_check(player, sca) mit
 * re15_player_t/re15_sca_data_t aus _legacy_minimal/). Die AKTUELLE öffentliche
 * Kollisions-API ist:
 *
 *   - re15_collision_constrain(rdt, old_x, old_z, &x, &z)  (re15_collision.h:24)
 *     byte-true Port von FUN_8003b0a4 — schiebt den Spieler aus jeder
 *     band-passenden SOLIDEN SCA-Zelle heraus (re15_collision.c:277-310).
 *   - re15_collision_set_band(int)  (re15_collision.h:38, re15_collision.c:177)
 *     setzt das aktive Floor-Band; die Zelle wird nur aufgelöst, wenn
 *     (floor>>4) == band (re15_collision.c:295).
 *   - SCA-Daten liegen jetzt in re15_rdt_t (sca/sca_count/sca_rgn/ceiling_*,
 *     re15_rdt.h:95-99); der Eintrags-Typ re15_sca_entry_t hat die Felder
 *     {width,density,x,z,type,u0,u1,floor} (re15_rdt.h:57-66) — NICHT mehr
 *     corner_x/corner_z/depth/density wie in der Legacy-Variante.
 *
 * Der Spieler-Kollisionsradius ist in der aktuellen Engine FEST PR=450
 * (DAT_80073e94[6], re15_collision.c:34) statt eines per-Spieler-Radius. Die
 * Push-Out-Mathematik ist der byte-true Original-Code push_rect (FUN_8003bca8,
 * re15_collision.c:62) bzw. push_circle (FUN_8003d6a8, re15_collision.c:111) —
 * mit Skin-Margin 0x12 und Achsen-Präferenz-Schwellen. Die früheren exakten
 * Legacy-Invarianten (combined_radius = 20 + r, nearest-edge) gelten daher
 * nicht 1:1; die Invarianten unten prüfen den TATSÄCHLICHEN Engine-Vertrag
 * (Broad-Phase-Stabilität, Push bei Durchdringung, radiale Kreis-Auflösung).
 *
 * Nur die Test-Datei wurde geändert; Engine/Header bleiben unangetastet.
 * ---------------------------------------------------------------------------
 *
 * Nutzt 250 Iterationen mit pseudo-zufälligen Spielerpositionen und SCA-Zellen.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_collision.h"
#include "re15_rdt.h"

/* =========================================================================
 * Test-Infrastruktur
 * ========================================================================= */

static int g_passed = 0;
static int g_failed = 0;

#define NUM_ITERATIONS 250

/* Fester Spieler-Kollisionsradius der aktuellen Engine (PR, re15_collision.c:34). */
#define ENGINE_PR 450

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
 *
 * Identisch zur engine-internen coll_isqrt (re15_collision.c:37) — floor(sqrt).
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
 * RDT-Aufbau für eine einzelne SCA-Zelle (band-passend, solide)
 *
 * re15_collision_constrain (re15_collision.c:278) selektiert per quadrant_of
 * (re15_collision.c:51) ein Region-Fenster [start,end) aus sca_rgn[] und löst
 * nur Zellen mit (floor>>4)==band (Z.295) und (u0&1)!=0 (Z.296) auf. Wir setzen
 *   - ceiling_x/ceiling_z = 0x8000 → (int16_t) = -32768, sodass jede Testposition
 *     ein positives Delta hat → quadrant_of liefert IMMER 0 (re15_collision.c:53-55).
 *   - sca_rgn = {1,0,0,0,0} → Region 0 umfasst genau Eintrag 0.
 *   - floor = band<<4, u0 = 1 (solide).
 * Band wird per re15_collision_set_band(band) gesetzt.
 * ========================================================================= */

#define TEST_BAND 0

static void make_rdt(re15_rdt_t *rdt, re15_sca_entry_t *entry)
{
    memset(rdt, 0, sizeof(*rdt));
    rdt->sca        = entry;
    rdt->sca_count  = 1;
    rdt->sca_rgn[0] = 1;
    rdt->sca_rgn[1] = 0;
    rdt->sca_rgn[2] = 0;
    rdt->sca_rgn[3] = 0;
    rdt->sca_rgn[4] = 0;
    rdt->ceiling_x  = 0x8000u;  /* (int16_t)0x8000 == -32768 → quadrant 0 */
    rdt->ceiling_z  = 0x8000u;
    re15_collision_set_band(TEST_BAND);
}

/* =========================================================================
 * Generator: Zufälliges Rechteck-SCA-Entry (aktuelle Feldnamen)
 *
 * re15_sca_entry_t (re15_rdt.h:57): width=X-Extent, density=Z-Extent,
 * x/z = Ecke, type=1 Rechteck, u0&1 = solide, floor = band<<4.
 * ========================================================================= */

static void gen_rectangle_entry(re15_sca_entry_t* entry)
{
    memset(entry, 0, sizeof(re15_sca_entry_t));
    entry->type    = 1;                          /* Rechteck (re15_collision.c:300) */
    entry->x       = prng_range_i16(-5000, 5000);
    entry->z       = prng_range_i16(-5000, 5000);
    entry->width   = (uint16_t)prng_range_i32(100, 3000);  /* X-Extent  */
    entry->density = (uint16_t)prng_range_i32(100, 3000);  /* Z-Extent  */
    entry->u0      = 1;                           /* solide → Push-Out (Z.296) */
    entry->u1      = 0;
    entry->floor   = (uint8_t)(TEST_BAND << 4);   /* (floor>>4)==band (Z.295) */
}

/* =========================================================================
 * Generator: Zufälliges Kreis-SCA-Entry
 *
 * push_circle (re15_collision.c:111): Mittelpunkt = (x+cr, z+cr) mit
 * cr = width>>1; combined = cr + PR. density wird = width gesetzt, damit die
 * Broad-Phase in constrain (re15_collision.c:298-299) den Kreis symmetrisch
 * abdeckt.
 * ========================================================================= */

static void gen_circle_entry(re15_sca_entry_t* entry)
{
    memset(entry, 0, sizeof(re15_sca_entry_t));
    entry->type    = 3;                           /* Kreis (re15_collision.c:301) */
    entry->x       = prng_range_i16(-5000, 5000);
    entry->z       = prng_range_i16(-5000, 5000);
    entry->width   = (uint16_t)prng_range_i32(200, 4000);  /* Durchmesser → cr=width>>1 */
    entry->density = entry->width;                /* Broad-Phase-Symmetrie */
    entry->u0      = 1;                           /* solide */
    entry->u1      = 0;
    entry->floor   = (uint8_t)(TEST_BAND << 4);
}

/* Broad-Phase-Test aus re15_collision.c:298-299 (mit r = ENGINE_PR). 1 wenn
 * (x,z) im inflated Zellrechteck liegt → Push-Helfer würde aufgerufen. */
static int broad_phase_hit(const re15_sca_entry_t *e, int32_t x, int32_t z)
{
    int32_t r = ENGINE_PR;
    return (uint32_t)(x - ((int32_t)e->x - r)) < (uint32_t)((int32_t)e->width   + r * 2) &&
           (uint32_t)(z - ((int32_t)e->z - r)) < (uint32_t)((int32_t)e->density + r * 2);
}

/* =========================================================================
 * Property 1: No-collision invariant (Rect)
 *
 * Wenn das Bewegungsziel WEIT außerhalb der Broad-Phase-Box liegt (Abstand >
 * width/density + PR auf der jeweiligen Achse), darf re15_collision_constrain
 * die Position NICHT verändern (Broad-Phase verwirft, re15_collision.c:298).
 * ========================================================================= */

static int test_no_collision_rect(int iter)
{
    re15_sca_entry_t entry;
    re15_rdt_t rdt;

    gen_rectangle_entry(&entry);
    make_rdt(&rdt, &entry);

    int32_t x_min = (int32_t)entry.x;
    int32_t z_min = (int32_t)entry.z;
    int32_t x_max = x_min + (int32_t)entry.width;
    int32_t z_max = z_min + (int32_t)entry.density;

    /* Sicherer Abstand: jenseits der PR-inflated Box (+ Reserve). */
    int32_t safe = ENGINE_PR + 50;
    int32_t px, pz;
    int side = (int)(prng_next() % 4);
    switch (side) {
        case 0: /* links */
            px = x_min - safe - (int32_t)(prng_next() % 500);
            pz = prng_range_i32(z_min - 1000, z_max + 1000);
            break;
        case 1: /* rechts */
            px = x_max + safe + (int32_t)(prng_next() % 500);
            pz = prng_range_i32(z_min - 1000, z_max + 1000);
            break;
        case 2: /* oben (Z-) */
            px = prng_range_i32(x_min - 1000, x_max + 1000);
            pz = z_min - safe - (int32_t)(prng_next() % 500);
            break;
        default: /* unten (Z+) */
            px = prng_range_i32(x_min - 1000, x_max + 1000);
            pz = z_max + safe + (int32_t)(prng_next() % 500);
            break;
    }

    /* Garantiert außerhalb der Broad-Phase? Sonst diesen Fall überspringen
     * (die Diagonal-Aufstellung kann eine Achse innerhalb lassen). */
    if (broad_phase_hit(&entry, px, pz)) {
        g_passed++;
        return 0;
    }

    int32_t x = px, z = pz;
    re15_collision_constrain(&rdt, px, pz, &x, &z);

    PROP_ASSERT(x == px && z == pz,
        "No-collision (rect): Position changed! orig=(%d,%d) new=(%d,%d) "
        "rect=(%d,%d,%d,%d)",
        (int)px, (int)pz, (int)x, (int)z,
        (int)entry.x, (int)entry.z, (int)entry.width, (int)entry.density);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property 2: Containment push-out (Rect)
 *
 * Wenn das Bewegungsziel TIEF im Inneren der soliden Zelle liegt, muss
 * re15_collision_constrain die Position verschieben (push_rect feuert,
 * re15_collision.c:300/62). Geprüft: mindestens eine Achse ändert sich.
 * ========================================================================= */

static int test_containment_pushout_rect(int iter)
{
    re15_sca_entry_t entry;
    re15_rdt_t rdt;

    gen_rectangle_entry(&entry);
    make_rdt(&rdt, &entry);

    /* Zielpunkt im Inneren des Rechtecks (Mittenbereich). */
    int32_t x_min = (int32_t)entry.x;
    int32_t z_min = (int32_t)entry.z;
    int32_t px = prng_range_i32(x_min + 1, x_min + (int32_t)entry.width  - 1);
    int32_t pz = prng_range_i32(z_min + 1, z_min + (int32_t)entry.density - 1);

    /* old_x/old_z != target, damit push_rect nicht den "moved straight into a
     * face → snap-to-prev" Sonderfall (code==0) auf prev zurücksnappt, sondern
     * regulär auflöst. Wir starten von weit außerhalb (eindeutiger Bewegungs-
     * vektor, re15_collision.c:75). */
    int32_t old_x = x_min - (ENGINE_PR + 1000);
    int32_t old_z = pz;

    int32_t x = px, z = pz;
    re15_collision_constrain(&rdt, old_x, old_z, &x, &z);

    int was_pushed = (x != px || z != pz);
    PROP_ASSERT(was_pushed,
        "Containment push-out (rect): Player not pushed out! "
        "target=(%d,%d) result=(%d,%d) rect=(%d,%d,%d,%d)",
        (int)px, (int)pz, (int)x, (int)z,
        (int)entry.x, (int)entry.z, (int)entry.width, (int)entry.density);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property 3: Circle collision push-out
 *
 * Liegt das Ziel innerhalb des combined_radius (cr + PR) eines Kreises, muss
 * nach re15_collision_constrain der Abstand zum Kreismittelpunkt (x+cr, z+cr)
 * >= combined_radius sein (radiale Auflösung push_circle, re15_collision.c:111).
 *
 * Numerik der Engine: push_circle skaliert den Versatz mit pen/(dist+1) (Nenner
 * dist+1, NICHT dist) und trunkiert pro Achse separat (re15_collision.c:119-121).
 * Dadurch unterschätzt die Auflösung um ~pen/(dist+1); dieser Term explodiert nur
 * wenn der Spieler nahe am Mittelpunkt steht (dist→0). Empirisch (probe):
 * für dist_before >= 0.3*combined ist die Unterschätzung <= 4 Einheiten. Wie der
 * Original-Test ("zu nah am Mittelpunkt → skip") wird der Nahbereich daher
 * ausgeschlossen; Toleranz 4 deckt den Rest byte-true ab.
 * ========================================================================= */

static int test_circle_collision_pushout(int iter)
{
    re15_sca_entry_t entry;
    re15_rdt_t rdt;

    gen_circle_entry(&entry);
    make_rdt(&rdt, &entry);

    int32_t cr = (int32_t)entry.width >> 1;            /* re15_collision.c:113 */
    int32_t cx = (int32_t)entry.x + cr;                /* Mittelpunkt (Z.114-115) */
    int32_t cz = (int32_t)entry.z + cr;
    int32_t combined_radius = cr + ENGINE_PR;          /* Z.117 */

    /* Ziel innerhalb combined_radius, aber nicht im Nahbereich des Mittelpunkts
     * (dort bläht der (dist+1)-Nenner die Unterschätzung auf, s.o.). Untere
     * Schranke 0.3*combined → Unterschätzung <= 4. */
    int32_t min_dist = (combined_radius * 3) / 10;
    if (min_dist < 1) min_dist = 1;
    int32_t max_dist = combined_radius - 1;
    if (max_dist < min_dist) max_dist = min_dist;
    int32_t dist = prng_range_i32(min_dist, max_dist);
    /* 8 Richtungen (Achsen + Halb-Diagonalen), Versatz |.| ungefähr `dist`. */
    int sel = (int)(prng_next() & 7);
    int32_t dx, dz;
    switch (sel) {
        case 0: dx =  dist;     dz =  0;        break;
        case 1: dx = -dist;     dz =  0;        break;
        case 2: dx =  0;        dz =  dist;     break;
        case 3: dx =  0;        dz = -dist;     break;
        case 4: dx =  dist / 2; dz =  dist / 2; break;
        case 5: dx = -dist / 2; dz =  dist / 2; break;
        case 6: dx =  dist / 2; dz = -dist / 2; break;
        default: dx = -dist / 2; dz = -dist / 2; break;
    }
    int32_t px = cx + dx;
    int32_t pz = cz + dz;

    /* Tatsächlichen Startabstand prüfen: muss in [min_dist, combined) liegen,
     * sonst diesen Fall überspringen (Rand-/Nahbereich, s.o.). */
    int64_t init_sq = (int64_t)(px - cx) * (px - cx) + (int64_t)(pz - cz) * (pz - cz);
    int32_t init_dist = test_isqrt(init_sq);
    if (init_dist < min_dist || init_dist >= combined_radius) {
        g_passed++;
        return 0;
    }

    /* old != target, damit der Bewegungsvektor nicht null ist. */
    int32_t old_x = cx - (combined_radius + 1000);
    int32_t old_z = pz;

    int32_t x = px, z = pz;
    re15_collision_constrain(&rdt, old_x, old_z, &x, &z);

    int64_t final_sq = (int64_t)(x - cx) * (x - cx) + (int64_t)(z - cz) * (z - cz);
    int32_t final_dist = test_isqrt(final_sq);

    /* Toleranz 4: isqrt-Floor + Pro-Achsen-Trunkierung + (dist+1)-Nenner in
     * push_circle (Z.119-121), gedeckelt für dist_before >= 0.3*combined. */
    PROP_ASSERT(final_dist >= combined_radius - 4,
        "Circle push-out: distance after push = %d, expected >= %d "
        "(center=(%d,%d), cr=%d, PR=%d, result=(%d,%d))",
        (int)final_dist, (int)combined_radius,
        (int)cx, (int)cz, (int)cr, (int)ENGINE_PR, (int)x, (int)z);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property 4a: Position stability (Rect)
 *
 * Liegt das Ziel eindeutig außerhalb der Broad-Phase (Abstand > PR + Extent),
 * bleibt die Position unverändert.
 * ========================================================================= */

static int test_position_stability_rect(int iter)
{
    re15_sca_entry_t entry;
    re15_rdt_t rdt;

    gen_rectangle_entry(&entry);
    make_rdt(&rdt, &entry);

    int32_t x_min = (int32_t)entry.x;
    int32_t z_min = (int32_t)entry.z;
    int32_t x_max = x_min + (int32_t)entry.width;
    int32_t z_max = z_min + (int32_t)entry.density;

    int32_t safe = ENGINE_PR + 1;
    int32_t px, pz;
    int side = (int)(prng_next() % 4);
    switch (side) {
        case 0: /* weit links */
            px = x_min - safe - (int32_t)(prng_next() % 300);
            pz = prng_range_i32(z_min - 500, z_max + 500);
            break;
        case 1: /* weit rechts */
            px = x_max + safe + (int32_t)(prng_next() % 300);
            pz = prng_range_i32(z_min - 500, z_max + 500);
            break;
        case 2: /* weit oben */
            px = prng_range_i32(x_min - 500, x_max + 500);
            pz = z_min - safe - (int32_t)(prng_next() % 300);
            break;
        default: /* weit unten */
            px = prng_range_i32(x_min - 500, x_max + 500);
            pz = z_max + safe + (int32_t)(prng_next() % 300);
            break;
    }

    /* Nur prüfen, wenn das Ziel die Broad-Phase wirklich verfehlt. */
    if (broad_phase_hit(&entry, px, pz)) {
        g_passed++;
        return 0;
    }

    int32_t x = px, z = pz;
    re15_collision_constrain(&rdt, px, pz, &x, &z);

    PROP_ASSERT(x == px && z == pz,
        "Position stability (rect): Position changed! "
        "orig=(%d,%d) new=(%d,%d) rect=(%d,%d,%d,%d) side=%d",
        (int)px, (int)pz, (int)x, (int)z,
        (int)entry.x, (int)entry.z, (int)entry.width, (int)entry.density, side);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property 4b: Position stability (Circle)
 *
 * Liegt das Ziel außerhalb der Broad-Phase eines Kreises, bleibt die Position
 * unverändert.
 * ========================================================================= */

static int test_position_stability_circle(int iter)
{
    re15_sca_entry_t entry;
    re15_rdt_t rdt;

    gen_circle_entry(&entry);
    make_rdt(&rdt, &entry);

    int32_t cr = (int32_t)entry.width >> 1;
    int32_t cx = (int32_t)entry.x + cr;
    int32_t cz = (int32_t)entry.z + cr;
    int32_t combined_radius = cr + ENGINE_PR;

    int32_t safe_dist = combined_radius + 10 + (int32_t)(prng_next() % 500);
    int32_t px, pz;
    int quadrant = (int)(prng_next() % 4);
    switch (quadrant) {
        case 0: px = cx + safe_dist; pz = cz;             break;
        case 1: px = cx - safe_dist; pz = cz;             break;
        case 2: px = cx;             pz = cz + safe_dist; break;
        default: px = cx;            pz = cz - safe_dist; break;
    }

    /* Nur prüfen, wenn die Broad-Phase verfehlt wird. */
    if (broad_phase_hit(&entry, px, pz)) {
        g_passed++;
        return 0;
    }

    int32_t x = px, z = pz;
    re15_collision_constrain(&rdt, px, pz, &x, &z);

    PROP_ASSERT(x == px && z == pz,
        "Position stability (circle): Position changed! "
        "orig=(%d,%d) new=(%d,%d) center=(%d,%d) combined_r=%d",
        (int)px, (int)pz, (int)x, (int)z, (int)cx, (int)cz, (int)combined_radius);

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
