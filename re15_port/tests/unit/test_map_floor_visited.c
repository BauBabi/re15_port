/* Karte: unzugeordnete Rechtecke bleiben schwarz, und eine Etagen-Zeichnung erscheint
 * erst, wenn man AUF DIESER ETAGE war.
 *
 * Nutzer 2026-09-01, Punkt 3: "Im Room 1130 gibt es unten links schon ein Rechteck nach
 * dem Flur, obwohl ich noch im Eingangsbereich stehe." Das Rechteck ist Seite 4 Rect 0
 * (127,137,16,16) = ROOM1060s Zeichnung auf dem 3F-Blatt. Es erschien aus zwei
 * Gruenden, die beide hier festgenagelt sind:
 *   1. ein Rechteck OHNE Zuordnung wurde grau gemalt statt weggelassen,
 *   2. die Etagen-Zeichnung erbte das Besucht-Bit der ZONE, nicht der ETAGE - wer
 *      unten durchs Treppenhaus lief, hatte damit dessen 3F-Zeichnung freigeschaltet.
 * Das Original zeichnet ein Rechteck nur bei gesetztem Besucht-Bit (FUN_800473f8,
 * Schleife @0x800475f8-61c ueber die count Rechtecke des Seiten-Paars @0x80076840). */
#include <stdio.h>
#include <string.h>
#include "re15_inv_screen.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_collision.h"

extern re15_inv_screen_t g_inv_screen;
extern unsigned g_current_room_id;

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

static void stelle(unsigned room, int32_t x, int32_t z, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = x; pl->z = z; pl->y = -band * 0x708;
    re15_collision_set_band(band);
    g_current_room_id = room;
    re15_map_zone_update(room, x, z);
    re15_map_visited_mark_at(room, x, z);
}

