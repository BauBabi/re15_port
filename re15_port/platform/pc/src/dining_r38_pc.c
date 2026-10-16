/*
 * dining_r38_pc.c — Runde 38 Spur F "dining1141": PC-Seite der drei Port-Baenke (engine/src/dining_r38.c,
 * include/re15_dining_r38.h; Dossier analysis/befunde_runde38/F_dining1141.md).
 *
 *   0x3B  Rolladen = die snd0-Bank von ROOM1131.RDT (RDT-Kopf +0x08 EDT / +0x0C VH / +0x10 VB, FUN_80045024 Bank 2
 *         -> `lw a0,0x8` @0x80045138 = RDT-snd0), gelesen aus dem ausgelieferten CD-Baum (kein Asset-Patch).
 *         Saetze 0x0A/0x0B/0x0C = VAG 4/5/8 (rolltor_sound.md 4.4, Pitch -41/-40/-36 Halbtoene ueber note2pitch2).
 *   0x3C  Klopfen  = RE2 ROOM1060 snd0, bytegleich als shared_assets/RE2/KLOPF1060.EDT/.VH/.VB
 *         (tools/r38_dining/re2_export.py), Saetze 0x0A/0x0B.
 *   0x3D  Hilferuf = synchro/STAGE1/room1141/john_help.wav ("Help me out.", Nutzer-Stimme Johns; der Nutzer darf die
 *         Datei ersetzen). WAV -> Mono 44100 Hz -> eine synthetische Ein-Ton-Bank (Prog 0 Ton 0, center = note =
 *         60 -> note2pitch2 = 0x1000 = 44100 Hz, Satz 0 `00 00 01 01` = Stimme < 0 = Direkt-Zweig wie FUN_80045024
 *         `(byte3 & 0x1f) < 0x10`).
 * Gespielt ueber dieselbe Stimmen-Maschine wie jede SE (re15_audio_se_play_bank -> se_play_layers).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_dining_r38.h"
#include "re15_engine.h"
#include "asset_root_pc.h"

extern long re15_audio_cap_tick(void);   /* audio_pc.c (Runde 38 Spur F): Mitschnitt-Takt */
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);   /* audio_pc.c */

typedef struct {
    int        state;                         /* 0 ungeprueft, 1 geladen, -1 fehlt */
    re15_vab_t vab;
    uint8_t   *edt;
    int        edt_count;
    int16_t   *decoded[RE15_VAB_MAX_SAMPLES];
    int        decoded_len[RE15_VAB_MAX_SAMPLES];
} bank_t;

static bank_t s_bank[3];
static unsigned s_gespielt[3];

