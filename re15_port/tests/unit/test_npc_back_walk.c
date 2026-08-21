/**
 * @file test_npc_back_walk.c
 * @brief PIN: die NPC-Executor-Subs 7 und 8 sind RUECKWAERTS-Subs.
 *
 * ZWILLING des Spieler-Befunds vom 2026-08-21 (ROOM1090-Cutscene, "Leon schaut nach rechts statt
 * nach links zum Transporter", gefixt in actor_locomotion.c / plc_yaw_slew, Commit 8ef5a283):
 * die Spieler-Modi 7/8 rufen den Yaw-Slew mit NEGATIVER Rate (`addiu a2,zero,-48` @0x80031254)
 * und die Translation mit Yaw-Offset 0x800 (`ori a0,zero,0x800` @0x8003125c).
 *
 * Die NPC-Seite (geteilte EXE-Bibliothek 0x80050be8, Sub-Tabelle @0x80076ca0) macht dasselbe:
 *   Sub 7 @0x80051908 / Sub 8 @0x80051b00 — KEIN arc_test, KEINE Align-Phase. Zweiwertiger
 *   Dispatch `lbu v0,6(v1); bne v0,zero,0x80051990` @0x80051918-20 bzw. @0x80051b10-18:
 *     +0x6 == 0 -> INIT (Speed 0x80076c20 @0x80051944 / 0x80076c60 @0x80051b3c, +0x6=1,
 *                  +0x94=1, +0x95=0, +0x8f=7), sonst der EINZIGE Body, der SOFORT translatiert.
 *   Body: `lbu a2,0(at)` aus 0x80076c21 (@0x800519b8) bzw. 0x80076c01 (@0x80051bb0),
 *         `jal 0x8001aac4` mit `subu a2,zero,a2` im DELAY-SLOT @0x800519c4 / @0x80051bbc
 *         = NEGATIVE RATE, danach `jal 0x800245d8` mit `ori a0,zero,0x800` @0x800519cc /
 *         @0x80051bc4 = 180-Grad-Translation.
 *   FUN_8001aac4 wertet das Vorzeichen aus: `bgez s0,0x8001ab1c` @0x8001ab08 ->
 *     `subu s1,zero,v0` @0x8001ab10 (Rate = -Rate) UND `addiu v0,a0,2048` @0x8001ab14
 *     (Peilung + 180 Grad).
 *   FUN_800245d8 addiert a0 auf rot_y vor RotMatrixY (`lh v0,106(v0); addu a0,v0,a0`
 *     @0x80024658-64).
 * Folge: BLICK = Peilung + 180 Grad, BEWEGUNG = Blick + 180 Grad = Peilung. Derselbe Weg,
 * gegenlaeufiger Blick.
 *
 * Der Port fuhr fuer 7/8 die Sub-4/5-Maschine (arc_test + Cone-Pivot + Vorwaertsschritt) — der
 * NPC schaute ZUM Ziel und ging vorwaerts; die Align-Phase kann beim Rueckwaertsgehen ausserdem
 * gar nicht terminieren (der Aktor dreht sich vom Ziel WEG, arc_test wird nie 0).
 * ERREICHBARKEIT (SCD-Walk ueber alle ausgelieferten RDTs, Opcode 0x40 Plc_dest, mode = pc[2]):
 * Mode 7 = 6 Records, Mode 8 = 7 Records — u.a. in ROOM1090, ROOM1211, ROOM20B0/20B1, ROOM3061,
 * ROOM3070/3071, ROOM4010, ROOM5031, ROOM50B0, ROOM50E0.
 *
 * Der Pin ist rein synthetisch (keine Assets) und deterministisch.
 */
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* Q12-Winkeldifferenz in (-2048, +2048] */
static int yawdiff(int a, int b) { int d = ((a - b) & 0xfff); if (d > 2048) d -= 4096; return d; }

