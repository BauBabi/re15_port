/*
 * Runde 41 Spur O "keyitems": Key Items ueber das Inventar in RE2-Form, Kurbel ohne Fliegen.
 * Dossier analysis/befunde_runde41/O_keyitems.md; Modul engine/src/keyitems_r41.c (re15_keyitems_r41.h).
 * Muster: test_r41_kleinkram_b_11b1.c (Raumstart + Spielbild).
 *
 * Teile (Aufruf: test_r41_keyitems <teil>):
 *   stellen  jede Zeile der Stellen-Tabelle gegen die ausgelieferte RDT: Aot_set/Slot/Ereignis, Message_on der Frage, Ck(12,31,0).
 *   use      je Stelle (Leon + Elza) mit dem echten Raumaufbau: USE vor dem Spieler -> Ereignis ohne Text und Frage mit Ja, Wirkung
 *            des Ja-Zweigs tritt ein; USE am falschen Ort oder mit dem falschen Gegenstand -> 0.
 *   aktion   je Stelle: Aktionstaste zeigt die Beschreibung, die Frage kommt nicht, Antwort Nein, keine Wirkung; Karten-Leser: Text
 *            endet am Seitenumbruch vor "Will you operate".
 *   besitz   Flag-Gegenstaende: Eintrag, Verbrauch, Kiste, Altstand, fremde Sicherung bleibt.
 *   voll     volles Inventar an den drei Aufnahme-Stellen: "You can't carry", kein Flag.
 *   kurbel   Mechanik des Fliegens: Pos_set und Plc_motion im ersten Skripttakt, keine Blende (Flag 0x40, anim_frac 0), Clip 0;
 *            Gegenprobe Aktionstaste + alte Frage: Blende 7 am Anfang und am Ende (Plc_ret).
 *   waffe    RE2 @0x80068cfc: Kurbel legt die Waffe ab, die naechste Oeffnung legt sie an.
 *   daten    Name, CHECK-Text, Icon, Bild, Rechtecke Block 0x35.
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
#include "re15_map_hint.h"     /* re15_host_clock_set_us */
#include "re15_cut10f0.h"      /* re15_cine_active */
#include "re15_inventory.h"
#include "re15_itembox.h"
#include "re15_item_modal.h"
#include "re15_damage.h"
#include "re15_keyitems_r41.h"
#include "re15_sicherung.h"
#include "re15_ventilkette_r41.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
void re15_actor_step_all_walkers(void);
extern int scd_event_fire(uint8_t event_id);

static int g_fail = 0, g_n = 0;
#define PRUEF(bed, ...) do { g_n++; int _ok = (bed) ? 1 : 0; if (!_ok) g_fail++; \
    printf("  %s: ", _ok ? "PASS" : "FAIL"); printf(__VA_ARGS__); printf("\n"); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *n = (size_t)sz; return b;
}

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_bild = 0, s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;
static uint64_t           s_uhr = 1000000;

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

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (Muster test_r39_johnruf1041.c bild()). */
static void bild(uint16_t pressed, uint16_t held)
{
    s_uhr += 16667; re15_host_clock_set_us(s_uhr);
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = pressed;
    if (re15_menu_gameplay_frozen()) { re15_game_step(&s_ctx); re15_fade_tick(); s_bild++; return; }
    const unsigned char *raw; int len, id;
    if (!re15_item_modal_active()) scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut = s_shown;
    re15_game_step(&s_ctx);
    if (re15_item_modal_active()) re15_item_modal_tick(pressed, held);
    re15_fade_tick();
    s_bild++;
}

/* Raumstart wie der Spielweg (scd_room_setup ueber scd_room_reenter): Flags VOR dem Raumskript setzen. */
static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, int cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_inventory_t inv_vorher = g_inv;     /* das Raumskript liest den Besitz (Kistenregel), der Port-Start legt ihn vorher an */
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    g_inv = inv_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_bild = 0; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* Flags und Wirkung je Stelle (Belege Dossier-Tabelle). wirkung: Bank/Bit/Wert, die der Ja-Zweig setzt; bank 0xFF = msg statt Flag. */
typedef struct { uint8_t b, bit; } fl_t;
typedef struct {
    uint16_t raum; uint8_t item;
    fl_t     flags[3];              /* {0,0} = Ende */
    uint8_t  w_bank, w_bit;         /* Wirkung des Ja-Zweigs                     */
    uint8_t  w_msg;                 /* w_bank 0xFF: diese Nachricht steht offen  */
    uint8_t  kein_inv;              /* Item nicht ins Inventar legen (kommt aus dem Flag) */
} lauf_t;

