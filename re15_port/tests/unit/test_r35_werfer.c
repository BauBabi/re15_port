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
#include "re15_rdt.h"
#include "re15_room.h"

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
    CHECK(9, !re15_ammo_mag_nonzero(), "GL 15: 6 Schuss verbraucht (Magazin 6 @0x80074e5c)");
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

/* ---- H: Nachbesserung 1 (Abnahme 0, Maengel M1/M2/M4; Dossier §8) -------------------------- */
extern int g_test_re2arms_last_id, g_test_re2arms_last_satz, g_test_re2arms_count;
static int bytes_at(const char *pfad, long ofs, uint8_t out[4])
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return -1;
    int ok = (fseek(f, ofs, SEEK_SET) == 0 && fread(out, 1, 4, f) == 4);
    fclose(f);
    return ok ? 0 : -1;
}
/* Rakete in der Welt: lokales +y (Flug) -> Welt -z, lokales +x (oben) -> Welt -y. */
static void mtx_minus_z(uint8_t m[32], int32_t tx, int32_t ty, int32_t tz)
{
    static const int16_t R[9] = { 0, 0, 4096,  -4096, 0, 0,  0, -4096, 0 };
    mtx_t(m, tx, ty, tz);
    for (int k = 0; k < 9; k++) { m[2*k] = (uint8_t)R[k]; m[2*k + 1] = (uint8_t)((uint16_t)R[k] >> 8); }
}
static int32_t s_expl_z; static int s_expl_n;
static void se_spion_lage(uint32_t code, const int32_t pos[3])
{
    se_spion(code, pos);
    if (code == 0x01140001u) { s_expl_n++; s_expl_z = pos[2]; }
}
/* Rakete von (x, -2530, z0) nach -z ueber ROOM1140; Rueckgabe: z der Explosion (0x01140001). */
static int32_t rakete_nach_minus_z(int32_t x, int32_t z0)
{
    start();
    re2fx_boden_hook = NULL;                       /* echter Wand-/Bodentest (werfer_boden) */
    re2fx_se_hook = se_spion_lage; s_expl_n = 0; s_expl_z = 0;
    uint8_t m[32]; mtx_minus_z(m, x, -2530, z0 + 1100);
    static const int16_t OM[4] = { 0, 1100, 0, 0 };
    (void)re2fx_spawn_sofort(0x020D1000u, 0, m, OM);
    int t; for (t = 0; t < 60 && s_expl_n == 0; t++) re2fx_tick();
    printf("     Rakete x=%d ab z=%d: %s nach %d Bildern, z=%d\n", (int)x, (int)z0, s_expl_n ? "Explosion" : "KEINE Explosion", t, (int)s_expl_z);
    return s_expl_n ? s_expl_z : 1;
}
static void teil_h(void)
{
    printf("== H Nachbesserung 1: M2 Leer-Ton Rakete, M4 Fresser-Sperre, M1 Moebel/Kreis-Zellen\n");
    /* M2 — Daten: RE1.5 ARMS12 Satz 1 leer, RE2 ARMS11 Satz 1 belegt (EDH Dateibytes 4..7). */
    {
        char p[1024]; uint8_t b[4];
        snprintf(p, sizeof p, "%s/SOUND/ARMS12.EDH", RE15_XSTR(RE15_ASSETS_PATH));
        int r1 = bytes_at(p, 4, b);
        CHECK(100, r1 == 0 && b[0] == 0xff && b[1] == 0xff && b[2] == 0xff && b[3] == 0xff,
              "RE1.5 ARMS12.EDH Satz 1 = ff ff ff ff (kein Sample -> Klick 0x01010001 stumm)");
        snprintf(p, sizeof p, "%s/../RE2/SOUND/ARMS11.EDH", RE15_XSTR(RE15_ASSETS_PATH));
        int r2 = bytes_at(p, 4, b);
        CHECK(101, r2 == 0 && b[0] == 0x00 && b[1] == 0x00 && b[2] == 0x54 && b[3] == 0x16,
              "RE2 ARMS11.EDH Satz 1 = 00 00 54 16 (belegt)");
    }
    /* M2 — Leerzweig: Rakete -> RE2 ARMS11 Satz 1 (Haltezustand @0x80043868/94-9c), sonst RE1.5-Klick. */
    {
        int n0 = g_test_re2arms_count;
        int r = re15_werfer_leer_ton(18);
        CHECK(102, r == 1 && g_test_re2arms_count == n0 + 1 && g_test_re2arms_last_id == 0x11 && g_test_re2arms_last_satz == 1,
              "leere Rakete: re2arms(0x11, 1), ist r=%d id=%d satz=%d", r, g_test_re2arms_last_id, g_test_re2arms_last_satz);
        int andere = re15_werfer_leer_ton(14) + re15_werfer_leer_ton(15) + re15_werfer_leer_ton(16) +
                     re15_werfer_leer_ton(17) + re15_werfer_leer_ton(20) + re15_werfer_leer_ton(3);
        CHECK(103, andere == 0 && g_test_re2arms_count == n0 + 1, "GL/Flamme/Python/Pistole: Aufrufer spielt den RE1.5-Klick");
    }
    /* M4 — RE2-Fresser: +0x1D3 Bit 0x80 (EXEC[8] P0 @0x80103c04-14) und +0x10E Bit 0x4000 sperren den
     * Applier (Gate 2 @0x80047138-40, Gate 4 @0x80047158-64) — jeder fuer sich. */
    {
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        memset(pl, 0, sizeof *pl);
        pl->active = 1; pl->hp = 100; pl->state = 1; pl->x = 100000; pl->y = 0; pl->z = 100000;
        re15_player_apply_hitbox(pl);
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) memset(&g_actors[s], 0, sizeof g_actors[s]);
        const int32_t P[3] = { 100800, -800, 100000 };
        static const int16_t box[4] = { -800, 0, 400, 200 };
        static const struct { uint8_t b1d3; uint16_t f10e; int treffer; const char *was; } F[4] = {
            { 0x80, 0x4000, 0, "frisst (1D3=80, 10E=4000)" }, { 0x80, 0x0000, 0, "nur 1D3=80 (Gate 2)" },
            { 0x00, 0x4000, 0, "nur 10E=4000 (Gate 4)" },     { 0x00, 0x0000, 1, "aufgestanden (beide frei)" } };
        for (int k = 0; k < 4; k++) {
            re15_actor_t *z = mk(RE15_ACTOR_SLOT_PLAYER + 1, 0x10, 100800, 0, 100000, 50);
            z->state = 1; z->sub_state_1 = 8; z->re2z_self1d3 = F[k].b1d3; z->re2z_f10e = F[k].f10e;
            int n = re15_re2_gl_apply(P, 0, box, 0x30011u);
            CHECK(104 + k, F[k].treffer ? (n != 0 && z->hp < 50) : (n == 0 && z->hp == 50),
                  "Rakete gegen RE2-Zombie %s: Treffer %d hp %d", F[k].was, n, z->hp);
        }
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) memset(&g_actors[s], 0, sizeof g_actors[s]);
    }
    /* M1 — ROOM1140: der Konferenztisch (SCA-Zelle 6) ist eine Zelle der RE1.5-Schusslinie (Band 0,
     * Klasse 3: Wort+10 = 0x0300, FUN_8003dcc4 @0x8003de5c-94) und sperrt die Rakete in Flughoehe
     * -2530 wie die Handgranate (Spur A); neben dem Tisch fliegt sie bis zur Fernwand; die Kreis-Zelle
     * (Typ 3, FUN_8003d6a8) sperrt formgenau. */
    {
        char p[1024];
        snprintf(p, sizeof p, "%s/STAGE1/ROOM1140.RDT", RE15_XSTR(RE15_ASSETS_PATH));
        FILE *f = fopen(p, "rb");
        static uint8_t *rdt = NULL; long n = 0;
        if (f) { fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
                 rdt = (uint8_t *)malloc((size_t)n);
                 if (!rdt || fread(rdt, 1, (size_t)n, f) != (size_t)n) n = 0; fclose(f); }
        memset(&g_room_rdt, 0, sizeof g_room_rdt);
        int ok = (n > 0 && re15_rdt_parse(rdt, (size_t)n, &g_room_rdt) == 0 && g_room_rdt.sca_count > 6);
        g_room_rdt_ok = ok;
        const re15_sca_entry_t *c6 = ok ? &g_room_rdt.sca[6] : NULL;
        CHECK(110, ok && (int16_t)c6->x == -4650 && (int16_t)c6->z == -17450 && c6->width == 12100 && c6->density == 6100 &&
                   c6->type == 1 && c6->u0 == 0xff && c6->u1 == 0x00 && c6->floor == 0x03,
              "ROOM1140 SCA-Zelle 6 = Tisch {x -4650..7450, z -17450..-11350, Typ 1, u0 ff, Wort+10 0x0300}");
        if (ok) {
            int32_t ze = rakete_nach_minus_z(200, -10400);
            CHECK(111, ze != 1 && ze > -11400 && ze <= -10400, "Rakete ueber den Tisch: Explosion an der Tischkante, z=%d", (int)ze);
            ze = rakete_nach_minus_z(9000, -10400);
            CHECK(112, ze != 1 && ze < -18000, "Rakete neben dem Tisch (x 9000): Explosion an der Fernwand, z=%d", (int)ze);
            ze = rakete_nach_minus_z(-4750, -10400);
            CHECK(113, ze != 1 && ze > -13450 && ze < -12000, "Rakete auf den Kreis (-4750,-13900) r 500: Explosion am Kreis, z=%d", (int)ze);
        }
        g_room_rdt_ok = 0; memset(&g_room_rdt, 0, sizeof g_room_rdt);
        start();
    }
}

