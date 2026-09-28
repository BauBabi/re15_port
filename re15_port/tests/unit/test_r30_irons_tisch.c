/* test_r30_irons_tisch.c — RIEGEL Runde 30, Thema irons-diary-welt:
 * Irons Diary (obj 5) und Memory Card (obj 6) auf dem Schreibtisch in Irons' Buero.
 * Dossier: analysis/befunde_runde30/irons-diary-welt.md (S6), Konstanten und Belege:
 * include/re15_irons_tisch.h.
 *
 * Jeder Teil faehrt den ECHTEN Raumstart des Ports (scd_room_reenter mit der ausgelieferten
 * RDT, sub00-Init-Lauf, dann re15_irons_tisch_install) — keine Nachbildung.
 *
 *   P  Props: ROOM1150 UND ROOM1151 tragen obj 5 (-23813,-1533,-18280) rot 2944 und obj 6
 *      (-23522,-1520,-18568) rot 3072, Typ 0, Band 1, Eltern -1, Flags 0x000B
 *      (0x000A | 1 @0x80040998), Nullbox. ROOM1140 traegt keines von beiden.
 *   Z  Zonen: Slot 7 ITEM x[-24000..-23000] z[-18780..-17780] Item 0x48 Menge 1 Bit 54 obj 5,
 *      Slot 8 ITEM z[-19149..-18149] Item 0x21 Menge 3 Bit 55 obj 6, sat 0x31, floor 0
 *      (x/w = Telefon-Satz ROOM1150.RDT @0x00DA6).
 *   G  schon genommen: Bit (9,54) bzw. (9,55) VOR dem Raumstart -> genau dieser Gegenstand
 *      fehlt (Prop UND Zone), der andere bleibt.
 *   D  Druck: Spieler an der Tischkante (-22664, z) mit Blick -X (rot 2048), Aktionstaste
 *      ueber re15_aot_scan (Pruefpunkt 620 voraus @0x80042bd0):
 *        z -18900 -> Item-Modal 0x21, bis zum Ende bestaetigt: 3 Stueck im Inventar,
 *                    Bit 55, Zone 8 aus, obj 6 aus (item_modal_common.c Zustand 7);
 *        z -17950 -> KEIN Item-Modal, der Aufnahme-Leser (RE2-Weg, re15_menu_request_doc);
 *                    nach Schliessen + Bestaetigen der Meldung: Zone 7 aus, Bit 54,
 *                    obj 5 aus, FILE-Platz 0 = Dokument 0.
 *      Gegenproben: Blick +X (rot 0) und Stand 300 weiter weg (x -22364) -> nichts.
 *   M  Marke: mit der Engine-Sichtmatrix von Cut 2 liegt die Buchmitte INNERHALB der roten
 *      Marke des Nutzerbilds (x 146..158, y 120..132; gemessen 2,29 px neben der Mitte
 *      (152,5;126,5) — Lage B), die Kartenmitte weniger als 1 px neben der blauen Marke
 *      (140,0;126,5).
 *   K  Tiefen-Klemme: ueber beiden Mitten liegt in Cut 2 eine Original-Maske der Tiefe 87 —
 *      in ROOM1150.RDT UND ROOM1151.RDT (Rohbytes @0x006E8/@0x006F4/@0x00714 `57 00`);
 *      ohne Klemme waeren beide verdeckt (re15_pri_mask_occludes), die Klemme liefert
 *      re15_pri_mask_camera_z(87) - 1 nur fuer obj 5/6 in Cut 2 von ROOM1150/1151. Cut 6
 *      hat keine Masken (pri_offset -> ff ff ff ff).
 *   B  Eingebackene Modelle: RE2-Buch 6 Vierecke, Keycard 2 Dreiecke; Karten-TIM mit
 *      CLUT[0] = 0x0000 und keinem weiteren 0x0000.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_files.h"
#include "re15_fade.h"
#include "re15_player.h"
#include "re15_pri.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_skeleton.h"   /* re15_cos_q12 / re15_sin_q12 */

extern re15_aot_state_t g_aot;
extern uint8_t          g_aot_action_pressed;
extern re15_inventory_t g_inv;

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