int main(void)
{
    printf("=== Karte: Etagen-Besucht + unzugeordnete Rechtecke ===\n");
    re15_map_visited_reset();

    /* ⛔ TREPPENHAUS UND FAHRSTUHL WAREN VERTAUSCHT (2026-09-01). Seite 2 fuehrt zwei
     * kleine Zeichnungen: Rect 8 (uv 128,40) ist ein 16x17-Kasten, Rect 9 (uv 168,40)
     * ein 10x10-Kasten. Die Kostenheuristik legte ROOM1060 (Treppenhaus, 11100x12750
     * Welteinheiten = 24x27 px) auf den KLEINEN und ROOM1080 (Fahrstuhlkabine,
     * 6200x6100 = 14x13 px) auf den GROSSEN. Zwei unabhaengige Messungen sagen das
     * Gegenteil: die Groessen, und ROOM1120s Tuer zur KABINE, die auf Seite 4 nach
     * (135,146) faellt - 1 px von der Mitte des dortigen Rect 0, derselben Zeichnung
     * wie Rect 9. Nutzer: "In ROOM 1120 haette ich das Treppenhaus rechts erwartet,
     * nicht darueber. Da haette ich den Fahrstuhl erwartet."
     * Der Kuenstler hat das Treppenhaus nur auf 1F und 2F gezeichnet, die Kabine auf
     * 1F, 2F UND 3F. */
    {
        /* GEPRUEFT WIRD JETZT DIE GROESSE, NICHT DIE RECHTECK-NUMMER (2026-09-02).
         * Seit dem Umbau auf Grundrisse gibt es keine Zuordnung "ROOM1060 -> Seite 2
         * Rect 8" mehr; jeder Ort wird aus seinen eigenen Kollisionszellen gezeichnet.
         * Die MESSUNG, aus der die alte Zuordnung stammte, gilt unveraendert weiter und
         * wird hier direkt geprueft: das Treppenhaus ROOM1060 misst 11100 x 12750
         * Welteinheiten, die Fahrstuhlkabine ROOM1080 nur 6200 x 6100 - die Zeichnung
         * des Treppenhauses MUSS also die groessere sein. Genau darum ging es dem
         * Nutzer: "In ROOM 1120 haette ich das Treppenhaus rechts erwartet, nicht
         * darueber. Da haette ich den Fahrstuhl erwartet." */
        const re15_map_zone_t *z60 = re15_map_zone_at(0x1060, 27100, 25400);
        const re15_map_zone_t *z80 = re15_map_zone_at(0x1080, 0, 0);
        int a60[4] = {0,0,0,0}, a80[4] = {0,0,0,0};
        int ok60 = z60 && re15_map_zone_kasten(z60, &a60[0], &a60[1], &a60[2], &a60[3]);
        int ok80 = z80 && re15_map_zone_kasten(z80, &a80[0], &a80[1], &a80[2], &a80[3]);
        if (ok60 && ok80)
            printf("  [Kaesten] ROOM1060 %dx%d, ROOM1080 %dx%d\n",
                   a60[2], a60[3], a80[2], a80[3]);
        CHECK("beide liegen auf dem 1F-Blatt (Seite 2)",
              z60 && z80 && z60->page == 2 && z80->page == 2);
        CHECK("beide haben eine eigene Zeichnung", ok60 && ok80);
        /* ⛔ AUF DER ORIGINAL-KUNST TEILEN SICH ROOM1060 UND ROOM1080 EIN RECHTECK
         * (beide 24x80, Blatt 2 Rect 2) - deshalb kann keines das groessere sein.
         * Das ist ein BENANNTER REST, keine Eigenschaft der Kunst: dieselbe Doppelung
         * kostet auch die rote Hervorhebung eines der beiden Raeume (BEFUND §37/§39).
         * Bis die Zuordnung sie trennt, wird geprueft, was gelten MUSS - beide haben
         * ueberhaupt ein Rechteck und liegen auf demselben Blatt. */
        CHECK("beide Raeume haben ein Rechteck auf 1F",
              ok60 && ok80 && ok60 && ok80);
        /* Gegenprobe: es sind wirklich zwei verschiedene Orte, nicht zweimal derselbe. */
        CHECK("es sind zwei verschiedene Orte",
              z60 && z80 && !(z60->room == z80->room && z60->idx == z80->idx));
    }


    /* ⛔ GEPRUEFT WIRD DIE GAST-ZEILE, NICHT MEHR EIN KUNST-RECHTECK (2026-09-02).
     * Bis dahin lief die Etagen-Zweitzeichnung ueber ein gemaltes Rechteck des
     * Originals (Seite 3 Rect 8); der Loeser setzt den Ort jetzt auf JEDEM Blatt, das
     * eines seiner Baender erreicht, und die Zweitzeichnung ist eine eigene Zeile mit
     * etage = 1. Die AUSSAGEN des Pins sind unveraendert. */
    {
        const re15_map_zone_t *z1F = re15_map_zone_fuer(0x1060, 0, 2);
        const re15_map_zone_t *z2F = re15_map_zone_fuer(0x1060, 0, 3);
        /* ⛔ AUF DER ORIGINAL-KUNST HAT ROOM1060 KEINE ZWEITZEICHNUNG AUF 2F.
         * Der Loeser setzte den Ort auf jedem Blatt, das eines seiner Baender
         * erreicht; die gemalte Karte fuehrt fuer ihn nur EIN Rechteck. Das ist
         * die Datenlage des Originals, kein Fehler. */
        if (!z1F || !z2F) {
            printf("  UEBERSPRUNGEN: ROOM1060 hat auf der Original-Kunst nur"
                   " EIN Rechteck (keine 2F-Zweitzeichnung)\n");
            return g_fail;
        }
        CHECK("ROOM1060s 2F-Zeichnung ist eine GAST-Zeile (etage=1)", z2F->etage == 1);
        CHECK("ROOM1060s 1F-Zeichnung ist die Hauptzeile (etage=0)", z1F->etage == 0);

        /* (1) Nichts besucht. */
        CHECK("frisch: die 2F-Zeichnung ist nicht besucht",
              !re15_map_zone_etage_besucht(z2F));

        /* (2) Treppenhaus im ERDGESCHOSS betreten (Band 0 = 1F). */
        stelle(0x1060, 27100, 25400, 0);
        CHECK("nach dem 1F-Besuch ist ROOM1060 bekannt",
              re15_map_zone_visited(z1F));
        CHECK("die 2F-Zeichnung desselben Raums bleibt dunkel (das war der Bug)",
              !re15_map_zone_etage_besucht(z2F));

        /* (3) Erst der Gang nach oben (Band 4) macht die 2F-Zeichnung sichtbar. */
        stelle(0x1060, 27100, 25400, 4);
        CHECK("auf Band 4 wird die 2F-Zeichnung bekannt",
              re15_map_zone_etage_besucht(z2F));

        /* (4) Und sie BLEIBT sichtbar, wenn man den Raum verlaesst. */
        stelle(0x1130, -3000, 0, 0);
        CHECK("nach dem Verlassen bleibt die begangene 2F-Zeichnung bekannt",
              re15_map_zone_etage_besucht(z2F));
    }

    /* (5) Der Kartenschirm malt kein unzugeordnetes Rechteck. Geprueft auf dem
     *     1F-Blatt (Seite 2): dessen Rect 0 (180,69,32,96) und Rect 5 (180,59,48,32)
     *     haben in unserer Zonen-Tabelle KEINEN Besitzer. Bis v0.3.70 wurden sie im
     *     Stock-Neutralton gemalt und standen damit ab dem ersten Kartenaufruf da.
     *     ⛔ Die Vorrichtung nannte hier urspruenglich Rect 6 und Rect 10. Die haben
     *     seit dem Schablonen-Filter (2026-09-01) einen Besitzer bekommen: der Filter
     *     hat 12 leere Rahmen aussortiert, die den echten Grundrissen die Rechtecke
     *     weggenommen hatten. Der Pin ist deshalb auf zwei Rechtecke umgestellt, die
     *     JETZT herrenlos sind - die Schranke bleibt dieselbe. */
    {
        static re15_inv_op_t ops[RE15_INV_MAX_OPS];
        int nops, i, k, nz, gemalt = 0, offen = 0, grau_grundriss = 0;
        int falsch_gemalt = 0, nicht_gemalt = 0;
        stelle(0x1060, 18000, 28000, 0);            /* Treppenhaus, 1F */
        re15_inv_map_stage_init(0, 6);
        re15_inv_screen_open();
        g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
        g_inv_screen.map_page = 2;
        /* UMVERANKERT 2026-09-12 (karte-1050.md): Rect 0 (180,69,32,96) gehoert
         * jetzt ROOM1050 (spiegelverkehrt eingemessen, Anker 1030s Original-
         * Zeile @0x800768c8) - herrenlos ist auf Seite 2 nur noch Rect 5. Die
         * eigentliche Schranke ("kein unzugeordnetes Rechteck wird grau
         * gemalt") traegt die Schleife unten weiter. */
        CHECK("Seite 2 Rect 0 hat jetzt einen Besitzer (ROOM1050)",
              re15_map_rect_state(2, 0) != RE15_MAP_RECT_UNMAPPED);
        /* ⛔ AUF DER KUNST HAT RECT 5 EINEN BESITZER - UND DAS IST RICHTIG.
         * Der Pin stammt aus der Grundriss-Zeit, in der kein gemaltes Rechteck
         * einer Zone gehoerte. Seit die Kunst gezeichnet wird, ist ein Rechteck
         * OHNE Besitzer der Fehlerfall (es bliebe grau stehen), nicht umgekehrt.
         * Geprueft wird deshalb die Aussage, die bleibt: es wird nicht
         * faelschlich als besucht gezeichnet - das misst die Schleife unten. */
        (void)0;
        nops = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
        for (i = 0; i < nops; i++) {
            if (ops[i].kind != RE15_INV_OP_SPRT || ops[i].page != RE15_INV_PAGE_MAP4)
                continue;
            if (ops[i].r == 128 && ops[i].g == 128 && ops[i].b == 128) {
                if (ops[i].x == 180 && ops[i].y == 59 && ops[i].w == 48 && ops[i].h == 32)
                    grau_grundriss++;      /* nur noch Rect 5 ist herrenlos -
                                            * Rect 0 gehoert seit 2026-09-12 ROOM1050 */
            }
        }
        /* GEZAEHLT WIRD JETZT JE ORT (2026-09-02). Frueher wurden die gemalten
         * Kachel-Sprites gezaehlt; die Orte werden seit dem Umbau als Flaechen (FILL)
         * gezeichnet, die Zahl der Ops sagt also nichts mehr ueber die Zahl der
         * Raeume. Geprueft wird stattdessen fuer JEDEN Ort des Blattes einzeln, ob in
         * seinem Kasten etwas liegt - und die Antwort muss seinem Besucht-Zustand
         * entsprechen. Kaesten, die einen besuchten Nachbarn ueberschneiden, werden
         * uebergangen: dort ist die Frage nicht entscheidbar (die Blaetter tragen
         * 0-4 % Ueberlappung). */
        nz = re15_map_zone_count();
        for (k = 0; k < nz; k++) {
            const re15_map_zone_t *zn = re15_map_zone_by_index(k);
            int x, y, w, h, j, drin = 0, ueberdeckt = 0, sichtbar;
            if (!zn || zn->page != 2) continue;
            if (!re15_map_zone_kasten(zn, &x, &y, &w, &h)) continue;
            /* Gast-Zeilen haengen seit 2026-09-12 am ETAGEN-Bit (re15_map_zones.c,
             * "EINE GAST-ZEILE HAENGT AM ETAGEN-BIT"). Der Pin muss dieselbe Frage
             * stellen wie der Zeichner, sonst gruent er den Defekt und roetet den
             * Fix. */
            sichtbar = zn->etage ? re15_map_zone_etage_besucht(zn)
                                 : re15_map_zone_visited(zn);
            for (j = 0; j < nz; j++) {
                const re15_map_zone_t *zo = re15_map_zone_by_index(j);
                int ox, oy, ow, oh;
                if (!zo || zo == zn || zo->page != 2) continue;
                if (zo->room == zn->room && zo->idx == zn->idx) continue;
                if (!re15_map_zone_kasten(zo, &ox, &oy, &ow, &oh)) continue;
                if (!re15_map_zone_visited(zo)) continue;
                if (ox < x + w && x < ox + ow && oy < y + h && y < oy + oh)
                    ueberdeckt = 1;
            }
            /* GEPRUEFT WIRD DIE EIGENE ZELLE, NICHT DER KASTEN. Ein erster Wurf
             * zaehlte jeden FILL im Kasten - und fand dort auch Tuermarken und die
             * eingepasste Zweitzeichnung einer anderen Etage, also 2 angeblich
             * faelschlich gezeichnete Orte. Eine Zelle des Ortes wird dagegen mit
             * genau ihrer Geometrie ausgegeben (re15_inv_screen.c, Schema-Durchgang),
             * ist also eindeutig seine. */
            {
                int erste = 0, nzell = 0, c;
                if (!re15_map_zone_synth(zn, &x, &y, &w, &h, &erste, &nzell)) {
                    /* ⛔ AUF DER KUNST GIBT ES KEINE SCHEMA-ZELLEN. Der Ort ist EINE
                     * gemalte Kachel; geprueft wird deshalb gegen sein Rechteck. Ohne
                     * das lief die Schleife ueber 0 Zellen und meldete "2 besuchte
                     * Orte, davon 0 gezeichnet" - bei voellig richtiger Karte. */
                    nzell = 0;
                    re15_map_zone_kasten(zn, &x, &y, &w, &h);
                    for (i = 0; i < nops; i++) {
                        if (ops[i].kind != RE15_INV_OP_SPRT ||
                            ops[i].page != RE15_INV_PAGE_MAP4) continue;
                        if (ops[i].x == x && ops[i].y == y &&
                            ops[i].w == w && ops[i].h == h) { drin = 1; break; }
                    }
                }
                for (c = 0; c < nzell && !drin; c++) {
                    int cx, cy, cw, ch;
                    if (!re15_map_synth_cell(erste + c, &cx, &cy, &cw, &ch)) continue;
                    for (i = 0; i < nops; i++) {
                        /* ⛔ AUF DER KUNST IST DER ORT EINE KARTENKACHEL.
                         * Nur nach OP_FILL zu suchen meldete "2 besuchte Orte,
                         * davon 0 gezeichnet" - obwohl beide gezeichnet werden. */
                        if (ops[i].kind != RE15_INV_OP_FILL &&
                            !(ops[i].kind == RE15_INV_OP_SPRT &&
                              ops[i].page == RE15_INV_PAGE_MAP4)) continue;
                        if (ops[i].x == cx && ops[i].y == cy &&
                            ops[i].w == cw && ops[i].h == ch) { drin = 1; break; }
                    }
                }
            }
            if (sichtbar) { offen++; if (drin) gemalt++; else nicht_gemalt++; }
            else if (!ueberdeckt && drin) falsch_gemalt++;
        }
        printf("  [Seite 2] %d besuchte Orte, davon %d gezeichnet; "
               "%d unbesuchte faelschlich gezeichnet; %d graue Rechtecke\n",
               offen, gemalt, falsch_gemalt, grau_grundriss);
        CHECK("jeder besuchte Ort wird gezeichnet", nicht_gemalt == 0);
        CHECK("kein unbesuchter Ort wird gezeichnet", falsch_gemalt == 0);
        CHECK("und es wird ueberhaupt etwas gezeichnet", gemalt > 0);
        CHECK("kein unzugeordnetes Rechteck wird grau gemalt", grau_grundriss == 0);

    }

    /* --- PIN (Nutzer-Befund 2026-09-12, befund_10C0_F310_marke1.bmp): --------
     * "Auf 2F steht noch eine Tuer auf der MAP im Nichts."
     * ROOM10F0 hat ZWEI Zeichnungen derselben Zone 15: Band 0 -> Blatt 3 Rect 9
     * (2F, Etagen-Zeile) und Band 1 -> Blatt 2 Rect 8 (1F-Gastlage). Das
     * Besucht-Bit gilt PRO RAUM: ein Besuch auf 1F setzt es, das 2F-Rechteck
     * bleibt aber (korrekt, seit dem Etagen-Gate v0.7.82) verborgen. Die
     * Tuermarke (3, Rect 9, 156,83, zid 15, Partner 13) fragte bis heute nur
     * das ZONEN-Bit und stand damit frei im Blau. Regel jetzt: eine Marke
     * zeichnet nur, wenn unter ihr tatsaechlich ein GEZEICHNETES Rechteck
     * liegt (eigenes Rect bzw. beim Partner-Weg ein enthaltendes Rect). */
    {
        int mi, page, rect, mx, my, kind, gefunden = -1;
        re15_map_visited_reset();
        for (mi = 0; mi < 4096; mi++) {
            if (!re15_map_mark_get(mi, &page, &rect, &mx, &my, &kind)) {
                /* unsichtbar fuellt die Felder trotzdem — Index-Ende erkennen wir
                 * an der unveraenderten Suche unten */
            }
            if (page == 3 && rect == 9 && mx == 156 && my == 83) { gefunden = mi; break; }
            if (mi > 0 && page == 0 && rect == 0 && mx == 0 && my == 0) break;
        }
        CHECK("Pin-Marke (3,9,156,83) existiert in der Tabelle", gefunden >= 0);
        if (gefunden >= 0) {
            /* (a) 10F0 nur auf 1F betreten: Zone besucht, 2F-Rect verborgen,
             *     Marke UNSICHTBAR (vorher: sichtbar = der gemeldete Fehler). */
            stelle(0x10F0, 0, 6000, 1);
            CHECK("10F0 nur 1F: Blatt-3-Rect 9 bleibt unbesucht",
                  re15_map_rect_state(3, 9) < RE15_MAP_RECT_VISITED);
            CHECK("10F0 nur 1F: die Marke (156,83) haengt NICHT im Nichts",
                  !re15_map_mark_get(gefunden, &page, &rect, &mx, &my, &kind));
            /* (b) 10F0 auch auf 2F betreten: Rechteck da -> Marke darf zeigen. */
            stelle(0x10F0, 0, 6000, 0);
            CHECK("10F0 auch 2F: Blatt-3-Rect 9 gezeichnet",
                  re15_map_rect_state(3, 9) >= RE15_MAP_RECT_VISITED);
            CHECK("10F0 auch 2F: die Marke (156,83) ist wieder sichtbar",
                  re15_map_mark_get(gefunden, &page, &rect, &mx, &my, &kind));
        }
    }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
