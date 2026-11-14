/*
 * test_r42_story2.c — Runde 42 "story" (v0.8.31), Stufe STAGE2: Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit allen
 * Installern, SCD-VM, re15_game_step), Harness-Muster test_r41_gatorszene.c. Konstanten/Belege include/re15_story2_r42.h, Code
 * engine/src/story2_r42.c. PORT-STORY nach dem Konzept (3.2 L1/L1b, 4.2 E1, 6b; K-01..K-03, K-07, K-08).
 *
 * Teile (Aufruf: test_r42_story2 <teil>):
 *   texte      2070/2071 msg 1 = ROOM11A0 msg 2 @0x0142C bytegleich; jede Zeile der Szene hat Text (Sprecher-Kopf), Uhr-Text Leon
 *   programme  Szenen-Programme: Zeilen 43..48 (Leon) / 43..51 (Elza) in Reihenfolge, Set(9,118,1), Ende Evt_end
 *   ablass     K-01: Rechner 2080 (Original-Frage msg 0, Ja) -> (3,245) = 1 und danach der Hinweis msg 52
 *   loch       K-01: Schluesselloch 2080 ohne Schluessel msg 53 + Verschlossen-Ton; mit Schluessel msg 54 + (9,113)/(9,119),
 *              Szene L1 laeuft (Marvin), Bit 118; Wiederbetreten: Marvin wartet, kein Schluesselloch, keine zweite Szene
 *   loch_elza  dasselbe mit Elza (2081): John + Sherry, Bit 118
 *   tor2090    K-01: das Tor 2090 nimmt den Schluessel nicht mehr (msg 42, (9,113) bleibt 0); nach (9,113) offen
 *   tueren     K-02/K-03: 2070 (Leon) beide Tueren "locked from the other side"; 11A0 schliesst auf (msg 11, (3,151), Bit 121),
 *              danach Durchgang nach 2070; 2040 schliesst auf (msg 6, Bit 122), danach Durchgang nach 2070; 2070 danach offen
 *   tueren_elza dasselbe mit 2071/11A1/2041
 *   uhr        K-08: vor dem Tod keine Uhr; nach dem Tod Uhr-Zone (Platz 16, Item 0x3F, Bit 90) auf dem Schluessel-Rechteck, Puls;
 *              Beschreibung 0x3F: Leon mit Gravur (F-19), Elza Original; Modell-Lage = re15_gatorszene_r41_neben_lage (im Bild der
 *              Schluessel-Kamera, nicht der alte Versatz laengs x; Pruefung 1 Befund 6)
 *   altstand   Pruefung 1 Befund 5: Spielstand v0.8.30 mit schon geoeffnetem Tor (9,113) = 1, (9,119) = 0 -> kein Schluesselloch
 *              (kein msg 53/54), Bit 119 nachgezogen, Druck am Rechner startet die Szene einmal
 *   begleiter  Pruefung 1 Befund 8 (K-07): 11A0 Marvin (@0x0118C) / 11A1 John (@0x0118C) + Sherry (@0x011A0) fehlen, solange sie in
 *              2080/2081 warten (Bit 118 = 1, Bit 120 = 0); ohne Bit 118 und nach Bit 120 unveraendert
 *   inszenierung Pruefung 1 Befund 1/4: Endlagen der Szene (Spieler P, Begleiter N, Sherry S) und die Reihenfolge Begleiter ->
 *              Spieler -> Drehen; Begleiter von Suedwesten, Sherry aus der Nische an der Treppe (re15_story2_r42.h)
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
#include "re15_gameflow.h"
#include "re15_blink_r41.h"
#include "re15_light.h"
#include "re15_lock_se.h"
#include "re15_gatorszene_r41.h"
#include "re15_story2_r42.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);
const uint8_t *re15_inv_desc_entry(int id);
extern int g_test_door_se_count;   /* tests/test_support.c: Spion re15_audio_re2_door_se */
extern int g_test_door_se_last;

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
typedef struct { int msg_id, raumwechsel, ziel; } druck_t;
static druck_t druecke(int32_t x, int32_t z, int16_t rot, int bilder)
{
    druck_t r = { -1, 0, -1 };
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = x; pl->z = z; pl->rot_y = rot;
    frame(0, 0);
    g_room_change.pending = 0;
    for (int f = 0; f < bilder; f++) {
        uint16_t e = 0, h = 0;
        if (f == 0) e = RE15_PAD_BIT_SQUARE;
        if (f < 12) h = RE15_PAD_BIT_SQUARE;   /* Tuer: 9 Bilder Halten (aot_common.c door_hold, FUN_8002bd44) */
        if (f >= 40 && (f % 15) == 0 && g_scd.message_active) e = h = RE15_PAD_BIT_SQUARE;
        frame(h, e);
        if (g_scd.message_active && r.msg_id < 0) r.msg_id = (int)g_scd.message_id;
        if (g_room_change.pending) { r.raumwechsel = 1; r.ziel = (int)g_room_change.room_id; break; }
    }
    return r;
}
static void nachricht_weg(int bilder)
{
    for (int f = 0; f < bilder; f++) { uint16_t e = (g_scd.message_active && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
}
static int inv_geben(uint8_t id)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == 0) { g_inv.slots[i].id = id; g_inv.slots[i].qty = 1; return i; }
    return -1;
}
static int typ_zahl(uint8_t typ)
{
    int n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == typ) n++;
    return n;
}
static int bit9(int b) { return re15_game_flag_get(9, (uint8_t)b) ? 1 : 0; }

