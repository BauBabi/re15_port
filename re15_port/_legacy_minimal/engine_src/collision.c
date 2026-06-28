/**
 * @file collision.c
 * @brief SCA-Kollisionsprüfung — Spieler-Zylinder vs. Raum-Geometrie
 *
 * Prüft den zylindrischen Spieler-Kollisionskörper (Position x/z + Radius)
 * gegen alle SCA-Einträge des aktuellen Raums. Unterstützte Typen:
 *
 *   Typ 1  — Zylinder vs. Rechteck (AABB): Nearest-Edge-Push
 *   Typ 3  — Zylinder vs. Kreis: Radialer Push
 *   Typ 12 — Treppe aufwärts: Y-Versatz +0x708 pro Band
 *   Typ 13 — Treppe abwärts: Y-Versatz -0x708 pro Band
 *
 * Alle Berechnungen verwenden Integer/Fixpoint-Arithmetik (kein Float).
 *
 * Validates: Requirements 11.3
 */

#include "re15_collision.h"
#include "re15_player.h"
#include "re15_types.h"

/* ============================================================================
 * Konstanten
 * ========================================================================= */

/** Y-Versatz pro Treppen-Band-Übergang (Fixkomma) */
#define RE15_STAIR_Y_STEP  0x708

/* ============================================================================
 * Hilfsfunktionen
 * ========================================================================= */

/**
 * Berechnet den absoluten Betrag eines int32_t.
 */
static int32_t iabs(int32_t v)
{
    return v < 0 ? -v : v;
}

/**
 * Ganzzahlige Näherung der Quadratwurzel (Integer Square Root).
 * Verwendet iterativen Algorithmus — ausreichend für Kollision.
 */
static int32_t isqrt(int32_t val)
{
    int32_t result = 0;
    int32_t bit = 1 << 30; /* Höchstes Bit, das eine Quadratzahl ergibt */

    if (val <= 0) {
        return 0;
    }

    /* Finde das höchste Bit <= val */
    while (bit > val) {
        bit >>= 2;
    }

    /* Standard-Integer-Sqrt via Subtraktions-Methode */
    while (bit != 0) {
        if (val >= result + bit) {
            val -= result + bit;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }
        bit >>= 2;
    }

    return result;
}

/* ============================================================================
 * Typ 1: Zylinder vs. Rechteck (AABB) Kollisionsprüfung
 *
 * Das Rechteck ist ein achsenparalleles Bounding-Box (AABB) definiert durch:
 *   X: [corner_x, corner_x + width]
 *   Z: [corner_z, corner_z + depth]
 *
 * Prüfung: Finde den nächsten Punkt auf dem Rechteck zum Spieler-Mittelpunkt.
 * Wenn Abstand < Radius → Durchdringung. Push-Richtung = Normale der
 * nächsten Kante.
 * ========================================================================= */

