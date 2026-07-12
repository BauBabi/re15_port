/*
 * item_use_common.c — inventory item-USE (heal) FSM. Byte-true to the RE1.5 direct context-action
 * model (RE'd wf_13443da6): confirm on a usable heal item (0x22..0x2e) opens the "Will you use the X?"
 * Yes/No prompt (entry[22] @0x8004b250), on Yes applies the heal (table @0x80010fbc) + consumes the
 * slot (@0x8004aef0-af28), then shows "You have used the X" (script[2] via c2=6 @0x8004b038). See
 * re15_item_use.h. Reuses the byte-true prompt-glyph replay + the modal's Yes/No input model.
 */
#include "re15_item_use.h"
#include "re15_item_prompt.h"   /* re15_item_prompt_walk — glyph count (typewriter total) */
#include "re15_inventory.h"     /* g_inv + re15_inv_remove_slot */
#include "re15_actor.h"         /* g_actors[PLAYER].hp / status_flags */

/* Heal jump table @0x80010fbc (13 words), ids 0x22..0x2e. set=1 -> HP = 100 (absolute `sh s0`,
 * s0=0x64 @0x8004ae14); else HP += add (raw, NO ceiling clamp @0x8004afb0/afe0/afac/b014). cure=1 ->
 * status_flags &= ~0x2 (DAT_800acaec &= 0xfffd @0x8004af8c). Byte-true quirks: additive items are NOT
 * clamped to 100; First Aid (0x22) heals full but does NOT cure poison; USE consumes even at full HP. */
typedef struct { uint8_t set; int16_t add; uint8_t cure; } re15_heal_t;
static const re15_heal_t s_heal[13] = {
    /*0x22 First Aid Spray*/ {1,  0, 0}, /*0x23 Antidote Spray*/ {0, 25, 1}, /*0x24 Green Medicine*/ {0, 50, 0},
    /*0x25 Red Medicine   */ {0,  0, 0}, /*0x26 Blue Medicine  */ {0,  0, 1}, /*0x27 G+R Mix       */ {0, 50, 0},
    /*0x28 G+G Mix         */ {0, 50, 0}, /*0x29 G+B Mix        */ {0, 25, 1}, /*0x2a G+G+R Mix     */ {1,  0, 0},
    /*0x2b G+G+G Mix       */ {0, 75, 0}, /*0x2c G+G+B Mix      */ {0, 50, 1}, /*0x2d G+R+R Mix     */ {1,  0, 0},
    /*0x2e G+R+B Mix       */ {0, 50, 1},
};

/* FSM state (mirrors entry[22]@0x8004b250 DAT_800b25c4 + FUN_8004adcc): 0=idle, 1=open use-prompt,
 * 2=use-prompt wait, 3=apply+open used-message, 4=used-message wait. */
static uint8_t s_state = 0;
static uint8_t s_id    = 0;
static int     s_slot  = -1;
static int     s_choice = 0;     /* 0=Yes, 1=No (DAT_800b8520 bit0) */
static int     s_reveal = 0, s_reveal_total = 0, s_reveal_timer = 0;

/* Heal-usable = id in [0x22,0x2f] EXCEPT Red Medicine (0x25). Byte-true classifier @0x8004aa64:
 * `sltiu (id-0x22),0xe` (@0x8004ab48; 0xe=14 -> id-0x22 < 14 -> id in [0x22,0x2f] INCLUSIVE) AND
 * `beq a0,0x25` (Red excluded) -> heal-execute state 3; Red alone falls to the inert state 6
 * (@0x8004ab6c). 0x2f ("NUT") IS admitted and has its own absolute HP=77 handler in apply (see below) —
 * the port previously capped at 0x2e (off-by-one) so 0x2f was inert. (RE1.5 has NO herb-combine to make
 * Red useful; the mixes 0x27-0x2e are pre-placed data items.) */
