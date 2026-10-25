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
 * Nachbesserung 6 (Abnahme 5 Maengel 1/2, Nachtrag 25 (o)/(p), C-E4; Dossier §0g):
 *   * Countdown in der Form der RE2-Anzeige: 8 Zeichen aus re15_ventilkette_r41_countdown_re2, je Zeichen ein 14x14-Feld
 *     aus RE2 TEX.TIM Seite 0 (shared_assets/RE2/TEX.TIM) bei x 104 + 14 i, y 32, CLUT-Zeile 480 + 2 * Farbe - Nachbau
 *     von FUN_800318a4 (Kopf re15_ventilkette_r41.h "Countdown"); Texel mit CLUT-Wert 0x0000 sind durchsichtig (PSX-Regel).
 *     Gezeichnet in die oberste Text-Ebene (re15_render_pc_config_tile_ov), wie die Druckwarteschlange FUN_800316a8 ueber dem
 *     Bild liegt.
 *   * RE2-Alarm SUB_25: re15_ventilkette_r41_alarm() an -> re15_audio_r41_alarm(1) (audio_pc.c, Haken "Runde 41 Spur C,
 *     C-E4"); Flut/Tod in 2030 -> re15_audio_r41_alarm(0); beim Raumwechsel ersetzt der Raum-BGM-Lader den SUB-Platz.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_engine.h"          /* SCREEN_XRES / SCREEN_YRES, re15_render_tile */
#include "re15_ventilkette_r41.h"
#include "re15_room.h"            /* g_current_room_id (Nachbesserung 6: Alarm nur in 2030 ausschalten) */
#include "re15_bss.h"             /* re15_bss_parse_chunk / _vlc_decode / _mdec_decode */
#include "re15_vab.h"
#include "re15_audio.h"
#include "asset_root_pc.h"        /* re15_pc_read_cd / re15_pc_read_re2 */

extern void re15_render_pc_show_fmv(const uint32_t *rgba, int w, int h);
extern int  re15_render_pc_msg_text(int x, int y, const unsigned char *raw, int len);   /* render_pc.c: TEX.TIM-Schrift */
extern void re15_render_pc_hide_fmv(void);
extern void re15_audio_re2_karte_se(int se_id);                              /* audio_pc.c (Runde 36 Spur G) */
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);       /* audio_pc.c */
extern void re15_render_pc_config_tile_ov(const re15_tim_t *t, int su, int sv, int w, int h, int dx, int dy); /* render_pc.c */
extern int  re15_audio_r41_alarm(int an);                                    /* audio_pc.c, Runde 41 Spur C, C-E4 */
extern long re15_audio_cap_tick(void);                                       /* audio_pc.c (Messtakt)           */

#define R41C_KARTE_SE_AUF_HIER 3   /* gen/re2_karten_bank.inc RE2_KARTE_SE_AUF_HIER = RE2 ROOM20A0 Satz 0x26 */
#include <math.h>
#include "../../../engine/src/gen/ventilkette_r41_hebel.inc"   /* re15_r41c_hebel[4] (tools/r41_ventilkette/hebel2050.py) */
extern uint32_t *re15_pc_framebuffer(void);   /* render_pc.c: RGBA8888, R in Bit 24..31 */

/* 2050-Hebel (Nachtrag 18 Punkt 2): Stellung t 0..16 -> Grund unter der Maske, dann Griff + Klingen senkrecht skaliert um
 * den Drehpunkt, Faktor cos(45 + 90 t/16 Grad) / cos(45 Grad) (Form ROOM11F0 sub06, Kopf re15_ventilkette_r41.h). */
static void hebel_zeichnen(void)
{
    uint32_t *fb = re15_pc_framebuffer();
    if (!fb) return;
    for (int i = 0; i < 4; i++) {
        const int t = re15_ventilkette_r41_hebel_stellung(i);
        if (t <= 0) continue;
        const re15_r41c_hebel_t *h = &re15_r41c_hebel[i];
        const double s = cos((45.0 + 90.0 * t / RE15_R41C_HEBEL_TAKTE) * 3.14159265358979 / 180.0) / cos(45.0 * 3.14159265358979 / 180.0);
        for (int y = 0; y < h->h; y++)
            for (int x = 0; x < h->w; x++) {
                const int k = y * h->w + x;
                if (!h->maske[k]) continue;
                const int sx = h->x0 + x, sy = h->y0 + y;
                if ((unsigned)sx >= SCREEN_XRES || (unsigned)sy >= SCREEN_YRES) continue;
                const uint8_t *g = &h->grund[3 * k];
                fb[sy * SCREEN_XRES + sx] = ((uint32_t)g[0] << 24) | ((uint32_t)g[1] << 16) | ((uint32_t)g[2] << 8) | 0xFFu;
            }
        for (int y = 0; y < h->h; y++)
            for (int x = 0; x < h->w; x++) {
                const int k = y * h->w + x;
                if (!h->maske[k]) continue;
                const int sx = h->x0 + x;
                const int sy = (int)lround(h->dreh_y + (h->y0 + y - h->dreh_y) * s);
                if ((unsigned)sx >= SCREEN_XRES || (unsigned)sy >= SCREEN_YRES) continue;
                const uint8_t *p = &h->teil[3 * k];
                fb[sy * SCREEN_XRES + sx] = ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | 0xFFu;
            }
    }
}
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

