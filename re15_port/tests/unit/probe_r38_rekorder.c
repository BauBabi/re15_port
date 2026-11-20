/* probe_r38_rekorder.c — Runde 38 Spur J "rekorder" (analysis/befunde_runde38/J_rekorder.md).
 *
 * Nutzer (AUFTRAG.md Runde 38): "Nutze beim Rekorder Rätsel beim reinsprechen den Sound Irons_Rekorder_Sound.wav aus dem
 * synchro Ordner, und nachdem er fertig abgespielt ist, machst du den Piep Sound für Erfolgreiche Eingabe und den Tür
 * öffnen Sound. Das zusammen mit der erfolgreich message auf den Panel."
 *
 * Teile (je ein add_test, probes/r38_rekorder.cmake):
 *   anker   Original-Bytes: ROOM1100.RDT == ROOM1101.RDT, sub02 @0x00C90..0x00CC7 (Message_on 4, Evt_next, Cut 12/11
 *           blinkend, Message_on 2, Set(2,7,0), Cut 6, Cut_auto 1), sub01 @0x00C2C (Mikrofon -> sub02), sub08 @0x00D82
 *           (SYSTEM READY / SAY HELLO), @0x00CC8 Aot_reset Platz 1 sce 2; RE2 ROOM2110 sub08 (@0x01B5C Piep, @0x01B68/
 *           @0x01B6C Anzeige im selben Takt, @0x01B8C Sleep 15, @0x01B90 Aufschliessen); Runde-36-Stellen 1100/2 und 1101/2
 *           = KARTE_AUF (Piep +0, Aufschliessen +15); Port-Konstanten == Bytes; Anker nur in 1100/1101 mit Signatur.
 *   datei   die Nutzer-Datei: liegt unter synchro/STAGE1/room1100/, PCM 16 bit mono 32000 Hz, 84707 Samples = 79,4
 *           Spielbilder; die Bindung (Raum 1100/1101, Id 40) zeigt genau auf sie, sonst nichts.
 *   ablauf  ECHTER Skriptweg headless (Panel-AOT Platz 2 -> sub07, Ja, Cursor auf ON/OFF -> SAY HELLO, Cursor auf das
 *           Mikrofon -> sub02) mit nachgebildetem Stimmen-Kanal der Plattform (audio_pc.c: erst stempeln, dann mischen,
 *           dann Warteschlange; Clip-Laenge aus der echten WAV):
 *             A Leon, Nachrichten langsam: Aufnahme im Takt von Message_on 4; Piep im Takt des ersten ACCEPTED (Cut 11),
 *               nach dem Ende der Aufnahme; Aufschliessen +15; genau 2 Toene; msg 2 danach ohne Ton; Tuer frei (Platz 1
 *               Tuer, Feuer -> ROOM1110).
 *             B Leon, Nachrichten so schnell wie moeglich: sub02 WARTET an @0x00C96, Piep erst nach dem Ende der Aufnahme.
 *             C Elza ROOM1101 schnell: dasselbe.
 *             D Gegenprobe ohne Haken (= Stand vorher): keine Aufnahme, Piep erst an Message_on 2.
 *             E ohne Ton (Plattform meldet nichts): kein Haenger.
 *   piepmass  das Piep-Mass des Werkzeugs cap an einem kuenstlichen Mitschnitt: Key-On bei zehn Musikpegeln im selben
 *           Tick, Gegenprobe mit dem frueheren Mass (Strang ton Stufe 1 Runde 3)
 *   Werkzeug (fuer integration_r38_rekorder):
 *     cap  <cap.raw> <wav> [von bis]   Aufnahme im RE15_AUDIO_CAP_SYNC-Mitschnitt finden (Anfang/Ende in Ticks),
 *                                      Key-On des Pieps per Kreuzkorrelation mit dem Satz der Mini-Bank
 *                                      (Strang ton Stufe 1 Runde 3; vorher Bandanteil 1350..1550 Hz > 0,3 - das
 *                                      hing vom Pegel der Musik darunter ab, s. Kopf ueber piep_bezug_t)
 *     bild <ppm> <ROOM110.BSS>          naechster Panel-Cut (8 SAY HELLO / 11 ACCEPTED / 12 leer) im Anzeigefeld
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
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_engine.h"
#include "re15_bss.h"
#include "re15_item_discard.h"
#include "re15_aufschliessen_r36.h"
#include "re15_rekorder_r38.h"
#include "re15_keyitems_r41.h"    /* Runde 41 Spur O: das Panel startet per USE des Minidisc-Spielers */
#include "re15_vab.h"             /* Strang ton Stufe 1 Runde 3: Piep-Bezug aus der Mini-Bank (Werkzeug cap, Teil piepmass) */
#include "re15_svm_vol.h"
#include "gen/re2_karten_bank.inc"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_RE2_RDT_DIR
#define RE15_RE2_RDT_DIR "../info/re2leon/PL0/RDT"
#endif
#ifndef RE15_REPO_DIR
#define RE15_REPO_DIR ".."
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

extern scd_vm_t g_scd;
extern int g_re15_voice_laeuft, g_re15_voice_restbilder;

static int g_fail = 0;
#define CHECK(name, cond) do {                                          \
        int _c = (cond);                                                \
        printf("  [%s] %s\n", _c ? "OK  " : "FAIL", (name));            \
        if (!_c) g_fail++;                                              \
    } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b && n) *n = (size_t)sz; return b;
}

static int bytes_gleich(const uint8_t *d, size_t n, size_t off, const uint8_t *soll, size_t len)
{
    return d && off + len <= n && memcmp(d + off, soll, len) == 0;
}

/* ==================== WAV ================================================================================ */
typedef struct { int tag, ch, rate, bits; long data_off, data_len; } wav_t;

static int wav_kopf(const uint8_t *b, size_t n, wav_t *w)
{
    memset(w, 0, sizeof *w);
    if (!b || n < 44 || memcmp(b, "RIFF", 4) || memcmp(b + 8, "WAVE", 4)) return 0;
    size_t o = 12;
    while (o + 8 <= n) {
        uint32_t sz = (uint32_t)b[o + 4] | ((uint32_t)b[o + 5] << 8) | ((uint32_t)b[o + 6] << 16) | ((uint32_t)b[o + 7] << 24);
        if (!memcmp(b + o, "fmt ", 4)) {
            w->tag = b[o + 8] | (b[o + 9] << 8);  w->ch = b[o + 10] | (b[o + 11] << 8);
            w->rate = (int)((uint32_t)b[o + 12] | ((uint32_t)b[o + 13] << 8) | ((uint32_t)b[o + 14] << 16) | ((uint32_t)b[o + 15] << 24));
            w->bits = b[o + 22] | (b[o + 23] << 8);
        } else if (!memcmp(b + o, "data", 4)) {
            w->data_off = (long)(o + 8); w->data_len = (long)sz; return 1;
        }
        o += 8 + sz + (sz & 1);
    }
    return 0;
}

/* Linear auf 44100 Hz wie audio_pc.c re15_voice_load_clip (Q16-Quellposition), Mono. */
static float *wav_44100(const uint8_t *b, const wav_t *w, long *n_out)
{
    long src_n = w->data_len / 2 / w->ch;
    long out_n = (long)((long long)src_n * 44100 / w->rate);
    float *o = (float *)malloc((size_t)out_n * sizeof(float));
    if (!o) return NULL;
    const uint8_t *d = b + w->data_off;
    for (long i = 0; i < out_n; i++) {
        long long sp = (long long)i * w->rate * 65536 / 44100;
        long si = (long)(sp >> 16); int fr = (int)(sp & 0xffff);
        long s1 = (si + 1 < src_n) ? si + 1 : si;
        int a = 0, c = 0;
        for (int k = 0; k < w->ch; k++) {
            a += (int16_t)(d[(si * w->ch + k) * 2] | (d[(si * w->ch + k) * 2 + 1] << 8));
            c += (int16_t)(d[(s1 * w->ch + k) * 2] | (d[(s1 * w->ch + k) * 2 + 1] << 8));
        }
        a /= w->ch; c /= w->ch;
        o[i] = (float)(a + (((c - a) * fr) >> 16));
    }
    *n_out = out_n;
    return o;
}

static char s_wav_pfad[700];
static long s_clip_len = 0;      /* Samples @44100 (wie der Lader) */

static int wav_laden(void)
{
    snprintf(s_wav_pfad, sizeof s_wav_pfad, "%s/" RE15_REKORDER_ORDNER RE15_REKORDER_STAMM ".wav", RE15_REPO_DIR);
    size_t n = 0; uint8_t *b = slurp(s_wav_pfad, &n);
    wav_t w;
    if (!b || !wav_kopf(b, n, &w)) { free(b); return 0; }
    s_clip_len = (long)((long long)(w.data_len / 2 / w.ch) * 44100 / w.rate);   /* long ist auf Windows 32 bit */
    free(b);
    return 1;
}

