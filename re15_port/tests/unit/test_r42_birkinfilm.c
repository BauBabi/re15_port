/*
 * test_r42_birkinfilm.c — Runde 42 Strang "birkinfilm", Stufe 1: Riegel fuer den RE2-Film "Birkins Mutation" in Leons erstem
 * Treffen mit Annette (ROOM6000 sub06). Herleitung: include/re15_birkinfilm_r42.h, include/re15_zws_r41.h ("ZWEITE BLENDENART"),
 * tools/r42_birkinfilm/*.py. Harness wie tests/unit/test_r41_zws114.c (echte Engine-Wege: scd_room_reenter mit allen Installern,
 * SCD-VM, Walker, Nachrichten-FSM, re15_game_step).
 *
 * Teile (Aufruf: test_r42_birkinfilm <teil>):
 *   datei     MOVIE/R409BIRK.STR = R409.STR ohne die Sektoren [11630, 13460) (Bilder 2327..2692), Rest bytegleich; 3927 Bilder
 *             320x160 mit den Port-Dekodern; Schnitt zwischen Weissblende (Bild 2325 hell) und Kanal; CD-XA-Ton = Filmlaenge
 *   xa        R409_XA09/11/14.XAS = die Kanal-Sektoren der Lesefenster aus VOICE04.XAS (STAGE4.BIN @0x384), Konstanten
 *   programm  Programm = sub06 bytegleich + Einschub vor @0x01156 (Folge der Zeilen, RE2-Sleeps, Musik), Texte 40..42 + 48..50
 *             (Strang namen: je Satz eine Nachricht wie RE2 ROOM4090, jede mit dem Namenskopf "Annette:" der RDT-Zeile msg 1)
 *   ablauf    ROOM6000 am VM: msg 0, 1, 2, 40, 48, 41, 49, 42, 50, 3 in dieser Folge; Satz 2 (48/49/50) 41/81/81 Bilder nach Satz 1
 *             (RE2 Sleep @0x02722/@0x027D8/@0x02850) ohne eigene Stimme; jede gezeigte Annette-Seite beginnt mit "Annette:"
 *             (Strang namen, Gegenprobe rot auf 8db089f5d: Seite 2 von 40/41/42 ohne Namen); Film genau einmal zwischen 41 und 42, erst nach dem
 *             Ende von Stimme 41 + Sleep 2 + Sleep 60 + 64 Abblendbildern; Blende vor dem Film 0, 4 ... 252 (64 Bilder) + 252,
 *             nach dem Film 255, 251 ... 3 (64 Bilder), dann Halt; MAIN Op 4 vor, Op 3 nach (Sleep 63); msg 42 nach Sleep 63
 *   einmal    zweites Betreten mit (3,100)=1: keine Umleitung, kein Film
 * Stufe 2 (Elza ROOM5011, ohne neue Saetze):
 *   programm5011  Programm = sub02 bytegleich + Einschub vor @0x00690 (Warte-Zeile 46, Sleep 2, Sleep 60, MAIN Op 4, Film 47,
 *                 Sleep 63, MAIN Op 3), Umleitung nur 5011 sub02 mit Signatur, Filmtabelle birk5011, keine RE2-Stimme in 5011
 *   ablauf5011    ROOM5011 am VM (Ankunft wie nach 5021: (4,237)=1, (3,58)=1): Zeilen 0..16 unveraendert in Folge, Film genau
 *                 einmal zwischen msg 3 und msg 4, erst nach Stimme 3 (synchro main03.wav) + Sleep 2 + 60 + 64 Abblendbildern;
 *                 Blenden wie 6000; msg 4 nach Sleep 63; MAIN Op 4 / Op 3; (4,237) danach 0, Szene endet wie im Original
 *   einmal5011    zweites Betreten ((4,237)=0 nach der Szene): keine Szene, kein Film, keine Umleitung
 * Gegenprobe (rot auf aaea5f8c2): ohne die Haken (scd_vm.c / msg_common.c / zws_r41.c) laufen ablauf und einmal ohne Film und
 * ohne Zeilen 40..42 (Bericht BERICHT_s1_r1.md).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_item_discard.h"
#include "re15_gameflow.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_zws_r41.h"
#include "re15_birkinfilm_r42.h"
#include "re15_str.h"
#include "re15_xa.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
#ifndef RE15_ASSET_RE2_DIR
#error RE15_ASSET_RE2_DIR fehlt
#endif
#ifndef RE15_INFO_RE2_DIR
#error RE15_INFO_RE2_DIR fehlt
#endif
#ifndef RE15_SYNCHRO_DIR
#error RE15_SYNCHRO_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}
static uint8_t *aus(const char *dir, const char *rel, size_t *n)
{
    char p[700];
    snprintf(p, sizeof p, "%s/%s", dir, rel);
    return slurp(p, n);
}
static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

static double helligkeit(const uint32_t *rgba, int n)
{
    double s = 0;
    for (int i = 0; i < n; i++) s += ((rgba[i] >> 24) & 0xFF) + ((rgba[i] >> 16) & 0xFF) + ((rgba[i] >> 8) & 0xFF);
    return s / (3.0 * n);
}

/* ---- Teil: datei ------------------------------------------------------------------------------------------------- */
#define S 2336
#define SCHNITT_A 11630   /* erster Sektor von Bild 2327 (STR-Kopf +0x08) */
#define SCHNITT_B 13460   /* erster Sektor von Bild 2693 */

static double bild_hell(const re15_str_t *s, int f, uint32_t *rgba)
{
    if (re15_str_decode(s, f, rgba) != 0) return -1.0;
    return helligkeit(rgba, s->width * s->height);
}

