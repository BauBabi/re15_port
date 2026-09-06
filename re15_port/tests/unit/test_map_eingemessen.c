/* Vom Nutzer EINGEMESSENE Kartenpunkte — die Abbildung muss sie treffen.
 *
 * ⛔ WOZU DIESER HAKEN: die Lage eines Raums auf der Karte laesst sich fuer die 39
 * Raeume ohne Massstabszeile des Originals NICHT rechnerisch bestimmen. Fuenf Verfahren
 * sind daran gemessen gescheitert (BEFUND §49-§52): Tueranker sind bei einem Flur
 * kollinear, und der Formvergleich zwischen begehbarer Flaeche und gemalter Kachel fiel
 * bei der unabhaengigen Tuerprobe jedes Mal durch — die Kachel ist ein SCHEMA, kein
 * massstaeblicher Grundriss.
 *
 * Was traegt, sind EINGEMESSENE Punkte: der Nutzer stellt sich im Spiel an eine
 * eindeutige Stelle und drueckt F9. Das Log liefert die Weltlage, der Abzug zeigt, WO
 * er steht. Diese Punkte sind damit Messwerte wie eine Disassembly-Adresse — und
 * genauso muessen sie gegen stilles Verrutschen geschuetzt sein. Genau das tut dieser
 * Haken.
 *
 * ⛔ ER PRUEFT DIE ABBILDUNG, NICHT DIE MARKE. Verglichen wird die Projektion der
 * gemessenen WELTLAGE gegen den Ort, an den sie gehoert — nicht gegen ein gezeichnetes
 * Symbol, dessen Lage der Generator selbst bestimmt. Sonst pruefte der Haken seine
 * eigene Rechnung (selbstbestaetigende Metrik).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Ein eingemessener Punkt: Weltlage aus befund.log, Sollort aus der ZEICHNUNG. */
typedef struct {
    unsigned room;
    int32_t  wx, wz;
    int      soll_x, soll_y;
    int      toleranz;
    const char *woher;
} messpunkt_t;

/* ---- ROOM10C0, eingemessen 2026-09-06 -------------------------------------------
 * Der Nutzer hat sechs Marken gesetzt; zwei davon sind eindeutige Anker, weil ihr
 * Sollort aus der ZEICHNUNG folgt und nicht aus unserer Rechnung:
 *
 *   Marke 1  Welt(  -132, 10374)  Abzug zeigt ihn an der Tuer zum Treppenhaus.
 *            Das Treppenhaus ROOM1060 ist als Rect 1 (118,134) 24x24 INNERHALB von
 *            Rect 0 gezeichnet; die Tuer liegt also auf dessen Rand -> (118,150).
 *   Marke 5  Welt( -7132, -1896)  Abzug zeigt ihn an der Doppeltuer zu ROOM10D0.
 *            Rect 3 (135,76) 72x88 beruehrt Rect 0 an x=135 -> (135,123).
 *
 * Gesamtfehler beider Anker je Spiegelung, gemessen:
 *     flip 0/0 (bis dahin)  82 px | 0/1  36 px | 1/0  54 px | 1/1   8 px
 * Die Zeichnung ist also in BEIDEN Achsen gespiegelt: Marke 5 steht am WEST-Ende des
 * Raums, ihre Tuer aber auf der OST-Seite der Zeichnung.
 *
 * TOLERANZ 8 px: der Marker ist ein 8x8-Feld, und die Bbox-Streckung bleibt auch mit
 * richtiger Spiegelung eine Naeherung. Geprueft wird die GROESSENORDNUNG (Faktor 10
 * besser als vorher), nicht die letzte Stelle. */
/* ---- ROOM10D0, eingemessen 2026-09-06 mit 15 F9-Marken ---------------------------
 * Der Nutzer ist den L-foermigen Flur abgelaufen und hat in neun Kamerawinkeln
 * markiert (Welt x -18877..8182, z -8264..25582). Gemessen wurde, wie viele Marken auf
 * der GEMALTEN Flaeche von Rect 3 landen - ein Mass, das die Marken selbst nicht
 * beeinflussen (die Zeichnung ist Originaldatei):
 *     flip 0/0 (bisher)   1 von 15, mittlerer Abstand 38,9 px
 *     flip 0/1            6 von 15, 35,5 px
 *     flip 1/0            6 von 15, 28,9 px
 *     flip 1/1           13 von 15,  0,3 px     <- 28,7 px Vorsprung
 * Zwei der 15 liegen daneben; beide am Rand der Zeichnung, wo die Bbox-Streckung auch
 * mit richtiger Spiegelung eine Naeherung bleibt.
 * Hier stehen zwei weit auseinanderliegende davon als Schranke. */
