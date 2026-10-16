/* test_r37_kistenregel.c — Runde 37 Spur Q: eine Benutzung verlangt den Gegenstand im INVENTAR (RE2 Keep_Item_ck
 * @0x800584F0 -> FUN_800696CC), die RE2-Item-Box des Ports zaehlt nicht. Dossier analysis/befunde_runde37/
 * Q_kistenregel.md, Regel include/re15_kistenregel_r37.h, Tabelle engine/src/kistenregel_r37.c.
 *
 * Jeder Teil misst die MECHANIK am echten Port-Code (echte RDT, scd_room_reenter mit allen Port-Installern, der
 * Ck-Opcode der SCD-VM, re15_aot_fire_slot = derselbe Pfad wie der Aktionsdruck, Nachrichten-FSM):
 *   tabelle   die 20 Zeilen gegen die ausgelieferten RDTs: am Datei-Offset steht `21 bank bit wert`, das "fertig"-Bit
 *             wird im Raum gesetzt (`22 bank bit 01`), 11 Faelle K1..K11; die Benutzungs-Stellen OHNE Inventar-
 *             Gegenstand (Crank 2020, Time Bomb 2040, Fuse 2060, Latch Key 3091) und fremde Ck-Stellen stehen NICHT drin
 *   raumstart je Fall der Platz, den das Raumskript beim Raumstart anlegt: Gegenstand in der KISTE -> Original-Zweig
 *             "nicht im Besitz" (Raumtext); im INVENTAR -> Benutzungs-Platz wie bisher; schon BENUTZT + Kiste -> der
 *             Zweig "fertig" wie im Original; nie genommen -> unveraendert "nicht im Besitz"
 *   ablauf    Druck auf den Platz (re15_aot_fire_slot) + Bild-Ablauf mit Ja auf jede Frage: welche Nachrichten
 *             kommen? 1090 Kiste -> msg 7 ohne Frage 8, Inventar -> msg 7 + Frage 8 + Loeschen ((3,0x81));
 *             Kartenleser Kiste -> msg 8 "You have not ...", Inventar -> msg 9 "You've used ..."; usw.
 *   spielstand Kiste + Bits reisen mit re15_savedata -> nach dem Laden dieselbe Entscheidung; Herausnehmen aus der
 *             Kiste (echter Tausch re15_itembox_transfer) -> Benutzung wieder moeglich
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_kistenregel_r37.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_item_modal.h"
#include "re15_inventory.h"
#include "re15_item_discard.h"
#include "re15_files.h"
#include "re15_msg.h"
#include "re15_gameflow.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_savedata.h"
#include "re15_itembox.h"

extern re15_aot_state_t g_aot;
void re15_actor_step_all_walkers(void);

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
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
    memset(r, 0, sizeof *r);
    r->buf = slurp(p, &n);
    if (!r->buf) { printf("FAIL: %s fehlt\n", p); fails++; return -1; }
    r->n = (size_t)n; r->id = id;
    if (re15_rdt_parse(r->buf, r->n, &r->rdt) != 0) { printf("FAIL: parse %s\n", p); fails++; return -1; }
    return 0;
}

static void raum_frei(raum_t *r) { free(r->buf); r->buf = NULL; }

/* Wo liegt der Gegenstand? */
enum { NIRGENDS = 0, INVENTAR = 1, KISTE = 2 };

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern; Muster test_r37_feuerloescher.c start).
 * Frisches Spiel: Flags/Inventar/Kiste leer, dann `flags` (Paare bank,bit; Ende <0), der Gegenstand `item` nach `wo`
 * (Kiste: Platz 2 = (Scroll 0 + 2) & 0x3f, RE2 FUN_800703b8 @0x8007042c/@0x80070440). */
