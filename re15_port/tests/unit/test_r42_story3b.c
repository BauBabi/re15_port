/*
 * test_r42_story3b.c — Runde 42 "story" (v0.8.31), Stufe STAGE3b: 30E0 in einem Besuch, Szenenende nach der letzten Zeile, Zeile in
 * 3070, Birkin-Clipende (T-03 3070-Auftritt, T-04 Hand im Aufzug 3080). Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit
 * allen Installern, SCD-VM, re15_game_step, Gegner-KI), Harness = test_r42_story3a.c. Belege include/re15_story3b_r42.h und
 * enemy_ai_common.c re15_birkin_anim_szene. PORT-STORY nach dem Konzept (3.2 L8-L10, L-11/L-12, F-08; 5 T-03/T-04, F-09).
 *
 * Teile (Aufruf: test_r42_story3b <teil>):
 *   texte      30E0 msg 10/11/13/16/17, 3070 msg 6: Untertitel mit Sprecherkopf und Farbe, Wortlaut nach dem Raumaufbau;
 *              Elzas 30E1/3071 ohne neue Texte
 *   programm   ein Besuch = sub02 [@0x00CB0,@0x00E54) + Glasbruch + 16/17 + Marvin + sub03 [@0x00E62,@0x00FB4) + Warten + Schluss;
 *              Zeilen 0..5,7,8,16,17,9..13; Altstand = sub03 mit Warten; 3070 eine Zeile
 *   besuch     30E0 als Leon mit (4,194): Ada-Szene, 16, 17, Marvin (0x40) tritt an der Tuer ein und erreicht seine sub11-Lage,
 *              Zeilen 9..13; Flags (3,75)=0 (3,180)=1 (4,194)=0; das Ende kommt erst, wenn die Stimme von msg 13 fertig ist
 *              (nachgebildete Restlaenge 270 Bilder = main13.wav) - und ohne Stimme nach der Textdauer
 *   altstand   (3,75)=1 aus einem alten Spielstand: sub03 mit Warten
 *   elza       30E1 mit (4,194): Original-sub02 (kein Glasbruch, kein msg 16/17, kein Marvin)
 *   zeile3070  3070 als Leon: (4,194) im Besuch gesetzt -> msg 6 einmal; schon gesetzt beim Betreten -> keine Zeile; Elza 3071 keine
 *   t03        3070-Auftritt (Emergence 0x80119378): im Wechselbild steht das letzte Bild von Clip 0x10, Clip 0xd blendet mit a3 0x100
 *   t04        3080-Hand (Szenen-Modus 0x80118d00): zwei Stoesse, der zweite aus dem gehaltenen letzten Bild ab Bild 0, a3 0x400
 *   t03labor   NB1 B2: derselbe Auftritt in 50E0 laeuft den alten Weg (STAGE5 = Endkampf, unveraendert)
 * NB1 B1 (t03): nach dem Auftritt Rate 0 (Kampf wie vor T-03), Tod mit a3 0x200 @0x8011a764. NB1 B3 (programm): Poltern vor msg 6.
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
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_fade.h"
#include "re15_inventory.h"
#include "re15_item_discard.h"
#include "re15_gameflow.h"
#include "re15_blink_r41.h"
#include "re15_light.h"
#include "re15_climb.h"
#include "re15_story_r42.h"
#include "re15_story3b_r42.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);
extern int g_re15_voice_laeuft, g_re15_voice_restbilder;

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

/* Stimmen-Nachbildung: ab Message_on <id> laeuft eine "Aufnahme" von <bilder> Bildern (die Plattform stempelt das sonst je Bild,
 * audio_pc.c; in den Unit-Laeufen gibt es keinen Ton). */
static int s_stimme_id = -1, s_stimme_bilder = 0, s_stimme_rest = 0, s_stimme_offen = 0;
static void stimme_takt(void)
{
    if (s_stimme_id >= 0 && g_scd.message_active && g_scd.message_id == s_stimme_id && !s_stimme_offen) {
        s_stimme_offen = 1; s_stimme_rest = s_stimme_bilder;
    }
    if (s_stimme_rest > 0) s_stimme_rest--;
    g_re15_voice_laeuft = s_stimme_rest > 0;
    g_re15_voice_restbilder = s_stimme_rest;
}

static int typ_slot(uint8_t typ)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == typ) return i;
    return -1;
}

