/*
 * annette_se_pc.c — Runde 41 Spur AN, Nachbesserung 1: PC-Seite der vier RE2-Toene der Annette-Szene
 * (engine/src/annette_r41.c ruft ueber re15_annette_r41_se_hook, Satznummern RE15_AN_SE_* in include/re15_annette_r41.h).
 *
 * ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL (Beta -> Retail, Sound ist RE2). Belege je Satz: tools/r41_annette/an_se_cut.py,
 * analysis/befunde_runde41/AN_belege/an_se.md.
 *
 * Mini-Bank shared_assets/RE2/ANNETTE_SE.VBS, Satzformat wie ELZASE.VBS / LIFT6030.VBS (FUN_8005a09c):
 *   [SE-Map @0 .. vh_off)[VH "pBAV" @vh_off][Trailer, u32 vh_off @edt_size-8][VBD]
 *   Satz 0 KRACH  = RE2 Claire ROOM6141 sub02 @0x01040 Se_on(3,0x0B): Bank 3 = ENEMSE-Bank 64 (Paar-Zeile 64 `31 28`)
 *   Satz 1 STURZ  = ROOM6141 sub02 @0x010CE Se_on(2,0x0A), zwei Lagen (Satz-Byte 3 = 0x20) auf derselben Welle
 *   Satz 2 BRUELL = ROOM6141 sub02 @0x01104 Se_on(2,0x0C)
 *   Satz 3 ZUG_AN = RE2 Leon ROOM7020 sub04 @0x014AA Se_on(2,0x0C), Strom fuer den Zug (Set(4,0x88) @0x01466)
 * Geladen wie lift2030_se_pc.c (re15_vab_parse / re15_vag_adpcm_decode), gespielt ueber dieselbe Stimmen-Maschine
 * (re15_audio_se_play_bank -> se_play_layers). Nicht positional (die RE2-Vorlagen spielen Se_on mit Lage 0,0,0).
 * Fehlt die Bank: EINE Protokollzeile, danach still stumm.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_annette_r41.h"
#include "asset_root_pc.h"             /* re15_pc_read_re2 */
#include "../../../engine/src/gen/re2_annette_se_bank.inc"   /* RE2_ANSE_EDT_* / _VBD_* / _SE_* */

/* RE15_AN_SE_* sind enum-Konstanten (fuer #if unsichtbar) -> Pruefung zur Uebersetzungszeit per _Static_assert. */
_Static_assert(RE2_ANSE_SE_KRACH == RE15_AN_SE_KRACH && RE2_ANSE_SE_STURZ == RE15_AN_SE_STURZ &&
               RE2_ANSE_SE_BRUELL == RE15_AN_SE_BRUELL && RE2_ANSE_SE_ZUG_AN == RE15_AN_SE_ZUG_AN &&
               RE2_ANSE_SE_COUNT == RE15_AN_SE_N,
               "gen/re2_annette_se_bank.inc und include/re15_annette_r41.h nummerieren die Saetze verschieden");

extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);   /* audio_pc.c */

static int        s_loaded = 0;
static int        s_failed = 0;
static re15_vab_t s_vab;
static uint8_t   *s_edt = NULL;
static int16_t   *s_decoded[RE15_VAB_MAX_SAMPLES];
static int        s_decoded_len[RE15_VAB_MAX_SAMPLES];
static unsigned   s_gespielt[RE2_ANSE_SE_COUNT];

