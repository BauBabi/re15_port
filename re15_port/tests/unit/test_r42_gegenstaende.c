/*
 * test_r42_gegenstaende.c — Runde 42 Strang "gegenstaende" (v0.8.31, Konzept K-10), Stufe 1: Riegel je Gegenstand (Fundort, Aufnahme,
 * Verwendung, Bits) ueber die ECHTEN Engine-Wege (scd_room_reenter mit allen Installern, SCD-VM, Aufnahme-Modal, Inventar).
 * Belege und Konstanten: include/re15_gegenstaende_r42.h. Harness = test_r42_story3b.c. Der Riegel benutzt NUR Schnittstellen, die
 * es schon vor dem Strang gab (g_aot, scd_event_fire, re15_item_modal_*, re15_item_use_is_heal, re15_keyitems_r41_testlauf), damit die
 * Gegenprobe auf aaea5f8c2 baut und dort ROT wird.
 *
 * Teile (Aufruf: test_r42_gegenstaende <teil>):
 *   stellen   je Fundstelle: Zone mit Item/Menge/Bit am erwarteten Slot und Rechteck; Vorrang-Satz unveraendert umgezogen;
 *             Bit gesetzt -> keine Zone, Original-Satz bleibt am Platz
 *   python    2040/2041 Slot 7 = Colt Python x6, Bit 91 (statt Head of Akuma 0x32)
 *   leiche    1050 Slot 11 Ereignis 5 + Slot 12 Glock x15 Bit 40; Ereignis 5 -> Text 5 -> Aufnahme-Modal Glock; 1051 unveraendert
 *   mutter    Mutter kein Heilmittel; 2030 Pult ohne Mutter -> Text 56 statt Panel; USE -> (9,43), Text 58, Wegwerfen, danach Panel
 *   granaten  genau EIN Zuender je Szenario, keine weitere Handgranaten-Stelle
 *   bilder    Item-Bilder der Kapseln 0x1E/0x1F/0x20 sind keine Platzhalter mehr (im Mod-Baum sind die drei Bloecke gleich:
 *             Capcoms 仮キャラ-Karte), sondern die Inventar-Kachel 2-fach (2x2-Bloecke im Kachelfeld x 16..95, y 6..65)
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

/* ---- Erwartete Fundstellen (unabhaengig vom Modul aufgeschrieben; Belege re15_gegenstaende_r42.h / gegenstaende_r42.c) ---------- */
typedef struct { uint16_t raum; uint8_t item, menge, bit; int vorrang; uint8_t org_item; int32_t x, z, w, d; } erwartet_t;
static const erwartet_t k_erw[] = {
    { 0x2010, 0x2F, 1,   41,  3, 0x22, 2500, 4700, 800, 800 },
    { 0x2011, 0x2F, 1,   41,  3, 0x22, 2500, 4700, 800, 800 },
    { 0x11F0, 0x02, 1,   44, 13, 0,    10200, -15950, 1500, 1000 },
    { 0x11F1, 0x02, 1,   44, 13, 0,    10200, -15950, 1500, 1000 },
    { 0x1071, 0x0F, 6,   45, -1, 0,    344, 13624, 1000, 1000 },
    { 0x1191, 0x1D, 6,   46, 11, 0x16, 19700, -19350, 1000, 1000 },
    { 0x30E1, 0x1D, 6,   47,  5, 0x17, 25100, 13600, 1000, 1000 },
    { 0x3000, 0x1C, 1,   48, -1, 0,    -16800, -17200, 1000, 1000 },
    { 0x3001, 0x1C, 1,   48, -1, 0,    -16800, -17200, 1000, 1000 },
    { 0x30E0, 0x1E, 1,   49,  8, 0x24, 26300, 13900, 1000, 1000 },
    { 0x30E1, 0x1E, 6,   49,  4, 0x24, 26300, 13900, 1000, 1000 },
    { 0x40B1, 0x1F, 6,   50,  5, 0x17, -19900, -11600, 1000, 1000 },
    { 0x4050, 0x18, 100, 51, 14, 0,    -6900, -16850, 1000, 1200 },
    { 0x4051, 0x1F, 6,   51, 14, 0,    -6900, -16850, 1000, 1200 },
    { 0x5021, 0x20, 6,   170, -1, 0,   -2250, -14500, 900, 900 },
};
#define N_ERW ((int)(sizeof k_erw / sizeof k_erw[0]))

