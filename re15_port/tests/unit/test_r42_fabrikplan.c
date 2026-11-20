/*
 * test_r42_fabrikplan.c — Runde 42 Strang fabrikplan (v0.8.31, 20.11.2026, Konzept K-16): der Fabrikplan, den Marvin in 3060
 * uebergibt, muss vorher wirklich aufgenommen werden.
 * NUTZERWUNSCH 20.11.2026 woertlich: "Im Story Zweig - room 3060 übergibt Marvin ein plan of the plant. Das macht natürlich nur
 * sinn wenn er ihn auch wirklich irgendwo aufnimmt..."
 * Belege: Kopf include/re15_kartenitems_r36.h (STRANG FABRIKPLAN), include/re15_story_r42.h (RE15_R42_BIT_PLAN_MARVIN),
 * engine/src/story3a_r42.c (wiedersehen_bauen / geben / re15_story3a_r42_blatt), engine/src/kartenitems_r36.c (begleit_bit).
 *
 * Riegel ueber die ECHTEN Engine-Wege (Harness wie test_r42_story3a.c: scd_room_reenter mit allen Installern, SCD-VM, re15_game_step
 * mit Aktionstaste; Figurwechsel ueber re15_sherry_r41_modell_setzen = Raumlader). Der Riegel benutzt NUR API, die schon auf
 * 9426267fa (r42/story, Abzweig dieses Zweigs) bestand - die Gegenprobe baut dort und wird rot. Teile:
 *   marvin  Marvin spielt (Figurwechsel 0 -> 0x0D) in 30C0: der Plan haengt (Prop obj 15 + Zone Ereignis 28); ECHTER Aktionsdruck vor
 *           der Wand + Ja -> Marvins Bit 139 (NICHT 35), Marvins Karte zeigt Blatt 7; danach haengt er nicht mehr. Rueckwechsel
 *           (0x0D -> 0): Leons Karte ohne Blatt 7 bis zur Uebergabe. Elzas 30C1: Marvins Bit hat dort keine Wirkung (Plan haengt,
 *           Blatt 7 nicht im Besitz) - Elza unveraendert.
 *   mit     Wiedersehen 3060, Marvin traegt den Plan: Zeilen 17/18 laufen, danach Bit 35 (Leon, Blatt 7), Bit 139 geloescht, in 30C0
 *           haengt nichts mehr.
 *   ohne    Wiedersehen 3060, Marvin hat den Plan NICHT genommen: keine 17/18 (alle anderen Zeilen wie mit Plan), kein Bit 35, Blatt
 *           7 nicht im Besitz; der Plan haengt weiter in 30C0, Leon nimmt ihn selbst (Aktionsdruck + Ja) -> Bit 35, nicht 139.
 *   alt     alte Spielstaende (Spielstand v10 ueber re15_savedata_capture/restore):
 *           (a) mitten in Marvins Abschnitt gespeichert -> Plan haengt in 30C0, Marvin nimmt ihn (139);
 *           (b) nach dem Wiedersehen gespeichert (Bit 35 + 124) -> kein Plan in 30C0, kein zweites Wiedersehen;
 *           (c) nach dem Rueckwechsel, vor dem Wiedersehen gespeichert -> Wiedersehen-Programm ohne 17/18, Plan haengt in 30C0.
 *   sicht   GEMESSEN auf Marvins Pflichtweg in 30C0 (Wegpunkte integration_r42_story3a): Gang 1 Ankunft aus 3020 (-11250,-10900)
 *           (Door_aot_set 3020 Slot 2 @0x00B6E, Cut 0) -> Osttuer nach 3090 (30C0 Slot 0 @0x00B5A); Gang 2 Ankunft aus 3090 (-6400,-400)
 *           (3090 Slot 0 @0x0132A, Cut 1) -> Treppe (-21700,300). Je Bild: aktive Kamera (echter Kamerawechsel im re15_game_step),
 *           Plakat ganz im Bild (alle vier Ecken der Flaeche 2160 x 1440 im 320 x 240-Bild) und seine Groesse in Bildpunkten. Soll: auf
 *           BEIDEN Gaengen ist das Plakat in einem Teil der Bilder ganz und mindestens 30 Bildpunkte breit zu sehen.
 * Gegenprobe: auf 9426267fa sind marvin, mit, ohne, alt und sicht rot (kein Plan in 30C0; 17/18 immer; Bit 139 nie gesetzt/geloescht).
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
#include "re15_light.h"
#include "re15_story3a_r42.h"
#include "re15_sherry_r41.h"
#include "re15_savedata.h"
#include "re15_story_r42.h"
#include "re15_kartenitems_r36.h"
#include "re15_map_owned.h"
#include "re15_map_hint.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

/* Strang fabrikplan: Marvins Bit (re15_story_r42.h). Fuer die Gegenprobe auf 9426267fa, wo es das Makro noch nicht gibt. */
#ifndef RE15_R42_BIT_PLAN_MARVIN
#define RE15_R42_BIT_PLAN_MARVIN 139
#endif
#define BIT35  RE15_R42_BIT_PLAN_FABRIK
#define BIT139 RE15_R42_BIT_PLAN_MARVIN
#define BLATT7 RE15_S3A_FABRIK_BLATT

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ---- Harness (wie test_r42_story3a.c) --------------------------------------------------------------------------------- */
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
    re15_fade_tick();
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