/* ---- texte ------------------------------------------------------------------------------------------------- */
static void teil_texte(void)
{
    printf("== texte ==\n");
    size_t n = 0;
    uint8_t *d = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM11A0.RDT", &n);
    int l = 0;
    const uint8_t *t = re15_story2_r42_text(0x2070, 1, &l);
    PRUEF(d && t && l > 4 && n > 0x0142C + (size_t)l && !memcmp(d + 0x0142C, t, (size_t)l),
          "2070 msg 1 = ROOM11A0 msg 2 @0x0142C bytegleich (%d B)", l);
    free(d);
    const uint8_t *t2 = re15_story2_r42_text(0x2071, 1, &l);
    PRUEF(t2 == t || (t2 && !memcmp(t2, t, (size_t)l)), "2071 msg 1 = 2070 msg 1");
    static const struct { uint16_t r; int von, bis; } k[] = { { 0x2080, 43, 54 }, { 0x2081, 43, 54 } };
    for (int i = 0; i < 2; i++)
        for (int id = k[i].von; id <= k[i].bis; id++) {
            if (k[i].r == 0x2080 && id >= 49 && id <= 51) { PRUEF(!re15_story2_r42_text(0x2080, (uint8_t)id, NULL), "2080 msg %d gibt es nicht (nur Elza)", id); continue; }
            const uint8_t *b = re15_story2_r42_text(k[i].r, (uint8_t)id, &l);
            const int npc = b && l > 4 && b[0] == 0x04 && b[1] == 0x00 && b[2] == 0x05 && !memcmp(b + l - 4, "\x04\x01\x01\x63", 4);
            const int sys = b && l > 4 && b[0] == 0x04 && b[1] == 0x02 && b[l - 2] == 0x01 && b[l - 1] == 0x00;
            PRUEF(npc || sys, "ROOM%04X msg %d: %s", k[i].r, id, npc ? "Untertitel 04 00 05 cc .. 04 01 01 63" : sys ? "Systemtext 04 02 .. 01 00" : "FEHLT");
        }
    for (int i = 0; i < 6; i++) {
        static const uint16_t r[6] = { 0x2090, 0x2091, 0x11A0, 0x11A1, 0x2040, 0x2041 };
        static const uint8_t id[6] = { 47, 47, 11, 11, 6, 6 };
        PRUEF(re15_story2_r42_text(r[i], id[i], NULL) != NULL, "ROOM%04X msg %d vorhanden", r[i], id[i]);
    }
}

/* ---- programme ----------------------------------------------------------------------------------------------- */
static void teil_programme(void)
{
    printf("== programme ==\n");
    for (int e = 0; e < 2; e++) {
        int n = 0;
        const uint8_t *p = re15_story2_r42_programm(e, &n);
        int folge[16], nf = 0, bit118 = 0;
        for (int i = 0; i + 3 < n; i++) {
            if (p[i] == 0x2b && p[i + 2] == 0x00 && p[i + 3] == 0x00 && p[i + 1] >= 43 && p[i + 1] <= 51 && nf < 16) folge[nf++] = p[i + 1];
            if (p[i] == 0x22 && p[i + 1] == 9 && p[i + 2] == RE15_R42_BIT_2080_SZENE && p[i + 3] == 1) bit118 = 1;
        }
        const int soll = e ? 9 : 6;
        int ok = nf == soll;
        for (int i = 0; ok && i < nf; i++) ok = folge[i] == 43 + i;
        PRUEF(ok, "%s: %d Zeilen 43..%d in Reihenfolge (gezaehlt %d)", e ? "Elza" : "Leon", soll, 42 + soll, nf);
        PRUEF(bit118, "%s: Set(9,118,1) im Programm", e ? "Elza" : "Leon");
        PRUEF(n >= 2 && p[n - 2] == 0x01 && p[n - 1] == 0x00, "%s: Ende Evt_end `01 00`", e ? "Elza" : "Leon");
    }
}