static int zone_mit_item(uint8_t item, uint8_t bit)
{
    for (int s = 0; s < RE15_AOT_MAX; s++)
        if (g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_ITEM && g_aot.item_params[s].item_type == item &&
            g_aot.item_params[s].taken_bit == bit) return s;
    return -1;
}

static void teil_stellen(void)
{
    printf("== stellen ==\n");
    for (int i = 0; i < N_ERW; i++) {
        const erwartet_t *e = &k_erw[i];
        bits_loeschen();
        const uint8_t cut = (e->raum == 0x1191) ? 0x0C : 0;   /* 1191: Waffenkammer = Eintritt Cut 0x0c (main00 @0x022DC) */
        if (room_boot(e->raum, e->x + e->w / 2, 0, e->z + e->d / 2 + 3000, 0, cut) != 0) continue;
        const int s = zone_mit_item(e->item, e->bit);
        PRUEF(s >= 0, "ROOM%04X: Zone Item 0x%02X Bit (9,%u) da (Slot %d)", e->raum, e->item, e->bit, s);
        if (s < 0) continue;
        const re15_aot_t *a = &g_aot.slots[s];
        PRUEF(g_aot.item_params[s].amount == e->menge && g_aot.item_params[s].taken_prop == 0xFF,
              "ROOM%04X: Menge %u (soll %u), kein Weltmodell", e->raum, g_aot.item_params[s].amount, e->menge);
        PRUEF(a->x == e->x + e->w / 2 && a->z == e->z + e->d / 2 && a->half_w == e->w / 2 && a->half_h == e->d / 2 &&
              a->sce_flags == 0x31,
              "ROOM%04X: Rechteck Mitte (%d,%d) halb %dx%d sat 0x%02x", e->raum, (int)a->x, (int)a->z, (int)a->half_w,
              (int)a->half_h, a->sce_flags);
        if (e->vorrang >= 0) {
            PRUEF(s == e->vorrang, "ROOM%04X: Vorrang-Slot %d (ist %d)", e->raum, e->vorrang, s);
            int org = -1;
            for (int k = 0; k < RE15_AOT_MAX; k++) {
                if (k == s || !g_aot.slots[k].active) continue;
                const re15_aot_t *b = &g_aot.slots[k];
                if (b->x != a->x || b->z != a->z) continue;
                if (e->org_item ? (b->type == RE15_AOT_TYPE_ITEM && g_aot.item_params[k].item_type == e->org_item)
                                : (b->type == RE15_AOT_TYPE_MESSAGE || b->type == RE15_AOT_TYPE_GENERIC)) { org = k; break; }
            }
            PRUEF(org > s, "ROOM%04X: Original-Satz am selben Rechteck nach Slot %d (> %d) umgezogen", e->raum, org, s);
        }
        /* genommen -> keine Zone, der Vorrang-Slot traegt wieder den Original-Satz */
        bits_loeschen();
        re15_game_flag_set(9, e->bit, 1);
        room_boot(e->raum, e->x + e->w / 2, 0, e->z + e->d / 2 + 3000, 0, cut);
        PRUEF(zone_mit_item(e->item, e->bit) < 0, "ROOM%04X: Bit (9,%u) gesetzt -> keine Zone", e->raum, e->bit);
        if (e->vorrang >= 0)
            PRUEF(g_aot.slots[e->vorrang].active && (e->org_item == 0 || g_aot.item_params[e->vorrang].item_type == e->org_item),
                  "ROOM%04X: Slot %d wieder Original", e->raum, e->vorrang);
    }
}

static void teil_python(void)
{
    printf("== python ==\n");
    for (int v = 0; v < 2; v++) {
        bits_loeschen();
        const uint16_t r = (uint16_t)(0x2040 + v);
        if (room_boot(r, 1700, -5400, -14000, 0, 0) != 0) continue;
        PRUEF(g_aot.slots[7].active && g_aot.slots[7].type == RE15_AOT_TYPE_ITEM && g_aot.item_params[7].item_type == 0x14 &&
              g_aot.item_params[7].amount == 6 && g_aot.item_params[7].taken_bit == 91,
              "ROOM%04X Slot 7: Item 0x%02X x%u Bit %u (soll Colt Python 0x14 x6 Bit 91)", r, g_aot.item_params[7].item_type,
              g_aot.item_params[7].amount, g_aot.item_params[7].taken_bit);
        PRUEF(zone_mit_item(0x32, 91) < 0, "ROOM%04X: kein Head of Akuma mehr", r);
    }
}

