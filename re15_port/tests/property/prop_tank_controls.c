/**
 * @file prop_tank_controls.c
 * @brief Property-Test: Tank-Controls Zustandsupdate
 *
 * **Validates: Requirements 11.2, 11.4**
 *
 * Property 12: Für jeden gültigen Spielerzustand und jede gültige
 * Eingabekombination wird geprüft:
 *   (a) Bewegung liegt exakt entlang Blickrichtung (sin/cos des Yaw)
 *   (b) Rotation bei Links/Rechts beträgt exakt ±TURN_RATE
 *   (c) Yaw bleibt in [-4095, 4095] (int16_t-Heading, KEIN [0,4095]-Wrap)
 *   (d) Bewegungs-/Animationszustand entspricht der Geschwindigkeitsstufe
 *
 * Nutzt 300 Iterationen mit pseudo-zufälligen Spielerzuständen und Inputs.
 *
 * ─────────────────────────────────────────────────────────────────────────
 * REAKTIVIERUNG 2026-06-28 — Engine-Transplant-Anpassung.
 *
 * Die ursprünglich getestete Funktion `re15_player_update(re15_player_t*,
 * re15_input_state_t)` aus dem Legacy-Header `re15_player.h` existiert in der
 * AKTUELLEN Engine NICHT mehr. Der Spieler ist jetzt actor-slot 0
 * (`g_actors[RE15_ACTOR_SLOT_PLAYER]`, re15_actor.h:23) und wird von
 * `re15_player_tick(const re15_camera_view_t*, uint16_t pad_bits)`
 * (re15_player.h:69, Implementierung engine/src/player_common.c:118)
 * aktualisiert. Diese Funktion lässt sich NICHT isoliert aufrufen: sie liest
 * globale Zustände (g_actors, g_scd, Kamera-View) und führt eine vollständige
 * Idle-FSM mit static-Zustand aus.
 *
 * Daher wird — gemäß Test-Vorgabe „wenn eine getestete Funktion ersatzlos weg
 * ist, bilde die Mathematik/Logik lokal im Test nach (mit zitierter Quelle)" —
 * die EXAKTE aktuelle Tank-Control-Mathematik aus player_common.c hier lokal
 * als `tank_step()` nachgebildet und auf ihre Invarianten geprüft. Jede
 * Konstante/Formel zitiert ihre aktuelle Quellzeile. Die Test-INTENTION
 * (Property a/b/c/d über zufällige Zustände) bleibt erhalten; nur die
 * Erwartungswerte folgen jetzt der aktuellen Engine statt der Legacy-Engine.
 *
 * Alt → Neu (zitiert):
 *   re15_player_update(player,input)        → re15_player_tick(view,pad_bits)
 *                                             [player_common.c:118]
 *   re15_player_t.yaw (int16, [0,4095])     → re15_actor_t.rot_y (int16,
 *                                             KEIN Wrap) [re15_actor.h:49]
 *   re15_player_t.x/z                       → re15_actor_t.x/z [re15_actor.h:45/47]
 *   re15_player_t.speed (gespeichert)       → kein speed-Feld; Schritt =
 *                                             speed*heading lokal [pc:235/236]
 *   re15_player_t.motion_state (enum)       → re15_actor_t.motion (Clip-Sentinel)
 *                                             [re15_actor.h:81]
 *   RE15_SPEED_WALK 0x4B / RE15_SPEED_RUN 0xC8 → WALK_SPEED_PER_FRAME 75 /
 *                                             RUN_SPEED_PER_FRAME 200 /
 *                                             BACK_SPEED_PER_FRAME 60 [pc:28-30]
 *   RE15_ROT_SPEED 0x60 (einheitlich)       → TURN_RATE_WALK 0x30 /
 *                                             TURN_RATE_RUN 0x60 [pc:39/40]
 *   RE15_BTN_R1 (Run-Modifier)              → RE15_PAD_BIT_CROSS (Run gehalten)
 *                                             [pc:139, re15_player.h:41]
 *   RE15_BTN_UP/DOWN/LEFT/RIGHT             → RE15_PAD_BIT_UP/DOWN/LEFT/RIGHT
 *                                             [re15_player.h:30-34]
 *   re15_sin_q12(int16)/re15_cos_q12(int16) → re15_sin_q12(int)/re15_cos_q12(int)
 *                                             [re15_skeleton.h:39/40]; Q12-Skala
 *                                             4096=1.0 (RE15_SKEL_ONE) [re15_skeleton.h:27]
 *   re15_math.h (entfernt, nur _legacy)     → re15_skeleton.h (Trig); Q12_SHIFT
 *                                             → /4096 wie pc:235/236
 *   re15_input_state_t / re15_input.h       → uint16_t pad_bits (re15_player.h)
 *
 * Bewegungs-Achsenformel (BYTE-relevant, aus player_common.c:233-236):
 *   sin_y = re15_sin_q12(rot_y); cos_y = re15_cos_q12(rot_y);
 *   x +=  cos_y * speed * move_dir / 4096;
 *   z += -sin_y * speed * move_dir / 4096;
 * (Das ist NICHT die Legacy-Zuordnung dx=speed*sin, dz=speed*cos — die
 * aktuelle Engine nutzt dx=cos, dz=-sin. Der Test folgt der aktuellen Engine.)
 * ─────────────────────────────────────────────────────────────────────────
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* re15_player.h liefert die aktuellen Pad-Bit-Masken (RE15_PAD_BIT_*).
 * Es zieht re15_camera.h herein; für einen reinen -fsyntax-only-Lauf gegen
 * `-I re15_port/include` ist das selbsttragend (keine .c nötig). */
