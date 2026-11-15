/*
 * test_r42_story3a.c — Runde 42 "story" (v0.8.31), Stufe STAGE3a: Marvin. Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit
 * allen Installern, SCD-VM, re15_game_step; Figurwechsel ueber re15_sherry_r41_modell_setzen = Raumlader FUN_800314b0), Harness-Muster
 * test_r42_story1.c. Konstanten/Belege include/re15_story3a_r42.h, Code engine/src/story3a_r42.c. PORT-STORY nach dem Konzept
 * (5 T-01/T-02, 3.2 L5-L7, L-05..L-10; F-04, F-05, F-06).
 *
 * Teile (Aufruf: test_r42_story3a <teil>):
 *   texte       3020 msg 0..2 Neutext (Glaettung), 7..9; 3060 msg 11..27: Untertitel mit Sprecherkopf je VERTRAG 2.3, 16/18 Systemtext
 *   programme   3020 sub02 = Original-Bytes + Zeilen 7/8 vor `24 10 0d 00` @0x0D7A; 3060 sub03 = Original + Zeile 11 + Abblende vor dem
 *               Rueckwechsel @0x010C6; Wiedersehen: Zeilen in Reihenfolge je Marvin-Seite (mit/ohne Keycard, mit Redhawk), Bit 124
 *   tausch      T-01/T-02: Leon (Pistole ausgeruestet) -> Marvin: Seite Browning x15 + H. Gun Bullets x50, Platz 0, Waffe 3 (kein
 *               Messer, kein UNEQUIP-Doppeldruck); Marvins Funde; Marvin -> Leon: Leons Seite, Platz und Waffe zurueck, Marvins Seite
 *               fuer die Uebergabe gemerkt; Ruestungswechsel (Weste 0 -> 1) tauscht nicht
 *   tausch_elza Elza -> Sherry (0x0F) und zurueck: kein Tausch, Verhalten wie Runde 41 (Elzas STAGE3 unveraendert)
 *   speichern   Spielstand v10 waehrend Marvin spielt: Block -> Neustart -> Laden: Marvins Seite, work_vars[0x10] = 0x0D, Neuladen
 *               ohne Schwanz (HP bleibt), Waffe aus dem Platz; danach Rueckwechsel gibt Leon alles zurueck
 *   hebung      v9-Block (Pruefwort bei 940) laedt als v10 ohne Tausch
 *   wiedersehen 3060 als Leon nach Marvins Abschnitt, Hunde tot: Marvin erscheint an der orangen Tuer, Zeilen 12..27, Keycard + Fabrikplan
 *               (Bit 35, Blatt 7) + Redhawk + Magnum (msg 20 je Fund mit Namen) in Leons Inventar, Marvin geht durch die Tuer, Bit 124
 *   karte3010   Kartentuer 3010 Platz 6: nach Marvins Abschnitt ohne Uebergabe "locked" (msg 0 @0x020B2), nach Bit 124 die Original-Wege
 *   rdttexte    NB1 B1/B5: nach dem ECHTEN Raumaufbau (scd_room_reenter + re15_msg_load_room_block) stehen in 3020 msg 0..2 und 30E0
 *               msg 3 die neuen Texte (Wortlaut), Elzas 3021 behaelt den RDT-Text
 *   weg30e0     NB1 B2: Marvin kommt aus 3030 in den Ost-Ring von 30E0 und erreicht die Rueckseite der Kartentuer (Slot 0
 *               @0x009F2, West-Ring) nicht (Messung re15_story3a_r42.h "L-10 gegen Marvins Weg")
 *   altstand    NB1 B6: ohne frischen Rueckwechsel kein "Gunshots...?" in 3020; ohne offene Uebergabe und nach (3,0x3C) kein
 *               Wiedersehen, davor holt der Altstand es nach
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
#include "re15_gameflow.h"
#include "re15_blink_r41.h"
#include "re15_light.h"
#include "re15_lock_se.h"
#include "re15_gatorszene_r41.h"
#include "re15_story2_r42.h"
#include "re15_story1_r42.h"
#include "re15_story3a_r42.h"
#include "re15_sherry_r41.h"
#include "re15_savedata.h"
#include "re15_itembox.h"
#include "re15_story_r42.h"
#include "re15_climb.h"
#include "re15_npckante_r38.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);
const uint8_t *re15_inv_desc_entry(int id);
extern int g_test_door_se_count;   /* tests/test_support.c: Spion re15_audio_re2_door_se */
extern int g_test_door_se_last;

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
#include "re15_map_owned.h"

static int bit9(int b) { return re15_game_flag_get(9, (uint8_t)b) ? 1 : 0; }
static int typ_zahl(uint8_t typ)
{
    int n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == typ) n++;
    return n;
}
static void inv_setzen(int i, uint8_t id, uint8_t qty) { g_inv.slots[i].id = id; g_inv.slots[i].qty = qty; g_inv.slots[i].flags = 0; }
static int inv_menge(uint8_t id)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) return g_inv.slots[i].qty ? g_inv.slots[i].qty : 1;
    return 0;
}
static int seite_menge(const re15_inv_slot_t *s, uint8_t id)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (s[i].id == id) return s[i].qty ? s[i].qty : 1;
    return 0;
}
static int belegt(void)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id) n++;
    return n;
}
extern int  re15_player_equipped_weapon(void);
extern void re15_player_set_equipped_weapon(int id);