/* ==================== Spielbild wie die PC-Hauptschleife ================================================ */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;
static unsigned           s_bild = 0;

/* Nachgebildeter Stimmen-Kanal der Plattform (audio_pc.c re15_audio_tick, RE15_AUDIO_CAP_SYNC-Reihenfolge):
 * (1) g_re15_voice_laeuft / g_re15_voice_restbilder aus dem Kanal stempeln, (2) ein Bild mischen (1470 Samples),
 * (3) Warteschlange: VOICE_ON -> re15_voice_play -> Lader (re15_rekorder_r38_stimme setzt die Datei ein). */
static int  s_ton_aus = 0;                 /* Teil E: Plattform ohne Ton (Kanal startet nie) */
static int  s_clip_aktiv = 0;
static long s_clip_pos = 0;
static int  s_voice40_bild = -1, s_voice40_n = 0, s_clip_ende_bild = -1;
static int  s_voice40_pfad_ok = 0;

static void plattform_ton(void)
{
    g_re15_voice_laeuft     = s_clip_aktiv;
    g_re15_voice_restbilder = s_clip_aktiv ? (int)((long long)(s_clip_len - s_clip_pos) * 30 / 44100) : 0;
    if (s_clip_aktiv) {
        s_clip_pos += 1470;
        if (s_clip_pos >= s_clip_len) { s_clip_aktiv = 0; s_clip_ende_bild = (int)s_bild; }
    }
    scd_audio_event_t ev;
    while (scd_audio_queue_pop(&ev)) {
        if (ev.kind != SCD_AUDIO_VOICE_ON) continue;
        char ordner[64], stamm[24];
        snprintf(ordner, sizeof ordner, "synchro/STAGE%u/room%04X/", (unsigned)(g_current_room_id >> 12),
                 (unsigned)g_current_room_id);
        snprintf(stamm, sizeof stamm, "main%02d", ev.sample_id);
        if (!re15_rekorder_r38_stimme(g_current_room_id, ev.sample_id, ordner, sizeof ordner, stamm, sizeof stamm))
            continue;                                  /* (Raum, Id) ohne Datei: der Lader bleibt stumm */
        char p[800]; snprintf(p, sizeof p, "%s/%s%s.wav", RE15_REPO_DIR, ordner, stamm);
        FILE *f = fopen(p, "rb"); s_voice40_pfad_ok = (f != NULL); if (f) fclose(f);
        s_voice40_n++;
        if (s_voice40_bild < 0) s_voice40_bild = (int)s_bild;
        if (!s_ton_aus) { s_clip_aktiv = 1; s_clip_pos = 0; }
    }
}

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    /* main.c: Wegwerf-Abfrage auf dem Spielpfad (re15_discard_tick mit den virtuellen Pad-Woertern) */
    if (re15_discard_active()) re15_discard_tick(re15_pad_virtual_word(edge), re15_pad_virtual_word(held));
    plattform_ton();
    s_bild++;
}

/* ---- Buchfuehrung je Bild ---- */
typedef struct {
    int msg4_bild, msg2_bild, piep_bild, auf_bild, piep_cut, vor_piep_cut, acc_bild, toene_ende, warte;
    unsigned z0;
    int erste_cut11_bild;
} lauf_t;
static lauf_t s_l;
static int    s_last_msg = -1, s_last_cut = -1;

static void beobachten(void)
{
    if (g_scd.message_active && g_scd.message_id != (uint8_t)s_last_msg) {
        if (g_scd.message_id == 4 && s_l.msg4_bild < 0) s_l.msg4_bild = (int)s_bild - 1;
        if (g_scd.message_id == 2 && s_l.msg2_bild < 0) s_l.msg2_bild = (int)s_bild - 1;
    }
    s_last_msg = g_scd.message_active ? g_scd.message_id : -1;
    unsigned gesp = g_re15_auf_zaehler - s_l.z0;
    if (gesp >= 1 && s_l.piep_bild < 0) { s_l.piep_bild = (int)s_bild - 1; s_l.piep_cut = g_scd.cam_id; s_l.vor_piep_cut = s_last_cut; }
    if (gesp >= 2 && s_l.auf_bild < 0)  s_l.auf_bild = (int)s_bild - 1;
    if (g_scd.cam_id == RE15_REK_ACCEPTED_CUT && s_l.erste_cut11_bild < 0 && s_l.msg4_bild >= 0)
        s_l.erste_cut11_bild = (int)s_bild - 1;
    s_last_cut = g_scd.cam_id;
}

static void bild(uint16_t held, uint16_t edge)
{
    frame(held, edge); beobachten();
    if (getenv("RE38J_DEBUG")) {
        printf("    dbg F%u pad=%04X msg=%d id=%d fsm=%d disc=%d cam=%d f54=%d f55=%d f1231=%d zust=%d thr:",
               s_bild - 1, held, g_scd.message_active, g_scd.message_id, g_scd.message_fsm, re15_discard_active(),
               g_scd.cam_id, re15_game_flag_get(5, 4), re15_game_flag_get(5, 5), re15_game_flag_get(12, 31),
               re15_rekorder_r38_zustand());
        for (int i = 0; i < SCD_THREAD_COUNT; i++)
            if (g_scd.threads[i].active && g_scd.threads[i].pc)
                printf(" %d@%05lX", i, (long)(g_scd.threads[i].pc - s_raw));
        printf("\n");
    }
}

static int room_boot(unsigned raum)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, raum);
    if (s_raw) { free(s_raw); s_raw = NULL; }
    s_raw = slurp(rp, &s_rawsz);
    if (!s_raw) return 0;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return 0;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = (uint16_t)raum; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -20690; pl->z = -25000; pl->rot_y = 0;      /* gemessene Spielerlage nach RE15_DEBUG_JUMP=1100 (Lauf v4) */
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    /* Minidisc Player w/ Disc (0x44) im Inventar + Genommen-Flag (3,0x67) (ROOM1200 sub01 @0x0095A) — sub00 @0x00AC8 */
    for (int s = 0; s < RE15_INV_MAX_SLOTS; s++) if (g_inv.slots[s].id == 0) { g_inv.slots[s].id = 0x44; g_inv.slots[s].qty = 1; break; }
    re15_game_flag_set(3, 0x67, 1);
    re15_game_flag_set(4, 0xE8, 0);
    s_shown = 0; s_bild = 0;
    s_clip_aktiv = 0; s_clip_pos = 0; s_voice40_bild = -1; s_voice40_n = 0; s_clip_ende_bild = -1; s_voice40_pfad_ok = 0;
    g_re15_voice_laeuft = 0; g_re15_voice_restbilder = 0;
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) frame(0, 0);
    return 1;
}

static int nachricht_schliessen(int max_bilder)
{
    for (int f = 0; f < max_bilder; f++) {
        if (!g_scd.message_active && !re15_discard_active()) return f;
        uint16_t b = (f % 4 == 0) ? RE15_PAD_BIT_SQUARE : 0;
        bild(b, b);
    }
    return -1;
}

/* Bis msg 2 aufgeht (oder sub02 durch ist): Nachricht(en) bedienen. schnell = 0: Viereck jedes 4. Bild (wie
 * nachricht_schliessen); schnell = 1: so schnell wie ein Spieler kann - Viereck 2 Bilder gehalten (Schnelllauf,
 * msg_common.c 2 Zeichen/Bild) und 1 Bild los, Flanke alle 3 Bilder (msg 4, Wegwerf-Abfrage "Ja"). */
static int bis_msg2(int schnell, int max_bilder)
{
    for (int f = 0; f < max_bilder; f++) {
        if (g_scd.message_active && g_scd.message_id == 2) return f;
        if (re15_rekorder_r38_zustand() == RE15_REK_ENDE) return f;
        uint16_t held, edge;
        if (schnell) { int k = f % 3; held = (k < 2) ? RE15_PAD_BIT_SQUARE : 0; edge = (k == 0) ? RE15_PAD_BIT_SQUARE : 0; }
        else         { held = edge = (f % 4 == 0) ? RE15_PAD_BIT_SQUARE : 0; }
        if (!g_scd.message_active && !re15_discard_active()) held = edge = 0;   /* nichts offen: nicht druecken */
        bild(held, edge);
    }
    return -1;
}

static int cursor_prop(int obj)
{
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_id == obj) return i;
    return -1;
}

/* Cursor per D-Pad (sub03..06: UP z+200 @0x00D24 `2f 02 c8 00`, DOWN z-200 @0x00D30, RIGHT x+200 @0x00D3C, LEFT x-200
 * @0x00D48) auf die Mitte des AOT-Platzes (sub00 @0x00B3C Platz 4 / @0x00B50 Platz 5, sce 5 flags 0x44). */
