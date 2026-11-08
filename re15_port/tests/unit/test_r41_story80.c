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
 *                  (Reihenfolge Wagen -> Fuehrerstand, Flags reisen mit); danach erneut vorne: kein Text, kein Nachtrag
 *   reihenfolge6030  Nachbesserung 1 (Abnahme 1 M1): Fuehrerstand VOR dem Wagen ((3,22)=0) -> msg 11, 31, 13..19, keine
 *                  Sherry-Zeile, (9,128)=1; erneut vorne vor dem Wagen: nichts; Wagen: 13 Zeilen widerspruchsfrei;
 *                  vorne nach dem Wagen: Nachtrag msg 40, 25, (9,128)=0; nochmals vorne: nichts. Jede Zeile hoechstens
 *                  einmal ueber den ganzen Weg. GEGENPROBE: Stand ad8532cd4 (Fuehrerstand ohne Gate) liefert vorne
 *                  msg 12/25 mit "Sherry" -> rot (in der echten exe: integration_r36_story_leon P4b).
 *   torweg6030     Integration A2 (Gegenpruefung Welle 2 Befund 2): Weg F vorne -> Wagen -> ohne Rueckkehr -> Tor ->
 *                  Fuehrerstand: Nachtrag 40, 25 (k_s80_6030_sub05_nachtrag_tor), dann beim naechsten Betreten der Zugstart
 *                  der Spur AN (27..30); Weg W: Zugstart sofort, kein Nachtrag. GEGENPROBE: Stand dabac8d22 -> 27..30 ohne
 *                  40/25 (exe-Lauf weg_F_vor)
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
#include "re15_story80_r41.h"
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
static int  s_prot_bild[PROT_N];
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
        s_prot_bild[s_prot_n] = s_bild;
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

/* ================================================ Nutzerliste ================================================== */
/* Jede neue/geaenderte Zeile mit Wortlaut (re15_msg_decode_text-Form: Zeilenumbruch = Leerzeichen) und Stimmen-Datei.
 * Quelle: Nutzerentscheide (analysis/story80/TEXTE_UMSETZUNG.md). Die Datei ist main<Id>.wav (audio_pc.c), es sei denn
 * stimmen_r39 leitet um — fuer diese Ids nicht (Teil stimmen). */
typedef struct { uint16_t raum; int id; const char *text; } zeile_t;
static const zeile_t k_liste[] = {
    { 0x30E1,  2, "John: This is bad... those monsters have us surrounded!" },
    { 0x30E1,  3, "John: ...Damn it, there's no place around here to hide..." },
    { 0x30C1,  1, "Elza: Who are you?" },
    { 0x30C1,  2, "Officer: Marvin Branagh, RPD." },
    { 0x30C1,  3, "Elza: Are there any others still alive?" },
    { 0x30C1,  4, "Marvin: A few. There are other survivors out there." },
    { 0x30C1,  5, "Elza: Then I hope they make it out." },
    { 0x6030,  0, "Leon: Hang on, Marvin. We're getting out of here." },
    { 0x6030,  1, "Leon: Who are you two?" },
    { 0x6030, 21, "John: Name's John. This is Sherry. We made it down here with a girl named Elza." },
    { 0x6030,  2, "Sherry: She's out there fighting the monsters!" },
    { 0x6030,  3, "John: She's trying to keep the outside clear so we can escape." },
    { 0x6030, 22, "John: She said she'd catch up with us." },
    { 0x6030,  4, "Leon: Why aren't you out there?" },
    { 0x6030,  5, "John: She told me to stay with the kid... and look after your friend here." },
    { 0x6030,  6, "Leon: Where's Annette?" },
    { 0x6030,  7, "John: The woman in the lab coat? Up front, trying to start the train." },
    { 0x6030, 23, "Sherry: A lab coat...? Is it Mom?!" },
    { 0x6030, 24, "John: Easy, kid. Stay with me." },
    { 0x6030,  8, "Leon: I'll find out. Stay here." },
    { 0x6030, 11, "Annette: Is everyone aboard?" },
    { 0x6030, 12, "Leon: Everyone we could find. There's a little girl in the back... Sherry." },
    { 0x6030, 25, "Annette: Sherry...? My daughter is on this train? ...Thank God." },
    { 0x6030, 26, "Leon: Can you start this thing?" },
    { 0x6030, 13, "Annette: Certainly." },
    /* Nachbesserung 1: Reihenfolge Fuehrerstand vor dem Wagen; beide ohne Aufnahme (stumm) */
    { 0x6030, RE15_S80_ID_VORNE,    "Leon: Everyone we could find. Can you start this thing?" },
    { 0x6030, RE15_S80_ID_NACHTRAG, "Leon: There's a little girl in the back... Sherry." },
};
#define LISTE_N ((int)(sizeof k_liste / sizeof k_liste[0]))

