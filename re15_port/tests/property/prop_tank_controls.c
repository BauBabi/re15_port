/**
 * @file prop_tank_controls.c
 * @brief Property-Test: Tank-Controls Zustandsupdate
 *
 * **Validates: Requirements 11.2, 11.4**
 *
 * Property 12: Für jeden gültigen Spielerzustand und jede gültige
 * Eingabekombination wird geprüft:
 *   (a) Bewegung liegt exakt entlang Blickrichtung (sin/cos des Yaw)
 *   (b) Rotation bei Links/Rechts beträgt exakt ±ROT_SPEED
 *   (c) Yaw bleibt im Bereich [0, 4095] (Wrap-Around)
 *   (d) Animationszustand entspricht der Geschwindigkeitsstufe
 *
 * Nutzt 300 Iterationen mit pseudo-zufälligen Spielerzuständen und Inputs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_player.h"
#include "re15_input.h"
#include "re15_math.h"

/* =========================================================================
 * Test-Infrastruktur
 * ========================================================================= */

static int g_passed = 0;
static int g_failed = 0;

#define NUM_ITERATIONS 300

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

static uint32_t prng_state = 0xDEADBEEF;

static uint32_t prng_next(void)
{
    uint32_t x = prng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    prng_state = x;
    return x;
}

static int16_t prng_yaw(void)
{
    return (int16_t)(prng_next() & 0x0FFF);
}

static int32_t prng_position(void)
{
    /* Zufällige Position im vernünftigen Bereich ±100000 Q12-Einheiten */
    return (int32_t)(prng_next() % 200001) - 100000;
}

static uint16_t prng_input_buttons(void)
{
    /* Generiere zufällige Button-Kombinationen (Fokus auf Bewegungs-Buttons) */
    uint16_t buttons = 0;
    uint32_t r = prng_next();

    if (r & 0x01) buttons |= RE15_BTN_UP;
    if (r & 0x02) buttons |= RE15_BTN_DOWN;
    if (r & 0x04) buttons |= RE15_BTN_LEFT;
    if (r & 0x08) buttons |= RE15_BTN_RIGHT;
    if (r & 0x10) buttons |= RE15_BTN_R1;

    return buttons;
}

/* =========================================================================
 * Externe Funktion unter Test
 * ========================================================================= */

extern void re15_player_update(re15_player_t* player, re15_input_state_t input);

/* =========================================================================
 * Property (a): Bewegung entlang Blickrichtung
 *
 * Wenn UP gehalten: dx == (speed * sin(yaw)) >> 12, dz == (speed * cos(yaw)) >> 12
 * Wenn DOWN gehalten: dx == (-SPEED_WALK * sin(yaw)) >> 12, etc.
 * Wenn weder UP noch DOWN: dx == 0, dz == 0
 * ========================================================================= */

