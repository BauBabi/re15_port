/* door_seq_archiv.c — die eingebackenen Tuerarchive der RE2-Tuersequenz.
 *
 * Eigene Uebersetzungseinheit, damit das 39-KB-Archiv nur dort gelinkt wird, wo es gelesen
 * wird (PC-Szene platform/pc/src/door_scene_pc.c, unit_door_seq). Der PSX-Bau referenziert
 * re15_door_seq_archiv nicht und zieht die Bytes deshalb nicht aus der Engine-Bibliothek.
 * Erzeugt von tools/tor/tor_sequenz_bauen.py (analysis/tor_1170/09_sequenz.md).
 */
#include "re15_door_seq.h"

#include <stddef.h>

#include "gen/tor_1170_door.inc"   /* re15_tor1170_door[] */

const uint8_t *re15_door_seq_archiv(int archiv, int *groesse)
{
    if (archiv == RE15_DOOR_ARCHIV_TOR1170) {
        if (groesse) *groesse = (int)sizeof re15_tor1170_door;
        return re15_tor1170_door;
    }
    if (groesse) *groesse = 0;
    return NULL;
}
