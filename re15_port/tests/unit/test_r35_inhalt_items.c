/*
 * Runde 35 Spur F, Punkte 4 und 5 — Memory Card im Regal ROOM1010 (add_card.bmp) und Shotgun
 * Shells auf dem rechten Aussenluefter ROOM1090 (Shotgun.bmp). Dossier F_inhalt.md Punkte 4/5.
 * Konstanten: include/re15_inhalt_r35.h. Misst ueber die ECHTEN Engine-Wege:
 *   P  nach scd_room_reenter (alle Port-Installer) liegt das Prop obj 4 an seiner Lage, Flags 0x000B
 *   Z  die Aufhebe-Zone (Slot, Item, Menge, Bit, Etage) — in beiden Varianten (1010/1011, 1090/1091)
 *   G  Bank-9-Bit gesetzt -> weder Prop noch Zone
 *   M  die Projektion der Lage im Cut des Nutzerbilds liegt in der roten Marke (re15_camera_build_view)
 *   V  (Nachbesserung 1, M2) die Memory Card ist in Cut 4 — der Kamera, die im Spiel am Regal aktiv
 *      ist — eine zugewandte Flaeche >= 40 px^2 (vorher flach 11,2 px^2) und in Cut 7 >= 100 px^2;
 *      Ecken ueber die Prop-Drehung des Ports (pc_prop_rot_q12, main.c:670-694, Welt = m * v)
 *   D  ein echter Aktionsdruck (re15_aot_scan, 620 voraus) vom Standort vor dem Regal / Luefter
 *      oeffnet das Aufnahme-Modal mit dem Item; "Ja" -> Bit gesetzt, Item im Inventar
 *   E  das Modell (eingebackene MD1/TIM) parst
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_inhalt_r35.h"
#include "re15_irons_tisch.h"
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_item_modal.h"
#include "re15_inventory.h"
#include "re15_files.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_skeleton.h"     /* re15_sin_q12 / re15_cos_q12 (Prop-Drehung) */

extern re15_aot_state_t g_aot;
extern uint8_t          g_aot_action_pressed;

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; uint16_t id; } raum_t;

static int raum_laden(raum_t *r, uint16_t id)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(id >> 12), (unsigned)id);
    FILE *f = fopen(p, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    r->buf = (uint8_t *)malloc((size_t)sz);
    if (!r->buf || fread(r->buf, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); return -1; }
    fclose(f);
    r->n = (size_t)sz; r->id = id;
    return re15_rdt_parse(r->buf, r->n, &r->rdt) == 0 ? 0 : -1;
}

static void hochfahren(raum_t *r, int32_t px, int32_t py, int32_t pz, int bit)
{
    re15_game_state_init();
    re15_inv_init();
    re15_files_reset();
    re15_aot_init();
    scd_vm_init(); re15_actor_init();
    if (bit) re15_game_flag_set(9, (uint8_t)bit, 1);
    g_current_room_id = r->id;
    g_room_rdt = r->rdt; g_room_rdt_ok = 1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = 0; pl->state = 1;
    re15_collision_set_band(re15_collision_band_from_y(py));
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size); }
    scd_register_room_events(&r->rdt);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

static int slot_von(uint8_t oid)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == oid) return k;
    return -1;
}

