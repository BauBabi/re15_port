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
 *   L8 Babytod (Nachbesserung 2, Abnahme 2 Mangel 2): EMS26 @0x80100d10-14 Record = Tabelle @0x801010C7 & 0x7f
 *      (@0x80100f50); Record 0 = `00 19 00 00 00 10` -> Spinnenblut gruen, Records 3/4 (Zeilen 10/11/16) bleiben.
 *   M  Lache (Nachbesserung 2, Abnahme 2 Mangel 1): RE1.5 CORPSE 0x801153d4 (0x00DF809F, +8 je Takt, 91 bzw. 121
 *      Takte, Eintritt je Todesweg), RE2 FUN_80104DE8 (0x00BF10BF, +8 je Takt, 90 Takte, Wasser sperrt), Babyspinne
 *      (Tinte beim SPLAT, der Record bleibt nach dem DESPAWN liegen). Byte-Pins aus STAGE2/EMS25/EMS26/RE2-PSX.EXE.
 *   M9 Schatten ohne Nahgrenze (Release v0.8.30, Pruefung R1): RE1.5 FUN_8001b064 (RotAverage4 @0x8001b2c8) und RE2
 *      FUN_8001699c (RTPT/RTPS/AVSZ4 @0x80016b30-6c) haengen das Viereck ohne Z-Pruefung ein; re15_objpoly_r38_schatten
 *      verwirft nur nach GPU-Groesse je Dreieck (0,1,2)/(1,2,3), Bildpunkte vor der Kamera wie bisher.
 *   M7b/M7c Raumwechsel (Nachbesserung 3, Abnahme 3 Mangel 2): der Fleck faellt ueber die RAUM-LADEKETTE
 *      (re15_re2z_rng_reset bzw. re15_actor_init), nicht ueber den Modul-Aufruf direkt (FUN_8001633c `sb zero,-50(s1)`
 *      @0x8001639c, einziger Aufrufer @0x80026e0c).
 *   N  Kraehenfedern (Nachbesserung 3, Abnahme 3 Mangel 1): RE2 EMOVL21_S0 FUN_80104678 (Bank 0x0C, Sub 0, Skala 0x800
 *      @0x801046ec-f0, Einheitsmatrix @0x801046f8-fc, SVECTOR = Aktor-Lage +-512 @0x801046a4-e8, Gier rand<<4 @0x801046f4,
 *      `jal 0x8001bf10` @0x80104700), Skript room1090 effect.esp (GLAS1090.ESP) Bank 0x0C, Ops 34/16/5/35/36 (RE2 PSX.EXE
 *      Tafel @0x8009d868). GIB w7 8..11 (@0x80102eb8-bc), Flug-Treffer w3 2..3 (@0x80102a18-1c), Launch w8 5..8
 *      (@0x801032f8-30c); die Feder pendelt, faellt und verschwindet unter dem Boden (Op 5 @0x8001e070 -> Op 35).
 *      PORT-WAHL: an den reinen Flug-Stellen (Abheben @0x80100CA8-C4 u.a.) wird keine Feder gezeichnet.
 * Modus 2 `pruef_log <log> <art>` = Auswerter des Integrationslaufs (RE15_SPINNEN_LOG mit {G..}-Klammern):
 *   art "platzt": eine RE2-Spinne (o=1) stirbt (st=3) und traegt danach sh mit Teil 0 und 19; seit Runde 42 U7 fliegt kein
 *                 Bein (fl & 0x7fff8 == 0; bis dahin fl == 0x7fff8).
 *   art "bein":   eine RE2-Spinne im Treffer (st=2) traegt ein geplatztes Beinpaar (sh-Bits in 3..18).
 *   art "aus":    eine RE2-Spinne stirbt, die Teilschicht bleibt aus (ta=0 in jedem Bild).
 * Modus 3 `pruef_lache <log> <art>` = Auswerter von RE15_LACHE_LOG (main.c, je Schatten einer Spinne):
 *   art "re2": eine Leiche (st=7) traegt pool=1 rgb=bf10bf und erreicht sichtbar 2020 x 2120 (1300/1400 + 90 x 8).
 *   art "r15": eine Leiche traegt pool=1 rgb=9f80df und erreicht 2028 x 2128 (+ 91 x 8) oder 2268 x 2368 (+ 121 x 8).
 * Modus 4 `pruef_flecken <log.flecken>` = Auswerter der Babyflecken (main.c, Nachbesserung 3): mindestens ein Fleck
 *   hx=200 hz=200 rgb=bf10bf sichtbar=1 gezeichnet=1 (der Renderer hat das Viereck eingereiht), kein Fleck mit anderer
 *   Tinte oder anderem Mass.
 * Modus 5 `pruef_federn <log>` = Auswerter von RE15_FEDER_LOG (glas1120_pc.c + <log>.spawn aus enemy_ai_re2_crow.c):
 *   ein GIB-Satz mit 8..11 Federn (anlass=gib, i=n/n), jede Feder gespawnt (platz 0..95) innerhalb +-512 um die Kraehe,
 *   und Bilder mit gezeichneten Feder-Quads (gezeichnet > 0, tex=1).
 */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_teile_r15.h"
#include "re15_spinnengore_r41.h"
#include "re15_spinnenlache_r41.h"
#include "re15_aot.h"
#include "fx_plattform_pc.h"   /* re15_pc_esp_clut / re15_pc_fx_seite_clut_ok: der Zeichner (platform/pc/src) */
#include "re2_fx.h"            /* Nachbesserung 3: RE2-FX-Maschine (Kraehenfeder Raumbank 0x0C) */
#include "re15_schuss_r36.h"   /* re15_schuss_r36_kraehe_parts_seed */
#include "re15_objpoly_r38.h"   /* M9: re15_objpoly_r38_schatten (Release v0.8.30) */
#include "re15_math.h"          /* M9: re15_gte_divide */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void re15_aspider_beine_reset(void);
int  re15_re2spider_tick(int slot);
int  re15_re2spider_baby_tick(int slot);

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
    /* ⛔ Runde 42 Strang gore U7 (Nutzerregel 16.11.2026; 80 %-Video F08 F2 0:15,1-0:15,5 Kadaver mit gespreizten Beinen): mit
     * 80 %-Gestaltung bleiben die Beine beim Platzen dran ((w & 0x41) == 1, kein Flugbit 0x20); der Disc-Beinflug 0x1062
     * (@0x8011419c-52c) gilt nur noch ohne sie (re15_spinnengore_an() == 0). */
    for (int k = 3; k <= 18; k++) {
        if (re15_spinnengore_an()) {
            if ((t[k].w & 0x20u) || (t[k].w & 0x41u) != 1u) return 0;
        } else if ((t[k].w & 0x1062) != 0x1062 || t[k].grav != 20 || t[k].v[1] != -100 || t[k].spin != -128 || t[k].spin_inc != 8)
            return 0;
    }
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
    /* L8 RE2-Babyspinne, Tod (SPLAT @0x80100C84): FUN_80100F28(self, 0, Tabelle @0x801010C7[+0x5]) @0x80100d10-14 */
    {
        size_t n26 = 0;
        uint8_t *e26 = datei(RE15_REPO_INFO "/re2leon/COMMON/BIN/EMS26.BIN", &n26);
        CHECK(e26 != NULL && n26 > 0x10f0, "L8: info/re2leon/COMMON/BIN/EMS26.BIN");
        static const uint8_t tab_soll[22] = { 0x80,0,0,0,0,0,0,0,0,0,3,4,0,0,0,0,3,0,0,0,0,0 };
        if (e26 && n26 > 0x10f0) {
            CHECK(rd32(e26, 0x80100d10u) == 0x902610c7u && rd32(e26, 0x80100d14u) == 0x0c0403cau,
                  "L8: @0x80100d10-14 nicht `lbu a2,4295(at)` / `jal 0x80100f28` (%08x %08x)", rd32(e26, 0x80100d10u), rd32(e26, 0x80100d14u));
            CHECK(rd32(e26, 0x80100f50u) == 0x30c3007fu, "L8: @0x80100f50 nicht `andi v1,a2,0x7f`");
            CHECK(memcmp(e26 + 0x10c7, tab_soll, sizeof tab_soll) == 0, "L8: Tabelle @0x801010C7 weicht ab");
            const uint8_t *r0 = e26 + 0x10dc;
            CHECK(r0[0] == 0 && r0[1] == 0x19 && w16(r0, 4) == 0x1000, "L8: EMS26 Record 0 nicht Effekt 0 Sub 0x19 (Spinnenblut)");
        }
        free(e26);
        static const struct { uint8_t zeile; int blut; } Z[] = { {0,1},{3,1},{7,1},{8,1},{13,1},{20,1},{10,0},{11,0},{16,0} };
        for (size_t k = 0; k < sizeof Z / sizeof Z[0]; k++) {
            frisch(RE15_AI_FLAVOR_RE2);
            SP->type = 0x26; SP->re2s_baby_spawned = 1; SP->grid_id = 1; SP->state = 0;
            re15_re2spider_baby_tick(S);                                  /* INIT */
            re15_esp_fx_reset();
            SP->hp = -1; SP->state = 3; SP->sub_state_1 = Z[k].zeile; SP->sub_state_2 = 0; SP->sub_state_3 = 0;
            re15_re2spider_baby_tick(S);
            fx_zaehlen(&g, &r, &a);
            if (Z[k].blut)
                CHECK(g == 1 && r == 0 && a == 0, "L8: Babytod Zeile %d: gruen %d rot %d alt-0x1500 %d (Record 0 = Spinnenblut)",
                      Z[k].zeile, g, r, a);
            else
                CHECK(g == 0 && a == 1, "L8: Babytod Zeile %d: Record %d ist kein Blut, gruen %d alt-0x1500 %d",
                      Z[k].zeile, tab_soll[Z[k].zeile] & 0x7f, g, a);
        }
    }
    re15_esp_set_global_bank(NULL);
    free(core); free(tex); free(core2); free(tex2);
}