static int bit9(int b) { return re15_game_flag_get(9, (uint8_t)b) ? 1 : 0; }
static void inv_setzen(int i, uint8_t id, uint8_t qty) { g_inv.slots[i].id = id; g_inv.slots[i].qty = qty; g_inv.slots[i].flags = 0; }
extern void re15_player_set_equipped_weapon(int id);
static void leon_inventar(void)
{
    re15_inv_init();
    inv_setzen(0, 0x03, 12); inv_setzen(1, 0x15, 30); inv_setzen(2, 0x23, 1);
    re15_player_set_equipped_weapon(0x03);
    re15_inv_set_equipped_slot(0);
}

/* Der Plan haengt = Prop obj 15 (RE15_R36K_OBJ) aktiv UND eine Zone Ereignis 28 (RE15_R36K_EREIGNIS) - nach dem echten Raumaufbau. */
static int plakat(void)
{
    int prop = 0, zone = 0;
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].active && g_scd.props[k].obj_id == RE15_R36K_OBJ) prop = 1;
    for (int s = 0; s < RE15_AOT_MAX; s++)
        if (g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[s].event_id == RE15_R36K_EREIGNIS) zone = 1;
    return prop && zone;
}

/* Standort vor dem Plakat: Wand z = 1100 (ROOM30C0/30C1 SCA-Zelle 1 @0x007F8), Plakatmitte x -7600 (karten_export.py PLAENE2),
 * 550 vor der Wand, Blick in die Wand (3072 = +z: Vorwaertspunkt fz = z - 620 sin(rot), @0x80042bd0). */
#define VOR_X  (-7600)
#define VOR_Z  (1100 - 550)
#define VOR_ROT 3072

static int haengt_in(uint16_t raum)
{
    if (room_boot(raum, VOR_X, 0, VOR_Z, VOR_ROT, 1) != 0) return -1;
    for (int f = 0; f < 4; f++) frame(0, 0);
    return plakat();
}

/* ECHTER Aktionsdruck (Quadrat-Flanke im Spielbild, re15_game_step -> Klettersonde -> Aktions-Scan FUN_80042bac) vor dem Plakat,
 * dann die Frage "There's a factory map. / Will you take it?" mit Quadrat bestaetigen (Ja vorbelegt) und die Meldung schliessen.
 * Rueckgabe: 1 = Meldung 31 ("You've taken the factory map.") lief. */
static int nehmen_in(uint16_t raum)
{
    if (room_boot(raum, VOR_X, 0, VOR_Z, VOR_ROT, 1) != 0) return -1;
    for (int f = 0; f < 20; f++) frame(0, 0);
    if (!plakat()) return 0;
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    frame(0, 0);
    int frage = 0, meldung = 0;
    for (int f = 0; f < 900; f++) {
        if (g_scd.message_active && g_scd.message_id == RE15_R36K_MSG_FRAGE) frage = 1;
        if (g_scd.message_active && g_scd.message_id == RE15_R36K_MSG_GENOMMEN) meldung = 1;
        frame(0, (uint16_t)((f % 15) == 14 ? RE15_PAD_BIT_SQUARE : 0));
        if (meldung && !g_scd.message_active && f > 60) break;
    }
    printf("    ROOM%04X: Aktion vor dem Plakat -> Frage %d, Meldung \"You've taken ...\" %d; Bits 35=%d 139=%d\n", (unsigned)raum,
           frage, meldung, bit9(BIT35), bit9(BIT139));
    return frage && meldung;
}

