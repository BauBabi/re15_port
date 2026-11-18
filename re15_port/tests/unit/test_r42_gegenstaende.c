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
 *   granaten  ZENSUS (Nacharbeit gelb G1, nicht mehr gegen die eigene Erwartungstabelle): (a) alle Item_aot_set (0x50) in ALLEN
 *             Skripten (main + sub) aller ROOM*.RDT beider Fassungen mit Item 0x09..0x0B / 0x1C; (b) jeder Raum beider Fassungen mit
 *             jedem Eintritts-Cut durch die echten Installer gebootet, alle Gegenstands-Zonen mit 0x09..0x0B / 0x1C; (c) jede direkte
 *             Vergabestelle im Port-Quelltext (re15_item_modal_start / re15_inv_grant) aufgeloest; (d) Startausruestung.
 *             Ergebnis je Szenario: hoechstens ZWEI Handgranaten (fertige + je Zuender eine gebaute)
 *   fund      Fundstellen mit Text (gelb G2/G3/G7): Spind 1071, Kisten 3010/3011, Spalt 5021 - Platz -> Text 21 -> Aufnahme-Fenster;
 *             3000/3001 und die Leiche 1071 tragen nichts mehr; genommen -> 1071 Text 22, sonst der Original-Satz
 *   texte     gelb G4: Name 0x1C "Detonator", CHECK 0x14 Python, 0x17 Magnum mit Python (ueber die alten Leser)
 *   ultimate  gelb G5: je Modus EINE Bildquelle - Aufnahme-Bild der Kapseln = Inventar-Kachel des Modus 2-fach (AUS Mod, AN Disc)
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
#include "re15_item_icon.h"
#include "re15_item_prompt.h"
#include "re15_inv_screen.h"
#include "re15_ultimate15.h"
#include "re15_gameflow.h"
#include <dirent.h>

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
typedef struct { uint16_t raum; uint8_t item, menge, bit; int vorrang; uint8_t org_item; int32_t x, z, w, d; int fund; } erwartet_t;
static const erwartet_t k_erw[] = {
    { 0x2010, 0x2F, 1,   41,  3, 0x22, 2500, 4700, 800, 800 },
    { 0x2011, 0x2F, 1,   41,  3, 0x22, 2500, 4700, 800, 800 },
    { 0x11F0, 0x02, 1,   44, 13, 0,    10200, -15950, 1500, 1000 },
    { 0x11F1, 0x02, 1,   44, 13, 0,    10200, -15950, 1500, 1000 },
    { 0x1071, 0x0F, 6,   45,  4, 0,    20616, 10718, 1008, 1500, 1 },   /* Spind-Abschnitt (gelb G2), Text 21/22 */
    { 0x1191, 0x1D, 6,   46, 11, 0x16, 19700, -19350, 1000, 1000 },
    { 0x30E1, 0x1D, 6,   47,  5, 0x17, 25100, 13600, 1000, 1000 },
    { 0x3010, 0x1C, 1,   48,  5, 0x16, -10533, 826, 560, 630, 1 },     /* Kisten im Laster (gelb G3), vor den Patronen */
    { 0x3011, 0x1C, 1,   48,  5, 0x16, -10533, 826, 560, 630, 1 },
    { 0x30E0, 0x1E, 1,   49,  8, 0x24, 26300, 13900, 1000, 1000 },
    { 0x30E1, 0x1E, 6,   49,  4, 0x24, 26300, 13900, 1000, 1000 },
    { 0x40B1, 0x1F, 6,   50,  5, 0x17, -19900, -11600, 1000, 1000 },
    { 0x4050, 0x18, 100, 51, 14, 0,    -6900, -16850, 1000, 1200 },
    { 0x4051, 0x1F, 6,   51, 14, 0,    -6900, -16850, 1000, 1200 },
    { 0x5021, 0x20, 6,   170, 6, 0,    -7600, -18000, 500, 1400, 1 },   /* Spalt unter den Kisten (gelb G7) */
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
        int s = zone_mit_item(e->item, e->bit);
        PRUEF(s >= 0, "ROOM%04X: Zone Item 0x%02X Bit (9,%u) da (Slot %d)", e->raum, e->item, e->bit, s);
        if (s < 0) continue;
        if (e->fund) {   /* Fundstelle mit Text: Gegenstand mit Rechteck 0 (Form 1051 @0x00C22), Platz = Ereignis 26 im Vorrang-Slot */
            PRUEF(g_aot.item_params[s].amount == e->menge && g_aot.slots[s].half_w == 0 && g_aot.slots[s].half_h == 0,
                  "ROOM%04X: Gegenstand x%u mit Rechteck 0 (Slot %d)", e->raum, g_aot.item_params[s].amount, s);
            s = e->vorrang;
            PRUEF(g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[s].event_id == 26,
                  "ROOM%04X: Slot %d = Ereignis-Platz 26 (Typ %d Ereignis %d)", e->raum, s, g_aot.slots[s].type, g_aot.slots[s].event_id);
        }
        const re15_aot_t *a = &g_aot.slots[s];
        if (!e->fund)
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
                /* Fundstelle: der Original-Satz hat sein eigenes (groesseres) Rechteck - gesucht wird er ueber Typ/Kennung */
            }
            if (e->fund && org < 0)
                for (int k = RE15_AOT_MAX - 1; k > s && org < 0; k--)
                    if (g_aot.slots[k].active && (e->org_item ? (g_aot.slots[k].type == RE15_AOT_TYPE_ITEM &&
                                                                 g_aot.item_params[k].item_type == e->org_item)
                                                              : g_aot.slots[k].type == RE15_AOT_TYPE_MESSAGE)) org = k;
            PRUEF(org > s, "ROOM%04X: Original-Satz nach Slot %d (> %d) umgezogen", e->raum, org, s);
        }
        /* genommen -> keine Zone, der Vorrang-Slot traegt wieder den Original-Satz */
        bits_loeschen();
        re15_game_flag_set(9, e->bit, 1);
        room_boot(e->raum, e->x + e->w / 2, 0, e->z + e->d / 2 + 3000, 0, cut);
        PRUEF(zone_mit_item(e->item, e->bit) < 0, "ROOM%04X: Bit (9,%u) gesetzt -> keine Zone", e->raum, e->bit);
        if (e->fund && e->raum == 0x1071)
            PRUEF(g_aot.slots[e->vorrang].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[e->vorrang].event_id == 22,
                  "ROOM1071: Slot %d nach der Aufnahme Text 22 (\"The locker is empty now.\")", e->vorrang);
        else if (e->vorrang >= 0 && !(e->fund && e->raum == 0x5021))   /* 5021 Slot 6 kommt aus sub00 (nur vor der Szene) */
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