/* Bank eines Typs aus CDEMD0.EMS wie der Raumlader (Muster test_birkin_szenen.c bank_laden) */
static uint8_t s_ems_buf[0x80000];
static int bank_laden(uint8_t type)
{
    size_t esz = 0, off = 0, len = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    if (!ems || re15_ems_get_entry(ems, esz, re15_ems_index_for_type(type), &off, &len) != 0 || len > sizeof s_ems_buf) {
        free(ems); return 0;
    }
    memcpy(s_ems_buf, ems + off, len); free(ems);
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re15_emd_parse_container(s_ems_buf, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return 1;
}
static int clip_bilder(uint8_t type, int clip)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb || !eb->ok || clip >= eb->anim.clip_count) return -1;
    return eb->anim.clips[clip].frame_count;
}

/* ---- texte ------------------------------------------------------------------------------------------------------- */
static int kopf_ok(const uint8_t *b, int l, const char *nm, uint8_t farbe)
{
    static const uint8_t halt[4] = { 0x04, 0x01, 0x01, 0x63 };
    int ok = b && l > 8 && b[0] == 0x04 && b[1] == 0x00 && b[2] == 0x05 && b[3] == farbe && !memcmp(b + l - 4, halt, 4);
    for (int j = 0; ok && nm[j]; j++) {
        const char c = nm[j];
        const uint8_t g = (c >= 'A' && c <= 'Z') ? (uint8_t)(0x1D + c - 'A') : (c >= 'a' && c <= 'z') ? (uint8_t)(0x3D + c - 'a') : 0x16;
        if (b[4 + j] != g) ok = 0;
    }
    return ok;
}
static void teil_texte(void)
{
    printf("== texte ==\n");
    /* Farben wie die ausgelieferten Zeilen: Leon 01 (ROOM30E0 msg 1 @0x110B), Ada 02 (msg 0 @0x10E8), Marvin 07 (msg 11 @0x1381) */
    static const struct { uint16_t r; int id; const char *nm; uint8_t f; } k[] = {
        { 0x30E0, 10, "Leon:", 1 }, { 0x30E0, 11, "Marvin:", 7 }, { 0x30E0, 13, "Marvin:", 7 }, { 0x30E0, 16, "Leon:", 1 },
        { 0x30E0, 17, "Ada:", 2 }, { 0x3070, 6, "Leon:", 1 } };
    for (size_t i = 0; i < sizeof k / sizeof k[0]; i++) {
        int l = 0;
        const uint8_t *b = re15_story3b_r42_text(k[i].r, (uint8_t)k[i].id, &l);
        PRUEF(kopf_ok(b, l, k[i].nm, k[i].f), "ROOM%04X msg %d: Untertitel %s Farbe %02X", k[i].r, k[i].id, k[i].nm, k[i].f);
    }
    PRUEF(!re15_story3b_r42_text(0x30E1, 16, NULL) && !re15_story3b_r42_text(0x3071, 6, NULL), "Elzas 30E1/3071: keine neuen Texte");
    static const struct { int id; const char *wort; } w[] = {
        { 10, "That's a bite" }, { 11, "smashed through the front glass" }, { 13, "the city's finished" },
        { 16, "That came from the lobby" }, { 17, "Someone's coming" }, { 3, "Some dogs got his leg" } };
    re15_game_state_init();
    g_gameflow.character = 0;
    if (room_boot(0x30E0, -19406, 0, -22938, 0, 0) != 0) return;
    for (size_t i = 0; i < sizeof w / sizeof w[0]; i++) {
        const char *t = re15_msg_get_text(w[i].id);
        PRUEF(t && strstr(t, w[i].wort), "ROOM30E0 msg %d nach dem Raumaufbau: \"%s\"", w[i].id, t ? t : "(leer)");
    }
    if (room_boot(0x3070, -12500, 0, -17000, 0, 0) != 0) return;
    { const char *t = re15_msg_get_text(6); PRUEF(t && strstr(t, "storage room"), "ROOM3070 msg 6: \"%s\"", t ? t : "(leer)"); }
    if (room_boot(0x30E1, -19406, 0, -22938, 0, 0) != 0) return;
    { const char *t = re15_msg_get_text(10); PRUEF(t && !strstr(t, "That's a bite"), "ROOM30E1 msg 10 bleibt RDT (\"%s\")", t ? t : "(leer)"); }
}

