/*
 * probe_r34_schaden.c — Runde 34 (Granaten), Spur B: Schaden, Resolver-Tore, RE2-Stempel,
 * RE2-GL-Applier, Trefferkasten-Versatz.
 *
 * Dossier: analysis/befunde_runde34_granaten/bau_b.md (Bau), BAUPLAN.md §3.2 (Abnahme Spur B).
 * Rueckgabe 0 = gruen, sonst die NUMMER der ersten fehlgeschlagenen Pruefung (jede Pruefung
 * traegt ihre Nummer im Text). Jede tragende Pruefung hat eine Negativ-Kontrolle; die
 * Mutationsproben sind im Dossier protokolliert.
 *
 * Aufruf: probe_r34_schaden [teil]   teil = tore | stempel | applier | kasten | alle (Vorgabe)
 */
#include "re15_damage.h"
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_math.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int s_first_fail = 0;
static int s_fails = 0;
#define CHECK(nr, c, ...) do { if (!(c)) { printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); \
    s_fails++; if (!s_first_fail) s_first_fail = (nr); } else { printf("ok   %d\n", (nr)); } } while (0)

/* ---- Aufbau ------------------------------------------------------------------------------ */
static re15_actor_t *pl_far(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->hp = 100; pl->state = 1; pl->hit_react = 0;
    pl->x = 100000; pl->y = 0; pl->z = 100000; pl->rot_y = 0;
    re15_player_apply_hitbox(pl);
    return pl;
}

static re15_actor_t *mk(int slot, uint8_t type, int32_t x, int32_t y, int32_t z, int16_t hp)
{
    re15_actor_t *e = &g_actors[slot];
    e->active = 1; e->type = type; e->x = x; e->y = y; e->z = z; e->hp = hp;
    e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hit_react = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, type);
    return e;
}

static re15_attack_box_t box_at(int32_t x, int32_t y, int32_t z)
{
    re15_attack_box_t b; b.x = x; b.y = y; b.z = z; b.radius = 500;   /* @0x80018598 ori a0,zero,0x1f4 */
    return b;
}

/* =========================================================================================
 * TEIL "tore" — B1: Gate B (@0x80012f54-60), NPC-Ausschluss (E7), Ivy/FX immun.
 * ========================================================================================= */
