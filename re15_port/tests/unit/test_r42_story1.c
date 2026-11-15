/*
 * test_r42_story1.c — Runde 42 "story" (v0.8.31), Stufe STAGE1: Szene vor der Leiter 11A0/11A1. Riegel ueber die ECHTEN Engine-Wege
 * (scd_room_reenter mit allen Installern, SCD-VM, re15_game_step), Harness-Muster test_r42_story2.c. Konstanten/Belege
 * include/re15_story1_r42.h, Code engine/src/story1_r42.c. PORT-STORY nach dem Konzept (3.2 L3, 4.2 E2, 6b; K-07, K-08, F-26).
 *
 * Teile (Aufruf: test_r42_story1 <teil>):
 *   texte      11A0 msg 3..6, 11A1 msg 3..10: Untertitel `04 00 05 cc <Name:> 05 00 00 .. 04 01 01 63`, Sprecher je VERTRAG 2.3
 *   programme  Leon 3,4,5,6 / Elza 3,4,5,10 / Elza mit Uhr 3..10 in Reihenfolge, Set(9,118,1) + Set(9,120,1), Ende Evt_end
 *   szene      Leon: Mod-Marvin (@0x0118C) nicht da; Spieler in der Ausloesezone (Band 1, nach dem Klettern) -> Szene L3, Marvin
 *              kommt (Band 0) und steht bei N, Spieler bei P, Zeilen 3..6; Abgang (Nachbesserung 1, B1/B2): Marvin geht an den
 *              Fuss der Kante K, Abblende, mindestens RE15_S1_BLENDE_HALTEN Bilder Schwarz, im Schwarz Kamera 2 und Marvin oben
 *              (Band 1), dort geht er nach Norden bis Z, Kamera 3, dann weg; Bit 120; Uhr bleibt (F-19); nie ein Kanten-Sub;
 *              wieder betreten: keine Szene, kein Marvin; in der Kletterbewegung (231) keine Ausloesung
 *   szene_elza / szene_elza_uhr  dasselbe mit John + Sherry (Sherry folgt ihm an B, oben bis ZS); mit Uhr Zeilen 6..9, Sherry an
 *              der Kante unter Elza (H), Elza kniet (PL00 Clip 11, Form ROOM2040 sub08) und die Uhr verlaesst das Inventar (B4)
 *   altstand   (B5) 4:198 = 1 (Szene 3000/3001 gelaufen, Bedingung sub00 @0x01188): Spieler in der Zone -> keine Szene, Bit 120 bleibt
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
static int inv_geben(uint8_t id)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == 0) { g_inv.slots[i].id = id; g_inv.slots[i].qty = 1; return i; }
    return -1;
}
static int typ_zahl(uint8_t typ)
{
    int n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == typ) n++;
    return n;
}
static int bit9(int b) { return re15_game_flag_get(9, (uint8_t)b) ? 1 : 0; }

static int zeilen_folge(int *ids, int max)
{
    int n = 0;
    for (int id = 0; id < 64 && n < max; id++) if (s_msg_gesehen[id]) ids[n++] = id;
    return n;
}

/* ---- texte ------------------------------------------------------------------------------------------------- */
static void teil_texte(void)
{
    printf("== texte ==\n");
    static const struct { uint16_t r; int id; const char *name; } k[] = {
        { 0x11A0, 3, "Marvin:" }, { 0x11A0, 4, "Marvin:" }, { 0x11A0, 5, "Leon:" }, { 0x11A0, 6, "Marvin:" },
        { 0x11A1, 3, "Sherry:" }, { 0x11A1, 4, "John:" }, { 0x11A1, 5, "Elza:" }, { 0x11A1, 6, "Elza:" }, { 0x11A1, 7, "Sherry:" },
        { 0x11A1, 8, "Elza:" }, { 0x11A1, 9, "Elza:" }, { 0x11A1, 10, "John:" } };
    for (size_t i = 0; i < sizeof k / sizeof k[0]; i++) {
        int l = 0;
        const uint8_t *b = re15_story1_r42_text(k[i].r, (uint8_t)k[i].id, &l);
        int ok = b && l > 8 && b[0] == 0x04 && b[1] == 0x00 && b[2] == 0x05 && !memcmp(b + l - 4, "\x04\x01\x01\x63", 4);
        /* Sprecherkopf: Glyphen wie texte_stage2.py (A = 0x1D, a = 0x3D, ':' = 0x16) */
        const char *nm = k[i].name;
        for (int j = 0; ok && nm[j]; j++) {
            const char c = nm[j];
            const uint8_t g = (c >= 'A' && c <= 'Z') ? (uint8_t)(0x1D + c - 'A') : (c >= 'a' && c <= 'z') ? (uint8_t)(0x3D + c - 'a') : 0x16;
            if (b[4 + j] != g) ok = 0;
        }
        PRUEF(ok, "ROOM%04X msg %d: Untertitel mit Kopf %s", k[i].r, k[i].id, k[i].name);
    }
    PRUEF(!re15_story1_r42_text(0x11A0, 7, NULL), "11A0 msg 7 gibt es nicht (Leon 3..6)");
    PRUEF(!re15_story1_r42_text(0x11A1, 11, NULL), "11A1 msg 11 nicht hier (story2: Elza unlocked the door.)");
}

