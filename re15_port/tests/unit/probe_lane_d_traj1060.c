/* probe_lane_d_traj1060.c — LANE D (Diagnose, kein ctest): Nutzer-Report 2026-08-06
 * "Im Treppenhaus schwenkt nach dem Runterlaufen der LETZTEN Treppe die Kamera
 *  TEILWEISE nicht richtig um."
 *
 * ZIEL: PRO-FRAME-TRAJEKTORIE des KOMPLETTEN Abstiegs durch alle vier ROOM1060-
 * Treppen + der exakte Bruchpunkt der RVD-Kamera-Zustandsmaschine.
 *
 * Protokolliert pro Frame: Frame, Phase, x, y, z, Band, angezeigter Cut, alle RVD-
 * Tabellenindizes, deren QUAD den Punkt enthaelt (mit Markierung, welcher davon
 * SCHARF ist, d.h. cam_from == angezeigter Cut), sowie ENTER/LEAVE-Marker je Zone.
 *
 * BELEGE (Original-Disassembly, PSX.EXE):
 *  - FUN_80014230 @0x80014230 = die RVD-Zustandsmaschine:
 *      @0x80014234 `lw a0,-14444(a0)`  -> a0 = DAT_800ac794 = Gruppen-Anker des
 *                                        ANGEZEIGTEN Cuts
 *      @0x8001423c `lbu v0,-1099(v0)`  -> v0 = DAT_800afbb5 = angezeigter Cut
 *      @0x80014254 `lbu v1,22(a0)`     -> cam_from des ZWEITEN Gruppeneintrags
 *      @0x8001425c `bne v1,v0,0x800142d8` -> passt nicht: SOFORT return (kein Scan)
 *      @0x8001426c `lbu v1,-1(s0)` / @0x80014274 `beq v1,0xff` / @0x80014288
 *                  `bne v1,player.floor` -> Floor-Gate (0xFF = jeder Floor)
 *      @0x80014298 `jal 0x80014368`    -> Punkt-in-Quad
 *      @0x800142a8 `lbu a0,1(s0)` (= Eintrag+3 = cam_to) / @0x800142ac
 *                  `jal 0x800142f4` / @0x800142b4 `j 0x800142d8` -> ERSTER Treffer
 *                  gewinnt, danach RETURN
 *      @0x800142bc `addiu s0,s0,20` / @0x800142d0 `beq v1,v0,0x8001426c` -> es wird
 *                  NUR die zusammenhaengende Gruppe mit cam_from == Cut gescannt,
 *                  beginnend beim ZWEITEN Eintrag (Anker uebersprungen)
 *  - LAB_80038e50 @0x80038e50-0x80038edc = Treppen-FINALIZE: schreibt
 *      0x800acae8=2 (Standby-Clip), 0x800acae9=0, 0x800aca59/5a/5b=0 und
 *      @0x80038edc `sb v0,player.floor(+0x82)` = Band aus dem committeten Y.
 *      Es gibt in diesem ganzen Block KEINEN Store nach player.X (0x800b0000)
 *      oder player.Z (0x800b0008) -> das Original VERSETZT den Spieler im
 *      Finalize NICHT.
 *  - @0x80038e00 `addiu v0,v0,1800` -> Bandhoehe 0x708 = 1800.
 *
 * Die Treppen laufen ueber den ECHTEN Port-Code (re15_stair_try_start/tick mit
 * PL00-Skelett), der Scan ueber den ECHTEN re15_aot_scan.
 *
 * TRIAL-PATCHES (NUR in dieser Sonde, KEIN Engine-Code):
 *   MODE_NOEJECT  — x/z nach dem Finalize-Tick auf den Stand VOR dem Tick
 *                   zuruecksetzen (= der vom Original belegte "kein Positions-
 *                   Store"-Zustand). Beantwortet den Hauptverdacht.
 *   MODE_SUBSTEP  — zwischen zwei aufeinanderfolgenden Frames zusaetzlich N
 *                   Zwischenpunkte scannen. Beantwortet "ueberspringt die
 *                   Schrittweite Zonen?".
 *
 * Anlaufpunkte/Blickrichtungen werden NICHT geraten, sondern aus den Treppen-
 * Record-Bytes (corner/extent/side, sce-13-Handler LAB_800435cc) abgeleitet:
 * side=0 -> unteres Ende bei NIEDRIGEM z, side=1 -> unteres Ende bei HOHEM z.
 * Der Spieler stellt sich 400u VOR die Rechteckkante und blickt bergab, so dass
 * die byte-true 620er-Vorwaertssonde (`ori v0,zero,0x26c` @0x80042bd0) im Rechteck
 * landet (Sonden-Test an sce_flags-Bit 0x20 @0x80042ef8; die Records tragen 0x31).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_aot.h"
#include "re15_emd.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

#define STAIR_APPROACH_GAP 400   /* Abstand vor der Rechteckkante; < 620 (Sondenreichweite) */
static int32_t g_gap = STAIR_APPROACH_GAP;   /* per Lauf variierbar */
#define MAXZ RE15_RDT_MAX_ZONES

