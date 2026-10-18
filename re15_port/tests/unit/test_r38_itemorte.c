/* test_r38_itemorte.c — Runde 38 Spur K "itemorte": Feuerloescher am Pfeiler ROOM1030 (statt ROOM1050), Parking Lot
 * Key (Leon ROOM10F0) / Prison Key (Elza ROOM10F1) gut sichtbar an der Pinnwand.
 * Dossier: analysis/befunde_runde38/K_itemorte.md, Konstanten include/re15_itemorte_r38.h (+ gen/itemorte_r38_lage.inc).
 *
 * Jeder Teil misst die MECHANIK am echten Port-Code (echte RDT, scd_room_reenter mit allen Port-Installern, Engine-
 * Kamera re15_camera_build_view, echte AOT-Scans, Item-Modal, PRI-Masken des Originals):
 *   lage      Loescher: das gezeichnete Netz (MD1 stehend mit Prop-Drehung/-Lage) liegt im Nutzer-Cut ROOM1030 Cut 0 AUF
 *             der Marke (Mitte +-1 px, Spalten der Marke), Rueckseite 15 vor der triangulierten Pfeilerflaeche x -2688,
 *             ganz auf ihrer z-Spanne. Schluessel: JEDER sichtbare Texel (CLUT-Farbe != 0) des gezeichneten Netzes
 *             liegt im Nutzer-Cut ROOM10F0 Cut 5 in der Marke; Breite der gezeichneten Huelle = RE2-Median (Runde 39
 *             Spur L "kleiner/subtiler", bis Runde 38: fuellt die Marke >= 9 von 11 Spalten); Licht flach (0x0010,
 *             RE2 Obj_model_set @0x80055320 aus pc[12..13], ROOM6150 @0x02C98 / ROOM1050 @0x01274)
 *   raum      Raumstart 1030 (Prop + Zone Slot 14, Monitor-Satz nach 20), 1031/1050/1051 kein Loescher; 10F0/10F1 Prop +
 *             Zone Slot 8, Satz msg 4 nach 15; mit Taken-Bit nichts; Installer idempotent
 *   aufnahme  echter Druck vor dem Pfeiler -> Modal 0x31 -> Ja -> (9,0x86) + (3,0x85); danach antwortet an derselben
 *             Stelle der Monitor-Satz (Slot 20, msg 14) — die genommene Zone (sce 0) wird uebersprungen (@0x80042f50);
 *             vorher antwortet er ausserhalb des Zonen-Rechtecks. Dasselbe an der Pinnwand 10F0/10F1 (msg 4, Slot 15),
 *             Prison Key setzt (4,0xEC)
 *   sicht     Sichtbarkeitsregel re15_itemorte_r38_verdeckt + WARUM: mit (4,0x0F) tauscht sub00 Cut 3 -> 10
 *             (@0x01FF7), das Anker-Viereck von Cut 10 enthaelt den Loescher (FUN_80014368 zeichnet ihn), keine Maske des
 *             Cuts verdeckt ihn bei seiner Tiefe (Pfeiler-Maske 2 erst ab vz 10570), der Sehstrahl aus Kamera 3 geht durch
 *             den Pfeiler; ROOM10F0 Cut 7: Anker-Viereck enthaelt den Schluessel, der Cut hat 0 Masken
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_itemorte_r38.h"
#include "re15_feuerloescher_r37.h"
#include "re15_schluessel_r36.h"
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
#include "re15_md1.h"
#include "re15_msg.h"
#include "re15_gameflow.h"
#include "re15_tim.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_itembox.h"
#include "re15_pri.h"
#include "re15_pri1100_r37.h"

extern re15_aot_state_t g_aot;
extern uint8_t          g_aot_action_pressed;
extern int scd_event_fire(uint8_t event_id);

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

/* Nutzer-Marken (analysis/befunde_runde38/nutzer/*.bmp, exakte Markenfarbe (237,28,36), volle Rechtecke) */
#define FL_MX0 38
#define FL_MY0 111
#define FL_MX1 48
#define FL_MY1 122
#define KS_MX0 133
#define KS_MY0 77
#define KS_MX1 143
#define KS_MY1 85
/* Pfeiler ROOM1030 (Dossier 2.2, trianguliert Cut 0/1/3): -x-Flaeche x -2688 zwischen den Ecken z -18459 .. -17564 */
#define PF_X0 (-2688)
#define PF_X1 (-1790)
#define PF_ZA (-18459)
#define PF_ZB (-17564)

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

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern); frisches Spiel mit `flags` (Paare
 * bank,bit; Ende < 0). Muster test_r37_feuerloescher.c start(). */
