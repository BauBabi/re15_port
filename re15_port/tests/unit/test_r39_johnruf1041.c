/*
 * test_r39_johnruf1041.c — Runde 39 Spur P "johnruf1041": Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit allen
 * Installern, SCD-VM mit Opcode 0x5F, Nachrichten-FSM, re15_game_step mit Kartenhinweis-Schirm und Spielbild-Tick).
 * Dossier analysis/befunde_runde39/P_johnruf1041.md, Code include/re15_johnruf_r39.h + engine/src/johnruf_r39.c, Programme
 * engine/src/gen/johnruf_r39_programme.inc (tools/r39_johnruf/programme_bauen.py).
 *
 * Teile (Aufruf: test_r39_johnruf1041 <teil>):
 *   texte      msg 40 dekodiert = Wortlaut des Nutzers (Nachtrag 10, Schreibung Nachtrag 6 = derselbe Satz wie Spur J msg 50)
 *   programme  Programm-Bytes: Reihenfolge Rufe leise (Xa_vol 50) -> Drehen -> Xa_vol 120 -> msg 40 -> Karte (59) -> Bit 97 ->
 *              Steuerung zurueck; 1131: Xa_vol 100 -> help02 -> Pause >= Ruflaenge -> Xa_vol 120; Original-Zeilen bytegleich
 *              mit ROOM1041.RDT sub11; Kopf-Konstanten = Werkzeug
 *   stimme     Sprach-Ordner/Stamm: (1041|1131, 53..57) -> room1141/help01..05, alles andere unveraendert
 *   xa         Opcode 0x5F: nur in den Programmen RE2 Xa_vol (Wert < 0x80, pc += 2), sonst op_unknown (pc += 1); CD_VOL =
 *              0x3FFF * vol / 120; Raumaufbau setzt 120
 *   ablauf     ROOM1041 mit Elza am VM: Programm nur mit (4,0xF0)=1 und Bit 97=0; Pad-Sperre, Rufe leise, Drehung zur Treppe,
 *              msg 40 mit Standardlautstaerke, Kartenhinweis ROOM1041 -> ROOM1141 (Blatt/Rechteck der Zone), Faden parkt bis
 *              der Schirm zu ist, dann Bit 97 und Steuerung zurueck; Musikfenster + Raummusik-Anstoss; kein zweites Mal
 *   musik      Weiche/Sperre: 0xFF01 in STAGE1 ausser ROOM1141, nur im Fenster; Rettung ((4,0xF0)=0 und (4,0xF2)=0) loescht
 *              Bit 97 und stoesst die Tabellen-Musik an; waehrend des Kampfs ((4,0xF2)=1) bleibt sie; Kartenziel aktiv
 *   ruf1131    ROOM1131 in der Panikphase: Ruf help02 mit Xa_vol 100, einmal je Phase; ohne Bit 97 / nach der Rettung keiner
 *   leon       Leon ROOM1040 mit denselben Flags: kein Programm, Weiche -1
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
#include "re15_gameflow.h"
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_inventory.h"
#include "re15_map_hint.h"
#include "re15_johnruf_r39.h"
#include "re15_tuersperre_r39.h"
#include "re15_elzazombies_r39.h"     /* Integration: Spur Q fuer diese P-Riegel abgeschaltet */
#include "re15_stimmen_pegel_r39.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

#define PI_D 3.14159265358979323846

void re15_actor_step_all_walkers(void);
extern int g_test_bgm_reset_count, g_test_bgm_start_count, g_test_bgm_start_room;   /* tests/test_support.c */

static int g_fail = 0, g_n = 0;
#define PRUEF(c, ...) do { g_n++; if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
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

/* ================================================================== Raum + Bilder (Muster test_r38_tagebuch.c) */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_bild = 0, s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;
static uint64_t           s_uhr = 1000000;

typedef struct { int bild, id, xa; } stimme_t;   /* VOICE_ON: Bild, Sprach-Id, XA-Lautstaerke beim Abspielen */
static stimme_t s_st[64];
static int      s_n_st = 0;
static int      s_hint_nr = -1, s_hint_bild = -1;

