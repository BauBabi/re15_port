/*
 * re15_mc_title.c — byte-true BIOS-Memory-Card-Blocktitel (FUN_80026e54).
 *
 * Das Original komponiert den Kartentitel (das, was ein PSX-Kartenmanager/DuckStation
 * anzeigt) als SJIS-Byte-Kette, NICHT als ASCII:
 *   1. memcpy(dst, Template, 0x2a)            @0x80026e98 / ori a2,zero,0x2a @0x80026e9c
 *      Template-Weiche: lbu 0x800B0FBE (Charakter-Bits) @0x80026e58, bit0==0 -> Leon
 *      DAT_800107f8 (@0x80026e94 addiu a1,a1,2040), sonst Elza DAT_800107cc
 *      (@0x80026e80 addiu a1,a1,1996). Inhalt: "BIO HAZARD <Name> /00/" in Vollbreiten-SJIS.
 *   2. Save-Zaehler-Ziffern IN das Template patchen: dst[0x25] += n/10 @0x80026ec8
 *      (sb v1,37(s0)), dst[0x27] += n%10 @0x80026f00 (sb v0,39(s0)); Division via Magic
 *      0xCCCCCCCD + srl 3 @0x80026ea0-f0. Basis = low byte der Vollbreiten-0 (82 4F).
 *   3. Ortsname anhaengen: memcpy(dst+0x2a, 0x80073628 + 0x13*idx, 0x13) @0x80026f0c-2c;
 *      idx kommt vom Resolver FUN_80026e4c — im Auslieferungsstand return-0-Stub
 *      (@0x80026e4c jr ra / addu v0,zero,zero), im Vorprojekt-Patch aus game_state[3]
 *      (SAVE_LOC_FUNC @0x80070890, Pfad B lbu 0x800B0FBF). Der Port folgt dem Patch
 *      (Nutzer-Entscheidung 2026-08-08): idx 0 = Schreibmaschine, 1 = Telefon.
 *
 * Gesamt: 0x2a + 0x13 = 0x3d Bytes SJIS + NUL. Datenbasis vendored aus der EXE
 * (tools/gen_mc_title_bank.py — Offsets dort belegt). End-to-End-Referenz: die echte
 * Mod-Karte re15_save_final_1.mcd traegt exakt diese Kette (recheck-Dossier §3.6).
 */
#include "re15_memcard.h"
#include <string.h>
#include <stdint.h>

#include "gen/mc_title_bank.inc"   /* s_mc_tmpl_leon/elza (0x2a) + s_mc_sjis_loc (7 x 0x13) */

void re15_mc_compose_title(char *out, int character, int count, int loc)
{
    /* SJIS-Tabelle hat EXAKT 7 Eintraege (Eintrag [7] waere Fremddaten "bu00:", recheck §3.2);
     * die Registry liefert nur 0/1 — Clamp ist reine Defensive gegen korrupten Input. */
    if (loc < 0 || loc > 6) loc = 0;
    if (count < 0) count = 0;
    count %= 100;

    memset(out, 0, RE15_MC_TITLE_LEN);
    memcpy(out, character ? s_mc_tmpl_elza : s_mc_tmpl_leon, 0x2a);   /* @0x80026e98/0x80026e74-94 */
    out[0x25] = (char)((uint8_t)out[0x25] + (count / 10));            /* Zehner @0x80026ec8 */
    out[0x27] = (char)((uint8_t)out[0x27] + (count % 10));            /* Einer  @0x80026f00 */
    memcpy(out + 0x2a, s_mc_sjis_loc + 0x13 * loc, 0x13);             /* @0x80026f0c-2c */
    /* out[0x3d..] bleibt 0 (NUL-terminiert; RE15_MC_TITLE_LEN=64 >= 0x3e) */
}
