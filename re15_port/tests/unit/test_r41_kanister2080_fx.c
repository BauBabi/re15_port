/*
 * test_r41_kanister2080_fx.c - Runde 41 Spur G, Nachbesserung 3 (Abnahme G 2 M3 / Nachtrag 28 "Explosionseffekt des Gaskanisters"):
 * die Ops der RE2-Kanister-Explosion (engine/src/re2_fx.c, Block "RUNDE 41 SPUR G") gegen UNABHAENGIGE Erwartungen aus der
 * selbst gelesenen RE2-Disassembly (info/re2leon/PSX.EXE, Op-Tafel 0x8009D868) und den Datei-Bytes der Raum-ESP von ROOM40A0
 * (shared_assets/RE2/GATOR40A0.ESP = RDT[0x59CC:0x65E8]). Dossier analysis/befunde_runde41/G_kanister2080.md 10.1.
 *
 *   register  GATOR40A0.ESP hinter CORE00.ESP registrieren (Ids 16 19 08 09 26 1A 18); Kern-Banken bleiben
 *   op69      Bank 0x26 Sub 3 (Fontaene Tick 1 @0x80103f4c-6c): Sender 90 Bilder (+3 = 90 @0x80022f20-24), je Bild 3 Kinder Sub
 *             0..2 (rnd % 3 + Sub & 0xf8), ab +3 < 65 Skala * 955 / 1000 (@0x80023170-b0), im 91. Aufruf Platz frei
 *   op41      Bank 0x08 Sub 2: Op 1, dann Op 41 = Bank 9 Sub 8/9/10 Skala 0x800/0x1000/0xc00, Op A 42, Op 42 = Sub 11/12 Skala
 *             0xc00/0xa00, Op A 0 (@0x800207e4-0x80020944)
 *   op4_6     Kern-Bank 0 Sub 3 (9 Teile): Op 4 wartet +3 Bilder, dann Op 1; Teil 0 Op 6 Lebensdauer +2 = 20, dann Platz frei
 *   op37_78   Bank 9 Sub 8 faellt auf den Boden (Op 16 -> Op 5 -> Op 37/38); Bank 0x1A Sub 1 Op 78 = Op 2 + v/a * Skala / 3000
 * Alle Teile: keine unbekannten Ops (re2fx_op_unbekannt == 0).
 */
#include "re2_fx.h"
#include "re15_ai_flavor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

static int fails;
#define PRUEFE(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint16_t u16(const uint8_t *b, int o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); }
static int16_t  s16(const uint8_t *b, int o) { return (int16_t)u16(b, o); }

static uint8_t *datei(const char *name, size_t *n)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/../RE2/%s", RE15_XSTR(RE15_ASSETS_PATH), name);
    FILE *f = fopen(p, "rb");
    if (!f) { printf("kann %s nicht oeffnen\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long l = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)l);
    if (b && fread(b, 1, (size_t)l, f) != (size_t)l) { free(b); b = NULL; }
    fclose(f);
    if (b && n) *n = (size_t)l;
    return b;
}

static uint8_t *s_core, *s_raum;
static size_t   s_core_n, s_raum_n;
static int registrieren(void)
{
    if (!s_core) s_core = datei("CORE00.ESP", &s_core_n);
    if (!s_raum) s_raum = datei("GATOR40A0.ESP", &s_raum_n);
    if (!s_core || !s_raum) return -1;
    if (re2fx_register_core(s_core, s_core_n) != 0) return -2;
    return re2fx_register_raum(s_raum, s_raum_n);
}
static int32_t boden_flach(const int32_t p[3], int r, uint32_t mask, int a3, int *kontakt)
{ (void)r; (void)mask; (void)a3; *kontakt = p[1] > 0; return 0; }
static int32_t kein_wasser(int32_t x, int32_t z) { (void)x; (void)z; return 0; }

static int lebende(unsigned bank, int sub)
{
    int n = 0;
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        const uint8_t *b = re2fx_platz(i);
        if (u16(b, 0x18) != 0 && b[0x1C] == bank && (sub < 0 || b[0x1E] == sub)) n++;
    }
    return n;
}
static void anfang(void)
{
    re2fx_reset();
    re2fx_boden_hook = boden_flach;
    re2fx_wasser_hook = kein_wasser;
}

