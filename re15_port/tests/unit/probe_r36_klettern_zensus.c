/* probe_r36_klettern_zensus.c — Runde 36 Spur A (BLOCKER "auf die verschiebbaren Leitern/Tritte
 * klettern - z.B. ROOM 10F0").
 *
 * Der Nutzer-Wortlaut ist PLURAL ("Leitern/Tritte"), 10F0 nur das Beispiel. Der Zensus ueber alle
 * 240 RDTs (analysis/befunde_runde36/A_belege/zensus_obj_typ4_flags.txt) nennt JEDES Typ-4-Objekt,
 * dessen Flags-Wort (pc[6..7] | 1, LAB_80040914 @0x80040990-a4) Bit 0x100 NICHT traegt — genau die
 * Objekte, die am alten Port-Gate `(flags & 0x101) == 0x101` durchfielen, waehrend das Original
 * FUN_8002d2c0 @0x8002d358-35c (`andi v0,v0,0x101` / `beq v0,zero`) nur bei 0 ueberspringt:
 *
 *   ROOM10F0/10F1 obj 0  Tritt 675x900x600, Schiebezone Aot Slot 10 sce 5 @0x010E0, Lage per (3,102)
 *   ROOM11A0/11A1 obj 0  Kiste 900^3,       Schiebezone Aot Slot 6  sce 5 @0x01044, Lage per (5,6)
 *   ROOM2040/2041 obj 1,2 Tritt 675x900x600 (statisch, Band 3, unbedingt @0x0137A/@0x0139C)
 *   ROOM2060/2061 obj 2  Tritt 675x900x600, Schiebezone Aot Slot 2  sce 5 @0x00E92, Lage per (3,172)
 *   ROOM20A0/20A1 obj 0  Kiste 900^3,       Schiebezonen Slots 4/7/9 sce 5,          Lage per (3,204)
 *   ROOM5020/5021 obj 0  Kiste 900^3,       Schiebezone Aot Slot 4  sce 5 @0x0122E, Lage per (4,203)
 *   (ROOM3091 obj 1, Flags 0x0081, Band 1, Box c(0,-1800,0) h(0,0,0) @0x01508 — kein Tritt, deshalb
 *    kein Riegel "oben angekommen". ⛔ BERICHTIGT (Nachbesserung 1, Abnahme 0 Mangel 1): hier stand
 *    "ohne Box trifft FUN_8002da4c mit Rand -112 nie" — FALSCH. FUN_8002da4c vergleicht
 *    VORZEICHENLOS (@0x8002da8c `lhu v0,6(a2)`, @0x8002da94 `addu v0,v0,t1` = h + Rand,
 *    @0x8002da9c `sll v0,v0,1`, @0x8002daa0 `sltu v0,v0,a1`; Z dito @0x8002daa8-c0 mit `xori 1`).
 *    Bei h = 0 und Rand -112 (@0x8002d398 `addiu a2,zero,-112`) ist h + Rand = 0xFFFFFF90, die
 *    Bedingung kippt: Treffer GENAU DANN, wenn |dx| >= 112 UND |dz| >= 112 vom Boxmittelpunkt.
 *    Die vier achsgeraden Anlaeufe unten (Querabstand 0) treffen deshalb nie; ein Schraeganlauf mit
 *    Querversatz >= 112 trifft und startet den Aufstieg. Gemessen + Riegel im Abschnitt
 *    "Null-Box" am Ende von main(): Wahrheitstafel 12 Punkte, achsgerade 4 Seiten ohne Objekt-
 *    Treffer, Querversatz +-300 auf 8 Seiten Substate 9 / KIND 0x81 / Objekt = obj 1.)
 *
 * Je Raum, je Lage (Flag 0/1) und je Objekt werden die vier Anlaufseiten gefahren (Spieler auf der
 * Ebene des Objekts, Blick auf das Objekt, Abstand Boxkante + 450). Riegel: auf MINDESTENS einer Seite
 * startet die Aktionstaste den Aufstieg (Substate 9 @0x8002d604, KIND 0x81 @0x8002d614/18, Objekt =
 * DAT_800ac78c) und der Aufstieg endet auf der Oberkante (y = obj.y - 2*box_hy, Band + 1
 * @0x80038200-08) mit dem Objekt als Standobjekt (@0x80031d20).
 *
 * Aufruf: probe_r36_klettern_zensus diag   (nur Diagnose, immer exit 0)
 *         probe_r36_klettern_zensus pin    (Riegel)
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

typedef struct {
    const char *stage;
    const char *room;
    uint16_t    room_id;
    int         bank, bit;      /* Lage-Flag des Objekts (-1 = unbedingt) */
    int         riegel;         /* 1 = mindestens ein Aufstieg je Objekt verlangt */
} zensus_t;

