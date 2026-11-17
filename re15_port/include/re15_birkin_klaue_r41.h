/* re15_birkin_klaue_r41.h — Runde 41 Kralle "synchron": die GEZEICHNETE Klaue der Schwanz-Gestalt gegen die Waende des Raums.
 *
 * NUTZERENTSCHEID 16.11.2026 (woertlich): "Das mit dem zurueckziehen wuerde nur sinn machen, wenn er gegen eine Wand schlaegt
 * und deshalb die Hand dann zurueck faehrt. Das muesste dann auch synchron dazu passieren. Bei allen anderen Faellen macht das
 * eigentlich keinen Sinn."
 *
 * Runde 3 (Orchestrator-Auslegung): massgeblich ist der PHYSISCHE Wandschlag in Weltgeometrie, nicht ob die Kamera ihn zeigt;
 * keine Bild-, Verdeckungs- oder Masken-Rechnung.
 *
 * Gilt NUR, wenn fuer den Typ 0x30 ein Ersatzmodell gezeichnet wird (re15_bk_kralle_logik_aktiv(): Labor 50E0/50F1, Hangar
 * 50A0/50A1, Modell EM30_HANGAR.EMD). Der regulaere Kampf mit dem Capcom-Modell (3070/3071) behaelt die Klauen-Sonde
 * @0x80117814-858 unveraendert.
 *
 * Bausteine (engine/src/birkin_klaue_r41.c):
 *   - Lage von Teil 15 so, wie der PC-Zeichner sie im selben Takt zeichnet (platform/pc/main.c Gegner-Zeichner: Pose des
 *     aktuellen Bilds +0x95 mit Ueberblendung +0x8f aus dem Zeichen-Schnappschuss, Gier-Matrix, Massstab +0x166).
 *   - Mesh 15 der gezeichneten Bank (die Klinge) in Welt; Pruefpunkte = Ecken + Kantenpunkte.
 *   - Waende des Raums: SCA-Zellen (Band 0, Rechteck/Kreis, solide), je Zelle eingeordnet nach den Messbildern
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

/* Ergebnis der Pruefung Klinge gegen die Waende des Raums. */
typedef struct {
    int     kontakt;      /* 1 = mindestens ein Pruefpunkt der Klinge steckt in einer Wand (unter der Oberkante eines Hindernisses) */
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

/* Klinge (Mesh m in Lage l) gegen die Waende des Raums raum_id (SCA aus rdt). Kontakt = ein Pruefpunkt liegt in einer Zelle (unter
 * der Oberkante, wenn sie ein Hindernis ist); der Befund traegt den tiefsten Punkt ueber alle Zellen. Runde 3: keine Kamera. */
int re15_bkk_pruefen(const re15_md1_mesh_t *m, const re15_bkk_lage_t *l, const re15_rdt_t *rdt, uint16_t raum_id, re15_bkk_befund_t *b);

/* Art einer SCA-Zelle in der Wand-Tabelle: 0 = nicht erfasst, 1 = Wand bis zur Decke, 2 = niedriges Hindernis
 * (Gelaender, Oberkante gemessen). Riegel schwanz_abdeckung: in 50E0/50F1/50A0/50A1 ist jede Zelle erfasst. */
int re15_bkk_zelle_art(uint16_t raum_id, const re15_sca_entry_t *c);

/* 1 = die Klinge steckt in der Pose, die der Zeichner in diesem Takt zeigt, in einer Wand des aktuellen Raums (Welt). */
int re15_bkk_kontakt(const re15_actor_t *e);

/* Schlagfenster der gezeichneten Klaue = JEDES Bild der Phase 1 von Clip 3 (PORT-WAHL nach dem NUTZERENTSCHEID 16.11.2026):
 * gemessen (Runde 1 "synchron", Merge-Commit e7858c897) steckt die gezeichnete Klinge nicht nur im Capcom-Fenster 0x24..0x2b
 * (@0x801176ec-f0) in Waenden/Gelaendern, sondern schon beim Ausholen (af 21..35, Ueberblendung +0x8f = 0xf @0x80117600-08)
 * und beim Absenken nach dem Schlag (af 51..68). Jede dieser Lagen ist ein Durchdringen; deshalb prueft die Kralle
 * in jedem gezeichneten Bild der Phase 1.
 *
 * NUTZERENTSCHEID 17.11.2026 auf "Soll der Rueckzug auch zaehlen, wenn die Klaue schon beim Ausholen die Wand beruehrt?": "Nein".
 * Ausholen = die Bilder von Clip 3 VOR dem ersten Bild, in dem die Klinge nach vorn/unten schlaegt. Gemessen an der gezeichneten
 * Bank (EM30_HANGAR.EMD, EDD dir[1] @Datei 0x1274, Clip 3 = Kopf @0x1280 `51 00 44 02`: 81 Bilder ab Eintrag 124; reine Pose der
 * Klingenspitze, Mesh 15, ausholen/tools/rein.py) und an den zeichner-Zeilen der Messlaeufe (ausholen/tools/bahn.py):
 *   af 0..30  Klaue hebt sich und zieht nach hinten: Hoehe 1078 -> 5070 (Scheitel af 30), vorn -1600 .. -813 (af 30), -863 (af 31);
 *   af 31     Scheitel/ruhend (Eintrag @Datei 0x1534 `94 00 00 00` = Keyframe 148): d = (-50, +47, -17);
 *   af 32     ERSTES Bild nach vorn UND unten (Eintrag @Datei 0x1538 `95 00 00 00` = Keyframe 149): d vorn +458, Hoehe -175;
 *   af 33..35 weiter nach vorn/unten (+833/-461, +295/-350, +653/-1179), af 36 der Querschwung (+2376 quer).
 * Dasselbe Bild im Capcom-EM30 (CDEMD0.EMS, Clip 3 ebenfalls ab Eintrag 124, Keyframe 148 -> 149): af 31 -> 32 vorn +356,
 * Hoehe -136, Scheitel af 30. Das Tor der Kralle laesst ab +0x95 >= 0x1e nicht mehr abbrechen (`lbu v0,149(v1)` @0x801176a4,
 * `sltiu v0,v0,0x1e` @0x801176ac `1e 00 42 2c`; STAGE5 @0x80117ec0) - der Hieb ist ab dem Scheitel festgelegt. Das Schaden-
 * fenster 0x24..0x2b (`addiu v0,v0,-36` / `sltiu v0,v0,0x8` @0x801176ec-f0 `dc ff 42 24` / `08 00 42 2c`) ist die Trefferzeit
 * der Angriffsbox (Keyframe N-2), nicht der Beginn der Schlagbewegung: af 32..35 bringen die Klinge schon 2165 nach unten (5053 -> 2888).
 * Deshalb zaehlt ein Wandkontakt ab af 0x20 (Schlag + Absenken, einschliesslich des Ausstiegsbilds am Clipende, das das letzte
 * Bild haelt); davor (Ausholen, auch Abbruch-/Ausstiegsbild nach dem Tor, das nur bei +0x95 < 0x1e abbricht) loest er keinen
 * Rueckzug aus. PORT-WAHL nach dem Entscheid, nur Schwanz-Gestalt. */
#define RE15_BKK_SCHLAG_AB 0x20   /* erstes Schlagbild: EM30_HANGAR.EMD @Datei 0x1538 (Clip 3 Bild 32 = Keyframe 149) */

/* Ueberblendung in die Erholung (Clip 0xa): Runde 3 - die PORT-WAHL +0x8f = 8 (Runde 1, hergeleitet aus der Port-Rate 0x200) ist
 * entfernt. Die Schwanz-Gestalt blendet wie das Original: +0x8f = 3 (`sb v0,143(v1)` @0x80117900, `8f 00 62 a0`), a3 = 0x400
 * (`ori a3,zero,0x400` @0x8011791c, `00 04 07 34`; STAGE5 @0x80118130), gezeichnet mit +0x8f VOR dem Abzug (@0x8001f408 /
 * `mult v0,a3` @0x8001f474, Abzug @0x8001f5b0-5b4) -> erstes 0xa-Bild 3 * 0x400 = 3072 = 3/4 Kontaktlage, dann 2048, 1024, 0
 * (enemy_ai_common.c re15_birkin_anim_kampf). */

/* Messhaken des PC-Zeichners: tatsaechlich gezeichnete Lage von Teil 15 des Typs 0x30 (RE15_BIRKIN_DBG -> birkin_dbg.log). */
void re15_bkk_zeichner_melden(const re15_actor_t *e, const int32_t R[9], const int32_t T[3]);
/* Messhaken der Logik: Lage nach re15_bkk_lage_logik + Befund (RE15_BIRKIN_DBG). */
void re15_bkk_logik_melden(const re15_actor_t *e);

#endif