/* ---- programm ----------------------------------------------------------------------------------------------------- */
static int msg_folge(const uint8_t *p, int n, int *ids, int max)
{
    int k = 0;   /* opcode-genau (Satzbreiten der VM), sonst traefe 0x2b in Operanden */
    for (int i = 0; i < n && k < max;) {
        if (p[i] == 0x2b) ids[k++] = p[i + 1];
        const int sz = scd_opcode_size_at(p + i);
        if (sz <= 0) return -1;
        i += sz;
    }
    return k;
}
static void als_text(const int *ids, int n, char *t, size_t g)
{
    t[0] = 0;
    for (int i = 0; i < n; i++) snprintf(t + strlen(t), g - strlen(t), "%s%d", i ? "," : "", ids[i]);
}
static int finde(const uint8_t *p, int n, const uint8_t *m, int ml)
{
    for (int i = 0; i + ml <= n; i++) if (!memcmp(p + i, m, (size_t)ml)) return i;
    return -1;
}
static void teil_programm(void)
{
    printf("== programm ==\n");
    if (rdt_laden(0x30E0) != 0) { PRUEF(0, "ROOM30E0 ladbar"); return; }
    g_gameflow.character = 0;
    int n = 0, ids[40]; char t[200];
    const uint8_t *p = re15_story3b_r42_programm(0, &n);
    PRUEF(p != NULL, "ein Besuch: Pruefbytes sub02/sub03/sub11 wie erwartet (Programm %d B)", n);
    if (!p) return;
    const uint8_t *r = g_room_rdt.raw;
    const int n02 = RE15_S3B_SUB02_SCHLUSS - RE15_S3B_SUB02_OFF, n03 = RE15_S3B_SUB03_WARTEN - RE15_S3B_SUB03_OFF;
    PRUEF(!memcmp(p, r + RE15_S3B_SUB02_OFF, (size_t)n02), "Original sub02 @0x00CB0..@0x00E53 unveraendert vorn (%d B)", n02);
    const int s03 = finde(p, n, r + RE15_S3B_SUB03_OFF, n03);
    PRUEF(s03 > n02, "Original sub03 @0x00E62..@0x00F85 am Stueck dahinter (ab %d)", s03);
    PRUEF(n >= 12 && !memcmp(p + n - 12, r + RE15_S3B_SUB03_SCHLUSS, 12), "Schluss Cut_auto/Riegel los/Evt_end @0x00FB4..@0x00FBF");
    static const uint8_t glas[] = { 0x36, 0x3e, 0x21, 0x00 };
    const int g = finde(p, n, glas, 4);
    PRUEF(g >= n02 && g < s03, "Glasbruch Se_on(0x3E,0x21) zwischen den Subs");
    static const uint8_t marvin[] = { 0x44, 0x01, 0x40, 0x40, 0x00, 0x01, 0x00, 0xff };
    const int m = finde(p, n, marvin, 8);
    PRUEF(m > g && m < s03 && (int16_t)(p[m + 8] | p[m + 9] << 8) == RE15_S3B_TUER_X && (int16_t)(p[m + 12] | p[m + 13] << 8) == RE15_S3B_TUER_Z,
          "Marvin-Satz (sub11 @0x010A0) an der Tuer (%d,%d) vor sub03", RE15_S3B_TUER_X, RE15_S3B_TUER_Z);
    const int k = msg_folge(p, n, ids, 40);
    als_text(ids, k > 0 ? k : 0, t, sizeof t);
    PRUEF(k > 0 && !strcmp(t, "0,1,2,3,4,5,7,8,16,17,9,10,11,12,13"), "Zeilen %s (soll 0,1,2,3,4,5,7,8,16,17,9,10,11,12,13)", t);
    static const uint8_t warten_kopf[] = { 0x22, 0x05, RE15_S3B_SIG_WARTEN, 0x01, 0x22, 0x05, RE15_S3B_SIG_FREI, 0x00 };
    const int w = finde(p, n, warten_kopf, 8);
    PRUEF(w == s03 + n03, "Warten (Set(5,FREI,0) + Do/Edwhile Ck(5,FREI,0)) nach der Sleep-Kette von msg 13, vor dem Ausklang @0x00F86");
    p = re15_story3b_r42_programm(1, &n);
    PRUEF(p && !memcmp(p, r + RE15_S3B_SUB03_OFF, (size_t)n03) && finde(p, n, warten_kopf, 8) == n03 &&
          !memcmp(p + n - 12, r + RE15_S3B_SUB03_SCHLUSS, 12), "Altstand: sub03 + Warten + Schluss");
    p = re15_story3b_r42_programm(2, &n);
    const int k2 = msg_folge(p, n, ids, 40);
    PRUEF(k2 == 1 && ids[0] == RE15_R42_MSG_3070_ADA, "3070: eine Zeile msg %d", RE15_R42_MSG_3070_ADA);
    /* NB1 B3: vier Holzschlaege (Se_on 0x3E/0x00) im Takt ROOM3091 sub07, Kopf zur Tuer nach 30E0, DANN die Zeile */
    {
        int knall = 0, erst = -1, nacken = -1, zeile = -1;
        for (int i = 0; p && i < n;) {
            if (p[i] == 0x36 && p[i + 1] == RE15_S3B_POLTERN_BANK && p[i + 2] == RE15_S3B_POLTERN_SATZ) { knall++; if (erst < 0) erst = i; }
            if (p[i] == 0x41 && p[i + 1] == 0x01 && (int16_t)(p[i + 2] | p[i + 3] << 8) == RE15_S3B_LAGER_X &&
                (int16_t)(p[i + 6] | p[i + 7] << 8) == RE15_S3B_LAGER_Z) nacken = i;
            if (p[i] == 0x2b && zeile < 0) zeile = i;
            const int sz = scd_opcode_size_at(p + i);
            if (sz <= 0) break;
            i += sz;
        }
        PRUEF(knall == 4 && erst >= 0 && nacken > erst && zeile > nacken,
              "3070: 4 Holzschlaege (%d) ab %d, Kopf zur Lagertuer (%d,%d) bei %d, Zeile bei %d", knall, erst, RE15_S3B_LAGER_X,
              RE15_S3B_LAGER_Z, nacken, zeile);
    }
}

