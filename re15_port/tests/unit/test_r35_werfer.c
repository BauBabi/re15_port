/* test_r35_werfer.c — Runde 35 Spur B: Werfer-Klasse (15..18), Flammenwerfer (14), Colt Python (20).
 *
 * Misst die MECHANIK (Zustaende, Werte, Bildfolgen) der RE2-Retail-Umsetzung — Belege und
 * Adressen im Dossier analysis/befunde_runde35/B_werfer.md und in include/re15_werfer.h /
 * engine/src/re2_fx.c (Block "RUNDE 35 SPUR B").
 *
 *  A  Daten: Magazine/Munition (@0x80074da8 + Records 0x80074cb4/b8/bc), Bank-/Clip-Umsetzung
 *     PL00W0F (11 Clips), Rueckstoss-Schwelle, Nachlade-Gate der Werfer.
 *  B  GL-Runde in der RE2-FX-Maschine (CORE00.ESP Bank 2 Skr 4): Op 17 Start (Art aus der Waffe,
 *     Lebensdauer 10..12 / 15), Op 22 + Op 15 je Bild, Applier-Treffer 0x30009+Art zweimal je Bild
 *     (Box {-1400,0,350,250}), Wand -> Op 47 (Sub 12: SE 0x01110001, Schaden 0x10020009 zweimal,
 *     drei Kinder), Saeure -> Op 49 + 0x01130001, Brand -> Op 48 + 0x01120001, Treffer -> sofort.
 *  C  Rakete (Bank 2 Skr 5): Op 23 Rauchspur + Op 24 je Bild, Treffer 0x30011 / Boden 0x20011
 *     (Box {-800,0,400,200}), Explosion Op 47 Sub 13 = SE 0x01140001 OHNE Flaechenschaden.
 *  D  Flammenstrahl (Bank 3 Skr 5, Op 70): Box {-1400,0,400,200}, Hitcode 0x20010 je Bild,
 *     Vortrieb vel.y 20 + acc 20, Zaehler 10 -> acc.y -10, Treffer -> Nachbrennen (Anim 12,
 *     step[2] 6, step[3] 1) und Puff 0x040C2000 bei step[2] == 3.
 *  E  Fuel-Takt des Flammenwerfers (RE2 FUN_8006a0cc Id 16): 7 Bilder nichts, 8. Bild zwei
 *     Einheiten, leer -> 0.
 *  F  Colt Python: Hitscan trifft wie ein Revolver (Schuss-Streifen), toetet den Zombie
 *     (Spalte 7 des Redhawk), Reaktionszeile +0x5 = 20; RE2-Zeile 5; Rakete ueber den Applier
 *     (Zeile 17 -> Art 9 -> +0x5 = 18, Tod).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_inventory.h"
#include "re15_werfer.h"
#include "re2_fx.h"
#include "re15_ai_flavor.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

extern re15_actor_t g_actors[];
extern unsigned re15_re2z_weapon_id(unsigned w);
extern const uint8_t re15_react_table[11];

static int s_fails = 0;
#define CHECK(nr, c, ...) do { if (!(c)) { printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); s_fails++; } \
                               else { printf("ok   %d\n", (nr)); } } while (0)

static uint16_t u16(const uint8_t *b, int o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); }
static int16_t  s16(const uint8_t *b, int o) { return (int16_t)u16(b, o); }

/* ---- Spione der RE2-FX-Maschine ---------------------------------------------------------- */
static int      s_se_n;  static uint32_t s_se_code[64];
static void se_spion(uint32_t code, const int32_t pos[3]) { (void)pos; if (s_se_n < 64) s_se_code[s_se_n] = code; s_se_n++; }
static int se_zaehl(uint32_t code) { int n = 0; for (int i = 0; i < s_se_n && i < 64; i++) if (s_se_code[i] == code) n++; return n; }

static int      s_app_n; static uint32_t s_app_hit[64]; static int16_t s_app_box[64][4]; static int32_t s_app_p[64][3];
static int      s_app_ret_ab = -1;      /* ab diesem Aufruf (0-basiert) liefert der Applier 1 */
static int app_spion(const int32_t p[3], int16_t gier, const int16_t box[4], uint32_t hitcode)
{
    (void)gier;
    if (s_app_n < 64) { s_app_hit[s_app_n] = hitcode; memcpy(s_app_box[s_app_n], box, 8); memcpy(s_app_p[s_app_n], p, 12); }
    int r = (s_app_ret_ab >= 0 && s_app_n >= s_app_ret_ab) ? 1 : 0;
    s_app_n++;
    return r;
}
static int app_zaehl(uint32_t hitcode) { int n = 0; for (int i = 0; i < s_app_n && i < 64; i++) if (s_app_hit[i] == hitcode) n++; return n; }

