/* test_r37_feuerloescher.c — Runde 37 Spur I: Feuerloescher-Modell, leerer Loescher bei Elza (1091) und bei
 * Leon nach dem Loeschen (1090), Leons Loescher aus dem Spind ROOM1000 an die Wand ROOM1050.
 * Dossier: analysis/befunde_runde37/I_feuerloescher.md, Konstanten include/re15_feuerloescher_r37.h.
 *
 * Jeder Teil misst die MECHANIK am echten Port-Code (echte RDT, scd_room_reenter mit allen Port-Installern,
 * echte AOT-Scans, Item-Modal, SCD-VM, Wegwerf-Abfrage):
 *   modell     MD1 stehend/liegend + TIM + Text: Kopf 65/0/2, Masse (Leon 3022 = 1,78 m), Normalen Q12 nach
 *              aussen, Vierecke in Z-Ordnung (Port teilt (0,1,3)+(0,3,2)), CLUT-Index 0 frei, Text 04 02..01 00
 *   raum       Raumstart: 1050 Prop + Aufnahme-Zone (Item 0x31 x1, Bit 0x86, tk_prop 5) / mit Bit nichts / 1051
 *              (Elza) nichts; 1090 nur mit (3,0x81) UND ohne Loescher im Inventar; 1091 immer; 1000 Spind ->
 *              msg 0; 1001 (Elza) unveraendert 30 Patronen; Installer idempotent
 *   aufnahme   echter Druck in 1050 vor der Wand -> Modal Item 0x31 -> Ja: (9,0x86), (3,0x85) (ROOM1000 sub01
 *              @0x00D0C), Loescher im Inventar, Modell weg, Zone weg; ROOM1090 legt danach Slot 2 sce 3 (sub06)
 *   loeschen   ROOM1090 mit Loescher: Druck am Feuer -> sub06 msg 7/8 -> Ja -> (3,0x81)/(3,0x84) + Selbst-Tuer;
 *              Neuladen -> sub03 msg 9 -> Wegwerf-Abfrage: Ja -> Loescher liegt (2500,-1800,-1300) + Platz
 *              msg 29; Nein -> Leon behaelt ihn, keiner liegt; Wiederbetreten (3,0x84)=0 -> wie vorher
 *   elza       ROOM1091: Druck am Loescher -> msg 29 "It's an empty fire extinguisher."; der Messpunkt der
 *              Runde 36 (0,250) Blick +X -> weiter msg 20 (Schuppen)
 *   spind      ROOM1000 Leon: Druck am Spind -> msg 0 "Nothing unusual.", kein Aufnahme-Modal
 *   spielstand (3,0x81)/(9,0x86)/(3,0x85) + Inventar reisen mit re15_savedata (Spielstand) -> gleicher Zustand
 *   kiste      Nachbesserung 1 (Abnahme 0, M1): echter NEIN-Weg, dann Einlagern ueber den echten Tausch
 *              re15_itembox_transfer (RE2 FUN_800703b8 @0x800706c8/@0x80070710) -> Wiederbetreten, Spielstand,
 *              Herausnehmen: es liegt KEINER (die Kiste lagert nur um); Gegenprobe Kiste mit fremdem Gegenstand
 *              bzw. leer + Loescher weg -> liegt
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_feuerloescher_r37.h"
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
#include "re15_game_step.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_savedata.h"
#include "re15_item_prompt.h"
#include "re15_inv_screen.h"
#include "re15_itembox.h"

extern re15_aot_state_t g_aot;
extern uint8_t          g_aot_action_pressed;
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

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern). neu=1: frisches Spiel
 * (Flags/Inventar/Abfrage leer, dann `flags` (Paare bank,bit; Ende <0) und `items` (Paare id,menge; Ende 0));
 * neu=0: Flags und Inventar bleiben (Neuladen / Wiederbetreten). */
static void start(raum_t *r, int32_t px, int32_t py, int32_t pz, int16_t rot, const int *flags,
                  const uint8_t *items, int elza, int neu)
{
    if (neu) {
        re15_game_state_init();
        re15_inv_init();
        re15_itembox_init();              /* Nachbesserung 1: auch die ITEM BOX (neues Spiel, FUN_8003e4f4) */
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
    g_current_room_id = r->id;
    g_room_rdt = r->rdt; g_room_rdt_ok = 1;
    memset(&g_room_change, 0, sizeof g_room_change);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(re15_collision_band_from_y(py));
    g_scd.player_mode = 0;
    re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_register_room_events(&r->rdt);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

static int slot_von(uint8_t oid)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == oid) return k;
    return -1;
}

static int menge(uint8_t id)
{
    int m = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) m += g_inv.slots[i].qty;
    return m;
}

/* ---- Bild-Ablauf wie platform/pc/main.c (Muster test_r36_schluessel.c bf_frame / test_item_stack_press.c) */
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_cine_was = 0;

static uint16_t virt(uint16_t raw) { return (raw & RE15_PAD_BIT_SQUARE) ? 0x4000u : 0u; }

static void ctx_neu(raum_t *r)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &r->rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_shown = 0; s_cine_was = 0;
    g_scd.cut_auto_enabled = 1;
}

/* Ein Bild. `antwort` = virtuelles Wort fuer die Wegwerf-Abfrage (0x4000 Ja, 0x8000 Nein, 0 nichts). */
static void bild(uint16_t held, uint16_t edge, uint16_t antwort)
{
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
        s_ctx.active_cut  = s_shown;
        s_ctx.pad_current = held;
        s_ctx.pad_pressed = edge;
        re15_game_step(&s_ctx);
        if (re15_item_modal_active()) re15_item_modal_tick(virt(edge), virt(held));
        { scd_audio_event_t e; while (scd_audio_queue_pop(&e)) { } }
    }
    if (re15_discard_active()) re15_discard_tick(antwort, 0);
}

/* EIN Aktionsdruck ohne Bild-Ablauf (Muster test_r36_schluessel.c druck): Flanke -> Scan -> Ereignis. */
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

