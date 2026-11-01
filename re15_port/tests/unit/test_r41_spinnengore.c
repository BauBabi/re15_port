/* test_r41_spinnengore.c — Runde 41 Spur S "spinnengore" (Dossier analysis/befunde_runde41/S_spinnengore.md).
 *
 * Nutzer: "die spinnen platzen bei magnum Schuss z.b. immer noch nicht". Gemessen (Dossier Teil 1): unter der
 * Standard-KI gehoert jede 0x25 dem RE2-Gehirn, das Krit-Bit kommt an (+0x93 = 0x41), die Teilschicht bleibt aus.
 *
 * Modus 1 (ohne Argument) = Unit-Riegel:
 *   A  Byte-Pins aus STAGE2.BIN: Waffentabelle @0x80118f38 == re15_spinnengore_platzwaffe, `ori v0,a0,0x1062`
 *      @0x80114220, `ori t0,t0,0x7f60` @0x80113c98, `andi v0,v0,0x40` @0x80113b74.
 *   B  RE2-Gehirn, Tod mit Redhawk (w7, Krit 0x41): Teile 0/19 platzen gruen, alle Beine 3..18 fliegen (0x1062,
 *      Schwerkraft 20, vy -100, Drehung -128/+8) — Zahlen aus 0x8011403c.
 *   C  dasselbe mit RE15_KEIN_80PROZENT_GORE (Test-Schalter 0): Teilschicht bleibt aus (Gegenprobe der Hebung).
 *   D  RE2-Tod mit Browning (w3, kein Krit): kein Platzen.
 *   E  RE2-Tod mit GL HE (w15: RE1.5 schwach, RE2 Zeile 9 = Hinterleib-Platzer @0x80104A2C): Platzen.
 *   F  RE2-Treffer Remington (w8 -> Zeile 7, FUN_80105BF0): das von RE2 abgetrennte Bein k wird als Paar
 *      2k+3/2k+4 gezeichnet.
 *   G  RE2-Treffer mit Krit, RE2 verweigert (+0x221 < 3 @0x80105c20): das RE1.5-Krit-Bein (@0x80113b74).
 *   H  RE1.5-Gehirn: Tod mit Handgranate (w9, kein Krit) platzt (Tabelle @0x80118f38 -> 0x80113ff8); w3 nicht.
 *   I  re15_actor_alloc leert die Teilschicht des Platzes (Babys erben keine geplatzten Teile).
 * Modus 2 `pruef_log <log> <art>` = Auswerter des Integrationslaufs (RE15_SPINNEN_LOG mit {G..}-Klammern):
 *   art "platzt": eine RE2-Spinne (o=1) stirbt (st=3) und traegt danach sh mit Teil 0 und 19 und fl == 0x7fff8.
 *   art "bein":   eine RE2-Spinne im Treffer (st=2) traegt ein geplatztes Beinpaar (sh-Bits in 3..18).
 *   art "aus":    eine RE2-Spinne stirbt, die Teilschicht bleibt aus (ta=0 in jedem Bild).
 */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_teile_r15.h"
#include "re15_spinnengore_r41.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void re15_aspider_beine_reset(void);
int  re15_re2spider_tick(int slot);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

#define S 1
static re15_actor_t *PL, *SP;

static void frisch(re15_ai_flavor_t fl)
{
    re15_actor_init();
    re15_ai_flavor_set(fl);
    PL = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    SP = &g_actors[S];
    memset(PL, 0, sizeof *PL); memset(SP, 0, sizeof *SP);
    PL->active = 1; PL->hp = 100; PL->x = 0; PL->z = -3000;
    SP->active = 1; SP->type = 0x25; SP->em_flag_id = 0xFF;
    re15_teile_reset(S);
    re15_aspider_beine_reset();
    re15_esp_fx_reset();
}

static void re2_takt(void)
{
    SP->ai_dist = (uint32_t)re15_enemy_player_dist(SP, PL);
    re15_re2spider_tick(S);
}

/* INIT des RE2-Gehirns laufen lassen (+0x221 = 8 @0x80100374), dann Zustand setzen. */
static void re2_spinne(uint8_t state, uint8_t waffe, uint8_t bits)
{
    frisch(RE15_AI_FLAVOR_RE2);
    SP->state = 0; re2_takt();
    SP->state = state; SP->sub_state_1 = waffe; SP->sub_state_2 = 0; SP->sub_state_3 = 0;
    SP->hp = (state == 3) ? -20 : 50; SP->hit_react = bits;
    re15_teile_reset(S);
}