static void collision_check_rectangle(re15_player_t* player,
                                      const re15_sca_entry_t* entry)
{
    int32_t px = player->x;
    int32_t pz = player->z;
    int32_t radius = (int32_t)player->radius;

    /* Rechteck-Grenzen */
    int32_t rect_x_min = (int32_t)entry->corner_x;
    int32_t rect_z_min = (int32_t)entry->corner_z;
    int32_t rect_x_max = rect_x_min + (int32_t)entry->width;
    int32_t rect_z_max = rect_z_min + (int32_t)entry->depth;

    /* Abstände zu jeder Kante (positiv = außerhalb, negativ = innerhalb) */
    int32_t dist_left   = rect_x_min - px;  /* Positive wenn Spieler links */
    int32_t dist_right  = px - rect_x_max;  /* Positive wenn Spieler rechts */
    int32_t dist_top    = rect_z_min - pz;  /* Positive wenn Spieler oben (Z-) */
    int32_t dist_bottom = pz - rect_z_max;  /* Positive wenn Spieler unten (Z+) */

    /* Spieler-Mittelpunkt relativ zum Rechteck */
    int32_t inside_x = (px >= rect_x_min && px <= rect_x_max);
    int32_t inside_z = (pz >= rect_z_min && pz <= rect_z_max);

    if (inside_x && inside_z) {
        /*
         * Spieler-Mittelpunkt ist INNERHALB des Rechtecks.
         * Finde nächste Kante und schiebe Spieler nach außen.
         */
        int32_t pen_left   = px - rect_x_min + radius;
        int32_t pen_right  = rect_x_max - px + radius;
        int32_t pen_top    = pz - rect_z_min + radius;
        int32_t pen_bottom = rect_z_max - pz + radius;

        /* Finde minimale Durchdringung → nächste Kante */
        int32_t min_pen = pen_left;
        int32_t push_x = -(pen_left);  /* Push nach links (X-) */
        int32_t push_z = 0;

        if (pen_right < min_pen) {
            min_pen = pen_right;
            push_x = pen_right;   /* Push nach rechts (X+) */
            push_z = 0;
        }
        if (pen_top < min_pen) {
            min_pen = pen_top;
            push_x = 0;
            push_z = -(pen_top);  /* Push nach oben (Z-) */
        }
        if (pen_bottom < min_pen) {
            min_pen = pen_bottom;
            push_x = 0;
            push_z = pen_bottom;  /* Push nach unten (Z+) */
        }

        player->x += push_x;
        player->z += push_z;

    } else if (inside_x) {
        /*
         * Spieler ist innerhalb des X-Bereichs, aber außerhalb des Z-Bereichs.
         * Nur Z-Kanten-Prüfung nötig.
         */
        if (dist_top > 0 && dist_top < radius) {
            /* Oben: Spieler-Z < rect_z_min, aber Radius überlappt */
            player->z = rect_z_min - radius;
        } else if (dist_bottom > 0 && dist_bottom < radius) {
            /* Unten: Spieler-Z > rect_z_max, aber Radius überlappt */
            player->z = rect_z_max + radius;
        }

    } else if (inside_z) {
        /*
         * Spieler ist innerhalb des Z-Bereichs, aber außerhalb des X-Bereichs.
         * Nur X-Kanten-Prüfung nötig.
         */
        if (dist_left > 0 && dist_left < radius) {
            /* Links: Spieler-X < rect_x_min, aber Radius überlappt */
            player->x = rect_x_min - radius;
        } else if (dist_right > 0 && dist_right < radius) {
            /* Rechts: Spieler-X > rect_x_max, aber Radius überlappt */
            player->x = rect_x_max + radius;
        }

    } else {
        /*
         * Spieler ist außerhalb beider Achsen → Ecken-Prüfung.
         * Finde nächste Ecke des Rechtecks und prüfe Abstand.
         */
        int32_t closest_x = (px < rect_x_min) ? rect_x_min : rect_x_max;
        int32_t closest_z = (pz < rect_z_min) ? rect_z_min : rect_z_max;

        int32_t dx = px - closest_x;
        int32_t dz = pz - closest_z;

        /* Abstand² (vermeidet Quadratwurzel wenn möglich) */
        int32_t dist_sq = dx * dx + dz * dz;
        int32_t rad_sq  = radius * radius;

        if (dist_sq < rad_sq && dist_sq > 0) {
            /* Ecken-Überlappung: Push entlang der Verbindungslinie */
            int32_t dist = isqrt(dist_sq);
            int32_t penetration = radius - dist;

            /* Normalisierte Richtung (skaliert mit Penetration/dist) */
            player->x += (dx * penetration) / dist;
            player->z += (dz * penetration) / dist;
        }
    }
}

/* ============================================================================
 * Typ 3: Zylinder vs. Kreis Kollisionsprüfung
 *
 * Kreis definiert durch:
 *   Mittelpunkt: (corner_x, corner_z)
 *   Radius: width
 *
 * Kollision wenn: Abstand(Spieler, Kreismitte) < Spieler.radius + Kreis.radius
 * Push: Entlang der Verbindungslinie zwischen Zentren nach außen.
 * ========================================================================= */

static void collision_check_circle(re15_player_t* player,
                                   const re15_sca_entry_t* entry)
{
    int32_t px = player->x;
    int32_t pz = player->z;
    int32_t player_radius = (int32_t)player->radius;

    int32_t cx = (int32_t)entry->corner_x;
    int32_t cz = (int32_t)entry->corner_z;
    int32_t circle_radius = (int32_t)entry->width;

    /* Vektor vom Kreismittelpunkt zum Spieler */
    int32_t dx = px - cx;
    int32_t dz = pz - cz;

    /* Kombinierter Radius (Spieler + Kreis) */
    int32_t combined_radius = player_radius + circle_radius;

    /* Abstand² */
    int32_t dist_sq = dx * dx + dz * dz;
    int32_t combined_sq = combined_radius * combined_radius;

    if (dist_sq < combined_sq && dist_sq > 0) {
        /* Durchdringung: Push Spieler entlang Richtungsvektor nach außen */
        int32_t dist = isqrt(dist_sq);
        int32_t penetration = combined_radius - dist;

        /* Push-Richtung: normalisiert * Penetration */
        player->x += (dx * penetration) / dist;
        player->z += (dz * penetration) / dist;
    } else if (dist_sq == 0) {
        /*
         * Spieler exakt auf Kreismittelpunkt — keine definierte Richtung.
         * Push in +X-Richtung als Fallback.
         */
        player->x += combined_radius;
    }
}