/* ======================================================================================== modell */
static void md1_pruefen(const char *art, int a)
{
    int sz = 0;
    const uint8_t *b = re15_feuerloescher_r37_md1(a, &sz);
    static re15_md1_t m;
    CHECK(b && sz > 12 && re15_md1_parse(b, (size_t)sz, &m) == 0 && m.mesh_count == 1,
          "M %s: MD1 %d B parst, 1 Mesh", art, sz);
    if (!b || m.mesh_count != 1) return;
    uint32_t len = (uint32_t)b[0] | ((uint32_t)b[1] << 8), obj = (uint32_t)b[8] | ((uint32_t)b[9] << 8);
    CHECK(len == 65 && obj == 2, "M %s: Kopf 65/0/2 wie gen/sicherung_prop.inc (ist %u/%u)", art, len, obj);
    const re15_md1_mesh_t *h = &m.meshes[0];
    int lo[3] = { 1 << 30, 1 << 30, 1 << 30 }, hi[3] = { -(1 << 30), -(1 << 30), -(1 << 30) };
    for (int k = 0; k < h->quad_vertex_count; k++) {
        int v[3] = { h->quad_vertices[k].x, h->quad_vertices[k].y, h->quad_vertices[k].z };
        for (int i = 0; i < 3; i++) { if (v[i] < lo[i]) lo[i] = v[i]; if (v[i] > hi[i]) hi[i] = v[i]; }
    }
    printf("      %s bbox x %d..%d y %d..%d z %d..%d, %d Dreiecke %d Vierecke\n", art, lo[0], hi[0], lo[1], hi[1],
           lo[2], hi[2], h->triangle_count, h->quad_count);
    if (a == RE15_FL37_STEHEND) {
        double meter = (double)(-lo[1]) * 1.78 / 3022.0;   /* Leon PL00.PLD Nullpose 3022 = 1,78 m */
        CHECK(hi[1] == 0 && meter > 0.40 && meter < 0.60,
              "M stehend: Boden y=0, Hoehe %d = %.2f m (10-lb-Klasse 0,40..0,60 m)", -lo[1], meter);
        CHECK(lo[2] == -110 && hi[2] == 110, "M stehend: Durchmesser 220 = 13 cm (z %d..%d)", lo[2], hi[2]);
    } else {
        CHECK(hi[1] == 0 && lo[1] == -220, "M liegend: liegt auf y=0, oberste Kante -220 (y %d..%d)", lo[1], hi[1]);
    }
    /* Normalen Q12 und nach aussen (Mantel: radial) */
    int laenge_ok = 1, aussen = 0, mantel = 0;
    for (int k = 0; k < h->quad_normal_count; k++) {
        double nx = h->quad_normals[k].x, ny = h->quad_normals[k].y, nz = h->quad_normals[k].z;
        double l = sqrt(nx * nx + ny * ny + nz * nz);
        if (fabs(l - 4096.0) > 8.0) laenge_ok = 0;
    }
    if (a == RE15_FL37_STEHEND) {
        for (int qi = 0; qi < h->quad_count; qi++) {
            const re15_md1_quad_t *q = &h->quads[qi];
            const uint16_t vv[4] = { q->v0, q->v1, q->v2, q->v3 }, nn[4] = { q->n0, q->n1, q->n2, q->n3 };
            for (int c = 0; c < 4; c++) {
                const re15_md1_vertex_t *p = &h->quad_vertices[vv[c]];
                double r = sqrt((double)p->x * p->x + (double)p->z * p->z);
                if (fabs(r - 110.0) > 1.5 || p->y > -40 || p->y < -560) continue;   /* nur Mantel-Ecken */
                mantel++;
                const re15_md1_vertex_t *n = &h->quad_normals[nn[c]];
                if ((double)n->x * p->x + (double)n->z * p->z > 0) aussen++;
            }
        }
        CHECK(mantel > 0 && aussen == mantel, "M stehend: %d von %d Mantel-Ecken mit Normale nach AUSSEN", aussen, mantel);
    }
    CHECK(laenge_ok, "M %s: alle %d Normalen auf 4096 (Q12)", art, h->quad_normal_count);
    /* Z-Ordnung: die Teildreiecke (0,1,3) und (0,3,2) des Ports zeigen in dieselbe Richtung */
    int z_ok = 0;
    for (int qi = 0; qi < h->quad_count; qi++) {
        const re15_md1_quad_t *q = &h->quads[qi];
        const re15_md1_vertex_t *P[4] = { &h->quad_vertices[q->v0], &h->quad_vertices[q->v1],
                                          &h->quad_vertices[q->v2], &h->quad_vertices[q->v3] };
        double a1[3] = { P[1]->x - P[0]->x, P[1]->y - P[0]->y, P[1]->z - P[0]->z };
        double a3[3] = { P[3]->x - P[0]->x, P[3]->y - P[0]->y, P[3]->z - P[0]->z };
        double a2[3] = { P[2]->x - P[0]->x, P[2]->y - P[0]->y, P[2]->z - P[0]->z };
        double n1[3] = { a1[1] * a3[2] - a1[2] * a3[1], a1[2] * a3[0] - a1[0] * a3[2], a1[0] * a3[1] - a1[1] * a3[0] };
        double n2[3] = { a3[1] * a2[2] - a3[2] * a2[1], a3[2] * a2[0] - a3[0] * a2[2], a3[0] * a2[1] - a3[1] * a2[0] };
        if (n1[0] * n2[0] + n1[1] * n2[1] + n1[2] * n2[2] > 0) z_ok++;
    }
    CHECK(z_ok == h->quad_count, "M %s: %d von %d Vierecken in Z-Ordnung (FUN_800256b0 / Port (0,1,3)+(0,3,2))",
          art, z_ok, h->quad_count);
}

