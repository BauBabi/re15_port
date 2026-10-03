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

static uint8_t *exe_lesen(size_t *n)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/info/Re1.5/PSX.EXE", RE15_REPO_ROOT);
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
    uint8_t *b = exe_lesen(&n);
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
    if (s_fehler) { printf("unit_r35_entladen_%s: %d Fehler\n", teil, s_fehler); return 1; }
    printf("unit_r35_entladen_%s OK\n", teil);
    return 0;
}