/* ---- granaten: ZENSUS (Nacharbeit gelb G1) ---------------------------------------------------------------------------------
 * Nicht gegen eine eigene Erwartungstabelle, sondern gegen das, was das Spiel hergibt:
 *   (a) RDT: jedes Item_aot_set (0x50, Item = u16 @+14) in JEDEM Skript (Zeigertabellen RDT+0x40 main / RDT+0x44 sub) jeder
 *       ROOM*.RDT unter STAGE1..6 (beide Fassungen), Vorschub mit der VM-Tabelle scd_opcode_size_at.
 *   (b) Port-Installer: jeder Raum beider Fassungen mit jedem Eintritts-Cut 0..nCut-1 ueber scd_room_reenter (alle Installer)
 *       gebootet, Bits 0; jede Gegenstands-Zone mit 0x09..0x0B / 0x1C.
 *   (c) direkte Vergaben im Quelltext (engine/src, platform/pc, platform/pc/src): jeder Aufruf re15_item_modal_start( /
 *       re15_inv_grant( ; erstes Argument aufgeloest (Zahl, #define, oder eine gepruefte Tabelle). Unbekannter Ausdruck = FEHLER.
 *   (d) Startausruestung re15_inv_load_briefing fuer Leon und Elza.
 * Ergebnis je Szenario: fertige Handgranaten + Zuender (jeder Zuender = hoechstens eine gebaute Granate, Kombi @0x80074D10,
 * Ergebnis = kleinere Menge @0x8004e680) <= 2. NUTZER 16.11.: "Handgranaten sollte es nicht zu viele geben". */
void re15_gegenstaende_r42_bild_einsetzen(uint8_t *itps, int size);   /* seit Stufe 1 (537349109) */
static int ist_granate(unsigned id) { return id >= 0x09 && id <= 0x0B; }
static int ist_zaehlbar(unsigned id) { return ist_granate(id) || id == 0x1C; }
static unsigned le16(const uint8_t *p) { return (unsigned)p[0] | ((unsigned)p[1] << 8); }
static unsigned le32(const uint8_t *p) { return (unsigned)p[0] | ((unsigned)p[1] << 8) | ((unsigned)p[2] << 16) | ((unsigned)p[3] << 24); }

typedef struct { uint16_t raum; unsigned item, menge; int quelle; } fund_eintrag_t;   /* quelle 0 RDT, 1 Zone, 2 Quelltext */
static fund_eintrag_t s_funde[64];
static int s_nfunde = 0;
static void fund_merken(uint16_t raum, unsigned item, unsigned menge, int quelle)
{
    for (int i = 0; i < s_nfunde; i++)
        if (s_funde[i].raum == raum && s_funde[i].item == item && s_funde[i].quelle == quelle) return;
    if (s_nfunde < 64) { s_funde[s_nfunde].raum = raum; s_funde[s_nfunde].item = item; s_funde[s_nfunde].menge = menge;
                         s_funde[s_nfunde++].quelle = quelle; }
}