static const lauf_t k_laeufe[] = {
    { 0x1090, 0x31, {{3,133}},        3, 129, 0, 0 },     /* sub06 @0x0271E Set(3,129)                     */
    { 0x10D0, 0x38, {{9,52}},      0xFF,   0, 9, 0 },     /* sub20 @0x019C0 Ck(9,52,1) -> @0x019C4 Message_on 9 */
    { 0x11E0, 0x39, {{9,138}},     0xFF,   0, 9, 0 },     /* sub20 @0x01FA8 Ck(9,138,1) -> @0x01FAC        */
    { 0x1230, 0x37, {{9,136}},     0xFF,   0, 9, 0 },     /* sub20 @0x01488 Ck(9,136,1) -> @0x0148C        */
    { 0x11E0, 0x30, {{3,127}},        5,  21, 0, 0 },     /* sub26 @0x020CC Set(5,21)                      */
    { 0x1100, 0x44, {{3,0x67}},       5,   0, 0, 0 },     /* sub07 @0x00D66 Set(5,0)                       */
    { 0x1050, 0x40, {{0,0}},          9,  63, 0, 0 },     /* k_mit +2A Set(9,63)                           */
    { 0x2010, 0x40, {{3,108}},        9, RE15_R41C_BIT_KASTEN, 0, 1 },
    { 0x2020, RE15_R41O_KURBEL_ITEM, {{3,114}}, 3, 113, 0, 1 },   /* sub02 @0x00A7A Set(3,113)             */
    { 0x2040, RE15_R41O_BOMBE_ITEM,  {{3,146}}, 5,   0, 0, 1 },   /* sub07 @0x016FE Set(5,0)               */
    { 0x2060, 0x40, {{3,108}},        5,  10, 0, 1 },     /* sub18 @0x0168E Set(5,10)                      */
    { 0x3050, 0x46, {{3,178}},        5,   0, 0, 0 },     /* sub10 @0x02598 Set(5,0)                       */
    { 0x4000, 0x47, {{3,101}},        3,  32, 0, 0 },     /* sub02 @0x01450 Set(3,32)                      */
    { 0x4030, 0x41, {{3,87}},         5,   1, 0, 0 },     /* sub02 @0x00648 Set(5,1)                       */
};
#define N_LAEUFE ((int)(sizeof k_laeufe / sizeof k_laeufe[0]))

static const lauf_t *lauf_zu(uint16_t raum, uint8_t item)
{
    for (int i = 0; i < N_LAEUFE; i++)
        if (k_laeufe[i].raum == raum && k_laeufe[i].item == item) return &k_laeufe[i];
    return NULL;
}

static void flags_leer(void)
{
    memset(&g_game, 0, sizeof g_game);
    re15_inv_init();
    memset(&g_itembox, 0, sizeof g_itembox);
    g_inv.slots[0].id = 0x03; g_inv.slots[0].qty = 15;
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = 50;
    re15_player_set_equipped_weapon(3);
}

static void flags_setzen(const lauf_t *l)
{
    for (int i = 0; i < 3 && (l->flags[i].b || l->flags[i].bit); i++)
        re15_game_flag_set(l->flags[i].b, l->flags[i].bit, 1);
}

/* Spieler so stellen, dass der Vorwaerts-Punkt 620 (Blick 0 = +x) die Mitte des Platzes trifft; Band des Platzes. */
static void vor_den_platz(int slot)
{
    const re15_aot_t *a = &g_aot.slots[slot];
    int32_t cx = a->x, cz = a->z;
    if (a->has_quad) {
        cx = (a->xs[0] + a->xs[1] + a->xs[2] + a->xs[3]) / 4;
        cz = (a->zs[0] + a->zs[1] + a->zs[2] + a->zs[3]) / 4;
    }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = cx - 620; pl->z = cz; pl->rot_y = 0;
    if (!(a->band & 0x80)) re15_collision_set_band((int)a->band);
}

static int laeufe_boot(uint16_t raum, const lauf_t *l, int slot)
{
    flags_leer();
    flags_setzen(l);
    if (!l->kein_inv) re15_inv_grant(l->item, 1);
    if (room_boot(raum, 0, 0, 0, 0) != 0) return -1;
    re15_keyitems_r41_raum(raum);
    bild(0, 0);                         /* sub01 + Besitz-Takt */
    vor_den_platz(slot);
    return 0;
}

