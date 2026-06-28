/**
 * @file camera_common.c
 * @brief Kamerazone-Dispatch — Prüft Spielerposition gegen RVD-Trigger-Zonen
 *
 * Iteriert alle RVD-Zonen und prüft, ob der Spieler eine Zonengrenze
 * überschritten hat. Bei Treffer wird die aktive Kamera-ID umgeschaltet.
 * Der zugehörige BSS-Hintergrund-Chunk wird vom Aufrufer (Raum-Manager)
 * bei Kamerawechsel geladen.
 *
 * Verwendet denselben Cross-Product Point-in-4P-Polygon-Test wie aot_common.c.
 */

#include "re15_camera.h"

/* ============================================================================
 * Point-in-4P-Polygon Hilfsfunktion (Cross-Product-Test)
 *
 * Prüft ob Punkt (px, pz) innerhalb des konvexen 4-Punkt-Polygons liegt.
 * Verwendet den Kreuzprodukt-Test über alle 4 Kanten: Punkt muss auf
 * derselben Seite aller Kanten liegen (konsistentes Vorzeichen).
 *
 * Identische Logik wie in aot_common.c — bewusst dupliziert statt
 * Shared-Library, um PSX-Link-Einfachheit zu bewahren.
 * ========================================================================= */

static int point_in_4p(int16_t px, int16_t pz,
                       const int16_t* poly_x, const int16_t* poly_z)
{
    int i;
    int sign = 0;

    for (i = 0; i < 4; i++) {
        int next = (i + 1) & 3;
        int32_t ex = (int32_t)poly_x[next] - (int32_t)poly_x[i];
        int32_t ez = (int32_t)poly_z[next] - (int32_t)poly_z[i];
        int32_t dx = (int32_t)px - (int32_t)poly_x[i];
        int32_t dz = (int32_t)pz - (int32_t)poly_z[i];
        int32_t cross = ex * dz - ez * dx;

        if (cross == 0) {
            continue; /* Auf der Kante — zählt als "drin" */
        }

        if (sign == 0) {
            sign = (cross > 0) ? 1 : -1;
        } else {
            if ((cross > 0 && sign < 0) || (cross < 0 && sign > 0)) {
                return 0; /* Außerhalb */
            }
        }
    }

    return 1; /* Innerhalb */
}

/* ============================================================================
 * re15_camera_check_zones — Kamerazone-Dispatch
 *
 * Iteriert alle RVD-Zonen. Für jede Zone mit cam_from == current_cam
 * wird geprüft, ob der Spieler (x, z) im 4-Punkt-Polygon liegt.
 * Bei Treffer: *out_new_cam = cam_to, sofortiger Return.
 * Kein Treffer: *out_new_cam = current_cam (keine Änderung).
 * ========================================================================= */

void re15_camera_check_zones(int16_t player_x, int16_t player_z,
                             const re15_rvd_data_t* rvd,
                             uint8_t current_cam,
                             uint8_t* out_new_cam)
{
    uint16_t i;
    const re15_rvd_zone_t* zone;

    /* Sicherheitscheck: ungültige Parameter → keine Änderung */
    if (!rvd || !out_new_cam) {
        if (out_new_cam) {
            *out_new_cam = current_cam;
        }
        return;
    }

    /* Alle Zonen iterieren */
    for (i = 0; i < rvd->count; i++) {
        zone = &rvd->zones[i];

        /* Nur Zonen prüfen, die von der aktuellen Kamera ausgehen */
        if (zone->cam_from != current_cam) {
            continue;
        }

        /* Point-in-Polygon: Spieler innerhalb der Trigger-Zone? */
        if (point_in_4p(player_x, player_z,
                        zone->trigger_x, zone->trigger_z)) {
            *out_new_cam = zone->cam_to;
            return;
        }
    }

    /* Keine Zone getroffen — Kamera bleibt unverändert */
    *out_new_cam = current_cam;
}
