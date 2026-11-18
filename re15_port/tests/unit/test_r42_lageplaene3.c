/* test_r42_lageplaene3.c — Runde 42 Strang lageplaene_nach (Nacharbeit zu den Kartenfunden): Dach, Sichtbarkeit, Ort.
 * Kopf mit allen Belegen: include/re15_kartenitems_r36.h (Abschnitt RUNDE 42 NACHARBEIT).
 *
 * Orchestrator-Entscheid 18.11.2026: "das Dach gehoert zum Revier" -> der Revierplan schaltet auch Blatt 5 (ROOM1170) frei.
 * NUTZERWUNSCH 18.11.2026: der Plan soll gut SICHTBAR sein (Pruefer s1 gelb 6: Revierplan an der alten Stelle ~11 x 8
 * Bildpunkte im 320er Bild; s2 gelb 2: Fabrikplan ~35 x 25 bei 640 x 480).
 *
 * Teile (je eigener ctest-Eintrag). Der Riegel benutzt NUR die API und die Zeilenfelder, die schon auf aaea5f8c2 bestanden
 * (raum, blatt, bit, aot_slot, x/y/z, rect_x/rect_z, md1_bytes), damit die Gegenprobe dort und auf bce9c91a8 BAUT und rot wird:
 *   dach        Revierplan in 1030 UND 1031 ueber das echte Port-Programm (Ereignis 28, Ja): Bit (9,42) gesetzt, Blatt 5 im
 *               Besitz und waehlbar; frischer Stand: Blatt 5 nicht im Besitz. Alter Stand mit 87/88/89 (Revierplan vor der
 *               Nacharbeit genommen): der Plan haengt noch und gibt beim Nehmen das Dach dazu.
 *   revier      (a) RDT-Fakten des Orts (ROOM1030 + ROOM1031): Kamera 2 ist nur ueber die RVD-Zonen 1->2 (1030 @0x00294,
 *               1031 @0x002B4) und 3->2 (@0x0030C / @0x0032C) erreichbar; Tuer nach 1050 = Door_aot_set Slot 0 (@0x01C6A bzw.
 *               @0x01CD8) x 1500..3000 z -13800..-11600; Ostwand SCA-Zelle 1 (@0x01A1C bzw. @0x01A80) Kante x 2398 (diese
 *               Fakten allein waeren auf jedem Stand gruen).
 *               (b) Plakat auf der Ostwand x 2398 neben der Tuer (z-Bereich ausserhalb der Tuer), ganz im Bild der Kamera 2 und
 *               mindestens 40 Bildpunkte breit (Massstab: Kanalisationsplakat in seiner Kamera 11 = 37 Bildpunkte, Ort vom
 *               Nutzer am 18.11. mit "ja" bestaetigt) — in 1030 UND 1031.
 *   druck       ECHTER Aktionsdruck vor dem Plan Blick Ostwand, 1030 UND 1031: keine Klettersonde an 5 Standorten,
 *               re15_aot_scan liefert Ereignis 28. Strang lageplaene_nach2: Standort (1500,-11300) links der Tuer (vorher
 *               (1700,-14000) im Streifen Bank/Tuer, Tafel Slot 9 verdraengt); eigener Riegel test_r42_lageplaene4.c.
 *   fabrik      Fabrikplan 30C1: MD1-Flaeche 2160 x 1440 und im Bild der Kamera 1 mindestens 40 Bildpunkte breit.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_camera.h"
#include "re15_climb.h"
#include "re15_msg.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_map_owned.h"
#include "re15_map_hint.h"
#include "re15_kartenitems_r36.h"

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
extern uint16_t         g_scd_pad_edge, g_scd_pad_held;

static int g_fail = 0;
#define PRUEFE(cond, ...) do { if (!(cond)) { printf("  FAIL: " __VA_ARGS__); printf("\n"); g_fail = 1; } \
                               else { printf("  ok:   " __VA_ARGS__); printf("\n"); } } while (0)
#define BANK9 9u
#define DACH_BLATT 5     /* re15_map_zones.h: Zeilen 0x1170/0x1171 Spalte Blatt = 5, sonst kein Raum auf Blatt 5 */
#define DACH_BIT   42    /* Kopf re15_kartenitems_r36.h RUNDE 42 NACHARBEIT (Zensus) */
#define MIN_BREITE 40    /* Bildpunkte im 320er Bild; Kanalisationsplakat Kamera 11 = 37 (bce9c91a8, Ort vom Nutzer bestaetigt) */

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