static int rdt_zensus(const uint8_t *d, size_t n, uint16_t raum, int *saetze)
{
    int treffer = 0;
    unsigned starts[128]; int ns = 0;
    for (int tab = 0; tab < 2; tab++) {
        const unsigned base = le32(d + 0x40 + 4 * tab);
        if (base + 2 > n) continue;
        const unsigned cnt = le16(d + base) / 2;
        for (unsigned i = 0; i < cnt && ns < 128 && base + 2 * i + 2 <= n; i++) starts[ns++] = base + le16(d + base + 2 * i);
    }
    for (int k = 0; k < ns; k++) {
        unsigned ende = (unsigned)n;
        for (int j = 0; j < ns; j++) if (starts[j] > starts[k] && starts[j] < ende) ende = starts[j];
        for (unsigned h = 0x08; h < 0x60; h += 4) { const unsigned v = le32(d + h); if (v > starts[k] && v < ende) ende = v; }
        unsigned pc = starts[k];
        while (pc < ende) {
            const int sz = scd_opcode_size_at(d + pc);
            if (sz <= 0 || pc + (unsigned)sz > ende) break;
            if (d[pc] == 0x50) {
                (*saetze)++;
                const unsigned item = le16(d + pc + 14);
                if (ist_zaehlbar(item)) { treffer++; fund_merken(raum, item, le16(d + pc + 16), 0);
                                          printf("  RDT ROOM%04X @0x%05X Item_aot_set Item 0x%02X x%u\n", raum, pc, item, le16(d + pc + 16)); }
            }
            pc += (unsigned)sz;
        }
    }
    return treffer;
}