/* ---- besuch ------------------------------------------------------------------------------------------------------- */
typedef struct { int folge[40], nf, gestartet, marvin_da, marvin_x, marvin_z, ende_bild, msg13_zu, s16, s9; } lauf_t;
static void besuch_lauf(uint16_t raum, int stimme13, lauf_t *L)
{
    memset(L, 0, sizeof *L);
    L->ende_bild = -1; L->msg13_zu = -1;
    re15_game_state_init();
    g_gameflow.character = (raum & 1) ? 4 : 0;
    re15_game_flag_set(4, 0xC2, 1);                                     /* ROOM3070 sub01 @0x03496 */
    if (raum == 0x30E1) re15_game_flag_set(3, 0xB5, 1);                 /* Elzas Szene: ROOM30E1 sub03 Ck(3,181) @0x00EA8 */
    s_stimme_id = stimme13 ? 13 : -1; s_stimme_bilder = stimme13; s_stimme_rest = 0; s_stimme_offen = 0;
    const unsigned g0 = g_re15_s3b_zaehler[0];
    if (room_boot(raum, -19406, 0, -22938, 0, 0) != 0) return;          /* Ankunft von der Kartentuer 3010 (re15_room_spawns.h) */
    int last = -1, zu13 = 0;
    for (int f = 0; f < 6000; f++) {
        stimme_takt();
        frame(0, 0);
        if (g_scd.message_active && g_scd.message_id != last && L->nf < 40) { L->folge[L->nf++] = g_scd.message_id; last = g_scd.message_id; }
        if (!g_scd.message_active) last = -1;
        if (last == 16 && !L->s16) L->s16 = f;
        if (last == 9 && !L->s9) L->s9 = f;
        const int ms = typ_slot(0x40);
        if (ms > 0 && !L->marvin_da) L->marvin_da = f;
        if (ms > 0 && !L->s9) { L->marvin_x = g_actors[ms].x; L->marvin_z = g_actors[ms].z; }   /* Lage beim Szenenstart sub03 */
        if (ms > 0 && getenv("S3B_SPUR") && (f % 20) == 0)
            printf("    F%d Marvin (%d,%d) st %d/%d mo %d  Leon (%d,%d)  Ada %d\n", f, g_actors[ms].x, g_actors[ms].z, g_actors[ms].state,
                   g_actors[ms].sub_state_1, (int)g_actors[ms].motion, g_actors[0].x, g_actors[0].z, typ_slot(0x42));
        if (L->nf && L->folge[L->nf - 1] == 13 && !g_scd.message_active && !zu13) { zu13 = 1; L->msg13_zu = f; }
        if (L->nf && re15_game_flag_get(3, 0xB4) && !re15_game_flag_get(2, 7) && L->ende_bild < 0) { L->ende_bild = f; break; }
        if (raum == 0x30E1 && f > 2500) break;
    }
    L->gestartet = (int)(g_re15_s3b_zaehler[0] - g0);
    g_re15_voice_laeuft = 0; g_re15_voice_restbilder = 0;
}
static void teil_besuch(void)
{
    printf("== besuch ==\n");
    lauf_t L; char t[200];
    /* 1. mit Aufnahme von msg 13 = 270 Bilder (synchro/STAGE3/room30E0/main13.wav 9,01 s, gemessen) */
    besuch_lauf(0x30E0, 270, &L);
    als_text(L.folge, L.nf, t, sizeof t);
    PRUEF(L.gestartet == 1, "Programm 'ein Besuch' gestartet (Evt_exec sub02 aus sub11 @0x0107C)");
    PRUEF(!strcmp(t, "0,1,2,3,4,5,7,8,16,17,9,10,11,12,13"), "Zeilen im Spiel: %s", t);
    PRUEF(L.marvin_da > L.s16 && L.marvin_da < L.s9, "Marvin (0x40) erscheint nach msg 16 (Bild %d) und vor msg 9 (Bild %d): Bild %d",
          L.s16, L.s9, L.marvin_da);
    PRUEF(abs(L.marvin_x - RE15_S3B_MARVIN_X) < 300 && abs(L.marvin_z - RE15_S3B_MARVIN_Z) < 300,
          "Marvin steht bei msg 9 bei (%d,%d) ~ sub11-Lage (%d,%d)", L.marvin_x, L.marvin_z, RE15_S3B_MARVIN_X, RE15_S3B_MARVIN_Z);
    PRUEF(!re15_game_flag_get(3, 0x4B) && re15_game_flag_get(3, 0xB4) && !re15_game_flag_get(4, 0xC2),
          "Flags: (3,75)=0, (3,180)=1 (3070-Aufzugszene), (4,194)=0");
    PRUEF(L.ende_bild > 0 && L.msg13_zu > 0, "Szene endet (Riegel (2,7) los) bei Bild %d, msg 13 zu bei Bild %d", L.ende_bild, L.msg13_zu);
    PRUEF(L.ende_bild >= L.msg13_zu, "Ende NICHT vor dem Ende von Untertitel und Stimme (Ende %d >= msg 13 zu %d)", L.ende_bild, L.msg13_zu);
    const int ende_stimme = L.ende_bild;
    /* 2. ohne Aufnahme: Textdauer */
    besuch_lauf(0x30E0, 0, &L);
    PRUEF(L.ende_bild > 0 && L.ende_bild >= L.msg13_zu && L.ende_bild < ende_stimme,
          "ohne Aufnahme: Ende %d nach der Textdauer von msg 13 (zu %d), frueher als mit 270-Bilder-Stimme (%d)", L.ende_bild,
          L.msg13_zu, ende_stimme);
}