static int cursor_zu(int obj, int slot)
{
    int ci = cursor_prop(obj); if (ci < 0) return 0;
    const re15_aot_t *a = &g_aot.slots[slot];
    for (int f = 0; f < 400; f++) {
        int32_t dx = a->x - g_scd.props[ci].x, dz = a->z - g_scd.props[ci].z;
        /* im Rechteck (Rand 100) genuegt: der Notch-Test fragt das AOT-Rechteck ab; der Cursor gleitet an der
         * Raumkollision (gemessen: z bleibt bei 16068 stehen, Mitte von Platz 5 ist z 15775) */
        int innen = (dx <= a->half_w - 100 && dx >= -(a->half_w - 100) && dz <= a->half_h - 100 && dz >= -(a->half_h - 100));
        uint16_t b = 0;
        if (!innen) {
            if (dx > 150) b = RE15_PAD_BIT_RIGHT; else if (dx < -150) b = RE15_PAD_BIT_LEFT;
            else if (dz > 150) b = RE15_PAD_BIT_UP; else if (dz < -150) b = RE15_PAD_BIT_DOWN;
        }
        if (!b) { bild(0, 0); bild(0, 0); return 1; }
        if (getenv("RE38J_CURSOR") && f % 10 == 0)
            printf("    cursor F%u Platz %d: (%d,%d) Ziel (%d,%d) Taste %04X\n", s_bild, slot, (int)g_scd.props[ci].x,
                   (int)g_scd.props[ci].z, (int)a->x, (int)a->z, b);
        bild(b, b);
    }
    return 0;
}

static int warten_auf_flag(int bank, int bit, int wert, int max)
{
    for (int f = 0; f < max; f++) { if (re15_game_flag_get((uint8_t)bank, (uint8_t)bit) == wert) return f; bild(0, 0); }
    return -1;
}

/* Echter Weg bis sub02: Panel (Platz 2 sce 3 -> sub07) per USE (Runde 41 Spur O: msg 1 und Frage msg 3 entfallen), Cursor ON/OFF (Platz 4) -> sub08,
 * SAY HELLO ((5,5) = 1 @0x00D8C), Cursor Mikrofon (Platz 5). Rueckgabe 1 = Viereck auf dem Mikrofon gedrueckt. */
static int bis_mikrofon(void)
{
    (void)re15_keyitems_r41_testlauf(0x44);
    int frei = 0;
    for (int f = 0; f < 900 && !frei; f++) {
        uint16_t b = (g_scd.message_active && f % 4 == 0) ? RE15_PAD_BIT_SQUARE : 0;
        bild(b, b);
        frei = re15_game_flag_get(5, 4);
    }
    if (!frei) { printf("  FAIL: Cursor nicht frei ((5,4) @0x00D76)\n"); return 0; }
    if (!cursor_zu(0, 4)) { printf("  FAIL: Cursor erreicht ON/OFF nicht\n"); return 0; }
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE); bild(0, 0);
    if (warten_auf_flag(5, 5, 1, 60) < 0) { printf("  FAIL: SAY HELLO kommt nicht ((5,5) @0x00D8C)\n"); return 0; }
    if (!cursor_zu(0, 5)) { printf("  FAIL: Cursor erreicht das Mikrofon nicht\n"); return 0; }
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    return 1;
}

/* Ein Lauf: modus 0 = langsam (msg 4 bleibt 150 Bilder offen), 1 = schnell. */
static lauf_t lauf(unsigned raum, int mit_haken, int modus, int ton_aus)
{
    memset(&s_l, 0xff, sizeof s_l);
    s_ton_aus = ton_aus;
    if (!room_boot(raum)) { printf("  FAIL: ROOM%04X fehlt\n", raum); return s_l; }
    if (!mit_haken) g_re15_rekorder_anker_n = 0;
    s_l.z0 = g_re15_auf_zaehler;
    if (!bis_mikrofon()) return s_l;
    for (int f = 0; f < 10 && !(g_scd.message_active && g_scd.message_id == 4); f++) bild(0, 0);
    if (modus == 0) { for (int f = 0; f < 150; f++) bild(0, 0); }
    if (bis_msg2(modus, 1200) < 0) { printf("  FAIL: msg 4 / Wegwerf-Abfrage / msg 2 - sub02 kommt nicht an\n"); return s_l; }
    for (int f = 0; f < 40; f++) bild(0, 0);
    if (nachricht_schliessen(400) < 0) { printf("  FAIL: msg 2 bleibt offen\n"); return s_l; }
    for (int f = 0; f < 60; f++) bild(0, 0);
    s_l.toene_ende = (int)(g_re15_auf_zaehler - s_l.z0);
    s_l.warte = re15_rekorder_r38_wartebilder();
    return s_l;
}

/* ==================== Teil anker ========================================================================= */
static void teil_anker(void)
{
    printf("\n=== anker: Original-Bytes RE1.5/RE2, Runde-36-Folge, Port-Konstanten ===\n");
    char rp[600]; size_t n0 = 0, n1 = 0;
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1100.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *d = slurp(rp, &n0);
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1101.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *e = slurp(rp, &n1);
    CHECK("ROOM1100.RDT == ROOM1101.RDT (bytegleich, Leon/Elza)", d && e && n0 == n1 && memcmp(d, e, n0) == 0);
    static const uint8_t sub02[56] = {
        0x2b,0x04,0xff,0xff, 0x02,0x00, 0x29,0x0c,0x09,0x0a,0x05,0x00, 0x29,0x0b,0x09,0x0a,0x05,0x00,
        0x29,0x0c,0x09,0x0a,0x05,0x00, 0x29,0x0b,0x09,0x0a,0x05,0x00, 0x29,0x0c,0x09,0x0a,0x05,0x00,
        0x29,0x0b,0x09,0x0a,0x05,0x00, 0x2b,0x02,0xff,0xff, 0x02,0x00, 0x22,0x02,0x07,0x00, 0x29,0x06, 0x3c,0x01 };
    CHECK("sub02 @0x00C90: Message_on 4 / Evt_next / Cut 12,11 x3 je Sleep 5 / Message_on 2 / Evt_next / Set(2,7,0) / "
          "Cut 6 / Cut_auto 1", bytes_gleich(d, n0, 0x00C90, sub02, sizeof sub02));
    static const uint8_t vor[8] = { 0x22,0x04,0xe8,0x01, 0x22,0x05,0x00,0x00 };
    CHECK("sub02 @0x00C68 Set(4,0xE8,1) (Schloss offen) am Anfang", bytes_gleich(d, n0, 0x00C68, vor, 8));
    static const uint8_t tuer[4] = { 0x46,0x01,0x02,0x31 };
    CHECK("sub02 @0x00CC8 Aot_reset Platz 1 sce 2 (Tuer frei), naechster Opcode @0x00CD2 `46 02`",
          bytes_gleich(d, n0, 0x00CC8, tuer, 4) && d && d[0x00CD2] == 0x46 && d[0x00CD3] == 0x02);
    static const uint8_t sub01[34] = { 0x06,0x00,0x24,0x00, 0x21,0x05,0x05,0x01, 0x2e,0x03,0x00,0x00, 0x06,0x00,0x16,0x00,
        0x3e,0x00,0x0f,0x00,0x05,0x00, 0x06,0x00,0x0a,0x00, 0x51,0x01,0x40,0x00, 0x04,0xff,0x18,0x02 };
    CHECK("sub01 @0x00C2C Ck(5,5,1) Work_set(3,0) Member_cmp(0,0x0F,5) Sce_key_ck(1,0x40) Evt_exec sub02 (Mikrofon)",
          bytes_gleich(d, n0, 0x00C2C, sub01, sizeof sub01));
    static const uint8_t sub08[18] = { 0x22,0x05,0x04,0x00, 0x29,0x07, 0x09,0x0a,0x1e,0x00, 0x22,0x05,0x05,0x01, 0x29,0x08, 0x01,0x00 };
    CHECK("sub08 @0x00D82 Cut 7 SYSTEM READY / Sleep 30 / Set(5,5,1) / Cut 8 SAY HELLO", bytes_gleich(d, n0, 0x00D82, sub08, 18));
    /* Port-Konstanten == Bytes */
    CHECK("RE15_REK_MSG4/START/WARTE/ACCEPTED/MSG2/TUER zeigen auf 2b 04 / 02 / 29 0c / 29 0b / 2b 02 / 46 02",
          d && d[RE15_REK_MSG4] == 0x2b && d[RE15_REK_MSG4 + 1] == 4 && d[RE15_REK_START] == 0x02 &&
          d[RE15_REK_WARTE] == 0x29 && d[RE15_REK_WARTE + 1] == 0x0c && d[RE15_REK_ACCEPTED] == 0x29 &&
          d[RE15_REK_ACCEPTED + 1] == RE15_REK_ACCEPTED_CUT && d[RE15_REK_MSG2] == 0x2b && d[RE15_REK_MSG2 + 1] == 2 &&
          d[RE15_REK_TUER] == 0x46 && RE15_REK_SIG_LAENGE == 56);
    /* Anker: nur 1100/1101, nur mit Signatur */
    re15_rekorder_r38_raum_scan(d, (int)n0, 0x1100);
    int a1100 = g_re15_rekorder_anker_n;
    re15_rekorder_r38_raum_scan(e, (int)n1, 0x1101);
    int a1101 = g_re15_rekorder_anker_n;
    re15_rekorder_r38_raum_scan(d, (int)n0, 0x10D0);
    int a10d0 = g_re15_rekorder_anker_n;
    if (d) { d[0x00CA0] ^= 0x01; }
    re15_rekorder_r38_raum_scan(d, (int)n0, 0x1100);
    int aflip = g_re15_rekorder_anker_n;
    if (d) { d[0x00CA0] ^= 0x01; }
    re15_rekorder_r38_raum_scan(NULL, 0, 0);
    CHECK("Anker armiert in ROOM1100 und ROOM1101, nicht in anderen Raeumen, nicht bei abweichender Signatur",
          a1100 == 1 && a1101 == 1 && a10d0 == 0 && aflip == 0);
    free(d); free(e);

    /* RE2 ROOM2110 sub08 */
    snprintf(rp, sizeof rp, "%s/ROOM2110.RDT", RE15_RE2_RDT_DIR);
    size_t n2 = 0; uint8_t *r2 = slurp(rp, &n2);
    if (!r2) printf("  [SKIP] RE2 ROOM2110.RDT fehlt (%s)\n", rp);
    else {
        static const uint8_t piep[3] = { 0x36,0x02,0x0d }, kill[4] = { 0x65,0x01,0x65,0x02 }, lampe[4] = { 0x64,0x01,0x16,0x08 },
                             lampe2[4] = { 0x64,0x02,0x16,0x08 }, sl15[4] = { 0x09,0x0a,0x0f,0x00 }, auf[3] = { 0x36,0x02,0x0b };
        CHECK("RE2 ROOM2110 @0x01B5C Se_on(2,0x0D) Piep, @0x01B68 Lampen aus, @0x01B6C/@0x01B7C Anzeige offen (Zeile 1) - "
              "selber Skript-Takt (kein Sleep dazwischen)",
              bytes_gleich(r2, n2, 0x01B5C, piep, 3) && bytes_gleich(r2, n2, 0x01B68, kill, 4) &&
              bytes_gleich(r2, n2, 0x01B6C, lampe, 4) && bytes_gleich(r2, n2, 0x01B7C, lampe2, 4));
        CHECK("RE2 ROOM2110 @0x01B8C Sleep 15, @0x01B90 Se_on(2,0x0B) Aufschliessen",
              bytes_gleich(r2, n2, 0x01B8C, sl15, 4) && bytes_gleich(r2, n2, 0x01B90, auf, 3));
        free(r2);
    }
    /* Runde-36-Stellen und Folge */
    int g1100 = 0, g1101 = 0;
    for (int i = 0; i < re15_aufschliessen_r36_stellen(); i++) {
        unsigned r = 0; int msg = -1, art = -1, wege = 0;
        re15_aufschliessen_r36_stelle(i, &r, &msg, &art, &wege);
        if (r == 0x1100 && msg == 2 && art == 2 && (wege & RE15_AUF_WEG_SKRIPT)) g1100 = 1;
        if (r == 0x1101 && msg == 2 && art == 2 && (wege & RE15_AUF_WEG_SKRIPT)) g1101 = 1;
    }
    CHECK("gen/r36_aufschliessen.inc: (1100, 2) und (1101, 2) = KARTE_AUF auf dem Skript-Weg", g1100 && g1101);
    int satz[3] = { -1, -1, -1 }, bildv[3] = { -1, -1, -1 };
    int nf = re15_aufschliessen_r36_folge(2, satz, bildv, 3);
    CHECK("Folge KARTE_AUF = Satz 1 (PIEP, RE2 2110 Satz 0x0D) +0, Satz 2 (AUF_E, Satz 0x0B) +15",
          nf == 2 && satz[0] == 1 && bildv[0] == 0 && satz[1] == 2 && bildv[1] == 15);
}

