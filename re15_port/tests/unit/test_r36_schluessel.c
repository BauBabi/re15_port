/*
 * Runde 36 Spur Q "schluessel" — Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit allen
 * Port-Installern, re15_aot_scan mit Aktionsdruck, scd_event_fire, Item-Modal, Namens-/Bild-Leser).
 * Dossier analysis/befunde_runde36/Q_schluessel.md, Konstanten include/re15_schluessel_r36.h.
 *
 *   test_r36_schluessel raum    10F0/10F1: Prop + Zone an der Lage, Bit -> weg, echter Druck -> Modal
 *                               mit Item 0x45 bzw. 0x42, "Ja" -> Bit 82/83, im Inventar; Elza: (4,0xEC)
 *   test_r36_schluessel tuer    ROOM11B0 (Strom an): Slot 1 gesperrt; Druck OHNE Schluessel -> msg 28 +
 *                               Verschlossen-Ton, kein Raumwechsel; Druck MIT Schluessel -> msg 29 +
 *                               Aufschliess-Ton + (9,93), Slot 1 wieder die Original-Tuer, naechster
 *                               Druck -> ROOM11C0; (9,93) gesetzt -> keine Sperre; ohne Strom und in
 *                               ROOM11B1 (Elza) bleibt die Tuer unberuehrt
 *   test_r36_schluessel zelle   ROOM1011: Roy-Ereignis 6 laeuft als sub07 (msg 18), (4,0xEC) bleibt 0;
 *                               ROOM1211: ohne (4,0xEC) Text msg 10, mit (4,0xEC) John + Befreiungs-Platz
 *   test_r36_schluessel namen   Prompt "Will you take the <Name>?", CHECK-Text, Icon-Tile, Item-Bild
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_schluessel_r36.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_item_modal.h"
#include "re15_item_prompt.h"
#include "re15_inventory.h"
#include "re15_inv_screen.h"
#include "re15_itps.h"
#include "re15_files.h"
#include "re15_lock_se.h"
#include "re15_md1.h"
#include "re15_msg.h"
#include "re15_gameflow.h"
#include "re15_tim.h"

#include "gen/re2_door_bank.inc"

extern re15_aot_state_t g_aot;
extern uint8_t          g_aot_action_pressed;
extern int g_test_door_se_last, g_test_door_se_count;
extern int scd_event_fire(uint8_t event_id);
extern void re15_itemall_set_pix(const uint8_t *pix, int size);
extern const uint8_t *re15_itemall_tile_raw(uint8_t tile);

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
#ifndef RE15_RE2_DIR
#error RE15_RE2_DIR fehlt (info/re2leon)
#endif

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; uint16_t id; } raum_t;

static uint8_t *slurp(const char *p, int *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(b); return NULL; }
    fclose(f);
    if (n) *n = (int)sz;
    return b;
}

static int raum_laden(raum_t *r, uint16_t id)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(id >> 12), (unsigned)id);
    int n = 0;
    r->buf = slurp(p, &n);
    if (!r->buf) return -1;
    r->n = (size_t)n; r->id = id;
    return re15_rdt_parse(r->buf, r->n, &r->rdt) == 0 ? 0 : -1;
}

static int32_t s_px, s_pz; static int16_t s_prot;   /* Standort des Spielers (ruhe) */
/* Flags als Paare (bank, bit), Ende bank < 0. */
static void hochfahren(raum_t *r, int32_t px, int32_t pz, int16_t rot, const int *flags, int elza)
{
    re15_game_state_init();
    re15_inv_init();
    re15_files_reset();
    re15_aot_init();
    scd_vm_init(); re15_actor_init();
    for (int k = 0; flags && flags[k] >= 0; k += 2) re15_game_flag_set((uint8_t)flags[k], (uint8_t)flags[k + 1], 1);
    g_gameflow.character = elza ? 4 : 0;
    g_current_room_id = r->id;
    g_room_rdt = r->rdt; g_room_rdt_ok = 1;
    memset(&g_room_change, 0, sizeof g_room_change);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = rot; pl->state = 1;
    s_px = px; s_pz = pz; s_prot = rot;
    re15_collision_set_band(0);
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size); }
    scd_register_room_events(&r->rdt);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