static void teil_modell(void)
{
    printf("\n[modell]\n");
    md1_pruefen("stehend", RE15_FL37_STEHEND);
    md1_pruefen("liegend", RE15_FL37_LIEGEND);
    int tsz = 0;
    const uint8_t *tb = re15_feuerloescher_r37_tim_bytes(0x1050, &tsz);
    re15_tim_t t;
    CHECK(tb && re15_tim_parse(tb, tsz, &t) == 0 && t.bpp == 8 && t.width == 128 && t.height == 128 &&
          t.has_clut && t.clut_entries == 256 && t.clut_x == 0 && t.clut_y == 480,
          "M TIM 8bpp 128x128, CLUT 256 @VRAM(0,480) (wie gen/sicherung_prop.inc)");
    if (tb && re15_tim_parse(tb, tsz, &t) == 0 && t.bpp == 8) {
        const uint8_t *px = (const uint8_t *)t.pixels;
        int null_texel = 0, schwarz_schluessel = 0;
        for (int i = 0; i < t.width * t.height; i++) if (px[i] == 0) null_texel++;
        for (int c = 1; c < t.clut_entries; c++) if (t.clut[c] == 0x0000) schwarz_schluessel++;
        CHECK(t.clut[0] == 0x0000 && null_texel == 0 && schwarz_schluessel == 0,
              "M CLUT[0]=0x0000 frei (PSX-Farbschluessel): %d Texel mit Index 0, %d weitere 0x0000-Eintraege",
              null_texel, schwarz_schluessel);
    }
    int tl = 0;
    const uint8_t *tx = re15_feuerloescher_r37_text(&tl);
    CHECK(tx && tl >= 4 && tl <= 128 && tx[0] == 0x04 && tx[1] == 0x02 && tx[tl - 2] == 0x01 && tx[tl - 1] == 0x00,
          "M Text msg 29: Form 04 02 .. 01 00 (ROOM1000 msg 0 @0x0D22), %d B", tl);
    re15_msg_install_text(RE15_FL37_MSG_LEER, tx, (size_t)tl);
    const char *s = re15_msg_get_text(RE15_FL37_MSG_LEER);
    CHECK(s && strcmp(s, "It's an empty fire extinguisher.") == 0, "M Text liest sich \"%s\"", s ? s : "(null)");
    CHECK(re15_feuerloescher_r37_obj_id(0x1050) == 5 && re15_feuerloescher_r37_obj_id(0x1090) == 5 &&
          re15_feuerloescher_r37_obj_id(0x1091) == 5 && re15_feuerloescher_r37_obj_id(0x1051) == -1 &&
          re15_feuerloescher_r37_obj_id(0x1000) == -1,
          "M Prop obj 5 nur in 1050/1090/1091 (TIM-Slot RE15_TIM_SLOT_PROP(5) = 9)");
    int s1 = 0, s2 = 0;
    CHECK(re15_feuerloescher_r37_md1_bytes(0x1050, &s1) == re15_feuerloescher_r37_md1(RE15_FL37_STEHEND, NULL) &&
          re15_feuerloescher_r37_md1_bytes(0x1091, &s2) == re15_feuerloescher_r37_md1(RE15_FL37_LIEGEND, NULL),
          "M 1050 = stehendes Netz (Wand), 1090/1091 = liegendes Netz");
}

/* ========================================================================================= raum */
static const int K_GENOMMEN[]   = { 9, 0x86, 3, 0x85, -1 };
static const int K_FEUER_AUS[]  = { 9, 0x86, 3, 0x85, 3, 0x81, -1 };
static const int K_NEULADEN[]   = { 9, 0x86, 3, 0x85, 3, 0x81, 3, 0x84, -1 };
static const uint8_t K_LOESCHER[] = { RE15_FL37_ITEM, 1, 0 };