/* (c) Quelltext */
static char *datei_text(const char *p)
{
    size_t n = 0; uint8_t *b = slurp(p, &n);
    if (!b) return NULL;
    char *t = (char *)realloc(b, n + 1);
    if (t) t[n] = 0;
    return t;
}
static char s_src[600];
static long define_wert(const char *name)
{
    static const char *const dirs[] = { "include", "engine/src", "platform/pc", "platform/pc/src", NULL };
    char pfad[900];
    for (int i = 0; dirs[i]; i++) {
        snprintf(pfad, sizeof pfad, "%s/%s", s_src, dirs[i]);
        DIR *dp = opendir(pfad); struct dirent *e;
        if (!dp) continue;
        while ((e = readdir(dp))) {
            const size_t l = strlen(e->d_name);
            if (l < 3 || (strcmp(e->d_name + l - 2, ".h") && strcmp(e->d_name + l - 2, ".c"))) continue;
            char f[1200]; snprintf(f, sizeof f, "%s/%s", pfad, e->d_name);
            char *t = datei_text(f);
            if (!t) continue;
            char muster[200]; snprintf(muster, sizeof muster, "#define %s ", name);
            const char *q = strstr(t, muster);
            if (!q) { snprintf(muster, sizeof muster, "#define %s\t", name); q = strstr(t, muster); }
            if (q) { long v = strtol(q + strlen(muster), NULL, 0); free(t); closedir(dp); return v; }
            free(t);
        }
        closedir(dp);
    }
    return -1;
}
/* gepruefte Tabellen-Ausdruecke: Datei, Tabelle, Feldnummer (0-basiert) der Item-Spalte */
static const struct { const char *datei, *ausdruck, *tabelle; int feld; } k_tabellen[] = {
    { "keyitems_r41.c", "k_aufnahme[i].item", "k_aufnahme[] = {", 2 },
    { "keyitems_r41.c", "f->item",            "k_flagitems[] = {", 0 },
};
static int tabelle_pruefen(const char *datei_pfad, const char *kopf, int feld, uint16_t *granaten, uint16_t *zuender)
{
    char *t = datei_text(datei_pfad);
    if (!t) return -1;
    const char *q = strstr(t, kopf);
    if (!q) { free(t); return -1; }
    q += strlen(kopf);
    const char *ende = strstr(q, "};");
    int zeilen = 0;
    while (q && q < ende) {
        const char *z = strchr(q, '{');
        if (!z || z > ende) break;
        const char *zu = strchr(z, '}');
        char feldtext[64] = { 0 }; const char *f = z + 1;
        for (int k = 0; k < feld; k++) { f = strchr(f, ','); if (!f || f > zu) break; f++; }
        if (f && f < zu) {
            while (*f == ' ') f++;
            int l = 0; while (f[l] && f[l] != ',' && f[l] != '}' && f[l] != ' ' && l < 63) { feldtext[l] = f[l]; l++; }
            long v = (feldtext[0] >= '0' && feldtext[0] <= '9') ? strtol(feldtext, NULL, 0) : define_wert(feldtext);
            if (v < 0) { free(t); return -1; }
            if (ist_granate((unsigned)v)) (*granaten)++;
            if (v == 0x1C) (*zuender)++;
            zeilen++;
        }
        q = zu + 1;
    }
    free(t);
    return zeilen;
}
static void quelltext_zensus(int *unbekannt, int *direkt_granaten, int *direkt_zuender, int *stellen)
{
    static const char *const dirs[] = { "engine/src", "platform/pc", "platform/pc/src", NULL };
    static const char *const rufe[] = { "re15_item_modal_start(", "re15_inv_grant(", "re15_inv_grant_stapeln_nutzer(", NULL };
    char pfad[900];
    for (int i = 0; dirs[i]; i++) {
        snprintf(pfad, sizeof pfad, "%s/%s", s_src, dirs[i]);
        DIR *dp = opendir(pfad); struct dirent *e;
        if (!dp) { printf("  FEHLER: Quelltext %s nicht lesbar\n", pfad); g_fail++; continue; }
        while ((e = readdir(dp))) {
            const size_t l = strlen(e->d_name);
            if (l < 3 || strcmp(e->d_name + l - 2, ".c")) continue;
            char f[1200]; snprintf(f, sizeof f, "%s/%s", pfad, e->d_name);
            char *t = datei_text(f);
            if (!t) continue;
            for (int r = 0; rufe[r]; r++) {
                for (const char *q = strstr(t, rufe[r]); q; q = strstr(q + 1, rufe[r])) {
                    const char *zeile = q; while (zeile > t && zeile[-1] != '\n') zeile--;
                    const char *zende = strchr(q, '\n');
                    char z[600] = { 0 }; size_t zl = (size_t)((zende ? zende : q + strlen(q)) - zeile); if (zl > 599) zl = 599;
                    memcpy(z, zeile, zl);
                    if (strstr(z, "uint8_t type") || strstr(z, "uint8_t item_type") || strstr(z, "void re15_item_modal_start") ||
                        strstr(z, "int re15_inv_grant_stapeln_nutzer(uint8_t") ||
                        strstr(z, "int re15_inv_grant") || strstr(z, "int  re15_inv_grant")) continue;   /* Definition/Prototyp */
                    { const char *c1 = strstr(z, "/*"), *c2 = strstr(z, "//"), *rp = strstr(z, rufe[r]);
                      if ((c1 && c1 < rp) || (c2 && c2 < rp) || z[strspn(z, " ")] == '*') continue; }    /* Kommentar */
                    const char *a = q + strlen(rufe[r]);
                    char arg[96] = { 0 }; int al = 0;
                    while (a[al] && a[al] != ',' && a[al] != ')' && al < 95) { arg[al] = a[al]; al++; }
                    (*stellen)++;
                    long v = -1; int geprueft = 0;
                    if (arg[0] >= '0' && arg[0] <= '9') v = strtol(arg, NULL, 0);
                    else if ((!strcmp(arg, "type") || !strcmp(arg, "item_id")) && !strcmp(e->d_name, "inventory_common.c"))
                        geprueft = 1;   /* Durchreichung im Stapel-Weg; Quelle = die Aufrufer unten */
                    else if (!strcmp(arg, "s_type") && !strcmp(e->d_name, "item_modal_common.c"))
                        geprueft = 1;   /* das Aufnahme-Fenster gibt aus, was re15_item_modal_start bekam */
                    else if (!strcmp(arg, "p->item_type") && !strcmp(e->d_name, "aot_common.c")) geprueft = 1;   /* Zonen: (a)+(b) */
                    else {
                        for (unsigned k = 0; k < sizeof k_tabellen / sizeof k_tabellen[0]; k++)
                            if (!strcmp(e->d_name, k_tabellen[k].datei) && !strcmp(arg, k_tabellen[k].ausdruck)) {
                                uint16_t g = 0, zz = 0;
                                const int n = tabelle_pruefen(f, k_tabellen[k].tabelle, k_tabellen[k].feld, &g, &zz);
                                PRUEF(n > 0, "Quelltext %s: Tabelle %s aufgeloest (%d Zeilen, Granaten %u, Zuender %u)",
                                      e->d_name, k_tabellen[k].tabelle, n, g, zz);
                                *direkt_granaten += g; *direkt_zuender += zz; geprueft = 1;
                            }
                        if (!geprueft) {
                            int ok = 1; for (int c = 0; arg[c]; c++) if (!(arg[c] == '_' || (arg[c] >= 'A' && arg[c] <= 'Z') ||
                                                                         (arg[c] >= '0' && arg[c] <= '9'))) ok = 0;
                            if (ok) v = define_wert(arg);
                        }
                    }
                    if (!geprueft && v < 0) {
                        (*unbekannt)++;
                        printf("  FEHLER: unbekannte Vergabestelle %s: %s(%s ...)\n", e->d_name, rufe[r], arg);
                        g_fail++;
                    } else if (!geprueft) {
                        if (ist_granate((unsigned)v)) { (*direkt_granaten)++; fund_merken(0, (unsigned)v, 1, 2); }
                        if (v == 0x1C) { (*direkt_zuender)++; fund_merken(0, 0x1C, 1, 2); }
                        if (ist_zaehlbar((unsigned)v)) printf("  Quelltext %s: %s%s) -> Item 0x%02lX\n", e->d_name, rufe[r], arg, v);
                    }
                }
            }
            free(t);
        }
        closedir(dp);
    }
}