/* Leon mit ausgeruesteter Pistole (Platz 0), Munition, Spray, Uhr - wie ein Spieler nach STAGE2 */
static void leon_inventar(void)
{
    re15_inv_init();
    inv_setzen(0, 0x03, 12); inv_setzen(1, 0x15, 30); inv_setzen(2, 0x23, 1); inv_setzen(3, 0x3F, 1);
    re15_player_set_equipped_weapon(0x03);
    re15_inv_set_equipped_slot(0);
}

/* ---- texte ------------------------------------------------------------------------------------------------- */
static void teil_texte(void)
{
    printf("== texte ==\n");
    static const struct { uint16_t r; int id; const char *name; } k[] = {
        { 0x3020, 0, "Leon:" }, { 0x3020, 1, "Marvin:" }, { 0x3020, 2, "Marvin:" }, { 0x3020, 7, "Marvin:" }, { 0x3020, 8, "Leon:" },
        { 0x3020, 9, "Leon:" },
        { 0x3060, 11, "Marvin:" }, { 0x3060, 12, "Marvin:" }, { 0x3060, 13, "Leon:" }, { 0x3060, 14, "Marvin:" },
        { 0x3060, 15, "Marvin:" }, { 0x3060, 17, "Marvin:" }, { 0x3060, 19, "Marvin:" }, { 0x3060, 21, "Leon:" },
        { 0x3060, 22, "Marvin:" }, { 0x3060, 23, "Leon:" }, { 0x3060, 24, "Marvin:" }, { 0x3060, 25, "Marvin:" },
        { 0x3060, 26, "Leon:" }, { 0x3060, 27, "Marvin:" }, { 0x30E0, 3, "Leon:" } };
    static const uint8_t halt[4] = { 0x04, 0x01, 0x01, 0x63 };
    for (size_t i = 0; i < sizeof k / sizeof k[0]; i++) {
        int l = 0;
        const uint8_t *b = re15_story3a_r42_text(k[i].r, (uint8_t)k[i].id, &l);
        int ok = b && l > 8 && b[0] == 0x04 && b[1] == 0x00 && b[2] == 0x05 && !memcmp(b + l - 4, halt, 4);
        const char *nm = k[i].name;
        /* Farbe wie die ausgelieferten Zeilen ROOM3020 (Leon 01 msg 0 @0x0E1A, Marvin 07 msg 1 @0x0E49) */
        if (ok) ok = b[3] == (nm[0] == 'L' ? 0x01 : 0x07);
        for (int j = 0; ok && nm[j]; j++) {
            const char c = nm[j];
            const uint8_t g = (c >= 'A' && c <= 'Z') ? (uint8_t)(0x1D + c - 'A') : (c >= 'a' && c <= 'z') ? (uint8_t)(0x3D + c - 'a') : 0x16;
            if (b[4 + j] != g) ok = 0;
        }
        PRUEF(ok, "ROOM%04X msg %d: Untertitel mit Kopf %s", k[i].r, k[i].id, k[i].name);
    }
    for (int id = 16; id <= 18; id += 2) {
        int l = 0;
        const uint8_t *b = re15_story3a_r42_text(0x3060, (uint8_t)id, &l);
        PRUEF(b && l > 4 && b[0] == 0x04 && b[1] == 0x02 && b[l - 2] == 0x01 && b[l - 1] == 0x00,
              "ROOM3060 msg %d: Systemtext 04 02 .. 01 00 (Form ROOM3020 msg 3 @0x0EBD)", id);
    }
    {
        int l = 0;
        const uint8_t *b = re15_story3a_r42_text(0x30E0, 18, &l);
        PRUEF(!b && !l, "ROOM30E0 msg 18: kein Port-Text (kein Riegel noetig, Messung B2)");
    }
    PRUEF(!re15_story3a_r42_text(0x3021, 7, NULL) && !re15_story3a_r42_text(0x3061, 11, NULL) && !re15_story3a_r42_text(0x30E1, 3, NULL),
          "Elzas 3021/3061/30E1: keine neuen Texte");
    PRUEF(!re15_story3a_r42_text(0x3020, 3, NULL), "3020 msg 3 bleibt Original");
}