/* ---- ablass ------------------------------------------------------------------------------------------------- */
static void teil_ablass(int elza)
{
    printf("== ablass%s ==\n", elza ? "_elza" : "");
    re15_game_state_init();
    re15_inv_load_briefing();
    const uint16_t raum = elza ? 0x2081 : 0x2080;
    if (room_boot(raum, -11160, -14400, -7316, 3072, 3) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    druecke(-11160, -7316, 3072, 60);
    for (int f = 0; f < 1500 && !(s_msg_gesehen[52] && !g_scd.message_active); f++) {
        uint16_t e = (g_scd.message_active && (f % 20) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
    PRUEF(s_msg_gesehen[0], "Original-Frage msg 0 (sub02 @0x0C86)");
    PRUEF(re15_game_flag_get(3, 245), "(3,245) = 1 (Ja, Wasser abgelassen, @0x0C94)");
    PRUEF(s_msg_gesehen[52], "danach der Hinweis msg 52 \"Next to the computer... there's a keyhole.\"");
}

/* ---- loch ---------------------------------------------------------------------------------------------------- */
static void teil_loch(int elza)
{
    printf("== loch%s ==\n", elza ? "_elza" : "");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(3, 245, 1);
    re15_game_flag_set(9, 110, 1); re15_game_flag_set(9, 111, 1); re15_game_flag_set(9, 112, 1);
    re15_game_flag_set(9, 114, 1); re15_game_flag_set(9, 115, 1);
    const uint16_t raum = elza ? 0x2081 : 0x2080;
    if (room_boot(raum, -11160, -14400, -7316, 3072, 3) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    PRUEF(g_aot.slots[4].active && g_aot.slots[4].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[4].event_id == RE15_S2_EREIGNIS,
          "Schluesselloch = Platz 4 (Rechteck @0x0C1A), Ereignis %d", RE15_S2_EREIGNIS);
    int se0 = g_test_door_se_count;
    druck_t r = druecke(-11160, -7316, 3072, 60);
    PRUEF(r.msg_id == RE15_R42_MSG_2080_TOR_ZU && !bit9(113), "ohne Schluessel: msg %d (soll 53), (9,113) = 0", r.msg_id);
    PRUEF(g_test_door_se_count - se0 == 1 && g_test_door_se_last == re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M),
          "ein Verschlossen-Ton Art M (RE2 @0x800516a0/a4)");
    nachricht_weg(120);
    inv_geben(RE15_S2_KEY_ITEM);
    se0 = g_test_door_se_count;
    r = druecke(-11160, -7316, 3072, 40);
    PRUEF(r.msg_id == RE15_R42_MSG_2080_TOR_AUF, "mit Schluessel: msg %d (soll 54 \"%s used the key.\")", r.msg_id, elza ? "Elza" : "Leon");
    PRUEF(bit9(113) && bit9(RE15_R42_BIT_2080_TOR), "(9,113) Tor 2090<->20A0 und (9,%d) gesetzt", RE15_R42_BIT_2080_TOR);
    PRUEF(g_test_door_se_count - se0 >= 1, "Aufschliess-Ton (RE2 @0x80051654/58)");
    int frage = 0;
    for (int f = 0; f < 4000 && !bit9(RE15_R42_BIT_2080_SZENE); f++) {
        if (re15_discard_active()) frage = 1;
        uint16_t e = ((f % 20) == 0 && (g_scd.message_active || re15_discard_active())) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
    PRUEF(frage && re15_inv_find_item(RE15_S2_KEY_ITEM) < 0, "Wegwerf-Abfrage nach msg 54 (RE2 @0x80051670), Water Key weg");
    PRUEF(bit9(RE15_R42_BIT_2080_SZENE), "Szene %s gelaufen: Bit 118 = 1", elza ? "E1" : "L1");
    for (int id = 43; id <= (elza ? 51 : 48); id++) PRUEF(s_msg_gesehen[id], "Zeile %d gezeigt", id);
    PRUEF(typ_zahl(elza ? RE15_S2_TYP_JOHN : RE15_S2_TYP_MARVIN) == 1, "%s im Raum", elza ? "John" : "Marvin");
    if (elza) PRUEF(typ_zahl(RE15_S2_TYP_SHERRY) == 1, "Sherry im Raum");
    for (int f = 0; f < 60; f++) frame(0, 0);
    PRUEF(!re15_story2_r42_szene_laeuft() && !re15_cine_active(), "Szene zu Ende, Spieler frei");
    /* Wiederbetreten */
    const unsigned z7 = g_re15_s2_zaehler[7];
    if (room_boot(raum, -11160, -14400, -7316, 3072, 3) != 0) return;
    for (int f = 0; f < 30; f++) frame(0, 0);
    PRUEF(!(g_aot.slots[4].active && g_aot.slots[4].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[4].event_id == RE15_S2_EREIGNIS),
          "Wiederbetreten: kein Schluesselloch mehr");
    PRUEF(typ_zahl(elza ? RE15_S2_TYP_JOHN : RE15_S2_TYP_MARVIN) == 1, "Wiederbetreten: %s wartet (K-07)", elza ? "John" : "Marvin");
    if (elza) PRUEF(typ_zahl(RE15_S2_TYP_SHERRY) == 1, "Wiederbetreten: Sherry wartet bei John");
    druecke(-11160, -7316, 3072, 60);
    PRUEF(g_re15_s2_zaehler[7] == z7 && !re15_story2_r42_szene_laeuft(), "keine zweite Szene");
    re15_game_flag_set(9, RE15_R42_BIT_LEITER, 1);
    if (room_boot(raum, -11160, -14400, -7316, 3072, 3) != 0) return;
    for (int f = 0; f < 30; f++) frame(0, 0);
    PRUEF(typ_zahl(elza ? RE15_S2_TYP_JOHN : RE15_S2_TYP_MARVIN) == 0, "nach der Szene an der Leiter (Bit 120): niemand wartet mehr");
}

/* ---- tor2090 ----------------------------------------------------------------------------------------------- */
static void teil_tor2090(void)
{
    printf("== tor2090 ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(9, 110, 1);
    if (room_boot(0x2090, 6630, -1800, -16350, 0, 2) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    inv_geben(RE15_S2_KEY_ITEM);
    druck_t r = druecke(6630, -16350, 0, 200);
    PRUEF(r.msg_id == RE15_GSZ_MSG_ZU && !r.raumwechsel && !bit9(113) && re15_inv_find_item(RE15_S2_KEY_ITEM) >= 0,
          "Tor 2090 mit Schluessel: msg %d (soll 42 \"It's locked.\"), Raumwechsel %d, (9,113) = %d, Schluessel-Platz %d (K-01)",
          r.msg_id, r.raumwechsel, bit9(113), re15_inv_find_item(RE15_S2_KEY_ITEM));
    nachricht_weg(120);
    re15_game_flag_set(9, 113, 1);
    if (room_boot(0x2090, 6630, -1800, -16350, 0, 2) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    r = druecke(6630, -16350, 0, 200);
    PRUEF(r.raumwechsel && (r.ziel & 0xFFFE) == 0x20A0, "nach dem Schluesselloch 2080 ((9,113) = 1): Durchgang nach ROOM%04X", (unsigned)r.ziel);
}

/* ---- tueren ------------------------------------------------------------------------------------------------ */
static void teil_tueren(int elza)
{
    printf("== tueren%s ==\n", elza ? "_elza" : "");
    const uint16_t k = elza ? 1 : 0;
    re15_game_state_init();
    re15_inv_load_briefing();
    /* 2070 Nordgang: beide Tueren verriegelt (K-02/K-03) */
    if (room_boot((uint16_t)(0x2070 + k), -18471, 0, -19256, 2048, 7) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    druck_t r = druecke(-18471, -19256, 2048, 60);
    PRUEF(r.msg_id == RE15_R42_MSG_2070_ANDERE_SEITE && !r.raumwechsel && !re15_game_flag_get(3, 151),
          "2070 -> 11A0: msg %d (soll 1 \"It's locked from the other side.\"), (3,151) bleibt 0", r.msg_id);
    nachricht_weg(120);
    r = druecke(4500, -19250, 0, 60);
    PRUEF(r.msg_id == RE15_R42_MSG_2070_ANDERE_SEITE && !r.raumwechsel, "2070 -> 2040: msg %d (soll 1), kein Raumwechsel", r.msg_id);
    nachricht_weg(120);
    /* 11A0: von hier auf */
    if (room_boot((uint16_t)(0x11A0 + k), 6500, 0, -2900, 1024, 0) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    r = druecke(6500, -2900, 1024, 60);
    PRUEF(r.msg_id == RE15_R42_MSG_11A_AUFGESPERRT && !r.raumwechsel, "11A0: msg %d (soll 11 \"%s unlocked the door.\"), kein Durchgang bei diesem Druck",
          r.msg_id, elza ? "Elza" : "Leon");
    PRUEF(re15_game_flag_get(3, 151) && bit9(RE15_R42_BIT_TUER_11A), "(3,151) = 1 und Bit %d = 1", RE15_R42_BIT_TUER_11A);
    nachricht_weg(120);
    r = druecke(6500, -2900, 1024, 120);
    PRUEF(r.raumwechsel && (r.ziel & 0xFFFE) == 0x2070, "11A0 danach: Durchgang nach ROOM%04X (Door_aot_set @0x00F5A)", (unsigned)r.ziel);
    if (room_boot((uint16_t)(0x2070 + k), -18471, 0, -19256, 2048, 7) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    r = druecke(-18471, -19256, 2048, 120);
    PRUEF(r.raumwechsel && (r.ziel & 0xFFFE) == 0x11A0, "2070 danach: Abkuerzung nach ROOM%04X", (unsigned)r.ziel);
    /* 2040 Galerie (Band 5): von hier auf */
    if (room_boot((uint16_t)(0x2040 + k), -19750, -9000, -19800, 2048, 9) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    r = druecke(-19750, -19800, 2048, 60);
    PRUEF(r.msg_id == RE15_R42_MSG_2040_AUFGESPERRT && !r.raumwechsel && bit9(RE15_R42_BIT_TUER_2040),
          "2040: msg %d (soll 6), Bit %d = 1, kein Durchgang bei diesem Druck", r.msg_id, RE15_R42_BIT_TUER_2040);
    nachricht_weg(120);
    r = druecke(-19750, -19800, 2048, 120);
    PRUEF(r.raumwechsel && (r.ziel & 0xFFFE) == 0x2070, "2040 danach: Durchgang nach ROOM%04X", (unsigned)r.ziel);
    if (room_boot((uint16_t)(0x2070 + k), 4500, 0, -19250, 0, 7) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    r = druecke(4500, -19250, 0, 120);
    PRUEF(r.raumwechsel && (r.ziel & 0xFFFE) == 0x2040, "2070 -> 2040 danach offen: ROOM%04X", (unsigned)r.ziel);
}

/* ---- uhr --------------------------------------------------------------------------------------------------- */
static void teil_uhr(void)
{
    printf("== uhr ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(9, 110, 1);
    if (room_boot(0x2090, -8200, 0, -25550, 0, 0) != 0) return;
    for (int f = 0; f < 5; f++) frame(0, 0);
    int da = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_ITEM && g_aot.item_params[i].item_type == RE15_S2_UHR_ITEM) da = 1;
    PRUEF(!da, "vor dem Tod des Alligators: keine Uhr (Original-Platz 5 @0x00A12 verborgen)");
    re15_game_flag_set(9, 111, 1); re15_game_flag_set(9, 114, 1); re15_game_flag_set(9, 115, 1);
    if (room_boot(0x2090, -8200, 0, -25550, 0, 0) != 0) return;
    for (int f = 0; f < 5; f++) frame(0, 0);
    int32_t kx = 0, ky = 0, kz = 0; re15_gatorszene_r41_key_lage(&kx, &ky, &kz);
    const re15_aot_t *a = &g_aot.slots[RE15_S2_UHR_PLATZ];
    PRUEF(a->active && a->type == RE15_AOT_TYPE_ITEM && g_aot.item_params[RE15_S2_UHR_PLATZ].item_type == RE15_S2_UHR_ITEM &&
          g_aot.item_params[RE15_S2_UHR_PLATZ].taken_bit == RE15_S2_UHR_BIT && a->x == kx && a->z == kz,
          "nach dem Tod: Uhr-Zone Platz %d auf dem Schluessel-Rechteck (%d,%d), Taken-Bit 90", RE15_S2_UHR_PLATZ, (int)kx, (int)kz);
    PRUEF(!(g_aot.slots[5].active && g_aot.slots[5].type == RE15_AOT_TYPE_ITEM), "Original-Platz 5 am Tor leer");
    PRUEF(re15_blink_r41_aktiv(0x2090, RE15_S2_UHR_OBJ), "Puls an obj 0 (dasselbe Mittel wie der Schluessel)");
    {   /* Befund 6: die Lage waehlt re15_gatorszene_r41_neben_lage beim ersten Legen (Spieler-/Leichen-Kugeln dieses Augenblicks) und
         * behaelt sie; hier: 450 vom Schluessel, auf dem Wasser, nicht der alte Versatz laengs x, und die Wahl hat eine Lage gefunden. */
        int32_t ul[3] = {0, 0, 0}; int cut = -1, sx = -1, sy = -1;
        const int ok = re15_gatorszene_r41_neben_lage(RE15_S2_UHR_VERSATZ, ul, &cut, &sx, &sy);
        int k = -1;
        for (int q = 0; q < (int)g_scd.prop_count; q++) if (g_scd.props[q].obj_id == RE15_S2_UHR_OBJ) k = q;
        const int64_t dx = k >= 0 ? (int64_t)g_scd.props[k].x - kx : 0, dz = k >= 0 ? (int64_t)g_scd.props[k].z - kz : 0;
        const int64_t d2 = dx * dx + dz * dz;
        PRUEF(ok && k >= 0 && d2 >= 448 * 448 && d2 <= 452 * 452 && re15_aot_water_at(g_scd.props[k].x, g_scd.props[k].z) != 0 &&
              !(g_scd.props[k].x == kx + RE15_S2_UHR_VERSATZ && g_scd.props[k].z == kz),
              "Modell 450 neben dem Schluessel auf dem Wasser (%d,%d), nicht laengs x; Wahl jetzt Kamera %d Bild %d,%d (Befund 6)",
              k >= 0 ? (int)g_scd.props[k].x : 0, k >= 0 ? (int)g_scd.props[k].z : 0, cut, sx, sy); }
    {   /* Befund 6: die Uhr wird so gross gezeichnet wie der Schluessel (Skala aus der Ausdehnung des Modells obj 0) */
        const re15_md1_vertex_t ein = { 100, -50, 20, 0 };
        const re15_md1_vertex_t *aus = re15_story2_r42_scheitel(0x2090, RE15_S2_UHR_OBJ, &ein, 1, 0);
        PRUEF(aus && aus->x > ein.x && aus->y < ein.y && abs((int)aus->x * ein.y - (int)aus->y * ein.x) <= 100,
              "Uhr-Modell vergroessert gezeichnet (x %d -> %d, gleichmaessig)", ein.x, aus ? aus->x : 0);
        PRUEF(re15_story2_r42_scheitel(0x2090, 1, &ein, 1, 0) == NULL && re15_story2_r42_scheitel(0x2080, RE15_S2_UHR_OBJ, &ein, 1, 0) == NULL,
              "nur obj 0 in 2090/2091"); }
    g_gameflow.character = 0;
    const uint8_t *dl = re15_inv_desc_entry(RE15_S2_UHR_ITEM);
    PRUEF(dl == re15_story2_r42_desc(RE15_S2_UHR_ITEM) && dl != NULL, "Leon: Beschreibung 0x3F mit Gravur (F-19)");
    g_gameflow.character = 4;
    PRUEF(re15_story2_r42_desc(RE15_S2_UHR_ITEM) == NULL, "Elza: Original-Beschreibung 0x3F");
}

/* ---- altstand (Befund 5) ------------------------------------------------------------------------------------- */
static void teil_altstand(int elza)
{
    printf("== altstand%s ==\n", elza ? "_elza" : "");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(3, 245, 1);
    re15_game_flag_set(9, 110, 1); re15_game_flag_set(9, 111, 1); re15_game_flag_set(9, 112, 1);
    re15_game_flag_set(9, 113, 1);                                   /* v0.8.30: Tor 2090 mit dem Schluessel aufgeschlossen */
    re15_game_flag_set(9, 114, 1); re15_game_flag_set(9, 115, 1);
    const uint16_t raum = elza ? 0x2081 : 0x2080;
    if (room_boot(raum, -11160, -14400, -7316, 3072, 3) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    PRUEF(bit9(RE15_R42_BIT_2080_TOR), "Bit %d nachgezogen (das Tor ist offen)", RE15_R42_BIT_2080_TOR);
    const unsigned z1 = g_re15_s2_zaehler[1], z2 = g_re15_s2_zaehler[2], z7 = g_re15_s2_zaehler[7];
    druck_t r = druecke(-11160, -7316, 3072, 40);
    PRUEF(r.msg_id != RE15_R42_MSG_2080_TOR_ZU && r.msg_id != RE15_R42_MSG_2080_TOR_AUF && g_re15_s2_zaehler[1] == z1 &&
          g_re15_s2_zaehler[2] == z2, "kein Schluesselloch-Text (msg %d; soll weder 53 noch 54)", r.msg_id);
    PRUEF(g_re15_s2_zaehler[7] == z7 + 1, "Druck am Rechner startet die Szene (Zaehler %u -> %u)", z7, g_re15_s2_zaehler[7]);
    for (int f = 0; f < 6000 && !bit9(RE15_R42_BIT_2080_SZENE); f++) {
        uint16_t e = ((f % 20) == 0 && g_scd.message_active) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
    PRUEF(bit9(RE15_R42_BIT_2080_SZENE), "Szene gelaufen: Bit 118 = 1");
    for (int f = 0; f < 60; f++) frame(0, 0);
    if (room_boot(raum, -11160, -14400, -7316, 3072, 3) != 0) return;
    for (int f = 0; f < 30; f++) frame(0, 0);
    PRUEF(!(g_aot.slots[4].active && g_aot.slots[4].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[4].event_id == RE15_S2_EREIGNIS),
          "Wiederbetreten: kein Platz mehr am Rechner");
    druecke(-11160, -7316, 3072, 40);
    PRUEF(g_re15_s2_zaehler[7] == z7 + 1, "keine zweite Szene");
}

/* ---- begleiter (Befund 8) ------------------------------------------------------------------------------------ */
static void begleiter_fall(int elza, int b118, int b120, int soll)
{
    re15_game_state_init();
    re15_inv_load_briefing();
    if (b118) re15_game_flag_set(9, RE15_R42_BIT_2080_SZENE, 1);
    if (b120) re15_game_flag_set(9, RE15_R42_BIT_LEITER, 1);
    const uint16_t raum = elza ? 0x11A1 : 0x11A0;
    if (room_boot(raum, 28150, -1800, 1250, 2048, 0) != 0) return;
    for (int f = 0; f < 10; f++) frame(0, 0);
    const int n = elza ? typ_zahl(RE15_S2_TYP_JOHN) + typ_zahl(RE15_S2_TYP_SHERRY) : typ_zahl(RE15_S2_TYP_MARVIN);
    PRUEF(n == soll, "ROOM%04X Bit 118 = %d, Bit 120 = %d: %d Begleiter (soll %d)", raum, b118, b120, n, soll);
}
static void teil_begleiter(void)
{
    printf("== begleiter ==\n");
    for (int e = 0; e < 2; e++) {
        const int voll = e ? 2 : 1;                                  /* 11A1: John + Sherry (@0x0118C/@0x011A0), 11A0: Marvin */
        begleiter_fall(e, 0, 0, voll);                               /* vor der Szene 2080: wie bisher (Mod-Satz) */
        begleiter_fall(e, 1, 0, 0);                                  /* wartet in 2080/2081: hier nicht */
        begleiter_fall(e, 1, 1, voll);                               /* nach der Szene an der Leiter: die naechste Stufe entscheidet */
    }
}

/* ---- inszenierung (Befund 1/4) -------------------------------------------------------------------------------- */
static int nahe(int32_t x, int32_t z, int32_t sx, int32_t sz, int32_t r)
{
    const int64_t dx = (int64_t)x - sx, dz = (int64_t)z - sz;
    return dx * dx + dz * dz <= (int64_t)r * r;
}
static void teil_inszenierung(int elza)
{
    printf("== inszenierung%s ==\n", elza ? "_elza" : "");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(3, 245, 1);
    re15_game_flag_set(9, 110, 1); re15_game_flag_set(9, 111, 1); re15_game_flag_set(9, 112, 1);
    re15_game_flag_set(9, 114, 1); re15_game_flag_set(9, 115, 1);
    const uint16_t raum = elza ? 0x2081 : 0x2080;
    if (room_boot(raum, -11160, -14400, -7316, 3072, 3) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    inv_geben(RE15_S2_KEY_ITEM);
    (void)druecke(-11160, -7316, 3072, 40);
    int npc_slot = -1, sherry_slot = -1, erst_npc = -1, erst_sherry = -1, npc_an = -1, sp_los = -1, f = 0;
    const int32_t sx0 = g_actors[RE15_ACTOR_SLOT_PLAYER].x, sz0 = g_actors[RE15_ACTOR_SLOT_PLAYER].z;
    for (f = 0; f < 6000 && !bit9(RE15_R42_BIT_2080_SZENE); f++) {
        uint16_t e = ((f % 20) == 0 && (g_scd.message_active || re15_discard_active())) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e);
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            if (!g_actors[i].active) continue;
            if (npc_slot < 0 && g_actors[i].type == (elza ? RE15_S2_TYP_JOHN : RE15_S2_TYP_MARVIN)) {
                npc_slot = i; erst_npc = f;
                PRUEF(nahe(g_actors[i].x, g_actors[i].z, RE15_S2_NPC_START_X, RE15_S2_NPC_START_Z, 50),
                      "Begleiter erscheint im Suedwesten (%d,%d)", g_actors[i].x, g_actors[i].z);
            }
            if (sherry_slot < 0 && g_actors[i].type == RE15_S2_TYP_SHERRY) {
                sherry_slot = i; erst_sherry = f;
                PRUEF(nahe(g_actors[i].x, g_actors[i].z, RE15_S2_SHERRY_START_X, RE15_S2_SHERRY_START_Z, 50),
                      "Sherry erscheint in der Nische an der Treppe (%d,%d)", g_actors[i].x, g_actors[i].z);
            }
        }
        if (npc_slot >= 0 && npc_an < 0 && nahe(g_actors[npc_slot].x, g_actors[npc_slot].z, RE15_S2_NPC_X, RE15_S2_NPC_Z, 300)) npc_an = f;
        if (sp_los < 0 && !nahe(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z, sx0, sz0, 60)) sp_los = f;
    }
    PRUEF(npc_slot >= 0 && npc_an >= 0, "Begleiter kommt an N (Bild %d)", npc_an);
    PRUEF(sp_los > npc_an, "Spieler geht erst nach der Ankunft des Begleiters los (Bild %d > %d)", sp_los, npc_an);
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    PRUEF(nahe(pl->x, pl->z, RE15_S2_SPIELER_X, RE15_S2_SPIELER_Z, 150), "Spieler steht bei P (%d,%d)", pl->x, pl->z);
    if (npc_slot >= 0)
        PRUEF(nahe(g_actors[npc_slot].x, g_actors[npc_slot].z, RE15_S2_NPC_X, RE15_S2_NPC_Z, 300), "Begleiter steht bei N (%d,%d)",
              g_actors[npc_slot].x, g_actors[npc_slot].z);
    if (elza) {
        PRUEF(sherry_slot >= 0 && erst_sherry - erst_npc >= RE15_S2_SHERRY_LAUF_BILDER, "Sherry erst nach dem Treppenweg (%d Bilder)",
              erst_sherry - erst_npc);
        if (sherry_slot >= 0)
            PRUEF(nahe(g_actors[sherry_slot].x, g_actors[sherry_slot].z, RE15_S2_SHERRY_X, RE15_S2_SHERRY_Z, 300),
                  "Sherry steht bei S (%d,%d)", g_actors[sherry_slot].x, g_actors[sherry_slot].z);
    } else PRUEF(sherry_slot < 0, "Leon: keine Sherry");
    PRUEF(bit9(RE15_R42_BIT_2080_SZENE), "Szene zu Ende (Bit 118) nach %d Bildern", f);
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "texte")) teil_texte();
    else if (!strcmp(t, "programme")) teil_programme();
    else if (!strcmp(t, "ablass")) teil_ablass(0);
    else if (!strcmp(t, "loch")) teil_loch(0);
    else if (!strcmp(t, "loch_elza")) teil_loch(1);
    else if (!strcmp(t, "tor2090")) teil_tor2090();
    else if (!strcmp(t, "tueren")) teil_tueren(0);
    else if (!strcmp(t, "tueren_elza")) teil_tueren(1);
    else if (!strcmp(t, "uhr")) teil_uhr();
    else if (!strcmp(t, "altstand")) teil_altstand(0);
    else if (!strcmp(t, "altstand_elza")) teil_altstand(1);
    else if (!strcmp(t, "begleiter")) teil_begleiter();
    else if (!strcmp(t, "inszenierung")) teil_inszenierung(0);
    else if (!strcmp(t, "inszenierung_elza")) teil_inszenierung(1);
    else { printf("unbekannter Teil '%s'\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
