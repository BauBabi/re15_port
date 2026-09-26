/* probe_eintritt_pose_einmal.c - RIEGEL (Runde 26, 2026-09-26).
 *
 * Nutzer: "Bei den Charakteren - Leon, Ada etc. wiederholt sich am Anfang immer die
 * 1. Animation, wenn sie eine Cutscene nach dem Raumwechsel haben. So zum Beispiel bei
 * Leon ROOM 1170 nach dem durchlaufen der Tuer, oder auch bei Leon/Ada wenn sie Room 1050
 * betreten und die Cutscene startet."
 *
 * Dossier: analysis/befunde_2026-09-26/cutscene-erste-anim-wiederholt.md
 *
 * ORIGINAL (selbst nachdisassembliert, info/Re1.5/PSX.EXE):
 *   cmd-0-Handler @0x800318f8 laeuft GENAU EINMAL - er stellt im selben Durchlauf das
 *   Kommandowort auf 1 (ori v0,zero,0x1 @0x8003191c, sw v0,-13736(at) = 0x800aca58
 *   @0x8003192c) und setzt die Eintritts-Pose hart:
 *     @0x80031bf4 addu a2,zero,zero    vorwaerts
 *     @0x80031c10 sb v0(=1),-13592(at) 0x800acae8 = +0x94 = 1  (W-Bank CLIP 1)
 *     @0x80031c18 sb zero,-13591(at)   0x800acae9 = +0x95 = 0  (Bild 0)
 *     @0x80031c20 sb zero,-13597(at)   0x800acae3 = +0x8f = 0
 *     @0x80031c24 jal 0x8001f314       anim_set
 *   Ab dem naechsten Bild dispatcht 0x80073f90[1]; der Idle-FSM Fall 0 (switchD_8003206c)
 *   uebernimmt mit
 *     @0x8003207c +0x06 = 1 / @0x80032088 +0x94 = 3 / @0x80032094 +0x95 = 0 /
 *     @0x8003209c +0x8f = 7 / @0x800320a4 0x800acae0 = 0 /
 *     @0x800320b0 andi 0x1f + @0x800320b4 addiu 90  (Haltezeit 90 + rng&0x1f)
 *   +0x95 bewegt sich im Original ausschliesslich innerhalb eines anim_set-Aufrufs
 *   (@0x8001f610-1c), also kann die Eintritts-Pose dort gar nicht zyklieren.
 *
 * PORT-DEFEKT (gemessen mit RE15_ANIM_TRACE, Dossier §1.2): die Eintritts-Pose wurde als
 * DAUER-Motion 210 geparkt und ueber den Modulo-Zweig cur %% frame_count (anim_select_
 * common.c) zyklisch abgespielt - ROOM1050 3,5 Durchlaeufe (cur 0..50), ROOM1170-Tuer
 * 1 Neustart. Sichtbar nur bei player_mode == 2, weil dort kein Pad-/Idle-Zweig die Pose
 * ersetzt.
 *
 * DIESE SONDE faehrt den Mechanismus deterministisch nach: Raum laden, Spieler setzen,
 * player_mode = 2 (Skript-Szene), re15_player_room_entry_pose() (= der cmd-0-Endzustand,
 * den room_common.c / game_step_common.c beim Tuer-Eintritt rufen), dann 90 Bilder
 * re15_game_step. Gemessen wird PRO BILD motion / anim_frame / anim_frac.
 *
 * SOLL (byte-true): hoechstens EIN Bild auf motion 210, dabei anim_frame == 0, NULL
 * Neustarts - danach motion 200 (W-Bank Clip 3) mit Crossfade-Saat 7.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

#define MOTION_IDLE_SETTLE 210   /* W-Bank Clip 1 = die Eintritts-Pose (@0x80031c10) */
#define MOTION_IDLE        200   /* W-Bank Clip 3 = Idle-FSM Fall 0   (@0x80032088) */

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

