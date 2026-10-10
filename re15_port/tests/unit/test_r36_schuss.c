/* test_r36_schuss.c — Runde 36 Spur C: der allgemeine Schusspfad (alle Schusswaffen).
 *
 * Misst die MECHANIK am Resolver re15_player_weapon_fire (FUN_80011f50) — Belege in
 * include/re15_schuss_r36.h und im Dossier analysis/befunde_runde36/C_schuss.md.
 *
 *  A  SICHTLINIE (RE1.5 FUN_8001b9b4 @0x80012168-74 -> FUN_8003dcc4): echte ROOM1140.RDT,
 *     Konferenztisch = SCA-Zelle 6 {x -4650..7450, z -17450..-11350, Band 0, Klasse 3}. Leon
 *     (200,-10300) Blick -z, Zombie (200,-19600): Pistole 3, Redhawk 7, Schrot 8, Python 20,
 *     Ingram 12, Messer 1 -> KEIN Treffer, Zaehler `verworfen` zaehlt; neben dem Tisch (x 9000)
 *     und ohne SCA -> Treffer; Spieler in Band 1 (Zelle traegt Band 0) -> Treffer (Bandfilter
 *     `lbu v1,130(cur)` / `bne` @0x8003de60-6c); Zombie VOR dem Tisch (z -11000) -> Treffer.
 *  B  FRESSER-SPERRE (RE2 FUN_800410CC @0x80041270-9c ueber re15_re2z_hit_filter_apply):
 *     RE2-KI, Zombie EXEC[8] (ss1 8) mit +0x1D3 = 0x80 / +0x10E = 0x4004 -> kein Treffer fuer
 *     3/7/8/20; nach dem Aufstehen (P3 `andi 0x7f` @0x80103CE4, Wecker `andi 0xbfff`
 *     @0x80104F0C) -> Treffer. Je Gate einzeln. Liegender EXEC[7] (nicht passiv) unveraendert.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_inventory.h"
#include "re15_ai_flavor.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_schuss_r36.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

extern re15_actor_t g_actors[];
extern void re15_re2z_hit_filter_apply(int slot);

static int s_fails = 0;
#define CHECK(nr, c, ...) do { if (!(c)) { printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); s_fails++; } \
                               else { printf("ok   %d\n", (nr)); } } while (0)

static uint8_t *rdt_laden(const char *raum)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/STAGE1/%s.RDT", RE15_XSTR(RE15_ASSETS_PATH), raum);
    FILE *f = fopen(p, "rb");
    uint8_t *rdt = NULL; long n = 0;
    if (f) { fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
             rdt = (uint8_t *)malloc((size_t)n);
             if (!rdt || fread(rdt, 1, (size_t)n, f) != (size_t)n) n = 0; fclose(f); }
    memset(&g_room_rdt, 0, sizeof g_room_rdt);
    g_room_rdt_ok = (n > 0 && re15_rdt_parse(rdt, (size_t)n, &g_room_rdt) == 0 && g_room_rdt.sca_count > 0);
    return rdt;
}

static re15_actor_t *spieler(int32_t x, int32_t z, int16_t rot, uint8_t band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->hp = 100; pl->state = 1; pl->x = x; pl->y = 0; pl->z = z; pl->rot_y = rot;
    pl->floor = band;                                   /* +0x82 = Band (re15_collision_set_band) */
    re15_player_apply_hitbox(pl);
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) memset(&g_actors[s], 0, sizeof g_actors[s]);
    return pl;
}
static re15_actor_t *mk(int slot, uint8_t type, int32_t x, int32_t z, int16_t hp, int16_t rot)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = type; e->x = x; e->y = 0; e->z = z; e->hp = hp;
    e->state = 1; e->rot_y = rot;
    re15_enemy_apply_hitbox(e, type);
    return e;
}
static int feuer(int w)
{
    re15_inv_init(); re15_inv_grant(w, 6); re15_player_set_equipped_weapon(w);
    return re15_player_weapon_fire(w);
}