static int32_t boden_flach(const int32_t p[3], int r, uint32_t mask, int a3, int *kontakt)
{ (void)r; (void)mask; (void)a3; *kontakt = (p[1] > 0); return 0; }

static uint8_t *s_esp; static size_t s_esp_n;
static int laden(void)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/../RE2/CORE00.ESP", RE15_XSTR(RE15_ASSETS_PATH));
    FILE *f = fopen(p, "rb");
    if (!f) { printf("kann %s nicht oeffnen\n", p); return -1; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    s_esp = (uint8_t *)malloc((size_t)n);
    if (!s_esp || fread(s_esp, 1, (size_t)n, f) != (size_t)n) { fclose(f); return -1; }
    fclose(f); s_esp_n = (size_t)n;
    return re2fx_register_core(s_esp, s_esp_n);
}
static void start(void)
{
    re2fx_reset(); re15_re2z_rng_reset();
    s_se_n = 0; s_app_n = 0; s_app_ret_ab = -1;
    re2fx_se_hook = se_spion; re2fx_applier = app_spion; re2fx_boden_hook = boden_flach;
    re2fx_wasser_hook = NULL;
    re2fx_boden_basis_setzen(0);
}
/* Einheitsmatrix mit Verschiebung T (RE2 MATRIX: m[3][3] s16 @+0, t[3] s32 @+20). */
static void mtx_t(uint8_t m[32], int32_t tx, int32_t ty, int32_t tz)
{
    memcpy(m, re2fx_einheitsmatrix, 32);
    uint32_t t[3] = { (uint32_t)tx, (uint32_t)ty, (uint32_t)tz };
    for (int k = 0; k < 3; k++) { m[20 + 4*k] = (uint8_t)t[k]; m[21 + 4*k] = (uint8_t)(t[k] >> 8); m[22 + 4*k] = (uint8_t)(t[k] >> 16); m[23 + 4*k] = (uint8_t)(t[k] >> 24); }
}
static int lebendig(void) { int n = 0; for (int i = 0; i < RE2FX_PLAETZE; i++) if (u16(re2fx_platz(i), 0x18) != 0) n++; return n; }

