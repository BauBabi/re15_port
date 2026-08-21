/* test_1090_room_pin.c — PIN fuer zwei Nutzer-Befunde in ROOM1090 (Spieltest v0.3.5):
 *
 *   (D) "Kiste im Raum ist irgendwie zu tief."
 *   (C) "Cutscene bei Feuer-Transporter startet noch nicht."
 *
 * ---------------------------------------------------------------------------
 * SOLLSEITE — Roh-Scan von STAGE1/ROOM1090.RDT (Datei-Offsets, Bytes selbst gedumpt)
 * ---------------------------------------------------------------------------
 * main00 @0x211A:
 *   @0x216E  2D 00 04 00 01 00 00 01 00 00 38 E0 F8 F8 91 12 ...
 *            Obj_model_set obj0: pc[2]=TYP 4, pc[4]=BAND 1, pos LE (-8136,-1800,4753),
 *            Box (pc[22..33]) = c(0,-900,0) h(900,900,900)  -> 1800 hohe Kiste
 *   @0x2190  2D 01 04 00 05 00 00 01 00 00 B8 E8 D8 DC 86 C3 ...
 *            Obj_model_set obj1: TYP 4, BAND 5, pos (-5960,-9000,-15482), gleiche Box
 * sub00 @0x21C4:  Switch(work_vars[0x0A]) { case 0: Cut_chg 8 ; case 3: Cut_chg 0x0B }
 * sub01 @0x23F4:  If Ck(3,0x80)==0 { If Cmp(work_vars[0x0A] == 13) { GOSUB sub02;
 *                                                                    Set(3,0x80)=1 } }
 * sub02 @0x2414:  die Cutscene (Spieler-Teleport + Message 0/1/2 + Plc_motion).
 *
 * ---------------------------------------------------------------------------
 * ORIGINAL-MECHANISMUS (disassembliert, Adressen belegt)
 * ---------------------------------------------------------------------------
 * (D) FUN_8002c18c, der Objekt-Draw-Loop, hebt JEDEN Typ-4-Prop beim Zeichnen um 900 an:
 *       8002c234 lbu   v1,8(s1)         ; obj[+0x08] = Typ (Obj_model_set pc[2])
 *       8002c238 ori   v0,zero,0x4
 *       8002c23c bne   v1,v0,0x8002c254
 *       8002c244 lw    v0,96(s1)        ; obj[+0x60] = MATRIX(+0x48).t[1] = Translations-Y
 *       8002c24c addiu v0,v0,-900
 *       8002c250 sw    v0,96(s1)
 *     Der Port zeichnete ohne diesen Versatz -> die Kiste sass ihre halbe Hoehe (900) tief.
 *
 * (C) work_vars[0x0A] (= DAT_800b0fe4, Basis DAT_800b0fd0 + 0x0A*2) IST der AKTIVE
 *     KAMERA-CUT. Drei Schreiber im Original:
 *       - Raum-Betreten  FUN_8001d600 @0x8001d930 `lbu v1,10(a0)` / @0x8001d948
 *         `sh v1,4068(at)`  -> Eintritts-Cut der Tuer (Door_aot_set pc[24])
 *       - Cut_chg (0x29) LAB_800402a0 @0x800402fc `sh a0,4068(at)`
 *       - PRO BILD     FUN_80021bc0 @0x80021bfc `sh v1,4068(at)`, v1 = DAT_800afbb5
 *         = der Cut, den der RVD-Zonen-Scan gesetzt hat.
 *     Der Port setzte work_vars[10] nur EINMAL beim Raum-Laden -> Cmp(==13) nie wahr.
 *     Belegt, dass nur der Per-Frame-Weg Cut 13 liefern kann: game-weiter
 *     Door_aot_set-Zensus -> alle Tueren nach ROOM1090 tragen Eintritts-Cut 0/3/6
 *     (ROOM10F0 @0x0F52 cut 0, ROOM1050 @0x0B7A cut 3, ROOM1090 sub00 @0x2352 cut 6).
 *     Cut 13 kommt ausschliesslich aus den RVD-Zonen (@0x0514 from 12 -> to 13,
 *     @0x0578 from 14 -> 13, @0x05A0 from 15 -> 13).
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

extern scd_vm_t g_scd;
extern re15_aot_state_t g_aot;

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); g_fail = 1; } } while (0)

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

/* Der ANGEZEIGTE Cut — im Spiel `active_cut_idx`/`s_last_cut_idx` (platform/pc/main.c) bzw.
 * `cam_active_cut` (platform/psx/main.c). Die Sonde fuehrt ihn genauso mit, damit ihr Bild-
 * Modell vollstaendig ist: Scan + Present, nicht nur Scan. */