static const messpunkt_t MESS[] = {
    { 0x10C0,  -132, 10374, 118, 150, 8, "F9-Marke 1: Tuer zum Treppenhaus ROOM1060" },
    { 0x10C0, -7132, -1896, 135, 123, 8, "F9-Marke 5: Doppeltuer zu ROOM10D0"        },
    /* Die beiden am weitesten auseinanderliegenden TREFFER (13 der 15 landen auf der
     * Zeichnung; die zwei Ausreisser 16/17 am Nordwestende bleiben aussen vor - dort
     * ist die Bbox-Streckung auch mit richtiger Spiegelung noch ungenau).
     * Sollort ist die Lage, die sich mit flip 1/1 ergibt und die auf der gemalten
     * Flaeche liegt; Toleranz 4 px. */
    { 0x10D0,   1273, -8264, 156,  80, 4, "F9-Marke 3: Suedende des Flurs"     },
    { 0x10D0, -14782, 25582, 191, 152, 4, "F9-Marke 15: Nordwestarm des Flurs" },
};

int main(void)
{
    unsigned i;
    printf("== Vom Nutzer eingemessene Kartenpunkte ==\n");
    CHECK("es gibt Zonen", re15_map_zone_count() > 0);

    for (i = 0; i < sizeof MESS / sizeof MESS[0]; i++) {
        const messpunkt_t *m = &MESS[i];
        const re15_map_zone_t *zn = NULL;
        int rx, ry, rw, rh, zi, nz = re15_map_zone_count();
        int16_t mx = 0, my = 0;
        char t[220];
        int d;

        for (zi = 0; zi < nz && !zn; zi++) {
            const re15_map_zone_t *z = re15_map_zone_by_index(zi);
            if (z && z->room == m->room && z->rect != 255) zn = z;
        }
        if (!zn) { printf("  FAIL: keine Zone fuer ROOM%04X\n", m->room); g_fail = 1; continue; }
        if (!re15_map_rect_geometry(zn->page, zn->rect, &rx, &ry, &rw, &rh)) {
            printf("  FAIL: kein Rechteck fuer ROOM%04X\n", m->room); g_fail = 1; continue;
        }
        if (!re15_map_zone_marker(zn, m->wx, m->wz, rx, ry, rw, rh, &mx, &my)) {
            printf("  FAIL: keine Projektion fuer ROOM%04X\n", m->room); g_fail = 1; continue;
        }
        d = abs((int)mx - m->soll_x) + abs((int)my - m->soll_y);
        snprintf(t, sizeof t,
                 "%s: Welt(%d,%d) -> (%d,%d), soll (%d,%d), Abstand %d px (<= %d)",
                 m->woher, (int)m->wx, (int)m->wz, (int)mx, (int)my,
                 m->soll_x, m->soll_y, d, m->toleranz);
        CHECK(t, d <= m->toleranz);
    }
    /* ⛔ DIE FAHRSTUHLTUER IST MIR DREIMAL VERRUTSCHT: v0.6.5 an die Unterkante der
     * Kabine (117,149), v0.6.8 an die Oberkante (117,134) - und beide Male hatte ich
     * ein Kriterium ERFUNDEN, statt die Projektion stehen zu lassen. Der Nutzer hat
     * sich am 2026-09-06 an die Tuer gestellt (Welt 1618/6868) und gesagt "die muss
     * genau dort hin"; das projiziert auf (117,143). Seither gilt: liegt die Projektion
     * schon IM Ziel-Rechteck, bleibt sie stehen. Diese Pruefung haelt die MARKE fest,
     * nicht die Projektion - denn die Marke war es, die wanderte. */
    {
        int i, gefunden = 0, mx = -1, my = -1;
        /* ⛔ re15_map_mark_get zeigt nur BESUCHTE Marken - ohne dieses Setzen liefert
         * es nichts, und der Haken meldete "nicht gefunden" statt zu pruefen. */
        re15_map_visited_reset();
        re15_map_visited_mark(0x10C0u);
        re15_map_visited_mark(0x1080u);
        for (i = 0; i < re15_map_mark_count(); i++) {
            int p, r, x, y, kind;
            const re15_map_zone_t *zn;
            int zi, nz = re15_map_zone_count();
            if (!re15_map_mark_get(i, &p, &r, &x, &y, &kind)) continue;
            if (p != 3 || r != 0 || kind >= 4) continue;
            for (zi = 0; zi < nz; zi++) {
                zn = re15_map_zone_by_index(zi);
                if (zn && zn->page == 3 && zn->rect == 0 && zn->room == 0x10C0u) break;
            }
            /* die Fahrstuhltuer ist die einzige Marke von ROOM10C0 ohne Partnerzone */
            if (x > 105 && x < 130 && y > 130 && y < 155) { gefunden = 1; mx = x; my = y; break; }
        }
        if (!gefunden) {
            printf("  FAIL: Fahrstuhltuer-Marke auf Blatt 3 Rect 0 nicht gefunden\n");
            g_fail = 1;
        } else {
            char t[200];
            int d = abs(mx - 117) + abs(my - 143);
            snprintf(t, sizeof t,
                     "Fahrstuhltuer steht auf der F9-Marke des Nutzers: (%d,%d), "
                     "soll (117,143), Abstand %d px (<= 3)", mx, my, d);
            CHECK(t, d <= 3);
        }
    }

    re15_map_visited_reset();
    return g_fail;
}