static void teil_raum(void)
{
    raum_t r50, r51, r90, r91, r00, r01;
    printf("\n[raum]\n");
    if (raum_laden(&r50, 0x1050) || raum_laden(&r51, 0x1051) || raum_laden(&r90, 0x1090) ||
        raum_laden(&r91, 0x1091) || raum_laden(&r00, 0x1000) || raum_laden(&r01, 0x1001)) return;

    /* 1050 Leon, nicht genommen */
    start(&r50, 18300, 0, -21500, 0, NULL, NULL, 0, 1);
    int p = slot_von(RE15_FL37_OBJ);
    CHECK(p >= 0 && g_scd.props[p].active && g_scd.props[p].x == 17200 && g_scd.props[p].y == -1150 &&
          g_scd.props[p].z == -23775 && g_scd.props[p].rot_y == 0 && g_scd.props[p].obj_type == 0 &&
          g_scd.props[p].band == 1 && g_scd.props[p].flags == 0x000B && g_scd.props[p].attr == 0x0000 &&
          g_scd.props[p].parent_obj == -1,
          "R 1050: Prop obj 5 an der Wand (17200,-1150,-23775), Typ 0, Band 1, Flags 0x000B, Attribut 0 (NCCT wie ROOM1010 @0x00930)");
    const re15_aot_t *a = &g_aot.slots[13];
    CHECK(a->active && a->type == RE15_AOT_TYPE_ITEM && g_aot.item_params[13].item_type == 0x31 &&
          g_aot.item_params[13].amount == 1 && g_aot.item_params[13].taken_bit == 0x86 &&
          g_aot.item_params[13].taken_prop == 5 && a->sce_flags == 0x31 && a->band == 0 &&
          a->x - a->half_w == 16700 && a->x + a->half_w == 17700 && a->z - a->half_h == -23900 && a->z + a->half_h == -22900,
          "R 1050: Slot 13 Aufnahme wie ROOM1000 @0x00C24 (Item 0x31 x1, Bit 0x86) mit tk_prop 5, x[16700..17700] z[-23900..-22900]");
    CHECK(g_aot.slots[5].active && g_aot.slots[5].type == RE15_AOT_TYPE_DOOR &&
          g_aot.slots[5].x - g_aot.slots[5].half_w == 18100 &&
          g_aot.slots[6].active && g_aot.slots[6].type == RE15_AOT_TYPE_ITEM && g_aot.item_params[6].item_type == 0x15 &&
          g_aot.slots[6].x + g_aot.slots[6].half_w == 16650,
          "R 1050: Tuer Slot 5 (ab x 18100) und Patronen Slot 6 (bis x 16650) unveraendert, kein Ueberlapp");
    int pc0 = (int)g_scd.prop_count;
    re15_feuerloescher_r37_install(0x1050);
    CHECK((int)g_scd.prop_count == pc0, "R 1050: Installer idempotent (zweiter Lauf, Boot-Weg nach Tuer-Weg)");

    start(&r50, 18300, 0, -21500, 0, K_GENOMMEN, K_LOESCHER, 0, 1);
    CHECK(slot_von(RE15_FL37_OBJ) < 0 && !(g_aot.slots[13].active && g_aot.slots[13].type == RE15_AOT_TYPE_ITEM),
          "R 1050: Bit (9,0x86) gesetzt -> weder Prop noch Zone");
    start(&r51, 18300, 0, -21500, 0, NULL, NULL, 1, 1);
    CHECK(slot_von(RE15_FL37_OBJ) < 0 && !g_aot.slots[13].active, "R 1051 (Elza): kein Loescher");

    /* 1090 Leon */
    start(&r90, 0, -1800, 250, 0, NULL, NULL, 0, 1);
    CHECK(slot_von(RE15_FL37_OBJ) < 0 && g_aot.slots[2].active && g_aot.slots[2].type == RE15_AOT_TYPE_MESSAGE &&
          g_aot.slots[2].event_id == 7,
          "R 1090 ohne Loescher: kein Prop, Slot 2 = Text msg 7 (sub00 @0x0230E) wie Original");
    start(&r90, 0, -1800, 250, 0, K_GENOMMEN, K_LOESCHER, 0, 1);
    CHECK(slot_von(RE15_FL37_OBJ) < 0 && g_aot.slots[2].active && g_aot.slots[2].type != RE15_AOT_TYPE_MESSAGE &&
          g_aot.slots[3].type == RE15_AOT_TYPE_DOOR,
          "R 1090 mit Loescher (3,0x85): kein Prop, Slot 2 = Loesch-Ereignis (@0x02336), Slot 3 = Selbst-Tuer (@0x02352)");
    start(&r90, -10100, -1800, 4200, 3072, K_NEULADEN, K_LOESCHER, 0, 1);
    CHECK(slot_von(RE15_FL37_OBJ) < 0 && re15_feuerloescher_r37_liegt(0x1090) == 0,
          "R 1090 Neuladen (3,0x81)+(3,0x84), Loescher noch im Inventar: kein Prop (sonst doppelt)");
    start(&r90, -10100, -1800, 4200, 3072, K_FEUER_AUS, NULL, 0, 1);
    p = slot_von(RE15_FL37_OBJ);
    CHECK(p >= 0 && g_scd.props[p].active && g_scd.props[p].x == 2500 && g_scd.props[p].y == -1800 &&
          g_scd.props[p].z == -1300 && g_scd.props[p].rot_y == 3250,
          "R 1090 Feuer aus (3,0x81), Loescher abgelegt: Prop liegt (2500,-1800,-1300) rot_y 3250");
    a = &g_aot.slots[6];
    CHECK(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == 29 && a->band == 1 && a->sce_flags == 0x31 &&
          a->pause_mask16 == 0xFFFF && re15_msg_get_text(29) && strcmp(re15_msg_get_text(29), "It's an empty fire extinguisher.") == 0,
          "R 1090: Slot 6 Untersuchen msg 29, Band 1, sat 0x31, Pause 0xFFFF");
    CHECK(!(g_aot.slots[5].active && g_aot.slots[5].type == RE15_AOT_TYPE_MESSAGE),
          "R 1090: Slot 5 bleibt frei (Runde 36 unit_r36_story_schuppen: kein Text-Platz bei Leon)");
    CHECK(!g_aot.slots[2].active || g_aot.slots[2].type == RE15_AOT_TYPE_EXAMINE_WORKVAR || g_aot.slots[2].event_id != 7,
          "R 1090 Feuer aus: kein Feuer-Text mehr in Slot 2 (Original)");

    /* 1091 Elza: immer */
    start(&r91, -10100, -1800, 4200, 3072, NULL, NULL, 1, 1);
    p = slot_von(RE15_FL37_OBJ);
    CHECK(p >= 0 && g_scd.props[p].active && g_scd.props[p].x == 2500 && g_scd.props[p].z == -1300,
          "R 1091 (Elza): leerer Loescher liegt von Anfang an (ohne Flags)");
    CHECK(g_aot.slots[3].active && g_aot.slots[3].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[3].event_id == 29 &&
          g_aot.slots[5].active && g_aot.slots[5].event_id == 20,
          "R 1091: Slot 3 msg 29 (Loescher), Slot 5 bleibt msg 20 (Schuppen, Runde 36)");

    /* 1000 / 1001 */
    start(&r00, 0, 0, 0, 0, NULL, NULL, 0, 1);
    a = &g_aot.slots[3];
    CHECK(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == 0 && a->sce_flags == 0x31 && a->band == 0 &&
          a->x - a->half_w == 20500 && a->x + a->half_w == 21300 && a->z - a->half_h == -800 && a->z + a->half_h == 0,
          "R 1000 (Leon): Spind Slot 3 -> Raumtext msg 0, Rechteck der Original-Aufnahme (20500,-800) 800x800");
    start(&r00, 0, 0, 0, 0, K_GENOMMEN, NULL, 0, 1);
    CHECK(g_aot.slots[3].active && g_aot.slots[3].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[3].event_id == 0,
          "R 1000 (Leon), Loescher schon genommen: Spind ebenfalls msg 0");
    start(&r01, 0, 0, 0, 0, NULL, NULL, 1, 1);
    CHECK(g_aot.slots[3].active && g_aot.slots[3].type == RE15_AOT_TYPE_ITEM && g_aot.item_params[3].item_type == 0x15 &&
          g_aot.item_params[3].amount == 30 && g_aot.item_params[3].taken_bit == 0x86,
          "R 1001 (Elza): Spind unveraendert 30 Patronen (Item 0x15, @0x00C24)");
}

