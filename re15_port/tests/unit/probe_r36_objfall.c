/*
 * probe_r36_objfall — Runde 36 Spur A, Nachbesserung 3 (Abnahme 2 Mangel 2, Orchestrator: "byte-true portieren"):
 * der OBJEKT-FALL der Typ-4-Objekte (engine/src/objfall_r36.c = Objekt-Pass FUN_8002bd44 @0x8002bde0-be08 /
 * @0x8002c0fc-c154 + FUN_8002c444 + FUN_8002dacc + Wasser-Stempel LAB_8004330c) in der echten VM, je Raum.
 *
 * Gemessen wird die MECHANIK, nicht ein Bild:
 *   Ausloeser  FUN_8003b7f0(&obj+0x34, 0x1c2, obj+0x82) & 2 (@0x8002c120-134): Grenze exakt an der um 450
 *              erweiterten Absprungzelle (eine Einheit davor kein Fall); Richtung vom Spieler weg (@0x8002d410-450).
 *   Fall       Vorschub 13, 26, ... (Init 0xd @0x8002c4a0-a8, +0xd je Bild @0x8002c5f4-614), Absinken (67c/3) bis
 *              Bild 12 (@0x8002c92c-968), Bild 13 Band = -(FUN_8001c6e8 / 1800) (@0x8002c6c4), Landung auf der
 *              Bodenhoehe (@0x8002c904-928), Aufsetzen/Nachfedern (@0x8002c96c/@0x8002ca10), obj+4 = 0 (@0x8002ca88),
 *              Kippung um 90 Grad (Winkelschritte 8, 16, ... bis 1024 @0x8002c548-5dc, FUN_8002dacc @0x8002ca8c).
 *   Toene      snd0 Satz 6 beim Aufsetzen ohne Wasser (@0x8002c96c-97c), snd0 Satz 8 + 12 Spritzer beim Wasser
 *              (@0x8002c6c8-8fc); der Wasserspiegel kommt aus dem AUTO-Stempel der sce-8-Zone (LAB_8004330c).
 *   Lage-Latch der Raum-SCD (Kerbe obj+0x0B -> Member_cmp 15 -> Set): 5020 @0x012F4-302 (4,203), 20A0 @0x01AD4-e2
 *              (3,204), 3050 @0x02188-196 (3,93).
 *   Klettern   auf die GEFALLENE Kiste (Substate 9 / KIND 0x81 / oben, Standobjekt = Kiste).
 *   Gegenprobe Raeume ohne Absprungzelle im Objekt-Band (10F0/10F1-Tritt): ueber das ganze Raster kein Ausloeser.
 *
 * Aufruf: probe_r36_objfall diag | pin
 * Dossier: analysis/befunde_runde36/A_klettern.md, Abschnitt "Nachbesserung 3".
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
#include "re15_esp.h"
#include "re15_objfall_r36.h"

extern scd_vm_t g_scd;
extern int g_test_snd0_se_last, g_test_snd0_se_count;

static int g_fail = 0;
#define PRUEF(cond, ...) do { if (!(cond)) { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); g_fail++; } } while (0)

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
static re15_esp_t    s_esp;
static re15_actor_t *s_pl;

/* RDT laden + Raum aufbauen (wie probe_r36_klettern_zensus.c reset_room, Lage-Flag VOR dem Raum-Init =
 * Spielstand); dazu die Raum-ESP binden (FUN_80019354-Weg wie platform/pc/main.c pc_load_room_esp), damit
 * die Spritzer (Effekt 6) aufloesbar sind. */
static int raum(const char *stage, const char *room, uint16_t room_id, int bank, int bit, int val)
{
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/%s/%s.RDT", RE15_ASSET_PSX_DIR, stage, room);
    uint8_t *raw = read_file(path, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", path); return 77; }
    if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("SKIP: RDT-Parse %s\n", room); return 77; }
#define U32LE(o) ((uint32_t)raw[o] | ((uint32_t)raw[(o)+1] << 8) | ((uint32_t)raw[(o)+2] << 16) | ((uint32_t)raw[(o)+3] << 24))
    re15_esp_set_room_bank(NULL);
    if (re15_esp_parse(raw, sz, U32LE(0x4C), U32LE(0x50), U32LE(0x54), U32LE(0x58), &s_esp) == 0)
        re15_esp_set_room_bank(&s_esp);
#undef U32LE
    re15_actor_init();
    scd_vm_init();
    re15_climb_reset();
    re15_esp_fx_reset();
    if (bank >= 0) re15_game_flag_set((uint8_t)bank, (uint8_t)bit, val);
    g_current_room_id = room_id;
    g_room_change.pending = 0;
    s_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_pl->active = 1; s_pl->type = 0; s_pl->hp = 100;
    s_pl->x = 0; s_pl->y = 0; s_pl->z = 0; s_pl->rot_y = 0;
    scd_room_reenter(&s_rdt, s_pl->x, s_pl->z, 0);
    re15_collision_set_band(0);
    return 0;
}