static int laden(void)
{
    if (s_loaded) return 1;
    if (s_failed) return 0;
    s_failed = 1;                                   /* nur EIN Versuch, danach still stumm */
    int sz = 0;
    uint8_t *vbs = re15_pc_read_re2("ANNETTE_SE.VBS", &sz);
    if (!vbs) {
        fprintf(stderr, "[annettese] shared_assets/RE2/ANNETTE_SE.VBS fehlt -> Annette-Szene ohne RE2-Toene\n");
        return 0;
    }
    if ((unsigned)sz != RE2_ANSE_DATEI_GROESSE || RE2_ANSE_EDT_SIZE < 12) {
        fprintf(stderr, "[annettese] ANNETTE_SE.VBS %d B statt %u -> stumm\n", sz, (unsigned)RE2_ANSE_DATEI_GROESSE);
        free(vbs); return 0;
    }
    uint8_t *edt = (uint8_t *)malloc(RE2_ANSE_EDT_SIZE);
    if (!edt) { free(vbs); return 0; }
    memcpy(edt, vbs + RE2_ANSE_EDT_OFF, RE2_ANSE_EDT_SIZE);
    /* VH-Offset = Trailer-u32 @[edt_size-8] (FUN_8005a09c, wie ELZASE.VBS). */
    uint32_t vh_off = (uint32_t)edt[RE2_ANSE_EDT_SIZE-8]         | ((uint32_t)edt[RE2_ANSE_EDT_SIZE-7] << 8)
                    | ((uint32_t)edt[RE2_ANSE_EDT_SIZE-6] << 16) | ((uint32_t)edt[RE2_ANSE_EDT_SIZE-5] << 24);
    if (vh_off + 0x20u > RE2_ANSE_EDT_SIZE ||
        re15_vab_parse(edt + vh_off, (size_t)RE2_ANSE_EDT_SIZE - vh_off, &s_vab) != 0) {
        fprintf(stderr, "[annettese] ANNETTE_SE.VBS: VH unlesbar -> stumm\n");
        free(edt); free(vbs); return 0;
    }
    const uint8_t *vb = vbs + RE2_ANSE_VBD_OFF;
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, vsz = s_vab.samples[i].size;
        if (off + vsz > RE2_ANSE_VBD_SIZE) continue;
        size_t cap = (vsz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_decoded[i]     = pcm;
        s_decoded_len[i] = re15_vag_adpcm_decode(vb + off, vsz, pcm, cap);
    }
    free(vbs);
    s_edt = edt;
    s_loaded = 1;
    s_failed = 0;
    return 1;
}

/* Der Ton. satz = RE15_AN_SE_* (= Satz der Mini-Bank). */
void re15_pc_annette_se(int satz)
{
    if (satz < 0 || satz >= RE2_ANSE_SE_COUNT) return;
    if (!g_audio.initialized) return;
    if (!laden()) return;
    s_gespielt[satz]++;
    re15_audio_se_play_bank(s_edt, &s_vab, s_decoded, s_decoded_len, satz);
}

unsigned re15_pc_annette_se_gespielt(int satz) { return (satz >= 0 && satz < RE2_ANSE_SE_COUNT) ? s_gespielt[satz] : 0u; }

/* Nachbesserung 4 (Abnahme 2 N4): Tonteil der Zugtuer DOOR2A in die RE2-Tuerbank (Port-Bank 0x0E), damit der Abgang in 6030
 * oeffnen/zufallen spielt (annette_r41.c z_tuer_auf/z_tuer_zu, Muster RE2 Claire ROOM4081 sub05 @0x03634/@0x03644). DOOR2A =
 * alle Zugtueren 5090/6030 (gen/tuer_zuordnung.inc S320/S321); Tonteil = Datei[0 .. 0x4028) laut RE2-Archivtabelle @0x8009A718
 * (`28 40 54 87 09 00 00 00 62 3f 00 00`, gen/re2_tuer_tabelle.inc), gelesen wie door_scene_pc.c re2_archiv_lesen. */
#include "re15_door_seq.h"   /* re15_door_seq_re2_archiv */
#define AN_TUER_ARCHIV 0x2A
int re15_pc_annette_tuerbank(void)
{
    int ton = 0, modell = 0, sektor = 0, groesse = 0, n = 0;
    if (!g_audio.initialized) return 0;
    if (re15_door_seq_re2_archiv(AN_TUER_ARCHIV, &ton, &modell, &sektor, &groesse) != 0) return 0;
    uint8_t *d = re15_pc_read_re2("DOOR/DOOR2A.DO2", &n);
    if (!d) { fprintf(stderr, "[annette] shared_assets/RE2/DOOR/DOOR2A.DO2 fehlt - Tuerton still\n"); return 0; }
    int ok = 0;
    if (n == groesse && sektor * 0x800 + modell == n && ton > 0 && ton <= n)
        ok = re15_audio_re2_tuer_laden(d, ton);          /* kopiert; gleiche Bytes schon geladen -> nichts zu tun */
    else
        fprintf(stderr, "[annette] DOOR2A.DO2: %d B passt nicht zur Tabelle @0x8009A718 (%d B)\n", n, groesse);
    free(d);
    return ok;
}
