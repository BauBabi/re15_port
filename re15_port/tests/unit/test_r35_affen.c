/* test_r35_affen.c — RIEGEL Runde 35 Spur J "affen": ROOM11C0 Gorilla-Boss (0x27) + Ada-Szene.
 *
 * Dossier analysis/befunde_runde35/J_affen.md, Belege include/re15_affen.h, Code engine/src/affen_11c0.c
 * + Haken in enemy_ai_common.c / main.c / emd_common.c.
 *
 * Teile (je ein ctest-Eintrag, Mechanik gemessen, nicht "Funktion existiert"):
 *   teile   EM027 aus CDEMD0.EMS: 18 Knochen, 22 Meshes; Part 18 (Brustschale) haengt am Rumpfknochen 1 mit
 *           rel (102,-810,0) (Gorilla-INIT @0x80117200-3c) — gemessen gegen die Part-Matrizen des Original-
 *           Savestates (t = (-4279,-2816,-16899)); Parts 19..21 bekommen die Welttransformation des Binders
 *           FUN_8001e5b0 (Identitaet, rel = EMR[8+6i] = (75,1,78)/(0,79,1)/(79,1,80)); Part 17 (hat einen
 *           Knochen) wird NICHT umgebogen.
 *   kdsonde Punkt 4a: die Knockdown-Sonde FUN_8001c2dc in ROOM11C0 — an Leons Szenen-Endpunkt (-7138,-12372)
 *           Flag 1 (Original-Savestate +0x9e = 1, +0x8c = 0), der Heavy-Knockdown [5] rutscht dort NICHT
 *           (vorher 4915 Einheiten); auf freiem Boden Flag 0 und der Sturz gleitet 500/Bild; [4] schlaegt nach
 *           einem Schritt in den Slam (Clip 0xf) um.
 *   biss    Punkt 4b: zwei Gorillas, der BEISSER steht hinter Leon, der naehere steht vor ihm -> der Flinch
 *           nimmt die Richtung des Beissers (Clip 9, a780 @0x80118488), nicht die des naechsten Gegners.
 *   frac    Punkt 4/5: der Crossfade-Zaehler +0x8f des Gorillas baut je anim_set-Aufruf um 1 ab
 *           (@0x8001f5a8-b4): CHASE-Eintritt setzt 7, nach 7 Bildern steht 0 (Original-Savestates: 7 - Bild).
 *   anker   Punkt 5: der Pin-Latch (sub 15 Phase 2) setzt den Anker des Gorillas und kopiert ihn an den Spieler
 *           (FUN_8001ac38 @0x8011ac18, Kopie @0x8001ad30/48) — Leon landet nicht mehr am Raumursprung.
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
 *   brust   Rear-up-Pin-Release: Clip 3 ab Bild 0x16 -> Phase 5/6 -> sub 2 (Clip 3 ab 0x1d) -> CHASE = der
 *           Brustschlag (Punkt 5), gemessen als Bildfolge.
 *   wagen   Punkt 2: der eingefrorene Szenen-Record bekommt seinen INIT im Spawn-Bild (Zustand 1, HP 180, Scale
 *           0x1b33 @0x80117148, Sce_em_set-Wurzelaufruf @0x8004259c) = die Ursache; in Cut 12 sitzt G1 auf
 *           y = -2500 im Wagen (@0x1996) und tritt bei (-3617,0,-17798) aus (@0x1A2E); dazu die Klappe (Objekt 0)
 *           waehrend des Umklappens — rot_z-Folge aus Speed_set/Add_speed/Add_aspeed (@0x80040f14/f40/fd4).
 *   takt    Punkt 4 (M1): zwei Gorillas ab der Original-Lage Bild F195 (GDB-Einzelbild-Spur) -> Treffer-Bilder/-Abstaende.
 *   griff   Punkt 4/5 (M1): verbundener Rear-up-Griff ab derselben Lage (e1 sub 15 in F250, wie das GDB-Experiment im
 *           Original): Pin F254, Leon steht bis Bild 0xb, erste Wurf-Platzierung F266, 0 HP.
 *   npcband M4: NPC-Wandklemme mit +0x82 statt y — je Raum (10D0, 1050, 1090, 10B1, 1170, 11B0, 4001, 4031, 6030, 6031)
 *           600 Bilder: laeuft ein NPC, dessen +0x82 != band_from_y(y) ist? (10D0/1050 gepinnt: nein).
 *   schrot  M3: Schrot-Treffer (Spur 1, Zeile 7) als 3. Treffer ab der Original-Lage F195 (GDB-Experiment): Exit-Sub 7,
 *           Landung, SELECTOR, Heavy-Anlauf, KEIN Zonen-Sprung — Bild fuer Bild wie das Original.
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
void re15_player_cmd_zero(void);

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
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;             /* Raumkollision fuer die Klemmen (wie probe_1030_*) */
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
    for (int part = 19; part < 22; part++) {
        int32_t rot[9] = {0}, tr[3] = {0};
        static re15_skel_pose_t dummy[18]; re15_skel_pose_t aus;
        memset(dummy, 0, sizeof dummy);
        int r = re15_affen_surplus_part_world(&sk, part, rot, tr);
        int ident = rot[0] == 0x1000 && rot[4] == 0x1000 && rot[8] == 0x1000 && rot[1] == 0 && rot[2] == 0 && rot[3] == 0 && rot[5] == 0 && rot[6] == 0 && rot[7] == 0;
        PRUEF(r == 1 && ident && tr[0] == erwartet[part - 18][0] && tr[1] == erwartet[part - 18][1] && tr[2] == erwartet[part - 18][2],
              "Part %d: weltfest Identitaet, t=(%d,%d,%d) (FUN_8001e5b0 rel=EMR[8+6*%d], Eltern DAT_80072d4c; Savestate s021: dieselben Werte)", part, tr[0], tr[1], tr[2], part);
        PRUEF(re15_affen_part_attach(0x27, part, dummy, 18, &aus) == 0, "Part %d wird NICHT umgehaengt", part);
    }
    {
        /* Part 18: die Weltmatrix von Part 1 aus dem ORIGINAL-Savestate (orig_scene/r3 s021_t036.41, Record
         * 0x8016f3b4 + 1*0xac + 0x40) hinein, die Weltlage von Part 18 desselben Savestates muss herauskommen. */
        static re15_skel_pose_t posen[18]; re15_skel_pose_t aus;
        static const int32_t m1[9] = { -2906, 2987, -5562, 5066, 4757, -97, 3766, -4087, -4174 };
        memset(posen, 0, sizeof posen); memset(&aus, 0, sizeof aus);
        for (int k = 0; k < 9; k++) posen[1].rot[k] = m1[k];
        posen[1].trans[0] = -3615; posen[1].trans[1] = -2001; posen[1].trans[2] = -17801;
        int r = re15_affen_part_attach(0x27, 18, posen, 18, &aus);
        int rotgleich = 1; for (int k = 0; k < 9; k++) if (aus.rot[k] != m1[k]) rotgleich = 0;
        PRUEF(r == 1 && rotgleich, "Part 18 haengt an Knochen 1: Rotation = Rumpf (INIT @0x80117210-1c, lokale Identitaet @0x80117234-3c)");
        PRUEF(aus.trans[0] == -4279 && aus.trans[1] == -2816 && aus.trans[2] == -16899,
              "Part 18: t = T1 + R1*(102,-810,0) = (%d,%d,%d); Original-Savestate Part 18: (-4279,-2816,-16899)",
              aus.trans[0], aus.trans[1], aus.trans[2]);
        PRUEF(re15_affen_part_attach(0x29, 18, posen, 18, &aus) == 0 && re15_affen_part_attach(0x30, 16, posen, 16, &aus) == 0,
              "0x29 Part 18 / 0x30 Part 16: keine Umhaengung (Roh-Scan sw rX,0xc84/0xb2c(rY): nur der Gorilla-INIT)");
        int32_t rot[9] = {0}, tr[3] = {0};
        PRUEF(re15_affen_surplus_part_world(&sk, 18, rot, tr) == 1 && tr[0] == 3 && tr[1] == 72 && tr[2] == 3,
              "Binder-Standard von Part 18 waere (3,72,3) — der INIT ueberschreibt ihn (main.c: affe_fest geht vor)");
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
        if (fall == 1) {                                             /* zweiter Fall: Raum frisch aufbauen */
            if (room_boot(0x11C0, -22604, 14455, 0, 0, 5) != 0) return;
            ada = aktor_vom_typ(0x42, 0);
            if (!ada) { PRUEF(0, "Ada im zweiten Aufbau"); return; }
            for (int k = 0; k < 2; k++) { re15_actor_t *g = aktor_vom_typ(0x27, k); if (g) { g->grid_id |= 0x20; g->x = 30000; g->z = 30000; } }
        }
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
    for (int f = 0; f < 900; f++) {                      /* sub03: erst Leons Gang zu (-14280,-8543) + Drehung (@0x1B26-1B38) */
        frame(0, 0);
        if ((f % 60) == 0)
            printf("    +%d: Ada y=%d st=%d/%d/%d @(%d,%d) rot=%d flag(3,0x43)=%d\n", f, (int)ada->y, ada->state, ada->sub_state_1, ada->sub_state_2,
                   (int)ada->x, (int)ada->z, (int)ada->rot_y, re15_game_flag_get(3, 0x43));
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
    /* M3 (Nachbesserung 1): dieselbe Vorgabe in den Spuren 1 (Luft-Treffer, Zeile 7 = Schrotflinte, Original-Exit
     * +0x5=7 @0x8011b3c8-cc) und 2 (Sturz, Zeile 9, Exit @0x8011b6c4-c8) und GEMISCHT ueber alle drei Spuren —
     * der Zaehler gehoert dem Gorilla, nicht der Waffe. */
    static const struct { uint8_t zeile; const char *name; } spur[3] = { {7, "Spur 1 (Zeile 7, Luft-Treffer)"},
                                                                         {9, "Spur 2 (Zeile 9, Sturz)"},
                                                                         {0, "gemischt 3/7/9"} };
    static const uint8_t misch[6] = { 3, 7, 9, 9, 3, 7 };
    for (int sp = 0; sp < 3; sp++) {
        g->mag_hit_ctr = 0; g->hp = 180;
        for (int hit = 0; hit < 6; hit++) {
            uint8_t z = spur[sp].zeile ? spur[sp].zeile : misch[hit];
            g->state = 2; g->sub_state_1 = z; g->sub_state_2 = 1; g->sub_state_3 = 0;
            g->hp = (int16_t)(g->hp - 12); g->hit_react |= 1;
            int exit_sub = -1;
            for (int f = 0; f < 200; f++) {
                frame(0, 0);
                if (g->state == 1) { exit_sub = g->sub_state_1; break; }
            }
            PRUEF(exit_sub == erwartet[hit], "%s, Treffer %d (Zeile %d): Exit -> Sub %d (erwartet %d)", spur[sp].name, hit + 1, z, exit_sub, erwartet[hit]);
            g->mag_airborne = 0; g->y = g->dog_floor_y; g->floor = 0; g->hit_react = 0;
            g->state = 1; g->sub_state_1 = 3; g->sub_state_2 = 0; g->sub_state_3 = 0;
        }
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
    {   /* M2 (Nachbesserung 1): die URSACHE von Punkt 2 — der eingefrorene Szenen-Record (grid 0x30) bekommt seinen
         * INIT im Spawn-Bild (Sce_em_set: Wurzel mit geloeschtem Bit 0x20, `jalr 0x80072bac[typ]` @0x8004259c).
         * Original-Savestate r3 s001 t=6.11: beide st=1/0/0/0, hp 180, +0x166 = 0x1b33, (-1220,-20000,-21568) /
         * (-554,-20000,-25423), grid 0x30. Ohne den Aufruf: Zustand 0, Scale 0 (= 1,0x statt 1,7x). */
        re15_actor_t *s1 = aktor_vom_typ(0x27, 0), *s2 = aktor_vom_typ(0x27, 1);
        PRUEF(s1 && s2, "zwei Gorilla-Records im Szenen-Layout");
        if (!s1 || !s2) return;
        PRUEF(s1->grid_id == 0x30 && s2->grid_id == 0x30, "beide eingefroren (grid 0x30 = Bit 0x20 Root-Skip @0x80116df4-f8): 0x%02x / 0x%02x",
              s1->grid_id, s2->grid_id);
        PRUEF(s1->state == 1 && s2->state == 1 && s1->sub_state_1 == 0 && s2->sub_state_1 == 0,
              "INIT im Spawn-Bild gelaufen: Zustand %d/%d, sub %d/%d (Original t=6.11: 1/0 und 1/0)",
              s1->state, s2->state, s1->sub_state_1, s2->sub_state_1);
        PRUEF(s1->hp == 180 && s2->hp == 180, "HP %d / %d = 180 (Original t=6.11: 180/180)", (int)s1->hp, (int)s2->hp);
        PRUEF(s1->render_scale_q12 == 0x1b33 && s2->render_scale_q12 == 0x1b33,
              "Scale +0x166 = 0x%x / 0x%x (INIT `ori v0,zero,0x1b33` / `sh v0,358(v1)` @0x80117148-4c; Original t=6.11: 0x1b33)",
              (unsigned)(uint16_t)s1->render_scale_q12, (unsigned)(uint16_t)s2->render_scale_q12);
        PRUEF(s1->x == -1220 && s1->y == -20000 && s1->z == -21568 && s2->x == -554 && s2->y == -20000 && s2->z == -25423,
              "Spawn-Lagen (-1220,-20000,-21568) / (-554,-20000,-25423) (sub00 @0x1784/@0x1798): (%d,%d,%d) / (%d,%d,%d)",
              (int)s1->x, (int)s1->y, (int)s1->z, (int)s2->x, (int)s2->y, (int)s2->z);
    }
    PRUEF(g_scd.prop_count >= 1, "Objekt 0 (Klappe) vorhanden (%d Props)", (int)g_scd.prop_count);
    int16_t rz0 = g_scd.props[0].rot_z;
    PRUEF(g_scd.props[0].x == -840 && g_scd.props[0].y == -2930 && g_scd.props[0].z == -19110 && g_scd.props[0].rot_x == -72 && g_scd.props[0].rot_y == -1400 && rz0 == 48,
          "Szenen-Lage der Klappe (-840,-2930,-19110) rot (-72,-1400,48) (sub00 @0x17B0-17C4): (%d,%d,%d) rot (%d,%d,%d)",
          (int)g_scd.props[0].x, (int)g_scd.props[0].y, (int)g_scd.props[0].z, (int)g_scd.props[0].rot_x, (int)g_scd.props[0].rot_y, (int)rz0);
    /* bis Cut 12 laufen, dann die rot_z-Folge mitschreiben */
    int f_cut12 = -1; int16_t folge[60]; int n = 0; int16_t rz_max = rz0;
    re15_actor_t *g1 = aktor_vom_typ(0x27, 0), *g2 = aktor_vom_typ(0x27, 1);
    int f_frei = -1, y_wagen_bilder = 0, y_wagen_falsch = 0, fst = -1, fmo = -1; int32_t fx = 0, fy = 1, fz = 0;
    for (int f = 0; f < 4000; f++) {
        frame(0, 0);
        if (f_cut12 < 0 && (int)g_scd.cam_id == 12) f_cut12 = f;
        if (f_cut12 >= 0 && n < 60) folge[n++] = g_scd.props[0].rot_z;
        /* M2: im Cut 12 VOR der Freigabe sitzt G1 im Wagen auf y = -2500 (`Member_set 01 = -2500` @0x1996),
         * die Freigabe setzt ihn auf (-3617,0,-17798) mit grid 0x10 (@0x1A2E: Member_set 00/01/02, 0C=16). */
        if (f_cut12 >= 0 && f_frei < 0 && g1) {
            if (g1->grid_id == 0x10) { f_frei = f; fx = g1->x; fy = g1->y; fz = g1->z; fst = g1->state; fmo = (int)g1->motion; }
            else { y_wagen_bilder++; if (g1->y != -2500) y_wagen_falsch++; }
        }
        if (f_cut12 >= 0 && (n % 10) == 1 && g1 && g2)   /* Messschiene Punkt 2: Lage der Gorillas waehrend Cut 12 */
            printf("    Cut12+%d: G1 y=%d @(%d,%d) g=0x%02x st=%d mo=%d | G2 y=%d @(%d,%d) g=0x%02x\n", n - 1,
                   (int)g1->y, (int)g1->x, (int)g1->z, g1->grid_id, g1->state, (int)g1->motion,
                   (int)g2->y, (int)g2->x, (int)g2->z, g2->grid_id);
        if (g_scd.props[0].rot_z > rz_max) rz_max = g_scd.props[0].rot_z;
        if (f_cut12 >= 0 && n >= 60 && f_frei >= 0) break;
    }
    PRUEF(f_cut12 >= 0, "Cut 12 erreicht (Bild %d)", f_cut12);
    PRUEF(y_wagen_bilder > 0 && y_wagen_falsch == 0,
          "G1 sitzt in Cut 12 bis zur Freigabe auf y = -2500 im Wagen (%d Bilder, %d abweichend; Original-Savestate t=34.89: y=-2500)",
          y_wagen_bilder, y_wagen_falsch);
    PRUEF(f_frei >= 0 && fx == -3617 && fy == 0 && fz == -17798,
          "Austritt: G1 frei (grid 0x10) %d Bilder nach Cut-12-Beginn bei (%d,%d,%d) (Original t=36.41: (-3617,0,-17798) g=10)",
          f_frei - f_cut12, (int)fx, (int)fy, (int)fz);
    PRUEF(fst == 1, "beim Austritt laeuft die KI im Zustand 1 (INIT war schon im Spawn-Bild), Clip %d", fmo);
    printf("  rot_z ab Cut 12:");
    for (int i = 0; i < n; i++) printf(" %d", (int)folge[i]);
    printf("\n");
    /* Script-Arithmetik @0x80040f14/f40/fd4: 12x (+175-6k) = +1704 -> 1752, dann 6 Wackler (-164,+164,-96,+96,-34,+34) */
    PRUEF(n >= 40 && folge[n - 1] == 1752, "Klappe endet bei rot_z = 1752 (= main00-Endlage @0x1732, Add_speed/Add_aspeed byte-true), ist %d", (int)folge[n - 1]);
    PRUEF(rz_max == 1752, "rot_z-Maximum 1752 (Spitze der Folge), ist %d", (int)rz_max);
}


/* ---------------------------------------------------------------------------------------------- */
/* Punkt 5: der Brustschlag = Clip 3 ab Bild 0x16 nach dem verbundenen Rear-up-Pin (sub 15 Phase 4
 * @0x8011ad50-78), Phase 5 bis Bild 0x31 (@0x8011adc0-d4), Phase 6 -> sub 2 Phase 1 mit Clip 3 ab
 * Bild 0x1d (@0x8011ae30-58), Clip-Ende -> CHASE (sub 3). Gemessen als Bildfolge der Mechanik. */
static void teil_brust(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -22604, 14455, 0, 0, 3) != 0) return;
    re15_actor_t *g = aktor_vom_typ(0x27, 0);
    re15_actor_t *g2 = aktor_vom_typ(0x27, 1);
    PRUEF(g != NULL, "Gorilla 1 vorhanden");
    if (!g) return;
    if (g2) { g2->grid_id |= 0x20; g2->x = 30000; g2->z = 30000; }
    g->state = 1; g->sub_state_1 = 15; g->sub_state_2 = 4; g->sub_state_3 = 0;   /* RELEASE-Init nach dem Pin */
    g->hit_react = 1; g->y = g->dog_floor_y + 1600;
    frame(0, 0);
    PRUEF(g->sub_state_1 == 15 && g->sub_state_2 == 5 && g->motion == 3 && g->anim_frame >= 0x16 && g->anim_frame <= 0x17,
          "Phase 4 -> 5: Clip 3 ab Bild 0x16 (@0x8011ad50-78): sub %d phase %d clip %d bild %d",
          g->sub_state_1, g->sub_state_2, (int)g->motion, (int)g->anim_frame);
    int f_sub2 = -1, f_chase = -1, clip3_bilder = 0, maxbild = 0;
    for (int f = 0; f < 200; f++) {
        frame(0, 0);
        if (g->motion == 3) { clip3_bilder++; if (g->anim_frame > maxbild) maxbild = g->anim_frame; }
        if (f_sub2 < 0 && g->sub_state_1 == 2) f_sub2 = f;
        if (f_sub2 >= 0 && g->sub_state_1 == 3) { f_chase = f; break; }
    }
    PRUEF(f_sub2 >= 0, "Phase 6 -> sub 2 (Clip 3 ab Bild 0x1d, @0x8011ae30-58) nach %d Bildern", f_sub2);
    PRUEF(f_chase >= 0 && maxbild >= 0x45, "Clip 3 laeuft bis zum Ende (Bild %d von 70) und muendet in CHASE (sub 3) nach %d Bildern; %d Bilder Clip 3 = der aufrechte Brustschlag",
          maxbild, f_chase, clip3_bilder);
}