/* Figurwechsel ueber den Raumlader (re15_sherry_r41_modell_setzen -> story3a figur_vor/figur_nach), Leon mit Pistole */
static void marvin_tausch(void)
{
    g_gameflow.character = 0;
    leon_inventar();
    re15_sherry_r41_modell_init(0);
    re15_sherry_r41_modell_setzen(RE15_S3A_MODELL_MARVIN);
}
static void rueckwechsel(void)
{
    g_gameflow.character = 0;
    re15_sherry_r41_modell_setzen(0);
}

/* Wiedersehen 3060 wie test_r42_story3a.c wiedersehen_lauf: Marvins Abschnitt gelaufen (3,0xA0), Schloss 3040 (4,0xC4), Karte genommen
 * (9,85), Hunde tot (7,0x50..0x52), Marvins Seite mit Green Keycard + Redhawk; ohne/mit Bit 139 je Fall. Rueckgabe Bilder, < 0 Fehler. */
static int wiedersehen(int plan)
{
    re15_game_state_init();
    re15_story3a_r42_neues_spiel();
    g_gameflow.character = 0;
    re15_game_flag_set(3, RE15_S3A_A0_BIT, 1); re15_game_flag_set(4, 0xC4, 1); re15_game_flag_set(9, 85, 1);
    re15_game_flag_set(7, 0x50, 1); re15_game_flag_set(7, 0x51, 1); re15_game_flag_set(7, 0x52, 1);
    if (plan) re15_game_flag_set(9, BIT139, 1);
    re15_s3a_figur_t s; memset(&s, 0, sizeof s);
    s.marvin_offen = 1;
    s.marvin[0].id = 0x03; s.marvin[0].qty = 15; s.marvin[1].id = 0x15; s.marvin[1].qty = 50;
    s.marvin[2].id = RE15_S3A_KEYCARD; s.marvin[2].qty = 1; s.marvin[3].id = RE15_S3A_REDHAWK; s.marvin[3].qty = 6;
    uint8_t blk[RE15_S3A_SPEICHER_BYTES]; memset(blk, 0, sizeof blk); memcpy(blk, &s, sizeof s);
    re15_story3a_r42_import(blk);
    if (room_boot(0x3060, -26027, 0, -22532, 0, 0) != 0) return -1;
    leon_inventar();
    int f;
    for (f = 0; f < 9000 && !bit9(RE15_R42_BIT_3060_UEBERGABE); f++) {
        const int sys = g_scd.message_active && (g_scd.message_id == 16 || g_scd.message_id == 18 || g_scd.message_id == 20);
        frame(0, (uint16_t)((sys && (f % 20) == 0) ? RE15_PAD_BIT_SQUARE : 0));
    }
    for (int k = 0; k < 120; k++) frame(0, 0);
    return f;
}
static const char *gesehen(char *t, size_t g)
{
    t[0] = 0;
    for (int id = 11; id < 64; id++) if (s_msg_gesehen[id]) snprintf(t + strlen(t), g - strlen(t), "%s%d", t[0] ? "," : "", id);
    return t;
}