static int s_shown_cut = 0;

static void enter(re15_rdt_t *rdt, uint8_t entry_cut, int32_t px, int32_t pz)
{
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1090;
    g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = -1800; pl->z = pz; pl->rot_y = 3072;
    scd_room_reenter(rdt, px, pz, entry_cut);
    s_shown_cut = (int)entry_cut;      /* room_common.c Schritt 5: *cam_active_cut = Eintritts-Cut */
    /* Tuer-Eintritt = Gameplay -> RVD/CAM_SWITCH-Auto-Kamera an (room_common.c Schritt 12,
     * byte-true Cut_auto/DAT_800aca3c Bit 0x100). */
    g_scd.cut_auto_enabled = 1;
    if (re15_cam_present_tick()) s_shown_cut = (int)g_scd.cam_id;
}

/* Ein GANZES Gameplay-Bild, so wie die beiden Hauptschleifen es fahren:
 *   scd_vm_tick -> re15_aot_scan(Spielerposition, ANGEZEIGTER Cut) -> re15_cam_present_tick.
 * Der Scan bekommt den ANGEZEIGTEN Cut (game_step_common.c: `c->active_cut`, gefuellt aus
 * platform/pc/main.c:4348 `gctx.active_cut = active_cut_idx`) — byte-true der cam_from-Filter
 * des Originals, FUN_80014230 @0x8001423c `lbu v0,DAT_800afbb5`.
 * Der Present-Schritt ist der selbstheilende Apply (@0x800214f4/@0x80021bfc) und der EINZIGE
 * Ort, an dem work_vars[0x0A] dem RVD-Zonen-Wechsel nachzieht. */