static void start(raum_t *r, int32_t px, int32_t py, int32_t pz, const int *flags, uint8_t item, int wo, int elza)
{
    re15_game_state_init();
    re15_inv_init();
    re15_inv_set_equipped_slot(0x80);
    re15_itembox_init();
    re15_files_reset();
    re15_discard_reset();
    for (int k = 0; flags && flags[k] >= 0; k += 2)
        re15_game_flag_set((uint8_t)flags[k], (uint8_t)flags[k + 1], 1);
    if (wo == INVENTAR) re15_inv_grant(item, 1);
    if (wo == KISTE) { g_itembox.slots[2].id = item; g_itembox.slots[2].qty = 1; g_itembox.slots[2].flags = 0; }

    re15_game_state_t flags_vorher = g_game;
    re15_inventory_t  inv_vorher   = g_inv;
    re15_aot_init();
    scd_vm_init(); re15_actor_init();        /* scd_vm_init leert auch das Inventar (scd_vm.c) */
    g_game = flags_vorher; g_inv = inv_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_gameflow.character = elza ? 4 : 0;
    g_current_room_id = r->id;
    g_room_rdt = r->rdt; g_room_rdt_ok = 1;
    memset(&g_room_change, 0, sizeof g_room_change);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = 0;
    re15_collision_set_band(re15_collision_band_from_y(py));
    g_scd.player_mode = 0;
    re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_register_room_events(&r->rdt);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

/* ---- Bild-Ablauf wie platform/pc/main.c (Muster test_r37_feuerloescher.c bild) ---------------------------- */
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_cine_was = 0;

static uint16_t virt(uint16_t raw) { return (raw & RE15_PAD_BIT_SQUARE) ? 0x4000u : 0u; }

static void ctx_neu(raum_t *r)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &r->rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_shown = 0; s_cine_was = 0;
    g_scd.cut_auto_enabled = 1;
}

static void bild(uint16_t held, uint16_t edge)
{
    if (!re15_discard_frozen()) {
        const unsigned char *raw; int len, id;
        if (!re15_item_modal_active()) scd_vm_tick();
        re15_actor_step_all_walkers();
        {
            int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
            re15_letterbox_tick(re15_game_flag_get(1, 27));
            if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
            else if (s_cine_was) g_scd.letterbox_countdown = 15;
            s_cine_was = cine;
            if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
        }
        re15_msg_tick(&raw, &len, &id);
        if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
        s_ctx.active_cut  = s_shown;
        s_ctx.pad_current = held;
        s_ctx.pad_pressed = edge;
        re15_game_step(&s_ctx);
        if (re15_item_modal_active()) re15_item_modal_tick(virt(edge), virt(held));
        { scd_audio_event_t e; while (scd_audio_queue_pop(&e)) { } }
    }
    if (re15_discard_active()) re15_discard_tick(0x8000u, 0);   /* Wegwerf-Abfrage: NEIN (nicht Thema hier) */
}

/* Druck auf den Platz `slot` (re15_aot_fire_slot = der Pfad des Aktionsdrucks / Aot_on, aot_common.c), dann
 * `n` Bilder; QUADRAT alle 6 Bilder, solange eine Nachricht offen ist (= weiter / Ja auf jede Frage, Cursor steht
 * auf Ja). Liefert die Folge der aufgegangenen Nachrichten-Ids in `ids` (Ende -1). */
static int s_ids[64], s_ids_n = 0;
static void druck_und_ablauf(raum_t *r, int slot, int n)
{
    ctx_neu(r);
    s_ids_n = 0;
    int vorher_aktiv = 0, vorher_id = -1;
    for (int f = 0; f < 20; f++) bild(0, 0);
    re15_aot_fire_slot(slot);
    for (int f = 0; f < n; f++) {
        uint16_t e = (g_scd.message_active && (f % 6) == 5) ? RE15_PAD_BIT_SQUARE : 0;
        bild(e, e);
        int a = (int)g_scd.message_active, id = (int)g_scd.message_id;
        if (a && (!vorher_aktiv || id != vorher_id) && s_ids_n < 63) s_ids[s_ids_n++] = id;
        vorher_aktiv = a; vorher_id = id;
    }
    s_ids[s_ids_n] = -1;
}

