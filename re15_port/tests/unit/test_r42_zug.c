/*
 * test_r42_zug.c — Runde 42 Strang "finale", Stufe 2 "Abfahrt": Zugfilm im Zugstart, WARNING beim Krachen; Stufe 4: Wagen B ohne
 * Loch. Herleitung: include/re15_zws_r41.h ("Zugfilm Raum 600"), engine/src/annette_r41.c (z_film, z_warnung), tools/r42_story/
 * szenen_stage6.py (Elza).
 *
 * Teile (Aufruf: test_r42_zug <teil>):
 *   datei    MOVIE/R600ZUG.STR: 405 Bilder 320x240 mit den Port-Dekodern, CD-XA 37800 Hz Stereo = Filmlaenge, und der Ton steigt
 *            bei Film-Bild 18 an (der Film traegt das Hochfahren ROOM7020 snd0 Satz 0x0C selbst - Grund fuer den Wegfall von
 *            z_sig_an)
 *   tabelle  Filmtabelle: zug600 (ROOM6030) / zug601 (ROOM6031) an Port-Zeile 60, kein Schrei, kein Vorspulen; Haken parkt bis
 *            zum Filmende, danach Einblenden 0x7800 wie 114; in anderen Raeumen keine Wirkung
 *   leon     Zugstart ROOM6030: Pultlicht -> Film-Zeile -> Cut 1 + Sleep 60 -> msg 27; kein Zug-an-Signal; nach dem Krachen
 *            das WARNING-Blinken (= ROOM6030 @0x01224..@0x0123F bytegleich) und Cut 1, vor msg 28
 *   elza     Zugstart ROOM6031: dasselbe (ROOM6031 @0x00FF0..@0x0100B), vor msg 14 bzw. 15
 *   musik    Stufe 2 Runde 2 (Pruefbericht s2r1 Rot 1, Raummusik unter dem Film): direkt vor der Film-Zeile MAIN (Slot 0) anhalten
 *            (Sce_bgm_control Op 4 = SsSeqPause, RE1.5 FUN_80044da4 @0x80044efc), direkt danach weiter (Op 3 = SsSeqReplay
 *            @0x80044ec4); Form der Original-Zeile 6030 sub06 @0x01092 bzw. 6031 @0x00EB8 (nur das Op-Byte anders); Vorbild RE2
 *            ROOM1080 sub14 @0x02524/@0x02536. Kein Befehl an Slot 1 (SUB laeuft bei (3,89)=1 nicht: 6030 @0x01060/@0x01118,
 *            6031 @0x00E86/@0x00F3A; ein Op 3 wuerde ihn einschalten). In der VM: der Halte-Befehl geht in die Ton-Warteschlange,
 *            BEVOR die Film-Zeile parkt; waehrend des Films nichts; nach dem Film der Weiter-Befehl
 *   wagenb   Stufe 4: das Loch (Stufe 2, Cut_replace(8,10) beim Raumaufbau) ist wieder entfernt. NUTZER 18.11.2026: "Aber natuerlich
 *            soll man nicht einfach ueber das Loch laufen koennen." / "Finden wir keinen Weg, muss es halt raus..." - gemessen gibt es
 *            mit dem Loch ab dem Anfahren keinen Weg nach Wagen C (Bericht s4 r1). Geprueft: 5090/5091 bauen Wagen B nach dem
 *            Zugstart wie das Original (sub00 @0x0127C / @0x01260 `04 ff 18 03` startet sub03 unveraendert): Zonen 7->8 @0x3F0 und
 *            9->8 @0x454, nie ->10; dazu re15_rdt_zonen_auffrischen (FUN_800396fc @0x800397e8) stellt getauschte Zonen zurueck
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_str.h"
#include "re15_xa.h"
#include "re15_zws_r41.h"
#include "re15_annette_r41.h"
#include "re15_story_r42.h"
#include "re15_story6_r42.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
/* Gegenprobe gegen den Stand vor Stufe 2 (aaea5f8c2 kennt die Makros nicht): dieselben Werte wie include/re15_zws_r41.h. */
#ifndef RE15_ZUG600_MSG_FILM
#define RE15_ZUG600_RAUM_LEON    0x6030u
#define RE15_ZUG600_RAUM_ELZA    0x6031u
#define RE15_ZUG600_MSG_FILM     60
#define RE15_ZUG600_FILM         "MOVIE/R600ZUG.STR"
#define RE15_ZUG600_FILM_BILDER  405
#endif