static void druck(raum_t *r, int32_t x, int32_t y, int32_t z, int16_t rot)
{
    hochfahren(r, x, y, z, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->floor = (uint8_t)re15_collision_band_from_y(y);
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
    re15_pauseflags_clear();
    pl->x = x; pl->y = y; pl->z = z; pl->rot_y = rot;
    for (int f = 0; f < 3; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    g_aot_action_pressed = 0;
}

static double proj(const re15_camera_view_t *v, double x, double y, double z, double *sx, double *sy)
{
    double vx = (v->rot[0]*x + v->rot[1]*y + v->rot[2]*z) / 4096.0 + v->trans[0];
    double vy = (v->rot[3]*x + v->rot[4]*y + v->rot[5]*z) / 4096.0 + v->trans[1];
    double vz = (v->rot[6]*x + v->rot[7]*y + v->rot[8]*z) / 4096.0 + v->trans[2];
    *sx = 160.0 + v->fov_screen_dist * vx / vz;
    *sy = 120.0 + v->fov_screen_dist * vy / vz;
    return vz;
}

/* Nachbildung von pc_prop_rot_q12 (platform/pc/main.c:670-694), Welt = m * v. */
static void prop_rot(int rx, int ry, int rz, int32_t m[9])
{
    int32_t sx = re15_sin_q12(rx), cx = re15_cos_q12(rx);
    int32_t sy = re15_sin_q12(ry), cy = re15_cos_q12(ry);
    int32_t sz = re15_sin_q12(rz), cz = re15_cos_q12(rz);
    m[0] = (int32_t)(((int64_t)cz * cy) >> 12);
    m[1] = -(int32_t)(((int64_t)sz * cy) >> 12);
    m[2] = sy;
    m[3] = (int32_t)((((int64_t)sz * cx << 12) + (int64_t)cz * sy * sx) >> 24);
    m[4] = (int32_t)((((int64_t)cz * cx << 12) - (int64_t)sz * sy * sx) >> 24);
    m[5] = -(int32_t)(((int64_t)cy * sx) >> 12);
    m[6] = (int32_t)((((int64_t)sz * sx << 12) - (int64_t)cz * sy * cx) >> 24);
    m[7] = (int32_t)((((int64_t)cz * sx << 12) + (int64_t)sz * sy * cx) >> 24);
    m[8] = (int32_t)(((int64_t)cy * cx) >> 12);
}

typedef struct {
    const char *name; uint16_t raum; uint8_t item, menge, bit, slot, obj, floor;
    int32_t x, y, z; int16_t rotx, rot, rotz;
    int32_t rx, rz, rw, rd;
    int32_t mitte_x, mitte_y, mitte_z;     /* sichtbare Mitte des Gegenstands (Dossier) */
    int cut, mx0, mx1, my0, my1;           /* rote Marke im Nutzerbild (marken.py) */
    int32_t stand_x, stand_y, stand_z; int16_t stand_rot;
} soll_t;

/* Soll unabhaengig vom Kopf aufgeschrieben (der Riegel soll eine Kopf-Aenderung bemerken). */
static const soll_t k_soll[2] = {
    /* Memory Card: Regal ROOM1010 Cut 7, Marke x 269..278 y 158..170; steht auf Brett B (Nachbesserung 1),
     * Kartenmitte (3719,-1855,-1299); Stand vor dem Regal (x < 3600 = Regalfront), Blick +x (yaw 0)
     * -> 620 voraus x 3920 */
    { "Memory Card", 0x1010, 0x21, 3, 80, 10, 4, 0, 3748, -1725, -1383, 896, 128, 3584, 3219, -1799, 1000, 1000,
      3719, -1855, -1299, 7, 269, 278, 158, 170, 3300, 0, -1033, 0 },
    /* Shotgun Shells: rechter Luefter ROOM1090 Cut 2, Marke x 98..117 y 127..146; Kistenmitte
     * (-2408,-10932,-15500); Stand vor dem Luefter (z > -15397), Blick -z (yaw 1024), Boden -9000 */
    { "Shotgun Shells", 0x1090, 0x16, 7, 81, 4, 4, 5, -2408, -10761, -15500, 0, 0, 0, -2908, -16000, 1000, 1000,
      -2408, -10932, -15500, 2, 98, 117, 127, 146, -2408, -9000, -14900, 1024 },
};

int main(void)
{
    raum_t r[4];
    static const uint16_t ids[4] = { 0x1010, 0x1011, 0x1090, 0x1091 };
    for (int i = 0; i < 4; i++)
        if (raum_laden(&r[i], ids[i]) != 0) { printf("FAIL: ROOM%04X nicht lesbar\n", ids[i]); return 1; }

    for (int k = 0; k < 2; k++) {
        const soll_t *s = &k_soll[k];
        printf("\n[%s] ROOM%04X\n", s->name, s->raum);
        for (int v = 0; v < 2; v++) {
            raum_t *rr = &r[k * 2 + v];
            hochfahren(rr, s->stand_x, s->stand_y, s->stand_z, 0);
            int p = slot_von(s->obj);
            int pok = p >= 0 && g_scd.props[p].x == s->x && g_scd.props[p].y == s->y &&
                      g_scd.props[p].z == s->z && g_scd.props[p].rot_y == s->rot &&
                      g_scd.props[p].rot_x == s->rotx && g_scd.props[p].rot_z == s->rotz &&
                      g_scd.props[p].flags == 0x000B && g_scd.props[p].parent_obj == -1;
            CHECK(pok,
                  "P ROOM%04X obj %d bei (%d,%d,%d) rot (%d,%d,%d), Flags 0x000B, Welt", rr->id, s->obj, s->x, s->y, s->z,
                  s->rotx, s->rot, s->rotz);
            const re15_aot_t *a = &g_aot.slots[s->slot];
            CHECK(a->active && a->type == RE15_AOT_TYPE_ITEM && g_aot.item_params[s->slot].item_type == s->item &&
                  g_aot.item_params[s->slot].amount == s->menge && a->sce_flags == 0x31 && a->band == s->floor &&
                  a->x == s->rx + s->rw / 2 && a->z == s->rz + s->rd / 2 && a->half_w == s->rw / 2 && a->half_h == s->rd / 2,
                  "Z ROOM%04X Slot %d: Item 0x%02X x%d, sat 0x31, Etage %d, Rechteck x[%d..%d] z[%d..%d]",
                  rr->id, s->slot, s->item, s->menge, s->floor, s->rx, s->rx + s->rw, s->rz, s->rz + s->rd);
            hochfahren(rr, s->stand_x, s->stand_y, s->stand_z, s->bit);
            CHECK(slot_von(s->obj) < 0 && !(g_aot.slots[s->slot].active &&
                  g_aot.slots[s->slot].type == RE15_AOT_TYPE_ITEM &&
                  g_aot.item_params[s->slot].item_type == s->item),
                  "G ROOM%04X Bit (9,%d) gesetzt: weder Prop noch Zone", rr->id, s->bit);
        }
        /* M: Projektion in die Marke */
        re15_camera_view_t view; double sx = -1, sy = -1;
        if (re15_camera_build_view(&r[k * 2].rdt.cuts[s->cut], &view) == 0)
            proj(&view, s->mitte_x, s->mitte_y, s->mitte_z, &sx, &sy);
        CHECK(sx >= s->mx0 && sx <= s->mx1 + 1 && sy >= s->my0 && sy <= s->my1 + 1,
              "M ROOM%04X Cut %d: Mitte (%.2f ; %.2f) in der Marke x %d..%d y %d..%d",
              s->raum, s->cut, sx, sy, s->mx0, s->mx1, s->my0, s->my1);
        /* V (nur Memory Card): Flaeche in Cut 4 (im Spiel aktiv) und Cut 7, zugewandte Seite */
        if (s->item == 0x21) {
            static const int qv[4][3] = { {0,0,0}, {0,0,270}, {-161,0,270}, {-161,0,0} };  /* ROOM1110 @0x0013D8 */
            int32_t m[9]; prop_rot(s->rotx, s->rot, s->rotz, m);
            double nn[3] = { -m[1] / 4096.0, -m[4] / 4096.0, -m[7] / 4096.0 };   /* m * (0,-1,0) */
            const int cuts[2] = { 4, 7 }; const double minf[2] = { 40.0, 100.0 };
            for (int ci = 0; ci < 2; ci++) {
                re15_camera_view_t cv;
                double px[4], py[4], fl = 0, zug = -1;
                if (re15_camera_build_view(&r[0].rdt.cuts[cuts[ci]], &cv) == 0) {
                    for (int e = 0; e < 4; e++) {
                        double wx = s->x + (m[0]*qv[e][0] + m[1]*qv[e][1] + m[2]*qv[e][2]) / 4096.0;
                        double wy = s->y + (m[3]*qv[e][0] + m[4]*qv[e][1] + m[5]*qv[e][2]) / 4096.0;
                        double wz = s->z + (m[6]*qv[e][0] + m[7]*qv[e][1] + m[8]*qv[e][2]) / 4096.0;
                        proj(&cv, wx, wy, wz, &px[e], &py[e]);
                    }
                    for (int e = 0; e < 4; e++) fl += px[e] * py[(e + 1) % 4] - px[(e + 1) % 4] * py[e];
                    fl = fabs(fl) / 2.0;
                    /* Blickrichtung: Kamera-z-Achse in Weltkoordinaten = Zeile 2 der Sichtmatrix */
                    zug = -(nn[0] * cv.rot[6] + nn[1] * cv.rot[7] + nn[2] * cv.rot[8]) / 4096.0;
                }
                CHECK(fl >= minf[ci] && zug > 0.5,
                      "V ROOM1010 Cut %d: Memory Card %.1f px^2 (>= %.0f), Sichtseite zugewandt %.2f", cuts[ci], fl,
                      minf[ci], zug);
            }
        }
        /* D: echter Aktionsdruck in beiden Varianten */
        for (int v = 0; v < 2; v++) {
            raum_t *rr = &r[k * 2 + v];
            druck(rr, s->stand_x, s->stand_y, s->stand_z, s->stand_rot);
            uint8_t typ = 0; int wahl = -1, w = 0;
            int modal = re15_item_modal_active();
            while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800)
                re15_item_modal_tick(0, 0);
            re15_item_modal_prompt(&typ, &wahl);
            CHECK(modal && typ == s->item, "D ROOM%04X Druck (%d,%d) yaw %d (Etage %d): Aufnahme-Modal Item 0x%02X",
                  rr->id, s->stand_x, s->stand_z, s->stand_rot, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].floor, typ);
            w = 0;
            while (re15_item_modal_active() && w++ < 1600) re15_item_modal_tick((uint16_t)0x4000u, 0);
            int menge = 0;
            for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == s->item) menge += g_inv.slots[i].qty;
            CHECK(re15_game_flag_get(9, s->bit) && menge > 0,
                  "D ROOM%04X Ja: Bit (9,%d) gesetzt, %s im Inventar (Menge %d)", rr->id, s->bit, s->name, menge);
        }
        /* E: Modell */
        int mn = 0, tn = 0;
        const uint8_t *mb = re15_inhalt_r35_md1_bytes(s->raum, &mn);
        const uint8_t *tb = re15_inhalt_r35_tim_bytes((uint16_t)(s->raum | 1), &tn);
        static re15_md1_t md; re15_tim_t tim;
        memset(&md, 0, sizeof md); memset(&tim, 0, sizeof tim);
        CHECK(mb && tb && re15_md1_parse(mb, mn, &md) == 0 && re15_tim_parse(tb, tn, &tim) == 0 &&
              re15_inhalt_r35_obj_id(s->raum) == s->obj && re15_inhalt_r35_obj_id((uint16_t)(s->raum | 1)) == s->obj,
              "E ROOM%04X/%04X Modell: MD1 %d B + TIM %d B parsen, obj %d", s->raum, s->raum | 1, mn, tn, s->obj);
    }
    CHECK(re15_inhalt_r35_obj_id(0x1020) < 0 && re15_inhalt_r35_md1_bytes(0x1020, NULL) == NULL,
          "F fremder Raum ROOM1020: kein Prop");

    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
