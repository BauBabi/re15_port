/* test_r36_kartenitems.c — Runde 36 Spur N: die fuenf Polizei-Lageplaene (RE2 karte01) als
 * aufsammelbare Etagenkarte. Dossier: analysis/befunde_runde36/N_kartenitems.md.
 *
 * Jeder Teil misst die MECHANIK am echten Port-Code (echte RDT, scd_room_reenter mit allen
 * Port-Installern, echter Bild-Ablauf re15_game_step -> Aktions-Scan -> scd_event_fire ->
 * Port-Programm -> Ja/Nein-FSM):
 *   T  Tabelle: 5 Zeilen, Blatt = Blatt der Raumzone (re15_map_zones.h), Bits 87..91 (VERTRAG
 *      1.1), MD1/TIM parsen, die 5 MD1 unterscheiden sich NUR in den Ecken (@0x44, 40 Byte),
 *      Texte = RE2-Wortlaut (room2130 sub00/sub12-Rahmen), Frage hat die Auswahl (0x03).
 *   R  Raumstart Leon UND Elza (xxx0/xxx1): Prop obj 15 an Lage/Drehung der Tabelle, EVENT-Satz
 *      Ereignis 28 (sce 3, sat 0x31) im Slot; ROOM1030/1031: Muenztelefon-Satz in Slot 39.
 *   G  Bit gesetzt -> kein Prop, keine Zone, ROOM1030 Slot 8 bleibt das Telefon.
 *   D  echter Druck vor der Karte (1140 Leon, 10D1 Elza): Frage 30 -> Ja -> Bit (9,87/88),
 *      Zone weg (Aot_reset), Modell bei (20000,20000,20000) (RE2 Pos_set @0x037F6), Meldung 31;
 *      Nein -> nichts genommen, Zone bleibt.
 *   K  Karte: ohne Bit KEINE unbesuchte Kachel (RE2 @0x8006E744), mit Bit Kacheln in Zeile 498
 *      (@0x8006E71C); Blatt erst mit Bit waehlbar (RE2 Bank 35 @0x8006D934, Set(35,n) @0x037E6).
 *   S  Spielstand: die Bits 87..91 reisen mit g_game.flags (re15_savedata_capture/restore) — kein
 *      neues Feld, alte Staende laden unveraendert (Bits 0 = keine Karte).
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
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_msg.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_inv_screen.h"
#include "re15_map_owned.h"
#include "re15_map_hint.h"
#include "re15_kartenitems_r36.h"
#include "re15_savedata.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern unsigned         g_current_room_id;
extern void re15_map_zone_update(unsigned room, int x, int z);
extern void re15_map_visited_reset(void);

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; uint16_t id; } raum_t;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int raum_laden(raum_t *r, uint16_t id)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(id >> 12), (unsigned)id);
    memset(r, 0, sizeof *r);
    r->id = id;
    r->buf = slurp(p, &r->n);
    if (!r->buf) { printf("  FAIL: %s fehlt\n", p); g_fail = 1; return -1; }
    if (re15_rdt_parse(r->buf, r->n, &r->rdt) < 0) { printf("  FAIL: parse %s\n", p); g_fail = 1; return -1; }
    return 0;
}

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern, scd_room_setup.c). */
static void hochfahren(raum_t *r, int32_t px, int32_t pz, int bit_gesetzt)
{
    re15_game_state_init();
    scd_vm_init();
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = r->id; g_room_change.pending = 0;
    if (bit_gesetzt >= 0) re15_game_flag_set(RE15_R36K_BANK, (uint8_t)bit_gesetzt, 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 0;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

static int prop_slot(uint8_t oid)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_id == oid) return k;
    return -1;
}

