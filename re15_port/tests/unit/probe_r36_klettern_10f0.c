/* probe_r36_klettern_10f0.c — Runde 36 Spur A (BLOCKER "auf die verschiebbaren Tritte klettern").
 *
 * Diagnose-Sonde + Pin fuer ROOM10F0 (Communication Room): der verschiebbare Tritt ist
 * Obj_model_set obj 0 (Typ 4) aus sub00 @0x010F4 (Startlage (-6050,0,4950), Flag (3,102)==0)
 * bzw. @0x0111A (Endlage (-4850,0,-850), Flag (3,102)==1). Anders als die Kisten in ROOM1090
 * (pc[6..7] = 00 01 -> Flags 0x0101) traegt der 10F0-Tritt pc[6..7] = 00 00 -> Flags 0x0001.
 *
 * Aufruf: probe_r36_klettern_10f0 diag            (nur Diagnose-Zeilen, immer exit 0)
 *         probe_r36_klettern_10f0 pin             (Riegel: Aufstieg in der Endlage)
 *         probe_r36_klettern_10f0 pin_start       (Riegel: Aufstieg in der STARTLAGE, ohne Verschieben)
 *
 * Dossier: analysis/befunde_runde36/A_klettern.md
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
#include "re15_skeleton.h"

extern scd_vm_t g_scd;

static int g_fail = 0;
static void chk(const char *what, long got, long want)
{
    if (got != want) { printf("FAIL %-48s got=%ld want=%ld\n", what, got, want); g_fail++; }
    else               printf("ok   %-48s %ld\n", what, got);
}

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
    char p[600]; size_t sz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", RE15_ASSET_PSX_DIR);
    uint8_t *a = read_file(p, &sz);
    if (!a || re15_emd_parse_animation(a, sz, &s_pl00) != 0) return 0;
    snprintf(p, sizeof p, "%s/PLD/PL00W01.EDD", RE15_ASSET_PSX_DIR);
    uint8_t *b = read_file(p, &sz);
    if (!b || re15_emd_parse_animation(b, sz, &s_w01) != 0) return 0;
    return 1;
}

static re15_rdt_t    s_rdt;
static re15_actor_t *s_pl;

/* Raum in den Auslieferungs-Zustand: Flag (3,102) entscheidet die Lage des Tritts (sub00). */
static void reset_room(int flag_3_102)
{
    re15_actor_init();
    scd_vm_init();
    re15_climb_reset();
    re15_game_flag_set(3, 102, flag_3_102);
    g_current_room_id = 0x10F0;
    g_room_change.pending = 0;
    s_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_pl->active = 1; s_pl->type = 0; s_pl->hp = 100;
    /* Tuer-Spawn aus ROOM10D0 main00 @0x01174: (8400, 0, -350), Gierung 2048. */
    s_pl->x = 8400; s_pl->y = 0; s_pl->z = -350; s_pl->rot_y = 2048;
    scd_room_reenter(&s_rdt, s_pl->x, s_pl->z, 0);
    re15_collision_set_band(re15_collision_band_from_y(s_pl->y));
}

static int find_tritt(void)
{
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].obj_id == 0 && g_scd.props[i].obj_type == 4) return i;
    return -1;
}

/* Vor den Tritt stellen: Blick auf den Tritt entlang der Achse `yaw` (0=+X, 1024=-Z,
 * 2048=-X, 3072=+Z). Abstand = Boxkante + Spielerradius 450 (dort stoppt der Push-Out). */
static void stand_before(int p, int yaw)
{
    const int32_t cx = g_scd.props[p].x + g_scd.props[p].box_cx;
    const int32_t cz = g_scd.props[p].z + g_scd.props[p].box_cz;
    const int32_t hx = g_scd.props[p].box_hx, hz = g_scd.props[p].box_hz;
    switch (yaw) {
    case 0:    s_pl->x = cx - hx - 450; s_pl->z = cz;            break;   /* steht westlich, Blick +X */
    case 1024: s_pl->x = cx;            s_pl->z = cz + hz + 450; break;   /* steht suedlich, Blick -Z */
    case 2048: s_pl->x = cx + hx + 450; s_pl->z = cz;            break;   /* steht oestlich, Blick -X */
    default:   s_pl->x = cx;            s_pl->z = cz - hz - 450; break;   /* steht noerdlich, Blick +Z */
    }
    s_pl->y = (int32_t)g_scd.props[p].y;       /* gleiche Ebene wie der Tritt */
    s_pl->rot_y = (int16_t)yaw;
    re15_collision_set_band((int)g_scd.props[p].band);
    re15_climb_standing_tick();
}