/* ========================================================================================== */
static void teil_a(void)
{
    printf("== A Daten\n");
    CHECK(1, re15_werfer_ist(15) && re15_werfer_ist(18) && !re15_werfer_ist(14) && !re15_werfer_ist(20), "werfer_ist 15..18");
    CHECK(2, re15_werfer_nachladbar(15) && re15_werfer_nachladbar(16) && re15_werfer_nachladbar(17) &&
             re15_werfer_nachladbar(20) && !re15_werfer_nachladbar(18) && !re15_werfer_nachladbar(19), "nachladbar 15/16/17/20");
    CHECK(3, re15_werfer_bank_id(16) == 15 && re15_werfer_bank_id(17) == 15 && re15_werfer_bank_id(15) == 15 &&
             re15_werfer_bank_id(18) == 18 && re15_werfer_bank_id(20) == 20, "Bank 16/17 -> W0F");
    /* PL00W0F (11 Clips): 6->4 Heben, 7->5 Feuer, 8->6 Hold, 9/10/11/12 -> 7/8/9/10, 13 -> 6 */
    CHECK(4, re15_werfer_clip_remap(15, 11, 6) == 4 && re15_werfer_clip_remap(15, 11, 7) == 5 &&
             re15_werfer_clip_remap(16, 11, 8) == 6 && re15_werfer_clip_remap(17, 11, 11) == 9 &&
             re15_werfer_clip_remap(15, 11, 12) == 10 && re15_werfer_clip_remap(15, 11, 13) == 6, "Clip-Umsetzung 11-Clip-Bank");
    CHECK(5, re15_werfer_clip_remap(15, 14, 7) == 7 && re15_werfer_clip_remap(18, 14, 7) == 7 &&
             re15_werfer_clip_remap(20, 14, 13) == 13 && re15_werfer_clip_remap(3, 11, 7) == 7, "keine Umsetzung bei 14 Clips / anderen Ids");
    CHECK(6, re15_werfer_recoil_break(15) == 10 && re15_werfer_recoil_break(18) == 10 &&
             re15_werfer_recoil_break(20) == 10 && re15_werfer_recoil_break(7) == -1, "Rueckstoss-Schwelle 10 fuer 15..18/20");

    /* Magazin + Munitions-Records: Waffe zuerst (Platz 0/1 bei breiten Waffen), Munition dahinter
     * (Platz-0-Eigenheit von FUN_8004eb70 `slt zero,slot`). */
    re15_inv_init();
    CHECK(7, re15_inv_grant(15, 6) == 0 && re15_inv_grant(0x19, 6) == 0, "GL Explosiv + EXPLOSIVE RND einlegen");
    re15_player_set_equipped_weapon(15);
    CHECK(8, re15_ammo_reserve_slot() > 0, "GL 15: Reserve EXPLOSIVE RND (0x19) erkannt (Record 0x80074cb4 `19 0f`)");
    for (int k = 0; k < 6; k++) re15_ammo_consume();
    CHECK(9, !re15_ammo_mag_nonzero(), "GL 15: 6 Schuss verbraucht (Magazin 6 @0x80074dd4)");
    re15_ammo_reload_exec();
    CHECK(10, re15_ammo_mag_nonzero() && re15_inv_find_item(0x19) < 0, "GL 15: Nachladen 6 aus 6 -> Schachtel weg");

    re15_inv_init();
    re15_inv_grant(16, 6); re15_inv_grant(0x1a, 12); re15_player_set_equipped_weapon(16);
    CHECK(11, re15_ammo_reserve_slot() > 0, "GL 16: ACID ROUNDS (0x1a) erkannt (Record 0x80074cb8)");
    re15_inv_init();
    re15_inv_grant(17, 6); re15_inv_grant(0x1b, 12); re15_player_set_equipped_weapon(17);
    CHECK(12, re15_ammo_reserve_slot() > 0, "GL 17: INCEND. ROUNDS (0x1b) erkannt (Record 0x80074cbc)");
    re15_inv_init();
    re15_inv_grant(18, 4); re15_inv_grant(0x19, 6); re15_inv_grant(0x17, 6); re15_player_set_equipped_weapon(18);
    CHECK(13, re15_ammo_reserve_slot() == 0, "Rakete 18: keine Munitionsart (Record NULL, RE2 Id 17 ohne Satz)");
    re15_inv_init();
    re15_inv_grant(20, 6); re15_inv_grant(0x17, 6); re15_player_set_equipped_weapon(20);
    CHECK(14, re15_ammo_reserve_slot() > 0, "Python 20: MAGNUM BULLETS (0x17) erkannt (PORT-WAHL)");
    re15_inv_init();
    re15_inv_grant(15, 6); re15_inv_grant(0x1a, 6); re15_player_set_equipped_weapon(15);
    CHECK(15, re15_ammo_reserve_slot() == 0, "GL 15 nimmt KEINE Saeure-Runden");
}

/* ========================================================================================== */
static int gl_runde_spawn(int waffe, int32_t ty)
{
    uint8_t m[32]; mtx_t(m, 0, ty, 0);
    static const int16_t OG[4] = { 120, 1200, 0, 0 };
    re15_inv_init(); re15_inv_grant(waffe, 6); re15_player_set_equipped_weapon(waffe);
    uint32_t a0 = (waffe == 15) ? 0x020C0A00u : 0x020C1000u;
    int i = re2fx_spawn_sofort(a0, 0, m, OG);
    if (waffe == 15 && i >= 0 && i < RE2FX_PLAETZE) {   /* Runde 1 der Explosiv-Ueberschreibung @0x80044c18-64 */
        uint8_t *b = re2fx_platz_sonde(i);
        b[0x08] = (uint8_t)-10; b[0x0C] = 0; b[0x0D] = 0; b[0x0E] = (uint8_t)600; b[0x0F] = (uint8_t)(600 >> 8); b[0x10] = 0; b[0x11] = 0;
    }
    return i;
}