/* ==================== Teil datei ========================================================================= */
static void teil_datei(void)
{
    printf("\n=== datei: Nutzer-Aufnahme und Bindung ===\n");
    char p[700]; snprintf(p, sizeof p, "%s/" RE15_REKORDER_ORDNER RE15_REKORDER_STAMM ".wav", RE15_REPO_DIR);
    size_t n = 0; uint8_t *b = slurp(p, &n);
    wav_t w; int ok = b && wav_kopf(b, n, &w);
    CHECK("synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav vorhanden und lesbar (RIFF/WAVE)", ok);
    if (ok) {
        printf("  WAV: Tag %d, %d Kanal, %d Hz, %d bit, data @%ld Laenge %ld -> %ld Samples = %.2f Spielbilder\n",
               w.tag, w.ch, w.rate, w.bits, w.data_off, w.data_len, w.data_len / 2 / w.ch,
               (double)(w.data_len / 2 / w.ch) * 30.0 / w.rate);
        CHECK("PCM 16 bit mono 32000 Hz (vom Lader angenommen), data-Laenge 169414 B = 84707 Samples = 79,4 Bilder",
              w.tag == 1 && w.ch == 1 && w.rate == 32000 && w.bits == 16 && w.data_off == 44 && w.data_len == 169414);
    }
    free(b);
    char ord[64], st[24];
    int r1 = re15_rekorder_r38_stimme(0x1100, RE15_REKORDER_STIMME_ID, ord, sizeof ord, st, sizeof st);
    char pfad[800]; snprintf(pfad, sizeof pfad, "%s/%s%s.wav", RE15_REPO_DIR, ord, st);
    FILE *f = r1 ? fopen(pfad, "rb") : NULL; int da = (f != NULL); if (f) fclose(f);
    CHECK("Bindung ROOM1100 Id 40 -> synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav (existiert)", r1 == 1 && da &&
          !strcmp(ord, "synchro/STAGE1/room1100/") && !strcmp(st, "Irons_Rekorder_Sound"));
    char ord2[64] = "x", st2[24] = "y";
    int r2 = re15_rekorder_r38_stimme(0x1101, RE15_REKORDER_STIMME_ID, ord2, sizeof ord2, st2, sizeof st2);
    CHECK("Bindung ROOM1101 (Elza) Id 40 -> dieselbe Datei im Ordner room1100", r2 == 1 && !strcmp(ord2, ord) && !strcmp(st2, st));
    char o3[64] = "unveraendert", s3[24] = "main04";
    int r3 = re15_rekorder_r38_stimme(0x1100, 4, o3, sizeof o3, s3, sizeof s3) +
             re15_rekorder_r38_stimme(0x1110, RE15_REKORDER_STIMME_ID, o3, sizeof o3, s3, sizeof s3) +
             re15_rekorder_r38_stimme(0x1100, 41, o3, sizeof o3, s3, sizeof s3);
    CHECK("keine Bindung fuer andere Ids/Raeume (msg 4 bleibt main04, ROOM1110, Id 41)", r3 == 0 &&
          !strcmp(o3, "unveraendert") && !strcmp(s3, "main04"));
    CHECK("Zensus-Erkennung: room1100/Irons_Rekorder_Sound.wav ja, room1101 nein, main04.wav nein",
          re15_rekorder_r38_ist_datei(0x1100, "Irons_Rekorder_Sound.wav") == 1 &&
          re15_rekorder_r38_ist_datei(0x1101, "Irons_Rekorder_Sound.wav") == 0 &&
          re15_rekorder_r38_ist_datei(0x1100, "main04.wav") == 0);
}

/* ==================== Teil ablauf ======================================================================== */
static void bericht(const char *name, const lauf_t *l)
{
    printf("  %s: msg4 F%d Aufnahme F%d (Anforderungen %d) Clip-Ende F%d | erstes ACCEPTED F%d Piep F%d (Cut %d, vorher %d) "
           "Aufschliessen F%d | msg2 F%d | Toene %d, Wartetakte %d, Pfad %s\n",
           name, l->msg4_bild, s_voice40_bild, s_voice40_n, s_clip_ende_bild, l->erste_cut11_bild, l->piep_bild, l->piep_cut,
           l->vor_piep_cut, l->auf_bild, l->msg2_bild, l->toene_ende, l->warte, s_voice40_pfad_ok ? "ok" : "-");
}

