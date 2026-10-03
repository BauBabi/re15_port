/* test_r35_entladen.c — Runde 35 Spur I: Riegel fuer das Entladen der Raum-Assets.
 *
 * Dossier: analysis/befunde_runde35/I_entladen.md. Zwei Teile:
 *
 *   beleg     Die Original-Bytes, auf denen die Grenzen des Ports stehen, liegen wirklich so in
 *             info/Re1.5/PSX.EXE (Auslieferungsstand; jal-Woerter VOLL gescannt, nicht gegrept):
 *               - FUN_800396fc setzt die Arena zurueck: `sw a0,-0x3884(at)` @0x80039738 (0x800ac77c),
 *                 `sw a0,-0x3888(at)` @0x80039740 (0x800ac778), Basis `lw a0,-0x3880(a0)` @0x80039704
 *               - genau ZWEI `jal 0x800396fc`: @0x8001d5ac (Spielmodul-Init) und @0x8001d988 (Tuer)
 *               - genau EIN `jal 0x80039590` (Masken-Zeichner): @0x8001ce54 (Spielmodul-Schleife)
 *               - `jal 0x80039270` (Masken-Tabelle in der Arena) @0x800399cc
 *               - `jal 0x8001b3f8` (Raum-Animation neu binden) @0x80039a08, unbedingt
 *               - Spielmodul-Init setzt die Arena selbst: `sw v0,-0x3884(at)` @0x8001d5a0
 *   n1beleg   Nachbesserung 1 (Abnahme 0, M1/M2) — Original-Bytes:
 *               RE1.5 info/Re1.5/PSX.EXE: Sce_em_set legt JEDES Modell in die Arena (`lw s1,-0x3884(s1)`
 *               @0x800422c4, `jal 0x80022300` @0x80042328, `sw s1,-0x3884(at)` @0x80042554); Spieler-PLD
 *               in den festen Puffer 0x801bd814 (`lui a1,0x801b` @0x800314c8, `ori a1,a1,0xd814`
 *               @0x800314cc); einziger Schreiber der Arena-Basis 0x800ac780 = `sw v1` @0x80039a58.
 *               RE2 info/re2leon/PSX.EXE: Raumlader `jal 0x80012fb8` @0x8004a1c4; der Leser sendet
 *               Pause (a0=9 @0x800130d4, jal DsCommand @0x800130e0), Setmode (a0=14 @0x800130f0) mit
 *               Byte 0xA0 @0x8009a429 (Bit 6 XA-ADPCM = 0), SeekL (21 @0x80013110), ReadN (6
 *               @0x80013140); Stimme abspielen mit Setmode-Byte 0xC8 @0x8009a415 (Bit 6 = 1).
 *   gegner    Mechanik der Generation an der Gegner-Registry (engine): eine angelegte Bank traegt
 *             die laufende Generation; nach einer Grenze ist sie FREMD; re15_enemy_reset leert
 *             alle Baenke (das ruft das Entladen an jeder Grenze).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_entladen.h"
#include "re15_enemy.h"

#ifndef RE15_REPO_ROOT
#define RE15_REPO_ROOT "."
#endif

static int s_fehler = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); s_fehler++; } } while (0)

static uint8_t *datei_lesen(const char *rel, size_t *n)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/%s", RE15_REPO_ROOT, rel);
    FILE *f = fopen(p, "rb");
    if (!f) { printf("FAIL: %s nicht lesbar\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    *n = (size_t)sz;
    return b;
}

static uint32_t rd32(const uint8_t *b, size_t off) {
    return (uint32_t)b[off] | ((uint32_t)b[off+1] << 8) | ((uint32_t)b[off+2] << 16) | ((uint32_t)b[off+3] << 24);
}

static int test_beleg(void)
{
    size_t n = 0;
    uint8_t *b = datei_lesen("info/Re1.5/PSX.EXE", &n);
    if (!b) return 1;
    const uint32_t t_addr = rd32(b, 0x18);
#define WORT(a) rd32(b, 0x800u + (uint32_t)(a) - t_addr)
    PRUEF(WORT(0x80039704) == 0x8c84c780u, "@0x80039704 lw a0,-0x3880(a0) erwartet, %08x", WORT(0x80039704));
    PRUEF(WORT(0x80039738) == 0xac24c77cu, "@0x80039738 sw a0,-0x3884(at) erwartet, %08x", WORT(0x80039738));
    PRUEF(WORT(0x80039740) == 0xac24c778u, "@0x80039740 sw a0,-0x3888(at) erwartet, %08x", WORT(0x80039740));
    PRUEF(WORT(0x8001d5a0) == 0xac22c77cu, "@0x8001d5a0 sw v0,-0x3884(at) erwartet, %08x", WORT(0x8001d5a0));
    PRUEF(WORT(0x800399cc) == (0x0C000000u | ((0x80039270u >> 2) & 0x3FFFFFFu)), "@0x800399cc jal 0x80039270");
    PRUEF(WORT(0x80039a08) == (0x0C000000u | ((0x8001b3f8u >> 2) & 0x3FFFFFFu)), "@0x80039a08 jal 0x8001b3f8");

    /* Voll-Scan der jal-Woerter. */
    const uint32_t jal_lader   = 0x0C000000u | ((0x800396fcu >> 2) & 0x3FFFFFFu);
    const uint32_t jal_zeichne = 0x0C000000u | ((0x80039590u >> 2) & 0x3FFFFFFu);
    int n_lader = 0, n_zeichne = 0, lader_ok = 1, zeichne_ok = 1;
    for (size_t off = 0x800; off + 4 <= n; off += 4) {
        uint32_t w = rd32(b, off);
        uint32_t a = (uint32_t)(off - 0x800) + t_addr;
        if (w == jal_lader)   { n_lader++;   if (a != 0x8001d5acu && a != 0x8001d988u) lader_ok = 0; }
        if (w == jal_zeichne) { n_zeichne++; if (a != 0x8001ce54u) zeichne_ok = 0; }
    }
    PRUEF(n_lader == 2 && lader_ok, "jal 0x800396fc: %d Stellen (erwartet genau @0x8001d5ac + @0x8001d988)", n_lader);
    PRUEF(n_zeichne == 1 && zeichne_ok, "jal 0x80039590: %d Stellen (erwartet genau @0x8001ce54)", n_zeichne);