/* ============================================================================================ stellen */
static const uint8_t *rdt_suche(const uint8_t *m, size_t mn)
{
    for (size_t i = 0; i + mn <= s_rawsz; i++)
        if (!memcmp(s_raw + i, m, mn)) return s_raw + i;
    return NULL;
}

static void teil_stellen(void)
{
    printf("== stellen ==\n");
    const int n = re15_keyitems_r41_stellen_anzahl();
    PRUEF(n == 14, "14 Stellen (%d)", n);
    for (int i = 0; i < n; i++) {
        uint16_t raum; uint8_t item, frage, art; int slot, ev;
        re15_keyitems_r41_stelle(i, &raum, &item, &slot, &ev, &frage, &art);
        if ((raum == 0x1050 && item == 0x40) || raum == 0x2010) { PRUEF(1, "ROOM%04X Port-Programm (eigene Riegel)", raum); continue; }
        for (int v = 0; v < 2; v++) {
            const uint16_t r = (uint16_t)(raum | v);
            if (v && (raum == 0x1090 || raum == 0x4000)) continue;   /* nur_der */
            if (rdt_laden(r) != 0) { PRUEF(0, "ROOM%04X ladbar", r); continue; }
            /* Message_on <frage>, Maske 0xFFFF bzw. 0xFF80 (4030 @0x0063A) */
            uint8_t mo[4] = { 0x2b, frage, 0xff, 0xff };
            const uint8_t *q = rdt_suche(mo, 4);
            if (!q) { mo[2] = 0x80; q = rdt_suche(mo, 4); }
            PRUEF(q != NULL, "ROOM%04X Message_on %d", r, frage);
            if (!q) continue;
            /* Ck(12,31,0) folgt in den naechsten 16 Bytes (Ifel_ck oder Kette, 4000 @0x01446) */
            int ck = 0;
            for (int k = 4; k < 20; k++) if (!memcmp(q + k, "\x21\x0c\x1f\x00", 4)) ck = 1;
            PRUEF(ck, "ROOM%04X Ck(12,31,0) hinter der Frage", r);
            /* Aot_set: 2c slot sce flags ...; sce 3 + Ereignis bzw. sce 5 (Marker) */
            int gefunden = 0;
            for (size_t k = 0; k + 20 < s_rawsz; k++) {
                const uint8_t *p = s_raw + k;
                if (p[0] != 0x2c || p[1] != (uint8_t)slot) continue;
                if (ev < 0) { if (p[2] == 5 && (p[3] & 0x10)) gefunden = 1; continue; }
                const int off = (p[3] & 0x80) ? 22 : 14;
                if (p[2] == 3 && p[off] == 0xff && p[off + 2] == 0x18 && p[off + 3] == (uint8_t)ev) gefunden = 1;
            }
            PRUEF(gefunden, "ROOM%04X Aot_set Slot %d Ereignis %d", r, slot, ev);
        }
    }
    /* Die Green Keycard bleibt Tuerschloss: ROOM3010 sub02 @0x02302 ohne Frage (RE2 Lab Card Key). */
    if (rdt_laden(0x3010) == 0) {
        PRUEF(!memcmp(s_raw + 0x02302, "\x22\x03\x3c\x01\x2b\x01\xff\xff\x02\x00\x47\x02", 12), "ROOM3010 @0x02302 Tuer ohne Frage");
    }
}