static int teil_register(void)
{
    const int rc = registrieren();
    PRUEFE(rc == 0, "Registrierung Kern + GATOR40A0.ESP rc %d", rc);
    PRUEFE(s_raum && s_raum_n == 3100 && !memcmp(s_raum, "\x16\x19\x08\x09\x26\x1a\x18\xff", 8),
           "GATOR40A0.ESP nicht die Raum-ESP von ROOM40A0 (3100 B, Ids 16 19 08 09 26 1A 18 = RDT @0x59CC)");
    const unsigned raum[7] = { 0x16, 0x19, 0x08, 0x09, 0x26, 0x1A, 0x18 }, kern[8] = { 3, 5, 0, 1, 2, 6, 7, 4 };
    for (int i = 0; i < 7; i++) PRUEFE(re2fx_bank_registriert(raum[i]), "Raumbank 0x%02X fehlt", raum[i]);
    for (int i = 0; i < 8; i++) PRUEFE(re2fx_bank_registriert(kern[i]), "Kernbank %u verloren", kern[i]);
    PRUEFE(!re2fx_bank_registriert(0x10), "Glas-Bank 0x10 noch registriert");
    return fails != 0;
}

static int teil_op69(void)
{
    if (registrieren() != 0) { PRUEFE(0, "Registrierung"); return 1; }
    anfang();
    const int16_t ofs[4] = { 300, 0, 200, 0 };
    const int i = re2fx_spawn_sofort(0x26032000u, 0, re2fx_einheitsmatrix, ofs);
    PRUEFE(i >= 0 && i < RE2FX_PLAETZE, "Spawn 0x26032000");
    if (i < 0 || i >= RE2FX_PLAETZE) return 1;
    const uint8_t *b = re2fx_platz(i);
    PRUEFE(b[0] == 69 && b[1] == 0 && b[0x1C] == 0x26 && b[0x1E] == 3 && u16(b, 0x3A) == 0x2000,
           "Schritt 0 Bank 0x26 Sub 3 (ROOM40A0-ESP Skript @0x808: Op A 69): Op %d/%d Bank 0x%02X Sub %d Skala 0x%X", b[0], b[1],
           b[0x1C], b[0x1E], u16(b, 0x3A));
    /* Erwartung (selbst disassembliert, FUN_80022ebc): Aufruf 1 = Phase 0 -> Status 0x8002, +2 1, +3 90, dann wie Phase 1;
     * Phase 1: je Aufruf 3 Spawns, vor dem -1 (+3 vorher = 91 - k im k-ten Aufruf) bei +3 < 65 Skala * 955 / 1000; der Aufruf mit
     * +3 == 0 (der 91.) gibt den Platz frei. */
    uint32_t skala = 0x2000; int frei_bei = -1, kinder_max = 0, sub_ok = 1;
    for (int k = 1; k <= 95 && frei_bei < 0; k++) {
        re2fx_tick();
        const unsigned aufrufe = re2fx_op_zaehler(69);
        if (u16(b, 0x18) == 0) { frei_bei = (int)aufrufe; break; }
        if (k == 1) PRUEFE(u16(b, 0x18) == 0x8002 && b[2] == 1 && b[3] == 89, "Aufruf 1: Status 0x%X +2 %d +3 %d (0x8002/1/89)",
                           u16(b, 0x18), b[2], b[3]);
        if (91 - k < 65) skala = skala * 955u / 1000u;
        PRUEFE(u16(b, 0x3A) == (uint16_t)skala, "Aufruf %d: Skala %u statt %u", k, u16(b, 0x3A), skala);
        PRUEFE(b[3] == (uint8_t)(90 - k), "Aufruf %d: +3 %d statt %d", k, b[3], 90 - k);
        int n = lebende(0x26, 0) + lebende(0x26, 1) + lebende(0x26, 2);
        if (n > kinder_max) kinder_max = n;
        if (lebende(0x26, -1) != n + 1) sub_ok = 0;
    }
    printf("  op69: Sender frei im %d. Aufruf, hoechstens %d Kinder (Sub 0..2) gleichzeitig, Endskala %u\n", frei_bei, kinder_max, skala);
    PRUEFE(frei_bei == 91, "Sender nicht im 91. Aufruf frei (%d)", frei_bei);
    PRUEFE(kinder_max >= 3, "keine Kinder (%d)", kinder_max);
    PRUEFE(sub_ok, "Kind mit Sub ausserhalb 0..2 (rnd %% 3 + (Sub & 0xf8) = 0..2)");
    PRUEFE(re2fx_op_zaehler(11) > 0, "Kinder ohne Op 11 (Skript @0x760/@0x798/@0x7D0)");
    PRUEFE(re2fx_op_unbekannt() == 0, "unbekannte Ops: %u", re2fx_op_unbekannt());
    return fails != 0;
}