static void teil_b(void)
{
    printf("== B GL-Runde\n");
    start();
    int i = gl_runde_spawn(15, -5000);
    CHECK(20, i == 95, "Spawn 0x020C0A00 auf Platz 95, ist %d", i);
    const uint8_t *b = re2fx_platz(i);
    CHECK(21, u16(b, 0x18) == 0xA003 && b[0x00] == 0 && b[0x01] == 17 && b[0x02] == 47 && b[0x03] == 47,
          "Step Bank 2 Skr 4: OpA 0 OpB 17 step[2]=47 step[3]=47, Status 0xA003");
    re2fx_tick();                                       /* Bild 1: Op 17 -> Op 15 (erster Flugschritt) */
    b = re2fx_platz(i);
    CHECK(22, re2fx_op_zaehler(17) == 1 && b[0x00] == 22 && b[0x01] == 15 && b[0x1B] == 0 && u16(b, 0x18) == 0xB403 && b[0x21] == 18,
          "Op 17: OpA 22, OpB 15, Art 0 (Explosiv), Status 0xB403, Anim 18 (@0x8001f1b8-f0)");
    CHECK(23, b[0x0B] >= 9 && b[0x0B] <= 11, "Lebensdauer 10 + rng%%3 - 1 nach dem ersten Flugschritt, ist %d", b[0x0B]);
    /* Op 17 ruft Op 15 DIREKT (`jal 0x8001ed9c` @0x8001f288, nicht ueber die Tabelle) — der
     * Tabellen-Zaehler fuer 15 bleibt im Spawn-Bild 0, die zwei Applier-Aufrufe zeigen den Flugschritt. */
    CHECK(24, re2fx_op_zaehler(15) == 0 && s_app_n == 2 && s_app_hit[0] == 0x30009u && s_app_hit[1] == 0x30009u,
          "Op 15 im Bild 1 (aus Op 17): zwei Applier-Aufrufe 0x30009 (y+1000 / y-1000), n=%d", s_app_n);
    CHECK(25, s_app_box[0][0] == -1400 && s_app_box[0][1] == 0 && s_app_box[0][2] == 350 && s_app_box[0][3] == 250,
          "Flug-Box {-1400,0,350,250} @0x80010900");
    CHECK(26, s_app_p[0][1] == s_app_p[1][1] + 2000, "Pruefpunkte y+1000 und y-1000 (@0x8001eec8/@0x8001eef8)");
    int16_t y1 = s16(b, 0x36);
    re2fx_tick();                                       /* Bild 2 */
    b = re2fx_platz(i);
    CHECK(27, re2fx_op_zaehler(22) == 1 && re2fx_op_zaehler(15) == 1 && s_app_n == 4,
          "Bild 2: Op 22 (Rauchspur, Op A) + Op 15 (Op B ueber die Tabelle), vier Applier-Aufrufe (22=%u 15=%u n=%d)",
          re2fx_op_zaehler(22), re2fx_op_zaehler(15), s_app_n);
    CHECK(28, s16(b, 0x36) > y1, "Runde fliegt (vel.y 600 im Knochenrahmen = hier Welt-y): %d -> %d", y1, s16(b, 0x36));
    /* Wand/Boden: die flache Ebene liegt bei y 0 (kontakt = p.y > 0) -> die Runde erreicht sie und explodiert */
    int t;
    for (t = 0; t < 20 && re2fx_op_zaehler(47) == 0; t++) re2fx_tick();
    CHECK(29, re2fx_op_zaehler(47) == 1, "Wandkontakt -> Rueckprall -> Op[step[3]+Art] = Op 47 (nach %d weiteren Bildern)", t);
    CHECK(30, se_zaehl(0x01110001u) == 1, "Op 47 Sub 12: SE 0x01110001 (@0x80020d40-48)");
    CHECK(31, app_zaehl(0x10020009u) == 2, "Op 47 Sub 12: Schaden 0x10020009 zweimal (y, y+900), ist %d", app_zaehl(0x10020009u));
    {   int k = -1; for (int q = 0; q < s_app_n && q < 64; q++) if (s_app_hit[q] == 0x10020009u) { k = q; break; }
        CHECK(32, k >= 0 && s_app_box[k][0] == -2000 && s_app_box[k][2] == 1000 && s_app_box[k][3] == 500 &&
                  s_app_p[k + 1][1] == s_app_p[k][1] + 900, "Explosions-Box {-2000,0,1000,500} @0x80010918, zweiter Punkt y+900"); }
    b = re2fx_platz(i);
    CHECK(33, u16(b, 0x18) == 0x8400 && b[0x01] == 47 && u16(b, 0x12) == 1, "Op 47 Phase 0: Status 0x8400, OpB 47, Phase 1");
    CHECK(34, lebendig() >= 4, "drei Kinder 0x031F/0x040C/0x041D + Platz, lebendig=%d", lebendig());
    for (t = 0; t < 4; t++) re2fx_tick();               /* Phasen 1..4 */
    b = re2fx_platz(i);
    CHECK(35, u16(b, 0x18) == 0 && re2fx_op_zaehler(47) == 5, "nach Phase 4: Platz frei (@0x80020f14-1c), Kind 0x04152700");
    CHECK(36, re2fx_op_unbekannt() == 0, "kein unbekannter Op (%u)", re2fx_op_unbekannt());

    /* Gegnertreffer im Flug: Applier liefert 1 beim ersten Aufruf -> Status |= 0x80, sofort Op 47 */
    start(); s_app_ret_ab = 0;
    i = gl_runde_spawn(15, -20000);
    re2fx_tick();
    CHECK(37, re2fx_op_zaehler(47) == 1 && se_zaehl(0x01110001u) == 1 && app_zaehl(0x10020009u) == 2,
          "Treffer im Flug -> Explosion im selben Bild (`bne s0,zero` @0x8001ef14)");

    /* Saeure (16 -> RE2 11 -> Art 2 -> Op 49), Brand (17 -> RE2 10 -> Art 1 -> Op 48) */
    start(); i = gl_runde_spawn(16, -5000); re2fx_tick();
    b = re2fx_platz(i);
    CHECK(38, b[0x1B] == 2 && b[0x0B] == 14, "Saeure: Art 2, Lebensdauer 15-1 (`sb a2(=15),11` @0x8001f284)");
    for (t = 0; t < 30 && re2fx_op_zaehler(49) == 0; t++) re2fx_tick();
    CHECK(39, re2fx_op_zaehler(49) >= 1 && se_zaehl(0x01130001u) == 1 && re2fx_op_zaehler(47) == 0, "Saeure -> Op 49 + SE 0x01130001");
    start(); i = gl_runde_spawn(17, -5000); re2fx_tick();
    b = re2fx_platz(i);
    CHECK(40, b[0x1B] == 1, "Brand: Art 1");
    for (t = 0; t < 30 && re2fx_op_zaehler(48) == 0; t++) re2fx_tick();
    CHECK(41, re2fx_op_zaehler(48) >= 1 && se_zaehl(0x01120001u) == 1, "Brand -> Op 48 + SE 0x01120001");
    CHECK(42, app_zaehl(0x3000Au) >= 1, "Brand-Runde fliegt mit Hitcode 0x3000A (0x30009 + Art 1)");

    /* Lebensdauer: ohne Kontakt (Ebene weit unten) explodiert die Explosiv-Runde nach 10..12 Bildern */
    start(); i = gl_runde_spawn(15, -30000);
    for (t = 0; t < 20 && re2fx_op_zaehler(47) == 0; t++) re2fx_tick();
    CHECK(43, t >= 9 && t <= 12, "Lebensdauer 10..12 Bilder (`lbu v0,11 / bne` @0x8001ef28-30), explodiert nach %d", t);
}