static void pruefe_mit(const char *name, const lauf_t *l, int erwarte_warten)
{
    char t[300];
    snprintf(t, sizeof t, "%s: Aufnahme (Id 40) genau einmal, im Takt von Message_on 4 (F%d), Datei gefunden", name, l->msg4_bild);
    CHECK(t, s_voice40_n == 1 && s_voice40_bild == l->msg4_bild && l->msg4_bild >= 0 && s_voice40_pfad_ok);
    snprintf(t, sizeof t, "%s: Piep im Takt des ersten ACCEPTED (Cut %d, vorher Cut 12 leer) - F%d", name, l->piep_cut, l->piep_bild);
    CHECK(t, l->piep_bild >= 0 && l->piep_bild == l->erste_cut11_bild && l->piep_cut == RE15_REK_ACCEPTED_CUT &&
             l->vor_piep_cut == 12);
    snprintf(t, sizeof t, "%s: Piep NACH dem Ende der Aufnahme (Clip-Ende F%d < Piep F%d)", name, s_clip_ende_bild, l->piep_bild);
    CHECK(t, s_clip_ende_bild >= 0 && l->piep_bild > s_clip_ende_bild);
    snprintf(t, sizeof t, "%s: Aufschliessen 15 Bilder nach dem Piep (F%d), vor msg 2 (F%d)", name, l->auf_bild, l->msg2_bild);
    CHECK(t, l->auf_bild == l->piep_bild + 15 && l->msg2_bild > l->auf_bild);
    snprintf(t, sizeof t, "%s: genau 2 Toene (Piep + Aufschliessen), msg 2 ohne zweite Folge (ausgelassen %u)", name,
             re15_rekorder_r38_unterdrueckt());
    CHECK(t, l->toene_ende == 2 && re15_rekorder_r38_zustand() == RE15_REK_ENDE);
    if (erwarte_warten) {
        snprintf(t, sizeof t, "%s: sub02 hat an @0x00C96 auf das Ende der Aufnahme gewartet (%d Takte)", name, l->warte);
        CHECK(t, l->warte > 0);
    } else {
        snprintf(t, sizeof t, "%s: Aufnahme vor dem Schliessen zu Ende -> kein Warten (%d Takte)", name, l->warte);
        CHECK(t, l->warte == 0);
    }
    /* Tuer frei: Platz 1 ist wieder die Tuer (sub02 @0x00CC8), ihr Feuer fordert den Raumwechsel nach ROOM1110 an */
    const re15_aot_t *a = &g_aot.slots[1];
    g_room_change.pending = 0;
    re15_aot_fire_slot(1);
    snprintf(t, sizeof t, "%s: Tuer offen - Platz 1 Tuer (typ %u), Feuer -> Raumwechsel ROOM%04X", name, (unsigned)a->type,
             g_room_change.room_id);
    CHECK(t, a->active && a->type == RE15_AOT_TYPE_DOOR && g_room_change.pending &&
             (g_room_change.room_id & 0xFFF0u) == 0x1110u && re15_rekorder_r38_tuer_frei() >= 1 &&
             re15_game_flag_get(4, 0xE8) == 1);
}

static void teil_ablauf(void)
{
    printf("\n=== ablauf: echter Skriptweg ROOM1100/1101 mit nachgebildetem Stimmen-Kanal ===\n");
    if (!wav_laden()) { CHECK("WAV fuer die Clip-Laenge lesbar", 0); return; }
    printf("  Clip-Laenge %ld Samples @44100 = %.2f Bilder\n", s_clip_len, (double)s_clip_len / 1470.0);

    lauf_t a = lauf(0x1100, 1, 0, 0); bericht("A Leon langsam", &a); pruefe_mit("A Leon langsam", &a, 0);
    lauf_t b = lauf(0x1100, 1, 1, 0); bericht("B Leon schnell", &b); pruefe_mit("B Leon schnell", &b, 1);
    CHECK("B: Piep mindestens 5 Bilder (Cut 12 + Sleep 5 @0x00C96/@0x00C98) nach dem Ende der Aufnahme",
          b.piep_bild >= s_clip_ende_bild + 5);
    lauf_t c = lauf(0x1101, 1, 1, 0); bericht("C Elza schnell", &c); pruefe_mit("C Elza ROOM1101", &c, 1);

    /* D Gegenprobe ohne Haken = Stand vorher */
    lauf_t dd = lauf(0x1100, 0, 1, 0); bericht("D Gegenprobe", &dd);
    CHECK("D Gegenprobe ohne Haken: keine Aufnahme, Piep erst im Takt von Message_on 2 (Runde 36), nicht am ACCEPTED",
          s_voice40_n == 0 && dd.piep_bild >= 0 && dd.piep_bild == dd.msg2_bild && dd.piep_bild > dd.erste_cut11_bild &&
          dd.toene_ende == 2);

    /* E ohne Ton: Plattform meldet nie "laeuft" -> nur die Stempel-Frist (2 Bilder), kein Haenger */
    lauf_t ee = lauf(0x1100, 1, 1, 1); bericht("E ohne Ton", &ee);
    CHECK("E ohne Ton: Ablauf haengt nicht (Piep am ACCEPTED, Tuer frei), gewartet hoechstens 2 Takte",
          ee.piep_bild >= 0 && ee.piep_bild == ee.erste_cut11_bild && ee.warte <= 2 && ee.toene_ende == 2 &&
          re15_rekorder_r38_zustand() == RE15_REK_ENDE);
}

/* ==================== Werkzeug cap ======================================================================= */
#define T_TICK 1470

/* Bester Versatz eines Bezugs w (wl Werte) im Mono-Mitschnitt m, Versatz lo..hi: grob im Raster D (Versatz UND Summe),
 * dann fein +-2*D mit allen Werten. Rueckgabe -1, wenn der Bereich leer ist. */
static long versatz_suchen(const float *m, long lo, long hi, const float *w, long wl, int D, double *best_out)
{
    double best = -1e300; long bk = -1;
    if (hi < lo) return -1;
    for (long k = lo; k <= hi; k += D) {
        double s = 0;
        for (long j = 0; j < wl; j += D) s += (double)m[k + j] * w[j];
        if (s > best) { best = s; bk = k; }
    }
    long k0 = bk - 2 * D < lo ? lo : bk - 2 * D, k1 = bk + 2 * D > hi ? hi : bk + 2 * D;
    best = -1e300;
    for (long k = k0; k <= k1; k++) {
        double s = 0;
        for (long j = 0; j < wl; j++) s += (double)m[k + j] * w[j];
        if (s > best) { best = s; bk = k; }
    }
    if (best_out) *best_out = best;
    return bk;
}

/* ---- Piep: Bezug aus der Bank, Suche per Kreuzkorrelation -------------------------------------------------------
 * Strang ton Stufe 1 Runde 3 (Pruefung Runde 2, R1): bis dahin galt als Piep-Einsatz der erste Tick, in dem das Band
 * 1380..1530 Hz mehr als 0,3 der Tick-Energie traegt (band_erster_tick unten). Das ist ein VERHAELTNIS zum Rest des
 * Ticks - also zur Musik darunter - und kippte, als die Musik unter gesprochenen Saetzen 8 dB leiser wurde: im Lauf S
 * (Piep 5 Spielbilder nach dem Ende der Aufnahme, Musik noch abgesenkt) Bandanteil im Key-On-Tick 0,308 statt knapp
 * unter 0,3 -> Tick 708 statt 709, obwohl der Piep auf demselben Abtastwert beginnt (Mitschnitt des Pruefers:
 * Bandenergie Tick 708 = 3,97e12, Folgeticks 3,0e12 .. 4,3e12; der Rest in Tick 708 ist der breitbandige Einsatz des
 * Tons selbst, 8,9e12).
 * Jetzt: der Piep wird als DAS gesucht, was er ist - Satz RE2_KARTE_SE_PIEP der Mini-Bank shared_assets/RE2/KARTSE.VBS
 * (gen/re2_karten_bank.inc: RE2 ROOM2110 Satz 0x0D, Satz der Mini-Bank `00 00 13 01`), gerendert wie der Mischer
 * (audio_pc.c audio_callback: Schritt = Tonhoehe << 4 in Q16, naechster Wert; Tonhoehe re15_vab_note2pitch2 wie
 * se_play_layers) - und zwar per Kreuzkorrelation. Deren groesster Wert haengt nicht davon ab, wie laut etwas anderes
 * daneben laeuft; er liefert den Abtastwert des Key-On. Ob wirklich der Piep dort liegt, sagt die gemessene
 * Verstaerkung (Skalarprodukt / Energie des Bezugs): sie muss zu der Lautstaerke passen, die der Mischer fuer diesen Ton
 * rechnet - RE2-Gesetz ohne Quadrat (re15_svm_keyon_reg_re2: `beq v1,v0,0x80083a98` @0x80083a38 ueberspringt
 * `mult a2,a2` @0x80083a40), Tastenstaerke = Ton-vol (RE2 FUN_8005bd6c), Register * 2 (psx-spx Z. 357-364), mal
 * Hauptlautstaerke 0x3fff * 2 (SsSetMVol(127,127) @0x80043ba8). */