/* ---- RE2-Countdown (Nachbesserung 6): Schrift aus RE2 TEX.TIM ---------------------------------------------------- */
/* TEX.TIM (RE2 Datei 0xe1): 4 bpp, CLUT (256,480) 32 x 19, Bild 256 x 256 Halbworte; Seite 0 = Halbworte 0..63 je Zeile =
 * Bytes 0..127 = Texel 0..255 (FUN_80031af4 GetTPage(0,0,0x300,0x100), TEX.TIM auf Seite 0x1c @0x8002b8cc). */
static struct {
    int      state;                        /* 0 ungeprueft, 1 geladen, -1 fehlt */
    uint8_t  idx[2][256 * 256];            /* je Farbe: Texel-Index, 0 = durchsichtig (CLUT-Wert 0x0000)              */
    uint16_t clut[2][16];                  /* CLUT-Zeile 480 + 2 * Farbe (Farbe 1 = Zeile 482, Farbe 2 = Zeile 484)    */
} s_re2tex;

static int re2tex_laden(void)
{
    int n = 0;
    uint8_t *b = re15_pc_read_re2("TEX.TIM", &n);
    int ok = 0;
    if (b && n >= 20 + 32 * 19 * 2 + 12 + 256 * 256 * 2 && b[0] == 0x10 && (b[4] & 8) && (b[4] & 7) == 0) {
        const uint8_t *c = b + 8;                                     /* CLUT-Block: Laenge, x, y, w, h, Daten          */
        const int cx = c[4] | (c[5] << 8), cy = c[6] | (c[7] << 8), cw = c[8] | (c[9] << 8), ch = c[10] | (c[11] << 8);
        const uint32_t clen = (uint32_t)c[0] | ((uint32_t)c[1] << 8) | ((uint32_t)c[2] << 16) | ((uint32_t)c[3] << 24);
        const uint8_t *im = c + clen;                                 /* Bild-Block                                      */
        const int iw = im[8] | (im[9] << 8), ih = im[10] | (im[11] << 8);
        if (cx == 256 && cy == RE15_R41C_CD_CLUT_Y0 && cw == 32 && ch >= 5 && iw == 256 && ih == 256 &&
            (size_t)(im - b) + 12u + 256u * 256u * 2u <= (size_t)n) {
            for (int f = 0; f < 2; f++) {
                const int zeile = 2 * (f + 1);                         /* Farbe f+1 -> CLUT y 480 + 2 (f+1)              */
                for (int k = 0; k < 16; k++)
                    s_re2tex.clut[f][k] = (uint16_t)(c[12 + (zeile * cw + k) * 2] | (c[12 + (zeile * cw + k) * 2 + 1] << 8));
                for (int v = 0; v < 256; v++)
                    for (int u = 0; u < 256; u++) {
                        const uint8_t byte = im[12 + v * iw * 2 + u / 2];
                        const uint8_t t = (u & 1) ? (uint8_t)(byte >> 4) : (uint8_t)(byte & 15);
                        s_re2tex.idx[f][v * 256 + u] = s_re2tex.clut[f][t] ? t : 0;   /* 0x0000 = durchsichtig */
                    }
            }
            ok = 1;
        }
    }
    free(b);
    fprintf(stderr, "[ventilkette41] RE2-Countdown-Schrift shared_assets/RE2/TEX.TIM %s\n", ok ? "geladen" : "FEHLT/defekt -> kein Countdown");
    return ok ? 1 : -1;
}