static int typ4_prop(void)
{
    for (int k = 0; k < (int)g_scd.prop_count && k < RE15_SCD_MAX_PROPS; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_type == 4) return k;
    return -1;
}

/* Ein Objekt-Pass-Bild: Handler (Anfang) + Ausloeser/0x88/Wasser-Stempel (Ende). */
static void bild(void)
{
    re15_objfall_vor_schub(&s_rdt);
    re15_objfall_nach_pass(&s_rdt);
}
/* Bild mit AUTO-Kerbe (Glied re15_object_notch_update) und VM-Lauf (sub01-Latch). */
static void bild_voll(void)
{
    bild();
    re15_object_notch_update();
    scd_vm_tick();
}

typedef struct {
    int   bilder;            /* Handler-Bilder bis obj+4 = 0 */
    int   landebild;         /* Zaehler obj+6 beim Zustand 1 -> 2 */
    int   band13;            /* Band nach Bild 13 */
    int   y_lande;           /* y nach der Landung (vor Aufsetzen) */
    int   se6, se8;          /* Anzahl snd0-Saetze 6 / 8 waehrend des Falls */
    int   spritzer_plaetze;  /* aktive ESP-Plaetze nach dem Fall (Spritzer) */
    int   dx1, dx2, dy1, dy2;/* Vorschub/Absinken der ersten zwei Bilder */
    int   d690_1, d690_2;    /* Kippwinkel nach Bild 1/2 */
} fall_t;

/* Fall von der aktuellen Lage aus fahren: erst EIN Bild Ausloeser (nach_pass), dann Bild fuer Bild bis obj+4 = 0. */
static int fall_fahren(int p, fall_t *o, const char *tag)
{
    memset(o, 0, sizeof *o);
    o->landebild = -1; o->band13 = -1;
    const unsigned land0 = re15_objfall_n_landungen();
    const unsigned spr0 = re15_objfall_n_spritzer();
    re15_objfall_nach_pass(&s_rdt);                       /* Ende eines Objekt-Passes: Ausloeser */
    re15_objfall_info_t fi;
    re15_objfall_info(p, &fi);
    if (!fi.fall) { printf("    %s: kein Ausloeser\n", tag); return 0; }
    printf("    %s: AUSLOESER pos=(%d,%d,%d) band=%d richt=(%d,%d)\n", tag, g_scd.props[p].x, g_scd.props[p].y,
           g_scd.props[p].z, (int)g_scd.props[p].band, (int)fi.richt_x, (int)fi.richt_z);
    const int se_n0 = g_test_snd0_se_count;
    int se6 = 0, se8 = 0;
    int32_t px = g_scd.props[p].x, py = g_scd.props[p].y, pz = g_scd.props[p].z;
    for (int f = 1; f <= 200; f++) {
        const int n_vor = g_test_snd0_se_count;
        bild();
        if (g_test_snd0_se_count != n_vor) { if (g_test_snd0_se_last == 6) se6++; if (g_test_snd0_se_last == 8) se8++; }
        re15_objfall_info(p, &fi);
        const int32_t dx = (g_scd.props[p].x - px) + (g_scd.props[p].z - pz), dy = g_scd.props[p].y - py;
        if (f == 1) { o->dx1 = dx; o->dy1 = dy; o->d690_1 = fi.winkel; }
        if (f == 2) { o->dx2 = dx; o->dy2 = dy; o->d690_2 = fi.winkel; }
        if (fi.zaehler == 13 && o->band13 < 0) o->band13 = g_scd.props[p].band;
        if (o->landebild < 0 && re15_objfall_n_landungen() != land0) { o->landebild = fi.zaehler; o->y_lande = g_scd.props[p].y; }
        px = g_scd.props[p].x; py = g_scd.props[p].y; pz = g_scd.props[p].z;
        if (!fi.fall) { o->bilder = f; break; }
    }
    (void)se_n0;
    o->se6 = se6; o->se8 = se8;
    o->spritzer_plaetze = (int)(re15_objfall_n_spritzer() - spr0) ? re15_esp_fx_count() : 0;
    printf("    %s: ENDE nach %d Bildern, Landung bei Bild %d y=%d, Band nach Bild 13 = %d, Endlage (%d,%d,%d) Band %d "
           "rot=(%d,%d,%d), Bild1 d=%d dy=%d w=%d, Bild2 d=%d dy=%d w=%d, snd0:6 x%d, snd0:8 x%d, Spritzer-Plaetze %d\n",
           tag, o->bilder, o->landebild, o->y_lande, o->band13, g_scd.props[p].x, g_scd.props[p].y, g_scd.props[p].z,
           (int)g_scd.props[p].band, (int)g_scd.props[p].rot_x, (int)g_scd.props[p].rot_y, (int)g_scd.props[p].rot_z,
           o->dx1, o->dy1, o->d690_1, o->dx2, o->dy2, o->d690_2, se6, se8, o->spritzer_plaetze);
    return o->bilder > 0;
}

