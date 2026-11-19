/*
 * zws_r41_pc.c — Runde 41, Zwischensequenzen mit den Gorilla-Filmen (114: ROOM1141 John/Elza, 11C: ROOM11C0 Leon/Ada):
 * PC-Seite von engine/src/zws_r41.c. Herleitung und Ablauf: include/re15_zws_r41.h.
 *
 *   * Schrei (re15_zws_r41_schrei_hook): die Raum-SE-Bank snd1 von ROOM11C0 (RDT-Kopf +0x14/+0x18/+0x1C, gelesen mit
 *     re15_rdt_parse) einmal laden und den Satz ueber re15_audio_se_play_bank spielen - derselbe Weg wie jeder Raum-SE
 *     (se_play_layers: Ton-Pegel/-Pan, Tonhoehe note2pitch2, Stimmen-Gate). Lader-Muster dining_r38_pc.c lade_rolladen.
 *     Rueckgabe = Laenge in SCD-Takten (re15_zws_r41_satz_takte) - auch ohne Audiogeraet, damit der Ablauf gleich bleibt.
 *   * Film (re15_zws_r41_pc_bild, je Spielbild aus platform/pc/main.c VOR re15_render_begin_frame): steht ein Film an, spielt
 *     er die Datei der Tabellenzeile (re15_zws_r41_aktiv()->film_datei) mit dem Weg des Vorspanns CAPCOM.STR (main.c, FE-3):
 *     re15_str.c-Bilder, re15_xa.c-Ton ueber re15_fmv_audio_start (CD->SPU 100/128 wie FUN_8002acac), Ton = Takt (Bild f bei
 *     f/30 s), ohne Audiogeraet 30 Bilder/s nach SDL_GetTicks, START ueberspringt (FUN_80029cd8:24, rohes Bit), Vollbild-Ebene
 *     re15_render_pc_show_fmv. Danach re15_zws_r41_film_gespielt -> das Skript laeuft im naechsten Takt weiter. Fehlt die
 *     Datei, laeuft es sofort weiter (Meldung auf stderr). Der Einstieg ist ein harter Schnitt, nach dem Film blendet das
 *     Spielbild in 15 Bildern aus Schwarz ein - beides wie RE2 (Belege include/re15_zws_r41.h "UEBERBLENDUNG WIE RE2").
 *   * Vorspulen (11C, re15_zws_r41_pc_vorspul_bild, aus platform/pc/main.c am Bildende): solange die Engine nach dem Film den
 *     Original-Auftritt @0x01988..@0x01A5B unsichtbar laufen laesst (re15_zws_r41_vorspulen), wird das Bild nicht
 *     praesentiert (re15_render_pc_bild_halten) und der Bildtakt der Hauptschleife entfaellt
 *     (re15_zws_r41_pc_bild_uebersprungen). Messhaken RE15_ZWS_VORSPULEN_SICHTBAR=1 (kein Spielverhalten): dieselben Bilder
 *     gezeigt und getaktet - fuer den Vergleich des Endzustands mit und ohne Vorspulen.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

#include "re15_engine.h"
#include "re15_zws_r41.h"
#include "re15_rdt.h"
#include "re15_vab.h"
#include "re15_audio.h"
#include "re15_player.h"   /* RE15_PAD_BIT_START */
#include "re15_str.h"
#include "re15_xa.h"
#include "asset_root_pc.h"

extern unsigned g_current_room_id;
extern void   re15_render_pc_show_fmv(const uint32_t *rgba, int w, int h);   /* render_pc.c */
extern void   re15_render_pc_hide_fmv(void);
extern void   re15_audio_tick(void);
/* Strang ton Stufe 1: ROOM11C0 snd0/snd1 ist eine RE1.5-Bank -> Gesetz mit Quadrat (SpuVmKeyOnNow @0x80055d10) */
extern void   re15_audio_se_play_bank_re15(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                           const int *decoded_len, int satz);   /* audio_pc.c */
extern void   re15_fmv_audio_start(const int16_t *src, int src_frames, int src_rate, int cd_vol);
extern void   re15_fmv_audio_stop(void);
extern double re15_fmv_audio_time(void);

static const char *kennung(void)
{
    const re15_zws_film_t *f = re15_zws_r41_aktiv();
    if (!f) f = re15_zws_r41_film_fuer_raum(g_current_room_id);
    return f ? f->kennung : "zws";
}

/* ---- Schrei: snd1 von ROOM11C0 ---------------------------------------------------------------------------------- */
static int        s_bank_zustand = 0;          /* 0 = ungeladen, 1 = geladen, -1 = fehlt */
static uint8_t   *s_rdt_buf = NULL;
static re15_rdt_t s_rdt;
static re15_vab_t s_vab;
static int16_t   *s_dec[RE15_VAB_MAX_SAMPLES];
static int        s_dec_len[RE15_VAB_MAX_SAMPLES];