/* ---- programme ----------------------------------------------------------------------------------------------- */
static int msg_folge(const uint8_t *p, int n, int *ids, int max)
{
    int k = 0;
    for (int i = 0; i + 3 < n && k < max; i++)
        if (p[i] == 0x2b && ((p[i + 2] == 0x00 && p[i + 3] == 0x00) || (p[i + 2] == 0xff && p[i + 3] == 0xff))) ids[k++] = p[i + 1];
    return k;
}
static const char *als_text(const int *ids, int n, char *t, size_t g)
{
    t[0] = 0;
    for (int i = 0; i < n; i++) snprintf(t + strlen(t), g - strlen(t), "%s%d", i ? "," : "", ids[i]);
    return t;
}
static void teil_programme(void)
{
    printf("== programme ==\n");
    char t[160];
    int ids[40];
    if (rdt_laden(0x3020) != 0) { PRUEF(0, "ROOM3020 ladbar"); return; }
    int n = 0;
    const uint8_t *p = re15_story3a_r42_programm(0, &n);
    PRUEF(p != NULL, "3020 sub02-Bytes @0x0C10/@0x0D76 wie erwartet");
    if (p) {
        int k = msg_folge(p, n, ids, 40);
        PRUEF(!strcmp(als_text(ids, k, t, sizeof t), "0,1,2,7,8"), "3020: Zeilen %s (soll 0,1,2,7,8)", t);
        int wech = -1, pos8 = -1;
        for (int i = 0; i + 3 < n; i++) {
            if (p[i] == 0x24 && p[i + 1] == 0x10 && p[i + 2] == 0x0d && wech < 0) wech = i;
            if (p[i] == 0x2b && p[i + 1] == 8) pos8 = i;
        }
        PRUEF(wech > pos8 && pos8 >= 0, "3020: Wechsel 24 10 0d 00 (@0x0D7A) NACH Zeile 8");
        PRUEF(!memcmp(p, g_room_rdt.raw + RE15_S3A_3020_SUB02_OFF, RE15_S3A_3020_WECHSEL - RE15_S3A_3020_SUB02_OFF),
              "3020: Original-Bytes @0x0C10..@0x0D79 unveraendert vorn");
        PRUEF(n >= 4 && p[n - 4] == 0x47 && p[n - 3] == 0x02 && p[n - 2] == 0x01 && p[n - 1] == 0x00, "3020: Ende Aot_on 2 + Evt_end");
    }
    if (rdt_laden(0x3060) != 0) { PRUEF(0, "ROOM3060 ladbar"); return; }
    p = re15_story3a_r42_programm(1, &n);
    PRUEF(p != NULL, "3060 sub03-Bytes @0x0104A/@0x010BE/@0x010E4 wie erwartet");
    if (p) {
        int k = msg_folge(p, n, ids, 40);
        PRUEF(!strcmp(als_text(ids, k, t, sizeof t), "11"), "3060 Marvin: Zeile %s (soll 11)", t);
        int abb = -1, wech = -1;
        for (int i = 0; i + 3 < n; i++) {
            if (p[i] == 0x22 && p[i + 1] == 5 && p[i + 2] == RE15_S3A_SIG_ABBLENDE && p[i + 3] == 1) abb = i;
            if (p[i] == 0x24 && p[i + 1] == 0x10 && p[i + 2] == 0x00 && wech < 0) wech = i;
        }
        PRUEF(abb >= 0 && wech > abb, "3060 Marvin: Abblende-Signal vor dem Rueckwechsel 24 10 00 00 (@0x010D2)");
        PRUEF(!memcmp(p, g_room_rdt.raw + RE15_S3A_3060_SUB03_OFF, RE15_S3A_3060_NACH_SLEEP - RE15_S3A_3060_SUB03_OFF),
              "3060 Marvin: Original-Bytes @0x0104A..@0x010C5 unveraendert vorn (Hunde los, Plc_motion(2,6), Sleep 40)");
    }
    static const struct { const char *name; uint8_t ids[4]; uint8_t qty[4]; const char *soll; int ueb; } fall[] = {
        { "Keycard",           { 0x03, 0x15, 0x36, 0 },    { 15, 50, 1, 0 }, "12,13,14,15,16,17,18,21,22,23,24,25,26,27", 2 },
        { "Keycard + Redhawk", { 0x03, 0x15, 0x36, 0x07 }, { 15, 50, 1, 6 }, "12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27", 3 },
        { "ohne Keycard",      { 0x03, 0x15, 0, 0 },       { 15, 50, 0, 0 }, "12,13,14,17,18,22,23,24,25,26,27", 1 },
        /* NB1 B8: 80 H. Gun Bullets = 50 eigene + 30 gefundene -> die 30 sind ein Fund (msg 19/20) */
        { "Munition ueber 50", { 0x03, 0x15, 0x36, 0 },    { 15, 80, 1, 0 }, "12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27", 3 },
        { "Munition 50",       { 0x03, 0x15, 0x36, 0 },    { 15, 50, 1, 0 }, "12,13,14,15,16,17,18,21,22,23,24,25,26,27", 2 },
    };
    for (size_t f = 0; f < sizeof fall / sizeof fall[0]; f++) {
        re15_s3a_figur_t s; memset(&s, 0, sizeof s);
        s.marvin_offen = 1;
        for (int i = 0; i < 4; i++) { s.marvin[i].id = fall[f].ids[i]; s.marvin[i].qty = fall[f].qty[i]; }
        uint8_t blk[RE15_S3A_SPEICHER_BYTES]; memset(blk, 0, sizeof blk); memcpy(blk, &s, sizeof s);
        re15_game_state_init();
        re15_story3a_r42_import(blk);
        p = re15_story3a_r42_programm(2, &n);
        int k = msg_folge(p, n, ids, 40), b124 = 0, ueb = 0;
        for (int i = 0; i + 3 < n; i++) {
            if (p[i] == 0x22 && p[i + 1] == 9 && p[i + 2] == RE15_R42_BIT_3060_UEBERGABE && p[i + 3] == 1) b124 = 1;
            if (p[i] == 0x22 && p[i + 1] == 5 && p[i + 2] == RE15_S3A_SIG_UEBERGABE && p[i + 3] == 1) ueb++;
        }
        PRUEF(!strcmp(als_text(ids, k, t, sizeof t), fall[f].soll), "Wiedersehen %s: Zeilen %s (soll %s)", fall[f].name, t, fall[f].soll);
        PRUEF(ueb == fall[f].ueb, "Wiedersehen %s: %d Uebergaben (soll %d)", fall[f].name, ueb, fall[f].ueb);
        PRUEF(b124 && n >= 2 && p[n - 2] == 0x01 && p[n - 1] == 0x00, "Wiedersehen %s: Set(9,124,1) und Evt_end", fall[f].name);
    }
    re15_story3a_r42_neues_spiel();
}