extern unsigned g_current_room_id;
extern void scd_register_current_rdt(const re15_rdt_t *);
extern const uint8_t *re15_annette_r41_programm_zug(int *out_len);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *asset(const char *rel, size_t *n)
{
    char p[600];
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

/* ---- datei ---------------------------------------------------------------------------------------------------- */
static double rms(const int16_t *pcm, int a, int b)
{
    double s = 0;
    for (int i = a; i < b; i++) { const double l = pcm[2 * i], r = pcm[2 * i + 1]; s += l * l + r * r; }
    return sqrt(s / (2.0 * (b - a)) + 1e-9);
}

static void teil_datei(void)
{
    printf("== datei ==\n");
    size_t n = 0;
    uint8_t *d = asset(RE15_ZUG600_FILM, &n);
    PRUEF(d && n > 0 && n % 2336 == 0, "%s vorhanden, %zu B = %zu Sektoren zu 2336 B", RE15_ZUG600_FILM, n, n / 2336);
    if (!d) return;
    re15_str_t s;
    memset(&s, 0, sizeof s);
    PRUEF(re15_str_open(&s, d, n) == 0 && s.num_frames == RE15_ZUG600_FILM_BILDER && s.width == 320 && s.height == 240,
          "re15_str_open: %d Bilder %dx%d (Quelle 405 Bilder bei 30/s)", s.num_frames, s.width, s.height);
    uint32_t *rgba = (uint32_t *)malloc(320 * 240 * 4);
    int fehler = 0;
    for (int f = 0; f < s.num_frames && rgba; f++) if (re15_str_decode(&s, f, rgba) != 0) fehler++;
    PRUEF(fehler == 0, "alle %d Bilder mit den Port-Dekodern dekodiert (%d Fehler)", s.num_frames, fehler);
    free(rgba);
    int16_t *pcm = NULL;
    int fr = 0, rate = 0, ch = 0;
    const int rc = re15_xa_decode_movie(d, n, &pcm, &fr, &rate, &ch);
    const double dauer = rate ? (double)fr / rate : 0.0, film = RE15_ZUG600_FILM_BILDER / 30.0;
    PRUEF(rc == 0 && rate == 37800 && ch == 2 && dauer >= film - 0.05 && dauer <= film + 0.1,
          "CD-XA-Ton 37800 Hz Stereo, %.3f s (Film %.3f s)", dauer, film);
    if (pcm && rate) {
        /* Film-Bild 18 (1-basiert) beginnt bei 17/30 s. Vorher [0, 16/30 s), danach [18/30, 30/30 s). */
        const double vor = rms(pcm, 0, rate * 16 / 30), nach = rms(pcm, rate * 18 / 30, rate);
        const double db = 20.0 * log10(nach / vor);
        printf("    Ton-Pegel vor Bild 17: %.1f, Bild 19..30: %.1f (%+.1f dB)\n", vor, nach, db);
        PRUEF(db > 6.0, "Hochfahren im Film: Pegel steigt ab Bild 18 um %.1f dB (> 6 dB)", db);
    }
    free(pcm);
    free(d);
}

/* ---- tabelle -------------------------------------------------------------------------------------------------- */
static void teil_tabelle(void)
{
    printf("== tabelle ==\n");
    const re15_zws_film_t *l = re15_zws_r41_film_fuer_raum(RE15_ZUG600_RAUM_LEON);
    const re15_zws_film_t *e = re15_zws_r41_film_fuer_raum(RE15_ZUG600_RAUM_ELZA);
    PRUEF(l && e && l != e, "Filmtabelle: Eintraege fuer ROOM6030 und ROOM6031");
    if (!l || !e) return;
    for (int k = 0; k < 2; k++) {
        const re15_zws_film_t *f = k ? e : l;
        PRUEF(f->film.msg == RE15_ZUG600_MSG_FILM && f->schrei.msg < 0 && f->schrei.n == 0 && f->sichtbar.n == 0 &&
              !strcmp(f->film_datei, RE15_ZUG600_FILM) && f->film_bilder == 405,
              "%s: Film-Zeile %d, kein Schrei, kein Vorspulen, %s mit %d Bildern", f->kennung, f->film.msg, f->film_datei,
              f->film_bilder);
    }
    static const uint8_t zeile[4] = { 0x2b, RE15_ZUG600_MSG_FILM, 0x00, 0x00 };
    const unsigned raeume[2] = { RE15_ZUG600_RAUM_LEON, RE15_ZUG600_RAUM_ELZA };
    for (int k = 0; k < 2; k++) {
        g_current_room_id = raeume[k];
        re15_zws_r41_install((uint16_t)raeume[k]);
        re15_zws_r41_film_spieler = 0;
        PRUEF(re15_zws_r41_message_on(zeile) == 1, "ROOM%04X ohne Filmspieler: Zeile laeuft durch", raeume[k]);
        re15_zws_r41_install((uint16_t)raeume[k]);
        re15_zws_r41_film_spieler = 1;
        memset(g_fade_ch, 0, sizeof g_fade_ch);
        const int r1 = re15_zws_r41_message_on(zeile), r2 = re15_zws_r41_message_on(zeile);
        const re15_zws_film_t *a = re15_zws_r41_aktiv();
        PRUEF(r1 == 2 && r2 == 2 && re15_zws_r41_film_offen() && a && !strcmp(a->film_datei, RE15_ZUG600_FILM),
              "ROOM%04X mit Filmspieler: parkt (%d, %d), Film %s angefordert", raeume[k], r1, r2, a ? a->film_datei : "-");
        re15_zws_r41_film_gespielt();
        PRUEF(g_fade_ch[RE15_ZWS_BLENDE_KANAL].level == RE15_ZWS_BLENDE_PEGEL && re15_zws_r41_message_on(zeile) == 1,
              "ROOM%04X nach dem Film: Einblenden Kanal %d Pegel 0x%04X wie 114, Zeile verbraucht", raeume[k],
              RE15_ZWS_BLENDE_KANAL, (unsigned)g_fade_ch[RE15_ZWS_BLENDE_KANAL].level);
    }
    g_current_room_id = 0x5090u;
    re15_zws_r41_install(0x5090u);
    PRUEF(re15_zws_r41_message_on(zeile) == 0, "ROOM5090: Zeile 60 nicht vom Film-Haken belegt");
    re15_zws_r41_film_spieler = 0;
}

/* ---- leon / elza ---------------------------------------------------------------------------------------------- */
/* Opcode-Walk: Position des ersten Vorkommens von pat (Laenge n) an einer Opcode-Grenze ab Position ab, sonst -1. */
static int finde(const uint8_t *p, int len, int ab, const uint8_t *pat, int n)
{
    for (int i = 0; i < len;) {
        const int l = scd_opcode_size_at(p + i);
        if (l <= 0) return -2;
        if (i >= ab && i + n <= len && !memcmp(p + i, pat, (size_t)n)) return i;
        i += l;
    }
    return -1;
}
static int zaehle(const uint8_t *p, int len, const uint8_t *pat, int n)
{
    int c = 0;
    for (int i = 0; i < len;) {
        const int l = scd_opcode_size_at(p + i);
        if (l <= 0) break;
        if (i + n <= len && !memcmp(p + i, pat, (size_t)n)) c++;
        i += l;
    }
    return c;
}

static const uint8_t k_halt[6]   = { 0x54, 0x00, 0x04, 0x00, 0x00, 0x00 };
static const uint8_t k_weiter[6] = { 0x54, 0x00, 0x03, 0x00, 0x00, 0x00 };

static void programm_pruefen(const char *wer, const uint8_t *p, int len, const char *rdt, unsigned warn_off, uint8_t msg_danach,
                             uint8_t msg_nach_krach, uint8_t sig_an, uint8_t sig_krach)
{
    size_t rn = 0;
    uint8_t *r = asset(rdt, &rn);
    PRUEF(p && len > 0 && r, "%s: Programm (%d B) und %s", wer, len, rdt);
    if (!p || !r) { free(r); return; }
    static const uint8_t licht_aus[4] = { 0x22, 0x01, 0x1c, 0x00 };
    static const uint8_t film[4] = { 0x2b, RE15_ZUG600_MSG_FILM, 0x00, 0x00 };
    static const uint8_t cut1_60[6] = { 0x29, 0x01, 0x09, 0x0a, 0x3c, 0x00 };
    static const uint8_t cut1[2] = { 0x29, 0x01 };
    const uint8_t an[4] = { 0x22, 0x05, sig_an, 0x01 }, krach[4] = { 0x22, 0x05, sig_krach, 0x01 };
    const uint8_t m1[4] = { 0x2b, msg_danach, 0x00, 0x00 }, m2[4] = { 0x2b, msg_nach_krach, 0x00, 0x00 };
    const int i_aus = finde(p, len, 0, licht_aus, 4), i_film = finde(p, len, 0, film, 4);
    const int i_cut = i_film >= 0 ? finde(p, len, i_film, cut1_60, 6) : -1, i_m1 = finde(p, len, 0, m1, 4);
    /* Stufe 2 Runde 2: zwischen Pultlicht und Film die Halte-Befehle, zwischen Film und Cut 1 die Weiter-Befehle (Teil musik) */
    PRUEF(zaehle(p, len, film, 4) == 1 && i_aus >= 0 && i_film == i_aus + 4 + 6 && i_cut == i_film + 4 + 6 && i_m1 > i_cut,
          "%s: Pultlicht aus @+0x%X -> Musik halt -> Film-Zeile 60 @+0x%X -> Musik weiter -> Cut 1 + Sleep 60 @+0x%X -> msg %d @+0x%X",
          wer, i_aus, i_film, i_cut, msg_danach, i_m1);
    PRUEF(zaehle(p, len, an, 4) == 0, "%s: kein Zug-an-Signal Set(5,0x%02X) mehr (der Film traegt den Ton)", wer, sig_an);
    const int i_kr = finde(p, len, 0, krach, 4), i_w = i_kr >= 0 ? i_kr + 4 : -1, i_m2 = finde(p, len, 0, m2, 4);
    const int w_ok = i_w >= 0 && i_w + 28 <= len && warn_off + 28 <= rn && !memcmp(p + i_w, r + warn_off, 28);
    const int c_ok = w_ok && !memcmp(p + i_w + 28, cut1, 2);
    PRUEF(i_kr > i_m1 && w_ok && c_ok && i_m2 > i_w + 30,
          "%s: Krachen @+0x%X, direkt danach WARNING = %s @0x%05X (28 B bytegleich) und Cut 1, vor msg %d @+0x%X", wer, i_kr, rdt,
          warn_off, msg_nach_krach, i_m2);
    /* Inhalt des WARNING-Blocks: For 3 mit Cut 4 / Cut 5 */
    PRUEF(w_ok && p[i_w + 4] == 0x0d && p[i_w + 8] == 3 && p[i_w + 10] == 0x29 && p[i_w + 11] == 4 && p[i_w + 16] == 0x29 &&
          p[i_w + 17] == 5, "%s: WARNING = For 3 { Cut 4, Sleep 15, Cut 5, Sleep 15 }", wer);
    free(r);
}

static void teil_leon(void)
{
    printf("== leon ==\n");
    int len = 0;
    const uint8_t *p = re15_annette_r41_programm_zug(&len);
    programm_pruefen("Leon ROOM6030", p, len, "STAGE6/ROOM6030.RDT", 0x01224u, RE15_AN_MSG_FAEHRT, RE15_AN_MSG_WAS,
                     RE15_AN_SIG_ZUG_AN, RE15_AN_SIG_KRACH);
}

static void teil_elza(void)
{
    printf("== elza ==\n");
    int len = 0;
    const uint8_t *p = re15_story6_r42_programm(RE15_S6_P_ZUG, &len);
    programm_pruefen("Elza ROOM6031", p, len, "STAGE6/ROOM6031.RDT", 0x00FF0u, 14, 15, RE15_S6_SIG_ZUG_AN, RE15_S6_SIG_KRACH);
}

/* ---- musik --------------------------------------------------------------------------------------------------- */
static void musik_pruefen(const char *wer, const uint8_t *p, int len, const char *rdt, unsigned raum, unsigned off_main)
{
    size_t rn = 0;
    uint8_t *r = asset(rdt, &rn);
    static const uint8_t film[4] = { 0x2b, RE15_ZUG600_MSG_FILM, 0x00, 0x00 };
    const int i_film = (p && len > 0) ? finde(p, len, 0, film, 4) : -1;
    PRUEF(r && i_film >= 6 && i_film + 10 <= len, "%s: Programm (%d B), Film-Zeile @+0x%X", wer, len, i_film);
    if (!r || i_film < 6 || i_film + 10 > len) { free(r); return; }
    const uint8_t *h = p + i_film - 6, *w = p + i_film + 4;
    int n54 = 0, slot1 = 0;
    for (int i = 0; i < len;) {
        const int l = scd_opcode_size_at(p + i);
        if (l <= 0) break;
        if (p[i] == 0x54) { n54++; if (p[i + 1] != 0) slot1++; }
        i += l;
    }
    PRUEF(!memcmp(h, k_halt, 6) && !memcmp(w, k_weiter, 6) && n54 == 2 && slot1 == 0,
          "%s: direkt vor dem Film MAIN Op 4, direkt danach MAIN Op 3; im Programm %d x 0x54 (Soll 2), davon %d an Slot != 0 (Soll 0)",
          wer, n54, slot1);
    /* Form = Original-Zeile des Raums (Zweig Ck(3,89)=1), nur das Op-Byte (pc[2]) anders */
    int gleich = off_main + 6 <= rn && !memcmp(r + off_main, "\x54\x00\x01\x00\x00\x00", 6);
    for (int k = 0; k < 6 && gleich; k++) if (k != 2) gleich = h[k] == r[off_main + k] && w[k] == r[off_main + k];
    PRUEF(gleich, "%s: Form = %s @0x%05X `54 00 01 00 00 00` (MAIN), nur Op 4/3 statt 1", wer, rdt, off_main);
    free(r);
    /* VM: Halt -> Film (parkt) -> Weiter, ueber die Ton-Warteschlange */
    static uint8_t prog[64];
    memcpy(prog, h, 6 + 4 + 6);
    prog[16] = 0x01; prog[17] = 0x00;                           /* Evt_end */
    g_current_room_id = raum;
    scd_register_current_rdt(NULL);
    scd_vm_init();
    re15_zws_r41_install((uint16_t)raum);
    re15_zws_r41_film_spieler = 1;
    scd_audio_event_t ev;
    while (scd_audio_queue_pop(&ev)) { }
    scd_thread_start(1, prog);
    for (int i = 0; i < 4; i++) scd_vm_tick();
    int ev_vor[4][2], n_vor = 0;
    while (scd_audio_queue_pop(&ev)) if (ev.kind == SCD_AUDIO_SEQ_CTL && n_vor < 4) { ev_vor[n_vor][0] = ev.bank; ev_vor[n_vor][1] = ev.volume; n_vor++; }
    PRUEF(n_vor == 1 && ev_vor[0][0] == 0 && ev_vor[0][1] == 4 && re15_zws_r41_film_offen(),
          "%s VM: vor dem Film %d Ton-Befehl(e), (Slot,Op) = (%d,%d) [Soll 1: (0,4)], Film angefordert, Skript parkt", wer, n_vor,
          n_vor > 0 ? ev_vor[0][0] : -1, n_vor > 0 ? ev_vor[0][1] : -1);
    for (int i = 0; i < 10; i++) scd_vm_tick();
    int n_film = 0;
    while (scd_audio_queue_pop(&ev)) if (ev.kind == SCD_AUDIO_SEQ_CTL) n_film++;
    PRUEF(n_film == 0 && re15_zws_r41_film_offen(), "%s VM: waehrend des Films kein Musik-Befehl (%d)", wer, n_film);
    re15_zws_r41_film_gespielt();
    for (int i = 0; i < 4; i++) scd_vm_tick();
    int ev_nach[4][2], n_nach = 0;
    while (scd_audio_queue_pop(&ev)) if (ev.kind == SCD_AUDIO_SEQ_CTL && n_nach < 4) { ev_nach[n_nach][0] = ev.bank; ev_nach[n_nach][1] = ev.volume; n_nach++; }
    PRUEF(n_nach == 1 && ev_nach[0][0] == 0 && ev_nach[0][1] == 3,
          "%s VM: nach dem Film %d Ton-Befehl(e), (Slot,Op) = (%d,%d) [Soll 1: (0,3)]", wer, n_nach,
          n_nach > 0 ? ev_nach[0][0] : -1, n_nach > 0 ? ev_nach[0][1] : -1);
    re15_zws_r41_film_spieler = 0;
}

static void teil_musik(void)
{
    printf("== musik ==\n");
    int len = 0;
    const uint8_t *p = re15_annette_r41_programm_zug(&len);
    musik_pruefen("Leon ROOM6030", p, len, "STAGE6/ROOM6030.RDT", RE15_ZUG600_RAUM_LEON, 0x01092u);
    len = 0;
    p = re15_story6_r42_programm(RE15_S6_P_ZUG, &len);
    musik_pruefen("Elza ROOM6031", p, len, "STAGE6/ROOM6031.RDT", RE15_ZUG600_RAUM_ELZA, 0x00EB8u);
}

/* ---- wagenb --------------------------------------------------------------------------------------------------- */
static re15_rdt_t s_rdt;
static uint8_t *raum_laden(const char *rel, unsigned id, int zug_leon, int zug_elza)
{
    size_t sz = 0;
    uint8_t *buf = asset(rel, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) { free(buf); return NULL; }
    scd_register_current_rdt(NULL);
    g_current_room_id = id;
    re15_enemy_reset();
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_game_flag_set(3, 89, 1);                       /* Tor: 5090 sub00 @0x01278 / 5091 @0x0125C Ck(3,89)=1 */
    re15_game_flag_set(3, 42, 1);                       /* Kampf gelaufen: keine Annette-Szene im Weg */
    re15_game_flag_set(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT, zug_leon);
    re15_game_flag_set(RE15_R42_BANK, RE15_R42_BIT_ZUG_ELZA, zug_elza);
    scd_register_current_rdt(&s_rdt);
    scd_vm_set_room_init(1);
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 5;
    for (int i = 0; i < 60; i++) scd_vm_tick();
    scd_vm_set_room_init(0);
    return buf;
}

static int zone_hat(int von, int nach)
{
    for (int i = 0; i < s_rdt.zone_count; i++) if (s_rdt.zones[i].cam_from == von && s_rdt.zones[i].cam_to == nach) return 1;
    return 0;
}

static void teil_wagenb(void)
{
    printf("== wagenb ==\n");
    size_t n0 = 0, n1 = 0;
    uint8_t *r0 = asset("STAGE5/ROOM5090.RDT", &n0), *r1 = asset("STAGE5/ROOM5091.RDT", &n1);
    PRUEF(r0 && r1 && !memcmp(r0 + 0x0127C, "\x04\xff\x18\x03", 4) && !memcmp(r1 + 0x01260, "\x04\xff\x18\x03", 4),
          "sub00 @0x0127C / @0x01260 Evt_exec sub03 (Licht-Schleife, Original)");
    free(r0); free(r1);
    /* VM: Raumaufbau mit und ohne Zugstart-Bit - immer Bild 8 (Original), nie Bild 10 */
    static const struct { const char *rdt; unsigned id; int zl, ze; } faelle[] = {
        { "STAGE5/ROOM5090.RDT", 0x5090u, 0, 0 }, { "STAGE5/ROOM5090.RDT", 0x5090u, 1, 0 },
        { "STAGE5/ROOM5091.RDT", 0x5091u, 0, 0 }, { "STAGE5/ROOM5091.RDT", 0x5091u, 0, 1 },
    };
    for (unsigned i = 0; i < sizeof faelle / sizeof faelle[0]; i++) {
        uint8_t *b = raum_laden(faelle[i].rdt, faelle[i].id, faelle[i].zl, faelle[i].ze);
        const int z78 = zone_hat(7, 8), z98 = zone_hat(9, 8), z710 = zone_hat(7, 10), z910 = zone_hat(9, 10);
        PRUEF(b && z78 && z98 && !z710 && !z910, "ROOM%04X Zugstart Leon %d / Elza %d: Zonen 7->8 %d, 9->8 %d, 7->10 %d, 9->10 %d "
              "(Soll Bild 8 wie das Original, kein Loch)", faelle[i].id, faelle[i].zl, faelle[i].ze, z78, z98, z710, z910);
        if (b && faelle[i].zl + faelle[i].ze) {   /* Raumlader: getauschte Zonen kommen frisch aus der Datei (FUN_800396fc @0x800397e8) */
            for (int k = 0; k < s_rdt.zone_count; k++) if (s_rdt.zones[k].cam_to == 8) s_rdt.zones[k].cam_to = 10;
            const int ok2 = re15_rdt_zonen_auffrischen(&s_rdt) == 0 && zone_hat(7, 8) && zone_hat(9, 8) && !zone_hat(7, 10);
            PRUEF(ok2, "ROOM%04X re15_rdt_zonen_auffrischen: Zonen wieder wie in der Datei (7->8, 9->8)", faelle[i].id);
        }
        scd_register_current_rdt(NULL);
        free(b);
    }
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "alle";
    const int alle = !strcmp(t, "alle");
    if (alle || !strcmp(t, "datei"))   teil_datei();
    if (alle || !strcmp(t, "tabelle")) teil_tabelle();
    if (alle || !strcmp(t, "leon"))    teil_leon();
    if (alle || !strcmp(t, "elza"))    teil_elza();
    if (alle || !strcmp(t, "musik"))   teil_musik();
    if (alle || !strcmp(t, "wagenb"))  teil_wagenb();
    printf("%s: %d Fehler\n", g_fail ? "ROT" : "GRUEN", g_fail);
    return g_fail ? 1 : 0;
}