static int platzt_ganz(void)
{
    re15_teil_t *t = re15_teile(S);
    if (!re15_teile_an(S)) return 0;
    if ((t[0].w & 0x4a) != 0x4a || (t[19].w & 0x4a) != 0x4a) return 0;
    if (t[0].col != 0x207f60u || t[19].col != 0x207f60u) return 0;
    for (int k = 3; k <= 18; k++)
        if ((t[k].w & 0x1062) != 0x1062 || t[k].grav != 20 || t[k].v[1] != -100 || t[k].spin != -128 || t[k].spin_inc != 8)
            return 0;
    return 1;
}
static int paare_geplatzt(void)
{
    re15_teil_t *t = re15_teile(S);
    int n = 0;
    for (int k = 3; k <= 17; k += 2)
        if ((t[k].w & 0x4a) == 0x4a && (t[k + 1].w & 0x4a) == 0x4a && t[k].col == 0x207f60u) n++;
    return n;
}

static uint32_t rd32(const uint8_t *b, uint32_t addr) { uint32_t o = addr - 0x80100000u;
    return (uint32_t)b[o] | ((uint32_t)b[o+1] << 8) | ((uint32_t)b[o+2] << 16) | ((uint32_t)b[o+3] << 24); }

static void test_bytes(void)
{
    FILE *f = fopen(RE15_ASSET_PSX_DIR "/BIN/STAGE2.BIN", "rb");
    CHECK(f != NULL, "STAGE2.BIN fehlt");
    if (!f) return;
    static uint8_t b[0x30000];
    size_t n = fread(b, 1, sizeof b, f); fclose(f);
    CHECK(n > 0x18f90, "STAGE2.BIN zu kurz (%u)", (unsigned)n);
    if (n <= 0x18f90) return;
    for (int w = 0; w < 22; w++) {
        uint32_t ziel = rd32(b, 0x80118f38u + 4u * (uint32_t)w);
        CHECK(ziel == 0x80113ff8u || ziel == 0x8011461cu, "@0x80118f38[%d] = %08x", w, ziel);
        CHECK(re15_spinnengore_platzwaffe(w) == (ziel == 0x80113ff8u), "platzwaffe(%d) != Tabelle (%08x)", w, ziel);
    }
    CHECK(rd32(b, 0x80114220u) == 0x34821062u, "@0x80114220 nicht `ori v0,a0,0x1062` (%08x)", rd32(b, 0x80114220u));
    CHECK(rd32(b, 0x80113c98u) == 0x35087f60u, "@0x80113c98 nicht `ori t0,t0,0x7f60` (%08x)", rd32(b, 0x80113c98u));
    CHECK(rd32(b, 0x80113b74u) == 0x30420040u, "@0x80113b74 nicht `andi v0,v0,0x40` (%08x)", rd32(b, 0x80113b74u));
}

