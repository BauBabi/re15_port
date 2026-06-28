/**
 * @file test_member.c
 * @brief Unit-Tests fuer die byte-true RE1.5 Member_set/Member_cmp id-Tabelle (#11/#12).
 *
 * Der Port uebersetzte RE1.5-Member-ids frueher nach RE2-ids — eine Fiktion:
 * RE1.5 nutzt die DIREKTE Tabelle FUN_8004116c (set) / FUN_80041358 (get),
 * ids 0..0x13 -> Actor-Offsets. Folgen des alten Modells:
 *   - id12 (haeufigster Member_set!) schrieb -> Leon.y = Spieler unter Boden.
 *   - id8 schrieb -> motion statt state; id6/9/10/11/13/16/17/18/19 falsch/gedroppt.
 *
 * Tests:
 *   1. Direkt-Tabelle (re15_actor_set_member/get_member): id->Feld byte-true,
 *      besonders id12!=y, id8=state(!=motion), id19=hp(signed), Koords id0-5 unveraendert.
 *   2. GET-Vorzeichen: lbu zero-extend (state 200), lh sign-extend (hp -7).
 *   3. Member_cmp (0x3E) via VM: liest das RICHTIGE Feld, Wert LE, Operatoren 0..6.
 */
#include "re15_scd.h"
#include "re15_actor.h"

#include <stdint.h>
#include <stdio.h>

/* ---- Test 1+2: Direkt-id-Tabelle + GET-Vorzeichen ---- */
static int test_member_table(void)
{
    re15_actor_init();
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* id12 (+0x09 grid_id) — der Bug: DARF y NICHT anfassen. */
    int32_t y0 = p->y;
    re15_actor_set_member(0, 12, 138);
    if (p->y != y0)      { fprintf(stderr, "FAIL: id12 hat y veraendert (%d, war %d)\n", p->y, y0); return 1; }
    if (p->grid_id != 138) { fprintf(stderr, "FAIL: id12 -> grid_id sollte 138 sein, ist %d\n", p->grid_id); return 1; }
    if (re15_actor_get_member(0, 12) != 138) { fprintf(stderr, "FAIL: get id12 != 138\n"); return 1; }

    /* id8 (+0x04 state) — DARF motion NICHT anfassen (alt: id8->motion). */
    p->motion = 99;
    re15_actor_set_member(0, 8, 5);
    if (p->state != 5)   { fprintf(stderr, "FAIL: id8 -> state sollte 5 sein, ist %d\n", p->state); return 1; }
    if (p->motion != 99) { fprintf(stderr, "FAIL: id8 hat motion veraendert (%d)\n", p->motion); return 1; }
    if (re15_actor_get_member(0, 8) != 5) { fprintf(stderr, "FAIL: get id8 != 5\n"); return 1; }

    /* id19 (+0x1ba hp, lh signed) — negativer Wert sign-extend. */
    re15_actor_set_member(0, 19, -7);
    if (p->hp != -7) { fprintf(stderr, "FAIL: id19 -> hp sollte -7 sein, ist %d\n", p->hp); return 1; }
    if (re15_actor_get_member(0, 19) != -7) { fprintf(stderr, "FAIL: get id19 (hp) sign-extend != -7\n"); return 1; }

    /* Koords id0/1/2 + rot id3/4/5 — byte-true UNVERAENDERT ggue. altem Modell (=> ROOM1170 identisch). */
    re15_actor_set_member(0, 0, -6010); re15_actor_set_member(0, 1, 42); re15_actor_set_member(0, 2, 6265);
    re15_actor_set_member(0, 3, 100);   re15_actor_set_member(0, 4, 1024); re15_actor_set_member(0, 5, -50);
    if (p->x != -6010 || p->y != 42 || p->z != 6265) { fprintf(stderr, "FAIL: Koords id0/1/2 falsch (%d,%d,%d)\n", p->x,p->y,p->z); return 1; }
    if (p->rot_x != 100 || p->rot_y != 1024 || p->rot_z != -50) { fprintf(stderr, "FAIL: rot id3/4/5 falsch\n"); return 1; }

    /* id16 anim_flags (+0x1c4 lhu), id17 status_flags (+0x98 lhu), id18 floor (+0x82 lbu). */
    re15_actor_set_member(0, 16, 0x1234);
    if (p->anim_flags != 0x1234) { fprintf(stderr, "FAIL: id16 -> anim_flags\n"); return 1; }
    re15_actor_set_member(0, 17, 0x2);
    if (p->status_flags != 0x2)  { fprintf(stderr, "FAIL: id17 -> status_flags\n"); return 1; }
    re15_actor_set_member(0, 18, 4);
    if (p->floor != 4)           { fprintf(stderr, "FAIL: id18 -> floor\n"); return 1; }

    /* GET-Vorzeichen: state +0x04 lbu = zero-extend (200 bleibt 200, nicht -56). */
    p->state = 200;
    if (re15_actor_get_member(0, 8) != 200) { fprintf(stderr, "FAIL: state lbu zero-extend != 200 (%d)\n", re15_actor_get_member(0,8)); return 1; }

    /* id >= 0x14 -> default (0/no-op), byte-true sltiu id,0x14. */
    if (re15_actor_get_member(0, 20) != 0) { fprintf(stderr, "FAIL: id20 sollte 0 liefern\n"); return 1; }

    printf("PASS: test_member_table\n");
    return 0;
}

