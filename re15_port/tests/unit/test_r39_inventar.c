/* test_r39_inventar.c — RIEGEL Spur C (Runde 39) "inventar". Dossier analysis/befunde_runde39/C_inventar.md,
 * Belege include/re15_inventar_r39.h, Zensus tools/r39_inventar/zensus_mengen.py.
 *
 * Teile (je ein ctest-Eintrag, je Nutzer-Punkt die MECHANIK am echten Raum-/VM-Weg):
 *   spind     (1) ROOM1000 (Leon): Slot 3 = Aufnahme Item 0x15 x30 Bit (9,69) prop 0xFF (Satzform ROOM1001 @0x00C24),
 *             der Raumtext msg 0 wandert UNVERAENDERT nach Slot 11 (gleiches Rechteck). Aktionsdruck vor dem Spind ->
 *             Aufnahme-Modal 0x15 -> Ja -> Inventar +15 (Port-Halbierung 30 -> 15), (9,69)=1, Zone weg; zweiter Druck
 *             -> msg 0 "Nothing unusual.". Wiederbetreten mit (9,69)=1: Slot 3 = Text, keine Munition.
 *             Elza ROOM1001: der Original-Satz (Item 0x15 x30, Bit 0x86) bleibt, Slot 11 leer.
 *   stapeln   (2) Stapelregel "alles mit einer Zahl": Zensus aller Ids gegen die Eigenschaftstabelle @0x80074DA8
 *             (Zahl: Ids 0x01..0x21; Anzahl-Zahl: Munition 0x15..0x21 + Wurfgranaten mit Obergrenze 250); Aufnahme
 *             (re15_inv_grant_stapeln_nutzer 0x21 3 + 3 -> EIN Platz 6), Paar-Matcher (0x21 + 0x21 -> Aktion 5,
 *             0x15 bleibt beim Original-Satz, verschiedene Ids 0), ITEM BOX (0x21 2 + 3 -> 5) und der ECHTE
 *             Menue-Weg (START, ITEM, Platz 0, COMBINE, Platz 1, Bestaetigen) -> 21:5 auf Platz 0, Platz 1 leer.
 *   status    (3) Status-Bild je Figur: Index aus work_vars[0x10] (Leon 0 -> STPIC_00, Elza 4 -> STPIC_04), Pfad,
 *             Dateien 12492 B mit gleicher TIM-Geometrie (CLUT (0,485) 256x1, Bild (640,406) 58hw x 103), STPIC_04 !=
 *             STPIC_00 und die Varianten je Figur gleich (00=01=02, 04=05=06).
 *   discard   (4) Wegwerf-Stelle der Yellow Keycard: ROOM11E1 msg 5 Gate (3,139) in gen/discard_sites.inc, ROOM11E0
 *             gestrichen; echter Weg bei Elza in ROOM11E1 (Kartenleser sub20 -> Code-Bits (5,13..16) -> sub01 ->
 *             sub19 msg 5) -> Abfrage -> Ja -> Karte weg; Leon ROOM11E0 mit (3,139)=1 (ROOM1240 @0x0055A): Tuer statt
 *             Leser, keine Abfrage.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_inventar_r39.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_item_modal.h"
#include "re15_inventory.h"
#include "re15_inv_ui.h"
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
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_itembox.h"
#include "re15_map_hint.h"
#include "gen/discard_sites.inc"

extern uint8_t g_aot_action_pressed;
extern int scd_event_fire(uint8_t event_id);
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

/* ---- Bild-Ablauf wie platform/pc/main.c (Muster test_r37_feuerloescher.c bild / test_r38_tagebuch.c bild) */
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_cine_was = 0, s_bild = 0;
static uint64_t           s_uhr = 1000000;

static uint16_t virt(uint16_t raw) { return (raw & RE15_PAD_BIT_SQUARE) ? 0x4000u : 0u; }

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern; Muster test_r37_feuerloescher.c start).
 * neu=1: frisches Spiel (Flags/Inventar/Abfrage leer, dann `flags` (Paare bank,bit; Ende <0) und `items` (Paare
 * id,menge; Ende 0)); neu=0: Flags und Inventar bleiben (Wiederbetreten). */
static void start(raum_t *r, int32_t px, int32_t py, int32_t pz, int16_t rot, const int *flags,
                  const uint8_t *items, int elza, int neu)
{
    if (neu) {
        re15_game_state_init();
        re15_inv_init();
        re15_itembox_init();
        re15_files_reset();
        re15_discard_reset();
        for (int k = 0; flags && flags[k] >= 0; k += 2)
            re15_game_flag_set((uint8_t)flags[k], (uint8_t)flags[k + 1], 1);
        for (int k = 0; items && items[k]; k += 2) re15_inv_grant(items[k], items[k + 1]);
    }
    re15_game_state_t flags_vorher = g_game;
    re15_inventory_t  inv_vorher   = g_inv;
    re15_aot_init();
    scd_vm_init(); re15_actor_init();        /* scd_vm_init leert auch das Inventar (scd_vm.c) */
    g_game = flags_vorher; g_inv = inv_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_gameflow.character = elza ? 4 : 0;
    g_scd.work_vars[0x10] = (int16_t)(g_gameflow.character & 0x0F);   /* wie main.c nach scd_vm_init (DAT_800b0ff0) */
    g_current_room_id = r->id;
    g_room_rdt = r->rdt; g_room_rdt_ok = 1;
    memset(&g_room_change, 0, sizeof g_room_change);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(re15_collision_band_from_y(py));
    g_scd.player_mode = 0;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &r->rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_shown = 0; s_cine_was = 0; s_bild = 0;
    g_scd.cut_auto_enabled = 1;
    re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_register_room_events(&r->rdt);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

/* Ein Bild. `antwort` = virtuelles Wort fuer die Wegwerf-Abfrage (0x4000 Ja, 0x8000 Nein, 0 nichts). Ist der
 * Statusschirm offen (Spiel eingefroren), laeuft nur der Schirm (re15_game_step -> re15_menu_fsm_tick). */
static void bild(uint16_t held, uint16_t edge, uint16_t antwort)
{
    s_uhr += 16667; re15_host_clock_set_us(s_uhr);
    s_bild++;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    if (re15_menu_gameplay_frozen()) {
        re15_game_step(&s_ctx);
        if (re15_menu_is_open()) re15_inv_screen_ecg_tick();
        re15_fade_tick();
        return;
    }
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
            if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
                g_scd.player_mode = 0;
                re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
            }
        }
        re15_msg_tick(&raw, &len, &id);
        if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
        s_ctx.active_cut = s_shown;
        re15_game_step(&s_ctx);
        if (re15_item_modal_active()) re15_item_modal_tick(virt(edge), virt(held));
        { scd_audio_event_t e; while (scd_audio_queue_pop(&e)) { } }
    }
    if (re15_discard_active()) re15_discard_tick(antwort, 0);
}