static const zensus_t T[] = {
    { "STAGE1", "ROOM10F0", 0x10F0, 3, 102, 1 },
    { "STAGE1", "ROOM10F1", 0x10F1, 3, 102, 1 },
    { "STAGE1", "ROOM11A0", 0x11A0, 5,   6, 1 },
    { "STAGE1", "ROOM11A1", 0x11A1, 5,   6, 1 },
    { "STAGE2", "ROOM2040", 0x2040, -1,  0, 1 },
    { "STAGE2", "ROOM2041", 0x2041, -1,  0, 1 },
    { "STAGE2", "ROOM2060", 0x2060, 3, 172, 1 },
    { "STAGE2", "ROOM2061", 0x2061, 3, 172, 1 },
    { "STAGE2", "ROOM20A0", 0x20A0, 3, 204, 1 },
    { "STAGE2", "ROOM20A1", 0x20A1, 3, 204, 1 },
    { "STAGE5", "ROOM5020", 0x5020, 4, 203, 1 },
    { "STAGE5", "ROOM5021", 0x5021, 4, 203, 1 },
    { "STAGE3", "ROOM3091", 0x3091, -1,  0, 0 },   /* Null-Box: nur Diagnose */
};

static int g_fail = 0;

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

static void reset_room(const zensus_t *e, int flag_val)
{
    re15_actor_init();
    scd_vm_init();
    re15_climb_reset();
    if (e->bank >= 0) re15_game_flag_set(e->bank, e->bit, flag_val);
    g_current_room_id = e->room_id;
    g_room_change.pending = 0;
    s_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_pl->active = 1; s_pl->type = 0; s_pl->hp = 100;
    s_pl->x = 0; s_pl->y = 0; s_pl->z = 0; s_pl->rot_y = 0;
    scd_room_reenter(&s_rdt, s_pl->x, s_pl->z, 0);
    re15_collision_set_band(0);
}

/* Vor das Objekt stellen: Blick entlang der Achse `yaw` (0=+X, 1024=-Z, 2048=-X, 3072=+Z),
 * Abstand = Boxkante + Spielerradius 450 (wie probe_r36_klettern_10f0.c). */
static void stand_before(int p, int yaw)
{
    const int32_t cx = g_scd.props[p].x + g_scd.props[p].box_cx;
    const int32_t cz = g_scd.props[p].z + g_scd.props[p].box_cz;
    const int32_t hx = g_scd.props[p].box_hx, hz = g_scd.props[p].box_hz;
    switch (yaw) {
    case 0:    s_pl->x = cx - hx - 450; s_pl->z = cz;            break;
    case 1024: s_pl->x = cx;            s_pl->z = cz + hz + 450; break;
    case 2048: s_pl->x = cx + hx + 450; s_pl->z = cz;            break;
    default:   s_pl->x = cx;            s_pl->z = cz - hz - 450; break;
    }
    s_pl->y = (int32_t)g_scd.props[p].y;
    s_pl->rot_y = (int16_t)yaw;
    re15_collision_set_band((int)g_scd.props[p].band);
    re15_climb_standing_tick();
}

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