/* ---- I: Nachbesserung 2 (Abnahme 1, Maengel N1/N2; Dossier §9) ------------------------------- */
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
    return rdt;                                    /* g_room_rdt zeigt hinein; bleibt bis Teilende */
}
/* Geschoss-Rahmen: lokales +y (Flug) -> Welt (ux,0,uz)/4096, lokales +x (oben) -> Welt -y, Spalte 2 = Kreuzprodukt. */
static void mtx_richtung(uint8_t m[32], int32_t tx, int32_t ty, int32_t tz, int32_t ux, int32_t uz)
{
    const int16_t R[9] = { 0, (int16_t)ux, (int16_t)-uz,  -4096, 0, 0,  0, (int16_t)uz, (int16_t)ux };
    mtx_t(m, tx, ty, tz);
    for (int k = 0; k < 9; k++) { m[2*k] = (uint8_t)R[k]; m[2*k + 1] = (uint8_t)((uint16_t)R[k] >> 8); }
}
static int32_t s_i_ex, s_i_ez; static int s_i_n; static uint32_t s_i_code;
static void se_spion_i(uint32_t code, const int32_t pos[3])
{
    se_spion(code, pos);
    if (code == s_i_code && s_i_n == 0) { s_i_ex = pos[0]; s_i_ez = pos[2]; }
    if (code == s_i_code) s_i_n++;
}
/* Rakete mit Muendung (mx, -2530, mz) in Richtung (ux,uz)/4096; schuetze: Lage fuer re2fx_r35_schuetze (NULL = keine).
 * Rueckgabe Bildzahl bis zur Explosion (0x01140001) bzw. -1; Lage der Explosion in *ex/*ez. */