/* ---- sicht --------------------------------------------------------------------------------------------------------------- */
/* Plakatflaeche aus der Zeile (Ursprung + 85 * s * n, karten_export.py), Groesse s aus den MD1-Ecken @0x44 */
static int plakat_bild(const re15_kartenitem_r36_t *z, const re15_camera_view_t *v, double *b0, double *b1, double *h0, double *h1)
{
    int msz = 0;
    const uint8_t *md1 = re15_kartenitems_r36_md1_bytes(0x30C0, &msz);
    int16_t ez0 = 0, ez1 = 0;
    for (int k = 0; md1 && k < 5 && 0x44 + 8 * k + 6 <= msz; k++) {
        int16_t mz = (int16_t)(md1[0x48 + 8 * k] | (md1[0x49 + 8 * k] << 8));
        if (mz < ez0) ez0 = mz;
        if (mz > ez1) ez1 = mz;
    }
    const double sk = (double)(ez1 - ez0) / 1080.0;
    double nx = (z->rect_x + RE15_R36K_RECT / 2.0) - z->x, nz = (z->rect_z + RE15_R36K_RECT / 2.0) - z->z;
    const double l = sqrt(nx * nx + nz * nz);
    nx /= l; nz /= l;
    const double M[3] = { z->x + 85.0 * sk * nx, z->y, z->z + 85.0 * sk * nz };
    const double rx[3] = { nz, 0, -nx };
    int drin = 1;
    *b0 = 1e9; *b1 = -1e9; *h0 = 1e9; *h1 = -1e9;
    for (int a = -1; a <= 1; a += 2)
        for (int b = -1; b <= 1; b += 2) {
            const double q[3] = { M[0] + a * 540.0 * sk * rx[0], M[1] + b * 360.0 * sk, M[2] + a * 540.0 * sk * rx[2] };
            const double vx = (v->rot[0] * q[0] + v->rot[1] * q[1] + v->rot[2] * q[2]) / 4096.0 + v->trans[0];
            const double vy = (v->rot[3] * q[0] + v->rot[4] * q[1] + v->rot[5] * q[2]) / 4096.0 + v->trans[1];
            const double vz = (v->rot[6] * q[0] + v->rot[7] * q[1] + v->rot[8] * q[2]) / 4096.0 + v->trans[2];
            if (vz <= 0) { drin = 0; continue; }
            const double sx = 160.0 + v->fov_screen_dist * vx / vz, sy = 120.0 + v->fov_screen_dist * vy / vz;
            if (sx < 0 || sx >= 320 || sy < 0 || sy >= 240) drin = 0;
            if (sx < *b0) *b0 = sx;
            if (sx > *b1) *b1 = sx;
            if (sy < *h0) *h0 = sy;
            if (sy > *h1) *h1 = sy;
        }
    return drin;
}

/* Spieler entlang der Wegpunkte schieben (60 Einheiten je Bild, knapp das Gehtempo ~72 aus integration_r42_lageplaene2_weg),
 * Kamerawechsel ueber das echte Spielbild; je Bild Kamera + Sichtbarkeit zaehlen. */
static void gang(const char *name, const int32_t (*wp)[2], int n, uint8_t cut0, int *bilder, int *sichtbar, double *breite_max)
{
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(0x30C0);
    *bilder = 0; *sichtbar = 0; *breite_max = 0;
    if (!z) { PRUEF(0, "%s: keine Zeile fuer ROOM30C0", name); return; }
    if (room_boot(0x30C0, wp[0][0], 0, wp[0][1], 0, cut0) != 0) return;
    int kam_bilder[32] = { 0 }, kam_sicht[32] = { 0 };
    double kam_breite[32] = { 0 };
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int i = 0; i + 1 < n; i++) {
        const double dx = wp[i + 1][0] - wp[i][0], dz = wp[i + 1][1] - wp[i][1], l = sqrt(dx * dx + dz * dz);
        const int schritte = (int)(l / 60.0) + 1;
        for (int k = 0; k <= schritte; k++) {
            pl->x = (int32_t)lround(wp[i][0] + dx * k / schritte);
            pl->z = (int32_t)lround(wp[i][1] + dz * k / schritte);
            frame(0, 0);
            const int cam = (int)g_scd.cam_id;
            re15_camera_view_t v;
            double b0, b1, h0, h1;
            int drin = cam >= 0 && cam < s_rdt.cut_count && re15_camera_build_view(&s_rdt.cuts[cam], &v) == 0 &&
                       plakat_bild(z, &v, &b0, &b1, &h0, &h1);
            (*bilder)++;
            if (cam >= 0 && cam < 32) kam_bilder[cam]++;
            if (drin) {
                (*sichtbar)++;
                if (cam >= 0 && cam < 32) { kam_sicht[cam]++; if (b1 - b0 > kam_breite[cam]) kam_breite[cam] = b1 - b0; }
                if (b1 - b0 > *breite_max) *breite_max = b1 - b0;
            }
        }
    }
    printf("    %s: %d Bilder, Plakat ganz im Bild in %d (max %.0f Bildpunkte breit im 320er Bild)\n", name, *bilder, *sichtbar,
           *breite_max);
    for (int c = 0; c < 32; c++)
        if (kam_bilder[c])
            printf("      Kamera %2d: %3d Bilder, Plakat ganz im Bild in %3d (bis %.0f Bildpunkte breit)\n", c, kam_bilder[c],
                   kam_sicht[c], kam_breite[c]);
}