/* ---------------------------------------------------------------------------------------------- */
/* Punkt 4a: Knockdown-Sonde FUN_8001c2dc (re15_affen_kd_sonde) + die beiden Sturz-Handler am gebauten Stand. */
static void gorillas_parken(void)
{
    for (int k = 0; k < 2; k++) { re15_actor_t *g = aktor_vom_typ(0x27, k); if (g) { g->grid_id |= 0x20; g->x = 30000; g->z = 30000; } }
}

static void teil_kdsonde(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -7138, -12372, 647, 5, 3) != 0) return;
    gorillas_parken();
    PRUEF(re15_affen_kd_sonde(&s_rdt, -7138, 0, -12372, RE15_KD_SONDE_RADIUS) == 1,
          "Sonde an Leons Szenen-Endpunkt (-7138,-12372): Flag 1 (Typ-2-Zelle 20199x18187 @(-15400,-13266), floor 2 = Bit 0x200; Savestate s035 +0x9e = 1)");
    PRUEF(re15_affen_kd_sonde(&s_rdt, -9013, 0, -15461, RE15_KD_SONDE_RADIUS) == 0,
          "Sonde auf freiem Boden (-9013,-15461): Flag 0 (kein Zell-AABB + 450 des Bandes 0)");
    /* [5] Sturz vorwaerts (Treffer von hinten) am Szenen-Endpunkt: kein Rutschen */
    pl->x = -7138; pl->z = -12372; pl->rot_y = 647; pl->hp = 88;
    re15_player_knockdown_begin(1);
    int32_t x0 = pl->x, z0 = pl->z; double dmax = 0.0; int clip_c = 0;
    for (int f = 0; f < 30; f++) {
        frame(0, 0);
        double d = dist2d(pl->x, pl->z, x0, z0); if (d > dmax) dmax = d;
        if (pl->motion == 0x0c) clip_c++;
    }
    PRUEF(clip_c > 0, "[5] laeuft: Clip 0xc ueber %d Bilder (@0x800364a8-b0)", clip_c);
    PRUEF(dmax <= 20.0, "[5] am Szenen-Endpunkt: Versatz %.0f Einheiten (Original r3: 9; Port vorher: 4915) — Sonde nullt +0x8c (@0x80036594-b0)", dmax);
    /* [5] auf freiem Boden: der Sturz gleitet (erstes Bewegungsbild 500 - 5*0 = 500) */
    if (room_boot(0x11C0, -9013, -15461, 0, 5, 3) != 0) return;
    gorillas_parken();
    pl->x = -9013; pl->z = -15461; pl->rot_y = 0; pl->hp = 88;
    re15_player_knockdown_begin(1);
    x0 = pl->x; z0 = pl->z;
    double d1 = 0.0;
    for (int f = 0; f < 3 && d1 == 0.0; f++) { frame(0, 0); d1 = dist2d(pl->x, pl->z, x0, z0); }
    PRUEF(d1 >= 480.0 && d1 <= 520.0, "[5] auf freiem Boden: erstes Sturzbild traegt %.0f (500 = ori v0,zero,0x1f4 @0x800364c0, Decel 5*t erst ab t=1)", d1);
    /* [4] Sturz rueckwaerts (Treffer von vorn) am Szenen-Endpunkt: ein Schritt, dann Slam */
    if (room_boot(0x11C0, -7138, -12372, 647, 5, 3) != 0) return;
    gorillas_parken();
    pl->x = -7138; pl->z = -12372; pl->rot_y = 647; pl->hp = 88;
    re15_player_knockdown_begin(0);
    x0 = pl->x; z0 = pl->z; int f_slam = -1; dmax = 0.0;
    for (int f = 0; f < 12; f++) {
        frame(0, 0);
        double d = dist2d(pl->x, pl->z, x0, z0); if (d > dmax) dmax = d;
        if (f_slam < 0 && pl->motion == 0x0f) f_slam = f;
    }
    PRUEF(f_slam >= 0 && f_slam <= 2, "[4] am Szenen-Endpunkt: Slam-Clip 0xf in Bild %d (Sonde NACH dem Vorschub @0x80036214 -> Phase 5 @0x80036228-30)", f_slam);
    PRUEF(dmax <= 1010.0, "[4]: hoechstens EIN Rueckwaerts-Schritt von 1000 (ori v0,zero,0x3e8 @0x8003615c), gemessen %.0f", dmax);
}

