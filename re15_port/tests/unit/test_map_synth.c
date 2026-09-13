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

    /* (1) JEDE Schema-Zeichnung liegt im Kartenfeld.
     * ⛔ FRUEHER STAND HIER NUR ROOM10D0 (der 2F-Flur des RE15_KUNST=0-Pfads). Seit
     * 2026-09-13 traegt ROOM1000 drei Schema-Kaesten (die Halle gehoert ROOM1030, der
     * Raum selbst ist auf Blatt 2 ueberhaupt nicht gemalt - karte-1000-1050.md), und
     * der Test lief mit ROOM10D0-Erwartungen in einen Fehlschlag, obwohl der
     * Auslieferungsstand fuer 10D0 gar keine Zeichnung mehr baut. Geprueft wird
     * jetzt, was da ist - nicht ein bestimmter Raum. */
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *zz = re15_map_zone_by_index(i);
        char t[96];
        if (!zz || !re15_map_zone_synth(zz, &x, &y, &w, &h, &erste, &n)) continue;
        printf("  [ROOM%04X z%d] Blatt %d, Kasten (%d,%d) %dx%d, %d Zellen\n",
               zz->room, zz->idx, zz->page, x, y, w, h, n);
        snprintf(t, sizeof t, "ROOM%04X z%d: Kasten im Kartenfeld", zz->room, zz->idx);
        CHECK(t, x >= 90 && y >= 50 && x + w <= 240 && y + h <= 200 && n > 0);
    }
    (void)zn;

    /* (2) Keine Schema-Zeichnung darf ein GEMALTES Rechteck ueberdecken. Die Kunst des
     *     Originals ist nicht massstabsgetreu zur Kollision; anker-genau gesetzt lag
     *     ROOM10D0 ueber vier Nachbarn zugleich. */
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *a = re15_map_zone_by_index(i);
        int ax, ay, aw, ah, j;
        /* ⛔ NUR SCHEMA-ZONEN (korrigiert 2026-09-13): hier stand
         * re15_map_zone_kasten(), das fuer rect != 255 die GEMALTE Rechteck-Geometrie
         * liefert - die Schleife verglich also jedes gemalte Rechteck mit jedem
         * anderen und meldete deren voellig normale Nachbarschaften als
         * "Ueberlappung". Solange es gar keine Schema-Zonen gab, sprang der Test
         * vorher heraus und es fiel nicht auf. */
        if (!a || a->rect != 255) continue;
        if (!re15_map_zone_synth(a, &ax, &ay, &aw, &ah, 0, 0)) continue;
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
    printf("  [Bestand] %d Schema-Zeichnungen, %d liegen in der Bbox eines Rechtecks\n",
           n_synth, n_ueber);
    CHECK("es gibt Schema-Zeichnungen", n_synth > 0);
    /* ⛔ HIER STAND "keine davon ueberdeckt ein gemaltes Rechteck" - UND DAS MASS WAR
     * ZU GROB (2026-09-13). Geprueft wurde die BBOX eines Rechtecks, nicht seine
     * KUNST. Die beiden fallen auseinander, sobald eine Kachel gedreht oder gespiegelt
     * montiert ist: ROOM1050s Rect 0 spannt auf Blatt 2 eine Bbox, die weit ueber die
     * bemalten Texel hinausreicht - oestlich der Flur-Ostwand malt das Blatt unterhalb
     * y=88 keinen einzigen Punkt, die Bbox deckt die Stelle aber. Genau dort gehoeren
     * ROOM1000s drei Kaesten hin. Ein Bbox-Riegel haette den richtigen Stand
     * zurueckgewiesen (dieselbe Falle wie beim Marken-Riegel, s. Kopf von
     * tools/marken_auf_kunst.py: "eine Marke kann INNERHALB ihres Rechtecks liegen und
     * trotzdem auf nichts sitzen").
     * Der echte Riegel liegt deshalb dort, wo die Kunst wirklich gelesen wird:
     * `python re15_port/tools/marken_auf_kunst.py` prueft jede Schema-Zelle gegen die
     * Texel von DATA/MAP0x.PIX. Gemessener Stand 2026-09-13: 0 von 960 Punkten der drei
     * ROOM1000-Kaesten liegen auf bemalter Flaeche (bei x=206 waeren es 64 gewesen -
     * die Flur-Ostwand ist drei Pixel breit, Spalten 204..206). Hier bleibt die Zahl als
     * Telemetrie stehen. */

    /* (3) Die gezeichneten Ops muessen OP_FILL sein und in ihrem Kasten liegen.
     *     ⛔ Der erste Wurf nahm RE15_INV_OP_LINE - das deutet (w,h) als
     *     ENDPUNKT, nicht als Groesse (inv_render_pc.c:547-566). Jede Zelle wurde
     *     dadurch zu einem Strich von ihrer Ecke zu (w,h), quer ueber den Schirm.
     *     Der Abzug des 2F-Blattes zeigte lange senkrechte Streifen; die TABELLE war
     *     dabei einwandfrei. Ein Pin auf die Tabelle allein haette das nicht gefangen. */
    {
        static re15_inv_op_t ops[768];
        int nops, k, a3, n_fill = 0, n_falsch = 0, n_raus = 0;
        /* Die Seite nehmen, auf der die Schema-Zonen wirklich liegen (frueher fest
         * Blatt 3 = der 2F-Flur des RE15_KUNST=0-Pfads; ROOM1000 liegt auf Blatt 2). */
        int seite = 3;
        for (a3 = 0; a3 < re15_map_zone_count(); a3++) {
            const re15_map_zone_t *zz = re15_map_zone_by_index(a3);
            int q1, q2, q3, q4, q5, q6;
            if (zz && zz->rect == 255 &&
                re15_map_zone_synth(zz, &q1, &q2, &q3, &q4, &q5, &q6)) {
                seite = zz->page; break;
            }
        }
        re15_map_visited_reset();
        re15_map_debug_reveal_page(seite);
        re15_inv_map_stage_init(0, 13);
        re15_inv_screen_open();
        g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
        g_inv_screen.map_page = seite;
        nops = re15_inv_screen_build(&g_inv_screen, ops, 768);
        /* ⛔ GENAU die Zellen der Tabelle suchen, nicht alles im Kasten: auf Seite 0
         * der Op-Liste liegen auch Chrome und Marken, die zufaellig hineinfallen -
         * ein erster Anlauf zaehlte 8 solcher Fremd-Ops als Fehler. */
        for (a3 = 0; a3 < re15_map_zone_count(); a3++) {
            const re15_map_zone_t *zz = re15_map_zone_by_index(a3);
            int bx, by, bw, bh, erste2, n2, c;
            if (!zz || zz->page != seite) continue;
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