/* ========================================================================================== */
static void teil_a(void)
{
    printf("== A Sichtlinie FUN_8001b9b4 (ROOM1140, Konferenztisch Zelle 6)\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    uint8_t *rdt = rdt_laden("ROOM1140");
    CHECK(1, g_room_rdt_ok && g_room_rdt.sca_count > 6, "ROOM1140.RDT geladen (sca_count %d)", g_room_rdt.sca_count);
    if (!g_room_rdt_ok) return;
    const re15_sca_entry_t *c6 = &g_room_rdt.sca[6];
    CHECK(2, (int16_t)c6->x == -4650 && (int16_t)c6->z == -17450 && c6->width == 12100 && c6->density == 6100 &&
             c6->type == 1 && c6->floor == 0x03,
          "Zelle 6 = Tisch {x -4650..7450, z -17450..-11350, Typ 1, Wort+10 0x0300 = Band 0 Klasse 3}");

    static const int W[5] = { 3, 7, 8, 20, 12 };
    static const char *WN[5] = { "Pistole 3", "Redhawk 7", "Schrot 8", "Python 20", "Ingram 12" };
    int nr = 10;
    for (int k = 0; k < 5; k++, nr += 2) {
        /* HINTER dem Tisch: Leon (200,-10300) Blick -z (rot 1024), Zombie (200,-19600). */
        re15_schuss_r36_sichtlinie_reset();
        spieler(200, -10300, 1024, 0);
        re15_actor_t *z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 200, -19600, 250, 0);
        int hit = feuer(W[k]);
        CHECK(nr, hit == 0 && z->hp == 250 && re15_schuss_r36_sichtlinie_verworfen() == 1,
              "%s ueber den Tisch: KEIN Treffer (hit=%d hp=%d verworfen=%d)", WN[k], hit, z->hp,
              re15_schuss_r36_sichtlinie_verworfen());
        /* NEBEN dem Tisch (x 9000 > 7450): Treffer. */
        re15_schuss_r36_sichtlinie_reset();
        spieler(9000, -10300, 1024, 0);
        z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 9000, -19600, 250, 0);
        hit = feuer(W[k]);
        CHECK(nr + 1, hit != 0 && z->hp < 250 && re15_schuss_r36_sichtlinie_verworfen() == 0,
              "%s neben dem Tisch: Treffer (hit=%d hp=%d)", WN[k], hit, z->hp);
    }
    /* MESSER (w1, Kegel 1100+400): die Sichtlinie laeuft auch fuer w1/w2 (@0x80012168 steht hinter der
     * Schleife, vor der Typ-Pruefung — die Tester-Tabelle @0x8006e548 aendert daran nichts). Der
     * FUN_8003dcc4-Test prueft aber nur, ob die Strecke eine der beiden ZELLDIAGONALEN kreuzt
     * (@0x8003df28/@0x8003df94 bzw. @0x8003e02c/@0x8003e08c): die 1200 lange Strecke (200,-10300) ->
     * (200,-11500) ragt nur 150 in die Zelle (Kante -11350) und erreicht keine Diagonale (Hauptdiagonale
     * bei x=200: z ~ -15000, Nebendiagonale z ~ -13800) -> byte-true NICHT blockiert, Treffer. */
    {
        re15_schuss_r36_sichtlinie_reset();
        spieler(200, -10300, 1024, 0);
        re15_actor_t *z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 200, -11500, 250, 0);
        int hit = feuer(1);
        CHECK(20, hit != 0 && z->hp < 250 && re15_schuss_r36_sichtlinie_verworfen() == 0,
              "Messer an der Tischkante (Strecke kreuzt keine Diagonale): Treffer (hit=%d hp=%d)", hit, z->hp);
        /* Dieselbe Strecke, aber VERLAENGERT bis hinter die Nebendiagonale: blockiert. */
        z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 200, -14500, 250, 0);
        CHECK(21, re15_schuss_r36_sichtlinie_blockiert(&g_actors[RE15_ACTOR_SLOT_PLAYER], z) == 1,
              "Strecke (200,-10300)->(200,-14500) kreuzt die Nebendiagonale: blockiert");
    }
    /* Zombie VOR dem Tisch (z -11000, Tischkante -11350): frei. */
    re15_schuss_r36_sichtlinie_reset();
    spieler(200, -10300, 1024, 0);
    re15_actor_t *z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 200, -11000, 250, 0);
    int hit = feuer(3);
    CHECK(30, hit != 0 && z->hp < 250, "Pistole auf Zombie VOR dem Tisch (z -11000): Treffer (hit=%d hp=%d)", hit, z->hp);
    /* Spieler im Band 1: die Zelle (Band 0) wird uebersprungen (`lbu v1,130(cur)` / `bne` @0x8003de60-6c). */
    re15_schuss_r36_sichtlinie_reset();
    spieler(200, -10300, 1024, 1);
    z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 200, -19600, 250, 0);
    hit = feuer(3);
    CHECK(31, hit != 0 && z->hp < 250, "Spieler-Band 1 gegen Band-0-Zelle: kein Filter-Treffer -> Treffer (hit=%d)", hit);
    /* Diagonal ueber die Tischecke: Leon (-6000,-10000) -> Zombie (0,-19000) kreuzt die Zelle. */
    re15_schuss_r36_sichtlinie_reset();
    spieler(-6000, -10000, 0, 0);
    {
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 0, -19000, 250, 0);
        CHECK(32, re15_schuss_r36_sichtlinie_blockiert(pl, z) == 1, "Diagonale ueber die Tischecke: blockiert");
        /* An der Zelle vorbei (links davon, x < -4650): frei. */
        z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, -6000, -19000, 250, 0);
        CHECK(33, re15_schuss_r36_sichtlinie_blockiert(pl, z) == 0, "links am Tisch vorbei (x -6000): frei");
    }
    /* Ohne SCA: nie blockiert. */
    g_room_rdt_ok = 0;
    re15_schuss_r36_sichtlinie_reset();
    spieler(200, -10300, 1024, 0);
    z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 200, -19600, 250, 0);
    hit = feuer(3);
    CHECK(34, hit != 0 && z->hp < 250 && re15_schuss_r36_sichtlinie_verworfen() == 0, "ohne SCA: Treffer");
    memset(&g_room_rdt, 0, sizeof g_room_rdt); free(rdt);
}