/* ---- M: Lache der Spinnenleiche (Runde 41 Spur S, Nachbesserung 2) --------------------------------------------- */
static void pin(const uint8_t *b, size_t n, uint32_t basis, uint32_t a, uint32_t soll, const char *was)
{
    size_t o = (size_t)(a - basis);
    uint32_t ist = (b && o + 4 <= n) ? w32(b, o) : 0xdeadbeefu;
    CHECK(ist == soll, "M1: @0x%08x nicht `%s` (%08x)", a, was, ist);
}
extern int g_test_room_se_log[2048], g_test_room_se_n;   /* SE-Spion tests/test_support.c (FUN_800453d0) */
static int raum_se_zahl(int id)
{
    int n = 0;
    for (int i = 0; i < g_test_room_se_n && i < 2048; i++) if (g_test_room_se_log[i] == id) n++;
    return n;
}
static void lache_rgb(const re15_actor_t *e, int32_t *hx, int32_t *hz, int *pool, uint8_t rgb[3])
{
    *hx = 500; *hz = 600; *pool = 0; rgb[0] = rgb[1] = rgb[2] = 0;
    re15_spinnenlache_render(e, hx, hz, pool, &rgb[0], &rgb[1], &rgb[2]);
}
/* Nachbesserung 3: ein Baby stirbt am Boden und legt seinen Fleck ab (wie M7). Rueckgabe = Zahl der Flecken. */
void re15_re2z_rng_reset(void);
static int baby_fleck_legen(void)
{
    frisch(RE15_AI_FLAVOR_RE2);
    SP->type = 0x26; SP->re2s_baby_spawned = 1; SP->grid_id = 1; SP->state = 0;
    re15_re2spider_baby_tick(S);
    SP->x = 2222; SP->z = -1111; SP->rot_y = 100;
    SP->hp = -1; SP->state = 3; SP->sub_state_1 = 3; SP->sub_state_2 = 0; SP->sub_state_3 = 0; SP->re2s_mode222 = 0;
    re15_re2spider_baby_tick(S);
    re15_re2spider_baby_tick(S);
    return re15_spinnenlache_flecken_zahl();
}
static void test_lache(void)
{
    size_t n = 0;
    /* M1 Byte-Pins (RE1.5 STAGE2 aus shared_assets = Mod-Baum; Mod = Disc fuer 0x80113f40-0x801155e0, Dossier) */
    uint8_t *st2 = datei(RE15_ASSET_PSX_DIR "/BIN/STAGE2.BIN", &n);
    CHECK(st2 != NULL, "M1: STAGE2.BIN");
    if (st2) {
        const uint32_t B = 0x80100000u;
        pin(st2, n, B, 0x80118e40u, 0x801153d4u, "Zustandstabelle [7] = CORPSE 0x801153d4");
        pin(st2, n, B, 0x8011541cu, 0x3402005au, "ori v0,zero,0x5a");
        pin(st2, n, B, 0x80115420u, 0xa062009eu, "sb v0,158(v1)");
        pin(st2, n, B, 0x8011546cu, 0x3c0600dfu, "lui a2,0xdf");
        pin(st2, n, B, 0x80115470u, 0x34c6809fu, "ori a2,a2,0x809f");
        pin(st2, n, B, 0x801154a8u, 0x24420008u, "addiu v0,v0,8");
        pin(st2, n, B, 0x801154acu, 0x24630008u, "addiu v1,v1,8");
        pin(st2, n, B, 0x801154b0u, 0xa48200bcu, "sh v0,188(a0)");
        pin(st2, n, B, 0x801154b8u, 0xa48300beu, "sh v1,190(a0)");
        pin(st2, n, B, 0x801154c4u, 0x34020078u, "ori v0,zero,0x78");
        pin(st2, n, B, 0x801155bcu, 0x34040009u, "ori a0,zero,0x9");
        pin(st2, n, B, 0x801155ccu, 0x0c0114f4u, "jal 0x800453d0");
        pin(st2, n, B, 0x80114578u, 0x34020007u, "ori v0,zero,0x7 (Platzweg Phase 2)");
        pin(st2, n, B, 0x80114580u, 0xac620004u, "sw v0,4(v1)");
        pin(st2, n, B, 0x80114b1cu, 0x34020007u, "ori v0,zero,0x7 (Krit-Platzer)");
        pin(st2, n, B, 0x80114b20u, 0xa0620005u, "sb v0,5(v1)");
        pin(st2, n, B, 0x80114d4cu, 0x3c030300u, "lui v1,0x300");
        pin(st2, n, B, 0x80114d58u, 0x34630007u, "ori v1,v1,0x7");
        pin(st2, n, B, 0x80114d60u, 0xac430004u, "sw v1,4(v0)");
    }
    free(st2);
    uint8_t *e25 = datei(RE15_REPO_INFO "/re2leon/COMMON/BIN/EMS25.BIN", &n);
    CHECK(e25 != NULL, "M1: EMS25.BIN");
    if (e25) {
        const uint32_t B = 0x80100000u;
        pin(e25, n, B, 0x80100180u, 0x0c04137au, "jal 0x80104de8");
        pin(e25, n, B, 0x80100368u, 0x3c060320u, "lui a2,0x320");
        pin(e25, n, B, 0x80104d7cu, 0x24020002u, "addiu v0,zero,2");
        pin(e25, n, B, 0x80104d80u, 0xa2020225u, "sb v0,549(s0)");
        pin(e25, n, B, 0x80104e7cu, 0x3c0500bfu, "lui a1,0xbf");
        pin(e25, n, B, 0x80104e80u, 0x34a510bfu, "ori a1,a1,0x10bf");
        pin(e25, n, B, 0x80104eacu, 0x2402005au, "addiu v0,zero,90");
        pin(e25, n, B, 0x80104eb4u, 0xa202016bu, "sb v0,363(s0)");
        pin(e25, n, B, 0x80104ed0u, 0x24420008u, "addiu v0,v0,8");
        pin(e25, n, B, 0x80104ed8u, 0xa6220004u, "sh v0,4(s1)");
        pin(e25, n, B, 0x80104edcu, 0xa6230006u, "sh v1,6(s1)");
    }
    free(e25);
    uint8_t *e26 = datei(RE15_REPO_INFO "/re2leon/COMMON/BIN/EMS26.BIN", &n);
    CHECK(e26 != NULL, "M1: EMS26.BIN");
    if (e26) {
        const uint32_t B = 0x80100000u;
        pin(e26, n, B, 0x8010024cu, 0x3c0600c8u, "lui a2,0xc8");
        pin(e26, n, B, 0x80100250u, 0x34c600c8u, "ori a2,a2,0xc8");
        pin(e26, n, B, 0x80100cb4u, 0x3c0500bfu, "lui a1,0xbf");
        pin(e26, n, B, 0x80100cb8u, 0x34a510bfu, "ori a1,a1,0x10bf");
        pin(e26, n, B, 0x80100cd8u, 0xacc2001cu, "sw v0,28(a2)");
        pin(e26, n, B, 0x80100ce0u, 0xacc30044u, "sw v1,68(a2)");
    }
    free(e26);
    uint8_t *x2 = datei(RE15_REPO_INFO "/re2leon/PSX.EXE", &n);
    CHECK(x2 != NULL && n > 0x800, "M1: info/re2leon/PSX.EXE");
    if (x2 && n > 0x800) {
        const uint32_t B = w32(x2, 0x18) - 0x800u;
        pin(x2, n, B, 0x8001b258u, 0xac800000u, "sw zero,0(a0) (FUN_8001B250 nullt nur word0/word1)");
        pin(x2, n, B, 0x8001b25cu, 0xac800004u, "sw zero,4(a0)");
        pin(x2, n, B, 0x800268d8u, 0x0c005a2du, "jal 0x800168b4 (nach dem Handler)");
        pin(x2, n, B, 0x80016960u, 0x34630004u, "ori v1,v1,0x4");
        pin(x2, n, B, 0x80016968u, 0xa203000eu, "sb v1,14(s0)");
        pin(x2, n, B, 0x80026e0cu, 0x0c0058cfu, "jal 0x8001633c (Pool-Init im Raumwechsel)");
        pin(x2, n, B, 0x8001639cu, 0xa220ffceu, "sb zero,-50(s1)");
        /* M9 (Release v0.8.30, Pruefung R1): Zeichner FUN_8001699c ohne Nahgrenze - Bereichspruefung, dann RTPT/RTPS/AVSZ4,
         * OTZ-Klemme 0x1fff und addPrim, kein Sprung auf ein Z dazwischen */
        pin(x2, n, B, 0x80016af8u, 0x0c00b208u, "jal 0x8002c820 (XZ-Bereich)");
        pin(x2, n, B, 0x80016b00u, 0x10400033u, "beq v0,zero,0x80016bd0");
        pin(x2, n, B, 0x80016b30u, 0x4a280030u, "RTPT");
        pin(x2, n, B, 0x80016b58u, 0x4a180001u, "RTPS");
        pin(x2, n, B, 0x80016b6cu, 0x4b68002eu, "AVSZ4");
        pin(x2, n, B, 0x80016b80u, 0x2c422000u, "sltiu v0,v0,0x2000");
        pin(x2, n, B, 0x80016b8cu, 0x24021fffu, "addiu v0,zero,8191");
        pin(x2, n, B, 0x80016b9cu, 0x000420c2u, "srl a0,a0,3");
    }
    free(x2);
    {   /* M9 RE1.5 FUN_8001b064: Bereichspruefung, RotAverage4 (RTPT/RTPS/AVSZ4), OT-Index otz >> 4, kein Z-Sprung */
        uint8_t *x1 = datei(RE15_REPO_INFO "/Re1.5/PSX.EXE", &n);
        CHECK(x1 != NULL && n > 0x800, "M9: info/Re1.5/PSX.EXE");
        if (x1 && n > 0x800) {
            const uint32_t B = w32(x1, 0x18) - 0x800u;
            pin(x1, n, B, 0x8001b224u, 0x0c0050dau, "jal 0x80014368 (XZ-Bereich)");
            pin(x1, n, B, 0x8001b22cu, 0x1040004du, "beq v0,zero,0x8001b364");
            pin(x1, n, B, 0x8001b2c8u, 0x0c019744u, "jal 0x80065d10 (RotAverage4)");
            pin(x1, n, B, 0x8001b2d8u, 0x00021102u, "srl v0,v0,4");
            pin(x1, n, B, 0x80065d2cu, 0x4a280030u, "RTPT");
            pin(x1, n, B, 0x80065d58u, 0x4a180001u, "RTPS");
            pin(x1, n, B, 0x80065d7cu, 0x4b68002eu, "AVSZ4");
        }
        free(x1);
    }
    {   /* M9 Port: re15_objpoly_r38_schatten verwirft nicht nach Z, nur nach GPU-Groesse je Dreieck (0,1,2)/(1,2,3) */
        const int h = 0x200, ox = 160, oy = 120;
        int sx[4], sy[4]; int32_t summe = -1; uint8_t tri[2];
        /* a) ganz vor der Kamera: dieselben Bildpunkte wie die bisherige Rechnung (cx + (IR1*n >> 16)), beide Dreiecke */
        const int32_t vor[4][3] = { { -400, 300, 3000 }, { -400, 300, 2000 }, { 400, 300, 3000 }, { 400, 300, 2000 } };
        CHECK(re15_objpoly_r38_schatten(vor, h, ox, oy, sx, sy, &summe, tri) == 1 && tri[0] && tri[1],
              "M9a: Viereck vor der Kamera nicht gezeichnet (%d/%d)", tri[0], tri[1]);
        for (int v = 0; v < 4; v++) {
            uint32_t nn = re15_gte_divide((uint32_t)h, (uint32_t)vor[v][2]);
            int ex = ox + (int)(((int64_t)vor[v][0] * nn) >> 16), ey = oy + (int)(((int64_t)vor[v][1] * nn) >> 16);
            CHECK(sx[v] == ex && sy[v] == ey, "M9a: Ecke %d (%d,%d) statt (%d,%d)", v, sx[v], sy[v], ex, ey);
        }
        CHECK(summe == 10000, "M9a: SZ-Summe %ld (Soll 10000)", (long)summe);
        /* b) zwei Ecken mit 0 < z < 64 (der alte Port-Ausschnitt warf das GANZE Viereck weg): GTE-Ueberlauf n = 0x1FFFF,
         *    Bildpunkte bleiben klein -> beide Dreiecke gerastert */
        const int32_t nah[4][3] = { { -10, 20, 40 }, { -100, 200, 3000 }, { 10, 20, 40 }, { 100, 200, 3000 } };
        CHECK(re15_objpoly_r38_schatten(nah, h, ox, oy, sx, sy, &summe, tri) == 1 && tri[0] && tri[1],
              "M9b: Viereck mit z 40 verworfen (%d/%d)", tri[0], tri[1]);
        CHECK(sx[0] == ox + (int)((-10 * 0x1FFFFLL) >> 16) && summe == 40 + 3000 + 40 + 3000,
              "M9b: Ecke 0 x %d, Summe %ld", sx[0], (long)summe);
        /* c) Ecke hinter der Kamera: SZ3 = 0 -> n = 0x1FFFF, SX = 160 - 1800 gesaettigt auf -400h, SY = 120 + 599 = 719;
         *    Dreieck (0,1,2) ueber 1023 breit -> weg, (1,2,3) bleibt */
        const int32_t hinten[4][3] = { { -900, 300, -500 }, { -400, 300, 2000 }, { 400, 300, 3000 }, { 400, 300, 2000 } };
        re15_objpoly_r38_schatten(hinten, h, ox, oy, sx, sy, &summe, tri);
        CHECK(sx[0] == -0x400 && sy[0] == oy + (int)((300 * 0x1FFFFLL) >> 16) && tri[0] == 0 && tri[1] == 1
              && summe == 0 + 2000 + 3000 + 2000,
              "M9c: Ecke 0 (%d,%d), Dreiecke %d/%d, Summe %ld (Soll (-1024,719), 0/1, 7000)", sx[0], sy[0], tri[0], tri[1], (long)summe);
    }

    int32_t hx, hz; int pool; uint8_t rgb[3];
    /* M2 RE1.5, Platzweg: Leiche Phase 0 -> 91 Schritte zu 8, Tinte 0x00DF809F, dann Phase 2 */
    frisch(RE15_AI_FLAVOR_RE15);
    SP->state = 7; SP->sub_state_3 = 0; SP->hp = -5;
    lache_rgb(SP, &hx, &hz, &pool, rgb);
    CHECK(pool == 0, "M2: Leiche vor dem ersten Takt hat schon Tinte");
    g_test_room_se_n = 0;
    for (int t = 0; t < 90; t++) re15_enemy_ai_run_all(1);
    CHECK(SP->crow_shadow_w == 1300 + 90 * 8 && SP->crow_shadow_h == 1400 + 90 * 8 && SP->sub_state_3 == 1,
          "M2: RE1.5 nach 90 Takten %u x %u Phase %d (Soll 2020 x 2120, Phase 1)", SP->crow_shadow_w, SP->crow_shadow_h, SP->sub_state_3);
    CHECK(raum_se_zahl(9) == 0, "M2: Raum-SE 9 schon vor dem Ende der Lache (%d)", raum_se_zahl(9));
    re15_enemy_ai_run_all(1);
    CHECK(SP->crow_shadow_w == 2028 && SP->crow_shadow_h == 2128 && SP->sub_state_3 == 2,
          "M2: RE1.5 nach 91 Takten %u x %u Phase %d (Soll 2028 x 2128, Phase 2)", SP->crow_shadow_w, SP->crow_shadow_h, SP->sub_state_3);
    CHECK(raum_se_zahl(9) == 1, "M2: Raum-SE 9 am Ende der Lache %d-mal (Soll 1, @0x801155bc-cc)", raum_se_zahl(9));
    re15_enemy_ai_run_all(1);
    CHECK(SP->crow_shadow_w == 2028, "M2: Phase 2 waechst weiter (%u)", SP->crow_shadow_w);
    CHECK(raum_se_zahl(9) == 1, "M2: Raum-SE 9 wiederholt sich in Phase 2 (%d)", raum_se_zahl(9));
    lache_rgb(SP, &hx, &hz, &pool, rgb);
    CHECK(pool == 1 && hx == 2028 && hz == 2128 && rgb[0] == 0x9f && rgb[1] == 0x80 && rgb[2] == 0xdf,
          "M2: Zeichner pool %d %d x %d rgb %02x%02x%02x (Soll 9f80df)", pool, (int)hx, (int)hz, rgb[0], rgb[1], rgb[2]);
    /* M3 RE1.5, Weg 0x8011461c: Phase 3 -> 121 Schritte */
    frisch(RE15_AI_FLAVOR_RE15);
    SP->state = 7; SP->sub_state_3 = 3; SP->hp = -5;
    for (int t = 0; t < 121; t++) re15_enemy_ai_run_all(1);
    CHECK(SP->crow_shadow_w == 1300 + 121 * 8 && SP->sub_state_3 == 2,
          "M3: RE1.5 Weg 0x8011461c %u Phase %d (Soll 2268, Phase 2)", SP->crow_shadow_w, SP->sub_state_3);
    /* M4 Eintritts-Phase je Todesweg (w7/w9 Platzweg; w3 schwach; w20 mit Krit -> +0x5 = 7 @0x80114b20 -> Platzweg) */
    static const struct { uint8_t w, bits, phase; } E[] = { {7,0x41,0}, {9,0x01,0}, {3,0x01,3}, {20,0x41,0}, {20,0x01,3} };
    for (size_t k = 0; k < sizeof E / sizeof E[0]; k++) {
        frisch(RE15_AI_FLAVOR_RE15);
        SP->state = 3; SP->sub_state_1 = E[k].w; SP->sub_state_3 = 0; SP->hp = -5; SP->hit_react = E[k].bits;
        for (int t = 0; t < 400 && SP->state == 3; t++) re15_enemy_ai_run_all(1);
        CHECK(SP->state == 7, "M4: w%d kommt nicht in die Leiche", E[k].w);
        /* der Takt, der +0x4 = 7 schreibt, laeuft die Leiche noch nicht; Phase 0/3 wird im naechsten Takt zu 1/4 */
        re15_enemy_ai_run_all(1);
        CHECK(SP->sub_state_3 == (uint8_t)(E[k].phase + 1u), "M4: w%d bits %02x Leichen-Phase %d (Soll %d)",
              E[k].w, E[k].bits, SP->sub_state_3, E[k].phase + 1);
    }
    /* M5 RE2: CORPSE +0x225 = 2, FUN_80104DE8 -> 90 Schritte, Tinte 0x00BF10BF, dann +0x225 = 4 */
    re15_spinnengore_test_schalter(1);
    re2_spinne(3, 7, 0x41);
    int t5 = 0;
    for (; t5 < 800 && SP->re2s_gs225 != 4u; t5++) re2_takt();
    CHECK(SP->re2s_gs225 == 4u && SP->crow_shadow_w == 1300 + 90 * 8 && SP->crow_shadow_h == 1400 + 90 * 8,
          "M5: RE2 +0x225 %d, %u x %u nach %d Takten (Soll 4, 2020 x 2120)", SP->re2s_gs225, SP->crow_shadow_w, SP->crow_shadow_h, t5);
    lache_rgb(SP, &hx, &hz, &pool, rgb);
    CHECK(pool == 1 && rgb[0] == 0xbf && rgb[1] == 0x10 && rgb[2] == 0xbf, "M5: RE2-Tinte %02x%02x%02x (Soll bf10bf)", rgb[0], rgb[1], rgb[2]);
    for (int t = 0; t < 5; t++) re2_takt();
    CHECK(SP->crow_shadow_w == 2020, "M5: [4] waechst weiter (%u)", SP->crow_shadow_w);
    /* M6 RE2 im Wasser: [2] bleibt stehen (`jal 0x800527b4` / `bne v0,zero` @0x80104e70-78), keine Lache */
    re2_spinne(3, 7, 0x41);
    re15_aot_init();
    g_aot.slots[0].active = 1; g_aot.slots[0].type = RE15_AOT_TYPE_WATER; g_aot.slots[0].has_quad = 0;
    g_aot.slots[0].x = 0; g_aot.slots[0].z = 0; g_aot.slots[0].half_w = 30000; g_aot.slots[0].half_h = 30000;
    g_aot.env_params[0].p0 = -1620;
    for (int t = 0; t < 400 && SP->state != 7; t++) re2_takt();
    for (int t = 0; t < 120; t++) re2_takt();
    CHECK(SP->state == 7 && SP->re2s_gs225 == 2u && SP->crow_pool == 0,
          "M6: RE2-Leiche im Wasser: Zustand %d +0x225 %d pool %d (Soll 7, 2, 0)", SP->state, SP->re2s_gs225, SP->crow_pool);
    re15_aot_init();
    /* M7 Babyspinne: SPLAT am Boden faerbt (200 x 200, bf10bf), nach dem DESPAWN bleibt der Fleck bis zum Raumwechsel */
    re15_spinnenlache_raum_reset();
    frisch(RE15_AI_FLAVOR_RE2);
    SP->type = 0x26; SP->re2s_baby_spawned = 1; SP->grid_id = 1; SP->state = 0;
    re15_re2spider_baby_tick(S);
    lache_rgb(SP, &hx, &hz, &pool, rgb);
    CHECK(hx == 200 && hz == 200 && pool == 0, "M7: lebendes Baby %d x %d pool %d (Soll 200 x 200 grau, @0x8010024c-50)", (int)hx, (int)hz, pool);
    SP->x = 1234; SP->z = -4321; SP->rot_y = 777;
    SP->hp = -1; SP->state = 3; SP->sub_state_1 = 3; SP->sub_state_2 = 0; SP->sub_state_3 = 0; SP->re2s_mode222 = 0;
    re15_re2spider_baby_tick(S);
    lache_rgb(SP, &hx, &hz, &pool, rgb);
    CHECK(SP->active && pool == 1 && hx == 200 && rgb[0] == 0xbf && rgb[1] == 0x10 && rgb[2] == 0xbf,
          "M7: SPLAT-Takt: aktiv %d pool %d %d rgb %02x%02x%02x", SP->active, pool, (int)hx, rgb[0], rgb[1], rgb[2]);
    CHECK(re15_spinnenlache_flecken_zahl() == 0, "M7: Fleck schon vor dem DESPAWN");
    re15_re2spider_baby_tick(S);
    CHECK(!SP->active && re15_spinnenlache_flecken_zahl() == 1, "M7: nach dem DESPAWN aktiv %d Flecken %d", SP->active,
          re15_spinnenlache_flecken_zahl());
    {
        int32_t fp[3] = { 0, 0, 0 }, fhx = 0, fhz = 0; int16_t fy = 0; uint8_t frgb[3] = { 0, 0, 0 };
        re15_spinnenlache_fleck(0, fp, &fy, &fhx, &fhz, frgb);
        CHECK(fp[0] == 1234 && fp[1] == (int32_t)SP->dog_floor_y && fp[2] == -4321 && fy == 777 && fhx == 200 && fhz == 200
              && frgb[0] == 0xbf && frgb[1] == 0x10 && frgb[2] == 0xbf,
              "M7: Fleck (%d,%d,%d) yaw %d %d x %d rgb %02x%02x%02x", (int)fp[0], (int)fp[1], (int)fp[2], fy, (int)fhx, (int)fhz,
              frgb[0], frgb[1], frgb[2]);
    }
    /* M7b (Nachbesserung 3, Abnahme 3 Mangel 2): der Raumwechsel ueber die Ladekette re15_re2z_rng_reset (Aufrufer
     * re15_actor_init und scd_room_setup.c), NICHT ueber re15_spinnenlache_raum_reset direkt. */
    re15_re2z_rng_reset();
    CHECK(re15_spinnenlache_flecken_zahl() == 0, "M7b: Raumwechsel (re15_re2z_rng_reset) loescht die Flecken nicht (%d)",
          re15_spinnenlache_flecken_zahl());
    /* M7c: noch einmal mit re15_actor_init (Raumwechsel: actor_common.c ruft re15_re2z_rng_reset) */
    CHECK(baby_fleck_legen() == 1, "M7c: kein Fleck nach dem DESPAWN (%d)", re15_spinnenlache_flecken_zahl());
    re15_actor_init();
    CHECK(re15_spinnenlache_flecken_zahl() == 0, "M7c: Raumwechsel (re15_actor_init) loescht die Flecken nicht (%d)",
          re15_spinnenlache_flecken_zahl());
    /* M7d Gegenrichtung: Spielschritte ohne Raumwechsel lassen den Fleck liegen (der tote Platz wird nicht mehr getickt) */
    CHECK(baby_fleck_legen() == 1, "M7d: kein Fleck nach dem DESPAWN");
    for (int t = 0; t < 30; t++) re15_enemy_ai_run_all(1);
    CHECK(re15_spinnenlache_flecken_zahl() == 1, "M7d: Fleck ohne Raumwechsel weg (%d)", re15_spinnenlache_flecken_zahl());
    re15_re2z_rng_reset();
    /* M8 Baby stirbt in der Luft (+0x222 != 0): word0 |= 0x400 statt Tinte (@0x80100ce4-f0) -> kein gruener Fleck */
    frisch(RE15_AI_FLAVOR_RE2);
    SP->type = 0x26; SP->re2s_baby_spawned = 1; SP->grid_id = 1; SP->state = 0;
    re15_re2spider_baby_tick(S);
    SP->hp = -1; SP->state = 3; SP->sub_state_1 = 3; SP->sub_state_2 = 0; SP->sub_state_3 = 0; SP->re2s_mode222 = 1;
    re15_re2spider_baby_tick(S); re15_re2spider_baby_tick(S);
    CHECK(SP->crow_pool == 0 && re15_spinnenlache_flecken_zahl() == 0, "M8: Baby in der Luft: pool %d Flecken %d",
          SP->crow_pool, re15_spinnenlache_flecken_zahl());
    /* M9 Abgrenzung: RE1.5-Feuer-Emitter 0x26 (nicht vom RE2-Spawner) bekommt kein Spinnenmass */
    frisch(RE15_AI_FLAVOR_RE2);
    SP->type = 0x26; SP->re2s_baby_spawned = 0;
    CHECK(re15_spinnenlache_render(SP, &hx, &hz, &pool, &rgb[0], &rgb[1], &rgb[2]) == 0, "M9: Feuer-Emitter 0x26 bekommt Spinnenschatten");
    /* M10 lebende Spinne 0x25 ohne Leiche: der Zeichner behaelt das Spur-L-Mass (kein Eingriff) */
    frisch(RE15_AI_FLAVOR_RE2);
    CHECK(re15_spinnenlache_render(SP, &hx, &hz, &pool, &rgb[0], &rgb[1], &rgb[2]) == 0, "M10: lebende Spinne bekommt ein Lachenmass");
}