/* Klettern auf das Objekt p: Spieler auf Band b vor die Box (Blick yaw), Aktionstaste, Aufstieg. */
static int klettern(int p, int yaw, int *y_end, int *band_end, int *stand_end)
{
    const int32_t cx = g_scd.props[p].x + g_scd.props[p].box_cx, cz = g_scd.props[p].z + g_scd.props[p].box_cz;
    const int32_t hx = g_scd.props[p].box_hx, hz = g_scd.props[p].box_hz;
    switch (yaw) {
    case 0:    s_pl->x = cx - hx - 450; s_pl->z = cz;            break;
    case 1024: s_pl->x = cx;            s_pl->z = cz + hz + 450; break;
    case 2048: s_pl->x = cx + hx + 450; s_pl->z = cz;            break;
    default:   s_pl->x = cx;            s_pl->z = cz - hz - 450; break;
    }
    s_pl->y = g_scd.props[p].y; s_pl->rot_y = (int16_t)yaw;
    re15_collision_set_band((int)g_scd.props[p].band);
    re15_climb_reset();
    re15_climb_standing_tick();
    const int r = re15_climb_try_start(&s_rdt, 1);
    const int ok_start = r && re15_climb_dbg_sub() == 9 && re15_climb_dbg_kind() == 0x81 && re15_climb_dbg_obj() == p;
    for (int f = 0; f < 300 && re15_climb_active(); f++) {
        re15_climb_tick(&s_rdt, NULL, &s_pl00, &s_w01);
        re15_climb_standing_tick();
    }
    *y_end = s_pl->y; *band_end = re15_collision_debug_band(); *stand_end = re15_climb_dbg_standing();
    re15_climb_reset();
    return ok_start;
}