/* ============================================================================================ use */
static void teil_use(void)
{
    printf("== use ==\n");
    const int n = re15_keyitems_r41_stellen_anzahl();
    for (int i = 0; i < n; i++) {
        uint16_t raum; uint8_t item, frage, art; int slot, ev;
        re15_keyitems_r41_stelle(i, &raum, &item, &slot, &ev, &frage, &art);
        const lauf_t *l = lauf_zu(raum, item);
        if (!l) { PRUEF(0, "ROOM%04X Item 0x%02x ohne Lauf-Zeile", raum, item); continue; }
        for (int v = 0; v < 2; v++) {
            const uint16_t r = (uint16_t)(raum | v);
            if (v && (raum == 0x1090 || raum == 0x4000)) continue;
            if (laeufe_boot(r, l, slot) != 0) continue;
            const re15_aot_t *a = &g_aot.slots[slot];
            PRUEF(a->active, "ROOM%04X Slot %d aktiv (Typ %d, Ereignis %d)", r, slot, (int)a->type, (int)a->event_id);
            PRUEF(re15_inv_find_item(item) >= 0, "ROOM%04X Item 0x%02x im Inventar", r, item);
            /* falscher Gegenstand, falscher Ort */
            PRUEF(re15_keyitems_r41_use(0x47 == item ? 0x46 : 0x47) == 0, "ROOM%04X falsches Item -> 0", r);
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            const int32_t x0 = pl->x;
            pl->x += 9000;
            PRUEF(re15_keyitems_r41_use(item) == 0, "ROOM%04X falscher Ort -> 0", r);
            pl->x = x0;
            const unsigned ja0 = re15_keyitems_r41_verschluckt(1);
            PRUEF(re15_keyitems_r41_use(item) == 1, "ROOM%04X USE an der Stelle -> 1", r);
            PRUEF(re15_keyitems_r41_schliessen() == 1, "ROOM%04X Ereignis gestartet", r);
            bild(0, 0);
            PRUEF(re15_keyitems_r41_verschluckt(1) == ja0 + 1, "ROOM%04X Frage %d entfallen", r, frage);
            PRUEF(!g_scd.message_active || g_scd.message_id != frage, "ROOM%04X Frage nicht offen", r);
            PRUEF(re15_game_flag_get(12, 31) == 0, "ROOM%04X Antwort Ja", r);
            PRUEF(!re15_keyitems_r41_lauf_aktiv(), "ROOM%04X USE-Lauf beendet", r);
            /* Wirkung: bis 120 Bilder. Texte HINTER der Frage bleiben und werden bestaetigt (ROOM4000 sub02 @0x0144A Message_on 2,
             * erst danach @0x01450 Set(3,32)); bei w_bank 0xFF ist genau dieser Text die Wirkung. */
            int ok = 0;
            for (int k = 0; k < 120 && !ok; k++) {
                ok = (l->w_bank == 0xFF) ? (g_scd.message_active && g_scd.message_id == l->w_msg)
                                         : re15_game_flag_get(l->w_bank, l->w_bit);
                if (!ok) bild((l->w_bank != 0xFF && (k % 8) == 7) ? RE15_PAD_BIT_SQUARE : 0, 0);
            }
            PRUEF(ok, "ROOM%04X Ja-Zweig wirkt (%d,%d / msg %d)", r, l->w_bank, l->w_bit, l->w_msg);
        }
    }
}

/* ============================================================================================ aktion */
static void teil_aktion(void)
{
    printf("== aktion ==\n");
    const int n = re15_keyitems_r41_stellen_anzahl();
    for (int i = 0; i < n; i++) {
        uint16_t raum; uint8_t item, frage, art; int slot, ev;
        re15_keyitems_r41_stelle(i, &raum, &item, &slot, &ev, &frage, &art);
        const lauf_t *l = lauf_zu(raum, item);
        if (!l) continue;
        for (int v = 0; v < 2; v++) {
            const uint16_t r = (uint16_t)(raum | v);
            if (v && (raum == 0x1090 || raum == 0x4000)) continue;
            if (laeufe_boot(r, l, slot) != 0) continue;
            const unsigned nein0 = re15_keyitems_r41_verschluckt(0);
            re15_game_flag_set(12, 31, 0);
            if (ev >= 0) scd_event_fire((uint8_t)ev);
            else { g_scd.work_vars[0] = (int16_t)slot; }
            int nachrichten = 0, auswahl = 0, vor_id = -1;
            for (int k = 0; k < 400; k++) {
                bild((k % 8) == 7 ? RE15_PAD_BIT_SQUARE : 0, 0);
                if (g_scd.message_active && (int)g_scd.message_id != vor_id) { nachrichten++; vor_id = g_scd.message_id; }
                if (g_scd.message_active && g_scd.message_fsm == 3) auswahl++;
            }
            PRUEF(re15_keyitems_r41_verschluckt(0) >= nein0 + 1, "ROOM%04X Frage %d traf den Haken", r, frage);
            PRUEF(auswahl == 0 || raum == 0x1050, "ROOM%04X keine Ja/Nein-Auswahl (%d Bilder)", r, auswahl);
            if (raum != 0x1050) PRUEF(nachrichten >= 1, "ROOM%04X Beschreibung gezeigt (%d)", r, nachrichten);
            PRUEF(re15_game_flag_get(12, 31) == 1, "ROOM%04X Antwort Nein", r);
            if (l->w_bank != 0xFF && raum != 0x4030)
                PRUEF(!re15_game_flag_get(l->w_bank, l->w_bit), "ROOM%04X Ja-Zweig wirkt nicht", r);
        }
    }
    /* Kombi-Text: die letzte Seite "Will you operate the card reader?" erscheint nicht */
    {
        const lauf_t *l = lauf_zu(0x10D0, 0x38);
        if (laeufe_boot(0x10D0, l, 1) == 0) {
            scd_event_fire(20);
            int seiten = 0, ende = 0;
            for (int k = 0; k < 500 && !ende; k++) {
                bild(0, 0);
                if (g_scd.message_active && g_scd.message_fsm == 1) { seiten++; bild(RE15_PAD_BIT_SQUARE, 0); }
                if (g_scd.message_active && g_scd.message_fsm == 4) ende = 1;
            }
            int len = 0; const unsigned char *raw = re15_msg_get_raw(7, &len);
            PRUEF(ende && seiten == 2, "ROOM10D0 msg 7: zwei Seiten, dann Ende statt Frage (Seiten %d, Ende %d)", seiten, ende);
            PRUEF(raw && (int)g_scd.message_parse < len && raw[g_scd.message_parse] == 0x02 &&
                  raw[g_scd.message_parse + 2] != 0, "Parse steht am Umbruch vor der Frage (@%d von %d)", (int)g_scd.message_parse, len);
        }
    }
}

