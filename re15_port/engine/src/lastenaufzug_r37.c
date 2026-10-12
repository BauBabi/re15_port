/* lastenaufzug_r37.c — Runde 37 Spur C Nachbesserung 1 (Nachtrag 5a): Tuersequenz beim Aussteigen aus dem
 * Lastenaufzug ROOM3080/3081 "WAREHOUSE LIFT" -> ROOM4000/4001. Belege: include/re15_lastenaufzug_r37.h,
 * analysis/befunde_runde37/C_fahrstuhl.md "Nachbesserung 1".
 *
 * Wie die Kabinen-Ausstiege (fahrstuhl_r37.c): der Satz ist ein Null-Rechteck, den das Fahrt-Skript per Aot_on
 * feuert, waehrend (2,7)/(1,0x1B) gesetzt sind (player_mode 2) - die Zwischensequenz-Sperre der Runde 31
 * (aot_common.c tuer_sequenz_anfragen) gilt hier nicht, die Anfrage kommt davor (eigener Haken).
 */
#include "re15_lastenaufzug_r37.h"

#include <stddef.h>
#include <string.h>

#define LIFT_RAUM        0x3080u   /* ROOM3080 (Leon) / ROOM3081 (Elza), Raum ohne Spielervariante */
#define LIFT_ZIEL_STAGE  3         /* Door_aot_set @0x0072A +22 = 03 (STAGE4) */
#define LIFT_ZIEL_RAUM   0x00      /* +23 = 00 (ROOM4000) */
#define LIFT_SATZ_OFF    0x0072Au  /* Datei-Offset des Door_aot_set (ROOM3080 = ROOM3081) */
#define LIFT_ARCHIV      0x2D      /* RE2 ROOM6170 @0x035A2 Byte 26 = 0x2D (DOOR2D Hubbuehne) */
#define LIFT_VARIANTE    5         /* Byte 27 = 0x05: Skript 0 @0x601c Case 5 -> Gosub 2 = hinab */
#define LIFT_KENNUNG     "P2DS"    /* Port-Archiv der Gegenrichtung S218 (Runde 33 G10, gen/re15_tuer_eigen.inc) */
#define LIFT_TUER        112       /* Runde 31 T112 (Lastenaufzug-Schacht), nur Protokoll */

void re15_lastenaufzug_r37_satz(uint16_t *raum, uint8_t *ziel_stage, uint8_t *ziel_raum, uint32_t *off,
                                const char **kennung, uint8_t *basis, uint8_t *variante)
{
    if (raum) *raum = (uint16_t)LIFT_RAUM;
    if (ziel_stage) *ziel_stage = LIFT_ZIEL_STAGE;
    if (ziel_raum) *ziel_raum = LIFT_ZIEL_RAUM;
    if (off) *off = LIFT_SATZ_OFF;
    if (kennung) *kennung = LIFT_KENNUNG;
    if (basis) *basis = LIFT_ARCHIV;
    if (variante) *variante = LIFT_VARIANTE;
}

int re15_lastenaufzug_r37_ausstieg(unsigned room_id, const re15_aot_t *a, const re15_aot_door_params_t *d,
                                   re15_door_seq_anfrage_t *out)
{
#ifdef RE15_PLATFORM_PSX
    /* wie door_seq_zuordnung.c / fahrstuhl_r37.c: auf der PSX laeuft die Szene nicht (kein Laeufer) */
    (void)room_id; (void)a; (void)d; (void)out;
    return 0;
#else
    if (!a || !d || !out) return 0;
    if ((room_id & 0xFFF0u) != LIFT_RAUM || (room_id & 0x000Fu) > 1u) return 0;
    if (a->has_quad || a->half_w != 0 || a->half_h != 0) return 0;            /* nur das Null-Rechteck */
    if (d->dest_stage != LIFT_ZIEL_STAGE || d->dest_room != LIFT_ZIEL_RAUM) return 0;
    int eig = 0;
    for (int i = 1; i <= re15_door_seq_eigen_anzahl(); i++) {
        const re15_tuer_eigen_t *e = re15_door_seq_eigen(i);
        if (e && e->basis == LIFT_ARCHIV && strcmp(e->kennung, LIFT_KENNUNG) == 0) { eig = i; break; }
    }
    if (eig == 0) return 0;                  /* Port-Archiv fehlt in der Tabelle -> RE1.5-Uebergang */
    memset(out, 0, sizeof *out);
    out->aktiv    = 1;
    out->archiv   = RE15_DOOR_ARCHIV_RE2;
    out->re2_nr   = LIFT_ARCHIV;
    out->tuer_nr  = LIFT_ARCHIV;             /* var 15 = Payload+12 = Archivnummer (door_seq_zuordnung.c) */
    out->variante = LIFT_VARIANTE;
    out->bit7     = 0;
    out->spender  = RE15_DOOR_KEIN_SPENDER;
    out->seite    = 0;                       /* Runde 31 fuehrt fuer die Skript-Ankunft keine Seite */
    out->tuer     = LIFT_TUER;
    out->eigen    = (uint8_t)eig;
    return 1;
#endif
}
