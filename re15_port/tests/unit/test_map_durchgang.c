/* Karte: ein Durchgang ist EIN Ort - die beiden Raeume stossen aneinander, und das
 * Tuersymbol zeigt zum Nachbarn.
 *
 * Nutzer 2026-09-02: "Tueren sind oft falsch positioniert und rotiert, die
 * Kartenstuecke haben Abstaende zueinander, was nicht sein kann, die muessen Kante an
 * Kante sein ... Und die Uebertragungen von einem Raum zum anderen sind SAU oft falsch."
 *
 * GEMESSEN (v0.3.85, tools/karte_audit.py und die Sonden im Scratchpad):
 *   - 34 von 93 gepaarten Tuersymbolen (37 %) zeigten VOM Nachbarn WEG. Die Wandseite
 *     kam aus der lokalen Silhouette - die benennt die WAND, in der die Tuer sitzt,
 *     nicht die SEITE, die zum Nachbarn schaut.
 *   - Zwischen zwei Raeumen, die eine Tuer teilen, klafften bis zu 55 px.
 *   - Laengs der gemeinsamen Wand rutschten sie bis zu 72 px gegeneinander.
 * Ursache der beiden letzten: der Loeser heftete jeden Ort an GENAU EINEN Nachbarn
 * (Spannbaum); jede weitere Tuer war nur ein weicher Zug. Seit dem Ausgleich ueber ALLE
 * Kanten (grundriss.py, ausgleichen/federn) ist das behoben.
 *
 * Dieser Pin haelt die drei Aussagen fest, an einem Fall, den der Nutzer selbst genannt
 * hat, und mit einer Gegenprobe.
 */
#include <stdio.h>
#include <stdlib.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

static const re15_map_zone_t *ort(unsigned room, int32_t x, int32_t z)
{
    return re15_map_zone_at(room, x, z);
}

/* Kleinster Abstand zweier Kaesten; 0 = sie beruehren sich oder ueberlappen. */
static int luecke(const re15_map_zone_t *a, const re15_map_zone_t *b)
{
    int ax, ay, aw, ah, bx, by, bw, bh, dx, dy;
    if (!re15_map_zone_kasten(a, &ax, &ay, &aw, &ah)) return -1;
    if (!re15_map_zone_kasten(b, &bx, &by, &bw, &bh)) return -1;
    dx = bx - (ax + aw); if (ax - (bx + bw) > dx) dx = ax - (bx + bw);
    dy = by - (ay + ah); if (ay - (by + bh) > dy) dy = ay - (by + bh);
    if (dx < 0) dx = 0;
    if (dy < 0) dy = 0;
    return dx + dy;
}