typedef struct {
    float *x; long n;              /* Bezug bei 44100 Hz, Rohwerte der Tonprobe (ohne Lautstaerke) */
    double soll;                   /* Verstaerkung, die der Mischer rechnet (Mittel links/rechts) */
    int vag, pitch, reg_l, reg_r;
    long pcm_n;
    uint8_t satz[4];
} piep_bezug_t;

static int piep_bezug_laden(piep_bezug_t *b)
{
    static re15_vab_t vab;
    memset(b, 0, sizeof *b);
    re15_karte_bank_rec_t rec;
    re15_karte_bank_rec(&rec);
    size_t nv = 0;
    uint8_t *vbs = slurp(RE15_ASSET_PSX_DIR "/../RE2/KARTSE.VBS", &nv);
    if (!vbs || nv < (size_t)rec.vbd_off + rec.vbd_size || rec.edt_size < 12) { free(vbs); return 0; }
    const uint8_t *edt = vbs + rec.edt_off;
    /* wie load_re2_karte_se_pc (audio_pc.c): u32 vh_off @edt_size-8, dahinter die VH */
    uint32_t vh_off = (uint32_t)edt[rec.edt_size - 8] | ((uint32_t)edt[rec.edt_size - 7] << 8) |
                      ((uint32_t)edt[rec.edt_size - 6] << 16) | ((uint32_t)edt[rec.edt_size - 5] << 24);
    if (vh_off + 0x20u > rec.edt_size || re15_vab_parse(edt + vh_off, (size_t)rec.edt_size - vh_off, &vab) != 0) {
        free(vbs); return 0;
    }
    int vags[8], tones[8];
    int n = re15_edt_resolve_layers_ex(edt, &vab, RE2_KARTE_SE_PIEP, vags, tones, 8);
    if (n != 1 || vags[0] < 0 || vags[0] >= vab.vag_count) { free(vbs); return 0; }     /* der Piep hat eine Schicht */
    memcpy(b->satz, edt + RE2_KARTE_SE_PIEP * 4, 4);
    const re15_vab_tone_t *t = &vab.tones[tones[0]];
    const uint32_t off = vab.samples[vags[0]].offset, vsz = vab.samples[vags[0]].size;
    if (off + vsz > rec.vbd_size) { free(vbs); return 0; }
    const size_t cap = (vsz / 16) * 28;
    int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
    if (!pcm) { free(vbs); return 0; }
    const int pn = re15_vag_adpcm_decode(vbs + rec.vbd_off + off, vsz, pcm, cap);
    b->vag = vags[0]; b->pcm_n = pn;
    b->pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift);
    /* Mischer: s->pos_frac += step; s->pos += pos_frac >> 16 (audio_pc.c audio_callback), step = pitch << 4 */
    const uint32_t step = (uint32_t)b->pitch << 4;
    long nout = 0;
    if (pn > 0 && step) nout = (long)(((uint64_t)pn << 16) / step) + 2;
    b->x = (float *)malloc((size_t)(nout > 0 ? nout : 1) * sizeof(float));
    if (!b->x || pn <= 0 || !step) { free(pcm); free(vbs); return 0; }
    uint32_t frac = 0; long pos = 0, i = 0;
    while (pos < pn && i < nout) {
        b->x[i++] = (float)pcm[pos];
        frac += step; pos += (long)(frac >> 16); frac &= 0xffffu;
    }
    b->n = i;
    /* Lautstaerke wie se_stimme_q15 (audio_pc.c) fuer eine RE2-Bank ohne Lage */
    const int prog = (tones[0] / RE15_VAB_TONES_PER_PROGRAM) & (RE15_VAB_PROGRAM_COUNT - 1);
    re15_svm_vol_t in;
    memset(&in, 0, sizeof in);
    in.vab_mvol = vab.master_volume; in.prog_mvol = vab.prog_mvol[prog]; in.prog_pan = vab.prog_mpan[prog];
    in.tone_vol = t->vol; in.tone_pan = t->pan; in.key = t->vol; in.key_pan = RE15_SVM_PAN_MITTE;
    re15_svm_keyon_reg_re2(&in, &b->reg_l, &b->reg_r);
    b->soll = 0.5 * ((double)RE15_SPU_REG_Q15(b->reg_l) + (double)RE15_SPU_REG_Q15(b->reg_r)) / 32768.0 *
              ((double)RE15_SPU_MVOL_Q15 / 32768.0);
    free(pcm); free(vbs);
    return b->n > 4 * T_TICK;
}

/* Key-On des Pieps im Mono-Mitschnitt m ab Abtastwert lo (Bezug muss ganz hineinpassen: letzter Versatz hi).
 * Grob: beide Signale auf ein Achtel der Rate (Mittel ueber PIEP_D = 8 Werte; der Ton liegt bei 1,4 kHz, die neue
 * Nyquist-Grenze bei 2,7 kHz) und JEDER Versatz im Achtel-Raster - das sind knapp vier Stuetzstellen je Periode des
 * Tons (30 Abtastwerte); ein Raster von 16 wie bei der Aufnahme wuerde die Periode ueberspringen. Fein: +-96
 * Abtastwerte mit allen Werten. Rueckgabe Versatz oder -1; *korr = normierte Korrelation (haengt von der Musik ab,
 * nur zur Anzeige), *g = Verstaerkung. */
#define PIEP_D 8
static long piep_suchen(const float *m, long lo, long hi, const piep_bezug_t *b, double *korr, double *g)
{
    if (hi < lo || b->n < 2 * PIEP_D) return -1;
    const long nd = (hi + b->n - lo) / PIEP_D, rd = b->n / PIEP_D;
    float *md = (float *)malloc((size_t)(nd > 0 ? nd : 1) * sizeof(float));
    float *xd = (float *)malloc((size_t)(rd > 0 ? rd : 1) * sizeof(float));
    if (!md || !xd || nd <= rd) { free(md); free(xd); return -1; }
    for (long i = 0; i < nd; i++) { float a = 0; for (int q = 0; q < PIEP_D; q++) a += m[lo + PIEP_D * i + q]; md[i] = a / PIEP_D; }
    for (long i = 0; i < rd; i++) { float a = 0; for (int q = 0; q < PIEP_D; q++) a += b->x[PIEP_D * i + q]; xd[i] = a / PIEP_D; }
    double best = -1e300; long bk = -1;
    for (long k = 0; k + rd <= nd; k++) {
        float s = 0;
        for (long j = 0; j < rd; j++) s += md[k + j] * xd[j];
        if ((double)s > best) { best = (double)s; bk = k; }
    }
    free(md); free(xd);
    if (bk < 0) return -1;
    long k0 = lo + PIEP_D * bk - 96, k1 = lo + PIEP_D * bk + 96;
    if (k0 < lo) k0 = lo;
    if (k1 > hi) k1 = hi;
    best = -1e300; bk = -1;
    for (long k = k0; k <= k1; k++) {
        double s = 0;
        for (long j = 0; j < b->n; j++) s += (double)m[k + j] * b->x[j];
        if (s > best) { best = s; bk = k; }
    }
    double ww = 0, mm = 0;
    for (long j = 0; j < b->n; j++) { ww += (double)b->x[j] * b->x[j]; mm += (double)m[bk + j] * m[bk + j]; }
    if (korr) *korr = best / sqrt(ww * mm + 1e-9);
    if (g)    *g    = best / (ww + 1e-9);
    return bk;
}

/* Das FRUEHERE Mass (bis Runde 2): erster Tick ab `von`, in dem das Band 1380..1530 Hz mehr als 0,3 der Tick-Energie
 * traegt (Hann-Fenster, Bins wie rfft(1470); wie tools/tueren/r36_cap_ticks.py). Bleibt nur fuer die Gegenprobe im
 * Teil "piepmass" und als Zusatzzeile des Werkzeugs stehen - es haengt vom Pegel der Musik darunter ab. */
static long band_erster_tick(const float *m, long von, long bis, long *letzter_out, long *anzahl_out)
{
    long erster = -1, letzter = -1, anzahl = 0;
    static float x[T_TICK];
    for (long t = von; t < bis; t++) {
        double e = 0;
        for (int i = 0; i < T_TICK; i++) {
            double h = 0.5 - 0.5 * cos(2.0 * M_PI * i / (T_TICK - 1));
            x[i] = (float)(m[t * T_TICK + i] * h); e += (double)x[i] * x[i];
        }
        double re0 = 0; for (int i = 0; i < T_TICK; i++) re0 += x[i];
        double tot = ((double)T_TICK * e - re0 * re0) / 2.0 + 1e-9, band = 0;
        for (int k = 46; k <= 51; k++) {           /* 30 Hz je Bin: 1380..1530 Hz (> 1350 und < 1550) */
            double re = 0, im = 0;
            for (int i = 0; i < T_TICK; i++) { double ph = 2.0 * M_PI * k * i / T_TICK; re += x[i] * cos(ph); im -= x[i] * sin(ph); }
            band += re * re + im * im;
        }
        if (band / tot > 0.3) { if (erster < 0) erster = t; letzter = t; anzahl++; }
    }
    if (letzter_out) *letzter_out = letzter;
    if (anzahl_out)  *anzahl_out  = anzahl;
    return erster;
}

