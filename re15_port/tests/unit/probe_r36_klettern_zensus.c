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
 *   ROOM11A0/11A1 obj 0  Kiste 900^3,       Schiebezone Aot Slot 6  sce 5 @0x01044, Lage per (5,6) -
 *                        (5,6) = Bank 5 Wort 0, beim Laden IMMER 0 (Nachbesserung 2, unten):
 *                        der Record @0x0107E (Band 0) wird im Auslieferungsstand nie installiert
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
 * Gefahren wird je RECORD (Raum x Objekt x Lage = ein Obj_model_set des RDT, Datei-Byte-Offset in R[]):
 * 24 Records. Je Record (1) die LAGE-PRUEFUNG: das installierte Objekt muss dem Record entsprechen
 * (obj_id, Typ 4, Band = pc[4], x/y/z = pc[10..15], Flags = pc[6..7] | 1, Box = pc[22..33];
 * LAB_80040914 @0x8004096c-74 / @0x80040990-a4) - ein wirkungsloses Lage-Flag wird rot;
 * (2) die Kandidaten-Zaehlung (aktive Typ-4-Objekte ohne 0x100 = erwartete Zahl); (3) die vier
 * Anlaufseiten (Spieler auf der Ebene des Objekts, Blick auf das Objekt, Abstand Boxkante + 450).
 * Riegel: auf MINDESTENS einer Seite startet die Aktionstaste den Aufstieg (Substate 9 @0x8002d604,
 * KIND 0x81 @0x8002d614/18, Objekt = DAT_800ac78c) und der Aufstieg endet auf der Oberkante
 * (y = obj.y - 2*box_hy, Band + 1 @0x80038200-08) mit dem Objekt als Standobjekt (@0x80031d20).
 *
 * NACHBESSERUNG 2 (Abnahme 1 Mangel 1): bis dahin setzte die Sonde das Lage-Flag VOR dem Raum-Init.
 * Der nullt Bank 5 Wort 0 (scd_room_setup.c = FUN_8003ecec @0x8003ed74 `sw zero,0x800b1028`, einziger
 * Aufruf @0x8003ef84 in FUN_8003ef6c, die main00 erst DANACH startet: @0x8003efa0-b0 Thread 0 =
 * RDT+0x40, @0x8003f018 Dispatcher). (5,6) war damit wirkungslos, die Lage-1-Zeilen von 11A0/11A1
 * fuhren die Lage 0 ein zweites Mal, und der Riegel zaehlte das als "24/24". Jetzt:
 *   - Lage-Flags der Banks 3/4 (persistent, vom Raum-Init nicht beruehrt) werden wie ein
 *     gespeicherter Spielstand VOR dem Raum-Init gesetzt = "beim Laden erreichbar".
 *   - (5,6)=1 ist beim Laden NIE gesetzt (der Latch in sub01 @0x012B2-c4 wirkt erst auf das
 *     naechste main00, und vor dem steht wieder der Wisch; das Objekt-Band +0x82 schreiben nur der
 *     Installer @0x80040974 und Member 18 @0x80041228, ROOM11A0/11A1 setzen nur Member 1). Der
 *     Record @0x0107E wird deshalb ERZWUNGEN gefahren: Flag NACH dem Wisch, ueber den einzigen
 *     Rueckruf zwischen Wisch und main00-Start (re15_scd_set_player_model_sync, scd_room_reenter),
 *     und getrennt gezaehlt.
 *   - Gegenprobe: (5,6)=1 VOR dem Raum-Init (der alte Weg) -> Flag danach 0, installiert ist
 *     @0x01058 statt @0x0107E - die Lage-Pruefung MUSS das melden, sonst ist sie selbst blind.
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
    int         bank, bit, val; /* Lage-Flag + Wert des Records (bank -1 = unbedingt) */
    int         laden;          /* 1 = beim Laden erreichbar (Flag wirkt in main00/sub00);
                                 * 0 = nur ERZWUNGEN (Bank 5: Wisch @0x8003ed74 vor main00) */
    uint32_t    rec;            /* Datei-Byte-Offset des Obj_model_set (Opcode 0x2D) im RDT */
    int         n_kand;         /* erwartete Zahl aktiver Typ-4-Objekte ohne 0x100 in dieser Lage */
    int         riegel;         /* 1 = mindestens ein Aufstieg verlangt */
} rec_t;