static re15_rdt_t g_rdt;
static re15_emd_skeleton_t  g_skel;
static re15_emd_animation_t g_anim;
static int g_skel_ok = 0;

/* ---- Laufzeitzustand der Sonde ---- */
static int   g_cut;
static int   g_frame;
static int   g_verbose = 1;
static int   g_mode_noeject = 0;
static int   g_mode_sweep_eject = 0;   /* Eject in 100u-Schritten SCANNEN statt teleportieren */
static int   g_substep = 0;      /* 0 = aus, sonst Anzahl Zwischenabtastungen */

static int   g_zone_slot[MAXZ];     /* RVD-Tabellenindex -> AOT-Slot (-1 = Anker/nicht installiert) */
static int   g_zone_in[MAXZ];       /* letzter Inside-Zustand (fuer ENTER/LEAVE) */
static int   g_zone_ever[MAXZ];     /* jemals betreten */
static int   g_zone_ever_armed[MAXZ]; /* jemals betreten WAEHREND cam_from == Cut */
static int   g_zone_first_f[MAXZ];  /* Frame des ersten Betretens */
static int32_t g_zone_first_x[MAXZ], g_zone_first_z[MAXZ];

static int32_t g_prev_x, g_prev_z;
static int   g_prev_valid = 0;

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

/* Anker-Regel wie rdt_common.c:431 (PSX FUN_80014324: der ERSTE Eintrag je cam_from
 * ist der Suchanker und wird vom Scan NIE getestet). */
static int zone_is_anchor(int i)
{
    return (i == 0 || g_rdt.zones[i - 1].cam_from != g_rdt.zones[i].cam_from);
}

static void build_zone_slot_map(void)
{
    int installed = 0;
    for (int i = 0; i < MAXZ; i++) g_zone_slot[i] = -1;
    for (int i = 0; i < g_rdt.zone_count; i++) {
        if (zone_is_anchor(i)) continue;
        int slot = RE15_AOT_MAX - 1 - installed;   /* rdt_common.c:439 */
        if (slot < 16) break;
        g_zone_slot[i] = slot;
        installed++;
    }
}

static int point_in_zone_quad(int32_t x, int32_t z, int zi)
{
    const re15_rdt_zone_t *zn = &g_rdt.zones[zi];
    return re15_aot_point_in_quad(x, z, zn->xs, zn->zs);
}

/* Welchen Slot WUERDE re15_aot_scan waehlen? (aot_common.c:611-707: aufsteigender
 * Slot-Scan, LETZTER Inside-Treffer gewinnt = hoechster Slot = niedrigster
 * Tabellenindex = byte-true "erster Eintrag in Tabellenordnung".) */