static void test_re2(void)
{
    /* B */
    re15_spinnengore_test_schalter(1);
    re2_spinne(3, 7, 0x41); re2_takt();
    CHECK(platzt_ganz(), "B: RE2-Tod w7 Krit: Spinne platzt nicht (ta=%d w0=%04x w19=%04x w3=%04x)",
          re15_teile_an(S), re15_teile(S)[0].w, re15_teile(S)[19].w, re15_teile(S)[3].w);
    CHECK(SP->re2s_row23c == 5, "B: Zeile %d statt 5", SP->re2s_row23c);
    /* C */
    re15_spinnengore_test_schalter(0);
    re2_spinne(3, 7, 0x41); re2_takt();
    CHECK(!re15_teile_an(S), "C: Schalter aus, Teilschicht trotzdem an");
    re15_spinnengore_test_schalter(1);
    /* D */
    re2_spinne(3, 3, 0x01); re2_takt();
    CHECK(!re15_teile_an(S) || (re15_teile(S)[0].w & 0x4a) != 0x4a, "D: Browning ohne Krit platzt");
    /* E */
    re2_spinne(3, 15, 0x01); re2_takt();
    CHECK(platzt_ganz(), "E: GL HE (RE2 Zeile 9) platzt nicht");
    /* F */
    re2_spinne(2, 8, 0x01); re2_takt();
    {
        int k = -1; for (int j = 0; j < 8; j++) if ((SP->re2s_legs220 >> j) & 1u) k = j;
        CHECK(k >= 0, "F: RE2 trennt mit w8 (Zeile 7) kein Bein ab (+0x220 = %02x)", SP->re2s_legs220);
        if (k >= 0) {
            re15_teil_t *t = re15_teile(S);
            CHECK((t[2*k+3].w & 0x4a) == 0x4a && (t[2*k+4].w & 0x4a) == 0x4a, "F: RE2-Bein %d nicht gezeichnet", k);
            CHECK(paare_geplatzt() == 1, "F: %d Paare statt 1", paare_geplatzt());
        }
    }
    /* G */
    re2_spinne(2, 7, 0x41); SP->re2s_legn221 = 2; re2_takt();
    CHECK(paare_geplatzt() == 1, "G: Krit ohne RE2-Abtrennung: %d Paare statt 1", paare_geplatzt());
    CHECK(SP->re2s_legs220 != 0, "G: Beinmaske leer");
    /* G2: ohne Krit und RE2 verweigert -> nichts */
    re2_spinne(2, 7, 0x01); SP->re2s_legn221 = 2; re2_takt();
    CHECK(paare_geplatzt() == 0, "G2: ohne Krit platzt ein Bein (%d)", paare_geplatzt());
}

static void test_re15(void)
{
    /* H: RE1.5-Gehirn, byte-true Tabelle @0x80118f38 */
    frisch(RE15_AI_FLAVOR_RE15);
    SP->state = 3; SP->sub_state_1 = 9; SP->sub_state_3 = 0; SP->hp = -5; SP->hit_react = 0x01;
    re15_enemy_ai_run_all(1);
    CHECK(platzt_ganz(), "H: RE1.5-Tod w9 (0x80113ff8) platzt nicht");
    frisch(RE15_AI_FLAVOR_RE15);
    SP->state = 3; SP->sub_state_1 = 3; SP->sub_state_3 = 0; SP->hp = -5; SP->hit_react = 0x01;
    re15_enemy_ai_run_all(1);
    CHECK(!re15_teile_an(S), "H: RE1.5-Tod w3 (0x8011461c ohne Krit) platzt");
}

static void test_alloc(void)
{
    frisch(RE15_AI_FLAVOR_RE2);
    re15_spinnengore_teil_platzt(5, 0);
    CHECK(re15_teile_an(5), "I: Vorbedingung");
    for (int i = 1; i < RE15_ACTOR_MAX; i++) g_actors[i].active = (i < 5);
    int s = re15_actor_alloc(0x26);
    CHECK(s == 5, "I: Platz %d statt 5", s);
    CHECK(!re15_teile_an(5) && re15_teile(5)[0].w == 1, "I: neuer Aktor erbt geplatzte Teile");
}

/* J: RE2-Zombie, MC51 (RE2 Zeile 18) in der Beinzone -> Zelle 7438 (RE1.5 @0x8012010c[0] = 0x80108abc). */
void re15_re2z_death_test(re15_actor_t *e, re15_actor_t *pl);
int  re15_re2z_last_death_handler(void);
int  re15_re2z_death_cell(unsigned row, unsigned col);
int  re15_re2z_gore_active(const re15_actor_t *e);
static void test_zombie_mc51(void)
{
    frisch(RE15_AI_FLAVOR_RE2);
    CHECK(re15_re2z_death_cell(18, 0) == 1, "J: Rohtabelle @0x8010CEAC Spalte 0 ist nicht MAIN (%d)", re15_re2z_death_cell(18, 0));
    re15_actor_t *e = &g_actors[3];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x10; e->hp = -1; e->em_flag_id = 0xFF;
    e->state = 3; e->sub_state_1 = 18; e->re2z_hits1d2 = 0; e->hit_react = 1;
    for (int i = 0; i < 16; i++) { e->re2z_part_flags[i] = (i < 15) ? 1u : 0u; e->re2z_part_tint[i] = 0x00808080u; e->re2z_part_mesh[i] = (uint8_t)i; }
    re15_re2z_death_test(e, PL);
    CHECK(re15_re2z_last_death_handler() == 2, "J: MC51 Beinzone Zelle %d statt 7438 (2)", re15_re2z_last_death_handler());
    e->state = 3; e->sub_state_1 = 18; e->re2z_hits1d2 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
    re15_re2z_death_test(e, PL);
    CHECK(re15_re2z_last_death_handler() == 1, "J: MC51 Rumpfzone Zelle %d statt MAIN", re15_re2z_last_death_handler());
}