/* ---- N: Kraehenfedern (Runde 41 Spur S, Nachbesserung 3, Abnahme 3 Mangel 1) ----------------------------------- */
int  re15_re2crow_hurt_test(re15_actor_t *e, re15_actor_t *pl);
int  re15_re2crow_tick(int slot);
void re15_re2crow_feder_zaehler(unsigned *flug, unsigned *gezeichnet, unsigned *gespawnt);
static uint8_t *s_core, *s_raum;
static int federn_bereit(void)
{
    static int ok = -1;
    if (ok >= 0) return ok;
    size_t nc = 0, nr = 0;
    s_core = datei(RE15_ASSET_RE2_DIR "/CORE00.ESP", &nc);
    s_raum = datei(RE15_ASSET_RE2_DIR "/GLAS1090.ESP", &nr);
    ok = (s_core && s_raum && re2fx_register_core(s_core, nc) == 0 && re2fx_register_raum(s_raum, nr) == 0);
    return ok;
}
static int federn_zaehlen(void)
{
    int n = 0;
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        const uint8_t *b = re2fx_platz(i);
        if (b && b[0x1C] == 0x0C && w16(b, 0x18) != 0) n++;
    }
    return n;
}
static re15_actor_t *kraehe_neu(int32_t y)
{
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    PL = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(PL, 0, sizeof *PL);
    PL->active = 1; PL->hp = 100; PL->x = 0; PL->z = -3000;
    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x21; e->state = 1; e->hp = 10;
    e->x = 1000; e->y = y; e->z = 2000; e->dog_floor_y = 0;
    re15_schuss_r36_kraehe_parts_seed(e);
    re2fx_reset();
    return e;
}
/* Prueft jede lebende Feder gegen FUN_80104678 / FUN_8001bf10 und gibt die Zahl zurueck. k = Kraehen-Lage im Spawn-Takt
 * (der Flug-Treffer und der Launch bewegen die Kraehe im selben Aufruf NACH den Federn weiter, @0x80102A98-C4 / @0x80103358-64). */
