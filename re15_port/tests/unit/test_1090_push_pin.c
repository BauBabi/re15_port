/* test_1090_push_pin.c — PIN "Kisten schieben" (Nutzer-Report ROOM1090).
 *
 * Gepinnt wird die Kette FUN_8002bd44 (Objekt-Update-Loop, Schiebe-Zweig) zusammen mit dem
 * Spieler-Handshake Substate 8, beides auf den ECHTEN Raumdaten von STAGE1/ROOM1090.RDT und
 * den echten PL00-Baenken:
 *   @0x8002bf14  obj[+0x08] == 4              nur Typ-4-Objekte sind schiebbar
 *   @0x8002bf24  DAT_800ac768 & 1             UP GEHALTEN (virt. Bit 0, Preset @0x80073dbc[0])
 *   @0x8002bf38  DAT_800acad6 == obj[+0x82]   Spieler-Band == Objekt-Band
 *   @0x8002bf50  FUN_8002d1e8(player,obj,0)   Punkt 563 (0x233 @0x8002d204) voraus, Radius 900
 *                                             (0x384 @0x8002d224)
 *   @0x8002bf68/@0x8002bf74  obj[+0x8C]++ bzw. = 0
 *   @0x8002bf90  obj[+0x8C] == 9              die Verzoegerung
 *   @0x8002bfa4  FUN_8002cabc(player,obj,1)   der Schub
 *   @0x8002bfb0  FUN_8003b558(obj,2)          die Gueltigkeitspruefung; Treffer -> = 10 (s7)
 *   @0x8002bfd4  DAT_800aca3c |= 0x2000       das Kontaktbit
 *   @0x8002bfe0/e4 DAT_800aca59 == 8          der Handshake (sonst Ruecksetzung @0x8002c0b8)
 * Spielerseite:
 *   @0x800323c0-cc + @0x8003247c  Gehen -> `sw 0x801` = Zustand 1 / Substate 8
 *   @0x800357a8/c4                Ausstieg, wenn UP ODER Kontakt wegfaellt (nur aus +0x06 == 5)
 *   @0x80035a78                   Ausstieg spielt Clip 0x11 RUECKWAERTS (10 Frames)
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
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_collision.h"

extern scd_vm_t g_scd;

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

static re15_emd_animation_t s_anim;
static re15_emd_skeleton_t  s_skel;
static int load_pl00(const char *base)
{
    char p[600];
    size_t esz = 0, rsz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", base);
    uint8_t *edd = read_file(p, &esz);
    snprintf(p, sizeof p, "%s/PLD/PL00.EMR", base);
    uint8_t *emr = read_file(p, &rsz);
    if (!edd || !emr) return 0;
    if (re15_emd_parse_animation(edd, esz, &s_anim) != 0) return 0;
    if (re15_emd_parse_skeleton (emr, rsz, &s_skel) != 0) return 0;
    return 1;
}

/* Ein Frame in der byte-true Reihenfolge des Originals: Spieler-FSM (@0x8001ce0c) ->
 * Wandklemme -> Objekt-Pass FUN_8002bd44 (@0x8001ce14: Schub @0x8002bfa4 VOR der
 * Spieler-Ausschiebung @0x8002c0d8). Identisch zu game_step_common.c. */
static void one_frame(const re15_rdt_t *rdt, re15_actor_t *pl, uint16_t pad)
{
    int32_t ox = pl->x, oz = pl->z;
    re15_player_tick(NULL, pad);
    int32_t nx = pl->x, nz = pl->z;
    re15_collision_constrain(rdt, ox, oz, &nx, &nz);
    pl->x = nx; pl->z = nz;
    re15_prop_push_tick(rdt, pad);
    re15_collision_objects(&nx, &nz);
    pl->x = nx; pl->z = nz;
}

/* Spieler `back` Einheiten hinter der buendigen Schiebelage (900 Box + 450 Spieler = 1350,
 * die Summe t5 aus FUN_8002cabc @0x8002cb48). yaw 0/1024/2048/3072 = +X/-Z/-X/+Z. */
static void place_before_box(re15_actor_t *pl, int box, int yaw, int32_t back)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    int32_t d = 1350 + back;
    pl->x = g_scd.props[box].x + g_scd.props[box].box_cx - (int32_t)((c * d) >> 12);
    pl->z = g_scd.props[box].z + g_scd.props[box].box_cz - (int32_t)((-s * d) >> 12);
    pl->y = g_scd.props[box].y;
    pl->rot_y = (int16_t)yaw;
    pl->motion = 0; pl->anim_frame = 0; pl->anim_frac = 0; pl->anim_flags = 0;
    re15_collision_set_band((int)g_scd.props[box].band);
    re15_player_push_reset();
    re15_prop_push_reset();
}