/* ===================================================================================== aufnahme */
static void teil_aufnahme(void)
{
    raum_t r50, r90;
    printf("\n[aufnahme]\n");
    if (raum_laden(&r50, 0x1050) || raum_laden(&r90, 0x1090)) return;
    /* vor der Wand, Blick Norden (-z) = rot 1024: 620 voraus (@0x80042bd0) = (17200,-23120) */
    start(&r50, 17200, 0, -22500, 1024, NULL, NULL, 0, 1);
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(17200, -22500, 0); }
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = 17200; g_actors[RE15_ACTOR_SLOT_PLAYER].z = -22500;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 1024;
    int p = slot_von(RE15_FL37_OBJ);
    druck();
    uint8_t typ = 0; int wahl = -1, w = 0;
    int modal = re15_item_modal_active();
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800) re15_item_modal_tick(0, 0);
    re15_item_modal_prompt(&typ, &wahl);
    CHECK(modal && typ == RE15_FL37_ITEM, "A Druck (17200,-22500) Blick Norden: Aufnahme-Modal Item 0x31 (ist 0x%02X)", typ);
    w = 0;
    while (re15_item_modal_active() && w++ < 1600) re15_item_modal_tick((uint16_t)0x4000u, 0);
    CHECK(re15_game_flag_get(9, 0x86) && menge(RE15_FL37_ITEM) == 1,
          "A Ja: Taken-Bit (9,0x86) (FUN_8001db28 @0x8001e0d0) und Loescher x1 im Inventar");
    CHECK(re15_game_flag_get(3, 0x85), "A Ja: (3,0x85)=1 wie ROOM1000 sub01 @0x00D0C (Haken item_modal_common.c)");
    CHECK(p >= 0 && !g_scd.props[p].active && !g_aot.slots[13].active,
          "A Ja: Modell weg (@0x80021fc8) und Zone tot (@0x8001e090)");
    /* Folge in ROOM1090: das Original liest (3,0x85) in sub00 @0x02306 */
    int fl[20], n = 0;
    for (int b = 0; b < 256; b++) {
        if (n < 18 && re15_game_flag_get(3, (uint8_t)b)) { fl[n++] = 3; fl[n++] = b; }
    }
    fl[n++] = 9; fl[n++] = 0x86; fl[n] = -1;
    start(&r90, 0, -1800, 250, 0, fl, K_LOESCHER, 0, 1);
    CHECK(g_aot.slots[2].active && g_aot.slots[2].type != RE15_AOT_TYPE_MESSAGE,
          "A danach ROOM1090: Slot 2 = Loesch-Ereignis sce 3 -> sub06 (@0x02336), nicht mehr der Text msg 7");
}

/* ===================================================================================== loeschen */
static int warte_msg(int id, int grenze)
{
    for (int f = 0; f < grenze; f++) {
        if (g_scd.message_active && (int)g_scd.message_id == id) return 1;
        bild(0, 0, 0);
    }
    return g_scd.message_active && (int)g_scd.message_id == id;
}

/* Loeschen am echten Bild-Ablauf; antwort 0x4000 = "Ja, wegwerfen", 0x8000 = "Nein" */
static void loeschen_fahren(raum_t *r90, uint16_t antwort, const char *was)
{
    start(r90, 0, -1800, 250, 0, K_GENOMMEN, K_LOESCHER, 0, 1);
    ctx_neu(r90);
    for (int f = 0; f < 30; f++) bild(0, 0, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 0; pl->z = 250; pl->rot_y = 0;     /* Messpunkt der Runde 36: 620 voraus = (620,250) im Viereck */
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE, 0);
    int m7 = warte_msg(7, 300);
    CHECK(m7, "L %s: Druck am Feuer -> sub06 msg 7 (@0x02702)", was);
    for (int f = 0; f < 600 && !(g_scd.message_active && g_scd.message_id == 8 && g_scd.message_fsm == 3); f++)
        bild(f % 25 == 0 ? RE15_PAD_BIT_SQUARE : 0, f % 25 == 0 ? RE15_PAD_BIT_SQUARE : 0, 0);
    CHECK(g_scd.message_active && g_scd.message_id == 8 && g_scd.message_fsm == 3,
          "L %s: Frage msg 8 \"Will you use the Fire Extinguisher?\" mit Ja/Nein", was);
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE, 0);          /* Ja (Ck(12,0x1F)==0 @0x02712) */
    /* Selbst-Tuer (Aot_on 3 @0x02726): Ziel == aktueller Raum -> g_scd_pending_scenario, der Spielschritt
     * steigt im selben Bild neu ein (game_step_common.c, scd_room_reenter mit allen Port-Installern; Original
     * FUN_8001d600 @0x8001d988 `jal 0x800396fc` laedt unbedingt). */
    re15_actor_t *sp = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int neu_drin = 0, kein_prop_beim_eintritt = 0;
    for (int f = 0; f < 600 && !neu_drin; f++) {
        bild(0, 0, 0);
        if (re15_game_flag_get(3, 0x81) && sp->x == 1252 && sp->z == -2529) {
            neu_drin = 1;
            kein_prop_beim_eintritt = (slot_von(RE15_FL37_OBJ) < 0);
        }
    }
    CHECK(re15_game_flag_get(3, 0x81) && neu_drin,
          "L %s: Ja -> Set(3,0x81) @0x0271E, Set(3,0x84) @0x02722, Aot_on 3 -> Selbst-Tuer, Neueintritt (1252,-1800,-2529)", was);
    CHECK(neu_drin && kein_prop_beim_eintritt && menge(RE15_FL37_ITEM) == 1,
          "L %s: Neueintritt: Leon traegt den Loescher noch -> keiner liegt (Tor: Item 0x31 im Inventar)", was);
    int m9 = warte_msg(9, 600);
    CHECK(m9, "L %s: Rettungsszene sub03 -> msg 9 \"You've used the Fire Extinguisher.\" (@0x02502)", was);
    int frage = 0;
    for (int f = 0; f < 900 && !frage; f++) {
        bild(f % 25 == 0 ? RE15_PAD_BIT_SQUARE : 0, f % 25 == 0 ? RE15_PAD_BIT_SQUARE : 0, 0);
        uint8_t it = 0;
        if (re15_discard_prompt(&it, NULL) && re15_discard_ready() && it == RE15_FL37_ITEM) frage = 1;
    }
    CHECK(frage, "L %s: Wegwerf-Abfrage fuer Item 0x31 (gen/discard_sites.inc ROOM1090/9)", was);
    for (int f = 0; f < 60 && re15_discard_active(); f++) bild(0, 0, f == 0 ? antwort : 0);
    for (int f = 0; f < 5; f++) bild(0, 0, 0);
}