/* ========================================================================================== */
static void teil_c(void)
{
    printf("== C Rakete\n");
    start();
    uint8_t m[32]; mtx_t(m, 0, -20000, 0);
    static const int16_t OM[4] = { 0, 1100, 0, 0 };
    int i = re2fx_spawn_sofort(0x020D1000u, 0, m, OM);
    const uint8_t *b = re2fx_platz(i);
    CHECK(50, i >= 0 && b[0x00] == 1 && b[0x01] == 0 && b[0x02] == 18, "Bank 2 Skr 5 Step 0: OpA 1, Anim 18");
    re2fx_tick();
    b = re2fx_platz(i);
    CHECK(51, b[0x00] == 23 && b[0x01] == 24 && b[0x02] == 47 && b[0x03] == 47 && s16(b, 0x0E) == 768,
          "Step 1: OpA 23 OpB 24 step[2]=step[3]=47, vel.y 768 (Skript)");
    int n0 = s_app_n;
    re2fx_tick();
    CHECK(52, re2fx_op_zaehler(23) >= 1 && re2fx_op_zaehler(24) >= 1, "Op 23 (Rauchspur) + Op 24 je Bild");
    CHECK(53, s_app_n == n0 + 2 && s_app_hit[n0] == 0x30011u && s_app_hit[n0 + 1] == 0x20011u,
          "Op 24: Applier 0x30011 (Gegner) dann 0x20011 (Bodenhoehe), n=%d", s_app_n - n0);
    CHECK(54, s_app_box[n0][0] == -800 && s_app_box[n0][2] == 400 && s_app_box[n0][3] == 200, "Raketen-Box {-800,0,400,200} @0x80010908");
    /* Treffer -> Op 47 Sub 13: SE 0x01140001, KEIN 0x10020009, drei Kinder */
    s_app_ret_ab = s_app_n;
    int a10 = app_zaehl(0x10020009u);
    re2fx_tick();
    CHECK(55, re2fx_op_zaehler(47) == 1 && se_zaehl(0x01140001u) == 1 && se_zaehl(0x01110001u) == 0,
          "Treffer 0x30011 -> Op 47 Sub 13: SE 0x01140001 (@0x80020d3c/dc0-c4)");
    CHECK(56, app_zaehl(0x10020009u) == a10, "Sub 13: kein Flaechenschaden (`bne v1,12` @0x80020d38)");
    b = re2fx_platz(i);
    CHECK(57, u16(b, 0x18) == 0x8400 && lebendig() >= 4, "Explosion: Status 0x8400 + Kinder (Skalen +2560)");
    /* Wand: Rueckprall + Op[step[3]] */
    start();
    mtx_t(m, 0, -500, 0);
    i = re2fx_spawn_sofort(0x020D1000u, 0, m, OM);
    re2fx_tick();
    int t; for (t = 0; t < 10 && re2fx_op_zaehler(47) == 0; t++) re2fx_tick();
    CHECK(58, re2fx_op_zaehler(47) == 1 && se_zaehl(0x01140001u) == 1, "Wandkontakt -> Rueckprall -> Op 47 (nach %d Bildern)", t);
    CHECK(59, re2fx_op_unbekannt() == 0, "kein unbekannter Op");
}