static int teil_op41(void)
{
    if (registrieren() != 0) { PRUEFE(0, "Registrierung"); return 1; }
    anfang();
    const int16_t ofs[4] = { 300, -2000, 200, 0 };
    const int i = re2fx_spawn_sofort(0x08024000u, 0, re2fx_einheitsmatrix, ofs);
    PRUEFE(i >= 0 && i < RE2FX_PLAETZE, "Spawn 0x08024000");
    if (i < 0 || i >= RE2FX_PLAETZE) return 1;
    const uint8_t *b = re2fx_platz(i);
    int gesehen[5] = { 0 }, skala_ok = 1, op_a42 = 0, op_a0 = 0;
    const uint16_t k_skala[5] = { 0x0800, 0x1000, 0x0c00, 0x0c00, 0x0a00 };   /* @0x800207f8 / @0x80020830 / @0x8002086c / @0x800208cc / @0x80020908 */
    for (int k = 1; k <= 6; k++) {
        const unsigned v41 = re2fx_op_zaehler(41), v42 = re2fx_op_zaehler(42);
        re2fx_tick();
        if (re2fx_op_zaehler(41) > v41) op_a42 = (b[0] == 42);
        if (re2fx_op_zaehler(42) > v42) op_a0 = (b[0] == 0);
        for (int p = 0; p < RE2FX_PLAETZE; p++) {
            const uint8_t *c = re2fx_platz(p);
            if (u16(c, 0x18) == 0 || c[0x1C] != 0x09) continue;
            const int s = c[0x1E];
            if (s < 8 || s > 12) { skala_ok = 0; continue; }
            gesehen[s - 8]++;
            if (u16(c, 0x3A) != k_skala[s - 8]) skala_ok = 0;
        }
    }
    printf("  op41: Bank 9 Sub 8..12 gesehen %d %d %d %d %d (Bild-Zaehlungen), Op 41 %u, Op 42 %u\n", gesehen[0], gesehen[1], gesehen[2],
           gesehen[3], gesehen[4], re2fx_op_zaehler(41), re2fx_op_zaehler(42));
    for (int s = 0; s < 5; s++) PRUEFE(gesehen[s] > 0, "Bank 9 Sub %d nie gespawnt", 8 + s);
    PRUEFE(skala_ok, "Bank-9-Kind mit falscher Sub oder Skala (Sub 8..12 = 0x800/0x1000/0xc00/0xc00/0xa00)");
    PRUEFE(re2fx_op_zaehler(41) == 1 && re2fx_op_zaehler(42) == 1, "Op 41/42 nicht je genau einmal (%u/%u)", re2fx_op_zaehler(41),
           re2fx_op_zaehler(42));
    PRUEFE(op_a42, "nach Op 41 Op A nicht 42 (`addiu v0,zero,42 / sb v0,0` @0x800208a4-ac)");
    PRUEFE(op_a0, "nach Op 42 Op A nicht 0 (`sb zero,0` @0x80020944)");
    PRUEFE(re2fx_op_unbekannt() == 0, "unbekannte Ops: %u", re2fx_op_unbekannt());
    return fails != 0;
}

static int teil_op4_6(void)
{
    if (registrieren() != 0) { PRUEFE(0, "Registrierung"); return 1; }
    anfang();
    const int16_t ofs[4] = { 300, -2000, 200, 0 };
    const int i = re2fx_spawn_sofort(0x00034000u, 0, re2fx_einheitsmatrix, ofs);
    PRUEFE(i >= 0 && i < RE2FX_PLAETZE, "Spawn 0x00034000");
    if (i < 0 || i >= RE2FX_PLAETZE) return 1;
    PRUEFE(lebende(0x00, 3) == 9, "Kern-Bank 0 Sub 3 nicht 9 Teile (Skript @0xDEC): %d", lebende(0x00, 3));
    /* Teil 0 (Haupt-Platz i): Schritt 0 Op 1 -> Schritt 1 Op 4 mit +3 = 14 -> nach 14 Wartebildern Op 1 -> Schritt 2 Op 6 mit +2 = 20
     * -> nach 20 Bildern Platz frei. */
    int frei = -1;
    for (int k = 1; k <= 60; k++) {
        re2fx_tick();
        if (u16(re2fx_platz(i), 0x18) == 0) { frei = k; break; }
    }
    printf("  op4_6: Teil 0 frei nach %d Bildern, Op 4 %u Aufrufe, Op 6 %u Aufrufe\n", frei, re2fx_op_zaehler(4), re2fx_op_zaehler(6));
    PRUEFE(re2fx_op_zaehler(4) >= 9 + 14, "Op 4 zu selten (%u)", re2fx_op_zaehler(4));
    PRUEFE(re2fx_op_zaehler(6) >= 21, "Op 6 zu selten (%u)", re2fx_op_zaehler(6));
    PRUEFE(frei > 0 && frei <= 1 + 15 + 21 + 1, "Teil 0 nicht nach Warten 14 + Lebensdauer 20 frei (%d)", frei);
    PRUEFE(re2fx_op_unbekannt() == 0, "unbekannte Ops: %u", re2fx_op_unbekannt());
    return fails != 0;
}