static int32_t s_px, s_pz; static int16_t s_prot;
static void start(raum_t *r, int32_t px, int32_t pz, int16_t rot, const int *flags, int elza)
{
    re15_game_state_init();
    re15_inv_init();
    re15_itembox_init();
    re15_files_reset();
    re15_discard_reset();
    re15_aot_init();
    scd_vm_init(); re15_actor_init();
    for (int k = 0; flags && flags[k] >= 0; k += 2) re15_game_flag_set((uint8_t)flags[k], (uint8_t)flags[k + 1], 1);
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
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = rot;
    s_px = px; s_pz = pz; s_prot = rot;
    re15_collision_set_band(0);
    g_scd.player_mode = 0;
    re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_register_room_events(&r->rdt);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

/* n Bilder laufen lassen, Nachricht/Freeze loesen, Spieler zurueck an seine Stelle (Muster test_r36_schluessel.c). */
static void ruhe(int n)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < n; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
    g_scd.message_fsm_active = 0;
    re15_pauseflags_clear();
    pl->x = s_px; pl->z = s_pz; pl->rot_y = s_prot;
}

static void stelle(int32_t x, int32_t z, int16_t rot)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_px = x; s_pz = z; s_prot = rot;
    pl->x = x; pl->z = z; pl->rot_y = rot;
}

/* EIN Aktionsdruck wie im Spiel: Flanke -> re15_aot_scan (Muster test_r36_schluessel.c druck). */
static void druck(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    g_aot_action_pressed = 0;
    if (g_aot.fired_event_id_this_frame) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
}

/* Aufnahme-Modal bis zur Frage, Typ zurueck; ja=1 beantwortet mit Ja (Muster test_r37_feuerloescher.c aufnahme). */
static uint8_t modal_fahren(int ja)
{
    uint8_t typ = 0; int wahl = -1, w = 0;
    if (!re15_item_modal_active()) return 0;
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800) re15_item_modal_tick(0, 0);
    re15_item_modal_prompt(&typ, &wahl);
    w = 0;
    while (ja && re15_item_modal_active() && w++ < 1600) re15_item_modal_tick((uint16_t)0x4000u, 0);
    return typ;
}

static int slot_von(uint8_t oid)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == oid) return k;
    return -1;
}

/* ---- Projektion wie die Sonde probe_r34n_e_dokumente (Engine-Matrix, H = fov_screen_dist) ------------------------ */
static int view_von(const raum_t *r, int cut, re15_camera_view_t *v)
{
    return (cut >= 0 && cut < r->rdt.cut_count) ? re15_camera_build_view(&r->rdt.cuts[cut], v) : -1;
}

static double projiziere(const re15_camera_view_t *v, double x, double y, double z, double *sx, double *sy)
{
    double vx = (v->rot[0] * x + v->rot[1] * y + v->rot[2] * z) / 4096.0 + v->trans[0];
    double vy = (v->rot[3] * x + v->rot[4] * y + v->rot[5] * z) / 4096.0 + v->trans[1];
    double vz = (v->rot[6] * x + v->rot[7] * y + v->rot[8] * z) / 4096.0 + v->trans[2];
    if (vz <= 1) { *sx = *sy = -1; return vz; }
    *sx = 160.0 + v->fov_screen_dist * vx / vz;
    *sy = 120.0 + v->fov_screen_dist * vy / vz;
    return vz;
}

/* Prop-Drehung wie pc_prop_rot_q12 (platform/pc/main.c, Ry*Rx*Rz) */
static void prop_matrix(int rx, int ry, int rz, double m[9])
{
    const double k = 2.0 * 3.14159265358979323846 / 4096.0;
    double sx = sin(rx * k), cx = cos(rx * k), sy = sin(ry * k), cy = cos(ry * k), sz = sin(rz * k), cz = cos(rz * k);
    m[0] = cz * cy;                 m[1] = -sz * cy;                m[2] = sy;
    m[3] = sz * cx + cz * sy * sx;  m[4] = cz * cx - sz * sy * sx;  m[5] = -cy * sx;
    m[6] = sz * sx - cz * sy * cx;  m[7] = cz * sx + sz * sy * cx;  m[8] = cy * cx;
}

static void welt(const double m[9], const int32_t pos[3], double x, double y, double z, double w[3])
{
    w[0] = m[0] * x + m[1] * y + m[2] * z + pos[0];
    w[1] = m[3] * x + m[4] * y + m[5] * z + pos[1];
    w[2] = m[6] * x + m[7] * y + m[8] * z + pos[2];
}

/* Masken eines Cuts wie die Sonde masken_des_cuts (Original-Sektion, sonst MSK-Seitendaten wie der Lader). */
static int masken_des_cuts(const raum_t *r, int cut, re15_pri_cut_t *pri)
{
    memset(pri, 0, sizeof *pri);
    int n = re15_pri_parse_section(r->buf, r->n, r->rdt.cuts[cut].pri_offset, pri);
    if (n > 0) return n;
    char path[600];
    snprintf(path, sizeof path, "%s/MASKS/ROOM%04X.MSK", RE15_ASSET_PSX_DIR, re15_pri_masken_raum((unsigned)r->id));
    int ms = 0;
    uint8_t *mb = slurp(path, &ms);
    if (!mb) return 0;
    uint32_t off = re15_pri_msk_section_offset(mb, (size_t)ms, cut);
    if (off) n = re15_pri_parse_section(mb, (size_t)ms, off, pri);
    free(mb);
    return n > 0 ? n : 0;
}