static void teil_datei(void)
{
    printf("== datei ==\n");
    size_t n = 0, no = 0;
    uint8_t *d = aus(RE15_ASSET_PSX_DIR, RE15_BF_FILM, &n);
    uint8_t *o = aus(RE15_INFO_RE2_DIR, "PL0/ZMOVIE/R409.STR", &no);
    PRUEF(d && n % S == 0, "%s vorhanden, %zu B = %zu Sektoren", RE15_BF_FILM, n, n / S);
    PRUEF(o && no == 21465u * S, "Quelle info/re2leon/PL0/ZMOVIE/R409.STR: %zu B (21465 Sektoren)", no);
    if (!d || !o) { free(d); free(o); return; }
    PRUEF(n == no - (size_t)(SCHNITT_B - SCHNITT_A) * S && !memcmp(d, o, (size_t)SCHNITT_A * S) &&
          !memcmp(d + (size_t)SCHNITT_A * S, o + (size_t)SCHNITT_B * S, n - (size_t)SCHNITT_A * S),
          "Schnitt: Sektoren [%d, %d) entfernt, alle anderen bytegleich und in Reihenfolge", SCHNITT_A, SCHNITT_B);
    PRUEF(rd16(o + (size_t)SCHNITT_A * S + 8) == 0x0160 && rd16(o + (size_t)SCHNITT_A * S + 12) == 0 &&
          rd32(o + (size_t)SCHNITT_A * S + 16) == 2327 && rd32(o + (size_t)SCHNITT_B * S + 16) == 2693 &&
          rd32(o + (size_t)(SCHNITT_A - 5) * S + 16) == 2326,
          "Schnittstellen = Bildanfaenge: @Sektor %d Bild %u, @Sektor %d Bild %u (STR-Kopf +0x08)", SCHNITT_A,
          rd32(o + (size_t)SCHNITT_A * S + 16), SCHNITT_B, rd32(o + (size_t)SCHNITT_B * S + 16));
    re15_str_t s, so;
    PRUEF(re15_str_open(&s, d, n) == 0 && s.num_frames == RE15_BF_FILM_BILDER && s.width == 320 && s.height == 160,
          "re15_str_open: %d Bilder %dx%d (Soll %d = 4293 - 366)", s.num_frames, s.width, s.height, RE15_BF_FILM_BILDER);
    PRUEF(re15_str_open(&so, o, no) == 0 && so.num_frames == 4293, "Quelle: %d Bilder", so.num_frames);
    uint32_t *rgba = (uint32_t *)malloc(320 * 240 * 4);
    if (rgba) {
        /* Weissblende-Ende / Einschub / Kanal in der Quelle (0-basiert) */
        /* Gemessen 2026-11-18 (ffmpeg signalstats YAVG an R409.STR[0].avi, 0-basiert): 2300 95,2 -> 2324 146,4 / 2325 145,7
         * (Aufhellen nach der Spritze), 2326 41,7 (harter Wechsel in den Einschub), 2684..2691 0 (Abblende), 2692 24,0 (Kanal). */
        const double a = bild_hell(&so, 2300, rgba), w = bild_hell(&so, 2325, rgba), e1 = bild_hell(&so, 2326, rgba),
                     e2 = bild_hell(&so, 2691, rgba), k = bild_hell(&so, 2692, rgba);
        printf("    Quelle: Bild 2300 %.1f, 2325 %.1f (Ende des Aufhellens), 2326 %.1f (Einschub Anfang), 2691 %.1f (Einschub Ende), "
               "2692 %.1f (Kanal)\n", a, w, e1, e2, k);
        PRUEF(w > a + 30.0 && e1 >= 0 && e1 < w - 80.0, "Schnitt nach dem Aufhellen (Bild 2300 %.1f -> 2325 %.1f), das erste "
              "entfernte Bild ist ein harter Szenenwechsel (%.1f)", a, w, e1);
        PRUEF(e2 >= 0 && e2 < 4.0 && k > e2, "letztes entferntes Bild schwarz (%.1f, Abblende des Einschubs), danach der Kanal (%.1f)",
              e2, k);
        const double n1 = bild_hell(&s, 2325, rgba), n2 = bild_hell(&s, 2326, rgba);
        PRUEF(n1 == w && n2 == k, "neue Datei: Bild 2325 = Quelle 2325 (%.1f), Bild 2326 = Quelle 2692 (%.1f)", n1, n2);
        int fehler = 0;
        for (int f = 0; f < s.num_frames; f += 97) if (re15_str_decode(&s, f, rgba) != 0) fehler++;
        PRUEF(fehler == 0, "Stichprobe jedes 97. Bild mit den Port-Dekodern dekodiert (%d Fehler)", fehler);
        printf("    letzte Bilder der Quelle:");
        for (int f = 4286; f < 4293; f++) printf(" %d:%.1f", f + 1, bild_hell(&so, f, rgba));
        printf("   (Filmtabelle +0x04 = 0x10bf = 4287, Leser nicht gefunden)\n");
        free(rgba);
    }
    int16_t *pcm = NULL; int fr = 0, rate = 0, ch = 0;
    const int rc = re15_xa_decode_movie(d, n, &pcm, &fr, &rate, &ch);
    const double dauer = rc == 0 ? (double)fr / rate : 0, film = RE15_BF_FILM_BILDER / 30.0;
    PRUEF(rc == 0 && rate == 37800 && ch == 2, "CD-XA-Ton: 37800 Hz Stereo (rc %d)", rc);
    PRUEF(dauer > film - 0.1 && dauer < film + 0.1, "Tonlaenge %.4f s ~ Filmlaenge %.4f s (2454 Ton-Sektoren x 2016 Proben)", dauer, film);
    free(pcm);
    size_t nt = 0;
    uint8_t *t = aus(RE15_ASSET_PSX_DIR, "MOVIE/R409BIRK.TXT", &nt);
    PRUEF(t && nt > 0 && strstr((const char *)t, "film_schneiden.py") && strstr((const char *)t, "80241f0a3211979b83410e7565cda960"),
          "Herkunft MOVIE/R409BIRK.TXT (Werkzeug, Quell-md5)");
    free(t); free(d); free(o);
}

/* ---- Teil: xa ---------------------------------------------------------------------------------------------------- */
static void teil_xa(void)
{
    printf("== xa ==\n");
    size_t nx = 0, ns = 0;
    uint8_t *x = aus(RE15_INFO_RE2_DIR, "PL0/VOICE/VOICE04.XAS", &nx);
    uint8_t *st = aus(RE15_INFO_RE2_DIR, "COMMON/BIN/STAGE4.BIN", &ns);
    PRUEF(x && nx == 1472u * S && st && ns > 0x400, "VOICE04.XAS (%zu B = Dateisatz 0x347, 3438592 B) und STAGE4.BIN", nx);
    if (!x || !st) { free(x); free(st); return; }
    static const struct { int nr, kanal, laenge, lese, msg; const char *datei; } z[3] = {
        { 0x09, 6, 68, RE15_BF_XA09_LESE, RE15_BF_MSG_XA09, RE15_BF_XA09_DATEI },
        { 0x11, 4, 71, RE15_BF_XA11_LESE, RE15_BF_MSG_XA11, RE15_BF_XA11_DATEI },
        { 0x14, 3, 78, RE15_BF_XA14_LESE, RE15_BF_MSG_XA14, RE15_BF_XA14_DATEI },
    };
    for (int i = 0; i < 3; i++) {
        const uint8_t *e = st + 0x384 + z[i].nr * 6;
        PRUEF(rd16(e) == z[i].kanal && rd16(e + 2) == 0 && rd16(e + 4) == z[i].laenge && z[i].lese == z[i].laenge * 16 - 0x1E,
              "STAGE4.BIN @0x%X Xa 0x%02X = (Kanal %d, Block %d, Laenge %d), Lesefenster %d = Laenge*16 - 0x1E",
              0x384 + z[i].nr * 6, z[i].nr, rd16(e), rd16(e + 2), rd16(e + 4), z[i].lese);
        size_t n = 0;
        uint8_t *d = aus(RE15_ASSET_RE2_DIR, z[i].datei, &n);
        int ok = d && n == (size_t)(z[i].laenge - 1) * S;
        for (int k = 0; ok && k < z[i].laenge - 1; k++)
            if (memcmp(d + (size_t)k * S, x + (size_t)(z[i].kanal + 16 * k) * S, S) != 0 || d[(size_t)k * S + 1] != z[i].kanal) ok = 0;
        PRUEF(ok, "%s = %d Kanal-%d-Sektoren aus VOICE04.XAS (Sektor %d, %d, ...), bytegleich", z[i].datei, z[i].laenge - 1,
              z[i].kanal, z[i].kanal, z[i].kanal + 16);
        int lese = 0;
        const char *s = re15_birkinfilm_r42_stimme(RE15_BF_RAUM, z[i].msg, &lese);
        PRUEF(s && !strcmp(s, z[i].datei) && lese == z[i].lese, "Sprachkanal: ROOM6000 msg %d -> %s, %d Sektoren", z[i].msg,
              s ? s : "-", lese);
        free(d);
    }
    PRUEF(!re15_birkinfilm_r42_stimme(0x6001, RE15_BF_MSG_XA09, NULL) && !re15_birkinfilm_r42_stimme(RE15_BF_RAUM, 3, NULL) &&
          !re15_birkinfilm_r42_stimme(RE15_BF_RAUM, RE15_BF_MSG_FILM, NULL), "andere Raeume/Ids: keine RE2-Stimme");
    free(x); free(st);
}

