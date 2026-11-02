/* test_r41_story80.c — RIEGEL Runde 41 Spur ST80-T "Story-Texte umsetzen" (Nutzerentscheide aus dem Story-Abgleich).
 *
 * Dossier: analysis/story80/TEXTE_UMSETZUNG.md. Texte: tools/r36_p/texte_bauen.py -> engine/src/gen/story_r36_texte.inc;
 * Szenen: tools/r41_story80/szenen_bauen.py -> engine/src/gen/story80_szenen.inc (engine/src/story80_r41.c).
 * Teile (je ein ctest-Eintrag, registriert in tests/unit/probes/r41_story80.cmake):
 *   hebetisch1151  f.2: ROOM1151 nach der Hinbeuge-Szene (3,0x9D)=1: Fehldruck am Hebetisch ("Nothing happened."),
 *                  Cursor mit CROSS verlassen, Irons anklicken -> der angezeigte Wortlaut muss "He has already passed..."
 *                  sein (Messung des Fehlers: vor der Behebung stand dort "Nothing happened."). Gegenprobe 1150 (Leon).
 *   texte          jede Zeile der Nutzerliste: Raum, Id, Sprecherkopf, Wortlaut nach dem Laden, Zeilenbreite
 *   ablauf30c1     ROOM30C1 (Elza trifft Marvin): genau fuenf Zeilen 1..5 in dieser Reihenfolge, Stimmen-Ids = Message-Ids,
 *                  msg 6..8 nie; Marvins Abgang (sub06, Cut 3, Spieler frei) wie bisher
 *   ablauf6030     ROOM6030 (Leon): Wagen-Szene sub02 dreizehn Zeilen, Fuehrerstand sub08 a..d vor "Certainly."
 *   stimmen        Stimmen-Pfade: keine Umleitung in stimmen_r39 fuer die neuen/umgeschriebenen Ids (Datei = main<Id>.wav)
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
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_engine.h"
#include "re15_elza_r36.h"
#include "re15_kleinkram_r38.h"
#include "re15_hebetisch_cursor.h"
#include "re15_story_r36.h"
#include "re15_stimmen_r39.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ================================================ Geruest (Muster test_r38_kleinkram.c) ======================== */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;
static int                s_bild = 0;

/* Protokoll je Bild: neu geoeffnete Texte (Id + Wortlaut) und eingereihte Stimmen. */
#define PROT_N 64
static int  s_prot_id[PROT_N];
static char s_prot_text[PROT_N][192];
static int  s_prot_n = 0;
static int  s_stimme[PROT_N];
static int  s_stimme_n = 0;
static int  s_offen_vorher = 0;
static int  s_id_vorher = -1;

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
static uint8_t  s_bank_buf[4][0x60000];
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

static void protokoll(void)
{
    const int offen = g_scd.message_active ? 1 : 0;
    const int id = offen ? (int)g_scd.message_id : -1;
    if (offen && (!s_offen_vorher || id != s_id_vorher) && s_prot_n < PROT_N) {
        const char *t = re15_msg_get_text(id);
        s_prot_id[s_prot_n] = id;
        snprintf(s_prot_text[s_prot_n], sizeof s_prot_text[0], "%s", t ? t : "");
        s_prot_n++;
    }
    s_offen_vorher = offen; s_id_vorher = id;
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (wie test_r38_kleinkram.c frame_pad). */
static void frame_pad(uint16_t pressed, uint16_t held)
{
    const unsigned char *raw; int len, id;
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
    protokoll();
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e))
        if (e.kind == SCD_AUDIO_VOICE_ON && s_stimme_n < PROT_N) s_stimme[s_stimme_n++] = (int)e.sample_id;
    s_bild++;
}
static void frame(void) { frame_pad(0, 0); }

static void prot_leeren(void) { s_prot_n = 0; s_stimme_n = 0; s_offen_vorher = 0; s_id_vorher = -1; }

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut, const uint8_t *banken, int nb)
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
    for (int i = 0; i < nb && i < 4; i++) bank_laden(banken[i], i);
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
    prot_leeren();
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* ================================================ f.2 Hebetisch / Irons ROOM1151 =============================== */
static const char *const k_irons_wortlaut = "He has already passed...";
static const char *const k_nichts_wortlaut = "Nothing happened.";

