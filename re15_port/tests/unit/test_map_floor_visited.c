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
        const re15_map_zone_t *z60 = re15_map_zone_at(0x1060, 27100, 25400);
        const re15_map_zone_t *z80 = re15_map_zone_at(0x1080, 0, 0);
        CHECK("ROOM1060 (Treppenhaus) liegt auf dem GROSSEN Kasten, Seite 2 Rect 8",
              z60 && z60->page == 2 && z60->rect == 8);
        CHECK("ROOM1080 (Fahrstuhl) liegt auf dem KLEINEN Kasten, Seite 2 Rect 9",
              z80 && z80->page == 2 && z80->rect == 9);
    }

    /* (1) Nichts besucht: das Treppenhaus ist auf dem 2F-Blatt noch nicht bekannt. */
    CHECK("frisch: Seite 3 Rect 8 ist nicht besucht",
          re15_map_rect_state(3, 8) != RE15_MAP_RECT_VISITED &&
          re15_map_rect_state(3, 8) != RE15_MAP_RECT_CURRENT);

    /* (2) Treppenhaus im ERDGESCHOSS betreten (Band 0 -> Seite 2 Rect 8). */
    stelle(0x1060, 27100, 25400, 0);
    CHECK("nach dem 1F-Besuch ist ROOM1060 auf dem 1F-Blatt aktuell",
          re15_map_rect_state(2, 8) == RE15_MAP_RECT_CURRENT);
    CHECK("die 2F-Zeichnung desselben Raums bleibt dunkel (das war der Bug)",
          re15_map_rect_state(3, 8) != RE15_MAP_RECT_VISITED &&
          re15_map_rect_state(3, 8) != RE15_MAP_RECT_CURRENT);

    /* (3) Erst der Gang nach oben (Band 4) macht die 2F-Zeichnung sichtbar. */
    stelle(0x1060, 27100, 25400, 4);
    CHECK("auf Band 4 ist ROOM1060 auf dem 2F-Blatt aktuell",
          re15_map_rect_state(3, 8) == RE15_MAP_RECT_CURRENT);

    /* (4) Und sie BLEIBT sichtbar, wenn man den Raum verlaesst. */
    stelle(0x1130, -3000, 0, 0);
    CHECK("nach dem Verlassen bleibt die begangene 2F-Zeichnung gruen",
          re15_map_rect_state(3, 8) == RE15_MAP_RECT_VISITED);

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
        static re15_inv_op_t ops[768];
        int nops, i, gemalt = 0, erlaubt = 0, grau_grundriss = 0;
        stelle(0x1060, 18000, 28000, 0);            /* Treppenhaus, 1F */
        re15_inv_map_stage_init(0, 6);
        re15_inv_screen_open();
        g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
        g_inv_screen.map_page = 2;
        CHECK("Seite 2 Rect 0 hat keinen Besitzer",
              re15_map_rect_state(2, 0) == RE15_MAP_RECT_UNMAPPED);
        CHECK("Seite 2 Rect 5 hat keinen Besitzer",
              re15_map_rect_state(2, 5) == RE15_MAP_RECT_UNMAPPED);
        for (i = 0; i < 11; i++) {
            int rs = re15_map_rect_state(2, (unsigned)i);
            if (rs == RE15_MAP_RECT_VISITED || rs == RE15_MAP_RECT_CURRENT) erlaubt++;
        }
        nops = re15_inv_screen_build(&g_inv_screen, ops, 768);
        for (i = 0; i < nops; i++) {
            int r_, g_, b_;
            if (ops[i].kind != RE15_INV_OP_SPRT || ops[i].page != RE15_INV_PAGE_MAP4)
                continue;
            r_ = ops[i].r; g_ = ops[i].g; b_ = ops[i].b;
            /* Nur besucht/aktuell zaehlen. Der Stock-Neutralton 128/128/128 taugt
             * hier NICHT als Merkmal: Titelkachel (30,30,88,32) und Legende
             * (270,40,32,48) liegen auf derselben Seite und tragen denselben Wert -
             * deshalb werden die Grauen an ihrer GEOMETRIE geprueft. */
            if ((r_ == 40 && g_ == 144 && b_ == 40) ||
                (r_ == 192 && g_ == 24 && b_ == 24))
                gemalt++;
            else if (r_ == 128 && g_ == 128 && b_ == 128) {
                if ((ops[i].x == 180 && ops[i].y == 69 && ops[i].w == 32 && ops[i].h == 96) ||
                    (ops[i].x == 180 && ops[i].y == 59 && ops[i].w == 48 && ops[i].h == 32))
                    grau_grundriss++;
            }
        }
        printf("  [Seite 2] %d Rechtecke gemalt, %d zustands-erlaubt, %d grau\n",
               gemalt, erlaubt, grau_grundriss);
        CHECK("es wird genau so viel gemalt, wie besucht/aktuell ist", gemalt == erlaubt);
        CHECK("und es wird ueberhaupt etwas gemalt", gemalt > 0);
        CHECK("kein unzugeordnetes Rechteck wird grau gemalt", grau_grundriss == 0);
    }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