static int federn_pruefen(const int32_t k[3], const char *was)
{
    int n = 0;
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        const uint8_t *b = re2fx_platz(i);
        if (!b || b[0x1C] != 0x0C || w16(b, 0x18) == 0) continue;
        n++;
        const int dx = (int16_t)w16(b, 0x2C) - (int)k[0], dy = (int16_t)w16(b, 0x2E) - (int)k[1], dz = (int16_t)w16(b, 0x30) - (int)k[2];
        CHECK(w16(b, 0x18) == 0xA003u && b[0x00] == 34 && b[0x01] == 16 && b[0x1E] == 0 && w16(b, 0x3A) == 0x800,
              "%s: Platz %d Status %04x Op %d/%d Sub %d Skala %04x (Soll A003, 34/16, 0, 0800: @0x8001bfa8, Skript @0xAD8, @0x801046ec-f0)",
              was, i, w16(b, 0x18), b[0], b[1], b[0x1E], w16(b, 0x3A));
        CHECK(dx >= -512 && dx <= 508 && dy >= -512 && dy <= 508 && dz >= -512 && dz <= 508 && (dx & 3) == 0 && (dy & 3) == 0 && (dz & 3) == 0,
              "%s: Platz %d Versatz (%d,%d,%d) nicht s8*4 um die Kraehe (@0x801046a4-e8)", was, i, dx, dy, dz);
        CHECK((w16(b, 0x22) & 0xFu) == 0 && w16(b, 0x22) <= 4080u, "%s: Gier %u nicht rand<<4 (@0x801046f4)", was, w16(b, 0x22));
        CHECK(!memcmp(b + 0x4C, re2fx_einheitsmatrix, 32), "%s: Matrix nicht 0x8009DB44 (@0x801046f8-fc)", was);
    }
    return n;
}
static void test_federn(void)
{
    size_t n = 0;
    /* N1 Byte-Pins */
    uint8_t *ov = datei(RE15_REPO_INFO "/re2leon/COMMON/BIN/EMOVL21_S0.BIN", &n);
    CHECK(ov != NULL, "N1: EMOVL21_S0.BIN fehlt");
    if (ov) {
        const uint32_t O = 0x80100000u;
        pin(ov, n, O, 0x801046a4u, 0x00021600u, "sll v0,v0,24");
        pin(ov, n, O, 0x801046a8u, 0x8e230038u, "lw v1,56(s1)");
        pin(ov, n, O, 0x801046acu, 0x00021583u, "sra v0,v0,22");
        pin(ov, n, O, 0x801046c0u, 0x8e23003cu, "lw v1,60(s1)");
        pin(ov, n, O, 0x801046d8u, 0x8e230040u, "lw v1,64(s1)");
        pin(ov, n, O, 0x801046ecu, 0x3c040c00u, "lui a0,0xc00");
        pin(ov, n, O, 0x801046f0u, 0x34840800u, "ori a0,a0,0x800");
        pin(ov, n, O, 0x801046f4u, 0x00022900u, "sll a1,v0,4");
        pin(ov, n, O, 0x801046f8u, 0x3c06800au, "lui a2,0x800a");
        pin(ov, n, O, 0x801046fcu, 0x24c6db44u, "addiu a2,a2,-9404");
        pin(ov, n, O, 0x80104700u, 0x0c006fc4u, "jal 0x8001bf10");
        pin(ov, n, O, 0x80104704u, 0x27a70010u, "addiu a3,sp,16");
        pin(ov, n, O, 0x80102eb8u, 0x0c04119eu, "jal 0x80104678 (GIB)");
        pin(ov, n, O, 0x80102ebcu, 0x34450008u, "ori a1,v0,0x8");
        pin(ov, n, O, 0x80102a18u, 0x0c04119eu, "jal 0x80104678 (Treffer)");
        pin(ov, n, O, 0x80102a1cu, 0x34450002u, "ori a1,v0,0x2");
        pin(ov, n, O, 0x80102b3cu, 0x8e02003cu, "lw v0,60(s0) (+0x3C = y)");
        free(ov);
    }
    uint8_t *exe = datei(RE15_REPO_INFO "/re2leon/PSX.EXE", &n);
    CHECK(exe != NULL, "N1: re2leon/PSX.EXE fehlt");
    if (exe) {
        CHECK(exe_wort(exe, n, 0x8009d8a8u) == 0x8001f128u, "N1: Op-Tafel [16] != 0x8001f128");
        CHECK(exe_wort(exe, n, 0x8009d8f0u) == 0x80020364u, "N1: Op-Tafel [34] != 0x80020364");
        CHECK(exe_wort(exe, n, 0x8009d8f4u) == 0x800204f8u, "N1: Op-Tafel [35] != 0x800204f8");
        CHECK(exe_wort(exe, n, 0x8009d8f8u) == 0x80020510u, "N1: Op-Tafel [36] != 0x80020510");
        CHECK(exe_wort(exe, n, 0x80020504u) == 0xa0400000u, "N1: @0x80020504 nicht `sb zero,0(v0)`");
        CHECK(exe_wort(exe, n, 0x8002050cu) == 0xa4400018u, "N1: @0x8002050c nicht `sh zero,24(v0)`");
        CHECK(exe_wort(exe, n, 0x8001e070u) == 0x0083182au, "N1: @0x8001e070 nicht `slt v1,a0,v1`");
        CHECK(exe_wort(exe, n, 0x8001bfa8u) == 0x3402a003u, "N1: @0x8001bfa8 nicht `ori v0,zero,0xa003`");
        CHECK(exe_wort(exe, n, 0x800204bcu) == 0xa4a30042u, "N1: @0x800204bc nicht `sh v1,66(a1)`");
        free(exe);
    }
    uint8_t *esp = datei(RE15_ASSET_RE2_DIR "/GLAS1090.ESP", &n);
    CHECK(esp != NULL && n == 3092, "N1: GLAS1090.ESP");
    if (esp && n == 3092) {
        static const uint8_t step[24] = { 0x22, 0x10, 0x23, 0x24, 0x00, 0x10, 0x00, 0x10, 0x00, 0xfe, 0x00, 0x22,
                                          0, 0, 0, 0, 0, 0, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00 };
        CHECK(esp[6] == 0x0C && w32(esp, 3088u - 4u * 6u) == 0x994u, "N1: Raumbank 0x0C nicht an Id-Platz 6 / Kopf 0x994");
        CHECK(w16(esp, 0x994 + 4) == 0x7840 && !memcmp(esp + 0xAD8 + 4, step, sizeof step),
              "N1: Bank 0x0C CLUT-Kopf %04x / Step @0xADC != 22 10 23 24 ..", w16(esp, 0x994 + 4));
        free(esp);
    }
    size_t na = 0, nb = 0;
    uint8_t *ta = datei(RE15_ASSET_RE2_DIR "/GLAS1090_0C.TIM", &na);
    uint8_t *tb = datei(RE15_REPO_INFO "/re2leon/PL0/RDT/room1090/esp0C.tim", &nb);
    CHECK(ta && tb && na == nb && na == 15424 && !memcmp(ta, tb, na), "N1: GLAS1090_0C.TIM != room1090/esp0C.tim");
    if (ta && na == 15424) CHECK(w16(ta, 0x2c + 8 + 8) == 0x40 && w16(ta, 0x2c + 8 + 10) == 0x78, "N1: esp0C.tim nicht 256 x 120");
    free(ta); free(tb);

    CHECK(federn_bereit(), "N: CORE00.ESP / GLAS1090.ESP nicht registriert");
    if (!federn_bereit()) return;

    /* N2 GIB: Redhawk w7 -> RE2-Zeile 5 -> 0x80102CA0, 8..11 Federn */
    unsigned f0 = 0, g0 = 0, s0 = 0, f1 = 0, g1 = 0, s1 = 0;
    re15_actor_t *e = kraehe_neu(-1500);
    re15_re2crow_feder_zaehler(&f0, &g0, &s0);
    e->state = 2; e->sub_state_1 = 7; e->sub_state_2 = 0;
    int32_t kl[3] = { e->x, e->y, e->z };
    re15_re2crow_hurt_test(e, PL);
    re15_re2crow_feder_zaehler(&f1, &g1, &s1);
    int nf = federn_pruefen(kl, "N2 GIB");
    CHECK(nf >= 8 && nf <= 11 && (int)(s1 - s0) == nf && (int)(g1 - g0) == nf,
          "N2: GIB %d Federn, gespawnt %u, gezeichnet %u (Soll 8..11 @0x80102ea8-bc)", nf, s1 - s0, g1 - g0);
    CHECK(e->state == 7 && e->sub_state_1 == 1, "N2: nach dem GIB Zustand %d/%d (Soll 7/1)", e->state, e->sub_state_1);
    /* N3 Fall: Pendel (Op 34), Boden (Op 16/5), unter dem Boden frei (Op 35); Quads aus dem Bild 256 x 120 */
    {
        re15_camera_view_t cam;
        memset(&cam, 0, sizeof cam);
        cam.rot[0] = cam.rot[4] = cam.rot[8] = 4096;
        cam.trans[0] = -1000; cam.trans[1] = 0; cam.trans[2] = 4000;
        cam.fov_screen_dist = 600;
        static re2fx_quad_t q[128];
        int quads = 0, uv_aus = 0, kipp = 0, bild = 0, max_dx = 0;
        int32_t y_start = 0;
        for (int i = 0; i < RE2FX_PLAETZE; i++) { const uint8_t *b = re2fx_platz(i); if (b && b[0x1C] == 0x0C && w16(b, 0x18)) { y_start = (int16_t)w16(b, 0x2E); break; } }
        uint16_t r42[RE2FX_PLAETZE];
        for (int i = 0; i < RE2FX_PLAETZE; i++) { const uint8_t *b = re2fx_platz(i); r42[i] = b ? w16(b, 0x42) : 0; }
        for (bild = 0; bild < 900 && federn_zaehlen() > 0; bild++) {
            re2fx_tick();
            for (int i = 0; i < RE2FX_PLAETZE; i++) {
                const uint8_t *b = re2fx_platz(i);
                if (!b || b[0x1C] != 0x0C || !w16(b, 0x18)) continue;
                if (w16(b, 0x42) != r42[i]) { kipp++; r42[i] = w16(b, 0x42); }
                int dx = (int16_t)w16(b, 0x34) - (int16_t)w16(b, 0x2C); if (dx < 0) dx = -dx;
                if (dx > max_dx) max_dx = dx;
            }
            int nq = re2fx_quads(&cam, 160, 120, 512, 0, NULL, NULL, q, 128);
            for (int k = 0; k < nq; k++) {
                const uint8_t *b = re2fx_platz(q[k].platz);
                if (!b || b[0x1C] != 0x0C) continue;
                quads++;
                if (q[k].u1 > 256 || q[k].v1 > 120 || q[k].code != 0x2C) uv_aus++;
            }
        }
        CHECK(federn_zaehlen() == 0, "N3: nach %d Bildern leben noch %d Federn (Op 5 @0x8001e070 -> Op 35 @0x80020504-0c)", bild, federn_zaehlen());
        CHECK(bild > 30, "N3: Federn schon nach %d Bildern weg (Start y %d, Boden 0)", bild, (int)y_start);
        CHECK(re2fx_op_zaehler(34) > 0 && re2fx_op_zaehler(16) >= (unsigned)nf && re2fx_op_zaehler(5) > 0 && re2fx_op_zaehler(35) >= (unsigned)nf,
              "N3: Ops 34 %u / 16 %u / 5 %u / 35 %u", re2fx_op_zaehler(34), re2fx_op_zaehler(16), re2fx_op_zaehler(5), re2fx_op_zaehler(35));
        CHECK(re2fx_op_unbekannt() == 0, "N3: %u unbekannte Ops", re2fx_op_unbekannt());
        CHECK(kipp >= nf && max_dx > 200, "N3: Pendel: %d Richtungswechsel (+0x42 @0x800204b0-bc), groesster Schwung %d", kipp, max_dx);
        CHECK(quads > 0 && uv_aus == 0, "N3: %d Feder-Quads, %d ausserhalb 256 x 120 / nicht Code 0x2C", quads, uv_aus);
        printf("N-Messung: GIB %d Federn ab y %d, weg nach %d Bildern, Ops 34/16/5/35/36 = %u/%u/%u/%u/%u, Richtungswechsel %d, "
               "groesster Schwung %d, Quads %d\n", nf, (int)y_start, bild, re2fx_op_zaehler(34), re2fx_op_zaehler(16),
               re2fx_op_zaehler(5), re2fx_op_zaehler(35), re2fx_op_zaehler(36), kipp, max_dx, quads);
    }
    /* N4 Flug-Treffer: Beretta w5 -> Zeile 4 -> 0x80102934 P0, 2..3 Federn (@0x80102a08-1c). Bis Runde 42 die Browning w3; die
     * zerlegt die Kraehe seit U5 (80 %-Video F07 F1 0:54,55). */
    e = kraehe_neu(-1500);
    re15_re2crow_feder_zaehler(&f0, &g0, &s0);
    e->state = 2; e->sub_state_1 = 5; e->sub_state_2 = 0; e->hp = 5;
    kl[0] = e->x; kl[1] = e->y; kl[2] = e->z;
    re15_re2crow_hurt_test(e, PL);
    nf = federn_pruefen(kl, "N4 Treffer");
    CHECK(nf >= 2 && nf <= 3, "N4: Flug-Treffer %d Federn (Soll 2..3 @0x80102a08-1c)", nf);
    /* N4b Sturz: je Zaehler eine Feder (@0x80102a68-70, @0x80102a88-90) */
    for (int t = 0; t < 6; t++) re15_re2crow_hurt_test(e, PL);
    CHECK(federn_zaehlen() == nf + 2, "N4b: Sturz %d Federn (Soll %d + 2)", federn_zaehlen(), nf);
    printf("N-Messung: Flug-Treffer %d Federn, mit Sturz %d\n", nf, federn_zaehlen());
    /* N5 Launch: Remington w8 -> Zeile 7 -> 0x8010327C, 5..8 Federn (@0x801032f8-30c). Runde 42 U5: im Spiel zerlegt die
     * Schrotflinte (Disc-Klasse @0x801211cc[8] = 0x801149c4); der LAUNCH-Zweig bleibt Referenz-Pin mit abgeschaltetem U5-Weg. */
    extern void re15_re2crow_r42_fetzen_test(int an);
    re15_re2crow_r42_fetzen_test(0);
    e = kraehe_neu(-1500);
    e->state = 2; e->sub_state_1 = 8; e->sub_state_2 = 0;
    kl[0] = e->x; kl[1] = e->y; kl[2] = e->z;
    re15_re2crow_hurt_test(e, PL);
    re15_re2crow_r42_fetzen_test(-1);
    nf = federn_pruefen(kl, "N5 Launch");
    CHECK(nf >= 5 && nf <= 8, "N5: Launch %d Federn (Soll 5..8 @0x801032f8-30c)", nf);
    printf("N-Messung: Launch %d Federn\n", nf);
    /* N6 PORT-WAHL: Abheben P0 (@0x80100CA8-C4, 1/8) zieht die 4 Draws je Feder, zeichnet aber nicht */
    e = kraehe_neu(0);
    e->state = 0;
    PL->x = 2500; PL->z = 2000;
    e->ai_dist = 1500; re15_re2crow_tick(1);
    re15_re2crow_feder_zaehler(&f0, &g0, &s0);
    for (int k = 0; k < 400; k++) {
        e->state = 1; e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0; e->y = 0;
        e->ai_dist = 1500;
        re15_re2crow_tick(1);
    }
    re15_re2crow_feder_zaehler(&f1, &g1, &s1);
    printf("N-Messung: Abheben 400 x: %u Flug-Federn gezogen, %u gezeichnet\n", f1 - f0, g1 - g0);
    CHECK(f1 - f0 > 10 && g1 == g0 && federn_zaehlen() == 0,
          "N6: Abheben: %u Flug-Federn gezogen, %u gezeichnet, %d lebend (Soll >10 / 0 / 0, PORT-WAHL)", f1 - f0, g1 - g0, federn_zaehlen());
    re2fx_reset();
}

