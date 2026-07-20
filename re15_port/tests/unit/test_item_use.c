/* test_item_use.c — the status-screen heal classifier gate + applier table (wave 3,
 * spec shots/inv_wave3_spec.md).
 *
 * WAVE-3 REWRITE — WHY the old prompt-flow assertions were removed: the previous test
 * asserted a 5-state "Will you use the X?" Yes/No + "You have used the X" FSM that was a
 * port INVENTION (spec "PORT DIVERGENCE 2"). The original heal flow (classifier c3=3 ->
 * FUN_8004adcc) has NO prompt, NO message and NO pad reads: the full disasm
 * @0x8004adcc-0x8004b070 contains no jal 0x80027e68 (message opener) and its only wait
 * is the 25d4 wipe poll. The byte-true c4 sub-FSM (wipe-arm -> wait -> consume+apply)
 * is exercised end-to-end in test_inv_fsm.c; THIS test pins the applier table
 * @0x80010fbc entry-by-entry (incl. the Green +25 fix @0x8004af78) and the classifier
 * gate @0x8004ab48-ab60.
 */
#include <stdio.h>
#include <stdint.h>
#include "re15_item_use.h"
#include "re15_actor.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static re15_actor_t *PL(void) { return &g_actors[RE15_ACTOR_SLOT_PLAYER]; }

/* apply `id` with the player primed to hp/poisoned, return the resulting hp */
static int apply_at(uint8_t id, int hp, int poisoned)
{
    PL()->hp = (int16_t)hp;
    PL()->status_flags = poisoned ? 0x2 : 0;
    re15_item_use_apply(id);
    return PL()->hp;
}

int main(void)
{
    printf("=== heal classifier gate + applier table @0x80010fbc (wave 3) ===\n");

    /* ---- classifier gate @0x8004aa64: sltiu (id-0x22),0xe && id!=0x25 ---- */
    CHECK("0x21 (ammo bound) is NOT heal-usable",  !re15_item_use_is_heal(0x21));
    CHECK("0x22 First Aid IS heal-usable",          re15_item_use_is_heal(0x22));
    CHECK("0x24 Green IS heal-usable",              re15_item_use_is_heal(0x24));
    CHECK("0x25 Red is EXCLUDED (beq a0,0x25 @0x8004ab54)", !re15_item_use_is_heal(0x25));
    CHECK("0x26 Blue IS heal-usable",               re15_item_use_is_heal(0x26));
    CHECK("0x2f NUT IS admitted (idx 13 special)",  re15_item_use_is_heal(0x2f));
    CHECK("0x30 (first key/doc) is NOT",            !re15_item_use_is_heal(0x30));

    /* ---- applier table @0x80010fbc (s0=100 @0x8004ae14) ---- */
    CHECK("0x22 First Aid: hp := 100 absolute (sh s0 @0x8004afe4)", apply_at(0x22, 10, 0) == 100);
    CHECK("0x22 First Aid does NOT cure poison",
          (apply_at(0x22, 10, 1) == 100) && (PL()->status_flags & 0x2) != 0);
    CHECK("0x23 Antidote: hp += 25 AND cure (@0x8004af60 -> srl s0,2 + &=0xfffd)",
          apply_at(0x23, 50, 1) == 75 && (PL()->status_flags & 0x2) == 0);
    /* THE WAVE-3 FIX: Green Medicine 0x24 = +25, NOT the invented +50. Raw bytes:
     * entry[2] -> 0x8004af68 with delay-slot `srl v0,s0,2` @0x8004af78 (0x00101082,
     * shamt 2 = 100>>2 = 25); the +50 entries use `srl s0,1` (@0x8004afa4). */
    CHECK("0x24 Green: hp += 25 (srl v0,s0,2 @0x8004af78 — NOT +50)", apply_at(0x24, 40, 0) == 65);
    CHECK("0x24 Green does not cure", (apply_at(0x24, 40, 1) == 65) && (PL()->status_flags & 0x2) != 0);
    CHECK("0x26 Blue: cure ONLY, hp unchanged (@0x8004af7c)",
          apply_at(0x26, 42, 1) == 42 && (PL()->status_flags & 0x2) == 0);
    CHECK("0x27 G+R: hp += 50 (srl s0,1 @0x8004afa4)", apply_at(0x27, 40, 0) == 90);
    CHECK("0x28 G+G: hp += 50",                        apply_at(0x28, 40, 0) == 90);
    CHECK("0x29 G+B: hp += 25 + cure",
          apply_at(0x29, 40, 1) == 65 && (PL()->status_flags & 0x2) == 0);
    CHECK("0x2a G+G+R: hp := 100 absolute",            apply_at(0x2a, 3, 0) == 100);
    CHECK("0x2b G+G+G: hp += 75 (100>>1 + 100>>2 @0x8004afbc)", apply_at(0x2b, 20, 0) == 95);
    CHECK("0x2c G+G+B: hp += 50 + cure",
          apply_at(0x2c, 40, 1) == 90 && (PL()->status_flags & 0x2) == 0);
    CHECK("0x2d G+R+R: hp := 100 absolute",            apply_at(0x2d, 3, 0) == 100);
    CHECK("0x2e G+R+B: hp += 50 + cure",
          apply_at(0x2e, 40, 1) == 90 && (PL()->status_flags & 0x2) == 0);
    CHECK("0x2f NUT: hp := 77 absolute (idx>=0xd @0x8004b028)", apply_at(0x2f, 3, 0) == 77);

    /* ---- byte-true NO-clamp quirk (@0x8004afa8-b0: raw addu+sh) ---- */
    CHECK("no ceiling clamp: hp 90 + Green = 115",     apply_at(0x24, 90, 0) == 115);
    CHECK("no ceiling clamp: hp 100 + G+R = 150",      apply_at(0x27, 100, 0) == 150);

    if (g_fail) { printf("ITEM-USE: FAIL\n"); return 1; }
    printf("ITEM-USE: all checks passed (applier table @0x80010fbc byte-true incl. "
           "Green +25 @0x8004af78)\n");
    return 0;
}
