/* =============================================================================
 * RIEGEL r42/karte Stufe 3 Runde 7 (Orchestrator-Vorgabe ab Runde 7, Punkt A): der Punkt laeuft beim Gehen
 * mindestens so GLATT mit wie in Runde 5.
 * =============================================================================
 * Pruefbericht pruef_s3_r6 ROT 2: "Die Klemme aus Runde 6 ... macht das Mitlaufen in anderen Raeumen schlechter als in
 * Runde 5 ... Spruenge von mindestens 4 px: R5 406, neu 646." Vorgabe: "Das Mitlaufen muss mindestens so glatt sein wie in
 * Runde 5 (Raster-Messung des Pruefers als Riegel uebernehmen: Spruenge >= 4 px je Raum nicht mehr als in Runde 5)."
 *
 * Messung wie die Pruefung (pruef_s3_r6/sweep, ausw_begeh): Weltraster im Abstand 500 ueber -32000..32000; gezaehlt
 * werden nur BEGEHBARE Lagen IM RAUM: die Spieler-Kollision re15_collision_constrain (FUN_8003b0a4, game_step_common.c)
 * verschiebt sie nicht (Band 0), und ihr ROHER Punkt (nur Zeile + Rechteck, ohne Klemmen und Gang-Zeile; Messhaken
 * g_re15_karte_r42_roh) liegt auf dem gemalten Raumkoerper der eigenen Kachel (re15_karte_r42_kunst_gemalt an der Ring-Mitte)
 * - so schliesst die Pruefung die begehbare Leere ausserhalb der Raumwaende aus. Fuer jede Lage baut der Riegel den
 * Kartenschirm auf (wie test_r42_karte_lauf.c punkt(), Raum nur einmal geladen) und liest den Punkt. Gezaehlt wird ein SPRUNG, wenn zwei Nachbarlagen (x+500 bzw. z+500), beide
 * begehbar, auf demselben Blatt 6..12 liegen und ihre Punkte um mindestens 4 Bildpunkte auseinander liegen
 * (Chebyshev-Abstand). Je Raum darf die Zahl nicht ueber dem Stand von Runde 5 (02972374c) liegen.
 *
 * Bezugswerte: gemessen mit diesem Riegel (R42_GLATT_MESS=1) auf dem Stand von Runde 5 (Bibliothek dieser Runde, Klemme
 * karte_r42.c aus 02972374c, ohne Gang-Zeile). Sie stimmen mit der Spalte 'alt' der Pruefung (ausw_begeh.txt) ueberein.
 * Gegenprobe: auf 1713acd21 (Runde 6) ROT - u. a. 2050/2051, 20B0/20B1, 11A0/11A1, 2030/2031, 3050/3051, 50D0, 6000.
 *
 * Aufruf: test_r42_karte_glatt <Stage 2..6>   (je Stage ein ctest-Eintrag, damit -j sie parallel faehrt; Stage 2 mit den
 *         STAGE1-Raeumen auf Blatt 6..12)
 *         R42_GLATT_MESS=1 -> nur messen und die Tabelle ausgeben (kein Vergleich).
 *         R42_GLATT_RAUM=4080 -> nur diesen Raum.
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_inv_screen.h"
#include "re15_scd.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_esp.h"
#include "re15_msg.h"
#include "re15_karte_r42.h"

extern re15_inv_screen_t g_inv_screen;
extern unsigned g_current_room_id;

static re15_rdt_t s_rdt;
static uint8_t *s_roh = NULL;

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    long sz;
    uint8_t *b;
    if (!f) return 0;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return 0; }
    b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return 0; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return 0; }
    fclose(f); *n = (size_t)sz; return b;
}

static int betrete(unsigned rid)
{
    char pfad[600];
    size_t n = 0;
    re15_actor_t *pl;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    if (s_roh) { free(s_roh); s_roh = NULL; }
    s_roh = slurp(pfad, &n);
    if (!s_roh) return 0;
    if (re15_rdt_parse(s_roh, n, &s_rdt) < 0) return 0;
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(1);
    re15_esp_fx_reset();
    g_current_room_id = rid;
    g_room_change.pending = 0;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 0; pl->y = 0; pl->z = 0;
    re15_collision_reset_band();
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

/* Kartenschirm fuer (rid, x, z) wie test_r42_karte_lauf.c punkt(), ohne den Raum neu zu laden. Rueckgabe Blatt, -1 = keins. */
static int punkt(unsigned rid, int32_t x, int32_t z, int16_t *mx, int16_t *my)
{
    static const int grenze[6] = { 0x26, 0xc, 0x20, 0xc, 0x15, 0x8 };   /* sltiu je Stage-Init (Seiten-Setzer) */
    unsigned st = (rid >> 12) & 0xF, r = (rid >> 4) & 0xFF;
    const re15_map_zone_t *zc;
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = x; g_actors[RE15_ACTOR_SLOT_PLAYER].z = z;
    re15_map_visited_reset();
    re15_map_visited_mark_at(rid, x, z);
    re15_map_zone_update(rid, x, z);
    zc = re15_map_zone_current();
    if (!zc || zc->room != rid) return -1;
    if (st >= 1 && st <= 6 && (int)r < grenze[st - 1]) re15_inv_map_stage_init((int)st - 1, (int)r);
    memset(&g_inv_screen, 0, sizeof g_inv_screen);
    re15_inv_screen_open();
    g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
    g_inv_screen.map_page = (uint8_t)re15_inv_map_page_shown();
    re15_inv_map_marker(x, z, 0, mx, my);
    return g_inv_screen.map_page;
}