static int rdt_laden(uint16_t room)
{
    if (s_raw && s_rdt_raum == room) return 0;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c: ist das Spiel eingefroren (Statusschirm samt Blenden), laeuft nur
 * der Schirm; sonst VM, Gaenger, Meldungen, Spielschritt (darin re15_johnruf_r39_tick und die Annahme des Hinweises). Die
 * Audio-Warteschlange wird nach dem Spielschritt geleert wie in audio_pc.c re15_audio_tick: die XA-Lautstaerke, mit der
 * re15_voice_play eine Sprachzeile startet, ist die zu diesem Zeitpunkt (re15_johnruf_r39_xa_cdvol). */
static void bild(uint16_t pressed, uint16_t held)
{
    s_uhr += 16667; re15_host_clock_set_us(s_uhr);
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = pressed;
    if (re15_menu_gameplay_frozen()) {
        re15_game_step(&s_ctx);
        if (re15_menu_is_open()) re15_inv_screen_ecg_tick();
        re15_fade_tick();
        s_bild++;
        return;
    }
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {   /* Szenen-Modus wie platform/pc/main.c (Muster test_r39_dining1141.c): (1,27)/(2,7) -> player_mode 2 */
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut = s_shown;
    if (re15_map_hint_pending() >= 0 && s_hint_nr < 0) { s_hint_nr = re15_map_hint_pending(); s_hint_bild = s_bild; }
    re15_game_step(&s_ctx);
    re15_fade_tick();
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e))
        if (e.kind == SCD_AUDIO_VOICE_ON && s_n_st < 64) {
            s_st[s_n_st].bild = s_bild; s_st[s_n_st].id = e.sample_id; s_st[s_n_st].xa = re15_johnruf_r39_xa_vol(); s_n_st++;
        }
    s_bild++;
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;   /* Nachbesserung 1: der Spielweg liest den GELADENEN Raum (Anker tuersperre_r39.c) */
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;   /* Normalzustand wie test_r39_dining1141.c */
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = 0; s_bild = 0; s_n_st = 0; s_hint_nr = -1; s_hint_bild = -1; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_map_hint_room_scan(s_raw, (int)s_rawsz, room);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static int faden_im_programm(int nr)
{
    int len = 0;
    const uint8_t *p = re15_johnruf_r39_programm(nr, &len);
    for (int s = 0; s < SCD_THREAD_COUNT; s++)
        if (g_scd.threads[s].active && g_scd.threads[s].pc >= p && g_scd.threads[s].pc < p + len) return 1;
    return 0;
}

static int flag(int b, int i) { return re15_game_flag_get((uint8_t)b, (uint8_t)i); }

/* ================================================================== texte */
static void teil_texte(void)
{
    printf("== texte ==\n");
    int len = 0;
    char t[512];
    const uint8_t *b = re15_johnruf_r39_text(&len);
    t[0] = 0;
    if (b && len > 0) re15_msg_decode_text(b, (size_t)len, t, sizeof t);
    printf("    msg 40: \"%s\"\n", t);
    PRUEF(!strcmp(t, "Elza: Oh, no, this is John! He is in trouble, I have to help him!"),
          "msg 40 = Nutzer-Wortlaut Nachtrag 10 (\"trouble\" klein, Nachtrag 6)");
    PRUEF(len > 8 && b[0] == 0x04 && b[1] == 0x00 && b[2] == 0x05 && b[3] == 0x01 &&
          b[len - 4] == 0x04 && b[len - 3] == 0x01 && b[len - 2] == 0x01 && b[len - 1] == 0x63,
          "Form ROOM1041 msg 2 @0x022DC: `04 00 05 01` (Elza Farbe 1) .. `04 01 01 63`");
}

/* ================================================================== programme */
static int op_len(const uint8_t *p)
{
    switch (p[0]) {
        case 0x00: case 0x02: case 0x42: return 1;
        case 0x01: case 0x12: case 0x3C: case 0x5F: return 2;
        case 0x2E: return 3;
        case 0x09: case 0x11: case 0x21: case 0x22: case 0x2B: return 4;
        case 0x40: return 8;
        default: return -1;
    }
}

typedef struct { int off, rdt, len; } orig_t;

static void teil_programme(void)
{
    printf("== programme ==\n");
    int n = 0;
    const uint8_t *p = re15_johnruf_r39_programm(0, &n);
    /* Opcode-Walk + Reihenfolge */
    int o = 0, xa = RE15_R39P_XA_NORMAL, i_rf1 = -1, i_rf2 = -1, i_dreh = -1, i_xa120 = -1, i_m40 = -1, i_karte = -1, i_b97 = -1,
        i_frei = -1, ende = -1, sperre = -1, xa_rf1 = -1, xa_rf2 = -1, xa_m40 = -1, dreh_x = 0, dreh_z = 0, dreh_mode = -1, ok = 1;
    while (o < n) {
        const int l = op_len(p + o);
        if (l < 0) { ok = 0; break; }
        if (p[o] == 0x22 && p[o + 1] == 2 && p[o + 2] == 7 && p[o + 3] == 1 && sperre < 0) sperre = o;
        if (p[o] == 0x5F) { xa = p[o + 1]; if (xa == RE15_R39P_XA_NORMAL && i_xa120 < 0) i_xa120 = o; }
        if (p[o] == 0x2B && p[o + 1] == RE15_R39P_HILFE_ID0 + RE15_R39P_RUF1041_A - 1) { i_rf1 = o; xa_rf1 = xa; }
        if (p[o] == 0x2B && p[o + 1] == RE15_R39P_HILFE_ID0 + RE15_R39P_RUF1041_B - 1) { i_rf2 = o; xa_rf2 = xa; }
        if (p[o] == 0x40) { i_dreh = o; dreh_mode = p[o + 2]; dreh_x = (int16_t)(p[o + 4] | p[o + 5] << 8); dreh_z = (int16_t)(p[o + 6] | p[o + 7] << 8); }
        if (p[o] == 0x2B && p[o + 1] == RE15_R39P_MSG_SATZ) { i_m40 = o; xa_m40 = xa; }
        if (p[o] == 0x2B && p[o + 1] == RE15_R39P_MSG_KARTE) i_karte = o;
        if (p[o] == 0x22 && p[o + 1] == 9 && p[o + 2] == RE15_R39P_BIT_PANIK && p[o + 3] == 1) i_b97 = o;
        if (p[o] == 0x22 && p[o + 1] == 2 && p[o + 2] == 7 && p[o + 3] == 0) i_frei = o;
        if (p[o] == 0x01) ende = o;
        o += l;
    }
    PRUEF(ok && o == n && ende == n - 2, "RUF1041 laesst sich opcode-genau durchlaufen (%d B), Evt_end am Ende", n);
    PRUEF(sperre == 0, "Pad-Sperre Set(2,7)=1 als erstes Opcode (Form ROOM1041 sub11 @0x021F8)");
    PRUEF(i_rf1 > 0 && xa_rf1 == RE15_R39P_XA_LEISE, "erster Ruf help%02d mit Xa_vol %d (leise, RE2 ROOM11D0 @0x01F84)",
          RE15_R39P_RUF1041_A, xa_rf1);
    PRUEF(i_dreh > i_rf1 && dreh_mode == 9 && dreh_x == RE15_R39P_TREPPE_X && dreh_z == RE15_R39P_TREPPE_Z,
          "danach Plc_dest Modus 9 zur Treppentuer (%d,%d) (Form sub11 @0x0220C)", dreh_x, dreh_z);
    PRUEF(i_rf2 > i_dreh && xa_rf2 == RE15_R39P_XA_LEISE, "zweiter Ruf help%02d (\"Please help!\") ebenfalls Xa_vol %d",
          RE15_R39P_RUF1041_B, xa_rf2);
    PRUEF(i_xa120 > i_rf2 && i_m40 > i_xa120 && xa_m40 == RE15_R39P_XA_NORMAL,
          "Xa_vol 120 VOR msg 40: Elzas Zeile mit Standardlautstaerke (Form RE2 ROOM1010 sub22)");
    PRUEF(i_karte > i_m40 && i_b97 > i_karte && i_frei > i_b97,
          "Reihenfolge msg 40 -> Karte (Warte-Zeile 59) -> Set(9,97)=1 -> Steuerung zurueck");
    /* zwischen zweitem Ruf und Xa_vol 120 mindestens die Ruflaenge help02 (1,96 s = 59 Bilder) */
    {
        int sl = 0;
        for (o = i_rf2; o < i_xa120; o += op_len(p + o)) if (p[o] == 0x09) sl += p[o + 2] | p[o + 3] << 8;
        PRUEF(sl >= 59, "Pause zwischen help02 und Xa_vol 120 = %d Bilder >= 59 (der Ruf klingt mit 50 aus)", sl);
    }
    /* Original-Zeilen bytegleich mit ROOM1041.RDT sub11 */
    if (rdt_laden(0x1041) == 0) {
        static const orig_t k[] = {
            { 0x00, 0x021F8, 8 },   /* Set(2,7)=1 Set(1,0x1B)=1                       */
            { 0x12, 0x02204, 8 },   /* Work_set(1,0) Nop Sleep 25                      */
            { 0x22, 0x02214, 16 },  /* Do Evt_next Nop Edwhile Ck(5,0x20)==0 Sleep 25  */
            { 0x40, 0x02228, 4 },   /* Sleep 100                                       */
            { 0x4C, 0x0222C, 18 },  /* Set(2,7)=0 .. Evt_end                           */
        };
        int alle = 1;
        for (unsigned i = 0; i < sizeof k / sizeof k[0]; i++)
            if (k[i].off + k[i].len > n || memcmp(p + k[i].off, s_raw + k[i].rdt, (size_t)k[i].len)) {
                alle = 0; printf("    Abweichung +%04X <-> @0x%05X\n", k[i].off, k[i].rdt);
            }
        PRUEF(alle, "Original-Zeilen bytegleich mit ROOM1041.RDT sub11 (5 Bereiche, 54 B)");
        PRUEF(p[0x1A] == 0x40 && !memcmp(p + 0x1A, s_raw + 0x0220C, 4) && p[0x3C] == 0x2B && s_raw[0x02224] == 0x2B &&
              p[0x3D] == RE15_R39P_MSG_SATZ && s_raw[0x02225] == 0x02,
              "Plc_dest-Kopf `40 00 09 20` = @0x0220C, msg 40 an der Stelle von msg 2 @0x02224");
    }
    /* RUF1131 */
    p = re15_johnruf_r39_programm(1, &n);
    {
        int xa2 = RE15_R39P_XA_NORMAL, rf = -1, xarf = -1, sl = 0, reset = -1;
        for (o = 0; o < n && op_len(p + o) > 0; o += op_len(p + o)) {
            if (p[o] == 0x5F) { xa2 = p[o + 1]; if (rf >= 0 && xa2 == RE15_R39P_XA_NORMAL) reset = o; }
            if (p[o] == 0x2B && p[o + 1] == RE15_R39P_HILFE_ID0 + RE15_R39P_RUF1131 - 1) { rf = o; xarf = xa2; }
            if (p[o] == 0x09 && rf >= 0 && reset < 0) sl += p[o + 2] | p[o + 3] << 8;
        }
        PRUEF(rf >= 0 && xarf == RE15_R39P_XA_LAUTER && xarf > RE15_R39P_XA_LEISE,
              "RUF1131: help%02d mit Xa_vol %d (> %d in 1041, RE2 ROOM1020 @0x01960)", RE15_R39P_RUF1131, xarf, RE15_R39P_XA_LEISE);
        PRUEF(reset > rf && sl >= 59 && p[n - 2] == 0x01, "danach Pause %d Bilder >= 59, Xa_vol 120, Evt_end - keine Pad-Sperre", sl);
        int sperre2 = 0;
        for (o = 0; o + 3 < n; o++) if (p[o] == 0x22 && p[o + 1] == 2 && p[o + 2] == 7) sperre2 = 1;
        PRUEF(!sperre2, "RUF1131 ohne Set(2,7): Elza laeuft weiter");
    }
}

/* ================================================================== stimme */
static void teil_stimme(void)
{
    printf("== stimme ==\n");
    static const struct { unsigned raum; int id; const char *dir, *stamm; } k[] = {
        { 0x1041, 53, "synchro/STAGE1/room1141/", "help01" }, { 0x1041, 54, "synchro/STAGE1/room1141/", "help02" },
        { 0x1041, 55, "synchro/STAGE1/room1141/", "help03" }, { 0x1041, 57, "synchro/STAGE1/room1141/", "help05" },
        { 0x1131, 54, "synchro/STAGE1/room1141/", "help02" },
        { 0x1041, 40, "synchro/STAGE1/room1041/", "main40" },   /* Elzas Satz: Ordnerregel, unveraendert */
        { 0x1041, 58, "synchro/STAGE1/room1041/", "main58" },
        { 0x1141, 54, "synchro/STAGE1/room1141/", "main54" },   /* 1141 = Spur J (re15_dining_r39_stamm), hier unberuehrt */
        { 0x1040, 54, "synchro/STAGE1/room1040/", "main54" },   /* Leon */
    };
    for (unsigned i = 0; i < sizeof k / sizeof k[0]; i++) {
        char dir[64], stamm[24];
        snprintf(dir, sizeof dir, "synchro/STAGE%u/room%04X/", k[i].raum >> 12, k[i].raum);
        snprintf(stamm, sizeof stamm, "main%02d", k[i].id);
        re15_johnruf_r39_stimme(k[i].raum, k[i].id, dir, sizeof dir, stamm, sizeof stamm);
        PRUEF(!strcmp(dir, k[i].dir) && !strcmp(stamm, k[i].stamm), "ROOM%04X Id %d -> %s%s.wav", k[i].raum, k[i].id, dir, stamm);
    }
}

/* ================================================================== xa */
static void teil_xa(void)
{
    printf("== xa ==\n");
    int n = 0;
    const uint8_t *p = re15_johnruf_r39_programm(0, &n);
    re15_johnruf_r39_install(0x1050);
    PRUEF(re15_johnruf_r39_xa_vol() == RE15_R39P_XA_NORMAL && re15_johnruf_r39_xa_cdvol() == 0x3FFF,
          "Raumaufbau: Xa_vol 120 (RE2 FUN_80052f3c @0x80053038) -> CD_VOL 0x3FFF = bisheriger Port-Wert");
    int o = -1;
    for (int i = 0; i < n; i++) if (p[i] == 0x5F) { o = i; break; }
    scd_thread_t t;
    memset(&t, 0, sizeof t);
    t.pc = p + o;
    int r = re15_johnruf_r39_op_xa_vol(&t);
    PRUEF(o >= 0 && r == 1 && t.pc == p + o + 2 && re15_johnruf_r39_xa_vol() == RE15_R39P_XA_LEISE,
          "im Programm: Xa_vol(%d) gesetzt, pc += 2 (@0x800580bc), Rueckgabe 1", RE15_R39P_XA_LEISE);
    PRUEF(re15_johnruf_r39_xa_cdvol() == 0x3FFFu * 50u / 120u, "CD_VOL bei 50 = %u (0x3FFF * 50 / 120)", re15_johnruf_r39_xa_cdvol());
    static const uint8_t fremd[4] = { 0x5F, 0x10, 0x00, 0x00 };
    t.pc = fremd;
    r = re15_johnruf_r39_op_xa_vol(&t);
    PRUEF(r == 1 && t.pc == fremd + 1 && re15_johnruf_r39_xa_vol() == RE15_R39P_XA_LEISE,
          "ausserhalb (RDT-Bytecode): op_unknown - pc += 1, Lautstaerke unveraendert");
    /* Grenze @0x800580a0 sltiu 0x80: ein Wert >= 0x80 wird nicht uebernommen - im Programm gibt es keinen, Probe am Puffer
     * des Programms (Kopie, damit die Schranke greift: die Kopie liegt AUSSERHALB -> op_unknown; daher nur die Werte pruefen) */
    PRUEF(RE15_R39P_XA_LEISE < RE15_R39P_XA_GRENZE && RE15_R39P_XA_LAUTER < RE15_R39P_XA_GRENZE &&
          RE15_R39P_XA_NORMAL < RE15_R39P_XA_GRENZE, "alle Werte < 0x80 (RE2 @0x800580a0 `sltiu v0,v1,0x80`)");
    re15_johnruf_r39_install(0x1131);
    PRUEF(re15_johnruf_r39_xa_vol() == RE15_R39P_XA_NORMAL, "naechster Raumaufbau: wieder 120");
}

/* ================================================================== ablauf */
static void elza_1041(int f0, int b97)
{
    re15_game_state_init();
    re15_inv_init();
    g_gameflow.character = 4;
    re15_game_flag_set(4, 0xF0, f0 ? 1 : 0);
    re15_game_flag_set(9, RE15_R39P_BIT_PANIK, b97 ? 1 : 0);
    re15_game_flag_set(3, 0x5F, 1); re15_game_flag_set(4, 0xF1, 1);
    (void)room_boot(0x1041, -24194, 16500, 2048);
}

static void teil_ablauf(void)
{
    printf("== ablauf ==\n");
    elza_1041(0, 0);
    PRUEF(!faden_im_programm(0), "(4,0xF0)=0 (John-Szene 1021 noch nicht gelaufen): kein Programm in 1041");
    elza_1041(1, 1);
    PRUEF(!faden_im_programm(0), "Bit 97 = 1 (Panikphase lief schon): kein Programm (Wiederbetreten ohne Wiederholung)");
    elza_1041(1, 0);
    PRUEF(faden_im_programm(0), "(4,0xF0)=1 und Bit 97=0: RUF1041 laeuft nach dem Raumaufbau");

    int start_vorher = g_test_bgm_start_count;
    int b_sperre = -1, b_m40 = -1, b_karte_zu = -1, b_b97 = -1, frei_davor = 0, rot_m40 = -1, hint_page = -1, hint_rect = -1;
    int hint_bilder = 0, b97_frei_gleich = 0;
    for (int f = 0; f < 4000; f++) {
        const int war_b97 = flag(9, RE15_R39P_BIT_PANIK);
        bild(0, 0);
        if (b_sperre < 0 && flag(2, 7)) b_sperre = s_bild;
        if (b_sperre >= 0 && !flag(9, RE15_R39P_BIT_PANIK) && !flag(2, 7) && !re15_menu_gameplay_frozen()) frei_davor++;
        if (b_m40 < 0 && g_scd.message_active && g_scd.message_id == RE15_R39P_MSG_SATZ) {
            b_m40 = s_bild; rot_m40 = (int)(g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y & 0xFFF);
        }
        if (g_inv_screen.hint_aktiv) { hint_bilder++; hint_page = g_inv_screen.hint_page; hint_rect = g_inv_screen.hint_rect; }
        if (s_hint_bild >= 0 && b_karte_zu < 0 && !re15_menu_gameplay_frozen() && !re15_menu_map_hint_active() && hint_bilder > 0)
            b_karte_zu = s_bild;
        if (!war_b97 && flag(9, RE15_R39P_BIT_PANIK)) { b_b97 = s_bild; b97_frei_gleich = !flag(2, 7); }
        if (b_b97 >= 0 && f > b_b97 + 30) break;
    }
    printf("    Sperre ab Bild %d, msg 40 Bild %d, Hinweis angefordert Bild %d (Eintrag %d), Schirm %d Bilder (Blatt %d Rechteck %d), "
           "zu Bild %d, Bit 97 Bild %d\n", b_sperre, b_m40, s_hint_bild, s_hint_nr, hint_bilder, hint_page, hint_rect, b_karte_zu, b_b97);
    for (int i = 0; i < s_n_st; i++) printf("    Stimme Bild %d Id %d Xa_vol %d\n", s_st[i].bild, s_st[i].id, s_st[i].xa);
    PRUEF(b_sperre >= 0 && b_sperre <= 1, "Pad-Sperre (2,7) ab dem ersten Spielbild (Form sub11 @0x021F8)");
    PRUEF(frei_davor == 0, "zwischen Sperre und Bit 97 kein freies Spielbild (Rufe, Satz, Karte am Stueck)");
    PRUEF(s_n_st >= 3 && s_st[0].id == RE15_R39P_HILFE_ID0 + RE15_R39P_RUF1041_A - 1 && s_st[0].xa == RE15_R39P_XA_LEISE &&
          s_st[1].id == RE15_R39P_HILFE_ID0 + RE15_R39P_RUF1041_B - 1 && s_st[1].xa == RE15_R39P_XA_LEISE,
          "zwei Rufe help03, help02 mit Xa_vol 50 beim Abspielen (leise)");
    PRUEF(s_n_st >= 3 && s_st[2].id == RE15_R39P_MSG_SATZ && s_st[2].xa == RE15_R39P_XA_NORMAL && s_st[2].bild > s_st[1].bild,
          "danach Elzas Stimme main40 (Id 40) mit Xa_vol 120");
    {
        const double soll = atan2(-(double)(RE15_R39P_TREPPE_Z - 16500), (double)(RE15_R39P_TREPPE_X - (-24194))) / (2.0 * PI_D) * 4096.0;
        int d = ((rot_m40 - (int)lround(soll)) % 4096 + 4096 + 2048) % 4096 - 2048;
        PRUEF(rot_m40 >= 0 && abs(d) <= 32, "Elza blickt bei msg 40 zur Treppentuer: Gier %d, Soll %.0f (Abweichung %d)", rot_m40, soll, d);
    }
    const int nr = re15_map_hint_eintrag_fuer(RE15_R39P_RAUM_1041, RE15_R39P_RAUM_1141);
    int zp = -1, zr = -1;
    (void)re15_map_hint_ziel(nr, &zp, &zr);
    PRUEF(nr >= 0 && s_hint_nr == nr && s_hint_bild > b_m40, "Kartenhinweis ROOM1041 -> ROOM1141 (Eintrag %d) nach msg 40 angefordert", nr);
    PRUEF(hint_bilder > 0 && hint_page == zp && hint_rect == zr, "Schirm zeigt das Ziel ROOM1141: Blatt %d Rechteck %d (Zone 0)", zp, zr);
    PRUEF(b_b97 >= 0 && b_karte_zu >= 0 && b_b97 >= b_karte_zu && b97_frei_gleich,
          "Bit 97 erst nach dem Schliessen (Bild %d >= %d), Steuerung im selben Bild zurueck", b_b97, b_karte_zu);
    PRUEF(re15_johnruf_r39_fenster() && g_test_bgm_start_count > start_vorher && g_test_bgm_start_room == 0x04,
          "Musikfenster auf, Raummusik in ROOM1041 angestossen (Raum-Byte 0x04)");
    PRUEF(re15_johnruf_r39_bgm_eintrag(0, 0x04) == RE15_R39P_BGM_EINTRAG, "Weiche ROOM1041 -> 0xFF01 (MAIN01)");
    PRUEF(!faden_im_programm(0) && !flag(2, 7) && !flag(1, 0x1B), "Programm zu Ende, Pad und Balken frei");
    PRUEF(re15_johnruf_r39_xa_vol() == RE15_R39P_XA_NORMAL, "Xa_vol nach dem Programm wieder 120");
    /* Wiederbetreten: kein zweites Mal */
    (void)room_boot(0x1041, -24194, 16500, 2048);
    for (int f = 0; f < 120; f++) bild(0, 0);
    PRUEF(!faden_im_programm(0) && s_n_st == 0 && s_hint_nr < 0, "Wiederbetreten in der Panikphase: kein Programm, kein Ruf, keine Karte");
}

/* ================================================================== musik */
static void teil_musik(void)
{
    printf("== musik ==\n");
    elza_1041(1, 0);
    for (int f = 0; f < 4000 && !flag(9, RE15_R39P_BIT_PANIK); f++) bild(0, 0);
    for (int f = 0; f < 5; f++) bild(0, 0);
    PRUEF(flag(9, RE15_R39P_BIT_PANIK) && re15_johnruf_r39_fenster(), "Panikphase + Fenster nach dem 1041-Ablauf");
    static const struct { int stage, room, soll; const char *name; } k[] = {
        { 0, 0x04, 1, "ROOM1041" }, { 0, 0x06, 1, "ROOM1061" }, { 0, 0x12, 1, "ROOM1121" }, { 0, 0x13, 1, "ROOM1131" },
        { 0, 0x03, 1, "ROOM1031" }, { 0, 0x14, 0, "ROOM1141 (eigene Musik MAIN16)" }, { 1, 0x00, 0, "STAGE2" },
    };
    for (unsigned i = 0; i < sizeof k / sizeof k[0]; i++) {
        const int e = re15_johnruf_r39_bgm_eintrag(k[i].stage, k[i].room);
        PRUEF((k[i].soll ? e == RE15_R39P_BGM_EINTRAG : e < 0), "Weiche %s -> %s", k[i].name, k[i].soll ? "0xFF01" : "Tabelle (-1)");
    }
    (void)re15_johnruf_r39_bgm_eintrag(0, 0x13);
    PRUEF(re15_johnruf_r39_main_gesperrt(0, 2, 0) == 1 && re15_johnruf_r39_main_gesperrt(1, 1, 0) == 0,
          "Sce_bgm_control: Slot 0 gesperrt, Slot 1 (SUB) frei, solange 0xFF01 geliefert ist");
    (void)re15_johnruf_r39_bgm_eintrag(0, 0x14);
    PRUEF(re15_johnruf_r39_main_gesperrt(0, 2, 0) == 0, "in ROOM1141 gilt der Stopp sub01 @0x00C06 (nicht gesperrt)");
    /* Kartenziel aktiv, solange 1141 nicht betreten ist */
    {
        int pg = -1, rc = -1, zp = -1, zr = -1;
        (void)re15_map_hint_ziel(re15_map_hint_eintrag_fuer(RE15_R39P_RAUM_1041, RE15_R39P_RAUM_1141), &zp, &zr);
        int gef = 0;
        for (int i = 0; i < 8; i++) if (re15_map_ziel_aktiv_n(i, &pg, &rc) && pg == zp && rc == zr) gef = 1;
        PRUEF(gef, "normale Karte: ROOM1141 (Blatt %d Rechteck %d) als Ziel markiert, solange nicht betreten", zp, zr);
    }
    /* Kampf: (4,0xF0)=0 (Szene begonnen, sub02 @0x00C18), (4,0xF2)=1 (Gorillas leben, @0x00C1C) -> Phase bleibt */
    re15_game_flag_set(4, 0xF0, 0); re15_game_flag_set(4, 0xF2, 1);
    re15_johnruf_r39_tick();
    PRUEF(flag(9, RE15_R39P_BIT_PANIK), "waehrend des Gorilla-Kampfs ((4,0xF2)=1) bleibt Bit 97");
    /* Rettung: sub01 @0x00C0C Set(4,0xF2)=0 */
    const int s0 = g_test_bgm_start_count;
    re15_game_flag_set(4, 0xF2, 0);
    re15_johnruf_r39_tick();
    PRUEF(!flag(9, RE15_R39P_BIT_PANIK) && !re15_johnruf_r39_fenster(), "John gerettet: Bit 97 = 0, Fenster zu");
    PRUEF(g_test_bgm_start_count == s0 + 1 && re15_johnruf_r39_bgm_eintrag(0, 0x04) < 0,
          "Tabellen-Musik angestossen (einmal), Weiche liefert -1 (\"Der Rest bleibt normal\")");
    {
        int pg = -1, rc = -1, zp = -1, zr = -1, gef = 0;
        (void)re15_map_hint_ziel(re15_map_hint_eintrag_fuer(RE15_R39P_RAUM_1041, RE15_R39P_RAUM_1141), &zp, &zr);
        for (int i = 0; i < 8; i++) if (re15_map_ziel_aktiv_n(i, &pg, &rc) && pg == zp && rc == zr) gef = 1;
        PRUEF(!gef, "nach der Rettung kein Kartenziel ROOM1141 mehr (\"gezeigt\" = Bit 97)");
    }
    /* Laden mitten in der Phase: Bit 97 gesetzt, (4,0xF0)=1 -> Fenster oeffnet nach dem ersten Spielbild-Lauf der VM */
    re15_game_state_init();
    re15_game_flag_set(4, 0xF0, 1); re15_game_flag_set(9, RE15_R39P_BIT_PANIK, 1);
    (void)room_boot(0x1061, 26000, 25300, 2048);
    const int s1 = g_test_bgm_start_count;
    for (int f = 0; f < 3; f++) bild(0, 0);
    PRUEF(re15_johnruf_r39_fenster() && g_test_bgm_start_count == s1 + 1 && g_test_bgm_start_room == 0x06,
          "geladener Stand in der Panikphase (ROOM1061): Fenster auf, MAIN01 angestossen (Raum-Byte 0x06)");
}

/* ================================================================== ruf1131 */
static void elza_1131(int f0, int b97)
{
    re15_game_state_init();
    g_gameflow.character = 4;
    re15_game_flag_set(4, 0xF0, f0 ? 1 : 0);
    re15_game_flag_set(9, RE15_R39P_BIT_PANIK, b97 ? 1 : 0);
    (void)room_boot(0x1131, -2200, -2150, 0);
}

static void teil_ruf1131(void)
{
    printf("== ruf1131 ==\n");
    /* neue Phase: die Flanke 0 -> 1 von Bit 97 im Tick setzt den 1131-Ruf zurueck */
    re15_game_state_init();
    re15_johnruf_r39_tick();
    re15_game_flag_set(4, 0xF0, 1); re15_game_flag_set(9, RE15_R39P_BIT_PANIK, 1);
    re15_johnruf_r39_tick();
    PRUEF(!re15_johnruf_r39_ruf1131_gespielt(), "Flanke Bit 97: der 1131-Ruf steht aus");
    elza_1131(1, 0);
    PRUEF(!faden_im_programm(1), "ohne Panikphase: kein Ruf in 1131");
    elza_1131(0, 1);
    PRUEF(!faden_im_programm(1), "(4,0xF0)=0 (John schon bei Elza): kein Ruf");
    elza_1131(1, 1);
    PRUEF(faden_im_programm(1), "Panikphase, John noch draussen: RUF1131 laeuft");
    for (int f = 0; f < 200; f++) bild(0, 0);
    for (int i = 0; i < s_n_st; i++) printf("    Stimme Bild %d Id %d Xa_vol %d\n", s_st[i].bild, s_st[i].id, s_st[i].xa);
    PRUEF(s_n_st == 1 && s_st[0].id == RE15_R39P_HILFE_ID0 + RE15_R39P_RUF1131 - 1 && s_st[0].xa == RE15_R39P_XA_LAUTER,
          "ein Ruf help02 mit Xa_vol %d (lauter als %d in 1041)", RE15_R39P_XA_LAUTER, RE15_R39P_XA_LEISE);
    PRUEF(!flag(2, 7) && re15_johnruf_r39_ruf1131_gespielt() && re15_johnruf_r39_xa_vol() == RE15_R39P_XA_NORMAL,
          "keine Pad-Sperre, Ruf vermerkt, Xa_vol zurueck auf 120");
    (void)room_boot(0x1131, -2200, -2150, 0);
    for (int f = 0; f < 200; f++) bild(0, 0);
    PRUEF(!faden_im_programm(1) && s_n_st == 0, "Wiederbetreten 1131 in derselben Phase: kein zweiter Ruf");
}

/* ================================================================== leon */
static void teil_leon(void)
{
    printf("== leon ==\n");
    re15_game_state_init();
    g_gameflow.character = 0;
    re15_game_flag_set(4, 0xF0, 1); re15_game_flag_set(3, 0x5F, 1);
    (void)room_boot(0x1040, -24000, 16500, 2048);
    for (int f = 0; f < 200; f++) bild(0, 0);
    PRUEF(!faden_im_programm(0) && s_n_st == 0 && s_hint_nr < 0 && !flag(9, RE15_R39P_BIT_PANIK),
          "Leon ROOM1040 mit denselben Flags: kein Programm, kein Ruf, keine Karte, Bit 97 bleibt 0");
    PRUEF(re15_johnruf_r39_bgm_eintrag(0, 0x04) < 0 && !re15_johnruf_r39_fenster(), "Weiche -1 (Leons Musik unveraendert)");
}

/* ================================================================== Nachbesserung 1: Tuersperren (re15_tuersperre_r39.h)
 * Nutzer-Nachtrag 12: Tuer 1041 -> 1031 und 1051 -> 10A1 in der Panikphase = "I have to rescue John! He is in danger!";
 * die Tuer in ROOM10B1 (Kanaldeckel -> ROOM1261) fuer Elza IMMER zu, auch in der Sherry-Szene. */
static void elza_raum(uint16_t room, int32_t x, int32_t z, int16_t rot, int b97, const int *bits)
{
    re15_game_state_init();
    re15_inv_init();
    g_gameflow.character = (room & 1u) ? 4 : 0;
    re15_game_flag_set(9, RE15_R39P_BIT_PANIK, b97 ? 1 : 0);
    re15_game_flag_set(3, 0x5F, 1); re15_game_flag_set(4, 0xF1, 1);
    for (int i = 0; bits && bits[i] >= 0; i += 2) re15_game_flag_set((uint8_t)bits[i], (uint8_t)bits[i + 1], 1);
    (void)room_boot(room, x, z, rot);
}

static int stimmen_mit_id(int id)
{
    int n = 0;
    for (int i = 0; i < s_n_st; i++) if (s_st[i].id == id) n++;
    return n;
}

/* EIN Aktionsdruck (Viereck) am ECHTEN Weg (Muster test_r38_tagebuch.c druck): Spielschritt mit der Taste -> AOT-Scan ->
 * Tuer-Handler (Raumwechsel angefordert, re15_room_request_change) bzw. MESSAGE-Handler (Text). Danach 40 Bilder. */
typedef struct { int raumwechsel; unsigned ziel; int msg; } tuer_erg_t;
static tuer_erg_t tuer_druecken(int32_t x, int32_t z, int16_t rot)
{
    tuer_erg_t r = { 0, 0u, -1 };
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_room_change.pending = 0;
    pl->x = x; pl->z = z; pl->rot_y = rot;
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    for (int f = 0; f < 40; f++) {
        if (g_scd.message_active && r.msg < 0) r.msg = (int)g_scd.message_id;
        if (g_room_change.pending) break;
        bild(0, 0);
    }
    if (g_room_change.pending) { r.raumwechsel = 1; r.ziel = g_room_change.room_id; }
    return r;
}

static int slot_ist(int slot, int typ, int ev)
{
    const re15_aot_t *a = &g_aot.slots[slot];
    return a->active && (int)a->type == typ && (ev < 0 || (int)a->event_id == ev);
}

static int rdt_nachrichten(void)   /* Zahl der Original-Nachrichten des geladenen Raums (Tabelle: erstes u16 / 2) */
{
    if (!s_rdt.messages || s_rdt.messages_size < 2) return 0;
    return (int)((s_rdt.messages[0] | (s_rdt.messages[1] << 8)) / 2);
}

/* ================================================================== tuertexte */
static void teil_tuertexte(void)
{
    printf("== tuertexte ==\n");
    char t[512];
    int n1 = 0, n2 = 0, n3 = 0;
    const uint8_t *r1 = re15_tuersperre_r39_text(RE15_R39P_TS_RAUM_1041, &n1);
    const uint8_t *r2 = re15_tuersperre_r39_text(RE15_R39P_TS_RAUM_1051, &n2);
    const uint8_t *d  = re15_tuersperre_r39_text(RE15_R39P_TS_RAUM_10B1, &n3);
    t[0] = 0; if (r1 && n1 > 0) re15_msg_decode_text(r1, (size_t)n1, t, sizeof t);
    printf("    1041/1051: \"%s\"\n", t);
    PRUEF(!strcmp(t, "I have to rescue John! He is in danger!"), "Satz = Nutzer-Wortlaut Nachtrag 12");
    PRUEF(r1 && r2 && n1 == n2 && !memcmp(r1, r2, (size_t)n1), "1041 und 1051 zeigen denselben Satz");
    {
        int z08 = 0, z05 = 0;
        for (int i = 0; r1 && i < n1; i++) { z08 += r1[i] == 0x08; z05 += r1[i] == 0x05; }
        PRUEF(n1 > 6 && r1[0] == 0x04 && r1[1] == 0x02 && z08 == 1 && z05 == 0 && r1[n1 - 2] == 0x01 && r1[n1 - 1] == 0x00,
              "Erzaehlform `04 02 <Zeile> 08 <Zeile> 01 00` ohne Sprecherkopf (Form ROOM1090 msg 7 @0x28E6, 1141/11C0-Sperren)");
    }
    /* 10B1 (Kanaldeckel) zurueckgestellt (Abnahme 1 M1, Softlock): kein Eintrag, kein Text */
    PRUEF(d == NULL && n3 == 0, "10B1: keine Sperre eingetragen (Kanaldeckel zurueckgestellt, Abnahme 1 M1)");
    PRUEF(RE15_R39P_TS_MSG_1041 == 41 && RE15_R39P_TS_MSG_1051 == 40 && RE15_R39P_TS_MSG_10B1 == 40,
          "Ids 1041 = 41, 1051 = 40 (Orchestrator Nachbesserung 1), 10B1 = 40");
    {   /* Ids liegen hinter den Original-Nachrichten der Raeume */
        int m41 = -1, m51 = -1, mb1 = -1;
        if (rdt_laden(0x1041) == 0) m41 = rdt_nachrichten();
        if (rdt_laden(0x1051) == 0) m51 = rdt_nachrichten();
        if (rdt_laden(0x10B1) == 0) mb1 = rdt_nachrichten();
        PRUEF(m41 > 0 && m41 <= RE15_R39P_MSG_SATZ && m51 > 0 && m51 < RE15_R39P_TS_MSG_1051 && mb1 > 0 && mb1 < RE15_R39P_TS_MSG_10B1,
              "keine Kollision mit Original-Ids (ROOM1041 %d, ROOM1051 %d, ROOM10B1 %d Nachrichten)", m41, m51, mb1);
    }
}

/* ================================================================== tuer1041 */
static void teil_tuer1041(void)
{
    printf("== tuer1041 ==\n");
    static const int k_gerettet[] = { -1 };
    tuer_erg_t r;
    /* ohne Panikphase (vor dem Ruf bzw. nach der Rettung: Bit 97 = 0, (4,0xF0) = 0, (4,0xF2) = 0) */
    elza_raum(0x1041, -20800, -3800, 0, 0, k_gerettet);
    PRUEF(re15_tuersperre_r39_anker_ok(RE15_R39P_TS_RAUM_1041), "Anker ROOM1041 main00 @0x010B8 (Door_aot_set Slot 1) wie ausgeliefert");
    PRUEF(slot_ist(RE15_R39P_TS_SLOT_1041, RE15_AOT_TYPE_DOOR, -1) &&
          g_aot.door_params[RE15_R39P_TS_SLOT_1041].dest_room == RE15_R39P_TS_ZIEL_1041,
          "ohne Panikphase: Slot 1 ist die Tuer nach Raum 0x03 (ROOM1031)");
    r = tuer_druecken(-20800, -3800, 0);
    PRUEF(r.raumwechsel && r.ziel == 0x1031u && r.msg < 0, "ohne Panikphase: Aktion -> Raumwechsel nach ROOM%04X", r.ziel);
    /* in der Panikphase */
    elza_raum(0x1041, -20800, -3800, 0, 1, NULL);
    PRUEF(slot_ist(RE15_R39P_TS_SLOT_1041, RE15_AOT_TYPE_MESSAGE, RE15_R39P_TS_MSG_1041) &&
          (g_aot.slots[RE15_R39P_TS_SLOT_1041].sce_flags & 0x7F) == RE15_R39P_TS_SAT &&
          g_aot.slots[RE15_R39P_TS_SLOT_1041].pause_mask16 == RE15_R39P_TS_MASKE &&
          g_aot.door_params[RE15_R39P_TS_SLOT_1041].dest_room == RE15_R39P_TS_ZIEL_1041,
          "Panikphase beim Betreten: Slot 1 = MESSAGE msg 41, sat 0x31, Maske 0xffff (Form ROOM11B1 @0x011C0); Ziel bleibt im Satz");
    s_n_st = 0;
    r = tuer_druecken(-20800, -3800, 0);
    PRUEF(!r.raumwechsel && r.msg == RE15_R39P_TS_MSG_1041 && stimmen_mit_id(RE15_R39P_TS_MSG_1041) == 0,
          "Panikphase: Aktion -> Text msg %d ohne Stimme (Nachtrag 13: einfacher Raumtext), KEIN Raumwechsel", r.msg);
    /* Beginn der Phase IN 1041 (RUF1041 -> Set(9,97)=1): Tuer wird im selben Bild Text, nie freies Bild mit offener Tuer */
    elza_1041(1, 0);
    int b97 = -1, offen_frei = 0, gesperrt_bei = -1;
    for (int f = 0; f < 4000; f++) {
        bild(0, 0);
        const int p97 = flag(9, RE15_R39P_BIT_PANIK);
        if (p97 && b97 < 0) b97 = s_bild;
        if (p97 && !flag(2, 7) && slot_ist(RE15_R39P_TS_SLOT_1041, RE15_AOT_TYPE_DOOR, -1)) offen_frei++;
        if (gesperrt_bei < 0 && slot_ist(RE15_R39P_TS_SLOT_1041, RE15_AOT_TYPE_MESSAGE, RE15_R39P_TS_MSG_1041)) gesperrt_bei = s_bild;
        if (b97 >= 0 && s_bild > b97 + 30) break;
    }
    printf("    Bit 97 ab Bild %d, Slot 1 Text ab Bild %d, freie Bilder mit offener Tuer %d\n", b97, gesperrt_bei, offen_frei);
    PRUEF(b97 >= 0 && gesperrt_bei == b97 && offen_frei == 0,
          "RUF1041 setzt Bit 97 -> Tuer im selben Spielbild Text (Tick vor dem AOT-Scan), kein freies Bild mit offener Tuer");
    PRUEF(gesperrt_bei < 0 || slot_ist(RE15_R39P_TS_SLOT_1041, RE15_AOT_TYPE_MESSAGE, RE15_R39P_TS_MSG_1041),
          "nach dem Beginn bleibt die Tuer Text");
    r = tuer_druecken(-20800, -3800, 0);
    PRUEF(!r.raumwechsel && r.msg == RE15_R39P_TS_MSG_1041, "danach an der Tuer: Text msg %d, kein Raumwechsel", r.msg);
    /* Leon ROOM1040 unveraendert (auch mit Bit 97) */
    elza_raum(0x1040, -20800, -3800, 0, 1, NULL);
    PRUEF(slot_ist(RE15_R39P_TS_SLOT_1041, RE15_AOT_TYPE_DOOR, -1), "Leon ROOM1040: Slot 1 bleibt Tuer");
}

/* ================================================================== tuer1051 */
static void teil_tuer1051(void)
{
    printf("== tuer1051 ==\n");
    static const int k_rollo[] = { 3, 0x79, -1 };   /* Rollo 1051 offen (sub02 @0x00CD6) */
    tuer_erg_t r;
    elza_raum(0x1051, 16456, -13709, 0, 0, k_rollo);
    PRUEF(re15_tuersperre_r39_anker_ok(RE15_R39P_TS_RAUM_1051), "Anker ROOM1051 main00 @0x00B5A (Door_aot_set Slot 4) wie ausgeliefert");
    PRUEF(slot_ist(RE15_R39P_TS_SLOT_1051, RE15_AOT_TYPE_DOOR, -1) &&
          g_aot.door_params[RE15_R39P_TS_SLOT_1051].dest_room == RE15_R39P_TS_ZIEL_1051,
          "ohne Panikphase: Slot 4 ist die Tuer nach Raum 0x0A (ROOM10A1)");
    r = tuer_druecken(16456, -13709, 0);
    PRUEF(r.raumwechsel && r.ziel == 0x10A1u, "ohne Panikphase: Aktion -> Raumwechsel nach ROOM%04X", r.ziel);
    elza_raum(0x1051, 16456, -13709, 0, 1, k_rollo);
    PRUEF(slot_ist(RE15_R39P_TS_SLOT_1051, RE15_AOT_TYPE_MESSAGE, RE15_R39P_TS_MSG_1051) &&
          (g_aot.slots[RE15_R39P_TS_SLOT_1051].sce_flags & 0x7F) == RE15_R39P_TS_SAT &&
          g_aot.slots[RE15_R39P_TS_SLOT_1051].pause_mask16 == RE15_R39P_TS_MASKE,
          "Panikphase: Slot 4 = MESSAGE msg 40, sat 0x31, Maske 0xffff");
    s_n_st = 0;
    r = tuer_druecken(16456, -13709, 0);
    PRUEF(!r.raumwechsel && r.msg == RE15_R39P_TS_MSG_1051 && stimmen_mit_id(RE15_R39P_TS_MSG_1051) == 0,
          "Panikphase: Aktion -> Text msg %d ohne Stimme (Nachtrag 13: einfacher Raumtext), KEIN Raumwechsel", r.msg);
    for (int f = 0; f < 60; f++) bild(0, 0);
    PRUEF(slot_ist(RE15_R39P_TS_SLOT_1051, RE15_AOT_TYPE_MESSAGE, RE15_R39P_TS_MSG_1051), "Sperre bleibt in der Phase stehen");
    /* Leon ROOM1050: Slot 4 gehoert dort adaruf_1050.c (vor der Ada-Rettung (3,0xBB)=0) und sperre1050_r37.c ((9,71)..(9,73));
     * nach der Ada-Rettung ohne Chief-Plan ist er die Original-Tuer - Bit 97 aendert daran nichts. */
    static const int k_leon[] = { 3, 0x79, 3, 0xBB, -1 };
    elza_raum(0x1050, 16456, -13709, 0, 1, k_leon);
    PRUEF(slot_ist(RE15_R39P_TS_SLOT_1051, RE15_AOT_TYPE_DOOR, -1), "Leon ROOM1050 (Ada gerettet, Bit 97 = 1): Slot 4 bleibt Tuer");
}

/* ================================================================== tuer10b1 */
static void teil_tuer10b1(void)
{
    printf("== tuer10b1 ==\n");
    static const int k_vor[]   = { -1 };
    static const int k_szene[] = { 3, 0x82, -1 };                                  /* ROOM1171 sub02 @0x013A4 */
    static const int k_nach[]  = { 3, 0x83, 3, 0x41, 3, 0x46, -1 };              /* sub04 @0x0195E/@0x01962, ROOM2001 @0x01958 */
    static const int k_gefunden[] = { 3, 0x83, 3, 0x41, 3, 0x44, 9, 97, -1 };   /* ROOM11B1 sub03 @0x0143C, mit Bit 97 */
    /* (3,0x82): John steht nach sub00 @0x017F6/@0x017FE am Deckel (-21422,3307) und die Szene startet, sobald Elza Cut 7
     * betritt (sub01 @0x0181E) - ein Druck "vor" ihr ist im Spiel nicht erreichbar (im Prueflauf schob Johns Koerper Elza
     * auf (-20518,3246), Vorwaertspunkt ausserhalb des Rechtecks). Dort zaehlen Sperre + Szenen-Huelle (Teil tuerszene). */
    static const struct { const int *bits; const char *was; int druck; } z[] = {
        { k_vor, "vor der Szene", 1 }, { k_szene, "Szene steht an (3,0x82)", 0 }, { k_nach, "nach der Szene, Sherry-Suche", 1 },
        { k_gefunden, "Sherry-Flags + Bit 97", 1 },
    };
    for (unsigned i = 0; i < sizeof z / sizeof z[0]; i++) {
        elza_raum(0x10B1, -21159, 3289, 3072, 0, z[i].bits);
        const int ok = re15_tuersperre_r39_anker_ok(RE15_R39P_TS_RAUM_10B1) &&
                       slot_ist(RE15_R39P_TS_SLOT_10B1, RE15_AOT_TYPE_MESSAGE, RE15_R39P_TS_MSG_10B1) &&
                       g_aot.door_params[RE15_R39P_TS_SLOT_10B1].dest_room == RE15_R39P_TS_ZIEL_10B1 &&
                       slot_ist(0, RE15_AOT_TYPE_DOOR, -1) && g_aot.door_params[0].dest_room == 0x17;
        if (!z[i].druck) {
            const uint8_t *orig = s_rdt.sub_scd[RE15_R39P_TS_SUB_10B1];
            int n = 0;
            const uint8_t *huelle = re15_tuersperre_r39_szene(&n);
            PRUEF(ok && orig && re15_tuersperre_r39_evt_exec(RE15_R39P_TS_RAUM_10B1, RE15_R39P_TS_SUB_10B1, orig) == huelle,
                  "%s: Deckel Slot 1 = Text msg 40 (Ziel 0x26 bleibt im Satz, Leiter Slot 0 -> 0x17 bleibt Tuer); Evt_exec sub04 "
                  "-> Szenen-Huelle SZENE10B1", z[i].was);
            continue;
        }
        s_n_st = 0;
        tuer_erg_t r = tuer_druecken(-21159, 3289, 3072);
        PRUEF(ok && !r.raumwechsel && r.msg == RE15_R39P_TS_MSG_10B1 && stimmen_mit_id(RE15_R39P_TS_MSG_10B1) == 0,
              "%s: Deckel Slot 1 = Text msg 40 (Ziel 0x26 bleibt im Satz, Leiter Slot 0 -> 0x17 bleibt Tuer), Aktion -> Text "
              "msg %d, kein Raumwechsel, keine Stimme", z[i].was, r.msg);
    }
    /* Leon ROOM10B0 unveraendert */
    static const uint8_t k_leon_sub[2] = { 0x01, 0x00 };
    elza_raum(0x10B0, -21159, 3289, 3072, 1, NULL);
    PRUEF(slot_ist(RE15_R39P_TS_SLOT_10B1, RE15_AOT_TYPE_DOOR, -1) &&
          re15_tuersperre_r39_evt_exec(0x10B0, RE15_R39P_TS_SUB_10B1, k_leon_sub) == k_leon_sub,
          "Leon ROOM10B0: Deckel bleibt Tuer, kein Programm");
    tuer_erg_t r = tuer_druecken(-21159, 3289, 3072);
    PRUEF(r.raumwechsel && r.ziel == 0x1260u, "Leon ROOM10B0: Aktion -> Raumwechsel nach ROOM%04X", r.ziel);
}

/* ================================================================== tuerszene (ROOM10B1 sub04 mit gesperrtem Deckel) */
typedef struct { int text, text_bild, pad, balken, raumwechsel, frei, ende; } szene_erg_t;
static szene_erg_t szene_10b1(int mit_huelle)
{
    static const int k_szene[] = { 3, 0x82, -1 };
    szene_erg_t e = { 0, -1, -1, -1, 0, 0, -1 };
    elza_raum(0x10B1, -21159, 3289, 3072, 0, k_szene);   /* (3,0x82)=1 beim Aufbau: sub00 @0x017F6 stellt John an den Deckel */
    int john = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == 0x49) john = 1;
    if (!john) printf("    (John Typ 0x49 nach dem Aufbau nicht aktiv)\n");
    /* Die Szene startet hier von Hand (wie op_evt_exec); sub01 (je Bild) darf sie nicht ein zweites Mal starten. sub04
     * loescht (3,0x82) als erstes Opcode selbst (@0x0195A) - hier vorweg, weil der Ausloeser umgangen wird. */
    re15_game_flag_set(3, 0x82, 0);
    g_room_change.pending = 0;
    const uint8_t *orig = s_rdt.sub_scd[RE15_R39P_TS_SUB_10B1];
    const uint8_t *p = mit_huelle ? re15_tuersperre_r39_evt_exec(RE15_R39P_TS_RAUM_10B1, RE15_R39P_TS_SUB_10B1, orig) : orig;
    int slot = -1;
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) if (!g_scd.threads[s].active) { slot = s; break; }
    if (slot < 0 || !p || scd_thread_start(slot, p) != 0) return e;   /* wie op_evt_exec (cond 0xFF: erster freier Faden) */
    int zu = -1;
    for (int f = 0; f < 4000; f++) {
        uint16_t taste = 0;
        if (g_scd.message_active && g_scd.message_id == RE15_R39P_TS_MSG_10B1) {
            if (!e.text) { e.text = 1; e.text_bild = s_bild; }
            if ((f % 15) == 0) taste = RE15_PAD_BIT_SQUARE;   /* Text lesen und schliessen */
        } else if (g_scd.message_active && (f % 15) == 0) {
            taste = RE15_PAD_BIT_SQUARE;                      /* Szenen-Zeilen 0..5 weiterschalten */
        }
        bild(taste, taste);
        if (g_room_change.pending) e.raumwechsel = 1;
        if (e.text && zu < 0 && !g_scd.message_active) zu = s_bild;
        if (zu >= 0 && s_bild > zu + 60) break;
    }
    e.pad = flag(2, 7); e.balken = flag(1, 0x1B);
    e.frei = !re15_cine_active() && g_scd.player_mode == 0;
    e.ende = !g_scd.threads[slot].active;
    return e;
}

