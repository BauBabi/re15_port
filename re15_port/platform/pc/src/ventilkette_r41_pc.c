/*
 * ventilkette_r41_pc.c — Runde 41 Spur C "ventilkette": PC-Seite des Moduls engine/src/ventilkette_r41.c
 * (include/re15_ventilkette_r41.h; Dossier analysis/befunde_runde41/C_ventilkette.md §0f).
 *
 * Ein Aufruf je Spielbild aus platform/pc/main.c (direkt nach dem Hintergrund, Haken "Runde 41 Spur C"):
 *   * 2050-Pult (Cut 15): roter Zeiger + gruene Marken unter den EIN-Hebeln (vorher inline in main.c, Nachbesserung 1).
 *   * Vollbild-Ebene (re15_render_pc_show_fmv, dieselbe wie STR-Filme): die Plakat-Nahaufnahme 2030 (Nachbesserung 4)
 *     ODER das Standbild der Ventile beim Einsetzen der Sicherung in den Ventilkasten 2010 (Nachbesserung 5,
 *     Nachtrag 16 Folge 2): ROOM2030 Cut 7 = BSS/ROOM2030/BG07.BSS, dekodiert mit denselben Engine-Dekodern wie jeder
 *     Raumhintergrund (bss_common.c / bss_vlc.c / bss_mdec.c; Kette wie tests/unit/probe_bg_dump.c).
 * Dazu die zwei Plattform-Dienste des Moduls:
 *   * Toene (re15_ventilkette_r41_ton_hook): RE15_R41C_TON_VENTIL = RE2 ROOM1080 snd0 Satz 5 (Ventil-Drehgeraeusch,
 *     se_on `36 02 05 01 ...` @0x02168), Bank shared_assets/RE2/VENTIL1080.EDT/.VH/.VB (bytegleich aus der RDT,
 *     tools/r41_ventilkette/re2_ventil_export.py; Lader-Muster dining_r38_pc.c lade_klopfen); RE15_R41C_TON_AUF =
 *     RE2 "You unlocked it." Satz 0x26 (EXE @0x800515f8) = KARTSE.VBS Satz 3 (gen/re2_karten_bank.inc
 *     RE2_KARTE_SE_AUF_HIER, Runde 36 Spur G) ueber re15_audio_re2_karte_se.
 *   * RBJ (re15_ventilkette_r41_pc_rbj): ROOM2030/2031 haben kein eigenes RBJ (RDT-Kopfwort [21] = 0); der Raum bindet
 *     das RBJ von RE2 ROOM1080 (@0x39AC, 13128 B, shared_assets/RE2/VENTIL1080.RBJ) als Spieler-Overlay, damit
 *     Plc_motion(0,15..17) die Ventil-Animation spielt (main.c: Boot- und Tuer-Weg, neben der Leihe cut10f0_pc.c).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_engine.h"          /* SCREEN_XRES / SCREEN_YRES, re15_render_tile */
#include "re15_ventilkette_r41.h"
#include "re15_bss.h"             /* re15_bss_parse_chunk / _vlc_decode / _mdec_decode */
#include "re15_vab.h"
#include "re15_audio.h"
#include "asset_root_pc.h"        /* re15_pc_read_cd / re15_pc_read_re2 */

extern void re15_render_pc_show_fmv(const uint32_t *rgba, int w, int h);
extern void re15_render_pc_hide_fmv(void);
extern void re15_audio_re2_karte_se(int se_id);                              /* audio_pc.c (Runde 36 Spur G) */
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);       /* audio_pc.c */

#define R41C_KARTE_SE_AUF_HIER 3   /* gen/re2_karten_bank.inc RE2_KARTE_SE_AUF_HIER = RE2 ROOM20A0 Satz 0x26 */
#define R41C_BSS_CHUNK 0x10000     /* eine Kamera-Scheibe (bg_pc.c / probe_bg_dump.c) */

