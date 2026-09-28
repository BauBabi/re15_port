/* door_seq_tor1170.c — Zuordnung Tuer -> Tuersequenz und das eingebackene Torarchiv.
 *
 * ⛔ RE2-ERGAENZUNG (Beta -> Retail), Begruendung im Kopf von include/re15_door_seq.h.
 *
 * Das Gelaendertor am Hubschrauberlandeplatz ROOM1170/1171 ist eine Selbst-Tuer zwischen zwei
 * Bereichen desselben Raums (analysis/tor_1170/05_port_anschluss.md 1.4):
 *
 *   Raum  Slot  Datei    Rechteck (x,z,w,d)          Band  Ziel
 *   1170  0     @0x1206  (1500,14400,2100,1700)      4     Cut 11 (Laufsteg)   -> Variante 0
 *   1170  6     @0x135A  (-11940,-28450,1750,1200)   4     Cut 0  (Landeplatz) -> Variante 1
 *   1171  0     @0x11DC  wie 1170 Slot 0             4                          -> Variante 0
 *   1171  5     @0x12F0  wie 1170 Slot 6             4                          -> Variante 1
 *
 * Variante 0 = DOOR2E Skript 1: Blick von der Landeplatz-Seite, Angel rechts, aufdruecken.
 * Variante 1 = DOOR2E Skript 2: Blick von der Laufsteg-Seite, Angel links, aufziehen.
 * Das passt zur Lage des Modells: Drehung 0 zeigt die Landeplatz-Seite mit der Angel rechts
 * wie Cut 0 (analysis/tor_1170/07_modell.md 0.), Drehung 2048 die Laufsteg-Seite wie Cut 12.
 *
 * Der Port fuehrt Rechtecke als Mitte + Halbmass (x + w/2, z + d/2, w/2, d/2).
 */
#include "re15_door_seq.h"

#include <stddef.h>

#include "gen/tor_1170_door.inc"   /* re15_tor1170_door[] */

re15_door_seq_anfrage_t g_door_seq_anfrage;
static re15_door_seq_laeufer_t s_laeufer;

typedef struct {
    uint16_t raum;
    int32_t  x, z, hw, hh;
    uint8_t  band;
    uint8_t  archiv;
    uint8_t  variante;
} tuer_eintrag_t;

static const tuer_eintrag_t s_tabelle[] = {
    /* ROOM1170 Slot 0 @0x1206 Rechteck (1500,14400,2100,1700) */
    { 0x1170,   2550,  15250, 1050, 850, 4, RE15_DOOR_ARCHIV_TOR1170, 0 },
    /* ROOM1170 Slot 6 @0x135A Rechteck (-11940,-28450,1750,1200) */
    { 0x1170, -11065, -27850,  875, 600, 4, RE15_DOOR_ARCHIV_TOR1170, 1 },
    /* ROOM1171 Slot 0 @0x11DC / Slot 5 @0x12F0: dieselben Saetze (Elza-Variante) */
    { 0x1171,   2550,  15250, 1050, 850, 4, RE15_DOOR_ARCHIV_TOR1170, 0 },
    { 0x1171, -11065, -27850,  875, 600, 4, RE15_DOOR_ARCHIV_TOR1170, 1 },
};

int re15_door_seq_zuordnen(unsigned room_id, int32_t x, int32_t z, int32_t half_w, int32_t half_h,
                           int band, int *variante)
{
    for (size_t i = 0; i < sizeof s_tabelle / sizeof s_tabelle[0]; i++) {
        const tuer_eintrag_t *t = &s_tabelle[i];
        if (t->raum == room_id && t->x == x && t->z == z && t->hw == half_w && t->hh == half_h
            && t->band == band) {
            if (variante) *variante = t->variante;
            return t->archiv;
        }
    }
    return RE15_DOOR_ARCHIV_KEINS;
}

const uint8_t *re15_door_seq_archiv(int archiv, int *groesse)
{
    if (archiv == RE15_DOOR_ARCHIV_TOR1170) {
        if (groesse) *groesse = (int)sizeof re15_tor1170_door;
        return re15_tor1170_door;
    }
    if (groesse) *groesse = 0;
    return NULL;
}

void re15_door_seq_setze_laeufer(re15_door_seq_laeufer_t f)
{
    s_laeufer = f;
}

int re15_door_seq_ausfuehren(void)
{
    if (!g_door_seq_anfrage.aktiv) return 0;
    re15_door_seq_anfrage_t a = g_door_seq_anfrage;
    g_door_seq_anfrage.aktiv = 0;          /* verfaellt auch ohne Laeufer */
    if (!s_laeufer) return 0;
    s_laeufer(&a);
    return 1;
}