static int engine_pick_zone(int32_t x, int32_t z, int cut)
{
    int best = -1;
    for (int s = 0; s < RE15_AOT_MAX; s++) {
        const re15_aot_t *a = &g_aot.slots[s];
        if (!a->active || a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;
        if (a->cam_from_filter != 0xFF && a->cam_from_filter != (uint8_t)cut) continue;
        int inside = a->has_quad ? re15_aot_point_in_quad(x, z, a->xs, a->zs)
                                 : 0;
        if (!inside) continue;
        int cam_pb = re15_collision_debug_band();
        int floor_ok = (a->band == 0xFF) || (cam_pb < 0) || ((int)a->band == cam_pb);
        if (!floor_ok) continue;
        for (int i = 0; i < g_rdt.zone_count; i++) if (g_zone_slot[i] == s) { best = i; break; }
    }
    return best;
}

/* Ein Frame: Zonen-Buchfuehrung + der ECHTE Scan. */
static void frame_tick(const char *phase)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t x = pl->x, z = pl->z;
    int band = re15_collision_band_from_y(pl->y);

    /* Zwischenabtastung (MODE_SUBSTEP): scannt die Strecke zwischen dem letzten und
     * diesem Frame, um zu sehen, ob ein grosser Sprung eine Zone UEBERSPRINGT. */
    char sub[220]; sub[0] = 0;
    if (g_substep > 0 && g_prev_valid) {
        int hits[MAXZ]; memset(hits, 0, sizeof hits);
        for (int k = 1; k < g_substep; k++) {
            int32_t ix = g_prev_x + (int32_t)(((int64_t)(x - g_prev_x) * k) / g_substep);
            int32_t iz = g_prev_z + (int32_t)(((int64_t)(z - g_prev_z) * k) / g_substep);
            for (int i = 0; i < g_rdt.zone_count; i++) {
                if (zone_is_anchor(i)) continue;
                if (g_rdt.zones[i].cam_from != (uint8_t)g_cut) continue;
                if (point_in_zone_quad(ix, iz, i)) hits[i] = 1;
            }
        }
        int sl = 0;
        for (int i = 0; i < g_rdt.zone_count; i++) {
            if (!hits[i]) continue;
            /* nur melden, wenn der Endpunkt NICHT drin ist -> echter Ueberspringer */
            if (point_in_zone_quad(x, z, i)) continue;
            sl += snprintf(sub + sl, sizeof sub - sl, " SKIPPED[%d]%d>%d", i,
                           (int)g_rdt.zones[i].cam_from, (int)g_rdt.zones[i].cam_to);
            if (sl > 180) break;
        }
    }

    /* Zonen-Inside-Liste + ENTER/LEAVE */
    char zin[300]; int zl = 0; zin[0] = 0;
    char evt[300]; int el = 0; evt[0] = 0;
    for (int i = 0; i < g_rdt.zone_count; i++) {
        if (zone_is_anchor(i)) continue;
        int in = point_in_zone_quad(x, z, i);
        int armed = ((int)g_rdt.zones[i].cam_from == g_cut);
        if (in) {
            if (zl < 240)
                zl += snprintf(zin + zl, sizeof zin - zl, " %s[%d]%d>%d%s",
                               armed ? "*" : "", i,
                               (int)g_rdt.zones[i].cam_from, (int)g_rdt.zones[i].cam_to,
                               armed ? "*" : "");
            if (!g_zone_ever[i]) {
                g_zone_ever[i] = 1; g_zone_first_f[i] = g_frame;
                g_zone_first_x[i] = x; g_zone_first_z[i] = z;
            }
            if (armed) g_zone_ever_armed[i] = 1;
        }
        if (in != g_zone_in[i]) {
            if (el < 240)
                el += snprintf(evt + el, sizeof evt - el, " %s[%d]%d>%d",
                               in ? "ENTER" : "LEAVE", i,
                               (int)g_rdt.zones[i].cam_from, (int)g_rdt.zones[i].cam_to);
            g_zone_in[i] = in;
        }
    }

    int pick = engine_pick_zone(x, z, g_cut);

    /* ECHTER Scan */
    g_scd.cut_auto_enabled = 1;
    re15_aot_scan(x, z, (uint8_t)g_cut);
    int newcut = g_cut;
    if (g_scd.cam_change_pending) {
        newcut = (int)g_scd.cam_id;
        g_scd.cam_change_pending = 0;
    }

    int32_t dx = g_prev_valid ? (x - g_prev_x) : 0;
    int32_t dz = g_prev_valid ? (z - g_prev_z) : 0;

    if (g_verbose) {
        printf("F%-4d %-9s (%6ld,%6ld,%6ld) b%d cut=%d d=(%+5ld,%+5ld)%s%s%s%s\n",
               g_frame, phase, (long)x, (long)pl->y, (long)z, band, g_cut,
               (long)dx, (long)dz,
               zin, evt, sub,
               (newcut != g_cut) ? "  <<<< CUT-WECHSEL" : "");
    }
    if (newcut != g_cut) {
        if (!g_verbose)
            printf("      F%-4d CUT %d -> %-2d @(%ld,%ld) b%d via RVD[%d]\n",
                   g_frame, g_cut, newcut, (long)x, (long)z, band, pick);
        else
            printf("      -> CUT %d -> %d via RVD[%d]\n", g_cut, newcut, pick);
        g_cut = newcut;
    }
    g_prev_x = x; g_prev_z = z; g_prev_valid = 1;
    g_frame++;
}

/* Gerader Marsch (Probe-Hilfsmittel; die Kamera interessiert nur der ueberstrichene
 * Punkt). step = Einheiten/Frame. Kein Collision-Constrain: die Treppenhaus-
 * Plattformen sind begehbar, und ein Constrain wuerde das Ergebnis von der
 * Kollisionsgeometrie abhaengig machen statt von der Kamera-Zustandsmaschine. */
static void march(int32_t tx, int32_t tz, int rot, int32_t step, const char *phase)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->rot_y = (int16_t)rot;
    for (int i = 0; i < 8000; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        int32_t ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        if (ad == 0) break;
        int32_t sx = (dx > step) ? step : ((dx < -step) ? -step : dx);
        int32_t sz = (dz > step) ? step : ((dz < -step) ? -step : dz);
        pl->x += sx; pl->z += sz;
        frame_tick(phase);
    }
}

/* Treppe: Anlaufpunkt + Blickrichtung aus dem RECORD abgeleitet. */
typedef struct { int slot; int32_t app_x, app_z; int rot; const char *name; } stair_plan_t;

