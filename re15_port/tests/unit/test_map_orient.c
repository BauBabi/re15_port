/* Karte: man betritt einen Raum DA, WO DIE TUER IST.
 *
 * Nutzer 2026-09-01: "Im Room 1120 lande ich wieder ganz komisch ploetzlich auf der
 * falschen Seite, wenn ich vom ROOM 1130 aus rein komme."
 *
 * ⛔ DIESER PIN WURDE AM 2026-09-02 UMGESCHRIEBEN, NICHT ABGESCHWAECHT.
 * Er hielt vorher eine Aussage ueber die gemalte Kunst fest: "ROOM1120 liegt auf
 * Seite 4 Rect 5, und seine Zeichnung ist um 180 Grad gedreht". Diese Rechteck-
 * Zuordnung existiert nicht mehr: seit dem Umbau auf Grundrisse (tools/grundriss.py)
 * wird jeder Ort aus seinen eigenen Kollisionszellen gezeichnet und vom Loeser
 * gesetzt, die Drehung faellt dort selbst und traegt keine feste Zahl mehr.
 *
 * Die BEOBACHTUNG des Nutzers ist davon unberuehrt und wird jetzt direkt geprueft,
 * statt ueber einen Umweg: der Punkt, an dem der Spieler in ROOM1120 ERSCHEINT
 * (Spawn nx/nz des Tuer-Datensatzes), muss auf der Karte dort liegen, wo dieselbe
 * Tuer gezeichnet wird. Das ist genau der Satz des Nutzers vom selben Tag - "ein
 * neues Kartenstueck schliesst genau da an, wo man den Raum davor durch die Tuer
 * verlassen hat" - und es ist eine schaerfere Bedingung als die alte, weil sie
 * BEIDE Seiten des Durchgangs und den Eintrittspunkt zugleich bindet.
 *
 * Daten (RDT Aot_set 0x3b, gelesen 2026-09-02):
 *   ROOM1120 Tuer[2] l=(-8450,-2900) -> ROOM1130, Spawn dort (-2200,-2150)
 *   ROOM1130 Tuer[1] l=(-3050,-2150) -> ROOM1120, Spawn dort (-7600,-2900)
 *   ROOM1120 Tuer[0] l=( -700, 9550) -> ROOM1060  (die GEGENPROBE: ein anderer
 *                                       Ausgang desselben Raums, weit entfernt)
 */
#include <stdio.h>
#include <stdlib.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Weltpunkt -> Kartenpixel, genau wie der Zeichner: der Ort wird ueber die
 * Spielerposition gesucht, sein Kasten ist der Rahmen der Projektion. */
static int auf_karte(unsigned room, int32_t wx, int32_t wz, int *mx, int *my)
{
    const re15_map_zone_t *z = re15_map_zone_at(room, wx, wz);
    int rx, ry, rw, rh;
    int16_t a, b;
    if (!z) return 0;
    if (!re15_map_zone_synth(z, &rx, &ry, &rw, &rh, 0, 0)) return 0;
    if (!re15_map_zone_marker(z, wx, wz, rx, ry, rw, rh, &a, &b)) return 0;
    *mx = a; *my = b;
    return 1;
}

static int abstand(int ax, int ay, int bx, int by)
{
    return abs(ax - bx) + abs(ay - by);
}