/* ============================================================================================================ lage */
static void teil_lage(void)
{
    raum_t r30, rf0, rf1;
    printf("\n[lage]\n");
    if (raum_laden(&r30, 0x1030) || raum_laden(&rf0, 0x10F0) || raum_laden(&rf1, 0x10F1)) return;

    /* --- Loescher: das gezeichnete Netz im Nutzer-Cut ROOM1030 Cut 0 --- */
    start(&r30, -11500, -19500, 2048, NULL, 0);
    int p = slot_von(RE15_FL37_OBJ);
    int sz = 0;
    const uint8_t *mb = re15_feuerloescher_r37_md1_bytes(0x1030, &sz);
    static re15_md1_t md;
    re15_camera_view_t v;
    CHECK(p >= 0 && mb && re15_md1_parse(mb, sz, &md) == 0 && md.mesh_count == 1 && view_von(&r30, 0, &v) == 0,
          "L 1030: Prop obj 5 da, stehendes Netz parst, Kamera Cut 0");
    if (p >= 0 && md.mesh_count == 1) {
        double m[9]; int32_t pos[3] = { g_scd.props[p].x, g_scd.props[p].y, g_scd.props[p].z };
        prop_matrix(g_scd.props[p].rot_x, g_scd.props[p].rot_y, g_scd.props[p].rot_z, m);
        double x0 = 1e9, x1 = -1e9, y0 = 1e9, y1 = -1e9, wx1 = -1e9, wz0 = 1e9, wz1 = -1e9;
        const re15_md1_mesh_t *h = &md.meshes[0];
        for (int k = 0; k < h->quad_vertex_count; k++) {
            double w[3], sx, sy;
            welt(m, pos, h->quad_vertices[k].x, h->quad_vertices[k].y, h->quad_vertices[k].z, w);
            projiziere(&v, w[0], w[1], w[2], &sx, &sy);
            if (sx < x0) x0 = sx;
            if (sx > x1) x1 = sx;
            if (sy < y0) y0 = sy;
            if (sy > y1) y1 = sy;
            if (w[0] > wx1) wx1 = w[0];
            if (w[2] < wz0) wz0 = w[2];
            if (w[2] > wz1) wz1 = w[2];
        }
        double mx = (x0 + x1) / 2.0, my = (y0 + y1) / 2.0;
        printf("      Loescher Cut 0: Bild x %.1f..%.1f y %.1f..%.1f (Mitte %.1f ; %.1f), Welt x bis %.0f, z %.0f..%.0f\n",
               x0, x1, y0, y1, mx, my, wx1, wz0, wz1);
        CHECK(fabs(mx - (FL_MX0 + FL_MX1 + 1) / 2.0) <= 1.0 && fabs(my - (FL_MY0 + FL_MY1 + 1) / 2.0) <= 1.0,
              "L 1030: Mitte des gezeichneten Loeschers (%.1f ; %.1f) auf der Markenmitte (43,5 ; 117,0) +-1 px", mx, my);
        CHECK(x0 >= FL_MX0 - 0.5 && x1 <= FL_MX1 + 1.5,
              "L 1030: Loescher in den Spalten der Marke (x %.1f..%.1f, Marke %d..%d)", x0, x1, FL_MX0, FL_MX1);
        CHECK(fabs(wx1 - (PF_X0 - 15)) <= 2.0,
              "L 1030: Rueckseite x %.0f = 15 vor der Pfeilerflaeche x %d (Runde 37: R + 15 vor der Wand)", wx1, PF_X0);
        CHECK(wz0 >= PF_ZA && wz1 <= PF_ZB,
              "L 1030: Loescher ganz auf der -x-Flaeche (z %.0f..%.0f in %d..%d)", wz0, wz1, PF_ZA, PF_ZB);
        CHECK(g_scd.props[p].rot_y == 3072 && g_scd.props[p].rot_x == 0 && g_scd.props[p].rot_z == 0 &&
              g_scd.props[p].attr == 0x0000,
              "L 1030: Etikett zum Raum (rot_y 3072: Modell -z in den Pfeiler), Licht NCCT wie jedes RE1.5-Item (Attribut 0)");
    }

    /* --- Schluessel: JEDER sichtbare Texel im Nutzer-Cut ROOM10F0 Cut 5 in der Marke --- */
    raum_t *rk[2] = { &rf0, &rf1 };
    for (int i = 0; i < 2; i++) {
        raum_t *r = rk[i];
        start(r, 1300, -2900, 1024, NULL, i);
        int q = slot_von(RE15_R36Q_OBJ);
        int ms = 0, ts = 0;
        const uint8_t *m1 = re15_schluessel_r36_md1_bytes(r->id, &ms);
        const uint8_t *tb = re15_schluessel_r36_tim_bytes(r->id, &ts);
        static re15_md1_t km; re15_tim_t tim;
        re15_camera_view_t kv;
        int ok = q >= 0 && m1 && tb && re15_md1_parse(m1, ms, &km) == 0 && km.mesh_count == 1 &&
                 re15_tim_parse(tb, ts, &tim) == 0 && tim.bpp == 8 && view_von(r, 5, &kv) == 0;
        CHECK(ok, "L ROOM%04X: Prop obj 4, skaliertes RE2-MD1 (%d B), TIM 8bpp, Kamera Cut 5", (unsigned)r->id, ms);
        if (!ok) continue;
        CHECK(g_scd.props[q].attr == RE15_R38K_SCHL_ATTR && RE15_R38K_SCHL_ATTR == 0x0010,
              "L ROOM%04X: Attribut 0x0010 = flach wie RE2 (Handler @0x80055260: @0x8005530c lh 12(s2) -> @0x80055320 "
              "sw 16(s1); Zeichner FUN_800363f0 @0x800365e8 andi 0x10)", (unsigned)r->id);
        double m[9]; int32_t pos[3] = { g_scd.props[q].x, g_scd.props[q].y, g_scd.props[q].z };
        prop_matrix(g_scd.props[q].rot_x, g_scd.props[q].rot_y, g_scd.props[q].rot_z, m);
        const re15_md1_mesh_t *h = &km.meshes[0];
        const uint8_t *px = (const uint8_t *)tim.pixels;
        long sicht = 0, ausser = 0;
        double x0 = 1e9, x1 = -1e9, y0 = 1e9, y1 = -1e9;
        /* Flaechen abtasten: Dreiecke direkt, Vierecke wie der Port (0,1,3)+(0,3,2) */
        int nf = h->triangle_count + 2 * h->quad_count;
        for (int f = 0; f < nf; f++) {
            const re15_md1_vertex_t *P[3]; int U[3], V[3]; uint16_t clut;
            if (f < h->triangle_count) {
                const re15_md1_triangle_t *t = &h->triangles[f]; const re15_md1_tri_uv_t *uv = &h->triangle_uvs[f];
                P[0] = &h->tri_vertices[t->v0]; P[1] = &h->tri_vertices[t->v1]; P[2] = &h->tri_vertices[t->v2];
                U[0] = uv->u0; V[0] = uv->v0; U[1] = uv->u1; V[1] = uv->v1; U[2] = uv->u2; V[2] = uv->v2; clut = uv->clut;
            } else {
                int qi = (f - h->triangle_count) / 2, half = (f - h->triangle_count) % 2;
                const re15_md1_quad_t *qd = &h->quads[qi]; const re15_md1_quad_uv_t *uv = &h->quad_uvs[qi];
                const re15_md1_vertex_t *Q[4] = { &h->quad_vertices[qd->v0], &h->quad_vertices[qd->v1],
                                                   &h->quad_vertices[qd->v2], &h->quad_vertices[qd->v3] };
                int QU[4] = { uv->u0, uv->u1, uv->u2, uv->u3 }, QV[4] = { uv->v0, uv->v1, uv->v2, uv->v3 };
                static const int idx[2][3] = { { 0, 1, 3 }, { 0, 3, 2 } };
                for (int c = 0; c < 3; c++) { P[c] = Q[idx[half][c]]; U[c] = QU[idx[half][c]]; V[c] = QV[idx[half][c]]; }
                clut = uv->clut;
            }
            int zeile = (int)(clut >> 6) - tim.clut_y, spalte = (int)(clut & 0x3F) * 16 - tim.clut_x;
            const int N = 48;
            for (int a = 0; a <= N; a++) for (int b = 0; a + b <= N; b++) {
                double l1 = (double)a / N, l2 = (double)b / N, l0 = 1.0 - l1 - l2;
                int tu = (int)(l0 * U[0] + l1 * U[1] + l2 * U[2]), tv = (int)(l0 * V[0] + l1 * V[1] + l2 * V[2]);
                if (tu < 0 || tv < 0 || tu >= tim.width || tv >= tim.height) continue;
                int ci = px[tv * tim.width + tu];
                uint16_t farbe = tim.clut[zeile * 256 + spalte + ci];
                if (farbe == 0x0000) continue;                     /* PSX: 0x0000 = durchsichtig */
                double w[3], sx, sy;
                welt(m, pos, l0 * P[0]->x + l1 * P[1]->x + l2 * P[2]->x, l0 * P[0]->y + l1 * P[1]->y + l2 * P[2]->y,
                     l0 * P[0]->z + l1 * P[1]->z + l2 * P[2]->z, w);
                projiziere(&kv, w[0], w[1], w[2], &sx, &sy);
                sicht++;
                if (sx < KS_MX0 - 0.5 || sx > KS_MX1 + 1.5 || sy < KS_MY0 - 0.5 || sy > KS_MY1 + 1.5) ausser++;
                if (sx < x0) x0 = sx;
                if (sx > x1) x1 = sx;
                if (sy < y0) y0 = sy;
                if (sy > y1) y1 = sy;
            }
        }
        printf("      ROOM%04X Cut 5: %ld sichtbare Abtastpunkte, Huelle x %.2f..%.2f y %.2f..%.2f, %ld ausserhalb\n",
               (unsigned)r->id, sicht, x0, x1, y0, y1, ausser);
        CHECK(sicht > 0 && ausser == 0,
              "L ROOM%04X: alle sichtbaren Texel in der Marke x %d..%d y %d..%d (+-0,5/+1,5 px, Massstab-Regel Runde 36 N)",
              (unsigned)r->id, KS_MX0, KS_MX1, KS_MY0, KS_MY1);
        /* Runde 39 Spur L (NUTZER-VORGABE "kleiner/subtiler, nicht so riesig"; Dossier analysis/befunde_runde39/L_optik.md
         * Punkt 2): die gezeichnete Huelle ist so breit, wie RE2 Retail dasselbe Modell an seinen eigenen Platzierungen
         * in den naeheren Kameras zeigt (oberes Quartil von L_nat*H/vz, gen/itemorte_r38_lage.inc RE15_R39L_*_ZIEL_PX100;
         * Riegel unit_r39_optik_schluessel rechnet es aus den RE2-Dateien nach). Bis Runde 38: "fuellt die Marke"
         * (>= 9 von 11 Spalten). */
        {
            double ziel = (i == 0 ? RE15_R39L_PARK_ZIEL_PX100 : RE15_R39L_PRISON_ZIEL_PX100) / 100.0;
            CHECK(fabs((x1 - x0) - ziel) <= 0.5 && x1 - x0 < 9.0,
                  "L ROOM%04X: Runde 39 L \"kleiner/subtiler\" = gezeichnete Huelle %.2f px breit == RE2 oberes Quartil %.2f "
                  "(+-0,5), nicht mehr die ganze Marke (< 9 von 11 Spalten)", (unsigned)r->id, x1 - x0, ziel);
        }
        CHECK(re15_itemorte_r38_schluessel_massstab(i == 0 ? 0x45 : 0x42) > 10000,
              "L ROOM%04X: Massstab %d/10000 > 1 (RE2-Groesse waere 2..3 Bildpunkte)", (unsigned)r->id,
              re15_itemorte_r38_schluessel_massstab(i == 0 ? 0x45 : 0x42));
    }
    free(r30.buf); free(rf0.buf); free(rf1.buf);
}

