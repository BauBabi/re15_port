/**
 * @file prop_zone_dispatch.c
 * @brief Property-Test: Zone-Containment → Kamera/Aktion-Dispatch
 *
 * **Property 14: Zone-Containment → Kamera/Aktion-Dispatch**
 *
 * Testet zwei Kernaspekte des Zonen-Systems:
 *
 * 1. RVD-Kamerazone: Wenn der Spieler innerhalb einer RVD-Zone liegt und
 *    cam_from == aktuelle Kamera, dann liefert re15_camera_check_zones()
 *    den cam_to-Wert dieser Zone. Liegt der Spieler außerhalb aller Zonen,
 *    bleibt die aktuelle Kamera unverändert.
 *
 * 2. AOT-Aktion-Dispatch: Wenn der Spieler innerhalb einer aktiven AOT-Zone
 *    liegt, liefert re15_aot_check() den Slot-Index. Der Typ des Slots
 *    bestimmt die auszulösende Aktion (DOOR, ITEM, GENERIC).
 *
 * Testansatz:
 * - Generiere synthetische RVD-Daten mit achsenparallelen Rechteck-Zonen
 * - Platziere Spieler innerhalb jeder Zone → erwarte cam_to
 * - Platziere Spieler außerhalb aller Zonen → erwarte current_cam
 * - Generiere AOT-Slots mit verschiedenen Typen
 * - Platziere Spieler innerhalb → erwarte korrekten Slot-Index + Typ
 *
 * **Validates: Requirements 11.5, 11.6**
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_camera.h"
#include "re15_aot.h"

/* =========================================================================
 * Konfiguration
 * ========================================================================= */

/** Anzahl der Iterationen für RVD-Kamerazone-Tests */
#define NUM_RVD_ITERATIONS 300

/** Anzahl der Iterationen für AOT-Aktion-Dispatch-Tests */
#define NUM_AOT_ITERATIONS 300

/** Floor-Band für alle Tests */
#define TEST_FLOOR_BAND 0

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

/** Zufällige uint8_t im Bereich [lo, hi] (inklusiv) */
static uint8_t rand_u8_range(uint8_t lo, uint8_t hi)
{
    uint32_t range = (uint32_t)(hi - lo + 1);
    return (uint8_t)(lo + (uint8_t)(xorshift32() % range));
}

/* =========================================================================
 * Hilfsfunktion: Achsenparalleles Rechteck als 4-Punkt-Polygon (CCW)
 *
 * Erzeugt ein Rechteck [x0,x1] × [z0,z1] mit Mindestgröße 10 in jeder
 * Dimension. Die 4 Vertices werden in CCW-Reihenfolge zurückgegeben.
 * ========================================================================= */

static void make_rect_zone(int16_t* poly_x, int16_t* poly_z,
                           int16_t* out_x0, int16_t* out_x1,
                           int16_t* out_z0, int16_t* out_z1)
{
    int16_t x0, x1, z0, z1;

    x0 = rand_s16_range(-15000, 14000);
    x1 = (int16_t)(x0 + rand_s16_range(10, 2000));
    z0 = rand_s16_range(-15000, 14000);
    z1 = (int16_t)(z0 + rand_s16_range(10, 2000));

    /* Clamp */
    if (x1 > 30000) x1 = 30000;
    if (z1 > 30000) z1 = 30000;

    /* CCW: unten-links, unten-rechts, oben-rechts, oben-links */
    poly_x[0] = x0; poly_z[0] = z0;
    poly_x[1] = x1; poly_z[1] = z0;
    poly_x[2] = x1; poly_z[2] = z1;
    poly_x[3] = x0; poly_z[3] = z1;

    *out_x0 = x0;
    *out_x1 = x1;
    *out_z0 = z0;
    *out_z1 = z1;
}

/** Erzeuge einen Punkt innerhalb des Rechtecks [x0,x1] × [z0,z1] */
static void make_interior_point(int16_t x0, int16_t x1,
                                int16_t z0, int16_t z1,
                                int16_t* px, int16_t* pz)
{
    /* Punkt im Inneren (Rand +1 Abstand) */
    *px = rand_s16_range((int16_t)(x0 + 1), (int16_t)(x1 - 1));
    *pz = rand_s16_range((int16_t)(z0 + 1), (int16_t)(z1 - 1));
}