/* ========================================================================================== */
static void teil_d(void)
{
    printf("== D Flammenstrahl\n");
    start();
    uint8_t m[32]; mtx_t(m, 0, -20000, 0);
    static const int16_t OF[4] = { 150, 1200, 0, 0 };
    int i = re2fx_spawn_sofort(0x031D1200u, 0, m, OF);
    const uint8_t *b = re2fx_platz(i);
    CHECK(60, i >= 0 && b[0x00] == 1 && u16(b, 0x3A) == 0x1200, "Bank 3 Skr 5: OpA 1, Skala 0x1200");
    re2fx_tick();                                       /* Bild 1: Op 1 (Update-Pass) laedt Step 1, Op 70 laeuft im Draw-Pass */
    b = re2fx_platz(i);
    CHECK(61, b[0x00] == 0 && b[0x01] == 70 && b[0x02] == 9 && (int8_t)b[0x09] == 20 && s16(b, 0x0E) == 40,
          "Step 1: OpB 70, Zaehler 10-1, acc.y 20, vel.y 20+20 nach der Physik; ist opB=%d z=%d acc=%d vel=%d",
          b[0x01], b[0x02], (int8_t)b[0x09], s16(b, 0x0E));
    int16_t y0 = s16(b, 0x36);
    re2fx_tick();
    b = re2fx_platz(i);
    CHECK(62, re2fx_op_zaehler(70) == 2 && b[0x02] == 8 && s_app_n == 2 && s_app_hit[s_app_n - 1] == 0x20010u,
          "Op 70 je Bild: Zaehler--, Applier 0x20010 (n=%d)", s_app_n);
    CHECK(63, s_app_box[s_app_n - 1][0] == -1400 && s_app_box[s_app_n - 1][2] == 400 && s_app_box[s_app_n - 1][3] == 200,
          "Flammen-Box {-1400,0,400,200} @0x80010964");
    CHECK(64, s16(b, 0x36) > y0, "Strahl bewegt sich vorwaerts (vel.y 20 + acc 20)");
    for (int t = 0; t < 8; t++) re2fx_tick();           /* Bilder 3..10: Zaehler 7..0 */
    b = re2fx_platz(i);
    CHECK(65, b[0x02] == 0 && (int8_t)b[0x09] == -10, "Zaehler 0 -> acc.y := -10 (@0x80023268-70); ist z=%d acc=%d", b[0x02], (int8_t)b[0x09]);
    /* Treffer -> Nachbrennen */
    s_app_ret_ab = s_app_n;
    re2fx_tick();
    b = re2fx_platz(i);
    CHECK(66, b[0x03] == 1 && b[0x02] == 6 && b[0x21] == 12 && s16(b, 0x0E) == 20 && b[0x08] == 0 && b[0x09] == 0,
          "Treffer: step[3]=1, step[2]=6, Anim 12, vel.y 20, acc 0 (@0x800234b4-538)");
    int lb = lebendig();
    for (int t = 0; t < 3; t++) re2fx_tick();           /* step[2] 6 -> 3: Puff 0x040C2000 */
    CHECK(67, lebendig() >= lb + 1, "step[2] == 3: Kind 0x040C2000 (@0x8002329c-b8), lebendig %d -> %d", lb, lebendig());
    CHECK(68, re2fx_op_unbekannt() == 0, "kein unbekannter Op");
}