/* Eine Seite fahren; Rueckgabe 1 = Aufstieg gestartet UND oben angekommen. */
static int seite(const zensus_t *e, int flag_val, int p, int yaw)
{
    reset_room(e, flag_val);
    /* Nach dem Reset denselben Prop-Index wiederfinden (Installation ist deterministisch). */
    if (p >= (int)g_scd.prop_count || !g_scd.props[p].active || g_scd.props[p].obj_type != 4) return 0;
    stand_before(p, yaw);
    const int32_t sx = s_pl->x, sz = s_pl->z;      /* Startposition (vor dem Aufstieg) */
    const int pband = (int)g_scd.props[p].band;
    const int reach = re15_prop_reach_test(s_pl->x, s_pl->z, s_pl->rot_y, p, 0);
    const int r = re15_climb_try_start(&s_rdt, 1);
    const int sub = re15_climb_dbg_sub(), kind = re15_climb_dbg_kind(), obj = re15_climb_dbg_obj();
    int ye = 0, be = 0, fe = -1, stand = -1;
    if (r) { fe = aufstieg(&ye, &be); stand = re15_climb_dbg_standing(); }
    const int want_y = (int)g_scd.props[p].y - 2 * g_scd.props[p].box_hy;
    const int ok = r && sub == 9 && kind == 0x81 && obj == p && fe >= 0 &&
                   ye == want_y && be == pband + 1 && stand == p;
    printf("    %s flag=%d obj=%d(prop %d) fl=0x%04X band=%d pos=(%ld,%ld,%ld) yaw=%4d start=(%ld,%ld) "
           "reach=%d try=%d sub=%d kind=0x%02X obj=%d ende=%d oben=(%ld,%d,%ld)(soll y %d) band=%d(soll %d) "
           "stand=%d -> %s\n",
           e->room, flag_val, g_scd.props[p].obj_id, p, g_scd.props[p].flags, pband,
           (long)g_scd.props[p].x, (long)g_scd.props[p].y, (long)g_scd.props[p].z, yaw,
           (long)sx, (long)sz, reach, r, sub, kind, obj, fe, (long)s_pl->x, ye, (long)s_pl->z, want_y,
           be, pband + 1, stand, ok ? "OBEN" : (r ? "gestartet, nicht oben" : "kein Start"));
    re15_climb_reset();
    return ok;
}

/* ==== Null-Box (Nachbesserung 1, Abnahme 0 Mangel 1): ROOM3091 obj 1 =========================
 * FUN_8002da4c @0x8002da4c, selbst disassembliert (Bytes ab @0x8002da8c:
 * 06 00 c2 94  0a 00 c3 94  21 10 49 00  21 28 a2 00  40 10 02 00  2b 10 45 00  03 00 40 10
 * 21 30 69 00 ...  2b 10 43 00  01 00 42 38):
 *   8002da8c lhu  v0,6(a2)     box.hx              8002da90 lhu  v1,10(a2)   box.hz
 *   8002da94 addu v0,v0,t1     hx + Rand           8002da98 addu a1,a1,v0    dx + (hx+Rand)
 *   8002da9c sll  v0,v0,1      2*(hx+Rand)         8002daa0 sltu v0,v0,a1    VORZEICHENLOS
 *   8002daa4 beq  v0,zero,...  (sonst 8002daac/b0 Rueckgabe 0)
 *   8002daa8 addu a2,v1,t1  /  8002dab4 addu v1,a0,a2 / 8002dab8 sll v0,a2,1 / 8002dabc sltu /
 *   8002dac0 xori v0,v0,0x1    Rueckgabe = Z-Bedingung
 * h = 0, Rand -112 (FUN_8002d2c0 @0x8002d398): hx+Rand = 0xFFFFFF90, 2*(hx+Rand) = 0xFFFFFF20;
 * abgelehnt wird nur, wenn (dx - 112) mod 2^32 > 0xFFFFFF20, also -111 <= dx <= 111.
 * -> Treffer <=> |dx| >= 112 UND |dz| >= 112. Die Tafel unten ist mit genau diesen Instruktionen
 * nachgerechnet (Dossier A_klettern.md, Nachbesserung 1). */