static void teil_loeschen(void)
{
    raum_t r90;
    printf("\n[loeschen]\n");
    if (raum_laden(&r90, 0x1090)) return;

    loeschen_fahren(&r90, 0x4000u, "Ja");
    int p = slot_von(RE15_FL37_OBJ);
    CHECK(menge(RE15_FL37_ITEM) == 0 && p >= 0 && g_scd.props[p].active && g_scd.props[p].x == 2500 &&
          g_scd.props[p].z == -1300,
          "L Ja: Loescher aus dem Inventar UND liegt sofort am Boden (2500,-1800,-1300)");
    CHECK(g_aot.slots[6].active && g_aot.slots[6].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[6].event_id == 29,
          "L Ja: Untersuchen-Platz Slot 6 msg 29 angelegt");
    CHECK(re15_game_flag_get(3, 0x81) && !re15_game_flag_get(3, 0x84),
          "L Ja: (3,0x81) bleibt, (3,0x84) von sub03 @0x024CE geloescht (Einmal-Zeichen)");
    /* Wiederbetreten (Tuer ROOM1050 -> 1090, Spawn @0x00B7A (-10100,-1800,4200)) */
    start(&r90, -10100, -1800, 4200, 3072, NULL, NULL, 0, 0);
    p = slot_von(RE15_FL37_OBJ);
    CHECK(p >= 0 && g_scd.props[p].active && !re15_game_flag_get(3, 0x84),
          "L Ja: Wiederbetreten -> Loescher liegt weiter (Tor (3,0x81), nicht (3,0x84))");

    loeschen_fahren(&r90, 0x8000u, "Nein");
    CHECK(menge(RE15_FL37_ITEM) == 1 && slot_von(RE15_FL37_OBJ) < 0,
          "L Nein: Leon behaelt den Loescher, keiner liegt (kein Doppel)");
    CHECK(re15_feuerloescher_r37_name(RE15_FL37_ITEM) != NULL && re15_feuerloescher_r37_desc(RE15_FL37_ITEM) != NULL,
          "L Nein: der behaltene Loescher heisst \"Empty Fire Extinguisher\" (Nachtrag 4c)");
    start(&r90, -10100, -1800, 4200, 3072, NULL, NULL, 0, 0);
    CHECK(slot_von(RE15_FL37_OBJ) < 0, "L Nein: auch beim Wiederbetreten liegt keiner");
}

/* ========================================================================================= elza */
static void teil_elza(void)
{
    raum_t r91;
    printf("\n[elza]\n");
    if (raum_laden(&r91, 0x1091)) return;
    /* Westlich des Loeschers, Blick +X (rot 0): 620 voraus = (2440,-1300) liegt im Platz x[2000..3000] */
    start(&r91, 1820, -1800, -1300, 0, NULL, NULL, 1, 1);
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(1820, -1300, 0); }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 1820; pl->z = -1300; pl->rot_y = 0;
    g_scd.message_active = 0; re15_pauseflags_clear();
    druck();
    for (int f = 0; f < 20; f++) scd_vm_tick();
    CHECK(g_scd.message_active && g_scd.message_id == 29,
          "E 1091 Druck (1820,-1300) Blick +X: msg 29 \"It's an empty fire extinguisher.\" (ist %d)", (int)g_scd.message_id);
    /* Messpunkt der Runde 36 bleibt der Schuppen-Text */
    start(&r91, 0, -1800, 250, 0, NULL, NULL, 1, 1);
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(0, 250, 0); }
    pl->x = 0; pl->z = 250; pl->rot_y = 0;
    g_scd.message_active = 0; re15_pauseflags_clear();
    druck();
    for (int f = 0; f < 20; f++) scd_vm_tick();
    CHECK(g_scd.message_active && g_scd.message_id == 20,
          "E 1091 Druck (0,250) Blick +X (Runde 36): weiter msg 20 Schuppen (ist %d)", (int)g_scd.message_id);
}

/* ======================================================================================== spind */
static void teil_spind(void)
{
    raum_t r00;
    printf("\n[spind]\n");
    if (raum_laden(&r00, 0x1000)) return;
    /* vor dem Spind-Rechteck (20500..21300, -800..0), Blick +z (rot 3072): 620 voraus = (20900,-480) */
    start(&r00, 20900, 0, -1100, 3072, NULL, NULL, 0, 1);
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(20900, -1100, 0); }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 20900; pl->z = -1100; pl->rot_y = 3072;
    g_scd.message_active = 0; re15_pauseflags_clear();
    druck();
    for (int f = 0; f < 20; f++) scd_vm_tick();
    CHECK(!re15_item_modal_active() && g_scd.message_active && g_scd.message_id == 0,
          "S 1000 Druck am Spind: Raumtext msg 0 \"Nothing unusual.\", kein Aufnahme-Modal (msg %d, Modal %d)",
          (int)g_scd.message_id, re15_item_modal_active());
    CHECK(!re15_game_flag_get(9, 0x86) && menge(RE15_FL37_ITEM) == 0, "S 1000: kein Loescher aus dem Spind");
}

/* ========================================================================================== leer */
/* Nachtrag 4c: "If he doesn't he has the text empty fire extinguisher in the player inventory." */
typedef struct { uint8_t g[160]; int n; } glyphen_t;
static void glyph_cb(void *ctx, unsigned char code, int attr, int newline)
{
    glyphen_t *G = (glyphen_t *)ctx;
    (void)attr;
    if (!newline && G->n < (int)sizeof G->g) G->g[G->n++] = code;
}
static int enthaelt(const glyphen_t *G, const uint8_t *nadel, int len)
{
    for (int i = 0; i + len <= G->n; i++) if (memcmp(G->g + i, nadel, (size_t)len) == 0) return 1;
    return 0;
}