static int text_zu(int max)
{
    /* Muster probe_r34n_b_cursor.c text_schliessen: bis END_WAIT (4) warten, Flanke, dann gehalten. */
    for (int f = 0; f < max && g_scd.message_active && g_scd.message_fsm != 4 && g_scd.message_fsm != 1; f++) frame();
    if (!g_scd.message_active) return 1;
    frame_pad(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    frame_pad(0, RE15_PAD_BIT_SQUARE);
    frame_pad(0, RE15_PAD_BIT_SQUARE);
    for (int f = 0; f < 5; f++) frame();
    if (g_scd.message_active) printf("   Text offen: fsm %d id %d\n", (int)g_scd.message_fsm, (int)g_scd.message_id);
    return !g_scd.message_active;
}

static void teil_hebetisch1151(uint16_t raum)
{
    printf("== hebetisch ROOM%04X ==\n", raum);
    re15_game_state_init();
    re15_inv_init();
    if (raum == 0x1151) re15_game_flag_set(3, 0x9D, 1);   /* Hinbeuge-Szene gesehen: sub01 @0x00EA8 Set(3,0x9D) */
    static const uint8_t k_banken[] = { 0x45 };
    if (room_boot(raum, -22250, -18500, 0, 5, k_banken, 1) != 0) return;
    for (int f = 0; f < 30; f++) frame();
    const re15_aot_t *a = &g_aot.slots[RE15_ELZA_IRONS_SLOT];
    if (raum == 0x1151)
        PRUEF(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_ELZA_IRONS_MSG_ID,
              "ROOM1151 (3,0x9D)=1: Irons-Platz Slot %d = Text %d", RE15_ELZA_IRONS_SLOT, a->event_id);

    /* 1. Hebetisch: Aktion am Tisch (Ereignis 4, wie game_step_common.c GENERIC), Cursor, Fehldruck. */
    int slot = scd_event_fire(RE15_HC_EREIGNIS);
    re15_hebetisch_cursor_aktion(RE15_HC_EREIGNIS, slot);
    int aktiv = -1;
    for (int f = 0; f < 60; f++) { frame(); if (re15_hebetisch_cursor_zustand() == RE15_HC_AKTIV) { aktiv = f; break; } }
    PRUEF(aktiv >= 0, "Hebetisch-Cursor aktiv nach %d Bildern", aktiv);
    prot_leeren();
    frame_pad(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    for (int f = 0; f < 5; f++) frame();
    const int heb_id = s_prot_n > 0 ? s_prot_id[0] : -1;
    PRUEF(s_prot_n >= 1 && strcmp(s_prot_text[0], k_nichts_wortlaut) == 0,
          "Fehldruck: Text Id %d \"%s\"", heb_id, s_prot_n ? s_prot_text[0] : "");
    PRUEF(text_zu(400), "Fehldruck-Text geschlossen");
    frame_pad(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    for (int f = 0; f < 30; f++) frame();
    PRUEF(re15_hebetisch_cursor_zustand() == RE15_HC_AUS, "Cursor mit CROSS verlassen (Aufraeumbytes)");

    /* 2. Irons anklicken: echte Aktion an der Gespraechslage (sub03 @0x00ECE/@0x00ED6, wie test_r38_kleinkram.c). */
    for (int f = 0; f < 40; f++) frame();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -20538; pl->z = -25147; pl->rot_y = 1500;
    prot_leeren();
    frame_pad(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    for (int f = 0; f < 200 && s_prot_n == 0; f++) frame();
    for (int i = 0; i < s_prot_n; i++) printf("   Text nach Irons-Klick: Id %d \"%s\"\n", s_prot_id[i], s_prot_text[i]);
    if (raum == 0x1151) {
        PRUEF(s_prot_n >= 1 && s_prot_id[0] == RE15_ELZA_IRONS_MSG_ID && strcmp(s_prot_text[0], k_irons_wortlaut) == 0,
              "Irons nach dem Fehldruck: Id %d \"%s\" (soll %d \"%s\")", s_prot_n ? s_prot_id[0] : -1,
              s_prot_n ? s_prot_text[0] : "", RE15_ELZA_IRONS_MSG_ID, k_irons_wortlaut);
        PRUEF(heb_id >= 0 && heb_id != RE15_ELZA_IRONS_MSG_ID, "Hebetisch-Id %d != Irons-Id %d", heb_id, RE15_ELZA_IRONS_MSG_ID);
    } else {
        /* Leon 1150: Irons lebt, sein Original-Gespraech (msg 2) laeuft wie bisher, kein "Nothing happened." */
        int nichts = 0;
        for (int i = 0; i < s_prot_n; i++) if (strcmp(s_prot_text[i], k_nichts_wortlaut) == 0) nichts = 1;
        PRUEF(!nichts, "Leon 1150: nach dem Fehldruck kein \"Nothing happened.\" am Irons-Platz");
    }
}

/* ================================================ main ========================================================= */
int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "hebetisch1151") || !strcmp(teil, "alle")) { teil_hebetisch1151(0x1151); teil_hebetisch1151(0x1150); }
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
