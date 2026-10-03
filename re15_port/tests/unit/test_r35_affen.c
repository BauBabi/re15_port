/* test_r35_affen.c — RIEGEL Runde 35 Spur J "affen": ROOM11C0 Gorilla-Boss (0x27) + Ada-Szene.
 *
 * Dossier analysis/befunde_runde35/J_affen.md, Belege include/re15_affen.h, Code engine/src/affen_11c0.c
 * + Haken in enemy_ai_common.c / main.c / emd_common.c.
 *
 * Teile (je ein ctest-Eintrag, Mechanik gemessen, nicht "Funktion existiert"):
 *   teile   EM027 aus CDEMD0.EMS: 18 Knochen, 22 Meshes; Parts 18..21 bekommen die Welttransformation des
 *           Binders FUN_8001e5b0 (Identitaet, rel = EMR[8+6i] = (3,72,3)/(75,1,78)/(0,79,1)/(79,1,80));
 *           Part 17 (hat einen Knochen) wird NICHT umgebogen.
 *   band    ROOM11C0, Kampf-Layout: eine NPC-Laeuferin (Typ 0x42, Sub 5 RUN) mit +0x82 = 2 erreicht
 *           (-18214,-7229) im Streifenwagen (Ankunftsbit 5/1); mit +0x82 = 0 klemmt die Band-0-Zelle
 *           (x <= -17790, FUN_8003b0a4) — die Mechanik des Ada-Befunds.
 *   ada     ROOM11C0, Szenen-Layout, die ECHTE Szene (sub02 -> sub07): Ada erreicht den Wagen und wird mit
 *           y = 20000 versenkt (@0x1C74); beide Kill-Bits (7,0x60)/(7,0x61) -> sub01 feuert sub03 -> y = 0,
 *           rot_y = 512 (@0x1B52/@0x1B56) und sie laeuft zu (-16211,-8183) (@0x1B70).
 *   sprung  Gorilla im Kampf-Layout: drei Boden-Flinches (HURT Spur 0) -> Exit-Sub 3, 3, 7 (NUTZER-VORGABE 3),
 *           danach wieder 3, 3, 7 (Zaehler setzt sich beim Sprung zurueck).
 *   flug    LEAP Phase 2 (in der Luft, +0x8c = 240): ein Bild verschiebt den Gorilla um genau EINEN
 *           c1a4-Schritt (240), nicht um den doppelten (Port-Defekt "double-advance", @0x80118cc0-dc4).
 *   wagen   Messschiene (kein Riegel-Urteil ausser Plausibilitaet): die Klappe (Objekt 0) waehrend des
 *           Umklappens in Cut 12 — rot_z-Folge aus Speed_set/Add_speed/Add_aspeed (@0x80040f14/f40/fd4).
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
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_affen.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

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

/* ---------------------------------------------------------------------------------------------- */
/* Raum-Harness (Muster test_r34n_d_adaruf.c: Bildfolge wie platform/pc/main.c)                     */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;

static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR,
             (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    return 0;
}

static void frame(uint16_t held, uint16_t edge)
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
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut, int bilder)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
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
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int f = 0; f < bilder; f++) frame(0, 0);
    return 0;
}

static re15_actor_t *aktor_vom_typ(uint8_t typ, int nth)
{
    int k = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) { if (k == nth) return &g_actors[s]; k++; }
    return NULL;
}

static double dist2d(int32_t x0, int32_t z0, int32_t x1, int32_t z1)
{
    double dx = (double)x0 - x1, dz = (double)z0 - z1; return sqrt(dx * dx + dz * dz);
}

