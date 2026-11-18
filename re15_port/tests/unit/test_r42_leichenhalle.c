/**
 * @file test_r42_leichenhalle.c
 * @brief Runde 42 Strang "original", Stufe 1 — Leichenhalle ROOM1200/1201: die Kuehlfach-Tuer klappert, der Zombie kriecht
 *        heraus (wie die 40%-Disc). Herleitung und Belege: include/re15_leichenhalle_r42.h.
 *
 * NUTZERWUNSCH 18.11.2026: "Im Original unmodifzierten 40% build klappert dort noch die eine Tür in der Leichenhalle und der
 * Zombie kriecht dann raus. Das ist in der Mod Version aus irgendeinen Grund nicht mehr so.... Das soll wieder rein!"
 *
 * Der Riegel benutzt NUR Schnittstellen, die es schon vor dem Bau gab (scd_room_reenter, scd_vm_tick, re15_game_step,
 * g_scd.props, g_actors) — er laeuft also auch auf dem Altstand aaea5f8c2 und muss dort ROT sein (Gegenprobe).
 *   A  echter Weg je Figur (ROOM1200 Leon, ROOM1201 Elza; die Mod-Dateien sind gleich): Eintritt durch die Tuer aus ROOM11E0
 *      (Ziel (-20154,0,-25245), ROOM11E0.RDT @0x01532), (4,17)=0. Ausserhalb des Disc-Rechtecks passiert nichts.
 *      Spieler ins Rechteck (Disc main00 @0x007D2: x -26590..-25180, z -17180..-16030, Etage 0):
 *        - obj 1 (die Kuehlfach-Tuer) fährt GENAU die 20 Pos_set-z-Werte der Disc ab (sub02 @0x00922..@0x009A0):
 *          4x (-14402, -14362), (-14404, -14360), 2x (-14404, -14360), 3x (-14404, -14360)
 *        - danach dreht die Tuer (rot_y != 1024) und Gegner 2 (Sce_em_set id 2, Mod @0x0087E) bekommt grid 0x81
 *          (Disc @0x009D0 `34 0c 81 00`); am Ende (4,17) = 1 (Disc @0x00A1E `22 04 11 01`).
 *      Elza: zusaetzlich steht die Tuer nach dem ersten Bild auf rot (0,1024,0) (Disc ROOM1201 @0x0091C Dir_set).
 *   B  Etage 1 im Rechteck loest nicht aus (rec[2] = 0, Gate @0x80042ccc).
 *   C  Mod-sub03 nach dem Disc-Ablauf (Minidisc aufgenommen, (9,104)=1): die Tuer schwingt KEIN zweites Mal (rot_y bleibt),
 *      Gegner 2 behaelt seinen Zustand; die Liegenden (0/1/3) werden wie in der Mod geweckt.
 *   D  (4,17)=1 beim Betreten: kein Ausloeser (Disc main00 @0x007CE).
 *   E  der Kriecher verlaesst das Fach: nach grid 0x81 bewegt sich Gegner 2 aus dem Fach (z unter die Tuerlinie -14382)
 *      und landet auf der Spieler-Ebene (y 0, Etage 0) — in BEIDEN KI-Geschmaeckern, Spieler vor den Faechern.

 *   F  Wiederbetreten nach dem Ablauf ohne Minidisc (Stufe 1 Runde 2, Pruefbericht R1): Hinterhalt bleibt, s. wieder().
 *   G  Minidisc ZUERST, danach sofort in die Flaeche (Stufe 1 Runde 3, Pruefbericht Runde 2 R1): kein zweiter Ablauf, s. md_zuerst().
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Zombie-Baenke wie probe_1010_kriecher.c (RE2 aus RE2/CDEMD0.EMS, RE1.5 aus EMD/CDEMD0.EMS). */
static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}
static uint8_t *s_re15_ems = NULL; static size_t s_re15_n = 0;
static uint8_t  s_blob[0x80000];
static int load_bank_re15(uint8_t type)
{
    if (!s_re15_ems) s_re15_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_re15_n);
    if (!s_re15_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_re15_ems, s_re15_n, idx, &off, &len) != 0 || len > sizeof s_blob) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob, s_re15_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return 0; }
    eb->ok = 1; eb->buf = NULL;
    re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own);
    eb->own_ok = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    return 1;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* Disc-Ruhelage und Pos_set-Folge der Kuehlfach-Tuer (obj 1): Disc sub02 @0x00922..@0x009A0. */
