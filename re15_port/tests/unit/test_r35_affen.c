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
 *           Original): Pin F254, Leon steht bis Bild 0xb, erste Wurf-Platzierung F266, 0 HP. Nachbesserung 3: Ritt des
 *           Greifers + Schub auf e2 (M2), Klemmen-Iteration ab T290 / Empfindlichkeit / Zerlegung (M1), Lauf "Weg 2" mit
 *           Original-Startzustand T253: Anker, Kette T268-T290, Freigabe, Bahn T291-T414 und Ruhe bitgleich zum Original.
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
#include "re15_anim_select.h"

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
    /* Nachbesserung 4: Pool + Zeichenstand des Original-Bildes F195 vorbelegen (das letzte anim_set vor F195 posierte
     * Bild 9 bzw. 15, re15_affen.h (12)) — sonst verliert der erste Tick die Fuss-Sperre (Harness-Artefakt). */
    a->anim_frame = 9;  re15_affen_pool_anim(a); a->anim_frame = 10;
    b->anim_frame = 15; re15_affen_pool_anim(b); b->anim_frame = 16;
    int hp_alt = pl->hp, treffer[64], nt = 0;
    char alt[256] = "";
    const char *spur = getenv("R35_TAKT_SPUR");   /* Nachbesserung 4: Bild-fuer-Bild-Spur im Format von jnb1/g_orig_dec.txt */
    int f_ende = 196 + 720;   /* N4: alle 14 Bisse bis zum Tod (Original-Zeile F892) */
    for (int f = 196; f < f_ende; f++) {
        if (desync && f == 203) b->dog_blocked_ctr = 30;   /* = GDB-Schreiben M800ad1f0,2:1e00 im Original */
        re15_schritt_station_reset();
        frame(0, 0);
        if (spur && !desync) {   /* N5: Spieler-Stationen wie die GDB-Haltepunkte 0x80031cbc / 0x80031cc4 / 0x80031d78 */
            int32_t st[4][2] = {{0}};
            for (int k = 0; k < 4; k++) re15_schritt_station_hole(k, &st[k][0], &st[k][1]);
            printf("P%4d r%d c%d/%d anf(%d,%d) tick(%d,%d) schub(%d,%d) klemme(%d,%d)\n", f, (int)pl->rot_y, (int)pl->motion,
                   (int)pl->anim_frame, (int)st[0][0], (int)st[0][1], (int)st[1][0],
                   (int)st[1][1], (int)st[2][0], (int)st[2][1], (int)st[3][0], (int)st[3][1]);
        }
        if (spur && !desync)
            printf("S%4d hp%d %d/%d c%d/%d (%d,%d) | e1 %d/%d/%d c%d/%d L%d (%d,%d) r%d d%.0f | e2 %d/%d/%d c%d/%d L%d (%d,%d) r%d d%.0f\n",
                   f, (int)pl->hp, pl->state, pl->sub_state_1, (int)pl->motion, (int)pl->anim_frame, (int)pl->x, (int)pl->z,
                   a->state, a->sub_state_1, a->sub_state_2, (int)a->motion, (int)a->anim_frame, (int)a->dog_blocked_ctr,
                   (int)a->x, (int)a->z, (int)a->rot_y, dist2d(a->x, a->z, pl->x, pl->z),
                   b->state, b->sub_state_1, b->sub_state_2, (int)b->motion, (int)b->anim_frame, (int)b->dog_blocked_ctr,
                   (int)b->x, (int)b->z, (int)b->rot_y, dist2d(b->x, b->z, pl->x, pl->z));
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
    PRUEF(fehlend == 0 && abw <= (desync ? 3 : 6), "%s: %d Bisse wie im Original (GDB-Einzelbild-Spur), groesste Abweichung %d Bilder (Schranke %d), fehlend %d",
          desync ? "Wechseltakt nach Desync (+0x1dc := 30 in F203)" : "Gleichtakt ab Original-Lage F195", nsoll, abw, desync ? 3 : 6, fehlend);
    if (!desync) {   /* N4 (N2): e1 = Slot 2 beisst die Treffer 1, 3, ... 13 — jeder Treffer im SELBEN Bild wie das Original */
        int e1_gleich = 0, e1_n = 0;
        for (int i = 0; i < nsoll && i < nt; i += 2) { e1_n++; if (treffer[i] == soll[i]) e1_gleich++; }
        PRUEF(e1_n == 7 && e1_gleich == 7, "Slot 2 (e1): %d/%d Bisse im selben Bild wie das Original (Zyklus 103; HEAD 458635e1: 218/321/425/529/633/737/841"
              " = Zyklus 104 nach (9), weil A/B-Folge @0x80117358-78 und Trefferpunkt Record+0x40 @0x801183c0 fehlten)", e1_gleich, e1_n);
        int e2_gleich = 0, e2_n = 0;   /* N5 (P1): e2 = Slot 3 beisst die Treffer 2, 4, ... 14 */
        for (int i = 1; i < nsoll && i < nt; i += 2) { e2_n++; if (treffer[i] == soll[i]) e2_gleich++; }
        PRUEF(e2_n == 7 && e2_gleich == 7, "Slot 3 (e2): %d/%d Bisse im selben Bild wie das Original (Zyklen 103, 104, 104, 104, 104, 104;"
              " Nachbesserung 4: 269/372/475/578/681/784/887 = Zyklus 103, weil die Fuss-Sperre die Kette nicht auf der GTE rechnete und"
              " Leons Rueckstoss im Handler klemmte — re15_affen.h (13), game_step_common.c (14))", e2_gleich, e2_n);
    }
    if (nt >= nsoll && nsoll >= 2) {
        double mitte = (double)(treffer[nsoll - 1] - treffer[0]) / (double)(nsoll - 1);
        double soll_m = (double)(soll[nsoll - 1] - soll[0]) / (double)(nsoll - 1);
        PRUEF(mitte >= soll_m - 2.0 && mitte <= soll_m + 2.0, "mittlerer Biss-Abstand %.1f Bilder (Original %.1f, +-2)", mitte, soll_m);
    }
}

static void teil_takt(void)
{
    /* Original (GDB-Zeile mit dem HP-Wechsel = Port-Bild des Treffers; N4: die Zeile F zeigt den Zustand beim Eintritt in die
     * Gorilla-Wurzel von Bild F, Port-Bild f = Zeile f — gemessen an den Lagen, Startzeile F195 = Port-Startzustand):
     * Gleichtakt jnb1/g_orig_dec.txt, Wechseltakt jnb1/g_desync.txt (Liste bisher Treffer-Tick = Zeile - 1, jetzt Zeile). */
    static const int gleich[14] = { 219, 269, 322, 372, 425, 476, 528, 580, 631, 684, 734, 788, 837, 892 };
    static const int wechsel[8] = { 219, 255, 291, 327, 363, 398, 434, 470 };
    takt_lauf(0, gleich, 14);
    takt_lauf(1, wechsel, 8);
}


/* ---------------------------------------------------------------------------------------------- */
/* A2 (Nachbesserung 2): die WURF-BAHN bis zur Freigabe gegen die Original-GDB-Spur (scratch jnb2/g_wer.txt, gleiches
 * Experiment): Lage am ENDE jedes Spieler-Ticks (Haltepunkt 0x80031d80 nach `jal 0x80037358`), Tick T = (VSync-9641)/2,
 * dazu aca58 / +0x94 / +0x95. Ab T254 (Pin). Platzierung je Bild (0x8001ad68 @0x8011c294, Fenster [0x0b,0x25)
 * @0x8011c244-5c/@0x8011c278), danach Koerper-Schub @0x80031cbc und Wandklemme @0x80031d70; P3/P4 Clip 0x10, P5/P6
 * Clip 0xb RUECKWAERTS aus PL00 (@0x8011c348/@0x8011c350-58), Freigabe T378 (aca58 = 1 @0x8011c384-8c). */
static const int32_t s_wurf_orig[][5] = {   /* x, z, aca58, +0x94, +0x95 (nach dem Tick) */
    {-6660,-12486,5,1,1}, {-6659,-12487,5,1,2}, {-6659,-12487,5,1,3}, {-6659,-12487,5,1,4},  /* T254 */
    {-6659,-12487,5,1,5}, {-6659,-12487,5,1,6}, {-6659,-12487,5,1,7}, {-6659,-12487,5,1,8},  /* T258 */
    {-6659,-12487,5,1,9}, {-6659,-12487,5,1,10}, {-6659,-12487,5,1,11}, {-7804,-10186,5,1,12},  /* T262 */
    {-6686,-10348,5,1,13}, {-7340,-11870,5,1,14}, {-7492,-11733,5,1,15}, {-7110,-12078,5,1,16},  /* T266 */
    {-7110,-12078,5,1,17}, {-6799,-12508,5,1,18}, {-6799,-12508,5,1,19}, {-6542,-12913,5,1,20},  /* T270 */
    {-6542,-12913,5,1,21}, {-6210,-13216,5,1,22}, {-6210,-13216,5,1,23}, {-5804,-13474,5,1,24},  /* T274 */
    {-5804,-13474,5,1,25}, {-5374,-13651,5,1,26}, {-5374,-13651,5,1,27}, {-4969,-13711,5,1,28},  /* T278 */
    {-4969,-13711,5,1,29}, {-4639,-13614,5,1,30}, {-4639,-13614,5,1,31}, {-4353,-13122,5,1,32},  /* T282 */
    {-4353,-13122,5,1,33}, {-3964,-12506,5,1,34}, {-3964,-12506,5,1,35}, {-4588,-11243,5,1,36},  /* T286 */
    {-5381,-10551,5,1,37}, {-5302,-10633,5,1,38}, {-5207,-10699,5,1,39}, {-5098,-10737,5,1,40},  /* T290 */
    {-5190,-10791,5,1,41}, {-5237,-10891,5,1,42}, {-5122,-10899,5,1,43}, {-5170,-10989,5,1,44},  /* T294 */
    {-5291,-11003,5,1,45}, {-5268,-10904,5,1,46}, {-5381,-10936,5,1,47}, {-5408,-10842,5,1,48},  /* T298 */
    {-5286,-10864,5,1,49}, {-5312,-10747,5,1,50}, {-5239,-10828,5,1,51}, {-5276,-10938,5,1,52},  /* T302 */
    {-5178,-10927,5,1,53}, {-5193,-11051,5,1,54}, {-5089,-10997,5,1,55}, {-5042,-10875,5,1,56},  /* T306 */
    {-5166,-10886,5,1,57}, {-5137,-10794,5,1,58}, {-5014,-10811,5,1,59}, {-5078,-10885,5,1,60},  /* T310 */
    {-5166,-10933,5,1,61}, {-5207,-11030,5,1,62}, {-5341,-11031,5,1,63}, {-5280,-11108,5,1,64},  /* T314 */
    {-5145,-11092,5,1,65}, {-5163,-11203,5,1,66}, {-5329,-11163,5,1,67}, {-5375,-11039,5,1,68},  /* T318 */
    {-5255,-11055,5,1,69}, {-5359,-11087,5,1,70}, {-5475,-11047,5,1,71}, {-5497,-10895,5,1,72},  /* T322 */
    {-5429,-10964,5,1,73}, {-5350,-11021,5,1,74}, {-5292,-11101,5,1,75}, {-5416,-11112,5,1,76},  /* T326 */
    {-5420,-11247,5,1,77}, {-5553,-11158,5,1,78}, {-5643,-11015,5,1,79}, {-5481,-10978,5,1,80},  /* T330 */
    {-5595,-10937,5,1,81}, {-5620,-10788,5,1,82}, {-5546,-10851,5,1,0}, {-5478,-10920,5,16,1},  /* T334 */
    {-5397,-10976,5,16,2}, {-5344,-11061,5,16,3}, {-5233,-11086,5,16,4}, {-5127,-11035,5,16,5},  /* T338 */
    {-5086,-10906,5,16,6}, {-5196,-10932,5,16,7}, {-5194,-10808,5,16,8}, {-5261,-10887,5,16,9},  /* T342 */
    {-5358,-10935,5,16,10}, {-5417,-10878,5,16,11}, {-5482,-10829,5,16,12}, {-5533,-10763,5,16,13},  /* T346 */
    {-5613,-10730,5,16,14}, {-5634,-10629,5,16,15}, {-5524,-10664,5,16,0}, {-5514,-10566,5,11,1},  /* T350 */
    {-5438,-10644,5,11,2}, {-5348,-10707,5,11,3}, {-5286,-10799,5,11,4}, {-5217,-10792,5,11,5},  /* T354 */
    {-5228,-10692,5,11,6}, {-5302,-10772,5,11,7}, {-5204,-10828,5,11,8}, {-5288,-10889,5,11,9},  /* T358 */
    {-5349,-10975,5,11,10}, {-5361,-10864,5,11,11}, {-5270,-10918,5,11,12}, {-5152,-10929,5,11,13},  /* T362 */
    {-5205,-11014,5,11,14}, {-5315,-11040,5,11,15}, {-5277,-11140,5,11,16}, {-5191,-11065,5,11,17},  /* T366 */
    {-5109,-10985,5,11,18}, {-5019,-10915,5,11,19}, {-4946,-10824,5,11,20}, {-4837,-10776,5,11,21},  /* T370 */
    {-4800,-10642,5,11,22}, {-4903,-10676,5,11,23}, {-4915,-10803,5,11,24}, {-4816,-10743,5,11,0},  /* T374 */
    {-4759,-10633,1,11,0}, {-4620,-10622,1,3,0}, {-4645,-10726,1,3,0}, {-4798,-10698,1,3,0},  /* T378 */
    {-4699,-10638,1,3,0}, {-4643,-10527,1,3,0}, {-4502,-10517,1,3,0}, {-4529,-10618,1,3,0},  /* T382 */
    {-4677,-10596,1,3,0}, {-4590,-10522,1,3,0}, {-4521,-10460,1,3,0}, {-4499,-10473,1,3,0},  /* T386 */
    {-4538,-10449,1,3,0}, {-4470,-10491,1,3,0}, {-4502,-10588,1,3,0}, {-4640,-10575,1,3,0},  /* T390 */
    {-4570,-10481,1,3,0}, {-4484,-10482,1,3,0}, {-4484,-10612,1,3,0}, {-4362,-10558,1,3,0},  /* T394 */
    {-4399,-10535,1,3,0}, {-4333,-10576,1,3,0}, {-4336,-10691,1,3,0}, {-4515,-10625,1,3,0},  /* T398 */
    {-4354,-10571,1,3,0}, {-4323,-10722,1,3,0}, {-4193,-10663,1,3,0}, {-4092,-10871,1,3,0},  /* T402 */
    {-3984,-10791,1,3,0}, {-3765,-11101,1,3,0}, {-3520,-11078,1,3,0}, {-3010,-11642,1,3,0},  /* T406 */
    {-3009,-11643,1,3,0}, {-3009,-11643,1,3,0}, {-3009,-11643,1,3,0}, {-3009,-11643,1,3,0},  /* T410 */
    {-3009,-11643,1,3,0},  /* T414 */
};
static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;
static int pl00_laden(void)
{
    static int ok = -1;
    if (ok >= 0) return ok;
    char p[600]; size_t esz = 0, rsz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", RE15_ASSET_PSX_DIR); uint8_t *edd = slurp(p, &esz);
    snprintf(p, sizeof p, "%s/PLD/PL00.EMR", RE15_ASSET_PSX_DIR); uint8_t *emr = slurp(p, &rsz);
    ok = (edd && emr && re15_emd_parse_animation(edd, esz, &s_pl00_anim) == 0 &&
          re15_emd_parse_skeleton(emr, rsz, &s_pl00_skel) == 0) ? 1 : 0;   /* Puffer bleiben (Zeiger in die Daten) */
    return ok;
}