static void teil_leer(void)
{
    raum_t r90;
    printf("\n[leer]\n");
    /* Erwartung unabhaengig vom Modul aufgeschrieben: "Empty " = DEBUG.BIN @0x4BA7 (Name 0x1C "Empty Grenade
     * Shells"), "Fire Extinguisher" = DEBUG.BIN @0x4CDC (Name 0x31). */
    static const uint8_t k_empty[6]  = { 0x21, 0x49, 0x4c, 0x50, 0x55, 0x00 };
    static const uint8_t k_fire[17]  = { 0x22, 0x45, 0x4e, 0x41, 0x00, 0x21, 0x54, 0x50, 0x45, 0x4a, 0x43, 0x51,
                                         0x45, 0x4f, 0x44, 0x41, 0x4e };
    uint8_t k_voll[23];
    memcpy(k_voll, k_empty, 6); memcpy(k_voll + 6, k_fire, 17);

    re15_game_state_init(); re15_inv_init();
    g_gameflow.character = 0;
    const uint8_t *orig_desc = re15_inv_desc_entry(RE15_FL37_ITEM);
    glyphen_t vor; memset(&vor, 0, sizeof vor);
    re15_item_prompt_walk(1, RE15_FL37_ITEM, 999, glyph_cb, &vor);
    CHECK(re15_feuerloescher_r37_name(RE15_FL37_ITEM) == NULL && re15_feuerloescher_r37_desc(RE15_FL37_ITEM) == NULL &&
          enthaelt(&vor, k_fire, 17) && !enthaelt(&vor, k_voll, 23),
          "4c Leon vor dem Loeschen: Original-Name \"Fire Extinguisher\" (Abfrage), kein Ersatz");

    re15_game_flag_set(3, 0x81, 1);
    const uint8_t *nm = re15_feuerloescher_r37_name(RE15_FL37_ITEM);
    int nlen = 0;
    while (nm && nm[nlen] != 0x07 && nlen < 64) nlen++;
    CHECK(nm && nlen == 23 && memcmp(nm, k_voll, 23) == 0,
          "4c Leon nach dem Loeschen (3,0x81): Name = \"Empty \" (@0x4BA7) + \"Fire Extinguisher\" (@0x4CDC), 0x07-Ende");
    glyphen_t nach; memset(&nach, 0, sizeof nach);
    re15_item_prompt_walk(1, RE15_FL37_ITEM, 999, glyph_cb, &nach);
    CHECK(enthaelt(&nach, k_voll, 23), "4c Abfrage-Text traegt \"Empty Fire Extinguisher\" (Haken item_prompt_common.c)");
    const uint8_t *d = re15_inv_desc_entry(RE15_FL37_ITEM);
    int dl = 0;
    while (d && !(d[dl] == 0x01 && d[dl + 1] == 0x00) && dl < 120) dl++;
    {   /* als Nachricht dekodieren: Kopf 04 02 + Text + 01 00 */
        uint8_t m[160]; m[0] = 0x04; m[1] = 0x02;
        memcpy(m + 2, d, (size_t)dl + 2);
        re15_msg_install_text(28, m, (size_t)dl + 4);
    }
    const char *dt = re15_msg_get_text(28);
    CHECK(d && d != orig_desc && dt && strcmp(dt, "An empty fire extinguisher.") == 0,
          "4c CHECK-Text (re15_inv_desc_entry, Haken re15_inv_screen.c) = \"%s\"", dt ? dt : "(null)");
    CHECK(re15_feuerloescher_r37_name(0x30) == NULL && re15_feuerloescher_r37_desc(0x22) == NULL,
          "4c andere Gegenstaende unberuehrt");
    g_gameflow.character = 4;
    CHECK(re15_feuerloescher_r37_name(RE15_FL37_ITEM) == NULL && re15_inv_desc_entry(RE15_FL37_ITEM) == orig_desc,
          "4c Elza (eigene Bedeutung von (3,0x81), ROOM10B1): kein Ersatz");
    g_gameflow.character = 0;

    /* Spielstand: Nein an der Abfrage -> Loescher im Inventar; nach Speichern/Laden weiter "Empty ..." */
    if (raum_laden(&r90, 0x1090)) return;
    start(&r90, -10100, -1800, 4200, 3072, K_FEUER_AUS, K_LOESCHER, 0, 1);
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    re15_game_state_init(); re15_inv_init();
    CHECK(re15_feuerloescher_r37_name(RE15_FL37_ITEM) == NULL, "4c nach dem Zuruecksetzen: kein Ersatz");
    uint16_t rr = 0;
    int ok = re15_savedata_restore(&sd, &rr) == 0;
    CHECK(ok && menge(RE15_FL37_ITEM) == 1 && re15_feuerloescher_r37_name(RE15_FL37_ITEM) != NULL,
          "4c nach Laden: Loescher im Inventar heisst wieder \"Empty Fire Extinguisher\"");
}

/* =================================================================================== spielstand */
static void teil_spielstand(void)
{
    raum_t r90;
    printf("\n[spielstand]\n");
    if (raum_laden(&r90, 0x1090)) return;
    start(&r90, -10100, -1800, 4200, 3072, K_FEUER_AUS, NULL, 0, 1);
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    re15_game_state_init(); re15_inv_init();
    uint16_t rr = 0;
    CHECK(re15_savedata_restore(&sd, &rr) == 0 && rr == 0x1090 && re15_game_flag_get(3, 0x81) &&
          re15_game_flag_get(9, 0x86) && re15_game_flag_get(3, 0x85),
          "P Spielstand traegt (3,0x81)/(9,0x86)/(3,0x85) (g_game.flags)");
    start(&r90, -10100, -1800, 4200, 3072, NULL, NULL, 0, 0);
    CHECK(slot_von(RE15_FL37_OBJ) >= 0, "P geladen + Raumstart: Loescher liegt");
    start(&r90, -10100, -1800, 4200, 3072, K_FEUER_AUS, K_LOESCHER, 0, 1);
    re15_savedata_capture(&sd, 0, 1);
    re15_game_state_init(); re15_inv_init();
    CHECK(re15_savedata_restore(&sd, &rr) == 0 && menge(RE15_FL37_ITEM) == 1, "P Spielstand traegt den Loescher im Inventar");
    start(&r90, -10100, -1800, 4200, 3072, NULL, NULL, 0, 0);
    CHECK(slot_von(RE15_FL37_OBJ) < 0, "P geladen mit Loescher im Inventar: keiner liegt");
}

/* ======================================================================================== kiste */
/* Nachbesserung 1 (I_abnahme_0.md M1): NEIN an der Wegwerf-Abfrage, Loescher in die ITEM BOX, ROOM1090
 * betreten -> es lag einer am Boden (Tor fragte nur das Inventar, FUN_8004dfec). Die Kiste ist ein eigenes
 * Feld (g_itembox = RE2 0x800d4a68): RE2 FUN_800703b8 tauscht inv[Cursor] <-> box[Platz] (@0x800706c8 /
 * @0x80070710) — eingelagert ist der Loescher NICHT weggeworfen. */
