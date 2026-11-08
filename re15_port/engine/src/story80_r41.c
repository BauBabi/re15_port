/*
 * RE1.5 Rebuilt — Runde 41 Spur ST80-T "Story-Texte umsetzen": Szenen 30C1 / 6030 (Wagen, Fuehrerstand).
 *
 * Herleitung und Belege: include/re15_story80_r41.h, gen/story80_szenen.inc (tools/r41_story80/szenen_bauen.py),
 * Dossier analysis/story80/TEXTE_UMSETZUNG.md. Kein Asset-Patch: die RDTs bleiben byte-true.
 */
#include "re15_story80_r41.h"
#include "re15_scd.h"   /* re15_game_flag_get (Nachbesserung 1: Gates (3,22) / (9,128)) */

#include <stddef.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/story80_szenen.inc"   /* k_s80_* (Programme, Signaturen, Folgen) */

unsigned g_re15_story80_umleitungen[RE15_S80_P_N];

/* Gate je Eintrag (Nachbesserung 1): bis zu zwei Flag-Bedingungen (Bank, Bit, Wert), wie Ck `21 <Bank> <Bit> <Wert>`
 * (Handler 0x8003fcf4; re15_game_flag_get liest dieselbe Tabelle). n = 0: ohne Bedingung. */
typedef struct { uint8_t bank, bit, wert; } s80_ck_t;

typedef struct {
    uint16_t       raum;
    uint8_t        sub;
    const uint8_t *prog;   size_t prog_n;
    const uint8_t *sig;    size_t sig_n;
    const uint8_t *folge;  size_t folge_n;
    const char    *was;
    uint8_t        ck_n;
    s80_ck_t       ck[2];
    int8_t         cut;    /* Integration A2: Eintritts-Cut work_vars[0x0A] (sub00 @0x00D28 Switch), -1 = jeder */
} s80_eintrag_t;

static const s80_eintrag_t k_s80[RE15_S80_P_N] = {
    { RE15_S80_RAUM_30C1, RE15_S80_SUB_30C1, k_s80_30c1_sub03, sizeof k_s80_30c1_sub03,
      k_s80_30c1_sub03_sig, sizeof k_s80_30c1_sub03_sig, k_s80_30c1_sub03_folge, sizeof k_s80_30c1_sub03_folge,
      "kurze Fassung msg 1..5, Marvins Abgang wie Original", 0, { { 0, 0, 0 } }, -1 },
    { RE15_S80_RAUM_6030, RE15_S80_SUB_WAGEN, k_s80_6030_sub02, sizeof k_s80_6030_sub02,
      k_s80_6030_sub02_sig, sizeof k_s80_6030_sub02_sig, k_s80_6030_sub02_folge, sizeof k_s80_6030_sub02_folge,
      "Wagen-Szene 13 Zeilen (msg 0,1,21,2,3,22,4,5,6,7,23,24,8)", 0, { { 0, 0, 0 } }, -1 },
    /* Wagen-Szene gelaufen: sub02 @0x00D7A `22 03 16 01` */
    { RE15_S80_RAUM_6030, RE15_S80_SUB_PULT, k_s80_6030_sub08, sizeof k_s80_6030_sub08,
      k_s80_6030_sub08_sig, sizeof k_s80_6030_sub08_sig, k_s80_6030_sub08_folge, sizeof k_s80_6030_sub08_folge,
      "Fuehrerstand msg 11,12,25,26 vor 'Certainly.'", 1,
      { { RE15_S80_WAGEN_BANK, RE15_S80_WAGEN_BIT, 1 } }, -1 },
    /* Nachbesserung 1: Fuehrerstand vor dem Wagen (3,22)=0 -> ohne Sherry, setzt (9,128) */
    { RE15_S80_RAUM_6030, RE15_S80_SUB_PULT, k_s80_6030_sub08_vorne, sizeof k_s80_6030_sub08_vorne,
      k_s80_6030_sub08_vorne_sig, sizeof k_s80_6030_sub08_vorne_sig, k_s80_6030_sub08_vorne_folge,
      sizeof k_s80_6030_sub08_vorne_folge,
      "Fuehrerstand vor dem Wagen msg 11,31 vor 'Certainly.' (NB1)", 1,
      { { RE15_S80_WAGEN_BANK, RE15_S80_WAGEN_BIT, 0 } }, -1 },
    /* Nachbesserung 1: naechstes Betreten nach dem Wagen, Nachtrag offen -> msg 40, 25; loescht (9,128) */
    { RE15_S80_RAUM_6030, RE15_S80_SUB_PULTRUHE, k_s80_6030_sub09_nachtrag, sizeof k_s80_6030_sub09_nachtrag,
      k_s80_6030_sub09_nachtrag_sig, sizeof k_s80_6030_sub09_nachtrag_sig, k_s80_6030_sub09_nachtrag_folge,
      sizeof k_s80_6030_sub09_nachtrag_folge,
      "Fuehrerstand nach dem Wagen, Nachtrag msg 40,25 (NB1)", 2,
      { { RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG, 1 }, { RE15_S80_WAGEN_BANK, RE15_S80_WAGEN_BIT, 1 } }, -1 },
    /* Integration A2 (Weg F gemessen: vorne zuerst -> Wagen -> ohne Rueckkehr -> Tor): nach dem Torschalter (ROOM6000 sub07
     * @0x011B4 Set(3,89)=1) erreicht sub07 den Nachtrag-Zweig @0x0110C nie mehr (@0x010F4 Ck(3,89)=0), nur noch @0x01128
     * Ck(3,89)=1 -> @0x01132 Evt_exec sub05. Hier laeuft der offene Nachtrag (msg 40, 25) VOR dem Zugstart der Spur AN;
     * Cut 0 = Fuehrerstand (sub00 @0x00D2C Case 0 -> sub07), im Wagen (Cut 6/8/12 -> sub06 @0x010C4) nicht. */
    { RE15_S80_RAUM_6030, RE15_S80_SUB_LICHT, k_s80_6030_sub05_nachtrag_tor, sizeof k_s80_6030_sub05_nachtrag_tor,
      k_s80_6030_sub05_nachtrag_tor_sig, sizeof k_s80_6030_sub05_nachtrag_tor_sig, k_s80_6030_sub05_nachtrag_tor_folge,
      sizeof k_s80_6030_sub05_nachtrag_tor_folge,
      "Fuehrerstand nach dem Tor, Nachtrag msg 40,25 vor dem Zugstart (Integration A2)", 2,
      { { RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG, 1 }, { RE15_S80_TOR_BANK, RE15_S80_TOR_BIT, 1 } }, 0 },
};

static int s80_gate(const s80_eintrag_t *e)
{
    for (int k = 0; k < e->ck_n; k++)
        if (re15_game_flag_get(e->ck[k].bank, e->ck[k].bit) != e->ck[k].wert) return 0;
    if (e->cut >= 0 && (int)g_scd.work_vars[0x0A] != e->cut) return 0;
    return 1;
}

const uint8_t *re15_story80_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *orig)
{
    if (!orig) return orig;
    for (int i = 0; i < RE15_S80_P_N; i++) {
        const s80_eintrag_t *e = &k_s80[i];
        if (e->raum != room_id || e->sub != sub_id || !s80_gate(e)) continue;
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