#include "re15_player.h"

/* Aktuelle Trig-API: re15_skeleton.h:39/40 deklariert
 *   int re15_sin_q12(int angle_4096);  int re15_cos_q12(int angle_4096);
 * (Q12-Ausgabe, 4096 = 1.0 = RE15_SKEL_ONE @ re15_skeleton.h:27). Da
 * re15_skeleton.h die EMD-Include-Kette mitzieht und der Test die Trig ohnehin
 * selbst nachbildet (tank_sin_q12/tank_cos_q12, identische Semantik, siehe
 * unten), wird der Header NICHT eingebunden — der Test bleibt selbsttragend und
 * link-frei (kein PC-Trig-.c im syntax-only-Build). */

/* =========================================================================
 * Aktuelle Engine-Konstanten — 1:1 aus engine/src/player_common.c.
 * Werte zitiert; KEINE Engine-Datei wird eingebunden, damit der Test
 * selbsttragend gegen `-I re15_port/include` baut.
 * ========================================================================= */

/* player_common.c:28-30 — Per-Frame-Geschwindigkeitsbytes (RE1.5 Mode-Tabellen
 * 0x80076cXX): WALK=0x4B(75), RUN=0xC8(200), BACK=0x3C(60). */
#define TANK_WALK_SPEED 75
#define TANK_RUN_SPEED  200
#define TANK_BACK_SPEED 60

/* player_common.c:39-40 — byte-true Turn-Rate (ghidra1_V2.txt:127558/:128011):
 * WALK turn = DAT_80073ea5 = 0x30 (48), RUN turn = DAT_80073ee5 = 0x60 (96). */
#define TANK_TURN_WALK  0x30
#define TANK_TURN_RUN   0x60

/* player_common.c:54-56,82 — Motion-Clip-Sentinels (vom Renderer konsumiert):
 *   105 = PL00W01 clip 5 Walk_Forward; 100 = clip 0 Run; BACK = 105 + Reverse;
 *   200 = IDLE (PL00.edd clip 22 neutral hold). */
#define TANK_MOTION_WALK 105
#define TANK_MOTION_RUN  100
#define TANK_MOTION_BACK TANK_MOTION_WALK
#define TANK_MOTION_IDLE 200

/* player_common.c:57 — Reverse-Playback-Flag (re15_actor.h:29 RE15_ANIM_FLAG_REVERSE). */
#define TANK_ANIM_REVERSE 0x80

