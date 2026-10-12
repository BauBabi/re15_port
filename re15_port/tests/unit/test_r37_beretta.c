/* test_r37_beretta.c — Runde 37 Spur G: der 3-Schuss-Feuerstoss der Beretta M93R (5) / Glock 18 (6) trifft
 * JE KUGEL (Nutzer-Nachtrag 2026-10-12). Belege: include/re15_beretta_r37.h, Dossier
 * analysis/befunde_runde37/G_beretta.md.
 *
 * Misst die MECHANIK am echten Resolver re15_player_weapon_fire (FUN_80011f50) mit der echten ROOM1140.RDT
 * (Sichtlinie FUN_8001b9b4 aktiv). Leon (200,-18200) Blick -z (rot 1024) hinter dem Konferenztisch (Kante -17450),
 * Ziel bei (200,-19600), zweites Ziel (200,-21600) — dieselbe freie Bahn wie die exe-Messung (Plaetze 5/4) und
 * integration_r36_schuss "frei".
 *
 *  A  Kugel-Takt: Kugel 2/3 in Tick 3/6 (`addiu v0,zero,3/6` @0x80048914/1c, ESP-Routine-15-Kette Warte
 *     0/2/2 @CORE00.ESP 0x1188/0x11B0/0x11D8); Ticks 1/2/4/5/7.. loesen nichts auf.
 *  B  Drei Treffer auf DENSELBEN Zombie im RE1.5-Modus: der RE1.5-Riegel +0x93 Bit 0 (`ori v0,v0,0x1`
 *     @0x800124F0) aus Kugel 1 haelt Kugel 2 OHNE Loesen ab (Negativkontrolle: direkter zweiter Resolve = 0);
 *     re15_beretta_r37 loest ihn fuer DIESEN Feuerstoss -> 3 Treffer, Zaehler riegel_geloest = 2.
 *  C  Schaden je Kugel = Spalte 3 (Einzelschuss-Pistole): RE2-Import-Zeile 16 (= Zeile 4 == Zeile 3,
 *     @0x800A4168/@0x800A4154 w0 0x00E03C10); RE1.5-Zeile (Import AUS) 5 statt 15 (@0x8006e0d0 + 0x10*0x58
 *     + 3*4 / + 5*4) -> 3 Kugeln = 15 = der RE1.5-Feuerstoss; Hund 0x20 (RE1.5-KI) 10 statt 24.
 *  D  Patronen: Kugel k nur mit eigener Patrone (RE2 `jal 0x80069f54` @0x80048938): 1 Patrone -> 1 Kugel,
 *     2 -> 2, 3 -> 3.
 *  E  Abbruch: Rueckstoss vorbei (tick_ex(0)) -> keine weitere Kugel; Zaehler abgebrochen.
 *  F  Sterbender Gegner: Kugel 1 toetet (HP < 0) -> der Riegel bleibt (`bltz` @0x80012520-2c), Kugel 2/3
 *     treffen ihn NICHT erneut (HP bleibt), kein zweites Ziel -> 0.
 *  G  Andere Waffen (3 Browning, 7 Redhawk, 12 Ingram, 19 MC51) starten keinen Feuerstoss.
 *  H  Glock 18 (6) wie M93R (gleicher Handler 0x800338A8, @0x80074118).
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
#include "re15_beretta_r37.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

extern re15_actor_t g_actors[];

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

static void spieler(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->hp = 100; pl->state = 1; pl->x = 200; pl->y = 0; pl->z = -18200; pl->rot_y = 1024;
    pl->floor = 0;
    re15_player_apply_hitbox(pl);
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) memset(&g_actors[s], 0, sizeof g_actors[s]);
}
static re15_actor_t *mk(int slot, uint8_t type, int32_t z, int16_t hp)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = type; e->x = 200; e->y = 0; e->z = z; e->hp = hp;
    e->state = 1; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, type);
    e->grid_id = 0; e->aim_band = 2;            /* aufrecht / LEVEL (wie test_re2_hp_model arm()) */
    return e;
}
/* Eintrittsbild wie game_step: Resolve (Kugel 1, @0x8003396c), dann der Feuerstoss mit n Patronen. */
static int eintritt(int w, int patronen)
{
    re15_inv_init(); re15_inv_grant(w, 15); re15_player_set_equipped_weapon(w);
    int r1 = re15_player_weapon_fire(w);
    re15_beretta_r37_feuerstoss(w, patronen, r1);
    return r1;
}
/* n Ticks "im Feuer"; zaehlt die Ticks mit Aufloesung (Rueckgabe != 0) und merkt die Tick-Nummern. */
static int ticks(int n, int *bei, int nmax)
{
    int k = 0;
    for (int t = 1; t <= n; t++) {
        int r = re15_beretta_r37_tick_ex(1);
        if (r && k < nmax) bei[k++] = t;
    }
    return k;
}

