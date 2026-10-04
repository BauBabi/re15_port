/*
 * RE1.5 Rebuilt — Zombie-Abstand zur Eintrittstuer ROOM1010 / ROOM1220 (Runde 35 Spur F, Punkt 3).
 * Herleitung, Regel und Messungen: include/re15_zombie_abstand.h und
 * analysis/befunde_runde35/F_inhalt.md (Punkt 3).
 */
#include "re15_zombie_abstand.h"

#include <stddef.h>

typedef struct {
    uint16_t raum;          /* Raum-Basis (John/Leon-Variante; Elza = raum | 1 gleich behandelt) */
    uint8_t  slot, type;    /* Sce_em_set pc[1] / pc[2]                                           */
    int16_t  x0, z0;        /* ORIGINAL pc[8..9] / pc[12..13] (Satz-Waechter)                     */
    int16_t  x1, z1;        /* PORT-WAHL (NUTZER-VORGABE "weiter zurueck")                         */
} abstand_t;

static const abstand_t k_abstand[] = {
    { 0, 0, 0, 0, 0, 0, 0 },   /* Platzhalter-Ende, wird mit der Messung gefuellt */
};
#define N_ABSTAND ((int)(sizeof k_abstand / sizeof k_abstand[0]) - 1)

static int s_aktiv = 1;

void re15_zombie_abstand_set_aktiv(int on) { s_aktiv = on ? 1 : 0; }

int re15_zombie_abstand_anzahl(void) { return N_ABSTAND; }

int re15_zombie_abstand_eintrag(int i, uint16_t *raum, uint8_t *slot, uint8_t *type,
                                int16_t *x0, int16_t *z0, int16_t *x1, int16_t *z1)
{
    if (i < 0 || i >= N_ABSTAND) return 0;
    const abstand_t *a = &k_abstand[i];
    if (raum) *raum = a->raum;
    if (slot) *slot = a->slot;
    if (type) *type = a->type;
    if (x0) *x0 = a->x0;
    if (z0) *z0 = a->z0;
    if (x1) *x1 = a->x1;
    if (z1) *z1 = a->z1;
    return 1;
}

int re15_zombie_abstand_anwenden(uint16_t room_id, uint8_t slot, uint8_t type,
                                 int16_t *x, int16_t *z)
{
    if (!s_aktiv || !x || !z) return 0;
    const uint16_t basis = (uint16_t)(room_id & 0xFFFEu);
    for (int i = 0; i < N_ABSTAND; i++) {
        const abstand_t *a = &k_abstand[i];
        if (a->raum != basis || a->slot != slot || a->type != type) continue;
        if (*x != a->x0 || *z != a->z0) return 0;          /* Satz-Waechter: nur der Original-Satz */
        *x = a->x1;
        *z = a->z1;
        return 1;
    }
    return 0;
}