int main(void)
{
    const char *base = RE15_ASSET_PSX_DIR;
    if (!load_pl00(base)) { printf("SKIP: PLD/PL00.EDD bzw. PL00.EMR fehlt\n"); return 77; }

    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1090.RDT", base);
    size_t sz = 0;
    uint8_t *raw = read_file(rp, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    re15_aot_init();
    g_current_room_id = 0x1090;
    g_room_change.pending = 0;
    re15_player_set_pl00_banks(&s_skel, &s_anim);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    int box = -1;
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_type == 4 && g_scd.props[i].band == 1) box = i;
    CHECK(box >= 0, "ROOM1090 hat keine Typ-4-Kiste im Band 1 (RDT main00 @0x216E: Typ 4, Band 1)");
    if (box < 0) return 1;
    const int32_t BX = g_scd.props[box].x, BZ = g_scd.props[box].z;
    /* Datenlage: die RDT-Kiste ist Typ 4 mit Box c=(0,-900,0) h=(900,900,900). */
    CHECK(g_scd.props[box].box_hx == 900 && g_scd.props[box].box_hz == 900,
          "Kistenbox nicht 900x900 (h=%d,%d)", g_scd.props[box].box_hx, g_scd.props[box].box_hz);

    /* ---- P0: die Schiebe-Clips 0x11/0x12 und ihr 0x2000-Muster (Sub-Update-8-Daten) ---- */
    CHECK(s_anim.clip_count > 0x12, "PL00.EDD hat keinen Clip 0x12");
    CHECK(s_anim.clips[0x11].frame_count == 10, "Clip 0x11 hat %d statt 10 Frames",
          s_anim.clips[0x11].frame_count);
    CHECK(s_anim.clips[0x12].frame_count == 25, "Clip 0x12 hat %d statt 25 Frames",
          s_anim.clips[0x12].frame_count);
    {   /* @0x80035a5c gated die Wurzelbewegung auf EDD-Bit 0x2000 — im Clip 0x12 sind das
         * genau die Frames 0..14 (die Schub-Haelfte); 15..24 sind die Erholung. */
        int first_clear = -1, n2000 = 0;
        for (int i = 0; i < 25; i++) {
            uint32_t e = re15_emd_get_frame_entry(&s_anim, 0x12, i);
            if (e & 0x2000u) n2000++;
            else if (first_clear < 0) first_clear = i;
        }
        CHECK(n2000 == 15 && first_clear == 15,
              "Clip 0x12 Bit-0x2000-Muster: %d Frames gesetzt, erster freier %d (erwartet 15/15)",
              n2000, first_clear);
    }

    /* ---- P1: die Kette feuert. Richtung +X — der EINZIGE Schiebe-Standort dieser Kiste,
     * auf dem der Spieler frei steht (die drei anderen liegen in SCA-Zellen; geprueft mit
     * re15_collision_box_blocked(Radius 450, Maske 1) in probe_1090_push). ---- */
    place_before_box(pl, box, 0, 0);
    int first_move = -1, contact_frame = -1, sub8_frame = -1, cnt_at_first_move = -1;
    int32_t prev_x = g_scd.props[box].x;
    int32_t step[8]; int nstep = 0;
    for (int f = 0; f < 60; f++) {
        one_frame(&rdt, pl, RE15_PAD_BIT_UP);
        if (contact_frame < 0 && re15_prop_push_contact()) contact_frame = f;
        if (sub8_frame   < 0 && re15_player_push_substate()) sub8_frame = f;
        if (g_scd.props[box].x != prev_x) {
            if (first_move < 0) { first_move = f; cnt_at_first_move = re15_prop_push_counter(box); }
            if (nstep < 8) step[nstep++] = g_scd.props[box].x - prev_x;
        }
        prev_x = g_scd.props[box].x;
    }
    CHECK(contact_frame == 8, "Kontaktbit 0x2000 erst bei Bild %d (Zaehler 1..9 = Bilder 0..8)",
          contact_frame);
    CHECK(sub8_frame == 9, "Substate 8 erst bei Bild %d (1 Bild nach dem Kontakt: der Spieler-FSM "
          "@0x8001ce0c laeuft VOR dem Objekt-Pass @0x8001ce14)", sub8_frame);
    CHECK(first_move > 0, "die Kiste hat sich in 60 Bildern NIE bewegt (der Nutzer-Befund)");
    CHECK(g_scd.props[box].x > BX, "die Kiste ist nicht nach +X gewandert (%ld -> %ld)",
          (long)BX, (long)g_scd.props[box].x);
    CHECK(g_scd.props[box].z == BZ, "die Kiste ist quer ausgewichen (z %ld -> %ld)",
          (long)BZ, (long)g_scd.props[box].z);
    /* Der Schub laeuft NUR waehrend Clip 0x12 (Substate-Phase 5) — Clip 0x11 (10 Frames) ist
     * der Anlauf, der Wechsel auf 0x12 passiert @0x800359e4 nach dessen Ende. */
    CHECK(first_move >= 20, "die Kiste bewegte sich schon bei Bild %d, also VOR dem Ende von "
          "Clip 0x11 (@0x80035958) und dem Wechsel auf Clip 0x12 (@0x800359e4)", first_move);
    CHECK(cnt_at_first_move == 8, "Zaehler beim ersten Schub = %d (nach dem Treffer setzt "
          "@0x8002bfc0 auf 8)", cnt_at_first_move);
    /* Der Schub-BETRAG je Bild ist die Wurzel-Translation des Clips 0x12 (FUN_800369f8 Modus 0
     * @0x80036aac-ae8), also die Differenz der EMR-Keyframe-Wurzel zwischen zwei Clip-Frames.
     * Gegen die Assetbytes gerechnet, nicht gegen eine Wunschzahl. */
    for (int i = 0; i < nstep && i < 4; i++) {
        int kf_n = (int)(re15_emd_get_frame_entry(&s_anim, 0x12, i + 1) & 0xFFFu);
        int kf_p = (int)(re15_emd_get_frame_entry(&s_anim, 0x12, i    ) & 0xFFFu);
        int16_t sx, sy, sz, sx0, sz0;
        re15_emd_get_keyframe_speed(&s_skel, kf_n, &sx,  &sy, &sz);
        re15_emd_get_keyframe_speed(&s_skel, kf_p, &sx0, &sy, &sz0);
        CHECK(step[i] == (int32_t)sx - sx0,
              "Schub %d = %+ld, Clip-0x12-Wurzeldelta Frame %d->%d = %+d",
              i, (long)step[i], i, i + 1, (int)sx - sx0);
    }
    /* Nach 4 Schueben steht die Kiste an der SCA-Zelle x>=-7133 (Band 1, u0=0xFF): die
     * Validierung FUN_8003b558(obj,2) greift, @0x8002c0b0 setzt den Zaehler auf 10, und der
     * zaehlt danach WEITER hoch — er wird nie wieder 9 (byte-true: kein Reset am Trefferpfad). */
    CHECK(re15_prop_push_counter(box) > 10,
          "Zaehler nach dem Wandtreffer %d (erwartet > 10)", re15_prop_push_counter(box));

    g_scd.props[box].x = BX; g_scd.props[box].z = BZ;

    /* ---- P2: Ausstieg — mitten im Schieben UP loslassen ---- */
    place_before_box(pl, box, 0, 0);
    for (int f = 0; f < 24; f++) one_frame(&rdt, pl, RE15_PAD_BIT_UP);
    CHECK(re15_player_push_phase() == 5,
          "vor dem Loslassen Phase %d statt 5 (Clip 0x12 laeuft)", re15_player_push_phase());
    int exit_f = -1;
    for (int f = 0; f < 40 && exit_f < 0; f++) {
        one_frame(&rdt, pl, 0);
        if (!re15_player_push_substate()) exit_f = f;
    }
    CHECK(exit_f == 10, "Substate 8 nach %d Bildern verlassen (Clip 0x11 rueckwaerts = 10 Bilder, "
          "@0x80035a78 -> [7] @0x80035ab4)", exit_f);
    CHECK(re15_prop_push_counter(box) == 0, "Zaehler nach dem Loslassen %d statt 0 (@0x8002bf74)",
          re15_prop_push_counter(box));
    CHECK(!re15_prop_push_contact(), "Kontaktbit steht nach dem Loslassen noch (@0x8002bd94)");
    g_scd.props[box].x = BX; g_scd.props[box].z = BZ;

    /* ---- P3..P6: Negativproben, je EIN Tor verletzt ---- */
    struct { const char *name; int hold_up; int type; int band; int32_t back; } neg[] = {
        { "ohne UP (@0x8002bf24)",              0, 4, 1,    0 },
        { "Typ 0 statt 4 (@0x8002bf14)",        1, 0, 1,    0 },
        { "falsches Band (@0x8002bf38)",        1, 4, 3,    0 },
        { "ausser Reichweite (@0x8002bf50)",    1, 4, 1, 4000 },
    };
    for (unsigned k = 0; k < sizeof neg / sizeof neg[0]; k++) {
        uint8_t t0 = g_scd.props[box].obj_type, b0 = g_scd.props[box].band;
        place_before_box(pl, box, 0, neg[k].back);
        g_scd.props[box].obj_type = (uint8_t)neg[k].type;
        g_scd.props[box].band     = (uint8_t)neg[k].band;
        re15_collision_set_band((int)b0);           /* Spieler bleibt im Original-Band */
        for (int f = 0; f < 60; f++) one_frame(&rdt, pl, neg[k].hold_up ? RE15_PAD_BIT_UP : 0);
        CHECK(g_scd.props[box].x == BX && g_scd.props[box].z == BZ,
              "%s: Kiste trotzdem verschoben auf (%ld,%ld)", neg[k].name,
              (long)g_scd.props[box].x, (long)g_scd.props[box].z);
        CHECK(!re15_player_push_substate(), "%s: Spieler steht trotzdem im Substate 8", neg[k].name);
        g_scd.props[box].obj_type = t0; g_scd.props[box].band = b0;
        g_scd.props[box].x = BX; g_scd.props[box].z = BZ;
    }

    /* ---- P7: die Verzoegerung — vor dem 9. Bild darf NICHTS passieren ---- */
    place_before_box(pl, box, 0, 0);
    for (int f = 0; f < 8; f++) {
        one_frame(&rdt, pl, RE15_PAD_BIT_UP);
        CHECK(re15_prop_push_counter(box) == f + 1,
              "Zaehler bei Bild %d = %d statt %d (@0x8002bf68)", f,
              re15_prop_push_counter(box), f + 1);
        CHECK(!re15_prop_push_contact(), "Kontaktbit schon bei Zaehler %d (@0x8002bf90 verlangt 9)",
              f + 1);
        CHECK(g_scd.props[box].x == BX, "Kiste schon bei Zaehler %d bewegt", f + 1);
    }
    g_scd.props[box].x = BX; g_scd.props[box].z = BZ;

    /* ---- P8: die Gueltigkeitspruefung FUN_8003b558(obj,2) haelt die Kiste vor der Wand ----
     * Richtung -Z (Standort (-8136,6103), ebenfalls frei) laeuft die Kiste sofort in die
     * SCA-Zelle Band 1 z=[974..3800] (u0=0xFF, um 900 aufgeblasen -> z <= 4700, die Kiste
     * steht auf 4753). Der Treffer setzt den Zaehler auf 10 (@0x8002c0b0), der danach WEITER
     * hochzaehlt und nie wieder 9 wird — byte-true, kein Reset auf dem Trefferpfad. */
    place_before_box(pl, box, 1024, 0);
    for (int f = 0; f < 60; f++) one_frame(&rdt, pl, RE15_PAD_BIT_UP);
    CHECK(g_scd.props[box].x == BX && g_scd.props[box].z == BZ,
          "Kiste in die Wand geschoben: (%ld,%ld)", (long)g_scd.props[box].x,
          (long)g_scd.props[box].z);
    CHECK(re15_prop_push_counter(box) > 10,
          "Zaehler nach dem Wandtreffer %d (erwartet > 10: @0x8002c0b0 setzt 10, dann ++)",
          re15_prop_push_counter(box));
    g_scd.props[box].x = BX; g_scd.props[box].z = BZ;

    /* ---- P9: REGRESSIONSWACHE — ohne Typ-4-Prop in Reichweite laeuft der normale Gang ---- */
    {
        uint8_t saved[16];
        for (int i = 0; i < (int)g_scd.prop_count && i < 16; i++) {
            saved[i] = g_scd.props[i].obj_type;
            g_scd.props[i].obj_type = 0;
        }
        place_before_box(pl, box, 0, 6000);
        int32_t x0 = pl->x;
        for (int f = 0; f < 10; f++) one_frame(&rdt, pl, RE15_PAD_BIT_UP);
        CHECK(!re15_player_push_substate(), "Substate 8 ohne Typ-4-Prop");
        CHECK(pl->x - x0 >= 700, "der normale Vorwaertsgang ist kaputt: %+ld in 10 Bildern "
              "(75/Bild erwartet)", (long)(pl->x - x0));
        CHECK(pl->motion == 105, "Laufclip %d statt 105", (int)pl->motion);
        for (int i = 0; i < (int)g_scd.prop_count && i < 16; i++)
            g_scd.props[i].obj_type = saved[i];
    }

    free(raw);
    printf(g_fail ? "PIN 1090-push: FEHLGESCHLAGEN\n" : "PIN 1090-push: OK\n");
    return g_fail;
}