/* Nachbesserung 3: Ergebnisse je Lauf fuer die Auswertung in teil_griff (hinter s_wand_orig). Lauf 0 = Riegel wie
 * bisher, Lauf 1 = "Weg 2": Leon, e1, e2 am Ende von T253 auf die Original-Lage/-Yaw gesetzt (jnb1/g_griff.txt F254). */
#define GRIFF_T0 250
#define GRIFF_N  246
static int32_t s_gl_pl[3][GRIFF_N][2], s_gl_e1[3][GRIFF_N][3], s_gl_e2[3][GRIFF_N][3];   /* N5: [2] = Lauf 0 mit Versatz bei T291 */
static int32_t s_gl_st[3][GRIFF_N][6];         /* Bezug, Eingang (nach Schub), Ausgang der Klemme je Bild (Wurf) */
static int32_t s_gl_anker[3][2];
static int     s_gl_frei[3];
static int32_t s_gl_frei_xy[3][2];
static int     s_gl_biss[3];          /* N5: erster Biss nach der Freigabe (-1 = keiner bis T495) */
static double  s_gl_e12[3][2];        /* N5: Abstand e1/e2 zu Leon am Laufende */
static void griff_lauf_v(int erzwinge, int vx, int vz);
static void griff_lauf(int erzwinge) { griff_lauf_v(erzwinge, 0, 0); }
/* N5 (P2): vx/vz != 0 -> Lauf 0, Leon vor Bild T291 um (vx,vz) versetzt (Start der Klemmen-Iteration), Ablage in [2]. */
static void griff_lauf_v(int erzwinge, int vx, int vz)
{
    const int L = (vx || vz) ? 2 : erzwinge;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -6729, -12800, 647, 5, 3) != 0) return;
    PRUEF(bank_laden_27(), "EM027-Bank geladen");
    PRUEF(pl00_laden(), "PL00.EDD/EMR geladen (COMMON-Bank der Aufsteh-Clips 0x10/0xb)");
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    re15_anim_banks_t banks; memset(&banks, 0, sizeof banks);
    banks.pl00_skel = &s_pl00_skel; banks.pl00_anim = &s_pl00_anim; banks.pl00_ok = 1;
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
    a->anim_frame = 9;  re15_affen_pool_anim(a); a->anim_frame = 10;   /* Nachbesserung 4: Pool/Zeichenstand F195 (wie takt) */
    b->anim_frame = 15; re15_affen_pool_anim(b); b->anim_frame = 16;
    const int T0 = 254, NT = (int)(sizeof s_wurf_orig / sizeof s_wurf_orig[0]);
    int f_pin = -1, f_sprung = -1, f_frei = -1, hp_griff = -1; int32_t px0 = 0, pz0 = 0, fx = 0, fz = 0;
    double abw_p2 = 0, abw_p2_mitte = 0, abw_e2 = 0, d265 = 0, d290 = 0; int n_p2 = 0, f_abw_p2 = -1, hp_frei = -1;
    int biss_nach_frei = -1; double e1_ende = 0, e2_ende = 0;   /* N4 (N1): Biss nach der Freigabe, Abstaende am Laufende */
    int n_nach = 0, n_fremd = 0, n_klemme = 0;   /* T291..Freigabe: Bilder / Bilder mit fremdem Beweger / Bilder mit Klemm-Schub */
    int p3_bilder = 0, p3_pl00 = 0, p5_bilder = 0, p5_rueck = 0;
    memset(s_gl_st[L], 0, sizeof s_gl_st[L]);
    for (int f = 196; f < 196 + 300; f++) {
        if (f == 250) { a->sub_state_1 = 15; a->sub_state_2 = 0; a->sub_state_3 = 0; }
        if (erzwinge && f == 254) {   /* Weg 2: Lage/Yaw am Ende von T253 = Original (jnb1/g_griff.txt F254) */
            pl->x = -6660; pl->z = -12486; pl->rot_y = 647;
            a->x = -5895; a->z = -14394; a->rot_y = 2825;
            b->x = -8709; b->z = -12400; b->rot_y = 27;
        }
        if (L == 2 && f == 291) { pl->x += vx; pl->z += vz; }   /* N5 (P2): Versatz vor dem ersten Klemmen-Bild */
        int32_t ox = pl->x, oz = pl->z;
        re15_schritt_station_reset();
        frame(0, 0);
        if (f >= GRIFF_T0 && f < GRIFF_T0 + GRIFF_N) {
            int i = f - GRIFF_T0;
            s_gl_pl[L][i][0] = pl->x; s_gl_pl[L][i][1] = pl->z;
            s_gl_e1[L][i][0] = a->x; s_gl_e1[L][i][1] = a->z; s_gl_e1[L][i][2] = a->rot_y;
            s_gl_e2[L][i][0] = b->x; s_gl_e2[L][i][1] = b->z; s_gl_e2[L][i][2] = b->rot_y;
            int32_t rx = 0, rz = 0, ix = 0, iz = 0, kx = 0, kz = 0;
            if (re15_schritt_station_hole(RE15_SCHRITT_ANFANG, &rx, &rz) && re15_schritt_station_hole(RE15_SCHRITT_SCHUB, &ix, &iz) &&
                re15_schritt_station_hole(RE15_SCHRITT_KLEMME, &kx, &kz)) {
                s_gl_st[L][i][0] = rx; s_gl_st[L][i][1] = rz; s_gl_st[L][i][2] = ix;
                s_gl_st[L][i][3] = iz; s_gl_st[L][i][4] = kx; s_gl_st[L][i][5] = kz;
            }
            if (f == 254) { s_gl_anker[L][0] = pl->anchor_x; s_gl_anker[L][1] = pl->anchor_z; }
        }
        int k = f - T0;
        const int32_t *o = (k >= 0 && k < NT) ? s_wurf_orig[k] : NULL;
        int zeigen = L != 2 && (f >= 249 && (f < 300 || (f % 6) == 0 || (f >= 370 && f <= 382) || (f >= 405 && f <= 412)));
        if (zeigen)
            printf("    T%d hp%d pl %d/%d c%d/%d%s h%d (%d,%d) r%d | e1 %d/%d/%d c%d/%d (%d,%d) r%d | e2 %d/%d c%d/%d (%d,%d) r%d\n", f, (int)pl->hp,
                   pl->state, pl->sub_state_1, (int)pl->motion, (int)pl->anim_frame, (pl->anim_flags & 0x80) ? "R" : "",
                   pl->hit_react, (int)pl->x, (int)pl->z, (int)pl->rot_y, a->state, a->sub_state_1, a->sub_state_2,
                   (int)a->motion, (int)a->anim_frame, (int)a->x, (int)a->z, (int)a->rot_y,
                   b->sub_state_1, b->sub_state_2, (int)b->motion, (int)b->anim_frame, (int)b->x, (int)b->z, (int)b->rot_y);
        if (o && zeigen)
            printf("         Original (%d,%d) cmd %d c%d/%d  d=%.0f\n", (int)o[0], (int)o[1], (int)o[2], (int)o[3], (int)o[4],
                   dist2d(pl->x, pl->z, o[0], o[1]));
        if (f_pin < 0 && a->sub_state_1 == 15 && a->sub_state_2 >= 3) { f_pin = f; px0 = pl->x; pz0 = pl->z; hp_griff = pl->hp; }
        if (f_pin >= 0 && f_sprung < 0 && dist2d(pl->x, pl->z, ox, oz) > 600.0) f_sprung = f;
        if (f_pin >= 0 && f_frei < 0 && f > f_pin + 5 && !re15_player_is_grabbed() && pl->state == 1) { f_frei = f; fx = pl->x; fz = pl->z; }
        if (o && f >= 265 && f <= 290) {            /* P2: Platzierung + Schub + Klemme je Bild */
            double d = dist2d(pl->x, pl->z, o[0], o[1]);
            abw_p2_mitte += d; n_p2++; if (f <= 289 && d > abw_p2) { abw_p2 = d; f_abw_p2 = f; }   /* N5: T265-T289 (T265 wieder drin), T290 = Klemmen-Start */
            if (f == 290) d290 = d;   /* N3: T265-T267 mit (Schub von e2); N4: T265 eigene Pruefung */
            if (f == 265) d265 = d;
            if (f >= 266 && f <= 267 && d > abw_e2) abw_e2 = d;
        }
        if (f_frei == f) hp_frei = pl->hp;
        if (f_frei >= 0 && f > f_frei && biss_nach_frei < 0 && pl->hp < hp_frei) biss_nach_frei = f;
        e1_ende = dist2d(a->x, a->z, pl->x, pl->z); e2_ende = dist2d(b->x, b->z, pl->x, pl->z);
        if (f >= 291 && f_frei < 0 && re15_player_victim_gorilla()) {   /* nach Bild 0x24: keine Platzierung mehr */
            int32_t ax = 0, az = 0, opx = 0, opz = 0, kx = 0, kz = 0, ex = 0, ez = 0, tx = 0, tz = 0;
            re15_schritt_station_hole(RE15_SCHRITT_ANFANG, &ax, &az);
            re15_schritt_station_hole(RE15_SCHRITT_OPFER, &opx, &opz);
            re15_schritt_station_hole(RE15_SCHRITT_TICK, &tx, &tz);
            re15_schritt_station_hole(RE15_SCHRITT_KLEMME, &kx, &kz);
            re15_schritt_station_hole(RE15_SCHRITT_ENDE, &ex, &ez);
            n_nach++;
            if (opx != ax || opz != az || ex != kx || ez != kz) n_fremd++;   /* Handler oder Nachlauf bewegt Leon */
            if (kx != tx || kz != tz) n_klemme++;
        }
        if (re15_player_victim_own_bank()) {        /* P3-P6: gerenderte Bank / Richtung */
            re15_anim_view_t av; re15_actor_anim_select(pl, 1, &banks, &av);
            int on_pl00 = (av.anim == &s_pl00_anim);
            if (pl->motion == 0x10) { p3_bilder++; if (on_pl00 && !re15_actor_anim_reverse(pl)) p3_pl00++; }
            if (pl->motion == 0x0b) { p5_bilder++; if (on_pl00 && re15_actor_anim_reverse(pl)) p5_rueck++; }
        }
    }
    if (n_p2) abw_p2_mitte /= n_p2;
    s_gl_frei[L] = f_frei; s_gl_frei_xy[L][0] = fx; s_gl_frei_xy[L][1] = fz;
    if (L != 2)
    printf("  %sPin T%d bei (%d,%d) hp %d, erste Platzierung T%d, frei T%d bei (%d,%d), Ende (%d,%d) hp %d\n", erzwinge ? "[Weg 2] " : "",
           f_pin, (int)px0, (int)pz0, hp_griff, f_sprung, f_frei, (int)fx, (int)fz, (int)pl->x, (int)pl->z, (int)pl->hp);
    s_gl_biss[L] = biss_nach_frei; s_gl_e12[L][0] = e1_ende; s_gl_e12[L][1] = e2_ende;
    if (L) return;
    printf("  Bahn P2 gegen das Original (%s): mittlere Abweichung %.0f, groesste %.0f in T%d; T265-T267 (Schub von e2) %.0f;"
           " P3/P4 %d Bilder Clip 0x10 (PL00 vorwaerts %d), P5/P6 %d Bilder Clip 0xb (PL00 rueckwaerts %d)\n",
           "T265-T290", abw_p2_mitte, abw_p2, f_abw_p2, abw_e2, p3_bilder, p3_pl00, p5_bilder, p5_rueck);
    PRUEF(f_pin >= 253 && f_pin <= 255, "Pin-Latch in T%d (Original T254: Clip 0x1c Bild 4, a780 = 0 -> Front-Griff @0x8011aa4c-bc)", f_pin);
    PRUEF(dist2d(px0, pz0, -6658, -12487) < 60.0, "beim Zupacken bleibt Leon stehen (%d,%d) (Original T254: (-6660,-12486))", (int)px0, (int)pz0);
    PRUEF(f_sprung == 265, "erste Wurf-Platzierung in T%d = Opfer-Bild 0x0b (Original T265 = VSync 10171, +0x95 0x0b beim Eintritt;"
          " P1->P2 @0x8011c244-5c, Fenster @0x8011c278)", f_sprung);
    PRUEF(n_p2 == 26 && abw_p2 <= 100.0 && abw_e2 <= 100.0, "Wurf-Bahn T265-T289 (Platzierung -> Schub -> Wandklemme, Gorilla-Paar ohne Schub)"
          " im Mittel %.0f (T265-T290), hoechstens %.0f (T%d) neben dem Original, T266-T267 mit dem Schub von e2 hoechstens %.0f; T290 (erstes"
          " Bild der Klemmen-Iteration) %.0f (N5: Schranke 100 jetzt auch fuer T265 — N3 64/166, N4 1238 in T265 — T290 = Empfindlichkeit"
          " der Klemme, s. M1 und die N5-Messung unten)", abw_p2_mitte, abw_p2, f_abw_p2, abw_e2, d290);
    {   /* N4: T265 = erstes Klemmen-Bild nach der Platzierung. Die Klemme ist eine Abbildung (Bezug, Eingang) -> Ausgang (Riegel wand:
         * FUN_8003b0a4 bitgleich); mit den ORIGINAL-Eingaben (kette[0]) liefert sie die Original-Lage, mit den Lauf-0-Eingaben (je
         * <= 20 daneben) die Lauf-0-Lage — die Abweichung in T265 ist die Empfindlichkeit der Klemme (M1 (c)), kein anderer Beweger. */
        const int i = 265 - GRIFF_T0;
        int32_t ox = -7190, oz = -10771, lx = s_gl_st[0][i][2], lz = s_gl_st[0][i][3];
        re15_collision_set_band(0); re15_collision_constrain(&s_rdt, -6659, -12487, &ox, &oz);
        re15_collision_set_band(0); re15_collision_constrain(&s_rdt, s_gl_st[0][i][0], s_gl_st[0][i][1], &lx, &lz);
        double din = dist2d(s_gl_st[0][i][2], s_gl_st[0][i][3], -7190, -10771), dbz = dist2d(s_gl_st[0][i][0], s_gl_st[0][i][1], -6659, -12487);
        PRUEF(ox == -7804 && oz == -10186 && lx == s_gl_st[0][i][4] && lz == s_gl_st[0][i][5] && din <= 20.0 && dbz <= 20.0,
              "T265 (Abweichung %.0f): Klemme mit Original-Eingaben -> (%d,%d) (Original (-7804,-10186)), mit Lauf-0-Eingaben (Bezug %.0f, Eingang %.0f"
              " daneben) -> (%d,%d) = Lauf 0 (%d,%d): Empfindlichkeit der Klemme, kein fremder Beweger", d265, (int)ox, (int)oz, dbz, din,
              (int)lx, (int)lz, (int)s_gl_st[0][i][4], (int)s_gl_st[0][i][5]);
    }
    /* N5 (P2): KEIN Pin auf den Ausgang von Lauf 0 — die Landung ist ein Ausgang der chaotischen Klemmen-Iteration
     * (Messung in teil_griff: 24 Starts 1-2 Einheiten neben der Lauf-0-Lage T290). Der N1-Mechanismus (an der Original-
     * Ruhelage kein Biss) wird im Weg-2-Lauf geprueft, der den Original-Zustand traegt. */
    printf("  Lauf 0 nach der Freigabe: erster Biss T%d, Ende (%d,%d), e1 %.0f / e2 %.0f entfernt (Original-Ruhelage (-3009,-11643),"
           " ~3925 / ~4990)\n", biss_nach_frei, (int)pl->x, (int)pl->z, e1_ende, e2_ende);
    PRUEF(p3_bilder == 16 && p3_pl00 == 16, "P3/P4: Clip 0x10 aus PL00 vorwaerts, %d/%d Bilder (Original 16: T338-T353, a2 = 0 @0x8011c318)", p3_pl00, p3_bilder);
    PRUEF(p5_bilder == 25 && p5_rueck == 25, "P5/P6: Clip 0xb aus PL00 RUECKWAERTS, %d/%d Bilder (Original 25: T354-T378, a2 = 1 @0x8011c348)", p5_rueck, p5_bilder);
    PRUEF(f_frei == 378, "Freigabe in T%d (Original T378: aca58 = 1 @0x8011c384-8c)", f_frei);
    printf("  T291..Freigabe: %d Bilder, %d mit fremdem Beweger, %d mit Klemm-Schub; Lage bei der Freigabe (%d,%d) (Original (-4759,-10633))\n",
           n_nach, n_fremd, n_klemme, (int)fx, (int)fz);
    PRUEF(n_nach == 87 && n_fremd == 0, "nach Opfer-Bild 0x24 (T291-T377, %d Bilder) bewegt Leon nur noch die Wandklemme @0x80031d70 (%d Bilder mit anderem"
          " Beweger; Original: kein `jal 0x8001ad68` ab +0x95 = 0x25, `sltiu 0x25` @0x8011c278) — das Zittern selbst ist im Riegel wand bitgleich",
          n_nach, n_fremd);
    PRUEF(hp_griff == 76 && hp_frei == 76, "der Griff kostet keine HP (%d -> %d bei der Freigabe; Original 76 -> 76, kein Schreiber auf Spieler+0x9a in"
          " 0x8011a878-af40 / 0x8011c118-c598)", hp_griff, hp_frei);
}