static uint16_t le16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern), Bank 9 bleibt (Muster test_r42_lageplaene2.c). */
static void raum_start(raum_t *r, int32_t px, int32_t py, int32_t pz)
{
    re15_game_state_t flags = g_game;
    scd_vm_init();
    g_game = flags;
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = r->id; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = 0;
    re15_collision_set_band(re15_collision_band_from_y(py));
    pl->floor = (uint8_t)re15_collision_band_from_y(py);
    re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

static int prop_slot(uint8_t oid)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_id == oid) return k;
    return -1;
}

static int karten_zone(void)
{
    for (int s = 0; s < RE15_AOT_MAX; s++)
        if (g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_GENERIC &&
            g_aot.slots[s].event_id == RE15_R36K_EREIGNIS) return s;
    return -1;
}

static int haengt(uint16_t raum)
{
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(raum);
    raum_t r;
    if (raum_laden(&r, raum) != 0) return -1;
    raum_start(&r, z ? z->rect_x + RE15_R36K_RECT / 2 : 0, 0, z ? z->rect_z + RE15_R36K_RECT / 2 : 0);
    int h = prop_slot(RE15_R36K_OBJ) >= 0 && karten_zone() >= 0;
    free(r.buf);
    return h;
}

/* Plan ueber das ECHTE Port-Programm nehmen (Ereignis 28 wie der Aktions-Scan, jede Frage mit JA = Bestaetigen-Bit 0x4000). */
static int plan_nehmen(uint16_t raum)
{
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(raum);
    raum_t r;
    if (!z || raum_laden(&r, raum) != 0) return -1;
    raum_start(&r, z->rect_x + RE15_R36K_RECT / 2, 0, z->rect_z + RE15_R36K_RECT / 2);
    if (karten_zone() < 0) { free(r.buf); return 0; }
    int slot = scd_event_fire(RE15_R36K_EREIGNIS);
    if (slot < 0) { free(r.buf); return -3; }
    for (long fr = 0; fr < 4000 && g_scd.threads[slot].active; fr++) {
        const unsigned char *raw; int len, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
        g_scd_pad_held = 0;
        scd_vm_tick();
        re15_msg_tick(&raw, &len, &id);
        g_scd_pad_edge = 0;
    }
    int ok = !g_scd.threads[slot].active;
    free(r.buf);
    return ok ? 1 : -4;
}

/* Halbe Plakatbreite/-hoehe aus den 5 MD1-Ecken @0x44 (Modell-z = Breite, Modell-x = Hoehe; karten_export.py). */
static int md1_masse(uint16_t raum, int *breite, int *hoehe)
{
    int msz = 0;
    const uint8_t *md1 = re15_kartenitems_r36_md1_bytes(raum, &msz);
    int16_t ex0 = 0, ex1 = 0, ez0 = 0, ez1 = 0;
    if (!md1) return -1;
    for (int k = 0; k < 5 && 0x44 + 8 * k + 6 <= msz; k++) {
        int16_t mx = (int16_t)le16(md1 + 0x44 + 8 * k), mz = (int16_t)le16(md1 + 0x48 + 8 * k);
        if (mx < ex0) ex0 = mx; if (mx > ex1) ex1 = mx; if (mz < ez0) ez0 = mz; if (mz > ez1) ez1 = mz;
    }
    *breite = ez1 - ez0; *hoehe = ex1 - ex0;
    return 0;
}