/* EIN Aktionsdruck ohne Bild-Ablauf (Muster test_r37_feuerloescher.c druck): Flanke -> Scan -> Ereignis. */
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

static int menge(uint8_t id)
{
    int m = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) m += g_inv.slots[i].qty;
    return m;
}
static int plaetze(uint8_t id)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) n++;
    return n;
}

static int warte_msg(int id, int max)
{
    for (int f = 0; f < max; f++) {
        bild(0, 0, 0);
        if (g_scd.message_active && g_scd.message_id == id) return 1;
    }
    return 0;
}

/* Aufnahme-Modal bis "bereit" laufen lassen, dann Ja (virtuell 0x4000 = Viereck). */
static int modal_ja(uint8_t soll_item)
{
    int w = 0; uint8_t typ = 0; int wahl = 0;
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800) re15_item_modal_tick(0, 0);
    re15_item_modal_prompt(&typ, &wahl);
    int ok = re15_item_modal_active() && typ == soll_item;
    w = 0;
    while (re15_item_modal_active() && w++ < 1600) re15_item_modal_tick(0x4000u, 0);
    return ok;
}

/* ============================================================================================ (1) spind */
static void spind_slots_pruefen(const char *was, int munition_in_3, int text_in)
{
    const re15_aot_t *a3 = &g_aot.slots[RE15_R39C_SPIND_SLOT];
    if (munition_in_3) {
        CHECK(a3->active && a3->type == RE15_AOT_TYPE_ITEM &&
              g_aot.item_params[RE15_R39C_SPIND_SLOT].item_type == RE15_R39C_MUNITION_ITEM &&
              g_aot.item_params[RE15_R39C_SPIND_SLOT].amount == RE15_R39C_MUNITION_MENGE &&
              g_aot.item_params[RE15_R39C_SPIND_SLOT].taken_bit == RE15_R39C_MUNITION_BIT &&
              g_aot.item_params[RE15_R39C_SPIND_SLOT].taken_prop == RE15_R39C_KEIN_MODELL,
              "%s: Slot 3 = Aufnahme Item 0x%02X x%u Bit (9,%u) prop 0x%02X (Satzform ROOM1001 @0x00C24)", was,
              (unsigned)g_aot.item_params[RE15_R39C_SPIND_SLOT].item_type,
              (unsigned)g_aot.item_params[RE15_R39C_SPIND_SLOT].amount,
              (unsigned)g_aot.item_params[RE15_R39C_SPIND_SLOT].taken_bit,
              (unsigned)g_aot.item_params[RE15_R39C_SPIND_SLOT].taken_prop);
        CHECK(a3->x == 20900 && a3->z == -400 && a3->half_w == 400 && a3->half_h == 400 &&
              a3->sce_flags == RE15_R39C_AOT_SAT && a3->band == 0,
              "%s: Slot 3 Rechteck (20500,-800) 800x800 = Mitte (%d,%d) halb (%d,%d), sat 0x%02X Band %u", was,
              a3->x, a3->z, a3->half_w, a3->half_h, (unsigned)a3->sce_flags, (unsigned)a3->band);
    }
    const re15_aot_t *t = &g_aot.slots[text_in];
    CHECK(t->active && t->type == RE15_AOT_TYPE_MESSAGE && t->event_id == RE15_R39C_TEXT_MSG &&
          t->x == 20900 && t->z == -400 && t->half_w == 400 && t->half_h == 400,
          "%s: Slot %d = Raumtext msg %u auf demselben Rechteck (ist type %u evt %u (%d,%d) halb (%d,%d))", was,
          text_in, (unsigned)RE15_R39C_TEXT_MSG, (unsigned)t->type, (unsigned)t->event_id, t->x, t->z, t->half_w,
          t->half_h);
}