static void teil_granaten(void)
{
    printf("== granaten (Zensus) ==\n");
    snprintf(s_src, sizeof s_src, "%s", RE15_ASSET_PSX_DIR);
    { char *k = strstr(s_src, "/shared_assets/PSX"); if (k) *k = 0; }
    /* (a)+(b) */
    int saetze = 0, rdt_treffer = 0, raeume = 0, boots = 0;
    unsigned zone_granaten[2] = { 0, 0 }, zone_zuender[2] = { 0, 0 };
    for (int st = 1; st <= 6; st++) {
        char pfad[700]; snprintf(pfad, sizeof pfad, "%s/STAGE%d", RE15_ASSET_PSX_DIR, st);
        DIR *dp = opendir(pfad); struct dirent *e;
        if (!dp) { printf("  FEHLER: %s fehlt\n", pfad); g_fail++; continue; }
        while ((e = readdir(dp))) {
            unsigned raum = 0;
            if (strlen(e->d_name) != 12 || strncmp(e->d_name, "ROOM", 4) || strcmp(e->d_name + 8, ".RDT") ||
                sscanf(e->d_name + 4, "%4x", &raum) != 1) continue;
            raeume++;
            char f[900]; snprintf(f, sizeof f, "%s/%s", pfad, e->d_name);
            size_t n = 0; uint8_t *d = slurp(f, &n);
            if (!d) continue;
            rdt_treffer += rdt_zensus(d, n, (uint16_t)raum, &saetze);
            free(d);
            if (rdt_laden((uint16_t)raum) != 0) continue;
            const int ncut = s_rdt.cut_count > 0 ? (s_rdt.cut_count < 16 ? s_rdt.cut_count : 16) : 1;
            for (int c = 0; c < ncut; c++) {
                bits_loeschen();
                if (room_boot((uint16_t)raum, 0, 0, 0, 0, (uint8_t)c) != 0) break;
                boots++;
                for (int sl = 0; sl < RE15_AOT_MAX; sl++) {
                    const re15_aot_t *a = &g_aot.slots[sl];
                    if (!a->active || a->type != RE15_AOT_TYPE_ITEM || !ist_zaehlbar(g_aot.item_params[sl].item_type)) continue;
                    int neu = 1;
                    for (int i = 0; i < s_nfunde; i++)
                        if (s_funde[i].raum == raum && s_funde[i].item == g_aot.item_params[sl].item_type && s_funde[i].quelle == 1) neu = 0;
                    if (!neu) continue;
                    fund_merken((uint16_t)raum, g_aot.item_params[sl].item_type, g_aot.item_params[sl].amount, 1);
                    printf("  Zone ROOM%04X (Eintritt Cut %d) Slot %d: Item 0x%02X x%u Bit (9,%u)\n", raum, c, sl,
                           g_aot.item_params[sl].item_type, g_aot.item_params[sl].amount, g_aot.item_params[sl].taken_bit);
                    if (ist_granate(g_aot.item_params[sl].item_type)) zone_granaten[raum & 1u] += g_aot.item_params[sl].amount;
                    else zone_zuender[raum & 1u] += g_aot.item_params[sl].amount;
                }
            }
        }
        closedir(dp);
    }
    printf("  %d RDT, %d Item_aot_set gelesen, %d Raum-Starts\n", raeume, saetze, boots);
    PRUEF(raeume == 240 && saetze == 164, "(a) Walker vollstaendig: 240 RDT, 164 Item_aot_set (Zensus bank9_zensus.py original)");
    PRUEF(rdt_treffer == 0, "(a) RDT: kein Item_aot_set mit Handgranate 0x09..0x0B oder Zuender 0x1C (%d)", rdt_treffer);
    /* (c) */
    int unbekannt = 0, d_gran = 0, d_zuend = 0, stellen = 0;
    quelltext_zensus(&unbekannt, &d_gran, &d_zuend, &stellen);
    printf("  (c) %d direkte Vergabestellen, unbekannt %d, Handgranaten %d, Zuender %d\n", stellen, unbekannt, d_gran, d_zuend);
    PRUEF(stellen >= 8 && unbekannt == 0, "(c) alle %d Vergabestellen aufgeloest", stellen);
    const long g_menge = define_wert("RE15_GRANATE_MENGE");
    PRUEF(d_gran == 1 && d_zuend == 0 && g_menge == 1,
          "(c) direkt: genau EINE Handgranate (granate_1150.c, RE15_GRANATE_MENGE %ld), kein Zuender", g_menge);
    /* (d) */
    int start = 0;
    for (int figur = 0; figur < 2; figur++) {
        g_gameflow.character = (uint8_t)(figur ? 4 : 0);
        re15_inv_load_briefing();
        for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (ist_zaehlbar(g_inv.slots[i].id)) start++;
    }
    PRUEF(start == 0, "(d) Startausruestung ohne Handgranate/Zuender (%d)", start);
    /* Summe je Szenario */
    for (int figur = 0; figur < 2; figur++) {
        const unsigned summe = zone_granaten[figur] + (unsigned)d_gran * (unsigned)g_menge + zone_zuender[figur] + (unsigned)d_zuend;
        PRUEF(summe <= 2, "%s: Handgranaten hoechstens %u (fertig: Zonen %u + direkt %d; Zuender: Zonen %u + direkt %d) <= 2",
              figur ? "Elza" : "Leon", summe, zone_granaten[figur], d_gran, zone_zuender[figur], d_zuend);
        PRUEF(zone_zuender[figur] == 1 && zone_granaten[figur] == 0, "%s: genau ein Zuender in der Welt, keine Granaten-Zone",
              figur ? "Elza" : "Leon");
    }
    int z3010 = 0, z3000 = 0;
    for (int i = 0; i < s_nfunde; i++) if (s_funde[i].item == 0x1C && s_funde[i].quelle == 1) {
        if ((s_funde[i].raum & 0xFFFE) == 0x3010) z3010++;
        if ((s_funde[i].raum & 0xFFFE) == 0x3000) z3000++;
    }
    PRUEF(z3010 == 2 && z3000 == 0, "Zuender liegt in 3010/3011 (gelb G3), nicht mehr in 3000/3001 (%d / %d)", z3010, z3000);
}

