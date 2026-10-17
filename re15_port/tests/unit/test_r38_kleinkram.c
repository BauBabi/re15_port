/* test_r38_kleinkram.c — RIEGEL Runde 38 Spur Q "kleinkram": misst die MECHANIK jedes Nutzer-Punkts.
 *
 * Dossier: analysis/befunde_runde38/Q_kleinkram.md. Belege/Konstanten: include/re15_kleinkram_r38.h.
 * Teile (je ein ctest-Eintrag, registriert in tests/unit/probes/r38_kleinkram.cmake):
 *   inventar        P1: START-Annahme -> CORE Satz 6 (RE2 @0x8002652c), Abbruch/START/EXIT der Reiterwahl -> Satz 5
 *                   (RE2 @0x8006a930/@0x8006a968/@0x8006a8d4); Box-Anforderung ohne Oeffnen-Ton (RE2 @0x80026404)
 *   irons           P2 ROOM1151 (Elza): ECHTER Aktions-Druck an der Gespraechslage -> Scan-Stempel -> sub01 -> sub03:
 *                   Hinbeugen, nach den Sleeps 10+40+30 Text 20 (statt msg 2), msg 2 nie, Irons bleibt Clip 2 Bild 89,
 *                   danach Slot 2 = Text-Platz; zweites Anklicken -> nur Text 20; Leon ROOM1150 unveraendert (msg 2)
 *   flammenwerfer   P3 ROOM30E0: Panel -> Cursor Zelle 4 -> Zelle 5 -> sub04: Aufnahme-Modal 0x0E x100 statt msg 15;
 *                   Ja -> Inventar Zellen 0/1 = 0x0E x100, (3,0x9C) = 1; Nein -> nichts genommen, (3,0x9C) = 0, Panel
 *                   wieder scharf (sub09 startet erneut); volles Inventar -> wie Nein */
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
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_engine.h"
#include "re15_damage.h"     /* re15_player_set_equipped_weapon */
#include "re15_elza_r36.h"
#include "re15_kleinkram_r38.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);
extern int g_test_core_se_last, g_test_core_se_count;     /* tests/support/test_support.c */
extern int g_test_room_se_tick;

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ================================================ P1 Inventar ================================================== */

static void menu_frame(uint16_t pressed, uint16_t held)
{
    /* Reihenfolge wie test_inv_fsm.c frame(): Poll, FSM, EKG, Blende. */
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen()) re15_menu_fsm_tick(pressed, held);
    if (re15_menu_is_open()) re15_inv_screen_ecg_tick();
    re15_fade_tick();
}
static void menu_idle(int n) { while (n-- > 0) menu_frame(0, 0); }

/* bis zur Reiterwahl offen (Blende + Halten + Task-Start + Init), hoechstens 60 Bilder */
static int bis_offen(void)
{
    for (int i = 0; i < 60; i++) {
        if (re15_menu_is_open() && re15_menu_phase() == 1 && re15_menu_substate() == 0) return 1;
        menu_frame(0, 0);
    }
    return 0;
}
/* bis wieder im Spiel (Schliess-Phase + Einblenden), hoechstens 80 Bilder */
static int bis_zu(void)
{
    for (int i = 0; i < 80; i++) {
        if (!re15_menu_is_open() && !re15_menu_gameplay_frozen()) return 1;
        menu_frame(0, 0);
    }
    return 0;
}