/* ---- programme ----------------------------------------------------------------------------------------------- */
static void teil_programme(void)
{
    printf("== programme ==\n");
    static const char *const name[3] = { "Leon", "Elza ohne Uhr", "Elza mit Uhr" };
    static const int soll[3][8] = { { 3, 4, 5, 6 }, { 3, 4, 5, 10 }, { 3, 4, 5, 6, 7, 8, 9, 10 } };
    static const int nsoll[3] = { 4, 4, 8 };
    for (int f = 0; f < 3; f++) {
        int n = 0;
        const uint8_t *p = re15_story1_r42_programm(f, &n);
        int folge[16], nf = 0, b118 = 0, b120 = 0, uhr = 0;
        for (int i = 0; i + 3 < n; i++) {
            if (p[i] == 0x2b && p[i + 2] == 0x00 && p[i + 3] == 0x00 && p[i + 1] >= 3 && p[i + 1] <= 10 && nf < 16) folge[nf++] = p[i + 1];
            if (p[i] == 0x22 && p[i + 1] == 9 && p[i + 2] == RE15_R42_BIT_2080_SZENE && p[i + 3] == 1) b118 = 1;
            if (p[i] == 0x22 && p[i + 1] == 9 && p[i + 2] == RE15_R42_BIT_LEITER && p[i + 3] == 1) b120 = 1;
            if (p[i] == 0x22 && p[i + 1] == 5 && p[i + 2] == RE15_S1_SIG_UHR && p[i + 3] == 1) uhr = 1;
        }
        int ok = nf == nsoll[f];
        for (int i = 0; ok && i < nf; i++) ok = folge[i] == soll[f][i];
        PRUEF(ok, "%s: %d Zeilen in Reihenfolge (gezaehlt %d)", name[f], nsoll[f], nf);
        PRUEF(b118 && b120, "%s: Set(9,118,1) und Set(9,120,1)", name[f]);
        PRUEF(uhr == (f == 2), "%s: Uebergabe-Signal der Uhr %s", name[f], f == 2 ? "vorhanden" : "fehlt");
        PRUEF(n >= 2 && p[n - 2] == 0x01 && p[n - 1] == 0x00, "%s: Ende Evt_end `01 00`", name[f]);
        /* Abgang (B1/B2): Cut_chg 2 im Schwarz, Sce_em_set Band 1 je Begleiter; Uebergabe (B4): Plc_motion(1,11) knien + aufstehen */
        int cut2 = 0, oben = 0, knien = 0, rueck = 0;
        for (int i = 0; i + 4 < n; i++) {
            if (p[i] == 0x29 && p[i + 1] == RE15_S1_CUT_ABGANG) cut2 = 1;
            if (p[i] == 0x44 && i + 19 < n && p[i + 3] == RE15_S1_NPC_GRID && p[i + 4] == RE15_S1_BAND_OBEN) oben++;
            if (p[i] == 0x3f && p[i + 1] == 0x01 && p[i + 2] == 0x0b && p[i + 3] == 0x00) knien++;
            if (p[i] == 0x43 && p[i + 1] == 0x00 && p[i + 2] == 0x80 && p[i + 3] == 0x00) rueck = 1;
        }
        PRUEF(cut2, "%s: Cut_chg %d (Abgang im Abschnitt)", name[f], RE15_S1_CUT_ABGANG);
        PRUEF(oben == (f == 0 ? 1 : 2), "%s: %d Begleiter oben gesetzt (Sce_em_set Band %d)", name[f], oben, RE15_S1_BAND_OBEN);
        PRUEF((knien == 2 && rueck) == (f == 2), "%s: Knien/Aufstehen (Plc_motion(1,11) x%d, Plc_flg 0x80 %d)", name[f], knien, rueck);
    }
}