/* ---- ROOM5020/5021: Band 4 -> Boden (Band 0), Latch (4,203) --------------------------------------------- */
static void raum_5020(const char *room, uint16_t id)
{
    printf("--- %s (Kiste Band 4, Absprungzelle x -7675..-7175 z -26275..-24025 Wort 0x4002)\n", room);
    if (raum("STAGE5", room, id, 4, 203, 0) != 0) { g_fail++; return; }
    const int p = typ4_prop();
    PRUEF(p >= 0 && g_scd.props[p].band == 4 && g_scd.props[p].x == -11000 && g_scd.props[p].y == -7200 &&
          g_scd.props[p].z == -23150, "%s: Startlage != Record (Band 4 (-11000,-7200,-23150))", room);
    if (p < 0) return;
    /* Ruhe: 30 Bilder in der Startlage, Spieler daneben -> kein Fall (Zelle 35 = Wand 0x4300, kein Bit 2). */
    s_pl->x = -12350; s_pl->z = -23150; re15_collision_set_band(4);
    for (int f = 0; f < 30; f++) bild();
    re15_objfall_info_t fi; re15_objfall_info(p, &fi);
    PRUEF(!fi.fall && g_scd.props[p].x == -11000 && g_scd.props[p].band == 4, "%s: Startlage faellt", room);
    /* Grenze: Zelle x0 -7675 - 450 = -8125 (FUN_8003b7f0 `sltu` @0x8003b8c8-d0). x -8126 -> kein Fall. */
    g_scd.props[p].x = -8126; g_scd.props[p].z = -25332;
    s_pl->x = -9500; s_pl->z = -25800;
    for (int f = 0; f < 3; f++) bild();
    re15_objfall_info(p, &fi);
    PRUEF(!fi.fall && g_scd.props[p].x == -8126, "%s: x -8126 (eine Einheit vor der erweiterten Zelle) faellt", room);
    printf("    Grenze x -8126: kein Ausloeser (fall=%d)\n", fi.fall);
    g_scd.props[p].x = -8125;
    fall_t o;
    const int se0 = g_test_snd0_se_count;
    const int ok = fall_fahren(p, &o, room);
    re15_objfall_info(p, &fi);
    PRUEF(ok, "%s: kein Fall bei x -8125", room);
    PRUEF(fi.richt_x == 1 && fi.richt_z == 0, "%s: Richtung (%d,%d) != (+1,0) (Spieler westlich, |dx| >= 900)", room,
          fi.richt_x, fi.richt_z);
    PRUEF(o.dx1 == 13 && o.dy1 == 8 && o.dx2 == 26 && o.dy2 == 13,
          "%s: Bild 1/2 Vorschub %d/%d Absinken %d/%d != 13/26 und 8/13 (0xd @0x8002c4a0-a8, 67c/3)", room,
          o.dx1, o.dx2, o.dy1, o.dy2);
    PRUEF(o.d690_1 == 0 && o.d690_2 == 8, "%s: Kippwinkel Bild 1/2 = %d/%d != 0/8 (684 += 8 @0x8002c5e0)", room,
          o.d690_1, o.d690_2);
    PRUEF(o.band13 == 0, "%s: Band nach Bild 13 = %d != 0 (FUN_8001c6e8 = 0)", room, o.band13);
    PRUEF(o.landebild == 22 && o.y_lande == 0, "%s: Landung Bild %d y %d != 22 / 0", room, o.landebild, o.y_lande);
    PRUEF(o.bilder == 24, "%s: Fall dauert %d Bilder != 24 (22 + Aufsetzen + Nachfedern)", room, o.bilder);
    PRUEF(o.se6 == 1 && o.se8 == 0, "%s: snd0:6 x%d snd0:8 x%d != 1/0", room, o.se6, o.se8);
    PRUEF(g_scd.props[p].band == 0 && g_scd.props[p].y == 0, "%s: Endlage Band %d y %d", room,
          (int)g_scd.props[p].band, g_scd.props[p].y);
    PRUEF(g_scd.props[p].rot_x == 0 && g_scd.props[p].rot_y == 0 &&
          g_scd.props[p].rot_z >= 1016 && g_scd.props[p].rot_z <= 1032,
          "%s: Endrotation (%d,%d,%d) != (0,0,1024 +- 8)", room, (int)g_scd.props[p].rot_x,
          (int)g_scd.props[p].rot_y, (int)g_scd.props[p].rot_z);
    (void)se0;
    /* Latch: Kiste auf Band 0 in Slot 4 (Aot_set @0x0122E, Floor 0, Rect (-4450,-26250,2050,2050)), wie am echten
     * Weg gemessen (F1712 x -4393) -> Kerbe 4 -> sub01 @0x012F4-302 setzt (4,203). */
    g_scd.props[p].x = -4393;
    for (int f = 0; f < 3; f++) bild_voll();
    const int flag = re15_game_flag_get(4, 203);
    printf("    Latch: Kerbe obj+0x0B = %d, Flag (4,203) = %d\n", (int)g_scd.props[p].member_0b, flag);
    PRUEF(g_scd.props[p].member_0b == 4 && flag == 1, "%s: Latch (4,203) nicht gesetzt (Kerbe %d, Flag %d)", room,
          (int)g_scd.props[p].member_0b, flag);
    /* Klettern auf die gefallene Kiste, Spieler auf Band 0 westlich, Blick +X. */
    int ye = 0, be = 0, st = -1;
    const int ks = klettern(p, 0, &ye, &be, &st);
    printf("    Klettern auf die gefallene Kiste: Start %d, oben y=%d Band %d Stand %d\n", ks, ye, be, st);
    PRUEF(ks && ye == -1800 && be == 1 && st == p, "%s: Klettern auf die gefallene Kiste (Start %d y %d Band %d Stand %d)",
          room, ks, ye, be, st);
}

/* ---- ROOM20A0/20A1: Tasche -> Laufsteg (Band 2) -> Kanal (Band 1, Wasser) -> Kante -> Band 0 (Spritzer) ---
 * MECHANIK-TEST mit GESETZTER Lage: durch Schieben ist die Tasche UNERREICHBAR (Freiraum-Zensus unten: Wand 37
 * sperrt ab Kistenmitte x >= 16700, Wand 38 bis x < 16700 — FUN_8003b558 `sltu` @0x8003b764, untere Grenze inklusiv,
 * obere exklusiv; die 1800 breite Tasche laesst keine einzige Mitte frei). Geprueft wird hier nur, dass der Fall
 * die Kette Laufsteg / Kanal / Wasser / Spritzer so faehrt, wie FUN_8002c444 sie vorgibt. */
