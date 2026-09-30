/* probe_r34n_b_cursor.c — RIEGEL Runde 34 Nacht / Spur B: Hebetisch-Cursor in Irons' Buero.
 *
 * Auftrag (analysis/befunde_runde34_nacht/AUFTRAG.md, zweiter Punkt): Cursor statt Direktoeffnung,
 * "Nothing happened" daneben, Klick + normaler Ablauf auf der Kuppel. Dossier B_hebetisch.md §6.1/§9,
 * Konstanten include/re15_hebetisch_cursor.h. Jede Pruefung laeuft in ROOM1150.RDT UND ROOM1151.RDT.
 *
 * Bildschleife in der Reihenfolge von main.c: VM-Takt (liest die Pad-Woerter des VORbilds, main.c
 * scd_vm_tick vor re15_game_step), Text-Takt (re15_msg_tick), dann veroeffentlicht der "Spielschritt"
 * die neuen virtuellen Woerter (re15_pad_virtual_word; unter RE15_PAUSE_PAD auf 0xf000 maskiert wie
 * game_step_common.c) und feuert ggf. die GENERIC-Ausgabe (scd_event_fire + re15_hebetisch_cursor_aktion).
 *
 * Teil-Riegel (Argument = Name, ohne Argument alle):
 *   halt      R1  Spielerweg: sub04 steht nach 10 VM-Takten VOR dem For (1150 @0x0FC0, 1151 @0x0F9E),
 *                 Plattform -305, angefordert Cut 4, gemerkt die Raumkamera, Deckel zu; 110 Bilder
 *                 spaeter unveraendert; Cursor sichtbar am Start, Heisspunkt (160,119)
 *   kuppel    R2  Engine-Projektion der Kuppel (Deckel Prop 1/2 + Podest aus Prop 0) unter Cut 4 bei
 *                 Plattform @0x0FB4 = die Huelle im Modul (+-1 px); Start nicht drin, Kuppelmitte drin,
 *                 Punkte knapp ausserhalb nicht drin
 *   ablauf    R3  Fehldruck -> "Nothing happened." (Id 20, Bytes, Maske 0xffff0000, kein Klick, Halt
 *                 bleibt), Text zu, D-Pad (18x RIGHT+DOWN, 4x DOWN) -> Heisspunkt (208,177) in der
 *                 Kuppel, Druck -> Klick (RE2 0x0A) und im SELBEN Takt Deckel +-10, nach 15 Takten
 *                 +-150; Rest von sub04 unveraendert: Sicherung/Granate in der Ruhe oben (y -1205),
 *                 beide Yes -> Flags (9,53)/(9,56)
 *   cut_old   R4  nach dem kompletten Durchlauf mit Cursor: Cut_old stellt die Raumkamera her (nicht 4)
 *   harness   R5  scd_event_fire(4) OHNE re15_hebetisch_cursor_aktion (RE15_FIRE_AOT, alte Riegel):
 *                 kein Halt, die Deckel bewegen sich im 11. Takt
 *   bytes     R6  eingebackenes MD1/TIM/Licht == ROOM11F0.RDT @0x001928 / @0x018DAC / @0x000718
 *   abbruch   R7  CROSS im Cursor -> Aufraeumbytes @0x109A (1151 @0x1078): Thread zu Ende, Plattform
 *                 -20224, Spieler/KI frei, Raumkamera, Deckel zu, Tisch-Items unberuehrt, kein Text,
 *                 kein Klick; erneute Aktion bringt den Cursor wieder
 *   kreuztext R8  "Nothing happened." mit CROSS schliessen bricht NICHT ab (Gegenpruefung (c)7)
 *
 * Rueckgabe 0 = alles bestanden, sonst die Nummer der ersten gerissenen Pruefung.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_camera.h"
#include "re15_math.h"
#include "re15_msg.h"
#include "re15_engine.h"
#include "re15_player.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_sicherung.h"
#include "re15_granate.h"
#include "re15_hebetisch.h"
#include "re15_hebetisch_cursor.h"
#include "re15_audio.h"          /* RE15_PANEL_SE_KLICK */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;
extern int g_test_panel_se_last;
extern int g_test_panel_se_count;