static void vags_dekodieren(bank_t *b, const uint8_t *vb, uint32_t vb_size)
{
    for (int i = 0; i < b->vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = b->vab.samples[i].offset, sz = b->vab.samples[i].size;
        if (sz == 0 || off + sz > vb_size) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        b->decoded[i]     = pcm;
        b->decoded_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
}

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* 0x3B: snd0 aus ROOM1131.RDT (RDT-Kopf +0x08/+0x0C/+0x10). */
static int lade_rolladen(bank_t *b)
{
    int sz = 0;
    uint8_t *rdt = re15_pc_read_cd(RE15_R38F_ROLLADEN_RDT, &sz);
    if (!rdt || sz < 0x40) { free(rdt); return -1; }
    uint32_t e = rd32(rdt + 0x08), h = rd32(rdt + 0x0C), v = rd32(rdt + 0x10);
    if (!(e < h && h < v && v < (uint32_t)sz) || re15_vab_parse(rdt + h, (size_t)(v - h), &b->vab) != 0) {
        free(rdt); return -1;
    }
    b->edt = (uint8_t *)malloc(h - e);
    if (!b->edt) { free(rdt); return -1; }
    memcpy(b->edt, rdt + e, h - e);
    b->edt_count = (int)((h - e) / 4);
    vags_dekodieren(b, rdt + v, (uint32_t)sz - v);
    free(rdt);
    return 0;
}

/* 0x3C: KLOPF1060.EDT/.VH/.VB (Muster audio_pc.c re2_schloss_laden). */
static int lade_klopfen(bank_t *b)
{
    int es = 0, hs = 0, vs = 0;
    uint8_t *edt = re15_pc_read_re2(RE15_R38F_KLOPF_DATEI ".EDT", &es);
    uint8_t *vh  = re15_pc_read_re2(RE15_R38F_KLOPF_DATEI ".VH", &hs);
    uint8_t *vb  = re15_pc_read_re2(RE15_R38F_KLOPF_DATEI ".VB", &vs);
    if (!edt || !vh || !vb || es < 4 || re15_vab_parse(vh, (size_t)hs, &b->vab) != 0) {
        free(edt); free(vh); free(vb); return -1;
    }
    vags_dekodieren(b, vb, (uint32_t)vs);
    free(vh); free(vb);
    b->edt = edt;
    b->edt_count = es / 4;
    return 0;
}

/* RIFF/WAVE -> Mono-int16 mit 44100 Hz (lineare Umtastung). PCM 8/16/24/32, Float 32, EXTENSIBLE. */
static int16_t *wav_zu_pcm(const uint8_t *d, int sz, int *out_n)
{
    *out_n = 0;
    if (sz < 44 || memcmp(d, "RIFF", 4) != 0 || memcmp(d + 8, "WAVE", 4) != 0) return NULL;
    int tag = 1, ch = 1, rate = 0, bits = 16, p = 12;
    const uint8_t *data = NULL; uint32_t dlen = 0;
    while (p + 8 <= sz) {
        uint32_t csz = rd32(d + p + 4);
        const uint8_t *body = d + p + 8;
        if (!memcmp(d + p, "fmt ", 4) && p + 8 + 16 <= sz) {
            tag = body[0] | (body[1] << 8); ch = body[2] | (body[3] << 8);
            rate = (int)rd32(body + 4); bits = body[14] | (body[15] << 8);
            if (tag == 0xFFFE && csz >= 40 && p + 8 + 26 <= sz) tag = body[24] | (body[25] << 8);
        } else if (!memcmp(d + p, "data", 4)) {
            data = body;
            uint32_t frei = (uint32_t)(sz - (int)(body - d));
            dlen = csz > frei ? frei : csz;
            break;
        }
        p += 8 + (int)csz + ((int)csz & 1);
    }
    if (!data || rate <= 0 || ch <= 0 || (tag != 1 && tag != 3)) return NULL;
    int bps = bits / 8;
    if (bps < 1 || bps > 4) return NULL;
    int n_in = (int)(dlen / (uint32_t)(bps * ch));
    if (n_in <= 1) return NULL;
    float *mono = (float *)malloc((size_t)n_in * sizeof(float));
    if (!mono) return NULL;
    for (int i = 0; i < n_in; i++) {
        float acc = 0.0f;
        for (int c = 0; c < ch; c++) {
            const uint8_t *s = data + ((size_t)i * ch + c) * bps;
            float v;
            if (tag == 3 && bits == 32) { float f; memcpy(&f, s, 4); v = f; }
            else if (bits == 8)  v = ((int)s[0] - 128) / 128.0f;
            else if (bits == 16) v = (int16_t)(s[0] | (s[1] << 8)) / 32768.0f;
            else if (bits == 24) v = (float)((int32_t)((uint32_t)s[0] << 8 | (uint32_t)s[1] << 16 | (uint32_t)s[2] << 24) >> 8) / 8388608.0f;
            else                 v = (float)(int32_t)rd32(s) / 2147483648.0f;
            acc += v;
        }
        mono[i] = acc / (float)ch;
    }
    int n_out = (int)((int64_t)n_in * 44100 / rate);
    int16_t *out = (int16_t *)malloc((size_t)(n_out > 0 ? n_out : 1) * sizeof(int16_t));
    if (!out) { free(mono); return NULL; }
    for (int i = 0; i < n_out; i++) {
        double pos = (double)i * rate / 44100.0;
        int a = (int)pos; double f = pos - a;
        float s0 = mono[a < n_in ? a : n_in - 1], s1 = mono[a + 1 < n_in ? a + 1 : n_in - 1];
        float v = (float)(s0 + (s1 - s0) * f) * 32767.0f;
        out[i] = (int16_t)(v > 32767.0f ? 32767 : (v < -32768.0f ? -32768 : v));
    }
    free(mono);
    *out_n = n_out;
    return out;
}

/* 0x3D: john_help.wav als Ein-Ton-Bank (s. Kopf). */
static int lade_hilfe(bank_t *b)
{
    int sz = 0;
    uint8_t *w = re15_pc_read_base(RE15_R38F_HILFE_DATEI, &sz);
    if (!w) return -1;
    int n = 0;
    int16_t *pcm = wav_zu_pcm(w, sz, &n);
    free(w);
    if (!pcm || n <= 0) { free(pcm); return -1; }
    memset(&b->vab, 0, sizeof b->vab);
    b->vab.program_count = 1;
    b->vab.tone_count    = 1;
    b->vab.vag_count     = 1;
    b->vab.master_volume = 127;
    b->vab.master_pan    = 64;
    b->vab.prog_mvol[0]  = 127;
    b->vab.prog_mpan[0]  = 64;
    b->vab.tones_loaded  = 1;
    b->vab.tones[0].vol = 127; b->vab.tones[0].pan = 64;
    b->vab.tones[0].center_note = 60; b->vab.tones[0].min_note = 60; b->vab.tones[0].max_note = 127;
    b->vab.tones[0].pitch_shift = 0; b->vab.tones[0].vag_index = 1;
    b->edt = (uint8_t *)malloc(4);
    if (!b->edt) { free(pcm); return -1; }
    b->edt[0] = 0x00; b->edt[1] = 0x00; b->edt[2] = 0x01; b->edt[3] = 0x01;
    b->edt_count = 1;
    b->decoded[0] = pcm;
    b->decoded_len[0] = n;
    return 0;
}

static void spielen(int bank, int satz)
{
    int k = (bank == RE15_R38F_SE_BANK_ROLLADEN) ? 0 : (bank == RE15_R38F_SE_BANK_KLOPFEN) ? 1 : 2;
    bank_t *b = &s_bank[k];
    if (!b->state) {
        int rc = (k == 0) ? lade_rolladen(b) : (k == 1) ? lade_klopfen(b) : lade_hilfe(b);
        b->state = rc == 0 ? 1 : -1;
        fprintf(stderr, "[dining] Port-Bank 0x%02X %s (%d VAGs, %d Saetze)\n", bank,
                rc == 0 ? "geladen" : "FEHLT -> stumm", b->vab.vag_count, b->edt_count);
    }
    if (b->state != 1 || satz < 0 || satz >= b->edt_count) return;
    s_gespielt[k]++;
    if (getenv("RE15_SE_DEBUG"))   /* Messschrieb: Spielbild + Mitschnitt-Takt (RE15_AUDIO_CAP_SYNC, Offset = Takt * 1470) */
        fprintf(stderr, "[se] Se_on bank=0x%02X id=%d -> Runde 38 Spur F (%s) F%u tick=%ld\n", bank, satz,
                k == 0 ? "ROOM1131-snd0 Rolladen" : k == 1 ? "RE2 ROOM1060 Klopfen" : "john_help.wav",
                (unsigned)g_engine.frame_count, re15_audio_cap_tick());
    re15_audio_se_play_bank(b->edt, &b->vab, b->decoded, b->decoded_len, satz);
}

unsigned re15_pc_dining_r38_gespielt(int k) { return (k >= 0 && k < 3) ? s_gespielt[k] : 0u; }

void re15_pc_dining_r38_init(void)
{
    re15_dining_r38_se_hook = spielen;
}