/* Grenzen der Verstaerkung, in denen der gefundene Ton als DER Piep gilt: Soll +-20 % (+1,6 / -1,9 dB). Gemessen am
 * echten Spiel (integration_r38_rekorder, Laeufe L / S / E): 0,992 / 0,997 / 0,997 des Solls mit abgesenkter Musik,
 * 0,992 / 0,993 / 0,997 mit voller Musik (Gegenprobe-exe ohne Absenkung); im Teil piepmass 0,988 .. 0,999 ueber zehn
 * Musikpegel. Gegenproben: die Mini-Bank mit dem RE1.5-Quadrat gespielt -> 0,391 des Solls; der Stand vor Stufe 1
 * (r42/story 9426267fa: Ton-vol linear und pauschal halbiert) -> 0,393; kein Piep -> 0,02; ein Piep mit halber
 * Lautstaerke -> 0,50. */
#define PIEP_G_MIN 0.80
#define PIEP_G_MAX 1.20

static int werkzeug_cap(int argc, char **argv)
{
    if (argc < 4) { printf("Aufruf: cap <cap.raw> <wav> [von bis]\n"); return 2; }
    size_t nc = 0, nw = 0;
    uint8_t *cb = slurp(argv[2], &nc), *wb = slurp(argv[3], &nw);
    wav_t w;
    if (!cb || !wb || !wav_kopf(wb, nw, &w) || w.bits != 16) { printf("FEHLER: Dateien\n"); return 2; }
    long wl = 0; float *wv = wav_44100(wb, &w, &wl);
    long ns = (long)(nc / 4);
    float *m = (float *)malloc((size_t)ns * sizeof(float));
    for (long i = 0; i < ns; i++) {
        int16_t l = (int16_t)(cb[i * 4] | (cb[i * 4 + 1] << 8)), r = (int16_t)(cb[i * 4 + 2] | (cb[i * 4 + 3] << 8));
        m[i] = 0.5f * ((float)l + (float)r);
    }
    long ticks = ns / T_TICK;
    long von = argc > 5 ? atol(argv[4]) : 0, bis = argc > 5 ? atol(argv[5]) : ticks;
    if (bis > ticks) bis = ticks;
    /* Aufnahme: grob Dezimation 16, dann fein +-32 */
    double best = 0;
    long bk = versatz_suchen(m, von * T_TICK, bis * T_TICK - wl, wv, wl, 16, &best);
    if (bk < 0) { printf("FEHLER: Mitschnitt kuerzer als die Aufnahme\n"); return 2; }
    double ww = 0, mm = 0;
    for (long j = 0; j < wl; j++) { ww += (double)wv[j] * wv[j]; mm += (double)m[bk + j] * m[bk + j]; }
    double korr = best / sqrt(ww * mm + 1e-9), g = best / (ww + 1e-9);
    long ende = bk + wl;
    printf("WAV %ld Samples = %.2f Ticks | Korrelation %.3f Verstaerkung %.3f\n", wl, (double)wl / T_TICK, korr, g);
    printf("WAV ANFANG Tick %ld (+%ld) ENDE letzter Tick %ld\n", bk / T_TICK, bk % T_TICK, (ende - 1) / T_TICK);
    /* Piep: Key-On per Kreuzkorrelation mit dem Bezug aus der Bank (Kopf ueber piep_bezug_t). Gesucht ab dem Anfang
     * der Aufnahme (ein Piep WAEHREND der Aufnahme waere der Befund; Menue-Toene von Titel/Spielerwahl davor zaehlen
     * nicht). Tick = der naechstliegende Tick-Anfang, Rest mit Vorzeichen. */
    piep_bezug_t pb;
    if (!piep_bezug_laden(&pb)) { printf("FEHLER: Piep-Bezug (shared_assets/RE2/KARTSE.VBS)\n"); return 2; }
    double pk = 0, pg = 0;
    long kp = piep_suchen(m, bk, bis * T_TICK - pb.n, &pb, &pk, &pg);
    printf("PIEP BEZUG Satz %02x %02x %02x %02x VAG %d Tonhoehe 0x%03x %ld Samples = %.2f Ticks Register %d/%d Soll-Verstaerkung %.4f\n",
           pb.satz[0], pb.satz[1], pb.satz[2], pb.satz[3], pb.vag, pb.pitch, pb.n, (double)pb.n / T_TICK,
           pb.reg_l, pb.reg_r, pb.soll);
    if (kp < 0) {
        printf("PIEP KEYON fehlt (Mitschnitt nach der Aufnahme kuerzer als der Piep)\n");
    } else {
        const long tick = (kp + T_TICK / 2) / T_TICK, rest = kp - tick * T_TICK;
        const double q = pg / pb.soll;
        printf("PIEP KEYON Tick %ld (%+ld) Korrelation %.3f Verstaerkung %.4f = %.3f des Solls -> %s\n", tick, rest, pk, pg, q,
               (q >= PIEP_G_MIN && q <= PIEP_G_MAX) ? "PEGEL-OK" : "PEGEL-FALSCH");
    }
    {   long letzter = -1, anzahl = 0;
        long erster = band_erster_tick(m, bk / T_TICK, bis, &letzter, &anzahl);
        printf("BAND 1380..1530 Hz > 0,3 (frueheres Mass, haengt vom Musikpegel ab): erster Tick %ld letzter %ld Anzahl %ld\n",
               erster, letzter, anzahl); }
    free(cb); free(wb); free(wv); free(m); free(pb.x);
    return 0;
}

/* ==================== Teil piepmass (Strang ton Stufe 1 Runde 3) ========================================
 * Riegel fuer das Mass selbst, ohne Spiel und ohne Tondatei im Repo: ein kuenstlicher Mitschnitt aus
 *   - "Musik": weisses Rauschen (fester Zufallsgenerator), einmal geglaettet, in ZEHN Pegeln im Abstand von 2 dB
 *     (Effektivwert 500 .. 3972),
 *   - dem Piep aus der Bank mit der Verstaerkung, die der Mischer rechnet, Key-On auf einem Tick-Anfang.
 * MUSS: das neue Mass findet den Key-On bei JEDEM Pegel im selben Tick (Rest hoechstens eine Periode des Tons,
 * 30 Abtastwerte) und die Verstaerkung innerhalb der Grenzen; ohne Piep meldet es PEGEL-FALSCH.
 * GEGENPROBE im selben Lauf: das fruehere Mass (band_erster_tick) nennt ueber dieselben zehn Pegel NICHT ueberall
 * denselben Tick - genau der Fehler, an dem integration_r38_rekorder in Runde 2 rot wurde. */
static uint32_t s_pm_lcg;
static float pm_rausch(void)
{
    s_pm_lcg = s_pm_lcg * 1664525u + 1013904223u;
    return (float)((int32_t)(s_pm_lcg >> 8) & 0xffff) / 32768.0f - 1.0f;      /* -1 .. +1 */
}

