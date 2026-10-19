/*
 * dining_r39_pc.c — Runde 39 Spur J "dining1141": PC-Seite der Port-Bank 0x3E (engine/src/dining_r39.c,
 * include/re15_dining_r39.h; Dossier analysis/befunde_runde39/J_dining1141.md).
 *
 *   Satz 0x00       Klopfen  = RE2 DOOR0F.DO2 Tonteil Satz 0 (Holzschlag "Treppe Holz"), bytegleich eingebettet
 *                              (gen/dining_r39_programme.inc k_r39j_klopfton; Aufbau wie TORSE: Tonkopf @0, VH @0x10,
 *                              Nachspann @0xC30, VB @0xC38 — Lader wie audio_pc.c re15_audio_re2_tuer_laden, aber eine
 *                              EIGENE Bank, damit die Tuerbank (DOOR1A fuer Johns Auftritt/Abgang) geladen bleibt).
 *   Satz 0x21       Glasbruch = RE2 ROOM1090 snd0 Satz 0x21 (GLAS1090, Runde 35 Spur M; audio_pc.c re15_audio_re2_glas_se).
 *   Satz 0x31..0x35 Hilferuf help01..05 = Sprach-Pfad (audio_pc.c re15_voice_play) mit Id 53..57; den Dateistamm setzt
 *                              re15_dining_r39_stamm (Haken in re15_voice_load_clip) — wie main42 der Runde 38.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_dining_r38.h"
#include "re15_dining_r39.h"
#include "re15_engine.h"
#include "re15_rdt.h"                 /* re15_rdt_parse: RBJ-Block (RDT+0x5C) von ROOM1171 */
#include "asset_root_pc.h"            /* re15_pc_read_any */

extern long re15_audio_cap_tick(void);   /* audio_pc.c (Runde 38 Spur F): Mitschnitt-Takt */
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);   /* audio_pc.c */
extern void re15_audio_re2_glas_se(int satz);                            /* audio_pc.c (Runde 35 Spur M) */
extern void re15_audio_stimme(uint16_t room, int voice_id);              /* audio_pc.c (Runde 38 Spur F) */
extern unsigned g_current_room_id;

#define TONTEIL_EDT 0xC38u   /* bis zum VB (audio_pc.c TORSE_EDT_SIZE, tueren_02_re2.md 4) */

static int        s_state;                  /* 0 ungeprueft, 1 geladen, -1 fehlt */
static const uint8_t *s_edt;              /* Tonkopf = EDT (4 Saetze @0, wie TORSE) */
static re15_vab_t s_vab;
static int16_t   *s_dec[RE15_VAB_MAX_SAMPLES];
static int        s_dec_len[RE15_VAB_MAX_SAMPLES];

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

static int klopfbank_laden(void)
{
    int n = 0;
    const uint8_t *ton = re15_dining_r39_klopfton(&n);
    if (!ton || n <= (int)TONTEIL_EDT) return -1;
    s_edt = ton;
    uint32_t vh_off = rd32(ton + TONTEIL_EDT - 8);              /* Nachspann @0xC30 (@0x80014e48 addiu a3,s0,3120) */
    if (vh_off + 0x20u > TONTEIL_EDT || re15_vab_parse(ton + vh_off, (size_t)TONTEIL_EDT - vh_off, &s_vab) != 0) return -1;
    const uint8_t *vb = ton + TONTEIL_EDT;                       /* VB @0xC38 */
    uint32_t vb_size = (uint32_t)n - TONTEIL_EDT;
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, sz = s_vab.samples[i].size;
        if (sz == 0 || off + sz > vb_size) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_dec[i]     = pcm;
        s_dec_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
    return 0;
}

static unsigned s_gespielt[3];