/* ========================================================================================== */
static void teil_e(void)
{
    printf("== E Fuel-Takt\n");
    re15_werfer_reset();
    re15_inv_init(); re15_inv_grant(14, 3); re15_player_set_equipped_weapon(14);
    int ok = 1;
    for (int k = 0; k < 7; k++) ok &= (re15_werfer_fuel_bild() == 1);
    CHECK(70, ok && re15_inv_find_item(14) >= 0, "7 Bilder: kein Verbrauch (`slti v0,v0,8` @0x8006a1ac)");
    int r = re15_werfer_fuel_bild();
    int slot = re15_inv_find_item(14);
    CHECK(71, r == 1 && slot >= 0 && g_inv.slots[slot].qty == 1, "8. Bild: zwei Einheiten (3 -> 1), ist %d", slot >= 0 ? g_inv.slots[slot].qty : -1);
    for (int k = 0; k < 7; k++) ok &= (re15_werfer_fuel_bild() == 1);
    r = re15_werfer_fuel_bild();
    CHECK(72, ok && r == 0 && g_inv.slots[slot].qty == 0, "16. Bild: letzte Einheit -> 0 -> leer (`bne s0,zero` @0x8006a1fc -> 0x8006a204)");
    CHECK(73, re15_werfer_fuel_bild() == 1, "danach wieder 7 Bilder Takt ohne Pruefung");
}

/* ========================================================================================== */
static re15_actor_t *mk(int slot, uint8_t type, int32_t x, int32_t y, int32_t z, int16_t hp)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = type; e->x = x; e->y = y; e->z = z; e->hp = hp;
    e->state = 1; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, type);
    return e;
}
static void teil_f(void)
{
    printf("== F Python / Rakete gegen Zombie (RE1.5-KI)\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->hp = 100; pl->state = 1; pl->x = 100000; pl->y = 0; pl->z = 100000; pl->rot_y = 0;
    re15_player_apply_hitbox(pl);
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) memset(&g_actors[s], 0, sizeof g_actors[s]);
    re15_actor_t *z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 100800, 0, 100000, 75);   /* 800 vor Leon (Gier 0 = +x) */
    re15_inv_init(); re15_inv_grant(20, 6); re15_player_set_equipped_weapon(20);
    int hit = re15_player_weapon_fire(20);
    CHECK(80, hit != 0, "Python trifft im Schuss-Streifen (wie w7, 0x80012574)");
    CHECK(81, z->hp < 75 && z->state == 3 && z->sub_state_1 == 20, "Python: Schaden Spalte 7 (>= 200) -> Tod, +0x5 = 20; hp=%d st=%d ss1=%d", z->hp, z->state, z->sub_state_1);
    CHECK(82, re15_re2z_weapon_id(20) == 5, "RE2-Zeile der Python = 5 (Magnum)");
    CHECK(83, re15_react_table[6] == 15 && re15_react_table[7] == 16 && re15_react_table[8] == 17 &&
              re15_react_table[9] == 18 && re15_react_table[10] == 20, "DAT_8006f430[6..10] = 15,16,17,18,20");

    /* Rakete ueber den Applier (Hitcode 0x30011 -> Art 9 -> +0x5 = 18) */
    z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 100800, 0, 100000, 75);
    const int32_t P[3] = { 100800, -800, 100000 };
    static const int16_t box[4] = { -800, 0, 400, 200 };
    int n = re15_re2_gl_apply(P, 0, box, 0x30011u);
    CHECK(84, n != 0 && z->hp < 0 && z->state == 3 && z->sub_state_1 == 18, "Rakete: Applier-Treffer -> Art 9 (1000/900) -> Tod, +0x5 = 18; hp=%d ss1=%d", z->hp, z->sub_state_1);
    /* GL Explosiv ueber den Applier (0x10020009 -> Art 6, +0x5 = 15) */
    z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 100800, 0, 100000, 250);
    static const int16_t box2[4] = { -2000, 0, 1000, 500 };
    n = re15_re2_gl_apply(P, 0, box2, 0x10020009u);
    CHECK(85, n != 0 && z->hp < 250 && z->sub_state_1 == 15, "GL Explosiv: Art 6 (100 bzw. RE2 200) -> +0x5 = 15; hp=%d ss1=%d", z->hp, z->sub_state_1);
    /* Flamme ueber den Applier (0x20010 -> Art 5 -> +0x5 = 14), Sperre 5 (w1 der Zeile 16) */
    z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 100800, 0, 100000, 250);
    static const int16_t box3[4] = { -1400, 0, 400, 200 };
    n = re15_re2_gl_apply(P, 0, box3, 0x20010u);
    CHECK(86, n != 0 && z->hp < 250 && z->sub_state_1 == 14, "Flamme: Art 5 -> +0x5 = 14; hp=%d ss1=%d", z->hp, z->sub_state_1);
    CHECK(87, z->re2_gl_sperre == 5 || z->re2_gl_sperre == 15, "Flammen-Sperre aus w1 (5) bzw. 15, ist %d", z->re2_gl_sperre);
}

