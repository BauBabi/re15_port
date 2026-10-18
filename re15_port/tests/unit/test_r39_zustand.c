/* test_r39_zustand.c — Runde 39 Spur F "zustand": volle Heilung = ganze Kleidung, Gift wie RE2, EKG.
 * Dossier analysis/befunde_runde39/F_zustand.md. Ohne Argument: Unit-Riegel (Mechanik):
 *   A  Kleidung: Heil-Applier (Tabelle @0x80010fbc, re15_item_use_apply) -> HP voll (100 `sh s0` @0x8004ae14
 *      + Westen-Bonus) nullt die Wund-Tabelle (= Builder FUN_80037c1c) und fordert die Haut neu an (= Haut-Upload
 *      des Spieler-Ladens FUN_8004ee78); Teilheilung laesst die Wunden stehen. Leon und Elza.
 *   B  Gift im echten Spielschritt (ROOM1130, keine Gegner): RE2 FUN_8003bfac @0x8003c12c-180 — Zaehler 30 am
 *      Treffer (@0x80020138-40), Abzug -1 HP bei Zaehler 0, Periode 31, Untergrenze HP 1, Takt ist kein Treffer,
 *      Pause (bltz) haelt den Takt an, Gegengift beendet ihn, Kommando 0 nullt ihn NICHT (kein Schreiber in RE2).
 *   C  EKG: Linienfarben des Statusschirms fuer Gift = RE2 POISON @0x800ab178-7d (ff 20 ff / 08 01 08 je Linie),
 *      FINE unveraendert (RE1.5 @0x8007622c).
 * Werkzeug-Teile fuer die Echtlauf-Riegel (tests/integration/test_r39_zustand_*.cmake):
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
static uint32_t u32(const unsigned char *b, size_t o)
{ return (uint32_t)b[o] | ((uint32_t)b[o+1] << 8) | ((uint32_t)b[o+2] << 16) | ((uint32_t)b[o+3] << 24); }

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
 * gruen 20/ff/20, gelb ff/ff/20, orange ff/80/20, rot ff/20/20, magenta ff/20/ff (jeweils mit Abdunkelung;
 * die Spur ist halbtransparent (ABE), Pixel also etwa (16,112,24) bzw. (131,20,135)). */
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

/* ================================================================================================= */
/* Unit-Riegel (Mechanik)                                                                             */
/* ================================================================================================= */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static re15_esp_t         s_esp;
static unsigned char     *s_rdt_buf;

/* ROOM1130 ohne Gegner (Lage der Echtlauf-Riegel: (-1024,0,-3336) rot 856, Cut 2) — Aufbau wie
 * test_r38_spinnen raum(), ohne Gegnerbaenke. */
