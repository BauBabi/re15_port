/*
 * probe_r36_g2_cursor_rahmen.c — Runde 36 G2 "generator_ruhezweig": ZENSUS der Panel-Cursor aller Raeume
 * waehrend ihrer Fahrt. Schiebt der Ruhe-Zweig des Objekt-Passes (LAB_8002c0b8: FUN_8003b558(obj,2)
 * `jal 0x8003b558` @0x8002c0d0, Ausschiebung FUN_8003da78 `jal` @0x8003b79c; Port: engine/src/zellschub_r36.c)
 * den Cursor aus SCA-Zellen, und wo bleibt er stehen?
 *
 * Der Panel-Cursor ist in allen Raeumen dasselbe Objekt (statischer Zensus aller 206 RDTs,
 * analysis/befunde_runde36/G2_belege/zensus_objekte_ausschiebung.txt): Obj_model_set Typ 4, Band 0,
 * pc[6..7] = `00 01` -> obj+0 = 0x0101 (Installer LAB_80040914 @0x80040990-9a4, Bit 3 frei -> FUN_8003b558
 * @0x8003b668-674 prueft die Zellen), Box c(0,-900,0) h(900,900,900), Fahrt per Work_set(3,k) + Speed_set(+-200)
 * + Add_speed (Handler @0x80040f40: obj+0x34/+0x3C += Tempo) — z. B. ROOM11F0 sub02..05 @0x012F2..0x0131E.
 *
 * Die Fahrt wird hier fuer JEDEN Raum gleich nachgebildet: je Bild wird die Cursorlage NACH der SCD und VOR dem
 * Objekt-Pass um +-200 verschoben — genau die Stelle, an der Add_speed im Original wirkt (Hauptschleife: SCD
 * `jal 0x8003f038` @0x8001cdec, Objekt-Pass `jal 0x8002bd44` @0x8001ce14). Kein Pad: die Schubkette des Spielers
 * (@0x8002bf24 HOCH gehalten) greift nie, jedes Bild laeuft der Ruhe-Zweig (@0x8002bf94 -> 0x8002c0b8).
 * Gegenstueck am Original: analysis/befunde_runde36/G2_belege/orig_cursor_zensus.py (GDB, dieselbe Nachbildung
 * am Haltepunkt @0x8002bd44); ROOM11F0 zusaetzlich mit der ECHTEN SCD-Fahrt (orig_11f0_cursor.py).
 *
 * Ausgabe je Cursor und Richtung (OBEN = +z, UNTEN = -z, LINKS = -x, RECHTS = +x; Reihenfolge wie die
 * Original-Messung): Start, Bilder mit Bewegung, Endlage, erste Ausschiebung (Lage nach +-200 -> nach dem Pass).
 *   probe_r36_g2_cursor_rahmen          Messung (alle Raeume)
 *   probe_r36_g2_cursor_rahmen pin      Riegel: Endlagen/Bilanzen = Original (Tabelle SOLL unten)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_panel_zeiger.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                g_fail = 0;
static int                s_shown = 0;

#define CHECK(name, cond) do {                                          \
        int _c = (cond);                                                \
        printf("  [%s] %s\n", _c ? "OK  " : "FAIL", (name));            \
        if (!_c) g_fail++;                                              \
    } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Ein Spielbild wie die PC-Hauptschleife (VM vor dem Spielschritt). Die Verschiebung (dx, dz) wird nach der VM
 * und vor dem Spielschritt (darin der Objekt-Pass re15_prop_push_tick bzw. re15_zellschub_nach_pass) addiert. */
static void frame_fahrt(int ci, int32_t dx, int32_t dz)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    if (ci >= 0) { g_scd.props[ci].x += dx; g_scd.props[ci].z += dz; }
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

typedef struct { int bank, bit, val; } flag_t;

static int room_boot(const char *stage, unsigned rid, const flag_t *fl)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/%s/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, stage, rid);
    if (s_raw) { free(s_raw); s_raw = NULL; }
    s_raw = slurp(rp, &s_rawsz);
    if (!s_raw) return 0;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return 0;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = rid; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 250; pl->z = 250; pl->rot_y = 1024;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int i = 0; fl && i < 2 && fl[i].bank; i++)               /* nach scd_vm_init (leert die Baenke) */
        re15_game_flag_set((uint8_t)fl[i].bank, (uint8_t)fl[i].bit, (uint8_t)fl[i].val);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_panel_zeiger_reset();
    for (int f = 0; f < 30; f++) frame_fahrt(-1, 0, 0);
    return 1;
}

static int prop_von_obj(int obj_id)
{
    for (int i = 0; i < (int)g_scd.prop_count && i < RE15_SCD_MAX_PROPS; i++)
        if (g_scd.props[i].active && (int)g_scd.props[i].obj_id == obj_id) return i;
    return -1;
}

typedef struct { int bewegt; int32_t ex, ez; int32_t vor_x, vor_z, nach_x, nach_z; int ausgeschoben; } phase_t;