/* ---- fund: Fundstellen mit Text (gelb G2/G3/G7) ------------------------------------------------------------------------------ */
/* Aufnahme-Fenster: bis es mit diesem Gegenstand offen ist (Text vorher mit SQUARE schliessen), dann antworten. Das Fenster tickt
 * im Spiel der Plattform-Takt; hier von Hand wie probe_r17_munition.c (virtuell 0x4000 = Bestaetigen, 0x1000 = Ja/Nein umschalten). */
static int fenster_bis(uint8_t item)
{
    for (int fr = 0; fr < 300; fr++) {
        if (re15_item_modal_active()) {
            int qx[4], qy[4], face = 0; uint8_t t = 0;
            re15_item_modal_tick(0, 0);
            if (re15_item_modal_quad(qx, qy, &t, &face) && t == item) return 1;
        }
        const uint16_t kk = (fr % 6) == 0 ? RE15_PAD_BIT_SQUARE : 0;
        frame(kk, kk);
    }
    return 0;
}
static void fenster_antwort(int nein)
{
    int w = 0;
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800) re15_item_modal_tick(0, 0);
    if (nein) re15_item_modal_tick(0x1000u, 0);
    w = 0;
    while (re15_item_modal_active() && w++ < 800) re15_item_modal_tick(0x4000u, 0);
}