static void diag_sonde(const char *tag, int p)
{
    /* Dieselbe Sonde wie FUN_8002d474: Punkt 800 voraus auf dem 90-Grad-gerasteten Kurs. */
    const int a = (int)(uint16_t)(((uint16_t)s_pl->rot_y + 512u) & 0xFC00u) & 0xFFF;
    const int32_t cs = re15_cos_q12(a), sn = re15_sin_q12(a);
    const int32_t fx = s_pl->x + (int32_t)((800 * cs) >> 12);
    const int32_t fz = s_pl->z - (int32_t)((800 * sn) >> 12);
    const int band = re15_collision_debug_band();
    const uint16_t s6 = re15_collision_floor_typeword(&s_rdt, fx, fz, band, 0);
    printf("[%s] spieler=(%ld,%ld,%ld) rot=%d band=%d sonde=(%ld,%ld) s6(typeword)=0x%04X "
           "obj%d flags=0x%04X band=%d pos=(%ld,%ld,%ld) box c(%d,%d,%d) h(%d,%d,%d) "
           "reach=%d boxhit(-112)=%d stand=%d\n",
           tag, (long)s_pl->x, (long)s_pl->y, (long)s_pl->z, (int)s_pl->rot_y, band,
           (long)fx, (long)fz, s6, p, g_scd.props[p].flags, (int)g_scd.props[p].band,
           (long)g_scd.props[p].x, (long)g_scd.props[p].y, (long)g_scd.props[p].z,
           g_scd.props[p].box_cx, g_scd.props[p].box_cy, g_scd.props[p].box_cz,
           g_scd.props[p].box_hx, g_scd.props[p].box_hy, g_scd.props[p].box_hz,
           re15_prop_reach_test(s_pl->x, s_pl->z, s_pl->rot_y, p, 0),
           re15_collision_prop_box_hit(p, fx, fz, -112, (int)g_scd.props[p].band),
           re15_climb_dbg_standing());
}

/* Den ganzen Aufstieg fahren; Rueckgabe = Bilder bis Substate 0, -1 = nie beendet. */
static int aufstieg(int *y_end, int *band_end)
{
    int f_end = -1;
    for (int f = 0; f < 300 && re15_climb_active(); f++) {
        re15_climb_tick(&s_rdt, NULL, &s_pl00, &s_w01);
        re15_climb_standing_tick();
        if (!re15_climb_active()) f_end = f;
    }
    *y_end = (int)s_pl->y;
    *band_end = re15_collision_debug_band();
    return f_end;
}

