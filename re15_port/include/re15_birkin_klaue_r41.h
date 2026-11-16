/* re15_birkin_klaue_r41.h — Runde 41 Kralle "synchron": die GEZEICHNETE Klaue der Schwanz-Gestalt gegen SICHTBARE Waende.
 *
 * NUTZERENTSCHEID 16.11.2026 (woertlich): "Das mit dem zurueckziehen wuerde nur sinn machen, wenn er gegen eine Wand schlaegt
 * und deshalb die Hand dann zurueck faehrt. Das muesste dann auch synchron dazu passieren. Bei allen anderen Faellen macht das
 * eigentlich keinen Sinn."
 *
 * Gilt NUR, wenn fuer den Typ 0x30 ein Ersatzmodell gezeichnet wird (re15_bk_kralle_logik_aktiv(): Labor 50E0/50F1, Hangar
 * 50A0/50A1, Modell EM30_HANGAR.EMD). Der regulaere Kampf mit dem Capcom-Modell (3070/3071) behaelt die Klauen-Sonde
 * @0x80117814-858 unveraendert.
 *
 * Bausteine (engine/src/birkin_klaue_r41.c):
 *   - Lage von Teil 15 so, wie der PC-Zeichner sie im selben Takt zeichnet (platform/pc/main.c Gegner-Zeichner: Pose des
 *     aktuellen Bilds +0x95 mit Ueberblendung +0x8f aus dem Zeichen-Schnappschuss, Gier-Matrix, Massstab +0x166).
 *   - Mesh 15 der gezeichneten Bank (die Klinge) in Welt; Pruefpunkte = Ecken + Kantenpunkte.
 *   - Sichtbare Waende des Raums: SCA-Zellen (Band 0, Rechteck, solide), je Zelle eingeordnet nach den Messbildern
 *     (Zellen auf dem Hintergrund der Kampf-Cuts), mit Oberkante, falls die Zelle im Bild ein niedriges Hindernis ist.
 */
#ifndef RE15_BIRKIN_KLAUE_R41_H
#define RE15_BIRKIN_KLAUE_R41_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_md1.h"
#include "re15_rdt.h"

/* Welt-Lage eines Teils: welt = R * lokal >> 12 + T (wie nyawed_rot / nbone_world_trans im Zeichner). */
typedef struct {
    int32_t R[9];
    int32_t T[3];
} re15_bkk_lage_t;

/* Ergebnis der Pruefung Klinge gegen sichtbare Waende. */
typedef struct {
    int     kontakt;      /* 1 = mindestens ein Pruefpunkt der Klinge steckt in einer sichtbaren Wand */
    int     zelle;        /* SCA-Index der Wand mit dem tiefsten Punkt (bzw. der naechsten Wand ohne Kontakt), -1 = keine */
    int32_t abstand;      /* < 0: Eindringtiefe (waagerecht bis zur Wandflaeche), >= 0: kleinster Abstand Klinge - Wand */
    int32_t punkt[3];     /* der Pruefpunkt dazu (Welt) */
    int     punkte;       /* Zahl der Pruefpunkte */
} re15_bkk_befund_t;

/* Lage von Teil 15 der gezeichneten 0x30-Bank fuer den aktuellen Zustand des Aktors (Bild +0x95, Ueberblendung +0x8f aus dem
 * Zeichen-Schnappschuss) - ohne den Schnappschuss des Aktors zu veraendern. 0 = keine Bank / kein Teil 15. */
int re15_bkk_lage_logik(const re15_actor_t *e, re15_bkk_lage_t *out);

/* Mesh 15 der gezeichneten 0x30-Bank (NULL = keins). */
const re15_md1_mesh_t *re15_bkk_klinge(void);

/* Klinge (Mesh m in Lage l) gegen die sichtbaren Waende des Raums raum_id (SCA aus rdt). */
int re15_bkk_pruefen(const re15_md1_mesh_t *m, const re15_bkk_lage_t *l, const re15_rdt_t *rdt, uint16_t raum_id,
                     re15_bkk_befund_t *b);

/* 1 = die Klinge steckt in der Pose, die der Zeichner in diesem Takt zeigt, in einer sichtbaren Wand des aktuellen Raums. */
int re15_bkk_kontakt(const re15_actor_t *e);

/* Bilder von Clip 3, in denen ein Klingenkontakt den Rueckzug ausloest (Schlagfenster der gezeichneten Klaue, s. .c). */
#define RE15_BKK_FENSTER_AB  0x22
#define RE15_BKK_FENSTER_BIS 0x2b

/* Messhaken des PC-Zeichners: tatsaechlich gezeichnete Lage von Teil 15 des Typs 0x30 (RE15_BIRKIN_DBG -> birkin_dbg.log). */
void re15_bkk_zeichner_melden(const re15_actor_t *e, const int32_t R[9], const int32_t T[3]);
/* Messhaken der Logik: Lage nach re15_bkk_lage_logik + Befund (RE15_BIRKIN_DBG). */
void re15_bkk_logik_melden(const re15_actor_t *e);

#endif
