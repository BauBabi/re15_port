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
 *   L  Blutfarbe (Nachbesserung 1): FUN_80019700 legt das GANZE Sub-Byte ab (@0x80019730/@0x800197d8), die CLUT
 *      steigt um (Sub >> 3) * 0x40 (@0x8001973c/54/84/88). Spinnenblut 0x82000 (@0x80113b10/30/50, @0x8011463c-54)
 *      -> CLUT 0x7991 = TEX.TIM y 486 gruen; Zombie 0x2000 -> 0x7951 = y 485 rot. RE2-Gehirn: die Blut-Records 0/1/3
 *      von FUN_801056DC (@0x80106684, Sub 0x19/0x09, nie die rote Grundzeile y 488) nehmen den Spinnen-Code.
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
#include "fx_plattform_pc.h"   /* re15_pc_esp_clut / re15_pc_fx_seite_clut_ok: der Zeichner (platform/pc/src) */
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

/* ---- L: Blutfarbe (Runde 41 Spur S, Nachbesserung 1) ------------------------------------------------------- */
/* Platzhalter fuer die Zusatzbank (die echte Funktion steht in audio_pc.c; fx_plattform_pc.c ruft sie, L nicht). */
void re15_audio_arms_zusatz_se(int arms_id, int satz) { (void)arms_id; (void)satz; }
static uint8_t *datei(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long l = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)l);
    if (b && fread(b, 1, (size_t)l, f) != (size_t)l) { free(b); b = NULL; }
    fclose(f);
    if (b && n) *n = (size_t)l;
    return b;
}
static uint32_t w32(const uint8_t *b, size_t o) { return (uint32_t)b[o] | ((uint32_t)b[o+1] << 8) | ((uint32_t)b[o+2] << 16) | ((uint32_t)b[o+3] << 24); }
static uint16_t w16(const uint8_t *b, size_t o) { return (uint16_t)(b[o] | (b[o+1] << 8)); }
/* EXE-Wort: Datei 0x800 + (addr - t_addr @0x18). */
static uint32_t exe_wort(const uint8_t *b, size_t n, uint32_t a)
{
    size_t o = 0x800u + (size_t)(a - w32(b, 0x18));
    return (o + 4 <= n) ? w32(b, o) : 0xdeadbeefu;
}
/* CLUT-Wort des Effekts 0 im Kopf einer CORE00.ESP (Id-Kopf ab 0, Zeigertabelle abwaerts ab round_up4(n)-4). */
static int core_clut0(const uint8_t *b, size_t n)
{
    size_t pe = ((n + 3u) & ~(size_t)3u) - 4u;
    for (int i = 0; i < 8 && b[i] != 0xFF; i++)
        if (b[i] == 0) { size_t s = (size_t)(int32_t)w32(b, pe - 4u * (size_t)i); return (s + 6 <= n) ? w16(b, s + 4) : -1; }
    return -1;
}
/* TIM-CLUT-Eintrag (x, y) als 15-Bit-Farbe; -1, wenn ausserhalb des CLUT-Blocks. */
static int tim_clut(const uint8_t *b, size_t n, int x, int y)
{
    int cx = w16(b, 12), cy = w16(b, 14), cw = w16(b, 16), ch = w16(b, 18);
    if (x < cx || x >= cx + cw || y < cy || y >= cy + ch) return -1;
    size_t o = 20u + ((size_t)(y - cy) * (size_t)cw + (size_t)(x - cx)) * 2u;
    return (o + 2 <= n) ? w16(b, o) : -1;
}
#define C5R(c) (((c) & 31) << 3)
#define C5G(c) ((((c) >> 5) & 31) << 3)
#define C5B(c) ((((c) >> 10) & 31) << 3)
static int ist_rot(int c)   { return c >= 0 && C5R(c) > 2 * C5G(c) && C5R(c) > 2 * C5B(c); }
static int ist_gruen(int c) { return c >= 0 && C5G(c) > 2 * C5R(c) && C5G(c) > 2 * C5B(c); }

/* Zaehlt die aktiven Effekt-0-Plaetze: gruen = Sub-Byte 8 + Zeichner-CLUT 0x7991, rot = Zeichner-CLUT 0x7951. */
static void fx_zaehlen(int *gruen, int *rot, int *alt1500)
{
    *gruen = *rot = *alt1500 = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || f->effect_id != 0) continue;
        uint16_t c = re15_pc_esp_clut(f);
        if (f->sub_index == 8 && c == 0x7991) (*gruen)++;
        if (c == 0x7951) (*rot)++;
        if (f->scale16 == 0x1500) (*alt1500)++;
    }
}