static void teil_spind(void)
{
    raum_t r;
    printf("\n[spind]\n");
    if (raum_laden(&r, RE15_R39C_SPIND_RAUM)) return;
    /* A: neues Spiel, Leon vor dem Spind (Karte der Messung: (20900,-1100) rot 3072, Pruefpunkt (20900,-480)). */
    start(&r, 20900, 0, -1100, 3072, NULL, NULL, 0, 1);
    for (int f = 0; f < 30; f++) bild(0, 0, 0);
    spind_slots_pruefen("A Eintritt", 1, RE15_R39C_TEXT_SLOT);
    int kein_0x15_anderswo = 1;
    for (int s = 0; s < RE15_AOT_MAX; s++)
        if (s != RE15_R39C_SPIND_SLOT && g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_ITEM &&
            g_aot.item_params[s].item_type == RE15_R39C_MUNITION_ITEM) kein_0x15_anderswo = 0;
    CHECK(kein_0x15_anderswo && !re15_game_flag_get(9, RE15_R39C_MUNITION_BIT) && menge(0x15) == 0,
          "A Eintritt: genau EINE Munitionszone, (9,69)=0, Inventar ohne 0x15");

    /* B: Aktionsdruck -> Modal 0x15 -> Ja -> +15 (Port-Halbierung der Welt-Aufnahme, item_modal_common.c:157). */
    int ev = druck();
    for (int f = 0; f < 3 && !re15_item_modal_active(); f++) bild(0, 0, 0);
    int modal = re15_item_modal_active();
    int ja = modal_ja(RE15_R39C_MUNITION_ITEM);
    for (int f = 0; f < 5; f++) bild(0, 0, 0);
    CHECK(modal && ja, "B Druck (20900,-1100) Blick +z: Aufnahme-Modal Item 0x15 (Ereignis %d, modal %d, ja %d)", ev,
          modal, ja);
    CHECK(menge(0x15) == 15 && plaetze(0x15) == 1,
          "B Ja: Inventar 0x15 x%d auf %d Platz (30 im Satz, 15 nach der Nutzer-Halbierung 2026-09-20)",
          menge(0x15), plaetze(0x15));
    CHECK(re15_game_flag_get(9, RE15_R39C_MUNITION_BIT) == 1, "B Ja: Taken-Bit (9,%u)=1 (VERTRAG 1.1)",
          (unsigned)RE15_R39C_MUNITION_BIT);
    CHECK(!(g_aot.slots[RE15_R39C_SPIND_SLOT].active && g_aot.slots[RE15_R39C_SPIND_SLOT].type == RE15_AOT_TYPE_ITEM
            && g_aot.slots[RE15_R39C_SPIND_SLOT].sce_flags),
          "B Ja: die Munitionszone antwortet nicht mehr (Slot 3 active %d type %u sce 0x%02X)",
          g_aot.slots[RE15_R39C_SPIND_SLOT].active, (unsigned)g_aot.slots[RE15_R39C_SPIND_SLOT].type,
          (unsigned)g_aot.slots[RE15_R39C_SPIND_SLOT].sce_flags);

    /* C: zweiter Druck an derselben Stelle -> der Raumtext msg 0 (Slot 11), kein Modal. */
    for (int f = 0; f < 10; f++) bild(0, 0, 0);
    ev = druck();
    for (int f = 0; f < 3; f++) bild(0, 0, 0);
    CHECK(!re15_item_modal_active() && g_scd.message_active && g_scd.message_id == RE15_R39C_TEXT_MSG,
          "C zweiter Druck: msg %u \"Nothing unusual.\" (ROOM1000 @0x0D22), kein Modal (msg %d/%d modal %d)",
          (unsigned)RE15_R39C_TEXT_MSG, (int)g_scd.message_active, (int)g_scd.message_id, re15_item_modal_active());
    {
        const char *txt = re15_msg_get_text(RE15_R39C_TEXT_MSG);
        CHECK(txt && strstr(txt, "Nothing unusual") != NULL, "C Text von msg 0: \"%s\"", txt ? txt : "(null)");
    }
    for (int f = 0; f < 400 && g_scd.message_active; f++)
        bild(f % 20 == 0 ? RE15_PAD_BIT_SQUARE : 0, f % 20 == 0 ? RE15_PAD_BIT_SQUARE : 0, 0);
    CHECK(menge(0x15) == 15, "C danach: Inventar unveraendert 0x15 x%d", menge(0x15));

    /* D: Wiederbetreten mit (9,69)=1 -> der Text bleibt in Slot 3 (feuerloescher_r37.c), keine Munition. */
    start(&r, 20900, 0, -1100, 3072, NULL, NULL, 0, 0);
    for (int f = 0; f < 30; f++) bild(0, 0, 0);
    spind_slots_pruefen("D Wiederbetreten (9,69)=1", 0, RE15_R39C_SPIND_SLOT);
    int zone_0x15 = 0;
    for (int s = 0; s < RE15_AOT_MAX; s++)
        if (g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_ITEM &&
            g_aot.item_params[s].item_type == RE15_R39C_MUNITION_ITEM) zone_0x15++;
    CHECK(zone_0x15 == 0 && !g_aot.slots[RE15_R39C_TEXT_SLOT].active,
          "D Wiederbetreten: keine Munitionszone (%d), Slot 11 leer (%d)", zone_0x15,
          g_aot.slots[RE15_R39C_TEXT_SLOT].active);
    ev = druck();
    for (int f = 0; f < 3; f++) bild(0, 0, 0);
    CHECK(!re15_item_modal_active() && g_scd.message_active && g_scd.message_id == RE15_R39C_TEXT_MSG,
          "D Druck: sofort msg 0 (ist msg %d/%d, modal %d)", (int)g_scd.message_active, (int)g_scd.message_id,
          re15_item_modal_active());
    for (int f = 0; f < 400 && g_scd.message_active; f++)
        bild(f % 20 == 0 ? RE15_PAD_BIT_SQUARE : 0, f % 20 == 0 ? RE15_PAD_BIT_SQUARE : 0, 0);

    /* E: Elza ROOM1001 = das Original (Item 0x15 x30, Bit 0x86 @0x00C36), der Installer greift nicht. */
    raum_t r1;
    if (raum_laden(&r1, 0x1001)) return;
    start(&r1, 20900, 0, -1100, 3072, NULL, NULL, 1, 1);
    for (int f = 0; f < 30; f++) bild(0, 0, 0);
    {
        const re15_aot_t *a3 = &g_aot.slots[3];
        CHECK(a3->active && a3->type == RE15_AOT_TYPE_ITEM && g_aot.item_params[3].item_type == 0x15 &&
              g_aot.item_params[3].amount == 30 && g_aot.item_params[3].taken_bit == 0x86 &&
              !g_aot.slots[RE15_R39C_TEXT_SLOT].active,
              "E Elza ROOM1001: Slot 3 = Original Item 0x%02X x%u Bit (9,0x%02X) @0x00C24, Slot 11 leer (%d)",
              (unsigned)g_aot.item_params[3].item_type, (unsigned)g_aot.item_params[3].amount,
              (unsigned)g_aot.item_params[3].taken_bit, g_aot.slots[RE15_R39C_TEXT_SLOT].active);
    }
    free(r.buf); free(r1.buf);
}

