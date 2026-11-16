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
 *   - Sichtbare Waende des Raums: SCA-Zellen (Band 0, Rechteck/Kreis, solide), je Zelle eingeordnet nach den Messbildern
 *     (Zellen auf dem Hintergrund der Kampf-Cuts), mit Oberkante, falls die Zelle im Bild ein niedriges Hindernis ist
 *     (Gelaender der Labor-Laufstege, Oberkante 1900 gemessen, s. .c).
 *   - Kontakt = mindestens ein Pruefpunkt der Klinge (Ecken von Mesh 15 + Kantenpunkte <= 64 auseinander) liegt in der Zelle
 *     unterhalb ihrer Oberkante. Kein Zuschlag: der Abstand ist die Klingengeometrie selbst.
 */
#ifndef RE15_BIRKIN_KLAUE_R41_H
#define RE15_BIRKIN_KLAUE_R41_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_md1.h"
#include "re15_rdt.h"
#include "re15_camera.h"

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
    int     ausser_bild;  /* Zellen, in denen die Klinge steckt, deren Einschlagstelle (tiefster Punkt) aber ausserhalb des Bilds liegt */
} re15_bkk_befund_t;

/* Lage von Teil 15 der gezeichneten 0x30-Bank fuer den aktuellen Zustand des Aktors (Bild +0x95, Ueberblendung +0x8f aus dem
 * Zeichen-Schnappschuss) - ohne den Schnappschuss des Aktors zu veraendern. 0 = keine Bank / kein Teil 15. */
int re15_bkk_lage_logik(const re15_actor_t *e, re15_bkk_lage_t *out);

/* Mesh 15 der gezeichneten 0x30-Bank (NULL = keins). */
const re15_md1_mesh_t *re15_bkk_klinge(void);

/* Klinge (Mesh m in Lage l) gegen die sichtbaren Waende des Raums raum_id (SCA aus rdt). sicht != NULL: eine Zelle zaehlt nur,
 * wenn ihre Einschlagstelle (der tiefste Punkt der Klinge in ihr) mit dieser Kamera im 320x240-Bild liegt (Runde 2: ein Treffer
 * ausserhalb des Bilds ist kein sichtbarer Treffer). */
int re15_bkk_pruefen(const re15_md1_mesh_t *m, const re15_bkk_lage_t *l, const re15_rdt_t *rdt, uint16_t raum_id,
                     const re15_camera_view_t *sicht, re15_bkk_befund_t *b);

/* Kamera des laufenden Cuts (g_scd.cam_id) - 0 = keine. */
int re15_bkk_sicht(re15_camera_view_t *v);

/* Art einer SCA-Zelle in der Tabelle sichtbarer Waende: 0 = nicht erfasst, 1 = Wand bis zur Decke, 2 = niedriges Hindernis
 * (Gelaender, Oberkante gemessen). Riegel schwanz_abdeckung: in 50E0/50F1/50A0/50A1 ist jede Zelle erfasst. */
int re15_bkk_zelle_art(uint16_t raum_id, const re15_sca_entry_t *c);

/* 1 = die Klinge steckt in der Pose, die der Zeichner in diesem Takt zeigt, in einer sichtbaren Wand des aktuellen Raums. */
int re15_bkk_kontakt(const re15_actor_t *e);

/* Schlagfenster der gezeichneten Klaue = JEDES Bild der Phase 1 von Clip 3 (PORT-WAHL nach dem NUTZERENTSCHEID 16.11.2026):
 * gemessen (Runde 1 "synchron", Merge-Commit e7858c897) steckt die gezeichnete Klinge nicht nur im Capcom-Fenster 0x24..0x2b
 * (@0x801176ec-f0) in Waenden/Gelaendern, sondern schon beim Ausholen (af 21..35, Ueberblendung +0x8f = 0xf @0x80117600-08)
 * und beim Absenken nach dem Schlag (af 51..68). Jede dieser Lagen ist ein sichtbares Durchdringen; deshalb prueft die Kralle
 * in jedem gezeichneten Bild der Phase 1. */

/* Ueberblendung in die Erholung (Clip 0xa) der Schwanz-Gestalt, PORT-WAHL nach dem NUTZERENTSCHEID ("synchron dazu"):
 * Bild 0 von Clip 0xa (Klaue links vorn, halbhoch) passt nur zu Kontakten beim Ausholen links (af 28..36). Gemessen auf dem
 * Merge-Commit-Stand mit Capcom-Wert +0x8f = 3 (@0x80117900): Spitzensprung Kontaktbild -> erstes 0xa-Bild 1092..4999,
 * Klinge im ersten 0xa-Bild bis 2977 von der Wand weg (Kontakte af 21, 40..55). Neu: +0x8f = 8. Herleitung: der Zeichner
 * mischt die alte Pose mit wp = +0x8f * 0x200 (Port-Rate, skeleton_common.c), gekappt bei 0x1000; 0x1000 / 0x200 = 8 ist die
 * laengste Ueberblendung, die die Rate erlaubt. Phase 4 zaehlt +0x8f vor dem Zeichnen um 1 herunter (re15_birkin_anim), das
 * erste gezeichnete 0xa-Bild zeigt also 7/8 Kontaktlage + 1/8 Clip 0xa: die Bewegung beginnt im naechsten Bild, die Klaue
 * liegt dabei noch an der Wand und gleitet ohne Sprung in den Clip. */
#define RE15_BKK_UEBERBLENDUNG 8

/* Messhaken des PC-Zeichners: tatsaechlich gezeichnete Lage von Teil 15 des Typs 0x30 (RE15_BIRKIN_DBG -> birkin_dbg.log). */
void re15_bkk_zeichner_melden(const re15_actor_t *e, const int32_t R[9], const int32_t T[3]);
/* Messhaken der Logik: Lage nach re15_bkk_lage_logik + Befund (RE15_BIRKIN_DBG). */
void re15_bkk_logik_melden(const re15_actor_t *e);

#endif