/* ---- G: Waffenrahmen des Leon-Granatwerfers PL00W0F (re15_werfer_rahmen) -------------------- */
static void teil_g(void)
{
    /* Einheitsmatrix: Spalte 1 (Lauf) = (sin, cos, 0), Spalte 0 (oben) = (cos, -sin, 0) — das Netz
     * PL00W0F ist um 35,62 Grad von +y nach +x gedreht (MD1 @0x50A8: Laufring-Mitte (377.5,852) ->
     * Muendungsring-Mitte (706,1310.5), Punkte @0x518C-0x522C). */
    int32_t r[9] = { 4096, 0, 0,  0, 4096, 0,  0, 0, 4096 };
    int16_t ofs[4] = { 120, 1200, 0, 0 };
    int n = re15_werfer_rahmen(15, 11, r, ofs);
    CHECK(90, n == 1 && r[1] == 2386 && r[4] == 3330 && r[7] == 0, "Laufachse im Knochenrahmen (2386,3330,0), ist (%d,%d,%d)", (int)r[1], (int)r[4], (int)r[7]);
    CHECK(91, r[0] == 3330 && r[3] == -2386 && r[6] == 0 && r[8] == 4096, "Hochachse (3330,-2386,0), z unveraendert; ist (%d,%d,%d) z=%d", (int)r[0], (int)r[3], (int)r[6], (int)r[8]);
    CHECK(92, ofs[0] == -204 && ofs[1] == 1717 && ofs[2] == 3, "Muendungsversatz im Netzrahmen {-204,1717,3}, ist {%d,%d,%d}", ofs[0], ofs[1], ofs[2]);
    /* Muendungspunkt im Knochenrahmen = M' * ofs = Ringmitte (706,1310.5) + 240 laengs - 14.5 quer */
    {
        const int32_t mx = (r[0] * ofs[0] + r[1] * ofs[1]) >> 12, my = (r[3] * ofs[0] + r[4] * ofs[1]) >> 12;
        CHECK(93, mx > 706 + 100 && mx < 706 + 160 && my > 1310 + 180 && my < 1310 + 220,
              "Muendungspunkt (%d,%d) = Ringmitte (706,1310) + 240 entlang (0.58,0.81) - 14.5 quer", (int)mx, (int)my);
    }
    /* Die gemessene Knochenachse der Haltepose (30,5 Grad abwaerts: Spalte 1 = (3478,2079), y nach
     * unten) wird mit der dazu senkrechten Hochachse zur fast waagerechten Laufachse. */
    {
        int32_t k[9] = { 2079, 3478, 0,   -3504, 2079, 0,   0, 0, 4096 };
        (void)re15_werfer_rahmen(16, 11, k, NULL);
        CHECK(94, k[1] > 3900 && k[4] > -500 && k[4] < 100, "Haltepose: Laufachse nach dem Rahmen fast waagerecht, ist (%d,%d)", (int)k[1], (int)k[4]);
    }
    /* Kein Eingriff: Standard-Baenke (14 Clips = Elza PL04W0F, Netz auf +y), Rakete, Flamme, Python */
    int32_t e[9] = { 4096, 0, 0,  0, 4096, 0,  0, 0, 4096 };
    int16_t o2[4] = { 120, 1200, 0, 0 };
    CHECK(95, re15_werfer_rahmen(15, 14, e, o2) == 0 && e[1] == 0 && e[4] == 4096 && o2[0] == 120 && o2[1] == 1200,
          "14-Clip-Bank (Elza PL04W0F): unveraendert");
    CHECK(96, re15_werfer_rahmen(18, 11, e, o2) == 0 && re15_werfer_rahmen(14, 11, e, o2) == 0 && re15_werfer_rahmen(20, 11, e, o2) == 0,
          "Rakete/Flamme/Python: kein Rahmen");
    CHECK(97, re15_werfer_rahmen(17, 11, e, o2) == 1, "GL Brand fuehrt Leons W0F-Netz (re15_werfer_bank_id) -> Rahmen");
}

int main(void)
{
    if (laden() != 0) { printf("CORE00.ESP (RE2) fehlt/ungueltig\n"); return 2; }
    teil_a(); teil_b(); teil_c(); teil_d(); teil_e(); teil_f(); teil_g();
    if (s_fails) { printf("test_r35_werfer: %d FAILURES\n", s_fails); return 1; }
    printf("test_r35_werfer: OK\n");
    return 0;
}
