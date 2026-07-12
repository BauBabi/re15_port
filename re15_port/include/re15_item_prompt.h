/*
 * RE1.5 Rebuilt — item-get prompt message replay (U11 font gap).
 *
 * Byte-true replay of the item-prompt message VM (FUN_80028134) for the pickup modal: the BSS prompt
 * scripts @0x800c4fc6 + the per-item name blob @0x800c4a28 (savestate-extracted, gen/item_prompt_data
 * .inc). Rather than re-encode ASCII, the port replays the game's OWN glyph bytes through the TEX.TIM
 * message font — so "Will you take the <item>." / "You can't carry any more items" render in the exact
 * game font, byte-for-byte. The walk yields each glyph (in stream order) to a callback so the ENGINE
 * owns the script logic once (count for the typewriter total; the PC backend supplies the draw).
 *
 * Control opcodes in the script: 0x05 <n> = set color/attr; 0x06 <n> = insert the item name (blob at
 * off[id], glyphs until 0x07); 0x08 = newline; 0x03 = page-break (Yes/No follows — stop); 0x02 <a><b>
 * = branch (stop); 0x01 <n> = END (stop). Every other byte is a literal glyph (0x00 = space).
 */
#ifndef RE15_ITEM_PROMPT_H
#define RE15_ITEM_PROMPT_H

#include <stdint.h>

/* Per-glyph callback: `code` = the TEX.TIM font glyph index (0x00 = space), `attr` = color 0..7,
 * `newline` = 1 when the pen should drop a line (code/attr ignored). */
typedef void (*re15_prompt_glyph_cb)(void *ctx, unsigned char code, int attr, int newline);

/* Walk the prompt script for `prompt_type` (1 = "Will you take the <item>." Yes/No, 2 = "You can't
 * carry any more items"), inserting item `item_id`'s name at the 0x06 opcode. Fires `cb` for each
 * glyph/newline while the running glyph index < `max_glyphs` (pass 0 + cb NULL to only COUNT). Returns
 * the TOTAL glyph count (for the typewriter reveal target). */
int re15_item_prompt_walk(int prompt_type, uint8_t item_id, int max_glyphs,
                          re15_prompt_glyph_cb cb, void *ctx);

#endif /* RE15_ITEM_PROMPT_H */
