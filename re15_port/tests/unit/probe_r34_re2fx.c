/*
 * probe_r34_re2fx — Runde 34 Spur D: die RE2-FX-Maschine (engine/src/re2_fx.c) gegen die
 * Abnahmepunkte aus BAUPLAN §3.3 (Spur C, Teil D) und bau_d.md.
 *
 * Rueckgabe 0 = gruen, sonst die Nummer der ersten verletzten Pruefung.
 *
 * Pruefungen (je mit Negativ-Kontrolle):
 *  1xx Registrierung CORE00.ESP (Ids 03 05 00 01 02 06 07 04, Bank 5 @0x05F0 Skr. 5 Step 00 1b 2e 32 …),
 *      Spawner-Felder FUN_8001cbe8, Pool voll -> 0xFF, unregistrierte Bank -> -1.
 *  2xx Saeure-Aufschlag (Op 49): SE 0x01130001 1x; Kinder 0x030F2000/0x040C2000/0x041D1800 im
 *      Aufschlagbild X, dann je eins 0x031F2000/0x03142000/0x040D2800/0x030F2000 in X+1..X+4, Platz frei
 *      in X+4; Lage der Phase-Kinder = Q + RotY(gier)*B*lokal (O-VB1).
 *  3xx Brand-Aufschlag (Op 48): SE 0x01120001 1x; 2 Kinder + 3 Flammen 0x0505xxxx mit Skala 7168 +
 *      (r%8)*768, Gier-Streuung r%40 / r%80+400 / r%80-400, vel.x 96 + r%25, acc.y 5 + r%8, +0x4A = 1
 *      (unabhaengiger Nachbau des RE2-Stroms @0x80015FE8).
 *  401-414 Flamme ueber flachem Boden: Landung (Op 46) -> Op A 19 / Op B 29, Zaehler 38 + r%8 bzw.
 *      90 + r%11, Lebensdauer (Zustand 2: +0x42 + 1 Bilder, Zustand 1: +0x42 + 2); Applier-Spion nur bei
 *      step[0x16] >= 16 und X-Aspekt >= 0x1001, Box {-600,0,300,150}, Hitcode 0x2002000A.
 *  420-430 Treffer -> Op 50 (Gleiten aus); ohne +0x4A kein Applier; Wand beim Gleiten -> Op 50;
 *      Wand in der Luft -> Op 50, dann Op 64 bei der Landung (kein Brennen, kein Applier); ohne Wand kein Op 64.
 *  440-445 Landung im Luft-Schrumpfen (Zustand 1 bleibt, Tod nach Zaehler + 2).
 *  450-451 Pause 0x10000000 haelt die Maschine an, danach laeuft Phase 0.
 *  601-602 Folgeflammen 0x0504xxxx (Skala x0.8) genau in den Bildern mit step[2] % 15 == 0 und
 *      vel.x >= 61 (Negativ-Kontrolle vel.x 60).
 *  701-705 Aspekt-Folgen der Ops 19/58 (990/980, 1009/1002, 880/800, 1010/1007) Bild fuer Bild gegen
 *      einen unabhaengigen Nachbau, alle Zweige durchlaufen.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "re2_fx.h"
#include "re15_ai_flavor.h"   /* re15_re2z_rng_reset / re15_re2_rand */

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

static uint8_t *s_esp; static size_t s_esp_n;

static int fail(int n, const char *what)
{
    printf("FAIL %d: %s\n", n, what);
    return n;
}

static uint16_t u16(const uint8_t *b, int o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); }
static int16_t  s16(const uint8_t *b, int o) { return (int16_t)u16(b, o); }
static uint32_t u32(const uint8_t *b, int o)
{ return (uint32_t)b[o] | ((uint32_t)b[o + 1] << 8) | ((uint32_t)b[o + 2] << 16) | ((uint32_t)b[o + 3] << 24); }

/* ---- Spione ------------------------------------------------------------------------------ */
static int      s_se_n;
static uint32_t s_se_code[32];
static int32_t  s_se_pos[32][3];
static void se_spion(uint32_t code, const int32_t pos[3])
{
    if (s_se_n < 32) { s_se_code[s_se_n] = code; memcpy(s_se_pos[s_se_n], pos, sizeof s_se_pos[0]); }
    s_se_n++;
}

static int      s_app_n, s_app_ret, s_app_bad;
static uint32_t s_app_hit;
static int      s_app_platz = -1;       /* welcher Platz den Applier gerufen hat (ueber seine Lage) */
static int32_t  s_app_p[3];
static int app_spion(const int32_t p[3], int16_t gier, const int16_t box[4], uint32_t hitcode)
{
    (void)gier;
    s_app_n++; s_app_hit = hitcode; memcpy(s_app_p, p, sizeof s_app_p);
    if (box[0] != -600 || box[1] != 0 || box[2] != 300 || box[3] != 150) s_app_bad++;
    /* Tor-Pruefung: der rufende Platz (Lage x/z gleich, y - 100) muss step[0x16] >= 16 und X > 0x1000 haben. */
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        const uint8_t *b = re2fx_platz(i);
        if (!(u16(b, 0x18) & 0x8000)) continue;
        if (s16(b, 0x34) == p[0] && s16(b, 0x36) - 100 == p[1] && s16(b, 0x38) == p[2] && b[0] == 19) {
            s_app_platz = i;
            if (u16(b, 0x16) < 16 || u16(b, 0x04) < 0x1001) s_app_bad++;
        }
    }
    return s_app_ret;
}

/* ---- unabhaengiger Nachbau des RE2-Zufallsstroms FUN_80015FE8 (Startwert wie re15_re2z_rng_reset) ---- */
static uint32_t s_rs;
static void     rnd_reset(void) { s_rs = 0xD2706CA4u; }
static uint32_t rnd(void)
{
    uint32_t h = (s_rs >> 7) & 0xffu;                  /* `srl v1,v0,7 / andi 0xff` @0x80015ff8-fc */
    uint32_t v = ((h + s_rs) & 0xffu) | (h << 8);      /* @0x80016000-0c */
    s_rs = v & 0xffffu;                                /* `sw v1,0(a0)` @0x80016018 */
    return v & 0xffu;                                  /* `andi v0,v0,0xff` @0x80016014 */
}