#define TUER_X   (-25182)
#define TUER_Y   (-1818)
#define TUER_Z   (-14382)
static const int32_t k_klapper_z[20] = {
    -14402, -14362, -14402, -14362, -14402, -14362, -14402, -14362,     /* For 4 @0x0091C: be c7 / e6 c7 */
    -14404, -14360,                                                     /* @0x00940 / @0x0094E: bc c7 / e8 c7 */
    -14404, -14360, -14404, -14360,                                     /* For 2 @0x0095C */
    -14404, -14360, -14404, -14360, -14404, -14360,                     /* For 3 @0x00980 */
};

static int obj1(void)
{
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_id == 1) return i;
    return -1;
}
static int em_slot(int32_t x, int32_t z)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10 && g_actors[s].x == x && g_actors[s].z == z) return s;
    return -1;
}

/* bit17 / md: 0 oder 1 setzen, -1 = stehen lassen (Wiederbetreten, Teil F). */
static void raum_betreten_mit(uint16_t room, re15_ai_flavor_t fl, int bit17, int md)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(fl);
    /* Wiederbetreten (bit17 < 0): scd_vm_init loescht g_game.flags (re15_game_state_init) wie ein neues Spiel - der Riegel
     * stellt sie danach wieder her, so wie ein Raumwechsel sie stehen laesst (scd_room_reenter fasst sie nicht an). */
    static uint32_t merk[sizeof g_game.flags / sizeof(uint32_t)];
    if (bit17 < 0) memcpy(merk, g_game.flags, sizeof g_game.flags);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    if (bit17 < 0) memcpy(g_game.flags, merk, sizeof g_game.flags);
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x1200u);
    g_current_room_id = room;
    if (bit17 >= 0) re15_game_flag_set(4, 17, (uint8_t)bit17);
    if (md >= 0)    re15_game_flag_set(9, 104, (uint8_t)md);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -20154; pl->z = -25245; pl->rot_y = 3040;        /* ROOM11E0.RDT @0x01532 Door_aot_set slot 1 -> 1200 */
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    if (fl == RE15_AI_FLAVOR_RE2) (void)load_bank_re2(0x10); else (void)load_bank_re15(0x10);
}
static void raum_betreten(uint16_t room, re15_ai_flavor_t fl, int bit17) { raum_betreten_mit(room, fl, bit17, 0); }