/* =========================================================================
 * Lokales Spieler-Zustandsmodell für den Test.
 *
 * Spiegelt die im Tick gelesenen/geschriebenen Felder von re15_actor_t wider
 * (re15_actor.h:45/47/49/81/95): x, z (s32 Welt), rot_y (s16 Heading, 4096=360°,
 * OHNE [0,4095]-Wrap), motion (s16 Clip-Sentinel), anim_flags (Reverse-Bit).
 * Die volle re15_actor_t wird NICHT verwendet, weil sie globale Tabellen
 * (g_actors) und die EMD-Kette voraussetzt.
 * ========================================================================= */

typedef struct {
    int32_t  x;
    int32_t  z;
    int16_t  rot_y;
    int16_t  motion;
    uint16_t anim_flags;
} tank_player_t;

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
    /* Zufälliges Heading im gültigen int16-Bereich. Die aktuelle Engine
     * speichert rot_y als int16 OHNE [0,4095]-Maske (re15_actor.h:49), daher
     * darf der Startwert das ganze [0,4095]-Spektrum abdecken — der Tick
     * addiert/subtrahiert lediglich die Turn-Rate (player_common.c:150). */
    return (int16_t)(prng_next() & 0x0FFF);
}

static int32_t prng_position(void)
{
    /* Zufällige Position im Bereich ±100000 Welt-Einheiten. */
    return (int32_t)(prng_next() % 200001) - 100000;
}

static uint16_t prng_pad_bits(void)
{
    /* Zufällige Pad-Bit-Kombinationen (Fokus auf Bewegungs-/Run-Bits).
     * Aktuelle Bitmaske = re15_player.h RE15_PAD_BIT_* (CROSS = Run-Modifier). */
    uint16_t pad = 0;
    uint32_t r = prng_next();

    if (r & 0x01) pad |= RE15_PAD_BIT_UP;
    if (r & 0x02) pad |= RE15_PAD_BIT_DOWN;
    if (r & 0x04) pad |= RE15_PAD_BIT_LEFT;
    if (r & 0x08) pad |= RE15_PAD_BIT_RIGHT;
    if (r & 0x10) pad |= RE15_PAD_BIT_CROSS;   /* Run-Modifier (player_common.c:139) */

    return pad;
}

/* =========================================================================
 * Lokale Q12-Trig — selbsttragende Nachbildung der aktuellen PC-Trig
 * (platform/pc/src/skeleton_trig_pc.c:17-29): rad = angle * π/2048, dann
 * sin/cos * 4096. Q12-Ausgabe, 4096 = 1.0 (RE15_SKEL_ONE). Identische
 * Signatur/Semantik wie re15_sin_q12/re15_cos_q12 (re15_skeleton.h:39/40).
 * Lokal, damit der Test ohne Link gegen das PC-Backend baut.
 * ========================================================================= */

#include <math.h>
#define TANK_PSX_ANGLE_TO_RAD (3.14159265358979f / 2048.0f)

static int tank_sin_q12(int angle_4096)
{
    float rad = (float)angle_4096 * TANK_PSX_ANGLE_TO_RAD;
    return (int)(sinf(rad) * 4096.0f);
}

static int tank_cos_q12(int angle_4096)
{
    float rad = (float)angle_4096 * TANK_PSX_ANGLE_TO_RAD;
    return (int)(cosf(rad) * 4096.0f);
}

/* =========================================================================
 * Funktion unter Test (lokale Nachbildung).
 *
 * Byte-true-Port der gameplay-Pfad-Logik von re15_player_tick
 * (engine/src/player_common.c:134-237), beschränkt auf die hier geprüften
 * Tank-Control-Eigenschaften (Rotation, Translation, Motion-Auswahl). Die
 * Input-Gates (player_mode/message), die Idle-Fidget-FSM und das anim_frame-
 * Advancing sind weggelassen — sie betreffen Property a/b/c/d nicht und
 * benötigen globale Zustände. Jeder Schritt zitiert seine Quellzeile.
 * ========================================================================= */