/* FUN_800318a4 nachgebaut: je Zeichen c != ' ' ein 14x14-Feld uv ((c % 18) * 14, (c / 18) * 14) bei (x + 14 i, y). */
static void re2_countdown_zeichnen(const char *txt, int farbe)
{
    if (!s_re2tex.state) s_re2tex.state = re2tex_laden();
    if (s_re2tex.state != 1 || farbe < 1 || farbe > 2) return;
    re15_tim_t t;
    memset(&t, 0, sizeof t);
    t.bpp = 8; t.has_clut = 1; t.clut_entries = 16; t.clut = s_re2tex.clut[farbe - 1];
    t.width = 256; t.height = 256; t.pixels = (const uint16_t *)(const void *)s_re2tex.idx[farbe - 1];
    for (int i = 0; txt[i]; i++) {
        const uint8_t c = (uint8_t)txt[i];
        if (c == 0x20) continue;                                      /* @0x80031970: Leerzeichen nur Vorschub */
        re15_render_pc_config_tile_ov(&t, (c % RE15_R41C_CD_SPALTEN) * RE15_R41C_CD_GLYPHE,
                                      (c / RE15_R41C_CD_SPALTEN) * RE15_R41C_CD_GLYPHE,
                                      RE15_R41C_CD_GLYPHE, RE15_R41C_CD_GLYPHE,
                                      RE15_R41C_CD_X + RE15_R41C_CD_SCHRITT * i, RE15_R41C_CD_Y);
    }
}

/* ---- je Spielbild (main.c, nach dem Hintergrund) ----------------------------------------------------------- */
void re15_ventilkette_r41_pc_bild(void)
{
    static int s_angemeldet = 0, s_vollbild = 0;
    if (!s_angemeldet) { re15_ventilkette_r41_ton_hook = ton_spielen; s_angemeldet = 1; }

    /* 2050-Pult: Hebel (Nachbesserung 5), Zeiger (rot) + Hebelmarken (gruen), Cut 15 (Farben wie bisher in main.c). */
    hebel_zeichnen();
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

    /* Countdown der 60 s in 2030 (Nachtrag 18 Punkt 4) in der Form der RE2-Anzeige (Nachbesserung 6, Kopf "Countdown"). */
    static int s_cd_vorher = 0;
    int cd_bild = 0;
    if (re15_ventilkette_r41_countdown(&cd_bild)) {
        char txt[RE15_R41C_CD_ZEICHEN + 1];
        const int farbe = re15_ventilkette_r41_countdown_re2(cd_bild, RE15_R41C_ZAEHLER_BILDER / RE15_R41C_CD_TAKTE, txt);
        re2_countdown_zeichnen(txt, farbe);
        if (!s_cd_vorher || cd_bild % RE15_R41C_CD_TAKTE == 0 || (txt[6] == '0' && txt[7] == '0'))
            fprintf(stderr, "[ventilkette41] Countdown RE2-Form Bild %d \"%s\" Farbe %d (x %d y %d) F%u\n", cd_bild, txt, farbe,
                    RE15_R41C_CD_X, RE15_R41C_CD_Y, (unsigned)g_engine.frame_count);
        s_cd_vorher = 1;
    } else s_cd_vorher = 0;

    /* RE2-Alarm SUB_25 (Nachbesserung 6, Kopf "Alarm"): an mit dem Warntext 46, aus mit Flut/Tod in 2030; nach einem
     * Raumwechsel hat der Raum-BGM-Lader den SUB-Platz schon neu belegt (audio_pc.c) -> hier nur vergessen. */
    static int s_alarm = 0;
    const int alarm_soll = re15_ventilkette_r41_alarm();
    if (alarm_soll && !s_alarm) {
        const int geladen = re15_audio_r41_alarm(1);
        fprintf(stderr, "[bgm] Runde 41 Spur C: RE2-Alarm SUB_25 an (ROOM%04X, Warntext 46) %s capTick %ld F%u\n",
                (unsigned)g_current_room_id, geladen ? "geladen" : "NICHT geladen", re15_audio_cap_tick(),
                (unsigned)g_engine.frame_count);
        s_alarm = 1;
    } else if (!alarm_soll && s_alarm) {
        const int in2030 = ((g_current_room_id & 0xFFFEu) == RE15_R41C_RAUM_2030);
        if (in2030) re15_audio_r41_alarm(0);
        fprintf(stderr, "[bgm] Runde 41 Spur C: RE2-Alarm SUB_25 aus (%s) capTick %ld F%u\n",
                in2030 ? "Flut/Tod" : "Raumwechsel, SUB-Platz vom Raum-BGM-Lader", re15_audio_cap_tick(),
                (unsigned)g_engine.frame_count);
        s_alarm = 0;
    }
}
