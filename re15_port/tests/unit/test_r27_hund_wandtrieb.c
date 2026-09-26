/* test_r27_hund_wandtrieb.c — RIEGEL zu Runde 27.
 *
 * Nutzer (ROOM11D0, nach v0.8.13): "der Spieler wurde durch den BISS der Hunde in die Wand
 * getrieben. Nicht die hunde durch den Spieler."
 *
 * Der Riegel misst GENAU das: Bilder, in denen der Spieler nach einem Hunde-Angriff auf einem
 * unbegehbaren Punkt steht — "unbegehbar" in der harten Lesart, also der Punkt liegt OHNE
 * Radius IN einer soliden SCA-Zelle (re15_collision_on_floor). Dazu ein Gegen-Riegel, dass
 * die Hunde weiter angreifen (sonst waere "0 unbegehbare Bilder" durch einen kaputten Hund
 * trivial erfuellt).
 *
 * WAS ER FESTHAELT: den fehlenden ersten Aufruf des Gegner-Schwanzes,
 *   8010d880  jal 0x8002aec4   a0 = 0x800ACA54 (Spieler) , a1 = der Hund
 * der den HUND aus dem Spieler schiebt (enemy_ai_common.c re15_enemy_body_push_tail).
 * Ohne ihn trug allein der Spieler-Schub FUN_8002b544 @0x80031cbc die Trennung und drueckte
 * den Spieler an der Wand hinein.
 *
 * ZAHLEN (probe_r27_hund_biss raster, 123 Startplaetze, ~27000 Bilder):
 *   ohne den Aufruf: 467 harte Bilder ueber 19 von 68 Plaetzen
 *   mit dem Aufruf :  25 harte Bilder ueber  2 von 69 Plaetzen (Rest: (-3500,-13500), offen)
 * DIESER RIEGEL auf den acht unten gelisteten Plaetzen, nachgemessen:
 *   ohne den Aufruf: 393 harte Bilder ueber 5 der 8 Plaetze, 91 Bisse   -> ROT
 *   mit dem Aufruf :   0 harte Bilder,                      106 Bisse  -> GRUEN
 *
 * Braucht die RE2-Bank shared_assets/RE2/CDEMD0.EMS (Auslieferungs-Default ist RE2-KI) —
 * ohne sie SKIP 77.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_emd.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static re15_rdt_t           s_rdt;
static re15_camera_view_t   s_cam;
static re15_game_ctx_t      s_ctx;
static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_dog_bank(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x20);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(0x20);
    if (!eb) return 0;
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) return 0;
    if (re2_ems_load_bank(ems, n, 0x20, eb, NULL) != 0) return 0;
    eb->buf = NULL; eb->ok = 1;
    return 1;
}

static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_victim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    { extern void re15_re2z_rng_reset(void); re15_re2z_rng_reset(); }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;      /* ohne das klemmt KEIN Gegner an den Waenden */
    g_current_room_id = 0x11D0; g_room_change.pending = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    /* Der FREIE Hunde-Satz: ROOM11D0 main00 @0x12B6 `06 00 38 01` If / @0x12BA `21 03 98 00`
     * Ck(Bank 3, Bit 152, Soll 0) waehlt die Zwinger-Hunde (@0x12BE..0x130E, Verhalten 0x41);
     * der Else-Zweig @0x13EE `07 00 fe 01` spawnt die frei laufenden (@0x13F2..0x1442,
     * Verhalten 0x00). Gebissen wird man nur vom freien Satz. */
    re15_game_flag_set(3, 152, 1);
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
}

/* Die Plaetze, an denen der alte Stand den Spieler in die Wand getrieben hat (alle aus
 * probe_r27_hund_biss raster, Schrittweite 1000 ueber den begehbaren Raum). */
static const int32_t k_platz[][2] = {
    {   500, -17500 },   /* alt: 296 unbegehbare Bilder */
    {  2500, -17500 },   /* alt: 247 */
    {  1500, -17500 },   /* alt: 207 */
    { -6500, -18500 },   /* alt: 141 */
    { -5500, -18500 },   /* alt:  99 */
    { -5500, -19500 },   /* alt:  65 */
    { -5500, -15500 },   /* alt:  51 */
    { -6500, -17500 },   /* alt:  37 */
};
#define PLAETZE ((int)(sizeof k_platz / sizeof k_platz[0]))