/* ---------------------------------------------------------------------------------------------- */
/* A1 (Nachbesserung 2): DIESELBE EINGABE WIE DAS ORIGINAL (Leon am Raumeintritt (-22604,14455), keine Eingabe = die
 * r3-Aufnahme / Abnahme-Lauf n10). (a) Leons sub02-Gang (Plc_dest 9/5/5/5/9) Lage fuer Lage gegen die GDB-Spur des
 * Originals (scratch jnb2/g_gang.txt, r3 s001, Haltepunkt Spieler-Tick 0x80031c44): 207 verschiedene (x, z, Yaw)-
 * Zustaende bis zur Endlage (-7138,-12372) Yaw 1513. Belegt: Schritt dz = (R31=-sin)*v SAR 12 (GTE MVMVA sf=1
 * @0x800246ac), Kegeltest halboffen (FUN_8001ab9c `slt` @0x8001abfc). (b) Kampf ohne Eingabe bis zum Tod: Original
 * (GDB) Freigabe aca58 4->1 bei VSync 9037, Heavy-Treffer VSync 9765 (+364 Bilder), Bisse alle 52,4/50/53/... (Gleich-
 * takt, Mittel 51,8 ueber 14 Abstaende), Tod VSync 11423 (+1193 Bilder = 39,8 s). Belegt: FUN_8001af20 hasht das a0
 * des Aufrufers (B[0]/B[3]/B[4], re15_affen.h (7)). */
static const int32_t s_gang_orig[][3] = {   /* x, z, Yaw â€” nur Wechsel */
    {-22604,14455,0}, {-22604,14455,96}, {-22604,14455,191}, {-22413,14397,191},
    {-22222,14339,191}, {-22031,14281,191}, {-21840,14223,191}, {-21649,14165,191},
    {-21458,14107,191}, {-21267,14049,191}, {-21076,13991,191}, {-20885,13933,191},
    {-20694,13875,191}, {-20503,13817,191}, {-20312,13759,191}, {-20121,13701,191},
    {-19930,13643,191}, {-19739,13585,191}, {-19548,13527,191}, {-19357,13469,191},
    {-19166,13411,191}, {-18975,13353,191}, {-18784,13295,191}, {-18593,13237,189},
    {-18402,13179,189}, {-18211,13121,189}, {-18020,13063,189}, {-17829,13005,189},
    {-17638,12947,189}, {-17447,12889,189}, {-17256,12831,189}, {-17065,12774,185},
    {-16874,12717,185}, {-16683,12660,185}, {-16492,12603,185}, {-16300,12547,183},
    {-16108,12491,183}, {-16108,12491,279}, {-16108,12491,375}, {-16108,12491,471},
    {-16108,12491,567}, {-16108,12491,663}, {-16108,12491,759}, {-16108,12491,855},
    {-16079,12293,927}, {-16072,12093,999}, {-16070,11893,1015}, {-16068,11693,1015},
    {-16066,11493,1015}, {-16064,11293,1015}, {-16062,11093,1015}, {-16060,10893,1015},
    {-16058,10693,1015}, {-16056,10493,1015}, {-16054,10293,1015}, {-16052,10093,1015},
    {-16050,9893,1015}, {-16048,9693,1015}, {-16046,9493,1015}, {-16044,9293,1015},
    {-16042,9093,1015}, {-16040,8893,1015}, {-16038,8693,1015}, {-16036,8493,1015},
    {-16034,8293,1015}, {-16032,8093,1015}, {-16030,7893,1015}, {-16028,7693,1015},
    {-16026,7493,1015}, {-16024,7293,1015}, {-16022,7093,1015}, {-16020,6893,1015},
    {-16018,6693,1015}, {-16016,6493,1015}, {-16014,6293,1015}, {-16012,6093,1015},
    {-16010,5893,1015}, {-16008,5693,1015}, {-16006,5493,1015}, {-16004,5293,1015},
    {-16002,5093,1015}, {-16000,4893,1015}, {-15998,4693,1015}, {-15996,4493,1015},
    {-15994,4293,1015}, {-15992,4093,1015}, {-15990,3893,1015}, {-15988,3693,1015},
    {-15986,3493,1015}, {-15984,3293,1015}, {-15982,3093,1015}, {-15980,2893,1015},
    {-15978,2693,1015}, {-15976,2493,1015}, {-15974,2293,1015}, {-15972,2093,1015},
    {-15970,1893,1015}, {-15968,1693,1015}, {-15966,1493,1015}, {-15964,1293,1015},
    {-15962,1093,1015}, {-15960,893,1015}, {-15958,693,1015}, {-15956,493,1015},
    {-15954,293,1015}, {-15952,93,1015}, {-15950,-107,1015}, {-15948,-307,1015},
    {-15946,-507,1015}, {-15944,-707,1015}, {-15942,-907,1015}, {-15940,-1107,1015},
    {-15938,-1307,1015}, {-15936,-1507,1015}, {-15934,-1707,1015}, {-15932,-1907,1015},
    {-15930,-2107,1015}, {-15928,-2307,1015}, {-15926,-2507,1015}, {-15924,-2707,1015},
    {-15922,-2907,1015}, {-15920,-3107,1015}, {-15918,-3307,1015}, {-15916,-3507,1015},
    {-15914,-3707,1015}, {-15912,-3907,1015}, {-15910,-4107,1015}, {-15908,-4307,1015},
    {-15906,-4507,1015}, {-15904,-4707,1015}, {-15902,-4907,1015}, {-15901,-5107,1019},
    {-15900,-5307,1019}, {-15898,-5507,1015}, {-15897,-5707,1019}, {-15895,-5907,1015},
    {-15894,-6107,1019}, {-15894,-6107,923}, {-15894,-6107,827}, {-15894,-6107,731},
    {-15894,-6107,635}, {-15765,-6260,563}, {-15620,-6397,491}, {-15461,-6517,419},
    {-15298,-6632,397}, {-15135,-6747,397}, {-14972,-6862,397}, {-14809,-6977,397},
    {-14646,-7092,397}, {-14483,-7207,397}, {-14320,-7322,397}, {-14157,-7437,397},
    {-13994,-7552,397}, {-13831,-7667,397}, {-13668,-7782,397}, {-13505,-7897,397},
    {-13342,-8012,397}, {-13179,-8127,397}, {-13016,-8242,397}, {-12853,-8357,397},
    {-12690,-8472,397}, {-12527,-8587,397}, {-12364,-8702,397}, {-12201,-8817,397},
    {-12038,-8932,397}, {-11875,-9047,397}, {-11712,-9162,397}, {-11549,-9277,397},
    {-11386,-9392,397}, {-11223,-9507,397}, {-11060,-9622,397}, {-10897,-9737,397},
    {-10734,-9852,397}, {-10571,-9967,397}, {-10408,-10082,397}, {-10245,-10197,397},
    {-10082,-10312,397}, {-9919,-10427,397}, {-9756,-10542,397}, {-9593,-10657,397},
    {-9430,-10772,397}, {-9267,-10887,397}, {-9103,-11001,395}, {-8940,-11116,397},
    {-8776,-11230,395}, {-8612,-11344,395}, {-8448,-11458,395}, {-8285,-11573,397},
    {-8121,-11687,395}, {-7957,-11801,395}, {-7793,-11915,395}, {-7630,-12030,397},
    {-7466,-12144,395}, {-7302,-12258,395}, {-7138,-12372,395}, {-7138,-12372,491},
    {-7138,-12372,587}, {-7138,-12372,683}, {-7138,-12372,779}, {-7138,-12372,875},
    {-7138,-12372,971}, {-7138,-12372,1067}, {-7138,-12372,1163}, {-7138,-12372,1259},
    {-7138,-12372,1355}, {-7138,-12372,1451}, {-7138,-12372,1513},
};
static void teil_szene(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_game_state_init();
    if (room_boot(0x11C0, -22604, 14455, 0, 0, 0) != 0) return;
    PRUEF(bank_laden_27(), "EM027-Bank geladen");
    PRUEF(pl00_laden(), "PL00.EDD/EMR geladen");
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    const int NG = (int)(sizeof s_gang_orig / sizeof s_gang_orig[0]);
    int ng = 0, gleich = 0, erste_abw = -1;
    int32_t lx = pl->x, lz = pl->z, lr = pl->rot_y;          /* Zustand 0 = Raumeintritt vor dem ersten Bild */
    if (lx == s_gang_orig[0][0] && lz == s_gang_orig[0][1] && lr == s_gang_orig[0][2]) gleich++;
    ng = 1;
    int f_frei = -1, f_heavy = -1, f_tod = -1, nb = 0, biss[64];
    int hp_alt = pl->hp, st_alt = pl->state;
    for (int f = 1; f < 3200; f++) {
        frame(0, 0);
        if (ng < NG && (pl->x != lx || pl->z != lz || (int)pl->rot_y != lr)) {
            lx = pl->x; lz = pl->z; lr = pl->rot_y;
            const int32_t *o = s_gang_orig[ng];
            if (o[0] == lx && o[1] == lz && o[2] == lr) gleich++;
            else if (erste_abw < 0) { erste_abw = ng; printf("    erste Abweichung Zustand %d: Port (%d,%d,%d) Original (%d,%d,%d)\n", ng,
                                                          (int)lx, (int)lz, (int)lr, (int)o[0], (int)o[1], (int)o[2]); }
            ng++;
        }
        if (f_frei < 0 && st_alt == 4 && pl->state == 1 && ng >= NG) { f_frei = f; printf("    Freigabe F%d bei (%d,%d)\n", f, (int)pl->x, (int)pl->z); }
        if (f_frei >= 0 && pl->hp < hp_alt) {
            if (f_heavy < 0) f_heavy = f; else if (nb < 64) biss[nb++] = f;
            printf("    F%d HP %d -> %d (Freigabe + %d)\n", f, hp_alt, (int)pl->hp, f - f_frei);
        }
        hp_alt = pl->hp; st_alt = pl->state;
        if (f_frei >= 0 && pl->hp < 0) { f_tod = f; break; }
    }
    printf("  Gang: %d/%d Zustaende gleich; Freigabe F%d, Heavy +%d, Tod +%d, %d Bisse\n", gleich, ng, f_frei,
           f_heavy - f_frei, f_tod - f_frei, nb);
    PRUEF(ng >= NG && gleich == NG, "sub02-Gang: %d/%d Lage-/Yaw-Zustaende bitgleich mit dem Original bis (-7138,-12372) Yaw 1513"
          " (vorher Endlage (-7153,-12351): dz -57 statt -58 je Schritt, Kegel geschlossen)", gleich, NG);
    PRUEF(f_frei > 0 && f_heavy - f_frei >= 361 && f_heavy - f_frei <= 367, "Heavy-Treffer Freigabe + %d Bilder (Original +364: Leerlauf-Timer"
          " H(Entity) + 59 = 239 @0x80117594, Sprung, Heavy-Anlauf 21 x +73 @0x80118164-94)", f_heavy - f_frei);
    if (nb >= 14) {
        int kmin = 999; for (int i = 1; i < 14; i++) { int d = biss[i] - biss[i - 1]; if (d < kmin) kmin = d; }
        double mitte = (double)(biss[13] - biss[0]) / 13.0;
        PRUEF(kmin >= 44 && mitte >= 50.8 && mitte <= 52.8, "Biss-Takt: Mittel %.1f Bilder, kuerzester Abstand %d (Original Gleichtakt 51,8, 49..55;"
              " vorher Wechseltakt 35/36)", mitte, kmin);
    } else PRUEF(0, "mindestens 15 Bisse bis zum Tod (%d)", nb);
    PRUEF(f_tod > 0 && f_tod - f_frei >= 1181 && f_tod - f_frei <= 1205, "Tod Freigabe + %d Bilder (Original +1193 = 39,8 s; vorher 33,3 s)",
          f_tod - f_frei);
    {   /* N4 (N2 (d)): EINZELBISSE gegen die Original-Liste (GDB, Abnahme 2 jnb2/g_frei.txt + jnb1/g_orig.txt, relativ zur Freigabe):
         * Slot 2 beisst die Bisse 2, 4, ... 14 (+521, 624, ... 1139 = Zyklus 103). Eine Drift von 1 Bild je Zyklus (HEAD 458635e1:
         * +524 ... +1148) faellt hier auf: der Versatz zum Original muss fuer alle sieben Slot-2-Bisse GLEICH bleiben (+-1). */
        static const int orig[14] = { 468, 521, 571, 624, 674, 727, 778, 830, 882, 933, 986, 1036, 1090, 1139 };
        int d0 = (nb >= 2) ? (biss[1] - f_frei) - orig[1] : 999, dmin = 999, dmax = -999, s3max = 0, s3min = 999, s3hi = -999;
        printf("  Einzelbisse (Port - Original):");
        for (int i = 0; i < 14 && i < nb; i++) {
            int d = (biss[i] - f_frei) - orig[i];
            printf(" %+d", d);
            if (i & 1) { if (d < dmin) dmin = d; if (d > dmax) dmax = d; }
            else { if (abs(d) > s3max) s3max = abs(d); if (d < s3min) s3min = d; if (d > s3hi) s3hi = d; }
        }
        printf("\n");
        PRUEF(nb >= 14 && dmax - dmin <= 1 && abs(d0) <= 3, "Slot 2: Versatz der 7 Bisse zum Original %+d .. %+d (gleichbleibend = Zyklus 103 wie das Original;"
              " HEAD 458635e1: +3 .. +9)", dmin, dmax);
        {   /* N5 (P1): Slot 3 (Bisse 1, 3, ... 13 + Todesbiss +1194) wie Slot 2 auf GLEICHBLEIBENDEN Versatz pruefen = Zyklus wie das
             * Original (103, 103, 104, 104, 104, 104, 104). Nachbesserung 4 lag bei 0,0,0,-1,-2,-3,-4,-5 (Zyklus 103) — die alte
             * Schranke "groesste Abweichung <= 6" liess diese Drift durch. */
            int dt = (f_tod > 0) ? (f_tod - f_frei) - 1194 : 999;
            if (dt < s3min) s3min = dt; if (dt > s3hi) s3hi = dt; if (abs(dt) > s3max) s3max = abs(dt);
            PRUEF(nb >= 14 && s3hi - s3min <= 1 && s3max <= 3, "Slot 3: Versatz der 7 Bisse und des Todesbisses zum Original %+d .. %+d"
                  " (gleichbleibend = Zyklen 103, 103, 104, 104, 104, 104, 104 wie das Original; Nachbesserung 4: 0 .. -5 = Zyklus 103)", s3min, s3hi);
        }
    }
}