static int fehler = 0, erste = 0;
static void pruefe(int nr, const char *was, int ok_)
{
    printf("   [%s] %d. %s\n", ok_ ? "OK " : "FEHL", nr, was);
    if (!ok_) { fehler++; if (!erste) erste = nr; }
}

static uint8_t *datei(const char *rel, size_t *n)
{
    char p[700];
    snprintf(p, sizeof p, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), rel);
    FILE *f = fopen(p, "rb");
    if (!f) { fprintf(stderr, "nicht lesbar: %s\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (!b || fread(b, 1, (size_t)s, f) != (size_t)s) { fclose(f); free(b); return NULL; }
    fclose(f); *n = (size_t)s; return b;
}

/* ------------------------------------------------------------------ Raum + Bildschleife ---- */
static re15_rdt_t s_rdt;
static uint8_t   *s_buf;
static uint16_t   s_rid;
static long       s_halt_off;      /* 0x0FC0 / 0x0F9E */

static int raum_laden(uint16_t rid)
{
    size_t n = 0;
    free(s_buf);
    s_buf = datei(rid == 0x1150 ? "STAGE1/ROOM1150.RDT" : "STAGE1/ROOM1151.RDT", &n);
    if (!s_buf) return 77;
    memset(&s_rdt, 0, sizeof s_rdt);
    if (re15_rdt_parse(s_buf, n, &s_rdt) != 0) return 1;
    s_rid = rid;
    s_halt_off = rid == 0x1150 ? 0x0FC0 : 0x0F9E;
    return 0;
}

static void raum_frisch(void)
{
    re15_game_state_init();
    re15_inv_init();
    scd_vm_init(); re15_actor_init();
    g_current_room_id = s_rid;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -22250; pl->y = 0; pl->z = -18500; pl->rot_y = 0; pl->state = 1;
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size); }
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, -22250, -18500, 0);      /* ruft re15_hebetisch_cursor_install */
    g_scd_pad_edge = g_scd_pad_held = 0;
    g_test_panel_se_count = 0; g_test_panel_se_last = -1;
    g_re15_hebetisch_klick_zaehler = g_re15_hebetisch_text_zaehler = g_re15_hebetisch_abbruch_zaehler = 0;
}

static int slot_von_obj(uint8_t o)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == o) return k;
    return -1;
}
static int prop_y(uint8_t o) { int k = slot_von_obj(o); return k >= 0 ? (int)g_scd.props[k].y : 0; }
static int prop_z(uint8_t o) { int k = slot_von_obj(o); return k >= 0 ? (int)g_scd.props[k].z : 0; }

static int sub04_slot(void)
{
    const uint8_t *von = s_rdt.raw + 0x0F96 - (s_rid == 0x1151 ? 0x22 : 0);
    const uint8_t *bis = von + 0x120;
    for (int t = 0; t < SCD_THREAD_COUNT; t++)
        if (g_scd.threads[t].active && !g_scd.threads[t].kill_pending &&
            g_scd.threads[t].pc >= von && g_scd.threads[t].pc < bis) return t;
    return -1;
}

/* Ein Spielbild: VM, Text, dann die Pad-Woerter des naechsten Bilds veroeffentlichen. */
static void bild(uint16_t roh_gehalten, uint16_t roh_flanke)
{
    scd_vm_tick();
    re15_msg_tick(NULL, NULL, NULL);
    uint16_t held = re15_pad_virtual_word(roh_gehalten);
    uint16_t edge = re15_pad_virtual_word(roh_flanke);
    if (g_re15_pauseflags & RE15_PAUSE_PAD) { held &= 0xf000u; edge &= 0xf000u; }
    g_scd_pad_held = held;
    g_scd_pad_edge = edge;
}