/* ---- Harness (wie test_r41_zws114.c) ------------------------------------------------------------------------------- */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static long               s_bild = 0;

typedef struct { long bild; uint8_t slot, op; } musik_t;
static musik_t s_musik[64];
static int     s_n_musik = 0;
static int     s_stimme_rest = 0;   /* Plattform-Ersatz Sprachkanal: Restbilder der laufenden RE2-Stimme */
static int     s_stimmen[8], s_n_stimmen = 0;
static int     s_synchro = 0;       /* 1 = auch synchro-Aufnahmen als laufende Stimme nachbilden (Stufe 2, ROOM5011) */
static int     s_synchro_bilder[64];   /* je Id: Laenge der Aufnahme in Bildern (0 = keine Datei) */
static int     s_synchro_raum = -1;

/* Laenge einer synchro-Aufnahme in Bildern (30/s): RIFF "fmt " Byte-Rate, "data"-Groesse; aufgerundet. 0 = keine Datei. */
static int synchro_bilder(unsigned raum, int id)
{
    char p[700];
    snprintf(p, sizeof p, "%s/STAGE%X/room%04X/main%02d.wav", RE15_SYNCHRO_DIR, (raum >> 12) & 0xF, raum & 0xFFFF, id);
    FILE *f = fopen(p, "rb");
    if (!f) return 0;
    uint8_t h[12]; uint32_t rate = 0, data = 0;
    if (fread(h, 1, 12, f) != 12 || memcmp(h, "RIFF", 4) || memcmp(h + 8, "WAVE", 4)) { fclose(f); return 0; }
    for (;;) {
        uint8_t c[8];
        if (fread(c, 1, 8, f) != 8) break;
        const uint32_t n = (uint32_t)c[4] | ((uint32_t)c[5] << 8) | ((uint32_t)c[6] << 16) | ((uint32_t)c[7] << 24);
        if (!memcmp(c, "fmt ", 4)) {
            uint8_t fm[16];
            if (n < 16 || fread(fm, 1, 16, f) != 16) break;
            rate = (uint32_t)fm[8] | ((uint32_t)fm[9] << 8) | ((uint32_t)fm[10] << 16) | ((uint32_t)fm[11] << 24);
            if (n > 16) fseek(f, (long)(n - 16 + (n & 1)), SEEK_CUR);
        } else if (!memcmp(c, "data", 4)) { data = n; break; }
        else fseek(f, (long)(n + (n & 1)), SEEK_CUR);
    }
    fclose(f);
    if (!rate || !data) return 0;
    return (int)(((unsigned long long)data * 30u + rate - 1) / rate);
}

static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt;
    return 0;
}

static void spielbild(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    extern int g_re15_voice_laeuft, g_re15_voice_restbilder;
    /* Plattform-Ersatz (audio_pc.c re15_audio_tick stempelt je Bild): Stimme laeuft, solange Restbilder > 0 */
    g_re15_voice_laeuft = s_stimme_rest > 0;
    g_re15_voice_restbilder = s_stimme_rest;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    if (s_stimme_rest > 0) s_stimme_rest--;
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) {
        if (e.kind == SCD_AUDIO_SEQ_CTL && s_n_musik < 64) {
            s_musik[s_n_musik].bild = s_bild; s_musik[s_n_musik].slot = e.bank; s_musik[s_n_musik].op = e.volume; s_n_musik++;
        }
        if (e.kind == SCD_AUDIO_VOICE_ON) {
            int lese = 0;
            if (re15_birkinfilm_r42_stimme(g_current_room_id, e.sample_id, &lese)) {
                s_stimme_rest = (lese + 4) / 5;    /* 150 Sektoren/s, 30 Bilder/s */
                if (s_n_stimmen < 8) s_stimmen[s_n_stimmen++] = e.sample_id;
            } else if (s_synchro && e.sample_id >= 0 && e.sample_id < 64) {
                if (s_synchro_raum != (int)g_current_room_id) {
                    s_synchro_raum = (int)g_current_room_id;
                    for (int i = 0; i < 64; i++) s_synchro_bilder[i] = synchro_bilder(g_current_room_id, i);
                }
                if (s_synchro_bilder[e.sample_id] > 0) s_stimme_rest = s_synchro_bilder[e.sample_id];
            }
        }
    }
    s_bild++;
}

static void frame(uint16_t held, uint16_t edge)
{
    if (!re15_discard_frozen()) spielbild(held, edge);
    else s_bild++;
}

static uint8_t *s_ems = NULL;
static int      s_ems_sz = 0;
static uint8_t *s_baenke[3];
/* Begleiter-Bank aus CDEMD1.EMS (Muster test_r41_zws114.c bank_john): Annette 0x4D (6000), John 0x49 / Sherry 0x4B (5011). Je Typ
 * ein eigener Puffer (die Bank zeigt in ihn). */
static void bank_typ(uint8_t typ, int platz)
{
    if (!s_ems) { size_t n = 0; s_ems = aus(RE15_ASSET_PSX_DIR, "EMD/CDEMD1.EMS", &n); s_ems_sz = (int)n; }
    if (!s_ems) { printf("  FEHLER: CDEMD1.EMS fehlt\n"); g_fail++; return; }
    int idx = re15_ems_index_for_type(typ);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, (size_t)s_ems_sz, idx, &off, &len) != 0 || len == 0) {
        printf("  FEHLER: Bank 0x%02X nicht im Archiv\n", typ); g_fail++; return; }
    re15_enemy_bank_t *eb = re15_enemy_alloc(typ);
    if (!eb) return;
    free(s_baenke[platz]); s_baenke[platz] = (uint8_t *)malloc(len);
    if (!s_baenke[platz]) return;
    uint8_t *s_bank = s_baenke[platz];
    memcpy(s_bank, s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank, len, &eb->skel_own, &eb->anim_own) == 0);
}
static void bank_annette(void) { bank_typ(0x4D, 0); }

static int s_band = 2;   /* Bodenband des Spawns: 6000 = 2 (Tuer-Ziel), 5011 = 0 (Door_aot @0x0039A floor 0) */
static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    if (room == 0x5011) { bank_typ(0x49, 1); bank_typ(0x4B, 2); }   /* main00 @0x004E0 / @0x004F4 Sce_em_set 0x49 / 0x4B */
    else bank_annette();
    re15_discard_reset();
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_room_rdt_ok = 1;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(s_band);
    s_shown = cut; s_cine_was = 0; s_bild = 0; s_n_musik = 0; s_stimme_rest = 0; s_n_stimmen = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* ---- Teil: programm ------------------------------------------------------------------------------------------------ */
static long finde(const uint8_t *b, long n, const uint8_t *m, long k, long ab)
{
    for (long i = ab; i + k <= n; i++) if (!memcmp(b + i, m, (size_t)k)) return i;
    return -1;
}