/* ---------------------------------------------------------------------------------------------- */
/* A2 (Nachbesserung 2): die WANDKLEMME FUN_8003b0a4 (Port re15_collision_constrain) gegen das Original, Bild fuer Bild.
 * Original-GDB (scratch jnb2/g_wer.txt, r3 s033, Griff-Experiment wie `griff`): je Spieler-Tick die Lage beim
 * Eintritt 0x80031c44 (= Spiegel +0x40/+0x44, Bezug von FUN_8003bca8 @0x8003b4ac/@0x8003bd44-48), vor `jal 0x8003b0a4`
 * @0x80031d70 und danach @0x80031d78. Band 0, Radius 450 (`lhu a1,6(v0)` @0x80031d6c), Maske 1 (@0x80031d74).
 * Spalten: VSync, Bezug x/z, Eingang x/z, Ausgang x/z. 135 der 168 Bilder schiebt das Original. */
static const int32_t s_wand_orig[][7] = {
    { 10135, -6657,-12488, -6657,-12487, -6658,-12488 }, /* mo 0300 */
    { 10137, -6658,-12488, -6658,-12487, -6658,-12487 }, /* mo 0300 */
    { 10139, -6658,-12487, -6658,-12486, -6659,-12487 }, /* mo 0300 */
    { 10141, -6659,-12487, -6659,-12486, -6659,-12486 }, /* mo 0300 */
    { 10143, -6659,-12486, -6659,-12485, -6660,-12486 }, /* mo 0300 */
    { 10145, -6660,-12486, -6660,-12485, -6660,-12485 }, /* mo 0300 */
    { 10147, -6660,-12485, -6658,-12484, -6660,-12486 }, /* mo 0300 */
    { 10149, -6660,-12486, -6660,-12486, -6660,-12486 }, /* mo 0101 */
    { 10151, -6660,-12486, -6658,-12486, -6659,-12487 }, /* mo 0102 */
    { 10153, -6659,-12487, -6659,-12487, -6659,-12487 }, /* mo 0103 */
    { 10155, -6659,-12487, -6659,-12487, -6659,-12487 }, /* mo 0104 */
    { 10157, -6659,-12487, -6659,-12487, -6659,-12487 }, /* mo 0105 */
    { 10159, -6659,-12487, -6659,-12487, -6659,-12487 }, /* mo 0106 */
    { 10161, -6659,-12487, -6659,-12487, -6659,-12487 }, /* mo 0107 */
    { 10163, -6659,-12487, -6659,-12487, -6659,-12487 }, /* mo 0108 */
    { 10165, -6659,-12487, -6659,-12487, -6659,-12487 }, /* mo 0109 */
    { 10167, -6659,-12487, -6659,-12487, -6659,-12487 }, /* mo 010a */
    { 10169, -6659,-12487, -6659,-12487, -6659,-12487 }, /* mo 010b */
    { 10171, -6659,-12487, -7190,-10771, -7804,-10186 }, /* mo 010c */
    { 10173, -7804,-10186, -7140,-10733, -6686,-10348 }, /* mo 010d */
    { 10175, -6686,-10348, -6756,-11225, -7340,-11870 }, /* mo 010e */
    { 10177, -7340,-11870, -7193,-11403, -7492,-11733 }, /* mo 010f */
    { 10179, -7492,-11733, -7016,-11975, -7110,-12078 }, /* mo 0110 */
    { 10181, -7110,-12078, -7016,-11975, -7110,-12078 }, /* mo 0111 */
    { 10183, -7110,-12078, -6799,-12508, -6799,-12508 }, /* mo 0112 */
    { 10185, -6799,-12508, -6799,-12508, -6799,-12508 }, /* mo 0113 */
    { 10187, -6799,-12508, -6542,-12913, -6542,-12913 }, /* mo 0114 */
    { 10189, -6542,-12913, -6542,-12913, -6542,-12913 }, /* mo 0115 */
    { 10191, -6542,-12913, -6210,-13216, -6210,-13216 }, /* mo 0116 */
    { 10193, -6210,-13216, -6210,-13216, -6210,-13216 }, /* mo 0117 */
    { 10195, -6210,-13216, -5804,-13474, -5804,-13474 }, /* mo 0118 */
    { 10197, -5804,-13474, -5804,-13474, -5804,-13474 }, /* mo 0119 */
    { 10199, -5804,-13474, -5374,-13651, -5374,-13651 }, /* mo 011a */
    { 10201, -5374,-13651, -5374,-13651, -5374,-13651 }, /* mo 011b */
    { 10203, -5374,-13651, -4969,-13711, -4969,-13711 }, /* mo 011c */
    { 10205, -4969,-13711, -4969,-13711, -4969,-13711 }, /* mo 011d */
    { 10207, -4969,-13711, -4639,-13614, -4639,-13614 }, /* mo 011e */
    { 10209, -4639,-13614, -4639,-13614, -4639,-13614 }, /* mo 011f */
    { 10211, -4639,-13614, -4353,-13122, -4353,-13122 }, /* mo 0120 */
    { 10213, -4353,-13122, -4353,-13122, -4353,-13122 }, /* mo 0121 */
    { 10215, -4353,-13122, -4181,-12266, -3964,-12506 }, /* mo 0122 */
    { 10217, -3964,-12506, -4181,-12266, -3964,-12506 }, /* mo 0123 */
    { 10219, -3964,-12506, -4334,-11423, -4588,-11243 }, /* mo 0124 */
    { 10221, -4588,-11243, -5402,-10631, -5381,-10551 }, /* mo 0125 */
    { 10223, -5381,-10551, -5381,-10551, -5302,-10633 }, /* mo 0126 */
    { 10225, -5302,-10633, -5302,-10633, -5207,-10699 }, /* mo 0127 */
    { 10227, -5207,-10699, -5207,-10699, -5098,-10737 }, /* mo 0128 */
    { 10229, -5098,-10737, -5098,-10737, -5190,-10791 }, /* mo 0129 */
    { 10231, -5190,-10791, -5190,-10791, -5237,-10891 }, /* mo 012a */
    { 10233, -5237,-10891, -5237,-10891, -5122,-10899 }, /* mo 012b */
    { 10235, -5122,-10899, -5122,-10899, -5170,-10989 }, /* mo 012c */
    { 10237, -5170,-10989, -5170,-10989, -5291,-11003 }, /* mo 012d */
    { 10239, -5291,-11003, -5291,-11003, -5268,-10904 }, /* mo 012e */
    { 10241, -5268,-10904, -5268,-10904, -5381,-10936 }, /* mo 012f */
    { 10243, -5381,-10936, -5381,-10936, -5408,-10842 }, /* mo 0130 */
    { 10245, -5408,-10842, -5408,-10842, -5286,-10864 }, /* mo 0131 */
    { 10247, -5286,-10864, -5286,-10864, -5312,-10747 }, /* mo 0132 */
    { 10249, -5312,-10747, -5312,-10747, -5239,-10828 }, /* mo 0133 */
    { 10251, -5239,-10828, -5239,-10828, -5276,-10938 }, /* mo 0134 */
    { 10253, -5276,-10938, -5276,-10938, -5178,-10927 }, /* mo 0135 */
    { 10255, -5178,-10927, -5178,-10927, -5193,-11051 }, /* mo 0136 */
    { 10257, -5193,-11051, -5193,-11051, -5089,-10997 }, /* mo 0137 */
    { 10259, -5089,-10997, -5089,-10997, -5042,-10875 }, /* mo 0138 */
    { 10261, -5042,-10875, -5042,-10875, -5166,-10886 }, /* mo 0139 */
    { 10263, -5166,-10886, -5166,-10886, -5137,-10794 }, /* mo 013a */
    { 10265, -5137,-10794, -5137,-10794, -5014,-10811 }, /* mo 013b */
    { 10267, -5014,-10811, -5014,-10811, -5078,-10885 }, /* mo 013c */
    { 10269, -5078,-10885, -5078,-10885, -5166,-10933 }, /* mo 013d */
    { 10271, -5166,-10933, -5166,-10933, -5207,-11030 }, /* mo 013e */
    { 10273, -5207,-11030, -5207,-11030, -5341,-11031 }, /* mo 013f */
    { 10275, -5341,-11031, -5341,-11031, -5280,-11108 }, /* mo 0140 */
    { 10277, -5280,-11108, -5280,-11108, -5145,-11092 }, /* mo 0141 */
    { 10279, -5145,-11092, -5145,-11092, -5163,-11203 }, /* mo 0142 */
    { 10281, -5163,-11203, -5163,-11203, -5329,-11163 }, /* mo 0143 */
    { 10283, -5329,-11163, -5329,-11163, -5375,-11039 }, /* mo 0144 */
    { 10285, -5375,-11039, -5375,-11039, -5255,-11055 }, /* mo 0145 */
    { 10287, -5255,-11055, -5255,-11055, -5359,-11087 }, /* mo 0146 */
    { 10289, -5359,-11087, -5359,-11087, -5475,-11047 }, /* mo 0147 */
    { 10291, -5475,-11047, -5475,-11047, -5497,-10895 }, /* mo 0148 */
    { 10293, -5497,-10895, -5497,-10895, -5429,-10964 }, /* mo 0149 */
    { 10295, -5429,-10964, -5429,-10964, -5350,-11021 }, /* mo 014a */
    { 10297, -5350,-11021, -5350,-11021, -5292,-11101 }, /* mo 014b */
    { 10299, -5292,-11101, -5292,-11101, -5416,-11112 }, /* mo 014c */
    { 10301, -5416,-11112, -5416,-11112, -5420,-11247 }, /* mo 014d */
    { 10303, -5420,-11247, -5420,-11247, -5553,-11158 }, /* mo 014e */
    { 10305, -5553,-11158, -5553,-11158, -5643,-11015 }, /* mo 014f */
    { 10307, -5643,-11015, -5643,-11015, -5481,-10978 }, /* mo 0150 */
    { 10309, -5481,-10978, -5481,-10978, -5595,-10937 }, /* mo 0151 */
    { 10311, -5595,-10937, -5595,-10937, -5620,-10788 }, /* mo 0152 */
    { 10313, -5620,-10788, -5620,-10788, -5546,-10851 }, /* mo 0100 */
    { 10315, -5546,-10851, -5546,-10851, -5478,-10920 }, /* mo 1001 */
    { 10317, -5478,-10920, -5478,-10920, -5397,-10976 }, /* mo 1002 */
    { 10319, -5397,-10976, -5397,-10976, -5344,-11061 }, /* mo 1003 */
    { 10321, -5344,-11061, -5344,-11061, -5233,-11086 }, /* mo 1004 */
    { 10323, -5233,-11086, -5233,-11086, -5127,-11035 }, /* mo 1005 */
    { 10325, -5127,-11035, -5127,-11035, -5086,-10906 }, /* mo 1006 */
    { 10327, -5086,-10906, -5086,-10906, -5196,-10932 }, /* mo 1007 */
    { 10329, -5196,-10932, -5196,-10932, -5194,-10808 }, /* mo 1008 */
    { 10331, -5194,-10808, -5194,-10808, -5261,-10887 }, /* mo 1009 */
    { 10333, -5261,-10887, -5261,-10887, -5358,-10935 }, /* mo 100a */
    { 10335, -5358,-10935, -5358,-10935, -5417,-10878 }, /* mo 100b */
    { 10337, -5417,-10878, -5417,-10878, -5482,-10829 }, /* mo 100c */
    { 10339, -5482,-10829, -5482,-10829, -5533,-10763 }, /* mo 100d */
    { 10341, -5533,-10763, -5533,-10763, -5613,-10730 }, /* mo 100e */
    { 10343, -5613,-10730, -5613,-10730, -5634,-10629 }, /* mo 100f */
    { 10345, -5634,-10629, -5634,-10629, -5524,-10664 }, /* mo 1000 */
    { 10347, -5524,-10664, -5524,-10664, -5514,-10566 }, /* mo 0b01 */
    { 10349, -5514,-10566, -5514,-10566, -5438,-10644 }, /* mo 0b02 */
    { 10351, -5438,-10644, -5438,-10644, -5348,-10707 }, /* mo 0b03 */
    { 10353, -5348,-10707, -5348,-10707, -5286,-10799 }, /* mo 0b04 */
    { 10355, -5286,-10799, -5286,-10799, -5217,-10792 }, /* mo 0b05 */
    { 10357, -5217,-10792, -5217,-10792, -5228,-10692 }, /* mo 0b06 */
    { 10359, -5228,-10692, -5228,-10692, -5302,-10772 }, /* mo 0b07 */
    { 10361, -5302,-10772, -5302,-10772, -5204,-10828 }, /* mo 0b08 */
    { 10363, -5204,-10828, -5204,-10828, -5288,-10889 }, /* mo 0b09 */
    { 10365, -5288,-10889, -5288,-10889, -5349,-10975 }, /* mo 0b0a */
    { 10367, -5349,-10975, -5349,-10975, -5361,-10864 }, /* mo 0b0b */
    { 10369, -5361,-10864, -5361,-10864, -5270,-10918 }, /* mo 0b0c */
    { 10371, -5270,-10918, -5270,-10918, -5152,-10929 }, /* mo 0b0d */
    { 10373, -5152,-10929, -5152,-10929, -5205,-11014 }, /* mo 0b0e */
    { 10375, -5205,-11014, -5205,-11014, -5315,-11040 }, /* mo 0b0f */
    { 10377, -5315,-11040, -5315,-11040, -5277,-11140 }, /* mo 0b10 */
    { 10379, -5277,-11140, -5277,-11140, -5191,-11065 }, /* mo 0b11 */
    { 10381, -5191,-11065, -5191,-11065, -5109,-10985 }, /* mo 0b12 */
    { 10383, -5109,-10985, -5109,-10985, -5019,-10915 }, /* mo 0b13 */
    { 10385, -5019,-10915, -5019,-10915, -4946,-10824 }, /* mo 0b14 */
    { 10387, -4946,-10824, -4946,-10824, -4837,-10776 }, /* mo 0b15 */
    { 10389, -4837,-10776, -4837,-10776, -4800,-10642 }, /* mo 0b16 */
    { 10391, -4800,-10642, -4800,-10642, -4903,-10676 }, /* mo 0b17 */
    { 10393, -4903,-10676, -4903,-10676, -4915,-10803 }, /* mo 0b18 */
    { 10395, -4915,-10803, -4915,-10803, -4816,-10743 }, /* mo 0b00 */
    { 10397, -4816,-10743, -4816,-10743, -4759,-10633 }, /* mo 0b00 */
    { 10399, -4759,-10633, -4759,-10633, -4620,-10622 }, /* mo 0300 */
    { 10401, -4620,-10622, -4620,-10622, -4645,-10726 }, /* mo 0300 */
    { 10403, -4645,-10726, -4645,-10726, -4798,-10698 }, /* mo 0300 */
    { 10405, -4798,-10698, -4798,-10698, -4699,-10638 }, /* mo 0300 */
    { 10407, -4699,-10638, -4699,-10638, -4643,-10527 }, /* mo 0300 */
    { 10409, -4643,-10527, -4643,-10527, -4502,-10517 }, /* mo 0300 */
    { 10411, -4502,-10517, -4502,-10517, -4529,-10618 }, /* mo 0300 */
    { 10413, -4529,-10618, -4529,-10618, -4677,-10596 }, /* mo 0300 */
    { 10415, -4677,-10596, -4677,-10596, -4590,-10522 }, /* mo 0300 */
    { 10417, -4590,-10522, -4590,-10522, -4521,-10460 }, /* mo 0300 */
    { 10419, -4521,-10460, -4521,-10460, -4499,-10473 }, /* mo 0300 */
    { 10421, -4499,-10473, -4499,-10473, -4538,-10449 }, /* mo 0300 */
    { 10423, -4538,-10449, -4538,-10449, -4470,-10491 }, /* mo 0300 */
    { 10425, -4470,-10491, -4470,-10491, -4502,-10588 }, /* mo 0300 */
    { 10427, -4502,-10588, -4502,-10588, -4640,-10575 }, /* mo 0300 */
    { 10429, -4640,-10575, -4640,-10575, -4570,-10481 }, /* mo 0300 */
    { 10431, -4570,-10481, -4570,-10481, -4484,-10482 }, /* mo 0300 */
    { 10433, -4484,-10482, -4484,-10482, -4484,-10612 }, /* mo 0300 */
    { 10435, -4484,-10612, -4484,-10612, -4362,-10558 }, /* mo 0300 */
    { 10437, -4362,-10558, -4362,-10558, -4399,-10535 }, /* mo 0300 */
    { 10439, -4399,-10535, -4399,-10535, -4333,-10576 }, /* mo 0300 */
    { 10441, -4333,-10576, -4333,-10576, -4336,-10691 }, /* mo 0300 */
    { 10443, -4336,-10691, -4336,-10691, -4515,-10625 }, /* mo 0300 */
    { 10445, -4515,-10625, -4515,-10625, -4354,-10571 }, /* mo 0300 */
    { 10447, -4354,-10571, -4354,-10571, -4323,-10722 }, /* mo 0300 */
    { 10449, -4323,-10722, -4323,-10722, -4193,-10663 }, /* mo 0300 */
    { 10451, -4193,-10663, -4193,-10663, -4092,-10871 }, /* mo 0300 */
    { 10453, -4092,-10871, -4092,-10871, -3984,-10791 }, /* mo 0300 */
    { 10455, -3984,-10791, -3984,-10791, -3765,-11101 }, /* mo 0300 */
    { 10457, -3765,-11101, -3765,-11101, -3520,-11078 }, /* mo 0300 */
    { 10459, -3520,-11078, -3520,-11078, -3010,-11642 }, /* mo 0300 */
    { 10461, -3010,-11642, -3010,-11642, -3009,-11643 }, /* mo 0300 */
    { 10463, -3009,-11643, -3009,-11643, -3009,-11643 }, /* mo 0300 */
    { 10465, -3009,-11643, -3009,-11643, -3009,-11643 }, /* mo 0300 */
    { 10467, -3009,-11643, -3009,-11643, -3009,-11643 }, /* mo 0300 */
    { 10469, -3009,-11643, -3009,-11643, -3009,-11643 }, /* mo 0300 */
};
static void teil_wand(void)
{
    re15_game_state_init();
    if (room_boot(0x11C0, -6729, -12800, 647, 5, 1) != 0) return;
    int n = (int)(sizeof s_wand_orig / sizeof s_wand_orig[0]), gleich = 0, schub = 0, abw_max = 0;
    for (int i = 0; i < n; i++) {
        const int32_t *t = s_wand_orig[i];
        re15_collision_set_band(0);
        int32_t x = t[3], z = t[4];
        re15_collision_constrain(&s_rdt, t[1], t[2], &x, &z);
        int dx = abs((int)(x - t[5])), dz = abs((int)(z - t[6])), d = dx > dz ? dx : dz;
        if (t[3] != t[5] || t[4] != t[6]) schub++;
        if (d == 0) gleich++;
        if (d > abw_max) abw_max = d;
        if (d != 0)
            printf("    vs%d Bezug (%d,%d) Eingang (%d,%d) -> Port (%d,%d) Original (%d,%d)\n", (int)t[0], (int)t[1], (int)t[2],
                   (int)t[3], (int)t[4], (int)x, (int)z, (int)t[5], (int)t[6]);
    }
    printf("  Wandklemme: %d/%d Bilder bitgleich (%d mit Schub im Original), groesste Abweichung %d\n", gleich, n, schub, abw_max);
    PRUEF(gleich == n, "re15_collision_constrain = FUN_8003b0a4 in allen %d Original-Bildern des Wurfs (%d bitgleich)", n, gleich);
    /* KOERPER-SCHUB des zweiten Gorillas in T265-T267 (Original: Lage nach dem Handler @0x80031cbc, nach dem Schub
     * @0x80031cc4, e2 nach seinem Tick aus g_griff.txt) und danach die Klemme — die ganze Kette des Spieler-Schwanzes. */
    static const int32_t kette[3][11] = {   /* T, Bezug x/z, Handler x/z, e2 x/z, Schub x/z, Klemme x/z */
        { 265, -6659,-12487, -7332,-10884, -8795,-12049, -7190,-10771, -7804,-10186 },
        { 266, -7804,-10186, -7332,-10884, -8754,-12002, -7140,-10733, -6686,-10348 },
        { 267, -6686,-10348, -7193,-11403, -8655,-12000, -6756,-11225, -7340,-11870 },
    };
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *g1 = aktor_vom_typ(0x27, 0), *g2 = aktor_vom_typ(0x27, 1);
    if (!g1 || !g2) { PRUEF(0, "zwei Gorillas im Kampf-Layout"); return; }
    g1->x = 20000; g1->z = 20000;                         /* der Greifer selbst ist ausgenommen (6e) */
    int kette_ok = 0;
    for (int i = 0; i < 3; i++) {
        const int32_t *k = kette[i];
        g2->x = k[5]; g2->z = k[6]; g2->y = 0; pl->x = k[3]; pl->z = k[4]; pl->y = 0; pl->hp = 76;
        re15_body_push_player();
        int32_t sx = pl->x, sz = pl->z;
        re15_collision_set_band(0);
        int32_t wx = sx, wz = sz;
        re15_collision_constrain(&s_rdt, k[1], k[2], &wx, &wz);
        printf("    T%d Handler (%d,%d) e2 (%d,%d) -> Schub Port (%d,%d) Original (%d,%d) -> Klemme Port (%d,%d) Original (%d,%d)\n",
               (int)k[0], (int)k[3], (int)k[4], (int)k[5], (int)k[6], (int)sx, (int)sz, (int)k[7], (int)k[8], (int)wx, (int)wz,
               (int)k[9], (int)k[10]);
        if (abs((int)(sx - k[7])) <= 3 && abs((int)(sz - k[8])) <= 3 && abs((int)(wx - k[9])) <= 3 && abs((int)(wz - k[10])) <= 3)
            kette_ok++;
    }
    PRUEF(kette_ok == 3, "Koerper-Schub FUN_8002b544 (e2, r %d + 450) + Wandklemme in T265-T267 wie das Original (%d/3 Bilder auf 3 Einheiten)",
          (int)g2->hit_radius_min, kette_ok);
}