static void raum_20a0(const char *room, uint16_t id)
{
    printf("--- %s (Kiste Band 4, Tasche x 15800..17600 vor Absprungzelle 45 z -22000..-21000 Wort 0x4206)\n", room);
    if (raum("STAGE2", room, id, 3, 204, 0) != 0) { g_fail++; return; }
    const int p = typ4_prop();
    PRUEF(p >= 0 && g_scd.props[p].band == 4 && g_scd.props[p].x == 12250 && g_scd.props[p].z == -12400,
          "%s: Startlage != Record (Band 4 (12250,-7200,-12400))", room);
    if (p < 0) return;
    re15_objfall_info_t fi;
    /* (1) Tasche: Kistenmitte x 16700 (= 15800 + 900 = 17600 - 900), Spieler noerdlich (z groesser). Obere
     *     z-Grenze der erweiterten Zelle 45 ist EXKLUSIV: (u32)(z - (z0 - 450)) < d + 900 (`sltu` @0x8003b8f4)
     *     -> z < -22000 + 1000 + 450 = -20550. */
    g_scd.props[p].x = 16700; g_scd.props[p].z = -20550;
    s_pl->x = 16700; s_pl->z = -18500; re15_collision_set_band(4);
    for (int f = 0; f < 3; f++) bild();
    re15_objfall_info(p, &fi);
    PRUEF(!fi.fall, "%s: z -20550 (Grenze der erweiterten Zelle 45, exklusiv) faellt", room);
    g_scd.props[p].z = -20551;
    fall_t o1;
    const int ok1 = fall_fahren(p, &o1, "Fall 1 (Tasche)");
    re15_objfall_info(p, &fi);
    PRUEF(ok1 && fi.richt_x == 0 && fi.richt_z == -1, "%s: Fall 1 Richtung (%d,%d) != (0,-1)", room, fi.richt_x, fi.richt_z);
    PRUEF(ok1 && o1.band13 == 2 && o1.y_lande == -3600 && o1.se6 == 1 && o1.se8 == 0,
          "%s: Fall 1 Band13 %d y %d snd0:6 x%d snd0:8 x%d != 2 / -3600 / 1 / 0 (Zelle 12 Band 1 u0 255 -> Boden -3600)",
          room, o1.band13, o1.y_lande, o1.se6, o1.se8);
    /* (2) Laufsteg-Kante: Zelle 28 (Band 2, x -3600..20550, z -26900..-23400, 0x2102) -> Kanal. Kistenmitte
     *     z < -22950 (= -23400 + 450, exklusiv). Die Wasserzone Slot 5 (Floor 1, Spiegel -540) stempelt erst
     *     nach Bild 13. */
    g_scd.props[p].z = -22950;
    s_pl->x = 16700; s_pl->z = -21000; re15_collision_set_band(2);
    for (int f = 0; f < 3; f++) bild();
    re15_objfall_info(p, &fi);
    PRUEF(!fi.fall, "%s: z -22950 (Grenze, exklusiv) faellt", room);
    g_scd.props[p].z = -22951;
    fall_t o2;
    const int ok2 = fall_fahren(p, &o2, "Fall 2 (Laufsteg -> Kanal)");
    re15_objfall_info(p, &fi);
    printf("    nach Fall 2: obj+0x88 = %d\n", (int)fi.wasser);
    PRUEF(ok2 && o2.band13 == 1 && o2.y_lande == -1800 && o2.se6 == 0 && o2.se8 == 0,
          "%s: Fall 2 Band13 %d y %d snd0:6 x%d snd0:8 x%d != 1 / -1800 / 0 (Wasser gestempelt) / 0 (Spiegel -540 nicht "
          "unter der Kiste)", room, o2.band13, o2.y_lande, o2.se6, o2.se8);
    /* (3) Kanal-Kante: Zelle 15 (Band 1, x 20550..21550, z -26900..-23400, 0x1002) -> Boden. Kistenmitte x 20100
     *     (= 20550 - 450), Spieler westlich. Danach Band 0 in Wasserzone Slot 6 (Floor 0, Spiegel -540). */
    g_scd.props[p].x = 20099; g_scd.props[p].z = -25000;
    s_pl->x = 18500; s_pl->z = -25000; re15_collision_set_band(1);
    for (int f = 0; f < 3; f++) bild();
    re15_objfall_info(p, &fi);
    PRUEF(!fi.fall, "%s: x 20099 faellt", room);
    g_scd.props[p].x = 20100;
    re15_esp_fx_reset();
    fall_t o3;
    const int ok3 = fall_fahren(p, &o3, "Fall 3 (Kanal-Kante -> Boden, Wasser)");
    PRUEF(ok3 && o3.band13 == 0 && o3.y_lande == 0 && o3.se6 == 0 && o3.se8 == 1 && o3.spritzer_plaetze > 0,
          "%s: Fall 3 Band13 %d y %d snd0:6 x%d snd0:8 x%d Spritzer %d != 0 / 0 / 0 / 1 / >0", room, o3.band13,
          o3.y_lande, o3.se6, o3.se8, o3.spritzer_plaetze);
    /* (4) Latch: Slot 7 (Floor 0, Rect (22050,-26900,2050,5900)) -> Kerbe 7 -> (3,204) (@0x01AD4-e2). */
    g_scd.props[p].x = 23150; g_scd.props[p].z = -24400;
    for (int f = 0; f < 3; f++) bild_voll();
    const int flag = re15_game_flag_get(3, 204);
    printf("    Latch: Kerbe obj+0x0B = %d, Flag (3,204) = %d\n", (int)g_scd.props[p].member_0b, flag);
    PRUEF(g_scd.props[p].member_0b == 7 && flag == 1, "%s: Latch (3,204) nicht gesetzt (Kerbe %d, Flag %d)", room,
          (int)g_scd.props[p].member_0b, flag);
    int ye = 0, be = 0, st = -1;
    const int ks = klettern(p, 0, &ye, &be, &st);   /* westlich, Blick +X (oestlich liegt Wand Zelle 1 x 24100..24600) */
    printf("    Klettern auf die gefallene Kiste: Start %d, oben y=%d Band %d Stand %d\n", ks, ye, be, st);
    PRUEF(ks && ye == -1800 && be == 1 && st == p, "%s: Klettern auf die gefallene Kiste (Start %d y %d Band %d Stand %d)",
          room, ks, ye, be, st);
}