/* Kopfband einer Figur aus ihrer PLD wie test_r42_lageplaene4.c kopfband (Clip 0 Bild 0; hals = Plc_neck-Ursprung Knochen 8,
 * neck_bone 8 @0x80031938; scheitel = hoechste Ecke des Kopf-Meshs). Nur Messausgabe (Hoehe des Plakats zur Figur). */
static int kopfband(const char *pld, int32_t *hals, int32_t *scheitel)
{
    char p[600];
    snprintf(p, sizeof p, "%s/PLD/%s", RE15_ASSET_PSX_DIR, pld);
    size_t n = 0;
    uint8_t *b = slurp(p, &n);
    if (!b) return 0;
    unsigned long o[3], l[3];
    static re15_emd_animation_t an; static re15_emd_skeleton_t sk; static re15_md1_t md;
    memset(&an, 0, sizeof an); memset(&sk, 0, sizeof sk); memset(&md, 0, sizeof md);
    int ok = 1;
    for (int t = 0; t < 3; t++) if (!re15_pld_part(b, (long)n, t, &o[t], &l[t])) ok = 0;
    if (ok && (re15_emd_parse_animation(b + o[0], l[0], &an) != 0 || re15_emd_parse_skeleton(b + o[1], l[1], &sk) != 0 ||
               re15_md1_parse(b + o[2], (int)l[2], &md) != 0 || sk.bone_count <= 8)) ok = 0;
    if (ok) {
        re15_actor_t a; memset(&a, 0, sizeof a);
        a.active = 1; a.motion = 0; a.anim_frame = 0;
        int kf = re15_compute_actor_kf(&an, &sk, &a, 0, 0);
        static re15_skel_pose_t P[RE15_EMD_MAX_BONES];
        g_anim_pose_actor = NULL;
        if (kf < 0 || re15_skel_compute_pose(&sk, kf, P) != 0) ok = 0;
        if (ok) {
            const int bn = 8;
            int32_t rp[3] = { sk.bone_relative_pos[bn][0], sk.bone_relative_pos[bn][1], sk.bone_relative_pos[bn][2] };
            *hals = ((P[0].rot[3] * rp[0] + P[0].rot[4] * rp[1] + P[0].rot[5] * rp[2]) >> 12) + P[0].trans[1];
            int mi = sk.bone_mesh_index[bn];
            if (mi < 0 || mi >= md.mesh_count) mi = bn;
            const re15_md1_mesh_t *m = &md.meshes[mi];
            int32_t top = 0; int gesehen = 0;
            for (int pass = 0; pass < 2; pass++) {
                const re15_md1_vertex_t *v = pass ? m->quad_vertices : m->tri_vertices;
                int nv = pass ? m->quad_vertex_count : m->tri_vertex_count;
                for (int i = 0; i < nv; i++) {
                    int32_t y = ((P[bn].rot[3] * v[i].x + P[bn].rot[4] * v[i].y + P[bn].rot[5] * v[i].z) >> 12) + P[bn].trans[1];
                    if (!gesehen || y < top) { top = y; gesehen = 1; }
                }
            }
            *scheitel = top;
            ok = gesehen && top < *hals;
        }
    }
    free(b);
    return ok;
}