static void run_sub(int sub, const char *name)
{
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 0;

    int s = re15_actor_alloc(0x40);                  /* Chief Irons — die geteilte NPC-Familie */
    if (s < 0) { CHECK(0, "%s: kein Aktor-Slot", name); return; }
    re15_actor_t *e = &g_actors[s];
    e->x = 0; e->y = 0; e->z = 0; e->hp = -1;
    /* Start-Yaw bewusst NICHT exakt 180 Grad vom Ziel entfernt: FUN_8001aac4 dreht dort
     * byte-true die "lange" Richtung (`slt v0,a1,rate*2` @0x8001ab40/44 und der 0x800-Test
     * @0x8001ab5c) — das ist korrekt, aber als Startbedingung fuer eine Konvergenz-Messung
     * degeneriert. */
    e->rot_y = 1500;
    e->state = 4; e->sub_state_1 = (uint8_t)sub; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->anim_flags = 0;                                /* kein Re-Arm (+0x1C4 & 4) */
    e->walk_active = 0;
    e->steer_x = 20000; e->steer_z = 0;               /* Ziel: reines +X */

    /* Peilung in DER Konvention, die re15_enemy_steer_point / re15_dog_advance benutzen
     * (Mesh: rot_y 0 = +X; der Steer zieht intern `- 0x400` ab) -> +X ist 0. */
    const int bearing = 0;

    int32_t x0 = e->x, z0 = e->z;
    for (int t = 0; t < 400; t++) {
        re15_enemy_ai_run_all(0);
        if (e->sub_state_1 != (uint8_t)sub) break;    /* angekommen -> Sub 6 */
    }
    int32_t dx = e->x - x0, dz = e->z - z0;
    int yd_target  = yawdiff((int)e->rot_y, bearing);
    int yd_away    = yawdiff((int)e->rot_y, (bearing + 2048) & 0xfff);

    printf("  [%s] rot_y=%d (Peilung %d) dPos=(%d,%d) Speed=%d s1=%u s2=%u\n",
           name, (int)e->rot_y & 0xfff, bearing, (int)dx, (int)dz,
           (int)e->crow_speed, e->sub_state_1, e->sub_state_2);

    CHECK(e->crow_speed > 0, "%s: INIT muss +0x8c aus der Speed-Tabelle fuellen (gemessen %d)",
          name, (int)e->crow_speed);
    /* (1) Der NPC BEWEGT sich zum Ziel (+X) — die Rueckwaerts-Subs translatieren ab dem
     * INIT-Tick (kein Align-Halt). */
    CHECK(dx > 1000,
          "%s: muss sich in Richtung Ziel (+X) bewegen — gemessen dx=%d dz=%d "
          "(die port-erfundene Align-Phase haelt hier fest)", name, (int)dx, (int)dz);
    /* (2) Er schaut dabei VOM Ziel WEG (Peilung + 180 Grad, `addiu v0,a0,2048` @0x8001ab14). */
    CHECK(yd_away > -256 && yd_away < 256,
          "%s: Blick muss Peilung+180 Grad sein (@0x8001ab14) — Abweichung %d (zum Ziel: %d)",
          name, yd_away, yd_target);
}

/* NEGATIV-KONTROLLE: Sub 4 (der VORWAERTS-Zwilling) muss unveraendert ZUM Ziel schauen —
 * sonst wuerde der Pin oben auch bei einem global falschen Vorzeichen gruen. */
static void run_forward_control(void)
{
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 0;

    int s = re15_actor_alloc(0x40);
    re15_actor_t *e = &g_actors[s];
    e->x = 0; e->y = 0; e->z = 0; e->rot_y = 1500; e->hp = -1;
    e->state = 4; e->sub_state_1 = 4; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->anim_flags = 0; e->walk_active = 0;
    e->steer_x = 20000; e->steer_z = 0;

    int32_t x0 = e->x;
    for (int t = 0; t < 400; t++) {
        re15_enemy_ai_run_all(0);
        if (e->sub_state_1 != 4) break;
    }
    int yd_target = yawdiff((int)e->rot_y, 0);
    printf("  [Sub 4 KONTROLLE] rot_y=%d dx=%d\n", (int)e->rot_y & 0xfff, (int)(e->x - x0));
    CHECK(yd_target > -256 && yd_target < 256,
          "Sub 4 (vorwaerts) muss ZUM Ziel schauen — Abweichung %d", yd_target);
}

int main(void)
{
    printf("== NPC-Executor-Subs 7/8 sind RUECKWAERTS-Subs ==\n");
    run_sub(7, "Sub 7");
    run_sub(8, "Sub 8");
    run_forward_control();
    if (fails) { printf("test_npc_back_walk: %d FAILURES\n", fails); return 1; }
    printf("test_npc_back_walk: OK\n");
    return 0;
}