/* n Bilder laufen lassen, Nachricht/Freeze loesen und den Spieler an (x,z,rot) zurueckstellen — der
 * Raumstart (sub00 / Szenario) setzt ihn sonst an seinen eigenen Eintrittspunkt (Muster
 * test_r35_inhalt_items.c druck()). */
static void ruhe(int n)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < n; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
    g_scd.message_fsm_active = 0;
    re15_pauseflags_clear();
    pl->x = s_px; pl->z = s_pz; pl->rot_y = s_prot;
}

/* EIN Aktionsdruck wie im Spiel: Flanke -> re15_aot_scan; ein GENERIC-Feuer laeuft wie in
 * game_step_common.c (g_aot.fired_event_id_this_frame -> scd_event_fire). Rueckgabe: das Ereignis. */
static int druck(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    g_aot_action_pressed = 0;
    int ev = g_aot.fired_event_id_this_frame;
    if (ev) (void)scd_event_fire((uint8_t)ev);
    return ev;
}

static int slot_von(uint8_t oid)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == oid) return k;
    return -1;
}

static int menge_im_inventar(uint8_t id)
{
    int m = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) m += g_inv.slots[i].qty;
    return m;
}

/* ======================================================================================== raum */
static void teil_raum(void)
{
    /* Soll unabhaengig vom Kopf aufgeschrieben (der Riegel soll eine Kopf-Aenderung bemerken). */
    static const struct { uint16_t raum; uint8_t item, bit; int elza; const char *name; } k[2] = {
        { 0x10F0, 0x45, 82, 0, "Parking Lot Key" },
        { 0x10F1, 0x42, 83, 1, "Prison Key" },
    };
    const int32_t stand_x = -2150, stand_z = 2600; const int16_t stand_rot = 3072;   /* Blick +z */
    for (int i = 0; i < 2; i++) {
        raum_t r;
        if (raum_laden(&r, k[i].raum) != 0) { printf("FAIL: ROOM%04X nicht lesbar\n", k[i].raum); fails++; continue; }
        printf("\n[raum] ROOM%04X %s\n", k[i].raum, k[i].name);
        hochfahren(&r, stand_x, stand_z, stand_rot, NULL, k[i].elza);
        int p = slot_von(4);
        CHECK(p >= 0 && g_scd.props[p].x == -2150 && g_scd.props[p].y == -1300 && g_scd.props[p].z == 3500 &&
              g_scd.props[p].rot_y == 1024 && g_scd.props[p].flags == 0x000B && g_scd.props[p].parent_obj == -1 &&
              g_scd.props[p].band == 1,
              "P obj 4 bei (-2150,-1300,3500) rot 1024, Flags 0x000B, Band 1, Welt");
        const re15_aot_t *a = &g_aot.slots[15];
        CHECK(a->active && a->type == RE15_AOT_TYPE_ITEM && g_aot.item_params[15].item_type == k[i].item &&
              g_aot.item_params[15].amount == 1 && g_aot.item_params[15].taken_bit == k[i].bit &&
              g_aot.item_params[15].taken_prop == 4 && a->sce_flags == 0x31 && a->band == 0 &&
              a->x - a->half_w == -2650 && a->x + a->half_w == -1650 && a->z - a->half_h == 3000 &&
              a->z + a->half_h == 4000,
              "Z Slot 15: Item 0x%02X x1, Bit (9,%d), Prop 4, sat 0x31, x[-2650..-1650] z[3000..4000]",
              k[i].item, k[i].bit);
        /* Original-Saetze unberuehrt: Slot 13 Remington (Zone x[-1450..-450]) bleibt, keine Ueberschneidung */
        CHECK(g_aot.slots[13].active && g_aot.item_params[13].item_type == 0x08 &&
              g_aot.slots[13].x - g_aot.slots[13].half_w > a->x + a->half_w,
              "Z Slot 13 (Remington 0x08) unveraendert, beginnt rechts der neuen Zone");
        const int bitflag[3] = { 9, k[i].bit, -1 };
        hochfahren(&r, stand_x, stand_z, stand_rot, bitflag, k[i].elza);
        CHECK(slot_von(4) < 0 && !(g_aot.slots[15].active && g_aot.slots[15].type == RE15_AOT_TYPE_ITEM),
              "G Bit (9,%d) gesetzt: weder Prop noch Zone", k[i].bit);

        /* D: echter Aktionsdruck vom Platz vor dem Tisch -> Modal -> Ja */
        hochfahren(&r, stand_x, stand_z, stand_rot, NULL, k[i].elza);
        ruhe(12);
        druck();
        uint8_t typ = 0; int wahl = -1, w = 0;
        int modal = re15_item_modal_active();
        while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800) re15_item_modal_tick(0, 0);
        re15_item_modal_prompt(&typ, &wahl);
        CHECK(modal && typ == k[i].item, "D Druck (%d,%d) Blick +z: Aufnahme-Modal Item 0x%02X (ist 0x%02X)",
              stand_x, stand_z, k[i].item, typ);
        w = 0;
        while (re15_item_modal_active() && w++ < 1600) re15_item_modal_tick((uint16_t)0x4000u, 0);
        CHECK(re15_game_flag_get(9, k[i].bit) && menge_im_inventar(k[i].item) == 1 && slot_von(4) >= 0 &&
              !g_scd.props[slot_von(4)].active,
              "D Ja: Bit (9,%d), %s x1 im Inventar, Modell ausgeblendet", k[i].bit, k[i].name);
        CHECK(re15_game_flag_get(4, 0xEC) == (k[i].elza ? 1 : 0),
              "D Zellen-Flag (4,0xEC) = %d (nur der Prison Key setzt es, wie ROOM1011 sub06 @0x00E2C)",
              k[i].elza ? 1 : 0);
        /* E: Modell parst */
        int mn = 0, tn = 0;
        const uint8_t *mb = re15_schluessel_r36_md1_bytes(k[i].raum, &mn);
        const uint8_t *tb = re15_schluessel_r36_tim_bytes(k[i].raum, &tn);
        static re15_md1_t md; re15_tim_t tim;
        memset(&md, 0, sizeof md); memset(&tim, 0, sizeof tim);
        CHECK(mb && tb && re15_md1_parse(mb, (size_t)mn, &md) == 0 && re15_tim_parse(tb, tn, &tim) == 0 &&
              re15_schluessel_r36_obj_id(k[i].raum) == 4 && mn == (i == 0 ? 236 : 140),
              "E Modell: MD1 %d B + TIM %d B parsen (RE2 %s)", mn, tn, i == 0 ? "P. Room Key" : "Cabin Key");
        free(r.buf);
    }
    CHECK(re15_schluessel_r36_obj_id(0x1011) < 0 && re15_schluessel_r36_obj_id(0x11B0) < 0 &&
          re15_schluessel_r36_md1_bytes(0x1020, NULL) == NULL, "F fremde Raeume: kein Prop");
}