static int gesehen(int id)
{
    for (int i = 0; i < s_ids_n; i++) if (s_ids[i] == id) return 1;
    return 0;
}

static const char *folge(void)
{
    static char b[256];
    b[0] = 0;
    for (int i = 0; i < s_ids_n && strlen(b) < 200; i++) {
        char t[16]; snprintf(t, sizeof t, "%s%d", i ? "," : "", s_ids[i]);
        strcat(b, t);
    }
    return s_ids_n ? b : "-";
}

static int slot_ist(int s, int typ, int ev)
{
    return g_aot.slots[s].active && g_aot.slots[s].type == typ && (ev < 0 || g_aot.slots[s].event_id == ev);
}

/* ===================================================================================================== tabelle */
static int bytes_da(const raum_t *r, uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    for (size_t i = 0; i + 4 <= r->n; i++)
        if (r->buf[i] == a && r->buf[i + 1] == b && r->buf[i + 2] == c && r->buf[i + 3] == d) return 1;
    return 0;
}

static void teil_tabelle(void)
{
    printf("\n[tabelle]\n");
    int n = re15_kistenregel_r37_anzahl();
    CHECK(n == 20, "20 Ck-Stellen (Dossier 3.1), ist %d", n);
    int faelle[12] = { 0 };
    for (int i = 0; i < n; i++) {
        const re15_kr37_fall_t *f = re15_kistenregel_r37_zeile(i);
        raum_t r;
        if (raum_laden(&r, f->raum)) continue;
        int ok = f->ck_off + 4 <= r.n && r.buf[f->ck_off] == 0x21 && r.buf[f->ck_off + 1] == f->bank &&
                 r.buf[f->ck_off + 2] == f->bit && r.buf[f->ck_off + 3] == f->wert;
        CHECK(ok, "K%u ROOM%04X @0x%05X = `21 %02x %02x %02x` (Ck(%u,0x%02X)==%u, Item 0x%02X)", f->fall, f->raum,
              f->ck_off, f->bank, f->bit, f->wert, f->bank, f->bit, f->wert, f->item);
        CHECK(bytes_da(&r, 0x22, f->fertig_bank, f->fertig_bit, 0x01),
              "K%u ROOM%04X setzt das fertig-Bit selbst: `22 %02x %02x 01` im RDT", f->fall, f->raum,
              f->fertig_bank, f->fertig_bit);
        if (f->fall >= 1 && f->fall <= 11) faelle[f->fall]++;
        raum_frei(&r);
    }
    int alle = 1;
    for (int k = 1; k <= 11; k++) if (!faelle[k]) alle = 0;
    CHECK(alle, "alle elf Faelle K1..K11 in der Tabelle");
    /* Keine Zeile fuer reine Skript-Gegenstaende (Dossier 3.2) und fremde Ck-Stellen desselben Bits (3.3). */
    static const struct { uint16_t raum; uint16_t off; } k_nicht[] = {
        { 0x2020, 0x00954 }, { 0x2021, 0x00954 },   /* Crank (3,0x72)      */
        { 0x2040, 0x0141A }, { 0x2041, 0x0141A },   /* Time Bomb (3,0x92)  */
        { 0x2060, 0x010A2 }, { 0x2061, 0x010A2 },   /* Fuse (3,0x6C)       */
        { 0x3091, 0x01596 },                        /* Latch Key (9,0x15)  */
        { 0x1000, 0x00D08 }, { 0x1000, 0x00D00 },   /* Ableitung (3,0x85) / (9,0x86) — Aufnahme-Folge */
        { 0x3070, 0x03408 },                        /* (4,0xEC) in Leons STAGE3-Szene */
        { 0x4010, 0x006E8 },                        /* Ableitung (3,0x65) im Fundraum */
    };
    for (size_t i = 0; i < sizeof k_nicht / sizeof k_nicht[0]; i++) {
        int drin = 0;
        for (int j = 0; j < n; j++) {
            const re15_kr37_fall_t *f = re15_kistenregel_r37_zeile(j);
            if (f->raum == k_nicht[i].raum && f->ck_off == k_nicht[i].off) drin = 1;
        }
        CHECK(!drin, "keine Zeile fuer ROOM%04X @0x%05X (Dossier 3.2/3.3)", k_nicht[i].raum, k_nicht[i].off);
    }
    /* Die Regel ausserhalb der Tabelle: Wert unveraendert, auch bei gleichem Bit in fremdem Raum. */
    re15_inv_init(); re15_itembox_init();
    CHECK(re15_kistenregel_r37_ck(0x1000, 0x00D08, 3, 0x85, 1) == 1 &&
          re15_kistenregel_r37_ck(0x1090, 0x0230B, 3, 0x85, 1) == 1 &&
          re15_kistenregel_r37_ck(0x1091, 0x0230A, 3, 0x85, 1) == 1,
          "ausserhalb der Tabelle (anderer Raum / anderer Offset): Bit unveraendert");
}