/* ---- Hilfen ------------------------------------------------------------------------------ */
static int finde(unsigned bank, unsigned sub, int ab)
{
    for (int i = ab; i >= 0; i--) {
        const uint8_t *b = re2fx_platz(i);
        if (u16(b, 0x18) != 0 && b[0x1C] == bank && b[0x1E] == sub) return i;
    }
    return -1;
}
static int lebendig(void)
{
    int n = 0;
    for (int i = 0; i < RE2FX_PLAETZE; i++) if (u16(re2fx_platz(i), 0x18) != 0) n++;
    return n;
}
static int32_t s_boden_y = 0;   /* flacher Boden fuer den Haken */
static int     s_wand = 0;      /* 1 = Kontakt immer */
static int32_t boden_flach(const int32_t p[3], int r, uint32_t mask, int a3, int *kontakt)
{
    (void)r; (void)mask; (void)a3;
    /* s_wand: 0 = nur Grundebene (P.y > 0, @0x8004fc48-58), 1 = Kontakt immer (Luft und Boden),
     * 2 = Kontakt nur fuer die Gleit-Sonde (Op 29 P.y = y - 100 > -500; Op 28 P.y = y - 900 bleibt frei). */
    *kontakt = (p[1] > 0) || s_wand == 1 || (s_wand == 2 && p[1] > -500);
    return s_boden_y;
}

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
    return 0;
}

static void start(void)
{
    re2fx_reset();
    re15_re2z_rng_reset(); rnd_reset();
    s_se_n = 0; s_app_n = 0; s_app_ret = 0; s_app_bad = 0; s_app_platz = -1;
    re2fx_se_hook = se_spion; re2fx_applier = app_spion;
    re2fx_boden_hook = boden_flach; s_boden_y = 0; s_wand = 0;
}

/* ============================================================================================ */
static int pruef_registrierung(void)
{
    if (s_esp_n != 8572) return fail(101, "CORE00.ESP nicht 8572 Byte");
    static const uint8_t ids[8] = { 3, 5, 0, 1, 2, 6, 7, 4 };
    if (memcmp(s_esp, ids, 8) != 0) return fail(102, "Id-Kopf nicht 03 05 00 01 02 06 07 04");
    /* Negativ-Kontrolle: abgeschnittene Datei -> Fehler, danach nichts registriert. */
    if (re2fx_register_core(s_esp, 8) >= 0) return fail(103, "Rumpf-Datei wurde registriert");
    if (re2fx_spawn(0x05051C00u, 0, re2fx_einheitsmatrix, NULL) != -1) return fail(104, "Spawn ohne Registrierung");
    if (re2fx_register_core(s_esp, s_esp_n) != 0) return fail(105, "Registrierung schlug fehl");
    start();
    int i = re2fx_spawn(0x05051C00u, 123, re2fx_einheitsmatrix, NULL);
    if (i != 95) return fail(106, "erster Spawn nicht auf Platz 95 (@0x8001cc44-6c)");
    const uint8_t *b = re2fx_platz(i);
    static const uint8_t step[14] = { 0x00, 0x1b, 0x2e, 0x32, 0x00, 0x10, 0x00, 0x10, 0x00, 0x05, 0x00, 0x00, 0x60, 0x00 };
    if (memcmp(b, step, sizeof step) != 0) return fail(107, "Step Bank 5 Skr. 5 != 00 1b 2e 32 …");
    if (u16(b, 0x18) != 0x4000) return fail(108, "Status nicht 0x4000 (@0x8001cc80)");
    if (b[0x1C] != 5 || b[0x1E] != 5 || u16(b, 0x3A) != 0x1C00 || s16(b, 0x22) != 123) return fail(109, "Id/Skala/Gier");
    if (u32(b, 0x70) != 0x05F0 + 8 || u32(b, 0x74) != 0x05F0 + 8 + 20 * 8 || u32(b, 0x78) != 0x0800)
        return fail(110, "Anim/UV/Step-Zeiger (Bank 5 @0x05F0, n1 = 20, Step @0x0800)");
    if (u16(b, 0x2A) != 0x001E || u16(b, 0x32) != 0x7911 || u16(b, 0x42) != 1 || b[0x20] != 1)
        return fail(111, "TPage/CLUT/+0x42/Countdown");
    /* CLUT-Zeile aus Sub>>3: 0x0415 = Bank 4, Sub 0x1D -> +3 Zeilen = 0x7811 + 0xC0. */
    int k = re2fx_spawn(0x041D1800u, 0, re2fx_einheitsmatrix, NULL);
    if (k != 94 || u16(re2fx_platz(k), 0x32) != 0x7811 + 3 * 0x40) return fail(112, "CLUT + (sub>>3)*0x40 (@0x8001ccf4-d10)");
    /* Negativ-Kontrolle: unregistrierte Bank 9 -> -1, nichts belegt. */
    int vor = lebendig();
    if (re2fx_spawn(0x09001000u, 0, re2fx_einheitsmatrix, NULL) != -1 || lebendig() != vor)
        return fail(113, "unregistrierte Bank spawnt");
    /* Pool voll -> 0xFF. */
    for (int n = 0; n < 200; n++) re2fx_spawn(0x04040000u, 0, re2fx_einheitsmatrix, NULL);
    if (lebendig() != 96) return fail(114, "Pool nicht voll");
    if (re2fx_spawn(0x04040000u, 0, re2fx_einheitsmatrix, NULL) != 0xFF) return fail(115, "voller Pool liefert nicht 0xFF");
    re2fx_reset();
    if (lebendig() != 0) return fail(116, "reset laesst Plaetze stehen");
    return 0;
}

