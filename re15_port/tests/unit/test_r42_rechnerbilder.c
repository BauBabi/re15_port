/*
 * test_r42_rechnerbilder.c — Runde 42 Strang "gegenstaende", Stufe 2: Kartenbild "MICHAEL", Rechner-Vollbilder (305 Cut 10, 505 Cut 5,
 * 2080 Bild 9 als Standbild) und das Nahbild des toten Polizisten 1050 (Cut 9, Leon).
 * Belege: include/re15_rechnerbilder_r42.h, include/re15_gegenstaende_r42.h, tools/r42_gegenstaende/michaelkarte.py.
 * Harness = test_r42_gegenstaende.c. Der Riegel benutzt NUR Schnittstellen, die es schon vor dem Strang gab (g_scd.work_vars[0x0A] =
 * angezeigter Cut nach op_cut_chg @0x800402fc, scd_thread_start, scd_event_fire, re15_itps_pixel, stderr), damit die Gegenprobe auf
 * aaea5f8c2 baut und dort ROT wird.
 *
 * Teile (Aufruf: test_r42_rechnerbilder <teil>):
 *   karte      ITPS-Block 0x47: Pixel nur im Namensfeld (x 50..80, y 33..57) anders als ITEM/ITPS.ITP, dort > 150, ausserhalb 0,
 *              kein durchsichtiger Eintrag
 *   r305       3050/3051 sub15 (Evt_exec wie sub01 @0x0221C): Cut 10 zu msg 6, danach Cut 2 wie @0x0262A
 *   r505       5050/5051 Rechner (Ereignis 2): Ja -> Cut 4 (msg 1), Cut 5 (msg 2), Cut 4 (msg 3); sub03: Cut 5 (msg 5), dann Cut 3
 *   s2080      2080/2081 Wasser ablassen: Standbild Bild 9 (Zeile "[rechnerbilder42]"), Kamera nie 9, danach Cut 7, Cut 3, Hinweis msg 52
 *   leiche9    1050 (Leon) Leiche: Cut 9 (Nahbild) zu Text 5, danach Cut 3 (Form ROOM1051 sub03 @0x00DB4/@0x00DC0)
 */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE 1
#endif
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
#include "re15_item_discard.h"
#include "re15_item_modal.h"
#include "re15_item_use.h"
#include "re15_keyitems_r41.h"
#include "re15_gameflow.h"
#include "re15_blink_r41.h"
#include "re15_light.h"
#include "re15_climb.h"
#include "re15_itps.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ---- Harness (Muster test_r41_gatorszene.c) --------------------------------------------------------------- */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static int                s_msg_gesehen[64];

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
static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt;
    g_re15_room_lights_ok = (s_rdt.lights && re15_light_parse(s_rdt.lights, (size_t)s_rdt.lights_size, &g_re15_room_lights) == 0);
    return 0;
}
static void spielbild(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
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
    if (g_scd.message_active && g_scd.message_id < 64) s_msg_gesehen[g_scd.message_id] = 1;
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
    re15_fade_tick();   /* im Spiel in re15_render_end_frame (nach dem Spielbild); Abblende story1_r42.c */
}
static void frame(uint16_t held, uint16_t edge)
{
    if (!re15_discard_frozen()) spielbild(held, edge);
    if (re15_discard_active()) re15_discard_tick((edge & RE15_PAD_BIT_SQUARE) ? (uint16_t)0x4000u : 0, 0);
}
static int room_boot(uint16_t room, int32_t px, int32_t py, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx); memset(s_msg_gesehen, 0, sizeof s_msg_gesehen);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_discard_reset();
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_room_rdt_ok = 1;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = py;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band((uint8_t)(py < 0 ? (-py) / 1800 : 0));
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}


static void bits_loeschen(void) { memset(&g_game, 0, sizeof g_game); }

