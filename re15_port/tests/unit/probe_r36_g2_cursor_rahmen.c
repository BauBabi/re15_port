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
#include "re15_kistenregel_r37.h"

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
    /* Runde 37 Spur Q: ein Besitz-Bit der Kistenregel zaehlt nur mit dem Gegenstand im INVENTAR (kistenregel_r37.c,
     * RE2 Keep_Item_ck @0x8005850c). Im Original heisst "Bit gesetzt, noch nicht benutzt" = er liegt im Inventar (die
     * Item-Box ist eine Port-Zutat) -> denselben Gegenstand aus der Q-Tabelle dazulegen, sonst laeuft der Zweig ohne.
     * Betrifft ROOM1100 (3,0x67) Minidisc 0x44, ROOM11E0 (3,0x7F) Zange 0x30, ROOM3050 (3,0xB2) Master-Karte 0x46. */
    for (int i = 0; fl && i < 2 && fl[i].bank; i++)
        for (int k = 0; fl[i].val && k < re15_kistenregel_r37_anzahl(); k++) {
            const re15_kr37_fall_t *q = re15_kistenregel_r37_zeile(k);
            if ((unsigned)q->raum == rid && q->bank == fl[i].bank && q->bit == fl[i].bit) { re15_inv_grant(q->item, 1); break; }
        }
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

/* ================= SOLL = ORIGINAL ================================================================
 * Gemessen am Original (DuckStation + GDB, Auslieferungs-EXE @0x80026e4c `08 00 e0 03`, Debug-Menue-JUMP je Raum,
 * SCA im RAM == RDT bis auf u0/floor aus Sca_id_set/Sca_floor_set): analysis/befunde_runde36/G2_belege/orig_zensus/
 * log.txt (orig_cursor_zensus.py; dieselbe Nachbildung +-200 nach der SCD am Haltepunkt @0x8002bd44), ROOM11F0
 * zusaetzlich mit der echten SCD-Fahrt (orig_cursor_lauf1, identische Zahlen). Je Richtung: Bilder mit Bewegung,
 * Endlage, erste Ausschiebung (Lage nach +-200 -> nach dem Pass). Vier Rahmenformen:
 *   A  Rahmen x -31700..-7750 z 14150..31150, Start (-19554,22684): ROOM1080 (log Z.10), 1100 (3:103=1, Z.106),
 *      11E0 obj 1 (3:127=1, Z.135), 11F0 (Z.1), 2040 (3:146=1, Z.115), 2060 obj 3 (Z.48), 3050 obj 3 (3:178=1, Z.154),
 *      30E0 (Z.64), 4020 (Z.72), 5050 (Z.80)
 *   B  Rahmen x -31700..-7750 z -31650..-14650, Start (-19554,-23216): ROOM10D0 (Z.18), 1230 (Z.34); ROOM10D1 ohne
 *      eigene Messung (Elza-Variante, per Leon-JUMP nicht ladbar) — SCA-Kopf+Zellen und Cursor-Bytes @0x0114E
 *      byte-gleich mit 10D0 (G2_generator_ruhezweig.md Z3)
 *   C  ROOM11E0 obj 2, Rahmen x 7150..31100 z 14150..31150, Start (19446,22684) (3:139=0, Z.135)
 *   D  ROOM2060 obj 4, Rahmen x 7500..31450 z -31050..-14050, Start (19571,-22542) (Z.48)
 * ROOM2050 obj 8 fehlt: main00 @0x01D30-34 installiert ihn nur bei Ck(5,7)==1, Bank 5 ist beim Raumaufbau leer
 * (FUN_8003ecec `sw zero,0x800b1028` @0x8003ed74) — im Original nach dem JUMP gemessen leer (log Z.178: Pool 8
 * `+0=0000`), im Port ebenso. */