static void step_at(int32_t x, int32_t z)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = x; pl->z = z;
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, (uint8_t)s_shown_cut);
    if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    if (re15_cam_present_tick()) s_shown_cut = (int)g_scd.cam_id;
}

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT";
    size_t sz = 0;
    uint8_t *raw = read_file(path, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", path); return 77; }

    /* --- Sollseite direkt aus den Rohbytes belegen (kein Vertrauen in den Walker) --- */
    CHECK(raw[0x216E] == 0x2D && raw[0x216F] == 0x00 && raw[0x2170] == 0x04,
          "main00 @0x216E ist kein Obj_model_set obj0 Typ4 (%02X %02X %02X)",
          raw[0x216E], raw[0x216F], raw[0x2170]);
    CHECK(raw[0x2190] == 0x2D && raw[0x2191] == 0x01 && raw[0x2192] == 0x04,
          "main00 @0x2190 ist kein Obj_model_set obj1 Typ4 (%02X %02X %02X)",
          raw[0x2190], raw[0x2191], raw[0x2192]);
    /* sub01 @0x23F4: 06 00 1A 00 | 21 03 80 00 | 06 00 10 00 | 23 00 0A 00 0D 00
     * -> der Cmp beginnt bei 0x23F4 + 0x0C = 0x2400. */
    CHECK(raw[0x2400] == 0x23 && raw[0x2402] == 0x0A && raw[0x2403] == 0x00 &&
          raw[0x2404] == 0x0D && raw[0x2405] == 0x00,
          "sub01 @0x2400 ist kein Cmp(work_vars[0x0A]==13): %02X %02X %02X %02X %02X %02X",
          raw[0x2400], raw[0x2401], raw[0x2402], raw[0x2403], raw[0x2404], raw[0x2405]);

    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    /* =====================================================================
     * (D) Kisten-Hoehe
     * ===================================================================== */
    enter(&rdt, /*entry_cut=*/0, -10100, 4200);

    int p0 = -1, p1 = -1;
    for (int i = 0; i < (int)g_scd.prop_count; i++) {
        if (!g_scd.props[i].active) continue;
        if (g_scd.props[i].obj_id == 0x00) p0 = i;
        if (g_scd.props[i].obj_id == 0x01) p1 = i;
    }
    CHECK(p0 >= 0 && p1 >= 0, "Obj_model_set obj0/obj1 nicht registriert (p0=%d p1=%d)", p0, p1);
    if (p0 >= 0) {
        CHECK(g_scd.props[p0].obj_type == 4, "obj0 Typ=%u != 4 (main00 pc[2]=0x04)",
              g_scd.props[p0].obj_type);
        CHECK(g_scd.props[p0].y == -1800, "obj0 gespeicherte Y=%ld != -1800",
              (long)g_scd.props[p0].y);
        CHECK(g_scd.props[p0].box_hy == 900, "obj0 Box-Halbhoehe=%d != 900",
              g_scd.props[p0].box_hy);
        /* FUN_8002c18c @0x8002c24c: Render-Y = gespeicherte Y - 900. */
        CHECK(re15_prop_render_y(g_scd.props[p0].obj_type, g_scd.props[p0].y) == -2700,
              "obj0 Render-Y=%ld != -2700 (@0x8002c24c addiu -900)",
              (long)re15_prop_render_y(g_scd.props[p0].obj_type, g_scd.props[p0].y));
    }
    if (p1 >= 0) {
        CHECK(g_scd.props[p1].obj_type == 4 && g_scd.props[p1].y == -9000,
              "obj1 Typ=%u Y=%ld (erwartet 4 / -9000)",
              g_scd.props[p1].obj_type, (long)g_scd.props[p1].y);
        CHECK(re15_prop_render_y(g_scd.props[p1].obj_type, g_scd.props[p1].y) == -9900,
              "obj1 Render-Y != -9900");
    }
    /* NEGATIV-TEST: der Versatz gilt NUR fuer Typ 4 (@0x8002c23c `bne v1,v0`).
     * obj3 ist Typ 0 und darf NICHT verschoben werden. */
    for (int i = 0; i < (int)g_scd.prop_count; i++) {
        if (!g_scd.props[i].active || g_scd.props[i].obj_type == 4) continue;
        CHECK(re15_prop_render_y(g_scd.props[i].obj_type, g_scd.props[i].y)
                  == g_scd.props[i].y,
              "Nicht-Typ-4-Prop id=0x%02X (Typ %u) wurde verschoben",
              g_scd.props[i].obj_id, g_scd.props[i].obj_type);
    }

    /* =====================================================================
     * (C) Cutscene-Gate: work_vars[0x0A] MUSS der aktive Kamera-Cut sein
     * ===================================================================== */
    /* 1) Raum-Betreten mit Eintritts-Cut 0 -> sub00 Switch case 0 -> Cut_chg 8. */
    CHECK(g_scd.work_vars[0x0A] == 8,
          "nach Eintritt(Cut 0): work_vars[0x0A]=%d != 8 (sub00 Cut_chg 8, @0x800402fc)",
          (int)g_scd.work_vars[0x0A]);
    CHECK(g_scd.cam_id == 8, "cam_id=%u != 8", g_scd.cam_id);
    CHECK(re15_game_flag_get(3, 0x80) == 0, "flag(3,0x80) darf beim Eintritt 0 sein");

    /* 2) NEGATIV-TEST des Eintritts-Cuts: Eintritt mit Cut 3 (Tuer aus ROOM1050
     *    @0x0B7A) MUSS sub00's `case 3` treffen -> Cut_chg 0x0B statt 8. */
    enter(&rdt, /*entry_cut=*/3, -10100, 4200);
    CHECK(g_scd.work_vars[0x0A] == 0x0B,
          "nach Eintritt(Cut 3): work_vars[0x0A]=%d != 11 (sub00 case 3 -> Cut_chg 0x0B)",
          (int)g_scd.work_vars[0x0A]);

    /* 3) Der Gameplay-Weg: aus Cut 8 heraus ueber die RVD-Zonen bis Cut 13 laufen.
     *    Zonen (RDT @0x28 -> 0x280, 20 B/Eintrag):
     *      @0x044C from  8 -> 11   Quad (-14900,3400)(-14900,8400)(-3597,8400)(-3699,3400)
     *      @0x04D8 from 11 -> 12   Quad (-6100,4000)(-1699,9400)(4301,7301)(-500,3300)
     *      @0x0514 from 12 -> 13   Quad (-4700,-1100)(-4700,4200)(3502,4900)(5201,2301)
     *    Sobald der Cut 13 ist, muss sub01 die Cutscene zuenden. */
    enter(&rdt, /*entry_cut=*/0, -10100, 4200);
    CHECK(g_scd.work_vars[0x0A] == 8, "Vorbedingung Cut 8 verfehlt (%d)",
          (int)g_scd.work_vars[0x0A]);

    static const struct { int32_t x, z; int want_cut; } s_walk[] = {
        { -9000, 5000, 11 },   /* Zone @0x044C  8 -> 11 */
        { -1000, 6000, 12 },   /* Zone @0x04D8 11 -> 12 */
        {  -174, 2575, 13 },   /* Zone @0x0514 12 -> 13 */
    };
    for (int s = 0; s < 3; s++) {
        step_at(s_walk[s].x, s_walk[s].z);
        printf("  Schritt %d @(%ld,%ld): cam_id=%u work_vars[0x0A]=%d work_vars[0x0C]=%d\n",
               s, (long)s_walk[s].x, (long)s_walk[s].z, g_scd.cam_id,
               (int)g_scd.work_vars[0x0A], (int)g_scd.work_vars[0x0C]);
        CHECK((int)g_scd.work_vars[0x0A] == (int)g_scd.cam_id,
              "Schritt %d: work_vars[0x0A]=%d spiegelt cam_id=%u nicht (@0x80021bfc)",
              s, (int)g_scd.work_vars[0x0A], g_scd.cam_id);
        CHECK((int)g_scd.cam_id == s_walk[s].want_cut,
              "Schritt %d: cam_id=%u != %d (RVD-Zone)", s, g_scd.cam_id, s_walk[s].want_cut);
    }

    /* 4) Cut 13 steht an -> sub01 muss GOSUB sub02 feuern und flag(3,0x80) setzen. */
    for (int f = 0; f < 3 && re15_game_flag_get(3, 0x80) == 0; f++) step_at(-174, 2575);
    CHECK(re15_game_flag_get(3, 0x80) == 1,
          "Cutscene-Gate: flag(3,0x80) nicht gesetzt -> sub01 hat sub02 nicht gefeuert");

    /* 5) NEGATIV-TEST des Gates: bei Cut 12 (statt 13) darf NICHTS feuern. */
    enter(&rdt, /*entry_cut=*/0, -10100, 4200);
    step_at(-9000, 5000);      /* -> 11 */
    step_at(-1000, 6000);      /* -> 12 */
    for (int f = 0; f < 5; f++) step_at(-1000, 6000);
    CHECK((int)g_scd.cam_id == 12, "Negativ-Test: cam_id=%u != 12", g_scd.cam_id);
    CHECK(re15_game_flag_get(3, 0x80) == 0,
          "Negativ-Test: Cutscene feuerte schon bei Cut 12 (Cmp muss ==13 verlangen)");

    free(raw);
    printf(g_fail ? "test_1090_room_pin: FAIL\n" : "test_1090_room_pin: OK\n");
    return g_fail;
}