static void teil_programm(void)
{
    printf("== programm ==\n");
    size_t nr = 0;
    uint8_t *r = aus(RE15_ASSET_PSX_DIR, "STAGE6/ROOM6000.RDT", &nr);
    int np = 0;
    const uint8_t *p = re15_birkinfilm_r42_programm(&np);
    PRUEF(r && nr > 0x11A6 && p && np > 0, "ROOM6000.RDT und Programm (%d B)", np);
    if (!r || !p) { free(r); return; }
    const long vor = 0x01156 - 0x0106A, nach = 0x011A6 - 0x01156, ein = np - vor - nach;
    PRUEF(!memcmp(p, r + 0x0106A, (size_t)vor) && !memcmp(p + vor + ein, r + 0x01156, (size_t)nach),
          "sub06 bytegleich: [@0x0106A, @0x01156) vorn, [@0x01156, @0x011A6) hinten, Einschub %ld B", ein);
    PRUEF(p[0] == 0x22 && p[1] == 0x03 && p[2] == 0x64 && p[3] == 0x01, "erstes Byte = Set(3,100,1) @0x0106A (einmal je Spielstand)");
    static const uint8_t m40[] = { 0x2b, 40, 0, 0 }, m41[] = { 0x2b, 41, 0, 0 }, m42[] = { 0x2b, 42, 0, 0 },
                         m43[] = { 0x2b, 43, 0, 0 }, m44[] = { 0x2b, 44, 0, 0 }, s2[] = { 0x09, 0x0a, 0x02, 0x00 },
                         s60[] = { 0x09, 0x0a, 0x3c, 0x00 }, s63[] = { 0x09, 0x0a, 0x3f, 0x00 },
                         s41[] = { 0x09, 0x0a, 0x29, 0x00 }, s81[] = { 0x09, 0x0a, 0x51, 0x00 },
                         m48[] = { 0x2b, RE15_BF_MSG_XA09_S2, 0, 0 }, m49[] = { 0x2b, RE15_BF_MSG_XA11_S2, 0, 0 },
                         m50[] = { 0x2b, RE15_BF_MSG_XA14_S2, 0, 0 },
                         halt[] = { 0x54, 0, 4, 0, 0, 0 }, weiter[] = { 0x54, 0, 3, 0, 0, 0 }, m3[] = { 0x2b, 3, 0, 0 };
    const uint8_t *g = r + 0x01142;
    const long i40 = finde(p, np, m40, 4, vor), i41 = finde(p, np, m41, 4, vor), i43 = finde(p, np, m43, 4, vor),
               i44 = finde(p, np, m44, 4, vor), i42 = finde(p, np, m42, 4, vor), i3 = finde(p, np, m3, 4, vor),
               i48 = finde(p, np, m48, 4, vor), i49 = finde(p, np, m49, 4, vor), i50 = finde(p, np, m50, 4, vor);
    /* Strang namen: Satz 1 - RE2-Sleep - Satz 2 - Geste (RE2 ROOM4090 @0x02718/@0x02722/@0x02726, @0x027CE/@0x027D8/@0x027DC,
     * @0x02846/@0x02850/@0x02854) */
    PRUEF(i40 == vor && !memcmp(p + i40 + 4, s41, 4) && i48 == i40 + 8 && !memcmp(p + i48 + 4, g, 20) && i41 == i48 + 24 &&
          !memcmp(p + i41 + 4, s81, 4) && i49 == i41 + 8 && !memcmp(p + i49 + 4, g, 20) && i43 == i49 + 24 &&
          !memcmp(p + i43 + 4, s2, 4) && !memcmp(p + i43 + 8, s60, 4) && !memcmp(p + i43 + 12, halt, 6) && i44 == i43 + 18 &&
          !memcmp(p + i44 + 4, s63, 4) && !memcmp(p + i44 + 8, weiter, 6) && i42 == i44 + 14 && !memcmp(p + i42 + 4, s81, 4) &&
          i50 == i42 + 8 && !memcmp(p + i50 + 4, g, 20) && i3 == i50 + 24 && i3 == vor + ein,
          "Einschub: msg 40, Sleep 41 (@0x02722), msg 48 + Geste, msg 41, Sleep 81 (@0x027D8), msg 49 + Geste, Warte-Zeile 43, "
          "Sleep 2 (@0x027E4), Sleep 60 (@0x027F0), MAIN Op 4, Film 44, Sleep 63 (@0x02820), MAIN Op 3, msg 42, Sleep 81 (@0x02850), "
          "msg 50 + Geste, dann msg 3 @0x01156");
    int n54 = 0;
    for (long i = vor; i < vor + ein; i++) if (p[i] == 0x54 && (i == i43 + 12 || i == i44 + 8)) n54++;
    PRUEF(n54 == 2, "Musik nur Slot 0 (Op 4 / Op 3), Lautstaerke/Pan 0");
    /* Namenskopf aus der RDT selbst: ROOM6000 msg 1 "Annette: Where's Ada?" (@0x01287) = `04 00 05 02 <Annette:> 05 00` */
    const uint32_t mb = rd32(r + 0x3C);
    const uint16_t o1 = rd16(r + mb + 2);
    const uint8_t *kopf = r + mb + o1;
    long kn = 0;
    while (kn + 1 < 24 && !(kopf[kn] == 0x05 && kopf[kn + 1] == 0x00 && kn > 2)) kn++;
    kn += 2;
    PRUEF(mb + o1 == 0x01287 && kopf[0] == 0x04 && kopf[1] == 0x00 && kopf[2] == 0x05 && kopf[3] == 0x02 && kn == 14,
          "Namenskopf aus ROOM6000 msg 1 @0x01287: %ld B (04 00 05 02 \"Annette:\" 05 00)", kn);
    static const int ids[6] = { 40, RE15_BF_MSG_XA09_S2, 41, RE15_BF_MSG_XA11_S2, 42, RE15_BF_MSG_XA14_S2 };
    for (int k = 0; k < 6; k++) {
        const int id = ids[k];
        int n = 0;
        const uint8_t *t = re15_birkinfilm_r42_text((uint8_t)id, &n);
        const long seite = t ? finde(t, n, (const uint8_t *)"\x04\x01\x02", 3, 0) : -1;
        PRUEF(t && n > kn && !memcmp(t, kopf, (size_t)kn) && seite < 0 && t[n - 4] == 0x04 && t[n - 3] == 0x01 &&
              t[n - 2] == 0x01 && t[n - 1] == 0x63 && n <= 128,
              "msg %d (%s): eine Seite, beginnt mit \"Annette:\" wie ROOM6000 msg 1, Schwanz 04 01 01 63, %d B <= 128 (PSX)",
              id, (k & 1) ? "Satz 2" : "Satz 1", n);
    }
    PRUEF(!re15_birkinfilm_r42_stimme(RE15_BF_RAUM, RE15_BF_MSG_XA09_S2, NULL) &&
          !re15_birkinfilm_r42_stimme(RE15_BF_RAUM, RE15_BF_MSG_XA11_S2, NULL) &&
          !re15_birkinfilm_r42_stimme(RE15_BF_RAUM, RE15_BF_MSG_XA14_S2, NULL), "Satz 2 (48/49/50) ohne eigene Stimme");
    /* Haken: Umleitung nur fuer ROOM6000 sub06 mit passender Signatur */
    const uint8_t *orig = r + 0x0106A;
    PRUEF(re15_birkinfilm_r42_evt_exec(0x6000, 6, orig) == p, "Evt_exec sub06 in ROOM6000 -> Programm");
    PRUEF(re15_birkinfilm_r42_evt_exec(0x6001, 6, orig) == orig && re15_birkinfilm_r42_evt_exec(0x6000, 7, orig) == orig,
          "andere Raeume/Subs unberuehrt");
    uint8_t *falsch = (uint8_t *)malloc(0x11A6 - 0x0106A);
    if (falsch) {
        memcpy(falsch, orig, 0x11A6 - 0x0106A); falsch[0x1156 - 0x106A + 1] = 0x05;
        PRUEF(re15_birkinfilm_r42_evt_exec(0x6000, 6, falsch) == falsch, "abweichende Bytes -> Original-Ablauf");
        free(falsch);
    }
    const re15_zws_film_t *f = re15_zws_r41_film_fuer_raum(RE15_BF_RAUM);
    PRUEF(f && f->film.msg == RE15_BF_MSG_FILM && f->schrei.msg < 0 && f->schrei.n == 0 && f->sichtbar.n == 0 &&
          !strcmp(f->film_datei, RE15_BF_FILM) && f->film_bilder == RE15_BF_FILM_BILDER
#ifdef RE15_ZWS_BLENDART_4090
          && f->blende == RE15_ZWS_BLENDART_4090 && f->bild_y == RE15_BF_FILM_Y
#endif
          , "Filmtabelle: ROOM6000 Zeile 44, %s, %d Bilder, Blendenart 4090, Bild 40 Zeilen tiefer", RE15_BF_FILM,
          RE15_BF_FILM_BILDER);
    PRUEF(!re15_zws_r41_film_fuer_raum(0x6001), "ROOM6001 (Elza) ohne Film");
    free(r);
}