/* Spielerweg: die GENERIC-Ausgabe von game_step_common.c (Aktion am Tisch, Slot 1 -> Ereignis 4). */
static int tisch_aktion(void)
{
    int slot = scd_event_fire(RE15_HC_EREIGNIS);
    re15_hebetisch_cursor_aktion(RE15_HC_EREIGNIS, slot);
    return slot;
}

static int bis_aktiv(int max)
{
    for (int f = 0; f < max; f++) {
        bild(0, 0);
        if (re15_hebetisch_cursor_zustand() == RE15_HC_AKTIV) return f;
    }
    return -1;
}

static void druecken(uint16_t roh)       /* Flanke im naechsten VM-Takt sichtbar */
{
    bild(roh, roh);
    bild(0, 0);
}

static int text_schliessen(uint16_t roh, int max)
{
    for (int f = 0; f < max && g_scd.message_fsm != 4; f++) bild(0, 0);
    if (g_scd.message_fsm != 4) return 0;
    bild(roh, roh);                  /* die Flanke liegt an ...                                  */
    bild(0, 0);                      /* ... VM steht noch (Text offen), der Text-Takt schliesst */
    return !g_scd.message_active;
}

/* ------------------------------------------------------------------ R1 halt ---------------- */
static void r1_halt(void)
{
    printf("\n== %04X R1 halt ==\n", s_rid);
    raum_frisch();
    const unsigned cam_vorher = g_scd.cam_id;
    int slot = tisch_aktion();
    int f = bis_aktiv(40);
    const scd_thread_t *t = slot >= 0 ? &g_scd.threads[slot] : NULL;
    long pc = t && t->active ? (long)(t->pc - s_rdt.raw) : -1;
    printf("   aktiv in Bild %d, sub04-Slot %d PC @0x%04lX, Plattform y %d, cam %u (gemerkt %u), Deckel z %d/%d\n",
           f, slot, pc, prop_y(0), (unsigned)g_scd.cam_id, (unsigned)g_scd.cam_id_prev, prop_z(1), prop_z(2));
    pruefe(1, "Halt im 11. VM-Takt nach dem Tischdruck, PC = For 15 (1150 @0x0FC0 / 1151 @0x0F9E)",
           f == 10 && pc == s_halt_off);
    pruefe(2, "Cursor-Zustand: Plattform y -305 (@0x0FB4), Cut 4 angefordert, Raumkamera gemerkt, Deckel 0/0",
           prop_y(0) == -305 && g_scd.cam_id == 4 && g_scd.cam_id_prev == cam_vorher &&
           prop_z(1) == 0 && prop_z(2) == 0);
    for (int k = 0; k < 110; k++) bild(0, 0);
    int32_t x = 0, z = 0;
    int sicht = re15_hebetisch_cursor_sicht(&x, &z);
    int sx = 0, sy = 0;
    re15_hebetisch_cursor_heisspunkt(x, z, &sx, &sy);
    long pc2 = t && t->active ? (long)(t->pc - s_rdt.raw) : -1;
    pruefe(3, "110 Bilder spaeter: PC, Deckel, Plattform unveraendert, Cursor sichtbar am Start (160,119)",
           pc2 == s_halt_off && prop_z(1) == 0 && prop_z(2) == 0 && prop_y(0) == -305 &&
           sicht && x == RE15_HC_START_X && z == RE15_HC_START_Z && sx == 160 && sy == 119);
}

/* ------------------------------------------------------------------ R2 kuppel -------------- */
typedef struct { int x, y; } pkt_t;
static int kreuzp(pkt_t o, pkt_t a, pkt_t b) { return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x); }
static int huelle(pkt_t *p, int n, pkt_t *h)
{
    for (int i = 1; i < n; i++) { pkt_t k = p[i]; int j = i - 1;
        while (j >= 0 && (p[j].x > k.x || (p[j].x == k.x && p[j].y > k.y))) { p[j + 1] = p[j]; j--; }
        p[j + 1] = k; }
    int m = 0;
    for (int i = 0; i < n; i++) { while (m >= 2 && kreuzp(h[m - 2], h[m - 1], p[i]) <= 0) m--; h[m++] = p[i]; }
    for (int i = n - 2, t = m + 1; i >= 0; i--) { while (m >= t && kreuzp(h[m - 2], h[m - 1], p[i]) <= 0) m--; h[m++] = p[i]; }
    return m - 1;
}
static int im_podest(int x, int y, int z) { return y >= -1036 && y <= -886 && x >= -490 && x <= -70 && z >= 870 && z <= 1650; }

