/* probe_1090_climb.c — MESSSONDE "auf die Kiste klettern" (ROOM1090).
 *
 * Nutzer-Befund: "Man kann noch nicht auf die Kiste draufklettern, was normalerweise geht."
 *
 * Gemessen wird der ECHTE Weg: ROOM1090 laden, den Spieler vor Kiste 0 stellen, ihn per
 * gehaltenem UP an die Kiste laufen lassen (der Objekt-Push-Out stoppt ihn), dann die
 * ACTION-Press-Edge feuern und Frame fuer Frame Substate / KIND / PHASE / Clip / Band /
 * Y / Objektbezug protokollieren.
 *
 * Sollseite (aus FUN_8002d474 @0x8002d474 + LAB_80037fd8 @0x80037fd8):
 *   ACTION -> Sonde 800 voraus (Kurs auf 90 Grad gerastet) -> FUN_8002d2c0 findet obj0
 *   -> Substate 9, KIND 0x81 -> Phase 0/1 (W01 clip 5, Kurs-Einschwingen) -> Phase 2/3
 *   (PL00 clip 2, 50 Frames): bei Frame 0x1d Vorwaertsschub 0x47c, Y -= 1800, Band += 1
 *   -> Phase 4: posS auf die Objektmitte, zurueck auf Substate 0.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_climb.h"
#include "re15_collision.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */

extern scd_vm_t g_scd;

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static re15_emd_animation_t s_pl00, s_w01;
static int load_banks(void)
{
    char p[600]; size_t sz = 0; int ok = 1;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", RE15_ASSET_PSX_DIR);
    uint8_t *a = read_file(p, &sz);
    if (!a || re15_emd_parse_animation(a, sz, &s_pl00) != 0) ok = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00W01.EDD", RE15_ASSET_PSX_DIR);
    uint8_t *b = read_file(p, &sz);
    if (!b || re15_emd_parse_animation(b, sz, &s_w01) != 0) ok = 0;
    return ok;
}