/* ---- Teil: ablauf -------------------------------------------------------------------------------------------------- */
#define FILM_HALTEN 30   /* so viele Bilder "laeuft" der Film im Riegel (die echte Plattform spielt ihn zwischen zwei Bildern) */

typedef struct {
    int  msgs[64]; long b_msg[64]; int n_msg;
    long b_film_an, b_film_aus; int filme;
    int  vor[80], n_vor;       /* Blendwerte der Bilder vor dem Film (ab dem ersten gezeichneten) */
    int  nach[80], n_nach;     /* Blendwerte der Bilder nach dem Film */
    int  msg42_im_film;
    /* Strang namen: jede neu gezeigte Seite (Nachricht, Seitenanfang) ab msg 40 bis vor msg 3 */
    int  sp_id[32]; long sp_bild[32]; int sp_kopf[32]; int n_s;
} lauf_t;

static uint8_t s_kopf[24];   /* Namenskopf "Annette:" aus der RDT-Zeile msg 1 (ohne `04 00`) */
static int     s_kopf_n = 0;
static int     s_seiten_an = 0;   /* 1 = Seiten mitschreiben (6000) */

static int s_folge_msg = 42;   /* die Zeile nach dem Film: 6000 msg 42, 5011 msg 4 */
static void lauf(lauf_t *L, int bilder)
{
    memset(L, 0, sizeof *L);
    L->b_film_an = L->b_film_aus = -1;
    int last = -1, halte = 0;
    for (int f = 0; f < bilder; f++) {
        /* Plattform-Ersatz Film: zu Beginn des Bilds, wie re15_zws_r41_pc_bild vor re15_render_begin_frame */
        if (re15_zws_r41_film_offen()) {
            if (L->b_film_an < 0) { L->b_film_an = s_bild; L->filme++; }
            if (++halte >= FILM_HALTEN) { re15_zws_r41_film_gespielt(); L->b_film_aus = s_bild; halte = 0; }
            continue;   /* waehrend des Films kein Spielbild (der Spieler blockiert die Hauptschleife) */
        }
        frame(0, 0);
        if (g_scd.message_active && (int)g_scd.message_id != last) {
            last = (int)g_scd.message_id;
            if (L->n_msg < 64) { L->msgs[L->n_msg] = last; L->b_msg[L->n_msg] = s_bild; L->n_msg++; }
        }
        if (!g_scd.message_active) last = -1;
        re15_fade_tick();   /* wie re15_render_end_frame (render_pc.c) je gezeichnetem Bild */
        const re15_fade_ch_t *c = &g_fade_ch[RE15_ZWS_BLENDE_KANAL];
        const int v = c->drawn ? c->out_r : -1;
        if (L->b_film_an < 0 && (v >= 0 || L->n_vor > 0) && L->n_vor < 80) L->vor[L->n_vor++] = v;
        if (L->b_film_aus >= 0 && L->n_nach < 80) L->nach[L->n_nach++] = v;
        if (L->b_film_aus < 0 && L->b_film_an >= 0 && g_scd.message_active && g_scd.message_id == s_folge_msg) L->msg42_im_film = 1;
        /* Seiten: neue (Nachricht, Seitenanfang) mit sichtbarem Fenster [page, parse) */
        {   static int s_last_id = -1, s_last_page = -1;
            if (f == 0) { s_last_id = -1; s_last_page = -1; }
            const int id = (int)g_scd.message_id, pg = (int)g_scd.message_page;
            if (s_seiten_an && g_scd.message_active && g_scd.message_fsm_active && g_scd.message_parse > g_scd.message_page &&
                (id != s_last_id || pg != s_last_page)) {
                s_last_id = id; s_last_page = pg;
                const int annette = (id >= 40 && id <= 42) || (id >= 48 && id <= 50);
                if (annette && L->n_s < 32) {
                    int rl = 0;
                    const unsigned char *raw = re15_msg_get_raw(id, &rl);
                    int k = 0;
                    if (raw && pg + 2 + s_kopf_n <= rl && raw[pg] == 0x04 && raw[pg + 1] == 0x00 &&
                        !memcmp(raw + pg + 2, s_kopf, (size_t)s_kopf_n)) k = 1;
                    L->sp_id[L->n_s] = id; L->sp_bild[L->n_s] = s_bild; L->sp_kopf[L->n_s] = k; L->n_s++;
                }
            }
        }
    }
}

static int idx(const lauf_t *L, int m) { for (int i = 0; i < L->n_msg; i++) if (L->msgs[i] == m) return i; return -1; }