/* ============================================================================================ (2) stapeln */
static uint8_t obergrenze(uint8_t id) { return *RE15_INV_PTR(0x80074DA8u + (uint32_t)id * 12u); }
static int selbst_saetze(uint8_t id)
{
    const uint32_t row = 0x80074DA8u + (uint32_t)id * 12u;
    const uint8_t n = *RE15_INV_PTR(row + 9u);
    const unsigned char *p = RE15_INV_PTR(row + 4u);
    const uint32_t satz = (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    int k = 0;
    for (uint8_t i = 0; i < n; i++) {
        const unsigned char *s = RE15_INV_PTR(satz + 4u * i);
        if (s[0] == id && s[2] == RE15_R39C_AKTION_SELBST) k++;
    }
    return k;
}

static void teil_stapeln(void)
{
    printf("\n[stapeln]\n");
    /* Z: Zensus 0x01..0x47 — Regel == Daten (Zahl-Fenster `sltiu id,0x22` @0x80049124; Anzahl-Zahl = Munition
     * 0x15..0x21 oder Obergrenze 250 @0x80074DA8+id*12). */
    int falsch = 0, n_ja = 0, n_granaten = 0, n_ohne_satz = 0;
    for (int id = 1; id < 0x48; id++) {
        int soll = (id >= 0x15 && id <= 0x21) || (id < 0x22 && obergrenze((uint8_t)id) == RE15_R39C_STAPEL_OBERGRENZE);
        int ist = re15_inventar_r39_stapelt((uint8_t)id);
        if (ist != soll) { falsch++; printf("      Id 0x%02X: Regel %d, Daten %d\n", id, ist, soll); }
        if (ist) n_ja++;
        if (ist && id < 0x15) n_granaten++;
        if (ist && selbst_saetze((uint8_t)id) == 0) n_ohne_satz++;
    }
    CHECK(falsch == 0 && n_ja == 16 && n_granaten == 3, "Z Zensus: %d stapelbare Ids (Soll 16 = 0x15..0x21 + 3 Granaten "
          "0x09..0x0B), %d Abweichungen", n_ja, falsch);
    CHECK(obergrenze(0x09) == 250 && obergrenze(0x0A) == 250 && obergrenze(0x0B) == 250 && obergrenze(0x21) == 250 &&
          obergrenze(0x03) != 250,
          "Z Obergrenzen @0x80074DA8: 0x09/0x0A/0x0B/0x21 = 250 (`fa`), Handgun 0x03 = %u (Magazin)", (unsigned)obergrenze(0x03));
    CHECK(selbst_saetze(0x15) == 1 && selbst_saetze(0x21) == 0 && selbst_saetze(0x09) == 0 && n_ohne_satz == 4,
          "Z Selbst-Saetze: 0x15 hat einen (@0x80074CCC `15 15 05 00`), 0x21/0x09..0x0B keinen (%d ohne) -> Port-Paar",
          n_ohne_satz);
    CHECK(!re15_inventar_r39_stapelt(0) && !re15_inventar_r39_stapelt(0x22) && !re15_inventar_r39_stapelt(0x39) &&
          !re15_inventar_r39_stapelt(0x03), "Z keine Zahl / Feuerwaffe / Schluessel stapeln nicht (0, 0x22, 0x39, 0x03)");

    /* G: Aufnahme-Gutschrift 0x21 3 + 3 -> EIN Platz 6; Obergrenze 250 greift. */
    re15_game_state_init(); re15_inv_init(); re15_itembox_init();
    CHECK(re15_inv_grant_stapeln_nutzer(0x21, 3) == 0 && re15_inv_grant_stapeln_nutzer(0x21, 3) == 0 &&
          menge(0x21) == 6 && plaetze(0x21) == 1, "G Gutschrift 3 + 3 -> 0x21 x%d auf %d Platz", menge(0x21), plaetze(0x21));
    CHECK(re15_pickup_passt_nutzer(0x21, 3) == 1 && re15_pickup_passt_nutzer(0x21, 250) == 0,
          "G passt: +3 ja, +250 nein (Obergrenze 250, `lbu` @0x8004e338)");
    re15_inv_init();
    CHECK(re15_inv_grant_stapeln_nutzer(0x09, 1) == 0 && re15_inv_grant_stapeln_nutzer(0x09, 2) == 0 &&
          menge(0x09) == 3 && plaetze(0x09) == 1, "G Granaten 0x09 1 + 2 -> x%d auf %d Platz", menge(0x09), plaetze(0x09));

    /* P: Paar-Matcher (menu_common.c exchange_match-Haken). */
    re15_inv_init();
    g_inv.slots[0].id = 0x21; g_inv.slots[0].qty = 3;
    g_inv.slots[1].id = 0x21; g_inv.slots[1].qty = 2;
    g_inv.slots[2].id = 0x15; g_inv.slots[2].qty = 15;
    g_inv.slots[3].id = 0x15; g_inv.slots[3].qty = 15;
    g_inv.slots[4].id = 0x39; g_inv.slots[4].qty = 1;
    g_inv.slots[5].id = 0x39; g_inv.slots[5].qty = 1;
    uint8_t res = 0, pic = 9;
    CHECK(re15_inventar_r39_paar(0, 1, &res, &pic) == RE15_R39C_AKTION_SELBST && res == 0x21 && pic == 0,
          "P 0x21 + 0x21 -> Aktion 5, Ergebnis 0x%02X, Bild %u (Form `15 15 05 00`)", (unsigned)res, (unsigned)pic);
    CHECK(re15_inventar_r39_paar(1, 0, &res, &pic) == RE15_R39C_AKTION_SELBST, "P Reihenfolge egal (1,0) -> 5");
    CHECK(re15_inventar_r39_paar(2, 3, &res, &pic) == 0, "P 0x15 + 0x15 -> 0 (eigener Original-Satz entscheidet)");
    CHECK(re15_inventar_r39_paar(0, 2, &res, &pic) == 0 && re15_inventar_r39_paar(4, 5, &res, &pic) == 0 &&
          re15_inventar_r39_paar(0, 0, &res, &pic) == 0, "P verschiedene Ids / Schluessel / gleicher Platz -> 0");

    /* K: ITEM BOX (re15_itembox_transfer, RE2 FUN_800703b8-Form): Kiste 0x21 x2 + Inventar 0x21 x3 -> 5, Kiste leer. */
    re15_inv_init(); re15_itembox_init();
    g_inv.slots[0].id = 0x21; g_inv.slots[0].qty = 3;
    g_itembox.slots[2].id = 0x21; g_itembox.slots[2].qty = 2;
    int rc = re15_itembox_transfer(0, 2);
    CHECK(rc == RE15_BOX_XFER_OK && g_inv.slots[0].id == 0x21 && g_inv.slots[0].qty == 5 && g_itembox.slots[2].id == 0,
          "K Kiste 2 + Inventar 3 -> Inventar 0x21 x%u, Kiste leer (rc %d)", (unsigned)g_inv.slots[0].qty, rc);

    /* M: der ECHTE Menue-Weg (Statusschirm): START, ITEM (Viereck), Platz 0 (Viereck), COMBINE (rechts), Viereck,
     * zweiter Cursor rechts -> Platz 1, Viereck -> 21:5 auf Platz 0, Platz 1 leer. Bildnummern wie im Spiel
     * (Messung C_belege: F129 offen, F139 sub 3, F149 st 3, F158 cmd 3, F160 st 7, F162 cur2 1, F164 zusammen). */
    raum_t r;
    if (raum_laden(&r, 0x1010)) return;
    {
        static const uint8_t items[] = { 0x21, 3, 0x21, 2, 0 };
        start(&r, 2800, 0, -1300, 0, NULL, items, 0, 1);
    }
    for (int f = 0; f < 20; f++) bild(0, 0, 0);
    CHECK(g_inv.slots[0].id == 0x21 && g_inv.slots[0].qty == 3 && g_inv.slots[1].id == 0x21 && g_inv.slots[1].qty == 2,
          "M Start: Platz 0 = 21:3, Platz 1 = 21:2 (ist %02X:%u, %02X:%u)", (unsigned)g_inv.slots[0].id,
          (unsigned)g_inv.slots[0].qty, (unsigned)g_inv.slots[1].id, (unsigned)g_inv.slots[1].qty);
    /* Die Tasten fallen wie im Spiel-Lauf (Messung): START F120 -> offen F129; Viereck F139 (10 Bilder nach dem
     * Oeffnen, die Einblendung nimmt vorher keine Taste); Viereck 2 Bilder nach st 1; rechts 1 Bild nach st 4;
     * Viereck 2 Bilder nach cmd 3; rechts 2 nach st 7; Viereck 2 nach cur2 1. */
    bild(RE15_PAD_BIT_START, RE15_PAD_BIT_START, 0);
    int f_offen = -1, f_sub3 = -1, f_st4 = -1, f_cmd3 = -1, f_st7 = -1, f_cur2 = -1, f_fertig = -1, bild0 = s_bild;
    for (int f = 0; f < 120 && f_offen < 0; f++) { bild(0, 0, 0); if (re15_menu_is_open()) f_offen = s_bild - bild0; }
    CHECK(f_offen >= 0, "M START -> Statusschirm offen nach %d Bildern", f_offen);
    for (int f = 0; f < 10; f++) bild(0, 0, 0);
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE, 0);
    for (int f = 0; f < 60 && f_sub3 < 0; f++) { if (g_inv_screen.substate == 3) f_sub3 = s_bild - bild0; else bild(0, 0, 0); }
    for (int f = 0; f < 60 && g_inv_screen.item_state != 1; f++) bild(0, 0, 0);
    CHECK(f_sub3 >= 0 && g_inv_screen.item_state == 1, "M Viereck -> ITEM-Liste (sub 3 nach %d, st %u)", f_sub3,
          (unsigned)g_inv_screen.item_state);
    for (int f = 0; f < 2; f++) bild(0, 0, 0);
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE, 0);
    for (int f = 0; f < 60 && f_st4 < 0; f++) { if (g_inv_screen.item_state == 4) f_st4 = s_bild - bild0; else bild(0, 0, 0); }
    CHECK(f_st4 >= 0 && g_inv_screen.item_cursor == 0, "M Platz 0 gewaehlt -> Befehle (st 4 nach %d, cur %u)", f_st4,
          (unsigned)g_inv_screen.item_cursor);
    bild(0, 0, 0);
    bild(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT, 0);
    for (int f = 0; f < 10 && f_cmd3 < 0; f++) { if (g_inv_screen.action_dir == 3) f_cmd3 = s_bild - bild0; else bild(0, 0, 0); }
    CHECK(f_cmd3 >= 0, "M rechts -> COMBINE (cmd 3 nach %d, @0x800c64c0-Cluster)", f_cmd3);
    for (int f = 0; f < 2; f++) bild(0, 0, 0);
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE, 0);
    for (int f = 0; f < 30 && f_st7 < 0; f++) { if (g_inv_screen.item_state == 7) f_st7 = s_bild - bild0; else bild(0, 0, 0); }
    CHECK(f_st7 >= 0, "M Viereck -> zweiter Cursor (st 7 nach %d)", f_st7);
    for (int f = 0; f < 2; f++) bild(0, 0, 0);
    bild(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT, 0);
    for (int f = 0; f < 10 && f_cur2 < 0; f++) { if (g_inv_screen.second_cursor == 1) f_cur2 = s_bild - bild0; else bild(0, 0, 0); }
    CHECK(f_cur2 >= 0, "M rechts -> zweiter Cursor Platz 1 (nach %d)", f_cur2);
    for (int f = 0; f < 2; f++) bild(0, 0, 0);
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE, 0);
    for (int f = 0; f < 60 && f_fertig < 0; f++) {
        if (g_inv.slots[0].qty == 5 && g_inv.slots[1].id == 0) f_fertig = s_bild - bild0; else bild(0, 0, 0);
    }
    CHECK(f_fertig >= 0 && g_inv.slots[0].id == 0x21 && g_inv.slots[0].qty == 5 && g_inv.slots[1].id == 0 &&
          g_inv.slots[1].qty == 0 && plaetze(0x21) == 1,
          "M Bestaetigen -> Platz 0 = %02X:%u, Platz 1 = %02X:%u (Soll 21:5 / 00:0, nach %d Bildern; Aktion 5 -> "
          "x_reload @0x8004e324)", (unsigned)g_inv.slots[0].id, (unsigned)g_inv.slots[0].qty,
          (unsigned)g_inv.slots[1].id, (unsigned)g_inv.slots[1].qty, f_fertig);
    for (int f = 0; f < 120 && re15_menu_is_open(); f++)
        bild(f == 0 ? RE15_PAD_BIT_CROSS : 0, f == 0 ? RE15_PAD_BIT_CROSS : 0, 0);
    free(r.buf);
}