static int teil_op37_78(void)
{
    if (registrieren() != 0) { PRUEFE(0, "Registrierung"); return 1; }
    anfang();
    const int16_t ofs[4] = { 0, -1500, 0, 0 };
    /* Bank 9 Sub 8 (Skript @0x504: Op A 0, Op B 16, Op[2] 37, Op[3] 38, v (32,-100,0), a (0,10,0)) faellt auf den flachen Boden */
    const int j = re2fx_spawn_sofort(0x09080800u, 0, re2fx_einheitsmatrix, ofs);
    PRUEFE(j >= 0 && j < RE2FX_PLAETZE, "Spawn 0x09080800");
    for (int k = 1; k <= 80; k++) re2fx_tick();
    printf("  op37_78: Op 16 %u, Op 5 %u, Op 37 %u, Op 38 %u\n", re2fx_op_zaehler(16), re2fx_op_zaehler(5), re2fx_op_zaehler(37),
           re2fx_op_zaehler(38));
    PRUEFE(re2fx_op_zaehler(37) + re2fx_op_zaehler(38) > 0, "Brocken landet nicht (Op 37/38 nie gerufen)");
    if (j >= 0 && j < RE2FX_PLAETZE && re2fx_op_zaehler(37) > 0 && u16(re2fx_platz(j), 0x18) != 0) {
        const uint8_t *b = re2fx_platz(j);
        PRUEFE(b[0x21] >= 6 && (u16(b, 0x18) & ~0xA001u) == 0, "nach Op 37: Anim %d (>= 6), Status 0x%X (& 0xA001)", b[0x21],
               u16(b, 0x18));
    }
    /* Bank 0x1A Sub 1 (Skript @0x92C): Op 78 = Op 2 laedt Schritt 1 (v (0,-60,0), a (0,10,0)), dann * Skala / 3000 */
    anfang();
    const unsigned v78 = re2fx_op_zaehler(78);
    const int w = re2fx_spawn_sofort(0x1A011800u, 0, re2fx_einheitsmatrix, ofs);
    PRUEFE(w >= 0 && w < RE2FX_PLAETZE, "Spawn 0x1A011800");
    if (w >= 0 && w < RE2FX_PLAETZE) {
        re2fx_tick();
        const uint8_t *b = re2fx_platz(w);
        const int vy_soll = -60 * 0x1800 / 3000, ay_soll = 10 * 0x1800 / 3000;
        printf("  op78: Op 78 %u, vy %d (soll %d nach einem Schritt %d), ay %d (soll %d)\n", re2fx_op_zaehler(78) - v78, s16(b, 0x0E),
               vy_soll, vy_soll + ay_soll, (int8_t)b[0x09], ay_soll);
        PRUEFE(re2fx_op_zaehler(78) - v78 == 1, "Op 78 nicht genau einmal");
        PRUEFE((int8_t)b[0x09] == ay_soll, "Beschl. y %d statt %d (10 * 0x1800 / 3000)", (int8_t)b[0x09], ay_soll);
        PRUEFE(s16(b, 0x0E) == vy_soll || s16(b, 0x0E) == vy_soll + ay_soll, "v y %d statt %d (-60 * 0x1800 / 3000, ggf. + a)",
               s16(b, 0x0E), vy_soll);
    }
    PRUEFE(re2fx_op_unbekannt() == 0, "unbekannte Ops: %u", re2fx_op_unbekannt());
    return fails != 0;
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    int r = 1;
    if      (!strcmp(t, "register")) r = teil_register();
    else if (!strcmp(t, "op69"))     r = teil_op69();
    else if (!strcmp(t, "op41"))     r = teil_op41();
    else if (!strcmp(t, "op4_6"))    r = teil_op4_6();
    else if (!strcmp(t, "op37_78"))  r = teil_op37_78();
    else { printf("unbekannter Teil '%s'\n", t); return 2; }
    printf("%s: %s (%d Fehler)\n", t, r ? "FAIL" : "ok", fails);
    free(s_core); free(s_raum);
    return r;
}