#undef WORT
    free(b);
    printf("beleg: Arena-Reset @0x80039738/40, Lader-Aufrufer %d, Zeichner-Aufrufer %d\n", n_lader, n_zeichne);
    return 0;
}

#define JAL(z) (0x0C000000u | (((uint32_t)(z) >> 2) & 0x3FFFFFFu))
static int test_n1beleg(void)
{
    size_t n = 0, n2 = 0;
    uint8_t *b = datei_lesen("info/Re1.5/PSX.EXE", &n);
    uint8_t *r = datei_lesen("info/re2leon/PSX.EXE", &n2);
    if (!b || !r) { free(b); free(r); return 1; }
    const uint32_t t = rd32(b, 0x18), t2 = rd32(r, 0x18);
#define W15(a) rd32(b, 0x800u + (uint32_t)(a) - t)
#define W2(a)  rd32(r, 0x800u + (uint32_t)(a) - t2)
#define B2(a)  r[0x800u + (uint32_t)(a) - t2]
    /* M2: Modell jedes Sce_em_set-Typs (auch 0x47 Elliot) in der Arena */
    PRUEF(W15(0x800422c4) == 0x8e31c77cu, "@0x800422c4 lw s1,-0x3884(s1) (Arena-Kopf 0x800ac77c), %08x", W15(0x800422c4));
    PRUEF(W15(0x80042328) == JAL(0x80022300), "@0x80042328 jal 0x80022300 (EMD in die Arena), %08x", W15(0x80042328));
    PRUEF(W15(0x80042554) == 0xac31c77cu, "@0x80042554 sw s1,-0x3884(at) (Kopf hinter das Modell), %08x", W15(0x80042554));
    /* M2: Spieler in festem Puffer 0x801bd814 (nicht Arena) */
    PRUEF(W15(0x800314c8) == 0x3c05801bu, "@0x800314c8 lui a1,0x801b, %08x", W15(0x800314c8));
    PRUEF(W15(0x800314cc) == 0x34a5d814u, "@0x800314cc ori a1,a1,0xd814, %08x", W15(0x800314cc));
    /* Arena-Basis 0x800ac780: genau ein Schreiber (sw ...,-0x3880) @0x80039a58 */
    int n_basis = 0, basis_ok = 1;
    for (size_t off = 0x800; off + 4 <= n; off += 4) {
        uint32_t w = rd32(b, off);
        if ((w >> 26) == 0x2bu && (w & 0xFFFFu) == 0xc780u) {
            n_basis++;
            if ((uint32_t)(off - 0x800) + t != 0x80039a58u) basis_ok = 0;
        }
    }
    PRUEF(n_basis == 1 && basis_ok && W15(0x80039a58) == 0xac23c780u,
          "Arena-Basis-Schreiber: %d Stellen (erwartet genau sw v1 @0x80039a58)", n_basis);
    /* M1 (RE2): Raumlader liest die RDT von CD; der Leser schaltet XA-ADPCM ab */
    PRUEF(W2(0x8004a1c4) == JAL(0x80012fb8), "RE2 @0x8004a1c4 jal 0x80012fb8, %08x", W2(0x8004a1c4));
    PRUEF(W2(0x800130d4) == 0x24040009u && W2(0x800130e0) == JAL(0x8008a380), "RE2 @0x800130d4/e0 DsCommand(9 Pause)");
    PRUEF(W2(0x800130f0) == 0x2404000eu, "RE2 @0x800130f0 addiu a0,zero,14 (Setmode), %08x", W2(0x800130f0));
    PRUEF(W2(0x80013110) == 0x24040015u, "RE2 @0x80013110 addiu a0,zero,21 (SeekL), %08x", W2(0x80013110));
    PRUEF(W2(0x80013140) == 0x24040006u, "RE2 @0x80013140 addiu a0,zero,6 (ReadN), %08x", W2(0x80013140));
    PRUEF(B2(0x8009a429) == 0xa0u && (B2(0x8009a429) & 0x40u) == 0, "RE2 @0x8009a429 Setmode Datei = 0xA0 (XA aus), %02x", B2(0x8009a429));
    PRUEF(B2(0x8009a415) == 0xc8u && (B2(0x8009a415) & 0x40u) != 0, "RE2 @0x8009a415 Setmode Stimme = 0xC8 (XA an), %02x", B2(0x8009a415));
#undef W15
#undef W2
#undef B2
    free(b); free(r);
    printf("n1beleg: Elliot/Typ-Modelle in der Arena (@0x80042328), Spieler fest (0x801bd814), "
           "RE2-Raumlader schaltet XA ab (@0x8004a1c4 -> Setmode 0xA0)
");
    return 0;
}

static int test_gegner(void)
{
    re15_enemy_reset();
    unsigned g0 = g_re15_entladen_gen;
    PRUEF(g0 != 0, "Generation 0 ist 'nie gefuellt' und darf nicht laufen");
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
    PRUEF(eb != NULL, "re15_enemy_alloc(0x10) lieferte NULL");
    if (!eb) return 1;
    eb->ok = 1;
    int bank = (int)(eb - g_enemy);
    PRUEF(re15_entladen_gegner_gen(bank) == g0, "Bank %d traegt Generation %u statt %u", bank,
          re15_entladen_gegner_gen(bank), g0);
    re15_entladen_gen_weiter();
    PRUEF(g_re15_entladen_gen == g0 + 1u, "Grenze zaehlt nicht weiter");
    PRUEF(re15_entladen_gegner_gen(bank) != g_re15_entladen_gen, "Bank aus der alten Generation gilt nicht als FREMD");
    re15_enemy_reset();
    int belegt = 0;
    for (int i = 0; i < RE15_ENEMY_MAX; i++) belegt += g_enemy[i].ok ? 1 : 0;
    PRUEF(belegt == 0, "nach re15_enemy_reset noch %d Baenke belegt", belegt);
    eb = re15_enemy_alloc(0x11);
    PRUEF(eb && re15_entladen_gegner_gen((int)(eb - g_enemy)) == g_re15_entladen_gen,
          "neue Bank nach der Grenze traegt nicht die neue Generation");
    re15_enemy_reset();
    /* Ueberlauf: 0 bleibt "nie gefuellt". */
    g_re15_entladen_gen = 0xFFFFFFFFu;
    re15_entladen_gen_weiter();
    PRUEF(g_re15_entladen_gen == 1u, "Ueberlauf landet auf %u statt 1", g_re15_entladen_gen);
    printf("gegner: Generation je Bank, fremd nach Grenze, leer nach Reset\n");
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    if (!strcmp(teil, "beleg") || !strcmp(teil, "alle"))  test_beleg();
    if (!strcmp(teil, "gegner") || !strcmp(teil, "alle")) test_gegner();
    if (!strcmp(teil, "n1beleg") || !strcmp(teil, "alle")) test_n1beleg();
    if (s_fehler) { printf("unit_r35_entladen_%s: %d Fehler\n", teil, s_fehler); return 1; }
    printf("unit_r35_entladen_%s OK\n", teil);
    return 0;
}