/* K: RE2-Hund, Redhawk-Tod (Zeile 5, Router @0x801046EC): Teile-Wurf 0x80104440 -> Bruecke an, 7 Parts 0x4a. */
static void test_hund(void)
{
    frisch(RE15_AI_FLAVOR_RE2);
    re15_actor_t *e = &g_actors[S];
    e->type = 0x20; e->state = 0;
    re15_enemy_ai_run_all(1);
    CHECK(!re15_re2z_gore_active(e), "K: Bruecke vor dem Wurf schon an");
    e->hp = -1; e->state = 3; e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 0; e->hit_react = 0x41;
    for (int i = 0; i < 3; i++) re15_enemy_ai_run_all(1);
    CHECK(re15_re2z_gore_active(e), "K: Bruecke nach dem Wurf aus");
    static const int p[7] = { 2, 3, 4, 7, 8, 9, 10 };
    for (int i = 0; i < 7; i++)
        CHECK((e->re2z_part_flags[p[i]] & 0x4Bu) == 0x4Bu && e->re2z_part_w9c[p[i]] == 800,
              "K: Part %d %04x w9c %d", p[i], e->re2z_part_flags[p[i]], e->re2z_part_w9c[p[i]]);
    CHECK(e->re2z_part_flags[0] == 1u && e->re2z_part_tint[0] == 0x00808080u, "K: Seed Part 0 %04x %08x",
          e->re2z_part_flags[0], e->re2z_part_tint[0]);
}

/* ---- Auswerter ---------------------------------------------------------------------------------------------- */
static int pruef_log(const char *pfad, const char *art)
{
    FILE *f = fopen(pfad, "r");
    if (!f) { printf("FAIL: kein Log %s\n", pfad); return 1; }
    static char z[65536];
    int tod_re2 = 0, platzt = 0, bein = 0, ta_an = 0, zeilen = 0;
    while (fgets(z, sizeof z, f)) {
        zeilen++;
        for (const char *q = strchr(z, '['); q; q = strchr(q + 1, '[')) {
            int slot, st; unsigned t;
            if (sscanf(q, "[%d t=%x st=%d", &slot, &t, &st) != 3 || t != 0x25) continue;
            char key[16]; snprintf(key, sizeof key, "{G%d ", slot);
            const char *g = strstr(q, key);
            int o, ta; unsigned sh, fl, un, lg; int ln, zz;
            if (!g || sscanf(g + strlen(key), "o=%d ta=%d sh=%x fl=%x un=%x lg=%x ln=%d z=%d", &o, &ta, &sh, &fl, &un, &lg, &ln, &zz) != 8)
                continue;
            if (o != 1) continue;
            if (ta) ta_an = 1;
            if (st == 3) {
                tod_re2 = 1;
                if ((sh & 0x80001u) == 0x80001u && fl == 0x7fff8u) platzt = 1;
            }
            if (st == 2 && (sh & 0x7fff8u)) bein = 1;
        }
    }
    fclose(f);
    printf("pruef_log %s: Zeilen %d, RE2-Tod %d, Platzen %d, Bein %d, Teilschicht %d\n", art, zeilen, tod_re2, platzt, bein, ta_an);
    if (!strcmp(art, "platzt")) return (tod_re2 && platzt) ? 0 : 1;
    if (!strcmp(art, "bein"))   return bein ? 0 : 1;
    if (!strcmp(art, "aus"))    return (tod_re2 && !ta_an) ? 0 : 1;
    printf("FAIL: unbekannte Art %s\n", art);
    return 1;
}

int main(int argc, char **argv)
{
    if (argc >= 4 && !strcmp(argv[1], "pruef_log")) return pruef_log(argv[2], argv[3]);
    test_bytes();
    test_re2();
    test_re15();
    test_alloc();
    test_zombie_mc51();
    test_hund();
    if (fails) { printf("%d Fehler\n", fails); return 1; }
    printf("OK unit_r41_spinnengore\n");
    return 0;
}