static void teil_sicht(void)
{
    printf("== sicht: das Plakat auf Marvins Pflichtweg in 30C0 ==\n");
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    marvin_tausch();
    /* Gang 1: Ankunft aus 3020 -> Osttuer nach 3090 (Wegpunkte integration_r42_story3a _weg_marvin, ohne den Abstecher zum Plan) */
    static const int32_t g1[][2] = { { -11250, -10900 }, { -8750, -6800 }, { -7550, -1250 }, { -6350, -350 } };
    /* Gang 2: zurueck aus 3090 -> Treppe nach oben (Wegpunkte ebenda) */
    static const int32_t g2[][2] = { { -6400, -400 }, { -19550, -3200 }, { -21650, -1100 }, { -21700, 300 } };
    int b1, s1, b2, s2;
    double w1, w2;
    gang("Gang 1 (Ankunft aus 3020, Cut 0 -> Osttuer)", g1, 4, 0, &b1, &s1, &w1);
    gang("Gang 2 (aus 3090, Cut 1 -> Treppe)", g2, 4, 1, &b2, &s2, &w2);
    PRUEF(s1 > 0 && w1 >= 30.0, "Gang 1: Plakat in %d von %d Bildern ganz im Bild, bis %.0f Bildpunkte breit (Soll > 0 Bilder, >= 30)",
          s1, b1, w1);
    PRUEF(s2 > 0 && w2 >= 30.0, "Gang 2: Plakat in %d von %d Bildern ganz im Bild, bis %.0f Bildpunkte breit (Soll > 0 Bilder, >= 30)",
          s2, b2, w2);
    {   /* Hoehe zur Figur (Messausgabe, kein Soll: Hoehe = 30C1, Entscheid des Orchestrators) */
        const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(0x30C0);
        int32_t mh = 0, ms = 0, lh = 0, ls = 0;
        if (z && kopfband("PL0D.PLD", &mh, &ms) && kopfband("PL00.PLD", &lh, &ls))
            printf("    Hoehe: Plakat y %ld..%ld (Mitte %ld, Boden 0); Kopfband Marvin PL0D %ld..%ld, Leon PL00 %ld..%ld (Hals..Scheitel)\n",
                   (long)(z->y - 720), (long)(z->y + 720), (long)z->y, (long)mh, (long)ms, (long)lh, (long)ls);
    }
    re15_story3a_r42_neues_spiel();
}

/* ---- marvin ------------------------------------------------------------------------------------------------------------- */
static void teil_marvin(void)
{
    printf("== marvin: Marvin nimmt den Fabrikplan in 30C0 ==\n");
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    marvin_tausch();
    PRUEF(re15_story3a_r42_marvin_spielt(), "Figurwechsel Leon -> Marvin (Raumlader, T-01)");
    PRUEF(!re15_map_owned_page(BLATT7), "vorher: Blatt %d nicht im Besitz", BLATT7);
    PRUEF(haengt_in(0x30C0) == 1, "ROOM30C0 (Marvin): der Fabrikplan haengt (Prop obj %d + Zone Ereignis %d)", RE15_R36K_OBJ,
          RE15_R36K_EREIGNIS);
    PRUEF(nehmen_in(0x30C0) == 1, "ROOM30C0: Aktionsdruck vor der Wand -> Frage -> Ja -> \"You've taken the factory map.\"");
    PRUEF(bit9(BIT139) && !bit9(BIT35), "Marvins Bit %d = 1, Leons Bit %d = 0 (der Plan gehoert zu Marvins Seite)", BIT139, BIT35);
    PRUEF(re15_map_owned_page(BLATT7) && re15_map_blatt_waehlbar(BLATT7), "Marvins Karte: Blatt %d im Besitz und waehlbar", BLATT7);
    PRUEF(haengt_in(0x30C0) == 0, "ROOM30C0 danach: der Plan haengt nicht mehr");
    rueckwechsel();
    PRUEF(!re15_story3a_r42_marvin_spielt(), "Rueckwechsel Marvin -> Leon");
    PRUEF(!re15_map_owned_page(BLATT7) && bit9(BIT139) && !bit9(BIT35),
          "Leons Karte nach dem Rueckwechsel: Blatt %d NICHT im Besitz (erst mit der Uebergabe), Bit 139 bleibt bei Marvin", BLATT7);
    PRUEF(haengt_in(0x30C0) == 0, "ROOM30C0 als Leon: kein Plan (Marvin traegt ihn)");
    /* Elza unveraendert: ihr 30C1 kennt Marvins Bit nicht (getrennte Spielstaende; die Zeile 30C1 hat kein begleit_bit) */
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    re15_game_flag_set(9, BIT139, 1);
    PRUEF(haengt_in(0x30C1) == 1 && !re15_map_owned_page(BLATT7),
          "ROOM30C1 (Elza) mit Bit 139: der Plan haengt, Blatt %d nicht im Besitz (Marvins Bit wirkt bei Elza nicht)", BLATT7);
    re15_game_state_init();
    PRUEF(nehmen_in(0x30C1) == 1 && bit9(BIT35) && !bit9(BIT139) && re15_map_owned_page(BLATT7),
          "ROOM30C1 (Elza): nimmt den Plan -> Bit 35, Blatt %d (wie bisher)", BLATT7);
    re15_story3a_r42_neues_spiel();
}