/* ---------------------------------------------------------------------------------------------- */
static void teil_teile(void)
{
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    PRUEF(ems != NULL, "CDEMD0.EMS geladen (%u B)", (unsigned)n);
    if (!ems) return;
    int idx = re15_ems_index_for_type(0x27);
    size_t off = 0, len = 0;
    PRUEF(idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0, "EM027 = Blob %d @0x%x (%u B)", idx, (unsigned)off, (unsigned)len);
    static re15_md1_t md1; static re15_emd_skeleton_t sk; static re15_emd_animation_t an;
    int rc = re15_emd_parse_container(ems + off, len, &md1, &sk, &an, NULL);
    PRUEF(rc == 0, "EMD-Container geparst (rc=%d)", rc);
    PRUEF(sk.bone_count == 18 && md1.mesh_count == 22, "EM027: %d Knochen, %d Meshes (Original: EMR+4 = 18, MD1+8>>1 = 22)", sk.bone_count, md1.mesh_count);
    PRUEF(sk.emr_raw != NULL && sk.emr_raw_size > 8 + 6 * 22, "EMR-Rohdaten am Skelett (%u B)", (unsigned)sk.emr_raw_size);
    static const int16_t erwartet[4][3] = { {3, 72, 3}, {75, 1, 78}, {0, 79, 1}, {79, 1, 80} };   /* = Bytes der Kindtabelle @EMR+0x74 */
    for (int part = 18; part < 22; part++) {
        int32_t rot[9] = {0}, tr[3] = {0};
        int r = re15_affen_surplus_part_world(&sk, part, rot, tr);
        int ident = rot[0] == 0x1000 && rot[4] == 0x1000 && rot[8] == 0x1000 && rot[1] == 0 && rot[2] == 0 && rot[3] == 0 && rot[5] == 0 && rot[6] == 0 && rot[7] == 0;
        PRUEF(r == 1 && ident && tr[0] == erwartet[part - 18][0] && tr[1] == erwartet[part - 18][1] && tr[2] == erwartet[part - 18][2],
              "Part %d: weltfest Identitaet, t=(%d,%d,%d) (FUN_8001e5b0 rel=EMR[8+6*%d], Eltern DAT_80072d4c)", part, tr[0], tr[1], tr[2], part);
    }
    {
        int32_t rot[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9}, tr[3] = {11, 12, 13};
        int r = re15_affen_surplus_part_world(&sk, 17, rot, tr);
        PRUEF(r == 0 && rot[0] == 1 && tr[0] == 11, "Part 17 (Knochen 17) bleibt auf seiner Knochenpose (r=%d)", r);
    }
    free(ems);
}

/* ---------------------------------------------------------------------------------------------- */
static void teil_band(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);                              /* Szene gesehen -> Kampf-Layout, kein sub02 */
    if (room_boot(0x11C0, -22604, 14455, 0, 0, 5) != 0) return;
    re15_actor_t *ada = aktor_vom_typ(0x42, 0);
    PRUEF(ada != NULL, "Ada (0x42) im Kampf-Layout vorhanden");
    if (!ada) return;
    /* Gorillas parken (Bit 0x20 = Root-Skip @0x80116df4-f8), damit nur die Laeuferin misst. */
    for (int k = 0; k < 2; k++) { re15_actor_t *g = aktor_vom_typ(0x27, k); if (g) { g->grid_id |= 0x20; g->x = 30000; g->z = 30000; } }
    for (int fall = 0; fall < 2; fall++) {
        int band = (fall == 0) ? 2 : 0;
        ada->x = -16000; ada->y = 0; ada->z = -6560; ada->rot_y = 2048; ada->floor = (uint8_t)band;
        ada->state = 4; ada->sub_state_1 = 5; ada->sub_state_2 = 0; ada->sub_state_3 = 0;
        ada->steer_x = -18214; ada->steer_z = -7229; ada->walk_dest_x = -18214; ada->walk_dest_z = -7229;
        ada->walk_flag_bit = 1; ada->anim_flags = 0; ada->walk_active = 0;
        re15_game_flag_set(5, 1, 0);
        int ank = -1;
        for (int f = 0; f < 150; f++) {
            frame(0, 0);
            if (re15_game_flag_get(5, 1) && ank < 0) ank = f;
        }
        double d = dist2d(ada->x, ada->z, -18214, -7229);
        printf("  Band %d: nach 150 Bildern bei (%d,%d), Abstand %.0f, Ankunft Bild %d\n", band, (int)ada->x, (int)ada->z, d, ank);
        if (band == 2)
            PRUEF(ank >= 0 && d < 300.0, "+0x82=2: keine Band-2-Zelle in 11C0 -> Ankunft im Wagen (Bild %d, Abstand %.0f)", ank, d);
        else
            PRUEF(ank < 0 && ada->x > -17790, "+0x82=0: die Band-0-Zelle x<=-17790 (flr=3, u0=0xff) klemmt (x=%d, keine Ankunft)", (int)ada->x);
    }
}