/* ---- FREIRAUM-ZENSUS: kann eine GESCHOBENE Kiste den Ausloeser ueberhaupt erreichen? ---------------------
 * Notwendige Bedingung fuer einen Fall durch Schieben: von der Startlage aus fuehrt ein zusammenhaengender Weg
 * UNGESPERRTER Kistenmitten zu einer Lage, an der der Ausloeser greift. Praedikate:
 *   Ausloeser  FUN_8003b7f0(&obj+0x34, 0x1c2, Band) & 2 (@0x8002c120-134; ERSTE Zelle des Bandes im Quadranten,
 *              Rechteck um 450 erweitert, halboffen [x0-450, x0+w+450) — `sltu` @0x8003b8c8-d0 / @0x8003b8f0-f4).
 *   gesperrt   FUN_8003b558(obj, 2) != 0 (@0x8002bfb0; Zellen des Objekt-Bands mit u0 & 2, Rechteck um den
 *              Kisten-Halbmesser r = box.hx erweitert [x0-r, x0+w+r) — `sltu` @0x8003b764 / @0x8003b790; hx == hz ->
 *              r = hx @0x8003b68c-690). Eine gesperrte Lage nimmt der Schub sofort zurueck (@0x8002c0b8-d4), die
 *              Kiste bewegt sich also nur durch ungesperrte Lagen.
 * Beide sind auf halboffenen achsparallelen Rechtecken stueckweise konstant -> EXAKT ueber Koordinaten-Kompression
 * (alle Zellgrenzen +-450 und +-r, Quadranten-Grenze ceil_x/ceil_z), Flutfuellung ueber ungesperrte Elementarzellen
 * ab der Startlage (4er-Nachbarschaft = gemeinsame Kante). Eine erreichte Ausloeser-Zelle beendet den Weg (die Kiste
 * faellt dort). Ergebnis = erreichbare Ausloeser-Flaeche (Einheiten^2). Spieler-Kollision bleibt aussen vor (nur die
 * notwendige Bedingung der Kiste). */