/* ============================================================================================ besitz */
static int sicherungen(void)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == 0x40) n++;
    return n;
}

static void teil_besitz(void)
{
    printf("== besitz ==\n");
    flags_leer();
    room_boot(0x2020, 0, 0, 0, 0);
    re15_keyitems_r41_raum(0x2020);
    bild(0, 0);
    PRUEF(re15_inv_find_item(RE15_R41O_KURBEL_ITEM) < 0, "ohne (3,114) keine Kurbel");
    re15_game_flag_set(3, 114, 1);
    bild(0, 0);
    PRUEF(re15_inv_find_item(RE15_R41O_KURBEL_ITEM) >= 0, "(3,114) -> Kurbel im Inventar");
    /* in der Kiste: kein zweiter Eintrag */
    {
        int p = re15_inv_find_item(RE15_R41O_KURBEL_ITEM);
        re15_inv_remove_slot(p);
        g_itembox.slots[5].id = RE15_R41O_KURBEL_ITEM; g_itembox.slots[5].qty = 1;
        bild(0, 0);
        PRUEF(re15_inv_find_item(RE15_R41O_KURBEL_ITEM) < 0, "Kurbel in der Kiste: kein neuer Eintrag");
        g_itembox.slots[5].id = 0; g_itembox.slots[5].qty = 0;
        bild(0, 0);
        PRUEF(re15_inv_find_item(RE15_R41O_KURBEL_ITEM) >= 0, "Altstand (Flag ohne Eintrag): Eintrag entsteht");
    }
    re15_game_flag_set(3, 113, 1);
    bild(0, 0);
    PRUEF(re15_inv_find_item(RE15_R41O_KURBEL_ITEM) < 0, "(3,113) benutzt -> Kurbel verlaesst das Inventar (RE2 ROOM1110 @0x02F66)");
    /* Sicherung aus ROOM1150 ohne (3,108): bleibt */
    flags_leer();
    room_boot(0x2060, 0, 0, 0, 0);
    re15_keyitems_r41_raum(0x2060);
    re15_inv_grant(0x40, 1);
    bild(0, 0); bild(0, 0);
    PRUEF(re15_inv_find_item(0x40) >= 0, "Sicherung 0x40 ohne (3,108) bleibt");
    re15_game_flag_set(3, 108, 1);
    bild(0, 0);
    PRUEF(g_inv.slots[3].id == 0, "(3,108) mit vorhandener Sicherung: kein zweiter Eintrag");
    re15_game_flag_set(3, 144, 1);
    bild(0, 0);
    PRUEF(re15_inv_find_item(0x40) < 0, "(3,144) -> Sicherung weg");
    /* Zwei Sicherungen: Hebetisch ROOM1150 genommen ((9,53)), noch nicht im Rolltor ((9,63) = 0), dazu ROOM2030 (3,108) */
    flags_leer();
    room_boot(0x2060, 0, 0, 0, 0);
    re15_keyitems_r41_raum(0x2060);
    re15_game_flag_set(9, 53, 1);
    re15_inv_grant(0x40, 1);
    bild(0, 0); bild(0, 0);
    PRUEF(sicherungen() == 1, "Hebetisch-Sicherung allein: ein Eintrag (%d)", sicherungen());
    re15_game_flag_set(3, 108, 1);
    bild(0, 0);
    PRUEF(sicherungen() == 2, "dazu (3,108): zwei Eintraege (%d)", sicherungen());
    bild(0, 0);
    PRUEF(sicherungen() == 2, "bleibt bei zwei Eintraegen (%d)", sicherungen());
    re15_game_flag_set(3, 144, 1);
    bild(0, 0);
    PRUEF(sicherungen() == 1, "(3,144) nimmt genau eine, die fuer den Rolltor bleibt (%d)", sicherungen());
    /* Ventilkasten 2010: Bit 125 raus aus dem Inventar, zurueck beim Herausnehmen */
    flags_leer();
    room_boot(0x2010, 0, 0, 0, 0);
    re15_keyitems_r41_raum(0x2010);
    re15_game_flag_set(3, 108, 1);
    bild(0, 0);
    PRUEF(re15_inv_find_item(0x40) >= 0, "(3,108) -> Sicherung");
    re15_game_flag_set(9, RE15_R41C_BIT_KASTEN, 1);
    bild(0, 0);
    PRUEF(re15_inv_find_item(0x40) < 0, "(9,125) Sicherung im Kasten -> nicht im Inventar");
    re15_game_flag_set(9, RE15_R41C_BIT_KASTEN, 0);
    bild(0, 0);
    PRUEF(re15_inv_find_item(0x40) >= 0, "Herausgenommen -> wieder im Inventar");
    /* Zeitbombe */
    flags_leer();
    room_boot(0x2040, 0, 0, 0, 0);
    re15_keyitems_r41_raum(0x2040);
    re15_game_flag_set(3, 146, 1);
    bild(0, 0);
    PRUEF(re15_inv_find_item(RE15_R41O_BOMBE_ITEM) >= 0, "(3,146) -> Timer Bomb im Inventar");
    re15_game_flag_set(3, 53, 1);
    bild(0, 0);
    PRUEF(re15_inv_find_item(RE15_R41O_BOMBE_ITEM) < 0, "(3,53) -> Timer Bomb weg");
    /* Zuendkerze: verlaesst das Inventar mit (3,78), nur an der Flanke */
    flags_leer();
    room_boot(0x4030, 0, 0, 0, 0);
    re15_keyitems_r41_raum(0x4030);
    re15_inv_grant(0x41, 1);
    bild(0, 0);
    PRUEF(re15_inv_find_item(0x41) >= 0, "Zuendkerze bleibt bis zur Benutzung");
    re15_game_flag_set(3, 78, 1);
    bild(0, 0);
    PRUEF(re15_inv_find_item(0x41) < 0, "(3,78) -> Zuendkerze weg");
}