/* ---------------------------------------------------------------------------------------------- */
/* Punkt 4b: die Biss-Richtung kommt vom BEISSER (a780 @0x80118488), nicht vom naechsten Gegner. */
static void teil_biss(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -9013, -15461, 0, 5, 3) != 0) return;
    re15_actor_t *a = aktor_vom_typ(0x27, 0), *b = aktor_vom_typ(0x27, 1);
    PRUEF(a != NULL && b != NULL, "zwei Gorillas im Kampf-Layout");
    if (!a || !b) return;
    /* Reine Richtungsregel zuerst (a780 @0x8001a788-a4, a0 = Spieler): */
    pl->rot_y = 0; a->rot_y = 0; b->rot_y = 2048;
    PRUEF(re15_affen_biss_clip(a, pl) == 0x09 && re15_affen_biss_clip(b, pl) == 0x08,
          "a780: gleiche Blickrichtung -> Clip 9 (von hinten, [3]); Gegenrichtung -> Clip 8 (frontal, [2])");
    /* Lage: Leon schaut nach +x. A (Beisser) steht HINTER ihm und schaut wie er; B steht VOR ihm, schaut ihn an,
     * ist naeher und geparkt (Bit 0x20) -> re15_nearest_hostile = B. */
    pl->x = -9013; pl->z = -15461; pl->rot_y = 0; pl->hp = 100; pl->hit_react = 0;
    a->x = pl->x - 700; a->z = pl->z; a->y = 0; a->rot_y = 0; a->grid_id = 0x10;
    b->x = pl->x + 500; b->z = pl->z; b->y = 0; b->rot_y = 2048; b->grid_id |= 0x20;
    a->state = 1; a->sub_state_1 = 5; a->sub_state_2 = 1; a->sub_state_3 = 0;      /* BITE B[5], Clip 0x12 */
    a->motion = 0x12; a->anim_frame = 0x0b; a->anim_frac = 0; a->dog_blocked_ctr = 0; a->hit_react = 0;
    const re15_actor_t *nah = re15_nearest_hostile(pl);
    PRUEF(nah == b, "naechster Gegner vor dem Biss = der GEPARKTE vor Leon (Abstand 500 < 700)");
    int hp0 = pl->hp, f_biss = -1, clip = -1;
    for (int f = 0; f < 8; f++) {
        /* Lage je Bild festhalten: Koerper-Schub und Wurzelbewegung sollen die Biss-Geometrie nicht verschieben */
        pl->x = -9013; pl->z = -15461; a->x = pl->x - 700; a->z = pl->z; b->x = pl->x + 500; b->z = pl->z;
        a->hit_radius_min = 0; b->hit_radius_min = 0;   /* kein Koerper-Schub (re15_body_push_player ueberspringt Radius 0):
                                                         * der Riegel misst die RICHTUNG, nicht den Abstandshalter */
        frame(0, 0);
        printf("    Bild %d: HP %d Clip %d | A sub %d/%d Bild %d @(%d,%d) | PL @(%d,%d)\n", f, (int)pl->hp, (int)pl->motion,
               a->sub_state_1, a->sub_state_2, (int)a->anim_frame, (int)a->x, (int)a->z, (int)pl->x, (int)pl->z);
        if (pl->hp < hp0 && f_biss < 0) { f_biss = f; clip = (int)pl->motion; break; }
    }
    PRUEF(f_biss >= 0 && pl->hp == hp0 - 6, "Biss sitzt in Bild %d: HP %d -> %d (-6 @0x80118460-6c)", f_biss, hp0, (int)pl->hp);
    PRUEF(clip == 0x09, "Flinch-Clip = %d: Richtung des BEISSERS (von hinten = 9, @0x80118488-9c), nicht die des naeheren Gegners (waere 8)", clip);
    for (int f = 0; f < 3; f++) frame(0, 0);
    PRUEF(pl->motion == 0x09, "der HP-Detektor ueberschreibt die Richtung im Folgebild nicht (Clip %d)", (int)pl->motion);
}