static const char *soll_text(uint16_t raum, int id)
{
    for (int i = 0; i < LISTE_N; i++) if (k_liste[i].raum == raum && k_liste[i].id == id) return k_liste[i].text;
    return NULL;
}

/* Zeilenbreite wie tools/r36_p/texte_bauen.py: sichtbare Glyphen je 0x08-Zeile, Kopf (Sprecher + ':' + ' ') zaehlt in
 * Zeile 1 mit (Steuercodes 04/05 mit Argument zeichnen nicht), Breite = include/font_width.h (DEBUG.BIN[0x4416+code]). */
#include "font_width.h"
static int max_zeilenbreite(const unsigned char *b, int n, int *zeilen)
{
    int w = 0, mx = 0, z = 1;
    for (int i = 0; i < n; i++) {
        const unsigned char c = b[i];
        if (c == 0x01) break;
        if (c == 0x02 || c == 0x04 || c == 0x05 || c == 0x06 || c == 0x09 || c == 0x0A || c == 0x0B) { i++; continue; }
        if (c == 0x03 || c == 0x07) continue;
        if (c == 0x08) { if (w > mx) mx = w; w = 0; z++; continue; }
        w += re15_font_width[c];
    }
    if (w > mx) mx = w;
    if (zeilen) *zeilen = z;
    return mx;
}

static void teil_texte(void)
{
    printf("== texte ==\n");
    static const uint16_t k_raeume[] = { 0x30E1, 0x30C1, 0x6030 };
    for (unsigned r = 0; r < sizeof k_raeume / sizeof k_raeume[0]; r++) {
        if (rdt_laden(k_raeume[r]) != 0) { PRUEF(0, "ROOM%04X ladbar", k_raeume[r]); continue; }
        g_current_room_id = k_raeume[r];
        re15_msg_clear_room_block();
        re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
        for (int i = 0; i < LISTE_N; i++) {
            if (k_liste[i].raum != k_raeume[r]) continue;
            const char *t = re15_msg_get_text(k_liste[i].id);
            int len = 0, z = 0;
            const unsigned char *raw = re15_msg_get_raw(k_liste[i].id, &len);
            const int px = raw ? max_zeilenbreite(raw, len, &z) : -1;
            PRUEF(t && strcmp(t, k_liste[i].text) == 0 && raw && len <= 128 && px > 0 && px <= 271 && z <= 3,
                  "ROOM%04X msg %2d (%d B, %d Zeilen, max %d px <= 271): \"%s\"", k_liste[i].raum, k_liste[i].id, len, z,
                  px, t ? t : "(fehlt)");
        }
    }
}

/* ================================================ Szenen ======================================================= */
static int faden_im_programm(const uint8_t *p, int n)
{
    for (int i = 0; i < SCD_THREAD_COUNT; i++) {
        const scd_thread_t *t = &g_scd.threads[i];
        if (t->active && t->pc >= p && t->pc < p + n) return i;
    }
    return -1;
}