/* ============================================================================================ voll */
static void teil_voll(void)
{
    printf("== voll ==\n");
    static const struct { uint16_t raum; uint8_t ev; uint8_t bank, bit; } k[] = {
        { 0x2010, 2, 3, 114 }, { 0x2011, 2, 3, 114 }, { 0x1110, 3, 3, 146 }, { 0x2030, 6, 3, 108 },
    };
    for (unsigned i = 0; i < sizeof k / sizeof k[0]; i++) {
        flags_leer();
        for (int s = 0; s < 10; s++) { g_inv.slots[s].id = (uint8_t)(0x15 + (s & 1)); g_inv.slots[s].qty = 1; }
        room_boot(k[i].raum, 0, 0, 0, 0);
        re15_keyitems_r41_raum(k[i].raum);
        bild(0, 0);
        scd_event_fire(k[i].ev);
        int modal = 0, frage = 0;
        for (int b = 0; b < 200; b++) {
            bild((b % 8) == 7 ? RE15_PAD_BIT_SQUARE : 0, 0);
            if (re15_item_modal_active()) modal++;
            if (g_scd.message_active && g_scd.message_fsm == 3) frage++;
        }
        PRUEF(modal > 0 && frage == 0, "ROOM%04X volles Inventar: Modal %d Bilder, keine Frage", k[i].raum, modal);
        PRUEF(!re15_game_flag_get(k[i].bank, k[i].bit), "ROOM%04X Flag (%d,%d) bleibt 0", k[i].raum, k[i].bank, k[i].bit);
    }
    /* mit Platz: Frage wie bisher */
    flags_leer();
    room_boot(0x2010, 0, 0, 0, 0);
    re15_keyitems_r41_raum(0x2010);
    bild(0, 0);
    scd_event_fire(2);
    int frage = 0;
    for (int b = 0; b < 120 && !frage; b++) {
        bild(0, 0);
        if (g_scd.message_active && g_scd.message_fsm == 3) frage = 1;
    }
    PRUEF(frage, "ROOM2010 mit Platz: \"Will you take the Crank?\" bleibt");
}