int main(int argc, char **argv)
{
    const char *mode = (argc > 1) ? argv[1] : "diag";
    size_t sz = 0;
    uint8_t *raw = read_file(RE15_ASSET_PSX_DIR "/STAGE1/ROOM10F0.RDT", &sz);
    if (!raw) { printf("SKIP: ROOM10F0.RDT fehlt\n"); return 77; }
    if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("SKIP: RDT-Parse\n"); return 77; }
    if (!load_banks()) { printf("SKIP: PL00/PL00W01-Baenke fehlen\n"); return 77; }

    if (!strcmp(mode, "diag")) {
        for (int fl = 0; fl < 2; fl++) {
            reset_room(fl);
            int p = find_tritt();
            printf("--- Flag(3,102)=%d: prop_count=%d tritt=%d\n", fl, (int)g_scd.prop_count, p);
            for (int i = 0; i < (int)g_scd.prop_count; i++)
                printf("    prop[%d] id=%d type=%d active=%d flags=0x%04X band=%d pos=(%ld,%ld,%ld)\n",
                       i, g_scd.props[i].obj_id, g_scd.props[i].obj_type, g_scd.props[i].active,
                       g_scd.props[i].flags, g_scd.props[i].band,
                       (long)g_scd.props[i].x, (long)g_scd.props[i].y, (long)g_scd.props[i].z);
            if (p < 0) continue;
            static const int yaws[4] = {0, 1024, 2048, 3072};
            for (int k = 0; k < 4; k++) {
                reset_room(fl); p = find_tritt();
                stand_before(p, yaws[k]);
                char tag[32]; snprintf(tag, sizeof tag, "f%d yaw%d", fl, yaws[k]);
                diag_sonde(tag, p);
                int r = re15_climb_try_start(&s_rdt, 1);
                int ye = 0, be = 0, fe = -1;
                if (r) fe = aufstieg(&ye, &be);
                printf("    try_start=%d sub=%d kind=0x%02X -> ende_bild=%d y=%d band=%d\n",
                       r, re15_climb_dbg_sub(), re15_climb_dbg_kind(), fe, ye, be);
                re15_climb_reset();
            }
        }
        return 0;
    }

    /* ---- PIN: Aufstieg auf den Tritt (Endlage: Flag (3,102)=1 / Startlage: =0) ------------ */
    const int fl = (!strcmp(mode, "pin_start")) ? 0 : 1;
    reset_room(fl);
    int p = find_tritt();
    chk("P0 Tritt (obj 0, Typ 4) installiert", p >= 0, 1);
    if (p < 0) return 1;
    chk("P0 Tritt aktiv (obj[0]&1 @0x8002d170)", (long)(g_scd.props[p].flags & 1u), 1);
    chk("P0 Tritt-Flags = pc[6..7]|1 (@0x80040990-a4)", (long)g_scd.props[p].flags, 0x0001);
    chk("P0 Tritt-Band", (long)g_scd.props[p].band, 0);
    chk("P0 Tritt-X", (long)g_scd.props[p].x, fl ? -4850 : -6050);
    chk("P0 Tritt-Z", (long)g_scd.props[p].z, fl ? -850 : 4950);

    /* Von Sueden (Blick -Z) — die Seite, von der man in der Endlage herankommt (Tuer-Zone
     * nach ROOM1090 liegt noerdlich des Tritts, Door_aot_set Slot 1 @0x00F52 floor 1). */
    stand_before(p, 1024);
    diag_sonde(fl ? "pin end" : "pin start", p);
    chk("P1 Sonde: Objekt in Reichweite (FUN_8002d1e8)",
        re15_prop_reach_test(s_pl->x, s_pl->z, s_pl->rot_y, p, 0), 1);
    chk("P2 try_start -> Klettern beginnt (@0x8002d604)", re15_climb_try_start(&s_rdt, 1), 1);
    chk("P2 Substate 9", re15_climb_dbg_sub(), 9);
    chk("P2 KIND 0x81 (@0x8002d614/18: nur wenn s5 != 0)", re15_climb_dbg_kind(), 0x81);
    chk("P2 getroffenes Objekt (DAT_800ac78c)", re15_climb_dbg_obj(), p);

    int ye = 0, be = 0;
    int fe = aufstieg(&ye, &be);
    chk("P3 Aufstieg endet (Substate 0)", fe >= 0, 1);
    chk("P3 Endhoehe = Oberkante obj.y - 2*box_hy", ye, (int)g_scd.props[p].y - 2 * g_scd.props[p].box_hy);
    chk("P3 Endband = Tritt-Band + 1 (@0x80038200-08)", be, (int)g_scd.props[p].band + 1);
    chk("P4 steht oben AUF dem Tritt (DAT_800ac788)", re15_climb_dbg_standing(), p);
    /* In der Endlage liegt der Spieler oben in der Tuer-Zone-Hoehe (floor 1); die Door_aot
     * Zone selbst (x -5350..-4150, z -2250..-1250) wird vom Tuer-Scan geprueft, nicht hier. */
    printf("    oben: (%ld,%ld,%ld) band=%d\n", (long)s_pl->x, (long)s_pl->y, (long)s_pl->z, be);

    printf(g_fail ? "FAIL %d\n" : "PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