/* ======================================================================================== tuer */
static void teil_tuer(void)
{
    raum_t r, r1;
    if (raum_laden(&r, 0x11B0) != 0 || raum_laden(&r1, 0x11B1) != 0) { printf("FAIL: ROOM11B0/11B1\n"); fails++; return; }
    /* Spieler mitten im Tuer-Rechteck (-26730..-24230, -29940..-26440): 620 voraus liegt in jeder
     * Richtung innen (halbe Ausdehnung 1250 / 1750). */
    const int32_t tx = -25480, tz = -28190;
    const int strom[3] = { 4, 0xF3, -1 };
    printf("\n[tuer] ROOM11B0 Parkplatz-Tuer\n");

    /* 1. ohne Schluessel */
    hochfahren(&r, tx, tz, 0, strom, 0);
    ruhe(6);
    const re15_aot_t *a = &g_aot.slots[1];
    CHECK(re15_schluessel_r36_tuer_gesperrt() && a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == 25 &&
          a->sce_flags == 0x31 && g_aot.door_params[1].dest_room == 0x1C,
          "S Strom an, (9,93)=0: Slot 1 = Ereignis 25 (sat 0x31), Tuer-Ziel Raum 0x1C bleibt gespeichert");
    {   extern int re15_collision_debug_band(void);
        printf("      Slot 1: aktiv %d Typ %d Ereignis %d sat 0x%02X Band %d Rechteck (%d,%d) hw %d hh %d; "
               "Spieler (%d,%d) rot %d Band %d\n", a->active, a->type, a->event_id, a->sce_flags, a->band,
               a->x, a->z, a->half_w, a->half_h, g_actors[0].x, g_actors[0].z, g_actors[0].rot_y,
               re15_collision_debug_band());
        for (int s2 = 0; s2 < 48; s2++) {
            const re15_aot_t *b = &g_aot.slots[s2];
            if (!b->active) continue;
            int in = (abs(tx - b->x) <= b->half_w) && (abs(tz - b->z) <= b->half_h);
            if (in || s2 < 8) printf("      Slot %2d Typ %d ev %d sat 0x%02X Band %d (%d,%d) hw %d hh %d %s\n", s2, b->type,
                                     b->event_id, b->sce_flags, b->band, b->x, b->z, b->half_w, b->half_h, in ? "<- Spieler" : "");
        }
    }
    int se0 = g_test_door_se_count;
    int ev = druck();
    CHECK(ev == 25 && g_scd.message_id == 28 && g_test_door_se_count == se0 + 1 &&
          g_test_door_se_last == re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M) && !g_room_change.pending &&
          !re15_game_flag_get(9, 93),
          "Z Druck ohne Schluessel: Ereignis %d, msg %d (28), Ton Satz %d (ZU_A = Art M), kein Raumwechsel",
          ev, g_scd.message_id, g_test_door_se_last);
    {
        int n = 0; const uint8_t *m = re15_schluessel_r36_meldung(28, &n);
        char txt[160] = { 0 };
        re15_msg_decode_text(m, (size_t)n, txt, sizeof txt);
        printf("      msg 28 = \"%s\"\n", txt);
        CHECK(strstr(txt, "It's locked.") && strstr(txt, "The door is marked") && strstr(txt, "Parking Lot"),
              "Z msg 28 Wortlaut (Form RE2 Index 0x13 @0x8009F2BB)");
    }
    /* Ein Inventar OHNE Schluessel, aber mit gesetztem Aufnahme-Bit (Schluessel z.B. in der Item-Box):
     * RE2 testet das INVENTAR (@0x80051628), nicht das Bit -> weiter verschlossen. */
    {
        const int fl[5] = { 4, 0xF3, 9, 82, -1 };
        hochfahren(&r, tx, tz, 0, fl, 0);
        ruhe(6);
        druck();
        CHECK(g_scd.message_id == 28 && !re15_game_flag_get(9, 93),
              "Z Bit (9,82) gesetzt, Schluessel NICHT im Inventar: weiter msg 28 (RE2 prueft das Inventar)");
    }

    /* 2. mit Schluessel */
    hochfahren(&r, tx, tz, 0, strom, 0);
    re15_inv_grant(0x45, 1);
    ruhe(6);
    se0 = g_test_door_se_count;
    ev = druck();
    CHECK(ev == 25 && g_scd.message_id == 29 && g_test_door_se_count == se0 + 1 &&
          g_test_door_se_last == RE2_DOOR_SE_AUF_KEY && re15_game_flag_get(9, 93) && !g_room_change.pending,
          "B Druck mit Schluessel: msg %d (29), Ton Satz %d (AUF_KEY = RE2 ROOM1140 0x25), (9,93)=1, "
          "KEIN Durchgang bei diesem Druck (RE2 v0 = 0 @0x80051698)", g_scd.message_id, g_test_door_se_last);
    CHECK(a->active && a->type == RE15_AOT_TYPE_DOOR && g_aot.door_params[1].dest_room == 0x1C &&
          g_aot.door_params[1].spawn_x == -25279 && g_aot.door_params[1].spawn_z == 17268 &&
          !re15_schluessel_r36_tuer_gesperrt(),
          "B Slot 1 wieder die Original-Tuer (Ziel 0x1C, Spawn (-25279,0,17268) wie main00 @0x00FAA)");
    {
        int n = 0; const uint8_t *m = re15_schluessel_r36_meldung(29, &n);
        char txt[160] = { 0 };
        re15_msg_decode_text(m, (size_t)n, txt, sizeof txt);
        printf("      msg 29 = \"%s\"\n", txt);
        CHECK(strstr(txt, "You've used the") && strstr(txt, "Parking Lot Key"), "B msg 29 Wortlaut (Form ROOM11E0 msg 12)");
    }
    /* naechster Druck: die Tuer geht (Tuer-Halten 9 Bilder, door_hold FUN_8002bd44) */
    ruhe(2);
    {
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        g_aot.slots[1].was_inside = 0;
        for (int f = 0; f < 12 && !g_room_change.pending; f++) {
            g_aot_action_pressed = 1;
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
        }
        g_aot_action_pressed = 0;
    }
    CHECK(g_room_change.pending && (g_room_change.room_id & 0xFFF0u) == 0x11C0u,
          "B naechster Druck: Raumwechsel nach ROOM%04X", g_room_change.room_id);

    /* 3. schon aufgeschlossen */
    {
        const int fl[5] = { 4, 0xF3, 9, 93, -1 };
        hochfahren(&r, tx, tz, 0, fl, 0);
        CHECK(!re15_schluessel_r36_tuer_gesperrt() && g_aot.slots[1].type == RE15_AOT_TYPE_DOOR,
              "O (9,93) gesetzt: keine Sperre, Slot 1 = Tuer");
    }
    /* 4. ohne Strom: sub01 @0x01216 macht Slot 1 zum Text msg 15, die Sperre greift nicht */
    hochfahren(&r, tx, tz, 0, NULL, 0);
    ruhe(3);
    CHECK(!re15_schluessel_r36_tuer_gesperrt() && g_aot.slots[1].type == RE15_AOT_TYPE_MESSAGE &&
          g_aot.slots[1].event_id == 15,
          "D ohne Strom: Slot 1 = Text msg 15 (\"It's too dark...\", RE1.5 sub01 @0x01216), keine Sperre");
    /* 5. Elza ROOM11B1: unberuehrt */
    hochfahren(&r1, tx, tz, 0, strom, 1);
    ruhe(3);
    CHECK(!re15_schluessel_r36_tuer_gesperrt() && g_aot.slots[1].type == RE15_AOT_TYPE_DOOR,
          "E ROOM11B1 (Elza): Parkplatz-Tuer unberuehrt (Nachtrag 14:00)");
    free(r.buf); free(r1.buf);
}

