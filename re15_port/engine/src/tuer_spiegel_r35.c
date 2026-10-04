/*
 * RE1.5 Rebuilt — symmetrische Griffe an den Port-Doppeltueren (Runde 35 Spur F, Punkt 1).
 * Herleitung und Messungen: include/re15_tuer_spiegel.h, analysis/befunde_runde35/F_inhalt.md.
 */
#include "re15_tuer_spiegel.h"

#include <string.h>

/* Port-Archive, deren Griffe am zweiten Fluegel ohne die Spiegeldrehung des Basis-Archivs
 * gezeichnet werden (Ursache 2). Gemessen mit probe_r35_inhalt_tueren: P0CD vorher 180 Grad,
 * mit dz 2048 (= rot0[2] 2048 aufgehoben) 0,0 Grad, vorn und hinten. */
typedef struct { const char *kennung; int fluegel; uint16_t dz; } spiegel_t;
static const spiegel_t k_spiegel[] = {
    { "P0CD", 1, 2048 },    /* DOOR0C V0/V1: obj 3 (0,0,2048) / obj 5 (0,2048,2048) am Fluegel obj 1 */
};

uint16_t re15_tuer_spiegel_dz(const char *kennung, int fluegel)
{
    if (!kennung) return 0;
    for (unsigned i = 0; i < sizeof k_spiegel / sizeof k_spiegel[0]; i++)
        if (k_spiegel[i].fluegel == fluegel && strcmp(k_spiegel[i].kennung, kennung) == 0)
            return k_spiegel[i].dz;
    return 0;
}

void re15_tuer_griff_tausch_rot(const uint16_t basis[3], int32_t ausschlag_x, uint16_t rot0_z,
                                uint16_t out[3])
{
    out[0] = (uint16_t)(basis[0] + ausschlag_x);
    out[1] = basis[1];
    out[2] = (uint16_t)(basis[2] + rot0_z);   /* Spiegel-Drehung des Archiv-Objekts (Ursache 1) */
}