/* ---------------------------------------------------------------- T */
static void teil_t(void)
{
    printf("=== T: Tabelle, Modelle, Texte ===\n");
    static const uint16_t raeume[5] = { 0x1140, 0x10D0, 0x1030, 0x1200, 0x11D0 };
    static const uint8_t  bits[5]   = { 87, 88, 89, 90, 91 };   /* VERTRAG.md 1.1 Spur N */
    static const char    *etage[5]  = { "3F", "2F", "1F", "B2", "B1" };
    CHECK("5 Zeilen", re15_kartenitems_r36_anzahl() == 5);
    for (int i = 0; i < 5 && i < re15_kartenitems_r36_anzahl(); i++) {
        const re15_kartenitem_r36_t *z = re15_kartenitems_r36_zeile(i);
        char nm[160];
        snprintf(nm, sizeof nm, "Zeile %d: ROOM%04X Bit %d", i, raeume[i], bits[i]);
        CHECK(nm, z && z->raum == raeume[i] && z->bit == bits[i]);
        /* Blatt = Blatt der Raumzone (re15_map_zones.h) */
        int blatt = -1;
        for (int k = 0; k < re15_map_zone_count(); k++) {
            const re15_map_zone_t *zn = re15_map_zone_by_index(k);
            if (zn && zn->room == raeume[i]) { blatt = (int)zn->page; break; }
        }
        snprintf(nm, sizeof nm, "ROOM%04X: Blatt %d == Zonen-Blatt %d", raeume[i], z ? z->blatt : -1, blatt);
        CHECK(nm, z && blatt >= 0 && z->blatt == (uint8_t)blatt);
        /* Elza-Variante findet dieselbe Zeile */
        CHECK("Elza-Variante (raum|1) -> dieselbe Zeile", re15_kartenitems_r36_des_raums((uint16_t)(raeume[i] | 1)) == z);
        /* Modell + Textur parsen */
        int msz = 0, tsz = 0;
        const uint8_t *mb = re15_kartenitems_r36_md1_bytes(raeume[i], &msz);
        const uint8_t *tb = re15_kartenitems_r36_tim_bytes(raeume[i], &tsz);
        re15_md1_t md1; re15_tim_t tim;
        CHECK("MD1 parst", mb && re15_md1_parse(mb, (size_t)msz, &md1) == 0 && md1.mesh_count == 1);
        CHECK("TIM parst (RE2 karte01, 16928 B)", tb && tsz == 16928 && re15_tim_parse(tb, tsz, &tim) == 0);
        if (i > 0) {
            const uint8_t *m0 = re15_kartenitems_r36_md1_bytes(raeume[0], NULL);
            int diff_aussen = 0;
            for (int b = 0; b < msz; b++)
                if ((b < 0x44 || b >= 0x44 + 40) && mb[b] != m0[b]) diff_aussen++;
            CHECK("MD1 unterscheidet sich nur in den 5 Ecken @0x44 (UV/Normalen/Topologie RE2)", diff_aussen == 0);
        }
        /* Texte */
        int fl = 0, gl = 0;
        const uint8_t *f = re15_kartenitems_r36_meldung(raeume[i], 0, &fl);
        const uint8_t *g = re15_kartenitems_r36_meldung(raeume[i], 1, &gl);
        char tf[200] = "", tg[200] = "", soll_f[200], soll_g[200];
        if (f) re15_msg_decode_text(f, (size_t)fl, tf, sizeof tf);
        if (g) re15_msg_decode_text(g, (size_t)gl, tg, sizeof tg);
        snprintf(soll_f, sizeof soll_f, "A police %s map.", etage[i]);
        snprintf(soll_g, sizeof soll_g, "police %s map.", etage[i]);
        printf("    Frage \"%s\" | genommen \"%s\"\n", tf, tg);
        CHECK("Frage beginnt mit 'A police XX map.' (RE2 room2130 sub00)", strncmp(tf, soll_f, strlen(soll_f)) == 0);
        CHECK("Frage endet mit 'Will you take it?'", strstr(tf, "Will you take it?") != NULL);
        CHECK("Meldung 'You've taken the ... police XX map.' (RE2 room2130 sub12)",
              strncmp(tg, "You've taken the", 16) == 0 && strstr(tg, soll_g) != NULL);
        re15_msg_install_text(RE15_R36K_MSG_FRAGE, f, (size_t)fl);
        re15_msg_install_text(RE15_R36K_MSG_GENOMMEN, g, (size_t)gl);
        CHECK("Frage ist eine Ja/Nein-Auswahl (0x03), Meldung nicht",
              re15_msg_is_choice(RE15_R36K_MSG_FRAGE) && !re15_msg_is_choice(RE15_R36K_MSG_GENOMMEN));
        CHECK("Dauer berechenbar", re15_msg_compute_duration(g, (size_t)gl, 0) > 0);
    }
    /* Programm: Slot-/Bit-Byte je Raum eingesetzt, Laengen wie ROOM5030 sub02 */
    int pl = 0;
    const uint8_t *p = re15_kartenitems_r36_programm(0x1030, &pl);
    CHECK("Programm 56 Byte, Aot_reset auf Slot 8 (1030), Set(9,89,1)",
          p && pl == 56 && p[0x0E] == 0x46 && p[0x0F] == 8 && p[0x1C] == 0x22 && p[0x1D] == 9 && p[0x1E] == 89);
    CHECK("Ifel_ck-Blockende = hinter dem Endif (+0x0A + 0x2C = +0x36)",
          p && p[0x06] == 0x06 && p[0x08] == 0x2C && p[0x34] == 0x08 && p[0x36] == 0x01);
}