static void r2_kuppel(void)
{
    printf("\n== %04X R2 kuppel ==\n", s_rid);
    re15_camera_view_t v;
    re15_camera_build_view(&s_rdt.cuts[RE15_HC_CUT], &v);                   /* Cut 4 @0x00E0 */
    static const int32_t R2048[9] = { -4096, 0, 0, 0, 4096, 0, 0, 0, -4096 };  /* rot_y 0x800 @0x0E00 */
    static const int32_t P[3] = { -20700, -305, -17460 };                    /* Pos_set @0x0FB4 */
    int32_t cr[9], ct[3];
    re15_camera_compose_view_bone(&v, R2048, P, cr, ct);
    static pkt_t pts[4096]; int n = 0;
    for (int op = 0; op <= 2; op++) {                                        /* Podest 0, Deckel 1/2 */
        re15_md1_t m;
        if (re15_md1_parse(s_rdt.prop_md1[op], s_rdt.prop_md1_size[op], &m) != 0) continue;
        for (int mi = 0; mi < m.mesh_count; mi++) {
            const re15_md1_mesh_t *me = &m.meshes[mi];
            for (int k = 0; k < me->triangle_count; k++) {
                const re15_md1_vertex_t *a[3] = { &me->tri_vertices[me->triangles[k].v0],
                    &me->tri_vertices[me->triangles[k].v1], &me->tri_vertices[me->triangles[k].v2] };
                int ok = 1; for (int j = 0; j < 3; j++) if (op == 0 && !im_podest(a[j]->x, a[j]->y, a[j]->z)) ok = 0;
                for (int j = 0; ok && j < 3 && n < 4096; j++, n++)
                    re15_hebetisch_cursor_projiziere(cr, ct, v.fov_screen_dist, a[j]->x, a[j]->y, a[j]->z, &pts[n].x, &pts[n].y, NULL);
            }
            for (int k = 0; k < me->quad_count; k++) {
                const re15_md1_vertex_t *a[4] = { &me->quad_vertices[me->quads[k].v0], &me->quad_vertices[me->quads[k].v1],
                    &me->quad_vertices[me->quads[k].v2], &me->quad_vertices[me->quads[k].v3] };
                int ok = 1; for (int j = 0; j < 4; j++) if (op == 0 && !im_podest(a[j]->x, a[j]->y, a[j]->z)) ok = 0;
                for (int j = 0; ok && j < 4 && n < 4096; j++, n++)
                    re15_hebetisch_cursor_projiziere(cr, ct, v.fov_screen_dist, a[j]->x, a[j]->y, a[j]->z, &pts[n].x, &pts[n].y, NULL);
            }
        }
    }
    static pkt_t h[4200];
    int hn = huelle(pts, n, h);
    const int16_t *k = re15_hebetisch_cursor_kuppel();
    int maxd = 0;
    for (int i = 0; i < RE15_HC_KUPPEL_N; i++) {                             /* Modul-Ecke -> Engine */
        int best = 1 << 30;
        for (int j = 0; j < hn; j++) { int dx = k[2*i] - h[j].x, dy = k[2*i+1] - h[j].y; if (dx*dx + dy*dy < best) best = dx*dx + dy*dy; }
        if (best > maxd) maxd = best;
    }
    for (int j = 0; j < hn; j++) {                                           /* Engine-Ecke -> Modul */
        int best = 1 << 30;
        for (int i = 0; i < RE15_HC_KUPPEL_N; i++) { int dx = k[2*i] - h[j].x, dy = k[2*i+1] - h[j].y; if (dx*dx + dy*dy < best) best = dx*dx + dy*dy; }
        if (best > maxd) maxd = best;
    }
    printf("   %d Punkte, Engine-Huelle %d Ecken, groesster Eckabstand %d px^2\n", n, hn, maxd);
    pruefe(4, "Huelle im Modul = Engine-Projektion der ausgelieferten Kuppel (+-1 px, beide Richtungen)",
           hn == RE15_HC_KUPPEL_N && maxd <= 2);
    int sx = 0, sy = 0;
    re15_hebetisch_cursor_heisspunkt(RE15_HC_START_X + 18 * RE15_HC_SCHRITT,
                                     RE15_HC_START_Z - 22 * RE15_HC_SCHRITT, &sx, &sy);
    printf("   Heisspunkt nach 18x RIGHT + 22x DOWN: (%d,%d)\n", sx, sy);
    pruefe(5, "Start (160,119) NICHT in der Kuppel; Mitte (209,176) und (208,177) drin; (150,180) (268,180) "
              "(208,147) (208,212) draussen",
           !re15_hebetisch_cursor_in_kuppel(160, 119) && re15_hebetisch_cursor_in_kuppel(209, 176) &&
           sx == 208 && sy == 177 && re15_hebetisch_cursor_in_kuppel(sx, sy) &&
           !re15_hebetisch_cursor_in_kuppel(150, 180) && !re15_hebetisch_cursor_in_kuppel(268, 180) &&
           !re15_hebetisch_cursor_in_kuppel(208, 147) && !re15_hebetisch_cursor_in_kuppel(208, 212));
}

