/* test_r36_schuss_kraehe.c — Runde 36 Spur C, Punkt 3: Kraehe gegen Super Redhawk (RE2 GIB 0x80102CA0).
 *
 * Misst die MECHANIK des 13-Teile-Zerlegers (EMOVL21_S0.BIN, Belege include/re15_schuss_r36.h Punkt 3):
 *  A  Felder je Zeile nach dem GIB fuer Maske 0 (alle Wirbel ausser 0/9/10) und 0x1F (alle Wurf):
 *     Wurf 0x80102EF4 (Flags |= 0x4A, Tinte 0x101040, +0x9C 800, +0x9E 10, +0x98 Peilung, +0xA4 -100),
 *     Wirbel 0x80102F34 (Flags |= 0x1062, +0x86 0, +0x3C 100..227, +0x79 20, +0x40 = rand<<4).
 *  B  RNG-Draws: 3 je Wirbel-Zeile (@0x80102F54/68/88), 0 je Wurf-Zeile.
 *  C  Physik ueber die Part-Bruecke (re15_re2z_gore_part_matrix, wie der Renderer je Bild): Wurf-Zeilen
 *     verschwinden nach 29 Bildern (FUN_80028DAC `sw zero,0` @0x80028DDC), Wirbel-Zeilen fallen mit +20/Bild
 *     auf den Boden der Kraehe (+0x1C2, nicht ihre Flughoehe) und verlieren beim zweiten Kontakt Bit 0
 *     (FUN_80028AD8 `and v0,v0,-2` @0x80028CA0) -> nach spaetestens 120 Bildern ist KEIN Teil mehr gezeichnet
 *     (re15_re2z_gore_resolve out_draw == 0 fuer alle 13).
 *  D  Der Kraehen-HURT-Dispatch (@0x80104A18): Redhawk w7 -> RE2-Zeile 5 -> GIB; Pistole w3 -> Zeile 3 ->
 *     Flug-Treffer (kein GIB) — ueber re2c_hurt am echten Aktor (RE2-KI).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_ai_flavor.h"
#include "re15_schuss_r36.h"

extern re15_actor_t g_actors[];
extern uint32_t re15_re2_rand(void);
extern uint32_t re15_re2_rand_draws(void);
extern void     re15_re2z_rng_reset(void);
extern int  re15_re2z_gore_resolve(const re15_actor_t *e, const int8_t *bone_parent, int n,
                                   uint8_t *out_draw, uint32_t *out_tint, uint8_t *out_mesh);
extern int  re15_re2z_gore_part_matrix(re15_actor_t *e, int part, uint32_t frame, int32_t rot[9], int32_t trans[3]);

static int s_fails = 0;
#define CHECK(nr, c, ...) do { if (!(c)) { printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); s_fails++; } \
                               else { printf("ok   %d\n", (nr)); } } while (0)

static re15_actor_t *kraehe(void)
{
    re15_actor_t *e = &g_actors[RE15_ACTOR_SLOT_PLAYER + 1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x21; e->state = 1; e->hp = 10;
    e->x = 1000; e->y = -1500; e->z = 2000; e->rot_y = 0;      /* fliegt 1500 ueber dem Boden */
    e->dog_floor_y = 0;                                         /* +0x1C2 Boden-Probe */
    re15_schuss_r36_kraehe_parts_seed(e);
    return e;
}