static void teil_inventar(void)
{
    printf("== inventar ==\n");
    re15_fade_init();
    g_current_room_id = 0x1140;
    re15_inv_load_briefing();
    re15_inv_set_equipped_slot(0);
    re15_player_set_equipped_weapon(1);
    re15_kleinkram_r38_reset();

    /* (a) Oeffnen ueber START: Ton im Bild der Annahme (RE2 @0x800264e8 Annahme -> @0x8002652c-34 Satz 6) */
    int n0 = g_test_core_se_count;
    menu_frame(RE15_PAD_BIT_START, RE15_PAD_BIT_START);
    PRUEF(re15_menu_gameplay_frozen() && g_test_core_se_count == n0 + 1 && g_test_core_se_last == RE15_R38Q_INV_TON_AUF &&
          re15_kleinkram_r38_inv_ton_zaehler(RE15_R38Q_INV_TON_AUF) == 1,
          "START-Annahme: genau ein CORE-SE im selben Bild, Satz %d (soll %d), Zaehler %d",
          g_test_core_se_last, RE15_R38Q_INV_TON_AUF, re15_kleinkram_r38_inv_ton_zaehler(RE15_R38Q_INV_TON_AUF));
    int n1 = g_test_core_se_count;
    PRUEF(bis_offen() && g_test_core_se_count == n1, "Blende/Halten/Task-Start bis zur Reiterwahl: kein weiterer Ton");

    /* (b) Schliessen mit START in der Reiterwahl -> Satz 5 (RE2 Statustaste @0x8006a94c/@0x8006a968-6c) */
    n1 = g_test_core_se_count;
    menu_frame(RE15_PAD_BIT_START, RE15_PAD_BIT_START);
    PRUEF(re15_menu_phase() == 2 && g_test_core_se_count == n1 + 1 && g_test_core_se_last == RE15_R38Q_INV_TON_ZU,
          "START in der Reiterwahl: Schliess-Phase + Satz %d (soll %d)", g_test_core_se_last, RE15_R38Q_INV_TON_ZU);
    PRUEF(bis_zu(), "geschlossen, Spiel laeuft wieder");

    /* (c) Schliessen mit Kreuz (virtuelles Abbruch-Bit 0x8000) -> Satz 5 (RE2 @0x8006a928/@0x8006a930-34) */
    menu_frame(RE15_PAD_BIT_START, RE15_PAD_BIT_START);
    PRUEF(g_test_core_se_last == RE15_R38Q_INV_TON_AUF && bis_offen(), "zweites Oeffnen: Satz 6");
    n1 = g_test_core_se_count;
    menu_frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    PRUEF(re15_menu_phase() == 2 && g_test_core_se_count == n1 + 1 && g_test_core_se_last == RE15_R38Q_INV_TON_ZU,
          "Kreuz in der Reiterwahl: Satz %d", g_test_core_se_last);
    PRUEF(bis_zu(), "geschlossen");

    /* (d) EXIT bestaetigen -> Satz 5 statt des Auswahl-Tons 6 (RE2 Cursor 3 @0x8006a8d4 `lui a0,0x405`) */
    menu_frame(RE15_PAD_BIT_START, RE15_PAD_BIT_START);
    PRUEF(bis_offen(), "drittes Oeffnen");
    menu_frame(RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN);                 /* Reiter 2 = EXIT (@0x80049888-9c) */
    PRUEF(g_inv_screen.tab == 2, "Reiter EXIT (%d)", g_inv_screen.tab);
    n1 = g_test_core_se_count;
    menu_frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    PRUEF(re15_menu_phase() == 2 && g_test_core_se_count == n1 + 1 && g_test_core_se_last == RE15_R38Q_INV_TON_ZU,
          "EXIT: Schliess-Phase + Satz %d (soll %d)", g_test_core_se_last, RE15_R38Q_INV_TON_ZU);
    PRUEF(bis_zu(), "geschlossen");

    /* (e) andere Reiter behalten den Auswahl-Ton 6 (Nutzer-Ergaenzung 2026-08-31, unveraendert) */
    menu_frame(RE15_PAD_BIT_START, RE15_PAD_BIT_START);
    PRUEF(bis_offen(), "viertes Oeffnen");
    menu_frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP);                     /* Reiter 0 = ITEM */
    n1 = g_test_core_se_count;
    menu_frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    PRUEF(g_test_core_se_count == n1 + 1 && g_test_core_se_last == 6 && re15_menu_phase() == 1,
          "ITEM bestaetigen: Satz 6 wie bisher, kein Schliessen");
    re15_menu_toggle();                                               /* Debug-Schliessen (ohne Blende) */
    menu_idle(2);

    /* (f) Box-Anforderung: kein Oeffnen-Ton (RE2-Handler setzen 0x800df348 selbst -> @0x80026404 ueberspringt) */
    int auf_vorher = re15_kleinkram_r38_inv_ton_zaehler(RE15_R38Q_INV_TON_AUF);
    re15_menu_request_box();
    menu_idle(20);
    PRUEF(re15_kleinkram_r38_inv_ton_zaehler(RE15_R38Q_INV_TON_AUF) == auf_vorher,
          "Box-Anforderung: kein Oeffnen-Ton (Zaehler %d)", re15_kleinkram_r38_inv_ton_zaehler(RE15_R38Q_INV_TON_AUF));
    re15_menu_toggle();
    menu_idle(2);
}

/* ================================================ Raum-Rahmen (P2/P3) ========================================== */

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;
static int                s_bild = 0;

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