/* ---------------------------------------------------------------- R + G */
static void teil_rg(raum_t *r)
{
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(r->id);
    char nm[200];
    printf("=== R/G: ROOM%04X ===\n", (unsigned)r->id);
    /* Standort egal (Installer haengt nicht am Spieler); Raummitte der Karte */
    hochfahren(r, z->rect_x + RE15_R36K_RECT / 2, z->rect_z + RE15_R36K_RECT / 2, -1);
    int ps = prop_slot(RE15_R36K_OBJ);
    snprintf(nm, sizeof nm, "Prop obj %d angelegt", RE15_R36K_OBJ);
    CHECK(nm, ps >= 0);
    if (ps >= 0) {
#define PP g_scd.props[ps]
        CHECK("Prop-Lage = Tabelle", PP.x == z->x && PP.y == z->y && PP.z == z->z);
        CHECK("Prop-Drehung = Tabelle", PP.rot_x == z->rot_x && PP.rot_y == z->rot_y && PP.rot_z == z->rot_z);
        CHECK("Prop-Flags 0x000B, Weltraum", PP.flags == RE15_R36K_PROP_FLAGS && PP.parent_obj == -1);
#undef PP
    }
    const re15_aot_t *a = &g_aot.slots[z->aot_slot];
    snprintf(nm, sizeof nm, "Slot %d = EVENT Ereignis 28, sat 0x31, Rechteck 1000x1000", z->aot_slot);
    CHECK(nm, a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == RE15_R36K_EREIGNIS &&
              (a->sce_flags & 0x7F) == RE15_R36K_AOT_SAT && a->half_w == 500 && a->half_h == 500 &&
              a->x == z->rect_x + 500 && a->z == z->rect_z + 500);
    if (z->verdraengt_nach < RE15_AOT_MAX) {
        const re15_aot_t *t = &g_aot.slots[z->verdraengt_nach];
        snprintf(nm, sizeof nm, "Muenztelefon (Aot_set Slot 8, 4096x4096) in Slot %d umgezogen", z->verdraengt_nach);
        CHECK(nm, t->active && t->type == RE15_AOT_TYPE_MESSAGE && t->half_w == 2048 && t->half_h == 2048);
    }
    /* zweiter Installer-Lauf (Boot-Weg nach Tuer-Weg) aendert nichts */
    int pc0 = (int)g_scd.prop_count;
    re15_kartenitems_r36_install(r->id);
    CHECK("Installer idempotent (kein zweites Prop, Telefon nicht ueberschrieben)",
          (int)g_scd.prop_count == pc0 &&
          (z->verdraengt_nach >= RE15_AOT_MAX || g_aot.slots[z->verdraengt_nach].type == RE15_AOT_TYPE_MESSAGE));

    /* G: genommen */
    hochfahren(r, z->rect_x + RE15_R36K_RECT / 2, z->rect_z + RE15_R36K_RECT / 2, z->bit);
    CHECK("Bit gesetzt -> kein Prop", prop_slot(RE15_R36K_OBJ) < 0);
    CHECK("Bit gesetzt -> keine Karten-Zone",
          !(g_aot.slots[z->aot_slot].active && g_aot.slots[z->aot_slot].type == RE15_AOT_TYPE_GENERIC &&
            g_aot.slots[z->aot_slot].event_id == RE15_R36K_EREIGNIS));
    if (z->verdraengt_nach < RE15_AOT_MAX)
        CHECK("Bit gesetzt -> Slot 8 bleibt das Muenztelefon, Slot 39 leer",
              g_aot.slots[z->aot_slot].type == RE15_AOT_TYPE_MESSAGE && !g_aot.slots[z->verdraengt_nach].active);
    CHECK("Bit gesetzt -> Ereignis 28 liefert kein Programm", re15_kartenitems_r36_ereignis(r->id, 28) == NULL);
}