/* ---------------------------------------------------------------------------------------------- */
/* Punkt 4/5: +0x8f (anim_frac) des Gorillas baut ab — anim_set FUN_8001f314 @0x8001f5a8-b4. */
static void teil_frac(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -22604, 14455, 0, 0, 3) != 0) return;
    re15_actor_t *g = aktor_vom_typ(0x27, 0), *g2 = aktor_vom_typ(0x27, 1);
    PRUEF(g != NULL, "Gorilla 1 vorhanden");
    if (!g) return;
    if (g2) { g2->grid_id |= 0x20; g2->x = 30000; g2->z = 30000; }
    g->state = 1; g->sub_state_1 = 3; g->sub_state_2 = 0; g->sub_state_3 = 0;      /* CHASE-Eintritt: Clip 4 -> 5, +0x8f = 7 */
    g->dog_blocked_ctr = 100;                                                     /* kein Biss-Commit waehrend der Messung */
    int folge[10];
    for (int f = 0; f < 10; f++) { frame(0, 0); folge[f] = (int)g->anim_frac; }
    printf("  +0x8f ueber 10 Bilder CHASE:");
    for (int f = 0; f < 10; f++) printf(" %d", folge[f]);
    printf("  (Clip %d)\n", (int)g->motion);
    PRUEF(g->motion == 5, "CHASE laeuft mit Clip 5 (sichtig, @0x80117cdc-e4)");
    PRUEF(folge[0] == 6 && folge[1] == 5 && folge[5] == 1 && folge[6] == 0 && folge[9] == 0,
          "+0x8f: 7 am Clip-Setzer, je anim_set -1 (6,5,4,3,2,1,0), dann 0 — Original-Savestates r3: Clip 5 Bild 2 -> 5, Bild 4 -> 3, Bild 5 -> 2");
}