/* Modus 3: RE15_LACHE_LOG */
static int pruef_lache(const char *pfad, const char *art)
{
    FILE *f = fopen(pfad, "r");
    if (!f) { printf("FAIL: kein Log %s\n", pfad); return 1; }
    static char z[1024];
    const char *soll_rgb = !strcmp(art, "re2") ? "bf10bf" : "9f80df";
    int zeilen = 0, pool_an = 0, end_sichtbar = 0, end_any = 0, falsche_tinte = 0, basis = 0;
    while (fgets(z, sizeof z, f)) {
        unsigned fr, typ; int slot, st, s1, s2, s3, hx, hz, pool, y, wx, wz, sb = 0; char rgb[16];
        int k = sscanf(z, "F=%u slot=%d typ=%x st=%d/%d/%d/%d hx=%d hz=%d pool=%d rgb=%6s y=%d w(%d,%d) sichtbar=%d",
                       &fr, &slot, &typ, &st, &s1, &s2, &s3, &hx, &hz, &pool, rgb, &y, &wx, &wz, &sb);
        if (k < 14 || typ != 0x25u) continue;
        zeilen++;
        if (st == 7 && !pool && hx == 1300 && hz == 1400) basis = 1;
        if (!pool) continue;
        pool_an = 1;
        if (strcmp(rgb, soll_rgb)) falsche_tinte++;
        int fertig = !strcmp(art, "re2") ? (hx == 2020 && hz == 2120)
                                         : ((hx == 2028 && hz == 2128) || (hx == 2268 && hz == 2368));
        if (fertig) { end_any = 1; if (sb) end_sichtbar = 1; }
    }
    fclose(f);
    printf("pruef_lache %s: Zeilen %d, Basis 1300x1400 %d, Lache %d, Endmass %d (sichtbar %d), falsche Tinte %d\n",
           art, zeilen, basis, pool_an, end_any, end_sichtbar, falsche_tinte);
    if (!strcmp(art, "re2")) return (basis && pool_an && end_sichtbar && !falsche_tinte) ? 0 : 1;
    if (!strcmp(art, "r15")) return (basis && pool_an && end_any && !falsche_tinte) ? 0 : 1;
    printf("FAIL: unbekannte Art %s\n", art);
    return 1;
}