/* Offsets selbst gelesen (tools/scd_dump_room.py, Dossier A_klettern.md "Zensus-Pin" +
 * "Nachbesserung 2"); die Lage-Pruefung liest die Bytes am Offset selbst und prueft Opcode 0x2D,
 * Typ 4 und obj_id - ein falscher Offset faellt also auf. */
static const rec_t R[] = {
    { "STAGE1", "ROOM10F0", 0x10F0, 3, 102, 0, 1, 0x010F4, 1, 1 },
    { "STAGE1", "ROOM10F0", 0x10F0, 3, 102, 1, 1, 0x0111A, 1, 1 },
    { "STAGE1", "ROOM10F1", 0x10F1, 3, 102, 0, 1, 0x010F4, 1, 1 },
    { "STAGE1", "ROOM10F1", 0x10F1, 3, 102, 1, 1, 0x0111A, 1, 1 },
    { "STAGE1", "ROOM11A0", 0x11A0, 5,   6, 0, 1, 0x01058, 1, 1 },
    { "STAGE1", "ROOM11A0", 0x11A0, 5,   6, 1, 0, 0x0107E, 1, 1 },   /* ERZWUNGEN */
    { "STAGE1", "ROOM11A1", 0x11A1, 5,   6, 0, 1, 0x01058, 1, 1 },
    { "STAGE1", "ROOM11A1", 0x11A1, 5,   6, 1, 0, 0x0107E, 1, 1 },   /* ERZWUNGEN */
    { "STAGE2", "ROOM2040", 0x2040, -1,  0, 0, 1, 0x0137A, 2, 1 },
    { "STAGE2", "ROOM2040", 0x2040, -1,  0, 0, 1, 0x0139C, 2, 1 },
    { "STAGE2", "ROOM2041", 0x2041, -1,  0, 0, 1, 0x0137A, 2, 1 },
    { "STAGE2", "ROOM2041", 0x2041, -1,  0, 0, 1, 0x0139C, 2, 1 },
    { "STAGE2", "ROOM2060", 0x2060, 3, 172, 0, 1, 0x00EA6, 1, 1 },
    { "STAGE2", "ROOM2060", 0x2060, 3, 172, 1, 1, 0x00ECC, 1, 1 },
    { "STAGE2", "ROOM2061", 0x2061, 3, 172, 0, 1, 0x00EA6, 1, 1 },
    { "STAGE2", "ROOM2061", 0x2061, 3, 172, 1, 1, 0x00ECC, 1, 1 },
    { "STAGE2", "ROOM20A0", 0x20A0, 3, 204, 0, 1, 0x01854, 1, 1 },
    { "STAGE2", "ROOM20A0", 0x20A0, 3, 204, 1, 1, 0x0187A, 1, 1 },
    { "STAGE2", "ROOM20A1", 0x20A1, 3, 204, 0, 1, 0x01854, 1, 1 },
    { "STAGE2", "ROOM20A1", 0x20A1, 3, 204, 1, 1, 0x0187A, 1, 1 },
    { "STAGE5", "ROOM5020", 0x5020, 4, 203, 0, 1, 0x01242, 1, 1 },
    { "STAGE5", "ROOM5020", 0x5020, 4, 203, 1, 1, 0x01268, 1, 1 },
    { "STAGE5", "ROOM5021", 0x5021, 4, 203, 0, 1, 0x01262, 1, 1 },
    { "STAGE5", "ROOM5021", 0x5021, 4, 203, 1, 1, 0x01288, 1, 1 },
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

static re15_rdt_t     s_rdt;
static const uint8_t *s_raw;      /* Datei-Bytes des geparsten RDT (fuer die Lage-Pruefung) */
static size_t         s_raw_sz;
static re15_actor_t  *s_pl;

/* ERZWUNGENE Lage: Flag NACH dem Bank-5-Wisch setzen. scd_room_reenter ruft den Modell-Sync-
 * Rueckruf nach `g_game.flags[5][0] = 0` (= @0x8003ed74) und vor scd_thread_start(main00/sub00)
 * (= @0x8003efb0/@0x8003efd4) - die einzige Stelle dazwischen, die ein Testprogramm erreicht. */
static int s_nach_bank = -1, s_nach_bit = 0, s_nach_val = 0;
static void nach_wisch(void)
{
    if (s_nach_bank >= 0) re15_game_flag_set((uint8_t)s_nach_bank, (uint8_t)s_nach_bit, s_nach_val);
}

/* modus 0 = wie R[] (laden: Flag vor dem Raum-Init; erzwungen: Flag nach dem Wisch);
 * modus 1 = GEGENPROBE: Flag immer VOR dem Raum-Init (der Weg der Sonde bis Nachbesserung 1). */
static void reset_room_modus(const rec_t *e, int modus)
{
    re15_actor_init();
    scd_vm_init();
    re15_climb_reset();
    s_nach_bank = -1;
    if (e->bank >= 0) {
        if (e->laden || modus == 1)
            re15_game_flag_set((uint8_t)e->bank, (uint8_t)e->bit, e->val);
        else {
            re15_game_flag_set((uint8_t)e->bank, (uint8_t)e->bit, 0);
            s_nach_bank = e->bank; s_nach_bit = e->bit; s_nach_val = e->val;
        }
    }
    re15_scd_set_player_model_sync(s_nach_bank >= 0 ? nach_wisch : NULL);
    g_current_room_id = e->room_id;
    g_room_change.pending = 0;
    s_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_pl->active = 1; s_pl->type = 0; s_pl->hp = 100;
    s_pl->x = 0; s_pl->y = 0; s_pl->z = 0; s_pl->rot_y = 0;
    scd_room_reenter(&s_rdt, s_pl->x, s_pl->z, 0);
    re15_scd_set_player_model_sync(NULL);
    s_nach_bank = -1;
    re15_collision_set_band(0);
}
static void reset_room(const rec_t *e) { reset_room_modus(e, 0); }

static int rd16(const uint8_t *b) { return (int)(int16_t)(uint16_t)(b[0] | (b[1] << 8)); }

/* LAGE-PRUEFUNG: steht das Objekt des Records so im Pool, wie der Record es installiert?
 * Rueckgabe Prop-Index (>= 0) oder -1 (Abweichung; Grund in why). */
static int lage_pruefen(const rec_t *e, char *why, size_t whylen)
{
    if (e->rec + 34u > s_raw_sz) { snprintf(why, whylen, "Offset @0x%05X ausserhalb des RDT", (unsigned)e->rec); return -1; }
    const uint8_t *r = s_raw + e->rec;
    if (r[0] != 0x2D || r[2] != 4) {
        snprintf(why, whylen, "@0x%05X ist kein Obj_model_set Typ 4 (%02X .. %02X)", (unsigned)e->rec, r[0], r[2]);
        return -1;
    }
    const int obj = r[1], band = r[4];
    const unsigned fl = (unsigned)((r[6] | (r[7] << 8)) | 1u);            /* @0x80040990-a4 */
    const int x = rd16(r + 10), y = rd16(r + 12), z = rd16(r + 14);
    int p = -1;
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj) { p = k; break; }
    if (p < 0) { snprintf(why, whylen, "obj %d nicht im Pool", obj); return -1; }
    if (!g_scd.props[p].active || g_scd.props[p].obj_type != 4 || (int)g_scd.props[p].band != band ||
        (int)g_scd.props[p].x != x || (int)g_scd.props[p].y != y || (int)g_scd.props[p].z != z ||
        (unsigned)g_scd.props[p].flags != fl ||
        (int)g_scd.props[p].box_cx != rd16(r + 22) || (int)g_scd.props[p].box_cy != rd16(r + 24) ||
        (int)g_scd.props[p].box_cz != rd16(r + 26) || (int)g_scd.props[p].box_hx != rd16(r + 28) ||
        (int)g_scd.props[p].box_hy != rd16(r + 30) || (int)g_scd.props[p].box_hz != rd16(r + 32)) {
        snprintf(why, whylen, "soll Band %d (%d,%d,%d) fl 0x%04X, ist aktiv %d Band %d (%ld,%ld,%ld) fl 0x%04X",
                 band, x, y, z, fl, (int)g_scd.props[p].active, (int)g_scd.props[p].band,
                 (long)g_scd.props[p].x, (long)g_scd.props[p].y, (long)g_scd.props[p].z,
                 (unsigned)g_scd.props[p].flags);
        return -1;
    }
    snprintf(why, whylen, "obj %d Band %d (%d,%d,%d) fl 0x%04X box c(%d,%d,%d) h(%d,%d,%d) = Record", obj, band,
             x, y, z, fl, rd16(r + 22), rd16(r + 24), rd16(r + 26), rd16(r + 28), rd16(r + 30), rd16(r + 32));
    return p;
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
static int seite(const rec_t *e, int yaw)
{
    reset_room(e);
    char why[200];
    const int p = lage_pruefen(e, why, sizeof why);   /* je Seite erneut: der Reset ist pro Seite */
    if (p < 0) { printf("    %s @0x%05X yaw=%4d: Lage falsch (%s)\n", e->room, (unsigned)e->rec, yaw, why); return 0; }
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
    printf("    %s lage=@0x%05X flag=%d%s obj=%d(prop %d) fl=0x%04X band=%d pos=(%ld,%ld,%ld) yaw=%4d "
           "start=(%ld,%ld) reach=%d try=%d sub=%d kind=0x%02X obj=%d ende=%d oben=(%ld,%d,%ld)(soll y %d) "
           "band=%d(soll %d) stand=%d -> %s\n",
           e->room, (unsigned)e->rec, e->val, e->laden ? "" : "(ERZWUNGEN)", g_scd.props[p].obj_id, p,
           (unsigned)g_scd.props[p].flags, pband,
           (long)g_scd.props[p].x, (long)g_scd.props[p].y, (long)g_scd.props[p].z, yaw,
           (long)sx, (long)sz, reach, r, sub, kind, obj, fe, (long)s_pl->x, ye, (long)s_pl->z, want_y,
           be, pband + 1, stand, ok ? "OBEN" : (r ? "gestartet, nicht oben" : "kein Start"));
    re15_climb_reset();
    return ok;
}

/* RDT eines Records laden (gecacht je Raum). raw bleibt stehen: s_rdt/g_room-Zeiger verweisen bis
 * zum naechsten Parse hinein. Rueckgabe 0 = ok, 77 = SKIP. */
static int rdt_laden(const char *stage, const char *room)
{
    static char cur[32] = "";
    if (!strcmp(cur, room)) return 0;
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/%s/%s.RDT", RE15_ASSET_PSX_DIR, stage, room);
    uint8_t *raw = read_file(path, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", path); return 77; }
    if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("SKIP: RDT-Parse %s\n", room); return 77; }
    s_raw = raw; s_raw_sz = sz;
    snprintf(cur, sizeof cur, "%s", room);
    return 0;
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
 * der Bandebene -1800*Band (0x708 je Band: Aufstieg @0x800381e4 `addiu v0,v0,-1800`, Band +1
 * @0x80038200-08). q = +-300 ist eine Testplatzierung (>= 112 gemaess Tafel), 450 der Abstand wie
 * stand_before(). Rueckgabe: 1 = Objekt-Treffer der Sonde (Substate 9, KIND 0x81, Objekt = p),
 * 0 = kein Objekt-Treffer. */
static int seite_quer(const rec_t *e, int p, int yaw, int q, int sband)
{
    reset_room(e);
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
    /* Sondenpunkt wie re15_climb_try_start: {800,0} (DAT_800109c8, lwl/lwr @0x8002d4a0-bc)
     * voraus, Kurs auf 90 Grad gerastet (@0x8002d4d4-dc); bei achsgeradem yaw also exakt 800
     * entlang der Achse. */
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
    static const rec_t E = { "STAGE3", "ROOM3091", 0x3091, -1, 0, 0, 1, 0x01508, 0, 0 };
    if (rdt_laden(E.stage, E.room) != 0) return -77;
    reset_room(&E);
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

    int n_all = 0, ok_all = 0, n_laden = 0, ok_laden = 0, n_erzw = 0, ok_erzw = 0;
    for (size_t i = 0; i < sizeof R / sizeof R[0]; i++) {
        const rec_t *e = &R[i];
        if (rdt_laden(e->stage, e->room) != 0) return 77;

        reset_room(e);
        char why[200];
        const int p = lage_pruefen(e, why, sizeof why);
        int nk = 0, drin = 0;
        for (int k = 0; k < (int)g_scd.prop_count && k < 16; k++)
            if (g_scd.props[k].active && g_scd.props[k].obj_type == 4 && (g_scd.props[k].flags & 0x100u) == 0u) {
                nk++;
                if (k == p) drin = 1;
            }
        const int flag_ist = (e->bank >= 0) ? re15_game_flag_get((uint8_t)e->bank, (uint8_t)e->bit) : -1;
        printf("--- %s Record @0x%05X flag(%d,%d)=%d %s: Flag nach dem Raum-Init %d; Lage %s (%s); "
               "Typ-4 ohne 0x100: %d (soll %d)\n",
               e->room, (unsigned)e->rec, e->bank, e->bit, e->val,
               e->laden ? "beim Laden" : "ERZWUNGEN (Flag nach dem Wisch @0x8003ed74)",
               flag_ist, p >= 0 ? "OK" : "FALSCH", why, nk, e->n_kand);
        int rec_ok = 1;
        if (p < 0) {
            printf("FAIL %s @0x%05X: installierte Lage != Record (Lage-Flag wirkungslos?)\n",
                   e->room, (unsigned)e->rec);
            rec_ok = 0;
        }
        if (nk != e->n_kand || (p >= 0 && !drin)) {
            printf("FAIL %s @0x%05X: %d Typ-4-Objekte ohne 0x100 (soll %d, Record-Objekt dabei: %d)\n",
                   e->room, (unsigned)e->rec, nk, e->n_kand, drin);
            rec_ok = 0;
        }
        int seiten_ok = 0;
        if (p >= 0) {
            static const int yaws[4] = { 0, 1024, 2048, 3072 };
            for (int y = 0; y < 4; y++) seiten_ok += seite(e, yaws[y]);
        }
        if (e->riegel && seiten_ok == 0) {
            printf("FAIL %s @0x%05X: auf KEINER Seite Aufstieg (FUN_8002d2c0 @0x8002d358-35c)\n",
                   e->room, (unsigned)e->rec);
            rec_ok = 0;
        }
        if (!e->riegel) continue;
        n_all++;   if (rec_ok) ok_all++;
        if (e->laden) { n_laden++; if (rec_ok) ok_laden++; }
        else          { n_erzw++;  if (rec_ok) ok_erzw++;  }
        if (!rec_ok) g_fail++;
    }
    printf("Zensus: %d/%d Records (Raum x Objekt x Lage) wie der RDT-Record installiert und kletterbar; davon "
           "beim Laden erreichbar %d/%d, nur ERZWUNGEN (11A0/11A1 @0x0107E, Bank 5) %d/%d\n",
           ok_all, n_all, ok_laden, n_laden, ok_erzw, n_erzw);

    /* GEGENPROBE (Abnahme 1 Mangel 1): der alte Weg - (5,6)=1 VOR dem Raum-Init - muss am Raum-Init
     * scheitern (Flag danach 0, @0x8003ed74) und die Lage-Pruefung muss das erkennen. */
    for (size_t i = 0; i < sizeof R / sizeof R[0]; i++) {
        const rec_t *e = &R[i];
        if (e->laden) continue;
        if (rdt_laden(e->stage, e->room) != 0) return 77;
        reset_room_modus(e, 1);
        char why[200];
        const int p = lage_pruefen(e, why, sizeof why);
        const int flag_ist = re15_game_flag_get((uint8_t)e->bank, (uint8_t)e->bit);
        printf("--- GEGENPROBE %s: flag(%d,%d)=1 VOR dem Raum-Init -> Flag danach %d, Lage-Pruefung @0x%05X: %s (%s)\n",
               e->room, e->bank, e->bit, flag_ist, (unsigned)e->rec, p >= 0 ? "OK" : "FALSCH", why);
        if (flag_ist != 0 || p >= 0) {
            printf("FAIL GEGENPROBE %s: Bank-5-Wisch (@0x8003ed74) nicht wirksam oder Lage-Pruefung blind\n",
                   e->room);
            g_fail++;
        }
    }
    {
        const int nb = nullbox_3091();
        if (nb == -77) return 77;
        g_fail += nb;
    }
    if (!pin) return 0;
    printf(g_fail ? "FAIL %d\n" : "PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