int main(void)
{
    uint8_t *rdt = rdt_laden("ROOM1140");
    CHECK(1, g_room_rdt_ok, "ROOM1140.RDT geladen");
    if (!g_room_rdt_ok) { printf("test_r37_beretta: %d FAIL\n", s_fails); return 1; }
    const int Z = RE15_ACTOR_SLOT_PLAYER + 1;
    re15_beretta_r37_mess_t m;

    /* ===================== A + B: RE1.5-Modus, Import AN (Zombie traegt die RE2-Zeile) ===================== */
    printf("== A/B Kugel-Takt und drei Treffer (RE1.5-KI, Riegel +0x93)\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re15_re2z_import_set(1);
    re15_beretta_r37_reset();
    spieler();
    re15_actor_t *z = mk(Z, 0x10, -19600, 250);
    int r1 = eintritt(5, 3);
    CHECK(10, r1 == Z + 1 && z->hp == 234 && (z->hit_react & 1),
          "Kugel 1 trifft (r=%d hp=%d +0x93=%02x), Riegel gesetzt", r1, z->hp, z->hit_react);
    CHECK(11, re15_beretta_r37_offen() == 2, "zwei Kugeln offen (%d)", re15_beretta_r37_offen());
    {   /* Negativkontrolle: OHNE Loesen haelt der RE1.5-Riegel einen direkten zweiten Resolve ab. */
        re15_actor_t sicher = *z;
        int r = re15_player_weapon_fire(5);
        CHECK(12, r == 0 && z->hp == 234, "ohne Loesen: zweiter Resolve trifft NICHT (r=%d hp=%d) — "
              "der RE1.5-Riegel ist das Hindernis", r, z->hp);
        *z = sicher;
    }
    int bei[8] = {0};
    int n = ticks(8, bei, 8);
    re15_beretta_r37_mess(&m);
    CHECK(13, n == 2 && bei[0] == 3 && bei[1] == 6, "Kugel 2/3 in Tick 3/6 (n=%d bei %d/%d)", n, bei[0], bei[1]);
    CHECK(14, z->hp == 202, "drei Treffer auf denselben Zombie: hp 250 -> %d (erwartet 202 = 3 x 16)", z->hp);
    CHECK(15, m.feuerstoesse == 1 && m.kugeln == 3 && m.treffer == 3 && m.riegel_geloest == 2,
          "Zaehler: Stoesse %u Kugeln %u Treffer %u Riegel %u (erwartet 1/3/3/2)",
          m.feuerstoesse, m.kugeln, m.treffer, m.riegel_geloest);
    CHECK(16, z->state == 2 && z->sub_state_1 == 5, "Reaktion je Treffer: state %d (+0x4=2 @0x80012520), +0x5 %d (= Waffe 5)",
          z->state, z->sub_state_1);
    CHECK(17, re15_beretta_r37_offen() == 0 && re15_beretta_r37_tick_ex(1) == 0, "danach nichts mehr offen");

    /* ===================== C: Schaden je Kugel ===================== */
    printf("== C Schaden je Kugel = Spalte 3\n");
    re15_re15_re2z_import_set(0);                      /* RE1.5-Zeile @0x8006e0d0 + 0x10*0x58 */
    re15_beretta_r37_reset();
    spieler();
    z = mk(Z, 0x10, -19600, 250);
    eintritt(5, 3);
    CHECK(20, z->hp == 245, "RE1.5-Zeile Zombie: Kugel 1 = 5 (Spalte 3), nicht 15 (Spalte 5): hp %d", z->hp);
    ticks(8, bei, 8);
    CHECK(21, z->hp == 235, "drei Kugeln = 15 = der RE1.5-Feuerstoss (3 x 5): hp %d", z->hp);
    re15_beretta_r37_reset();
    spieler();
    z = mk(Z, 0x20, -19600, 500);                       /* Hund (RE1.5-Zeile): Spalte 3 = 10, Spalte 5 = 24 */
    eintritt(5, 3);
    CHECK(22, z->hp == 490, "Hund 0x20 (RE1.5-Zeile @0x8006e0d0+0x20*0x58): Kugel 1 = 10 (Spalte 3), nicht 24 "
          "(Spalte 5): hp %d", z->hp);
    re15_re15_re2z_import_set(1);
    re15_beretta_r37_reset();
    spieler();
    z = mk(Z, 0x10, -19600, 250);
    eintritt(6, 3);
    ticks(8, bei, 8);
    re15_beretta_r37_mess(&m);
    CHECK(23, z->hp == 202 && m.treffer == 3, "H Glock 18 (6): drei Treffer, hp %d, Treffer %u", z->hp, m.treffer);

    /* ===================== D: Patronen ===================== */
    printf("== D Patronen je Kugel\n");
    for (int p = 1; p <= 3; p++) {
        re15_beretta_r37_reset();
        spieler();
        z = mk(Z, 0x10, -19600, 250);
        eintritt(5, p);
        ticks(8, bei, 8);
        re15_beretta_r37_mess(&m);
        CHECK(30 + p, (int)m.kugeln == p && z->hp == 250 - 16 * p,
              "%d Patrone(n) -> %u Kugel(n), hp %d (erwartet %d)", p, m.kugeln, z->hp, 250 - 16 * p);
    }

    /* ===================== E: Abbruch ===================== */
    printf("== E Abbruch mit dem Rueckstoss\n");
    re15_beretta_r37_reset();
    spieler();
    z = mk(Z, 0x10, -19600, 250);
    eintritt(5, 3);
    (void)re15_beretta_r37_tick_ex(1);
    (void)re15_beretta_r37_tick_ex(0);                 /* Treffer/Griff/Raumwechsel loeschen den Rueckstoss */
    int nach = ticks(8, bei, 8);
    re15_beretta_r37_mess(&m);
    CHECK(40, nach == 0 && z->hp == 234 && m.abgebrochen == 1 && m.kugeln == 1,
          "nach dem Abbruch keine Kugel mehr (n=%d hp=%d abgebrochen=%u kugeln=%u)", nach, z->hp, m.abgebrochen, m.kugeln);

    /* ===================== F: sterbender Gegner ===================== */
    printf("== F Sterbender Gegner bekommt keine zweite Kugel\n");
    re15_beretta_r37_reset();
    spieler();
    z = mk(Z, 0x10, -19600, 10);
    eintritt(5, 3);
    CHECK(50, z->hp < 0 && z->state == 3, "Kugel 1 toetet (hp %d state %d)", z->hp, z->state);
    int16_t hp_tot = z->hp;
    n = ticks(8, bei, 8);
    re15_beretta_r37_mess(&m);
    CHECK(51, n == 0 && z->hp == hp_tot && z->state == 3 && m.riegel_geloest == 0,
          "Kugel 2/3 treffen den Sterbenden NICHT (n=%d hp %d state %d Riegel %u)", n, z->hp, z->state, m.riegel_geloest);
    {   /* mit einem zweiten Zombie dahinter: Kugel 2/3 gehen auf ihn (Rueckgriff @0x80012404-18) */
        re15_beretta_r37_reset();
        spieler();
        re15_actor_t *z2 = mk(Z + 1, 0x10, -21600, 250);
        int frei = re15_player_weapon_fire(3);       /* Kontrolle: der zweite Platz ist allein treffbar */
        CHECK(52, frei == Z + 2 && z2->hp == 234, "Kontrolle: Platz 2 allein treffbar (r=%d hp=%d)", frei, z2->hp);
        spieler();
        z = mk(Z, 0x10, -19600, 10);
        z2 = mk(Z + 1, 0x10, -21600, 250);
        eintritt(5, 3);
        CHECK(53, z->hp < 0 && z2->hp == 250, "Kugel 1 toetet den vorderen (hp %d), der hintere unberuehrt (hp %d)",
              z->hp, z2->hp);
        ticks(8, bei, 8);
        CHECK(54, z2->hp == 218, "Kugel 2/3 treffen den naechsten Zombie (hp %d, erwartet 218)", z2->hp);
    }

    /* ===================== G: andere Waffen ===================== */
    printf("== G Andere Waffen starten keinen Feuerstoss\n");
    static const int W[4] = { 3, 7, 12, 19 };
    for (int k = 0; k < 4; k++) {
        re15_beretta_r37_reset();
        spieler();
        z = mk(Z, 0x10, -19600, 2500);
        eintritt(W[k], 3);
        int a = ticks(8, bei, 8);
        CHECK(60 + k, re15_beretta_r37_offen() == 0 && a == 0 && !re15_beretta_r37_ist_feuerstoss(W[k]),
              "Waffe %d: kein Feuerstoss (offen %d, Kugeln %d)", W[k], re15_beretta_r37_offen(), a);
    }
    CHECK(64, re15_beretta_r37_ist_feuerstoss(5) && re15_beretta_r37_ist_feuerstoss(6),
          "5 und 6 sind Feuerstoss-Waffen (@0x80074114/18 -> 0x800338A8)");

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re15_re2z_import_set(1);
    free(rdt);
    printf(s_fails ? "test_r37_beretta: %d FAIL\n" : "test_r37_beretta: OK\n", s_fails);
    return s_fails ? 1 : 0;
}