/* ---- szene --------------------------------------------------------------------------------------------------- */
static int nahe(int32_t x, int32_t z, int32_t sx, int32_t sz, int32_t r)
{
    const int64_t dx = (int64_t)x - sx, dz = (int64_t)z - sz;
    return dx * dx + dz * dz <= (int64_t)r * r;
}
static int ist_begleiter_typ(uint8_t t)
{
    return t == RE15_S1_TYP_MARVIN || t == RE15_S1_TYP_JOHN || t == RE15_S1_TYP_SHERRY;
}
static int begleiter_zahl(void)
{
    return typ_zahl(RE15_S1_TYP_MARVIN) + typ_zahl(RE15_S1_TYP_JOHN) + typ_zahl(RE15_S1_TYP_SHERRY);
}
static void teil_szene(int elza, int uhr)
{
    printf("== szene%s%s ==\n", elza ? "_elza" : "", uhr && elza ? "_uhr" : "");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(9, RE15_R42_BIT_2080_SZENE, 1);
    re15_game_flag_set(3, 151, 1); re15_game_flag_set(9, RE15_R42_BIT_TUER_11A, 1);
    if (uhr) re15_game_flag_set(9, 90, 1);
    const uint16_t raum = elza ? 0x11A1 : 0x11A0;
    const unsigned z0 = g_re15_s1_zaehler[0], z1 = g_re15_s1_zaehler[1], z2 = g_re15_s1_zaehler[2];
    if (room_boot(raum, 321, -1800, 338, 3072, RE15_S1_CUT_SZENE) != 0) return;
    if (uhr) PRUEF(inv_geben(RE15_S1_UHR_ITEM) >= 0, "Uhr im Inventar (nach scd_vm_init, das es leert)");
    PRUEF(begleiter_zahl() == 0, "Mod-Begleiter (sub00 @0x0118C%s) verworfen (K-07)", elza ? "/@0x011A0" : "");
    /* Kletterbewegung: in ihr loest die Zone nicht aus (re15_story1_r42.h, Lauf s1/l3) */
    for (int f = 0; f < 5; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].motion = RE15_PLAYER_MOTION_CLIMB_UP; frame(0, 0); }
    PRUEF(g_re15_s1_zaehler[0] == z0, "waehrend der Kletterbewegung 231 keine Ausloesung");
    g_actors[RE15_ACTOR_SLOT_PLAYER].motion = 0;
    int npc_da = 0, f = 0, npc_bei_n = 0, sherry_bei_s = 0, pl_bei_p = 0;
    int npc_bei_k = 0, sherry_bei_b = 0, sherry_bei_h = 0, knie = 0, kante_sub = 0, cut2 = 0, schwarz = 0, schwarz_max = 0;
    int npc_oben_z = 0, sherry_oben_z = 0, weg_in_cut = -1;
    for (f = 0; f < 8000 && !bit9(RE15_R42_BIT_LEITER); f++) {
        const int vorher = begleiter_zahl();
        frame(0, 0);
        if (vorher > 0 && begleiter_zahl() == 0 && weg_in_cut < 0 && cut2) weg_in_cut = (int)g_scd.cam_id;
        if (g_scd.cam_id == RE15_S1_CUT_ABGANG) cut2 = 1;
        /* Schwarz = statischer Pegel 0x7fff im Kanal (Takt Stufe 2) */
        if (g_fade_ch[RE15_S1_BLENDE_KANAL].level == RE15_S1_BLENDE_SCHWARZ) { if (++schwarz > schwarz_max) schwarz_max = schwarz; }
        else schwarz = 0;
        if (g_actors[RE15_ACTOR_SLOT_PLAYER].motion == 11 && g_actors[RE15_ACTOR_SLOT_PLAYER].anim_use_pl00) knie = 1;
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            const re15_actor_t *a = &g_actors[i];
            if (!a->active) continue;
            if (a->type == (elza ? RE15_S1_TYP_JOHN : RE15_S1_TYP_MARVIN)) {
                if (!npc_da) {
                    npc_da = 1;
                    PRUEF(nahe(a->x, a->z, RE15_S1_NPC_START_X, RE15_S1_NPC_START_Z, 600) && a->floor == RE15_S1_BAND_UNTEN,
                          "Begleiter erscheint im unteren Gang im Osten (%d,%d) Band %d", (int)a->x, (int)a->z, (int)a->floor);
                }
                if (nahe(a->x, a->z, RE15_S1_NPC_X, RE15_S1_NPC_Z, 400)) npc_bei_n = 1;
                if (a->floor == RE15_S1_BAND_UNTEN && nahe(a->x, a->z, RE15_S1_KANTE_X, RE15_S1_KANTE_Z, 150)) npc_bei_k = 1;
                if (a->floor == RE15_S1_BAND_OBEN && (int)a->z > npc_oben_z) npc_oben_z = (int)a->z;
            }
            if (a->type == RE15_S1_TYP_SHERRY) {
                if (nahe(a->x, a->z, RE15_S1_SHERRY_X, RE15_S1_SHERRY_Z, 400)) sherry_bei_s = 1;
                if (a->floor == RE15_S1_BAND_UNTEN && nahe(a->x, a->z, RE15_S1_UEBERGABE_X, RE15_S1_UEBERGABE_Z, 150)) sherry_bei_h = 1;
                if (a->floor == RE15_S1_BAND_UNTEN && nahe(a->x, a->z, RE15_S1_HINTER_X, RE15_S1_HINTER_Z, 150)) sherry_bei_b = 1;
                if (a->floor == RE15_S1_BAND_OBEN && (int)a->z > sherry_oben_z) sherry_oben_z = (int)a->z;
            }
            if (ist_begleiter_typ(a->type) && re15_npckante_aktiv(a)) kante_sub = 1;
        }
        if (nahe(g_actors[0].x, g_actors[0].z, RE15_S1_SPIELER_X, RE15_S1_SPIELER_Z, 150)) pl_bei_p = 1;
    }
    PRUEF(g_re15_s1_zaehler[0] == z0 + 1, "Szene einmal gestartet");
    PRUEF(npc_bei_n, "Begleiter kam bei N (%d,%d) an", RE15_S1_NPC_X, RE15_S1_NPC_Z);
    if (elza) PRUEF(sherry_bei_s, "Sherry kam bei S (%d,%d) an", RE15_S1_SHERRY_X, RE15_S1_SHERRY_Z);
    else PRUEF(typ_zahl(RE15_S1_TYP_SHERRY) == 0, "Leon: keine Sherry");
    PRUEF(pl_bei_p, "Spieler trat an die Kante P (%d,%d)", RE15_S1_SPIELER_X, RE15_S1_SPIELER_Z);
    int ids[16];
    const int n = zeilen_folge(ids, 16);
    char t[96] = "";
    for (int i = 0; i < n; i++) snprintf(t + strlen(t), sizeof t - strlen(t), "%s%d", i ? "," : "", ids[i]);
    const char *soll = !elza ? "3,4,5,6" : uhr ? "3,4,5,6,7,8,9,10" : "3,4,5,10";
    PRUEF(!strcmp(t, soll), "Zeilen %s (soll %s)", t, soll);
    PRUEF(bit9(RE15_R42_BIT_LEITER) && bit9(RE15_R42_BIT_2080_SZENE), "Bit 120 (und 118) = 1 nach %d Bildern", f);
    PRUEF(g_re15_s1_zaehler[2] == z2 + 1, "Abblende einmal");
    PRUEF(npc_bei_k, "B1: Begleiter ging vor der Abblende an den Fuss der Kante K (%d,%d)", RE15_S1_KANTE_X, RE15_S1_KANTE_Z);
    if (elza) PRUEF(sherry_bei_b, "B2: Sherry folgte ihm an B (%d,%d) vor der Abblende", RE15_S1_HINTER_X, RE15_S1_HINTER_Z);
    PRUEF(schwarz_max >= RE15_S1_BLENDE_HALTEN, "B1: Schwarz %d Bilder (>= %d, PORT-WAHL Zeitsprung)", schwarz_max, RE15_S1_BLENDE_HALTEN);
    PRUEF(cut2, "B1: Abgang in Kamera %d", RE15_S1_CUT_ABGANG);
    PRUEF(npc_oben_z >= RE15_S1_ZIEL_Z - 150, "B1: Begleiter ging oben bis z %d (Ziel %d)", npc_oben_z, RE15_S1_ZIEL_Z);
    if (elza) PRUEF(sherry_oben_z >= RE15_S1_ZIEL_SHERRY_Z - 150, "B2: Sherry ging oben bis z %d (Ziel %d)", sherry_oben_z,
                    RE15_S1_ZIEL_SHERRY_Z);
    PRUEF(weg_in_cut == RE15_S1_CUT_SZENE, "B1: Begleiter verlassen den Raum erst in Kamera %d (war %d)", RE15_S1_CUT_SZENE, weg_in_cut);
    PRUEF(!kante_sub, "kein Kanten-Sub (Sonde FUN_8002d7d8 trifft nie)");
    if (elza && uhr) {
        PRUEF(sherry_bei_h, "B4: Sherry an der Kante unter Elza (H %d,%d)", RE15_S1_UEBERGABE_X, RE15_S1_UEBERGABE_Z);
        PRUEF(knie, "B4: Elza kniet (PL00 Clip 11, Plc_motion(1,11) Form ROOM2040 sub08 @0x01722)");
    } else PRUEF(!knie, "ohne Uebergabe kein Knien");
    for (int k = 0; k < 30; k++) frame(0, 0);
    PRUEF(begleiter_zahl() == 0, "Begleiter nach der Abblende nicht mehr im Raum");
    PRUEF(g_fade_ch[RE15_S1_BLENDE_KANAL].level & 0x8000u, "Blende fertig (Kanal %d Bit 15)", RE15_S1_BLENDE_KANAL);
    PRUEF(!re15_game_flag_get(1, 27) && !re15_game_flag_get(2, 7), "Spieler wieder frei (Riegel (1,27)/(2,7) los)");
    if (elza && uhr) PRUEF(re15_inv_find_item(RE15_S1_UHR_ITEM) < 0 && g_re15_s1_zaehler[1] == z1 + 1, "Uhr an Sherry uebergeben");
    else if (uhr) PRUEF(re15_inv_find_item(RE15_S1_UHR_ITEM) >= 0 && g_re15_s1_zaehler[1] == z1, "Leon behaelt die Uhr (F-19)");
    else PRUEF(g_re15_s1_zaehler[1] == z1, "ohne Uhr: nichts uebergeben");
    /* Wiederbetreten */
    if (room_boot(raum, 321, -1800, 338, 3072, RE15_S1_CUT_SZENE) != 0) return;
    for (int k = 0; k < 60; k++) frame(0, 0);
    PRUEF(g_re15_s1_zaehler[0] == z0 + 1 && begleiter_zahl() == 0, "Wiederbetreten: keine zweite Szene, kein Begleiter");
}