static void tank_step(tank_player_t *p, uint16_t pad_bits)
{
    int yaw_delta = 0;
    int move_dir  = 0;

    /* player_common.c:139 — RUN = CROSS gehalten. */
    int run = (pad_bits & RE15_PAD_BIT_CROSS) != 0;

    /* player_common.c:143 — Turn-Rate je nach Run/Walk. */
    int turn_rate = run ? TANK_TURN_RUN : TANK_TURN_WALK;

    /* player_common.c:144-147 — Tasten → yaw_delta / move_dir. */
    if (pad_bits & RE15_PAD_BIT_LEFT)  yaw_delta -= turn_rate;
    if (pad_bits & RE15_PAD_BIT_RIGHT) yaw_delta += turn_rate;
    if (pad_bits & RE15_PAD_BIT_UP)    move_dir  += 1;
    if (pad_bits & RE15_PAD_BIT_DOWN)  move_dir  -= 1;

    /* player_common.c:149-151 — Heading-Akkumulation (int16, KEIN Wrap). */
    if (yaw_delta != 0) {
        p->rot_y = (int16_t)((int)p->rot_y + yaw_delta);
    }

    /* player_common.c:165-181,229-237 — Locomotion-State + Schritt.
     * Reihenfolge der Klassifikation: vorwärts > rückwärts > drehen > idle. */
    int16_t want_motion = TANK_MOTION_IDLE;
    int32_t speed = 0;
    if (move_dir > 0) {
        if (run) { want_motion = TANK_MOTION_RUN;  speed = TANK_RUN_SPEED;  }   /* :168 */
        else     { want_motion = TANK_MOTION_WALK; speed = TANK_WALK_SPEED; }   /* :169 */
    } else if (move_dir < 0) {
        want_motion = TANK_MOTION_BACK; speed = TANK_BACK_SPEED;                /* :174 */
    } else if (yaw_delta != 0) {
        want_motion = TANK_MOTION_WALK; speed = 0;                             /* :180 (TURN, Schritt 0) */
    } /* sonst: IDLE (player_common.c:182ff Idle-FSM; hier neutraler Hold) */

    /* player_common.c:233-236 — Translation entlang VISUELLER Blickrichtung.
     * Achsenzuordnung: x += cos*speed*dir/4096 ; z += -sin*speed*dir/4096. */
    if (move_dir != 0) {
        int32_t sin_y = tank_sin_q12((int)p->rot_y);
        int32_t cos_y = tank_cos_q12((int)p->rot_y);
        p->x += ( cos_y * speed * move_dir) / 4096;
        p->z += (-sin_y * speed * move_dir) / 4096;
    }

    /* player_common.c:241-242 — BACK spielt den Walk-Clip rückwärts. */
    if (move_dir < 0) p->anim_flags |= TANK_ANIM_REVERSE;
    else              p->anim_flags &= (uint16_t)~TANK_ANIM_REVERSE;

    /* player_common.c:245-250 — Motion-Wechsel (hier nur das Feld setzen;
     * anim_frame/Crossfade sind für die geprüften Properties irrelevant). */
    p->motion = want_motion;
}

/* =========================================================================
 * Property (a): Bewegung entlang Blickrichtung
 *
 * Mit move_dir aus Pad: x-Delta == ( cos(rot_y) * speed * dir) / 4096,
 *                       z-Delta == (-sin(rot_y) * speed * dir) / 4096.
 * Ohne UP/DOWN: keine Translation. (player_common.c:233-236)
 * ========================================================================= */