/* Laeuft die Szene bis zum Ende des Programms (Faden verlaesst es) und vergleicht die Text- und Stimmen-Folge. */
static void szene_pruefen(int p, uint16_t raum, int max_bilder, int *bilder_out)
{
    int pn = 0, fn = 0;
    const uint8_t *prog = re15_story80_programm(p, &pn);
    const uint8_t *folge = re15_story80_folge(p, &fn);
    /* Ende = der Faden, der das Programm betrat, ist nicht mehr aktiv (Evt_end @Programmende). Gosub (sub04/05,
     * Warteschleifen) laeuft im selben Faden ausserhalb des Programms und zaehlt nicht als Ende. */
    int start = -1, ende = -1, slot = -1;
    for (int f = 0; f < max_bilder; f++) {
        frame();
        if (start < 0) { slot = faden_im_programm(prog, pn); if (slot >= 0) start = s_bild; }
        else if (!g_scd.threads[slot].active) { ende = s_bild; break; }
    }
    if (bilder_out) *bilder_out = (start >= 0 && ende >= 0) ? ende - start : -1;
    PRUEF(start >= 0 && ende > start, "ROOM%04X Programm %d lief von Bild %d bis %d (%d Bilder)", raum, p, start, ende,
          ende - start);
    printf("   Texte (%d):", s_prot_n);
    for (int i = 0; i < s_prot_n; i++) printf(" %d", s_prot_id[i]);
    printf("\n   Stimmen (%d):", s_stimme_n);
    for (int i = 0; i < s_stimme_n; i++) printf(" %d", s_stimme[i]);
    printf("\n");
    int folge_ok = (s_prot_n == fn), stimme_ok = (s_stimme_n == fn), text_ok = 1;
    for (int i = 0; i < fn && i < s_prot_n; i++) if (s_prot_id[i] != folge[i]) folge_ok = 0;
    for (int i = 0; i < fn && i < s_stimme_n; i++) if (s_stimme[i] != folge[i]) stimme_ok = 0;
    for (int i = 0; i < s_prot_n; i++) {
        const char *soll = soll_text(raum, s_prot_id[i]);
        printf("   %2d. Bild %4d msg %2d \"%s\"%s\n", i + 1, s_prot_bild[i], s_prot_id[i], s_prot_text[i],
               (soll && strcmp(soll, s_prot_text[i]) != 0) ? "  <- WEICHT AB" : "");
        if (soll && strcmp(soll, s_prot_text[i]) != 0) text_ok = 0;
    }
    PRUEF(folge_ok, "ROOM%04X: genau %d Zeilen in der Soll-Folge (gemessen %d)", raum, fn, s_prot_n);
    PRUEF(stimme_ok, "ROOM%04X: Stimmen-Ids = Message-Ids in derselben Folge (%d)", raum, s_stimme_n);
    PRUEF(text_ok, "ROOM%04X: jeder angezeigte Wortlaut = Nutzerliste", raum);
}

static void teil_ablauf30c1(void)
{
    printf("== ablauf30c1 ==\n");
    re15_game_state_init();
    re15_inv_init();
    re15_game_flag_set(3, 0x39, 1);   /* sub00 @0x00BEA Ck(3,0x39)=1 -> Sce_em_set Marvin, @0x00C02 Evt_exec sub03 */
    static const uint8_t k_banken[] = { 0x40 };
    for (int i = 0; i < RE15_S80_P_N; i++) g_re15_story80_umleitungen[i] = 0;
    if (room_boot(0x30C1, -6400, -400, 0, 1, k_banken, 1) != 0) return;
    int bilder = 0;
    szene_pruefen(RE15_S80_P_30C1, 0x30C1, 6000, &bilder);
    PRUEF(g_re15_story80_umleitungen[RE15_S80_P_30C1] == 1, "sub03 genau einmal umgeleitet (%u)",
          g_re15_story80_umleitungen[RE15_S80_P_30C1]);
    int msg678 = 0;
    for (int i = 0; i < s_prot_n; i++) if (s_prot_id[i] >= 6 && s_prot_id[i] <= 8) msg678 = 1;
    PRUEF(!msg678, "msg 6..8 (Leon/Ada/'tell them'/'See you soon') laufen nicht");
    for (int f = 0; f < 60; f++) frame();
    /* Marvins Abgang wie Original: sub03 @0x00E4C Cut_chg 3, @0x00E4E/@0x00E52 Sperren (2,7)/(1,27) wieder 0 */
    PRUEF(g_scd.cam_id == 3 && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27) && !re15_game_flag_get(3, 0x39),
          "nach der Szene: Cut %u (soll 3), Pad-Sperre (2,7)=%d, Letterbox (1,27)=%d, (3,0x39)=%d (sub03 @0x00C2C Set 0)",
          (unsigned)g_scd.cam_id, re15_game_flag_get(2, 7), re15_game_flag_get(1, 27), re15_game_flag_get(3, 0x39));
}

static void vorne_ruhig(const char *was);