/* ---- mit ---------------------------------------------------------------------------------------------------------------- */
static void teil_mit(void)
{
    printf("== mit: Wiedersehen 3060, Marvin traegt den Plan ==\n");
    char t[200];
    const int f = wiedersehen(1);
    if (f < 0) return;
    gesehen(t, sizeof t);
    PRUEF(bit9(RE15_R42_BIT_3060_UEBERGABE), "Wiedersehen lief zu Ende (Bit 124) nach %d Bildern", f);
    PRUEF(!strcmp(t, "12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27"), "Zeilen %s (mit 17 \"And this. A floor plan of the plant.\" "
          "und 18 \"You've received the factory map.\")", t);
    PRUEF(bit9(BIT35) && re15_map_owned_page(BLATT7), "Uebergabe: Bit 35, Leons Karte zeigt Blatt %d", BLATT7);
    PRUEF(!bit9(BIT139), "Marvin traegt den Plan nicht mehr (Bit 139 = 0)");
    PRUEF(haengt_in(0x30C0) == 0, "ROOM30C0: kein Plan an der Wand (keine zweite Aufnahme)");
    re15_story3a_r42_neues_spiel();
}

/* ---- ohne --------------------------------------------------------------------------------------------------------------- */
static void teil_ohne(void)
{
    printf("== ohne: Wiedersehen 3060, Marvin hat den Plan nicht genommen ==\n");
    char t[200];
    const int f = wiedersehen(0);
    if (f < 0) return;
    gesehen(t, sizeof t);
    PRUEF(bit9(RE15_R42_BIT_3060_UEBERGABE), "Wiedersehen lief zu Ende (Bit 124) nach %d Bildern", f);
    PRUEF(!strcmp(t, "12,13,14,15,16,19,20,21,22,23,24,25,26,27"), "Zeilen %s (ohne 17/18; alle anderen Ids wie mit Plan)", t);
    PRUEF(!bit9(BIT35) && !bit9(BIT139) && !re15_map_owned_page(BLATT7), "kein Bit 35/139, Blatt %d nicht im Besitz", BLATT7);
    PRUEF(haengt_in(0x30C0) == 1, "ROOM30C0: der Plan haengt weiter");
    PRUEF(nehmen_in(0x30C0) == 1, "ROOM30C0: Leon nimmt ihn selbst (Aktion + Ja)");
    PRUEF(bit9(BIT35) && !bit9(BIT139) && re15_map_owned_page(BLATT7), "Leon: Bit 35 (nicht 139), Blatt %d im Besitz", BLATT7);
    PRUEF(haengt_in(0x30C0) == 0, "ROOM30C0 danach: kein Plan");
    re15_story3a_r42_neues_spiel();
}