/* Bildrechteck des Plakats (Mitte M, Raumnormale n, Groesse b x h) in Kamera cut; 1 = ganz im Bild. */
static int im_bild(const raum_t *r, int cut, const double M[3], const double n[3], double b, double h, double box[4])
{
    re15_camera_view_t v;
    if (cut >= r->rdt.cut_count || re15_camera_build_view(&r->rdt.cuts[cut], &v) != 0) return 0;
    double rx[3] = { n[2], 0, -n[0] };
    int drin = 1;
    box[0] = 1e9; box[1] = -1e9; box[2] = 1e9; box[3] = -1e9;
    for (int a = -1; a <= 1; a += 2)
        for (int c = -1; c <= 1; c += 2) {
            double p[3] = { M[0] + a * b / 2 * rx[0], M[1] + c * h / 2, M[2] + a * b / 2 * rx[2] };
            double vx = (v.rot[0] * p[0] + v.rot[1] * p[1] + v.rot[2] * p[2]) / 4096.0 + v.trans[0];
            double vy = (v.rot[3] * p[0] + v.rot[4] * p[1] + v.rot[5] * p[2]) / 4096.0 + v.trans[1];
            double vz = (v.rot[6] * p[0] + v.rot[7] * p[1] + v.rot[8] * p[2]) / 4096.0 + v.trans[2];
            if (vz <= 0) return 0;
            double sx = 160.0 + v.fov_screen_dist * vx / vz, sy = 120.0 + v.fov_screen_dist * vy / vz;
            if (sx < 0 || sx >= 320 || sy < 0 || sy >= 240) drin = 0;
            if (sx < box[0]) box[0] = sx; if (sx > box[1]) box[1] = sx;
            if (sy < box[2]) box[2] = sy; if (sy > box[3]) box[3] = sy;
        }
    return drin;
}

/* ------------------------------------------------------------------------------------------------ dach */
static void teil_dach(void)
{
    printf("=== dach: Revierplan schaltet auch das Dach frei (Blatt 5, Bit 42; Orchestrator-Entscheid 18.11.) ===\n");
    static const uint16_t raeume[2] = { 0x1030, 0x1031 };
    for (int i = 0; i < 2; i++) {
        re15_game_state_init();
        PRUEFE(!re15_map_owned_page(DACH_BLATT), "ROOM%04X frischer Stand: Blatt 5 (Dach) nicht im Besitz", raeume[i]);
        int r = plan_nehmen(raeume[i]);
        PRUEFE(r == 1, "ROOM%04X: Zone da, Port-Programm lief mit Ja zu Ende (%d)", raeume[i], r);
        PRUEFE(re15_game_flag_get(BANK9, DACH_BIT), "ROOM%04X: Bit (9,%d) gesetzt", raeume[i], DACH_BIT);
        PRUEFE(re15_map_owned_page(DACH_BLATT) && re15_map_blatt_waehlbar(DACH_BLATT),
               "ROOM%04X: Blatt 5 (Dach, ROOM1170) ohne Besuch im Besitz und waehlbar", raeume[i]);
        PRUEFE(re15_map_owned_page(2) && re15_map_owned_page(3) && re15_map_owned_page(4),
               "ROOM%04X: 1F/2F/3F weiter im Besitz (RE2 Set(33,2..4) @0x037DA-E2)", raeume[i]);
        PRUEFE(!re15_map_owned_page(0) && !re15_map_owned_page(1), "ROOM%04X: Kellerblaetter 0/1 nicht", raeume[i]);
    }
    /* alter Stand: Revierplan vor der Nacharbeit genommen (87/88/89, kein Dach-Bit) */
    re15_game_state_init();
    for (int b = 87; b <= 89; b++) re15_game_flag_set(BANK9, (uint8_t)b, 1);
    PRUEFE(!re15_map_owned_page(DACH_BLATT), "Stand mit 87/88/89: Dach noch nicht im Besitz");
    PRUEFE(haengt(0x1030) == 1 && haengt(0x1031) == 1, "Stand mit 87/88/89: Revierplan haengt in 1030/1031 (Dach fehlt)");
    PRUEFE(plan_nehmen(0x1030) == 1 && re15_map_owned_page(DACH_BLATT) && re15_game_flag_get(BANK9, DACH_BIT),
           "Stand mit 87/88/89: Aufnahme gibt das Dach dazu");
    PRUEFE(haengt(0x1030) == 0 && haengt(0x1031) == 0, "danach haengt der Revierplan nicht mehr");
}

