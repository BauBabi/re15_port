/* probe_r30_tuerse_bank.c — MESSSONDE Runde 30, Thema "tuer-verschlossen".
 *
 * Frage: Laesst sich die von tools/re2_door_se_cut.py geschnittene Mini-Bank TUERSE.VBS mit
 * GENAU dem Port-Code laden, der heute ELEVSE.VBS laedt (audio_pc.c load_re2_elev_se_pc:
 * re15_vab_parse / re15_edt_decode / re15_edt_resolve_layers_ex / re15_vab_note2pitch2 /
 * re15_vag_adpcm_decode) — und kommen je Satz die erwartete Welle, Tonhoehe und Dauer heraus?
 *
 * Die Sonde spielt NICHTS ab; sie faehrt nur den Lade- und Aufloeseweg und druckt je Satz:
 *   EDT-Rohsatz, Stimme/Prioritaet, Tone-Attribute, VAG-Index, Groesse, Pitch, Rate, Dauer.
 * Erwartung (aus den RE2-Bytes, tools/re2_door_se_cut.py):
 *   Satz 0 ZU_A  7184 B  vol110 center85 shift42 note66
 *   Satz 1 ZU_B  3232 B  vol105 center91 shift 0 note67
 *   Satz 2 ZU_E 10272 B  vol127 center84 shift57 note66
 *
 * Aufruf: probe_r30_tuerse_bank <pfad/TUERSE.VBS> <edt_size> <vbd_size>
 */
#include "re15_vab.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static re15_vab_t s_vab;

int main(int argc, char **argv)
{
    if (argc < 4) { printf("Aufruf: %s <TUERSE.VBS> <edt_size> <vbd_size>\n", argv[0]); return 2; }
    size_t sz = 0;
    uint8_t *vbs = slurp(argv[1], &sz);
    uint32_t edt_size = (uint32_t)strtoul(argv[2], NULL, 0);
    uint32_t vbd_size = (uint32_t)strtoul(argv[3], NULL, 0);
    if (!vbs) { printf("FAIL: %s nicht lesbar\n", argv[1]); return 1; }
    if (sz < (size_t)edt_size + vbd_size || edt_size < 12) { printf("FAIL: Groessen passen nicht (%u B Datei)\n", (unsigned)sz); return 1; }

    const uint8_t *edt = vbs;
    /* VH-Offset = Trailer-u32 @[edt_size-8] (FUN_8005a09c) — wie load_re2_elev_se_pc */
    uint32_t vh_off = (uint32_t)edt[edt_size-8]         | ((uint32_t)edt[edt_size-7] << 8)
                    | ((uint32_t)edt[edt_size-6] << 16) | ((uint32_t)edt[edt_size-5] << 24);
    printf("TUERSE.VBS %u B, edt_size=%u vh_off=0x%X vbd_size=%u\n",
           (unsigned)sz, (unsigned)edt_size, (unsigned)vh_off, (unsigned)vbd_size);
    if (vh_off + 0x20u > edt_size ||
        re15_vab_parse(edt + vh_off, (size_t)edt_size - vh_off, &s_vab) != 0) {
        printf("FAIL: re15_vab_parse\n"); return 1;
    }
    printf("VAB: programme=%d tones=%d vags=%d tones_loaded=%d\n",
           s_vab.program_count, s_vab.tone_count, s_vab.vag_count, s_vab.tones_loaded);

    const uint8_t *vb = vbs + edt_size;
    int fails = 0;
    int nse = (int)(vh_off / 4);
    for (int se = 0; se < nse; se++) {
        re15_edt_rec_t rec;
        if (re15_edt_decode(edt, se, &rec) != 0 || rec.empty) continue;
        int vags[8], tones[8];
        int n = re15_edt_resolve_layers_ex(edt, &s_vab, se, vags, tones, 8);
        printf("Satz %d roh %02x %02x %02x %02x  stimme=%d prio=%d(nib %d) lagen=%d\n",
               se, edt[se*4], edt[se*4+1], edt[se*4+2], edt[se*4+3],
               rec.voice, rec.prio, rec.prio_nib, n);
        if (n <= 0) { printf("   FAIL: keine Lage aufgeloest\n"); fails++; continue; }
        for (int k = 0; k < n; k++) {
            int vag = vags[k];
            const re15_vab_tone_t *t = &s_vab.tones[tones[k]];
            uint32_t off = s_vab.samples[vag].offset, vsz = s_vab.samples[vag].size;
            uint16_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift,
                                                  t->center_note, t->pitch_shift);
            int rate = (int)((44100u * pitch) >> 12);
            size_t cap = (vsz / 16) * 28;
            int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t) + 16);
            int len = (off + vsz <= vbd_size) ? re15_vag_adpcm_decode(vb + off, vsz, pcm, cap) : -1;
            int peak = 0;
            for (int i = 0; i < len; i++) { int v = pcm[i] < 0 ? -pcm[i] : pcm[i]; if (v > peak) peak = v; }
            printf("   lage %d: tone-index %d vag %d @VB+0x%X %u B  vol%u pan%u center%u shift%u note%u  "
                   "pitch 0x%03x = %d Hz  pcm %d Samples = %.3f s  spitze %d\n",
                   k, tones[k], vag, (unsigned)off, (unsigned)vsz, t->vol, t->pan, t->center_note,
                   t->pitch_shift, t->min_note, pitch, rate, len, rate ? (double)len / rate : 0.0, peak);
            if (len <= 0 || peak == 0) { printf("   FAIL: Welle leer/nicht dekodiert\n"); fails++; }
            free(pcm);
        }
    }
    free(vbs);
    printf(fails ? "TUERSE-BANK: %d FAIL\n" : "TUERSE-BANK: alle Saetze geladen (%d Fehler)\n", fails);
    return fails ? 1 : 0;
}