static int raum(void)
{
    size_t n = 0; char p[512];
    snprintf(p, sizeof p, "%s/STAGE1/ROOM1130.RDT", RE15_ASSET_PSX_DIR);
    free(s_rdt_buf);
    s_rdt_buf = slurp(p, &n);
    if (!s_rdt_buf) { printf("FAIL: RDT fehlt: %s\n", p); s_fails++; return 0; }
    memset(&g_room_rdt, 0, sizeof g_room_rdt);
    if (re15_rdt_parse(s_rdt_buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT unlesbar\n"); s_fails++; return 0; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    memset(&s_esp, 0, sizeof s_esp);
    if (re15_esp_parse(s_rdt_buf, n, u32(s_rdt_buf, 0x4C), u32(s_rdt_buf, 0x50), u32(s_rdt_buf, 0x54),
                       u32(s_rdt_buf, 0x58), &s_esp) == 0)
        re15_esp_set_room_bank(&s_esp);
    else
        re15_esp_set_room_bank(NULL);
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 2;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    re15_esp_fx_reset();
    re15_spinnen_r38_reset();
    re15_zustand_r39_reset();
    re15_wound_reset();
    g_current_room_id = 0x1130;
    g_gameflow.character = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->status_flags = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -1024; pl->z = -3336; pl->rot_y = 856;
    re15_player_apply_hitbox(pl);
    re15_collision_set_band(pl->floor);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 2);
    pl->x = -1024; pl->y = 0; pl->z = -3336; pl->rot_y = 856;
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
    return 1;
}
static void bild(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
    re15_esp_fx_tick(re15_esp_room_bank());
}

/* ---- A: Kleidung ---------------------------------------------------------------------------------- */
static int wunden_summe(void)
{
    int s = 0;
    for (int p = 0; p < 8; p++) s += re15_wound_level(p) * 100 + re15_wound_acc(p);
    return s;
}
static void teil_a(void)
{
    printf("\n== A Kleidung: volle Heilung = ganze Kleidung (Applier @0x80010fbc, voll = 100 `sh s0` @0x8004ae14 + Weste)\n");
    re15_actor_init(); scd_vm_init(); re15_aot_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0;
    const int voll = re15_zustand_r39_hp_voll();
    CHECK(voll == 100 + re15_vest_hp_bonus(), "volle HP = %d (100 @0x8004ae14 + Westen-Bonus %d)", voll, re15_vest_hp_bonus());
    (void)re15_zustand_r39_haut_neu_abholen();

    /* (a) Leon: alle 8 Panels Stufe 2, HP 20, Heilspray 0x22 (absolut 100) */
    g_gameflow.character = 0;
    re15_wound_reset(); for (int p = 0; p < 8; p++) { re15_wound_add(p, 200); re15_wound_add(p, 200); }
    pl->hp = 20; pl->status_flags = 0;
    int w_vor = wunden_summe();
    re15_item_use_apply(0x22);
    CHECK(w_vor == 1600 && pl->hp == voll && wunden_summe() == 0 && re15_zustand_r39_haut_neu_pending() == 1,
          "Leon Heilspray 0x22: Wunden %d -> %d, HP 20 -> %d, Haut-Neuaufbau angefordert %d",
          w_vor, wunden_summe(), pl->hp, re15_zustand_r39_haut_neu_pending());
    CHECK(re15_zustand_r39_haut_neu_abholen() == 1 && re15_zustand_r39_haut_neu_abholen() == 0,
          "Anforderung wird genau einmal abgeholt");

    /* (b) Teilheilung: Gruenes Kraut 0x24 (+25 @0x8004af78) von 20 -> 45: Wunden bleiben */
    re15_wound_reset(); for (int p = 0; p < 8; p++) { re15_wound_add(p, 200); re15_wound_add(p, 200); }
    pl->hp = 20;
    re15_item_use_apply(0x24);
    CHECK(pl->hp == 45 && wunden_summe() == 1600 && re15_zustand_r39_haut_neu_pending() == 0,
          "Gruenes Kraut 0x24: HP 20 -> %d (< %d), Wunden bleiben (%d), keine Anforderung", pl->hp, voll, wunden_summe());
    /* (b2) zweites Kraut 45 -> 70: immer noch nicht voll */
    re15_item_use_apply(0x24);
    CHECK(pl->hp == 70 && wunden_summe() == 1600 && re15_zustand_r39_haut_neu_pending() == 0,
          "noch ein Kraut: HP %d, Wunden bleiben (%d)", pl->hp, wunden_summe());

    /* (c) additive Heilung erreicht voll: G+G+G 0x2b (+75 @0x8004afbc) von 30 -> 105 >= voll */
    re15_wound_reset(); re15_wound_add(5, 200);
    pl->hp = 30;
    re15_item_use_apply(0x2b);
    CHECK(pl->hp == 105 && pl->hp >= voll && wunden_summe() == 0 && re15_zustand_r39_haut_neu_abholen() == 1,
          "G+G+G 0x2b: HP 30 -> %d (>= %d), Wunde Panel 5 weg (%d), Haut neu", pl->hp, voll, wunden_summe());

    /* (d) Nuss 0x2f: absolut 77 (@0x8004b028) — nicht voll */
    re15_wound_reset(); re15_wound_add(7, 200);
    pl->hp = 20;
    re15_item_use_apply(0x2f);
    CHECK(pl->hp == 77 && wunden_summe() == 100 && re15_zustand_r39_haut_neu_pending() == 0,
          "Nuss 0x2f: HP %d, Wunde bleibt (%d)", pl->hp, wunden_summe());

    /* (e) Gegengift 0x23 (+25, Gift weg @0x8004af8c) von 90 vergiftet -> 115 voll */
    re15_wound_reset(); re15_wound_add(0, 200);
    pl->hp = 90; pl->status_flags = 2;
    re15_item_use_apply(0x23);
    CHECK(pl->hp == 115 && !(pl->status_flags & 2u) && wunden_summe() == 0 && re15_zustand_r39_haut_neu_abholen() == 1,
          "Gegengift 0x23 bei 90: HP %d, Gift weg (0x%04X), Wunde weg (%d), Haut neu", pl->hp, pl->status_flags,
          wunden_summe());

    /* (f) Elza (Charakter 4): dieselbe Mechanik (Panels sind figur-unabhaengig, LUT-Zeile 4 nur beim Blit) */
    g_gameflow.character = 4;
    re15_wound_reset(); for (int p = 0; p < 8; p++) { re15_wound_add(p, 200); re15_wound_add(p, 200); }
    pl->hp = 20;
    re15_item_use_apply(0x22);
    CHECK(pl->hp == voll && wunden_summe() == 0 && re15_zustand_r39_haut_neu_abholen() == 1,
          "Elza Heilspray 0x22: HP %d, Wunden weg (%d), Haut neu", pl->hp, wunden_summe());
    g_gameflow.character = 0;
    /* (g) Vollheilung ohne Wunden: harmlos (Tabelle bleibt 0, Anforderung trotzdem = Spieler-Laden) */
    re15_wound_reset(); pl->hp = 50;
    re15_item_use_apply(0x2a);
    CHECK(pl->hp == voll && wunden_summe() == 0 && re15_zustand_r39_haut_neu_abholen() == 1,
          "G+G+R 0x2a ohne Wunden: HP %d, Tabelle 0", pl->hp);
}

/* ---- B: Gift im Spielschritt ------------------------------------------------------------------------ */
static void teil_b(void)
{
    printf("\n== B Gift-Takt im Spielschritt (RE2 FUN_8003bfac @0x8003c12c-180, Zaehler 30 @0x80020138-40)\n");
    if (!raum()) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 5; f++) bild();                       /* Kommando 1 einschwingen */

    /* (a) stehend: Gift + Zaehler 30 -> Takte in Bild 30/61/92/123, je -1 HP, kein Treffer */
    pl->hp = 100; pl->status_flags = 2; pl->hit_react = 0;
    re15_zustand_r39_reset(); re15_zustand_r39_gift_setzen();
    const int32_t x0 = pl->x, z0 = pl->z;
    int takt[4] = { -1, -1, -1, -1 }, nt = 0, abzug_falsch = 0, flinch = 0, clip = 0, weg = 0;
    for (int f = 0; f < 124; f++) {
        int16_t vor = pl->hp;
        bild();
        if (pl->hp != vor) { if (nt < 4) takt[nt] = f; nt++; if (vor - pl->hp != 1) abzug_falsch++; }
        if (re15_player_hit_flinch_active()) flinch++;
        if (pl->motion == 8 || pl->motion == 9 || pl->motion == 0x0a) clip++;
        int w = abs(pl->x - x0) + abs(pl->z - z0);
        if (w > weg) weg = w;
    }
    CHECK(nt == 4 && abzug_falsch == 0 && takt[0] == 30 && takt[1] == 61 && takt[2] == 92 && takt[3] == 123,
          "stehend: %d Takte zu -1 HP in Bild %d/%d/%d/%d (Periode 31 = Zaehler 30..0 @0x8003c144-64)",
          nt, takt[0], takt[1], takt[2], takt[3]);
    CHECK(flinch == 0 && clip == 0 && weg == 0 && pl->hp == 96 && re15_zustand_r39_gift_takte() == 4,
          "stehend: keine Treffer-Reaktion (Flinch %d, Clip 8/9/0xa %d, Rueckstoss %d), HP %d, Takte %u",
          flinch, clip, weg, pl->hp, re15_zustand_r39_gift_takte());

    /* (b) Pause (RE15_PAUSE_PLAYER = das bltz @0x80031c78 / RE2 DAT_800cfbdc < 0 @0x8003bfac-c0): kein Takt, Zaehler steht */
    {
        unsigned z0z = re15_zustand_r39_gift_zaehler(); int16_t h0 = pl->hp;
        g_re15_pauseflags |= RE15_PAUSE_PLAYER;
        for (int f = 0; f < 50; f++) bild();
        unsigned z1 = re15_zustand_r39_gift_zaehler(); int16_t h1 = pl->hp;
        g_re15_pauseflags &= ~RE15_PAUSE_PLAYER;
        CHECK(z0z == z1 && h0 == h1, "eingefroren: Zaehler %u -> %u, HP %d -> %d (unveraendert)", z0z, z1, h0, h1);
    }

    /* (c) Kommando 0 nullt den RE2-Zaehler nicht (kein Schreiber ausser Op 32/Takt/Poison_clr) */
    {
        extern void re15_player_cmd_zero(void);
        unsigned zv = re15_zustand_r39_gift_zaehler();
        re15_player_cmd_zero();
        CHECK(re15_zustand_r39_gift_zaehler() == zv, "Kommando 0: Zaehler %u bleibt %u", zv, re15_zustand_r39_gift_zaehler());
    }

    /* (d) Untergrenze: HP 2 -> 1, dann kein Abzug mehr (`slti v0,v0,0x2` @0x8003c174) */
    pl->hp = 2; re15_zustand_r39_reset();
    int min_hp = 99;
    for (int f = 0; f < 100; f++) { bild(); if (pl->hp < min_hp) min_hp = pl->hp; }
    CHECK(pl->hp == 1 && min_hp == 1 && re15_zustand_r39_gift_takte() == 1,
          "Untergrenze: HP 2 -> %d, Minimum %d, Takte %u (kein Tod durch Gift)", pl->hp, min_hp,
          re15_zustand_r39_gift_takte());

    /* (e) Gegengift beendet den Takt (@0x8004af8c loescht das Bit; RE2 @0x8006b1e8 `sh zero` 0x800cfe10) */
    pl->hp = 50; pl->status_flags = 2; re15_zustand_r39_reset();
    re15_item_use_apply(0x23);
    int16_t h2 = pl->hp;
    for (int f = 0; f < 100; f++) bild();
    CHECK(!(pl->status_flags & 2u) && pl->hp == h2 && h2 == 75,
          "Gegengift: Bit 0x%04X, HP %d bleibt ueber 100 Bilder", pl->status_flags, pl->hp);
}