/* ---- Ventil-Bank VENTIL1080 (RE2 ROOM1080 snd0) ------------------------------------------------------------- */
static struct {
    int        state;                         /* 0 ungeprueft, 1 geladen, -1 fehlt */
    re15_vab_t vab;
    uint8_t   *edt;
    int        edt_count;
    int16_t   *decoded[RE15_VAB_MAX_SAMPLES];
    int        decoded_len[RE15_VAB_MAX_SAMPLES];
} s_ventil;

static int ventil_bank_laden(void)
{
    int es = 0, hs = 0, vs = 0;
    uint8_t *edt = re15_pc_read_re2("VENTIL1080.EDT", &es);
    uint8_t *vh  = re15_pc_read_re2("VENTIL1080.VH", &hs);
    uint8_t *vb  = re15_pc_read_re2("VENTIL1080.VB", &vs);
    if (!edt || !vh || !vb || es < 4 || re15_vab_parse(vh, (size_t)hs, &s_ventil.vab) != 0) {
        free(edt); free(vh); free(vb);
        return -1;
    }
    for (int i = 0; i < s_ventil.vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_ventil.vab.samples[i].offset, sz = s_ventil.vab.samples[i].size;
        if (sz == 0 || off + sz > (uint32_t)vs) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_ventil.decoded[i]     = pcm;
        s_ventil.decoded_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
    free(vh); free(vb);
    s_ventil.edt = edt;
    s_ventil.edt_count = es / 4;
    return 0;
}

static void ton_spielen(int ton)
{
    if (ton == RE15_R41C_TON_AUF) {
        if (getenv("RE15_SE_DEBUG"))
            fprintf(stderr, "[se] Runde 41 Spur C: Aufschliess-Ton KARTSE Satz %d (RE2 Satz 0x26, EXE @0x800515f8) F%u\n",
                    R41C_KARTE_SE_AUF_HIER, (unsigned)g_engine.frame_count);
        re15_audio_re2_karte_se(R41C_KARTE_SE_AUF_HIER);
        return;
    }
    if (ton != RE15_R41C_TON_VENTIL) return;
    if (!s_ventil.state) {
        s_ventil.state = ventil_bank_laden() == 0 ? 1 : -1;
        fprintf(stderr, "[ventilkette41] Bank VENTIL1080 (RE2 ROOM1080 snd0) %s (%d VAGs, %d Saetze)\n",
                s_ventil.state == 1 ? "geladen" : "FEHLT -> stumm", s_ventil.vab.vag_count, s_ventil.edt_count);
    }
    if (s_ventil.state != 1 || RE15_R41C_VENTIL_SE >= s_ventil.edt_count) return;
    if (getenv("RE15_SE_DEBUG"))
        fprintf(stderr, "[se] Runde 41 Spur C: Ventil-Ton VENTIL1080 Satz %d (RE2 ROOM1080 se_on @0x02168) F%u\n",
                RE15_R41C_VENTIL_SE, (unsigned)g_engine.frame_count);
    re15_audio_se_play_bank(s_ventil.edt, &s_ventil.vab, s_ventil.decoded, s_ventil.decoded_len, RE15_R41C_VENTIL_SE);
}

/* ---- RE2-RBJ fuer 2030/2031 ------------------------------------------------------------------------------- */
static uint8_t *s_rbj = NULL;
static int      s_rbj_n = 0;

uint8_t *re15_ventilkette_r41_pc_rbj(unsigned room_id, int *size)
{
    if (!size || !re15_ventilkette_r41_ventil_rbj(room_id)) return NULL;
    if (!s_rbj) {
        s_rbj = re15_pc_read_re2("VENTIL1080.RBJ", &s_rbj_n);
        if (!s_rbj || s_rbj_n <= 0) { free(s_rbj); s_rbj = NULL; s_rbj_n = 0; }
    }
    if (!s_rbj) {
        fprintf(stderr, "[rbj] Runde 41 Spur C: shared_assets/RE2/VENTIL1080.RBJ fehlt -> keine Ventil-Animation\n");
        return NULL;
    }
    *size = s_rbj_n;
    fprintf(stderr, "[rbj] Runde 41 Spur C: ROOM%04X bindet das RBJ von RE2 ROOM1080 (VENTIL1080.RBJ, %d B; Ventil-Animation)\n",
            room_id, s_rbj_n);
    return s_rbj;   /* bleibt resident (main.c: rbj_borrowed = 1, nie free) */
}

/* ---- Standbild der Ventile (2030 Cut 7) --------------------------------------------------------------------- */
static uint32_t s_standbild[RE15_BSS_FRAME_WIDTH * RE15_BSS_FRAME_HEIGHT];
static int      s_standbild_ok = -1;           /* -1 ungeprueft, 0 fehlt, 1 dekodiert */

static int standbild_laden(unsigned raum, int cut)
{
    char rel[48];
    int n = 0;
    snprintf(rel, sizeof rel, "BSS/ROOM%04X/BG%02d.BSS", raum, cut);
    uint8_t *buf = re15_pc_read_cd(rel, &n);
    if (!buf || n < R41C_BSS_CHUNK) { free(buf); return 0; }
    re15_bss_chunk_t ch;
    int ok = 0;
    if (re15_bss_parse_chunk(buf, R41C_BSS_CHUNK, &ch) && re15_bss_chunk_has_video(&ch)) {
        size_t cap = ((size_t)ch.run_length_words + 2) * 8 + 4096;
        int16_t *co = (int16_t *)malloc(cap * sizeof(int16_t));
        uint8_t *rgb = (uint8_t *)malloc((size_t)RE15_BSS_FRAME_WIDTH * RE15_BSS_FRAME_HEIGHT * 3);
        if (co && rgb) {
            int k = re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size, ch.run_length_words, ch.quant, ch.version,
                                        co, cap);
            if (k >= 0 && re15_bss_mdec_decode(co, (size_t)k, RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT, rgb) == 0) {
                for (int i = 0; i < RE15_BSS_FRAME_WIDTH * RE15_BSS_FRAME_HEIGHT; i++)
                    s_standbild[i] = ((uint32_t)rgb[3 * i] << 24) | ((uint32_t)rgb[3 * i + 1] << 16) |
                                     ((uint32_t)rgb[3 * i + 2] << 8) | 0xFFu;
                ok = 1;
            }
        }
        free(co); free(rgb);
    }
    free(buf);
    fprintf(stderr, "[ventilkette41] Standbild %s %s\n", rel, ok ? "dekodiert" : "FEHLT/defekt");
    return ok;
}