/* A + C + E je Figur und Geschmack. */
static void lauf(uint16_t room, re15_ai_flavor_t fl, const char *name)
{
    raum_betreten(room, fl, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int o = obj1();
    const int k = em_slot(-24618, -13054);                    /* Gegner 2, Mod main00 @0x0087E */
    CHECK(o >= 0, "[%s] obj 1 (Kuehlfach-Tuer) fehlt", name);
    CHECK(k > 0, "[%s] Gegner 2 im Fach (-24618,-1800,-13054) fehlt", name);
    if (o < 0 || k <= 0) return;
    re15_actor_t *z = &g_actors[k];
    CHECK(z->grid_id == 0xA1 && z->y == -1800 && z->floor == 1, "[%s] Spawn grid %02x y %d b%d (erwartet a1/-1800/1)",
          name, z->grid_id, z->y, z->floor);

    /* ausserhalb: 60 Bilder am Eintritt */
    for (int f = 0; f < 60; f++) { pl->x = -20154; pl->z = -25245; frame_step(); }
    CHECK(g_scd.props[o].z == TUER_Z && z->grid_id == 0xA1, "[%s] vor dem Rechteck: Tuer z %d grid %02x", name,
          g_scd.props[o].z, z->grid_id);

    /* ins Rechteck */
    int32_t folge[64]; int nf = 0; int32_t zalt = g_scd.props[o].z;
    int bild_81 = -1, bild_17 = -1, bild_dreh = -1, bild_raus = -1, bild_boden = -1;
    int dir_1201 = -1;
    for (int f = 0; f < 1500; f++) {
        pl->hp = 100;
        if (f < 260) { pl->x = -25885; pl->z = -16605; }                          /* im Rechteck, Blick egal */
        else         { pl->x = -24600; pl->z = -15700 - ((f / 90) & 1) * 600; }   /* vor die Faecher (Wand Zelle 0 z -14753), hin und her */
        pl->floor = 0; pl->y = 0;
        frame_step();
        if (f == 0) dir_1201 = g_scd.props[o].rot_y;
        if (g_scd.props[o].z != zalt) { if (nf < 64) folge[nf++] = g_scd.props[o].z; zalt = g_scd.props[o].z; }
        if (bild_dreh < 0 && g_scd.props[o].rot_y != 1024) bild_dreh = f;
        if (bild_81 < 0 && z->grid_id == 0x81) bild_81 = f;
        if (bild_17 < 0 && re15_game_flag_get(4, 17)) bild_17 = f;
        if (bild_raus < 0 && z->active && z->z < TUER_Z) bild_raus = f;
        if (bild_boden < 0 && z->active && z->y == 0 && z->floor == 0) bild_boden = f;
        if (getenv("R42L_DBG") && (f % 30) == 0)
            printf("    %s f%d tuer z%d ry%d | z slot%d st=%d/%d/%d g=%02x mo=%d 10e=%04x @(%d,%d,%d) b%d\n", name, f,
                   g_scd.props[o].z, g_scd.props[o].rot_y, k, z->state, z->sub_state_1, z->sub_state_2, z->grid_id,
                   z->motion, (unsigned)z->re2z_f10e, z->x, z->y, z->z, z->floor);
        if (bild_17 >= 0 && bild_boden >= 0 && f > bild_boden + 30) break;
    }
    printf("  [A %s] Klapperfolge %d Werte, Tuer dreht ab Bild %d, grid 0x81 ab Bild %d, (4,17) ab Bild %d, "
           "Kriecher aus dem Fach ab Bild %d, auf dem Boden ab Bild %d\n", name, nf, bild_dreh, bild_81, bild_17, bild_raus,
           bild_boden);
    int folge_ok = nf >= 20;
    for (int i = 0; i < 20 && i < nf; i++) if (folge[i] != k_klapper_z[i]) folge_ok = 0;
    CHECK(folge_ok, "[%s] Klappern: %d z-Werte, erwartet die 20 Pos_set-Werte der Disc (sub02 @0x00922..@0x009A0)",
          name, nf);
    CHECK(nf == 20 || nf == 21, "[%s] Klappern: %d z-Wechsel (20 Pos_set; danach nur Add_speed)", name, nf);
    CHECK(bild_dreh > 0 && bild_81 > bild_dreh, "[%s] Reihenfolge: Tuer dreht (%d) vor grid 0x81 (%d) — Disc @0x009AC < @0x009D0",
          name, bild_dreh, bild_81);
    CHECK(bild_17 > bild_81, "[%s] (4,17) nicht gesetzt (Disc @0x00A1E), Bild %d", name, bild_17);
    if (room == 0x1201)
        CHECK(dir_1201 == 1024, "[%s] Dir_set @0x0091C: Tuer rot_y %d", name, dir_1201);
    /* E: Kriecher raus und unten */
    CHECK(bild_raus > bild_81, "[%s] E: der Kriecher verlaesst das Fach nicht (z bleibt hinter %d)", name, TUER_Z);
    CHECK(bild_boden > bild_81, "[%s] E: der Kriecher kommt nicht auf die Spieler-Ebene (y=%d b%d)", name, z->y, z->floor);

    /* C: Minidisc aufgenommen -> Mod-sub03 ab dem inneren Block @0x00A04 (`06 00 34 00` Ifel_ck, @0x00A08 Ck(9,104)==1,
     *    @0x00A0C Evt_exec sub02, Wecken @0x00A16..@0x00A32, @0x00A36 Set(4,17,1)) — so laeuft sub03 nach dem "Ja" der
     *    Aufnahme (@0x00A00 Aot_on 3 / @0x00A02 Evt_next davor). Die Tuer darf nicht ein zweites Mal schwingen, die drei
     *    Liegenden (Gegner 0/1/3, grid 0x88/0x87/0x87) werden wie in der Mod geweckt. */
    {
        const int16_t ry_vor = g_scd.props[o].rot_y;
        const int32_t tz_vor = g_scd.props[o].z;
        uint8_t g_vor[RE15_ACTOR_MAX];
        for (int s = 0; s < RE15_ACTOR_MAX; s++) g_vor[s] = g_actors[s].grid_id;
        re15_game_flag_set(9, 104, 1);
        int slot = -1;
        for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++)
            if (!g_scd.threads[s].active && scd_thread_start(s, s_rdt.raw + 0x00A04) == 0) { slot = s; break; }
        CHECK(slot >= 0, "[%s] C: kein Faden fuer sub03 @0x00A04", name);
        for (int f = 0; f < 200; f++) { pl->hp = 100; pl->x = -24600; pl->z = -16300; frame_step(); }
        CHECK(g_scd.props[o].rot_y == ry_vor && g_scd.props[o].z == tz_vor,
              "[%s] C: Tuer schwang ein zweites Mal (rot %d -> %d, z %d -> %d)", name, ry_vor, g_scd.props[o].rot_y,
              tz_vor, g_scd.props[o].z);
        if (getenv("R42L_DBG"))
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active)
                    printf("    C slot%d typ %02x g=%02x->%02x st=%d/%d @(%d,%d,%d) b%d\n", s, g_actors[s].type, g_vor[s],
                           g_actors[s].grid_id, g_actors[s].state, g_actors[s].sub_state_1, g_actors[s].x, g_actors[s].y,
                           g_actors[s].z, g_actors[s].floor);
        int geweckt = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (s != k && g_actors[s].active && g_actors[s].type == 0x10 && (g_vor[s] == 0x87 || g_vor[s] == 0x88) &&
                g_actors[s].grid_id != g_vor[s]) geweckt++;
        CHECK(geweckt == 3, "[%s] C: %d von 3 Liegenden geweckt (Mod sub03 @0x00A1A/@0x00A26/@0x00A32)", name, geweckt);
        CHECK(re15_game_flag_get(4, 17), "[%s] C: (4,17) nach sub03", name);
        printf("  [C %s] nach sub03: Tuer rot %d z %d (unveraendert), %d Liegende geweckt\n", name, g_scd.props[o].rot_y,
               g_scd.props[o].z, geweckt);
    }
}

