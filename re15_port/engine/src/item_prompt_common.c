/*
 * item_prompt_common.c — item-get prompt message replay (U11 font gap).
 *
 * Walks die byte-true Prompt-Skripte + die Item-Namenstabelle des Originals
 * (gen/item_prompt_data.inc, erzeugt von tools/gen_item_prompt_data.py aus
 * shared_assets/PSX/BIN/DEBUG.BIN) und liefert die Glyphen-Indizes der Spielschrift
 * (TEX.TIM) an einen Callback. Gemeinsame Engine-Logik (zaehlen + zeichnen).
 * Siehe re15_item_prompt.h.
 *
 * ⛔ NUTZER-BEFUND 2026-08-27: "beim nehmen des Feuerloeschers room 1000 steht noch
 * 'will you take ?' statt 'will you take fire extinguisher'."
 * Ursache: die vendorte Namenstabelle war ein Savestate-Auszug mit nur 48 Eintraegen.
 * Der Feuerloescher ist Item 0x31 (= 49) — belegt in ROOM1000.RDT @0xC24, Item_aot_set
 * (Opcode 0x50): `50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 | 31 00 | 01 00`, also
 * pc[14] @0xC32 = 0x31 (Typ) / pc[16] @0xC34 = 0x01 (Anzahl). Fuer jede ID >= 48 fiel der
 * `item_id < NIDS`-Zweig unten durch und setzte den Namen STILLSCHWEIGEND nicht ein —
 * genau die Luecke zwischen "the" und "?".
 *
 * Die Tabelle hat im Original 102 Eintraege (IDs 0x00..0x65), nicht 48. Beleg = der
 * Reader selbst, FUN_80028840 @0x80028840:
 *     80028840: andi  a0,a0,0xff          <- KEINE Bereichspruefung, nur Byte-Maske
 *     80028844: sll   a0,a0,1
 *     8002884c: addiu at,at,18780         ; at = 0x800c495c (Offsettabelle)
 *     80028854: lhu   v1,0(at)
 *     8002885c: addiu v0,v0,18984         ; v0 = 0x800c4a28 (Blob)
 *     80028864: addu  v0,v1,v0
 * Die Offsettabelle endet physisch am Blob-Anfang: (0x800c4a28-0x800c495c)/2 = 102.
 * Alle 102 Offsets sind monoton und zeigen auf 0x07-terminierte Namen (der Generator
 * prueft die Monotonie per assert) — 102 ist gemessen, nicht geschaetzt.
 *
 * Ebenfalls vervollstaendigt: das Original haelt ACHT Prompt-Skripte, der Port hatte vier.
 * Selektor FUN_80027e68 @0x80027f20 (zweite identische Fundstelle @0x80028040):
 *     80027f20: sll   v0,a2,1
 *     80027f30: addiu at,at,20422         ; at = 0x800c4fc6 == Offsettabelle UND Basis
 *     80027f34: addu  v0,v0,at
 *     80027f38: lhu   v0,0(v0)
 *     80027f44: addu  v0,v0,at            ; ptr = 0x800c4fc6 + u16[idx]
 * Die acht u16 ab 0x800c4fc6 sind 0x10,0x2f,0x50,0x6b,0x8b,0xa9,0xc5,0xf8; der kleinste
 * (0x10) ist zugleich das Ende der Tabelle selbst = 8 Eintraege.
 */
#include "re15_item_prompt.h"
#include "gen/item_prompt_data.inc"

/* Port-Prompt-Schluessel -> BSS-Skriptindex. Die Zuordnung der vier bisher genutzten
 * Schluessel bleibt unveraendert (1/default->[0] take, 2->[1] can't-carry, 4->[4] use,
 * 5->[2] used); der Port-Schluessel 3 ist historisch belegt, deshalb liegt "place" auf 6.
 *   [0] "Will you take the <NAME>?"      [1] "You can't carry any more items."
 *   [2] "You have used the <NAME>."      [3] "Will you place the <NAME>?"
 *   [4] "Will you use the <NAME>?"       [5] "The <NAME> has been filed."
 *   [6] "You don't need this key any more. Discard it?"
 *   [7] "Locked from the other side."                                                    */
static int prompt_key_to_script(int prompt_type)
{
    switch (prompt_type) {
        case 2:  return 1;   /* can't carry any more items */
        case 4:  return 4;   /* Will you use the X?        */
        case 5:  return 2;   /* You have used the X.       */
        case 6:  return 3;   /* Will you place the X?      */
        case 7:  return 5;   /* The X has been filed.      */
        case 8:  return 6;   /* Discard the spent key?     */
        case 9:  return 7;   /* Locked from the other side */
        default: return 0;   /* Will you take the X?       */
    }
}

int re15_item_prompt_walk(int prompt_type, uint8_t item_id, int max_glyphs,
                          re15_prompt_glyph_cb cb, void *ctx)
{
    int idx = prompt_key_to_script(prompt_type);
    if (idx >= RE15_ITEM_PROMPT_NSCRIPTS) idx = 0;
    /* Offsets sind relativ zur Tabelle selbst, und script_blob[] beginnt AN der Tabelle
     * (@0x800c4fc6) — deshalb indiziert der u16 den Blob direkt (@0x80027f44 addu v0,v0,at). */
    const unsigned char *s = re15_item_prompt_script_blob + re15_item_prompt_script_off[idx];
    int slen = (int)sizeof re15_item_prompt_script_blob
             - (int)re15_item_prompt_script_off[idx];

    int total = 0, attr = 0;
    for (int i = 0; i < slen; i++) {
        unsigned char b = s[i];
        if (b == 0x01 || b == 0x03) break;          /* END / page-break -> Yes/No follows */
        if (b == 0x02) { i += 2; continue; }        /* branch operand (02 a b) */
        if (b == 0x05) { attr = s[++i] & 7; continue; }   /* set color */
        if (b == 0x08) { if (cb && total < max_glyphs) cb(ctx, 0, attr, 1); continue; }  /* newline */
        if (b == 0x06) {                            /* insert the item name (blob until 0x07) */
            i++;                                    /* skip the operand */
            /* Grenze = die physische Ausdehnung der Original-Offsettabelle (102, s. Kopf),
             * nicht ein gewaehltes Limit. Das Original maskiert nur `andi a0,a0,0xff` und
             * wuerde fuer >= 102 in den Blob hinein indizieren; der Port braucht hier eine
             * echte Schranke, weil die Tabellen getrennte C-Arrays sind. */
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

int re15_item_prompt_name_count(void)   { return RE15_ITEM_PROMPT_NIDS; }
int re15_item_prompt_script_count(void) { return RE15_ITEM_PROMPT_NSCRIPTS; }