/* Anlaufpunkt eines Treppen-Records aus den Record-Bytes:
 *   side=0 -> unteres Ende bei NIEDRIGEM z -> oberes Ende bei corner+extent, bergab = -z (rot 1024)
 *   side=1 -> unteres Ende bei HOHEM z    -> oberes Ende bei corner,        bergab = +z (rot 3072)
 * (Halb-Test + side-Auswertung: LAB_800435cc @0x800435dc-e8 / @0x800435ec-f0 / @0x80043530-90.) */
static void stair_top_of(int slot, int32_t *app_z, int *rot, int32_t *top_edge)
{
    const re15_aot_stair_params_t *sp = &g_aot.stair_params[slot];
    int32_t lo = sp->corner, hi = sp->corner + sp->extent;
    if (sp->side == 0) { *top_edge = hi; *app_z = hi + g_gap; *rot = 1024; }
    else               { *top_edge = lo; *app_z = lo - g_gap; *rot = 3072; }
}

/* Auf Band B gibt es ZWEI Records (Fuss der eben benutzten Treppe + Kopf der naechsten).
 * Der Kopf der NAECHSTEN Treppe ist der, dessen Anlaufpunkt dem aktuellen Standort am
 * naechsten liegt — die Plattform ist zusammenhaengend. (Probe-Auswahlregel, KEINE
 * Original-Behauptung; die Richtung selbst kommt aus dem Record.) */
static int plan_stair(int band, stair_plan_t *out, const char *name)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int best = -1; int32_t bestd = 0;
    for (int s = 0; s < RE15_AOT_MAX; s++) {
        const re15_aot_t *a = &g_aot.slots[s];
        if (!a->active || a->type != RE15_AOT_TYPE_STAIR) continue;
        if ((int)a->band != band) continue;
        if (g_aot.stair_params[s].axis != 13) continue;  /* ROOM1060: alle vier sind Z-Treppen */
        int32_t az, te; int rt;
        stair_top_of(s, &az, &rt, &te);
        int32_t d = (az - pl->z < 0) ? (pl->z - az) : (az - pl->z);
        if (best < 0 || d < bestd) { best = s; bestd = d; }
    }
    if (best < 0) return 0;
    int32_t te;
    stair_top_of(best, &out->app_z, &out->rot, &te);
    out->app_x = g_aot.slots[best].x;
    out->slot  = best;
    out->name  = name;
    return 1;
}

static void descend(const stair_plan_t *sp)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->rot_y = (int16_t)sp->rot;
    printf("  ---- %s: Ausloesen @(%ld,%ld) rot=%d band=%d cut=%d\n",
           sp->name, (long)pl->x, (long)pl->z, sp->rot,
           re15_collision_debug_band(), g_cut);
    re15_stair_reset();
    if (!re15_stair_try_start(&g_rdt, 1)) {
        printf("       !! KEIN Stair-Start (Geometrie-Gate)\n");
        return;
    }
    printf("       Start %s, rot nach Snap=%d\n",
           pl->motion == (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN ? "DOWN" : "UP",
           (int)pl->rot_y);
    int n = 0;
    while (re15_stair_active() && n < 900) {
        int32_t bx = pl->x, bz = pl->z;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        int was_finalize = !re15_stair_active();
        if (was_finalize && g_mode_noeject) {
            /* TRIAL-PATCH (nur Sonde): LAB_80038e50 hat KEINEN X/Z-Store
             * (@0x80038e50-0x80038edc disassembliert) -> Position zuruecksetzen. */
            int32_t ex = pl->x - bx, ez = pl->z - bz;
            pl->x = bx; pl->z = bz;
            if (ex || ez) printf("       [NOEJECT] Eject unterdrueckt: (%+ld,%+ld)\n",
                                 (long)ex, (long)ez);
        } else if (was_finalize) {
            int32_t ex = pl->x - bx, ez = pl->z - bz;
            if (ex || ez) printf("       [EJECT] Finalize versetzt: (%+ld,%+ld)\n",
                                 (long)ex, (long)ez);
        }
        frame_tick(was_finalize ? "stair-fin" : "stair");
        n++;
    }
    printf("       Ende nach %d Frames @(%ld,%ld,%ld) band=%d cut=%d\n",
           n, (long)pl->x, (long)pl->y, (long)pl->z,
           re15_collision_band_from_y(pl->y), g_cut);
}

static void reset_bookkeeping(int start_cut)
{
    memset(g_zone_in, 0, sizeof g_zone_in);
    memset(g_zone_ever, 0, sizeof g_zone_ever);
    memset(g_zone_ever_armed, 0, sizeof g_zone_ever_armed);
    memset(g_zone_first_f, 0, sizeof g_zone_first_f);
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
    g_cut = start_cut; g_frame = 0; g_prev_valid = 0;
    g_scd.cam_id = (uint8_t)start_cut; g_scd.cam_change_pending = 0;
}