/* ============================================================================================ kurbel */
static void teil_kurbel(void)
{
    printf("== kurbel ==\n");
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int v = 0; v < 2; v++) {
        const uint16_t r = (uint16_t)(0x2020 | v);
        const lauf_t *l = lauf_zu(0x2020, RE15_R41O_KURBEL_ITEM);
        if (laeufe_boot(r, l, 3) != 0) continue;
        PRUEF(re15_keyitems_r41_use(RE15_R41O_KURBEL_ITEM) == 1, "ROOM%04X USE", r);
        re15_keyitems_r41_schliessen();
        bild(0, 0);
        PRUEF(pl->x == 3650, "ROOM%04X erstes Bild: Pos_set @0x0A8C x (%d)", r, (int)pl->x);
        PRUEF(pl->z == -3018, "ROOM%04X erstes Bild: z %d statt -3018 (Pos_set @0x0A8C -2925, Schub wie Original o_stand1 n90)", r, (int)pl->z);
        PRUEF(pl->rot_y == 1024, "ROOM%04X erstes Bild: Gier @0x0A94 (%d)", r, (int)pl->rot_y);
        PRUEF(pl->anim_flags & 0x40, "ROOM%04X Plc_motion ohne Blende: Flags 0x%02x", r, (unsigned)pl->anim_flags);
        PRUEF(pl->anim_frac == 0, "ROOM%04X anim_frac 0 (%d)", r, (int)pl->anim_frac);
        PRUEF(pl->motion == 0, "ROOM%04X Clip 0 (%d)", r, (int)pl->motion);
        PRUEF(g_scd.cam_id == 3, "ROOM%04X Cut_chg 3 im ersten Bild (%d)", r, (int)g_scd.cam_id);
        PRUEF(re15_game_flag_get(3, 113) == 1, "ROOM%04X Set(3,113) @0x0A7A", r);
        bild(0, 0);
        PRUEF(re15_inv_find_item(RE15_R41O_KURBEL_ITEM) < 0, "ROOM%04X Kurbel weg", r);
    }
    /* Gegenprobe: alter Stand = Altstand-Weg (Kurbel-Flag ohne Eintrag und ohne Platz): Frage, Ja, Blende 7 */
    {
        flags_leer();
        for (int s = 0; s < 10; s++) { g_inv.slots[s].id = (uint8_t)(0x15 + (s & 1)); g_inv.slots[s].qty = 1; }
        re15_game_flag_set(3, 114, 1);
        room_boot(0x2020, 2600, -3750, 0, 0);
        re15_keyitems_r41_raum(0x2020);
        bild(0, 0);
        scd_event_fire(2);
        int frage = 0;
        for (int b = 0; b < 300; b++) {
            bild((b % 8) == 7 ? RE15_PAD_BIT_SQUARE : 0, 0);
            if (g_scd.message_active && g_scd.message_fsm == 3) { frage = 1; break; }
        }
        PRUEF(frage, "Altstand ohne Eintrag: Frage bleibt (keine tote Stelle)");
        int blende = -1;
        for (int b = 0; b < 200 && blende < 0; b++) {
            bild((b % 8) == 7 ? RE15_PAD_BIT_SQUARE : 0, 0);
            if (pl->state == 4 && pl->motion == 0) blende = pl->anim_frac;
        }
        PRUEF(blende >= 6, "Gegenprobe Frage+Ja: Plc_motion mit Blende (+0x8f %d, @0x80050cfc)", blende);
        int ende_frac = -1;
        for (int b = 0; b < 400 && ende_frac < 0; b++) {
            bild(0, 0);
            if (pl->state != 4) ende_frac = pl->anim_frac;
        }
        PRUEF(ende_frac >= 6, "Gegenprobe Frage+Ja: Plc_ret @0x0B16 mit Mischung (+0x8f %d)", ende_frac);
    }
}