static void teil_altstand(void)
{
    printf("== altstand ==\n");
    re15_game_state_init(); g_gameflow.character = 0;
    re15_game_flag_set(3, 0x4B, 1);                                     /* sub02 @0x00CB4 aus einem Spielstand v0.8.30 */
    s_stimme_id = 13; s_stimme_bilder = 270; s_stimme_rest = 0; s_stimme_offen = 0;
    const unsigned z1 = g_re15_s3b_zaehler[1];
    if (room_boot(0x30E0, -19406, 0, -22938, 0, 0) != 0) return;
    int zu = -1, ende = -1, seen13 = 0;
    for (int f = 0; f < 5000; f++) {
        stimme_takt(); frame(0, 0);
        if (g_scd.message_active && g_scd.message_id == 13) seen13 = 1;
        if (seen13 && !g_scd.message_active && zu < 0) zu = f;
        if (seen13 && re15_game_flag_get(3, 0xB4) && !re15_game_flag_get(2, 7)) { ende = f; break; }
    }
    g_re15_voice_laeuft = 0; g_re15_voice_restbilder = 0;
    PRUEF(g_re15_s3b_zaehler[1] == z1 + 1, "sub03 (Evt_exec @0x010B4) mit Warten");
    PRUEF(seen13 && zu > 0 && ende >= zu, "Ende %d nach msg 13 + Stimme (zu %d)", ende, zu);
}