static void load_room(const char *base)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s\n", path); exit(1); }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { fprintf(stderr, "FAIL parse\n"); exit(1); }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 26000; pl->y = -8 * 0x708; pl->z = 25300;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();
    build_zone_slot_map();
}

/* Der komplette Abstieg. bands[] = die Baender, auf denen der Spieler jeweils steht. */
static void run_chain(const char *label, int start_cut, int32_t march_step,
                      int noeject, int substep, int verbose)
{
    static const int bands[4] = { 8, 6, 4, 2 };
    static const char *names[4] = { "Treppe 1 (Band 8->6)", "Treppe 2 (Band 6->4)",
                                    "Treppe 3 (Band 4->2)", "Treppe 4 (Band 2->0) = LETZTE" };
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    g_mode_noeject = noeject; g_substep = substep; g_verbose = verbose;

    printf("\n================================================================\n");
    printf("== %s   (start_cut=%d march_step=%ld noeject=%d substep=%d)\n",
           label, start_cut, (long)march_step, noeject, substep);
    printf("================================================================\n");

    /* Spawn = ROOM1120 Door slot0 @Datei 0x0c76 -> (26000,-14400,25300), Band 8. */
    pl->x = 26000; pl->y = -8 * 0x708; pl->z = 25300; pl->rot_y = 1024;
    pl->motion = 0; pl->anim_frame = 0; pl->anim_flags = 0;
    re15_collision_set_band(8);
    re15_stair_reset();
    reset_bookkeeping(start_cut);
    printf("Spawn (%ld,%ld,%ld) band=8 cut=%d\n", (long)pl->x, (long)pl->y, (long)pl->z, g_cut);

    for (int k = 0; k < 4; k++) {
        stair_plan_t sp;
        if (!plan_stair(bands[k], &sp, names[k])) {
            printf("  !! kein Treppen-Record fuer Band %d\n", bands[k]); break;
        }
        printf("  -> Anlauf zu (%ld,%ld) rot=%d [Record: slot=%d corner=%ld extent=%ld side=%d count=%d]\n",
               (long)sp.app_x, (long)sp.app_z, sp.rot, sp.slot,
               (long)g_aot.stair_params[sp.slot].corner,
               (long)g_aot.stair_params[sp.slot].extent,
               (int)g_aot.stair_params[sp.slot].side,
               (int)g_aot.stair_params[sp.slot].count);
        /* Anlauf: erst x, dann z (die Plattformen sind L-foermig um den Schacht). */
        march(sp.app_x, pl->z, sp.rot, march_step, "walk-x");
        march(sp.app_x, sp.app_z, sp.rot, march_step, "walk-z");
        descend(&sp);
    }

    /* Nach der letzten Treppe zur Ausgangstuer ROOM1040 (Door slot2 @0x0d52, band 0). */
    printf("  ---- Weg zur Ausgangstuer (27100,25400)\n");
    march(pl->x, 25400, 3072, march_step, "exit-z");
    march(27100, 25400, 0, march_step, "exit-x");

    printf("\nERGEBNIS %s: END-CUT = %d @(%ld,%ld) band=%d  (Frames=%d)\n",
           label, g_cut, (long)pl->x, (long)pl->z,
           re15_collision_band_from_y(pl->y), g_frame);

    printf("ZONEN-BILANZ (nur Nicht-Anker):\n");
    for (int i = 0; i < g_rdt.zone_count; i++) {
        if (zone_is_anchor(i)) continue;
        printf("   RVD[%2d] %d->%-2d  betreten=%-3s  scharf-betreten=%-3s",
               i, (int)g_rdt.zones[i].cam_from, (int)g_rdt.zones[i].cam_to,
               g_zone_ever[i] ? "JA" : "NEIN", g_zone_ever_armed[i] ? "JA" : "NEIN");
        if (g_zone_ever[i])
            printf("  erstmals F%d @(%ld,%ld)", g_zone_first_f[i],
                   (long)g_zone_first_x[i], (long)g_zone_first_z[i]);
        printf("\n");
    }
}

/* ================================================================================
 * TRIGGER-Z-SWEEP: der Ausloesepunkt einer Treppe ist im Original NICHT ein Punkt,
 * sondern ein ganzes Intervall — die byte-true Sonde liegt 620u (`ori v0,zero,0x26c`
 * @0x80042bd0) vor dem Spieler und muss im Rechteck landen (Sonden-Test an
 * sce_flags-Bit 0x20, @0x80042ef8; die ausgelieferten Records tragen 0x31, d.h. der
 * Positions-Test @0x80042ea8 (Bit 0x40) laeuft NICHT). Der Spieler kann also ueberall
 * zwischen "Sonde beruehrt gerade die Kante" und "Spieler steht selbst im Rechteck"
 * ausloesen. Diese Sonde faehrt das GANZE Intervall ab und misst pro Startpunkt, ob
 * die zustaendige RVD-Zone feuert.
 * ============================================================================== */