/* ===================================================================================================== raumstart */
typedef struct {
    const char *name;
    uint16_t raum; int elza; int32_t x, y, z;
    const int *flags;          /* Besitz (T/A-Bits) — Ende -1 */
    uint8_t item;
    uint8_t fbank, fbit;       /* fertig */
    int slot;                  /* der Platz an der Benutzungs-Stelle */
    int ohne_typ, ohne_ev;     /* Raumstart, Gegenstand fehlt: Original-Zweig "nicht im Besitz" */
    int mit_typ, mit_ev;       /* Raumstart, Gegenstand im Inventar: Benutzungs-Platz */
    int fertig_typ, fertig_ev; /* schon benutzt (Gegenstand weg/Kiste): Zweig "fertig" (-1 typ = Platz leer/fremd) */
} rs_fall_t;

#define T_MSG RE15_AOT_TYPE_MESSAGE
#define T_GEN RE15_AOT_TYPE_GENERIC
#define T_EXW RE15_AOT_TYPE_EXAMINE_WORKVAR
#define T_DOOR RE15_AOT_TYPE_DOOR

static const int F_K1[]  = { 9, 0x86, 3, 0x85, 3, 0x80, -1 };
static const int F_K5[]  = { 9, 0x0A, 3, 0x7F, -1 };
static const int F_K6[]  = { 9, 0x68, 3, 0x67, -1 };
static const int F_K7[]  = { 9, 0x55, 3, 0x3B, -1 };
static const int F_K8[]  = { 9, 0xB4, 3, 0xB2, -1 };
static const int F_K9[]  = { 9, 0x06, 3, 0x65, -1 };
static const int F_K10[] = { 9, 0x56, 3, 0x57, -1 };
static const int F_K11[] = { 4, 0xEC, -1 };