/* ---------------------------------------------------------------------------------------------- */
/* Punkt 5: Pin-Latch setzt den Anker (FUN_8001ac38 @0x8011ac18) — Leon bleibt beim Gorilla. */
static void teil_anker(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -3180, -15263, 2048, 5, 3) != 0) return;
    re15_actor_t *g = aktor_vom_typ(0x27, 0), *g2 = aktor_vom_typ(0x27, 1);
    PRUEF(g != NULL, "Gorilla 1 vorhanden");
    if (!g) return;
    if (g2) { g2->grid_id |= 0x20; g2->x = 30000; g2->z = 30000; }
    /* Lage wie im exe-Lauf C1 (F446): Gorilla (-5221,-14988) r87, Leon (-3176,-15263) r2048, Pin verbunden */
    pl->x = -3176; pl->z = -15263; pl->rot_y = 2048; pl->hp = 64; pl->hit_react = 0;
    pl->anchor_x = 0; pl->anchor_z = 0;
    g->x = -5221; g->z = -14988; g->y = 1600; g->rot_y = 87; g->grid_id = 0x10;
    g->state = 1; g->sub_state_1 = 15; g->sub_state_2 = 2; g->sub_state_3 = 0;      /* PIN-Latch @0x8011abe8 */
    g->motion = 0x1c; g->anim_frame = 4; g->anim_frac = 0; g->hit_react = 1;
    frame(0, 0);
    printf("  nach dem Latch: Gorilla (%d,%d) Anker (%d,%d) | Leon (%d,%d) Anker (%d,%d)\n",
           (int)g->x, (int)g->z, (int)g->anchor_x, (int)g->anchor_z, (int)pl->x, (int)pl->z, (int)pl->anchor_x, (int)pl->anchor_z);
    PRUEF(pl->anchor_x == g->anchor_x && pl->anchor_z == g->anchor_z,
          "Spieler-Anker = Gorilla-Anker (Kopie `sh v0,160(s2)` @0x8001ad30 / `sh v0,162(s2)` @0x8001ad48)");
    PRUEF(dist2d(g->anchor_x, g->anchor_z, -5221, -14988) < 4000.0 && !(pl->anchor_x == 0 && pl->anchor_z == 0),
          "Anker liegt am Gorilla (Abstand %.0f), nicht mehr am Raumursprung (0,0)", dist2d(g->anchor_x, g->anchor_z, -5221, -14988));
    double dmax = 0.0; int am_ursprung = 0;
    for (int f = 0; f < 140; f++) {
        frame(0, 0);
        double d = dist2d(pl->x, pl->z, -5221, -14988); if (d > dmax) dmax = d;
        if (dist2d(pl->x, pl->z, 0, 0) < 600.0 || dist2d(pl->x, pl->z, -4330, 387) < 600.0) am_ursprung++;
    }
    PRUEF(am_ursprung == 0, "Leon steht in 140 Bildern nie am Raumursprung / bei (-4330,387) (vorher: ab dem 1. Pin-Bild, exe-Lauf C1 F447/F480)");
    PRUEF(dmax < 6500.0, "Leon bleibt waehrend Griff und Wurf beim Gorilla (groesster Abstand %.0f; vorher 15400)", dmax);
}

/* EM027-Bank wie die exe (Muster probe_1010_kriecher.c load_bank_re15): ohne Bank laeuft keine Fuss-Sperre
 * (re15_maggot_footlock) und keine Knochen-Trefferprobe (bff8) — die Gorillas stuenden still. */
static uint8_t s_blob27[0x80000];
static int bank_laden_27(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x27);
    if (eb && eb->ok) return 1;
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x27);
    size_t off = 0, len = 0;
    int ok = (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob27);
    if (ok) { memcpy(s_blob27, ems + off, len); if (!eb) eb = re15_enemy_alloc(0x27); ok = (eb != NULL); }
    if (ok) {
        re15_tim_t tim = (re15_tim_t){0};
        ok = (re15_emd_parse_container(s_blob27, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0);
        if (ok) { eb->ok = 1; eb->buf = NULL;
                  eb->victim_ok = (re15_emd_parse_victim_bank(s_blob27, len, &eb->skel_victim, &eb->anim_victim) == 0); }   /* wie main.c:1270 */
        else eb->type = 0;
    }
    free(ems);
    return ok;
}