static void trigger_sweep(int slot, int band, int start_cut, int expect_zone,
                          int32_t z_from, int32_t z_to, int32_t z_step, const char *label)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const re15_aot_t *a = &g_aot.slots[slot];
    int32_t az, te; int rot;
    stair_top_of(slot, &az, &rot, &te);
    const re15_rdt_zone_t *ez = &g_rdt.zones[expect_zone];

    printf("\n--- TRIGGER-SWEEP %s  slot=%d band=%d cut_in=%d  Soll-Zone RVD[%d] %d->%d z[%d..%d]\n",
           label, slot, band, start_cut, expect_zone,
           (int)ez->cam_from, (int)ez->cam_to,
           (int)(ez->cz - ez->half_h), (int)(ez->cz + ez->half_h));
    printf("    Rechteck z[%ld..%ld], Oberkante=%ld, bergab rot=%d\n",
           (long)(a->z - a->half_h), (long)(a->z + a->half_h), (long)te, rot);
    printf("    trigZ | Start? |  gaitEndZ | EjectDZ |  endZ  | Zone[%2d] betreten | Cut-Folge\n", expect_zone);

    g_verbose = 0; g_substep = 0;
    for (int32_t tz = z_from; tz <= z_to; tz += z_step) {
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = a->x; pl->z = tz; pl->y = -(int32_t)band * 0x708;
        pl->rot_y = (int16_t)rot;
        pl->motion = 0; pl->anim_frame = 0; pl->anim_flags = 0;
        re15_stair_reset();
        re15_collision_set_band(band);
        reset_bookkeeping(start_cut);

        if (!re15_stair_try_start(&g_rdt, 1)) {
            printf("  %7ld | NEIN   (Sonde/Rect-Gate)\n", (long)tz);
            continue;
        }
        char seq[160]; int sl = 0;
        sl += snprintf(seq + sl, sizeof seq - sl, "%d", g_cut);
        int32_t gait_z = pl->z, ej_z = 0;
        int n = 0, hit = 0;
        while (re15_stair_active() && n < 900) {
            int32_t bz = pl->z, bx0 = pl->x;
            re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
            int fin = !re15_stair_active();
            if (fin) { gait_z = bz; ej_z = pl->z - bz; }
            if (g_mode_noeject && fin) pl->z = bz, pl->x = pl->x;   /* z-Eject unterdruecken */
            /* VORSCHLAG-SIMULATION: den Eject als GESCHWENKTE Strecke behandeln — der
             * Scan sieht jeden 100u-Zwischenpunkt statt nur den Endpunkt. Genau das,
             * was der Original-Gang tut (er bewegt sich nie schneller als ~130u/Tick). */
            if (g_mode_sweep_eject && fin) {
                int32_t ex = pl->x, ez2 = pl->z, sxp = ex - bx0, szp = ez2 - bz;
                int steps = (int)((sxp < 0 ? -sxp : sxp) + (szp < 0 ? -szp : szp)) / 100 + 1;
                for (int k = 1; k <= steps; k++) {
                    pl->x = bx0 + (int32_t)(((int64_t)sxp * k) / steps);
                    pl->z = bz  + (int32_t)(((int64_t)szp * k) / steps);
                    if (point_in_zone_quad(pl->x, pl->z, expect_zone)) hit = 1;
                    int b4 = g_cut;
                    frame_tick("ejsweep");
                    if (g_cut != b4 && sl < (int)sizeof seq - 12)
                        sl += snprintf(seq + sl, sizeof seq - sl, "->%d(ej%d)", g_cut, k);
                }
                pl->x = ex; pl->z = ez2;
                n++; continue;
            }
            if (point_in_zone_quad(pl->x, pl->z, expect_zone)) hit = 1;
            int before = g_cut;
            frame_tick("sweep");
            if (g_cut != before && sl < (int)sizeof seq - 12)
                sl += snprintf(seq + sl, sizeof seq - sl, "->%d(F%d)", g_cut, n);
            n++;
        }
        printf("  %7ld | JA %s | %9ld | %+7ld | %6ld | %-17s | %-22s %s\n",
               (long)tz,
               pl->motion == (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN ? "AB" : "AUF",
               (long)gait_z, (long)ej_z, (long)pl->z,
               hit ? "JA" : "NEIN  <<<<<<<<", seq,
               (g_cut == (int)ez->cam_to) ? "" : "  <<<< KAMERA SCHWENKT NICHT");
    }
}

/* Wie weit kommt der Spieler mit der ECHTEN Kollision an die Treppe heran?
 * Entscheidet, ob das gemessene Fehlerfenster im Spiel ueberhaupt erreichbar ist. */