static const rs_fall_t k_rs[] = {
    /* name           raum   elza  x       y      z      flags  item  fertig     slot ohne        mit          fertig */
    { "K1 1090",      0x1090, 0,      0, -1800,    250, F_K1,  0x31, 3, 0x81,  2, T_MSG, 7,  T_GEN, 6,  -1, 0 },
    { "K5 11E0",      0x11E0, 0, -12450,     0,  -8200, F_K5,  0x30, 4, 0x24,  6, T_MSG, 10, T_GEN, 26, -2, 0 },
    { "K5 11E1",      0x11E1, 1, -12450,     0,  -8200, F_K5,  0x30, 4, 0x24,  6, T_MSG, 10, T_GEN, 26, -2, 0 },
    { "K6 1100",      0x1100, 0, -25450,     0,  -9000, F_K6,  0x44, 4, 0xE8,  2, T_MSG, 1,  T_GEN, 7,  T_MSG, 5 },
    { "K6 1101",      0x1101, 1, -25450,     0,  -9000, F_K6,  0x44, 4, 0xE8,  2, T_MSG, 1,  T_GEN, 7,  T_MSG, 5 },
    { "K7 3010",      0x3010, 0, -26750, -1800, -19800, F_K7,  0x36, 3, 0x3C,  6, T_MSG, 0,  T_GEN, 2,  T_DOOR, -1 },
    { "K7 3011",      0x3011, 1, -26750, -1800, -19800, F_K7,  0x36, 3, 0x3C,  6, T_MSG, 0,  T_GEN, 2,  T_DOOR, -1 },
    { "K8 3050",      0x3050, 0, -25400,     0, -15900, F_K8,  0x46, 3, 0xB3, 10, T_MSG, 3,  T_GEN, 10, T_MSG, 7 },
    { "K8 3051",      0x3051, 1, -25400,     0, -15900, F_K8,  0x46, 3, 0xB3, 10, T_MSG, 3,  T_GEN, 10, T_MSG, 7 },
    { "K9 4000",      0x4000, 0, -14100,     0, -19900, F_K9,  0x47, 3, 0x20,  6, T_MSG, 0,  -1, 0,     -1, 0 },
    { "K10 4030",     0x4030, 0, -22960,     0, -22960, F_K10, 0x41, 4, 0x19,  6, T_MSG, 0,  -1, 0,     -1, 0 },
    { "K10 4031",     0x4031, 1, -22960,     0, -22960, F_K10, 0x41, 4, 0x19,  6, T_MSG, 0,  -1, 0,     -1, 0 },
    { "K11 1211",     0x1211, 1, -21500,     0, -21600, F_K11, 0x42, 4, 0xFC,  8, T_MSG, 10, -1, 0,     -1, 0 },
};

static const char *typname(int s)
{
    if (!g_aot.slots[s].active) return "leer";
    switch (g_aot.slots[s].type) {
    case T_MSG: return "MESSAGE"; case T_GEN: return "EVENT"; case T_EXW: return "MARKER(sce5)";
    case T_DOOR: return "DOOR"; case RE15_AOT_TYPE_AUTO_EVENT: return "AUTO"; case RE15_AOT_TYPE_NONE: return "NONE";
    default: return "?";
    }
}

static int platz_wie(int slot, int typ, int ev)
{
    if (typ == -1) return !(g_aot.slots[slot].active && g_aot.slots[slot].type == T_MSG);   /* kein Raumtext */
    if (typ == -2) return !(g_aot.slots[slot].active &&
                            (g_aot.slots[slot].type == T_MSG || g_aot.slots[slot].type == T_GEN));
    return slot_ist(slot, typ, ev);
}