/* Modus 4: <RE15_LACHE_LOG>.flecken (Nachbesserung 3, Abnahme 3 Mangel 2) */
static int pruef_flecken(const char *pfad)
{
    FILE *f = fopen(pfad, "r");
    if (!f) { printf("FAIL: kein Log %s\n", pfad); return 1; }
    static char z[512];
    int zeilen = 0, gut = 0, sichtbar = 0, fremd = 0, ohne_zeichnung = 0;
    while (fgets(z, sizeof z, f)) {
        unsigned fr, raum = 0; int fi, hx, hz, y, wx, wz, sb, gz = -1; char rgb[16];
        int k = sscanf(z, "F=%u fleck=%d hx=%d hz=%d rgb=%6s y=%d w(%d,%d) sichtbar=%d gezeichnet=%d raum=%x",
                       &fr, &fi, &hx, &hz, rgb, &y, &wx, &wz, &sb, &gz, &raum);
        if (k < 9) continue;
        zeilen++;
        if (hx != 200 || hz != 200 || strcmp(rgb, "bf10bf")) { fremd++; continue; }
        if (sb) sichtbar++;
        if (sb && gz != 1) ohne_zeichnung++;
        if (sb && gz == 1) gut++;
    }
    fclose(f);
    printf("pruef_flecken: Zeilen %d, sichtbar %d, gezeichnet %d, sichtbar ohne Zeichnung %d, fremd %d\n",
           zeilen, sichtbar, gut, ohne_zeichnung, fremd);
    return (gut > 0 && !fremd && !ohne_zeichnung) ? 0 : 1;
}