/* ---- Hilfen ------------------------------------------------------------------------------------------------------------- */
/* Cut-Folge: jeder neue Wert von work_vars[0x0A] (= angezeigter Cut, op_cut_chg @0x800402fc) und die erste Nachricht dazu. */
static int s_folge[32], s_folge_msg[32], s_nfolge;
static void folge_start(void) { s_nfolge = 0; s_folge[s_nfolge] = g_scd.work_vars[0x0A]; s_folge_msg[s_nfolge++] = -1; }
static void folge_bild(uint16_t k)
{
    frame(k, k);
    const int c = g_scd.work_vars[0x0A];
    if (s_nfolge < 32 && c != s_folge[s_nfolge - 1]) { s_folge[s_nfolge] = c; s_folge_msg[s_nfolge++] = -1; }
    if (g_scd.message_active && s_folge_msg[s_nfolge - 1] < 0) s_folge_msg[s_nfolge - 1] = g_scd.message_id;
}
static void folge_druck(const char *was)
{
    printf("  %s Cut-Folge:", was);
    for (int i = 0; i < s_nfolge; i++) printf(" %d(msg %d)", s_folge[i], s_folge_msg[i]);
    printf("\n");
}
/* Teilfolge (Cut, msg) der Reihe nach enthalten; msg -2 = egal */
static int folge_hat(const int *cut, const int *msg, int n)
{
    int k = 0;
    for (int i = 0; i < s_nfolge && k < n; i++)
        if (s_folge[i] == cut[k] && (msg[k] == -2 || s_folge_msg[i] == msg[k])) k++;
    return k == n;
}
static void laufen(int bilder)
{
    for (int f = 0; f < bilder; f++) {
        const uint16_t k = ((f % 15) == 0 && (g_scd.message_active || re15_discard_active())) ? RE15_PAD_BIT_SQUARE : 0;
        folge_bild(k);
        if (f > 90 && !g_scd.message_active && !re15_discard_active() && g_scd.player_mode != 2 &&
            !g_scd.threads[SCD_EVENT_SLOT_FIRST].active && !g_scd.threads[SCD_EVENT_SLOT_FIRST + 1].active) break;
    }
}
static const uint8_t k_evt_exec_15[6] = { 0x04, 0xff, 0x18, 0x0f, 0x01, 0x00 };   /* Form ROOM3050 sub01 @0x0221C + Evt_end */
static const uint8_t k_evt_exec_03[6] = { 0x04, 0xff, 0x18, 0x03, 0x01, 0x00 };   /* Form ROOM5050 sub01 @0x00850 + Evt_end */

/* stderr in eine Datei (Zeilen der VM / des Moduls zaehlen) */
static char s_logpfad[64];
static void log_an(const char *name)
{
    snprintf(s_logpfad, sizeof s_logpfad, "r42rb_%s.log", name);
    fflush(stderr);
    if (!freopen(s_logpfad, "w", stderr)) s_logpfad[0] = 0;
}
static int log_zaehle(const char *a, const char *b)
{
    fflush(stderr);
    FILE *f = s_logpfad[0] ? fopen(s_logpfad, "r") : NULL;
    char z[512]; int n = 0;
    if (!f) return 0;
    while (fgets(z, sizeof z, f)) if (strstr(z, a) && (!b || strstr(z, b))) n++;
    fclose(f);
    return n;
}

/* ---- karte ---------------------------------------------------------------------------------------------------------------- */
static void teil_karte(void)
{
    printf("== karte ==\n");
    size_t n = 0;
    uint8_t *d = slurp(RE15_ASSET_PSX_DIR "/ITEM/ITPS.ITP", &n);
    PRUEF(d && n >= 0x48 * 0x3000 && re15_itps_load() == 0 && re15_itps_available(0x47), "ITEM/ITPS.ITP geladen, Block 0x47 da");
    if (!d || n < 0x48 * 0x3000) return;
    const uint8_t *b = d + 0x47 * 0x3000;
    int innen = 0, aussen = 0, durchsichtig = 0;
    for (int v = 0; v < RE15_ITPS_H; v++)
        for (int u = 0; u < RE15_ITPS_W; u++) {
            const uint8_t idx = b[0x220 + v * RE15_ITPS_W + u];
            const uint16_t c = (uint16_t)(b[0x14 + idx * 2] | (b[0x14 + idx * 2 + 1] << 8));
            uint8_t r = 0, g = 0, bl = 0;
            const int o = re15_itps_pixel(0x47, u, v, &r, &g, &bl);
            const int gleich = (c == 0) ? !o
                             : (o && r == ((c & 31) << 3) && g == (((c >> 5) & 31) << 3) && bl == (((c >> 10) & 31) << 3));
            const int feld = u >= 50 && u <= 80 && v >= 33 && v <= 57;
            if (!gleich) { if (feld) innen++; else aussen++; }
            if (feld && !o) durchsichtig++;
        }
    PRUEF(innen > 150, "Namensfeld x 50..80, y 33..57: %d Bildpunkte neu (\"MICHAEL\" statt \"GORO / MACIDA\")", innen);
    PRUEF(aussen == 0, "ausserhalb des Namensfelds unveraendert (%d abweichend)", aussen);
    PRUEF(durchsichtig == 0, "kein durchsichtiger CLUT-Eintrag im Namensfeld (%d)", durchsichtig);
    free(d);
}