/* ---------------------------------------------------------------------------------------------- */
static void teil_ada(void)
{
    re15_game_state_init();                                       /* neues Spiel: (4,0x40)=0 -> Szenen-Layout */
    if (room_boot(0x11C0, -22604, 14455, 0, 0, 1) != 0) return;
    re15_actor_t *ada = aktor_vom_typ(0x42, 0);
    PRUEF(ada != NULL && ada->x == -8965 && ada->z == -14347, "Ada im Szenen-Layout @(-8965,-14347) (sub00 @0x1770)");
    if (!ada) return;
    int f_ank = -1, f_versteck = -1, f_sub7 = -1;
    for (int f = 0; f < 4000; f++) {
        frame(0, 0);
        if (f_sub7 < 0 && ada->state == 4 && ada->sub_state_1 == 5) f_sub7 = f;
        if (f_ank < 0 && f_sub7 >= 0 && dist2d(ada->x, ada->z, -18214, -7229) < 300.0) f_ank = f;
        if (f_versteck < 0 && ada->y == 20000) { f_versteck = f; break; }
    }
    PRUEF(f_sub7 >= 0, "sub07: Ada laeuft (Sub 5 RUN) ab Bild %d (@0x1C6A)", f_sub7);
    PRUEF(f_ank >= 0, "Ada erreicht den Streifenwagen (-18214,-7229) in Bild %d, Position (%d,%d)", f_ank, (int)ada->x, (int)ada->z);
    PRUEF(f_versteck >= 0, "Ada versteckt: y = 20000 ab Bild %d (@0x1C74 Member_set 01=20000)", f_versteck);
    /* Die Szene zu Ende laufen lassen (Cut_auto/Flags), dann beide Bosse "toeten": sub01 -> sub03. */
    for (int f = 0; f < 300; f++) frame(0, 0);
    PRUEF(ada->y == 20000, "Ada bleibt versenkt bis zum Sieg (y=%d)", (int)ada->y);
    re15_game_flag_set(7, 0x60, 1); re15_game_flag_set(7, 0x61, 1);
    int f_zurueck = -1;
    for (int f = 0; f < 200; f++) {
        frame(0, 0);
        if (ada->y == 0 && f_zurueck < 0) { f_zurueck = f; break; }
    }
    PRUEF(f_zurueck >= 0, "sub03: Ada kommt zurueck, y = 0 nach %d Bildern (@0x1B52), rot_y = %d (@0x1B56: 512)", f_zurueck, (int)ada->rot_y);
    PRUEF(ada->rot_y == 512, "Ada rot_y nach dem Auftauchen = 512 (Member_set 04 @0x1B56)");
    PRUEF(re15_game_flag_get(3, 0x43) == 1, "sub03 setzt (3,0x43) (@0x1B02)");
    int f_weg = -1;
    for (int f = 0; f < 1500; f++) {
        frame(0, 0);
        if (dist2d(ada->x, ada->z, -16211, -8183) < 300.0) { f_weg = f; break; }
    }
    PRUEF(f_weg >= 0, "Ada laeuft aus dem Wagen zu (-16211,-8183) (@0x1B70, Bild %d, bei (%d,%d))", f_weg, (int)ada->x, (int)ada->z);
}

/* ---------------------------------------------------------------------------------------------- */
static void teil_sprung(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -22604, 14455, 0, 0, 3) != 0) return;
    re15_actor_t *g = aktor_vom_typ(0x27, 0);
    re15_actor_t *g2 = aktor_vom_typ(0x27, 1);
    PRUEF(g != NULL && g->state == 1, "Gorilla 1 im Kampf-Layout aktiv (st=%d)", g ? g->state : -1);
    if (!g) return;
    if (g2) { g2->grid_id |= 0x20; g2->x = 30000; g2->z = 30000; }
    g->mag_hit_ctr = 0;
    int erwartet[6] = { 3, 3, 7, 3, 3, 7 };
    for (int hit = 0; hit < 6; hit++) {
        /* Treffer wie re15_enemy_take_damage (@0x80012fd4-13020): +0x7=0, +0x6=1, +0x5=Zeile 3 (Spur 0 =
         * Boden-Flinch @0x801214a8[3]), hp -= 12, +0x93|=1, +0x4=2. */
        g->state = 2; g->sub_state_1 = 3; g->sub_state_2 = 1; g->sub_state_3 = 0;
        g->hp = (int16_t)(g->hp - 12); g->hit_react |= 1;
        int exit_sub = -1;
        for (int f = 0; f < 60; f++) {
            frame(0, 0);
            if (g->state == 1) { exit_sub = g->sub_state_1; break; }
        }
        PRUEF(exit_sub == erwartet[hit], "Treffer %d: Flinch-Exit -> Sub %d (erwartet %d; 7 = Vergeltungs-Sprung @0x8011b188-98 erst beim 3., sonst 3)", hit + 1, exit_sub, erwartet[hit]);
        /* den laufenden Sprung/Jagd-Zustand neutralisieren, damit der naechste Treffer sauber beginnt */
        g->mag_airborne = 0; g->y = g->dog_floor_y; g->floor = 0; g->hit_react = 0;
        g->state = 1; g->sub_state_1 = 3; g->sub_state_2 = 0; g->sub_state_3 = 0;
    }
}