/* ---------------------------------------------------------------------------------------------- */
/* M1 (Nachbesserung 1): Biss-Takt gegen die EINZELBILD-Spur des Originals (DuckStation-GDB-Server, Haltepunkt
 * Gorilla-Wurzel 0x80116db8, r3-Savestate s033 direkt geladen, Leon ohne Eingabe; scratch jnb1/g_orig.txt).
 * Startlage = Original-Bild F195 (VSync-Zaehler 0x800787dc, (vs-9641)/2): Leon (-6729,-12800) r647 hp82 frei
 * (1/0/1, +0x93 = 0); e1 (-5525,-14883) r2731 CHASE Clip 5 Bild 10 +0x1dc 9; e2 (-8915,-12487) r93 CHASE Clip 5
 * Bild 16 +0x1dc 15; beide grid 0x10, +0x82 = 0, +0x1d0 = 1 (LOS), +0x1e2 = 4.
 * Original ab F195: Commit e1 F205, e2 F211 (beide Biss, Clip 0x12), Treffer e1 F218 (+0x1dc 45), Flinch Clip 8
 * bis F240, Spieler frei F241; e2 Exit F235 -> +0x1dc 0x14; Commit e2 F256, e1 F264; Treffer e2 F268, ...
 * Treffer-Bilder 218/268/321/371/... (Abstaende 50/53/50/...). */
static void takt_lauf(int desync, const int *soll, int nsoll)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -6729, -12800, 647, 5, 3) != 0) return;
    PRUEF(bank_laden_27(), "EM027-Bank geladen (Fuss-Sperre + Knochenprobe)");
    re15_actor_t *a = aktor_vom_typ(0x27, 0), *b = aktor_vom_typ(0x27, 1);
    PRUEF(a != NULL && b != NULL, "zwei Gorillas im Kampf-Layout");
    if (!a || !b) return;
    pl->x = -6729; pl->z = -12800; pl->rot_y = 647; pl->hp = 82; pl->hit_react = 0; pl->state = 1; pl->sub_state_1 = 0;
    re15_player_cmd_zero();   /* HP-Abfall-Detektor ohne Baseline (s_prev_hp = -1): 100 -> 82 ist KEIN Treffer */
    a->x = -5525; a->z = -14883; a->y = 0; a->rot_y = 2731; a->grid_id = 0x10; a->floor = 0;
    a->state = 1; a->sub_state_1 = 3; a->sub_state_2 = 1; a->sub_state_3 = 0; a->motion = 5; a->anim_frame = 10;
    a->anim_frac = 0; a->dog_blocked_ctr = 9; a->hit_react = 0; a->dog_flags = 1; a->mag_boost = 4;
    b->x = -8915; b->z = -12487; b->y = 0; b->rot_y = 93; b->grid_id = 0x10; b->floor = 0;
    b->state = 1; b->sub_state_1 = 3; b->sub_state_2 = 1; b->sub_state_3 = 0; b->motion = 5; b->anim_frame = 16;
    b->anim_frac = 0; b->dog_blocked_ctr = 15; b->hit_react = 0; b->dog_flags = 1; b->mag_boost = 4;
    int hp_alt = pl->hp, treffer[64], nt = 0;
    char alt[256] = "";
    for (int f = 196; f < 196 + 420; f++) {
        if (desync && f == 203) b->dog_blocked_ctr = 30;   /* = GDB-Schreiben M800ad1f0,2:1e00 im Original */
        frame(0, 0);
        int ev = (pl->hp != hp_alt);
        if (ev && nt < 64) treffer[nt++] = f;
        char z[256];
        snprintf(z, sizeof z, "hp%d pl %d/%d c%d h%d | e1 %d/%d/%d c%d L%d | e2 %d/%d/%d c%d L%d",
                 (int)pl->hp, pl->state, pl->sub_state_1, (int)pl->motion, pl->hit_react ? 1 : 0,
                 a->state, a->sub_state_1, a->sub_state_2, (int)a->motion, a->dog_blocked_ctr ? 1 : 0,
                 b->state, b->sub_state_1, b->sub_state_2, (int)b->motion, b->dog_blocked_ctr ? 1 : 0);
        if (strcmp(z, alt) != 0)
            printf("    F%d hp%d pl %d/%d c%d/%d h%d (%d,%d) | e1 %d/%d/%d c%d/%d L%d d%.0f | e2 %d/%d/%d c%d/%d L%d d%.0f%s\n",
                   f, (int)pl->hp, pl->state, pl->sub_state_1, (int)pl->motion, (int)pl->anim_frame, pl->hit_react,
                   (int)pl->x, (int)pl->z,
                   a->state, a->sub_state_1, a->sub_state_2, (int)a->motion, (int)a->anim_frame, (int)a->dog_blocked_ctr,
                   dist2d(a->x, a->z, pl->x, pl->z),
                   b->state, b->sub_state_1, b->sub_state_2, (int)b->motion, (int)b->anim_frame, (int)b->dog_blocked_ctr,
                   dist2d(b->x, b->z, pl->x, pl->z), ev ? "  <-- TREFFER" : "");
        snprintf(alt, sizeof alt, "%s", z);
        hp_alt = pl->hp;
        if (pl->hp < 0) break;
    }
    printf("  Treffer-Bilder:");
    for (int i = 0; i < nt; i++) printf(" %d", treffer[i]);
    printf("\n  Treffer-Abstaende:");
    for (int i = 1; i < nt; i++) printf(" %d", treffer[i] - treffer[i - 1]);
    printf("  (%d Treffer)\n", nt);
    int abw = 0, fehlend = 0;
    for (int i = 0; i < nsoll; i++) {
        if (i >= nt) { fehlend++; continue; }
        int d = treffer[i] - soll[i]; if (d < 0) d = -d; if (d > abw) abw = d;
    }
    /* Schranke 6 Bilder kumuliert ueber 8 Bisse: das Original selbst streut je Biss um +-3 (Abstaende 49..55 bzw.
     * 35..36 — Treffer in Fenster-Bild 0x0c oder 0x0d je nach Knochenabstand, bff8 r=0x3e8 @0x801183c0-cc). */
    PRUEF(fehlend == 0 && abw <= 6, "%s: %d Bisse wie im Original (GDB-Einzelbild-Spur), groesste Abweichung %d Bilder (Schranke 6), fehlend %d",
          desync ? "Wechseltakt nach Desync (+0x1dc := 30 in F203)" : "Gleichtakt ab Original-Lage F195", nsoll, abw, fehlend);
    if (nt >= nsoll && nsoll >= 2) {
        double mitte = (double)(treffer[nsoll - 1] - treffer[0]) / (double)(nsoll - 1);
        double soll_m = (double)(soll[nsoll - 1] - soll[0]) / (double)(nsoll - 1);
        PRUEF(mitte >= soll_m - 2.0 && mitte <= soll_m + 2.0, "mittlerer Biss-Abstand %.1f Bilder (Original %.1f, +-2)", mitte, soll_m);
    }
}

static void teil_takt(void)
{
    /* Original (VSync-Bild, Treffer im Bild davor): Gleichtakt 218/268/321/371/424/475/527/579; nach dem
     * Desync 218/254/290/326/362/397/433/469 (36er-Wechseltakt, scratch jnb1/g_orig.txt / g_desync.txt). */
    static const int gleich[8]  = { 218, 268, 321, 371, 424, 475, 527, 579 };
    static const int wechsel[8] = { 218, 254, 290, 326, 362, 397, 433, 469 };
    takt_lauf(0, gleich, 8);
    takt_lauf(1, wechsel, 8);
}