static void teil_tuerszene(void)
{
    printf("== tuerszene ==\n");
    int n = 0;
    const uint8_t *p = re15_tuersperre_r39_szene(&n);
    {
        uint8_t *r51 = NULL;
        size_t sz = 0;
        char rp[600];
        snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1051.RDT", RE15_ASSET_PSX_DIR);
        r51 = slurp(rp, &sz);
        PRUEF(r51 && sz > 0xDA4 && n == 22 && p[0] == 0x18 && p[1] == RE15_R39P_TS_SUB_10B1 && !memcmp(p + 2, r51 + 0x00CCC, 2) &&
              !memcmp(p + 4, r51 + 0x00D90, 16) && !memcmp(p + 20, r51 + 0x00DA2, 2),
              "SZENE10B1 = Gosub sub04 + ROOM1051 sub02 @0x00CCC (Evt_next) + @0x00D90..@0x00D9F (Pad frei, Balken weg, Cut_auto, "
              "Plc_ret) + @0x00DA2 Evt_end, bytegleich");
        free(r51);
    }
    /* Ohne Huelle (nur die Original-Szene): der Deckel bleibt zu, aber die Szene loest Pad-Sperre und Balken nie */
    szene_erg_t o = szene_10b1(0);
    printf("    ohne Huelle: Text %d (Bild %d), Raumwechsel %d, (2,7)=%d, (1,0x1B)=%d, frei %d\n", o.text, o.text_bild, o.raumwechsel,
           o.pad, o.balken, o.frei);
    PRUEF(o.text && !o.raumwechsel && o.pad == 1 && o.balken == 1 && !o.frei,
          "MESSUNG: Original-sub04 allein zeigt den Deckel-Text (Aot_on @0x01A8E -> MESSAGE), laesst Pad-Sperre @0x01966 und "
          "Balken @0x0196A aber stehen (Elza eingefroren) -> Huelle noetig");
    szene_erg_t m = szene_10b1(1);
    printf("    mit Huelle:  Text %d (Bild %d), Raumwechsel %d, (2,7)=%d, (1,0x1B)=%d, frei %d, Programm zu Ende %d\n", m.text,
           m.text_bild, m.raumwechsel, m.pad, m.balken, m.frei, m.ende);
    PRUEF(m.text && !m.raumwechsel, "Sherry-Szene: am Ende Deckel-Text msg 40 statt Raumwechsel nach ROOM1261");
    PRUEF(m.pad == 0 && m.balken == 0 && m.frei && m.ende, "nach dem Text: Pad frei, Balken weg, Steuerung zurueck, Programm zu Ende");
    PRUEF(flag(3, 0x83) && flag(3, 0x41) && !flag(3, 0x82), "Szenen-Flags wie im Original gesetzt ((3,0x83)/(3,0x41), (3,0x82)=0)");
    PRUEF(slot_ist(RE15_R39P_TS_SLOT_10B1, RE15_AOT_TYPE_MESSAGE, RE15_R39P_TS_MSG_10B1), "Deckel bleibt danach Text");
}