/* ---------------------------------------------------------------- D (echter Bild-Ablauf) */
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_cine_was = 0;

static void bild(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    {   /* Cine/Letterbox wie test_item_stack_press.c / main.c */
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) { g_scd.letterbox_countdown = 15; }
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    re15_cam_present_tick();
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* Bilder ohne Eingabe, bis Bedingung oder Grenze. */
static int warte_auf_auswahl(int grenze)
{
    for (int f = 0; f < grenze; f++) {
        if (g_scd.message_active && g_scd.message_fsm == 3) return 1;
        bild(0, 0);
    }
    return g_scd.message_active && g_scd.message_fsm == 3;
}

static void teil_d(raum_t *r, int ja, int32_t px, int32_t pz)
{
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(r->id);
    char nm[200];
    printf("=== D: ROOM%04X echter Druck, Antwort %s ===\n", (unsigned)r->id, ja ? "JA" : "NEIN");
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &r->rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_cine_was = 0;
    hochfahren(r, px, pz, -1);
    g_scd.cut_auto_enabled = 1;
    for (int f = 0; f < 30; f++) bild(0, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    {   /* zur Zonenmitte drehen (Pruefpunkt 620 voraus @0x80042bd0, sat 0x31 = vorne) */
        int32_t dx = g_aot.slots[z->aot_slot].x - pl->x, dz = g_aot.slots[z->aot_slot].z - pl->z;
        double ang = atan2((double)(-dz), (double)dx) * 4096.0 / 6.283185307179586;
        pl->rot_y = (int16_t)(((int)(ang + (ang >= 0 ? 0.5 : -0.5))) & 0x0FFF);
    }
    printf("    Spieler (%ld,%ld) rot %d, player_mode %d\n", (long)pl->x, (long)pl->z, (int)pl->rot_y,
           (int)g_scd.player_mode);
    int ps = prop_slot(RE15_R36K_OBJ);
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);   /* Aktionsdruck -> Scan -> Ereignis 28 */
    bild(0, 0);
    int frage = warte_auf_auswahl(400);
    snprintf(nm, sizeof nm, "Druck oeffnet Frage %d mit Ja/Nein-Auswahl", RE15_R36K_MSG_FRAGE);
    CHECK(nm, frage && g_scd.message_id == RE15_R36K_MSG_FRAGE);
    if (!ja) { bild(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT); bild(0, 0); }   /* auf NEIN */
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);   /* bestaetigen */
    int meldung = 0;
    for (int f = 0; f < 120; f++) {
        bild(0, 0);
        if (g_scd.message_active && g_scd.message_id == RE15_R36K_MSG_GENOMMEN) meldung = 1;
    }
    if (ja) {
        snprintf(nm, sizeof nm, "JA -> Bit (9,%d) gesetzt", z->bit);
        CHECK(nm, re15_game_flag_get(RE15_R36K_BANK, z->bit));
        CHECK("JA -> Zone weg (Aot_reset Slot, sce 0)",
              !(g_aot.slots[z->aot_slot].active && g_aot.slots[z->aot_slot].type == RE15_AOT_TYPE_GENERIC));
        CHECK("JA -> Modell bei (20000,20000,20000) (RE2 Pos_set @0x037F6)",
              ps >= 0 && g_scd.props[ps].x == 20000 && g_scd.props[ps].y == 20000 && g_scd.props[ps].z == 20000);
        snprintf(nm, sizeof nm, "JA -> Meldung %d \"You've taken the ...\" erscheint", RE15_R36K_MSG_GENOMMEN);
        CHECK(nm, meldung);
        snprintf(nm, sizeof nm, "JA -> Blatt %d im Besitz und waehlbar", z->blatt);
        CHECK(nm, re15_map_owned_page(z->blatt) && re15_map_blatt_waehlbar(z->blatt));
        if (z->verdraengt_nach < RE15_AOT_MAX) {
            /* ROOM1030/1031: nach der Aufnahme antwortet an derselben Stelle wieder das Telefon (Slot 39).
             * Erst die Meldung 31 wegdruecken (Ende-Code 01 00 = auf Taste warten). */
            for (int f = 0; f < 200 && g_scd.message_active; f++) bild(f % 20 == 0 ? RE15_PAD_BIT_SQUARE : 0,
                                                                      f % 20 == 0 ? RE15_PAD_BIT_SQUARE : 0);
            for (int f = 0; f < 30; f++) bild(0, 0);
            CHECK("Meldung 31 weggedrueckt", !g_scd.message_active);
            CHECK("Telefon-Satz steht nach der Aufnahme weiter in Slot 39",
                  g_aot.slots[z->verdraengt_nach].active && g_aot.slots[z->verdraengt_nach].type == RE15_AOT_TYPE_MESSAGE);
            int tel = (int)g_aot.slots[z->verdraengt_nach].event_id, gesehen = 0;
            bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
            for (int f = 0; f < 60; f++) {
                if (g_scd.message_active && (int)g_scd.message_id == tel) gesehen = 1;
                bild(0, 0);
            }
            snprintf(nm, sizeof nm, "zweiter Druck an derselben Stelle -> Telefon-Text (Nachricht %d)", tel);
            CHECK(nm, gesehen);
        }
    } else {
        CHECK("NEIN -> Bit bleibt 0", !re15_game_flag_get(RE15_R36K_BANK, z->bit));
        CHECK("NEIN -> Zone bleibt scharf",
              g_aot.slots[z->aot_slot].active && g_aot.slots[z->aot_slot].type == RE15_AOT_TYPE_GENERIC);
        CHECK("NEIN -> Modell bleibt an der Wand", ps >= 0 && g_scd.props[ps].x == z->x);
        CHECK("NEIN -> keine Meldung 31", !meldung);
    }
}