/* ---------------------------------------------------------------------------------------------- */
/* Nachbesserung 3 (Abnahme 2, M1/M2) — Auswertung des Griff-Laufs gegen die Original-GDB-Spuren.
 * M2: der Greifer e1 wird in Phase 2/3 jedes Bild absolut aus seinem Anker platziert (`jal 0x8001ad68` a0 = g_entity
 *     @0x8011accc, Yaw-Fang `jal 0x8001a8f8` a1 = 0x800 @0x8011acac) und schiebt dabei e2 weg (Abstand 3200 T256-T264).
 * M1: ab T291 bewegt im Original nur FUN_8003b0a4 (s_wand_orig vs10223-10469: Bezug == Eingang == Vorbild-Ausgang in
 *     124/124 Bildern) -> die Bahn ab T290 ist die Iteration p -> Klemme(p, Bezug p). Gemessen wird, ob die Port-Klemme
 *     die Original-Bahn ab dem Original-Zustand T290 reproduziert und wie empfindlich die Iteration auf den Start ist. */
static const int32_t s_e12_orig[][7] = {   /* T, e1 x/z/Yaw, e2 x/z/Yaw (nach dem Bild; jnb1/g_griff.txt F = T + 1) */
    { 250, -5895,-14391,2825, -8711,-12397,29 }, /* e1 15/1/0 c28/1  e2 3/1/0 c5/15 */
    { 251, -5895,-14392,2825, -8710,-12400,29 }, /* e1 15/1/0 c28/2  e2 3/1/0 c5/16 */
    { 252, -5895,-14393,2825, -8712,-12403,27 }, /* e1 15/1/0 c28/3  e2 3/1/0 c5/17 */
    { 253, -5895,-14394,2825, -8709,-12400,27 }, /* e1 15/2/0 c28/4  e2 3/1/0 c5/18 */
    { 254, -5882,-14389,2823, -8711,-12399,27 }, /* e1 15/3/0 c28/5  e2 3/1/0 c5/19 */
    { 255, -5953,-14268,2823, -8709,-12406,27 }, /* e1 15/3/0 c28/6  e2 3/1/0 c5/20 */
    { 256, -6062,-14169,2823, -8731,-12394,27 }, /* e1 15/3/0 c28/7  e2 3/1/0 c5/21 */
    { 257, -6116,-14119,2823, -8782,-12348,29 }, /* e1 15/3/0 c28/8  e2 3/1/0 c5/22 */
    { 258, -6199,-14044,2823, -8868,-12264,41 }, /* e1 15/3/0 c28/9  e2 3/1/0 c5/23 */
    { 259, -6308,-13944,2823, -8963,-12141,62 }, /* e1 15/3/0 c28/10  e2 3/1/0 c5/24 */
    { 260, -6404,-13856,2823, -9041,-12032,83 }, /* e1 15/3/0 c28/11  e2 3/1/0 c5/25 */
    { 261, -6471,-13796,2823, -9092,-12016,104 }, /* e1 15/3/0 c28/12  e2 3/1/0 c5/26 */
    { 262, -6468,-13798,2823, -9094,-12016,123 }, /* e1 15/3/0 c28/13  e2 3/1/0 c5/27 */
    { 263, -6380,-13878,2823, -8996,-12031,123 }, /* e1 15/3/0 c28/14  e2 3/1/0 c5/28 */
    { 264, -6307,-13944,2823, -8890,-12048,123 }, /* e1 15/3/0 c28/15  e2 3/1/0 c5/29 */
    { 265, -6249,-13997,2823, -8795,-12049,127 }, /* e1 15/3/0 c28/16  e2 3/1/0 c5/30 */
    { 266, -6249,-13997,2823, -8754,-12002,106 }, /* e1 15/3/0 c28/17  e2 3/1/0 c5/31 */
    { 267, -6191,-14052,2823, -8655,-12000,85 }, /* e1 15/3/0 c28/18  e2 3/1/0 c5/32 */
    { 268, -6191,-14052,2823, -9377,-12112,64 }, /* e1 15/3/0 c28/19  e2 3/1/0 c5/33 */
    { 269, -6132,-14105,2823, -9497,-12178,43 }, /* e1 15/3/0 c28/20  e2 3/1/0 c5/34 */
    { 270, -6132,-14105,2823, -9396,-12183,22 }, /* e1 15/3/0 c28/21  e2 3/1/0 c5/35 */
    { 271, -6069,-14162,2823, -9307,-12188,1 }, /* e1 15/3/0 c28/22  e2 3/1/0 c5/36 */
    { 272, -6069,-14162,2823, -9231,-12187,22 }, /* e1 15/3/0 c28/23  e2 3/1/0 c5/37 */
    { 273, -6006,-14220,2823, -9175,-12190,43 }, /* e1 15/3/0 c28/24  e2 3/1/0 c5/38 */
    { 274, -6006,-14220,2823, -9217,-12189,64 }, /* e1 15/3/0 c28/25  e2 3/1/0 c5/0 */
    { 275, -5926,-14294,2823, -9232,-12187,85 }, /* e1 15/3/0 c28/26  e2 3/1/0 c5/1 */
    { 276, -5926,-14294,2823, -9224,-12188,106 }, /* e1 15/3/0 c28/27  e2 3/1/0 c5/2 */
    { 277, -5867,-14427,2823, -9205,-12192,127 }, /* e1 15/3/0 c28/28  e2 3/1/0 c5/3 */
    { 278, -5867,-14427,2823, -9173,-12198,148 }, /* e1 15/3/0 c28/29  e2 3/1/0 c5/4 */
    { 279, -5816,-14555,2823, -9131,-12206,169 }, /* e1 15/3/0 c28/30  e2 3/1/0 c5/5 */
    { 280, -5816,-14555,2823, -9080,-12222,190 }, /* e1 15/3/0 c28/31  e2 3/1/0 c5/6 */
    { 281, -5780,-14644,2823, -9020,-12237,211 }, /* e1 15/3/0 c28/32  e2 3/1/0 c5/7 */
    { 282, -5780,-14644,2823, -8953,-12263,227 }, /* e1 15/3/0 c28/33  e2 3/1/0 c5/8 */
    { 283, -5774,-14660,2823, -8880,-12282,225 }, /* e1 15/3/0 c28/34  e2 3/1/0 c5/9 */
    { 284, -5774,-14660,2823, -8807,-12311,204 }, /* e1 15/3/0 c28/35  e2 3/1/0 c5/10 */
    { 285, -5812,-14566,2823, -8730,-12340,199 }, /* e1 15/3/0 c28/36  e2 3/1/0 c5/11 */
    { 286, -5812,-14566,2823, -8651,-12364,178 }, /* e1 15/3/0 c28/37  e2 3/1/0 c5/12 */
    { 287, -5885,-14382,2823, -8566,-12391,157 }, /* e1 15/3/0 c28/38  e2 3/1/0 c5/13 */
    { 288, -5885,-14382,2823, -8484,-12411,136 }, /* e1 15/3/0 c28/39  e2 3/1/0 c5/14 */
    { 289, -5913,-14305,2823, -8469,-12378,115 }, /* e1 15/3/0 c28/40  e2 3/1/0 c5/15 */
    { 290, -5928,-14291,2823, -8468,-12331,94 }, /* e1 15/3/0 c28/41  e2 3/1/0 c5/16 */
    { 291, -5954,-14267,2823, -8475,-12279,73 }, /* e1 15/3/0 c28/42  e2 3/1/0 c5/17 */
    { 292, -5975,-14248,2823, -8464,-12230,52 }, /* e1 15/3/0 c28/43  e2 3/1/0 c5/18 */
    { 293, -5983,-14241,2823, -8446,-12196,31 }, /* e1 15/3/0 c28/44  e2 3/1/0 c5/19 */
    { 294, -5984,-14240,2823, -8430,-12170,10 }, /* e1 15/3/0 c28/45  e2 3/1/0 c5/20 */
    { 295, -5989,-14236,2823, -8406,-12135,65525 }, /* e1 15/3/0 c28/46  e2 3/1/0 c5/21 */
    { 296, -5977,-14246,2823, -8368,-12112,65504 }, /* e1 15/3/0 c28/47  e2 3/1/0 c5/22 */
    { 297, -5974,-14249,2823, -8344,-12090,65483 }, /* e1 15/3/0 c28/48  e2 3/1/0 c5/23 */
    { 298, -5979,-14245,2823, -8344,-12082,65462 }, /* e1 15/3/0 c28/49  e2 3/1/0 c5/24 */
    { 299, -5977,-14247,2823, -8341,-12086,65441 }, /* e1 15/3/0 c28/50  e2 3/1/0 c5/25 */
    { 300, -5975,-14249,2823, -8336,-12090,65420 }, /* e1 15/3/0 c28/51  e2 3/1/0 c5/26 */
    { 301, -5969,-14254,2823, -8331,-12095,65399 }, /* e1 15/4/0 c28/0  e2 3/1/0 c5/27 */
    { 302, -5967,-14256,2823, -8331,-12095,65378 }, /* e1 15/5/0 c3/23  e2 3/1/0 c5/28 */
};