/* ============================================================================================================ raum */
static void teil_raum(void)
{
    raum_t r30, r31, r50, rf0, rf1;
    printf("\n[raum]\n");
    if (raum_laden(&r30, 0x1030) || raum_laden(&r31, 0x1031) || raum_laden(&r50, 0x1050) ||
        raum_laden(&rf0, 0x10F0) || raum_laden(&rf1, 0x10F1)) return;
    static const int k_taken86[] = { 9, 0x86, 3, 0x85, -1 };

    start(&r30, -11500, -19500, 2048, NULL, 0);
    int p = slot_von(RE15_FL37_OBJ);
    CHECK(p >= 0 && g_scd.props[p].x == RE15_R38K_FL_X && g_scd.props[p].y == RE15_R38K_FL_Y &&
          g_scd.props[p].z == RE15_R38K_FL_Z && g_scd.props[p].band == 1 && g_scd.props[p].flags == 0x000B,
          "R 1030: Loescher-Prop obj 5 an der gemessenen Lage (%d,%d,%d)", RE15_R38K_FL_X, RE15_R38K_FL_Y, RE15_R38K_FL_Z);
    CHECK(g_aot.slots[14].active && g_aot.slots[14].type == RE15_AOT_TYPE_ITEM &&
          g_aot.item_params[14].item_type == 0x31 && g_aot.item_params[14].taken_bit == 0x86 &&
          g_aot.item_params[14].taken_prop == RE15_FL37_OBJ && g_aot.slots[14].sce_flags == 0x31,
          "R 1030: Aufhebe-Zone Slot 14 (Item 0x31, Bit 0x86, tk_prop 5, sat 0x31)");
    CHECK(g_aot.slots[20].active && g_aot.slots[20].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[20].event_id == 14 &&
          g_aot.slots[20].sce_flags == 0x31 && g_aot.slots[20].pause_mask16 == 0xFFFF &&
          g_aot.slots[20].x - g_aot.slots[20].half_w == -4190 && g_aot.slots[20].z - g_aot.slots[20].half_h == -18984,
          "R 1030: Monitor-Satz msg 14 (main00 @0x01DA6 x -4190 +1331, z -18984 +1786) unveraendert in Slot 20");
    int n0 = (int)g_scd.prop_count;
    re15_feuerloescher_r37_install(0x1030);
    CHECK((int)g_scd.prop_count == n0 && g_aot.slots[14].type == RE15_AOT_TYPE_ITEM && g_aot.slots[20].event_id == 14 &&
          re15_itemorte_r38_platz_frei(0x1030, 14) == 0,
          "R 1030: zweiter Installer-Lauf (Boot-Weg nach Tuer-Weg) aendert nichts");
    start(&r30, -11500, -19500, 2048, k_taken86, 0);
    CHECK(slot_von(RE15_FL37_OBJ) < 0 && g_aot.slots[14].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[14].event_id == 14 &&
          !g_aot.slots[20].active,
          "R 1030 genommen (9,0x86): kein Prop, keine Zone, Monitor-Satz bleibt in Slot 14");
    start(&r31, -11500, -19500, 2048, NULL, 1);
    CHECK(slot_von(RE15_FL37_OBJ) < 0 && g_aot.slots[14].event_id == 14 && g_aot.slots[14].type == RE15_AOT_TYPE_MESSAGE,
          "R 1031 (Elza): kein Loescher (Runde 37: Leon war zuerst da), Slot 14 Original");
    start(&r50, 18300, -21500, 0, NULL, 0);
    CHECK(slot_von(RE15_FL37_OBJ) < 0 && re15_feuerloescher_r37_obj_id(0x1050) < 0,
          "R 1050: kein Loescher mehr (\"anstatt in room 1050\")");

    raum_t *rk[2] = { &rf0, &rf1 };
    static const uint8_t item[2] = { 0x45, 0x42 }, bit[2] = { 82, 83 };
    for (int i = 0; i < 2; i++) {
        start(rk[i], 1300, -2900, 1024, NULL, i);
        int q = slot_von(RE15_R36Q_OBJ);
        int32_t lp[3]; int16_t lr[3];
        re15_itemorte_r38_schluessel_lage(item[i], lp, lr);
        CHECK(q >= 0 && g_scd.props[q].x == lp[0] && g_scd.props[q].y == lp[1] && g_scd.props[q].z == lp[2] &&
              g_scd.props[q].rot_x == lr[0] && g_scd.props[q].rot_y == lr[1] && g_scd.props[q].rot_z == lr[2],
              "R ROOM%04X: Schluessel-Prop obj 4 an der Pinnwand (%ld,%ld,%ld) rot (%d,%d,%d)", (unsigned)rk[i]->id,
              (long)lp[0], (long)lp[1], (long)lp[2], lr[0], lr[1], lr[2]);
        CHECK(g_aot.slots[8].active && g_aot.slots[8].type == RE15_AOT_TYPE_ITEM && g_aot.item_params[8].item_type == item[i] &&
              g_aot.item_params[8].taken_bit == bit[i] && g_aot.slots[15].active &&
              g_aot.slots[15].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[15].event_id == 4 &&
              g_aot.slots[15].x - g_aot.slots[15].half_w == 700 && g_aot.slots[15].z - g_aot.slots[15].half_h == -3800,
              "R ROOM%04X: Zone Slot 8 (Item 0x%02X, Bit (9,%d)), Satz msg 4 (main00 @0x00FEA) in Slot 15",
              (unsigned)rk[i]->id, item[i], bit[i]);
        int nq = (int)g_scd.prop_count;
        re15_schluessel_r36_install(rk[i]->id);
        CHECK((int)g_scd.prop_count == nq && g_aot.slots[8].type == RE15_AOT_TYPE_ITEM && g_aot.slots[15].event_id == 4,
              "R ROOM%04X: zweiter Installer-Lauf aendert nichts", (unsigned)rk[i]->id);
        const int tk[3] = { 9, bit[i], -1 };
        start(rk[i], 1300, -2900, 1024, tk, i);
        CHECK(slot_von(RE15_R36Q_OBJ) < 0 && g_aot.slots[8].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[8].event_id == 4,
              "R ROOM%04X genommen: kein Prop, Slot 8 bleibt der Satz msg 4", (unsigned)rk[i]->id);
    }
    free(r30.buf); free(r31.buf); free(r50.buf); free(rf0.buf); free(rf1.buf);
}