static void test_blutfarbe(void)
{
    size_t n = 0;
    /* L1 Byte-Pins RE1.5 PSX.EXE FUN_80019700 (Mod = Disc, Dossier S Nachbesserung 1) */
    uint8_t *exe = datei(RE15_REPO_INFO "/Re1.5/PSX.EXE", &n);
    CHECK(exe != NULL, "L1: info/Re1.5/PSX.EXE fehlt");
    if (exe) {
        static const struct { uint32_t a, w; const char *t; } P[] = {
            { 0x80019730u, 0x304f00ffu, "andi t7,v0,0xff" },  { 0x80019734u, 0x31e20007u, "andi v0,t7,0x7" },
            { 0x8001973cu, 0x000f18c2u, "srl v1,t7,3" },      { 0x80019754u, 0x00038180u, "sll s0,v1,6" },
            { 0x800197d8u, 0xa10f0071u, "sb t7,113(t0)" },    { 0x80019884u, 0x00501021u, "addu v0,v0,s0" },
            { 0x80019888u, 0xa5020032u, "sh v0,50(t0)" } };
        for (size_t i = 0; i < sizeof P / sizeof P[0]; i++)
            CHECK(exe_wort(exe, n, P[i].a) == P[i].w, "L1: @0x%08x nicht `%s` (%08x)", P[i].a, P[i].t, exe_wort(exe, n, P[i].a));
        free(exe);
    }
    /* L1 STAGE2: Spinnenblut a0 = 0x00082000 (HURT @0x80113b50, Tod der schwachen Waffen @0x80114654) */
    uint8_t *st2 = datei(RE15_ASSET_PSX_DIR "/BIN/STAGE2.BIN", &n);
    CHECK(st2 != NULL && n > 0x14700, "L1: STAGE2.BIN");
    if (st2 && n > 0x14700) {
        CHECK(rd32(st2, 0x80113b10u) == 0x3c040008u && rd32(st2, 0x80113b30u) == 0x34842000u
              && rd32(st2, 0x80113b50u) == 0x0c0065c0u, "L1: HURT-Blut @0x80113b10/30/50 nicht lui 8 / ori 0x2000 / jal 0x80019700");
        CHECK(rd32(st2, 0x8011463cu) == 0x3c040008u && rd32(st2, 0x80114640u) == 0x34842000u
              && rd32(st2, 0x80114654u) == 0x0c0065c0u, "L1: Todesblut @0x8011463c/40/54 nicht lui 8 / ori 0x2000 / jal");
    }
    free(st2);
    /* L1 RE2 EMS25.BIN: Records @0x80106684 (0, 1, 3 = Effekt 0, Sub 0x19/0x19/0x09), generischer Treffer Record 0 */
    uint8_t *ems = datei(RE15_REPO_INFO "/re2leon/COMMON/BIN/EMS25.BIN", &n);
    CHECK(ems != NULL && n > 0x66a0, "L1: info/re2leon/COMMON/BIN/EMS25.BIN");
    if (ems && n > 0x66a0) {
        const uint8_t *r = ems + 0x6684;
        CHECK(r[0] == 0 && r[1] == 0x19 && w16(r, 4) == 0x2000, "L1: RE2 Record 0 nicht Effekt 0 Sub 0x19 Skala 0x2000");
        CHECK(r[6] == 0 && r[7] == 0x19 && w16(r, 10) == 0x2800, "L1: RE2 Record 1 nicht Effekt 0 Sub 0x19 Skala 0x2800");
        CHECK(r[18] == 0 && r[19] == 0x09 && w16(r, 22) == 0x1000, "L1: RE2 Record 3 nicht Effekt 0 Sub 0x09 Skala 0x1000");
        CHECK(rd32(ems, 0x80102dd0u) == 0x0c0415b7u && rd32(ems, 0x80102dd4u) == 0x00003021u,
              "L1: RE2 @0x80102dd0 nicht `jal 0x801056dc` / `addu a2,zero,zero` (%08x %08x)",
              rd32(ems, 0x80102dd0u), rd32(ems, 0x80102dd4u));
    }
    free(ems);
    /* L2 Daten: Effekt-0-CLUT und Paletten in RE1.5 und RE2 */
    size_t nc = 0, nt = 0, nc2 = 0, nt2 = 0;
    uint8_t *core = datei(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &nc);
    uint8_t *tex  = datei(RE15_ASSET_PSX_DIR "/DATA/TEX.TIM", &nt);
    uint8_t *core2 = datei(RE15_ASSET_PSX_DIR "/../RE2/CORE00.ESP", &nc2);
    uint8_t *tex2  = datei(RE15_ASSET_PSX_DIR "/../RE2/TEX.TIM", &nt2);
    CHECK(core && tex && core2 && tex2, "L2: CORE00.ESP / TEX.TIM (RE1.5 und RE2) fehlen");
    if (core && tex && core2 && tex2) {
        CHECK(core_clut0(core, nc) == 0x7951, "L2: RE1.5 Effekt 0 CLUT %04x statt 0x7951 (x 272 y 485)", core_clut0(core, nc));
        CHECK(ist_rot(tim_clut(tex, nt, 273, 485)), "L2: RE1.5 TEX.TIM y 485 nicht rot (%04x)", tim_clut(tex, nt, 273, 485));
        CHECK(ist_gruen(tim_clut(tex, nt, 273, 486)), "L2: RE1.5 TEX.TIM y 486 nicht gruen (%04x)", tim_clut(tex, nt, 273, 486));
        CHECK(core_clut0(core2, nc2) == 0x7a11, "L2: RE2 Effekt 0 CLUT %04x statt 0x7a11 (y 488)", core_clut0(core2, nc2));
        CHECK(ist_rot(tim_clut(tex2, nt2, 273, 488)), "L2: RE2 TEX.TIM y 488 nicht rot");
        CHECK(!ist_rot(tim_clut(tex2, nt2, 273, 488 + (0x19 >> 3))) && !ist_rot(tim_clut(tex2, nt2, 273, 488 + (0x09 >> 3))),
              "L2: RE2-Spinnenblut (y 491 / y 489) ist rot");
    }
    /* Global-Bank = CORE00.ESP wie im Spiel (Effekt 0 liegt nicht in der Raumbank) */
    static re15_esp_t bank;
    int bank_ok = core && re15_esp_parse_global(core, nc, &bank) == 0;
    CHECK(bank_ok, "L2: CORE00.ESP nicht parsebar");
    if (!bank_ok) { free(core); free(tex); free(core2); free(tex2); return; }
    re15_esp_set_global_bank(&bank);
    re15_esp_set_room_bank(NULL);
    int g, r, a;
    /* L3 Helfer: Spinnenblut gruen, Zombieblut rot */
    frisch(RE15_AI_FLAVOR_RE2);
    re15_enemy_fx19700(SP, 0x00082000u, 0, NULL);
    fx_zaehlen(&g, &r, &a);
    CHECK(g == 1 && r == 0, "L3: re15_enemy_fx19700(0x82000): gruen %d rot %d (Sub-Byte geht verloren)", g, r);
    CHECK(re15_pc_fx_seite_clut_ok(0x7991), "L3: CLUT 0x7991 liegt nicht in der hochgeladenen Effektseite");
    frisch(RE15_AI_FLAVOR_RE2);
    re15_enemy_fx19700(SP, 0x00002000u, 0, NULL);
    fx_zaehlen(&g, &r, &a);
    CHECK(g == 0 && r == 1, "L3: Zombie 0x2000: gruen %d rot %d (muss rot bleiben)", g, r);
    /* L4 RE2-Gehirn, generischer Treffer w3 (Zeile 3 -> @0x80102DD0 Record 0) */
    re15_spinnengore_test_schalter(1);
    re2_spinne(2, 3, 0x01); re2_takt();
    fx_zaehlen(&g, &r, &a);
    CHECK(g >= 1 && r == 0 && a == 0, "L4: RE2-Treffer w3: gruen %d rot %d alt-0x1500 %d", g, r, a);
    /* L5 RE2-Tod w7 (Zeile 5): Hebung (3x 0x82000) + Record 1 @0x80104580 + FUN_80104890 @0x80104588 */
    re2_spinne(3, 7, 0x41); re2_takt();
    fx_zaehlen(&g, &r, &a);
    CHECK(g == 5 && r == 0 && a == 0, "L5: RE2-Tod w7: gruen %d (Soll 5) rot %d alt-0x1500 %d", g, r, a);
    /* L6 derselbe Tod mit RE15_KEIN_80PROZENT_GORE: die Hebung fehlt, das RE2-Blut bleibt gruen (keine 80%-Sache) */
    re15_spinnengore_test_schalter(0);
    re2_spinne(3, 7, 0x41); re2_takt();
    fx_zaehlen(&g, &r, &a);
    CHECK(g == 2 && r == 0, "L6: Schalter aus: gruen %d (Soll 2) rot %d", g, r);
    re15_spinnengore_test_schalter(1);
    /* L7 RE1.5-Gehirn: Tod w3 (0x8011461c, Todesblut an Teil 0 @0x80114654) */
    frisch(RE15_AI_FLAVOR_RE15);
    SP->state = 3; SP->sub_state_1 = 3; SP->sub_state_3 = 0; SP->hp = -5; SP->hit_react = 0x01;
    re15_enemy_ai_run_all(1);
    fx_zaehlen(&g, &r, &a);
    CHECK(g >= 1 && r == 0, "L7: RE1.5-Tod w3: gruen %d rot %d", g, r);
    re15_esp_set_global_bank(NULL);
    free(core); free(tex); free(core2); free(tex2);
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
    test_blutfarbe();
    if (fails) { printf("%d Fehler\n", fails); return 1; }
    printf("OK unit_r41_spinnengore\n");
    return 0;
}