/* ---- Test 3: Member_cmp (0x3E) via VM — Feld-Quelle + LE-Wert + Operatoren ---- */
static int run_cmp(int16_t hp, int16_t state, uint8_t id, uint8_t op, int16_t val, int expect, const char *tag)
{
    scd_vm_init();
    re15_actor_init();
    g_actors[0].hp     = hp;
    g_actors[0].state  = (uint8_t)state;
    /* Member_cmp 0x3E — 6 B: [op,_,member_id,cmp_op,val_lo,val_hi]; Wert LE. */
    uint8_t bc[7] = { 0x3E, 0x00, id, op, (uint8_t)(val & 0xff), (uint8_t)((val >> 8) & 0xff), SCD_OP_EVT_END };
    scd_thread_start(0, bc);
    g_scd.work_slot = 0;            /* Ziel = Spieler-Actor (Slot 0) */
    scd_vm_tick();
    int got = (int)g_scd.threads[0].locals[0];
    if (got != expect) { fprintf(stderr, "FAIL: cmp %s -> %d, erwartet %d\n", tag, got, expect); return 1; }
    return 0;
}

static int test_member_cmp(void)
{
    int rc = 0;
    /* Feld-Quelle: id19 liest hp; id8 liest state (NICHT motion). */
    rc |= run_cmp(50, 0, 19, 0, 50, 1, "hp==50");          /* == true  */
    rc |= run_cmp(50, 0, 19, 0, 51, 0, "hp==51");          /* == false */
    rc |= run_cmp(50, 7,  8, 0,  7, 1, "state==7");        /* id8=state */
    /* LE-Wert: hp=300 (0x012C). LE liest 300 (==300 true); BE laese 0x2C01 (false). */
    rc |= run_cmp(300, 0, 19, 0, 300, 1, "hp==300 (LE)");
    /* Operatoren 0..6 (cur=hp=50): */
    rc |= run_cmp(50, 0, 19, 1, 40, 1, "50>40");           /* 1: >  */
    rc |= run_cmp(50, 0, 19, 1, 60, 0, "50>60");
    rc |= run_cmp(50, 0, 19, 2, 50, 1, "50>=50");          /* 2: >= */
    rc |= run_cmp(50, 0, 19, 3, 60, 1, "50<60");           /* 3: <  */
    rc |= run_cmp(50, 0, 19, 4, 50, 1, "50<=50");          /* 4: <= */
    rc |= run_cmp(50, 0, 19, 5, 40, 1, "50!=40");          /* 5: != */
    /* 6: & (bitwise test) auf state (lbu). state=0x0F & 0x09 = 9 != 0 -> true; & 0x10 = 0 -> false. */
    rc |= run_cmp(0, 0x0F, 8, 6, 0x09, 1, "0x0F&0x09");
    rc |= run_cmp(0, 0x0F, 8, 6, 0x10, 0, "0x0F&0x10");
    /* cmp_op>=7 -> false (sltiu<0x7 guard). */
    rc |= run_cmp(50, 0, 19, 7, 50, 0, "op7->false");

    if (rc == 0) printf("PASS: test_member_cmp\n");
    return rc;
}

int main(void)
{
    int failures = 0;
    printf("=== Member_set/cmp Unit Tests (#11/#12, FUN_8004116c/FUN_80041358) ===\n\n");

    failures += test_member_table();
    failures += test_member_cmp();

    if (failures == 0) printf("\nALL MEMBER TESTS PASSED\n");
    else               fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    return failures;
}
