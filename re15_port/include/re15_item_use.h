/*
 * RE1.5 Rebuilt — the status-screen heal classifier gate + heal-apply table (wave 3,
 * spec shots/inv_wave3_spec.md).
 *
 * WAVE-3 REWRITE: the original ITEM-state-5 heal flow (classifier c3=3 -> FUN_8004adcc)
 * has NO "Will you use the X?" prompt and NO "You have used the X" message — the full
 * disasm @0x8004adcc-0x8004b070 contains no jal 0x80027e68 (message opener) and no pad
 * reads anywhere; the only feedback is the ECG condition wipe (per-item table @0x80010f84)
 * + the sweep reset (@0x8004b038). The Yes/No prompt FSM that used to live behind this
 * header was a port INVENTION for the menu (spec "PORT DIVERGENCE 2") and was deleted;
 * the byte-true c4 sub-FSM (wipe-arm -> wipe-wait -> consume+apply) lives in
 * menu_common.c (state-5 c3=3). The pickup-prompt INFRA (re15_item_prompt.*) is untouched
 * — it is the byte-true item-get MODAL machinery (FUN_8001db28 family), not this flow.
 */
#ifndef RE15_ITEM_USE_H
#define RE15_ITEM_USE_H

#include <stdint.h>

/* Classifier heal gate @0x8004aa64: `sltiu (id-0x22),0xe` (@0x8004ab48; id-0x22 < 14 ->
 * id in [0x22,0x2f] INCLUSIVE) AND `beq a0,0x25` (Red excluded, @0x8004ab54) -> c3=3 HEAL.
 * Everything else non-weapon (ammo 0x15..0x21, Red 0x25, keys/docs >= 0x30) -> c3=6
 * (the "You can't use it here." message, @0x8004ab6c-ab7c). */
int  re15_item_use_is_heal(uint8_t id);

/* Apply the heal for `id` to the player — the per-item applier table @0x80010fbc
 * (13 pointers indexed id-0x22, s0=100 @0x8004ae14); idx >= 0xd (only 0x2f NUT reachable)
 * -> absolute hp=0x4d=77 (@0x8004b028). NO clamp anywhere; poison-clear = status &= ~0x2
 * (@0x8004af8c on 0x800acaec). Called by the heal sub-FSM at c4==2 AFTER the consume
 * (@0x8004aee4-af28 zero + jal FUN_8004dadc, then the applier dispatch @0x8004af34-af58). */
void re15_item_use_apply(uint8_t id);

#endif /* RE15_ITEM_USE_H */
