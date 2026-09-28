/* probe_r30_karte-3010_ton.c - MESSSONDE Runde 30, Thema B, dritte Messung.
 *
 * KEIN RIEGEL, NUR MESSUNG: laeuft die aus RE2 geschnittene Mini-Bank HINTSE.VBS
 * (re15_port/tools/re2_hint_cut.py, in der Ermittlungsphase nach build/r30_karte-3010/)
 * durch den ECHTEN VAB-Pfad des Ports - dieselben Funktionen, die se_play_layers in
 * platform/pc/src/audio_pc.c ruft (re15_vab_parse / re15_edt_decode /
 * re15_edt_resolve_layers_ex / re15_vab_note2pitch2 / re15_vag_adpcm_decode)?
 *
 * Erwartung aus RE2 (Dossier analysis/befunde_runde30/karte-3010.md 3.8):
 *   EDT[0x2B] = 00 01 23 00 -> Programm 1 Ton 2; Tone vol 80 pan 64 center 85 shift 0
 *   min 61; Note 61 gegen center 85 = -24 Halbtoene -> Pitch 0x0400 = 11025 Hz;
 *   Welle 4480 B.
 *
 * Aufruf: probe_r30_karte-3010_ton <Pfad zu HINTSE.VBS>
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_vab.h"

#define EDT_SIZE 3800u   /* RE2_HINT_EDT_SIZE  (build/r30_karte-3010/re2_hint_bank.inc) */
#define VBD_OFF  3800u   /* RE2_HINT_VBD_OFF  */
#define VBD_SIZE 4480u   /* RE2_HINT_VBD_SIZE */
#define SE_ID    0x2B    /* lui a0,0x22b @0x8006F234 (RE2) */

static re15_vab_t s_vab;

int main(int argc, char **argv)
{
    if (argc < 2) { printf("Aufruf: %s <HINTSE.VBS>\n", argv[0]); return 2; }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { printf("SKIP: %s fehlt\n", argv[1]); return 77; }
    static uint8_t buf[16384];
    size_t n = fread(buf, 1, sizeof buf, f);
    fclose(f);
    printf("[T0] %s: %lu B (erwartet %u)\n", argv[1], (unsigned long)n, EDT_SIZE + VBD_SIZE);
    if (n != EDT_SIZE + VBD_SIZE) { printf("FAIL: Groesse\n"); return 1; }

    uint32_t vh_off = (uint32_t)buf[EDT_SIZE - 8] | ((uint32_t)buf[EDT_SIZE - 7] << 8) |
                      ((uint32_t)buf[EDT_SIZE - 6] << 16) | ((uint32_t)buf[EDT_SIZE - 5] << 24);
    printf("[T1] Trailer @0x%X: vh_off = 0x%X; Kennung dort: %.4s\n",
           EDT_SIZE - 8, (unsigned)vh_off, (const char *)(buf + vh_off));
    int rc = re15_vab_parse(buf + vh_off, (size_t)EDT_SIZE - vh_off, &s_vab);
    printf("[T2] re15_vab_parse rc=%d  programme=%d vag=%d tones_loaded=%d  Welle 1: off=%lu size=%lu\n",
           rc, s_vab.program_count, s_vab.vag_count, s_vab.tones_loaded,
           (unsigned long)s_vab.samples[0].offset, (unsigned long)s_vab.samples[0].size);
    if (rc != 0) { printf("FAIL: Parse\n"); return 1; }

    re15_edt_rec_t rec;
    rc = re15_edt_decode(buf, SE_ID, &rec);
    printf("[T3] re15_edt_decode(0x%02X) rc=%d: Bytes %02x %02x %02x %02x -> prog=%d tone=%d prio=%d "
           "voice=%d extra=%d empty=%d\n", SE_ID, rc,
           buf[SE_ID * 4], buf[SE_ID * 4 + 1], buf[SE_ID * 4 + 2], buf[SE_ID * 4 + 3],
           rec.prog, rec.tone, rec.prio, rec.voice, rec.extra, rec.empty);

    int vags[8], tones[8];
    int nl = re15_edt_resolve_layers_ex(buf, &s_vab, SE_ID, vags, tones, 8);
    printf("[T4] Lagen: %d\n", nl);
    for (int k = 0; k < nl; k++) {
        const re15_vab_tone_t *t = &s_vab.tones[tones[k]];
        uint16_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift,
                                              t->center_note, t->pitch_shift);
        printf("[T4]   Lage %d: vag=%d tone-index=%d  vol=%u pan=%u center=%u shift=%u min=%u max=%u "
               "adsr=0x%04X/0x%04X  pitch=0x%04X = %lu Hz\n",
               k, vags[k], tones[k], t->vol, t->pan, t->center_note, t->pitch_shift,
               t->min_note, t->max_note, t->adsr1, t->adsr2, pitch,
               (unsigned long)((44100ul * pitch) >> 12));
        if (vags[k] >= 0 && vags[k] < s_vab.vag_count) {
            uint32_t off = s_vab.samples[vags[k]].offset, sz = s_vab.samples[vags[k]].size;
            size_t cap = (sz / 16) * 28;
            int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
            int ns = pcm ? re15_vag_adpcm_decode(buf + VBD_OFF + off, sz, pcm, cap) : -1;
            long spitze = 0;
            for (int i = 0; i < ns; i++) { long a = pcm[i] < 0 ? -pcm[i] : pcm[i]; if (a > spitze) spitze = a; }
            printf("[T4]   Welle: %lu B -> %d Samples, Spitze %ld; Dauer bei diesem Pitch %.3f s\n",
                   (unsigned long)sz, ns, spitze,
                   ns > 0 ? (double)ns / ((44100.0 * pitch) / 4096.0) : 0.0);
            free(pcm);
        }
    }
    /* Gegenprobe: ein unbelegter Satz muss als leer gelten. */
    rc = re15_edt_decode(buf, 0x10, &rec);
    printf("[T5] unbelegter Satz 0x10: rc=%d empty=%d\n", rc, rec.empty);
    return 0;
}