static void teil_ablauf6030(void)
{
    printf("== ablauf6030 Wagen ==\n");
    re15_game_state_init();
    re15_inv_init();
    static const uint8_t k_banken[] = { 0x49, 0x40, 0x4B, 0x4D };   /* sub06 @0x0101C/@0x01030/@0x01044, sub07 @0x010D4 */
    for (int i = 0; i < RE15_S80_P_N; i++) g_re15_story80_umleitungen[i] = 0;
    /* Eintritt Cut 6 (sub00 @0x00D3C Case 6 -> Gosub sub06), Leon in der Marke Slot 5 (-19730,-25570,1600,6300) */
    if (room_boot(0x6030, -18930, -22420, 0, 6, k_banken, 4) != 0) return;
    int bilder = 0;
    const re15_actor_t *sherry = NULL;
    for (int i = 0; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == 0x4B) sherry = &g_actors[i];
    const int16_t sherry_rot0 = sherry ? sherry->rot_y : 0;
    szene_pruefen(RE15_S80_P_WAGEN, 0x6030, 9000, &bilder);
    PRUEF(g_re15_story80_umleitungen[RE15_S80_P_WAGEN] == 1, "sub02 genau einmal umgeleitet (%u)",
          g_re15_story80_umleitungen[RE15_S80_P_WAGEN]);
    PRUEF(sherry && sherry->rot_y != sherry_rot0, "Sherry hat sich zu Leon gedreht (Gier %d -> %d)", sherry_rot0,
          sherry ? sherry->rot_y : 0);
    for (int f = 0; f < 60; f++) frame();
    PRUEF(g_scd.cam_id == 12 && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27) && re15_game_flag_get(3, 22),
          "nach der Szene: Cut %u (soll 12, sub02 @0x00FE0), Sperren (2,7)=%d (1,27)=%d, (3,22)=%d (@0x00D7A)",
          (unsigned)g_scd.cam_id, re15_game_flag_get(2, 7), re15_game_flag_get(1, 27), re15_game_flag_get(3, 22));

    printf("== ablauf6030 Fuehrerstand (nach dem Wagen) ==\n");
    /* Flags reisen mit (room_boot behaelt g_game): (3,22)=1 aus der Wagen-Szene -> Programm sub08 wie bisher.
     * Eintritt Cut 0 (sub00 @0x00D2C Case 0 -> Gosub sub07; @0x010D0 Ck(4,23)=0 -> Annette + Evt_exec sub08) */
    if (room_boot(0x6030, -5000, -23000, 3072, 0, k_banken, 4) != 0) return;
    szene_pruefen(RE15_S80_P_PULT, 0x6030, 9000, &bilder);
    PRUEF(g_re15_story80_umleitungen[RE15_S80_P_PULT] == 1 && g_re15_story80_umleitungen[RE15_S80_P_PULT_VORNE] == 0,
          "sub08 genau einmal umgeleitet (%u), Fassung 'vorne zuerst' nie (%u)",
          g_re15_story80_umleitungen[RE15_S80_P_PULT], g_re15_story80_umleitungen[RE15_S80_P_PULT_VORNE]);
    PRUEF(re15_game_flag_get(4, 23) && !re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG),
          "(4,23)=1 nach der Szene (sub08 @0x0113A), (9,128)=%d (kein Nachtrag offen)",
          re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG));
    for (int f = 0; f < 60; f++) frame();
    printf("== ablauf6030 Fuehrerstand erneut ==\n");
    vorne_ruhig("Wagen zuerst, erneut vorne");
}

/* Fuehrerstand erneut betreten, wenn nichts mehr zu sagen ist: sub07 @0x010EC Else_ck (4,23)=1 -> @0x010F4 Ck(3,89)=0
 * -> Annette + Evt_exec sub09 (Pult-Haltung). Erwartet: kein Text, keine Umleitung, Spieler frei. */
static void vorne_ruhig(const char *was)
{
    static const uint8_t k_banken[] = { 0x49, 0x40, 0x4B, 0x4D };
    unsigned vorher[RE15_S80_P_N];
    for (int i = 0; i < RE15_S80_P_N; i++) vorher[i] = g_re15_story80_umleitungen[i];
    if (room_boot(0x6030, -5000, -23000, 3072, 0, k_banken, 4) != 0) return;
    for (int f = 0; f < 900; f++) frame();
    int umgeleitet = 0;
    for (int i = 0; i < RE15_S80_P_N; i++) if (g_re15_story80_umleitungen[i] != vorher[i]) umgeleitet = 1;
    PRUEF(s_prot_n == 0 && !umgeleitet && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27),
          "%s: kein Text (%d), keine Umleitung, Sperren (2,7)=%d (1,27)=%d", was, s_prot_n,
          re15_game_flag_get(2, 7), re15_game_flag_get(1, 27));
}