int re15_item_use_is_heal(uint8_t id) { return id >= 0x22 && id <= 0x2f && id != 0x25; }
int re15_item_use_active(void)        { return s_state != 0; }
int re15_item_use_reveal(void)        { return s_reveal; }
/* Ready only in the WAIT states (2 = "use?", 4 = "used") — NOT the transient state 1/3 (before
 * s_reveal_total is computed, else it would read ready-at-zero and skip the typewriter). */
int re15_item_use_prompt_ready(void)  { return (s_state == 2 || s_state == 4) && s_reveal >= s_reveal_total; }

static void apply_heal(uint8_t id)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (id == 0x2f) { pl->hp = 77; return; }          /* 0x2f "NUT": absolute HP=0x4d=77, the substate-0
                                                       * FALL-THROUGH default @0x8004ae98 (`ori 0x4d; sh
                                                       * player.hp`) — index 13 is the NULL substate slot,
                                                       * so it must NOT index the 13-entry s_heal add-table. */
    const re15_heal_t *h = &s_heal[id - 0x22];        /* addiu s1,v0,-0x22 @0x8004ae08 */
    if (h->set) pl->hp = 100;                          /* sh s0 (=0x64) — absolute full heal */
    else        pl->hp = (int16_t)(pl->hp + h->add);   /* raw add, NO clamp (Green @90 -> 140) */
    if (h->cure) pl->status_flags &= (uint16_t)~0x2u;  /* clear poison @0x8004af8c */
}

void re15_item_use_start(uint8_t id, int slot)
{
    if (s_state != 0 || !re15_item_use_is_heal(id)) return;
    s_id    = id;
    s_slot  = slot;
    s_choice = 0;
    s_state = 1;
}

void re15_item_use_tick(uint16_t pad_edge)
{
    int again = 1;
    while (again) {
        again = 0;
        switch (s_state) {
        case 0:
            return;

        case 1:  /* open "Will you use the X?" (script 4) */
            s_reveal       = 0;
            s_reveal_timer = 2;                                        /* 2 frames/glyph (DAT_800b8524) */
            s_reveal_total = re15_item_prompt_walk(4, s_id, 0, 0, 0);  /* byte-true glyph count */
            s_state = 2;
            return;

        case 2:  /* use-prompt wait (typewriter -> Yes/No). Yes -> apply, No -> done. */
            if (s_reveal < s_reveal_total) {                            /* typewriter, no input yet */
                if (--s_reveal_timer <= 0) { s_reveal++; s_reveal_timer = 2; }
                return;
            }
            if (pad_edge & 0x3000) s_choice ^= 1;                       /* toggle Yes/No */
            if (!(pad_edge & 0x4000)) return;                          /* wait CROSS confirm */
            if (s_choice) { s_state = 0; return; }                     /* No -> abort, item stays */
            s_state = 3;                                                /* Yes */
            again   = 1;
            break;

        case 3:  /* APPLY heal + consume the slot (byte-true @0x8004aee4-af2c), open "used" (script 5). */
            apply_heal(s_id);
            re15_inv_remove_slot(s_slot);                              /* zero id/qty/flags @0x8004aef0 */
            s_reveal       = 0;
            s_reveal_timer = 2;
            s_reveal_total = re15_item_prompt_walk(5, s_id, 0, 0, 0);
            s_state = 4;
            return;

        case 4:  /* "You have used the X" wait -> dismiss on any confirm, back to the grid. */
            if (s_reveal < s_reveal_total) {
                if (--s_reveal_timer <= 0) { s_reveal++; s_reveal_timer = 2; }
                return;
            }
            if (!(pad_edge & 0xc000)) return;                          /* wait CROSS/SQUARE dismiss */
            s_state = 0;
            return;

        default:
            s_state = 0;
            return;
        }
    }
}

int re15_item_use_prompt(uint8_t *out_id, int *out_choice)
{
    if (s_state == 0) return 0;
    if (out_id)     *out_id     = s_id;
    if (out_choice) *out_choice = s_choice;
    if (s_state == 1 || s_state == 2) return 4;   /* "Will you use the X?" */
    return 5;                                     /* "You have used the X" */
}