static void teil_fund(void)
{
    printf("== fund ==\n");
    static const struct { uint16_t raum; uint8_t item; uint8_t bit; int slot; int32_t x, z; const char *wort; int leer; } k[] = {
        { 0x1071, 0x0F, 45,  4, 20616 + 504, 10718 + 750, "grenade launcher", 1 },
        { 0x3010, 0x1C, 48,  5, -10533 + 280, 826 + 315, "detonator", 0 },
        { 0x3011, 0x1C, 48,  5, -10533 + 280, 826 + 315, "detonator", 0 },
        { 0x5021, 0x20, 170, 6, -7600 + 250, -18000 + 700, "capsule", 0 },
    };
    for (unsigned i = 0; i < sizeof k / sizeof k[0]; i++) {
        bits_loeschen();
        re15_inv_init();
        if (room_boot(k[i].raum, k[i].x, 0, k[i].z + 1500, 2048, 0) != 0) continue;
        const re15_aot_t *a = &g_aot.slots[k[i].slot];
        PRUEF(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == 26, "ROOM%04X Slot %d: Platz Ereignis 26", k[i].raum, k[i].slot);
        PRUEF(scd_event_fire(26) >= 0, "ROOM%04X: Ereignis 26 startet", k[i].raum);
        const int f = bis_nachricht(21, 120);
        const char *txt = re15_msg_get_text(21);
        PRUEF(f >= 0 && txt && strstr(txt, k[i].wort), "ROOM%04X: Fundtext 21 erscheint (\"%s\")", k[i].raum, txt ? txt : "-");
        PRUEF(fenster_bis(k[i].item), "ROOM%04X: danach das Aufnahme-Fenster mit Item 0x%02X", k[i].raum, k[i].item);
        /* "Nein" (FUN_8001db28 Zustand 7 @0x8001e068: kein Bit, kein Gegenstand) -> beim naechsten Druck wieder Text + Fenster */
        fenster_antwort(1);
        PRUEF(!re15_game_flag_get(9, k[i].bit) && re15_inv_find_item(k[i].item) < 0 && !re15_item_modal_active(),
              "ROOM%04X: Nein -> nichts genommen", k[i].raum);
        for (int fr = 0; fr < 30; fr++) frame(0, 0);
        PRUEF(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == 26 && scd_event_fire(26) >= 0 &&
              bis_nachricht(21, 120) >= 0 && fenster_bis(k[i].item), "ROOM%04X: nach Nein wieder Text 21 und Fenster", k[i].raum);
        /* "Ja" (Bit setzt das Fenster selbst, @0x8001e0d0) -> der Takt baut den Platz um */
        fenster_antwort(0);
        PRUEF(re15_game_flag_get(9, k[i].bit) && re15_inv_find_item(k[i].item) >= 0 && !re15_item_modal_active(),
              "ROOM%04X: Ja -> aufgenommen (Bit (9,%u), Item im Inventar)", k[i].raum, k[i].bit);
        for (int fr = 0; fr < 4; fr++) frame(0, 0);
        if (k[i].leer)
            PRUEF(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == 22,
                  "ROOM%04X: nach der Aufnahme Text 22 (\"%s\")", k[i].raum, re15_msg_get_text(22) ? re15_msg_get_text(22) : "-");
        else
            PRUEF(!(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == 26), "ROOM%04X: nach der Aufnahme kein Platz 26", k[i].raum);
        PRUEF(scd_event_fire(26) < 0 || bis_nachricht(21, 60) < 0, "ROOM%04X: Ereignis 26 zeigt den Fundtext nicht mehr", k[i].raum);
    }
    /* alte Stellen leer */
    for (int v = 0; v < 2; v++) {
        bits_loeschen();
        room_boot((uint16_t)(0x3000 + v), -16300, 0, -14000, 0, 0);
        PRUEF(zone_mit_item(0x1C, 48) < 0, "ROOM%04X: kein Zuender mehr an der Stelle des Liegenden em4 @0x0151A", 0x3000 + v);
    }
    bits_loeschen();
    room_boot(0x1071, 844, 0, 16000, 2048, 0);
    {
        int werfer_alt = 0;
        for (int sl = 0; sl < RE15_AOT_MAX; sl++) {
            const re15_aot_t *a = &g_aot.slots[sl];
            if (a->active && a->type == RE15_AOT_TYPE_ITEM && g_aot.item_params[sl].item_type == 0x0F && a->half_w > 0 &&
                a->x > 0 && a->x < 2000 && a->z > 13000 && a->z < 15000) werfer_alt++;
        }
        PRUEF(werfer_alt == 0, "ROOM1071: kein Granatwerfer mehr beim Liegenden em4 @0x0161A (844,14124)");
    }
}

/* ---- texte (gelb G4) ----------------------------------------------------------------------------------------------------------- */
static void glyphen_text(const uint8_t *p, int n, char *out, int max)
{
    int o = 0;
    for (int i = 0; i < n && o < max - 4; i++) {
        const uint8_t b = p[i];
        if (b == 0x01 || b == 0x07) break;
        if (b == 0x02 && i + 1 < n && p[i + 1] == 0x00) { out[o++] = ' '; out[o++] = '|'; out[o++] = ' '; i++; continue; }
        char c = '?';
        if (b >= 0x1D && b < 0x1D + 26) c = (char)('A' + b - 0x1D);
        else if (b >= 0x3D && b < 0x3D + 26) c = (char)('a' + b - 0x3D);
        else if (b >= 0x0C && b < 0x0C + 10) c = (char)('0' + b - 0x0C);
        else switch (b) { case 0x00: c = ' '; break; case 0x1A: c = '!'; break; case 0x1B: c = '?'; break; case 0x16: c = ':'; break;
                          case 0x18: c = ','; break; case 0x3A: c = '\''; break; case 0x3B: c = '-'; break; case 0x57: c = '.'; break;
                          case 0x19: case 0x61: c = '"'; break; case 0x08: c = '/'; break; default: break; }
        out[o++] = c;
    }
    out[o] = 0;
}
static void teil_texte(void)
{
    printf("== texte ==\n");
    uint8_t nm[64]; char t[256];
    const int n = re15_item_prompt_name(0x1C, nm, (int)sizeof nm);
    glyphen_text(nm, n, t, (int)sizeof t);
    PRUEF(!strcmp(t, "Detonator"), "Name 0x1C = \"%s\" (soll \"Detonator\", GEGENSTAENDE.md V-06)", t);
    const uint8_t *d = re15_inv_desc_entry(0x14);
    glyphen_text(d, 200, t, (int)sizeof t);
    PRUEF(strstr(t, "Colt's Manufacturing") && strstr(t, "A .357 magnum revolver.") && !strstr(t, "Barry"),
          "CHECK 0x14 Python: \"%s\"", t);
    d = re15_inv_desc_entry(0x17);
    glyphen_text(d, 200, t, (int)sizeof t);
    PRUEF(!strcmp(t, "DOT50A.E. rounds. Magnum/rounds for the Super/Redhawk or the Colt Python."),
          "CHECK 0x17 Magnum: \"%s\" (Zeilen 1/2 wie Bank-Eintrag 0x17)", t);
}