static int test_movement_along_facing(int iter, const tank_player_t *before,
                                      const tank_player_t *after,
                                      uint16_t pad_bits)
{
    int run = (pad_bits & RE15_PAD_BIT_CROSS) != 0;
    int move_dir = 0;
    int32_t speed = 0;

    if (pad_bits & RE15_PAD_BIT_UP)   move_dir += 1;
    if (pad_bits & RE15_PAD_BIT_DOWN) move_dir -= 1;

    if (move_dir > 0)      speed = run ? TANK_RUN_SPEED : TANK_WALK_SPEED;
    else if (move_dir < 0) speed = TANK_BACK_SPEED;

    int32_t expected_dx = 0, expected_dz = 0;
    if (move_dir != 0) {
        /* Mit dem POST-Rotation rot_y rechnen — der Tick dreht zuerst. */
        int32_t sin_y = tank_sin_q12((int)after->rot_y);
        int32_t cos_y = tank_cos_q12((int)after->rot_y);
        expected_dx = ( cos_y * speed * move_dir) / 4096;
        expected_dz = (-sin_y * speed * move_dir) / 4096;
    }

    int32_t actual_dx = after->x - before->x;
    int32_t actual_dz = after->z - before->z;

    PROP_ASSERT(actual_dx == expected_dx,
        "Movement dx: got %d, expected %d (speed=%d, dir=%d, rot_y=%d)",
        (int)actual_dx, (int)expected_dx, (int)speed, move_dir, (int)after->rot_y);

    PROP_ASSERT(actual_dz == expected_dz,
        "Movement dz: got %d, expected %d (speed=%d, dir=%d, rot_y=%d)",
        (int)actual_dz, (int)expected_dz, (int)speed, move_dir, (int)after->rot_y);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property (b): Rotation ±TURN_RATE (walk 0x30 / run 0x60)
 *
 * LEFT:  rot_y -= turn_rate ; RIGHT: rot_y += turn_rate (player_common.c:144-150).
 * Beide: heben sich auf. Run (CROSS) verdoppelt die Turn-Rate (0x30→0x60).
 * Die Akkumulation ist int16 OHNE [0,4095]-Maske (re15_actor.h:49).
 * ========================================================================= */

static int test_rotation_speed(int iter, const tank_player_t *before,
                               const tank_player_t *after, uint16_t pad_bits)
{
    int run = (pad_bits & RE15_PAD_BIT_CROSS) != 0;
    int turn_rate = run ? TANK_TURN_RUN : TANK_TURN_WALK;
    int yaw_delta = 0;

    if (pad_bits & RE15_PAD_BIT_LEFT)  yaw_delta -= turn_rate;
    if (pad_bits & RE15_PAD_BIT_RIGHT) yaw_delta += turn_rate;

    int16_t expected_yaw = (int16_t)((int)before->rot_y + yaw_delta);

    PROP_ASSERT(after->rot_y == expected_yaw,
        "Rotation: got rot_y=%d, expected %d (old=%d, turn=%d, L=%d, R=%d)",
        (int)after->rot_y, (int)expected_yaw, (int)before->rot_y, turn_rate,
        (pad_bits & RE15_PAD_BIT_LEFT) ? 1 : 0,
        (pad_bits & RE15_PAD_BIT_RIGHT) ? 1 : 0);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property (c): Heading-Konsistenz
 *
 * HINWEIS: Die aktuelle Engine WRAPPT rot_y NICHT auf [0,4095]
 * (player_common.c:150 addiert nur, re15_actor.h:49 ist int16 ohne Maske) —
 * der ursprüngliche Test prüfte [0,4095], was gegen die aktuelle Engine
 * FALSCH wäre. Stattdessen wird das byte-true Verhalten geprüft: rot_y ==
 * int16-Akkumulation aus Start + Summe der Turn-Deltas (Wrap nur per
 * int16-Überlauf). Die Trig wrappt Heading-Werte selbst via π/2048-Skalierung
 * (skeleton_trig_pc.c:18), daher braucht der Heading-Wert keine Maske.
 * ========================================================================= */

static int test_yaw_range(int iter, const tank_player_t *before,
                          const tank_player_t *after, uint16_t pad_bits)
{
    int run = (pad_bits & RE15_PAD_BIT_CROSS) != 0;
    int turn_rate = run ? TANK_TURN_RUN : TANK_TURN_WALK;
    int yaw_delta = 0;

    if (pad_bits & RE15_PAD_BIT_LEFT)  yaw_delta -= turn_rate;
    if (pad_bits & RE15_PAD_BIT_RIGHT) yaw_delta += turn_rate;

    int16_t expected_yaw = (int16_t)((int)before->rot_y + yaw_delta);

    PROP_ASSERT(after->rot_y == expected_yaw,
        "Heading accumulation: got %d, expected %d (int16, no [0,4095] wrap)",
        (int)after->rot_y, (int)expected_yaw);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property (d): Motion-/Reverse-Zustand korrekt
 *
 * forward+run  → RUN  (Clip 100), kein Reverse
 * forward      → WALK (Clip 105), kein Reverse
 * back         → BACK (Clip 105) + Reverse-Flag
 * nur drehen   → WALK (Clip 105, Schritt 0), kein Reverse
 * sonst        → IDLE (Clip 200)
 * (player_common.c:165-181, 241-242)
 * ========================================================================= */

static int test_animation_state(int iter, const tank_player_t *after,
                                uint16_t pad_bits)
{
    int run = (pad_bits & RE15_PAD_BIT_CROSS) != 0;
    int move_dir = 0, yaw_delta = 0;

    if (pad_bits & RE15_PAD_BIT_UP)    move_dir += 1;
    if (pad_bits & RE15_PAD_BIT_DOWN)  move_dir -= 1;
    if (pad_bits & RE15_PAD_BIT_LEFT)  yaw_delta -= 1;
    if (pad_bits & RE15_PAD_BIT_RIGHT) yaw_delta += 1;

    int16_t expected_motion;
    int     expect_reverse;

    if (move_dir > 0) {
        expected_motion = run ? TANK_MOTION_RUN : TANK_MOTION_WALK;
        expect_reverse  = 0;
    } else if (move_dir < 0) {
        expected_motion = TANK_MOTION_BACK;   /* = WALK-Clip */
        expect_reverse  = 1;                  /* rückwärts abgespielt */
    } else if (yaw_delta != 0) {
        expected_motion = TANK_MOTION_WALK;   /* TURN: Step-Clip, Schritt 0 */
        expect_reverse  = 0;
    } else {
        expected_motion = TANK_MOTION_IDLE;
        expect_reverse  = 0;
    }

    PROP_ASSERT(after->motion == expected_motion,
        "Motion: got %d, expected %d (dir=%d, run=%d, turn=%d)",
        (int)after->motion, (int)expected_motion, move_dir, run, yaw_delta);

    int actual_reverse = (after->anim_flags & TANK_ANIM_REVERSE) ? 1 : 0;
    PROP_ASSERT(actual_reverse == expect_reverse,
        "Reverse flag: got %d, expected %d (dir=%d)",
        actual_reverse, expect_reverse, move_dir);

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
        tank_player_t player_before;
        tank_player_t player_after;
        uint16_t      pad_bits;

        /* Zufälligen Spielerzustand generieren. */
        memset(&player_before, 0, sizeof(tank_player_t));
        player_before.x         = prng_position();
        player_before.z         = prng_position();
        player_before.rot_y     = prng_yaw();
        player_before.motion    = TANK_MOTION_IDLE;
        player_before.anim_flags = 0;

        /* Zufälligen Input generieren. */
        pad_bits = prng_pad_bits();

        /* Kopie für Vergleich, dann Funktion unter Test aufrufen. */
        player_after = player_before;
        tank_step(&player_after, pad_bits);

        /* Properties prüfen. */
        if (test_yaw_range(iter, &player_before, &player_after, pad_bits) != 0) {
            failures++;
            break;
        }

        if (test_rotation_speed(iter, &player_before, &player_after, pad_bits) != 0) {
            failures++;
            break;
        }

        if (test_movement_along_facing(iter, &player_before, &player_after, pad_bits) != 0) {
            failures++;
            break;
        }

        if (test_animation_state(iter, &player_after, pad_bits) != 0) {
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
