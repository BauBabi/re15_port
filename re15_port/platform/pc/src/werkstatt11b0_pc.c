/*
 * werkstatt11b0_pc.c — Runde 38 Spur C "werkstatt11b0": PC-Seite des Schiebe-Tons in ROOM11B0
 * (AUFTRAG.md Z.34 "macht das Verschieben des Containers mit Marvin zusammen in der Cutscene noch keinen
 * Verschiebe Sound"). Herleitung mit Adressen: include/re15_werkstatt11b0_r38.h, Dossier
 * analysis/befunde_runde38/C_werkstatt11b0.md §2.
 *
 * Der Schub-Sub 0x13 fordert den Ton im Original-Takt an (szenenleon_r36.c -> re15_audio_room_se_snd0(7) =
 * FUN_80045024(0x02070000) @0x80052ad8/@0x80052bcc). ROOM11B0s Raumbank hat Satz 7 nicht (@0xE4A0 `00 00 00 00`);
 * der Port legt ihn aus der snd0-Bank von ROOM1171 nach — dem einzigen anderen Raum mit dem gemeinsamen Schieben
 * (Plc_dest 0x13 @0x01566/@0x01572) und bytegleich zu ROOM1090, wo Leon die Kiste allein schiebt
 * (Satz `00 00 45 10`, Ton 4, VAG 4 6688 B sha1 f343933e). Die Bank wird beim ersten Ton aus
 * shared_assets/PSX/STAGE1/ROOM1171.RDT geschnitten (Kopfworte +0x08/+0x0C/+0x10 wie rdt_common.c parse_audio,
 * FUN_80045024 @0x8004513c `lw a0,8(v0)`), keine neue Datei. Gespielt wird ueber dieselbe Stimmen-/Prioritaets-
 * Maschine (re15_audio_se_play_bank -> se_play_layers = FUN_80045024-Nachbau): Stimme 0, Prio 5, nicht positional
 * (a0 & 0xff = 0 -> @0x800451dc Ton-vol), wie im Original.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_rdt.h"
#include "re15_room.h"                 /* g_current_room_id, g_room_rdt, g_room_rdt_ok */
#include "re15_engine.h"               /* g_engine.frame_count — nur Messschrieb */
#include "re15_werkstatt11b0_r38.h"
#include "asset_root_pc.h"             /* re15_pc_read_any */

extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);   /* audio_pc.c */

static int        s_state = 0;          /* 0 ungeprueft, 1 geladen, -1 fehlt (ein Versuch) */
static re15_vab_t s_vab;
static uint8_t   *s_edt = NULL;
static int16_t   *s_decoded[RE15_VAB_MAX_SAMPLES];
static int        s_decoded_len[RE15_VAB_MAX_SAMPLES];
static unsigned   s_gespielt = 0;

/* Satz 7 der Quelle, wie er in ROOM1171.RDT @0x6EF8 steht (Prog 0, Ton 4 | Prio 5, Stimme 0x10-0x10 = 0). */
static const uint8_t k_satz7[4] = { 0x00, 0x00, 0x45, 0x10 };

