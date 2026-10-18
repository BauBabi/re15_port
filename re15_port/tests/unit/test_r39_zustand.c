/* test_r39_zustand.c — Runde 39 Spur F "zustand": volle Heilung = ganze Kleidung, Gift wie RE2, EKG.
 * Dossier analysis/befunde_runde39/F_zustand.md. Ohne Argument: Unit-Riegel (Mechanik). Werkzeug-Teile
 * fuer die Echtlauf-Riegel (tests/integration/test_r39_zustand_*.cmake):
 *   karte <datei> <raum-hex> <char 0|4> <x> <y> <z> <rot> <hp> <wundspec> <inv> [cut]
 *         Spielstand Slot 0 (v9 mit wounds[8][2]); wundspec wie test_r36_kleidung ("-" | "alle:N" | "5:1,7:1");
 *         inv = "22:1,03:15" (Id hex : Menge dezimal, Slot 0.., "-" = leer).
 *   bild  <a.ppm> <b.ppm> <min_diff> <x0> <y0> <x1> <y1>   Pixel-Differenz zweier Framedumps (wie r36)
 *   ekg   <a.ppm> <x0> <y0> <x1> <y1>                      Farbzensus im Kasten (gruen/gelb/rot/magenta)
 *   hp    <log>                                            HP-Takte aus RE15_R39_LOG (Bild, Abzug, Abstand) */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_item_use.h"
#include "re15_inv_screen.h"
#include "re15_msg.h"
#include "re15_esp.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_gameflow.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_spinnen_r38.h"
#include "re15_zustand_r39.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int s_fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); s_fails++; } \
                              else { printf("ok   "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static unsigned char *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }
    unsigned char *d = (unsigned char *)malloc((size_t)len + 1);
    if (!d) { fclose(f); return NULL; }
    size_t r = fread(d, 1, (size_t)len, f); fclose(f);
    d[r] = 0; *n = r;
    return d;
}

/* ------------------------------------------------------------------------------------------------- */
/* karte */
static void wunden_setzen(const char *w)
{
    if (strncmp(w, "alle:", 5) == 0) {
        int lv = atoi(w + 5);
        for (int p = 0; p < 8; p++) for (int k = 0; k < lv; k++) re15_wound_add(p, 200);
    } else if (strcmp(w, "-") != 0) {
        const char *q = w;
        while (*q) {
            int p = atoi(q); const char *c = strchr(q, ':'); if (!c) break;
            int lv = atoi(c + 1);
            for (int k = 0; k < lv; k++) re15_wound_add(p, 200);
            const char *k2 = strchr(q, ','); if (!k2) break; q = k2 + 1;
        }
    }
}
static void inventar_setzen(const char *spec)
{
    re15_inv_init();
    re15_inv_set_equipped_slot(0x80);
    re15_inv_set_prev_equip_slot(0x80);
    if (strcmp(spec, "-") == 0) return;
    const char *q = spec; int slot = 0;
    while (*q && slot < RE15_INV_MAX_SLOTS) {
        unsigned id = (unsigned)strtoul(q, (char **)&q, 16);
        int qty = 0;
        if (*q == ':') { q++; qty = (int)strtol(q, (char **)&q, 10); }
        g_inv.slots[slot].id = (uint8_t)id; g_inv.slots[slot].qty = (uint8_t)qty; g_inv.slots[slot].flags = 0;
        slot++;
        while (*q == ',' || *q == ' ') q++;
    }
}
static int teil_karte(int argc, char **argv)
{
    if (argc < 12) { printf("karte <datei> <raum-hex> <char> <x> <y> <z> <rot> <hp> <wundspec> <inv> [cut]\n"); return 2; }
    const char *path = argv[2];
    unsigned room = (unsigned)strtoul(argv[3], NULL, 16);
    int chr = atoi(argv[4]);
    scd_vm_init(); re15_actor_init(); re15_aot_init();
    re15_wound_reset();
    g_current_room_id = (int)room;
    g_gameflow.character = (uint8_t)chr;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0;
    pl->x = atoi(argv[5]); pl->y = atoi(argv[6]); pl->z = atoi(argv[7]);
    pl->rot_y = (int16_t)atoi(argv[8]); pl->hp = (int16_t)atoi(argv[9]);
    inventar_setzen(argv[11]);
    g_scd.cam_id = (uint8_t)((argc > 12) ? atoi(argv[12]) : 0);
    wunden_setzen(argv[10]);
    re15_savedata_t sd; re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, chr ? "ELZA  R39F " : "LEON  R39F ") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path); return 1;
    }
    printf("Karte %s: Raum %04X char %d pos (%d,%d,%d) rot %d hp %d inv %s wunden", path, room, chr,
           (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y, (int)pl->hp, argv[11]);
    for (int p = 0; p < 8; p++) printf(" %u", sd.wounds[p][0]);
    printf("\n");
    return 0;
}