/* ============================================================================================ */
static int pruef_saeure(void)
{
    start();
    const int32_t q[3] = { 1000, 20, -2000 };
    /* Negativ-Kontrollen: Art 0 (Explosiv, bleibt RE1.5) und 3 (unbekannt) -> nichts. */
    re2fx_aufschlag(0, q, 1024); re2fx_aufschlag(3, q, 1024);
    if (lebendig() != 0) return fail(201, "Art 0/3 legt Plaetze an");
    re2fx_aufschlag(2, q, 1024);
    int a = finde(2, 0x0C, 95);
    if (a != 95 || u16(re2fx_platz(a), 0x18) != 0xB403 || re2fx_platz(a)[1] != 49 || re2fx_platz(a)[0x1B] != 2)
        return fail(202, "Aufschlag-Platz nicht Bank 2 Skr. 4 mit Status 0xB403 / Op B 49 / Art 2");
    /* Bild X */
    re2fx_tick();
    if (s_se_n != 1 || s_se_code[0] != 0x01130001u) return fail(203, "SE 0x01130001 nicht genau 1x");
    if (s_se_pos[0][0] != q[0] || s_se_pos[0][1] != q[1] || s_se_pos[0][2] != q[2]) return fail(204, "SE-Lage != Q");
    const uint8_t *ab = re2fx_platz(a);
    if (u16(ab, 0x18) != 0x8403 || u16(ab, 0x12) != 1 || ab[0] != 0) return fail(205, "Phase 0 -> Status 0x8403 / Phase 1 / Op A 0");
    if ((int8_t)ab[0x08] != -23 || s16(ab, 0x0E) != 240 || s16(ab, 0x0C) != -23 + 0 * 0) {
        /* nach der Physik dieses Bilds: vel.x = 0 + acc.x = -23 (Physik nach Op B, @0x8001d70c-798) */
        return fail(206, "Spritzer vel/acc (vel.y 240, acc.x -23 @0x80021738-7c)");
    }
    static const uint32_t k0[3] = { 0x030F2000u, 0x040C2000u, 0x041D1800u };
    for (int n = 0; n < 3; n++) {
        const uint8_t *c = re2fx_platz(94 - n);
        if (c[0x1C] != (k0[n] >> 24) || c[0x1E] != ((k0[n] >> 16) & 0xff) || u16(c, 0x3A) != (k0[n] & 0xffff))
            return fail(207, "Kinder Bild X != 0x030F2000/0x040C2000/0x041D1800");
        if (u16(c, 0x18) != 0x4000) return fail(208, "Kind Bild X nicht aufgeschoben (0x4000)");
        if (s16(c, 0x2C) != q[0] || s16(c, 0x2E) != q[1] || s16(c, 0x30) != q[2]) return fail(209, "Kind-Versatz != Q");
    }
    if (lebendig() != 4) return fail(210, "Bild X: nicht 1 + 3 Plaetze");
    /* Bilder X+1..X+4 */
    static const uint32_t kp[4] = { 0x031F2000u, 0x03142000u, 0x040D2800u, 0x030F2000u };
    int16_t basis[9]; re2fx_gl_basis(basis);
    for (int f = 1; f <= 4; f++) {
        re2fx_tick();
        int slot = 94 - 3 - (f - 1);
        const uint8_t *c = re2fx_platz(slot);
        if (c[0x1C] != (kp[f - 1] >> 24) || c[0x1E] != ((kp[f - 1] >> 16) & 0xff) || u16(c, 0x3A) != (kp[f - 1] & 0xffff))
            return fail(210 + f, "Phasen-Kind X+f falsch");
        /* Lage = Q + RotY(1024)*B*lokal, lokal = (-23*(f-1)*f/2... ) -> nach f Bildern Physik:
         * Welt im Bild X+f aus lokal nach (f) Integrationen, gerechnet VOR der Integration dieses Bilds. */
        double lx = -23.0 * (double)((f - 1) * f) / 2.0, ly = 240.0 * f, lz = 0.0;
        double bx = (basis[0] * lx + basis[1] * ly + basis[2] * lz) / 4096.0;
        double by = (basis[3] * lx + basis[4] * ly + basis[5] * lz) / 4096.0;
        double bz = (basis[6] * lx + basis[7] * ly + basis[8] * lz) / 4096.0;
        /* RotY(1024): [[c,0,s],[0,1,0],[-s,0,c]] mit c = 0, s = 1 */
        double wx = q[0] + bz, wy = q[1] + by, wz = q[2] - bx;
        if (fabs(s16(c, 0x2C) - wx) > 2.0 || fabs(s16(c, 0x2E) - wy) > 2.0 || fabs(s16(c, 0x30) - wz) > 2.0) {
            printf("  f=%d Kind (%d,%d,%d) erwartet (%.1f,%.1f,%.1f)\n", f, s16(c, 0x2C), s16(c, 0x2E), s16(c, 0x30), wx, wy, wz);
            return fail(215 + f, "Phasen-Kind-Lage != Q + RotY(gier)*B*lokal (O-VB1)");
        }
    }
    if (u16(re2fx_platz(a), 0x18) != 0) return fail(220, "Aufschlag-Platz in X+4 nicht frei (@0x80021950-58)");
    if (s_se_n != 1) return fail(221, "weitere SEs nach Phase 0");
    if (re2fx_op_unbekannt() != 0) return fail(222, "nicht umgesetzter Op erreicht");
    return 0;
}