/* ---- r305 ----------------------------------------------------------------------------------------------------------------- */
static void teil_r305(void)
{
    printf("== r305 ==\n");
    for (int v = 0; v < 2; v++) {
        const uint16_t raum = (uint16_t)(0x3050 + v);
        bits_loeschen();
        re15_inv_init();
        if (room_boot(raum, -25400, 0, -16000, 2048, 2) != 0) continue;
        for (int f = 0; f < 10; f++) frame(0, 0);
        folge_start();
        PRUEF(scd_thread_start(SCD_EVENT_SLOT_FIRST, k_evt_exec_15) == 0, "ROOM%04X: Evt_exec sub15 (Form sub01 @0x0221C) gestartet", raum);
        laufen(2000);
        folge_druck(v ? "3051" : "3050");
        static const int c[2] = { 10, 2 }, mm[2] = { 6, -2 };
        PRUEF(folge_hat(c, mm, 2), "ROOM%04X: Cut 10 (Bildschirm) zu msg 6 \"successfully restored\", danach Cut 2 (@0x0262A)", raum);
        PRUEF(re15_game_flag_get(3, 0xB3) && re15_game_flag_get(3, 0xB0), "ROOM%04X: Set(3,0xB3)/(3,0xB0) wie @0x0260E/@0x02612", raum);
    }
}

/* ---- r505 ----------------------------------------------------------------------------------------------------------------- */
static void teil_r505(void)
{
    printf("== r505 ==\n");
    for (int v = 0; v < 2; v++) {
        const uint16_t raum = (uint16_t)(0x5050 + v);
        bits_loeschen();
        re15_inv_init();
        if (room_boot(raum, -17000, 0, -22300, 2048, 0) != 0) continue;
        for (int f = 0; f < 10; f++) frame(0, 0);
        folge_start();
        PRUEF(scd_event_fire(2) >= 0, "ROOM%04X: Ereignis 2 (Rechner, sub00 @0x0076C) startet", raum);
        for (int f = 0; f < 900; f++) {
            const uint16_t k = ((f % 15) == 0 && g_scd.message_active) ? RE15_PAD_BIT_SQUARE : 0;
            folge_bild(k);
            if (re15_game_flag_get(5, 5) && !g_scd.message_active && f > 60) break;
        }
        folge_druck(v ? "5051 sub02" : "5050 sub02");
        static const int c1[3] = { 4, 5, 4 }, m1[3] = { 1, 2, 3 };
        PRUEF(folge_hat(c1, m1, 3), "ROOM%04X: Ja -> Cut 4 (@0x00880) zu msg 1, Cut 5 (Bildschirm) zu msg 2, Cut 4 zu msg 3 (Cursor-Spiel)", raum);
        PRUEF(re15_game_flag_get(5, 0) && re15_game_flag_get(5, 5), "ROOM%04X: Spiel an (Bank 5 Bits 0..5 @0x00898..@0x008AC)", raum);
        /* Spiel-Ende wie sub01 @0x00850 */
        folge_start();
        PRUEF(scd_thread_start(SCD_EVENT_SLOT_FIRST + 1, k_evt_exec_03) == 0, "ROOM%04X: Evt_exec sub03 (Form sub01 @0x00850) gestartet", raum);
        laufen(2000);
        folge_druck(v ? "5051 sub03" : "5050 sub03");
        static const int c2[2] = { 5, 3 }, m2[2] = { 5, -2 };
        PRUEF(folge_hat(c2, m2, 2), "ROOM%04X: Cut 5 zu msg 5 \"Registration complete.\", danach Cut 3 (@0x008F0)", raum);
        PRUEF(re15_game_flag_get(3, 0x7B), "ROOM%04X: Set(3,0x7B) (@0x008B4)", raum);
    }
}