static void teil_elza(void)
{
    printf("== elza ==\n");
    lauf_t L; char t[200];
    const unsigned g0 = g_re15_s3b_zaehler[0];
    besuch_lauf(0x30E1, 0, &L);
    als_text(L.folge, L.nf, t, sizeof t);
    PRUEF(g_re15_s3b_zaehler[0] == g0 && L.nf > 0, "30E1: Original-Szene ohne Port-Programm (Zeilen %s)", t);
    const char *t16 = re15_msg_get_text(16), *t17 = re15_msg_get_text(17);
    PRUEF((!t16 || !strstr(t16, "That came from the lobby")) && (!t17 || !strstr(t17, "Someone's coming")) && typ_slot(0x40) < 0,
          "30E1: RDT-Texte 16/17 (\"%.30s\" / \"%.30s\"), kein Glasbruch-Uebergang, kein Marvin (0x40)", t16 ? t16 : "", t17 ? t17 : "");
}

/* ---- zeile3070 ------------------------------------------------------------------------------------------------------ */
static void teil_zeile3070(void)
{
    printf("== zeile3070 ==\n");
    for (int fall = 0; fall < 3; fall++) {
        re15_game_state_init();
        const uint16_t raum = fall == 2 ? 0x3071 : 0x3070;
        g_gameflow.character = fall == 2 ? 4 : 0;
        re15_game_flag_set(4, 0xD7, 1);                                  /* Birkin-Szene schon gesehen (sub02 @0x034A8) */
        re15_game_flag_set(4, 0x4B, 1);                                  /* Elza 3071 entsprechend */
        if (fall == 1) re15_game_flag_set(4, 0xC2, 1);
        if (room_boot(raum, -12500, 0, -17000, 0, 0) != 0) return;
        const unsigned z = g_re15_s3b_zaehler[2];
        int sah6 = 0;
        for (int f = 0; f < 600; f++) {
            if (f == 30) re15_game_flag_set(4, 0xC2, 1);                 /* wie sub01 @0x03496 nach (5,0x1c) */
            frame(0, 0);
            if (g_scd.message_active && g_scd.message_id == RE15_R42_MSG_3070_ADA) {
                const char *tx = re15_msg_get_text(RE15_R42_MSG_3070_ADA);
                if (tx && strstr(tx, "storage room")) sah6 = 1;
            }
        }
        const int n = (int)(g_re15_s3b_zaehler[2] - z);
        if (fall == 0) PRUEF(sah6 && n == 1, "3070 Leon: (4,194) im Besuch -> msg 6 genau einmal (%d)", n);
        if (fall == 1) PRUEF(!sah6 && n == 0, "3070 Leon: (4,194) schon beim Betreten -> keine Zeile");
        if (fall == 2) PRUEF(!sah6 && n == 0, "3071 Elza: keine Zeile");
        PRUEF(!re15_game_flag_get(2, 7), "Pad-Sperre (2,7) wieder frei");
    }
}