static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static uint8_t  s_bank_buf[2][0x60000];
static void bank_laden(uint8_t type, int buf)
{
    if (!s_ems) {
        char p2[600]; snprintf(p2, sizeof p2, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
        s_ems = slurp(p2, &s_ems_sz);
    }
    if (!s_ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, s_ems_sz, idx, &off, &len) != 0) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb || len > sizeof s_bank_buf[buf]) return;
    memcpy(s_bank_buf[buf], s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank_buf[buf], len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank_buf[buf], len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank_buf[buf], len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank_buf[buf], len, &eb->skel_own, &eb->anim_own) == 0);
}

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

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c: Item-Modal aktiv -> KEIN VM-Takt (main.c Zweig
 * re15_item_modal_active, Original g_pauseflags |= 0xff000000), Schritt, danach der Modal-Takt mit den virtuellen
 * Pad-Woertern (main.c re15_item_modal_tick nach re15_game_step). */
static void frame_pad(uint16_t pressed, uint16_t held)
{
    const unsigned char *raw; int len, id;
    g_test_room_se_tick = s_bild;
    if (!re15_item_modal_active()) scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
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
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = pressed;
    re15_game_step(&s_ctx);
    if (re15_item_modal_active())
        re15_item_modal_tick(re15_pad_virtual_word(pressed), re15_pad_virtual_word(held));
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
    s_bild++;
}
static void frame(void) { frame_pad(0, 0); }

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_inventory_t inv_vorher = g_inv;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    g_inv = inv_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    bank_laden(0x45, 0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    re15_elza_r36_reset();
    re15_kleinkram_r38_reset();
    s_bild = 0;
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* Datei-Offset des Programmzeigers eines Fadens (Slots 2.. = Ereignis-Faeden) innerhalb [lo, hi) oder -1. */
static int faden_in(unsigned lo, unsigned hi)
{
    for (int i = 0; i < SCD_THREAD_COUNT; i++) {
        const scd_thread_t *t = &g_scd.threads[i];
        if (!t->active || !t->pc) continue;
        unsigned o = (unsigned)(t->pc - s_rdt.raw);
        if (o >= lo && o < hi) return (int)o;
    }
    return -1;
}

/* ================================================ P2 Irons ROOM1151 ============================================ */

static void teil_irons(void)
{
    printf("== irons ==\n");
    re15_game_state_init();
    if (room_boot(0x1151, -20538, -25147, 1500, 5) != 0) return;
    PRUEF(g_re15_r38q_anker_n == 6, "ROOM1151: 6 Anker armiert (Signaturen @0x00EFA/F02/F0A/F12/F16/F72), %d",
          g_re15_r38q_anker_n);
    for (int f = 0; f < 30; f++) frame();
    const re15_aot_t *a = &g_aot.slots[RE15_ELZA_IRONS_SLOT];
    PRUEF(a->active && a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR, "vor der Szene: Slot 2 = Original-Platz sce 5");

    /* ECHTER Aktions-Druck (Viereck) an der Gespraechslage sub03 @0x00ECE/@0x00ED6 = (-20538,-25147) Gier 1500 */
    frame_pad(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    int druck = s_bild - 1;
    const re15_actor_t *ir = &g_actors[1];
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int start = -1, text = -1, msg2 = 0, irons_bewegt = 0, gebeugt_beim_text = -1, text_vor_szene = 0;
    for (int f = 0; f < 600; f++) {
        frame();
        if (start < 0 && faden_in(0x00EC2u, 0x00F74u) >= 0) start = s_bild - 1;
        if (start < 0 && g_scd.message_active) text_vor_szene = 1;
        if (text < 0 && g_scd.message_active && g_scd.message_id == RE15_ELZA_IRONS_MSG_ID) {
            text = s_bild - 1; gebeugt_beim_text = pl->motion;
        }
        if (g_scd.message_active && g_scd.message_id == RE15_R38Q_1151_MSG_ORIG) msg2 = 1;
        if (ir->motion != RE15_ELZA_IRONS_CLIP_TOT || ir->anim_frame != RE15_ELZA_IRONS_BILD_TOT || ir->sub_state_2 != 2)
            irons_bewegt = 1;
        if (start >= 0 && faden_in(0x00EC2u, 0x00F74u) < 0 && f > 450) break;
    }
    PRUEF(start >= 0 && start - druck <= 2, "Druck Bild %d -> sub03 laeuft ab Bild %d (Stempel -> sub01 -> Evt_exec)", druck, start);
    PRUEF(!text_vor_szene, "kein Text vor der Szene (Runde 36 zeigte Text 20 sofort)");
    PRUEF(text >= 0 && text - start == 80, "Text 20 in Bild %d = sub03-Start + 80 (Sleep 10 @0x00EDE + 40 @0x00EF0 + 30 @0x00EF6), "
          "gemessen +%d", text, text - start);
    PRUEF(gebeugt_beim_text == 0x0B, "Elza beim Text in Plc_motion(1,0x0B) @0x00EEC (Hinbeugen), motion %d", gebeugt_beim_text);
    PRUEF(!msg2, "msg 2 \"I'll be fine...\" in keinem Bild");
    PRUEF(!irons_bewegt, "Irons in allen Bildern Clip 2 Bild 89 Phase 2 (Gesten @0x00F02/0A/12/16 entfallen)");
    PRUEF(re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_IRONS_TEXT) == 1 && re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_IRONS_GESTE) == 4 &&
          re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_IRONS_ENDE) == 1,
          "Anker: Text %d, Gesten %d, Ende %d (soll 1/4/1)", re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_IRONS_TEXT),
          re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_IRONS_GESTE), re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_IRONS_ENDE));
    PRUEF(re15_game_flag_get(3, 0x9D) == 1 && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_ELZA_IRONS_MSG_ID,
          "nach der Szene: (3,0x9D)=1, Slot 2 = Text-Platz %d", a->event_id);

    /* Text schliessen, dann zweites Anklicken: nur Text 20, keine Szene */
    for (int f = 0; f < 40 && g_scd.message_active; f++) frame_pad(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE), frame();
    for (int f = 0; f < 60; f++) frame();
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = -20538; g_actors[RE15_ACTOR_SLOT_PLAYER].z = -25147;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 1500;
    frame_pad(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    int text2 = 0, szene2 = 0; msg2 = 0;
    for (int f = 0; f < 200; f++) {
        frame();
        if (g_scd.message_active && g_scd.message_id == RE15_ELZA_IRONS_MSG_ID) text2 = 1;
        if (g_scd.message_active && g_scd.message_id == RE15_R38Q_1151_MSG_ORIG) msg2 = 1;
        if (faden_in(0x00EC2u, 0x00F74u) >= 0) szene2 = 1;
    }
    PRUEF(text2 && !szene2 && !msg2 && re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_IRONS_TEXT) == 1,
          "zweites Anklicken: Text 20, keine Szene, kein msg 2");

    /* Leon ROOM1150 unveraendert: keine Anker, sub03 mit msg 2 (Irons lebt) */
    re15_game_state_init();
    if (room_boot(0x1150, -20538, -25147, 1500, 5) == 0) {
        PRUEF(g_re15_r38q_anker_n == 0, "Leon ROOM1150: keine Anker");
        for (int f = 0; f < 30; f++) frame();
        frame_pad(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
        int m2 = 0;
        for (int f = 0; f < 200; f++) {
            frame();
            if (g_scd.message_active && g_scd.message_id == 2) m2 = 1;
        }
        PRUEF(m2, "Leon ROOM1150: Original-Gespraech mit msg 2 laeuft weiter");
    }
}