static int laden(void)
{
    if (s_state) return s_state == 1;
    s_state = -1;
    char rel[32];
    snprintf(rel, sizeof rel, "STAGE%u/ROOM%04X.RDT", (RE15_W11B0_QUELLE_RAUM >> 12) & 0xFu, RE15_W11B0_QUELLE_RAUM);
    int n = 0;
    uint8_t *b = re15_pc_read_any(rel, &n);
    if (!b) { fprintf(stderr, "[schub-r38] %s fehlt -> Schiebe-Ton ROOM11B0 stumm\n", rel); return 0; }
    static re15_rdt_t rdt;                                   /* gross: nicht auf den Stapel */
    memset(&rdt, 0, sizeof rdt);
    int ok = (re15_rdt_parse(b, (size_t)n, &rdt) == 0 && rdt.snd_edt[0] && rdt.snd_vh[0] && rdt.snd_vb[0]);
    if (ok) ok = ((uint32_t)(rdt.snd_edt[0] - b) == RE15_W11B0_QUELLE_EDT_OFF &&
                  (uint32_t)(rdt.snd_vh[0]  - b) == RE15_W11B0_QUELLE_VH_OFF  &&
                  (uint32_t)(rdt.snd_vb[0]  - b) == RE15_W11B0_QUELLE_VB_OFF  &&
                  rdt.snd_edt_size[0] >= (RE15_W11B0_SCHUB_SATZ + 1) * 4 &&
                  memcmp(rdt.snd_edt[0] + RE15_W11B0_SCHUB_SATZ * 4, k_satz7, 4) == 0);
    if (ok) ok = (re15_vab_parse(rdt.snd_vh[0], (size_t)rdt.snd_vh_size[0], &s_vab) == 0 &&
                  s_vab.vag_count >= RE15_W11B0_QUELLE_VAG &&
                  (int)s_vab.samples[RE15_W11B0_QUELLE_VAG - 1].size == RE15_W11B0_QUELLE_VAG_LEN);
    if (ok) {
        s_edt = (uint8_t *)malloc((size_t)rdt.snd_edt_size[0]);
        ok = (s_edt != NULL);
        if (ok) memcpy(s_edt, rdt.snd_edt[0], (size_t)rdt.snd_edt_size[0]);
    }
    if (ok) {
        for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
            uint32_t off = s_vab.samples[i].offset, sz = s_vab.samples[i].size;
            if (off + sz > (uint32_t)rdt.snd_vb_size[0]) continue;
            size_t cap = (sz / 16) * 28;
            int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
            if (!pcm) continue;
            s_decoded[i]     = pcm;
            s_decoded_len[i] = re15_vag_adpcm_decode(rdt.snd_vb[0] + off, sz, pcm, cap);
        }
        ok = (s_decoded[RE15_W11B0_QUELLE_VAG - 1] != NULL);
    }
    free(b);
    if (!ok) {
        fprintf(stderr, "[schub-r38] %s: snd0-Bank weicht ab (Satz 7 @0x6EF8 / VAG 4) -> Schiebe-Ton ROOM11B0 stumm\n", rel);
        return 0;
    }
    s_state = 1;
    fprintf(stderr, "[schub-r38] Ergaenzungsbank ROOM%04X snd0 geladen: %d VAGs, Satz 7 @0x6EF8 00 00 45 10 -> "
                    "Ton 4 -> VAG 4 (%d B)\n", RE15_W11B0_QUELLE_RAUM, s_vab.vag_count, RE15_W11B0_QUELLE_VAG_LEN);
    return 1;
}

/* Der Haken (re15_w11b0_snd0_hook): 1 = Satz gespielt, 0 = Raumbank wie bisher. */
static int ergaenzung(int se_id)
{
    if (!g_audio.initialized || !g_room_rdt_ok) return 0;
    if (!re15_w11b0_ergaenzung_noetig(g_current_room_id, g_room_rdt.snd_edt[0], g_room_rdt.snd_edt_size[0], se_id))
        return 0;
    if (!laden()) return 0;
    s_gespielt++;
    fprintf(stderr, "[schub-r38] F%u ROOM%04X snd0 Satz %d leer (@0xE4A0 00 00 00 00) -> ROOM%04X snd0 Satz %d "
                    "(Schiebe-Ton, Nr. %u)\n", (unsigned)g_engine.frame_count, (unsigned)g_current_room_id, se_id,
            RE15_W11B0_QUELLE_RAUM, se_id, s_gespielt);
    re15_audio_se_play_bank(s_edt, &s_vab, s_decoded, s_decoded_len, se_id);
    return 1;
}

unsigned re15_pc_w11b0_gespielt(void) { return s_gespielt; }

void re15_pc_w11b0_init(void)
{
    re15_w11b0_snd0_hook = ergaenzung;
}
