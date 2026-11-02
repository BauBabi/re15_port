/* re15_schwanz_r41.h — Runde 41 Spur SF Nachbesserung 1 (Dossier analysis/befunde_runde41/SF_sichel.md N1.9).
 *
 * Nutzer 2026-11-02 zur Hangar-Szene: "Der Schwanz ist viel zu starr". Die Sichel-Form (EM30_HANGAR.EMD,
 * tools/birkin_formen.py) traegt den Schwanz als Knochenkette 16-22 an Knochen 8; ihre Winkel stehen nur in den
 * Keyframes (Nachschwingen offline je Clip gerechnet). Im Spiel bewegt sich der Schwanz dadurch nur, wenn der Clip
 * es tut, und springt bei jedem Clip-, Bild- oder Gierwechsel mit dem Koerper mit. Das 80%-Video zeigt ihn dagegen
 * mit eigenem Schwung (Video 4, Hangar h18-h70: Bild-links bis zum Boden, durch die Knie, nach Bild-rechts).
 *
 * PORT-WAHL (Gestaltung, kein Original - die Form gibt es auf der Disc nicht, keine @0x-Adresse): eine Feder-Kette
 * im Weltraum. Die Keyframe-Gelenke sind das Ziel, jedes Gelenk hat Traegheit, Feder zum Ziel und feste Gliedlaenge,
 * die Wurzel haengt fest am Rumpf. Daraus werden die Knochen 16-22 neu gedreht (kleinste Drehung je Glied), alle
 * anderen Knochen bleiben unberuehrt. Gilt nur fuer Typ 0x30 mit genau 23 Knochen und Kette 8-16-17-..-22.
 *
 * Die Hangar-Szene kann den Schwanz seitlich vorgeben (re15_schwanz_r41_seite): Video h18-h28 haengt er auf Birkins
 * rechter Seite (Bild-links), danach gibt die Szene ihn frei und die Feder zieht ihn durch die Knie zurueck. */
#ifndef RE15_SCHWANZ_R41_H
#define RE15_SCHWANZ_R41_H
#include <stdint.h>
#include "re15_actor.h"
#include "re15_emd.h"
#include "re15_skeleton.h"

/* Zeichner (platform/pc/main.c), direkt nach re15_skel_compute_pose: Knochen 16-22 von `poses` nach der Feder-Kette
 * neu setzen. Schritt der Kette einmal je Spieltakt (g_engine.frame_count), Zeichnen dazwischen nutzt den Stand.
 * Rueckgabe 1 = Kette angewandt. */
int  re15_schwanz_r41_pose(const re15_actor_t *a, const re15_emd_skeleton_t *skel, re15_skel_pose_t *poses);

/* Szene: seitliche Vorgabe fuer Slot `slot` (Birkin-Raum z; + = rechte Seite, Einheiten am Schwanzende), 0 = frei. */
void re15_schwanz_r41_seite(int slot, int32_t seite);

/* Zuruecksetzen (Raumwechsel, Tests). */
void re15_schwanz_r41_reset(void);

/* Messzugang fuer Tests: Gelenk j (0 = Wurzel .. 7 = Spitze) der Kette von Slot `slot` in Weltkoordinaten. */
int  re15_schwanz_r41_gelenk(int slot, int j, int32_t out[3]);

#endif