static void teil_raumstart(void)
{
    printf("\n[raumstart]\n");
    for (size_t i = 0; i < sizeof k_rs / sizeof k_rs[0]; i++) {
        const rs_fall_t *c = &k_rs[i];
        raum_t r;
        if (raum_laden(&r, c->raum)) continue;
        /* (1) Kiste: Original-Zweig "nicht im Besitz" mit dem Raumtext */
        int z0 = g_re15_kr37_gesperrt_zaehler;
        start(&r, c->x, c->y, c->z, c->flags, c->item, KISTE, c->elza);
        CHECK(slot_ist(c->slot, c->ohne_typ, c->ohne_ev) && g_re15_kr37_gesperrt_zaehler > z0,
              "%s KISTE: Platz %d = %s ev %d (soll Raumtext msg %d), Regel griff %d x", c->name, c->slot,
              typname(c->slot), g_aot.slots[c->slot].event_id, c->ohne_ev, g_re15_kr37_gesperrt_zaehler - z0);
        /* (2) Inventar: Benutzungs-Platz wie das Original mit Bit */
        start(&r, c->x, c->y, c->z, c->flags, c->item, INVENTAR, c->elza);
        int mit_ok = (c->mit_typ >= 0) ? slot_ist(c->slot, c->mit_typ, c->mit_ev)
                                       : !(g_aot.slots[c->slot].active && g_aot.slots[c->slot].type == T_MSG);
        CHECK(mit_ok, "%s INVENTAR: Platz %d = %s ev %d (Benutzung wie das Original)", c->name, c->slot,
              typname(c->slot), g_aot.slots[c->slot].event_id);
        /* (3) schon benutzt, Gegenstand in der Kiste: Zweig "fertig" wie im Original (RE2 ROOM6150 @0x02D3C) */
        {
            int fl[16]; int k = 0;
            for (; c->flags[k] >= 0 && k < 12; k++) fl[k] = c->flags[k];
            fl[k++] = c->fbank; fl[k++] = c->fbit; fl[k] = -1;
            start(&r, c->x, c->y, c->z, fl, c->item, KISTE, c->elza);
            int ok3 = platz_wie(c->slot, c->fertig_typ, c->fertig_ev);
            CHECK(ok3, "%s FERTIG ((%u,0x%02X)=1) + KISTE: Platz %d = %s ev %d (Zweig 'fertig' wie im Original)",
                  c->name, c->fbank, c->fbit, c->slot, typname(c->slot), g_aot.slots[c->slot].event_id);
        }
        /* (4) nie genommen (Bit 0): unveraendert "nicht im Besitz" */
        start(&r, c->x, c->y, c->z, NULL, c->item, NIRGENDS, c->elza);
        CHECK(slot_ist(c->slot, c->ohne_typ, c->ohne_ev), "%s NIE GENOMMEN: Platz %d = %s ev %d (Original)",
              c->name, c->slot, typname(c->slot), g_aot.slots[c->slot].event_id);
        raum_frei(&r);
    }
    /* K9/K10/K11: der Benutzungs-Weg selbst (Marker sce 5 / Auto-Ereignis) */
    {
        raum_t r;
        if (!raum_laden(&r, 0x4000)) {
            start(&r, -14100, 0, -19900, F_K9, 0x47, KISTE, 0);
            CHECK(!g_aot.slots[2].active || g_aot.slots[2].type == RE15_AOT_TYPE_NONE,
                  "K9 4000 KISTE: Marker Slot 2 weg (Aot_reset @0x01140), ist %s", typname(2));
            start(&r, -14100, 0, -19900, F_K9, 0x47, INVENTAR, 0);
            CHECK(slot_ist(2, T_EXW, -1), "K9 4000 INVENTAR: Marker Slot 2 (sce 5 @0x010BC), ist %s", typname(2));
            raum_frei(&r);
        }
        if (!raum_laden(&r, 0x4030)) {
            start(&r, -22960, 0, -22960, F_K10, 0x41, KISTE, 0);
            CHECK(!g_aot.slots[5].active, "K10 4030 KISTE: kein Marker Slot 5, ist %s", typname(5));
            start(&r, -22960, 0, -22960, F_K10, 0x41, INVENTAR, 0);
            CHECK(slot_ist(5, T_EXW, -1), "K10 4030 INVENTAR: Marker Slot 5 (sce 5 @0x00516), ist %s", typname(5));
            raum_frei(&r);
        }
        if (!raum_laden(&r, 0x1211)) {
            start(&r, -21500, 0, -21600, F_K11, 0x42, KISTE, 1);
            CHECK(!g_aot.slots[6].active, "K11 1211 KISTE: kein Befreiungs-Platz Slot 6, ist %s", typname(6));
            start(&r, -21500, 0, -21600, F_K11, 0x42, INVENTAR, 1);
            CHECK(g_aot.slots[6].active && g_aot.slots[6].event_id == 2,
                  "K11 1211 INVENTAR: Befreiungs-Platz Slot 6 -> sub02 (@0x01E22), ist %s ev %d", typname(6),
                  g_aot.slots[6].event_id);
            raum_frei(&r);
        }
    }
}

/* ===================================================================================================== ablauf */
typedef struct {
    const char *name;
    uint16_t raum; int elza; int32_t x, y, z;
    const int *flags; uint8_t item; int slot; int n;
    int kiste_soll, kiste_nicht;     /* Kiste: diese Nachricht kommt / diese nicht */
    int inv_soll1, inv_soll2;        /* Inventar: diese beiden kommen */
} ab_fall_t;