/* ================================================ P3 Flammenwerfer ROOM30E0 ==================================== */

static int s_msg15 = 0;
static void spiel(int n, uint16_t held)
{
    for (int i = 0; i < n; i++) {
        frame_pad(i == 0 ? held : 0, held);
        if (g_scd.message_active && g_scd.message_id == 15) s_msg15 = 1;
    }
}
static void tipp(uint16_t b) { frame_pad(b, b); if (g_scd.message_active && g_scd.message_id == 15) s_msg15 = 1; }

/* Panel -> Zelle 4 (LINKS 24 Bilder: x -19554 - 24*200 = -24354, nur Slot 4) -> Viereck (sub10) -> RECHTS 4
 * (x -23554, Slot 4 und 5, letzter gewinnt = 5) -> Viereck (sub04). Gemessen am echten Lauf (Dossier Abschnitt 1). */
static void bis_sub04(void)
{
    tipp(RE15_PAD_BIT_SQUARE);             spiel(15, 0);
    spiel(24, RE15_PAD_BIT_LEFT);          spiel(15, 0);
    tipp(RE15_PAD_BIT_SQUARE);             spiel(30, 0);
    spiel(4, RE15_PAD_BIT_RIGHT);          spiel(15, 0);
    tipp(RE15_PAD_BIT_SQUARE);
}

