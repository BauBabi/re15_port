/*
 * RE1.5 Rebuilt — inventory item-USE (heal) flow. Byte-true to the RE1.5 direct context-action model:
 * confirm on a grid item runs the id-classifier FUN_8004aa24 (@0x8004aa64, `sltiu id,0x15`) — a WEAPON
 * (id<0x15) equips, a usable heal item (0x22..0x2e) opens the "Will you use the X?" Yes/No prompt
 * (entry[22] @0x8004b250) and on Yes applies the heal (table @0x80010fbc) + consumes the slot, then
 * shows "You have used the X". There is NO USE/CHECK/COMBINE sub-menu. RE'd workflow wf_13443da6.
 *
 * This is a SUB-STATE of the open inventory menu (gameplay stays paused). It reuses the byte-true
 * prompt-glyph replay (re15_item_prompt_walk, scripts 4=use / 5=used) + the modal's Yes/No input model.
 */
#ifndef RE15_ITEM_USE_H
#define RE15_ITEM_USE_H

#include <stdint.h>

/* Is item `id` a usable HEAL item (0x22 First Aid .. 0x2e G+R+B mix)? = the classifier's default
 * non-weapon USE branch (c3=6). Key items (special table) are out of this slice. */
int  re15_item_use_is_heal(uint8_t id);

/* Start the USE flow for a heal item at inventory `slot` (the cursor slot). Ignored if already active
 * or `id` isn't a heal item. */
void re15_item_use_start(uint8_t id, int slot);

/* Non-zero while the USE prompt/heal is running — the caller (menu) freezes its own input while true. */
int  re15_item_use_active(void);

/* Advance the USE FSM one tick. `pad_edge` = newly-pressed pad bits: & 0x3000 toggles Yes/No,
 * & 0x4000 confirms, & 0xc000 dismisses the "used" message. */
void re15_item_use_tick(uint16_t pad_edge);

/* RENDER query: 0 = no prompt, 4 = "Will you use the <item>." (Yes/No; fills *out_choice 0=Yes/1=No),
 * 5 = "You have used the <item>". *out_id = the item id (for the name). */
int  re15_item_use_prompt(uint8_t *out_id, int *out_choice);

/* Typewriter reveal (glyphs shown) + whether the current prompt has fully revealed. */
int  re15_item_use_reveal(void);
int  re15_item_use_prompt_ready(void);

#endif /* RE15_ITEM_USE_H */