typedef struct { int dx, dz, soll; } tafel_t;
static const tafel_t TAFEL[] = {
    {    0,    0, 0 }, {    0,  300, 0 }, {  300,    0, 0 }, {  111,  111, 0 },
    { -111,  500, 0 }, {  112, -111, 0 },
    {  112,  112, 1 }, { -112, -112, 1 }, { -112,  500, 1 }, {  300,  300, 1 },
    { -500,  800, 1 }, {  900, -900, 1 },
};

static int nullbox_tafel(int p)
{
    const int32_t cx = g_scd.props[p].x + g_scd.props[p].box_cx;
    const int32_t cz = g_scd.props[p].z + g_scd.props[p].box_cz;
    const int band = (int)g_scd.props[p].band;
    int bad = 0;
    for (size_t i = 0; i < sizeof TAFEL / sizeof TAFEL[0]; i++) {
        const int ist = re15_collision_prop_box_hit(p, cx + TAFEL[i].dx, cz + TAFEL[i].dz, -112, band);
        printf("    Tafel FUN_8002da4c h=0 Rand=-112 dx=%5d dz=%5d -> %d (soll %d)%s\n",
               TAFEL[i].dx, TAFEL[i].dz, ist, TAFEL[i].soll, ist == TAFEL[i].soll ? "" : "  <-- ABWEICHUNG");
        if (ist != TAFEL[i].soll) bad++;
    }
    return bad;
}

/* Anlauf mit QUERVERSATZ q (quer zur Blickachse), sonst wie stand_before(). Spieler-Band
 * `sband` (-1 = Objekt-Band: @0x8002d364-70 verlangt Spieler-Band == Objekt-Band), Spieler-Y auf
 * der Bandebene -1800*Band. Rueckgabe: 1 = Objekt-Treffer der Sonde (Substate 9, KIND 0x81,
 * Objekt = p), 0 = kein Objekt-Treffer. */
static int seite_quer(const zensus_t *e, int p, int yaw, int q, int sband)
{
    reset_room(e, 0);
    if (p >= (int)g_scd.prop_count || !g_scd.props[p].active) return -1;
    const int32_t cx = g_scd.props[p].x + g_scd.props[p].box_cx;
    const int32_t cz = g_scd.props[p].z + g_scd.props[p].box_cz;
    const int32_t hx = g_scd.props[p].box_hx, hz = g_scd.props[p].box_hz;
    switch (yaw) {
    case 0:    s_pl->x = cx - hx - 450; s_pl->z = cz + q;            break;
    case 1024: s_pl->x = cx + q;        s_pl->z = cz + hz + 450;     break;
    case 2048: s_pl->x = cx + hx + 450; s_pl->z = cz + q;            break;
    default:   s_pl->x = cx + q;        s_pl->z = cz - hz - 450;     break;
    }
    const int pband = (int)g_scd.props[p].band;
    const int spl   = (sband >= 0) ? sband : pband;
    s_pl->y = -1800 * spl;
    s_pl->rot_y = (int16_t)yaw;
    re15_collision_set_band(spl);
    re15_climb_standing_tick();
    const int32_t sx = s_pl->x, sy = s_pl->y, sz = s_pl->z;
    /* Sondenpunkt wie re15_climb_try_start: 800 voraus, Kurs auf 90 Grad gerastet
     * (@0x8002d4d4-dc); bei achsgeradem yaw also exakt 800 entlang der Achse. */
    const int32_t fx = sx + (yaw == 0 ? 800 : yaw == 2048 ? -800 : 0);
    const int32_t fz = sz + (yaw == 1024 ? -800 : yaw == 3072 ? 800 : 0);
    const int reach = re15_prop_reach_test(sx, sz, s_pl->rot_y, p, 0);   /* @0x8002d37c */
    const int bhit  = re15_collision_prop_box_hit(p, fx, fz, -112, pband); /* @0x8002d394 */
    const int r = re15_climb_try_start(&s_rdt, 1);
    const int sub = re15_climb_dbg_sub(), kind = re15_climb_dbg_kind(), obj = re15_climb_dbg_obj();
    const int treffer = (r && sub == 9 && kind == 0x81 && obj == p) ? 1 : 0;
    int ye = 0, be = 0, fe = -1, stand = -1;
    if (r) { fe = aufstieg(&ye, &be); stand = re15_climb_dbg_standing(); }
    printf("    %s obj=%d(prop %d) fl=0x%04X band=%d box=h(%d,%d,%d) spieler-band=%d yaw=%4d quer=%4d "
           "start=(%ld,%ld,%ld) sonde=(%ld,%ld) d=(%ld,%ld) reach=%d boxhit(-112)=%d try=%d sub=%d "
           "kind=0x%02X obj=%d ende=%d end=(%ld,%d,%ld) band=%d stand=%d -> %s\n",
           e->room, g_scd.props[p].obj_id, p, g_scd.props[p].flags, pband,
           (int)g_scd.props[p].box_hx, (int)g_scd.props[p].box_hy, (int)g_scd.props[p].box_hz,
           spl, yaw, q, (long)sx, (long)sy, (long)sz, (long)fx, (long)fz, (long)(fx - cx), (long)(fz - cz),
           reach, bhit, r, sub, kind, obj, fe, (long)s_pl->x, ye, (long)s_pl->z, be, stand,
           treffer ? "Objekt-Treffer (Start)" : "kein Objekt-Treffer");
    re15_climb_reset();
    return treffer;
}