/* ---- je Spielbild (main.c, nach dem Hintergrund) ----------------------------------------------------------- */
void re15_ventilkette_r41_pc_bild(void)
{
    static int s_angemeldet = 0, s_vollbild = 0;
    if (!s_angemeldet) { re15_ventilkette_r41_ton_hook = ton_spielen; s_angemeldet = 1; }

    /* 2050-Pult: Zeiger (rot) + Hebelmarken (gruen), Cut 15 (Farben wie bisher in main.c, Nachbesserung 1). */
    int vx0, vy0, vw, vh;
    if (re15_ventilkette_r41_sicht(0, &vx0, &vy0, &vw, &vh)) re15_render_tile(vx0, vy0, vw, vh, 0, 208, 24, 40);
    for (int vn = 1; vn <= 4; vn++)
        if (re15_ventilkette_r41_sicht(vn, &vx0, &vy0, &vw, &vh)) re15_render_tile(vx0, vy0, vw, vh, 0, 40, 200, 60);

    /* Vollbild-Ebene: Plakat (2030) oder Standbild der Ventile (2010); nur die eigene Ebene wird wieder versteckt. */
    const uint32_t *bild = re15_ventilkette_r41_plakat_rgba();
    unsigned sraum = 0; int scut = 0;
    if (!bild && re15_ventilkette_r41_standbild(&sraum, &scut)) {
        if (s_standbild_ok < 0) s_standbild_ok = standbild_laden(sraum, scut);
        if (s_standbild_ok == 1) bild = s_standbild;
    }
    if (bild) { re15_render_pc_show_fmv(bild, 320, 240); s_vollbild = 1; }
    else if (s_vollbild) { re15_render_pc_hide_fmv(); s_vollbild = 0; }
}