/* Ganzer Weg: gesehene Ids sammeln (jede Zeile hoechstens einmal) */
static int s_weg_ids[64];
static int s_weg_n = 0;
static void weg_merken(void)
{
    for (int i = 0; i < s_prot_n && s_weg_n < 64; i++) s_weg_ids[s_weg_n++] = s_prot_id[i];
}
static int prot_hat_wort(const char *w)
{
    for (int i = 0; i < s_prot_n; i++) if (strstr(s_prot_text[i], w)) return 1;
    return 0;
}

static void teil_reihenfolge6030(void)
{
    static const uint8_t k_banken[] = { 0x49, 0x40, 0x4B, 0x4D };   /* sub06 @0x0101C/@0x01030/@0x01044, sub07 @0x010D4 */
    int bilder = 0;
    printf("== reihenfolge6030: 1. Fuehrerstand vor dem Wagen ==\n");
    re15_game_state_init();
    re15_inv_init();
    for (int i = 0; i < RE15_S80_P_N; i++) g_re15_story80_umleitungen[i] = 0;
    s_weg_n = 0;
    PRUEF(!re15_game_flag_get(RE15_S80_WAGEN_BANK, RE15_S80_WAGEN_BIT), "Vorbedingung (3,22)=0 (Wagen-Szene nicht gelaufen)");
    if (room_boot(0x6030, -5000, -23000, 3072, 0, k_banken, 4) != 0) return;
    szene_pruefen(RE15_S80_P_PULT_VORNE, 0x6030, 9000, &bilder);
    weg_merken();
    PRUEF(!prot_hat_wort("Sherry") && !prot_hat_wort("daughter") && !prot_hat_wort("girl"),
          "vorne zuerst: keine Sherry-Zeile (kein 'Sherry'/'daughter'/'girl')");
    PRUEF(g_re15_story80_umleitungen[RE15_S80_P_PULT_VORNE] == 1 && g_re15_story80_umleitungen[RE15_S80_P_PULT] == 0,
          "sub08 -> Fassung 'vorne zuerst' (%u), volle Fassung nie (%u)",
          g_re15_story80_umleitungen[RE15_S80_P_PULT_VORNE], g_re15_story80_umleitungen[RE15_S80_P_PULT]);
    PRUEF(re15_game_flag_get(4, 23) && re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG) &&
          !re15_game_flag_get(RE15_S80_WAGEN_BANK, RE15_S80_WAGEN_BIT),
          "danach (4,23)=%d (sub08 @0x0113A), (9,128)=%d (Nachtrag offen), (3,22)=%d", re15_game_flag_get(4, 23),
          re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG),
          re15_game_flag_get(RE15_S80_WAGEN_BANK, RE15_S80_WAGEN_BIT));
    for (int f = 0; f < 60; f++) frame();
    PRUEF(!re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27), "Sperren (2,7)/(1,27) frei (sub08 @0x01368/@0x0136C)");

    printf("== reihenfolge6030: 2. erneut vorne, noch vor dem Wagen ==\n");
    vorne_ruhig("vorne vor dem Wagen, erneut");
    PRUEF(re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG), "(9,128) bleibt offen, solange (3,22)=0");

    printf("== reihenfolge6030: 3. Wagen ==\n");
    if (room_boot(0x6030, -18930, -22420, 0, 6, k_banken, 4) != 0) return;
    szene_pruefen(RE15_S80_P_WAGEN, 0x6030, 9000, &bilder);
    weg_merken();
    PRUEF(s_prot_n >= 3 && s_prot_id[1] == 1 && s_prot_id[2] == 21,
          "Wagen: 'Who are you two?' (msg 1), dann Johns Vorstellung (msg 21) - Leon kennt Sherry noch nicht");
    for (int f = 0; f < 60; f++) frame();
    PRUEF(re15_game_flag_get(RE15_S80_WAGEN_BANK, RE15_S80_WAGEN_BIT), "(3,22)=1 nach dem Wagen (sub02 @0x00D7A)");

    printf("== reihenfolge6030: 4. vorne nach dem Wagen: Nachtrag ==\n");
    if (room_boot(0x6030, -5000, -23000, 3072, 0, k_banken, 4) != 0) return;
    szene_pruefen(RE15_S80_P_NACHTRAG, 0x6030, 9000, &bilder);
    weg_merken();
    PRUEF(g_re15_story80_umleitungen[RE15_S80_P_NACHTRAG] == 1, "sub09 -> Nachtrag genau einmal (%u)",
          g_re15_story80_umleitungen[RE15_S80_P_NACHTRAG]);
    for (int f = 0; f < 60; f++) frame();
    PRUEF(!re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG) && !re15_game_flag_get(2, 7) &&
          !re15_game_flag_get(1, 27) && g_scd.cam_id == 0,
          "danach (9,128)=%d, Sperren (2,7)=%d (1,27)=%d, Cut %u (soll 0, sub08 @0x01336)",
          re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG), re15_game_flag_get(2, 7),
          re15_game_flag_get(1, 27), (unsigned)g_scd.cam_id);

    printf("== reihenfolge6030: 5. nochmals vorne ==\n");
    vorne_ruhig("nach dem Nachtrag, erneut vorne");

    int doppelt = 0;
    for (int i = 0; i < s_weg_n; i++)
        for (int k = i + 1; k < s_weg_n; k++) if (s_weg_ids[i] == s_weg_ids[k]) doppelt = s_weg_ids[i] + 1;
    printf("   ganzer Weg (%d Zeilen):", s_weg_n);
    for (int i = 0; i < s_weg_n; i++) printf(" %d", s_weg_ids[i]);
    printf("\n");
    PRUEF(!doppelt && s_weg_n == 9 + 13 + 2, "jede Zeile hoechstens einmal (%d Zeilen, doppelt: %d)", s_weg_n,
          doppelt - 1);
}