/* ------------------------------------------------------------------------------------------------ tuerkamera */
static void teil_tuerkamera(void)
{
    printf("=== tuerkamera: RDT-Fakten des neuen Orts (Kamera 2 = Tuerkamera nach 1050) ===\n");
    static const uint16_t raeume[2] = { 0x1030, 0x1031 };
    static const uint32_t tuer_off[2] = { 0x01C6A, 0x01CD8 };   /* main00 Door_aot_set Slot 0 */
    static const uint32_t sca_off[2]  = { 0x01A1C, 0x01A80 };   /* SCA-Zelle 1 Ostwand (1031: anderer Datei-Offset)  */
    for (int i = 0; i < 2; i++) {
        raum_t r;
        if (raum_laden(&r, raeume[i]) != 0) continue;
        int n2 = 0, aus13 = 1;
        for (int k = 0; k < r.rdt.zone_count; k++) {
            const re15_rdt_zone_t *zn = &r.rdt.zones[k];
            if (k > 0 && r.rdt.zones[k - 1].cam_from == zn->cam_from && zn->cam_to == 2) {   /* Anker (erste Zone) nicht */
                n2++;
                if (zn->cam_from != 1 && zn->cam_from != 3) aus13 = 0;
            }
        }
        PRUEFE(n2 == 2 && aus13, "ROOM%04X: genau %d RVD-Zonen fuehren nach Kamera 2, nur aus 1 und 3 (1030 RVD @0x00294 / @0x0030C)",
               raeume[i], n2);
        const uint8_t *t = r.buf + tuer_off[i];
        int16_t tx = (int16_t)le16(t + 6), tz = (int16_t)le16(t + 8);
        uint16_t tw = le16(t + 10), td = le16(t + 12);
        PRUEFE(t[0] == 0x3B && t[1] == 0 && tx == 1500 && tz == -13800 && tw == 1500 && td == 2200 && t[23] == 0x05,
               "ROOM%04X @0x%05X Door_aot_set Slot 0 x %d..%d z %d..%d -> Raum 05 (1050)", raeume[i], (unsigned)tuer_off[i],
               tx, tx + tw, tz, tz + td);
        const uint8_t *e = r.buf + sca_off[i];   /* SCA-Zelle 1: w u16, d u16, x s16, z s16 */
        PRUEFE((int16_t)le16(e + 4) == 2398 && (int16_t)le16(e + 6) == -23528,
               "ROOM%04X SCA @0x%05X Ostwand Kante x %d z ab %d", raeume[i], (unsigned)sca_off[i], (int16_t)le16(e + 4),
               (int16_t)le16(e + 6));
        free(r.buf);
    }
}

/* ------------------------------------------------------------------------------------------------ revier */
static void teil_revier(void)
{
    printf("=== revier: Revierplan auf der Ostwand neben der Tuer, gross im Bild der Kamera 2 ===\n");
    static const uint16_t raeume[2] = { 0x1030, 0x1031 };
    for (int i = 0; i < 2; i++) {
        const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(raeume[i]);
        int b = 0, h = 0;
        if (!z || md1_masse(raeume[i], &b, &h) != 0) { PRUEFE(0, "ROOM%04X: keine Zeile/kein Modell", raeume[i]); continue; }
        double s = b / 1080.0;
        /* Plakat liegt 85*s vor dem Ursprung (karten_export.py); Ostwand: n = (-1,0,0) */
        double M[3] = { z->x - 85.0 * s, z->y, z->z }, n[3] = { -1, 0, 0 };
        PRUEFE(fabs(M[0] - 2398.0) < 1.0, "ROOM%04X: Plakatflaeche bei x %.0f = Ostwand 2398 (SCA @0x01A1C)", raeume[i], M[0]);
        PRUEFE(M[2] - b / 2.0 > -23528.0 && (M[2] + b / 2.0 <= -13800.0 || M[2] - b / 2.0 >= -11600.0),
               "ROOM%04X: Plakat z %.0f..%.0f auf der Wand und neben (nicht ueber) der Tuer z -13800..-11600", raeume[i],
               M[2] - b / 2.0, M[2] + b / 2.0);
        raum_t r;
        if (raum_laden(&r, raeume[i]) != 0) continue;
        double box[4];
        int drin = im_bild(&r, 2, M, n, b, h, box);
        PRUEFE(drin && box[1] - box[0] >= MIN_BREITE,
               "ROOM%04X Kamera 2: Plakat ganz im Bild x %.0f..%.0f y %.0f..%.0f = %.0f x %.0f Bildpunkte (mind. %d breit)",
               raeume[i], box[0], box[1], box[2], box[3], box[1] - box[0], box[3] - box[2], MIN_BREITE);
        free(r.buf);
    }
}