/* F: Wiederbetreten nach dem Disc-Ablauf OHNE Minidisc (Pruefbericht Stufe 1 Runde 1, R1). (4,17) heisst in der Mod "Minidisc
 * genommen, Liegende geweckt" (einziger Setzer sub03 @0x00A36 hinter @0x00A08 Ck(9,104)==1); Mod main00 @0x00852 waehlt daran
 * die Gegner. Erwartet (re15_leichenhalle_r42.h "WIEDERBETRETEN"):
 *   F1 echter Weg: Ablauf ausloesen, ohne Minidisc gehen, wiederkommen -> Gegner 0/1/3 liegen wie im Mod-Ja-Zweig
 *      (@0x00856 0x88 / @0x0086A 0x87 / @0x00892 0x87), Gegner 2 ist der Disc-Kriecher am Boden (@0x008C6: grid 0x81,
 *      (-24408,0,-15900), Gier 280) mit dem Mod-Tod-Merkzeichen 0x8E, KEIN stehender 0x0D; Tuer offen (Mod @0x0082A Gier
 *      2949), kein zweites Klappern.
 *   F2 danach Minidisc (sub03 @0x00A04): die drei Liegenden werden geweckt (Hinterhalt da), die Tuer schwingt nicht.
 *   F3 nochmal wiederkommen (jetzt (9,104)=1): Mod-Else-Zweig unveraendert = vier stehende 0x0D.
 *   F4 Kriecher getoetet ((7,0x8E)=1), ohne Minidisc wiederkommen: Gegner 2 bleibt weg (Tor @0x80042120-38). */