/* Integration A2 (Gegenpruefung Welle 2 Befund 2): Weg F = vorne zuerst -> Wagen -> OHNE Rueckkehr -> Tor -> Fuehrerstand.
 * Torschalter ROOM6000 sub07 (scd_dump_room.py): @0x011B4 `22 03 59 01` Set(3,89)=1, @0x011BC/@0x011C0 Ck(3,22)=0 ->
 * `22 03 16 01`, @0x011CA/@0x011CE Ck(4,23)=0 -> `22 04 17 01`. Danach ruft ROOM6030 sub07 nur noch @0x01132 Evt_exec sub05
 * (Zweig @0x01128 Ck(3,89)=1), der Nachtrag-Zweig @0x0110C (Ck(3,89)=0 @0x010F4) ist zu. Vor dem Fix (exe-Lauf weg_F_vor):
 * Zugstart msg 27..30 mit "Sherry is back there!" ohne msg 40/25, (9,128) blieb 1. Soll: Nachtrag 40, 25 beim ersten
 * Betreten nach dem Tor (k_s80_6030_sub05_nachtrag_tor), Zugstart 27..30 beim naechsten; jede Zeile hoechstens einmal. */
static int weg_pos(int id)
{
    for (int i = 0; i < s_weg_n; i++) if (s_weg_ids[i] == id) return i;
    return -1;
}
static int typ_da(uint8_t typ)
{
    for (int i = 0; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == typ) return 1;
    return 0;
}
static void torschalter(void)
{
    re15_game_flag_set(3, 89, 1);                                   /* ROOM6000 sub07 @0x011B4 */
    if (!re15_game_flag_get(3, 22)) re15_game_flag_set(3, 22, 1);   /* @0x011BC / @0x011C0 */
    if (!re15_game_flag_get(4, 23)) re15_game_flag_set(4, 23, 1);   /* @0x011CA / @0x011CE */
}
static void zugstart_laufen(int bilder)
{
    for (int f = 0; f < bilder; f++) frame();
    weg_merken();
}