/* ------------------------------------------------------------------------------------------------- */
/* bild / ekg (P6 320x240) */
static unsigned char *ppm_lesen(const char *path, int *w, int *h)
{
    size_t n = 0; unsigned char *d = slurp(path, &n);
    if (!d) return NULL;
    int maxv = 0, pos = 0;
    if (sscanf((const char *)d, "P6 %d %d %d%n", w, h, &maxv, &pos) != 3) { free(d); return NULL; }
    pos++;
    if ((size_t)pos + (size_t)(*w) * (size_t)(*h) * 3 > n) { free(d); return NULL; }
    unsigned char *px = (unsigned char *)malloc((size_t)(*w) * (size_t)(*h) * 3);
    memcpy(px, d + pos, (size_t)(*w) * (size_t)(*h) * 3); free(d);
    return px;
}
static int teil_bild(int argc, char **argv)
{
    if (argc < 9) { printf("bild <a.ppm> <b.ppm> <min_diff> <x0> <y0> <x1> <y1>\n"); return 2; }
    int wa, ha, wb, hb;
    unsigned char *a = ppm_lesen(argv[2], &wa, &ha), *b = ppm_lesen(argv[3], &wb, &hb);
    if (!a || !b) { printf("FAIL: PPM nicht lesbar (%s / %s)\n", argv[2], argv[3]); return 1; }
    if (wa != wb || ha != hb) { printf("FAIL: Groesse %dx%d vs %dx%d\n", wa, ha, wb, hb); return 1; }
    int min_diff = atoi(argv[4]);
    int x0 = atoi(argv[5]), y0 = atoi(argv[6]), x1 = atoi(argv[7]), y1 = atoi(argv[8]);
    int diff = 0, aussen = 0, bx0 = wa, by0 = ha, bx1 = -1, by1 = -1;
    for (int y = 0; y < ha; y++) for (int x = 0; x < wa; x++) {
        size_t o = ((size_t)y * (size_t)wa + (size_t)x) * 3;
        if (a[o] != b[o] || a[o+1] != b[o+1] || a[o+2] != b[o+2]) {
            diff++;
            if (x < bx0) bx0 = x; if (x > bx1) bx1 = x; if (y < by0) by0 = y; if (y > by1) by1 = y;
            if (x < x0 || x >= x1 || y < y0 || y >= y1) aussen++;
        }
    }
    printf("bild: %d Pixel verschieden, Kasten (%d,%d)-(%d,%d), ausserhalb [%d,%d)x[%d,%d): %d\n",
           diff, bx0, by0, bx1, by1, x0, x1, y0, y1, aussen);
    free(a); free(b);
    if (min_diff == 0) { if (diff != 0) { printf("FAIL: %d Pixel weichen ab, erwartet 0\n", diff); return 1; } return 0; }
    if (diff < min_diff) { printf("FAIL: nur %d Pixel verschieden, mindestens %d erwartet\n", diff, min_diff); return 1; }
    if (aussen != 0) { printf("FAIL: %d Abweichungen ausserhalb der Figur\n", aussen); return 1; }
    return 0;
}
/* Farbklassen fuer den EKG-Kasten: Linienfarben der Tabellen (RE1.5 @0x8007622c, RE2 @0x800ab160) —
 * gruen 20/ff/20, gelb ff/ff/20, orange ff/80/20, rot ff/20/20, magenta ff/20/ff (jeweils mit Abdunkelung). */