int main(void)
{
    int tx, ty, sx, sy, gx, gy, d;
    const re15_map_zone_t *z20, *z30;
    int k20x, k20y, k20w, k20h, k30x, k30y, k30w, k30h;

    printf("=== Karte: der Eintritt liegt an der Tuer (ROOM1120 <-> ROOM1130) ===\n");

    /* ---- (1) ROOM1120: Tuer nach 1130 gegen den Punkt, an dem man dort steht ---- */
    if (!auf_karte(0x1120, -8450, -2900, &tx, &ty) ||
        !auf_karte(0x1120, -7600, -2900, &sx, &sy)) {
        printf("  FAIL: ROOM1120 hat keinen Grundriss\n"); return 1;
    }
    printf("  [ROOM1120] Tuer -> 1130 auf (%d,%d), Eintritt von 1130 auf (%d,%d)\n",
           tx, ty, sx, sy);
    d = abstand(tx, ty, sx, sy);
    printf("  [Abstand] Eintritt zur eigenen Tuer: %d px (Manhattan)\n", d);
    CHECK("man erscheint in ROOM1120 an der Tuer, durch die man kam (<= 4 px)", d <= 4);

    /* ---- (2) GEGENPROBE: ein anderer Ausgang desselben Raums liegt WOANDERS ----
     * Ohne diese Probe waere die erste Bedingung auch dann erfuellt, wenn die
     * Projektion alles auf einen Punkt zusammenfallen liesse. */
    if (!auf_karte(0x1120, -700, 9550, &gx, &gy)) {
        printf("  FAIL: ROOM1120 Tuer nach 1060 nicht projizierbar\n"); return 1;
    }
    d = abstand(tx, ty, gx, gy);
    {
        /* ⛔ DIE SCHRANKE MUSS AM RAUM HAENGEN, NICHT AN EINER PIXELZAHL. Sie stand
         * bei 15 px - gemessen an einem ROOM1120, das damals 34x36 px gross war. Seit
         * alle Blaetter eines Gebaeudes denselben Massstab teilen (damit die Stockwerke
         * stapeln), misst derselbe Raum 14x15 px, und die 15 px waeren allein durch den
         * Massstabswechsel gerissen - ohne dass sich an der Karte etwas verschlechtert
         * haette. Verlangt wird deshalb: die andere Tuer liegt mindestens ein Viertel
         * der Raumausdehnung entfernt. */
        const re15_map_zone_t *zz = re15_map_zone_at(0x1120, -8450, -2900);
        int qx, qy, qw, qh, schranke;
        if (!zz || !re15_map_zone_synth(zz, &qx, &qy, &qw, &qh, 0, 0)) {
            printf("  FAIL: ROOM1120 hat keinen Kasten\n"); return 1;
        }
        schranke = (qw + qh) / 4;
        if (schranke < 5) schranke = 5;
        printf("  [Gegenprobe] Tuer -> 1060 auf (%d,%d), Abstand zur 1130-Tuer: %d px "
               "(Raum %dx%d, Schranke %d)\n", gx, gy, d, qw, qh, schranke);
        CHECK("die andere Tuer desselben Raums liegt deutlich woanders",
              d >= schranke);
    }

    /* ---- (3) DASSELBE VON DER ANDEREN SEITE ---- */
    if (!auf_karte(0x1130, -3050, -2150, &tx, &ty) ||
        !auf_karte(0x1130, -2200, -2150, &sx, &sy)) {
        printf("  FAIL: ROOM1130 hat keinen Grundriss\n"); return 1;
    }
    d = abstand(tx, ty, sx, sy);
    printf("  [ROOM1130] Tuer -> 1120 auf (%d,%d), Eintritt von 1120 auf (%d,%d), "
           "Abstand %d px\n", tx, ty, sx, sy, d);
    CHECK("man erscheint in ROOM1130 an der Tuer, durch die man kam (<= 4 px)", d <= 4);

    /* ---- (4) DIE BEIDEN ORTE BERUEHREN SICH AUF DEM BLATT ----
     * Der Durchgang ist EIN Ort: die Zeichnungen duerfen nicht auseinanderstehen. */
    z20 = re15_map_zone_at(0x1120, -8450, -2900);
    z30 = re15_map_zone_at(0x1130, -3050, -2150);
    if (!z20 || !z30 ||
        !re15_map_zone_synth(z20, &k20x, &k20y, &k20w, &k20h, 0, 0) ||
        !re15_map_zone_synth(z30, &k30x, &k30y, &k30w, &k30h, 0, 0)) {
        printf("  FAIL: kein Kasten\n"); return 1;
    }
    CHECK("beide Orte liegen auf demselben Blatt", z20->page == z30->page);
    {
        int dx = k30x - (k20x + k20w);
        int dy = k30y - (k20y + k20h);
        int ex = k20x - (k30x + k30w);
        int ey = k20y - (k30y + k30h);
        int lu = (dx > 0 ? dx : 0) + (ex > 0 ? ex : 0) +
                 (dy > 0 ? dy : 0) + (ey > 0 ? ey : 0);
        printf("  [Kaesten] 1120 (%d,%d %dx%d), 1130 (%d,%d %dx%d), Luecke %d px\n",
               k20x, k20y, k20w, k20h, k30x, k30y, k30w, k30h, lu);
        CHECK("die beiden Zeichnungen stossen aneinander (Luecke <= 1 px)", lu <= 1);
    }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