extern uint8_t re15_em_status_zone(void);
static int em_bei(int32_t x, int32_t z, uint8_t grid)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10 && g_actors[s].x == x && g_actors[s].z == z &&
            g_actors[s].grid_id == grid) return s;
    return -1;
}
static int zaehle_grid(uint8_t grid)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10 && g_actors[s].grid_id == grid) n++;
    return n;
}
static void tote_loeschen(void)
{
    g_current_room_id = 0x1200;
    for (int b = 0x8c; b <= 0x8f; b++) re15_game_flag_set(re15_em_status_zone(), (unsigned)b, 0);
}
static void liegende_pruefen(const char *name, const char *teil)
{
    CHECK(em_bei(-20950, -21700, 0x88) > 0, "[%s %s] Gegner 0 liegt nicht (Mod @0x00856 0x88 (-20950,-21700))", name, teil);
    CHECK(em_bei(-24249, -18579, 0x87) > 0, "[%s %s] Gegner 1 liegt nicht (Mod @0x0086A 0x87 (-24249,-18579))", name, teil);
    CHECK(em_bei(-19622, -16118, 0x87) > 0, "[%s %s] Gegner 3 liegt nicht (Mod @0x00892 0x87 (-19622,-16118))", name, teil);
    CHECK(zaehle_grid(0x0D) == 0, "[%s %s] %d stehende 0x0D (Mod-Else-Zweig @0x008AA..@0x008E6) - Hinterhalt weg", name, teil,
          zaehle_grid(0x0D));
}
static void wieder(uint16_t room, re15_ai_flavor_t fl, const char *name)
{
    tote_loeschen();
    raum_betreten_mit(room, fl, 0, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int f17 = -1;
    for (int f = 0; f < 600 && f17 < 0; f++) {                  /* im Rechteck bleiben; Fach >= 3029 entfernt */
        pl->hp = 100; pl->x = -25885; pl->z = -16605; pl->floor = 0; pl->y = 0;
        frame_step();
        if (re15_game_flag_get(4, 17)) f17 = f;
    }
    CHECK(f17 > 0, "[%s F1] Disc-Ablauf hat (4,17) nicht gesetzt (Disc @0x00A1E)", name);
    CHECK(!re15_game_flag_get(9, 104), "[%s F1] Minidisc (9,104) gesetzt", name);
    printf("  [F1 %s] Ablauf ohne Minidisc: (4,17) ab Bild %d; gehen und wiederkommen\n", name, f17);

    raum_betreten_mit(room, fl, -1, -1);                         /* Flags stehen lassen: (4,17)=1, (9,104)=0 */
    liegende_pruefen(name, "F1");
    const int k = em_bei(-24408, -15900, 0x81);
    CHECK(k > 0, "[%s F1] Gegner 2 ist nicht der Kriecher am Boden (Disc @0x008C6 grid 0x81 (-24408,0,-15900))", name);
    if (k > 0)
        CHECK(g_actors[k].y == 0 && g_actors[k].floor == 0 && g_actors[k].rot_y == 280 && g_actors[k].em_flag_id == 0x8E,
              "[%s F1] Kriecher y %d b%d Gier %d Tod-Merkzeichen %02x (erwartet 0/0/280/8e)", name, g_actors[k].y,
              g_actors[k].floor, g_actors[k].rot_y, g_actors[k].em_flag_id);
    const int o = obj1();
    CHECK(o >= 0 && g_scd.props[o].rot_y == 2949, "[%s F1] Tuer rot %d (Mod Else @0x0082A 2949)", name,
          o >= 0 ? g_scd.props[o].rot_y : -1);
    int bewegt = 0;
    for (int f = 0; f < 200 && o >= 0; f++) {
        const int32_t vz = g_scd.props[o].z; const int16_t vr = g_scd.props[o].rot_y;
        pl->hp = 100; pl->x = -25885; pl->z = -16605; pl->floor = 0; pl->y = 0;
        frame_step();
        if (g_scd.props[o].z != vz || g_scd.props[o].rot_y != vr) bewegt++;
    }
    CHECK(bewegt == 0, "[%s F1] Tuer bewegt sich beim Wiederbetreten (%d Bilder)", name, bewegt);
    if (k > 0) {   /* der Kriecher lebt am Boden weiter: Spieler ~1640 entfernt (Disc-Wecken Weg 2 < 3000 @0x80104718) */
        const re15_actor_t *z = &g_actors[k];
        CHECK(z->active && z->y == 0 && z->floor == 0 && z->type == 0x10,
              "[%s F1] Kriecher nach 200 Bildern: aktiv %d y %d b%d", name, z->active, z->y, z->floor);
        printf("  [F1 %s] Kriecher nach 200 Bildern: st=%d/%d/%d g=%02x mo=%d @(%d,%d,%d) b%d hp %d\n", name, z->state,
               z->sub_state_1, z->sub_state_2, z->grid_id, z->motion, z->x, z->y, z->z, z->floor, z->hp);
        /* Spieler VOR den Kriecher (Gier 280 ~ +x), 1408 entfernt: er setzt sich in Bewegung (Disc-Wecken Weg 2: < 3000
         * `sltiu 0xbb8` @0x80104718 im Bogen 0x200 @0x8010472c). Seitlich/hinten (oben, ~1640) blieb er liegen. */
        int los = -1;
        for (int f = 0; f < 300 && los < 0; f++) {
            pl->hp = 100; pl->x = -23000; pl->z = -15900; pl->floor = 0; pl->y = 0;
            frame_step();
            if (z->x != -24408 || z->z != -15900) los = f;
        }
        CHECK(los >= 0, "[%s F1] Kriecher bleibt liegen, obwohl der Spieler vor ihm steht", name);
        printf("  [F1 %s] Spieler vor dem Kriecher: er kriecht ab Bild %d los\n", name, los);
    }
    printf("  [F1 %s] wieder da: Liegende 0x88/0x87/0x87, Kriecher 0x81 am Boden (Slot %d), 0x0D %d, Tuer rot %d, Bewegung %d\n",
           name, k, zaehle_grid(0x0D), o >= 0 ? g_scd.props[o].rot_y : -1, bewegt);

    /* F2: Minidisc -> Mod-sub03 ab @0x00A04 */
    {
        uint8_t g_vor[RE15_ACTOR_MAX];
        for (int s = 0; s < RE15_ACTOR_MAX; s++) g_vor[s] = g_actors[s].grid_id;
        const int16_t ry_vor = o >= 0 ? g_scd.props[o].rot_y : 0;
        re15_game_flag_set(9, 104, 1);
        int slot = -1;
        for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++)
            if (!g_scd.threads[s].active && scd_thread_start(s, s_rdt.raw + 0x00A04) == 0) { slot = s; break; }
        CHECK(slot >= 0, "[%s F2] kein Faden fuer sub03 @0x00A04", name);
        for (int f = 0; f < 200; f++) { pl->hp = 100; pl->x = -25885; pl->z = -16605; frame_step(); }
        int geweckt = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x10 && (g_vor[s] == 0x87 || g_vor[s] == 0x88) &&
                g_actors[s].grid_id != g_vor[s]) geweckt++;
        CHECK(geweckt == 3, "[%s F2] %d von 3 Liegenden geweckt (Mod sub03 @0x00A1A/@0x00A26/@0x00A32)", name, geweckt);
        CHECK(o >= 0 && g_scd.props[o].rot_y == ry_vor, "[%s F2] Tuer schwang (rot %d -> %d)", name, ry_vor,
              o >= 0 ? g_scd.props[o].rot_y : -1);
        printf("  [F2 %s] Minidisc: %d Liegende geweckt, Tuer rot %d\n", name, geweckt, o >= 0 ? g_scd.props[o].rot_y : -1);
    }
    /* F3: wiederkommen mit Minidisc -> Mod-Else-Zweig unveraendert */
    tote_loeschen();
    raum_betreten_mit(room, fl, -1, -1);
    CHECK(zaehle_grid(0x0D) == 4, "[%s F3] %d stehende 0x0D, erwartet 4 (Mod-Else @0x008AA..@0x008E6 unveraendert)", name,
          zaehle_grid(0x0D));
    printf("  [F3 %s] mit Minidisc wieder da: %d stehende 0x0D (Mod)\n", name, zaehle_grid(0x0D));
    /* F4: Kriecher tot, ohne Minidisc wiederkommen */
    re15_game_flag_set(9, 104, 0);
    re15_game_flag_set(4, 17, 1);
    g_current_room_id = room;
    re15_game_flag_set(re15_em_status_zone(), 0x8E, 1);
    raum_betreten_mit(room, fl, -1, -1);
    CHECK(em_bei(-24408, -15900, 0x81) < 0, "[%s F4] getoeteter Kriecher wieder da (Tod-Merkzeichen 0x8E)", name);
    liegende_pruefen(name, "F4");
    printf("  [F4 %s] Kriecher getoetet: bleibt weg, Liegende liegen\n", name);
    tote_loeschen();
}