static const int F_K2[] = { 9, 0x34, -1 };
static const int F_K3[] = { 9, 0x88, -1 };
static const int F_K4[] = { 9, 0x8A, -1 };

static const ab_fall_t k_ab[] = {
    /* name        raum   elza  x       y      z       flags item  slot  n    Kiste soll/nicht   Inventar soll */
    { "K1 1090",   0x1090, 0,      0, -1800,    250, F_K1, 0x31,  2, 600,  7,  8,               7,  8 },
    { "K2 10D0",   0x10D0, 0,    800,     0,  -5000, F_K2, 0x38,  1, 600,  8,  9,               7,  9 },
    { "K2 10D1",   0x10D1, 1,    800,     0,  -5000, F_K2, 0x38,  1, 600,  8,  9,               7,  9 },
    { "K3 1230",   0x1230, 0,  -1650,     0,  12300, F_K3, 0x37,  5, 600,  8,  9,               7,  9 },
    { "K3 1231",   0x1231, 1,  -1650,     0,  12300, F_K3, 0x37,  5, 600,  8,  9,               7,  9 },
    { "K4 11E1",   0x11E1, 1,  11050,     0,   4250, F_K4, 0x39,  4, 600,  8,  9,               7,  9 },
    { "K5 11E1",   0x11E1, 1, -12450,     0,  -8200, F_K5, 0x30,  6, 400, 10, 11,              10, 11 },
    { "K6 1100",   0x1100, 0, -25450,     0,  -9000, F_K6, 0x44,  2, 400,  1,  3,               1,  3 },
    { "K7 3010",   0x3010, 0, -26750, -1800, -19800, F_K7, 0x36,  6, 300,  0,  1,               1,  1 },
    { "K8 3050",   0x3050, 0, -25400,     0, -15900, F_K8, 0x46, 10, 400,  3,  4,               3,  4 },
    { "K11 1211",  0x1211, 1, -21500,     0, -21600, F_K11,0x42,  8, 300, 10, -1,              -1, -1 },
};

static void teil_ablauf(void)
{
    printf("\n[ablauf]\n");
    for (size_t i = 0; i < sizeof k_ab / sizeof k_ab[0]; i++) {
        const ab_fall_t *c = &k_ab[i];
        raum_t r;
        if (raum_laden(&r, c->raum)) continue;
        start(&r, c->x, c->y, c->z, c->flags, c->item, KISTE, c->elza);
        druck_und_ablauf(&r, c->slot, c->n);
        CHECK(gesehen(c->kiste_soll) && (c->kiste_nicht < 0 || !gesehen(c->kiste_nicht)),
              "%s KISTE: Druck auf Platz %d -> Nachrichten [%s]: msg %d ja, msg %d nein", c->name, c->slot, folge(),
              c->kiste_soll, c->kiste_nicht);
        if (c->inv_soll1 >= 0) {
            start(&r, c->x, c->y, c->z, c->flags, c->item, INVENTAR, c->elza);
            druck_und_ablauf(&r, c->slot, c->n);
            CHECK(gesehen(c->inv_soll1) && gesehen(c->inv_soll2),
                  "%s INVENTAR (Gegenprobe): Druck auf Platz %d -> Nachrichten [%s]: msg %d und msg %d wie bisher",
                  c->name, c->slot, folge(), c->inv_soll1, c->inv_soll2);
        }
        raum_frei(&r);
    }
    /* K1 Ende-zu-Ende: mit dem Loescher in der Kiste geht das Feuer NICHT aus; im Inventar schon (sub06 @0x0271E). */
    raum_t r;
    if (!raum_laden(&r, 0x1090)) {
        start(&r, 0, -1800, 250, F_K1, 0x31, KISTE, 0);
        druck_und_ablauf(&r, 2, 700);
        CHECK(!re15_game_flag_get(3, 0x81) && !re15_game_flag_get(3, 0x84),
              "K1 1090 KISTE: Feuer brennt weiter ((3,0x81)=%d, (3,0x84)=%d), keine Rettungsszene",
              re15_game_flag_get(3, 0x81), re15_game_flag_get(3, 0x84));
        start(&r, 0, -1800, 250, F_K1, 0x31, INVENTAR, 0);
        druck_und_ablauf(&r, 2, 700);
        CHECK(re15_game_flag_get(3, 0x81), "K1 1090 INVENTAR (Gegenprobe): geloescht ((3,0x81)=%d, sub06 @0x0271E)",
              re15_game_flag_get(3, 0x81));
        raum_frei(&r);
    }
}