/* ------------------------------------------------------------------ R3 ablauf + R4 cut_old - */
static int menge(uint8_t id)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) n += g_inv.slots[i].qty;
    return n;
}

static void r3_ablauf(int mit_r4)
{
    printf("\n== %04X R3 ablauf%s ==\n", s_rid, mit_r4 ? " + R4 cut_old" : "");
    raum_frisch();
    const unsigned cam_vorher = g_scd.cam_id;
    int slot = tisch_aktion();
    bis_aktiv(40);
    const scd_thread_t *t = &g_scd.threads[slot];

    /* Fehldruck am Start */
    druecken(RE15_PAD_BIT_SQUARE);
    int len = 0;
    const unsigned char *raw = re15_msg_get_raw(RE15_HC_TEXT_ID, &len);
    int tlen = 0;
    const uint8_t *soll = re15_hebetisch_cursor_text(&tlen);
    printf("   Fehldruck: Texte %u, aktiv %d Id %d, Pause 0x%08X, Klicks %d, PC @0x%04lX\n",
           g_re15_hebetisch_text_zaehler, (int)g_scd.message_active, (int)g_scd.message_id,
           (unsigned)g_re15_pauseflags, g_test_panel_se_count, (long)(t->pc - s_rdt.raw));
    static const uint8_t k_bytes[] = { 0x04,0x02, 0x2a,0x4b,0x50,0x44,0x45,0x4a,0x43, 0x00,0x44,0x3d,0x4c,
                                       0x4c,0x41,0x4a,0x41,0x40, 0x57, 0x01,0x00 };
    pruefe(6, "Fehldruck: \"Nothing happened.\" (Id 20, Bytes wie Kopf), Maske 0xffff0000, KEIN Klick, Halt bleibt",
           g_re15_hebetisch_text_zaehler == 1 && g_scd.message_active && g_scd.message_id == RE15_HC_TEXT_ID &&
           raw && len == (int)sizeof k_bytes && memcmp(raw, k_bytes, sizeof k_bytes) == 0 &&
           tlen == (int)sizeof k_bytes && memcmp(soll, k_bytes, sizeof k_bytes) == 0 &&
           (g_re15_pauseflags & 0xFFFF0000u) == 0xFFFF0000u && g_test_panel_se_count == 0 &&
           t->pc - s_rdt.raw == s_halt_off && re15_hebetisch_cursor_zustand() == RE15_HC_AKTIV);
    int zu = text_schliessen(RE15_PAD_BIT_SQUARE, 200);
    for (int f = 0; f < 5; f++) bild(0, 0);
    pruefe(7, "Text mit SQUARE zu: kein zweiter Text, Cursor weiter aktiv, Spieler/KI weiter angehalten (sub04)",
           zu && g_re15_hebetisch_text_zaehler == 1 && re15_hebetisch_cursor_zustand() == RE15_HC_AKTIV &&
           (g_re15_pauseflags & RE15_PAUSE_PLAYER) && (g_re15_pauseflags & RE15_PAUSE_AI));

    /* D-Pad auf die Kuppel: 18x RIGHT+DOWN, 4x DOWN (vier unabhaengige Abfragen wie sub01) */
    for (int f = 0; f < 18; f++) bild(RE15_PAD_BIT_RIGHT | RE15_PAD_BIT_DOWN, f == 0 ? (RE15_PAD_BIT_RIGHT | RE15_PAD_BIT_DOWN) : 0);
    for (int f = 0; f < 4; f++) bild(RE15_PAD_BIT_DOWN, 0);
    bild(0, 0);
    int32_t x = 0, z = 0;
    re15_hebetisch_cursor_sicht(&x, &z);
    int sx = 0, sy = 0;
    re15_hebetisch_cursor_heisspunkt(x, z, &sx, &sy);
    printf("   nach dem D-Pad: (%d,%d) -> Heisspunkt (%d,%d)\n", (int)x, (int)z, sx, sy);
    pruefe(8, "D-Pad: 18x RIGHT+DOWN, 4x DOWN = je 200 (sub02..05) -> (-15954,18284), Heisspunkt (208,177)",
           x == RE15_HC_START_X + 18 * 200 && z == RE15_HC_START_Z - 22 * 200 && sx == 208 && sy == 177);

    /* Druck auf der Kuppel */
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);   /* Flanke liegt an                            */
    bild(0, 0);                                       /* VM: Klick + For im selben Takt             */
    printf("   Kuppeldruck: Klicks %d (letzter 0x%02X), Zustand %d, Deckel z %d/%d\n", g_test_panel_se_count,
           g_test_panel_se_last, re15_hebetisch_cursor_zustand(), prop_z(1), prop_z(2));
    pruefe(9, "Kuppeldruck: RE2-Klick 0x0A (einmal), Cursor aus, For im selben Takt (Deckel +10/-10)",
           g_test_panel_se_count == 1 && g_test_panel_se_last == RE15_PANEL_SE_KLICK &&
           g_re15_hebetisch_klick_zaehler == 1 && re15_hebetisch_cursor_zustand() == RE15_HC_AUS &&
           prop_z(1) == 10 && prop_z(2) == -10);
    for (int f = 0; f < 14; f++) bild(0, 0);
    pruefe(10, "nach 15 Takten Deckel offen (+150/-150, For 15 @0x0FC0 unveraendert)",
           prop_z(1) == 150 && prop_z(2) == -150);

    /* Rest von sub04 wie unit_r31_hebetisch: Aufnahme in der Ruhe oben, Yes/Yes */
    int f_si = -1, f_gr = -1, y_si = 0, y_gr = 0, ende = -1;
    for (int f = 0; f < 1400; f++) {
        if (!re15_item_modal_active()) scd_vm_tick();                          /* main.c Freeze-Zweig */
        re15_msg_tick(NULL, NULL, NULL);
        if (re15_sicherung_tick()) { f_si = f; y_si = prop_y(0); }
        if (re15_granate_tick())   { f_gr = f; y_gr = prop_y(0); }
        if (re15_item_modal_active()) {
            uint16_t edge = 0; uint8_t typ = 0; int wahl = 0;
            int art = re15_item_modal_prompt(&typ, &wahl);
            if (re15_item_modal_prompt_ready() && art >= 1) edge = 0x4000;         /* Yes */
            re15_item_modal_tick(edge, edge);
        }
        if (sub04_slot() < 0 && f_gr >= 0) { ende = f; break; }
    }
    for (int f = 0; f < 3; f++) bild(0, 0);
    printf("   Sicherung Bild %d (y %d), Granate Bild %d (y %d), sub04 zu Ende Bild %d, Plattform y %d, cam %u\n",
           f_si, y_si, f_gr, y_gr, ende, prop_y(0), (unsigned)g_scd.cam_id);
    pruefe(11, "Sicherung, dann Granate in der Ruhe oben (y -1205); Yes/Yes -> Flags (9,53)/(9,56), Inventar",
           f_si >= 0 && f_gr > f_si && y_si == -1205 && y_gr == -1205 &&
           re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) && re15_game_flag_get(9, RE15_GRANATE_TAKEN_BIT) &&
           menge(RE15_SICHERUNG_ITEM) == 1 && menge(RE15_GRANATE_ITEM) == RE15_GRANATE_MENGE);
    if (mit_r4)
        pruefe(12, "R4: sub04 zu Ende, Plattform geparkt (-20224 @0x109E), Cut_old @0x10B2 = Raumkamera (nicht 4)",
               ende >= 0 && prop_y(0) == -20224 && g_scd.cam_id == cam_vorher && g_scd.cam_id != 4 &&
               !(g_re15_pauseflags & RE15_PAUSE_PLAYER));
}