#define N_RASTER 129            /* -32000..32000 in 500er-Schritten */
static signed char s_blatt[N_RASTER][N_RASTER];
static short s_px[N_RASTER][N_RASTER], s_py[N_RASTER][N_RASTER];

/* Spruenge >= 4 px zwischen begehbaren Nachbarlagen desselben Blatts 6..12; *n_lagen = begehbare Lagen mit Karte. */
static int spruenge(unsigned rid, int *n_lagen)
{
    int i, j, n = 0, l = 0;
    if (!betrete(rid)) return -1;
    re15_collision_ensure_band(0);
    for (j = 0; j < N_RASTER; j++)
        for (i = 0; i < N_RASTER; i++) {
            int32_t x = -32000 + 500 * i, z = -32000 + 500 * j, nx = x, nz = z;
            int16_t mx = 0, my = 0;
            int b;
            s_blatt[j][i] = -1;
            re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
            if (nx != x || nz != z) continue;                     /* nicht begehbar */
            g_re15_karte_r42_roh = 1;
            b = punkt(rid, x, z, &mx, &my);
            g_re15_karte_r42_roh = 0;
            if (b < 6 || b > 12) continue;
            {   const re15_map_zone_t *zc = re15_map_zone_current();
                if (!zc || zc->rect == 255 ||
                    !re15_karte_r42_kunst_gemalt((unsigned)b, zc->rect, mx - 1, my - 1)) continue;   /* Leere ausserhalb */
            }
            b = punkt(rid, x, z, &mx, &my);
            if (b < 6 || b > 12) continue;
            s_blatt[j][i] = (signed char)b; s_px[j][i] = mx; s_py[j][i] = my; l++;
        }
    for (j = 0; j < N_RASTER; j++)
        for (i = 0; i < N_RASTER; i++) {
            int k;
            if (s_blatt[j][i] < 0) continue;
            for (k = 0; k < 2; k++) {
                int i2 = i + (k == 0), j2 = j + (k == 1), dx, dy;
                if (i2 >= N_RASTER || j2 >= N_RASTER || s_blatt[j2][i2] != s_blatt[j][i]) continue;
                dx = s_px[j2][i2] - s_px[j][i]; if (dx < 0) dx = -dx;
                dy = s_py[j2][i2] - s_py[j][i]; if (dy < 0) dy = -dy;
                if ((dx > dy ? dx : dy) >= 4) n++;
            }
        }
    *n_lagen = l;
    return n;
}

/* Bezug Runde 5 (02972374c): Raum, Spruenge >= 4 px. Raeume ohne Eintrag: 0. */
static const struct { unsigned short raum; short n; } s_r5[] = {
#include "r42_karte_glatt_r5.inc"
};

int main(int argc, char **argv)
{
    int stage = argc > 1 ? atoi(argv[1]) : 0, i, fail = 0, mess = 0, n_raum = 0;
    unsigned nur = 0, gesehen[256];
    int n_ges = 0;
    const char *e = getenv("R42_GLATT_MESS");
    if (e && *e == '1') mess = 1;
    e = getenv("R42_GLATT_RAUM");
    if (e && *e) nur = (unsigned)strtoul(e, 0, 16);
    if (stage < 2 || stage > 6) { printf("Aufruf: test_r42_karte_glatt <Stage 2..6>\n"); return 2; }
    printf("[glatt] Stage %d: Spruenge >= 4 px auf dem begehbaren 500er-Raster, je Raum nicht mehr als Runde 5\n", stage);
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        int k, n, l, bezug = 0, schon = 0;
        if (!z || z->etage || z->page < 6 || z->page > 12) continue;
        /* Stage 2 nimmt auch die STAGE1-Raeume mit, die auf einem Blatt 6..12 liegen (11A0/11A1: Gast-Zeichnung auf Blatt 6). */
        if (((unsigned)z->room >> 12) != (unsigned)stage && !(stage == 2 && ((unsigned)z->room >> 12) == 1u)) continue;
        if (nur && z->room != nur) continue;
        for (k = 0; k < n_ges; k++) if (gesehen[k] == z->room) schon = 1;
        if (schon || n_ges >= 256) continue;
        gesehen[n_ges++] = z->room;
        n = spruenge(z->room, &l);
        if (n < 0) { printf("  %04X: Raum nicht ladbar\n", z->room); continue; }
        n_raum++;
        for (k = 0; k < (int)(sizeof s_r5 / sizeof s_r5[0]); k++) if (s_r5[k].raum == z->room) bezug = s_r5[k].n;
        if (mess) { printf("    { 0x%04X, %d },   /* begehbar %d */\n", z->room, n, l); continue; }
        printf("  %s: %04X Lagen %d, Spruenge %d (Runde 5: %d)\n", n <= bezug ? "PASS" : "FAIL", z->room, l, n, bezug);
        if (n > bezug) fail = 1;
    }
    if (n_raum == 0) { printf("  FAIL: kein Raum gemessen\n"); fail = 1; }
    if (s_roh) free(s_roh);
    printf(fail ? "\nFEHLER\n" : "\nOK\n");
    return fail;
}