/* ---------------------------------------------------------------- K */
static int ist_fest(const re15_inv_op_t *o)
{
    return (o->x == 0x1e  && o->y == 0x1e && o->w == 0x58 && o->h == 0x20) ||
           (o->x == 0x10e && o->y == 0x28 && o->w == 0x20 && o->h == 0x30);
}

static void kacheln(unsigned room, int x, int z, unsigned page, int *gesamt, int *unbesucht)
{
    static re15_inv_op_t ops[1024];
    *gesamt = 0; *unbesucht = 0;
    g_current_room_id = room;
    re15_map_zone_update(room, x, z);
    re15_inv_screen_open();
    g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
    g_inv_screen.map_page = (unsigned char)page;
    int nops = re15_inv_screen_build(&g_inv_screen, ops, 1024);
    for (int i = 0; i < nops; i++) {
        if (ops[i].kind != RE15_INV_OP_SPRT || ops[i].page != RE15_INV_PAGE_MAP4 || ist_fest(&ops[i])) continue;
        (*gesamt)++;
        if (ops[i].clut == RE15_INV_CLUT_MAP_UNBESUCHT) (*unbesucht)++;
    }
}

static void teil_k(void)
{
    printf("=== K: Kartenblatt je Etage (RE2 FUN_8006E120) ===\n");
    for (int i = 0; i < re15_kartenitems_r36_anzahl(); i++) {
        const re15_kartenitem_r36_t *z = re15_kartenitems_r36_zeile(i);
        int g0 = 0, u0 = 0, g1 = 0, u1 = 0, w0, w1;
        char nm[200];
        re15_game_state_init();
        re15_map_visited_reset();
        /* Stand im Kartenraum (eine Zone des Raums ist besucht), Bit fehlt */
        kacheln(z->raum, z->rect_x + 500, z->rect_z + 500, z->blatt, &g0, &u0);
        w0 = re15_map_blatt_waehlbar((unsigned)((z->blatt + 1) % 5));
        re15_game_flag_set(RE15_R36K_BANK, z->bit, 1);
        kacheln(z->raum, z->rect_x + 500, z->rect_z + 500, z->blatt, &g1, &u1);
        printf("    Blatt %d (ROOM%04X): ohne Plan %d Kacheln / %d unbesucht, mit Plan %d / %d\n",
               z->blatt, (unsigned)z->raum, g0, u0, g1, u1);
        snprintf(nm, sizeof nm, "Blatt %d ohne Plan: keine unbesuchte Kachel (RE2 @0x8006E744)", z->blatt);
        CHECK(nm, u0 == 0 && g0 > 0);
        snprintf(nm, sizeof nm, "Blatt %d mit Plan: unbesuchte Kacheln in Zeile 498 (RE2 @0x8006E71C)", z->blatt);
        CHECK(nm, u1 > 0 && g1 - g0 == u1);
        CHECK("Besitz-Bits enthalten das Blatt", (re15_map_owned_bits() >> z->blatt) & 1u);
        /* Etagenwahl: ein fremdes, nie betretenes Blatt wird mit SEINEM Plan waehlbar */
        {
            unsigned fremd = (unsigned)((z->blatt + 1) % 5);
            const re15_kartenitem_r36_t *zf = NULL;
            for (int k = 0; k < re15_kartenitems_r36_anzahl(); k++)
                if (re15_kartenitems_r36_zeile(k)->blatt == fremd) zf = re15_kartenitems_r36_zeile(k);
            w1 = re15_map_blatt_waehlbar(fremd);
            if (zf) re15_game_flag_set(RE15_R36K_BANK, zf->bit, 1);
            snprintf(nm, sizeof nm, "fremdes Blatt %u: vorher %s, eigener Plan aendert nichts (%d), mit seinem Plan waehlbar",
                     fremd, w0 ? "waehlbar" : "gesperrt", w1);
            CHECK(nm, w1 == w0 && (!zf || re15_map_blatt_waehlbar(fremd)));
        }
    }
    re15_game_state_init();
    CHECK("frischer Stand: keine Besitz-Bits der Polizei-Blaetter", (re15_map_owned_bits() & 0x1Fu) == 0);
}