/* ---- t03 ----------------------------------------------------------------------------------------------------------- */
static void teil_t03(void)
{
    printf("== t03 ==\n");
    re15_game_state_init(); g_gameflow.character = 0;
    if (room_boot(0x3070, -26155, 0, -8635, 2048, 2) != 0) return;     /* im Ausloese-Rechteck Slot 2 @0x033E2 */
    PRUEF(bank_laden(0x30), "EM30 aus CDEMD0.EMS");
    const int fc10 = clip_bilder(0x30, 0x10), fc0d = clip_bilder(0x30, 0x0d);
    PRUEF(fc10 > 2 && fc0d > 2, "Clip 0x10 %d Bilder, Clip 0xd %d Bilder", fc10, fc0d);
    int b = -1, halt10 = -1, rate0d = -1, frac0d = -1, halt0d = -1, wechsel = -1, phase_alt = -1, sub9 = 0;
    int rate_kampf = -1, rate_tod = -1, frac_tod = -1;
    for (int f = 0; f < 3000; f++) {
        if (f == 5) re15_aot_fire_slot(2);   /* Aot_set Slot 2 @0x033E2 -> sub02 (Muster probe_r30_n_room5080.c); der echte Ausloeser
                                              * laeuft im Durchlauf (integration_r42_story3b_leon) */
        frame(0, 0);
        if (b < 0) b = typ_slot(0x30);
        if (b < 0) continue;
        re15_actor_t *e = &g_actors[b];
        if (e->sub_state_1 == 9) sub9 = 1;
        if (getenv("S3B_SPUR") && (f % 10) == 0) printf("    F%d st %d s1 %d s2 %d mo %d af %d gr %02x\n", f, e->state, e->sub_state_1,
                                                    e->sub_state_2, (int)e->motion, (int)e->anim_frame, e->grid_id);
        if (!sub9) continue;
        const int ph = e->sub_state_1 == 9 ? e->sub_state_2 : 5;
        if (ph != phase_alt) {
            if (phase_alt == 1 && ph == 2) halt10 = (int)e->anim_frame;                     /* Wechselbild Clip 0x10 */
            if (phase_alt == 2 && ph == 3) { rate0d = e->anim_blend_rate; frac0d = e->anim_frac; wechsel = e->motion; }
            if (phase_alt == 3 && ph == 4) halt0d = (int)e->anim_frame;
            phase_alt = ph;
        }
        if (ph == 5) { rate_kampf = e->anim_blend_rate; break; }
    }
    /* NB1 B1: Tod nach dem Auftritt (Commit wie der Messhaken RE15_S3B_BIRKIN_TOT): Phase 0/1 mit a3 0x200 @0x8011a764 */
    if (b >= 0 && phase_alt == 5) {
        re15_actor_t *e = &g_actors[b];
        e->hp = 0; e->state = 3; e->sub_state_3 = 0;
        for (int f = 0; f < 400 && e->state == 3 && e->sub_state_3 < 2; f++) {
            frame(0, 0);
            if (e->state == 3 && e->sub_state_3 == 1 && e->motion == 9 && rate_tod < 0) { rate_tod = e->anim_blend_rate; frac_tod = e->anim_frac; }
        }
    }
    PRUEF(sub9, "Auftritt (Sub 9) nach Member_set(0x0C,0x13) @0x0358E");
    PRUEF(rate_kampf == 0, "nach dem Auftritt (Phase 4 @0x801194d0 -> Sub 3) Rate zurueck auf 0 = Kampf wie vor T-03 (Rate %#x)", rate_kampf);
    PRUEF(rate_tod == 0x200 && frac_tod >= 6,
          "Tod Clip 9: a3 0x200 (@0x8011a764), +0x8f 7 (@0x8011a654) -> Startgewicht 7 x 0x200 = 3584 (Rate %#x frac %d)", rate_tod, frac_tod);
    PRUEF(halt10 == fc10 - 1, "Wechselbild Clip 0x10: Bild %d = letztes (%d) statt Umlauf 0 (@0x8001f35c/@0x8001f63c)", halt10, fc10 - 1);
    PRUEF(wechsel == 0x0d && rate0d == 0x100 && frac0d == 0x0e,
          "Clip 0xd (@0x80119464) blendet mit a3 0x100 (@0x801194a4), frac 0xf -> 0xe nach dem ersten Schritt (Rate %#x frac %d)",
          rate0d, frac0d);
    PRUEF(halt0d == fc0d - 1, "Ende Clip 0xd: letztes Bild %d (soll %d) bleibt bis zum Ansprung (Phase 4 ohne anim_set @0x801194d0)",
          halt0d, fc0d - 1);
}