static void dump(const char *tag, const re15_actor_t *pl, int f)
{
    printf("  %-6s F%-3d sub=%-2d kind=0x%02X phase=%d motion=%-4d frame=%-3d "
           "band=%d pos=(%ld,%ld,%ld) rot=%d obj=%d stand=%d\n",
           tag, f, re15_climb_dbg_sub(), re15_climb_dbg_kind(), re15_climb_dbg_phase(),
           (int)pl->motion, (int)pl->anim_frame, re15_collision_debug_band(),
           (long)pl->x, (long)pl->y, (long)pl->z, (int)pl->rot_y,
           re15_climb_dbg_obj(), re15_climb_dbg_standing());
}

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT";
    size_t sz = 0;
    uint8_t *raw = read_file(path, &sz);
    if (!raw) { printf("FEHLT: %s\n", path); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("PARSE FEHLGESCHLAGEN\n"); return 1; }
    if (!load_banks()) printf("  (WARNUNG: PL00/PL00W01-Baenke fehlen — Cliplaengen = 1)\n");

    re15_actor_init();
    scd_vm_init();
    re15_climb_reset();
    g_current_room_id = 0x1090;
    g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    re15_collision_set_band(re15_collision_band_from_y(pl->y));

    printf("== Props ==\n");
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        printf("  prop[%d] flags=0x%04X typ=%u band=%u pos=(%ld,%ld,%ld) box c=(%d,%d,%d) h=(%d,%d,%d)\n",
               i, g_scd.props[i].flags, g_scd.props[i].obj_type, g_scd.props[i].band,
               (long)g_scd.props[i].x, (long)g_scd.props[i].y, (long)g_scd.props[i].z,
               g_scd.props[i].box_cx, g_scd.props[i].box_cy, g_scd.props[i].box_cz,
               g_scd.props[i].box_hx, g_scd.props[i].box_hy, g_scd.props[i].box_hz);

    /* --- 1) An die Kiste laufen: Blick auf +X (rot_y = 0 => x += cos), UP halten.
     *     Startpunkt = die Z-Achse der Kiste, damit der Anlauf frontal ist. --- */
    pl->rot_y = 0;
    pl->z = g_scd.props[0].z;
    printf("\n== 1) Anlauf an Kiste 0 (Schritt +60/Frame, Wand- + Objekt-Push-Out) ==\n");
    printf("  on_floor(Start) = %d\n", re15_collision_on_floor(&rdt, pl->x, pl->z));
    for (int f = 0; f < 80; f++) {
        int32_t ox = pl->x, oz = pl->z;
        int32_t nx = pl->x + 60, nz = pl->z;          /* Vorwaertsschritt */
        re15_collision_constrain(&rdt, ox, oz, &nx, &nz);
        pl->x = nx; pl->z = nz;
        re15_collision_objects(&pl->x, &pl->z);
        re15_climb_standing_tick();
        if (f % 20 == 19) dump("lauf", pl, f);
    }
    printf("  Endstand vor der Kiste: x=%ld z=%ld (Kiste0 x=%ld, Box +-900, Spielerradius 450"
           " -> erwartet x = %ld)\n",
           (long)pl->x, (long)pl->z, (long)g_scd.props[0].x,
           (long)(g_scd.props[0].x - 900 - 450));

    /* --- 2) ACTION: die Sonde --- */
    printf("\n== 2) ACTION -> FUN_8002d474 ==\n");
    int consumed = re15_climb_try_start(&rdt, 1);
    printf("  re15_climb_try_start = %d  (1 = Aktion verbraucht = Klettern startet)\n", consumed);
    dump("start", pl, 0);
    if (!consumed) { printf("  KEIN KLETTERN — Sonde hat nicht gegriffen.\n"); free(raw); return 1; }

    /* --- 3) Der FSM Frame fuer Frame --- */
    printf("\n== 3) Substate-9/10-Verlauf ==\n");
    int prev_phase = -1, prev_sub = -1;
    for (int f = 0; f < 400 && re15_climb_active(); f++) {
        re15_climb_tick(&rdt, NULL, &s_pl00, &s_w01);
        re15_climb_standing_tick();
        if (re15_climb_dbg_phase() != prev_phase || re15_climb_dbg_sub() != prev_sub ||
            f < 4) {
            dump("fsm", pl, f);
            prev_phase = re15_climb_dbg_phase(); prev_sub = re15_climb_dbg_sub();
        }
    }
    printf("\n== 4) Endstand ==\n");
    dump("ende", pl, 0);
    printf("  Kistendeckel = obj.y - 2*box_hy = %ld ; Spieler-Y = %ld ; Band = %d\n",
           (long)((int32_t)g_scd.props[0].y - 2 * (int32_t)g_scd.props[0].box_hy),
           (long)pl->y, re15_collision_debug_band());

    /* --- 5) Runter: ACTION am Kistenrand. In der Kistenmitte MUSS die Sonde durchfallen
     *     (der Sondenpunkt liegt noch in der um 450 aufgeblasenen Kiste des Bandes 1,
     *     @0x8002d6a0). Erst am aeusseren Rand greift der Absprung. --- */
    printf("\n== 5) Herunterspringen ==\n");
    printf("  Mitte (x=%ld): try_start = %d  (erwartet 0 — Sonde faellt in die eigene Kiste)\n",
           (long)pl->x, re15_climb_try_start(&rdt, 1));
    pl->x = (int32_t)g_scd.props[0].x + 850;      /* an die vordere Kistenkante */
    re15_climb_standing_tick();
    {   /* Zwischenwerte der Sonde offenlegen (Sondenpunkt = 800 voraus, Kurs 0). */
        int band = re15_collision_debug_band();
        int32_t fx = pl->x + 800, fz = pl->z;
        printf("  Sonde: probe=(%ld,%ld) band=%d typeword(band)=0x%04X "
               "room_coll(r=1,b=8)=%d room_coll(r=1,b=%d)=%d reach(stand,mode1)=%d\n",
               (long)fx, (long)fz, band,
               re15_collision_floor_typeword(&rdt, fx, fz, band, 0),
               (int)re15_collision_room_coll(&rdt, fx, fz, 1, 8, 0x100),
               band, (int)re15_collision_room_coll(&rdt, fx, fz, 1, band, 0x100),
               re15_prop_reach_test(pl->x, pl->z, pl->rot_y, re15_climb_dbg_standing(), 1));
        for (int b = 0; b <= 7; b++)
            printf("        typeword(band %d) = 0x%04X\n", b,
                   re15_collision_floor_typeword(&rdt, fx, fz, b, 0));
    }
    /* Alle vier Kanten/Kurse durchprobieren — wo faellt der Absprung? */
    {
        const int yaws[4] = {0, 1024, 2048, 3072};
        for (int k = 0; k < 4; k++) {
            pl->x = (int32_t)g_scd.props[0].x; pl->z = (int32_t)g_scd.props[0].z;
            pl->rot_y = (int16_t)yaws[k];
            int32_t cs = re15_cos_q12(yaws[k]), sn = re15_sin_q12(yaws[k]);
            pl->x += (int32_t)((850 * cs) >> 12);
            pl->z -= (int32_t)((850 * sn) >> 12);
            re15_climb_standing_tick();
            int32_t fx = pl->x + (int32_t)((800 * cs) >> 12);
            int32_t fz = pl->z - (int32_t)((800 * sn) >> 12);
            int r = re15_climb_try_start(&rdt, 1);
            printf("  yaw=%-4d pos=(%ld,%ld) probe=(%ld,%ld) roomcoll(b=8)=%d "
                   "-> try_start=%d sub=%d kind=0x%02X\n",
                   yaws[k], (long)pl->x, (long)pl->z, (long)fx, (long)fz,
                   (int)re15_collision_room_coll(&rdt, fx, fz, 1, 8, 0x100),
                   r, re15_climb_dbg_sub(), re15_climb_dbg_kind());
            if (r) break;
        }
    }
    printf("  Kante (x=%ld, stand=%d): ", (long)pl->x, re15_climb_dbg_standing());
    int c2 = re15_climb_active();
    printf("  re15_climb_try_start = %d\n", c2);
    dump("down", pl, 0);
    prev_phase = -1; prev_sub = -1;
    for (int f = 0; f < 400 && re15_climb_active(); f++) {
        re15_climb_tick(&rdt, NULL, &s_pl00, &s_w01);
        re15_climb_standing_tick();
        if (re15_climb_dbg_phase() != prev_phase || re15_climb_dbg_sub() != prev_sub) {
            dump("fsm", pl, f);
            prev_phase = re15_climb_dbg_phase(); prev_sub = re15_climb_dbg_sub();
        }
    }
    dump("ende", pl, 0);

    free(raw);
    return 0;
}