#define PAD_CONFIRM 0x4000u   /* virtuelles Bestaetigen (<- roh SQUARE, @0x80073dbc[14]) */

static uint8_t *datei(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = (size_t)sz;
    return b;
}

typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; } raum_t;
static raum_t s_r1150, s_r1151, s_r1140;

static int raum_laden(raum_t *r, const char *name)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE1/ROOM%s.RDT", RE15_ASSET_PSX_DIR, name);
    r->buf = datei(p, &r->n);
    if (!r->buf) { printf("FAIL: RDT nicht lesbar: %s\n", p); return -1; }
    if (re15_rdt_parse(r->buf, r->n, &r->rdt) != 0) { printf("FAIL: parse %s\n", p); return -1; }
    return 0;
}

/* Raumstart wie probe_r30_irons-diary-welt (raum_hochfahren) = der Tuerweg des Ports. */
static void hochfahren(raum_t *r, uint16_t room_id, int32_t px, int32_t pz, const int *bits)
{
    re15_game_state_init();
    re15_inv_init();
    re15_files_reset();
    re15_aot_init();
    scd_vm_init(); re15_actor_init();
    for (; bits && *bits; bits++) re15_game_flag_set(9, (uint8_t)*bits, 1);
    g_current_room_id = room_id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 2048; pl->state = 1;
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

static int prop_ok(int k, int32_t x, int32_t y, int32_t z, int16_t ry)
{
    if (k < 0) return 0;
    const __typeof__(g_scd.props[0]) *p = &g_scd.props[k];
    return p->active == 1 && p->obj_type == 0 && p->band == 1 && p->parent_obj == -1 &&
           p->x == x && p->y == y && p->z == z && p->rot_x == 0 && p->rot_y == ry &&
           p->rot_z == 0 && p->flags == 0x000B && p->box_hx == 0 && p->box_hy == 0 &&
           p->box_hz == 0;
}

static int zone_ok(int s, int32_t x0, int32_t z0, uint8_t item, uint8_t menge, uint8_t bit,
                   uint8_t oid)
{
    const re15_aot_t *a = &g_aot.slots[s];
    const re15_aot_item_params_t *p = &g_aot.item_params[s];
    return a->active && a->type == RE15_AOT_TYPE_ITEM && a->sce_flags == 0x31 && a->band == 0 &&
           a->x - a->half_w == x0 && a->x + a->half_w == x0 + 1000 &&
           a->z - a->half_h == z0 && a->z + a->half_h == z0 + 1000 &&
           p->item_type == item && p->amount == menge && p->taken_bit == bit &&
           p->taken_prop == oid;
}

/* ------------------------------------------------------------------ P / Z / G */
static void teil_pzg(void)
{
    struct { raum_t *r; uint16_t id; const char *n; } raeume[2] = {
        { &s_r1150, 0x1150, "ROOM1150" }, { &s_r1151, 0x1151, "ROOM1151" } };
    printf("\n[P/Z] Props und Zonen nach dem Raumstart\n");
    for (int i = 0; i < 2; i++) {
        hochfahren(raeume[i].r, raeume[i].id, -19000, -23000, NULL);
        CHECK(prop_ok(slot_von(5), -23813, -1533, -18280, 2944),
              "P %s obj 5 (Diary) bei (-23813,-1533,-18280) rot 2944, Flags 0x000B, Nullbox",
              raeume[i].n);
        CHECK(prop_ok(slot_von(6), -23522, -1520, -18568, 3072),
              "P %s obj 6 (Karte) bei (-23522,-1520,-18568) rot 3072, Flags 0x000B, Nullbox",
              raeume[i].n);
        CHECK(slot_von(0) >= 0 && slot_von(4) >= 0 && g_scd.prop_count == 7,
              "P %s Raum-Props 0..3 und Sicherung 4 bleiben, Pool %d Eintraege", raeume[i].n,
              (int)g_scd.prop_count);
        CHECK(zone_ok(7, -24000, -18780, 0x48, 1, 54, 5),
              "Z %s Slot 7: ITEM x[-24000..-23000] z[-18780..-17780] Item 0x48 x1 Bit 54 obj 5",
              raeume[i].n);
        CHECK(zone_ok(8, -24000, -19149, 0x21, 3, 55, 6),
              "Z %s Slot 8: ITEM x[-24000..-23000] z[-19149..-18149] Item 0x21 x3 Bit 55 obj 6",
              raeume[i].n);
    }
    hochfahren(&s_r1140, 0x1140, -7000, -7000, NULL);
    CHECK(slot_von(5) < 0 && slot_von(6) < 0 &&
          !(g_aot.slots[7].active && g_aot.slots[7].type == RE15_AOT_TYPE_ITEM &&
            g_aot.item_params[7].item_type == 0x48) &&
          !(g_aot.slots[8].active && g_aot.slots[8].type == RE15_AOT_TYPE_ITEM &&
            g_aot.item_params[8].item_type == 0x21),
          "P ROOM1140: weder obj 5/6 noch die Zonen");

    printf("\n[G] schon genommen\n");
    for (int i = 0; i < 2; i++) {
        static const int nur54[] = { 54, 0 }, nur55[] = { 55, 0 }, beide[] = { 54, 55, 0 };
        hochfahren(raeume[i].r, raeume[i].id, -19000, -23000, nur54);
        CHECK(slot_von(5) < 0 && !g_aot.slots[7].active &&
              prop_ok(slot_von(6), -23522, -1520, -18568, 3072) &&
              zone_ok(8, -24000, -19149, 0x21, 3, 55, 6),
              "G %s Bit (9,54) gesetzt: Diary fehlt (Prop + Zone), Karte da", raeume[i].n);
        hochfahren(raeume[i].r, raeume[i].id, -19000, -23000, nur55);
        CHECK(slot_von(6) < 0 && !g_aot.slots[8].active &&
              prop_ok(slot_von(5), -23813, -1533, -18280, 2944) &&
              zone_ok(7, -24000, -18780, 0x48, 1, 54, 5),
              "G %s Bit (9,55) gesetzt: Karte fehlt (Prop + Zone), Diary da", raeume[i].n);
        hochfahren(raeume[i].r, raeume[i].id, -19000, -23000, beide);
        CHECK(slot_von(5) < 0 && slot_von(6) < 0 && !g_aot.slots[7].active &&
              !g_aot.slots[8].active && g_scd.prop_count == 5,
              "G %s beide genommen: keines da, Pool %d", raeume[i].n, (int)g_scd.prop_count);
    }
}

/* ------------------------------------------------------------------ D */
static void menue_bild(uint16_t pressed, uint16_t held)
{
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen())
        re15_menu_fsm_tick(pressed, held);
    if (re15_menu_is_open())
        re15_inv_screen_ecg_tick();
    re15_fade_tick();
}