/* ---------------------------------------------------------------------------------------------- */
static void teil_flug(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -22604, 14455, 0, 0, 3) != 0) return;
    re15_actor_t *g = aktor_vom_typ(0x27, 0);
    re15_actor_t *g2 = aktor_vom_typ(0x27, 1);
    PRUEF(g != NULL, "Gorilla 1 vorhanden");
    if (!g) return;
    if (g2) { g2->grid_id |= 0x20; g2->x = 30000; g2->z = 30000; }
    /* LEAP Phase 2 (in der Luft) @0x80118b14: +0x8c = 240, +0x1e0 = 1, hoch ueber dem Boden */
    g->state = 1; g->sub_state_1 = 7; g->sub_state_2 = 2; g->sub_state_3 = 0;
    g->crow_speed = 240; g->mag_airborne = 1; g->ai_timer = 0; g->hit_react = 1;
    g->dog_floor_y = 0; g->y = -3000; g->rot_y = 0; g->x = -9013; g->z = -15461;
    int32_t x0 = g->x, z0 = g->z;
    frame(0, 0);
    double d = dist2d(g->x, g->z, x0, z0);
    PRUEF(g->sub_state_1 == 7 && g->sub_state_2 == 2, "Gorilla noch im Flug (sub %d phase %d)", g->sub_state_1, g->sub_state_2);
    PRUEF(d >= 238.0 && d <= 242.0, "ein Flugbild verschiebt um %.0f = EIN c1a4-Schritt (+0x8c=240), kein doppelter 480 (@0x80118cc0/cdc/d6c -> Epilog)", d);
}

/* ---------------------------------------------------------------------------------------------- */
static void teil_wagen(void)
{
    re15_game_state_init();
    if (room_boot(0x11C0, -22604, 14455, 0, 0, 1) != 0) return;
    PRUEF(g_scd.prop_count >= 1, "Objekt 0 (Klappe) vorhanden (%d Props)", (int)g_scd.prop_count);
    int16_t rz0 = g_scd.props[0].rot_z;
    PRUEF(g_scd.props[0].x == -840 && g_scd.props[0].y == -2930 && g_scd.props[0].z == -19110 && g_scd.props[0].rot_x == -72 && g_scd.props[0].rot_y == -1400 && rz0 == 48,
          "Szenen-Lage der Klappe (-840,-2930,-19110) rot (-72,-1400,48) (sub00 @0x17B0-17C4): (%d,%d,%d) rot (%d,%d,%d)",
          (int)g_scd.props[0].x, (int)g_scd.props[0].y, (int)g_scd.props[0].z, (int)g_scd.props[0].rot_x, (int)g_scd.props[0].rot_y, (int)rz0);
    /* bis Cut 12 laufen, dann die rot_z-Folge mitschreiben */
    int f_cut12 = -1; int16_t folge[60]; int n = 0; int16_t rz_max = rz0;
    for (int f = 0; f < 4000; f++) {
        frame(0, 0);
        if (f_cut12 < 0 && (int)g_scd.cam_id == 12) f_cut12 = f;
        if (f_cut12 >= 0 && n < 60) folge[n++] = g_scd.props[0].rot_z;
        if (g_scd.props[0].rot_z > rz_max) rz_max = g_scd.props[0].rot_z;
        if (f_cut12 >= 0 && n >= 60) break;
    }
    PRUEF(f_cut12 >= 0, "Cut 12 erreicht (Bild %d)", f_cut12);
    printf("  rot_z ab Cut 12:");
    for (int i = 0; i < n; i++) printf(" %d", (int)folge[i]);
    printf("\n");
    /* Script-Arithmetik @0x80040f14/f40/fd4: 12x (+175-6k) = +1704 -> 1752, dann 6 Wackler (-164,+164,-96,+96,-34,+34) */
    PRUEF(n >= 40 && folge[n - 1] == 1752, "Klappe endet bei rot_z = 1752 (= main00-Endlage @0x1732, Add_speed/Add_aspeed byte-true), ist %d", (int)folge[n - 1]);
    PRUEF(rz_max == 1752, "rot_z-Maximum 1752 (Spitze der Folge), ist %d", (int)rz_max);
}

/* ---------------------------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    printf("test_r35_affen: Teil %s\n", teil);
    if (!strcmp(teil, "teile")  || !strcmp(teil, "alle")) teil_teile();
    if (!strcmp(teil, "band")   || !strcmp(teil, "alle")) teil_band();
    if (!strcmp(teil, "ada")    || !strcmp(teil, "alle")) teil_ada();
    if (!strcmp(teil, "sprung") || !strcmp(teil, "alle")) teil_sprung();
    if (!strcmp(teil, "flug")   || !strcmp(teil, "alle")) teil_flug();
    if (!strcmp(teil, "wagen")  || !strcmp(teil, "alle")) teil_wagen();
    printf("test_r35_affen %s: %s (%d Fehler)\n", teil, g_fail ? "FEHLER" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
