/*
 * RE1.5 Rebuilt — symmetrische Griffe an den Port-Doppeltueren (Runde 35 Spur F, Punkt 1).
 * Herleitung und Messungen: include/re15_tuer_spiegel.h, analysis/befunde_runde35/F_inhalt.md
 * (Punkt 1 und Nachbesserung 1, Mangel M3).
 */
#include "re15_tuer_spiegel.h"

#include <string.h>

/* Port-Archive, deren Griffe am zweiten Fluegel mit einer Zusatzdrehung gezeichnet werden
 * (Ursache 2). Nachbesserung 1: mit der KAMERASEITEN-Paarung gemessen (probe_r35_inhalt_tueren,
 * Variante "alt dy:2048 dz:2048"): P0CD V1 Spitzen-Winkel 146,1 -> 0,0 Grad, Bild-Fehler 0,30 px;
 * V0 0,91 / 0,83 px (Rest = Anhaengehoehen des RE2-Archivs DOOR0C: -3000/-3008 bzw. -2976/-3000).
 * Die erste Wahl dz 2048 allein (Runde-35-Stand vor der Nachbesserung) war an FALSCHEN Paaren
 * gemessen (Vorderseiten-Griff gegen Rueckseiten-Griff) und ergibt auf den echten Paaren 33,9 Grad
 * / 3,20 px. Fluegel 1 steht um 2048 um die Senkrechte gedreht (x-Achse (0,0,+4096) gegen
 * (0,0,-4096) an Fluegel 0) -> dy 2048 hebt diese Drehung fuer den Griff auf, dz 2048 die
 * z-Spiegeldrehung rot0[2] des Archivs. */
typedef struct { const char *kennung; int fluegel; uint16_t dy, dz; } spiegel_t;
static const spiegel_t k_spiegel[] = {
    { "P0CD", 1, 2048, 2048 },   /* DOOR0C V0/V1: obj 3 (0,0,2048) / obj 5 (0,2048,2048) an Fluegel obj 1 */
};

static const spiegel_t *suche(const char *kennung, int fluegel)
{
    if (!kennung) return NULL;
    for (unsigned i = 0; i < sizeof k_spiegel / sizeof k_spiegel[0]; i++)
        if (k_spiegel[i].fluegel == fluegel && strcmp(k_spiegel[i].kennung, kennung) == 0)
            return &k_spiegel[i];
    return NULL;
}

uint16_t re15_tuer_spiegel_dz(const char *kennung, int fluegel)
{
    const spiegel_t *s = suche(kennung, fluegel);
    return s ? s->dz : 0;
}

uint16_t re15_tuer_spiegel_dy(const char *kennung, int fluegel)
{
    const spiegel_t *s = suche(kennung, fluegel);
    return s ? s->dy : 0;
}

/* Spender, deren getauschter Griff die Spiegel-Drehung rot0[2] des Archiv-Objekts uebernimmt.
 * Gemessen (Nachbesserung 1, Kameraseiten-Paarung): DOOR07 (Druecker, an DOOR1D-Archiven
 * P1DG/P1DK/P1DL) braucht sie — V3 ohne: kein Griff des zweiten Fluegels auf der Kameraseite
 * (der Druecker steht nach hinten), mit: 0,1 Grad. DOOR23 (Riegelstange, an DOOR1B-Archiven
 * P1B3/P1BD) darf sie NICHT bekommen — ohne: Kameraseite 0,1 Grad / 0,14 px, mit: 37,1 Grad /
 * 3,27 px (die Stange des zweiten Fluegels stuende dann ins Blatt). */
static int tausch_mit_spiegel(int spender)
{
    return spender == 0x07;
}

void re15_tuer_griff_tausch_rot(const uint16_t basis[3], int32_t ausschlag_x, uint16_t rot0_z, int spender,
                                uint16_t out[3])
{
    out[0] = (uint16_t)(basis[0] + ausschlag_x);
    out[1] = basis[1];
    out[2] = (uint16_t)(basis[2] + (tausch_mit_spiegel(spender) ? rot0_z : 0));   /* Ursache 1 */
}

int32_t re15_tuer_griff_tausch_dx(int spender, int fluegel, int32_t pos0, uint16_t rot0_z)
{
    /* DOOR1D V3: obj 3 sitzt im Archiv an der RUECKSEITE des zweiten Fluegels (lokal x -130,
     * dessen x-Achse zeigt dort nach hinten: Welt-z 8130) und ragt mit der Spiegel-Drehung durch das
     * Blatt nach VORN (Huelle z 7918..8130) — so auch im RE2-Original S192. Die 260 Einheiten mehr
     * Tiefe als der Partner obj 2 (Vorderseite, z 7658..7870) ergeben in der Projektion (H 290
     * @0x80013e34) 1,1..1,3 px Hoehenunterschied = der Abnahme-Befund 157,0/156,0 px. Fuer den
     * getauschten Druecker sitzt der Anhaengepunkt deshalb auf der Vorderseite (pos[0] gespiegelt).
     * ⛔ PORT-WAHL zur NUTZER-VORGABE "Das ist so im allgemeinen nicht"; RE2-Archiv unveraendert. */
    if (tausch_mit_spiegel(spender) && fluegel == 1 && rot0_z == 2048 && pos0 < 0)
        return -2 * pos0;
    return 0;
}