/* Wie probe_r30_irons-diary-welt `druck`: Raum hochfahren, einschwingen, Meldungen weg,
 * dann EIN Aktionsdruck ueber re15_aot_scan. */
static void druck(raum_t *r, uint16_t room_id, int32_t x, int32_t z, int16_t rot)
{
    hochfahren(r, room_id, x, z, NULL);
    re15_collision_ensure_band(0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
    re15_pauseflags_clear();
    pl->x = x; pl->z = z; pl->rot_y = rot;
    for (int f = 0; f < 3; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_aot_action_pressed = 1;
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    g_aot_action_pressed = 0;
}

static void teil_d(void)
{
    printf("\n[D] Aufheben mit der Aktionstaste\n");
    /* Karte */
    druck(&s_r1150, 0x1150, -22664, -18900, 2048);
    uint8_t typ = 0; int wahl = -1;
    int modal = re15_item_modal_active();
    CHECK(modal && !re15_menu_doc_active(),
          "D Karte: Stand (-22664,-18900) Blick -X -> Item-Modal aktiv, kein Leser");
    int w = 0;
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800)
        re15_item_modal_tick(0, 0);
    re15_item_modal_prompt(&typ, &wahl);
    CHECK(typ == 0x21, "D Karte: die Abfrage nennt Item 0x%02X (Soll 0x21 Memory Card)", typ);
    w = 0;
    while (re15_item_modal_active() && w++ < 800) re15_item_modal_tick((uint16_t)PAD_CONFIRM, 0);
    int ks = re15_inv_find_item(0x21);
    CHECK(ks >= 0 && g_inv.slots[ks].qty == 3,
          "D Karte: nach dem Bestaetigen 0x21 x%d im Inventar (Soll 3; RE2 Ink Ribbon, "
          "nicht halbiert: Null-Satz @0x80074C88)", ks >= 0 ? (int)g_inv.slots[ks].qty : -1);
    int k6 = slot_von(6);
    CHECK(re15_game_flag_get(9, 55) && !g_aot.slots[8].active && k6 >= 0 &&
          !g_scd.props[k6].active && g_aot.slots[7].active &&
          g_scd.props[slot_von(5)].active,
          "D Karte: Bit (9,55) gesetzt, Zone 8 aus, obj 6 aus; Diary unberuehrt");

    /* Diary */
    druck(&s_r1150, 0x1150, -22664, -17950, 2048);
    CHECK(!re15_item_modal_active() && re15_menu_doc_active() && re15_menu_stage() != 0,
          "D Diary: Stand (-22664,-17950) Blick -X -> KEIN Item-Modal, Leser angefordert");
    int bild = 0;
    while (bild < 300 && !(re15_menu_phase() == 1 && g_inv_screen.item_state == 3)) {
        menue_bild(0, 0); bild++;
    }
    CHECK(g_inv_screen.item_state == 3 && g_inv_screen.file_reader_page == 0 &&
          re15_files_get(0) == 0,
          "D Diary: Leser liest nach %d Bildern (Titelseite), FILE-Platz 0 = Dokument 0", bild);
    CHECK(!re15_game_flag_get(9, 54) && g_aot.slots[7].active && g_scd.props[slot_von(5)].active,
          "D Diary: beim Lesen liegt das Buch noch (Zone 7, obj 5, Bit 54 frei)");
    menue_bild(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    uint8_t mid = 0; int rev = -1, total = re15_menu_doc_msg_total(), n = 0;
    while (n < 400 && re15_menu_doc_msg(&mid, &rev) && rev < total) { menue_bild(0, 0); n++; }
    CHECK(mid == 0x48 && rev == total && g_aot.slots[7].active,
          "D Diary: Meldung \"filed\" steht (Id 0x%02X), noch nichts abgeraeumt", mid);
    menue_bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(re15_game_flag_get(9, 54) && !g_aot.slots[7].active && !g_scd.props[slot_von(5)].active &&
          g_aot.slots[8].active && g_scd.props[slot_von(6)].active,
          "D Diary: Bestaetigen -> Bit (9,54), Zone 7 aus, obj 5 aus; Karte unberuehrt");
    int zu = 0;
    while (zu < 200 && (re15_menu_is_open() || re15_menu_stage() != 0)) { menue_bild(0, 0); zu++; }
    CHECK(!re15_menu_is_open() && re15_files_get(0) == 0 && re15_files_count() == 1,
          "D Diary: Menue zu nach %d Bildern, \"Irons Diary\" bleibt in der FILE-Liste", zu);

    /* Gegenproben */
    druck(&s_r1150, 0x1150, -22664, -18900, 0);
    CHECK(!re15_item_modal_active() && !re15_menu_doc_active(),
          "D Gegenprobe Blick +X (rot 0): nichts");
    druck(&s_r1150, 0x1150, -22364, -18900, 2048);
    CHECK(!re15_item_modal_active() && !re15_menu_doc_active(),
          "D Gegenprobe Stand x -22364 (Pruefpunkt -22984 vor dem Rechteck): nichts");
    /* ROOM1151 (Elza-Variante) hat dieselben Zonen */
    druck(&s_r1151, 0x1151, -22664, -18900, 2048);
    CHECK(re15_item_modal_active(), "D ROOM1151 Karte: Item-Modal aktiv");
    w = 0;
    while (re15_item_modal_active() && w++ < 1600) re15_item_modal_tick((uint16_t)PAD_CONFIRM, 0);
}

/* ------------------------------------------------------------------ M / K */
static double proj(const re15_camera_view_t *v, double x, double y, double z,
                   double *sx, double *sy)
{
    double vx = (v->rot[0]*x + v->rot[1]*y + v->rot[2]*z) / 4096.0 + v->trans[0];
    double vy = (v->rot[3]*x + v->rot[4]*y + v->rot[5]*z) / 4096.0 + v->trans[1];
    double vz = (v->rot[6]*x + v->rot[7]*y + v->rot[8]*z) / 4096.0 + v->trans[2];
    *sx = 160.0 + v->fov_screen_dist * vx / vz;
    *sy = 120.0 + v->fov_screen_dist * vy / vz;
    return vz;
}

static void teil_mk(void)
{
    printf("\n[M] Marken des Nutzerbilds (Cut 2)\n");
    re15_camera_view_t v;
    CHECK(re15_camera_build_view(&s_r1150.rdt.cuts[2], &v) == 0, "M Sichtmatrix Cut 2");
    double dx, dy, kx, ky;
    double dvz = proj(&v, RE15_IRONS_DIARY_X, RE15_IRONS_DIARY_Y, RE15_IRONS_DIARY_Z, &dx, &dy);
    /* Kartenmitte: Modellmitte (-80,5 ; 0 ; 135) des Keycard-MD1, gedreht wie der Prop-
     * Zeichner (pc_prop_rot_q12, reines rot_y: x' = c*x + s*z, z' = -s*x + c*z). */
    double c = re15_cos_q12(RE15_IRONS_KARTE_ROT_Y) / 4096.0;
    double s = re15_sin_q12(RE15_IRONS_KARTE_ROT_Y) / 4096.0;
    double mx = RE15_IRONS_KARTE_X + (c * -80.5 + s * 135.0);
    double mz = RE15_IRONS_KARTE_Z + (-s * -80.5 + c * 135.0);
    double kvz = proj(&v, mx, RE15_IRONS_KARTE_Y, mz, &kx, &ky);
    double ad = hypot(dx - 152.5, dy - 126.5), ak = hypot(kx - 140.0, ky - 126.5);
    CHECK(dx >= 146.0 && dx <= 158.0 && dy >= 120.0 && dy <= 132.0 && ad < 2.5,
          "M Buchmitte (%.2f ; %.2f) in der roten Marke x146..158 y120..132, %.2f px neben "
          "der Mitte (Lage B, gemessen 2,29)", dx, dy, ad);
    CHECK(ak < 1.0, "M Kartenmitte (%.2f ; %.2f) %.2f px neben der blauen Marke (140,0;126,5)",
          kx, ky, ak);

    printf("\n[K] Tiefen-Klemme gegen die Tischmasken\n");
    raum_t *rr[2] = { &s_r1150, &s_r1151 };
    for (int i = 0; i < 2; i++) {
        static const uint32_t off[3] = { 0x6E8, 0x6F4, 0x714 };
        int roh = 1;
        for (int k = 0; k < 3; k++)
            roh &= rr[i]->buf[off[k] + 4] == 0x57 && rr[i]->buf[off[k] + 5] == 0x00;
        CHECK(roh, "K %s Rohbytes @0x006E8/@0x006F4/@0x00714 +4 = 57 00 (Tiefe 87)",
              i ? "ROOM1151" : "ROOM1150");
        static re15_pri_cut_t pri;
        memset(&pri, 0, sizeof pri);
        int nm = re15_pri_parse_section(rr[i]->buf, rr[i]->n, rr[i]->rdt.cuts[2].pri_offset, &pri);
        double pts[2][2] = { { dx, dy }, { kx, ky } };
        for (int p = 0; p < 2; p++) {
            int tief = 9999;
            for (int m = 0; m < pri.draw_count && m < nm; m++) {
                const re15_pri_mask_t *q = &pri.masks[m];
                if (pts[p][0] >= q->dstX && pts[p][0] < q->dstX + q->width &&
                    pts[p][1] >= q->dstY && pts[p][1] < q->dstY + q->height && q->depth < tief)
                    tief = q->depth;
            }
            CHECK(tief == RE15_IRONS_KLEMME_TIEFE,
                  "K %s Cut 2: flachste Maske ueber der %s-Mitte hat Tiefe %d (Soll 87)",
                  i ? "ROOM1151" : "ROOM1150", p ? "Karten" : "Buch", tief);
        }
        const uint8_t *c6 = rr[i]->buf + rr[i]->rdt.cuts[6].pri_offset;
        CHECK(c6[0] == 0xFF && c6[1] == 0xFF && c6[2] == 0xFF && c6[3] == 0xFF,
              "K %s Cut 6 ohne Masken (pri_offset 0x%X -> ff ff ff ff)",
              i ? "ROOM1151" : "ROOM1150", (unsigned)rr[i]->rdt.cuts[6].pri_offset);
    }
    CHECK(re15_pri_mask_occludes(87, (long)dvz) && re15_pri_mask_occludes(87, (long)kvz),
          "K ohne Klemme verdeckt: vz Buch %.0f / Karte %.0f liegen hinter Tiefe 87", dvz, kvz);
    int km = re15_irons_tisch_sort_max(0x1150, 2, 5);
    CHECK(km == (int)re15_pri_mask_camera_z(87) - 1 && km == 5636 &&
          (float)km < re15_pri_mask_camera_z(87),
          "K Klemme obj 5 Cut 2 = %d (= re15_pri_mask_camera_z(87) - 1, vor der Maske)", km);
    CHECK(re15_irons_tisch_sort_max(0x1151, 2, 6) == 5636 &&
          re15_irons_tisch_sort_max(0x1150, 6, 5) == -1 &&
          re15_irons_tisch_sort_max(0x1150, 2, 4) == -1 &&
          re15_irons_tisch_sort_max(0x1150, 2, 0) == -1 &&
          re15_irons_tisch_sort_max(0x1140, 2, 5) == -1,
          "K Klemme NUR obj 5/6, NUR Cut 2, NUR ROOM1150/1151");
}

/* ------------------------------------------------------------------ B */
static void teil_b(void)
{
    printf("\n[B] eingebackene Modelle\n");
    static re15_md1_t md;
    re15_tim_t t;
    int n = 0, tn = 0;
    const uint8_t *m = re15_irons_tisch_md1_bytes(5, &n);
    const uint8_t *tb = re15_irons_tisch_tim_bytes(5, &tn);
    CHECK(m && n == 372 && re15_md1_parse(m, n, &md) == 0 && md.mesh_count == 1 &&
          md.meshes[0].quad_count == 6 && md.meshes[0].triangle_count == 0,
          "B Diary-MD1 372 B (RE2 ROOM10E0 @0x002320): 1 Mesh, 6 Vierecke");
    CHECK(tb && tn == 17440 && re15_tim_parse(tb, tn, &t) == 0 && t.bpp == 8 &&
          t.width == 128 && t.height == 128,
          "B Diary-TIM 17440 B (RE2 ROOM10E0 @0x015224): 8 bpp 128x128");
    m = re15_irons_tisch_md1_bytes(6, &n);
    tb = re15_irons_tisch_tim_bytes(6, &tn);
    CHECK(m && n == 156 && re15_md1_parse(m, n, &md) == 0 && md.mesh_count == 1 &&
          md.meshes[0].triangle_count == 2,
          "B Karten-MD1 156 B (Keycard ROOM1110 @0x0013D8): 2 Dreiecke");
    int nullen = -1;
    if (tb && tn == 8736 && re15_tim_parse(tb, tn, &t) == 0 && t.has_clut) {
        nullen = 0;
        for (int i = 1; i < t.clut_entries; i++) if (t.clut[i] == 0) nullen++;
    }
    CHECK(nullen == 0 && t.clut[0] == 0, "B Karten-TIM 8736 B: CLUT[0] = 0x0000 (Farbschluessel), "
          "sonst %d Nullen", nullen);
    CHECK(re15_irons_tisch_md1_bytes(4, &n) == NULL && re15_irons_tisch_tim_bytes(7, &n) == NULL,
          "B andere obj_id -> kein Modell");
}

int main(void)
{
    if (raum_laden(&s_r1150, "1150") || raum_laden(&s_r1151, "1151") ||
        raum_laden(&s_r1140, "1140"))
        return 1;
    teil_pzg();
    teil_d();
    teil_mk();
    teil_b();
    printf("\n%s: %d Fehler\n", fails ? "FAIL" : "OK", fails);
    return fails ? 1 : 0;
}