static void teil_ablauf(void)
{
    printf("== ablauf ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_fade_init();
    re15_zws_r41_film_spieler = 1;
    /* Ankunft am Bahnsteig wie Leon aus 5080 (Tuer-Ziel; Lage aus re15_room_spawns.h ROOM6000 (7186,-9289)) */
    if (room_boot(0x6000, 7186, -9289, 0, 0) != 0) return;
    PRUEF(g_re15_bf_umleitungen == 1, "sub00 @0x00F58 Evt_exec sub06 -> Programm (%u Umleitung)", g_re15_bf_umleitungen);
    {   /* Namenskopf aus der RDT-Zeile msg 1 (@0x01287 "Annette: Where's Ada?"): `05 02 <Annette:> 05 00` */
        int rl = 0;
        const unsigned char *k = re15_msg_get_raw(1, &rl);
        s_kopf_n = 0;
        if (k && rl > 4) {
            int e = 2;
            while (e + 1 < rl && e < 22 && !(k[e] == 0x05 && k[e + 1] == 0x00 && e > 2)) e++;
            s_kopf_n = e + 2 - 2;
            memcpy(s_kopf, k + 2, (size_t)s_kopf_n);
        }
        PRUEF(s_kopf_n == 12 && s_kopf[0] == 0x05 && s_kopf[1] == 0x02, "Namenskopf aus msg 1: %d B (05 02 \"Annette:\" 05 00)",
              s_kopf_n);
    }
    lauf_t L;
    s_seiten_an = 1;
    lauf(&L, 9000);
    s_seiten_an = 0;
    printf("    Nachrichten:"); for (int i = 0; i < L.n_msg; i++) printf(" %d@%ld", L.msgs[i], L.b_msg[i]); printf("\n");
    printf("    Film an Bild %ld, aus %ld (%d mal); Stimmen:", L.b_film_an, L.b_film_aus, L.filme);
    for (int i = 0; i < s_n_stimmen; i++) printf(" %d", s_stimmen[i]);
    printf("; Musik:"); for (int i = 0; i < s_n_musik; i++) printf(" (%d,%d)@%ld", s_musik[i].slot, s_musik[i].op, s_musik[i].bild);
    printf("\n    Blende vor dem Film (%d):", L.n_vor); for (int i = 0; i < L.n_vor; i++) printf(" %d", L.vor[i]);
    printf("\n    Blende nach dem Film (%d):", L.n_nach); for (int i = 0; i < L.n_nach && i < 70; i++) printf(" %d", L.nach[i]);
    printf("\n");
    const int i0 = idx(&L, 0), i1 = idx(&L, 1), i2 = idx(&L, 2), i40 = idx(&L, 40), i41 = idx(&L, 41), i42 = idx(&L, 42),
              i3 = idx(&L, 3), i48 = idx(&L, RE15_BF_MSG_XA09_S2), i49 = idx(&L, RE15_BF_MSG_XA11_S2),
              i50 = idx(&L, RE15_BF_MSG_XA14_S2);
    PRUEF(i0 >= 0 && i1 == i0 + 1 && i2 == i1 + 1 && i40 == i2 + 1 && i48 == i40 + 1 && i41 == i48 + 1 && i49 == i41 + 1 &&
          i42 == i49 + 1 && i50 == i42 + 1 && i3 == i50 + 1,
          "Zeilenfolge 0, 1, 2 (\"I see...\"), 40, 48, 41, 49, 42, 50, 3 (\"Come on...\") ohne Zwischenzeile (43/44 ohne Text)");
    /* Strang namen: Satz 2 nach dem RE2-Sleep (41 @0x02722, 81 @0x027D8, 81 @0x02850), waehrend die Stimme weiterspricht */
    {   const long d48 = (i40 >= 0 && i48 >= 0) ? L.b_msg[i48] - L.b_msg[i40] : -1,
                   d49 = (i41 >= 0 && i49 >= 0) ? L.b_msg[i49] - L.b_msg[i41] : -1,
                   d50 = (i42 >= 0 && i50 >= 0) ? L.b_msg[i50] - L.b_msg[i42] : -1;
        PRUEF(d48 >= 41 && d48 <= 42 && d49 >= 81 && d49 <= 82 && d50 >= 81 && d50 <= 82,
              "Satz 2 nach dem RE2-Sleep: msg 48 %ld Bilder nach 40 (Sleep 41 @0x02722), 49 %ld nach 41 (Sleep 81 @0x027D8), "
              "50 %ld nach 42 (Sleep 81 @0x02850)", d48, d49, d50);
        const long st40 = (RE15_BF_XA09_LESE + 4) / 5, st41 = (RE15_BF_XA11_LESE + 4) / 5, st42 = (RE15_BF_XA14_LESE + 4) / 5;
        PRUEF(d48 < st40 && d49 < st41 && d50 < st42,
              "Satz 2 erscheint, waehrend die Stimme spricht (Stimme %ld/%ld/%ld Bilder) - der Stimmen-Riegel haelt ihn nicht auf",
              st40, st41, st42);
    }
    {   int mit = 0;
        printf("    Annette-Seiten:");
        for (int i = 0; i < L.n_s; i++) { printf(" %d@%ld%s", L.sp_id[i], L.sp_bild[i], L.sp_kopf[i] ? "" : "(OHNE NAMEN)"); mit += L.sp_kopf[i]; }
        printf("\n");
        PRUEF(L.n_s == 6 && mit == L.n_s,
              "jede gezeigte Annette-Seite beginnt mit \"Annette:\" (Seiten %d, mit Namen %d; Strang namen, Nutzer 18.11.)", L.n_s, mit);
    }
    PRUEF(s_n_stimmen == 3 && s_stimmen[0] == 40 && s_stimmen[1] == 41 && s_stimmen[2] == 42,
          "RE2-Stimmen ueber den Sprachkanal: msg 40, 41, 42 (Xa 0x09, 0x11, 0x14), keine fuer Satz 2");
    PRUEF(L.filme == 1 && L.b_film_an > 0 && i41 >= 0 && i42 >= 0 && L.b_film_an > L.b_msg[i41] && L.b_film_an < L.b_msg[i42],
          "Film genau einmal, zwischen msg 41 (Bild %ld) und msg 42 (Bild %ld): Bild %ld", i41 >= 0 ? L.b_msg[i41] : -1L,
          i42 >= 0 ? L.b_msg[i42] : -1L, L.b_film_an);
    const long stimme41 = (RE15_BF_XA11_LESE + 4) / 5;
    PRUEF(i41 >= 0 && L.b_film_an - L.b_msg[i41] >= stimme41 + 2 + 60 + 64,
          "Film erst nach Stimme 41 (%ld Bilder) + Sleep 2 + Sleep 60 + 64 Abblendbildern: %ld Bilder nach msg 41", stimme41,
          i41 >= 0 ? L.b_film_an - L.b_msg[i41] : -1L);
    {   int ok = L.n_vor == 65;
        for (int i = 0; ok && i < 64; i++) if (L.vor[i] != 4 * i) ok = 0;
        if (ok && L.vor[64] != 252) ok = 0;
        PRUEF(ok, "Abblende wie RE2 ROOM4090 @0x027F4: %d Bilder 0, 4 ... 248, 252 und im Bild der Anforderung 252 (@0x02812) "
                  "- gelesen %d, %d ... %d, %d", L.n_vor, L.n_vor > 0 ? L.vor[0] : -9, L.n_vor > 1 ? L.vor[1] : -9,
              L.n_vor > 63 ? L.vor[63] : -9, L.n_vor > 64 ? L.vor[64] : -9);
    }
    {   int ok = L.n_nach >= 66;
        for (int i = 0; ok && i < 64; i++) if (L.nach[i] != 255 - 4 * i) ok = 0;
        if (ok && (L.nach[64] != -1 || L.nach[65] != -1)) ok = 0;
        PRUEF(ok, "Einblende wie RE2 @0x0281C (Pegel 0x7FFF): 255, 251 ... 3 (64 Bilder), dann Halt - gelesen %d, %d ... %d, %d",
              L.n_nach > 0 ? L.nach[0] : -9, L.n_nach > 1 ? L.nach[1] : -9, L.n_nach > 63 ? L.nach[63] : -9,
              L.n_nach > 64 ? L.nach[64] : -9);
    }
    PRUEF(!L.msg42_im_film && i42 >= 0 && L.b_msg[i42] - L.b_film_aus >= 63 && L.b_msg[i42] - L.b_film_aus <= 65,
          "msg 42 nach Sleep 63 @0x02820: %ld Bilder nach dem Film", i42 >= 0 ? L.b_msg[i42] - L.b_film_aus : -1L);
    int halt = -1, weiter = -1, andere = 0;
    for (int i = 0; i < s_n_musik; i++) {
        if (s_musik[i].slot == 0 && s_musik[i].op == 4 && halt < 0) halt = i;
        else if (s_musik[i].slot == 0 && s_musik[i].op == 3 && weiter < 0) weiter = i;
        else if (s_musik[i].bild > (i40 >= 0 ? L.b_msg[i40] : 0)) andere++;
    }
    PRUEF(halt >= 0 && weiter >= 0 && s_musik[halt].bild <= L.b_film_an - 64 && s_musik[weiter].bild >= L.b_film_aus + 62 &&
          andere == 0, "MAIN Op 4 im Bild der Abblende (%ld), Op 3 nach Sleep 63 (%ld), sonst kein Musikbefehl im Einschub",
          halt >= 0 ? s_musik[halt].bild : -1L, weiter >= 0 ? s_musik[weiter].bild : -1L);
    PRUEF(re15_game_flag_get(RE15_BF_SZENE_BANK, RE15_BF_SZENE_BIT), "(3,100) gesetzt (Szene gesehen)");
    PRUEF(i3 >= 0 && L.b_msg[i3] > L.b_msg[i42], "danach weiter wie im Original: msg 3 \"Come on...\" (Bild %ld)",
          i3 >= 0 ? L.b_msg[i3] : -1L);
}

/* ---- Teil: einmal -------------------------------------------------------------------------------------------------- */
static void teil_einmal(void)
{
    printf("== einmal ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_fade_init();
    re15_zws_r41_film_spieler = 1;
    if (room_boot(0x6000, 7186, -9289, 0, 0) != 0) return;
    lauf_t L;
    lauf(&L, 9000);
    PRUEF(L.filme == 1, "erstes Betreten: Film %d mal", L.filme);
    const unsigned u = g_re15_bf_umleitungen;
    if (room_boot(0x6000, 7186, -9289, 0, 0) != 0) return;   /* (3,100) bleibt gesetzt (g_game uebernommen) */
    lauf(&L, 3000);
    PRUEF(L.filme == 0 && g_re15_bf_umleitungen == u && idx(&L, 40) < 0,
          "zweites Betreten mit (3,100)=1: keine Szene, kein Film (%d), keine Umleitung", L.filme);
}

/* ==== Stufe 2: Elza ROOM5011 ======================================================================================== */
#define E_VOR   (0x00690 - 0x0055A)   /* sub02 [@0x0055A, @0x00690) */
#define E_NACH  (0x0093A - 0x00690)   /* sub02 [@0x00690, @0x0093A) */

static void teil_programm5011(void)
{
    printf("== programm5011 ==\n");
    size_t nr = 0;
    uint8_t *r = aus(RE15_ASSET_PSX_DIR, "STAGE5/ROOM5011.RDT", &nr);
    int np = 0;
    const uint8_t *p = re15_birkinfilm_r42_programm_raum(RE15_BF_RAUM_ELZA, &np);
    PRUEF(r && nr > 0x0093A && p && np > 0, "ROOM5011.RDT und Programm (%d B)", np);
    if (!r || !p) { free(r); return; }
    const long ein = np - E_VOR - E_NACH;
    PRUEF(r[0x00524 + 4] == 0x36 && r[0x00524 + 5] == 0x00 && r[0x00524 + 6] == 0x16 && r[0x00524 + 7] == 0x04,
          "Zeigertabelle sub_scd @0x00524: sub02 = @0x0055A, sub03 = @0x0093A");
    PRUEF(!memcmp(r + 0x0053A, "\x21\x04\xed\x01\x04\xff\x18\x02", 8),
          "sub00 @0x0053A Ck(4,237)=1 -> @0x0053E Evt_exec sub02");
    PRUEF(!memcmp(p, r + 0x0055A, (size_t)E_VOR) && !memcmp(p + E_VOR + ein, r + 0x00690, (size_t)E_NACH),
          "sub02 bytegleich: [@0x0055A, @0x00690) vorn, [@0x00690, @0x0093A) hinten, Einschub %ld B", ein);
    PRUEF(p[0] == 0x22 && p[1] == 0x04 && p[2] == 0xed && p[3] == 0x00, "erstes Byte = Set(4,237,0) @0x0055A (einmal je Spielstand)");
    PRUEF(!memcmp(r + 0x00640, "\x2b\x03\x00\x00", 4) && !memcmp(r + 0x00690, "\x2b\x04\x00\x00", 4),
          "msg 3 @0x00640, msg 4 @0x00690 (Anker)");
    static const uint8_t soll[] = { 0x2b, 46, 0, 0,  0x09, 0x0a, 0x02, 0x00,  0x09, 0x0a, 0x3c, 0x00,  0x54, 0, 4, 0, 0, 0,
                                    0x2b, 47, 0, 0,  0x09, 0x0a, 0x3f, 0x00,  0x54, 0, 3, 0, 0, 0 };
    PRUEF(ein == (long)sizeof soll && !memcmp(p + E_VOR, soll, sizeof soll),
          "Einschub vor msg 4: Warte-Zeile 46, Sleep 2 (@0x027E4), Sleep 60 (@0x027F0), MAIN Op 4 (@0x02804), Film 47, "
          "Sleep 63 (@0x02820), MAIN Op 3 (@0x0282B) - ohne neue Zeilen");
    int n2b = 0;
    for (long i = 0; i < (long)sizeof soll; i += (soll[i] == 0x54 ? 6 : 4)) if (soll[i] == 0x2b) n2b++;
    PRUEF(n2b == 2 && RE15_BF_ELZA_MSG_WARTEN == 46 && RE15_BF_ELZA_MSG_FILM == 47, "nur zwei Port-Zeilen ohne Text (46, 47)");
    /* Haken: Umleitung nur fuer ROOM5011 sub02 mit passender Signatur */
    const uint8_t *orig = r + 0x0055A;
    const unsigned u0 = g_re15_bf_umleitungen;
    PRUEF(re15_birkinfilm_r42_evt_exec(0x5011, 2, orig) == p && g_re15_bf_umleitungen == u0 + 1, "Evt_exec sub02 in ROOM5011 -> Programm");
    PRUEF(re15_birkinfilm_r42_evt_exec(0x5010, 2, orig) == orig && re15_birkinfilm_r42_evt_exec(0x5011, 4, orig) == orig &&
          re15_birkinfilm_r42_evt_exec(0x5011, 6, orig) == orig, "andere Raeume/Subs unberuehrt (5010 sub02, 5011 sub04/sub06)");
    uint8_t *falsch = (uint8_t *)malloc(0x0093A - 0x0055A);
    if (falsch) {
        memcpy(falsch, orig, 0x0093A - 0x0055A); falsch[0x00690 - 0x0055A + 1] = 0x05;
        PRUEF(re15_birkinfilm_r42_evt_exec(0x5011, 2, falsch) == falsch, "abweichende Bytes -> Original-Ablauf");
        free(falsch);
    }
    const re15_zws_film_t *f = re15_zws_r41_film_fuer_raum(RE15_BF_RAUM_ELZA);
    PRUEF(f && f->film.msg == RE15_BF_ELZA_MSG_FILM && f->schrei.msg < 0 && f->schrei.n == 0 && f->sichtbar.n == 0 &&
          !strcmp(f->film_datei, RE15_BF_FILM) && f->film_bilder == RE15_BF_FILM_BILDER
#ifdef RE15_ZWS_BLENDART_4090
          && f->blende == RE15_ZWS_BLENDART_4090 && f->bild_y == RE15_BF_FILM_Y
#endif
          , "Filmtabelle: ROOM5011 Zeile 47, %s, %d Bilder, Blendenart 4090, Bild 40 Zeilen tiefer", RE15_BF_FILM,
          RE15_BF_FILM_BILDER);
    PRUEF(!re15_zws_r41_film_fuer_raum(0x5010), "ROOM5010 ohne Film");
    PRUEF(!re15_birkinfilm_r42_stimme(RE15_BF_RAUM_ELZA, 3, NULL) && !re15_birkinfilm_r42_stimme(RE15_BF_RAUM_ELZA, 46, NULL) &&
          !re15_birkinfilm_r42_stimme(RE15_BF_RAUM_ELZA, 47, NULL), "ROOM5011: keine RE2-Stimme (Elzas Zeilen bleiben synchro)");
    free(r);
}

/* Ankunft in 5011 wie nach ROOM5021 sub02: (4,237)=1 (@0x01356) und (3,58)=1 (@0x0135A, main00 @0x004DC setzt dann John/Sherry).
 * Lage = Tuer-Ziel aus 5000 (re15_room_spawns.h ROOM5011 (-23636,-18240) yaw 456), Band 0 (Door_aot @0x0039A floor 0). */
static int boot5011(void)
{
    s_band = 0;
    s_synchro = 1; s_synchro_raum = -1;
    s_folge_msg = 4;
    return room_boot(0x5011, -23636, -18240, 456, 0);
}

static void teil_ablauf5011(void)
{
    printf("== ablauf5011 ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_fade_init();
    re15_zws_r41_film_spieler = 1;
    re15_game_flag_set(4, 237, 1);
    re15_game_flag_set(3, 58, 1);
    const unsigned u0 = g_re15_bf_umleitungen;
    if (boot5011() != 0) return;
    PRUEF(g_re15_bf_umleitungen == u0 + 1, "sub00 @0x0053E Evt_exec sub02 -> Programm (%u Umleitung)", g_re15_bf_umleitungen - u0);
    const int st3 = synchro_bilder(0x5011, 3);
    PRUEF(st3 > 0, "synchro/STAGE5/room5011/main03.wav: %d Bilder (Stimme von msg 3)", st3);
    lauf_t L;
    lauf(&L, 12000);
    printf("    Nachrichten:"); for (int i = 0; i < L.n_msg; i++) printf(" %d@%ld", L.msgs[i], L.b_msg[i]); printf("\n");
    printf("    Film an Bild %ld, aus %ld (%d mal); Warte-Zeile %u Takte auf den Untertitel; Musik:", L.b_film_an, L.b_film_aus,
           L.filme, g_re15_bf_warten);
    for (int i = 0; i < s_n_musik; i++) printf(" (%d,%d)@%ld", s_musik[i].slot, s_musik[i].op, s_musik[i].bild);
    printf("\n    Blende vor dem Film (%d):", L.n_vor); for (int i = 0; i < L.n_vor; i++) printf(" %d", L.vor[i]);
    printf("\n    Blende nach dem Film (%d):", L.n_nach); for (int i = 0; i < L.n_nach && i < 70; i++) printf(" %d", L.nach[i]);
    printf("\n");
    int folge = L.n_msg >= 17;
    for (int i = 0; folge && i < 17; i++) if (L.msgs[i] != i) folge = 0;
    PRUEF(folge, "Zeilenfolge 0..16 wie im Original, keine neue Zeile (46/47 ohne Text)");
    const int i3 = idx(&L, 3), i4 = idx(&L, 4);
    PRUEF(idx(&L, 46) < 0 && idx(&L, 47) < 0 && s_n_stimmen == 0, "keine Zeile 46/47 sichtbar, keine RE2-Stimme");
    PRUEF(L.filme == 1 && i3 >= 0 && i4 >= 0 && L.b_film_an > L.b_msg[i3] && L.b_film_an < L.b_msg[i4],
          "Film genau einmal, zwischen msg 3 (Bild %ld) und msg 4 (Bild %ld): Bild %ld", i3 >= 0 ? L.b_msg[i3] : -1L,
          i4 >= 0 ? L.b_msg[i4] : -1L, L.b_film_an);
    PRUEF(i3 >= 0 && L.b_film_an - L.b_msg[i3] >= st3 + 2 + 60 + 64,
          "Film erst nach Stimme 3 (%d Bilder) + Sleep 2 + Sleep 60 + 64 Abblendbildern: %ld Bilder nach msg 3", st3,
          i3 >= 0 ? L.b_film_an - L.b_msg[i3] : -1L);
    {   int ok = L.n_vor == 65;
        for (int i = 0; ok && i < 64; i++) if (L.vor[i] != 4 * i) ok = 0;
        if (ok && L.vor[64] != 252) ok = 0;
        PRUEF(ok, "Abblende wie RE2 ROOM4090 @0x027F4: %d Bilder 0, 4 ... 248, 252 und im Bild der Anforderung 252 (@0x02812) "
                  "- gelesen %d, %d ... %d, %d", L.n_vor, L.n_vor > 0 ? L.vor[0] : -9, L.n_vor > 1 ? L.vor[1] : -9,
              L.n_vor > 63 ? L.vor[63] : -9, L.n_vor > 64 ? L.vor[64] : -9);
    }
    {   int ok = L.n_nach >= 66;
        for (int i = 0; ok && i < 64; i++) if (L.nach[i] != 255 - 4 * i) ok = 0;
        if (ok && (L.nach[64] != -1 || L.nach[65] != -1)) ok = 0;
        PRUEF(ok, "Einblende wie RE2 @0x0281C (Pegel 0x7FFF): 255, 251 ... 3 (64 Bilder), dann Halt - gelesen %d, %d ... %d, %d",
              L.n_nach > 0 ? L.nach[0] : -9, L.n_nach > 1 ? L.nach[1] : -9, L.n_nach > 63 ? L.nach[63] : -9,
              L.n_nach > 64 ? L.nach[64] : -9);
    }
    PRUEF(!L.msg42_im_film && i4 >= 0 && L.b_msg[i4] - L.b_film_aus >= 63 && L.b_msg[i4] - L.b_film_aus <= 65,
          "msg 4 nach Sleep 63 @0x02820: %ld Bilder nach dem Film", i4 >= 0 ? L.b_msg[i4] - L.b_film_aus : -1L);
    int halt = -1, weiter = -1, andere = 0;
    for (int i = 0; i < s_n_musik; i++) {
        if (s_musik[i].slot == 0 && s_musik[i].op == 4 && halt < 0) halt = i;
        else if (s_musik[i].slot == 0 && s_musik[i].op == 3 && weiter < 0) weiter = i;
        else andere++;
    }
    PRUEF(halt >= 0 && weiter >= 0 && s_musik[halt].bild <= L.b_film_an - 64 && s_musik[weiter].bild >= L.b_film_aus + 62 &&
          andere == 0, "MAIN Op 4 im Bild der Abblende (%ld), Op 3 nach Sleep 63 (%ld), sonst kein Musikbefehl",
          halt >= 0 ? s_musik[halt].bild : -1L, weiter >= 0 ? s_musik[weiter].bild : -1L);
    PRUEF(!re15_game_flag_get(RE15_BF_ELZA_BANK, RE15_BF_ELZA_BIT), "(4,237) = 0 (sub02 @0x0055A, Szene gesehen)");
    PRUEF(!re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27),
          "Szene zu Ende wie im Original: (2,7) und (1,27) wieder 0 (sub02 @0x00930 / @0x00934)");
}

static void teil_einmal5011(void)
{
    printf("== einmal5011 ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_fade_init();
    re15_zws_r41_film_spieler = 1;
    re15_game_flag_set(4, 237, 1);
    re15_game_flag_set(3, 58, 1);
    if (boot5011() != 0) return;
    lauf_t L;
    lauf(&L, 12000);
    PRUEF(L.filme == 1, "erstes Betreten: Film %d mal", L.filme);
    const unsigned u = g_re15_bf_umleitungen;
    if (boot5011() != 0) return;   /* (4,237) ist jetzt 0 (g_game uebernommen) */
    lauf(&L, 3000);
    PRUEF(L.filme == 0 && g_re15_bf_umleitungen == u && idx(&L, 3) < 0,
          "zweites Betreten mit (4,237)=0: keine Szene, kein Film (%d), keine Umleitung", L.filme);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    const int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "datei"))    teil_datei();
    if (alle || !strcmp(teil, "xa"))       teil_xa();
    if (alle || !strcmp(teil, "programm")) teil_programm();
    if (alle || !strcmp(teil, "ablauf"))   teil_ablauf();
    if (alle || !strcmp(teil, "einmal"))   teil_einmal();
    if (alle || !strcmp(teil, "programm5011")) teil_programm5011();
    if (alle || !strcmp(teil, "ablauf5011"))   teil_ablauf5011();
    if (alle || !strcmp(teil, "einmal5011"))   teil_einmal5011();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