/* ============================================================================================ waffe */
static void teil_waffe(void)
{
    printf("== waffe ==\n");
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const lauf_t *l = lauf_zu(0x2020, RE15_R41O_KURBEL_ITEM);
    if (laeufe_boot(0x2020, l, 3) != 0) return;
    PRUEF(re15_player_equipped_weapon() == 3 && re15_inv_equipped_slot() == 0, "Handgun angelegt");
    PRUEF(re15_keyitems_r41_use(RE15_R41O_KURBEL_ITEM) == 1, "USE");
    re15_keyitems_r41_schliessen();
    PRUEF(re15_player_equipped_weapon() == 0, "RE2 @0x80068d34 5bfa := 0: keine Waffe an Knochen 11");
    PRUEF(re15_inv_equipped_slot() == 0, "Ausruest-Platz 25c8 bleibt (Inventar zeigt die Waffe weiter)");
    int ende = -1, ende_frac = -1, ende_mo = -1;
    for (int b = 0; b < 400 && ende < 0; b++) {
        bild(0, 0);
        if (pl->state != 4 && b > 0) { ende = b; ende_frac = pl->anim_frac; ende_mo = pl->motion; }
        else PRUEF(b == 0 || re15_player_equipped_weapon() == 0 || pl->state != 4, "F%d unbewaffnet", b);
    }
    PRUEF(ende > 100, "Szene bis Plc_ret @0x0B16 (%d Bilder)", ende);
    PRUEF(ende_frac == 0 && ende_mo == 200, "Plc_ret: Stehpose ohne Mischung, Wechsel faellt in den Schnitt @0x0B0E (frac %d, Clip %d)", ende_frac, ende_mo);
    bild(0, 0);
    PRUEF(re15_player_equipped_weapon() == 3, "PORT-WAHL: nach Plc_ret Waffe zurueck (%d)", re15_player_equipped_weapon());
    /* andere Stellen legen nicht ab */
    const lauf_t *b = lauf_zu(0x2060, 0x40);
    if (laeufe_boot(0x2060, b, 9) != 0) return;
    PRUEF(re15_keyitems_r41_use(0x40) == 1, "USE Sicherung");
    re15_keyitems_r41_schliessen();
    PRUEF(re15_player_equipped_weapon() == 3, "Sicherung: Waffe bleibt (RE2-Liste nur 0x32/0x4B/0x43)");
}

/* ============================================================================================ daten */
static void teil_daten(void)
{
    printf("== daten ==\n");
    const uint8_t *n = re15_keyitems_r41_name(RE15_R41O_KURBEL_ITEM);
    PRUEF(n && !memcmp(n, "\x1f\x4e\x3d\x4a\x47\x07", 6), "Name \"Crank\" RE2 PSX.EXE @0x8F041");
    const uint8_t *d = re15_keyitems_r41_desc(RE15_R41O_KURBEL_ITEM);
    PRUEF(d && d[0] == 0x30 && d[10] == 0x08 && d[24] == 0x01 && d[25] == 0x00, "CHECK \"The end is / square-shaped\" RE2 @0x9206D");
    PRUEF(re15_keyitems_r41_name(0x34) == NULL && re15_keyitems_r41_desc(0x35) == NULL, "nur die Kurbel hat Port-Texte");
    static uint8_t pix[86400];
    re15_keyitems_r41_icon_einsetzen(pix, (int)sizeof pix);
    int nz = 0; for (int i = 0; i < 1200; i++) nz += pix[RE15_R41O_KURBEL_ITEM * 1200 + i] != 0;
    PRUEF(nz == 1200, "Icon-Tile 0x33 gesetzt, Index 0 kommt nicht vor (%d)", nz);
    static uint8_t itp[884736];
    itp[RE15_R41O_BOMBE_ITEM * 0x3000 + 0x0E] = 0xE0;   /* ausgeliefert cy 480 */
    re15_keyitems_r41_bild_einsetzen(itp, (int)sizeof itp);
    const uint8_t *k = itp + RE15_R41O_KURBEL_ITEM * 0x3000, *b = itp + RE15_R41O_BOMBE_ITEM * 0x3000;
#define U16(p, o) ((unsigned)(p)[o] | ((unsigned)(p)[(o) + 1] << 8))
    PRUEF(U16(k, 0x0C) == 0 && U16(k, 0x0E) == 489 && U16(k, 0x218) == 832 && U16(k, 0x21A) == 256, "Kurbel-Bild: Rechtecke RE1.5");
    PRUEF(U16(b, 0x0C) == 0 && U16(b, 0x0E) == 489 && U16(b, 0x218) == 832 && U16(b, 0x21A) == 256, "Bomben-Bild: Rechtecke RE1.5");
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if      (!strcmp(t, "stellen")) teil_stellen();
    else if (!strcmp(t, "use"))     teil_use();
    else if (!strcmp(t, "aktion"))  teil_aktion();
    else if (!strcmp(t, "besitz"))  teil_besitz();
    else if (!strcmp(t, "voll"))    teil_voll();
    else if (!strcmp(t, "kurbel"))  teil_kurbel();
    else if (!strcmp(t, "waffe"))   teil_waffe();
    else if (!strcmp(t, "daten"))   teil_daten();
    else { printf("Teil?\n"); return 2; }
    printf("%d Pruefungen, %d Fehler\n", g_n, g_fail);
    return g_fail ? 1 : 0;
}