/* ---- tausch -------------------------------------------------------------------------------------------------- */
static void teil_tausch(void)
{
    printf("== tausch ==\n");
    re15_game_state_init();
    re15_story3a_r42_neues_spiel();
    g_gameflow.character = 0;
    leon_inventar();
    re15_inv_slot_t leon_vorher[RE15_INV_MAX_SLOTS];
    memcpy(leon_vorher, g_inv.slots, sizeof leon_vorher);
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 64;
    g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags = 0x2u;              /* Gift (Bit 0x2 von 0x800acaec) */
    re15_inv_set_prev_equip_slot(2);
    re15_sherry_r41_modell_init(0);
    re15_sherry_r41_modell_setzen(RE15_S3A_MODELL_MARVIN);             /* ROOM3020 @0x00D7A -> Raumlader */
    PRUEF(re15_story3a_r42_marvin_spielt(), "Tausch aktiv");
    PRUEF(g_inv.slots[0].id == 0x03 && g_inv.slots[0].qty == 15 && g_inv.slots[1].id == 0x15 && g_inv.slots[1].qty == 50 && belegt() == 2,
          "Marvins Seite: Browning x15, H. Gun Bullets x50 (@0x80074bb8/@0x80074bd0), sonst leer (%d belegt)", belegt());
    PRUEF(re15_inv_equipped_slot() == 0 && re15_player_equipped_weapon() == 0x03,
          "Marvin: Platz 0 ausgeruestet, Waffe 3 (kein Messer; Platz und Waffe passen -> kein UNEQUIP-Doppeldruck) - Platz %d Waffe %d",
          re15_inv_equipped_slot(), re15_player_equipped_weapon());
    PRUEF(g_actors[RE15_ACTOR_SLOT_PLAYER].hp == 100 && !(g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags & 0x2u),
          "Marvin: HP := 100, kein Gift (Schwanz FUN_800314b0 @0x80031710/@0x80031720 bleibt)");
    const re15_s3a_figur_t *f = re15_story3a_r42_figur();
    PRUEF(!memcmp(f->leon, leon_vorher, sizeof leon_vorher) && f->leon_platz == 0 && f->leon_waffe == 0x03,
          "Leons Seite, Platz 0 und Waffe 3 gesichert (vor dem Schwanz)");
    inv_setzen(2, 0x36, 1); inv_setzen(3, 0x07, 6); g_inv.slots[0].qty = 12;   /* Marvin findet Keycard + Redhawk, schiesst */
    re15_sherry_r41_modell_setzen(0);                                  /* ROOM3060 @0x010D2 -> Raumlader */
    PRUEF(!re15_story3a_r42_marvin_spielt(), "Tausch beendet");
    PRUEF(!memcmp(g_inv.slots, leon_vorher, sizeof leon_vorher), "Leon hat alles zurueck (4 Plaetze wie vorher)");
    PRUEF(re15_inv_equipped_slot() == 0 && re15_player_equipped_weapon() == 0x03, "Leon: Platz 0, Waffe 3 zurueck (Platz %d Waffe %d)",
          re15_inv_equipped_slot(), re15_player_equipped_weapon());
    PRUEF(g_actors[RE15_ACTOR_SLOT_PLAYER].hp == 64 && (g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags & 0x2u),
          "NB1 B8: Leon hat HP 64 und Gift zurueck (RE2 FUN_80049e48 @0x8004a058-@0x8004a07c; keine Heilung) - HP %d",
          (int)g_actors[RE15_ACTOR_SLOT_PLAYER].hp);
    PRUEF(re15_inv_prev_equip_slot() == 2, "NB1 B8: Vor-Platz 2 zurueck (nicht 0x80) - %d", re15_inv_prev_equip_slot());
    PRUEF(f->marvin_offen && seite_menge(f->marvin, 0x36) == 1 && seite_menge(f->marvin, 0x07) == 6,
          "Marvins Funde (Keycard, Redhawk x6) fuer die Uebergabe gemerkt");
    PRUEF(inv_menge(0x36) == 0, "Leon hat die Keycard NICHT (erst die Uebergabe, L6)");
    re15_sherry_r41_modell_setzen(1);
    PRUEF(!re15_story3a_r42_marvin_spielt() && !memcmp(g_inv.slots, leon_vorher, sizeof leon_vorher), "Weste (0 -> 1): kein Tausch");
    re15_sherry_r41_modell_setzen(0);
    re15_player_set_equipped_weapon(1); re15_inv_set_equipped_slot(0x80);   /* Messer (Runde 35 Spur E) */
    re15_sherry_r41_modell_setzen(RE15_S3A_MODELL_MARVIN);
    PRUEF(re15_player_equipped_weapon() == 0x03, "Marvin auch nach Leons Messer: Waffe 3");
    re15_sherry_r41_modell_setzen(0);
    PRUEF(re15_inv_equipped_slot() == 0x80 && re15_player_equipped_weapon() == 1, "Leon mit Messer: Platz 0x80, Waffe 1 zurueck");
    re15_story3a_r42_neues_spiel();
}

