/*
 * item_prompt_common.c — item-get prompt message replay (U11 font gap).
 *
 * Walks the byte-true BSS prompt scripts (gen/item_prompt_data.inc, savestate-extracted from
 * stage_saves/mzd_stage1_briefing.sav) + the per-item name blob, yielding the game's own TEX.TIM font
 * glyph indices to a callback. Shared engine logic (count + draw). See re15_item_prompt.h.
 */
#include "re15_item_prompt.h"
#include "gen/item_prompt_data.inc"

int re15_item_prompt_walk(int prompt_type, uint8_t item_id, int max_glyphs,
                          re15_prompt_glyph_cb cb, void *ctx)
{
    const unsigned char *s;
    int slen;
    if (prompt_type == 2) { s = re15_item_prompt_script_full; slen = (int)sizeof re15_item_prompt_script_full; }
    else                  { s = re15_item_prompt_script_take; slen = (int)sizeof re15_item_prompt_script_take; }

    int total = 0, attr = 0;
    for (int i = 0; i < slen; i++) {
        unsigned char b = s[i];
        if (b == 0x01 || b == 0x03) break;          /* END / page-break -> Yes/No follows */
        if (b == 0x02) { i += 2; continue; }        /* branch operand (02 a b) */
        if (b == 0x05) { attr = s[++i] & 7; continue; }   /* set color */
        if (b == 0x08) { if (cb && total < max_glyphs) cb(ctx, 0, attr, 1); continue; }  /* newline */
        if (b == 0x06) {                            /* insert the item name (blob until 0x07) */
            i++;                                    /* skip the operand */
            if ((int)item_id < RE15_ITEM_PROMPT_NIDS) {
                const unsigned char *nm = re15_item_prompt_name_blob + re15_item_prompt_name_off[item_id];
                for (; *nm != 0x07; nm++) {
                    if (cb && total < max_glyphs) cb(ctx, *nm, attr, 0);
                    total++;
                }
            }
            continue;
        }
        if (cb && total < max_glyphs) cb(ctx, b, attr, 0);   /* literal glyph (0x00 = space) */
        total++;
    }
    return total;
}