/* Richtung halten, bis der Cursor nach dem Anlaufen 15 Bilder steht (wie orig_11f0_cursor.py: steh_ende 15). */
static phase_t fahre(int ci, int32_t dx, int32_t dz)
{
    phase_t r; memset(&r, 0, sizeof r);
    int steh = 0;
    for (int f = 0; f < 400 && steh < 15; f++) {
        const int32_t ax = g_scd.props[ci].x, az = g_scd.props[ci].z;
        frame_fahrt(ci, dx, dz);
        const int32_t nx = g_scd.props[ci].x, nz = g_scd.props[ci].z;
        if (!r.ausgeschoben && (nx != ax + dx || nz != az + dz)) {
            r.ausgeschoben = 1; r.vor_x = ax + dx; r.vor_z = az + dz; r.nach_x = nx; r.nach_z = nz;
        }
        if (nx != ax || nz != az) { r.bewegt++; steh = 0; } else steh++;
    }
    r.ex = g_scd.props[ci].x; r.ez = g_scd.props[ci].z;
    for (int f = 0; f < 5; f++) frame_fahrt(ci, 0, 0);   /* Taste los (Original: 1 s ohne Taste) */
    return r;
}

typedef struct { const char *stage; unsigned rid; int obj_id; flag_t fl[2]; } cursor_t;

/* Alle Panel-Cursor (statischer Zensus): Raum, obj-Nummer (= Pool-Platz, Installer @0x8004093c-958 pc[1]).
 * ROOM1081/1101/11E1/11F1/1231 sind byte-gleich mit ...0 (cmp), ROOM10D1 nicht (eigene Zeile).
 * Spielflags: Cursor, die sub00 nur im Else-Zweig installiert — ROOM1100 sub00 @0x00AC8 Ck(3,103)==0 -> Else
 * @0x00AFE; ROOM11E0 sub00 @0x01798 Ck(3,127)==0 -> Else @0x017C4; ROOM2040 sub00 @0x0141A Ck(3,146)==0 -> Else
 * @0x0144A; ROOM3050 main00 @0x01F90 Ck(3,178)==0 -> Else @0x01FA8. ROOM2050 obj 8 haengt an Ck(5,7)==1
 * @0x01D34 (Bank 5 = raumlokal, FUN_8003ecec `sw zero,0x800b1028` @0x8003ed74) -> beim Raumaufbau nie da. */
static const cursor_t CURSOR[] = {
    { "STAGE1", 0x1080, 0, {{0}} }, { "STAGE1", 0x10D0, 1, {{0}} }, { "STAGE1", 0x10D1, 1, {{0}} },
    { "STAGE1", 0x1100, 0, {{3, 103, 1}} }, { "STAGE1", 0x11E0, 1, {{3, 127, 1}} }, { "STAGE1", 0x11E0, 2, {{0}} },
    { "STAGE1", 0x11F0, 0, {{0}} }, { "STAGE1", 0x1230, 0, {{0}} }, { "STAGE2", 0x2040, 0, {{3, 146, 1}} },
    { "STAGE2", 0x2050, 8, {{0}} }, { "STAGE2", 0x2060, 3, {{0}} }, { "STAGE2", 0x2060, 4, {{0}} },
    { "STAGE3", 0x3050, 3, {{3, 178, 1}} }, { "STAGE3", 0x30E0, 0, {{0}} }, { "STAGE4", 0x4020, 0, {{0}} },
    { "STAGE5", 0x5050, 0, {{0}} },
};
#define N_CURSOR ((int)(sizeof CURSOR / sizeof CURSOR[0]))

static const char *RNAME[4] = { "OBEN", "UNTEN", "LINKS", "RECHTS" };
static const int32_t RDX[4] = { 0, 0, -200, 200 };
static const int32_t RDZ[4] = { 200, -200, 0, 0 };

int main(int argc, char **argv)
{
    const int pin = (argc > 1 && strcmp(argv[1], "pin") == 0);
    printf("=== Runde 36 G2: Panel-Cursor gegen den Ruhe-Zweig (Ausschiebung @0x8003b79c) — %s ===\n",
           pin ? "RIEGEL" : "MESSUNG");
    for (int c = 0; c < N_CURSOR; c++) {
        const cursor_t *k = &CURSOR[c];
        if (!room_boot(k->stage, k->rid, k->fl)) { printf("ROOM%04X: RDT fehlt\n", k->rid); g_fail++; continue; }
        const int ci = prop_von_obj(k->obj_id);
        if (ci < 0) { printf("ROOM%04X obj %d: nicht installiert\n", k->rid, k->obj_id); continue; }
        printf("ROOM%04X obj %d (Platz %d) +0=%04x typ %d band %d box c(%d,%d,%d) h(%d,%d,%d) start (%d,%d)\n",
               k->rid, k->obj_id, ci, g_scd.props[ci].flags, g_scd.props[ci].obj_type, g_scd.props[ci].band,
               g_scd.props[ci].box_cx, g_scd.props[ci].box_cy, g_scd.props[ci].box_cz,
               g_scd.props[ci].box_hx, g_scd.props[ci].box_hy, g_scd.props[ci].box_hz,
               g_scd.props[ci].x, g_scd.props[ci].z);
        for (int d = 0; d < 4; d++) {
            const int32_t sx = g_scd.props[ci].x, sz = g_scd.props[ci].z;
            phase_t r = fahre(ci, RDX[d], RDZ[d]);
            printf("  %-6s start (%d,%d) bewegt %3d ende (%d,%d)", RNAME[d], sx, sz, r.bewegt, r.ex, r.ez);
            if (r.ausgeschoben) printf("  erste Ausschiebung (%d,%d) -> (%d,%d)", r.vor_x, r.vor_z, r.nach_x, r.nach_z);
            else printf("  KEINE Ausschiebung");
            printf("\n");
        }
    }
    printf("\n%s (%d Fehler)\n", g_fail ? "FEHLER" : "ALLES GRUEN", g_fail);
    return g_fail ? 1 : 0;
}