static void collision_reach(int slot, int band, const char *label)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const re15_aot_t *a = &g_aot.slots[slot];
    int32_t az, te; int rot;
    stair_top_of(slot, &az, &rot, &te);
    int dir = (rot == 3072) ? +1 : -1;                 /* bergab-Richtung in z */
    int32_t probe_edge = te - dir * 620;               /* fruehester Ausloesepunkt (Sonde 620) */
    pl->y = -(int32_t)band * 0x708;
    re15_collision_set_band(band);
    printf("  %s: slot=%d band=%d Oberkante z=%ld bergab=%s  fruehester byte-true Trigger z=%ld\n",
           label, slot, band, (long)te, dir > 0 ? "+z" : "-z", (long)probe_edge);
    /* Begehbare z-Intervalle bei x = Rechteckmitte (constrain laesst begehbare Punkte
     * unveraendert). Zeigt, wo der Spieler tatsaechlich stehen kann. */
    printf("      begehbar bei x=%ld:", (long)a->x);
    int open = 0; int32_t seg = 0;
    for (int32_t z = 17000; z <= 28000; z += 25) {
        int32_t cx = a->x, cz = z;
        re15_collision_constrain(&g_rdt, a->x, z, &cx, &cz);
        int ok = (cx == a->x && cz == z);
        if (ok && !open) { open = 1; seg = z; }
        else if (!ok && open) { open = 0; printf(" [%ld..%ld]", (long)seg, (long)(z - 25)); }
    }
    if (open) printf(" [%ld..28000]", (long)seg);
    printf("\n");
}

/* Fehl-Trigger + anschliessender Lauf zur Ausgangstuer: erholt sich die Kamera,
 * wenn der Eject NICHT teleportiert? */
static void fail_then_walk(int32_t trig_z, int noeject)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const re15_aot_t *a = &g_aot.slots[9];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = a->x; pl->z = trig_z; pl->y = -2 * 0x708; pl->rot_y = 3072;
    pl->motion = 0; pl->anim_frame = 0; pl->anim_flags = 0;
    re15_stair_reset(); re15_collision_set_band(2);
    reset_bookkeeping(4);
    g_verbose = 0; g_substep = 0; g_mode_noeject = noeject;
    if (!re15_stair_try_start(&g_rdt, 1)) { printf("   trigZ=%ld: kein Start\n", (long)trig_z); return; }
    int n = 0;
    while (re15_stair_active() && n < 900) {
        int32_t bx = pl->x, bz = pl->z;
        re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
        if (!re15_stair_active() && noeject) { pl->x = bx; pl->z = bz; }
        frame_tick("f"); n++;
    }
    int cut_after_stair = g_cut; int32_t z_after = pl->z;
    march(pl->x, 25400, 3072, 40, "f");
    march(27100, 25400, 0, 40, "f");
    printf("   trigZ=%ld %-10s: nach Treppe z=%ld cut=%d  ->  an der Tuer cut=%d  %s\n",
           (long)trig_z, noeject ? "OHNE-Eject" : "MIT-Eject",
           (long)z_after, cut_after_stair, g_cut,
           (g_cut == 7) ? "OK (Tuer-Cut 7)" : "<<<< FALSCHER TUER-CUT");
}

