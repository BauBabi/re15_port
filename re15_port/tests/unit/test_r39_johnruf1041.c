/*
 * test_r39_johnruf1041.c — Runde 39 Spur P "johnruf1041": Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit allen
 * Installern, SCD-VM mit Opcode 0x5F, Nachrichten-FSM, re15_game_step mit Kartenhinweis-Schirm und Spielbild-Tick).
 * Dossier analysis/befunde_runde39/P_johnruf1041.md, Code include/re15_johnruf_r39.h + engine/src/johnruf_r39.c, Programme
 * engine/src/gen/johnruf_r39_programme.inc (tools/r39_johnruf/programme_bauen.py).
 *
 * Teile (Aufruf: test_r39_johnruf1041 <teil>):
 *   texte      msg 40 dekodiert = Wortlaut des Nutzers (Nachtrag 10, Schreibung Nachtrag 6 = derselbe Satz wie Spur J msg 50)
 *   programme  Programm-Bytes: Reihenfolge Rufe leise (Xa_vol 50) -> Drehen -> Xa_vol 120 -> msg 40 -> Karte (59) -> Bit 97 ->
 *              Steuerung zurueck; 1131: Xa_vol 70 -> help02 -> Pause >= Ruflaenge -> Xa_vol 120; Original-Zeilen bytegleich
 *              mit ROOM1041.RDT sub11; Kopf-Konstanten = Werkzeug
 *   stimme     Sprach-Ordner/Stamm: (1041|1131, 53..57) -> room1141/help01..05, alles andere unveraendert
 *   xa         Opcode 0x5F: nur in den Programmen RE2 Xa_vol (Wert < 0x80, pc += 2), sonst op_unknown (pc += 1); CD_VOL =
 *              0x3FFF * vol / 120; Raumaufbau setzt 120
 *   ablauf     ROOM1041 mit Elza am VM: Programm nur mit (4,0xF0)=1 und Bit 97=0; Pad-Sperre, Rufe leise, Drehung zur Treppe,
 *              msg 40 mit Standardlautstaerke, Kartenhinweis ROOM1041 -> ROOM1141 (Blatt/Rechteck der Zone), Faden parkt bis
 *              der Schirm zu ist, dann Bit 97 und Steuerung zurueck; Musikfenster + Raummusik-Anstoss; kein zweites Mal
 *   musik      Weiche/Sperre: 0xFF01 in STAGE1 ausser ROOM1141, nur im Fenster; Rettung ((4,0xF0)=0 und (4,0xF2)=0) loescht
 *              Bit 97 und stoesst die Tabellen-Musik an; waehrend des Kampfs ((4,0xF2)=1) bleibt sie; Kartenziel aktiv
 *   ruf1131    ROOM1131 in der Panikphase: Ruf help02 mit Xa_vol 70, einmal je Phase; ohne Bit 97 / nach der Rettung keiner
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
static int                s_shown = 0, s_bild = 0;
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
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = 0; s_bild = 0; s_n_st = 0; s_hint_nr = -1; s_hint_bild = -1;
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
              "RUF1131: help%02d mit Xa_vol %d (> %d in 1041, RE2 ROOM11D0 @0x01F7A)", RE15_R39P_RUF1131, xarf, RE15_R39P_XA_LEISE);
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

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "";
    if (!strcmp(teil, "texte"))          teil_texte();
    else if (!strcmp(teil, "programme")) teil_programme();
    else if (!strcmp(teil, "stimme"))    teil_stimme();
    else if (!strcmp(teil, "xa"))        teil_xa();
    else if (!strcmp(teil, "ablauf"))    teil_ablauf();
    else if (!strcmp(teil, "musik"))     teil_musik();
    else if (!strcmp(teil, "ruf1131"))   teil_ruf1131();
    else if (!strcmp(teil, "leon"))      teil_leon();
    else { printf("Teil? texte|programme|stimme|xa|ablauf|musik|ruf1131|leon\n"); return 2; }
    printf("%d Pruefungen, %d Fehler\n", g_n, g_fail);
    return g_fail ? 1 : 0;
}