/* ============================================================================================ */
static int pruef_brand(void)
{
    start();
    const int32_t q[3] = { -500, 10, 3000 };
    const int16_t gier = 300;
    re2fx_aufschlag(1, q, gier);
    re2fx_tick();                                       /* Bild X */
    if (s_se_n != 1 || s_se_code[0] != 0x01120001u) return fail(301, "SE 0x01120001 nicht genau 1x");
    if (s_se_pos[0][0] != q[0] || s_se_pos[0][1] != q[1] || s_se_pos[0][2] != q[2]) return fail(302, "SE-Lage (+0x60) != Q");
    const uint8_t *ab = re2fx_platz(95);
    if (u16(ab, 0x18) != 0x8000 || u16(ab, 0x12) != 1 || ab[1] != 48) return fail(303, "Phase 0 -> Status 0x8000 / Phase 1 / Op B 48");
    if ((int32_t)u32(ab, 0x64) != q[1] + 1800 + 1800) return fail(304, "Ende: +0x64 := alte Translation + 3600 (@0x80021578-a0)");
    const uint8_t *c1 = re2fx_platz(94), *c2 = re2fx_platz(93);
    if (c1[0x1C] != 4 || c1[0x1E] != 0x0C || u16(c1, 0x3A) != 0x2800) return fail(305, "Kind 0x040C2800");
    if (c2[0x1C] != 4 || c2[0x1E] != 0x1D || u16(c2, 0x3A) != 0x2700) return fail(306, "Kind 0x041D2700");
    if ((int32_t)u32(c1, 0x60) != q[0] || (int32_t)u32(c1, 0x64) != q[1] || (int32_t)u32(c1, 0x68) != q[2])
        return fail(307, "Kind-Matrix-Translation != Q (a2 = Platz+0x4C)");
    /* Die drei Flammen gegen den nachgebauten Strom. */
    for (int n = 0; n < 3; n++) {
        const uint8_t *f = re2fx_platz(92 - n);
        uint32_t r1 = rnd(), r2 = rnd(), r3 = rnd(), r4 = rnd();
        uint32_t skala = 7168u + (r1 % 8u) * 768u;
        int32_t g = (n == 0) ? gier + (int32_t)(r2 % 40u)
                  : (n == 1) ? gier + (int32_t)(r2 % 80u) + 400 : gier + (int32_t)(r2 % 80u) - 400;
        if (f[0x1C] != 5 || f[0x1E] != 5) return fail(310 + n, "Flamme nicht Bank 5 Skr. 5");
        if (u16(f, 0x3A) != skala) { printf("  Flamme %d Skala %u erwartet %u\n", n, u16(f, 0x3A), skala); return fail(313 + n, "Skala != 7168 + (r%8)*768"); }
        if (s16(f, 0x22) != (int16_t)g) return fail(316 + n, "Gier-Streuung");
        if (s16(f, 0x0C) != 96 + (int32_t)(r3 % 25u)) return fail(319 + n, "vel.x != 96 + r%25");
        if ((int8_t)f[0x09] != 5 + (int32_t)(r4 % 8u)) return fail(322 + n, "acc.y != 5 + r%8");
        if (s16(f, 0x4A) != 1) return fail(325 + n, "+0x4A != 1");
        if ((int32_t)u32(f, 0x60) != q[0] || (int32_t)u32(f, 0x68) != q[2]) return fail(328 + n, "Flammen-Matrix-Translation != Q");
    }
    if (re15_re2_rand() != rnd()) return fail(331, "Strom laeuft auseinander (Zahl der Zufallszuege)");
    if (lebendig() != 6) return fail(332, "Bild X: nicht 1 + 2 + 3 Plaetze");
    re2fx_tick();                                       /* X+1: Platz frei, Kinder befoerdert */
    if (u16(re2fx_platz(95), 0x18) != 0) return fail(333, "Op-48-Platz in X+1 nicht frei (@0x800215a4)");
    for (int n = 0; n < 3; n++) {
        const uint8_t *f = re2fx_platz(92 - n);
        if (u16(f, 0x18) != 0xB003 || f[0] != 58 || f[1] != 28 || f[0x1B] != 2)
            return fail(334 + n, "Flamme nach Op 27 nicht 0xB003 / Op A 58 / Op B 28 / Zustand 2");
    }
    /* (Gegenpruefung M6) Op 27 zieht in X+1 je Flamme zwei Zahlen, der Draw-Pass laeuft AUFSTEIGEND
     * (@0x8001d5d0-668: Flammen 90, 91, 92 vor den Kindern 93/94): Anim r%3 (`0x55555556` @0x8001faa4-f4)
     * und Luftzaehler 8 + r%3 (`addiu v0,v0,8 / sh v0,66` @0x8001fbb4-b8). */
    for (int n = 2; n >= 0; n--) {
        const uint8_t *f = re2fx_platz(92 - n);
        uint32_t ra = rnd(), rb = rnd();
        if (f[0x21] != ra % 3u) return fail(337, "Op 27 Anim != r%3");
        if (s16(f, 0x42) != (int32_t)(rb % 3u) + 8) return fail(338, "Op 27 Luftzaehler != 8 + r%3");
    }
    return 0;
}

/* ============================================================================================ */
/* Eine einzelne Flamme in der Luft (Q.y = -300), flacher Boden 0: Landung, Brennen, Schaden-Tor. */
static int pruef_flamme(int app_treffer, int *lebenszeit, int *folge, int *folge_soll, int *app_rufe)
{
    start();
    s_app_ret = app_treffer;
    const int32_t q[3] = { 0, -100, 0 };
    re2fx_aufschlag(1, q, 0);
    re2fx_tick();                                       /* X: Aufschlag */
    int fl = 92;                                        /* erste Flamme */
    int gelandet = -1, zustand1 = -1, tot = -1, c2 = -1, c1 = -1;
    *folge = 0; *folge_soll = 0;
    for (int t = 1; t < 400; t++) {
        const uint8_t *f = re2fx_platz(fl);
        int vorher_opa = f[0];
        int16_t vx_vor = s16(f, 0x0C);
        uint8_t s2_vor = f[0x02];
        int opb_vor = f[1];
        uint8_t z_vor = f[0x1B];
        uint8_t belegt[RE2FX_PLAETZE];
        for (int i = 0; i < RE2FX_PLAETZE; i++) belegt[i] = (u16(re2fx_platz(i), 0x18) != 0);
        re2fx_tick();
        f = re2fx_platz(fl);
        /* neue 0x0504-Kinder DIESER Flamme: Versatz (+0x2C/+0x30) = ihre Weltlage (+0x34/+0x38) in
         * diesem Bild (a3 = Platz+0x34 @0x8001fdd8). */
        for (int i = 0; i < RE2FX_PLAETZE; i++) {
            const uint8_t *b = re2fx_platz(i);
            if (belegt[i] || !u16(b, 0x18) || b[0x1C] != 5 || b[0x1E] != 4) continue;
            if (s16(b, 0x2C) == s16(f, 0x34) && s16(b, 0x30) == s16(f, 0x38)) {
                (*folge)++;
                if (u16(b, 0x3A) != (uint16_t)((u16(f, 0x3A) * 4u) / 5u)) *folge = -1000;   /* Skala x0.8 */
                if (s16(b, 0x4A) != 1) *folge = -1000;
            }
        }
        if (opb_vor == 29 && vx_vor >= 61 && (s2_vor % 15) == 0) (*folge_soll)++;
        if (gelandet < 0 && f[0] == 19) {
            gelandet = t;
            /* (Gegenpruefung M5) Op 46 setzt vel.x 180 (`addiu v0,zero,180 / sh v0,12` @0x80020ba4-ac) und
             * acc.x -10 - r%11 (@0x80020bb0-f4); die Physik DESSELBEN Bilds addiert acc danach (@0x8001d70c-798)
             * -> nach dem Landebild vel.x = 180 + acc.x exakt. */
            if (f[1] != 29 || s16(f, 0x0C) != 180 + (int8_t)f[0x08] || s16(f, 0x0E) != 0)
                return fail(401, "Landung: Op B 29 / vel.x != 180 + acc.x / vel.y 0 (@0x80020b60)");
            if ((int8_t)f[0x08] > -10 || (int8_t)f[0x08] < -20) return fail(408, "Landung: acc.x nicht -10 - r%11");
            c2 = s16(f, 0x42);
            if (c2 < 38 || c2 > 45) return fail(402, "Landezaehler != 38 + r%8");
        }
        if (gelandet > 0 && zustand1 < 0 && z_vor == 2 && f[0x1B] == 1) {
            zustand1 = t;
            c1 = s16(f, 0x42);
            if (c1 < 90 || c1 > 100) return fail(403, "Brennzaehler != 90 + r%11");
        }
        if (u16(f, 0x18) == 0) { tot = t; break; }
        (void)vorher_opa;
    }
    if (gelandet < 0) return fail(404, "Flamme landet nicht (Op 28 -> Op 46)");
    if (zustand1 < 0 || tot < 0) return fail(405, "Flamme brennt nicht aus");
    /* Zeitplan (Op 19 @0x8001f388-51c): Zustand 2 zaehlt c2 herunter + 1 Wechselbild, Zustand 1
     * zaehlt c1 herunter + 1 Bild auf Zustand 0, dann 1 Bild bis Status 0. */
    if (zustand1 - gelandet != c2 + 1) return fail(406, "Zustand-2-Dauer != Zaehler + 1");
    if (tot - zustand1 != c1 + 2) return fail(407, "Zustand-1-Dauer != Zaehler + 2");
    *lebenszeit = tot - gelandet;
    *app_rufe = s_app_n;
    return 0;
}

