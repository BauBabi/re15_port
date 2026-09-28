/* probe_r30_karte-3010_ziel.c - MESSSONDE Runde 30, Thema B, zweite Messung.
 *
 * KEIN RIEGEL, NUR MESSUNG: wo liegt der Zielraum des Kartenhinweises (ROOM10F0,
 * "COMMUNIC. ROOM", DEBUG.BIN @0x027C0) auf der Karte des Ports, und in welchem ZUSTAND
 * saehe ihn der normale Kartenzeichner, wenn der Spieler direkt nach der Irons-Szene in
 * ROOM1150 steht und den Funkraum noch nie betreten hat?
 *
 * Gelesen wird ausschliesslich ueber die oeffentliche Karten-API (include/re15_room.h,
 * include/re15_map_owned.h) - dieselben Funktionen, die der Zeichner in
 * engine/src/re15_inv_screen.c benutzt.
 *
 * Aufruf: probe_r30_karte-3010_ziel   (stdout; Exit 0 = Messung gelaufen)
 */
#include <stdint.h>
#include <stdio.h>

#include "re15_actor.h"
#include "re15_room.h"
#include "re15_map_owned.h"

extern unsigned g_current_room_id;

static const char *zname(int s)
{
    switch (s) {
    case RE15_MAP_RECT_UNMAPPED:  return "UNMAPPED";
    case RE15_MAP_RECT_UNVISITED: return "UNVISITED";
    case RE15_MAP_RECT_VISITED:   return "VISITED";
    case RE15_MAP_RECT_CURRENT:   return "CURRENT";
    }
    return "?";
}

static void zeile(const char *was, unsigned room, unsigned page)
{
    const re15_map_zone_t *zn = re15_map_zone_fuer(room, 0, page);
    int x = -1, y = -1, w = -1, h = -1, u = -1, v = -1;
    if (!zn) { printf("[Z] %s ROOM%04X Blatt %u: KEINE Zeile\n", was, room, page); return; }
    re15_map_rect_geometry(zn->page, zn->rect, &x, &y, &w, &h);
    re15_map_rect_uv(zn->page, zn->rect, &u, &v);
    printf("[Z] %s ROOM%04X idx %d: Blatt %d Rechteck %d zid %d etage=%d  Welt x %ld..%ld z %ld..%ld\n",
           was, room, (int)zn->idx, (int)zn->page, (int)zn->rect, (int)zn->zid, (int)zn->etage,
           (long)zn->wx0, (long)zn->wx1, (long)zn->wz0, (long)zn->wz1);
    printf("[Z]    Schirm (%d,%d) %dx%d  uv (%d,%d)  Teilbereiche %d\n",
           x, y, w, h, u, v, re15_map_teil_count(zn->page, zn->rect));
}

int main(void)
{
    re15_actor_init();
    re15_map_visited_reset();

    /* Der Spieler steht in Irons' Buero, an der Stelle, an der sub08 ihn abstellt
     * (Sonde probe_r30_karte-3010, [M2]: Spieler=(-20529,-25094)). */
    g_current_room_id = 0x1150;
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = -20529;
    g_actors[RE15_ACTOR_SLOT_PLAYER].y = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = -25094;
    re15_map_zone_update(0x1150, -20529, -25094);

    zeile("Spieler", 0x1150, 4);
    zeile("Ziel   ", 0x10F0, 3);
    zeile("Ziel/E ", 0x10F1, 3);
    zeile("Gast   ", 0x10F0, 2);

    {
        const re15_map_zone_t *cur = re15_map_zone_current();
        printf("[S] aktuelle Zone: ROOM%04X Blatt %d Rechteck %d\n",
               cur ? (unsigned)cur->room : 0u, cur ? (int)cur->page : -1, cur ? (int)cur->rect : -1);
    }
    printf("[S] Blatt 4 Rechteck 2 (Irons' Buero): Zustand %s\n", zname(re15_map_rect_state(4, 2)));
    printf("[S] Blatt 3 Rechteck 9 (Funkraum)    : Zustand %s\n", zname(re15_map_rect_state(3, 9)));
    printf("[S] Besitz Blatt 3: %d   Besitz Blatt 4: %d   Besitz-Bits 0x%08lX\n",
           re15_map_owned_page(3), re15_map_owned_page(4), (unsigned long)re15_map_owned_bits());
    printf("[S] Blatt 3 bekannt (blaetterbar): %d   Blatt 4 bekannt: %d\n",
           re15_map_page_known(3), re15_map_page_known(4));
    printf("[S] Rechtecke auf Blatt 3: %d\n", re15_map_rect_count(3));
    {
        int i, n = re15_map_rect_count(3), sichtbar = 0;
        for (i = 0; i < n; i++) {
            int s = re15_map_rect_state(3, (unsigned)i);
            int gez = (s == RE15_MAP_RECT_VISITED || s == RE15_MAP_RECT_CURRENT ||
                       (s == RE15_MAP_RECT_UNVISITED && re15_map_owned_page(3)));
            if (gez) sichtbar++;
            printf("[S]    Blatt 3 Rechteck %2d: %-9s -> normaler Zeichner %s\n",
                   i, zname(s), gez ? "ZEICHNET" : "ueberspringt");
        }
        printf("[S] vom normalen Zeichner gezeichnete Rechtecke auf Blatt 3: %d von %d\n", sichtbar, n);
    }
    return 0;
}