/* ---- C: EKG-Farbe ---------------------------------------------------------------------------------- */
static int ekg_linien(int cond, uint8_t r[32], uint8_t g[32], uint8_t b[32])
{
    static re15_inv_op_t ops[RE15_INV_MAX_OPS];
    g_inv_screen.cond = (uint8_t)cond;
    g_inv_screen.ecg_sweep = 0x48;                            /* alle 32 Linien sichtbar (Tor @0x80048c14-1c) */
    int n = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
    int k = 0;
    for (int i = 0; i < n && k < 32; i++)
        if (ops[i].kind == RE15_INV_OP_LINE && ops[i].abe) { r[k] = ops[i].r; g[k] = ops[i].g; b[k] = ops[i].b; k++; }
    return k;
}
static void teil_c(void)
{
    printf("\n== C EKG: Gift = RE2-Farbe @0x800ab178-7d (ff 20 ff, Schritt 08 01 08), FINE = RE1.5 @0x8007622c\n");
    re15_inv_screen_open();
    g_inv_screen.substate = 3;                                 /* ITEM-Hauptschirm */
    uint8_t r[32], g[32], b[32];
    int k = ekg_linien(3, r, g, b);
    int ok = (k == 32);
    for (int i = 0; i < k; i++)
        if (r[i] != (uint8_t)(0xff - 8 * i) || g[i] != (uint8_t)(0x20 - i) || b[i] != (uint8_t)(0xff - 8 * i)) ok = 0;
    CHECK(ok, "Gift (cond 3): %d Linien, Linie 0 = (%02x,%02x,%02x), Linie 31 = (%02x,%02x,%02x) = base - step*i",
          k, k ? r[0] : 0, k ? g[0] : 0, k ? b[0] : 0, k ? r[k-1] : 0, k ? g[k-1] : 0, k ? b[k-1] : 0);
    k = ekg_linien(0, r, g, b);
    ok = (k == 32);
    for (int i = 0; i < k; i++)
        if (r[i] != 0x20 || g[i] != (uint8_t)(0xff - 8 * i) || b[i] != 0x20) ok = 0;   /* 20 ff 20, Maske 2 */
    CHECK(ok, "FINE (cond 0): %d Linien, Linie 0 = (%02x,%02x,%02x) unveraendert RE1.5 (20 ff 20, Maske 02)",
          k, k ? r[0] : 0, k ? g[0] : 0, k ? b[0] : 0);
    /* Klassifikator: Gift -> 3 unabhaengig von den HP (FUN_8004ed6c; RE2 FUN_80069538 ebenso 4 vor allem anderen) */
    CHECK(re15_inv_screen_condition(100, 1) == 3 && re15_inv_screen_condition(5, 1) == 3 &&
          re15_inv_screen_condition(5, 0) == 2 && re15_inv_screen_condition(100, 0) == 0,
          "Klassifikator: Gift vor HP (3), sonst 0/2");
}

static void teil_unit(void)
{
    teil_a();
    teil_b();
    teil_c();
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