static int bank_laden(void)
{
    if (s_bank_zustand) return s_bank_zustand == 1 ? 0 : -1;
    s_bank_zustand = -1;
    int n = 0;
    s_rdt_buf = re15_pc_read_cd(RE15_ZWS_SCHREI_RDT, &n);
    const int b = RE15_ZWS_SCHREI_BANK;
    if (!s_rdt_buf || re15_rdt_parse(s_rdt_buf, (size_t)n, &s_rdt) < 0 || !s_rdt.snd_edt[b] || !s_rdt.snd_vh[b] ||
        !s_rdt.snd_vb[b] || re15_vab_parse(s_rdt.snd_vh[b], (size_t)s_rdt.snd_vh_size[b], &s_vab) != 0) {
        fprintf(stderr, "[%s] Tonbank %s snd%d FEHLT -> Schrei stumm\n", kennung(), RE15_ZWS_SCHREI_RDT, b);
        return -1;
    }
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {   /* Muster dining_r38_pc.c vags_dekodieren */
        const uint32_t off = s_vab.samples[i].offset, sz = s_vab.samples[i].size;
        if (sz == 0 || (long)off + (long)sz > (long)s_rdt.snd_vb_size[b]) continue;
        const size_t cap = (sz / 16) * 28;
        s_dec[i] = (int16_t *)malloc(cap * sizeof(int16_t));
        if (s_dec[i]) s_dec_len[i] = re15_vag_adpcm_decode(s_rdt.snd_vb[b] + off, sz, s_dec[i], cap);
    }
    s_bank_zustand = 1;
    fprintf(stderr, "[%s] Tonbank %s snd%d geladen (%d VAGs)\n", kennung(), RE15_ZWS_SCHREI_RDT, b, s_vab.vag_count);
    return 0;
}

static int schrei(int satz)
{
    if (bank_laden() != 0) return -1;
    const int b = RE15_ZWS_SCHREI_BANK;
    const int takte = re15_zws_r41_satz_takte(s_rdt.snd_edt[b], s_rdt.snd_vh[b], s_rdt.snd_vh_size[b], s_rdt.snd_vb[b],
                                              s_rdt.snd_vb_size[b], satz);
    re15_audio_se_play_bank_re15(s_rdt.snd_edt[b], &s_vab, s_dec, s_dec_len, satz);
    fprintf(stderr, "[%s] F%u Schrei ROOM11C0 snd%d Satz %d -> %d Takte\n", kennung(), (unsigned)g_engine.frame_count, b,
            satz, takte);
    return takte;
}

/* ---- Film ------------------------------------------------------------------------------------------------------- */
/* Runde 42 birkinfilm: Filme niedriger als 240 Zeilen (RE2 R409: 320x160) stehen wie in RE2 mit `bild_y` Zeilen Rand oben auf
 * schwarzem Grund (RE2-Filmtabelle @0x800a22cc +0x12 = 0x0028, `lhu v0,0x12(a3)` @0x80032254 -> +0x50, addiert @0x80033200).
 * Die Vollbild-Ebene (re15_render_pc_show_fmv) zieht ihr Bild auf 320x240 - deshalb wird hier auf 320x240 zusammengesetzt. */