static int pruef_bodenfeuer(void)
{
    int leben = 0, folge = 0, folge_soll = 0, rufe = 0;
    int rc = pruef_flamme(0, &leben, &folge, &folge_soll, &rufe);
    if (rc) return rc;
    if (leben < 38 + 90 + 3 || leben > 45 + 100 + 3) return fail(410, "Lebensdauer ausserhalb (38..45)+(90..100)");
    if (rufe <= 0) return fail(411, "Applier nie gerufen");
    if (s_app_bad) return fail(412, "Applier-Ruf trotz step[0x16] < 16 oder X <= 0x1000 bzw. falsche Box");
    if (s_app_hit != 0x2002000Au) return fail(413, "Hitcode != 0x2002000A");
    if (folge != folge_soll || folge < 1) {
        printf("  Folgeflammen %d, erwartet %d\n", folge, folge_soll);
        return fail(414, "Folgeflammen != Bilder mit step[2] %% 15 == 0 und vel.x >= 61");
    }
    printf("  Flamme: Lebensdauer %d Bilder, Applier-Rufe %d, Folgeflammen %d\n", leben, rufe, folge);
    /* Treffer -> Op 50: Gleiten aus (vel.x 0, step[2] 64, step[3] 0). */
    start();
    s_app_ret = 1;
    const int32_t q[3] = { 0, -100, 0 };
    re2fx_aufschlag(1, q, 0);
    for (int t = 0; t < 40; t++) re2fx_tick();
    const uint8_t *f = re2fx_platz(92);
    if (s_app_n < 1) return fail(420, "Treffer-Lauf: Applier nie gerufen");
    if (s16(f, 0x0C) != 0 || f[0x08] != 0 || f[0x03] != 0) return fail(421, "Treffer -> Op 50 (vel.x/acc.x/step[3] := 0 @0x80021978-9c)");
    if (f[0] != 19) return fail(422, "Treffer: Op A 19 laeuft nicht weiter");
    /* Negativ-Kontrolle: Flamme OHNE +0x4A (direkt gespawnt) ruft den Applier nie. */
    start();
    int i = re2fx_spawn(0x05051C00u, 0, re2fx_einheitsmatrix, NULL);
    (void)i;
    for (int t = 0; t < 200; t++) re2fx_tick();
    if (s_app_n != 0) return fail(423, "Flamme ohne +0x4A ruft den Applier");
    /* Wand beim GLEITEN (Kontakt nur fuer Op 29): Op[step[3]] = Op 50 -> vel.x 0, brennt weiter (Op A 19). */
    start();
    s_wand = 2;
    re2fx_aufschlag(1, q, 0);
    int gl = -1;
    for (int t = 0; t < 30; t++) {
        re2fx_tick();
        f = re2fx_platz(92);
        if (gl < 0 && f[0] == 19) gl = t;
    }
    f = re2fx_platz(92);
    if (gl < 0) return fail(424, "Wand-Gleiten: Flamme landet nicht");
    if (s16(f, 0x0C) != 0 || f[0] != 19 || f[0x03] != 0) return fail(425, "Wand beim Gleiten stoppt nicht ueber Op 50 (@0x8001fe84-b4)");
    /* Wand in der LUFT (Kontakt immer, Boden gleich +0x14): Op 50 in der Luft setzt step[2] := 64, die
     * Landung dispatcht dann Op 64 (@0x8001fcac-c8 -> 0x80022728): Op B := 0, kein Brennen (Op A bleibt 58). */
    start();
    s_wand = 1;
    re2fx_aufschlag(1, q, 0);
    int op19 = 0;
    for (int t = 0; t < 60; t++) { re2fx_tick(); if (re2fx_platz(92)[0] == 19) op19 = 1; }
    if (re2fx_op_zaehler(64) < 1) return fail(426, "Luft-Wand: Op 64 nie erreicht");
    if (op19) return fail(427, "Luft-Wand: Flamme brennt trotzdem (Op A 19)");
    if (s_app_n != 0) return fail(428, "Luft-Wand: Applier gerufen");
    if (re2fx_op_unbekannt() != 0) return fail(429, "nicht umgesetzter Op erreicht");
    /* Negativ-Kontrolle zu 426: ohne Wand kein Op 64. */
    start();
    re2fx_aufschlag(1, q, 0);
    for (int t = 0; t < 60; t++) re2fx_tick();
    if (re2fx_op_zaehler(64) != 0) return fail(430, "ohne Wand Op 64 erreicht");
    return 0;
}


