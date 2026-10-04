/*
 * RE1.5 Rebuilt — Zombie-Abstand zur Eintrittstuer ROOM1010 / ROOM1220 (Runde 35 Spur F, Punkt 3).
 * Herleitung, Regel und Messungen: include/re15_zombie_abstand.h und
 * analysis/befunde_runde35/F_inhalt.md (Punkt 3).
 */
#include "re15_zombie_abstand.h"

#include <stddef.h>

typedef struct {
    uint16_t raum;          /* Raum-Basis (raum & 0xFFFE: Leon-/Elza-Variante gleich)              */
    uint8_t  type;          /* Sce_em_set pc[2]                                                    */
    int16_t  x0, z0;        /* ORIGINAL pc[8..9] / pc[12..13] = Schluessel + Satz-Waechter         */
    int16_t  x1, z1;        /* PORT-WAHL nach NUTZER-VORGABE, Regel s.u.                           */
} abstand_t;

/* DIE TABELLE — Ergebnis EINER Regel (Dossier F_inhalt.md Punkt 3, Werkzeug
 * `test_r35_inhalt_zombies suche`), keine Handwerte:
 *   Verletzer = ein Zombie, der bei der schnellstmoeglichen Flucht naeher als die Sprungschwelle
 *     0xBB8 = 3000 an den Spieler kommt (RE2-Zombie DECISION[0] `sltiu 0xbb8` @0x80101374 und
 *     DECISION[2] @0x801020A8: unter 3000 + Blickkegel + Sicht -> Sprung-Biss 0x0C01, EXEC[12]
 *     @0x80104748 mit Schub-Clip 0x1B). Flucht = sofort auf der Stelle drehen (96/Bild,
 *     TURN-IN-PLACE @0x80073ee4 = [0,96]) und VIERECK an der Eintrittstuer.
 *   Neue Lage = die KLEINSTE Versetzung (Ringe 100, 200, ... um die Original-Lage, 64 Richtungen),
 *     bei der der Zombie waehrend der Flucht >= 3000 bleibt und die Flucht "raus" endet; auf dem
 *     Ring gewinnt der Ort mit dem groessten Abstand zum Eintritt ("weiter zurueck"). Nur Orte mit
 *     freiem Weg von der Original-Lage (Gegner-Wandklemme, Band 0, Maske 4) und freier
 *     Grundflaeche (FUN_8003b558-Port re15_collision_box_blocked, Radius hit_radius_min).
 *     Zwei Durchgaenge: erst je Zombie (die anderen Verletzer ausgeblendet), dann GEMEINSAM bis
 *     alle Zombies des Eintritts die Flucht ueber >= 3000 bleiben (1010 Cut 0 / 1220 Cut 2: je
 *     ein Zombie im 2. Durchgang 100 weiter).
 * Eintritte (Ziel des Tuer-Satzes im Nachbarraum): 1010 Cut 0 = ROOM1020 @0x01CA2 (3650,6900);
 * 1220 Cut 0/2/6/8 = ROOM1210 @0x01CE6 / @0x01D06 / @0x01D46 / @0x01D66.
 * Satz-Fundstellen (ROOM1010.RDT sub00 / ROOM1220.RDT sub00; 1011/1221 dieselben Werte, 1011 mit
 * um 1 verschobener Slot-Nummer, deshalb Schluessel = Typ + Original-Lage):
 *   @0x009E2 `44 00 10 00 .. a6 0e 00 00 88 13`  (3750,5000)    Versatz 1500 -> (3750,3500)
 *   @0x009F6 `44 01 10 00 .. b0 04 00 00 ee 1b`  (1200,7150)    Versatz 1000 -> (205,7248)
 *   @0x00F2A `44 00 16 81 .. 58 9e 00 00 d4 e5`  (-25000,-6700) Versatz  400 -> (-25398,-6739)
 *   @0x00F5A `44 02 16 02 .. 16 c2 00 00 a4 d4`  (-15850,-11100) Versatz 2600 -> (-13362,-10345)
 *   @0x00F6E `44 03 16 02 .. cc c5 00 00 8e e0`  (-14900,-8050) Versatz  900 -> (-14264,-7414)
 *   @0x00FBA `44 06 16 02 .. d8 c3 00 00 74 c3`  (-15400,-15500) Versatz 700 -> (-15070,-14883)
 *   @0x00FFE `44 09 16 02 .. f2 c7 00 00 60 a5`  (-14350,-23200) Versatz 100 -> (-14267,-23144)
 * Unveraendert (Flucht-Mindestabstand schon >= 3000): 1010 Cut 4 (Kriecher), 1220 Cut 0 Satz 1,
 * Cut 4 beide, Cut 6 Satz 7, Cut 8 Satz 8. */
static const abstand_t k_abstand[] = {
    { 0x1010, 0x10,   3750,   5000,   3750,   3500 },
    { 0x1010, 0x10,   1200,   7150,    205,   7248 },
    { 0x1220, 0x16, -25000,  -6700, -25398,  -6739 },
    { 0x1220, 0x16, -15850, -11100, -13362, -10345 },
    { 0x1220, 0x16, -14900,  -8050, -14264,  -7414 },
    { 0x1220, 0x16, -15400, -15500, -15070, -14883 },
    { 0x1220, 0x16, -14350, -23200, -14267, -23144 },
};
#define N_ABSTAND ((int)(sizeof k_abstand / sizeof k_abstand[0]))

static int s_aktiv = 1;

void re15_zombie_abstand_set_aktiv(int on) { s_aktiv = on ? 1 : 0; }

int re15_zombie_abstand_anzahl(void) { return N_ABSTAND; }

int re15_zombie_abstand_eintrag(int i, uint16_t *raum, uint8_t *slot, uint8_t *type,
                                int16_t *x0, int16_t *z0, int16_t *x1, int16_t *z1)
{
    if (i < 0 || i >= N_ABSTAND) return 0;
    const abstand_t *a = &k_abstand[i];
    if (raum) *raum = a->raum;
    if (slot) *slot = 0xFF;          /* Schluessel ist Typ + Original-Lage, nicht der Slot */
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
    (void)slot;
    if (!s_aktiv || !x || !z) return 0;
    const uint16_t basis = (uint16_t)(room_id & 0xFFFEu);
    for (int i = 0; i < N_ABSTAND; i++) {
        const abstand_t *a = &k_abstand[i];
        if (a->raum != basis || a->type != type) continue;
        if (*x != a->x0 || *z != a->z0) continue;          /* Satz-Waechter: nur der Original-Satz */
        *x = a->x1;
        *z = a->z1;
        return 1;
    }
    return 0;
}