static void teil_tore(void)
{
    printf("== tore (B1)\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);

    /* (11) GATE B: +0x93 & 3 == 3 -> uebersprungen, NICHT gezaehlt, +0x93 unberuehrt. */
    re15_actor_init(); pl_far();
    re15_actor_t *e = mk(1, 0x27, 300, 0, 0, 180);
    e->hit_react = 0x83;
    re15_attack_box_t b = box_at(0, -500, 0);
    int n = re15_resolve_attack(&b, 2, -1);
    CHECK(11, e->hp == 180 && e->hit_react == 0x83 && n == 0,
          "Gate B: hp=%d (180) hit_react=0x%02X (0x83) n=%d (0)", e->hp, e->hit_react, n);

    /* (18) GATE B laesst auch Bit 0x80 stehen: mit +0x93 = 0x03 und P HINTER der Figur (der
     *      Seitentest FUN_8001a7a8 wuerde 0x80 setzen, s. Pruefung 12/13) bleibt es 0x03. */
    re15_actor_init(); pl_far();
    e = mk(1, 0x27, 300, 0, 0, 180);
    e->hit_react = 0x03;
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(18, e->hp == 180 && e->hit_react == 0x03 && n == 0,
          "Gate B / Bit 0x80: hp=%d hit_react=0x%02X (0x03) n=%d (0)", e->hp, e->hit_react, n);

    /* (12) NEGATIV-KONTROLLE zu 11: nur Bit 0 -> Kandidat, |= 2 ohne Schaden, GEZAEHLT. */
    re15_actor_init(); pl_far();
    e = mk(1, 0x27, 300, 0, 0, 180);
    e->hit_react = 0x01;
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(12, e->hp == 180 && (e->hit_react & 3u) == 3u && n == 1,
          "Bit-0-Riegel: hp=%d (180) hit_react=0x%02X (&3==3) n=%d (1)", e->hp, e->hit_react, n);

    /* (13) NEGATIV-KONTROLLE: frei -> 1000 Schaden (@0x8006f41c), +0x5 = 9 (@0x8006f432). */
    re15_actor_init(); pl_far();
    e = mk(1, 0x27, 300, 0, 0, 180);
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(13, e->hp == 180 - 1000 && e->state == 3 && e->sub_state_1 == 9 && e->sub_state_2 == 1 && n == 1,
          "freier Gegner: hp=%d state=%d +5=%d +6=%d n=%d", e->hp, e->state, e->sub_state_1,
          e->sub_state_2, n);

    /* (14) NPC 0x45 mit HP -1 300 neben P -> kein Kandidat (E7). */
    re15_actor_init(); pl_far();
    e = mk(1, 0x45, 300, 0, 0, -1);
    uint8_t st0 = e->state;
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(14, e->hp == -1 && e->state == st0 && e->hit_react == 0 && n == 0,
          "NPC 0x45 HP -1: hp=%d state=%d (%d) hit_react=0x%02X n=%d", e->hp, e->state, st0,
          e->hit_react, n);

    /* (15) NEGATIV-KONTROLLE zu 14: derselbe Kasten mit HP >= 0 wird getroffen (der Kasten
     *      allein schliesst also NICHT aus — es ist die HP-Kennung). */
    re15_actor_init(); pl_far();
    e = mk(1, 0x45, 300, 0, 0, 0);
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(15, e->state == 3 && n == 1, "NPC 0x45 HP 0: state=%d (3) n=%d (1)", e->state, n);

    /* (16) Ivy 0x2d: INIT +0x93 = 3 (@0x8011693c/44 STAGE4) -> Gate B, immun. */
    re15_actor_init(); pl_far();
    e = mk(1, 0x2d, 300, 0, 0, 100);
    e->hit_react = 3;
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(16, e->hp == 100 && e->hit_react == 3 && e->state == 1 && n == 0,
          "Ivy: hp=%d hit_react=0x%02X state=%d n=%d", e->hp, e->hit_react, e->state, n);

    /* (17) FX-Emitter 0x24: +0x93 = 1 (@0x8010ef58 STAGE2) -> 1. Treffer nur |= 2, kein Schaden;
     *      2. Treffer: Gate B. */
    re15_actor_init(); pl_far();
    e = mk(1, 0x24, 300, 0, 0, 100);
    e->hit_react = 1;
    int n1 = re15_resolve_attack(&b, 2, -1);
    uint8_t hr1 = e->hit_react;
    int n2 = re15_resolve_attack(&b, 2, -1);
    CHECK(17, e->hp == 100 && e->state == 1 && (hr1 & 3u) == 3u && n1 == 1 && n2 == 0 &&
              e->hit_react == hr1,
          "FX 0x24: hp=%d state=%d hr1=0x%02X n1=%d n2=%d hr=0x%02X", e->hp, e->state, hr1,
          n1, n2, e->hit_react);
}

/* =========================================================================================
 * TEIL "applier" — B4: re15_re2_gl_apply = FUN_800470C0-Zwilling (RE2-Flavor).
 * Aufruf wie Op 40 (@0x80020768-bc): Box {-600,0,300,150} (@0x80010910), Hitcode 0x2002000A.
 * ========================================================================================= */
static const int16_t k_box_op40[4] = { -600, 0, 300, 150 };   /* @0x80010910 `a8 fd 00 00 2c 01 96 00` */
#define HIT_OP40 0x2002000Au                                    /* lui a3,0x2002 / ori a3,a3,0xa @0x80020794/a0 */

static re15_actor_t *mk_re2z(int slot, uint8_t type, int32_t x, int32_t y, int32_t z, int16_t hp)
{
    re15_actor_t *e = mk(slot, type, x, y, z, hp);
    e->re2z_self1d3 = 0; e->re2z_f10e = 0; e->re2z_hitdir1d0 = 0; e->re2z_hits1d2 = 1;
    e->re2_hit_box_set = 1; e->re2_hit_b98 = -1500; e->re2_hit_h9e = 1500;  /* Steh-Box @0x8010095C-64 */
    e->re2z_pool151 = e->re2z_pool152 = e->re2z_pool153 = 13;              /* @0x8010081C */
    return e;
}

static void teil_applier(void)
{
    printf("== applier (B4)\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    int32_t P[3] = { 0, -100, 0 };                                         /* Flamme y - 100 @0x800207a4 */

    /* (21) Zombie Zeile 10 Klammer 2: HP 80 - 5 (w0 0x0050C8C8 >> 20 @0x800A41E0), Zustand 2,
     *      +0x5 = 10, +0x1D2 = Zone 0 + 3*2 = 6 (Y + (-1500>>1) = -750 < P.y = -100 -> Beine),
     *      +0x1D3 = 15 (w1 0x078F1E0A >> 9 & 0x7F), +0x1D0 Bit 0, +0x6 = 0, Rueckgabe Slot+1. */
    re15_actor_init(); pl_far();
    re15_actor_t *a = mk_re2z(1, 0x10, 0, 0, 0, 80);
    int r = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40);
    CHECK(21, r == 2 && a->hp == 75 && a->state == 2 && a->sub_state_1 == 10 && a->sub_state_2 == 0 &&
              a->re2z_hits1d2 == 6 && a->re2z_self1d3 == 15 && (a->re2z_hitdir1d0 & 1u),
          "Z10/K2: r=%d hp=%d st=%d +5=%d +6=%d 1D2=%d 1D3=%d 1D0=0x%04X", r, a->hp, a->state,
          a->sub_state_1, a->sub_state_2, a->re2z_hits1d2, a->re2z_self1d3, a->re2z_hitdir1d0);
    /* (22) Zonen-Reserve unberuehrt (der GL-Applier schreibt +0x151..0x153 nicht). */
    CHECK(22, a->re2z_pool151 == 13 && a->re2z_pool152 == 13 && a->re2z_pool153 == 13 && a->re2_gl_stamp == 1,
          "Reserve %d/%d/%d (13), gl_stamp=%d", a->re2z_pool151, a->re2z_pool152, a->re2z_pool153,
          a->re2_gl_stamp);

    /* (23) Nur der ERSTE ungesperrte Gegner (Hitcode-Bit 0x10000 = 0): zwei Zombies im Kasten. */
    re15_actor_init(); pl_far();
    a = mk_re2z(1, 0x10, 0, 0, 0, 80);
    re15_actor_t *b = mk_re2z(2, 0x10, 200, 0, 0, 80);
    r = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40);
    CHECK(23, r == 2 && a->hp == 75 && b->hp == 80 && b->state == 1,
          "nur der erste: r=%d a.hp=%d b.hp=%d b.st=%d", r, a->hp, b->hp, b->state);

    /* (24) Gate 2: +0x1D3 != 0 -> kein Kandidat, der naechste wird getroffen. */
    re15_actor_init(); pl_far();
    a = mk_re2z(1, 0x10, 0, 0, 0, 80); a->re2z_self1d3 = 5;
    b = mk_re2z(2, 0x10, 200, 0, 0, 80);
    r = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40);
    CHECK(24, r == 3 && a->hp == 80 && a->re2z_self1d3 == 5 && b->hp == 75,
          "Gate 2: r=%d a.hp=%d a.1D3=%d b.hp=%d", r, a->hp, a->re2z_self1d3, b->hp);

    /* (25) Gate 3 (HP < 0) und Gate 4 (+0x10E & 0xC000): kein Kandidat. */
    re15_actor_init(); pl_far();
    a = mk_re2z(1, 0x10, 0, 0, 0, -1);
    b = mk_re2z(2, 0x10, 200, 0, 0, 80); b->re2z_f10e = 0x4000;
    r = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40);
    CHECK(25, r == 0 && a->hp == -1 && b->hp == 80, "Gates 3/4: r=%d a.hp=%d b.hp=%d", r, a->hp, b->hp);

    /* (26) Leerliste (0x800CFBF3 == 0 @0x8004710c): nur der Spieler -> 0. */
    re15_actor_init(); pl_far();
    r = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40);
    CHECK(26, r == 0, "Leerliste: r=%d", r);

    /* (27) Band (@0x8004716c-a4): P.y in (Y+b-d-100, Y+b+d+100] = (-3100, 100]. Grenzen +-1. */
    {
        int ok = 1; int got[4]; const int32_t ys[4] = { 100, 101, -3099, -3100 }; const int soll[4] = { 1, 0, 1, 0 };
        for (int i = 0; i < 4; i++) {
            re15_actor_init(); pl_far();
            a = mk_re2z(1, 0x10, 0, 0, 0, 80);
            int32_t Q[3] = { 0, ys[i], 0 };
            got[i] = re15_re2_gl_apply(Q, 0, k_box_op40, HIT_OP40) != 0;
            if (got[i] != soll[i]) ok = 0;
        }
        CHECK(27, ok, "Band P.y 100/101/-3099/-3100 -> %d/%d/%d/%d (1/0/1/0)", got[0], got[1], got[2], got[3]);
    }

    /* (28) Kasten FUN_80041EF8 mit Radius 500 (+0x1EE @0x80100980): Gier 0 -> x in [-600, 1100),
     *      z in [-1100, 1100) (Viertel-Raster, Kanten e1 = 4*(300+125), e2 = 8*(150+125)).
     *      Grenzen +-1 in beiden Achsen. */
    {
        const int32_t xs[4] = { 1099, 1100, -600, -601 }; const int32_t zs[4] = { 1099, 1100, -1100, -1101 };
        const int soll[4] = { 1, 0, 1, 0 };
        int gx[4], gz[4], ok = 1;
        for (int i = 0; i < 4; i++) {
            re15_actor_init(); pl_far();
            a = mk_re2z(1, 0x10, xs[i], 0, 0, 80);
            gx[i] = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40) != 0;
            re15_actor_init(); pl_far();
            a = mk_re2z(1, 0x10, 0, 0, zs[i], 80);
            gz[i] = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40) != 0;
            if (gx[i] != soll[i] || gz[i] != soll[i]) ok = 0;
        }
        CHECK(28, ok, "Kasten x 1099/1100/-600/-601 -> %d/%d/%d/%d, z 1099/1100/-1100/-1101 -> %d/%d/%d/%d (1/0/1/0)",
              gx[0], gx[1], gx[2], gx[3], gz[0], gz[1], gz[2], gz[3]);
    }

    /* (29) Gier dreht den Kasten: Gier 1024 (RotMatrix @0x8008e1f4 = [[0,0,4096],[0,4096,0],
     *      [-4096,0,0]]: lokal (x,z) -> Welt (z,-x)) -> Welt X in [-1100,1100), Z in (-1100,600].
     *      (-1000,0) liegt dann drin (ungedreht: x < -600 draussen), (0,700) draussen (ungedreht drin). */
    {
        int r1[2], r0[2]; const int32_t xs[2] = { -1000, 0 }, zs[2] = { 0, 700 };
        for (int i = 0; i < 2; i++) {
            re15_actor_init(); pl_far();
            a = mk_re2z(1, 0x10, xs[i], 0, zs[i], 80);
            r1[i] = re15_re2_gl_apply(P, 1024, k_box_op40, HIT_OP40) != 0;
            re15_actor_init(); pl_far();
            a = mk_re2z(1, 0x10, xs[i], 0, zs[i], 80);
            r0[i] = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40) != 0;
        }
        CHECK(29, r1[0] == 1 && r1[1] == 0 && r0[0] == 0 && r0[1] == 1,
              "Gier 1024: (-1000,0) -> %d (1), (0,700) -> %d (0); Gier 0: %d (0), %d (1)",
              r1[0], r1[1], r0[0], r0[1]);
    }

    /* (91) Radius-Erweiterung bleibt nach einem Treffer im Puffer (@0x800471f0 nur Nicht-Treffer
     *      nimmt zurueck): Modus ALLE (0x10000) -> der zweite Kandidat sieht p2/p3 doppelt
     *      erweitert. B bei x = 1500 liegt ausserhalb [-600,1100) (einfach), innerhalb [-600,1600)
     *      (doppelt). Kontrolle: ohne A kein Treffer an B; Einzelmodus: A getroffen, B nie geprueft. */
    {
        re15_actor_init(); pl_far();
        a = mk_re2z(1, 0x10, 0, 0, 0, 80);
        b = mk_re2z(2, 0x10, 1500, 0, 0, 80);
        int r_alle = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40 | 0x10000u);
        int b_alle = (b->hp == 75);
        re15_actor_init(); pl_far();
        b = mk_re2z(2, 0x10, 1500, 0, 0, 80);
        int r_ohneA = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40 | 0x10000u);
        re15_actor_init(); pl_far();
        a = mk_re2z(1, 0x10, 0, 0, 0, 80);
        b = mk_re2z(2, 0x10, 1500, 0, 0, 80);
        int r_einzel = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40);
        CHECK(91, r_alle == 3 && b_alle && r_ohneA == 0 && r_einzel == 2 && b->hp == 80,
              "Puffer: alle r=%d B getroffen=%d; ohne A r=%d (0); einzeln r=%d (2) B.hp=%d (80)",
              r_alle, b_alle, r_ohneA, r_einzel, b->hp);
    }

    /* (92) Richtung aus P (@0x80047350-3d8): Gier 0, P hinter der Figur in -z (dz > 0 -> 3072)
     *      -> a = 3072: 0x20|0x40 (+Bit 0) = 0x61; P in +z (dz < 0 -> 1024): 0x80 -> 0x81. */
    {
        re15_actor_init(); pl_far();
        a = mk_re2z(1, 0x10, 0, 0, 0, 80);
        int32_t Qa[3] = { 0, -100, -500 };
        re15_re2_gl_apply(Qa, 0, k_box_op40, HIT_OP40);
        uint16_t d1 = a->re2z_hitdir1d0;
        re15_actor_init(); pl_far();
        a = mk_re2z(1, 0x10, 0, 0, 0, 80);
        int32_t Qb[3] = { 0, -100, 500 };
        re15_re2_gl_apply(Qb, 0, k_box_op40, HIT_OP40);
        uint16_t d2 = a->re2z_hitdir1d0;
        CHECK(92, (d1 & 0xffu) == 0x61u && (d2 & 0xffu) == 0x81u,
              "Richtung: P -z -> 0x%02X (0x61), P +z -> 0x%02X (0x81)", d1 & 0xffu, d2 & 0xffu);
    }

    /* (93) RE1.5-KI-Kandidat (O-VB4): Made 0x27 (RE1.5-Gehirn in jedem Flavor) -> Art 5:
     *      HP 180 - 50 (@0x8006f422), +0x5 = 14 (@0x8006f435), +0x6 = 1, +0x93 Bit 0. */
    re15_actor_init(); pl_far();
    a = mk(1, 0x27, 0, 0, 0, 180);
    r = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40);
    CHECK(93, r == 2 && a->hp == 130 && a->state == 2 && a->sub_state_1 == 14 && a->sub_state_2 == 1 &&
              (a->hit_react & 1u),
          "Made Art 5: r=%d hp=%d st=%d +5=%d +6=%d hr=0x%02X", r, a->hp, a->state, a->sub_state_1,
          a->sub_state_2, a->hit_react);
    /* (94) RE1.5-Kandidat mit Gate B (+0x93 & 3 == 3): kein Treffer, der naechste wird getroffen. */
    re15_actor_init(); pl_far();
    a = mk(1, 0x27, 0, 0, 0, 180); a->hit_react = 3;
    b = mk_re2z(2, 0x10, 200, 0, 0, 80);
    r = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40);
    CHECK(94, r == 3 && a->hp == 180 && a->hit_react == 3 && b->hp == 75,
          "Gate B im Applier: r=%d a.hp=%d a.hr=0x%02X b.hp=%d", r, a->hp, a->hit_react, b->hp);

    /* (82) +0x1D0 &= 0xFF00 JE KANDIDAT, VOR Band und Kasten (`lhu v1,464(s0)` @0x8004716c /
     *      `andi v1,v1,0xff00` @0x80047178 / `sh v1,464(s0)` @0x80047184): A (Slot 1, Gates 1-4
     *      frei, im Band, aber ausserhalb des Kastens x 3000) vorbelegt 0x34C0 -> 0x3400; B (Slot 2,
     *      im Kasten) vorbelegt 0x56E0, P in -z -> 0x5661 (Richtung 0x61 wie 92, Bit 0 @0x80047200).
     *      Ohne das Loeschen: A bliebe 0x34C0, B wuerde 0x56E1. */
    {
        re15_actor_init(); pl_far();
        a = mk_re2z(1, 0x10, 3000, 0, 0, 80); a->re2z_hitdir1d0 = 0x34C0u;
        b = mk_re2z(2, 0x10, 0, 0, 0, 80);    b->re2z_hitdir1d0 = 0x56E0u;
        int32_t Qa[3] = { 0, -100, -500 };
        r = re15_re2_gl_apply(Qa, 0, k_box_op40, HIT_OP40);
        CHECK(82, r == 3 && a->hp == 80 && a->re2z_hitdir1d0 == 0x3400u && b->hp == 75 &&
                  b->re2z_hitdir1d0 == 0x5661u,
              "1D0 je Kandidat: r=%d A.hp=%d A.1D0=0x%04X (0x3400) B.hp=%d B.1D0=0x%04X (0x5661)", r, a->hp,
              a->re2z_hitdir1d0, b->hp, b->re2z_hitdir1d0);
    }

    /* ---- NACHBESSERUNG M1: Bodenfeuer an einem RE1.5-KI-Zombie MIT Import-Option (Vorgabe im
     *      RE1.5-Flavor, re15_re15_re2z_import). Er steht unter dem RE2-Modell (RE2-HP) -> der
     *      RE2-Record-Wert desselben Hitcodes: Z10 w0 0x0050C8C8 @0x800A41E0, K2 = (w0 >> 20) &
     *      0x3FF (`srlv` @0x80047254 / `andi` @0x8004725c) = 5; Reaktion bleibt O-VB4 (+0x5 = 14
     *      @0x8006f435, +0x6 = 1). Die Import-Bruecke stempelt wie FUN_800470C0: KEINE Reserve. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re15_re2z_import_set(1);
    /* (95) ein Treffer: HP 80 -> 75, +0x5 14, +0x6 1, Zustand 2; Bruecken-Stempel: +0x1D2 = Zone 0 +
     *      3*2 = 6 (Y + (-1440 >> 1) = -720 < P.y = -100), +0x1D0 = Bit 0 | Richtung aus P. */
    re15_actor_init(); pl_far();
    a = mk(1, 0x10, 0, 0, 0, 80);
    r = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40);
    CHECK(95, r == 2 && a->hp == 75 && a->sub_state_1 == 14 && a->sub_state_2 == 1 && a->state == 2 &&
              a->re2z_hits1d2 == 6 && (a->re2z_hitdir1d0 & 1u),
          "Import-Zombie Bodenfeuer: r=%d hp=%d (75) +5=%d (14) +6=%d st=%d 1D2=%d (6) 1D0=0x%04X", r, a->hp,
          a->sub_state_1, a->sub_state_2, a->state, a->re2z_hits1d2, a->re2z_hitdir1d0);
    /* (96) zwoelf seitliche Flammen (P.z +-300, wie Gegenpruefung mess3; Riegel/Zustand zwischen den
     *      Treffern zurueck = Ende der HURT-Reaktion): je 5 Schaden, die Mitte-Reserve +0x152 bleibt
     *      13, KEIN Bein (+0x21A & 0x60 == 0). NEGATIV-KONTROLLE im selben Aktor: ein Pistolentreffer
     *      ueber die Hitscan-Bruecke (Waffe 3 -> Zeile 3, w1 0x02851014 & 7 = 4, @0x80041954-70)
     *      zieht ab: 13 -> 9. */
    {
        re15_actor_init(); pl_far();
        a = mk(1, 0x10, 0, 0, 0, 2000);
        int dmg_ok = 1, pool_min = 99;
        for (int i = 0; i < 12; i++) {
            int32_t Q[3] = { 0, -100, (i & 1) ? 300 : -300 };
            int16_t h0 = a->hp;
            re15_re2_gl_apply(Q, 0, k_box_op40, HIT_OP40);
            if (h0 - a->hp != 5) dmg_ok = 0;
            if (a->re2z_pool152 < pool_min) pool_min = a->re2z_pool152;
            a->hit_react = 0; a->state = 1; a->sub_state_1 = 0; a->sub_state_2 = 0; a->sub_state_3 = 0;
        }
        const int bein = (a->re2z_flags21a & 0x60u) != 0u;
        const int p151 = a->re2z_pool151, p152 = a->re2z_pool152, p153 = a->re2z_pool153;
        re15_re15_re2z_gore_hit(a, &g_actors[RE15_ACTOR_SLOT_PLAYER], 0, 3u);   /* Pistole, Hitscan */
        const int p152_schuss = a->re2z_pool152;
        CHECK(96, dmg_ok && pool_min == 13 && p151 == 13 && p152 == 13 && p153 == 13 && !bein &&
                  p152_schuss == 9,
              "12 Flammen: je 5 %d, Reserve %d/%d/%d (13), min %d, Bein %d (0); Schuss danach +0x152 %d (9)",
              dmg_ok, p151, p152, p153, pool_min, bein, p152_schuss);
    }
    /* (97) NEGATIV: Import AUS -> kein RE2-Modell -> O-VB4 = 50 (@0x8006f422), keine Bruecke. */
    re15_re15_re2z_import_set(0);
    re15_actor_init(); pl_far();
    a = mk(1, 0x10, 0, 0, 0, 80);
    r = re15_re2_gl_apply(P, 0, k_box_op40, HIT_OP40);
    CHECK(97, r == 2 && a->hp == 30 && a->sub_state_1 == 14 && a->re2z_hits1d2 == 0,
          "Import AUS: r=%d hp=%d (30) +5=%d 1D2=%d (0, keine Bruecke)", r, a->hp, a->sub_state_1, a->re2z_hits1d2);
    re15_re15_re2z_import_set(1);
    /* (98) Der Zerleger laeuft im GL-Stempel weiter (Reserve-Tor `(s8)+0x152 < 0` @0x80105288-B0):
     *      haben Schuesse die Mitte-Reserve geleert, reisst eine SEITLICHE Flamme das Bein ab — wie im
     *      RE2-Flavor (re2z_hurt -> re2z_leg_gore nach dem GL-Stempel). */
    re15_actor_init(); pl_far();
    a = mk(1, 0x10, 0, 0, 0, 2000);
    {
        int32_t Q[3] = { 0, -100, 300 };
        re15_re2_gl_apply(Q, 0, k_box_op40, HIT_OP40);          /* 1. Treffer saet die Felder */
        a->hit_react = 0; a->state = 1; a->sub_state_1 = 0; a->sub_state_2 = 0;
        a->re2z_pool152 = -1;                                    /* Reserve von Schuessen geleert */
        re15_re2_gl_apply(Q, 0, k_box_op40, HIT_OP40);
    }
    CHECK(98, (a->re2z_flags21a & 0x60u) != 0u && a->re2z_pool152 == -1,
          "Zerleger im GL-Stempel: 21a=0x%04X (&0x60 != 0), +0x152 %d (-1)", a->re2z_flags21a, a->re2z_pool152);
    /* (99) E4 gilt NUR fuer Art 2..4 (BAUPLAN E4, react_table @0x8006f432..34): ein Direktaufruf mit
     *      Art 5 OHNE Hitcode (kein GL-Treffer) nimmt am Import-Zombie die RE1.5-Zahl 50 (@0x8006f422),
     *      nicht die Modellzeile [14] = 15 (RE2-Flammenwerfer @0x800A4258); Kontrolle Art 2 -> 200
     *      (E4, Z9 K0 @0x800A41CC). */
    {
        re15_actor_init(); pl_far();
        a = mk(1, 0x10, 0, 0, 0, 2000);
        re15_enemy_take_damage(a, 5);
        const int d5 = 2000 - a->hp;
        re15_actor_init(); pl_far();
        a = mk(1, 0x10, 0, 0, 0, 2000);
        re15_enemy_take_damage(a, 2);
        const int d2 = 2000 - a->hp;
        CHECK(99, d5 == 50 && d2 == 200, "Import-Zombie Direktaufruf: Art 5 -> %d (50), Art 2 -> %d (200)", d5, d2);
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
}