/* ============================================================================================ (3) status */
static uint8_t *stpic_laden(int k, int *n)
{
    char p[600];
    snprintf(p, sizeof p, "%s/ITEM/STPIC_%02X.TIM", RE15_ASSET_PSX_DIR, (unsigned)k);
    return slurp(p, n);
}
static int tim_geometrie_ok(const uint8_t *b, int n)
{
    if (!b || n != 12492 || b[0] != 0x10 || (b[4] & 0x0F) != 0x09) return 0;      /* 8bpp + CLUT */
    int cx = b[12] | b[13] << 8, cy = b[14] | b[15] << 8, cw = b[16] | b[17] << 8, ch = b[18] | b[19] << 8;
    uint32_t cl = (uint32_t)b[8] | ((uint32_t)b[9] << 8) | ((uint32_t)b[10] << 16);
    const uint8_t *q = b + 8 + cl;
    int px = q[4] | q[5] << 8, py = q[6] | q[7] << 8, pw = q[8] | q[9] << 8, ph = q[10] | q[11] << 8;
    return cx == 0 && cy == 485 && cw == 256 && ch == 1 && px == 640 && py == 406 && pw == 58 && ph == 103;
}

static void teil_status(void)
{
    printf("\n[status]\n");
    char pfad[64];
    g_gameflow.character = 0; g_scd.work_vars[0x10] = 0;
    re15_inventar_r39_stpic_pfad(re15_inventar_r39_stpic_index(), pfad, (int)sizeof pfad);
    CHECK(re15_inventar_r39_stpic_index() == 0 && strcmp(pfad, "ITEM/STPIC_00.TIM") == 0,
          "Leon: Index %d -> %s (DAT_800aca5c = 0, Tabelle @0x80073ad8[0] = 0x2c)", re15_inventar_r39_stpic_index(), pfad);
    g_gameflow.character = 4; g_scd.work_vars[0x10] = 4;
    re15_inventar_r39_stpic_pfad(re15_inventar_r39_stpic_index(), pfad, (int)sizeof pfad);
    CHECK(re15_inventar_r39_stpic_index() == 4 && strcmp(pfad, "ITEM/STPIC_04.TIM") == 0,
          "Elza: Index %d -> %s (DAT_800aca5c = 4 @0x801024cc, Datei-Id 0x30)", re15_inventar_r39_stpic_index(), pfad);
    g_scd.work_vars[0x10] = 1;   /* Leon mit Weste (PL01) */
    re15_inventar_r39_stpic_pfad(re15_inventar_r39_stpic_index(), pfad, (int)sizeof pfad);
    CHECK(re15_inventar_r39_stpic_index() == 1 && strcmp(pfad, "ITEM/STPIC_01.TIM") == 0,
          "Weste: work_vars[0x10]=1 -> %s (Raumlader-Merge @0x80039784)", pfad);
    g_scd.work_vars[0x10] = 99; g_gameflow.character = 4;
    CHECK(re15_inventar_r39_stpic_index() == 4, "ausser Tabelle (99) -> Rueckfall auf die Figur (4)");
    g_scd.work_vars[0x10] = 0; g_gameflow.character = 0;

    int n0 = 0, n1 = 0, n2 = 0, n4 = 0, n5 = 0, n6 = 0;
    uint8_t *s0 = stpic_laden(0, &n0), *s1 = stpic_laden(1, &n1), *s2 = stpic_laden(2, &n2);
    uint8_t *s4 = stpic_laden(4, &n4), *s5 = stpic_laden(5, &n5), *s6 = stpic_laden(6, &n6);
    CHECK(tim_geometrie_ok(s0, n0) && tim_geometrie_ok(s4, n4),
          "STPIC_00/04: 12492 B, 8bpp, CLUT (0,485) 256x1, Bild (640,406) 58hw x 103 (Datei-Tabelle @0x8006f43c)");
    CHECK(s0 && s4 && n0 == n4 && memcmp(s0, s4, (size_t)n0) != 0, "STPIC_04 (Elza) != STPIC_00 (Leon)");
    CHECK(s0 && s1 && s2 && n0 == n1 && n1 == n2 && !memcmp(s0, s1, (size_t)n0) && !memcmp(s0, s2, (size_t)n0),
          "Leons Varianten 00 = 01 = 02 (PL00..PL02 teilen den Ausweis)");
    CHECK(s4 && s5 && s6 && n4 == n5 && n5 == n6 && !memcmp(s4, s5, (size_t)n4) && !memcmp(s4, s6, (size_t)n4),
          "Elzas Varianten 04 = 05 = 06 (PL04..PL06 teilen die Driver License)");
    free(s0); free(s1); free(s2); free(s4); free(s5); free(s6);
}