/* ======================================================================================= zelle */
static void teil_zelle(void)
{
    raum_t r, z;
    if (raum_laden(&r, 0x1011) != 0 || raum_laden(&z, 0x1211) != 0) { printf("FAIL: ROOM1011/1211\n"); fails++; return; }
    printf("\n[zelle] ROOM1011 Roy / ROOM1211 Zelle\n");
    /* Roy bewusstlos: (4,0xF1)=1 Dialog gelaufen, (4,0xFA)=1 -> sub00 @0x00A3E Roy + @0x00A52 Slot 5 -> sub06 */
    const int fl[5] = { 4, 0xF1, 4, 0xFA, -1 };
    hochfahren(&r, -2000, 6000, 0, fl, 1);
    ruhe(3);
    CHECK(g_aot.slots[5].active && g_aot.slots[5].event_id == 6,
          "R ROOM1011 Slot 5 zeigt (Original) auf sub06 (Prison key, @0x00A52)");
    (void)scd_event_fire(g_aot.slots[5].event_id);
    int msg = -1, sah19 = 0;
    for (int f = 0; f < 120; f++) {
        scd_vm_tick();
        if (g_scd.message_active) { if (msg < 0) msg = g_scd.message_id; if (g_scd.message_id == 19 || g_scd.message_id == 20) sah19 = 1; }
    }
    CHECK(msg == 18 && !sah19 && !re15_game_flag_get(4, 0xEC),
          "R Roy untersuchen: msg %d (18 = Zettel 0513, sub07), keine msg 19/20, (4,0xEC) bleibt 0", msg);

    /* Zelle ROOM1211 */
    hochfahren(&z, -26000, -20000, 0, NULL, 1);
    ruhe(3);
    CHECK(g_aot.slots[8].active && g_aot.slots[8].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[8].event_id == 10,
          "Z ROOM1211 ohne (4,0xEC): Slot 8 Text msg 10 (\"It's locked. It seems that there is someone inside\")");
    const int fz[3] = { 4, 0xEC, -1 };
    hochfahren(&z, -26000, -20000, 0, fz, 1);
    ruhe(3);
    int john = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == 0x49) john = 1;
    CHECK(john && g_aot.slots[6].active && g_aot.slots[6].event_id == 2,
          "Z ROOM1211 mit (4,0xEC) (= Prison Key genommen): John (Typ 0x49) in der Zelle, Slot 6 -> sub02 (Befreiung)");
    free(r.buf); free(z.buf);
}