static void teil_s(void)
{
    printf("=== S: Spielstand-Rundlauf ===\n");
    re15_savedata_t sd, zurueck;
    uint16_t raum = 0;
    re15_game_state_init();
    g_current_room_id = 0x1140;
    re15_game_flag_set(RE15_R36K_BANK, 87, 1);
    re15_game_flag_set(RE15_R36K_BANK, 91, 1);
    re15_savedata_capture(&sd, 0, 1);
    zurueck = sd;
    re15_game_state_init();
    CHECK("nach dem Zuruecksetzen kein Besitz", (re15_map_owned_bits() & 0x1Fu) == 0);
    CHECK("Restore ok", re15_savedata_restore(&zurueck, &raum) == 0);
    CHECK("Bits 87/91 aus dem Spielstand -> Blatt 4 (3F) und 0 (B1) im Besitz, 1..3 nicht",
          re15_map_owned_page(4) && re15_map_owned_page(0) &&
          !re15_map_owned_page(1) && !re15_map_owned_page(2) && !re15_map_owned_page(3));
}

int main(void)
{
    printf("=== Runde 36 Spur N: Lageplaene zum Aufsammeln ===\n");
    teil_t();
    static const uint16_t ids[10] = { 0x1140, 0x1141, 0x10D0, 0x10D1, 0x1030, 0x1031,
                                      0x1200, 0x1201, 0x11D0, 0x11D1 };
    static raum_t raeume[10];
    for (int i = 0; i < 10; i++) {
        if (raum_laden(&raeume[i], ids[i]) != 0) continue;
        teil_rg(&raeume[i]);
    }
    /* D: Standorte aus der Abdeckungsmessung (probe_r34n_e_dokumente abdeckung, Dossier 3.4) */
    teil_d(&raeume[0], 1, -2050, -9900);     /* 1140 Leon, vor der Tafel                          */
    teil_d(&raeume[0], 0, -2050, -9900);     /* 1140 Leon, NEIN                                    */
    teil_d(&raeume[3], 1, 2130, 22700);      /* 10D1 Elza, vor dem Aushang                         */
    teil_d(&raeume[4], 1, -1690, -22900);    /* 1030 Leon, vor der Rueckwand (Slot 8, Telefon -> 39) */
    teil_k();
    teil_s();
    for (int i = 0; i < 10; i++) free(raeume[i].buf);
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