static int rakete_i(int32_t mx, int32_t mz, int32_t ux, int32_t uz, const int32_t *schuetze, int32_t *ex, int32_t *ez)
{
    start();
    re2fx_boden_hook = NULL;                       /* echter Wand-/Bodentest (werfer_boden) */
    re2fx_se_hook = se_spion_i; s_i_code = 0x01140001u; s_i_n = 0; s_i_ex = s_i_ez = 0;
    uint8_t m[32]; mtx_richtung(m, mx - ((ux * 1100) >> 12), -2530, mz - ((uz * 1100) >> 12), ux, uz);
    static const int16_t OM[4] = { 0, 1100, 0, 0 };
    int i = re2fx_spawn_sofort(0x020D1000u, 0, m, OM);
    if (schuetze && i >= 0) re2fx_r35_schuetze(i, schuetze[0], schuetze[1]);
    int t; for (t = 0; t < 80 && s_i_n == 0; t++) re2fx_tick();
    *ex = s_i_ex; *ez = s_i_ez;
    printf("     Rakete ab (%d,%d) Richtung (%d,%d)%s: %s nach %d Bildern @(%d,%d)\n", (int)mx, (int)mz, (int)ux, (int)uz,
           schuetze ? " mit Schuetze" : "", s_i_n ? "Explosion" : "KEINE Explosion", t, (int)*ex, (int)*ez);
    return s_i_n ? t : -1;
}
static void teil_i(void)
{
    printf("== I Nachbesserung 2: N1 Formen 2/4..9 + Strecke, N2 Python in der Kritklasse\n");
    int32_t ex = 0, ez = 0;
    /* N1 — ROOM10E0 Zelle 21 = Typ 5 (LAB_8003c734, Verteiler @0x8003af44), Rezept der Abnahme 1 §2.6:
     * Flugbahn der Rakete (-2477,-1657) mit Schritt (87,761). Der feste Teil ist d*(x-X)/w < z-Z (@0x8003c764-7e0). */
    uint8_t *r10e0 = rdt_laden("ROOM10E0");
    {
        const re15_sca_entry_t *c = g_room_rdt_ok && g_room_rdt.sca_count > 21 ? &g_room_rdt.sca[21] : NULL;
        CHECK(120, c && c->width == 2850 && c->density == 3700 && c->x == -3700 && c->z == -1400 && c->type == 5 &&
                   c->u0 == 0xff && c->u1 == 0x00 && c->floor == 0x03,
              "ROOM10E0 SCA-Zelle 21 = {w 2850, d 3700, x -3700, z -1400, Typ 5, u0 ff, u1 00, floor 03} (@RDT 0x758)");
        if (c) {
            /* Formtest selbst: Punkt im festen Dreieck / im leeren Teil des Rechtecks / Strecke ueber die Kante. */
            CHECK(121, re15_werfer_zelle_strecke(c, -2215, 627, -2215, 627) == 1 &&
                       re15_werfer_zelle_strecke(c, -1200, 0, -1200, 0) == 0 &&
                       re15_werfer_zelle_strecke(c, -1200, 0, -1200, 2000) == 1,
                  "Typ 5: (-2215,627) fest, (-1200,0) leer (im Rechteck), Strecke (-1200,0)-(-1200,2000) schneidet die Hypotenuse");
            int b = rakete_i(-2477, -1657, 465, 4069, NULL, &ex, &ez);
            CHECK(122, b > 0 && ez > -1657 && ez < 528,
                  "Rakete 10E0 (Abnahme-Bahn): Explosion vor dem festen Dreieck (z < 528), vorher @(-2070,..,1897); ist z=%d", (int)ez);
            /* GL-Runde IN Bandhoehe (y -1000) durch den LEEREN Teil des Rechtecks: der Rechteck-Vortest allein meldete dort
             * Kontakt; mit dem Formtest (RE2 @0x8004fe34-54) fliegt sie bis zur Hypotenuse (x -1200: z > 1845). */
            start();
            re2fx_boden_hook = NULL;
            re2fx_se_hook = se_spion_i; s_i_code = 0x01110001u; s_i_n = 0; s_i_ex = s_i_ez = 0;
            re15_inv_init(); re15_inv_grant(15, 6); re15_player_set_equipped_weapon(15);
            uint8_t m[32]; mtx_richtung(m, -1200, -880, -3700, 0, 4096);   /* Muendung (-1200,-1000,-2500) */
            static const int16_t OG[4] = { 120, 1200, 0, 0 };
            int i = re2fx_spawn_sofort(0x020C0A00u, 0, m, OG);
            if (i >= 0 && i < RE2FX_PLAETZE) {             /* Runde 1 der Explosiv-Ueberschreibung wie gl_runde_spawn */
                uint8_t *pb = re2fx_platz_sonde(i);
                pb[0x08] = (uint8_t)-10; pb[0x0C] = 0; pb[0x0D] = 0; pb[0x0E] = (uint8_t)600; pb[0x0F] = (uint8_t)(600 >> 8); pb[0x10] = 0; pb[0x11] = 0;
            }
            int t; for (t = 0; t < 40 && s_i_n == 0; t++) re2fx_tick();
            printf("     GL-Runde x -1200 ab z -2500 (y -1000): %s nach %d Bildern @(%d,%d)\n", s_i_n ? "Explosion" : "KEINE", t, (int)s_i_ex, (int)s_i_ez);
            CHECK(123, s_i_n > 0 && s_i_ez > 0 && s_i_ez < 2300,
                  "GL in Bandhoehe: kein Kontakt im leeren Teil des Rechtecks (z -1400..1845), Explosion an der Hypotenuse; ist z=%d", (int)s_i_ez);
        }
    }
    /* N1 — ROOM11C0 Zelle 5 = Typ 2 (Raute, LAB_8003d00c @0x8003d090-9c) — der Affen-Parkplatz: Rakete auf z -12500 nach +x;
     * die Rautenkante liegt dort bei x -6152 (|dx|/10099 + 8327/9093 = 1). */
    uint8_t *r11c0 = rdt_laden("ROOM11C0");
    {
        const re15_sca_entry_t *c = g_room_rdt_ok && g_room_rdt.sca_count > 5 ? &g_room_rdt.sca[5] : NULL;
        CHECK(124, c && c->type == 2 && c->x == -15400 && c->z == -13266 && c->width == 20199 && c->density == 18187 && c->u0 == 0xff,
              "ROOM11C0 SCA-Zelle 5 = Raute {x -15400, z -13266, w 20199, d 18187, u0 ff}");
        if (c) {
            int b = rakete_i(-13000, -12500, 4096, 0, NULL, &ex, &ez);
            CHECK(125, b > 0 && ex > -13000 && ex < -6152,
                  "Rakete 11C0 nach +x: Explosion vor der Rautenkante (x < -6152); ist x=%d", (int)ex);
        }
    }
    /* N1 — Durchtunneln (OFFEN 19): ROOM1000 Kreis-Zelle 15 (Mitte (-1700,-4350), r 500); bei x -2117 ist die Sehne 552 <
     * Schritt 768. Lagen -4058 (vor der Sehne) und -4826 (dahinter): der Punkttest sieht den Kreis nie, die Strecke schon. */
    uint8_t *r1000 = rdt_laden("ROOM1000");
    {
        const re15_sca_entry_t *c = g_room_rdt_ok && g_room_rdt.sca_count > 15 ? &g_room_rdt.sca[15] : NULL;
        CHECK(126, c && c->type == 3 && c->x == -2200 && c->z == -4850 && c->width == 1000,
              "ROOM1000 SCA-Zelle 15 = Kreis {x -2200, z -4850, w 1000}");
        if (c) {
            CHECK(127, re15_werfer_zelle_strecke(c, -2117, -4058, -2117, -4058) == 0 &&
                       re15_werfer_zelle_strecke(c, -2117, -4826, -2117, -4826) == 0 &&
                       re15_werfer_zelle_strecke(c, -2117, -4058, -2117, -4826) == 1,
                  "Kreis: beide Lagen ausserhalb, die Strecke dazwischen schneidet die Sehne");
            int b = rakete_i(-2117, -3290, 0, -4096, NULL, &ex, &ez);
            CHECK(128, b > 0 && ez > -4074,
                  "Rakete ROOM1000 x -2117: Explosion vor dem Kreis (z > -4074), nicht an der Wand dahinter; ist z=%d", (int)ez);
        }
    }
    /* N1 — PORT-WAHL Schuetze -> Muendung: ROOM1140 Kreis (-4750,-13900) r 500 zwischen Schuetze (-4750,-13300) und
     * Muendung (-4750,-14600). Ohne Schuetzenlage (RE2: Punkt je Bild) fliegt sie ueber die Muendung hinaus weiter. */
    uint8_t *r1140 = rdt_laden("ROOM1140");
    if (g_room_rdt_ok) {
        const int32_t S[2] = { -4750, -13300 };
        int b0 = rakete_i(-4750, -14600, 0, -4096, NULL, &ex, &ez);
        const int32_t ez0 = ez;
        int b1 = rakete_i(-4750, -14600, 0, -4096, S, &ex, &ez);
        CHECK(129, b0 > 0 && ez0 < -15368 && b1 > 0 && b1 <= 3 && b1 < b0 && ez > -14700,
              "Muendung hinter dem Kreis: ohne Schuetze Fernwand z=%d (%d Bilder), mit Schuetze sofort z=%d (%d Bilder)",
              (int)ez0, b0, (int)ez, b1);
    }
    g_room_rdt_ok = 0; memset(&g_room_rdt, 0, sizeof g_room_rdt);
    free(r10e0); free(r11c0); free(r1000); free(r1140);
    start();

    /* N2 — Kritklasse: Redhawk (7) und Python (20) setzen +0x93 |= 0x40 (@0x800123b4-b8; 20 = PORT-WAHL §3.5), Typ < 0x20
     * -> HP -1 am Bit (@0x800124fc-1c); Typ 0x27 (Affe) traegt nur das Bit; Pistole (3) keins. */
    {
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        static const struct { int w; uint8_t typ; int16_t hp; int bit; int tot; } K[5] = {
            { 7, 0x10, 2000, 1, 1 }, { 20, 0x10, 2000, 1, 1 }, { 3, 0x10, 2000, 0, 0 }, { 7, 0x27, 5000, 1, 0 }, { 20, 0x27, 5000, 1, 0 } };
        for (int k = 0; k < 5; k++) {
            memset(pl, 0, sizeof *pl);
            pl->active = 1; pl->hp = 100; pl->state = 1; pl->x = 100000; pl->y = 0; pl->z = 100000; pl->rot_y = 0;
            re15_player_apply_hitbox(pl);
            for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) memset(&g_actors[s], 0, sizeof g_actors[s]);
            re15_actor_t *z = mk(RE15_ACTOR_SLOT_PLAYER + 1, K[k].typ, 100800, 0, 100000, K[k].hp);
            re15_inv_init(); re15_inv_grant(K[k].w, 6); re15_player_set_equipped_weapon(K[k].w);
            int hit = re15_player_weapon_fire(K[k].w);
            const int bit = (z->hit_react & 0x40) != 0;
            CHECK(130 + k, hit != 0 && bit == K[k].bit && ((z->hp == -1) == (K[k].tot != 0)),
                  "w%d gegen Typ 0x%02x (hp %d): Treffer %d, +0x93 Bit 0x40 %d (soll %d), hp %d (soll %s)",
                  K[k].w, K[k].typ, K[k].hp, hit, bit, K[k].bit, z->hp, K[k].tot ? "-1" : "> 0");
        }
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) memset(&g_actors[s], 0, sizeof g_actors[s]);
    }
}

int main(void)
{
    if (laden() != 0) { printf("CORE00.ESP (RE2) fehlt/ungueltig\n"); return 2; }
    teil_a(); teil_b(); teil_c(); teil_d(); teil_e(); teil_f(); teil_g(); teil_h(); teil_i();
    if (s_fails) { printf("test_r35_werfer: %d FAILURES\n", s_fails); return 1; }
    printf("test_r35_werfer: OK\n");
    return 0;
}