/* ======================================================================================= namen */
typedef struct { unsigned char g[256]; int n; } glyphen_t;
static void sammle(void *ctx, unsigned char code, int attr, int nl)
{
    glyphen_t *s = (glyphen_t *)ctx; (void)attr;
    if (s->n < 255) s->g[s->n++] = nl ? 0x08 : code;
}

static int enthaelt(const glyphen_t *s, const uint8_t *name)
{
    int l = 0; while (name[l] != 0x07) l++;
    for (int i = 0; i + l <= s->n; i++) if (memcmp(s->g + i, name, (size_t)l) == 0) return 1;
    return 0;
}

static void teil_namen(void)
{
    printf("\n[namen] Name, CHECK-Text, Icon, Item-Bild\n");
    static const uint8_t ids[2] = { 0x45, 0x42 };
    for (int i = 0; i < 2; i++) {
        const uint8_t *nm = re15_schluessel_r36_name(ids[i]);
        glyphen_t s; memset(&s, 0, sizeof s);
        int n = re15_item_prompt_walk(0, ids[i], 255, sammle, &s);
        CHECK(nm && n > 0 && enthaelt(&s, nm), "N Prompt [0] \"Will you take the <Name>?\" fuer 0x%02X traegt den Port-Namen (%s)",
              ids[i], re15_schluessel_r36_name_ascii(ids[i]));
        memset(&s, 0, sizeof s);
        n = re15_item_prompt_walk(5, ids[i], 255, sammle, &s);
        CHECK(nm && n > 0 && enthaelt(&s, nm), "N Prompt [2] \"You have used the <Name>.\" fuer 0x%02X", ids[i]);
        CHECK(re15_inv_desc_entry(ids[i]) == re15_schluessel_r36_desc(ids[i]) && re15_inv_desc_entry(ids[i]),
              "C CHECK-Text 0x%02X kommt aus dem Port (Beschreibungsbank-Leser re15_inv_desc_entry)", ids[i]);
    }
    CHECK(re15_schluessel_r36_name(0x38) == NULL && re15_inv_desc_entry(0x38) != NULL &&
          re15_schluessel_r36_desc(0x38) == NULL, "C fremde Id 0x38 (Blue Keycard) unveraendert");

    /* Icon: RE1.5 ITEMALL.PIX laden, einsetzen, gegen RE2 ITEMALL.PIX Tile 95/88 */
    char p[600]; int n15 = 0, n2 = 0;
    snprintf(p, sizeof p, "%s/DATA/ITEMALL.PIX", RE15_ASSET_PSX_DIR);
    uint8_t *pix = slurp(p, &n15);
    snprintf(p, sizeof p, "%s/COMMON/DATA/ITEMALL.PIX", RE15_RE2_DIR);
    uint8_t *pix2 = slurp(p, &n2);
    if (pix && pix2) {
        uint8_t *orig = (uint8_t *)malloc((size_t)n15); memcpy(orig, pix, (size_t)n15);
        re15_schluessel_r36_icon_einsetzen(pix, n15);
        int anders = 0;
        for (int b = 0; b < n15; b++) if (pix[b] != orig[b] && (b / 1200 != 0x45 && b / 1200 != 0x42)) anders++;
        CHECK(memcmp(pix + 0x45 * 1200, pix2 + 95 * 1200, 1200) == 0 && memcmp(pix + 0x42 * 1200, pix2 + 88 * 1200, 1200) == 0 &&
              anders == 0, "I Icon-Tiles 0x45 = RE2-Tile 95, 0x42 = RE2-Tile 88; %d fremde Bytes veraendert", anders);
        re15_itemall_set_pix(pix, n15);
        CHECK(re15_itemall_tile_raw(0x45) && memcmp(re15_itemall_tile_raw(0x45), pix2 + 95 * 1200, 1200) == 0,
              "I Leser re15_itemall_tile_raw(0x45) liefert das RE2-Icon");
        free(orig);
    } else { printf("FAIL: ITEMALL.PIX nicht lesbar\n"); fails++; }

    /* Item-Bild: RE1.5 ITPS.ITP laden, einsetzen; Rechtecke RE1.5 (0,489)/(832,256), Bildbytes = RE2-Block */
    int t15 = 0, t2 = 0;
    snprintf(p, sizeof p, "%s/ITEM/ITPS.ITP", RE15_ASSET_PSX_DIR);
    uint8_t *itps = slurp(p, &t15);
    snprintf(p, sizeof p, "%s/COMMON/DATA/ITPS.ITP", RE15_RE2_DIR);
    uint8_t *itps2 = slurp(p, &t2);
    if (itps && itps2) {
        re15_schluessel_r36_bild_einsetzen(itps, t15);
        static const struct { uint8_t id; int re2; } bl[2] = { { 0x45, 95 }, { 0x42, 88 } };
        for (int i = 0; i < 2; i++) {
            const uint8_t *b = itps + bl[i].id * 0x3000, *q = itps2 + bl[i].re2 * 0x3000;
            int cx = b[12] | (b[13] << 8), cy = b[14] | (b[15] << 8);
            int px = b[0x218] | (b[0x219] << 8), py = b[0x21A] | (b[0x21B] << 8);
            int gleich = memcmp(b + 0x14, q + 0x14, 0x200) == 0 && memcmp(b + 0x220, q + 0x220, 0x3000 - 0x220) == 0;
            CHECK(cx == 0 && cy == 489 && px == 832 && py == 256 && gleich,
                  "B ITPS-Block 0x%02X: crect (%d,%d) prect (%d,%d), CLUT+Bild = RE2-Block %d", bl[i].id, cx, cy, px, py,
                  bl[i].re2);
        }
        re15_itps_set_data(itps, t15);
        int sicht = 0; uint8_t rr, gg, bb;
        for (int v = 0; v < RE15_ITPS_H; v++) for (int u = 0; u < RE15_ITPS_W; u++) sicht += re15_itps_pixel(0x45, u, v, &rr, &gg, &bb);
        CHECK(sicht > 4000, "B Modal-Leser re15_itps_pixel(0x45): %d von %d Bildpunkten gezeichnet", sicht, RE15_ITPS_W * RE15_ITPS_H);
    } else { printf("FAIL: ITPS.ITP nicht lesbar\n"); fails++; }
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int alle = strcmp(teil, "alle") == 0;
    if (alle || !strcmp(teil, "raum"))  teil_raum();
    if (alle || !strcmp(teil, "tuer"))  teil_tuer();
    if (alle || !strcmp(teil, "zelle")) teil_zelle();
    if (alle || !strcmp(teil, "namen")) teil_namen();
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