/* ---- alt ---------------------------------------------------------------------------------------------------------------- */
static void teil_alt(void)
{
    printf("== alt: alte Spielstaende ==\n");
    static re15_savedata_t sd;
    uint16_t raum = 0;
    /* (a) mitten in Marvins Abschnitt gespeichert (Marvin spielt, kein Plan-Bit) */
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    marvin_tausch();
    g_scd.work_vars[0x10] = RE15_S3A_MODELL_MARVIN;
    g_current_room_id = 0x30C0;
    re15_savedata_capture(&sd, 1234, 1);
    re15_game_state_init(); re15_story3a_r42_neues_spiel(); re15_inv_init();
    re15_sherry_r41_modell_init(0); g_scd.work_vars[0x10] = 0;
    PRUEF(re15_savedata_restore(&sd, &raum) == 0 && re15_story3a_r42_marvin_spielt(), "(a) Stand als Marvin geladen (Raum %04X)", raum);
    PRUEF(haengt_in(0x30C0) == 1, "(a) ROOM30C0: der Plan haengt");
    PRUEF(nehmen_in(0x30C0) == 1 && bit9(BIT139) && !bit9(BIT35), "(a) Marvin nimmt ihn -> Bit 139 (nicht 35)");
    re15_story3a_r42_neues_spiel();

    /* (b) nach dem Wiedersehen gespeichert (alter Stand: Uebergabe lief, Bit 35 + 124; nichts mehr gemerkt) */
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    g_gameflow.character = 0;
    leon_inventar();
    re15_game_flag_set(3, RE15_S3A_A0_BIT, 1); re15_game_flag_set(4, 0xC4, 1); re15_game_flag_set(9, 85, 1);
    re15_game_flag_set(7, 0x50, 1); re15_game_flag_set(7, 0x51, 1); re15_game_flag_set(7, 0x52, 1);
    re15_game_flag_set(9, BIT35, 1); re15_game_flag_set(9, RE15_R42_BIT_3060_UEBERGABE, 1);
    g_current_room_id = 0x3060;
    re15_savedata_capture(&sd, 1500, 2);
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    PRUEF(re15_savedata_restore(&sd, &raum) == 0 && bit9(BIT35) && !bit9(BIT139), "(b) alter Stand nach dem Wiedersehen geladen");
    PRUEF(haengt_in(0x30C0) == 0 && re15_map_owned_page(BLATT7), "(b) ROOM30C0: kein Plan an der Wand, Blatt %d im Besitz", BLATT7);
    {
        const unsigned z7 = g_re15_s3a_zaehler[7];
        if (room_boot(0x3060, -26027, 0, -22532, 0, 0) == 0) {
            for (int k = 0; k < 300; k++) frame(0, 0);
            PRUEF(g_re15_s3a_zaehler[7] == z7 && !s_msg_gesehen[17] && !s_msg_gesehen[18], "(b) 3060: keine zweite Uebergabe");
        }
    }
    re15_story3a_r42_neues_spiel();

    /* (c) nach dem Rueckwechsel, vor dem Wiedersehen gespeichert (Funde gemerkt, kein Plan-Bit) */
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    marvin_tausch();
    inv_setzen(2, RE15_S3A_KEYCARD, 1);
    rueckwechsel();
    re15_game_flag_set(3, RE15_S3A_A0_BIT, 1);
    g_current_room_id = 0x3020;
    re15_savedata_capture(&sd, 1300, 3);
    re15_game_state_init(); re15_story3a_r42_neues_spiel();
    PRUEF(re15_savedata_restore(&sd, &raum) == 0 && !re15_story3a_r42_marvin_spielt() && re15_story3a_r42_figur()->marvin_offen,
          "(c) Stand zwischen Rueckwechsel und Wiedersehen geladen (Funde gemerkt)");
    {
        int n = 0, mit17 = 0, mit18 = 0, mit16 = 0;
        const uint8_t *p = re15_story3a_r42_programm(2, &n);
        for (int i = 0; p && i + 3 < n; i++)
            if (p[i] == 0x2b && p[i + 2] == 0x00 && p[i + 3] == 0x00) {
                if (p[i + 1] == 17) mit17 = 1;
                if (p[i + 1] == 18) mit18 = 1;
                if (p[i + 1] == 16) mit16 = 1;
            }
        PRUEF(p && mit16 && !mit17 && !mit18, "(c) Wiedersehen-Programm: Green Keycard (16) ja, Plan-Zeilen 17/18 nein");
    }
    PRUEF(haengt_in(0x30C0) == 1 && !re15_map_owned_page(BLATT7), "(c) ROOM30C0: der Plan haengt, Blatt %d nicht im Besitz", BLATT7);
    re15_story3a_r42_neues_spiel();
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "marvin")) teil_marvin();
    else if (!strcmp(t, "mit")) teil_mit();
    else if (!strcmp(t, "ohne")) teil_ohne();
    else if (!strcmp(t, "alt")) teil_alt();
    else if (!strcmp(t, "sicht")) teil_sicht();
    else { printf("Teil unbekannt: '%s' (marvin | mit | ohne | alt | sicht)\n", t); return 2; }
    free(s_raw);
    printf(g_fail ? "FEHLER: %d\n" : "OK\n", g_fail);
    return g_fail ? 1 : 0;
}