/* ======================================================================================================== aufnahme */
static void teil_aufnahme(void)
{
    raum_t r30, rf0, rf1;
    printf("\n[aufnahme]\n");
    if (raum_laden(&r30, 0x1030) || raum_laden(&rf0, 0x10F0) || raum_laden(&rf1, 0x10F1)) return;

    /* VORHER, ausserhalb des Zonen-Rechtecks: (-4300,-17250) Blick +x -> 620 voraus (-3680,-17250) liegt im Monitor-
     * Rechteck (z bis -17198), nicht in der Zone (z bis -17370) und nicht in Slot 13 (z bis -18372). */
    start(&r30, -4300, -17250, 0, NULL, 0);
    ruhe(12);
    druck();
    CHECK(!re15_item_modal_active() && g_scd.message_active && g_scd.message_id == 14,
          "A 1030 vorher, ausserhalb der Zone: der Monitor-Satz antwortet aus Slot 20 (msg 14, ist %d/%d)",
          (int)g_scd.message_active, (int)g_scd.message_id);
    /* Aufnahme vor dem Pfeiler: (-4300,-17904) Blick +x -> (-3680,-17904) im Rechteck x -3980..-2980 */
    start(&r30, -4300, -17904, 0, NULL, 0);
    ruhe(12);
    int p = slot_von(RE15_FL37_OBJ);
    druck();
    uint8_t typ = modal_fahren(1);
    CHECK(typ == RE15_FL37_ITEM, "A 1030 Druck (-4300,-17904) Blick +x: Aufnahme-Modal Item 0x31 (ist 0x%02X)", typ);
    CHECK(re15_game_flag_get(9, 0x86) && re15_game_flag_get(3, 0x85) && re15_inv_find_item(RE15_FL37_ITEM) >= 0 &&
          p >= 0 && !g_scd.props[p].active,
          "A 1030 Ja: (9,0x86), (3,0x85) (ROOM1000 sub01 @0x00D0C), Loescher im Inventar, Modell weg (@0x80021fc8)");
    ruhe(6);
    stelle(-4300, -17904, 0);
    druck();
    CHECK(!re15_item_modal_active() && g_scd.message_active && g_scd.message_id == 14,
          "A 1030 danach an derselben Stelle: genommene Zone uebersprungen (@0x80042f50), Monitor-Satz antwortet (msg 14)");

    /* Pinnwand 10F0/10F1 */
    raum_t *rk[2] = { &rf0, &rf1 };
    static const uint8_t item[2] = { 0x45, 0x42 }, bit[2] = { 82, 83 };
    for (int i = 0; i < 2; i++) {
        /* vorher, ausserhalb: (1700,-2900) rot 683 (60 Grad) -> 620 voraus (2010,-3437): Satz msg 4 (x bis 2300), nicht
         * die Zone (x bis 1798), nicht Slot 12 (z ab -3250). */
        start(rk[i], 1700, -2900, 683, NULL, i);
        ruhe(12);
        druck();
        CHECK(!re15_item_modal_active() && g_scd.message_active && g_scd.message_id == 4,
              "A ROOM%04X vorher, ausserhalb der Zone: Satz msg 4 antwortet aus Slot 15 (ist %d/%d)", (unsigned)rk[i]->id,
              (int)g_scd.message_active, (int)g_scd.message_id);
        start(rk[i], 1300, -2900, 1024, NULL, i);
        ruhe(12);
        int q = slot_von(RE15_R36Q_OBJ);
        druck();
        typ = modal_fahren(1);
        CHECK(typ == item[i], "A ROOM%04X Druck (1300,-2900) Blick -z: Aufnahme-Modal Item 0x%02X (ist 0x%02X)",
              (unsigned)rk[i]->id, item[i], typ);
        CHECK(re15_game_flag_get(9, bit[i]) && re15_inv_find_item(item[i]) >= 0 && q >= 0 && !g_scd.props[q].active &&
              re15_game_flag_get(4, 0xEC) == (i == 1),
              "A ROOM%04X Ja: Bit (9,%d), Schluessel im Inventar, Modell weg; (4,0xEC) = %d (nur Prison Key, ROOM1011 @0x00E2C)",
              (unsigned)rk[i]->id, bit[i], i == 1);
        ruhe(6);
        stelle(1300, -2900, 1024);
        druck();
        CHECK(!re15_item_modal_active() && g_scd.message_active && g_scd.message_id == 4,
              "A ROOM%04X danach an derselben Stelle: Satz msg 4 aus Slot 15 antwortet", (unsigned)rk[i]->id);
    }
    free(r30.buf); free(rf0.buf); free(rf1.buf);
}