static int cmp_i32(const void *a, const void *b)
{
    const int32_t x = *(const int32_t *)a, y = *(const int32_t *)b;
    return (x > y) - (x < y);
}
static int uniq(int32_t *v, int n)
{
    qsort(v, (size_t)n, sizeof v[0], cmp_i32);
    int m = 0;
    for (int i = 0; i < n; i++) if (m == 0 || v[i] != v[m - 1]) v[m++] = v[i];
    return m;
}
static int idx_of(const int32_t *v, int n, int32_t x)
{
    int lo = 0, hi = n - 2;
    if (x < v[0] || x >= v[n - 1]) return -1;
    while (lo < hi) { int mid = (lo + hi + 1) / 2; if (v[mid] <= x) lo = mid; else hi = mid - 1; }
    return lo;
}
static long long freiraum(const char *stage, const char *room, uint16_t id, int obj_id, int bank, int bit,
                          int32_t *bx, int32_t *bz)
{
    if (raum(stage, room, id, bank, bit, 0) != 0) { g_fail++; return -1; }
    int p = -1;
    for (int k = 0; k < (int)g_scd.prop_count && k < RE15_SCD_MAX_PROPS; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_type == 4 && g_scd.props[k].obj_id == obj_id) p = k;
    if (p < 0) { printf("--- FREIRAUM %s: obj %d nicht installiert (Lage-Flag 0)\n", room, obj_id); return -2; }
    const int band = (int)g_scd.props[p].band;
    const int32_t r = (int32_t)(uint16_t)g_scd.props[p].box_hx;
    const int32_t sx = g_scd.props[p].x + g_scd.props[p].box_cx, sz = g_scd.props[p].z + g_scd.props[p].box_cz;
    static int32_t xs[8192], zs[8192];
    int nx = 0, nz = 0;
    xs[nx++] = (int16_t)s_rdt.ceiling_x; zs[nz++] = (int16_t)s_rdt.ceiling_z;
    xs[nx++] = -40000; xs[nx++] = 40000; zs[nz++] = -40000; zs[nz++] = 40000;
    for (int i = 0; i < s_rdt.sca_count && nx < 8186; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        const int32_t x0 = e->x, z0 = e->z, w = e->width, d = e->density;
        xs[nx++] = x0 - 450; xs[nx++] = x0 + w + 450; xs[nx++] = x0 - r; xs[nx++] = x0 + w + r;
        zs[nz++] = z0 - 450; zs[nz++] = z0 + d + 450; zs[nz++] = z0 - r; zs[nz++] = z0 + d + r;
    }
    nx = uniq(xs, nx); nz = uniq(zs, nz);
    const int W = nx - 1, H = nz - 1;
    uint8_t *st = (uint8_t *)calloc((size_t)W * (size_t)H, 1);      /* 0 neu, 1 gesperrt, 2 frei, 3 besucht */
    int *q = (int *)malloc(sizeof(int) * (size_t)W * (size_t)H);
    long long erreicht = 0; int n_aus = 0, n_besucht = 0;
    *bx = 0; *bz = 0;
    const int i0 = idx_of(xs, nx, sx), j0 = idx_of(zs, nz, sz);
    int qh = 0, qt = 0;
    if (i0 >= 0 && j0 >= 0 && !re15_collision_box_blocked(&s_rdt, xs[i0], zs[j0], band, r, 2u)) {
        st[(size_t)j0 * W + i0] = 3; q[qt++] = j0 * W + i0;
    }
    while (qh < qt) {
        const int c = q[qh++], i = c % W, j = c / W;
        n_besucht++;
        if (re15_collision_floor_typeword(&s_rdt, xs[i], zs[j], band, 450) & 2) {
            erreicht += (long long)(xs[i + 1] - xs[i]) * (long long)(zs[j + 1] - zs[j]);
            if (n_aus++ == 0) { *bx = xs[i]; *bz = zs[j]; }
            continue;                                   /* faellt hier: kein Weiterweg */
        }
        static const int di[4] = { 1, -1, 0, 0 }, dj[4] = { 0, 0, 1, -1 };
        for (int k = 0; k < 4; k++) {
            const int ni = i + di[k], nj = j + dj[k];
            if (ni < 0 || nj < 0 || ni >= W || nj >= H) continue;
            uint8_t *s2 = &st[(size_t)nj * W + ni];
            if (*s2) continue;
            if (re15_collision_box_blocked(&s_rdt, xs[ni], zs[nj], band, r, 2u)) { *s2 = 1; continue; }
            *s2 = 3; q[qt++] = nj * W + ni;
        }
    }
    free(st); free(q);
    printf("--- FREIRAUM %s obj %d Band %d r %d Start (%d,%d): %d ungesperrte Elementarzellen erreichbar, davon "
           "Ausloeser %d, Flaeche %lld%s", room, obj_id, band, (int)r, (int)sx, (int)sz, n_besucht, n_aus, erreicht,
           n_aus ? "" : " -> durch Schieben UNERREICHBAR\n");
    if (n_aus) printf(", erste erreichte Ausloeser-Lage (%d,%d)\n", (int)*bx, (int)*bz);
    return erreicht;
}

/* ---- Gegenprobe: ROOM10F0/10F1-Tritt (Band 0 ohne Absprungzelle) faellt nirgends ------------------------- */
static void raster_ohne_fall(const char *stage, const char *room, uint16_t id)
{
    if (raum(stage, room, id, 3, 102, 0) != 0) { g_fail++; return; }
    const int p = typ4_prop();
    if (p < 0) { printf("FAIL %s: kein Typ-4-Objekt\n", room); g_fail++; return; }
    int n = 0, aus = 0;
    const unsigned a0 = re15_objfall_n_ausloeser();
    for (int32_t x = -16000; x <= 16000; x += 250)
        for (int32_t z = -16000; z <= 16000; z += 250) {
            g_scd.props[p].x = x; g_scd.props[p].z = z;
            re15_objfall_nach_pass(&s_rdt);
            n++;
        }
    aus = (int)(re15_objfall_n_ausloeser() - a0);
    printf("--- %s Tritt Band %d: %d Rasterpunkte, Ausloeser %d\n", room, (int)g_scd.props[p].band, n, aus);
    PRUEF(aus == 0, "%s: %d Ausloeser ohne Absprungzelle im Band", room, aus);
}

