/* Karte: Raeume ohne Karten-Rechteck werden aus ihrer KOLLISIONS-BOX gezeichnet.
 *
 * Nutzer 2026-09-01: "Wo die Kartenlage fehlt oder unklar ist, die Collision-Box des
 * Raums nutzen."
 *
 * 33 der 100 Karten-Raeume hatten gar kein Rechteck, insgesamt 40,7 % der begehbaren
 * Flaeche. Der Spieler-Marker fand dort keine Zone - das ist der Sprung, den der Nutzer
 * auf 2F sah. ROOM10D0, der 2F-Flur, ist der prominenteste Fall: sein begehbarer
 * Bereich misst im ausgelieferten Massstab (@0x800768b0, STAGE1-Median 459/464
 * Welteinheiten je Pixel) 70 x 89 px, das groesste Rechteck der Seite 3 ist 72 x 64.
 *
 * ⛔ PORT-ERGAENZUNG: das Original zeichnet diese Raeume nicht. Die Zeichnung traegt
 * rect == 255 und einen Index in s_map_synth. */
#include <stdio.h>
#include "re15_inv_screen.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_collision.h"

extern unsigned g_current_room_id;
extern re15_inv_screen_t g_inv_screen;

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

int main(void)
{
    /* ⛔ DIESER TEST PRUEFT DEN RUECKFALL, NICHT DEN AUSLIEFERUNGSSTAND.
     * Seit 2026-09-04 zeichnet der Port die ORIGINAL-KUNST (RE15_KUNST, Nutzer-
     * Entscheidung); die Schema-Zeichnung aus der Kollisions-Box ist damit inaktiv -
     * gemessen 0 von 192 Zonen tragen noch eine. Der Pfad lebt weiter unter
     * RE15_KUNST=0 und wird dort geprueft; hier ist "keine Schema-Zeichnung
     * vorhanden" das ERWARTETE Ergebnis und kein Fehler. */
    {
        int i, n = 0;
        for (i = 0; i < re15_map_zone_count(); i++) {
            const re15_map_zone_t *z = re15_map_zone_by_index(i);
            if (z && z->synth) { n++; break; }
        }
        if (n == 0) {
            printf("=== Karte: Schema-Zeichnungen ===\n");
            printf("  UEBERSPRUNGEN: der Auslieferungsstand zeichnet die Original-Kunst,"
                   " es gibt keine Schema-Zonen (RE15_KUNST=0 baut sie)\n");
            return 0;
        }
    }

    const re15_map_zone_t *zn;
    int x, y, w, h, erste, n, i, n_synth = 0, n_ueber = 0;

    printf("=== Karte: Schema-Zeichnung aus der Kollisions-Box ===\n");

    /* (1) ROOM10D0 - der 2F-Flur - hat eine Zeichnung. */
    zn = re15_map_zone_at(0x10D0, 0, 0);
    if (!zn) { /* ueber die Weltmitte seiner Zone suchen */
        zn = re15_map_zone_at(0x10D0, -5000, 10000);
    }
    CHECK("ROOM10D0 hat ueberhaupt eine Zone", zn != 0);
    if (zn) {
        CHECK("ROOM10D0 traegt kein Karten-Rechteck des Originals (rect == 255)",
              zn->rect == 255);
        CHECK("ROOM10D0 hat eine Schema-Zeichnung",
              re15_map_zone_synth(zn, &x, &y, &w, &h, &erste, &n) && n > 0);
        printf("  [ROOM10D0] Kasten (%d,%d) %dx%d, %d Zellen\n", x, y, w, h, n);
        CHECK("ihr Kasten liegt im Kartenfeld", x >= 90 && y >= 50 &&
                                                x + w <= 240 && y + h <= 200);
    }

    /* (2) Keine Schema-Zeichnung darf ein GEMALTES Rechteck ueberdecken. Die Kunst des
     *     Originals ist nicht massstabsgetreu zur Kollision; anker-genau gesetzt lag
     *     ROOM10D0 ueber vier Nachbarn zugleich. */
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *a = re15_map_zone_by_index(i);
        int ax, ay, aw, ah, j;
        if (!a || !re15_map_zone_kasten(a, &ax, &ay, &aw, &ah)) continue;
        n_synth++;
        for (j = 0; j < re15_map_zone_count(); j++) {
            const re15_map_zone_t *b = re15_map_zone_by_index(j);
            int bx, by, bw, bh;
            if (!b || b->page != a->page || b->rect == 255) continue;
            if (!re15_map_rect_geometry(b->page, b->rect, &bx, &by, &bw, &bh)) continue;
            if (ax < bx + bw && bx < ax + aw && ay < by + bh && by < ay + ah) {
                n_ueber++;
                printf("  [Ueberlappung] ROOM%04X Schema auf ROOM%04X Rect %d\n",
                       a->room, b->room, b->rect);
                j = re15_map_zone_count();
            }
        }
    }
    printf("  [Bestand] %d Schema-Zeichnungen, %d ueberlappen gemalte Rechtecke\n",
           n_synth, n_ueber);
    CHECK("es gibt Schema-Zeichnungen", n_synth > 0);
    CHECK("keine davon ueberdeckt ein gemaltes Rechteck", n_ueber == 0);

    /* (3) Die gezeichneten Ops muessen OP_FILL sein und in ihrem Kasten liegen.
     *     ⛔ Der erste Wurf nahm RE15_INV_OP_LINE - das deutet (w,h) als
     *     ENDPUNKT, nicht als Groesse (inv_render_pc.c:547-566). Jede Zelle wurde
     *     dadurch zu einem Strich von ihrer Ecke zu (w,h), quer ueber den Schirm.
     *     Der Abzug des 2F-Blattes zeigte lange senkrechte Streifen; die TABELLE war
     *     dabei einwandfrei. Ein Pin auf die Tabelle allein haette das nicht gefangen. */
    {
        static re15_inv_op_t ops[768];
        int nops, k, a3, n_fill = 0, n_falsch = 0, n_raus = 0;
        re15_map_visited_reset();
        re15_map_debug_reveal_page(3);
        re15_inv_map_stage_init(0, 13);
        re15_inv_screen_open();
        g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
        g_inv_screen.map_page = 3;
        nops = re15_inv_screen_build(&g_inv_screen, ops, 768);
        /* ⛔ GENAU die Zellen der Tabelle suchen, nicht alles im Kasten: auf Seite 0
         * der Op-Liste liegen auch Chrome und Marken, die zufaellig hineinfallen -
         * ein erster Anlauf zaehlte 8 solcher Fremd-Ops als Fehler. */
        for (a3 = 0; a3 < re15_map_zone_count(); a3++) {
            const re15_map_zone_t *zz = re15_map_zone_by_index(a3);
            int bx, by, bw, bh, erste2, n2, c;
            if (!zz || zz->page != 3) continue;
            if (!re15_map_zone_synth(zz, &bx, &by, &bw, &bh, &erste2, &n2)) continue;
            for (c = 0; c < n2; c++) {
                int cx, cy, cw, ch, gefunden = 0;
                if (!re15_map_synth_cell(erste2 + c, &cx, &cy, &cw, &ch)) continue;
                for (k = 0; k < nops; k++) {
                    if (ops[k].x != cx || ops[k].y != cy) continue;
                    if (ops[k].w != cw || ops[k].h != ch) continue;
                    gefunden = 1;
                    n_fill++;
                    if (ops[k].kind != RE15_INV_OP_FILL) n_falsch++;
                    break;
                }
                if (!gefunden) n_raus++;
                if (cx < bx || cy < by || cx + cw > bx + bw || cy + ch > by + bh)
                    n_raus++;
            }
        }
        printf("  [Zeichner] %d Zellen gezeichnet, %d nicht OP_FILL, %d fehlen/ragen heraus\n",
               n_fill, n_falsch, n_raus);
        CHECK("die Schema-Zellen werden gezeichnet", n_fill > 0);
        CHECK("alle sind OP_FILL (OP_LINE deutet w/h als Endpunkt)", n_falsch == 0);
        CHECK("keine fehlt und keine ragt aus ihrem Kasten", n_raus == 0);
    }

    /* ⛔ DIE WELTBOX MUSS HERUM STIMMEN. Solange die vier Felder `short` waren, liefen
     * ROOM1180 und ROOM1230 mit wz1 = 32871 ueber und standen im Binary als -32665 -
     * die Box war verkehrt herum, und nur weil beide Raeume genau EINE Zone haben, fing
     * der Naechstgelegen-Rueckfall (re15_map_zones.c:101) das auf. Ein Ueberlauf faellt
     * hier auf, egal welcher Typ die Felder traegt. */
    {
        int n = re15_map_zone_count(), schief = 0;
        for (i = 0; i < n; i++) {
            const re15_map_zone_t *z = re15_map_zone_by_index(i);
            if (!z) continue;
            if (z->wx1 <= z->wx0 || z->wz1 <= z->wz0) {
                schief++;
                printf("     VERKEHRT: ROOM%04X z%d  x %ld..%ld  z %ld..%ld\n",
                       z->room, (int)z->idx, (long)z->wx0, (long)z->wx1,
                       (long)z->wz0, (long)z->wz1);
            }
        }
        printf("  [Weltbox] %d Zonen geprueft, %d verkehrt herum\n", n, schief);
        CHECK("jede Zonen-Weltbox hat positive Ausdehnung", schief == 0);
    }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