/* =========================================================================================================== sicht */
static void teil_sicht(void)
{
    raum_t r30, rf0;
    printf("\n[sicht]\n");
    if (raum_laden(&r30, 0x1030) || raum_laden(&rf0, 0x10F0)) return;

    CHECK(re15_itemorte_r38_verdeckt(0x1030, 3, RE15_FL37_OBJ) && re15_itemorte_r38_verdeckt(0x1030, 10, RE15_FL37_OBJ) &&
          !re15_itemorte_r38_verdeckt(0x1030, 0, RE15_FL37_OBJ) && !re15_itemorte_r38_verdeckt(0x1030, 1, RE15_FL37_OBJ) &&
          !re15_itemorte_r38_verdeckt(0x1030, 2, RE15_FL37_OBJ) && !re15_itemorte_r38_verdeckt(0x1030, 9, RE15_FL37_OBJ) &&
          !re15_itemorte_r38_verdeckt(0x1030, 3, 15) && !re15_itemorte_r38_verdeckt(0x1090, 3, RE15_FL37_OBJ),
          "S Regel 1030: Loescher nur in Cut 3/10 nicht gezeichnet (Cut 0/1/2/9 ja; Lageplan obj 15 und 1090 unberuehrt)");
    CHECK(re15_itemorte_r38_verdeckt(0x10F0, 7, RE15_R36Q_OBJ) && re15_itemorte_r38_verdeckt(0x10F1, 7, RE15_R36Q_OBJ) &&
          !re15_itemorte_r38_verdeckt(0x10F0, 5, RE15_R36Q_OBJ) && !re15_itemorte_r38_verdeckt(0x10F0, 6, RE15_R36Q_OBJ) &&
          !re15_itemorte_r38_verdeckt(0x10F0, 7, 3) && !re15_itemorte_r38_verdeckt(0x1150, 7, RE15_R36Q_OBJ),
          "S Regel 10F0/10F1: Schluessel nur in Cut 7 nicht gezeichnet (Cut 5/6 ja; Original-Prop obj 3 unberuehrt)");

    /* WARUM 1030 Cut 10: Raumstart mit (4,0x0F) -> sub00 @0x01FF0 Ck(4,0x0F)==1 -> Cut_replace 3 -> 10 @0x01FF7 */
    static const int k_rolltor[] = { 4, 0x0F, -1 };
    start(&r30, -11500, -19500, 2048, k_rolltor, 0);
    int p = slot_von(RE15_FL37_OBJ);
    int16_t xs[4], zs[4];
    int anker = re15_rdt_get_region_quad(&r30.rdt, 10, xs, zs);
    CHECK(p >= 0 && anker && re15_aot_point_in_quad(g_scd.props[p].x, g_scd.props[p].z, xs, zs),
          "S 1030 nach Cut_replace: Anker-Viereck von Cut 10 (= altes von Cut 3) enthaelt den Loescher -> FUN_80014368 zeichnet ihn");
    re15_camera_view_t v10, v3;
    re15_pri_cut_t pri;
    int n10 = masken_des_cuts(&r30, 10, &pri);
    double sx, sy;
    double vz = (view_von(&r30, 10, &v10) == 0) ? projiziere(&v10, RE15_R38K_FL_X, RE15_R38K_FL_Y - 396, RE15_R38K_FL_Z, &sx, &sy) : -1;
    int ueber = 0, deckt = 0;
    for (int k = 0; k < pri.mask_count; k++) {
        const re15_pri_mask_t *m = &pri.masks[k];
        int dx = (int16_t)m->dstX, dy = (int16_t)m->dstY;
        if (sx < dx || sx >= dx + m->width || sy < dy || sy >= dy + m->height) continue;
        ueber++;
        deckt += re15_pri_mask_occludes(m->depth, (long)vz);
    }
    printf("      Cut 10: Loescher-Mitte (%.1f ; %.1f) vz %.0f, %d Masken im Cut, %d ueber dem Pixel, %d verdeckend\n",
           sx, sy, vz, n10, ueber, deckt);
    CHECK(n10 > 0 && ueber >= 1 && deckt == 0,
          "S 1030 Cut 10: die Pfeiler-Maske liegt ueber dem Loescher, verdeckt ihn bei vz %.0f aber NICHT -> ohne Regel schiene er durch", vz);
    /* der Sehstrahl Kamera 3 -> Loescher-Mitte schneidet den Pfeiler (Draufsicht, Quader x -2688..-1790 z -18459..-17580) */
    int durch = 0;
    if (view_von(&r30, 3, &v3) == 0) {
        double cx = r30.rdt.cuts[3].pos_x, cz = r30.rdt.cuts[3].pos_z, tx = RE15_R38K_FL_X, tz = RE15_R38K_FL_Z;
        for (int k = 0; k <= 2000 && !durch; k++) {
            double t = k / 2000.0, x = cx + (tx - cx) * t, z = cz + (tz - cz) * t;
            if (x >= PF_X0 && x <= PF_X1 && z >= PF_ZA && z <= -17580) durch = 1;
        }
    }
    CHECK(durch, "S 1030: Sehstrahl Kamera 3/10 -> Loescher geht durch den Pfeiler (Flaeche -x liegt hinter ihm)");

    /* WARUM 10F0 Cut 7: Anker-Viereck enthaelt den Schluessel, der Cut hat keine Maske */
    start(&rf0, 1300, -2900, 1024, NULL, 0);
    int q = slot_von(RE15_R36Q_OBJ);
    anker = re15_rdt_get_region_quad(&rf0.rdt, 7, xs, zs);
    int n7 = masken_des_cuts(&rf0, 7, &pri);
    CHECK(q >= 0 && anker && re15_aot_point_in_quad(g_scd.props[q].x, g_scd.props[q].z, xs, zs) && n7 == 0,
          "S 10F0 Cut 7: Anker-Viereck enthaelt den Schluessel, 0 Masken (weder Original noch MSK) -> ohne Regel ueber dem Regal");
    anker = re15_rdt_get_region_quad(&rf0.rdt, 5, xs, zs);
    CHECK(anker && re15_aot_point_in_quad(g_scd.props[q].x, g_scd.props[q].z, xs, zs),
          "S 10F0 Cut 5 (Nutzer-Cut): Anker-Viereck enthaelt den Schluessel -> gezeichnet");
    free(r30.buf); free(rf0.buf);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "lage"))     teil_lage();
    if (alle || !strcmp(teil, "raum"))     teil_raum();
    if (alle || !strcmp(teil, "aufnahme")) teil_aufnahme();
    if (alle || !strcmp(teil, "sicht"))    teil_sicht();
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