/* ---- ultimate (gelb G5) --------------------------------------------------------------------------------------------------------- */
static int bild_wie_kachel(uint8_t id, const uint8_t *kachel, const uint16_t *clut)
{
    int abw = 0;
    for (int y = 0; y < 30; y++)
        for (int x = 0; x < 40; x++) {
            const uint16_t c = clut[kachel[y * 40 + x]];
            uint8_t r = 0, g = 0, b = 0;
            const int da = re15_itps_pixel(id, 16 + 2 * x, 6 + 2 * y, &r, &g, &b);
            if (da != (c != 0)) { abw++; continue; }
            if (da && (r != (uint8_t)((c & 0x1f) << 3) || g != (uint8_t)(((c >> 5) & 0x1f) << 3) || b != (uint8_t)(((c >> 10) & 0x1f) << 3))) abw++;
        }
    return abw;
}
static void teil_ultimate(void)
{
    printf("== ultimate ==\n");
    char p[700];
    size_t tn = 0, mn = 0, dn = 0, in = 0, idn = 0;
    snprintf(p, sizeof p, "%s/DATA/ST_00.TIM", RE15_ASSET_PSX_DIR);        uint8_t *tim = slurp(p, &tn);
    snprintf(p, sizeof p, "%s/DATA/ITEMALL.PIX", RE15_ASSET_PSX_DIR);      uint8_t *mod = slurp(p, &mn);
    snprintf(p, sizeof p, "%s/DISC40/DATA/ITEMALL.PIX", RE15_ASSET_PSX_DIR); uint8_t *disc = slurp(p, &dn);
    snprintf(p, sizeof p, "%s/ITEM/ITPS.ITP", RE15_ASSET_PSX_DIR);         uint8_t *itps = slurp(p, &in);
    snprintf(p, sizeof p, "%s/DISC40/ITEM/ITPS.ITP", RE15_ASSET_PSX_DIR);  uint8_t *ditps = slurp(p, &idn);
    PRUEF(tim && mod && disc && itps && ditps && tn >= 20 + 512, "Dateien da (ST_00.TIM, ITEMALL/ITPS Mod und Disc)");
    if (!(tim && mod && disc && itps && ditps)) return;
    uint16_t clut[256];
    for (int i = 0; i < 256; i++) clut[i] = (uint16_t)(tim[20 + 2 * i] | (tim[21 + 2 * i] << 8));   /* ST_00.TIM CLUT-Zeile 0 */
    re15_gegenstaende_r42_bild_einsetzen(itps, (int)in);   /* wie alle drei Ladestellen */
    uint8_t *u_icon = (uint8_t *)malloc(mn), *u_itps = (uint8_t *)malloc(in);
    PRUEF(re15_ultimate15_itemall_bauen(u_icon, mod, (int)mn, disc, (int)dn) > 0, "ULTIMATE ITEMALL gebaut");
    PRUEF(re15_ultimate15_itps_bauen(u_itps, itps, (int)in, ditps, (int)idn) > 0, "ULTIMATE ITPS gebaut (nach ITEMALL, wie die Ladestellen)");
    re15_itemall_set_pix(mod, (int)mn);
    re15_itemall_set_pix_u15(u_icon, (int)mn);
    re15_itps_set_data(itps, (int)in);
    re15_itps_set_data_u15(u_itps, (int)in);
    for (int an = 0; an < 2; an++) {
        re15_ultimate15_set(an);
        for (uint8_t id = 0x1E; id <= 0x20; id++) {
            const uint8_t *kachel = re15_itemall_tile_raw(id);
            const int abw = kachel ? bild_wie_kachel(id, kachel, clut) : -1;
            PRUEF(abw == 0, "%s: Aufnahme-Bild 0x%02X = Inventar-Kachel des Modus 2-fach (%d Abweichungen)", an ? "AN" : "AUS", id, abw);
        }
    }
    int versch = 0;
    for (int i = 0; i < 1200; i++) versch += mod[0x1E * 1200 + i] != disc[0x1E * 1200 + i];
    PRUEF(versch > 0, "Mod- und Disc-Kachel 0x1E sind verschieden (%d Bytes), die Pruefung unterscheidet die Modi", versch);
    re15_ultimate15_set(0);
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
    else if (!strcmp(t, "fund")) teil_fund();
    else if (!strcmp(t, "texte")) teil_texte();
    else if (!strcmp(t, "ultimate")) teil_ultimate();
    else { printf("unbekannter Teil '%s'\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
