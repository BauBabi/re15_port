/*
 * Runde 41 Spur M (masken36): Tiefe aus der Erreichbarkeit fuer Generator-Rechtecke der Stages 3..6.
 * Belege und Regel: include/re15_masken36_r41.h, Dossier analysis/befunde_runde41/M_masken36.md (Punkt 1).
 */
#include <stddef.h>
#include "re15_pri.h"
#include "re15_masken36_r41.h"

typedef struct {
    uint16_t raum;     /* Raum-Id der MSK-Datei */
    uint8_t  cut;
    uint8_t  idx;      /* Rechteck in der Reihenfolge von re15_pri_parse_section */
    uint16_t tiefe;    /* neue Tiefe = groesster Bucket eines Figurpunkts ohne Zelle davor unter dem Rechteck */
} re15_masken36_tiefe_zeile_t;

#include "gen/masken36_tiefe_r41.inc"

int re15_masken36_tiefe_r41_wert(unsigned masken_raum, int cut, int idx)
{
    size_t i;
    for (i = 0; i < sizeof re15_masken36_tiefe_zeilen / sizeof re15_masken36_tiefe_zeilen[0]; i++) {
        const re15_masken36_tiefe_zeile_t *z = &re15_masken36_tiefe_zeilen[i];
        if (z->raum == (uint16_t) masken_raum && z->cut == (uint8_t) cut && z->idx == (uint8_t) idx && z->raum != 0)
            return (int) z->tiefe;
    }
    return -1;
}

int re15_masken36_tiefe_r41(unsigned masken_raum, int cut, re15_pri_cut_t *pri)
{
    size_t i;
    int n = 0;
    if (!pri || cut < 0) return 0;
    for (i = 0; i < sizeof re15_masken36_tiefe_zeilen / sizeof re15_masken36_tiefe_zeilen[0]; i++) {
        const re15_masken36_tiefe_zeile_t *z = &re15_masken36_tiefe_zeilen[i];
        if (z->raum != (uint16_t) masken_raum || z->cut != (uint8_t) cut || z->raum == 0) continue;
        if ((int) z->idx < pri->mask_count) { pri->masks[z->idx].depth = z->tiefe; n++; }
    }
    return n;
}

int re15_masken36_tiefe_r41_zeilen(void)
{
    return (int) (sizeof re15_masken36_tiefe_zeilen / sizeof re15_masken36_tiefe_zeilen[0]);
}