/* ============================================================================================ (4) discard */
static int stelle_index(unsigned room, uint8_t msg)
{
    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++)
        if (re15_discard_sites[i].room == (uint16_t)room && re15_discard_sites[i].msg == msg) return i;
    return -1;
}
static const re15_discard_panel_t *panel(unsigned room)
{
    for (int i = 0; i < RE15_DISCARD_PANEL_COUNT; i++)
        if (re15_discard_panels[i].room == (uint16_t)room) return &re15_discard_panels[i];
    return NULL;
}

static void teil_discard(void)
{
    printf("\n[discard]\n");
    /* T: die erzeugte Tabelle (tools/gen_discard_sites.py, E5 je Szenario-Variante). */
    int i1 = stelle_index(0x11E1, 5), i0 = stelle_index(0x11E0, 5), i9 = stelle_index(0x11E1, 9);
    CHECK(i1 >= 0 && re15_discard_sites[i1].item == 0x39 && re15_discard_sites[i1].gate_zone == 3 &&
          re15_discard_sites[i1].gate_bit == 139,
          "T ROOM11E1 msg 5 = Stelle der Yellow Keycard 0x39, Gate flag(3,139) (`Set` @0x01A8A)");
    CHECK(i0 < 0 && i9 < 0, "T ROOM11E0 msg 5 keine Stelle (Leon: ROOM1240 @0x0055A), ROOM11E1 msg 9 (Einstecken) keine");
    int n39 = 0;
    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) if (re15_discard_sites[i].item == 0x39) n39++;
    CHECK(n39 == 1 && RE15_DISCARD_SITE_COUNT == 15, "T genau eine 0x39-Stelle, %d Stellen gesamt (14 + 1)",
          RE15_DISCARD_SITE_COUNT);
    const re15_discard_panel_t *p1 = panel(0x11E1), *p0 = panel(0x11E0);
    CHECK(p1 && !p1->tot && p1->ausloeser_msg == 5 && p1->erfolg_sub == 19 && p1->taken_bit == 138 &&
          p0 && p0->tot, "T Panel 11E1 lebt (Erfolgs-Sub 19, msg 5, Taken-Bit 138), Panel 11E0 gestrichen");

    /* E: Vorentscheid-Logik (item_discard_common.c): Gate + Besitz. */
    re15_game_state_init(); re15_inv_init(); re15_discard_reset();
    re15_inv_grant(0x39, 1);
    g_current_room_id = 0x11E1;
    CHECK(re15_discard_besitz_vor_nachricht(0x11E1, 5) == 0, "E Gate zu ((3,139)=0): keine Stelle");
    re15_game_flag_set(3, 139, 1);
    CHECK(re15_discard_besitz_vor_nachricht(0x11E1, 5) == 1, "E Gate offen + Karte im Beutel: Vorentscheid mit Treffer");
    re15_discard_reset();
    CHECK(re15_discard_besitz_vor_nachricht(0x11E0, 5) == 0, "E ROOM11E0 (Leon) msg 5: keine Stelle");
    re15_inv_init(); re15_discard_reset();
    CHECK(re15_discard_besitz_vor_nachricht(0x11E1, 5) == 0, "E ohne Karte: kein Treffer");

    /* W: der ECHTE Weg bei Elza (ROOM11E1): Kartenleser (10650,3850) 800x800 -> msg 7 (Ja) -> msg 9 -> Tastenfeld;
     * Code-Bits (5,13..16) wie die vier Ziffern-Riegel sub01 @0x01A76..@0x01A82 -> Evt_exec sub19 @0x01A86 ->
     * Set(3,0x8B) @0x01A8A -> msg 5 @0x01F86 -> Wegwerf-Abfrage -> Ja -> Karte weg. */
    raum_t r;
    if (raum_laden(&r, 0x11E1)) return;
    {
        static const int flags[] = { 9, 0x8A, -1 };          /* Karte in ROOM1011 genommen (kistenregel_r37 K4) */
        static const uint8_t items[] = { 0x39, 1, 0 };
        start(&r, 10400, 0, 4250, 0, flags, items, 1, 1);
    }
    for (int f = 0; f < 30; f++) bild(0, 0, 0);
    CHECK(!re15_game_flag_get(3, 139) && menge(0x39) == 1, "W Elza-Start: (3,139)=0, Yellow Keycard im Beutel");
    int ev = druck();
    int m7 = (g_scd.message_active && g_scd.message_id == 7) || warte_msg(7, 60);
    CHECK(ev == 20 && m7, "W Druck am Leser: Ereignis sub%d, msg 7 \"Will you operate the card reader?\"", ev);
    /* Ja: bis zur Auswahl tippen lassen (Viereck = Schnellvorlauf/Seite), dann Viereck bei sel=1. */
    int ja = 0;
    for (int f = 0; f < 900 && !ja; f++) {
        if (g_scd.message_active && g_scd.message_id == 7 && g_scd.message_select) {
            bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE, 0); ja = 1;
        } else bild(f % 15 == 0 ? RE15_PAD_BIT_SQUARE : 0, f % 15 == 0 ? RE15_PAD_BIT_SQUARE : 0, 0);
    }
    int m9 = warte_msg(9, 120);
    CHECK(ja && m9, "W Ja -> msg 9 \"You've used the Yellow Keycard.\" (sub20 @0x01FAC) — noch KEINE Abfrage (%d)",
          re15_discard_active());
    CHECK(!re15_discard_active() && re15_discard_gefragt() == 0, "W msg 9 ist keine Stelle mehr (umgehaengt auf msg 5)");
    for (int f = 0; f < 300 && g_scd.message_active; f++)
        bild(f % 15 == 0 ? RE15_PAD_BIT_SQUARE : 0, f % 15 == 0 ? RE15_PAD_BIT_SQUARE : 0, 0);
    for (int f = 0; f < 60; f++) bild(0, 0, 0);
    /* Tastenfeld (sub17): die vier Ziffern-Riegel setzen, sub01 (Slot 1, jedes Bild neu geseedet) feuert sub19. */
    re15_game_flag_set(5, 13, 1); re15_game_flag_set(5, 14, 1); re15_game_flag_set(5, 15, 1); re15_game_flag_set(5, 16, 1);
    int m5 = 0;
    for (int f = 0; f < 600 && !m5; f++) {
        bild(f % 15 == 0 ? RE15_PAD_BIT_SQUARE : 0, f % 15 == 0 ? RE15_PAD_BIT_SQUARE : 0, 0);
        if (g_scd.message_active && g_scd.message_id == 5) m5 = 1;
    }
    CHECK(m5 && re15_game_flag_get(3, 139), "W Code-Bits -> sub19: Set(3,0x8B) @0x01A8A, msg 5 \"You've opened the lock.\"");
    int frage = 0; uint8_t it = 0;
    for (int f = 0; f < 900 && !frage; f++) {
        bild(f % 20 == 0 ? RE15_PAD_BIT_SQUARE : 0, f % 20 == 0 ? RE15_PAD_BIT_SQUARE : 0, 0);
        if (re15_discard_prompt(&it, NULL) && re15_discard_ready() && it == 0x39) frage = 1;
    }
    CHECK(frage, "W Wegwerf-Abfrage fuer Item 0x39 nach msg 5 (gen/discard_sites.inc ROOM11E1/5, Gate (3,139)=1)");
    for (int f = 0; f < 60 && re15_discard_active(); f++) bild(0, 0, f == 0 ? 0x4000u : 0);
    for (int f = 0; f < 5; f++) bild(0, 0, 0);
    CHECK(!re15_discard_active() && re15_discard_weggeworfen() == 1 && menge(0x39) == 0 && plaetze(0x39) == 0,
          "W Ja -> Yellow Keycard entfernt (Inventar 0x39 x%d, weggeworfen %d)", menge(0x39), re15_discard_weggeworfen());
    free(r.buf);

    /* L: Leon ROOM11E0 wie nach dem Neuspiel-Start (ROOM1240 @0x0055A setzt (3,0x8B)): kein Kartenleser (main00
     * @0x01576 Else-Zweig @0x016C0 = Tuer), kein Ereignis 20 am Leser-Rechteck, keine Abfrage. */
    raum_t r0;
    if (raum_laden(&r0, 0x11E0)) return;
    {
        static const int flags[] = { 3, 139, 9, 0x8A, -1 };
        static const uint8_t items[] = { 0x39, 1, 0 };
        start(&r0, 10400, 0, 4250, 0, flags, items, 0, 1);
    }
    for (int f = 0; f < 30; f++) bild(0, 0, 0);
    int leser = 0;
    for (int s = 0; s < RE15_AOT_MAX; s++)
        if (g_aot.slots[s].active && g_aot.slots[s].event_id == 20 &&
            (g_aot.slots[s].type == RE15_AOT_TYPE_GENERIC || g_aot.slots[s].type == RE15_AOT_TYPE_AUTO_EVENT)) leser++;
    ev = druck();
    for (int f = 0; f < 60; f++) bild(0, 0, 0);
    CHECK(leser == 0 && ev != 20 && re15_discard_gefragt() == 1 && menge(0x39) == 1,
          "L Leon ROOM11E0 mit (3,139)=1: kein Leser (sub20-Ausloeser %d, Ereignis %d), keine neue Abfrage (%d), Karte bleibt",
          leser, ev, re15_discard_gefragt());
    free(r0.buf);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    printf("test_r39_inventar %s\n", teil);
    if (!strcmp(teil, "spind"))        teil_spind();
    else if (!strcmp(teil, "stapeln")) teil_stapeln();
    else if (!strcmp(teil, "status"))  teil_status();
    else if (!strcmp(teil, "discard")) teil_discard();
    else { teil_spind(); teil_stapeln(); teil_status(); teil_discard(); }
    printf("\n%d Pruefungen, %d FEHLER\n", checks, fails);
    return fails ? 1 : 0;
}