/* ================================================================== pegel (Werkzeug fuer die Integrations-Riegel)
 * test_r39_johnruf1041 pegel <cap.raw> <wav> <capTick> <xa_vol>
 * Misst im RE15_AUDIO_CAP_SYNC-Mitschnitt (Stereo int16, 44100 Hz, 1470 Werte je Spielbild) die Verstaerkung einer
 * Sprachzeile, die laut debug.log ("[voice] Start ROOM.. Id .. CD_VOL .. capTick N", audio_pc.c re15_voice_play) bei Takt N
 * startet. Referenz = die Datei genau so, wie der Port sie laedt (audio_pc.c re15_voice_load_clip: lineares Resampling Q16
 * auf 44100 Hz, Spitzen-Boden 14745 mit Deckel 4x, dann re15_stimmen_pegel_r39 Stufe 2). Gemessen wird
 * g = <Mitschnitt, Referenz> / <Referenz, Referenz> am besten Versatz innerhalb +-3 Takten; Soll = xa_vol / 120 (RE2
 * FUN_80013ac8 relativ zum Standard 120 = CD_VOL 0x3FFF, Mischer (smp * CD_VOL) >> 14). Rueckgabe 0 = |g/Soll - 1| <= 10 %. */
static int16_t *wav_laden(const char *p, long *n_out)
{
    size_t n = 0;
    uint8_t *b = slurp(p, &n);
    if (!b || n < 44) { free(b); return NULL; }
    int ch = 0, bits = 0; long rate = 0; const uint8_t *data = NULL; long dlen = 0;
    for (size_t o = 12; o + 8 <= n;) {
        const uint32_t len = (uint32_t)b[o + 4] | (uint32_t)b[o + 5] << 8 | (uint32_t)b[o + 6] << 16 | (uint32_t)b[o + 7] << 24;
        if (!memcmp(b + o, "fmt ", 4)) { ch = b[o + 10]; rate = (long)(b[o + 12] | b[o + 13] << 8 | b[o + 14] << 16); bits = b[o + 22]; }
        if (!memcmp(b + o, "data", 4)) { data = b + o + 8; dlen = (long)len; if (o + 8 + len > n) dlen = (long)(n - o - 8); break; }
        o += 8 + len + (len & 1);
    }
    if (!data || bits != 16 || ch < 1 || rate <= 0) { free(b); return NULL; }
    const long src_n = dlen / 2 / ch;
    const int16_t *src = (const int16_t *)data;
    const long out_n = (long)((int64_t)src_n * 44100 / rate);
    int16_t *pcm = (int16_t *)malloc((size_t)out_n * sizeof(int16_t));
    if (!pcm) { free(b); return NULL; }
    for (long i = 0; i < out_n; i++) {              /* audio_pc.c re15_voice_load_clip, Resampling Q16 */
        const int64_t sp = (int64_t)i * rate * 65536 / 44100;
        const int64_t si = sp >> 16; const int frac = (int)(sp & 0xffff);
        const int64_t s0 = si * ch, s1 = (si + 1 < src_n ? si + 1 : si) * ch;
        int32_t a, c;
        if (ch == 1) { a = src[s0]; c = src[s1]; } else { a = (src[s0] + src[s0 + 1]) / 2; c = (src[s1] + src[s1 + 1]) / 2; }
        pcm[i] = (int16_t)(a + (((c - a) * frac) >> 16));
    }
    free(b);
    int32_t peak = 1;
    for (long i = 0; i < out_n; i++) { int32_t a = pcm[i] < 0 ? -pcm[i] : pcm[i]; if (a > peak) peak = a; }
    if (peak < 14745) {                             /* audio_pc.c Spitzen-Boden ~45 % FS, Deckel 4x */
        int g = 14745 * 256 / peak; if (g > 4 * 256) g = 4 * 256;
        for (long i = 0; i < out_n; i++) {
            int32_t s = (pcm[i] * g) >> 8;
            if (s > 32767) s = 32767; else if (s < -32768) s = -32768;
            pcm[i] = (int16_t)s;
        }
    }
    (void)re15_stimmen_pegel_r39(pcm, out_n, NULL, NULL);   /* Runde 39 Spur A Stufe 2 (wie der Port) */
    *n_out = out_n;
    return pcm;
}