/* ============================================================================================ */
/* 6xx Folgeflammen-Takt: gelandete Flamme mit vel.x 200 / acc.x 0 legt genau alle 15 Bilder eine
 * Folgeflamme (step[2] % 15 == 0, @0x8001fd94-b8), mit vel.x 60 (< 61, @0x8001fd78) keine. */
static int folge_zaehlen(int vx, int bilder)
{
    start();
    const int32_t q[3] = { 0, -100, 0 };
    re2fx_aufschlag(1, q, 0);
    int t;
    for (t = 0; t < 40 && re2fx_platz(92)[0] != 19; t++) re2fx_tick();
    if (re2fx_platz(92)[0] != 19) return -1;
    uint8_t *w = re2fx_platz_sonde(92);
    w[0x0C] = (uint8_t)vx; w[0x0D] = (uint8_t)(vx >> 8); w[0x08] = 0; w[0x02] = 0;
    int n = 0;
    for (t = 0; t < bilder; t++) {
        uint8_t belegt[RE2FX_PLAETZE];
        for (int i = 0; i < RE2FX_PLAETZE; i++) belegt[i] = (u16(re2fx_platz(i), 0x18) != 0);
        re2fx_tick();
        const uint8_t *f = re2fx_platz(92);
        for (int i = 0; i < RE2FX_PLAETZE; i++) {
            const uint8_t *b = re2fx_platz(i);
            if (belegt[i] || !u16(b, 0x18) || b[0x1C] != 5 || b[0x1E] != 4) continue;
            if (s16(b, 0x2C) == s16(f, 0x34) && s16(b, 0x30) == s16(f, 0x38)) n++;
        }
    }
    return n;
}
static int pruef_folgetakt(void)
{
    int n = folge_zaehlen(200, 60);
    if (n != 4) { printf("  Folgeflammen bei vel.x 200 in 60 Bildern: %d\n", n); return fail(601, "Folgeflammen-Takt != alle 15 Bilder"); }
    n = folge_zaehlen(60, 60);
    if (n != 0) return fail(602, "Negativ-Kontrolle: vel.x 60 legt Folgeflammen");
    return 0;
}

/* 7xx Aspekt-Folgen der Flamme (Op 58 in der Luft, Op 19 am Boden) je Bild exakt:
 *   Op 58 Zustand 2 x1010/x1007 (@0x80022350-84), Zustand 1 x880/x800 (@0x800222c8-328),
 *   Op 19 Zustand 2 x1009/x1002 (@0x8001f40c-44), Zustand 1 x990/x980 (@0x8001f3a4-e4). */
static int geprueft[4];
static int aspekt_lauf(int32_t hoehe);
static int pruef_aspekte(void)
{
    memset(geprueft, 0, sizeof geprueft);
    int rc = aspekt_lauf(-100);                         /* landet: Op 58 Zustand 2, dann Op 19 */
    if (!rc) rc = aspekt_lauf(-30000);                  /* bleibt in der Luft: Op 58 Zustand 2 und 1 */
    if (rc) return rc;
    if (!geprueft[0] || !geprueft[1] || !geprueft[2] || !geprueft[3]) return fail(705, "nicht alle Aspekt-Zweige durchlaufen");
    return 0;
}
static int aspekt_lauf(int32_t hoehe)
{
    start();
    const int32_t q[3] = { 0, hoehe, 0 };
    re2fx_aufschlag(1, q, 0);
    re2fx_tick(); re2fx_tick();                         /* X, X+1 (Op 27) */
    for (int t = 0; t < 300; t++) {
        const uint8_t *f = re2fx_platz(92);
        if (!u16(f, 0x18)) break;
        unsigned opa = f[0], z = f[0x1B];
        int ctr = s16(f, 0x42);
        uint32_t x = u16(f, 0x04), y = u16(f, 0x06);
        re2fx_tick();
        f = re2fx_platz(92);
        if (!u16(f, 0x18) || ctr == 0) continue;
        uint32_t kx = 0, ky = 0; int k = -1;
        if (opa == 58 && z == 2) { kx = 1010; ky = 1007; k = 0; }
        if (opa == 58 && z == 1) { kx = 880;  ky = 800;  k = 1; }
        if (opa == 19 && z == 2) { kx = 1009; ky = 1002; k = 2; }
        if (opa == 19 && z == 1) { kx = 990;  ky = 980;  k = 3; }
        if (k < 0) continue;
        if (u16(f, 0x04) != (uint16_t)(x * kx / 1000u) || u16(f, 0x06) != (uint16_t)(y * ky / 1000u)) {
            printf("  t=%d Op %u Zustand %u: (%u,%u) -> (%u,%u), erwartet (%u,%u)\n", t, opa, z, x, y,
                   u16(f, 0x04), u16(f, 0x06), x * kx / 1000u, y * ky / 1000u);
            return fail(701 + k, "Aspekt-Folge falsch");
        }
        geprueft[k]++;
    }
    return 0;
}

/* 44x Landung im Luft-SCHRUMPFEN (Zustand 1): Op 46 setzt +0x1B nicht (@0x80020b60-c30), Op 19 zaehlt
 * dann nur den Landezaehler 38 + r%8 im Schrumpfzweig herunter und stirbt (Saeure-GP §11). */