static void teil_torweg6030(void)
{
    static const uint8_t k_banken[] = { 0x49, 0x40, 0x4B, 0x4D };
    int bilder = 0;
    printf("== torweg6030 F: 1. vorne zuerst ==\n");
    re15_game_state_init();
    re15_inv_init();
    for (int i = 0; i < RE15_S80_P_N; i++) g_re15_story80_umleitungen[i] = 0;
    s_weg_n = 0;
    if (room_boot(0x6030, -5000, -23000, 3072, 0, k_banken, 4) != 0) return;
    szene_pruefen(RE15_S80_P_PULT_VORNE, 0x6030, 9000, &bilder);
    weg_merken();
    for (int f = 0; f < 60; f++) frame();
    printf("== torweg6030 F: 2. Wagen, danach NICHT nach vorne ==\n");
    if (room_boot(0x6030, -18930, -22420, 0, 6, k_banken, 4) != 0) return;
    szene_pruefen(RE15_S80_P_WAGEN, 0x6030, 9000, &bilder);
    weg_merken();
    for (int f = 0; f < 60; f++) frame();
    PRUEF(re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG) && re15_game_flag_get(3, 22),
          "vor dem Tor: (9,128)=%d (Nachtrag offen), (3,22)=%d", re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG),
          re15_game_flag_get(3, 22));
    printf("== torweg6030 F: 3. Torschalter (ROOM6000 sub07 @0x011B4/@0x011C0/@0x011CE), Wagen-Eintritt ==\n");
    torschalter();
    unsigned vorher[RE15_S80_P_N];
    for (int i = 0; i < RE15_S80_P_N; i++) vorher[i] = g_re15_story80_umleitungen[i];
    if (room_boot(0x6030, -17970, -20380, 1100, 6, k_banken, 4) != 0) return;
    for (int f = 0; f < 300; f++) frame();
    int umgeleitet = 0;
    for (int i = 0; i < RE15_S80_P_N; i++) if (g_re15_story80_umleitungen[i] != vorher[i]) umgeleitet = 1;
    PRUEF(s_prot_n == 0 && !umgeleitet && re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG),
          "Wagen nach dem Tor (Cut 6, sub06 @0x010C4 -> sub05): kein Text (%d), keine ST80-Umleitung, (9,128) bleibt 1",
          s_prot_n);
    printf("== torweg6030 F: 4. Fuehrerstand nach dem Tor: Nachtrag ==\n");
    if (room_boot(0x6030, -5000, -23000, 3072, 0, k_banken, 4) != 0) return;
    szene_pruefen(RE15_S80_P_NACHTRAG_TOR, 0x6030, 9000, &bilder);
    weg_merken();
    PRUEF(g_re15_story80_umleitungen[RE15_S80_P_NACHTRAG_TOR] == 1 && g_re15_story80_umleitungen[RE15_S80_P_NACHTRAG] == 0,
          "sub07 @0x01132 -> Nachtrag nach dem Tor genau einmal (%u), sub09-Nachtrag nie (%u)",
          g_re15_story80_umleitungen[RE15_S80_P_NACHTRAG_TOR], g_re15_story80_umleitungen[RE15_S80_P_NACHTRAG]);
    for (int f = 0; f < 60; f++) frame();
    PRUEF(!re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG) && !re15_game_flag_get(9, 133) && typ_da(0x4D) &&
          !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27) && g_scd.cam_id == 0,
          "danach (9,128)=%d, (9,133)=%d (Zugstart noch nicht), Annette 0x4D am Pult %d (Sce_em_set @0x010F8), Sperren "
          "(2,7)=%d (1,27)=%d, Cut %u", re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG),
          re15_game_flag_get(9, 133), typ_da(0x4D), re15_game_flag_get(2, 7), re15_game_flag_get(1, 27),
          (unsigned)g_scd.cam_id);
    printf("== torweg6030 F: 5. Fuehrerstand erneut: Zugstart ==\n");
    if (room_boot(0x6030, -5000, -23000, 3072, 0, k_banken, 4) != 0) return;
    zugstart_laufen(2400);
    PRUEF(re15_game_flag_get(9, 133) && g_re15_story80_umleitungen[RE15_S80_P_NACHTRAG_TOR] == 1,
          "Zugstart gelaufen (9,133)=%d, kein zweiter Nachtrag (%u)", re15_game_flag_get(9, 133),
          g_re15_story80_umleitungen[RE15_S80_P_NACHTRAG_TOR]);
    printf("   ganzer Weg F (%d Zeilen):", s_weg_n);
    for (int i = 0; i < s_weg_n; i++) printf(" %d", s_weg_ids[i]);
    printf("\n");
    int doppelt = 0;
    for (int i = 0; i < s_weg_n; i++)
        for (int k = i + 1; k < s_weg_n; k++) if (s_weg_ids[i] == s_weg_ids[k]) doppelt = s_weg_ids[i] + 1;
    PRUEF(!doppelt && s_weg_n == 9 + 13 + 2 + 4, "jede Zeile hoechstens einmal (%d Zeilen, doppelt: %d)", s_weg_n, doppelt - 1);
    PRUEF(weg_pos(25) >= 0 && weg_pos(40) >= 0 && weg_pos(28) > weg_pos(25) && weg_pos(25) > weg_pos(40) &&
          weg_pos(27) > weg_pos(25),
          "Annette erfaehrt von Sherry (40 @%d, 25 @%d) vor dem Zugstart (27 @%d, 28 '...Sherry is back there!' @%d)",
          weg_pos(40), weg_pos(25), weg_pos(27), weg_pos(28));

    printf("== torweg6030 W: Wagen zuerst, Fuehrerstand, Tor, Fuehrerstand: Zugstart sofort, kein Nachtrag ==\n");
    re15_game_state_init();
    re15_inv_init();
    for (int i = 0; i < RE15_S80_P_N; i++) g_re15_story80_umleitungen[i] = 0;
    s_weg_n = 0;
    re15_game_flag_set(3, 22, 1); re15_game_flag_set(4, 23, 1);   /* sub02 @0x00D7A, sub08 @0x0113A (volle Fassung) */
    torschalter();
    if (room_boot(0x6030, -5000, -23000, 3072, 0, k_banken, 4) != 0) return;
    zugstart_laufen(2400);
    PRUEF(re15_game_flag_get(9, 133) && g_re15_story80_umleitungen[RE15_S80_P_NACHTRAG_TOR] == 0 && weg_pos(40) < 0 &&
          weg_pos(27) == 0 && s_weg_n == 4,
          "W: Zugstart beim ersten Betreten (%d Zeilen, erste %d), kein Nachtrag (%u), (9,133)=%d", s_weg_n,
          s_weg_n ? s_weg_ids[0] : -1, g_re15_story80_umleitungen[RE15_S80_P_NACHTRAG_TOR], re15_game_flag_get(9, 133));
}