static int test_movement_along_facing(int iter, re15_player_t* player_before,
                                      re15_player_t* player_after,
                                      re15_input_state_t input)
{
    int16_t expected_speed;
    int32_t expected_dx, expected_dz;
    int32_t actual_dx, actual_dz;

    /* Bestimme erwartete Geschwindigkeit */
    if (input.held & RE15_BTN_UP) {
        if (input.held & RE15_BTN_R1) {
            expected_speed = RE15_SPEED_RUN;
        } else {
            expected_speed = RE15_SPEED_WALK;
        }
    } else if (input.held & RE15_BTN_DOWN) {
        expected_speed = -((int16_t)RE15_SPEED_WALK);
    } else {
        expected_speed = 0;
    }

    /* Berechne erwartete Positionsänderung mit dem POST-Rotation Yaw */
    if (expected_speed != 0) {
        expected_dx = ((int32_t)expected_speed * re15_sin_q12(player_after->yaw)) >> Q12_SHIFT;
        expected_dz = ((int32_t)expected_speed * re15_cos_q12(player_after->yaw)) >> Q12_SHIFT;
    } else {
        expected_dx = 0;
        expected_dz = 0;
    }

    actual_dx = player_after->x - player_before->x;
    actual_dz = player_after->z - player_before->z;

    PROP_ASSERT(actual_dx == expected_dx,
        "Movement dx: got %d, expected %d (speed=%d, yaw=%d)",
        (int)actual_dx, (int)expected_dx, (int)expected_speed, (int)player_after->yaw);

    PROP_ASSERT(actual_dz == expected_dz,
        "Movement dz: got %d, expected %d (speed=%d, yaw=%d)",
        (int)actual_dz, (int)expected_dz, (int)expected_speed, (int)player_after->yaw);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property (b): Rotation ±ROT_SPEED
 *
 * Wenn LEFT gehalten: new_yaw == (old_yaw - ROT_SPEED) & 0x0FFF
 * Wenn RIGHT gehalten: new_yaw == (old_yaw + ROT_SPEED) & 0x0FFF
 * Wenn beide: new_yaw == (old_yaw - ROT_SPEED + ROT_SPEED) & 0x0FFF == old_yaw
 * Wenn weder: new_yaw == old_yaw
 * ========================================================================= */

static int test_rotation_speed(int iter, re15_player_t* player_before,
                               re15_player_t* player_after,
                               re15_input_state_t input)
{
    int16_t expected_yaw = player_before->yaw;

    if (input.held & RE15_BTN_LEFT) {
        expected_yaw -= RE15_ROT_SPEED;
    }
    if (input.held & RE15_BTN_RIGHT) {
        expected_yaw += RE15_ROT_SPEED;
    }
    expected_yaw = expected_yaw & 0x0FFF;

    PROP_ASSERT(player_after->yaw == expected_yaw,
        "Rotation: got yaw=%d, expected %d (old_yaw=%d, LEFT=%d, RIGHT=%d)",
        (int)player_after->yaw, (int)expected_yaw, (int)player_before->yaw,
        (input.held & RE15_BTN_LEFT) ? 1 : 0,
        (input.held & RE15_BTN_RIGHT) ? 1 : 0);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property (c): Yaw im Bereich [0, 4095]
 * ========================================================================= */

static int test_yaw_range(int iter, re15_player_t* player_after)
{
    PROP_ASSERT(player_after->yaw >= 0 && player_after->yaw <= 4095,
        "Yaw range: got %d, expected [0, 4095]",
        (int)player_after->yaw);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property (d): Animationszustand korrekt
 *
 * speed == 0        → IDLE
 * speed == SPEED_WALK (oder -SPEED_WALK) → WALK
 * speed == SPEED_RUN → RUN
 * ========================================================================= */

static int test_animation_state(int iter, re15_player_t* player_after,
                                re15_input_state_t input)
{
    int16_t expected_speed;
    uint8_t expected_motion;

    /* Bestimme erwartete Geschwindigkeit (gleiche Logik wie Property a) */
    if (input.held & RE15_BTN_UP) {
        if (input.held & RE15_BTN_R1) {
            expected_speed = RE15_SPEED_RUN;
        } else {
            expected_speed = RE15_SPEED_WALK;
        }
    } else if (input.held & RE15_BTN_DOWN) {
        expected_speed = -((int16_t)RE15_SPEED_WALK);
    } else {
        expected_speed = 0;
    }

    /* Bestimme erwarteten Motion-State */
    if (expected_speed == 0) {
        expected_motion = RE15_MOTION_IDLE;
    } else if (expected_speed == RE15_SPEED_RUN) {
        expected_motion = RE15_MOTION_RUN;
    } else {
        expected_motion = RE15_MOTION_WALK;
    }

    PROP_ASSERT(player_after->motion_state == expected_motion,
        "Motion state: got %d, expected %d (speed=%d)",
        (int)player_after->motion_state, (int)expected_motion,
        (int)player_after->speed);

    PROP_ASSERT(player_after->speed == expected_speed,
        "Speed value: got %d, expected %d",
        (int)player_after->speed, (int)expected_speed);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Main — Property-Tests mit zufälligen Spielerzuständen und Inputs
 * ========================================================================= */

int main(void)
{
    int iter;
    int failures = 0;

    printf("=== Property-Test: Tank-Controls Zustandsupdate ===\n");
    printf("    Validates: Requirements 11.2, 11.4\n");
    printf("    Iterationen: %d\n\n", NUM_ITERATIONS);

    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        re15_player_t player_before;
        re15_player_t player_after;
        re15_input_state_t input;

        /* Zufälligen Spielerzustand generieren */
        memset(&player_before, 0, sizeof(re15_player_t));
        player_before.x = prng_position();
        player_before.y = prng_position();
        player_before.z = prng_position();
        player_before.yaw = prng_yaw();
        player_before.speed = 0;
        player_before.motion_state = RE15_MOTION_IDLE;

        /* Zufälligen Input generieren */
        memset(&input, 0, sizeof(re15_input_state_t));
        input.held = prng_input_buttons();
        input.pressed = 0;
        input.released = 0;

        /* Kopie für Vergleich */
        memcpy(&player_after, &player_before, sizeof(re15_player_t));

        /* Funktion unter Test aufrufen */
        re15_player_update(&player_after, input);

        /* Properties prüfen */
        if (test_yaw_range(iter, &player_after) != 0) {
            failures++;
            break;
        }

        if (test_rotation_speed(iter, &player_before, &player_after, input) != 0) {
            failures++;
            break;
        }

        if (test_movement_along_facing(iter, &player_before, &player_after, input) != 0) {
            failures++;
            break;
        }

        if (test_animation_state(iter, &player_after, input) != 0) {
            failures++;
            break;
        }
    }

    if (failures == 0) {
        printf("  Alle %d Iterationen bestanden.\n", NUM_ITERATIONS);
    }

    printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