static int teil_ekg(int argc, char **argv)
{
    if (argc < 7) { printf("ekg <a.ppm> <x0> <y0> <x1> <y1>\n"); return 2; }
    int w, h; unsigned char *a = ppm_lesen(argv[2], &w, &h);
    if (!a) { printf("FAIL: PPM nicht lesbar (%s)\n", argv[2]); return 1; }
    int x0 = atoi(argv[3]), y0 = atoi(argv[4]), x1 = atoi(argv[5]), y1 = atoi(argv[6]);
    int gruen = 0, gelb = 0, rot = 0, magenta = 0, sonst = 0, hell = 0;
    for (int y = y0; y < y1 && y < h; y++) for (int x = x0; x < x1 && x < w; x++) {
        size_t o = ((size_t)y * (size_t)w + (size_t)x) * 3;
        int r = a[o], g = a[o+1], b = a[o+2];
        if (r + g + b < 120) continue;                      /* Hintergrund/Bezel dunkel */
        hell++;
        if      (g > r + 40 && g > b + 40) gruen++;
        else if (r > b + 40 && g > b + 40 && abs(r - g) < 70) gelb++;
        else if (r > g + 40 && r > b + 40) rot++;
        else if (r > g + 40 && b > g + 40) magenta++;
        else sonst++;
    }
    printf("ekg: Kasten [%d,%d)x[%d,%d) hell=%d gruen=%d gelb=%d rot=%d magenta=%d sonst=%d\n",
           x0, x1, y0, y1, hell, gruen, gelb, rot, magenta, sonst);
    free(a);
    return 0;
}
/* hp <log>: RE15_R39_LOG-Zeilen "F<n> aktiv=.. hp=<hp> status=0x.... ..." -> Takte (Bild, Abzug, Abstand) */
static int teil_hp(int argc, char **argv)
{
    if (argc < 3) { printf("hp <log>\n"); return 2; }
    FILE *f = fopen(argv[2], "r");
    if (!f) { printf("FAIL: Log nicht lesbar (%s)\n", argv[2]); return 1; }
    char z[512]; long letzt_f = -1, vor_hp = -1, erster = -1; int n = 0, summe = 0, perioden_ok = 0, perioden = 0;
    long letzt_takt = -1; int abzug_min = 9999, abzug_max = -9999, min_hp = 9999, status_an = 0;
    while (fgets(z, sizeof z, f)) {
        long fr; int hp; unsigned st;
        if (sscanf(z, "F%ld aktiv=%*d hp=%d status=0x%x", &fr, &hp, &st) != 3) continue;
        if (st & 2u) status_an++;
        if (hp < min_hp) min_hp = hp;
        if (vor_hp >= 0 && hp < vor_hp) {
            int d = (int)(vor_hp - hp);
            n++; summe += d;
            if (d < abzug_min) abzug_min = d;
            if (d > abzug_max) abzug_max = d;
            if (erster < 0) erster = fr;
            if (letzt_takt >= 0) { perioden++; if (fr - letzt_takt == 31) perioden_ok++; }
            printf("takt %d: F%ld hp %ld -> %d (-%d) Abstand %ld\n", n, fr, vor_hp, hp, d,
                   letzt_takt >= 0 ? fr - letzt_takt : 0L);
            letzt_takt = fr;
        }
        vor_hp = hp; letzt_f = fr;
    }
    fclose(f);
    printf("hp: %d Takte, Summe %d, Abzug min %d max %d, erster Takt F%ld, Abstaende 31: %d von %d, HP-Minimum %d, "
           "Giftbilder %d, letztes Bild F%ld\n", n, summe, abzug_min, abzug_max, erster, perioden_ok, perioden, min_hp,
           status_an, letzt_f);
    return 0;
}

/* ------------------------------------------------------------------------------------------------- */
/* Unit-Riegel (Mechanik) — folgt mit der Umsetzung (Stand 1: Werkzeug) */
static void teil_unit(void)
{
    printf("== Unit-Riegel Runde 39 Spur F (Stand 1: noch ohne Mechanik-Pruefung)\n");
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "karte") == 0) return teil_karte(argc, argv);
        if (strcmp(argv[1], "bild")  == 0) return teil_bild(argc, argv);
        if (strcmp(argv[1], "ekg")   == 0) return teil_ekg(argc, argv);
        if (strcmp(argv[1], "hp")    == 0) return teil_hp(argc, argv);
        printf("unbekannter Teil: %s\n", argv[1]); return 2;
    }
    teil_unit();
    printf(s_fails ? "\n%d FEHLER\n" : "\nALLE PRUEFUNGEN GRUEN\n", s_fails);
    return s_fails ? 1 : 0;
}