/* ---- altstand (B5) --------------------------------------------------------------------------------------------- */
static void teil_altstand(void)
{
    printf("== altstand ==\n");
    for (int elza = 0; elza < 2; elza++) {
        re15_game_state_init();
        re15_inv_load_briefing();
        re15_game_flag_set(9, RE15_R42_BIT_2080_SZENE, 1);
        re15_game_flag_set(3, 151, 1); re15_game_flag_set(9, RE15_R42_BIT_TUER_11A, 1);
        re15_game_flag_set(RE15_S1_BANK_3000, RE15_S1_BIT_3000, 1);   /* 3000 sub03 @0x0166E / 3001 @0x016A6 schon gelaufen */
        const uint16_t raum = elza ? 0x11A1 : 0x11A0;
        const unsigned z0 = g_re15_s1_zaehler[0];
        if (room_boot(raum, 321, -1800, 338, 3072, RE15_S1_CUT_SZENE) != 0) return;
        for (int k = 0; k < 120; k++) frame(0, 0);
        PRUEF(g_re15_s1_zaehler[0] == z0 && begleiter_zahl() == 0 && !bit9(RE15_R42_BIT_LEITER),
              "ROOM%04X mit 4:198 = 1 (Altstand jenseits 3000): Spieler in der Zone, keine Szene, kein Begleiter", raum);
        /* Gegenprobe: dieselbe Lage ohne 4:198 loest aus */
        re15_game_flag_set(RE15_S1_BANK_3000, RE15_S1_BIT_3000, 0);
        if (room_boot(raum, 321, -1800, 338, 3072, RE15_S1_CUT_SZENE) != 0) return;
        for (int k = 0; k < 5; k++) frame(0, 0);
        PRUEF(g_re15_s1_zaehler[0] == z0 + 1, "ROOM%04X Gegenprobe 4:198 = 0: Szene startet", raum);
        scd_vm_init();
    }
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "texte")) teil_texte();
    else if (!strcmp(t, "programme")) teil_programme();
    else if (!strcmp(t, "szene")) teil_szene(0, 1);
    else if (!strcmp(t, "szene_elza")) teil_szene(1, 0);
    else if (!strcmp(t, "szene_elza_uhr")) teil_szene(1, 1);
    else if (!strcmp(t, "altstand")) teil_altstand();
    else { printf("unbekannter Teil '%s'\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