static void klemme_iter(int32_t x, int32_t z, int t_bis, int32_t (*bahn)[2])
{
    for (int t = 291; t <= t_bis; t++) {
        int32_t nx = x, nz = z;
        re15_collision_set_band(0);
        re15_collision_constrain(&s_rdt, x, z, &nx, &nz);        /* Bezug = Eingang = Lage am Ende des Vorbilds */
        x = nx; z = nz;
        bahn[t - 291][0] = x; bahn[t - 291][1] = z;
    }
}

static void griff_kette(int lauf)
{
    /* Platzierungskette T265-T290: Bezug / Eingang (nach Platzierung + Schub) / Ausgang der Klemme, Port gegen Original */
    for (int t = 265; t <= 290; t++) {
        const int32_t *w = s_wand_orig[t - 247];                 /* vs = 9641 + 2T, Tabelle ab vs10135 = T247 */
        const int32_t *q = s_gl_st[lauf][t - GRIFF_T0];
        printf("    %sT%d Bezug (%d,%d)/(%d,%d) Eingang (%d,%d)/(%d,%d) d%.0f Ausgang (%d,%d)/(%d,%d) d%.0f  [Port/Original]\n",
               lauf ? "[Weg 2] " : "", t, (int)q[0], (int)q[1], (int)w[1], (int)w[2], (int)q[2], (int)q[3], (int)w[3], (int)w[4],
               dist2d(q[2], q[3], w[3], w[4]), (int)q[4], (int)q[5], (int)w[5], (int)w[6], dist2d(q[4], q[5], w[5], w[6]));
    }
}

/* DIAGNOSE (Nachbesserung 3, nicht in ctest): Fuss-Sperren-Locator je Bild, Wurzel einfach (s_pose enthaelt die
 * Keyframe-Wurzel, skeleton_common.c root trans = kf_px/kf_pz) gegen doppelt (+= rx/rz wie re15_maggot_footlock). */
static void teil_fuss(void)
{
    if (!bank_laden_27()) { PRUEF(0, "EM027-Bank"); return; }
    re15_enemy_bank_t *gb = re15_enemy_find(0x27);
    const re15_emd_skeleton_t *sk = &gb->skel; const re15_emd_animation_t *an = &gb->anim;
    static const int clips[2] = { 3, 5 }, bones[2] = { 14, 17 };
    static re15_skel_pose_t ps[RE15_EMD_MAX_BONES];
    for (int ci = 0; ci < 2; ci++) {
        const re15_emd_clip_t *c = &an->clips[clips[ci]];
        int bone = bones[ci];
        int32_t l1p[3] = {0,0,0}, l2p[3] = {0,0,0};
        for (int k = 0; k <= c->frame_count; k++) {
            int s_ = k % c->frame_count;
            int kf = (int)(an->frames[c->first_frame + s_] & 0xFFFu);
            int16_t rx = 0, ry = 0, rz = 0;
            re15_affen_pose_abfrage(sk, kf, ps);
            re15_emd_get_keyframe_position(sk, kf, &rx, &ry, &rz);
            int32_t l1[3] = { ps[bone].trans[0], ps[bone].trans[1], ps[bone].trans[2] };
            int32_t l2[3] = { l1[0] + rx, l1[1], l1[2] + rz };
            if (k > 0)
                printf("    clip %d bone %d Bild %d kf %d: Wurzel (%d,%d) Locator einfach (%d,%d) d(%d,%d) | doppelt d(%d,%d)\n", clips[ci], bone, k, kf,
                       (int)rx, (int)rz, (int)l1[0], (int)l1[2], (int)(l1[0] - l1p[0]), (int)(l1[2] - l1p[2]), (int)(l2[0] - l2p[0]), (int)(l2[2] - l2p[2]));
            l1p[0] = l1[0]; l1p[2] = l1[2]; l2p[0] = l2[0]; l2p[2] = l2[2];
        }
    }
}