/* ========================================================================================== */
static void teil_b(void)
{
    printf("== B Fresser-Sperre (RE2 FUN_800410CC Gate 2/4 ueber +0x93 Bit 0)\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    g_room_rdt_ok = 0;
    static const int W[4] = { 3, 7, 8, 20 };
    static const char *WN[4] = { "Pistole 3", "Redhawk 7", "Schrot 8", "Python 20" };
    int nr = 40;
    for (int k = 0; k < 4; k++, nr += 3) {
        /* FRISST: EXEC[8] (ss1 8), +0x1D3 = 0x80 (@0x80103c04-14), +0x10E = 0x4004 (@0x80100A88-8C), Deskriptor 0x06. */
        spieler(0, 0, 0, 0);
        re15_actor_t *z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 2500, 0, 250, 2048);
        z->sub_state_1 = 8; z->grid_id = 0x06; z->re2z_self1d3 = 0x80; z->re2z_f10e = 0x4004;
        re15_re2z_hit_filter_apply(RE15_ACTOR_SLOT_PLAYER + 1);
        int hit = feuer(W[k]);
        CHECK(nr, hit == 0 && z->hp == 250, "%s auf den FRESSER (1D3=80, 10E=4004): KEIN Treffer (hit=%d hp=%d hr=%02x)",
              WN[k], hit, z->hp, z->hit_react);
        /* Zwei Fresser hintereinander: der zweite ist ebenso gesperrt (kein Rueckgriff auf ihn). */
        re15_actor_t *z2 = mk(RE15_ACTOR_SLOT_PLAYER + 2, 0x10, 4000, 0, 250, 2048);
        z2->sub_state_1 = 8; z2->grid_id = 0x06; z2->re2z_self1d3 = 0x80; z2->re2z_f10e = 0x4004;
        re15_re2z_hit_filter_apply(RE15_ACTOR_SLOT_PLAYER + 1);
        re15_re2z_hit_filter_apply(RE15_ACTOR_SLOT_PLAYER + 2);
        hit = feuer(W[k]);
        CHECK(nr + 1, hit == 0 && z->hp == 250 && z2->hp == 250, "%s auf zwei Fresser: KEIN Treffer (hit=%d)", WN[k], hit);
        /* AUFGESTANDEN: P3 `andi 0x7f` @0x80103CE4-FC + Wecker `andi 0xbfff` @0x80104F0C-10 -> Treffer. */
        memset(z2, 0, sizeof *z2);
        z->re2z_self1d3 &= 0x7f; z->re2z_f10e &= (uint16_t)~0x4000u;
        re15_re2z_hit_filter_apply(RE15_ACTOR_SLOT_PLAYER + 1);
        hit = feuer(W[k]);
        CHECK(nr + 2, hit != 0 && z->hp < 250, "%s nach dem Aufstehen: Treffer (hit=%d hp=%d)", WN[k], hit, z->hp);
    }
    /* Je Gate einzeln (Pistole): nur 1D3=80 sperrt, nur 10E=4000 sperrt. */
    {
        spieler(0, 0, 0, 0);
        re15_actor_t *z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 2500, 0, 250, 2048);
        z->sub_state_1 = 8; z->grid_id = 0x06; z->re2z_self1d3 = 0x80; z->re2z_f10e = 0x0004;
        re15_re2z_hit_filter_apply(RE15_ACTOR_SLOT_PLAYER + 1);
        int hit = feuer(3);
        CHECK(52, hit == 0 && z->hp == 250, "nur Gate 2 (+0x1D3 = 0x80): KEIN Treffer (hit=%d)", hit);
        z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 2500, 0, 250, 2048);
        z->sub_state_1 = 8; z->grid_id = 0x06; z->re2z_self1d3 = 0x00; z->re2z_f10e = 0x4004;
        re15_re2z_hit_filter_apply(RE15_ACTOR_SLOT_PLAYER + 1);
        hit = feuer(3);
        CHECK(53, hit == 0 && z->hp == 250, "nur Gate 4 (+0x10E & 0x4000): KEIN Treffer (hit=%d)", hit);
    }
    /* Liegender EXEC[7], nicht passiv (Deskriptor 0x08 ohne 0x80): bisherige Ausnahme bleibt (nicht Teil des Auftrags). */
    {
        spieler(0, 0, 0, 0);
        re15_actor_t *z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 2500, 0, 250, 2048);
        z->sub_state_1 = 7; z->grid_id = 0x08; z->re2z_self1d3 = 0x80; z->re2z_f10e = 0x4002;
        re15_re2z_hit_filter_apply(RE15_ACTOR_SLOT_PLAYER + 1);
        CHECK(54, (z->hit_react & 1u) == 0u, "Liegender EXEC[7] (0x08): Ausnahme unveraendert, Kandidat frei (hr=%02x)", z->hit_react);
        /* Passiver Liegender 0x88: gesperrt (Runde 16). */
        z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 2500, 0, 250, 2048);
        z->sub_state_1 = 7; z->grid_id = 0x88; z->re2z_self1d3 = 0x80; z->re2z_f10e = 0x4002;
        re15_re2z_hit_filter_apply(RE15_ACTOR_SLOT_PLAYER + 1);
        CHECK(55, (z->hit_react & 1u) != 0u, "Passiver Liegender 0x88: gesperrt (hr=%02x)", z->hit_react);
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
}

int main(void)
{
    teil_a(); teil_b();
    if (s_fails) { printf("test_r36_schuss: %d FAILURES\n", s_fails); return 1; }
    printf("test_r36_schuss: OK\n");
    return 0;
}