/* Ein Lauf = ein Raum. Rueckgabe: 0 = gruen, 1 = rot. */
static int lauf(const char *name, unsigned room_id, int bilder)
{
    char path[600];
    const char *base = getenv("RE15_ASSET_DIR");
    size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM%04X.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR, room_id);
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 1; }
    memset(&s_rdt, 0, sizeof s_rdt);
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse %s\n", path); return 1; }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_player_cmd_reset();
    re15_inv_init();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)room_id;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 105; pl->floor = 0; pl->y = 0;   /* Vorraum-Laufclip */
    pl->x = 0; pl->z = 0; pl->anim_frame = 9;
    re15_collision_set_band(0);

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    /* Die Cutscene haelt den Spieler auf player_mode 2 (Skript-Szene). Genau dort faellt
     * der Defekt auf (Dossier §1.3): der pm-2-Zweig holt nur die Lauf-Sentinels 105/100
     * in den Idle zurueck, die 210 bleibt stehen. */
    g_scd.player_mode = 2;

    /* Der cmd-0-Endzustand, wie ihn der Tuer-/Reenter-Pfad setzt
     * (room_common.c / game_step_common.c). */
    re15_player_room_entry_pose();

    printf("\n===== %s (ROOM%04X, player_mode=2) =====\n", name, room_id);
    printf("EINTRITT: mo=%d af=%u frac=%u\n",
           (int)pl->motion, (unsigned)pl->anim_frame, (unsigned)pl->anim_frac);

    int bilder_210 = 0, max_af_210 = 0;
    int erstes_200 = -1, frac_bei_200 = -1;
    int vor_mo = -1;
    for (int f = 0; f < bilder; f++) {
        const unsigned char *raw; int len, id;
        re15_msg_tick(&raw, &len, &id);
        s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
        g_scd.player_mode = 2;                 /* die Szene haelt ihn dort fest */
        re15_game_step(&s_ctx);
        int mo = (int)pl->motion, af = (int)pl->anim_frame;
        if (mo == MOTION_IDLE_SETTLE) {
            bilder_210++;
            if (af > max_af_210) max_af_210 = af;
        }
        if (mo == MOTION_IDLE && erstes_200 < 0) {
            erstes_200 = f; frac_bei_200 = (int)pl->anim_frac;
        }
        if (f < 8 || mo != vor_mo)
            printf("  F%-3d mo=%-3d af=%-3d frac=%d\n", f, mo, af, (int)pl->anim_frac);
        vor_mo = mo;
    }
    /* WIEDERHOLUNGEN: der Port rendert die Pose ueber cur %% frame_count; W-Bank Clip 1 hat
     * die gemessene Laenge 16 (Dossier §1.2 Lauf A: fc=16). max anim_frame / 16 ist damit
     * die Zahl der sichtbaren Wiederholungen. Die harte Schranke unten ist aber
     * max anim_frame == 0 - die schliesst eine Wiederholung fuer JEDE Cliplaenge aus,
     * nicht nur fuer 16. */
    printf("ABDECKUNG: %d Bilder; auf motion 210: %d Bilder, max anim_frame %d "
           "(= %d Wiederholungen bei Cliplaenge 16); erstes motion 200 in F%d (frac=%d)\n",
           bilder, bilder_210, max_af_210, max_af_210 / 16, erstes_200, frac_bei_200);

    int fehler = 0;
    if (bilder_210 > 1) {
        printf("RIEGEL-ROT [%s]: die Eintritts-Pose (motion 210) steht %d Bilder statt "
               "hoechstens 1 -- sie ist als DAUER-Motion geparkt. Das Original setzt sie im "
               "cmd-0-Handler genau EINMAL (@0x80031c10-c24) und gibt im selben Durchlauf an "
               "Kommando 1 ab (@0x8003192c).\n", name, bilder_210);
        fehler = 1;
    }
    if (max_af_210 > 0) {
        printf("RIEGEL-ROT [%s]: anim_frame laeuft auf motion 210 bis %d -- im Original nullt "
               "der Handler +0x95 vor dem einzigen anim_set (@0x80031c18) und +0x95 bewegt "
               "sich nur INNERHALB von anim_set (@0x8001f610-1c).\n", name, max_af_210);
        fehler = 1;
    }
    if (erstes_200 != 1) {
        printf("RIEGEL-ROT [%s]: die Uebergabe an den Idle-FSM Fall 0 (motion 200, "
               "+0x94 = 3 @0x80032088) faellt in F%d statt F1.\n", name, erstes_200);
        fehler = 1;
    }
    if (erstes_200 == 1 && frac_bei_200 != 7) {
        printf("RIEGEL-ROT [%s]: Crossfade-Saat bei der Uebergabe ist %d statt 7 "
               "(+0x8f = 7 @0x8003209c).\n", name, frac_bei_200);
        fehler = 1;
    }
    if (!fehler)
        printf("RIEGEL-GRUEN [%s]: 210 nur %d Bild(er) bei anim_frame 0 (keine Wiederholung bei beliebiger Cliplaenge), Uebergabe an 200 in F1 mit frac 7.\n", name, bilder_210);
    free(buf);
    return fehler;
}

int main(void)
{
    int fehler = 0;
    fehler |= lauf("ROOM1170 - Leon nach dem Durchlaufen der Tuer", 0x1170, 90);
    fehler |= lauf("ROOM1050 - Leon/Ada, Cutscene beim Betreten",   0x1050, 90);
    printf("\n%s\n", fehler ? "=== EINTRITTS-POSE: ROT ===" : "=== EINTRITTS-POSE: GRUEN ===");
    return fehler;
}