static void teil_griff(void)
{
    griff_lauf(0);
    /* ---- M2: Ritt des Greifers und Schub auf e2 ---- */
    double e1_max = 0, e2_max = 0; int e1_t = -1, e2_t = -1;
    for (int t = 253; t <= 268; t++) {
        const int32_t *q = s_e12_orig[t - 250]; int i = t - GRIFF_T0;
        printf("    T%d e1 Port (%d,%d) r%d Original (%d,%d) r%d d%.0f | e2 Port (%d,%d) r%d Original (%d,%d) r%d d%.0f | e1-e2 %.0f\n", t,
               (int)s_gl_e1[0][i][0], (int)s_gl_e1[0][i][1], (int)s_gl_e1[0][i][2], (int)q[1], (int)q[2], (int)q[3],
               dist2d(s_gl_e1[0][i][0], s_gl_e1[0][i][1], q[1], q[2]),
               (int)s_gl_e2[0][i][0], (int)s_gl_e2[0][i][1], (int)s_gl_e2[0][i][2], (int)q[4], (int)q[5], (int)q[6],
               dist2d(s_gl_e2[0][i][0], s_gl_e2[0][i][1], q[4], q[5]),
               dist2d(s_gl_e1[0][i][0], s_gl_e1[0][i][1], s_gl_e2[0][i][0], s_gl_e2[0][i][1]));
    }
    for (int t = 254; t <= 301; t++) {
        const int32_t *q = s_e12_orig[t - 250]; int i = t - GRIFF_T0;
        double d1 = dist2d(s_gl_e1[0][i][0], s_gl_e1[0][i][1], q[1], q[2]);
        if (d1 > e1_max) { e1_max = d1; e1_t = t; }
        if (t <= 264) { double d2 = dist2d(s_gl_e2[0][i][0], s_gl_e2[0][i][1], q[4], q[5]); if (d2 > e2_max) { e2_max = d2; e2_t = t; } }
    }
    double e2_weg = dist2d(s_gl_e2[0][262 - GRIFF_T0][0], s_gl_e2[0][262 - GRIFF_T0][1], s_gl_e2[0][254 - GRIFF_T0][0], s_gl_e2[0][254 - GRIFF_T0][1]);
    printf("  M2: e1 T254-T301 hoechstens %.0f (T%d) neben dem Original; e2 T254-T264 hoechstens %.0f (T%d); e2-Weg T254->T262 %.0f (Original 542)\n",
           e1_max, e1_t, e2_max, e2_t, e2_weg);
    {
        double e1_ritt = 0, kmin = 1e9, kmax = 0;
        for (int t = 254; t <= 288; t++) {           /* Ritt bis vor den Koerperkontakt mit e2 (ab T289, Rest = OFFEN) */
            const int32_t *q = s_e12_orig[t - 250]; int i = t - GRIFF_T0;
            double d1 = dist2d(s_gl_e1[0][i][0], s_gl_e1[0][i][1], q[1], q[2]); if (d1 > e1_ritt) e1_ritt = d1;
        }
        for (int t = 256; t <= 264; t++) {
            int i = t - GRIFF_T0;
            double k = dist2d(s_gl_e1[0][i][0], s_gl_e1[0][i][1], s_gl_e2[0][i][0], s_gl_e2[0][i][1]);
            if (k < kmin) kmin = k; if (k > kmax) kmax = k;
        }
        PRUEF(e1_ritt <= 40.0, "M2: der Greifer reitet die Clip-0x1c-Bahn (Yaw-Fang a8f8 0x800 @0x8011acac, ad68(g_entity) @0x8011accc,"
              " Paar ohne aec4 @0x8002af14): e1 T254-T288 hoechstens %.0f neben dem Original (Schranke = Anker-Versatz 39; vorher bis 844)", e1_ritt);
        PRUEF(kmin >= 3150.0 && kmax <= 3230.0 && e2_weg >= 400.0, "M2: e1 schiebt e2 (b544): Abstand e1-e2 T256-T264 %.0f..%.0f (Original"
              " 3168..3209 = 2 x 1600), e2-Weg T254->T262 %.0f (Original 542; vorher 4 = e2 stand still)", kmin, kmax, e2_weg);
    }
    griff_kette(0);

    /* ---- M1: die Wandklemme allein ab T290 ---- */
    static int32_t bahn[124][2];
    klemme_iter(-5381, -10551, 414, bahn);                       /* Original-Zustand T290 (s_wurf_orig[36]) */
    int gleich = 0, erste = -1;
    for (int t = 291; t <= 414; t++) {
        if (bahn[t - 291][0] == s_wurf_orig[t - 254][0] && bahn[t - 291][1] == s_wurf_orig[t - 254][1]) gleich++;
        else if (erste < 0) erste = t;
    }
    PRUEF(gleich == 124, "Port-Klemme ab dem Original-Zustand T290 (-5381,-10551) iteriert: %d/124 Bilder T291-T414 bitgleich mit der Original-Bahn"
          " (erste Abweichung T%d); Freigabe T378 (%d,%d) (Original (-4759,-10633)), T410 (%d,%d) (Original-Ruhelage (-3009,-11643))",
          gleich, erste, (int)bahn[378 - 291][0], (int)bahn[378 - 291][1], (int)bahn[410 - 291][0], (int)bahn[410 - 291][1]);
    /* Zerlegung des Riegel-Laufs: Freigabe = Iteration ab der EIGENEN T290-Lage */
    {
        int32_t px = s_gl_pl[0][290 - GRIFF_T0][0], pz = s_gl_pl[0][290 - GRIFF_T0][1];
        static int32_t pb[124][2];
        klemme_iter(px, pz, 414, pb);
        int g = 0;
        for (int t = 291; t <= 378; t++)
            if (pb[t - 291][0] == s_gl_pl[0][t - GRIFF_T0][0] && pb[t - 291][1] == s_gl_pl[0][t - GRIFF_T0][1]) g++;
        PRUEF(g == 88 && pb[378 - 291][0] == s_gl_frei_xy[0][0] && pb[378 - 291][1] == s_gl_frei_xy[0][1],
              "Riegel-Lauf T291-T378 = Klemmen-Iteration ab seiner eigenen T290-Lage (%d,%d): %d/88 Bilder gleich, Freigabe (%d,%d)"
              " (Riegel (%d,%d)) -> die Freigabe-Abweichung ist allein der Startversatz bei T290 (%.0f zum Original)",
              (int)px, (int)pz, g, (int)pb[378 - 291][0], (int)pb[378 - 291][1], (int)s_gl_frei_xy[0][0], (int)s_gl_frei_xy[0][1],
              dist2d(px, pz, -5381, -10551));
    }
    /* Empfindlichkeit: 24 Starts im 5x5-Gitter (+-1/+-2 Einheiten) um den Original-Zustand T290 + die beiden Port-Staende
     * vor/nach A1 (jnb2/griff_nach4: T290 (-5374,-10724) -> frei (-4580,-10518); griff_nach5: (-5376,-10728) -> (-5433,-10693)) */
    {
        static const int32_t hist[2][2] = { { -5374, -10724 }, { -5376, -10728 } };
        int n100 = 0, n400 = 0, n0 = 0; double dmax = 0;
        static int32_t sb[124][2];
        for (int k = 0; k < 26; k++) {
            int32_t sx, sz;
            if (k < 24) { int j = k < 12 ? k : k + 1; sx = -5381 + (j % 5) - 2; sz = -10551 + (j / 5) - 2; }
            else { sx = hist[k - 24][0]; sz = hist[k - 24][1]; }
            klemme_iter(sx, sz, 414, sb);
            double d378 = dist2d(sb[378 - 291][0], sb[378 - 291][1], -4759, -10633);
            int div = -1, ruhe = -1;
            for (int t = 291; t <= 414; t++) {
                if (div < 0 && dist2d(sb[t - 291][0], sb[t - 291][1], s_wurf_orig[t - 254][0], s_wurf_orig[t - 254][1]) > 50.0) div = t;
                if (ruhe < 0 && t >= 293 && sb[t - 291][0] == sb[t - 292][0] && sb[t - 291][1] == sb[t - 292][1] &&
                    sb[t - 292][0] == sb[t - 293][0] && sb[t - 292][1] == sb[t - 293][1]) ruhe = t - 2;
            }
            printf("    Start (%d,%d) [%+d,%+d]: >50 vom Original ab T%d, T378 (%d,%d) d%.0f, Ruhe ab T%d bei (%d,%d)\n", (int)sx, (int)sz,
                   (int)(sx + 5381), (int)(sz + 10551), div, (int)sb[378 - 291][0], (int)sb[378 - 291][1], d378, ruhe,
                   ruhe > 0 ? (int)sb[ruhe - 291][0] : 0, ruhe > 0 ? (int)sb[ruhe - 291][1] : 0);
            if (k < 24) { if (d378 > 100) n100++; if (d378 > 400) n400++; if (d378 == 0) n0++; if (d378 > dmax) dmax = d378; }
        }
        printf("  M1 Empfindlichkeit: von 24 Starts 1-2 Einheiten neben dem Original-T290 enden bei T378 %d gleich, %d > 100 und %d > 400"
               " Einheiten neben der Original-Freigabe (hoechstens %.0f)\n", n0, n100, n400, dmax);
        PRUEF(n0 == 0 && n100 == 24, "M1: die Klemmen-Iteration ist empfindlich — alle 24 Starts 1-2 Einheiten neben dem Original-T290"
              " enden bei T378 mehr als 100 neben der Original-Freigabe (%d/24, %d davon > 400)", n100, n400);
        int32_t h1[124][2], h2[124][2];
        klemme_iter(hist[0][0], hist[0][1], 414, h1); klemme_iter(hist[1][0], hist[1][1], 414, h2);
        PRUEF(h1[378 - 291][0] == -4580 && h1[378 - 291][1] == -10518 && h1[392 - 291][0] == -3018 && h1[392 - 291][1] == -11651 &&
              h2[378 - 291][0] == -5433 && h2[378 - 291][1] == -10693,
              "M1: die Dossier-Zahlen von Nachbesserung 2 (frei (-4580,-10518), Ruhe (-3018,-11651)) gehoeren zum T290-Stand (-5374,-10724)"
              " vor den A1-Aenderungen; derselbe Code ab (-5376,-10728) (2/4 Einheiten weiter) gibt frei (%d,%d)", (int)h2[378 - 291][0], (int)h2[378 - 291][1]);
    }

    /* ---- Weg 2: Startversatz entfernt (Leon/e1/e2 am Ende von T253 = Original) ---- */
    griff_lauf(1);
    printf("  Anker beim Pin: Lauf 0 (%d,%d), Weg 2 (%d,%d) (Original (-7507,-10327), jnb2/g_wer.txt)\n",
           (int)s_gl_anker[0][0], (int)s_gl_anker[0][1], (int)s_gl_anker[1][0], (int)s_gl_anker[1][1]);
    griff_kette(1);
    printf("  [Weg 2] T290 (%d,%d) (Original (-5381,-10551)), frei T%d bei (%d,%d) (Original T378 (-4759,-10633))\n",
           (int)s_gl_pl[1][290 - GRIFF_T0][0], (int)s_gl_pl[1][290 - GRIFF_T0][1], s_gl_frei[1], (int)s_gl_frei_xy[1][0], (int)s_gl_frei_xy[1][1]);
    {
        int ritt_ok = s_gl_e1[1][254 - GRIFF_T0][0] == -5882 && s_gl_e1[1][254 - GRIFF_T0][1] == -14389 && s_gl_e1[1][254 - GRIFF_T0][2] == 2823 &&
                      s_gl_e1[1][255 - GRIFF_T0][0] == -5953 && s_gl_e1[1][255 - GRIFF_T0][1] == -14268;
        PRUEF(ritt_ok, "Weg 2 / M2: e1 T254 (%d,%d) r%d, T255 (%d,%d) = Original (-5882,-14389) r2823 / (-5953,-14268) — Yaw-Fang @0x8011acac +"
              " Ritt-Platzierung @0x8011accc bitgleich", (int)s_gl_e1[1][254 - GRIFF_T0][0], (int)s_gl_e1[1][254 - GRIFF_T0][1],
              (int)s_gl_e1[1][254 - GRIFF_T0][2], (int)s_gl_e1[1][255 - GRIFF_T0][0], (int)s_gl_e1[1][255 - GRIFF_T0][1]);
        int kette = 0;
        for (int t = 268; t <= 290; t++) {
            const int32_t *w = s_wand_orig[t - 247]; const int32_t *q = s_gl_st[1][t - GRIFF_T0];
            if (q[2] == w[3] && q[3] == w[4] && q[4] == w[5] && q[5] == w[6]) kette++;
        }
        int bahn_w2 = 0;
        for (int t = 291; t <= 414; t++)
            if (s_gl_pl[1][t - GRIFF_T0][0] == s_wurf_orig[t - 254][0] && s_gl_pl[1][t - GRIFF_T0][1] == s_wurf_orig[t - 254][1]) bahn_w2++;
        PRUEF(s_gl_anker[1][0] == -7507 && s_gl_anker[1][1] == -10327 && kette == 23 && s_gl_frei[1] == 378 &&
              s_gl_frei_xy[1][0] == -4759 && s_gl_frei_xy[1][1] == -10633,
              "Weg 2 (Startversatz entfernt): Anker (%d,%d) = Original (-7507,-10327), Kette T268-T290 %d/23 Bilder bitgleich (Eingang und"
              " Ausgang der Klemme), Freigabe T%d bei (%d,%d) = Original T378 (-4759,-10633) (Lauf 0: Anker (%d,%d))",
              (int)s_gl_anker[1][0], (int)s_gl_anker[1][1], kette, s_gl_frei[1], (int)s_gl_frei_xy[1][0], (int)s_gl_frei_xy[1][1],
              (int)s_gl_anker[0][0], (int)s_gl_anker[0][1]);
        int n = GRIFF_N - 1;
        PRUEF(bahn_w2 == 124 && s_gl_pl[1][n][0] == -3009 && s_gl_pl[1][n][1] == -11643,
              "Weg 2: nach der Freigabe Leons Bahn T291-T414 %d/124 Bilder = Original, Ruhelage (-3009,-11643) ab T410, am Laufende T%d"
              " (%d,%d) wie das Original (g_griff F496: (-3009,-11643), HP 76)", bahn_w2, GRIFF_T0 + n,
              (int)s_gl_pl[1][n][0], (int)s_gl_pl[1][n][1]);
        double sprung = 0; int t_sprung = -1;
        for (int t = 331; t <= 372; t++) {          /* Sub 2 (Brustschlag-Rest, Clip 3 ab Bild 0x1d bis zum Wrap) */
            int i = t - GRIFF_T0;
            double d = dist2d(s_gl_e1[1][i][0], s_gl_e1[1][i][1], s_gl_e1[1][i - 1][0], s_gl_e1[1][i - 1][1]);
            if (d > sprung) { sprung = d; t_sprung = t; }
        }
        PRUEF(sprung <= 100.0, "Fusssperre mit der Pool-Pose (FUN_8001f3bc Pose vor +0x95++ @0x8001f40c/@0x8001f610-1c, bf50 @0x8011bf80-c008):"
              " e1 in Sub 2 T331-T372 hoechstens %.0f je Bild (T%d) — vorher ~1300 im Clip-3-Wrap-Bild T370 (Original F370 -> F371: (4,-4));"
              " Rest am Sub-2-Eintritt (Port bis 64, Original bis 34) = OFFEN Frac-Mischung der Pool-Pose", sprung, t_sprung);
    }

    /* ---- N5 (P2): N1-Mechanismus am Original-Zustand; Empfindlichkeit des Lauf-0-Ausgangs (kein Pin darauf) ---- */
    PRUEF(s_gl_biss[1] < 0 && fabs(s_gl_e12[1][0] - 3925.0) <= 250.0 && fabs(s_gl_e12[1][1] - 4990.0) <= 250.0,
          "N1 (Weg 2 = Original-Zustand): an der Original-Ruhelage kein Biss bis T495 (erster Biss T%d), e1 %.0f / e2 %.0f entfernt"
          " (Original g_griff F496: HP 76, ~3925 / ~4990; A[3]-Gate @0x80117a54-90 sperrt ausser Reichweite)", s_gl_biss[1],
          s_gl_e12[1][0], s_gl_e12[1][1]);
    {
        const int32_t bx = s_gl_pl[0][290 - GRIFF_T0][0], bz = s_gl_pl[0][290 - GRIFF_T0][1];
        const int32_t lx = s_gl_pl[0][GRIFF_N - 1][0], lz = s_gl_pl[0][GRIFF_N - 1][1];
        const int lbiss = s_gl_biss[0];
        int n_ruhe = 0, n_biss = 0, n_gleich = 0;
        for (int k = 0; k < 24; k++) {               /* 5x5-Gitter +-1/+-2 ohne die Mitte, wie M1 */
            int j = k < 12 ? k : k + 1, vx = (j % 5) - 2, vz = (j / 5) - 2;
            griff_lauf_v(0, vx, vz);
            int32_t ex = s_gl_pl[2][GRIFF_N - 1][0], ez = s_gl_pl[2][GRIFF_N - 1][1];
            int ruhe = dist2d(ex, ez, -3009, -11643) <= 60.0;
            n_ruhe += ruhe; n_biss += (s_gl_biss[2] >= 0); n_gleich += (ex == lx && ez == lz && s_gl_biss[2] == lbiss);
            printf("    N5 Start T290 (%d,%d) [%+d,%+d]: frei T%d (%d,%d), Ende (%d,%d)%s, erster Biss T%d\n", (int)(bx + vx), (int)(bz + vz),
                   vx, vz, s_gl_frei[2], (int)s_gl_frei_xy[2][0], (int)s_gl_frei_xy[2][1], (int)ex, (int)ez, ruhe ? " = Ruhelage" : "", s_gl_biss[2]);
        }
        printf("  N5 Empfindlichkeit Lauf 0: von 24 Starts 1-2 Einheiten neben der Lauf-0-Lage T290 (%d,%d) enden %d an der Original-Ruhelage,"
               " %d werden bis T495 gebissen, %d gleich wie Lauf 0 (Ende (%d,%d), Biss T%d)\n", (int)bx, (int)bz, n_ruhe, n_biss, n_gleich,
               (int)lx, (int)lz, lbiss);
        PRUEF(n_gleich < 24 && (n_ruhe > 0 || n_biss > 0) && (n_ruhe < 24 || n_biss < 24),
              "N5 (P2): der Ausgang von Lauf 0 ist empfindlich — von 24 Starts 1-2 Einheiten daneben enden %d gleich wie Lauf 0, %d an der"
              " Original-Ruhelage, %d mit Biss bis T495; ein Pin auf Ruhelage/Biss in Lauf 0 misst keinen Mechanismus (Nachbesserung 3, Z. 1217)",
              n_gleich, n_ruhe, n_biss);
    }
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
/* Nachbesserung 6, M1: der Finisher B[8] (Treffer im Sprung, -600 @0x801191a8-ac) fuehrt Leon durch den cmd-6-Hook
 * 0x8011c3d4 -> 0x8011c414 (re15_affen.h (15)), NICHT durch den Wurf 0x8011c118. Gemessen wird die Mechanik je Bild:
 * Opfer-Zustand 2 / Greifer 0x27, Clip 0 der Opfer-Bank (acae8 := 0 @0x8011c490) vorwaerts ab Bild 0 (@0x8011c498),
 * Blend-Saat +0x8f = 7 (0x800acae3 @0x8011c468-70) und Abbau 7, 6, ..., 0 je f314, +0x93 Bit 1 aus B[8]
 * (@0x801191d0-fc), Eintritt im Treffer-Bild, Koerperfall + Blut bei Bild 0x3c (@0x8011c4e0-518), Tod
 * (Wunden + Leiche @0x8011c55c-84) im Bild NACH dem letzten Clip-Bild, und KEINE Platzierung: Leons Lage ist in
 * jedem Bild bis zum Tod am Bildende gleich der am Bildanfang (kein 0x8001ad68 im Hook). Abnahme 5 (w3y F2814) mass
 * am alten Stand einen Sprung um 14000 Einheiten; mit abgeschalteten Haken ist dieser Riegel rot (gemessen: Leon bis
 * 15116 vom Treffer-Ort, Clip 1 dann 0xb statt 0, +0x93 = 1, kein Tod). */
static void teil_finisher(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -9975, -10422, 1024, 5, 3) != 0) return;
    PRUEF(bank_laden_27(), "EM027-Bank geladen (Opfer-Bank fuer den Finisher)");
    re15_enemy_bank_t *eb = re15_enemy_find(0x27);
    int fc = (eb && eb->victim_ok && eb->anim_victim.clip_count > 0) ? eb->anim_victim.clips[0].frame_count : 0;
    PRUEF(fc > 0x3c, "Opfer-Bank Clip 0 hat %d Bilder (> 0x3c: der Koerperfall @0x8011c4e0 ist erreichbar)", fc);
    re15_actor_t *a = aktor_vom_typ(0x27, 0), *b = aktor_vom_typ(0x27, 1);
    if (!a || !b || fc <= 0) { PRUEF(0, "zwei Gorillas im Kampf-Layout"); return; }
    b->grid_id |= 0x20; b->x = 30000; b->z = 30000;
    pl->x = -9975; pl->z = -10422; pl->y = 0; pl->rot_y = 587; pl->hp = 46; pl->hit_react = 0; pl->state = 1; pl->sub_state_1 = 0;
    re15_player_cmd_zero();
    /* Der echte Weg in den Finisher: Gorilla im LEAP B[7] (Phase 0), Leon vor ihm mit hp 46 < 50 -> Commit B[8] in
     * Phase 2 bei Bild 0x13 (@0x80118cc8-d68), Treffer im B[8]-Fenster Bild 8..0xa (@0x80121478). Yaw 3072 = +Z. */
    a->x = -5125; a->z = -14706; a->y = 0; a->rot_y = 2517; a->grid_id = 0x10; a->floor = 0;   /* w3y F2786 */
    a->state = 1; a->sub_state_1 = 7; a->sub_state_2 = 0; a->sub_state_3 = 0; a->hit_react = 0; a->crow_speed = 0;
    a->mag_boost = 4; a->mag_airborne = 0; a->dog_blocked_ctr = 0;
    int t_hit = -1, t_tod = -1, f_fall = -1, n_tick_weg = 0, max_weg = 0, n_clip_falsch = 0, n_bild_falsch = 0;
    int pl_x0 = 0, pl_z0 = 0, hr_nach = -1, typ = -1, eigen = -1, bild_vorher = -1, frac[10], nfrac = 0;
    for (int f = 0; f < 200; f++) {
        int32_t x0 = pl->x, z0 = pl->z;
        int hp0 = pl->hp;
        re15_schritt_station_reset();
        frame(0, 0);
        int32_t tx = pl->x, tz = pl->z;
        re15_schritt_station_hole(RE15_SCHRITT_TICK, &tx, &tz);
        if (getenv("R35_FIN_SPUR") && f < 120)
            printf("  S%-3d G st %d/%d/%d c%d/%d (%d,%d,%d) r%d | PL (%d,%d) hp %d hr %d c%d/%d vs %d\n", f, (int)a->state, (int)a->sub_state_1,
                   (int)a->sub_state_2, (int)a->motion, (int)a->anim_frame, (int)a->x, (int)a->y, (int)a->z, (int)a->rot_y, (int)pl->x, (int)pl->z,
                   (int)pl->hp, (int)pl->hit_react, (int)pl->motion, (int)pl->anim_frame, re15_player_victim_state());
        if (t_hit < 0 && hp0 >= 0 && pl->hp < 0) {
            t_hit = f; pl_x0 = x0; pl_z0 = z0; hr_nach = pl->hit_react;
            for (int k = 0; k < 10; k++) frac[k] = -1;
            typ = re15_player_victim_type(); eigen = re15_player_victim_own_bank();
            printf("  Treffer T%d: hp %d -> %d, Leon (%d,%d), Gorilla (%d,%d) Bild %d, Opfer-Zustand %d Typ 0x%02x\n", f, hp0,
                   (int)pl->hp, (int)x0, (int)z0, (int)a->x, (int)a->z, (int)a->anim_frame, re15_player_victim_state(), typ);
        }
        if (t_hit >= 0 && f - t_hit < 10) frac[nfrac++] = (int)pl->anim_frac;
        if (t_hit >= 0 && t_tod < 0) {
            if (pl->x != x0 || pl->z != z0) n_tick_weg++;   /* Bildanfang -> Bildende (der Opfer-Tick laeuft nach der KI) */
            int dg = (int)dist2d(pl->x, pl->z, pl_x0, pl_z0); if (dg > max_weg) max_weg = dg;
            if (pl->motion != 0) n_clip_falsch++;
            if (pl->state != 7 && (int)pl->anim_frame != bild_vorher + 1 && !(f == t_hit && pl->anim_frame == 0)) n_bild_falsch++;   /* Todesbild: aca5a 2 ruft kein f314 */
            if (pl->anim_frame == 0x3c && f_fall < 0) f_fall = f;
            if (pl->state == 7) t_tod = f;
            if ((f - t_hit) % 10 == 0 || pl->state == 7 || pl->anim_frame == 0x3c)
                printf("  T%-3d Leon (%d,%d) Bildanfang (%d,%d) Clip %d Bild %d +0x8f %d hp %d Zustand %d\n", f, (int)pl->x,
                       (int)pl->z, (int)x0, (int)z0, (int)pl->motion, (int)pl->anim_frame, (int)pl->anim_frac, (int)pl->hp, (int)pl->state);
            bild_vorher = (int)pl->anim_frame;
        }
        if (t_tod >= 0 && f > t_tod + 5) break;
    }
    int ev[RE15_AFFEN_FIN_LOG_N], nev = re15_affen_finisher_ereignisse(ev, RE15_AFFEN_FIN_LOG_N);
    printf("  Ereignisse:"); for (int i = 0; i < nev; i++) printf(" 0x%04x", ev[i]); printf("\n");
    PRUEF(t_hit >= 0, "B[8] trifft (hp 46 -> %d in T%d)", (int)pl->hp, t_hit);
    if (t_hit < 0) return;
    PRUEF(typ == 0x27 && eigen == 0, "Opfer-Zustand mit Greifer 0x%02x, Opfer-Bank (eigene Bank %d = nein) — nicht der Wurf", typ, eigen);
    PRUEF((hr_nach & 1) != 0, "+0x93 = 0x%02x nach dem Treffer (Bit 1 aus B[8] @0x801191d0-fc; der Hook schreibt +0x93 nicht)", hr_nach);
    {   int ok = 1; for (int k = 0; k < 10; k++) if (frac[k] != (k < 8 ? 7 - k : 0)) ok = 0;
        PRUEF(ok, "+0x8f ab dem Treffer-Bild %d %d %d %d %d %d %d %d %d %d (Saat 7 @0x8011c468-70, -1 je f314 Decompilat FUN_8001f3bc Z. 78)",
              frac[0], frac[1], frac[2], frac[3], frac[4], frac[5], frac[6], frac[7], frac[8], frac[9]); }
    PRUEF(n_clip_falsch == 0, "Clip 0 der Opfer-Bank in jedem Bild bis zum Tod (%d Bilder mit anderem Clip; Wurf waere 1/0x10/0xb)", n_clip_falsch);
    PRUEF(n_bild_falsch == 0, "Bild 0, 1, 2, ... ohne Luecke ab dem Treffer-Bild (%d Abweichungen)", n_bild_falsch);
    PRUEF(n_tick_weg == 0, "keine Platzierung: Lage am Bildende = Bildanfang in jedem Bild bis zum Tod (%d Bilder bewegt; kein 0x8001ad68 im Hook)", n_tick_weg);
    PRUEF(max_weg <= 200, "Leon bleibt am Ort: groesste Entfernung vom Treffer-Ort %d (<= 200; Abnahme 5 alt: rund 14000)", max_weg);
    PRUEF(nev >= 3 && ev[0] == RE15_AFFEN_FIN_EINTRITT, "Eintritt im Treffer-Bild (Ereignis 0 = 0x%04x)", nev ? ev[0] : -1);
    PRUEF(nev >= 3 && ev[1] == (RE15_AFFEN_FIN_FALL | 0x3c) && f_fall == t_hit + 0x3c,
          "Koerperfall + zweites Blut bei Bild 0x3c = T%d (Treffer + 60 = T%d) @0x8011c4e0-518", f_fall, t_hit + 0x3c);
    PRUEF(nev >= 3 && ev[2] == (RE15_AFFEN_FIN_TOD | (fc - 1)) && t_tod == t_hit + fc,
          "Tod (Wunden + Leiche @0x8011c55c-84) in T%d = Bild nach dem letzten Clip-Bild %d (erwartet T%d), Endpose bleibt", t_tod, fc - 1, t_hit + fc);
    PRUEF(pl->state == 7 && pl->anim_frame == fc - 1 && pl->motion == 0 && re15_player_victim_state() == 2,
          "nach dem Tod: Zustand %d, Clip %d Bild %d gehalten, Opfer-Zustand %d", (int)pl->state, (int)pl->motion, (int)pl->anim_frame, re15_player_victim_state());
}