/* ---- t04 ----------------------------------------------------------------------------------------------------------- */
static void teil_t04(void)
{
    printf("== t04 ==\n");
    re15_game_state_init(); g_gameflow.character = 0;
    if (room_boot(0x3080, -21000, 0, -21000, 0, 0) != 0) return;
    PRUEF(bank_laden(0x36), "EM36 aus CDEMD0.EMS");
    const int fc4 = clip_bilder(0x36, 4);
    int b = -1, phase_alt = -1, halt9 = -1, bild10 = -1, rate10 = -1, frac10 = -1, rate9 = -1, stoesse = 0;
    for (int f = 0; f < 3000; f++) {
        frame(0, 0);
        if (b < 0) b = typ_slot(0x36);
        if (b < 0) continue;
        re15_actor_t *e = &g_actors[b];
        const int ph = e->sub_state_2;
        if (e->motion == 4 && ph == 9 && rate9 < 0) rate9 = e->anim_blend_rate;
        if (ph != phase_alt) {
            if (ph == 9 || ph == 11) stoesse++;
            if (phase_alt == 9 && ph == 10) halt9 = (int)e->anim_frame;
            if (phase_alt == 9 && ph == 11) halt9 = -2;                 /* Phase 10 im selben Bild: nicht messbar */
            if (ph == 11 && bild10 < 0) { bild10 = (int)e->anim_frame; rate10 = e->anim_blend_rate; frac10 = e->anim_frac; }
            phase_alt = ph;
        }
        if (ph >= 12) break;
    }
    PRUEF(fc4 > 2, "Clip 4 hat %d Bilder", fc4);
    PRUEF(stoesse == 2, "zwei Stoesse (Phase 8 -> 9 und Phase 10 -> 11, Original @0x801191e4/@0x8011922c): %d", stoesse);
    PRUEF(rate9 == 0x400, "Stoss 1: a3 0x400 (@0x80119274): %#x", rate9);
    PRUEF(halt9 == fc4 - 1, "Wechselbild nach Stoss 1: letztes Bild %d (soll %d) gehalten, kein Ruecksprung auf Bild 0", halt9, fc4 - 1);
    PRUEF(bild10 == 1 && rate10 == 0x400 && frac10 == 2,
          "Stoss 2: Clip 4 ab Bild 0 (Umlaufwert @0x8001f63c, gezeichnet 1), frac 3 * a3 0x400 (Bild %d Rate %#x frac %d)",
          bild10, rate10, frac10);
}

/* ---- t03labor (NB1 B2): derselbe Auftritt in 50E0 (grid 0x33 @0xACE) laeuft den ALTEN Weg (Endkampf-Raeume unveraendert) ------- */
static void teil_t03labor(void)
{
    printf("== t03labor ==\n");
    re15_game_state_init(); g_gameflow.character = 0;
    if (room_boot(0x50E0, -25450, 0, -14700, 0, 0) != 0) return;
    PRUEF(bank_laden(0x30), "EM30 aus CDEMD0.EMS");
    int b = -1;
    for (int f = 0; f < 40 && b < 0; f++) { frame(0, 0); b = typ_slot(0x30); }
    PRUEF(b >= 0, "50E0: Birkin (0x30) gesetzt (Sce_em_set @0xACE)");
    if (b < 0) return;
    re15_actor_t *e = &g_actors[b];
    e->grid_id = 0x13; e->state = 0;      /* wie sub04 Member_set(0x0C,0x13): INIT -> nib 3 -> Sub 9 (Muster t03 re15_aot_fire_slot) */
    const int fc10 = clip_bilder(0x30, 0x10);
    int phase_alt = -1, wechsel10 = -1, rate_max = 0, sub9 = 0;
    for (int f = 0; f < 3000; f++) {
        frame(0, 0);
        if (e->sub_state_1 == 9 && e->state == 1) sub9 = 1;
        if (!sub9) continue;
        if (e->anim_blend_rate > rate_max) rate_max = e->anim_blend_rate;
        const int ph = (e->state == 1 && e->sub_state_1 == 9) ? e->sub_state_2 : 5;
        if (ph != phase_alt) { if (phase_alt == 1 && ph == 2) wechsel10 = e->anim_frame; phase_alt = ph; }
        if (ph == 5) break;
    }
    PRUEF(sub9 && phase_alt == 5, "50E0: Auftritt Sub 9 bis zum Ansprung");
    PRUEF(wechsel10 == 0 && fc10 > 1, "50E0: Wechselbild Clip 0x10 = Umlauf 0 wie vor T-03 (Bild %d, Clip %d Bilder)", wechsel10, fc10);
    PRUEF(rate_max == 0, "50E0: keine Szenen-Staerke (Rate %#x), Zeichenpfad-Standard wie vor T-03", rate_max);
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "texte")) teil_texte();
    else if (!strcmp(t, "programm")) teil_programm();
    else if (!strcmp(t, "besuch")) teil_besuch();
    else if (!strcmp(t, "altstand")) teil_altstand();
    else if (!strcmp(t, "elza")) teil_elza();
    else if (!strcmp(t, "zeile3070")) teil_zeile3070();
    else if (!strcmp(t, "t03")) teil_t03();
    else if (!strcmp(t, "t04")) teil_t04();
    else if (!strcmp(t, "t03labor")) teil_t03labor();
    else { printf("unbekannter Teil '%s'\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