/* ------------------------------------------------------------------ R5 harness ------------- */
static void r5_harness(void)
{
    printf("\n== %04X R5 harness ==\n", s_rid);
    raum_frisch();
    scd_event_fire(RE15_HC_EREIGNIS);                  /* wie re15_aot_fire_slot / RE15_FIRE_AOT */
    int nie_aktiv = 1;
    for (int f = 0; f < 11; f++) { bild(0, 0); if (re15_hebetisch_cursor_zustand() != RE15_HC_AUS) nie_aktiv = 0; }
    printf("   nach 11 Takten: Zustand %d, Deckel z %d/%d\n", re15_hebetisch_cursor_zustand(), prop_z(1), prop_z(2));
    pruefe(13, "Harness-Weg ohne Aktion: kein Cursor, die Deckel laufen im 11. Takt wie bisher (+10/-10)",
           nie_aktiv && prop_z(1) == 10 && prop_z(2) == -10);
}

/* ------------------------------------------------------------------ R6 bytes --------------- */
static void r6_bytes(void)
{
    printf("\n== R6 bytes ==\n");
    size_t n = 0;
    uint8_t *f = datei("STAGE1/ROOM11F0.RDT", &n);
    int md = 0, tm = 0, li = 0;
    const uint8_t *m = re15_hebetisch_cursor_md1_bytes(&md);
    const uint8_t *t = re15_hebetisch_cursor_tim_bytes(&tm);
    const uint8_t *l = re15_hebetisch_cursor_licht_bytes(&li);
    printf("   MD1 %d B, TIM %d B, Licht %d B\n", md, tm, li);
    pruefe(14, "MD1 == ROOM11F0.RDT @0x001928 (5556 B), TIM == @0x018DAC (33312 B), Licht == @0x000718 (40 B)",
           f && m && t && l && md == 5556 && tm == 33312 && li == 40 && n >= 0x018DAC + 33312u &&
           memcmp(m, f + 0x001928, 5556) == 0 && memcmp(t, f + 0x018DAC, 33312) == 0 &&
           memcmp(l, f + 0x000718, 40) == 0);
    free(f);
}