/* ---------------------------------------------------------------------------------------------- */
/* Nachbesserung 6, M2: der Blind-Zonen-Sprung von Gorilla 2 auf der Stelle (Abnahme 5 w3y: 9 Spruenge (-13518,-403) <->
 * (-13418,-403), F2524-F2844, Leon rund 800 Bilder nicht angegriffen) ist ORIGINAL-VERHALTEN. Gegenprobe im Original
 * (DuckStation-GDB, r3-Savestate s033, scratch jnb6/gdbm2.py -> g_m2b.txt): beim ersten e2-Halt der Gorilla-Wurzel
 * 0x80116db8 ab VSync 10029 (F194) e2 := (-13518,0,-403) Yaw 0, +0x4..+0x7 := 1/4/0/0, +0x82 := 0, +0x1d0 := 0; Leon :=
 * (-9975,-10422) r587; e1 weit weg. Ergebnis F195-F594: 10 Blind-Zonen-Spruenge (+0x7 = 3, Impuls 0x32a = 810), alle 40
 * Bilder, nur die zwei Lagen, LOS-Latch 0, Leon hp 82 unveraendert — streng periodisch (Periode 40, 0 Abweichungen).
 * Diese Periode (Lage, Hoehe, Sub/Phase/+0x7, Bild, +0x82 je Bild, Wurzel-Eintritt = Stand nach dem Vortick) steht unten;
 * der Port muss sie ab derselben Lage Bild fuer Bild treffen. Entscheidung (A[4] @0x80117f78-8011802c), Flug (B[7]
 * @0x80118a94-af0) und Klemme (FUN_8003b0a4 auf +0x82, @0x80116e70) sind dieselben. */
static const int16_t s_zone_orig[40][7] = {   /* x, y, +0x5, +0x6, +0x7, +0x95, +0x82 (z immer -403, Yaw immer 0) */
    {-13518,0,7,1,3,1,0}, {-13518,0,7,1,3,2,0}, {-13518,0,7,1,3,3,0}, {-13518,0,7,1,3,4,0}, {-13518,0,7,1,3,5,0},
    {-13518,0,7,1,3,6,0}, {-13518,0,7,1,3,7,0}, {-13518,0,7,1,3,8,0}, {-13518,0,7,1,3,9,0}, {-13418,0,7,2,3,10,1},
    {-13418,-720,7,2,3,11,1}, {-13418,-1380,7,2,3,12,1}, {-13418,-1980,7,2,3,13,1}, {-13418,-2520,7,2,3,14,1}, {-13418,-3000,7,2,3,15,1},
    {-13418,-3420,7,2,3,16,1}, {-13418,-3780,7,2,3,17,1}, {-13418,-4080,7,2,3,18,1}, {-13418,-4320,7,2,3,19,1}, {-13418,-4500,7,2,3,20,1},
    {-13418,-4620,7,2,3,21,1}, {-13418,-4680,7,2,3,22,1}, {-13418,-4680,7,2,3,23,1}, {-13418,-4620,7,2,3,24,1}, {-13418,-4500,7,2,3,25,1},
    {-13418,-4320,7,2,3,26,1}, {-13418,-4080,7,2,3,27,1}, {-13418,-3780,7,2,3,28,1}, {-13418,-3420,7,2,3,29,1}, {-13418,-3000,7,2,3,30,1},
    {-13418,-2520,7,2,3,31,1}, {-13418,-1980,7,2,3,32,1}, {-13418,-1380,7,2,3,33,1}, {-13418,-720,7,2,3,34,1}, {-13418,0,7,2,3,35,1},
    {-13518,0,7,3,0,36,0}, {-13518,0,7,3,0,37,0}, {-13518,0,7,3,0,38,0}, {-13518,0,7,3,0,39,0}, {-13518,0,4,0,0,0,0},
};
static void teil_zonensprung(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_game_state_init();
    re15_game_flag_set(4, 0x40, 1);
    if (room_boot(0x11C0, -9975, -10422, 587, 5, 3) != 0) return;
    PRUEF(bank_laden_27(), "EM027-Bank geladen");
    re15_actor_t *a = aktor_vom_typ(0x27, 0), *b = aktor_vom_typ(0x27, 1);
    if (!a || !b) { PRUEF(0, "zwei Gorillas im Kampf-Layout"); return; }
    a->grid_id |= 0x20; a->x = 30000; a->z = 30000;              /* e1 weit weg (Original: e1 bei (8000,15000), griff nicht ein) */
    pl->x = -9975; pl->z = -10422; pl->y = 0; pl->rot_y = 587; pl->hp = 82; pl->hit_react = 0; pl->state = 1; pl->sub_state_1 = 0;
    re15_player_cmd_zero();
    b->x = -13518; b->y = 0; b->z = -403; b->rot_y = 0; b->grid_id = 0x10; b->floor = 0;
    b->state = 1; b->sub_state_1 = 4; b->sub_state_2 = 0; b->sub_state_3 = 0; b->motion = 5; b->anim_frame = 15;   /* Original F194: c5/15 */
    b->anim_frac = 0; b->hit_react = 0; b->dog_flags = 0; b->mag_boost = 4; b->mag_airborne = 0; b->crow_speed = 188;
    int n_gleich = 0, erste = -1, n_spr = 0, hp_min = pl->hp, n_bild = 400;
    for (int i = 0; i < n_bild; i++) {
        frame(0, 0);
        const int16_t *o = s_zone_orig[i % 40];
        int gleich = (b->x == o[0] && b->y == o[1] && b->z == -403 && b->rot_y == 0 && b->state == 1 && b->sub_state_1 == o[2]
                      && b->sub_state_2 == o[3] && b->sub_state_3 == o[4] && b->anim_frame == o[5] && b->floor == o[6]);
        if (gleich) n_gleich++;
        else if (erste < 0) {
            erste = i;
            printf("  erste Abweichung F%d: Port (%d,%d,%d) r%d s%d/%d/%d/%d f%d b%d, Original (%d,%d,-403) r0 s1/%d/%d/%d f%d b%d\n",
                   195 + i, (int)b->x, (int)b->y, (int)b->z, (int)b->rot_y, (int)b->state, (int)b->sub_state_1, (int)b->sub_state_2,
                   (int)b->sub_state_3, (int)b->anim_frame, (int)b->floor, o[0], o[1], o[2], o[3], o[4], o[5], o[6]);
        }
        if (b->sub_state_1 == 7 && b->sub_state_2 == 1 && b->sub_state_3 == 3 && b->anim_frame == 1) n_spr++;
        if (pl->hp < hp_min) hp_min = pl->hp;
    }
    printf("  Port F195-F%d: %d/%d Bilder gleich dem Original, %d Blind-Zonen-Anlaeufe, Leon hp min %d\n", 194 + n_bild, n_gleich, n_bild,
           n_spr, hp_min);
    PRUEF(n_gleich == n_bild, "Blind-Zonen-Schleife Bild fuer Bild wie das Original (%d/%d; Lage, Hoehe, Sub/Phase/+0x7, Bild, +0x82)", n_gleich, n_bild);
    PRUEF(n_spr == 10, "10 Spruenge in 400 Bildern, alle 40 Bilder (Original 10; %d)", n_spr);
    PRUEF(hp_min == 82, "Leon wird dabei nicht angegriffen (hp min %d, Original 82)", hp_min);
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
    if (!strcmp(teil, "wand")   || !strcmp(teil, "alle")) teil_wand();
    if (!strcmp(teil, "szene")  || !strcmp(teil, "alle")) teil_szene();
    if (!strcmp(teil, "finisher") || !strcmp(teil, "alle")) teil_finisher();
    if (!strcmp(teil, "zonensprung") || !strcmp(teil, "alle")) teil_zonensprung();
    if (!strcmp(teil, "fuss")) teil_fuss();   /* Diagnose, nicht in ctest */
    printf("test_r35_affen %s: %s (%d Fehler)\n", teil, g_fail ? "FEHLER" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
