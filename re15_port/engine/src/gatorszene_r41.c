/*
 * RE1.5 Rebuilt — Runde 41 Spur F "gatorszene" (ROOM2090/2091). Herleitung, Belege und Konstanten:
 * include/re15_gatorszene_r41.h, Dossier analysis/befunde_runde41/F_gatorszene.md.
 */
#include "re15_gatorszene_r41.h"

#include <stddef.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* ==== 1. Spinnen ================================================================================================ */
static int s_spinnen_weg = 1;
static int s_spinnen_verworfen = 0;

int re15_gatorszene_r41_spinnen_weg_setzen(int an)
{
    int alt = s_spinnen_weg;
    s_spinnen_weg = an ? 1 : 0;
    return alt;
}

int re15_gatorszene_r41_spinnen_verworfen(void) { return s_spinnen_verworfen; }

int re15_gatorszene_r41_spawn_weg(uint16_t room_id, uint8_t type, int16_t x, int16_t z)
{
    if (!s_spinnen_weg) return 0;
    if ((uint16_t)(room_id & 0xFFFEu) != RE15_GSZ_RAUM || type != RE15_GSZ_SPINNE_TYP) return 0;
    if (!((x == RE15_GSZ_SPINNE0_X && z == RE15_GSZ_SPINNE0_Z) ||
          (x == RE15_GSZ_SPINNE1_X && z == RE15_GSZ_SPINNE1_Z))) return 0;      /* Satz-Waechter: nur die RDT-Saetze */
    s_spinnen_verworfen++;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] ROOM%04X: Spinne 0x25 bei (%d,%d) verworfen (Nutzer: \"Entferne die Spinnen aus dem "
                    "ROOM 2090\"; Form = Kill-Flag-Tor @0x80042120-38)\n", (unsigned)room_id, (int)x, (int)z);
#endif
    return 1;
}