/* =========================================================================================
 * TEIL "stempel" — B3: Explosion (Resolver Art 2/3/4 mit Punkt P) an RE2-KI-Typen (E4/E6).
 * ========================================================================================= */
static void teil_stempel(void)
{
    printf("== stempel (B3)\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_re2_damage_model_set(1);

    /* (31) RE2-Zombie 0x10 HP 80, P 300 daneben auf gleichem Boden (P.y = -500 @0x800185a8):
     *      HP 80 - 200 (E4, Zeile 9 K0 @0x800A41CC) < 0 -> Zustand 3, +0x5 = 9, +0x1D2 = 3
     *      (Zone 0: -750 < -500, + 3*K mit K = 1 der RE2-Explosion Op 47: Hitcode 0x10020009
     *      `lui a3,0x1002` / `ori a3,a3,0x9` @0x80020d54-58, Spalte @0x80047310-30 —
     *      NACHBESSERUNG K1), +0x1D3 = 15, +0x6 = 0, Reserve unberuehrt, gl_stamp. */
    re15_actor_init(); pl_far();
    re15_actor_t *a = mk_re2z(1, 0x10, 0, 0, 0, 80);
    re15_attack_box_t b = box_at(-300, -500, 0);
    int n = re15_resolve_attack(&b, 2, -1);
    CHECK(31, n == 1 && a->hp == -120 && a->state == 3 && a->sub_state_1 == 9 && a->sub_state_2 == 0 &&
              a->re2z_hits1d2 == 3 && a->re2z_self1d3 == 15 && a->re2_gl_stamp == 1 &&
              a->re2z_pool152 == 13,
          "HE 0x10: n=%d hp=%d st=%d +5=%d +6=%d 1D2=%d 1D3=%d gl=%d pool152=%d", n, a->hp, a->state,
          a->sub_state_1, a->sub_state_2, a->re2z_hits1d2, a->re2z_self1d3, a->re2_gl_stamp, a->re2z_pool152);
    /* (32) Richtung aus P: P bei x = -300 (dx = +300 -> Peilung 4096 - catan(0) = 4095) ->
     *      a = 4095: nur 0x20 (+Bit 0) = 0x21; P bei x = +300 (dx = -300 -> 2047) -> nur Bit 0. */
    {
        uint16_t d1 = a->re2z_hitdir1d0;
        re15_actor_init(); pl_far();
        a = mk_re2z(1, 0x10, 0, 0, 0, 80);
        re15_attack_box_t b2 = box_at(300, -500, 0);
        re15_resolve_attack(&b2, 2, -1);
        uint16_t d2 = a->re2z_hitdir1d0;
        CHECK(32, (d1 & 0xffu) == 0x21u && (d2 & 0xffu) == 0x01u,
              "Richtung aus P: 0x%02X (0x21), 0x%02X (0x01)", d1 & 0xffu, d2 & 0xffu);
    }
    /* (33) Saeure (Art 3) -> Zeile 11, Brand (Art 4) -> Zeile 10 (re2z_row_from_weapon[10/11]). */
    {
        re15_actor_init(); pl_far();
        a = mk_re2z(1, 0x10, 0, 0, 0, 80);
        re15_resolve_attack(&b, 3, -1);
        int s3 = a->sub_state_1;
        re15_actor_init(); pl_far();
        a = mk_re2z(1, 0x10, 0, 0, 0, 80);
        re15_resolve_attack(&b, 4, -1);
        int s4 = a->sub_state_1;
        CHECK(33, s3 == 11 && s4 == 10, "Saeure +5=%d (11), Brand +5=%d (10)", s3, s4);
    }
    /* (34) Brad 0x11 HP 250 ueberlebt (250 - 200 = 50) -> HURT, Zeile 9, Spalte 3 (Op 47, K1;
     *      HURT[9][3] = 0x80105438 `table 0x8010CA84`). */
    re15_actor_init(); pl_far();
    a = mk_re2z(1, 0x11, 0, 0, 0, 250);
    re15_resolve_attack(&b, 2, -1);
    CHECK(34, a->hp == 50 && a->state == 2 && a->sub_state_1 == 9 && a->re2z_hits1d2 == 3,
          "Brad: hp=%d st=%d +5=%d 1D2=%d (3)", a->hp, a->state, a->sub_state_1, a->re2z_hits1d2);
    /* (30) ABGRENZUNG der Klammer-1-Spalte (NACHBESSERUNG K1): NUR HE an der Zombie-Familie.
     *      Saeure/Brand an 0x10 -> Spalte 0 (E6 K0; DEATH 10/11 spaltenunabhaengig `table
     *      0x8010CD8C` = 0x80108530, HURT 10/11 Spalte 0 = 0x80105BC0); Hund HE -> Spalte < 3 (K0,
     *      Spalte >= 3 waere nur Kern/Schrei @0x801046a8-d4 statt "zerplatzt"). */
    {
        int sp[2]; const uint8_t art[2] = { 3, 4 };
        for (int i = 0; i < 2; i++) {
            re15_actor_init(); pl_far();
            a = mk_re2z(1, 0x10, 0, 0, 0, 80);
            re15_resolve_attack(&b, art[i], -1);
            sp[i] = a->re2z_hits1d2;
        }
        re15_actor_init(); pl_far();
        a = mk(1, 0x20, 0, 0, 0, 100); a->re2z_self1d3 = 0;
        re15_resolve_attack(&b, 2, -1);
        int hund = a->re2z_hits1d2, hund_z = a->sub_state_1;
        CHECK(30, sp[0] == 0 && sp[1] == 0 && hund < 3 && hund_z == 9,
              "Spalte: Saeure %d (0), Brand %d (0), Hund HE %d (<3) +5=%d (9)", sp[0], sp[1], hund, hund_z);
    }
    /* (81) +0x1D0 &= 0xFF00 VOR der Richtung (@0x8004716c-84, `andi v1,v1,0xff00` @0x80047178 /
     *      `sh v1,464(s0)` @0x80047184) auch im Explosions-Stempel: vorbelegt 0x12E0, P bei x -300
     *      (Richtung 0x21, s. 32) -> 0x1221 (Oberbyte bleibt, Unterbyte frisch). Ohne das Loeschen
     *      bliebe 0x12E1 stehen (Mutation im Dossier). */
    re15_actor_init(); pl_far();
    a = mk_re2z(1, 0x10, 0, 0, 0, 80);
    a->re2z_hitdir1d0 = 0x12E0u;
    re15_resolve_attack(&b, 2, -1);
    CHECK(81, a->re2z_hitdir1d0 == 0x1221u, "Explosion, 1D0 vorbelegt 0x12E0 -> 0x%04X (0x1221)",
          a->re2z_hitdir1d0);
    /* (35) 0x16 (Zeile @0x800A42A8): HE 80 / Saeure 200 / Brand 80. */
    {
        int hp[3]; const uint8_t art[3] = { 2, 3, 4 };
        for (int i = 0; i < 3; i++) {
            re15_actor_init(); pl_far();
            a = mk_re2z(1, 0x16, 0, 0, 0, 300);
            re15_resolve_attack(&b, art[i], -1);
            hp[i] = a->hp;
        }
        CHECK(35, hp[0] == 220 && hp[1] == 100 && hp[2] == 220, "0x16: HE %d (220) Saeure %d (100) Brand %d (220)",
              hp[0], hp[1], hp[2]);
    }
    /* (36) Hund 0x20 RE2: 300 (@0x800A44C4), +0x5 bleibt RE1.5-Waffen-Id (Art 3 -> 10 = Saeure ->
     *      s_re2d_row_von_waffe[10] = 11), +0x1D3 = 15, +0x6 = 0. */
    re15_actor_init(); pl_far();
    a = mk(1, 0x20, 0, 0, 0, 100); a->re2z_self1d3 = 0;
    re15_resolve_attack(&b, 3, -1);
    CHECK(36, a->hp == -200 && a->state == 3 && a->sub_state_1 == 10 && a->sub_state_2 == 0 && a->re2z_self1d3 == 15,
          "Hund Saeure: hp=%d st=%d +5=%d +6=%d 1D3=%d", a->hp, a->state, a->sub_state_1, a->sub_state_2,
          a->re2z_self1d3);
    /* (39) Direktaufruf OHNE Punkt (re15_enemy_take_damage, Sonden-/Altpfad) im RE2-Flavor: der
     *      Zeilen-Stempel re15_re2_stamp_hit(row_src 1) liest re2z_row_from_atktype -> Art 2/3/4 =
     *      9/11/10 (DAT_8006F430 @0x8006f432..34 ueber re2z_row_from_weapon), nicht mehr 17. */
    {
        int z[3]; const uint8_t art[3] = { 2, 3, 4 };
        for (int i = 0; i < 3; i++) {
            re15_actor_init(); pl_far();
            a = mk_re2z(1, 0x11, 0, 0, 0, 2000);
            re15_enemy_take_damage(a, art[i]);
            z[i] = a->sub_state_1;
        }
        CHECK(39, z[0] == 9 && z[1] == 11 && z[2] == 10, "atktype 2/3/4 -> %d/%d/%d (9/11/10)", z[0], z[1], z[2]);
    }
    /* (37) NEGATIV: RE1.5-Flavor, Import AUS -> 1000 flach, kein RE2-Stempel (+0x6 = 1). */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re15_re2z_import_set(0);
    re15_actor_init(); pl_far();
    a = mk_re2z(1, 0x10, 0, 0, 0, 80);
    re15_resolve_attack(&b, 2, -1);
    CHECK(37, a->hp == 80 - 1000 && a->sub_state_2 == 1 && a->re2z_self1d3 == 0 && a->re2_gl_stamp == 0,
          "RE1.5 ohne Import: hp=%d +6=%d 1D3=%d gl=%d", a->hp, a->sub_state_2, a->re2z_self1d3, a->re2_gl_stamp);
    /* (38) RE1.5-Flavor MIT Import (Default): Modellwert 200 (E4), aber kein RE2-Stempel. */
    re15_re15_re2z_import_set(1);
    re15_actor_init(); pl_far();
    a = mk_re2z(1, 0x10, 0, 0, 0, 80);
    re15_resolve_attack(&b, 2, -1);
    CHECK(38, a->hp == 80 - 200 && a->sub_state_2 == 1 && a->sub_state_1 == 9 && a->re2_gl_stamp == 0,
          "RE1.5 mit Import: hp=%d +6=%d +5=%d gl=%d", a->hp, a->sub_state_2, a->sub_state_1, a->re2_gl_stamp);
}

/* =========================================================================================
 * TEIL "kasten" — B2: Kasten-Versatz FUN_8002b498 (gedreht) + Kaesten je Typ + Voreinstellung.
 * Alle Faelle ueber den echten Resolver (re15_resolve_attack, Art 2, Radius 500), RE1.5-Flavor.
 * Sqrt-Grenzen (SquareRoot0-Tabelle) mit Abstand 50 geprueft; das Hoehenband exakt (+-1).
 * ========================================================================================= */
static int trifft(uint8_t type, int32_t ex, int32_t ez, int16_t rot, int32_t px, int32_t py, int32_t pz)
{
    re15_actor_init(); pl_far();
    re15_actor_t *e = mk(1, type, ex, 0, ez, 5000);
    e->rot_y = rot;
    re15_attack_box_t b = box_at(px, py, pz);
    re15_resolve_attack(&b, 2, -1);
    return e->hp != 5000;
}

static void teil_kasten(void)
{
    printf("== kasten (B2)\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re15_re2z_import_set(0);

    /* (41) ALLIGATOR {1000,-720,0,2200,720,800} @0x80118b98: Mitte = Lage + RotY(Gier)*(1000,0).
     *      Gier 0 -> Mitte (1000,·,0); laengs +X Radius 2200+500: 3650 trifft, 3750 nicht.
     *      Gier 1024 -> Mitte (0,·,-1000) (RotMatrix: lokal +x -> Welt -z); laengs -Z: -3650
     *      trifft, -3750 nicht; der ungedrehte Punkt (3650,0) trifft dann NICHT mehr. */
    {
        int a0 = trifft(0x23, 0, 0, 0, 3650, -500, 0), a1 = trifft(0x23, 0, 0, 0, 3750, -500, 0);
        int b0 = trifft(0x23, 0, 0, 1024, 0, -500, -3650), b1 = trifft(0x23, 0, 0, 1024, 0, -500, -3750);
        int c0 = trifft(0x23, 0, 0, 1024, 3650, -500, 0);
        CHECK(41, a0 && !a1 && b0 && !b1 && !c0,
              "Alligator: G0 3650/3750 -> %d/%d (1/0); G1024 z -3650/-3750 -> %d/%d (1/0); G1024 x 3650 -> %d (0)",
              a0, a1, b0, b1, c0);
    }
    /* (42) Alligator-Hoehenband exakt: Mitte y = -720, h 720 -> |P.y + 720| < 500 + 720 = 1220
     *      (streng, @0x8002b778-7a4): P.y 499 trifft, 500 nicht; -1939 trifft, -1940 nicht. */
    {
        int u = trifft(0x23, 0, 0, 0, 1000, 499, 0), v = trifft(0x23, 0, 0, 0, 1000, 500, 0);
        int w = trifft(0x23, 0, 0, 0, 1000, -1939, 0), x = trifft(0x23, 0, 0, 0, 1000, -1940, 0);
        CHECK(42, u && !v && w && !x, "Alligator-Band 499/500/-1939/-1940 -> %d/%d/%d/%d (1/0/1/0)", u, v, w, x);
    }
    /* (43) HUND {0,-720,0,900,720,450} @0x80120f64 (Sektor): Gier 0 laengs +X 900+500 -> 1350 trifft,
     *      1450 nicht; quer (+Z) 450+500 -> 900 trifft, 1000 nicht. Gier 1024 tauscht die Achsen. */
    {
        int a0 = trifft(0x20, 0, 0, 0, 1350, -500, 0), a1 = trifft(0x20, 0, 0, 0, 1450, -500, 0);
        int b0 = trifft(0x20, 0, 0, 0, 0, -500, 900),  b1 = trifft(0x20, 0, 0, 0, 0, -500, 1000);
        int c0 = trifft(0x20, 0, 0, 1024, 0, -500, 1350), c1 = trifft(0x20, 0, 0, 1024, 1350, -500, 0);
        CHECK(43, a0 && !a1 && b0 && !b1 && c0 && !c1,
              "Hund: laengs 1350/1450 -> %d/%d, quer 900/1000 -> %d/%d, G1024 z1350/x1350 -> %d/%d",
              a0, a1, b0, b1, c0, c1);
    }
    /* (44) Hund-Band: Mitte y -720, h 720 -> 1220: P.y 499 trifft, 500 nicht. */
    {
        int u = trifft(0x20, 0, 0, 0, 0, 499, 0), v = trifft(0x20, 0, 0, 0, 0, 500, 0);
        CHECK(44, u && !v, "Hund-Band 499/500 -> %d/%d (1/0)", u, v);
    }
    /* (45) FEUER 0x26 (ROOM1090) {0,0,0,600,720,600} @0x80121258: Band um y 0 (nicht -720):
     *      P.y 1000 trifft, -1300 nicht (mit dem alten -720 genau umgekehrt). */
    {
        int u = trifft(0x26, 0, 0, 0, 0, 1000, 0), v = trifft(0x26, 0, 0, 0, 0, -1300, 0);
        CHECK(45, u && !v, "Feuer 0x26 Band: P.y 1000 -> %d (1), -1300 -> %d (0)", u, v);
    }
    /* (46) NPC 0x45 {..,500,1440,..} @0x80121728 / 0x4b {..,300,1440,..} @0x801218c8 (HP >= 0):
     *      0x45: 950 trifft (R 1000), 1050 nicht; 0x4b: 750 trifft (R 800), 850 nicht. */
    {
        int a0 = trifft(0x45, 0, 0, 0, 950, -500, 0), a1 = trifft(0x45, 0, 0, 0, 1050, -500, 0);
        int b0 = trifft(0x4b, 0, 0, 0, 750, -500, 0), b1 = trifft(0x4b, 0, 0, 0, 850, -500, 0);
        CHECK(46, a0 && !a1 && b0 && !b1, "NPC 0x45 950/1050 -> %d/%d, 0x4b 750/850 -> %d/%d", a0, a1, b0, b1);
    }
    /* (47) Spawn-Voreinstellung {0,0,0,1,1,1} @0x80072be0 fuer kastenlose Typen (FX 0x24):
     *      waagrecht 1 + 500: 400 trifft, 600 nicht; Band 1 + 500 um y 0: P.y -500 trifft. */
    {
        int a0 = trifft(0x24, 0, 0, 0, 400, -500, 0), a1 = trifft(0x24, 0, 0, 0, 600, -500, 0);
        CHECK(47, a0 && !a1, "Voreinstellung 0x24: 400 -> %d (1), 600 -> %d (0)", a0, a1);
    }
    /* (48) G5-Tentakel 0x37 mit HP -1 ist KEIN Kandidat (RE2 Gate 3; em37 HP -1 @0x80100530/34). */
    {
        re15_actor_init(); pl_far();
        re15_actor_t *e = mk(1, 0x37, 0, 0, 0, -1);
        re15_attack_box_t b = box_at(0, -500, 0);
        int n = re15_resolve_attack(&b, 2, -1);
        CHECK(48, n == 0 && e->hp == -1 && e->state == 1, "Tentakel: n=%d hp=%d st=%d", n, e->hp, e->state);
    }
    /* (49) NEGATIV/REGRESSION: Zombie {0,-1440,0,400,1440,400} unveraendert (kein Versatz x/z):
     *      Gier 1024 aendert nichts; 850 trifft (R 900), 950 nicht. */
    {
        int a0 = trifft(0x10, 0, 0, 0, 850, -500, 0), a1 = trifft(0x10, 0, 0, 0, 950, -500, 0);
        int b0 = trifft(0x10, 0, 0, 1024, 850, -500, 0);
        CHECK(49, a0 && !a1 && b0, "Zombie 850/950 -> %d/%d, G1024 850 -> %d", a0, a1, b0);
    }
    re15_re15_re2z_import_set(1);
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    int alle = (strcmp(teil, "alle") == 0);
    if (alle || !strcmp(teil, "tore"))    teil_tore();
    if (alle || !strcmp(teil, "applier")) teil_applier();
    if (alle || !strcmp(teil, "stempel")) teil_stempel();
    if (alle || !strcmp(teil, "kasten"))  teil_kasten();
    printf("probe_r34_schaden %s: %d Fehler (erste Pruefung %d)\n", teil, s_fails, s_first_fail);
    return s_first_fail;
}