int main(void)
{
    size_t rsz = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM11D0.RDT", &rsz);
    if (!raw) { printf("SKIP: ROOM11D0.RDT fehlt\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("SKIP: RDT-Parse\n"); return 77; }
    {   size_t a = 0, b = 0;
        uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &a);
        uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &b);
        if (!(edd && emr && re15_emd_parse_animation(edd, a, &s_pl00_anim) == 0 &&
              re15_emd_parse_skeleton(emr, b, &s_pl00_skel) == 0)) {
            printf("SKIP: PL00-Rig fehlt\n"); return 77;
        }
    }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);          /* Auslieferungs-Default */
    if (!load_dog_bank()) { printf("SKIP: RE2-Hundebank (RE2/CDEMD0.EMS) fehlt\n"); return 77; }

    int hart = 0, bisse = 0, bilder = 0, plaetze_rot = 0, plaetze_mit_biss = 0;
    for (int p = 0; p < PLAETZE; p++) {
        bringup();
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->x = k_platz[p][0]; pl->z = k_platz[p][1]; pl->y = 0; pl->rot_y = 1024;
        pl->hp = 100; pl->floor = 0;
        re15_collision_set_band(0);
        /* ⛔ Der Startplatz MUSS begehbar sein — sonst misst der Riegel einen Spieler, der
         * schon vor dem ersten Bild in der Wand steht (passiert: (-4500,-13500) lieferte so
         * 337 "harte" Bilder ohne jeden Hundekontakt). */
        {   int32_t nx = pl->x, nz = pl->z;
            re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
            if (re15_collision_on_floor(&s_rdt, pl->x, pl->z) || nx != pl->x || nz != pl->z) {
                printf("FAIL: Startplatz (%d,%d) ist nicht begehbar — Sonde misst Unsinn\n",
                       (int)pl->x, (int)pl->z);
                return 1;
            }
        }
        int p_hart = 0, p_bisse = 0;
        for (int f = 0; f < 400; f++) {
            int hp0 = pl->hp;
            if (pl->hp < 30) pl->hp = 100;           /* am Leben halten (der Nutzer ueberlebte auch) */
            frame();
            bilder++;
            if (pl->hp < hp0) p_bisse++;
            re15_collision_set_band(0);
            if (re15_collision_on_floor(&s_rdt, pl->x, pl->z)) p_hart++;
        }
        bisse += p_bisse; hart += p_hart;
        if (p_bisse) plaetze_mit_biss++;
        if (p_hart) { plaetze_rot++;
            printf("  ROT  Platz (%6d,%6d): %d Bilder IN einer soliden Zelle (Bisse %d)\n",
                   (int)k_platz[p][0], (int)k_platz[p][1], p_hart, p_bisse); }
    }

    printf("ABDECKUNG: %d Plaetze, %d Bilder, %d Bisse, %d Plaetze mit Biss\n",
           PLAETZE, bilder, bisse, plaetze_mit_biss);
    printf("ERGEBNIS : %d Bilder mit dem Spieler IN einer soliden Zelle (%d Plaetze)\n",
           hart, plaetze_rot);

    int fehler = 0;
    /* (1) DER RIEGEL: kein Bild im Wandinneren. Am alten Stand (ohne aec4 @0x8010d880)
     *     NACHGEMESSEN: 393 Bilder ueber 5 dieser 8 Plaetze. */
    if (hart != 0) { printf("FAIL: %d Bilder im Wandinneren (erwartet 0)\n", hart); fehler = 1; }
    /* (2) GEGEN-RIEGEL: die Hunde greifen weiter an. Ohne ihn waere (1) auch mit einem
     *     kaputten Hund gruen. */
    if (bisse < 20) { printf("FAIL: nur %d Bisse — die Hunde greifen nicht mehr an\n", bisse);
                      fehler = 1; }
    if (plaetze_mit_biss < PLAETZE / 2) {
        printf("FAIL: nur %d von %d Plaetzen mit Biss\n", plaetze_mit_biss, PLAETZE);
        fehler = 1;
    }
    printf(fehler ? "FAIL\n" : "OK\n");
    return fehler;
}