static int nullbox_3091(void)
{
    static const zensus_t E = { "STAGE3", "ROOM3091", 0x3091, -1, 0, 0 };
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/%s/%s.RDT", RE15_ASSET_PSX_DIR, E.stage, E.room);
    uint8_t *raw = read_file(path, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", path); return -77; }
    if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("SKIP: RDT-Parse %s\n", E.room); return -77; }
    reset_room(&E, 0);
    int p = -1;
    for (int k = 0; k < (int)g_scd.prop_count && k < 16; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_id == 1 && g_scd.props[k].obj_type == 4) p = k;
    printf("--- Null-Box ROOM3091 obj 1 (sub00 @0x01508): prop %d\n", p);
    if (p < 0 || g_scd.props[p].box_hx != 0 || g_scd.props[p].box_hz != 0 ||
        (g_scd.props[p].flags & 0x101u) == 0u || g_scd.props[p].band != 1) {
        printf("FAIL Null-Box: obj 1 nicht wie @0x01508 installiert (Flags 0x0081, Band 1, h 0)\n");
        return 1;
    }
    int bad = nullbox_tafel(p);
    static const int yaws[4] = { 0, 1024, 2048, 3072 };
    int achs = 0, quer = 0, boden = 0;
    for (int y = 0; y < 4; y++) {
        int t = seite_quer(&E, p, yaws[y], 0, -1);
        if (t == 0) achs++;
        for (int s = -1; s <= 1; s += 2)
            if (seite_quer(&E, p, yaws[y], 300 * s, -1) == 1) quer++;
    }
    /* Derselbe Schraeganlauf vom BODEN (Band 0): so steht der Spieler in ROOM3091 nach jeder Tuer
     * (alle Door_aot_set nach 309x: next_y = 0) — das Band-Gate @0x8002d370 `bne v1,v0` haelt das
     * Band-1-Objekt dann aus der Sonde. */
    for (int y = 0; y < 4; y++)
        for (int s = -1; s <= 1; s += 2)
            if (seite_quer(&E, p, yaws[y], 300 * s, 0) == 0) boden++;
    printf("Null-Box: Tafel %d Abweichungen, achsgerade ohne Objekt-Treffer %d/4, Querversatz +-300 mit "
           "Start %d/8 (Spieler Band 1), vom Boden (Band 0) ohne Objekt-Treffer %d/8\n",
           bad, achs, quer, boden);
    if (achs != 4)  { printf("FAIL Null-Box: achsgerader Anlauf traf das Objekt (|dx| < 112)\n"); bad++; }
    if (quer != 8)  { printf("FAIL Null-Box: Schraeganlauf ohne Start (|dx|,|dz| >= 112 muss treffen)\n"); bad++; }
    if (boden != 8) { printf("FAIL Null-Box: Spieler auf Band 0 traf das Band-1-Objekt (@0x8002d370)\n"); bad++; }
    return bad;
}