int main(int argc, char **argv)
{
    const char *mode = (argc > 1) ? argv[1] : "diag";
    const int pin = !strcmp(mode, "pin");
    setvbuf(stdout, NULL, _IONBF, 0);
    if (!load_banks()) { printf("SKIP: PL00/PL00W01-Baenke fehlen\n"); return 77; }
    raum_5020("ROOM5020", 0x5020);
    raum_5020("ROOM5021", 0x5021);
    raum_20a0("ROOM20A0", 0x20A0);
    raum_20a0("ROOM20A1", 0x20A1);
    raster_ohne_fall("STAGE1", "ROOM10F0", 0x10F0);
    raster_ohne_fall("STAGE1", "ROOM10F1", 0x10F1);
    {   /* Freiraum-Zensus aller fallfaehigen Objekte (A_belege/nb3_absprung_zensus.txt), Lage-Flag 0 = obere Lage. */
        int32_t bx, bz;
        static const struct { const char *st, *rm; uint16_t id; int obj, bank, bit, soll; } F[] = {
            /* soll: 1 = Flaeche > 0 (Fall erreichbar), 0 = Flaeche 0 (unerreichbar), -1 = nur Diagnose */
            { "STAGE5", "ROOM5020", 0x5020, 0, 4, 203, 1 }, { "STAGE5", "ROOM5021", 0x5021, 0, 4, 203, 1 },
            { "STAGE2", "ROOM20A0", 0x20A0, 0, 3, 204, 0 }, { "STAGE2", "ROOM20A1", 0x20A1, 0, 3, 204, 0 },
            { "STAGE1", "ROOM11A0", 0x11A0, 0, 5,   6, 0 }, { "STAGE1", "ROOM11A1", 0x11A1, 0, 5,   6, 0 },
            { "STAGE3", "ROOM3050", 0x3050, 1, 3,  93, -1 }, { "STAGE3", "ROOM3051", 0x3051, 1, 3,  93, -1 },
            { "STAGE1", "ROOM1090", 0x1090, 1, -1,  0, -1 }, { "STAGE1", "ROOM1091", 0x1091, 1, -1,  0, -1 },
            { "STAGE2", "ROOM2050", 0x2050, 6, -1,  0, -1 }, { "STAGE2", "ROOM2051", 0x2051, 6, -1,  0, -1 },
        };
        for (size_t i = 0; i < sizeof F / sizeof F[0]; i++) {
            const long long a = freiraum(F[i].st, F[i].rm, F[i].id, F[i].obj, F[i].bank, F[i].bit, &bx, &bz);
            if (a == -2 && F[i].soll >= 0) PRUEF(0, "%s: obj %d nicht installiert", F[i].rm, F[i].obj);
            if (a < 0) continue;
            if (F[i].soll == 1) PRUEF(a > 0, "%s: Ausloeser nicht ungesperrt erreichbar (Flaeche 0)", F[i].rm);
            if (F[i].soll == 0) PRUEF(a == 0, "%s: Ausloeser ungesperrt erreichbar (Flaeche %lld)", F[i].rm, a);
            if (a > 0) {
                /* Diagnose: an der ersten erreichten Ausloeser-Lage fallen lassen (Spieler auf der Startlage der
                 * Kiste = hinter der Schubrichtung) -> wo landet sie? */
                int p = -1;
                for (int k = 0; k < (int)g_scd.prop_count && k < RE15_SCD_MAX_PROPS; k++)
                    if (g_scd.props[k].active && g_scd.props[k].obj_type == 4 && g_scd.props[k].obj_id == F[i].obj) p = k;
                if (p >= 0) {
                    s_pl->x = g_scd.props[p].x; s_pl->z = g_scd.props[p].z;
                    re15_collision_set_band((int)g_scd.props[p].band);
                    g_scd.props[p].x = bx; g_scd.props[p].z = bz;
                    fall_t o;
                    char tag[64]; snprintf(tag, sizeof tag, "%s obj %d (Diagnose)", F[i].rm, F[i].obj);
                    const int ok = fall_fahren(p, &o, tag);
                    if (F[i].soll == 1) PRUEF(ok && o.y_lande == 0 && g_scd.props[p].band == 0,
                                              "%s: Fall von der erreichten Ausloeser-Lage endet nicht auf Band 0", F[i].rm);
                }
            }
        }
    }
    if (!pin) return 0;
    printf(g_fail ? "FAIL %d\n" : "PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