static int teil_pegel(int argc, char **argv)
{
    if (argc < 6) { printf("pegel <cap.raw> <wav> <capTick> <xa_vol>\n"); return 2; }
    size_t cn = 0;
    uint8_t *cb = slurp(argv[2], &cn);
    long rn = 0;
    int16_t *ref = wav_laden(argv[3], &rn);
    const long takt = atol(argv[4]);
    const int xa = atoi(argv[5]);
    if (!cb || !ref || rn <= 0) { printf("  FEHLER: Mitschnitt oder WAV nicht lesbar\n"); return 1; }
    const int16_t *cs = (const int16_t *)cb;
    const long cap_n = (long)(cn / 4);
    double rr = 0;
    for (long i = 0; i < rn; i++) rr += (double)ref[i] * ref[i];
    double best_rho = -2, best_g = 0; long best_o = 0;
    for (long d = -3 * 1470; d <= 3 * 1470; d++) {
        const long o = takt * 1470 + d;
        if (o < 0 || o + rn > cap_n) continue;
        double cr = 0, cc = 0;
        for (long i = 0; i < rn; i++) {
            const double m = 0.5 * ((double)cs[2 * (o + i)] + (double)cs[2 * (o + i) + 1]);
            cr += m * ref[i]; cc += m * m;
        }
        const double rho = cr / (sqrt(cc * rr) + 1e-9);
        if (rho > best_rho) { best_rho = rho; best_g = cr / (rr + 1e-9); best_o = d; }
        if (d % 7 == 0 && d < 3 * 1470) d += 6;   /* grob in Schritten von 7 Werten ... */
    }
    /* ... dann fein um den besten Wert */
    {
        const long mitte = best_o;
        for (long d = mitte - 7; d <= mitte + 7; d++) {
            const long o = takt * 1470 + d;
            if (o < 0 || o + rn > cap_n) continue;
            double cr = 0, cc = 0;
            for (long i = 0; i < rn; i++) {
                const double m = 0.5 * ((double)cs[2 * (o + i)] + (double)cs[2 * (o + i) + 1]);
                cr += m * ref[i]; cc += m * m;
            }
            const double rho = cr / (sqrt(cc * rr) + 1e-9);
            if (rho > best_rho) { best_rho = rho; best_g = cr / (rr + 1e-9); best_o = d; }
        }
    }
    const double soll = (double)xa / 120.0;
    const double v = best_g / soll;
    printf("pegel %s Takt %ld Versatz %+ld Werte: rho %.3f g %.4f Soll %.4f (Xa_vol %d / 120) Verhaeltnis %.3f\n",
           argv[3], takt, best_o, best_rho, best_g, soll, xa, v);
    free(cb); free(ref);
    return (best_rho >= 0.3 && v > 0.9 && v < 1.1) ? 0 : 1;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "";
    /* Diese Riegel pruefen Spur P allein (Stand vor Spur Q, Pruefhaken elzazombies_r39.c @re15_elzazombies_r39_enable): sonst
     * startet Q auf jede hier gesetzte Flanke (9,97) ihre Montage. Die Kette P -> Q pruefen integration_r39_elzazombies_kette
     * (echter Ruf) und integration_r39_johnruf1041_a. */
    re15_elzazombies_r39_enable(0);
    if (!strcmp(teil, "pegel"))          return teil_pegel(argc, argv);
    if (!strcmp(teil, "texte"))          teil_texte();
    else if (!strcmp(teil, "programme")) teil_programme();
    else if (!strcmp(teil, "stimme"))    teil_stimme();
    else if (!strcmp(teil, "xa"))        teil_xa();
    else if (!strcmp(teil, "ablauf"))    teil_ablauf();
    else if (!strcmp(teil, "musik"))     teil_musik();
    else if (!strcmp(teil, "ruf1131"))   teil_ruf1131();
    else if (!strcmp(teil, "leon"))      teil_leon();
    else if (!strcmp(teil, "tuertexte")) teil_tuertexte();
    else if (!strcmp(teil, "tuer1041"))  teil_tuer1041();
    else if (!strcmp(teil, "tuer1051"))  teil_tuer1051();
    else if (!strcmp(teil, "tuer10b1"))  teil_tuer10b1();
    else if (!strcmp(teil, "tuerszene")) teil_tuerszene();
    else { printf("Teil? texte|programme|stimme|xa|ablauf|musik|ruf1131|leon|tuertexte|tuer1041|tuer1051|tuer10b1|tuerszene\n");
           return 2; }
    printf("%d Pruefungen, %d Fehler\n", g_n, g_fail);
    return g_fail ? 1 : 0;
}