static int pruef_landung_zustand1(void)
{
    start();
    const int32_t q[3] = { 0, -100, 0 };
    re2fx_aufschlag(1, q, 0);
    re2fx_tick(); re2fx_tick();                         /* X, X+1: Flamme 92 nach Op 27 in der Luft */
    uint8_t *w = re2fx_platz_sonde(92);
    if (w[0] != 58) return fail(440, "Flamme nach Op 27 nicht in Op 58");
    w[0x1B] = 1; w[0x42] = 50; w[0x43] = 0;            /* Luft-Zustand 1 mit langem Zaehler erzwingen */
    int gel = -1, c = -1, tot = -1, zustand2 = 0;
    for (int t = 0; t < 200; t++) {
        re2fx_tick();
        const uint8_t *f = re2fx_platz(92);
        if (gel < 0 && f[0] == 19) { gel = t; c = s16(f, 0x42); if (f[0x1B] != 1) return fail(441, "Op 46 hat +0x1B gesetzt"); }
        if (gel >= 0 && f[0x1B] == 2) zustand2 = 1;
        if (u16(f, 0x18) == 0) { tot = t; break; }
    }
    if (gel < 0 || tot < 0) return fail(442, "Flamme landet/stirbt nicht");
    if (c < 38 || c > 45) return fail(443, "Landezaehler != 38 + r%8");
    if (zustand2) return fail(444, "nach Landung im Zustand 1 wieder Zustand 2");
    if (tot - gel != c + 2) return fail(445, "Lebensdauer nach Landung im Zustand 1 != Zaehler + 2");
    return 0;
}

/* 45x Pause-Gate: 0x10000000 (RE15_PAUSE_ACTION == RE2 0x800CFBDC-Bit @0x8001d318-2c) haelt die
 * Aufschlag-Baenke 2/3/4/5 an (Bank-Liste 26/28/40/21/22/1 @0x8001d354-80); danach geht es weiter. */
extern uint32_t g_re15_pauseflags;
static int pruef_pause(void)
{
    start();
    const int32_t q[3] = { 0, 0, 0 };
    re2fx_aufschlag(2, q, 0);
    g_re15_pauseflags |= 0x10000000u;
    for (int t = 0; t < 5; t++) re2fx_tick();
    int angehalten = (s_se_n == 0 && u16(re2fx_platz(95), 0x12) == 0 && lebendig() == 1);
    g_re15_pauseflags &= ~0x10000000u;
    if (!angehalten) return fail(450, "Pause haelt die RE2-FX nicht an");
    re2fx_tick();
    if (s_se_n != 1 || lebendig() != 4) return fail(451, "nach der Pause laeuft Phase 0 nicht");
    return 0;
}

/* ============================================================================================ */
/* 8xx Weltlage-NORMALZWEIG FUN_8001d894 ohne Bit 0x400 (Gegenpruefung M4): +0x34 := RotY(+0x22)*lokal
 * (`lh a0,34(a2)` / `jal 0x8008e8b4` @0x8001dac8-cc, `sh` @0x8001db50-70), dann += M.t + M.rot*Versatz
 * (`lhu +0x34 / addu +0x60 / addu MAC / sh` @0x8001dbd0-dc1c). RotMatrixY FUN_8008e8b4 auf die Einheit
 * (a >= 0: t1 = -sin `subu t1,zero,t7` @0x8008e910; m0j' = (c*m0j - t1*m2j)>>12, m2j' = (t1*m0j + c*m2j)>>12
 * @0x8008e918-a40) = [[c,0,s],[0,1,0],[-s,0,c]]; die Viertel sind exakt (sin/cos 0 oder 4096):
 *   Gier 1024 -> (l.z, l.y, -l.x), 2048 -> (-l.x, l.y, -l.z), 3072 -> (-l.z, l.y, l.x).
 * Die Erwartung kommt NICHT aus re2_fx.c: Viertel-Drehung von Hand, lokal/M.t aus dem Abbild. */
static void viertel(int16_t gier, const int32_t l[3], int32_t e[3])
{
    switch (gier) {
    case 1024: e[0] =  l[2]; e[1] = l[1]; e[2] = -l[0]; break;
    case 2048: e[0] = -l[0]; e[1] = l[1]; e[2] = -l[2]; break;
    case 3072: e[0] = -l[2]; e[1] = l[1]; e[2] =  l[0]; break;
    default:   e[0] =  l[0]; e[1] = l[1]; e[2] =  l[2]; break;   /* 0 */
    }
}
/* Eine Bodenflamme (Platz 92) mit genau ziel_gier gleiten lassen: Aufschlag-Gier so waehlen, dass
 * Gier + r%40 (Flamme 0, zweiter Zug nach dem Skala-Zug @0x80021104-164) das Ziel trifft. Jedes Bild ab
 * X+1: Weltlage == M.t + Viertel(lokal VOR dem Bild) (Weltlage vor Op B und Physik, @0x8001d6c8). */