static int bis_nachricht(int id, int max)
{
    for (int f = 0; f < max; f++) {
        frame(0, 0);
        if (g_scd.message_active && g_scd.message_id == id) return f;
    }
    return -1;
}

/* Texte schliessen, bis keiner mehr steht und das Wegwerf-/Aufnahme-Fenster zu ist; danach ohne Taste (kein neuer Aktions-Treffer). */
static void zu(int max)
{
    int ruhe = 0;
    for (int f = 0; f < max && ruhe < 20; f++) {
        const int offen = g_scd.message_active || re15_discard_active() || re15_item_modal_active();
        const uint16_t k = (offen && (f % 4) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(k, k);
        ruhe = offen ? 0 : ruhe + 1;
    }
}

static void teil_leiche(void)
{
    printf("== leiche ==\n");
    bits_loeschen();
    re15_inv_init();
    if (room_boot(0x1050, 15750, 0, -5800, 2048, 3) != 0) return;
    const re15_aot_t *a = &g_aot.slots[11];
    PRUEF(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == 5 && a->x == 15750 && a->z == -6750,
          "ROOM1050 Slot 11: Leichen-Platz Ereignis 5 (Form 1051 @0x00C0E), Typ %d Ereignis %d", a->type, a->event_id);
    PRUEF(g_aot.slots[12].active && g_aot.item_params[12].item_type == 0x06 && g_aot.item_params[12].amount == 15 &&
          g_aot.item_params[12].taken_bit == 40 && g_aot.slots[12].half_w == 0,
          "ROOM1050 Slot 12: Glock 18 x15 Bit 40, Rechteck 0 (Form 1051 @0x00C22)");
    PRUEF(scd_event_fire(5) >= 0, "Ereignis 5 startet");
    PRUEF(bis_nachricht(5, 120) >= 0, "Text 5 (\"... Sorry, I'll just borrow this...\") erscheint");
    int modal = 0;
    for (int f = 0; f < 300 && !modal; f++) {
        const uint16_t k = (f % 6) == 0 ? RE15_PAD_BIT_SQUARE : 0;
        frame(k, k);
        if (re15_item_modal_active()) {
            int qx[4], qy[4], face = 0; uint8_t t = 0;
            re15_item_modal_tick(0, 0);
            if (re15_item_modal_quad(qx, qy, &t, &face) && t == 0x06) modal = 1;
        }
    }
    PRUEF(modal, "Aot_on 12 -> Aufnahme-Modal mit der Glock (0x06)");
    /* Elza 1051 unveraendert: Original-Satz SIG x15 Bit 165 */
    bits_loeschen();
    room_boot(0x1051, 15750, 0, -5800, 2048, 3);
    PRUEF(g_aot.item_params[12].item_type == 0x04 && g_aot.item_params[12].taken_bit == 165, "ROOM1051 Slot 12 bleibt SIG P228 Bit 165");
}

static void teil_mutter(void)
{
    printf("== mutter ==\n");
    PRUEF(!re15_item_use_is_heal(0x2F), "0x2F Mutter ist kein Heilmittel (PORT-KORREKTUR @0x8004ab48/@0x8004ae98)");
    PRUEF(re15_item_use_is_heal(0x22) && re15_item_use_is_heal(0x2E) && !re15_item_use_is_heal(0x25), "Heilmittel 0x22..0x2E unveraendert");

    bits_loeschen();
    re15_inv_init();
    re15_inv_grant(0x2F, 1);
    if (room_boot(0x2030, 350, -5400, -9900, 1024, 0) != 0) return;
    PRUEF(g_aot.slots[12].active && g_aot.slots[12].event_id == 24, "2030 Pult Slot 12 Ereignis 24");
    PRUEF(scd_event_fire(24) >= 0, "Pult ohne Mutter: Ereignis 24 startet");
    PRUEF(bis_nachricht(56, 60) >= 0, "Pult ohne Mutter -> Text 56 (Handrad dreht durch) statt Panel-Frage 40");
    zu(400);
    PRUEF(!re15_game_flag_get(9, 43), "Aktionstaste allein setzt die Mutter nicht ein ((9,43)=0)");

    PRUEF(re15_keyitems_r41_testlauf(0x2F) == 1, "USE Mutter am Pult: Stelle gefunden");
    PRUEF(bis_nachricht(58, 120) >= 0 && re15_game_flag_get(9, 43), "USE -> Text 58 \"You've fastened ...\" und (9,43)=1");
    zu(600);
    PRUEF(re15_inv_find_item(0x2F) < 0, "Mutter nach der Wegwerf-Abfrage nicht mehr im Inventar");
    PRUEF(scd_event_fire(24) >= 0 && bis_nachricht(40, 60) >= 0, "danach wie bisher: Ereignis 24 -> Panel-Frage 40");
}

static void teil_granaten(void)
{
    printf("== granaten ==\n");
    for (int figur = 0; figur < 2; figur++) {
        int zuender = 0, granaten = 0;
        for (int i = 0; i < N_ERW; i++) {
            if ((k_erw[i].raum & 1u) != (unsigned)figur) continue;
            if (k_erw[i].item == 0x1C) zuender += k_erw[i].menge;
            if (k_erw[i].item >= 0x09 && k_erw[i].item <= 0x0B) granaten++;
        }
        PRUEF(zuender == 1 && granaten == 0, "%s: Zuender gesamt %d (soll 1), neue Handgranaten-Stellen %d (soll 0)",
              figur ? "Elza" : "Leon", zuender, granaten);
        bits_loeschen();
        room_boot((uint16_t)(0x3000 + figur), -16300, 0, -14000, 0, 0);
        const int s = zone_mit_item(0x1C, 48);
        PRUEF(s >= 0 && g_aot.item_params[s].amount == 1 && re15_pickup_menge_nutzer(0x1C, 1) == 1,
              "ROOM%04X: Zuender-Zone x1, ins Inventar 1 (Halbierung laesst 1 stehen)", 0x3000 + figur);
    }
}

static int px_gleich(uint8_t a, uint8_t b, int u, int v)
{
    uint8_t r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;
    const int o1 = re15_itps_pixel(a, u, v, &r1, &g1, &b1), o2 = re15_itps_pixel(b, u, v, &r2, &g2, &b2);
    return o1 == o2 && r1 == r2 && g1 == g2 && b1 == b2;
}

static void teil_bilder(void)
{
    printf("== bilder ==\n");
    PRUEF(re15_itps_load() == 0 && re15_itps_available(0x1E) && re15_itps_available(0x20), "ITEM/ITPS.ITP geladen");
    int d12 = 0, d23 = 0, block_ok = 1;
    for (int v = 0; v < RE15_ITPS_H; v++)
        for (int u = 0; u < RE15_ITPS_W; u++) {
            d12 += !px_gleich(0x1E, 0x1F, u, v);
            d23 += !px_gleich(0x1F, 0x20, u, v);
        }
    for (int id = 0x1E; id <= 0x20; id++)
        for (int y = 0; y < 30; y++)
            for (int x = 0; x < 40; x++) {
                const int u = 16 + 2 * x, v = 6 + 2 * y;
                if (!px_gleich((uint8_t)id, (uint8_t)id, u, v)) block_ok = 0;
                uint8_t r0, g0, b0, r, g, b; int o0 = re15_itps_pixel((uint8_t)id, u, v, &r0, &g0, &b0);
                for (int k = 1; k < 4; k++) {
                    r = r0; g = g0; b = b0;
                    int o = re15_itps_pixel((uint8_t)id, u + (k & 1), v + (k >> 1), &r, &g, &b);
                    if (o != o0 || r != r0 || g != g0 || b != b0) block_ok = 0;
                }
            }
    PRUEF(d12 > 200 && d23 > 200, "Kapsel-Bilder unterscheiden sich (0x1E/0x1F %d, 0x1F/0x20 %d Bildpunkte; Platzhalter = 0)", d12, d23);
    PRUEF(block_ok, "Kachelfeld x 16..95, y 6..65 aus 2x2-Bloecken (Kachel 40x30 2-fach)");
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "stellen")) teil_stellen();
    else if (!strcmp(t, "python")) teil_python();
    else if (!strcmp(t, "leiche")) teil_leiche();
    else if (!strcmp(t, "mutter")) teil_mutter();
    else if (!strcmp(t, "granaten")) teil_granaten();
    else if (!strcmp(t, "bilder")) teil_bilder();
    else { printf("unbekannter Teil '%s'\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