int main(int argc, char **argv)
{
    const char *mode = (argc > 1) ? argv[1] : "diag";
    const int pin = !strcmp(mode, "pin");
    /* Ungepuffert: die Engine schreibt auf stderr ([scd] ...); mit 2>&1 in EINE Datei zerschnitt
     * der Blockpuffer sonst einzelne Messzeilen (Abnahme 0, Abschnitt 2). */
    setvbuf(stdout, NULL, _IONBF, 0);
    if (!load_banks()) { printf("SKIP: PL00/PL00W01-Baenke fehlen\n"); return 77; }

    int objekte = 0, objekte_ok = 0;
    for (size_t i = 0; i < sizeof T / sizeof T[0]; i++) {
        const zensus_t *e = &T[i];
        char path[600]; size_t sz = 0;
        snprintf(path, sizeof path, "%s/%s/%s.RDT", RE15_ASSET_PSX_DIR, e->stage, e->room);
        uint8_t *raw = read_file(path, &sz);
        if (!raw) { printf("SKIP: %s fehlt\n", path); return 77; }
        if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("SKIP: RDT-Parse %s\n", e->room); return 77; }

        const int n_flag = (e->bank >= 0) ? 2 : 1;
        for (int fv = 0; fv < n_flag; fv++) {
            reset_room(e, fv);
            /* Kandidaten: aktive Typ-4-Objekte OHNE Bit 0x100 (= die vom alten Gate betroffenen). */
            int kand[16], nk = 0;
            for (int p = 0; p < (int)g_scd.prop_count && p < 16; p++)
                if (g_scd.props[p].active && g_scd.props[p].obj_type == 4 &&
                    (g_scd.props[p].flags & 0x100u) == 0u)
                    kand[nk++] = p;
            printf("--- %s flag(%d,%d)=%d: prop_count=%d, Typ-4 ohne 0x100: %d\n",
                   e->room, e->bank, e->bit, fv, (int)g_scd.prop_count, nk);
            for (int k = 0; k < nk; k++) {
                const int p = kand[k];
                static const int yaws[4] = { 0, 1024, 2048, 3072 };
                int seiten_ok = 0;
                for (int y = 0; y < 4; y++) seiten_ok += seite(e, fv, p, yaws[y]);
                if (!e->riegel) continue;
                objekte++;
                if (seiten_ok > 0) objekte_ok++;
                else {
                    printf("FAIL %s flag=%d prop %d: auf KEINER Seite Aufstieg (FUN_8002d2c0 @0x8002d358-35c)\n",
                           e->room, fv, p);
                    g_fail++;
                }
            }
            if (e->riegel && nk == 0) {
                printf("FAIL %s flag=%d: kein Typ-4-Objekt ohne 0x100 installiert (Zensus-Erwartung)\n",
                       e->room, fv);
                g_fail++;
            }
        }
        /* raw bleibt stehen: s_rdt/g_room-Zeiger verweisen bis zum naechsten Parse hinein. */
    }
    printf("Zensus: %d/%d Objekte (Raum x Lage) kletterbar\n", objekte_ok, objekte);
    {
        const int nb = nullbox_3091();
        if (nb == -77) return 77;
        g_fail += nb;
    }
    if (!pin) return 0;
    printf(g_fail ? "FAIL %d\n" : "PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
