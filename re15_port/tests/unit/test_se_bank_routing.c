/* test_se_bank_routing.c — the byte-true SCD Se_on VAB-bank selector (FUN_80045024 @0x80045028).
 *
 * The Se_on arg's top byte (arg>>24 = the SCD opcode's bank byte) selects one of 6 VAB banks via
 * the dispatch table @0x80010e70 (`sltiu v0,v1,0x6` bounds it to 0..5). The SE_ON handler used to
 * IGNORE the bank and play (sample_id-1) from a bring-up TEST VAB. This asserts the byte-true bank
 * -> port-bank mapping (re15_audio_se_bank_kind), including the SKIP cases that mirror the original's
 * "bank not resident -> return" (DAT_800b21ec[bank] == -1 @0x8004506c) and its 0..5 bound.
 */
#include <stdio.h>
#include "re15_audio.h"

int main(void)
{
    int fail = 0;
    printf("=== Se_on VAB-bank selector (byte-true FUN_80045024 @0x80045028) ===\n");

    struct { unsigned bank; re15_se_bank_kind_t want; const char *name; } cases[] = {
        { 0, RE15_SE_BANK_SKIP,   "0 resident-blob @0x801fdd00 (not loaded -> skip)" },
        { 1, RE15_SE_BANK_WEAPON, "1 ARMS weapon @0x801fcd00" },
        { 2, RE15_SE_BANK_SND0,   "2 RDT snd0" },
        { 3, RE15_SE_BANK_SND1,   "3 RDT snd1 (FUN_800453d0)" },
        { 4, RE15_SE_BANK_CORE,   "4 CORE @0x801fbd00" },
        { 5, RE15_SE_BANK_SND0,   "5 RDT snd0 (caseD_5 @0x80045130: lw a0,0x8 @0x8004513c = snd0; "
                                  "korrigiert 2026-08-02, Dossier analysis/rolltor_sound.md D6 — "
                                  "die alte SND1-Erwartung war ohne Beleg)" },
    };
    for (int i = 0; i < 6; i++) {
        re15_se_bank_kind_t got = re15_audio_se_bank_kind(cases[i].bank);
        if (got != cases[i].want) {
            fprintf(stderr, "FAIL: bank %u -> %d, want %d (%s)\n", cases[i].bank, got, cases[i].want, cases[i].name);
            fail = 1;
        } else printf("  bank %s\n", cases[i].name);
    }

    /* every invalid bank (>= 6, the `sltiu 0x6` bound) must SKIP — not misroute to a valid bank. */
    int invalid_ok = 1;
    for (unsigned b = 6; b <= 255; b++)
        if (re15_audio_se_bank_kind(b) != RE15_SE_BANK_SKIP) { invalid_ok = 0;
            fprintf(stderr, "FAIL: invalid bank %u must SKIP (got %d)\n", b, re15_audio_se_bank_kind(b)); fail = 1; }
    if (invalid_ok) printf("  banks 6..255 (invalid, sltiu 0x6 bound) -> all SKIP\n");

    if (fail) { printf("SE-BANK-ROUTING: FAIL\n"); return 1; }
    printf("SE-BANK-ROUTING: all checks passed\n");
    return 0;
}