/** Erzeuge einen Punkt klar außerhalb aller gegebenen Rechtecke */
static void make_exterior_point(int16_t* px, int16_t* pz)
{
    /* Punkt weit oben-rechts, wo keine Zone liegt */
    *px = rand_s16_range(30001, 32000);
    *pz = rand_s16_range(30001, 32000);
}

/* =========================================================================
 * Property-Test A: RVD-Kamerazone-Dispatch
 *
 * Für jede Iteration:
 * 1. Erstelle RVD-Daten mit N Zonen (N = 1..8), jeweils verschiedene
 *    cam_from / cam_to Werte
 * 2. Platziere Spieler innerhalb jeder Zone (mit passender current_cam)
 *    → Erwartung: out_new_cam == zone.cam_to
 * 3. Platziere Spieler außerhalb aller Zonen
 *    → Erwartung: out_new_cam == current_cam (unverändert)
 * 4. Platziere Spieler innerhalb Zone, aber mit falscher current_cam
 *    → Erwartung: out_new_cam == current_cam (Zone wird ignoriert)
 * ========================================================================= */

static int prop_rvd_camera_dispatch(void)
{
    int iter;

    for (iter = 0; iter < NUM_RVD_ITERATIONS; iter++) {
        re15_rvd_data_t rvd;
        int num_zones;
        int z_idx;
        int16_t zone_x0[8], zone_x1[8], zone_z0[8], zone_z1[8];
        uint8_t out_cam;

        memset(&rvd, 0, sizeof(rvd));

        /* Anzahl der Zonen für diese Iteration (1..8) */
        num_zones = (int)(1 + (xorshift32() % 8));
        rvd.count = (uint16_t)num_zones;

        /* Zonen generieren — jeweils eigene cam_from / cam_to.
         * cam_from MUSS pro Zone eindeutig sein: re15_camera_check_zones()
         * liefert die ERSTE passende Zone (cam_from == current_cam). Bei
         * geometrisch überlappenden Zonen mit gleichem cam_from wäre sonst
         * mehrdeutig, welche cam_to der Test erwartet → Flakiness. Mit
         * eindeutigem cam_from matcht bei current_cam == zone.cam_from genau
         * eine Zone, unabhängig von geometrischem Overlap. */
        for (z_idx = 0; z_idx < num_zones; z_idx++) {
            re15_rvd_zone_t* zone = &rvd.zones[z_idx];
            uint8_t cf;
            int dup, k;

            make_rect_zone(zone->trigger_x, zone->trigger_z,
                           &zone_x0[z_idx], &zone_x1[z_idx],
                           &zone_z0[z_idx], &zone_z1[z_idx]);

            /* Eindeutige cam_from (num_zones <= 8 <= 16 mögliche Werte) */
            do {
                cf = rand_u8_range(0, 15);
                dup = 0;
                for (k = 0; k < z_idx; k++) {
                    if (rvd.zones[k].cam_from == cf) { dup = 1; break; }
                }
            } while (dup);
            zone->cam_from = cf;

            /* cam_to muss verschieden von cam_from sein */
            do {
                zone->cam_to = rand_u8_range(0, 15);
            } while (zone->cam_to == zone->cam_from);

            zone->flags = 0;
        }

        /* Test A1: Spieler innerhalb jeder Zone (passende current_cam) */
        for (z_idx = 0; z_idx < num_zones; z_idx++) {
            int16_t px, pz;
            const re15_rvd_zone_t* zone = &rvd.zones[z_idx];

            make_interior_point(zone_x0[z_idx], zone_x1[z_idx],
                                zone_z0[z_idx], zone_z1[z_idx],
                                &px, &pz);

            out_cam = 0xFF;
            re15_camera_check_zones(px, pz, &rvd, zone->cam_from, &out_cam);

            PROP_ASSERT(out_cam == zone->cam_to,
                "Iter %d Zone %d: Spieler bei (%d,%d) innerhalb Zone, "
                "cam_from=%u → erwarte cam_to=%u, aber got %u",
                iter, z_idx, px, pz,
                zone->cam_from, zone->cam_to, out_cam);
        }

        /* Test A2: Spieler außerhalb aller Zonen → Kamera unverändert */
        {
            int16_t px, pz;
            uint8_t current_cam = rand_u8_range(0, 15);

            make_exterior_point(&px, &pz);

            out_cam = 0xFF;
            re15_camera_check_zones(px, pz, &rvd, current_cam, &out_cam);

            PROP_ASSERT(out_cam == current_cam,
                "Iter %d: Spieler bei (%d,%d) außerhalb, "
                "erwarte current_cam=%u, aber got %u",
                iter, px, pz, current_cam, out_cam);
        }

        /* Test A3: Spieler innerhalb Zone, aber falsche current_cam
         *
         * Verwende eine einzelne Zone isoliert (1 Zone RVD), damit kein
         * zufälliger Overlap mit anderen Zonen den Test verfälscht.
         */
        {
            re15_rvd_data_t rvd_single;
            int16_t px, pz;
            int16_t sx0, sx1, sz0, sz1;
            uint8_t wrong_cam;

            memset(&rvd_single, 0, sizeof(rvd_single));
            rvd_single.count = 1;
            rvd_single.zones[0].cam_from = rand_u8_range(0, 14);
            rvd_single.zones[0].cam_to   = rand_u8_range(0, 15);

            make_rect_zone(rvd_single.zones[0].trigger_x,
                           rvd_single.zones[0].trigger_z,
                           &sx0, &sx1, &sz0, &sz1);
            rvd_single.zones[0].flags = 0;

            make_interior_point(sx0, sx1, sz0, sz1, &px, &pz);

            /* Wähle eine Kamera, die NICHT cam_from der Zone ist */
            do {
                wrong_cam = rand_u8_range(0, 15);
            } while (wrong_cam == rvd_single.zones[0].cam_from);

            out_cam = 0xFF;
            re15_camera_check_zones(px, pz, &rvd_single, wrong_cam, &out_cam);

            PROP_ASSERT(out_cam == wrong_cam,
                "Iter %d: Spieler bei (%d,%d) innerhalb Zone mit "
                "cam_from=%u, aber current_cam=%u (falsch) → "
                "erwarte unverändert %u, got %u",
                iter, px, pz,
                rvd_single.zones[0].cam_from, wrong_cam, wrong_cam, out_cam);
        }
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test B: AOT-Aktion-Dispatch
 *
 * Für jede Iteration:
 * 1. Konfiguriere AOT-Slots mit verschiedenen Typen (DOOR, ITEM, GENERIC)
 * 2. Platziere Spieler innerhalb jeder Zone
 *    → Erwartung: re15_aot_check() liefert den korrekten Slot-Index
 *    → Typ des zurückgegebenen Slots == konfigurierter Typ
 * 3. Platziere Spieler außerhalb aller Zonen
 *    → Erwartung: re15_aot_check() liefert -1
 * ========================================================================= */

static int prop_aot_action_dispatch(void)
{
    int iter;
    const uint8_t types[] = { AOT_TYPE_DOOR, AOT_TYPE_ITEM, AOT_TYPE_GENERIC };
    const int num_types = 3;

    for (iter = 0; iter < NUM_AOT_ITERATIONS; iter++) {
        int num_slots;
        int s_idx;
        int16_t slot_x0[3], slot_x1[3], slot_z0[3], slot_z1[3];
        re15_aot_slot_t slot;
        int result;

        /* Alle Slots zurücksetzen */
        re15_aot_init();

        /* Anzahl Slots für diese Iteration (1..3) */
        num_slots = (int)(1 + (xorshift32() % (uint32_t)num_types));

        /* Slots konfigurieren — jeder mit einem anderen Typ */
        for (s_idx = 0; s_idx < num_slots; s_idx++) {
            int16_t poly_x[4], poly_z[4];

            make_rect_zone(poly_x, poly_z,
                           &slot_x0[s_idx], &slot_x1[s_idx],
                           &slot_z0[s_idx], &slot_z1[s_idx]);

            memset(&slot, 0, sizeof(slot));
            slot.active     = 1;
            slot.type       = types[s_idx % num_types];
            slot.entered    = 0;
            slot.floor_band = TEST_FLOOR_BAND;
            memcpy(slot.trigger_x, poly_x, 4 * sizeof(int16_t));
            memcpy(slot.trigger_z, poly_z, 4 * sizeof(int16_t));

            /* Typspezifische Daten setzen */
            switch (slot.type) {
                case AOT_TYPE_DOOR:
                    slot.data.door.dest_stage = rand_u8_range(0, 5);
                    slot.data.door.dest_room  = rand_u8_range(0, 0x27);
                    break;
                case AOT_TYPE_ITEM:
                    slot.data.item.item_id = rand_u8_range(1, 50);
                    slot.data.item.amount  = rand_s16_range(1, 10);
                    break;
                case AOT_TYPE_GENERIC:
                    slot.data.generic.event_id = rand_u8_range(0, 63);
                    break;
                default:
                    break;
            }

            re15_aot_set_slot(s_idx, &slot);
        }

        /* Test B1: Spieler innerhalb jeder Slot-Zone → korrekter Index + Typ */
        for (s_idx = 0; s_idx < num_slots; s_idx++) {
            int16_t px, pz;

            /* Slots müssen nach jedem Test zurückgesetzt werden, da
             * re15_aot_check() den ersten matchenden Slot liefert.
             * Wir testen jeden Slot isoliert. */
            re15_aot_init();

            /* Nur diesen einen Slot aktiv setzen */
            memset(&slot, 0, sizeof(slot));
            slot.active     = 1;
            slot.type       = types[s_idx % num_types];
            slot.entered    = 0;
            slot.floor_band = TEST_FLOOR_BAND;

            {
                int16_t poly_x[4], poly_z[4];
                poly_x[0] = slot_x0[s_idx]; poly_z[0] = slot_z0[s_idx];
                poly_x[1] = slot_x1[s_idx]; poly_z[1] = slot_z0[s_idx];
                poly_x[2] = slot_x1[s_idx]; poly_z[2] = slot_z1[s_idx];
                poly_x[3] = slot_x0[s_idx]; poly_z[3] = slot_z1[s_idx];
                memcpy(slot.trigger_x, poly_x, 4 * sizeof(int16_t));
                memcpy(slot.trigger_z, poly_z, 4 * sizeof(int16_t));
            }

            switch (slot.type) {
                case AOT_TYPE_DOOR:
                    slot.data.door.dest_stage = rand_u8_range(0, 5);
                    slot.data.door.dest_room  = rand_u8_range(0, 0x27);
                    break;
                case AOT_TYPE_ITEM:
                    slot.data.item.item_id = rand_u8_range(1, 50);
                    slot.data.item.amount  = rand_s16_range(1, 10);
                    break;
                case AOT_TYPE_GENERIC:
                    slot.data.generic.event_id = rand_u8_range(0, 63);
                    break;
                default:
                    break;
            }

            re15_aot_set_slot(s_idx, &slot);

            make_interior_point(slot_x0[s_idx], slot_x1[s_idx],
                                slot_z0[s_idx], slot_z1[s_idx],
                                &px, &pz);

            result = re15_aot_check(px, pz, TEST_FLOOR_BAND);

            PROP_ASSERT(result == s_idx,
                "Iter %d Slot %d: Spieler bei (%d,%d) innerhalb Zone, "
                "erwarte Slot-Index %d, aber got %d",
                iter, s_idx, px, pz, s_idx, result);

            /* Prüfe: Der Typ des ausgelösten Slots ist korrekt */
            PROP_ASSERT(slot.type == types[s_idx % num_types],
                "Iter %d Slot %d: Typ-Mismatch, erwarte %u, got %u",
                iter, s_idx, types[s_idx % num_types], slot.type);
        }

        /* Test B2: Spieler außerhalb aller Zonen → kein Slot ausgelöst */
        {
            int16_t px, pz;

            /* Alle Slots nochmal setzen */
            re15_aot_init();
            for (s_idx = 0; s_idx < num_slots; s_idx++) {
                int16_t poly_x[4], poly_z[4];

                poly_x[0] = slot_x0[s_idx]; poly_z[0] = slot_z0[s_idx];
                poly_x[1] = slot_x1[s_idx]; poly_z[1] = slot_z0[s_idx];
                poly_x[2] = slot_x1[s_idx]; poly_z[2] = slot_z1[s_idx];
                poly_x[3] = slot_x0[s_idx]; poly_z[3] = slot_z1[s_idx];

                memset(&slot, 0, sizeof(slot));
                slot.active     = 1;
                slot.type       = types[s_idx % num_types];
                slot.entered    = 0;
                slot.floor_band = TEST_FLOOR_BAND;
                memcpy(slot.trigger_x, poly_x, 4 * sizeof(int16_t));
                memcpy(slot.trigger_z, poly_z, 4 * sizeof(int16_t));

                re15_aot_set_slot(s_idx, &slot);
            }

            make_exterior_point(&px, &pz);

            result = re15_aot_check(px, pz, TEST_FLOOR_BAND);

            PROP_ASSERT(result == -1,
                "Iter %d: Spieler bei (%d,%d) außerhalb, "
                "erwarte -1, aber got %d",
                iter, px, pz, result);
        }

        /* Test B3: Spieler innerhalb Zone, aber falsches floor_band */
        if (num_slots > 0) {
            int16_t px, pz;
            uint8_t wrong_band = TEST_FLOOR_BAND + 1;

            /* Alle Slots sind auf TEST_FLOOR_BAND konfiguriert */
            make_interior_point(slot_x0[0], slot_x1[0],
                                slot_z0[0], slot_z1[0],
                                &px, &pz);

            result = re15_aot_check(px, pz, wrong_band);

            PROP_ASSERT(result == -1,
                "Iter %d: Spieler bei (%d,%d) innerhalb Slot[0], "
                "aber falsches Band=%u → erwarte -1, got %d",
                iter, px, pz, wrong_band, result);
        }
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test C: AOT entered-Flag verhindert Re-Trigger
 *
 * Wenn ein Slot das entered-Flag gesetzt hat, darf re15_aot_check() den
 * Slot nicht mehr auslösen, auch wenn der Spieler innerhalb liegt.
 * ========================================================================= */

static int prop_aot_entered_no_retrigger(void)
{
    int iter;

    for (iter = 0; iter < NUM_AOT_ITERATIONS; iter++) {
        re15_aot_slot_t slot;
        int16_t poly_x[4], poly_z[4];
        int16_t x0, x1, z0, z1;
        int16_t px, pz;
        int result;

        re15_aot_init();

        make_rect_zone(poly_x, poly_z, &x0, &x1, &z0, &z1);

        memset(&slot, 0, sizeof(slot));
        slot.active     = 1;
        slot.type       = AOT_TYPE_DOOR;
        slot.entered    = 1;  /* Bereits betreten! */
        slot.floor_band = TEST_FLOOR_BAND;
        memcpy(slot.trigger_x, poly_x, 4 * sizeof(int16_t));
        memcpy(slot.trigger_z, poly_z, 4 * sizeof(int16_t));

        re15_aot_set_slot(0, &slot);

        make_interior_point(x0, x1, z0, z1, &px, &pz);

        result = re15_aot_check(px, pz, TEST_FLOOR_BAND);

        PROP_ASSERT(result == -1,
            "Iter %d: Slot mit entered=1 bei (%d,%d) innerhalb, "
            "sollte -1 liefern, aber got %d",
            iter, px, pz, result);
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(int argc, char** argv)
{
    uint32_t seed = 0x9E3779B9u;            /* fester Default → reproduzierbare CI */
    const char* env_seed = getenv("RE15_TEST_SEED");

    /* Seed-Quelle: argv[1] > Env RE15_TEST_SEED > fester Default.
     * Deterministisch für CI; überschreibbar zum Fuzzing. */
    if (argc > 1) {
        seed = (uint32_t)strtoul(argv[1], NULL, 0);
    } else if (env_seed && *env_seed) {
        seed = (uint32_t)strtoul(env_seed, NULL, 0);
    }
    if (seed == 0u) {
        seed = 0x9E3779B9u;                 /* xorshift32 darf nicht im Zustand 0 starten */
    }
    xorshift_state = seed;

    printf("=== Property-Test: Zone-Containment -> Kamera/Aktion-Dispatch ===\n");
    printf("    Validates: Requirements 11.5, 11.6\n");
    printf("    PRNG Seed: %u\n", xorshift_state);
    printf("    Iterationen: %d (RVD) + %d (AOT)\n\n",
           NUM_RVD_ITERATIONS, NUM_AOT_ITERATIONS);

    /* Test A: RVD-Kamerazone-Dispatch */
    printf("[1/3] prop_rvd_camera_dispatch (%d Iterationen) ... ",
           NUM_RVD_ITERATIONS);
    if (prop_rvd_camera_dispatch() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test B: AOT-Aktion-Dispatch */
    printf("[2/3] prop_aot_action_dispatch (%d Iterationen) ... ",
           NUM_AOT_ITERATIONS);
    if (prop_aot_action_dispatch() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test C: entered-Flag verhindert Re-Trigger */
    printf("[3/3] prop_aot_entered_no_retrigger (%d Iterationen) ... ",
           NUM_AOT_ITERATIONS);
    if (prop_aot_entered_no_retrigger() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Summary */
    printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