/* ------------------------------------------------------------------------------------------------ druck */
static void teil_druck(void)
{
    printf("=== druck: echter Aktionsdruck vor dem Plan (lageplaene_nach2: links der Tuer) ===\n");
    static const uint16_t raeume[2] = { 0x1030, 0x1031 };
    for (int i = 0; i < 2; i++) {
        raum_t r;
        if (raum_laden(&r, raeume[i]) != 0) continue;
        re15_game_state_init();
        const int32_t sx = 1500, sz = -11300;   /* lageplaene_nach2: vor dem Plan links der Tuer (Zone z -12010..-11010) */
        raum_start(&r, sx, 0, sz);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        const int rot = 0;   /* Vorwaertspunkt fx = x + 620 cos r (@0x80042bd0): r = 0 -> Blick +x = Ostwand */
        for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
        g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
        re15_pauseflags_clear();
        int klettert = 0;
        for (int k = -2; k <= 2; k++) {
            pl->x = sx; pl->z = sz + 200 * k; pl->rot_y = (int16_t)rot;
            re15_climb_reset();
            int kk = re15_climb_try_start(&r.rdt, 1);
            re15_climb_reset();
            if (kk) klettert++;
        }
        pl->x = sx; pl->y = 0; pl->z = sz; pl->rot_y = (int16_t)rot;
        for (int f = 0; f < 3; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
        PRUEFE(klettert == 0, "ROOM%04X: Klettersonde an 5 Standorten z %d..%d -> %d (0 = Druck bleibt fuer den Scan)",
               raeume[i], sz - 400, sz + 400, klettert);
        g_aot_action_pressed = 1;
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        g_aot_action_pressed = 0;
        int ev = g_aot.fired_event_id_this_frame;
        PRUEFE(ev == RE15_R36K_EREIGNIS, "ROOM%04X: Stand (%d,%d) Blick Ostwand -> Ereignis %d (soll %d)", raeume[i],
               sx, sz, ev, RE15_R36K_EREIGNIS);
        free(r.buf);
    }
}

/* ------------------------------------------------------------------------------------------------ fabrik */
static void teil_fabrik(void)
{
    printf("=== fabrik: Fabrikplan 30C1 doppelt so gross, gross im Bild der Kamera 1 ===\n");
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(0x30C1);
    int b = 0, h = 0;
    if (!z || md1_masse(0x30C1, &b, &h) != 0) { PRUEFE(0, "ROOM30C1: keine Zeile/kein Modell"); return; }
    PRUEFE(b == 2160 && h == 1440, "ROOM30C1: MD1-Flaeche %d x %d (soll 2160 x 1440 = 2 x RE2 karte01)", b, h);
    double s = b / 1080.0;
    double M[3] = { z->x, z->y, z->z - 85.0 * s }, n[3] = { 0, 0, -1 };   /* Nordwand z 1100, n = -z */
    PRUEFE(fabs(M[2] - 1100.0) < 1.0, "ROOM30C1: Plakatflaeche bei z %.0f = Nordwand 1100 (SCA @0x007F8)", M[2]);
    raum_t r;
    if (raum_laden(&r, 0x30C1) != 0) return;
    double box[4];
    int drin = im_bild(&r, 1, M, n, b, h, box);
    PRUEFE(drin && box[1] - box[0] >= MIN_BREITE,
           "ROOM30C1 Kamera 1: Plakat ganz im Bild x %.0f..%.0f y %.0f..%.0f = %.0f x %.0f Bildpunkte (mind. %d breit)",
           box[0], box[1], box[2], box[3], box[1] - box[0], box[3] - box[2], MIN_BREITE);
    free(r.buf);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    printf("=== Runde 42 Strang lageplaene_nach: Dach, Sichtbarkeit, Ort (Teil %s) ===\n", teil);
    int alle = strcmp(teil, "alle") == 0;
    if (alle || !strcmp(teil, "dach"))       teil_dach();
    if (alle || !strcmp(teil, "revier"))     { teil_tuerkamera(); teil_revier(); }   /* Fakten des Orts + Plakat dort */
    if (alle || !strcmp(teil, "druck"))      teil_druck();
    if (alle || !strcmp(teil, "fabrik"))     teil_fabrik();
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