static void teil_tausch_elza(void)
{
    printf("== tausch_elza ==\n");
    re15_game_state_init();
    re15_story3a_r42_neues_spiel();
    g_gameflow.character = 4;
    re15_inv_init();
    inv_setzen(0, 0x03, 9); inv_setzen(1, 0x15, 20);
    re15_inv_slot_t vorher[RE15_INV_MAX_SLOTS];
    memcpy(vorher, g_inv.slots, sizeof vorher);
    const unsigned z0 = g_re15_s3a_zaehler[0], z1 = g_re15_s3a_zaehler[1];
    re15_sherry_r41_modell_init(4);
    re15_sherry_r41_modell_setzen(0x0F);                               /* ROOM3061 @0x011F6 */
    PRUEF(!re15_story3a_r42_marvin_spielt() && !memcmp(g_inv.slots, vorher, sizeof vorher),
          "Elza -> Sherry: kein Tausch, Inventar unveraendert (Runde 41 Spur K)");
    PRUEF(re15_player_equipped_weapon() == 0, "Sherry: Waffe 0 (@0x800316c4)");
    re15_sherry_r41_modell_setzen(4);                                  /* ROOM3091 @0x01D9C */
    PRUEF(!memcmp(g_inv.slots, vorher, sizeof vorher) && re15_player_equipped_weapon() == 1,
          "Sherry -> Elza: Inventar unveraendert, Waffe 1 (@0x800c00d4[4])");
    PRUEF(g_re15_s3a_zaehler[0] == z0 && g_re15_s3a_zaehler[1] == z1, "kein Tausch gezaehlt");
}

/* ---- speichern ------------------------------------------------------------------------------------------------- */
static void teil_speichern(void)
{
    printf("== speichern ==\n");
    re15_game_state_init();
    re15_story3a_r42_neues_spiel();
    g_gameflow.character = 0;
    leon_inventar();
    re15_inv_slot_t leon_vorher[RE15_INV_MAX_SLOTS];
    memcpy(leon_vorher, g_inv.slots, sizeof leon_vorher);
    re15_sherry_r41_modell_init(0);
    re15_sherry_r41_modell_setzen(RE15_S3A_MODELL_MARVIN);
    g_scd.work_vars[0x10] = RE15_S3A_MODELL_MARVIN;
    inv_setzen(2, 0x36, 1); g_inv.slots[0].qty = 11;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 57;
    g_current_room_id = 0x30C0;
    static re15_savedata_t sd;
    re15_savedata_capture(&sd, 1234, 1);
    PRUEF(sd.version == 10 && sd.story_r42[0] == 1, "Block v10, story_r42[0] (aktiv) = 1");
    re15_game_state_init(); re15_story3a_r42_neues_spiel(); re15_inv_init();
    re15_sherry_r41_modell_init(0); g_scd.work_vars[0x10] = 0;
    uint16_t raum = 0;
    PRUEF(re15_savedata_restore(&sd, &raum) == 0, "Laden ok (Raum %04X)", raum);
    PRUEF(re15_story3a_r42_marvin_spielt() && g_scd.work_vars[0x10] == RE15_S3A_MODELL_MARVIN,
          "nach dem Laden: Marvin spielt, work_vars[0x10] = 0x0D");
    PRUEF(inv_menge(0x03) == 11 && inv_menge(0x15) == 50 && inv_menge(0x36) == 1 && belegt() == 3, "Marvins Seite geladen");
    re15_sherry_r41_modell_setzen(RE15_S3A_MODELL_MARVIN);   /* Lade-Weg: Raumlader zieht das Modell nach (alt = Boot-Modell 0) */
    PRUEF(g_actors[RE15_ACTOR_SLOT_PLAYER].hp == 57, "Neuladen: HP aus dem Spielstand (57) bleibt (hp %d)", (int)g_actors[RE15_ACTOR_SLOT_PLAYER].hp);
    PRUEF(re15_player_equipped_weapon() == 0x03 && re15_inv_equipped_slot() == 0, "Neuladen: Waffe 3 aus Platz 0");
    PRUEF(!memcmp(re15_story3a_r42_figur()->leon, leon_vorher, sizeof leon_vorher), "Leons Seite im Spielstand");
    re15_sherry_r41_modell_setzen(0);
    PRUEF(!memcmp(g_inv.slots, leon_vorher, sizeof leon_vorher) && re15_player_equipped_weapon() == 0x03,
          "Rueckwechsel nach dem Laden: Leon hat alles zurueck, Waffe 3");
    PRUEF(seite_menge(re15_story3a_r42_figur()->marvin, 0x36) == 1, "Keycard fuer die Uebergabe gemerkt");
    re15_savedata_capture(&sd, 1300, 2);
    re15_story3a_r42_neues_spiel();
    PRUEF(re15_savedata_restore(&sd, &raum) == 0 && !re15_story3a_r42_marvin_spielt() &&
          seite_menge(re15_story3a_r42_figur()->marvin, 0x36) == 1 && re15_story3a_r42_figur()->marvin_offen,
          "Spielstand zwischen Rueckwechsel und Uebergabe: Funde bleiben gemerkt");
    re15_story3a_r42_neues_spiel();
}

static void teil_hebung(void)
{
    printf("== hebung ==\n");
    re15_game_state_init(); re15_story3a_r42_neues_spiel(); re15_inv_init();
    g_current_room_id = 0x3020;
    static re15_savedata_t sd;
    re15_savedata_capture(&sd, 99, 1);
    PRUEF(offsetof(re15_savedata_t, story_r42) == 940 && sizeof(re15_savedata_t) == 1040 && offsetof(re15_savedata_t, checksum) == 1036,
          "Layout v10: story_r42 @940 (= Pruefwort v9), checksum @1036, sizeof 1040");
    memset(sd.story_r42, 0x5A, sizeof sd.story_r42);
    sd.version = 9;
    uint32_t sum = 0;
    const uint8_t *p = (const uint8_t *)&sd;
    for (size_t i = 0; i < offsetof(re15_savedata_t, story_r42); i++) sum += p[i];
    memcpy((uint8_t *)&sd + offsetof(re15_savedata_t, story_r42), &sum, 4);
    PRUEF(re15_savedata_validate(&sd) == 0 && sd.version == RE15_SAVE_VERSION, "v9 -> v10 gehoben");
    int null = 1;
    for (size_t i = 0; i < sizeof sd.story_r42; i++) if (sd.story_r42[i]) null = 0;
    PRUEF(null, "v9: story_r42 = 0 (kein Tausch)");
    uint16_t raum = 0;
    PRUEF(re15_savedata_restore(&sd, &raum) == 0 && !re15_story3a_r42_marvin_spielt(), "v9-Stand laedt ohne Tausch");
}