typedef struct { int bewegt; int32_t ex, ez, vx, vz, nx, nz; } soll_t;
static const soll_t SOLL_A[4] = {
    {  33, -19554,  29232, -19554,  29284, -19554,  29232 },
    {  66, -19554,  16068, -19554,  16032, -19554,  16068 },
    {  52, -29782,  16068, -29954,  16068, -29782,  16068 },
    { 101,  -9668,  16068,  -9582,  16068,  -9668,  16068 },
};
static const soll_t SOLL_B[4] = {
    {  34, -19554, -16568, -19554, -16416, -19554, -16568 },
    {  66, -19554, -29732, -19554, -29768, -19554, -29732 },
    {  52, -29782, -29732, -29954, -29732, -29782, -29732 },
    { 101,  -9668, -29732,  -9582, -29732,  -9668, -29732 },
};
static const soll_t SOLL_C[4] = {
    {  33,  19446,  29232,  19446,  29284,  19446,  29232 },
    {  66,  19446,  16068,  19446,  16032,  19446,  16068 },
    {  52,   9068,  16068,   9046,  16068,   9068,  16068 },
    { 101,  29182,  16068,  29268,  16068,  29182,  16068 },
};
static const soll_t SOLL_D[4] = {
    {  33,  19571, -15968,  19571, -15942,  19571, -15968 },
    {  66,  19571, -29132,  19571, -29168,  19571, -29132 },
    {  51,   9418, -29132,   9371, -29132,   9418, -29132 },
    { 101,  29532, -29132,  29618, -29132,  29532, -29132 },
};

static const soll_t *soll_fuer(unsigned rid, int obj)
{
    switch (rid) {
    case 0x1080: case 0x1100: case 0x11F0: case 0x2040: case 0x30E0: case 0x4020: case 0x5050: return SOLL_A;
    case 0x3050: return obj == 3 ? SOLL_A : NULL;
    case 0x11E0: return obj == 1 ? SOLL_A : (obj == 2 ? SOLL_C : NULL);
    case 0x2060: return obj == 3 ? SOLL_A : (obj == 4 ? SOLL_D : NULL);
    case 0x10D0: case 0x10D1: case 0x1230: return SOLL_B;
    default: return NULL;
    }
}

int main(int argc, char **argv)
{
    const int pin = (argc > 1 && strcmp(argv[1], "pin") == 0);
    printf("=== Runde 36 G2: Panel-Cursor gegen den Ruhe-Zweig (Ausschiebung @0x8003b79c) — %s ===\n",
           pin ? "RIEGEL" : "MESSUNG");
    for (int c = 0; c < N_CURSOR; c++) {
        const cursor_t *k = &CURSOR[c];
        if (!room_boot(k->stage, k->rid, k->fl)) { printf("ROOM%04X: RDT fehlt\n", k->rid); g_fail++; continue; }
        const int ci = prop_von_obj(k->obj_id);
        const soll_t *soll = soll_fuer(k->rid, k->obj_id);
        if (ci < 0) {
            printf("ROOM%04X obj %d: nicht installiert\n", k->rid, k->obj_id);
            if (pin) {
                char t[160];
                snprintf(t, sizeof t, "ROOM%04X obj %d: %s", k->rid, k->obj_id,
                         soll ? "Cursor fehlt, das Original installiert ihn" : "beim Raumaufbau nicht da (wie im Original)");
                CHECK(t, soll == NULL);
            }
            continue;
        }
        if (pin) {
            char t[200];
            snprintf(t, sizeof t, "ROOM%04X obj %d: Panel-Cursor +0=0x0101 (pc[6..7] `00 01` | 1 @0x80040990-9a4), "
                     "Typ 4, Band 0, h900", k->rid, k->obj_id);
            CHECK(t, g_scd.props[ci].flags == 0x0101u && g_scd.props[ci].obj_type == 4 && g_scd.props[ci].band == 0 &&
                     g_scd.props[ci].box_hx == 900 && g_scd.props[ci].box_hz == 900);
            CHECK("  und es gibt eine Original-Messung dafuer", soll != NULL);
        }
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
            if (pin && soll) {
                const soll_t *s = &soll[d];
                char t[220];
                snprintf(t, sizeof t, "ROOM%04X obj %d %s = Original: %d Bilder, Endlage (%d,%d), Ausschiebung (%d,%d)->(%d,%d)",
                         k->rid, k->obj_id, RNAME[d], s->bewegt, (int)s->ex, (int)s->ez, (int)s->vx, (int)s->vz,
                         (int)s->nx, (int)s->nz);
                CHECK(t, r.bewegt == s->bewegt && r.ex == s->ex && r.ez == s->ez && r.ausgeschoben &&
                         r.vor_x == s->vx && r.vor_z == s->vz && r.nach_x == s->nx && r.nach_z == s->nz);
            }
        }
    }
    printf("\n%s (%d Fehler)\n", g_fail ? "FEHLER" : "ALLES GRUEN", g_fail);
    return g_fail ? 1 : 0;
}