/* ---------------------------------------------------------------------------------------------- */
/* M1-Nachtrag (Nachbesserung 1): verbundener Rear-up-Griff gegen das Original. Original-Experiment per GDB:
 * r3 s033 direkt geladen, in F250 e1 +0x5/+0x6/+0x7 := 15/0/0 (`M800ace25,3:0f0000`), Leon ohne Eingabe
 * (scratch jnb1/g_griff.txt). Original: Pin in F254 (Clip 0x1c Bild 4), Spieler-Zustand 5 ab F255 (Yaw := Gorilla
 * 2823), Leon bleibt F255-F265 stehen (Front-Intro ohne 0x8001ad68), ab F266 Wurf-Platzierung am Gorilla-Anker,
 * KEIN HP-Verlust (76 -> 76; in B[15] 0x8011a878-af40 und im Opfer-Handler 0x8011c118-c598 gibt es keinen
 * Schreiber auf Spieler+0x9a), Leon frei (Zustand 1) in F381 bei (-4645,-10726). */
static void teil_griff(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -6729, -12800, 647, 5, 3) != 0) return;
    PRUEF(bank_laden_27(), "EM027-Bank geladen");
    re15_actor_t *a = aktor_vom_typ(0x27, 0), *b = aktor_vom_typ(0x27, 1);
    if (!a || !b) { PRUEF(0, "zwei Gorillas"); return; }
    pl->x = -6729; pl->z = -12800; pl->rot_y = 647; pl->hp = 82; pl->hit_react = 0; pl->state = 1; pl->sub_state_1 = 0;
    re15_player_cmd_zero();
    a->x = -5525; a->z = -14883; a->y = 0; a->rot_y = 2731; a->grid_id = 0x10; a->floor = 0;
    a->state = 1; a->sub_state_1 = 3; a->sub_state_2 = 1; a->sub_state_3 = 0; a->motion = 5; a->anim_frame = 10;
    a->anim_frac = 0; a->dog_blocked_ctr = 9; a->hit_react = 0; a->dog_flags = 1; a->mag_boost = 4;
    b->x = -8915; b->z = -12487; b->y = 0; b->rot_y = 93; b->grid_id = 0x10; b->floor = 0;
    b->state = 1; b->sub_state_1 = 3; b->sub_state_2 = 1; b->sub_state_3 = 0; b->motion = 5; b->anim_frame = 16;
    b->anim_frac = 0; b->dog_blocked_ctr = 15; b->hit_react = 0; b->dog_flags = 1; b->mag_boost = 4;
    int f_pin = -1, f_sprung = -1, f_frei = -1, hp_griff = -1; int32_t px0 = 0, pz0 = 0;
    for (int f = 196; f < 196 + 300; f++) {
        if (f == 250) { a->sub_state_1 = 15; a->sub_state_2 = 0; a->sub_state_3 = 0; }
        int32_t ox = pl->x, oz = pl->z;
        frame(0, 0);
        if (f >= 249 && (f < 300 || (f % 6) == 0))
            printf("    F%d hp%d pl %d/%d c%d/%d h%d (%d,%d) r%d | e1 %d/%d/%d c%d/%d (%d,%d) r%d\n", f, (int)pl->hp, pl->state,
                   pl->sub_state_1, (int)pl->motion, (int)pl->anim_frame, pl->hit_react, (int)pl->x, (int)pl->z, (int)pl->rot_y,
                   a->state, a->sub_state_1, a->sub_state_2, (int)a->motion, (int)a->anim_frame, (int)a->x, (int)a->z, (int)a->rot_y);
        if (f_pin < 0 && a->sub_state_1 == 15 && a->sub_state_2 >= 3) { f_pin = f; px0 = pl->x; pz0 = pl->z; hp_griff = pl->hp; }
        if (f_pin >= 0 && f_sprung < 0 && dist2d(pl->x, pl->z, ox, oz) > 600.0) f_sprung = f;
        if (f_pin >= 0 && f_frei < 0 && f > f_pin + 5 && !re15_player_is_grabbed() && pl->state == 1) f_frei = f;
    }
    printf("  Pin F%d bei (%d,%d) hp %d, erster Platzierungs-Sprung F%d, frei F%d bei (%d,%d) hp %d\n",
           f_pin, (int)px0, (int)pz0, hp_griff, f_sprung, f_frei, (int)pl->x, (int)pl->z, (int)pl->hp);
    PRUEF(f_pin >= 253 && f_pin <= 255, "Pin-Latch in F%d (Original F254: Clip 0x1c Bild 4, a780 = 0 -> Front-Griff @0x8011aa4c-bc)", f_pin);
    PRUEF(dist2d(px0, pz0, -6658, -12487) < 60.0, "beim Zupacken bleibt Leon stehen (%d,%d) (Original F255: (-6660,-12486)) — vorher sprang er hier ~1200-1600", (int)px0, (int)pz0);
    PRUEF(f_sprung >= 265 && f_sprung <= 267, "erste Wurf-Platzierung in F%d = Bild 0x0c des Opfer-Clips (Original F266; Front-Intro 0..0xb ohne 0x8001ad68,"
          " aca59 = a780 VOR dem Yaw-Latch @0x8011ac50-68 / @0x8011acac)", f_sprung);
    PRUEF(hp_griff == 76, "der Griff kostet keine HP (%d; Original 76 -> 76, kein Schreiber auf Spieler+0x9a in 0x8011a878-af40 / 0x8011c118-c598)", hp_griff);
}

/* ---------------------------------------------------------------------------------------------- */
/* M4 (Nachbesserung 1): die NPC-Wandklemme nimmt seit Runde 35 das Band aus +0x82 statt aus y. Fuer jeden NPC, dessen
 * +0x82 gleich band_from_y(y) ist, ist das Ergebnis identisch (re15_collision_constrain_enemy ruft dieselbe
 * constrain_contact_band mit band_from_y(y), re15_collision.c). Gemessen wird je Raum ueber 600 Bilder, ob ein NPC
 * (0x40/42/45/47/49/4b/4d) mit abweichendem Band LAEUFT (Lage aendert sich) — nur dann kann die Umstellung wirken. */