static void teil_a_b(void)
{
    printf("== A/B Felder und Draws des GIB\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_t *e = kraehe();
    for (int i = 0; i < 16; i++) {
        if (i < 13) CHECK(1, e->re2z_part_flags[i] == 1u && e->re2z_part_tint[i] == 0x00808080u && e->re2z_part_mesh[i] == i,
                          "Seed Zeile %d: Bit 0, Tinte 0x808080, Mesh = i", i);
    }
    CHECK(2, e->re2z_part_flags[13] == 0u && e->re2z_part_flags[15] == 0u, "Seed: Zeilen 13..15 leer (13 Bones)");

    /* Maske 0x1F: alle 13 Zeilen WURF, keine Draws. */
    re15_re2z_rng_reset();
    uint32_t d0 = re15_re2_rand_draws();
    re15_schuss_r36_kraehe_gib(e, 1234, 0x1Fu);
    CHECK(3, re15_re2_rand_draws() == d0, "Maske 0x1F: 0 Draws (Wurf 0x80102EF4 ist RNG-frei), ist %u", re15_re2_rand_draws() - d0);
    int ok = 1;
    for (int i = 0; i < 13; i++)
        ok &= (e->re2z_part_flags[i] == (1u | 0x4Au)) && e->re2z_part_tint[i] == 0x00101040u &&
              e->re2z_part_w9c[i] == 800 && e->re2z_part_w9e[i] == 10 && e->re2z_part_yaw98[i] == 1234 &&
              e->re2z_part_life[i] == 0 && e->re2z_part_w9a[i] == 0 && e->re2z_part_wa4[i] == -100;
    CHECK(4, ok, "Maske 0x1F: alle 13 Zeilen Flags 0x4B, Tinte 0x101040, 800/10/Peilung/0/0/-100");

    /* Maske 0: Zeilen 0/9/10 Wurf, die zehn anderen Wirbel = 30 Draws. */
    e = kraehe();
    re15_re2z_rng_reset();
    d0 = re15_re2_rand_draws();
    re15_schuss_r36_kraehe_gib(e, 512, 0u);
    CHECK(5, re15_re2_rand_draws() - d0 == 30u, "Maske 0: 10 Wirbel-Zeilen x 3 Draws = 30, ist %u", re15_re2_rand_draws() - d0);
    ok = 1;
    for (int i = 0; i < 13; i++) {
        int wurf = (i == 0 || i == 9 || i == 10);
        if (wurf) ok &= (e->re2z_part_flags[i] == (1u | 0x4Au));
        else      ok &= (e->re2z_part_flags[i] == (1u | 0x1062u)) && e->re2z_part_st86[i] == 0 &&
                        e->re2z_part_v[i][2] >= 100 && e->re2z_part_v[i][2] <= 227 && e->re2z_part_v[i][1] == 0 &&
                        e->re2z_part_grav[i] == 20 && e->re2z_part_rot[i][0] == 0 && e->re2z_part_rot[i][2] == 0 &&
                        (e->re2z_part_rot[i][1] & 0xf) == 0 && e->re2z_part_tint[i] == 0x00808080u;
    }
    CHECK(6, ok, "Maske 0: 0/9/10 Wurf (0x4B), 1..8/11/12 Wirbel (0x1063, +0x3C 100..227, +0x79 20, +0x40 = rand<<4)");
    /* Maske 0x02 nur: 3..5 Wurf, 1,2,6,7,8,11,12 Wirbel = 7 x 3 = 21 Draws. */
    e = kraehe();
    re15_re2z_rng_reset(); d0 = re15_re2_rand_draws();
    re15_schuss_r36_kraehe_gib(e, 0, 0x02u);
    CHECK(7, re15_re2_rand_draws() - d0 == 21u && (e->re2z_part_flags[3] & 0x4Au) == 0x4Au && (e->re2z_part_flags[1] & 0x1062u) == 0x1062u,
          "Maske 0x02: 3..5 Wurf, 7 Wirbel-Zeilen = 21 Draws (ist %u)", re15_re2_rand_draws() - d0);
}

/* Physik wie der Renderer: je Bild fuer jede Zeile re15_re2z_gore_part_matrix mit der Ausgangsmatrix. */
static int teile_gezeichnet(re15_actor_t *e)
{
    static const int8_t parent[13] = { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t draw[RE15_EMD_MAX_BONES]; uint32_t tint[RE15_EMD_MAX_BONES]; uint8_t mesh[RE15_EMD_MAX_BONES];
    memset(draw, 1, sizeof draw);
    if (!re15_re2z_gore_resolve(e, parent, 13, draw, tint, mesh)) return -1;
    int n = 0;
    for (int i = 0; i < 13; i++) n += draw[i] ? 1 : 0;
    return n;
}
static void teil_c(void)
{
    printf("== C Physik bis zum Verschwinden (Wurf 29 Bilder, Wirbel 2. Bodenkontakt)\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_t *e = kraehe();
    re15_re2z_rng_reset();
    re15_schuss_r36_kraehe_gib(e, 0, 0x0Au);               /* 0/9/10 + 3..5 + 11 Wurf; 1,2,6,7,8,12 Wirbel */
    CHECK(10, teile_gezeichnet(e) == 13, "nach dem GIB: alle 13 Teile gezeichnet (Bit 0 steht), ist %d", teile_gezeichnet(e));
    int n_wurf_weg_bild = -1, n_alle_weg_bild = -1;
    int32_t y_min_wirbel = 1 << 30;
    for (uint32_t f = 1; f <= 160; f++) {
        for (int i = 0; i < 13; i++) {
            int32_t rot[9] = { 4096,0,0, 0,4096,0, 0,0,4096 };
            int32_t t[3] = { e->x, e->y, e->z };           /* Ausgangsmatrix = Lage der Kraehe (Flughoehe) */
            (void)re15_re2z_gore_part_matrix(e, i, f, rot, t);
        }
        for (int i = 0; i < 13; i++)
            if ((e->re2z_part_flags[i] & 0x20u) && (e->re2z_part_flags[i] & 1u) && e->re2z_part_t[i][1] < y_min_wirbel)
                y_min_wirbel = e->re2z_part_t[i][1];
        int wurf_da = 0;
        for (int i = 0; i < 13; i++) if (e->re2z_part_flags[i] == (1u | 0x4Au)) wurf_da++;
        if (n_wurf_weg_bild < 0 && wurf_da == 0) n_wurf_weg_bild = (int)f;
        if (n_alle_weg_bild < 0 && teile_gezeichnet(e) == 0) n_alle_weg_bild = (int)f;
    }
    CHECK(11, n_wurf_weg_bild == 30, "Wurf-Zeilen verschwinden nach 29 Bildern (`sltiu 0x1d` @0x80028DCC, Flags = 0 @0x80028DDC): Bild %d", n_wurf_weg_bild);
    CHECK(12, n_alle_weg_bild > 0 && n_alle_weg_bild <= 120, "alle 13 Teile weg (Bit 0) bis Bild 120: Bild %d", n_alle_weg_bild);
    /* Wirbel-Teile sind vom Boden +0x1C2 (0) abgeprallt, nicht von der Flughoehe (-1500): ihre tiefste Lage liegt
     * UNTER -1500 (PSX y+ = unten), d.h. sie sind gefallen. */
    int32_t y_max_wirbel = -(1 << 30);
    for (int i = 0; i < 13; i++) if (e->re2z_part_flags[i] & 0x20u) if (e->re2z_part_t[i][1] > y_max_wirbel) y_max_wirbel = e->re2z_part_t[i][1];
    CHECK(13, y_max_wirbel >= -300 && y_max_wirbel <= 0, "Wirbel-Teile liegen am Boden +0x1C2 = 0 (nicht an der Flughoehe -1500): y_max %d", y_max_wirbel);
    CHECK(14, e->crow_hide == 0, "crow_hide bleibt 0 (das Netz wird ueber die Part-Bruecke gezeichnet)");
}

static void teil_d(void)
{
    printf("== D Dispatch @0x80104A18: Redhawk -> GIB, Pistole -> Flug-Treffer\n");
    extern int  re15_re2crow_hurt_test(re15_actor_t *e, re15_actor_t *pl);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl); pl->active = 1; pl->hp = 100; pl->state = 1; pl->x = 0; pl->z = 2000;
    re15_actor_t *e = kraehe();
    e->state = 2; e->sub_state_1 = 7; e->sub_state_2 = 0;     /* HURT, +0x5 = Redhawk (w7) */
    re15_re2z_rng_reset();
    re15_re2crow_hurt_test(e, pl);
    int zerlegt = 0; for (int i = 0; i < 13; i++) zerlegt += (e->re2z_part_flags[i] & 0x40u) ? 1 : 0;
    CHECK(20, zerlegt == 13 && e->state == 7 && e->sub_state_1 == 1 && e->crow_hide == 0,
          "Redhawk (Zeile 5 @0x80104A2C): GIB -> 13 Teile mit eigener Matrix, Zustand (7,1), crow_hide 0 (ist %d/%u/%u/%u)",
          zerlegt, e->state, e->sub_state_1, e->crow_hide);
    e = kraehe();
    e->state = 2; e->sub_state_1 = 3; e->sub_state_2 = 0;     /* HURT, +0x5 = Pistole (w3 -> Zeile 3) */
    re15_re2crow_hurt_test(e, pl);
    zerlegt = 0; for (int i = 0; i < 13; i++) zerlegt += (e->re2z_part_flags[i] & 0x40u) ? 1 : 0;
    CHECK(21, zerlegt == 0 && e->state == 2, "Pistole (Zeile 3 @0x80104A24 -> 0x80102934): kein GIB, Flug-Treffer (ist %d, st=%u)", zerlegt, e->state);
}

int main(void)
{
    teil_a_b(); teil_c(); teil_d();
    if (s_fails) { printf("test_r36_schuss_kraehe: %d FAILURES\n", s_fails); return 1; }
    printf("test_r36_schuss_kraehe: OK\n");
    return 0;
}