/* G: Minidisc ZUERST (Pruefbericht Stufe 1 Runde 2, R1). Mod-sub03 @0x009F4 oeffnet das Fach selbst (@0x00A08 Ck(9,104)==1,
 * @0x00A0C Evt_exec sub02 @0x009A0) und setzt (4,17) erst 10 Bilder spaeter (@0x00A1E/@0x00A2A Sleep 5+5, @0x00A36 Set).
 *   G0 echter Weg: an der Nordwand (z -15900) von Osten nach Westen bis an die Klemme, OHNE das Disc-Rechteck (Disc main00
 *      @0x007D2: x -26590 +1410, z -17180 +1150) zu betreten; die Aktion dort feuert Ereignis 3 (Mod sub00 @0x0091E Aot 2 =
 *      sub03). Bis dahin klappert nichts.
 *   G1 sub03 ab @0x00A04 mit (9,104)=1, der Spieler tritt nach K Bildern (K = 0..20) in die Flaeche: die Tuer klappert NICHT
 *      (z bleibt -14382, kein Pos_set der Disc @0x00922..) und endet bei derselben Drehung wie ohne Betreten (Mod allein). */
static int g_im_rechteck(int32_t px, int32_t pz)   /* FUN_80042b64 `sltu` @0x80042b78 / @0x80042b94, Satz @0x007D2 */
{
    return (uint32_t)(px - (-26590)) <= 1410u && (uint32_t)(pz - (-17180)) <= 1150u;
}
static void g_schritt(uint16_t pad)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = pad; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}
/* K < 0: der Spieler bleibt draussen (Bezug = Mod allein). Rueckgabe: Endrot der Tuer; *z_wechsel = Zahl der z-Aenderungen. */
static int g_lauf(uint16_t room, int K, int *z_wechsel, int *f17)
{
    raum_betreten_mit(room, RE15_AI_FLAVOR_RE2, 0, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int o = obj1();
    if (o < 0) return -99999;
    for (int f = 0; f < 20; f++) { pl->hp = 100; pl->x = -25788; pl->z = -15900; pl->floor = 0; pl->y = 0; frame_step(); }
    re15_game_flag_set(9, 104, 1);                                 /* Aufnahme bestaetigt (sub03 @0x00A00/@0x00A02) */
    int slot = -1;
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++)
        if (!g_scd.threads[s].active && scd_thread_start(s, s_rdt.raw + 0x00A04) == 0) { slot = s; break; }
    if (slot < 0) return -99998;
    *z_wechsel = 0; *f17 = -1;
    int32_t vz = g_scd.props[o].z;
    for (int f = 0; f < 400; f++) {
        pl->hp = 100; pl->floor = 0; pl->y = 0;
        if (K < 0 || f < K) { pl->x = -25788; pl->z = -15900; } else { pl->x = -25885; pl->z = -16605; }
        frame_step();
        if (g_scd.props[o].z != vz) { (*z_wechsel)++; vz = g_scd.props[o].z; }
        if (*f17 < 0 && re15_game_flag_get(4, 17)) *f17 = f;
    }
    return g_scd.props[o].rot_y;
}
static void md_zuerst(uint16_t room, const char *name)
{
    /* G0 */
    raum_betreten_mit(room, RE15_AI_FLAVOR_RE2, 0, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int o = obj1();
    CHECK(o >= 0, "[%s G0] obj 1 fehlt", name);
    if (o < 0) return;
    pl->x = -23800; pl->z = -15900; pl->rot_y = 2048;
    int drin = 0, zw = 0; int32_t vz = g_scd.props[o].z;
    for (int f = 0; f < 300; f++) {
        pl->hp = 100; g_schritt(RE15_PAD_BIT_UP);
        if (g_im_rechteck(pl->x, pl->z)) drin++;
        if (g_scd.props[o].z != vz) { zw++; vz = g_scd.props[o].z; }
    }
    pl->rot_y = 2048;
    for (int f = 0; f < 3; f++) { pl->hp = 100; g_schritt(0); }
    g_aot_action_pressed = 1;
    re15_aot_scan(pl->x, pl->z, 0);
    const int ev = g_aot.fired_event_id_this_frame;
    g_aot_action_pressed = 0;
    CHECK(drin == 0 && zw == 0, "[%s G0] auf dem Weg an der Nordwand: %d Bilder im Rechteck, %d Klapper-Schritte", name, drin, zw);
    CHECK(ev == 3, "[%s G0] Aktion bei (%d,%d): Ereignis %d, erwartet 3 (Mod sub00 @0x0091E Aot 2 -> sub03)", name, pl->x, pl->z, ev);
    printf("  [G0 %s] Nordwand bis (%d,%d) ohne Rechteck (%d Bilder drin), Aktion -> Ereignis %d (sub03 Minidisc)\n", name,
           pl->x, pl->z, drin, ev);

    /* G1 */
    int zb = 0, f17b = -1;
    const int bezug = g_lauf(room, -1, &zb, &f17b);
    CHECK(zb == 0 && f17b > 0, "[%s G1] Bezug Mod allein: z-Wechsel %d, (4,17) ab Bild %d", name, zb, f17b);
    static const int ks[] = { 0, 1, 2, 3, 5, 8, 10, 12, 20 };
    int schlecht = 0;
    for (unsigned i = 0; i < sizeof ks / sizeof ks[0]; i++) {
        int zwk = 0, f17 = -1;
        const int rot = g_lauf(room, ks[i], &zwk, &f17);
        if (zwk != 0 || rot != bezug) {
            schlecht++;
            printf("    [G1 %s] K=%d: z-Wechsel %d, Endrot %d (Mod allein %d)\n", name, ks[i], zwk, rot, bezug);
        }
        CHECK(zwk == 0, "[%s G1] K=%d: Tuer klappert nach dem Minidisc (%d z-Wechsel, Disc-Pos_set @0x00922..)", name, ks[i], zwk);
        CHECK(rot == bezug, "[%s G1] K=%d: Tuer endet bei %d statt %d (Mod-sub02 @0x009A0 allein)", name, ks[i], rot, bezug);
        CHECK(f17 > 0 && re15_game_flag_get(9, 104), "[%s G1] K=%d: (4,17) ab Bild %d", name, ks[i], f17);
    }
    printf("  [G1 %s] Minidisc zuerst, K = 0..20 Bilder spaeter in die Flaeche: %d von %u Laeufen falsch, Tuer endet bei %d "
           "(Mod allein), (4,17) ab Bild %d\n", name, schlecht, (unsigned)(sizeof ks / sizeof ks[0]), bezug, f17b);
}

int main(void)
{
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1200.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1200.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    if (getenv("R42L_SCA"))
        for (int i = 0; i < s_rdt.sca_count; i++) {
            const re15_sca_entry_t *c = &s_rdt.sca[i];
            printf("  SCA %2d x %6d..%6d z %6d..%6d typ %u u0 %02x u1 %02x fl %02x\n", i, c->x, c->x + c->width, c->z,
                   c->z + c->density, c->type, c->u0, c->u1, c->floor);
        }

    lauf(0x1200, RE15_AI_FLAVOR_RE2,  "Leon RE2");
    lauf(0x1201, RE15_AI_FLAVOR_RE2,  "Elza RE2");
    lauf(0x1200, RE15_AI_FLAVOR_RE15, "Leon RE1.5");

    /* B: Etage 1 im Rechteck */
    {
        raum_betreten(0x1200, RE15_AI_FLAVOR_RE2, 0);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        const int o = obj1();
        for (int f = 0; f < 120; f++) { pl->x = -25885; pl->z = -16605; pl->floor = 1; pl->y = -1800; frame_step(); }
        CHECK(o >= 0 && g_scd.props[o].z == TUER_Z && !re15_game_flag_get(4, 17), "B: Etage 1 hat ausgeloest");
        printf("  [B] Etage 1 im Rechteck: kein Klappern\n");
    }
    /* D: (4,17)=1 beim Betreten */
    {
        raum_betreten(0x1200, RE15_AI_FLAVOR_RE2, 1);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        const int o = obj1();
        int bewegt = 0;
        for (int f = 0; f < 200; f++) {
            pl->x = -25885; pl->z = -16605; pl->floor = 0; pl->y = 0;
            const int32_t vz = o >= 0 ? g_scd.props[o].z : 0;
            frame_step();
            if (o >= 0 && g_scd.props[o].z != vz) bewegt++;
        }
        CHECK(o >= 0 && bewegt == 0, "D: (4,17)=1 und trotzdem Klappern (%d)", bewegt);
        printf("  [D] (4,17)=1: Tuer ruht (Mod main00 Else-Zweig rot %d)\n", o >= 0 ? g_scd.props[o].rot_y : -1);
    }

    /* F: Wiederbetreten (Pruefbericht R1) */
    wieder(0x1200, RE15_AI_FLAVOR_RE2,  "Leon RE2");
    wieder(0x1201, RE15_AI_FLAVOR_RE2,  "Elza RE2");
    wieder(0x1200, RE15_AI_FLAVOR_RE15, "Leon RE1.5");

    /* G: Minidisc zuerst (Pruefbericht Stufe 1 Runde 2, R1) */
    tote_loeschen();
    md_zuerst(0x1200, "Leon");
    md_zuerst(0x1201, "Elza");

    free(buf);
    if (fails) { printf("test_r42_leichenhalle: %d FAIL\n", fails); return 1; }
    printf("test_r42_leichenhalle: OK\n");
    return 0;
}