static int ist_npc(uint8_t t) { return t == 0x40 || t == 0x42 || t == 0x45 || t == 0x47 || t == 0x49 || t == 0x4b || t == 0x4d; }
static void npcband_raum(uint16_t room, int32_t px, int32_t pz, int bilder, int *n_npc, int *n_abw, int *n_abw_lauf)
{
    *n_npc = *n_abw = *n_abw_lauf = 0;
    re15_game_state_init();
    if (room_boot(room, px, pz, 0, 0, 1) != 0) return;
    int32_t ax[RE15_ACTOR_MAX], az[RE15_ACTOR_MAX];
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++) { ax[sl] = g_actors[sl].x; az[sl] = g_actors[sl].z; }
    for (int f = 0; f < bilder; f++) {
        frame(0, 0);
        for (int sl = 1; sl < RE15_ACTOR_MAX; sl++) {
            re15_actor_t *e = &g_actors[sl];
            if (!e->active || !ist_npc(e->type)) continue;
            if (f == 0) (*n_npc)++;
            int bandy = re15_collision_band_from_y(e->y);
            if ((int)e->floor != bandy) {
                (*n_abw)++;
                if (e->x != ax[sl] || e->z != az[sl]) {
                    if (*n_abw_lauf < 3) printf("    ROOM%04X Bild %d Slot %d Typ 0x%02x +0x82=%d band_from_y(%d)=%d @(%d,%d)%s", room, f, sl,
                                                e->type, e->floor, (int)e->y, bandy, (int)e->x, (int)e->z, "\n");
                    (*n_abw_lauf)++;
                }
            }
            ax[sl] = e->x; az[sl] = e->z;
        }
    }
}
static void teil_npcband(void)
{
    static const struct { uint16_t room; int32_t x, z; } r[] = {
        {0x10D0, -21500, -3300}, {0x1050, -1800, -1800}, {0x1090, -25000, 2000}, {0x10B1, -1000, -1000},
        {0x1170, -17000, -5000}, {0x11B0, -22604, 14455}, {0x4001, -4000, -3000}, {0x4031, 0, 0},
        {0x6030, -21000, -24000}, {0x6031, -21000, -24000} };
    for (unsigned i = 0; i < sizeof r / sizeof r[0]; i++) {
        int n_npc, n_abw, n_lauf;
        npcband_raum(r[i].room, r[i].x, r[i].z, 600, &n_npc, &n_abw, &n_lauf);
        printf("  ROOM%04X: %d NPC, %d NPC-Bilder mit +0x82 != band_from_y(y), davon %d mit Bewegung\n", r[i].room, n_npc, n_abw, n_lauf);
        if (r[i].room == 0x10D0 || r[i].room == 0x1050)
            PRUEF(n_lauf == 0, "ROOM%04X: kein laufender NPC mit abweichendem Band -> die +0x82-Klemme wirkt hier identisch zur alten y-Klemme", r[i].room);
    }
}

/* ---------------------------------------------------------------------------------------------- */
/* M3-Nachtrag (Nachbesserung 1): Schrotflinten-Treffer (Spur 1, Zeile 7) als 3. Treffer -> Vergeltungssprung und was
 * danach kommt, gegen das Original. Original-Experiment per GDB: r3 s033 direkt geladen, in F250 e1 +0x4..+0x7 :=
 * 2/7/1/0 (`M800ace24,4:02070100`, scratch jnb1/g_schrot.txt): Spur-1-Exit F276 -> sub 7, Anlauf F277-F286, Flug
 * F286-F312, Landung F312 bei (-10040,-8792) (5361 von Leon), F316 SELECTOR -> Clip 6 Heavy-Anlauf bis F444, dann
 * CHASE — KEIN Zonen-Sprung. Port: mag_hit_ctr = 2, damit dieser Treffer der dritte ist (NUTZER-VORGABE). */
static void teil_schrot(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -6729, -12800, 647, 5, 3) != 0) return;
    PRUEF(bank_laden_27(), "EM027-Bank geladen");
    re15_actor_t *a = aktor_vom_typ(0x27, 0), *b = aktor_vom_typ(0x27, 1);
    if (!a || !b) { PRUEF(0, "zwei Gorillas"); return; }
    pl->x = -6729; pl->z = -12800; pl->rot_y = 647; pl->hp = 82; pl->hit_react = 0; pl->state = 1; pl->sub_state_1 = 0;
    re15_player_cmd_zero();
    a->x = -5525; a->z = -14883; a->y = 0; a->rot_y = 2731; a->grid_id = 0x10; a->floor = 0;
    a->state = 1; a->sub_state_1 = 3; a->sub_state_2 = 1; a->sub_state_3 = 0; a->motion = 5; a->anim_frame = 10;
    a->anim_frac = 0; a->dog_blocked_ctr = 9; a->hit_react = 0; a->dog_flags = 1; a->mag_boost = 4;
    b->x = -8915; b->z = -12487; b->y = 0; b->rot_y = 93; b->grid_id = 0x10; b->floor = 0;
    b->state = 1; b->sub_state_1 = 3; b->sub_state_2 = 1; b->sub_state_3 = 0; b->motion = 5; b->anim_frame = 16;
    b->anim_frac = 0; b->dog_blocked_ctr = 15; b->hit_react = 0; b->dog_flags = 1; b->mag_boost = 4;
    int f_exit = -1, exit_sub = -1, f_land = -1, f_sel = -1, zonen = 0; int32_t lx = 0, lz = 0;
    int alt = -1;
    for (int f = 196; f < 196 + 300; f++) {
        if (f == 250) { a->state = 2; a->sub_state_1 = 7; a->sub_state_2 = 1; a->sub_state_3 = 0; a->mag_hit_ctr = 2; }
        frame(0, 0);
        int k = a->state * 10000 + a->sub_state_1 * 100 + a->sub_state_2 * 10 + a->sub_state_3;
        if (f >= 250 && k != alt)
            printf("    F%d e1 %d/%d/%d/%d c%d/%d (%d,%d) d%.0f%s", f, a->state, a->sub_state_1, a->sub_state_2, a->sub_state_3,
                   (int)a->motion, (int)a->anim_frame, (int)a->x, (int)a->z, dist2d(a->x, a->z, pl->x, pl->z), "\n");
        alt = k;
        if (f > 250 && f_exit < 0 && a->state == 1) { f_exit = f; exit_sub = a->sub_state_1; }
        if (f_exit >= 0 && f_land < 0 && a->sub_state_1 == 7 && a->sub_state_2 == 3) { f_land = f; lx = a->x; lz = a->z; }
        if (f_land >= 0 && f_sel < 0 && a->sub_state_1 == 4) f_sel = f;
        if (f_sel >= 0 && a->sub_state_1 == 7 && a->sub_state_3 != 0) zonen++;
    }
    PRUEF(exit_sub == 7 && f_exit >= 274 && f_exit <= 278, "3. Treffer Spur 1: Exit F%d -> Sub %d (Original F276 -> 7 @0x8011b3c8-cc)", f_exit, exit_sub);
    PRUEF(f_land >= 309 && f_land <= 315, "Landung F%d bei (%d,%d) (Original F312 bei (-10040,-8792))", f_land, (int)lx, (int)lz);
    PRUEF(dist2d(lx, lz, -10040, -8792) < 1500.0, "Landepunkt %.0f vom Original entfernt", dist2d(lx, lz, -10040, -8792));
    PRUEF(f_sel >= 0 && zonen == 0, "danach SELECTOR (F%d) und KEIN Zonen-Sprung (Original: Clip-6-Anlauf bis F444, dann CHASE); Zonen-Sprung-Bilder %d", f_sel, zonen);
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
    if (!strcmp(teil, "brust")  || !strcmp(teil, "alle")) teil_brust();
    if (!strcmp(teil, "kdsonde")|| !strcmp(teil, "alle")) teil_kdsonde();
    if (!strcmp(teil, "biss")   || !strcmp(teil, "alle")) teil_biss();
    if (!strcmp(teil, "frac")   || !strcmp(teil, "alle")) teil_frac();
    if (!strcmp(teil, "anker")  || !strcmp(teil, "alle")) teil_anker();
    if (!strcmp(teil, "takt")   || !strcmp(teil, "alle")) teil_takt();
    if (!strcmp(teil, "griff")  || !strcmp(teil, "alle")) teil_griff();
    if (!strcmp(teil, "npcband")|| !strcmp(teil, "alle")) teil_npcband();
    if (!strcmp(teil, "schrot") || !strcmp(teil, "alle")) teil_schrot();
    printf("test_r35_affen %s: %s (%d Fehler)\n", teil, g_fail ? "FEHLER" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
