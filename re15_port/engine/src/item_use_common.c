/*
 * item_use_common.c — the status-screen heal-apply table (wave 3, spec
 * shots/inv_wave3_spec.md; RE1.5-EXE throughout).
 *
 * WAVE-3 REWRITE: the 5-state "Will you use the X?" Yes/No + "You have used the X" FSM
 * that used to live here was a port INVENTION for the menu (spec "PORT DIVERGENCE 2") —
 * the original heal flow (classifier c3=3 -> FUN_8004adcc @0x8004adcc-0x8004b070) has NO
 * prompt, NO message and NO pad reads (no jal 0x80027e68 anywhere in the flow; the only
 * wait is the 25d4 wipe poll). The byte-true c4 sub-FSM (wipe-arm -> wipe-wait ->
 * consume+apply) now lives in menu_common.c; this module keeps the two byte-true pieces
 * shared by it: the classifier heal gate and the @0x80010fbc applier table.
 * (The old header comment mis-cited @0x8004b250 as a Yes/No prompt — @0x8004b250 is the
 * c3=6 "You can't use it here." plain message with DAT_800b8522=0 = NO Yes/No.)
 */
#include "re15_item_use.h"
#include "re15_actor.h"         /* g_actors[PLAYER].hp / status_flags */
#include "re15_scd.h"           /* re15_vest_hp_bonus — Weste-Nachruestung */

/* Heal applier table @0x80010fbc (13 words), ids 0x22..0x2e, s0=100 (@0x8004ae14).
 * set=1 -> HP = 100 (absolute `sh s0`); else HP += add (raw, NO ceiling clamp
 * @0x8004afb0/afe0/afac/b014). cure=1 -> status_flags &= ~0x2 (DAT_800acaec &= 0xfffd
 * @0x8004af8c). Byte-true quirks: additive items are NOT clamped to 100; First Aid (0x22)
 * heals full but does NOT cure poison; the consume runs even at full HP. */
typedef struct { uint8_t set; int16_t add; uint8_t cure; } re15_heal_t;
static const re15_heal_t s_heal[13] = {
    /*0x22 First Aid Spray*/ {1,  0, 0}, /*0x23 Antidote Spray*/ {0, 25, 1},
    /*0x24 Green Medicine  */ {0, 25, 0}, /* +25, NOT +50: table entry [2] -> 0x8004af68 with
                                           * delay-slot `srl v0,s0,2` @0x8004af78 (0x00101082,
                                           * shamt 2) = 100>>2 = 25, then addu+sh @0x8004afa8-b0.
                                           * Contrast the +50 handler @0x8004af98: `srl s0,1`
                                           * @0x8004afa4. (Wave-3 fix of the port's invented +50;
                                           * spec "PORT DIVERGENCE 1".) */
    /*0x25 Red Medicine    */ {0,  0, 0}, /* dead — the classifier blocks Red (@0x8004ab54);
                                           * applier [3] -> 0x8004b034 = the plain exit */
    /*0x26 Blue Medicine   */ {0,  0, 1}, /*0x27 G+R Mix       */ {0, 50, 0},
    /*0x28 G+G Mix         */ {0, 50, 0}, /*0x29 G+B Mix       */ {0, 25, 1},
    /*0x2a G+G+R Mix       */ {1,  0, 0}, /*0x2b G+G+G Mix     */ {0, 75, 0}, /* 100>>1 + 100>>2
                                           * @0x8004afbc */
    /*0x2c G+G+B Mix       */ {0, 50, 1}, /*0x2d G+R+R Mix     */ {1,  0, 0},
    /*0x2e G+R+B Mix       */ {0, 50, 1},
};

/* Heal-usable = id in [0x22,0x2f] EXCEPT Red Medicine (0x25). Byte-true classifier
 * @0x8004aa64: `sltiu (id-0x22),0xe` (@0x8004ab48; 0xe=14 -> id in [0x22,0x2f] INCLUSIVE)
 * AND `beq a0,0x25` (Red excluded @0x8004ab54) -> c3=3. 0x2f ("NUT") IS admitted and has
 * its own absolute HP=77 handler (idx>=0xd @0x8004ae98/@0x8004b028). */
int re15_item_use_is_heal(uint8_t id) { return id >= 0x22 && id <= 0x2f && id != 0x25; }

void re15_item_use_apply(uint8_t id)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if ((uint8_t)(id - 0x22) >= 0xd) {                 /* sltiu 0xd @0x8004af40: only 0x2f
                                                        * reachable via the classifier */
        pl->hp = 77;                                   /* hp := 0x4d @0x8004b028 (the heal
                                                        * flow already wrote 77 at c4==0
                                                        * @0x8004ae98 — harmless double
                                                        * write, modeled by re-applying) */
        return;
    }
    {
        const re15_heal_t *h = &s_heal[id - 0x22];     /* jr [0x80010fbc+idx*4] @0x8004af50 */
        if (h->set) pl->hp = (int16_t)(100 + re15_vest_hp_bonus());
                                                       /* sh s0 (=0x64) — absolute full heal;
                                                        * +5 Weste-Nachruestung (sonst wuerde
                                                        * jede Vollheilung den Bonus loeschen —
                                                        * Belege scd_room_setup.c) */
        else        pl->hp = (int16_t)(pl->hp + h->add); /* raw add, NO clamp (@0x8004afa8-b0) */
        if (h->cure) pl->status_flags &= (uint16_t)~0x2u; /* clear poison @0x8004af8c */
    }
}