/* ===================================================================================================== spielstand */
static void teil_spielstand(void)
{
    printf("\n[spielstand]\n");
    raum_t r;
    if (raum_laden(&r, 0x1090)) return;
    start(&r, 0, -1800, 250, F_K1, 0x31, KISTE, 0);
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    /* alles weg, dann laden */
    re15_game_state_init(); re15_inv_init(); re15_itembox_init();
    uint16_t rr = 0;
    int ok = re15_savedata_restore(&sd, &rr) == 0;
    int k = -1;
    for (int i = 0; i < RE15_BOX_SLOTS; i++) if (g_itembox.slots[i].id == 0x31) k = i;
    CHECK(ok && k == 2 && re15_inv_find_item(0x31) < 0 && re15_game_flag_get(3, 0x85),
          "Spielstand: (3,0x85) + Loescher in der Kiste (Platz %d) reisen mit re15_savedata", k);
    /* Raumstart mit dem geladenen Zustand (Flags/Inventar/Kiste bleiben) */
    {
        re15_game_state_t gs = g_game; re15_inventory_t iv = g_inv; re15_itembox_t bx = g_itembox;
        re15_aot_init(); scd_vm_init(); re15_actor_init();
        g_game = gs; g_inv = iv; g_itembox = bx;
        g_current_room_id = r.id; g_room_rdt = r.rdt; g_room_rdt_ok = 1;
        re15_msg_load_room_block(r.rdt.messages, r.rdt.messages_size);
        scd_register_room_events(&r.rdt);
        scd_room_reenter(&r.rdt, 0, 250, 0);
    }
    CHECK(slot_ist(2, T_MSG, 7), "nach dem Laden: Platz 2 = msg 7 (kein Loesch-Platz), ist %s ev %d", typname(2),
          g_aot.slots[2].event_id);
    /* Herausnehmen ueber den ECHTEN Tausch (RE2 FUN_800703b8 @0x800706c8 / @0x80070710) -> wieder im Inventar */
    int erg = re15_itembox_transfer(0, 2);
    CHECK(erg == RE15_BOX_XFER_OK && re15_inv_find_item(0x31) >= 0, "Herausnehmen: re15_itembox_transfer(0,2) -> "
          "Loescher im Inventar (Platz %d)", re15_inv_find_item(0x31));
    {
        re15_game_state_t gs = g_game; re15_inventory_t iv = g_inv; re15_itembox_t bx = g_itembox;
        re15_aot_init(); scd_vm_init(); re15_actor_init();
        g_game = gs; g_inv = iv; g_itembox = bx;
        g_current_room_id = r.id; g_room_rdt = r.rdt; g_room_rdt_ok = 1;
        re15_msg_load_room_block(r.rdt.messages, r.rdt.messages_size);
        scd_register_room_events(&r.rdt);
        scd_room_reenter(&r.rdt, 0, 250, 0);
    }
    CHECK(slot_ist(2, T_GEN, 6), "nach dem Herausnehmen: Platz 2 = Ereignis sub06 (Loeschen moeglich), ist %s ev %d",
          typname(2), g_aot.slots[2].event_id);
    raum_frei(&r);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int alle = strcmp(teil, "alle") == 0;
    if (alle || !strcmp(teil, "tabelle"))    teil_tabelle();
    if (alle || !strcmp(teil, "raumstart"))  teil_raumstart();
    if (alle || !strcmp(teil, "ablauf"))     teil_ablauf();
    if (alle || !strcmp(teil, "spielstand")) teil_spielstand();
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