static int in_kiste(uint8_t id)
{
    for (int i = 0; i < RE15_BOX_SLOTS; i++) if (g_itembox.slots[i].id == id) return i;
    return -1;
}

static int msg29_platz(void)
{
    return g_aot.slots[6].active && g_aot.slots[6].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[6].event_id == 29;
}

static void teil_kiste(void)
{
    raum_t r90;
    printf("\n[kiste]\n");
    if (raum_laden(&r90, 0x1090)) return;

    /* (1) echter NEIN-Weg am Bild-Ablauf (Feuer, msg 7/8, Ja, Neueintritt, msg 9, Wegwerf-Abfrage NEIN) */
    loeschen_fahren(&r90, 0x8000u, "Kiste/Nein");
    int s = re15_inv_find_item(RE15_FL37_ITEM);
    CHECK(s >= 0 && slot_von(RE15_FL37_OBJ) < 0 && re15_game_flag_get(3, 0x81),
          "K Nein: Leon behaelt den Loescher (Inventar-Platz %d), (3,0x81) gesetzt, keiner liegt", s);

    /* (2) Einlagern ueber den ECHTEN Tausch (Box-Platz 2 = (Scroll 0 + 2) & 0x3f, Auswahlformel RE2 FUN_800703b8) */
    int r = re15_itembox_transfer(s, 2);
    CHECK(r == RE15_BOX_XFER_OK && re15_inv_find_item(RE15_FL37_ITEM) < 0 && in_kiste(RE15_FL37_ITEM) == 2,
          "K Einlagern: Loescher verlaesst das Inventar und liegt in Box-Platz 2 (ist %d)", in_kiste(RE15_FL37_ITEM));
    CHECK(re15_feuerloescher_r37_liegt(0x1090) == 0,
          "K Tor zu, solange der Loescher in der Kiste liegt (nicht weggeworfen)");

    /* (3) Wiederbetreten ueber die Tuer ROOM1050 -> 1090 (Spawn @0x00B7A (-10100,-1800,4200)) — DER Befund */
    start(&r90, -10100, -1800, 4200, 3072, NULL, NULL, 0, 0);
    CHECK(slot_von(RE15_FL37_OBJ) < 0 && !msg29_platz() && in_kiste(RE15_FL37_ITEM) == 2,
          "K Wiederbetreten mit dem Loescher in der Kiste: kein Prop obj 5, kein Platz msg 29 (kein Doppel)");
    CHECK(re15_feuerloescher_r37_name(RE15_FL37_ITEM) != NULL,
          "K in der Kiste heisst er weiter \"Empty Fire Extinguisher\" (Nachtrag 4c)");

    /* (4) Spielstand: die Kiste reist mit (re15_savedata.c re15_itembox_export/_import) */
    re15_savedata_t sd;
    uint16_t rr = 0;
    re15_savedata_capture(&sd, 0, 1);
    re15_game_state_init(); re15_inv_init(); re15_itembox_init();
    CHECK(re15_savedata_restore(&sd, &rr) == 0 && in_kiste(RE15_FL37_ITEM) == 2 && re15_game_flag_get(3, 0x81) &&
          re15_inv_find_item(RE15_FL37_ITEM) < 0,
          "K Spielstand traegt den Loescher in der Kiste und (3,0x81)");
    start(&r90, -10100, -1800, 4200, 3072, NULL, NULL, 0, 0);
    CHECK(slot_von(RE15_FL37_OBJ) < 0 && !msg29_platz(), "K geladen + Raumstart: keiner liegt");

    /* (5) Herausnehmen (leerer Inventar-Platz 0 <-> Box-Platz 2): wieder im Inventar, weiter keiner am Boden */
    r = re15_itembox_transfer(0, 2);
    CHECK(r == RE15_BOX_XFER_OK && re15_inv_find_item(RE15_FL37_ITEM) >= 0 && in_kiste(RE15_FL37_ITEM) < 0,
          "K Herausnehmen: Loescher wieder im Inventar, Kiste leer");
    start(&r90, -10100, -1800, 4200, 3072, NULL, NULL, 0, 0);
    CHECK(slot_von(RE15_FL37_OBJ) < 0, "K nach dem Herausnehmen: keiner liegt (Leon traegt ihn)");

    /* (6) Gegenprobe: die Kiste sperrt NUR mit dem Loescher darin — fremder Gegenstand in der Kiste, Loescher
     *     weg (Wegwerf-Ja nimmt ihn aus dem Inventar, re15_inv_remove_slot) -> liegt */
    re15_inv_remove_slot(re15_inv_find_item(RE15_FL37_ITEM));
    g_itembox.slots[5].id = 0x15; g_itembox.slots[5].qty = 30; g_itembox.slots[5].flags = 0;
    start(&r90, -10100, -1800, 4200, 3072, NULL, NULL, 0, 0);
    int p = slot_von(RE15_FL37_OBJ);
    CHECK(p >= 0 && g_scd.props[p].active && g_scd.props[p].x == 2500 && g_scd.props[p].z == -1300 && msg29_platz() &&
          re15_feuerloescher_r37_liegt(0x1090) == 1,
          "K Gegenprobe: Loescher weg (weder Inventar noch Kiste), Kiste mit Patronen 0x15 -> liegt (2500,-1800,-1300)");
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "modell"))     teil_modell();
    if (alle || !strcmp(teil, "raum"))       teil_raum();
    if (alle || !strcmp(teil, "aufnahme"))   teil_aufnahme();
    if (alle || !strcmp(teil, "loeschen"))   teil_loeschen();
    if (alle || !strcmp(teil, "elza"))       teil_elza();
    if (alle || !strcmp(teil, "spind"))      teil_spind();
    if (alle || !strcmp(teil, "spielstand")) teil_spielstand();
    if (alle || !strcmp(teil, "leer"))       teil_leer();
    if (alle || !strcmp(teil, "kiste"))      teil_kiste();
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return fails ? 1 : 0;
}