/* ------------------------------------------------------------------ R7 abbruch ------------- */
static void r7_abbruch(void)
{
    printf("\n== %04X R7 abbruch ==\n", s_rid);
    raum_frisch();
    const unsigned cam_vorher = g_scd.cam_id;
    tisch_aktion();
    bis_aktiv(40);
    int si = slot_von_obj(RE15_SICHERUNG_OBJ_ID), gr = slot_von_obj(RE15_GRANATE_OBJ_ID);
    bild(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    bild(0, 0);
    printf("   nach CROSS: Abbrueche %u, Zustand %d, sub04 %d, Plattform y %d, cam %u, Pause 0x%08X, Deckel %d/%d\n",
           g_re15_hebetisch_abbruch_zaehler, re15_hebetisch_cursor_zustand(), sub04_slot(), prop_y(0),
           (unsigned)g_scd.cam_id, (unsigned)g_re15_pauseflags, prop_z(1), prop_z(2));
    pruefe(15, "CROSS: Aufraeumbytes gelaufen (sub04 zu Ende, Plattform -20224, Spieler+KI frei, Raumkamera, "
               "Deckel zu), kein Text, kein Klick",
           g_re15_hebetisch_abbruch_zaehler == 1 && re15_hebetisch_cursor_zustand() == RE15_HC_AUS &&
           sub04_slot() < 0 && prop_y(0) == -20224 && g_scd.cam_id == cam_vorher && g_scd.cam_id != 4 &&
           !(g_re15_pauseflags & RE15_PAUSE_PLAYER) && !(g_re15_pauseflags & RE15_PAUSE_AI) &&
           prop_z(1) == 0 && prop_z(2) == 0 && g_re15_hebetisch_text_zaehler == 0 &&
           g_test_panel_se_count == 0);
    pruefe(16, "Tisch-Items unberuehrt (Props da, Flags (9,53)/(9,56) = 0)",
           si >= 0 && gr >= 0 && g_scd.props[si].active && g_scd.props[gr].active &&
           !re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) && !re15_game_flag_get(9, RE15_GRANATE_TAKEN_BIT));
    tisch_aktion();
    int f = bis_aktiv(40);
    pruefe(17, "erneute Aktion am Tisch bringt den Cursor wieder (Halt nach 11 Takten)", f == 10);
}