static void teil_stimmen(void)
{
    printf("== stimmen ==\n");
    for (int i = 0; i < LISTE_N; i++) {
        char st[24] = "";
        const int r = re15_stimmen_r39_stamm(k_liste[i].raum, k_liste[i].id, st, sizeof st);
        const char *sd = re15_elza_stimme_datei(k_liste[i].raum, k_liste[i].id);
        const int stumm = k_liste[i].raum == 0x6030 &&
                          (k_liste[i].id == RE15_S80_ID_VORNE || k_liste[i].id == RE15_S80_ID_NACHTRAG);
        PRUEF(r == 0 && sd == NULL && k_liste[i].id < 64,
              "synchro/STAGE%u/room%04X/main%02d.wav (keine Umleitung, Id < VOICE_MAX_MSG 64)%s",
              (unsigned)(k_liste[i].raum >> 12), k_liste[i].raum, k_liste[i].id,
              stumm ? " - keine Aufnahme geliefert, Zeile bleibt stumm (NB1)" : "");
    }
}

/* ================================================ main ========================================================= */
int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "hebetisch1151") || !strcmp(teil, "alle")) { teil_hebetisch1151(0x1151); teil_hebetisch1151(0x1150); }
    if (!strcmp(teil, "texte")         || !strcmp(teil, "alle")) teil_texte();
    if (!strcmp(teil, "ablauf30c1")    || !strcmp(teil, "alle")) teil_ablauf30c1();
    if (!strcmp(teil, "ablauf6030")    || !strcmp(teil, "alle")) teil_ablauf6030();
    if (!strcmp(teil, "reihenfolge6030") || !strcmp(teil, "alle")) teil_reihenfolge6030();
    if (!strcmp(teil, "torweg6030")    || !strcmp(teil, "alle")) teil_torweg6030();
    if (!strcmp(teil, "stimmen")       || !strcmp(teil, "alle")) teil_stimmen();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