static void teil_piepmass(void)
{
    printf("== piepmass: Piep-Key-On unabhaengig vom Musikpegel ==\n");
    piep_bezug_t pb;
    int ok = piep_bezug_laden(&pb);
    CHECK("Bezug: KARTSE.VBS Satz 1 (PIEP) = `00 00 13 01` (Direkt-Zweig: byte3 & 0x1f = 1 < 0x10), eine Schicht, "
          "14 .. 16 Ticks lang", ok && pb.satz[0] == 0x00 && pb.satz[1] == 0x00 && pb.satz[2] == 0x13 && pb.satz[3] == 0x01 &&
          pb.n >= 14 * T_TICK && pb.n <= 16 * T_TICK);
    if (!ok) return;
    printf("  Bezug: VAG %d, %ld Proben-Werte, Tonhoehe 0x%03x, %ld Samples, Register %d/%d, Soll-Verstaerkung %.4f\n",
           pb.vag, pb.pcm_n, pb.pitch, pb.n, pb.reg_l, pb.reg_r, pb.soll);
    /* RE2-Gesetz ohne Quadrat (re15_svm_keyon_reg_re2, RE2 @0x8008376c-0x80083804 und `beq` @0x80083a38): die Mini-Bank
     * hat Bank-Lautstaerke 127 (VH+0x18), Programm 0 Lautstaerke 127 / Pan 64 (VH+0x21 / +0x24), Ton 1 Lautstaerke 80 /
     * Pan 64 (VH+0x820+32+2 / +3); Tastenstaerke = Ton-vol = 80 (RE2 FUN_8005bd6c). Register = 0x3fff * 80 * 80 /
     * 127^2 = 6500 je Seite. Mit dem RE1.5-Quadrat waeren es 6500^2 / 0x3fff = 2578 (-8,0 dB). */
    CHECK("Soll-Lautstaerke des Pieps nach dem RE2-Gesetz (ohne Quadrat): Register 6500/6500 = 0x3fff * 80 * 80 / 127^2",
          pb.reg_l == 6500 && pb.reg_r == 6500);

    enum { TICKS = 60, KEYON = 20, PEGEL = 10 };
    const long ns = (long)TICKS * T_TICK;
    float *rau = (float *)malloc((size_t)ns * sizeof(float)), *m = (float *)malloc((size_t)ns * sizeof(float));
    if (!rau || !m) { CHECK("Speicher", 0); free(rau); free(m); free(pb.x); return; }
    s_pm_lcg = 0x52453135u;                                    /* "RE15" */
    {   float alt = 0; double e = 0;
        for (long i = 0; i < ns; i++) { float r = pm_rausch(); rau[i] = 0.5f * (r + alt); alt = r; e += (double)rau[i] * rau[i]; }
        const float norm = (float)(1.0 / sqrt(e / (double)ns));
        for (long i = 0; i < ns; i++) rau[i] *= norm; }         /* Effektivwert 1 */
    long neu_tick[PEGEL], neu_rest[PEGEL], alt_tick[PEGEL];
    int neu_gleich = 1, neu_pegel = 1, alt_verschieden = 0;
    double qmin = 1e9, qmax = -1e9;
    for (int p = 0; p < PEGEL; p++) {
        const double rms = 500.0 * pow(10.0, 2.0 * p / 20.0);
        for (long i = 0; i < ns; i++) m[i] = (float)(rms * rau[i]);
        for (long j = 0; j < pb.n; j++) m[(long)KEYON * T_TICK + j] += (float)(pb.soll * pb.x[j]);
        double k = 0, g = 0;
        const long kp = piep_suchen(m, 0, ns - pb.n, &pb, &k, &g);
        neu_tick[p] = (kp + T_TICK / 2) / T_TICK; neu_rest[p] = kp - neu_tick[p] * T_TICK;
        alt_tick[p] = band_erster_tick(m, 0, TICKS, NULL, NULL);
        const double q = g / pb.soll;
        if (q < qmin) qmin = q;
        if (q > qmax) qmax = q;
        printf("  Musik-Effektivwert %6.0f: neu Tick %ld (%+ld) Korrelation %.3f Verstaerkung %.3f des Solls | frueher Tick %ld\n",
               rms, neu_tick[p], neu_rest[p], k, q, alt_tick[p]);
        if (neu_tick[p] != KEYON || labs(neu_rest[p]) > 30) neu_gleich = 0;
        if (q < PIEP_G_MIN || q > PIEP_G_MAX) neu_pegel = 0;
        if (alt_tick[p] != alt_tick[0]) alt_verschieden = 1;
    }
    CHECK("neues Mass: Key-On bei allen zehn Musikpegeln im Tick des Key-On (Rest hoechstens 30 Abtastwerte)", neu_gleich);
    printf("  Verstaerkung ueber alle Pegel: %.3f .. %.3f des Solls\n", qmin, qmax);
    CHECK("neues Mass: Verstaerkung bei allen zehn Musikpegeln innerhalb 0,80 .. 1,20 des Solls", neu_pegel);
    CHECK("GEGENPROBE: das fruehere Mass (Bandanteil > 0,3) nennt ueber dieselben Pegel NICHT ueberall denselben Tick",
          alt_verschieden);
    /* ohne Piep: nur Musik (mittlerer Pegel) -> irgendein Versatz, aber die Verstaerkung passt nicht */
    {   const double rms = 500.0 * pow(10.0, 2.0 * 5 / 20.0);
        for (long i = 0; i < ns; i++) m[i] = (float)(rms * rau[i]);
        double k = 0, g = 0;
        (void)piep_suchen(m, 0, ns - pb.n, &pb, &k, &g);
        printf("  ohne Piep: Verstaerkung %.3f des Solls, Korrelation %.3f\n", g / pb.soll, k);
        CHECK("ohne Piep: Verstaerkung ausserhalb 0,80 .. 1,20 des Solls (PEGEL-FALSCH)", g / pb.soll < PIEP_G_MIN);
        /* Piep mit halber Lautstaerke (6 dB zu leise) -> ebenfalls ausserhalb */
        for (long j = 0; j < pb.n; j++) m[(long)KEYON * T_TICK + j] += (float)(0.5 * pb.soll * pb.x[j]);
        (void)piep_suchen(m, 0, ns - pb.n, &pb, &k, &g);
        printf("  Piep mit halber Lautstaerke: Verstaerkung %.3f des Solls\n", g / pb.soll);
        CHECK("Piep 6 dB zu leise: Verstaerkung ausserhalb 0,80 .. 1,20 des Solls (PEGEL-FALSCH)", g / pb.soll < PIEP_G_MIN); }
    free(rau); free(m); free(pb.x);
}

/* ==================== Werkzeug bild ====================================================================== */
#define CHUNK 0x10000
static int decode_chunk(const uint8_t *chunk, uint8_t *rgb_out)
{
    re15_bss_chunk_t ch;
    if (!re15_bss_parse_chunk(chunk, CHUNK, &ch))  return -1;
    if (!re15_bss_chunk_has_video(&ch))            return -2;
    size_t cap = ((size_t)ch.run_length_words + 2) * 8 + 4096;
    int16_t *co = (int16_t *)malloc(cap * sizeof(int16_t));
    if (!co) return -3;
    int n = re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size, ch.run_length_words, ch.quant, ch.version, co, cap);
    if (n < 0) { free(co); return -4; }
    int rv = re15_bss_mdec_decode(co, (size_t)n, RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT, rgb_out);
    free(co);
    return rv == 0 ? 0 : -5;
}

static int werkzeug_bild(int argc, char **argv)
{
    if (argc < 4) { printf("Aufruf: bild <ppm> <ROOM110.BSS>\n"); return 2; }
    size_t np = 0, nb = 0;
    uint8_t *pp = slurp(argv[2], &np), *bss = slurp(argv[3], &nb);
    if (!pp || !bss || np < 15 || pp[0] != 'P' || pp[1] != '6') { printf("FEHLER: Dateien\n"); return 2; }
    int wd = 0, ht = 0, mx = 0, hdr = 0;
    if (sscanf((const char *)pp, "P6 %d %d %d%n", &wd, &ht, &mx, &hdr) < 3 || wd != 320 || ht != 240) {
        printf("FEHLER: kein 320x240-P6\n"); return 2;
    }
    const uint8_t *img = pp + hdr + 1;
    static const int cuts[3] = { 8, 11, 12 };
    static uint8_t bg[3][320 * 240 * 3];
    for (int c = 0; c < 3; c++)
        if ((long)(cuts[c] + 1) * CHUNK > (long)nb || decode_chunk(bss + (long)cuts[c] * CHUNK, bg[c]) != 0) {
            printf("FEHLER: Cut %d nicht dekodierbar\n", cuts[c]); return 2;
        }
    /* Anzeigefeld = Pixel, in denen sich die drei Hintergruende unterscheiden (SAY HELLO / ACCEPTED / leer) */
    double diff[3] = { 0, 0, 0 }; long pix = 0;
    for (int i = 0; i < 320 * 240; i++) {
        int unters = 0;
        for (int k = 0; k < 3; k++) {
            int a = bg[0][i * 3 + k], b = bg[1][i * 3 + k], c = bg[2][i * 3 + k];
            if (abs(a - b) > 40 || abs(b - c) > 40 || abs(a - c) > 40) unters = 1;
        }
        if (!unters) continue;
        pix++;
        for (int c = 0; c < 3; c++)
            for (int k = 0; k < 3; k++) diff[c] += abs((int)img[i * 3 + k] - (int)bg[c][i * 3 + k]);
    }
    int best = 0;
    for (int c = 1; c < 3; c++) if (diff[c] < diff[best]) best = c;
    printf("ANZEIGE Cut %d | Feld %ld Pixel | Abstand Cut 8 %.1f, Cut 11 %.1f, Cut 12 %.1f\n", cuts[best], pix,
           pix ? diff[0] / pix : 0.0, pix ? diff[1] / pix : 0.0, pix ? diff[2] / pix : 0.0);
    free(pp); free(bss);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "cap"))  return werkzeug_cap(argc, argv);
    if (argc > 1 && !strcmp(argv[1], "bild")) return werkzeug_bild(argc, argv);
    const char *teil = argc > 1 ? argv[1] : "alle";
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "anker"))  teil_anker();
    if (alle || !strcmp(teil, "datei"))  teil_datei();
    if (alle || !strcmp(teil, "ablauf")) teil_ablauf();
    if (alle || !strcmp(teil, "piepmass")) teil_piepmass();
    printf("\n%s: %d Fehler\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