static int gleit_lage(int16_t ziel_gier, int nr, int32_t *weg_soll)
{
    start();
    (void)rnd(); uint32_t r2 = rnd();
    const int16_t gier_a = (int16_t)(ziel_gier - (int16_t)(r2 % 40u));
    const int32_t q[3] = { 700, 10, -1300 };
    re2fx_aufschlag(1, q, gier_a);
    re2fx_tick();                                       /* X: Flamme 92 gespawnt (0x4000) */
    const uint8_t *f = re2fx_platz(92);
    if (s16(f, 0x22) != ziel_gier) return fail(nr, "Flammen-Gier != Ziel (Strom-Nachbau)");
    int bilder = 0, gleit = 0;
    int32_t l0x = 0;
    for (int t = 1; t < 40 && u16(re2fx_platz(92), 0x18); t++) {
        f = re2fx_platz(92);
        const int32_t l[3] = { s16(f, 0x24), s16(f, 0x26), s16(f, 0x28) };
        const int16_t vx = s16(f, 0x0C);
        const int war_19 = (f[0] == 19);
        re2fx_tick();
        f = re2fx_platz(92);
        const int32_t t3[3] = { (int32_t)u32(f, 0x60), (int32_t)u32(f, 0x64), (int32_t)u32(f, 0x68) };
        if (t3[0] != q[0] || t3[1] != q[1] || t3[2] != q[2]) return fail(nr + 1, "Flammen-M.t != Q (Op 48 a2 = Platz+0x4C)");
        int32_t e[3]; viertel(ziel_gier, l, e);
        if (s16(f, 0x34) != (int16_t)(t3[0] + e[0]) || s16(f, 0x36) != (int16_t)(t3[1] + e[1]) ||
            s16(f, 0x38) != (int16_t)(t3[2] + e[2])) {
            printf("  Gier %d Bild %d: Welt (%d,%d,%d), erwartet (%d,%d,%d) aus lokal (%d,%d,%d)\n", ziel_gier, t,
                   s16(f, 0x34), s16(f, 0x36), s16(f, 0x38), t3[0] + e[0], t3[1] + e[1], t3[2] + e[2], l[0], l[1], l[2]);
            return fail(nr + 2, "Weltlage != M.t + RotY(Gier)*lokal (Normalzweig @0x8001dac8-dc1c)");
        }
        bilder++;
        if (war_19) {                                   /* gleitet: lokal.x waechst um vel.x (Physik @0x8001d720-794) */
            if (!gleit) l0x = l[0];
            gleit++;
            if (vx > 0) *weg_soll += vx;
            if (s16(f, 0x24) != (int16_t)(l0x + *weg_soll)) return fail(nr + 3, "lokal.x != Start + Summe vel.x");
        }
    }
    if (bilder < 20 || gleit < 8) return fail(nr + 4, "zu wenige Gleitbilder geprueft");
    return 0;
}
static int pruef_weltlage(void)
{
    static const int16_t g[3] = { 1024, 2048, 3072 };
    for (int k = 0; k < 3; k++) {
        int32_t weg = 0;
        int rc = gleit_lage(g[k], 801 + 10 * k, &weg);
        if (rc) return rc;
        /* Gleitrichtung ausdruecklich: Gier 1024 -> Welt -z, 2048 -> -x, 3072 -> +z (RotMatrixY-Konvention). */
        const uint8_t *f = re2fx_platz(92);
        int32_t dx = s16(f, 0x34) - 700, dz = s16(f, 0x38) - (-1300);
        if (weg < 500) return fail(831, "Gleitweg < 500");
        if (g[k] == 1024 && !(dz < -500 && dx > -60 && dx < 60)) return fail(832, "Gier 1024 gleitet nicht nach -z");
        if (g[k] == 2048 && !(dx < -500 && dz > -60 && dz < 60)) return fail(833, "Gier 2048 gleitet nicht nach -x");
        if (g[k] == 3072 && !(dz >  500 && dx > -60 && dx < 60)) return fail(834, "Gier 3072 gleitet nicht nach +z");
        printf("  Gier %d: Gleitweg %d, Lage-Versatz (%d,%d)\n", g[k], weg, dx, dz);
    }
    /* M.rot*Versatz mit NICHT-Einheitsmatrix (kein Aufschlag-Kind nutzt das; der Normalzweig rechnet es
     * trotzdem @0x8001db78-dc1c): Flamme direkt gespawnt, M.rot = [[0,0,4096],[0,4096,0],[-4096,0,0]],
     * M.t = (1000,20,-2000), Versatz (100,5,300) -> M.rot*Versatz = (300,5,-100), Gier 1024. */
    start();
    uint8_t m[32]; memset(m, 0, sizeof m);
    static const int16_t rot[9] = { 0, 0, 4096, 0, 4096, 0, -4096, 0, 0 };
    for (int k = 0; k < 9; k++) { m[2 * k] = (uint8_t)rot[k]; m[2 * k + 1] = (uint8_t)((uint16_t)rot[k] >> 8); }
    static const int32_t mt[3] = { 1000, 20, -2000 };
    for (int k = 0; k < 3; k++) for (int j = 0; j < 4; j++) m[20 + 4 * k + j] = (uint8_t)((uint32_t)mt[k] >> (8 * j));
    static const int16_t ofs[4] = { 100, 5, 300, 0 };
    int i = re2fx_spawn(0x05051C00u, 1024, m, ofs);
    if (i != 95) return fail(840, "Spawn nicht auf Platz 95");
    int bilder = 0;
    for (int t = 0; t < 30 && u16(re2fx_platz(i), 0x18); t++) {
        const uint8_t *f = re2fx_platz(i);
        const int32_t l[3] = { s16(f, 0x24), s16(f, 0x26), s16(f, 0x28) };
        re2fx_tick();
        f = re2fx_platz(i);
        int32_t e[3]; viertel(1024, l, e);
        const int32_t soll[3] = { 1000 + e[0] + 300, 20 + e[1] + 5, -2000 + e[2] - 100 };
        if (s16(f, 0x34) != soll[0] || s16(f, 0x36) != soll[1] || s16(f, 0x38) != soll[2]) {
            printf("  Versatz Bild %d: Welt (%d,%d,%d), erwartet (%d,%d,%d)\n", t, s16(f, 0x34), s16(f, 0x36), s16(f, 0x38), soll[0], soll[1], soll[2]);
            return fail(841, "Weltlage != M.t + RotY*lokal + M.rot*Versatz");
        }
        bilder++;
    }
    if (bilder < 20) return fail(842, "Versatz-Lauf zu kurz");
    /* Negativ-Kontrolle: dieselbe Flamme mit Versatz 0 liegt um genau (300,5,-100) anders. */
    start();
    static const int16_t null4[4] = { 0, 0, 0, 0 };
    i = re2fx_spawn(0x05051C00u, 1024, m, null4);
    re2fx_tick();
    if (s16(re2fx_platz(i), 0x34) != 1000 || s16(re2fx_platz(i), 0x36) != 20 || s16(re2fx_platz(i), 0x38) != -2000)
        return fail(843, "Negativ-Kontrolle: ohne Versatz nicht M.t");
    return 0;
}

int main(void)
{
    if (laden() != 0) return fail(1, "CORE00.ESP fehlt");
    int rc;
    if ((rc = pruef_registrierung())) return rc;
    if ((rc = pruef_saeure())) return rc;
    if ((rc = pruef_brand())) return rc;
    if ((rc = pruef_bodenfeuer())) return rc;
    if ((rc = pruef_folgetakt())) return rc;
    if ((rc = pruef_aspekte())) return rc;
    if ((rc = pruef_landung_zustand1())) return rc;
    if ((rc = pruef_pause())) return rc;
    if ((rc = pruef_weltlage())) return rc;
    printf("probe_r34_re2fx: alle Pruefungen gruen\n");
    return 0;
}