#define ZWS_SCHIRM_W 320
#define ZWS_SCHIRM_H 240
static int film_spielen(const char *rel, int bild_y)
{
    int msz = 0;
    uint8_t *mbuf = re15_pc_read_cd(rel, &msz);
    re15_str_t movie;
    memset(&movie, 0, sizeof movie);
    if (!mbuf || re15_str_open(&movie, mbuf, (size_t)msz) != 0) {
        fprintf(stderr, "[%s] Film %s FEHLT/defekt -> Szene laeuft ohne Film weiter\n", kennung(), rel);
        free(mbuf);
        return -1;
    }
    uint32_t *rgba = (uint32_t *)malloc((size_t)movie.width * (size_t)movie.height * 4);
    const int setzen = movie.height < ZWS_SCHIRM_H && movie.width == ZWS_SCHIRM_W && bild_y >= 0 &&
                       bild_y + movie.height <= ZWS_SCHIRM_H;
    uint32_t *schirm = setzen ? (uint32_t *)malloc((size_t)ZWS_SCHIRM_W * ZWS_SCHIRM_H * 4) : NULL;
    if (schirm) for (int i = 0; i < ZWS_SCHIRM_W * ZWS_SCHIRM_H; i++) schirm[i] = 0x000000FFu;   /* schwarz, Alpha 0xFF */
    int16_t *apcm = NULL;
    int aframes = 0, arate = 0, achans = 0, have_audio = 0;
    if (re15_xa_decode_movie(mbuf, (size_t)msz, &apcm, &aframes, &arate, &achans) == 0) {
        re15_fmv_audio_start(apcm, aframes, arate, 100);          /* wie der Vorspann (main.c FE-3): CdlATV 100/128 */
        have_audio = g_audio.initialized && (re15_fmv_audio_time() >= 0.0);
    }
    const uint32_t start_ms = SDL_GetTicks();
    fprintf(stderr, "[%s] F%u Film %s: %d Bilder %dx%d, Ton %d Proben %d Hz (%s)\n", kennung(), (unsigned)g_engine.frame_count,
            rel, movie.num_frames, movie.width, movie.height, aframes, arate, have_audio ? "Takt = Ton" : "30 Bilder/s");
    const uint32_t budget_ms = 1000u / 30u;
    uint32_t zuletzt = 0;
    int gezeigt = 0, abbruch = 0;
    for (int f = 0; f < movie.num_frames && rgba && !abbruch; f++) {
        if (re15_str_decode(&movie, f, rgba) != 0) continue;
        if (have_audio) {                                       /* Ton = Takt: Bild f bei f/30 s */
            const double soll = (double)f / 30.0;
            for (;;) {
                const double t = re15_fmv_audio_time();
                if (t < 0.0 || t >= soll) break;
                if (g_engine.pad_pressed & RE15_PAD_BIT_START) { abbruch = 1; break; }
                re15_input_tick();
                SDL_Delay(2);
            }
            if (abbruch) break;
        }
        re15_render_begin_frame();
        re15_input_tick();
        if (schirm) {
            memcpy(schirm + (size_t)bild_y * ZWS_SCHIRM_W, rgba, (size_t)movie.width * (size_t)movie.height * 4);
            re15_render_pc_show_fmv(schirm, ZWS_SCHIRM_W, ZWS_SCHIRM_H);
        } else {
            re15_render_pc_show_fmv(rgba, movie.width, movie.height);
        }
        re15_render_end_frame();
        re15_audio_tick();
        gezeigt++;
        if (g_engine.pad_pressed & RE15_PAD_BIT_START) abbruch = 1;   /* START ueberspringt (wie der Vorspann) */
        if (have_audio) {
            if (re15_fmv_audio_time() < 0.0) break;
        } else {
            const uint32_t jetzt = SDL_GetTicks();
            if (zuletzt != 0 && jetzt - zuletzt < budget_ms) SDL_Delay(budget_ms - (jetzt - zuletzt));
            zuletzt = SDL_GetTicks();
        }
    }
    re15_fmv_audio_stop();
    free(apcm);
    free(schirm);
    free(rgba);
    free(mbuf);
    re15_render_pc_hide_fmv();
    fprintf(stderr, "[%s] Film zu Ende: %d von %d Bildern gezeigt%s (F%u, %u ms)\n", kennung(), gezeigt, movie.num_frames,
            abbruch ? " (START)" : "", (unsigned)g_engine.frame_count, (unsigned)(SDL_GetTicks() - start_ms));
    return gezeigt;
}

/* ---- Vorspulen (11C) --------------------------------------------------------------------------------------------- */
static int      s_uebersprungen = 0;     /* dieses Bild wurde nicht praesentiert */
static uint32_t s_vorspul_start = 0;     /* Wanduhr beim ersten uebersprungenen Bild */
int re15_zws_r41_pc_vorspul_bild(void)
{
    static int s_sichtbar = -1;
    if (s_sichtbar < 0) { const char *e = getenv("RE15_ZWS_VORSPULEN_SICHTBAR"); s_sichtbar = (e && *e && *e != '0') ? 1 : 0; }
    const int an = re15_zws_r41_vorspulen();
    if (an && !s_uebersprungen) s_vorspul_start = SDL_GetTicks();
    if (!an && s_uebersprungen)
        fprintf(stderr, "[%s] F%u Vorspulen: erstes gezeigtes Bild, %u ms Wanduhr seit dem Film\n", kennung(),
                (unsigned)g_engine.frame_count, (unsigned)(SDL_GetTicks() - s_vorspul_start));
    if (an) re15_zws_r41_vorspul_bild();     /* Zaehler + Notbremse */
    s_uebersprungen = an;
    return an && !s_sichtbar;
}
int re15_zws_r41_pc_bild_uebersprungen(void)
{
    static int s_sichtbar = -1;
    if (s_sichtbar < 0) { const char *e = getenv("RE15_ZWS_VORSPULEN_SICHTBAR"); s_sichtbar = (e && *e && *e != '0') ? 1 : 0; }
    return s_uebersprungen && !s_sichtbar;
}

void re15_zws_r41_pc_bild(void)
{
    if (!re15_zws_r41_film_offen()) return;
    const re15_zws_film_t *f = re15_zws_r41_aktiv();
    const int gezeigt = (f && f->raum == (g_current_room_id & 0xFFFFu)) ? film_spielen(f->film_datei, f->bild_y) : -1;
    if (gezeigt > 0) re15_zws_r41_film_gespielt();   /* RE2-Einblenden (re15_zws_r41.h "UEBERBLENDUNG WIE RE2") */
    else re15_zws_r41_film_fehlt();
}

void re15_pc_zws_r41_init(void)
{
    re15_zws_r41_schrei_hook = schrei;
    re15_zws_r41_film_spieler = 1;
}