/* ---- s2080 ---------------------------------------------------------------------------------------------------------------- */
static void teil_s2080(void)
{
    printf("== s2080 ==\n");
    for (int v = 0; v < 2; v++) {
        const uint16_t raum = (uint16_t)(0x2080 + v);
        bits_loeschen();
        re15_inv_init();
        re15_inv_load_briefing();
        log_an(v ? "2081" : "2080");
        if (room_boot(raum, -11160, -14400, -7316, 3072, 3) != 0) continue;
        for (int f = 0; f < 20; f++) frame(0, 0);
        folge_start();
        PRUEF(scd_event_fire(2) >= 0, "ROOM%04X: Ereignis 2 (Rechner, sub00 @0x00C1A) startet", raum);
        int nie9 = 1, msg52 = 0;
        for (int f = 0; f < 3000; f++) {
            const uint16_t k = ((f % 20) == 0 && g_scd.message_active) ? RE15_PAD_BIT_SQUARE : 0;
            folge_bild(k);
            if (g_scd.work_vars[0x0A] == 9 || g_scd.cam_id == 9) nie9 = 0;
            if (g_scd.message_active && g_scd.message_id == 52) msg52 = 1;
            if (msg52 && !g_scd.message_active) break;
        }
        folge_druck(v ? "2081" : "2080");
        const int an = log_zaehle("[rechnerbilder42]", "Standbild (Kamera bleibt)");
        const int aus = log_zaehle("[rechnerbilder42]", "endet (Cut_chg 7)");
        PRUEF(an == 1, "ROOM%04X: Cut_chg(9) >= nCut 9 -> Standbild (Zeilen %d)", raum, an);
        PRUEF(aus == 1, "ROOM%04X: Standbild endet mit Cut_chg 7 (@0x00CC2) (Zeilen %d)", raum, aus);
        PRUEF(nie9, "ROOM%04X: Kamera nie auf 9 (keine Kamera 9 in der Tabelle, nCut 9)", raum);
        static const int c[2] = { 7, 3 }, mm[2] = { -2, -2 };
        PRUEF(folge_hat(c, mm, 2), "ROOM%04X: danach Cut 7 (Wasser) und Cut 3 wie im Original", raum);
        PRUEF(re15_game_flag_get(3, 245) && msg52, "ROOM%04X: (3,245) = 1 und Hinweis msg 52 unveraendert", raum);
    }
}

/* ---- leiche9 -------------------------------------------------------------------------------------------------------------- */
static void teil_leiche9(void)
{
    printf("== leiche9 ==\n");
    bits_loeschen();
    re15_inv_init();
    if (room_boot(0x1050, 15750, 0, -5800, 2048, 3) != 0) return;
    for (int f = 0; f < 10; f++) frame(0, 0);
    folge_start();
    PRUEF(scd_event_fire(5) >= 0, "ROOM1050: Ereignis 5 (Leiche) startet");
    for (int f = 0; f < 600; f++) {
        const uint16_t k = ((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        folge_bild(k);
        if (re15_item_modal_active()) re15_item_modal_tick(0, 0);
    }
    folge_druck("1050");
    static const int c[2] = { 9, 3 }, mm[2] = { 5, -2 };
    PRUEF(folge_hat(c, mm, 2), "ROOM1050 (Leon): Cut 9 = toter Polizist zu Text 5, danach Cut 3 (Form 1051 @0x00DB4/@0x00DC0)");
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "karte")) teil_karte();
    else if (!strcmp(t, "r305")) teil_r305();
    else if (!strcmp(t, "r505")) teil_r505();
    else if (!strcmp(t, "s2080")) teil_s2080();
    else if (!strcmp(t, "leiche9")) teil_leiche9();
    else { printf("unbekannter Teil '%s'\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