/* ------------------------------------------------------------------ R8 kreuztext ----------- */
static void r8_kreuztext(void)
{
    printf("\n== %04X R8 kreuztext ==\n", s_rid);
    raum_frisch();
    tisch_aktion();
    bis_aktiv(40);
    druecken(RE15_PAD_BIT_SQUARE);
    int zu = text_schliessen(RE15_PAD_BIT_CROSS, 200);
    for (int f = 0; f < 10; f++) bild(0, 0);
    printf("   Text zu %d, Abbrueche %u, Zustand %d, sub04 %d\n", zu, g_re15_hebetisch_abbruch_zaehler,
           re15_hebetisch_cursor_zustand(), sub04_slot());
    pruefe(18, "\"Nothing happened.\" mit CROSS geschlossen: KEIN Abbruch, Cursor bleibt, sub04 steht am Halt",
           zu && g_re15_hebetisch_abbruch_zaehler == 0 && re15_hebetisch_cursor_zustand() == RE15_HC_AKTIV &&
           sub04_slot() >= 0 && g_scd.threads[sub04_slot()].pc - s_rdt.raw == s_halt_off);
}

/* ------------------------------------------------------------------ main ------------------- */
static int will(int argc, char **argv, const char *name)
{
    if (argc < 2) return 1;
    for (int i = 1; i < argc; i++) if (strcmp(argv[i], name) == 0) return 1;
    return 0;
}

int main(int argc, char **argv)
{
    static const uint16_t raeume[2] = { 0x1150, 0x1151 };
    for (int r = 0; r < 2; r++) {
        int rc = raum_laden(raeume[r]);
        if (rc == 77) return 77;
        if (rc) { printf("RDT %04X nicht lesbar\n", raeume[r]); return 99; }
        if (will(argc, argv, "halt"))      r1_halt();
        if (will(argc, argv, "kuppel"))    r2_kuppel();
        if (will(argc, argv, "ablauf"))    r3_ablauf(0);
        if (will(argc, argv, "cut_old"))   r3_ablauf(1);
        if (will(argc, argv, "harness"))   r5_harness();
        if (will(argc, argv, "abbruch"))   r7_abbruch();
        if (will(argc, argv, "kreuztext")) r8_kreuztext();
    }
    if (will(argc, argv, "bytes")) r6_bytes();
    printf("\n%s — %d Pruefung(en) gerissen\n", fehler ? "FEHLGESCHLAGEN" : "ALLES BESTANDEN", fehler);
    return fehler ? erste : 0;
}