int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    {
        char p2[600]; size_t s1 = 0, s2 = 0;
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EDD", base);
        uint8_t *edd = read_file(p2, &s1);
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EMR", base);
        uint8_t *emr = read_file(p2, &s2);
        if (edd && emr && re15_emd_parse_animation(edd, s1, &g_anim) == 0 &&
            re15_emd_parse_skeleton(emr, s2, &g_skel) == 0) g_skel_ok = 1;
        printf("PL00 %s\n", g_skel_ok ? "geladen" : "FEHLT (FK-Foot-Lock inaktiv!)");
    }
    load_room(base);

    printf("\nRVD-Tabelle ROOM1060 (%d Eintraege). Anker = erster je cam_from,\n"
           "wird von FUN_80014230 NIE getestet (@0x80014268 s0 = a0+22).\n", g_rdt.zone_count);
    for (int i = 0; i < g_rdt.zone_count; i++) {
        const re15_rdt_zone_t *zn = &g_rdt.zones[i];
        printf("  [%2d]%-7s from=%-2d to=%-2d floor=0x%02x slot=%-3d x[%d..%d] z[%d..%d]\n",
               i, zone_is_anchor(i) ? " ANKER" : "",
               (int)zn->cam_from, (int)zn->cam_to, (int)zn->floor, g_zone_slot[i],
               (int)(zn->cx - zn->half_w), (int)(zn->cx + zn->half_w),
               (int)(zn->cz - zn->half_h), (int)(zn->cz + zn->half_h));
    }
    printf("\nTREPPEN-RECORDS:\n");
    for (int s = 0; s < RE15_AOT_MAX; s++) {
        const re15_aot_t *a = &g_aot.slots[s];
        if (!a->active || a->type != RE15_AOT_TYPE_STAIR) continue;
        const re15_aot_stair_params_t *sp = &g_aot.stair_params[s];
        printf("  slot=%2d band=%d axis=%d corner=%ld extent=%ld side=%d count=%d rect x[%ld..%ld] z[%ld..%ld]\n",
               s, (int)a->band, (int)sp->axis, (long)sp->corner, (long)sp->extent,
               (int)sp->side, (int)sp->count,
               (long)(a->x - a->half_w), (long)(a->x + a->half_w),
               (long)(a->z - a->half_h), (long)(a->z + a->half_h));
    }

    /* A: Basislauf, volle Pro-Frame-Trajektorie. */
    run_chain("A BASIS (voll, pro Frame)", 0, 40, 0, 0, 1);
    /* B: Eject unterdrueckt (Hauptverdacht). */
    run_chain("B OHNE EJECT", 0, 40, 1, 0, 0);
    /* C: feine Schrittweite beim Laufen + Zwischenabtastung der Treppen-Frames. */
    run_chain("C FEIN (march_step=5, substep=16)", 0, 5, 0, 16, 0);
    /* D: Eject unterdrueckt UND fein. */
    run_chain("D OHNE EJECT + FEIN", 0, 5, 1, 16, 0);

    /* ---- Die entscheidenden Sweeps: das GANZE byte-true Ausloese-Intervall ---- */
    /* LETZTE Treppe (West, slot 9, Band 2->0, Kopf bei z=20700, bergab +z).
     * Byte-true erreichbar: Spieler-z ab 20700-620 = 20080 (Sonde beruehrt die Kante)
     * bis tief ins Rechteck. Zustaendige Zone = RVD[13] 4->5 z[24000..25000]. */
    g_mode_noeject = 0;
    trigger_sweep(9, 2, 4, 13, 20050, 21500, 25, "LETZTE Treppe (West 2->0), MIT Eject");
    g_mode_noeject = 1;
    trigger_sweep(9, 2, 4, 13, 20050, 21500, 25, "LETZTE Treppe (West 2->0), OHNE Eject");
    /* Kontroll-Sweep: die dritte Treppe (Ost, slot 7, Band 4->2, Kopf bei z=24200,
     * bergab -z). Zustaendige Zone = RVD[7] 2->3 z[23000..24000]. */
    g_mode_noeject = 0;
    trigger_sweep(7, 4, 2, 7, 24200, 25000, 25, "Treppe 3 (Ost 4->2), MIT Eject");
    /* Und die erste (Ost, slot 3, Band 8->6): Zone RVD[1] 0->1 z[21500..22500]. */
    trigger_sweep(3, 8, 0, 1, 24200, 25000, 25, "Treppe 1 (Ost 8->6), MIT Eject");
    /* VORSCHLAG: Eject geschwenkt statt teleportiert. */
    g_mode_noeject = 0; g_mode_sweep_eject = 1;
    trigger_sweep(9, 2, 4, 13, 20050, 20350, 20, "LETZTE Treppe, EJECT GESCHWENKT (Vorschlag)");
    trigger_sweep(7, 4, 2, 10, 24550, 24820, 20, "Treppe 3 Ost, EJECT GESCHWENKT (Vorschlag), Soll RVD[10]");
    g_mode_sweep_eject = 0;
    trigger_sweep(7, 4, 2, 10, 24550, 24820, 20, "Treppe 3 Ost, IST-Zustand, Soll RVD[10]");
    /* Kette mit dem FRUEHESTMOEGLICHEN byte-true Ausloesepunkt (Sonde beruehrt gerade
     * die Kante: gap=615 < 620). Das ist der natuerliche Fall "Spieler drueckt, sobald
     * das Spiel es zulaesst". */
    g_gap = 615; run_chain("E KETTE, frueheste Ausloesung (gap=615)", 0, 40, 0, 0, 0);
    /* Und mit dem SPAETESTMOEGLICHEN (an der Kollisionskante, gap=375 West / 350 Ost). */
    g_gap = 360; run_chain("F KETTE, spaeteste Ausloesung (gap=360)", 0, 40, 0, 0, 0);
    g_gap = STAIR_APPROACH_GAP;

    printf("\n--- KOLLISIONS-REICHWEITE (ist das Fehlerfenster ueberhaupt erreichbar?)\n");
    collision_reach(9, 2, "LETZTE Treppe West 2->0");
    collision_reach(7, 4, "Treppe 3 Ost 4->2");
    collision_reach(5, 6, "Treppe 2 West 6->4");
    collision_reach(3, 8, "Treppe 1 Ost 8->6");

    printf("\n--- FEHL-TRIGGER + WEG ZUR TUER (erholt sich die Kamera ohne Eject-Teleport?)\n");
    for (int32_t tz = 20100; tz <= 20250; tz += 50) {
        fail_then_walk(tz, 0);
        fail_then_walk(tz, 1);
    }
    return 0;
}