/* Modus 5: RE15_FEDER_LOG (Nachbesserung 3, Abnahme 3 Mangel 1) */
static int pruef_federn(const char *pfad, const char *anlass)
{
    char sp[600];
    snprintf(sp, sizeof sp, "%s.spawn", pfad);
    FILE *f = fopen(sp, "r");
    if (!f) { printf("FAIL: kein Log %s\n", sp); return 1; }
    static char z[512];
    int saetze = 0, federn = 0, fern = 0, voll = 0, fuell = 0;
    while (fgets(z, sizeof z, f)) {
        unsigned fr; int slot, i, n, platz, ox, oy, oz, gier, kx, ky, kz, boden; char an[32];
        if (sscanf(z, "F=%u slot=%d anlass=%31s i=%d/%d platz=%d ofs=(%d,%d,%d) gier=%d kraehe=(%d,%d,%d) boden=%d",
                   &fr, &slot, an, &i, &n, &platz, &ox, &oy, &oz, &gier, &kx, &ky, &kz, &boden) != 14) continue;
        if (strcmp(an, anlass)) continue;
        federn++;
        if (platz < 0 || platz >= RE2FX_PLAETZE) voll++;
        if (ox - kx < -512 || ox - kx > 508 || oy - ky < -512 || oy - ky > 508 || oz - kz < -512 || oz - kz > 508) fern++;
        if (i == n) { saetze++; if (!strcmp(anlass, "gib") && (n < 8 || n > 11)) fuell++; }
    }
    fclose(f);
    f = fopen(pfad, "r");
    int bilder = 0, gezeichnet = 0, tex = 1;
    if (f) {
        while (fgets(z, sizeof z, f)) {
            unsigned fr; int leb, qu, gz, tx;
            if (sscanf(z, "F=%u federn=%d quads=%d gezeichnet=%d tex=%d", &fr, &leb, &qu, &gz, &tx) != 5) continue;
            bilder++;
            if (gz > 0) gezeichnet++;
            if (!tx) tex = 0;
        }
        fclose(f);
    }
    printf("pruef_federn %s: Saetze %d, Federn %d (nicht gespawnt %d, ausserhalb +-512 %d, Satzgroesse falsch %d), "
           "Bilder mit Feder %d, davon gezeichnet %d, Bild geladen %d\n",
           anlass, saetze, federn, voll, fern, fuell, bilder, gezeichnet, tex);
    return (saetze > 0 && federn > 0 && !voll && !fern && !fuell && gezeichnet > 0 && tex) ? 0 : 1;
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
                /* Runde 42 Strang gore U7 (80 %-Video F08 F2 0:15,1-0:15,5): Teile 0/19 platzen, die Beine 3..18 fliegen NICHT
                 * mehr (vorher fl == 0x7fff8, Disc-Beinflug @0x8011419c-52c). */
                if ((sh & 0x80001u) == 0x80001u && (fl & 0x7fff8u) == 0u) platzt = 1;
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
    if (argc >= 4 && !strcmp(argv[1], "pruef_lache")) return pruef_lache(argv[2], argv[3]);
    if (argc >= 3 && !strcmp(argv[1], "pruef_flecken")) return pruef_flecken(argv[2]);
    if (argc >= 4 && !strcmp(argv[1], "pruef_federn")) return pruef_federn(argv[2], argv[3]);
    test_bytes();
    test_re2();
    test_re15();
    test_alloc();
    test_zombie_mc51();
    test_hund();
    test_blutfarbe();
    test_lache();
    test_federn();
    if (fails) { printf("%d Fehler\n", fails); return 1; }
    printf("OK unit_r41_spinnengore\n");
    return 0;
}