/* Modal fahren: warten bis die Frage steht, optional RECHTS (= No), dann Viereck; hoechstens 400 Bilder */
static void modal_antwort(int nein)
{
    for (int i = 0; i < 400 && re15_item_modal_active(); i++) {
        if (re15_item_modal_prompt_ready()) {
            if (nein) { frame_pad(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT); frame(); nein = 0; continue; }
            frame_pad(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
            frame();
            continue;
        }
        frame();
    }
}

static int fw_zellen(int *menge)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == RE15_R38Q_FW_ITEM) { n++; if (menge) *menge = g_inv.slots[i].qty; }
    return n;
}

static void teil_flammenwerfer(void)
{
    printf("== flammenwerfer ==\n");
    /* --- Ja --- */
    re15_game_state_init();
    re15_inv_init();
    re15_inv_grant(0x03, 15);                               /* eine Pistole, Platz fuer die 2-Zellen-Waffe */
    if (room_boot(0x30E0, -25000, -26400, 2048, 0) != 0) return;
    PRUEF(g_re15_r38q_anker_n == 2, "ROOM30E0: 2 Anker (@0x00FEE/@0x00FFA), %d", g_re15_r38q_anker_n);
    PRUEF(g_aot.slots[RE15_R38Q_FW_PANEL_SLOT].active && g_aot.slots[RE15_R38Q_FW_PANEL_SLOT].type == RE15_AOT_TYPE_GENERIC,
          "Panel Slot 3 scharf ((3,0x9C)=0, main00 @0x00A5A)");
    for (int f = 0; f < 20; f++) frame();
    s_msg15 = 0;
    bis_sub04();
    int modal_bild = -1;
    for (int f = 0; f < 10 && modal_bild < 0; f++) { if (re15_item_modal_active()) modal_bild = f; else frame(); }
    uint8_t typ = 0; int wahl = 0;
    PRUEF(modal_bild >= 0 && re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_FW_MODAL) == 1, "sub04 @0x00FEE: Aufnahme-Modal statt msg 15");
    for (int i = 0; i < 80 && !re15_item_modal_prompt(&typ, &wahl); i++) frame();
    PRUEF(re15_item_modal_prompt(&typ, &wahl) == 1 && typ == RE15_R38Q_FW_ITEM,
          "Frage \"Will you take the ...\" (1) fuer Item 0x%02X (soll 0x0E), Vorwahl %s", typ, wahl ? "No" : "Yes");
    modal_antwort(0);
    for (int f = 0; f < 60; f++) { frame(); if (g_scd.message_active && g_scd.message_id == 15) s_msg15 = 1; }
    int menge = 0;
    PRUEF(!s_msg15, "msg 15 \"...not available in this preview.\" in keinem Bild");
    PRUEF(fw_zellen(&menge) == 2 && g_inv.slots[0].id == RE15_R38Q_FW_ITEM && g_inv.slots[0].flags == 1 &&
          g_inv.slots[1].flags == 2 && menge == RE15_R38Q_FW_MENGE,
          "Ja: Flammenwerfer in Zelle 0/1 (Flags 1/2), Menge %d (soll 100 = @0x80074e50)", menge);
    PRUEF(re15_game_flag_get(RE15_R38Q_FW_DONE_BANK, RE15_R38Q_FW_DONE_BIT) == 1 &&
          re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_FW_GENOMMEN) == 1 && re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_FW_BIT_UEBER) == 0,
          "Ja: (3,0x9C) = 1 (sub04 @0x00FFA wie im Original)");
    PRUEF(g_aot.slots[RE15_R38Q_FW_PANEL_SLOT].type == RE15_AOT_TYPE_NONE, "Ja: Panel bleibt weg (Aot_reset @0x00FE0)");
    /* Raum neu betreten: (3,0x9C) = 1 -> main00 baut das Panel nicht mehr auf */
    if (room_boot(0x30E0, -25000, -26400, 2048, 0) == 0) {
        for (int f = 0; f < 5; f++) frame();
        const re15_aot_t *q = &g_aot.slots[RE15_R38Q_FW_PANEL_SLOT];
        PRUEF(!(q->active && q->type == RE15_AOT_TYPE_GENERIC && q->event_id == (RE15_R38Q_FW_PANEL_P1 >> 8)),
              "Wiederbetreten: kein Panel (Gate @0x00A56): Slot 3 aktiv %d Typ %d sub %d, (3,0x9C)=%d", q->active, q->type,
              q->event_id, re15_game_flag_get(RE15_R38Q_FW_DONE_BANK, RE15_R38Q_FW_DONE_BIT));
    }

    /* --- Nein --- */
    re15_game_state_init();
    re15_inv_init();
    re15_inv_grant(0x03, 15);
    if (room_boot(0x30E0, -25000, -26400, 2048, 0) != 0) return;
    for (int f = 0; f < 20; f++) frame();
    s_msg15 = 0;
    bis_sub04();
    for (int f = 0; f < 10 && !re15_item_modal_active(); f++) frame();
    for (int i = 0; i < 80 && !re15_item_modal_prompt(&typ, &wahl); i++) frame();
    modal_antwort(1);
    for (int f = 0; f < 60; f++) { frame(); if (g_scd.message_active && g_scd.message_id == 15) s_msg15 = 1; }
    PRUEF(!s_msg15 && fw_zellen(NULL) == 0, "Nein: kein msg 15, kein Flammenwerfer im Inventar");
    PRUEF(re15_game_flag_get(RE15_R38Q_FW_DONE_BANK, RE15_R38Q_FW_DONE_BIT) == 0 &&
          re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_FW_LIEGT) == 1 && re15_kleinkram_r38_zaehler(RE15_R38Q_AKT_FW_BIT_UEBER) == 1,
          "Nein: (3,0x9C) bleibt 0 (Set @0x00FFA ausgelassen)");
    const re15_aot_t *p = &g_aot.slots[RE15_R38Q_FW_PANEL_SLOT];
    PRUEF(p->active && p->type == RE15_AOT_TYPE_GENERIC && p->event_id == (RE15_R38Q_FW_PANEL_P1 >> 8) &&
          (p->sce_flags & 0x7f) == RE15_R38Q_FW_PANEL_FLAGS,
          "Nein: Panel Slot 3 wieder sce 3 -> sub %d, Flags 0x%02X (main00 @0x00A5A)", p->event_id, p->sce_flags);
    /* erneut ans Panel: sub09 startet wieder (Cut_chg 2 @0x01038) */
    for (int f = 0; f < 30; f++) frame();
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = -25000; g_actors[RE15_ACTOR_SLOT_PLAYER].z = -26400;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 2048;
    tipp(RE15_PAD_BIT_SQUARE);
    int sub09 = 0;
    for (int f = 0; f < 10; f++) { if (faden_in(0x01038u, 0x01054u) >= 0 || g_scd.cam_id == 2) sub09 = 1; frame(); }
    PRUEF(sub09, "Nein: Viereck am Panel startet sub09 erneut (Cut 2) — der Flammenwerfer bleibt erreichbar");

    /* --- Inventar voll --- */
    re15_game_state_init();
    re15_inv_init();
    for (int i = 0; i < 10; i++) re15_inv_grant(0x15, 10);   /* 10 Plaetze belegt (Kapazitaet @0x800b0fbc = 10) */
    if (room_boot(0x30E0, -25000, -26400, 2048, 0) != 0) return;
    for (int f = 0; f < 20; f++) frame();
    bis_sub04();
    for (int f = 0; f < 10 && !re15_item_modal_active(); f++) frame();
    for (int i = 0; i < 80 && !re15_item_modal_prompt(&typ, &wahl); i++) frame();
    PRUEF(re15_item_modal_prompt(&typ, &wahl) == 2, "voll: \"You can't carry any more items\" (2)");
    modal_antwort(0);
    for (int f = 0; f < 60; f++) frame();
    PRUEF(fw_zellen(NULL) == 0 && re15_game_flag_get(RE15_R38Q_FW_DONE_BANK, RE15_R38Q_FW_DONE_BIT) == 0 &&
          g_aot.slots[RE15_R38Q_FW_PANEL_SLOT].type == RE15_AOT_TYPE_GENERIC,
          "voll: nichts genommen, (3,0x9C) = 0, Panel wieder scharf");
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "inventar")      || !strcmp(teil, "alle")) teil_inventar();
    if (!strcmp(teil, "irons")         || !strcmp(teil, "alle")) teil_irons();
    if (!strcmp(teil, "flammenwerfer") || !strcmp(teil, "alle")) teil_flammenwerfer();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