static void spielen(int satz)
{
    const int dbg = getenv("RE15_SE_DEBUG") != NULL;
    if (satz == RE15_R39J_SATZ_KLOPFEN) {
        if (!s_state) {
            s_state = (klopfbank_laden() == 0) ? 1 : -1;
            fprintf(stderr, "[dining39] Klopfbank RE2 DOOR0F %s (%d VAGs)\n", s_state == 1 ? "geladen" : "FEHLT -> stumm",
                    s_vab.vag_count);
        }
        if (s_state != 1) return;
        s_gespielt[0]++;
        if (dbg) fprintf(stderr, "[se] Se_on bank=0x%02X id=%d -> Runde 39 Spur J (RE2 DOOR0F Klopfen) F%u tick=%ld\n",
                         RE15_R39J_SE_BANK, satz, (unsigned)g_engine.frame_count, re15_audio_cap_tick());
        re15_audio_se_play_bank(s_edt, &s_vab, s_dec, s_dec_len, RE15_R39J_SATZ_KLOPFEN);
        return;
    }
    if (satz == RE15_R39J_SATZ_GLAS) {
        s_gespielt[1]++;
        if (dbg) fprintf(stderr, "[se] Se_on bank=0x%02X id=0x%02X -> Runde 39 Spur J (RE2 GLAS1090 Glasbruch) F%u tick=%ld\n",
                         RE15_R39J_SE_BANK, satz, (unsigned)g_engine.frame_count, re15_audio_cap_tick());
        re15_audio_re2_glas_se(RE15_R39J_SATZ_GLAS);
        return;
    }
    if (satz >= RE15_R39J_SATZ_HILFE0 && satz < RE15_R39J_SATZ_HILFE0 + RE15_R39J_HILFE_N) {
        const int id = RE15_R39J_HILFE_ID0 + (satz - RE15_R39J_SATZ_HILFE0);
        s_gespielt[2]++;
        if (dbg) fprintf(stderr, "[se] Se_on bank=0x%02X id=0x%02X -> Runde 39 Spur J (Stimme help%02d.wav, Id %d) F%u tick=%ld\n",
                         RE15_R39J_SE_BANK, satz, satz - RE15_R39J_SATZ_HILFE0 + 1, id, (unsigned)g_engine.frame_count,
                         re15_audio_cap_tick());
        re15_audio_stimme((uint16_t)g_current_room_id, id);
    }
}

unsigned re15_pc_dining_r39_gespielt(int k) { return (k >= 0 && k < 3) ? s_gespielt[k] : 0u; }

/* M2/N4.4 + Nutzer-Nachtrag 7: Johns Heranwinken liegt im RBJ-Block von ROOM1171 (RDT+0x5C, Lader FUN_8001b3f8 @0x8001b404).
 * Datei einmal lesen und halten (Muster platform/pc/src/cut10f0_pc.c re15_cut10f0_pc_rbj_leihen). */
static uint8_t   *s_rbj_buf = NULL;
static re15_rdt_t s_rbj_rdt;
static const uint8_t *rbj_1171(int *size)
{
    if (!s_rbj_buf) {
        char rel[48];
        int n = 0;
        snprintf(rel, sizeof rel, "STAGE%u/ROOM%04X.RDT", (RE15_R39J_WINK_RAUM >> 12) & 0xFu, RE15_R39J_WINK_RAUM);
        uint8_t *b = re15_pc_read_any(rel, &n);
        if (!b) return NULL;
        if (re15_rdt_parse(b, (size_t)n, &s_rbj_rdt) != 0 || !s_rbj_rdt.animation || s_rbj_rdt.animation_size <= 0) {
            free(b);
            return NULL;
        }
        s_rbj_buf = b;
        fprintf(stderr, "[rbj] ROOM1141: Animationsblock von ROOM%04X fuer Johns Heranwinken gelesen (%d B, Runde 39 Spur J)\n",
                RE15_R39J_WINK_RAUM, s_rbj_rdt.animation_size);
    }
    if (size) *size = s_rbj_rdt.animation_size;
    return s_rbj_rdt.animation;
}

void re15_pc_dining_r39_init(void)
{
    re15_dining_r39_se_hook = spielen;
    re15_dining_r39_rbj_hook = rbj_1171;
}