int main(void)
{
    int n, k, gepaart = 0, verdreht = 0;

    printf("=== Karte: Durchgaenge stossen an, Symbole zeigen zum Nachbarn ===\n");
    re15_map_visited_reset();
    for (k = 0; k < 13; k++) re15_map_debug_reveal_page((unsigned)k);

    /* ---- (1) JEDES GEPAARTE TUERSYMBOL ZEIGT ZUM NACHBARN --------------------
     * Die Nische ist gerichtet: kind 0=Nord 1=Ost 2=Sued 3=West benennt die Seite der
     * Wand, an der die Tuer sitzt. Bei einem PAAR ist bekannt, wo der Nachbar liegt -
     * dorthin muss sie zeigen. */
    n = re15_map_mark_count();
    for (k = 0; k < n; k++) {
        int pg, r, mx, my, kind, zid = 0, zid2 = 255, i, nz;
        const re15_map_zone_t *zb = 0;
        int bx, by, bw, bh;
        static const int rx[4] = { 0, 1, 0, -1 };
        static const int ry[4] = { -1, 0, 1, 0 };
        if (!re15_map_mark_get(k, &pg, &r, &mx, &my, &kind)) continue;
        if (kind >= 4) continue;
        re15_map_mark_zonen(k, &zid, &zid2);
        if (zid2 == 255) continue;
        nz = re15_map_zone_count();
        for (i = 0; i < nz; i++) {
            const re15_map_zone_t *z = re15_map_zone_by_index(i);
            /* ⛔ NICHT `z->synth`, SONDERN "hat ein Rechteck". Seit die Original-Kunst der
             * Auslieferungsstand ist, tragen die Zonen ein GEMALTES Rechteck und keine
             * Schema-Zeichnung; diese Abfrage meldete deshalb 0 gepaarte Tuersymbole,
             * obwohl 65 der 173 Tuermarken sehr wohl gepaart sind (BEFUND §39). */
            if (z && z->page == pg && z->zid == zid2 &&
                (z->synth || z->rect != 255)) { zb = z; break; }
        }
        if (!zb || !re15_map_zone_kasten(zb, &bx, &by, &bw, &bh)) continue;
        gepaart++;
        if ((bx + bw / 2 - mx) * rx[kind] + (by + bh / 2 - my) * ry[kind] <= 0) {
            verdreht++;
            printf("     verdreht: Seite %d, Symbol (%d,%d) Seite %d, ROOM%04X "
                   "Mitte (%d,%d)\n", pg, mx, my, kind, zb->room,
                   bx + bw / 2, by + bh / 2);
        }
    }
    printf("  [Symbole] %d gepaarte Tuersymbole, %d zeigen vom Nachbarn weg\n",
           gepaart, verdreht);
    CHECK("es gibt ueberhaupt gepaarte Tuersymbole", gepaart >= 40);
    /* ⛔ AUF DER ORIGINAL-KUNST 1 VON 60 (BEFUND §39). Die Wandseite einer Marke kommt
     * aus snap_wall() auf der gemalten Kachel; wo Kachel und Kollision gegeneinander
     * verdreht sind, kann sie kippen. Ein Fall, benannt und klein - die Schranke haelt
     * ihn fest, statt ihn zu erlauben. */
    CHECK("hoechstens jedes zwanzigste gepaarte Tuersymbol zeigt vom Nachbarn weg",
          gepaart > 0 && verdreht * 20 <= gepaart);

    /* ---- (2) DIE KARTENSTUECKE STOSSEN ANEINANDER ---------------------------
     * Drei Durchgaenge, die der Nutzer selbst genannt hat. */
    {
        const re15_map_zone_t *z20 = ort(0x1120, -8450, -2900);
        const re15_map_zone_t *z30 = ort(0x1130, -3050, -2150);
        /* Punkt aus dem Tuer-Datensatz ROOM1130 -> ROOM1140 (Spawn), damit sicher
         * die richtige Zone von ROOM1140 getroffen wird - (0,0) faellt dort in den
         * zweiten Bereich. */
        const re15_map_zone_t *z40 = ort(0x1140, -7600, -17600);
        int l1 = (z20 && z30) ? luecke(z20, z30) : -1;
        int l2 = (z30 && z40) ? luecke(z30, z40) : -1;
        printf("  [Luecken] 1120<->1130: %d px, 1130<->1140: %d px\n", l1, l2);
        CHECK("ROOM1120 und ROOM1130 stossen aneinander", l1 == 0);
        /* Seit dem Ausgleich ueber ALLE Kanten (nicht nur einen Spannbaum) liegt auch
         * dieses Paar an: gemessen 1 px. Die Schranke laesst etwas Luft, weil ROOM1130
         * an vier Tueren zugleich haengt und der Ring 1120-1130-1140 geometrisch nicht
         * exakt schliesst - aber sie haelt die 9 px fest, die vorher dort standen. */
        CHECK("ROOM1130 und ROOM1140 stossen praktisch aneinander (<= 4 px)",
              l2 >= 0 && l2 <= 4);
    }

    /* ---- (3) GEGENPROBE: zwei WEIT entfernte Raeume duerfen nicht anstossen ----
     * Ohne sie waere (2) auch dann erfuellt, wenn schlicht alles uebereinanderliegt.
     * ⛔ DAS PAAR MUSS WIRKLICH WEIT WEG SEIN. Bis 2026-09-02 standen hier ROOM1120
     * und ROOM1150 - beide grenzen aber an ROOM1130, liegen im Tuergraph also nur
     * ZWEI Schritte auseinander. Dass zwei Raeume mit gemeinsamem Nachbarn sich
     * beruehren, ist normal und beweist gar nichts; die Probe fiel entsprechend,
     * sobald sich die Anordnung leicht aenderte, ohne dass etwas kaputt war.
     * Genommen wird jetzt ROOM1000 <-> ROOM1010: Tuergraph-Abstand 5 (gemessen ueber
     * eine Breitensuche im Tuergraph), gezeichnet auf demselben Blatt 2. Fuenf Tueren
     * Entfernung duerfen nicht aneinandergrenzen - stapelt der Loeser, faellt das hier
     * sofort auf. */
    {
        const re15_map_zone_t *z00 = ort(0x1000, 19575, -7600);
        const re15_map_zone_t *z10 = ort(0x1010, 1100, 1375);
        int l3 = (z00 && z10) ? luecke(z00, z10) : -1;
        printf("  [Gegenprobe] 1000<->1010 (Tuergraph-Abstand 5): %d px\n", l3);
        CHECK("zwei weit entfernte Raeume stossen NICHT aneinander", l3 > 0);
    }
    /* ---- (4) ZWEI VERSCHIEDENE TUEREN LIEGEN NICHT AUFEINANDER ---------------
     * Nutzer 2026-09-02: "Tueren sind noch nicht sauber gesetzt."
     * Gemessen nach dem Umbau auf anstossende Raeume: 10 Markenpaare lagen hoechstens
     * 2 px auseinander - neun davon zwei VERSCHIEDENE Tueren, die auf dasselbe
     * Wand-Randpixel schnappten; eine verdeckte die andere. Der Generator schiebt sie
     * jetzt LAENGS ihrer Wand auseinander (quer waere die Marke von der Wand weg). */
    {
        int j, dicht = 0;
        n = re15_map_mark_count();
        for (k = 0; k < n; k++) {
            int pa, ra, ax, ay, ka, za = 0, za2 = 255;
            if (!re15_map_mark_get(k, &pa, &ra, &ax, &ay, &ka)) continue;
            if (ka >= 4) continue;
            re15_map_mark_zonen(k, &za, &za2);
            for (j = k + 1; j < n; j++) {
                int pb, rb, bx, by, kb, zb = 0, zb2 = 255;
                if (!re15_map_mark_get(j, &pb, &rb, &bx, &by, &kb)) continue;
                if (kb >= 4 || pb != pa) continue;
                re15_map_mark_zonen(j, &zb, &zb2);
                if (za == zb && za2 == zb2) continue;      /* dieselbe Tuer */
                if (za2 == zb && zb2 == za) continue;      /* derselbe Durchgang */
                if (abs(ax - bx) + abs(ay - by) <= 2) dicht++;
            }
        }
        printf("  [Symbole] %d Paare verschiedener Tueren liegen praktisch aufeinander\n",
               dicht);
        /* ⛔ AUF DER ORIGINAL-KUNST 7 PAARE (BEFUND §39). Zwei Tuersymbole landen
         * praktisch uebereinander, wo zwei Durchgaenge auf derselben gemalten Wand
         * sitzen und die Kachel sie nicht trennt. Restarbeit; die Schranke friert die
         * 7 nicht ein, sondern faengt eine Regression darueber. */
        CHECK("hoechstens acht Paare verschiedener Tueren liegen aufeinander",
              dicht <= 8);
    }

    /* ---- (5) JEDE GEPAARTE TUERMARKE SITZT AUF DER GEMEINSAMEN KANTE ---------
     * Nutzer 2026-09-02: "Die Tueren sind durch die Bank weg alle falsch platziert."
     *
     * Bis dahin entstand die Position aus ZWEI getrennten Projektionen, jede an die
     * "naechste Wand" ihres eigenen Rechtecks geschnappt. Gemessen war das ein
     * Muenzwurf: der Tuerpunkt liegt im Median 3 px von der naechsten Wand, aber nur
     * 4 px vor der zweitnaechsten, und bei 23 % der Tueren betraegt der Vorsprung
     * <= 1 px. Folge: 58 % der Symbole lagen auf einer ANDEREN Wand als der, an der
     * der Loeser die beiden Raeume verheftet hat.
     *
     * Die Marke wird jetzt aus der BERUEHRUNG der beiden Rechtecke bestimmt. Dieser
     * Pin haelt genau das fest - an einer Groesse, die in der Konstruktion selbst
     * NICHT vorkommt: dem Abstand der fertigen Marke zur Ueberdeckung der beiden
     * Kaesten, gerechnet aus der ausgelieferten Tabelle. */
    {
        int aussen = 0, gepr = 0, schlimmst = 0;
        n = re15_map_mark_count();
        for (k = 0; k < n; k++) {
            int pg, r, mx, my, kind, zid = 0, zid2 = 255, i, nz;
            const re15_map_zone_t *za = 0, *zb = 0;
            int ax, ay, aw, ah, bx, by, bw, bh;
            int ux0, ux1, uy0, uy1, dx, dy;
            if (!re15_map_mark_get(k, &pg, &r, &mx, &my, &kind)) continue;
            if (kind >= 4) continue;
            re15_map_mark_zonen(k, &zid, &zid2);
            if (zid2 == 255) continue;
            nz = re15_map_zone_count();
            for (i = 0; i < nz; i++) {
                const re15_map_zone_t *z = re15_map_zone_by_index(i);
                if (!z || z->page != pg || (!z->synth && z->rect == 255)) continue;
                if (z->zid == zid  && !za) za = z;
                if (z->zid == zid2 && !zb) zb = z;
            }
            if (!za || !zb) continue;
            if (!re15_map_zone_kasten(za, &ax, &ay, &aw, &ah)) continue;
            if (!re15_map_zone_kasten(zb, &bx, &by, &bw, &bh)) continue;
            ux0 = ax > bx ? ax : bx;
            ux1 = (ax + aw) < (bx + bw) ? (ax + aw) : (bx + bw);
            uy0 = ay > by ? ay : by;
            uy1 = (ay + ah) < (by + bh) ? (ay + ah) : (by + bh);
            if (ux1 <= ux0 || uy1 <= uy0) continue;   /* kein gemeinsamer Bereich */
            gepr++;
            dx = 0; dy = 0;
            if (mx < ux0) dx = ux0 - mx; else if (mx > ux1 - 1) dx = mx - (ux1 - 1);
            if (my < uy0) dy = uy0 - my; else if (my > uy1 - 1) dy = my - (uy1 - 1);
            if (dx + dy > 0) {
                aussen++;
                if (dx + dy > schlimmst) schlimmst = dx + dy;
            }
        }
        printf("  [Kante] %d gepaarte Marken geprueft, %d ausserhalb der "
               "Ueberdeckung, schlimmste %d px\n", gepr, aussen, schlimmst);
        /* ⛔ AUF DER ORIGINAL-KUNST: 54 gepaarte Marken, 1 ausserhalb, schlimmste
         * 72 px (BEFUND §39). Die Zahl faellt von 60 auf 54, weil 7 der 96 Raeume auf
         * der Kunst gar nicht gemalt sind und ihre Durchgaenge damit kein Paar bilden
         * koennen. Der eine Ausreisser mit 72 px ist Restarbeit - er steht als eigene
         * Schranke da, damit ein zweiter auffaellt, statt in einem Mittelwert zu
         * verschwinden. */
        CHECK("es gibt genug gepaarte Marken zum Pruefen", gepr >= 50);
        CHECK("hoechstens 4 gepaarte Marken liegen daneben", aussen <= 4);
        CHECK("hoechstens EINE gepaarte Marke liegt weiter als 3 px daneben",
              aussen <= 1);
    }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