/* ---- wiedersehen ------------------------------------------------------------------------------------------------ */
static void teil_wiedersehen(void)
{
    printf("== wiedersehen ==\n");
    re15_game_state_init();
    re15_story3a_r42_neues_spiel();
    g_gameflow.character = 0;
    re15_game_flag_set(3, RE15_S3A_A0_BIT, 1); re15_game_flag_set(4, 0xC4, 1); re15_game_flag_set(9, 85, 1);
    re15_game_flag_set(7, 0x50, 1); re15_game_flag_set(7, 0x51, 1); re15_game_flag_set(7, 0x52, 1);
    re15_s3a_figur_t s; memset(&s, 0, sizeof s);
    s.marvin_offen = 1;
    static const uint8_t ids[5] = { 0x03, 0x15, 0x36, 0x07, 0x17 }, qty[5] = { 9, 44, 1, 6, 6 };
    for (int i = 0; i < 5; i++) { s.marvin[i].id = ids[i]; s.marvin[i].qty = qty[i]; }
    uint8_t blk[RE15_S3A_SPEICHER_BYTES]; memset(blk, 0, sizeof blk); memcpy(blk, &s, sizeof s);
    re15_story3a_r42_import(blk);
    const unsigned z7 = g_re15_s3a_zaehler[7], z8 = g_re15_s3a_zaehler[8];
    if (room_boot(0x3060, -26027, 0, -22532, 0, 0) != 0) return;
    leon_inventar();
    PRUEF(typ_zahl(0x20) == 0, "Hunde tot (Kill-Ids 0x50..0x52 @0x00FB8.. in Zone 7): kein Hund");
    int marvin_da = 0, tuer = 0, schwarz_max = 0, schwarz = 0, cut4 = 0, f;
    for (f = 0; f < 9000 && !bit9(RE15_R42_BIT_3060_UEBERGABE); f++) {
        const int sys = g_scd.message_active && (g_scd.message_id == 16 || g_scd.message_id == 18 || g_scd.message_id == 20);
        frame(0, (uint16_t)((sys && (f % 20) == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (g_scd.cam_id == RE15_S3A_CUT_TUER) cut4 = 1;
        if (g_fade_ch[RE15_S1_BLENDE_KANAL].level == RE15_S1_BLENDE_SCHWARZ) { if (++schwarz > schwarz_max) schwarz_max = schwarz; }
        else schwarz = 0;
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            const re15_actor_t *a = &g_actors[i];
            if (!a->active || a->type != RE15_S3A_TYP_MARVIN) continue;
            if (!marvin_da) {
                marvin_da = 1;
                const int dx = a->x - RE15_S3A_TUER_X, dz = a->z - RE15_S3A_TUER_Z;
                tuer = dx * dx + dz * dz < 300 * 300;
            }
        }
    }
    for (int k = 0; k < 120; k++) frame(0, 0);
    PRUEF(g_re15_s3a_zaehler[7] == z7 + 1, "Wiedersehen einmal gestartet");
    PRUEF(marvin_da && tuer, "Marvin erscheint an der orangen Tuer (%d,%d) (Pos_set @0x01064)", RE15_S3A_TUER_X, RE15_S3A_TUER_Z);
    PRUEF(cut4, "Kamera %d (blickt auf die orange Tuer)", RE15_S3A_CUT_TUER);
    PRUEF(schwarz_max >= RE15_S1_BLENDE_HALTEN, "Zeitsprung: %d Bilder schwarz (>= %d)", schwarz_max, RE15_S1_BLENDE_HALTEN);
    char t[160] = "";
    for (int id = 11; id < 64; id++) if (s_msg_gesehen[id]) snprintf(t + strlen(t), sizeof t - strlen(t), "%s%d", t[0] ? "," : "", id);
    PRUEF(!strcmp(t, "12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27"), "Zeilen %s", t);
    PRUEF(g_re15_s3a_zaehler[8] == z8 + 3, "drei Gegenstaende uebergeben (Keycard, Redhawk, Magnum)");
    PRUEF(inv_menge(0x36) == 1 && inv_menge(0x07) == 6 && inv_menge(0x17) == 6, "Leon: Green Keycard, Super Redhawk x6, Magnum Bullets x6");
    PRUEF(re15_game_flag_get(3, RE15_S3A_KARTE_BIT), "Keycard uebergeben -> (3,0x3B) = 1 wie ROOM3040 sub01 @0x014DA (Kartentuer 3010, K7)");
    PRUEF(inv_menge(0x03) == 12 && inv_menge(0x15) == 30, "Leon behaelt seine Pistole/Munition; Marvins Browning/Munition bleiben bei Marvin");
    PRUEF(bit9(RE15_R42_BIT_PLAN_FABRIK) && re15_map_owned_page(RE15_S3A_FABRIK_BLATT), "Fabrikplan: Bit %d, Kartenblatt %d im Besitz",
          RE15_R42_BIT_PLAN_FABRIK, RE15_S3A_FABRIK_BLATT);
    PRUEF(bit9(RE15_R42_BIT_3060_UEBERGABE), "Bit 124 nach %d Bildern", f);
    PRUEF(typ_zahl(RE15_S3A_TYP_MARVIN) == 0, "Marvin ist durch die orange Tuer gegangen");
    PRUEF(!re15_story3a_r42_figur()->marvin_offen, "Uebergabe erledigt (nichts mehr gemerkt)");
    PRUEF(!re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27), "Riegel los (Set(2,7,0)/Set(1,27,0))");
    room_boot(0x3060, -26027, 0, -22532, 0, 0);
    for (int k = 0; k < 200; k++) frame(0, 0);
    PRUEF(g_re15_s3a_zaehler[7] == z7 + 1 && typ_zahl(RE15_S3A_TYP_MARVIN) == 0, "wieder betreten: keine zweite Szene");
}

/* ---- karte3010 -------------------------------------------------------------------------------------------------- */
static void teil_karte3010(void)
{
    printf("== karte3010 ==\n");
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    g_gameflow.character = 0;
    if (room_boot(0x3010, -7578, 0, 10065, 0, 0) != 0) return;
    PRUEF(g_aot.slots[6].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[6].event_id == 0, "vor Marvins Abschnitt: msg 0 (@0x020B2)");
    /* nach Marvins Abschnitt: Karte bei Marvin genommen, (3,0x3B) sogar gesetzt (ROOM3040 sub01 @0x014DA), Leon traegt sie NICHT.
     * room_boot leert das Inventar (scd_vm_init) - Leons Seite danach setzen und den Raumaufbau (main00 + Installer) neu laufen lassen. */
    re15_game_flag_set(3, RE15_S3A_A0_BIT, 1); re15_game_flag_set(3, RE15_S3A_KARTE_BIT, 1); re15_game_flag_set(9, 85, 1);
    room_boot(0x3010, -7578, 0, 10065, 0, 0);
    leon_inventar();
    re15_aot_init();
    scd_room_reenter(&s_rdt, -7578, 10065, 0);
    PRUEF(g_aot.slots[6].active && g_aot.slots[6].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[6].event_id == 0,
          "ohne Uebergabe (Karte in Marvins Seite): Platz 6 = msg 0 \"It's locked. An ID card is required...\" (L-10, K7 @0x020AE)");
    /* nach der Uebergabe: Karte im Inventar */
    re15_game_flag_set(9, RE15_R42_BIT_3060_UEBERGABE, 1);
    inv_setzen(5, RE15_S3A_KEYCARD, 1);
    re15_aot_init();
    scd_room_reenter(&s_rdt, -7578, 10065, 0);
    PRUEF(g_aot.slots[6].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[6].event_id == 2,
          "nach der Uebergabe: Platz 6 = sub02 (\"You've used the Green Keycard.\" @0x020D2), Typ %d Ereignis %d",
          (int)g_aot.slots[6].type, (int)g_aot.slots[6].event_id);
}

/* ---- rdttexte (NB1 B1/B5) ---------------------------------------------------------------------------------------- */
static void teil_rdttexte(void)
{
    printf("== rdttexte ==\n");
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    static const struct { uint16_t r; int id; int32_t x, z; const char *wort; } k[] = {
        { 0x3020, 0, -19500, -19000, "It won't budge." },
        { 0x3020, 1, -19500, -19000, "We could get in that way." },
        { 0x3020, 2, -19500, -19000, "Leon, give me a hand!" },
        { 0x30E0, 3, -19406, -22938, "Some dogs got his leg" } };
    for (size_t i = 0; i < sizeof k / sizeof k[0]; i++) {
        if (room_boot(k[i].r, k[i].x, 0, k[i].z, 0, 0) != 0) return;
        int len = 0, l = 0;
        const unsigned char *raw = re15_msg_get_raw(k[i].id, &len);
        const uint8_t *tab = re15_story3a_r42_text(k[i].r, (uint8_t)k[i].id, &l);
        const char *txt = re15_msg_get_text(k[i].id);
        PRUEF(raw && tab && len >= l && !memcmp(raw, tab, (size_t)l) && txt && strstr(txt, k[i].wort),
              "ROOM%04X msg %d nach dem Raumaufbau (re15_msg_load_room_block): \"%s\"", k[i].r, k[i].id, txt ? txt : "(leer)");
    }
    if (room_boot(0x3021, -19500, 0, -19000, 0, 0) != 0) return;
    for (int id = 0; id <= 2; id++) {
        const char *t0 = re15_msg_get_text(id);
        PRUEF(t0 && !strstr(t0, "budge") && !strstr(t0, "get in that way") && !strstr(t0, "give me a hand"),
              "Elzas ROOM3021 msg %d: RDT-Text, keine Leon-Glaettung (\"%s\")", id, t0 ? t0 : "(leer)");
    }
}

/* ---- weg30e0 (NB1 B2): Marvin erreicht die Rueckseite der Kartentuer (30E0 Slot 0 @0x009F2) nicht ------------------------ */
static void teil_weg30e0(void)
{
    printf("== weg30e0 ==\n");
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    re15_s3a_figur_t s; memset(&s, 0, sizeof s);
    s.aktiv = 1;                                                   /* Marvin spielt (Leons Seite gesichert) */
    uint8_t blk[RE15_S3A_SPEICHER_BYTES]; memset(blk, 0, sizeof blk); memcpy(blk, &s, sizeof s);
    re15_story3a_r42_import(blk);
    /* Ankunft aus 3030 (3030 Slot 0 @0x001D2 -> (7025,0,2700) Blick 3072). Je Blickrichtung (16 Stufen) 240 Bilder OBEN gehalten,
     * jedes Mal ab der Ankunft: der Spieler laeuft gegen die Waende des Ost-Rings (SCA Eintr. 13..18). */
    int x_min = 1 << 30, weit = 0, tuer = 0, wechsel = 0;
    for (int yaw = 0; yaw < 4096; yaw += 256) {
        if (room_boot(0x30E0, 7025, 0, 2700, (int16_t)yaw, 6) != 0) return;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        for (int f = 0; f < 240; f++) {
            frame(RE15_PAD_BIT_UP, 0);
            if (pl->x < x_min) x_min = pl->x;
            const int dx = pl->x - 7025, dz = pl->z - 2700;
            if (dx * dx + dz * dz > 1500 * 1500) weit = 1;
            if (pl->x >= -18900 && pl->x <= -18100 && pl->z >= -23500 && pl->z <= -22180) tuer = 1;
            if (g_room_change.pending) { wechsel = 1; g_room_change.pending = 0; }
        }
    }
    PRUEF(weit, "der Spieler laeuft (mehr als 1500 vom Ankunftspunkt)");
    PRUEF(x_min >= RE15_S3A_30E0_OST_X0, "Marvin bleibt im Ost-Ring: kleinstes x = %d (Westwand x %d, Slot 0 bei x -18900..-18100)", x_min,
          RE15_S3A_30E0_OST_X0);
    PRUEF(!tuer && !wechsel, "Slot 0 (@0x009F2) nie erreicht, kein Raumwechsel");
    re15_story3a_r42_neues_spiel();
}

/* ---- altstand (NB1 B6) --------------------------------------------------------------------------------------------- */
static void teil_altstand(void)
{
    printf("== altstand ==\n");
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    re15_game_flag_set(3, RE15_S3A_A0_BIT, 1);                      /* Marvins 3060-Szene gelaufen (@0x01026), Bit 123 = 0 */
    const unsigned z6 = g_re15_s3a_zaehler[6], z7 = g_re15_s3a_zaehler[7];
    if (room_boot(0x3020, -19500, 0, -19000, 0, 0) != 0) return;
    for (int f = 0; f < 240; f++) frame(0, 0);
    PRUEF(g_re15_s3a_zaehler[6] == z6 && !s_msg_gesehen[RE15_R42_MSG_3020_ERSTE + 2],
          "Spielstand nach Marvins Abschnitt (ohne frischen Rueckwechsel): kein \"Gunshots...?\" in 3020");
    /* frischer Rueckwechsel: Tausch hin und zurueck (Raumlader), dann 3020 */
    g_gameflow.character = 0;
    leon_inventar();
    re15_sherry_r41_modell_init(0);
    re15_sherry_r41_modell_setzen(RE15_S3A_MODELL_MARVIN);
    re15_sherry_r41_modell_setzen(0);
    room_boot(0x3020, -19500, 0, -19000, 0, 0);
    for (int f = 0; f < 240 && !s_msg_gesehen[RE15_R42_MSG_3020_ERSTE + 2]; f++) frame(0, 0);
    PRUEF(g_re15_s3a_zaehler[6] == z6 + 1 && s_msg_gesehen[RE15_R42_MSG_3020_ERSTE + 2], "direkt nach dem Rueckwechsel: msg 9");
    room_boot(0x3020, -19500, 0, -19000, 0, 0);
    for (int f = 0; f < 240; f++) frame(0, 0);
    PRUEF(g_re15_s3a_zaehler[6] == z6 + 1, "wieder betreten: keine zweite Zeile");
    /* Wiedersehen im Altstand (gemeinsames Inventar, nichts gemerkt) */
    re15_story3a_r42_neues_spiel();
    re15_game_flag_set(7, 0x50, 1); re15_game_flag_set(7, 0x51, 1); re15_game_flag_set(7, 0x52, 1);
    re15_game_flag_set(RE15_S3A_KARTE_BANK, RE15_S3A_KARTE_BENUTZT, 1);
    room_boot(0x3060, -26027, 0, -22532, 0, 0);
    for (int f = 0; f < 240; f++) frame(0, 0);
    PRUEF(g_re15_s3a_zaehler[7] == z7, "Altstand nach der Kartentuer ((3,0x3C) = 1): kein Wiedersehen");
    re15_game_flag_set(RE15_S3A_KARTE_BANK, RE15_S3A_KARTE_BENUTZT, 0);
    room_boot(0x3060, -26027, 0, -22532, 0, 0);
    for (int f = 0; f < 240 && g_re15_s3a_zaehler[7] == z7; f++) frame(0, 0);
    PRUEF(g_re15_s3a_zaehler[7] == z7 + 1, "Altstand vor der Kartentuer: das Wiedersehen wird nachgeholt");
    re15_story3a_r42_neues_spiel();
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "texte")) teil_texte();
    else if (!strcmp(t, "programme")) teil_programme();
    else if (!strcmp(t, "tausch")) teil_tausch();
    else if (!strcmp(t, "tausch_elza")) teil_tausch_elza();
    else if (!strcmp(t, "speichern")) teil_speichern();
    else if (!strcmp(t, "hebung")) teil_hebung();
    else if (!strcmp(t, "wiedersehen")) teil_wiedersehen();
    else if (!strcmp(t, "karte3010")) teil_karte3010();
    else if (!strcmp(t, "rdttexte")) teil_rdttexte();
    else if (!strcmp(t, "weg30e0")) teil_weg30e0();
    else if (!strcmp(t, "altstand")) teil_altstand();
    else { printf("unbekannter Teil '%s'\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
