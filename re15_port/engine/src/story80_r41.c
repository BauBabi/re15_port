/*
 * RE1.5 Rebuilt — Runde 41 Spur ST80-T "Story-Texte umsetzen": Szenen 30C1 / 6030 (Wagen, Fuehrerstand).
 *
 * Herleitung und Belege: include/re15_story80_r41.h, gen/story80_szenen.inc (tools/r41_story80/szenen_bauen.py),
 * Dossier analysis/story80/TEXTE_UMSETZUNG.md. Kein Asset-Patch: die RDTs bleiben byte-true.
 */
#include "re15_story80_r41.h"

#include <stddef.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/story80_szenen.inc"   /* k_s80_* (Programme, Signaturen, Folgen) */

unsigned g_re15_story80_umleitungen[RE15_S80_P_N];

typedef struct {
    uint16_t       raum;
    uint8_t        sub;
    const uint8_t *prog;   size_t prog_n;
    const uint8_t *sig;    size_t sig_n;
    const uint8_t *folge;  size_t folge_n;
    const char    *was;
} s80_eintrag_t;

static const s80_eintrag_t k_s80[RE15_S80_P_N] = {
    { RE15_S80_RAUM_30C1, RE15_S80_SUB_30C1, k_s80_30c1_sub03, sizeof k_s80_30c1_sub03,
      k_s80_30c1_sub03_sig, sizeof k_s80_30c1_sub03_sig, k_s80_30c1_sub03_folge, sizeof k_s80_30c1_sub03_folge,
      "kurze Fassung msg 1..5, Marvins Abgang wie Original" },
    { RE15_S80_RAUM_6030, RE15_S80_SUB_WAGEN, k_s80_6030_sub02, sizeof k_s80_6030_sub02,
      k_s80_6030_sub02_sig, sizeof k_s80_6030_sub02_sig, k_s80_6030_sub02_folge, sizeof k_s80_6030_sub02_folge,
      "Wagen-Szene 13 Zeilen (msg 0,1,21,2,3,22,4,5,6,7,23,24,8)" },
    { RE15_S80_RAUM_6030, RE15_S80_SUB_PULT, k_s80_6030_sub08, sizeof k_s80_6030_sub08,
      k_s80_6030_sub08_sig, sizeof k_s80_6030_sub08_sig, k_s80_6030_sub08_folge, sizeof k_s80_6030_sub08_folge,
      "Fuehrerstand msg 11,12,25,26 vor 'Certainly.'" },
};

const uint8_t *re15_story80_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *orig)
{
    if (!orig) return orig;
    for (int i = 0; i < RE15_S80_P_N; i++) {
        const s80_eintrag_t *e = &k_s80[i];
        if (e->raum != room_id || e->sub != sub_id) continue;
        if (memcmp(orig, e->sig, e->sig_n) != 0) {
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[story80] ROOM%04X sub%02u: Signatur weicht ab - Original-Sub\n", (unsigned)room_id,
                    (unsigned)sub_id);
#endif
            return orig;
        }
        g_re15_story80_umleitungen[i]++;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[story80] ROOM%04X Evt_exec sub%02u -> Programm (%u B): %s\n", (unsigned)room_id,
                (unsigned)sub_id, (unsigned)e->prog_n, e->was);
#endif
        return e->prog;
    }
    return orig;
}

const uint8_t *re15_story80_programm(int p, int *out_len)
{
    if (p < 0 || p >= RE15_S80_P_N) { if (out_len) *out_len = 0; return NULL; }
    if (out_len) *out_len = (int)k_s80[p].prog_n;
    return k_s80[p].prog;
}

const uint8_t *re15_story80_folge(int p, int *out_n)
{
    if (p < 0 || p >= RE15_S80_P_N) { if (out_n) *out_n = 0; return NULL; }
    if (out_n) *out_n = (int)k_s80[p].folge_n;
    return k_s80[p].folge;
}