/* ============================================================================
 * Typ 12/13: Treppen-Kollision (Y-Versatz pro Band)
 *
 * Treppen sind als Rechtecke definiert (gleiche Geometrie wie Typ 1).
 * Wenn der Spieler innerhalb des Treppen-Rechtecks ist:
 *   Typ 12 (up):   Y += 0x708 pro Band-Übergang
 *   Typ 13 (down): Y -= 0x708 pro Band-Übergang
 *
 * Der density-Wert gibt das Ziel-Band (floor_band) an.
 * ========================================================================= */

static void collision_check_stair(re15_player_t* player,
                                  const re15_sca_entry_t* entry)
{
    int32_t px = player->x;
    int32_t pz = player->z;

    /* Treppen-Rechteck-Grenzen */
    int32_t rect_x_min = (int32_t)entry->corner_x;
    int32_t rect_z_min = (int32_t)entry->corner_z;
    int32_t rect_x_max = rect_x_min + (int32_t)entry->width;
    int32_t rect_z_max = rect_z_min + (int32_t)entry->depth;

    /* Prüfe ob Spieler-Mittelpunkt innerhalb des Treppen-Rechtecks ist */
    if (px < rect_x_min || px > rect_x_max) {
        return;
    }
    if (pz < rect_z_min || pz > rect_z_max) {
        return;
    }

    /* Spieler ist auf der Treppe — Band-Übergang berechnen */
    uint8_t target_band = (uint8_t)(entry->density & 0xFF);

    if (player->floor_band != target_band) {
        /* Band-Differenz bestimmt Anzahl der Stufen */
        int32_t band_diff;

        if (entry->type == RE15_SCA_TYPE_STAIR_UP) {
            /* Treppe aufwärts: Y erhöhen */
            band_diff = (int32_t)target_band - (int32_t)player->floor_band;
            if (band_diff > 0) {
                player->y += RE15_STAIR_Y_STEP * band_diff;
            } else {
                /* Spieler kommt von oben — ein Band runter */
                player->y += RE15_STAIR_Y_STEP;
            }
        } else {
            /* Treppe abwärts (Typ 13): Y verringern */
            band_diff = (int32_t)player->floor_band - (int32_t)target_band;
            if (band_diff > 0) {
                player->y -= RE15_STAIR_Y_STEP * band_diff;
            } else {
                /* Spieler kommt von unten — ein Band hoch */
                player->y -= RE15_STAIR_Y_STEP;
            }
        }

        /* Band-Wert aktualisieren */
        player->floor_band = target_band;
    }
}

/* ============================================================================
 * Öffentliche API: re15_collision_check
 * ========================================================================= */

/**
 * Prüft den Spieler-Kollisionskörper gegen alle SCA-Einträge.
 *
 * Iteriert über alle Einträge der SCA-Daten und führt typspezifische
 * Kollisionsprüfungen durch. Bei Durchdringung wird die Spielerposition
 * korrigiert (Push-Back entlang der Kollisionsnormale).
 *
 * @param player    Spieler-Entität (Position wird bei Kollision modifiziert)
 * @param sca       Geparste SCA-Kollisionsdaten des aktuellen Raums
 */
void re15_collision_check(re15_player_t* player, const re15_sca_data_t* sca)
{
    int i;
    const re15_sca_entry_t* entry;

    if (!player || !sca || !sca->entries) {
        return;
    }

    for (i = 0; i < (int)sca->count; i++) {
        entry = &sca->entries[i];

        switch (entry->type) {
            case RE15_SCA_TYPE_RECTANGLE:
                collision_check_rectangle(player, entry);
                break;

            case RE15_SCA_TYPE_CIRCLE:
                collision_check_circle(player, entry);
                break;

            case RE15_SCA_TYPE_STAIR_UP:
            case RE15_SCA_TYPE_STAIR_DOWN:
                collision_check_stair(player, entry);
                break;

            default:
                /* Unbekannter Typ — ignorieren */
                break;
        }
    }
}
